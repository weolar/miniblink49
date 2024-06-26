// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/renderers/audio_renderer_impl.h"

#include <math.h>

#include <algorithm>

#include "base/bind.h"
#include "base/callback.h"
#include "base/callback_helpers.h"
#include "base/logging.h"
#include "base/metrics/histogram.h"
#include "base/single_thread_task_runner.h"
#include "base/time/default_tick_clock.h"
#include "media/base/audio_buffer.h"
#include "media/base/audio_buffer_converter.h"
#include "media/base/audio_hardware_config.h"
#include "media/base/audio_splicer.h"
#include "media/base/bind_to_current_loop.h"
#include "media/base/demuxer_stream.h"
#include "media/base/media_log.h"
#include "media/base/timestamp_constants.h"
#include "media/filters/audio_clock.h"
#include "media/filters/decrypting_demuxer_stream.h"

namespace media {

namespace {

    enum AudioRendererEvent {
        INITIALIZED,
        RENDER_ERROR,
        RENDER_EVENT_MAX = RENDER_ERROR,
    };

    void HistogramRendererEvent(AudioRendererEvent event)
    {
        UMA_HISTOGRAM_ENUMERATION(
            "Media.AudioRendererEvents", event, RENDER_EVENT_MAX + 1);
    }

} // namespace

AudioRendererImpl::AudioRendererImpl(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    media::AudioRendererSink* sink,
    ScopedVector<AudioDecoder> decoders,
    const AudioHardwareConfig& hardware_config,
    const scoped_refptr<MediaLog>& media_log)
    : task_runner_(task_runner)
    , expecting_config_changes_(false)
    , sink_(sink)
    , audio_buffer_stream_(
          new AudioBufferStream(task_runner, decoders.Pass(), media_log))
    , hardware_config_(hardware_config)
    , media_log_(media_log)
    , tick_clock_(new base::DefaultTickClock())
    , last_audio_memory_usage_(0)
    , playback_rate_(0.0)
    , state_(kUninitialized)
    , buffering_state_(BUFFERING_HAVE_NOTHING)
    , rendering_(false)
    , sink_playing_(false)
    , pending_read_(false)
    , received_end_of_stream_(false)
    , rendered_end_of_stream_(false)
    , weak_factory_(this)
{
    audio_buffer_stream_->set_splice_observer(base::Bind(
        &AudioRendererImpl::OnNewSpliceBuffer, weak_factory_.GetWeakPtr()));
    audio_buffer_stream_->set_config_change_observer(base::Bind(
        &AudioRendererImpl::OnConfigChange, weak_factory_.GetWeakPtr()));
}

AudioRendererImpl::~AudioRendererImpl()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());

    // If Render() is in progress, this call will wait for Render() to finish.
    // After this call, the |sink_| will not call back into |this| anymore.
    sink_->Stop();

    if (!init_cb_.is_null())
        base::ResetAndReturn(&init_cb_).Run(PIPELINE_ERROR_ABORT);
}

void AudioRendererImpl::StartTicking()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(!rendering_);
    rendering_ = true;

    base::AutoLock auto_lock(lock_);
    // Wait for an eventual call to SetPlaybackRate() to start rendering.
    if (playback_rate_ == 0) {
        DCHECK(!sink_playing_);
        return;
    }

    StartRendering_Locked();
}

void AudioRendererImpl::StartRendering_Locked()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(state_, kPlaying);
    DCHECK(!sink_playing_);
    DCHECK_NE(playback_rate_, 0.0);
    lock_.AssertAcquired();

    sink_playing_ = true;

    base::AutoUnlock auto_unlock(lock_);
    sink_->Play();
}

void AudioRendererImpl::StopTicking()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(rendering_);
    rendering_ = false;

    base::AutoLock auto_lock(lock_);
    // Rendering should have already been stopped with a zero playback rate.
    if (playback_rate_ == 0) {
        DCHECK(!sink_playing_);
        return;
    }

    StopRendering_Locked();
}

