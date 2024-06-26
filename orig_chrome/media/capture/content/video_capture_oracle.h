// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAPTURE_VIDEO_CAPTURE_ORACLE_H_
#define MEDIA_CAPTURE_VIDEO_CAPTURE_ORACLE_H_

#include "base/callback_forward.h"
#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"
#include "media/base/media_export.h"
#include "media/capture/content/animated_content_sampler.h"
#include "media/capture/content/capture_resolution_chooser.h"
#include "media/capture/content/feedback_signal_accumulator.h"
#include "media/capture/content/smooth_event_sampler.h"
#include "ui/gfx/geometry/rect.h"

namespace media {

// VideoCaptureOracle manages the producer-side throttling of captured frames
// from a video capture device.  It is informed of every update by the device;
// this empowers it to look into the future and decide if a particular frame
// ought to be captured in order to achieve its target frame rate.
class MEDIA_EXPORT VideoCaptureOracle {
public:
    enum Event {
        kTimerPoll,
        kCompositorUpdate,
        kNumEvents,
    };

    enum {
        // The recommended minimum amount of time between calls to
        // ObserveEventAndDecideCapture() for the kTimerPoll Event type.  Anything
        // lower than this isn't harmful, just wasteful.
        kMinTimerPollPeriodMillis = 125, // 8 FPS
    };

    VideoCaptureOracle(base::TimeDelta min_capture_period,
        const gfx::Size& max_frame_size,
        media::ResolutionChangePolicy resolution_change_policy,
        bool enable_auto_throttling);

    virtual ~VideoCaptureOracle();

    // Sets the source content size.  This may not have an immediate effect on the
    // proposed capture size, as the oracle will prevent too-frequent changes from
    // occurring.
    void SetSourceSize(const gfx::Size& source_size);

    // Record a event of type |event|, and decide whether the caller should do a
    // frame capture.  |damage_rect| is the region of a frame about to be drawn,
    // and may be an empty Rect, if this is not known.  If the caller accepts the
    // oracle's proposal, it should call RecordCapture() to indicate this.
    bool ObserveEventAndDecideCapture(Event event,
        const gfx::Rect& damage_rect,
        base::TimeTicks event_time);

    // Record and update internal state based on whether the frame capture will be
    // started.  |pool_utilization| is a value in the range 0.0 to 1.0 to indicate
    // the current buffer pool utilization relative to a sustainable maximum (not
    // the absolute maximum).  This method should only be called if the last call
    // to ObserveEventAndDecideCapture() returned true.  The first method returns
    // the |frame_number| to be used with CompleteCapture().
    int RecordCapture(double pool_utilization);
    void RecordWillNotCapture(double pool_utilization);

    // Notify of the completion of a capture, and whether it was successful.
    // Returns true iff the captured frame should be delivered.  |frame_timestamp|
    // is set to the timestamp that should be provided to the consumer of the
    // frame.
    bool CompleteCapture(int frame_number,
        bool capture_was_successful,
        base::TimeTicks* frame_timestamp);

    // Record the resource utilization feedback for a frame that was processed by
    // the consumer.  This allows the oracle to reduce/increase future data volume
    // if the consumer is overloaded/under-utilized.  |resource_utilization| is a
    // value in the range 0.0 to 1.0 to indicate the current consumer resource
    // utilization relative to a sustainable maximum (not the absolute maximum).
    // This method should only be called for frames where CompleteCapture()
    // returned true.
    void RecordConsumerFeedback(int frame_number, double resource_utilization);

    base::TimeDelta min_capture_period() const
    {
        return smoothing_sampler_.min_capture_period();
    }

    // Returns the oracle's estimate of the duration of the next frame.  This
    // should be called just after ObserveEventAndDecideCapture(), and will only
    // be non-zero if the call returned true.
    base::TimeDelta estimated_frame_duration() const
    {
        return duration_of_next_frame_;
    }

    // Returns the capture frame size the client should use.  This is updated by
    // calls to ObserveEventAndDecideCapture().  The oracle prevents too-frequent
    // changes to the capture size, to avoid stressing the end-to-end pipeline.
    gfx::Size capture_size() const { return capture_size_; }

private:
    // Retrieve/Assign a frame timestamp by capture |frame_number|.  Only valid
    // when IsFrameInRecentHistory(frame_number) returns true.
    base::TimeTicks GetFrameTimestamp(int frame_number) const;
    void SetFrameTimestamp(int frame_number, base::TimeTicks timestamp);

