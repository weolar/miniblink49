// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/renderers/video_renderer_impl.h"

#include "base/bind.h"
#include "base/callback.h"
#include "base/callback_helpers.h"
#include "base/location.h"
#include "base/metrics/histogram_macros.h"
#include "base/single_thread_task_runner.h"
#include "base/strings/string_util.h"
#include "base/time/default_tick_clock.h"
#include "base/trace_event/trace_event.h"
#include "media/base/bind_to_current_loop.h"
#include "media/base/limits.h"
#include "media/base/media_log.h"
#include "media/base/media_switches.h"
#include "media/base/pipeline.h"
#include "media/base/video_frame.h"
#include "media/renderers/gpu_video_accelerator_factories.h"
#include "media/video/gpu_memory_buffer_video_frame_pool.h"

namespace media {

VideoRendererImpl::VideoRendererImpl(
    const scoped_refptr<base::SingleThreadTaskRunner>& media_task_runner,
    const scoped_refptr<base::TaskRunner>& worker_task_runner,
    VideoRendererSink* sink,
    ScopedVector<VideoDecoder> decoders,
    bool drop_frames,
    GpuVideoAcceleratorFactories* gpu_factories,
    const scoped_refptr<MediaLog>& media_log)
    : task_runner_(media_task_runner)
    , sink_(sink)
    , sink_started_(false)
    , video_frame_stream_(
          new VideoFrameStream(media_task_runner, decoders.Pass(), media_log))
    , gpu_memory_buffer_pool_(nullptr)
    , media_log_(media_log)
    , low_delay_(false)
    , received_end_of_stream_(false)
    , rendered_end_of_stream_(false)
    , state_(kUninitialized)
    , sequence_token_(0)
    , pending_read_(false)
    , drop_frames_(drop_frames)
    , buffering_state_(BUFFERING_HAVE_NOTHING)
    , frames_decoded_(0)
    , frames_dropped_(0)
    , tick_clock_(new base::DefaultTickClock())
    , was_background_rendering_(false)
    , time_progressing_(false)
    , render_first_frame_and_stop_(false)
    , posted_maybe_stop_after_first_paint_(false)
    , last_video_memory_usage_(0)
    , weak_factory_(this)
{
    if (gpu_factories && gpu_factories->ShouldUseGpuMemoryBuffersForVideoFrames()) {
        //         gpu_memory_buffer_pool_.reset(new GpuMemoryBufferVideoFramePool(
        //             media_task_runner, worker_task_runner, gpu_factories));
        DebugBreak();
    }
}

VideoRendererImpl::~VideoRendererImpl()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (!init_cb_.is_null())
        base::ResetAndReturn(&init_cb_).Run(PIPELINE_ERROR_ABORT);

    if (!flush_cb_.is_null())
        base::ResetAndReturn(&flush_cb_).Run();

    if (sink_started_)
        StopSink();
}

void VideoRendererImpl::Flush(const base::Closure& callback)
{
    DVLOG(1) << __FUNCTION__;
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (sink_started_)
        StopSink();

    base::AutoLock auto_lock(lock_);
    DCHECK_EQ(state_, kPlaying);
    flush_cb_ = callback;
    state_ = kFlushing;

    // This is necessary if the |video_frame_stream_| has already seen an end of
    // stream and needs to drain it before flushing it.
    if (buffering_state_ != BUFFERING_HAVE_NOTHING) {
        buffering_state_ = BUFFERING_HAVE_NOTHING;
        buffering_state_cb_.Run(BUFFERING_HAVE_NOTHING);
    }
    received_end_of_stream_ = false;
    rendered_end_of_stream_ = false;

    algorithm_->Reset();

    video_frame_stream_->Reset(
        base::Bind(&VideoRendererImpl::OnVideoFrameStreamResetDone,
            weak_factory_.GetWeakPtr()));
}