void AudioRendererImpl::StopRendering_Locked()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(state_, kPlaying);
    DCHECK(sink_playing_);
    lock_.AssertAcquired();

    sink_playing_ = false;

    base::AutoUnlock auto_unlock(lock_);
    sink_->Pause();
    stop_rendering_time_ = last_render_time_;
}

void AudioRendererImpl::SetMediaTime(base::TimeDelta time)
{
    DVLOG(1) << __FUNCTION__ << "(" << time << ")";
    DCHECK(task_runner_->BelongsToCurrentThread());

    base::AutoLock auto_lock(lock_);
    DCHECK(!rendering_);
    DCHECK_EQ(state_, kFlushed);

    start_timestamp_ = time;
    ended_timestamp_ = kInfiniteDuration();
    last_render_time_ = stop_rendering_time_ = base::TimeTicks();
    first_packet_timestamp_ = kNoTimestamp();
    audio_clock_.reset(new AudioClock(time, audio_parameters_.sample_rate()));
}

base::TimeDelta AudioRendererImpl::CurrentMediaTime()
{
    // In practice the Render() method is called with a high enough frequency
    // that returning only the front timestamp is good enough and also prevents
    // returning values that go backwards in time.
    base::TimeDelta current_media_time;
    {
        base::AutoLock auto_lock(lock_);
        current_media_time = audio_clock_->front_timestamp();
    }

    DVLOG(2) << __FUNCTION__ << ": " << current_media_time;
    return current_media_time;
}

bool AudioRendererImpl::GetWallClockTimes(
    const std::vector<base::TimeDelta>& media_timestamps,
    std::vector<base::TimeTicks>* wall_clock_times)
{
    base::AutoLock auto_lock(lock_);
    DCHECK(wall_clock_times->empty());

    // When playback is paused (rate is zero), assume a rate of 1.0.
    const double playback_rate = playback_rate_ ? playback_rate_ : 1.0;
    const bool is_time_moving = sink_playing_ && playback_rate_ && !last_render_time_.is_null() && stop_rendering_time_.is_null();

    // Pre-compute the time until playback of the audio buffer extents, since
    // these values are frequently used below.
    const base::TimeDelta time_until_front = audio_clock_->TimeUntilPlayback(audio_clock_->front_timestamp());
    const base::TimeDelta time_until_back = audio_clock_->TimeUntilPlayback(audio_clock_->back_timestamp());

    if (media_timestamps.empty()) {
        // Return the current media time as a wall clock time while accounting for
        // frames which may be in the process of play out.
        wall_clock_times->push_back(std::min(
            std::max(tick_clock_->NowTicks(), last_render_time_ + time_until_front),
            last_render_time_ + time_until_back));
        return is_time_moving;
    }

    wall_clock_times->reserve(media_timestamps.size());
    for (const auto& media_timestamp : media_timestamps) {
        // When time was or is moving and the requested media timestamp is within
        // range of played out audio, we can provide an exact conversion.
        if (!last_render_time_.is_null() && media_timestamp >= audio_clock_->front_timestamp() && media_timestamp <= audio_clock_->back_timestamp()) {
            wall_clock_times->push_back(
                last_render_time_ + audio_clock_->TimeUntilPlayback(media_timestamp));
            continue;
        }

        base::TimeDelta base_timestamp, time_until_playback;
        if (media_timestamp < audio_clock_->front_timestamp()) {
            base_timestamp = audio_clock_->front_timestamp();
            time_until_playback = time_until_front;
        } else {
            base_timestamp = audio_clock_->back_timestamp();
            time_until_playback = time_until_back;
        }

        // In practice, most calls will be estimates given the relatively small
        // window in which clients can get the actual time.
        wall_clock_times->push_back(last_render_time_ + time_until_playback + (media_timestamp - base_timestamp) / playback_rate);
    }

    return is_time_moving;
}

TimeSource* AudioRendererImpl::GetTimeSource()
{
    return this;
}

