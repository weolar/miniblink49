// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/pipeline.h"

#include <algorithm>

#include "base/bind.h"
#include "base/callback.h"
#include "base/callback_helpers.h"
#include "base/command_line.h"
#include "base/compiler_specific.h"
#include "base/location.h"
#include "base/metrics/histogram.h"
#include "base/single_thread_task_runner.h"
#include "base/stl_util.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"
#include "base/synchronization/condition_variable.h"
#include "media/base/media_log.h"
#include "media/base/media_switches.h"
#include "media/base/renderer.h"
#include "media/base/text_renderer.h"
#include "media/base/text_track_config.h"
#include "media/base/video_decoder_config.h"

using base::TimeDelta;

namespace media {

Pipeline::Pipeline(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    MediaLog* media_log)
    : task_runner_(task_runner)
    , media_log_(media_log)
    , running_(false)
    , did_loading_progress_(false)
    , volume_(1.0f)
    , playback_rate_(0.0)
    , status_(PIPELINE_OK)
    , state_(kCreated)
    , renderer_ended_(false)
    , text_renderer_ended_(false)
    , demuxer_(NULL)
    , pending_cdm_context_(nullptr)
    , weak_factory_(this)
{
    media_log_->AddEvent(media_log_->CreatePipelineStateChangedEvent(kCreated));
}

Pipeline::~Pipeline()
{
    DCHECK(thread_checker_.CalledOnValidThread())
        << "Pipeline must be destroyed on same thread that created it";
    DCHECK(!running_) << "Stop() must complete before destroying object";
    DCHECK(stop_cb_.is_null());
    DCHECK(seek_cb_.is_null());
}

void Pipeline::Start(Demuxer* demuxer,
    scoped_ptr<Renderer> renderer,
    const base::Closure& ended_cb,
    const PipelineStatusCB& error_cb,
    const PipelineStatusCB& seek_cb,
    const PipelineMetadataCB& metadata_cb,
    const BufferingStateCB& buffering_state_cb,
    const base::Closure& duration_change_cb,
    const AddTextTrackCB& add_text_track_cb,
    const base::Closure& waiting_for_decryption_key_cb)
{
    DCHECK(!ended_cb.is_null());
    DCHECK(!error_cb.is_null());
    DCHECK(!seek_cb.is_null());
    DCHECK(!metadata_cb.is_null());
    DCHECK(!buffering_state_cb.is_null());

    base::AutoLock auto_lock(lock_);
    CHECK(!running_) << "Media pipeline is already running";
    running_ = true;

    demuxer_ = demuxer;
    renderer_ = renderer.Pass();
    ended_cb_ = ended_cb;
    error_cb_ = error_cb;
    seek_cb_ = seek_cb;
    metadata_cb_ = metadata_cb;
    buffering_state_cb_ = buffering_state_cb;
    duration_change_cb_ = duration_change_cb;
    add_text_track_cb_ = add_text_track_cb;
    waiting_for_decryption_key_cb_ = waiting_for_decryption_key_cb;

    task_runner_->PostTask(
        FROM_HERE, base::Bind(&Pipeline::StartTask, weak_factory_.GetWeakPtr()));
}

void Pipeline::Stop(const base::Closure& stop_cb)
{
    DVLOG(2) << __FUNCTION__;
    task_runner_->PostTask(
        FROM_HERE,
        base::Bind(&Pipeline::StopTask, weak_factory_.GetWeakPtr(), stop_cb));
}

void Pipeline::Seek(TimeDelta time, const PipelineStatusCB& seek_cb)
{
    base::AutoLock auto_lock(lock_);
    if (!running_) {
        DLOG(ERROR) << "Media pipeline isn't running. Ignoring Seek().";
        return;
    }

    task_runner_->PostTask(
        FROM_HERE,
        base::Bind(
            &Pipeline::SeekTask, weak_factory_.GetWeakPtr(), time, seek_cb));
}

bool Pipeline::IsRunning() const
{
    base::AutoLock auto_lock(lock_);
    return running_;
}

double Pipeline::GetPlaybackRate() const
{
    base::AutoLock auto_lock(lock_);
    return playback_rate_;
}

void Pipeline::SetPlaybackRate(double playback_rate)
{
    if (playback_rate < 0.0)
        return;

    base::AutoLock auto_lock(lock_);
    playback_rate_ = playback_rate;
    if (running_) {
        task_runner_->PostTask(FROM_HERE,
            base::Bind(&Pipeline::PlaybackRateChangedTask,
                weak_factory_.GetWeakPtr(),
                playback_rate));
    }
}

float Pipeline::GetVolume() const
{
    base::AutoLock auto_lock(lock_);
    return volume_;
}

void Pipeline::SetVolume(float volume)
{
    if (volume < 0.0f || volume > 1.0f)
        return;

    base::AutoLock auto_lock(lock_);
    volume_ = volume;
    if (running_) {
        task_runner_->PostTask(
            FROM_HERE,
            base::Bind(
                &Pipeline::VolumeChangedTask, weak_factory_.GetWeakPtr(), volume));
    }
}

TimeDelta Pipeline::GetMediaTime() const
{
    base::AutoLock auto_lock(lock_);
    return renderer_ ? std::min(renderer_->GetMediaTime(), duration_)
                     : TimeDelta();
}

Ranges<TimeDelta> Pipeline::GetBufferedTimeRanges() const
{
    base::AutoLock auto_lock(lock_);
    return buffered_time_ranges_;
}

TimeDelta Pipeline::GetMediaDuration() const
{
    base::AutoLock auto_lock(lock_);
    return duration_;
}

bool Pipeline::DidLoadingProgress()
{
    base::AutoLock auto_lock(lock_);
    bool ret = did_loading_progress_;
    did_loading_progress_ = false;
    return ret;
}

PipelineStatistics Pipeline::GetStatistics() const
{
    base::AutoLock auto_lock(lock_);
    return statistics_;
}

void Pipeline::SetCdm(CdmContext* cdm_context,
    const CdmAttachedCB& cdm_attached_cb)
{
    task_runner_->PostTask(
        FROM_HERE, base::Bind(&Pipeline::SetCdmTask, weak_factory_.GetWeakPtr(), cdm_context, cdm_attached_cb));
}

void Pipeline::SetErrorForTesting(PipelineStatus status)
{
    OnError(status);
}

bool Pipeline::HasWeakPtrsForTesting() const
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    return weak_factory_.HasWeakPtrs();
}

