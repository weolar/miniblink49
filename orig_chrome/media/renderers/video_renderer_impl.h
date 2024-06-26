// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_RENDERERS_VIDEO_RENDERER_IMPL_H_
#define MEDIA_RENDERERS_VIDEO_RENDERER_IMPL_H_

#include <deque>

#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/scoped_vector.h"
#include "base/memory/weak_ptr.h"
#include "base/synchronization/condition_variable.h"
#include "base/synchronization/lock.h"
#include "base/timer/timer.h"
#include "media/base/decryptor.h"
#include "media/base/demuxer_stream.h"
#include "media/base/media_log.h"
#include "media/base/pipeline_status.h"
#include "media/base/video_decoder.h"
#include "media/base/video_frame.h"
#include "media/base/video_renderer.h"
#include "media/base/video_renderer_sink.h"
#include "media/filters/decoder_stream.h"
#include "media/filters/video_renderer_algorithm.h"
#include "media/renderers/gpu_video_accelerator_factories.h"
#include "media/video/gpu_memory_buffer_video_frame_pool.h"

namespace base {
class SingleThreadTaskRunner;
class TickClock;
}

namespace media {

// VideoRendererImpl creates its own thread for the sole purpose of timing frame
// presentation.  It handles reading from the VideoFrameStream and stores the
// results in a queue of decoded frames and executing a callback when a frame is
// ready for rendering.
class MEDIA_EXPORT VideoRendererImpl
    : public VideoRenderer,
      public NON_EXPORTED_BASE(VideoRendererSink::RenderCallback) {
public:
    // |decoders| contains the VideoDecoders to use when initializing.
    //
    // Implementors should avoid doing any sort of heavy work in this method and
    // instead post a task to a common/worker thread to handle rendering.  Slowing
    // down the video thread may result in losing synchronization with audio.
    //
    // Setting |drop_frames_| to true causes the renderer to drop expired frames.
    VideoRendererImpl(
        const scoped_refptr<base::SingleThreadTaskRunner>& media_task_runner,
        const scoped_refptr<base::TaskRunner>& worker_task_runner,
        VideoRendererSink* sink,
        ScopedVector<VideoDecoder> decoders,
        bool drop_frames,
        GpuVideoAcceleratorFactories* gpu_factories,
        const scoped_refptr<MediaLog>& media_log);
    ~VideoRendererImpl() override;

    // VideoRenderer implementation.
    void Initialize(DemuxerStream* stream,
        const PipelineStatusCB& init_cb,
        const SetCdmReadyCB& set_cdm_ready_cb,
        const StatisticsCB& statistics_cb,
        const BufferingStateCB& buffering_state_cb,
        const base::Closure& ended_cb,
        const PipelineStatusCB& error_cb,
        const TimeSource::WallClockTimeCB& wall_clock_time_cb,
        const base::Closure& waiting_for_decryption_key_cb) override;
    void Flush(const base::Closure& callback) override;
    void StartPlayingFrom(base::TimeDelta timestamp) override;
    void OnTimeStateChanged(bool time_progressing) override;

    void SetTickClockForTesting(scoped_ptr<base::TickClock> tick_clock);
    void SetGpuMemoryBufferVideoForTesting(
        scoped_ptr<GpuMemoryBufferVideoFramePool> gpu_memory_buffer_pool);

    // VideoRendererSink::RenderCallback implementation.
    scoped_refptr<VideoFrame> Render(base::TimeTicks deadline_min,
        base::TimeTicks deadline_max,
        bool background_rendering) override;
    void OnFrameDropped() override;

private:
    // Callback for |video_frame_stream_| initialization.
    void OnVideoFrameStreamInitialized(bool success);

    // Callback for |video_frame_stream_| to deliver decoded video frames and
    // report video decoding status. If a frame is available the planes will be
    // copied asynchronously and FrameReady will be called once finished copying.
    void FrameReadyForCopyingToGpuMemoryBuffers(
        VideoFrameStream::Status status,
        const scoped_refptr<VideoFrame>& frame);

    // Callback for |video_frame_stream_| to deliver decoded video frames and
    // report video decoding status.
    void FrameReady(uint32_t sequence_token,
        VideoFrameStream::Status status,
        const scoped_refptr<VideoFrame>& frame);

    // Helper method for enqueueing a frame to |alogorithm_|.
    void AddReadyFrame_Locked(const scoped_refptr<VideoFrame>& frame);

    // Helper method that schedules an asynchronous read from the
    // |video_frame_stream_| as long as there isn't a pending read and we have
    // capacity.
    void AttemptRead();
    void AttemptRead_Locked();

    // Called when VideoFrameStream::Reset() completes.
    void OnVideoFrameStreamResetDone();

    // Returns true if the renderer has enough data for playback purposes.
    // Note that having enough data may be due to reaching end of stream.
    bool HaveEnoughData_Locked();
    void TransitionToHaveEnough_Locked();
    void TransitionToHaveNothing();

    // Runs |statistics_cb_| with |frames_decoded_| and |frames_dropped_|, resets
    // them to 0.
    void UpdateStats_Locked();

    // Called after we've painted the first frame.  If |time_progressing_| is
    // false it Stop() on |sink_|.
    void MaybeStopSinkAfterFirstPaint();

    // Returns true if there is no more room for additional buffered frames.
    bool HaveReachedBufferingCap();

    // Starts or stops |sink_| respectively. Do not call while |lock_| is held.
    void StartSink();
    void StopSink();

    // Fires |ended_cb_| if there are no remaining usable frames and
    // |received_end_of_stream_| is true.  Sets |rendered_end_of_stream_| if it
    // does so.
    //
    // When called from the media thread, |time_progressing| should reflect the
    // value of |time_progressing_|.  When called from Render() on the sink
    // callback thread, the inverse of |render_first_frame_and_stop_| should be
    // used as a proxy for |time_progressing_|.
    //
    // Returns algorithm_->EffectiveFramesQueued().
    size_t MaybeFireEndedCallback_Locked(bool time_progressing);

    // Helper method for converting a single media timestamp to wall clock time.
    base::TimeTicks ConvertMediaTimestamp(base::TimeDelta media_timestamp);

    scoped_refptr<base::SingleThreadTaskRunner> task_runner_;

    // Sink which calls into VideoRendererImpl via Render() for video frames.  Do
    // not call any methods on the sink while |lock_| is held or the two threads
    // might deadlock. Do not call Start() or Stop() on the sink directly, use
    // StartSink() and StopSink() to ensure background rendering is started.  Only
    // access these values on |task_runner_|.
    VideoRendererSink* const sink_;
    bool sink_started_;

    // Used for accessing data members.
    base::Lock lock_;

    // Provides video frames to VideoRendererImpl.
    scoped_ptr<VideoFrameStream> video_frame_stream_;

    // Pool of GpuMemoryBuffers and resources used to create hardware frames.
    scoped_ptr<GpuMemoryBufferVideoFramePool> gpu_memory_buffer_pool_;

    scoped_refptr<MediaLog> media_log_;

    // Flag indicating low-delay mode.
    bool low_delay_;

    // Keeps track of whether we received the end of stream buffer and finished
    // rendering.
    bool received_end_of_stream_;
    bool rendered_end_of_stream_;

    // Important detail: being in kPlaying doesn't imply that video is being
    // rendered. Rather, it means that the renderer is ready to go. The actual
    // rendering of video is controlled by time advancing via |get_time_cb_|.
    //
    //   kUninitialized
    //         | Initialize()
    //         |
    //         V
    //    kInitializing
    //         | Decoders initialized
    //         |
    //         V            Decoders reset
    //      kFlushed <------------------ kFlushing
    //         | StartPlayingFrom()         ^
    //         |                            |
    //         |                            | Flush()
    //         `---------> kPlaying --------'
    enum State {
        kUninitialized,
        kInitializing,
        kFlushing,
        kFlushed,
        kPlaying
    };
    State state_;

    // An integer that represents how many times the video frame stream has been
    // reset. This is useful when doing video frame copies asynchronously since we
    // want to discard video frames that might be received after the stream has
    // been reset.
    uint32_t sequence_token_;

    // Keep track of the outstanding read on the VideoFrameStream. Flushing can
    // only complete once the read has completed.
    bool pending_read_;

    bool drop_frames_;

    BufferingState buffering_state_;

    // Playback operation callbacks.
    base::Closure flush_cb_;

    // Event callbacks.
    PipelineStatusCB init_cb_;
    StatisticsCB statistics_cb_;
    BufferingStateCB buffering_state_cb_;
    base::Closure ended_cb_;
    PipelineStatusCB error_cb_;
    TimeSource::WallClockTimeCB wall_clock_time_cb_;

    base::TimeDelta start_timestamp_;

    // Keeps track of the number of frames decoded and dropped since the
    // last call to |statistics_cb_|. These must be accessed under lock.
    int frames_decoded_;
    int frames_dropped_;

    scoped_ptr<base::TickClock> tick_clock_;

    // Algorithm for selecting which frame to render; manages frames and all
    // timing related information.
    scoped_ptr<VideoRendererAlgorithm> algorithm_;

    // Indicates that Render() was called with |background_rendering| set to true,
    // so we've entered a background rendering mode where dropped frames are not
    // counted.  Must be accessed under |lock_| once |sink_| is started.
    bool was_background_rendering_;

    // Indicates whether or not media time is currently progressing or not.  Must
    // only be accessed from |task_runner_|.
    bool time_progressing_;

    // Indicates that Render() should only render the first frame and then request
    // that the sink be stopped.  |posted_maybe_stop_after_first_paint_| is used
    // to avoid repeated task posts.
    bool render_first_frame_and_stop_;
    bool posted_maybe_stop_after_first_paint_;

    // Memory usage of |algorithm_| recorded during the last UpdateStats_Locked()
    // call.
    int64_t last_video_memory_usage_;

    // NOTE: Weak pointers must be invalidated before all other member variables.
    base::WeakPtrFactory<VideoRendererImpl> weak_factory_;

    DISALLOW_COPY_AND_ASSIGN(VideoRendererImpl);
};

} // namespace media

#endif // MEDIA_RENDERERS_VIDEO_RENDERER_IMPL_H_