void AudioRendererImpl::Flush(const base::Closure& callback)
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());

    base::AutoLock auto_lock(lock_);
    DCHECK_EQ(state_, kPlaying);
    DCHECK(flush_cb_.is_null());

    flush_cb_ = callback;
    ChangeState_Locked(kFlushing);

    if (pending_read_)
        return;

    ChangeState_Locked(kFlushed);
    DoFlush_Locked();
}

void AudioRendererImpl::DoFlush_Locked()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    lock_.AssertAcquired();

    DCHECK(!pending_read_);
    DCHECK_EQ(state_, kFlushed);

    audio_buffer_stream_->Reset(base::Bind(&AudioRendererImpl::ResetDecoderDone,
        weak_factory_.GetWeakPtr()));
}

void AudioRendererImpl::ResetDecoderDone()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    {
        base::AutoLock auto_lock(lock_);

        DCHECK_EQ(state_, kFlushed);
        DCHECK(!flush_cb_.is_null());

        received_end_of_stream_ = false;
        rendered_end_of_stream_ = false;

        // Flush() may have been called while underflowed/not fully buffered.
        if (buffering_state_ != BUFFERING_HAVE_NOTHING)
            SetBufferingState_Locked(BUFFERING_HAVE_NOTHING);

        splicer_->Reset();
        if (buffer_converter_)
            buffer_converter_->Reset();
        algorithm_->FlushBuffers();
    }

    // Changes in buffering state are always posted. Flush callback must only be
    // run after buffering state has been set back to nothing.
    task_runner_->PostTask(FROM_HERE, base::ResetAndReturn(&flush_cb_));
}

void AudioRendererImpl::StartPlaying()
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());

    base::AutoLock auto_lock(lock_);
    DCHECK(!sink_playing_);
    DCHECK_EQ(state_, kFlushed);
    DCHECK_EQ(buffering_state_, BUFFERING_HAVE_NOTHING);
    DCHECK(!pending_read_) << "Pending read must complete before seeking";

    ChangeState_Locked(kPlaying);
    AttemptRead_Locked();
}

void AudioRendererImpl::Initialize(
    DemuxerStream* stream,
    const PipelineStatusCB& init_cb,
    const SetCdmReadyCB& set_cdm_ready_cb,
    const StatisticsCB& statistics_cb,
    const BufferingStateCB& buffering_state_cb,
    const base::Closure& ended_cb,
    const PipelineStatusCB& error_cb,
    const base::Closure& waiting_for_decryption_key_cb)
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(stream);
    DCHECK_EQ(stream->type(), DemuxerStream::AUDIO);
    DCHECK(!init_cb.is_null());
    DCHECK(!statistics_cb.is_null());
    DCHECK(!buffering_state_cb.is_null());
    DCHECK(!ended_cb.is_null());
    DCHECK(!error_cb.is_null());
    DCHECK_EQ(kUninitialized, state_);
    DCHECK(sink_.get());

    state_ = kInitializing;

    // Always post |init_cb_| because |this| could be destroyed if initialization
    // failed.
    init_cb_ = BindToCurrentLoop(init_cb);

    buffering_state_cb_ = buffering_state_cb;
    ended_cb_ = ended_cb;
    error_cb_ = error_cb;
    statistics_cb_ = statistics_cb;

    const AudioParameters& hw_params = hardware_config_.GetOutputConfig();
    expecting_config_changes_ = stream->SupportsConfigChanges();
    if (!expecting_config_changes_ || !hw_params.IsValid() || hw_params.format() == AudioParameters::AUDIO_FAKE) {
        // The actual buffer size is controlled via the size of the AudioBus
        // provided to Render(), so just choose something reasonable here for looks.
        int buffer_size = stream->audio_decoder_config().samples_per_second() / 100;
        audio_parameters_.Reset(
            AudioParameters::AUDIO_PCM_LOW_LATENCY,
            stream->audio_decoder_config().channel_layout(),
            stream->audio_decoder_config().samples_per_second(),
            stream->audio_decoder_config().bits_per_channel(),
            buffer_size);
        buffer_converter_.reset();
    } else {
        audio_parameters_.Reset(
            hw_params.format(),
            // Always use the source's channel layout to avoid premature downmixing
            // (http://crbug.com/379288), platform specific issues around channel
            // layouts (http://crbug.com/266674), and unnecessary upmixing overhead.
            stream->audio_decoder_config().channel_layout(),
            hw_params.sample_rate(), hw_params.bits_per_sample(),
            hardware_config_.GetHighLatencyBufferSize());
    }

    audio_clock_.reset(
        new AudioClock(base::TimeDelta(), audio_parameters_.sample_rate()));

    audio_buffer_stream_->Initialize(
        stream, base::Bind(&AudioRendererImpl::OnAudioBufferStreamInitialized, weak_factory_.GetWeakPtr()),
        set_cdm_ready_cb, statistics_cb, waiting_for_decryption_key_cb);
}