void Pipeline::SetState(State next_state)
{
    DVLOG(1) << GetStateString(state_) << " -> " << GetStateString(next_state);

    state_ = next_state;
    media_log_->AddEvent(media_log_->CreatePipelineStateChangedEvent(next_state));
}

#define RETURN_STRING(state) \
    case state:              \
        return #state;

const char* Pipeline::GetStateString(State state)
{
    switch (state) {
        RETURN_STRING(kCreated);
        RETURN_STRING(kInitDemuxer);
        RETURN_STRING(kInitRenderer);
        RETURN_STRING(kSeeking);
        RETURN_STRING(kPlaying);
        RETURN_STRING(kStopping);
        RETURN_STRING(kStopped);
    }
    NOTREACHED();
    return "INVALID";
}

#undef RETURN_STRING

Pipeline::State Pipeline::GetNextState() const
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(stop_cb_.is_null())
        << "State transitions don't happen when stopping";
    DCHECK_EQ(status_, PIPELINE_OK)
        << "State transitions don't happen when there's an error: " << status_;

    switch (state_) {
    case kCreated:
        return kInitDemuxer;

    case kInitDemuxer:
        return kInitRenderer;

    case kInitRenderer:
    case kSeeking:
        return kPlaying;

    case kPlaying:
    case kStopping:
    case kStopped:
        break;
    }
    NOTREACHED() << "State has no transition: " << state_;
    return state_;
}

