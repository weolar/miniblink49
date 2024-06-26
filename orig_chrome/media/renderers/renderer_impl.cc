// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/renderers/renderer_impl.h"

#include "base/bind.h"
#include "base/callback.h"
#include "base/callback_helpers.h"
#include "base/command_line.h"
#include "base/compiler_specific.h"
#include "base/location.h"
#include "base/single_thread_task_runner.h"
#include "base/strings/string_number_conversions.h"
#include "media/base/audio_renderer.h"
#include "media/base/bind_to_current_loop.h"
#include "media/base/demuxer_stream_provider.h"
#include "media/base/media_switches.h"
#include "media/base/time_source.h"
#include "media/base/video_renderer.h"
#include "media/base/wall_clock_time_source.h"

namespace media {

// See |video_underflow_threshold_|.
static const int kDefaultVideoUnderflowThresholdMs = 3000;

RendererImpl::RendererImpl(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    scoped_ptr<AudioRenderer> audio_renderer,
    scoped_ptr<VideoRenderer> video_renderer)
    : state_(STATE_UNINITIALIZED)
    , task_runner_(task_runner)
    , audio_renderer_(audio_renderer.Pass())
    , video_renderer_(video_renderer.Pass())
    , time_source_(NULL)
    , time_ticking_(false)
    , playback_rate_(0.0)
    , audio_buffering_state_(BUFFERING_HAVE_NOTHING)
    , video_buffering_state_(BUFFERING_HAVE_NOTHING)
    , audio_ended_(false)
    , video_ended_(false)
    , cdm_context_(nullptr)
    , underflow_disabled_for_testing_(false)
    , clockless_video_playback_enabled_for_testing_(false)
    , video_underflow_threshold_(
          base::TimeDelta::FromMilliseconds(kDefaultVideoUnderflowThresholdMs))
    , weak_factory_(this)
{
    weak_this_ = weak_factory_.GetWeakPtr();
    DVLOG(1) << __FUNCTION__;

    // TODO(dalecurtis): Remove once experiments for http://crbug.com/470940 are
    // complete.
    int threshold_ms = 0;
    std::string threshold_ms_str(
        base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
            switches::kVideoUnderflowThresholdMs));
    if (base::StringToInt(threshold_ms_str, &threshold_ms) && threshold_ms > 0) {
        video_underflow_threshold_ = base::TimeDelta::FromMilliseconds(threshold_ms);
    }
}

RendererImpl::~RendererImpl()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());

    // Tear down in opposite order of construction as |video_renderer_| can still
    // need |time_source_| (which can be |audio_renderer_|) to be alive.
    video_renderer_.reset();
    audio_renderer_.reset();

    if (!init_cb_.is_null())
        base::ResetAndReturn(&init_cb_).Run(PIPELINE_ERROR_ABORT);
    else if (!flush_cb_.is_null())
        base::ResetAndReturn(&flush_cb_).Run();
}

void RendererImpl::Initialize(
    DemuxerStreamProvider* demuxer_stream_provider,
    const PipelineStatusCB& init_cb,
    const StatisticsCB& statistics_cb,
    const BufferingStateCB& buffering_state_cb,
    const base::Closure& ended_cb,
    const PipelineStatusCB& error_cb,
    const base::Closure& waiting_for_decryption_key_cb)
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(state_, STATE_UNINITIALIZED);
    DCHECK(!init_cb.is_null());
    DCHECK(!statistics_cb.is_null());
    DCHECK(!buffering_state_cb.is_null());
    DCHECK(!ended_cb.is_null());
    DCHECK(!error_cb.is_null());
    DCHECK(demuxer_stream_provider->GetStream(DemuxerStream::AUDIO) || demuxer_stream_provider->GetStream(DemuxerStream::VIDEO));

    demuxer_stream_provider_ = demuxer_stream_provider;
    statistics_cb_ = statistics_cb;
    buffering_state_cb_ = buffering_state_cb;
    ended_cb_ = ended_cb;
    error_cb_ = error_cb;
    init_cb_ = init_cb;
    waiting_for_decryption_key_cb_ = waiting_for_decryption_key_cb;

    state_ = STATE_INITIALIZING;
    InitializeAudioRenderer();
}

