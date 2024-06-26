// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/capture/content/thread_safe_capture_oracle.h"

#include "base/basictypes.h"
#include "base/bind.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/synchronization/lock.h"
#include "base/time/time.h"
#include "base/trace_event/trace_event.h"
#include "media/base/video_capture_types.h"
#include "media/base/video_frame.h"
#include "media/base/video_frame_metadata.h"
#include "media/base/video_util.h"
#include "ui/gfx/geometry/rect.h"

namespace media {

namespace {

    // The target maximum amount of the buffer pool to utilize.  Actual buffer pool
    // utilization is attenuated by this amount before being reported to the
    // VideoCaptureOracle.  This value takes into account the maximum number of
    // buffer pool buffers and a desired safety margin.
    const int kTargetMaxPoolUtilizationPercent = 60;

} // namespace

ThreadSafeCaptureOracle::ThreadSafeCaptureOracle(
    scoped_ptr<VideoCaptureDevice::Client> client,
    const VideoCaptureParams& params,
    bool enable_auto_throttling)
    : client_(client.Pass())
    , oracle_(base::TimeDelta::FromMicroseconds(static_cast<int64>(
                  1000000.0 / params.requested_format.frame_rate + 0.5 /* to round to nearest int */)),
          params.requested_format.frame_size,
          params.resolution_change_policy,
          enable_auto_throttling)
    , params_(params)
{
}

ThreadSafeCaptureOracle::~ThreadSafeCaptureOracle()
{
}

bool ThreadSafeCaptureOracle::ObserveEventAndDecideCapture(
    VideoCaptureOracle::Event event,
    const gfx::Rect& damage_rect,
    base::TimeTicks event_time,
    scoped_refptr<VideoFrame>* storage,
    CaptureFrameCallback* callback)
{
    // Grab the current time before waiting to acquire the |lock_|.
    const base::TimeTicks capture_begin_time = base::TimeTicks::Now();

    base::AutoLock guard(lock_);

    if (!client_)
        return false; // Capture is stopped.

    const bool should_capture = oracle_.ObserveEventAndDecideCapture(event, damage_rect, event_time);
    const gfx::Size visible_size = oracle_.capture_size();
    // Always round up the coded size to multiple of 16 pixels.
    // See http://crbug.com/402151.
    const gfx::Size coded_size((visible_size.width() + 15) & ~15,
        (visible_size.height() + 15) & ~15);

    scoped_ptr<media::VideoCaptureDevice::Client::Buffer> output_buffer(
        client_->ReserveOutputBuffer(coded_size,
            (params_.requested_format.pixel_storage != media::PIXEL_STORAGE_TEXTURE)
                ? media::PIXEL_FORMAT_I420
                : media::PIXEL_FORMAT_ARGB,
            params_.requested_format.pixel_storage));
    // Get the current buffer pool utilization and attenuate it: The utilization
    // reported to the oracle is in terms of a maximum sustainable amount (not the
    // absolute maximum).
    const double attenuated_utilization = client_->GetBufferPoolUtilization() * (100.0 / kTargetMaxPoolUtilizationPercent);

    const char* event_name = (event == VideoCaptureOracle::kTimerPoll
            ? "poll"
            : (event == VideoCaptureOracle::kCompositorUpdate ? "gpu"
                                                              : "unknown"));

    // Consider the various reasons not to initiate a capture.
    if (should_capture && !output_buffer.get()) {
        TRACE_EVENT_INSTANT1("gpu.capture", "PipelineLimited",
            TRACE_EVENT_SCOPE_THREAD, "trigger", event_name);
        oracle_.RecordWillNotCapture(attenuated_utilization);
        return false;
    } else if (!should_capture && output_buffer.get()) {
        if (event == VideoCaptureOracle::kCompositorUpdate) {
            // This is a normal and acceptable way to drop a frame. We've hit our
            // capture rate limit: for example, the content is animating at 60fps but
            // we're capturing at 30fps.
            TRACE_EVENT_INSTANT1("gpu.capture", "FpsRateLimited",
                TRACE_EVENT_SCOPE_THREAD, "trigger", event_name);
        }
        return false;
    } else if (!should_capture && !output_buffer.get()) {
        // We decided not to capture, but we wouldn't have been able to if we wanted
        // to because no output buffer was available.
        TRACE_EVENT_INSTANT1("gpu.capture", "NearlyPipelineLimited",
            TRACE_EVENT_SCOPE_THREAD, "trigger", event_name);
        return false;
    }
    const int frame_number = oracle_.RecordCapture(attenuated_utilization);
    TRACE_EVENT_ASYNC_BEGIN2("gpu.capture", "Capture", output_buffer.get(),
        "frame_number", frame_number, "trigger", event_name);
    // Texture frames wrap a texture mailbox, which we don't have at the moment.
    // We do not construct those frames.
    if (params_.requested_format.pixel_storage != media::PIXEL_STORAGE_TEXTURE) {
        *storage = VideoFrame::WrapExternalData(
            media::PIXEL_FORMAT_I420, coded_size, gfx::Rect(visible_size),
            visible_size, static_cast<uint8*>(output_buffer->data()),
            output_buffer->mapped_size(), base::TimeDelta());
        DCHECK(*storage);
    }
    *callback = base::Bind(&ThreadSafeCaptureOracle::DidCaptureFrame, this, frame_number,
        base::Passed(&output_buffer), capture_begin_time,
        oracle_.estimated_frame_duration());
    return true;
}

gfx::Size ThreadSafeCaptureOracle::GetCaptureSize() const
{
    base::AutoLock guard(lock_);
    return oracle_.capture_size();
}

void ThreadSafeCaptureOracle::UpdateCaptureSize(const gfx::Size& source_size)
{
    base::AutoLock guard(lock_);
    VLOG(1) << "Source size changed to " << source_size.ToString();
    oracle_.SetSourceSize(source_size);
}

void ThreadSafeCaptureOracle::Stop()
{
    base::AutoLock guard(lock_);
    client_.reset();
}

void ThreadSafeCaptureOracle::ReportError(
    const tracked_objects::Location& from_here,
    const std::string& reason)
{
    base::AutoLock guard(lock_);
    if (client_)
        client_->OnError(from_here, reason);
}

void ThreadSafeCaptureOracle::DidCaptureFrame(
    int frame_number,
    scoped_ptr<VideoCaptureDevice::Client::Buffer> buffer,
    base::TimeTicks capture_begin_time,
    base::TimeDelta estimated_frame_duration,
    const scoped_refptr<VideoFrame>& frame,
    base::TimeTicks timestamp,
    bool success)
{
    base::AutoLock guard(lock_);
    TRACE_EVENT_ASYNC_END2("gpu.capture", "Capture", buffer.get(), "success",
        success, "timestamp", timestamp.ToInternalValue());

    if (oracle_.CompleteCapture(frame_number, success, &timestamp)) {
        TRACE_EVENT_INSTANT0("gpu.capture", "CaptureSucceeded",
            TRACE_EVENT_SCOPE_THREAD);

        if (!client_)
            return; // Capture is stopped.

        frame->metadata()->SetDouble(VideoFrameMetadata::FRAME_RATE,
            params_.requested_format.frame_rate);
        frame->metadata()->SetTimeTicks(VideoFrameMetadata::CAPTURE_BEGIN_TIME,
            capture_begin_time);
        frame->metadata()->SetTimeTicks(VideoFrameMetadata::CAPTURE_END_TIME,
            base::TimeTicks::Now());
        frame->metadata()->SetTimeDelta(VideoFrameMetadata::FRAME_DURATION,
            estimated_frame_duration);

        frame->AddDestructionObserver(
            base::Bind(&ThreadSafeCaptureOracle::DidConsumeFrame, this,
                frame_number, frame->metadata()));

        client_->OnIncomingCapturedVideoFrame(buffer.Pass(), frame, timestamp);
    }
}

void ThreadSafeCaptureOracle::DidConsumeFrame(
    int frame_number,
    const media::VideoFrameMetadata* metadata)
{
    // Note: This function may be called on any thread by the VideoFrame
    // destructor.  |metadata| is still valid for read-access at this point.
    double utilization = -1.0;
    if (metadata->GetDouble(media::VideoFrameMetadata::RESOURCE_UTILIZATION,
            &utilization)) {
        base::AutoLock guard(lock_);
        oracle_.RecordConsumerFeedback(frame_number, utilization);
    }
}

} // namespace media