void Pipeline::OnDemuxerError(PipelineStatus error)
{
    task_runner_->PostTask(FROM_HERE,
        base::Bind(&Pipeline::ErrorChangedTask,
            weak_factory_.GetWeakPtr(),
            error));
}

void Pipeline::AddTextStream(DemuxerStream* text_stream,
    const TextTrackConfig& config)
{
    task_runner_->PostTask(FROM_HERE,
        base::Bind(&Pipeline::AddTextStreamTask,
            weak_factory_.GetWeakPtr(),
            text_stream,
            config));
}

void Pipeline::RemoveTextStream(DemuxerStream* text_stream)
{
    task_runner_->PostTask(FROM_HERE,
        base::Bind(&Pipeline::RemoveTextStreamTask,
            weak_factory_.GetWeakPtr(),
            text_stream));
}

void Pipeline::OnError(PipelineStatus error)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(IsRunning());
    DCHECK_NE(PIPELINE_OK, error);
    VLOG(1) << "Media pipeline error: " << error;

    task_runner_->PostTask(FROM_HERE, base::Bind(&Pipeline::ErrorChangedTask, weak_factory_.GetWeakPtr(), error));
}

void Pipeline::SetDuration(TimeDelta duration)
{
    DCHECK(IsRunning());
    media_log_->AddEvent(
        media_log_->CreateTimeEvent(
            MediaLogEvent::DURATION_SET, "duration", duration));
    //UMA_HISTOGRAM_LONG_TIMES("Media.Duration", duration);

    base::AutoLock auto_lock(lock_);
    duration_ = duration;
    if (!duration_change_cb_.is_null())
        duration_change_cb_.Run();
}

void Pipeline::StateTransitionTask(PipelineStatus status)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    // No-op any state transitions if we're stopping.
    if (state_ == kStopping || state_ == kStopped)
        return;

    // Preserve existing abnormal status, otherwise update based on the result of
    // the previous operation.
    status_ = (status_ != PIPELINE_OK ? status_ : status);

    if (status_ != PIPELINE_OK) {
        ErrorChangedTask(status_);
        return;
    }

    // Guard against accidentally clearing |pending_callbacks_| for states that
    // use it as well as states that should not be using it.
    DCHECK_EQ(pending_callbacks_.get() != NULL, state_ == kSeeking);

    pending_callbacks_.reset();

    PipelineStatusCB done_cb = base::Bind(&Pipeline::StateTransitionTask, weak_factory_.GetWeakPtr());

    // Switch states, performing any entrance actions for the new state as well.
    SetState(GetNextState());
    switch (state_) {
    case kInitDemuxer:
        return InitializeDemuxer(done_cb);

    case kInitRenderer:
        // When the state_ transfers to kInitRenderer, it means the demuxer has
        // finished parsing the init info. It should call ReportMetadata in case
        // meeting 'decode' error when passing media segment but WebMediaPlayer's
        // ready_state_ is still ReadyStateHaveNothing. In that case, it will
        // treat it as NetworkStateFormatError not NetworkStateDecodeError.
        ReportMetadata();
        start_timestamp_ = demuxer_->GetStartTime();

        return InitializeRenderer(done_cb);

    case kPlaying:
        DCHECK(start_timestamp_ >= base::TimeDelta());
        renderer_->StartPlayingFrom(start_timestamp_);

        if (text_renderer_)
            text_renderer_->StartPlaying();

        base::ResetAndReturn(&seek_cb_).Run(PIPELINE_OK);

        PlaybackRateChangedTask(GetPlaybackRate());
        VolumeChangedTask(GetVolume());
        return;

    case kStopping:
    case kStopped:
    case kCreated:
    case kSeeking:
        NOTREACHED() << "State has no transition: " << state_;
        return;
    }
}