void RendererImpl::SetCdm(CdmContext* cdm_context,
    const CdmAttachedCB& cdm_attached_cb)
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(cdm_context);

    if (cdm_context_) {
        DVLOG(1) << "Switching CDM not supported.";
        cdm_attached_cb.Run(false);
        return;
    }

    cdm_context_ = cdm_context;

    if (cdm_ready_cb_.is_null()) {
        cdm_attached_cb.Run(true);
        return;
    }

    base::ResetAndReturn(&cdm_ready_cb_).Run(cdm_context, cdm_attached_cb);
}

void RendererImpl::Flush(const base::Closure& flush_cb)
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(flush_cb_.is_null());

    if (state_ != STATE_PLAYING) {
        DCHECK_EQ(state_, STATE_ERROR);
        return;
    }

    flush_cb_ = flush_cb;
    state_ = STATE_FLUSHING;

    if (time_ticking_)
        PausePlayback();

    FlushAudioRenderer();
}

void RendererImpl::StartPlayingFrom(base::TimeDelta time)
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (state_ != STATE_PLAYING) {
        DCHECK_EQ(state_, STATE_ERROR);
        return;
    }

    time_source_->SetMediaTime(time);

    if (audio_renderer_)
        audio_renderer_->StartPlaying();
    if (video_renderer_)
        video_renderer_->StartPlayingFrom(time);
}

void RendererImpl::SetPlaybackRate(double playback_rate)
{
    DVLOG(1) << __FUNCTION__ << "(" << playback_rate << ")";
    DCHECK(task_runner_->BelongsToCurrentThread());

    // Playback rate changes are only carried out while playing.
    if (state_ != STATE_PLAYING)
        return;

    time_source_->SetPlaybackRate(playback_rate);

    const double old_rate = playback_rate_;
    playback_rate_ = playback_rate;
    if (!time_ticking_ || !video_renderer_)
        return;

    if (old_rate == 0 && playback_rate > 0)
        video_renderer_->OnTimeStateChanged(true);
    else if (old_rate > 0 && playback_rate == 0)
        video_renderer_->OnTimeStateChanged(false);
}

void RendererImpl::SetVolume(float volume)
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (audio_renderer_)
        audio_renderer_->SetVolume(volume);
}

base::TimeDelta RendererImpl::GetMediaTime()
{
    // No BelongsToCurrentThread() checking because this can be called from other
    // threads.
    return time_source_->CurrentMediaTime();
}

bool RendererImpl::HasAudio()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    return audio_renderer_ != NULL;
}

bool RendererImpl::HasVideo()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    return video_renderer_ != NULL;
}

void RendererImpl::DisableUnderflowForTesting()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(state_, STATE_UNINITIALIZED);

    underflow_disabled_for_testing_ = true;
}

void RendererImpl::EnableClocklessVideoPlaybackForTesting()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(state_, STATE_UNINITIALIZED);
    DCHECK(underflow_disabled_for_testing_)
        << "Underflow must be disabled for clockless video playback";

    clockless_video_playback_enabled_for_testing_ = true;
}

bool RendererImpl::GetWallClockTimes(
    const std::vector<base::TimeDelta>& media_timestamps,
    std::vector<base::TimeTicks>* wall_clock_times)
{
    // No BelongsToCurrentThread() checking because this can be called from other
    // threads.
    //
    // TODO(scherkus): Currently called from VideoRendererImpl's internal thread,
    // which should go away at some point http://crbug.com/110814
    if (clockless_video_playback_enabled_for_testing_) {
        if (media_timestamps.empty()) {
            *wall_clock_times = std::vector<base::TimeTicks>(1,
                base::TimeTicks::Now());
        } else {
            *wall_clock_times = std::vector<base::TimeTicks>();
            for (auto const& media_time : media_timestamps) {
                wall_clock_times->push_back(base::TimeTicks() + media_time);
            }
        }
        return true;
    }

    return time_source_->GetWallClockTimes(media_timestamps, wall_clock_times);
}