    // Returns true if the frame timestamp ring-buffer currently includes a
    // slot for the given |frame_number|.
    bool IsFrameInRecentHistory(int frame_number) const;

    // Queries the ResolutionChooser to update |capture_size_|, and resets all the
    // FeedbackSignalAccumulator members to stable-state starting values.  The
    // accumulators are reset such that they can only apply feedback updates for
    // future frames (those with a timestamp after |last_frame_time|).
    void CommitCaptureSizeAndReset(base::TimeTicks last_frame_time);

    // Called after a capture or no-capture decision was recorded.  This analyzes
    // current state and may result in a future change to the capture frame size.
    void AnalyzeAndAdjust(base::TimeTicks analyze_time);

    // Analyzes current feedback signal accumulators for an indication that the
    // capture size should be decreased.  Returns either a decreased area, or -1
    // if no decrease should be made.
    int AnalyzeForDecreasedArea(base::TimeTicks analyze_time);

    // Analyzes current feedback signal accumulators for an indication that the
    // the system has had excellent long-term performance and the capture size
    // should be increased to improve quality.  Returns either an increased area,
    // or -1 if no increase should be made.
    int AnalyzeForIncreasedArea(base::TimeTicks analyze_time);

    // Set to false to prevent the oracle from automatically adjusting the capture
    // size in response to end-to-end utilization.
    const bool auto_throttling_enabled_;

    // Incremented every time a paint or update event occurs.
    int next_frame_number_;

    // Stores the last |event_time| from the last observation/decision.  Used to
    // sanity-check that event times are monotonically non-decreasing.
    base::TimeTicks last_event_time_[kNumEvents];

    // Updated by the last call to ObserveEventAndDecideCapture() with the
    // estimated duration of the next frame to sample.  This is zero if the method
    // returned false.
    base::TimeDelta duration_of_next_frame_;

    // Stores the frame number from the last successfully delivered frame.
    int last_successfully_delivered_frame_number_;

    // Stores the number of pending frame captures.
    int num_frames_pending_;

    // These track present/paint history and propose whether to sample each event
    // for capture.  |smoothing_sampler_| uses a "works for all" heuristic, while
    // |content_sampler_| specifically detects animated content (e.g., video
    // playback) and decides which events to sample to "lock into" that content.
    SmoothEventSampler smoothing_sampler_;
    AnimatedContentSampler content_sampler_;

    // Determines video capture frame sizes.
    CaptureResolutionChooser resolution_chooser_;

    // The timestamp of the frame just before the last call to SetSourceSize().
    base::TimeTicks source_size_change_time_;

    // The current capture size.  |resolution_chooser_| may hold an updated value
    // because the oracle prevents this size from changing too frequently.  This
    // avoids over-stressing consumers (e.g., when a window is being activly
    // drag-resized) and allowing the end-to-end system time to stabilize.
    gfx::Size capture_size_;

    // Recent history of frame timestamps proposed by VideoCaptureOracle.  This is
    // a ring-buffer, and should only be accessed by the Get/SetFrameTimestamp()
    // methods.
    enum { kMaxFrameTimestamps = 16 };
    base::TimeTicks frame_timestamps_[kMaxFrameTimestamps];

    // Recent average buffer pool utilization for capture.
    FeedbackSignalAccumulator buffer_pool_utilization_;

    // Estimated maximum frame area that currently can be handled by the consumer,
    // in number of pixels per frame.  This is used to adjust the capture size up
    // or down to a data volume the consumer can handle.  Note that some consumers
    // do not provide feedback, and the analysis logic should account for that.
    FeedbackSignalAccumulator estimated_capable_area_;

    // The time of the first analysis which concluded the end-to-end system was
    // under-utilized.  If null, the system is not currently under-utilized.  This
    // is used to determine when a proving period has elapsed that justifies an
    // increase in capture size.
    base::TimeTicks start_time_of_underutilization_;

    // The timestamp of the frame where |content_sampler_| last detected
    // animation.  This determines whether capture size increases will be
    // aggressive (because content is not animating).
    base::TimeTicks last_time_animation_was_detected_;
};

} // namespace media

#endif // MEDIA_CAPTURE_VIDEO_CAPTURE_ORACLE_H_
