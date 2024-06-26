// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/blink/video_frame_compositor.h"

#include "base/bind.h"
#include "base/message_loop/message_loop.h"
#include "base/time/default_tick_clock.h"
#include "base/trace_event/trace_event.h"
#include "media/base/video_frame.h"

namespace media {

// Amount of time to wait between UpdateCurrentFrame() callbacks before starting
// background rendering to keep the Render() callbacks moving.
const int kBackgroundRenderingTimeoutMs = 250;

// Returns true if the format has no Alpha channel (hence is always opaque).
static bool IsOpaque(const scoped_refptr<VideoFrame>& frame)
{
    switch (frame->format()) {
    case PIXEL_FORMAT_UNKNOWN:
    case PIXEL_FORMAT_I420:
    case PIXEL_FORMAT_YV12:
    case PIXEL_FORMAT_YV16:
    case PIXEL_FORMAT_YV24:
    case PIXEL_FORMAT_NV12:
    case PIXEL_FORMAT_NV21:
    case PIXEL_FORMAT_UYVY:
    case PIXEL_FORMAT_YUY2:
    case PIXEL_FORMAT_XRGB:
    case PIXEL_FORMAT_RGB24:
    case PIXEL_FORMAT_MJPEG:
    case PIXEL_FORMAT_MT21:
        return true;
    case PIXEL_FORMAT_YV12A:
    case PIXEL_FORMAT_ARGB:
    case PIXEL_FORMAT_RGB32:
        break;
    }
    return false;
}

VideoFrameCompositor::VideoFrameCompositor(
    const scoped_refptr<base::SingleThreadTaskRunner>& compositor_task_runner,
    const base::Callback<void(gfx::Size)>& natural_size_changed_cb,
    const base::Callback<void(bool)>& opacity_changed_cb)
    : compositor_task_runner_(compositor_task_runner)
    , tick_clock_(new base::DefaultTickClock())
    , natural_size_changed_cb_(natural_size_changed_cb)
    , opacity_changed_cb_(opacity_changed_cb)
    , background_rendering_enabled_(true)
    , background_rendering_timer_(
          FROM_HERE,
          base::TimeDelta::FromMilliseconds(kBackgroundRenderingTimeoutMs),
          base::Bind(&VideoFrameCompositor::BackgroundRender, base::Unretained(this)),
          // Task is not repeating, CallRender() will reset the task as needed.
          false)
    , client_(nullptr)
    , rendering_(false)
    , rendered_last_frame_(false)
    , is_background_rendering_(false)
    , new_background_frame_(false)
    // Assume 60Hz before the first UpdateCurrentFrame() call.
    , last_interval_(base::TimeDelta::FromSecondsD(1.0 / 60))
    , callback_(nullptr)
{
    background_rendering_timer_.SetTaskRunner(compositor_task_runner_);
}

VideoFrameCompositor::~VideoFrameCompositor()
{
    DCHECK(compositor_task_runner_->BelongsToCurrentThread());
    DCHECK(!callback_);
    DCHECK(!rendering_);
    if (client_)
        client_->StopUsingProvider();
}

void VideoFrameCompositor::OnRendererStateUpdate(bool new_state)
{
    DCHECK(compositor_task_runner_->BelongsToCurrentThread());
    DCHECK_NE(rendering_, new_state);
    rendering_ = new_state;

    if (rendering_) {
        // Always start playback in background rendering mode, if |client_| kicks
        // in right away it's okay.
        BackgroundRender();
    } else if (background_rendering_enabled_) {
        background_rendering_timer_.Stop();
    } else {
        DCHECK(!background_rendering_timer_.IsRunning());
    }

    if (!client_)
        return;

    if (rendering_)
        client_->StartRendering();
    else
        client_->StopRendering();
}

void VideoFrameCompositor::SetVideoFrameProviderClient(cc::VideoFrameProvider::Client* client)
{
    DCHECK(compositor_task_runner_->BelongsToCurrentThread());
    if (client_)
        client_->StopUsingProvider();
    client_ = client;

    // |client_| may now be null, so verify before calling it.
    if (rendering_ && client_)
        client_->StartRendering();
}

scoped_refptr<VideoFrame> VideoFrameCompositor::GetCurrentFrame()
{
    DCHECK(compositor_task_runner_->BelongsToCurrentThread());
    return current_frame_;
}

void VideoFrameCompositor::PutCurrentFrame()
{
    DCHECK(compositor_task_runner_->BelongsToCurrentThread());
    rendered_last_frame_ = true;
}

bool VideoFrameCompositor::UpdateCurrentFrame(base::TimeTicks deadline_min, base::TimeTicks deadline_max)
{
    DCHECK(compositor_task_runner_->BelongsToCurrentThread());
    return CallRender(deadline_min, deadline_max, false);
}

bool VideoFrameCompositor::HasCurrentFrame()
{
    DCHECK(compositor_task_runner_->BelongsToCurrentThread());
    return current_frame_;
}

void VideoFrameCompositor::Start(RenderCallback* callback)
{
    TRACE_EVENT0("media", "VideoFrameCompositor::Start");

    // Called from the media thread, so acquire the callback under lock before
    // returning in case a Stop() call comes in before the PostTask is processed.
    base::AutoLock lock(lock_);
    DCHECK(!callback_);
    callback_ = callback;
    compositor_task_runner_->PostTask(
        FROM_HERE, base::Bind(&VideoFrameCompositor::OnRendererStateUpdate, base::Unretained(this), true));
}

void VideoFrameCompositor::Stop()
{
    TRACE_EVENT0("media", "VideoFrameCompositor::Stop");

    // Called from the media thread, so release the callback under lock before
    // returning to avoid a pending UpdateCurrentFrame() call occurring before
    // the PostTask is processed.
    base::AutoLock lock(lock_);
    DCHECK(callback_);
    callback_ = nullptr;
    compositor_task_runner_->PostTask(FROM_HERE, base::Bind(&VideoFrameCompositor::OnRendererStateUpdate, base::Unretained(this), false));
}

void VideoFrameCompositor::PaintFrameUsingOldRenderingPath(const scoped_refptr<VideoFrame>& frame)
{
    if (!compositor_task_runner_->BelongsToCurrentThread()) {
        compositor_task_runner_->PostTask(
            FROM_HERE,
            base::Bind(&VideoFrameCompositor::PaintFrameUsingOldRenderingPath, base::Unretained(this), frame));
        return;
    }

    if (ProcessNewFrame(frame) && client_)
        client_->DidReceiveFrame();
}

scoped_refptr<VideoFrame>
VideoFrameCompositor::GetCurrentFrameAndUpdateIfStale()
{
    DCHECK(compositor_task_runner_->BelongsToCurrentThread());
    if (client_ || !rendering_ || !is_background_rendering_)
        return current_frame_;

    DCHECK(!last_background_render_.is_null());

    const base::TimeTicks now = tick_clock_->NowTicks();
    const base::TimeDelta interval = now - last_background_render_;

    // Cap updates to 250Hz which should be more than enough for everyone.
    if (interval < base::TimeDelta::FromMilliseconds(4))
        return current_frame_;

    // Update the interval based on the time between calls and call background
    // render which will give this information to the client.
    last_interval_ = interval;
    BackgroundRender();

    return current_frame_;
}

bool VideoFrameCompositor::ProcessNewFrame(const scoped_refptr<VideoFrame>& frame)
{
    DCHECK(compositor_task_runner_->BelongsToCurrentThread());

    if (frame == current_frame_)
        return false;

    // Set the flag indicating that the current frame is unrendered, if we get a
    // subsequent PutCurrentFrame() call it will mark it as rendered.
    rendered_last_frame_ = false;

    if (current_frame_ && current_frame_->natural_size() != frame->natural_size()) {
        natural_size_changed_cb_.Run(frame->natural_size());
    }

    if (!current_frame_ || IsOpaque(current_frame_) != IsOpaque(frame))
        opacity_changed_cb_.Run(IsOpaque(frame));

    current_frame_ = frame;
    return true;
}

void VideoFrameCompositor::BackgroundRender()
{
    DCHECK(compositor_task_runner_->BelongsToCurrentThread());
    const base::TimeTicks now = tick_clock_->NowTicks();
    last_background_render_ = now;
    bool new_frame = CallRender(now, now + last_interval_, true);
    if (new_frame && client_)
        client_->DidReceiveFrame();
}

bool VideoFrameCompositor::CallRender(base::TimeTicks deadline_min, base::TimeTicks deadline_max, bool background_rendering)
{
    DCHECK(compositor_task_runner_->BelongsToCurrentThread());

    base::AutoLock lock(lock_);
    if (!callback_) {
        // Even if we no longer have a callback, return true if we have a frame
        // which |client_| hasn't seen before.
        return !rendered_last_frame_ && current_frame_;
    }

    DCHECK(rendering_);

    // If the previous frame was never rendered and we're not in background
    // rendering mode (nor have just exited it), let the client know.
    if (!rendered_last_frame_ && current_frame_ && !background_rendering && !is_background_rendering_) {
        callback_->OnFrameDropped();
    }

    const bool new_frame = ProcessNewFrame(callback_->Render(deadline_min, deadline_max, background_rendering));

    // We may create a new frame here with background rendering, but the provider
    // has no way of knowing that a new frame had been processed, so keep track of
    // the new frame, and return true on the next call to |CallRender|.
    const bool had_new_background_frame = new_background_frame_;
    new_background_frame_ = background_rendering && new_frame;

    is_background_rendering_ = background_rendering;
    last_interval_ = deadline_max - deadline_min;

    // Restart the background rendering timer whether we're background rendering
    // or not; in either case we should wait for |kBackgroundRenderingTimeoutMs|.
    if (background_rendering_enabled_)
        background_rendering_timer_.Reset();
    return new_frame || had_new_background_frame;
}

} // namespace media