void AudioRendererImpl::OnAudioBufferStreamInitialized(bool success)
{
    DVLOG(1) << __FUNCTION__ << ": " << success;
    DCHECK(task_runner_->BelongsToCurrentThread());

    base::AutoLock auto_lock(lock_);

    if (!success) {
        state_ = kUninitialized;
        base::ResetAndReturn(&init_cb_).Run(DECODER_ERROR_NOT_SUPPORTED);
        return;
    }

    if (!audio_parameters_.IsValid()) {
        DVLOG(1) << __FUNCTION__ << ": Invalid audio parameters: "
                 << audio_parameters_.AsHumanReadableString();
        ChangeState_Locked(kUninitialized);
        base::ResetAndReturn(&init_cb_).Run(PIPELINE_ERROR_INITIALIZATION_FAILED);
        return;
    }

    if (expecting_config_changes_)
        buffer_converter_.reset(new AudioBufferConverter(audio_parameters_));
    splicer_.reset(new AudioSplicer(audio_parameters_.sample_rate(), media_log_));

    // We're all good! Continue initializing the rest of the audio renderer
    // based on the decoder format.
    algorithm_.reset(new AudioRendererAlgorithm());
    algorithm_->Initialize(audio_parameters_);

    ChangeState_Locked(kFlushed);

    HistogramRendererEvent(INITIALIZED);

    {
        base::AutoUnlock auto_unlock(lock_);
        sink_->Initialize(audio_parameters_, this);
        sink_->Start();

        // Some sinks play on start...
        sink_->Pause();
    }

    DCHECK(!sink_playing_);
    base::ResetAndReturn(&init_cb_).Run(PIPELINE_OK);
}

void AudioRendererImpl::SetVolume(float volume)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(sink_.get());
    sink_->SetVolume(volume);
}

void AudioRendererImpl::DecodedAudioReady(
    AudioBufferStream::Status status,
    const scoped_refptr<AudioBuffer>& buffer)
{
    DVLOG(2) << __FUNCTION__ << "(" << status << ")";
    DCHECK(task_runner_->BelongsToCurrentThread());

    base::AutoLock auto_lock(lock_);
    DCHECK(state_ != kUninitialized);

    CHECK(pending_read_);
    pending_read_ = false;

    if (status == AudioBufferStream::ABORTED || status == AudioBufferStream::DEMUXER_READ_ABORTED) {
        HandleAbortedReadOrDecodeError(false);
        return;
    }

    if (status == AudioBufferStream::DECODE_ERROR) {
        HandleAbortedReadOrDecodeError(true);
        return;
    }

    DCHECK_EQ(status, AudioBufferStream::OK);
    DCHECK(buffer.get());

    if (state_ == kFlushing) {
        ChangeState_Locked(kFlushed);
        DoFlush_Locked();
        return;
    }

    if (expecting_config_changes_) {
        DCHECK(buffer_converter_);
        buffer_converter_->AddInput(buffer);
        while (buffer_converter_->HasNextBuffer()) {
            if (!splicer_->AddInput(buffer_converter_->GetNextBuffer())) {
                HandleAbortedReadOrDecodeError(true);
                return;
            }
        }
    } else {
        if (!splicer_->AddInput(buffer)) {
            HandleAbortedReadOrDecodeError(true);
            return;
        }
    }

    if (!splicer_->HasNextBuffer()) {
        AttemptRead_Locked();
        return;
    }

    bool need_another_buffer = false;
    while (splicer_->HasNextBuffer())
        need_another_buffer = HandleSplicerBuffer_Locked(splicer_->GetNextBuffer());

    if (!need_another_buffer && !CanRead_Locked())
        return;

    AttemptRead_Locked();
}