void VideoRendererImpl::StartPlayingFrom(base::TimeDelta timestamp)
{
    DVLOG(1) << __FUNCTION__ << "(" << timestamp.InMicroseconds() << ")";
    DCHECK(task_runner_->BelongsToCurrentThread());
    base::AutoLock auto_lock(lock_);
    DCHECK_EQ(state_, kFlushed);
    DCHECK(!pending_read_);
    DCHECK_EQ(buffering_state_, BUFFERING_HAVE_NOTHING);

    state_ = kPlaying;
    start_timestamp_ = timestamp;
    AttemptRead_Locked();
}

void VideoRendererImpl::Initialize(
    DemuxerStream* stream,
    const PipelineStatusCB& init_cb,
    const SetCdmReadyCB& set_cdm_ready_cb,
    const StatisticsCB& statistics_cb,
    const BufferingStateCB& buffering_state_cb,
    const base::Closure& ended_cb,
    const PipelineStatusCB& error_cb,
    const TimeSource::WallClockTimeCB& wall_clock_time_cb,
    const base::Closure& waiting_for_decryption_key_cb)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    base::AutoLock auto_lock(lock_);
    DCHECK(stream);
    DCHECK_EQ(stream->type(), DemuxerStream::VIDEO);
    DCHECK(!init_cb.is_null());
    DCHECK(!statistics_cb.is_null());
    DCHECK(!buffering_state_cb.is_null());
    DCHECK(!ended_cb.is_null());
    DCHECK(!wall_clock_time_cb.is_null());
    DCHECK_EQ(kUninitialized, state_);
    DCHECK(!render_first_frame_and_stop_);
    DCHECK(!posted_maybe_stop_after_first_paint_);
    DCHECK(!was_background_rendering_);
    DCHECK(!time_progressing_);

    low_delay_ = (stream->liveness() == DemuxerStream::LIVENESS_LIVE);
    UMA_HISTOGRAM_BOOLEAN("Media.VideoRenderer.LowDelay", low_delay_);
    if (low_delay_)
        MEDIA_LOG(DEBUG, media_log_) << "Video rendering in low delay mode.";

    // Always post |init_cb_| because |this| could be destroyed if initialization
    // failed.
    init_cb_ = BindToCurrentLoop(init_cb);

    // Always post |buffering_state_cb_| because it may otherwise invoke reentrant
    // calls to OnTimeStateChanged() under lock, which can deadlock the compositor
    // and media threads.
    buffering_state_cb_ = BindToCurrentLoop(buffering_state_cb);

    statistics_cb_ = statistics_cb;
    ended_cb_ = ended_cb;
    error_cb_ = error_cb;
    wall_clock_time_cb_ = wall_clock_time_cb;
    state_ = kInitializing;

    video_frame_stream_->Initialize(
        stream, base::Bind(&VideoRendererImpl::OnVideoFrameStreamInitialized, weak_factory_.GetWeakPtr()),
        set_cdm_ready_cb, statistics_cb, waiting_for_decryption_key_cb);
}