void RendererImpl::SetCdmReadyCallback(const CdmReadyCB& cdm_ready_cb)
{
    // Cancels the previous CDM request.
    if (cdm_ready_cb.is_null()) {
        if (!cdm_ready_cb_.is_null()) {
            base::ResetAndReturn(&cdm_ready_cb_)
                .Run(nullptr, base::Bind(IgnoreCdmAttached));
        }
        return;
    }

    // We initialize audio and video decoders in sequence.
    DCHECK(cdm_ready_cb_.is_null());

    if (cdm_context_) {
        cdm_ready_cb.Run(cdm_context_, base::Bind(IgnoreCdmAttached));
        return;
    }

    cdm_ready_cb_ = cdm_ready_cb;
}

void RendererImpl::InitializeAudioRenderer()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(state_, STATE_INITIALIZING);
    DCHECK(!init_cb_.is_null());

    PipelineStatusCB done_cb = base::Bind(&RendererImpl::OnAudioRendererInitializeDone, weak_this_);

    if (!demuxer_stream_provider_->GetStream(DemuxerStream::AUDIO)) {
        audio_renderer_.reset();
        task_runner_->PostTask(FROM_HERE, base::Bind(done_cb, PIPELINE_OK));
        return;
    }

    // Note: After the initialization of a renderer, error events from it may
    // happen at any time and all future calls must guard against STATE_ERROR.
    audio_renderer_->Initialize(
        demuxer_stream_provider_->GetStream(DemuxerStream::AUDIO), done_cb,
        base::Bind(&RendererImpl::SetCdmReadyCallback, weak_this_),
        base::Bind(&RendererImpl::OnUpdateStatistics, weak_this_),
        base::Bind(&RendererImpl::OnBufferingStateChanged, weak_this_,
            &audio_buffering_state_),
        base::Bind(&RendererImpl::OnAudioRendererEnded, weak_this_),
        base::Bind(&RendererImpl::OnError, weak_this_),
        waiting_for_decryption_key_cb_);
}

void RendererImpl::OnAudioRendererInitializeDone(PipelineStatus status)
{
    DVLOG(1) << __FUNCTION__ << ": " << status;
    DCHECK(task_runner_->BelongsToCurrentThread());

    // OnError() may be fired at any time by the renderers, even if they thought
    // they initialized successfully (due to delayed output device setup).
    if (state_ != STATE_INITIALIZING) {
        DCHECK(init_cb_.is_null());
        audio_renderer_.reset();
        return;
    }

    if (status != PIPELINE_OK) {
        base::ResetAndReturn(&init_cb_).Run(status);
        return;
    }

    DCHECK(!init_cb_.is_null());
    InitializeVideoRenderer();
}

void RendererImpl::InitializeVideoRenderer()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(state_, STATE_INITIALIZING);
    DCHECK(!init_cb_.is_null());

    PipelineStatusCB done_cb = base::Bind(&RendererImpl::OnVideoRendererInitializeDone, weak_this_);

    if (!demuxer_stream_provider_->GetStream(DemuxerStream::VIDEO)) {
        video_renderer_.reset();
        task_runner_->PostTask(FROM_HERE, base::Bind(done_cb, PIPELINE_OK));
        return;
    }

    video_renderer_->Initialize(
        demuxer_stream_provider_->GetStream(DemuxerStream::VIDEO), done_cb,
        base::Bind(&RendererImpl::SetCdmReadyCallback, weak_this_),
        base::Bind(&RendererImpl::OnUpdateStatistics, weak_this_),
        base::Bind(&RendererImpl::OnBufferingStateChanged, weak_this_,
            &video_buffering_state_),
        base::Bind(&RendererImpl::OnVideoRendererEnded, weak_this_),
        base::Bind(&RendererImpl::OnError, weak_this_),
        base::Bind(&RendererImpl::GetWallClockTimes, base::Unretained(this)),
        waiting_for_decryption_key_cb_);
}