bool AudioRendererImpl::HandleSplicerBuffer_Locked(
    const scoped_refptr<AudioBuffer>& buffer)
{
    lock_.AssertAcquired();
    if (buffer->end_of_stream()) {
        received_end_of_stream_ = true;
    } else {
        if (state_ == kPlaying) {
            if (IsBeforeStartTime(buffer))
                return true;

            // Trim off any additional time before the start timestamp.
            const base::TimeDelta trim_time = start_timestamp_ - buffer->timestamp();
            if (trim_time > base::TimeDelta()) {
                buffer->TrimStart(buffer->frame_count() * (static_cast<double>(trim_time.InMicroseconds()) / buffer->duration().InMicroseconds()));
            }
            // If the entire buffer was trimmed, request a new one.
            if (!buffer->frame_count())
                return true;
        }

        if (state_ != kUninitialized)
            algorithm_->EnqueueBuffer(buffer);
    }

    // Store the timestamp of the first packet so we know when to start actual
    // audio playback.
    if (first_packet_timestamp_ == kNoTimestamp())
        first_packet_timestamp_ = buffer->timestamp();

    const size_t memory_usage = algorithm_->GetMemoryUsage();
    PipelineStatistics stats;
    stats.audio_memory_usage = memory_usage - last_audio_memory_usage_;
    last_audio_memory_usage_ = memory_usage;
    task_runner_->PostTask(FROM_HERE, base::Bind(statistics_cb_, stats));

    switch (state_) {
    case kUninitialized:
    case kInitializing:
    case kFlushing:
        NOTREACHED();
        return false;

    case kFlushed:
        DCHECK(!pending_read_);
        return false;

    case kPlaying:
        if (buffer->end_of_stream() || algorithm_->IsQueueFull()) {
            if (buffering_state_ == BUFFERING_HAVE_NOTHING)
                SetBufferingState_Locked(BUFFERING_HAVE_ENOUGH);
            return false;
        }
        return true;
    }
    return false;
}

void AudioRendererImpl::AttemptRead()
{
    base::AutoLock auto_lock(lock_);
    AttemptRead_Locked();
}

void AudioRendererImpl::AttemptRead_Locked()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    lock_.AssertAcquired();

    if (!CanRead_Locked())
        return;

    pending_read_ = true;
    audio_buffer_stream_->Read(base::Bind(&AudioRendererImpl::DecodedAudioReady,
        weak_factory_.GetWeakPtr()));
}

bool AudioRendererImpl::CanRead_Locked()
{
    lock_.AssertAcquired();

    switch (state_) {
    case kUninitialized:
    case kInitializing:
    case kFlushing:
    case kFlushed:
        return false;

    case kPlaying:
        break;
    }

    return !pending_read_ && !received_end_of_stream_ && !algorithm_->IsQueueFull();
}

void AudioRendererImpl::SetPlaybackRate(double playback_rate)
{
    DVLOG(1) << __FUNCTION__ << "(" << playback_rate << ")";
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_GE(playback_rate, 0);
    DCHECK(sink_.get());

    base::AutoLock auto_lock(lock_);

    // We have two cases here:
    // Play: current_playback_rate == 0 && playback_rate != 0
    // Pause: current_playback_rate != 0 && playback_rate == 0
    double current_playback_rate = playback_rate_;
    playback_rate_ = playback_rate;

    if (!rendering_)
        return;

    if (current_playback_rate == 0 && playback_rate != 0) {
        StartRendering_Locked();
        return;
    }

    if (current_playback_rate != 0 && playback_rate == 0) {
        StopRendering_Locked();
        return;
    }
}