// Note that the usage of base::Unretained() with the renderers is considered
// safe as they are owned by |pending_callbacks_| and share the same lifetime.
//
// That being said, deleting the renderers while keeping |pending_callbacks_|
// running on the media thread would result in crashes.
void Pipeline::DoSeek(TimeDelta seek_timestamp,
    const PipelineStatusCB& done_cb)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(!pending_callbacks_.get());
    DCHECK_EQ(state_, kSeeking);
    SerialRunner::Queue bound_fns;

    // Pause.
    if (text_renderer_) {
        bound_fns.Push(base::Bind(
            &TextRenderer::Pause, base::Unretained(text_renderer_.get())));
    }

    // Flush.
    DCHECK(renderer_);
    bound_fns.Push(
        base::Bind(&Renderer::Flush, base::Unretained(renderer_.get())));

    if (text_renderer_) {
        bound_fns.Push(base::Bind(
            &TextRenderer::Flush, base::Unretained(text_renderer_.get())));
    }

    // Seek demuxer.
    bound_fns.Push(base::Bind(
        &Demuxer::Seek, base::Unretained(demuxer_), seek_timestamp));

    pending_callbacks_ = SerialRunner::Run(bound_fns, done_cb);
}

void Pipeline::DoStop(const PipelineStatusCB& done_cb)
{
    DVLOG(2) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(!pending_callbacks_.get());

    // TODO(scherkus): Enforce that Renderer is only called on a single thread,
    // even for accessing media time http://crbug.com/370634
    scoped_ptr<Renderer> renderer;
    {
        base::AutoLock auto_lock(lock_);
        renderer.swap(renderer_);
    }
    renderer.reset();
    text_renderer_.reset();

    if (demuxer_) {
        demuxer_->Stop();
        demuxer_ = NULL;
    }

    task_runner_->PostTask(FROM_HERE, base::Bind(done_cb, PIPELINE_OK));
}

void Pipeline::OnStopCompleted(PipelineStatus status)
{
    DVLOG(2) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(state_, kStopping);
    DCHECK(!renderer_);
    DCHECK(!text_renderer_);

    {
        base::AutoLock auto_lock(lock_);
        running_ = false;
    }

    SetState(kStopped);
    demuxer_ = NULL;

    // If we stop during initialization/seeking we want to run |seek_cb_|
    // followed by |stop_cb_| so we don't leave outstanding callbacks around.
    if (!seek_cb_.is_null()) {
        base::ResetAndReturn(&seek_cb_).Run(status_);
        error_cb_.Reset();
    }
    if (!stop_cb_.is_null()) {
        error_cb_.Reset();

        // Invalid all weak pointers so it's safe to destroy |this| on the render
        // main thread.
        weak_factory_.InvalidateWeakPtrs();

        base::ResetAndReturn(&stop_cb_).Run();

        // NOTE: pipeline may be deleted at this point in time as a result of
        // executing |stop_cb_|.
        return;
    }
    if (!error_cb_.is_null()) {
        DCHECK_NE(status_, PIPELINE_OK);
        base::ResetAndReturn(&error_cb_).Run(status_);
    }
}

void Pipeline::AddBufferedTimeRange(TimeDelta start, TimeDelta end)
{
    DCHECK(IsRunning());
    base::AutoLock auto_lock(lock_);
    buffered_time_ranges_.Add(start, end);
    did_loading_progress_ = true;
}

// Called from any thread.
void Pipeline::OnUpdateStatistics(const PipelineStatistics& stats_delta)
{
    base::AutoLock auto_lock(lock_);
    statistics_.audio_bytes_decoded += stats_delta.audio_bytes_decoded;
    statistics_.video_bytes_decoded += stats_delta.video_bytes_decoded;
    statistics_.video_frames_decoded += stats_delta.video_frames_decoded;
    statistics_.video_frames_dropped += stats_delta.video_frames_dropped;
    statistics_.audio_memory_usage += stats_delta.audio_memory_usage;
    statistics_.video_memory_usage += stats_delta.video_memory_usage;
}

void Pipeline::StartTask()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    CHECK_EQ(kCreated, state_)
        << "Media pipeline cannot be started more than once";

    text_renderer_ = CreateTextRenderer();
    if (text_renderer_) {
        text_renderer_->Initialize(
            base::Bind(&Pipeline::OnTextRendererEnded, weak_factory_.GetWeakPtr()));
    }

    // Set CDM early to avoid unnecessary delay in Renderer::Initialize().
    if (pending_cdm_context_) {
        renderer_->SetCdm(pending_cdm_context_, base::Bind(&IgnoreCdmAttached));
        pending_cdm_context_ = nullptr;
    }

    StateTransitionTask(PIPELINE_OK);
}