void RendererImpl::OnVideoRendererInitializeDone(PipelineStatus status)
{
    DVLOG(1) << __FUNCTION__ << ": " << status;
    DCHECK(task_runner_->BelongsToCurrentThread());

    // OnError() may be fired at any time by the renderers, even if they thought
    // they initialized successfully (due to delayed output device setup).
    if (state_ != STATE_INITIALIZING) {
        DCHECK(init_cb_.is_null());
        audio_renderer_.reset();
        video_renderer_.reset();
        return;
    }

    DCHECK(!init_cb_.is_null());

    if (status != PIPELINE_OK) {
        base::ResetAndReturn(&init_cb_).Run(status);
        return;
    }

    if (audio_renderer_) {
        time_source_ = audio_renderer_->GetTimeSource();
    } else if (!time_source_) {
        wall_clock_time_source_.reset(new WallClockTimeSource());
        time_source_ = wall_clock_time_source_.get();
    }

    state_ = STATE_PLAYING;
    DCHECK(time_source_);
    DCHECK(audio_renderer_ || video_renderer_);
    base::ResetAndReturn(&init_cb_).Run(PIPELINE_OK);
}

void RendererImpl::FlushAudioRenderer()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(state_, STATE_FLUSHING);
    DCHECK(!flush_cb_.is_null());

    if (!audio_renderer_) {
        OnAudioRendererFlushDone();
        return;
    }

    audio_renderer_->Flush(
        base::Bind(&RendererImpl::OnAudioRendererFlushDone, weak_this_));
}

void RendererImpl::OnAudioRendererFlushDone()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (state_ == STATE_ERROR) {
        DCHECK(flush_cb_.is_null());
        return;
    }

    DCHECK_EQ(state_, STATE_FLUSHING);
    DCHECK(!flush_cb_.is_null());

    // If we had a deferred video renderer underflow prior to the flush, it should
    // have been cleared by the audio renderer changing to BUFFERING_HAVE_NOTHING.
    DCHECK(deferred_underflow_cb_.IsCancelled());

    DCHECK_EQ(audio_buffering_state_, BUFFERING_HAVE_NOTHING);
    audio_ended_ = false;
    FlushVideoRenderer();
}

void RendererImpl::FlushVideoRenderer()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(state_, STATE_FLUSHING);
    DCHECK(!flush_cb_.is_null());

    if (!video_renderer_) {
        OnVideoRendererFlushDone();
        return;
    }

    video_renderer_->Flush(
        base::Bind(&RendererImpl::OnVideoRendererFlushDone, weak_this_));
}

void RendererImpl::OnVideoRendererFlushDone()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (state_ == STATE_ERROR) {
        DCHECK(flush_cb_.is_null());
        return;
    }

    DCHECK_EQ(state_, STATE_FLUSHING);
    DCHECK(!flush_cb_.is_null());

    DCHECK_EQ(video_buffering_state_, BUFFERING_HAVE_NOTHING);
    video_ended_ = false;
    state_ = STATE_PLAYING;
    base::ResetAndReturn(&flush_cb_).Run();
}

void RendererImpl::OnUpdateStatistics(const PipelineStatistics& stats)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    statistics_cb_.Run(stats);
}

void RendererImpl::OnBufferingStateChanged(BufferingState* buffering_state,
    BufferingState new_buffering_state)
{
    const bool is_audio = buffering_state == &audio_buffering_state_;
    DVLOG(1) << __FUNCTION__ << "(" << *buffering_state << ", "
             << new_buffering_state << ") " << (is_audio ? "audio" : "video");
    DCHECK(task_runner_->BelongsToCurrentThread());

    bool was_waiting_for_enough_data = WaitingForEnoughData();

    // When audio is present and has enough data, defer video underflow callbacks
    // for some time to avoid unnecessary glitches in audio; see
    // http://crbug.com/144683#c53.
    if (audio_renderer_ && !is_audio && state_ == STATE_PLAYING) {
        if (video_buffering_state_ == BUFFERING_HAVE_ENOUGH && audio_buffering_state_ == BUFFERING_HAVE_ENOUGH && new_buffering_state == BUFFERING_HAVE_NOTHING && deferred_underflow_cb_.IsCancelled()) {
            deferred_underflow_cb_.Reset(base::Bind(
                &RendererImpl::OnBufferingStateChanged, weak_factory_.GetWeakPtr(),
                buffering_state, new_buffering_state));
            task_runner_->PostDelayedTask(FROM_HERE,
                deferred_underflow_cb_.callback(),
                video_underflow_threshold_);
            return;
        }

        deferred_underflow_cb_.Cancel();
    } else if (!deferred_underflow_cb_.IsCancelled() && is_audio && new_buffering_state == BUFFERING_HAVE_NOTHING) {
        // If audio underflows while we have a deferred video underflow in progress
        // we want to mark video as underflowed immediately and cancel the deferral.
        deferred_underflow_cb_.Cancel();
        video_buffering_state_ = BUFFERING_HAVE_NOTHING;
    }

    *buffering_state = new_buffering_state;

    // Disable underflow by ignoring updates that renderers have ran out of data.
    if (state_ == STATE_PLAYING && underflow_disabled_for_testing_ && time_ticking_) {
        DVLOG(1) << "Update ignored because underflow is disabled for testing.";
        return;
    }

    // Renderer underflowed.
    if (!was_waiting_for_enough_data && WaitingForEnoughData()) {
        PausePlayback();

        // TODO(scherkus): Fire BUFFERING_HAVE_NOTHING callback to alert clients of
        // underflow state http://crbug.com/144683
        return;
    }

    // Renderer prerolled.
    if (was_waiting_for_enough_data && !WaitingForEnoughData()) {
        StartPlayback();
        buffering_state_cb_.Run(BUFFERING_HAVE_ENOUGH);
        return;
    }
}