bool AudioRendererImpl::IsBeforeStartTime(
    const scoped_refptr<AudioBuffer>& buffer)
{
    DCHECK_EQ(state_, kPlaying);
    return buffer.get() && !buffer->end_of_stream() && (buffer->timestamp() + buffer->duration()) < start_timestamp_;
}

int AudioRendererImpl::Render(AudioBus* audio_bus,
    int audio_delay_milliseconds)
{
    const int requested_frames = audio_bus->frames();
    base::TimeDelta playback_delay = base::TimeDelta::FromMilliseconds(
        audio_delay_milliseconds);
    const int delay_frames = static_cast<int>(playback_delay.InSecondsF() * audio_parameters_.sample_rate());
    int frames_written = 0;
    {
        base::AutoLock auto_lock(lock_);
        last_render_time_ = tick_clock_->NowTicks();

        if (!stop_rendering_time_.is_null()) {
            audio_clock_->CompensateForSuspendedWrites(
                last_render_time_ - stop_rendering_time_, delay_frames);
            stop_rendering_time_ = base::TimeTicks();
        }

        // Ensure Stop() hasn't destroyed our |algorithm_| on the pipeline thread.
        if (!algorithm_) {
            audio_clock_->WroteAudio(
                0, requested_frames, delay_frames, playback_rate_);
            return 0;
        }

        if (playback_rate_ == 0) {
            audio_clock_->WroteAudio(
                0, requested_frames, delay_frames, playback_rate_);
            return 0;
        }

        // Mute audio by returning 0 when not playing.
        if (state_ != kPlaying) {
            audio_clock_->WroteAudio(
                0, requested_frames, delay_frames, playback_rate_);
            return 0;
        }

        // Delay playback by writing silence if we haven't reached the first
        // timestamp yet; this can occur if the video starts before the audio.
        if (algorithm_->frames_buffered() > 0) {
            DCHECK(first_packet_timestamp_ != kNoTimestamp());
            const base::TimeDelta play_delay = first_packet_timestamp_ - audio_clock_->back_timestamp();
            if (play_delay > base::TimeDelta()) {
                DCHECK_EQ(frames_written, 0);
                frames_written = std::min(static_cast<int>(play_delay.InSecondsF() * audio_parameters_.sample_rate()),
                    requested_frames);
                audio_bus->ZeroFramesPartial(0, frames_written);
            }

            // If there's any space left, actually render the audio; this is where the
            // aural magic happens.
            if (frames_written < requested_frames) {
                frames_written += algorithm_->FillBuffer(
                    audio_bus, frames_written, requested_frames - frames_written,
                    playback_rate_);
            }
        }

        // We use the following conditions to determine end of playback:
        //   1) Algorithm can not fill the audio callback buffer
        //   2) We received an end of stream buffer
        //   3) We haven't already signalled that we've ended
        //   4) We've played all known audio data sent to hardware
        //
        // We use the following conditions to determine underflow:
        //   1) Algorithm can not fill the audio callback buffer
        //   2) We have NOT received an end of stream buffer
        //   3) We are in the kPlaying state
        //
        // Otherwise the buffer has data we can send to the device.
        //
        // Per the TimeSource API the media time should always increase even after
        // we've rendered all known audio data. Doing so simplifies scenarios where
        // we have other sources of media data that need to be scheduled after audio
        // data has ended.
        //
        // That being said, we don't want to advance time when underflowed as we
        // know more decoded frames will eventually arrive. If we did, we would
        // throw things out of sync when said decoded frames arrive.
        int frames_after_end_of_stream = 0;
        if (frames_written == 0) {
            if (received_end_of_stream_) {
                if (ended_timestamp_ == kInfiniteDuration())
                    ended_timestamp_ = audio_clock_->back_timestamp();
                frames_after_end_of_stream = requested_frames;
            } else if (state_ == kPlaying && buffering_state_ != BUFFERING_HAVE_NOTHING) {
                algorithm_->IncreaseQueueCapacity();
                SetBufferingState_Locked(BUFFERING_HAVE_NOTHING);
            }
        }

        audio_clock_->WroteAudio(frames_written + frames_after_end_of_stream,
            requested_frames,
            delay_frames,
            playback_rate_);

        if (CanRead_Locked()) {
            task_runner_->PostTask(FROM_HERE,
                base::Bind(&AudioRendererImpl::AttemptRead,
                    weak_factory_.GetWeakPtr()));
        }

        if (audio_clock_->front_timestamp() >= ended_timestamp_ && !rendered_end_of_stream_) {
            rendered_end_of_stream_ = true;
            task_runner_->PostTask(FROM_HERE, ended_cb_);
        }
    }

    DCHECK_LE(frames_written, requested_frames);
    return frames_written;
}