void Pipeline::StopTask(const base::Closure& stop_cb)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(stop_cb_.is_null());

    if (state_ == kStopped) {
        // Invalid all weak pointers so it's safe to destroy |this| on the render
        // main thread.
        weak_factory_.InvalidateWeakPtrs();

        // NOTE: pipeline may be deleted at this point in time as a result of
        // executing |stop_cb|.
        stop_cb.Run();

        return;
    }

    stop_cb_ = stop_cb;

    // We may already be stopping due to a runtime error.
    if (state_ == kStopping)
        return;

    // Do not report statistics if the pipeline is not fully initialized.
    if (state_ == kSeeking || state_ == kPlaying) {
        PipelineStatistics stats = GetStatistics();
        if (renderer_->HasVideo() && stats.video_frames_decoded > 0) {
            UMA_HISTOGRAM_COUNTS("Media.DroppedFrameCount",
                stats.video_frames_dropped);
        }
    }

    SetState(kStopping);
    pending_callbacks_.reset();
    DoStop(base::Bind(&Pipeline::OnStopCompleted, weak_factory_.GetWeakPtr()));
}

void Pipeline::ErrorChangedTask(PipelineStatus error)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_NE(PIPELINE_OK, error) << "PIPELINE_OK isn't an error!";

    media_log_->AddEvent(media_log_->CreatePipelineErrorEvent(error));

    if (state_ == kStopping || state_ == kStopped)
        return;

    SetState(kStopping);
    pending_callbacks_.reset();
    status_ = error;

    DoStop(base::Bind(&Pipeline::OnStopCompleted, weak_factory_.GetWeakPtr()));
}

void Pipeline::PlaybackRateChangedTask(double playback_rate)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    // Playback rate changes are only carried out while playing.
    if (state_ != kPlaying)
        return;

    renderer_->SetPlaybackRate(playback_rate);
}

void Pipeline::VolumeChangedTask(float volume)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    // Volume changes are only carried out while playing.
    if (state_ != kPlaying)
        return;

    renderer_->SetVolume(volume);
}

void Pipeline::SeekTask(TimeDelta time, const PipelineStatusCB& seek_cb)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(stop_cb_.is_null());

    // Suppress seeking if we're not fully started.
    if (state_ != kPlaying) {
        DCHECK(state_ == kStopping || state_ == kStopped)
            << "Receive extra seek in unexpected state: " << state_;

        // TODO(scherkus): should we run the callback?  I'm tempted to say the API
        // will only execute the first Seek() request.
        DVLOG(1) << "Media pipeline has not started, ignoring seek to "
                 << time.InMicroseconds() << " (current state: " << state_ << ")";
        return;
    }

    DCHECK(seek_cb_.is_null());

    const base::TimeDelta seek_timestamp = std::max(time, demuxer_->GetStartTime());

    SetState(kSeeking);
    seek_cb_ = seek_cb;
    renderer_ended_ = false;
    text_renderer_ended_ = false;
    start_timestamp_ = seek_timestamp;

    DoSeek(seek_timestamp, base::Bind(&Pipeline::StateTransitionTask, weak_factory_.GetWeakPtr()));
}

void Pipeline::SetCdmTask(CdmContext* cdm_context,
    const CdmAttachedCB& cdm_attached_cb)
{
    base::AutoLock auto_lock(lock_);
    if (!renderer_) {
        pending_cdm_context_ = cdm_context;
        cdm_attached_cb.Run(true);
        return;
    }

    renderer_->SetCdm(cdm_context, cdm_attached_cb);
}