bool RendererImpl::WaitingForEnoughData() const
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    if (state_ != STATE_PLAYING)
        return false;
    if (audio_renderer_ && audio_buffering_state_ != BUFFERING_HAVE_ENOUGH)
        return true;
    if (video_renderer_ && video_buffering_state_ != BUFFERING_HAVE_ENOUGH)
        return true;
    return false;
}

void RendererImpl::PausePlayback()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(time_ticking_);
    switch (state_) {
    case STATE_PLAYING:
        DCHECK(PlaybackHasEnded() || WaitingForEnoughData())
            << "Playback should only pause due to ending or underflowing";
        break;

    case STATE_FLUSHING:
        // It's OK to pause playback when flushing.
        break;

    case STATE_UNINITIALIZED:
    case STATE_INITIALIZING:
        NOTREACHED() << "Invalid state: " << state_;
        break;

    case STATE_ERROR:
        // An error state may occur at any time.
        break;
    }

    time_ticking_ = false;
    time_source_->StopTicking();
    if (playback_rate_ > 0 && video_renderer_)
        video_renderer_->OnTimeStateChanged(false);
}

void RendererImpl::StartPlayback()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(state_, STATE_PLAYING);
    DCHECK(!time_ticking_);
    DCHECK(!WaitingForEnoughData());

    time_ticking_ = true;
    time_source_->StartTicking();
    if (playback_rate_ > 0 && video_renderer_)
        video_renderer_->OnTimeStateChanged(true);
}

void RendererImpl::OnAudioRendererEnded()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (state_ != STATE_PLAYING)
        return;

    DCHECK(!audio_ended_);
    audio_ended_ = true;

    RunEndedCallbackIfNeeded();
}

void RendererImpl::OnVideoRendererEnded()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (state_ != STATE_PLAYING)
        return;

    DCHECK(!video_ended_);
    video_ended_ = true;

    RunEndedCallbackIfNeeded();
}

bool RendererImpl::PlaybackHasEnded() const
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (audio_renderer_ && !audio_ended_)
        return false;

    if (video_renderer_ && !video_ended_)
        return false;

    return true;
}

void RendererImpl::RunEndedCallbackIfNeeded()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (!PlaybackHasEnded())
        return;

    if (time_ticking_)
        PausePlayback();

    ended_cb_.Run();
}

void RendererImpl::OnError(PipelineStatus error)
{
    DVLOG(1) << __FUNCTION__ << "(" << error << ")";
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_NE(PIPELINE_OK, error) << "PIPELINE_OK isn't an error!";

    // An error has already been delivered.
    if (state_ == STATE_ERROR)
        return;

    const State old_state = state_;
    state_ = STATE_ERROR;

    if (old_state == STATE_INITIALIZING) {
        base::ResetAndReturn(&init_cb_).Run(error);
        return;
    }

    // After OnError() returns, the pipeline may destroy |this|.
    base::ResetAndReturn(&error_cb_).Run(error);

    if (!flush_cb_.is_null())
        base::ResetAndReturn(&flush_cb_).Run();
}

} // namespace media