scoped_refptr<VideoFrame> VideoRendererImpl::Render(
    base::TimeTicks deadline_min,
    base::TimeTicks deadline_max,
    bool background_rendering)
{
    base::AutoLock auto_lock(lock_);
    DCHECK_EQ(state_, kPlaying);

    size_t frames_dropped = 0;
    scoped_refptr<VideoFrame> result = algorithm_->Render(deadline_min, deadline_max, &frames_dropped);

    // Due to how the |algorithm_| holds frames, this should never be null if
    // we've had a proper startup sequence.
    DCHECK(result);

    // Declare HAVE_NOTHING if we reach a state where we can't progress playback
    // any further.  We don't want to do this if we've already done so, reached
    // end of stream, or have frames available.  We also don't want to do this in
    // background rendering mode unless this isn't the first background render
    // tick and we haven't seen any decoded frames since the last one.
    //
    // We use the inverse of |render_first_frame_and_stop_| as a proxy for the
    // value of |time_progressing_| here since we can't access it from the
    // compositor thread.  If we're here (in Render()) the sink must have been
    // started -- but if it was started only to render the first frame and stop,
    // then |time_progressing_| is likely false.  If we're still in Render() when
    // |render_first_frame_and_stop_| is false, then |time_progressing_| is true.
    // If |time_progressing_| is actually true when |render_first_frame_and_stop_|
    // is also true, then the ended callback will be harmlessly delayed until
    // MaybeStopSinkAfterFirstPaint() runs and the next Render() call comes in.
    const size_t effective_frames = MaybeFireEndedCallback_Locked(!render_first_frame_and_stop_);
    if (buffering_state_ == BUFFERING_HAVE_ENOUGH && !received_end_of_stream_ && !effective_frames && (!background_rendering || (!frames_decoded_ && was_background_rendering_))) {
        // Do not set |buffering_state_| here as the lock in FrameReady() may be
        // held already and it fire the state changes in the wrong order.
        task_runner_->PostTask(
            FROM_HERE, base::Bind(&VideoRendererImpl::TransitionToHaveNothing, weak_factory_.GetWeakPtr()));
    }

    // We don't count dropped frames in the background to avoid skewing the count
    // and impacting JavaScript visible metrics used by web developers.
    //
    // Just after resuming from background rendering, we also don't count the
    // dropped frames since they are likely just dropped due to being too old.
    if (!background_rendering && !was_background_rendering_)
        frames_dropped_ += frames_dropped;
    UpdateStats_Locked();
    was_background_rendering_ = background_rendering;

    // After painting the first frame, if playback hasn't started, we post a
    // delayed task to request that the sink be stopped.  The task is delayed to
    // give videos with autoplay time to start.
    //
    // OnTimeStateChanged() will clear this flag if time starts before we get here
    // and MaybeStopSinkAfterFirstPaint() will ignore this request if time starts
    // before the call executes.
    if (render_first_frame_and_stop_ && !posted_maybe_stop_after_first_paint_) {
        posted_maybe_stop_after_first_paint_ = true;
        task_runner_->PostDelayedTask(
            FROM_HERE, base::Bind(&VideoRendererImpl::MaybeStopSinkAfterFirstPaint, weak_factory_.GetWeakPtr()),
            base::TimeDelta::FromMilliseconds(250));
    }

    // Always post this task, it will acquire new frames if necessary and since it
    // happens on another thread, even if we don't have room in the queue now, by
    // the time it runs (may be delayed up to 50ms for complex decodes!) we might.
    task_runner_->PostTask(FROM_HERE, base::Bind(&VideoRendererImpl::AttemptRead, weak_factory_.GetWeakPtr()));

    return result;
}

void VideoRendererImpl::OnFrameDropped()
{
    base::AutoLock auto_lock(lock_);
    algorithm_->OnLastFrameDropped();
}

void VideoRendererImpl::OnVideoFrameStreamInitialized(bool success)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    base::AutoLock auto_lock(lock_);
    DCHECK_EQ(state_, kInitializing);

    if (!success) {
        state_ = kUninitialized;
        base::ResetAndReturn(&init_cb_).Run(DECODER_ERROR_NOT_SUPPORTED);
        return;
    }

    // We're all good!  Consider ourselves flushed. (ThreadMain() should never
    // see us in the kUninitialized state).
    // Since we had an initial Preroll(), we consider ourself flushed, because we
    // have not populated any buffers yet.
    state_ = kFlushed;

    algorithm_.reset(new VideoRendererAlgorithm(wall_clock_time_cb_));
    if (!drop_frames_)
        algorithm_->disable_frame_dropping();

    base::ResetAndReturn(&init_cb_).Run(PIPELINE_OK);
}

void VideoRendererImpl::SetTickClockForTesting(
    scoped_ptr<base::TickClock> tick_clock)
{
    tick_clock_.swap(tick_clock);
}

void VideoRendererImpl::SetGpuMemoryBufferVideoForTesting(
    scoped_ptr<GpuMemoryBufferVideoFramePool> gpu_memory_buffer_pool)
{
    gpu_memory_buffer_pool_.swap(gpu_memory_buffer_pool);
}