void AudioRendererImpl::OnRenderError()
{
    // UMA data tells us this happens ~0.01% of the time. Trigger an error instead
    // of trying to gracefully fall back to a fake sink. It's very likely
    // OnRenderError() should be removed and the audio stack handle errors without
    // notifying clients. See http://crbug.com/234708 for details.
    HistogramRendererEvent(RENDER_ERROR);

    MEDIA_LOG(ERROR, media_log_) << "audio render error";

    // Post to |task_runner_| as this is called on the audio callback thread.
    task_runner_->PostTask(FROM_HERE,
        base::Bind(error_cb_, PIPELINE_ERROR_DECODE));
}

void AudioRendererImpl::HandleAbortedReadOrDecodeError(bool is_decode_error)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    lock_.AssertAcquired();

    PipelineStatus status = is_decode_error ? PIPELINE_ERROR_DECODE : PIPELINE_OK;
    switch (state_) {
    case kUninitialized:
    case kInitializing:
        NOTREACHED();
        return;
    case kFlushing:
        ChangeState_Locked(kFlushed);
        if (status == PIPELINE_OK) {
            DoFlush_Locked();
            return;
        }

        MEDIA_LOG(ERROR, media_log_) << "audio decode error during flushing";
        error_cb_.Run(status);
        base::ResetAndReturn(&flush_cb_).Run();
        return;

    case kFlushed:
    case kPlaying:
        if (status != PIPELINE_OK) {
            MEDIA_LOG(ERROR, media_log_) << "audio decode error during playing";
            error_cb_.Run(status);
        }
        return;
    }
}

void AudioRendererImpl::ChangeState_Locked(State new_state)
{
    DVLOG(1) << __FUNCTION__ << " : " << state_ << " -> " << new_state;
    lock_.AssertAcquired();
    state_ = new_state;
}

void AudioRendererImpl::OnNewSpliceBuffer(base::TimeDelta splice_timestamp)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    splicer_->SetSpliceTimestamp(splice_timestamp);
}

void AudioRendererImpl::OnConfigChange()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK(expecting_config_changes_);
    buffer_converter_->ResetTimestampState();
    // Drain flushed buffers from the converter so the AudioSplicer receives all
    // data ahead of any OnNewSpliceBuffer() calls.  Since discontinuities should
    // only appear after config changes, AddInput() should never fail here.
    while (buffer_converter_->HasNextBuffer())
        CHECK(splicer_->AddInput(buffer_converter_->GetNextBuffer()));
}

void AudioRendererImpl::SetBufferingState_Locked(
    BufferingState buffering_state)
{
    DVLOG(1) << __FUNCTION__ << " : " << buffering_state_ << " -> "
             << buffering_state;
    DCHECK_NE(buffering_state_, buffering_state);
    lock_.AssertAcquired();
    buffering_state_ = buffering_state;

    task_runner_->PostTask(FROM_HERE,
        base::Bind(buffering_state_cb_, buffering_state_));
}

} // namespace media