void Pipeline::OnRendererEnded()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    media_log_->AddEvent(media_log_->CreateEvent(MediaLogEvent::ENDED));

    if (state_ != kPlaying)
        return;

    DCHECK(!renderer_ended_);
    renderer_ended_ = true;

    RunEndedCallbackIfNeeded();
}

void Pipeline::OnTextRendererEnded()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    media_log_->AddEvent(media_log_->CreateEvent(MediaLogEvent::TEXT_ENDED));

    if (state_ != kPlaying)
        return;

    DCHECK(!text_renderer_ended_);
    text_renderer_ended_ = true;

    RunEndedCallbackIfNeeded();
}

void Pipeline::RunEndedCallbackIfNeeded()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (renderer_ && !renderer_ended_)
        return;

    if (text_renderer_ && text_renderer_->HasTracks() && !text_renderer_ended_)
        return;

    DCHECK_EQ(status_, PIPELINE_OK);
    ended_cb_.Run();
}

scoped_ptr<TextRenderer> Pipeline::CreateTextRenderer()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    const base::CommandLine* cmd_line = base::CommandLine::ForCurrentProcess();
    if (!cmd_line->HasSwitch(switches::kEnableInbandTextTracks))
        return scoped_ptr<media::TextRenderer>();

    return scoped_ptr<media::TextRenderer>(new media::TextRenderer(
        task_runner_,
        base::Bind(&Pipeline::OnAddTextTrack, weak_factory_.GetWeakPtr())));
}

void Pipeline::AddTextStreamTask(DemuxerStream* text_stream,
    const TextTrackConfig& config)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    // TODO(matthewjheaney): fix up text_ended_ when text stream
    // is added (http://crbug.com/321446).
    if (text_renderer_)
        text_renderer_->AddTextStream(text_stream, config);
}

void Pipeline::RemoveTextStreamTask(DemuxerStream* text_stream)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    if (text_renderer_)
        text_renderer_->RemoveTextStream(text_stream);
}

void Pipeline::OnAddTextTrack(const TextTrackConfig& config,
    const AddTextTrackDoneCB& done_cb)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    add_text_track_cb_.Run(config, done_cb);
}

void Pipeline::InitializeDemuxer(const PipelineStatusCB& done_cb)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    demuxer_->Initialize(this, done_cb, text_renderer_);
}

void Pipeline::InitializeRenderer(const PipelineStatusCB& done_cb)
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (!demuxer_->GetStream(DemuxerStream::AUDIO) && !demuxer_->GetStream(DemuxerStream::VIDEO)) {
        {
            base::AutoLock auto_lock(lock_);
            renderer_.reset();
        }
        OnError(PIPELINE_ERROR_COULD_NOT_RENDER);
        return;
    }

    base::WeakPtr<Pipeline> weak_this = weak_factory_.GetWeakPtr();
    renderer_->Initialize(
        demuxer_,
        done_cb,
        base::Bind(&Pipeline::OnUpdateStatistics, weak_this),
        base::Bind(&Pipeline::BufferingStateChanged, weak_this),
        base::Bind(&Pipeline::OnRendererEnded, weak_this),
        base::Bind(&Pipeline::OnError, weak_this),
        waiting_for_decryption_key_cb_);
}

void Pipeline::ReportMetadata()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    PipelineMetadata metadata;
    metadata.timeline_offset = demuxer_->GetTimelineOffset();
    DemuxerStream* stream = demuxer_->GetStream(DemuxerStream::VIDEO);
    if (stream) {
        metadata.has_video = true;
        metadata.natural_size = stream->video_decoder_config().natural_size();
        metadata.video_rotation = stream->video_rotation();
    }
    if (demuxer_->GetStream(DemuxerStream::AUDIO)) {
        metadata.has_audio = true;
    }
    metadata_cb_.Run(metadata);
}

void Pipeline::BufferingStateChanged(BufferingState new_buffering_state)
{
    DVLOG(1) << __FUNCTION__ << "(" << new_buffering_state << ") ";
    DCHECK(task_runner_->BelongsToCurrentThread());
    buffering_state_cb_.Run(new_buffering_state);
}

} // namespace media