void VideoRendererImpl::OnTimeStateChanged(bool time_progressing)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    time_progressing_ = time_progressing;

    // WARNING: Do not attempt to use |lock_| here as this may be a reentrant call
    // in response to callbacks firing above.

    if (sink_started_ == time_progressing_)
        return;

    if (time_progressing_) {
        // If only an EOS frame came in after a seek, the renderer may not have
        // received the ended event yet though we've posted it.
        if (!rendered_end_of_stream_)
            StartSink();
    } else {
        StopSink();
    }
}

void VideoRendererImpl::FrameReadyForCopyingToGpuMemoryBuffers(
    VideoFrameStream::Status status,
    const scoped_refptr<VideoFrame>& frame)
{
    if (status != VideoFrameStream::OK || start_timestamp_ > frame->timestamp()) {
        VideoRendererImpl::FrameReady(sequence_token_, status, frame);
        return;
    }

    DCHECK(frame);
    gpu_memory_buffer_pool_->MaybeCreateHardwareFrame(
        frame, base::Bind(&VideoRendererImpl::FrameReady, weak_factory_.GetWeakPtr(), sequence_token_, status));
}

void VideoRendererImpl::FrameReady(uint32_t sequence_token,
    VideoFrameStream::Status status,
    const scoped_refptr<VideoFrame>& frame)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    bool start_sink = false;
    {
        base::AutoLock auto_lock(lock_);
        // Stream has been reset and this VideoFrame was decoded before the reset
        // but the async copy finished after.
        if (sequence_token != sequence_token_)
            return;

        DCHECK_NE(state_, kUninitialized);
        DCHECK_NE(state_, kFlushed);

        CHECK(pending_read_);
        pending_read_ = false;

        if (status == VideoFrameStream::DECODE_ERROR) {
            DCHECK(!frame.get());
            PipelineStatus error = PIPELINE_ERROR_DECODE;
            task_runner_->PostTask(FROM_HERE, base::Bind(error_cb_, error));
            return;
        }

        // Already-queued VideoFrameStream ReadCB's can fire after various state
        // transitions have happened; in that case just drop those frames
        // immediately.
        if (state_ == kFlushing)
            return;

        DCHECK_EQ(state_, kPlaying);

        // Can happen when demuxers are preparing for a new Seek().
        if (!frame.get()) {
            DCHECK_EQ(status, VideoFrameStream::DEMUXER_READ_ABORTED);
            return;
        }

        // In low delay mode, don't accumulate frames that's earlier than the start
        // time. Otherwise we could declare HAVE_ENOUGH_DATA and start playback
        // prematurely.
        if (low_delay_ && !frame->metadata()->IsTrue(VideoFrameMetadata::END_OF_STREAM) && frame->timestamp() < start_timestamp_) {
            AttemptRead_Locked();
            return;
        }

        if (frame->metadata()->IsTrue(VideoFrameMetadata::END_OF_STREAM)) {
            DCHECK(!received_end_of_stream_);
            received_end_of_stream_ = true;

            // See if we can fire EOS immediately instead of waiting for Render().
            MaybeFireEndedCallback_Locked(time_progressing_);
        } else {
            // Maintain the latest frame decoded so the correct frame is displayed
            // after prerolling has completed.
            if (frame->timestamp() <= start_timestamp_) {
                algorithm_->Reset();
            }
            AddReadyFrame_Locked(frame);
        }

        // Background rendering updates may not be ticking fast enough by itself to
        // remove expired frames, so give it a boost here by ensuring we don't exit
        // the decoding cycle too early.
        //
        // Similarly, if we've paused for underflow, remove all frames which are
        // before the current media time.
        const bool have_nothing = buffering_state_ != BUFFERING_HAVE_ENOUGH;
        const bool have_nothing_and_paused = have_nothing && !sink_started_;
        if (was_background_rendering_ || (have_nothing_and_paused && drop_frames_)) {
            base::TimeTicks expiry_time;
            if (have_nothing_and_paused) {
                // Use the current media wall clock time plus the frame duration since
                // RemoveExpiredFrames() is expecting the end point of an interval (it
                // will subtract from the given value).
                std::vector<base::TimeTicks> current_time;
                wall_clock_time_cb_.Run(std::vector<base::TimeDelta>(), &current_time);
                expiry_time = current_time[0] + algorithm_->average_frame_duration();
            } else {
                expiry_time = tick_clock_->NowTicks();
            }

            // Prior to rendering the first frame, |have_nothing_and_paused| will be
            // true, correspondingly the |expiry_time| will be null; in this case
            // there's no reason to try and remove any frames.
            if (!expiry_time.is_null()) {
                const size_t removed_frames = algorithm_->RemoveExpiredFrames(expiry_time);

                // Frames removed during underflow should be counted as dropped.
                if (have_nothing_and_paused && removed_frames)
                    frames_dropped_ += removed_frames;
            }
        }

        // Signal buffering state if we've met our conditions for having enough
        // data.
        if (have_nothing && HaveEnoughData_Locked()) {
            TransitionToHaveEnough_Locked();
            if (!sink_started_ && !rendered_end_of_stream_) {
                start_sink = true;
                render_first_frame_and_stop_ = true;
                posted_maybe_stop_after_first_paint_ = false;
            }
        }

        // Always request more decoded video if we have capacity. This serves two
        // purposes:
        //   1) Prerolling while paused
        //   2) Keeps decoding going if video rendering thread starts falling behind
        AttemptRead_Locked();
    }

    // If time is progressing, the sink has already been started; this may be true
    // if we have previously underflowed, yet weren't stopped because of audio.
    if (start_sink) {
        DCHECK(!sink_started_);
        StartSink();
    }
}

