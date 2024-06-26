// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BLINK_VIDEO_FRAME_COMPOSITOR_H_
#define MEDIA_BLINK_VIDEO_FRAME_COMPOSITOR_H_

#include "base/callback.h"
#include "base/memory/ref_counted.h"
#include "base/single_thread_task_runner.h"
#include "base/synchronization/lock.h"
#include "base/time/tick_clock.h"
#include "base/timer/timer.h"
#include "cc/layers/video_frame_provider.h"
#include "media/base/video_renderer_sink.h"
#include "media/blink/media_blink_export.h"
#include "ui/gfx/geometry/size.h"

namespace media {
class VideoFrame;

// VideoFrameCompositor acts as a bridge between the media and cc layers for
// rendering video frames. I.e. a media::VideoRenderer will talk to this class
// from the media side, while a cc::VideoFrameProvider::Client will talk to it
// from the cc side.
//
// This class is responsible for requesting new frames from a video renderer in
// response to requests from the VFP::Client. Since the VFP::Client may stop
// issuing requests in response to visibility changes it is also responsible for
// ensuring the "freshness" of the current frame for programmatic frame
// requests; e.g., Canvas.drawImage() requests
//
// This class is also responsible for detecting frames dropped by the compositor
// after rendering and signaling that information to a RenderCallback. It
// detects frames not dropped by verifying each GetCurrentFrame() is followed
// by a PutCurrentFrame() before the next UpdateCurrentFrame() call.
//
// VideoRenderSink::RenderCallback implementations must call Start() and Stop()
// once new frames are expected or are no longer expected to be ready; this data
// is relayed to the compositor to avoid extraneous callbacks.
//
// VideoFrameCompositor is also responsible for pumping UpdateCurrentFrame()
// callbacks in the background when |client_| has decided to suspend them.
//
// VideoFrameCompositor must live on the same thread as the compositor, though
// it may be constructed on any thread.
class MEDIA_BLINK_EXPORT VideoFrameCompositor
    : public VideoRendererSink,
      NON_EXPORTED_BASE(public cc::VideoFrameProvider) {
public:
    // |compositor_task_runner| is the task runner on which this class will live,
    // though it may be constructed on any thread.
    //
    // |natural_size_changed_cb| is run with the new natural size of the video
    // frame whenever a change in natural size is detected. It is not called the
    // first time UpdateCurrentFrame() is called. Run on the same thread as the
    // caller of UpdateCurrentFrame().
    //
    // |opacity_changed_cb| is run when a change in opacity is detected. It *is*
    // called the first time UpdateCurrentFrame() is called. Run on the same
    // thread as the caller of UpdateCurrentFrame().
    //
    // TODO(dalecurtis): Investigate the inconsistency between the callbacks with
    // respect to why we don't call |natural_size_changed_cb| on the first frame.
    // I suspect it was for historical reasons that no longer make sense.
    VideoFrameCompositor(
        const scoped_refptr<base::SingleThreadTaskRunner>& compositor_task_runner,
        const base::Callback<void(gfx::Size)>& natural_size_changed_cb,
        const base::Callback<void(bool)>& opacity_changed_cb);

    // Destruction must happen on the compositor thread; Stop() must have been
    // called before destruction starts.
    ~VideoFrameCompositor() override;

    // cc::VideoFrameProvider implementation. These methods must be called on the
    // |compositor_task_runner_|.
    void SetVideoFrameProviderClient(
        cc::VideoFrameProvider::Client* client) override;
    bool UpdateCurrentFrame(base::TimeTicks deadline_min,
        base::TimeTicks deadline_max) override;
    bool HasCurrentFrame() override;
    scoped_refptr<VideoFrame> GetCurrentFrame() override;
    void PutCurrentFrame() override;

    // VideoRendererSink implementation. These methods must be called from the
    // same thread (typically the media thread).
    void Start(RenderCallback* callback) override;
    void Stop() override;
    void PaintFrameUsingOldRenderingPath(
        const scoped_refptr<VideoFrame>& frame) override;

    // Returns |current_frame_| if |client_| is set.  If no |client_| is set,
    // |is_background_rendering_| is true, and |callback_| is set, it requests a
    // new frame from |callback_|, using the elapsed time between calls to this
    // function as the render interval; defaulting to 16.6ms if no prior calls
    // have been made.  A cap of 250Hz (4ms) is in place to prevent clients from
    // accidentally (or intentionally) spamming the rendering pipeline.
    //
    // This method is primarily to facilitate canvas and WebGL based applications
    // where the <video> tag is invisible (possibly not even in the DOM) and thus
    // does not receive a |client_|.  In this case, frame acquisition is driven by
    // the frequency of canvas or WebGL paints requested via JavaScript.
    scoped_refptr<VideoFrame> GetCurrentFrameAndUpdateIfStale();

    void set_tick_clock_for_testing(scoped_ptr<base::TickClock> tick_clock)
    {
        tick_clock_ = tick_clock.Pass();
    }

    void clear_current_frame_for_testing() { current_frame_ = nullptr; }

    // Enables or disables background rendering. If |enabled|, |timeout| is the
    // amount of time to wait after the last Render() call before starting the
    // background rendering mode.  Note, this can not disable the background
    // rendering call issues when a sink is started.
    void set_background_rendering_for_testing(bool enabled)
    {
        background_rendering_enabled_ = enabled;
    }

private:
    // Called on the compositor thread in response to Start() or Stop() calls;
    // must be used to change |rendering_| state.
    void OnRendererStateUpdate(bool new_state);

    // Handles setting of |current_frame_| and fires |natural_size_changed_cb_|
    // and |opacity_changed_cb_| when the frame properties changes.
    bool ProcessNewFrame(const scoped_refptr<VideoFrame>& frame);

    // Called by |background_rendering_timer_| when enough time elapses where we
    // haven't seen a Render() call.
    void BackgroundRender();

    // If |callback_| is available, calls Render() with the provided properties.
    // Updates |is_background_rendering_|, |last_interval_|, and resets
    // |background_rendering_timer_|.  Ensures that natural size and opacity
    // changes are correctly fired.  Returns true if there's a new frame available
    // via GetCurrentFrame().
    bool CallRender(base::TimeTicks deadline_min,
        base::TimeTicks deadline_max,
        bool background_rendering);

    scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner_;
    scoped_ptr<base::TickClock> tick_clock_;

    // These callbacks are executed on the compositor thread.
    const base::Callback<void(gfx::Size)> natural_size_changed_cb_;
    const base::Callback<void(bool)> opacity_changed_cb_;

    // Allows tests to disable the background rendering task.
    bool background_rendering_enabled_;

    // Manages UpdateCurrentFrame() callbacks if |client_| has stopped sending
    // them for various reasons.  Runs on |compositor_task_runner_| and is reset
    // after each successful UpdateCurrentFrame() call.
    base::Timer background_rendering_timer_;

    // These values are only set and read on the compositor thread.
    cc::VideoFrameProvider::Client* client_;
    scoped_refptr<VideoFrame> current_frame_;
    bool rendering_;
    bool rendered_last_frame_;
    bool is_background_rendering_;
    bool new_background_frame_;
    base::TimeDelta last_interval_;
    base::TimeTicks last_background_render_;

    // These values are updated and read from the media and compositor threads.
    base::Lock lock_;
    VideoRendererSink::RenderCallback* callback_;

    DISALLOW_COPY_AND_ASSIGN(VideoFrameCompositor);
};

} // namespace media

#endif // MEDIA_BLINK_VIDEO_FRAME_COMPOSITOR_H_