bool VideoRendererImpl::HaveEnoughData_Locked()
{
    DCHECK_EQ(state_, kPlaying);

    if (received_end_of_stream_ || !video_frame_stream_->CanReadWithoutStalling())
        return true;

    if (HaveReachedBufferingCap())
        return true;

    if (was_background_rendering_ && frames_decoded_) {
        return true;
    }

    if (!low_delay_)
        return false;

    return algorithm_->frames_queued() > 0;
}

void VideoRendererImpl::TransitionToHaveEnough_Locked()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_EQ(buffering_state_, BUFFERING_HAVE_NOTHING);

    buffering_state_ = BUFFERING_HAVE_ENOUGH;
    buffering_state_cb_.Run(BUFFERING_HAVE_ENOUGH);
}

void VideoRendererImpl::TransitionToHaveNothing()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    base::AutoLock auto_lock(lock_);
    if (buffering_state_ != BUFFERING_HAVE_ENOUGH || HaveEnoughData_Locked())
        return;

    buffering_state_ = BUFFERING_HAVE_NOTHING;
    buffering_state_cb_.Run(BUFFERING_HAVE_NOTHING);
}

void VideoRendererImpl::AddReadyFrame_Locked(
    const scoped_refptr<VideoFrame>& frame)
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    lock_.AssertAcquired();
    DCHECK(!frame->metadata()->IsTrue(VideoFrameMetadata::END_OF_STREAM));

    frames_decoded_++;

    algorithm_->EnqueueFrame(frame);
}

void VideoRendererImpl::AttemptRead()
{
    base::AutoLock auto_lock(lock_);
    AttemptRead_Locked();
}

void VideoRendererImpl::AttemptRead_Locked()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    lock_.AssertAcquired();

    if (pending_read_ || received_end_of_stream_)
        return;

    if (HaveReachedBufferingCap())
        return;

    switch (state_) {
    case kPlaying:
        pending_read_ = true;
        if (gpu_memory_buffer_pool_) {
            video_frame_stream_->Read(base::Bind(
                &VideoRendererImpl::FrameReadyForCopyingToGpuMemoryBuffers,
                weak_factory_.GetWeakPtr()));
        } else {
            video_frame_stream_->Read(base::Bind(&VideoRendererImpl::FrameReady,
                weak_factory_.GetWeakPtr(),
                sequence_token_));
        }
        return;
    case kUninitialized:
    case kInitializing:
    case kFlushing:
    case kFlushed:
        return;
    }
}

void VideoRendererImpl::OnVideoFrameStreamResetDone()
{
    base::AutoLock auto_lock(lock_);
    DCHECK_EQ(kFlushing, state_);
    DCHECK(!received_end_of_stream_);
    DCHECK(!rendered_end_of_stream_);
    DCHECK_EQ(buffering_state_, BUFFERING_HAVE_NOTHING);

    // Pending read might be true if an async video frame copy is in flight.
    pending_read_ = false;
    sequence_token_++;
    state_ = kFlushed;
    base::ResetAndReturn(&flush_cb_).Run();
}

void VideoRendererImpl::UpdateStats_Locked()
{
    lock_.AssertAcquired();
    DCHECK_GE(frames_decoded_, 0);
    DCHECK_GE(frames_dropped_, 0);

    if (frames_decoded_ || frames_dropped_) {
        PipelineStatistics statistics;
        statistics.video_frames_decoded = frames_decoded_;
        statistics.video_frames_dropped = frames_dropped_;

        const size_t memory_usage = algorithm_->GetMemoryUsage();
        statistics.video_memory_usage = memory_usage - last_video_memory_usage_;

        task_runner_->PostTask(FROM_HERE, base::Bind(statistics_cb_, statistics));
        frames_decoded_ = 0;
        frames_dropped_ = 0;
        last_video_memory_usage_ = memory_usage;
    }
}

void VideoRendererImpl::MaybeStopSinkAfterFirstPaint()
{
    DCHECK(task_runner_->BelongsToCurrentThread());

    if (!time_progressing_ && sink_started_)
        StopSink();

    base::AutoLock auto_lock(lock_);
    render_first_frame_and_stop_ = false;
}

bool VideoRendererImpl::HaveReachedBufferingCap()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    const size_t kMaxVideoFrames = limits::kMaxVideoFrames;

    // When the display rate is less than the frame rate, the effective frames
    // queued may be much smaller than the actual number of frames queued.  Here
    // we ensure that frames_queued() doesn't get excessive.
    return algorithm_->EffectiveFramesQueued() >= kMaxVideoFrames || algorithm_->frames_queued() >= 3 * kMaxVideoFrames;
}

void VideoRendererImpl::StartSink()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    DCHECK_GT(algorithm_->frames_queued(), 0u);
    sink_started_ = true;
    was_background_rendering_ = false;
    sink_->Start(this);
}

void VideoRendererImpl::StopSink()
{
    DCHECK(task_runner_->BelongsToCurrentThread());
    sink_->Stop();
    algorithm_->set_time_stopped();
    sink_started_ = false;
    was_background_rendering_ = false;
}

size_t VideoRendererImpl::MaybeFireEndedCallback_Locked(bool time_progressing)
{
    lock_.AssertAcquired();

    // If there's only one frame in the video or Render() was never called, the
    // algorithm will have one frame linger indefinitely.  So in cases where the
    // frame duration is unknown and we've received EOS, fire it once we get down
    // to a single frame.
    const size_t effective_frames = algorithm_->EffectiveFramesQueued();

    // Don't fire ended if we haven't received EOS or have already done so.
    if (!received_end_of_stream_ || rendered_end_of_stream_)
        return effective_frames;

    // Don't fire ended if time isn't moving and we have frames.
    if (!time_progressing && algorithm_->frames_queued())
        return effective_frames;

    // Fire ended if we have no more effective frames or only ever had one frame.
    if (!effective_frames || (algorithm_->frames_queued() == 1u && algorithm_->average_frame_duration() == base::TimeDelta())) {
        rendered_end_of_stream_ = true;
        task_runner_->PostTask(FROM_HERE, ended_cb_);
    }

    return effective_frames;
}

base::TimeTicks VideoRendererImpl::ConvertMediaTimestamp(
    base::TimeDelta media_time)
{
    std::vector<base::TimeDelta> media_times(1, media_time);
    std::vector<base::TimeTicks> wall_clock_times;
    if (!wall_clock_time_cb_.Run(media_times, &wall_clock_times))
        return base::TimeTicks();
    return wall_clock_times[0];
}

} // namespace media
