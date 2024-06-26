// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/capture/video/fake_video_capture_device.h"

#include <algorithm>

#include "base/bind.h"
#include "base/strings/stringprintf.h"
#include "media/audio/fake_audio_input_stream.h"
#include "media/base/video_frame.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPaint.h"

namespace media {

// Sweep at 600 deg/sec.
static const float kPacmanAngularVelocity = 600;
// Beep every 500 ms.
static const int kBeepInterval = 500;

void DrawPacman(bool use_argb,
    uint8_t* const data,
    base::TimeDelta elapsed_time,
    float frame_rate,
    const gfx::Size& frame_size)
{
    // |kN32_SkColorType| stands for the appropriiate RGBA/BGRA format.
    const SkColorType colorspace = use_argb ? kN32_SkColorType : kAlpha_8_SkColorType;
    const SkImageInfo info = SkImageInfo::Make(
        frame_size.width(), frame_size.height(), colorspace, kOpaque_SkAlphaType);
    SkBitmap bitmap;
    bitmap.setInfo(info);
    bitmap.setPixels(data);
    SkPaint paint;
    paint.setStyle(SkPaint::kFill_Style);
    SkCanvas canvas(bitmap);

    // Equalize Alpha_8 that has light green background while RGBA has white.
    if (use_argb) {
        const SkRect full_frame = SkRect::MakeWH(frame_size.width(), frame_size.height());
        paint.setARGB(255, 0, 127, 0);
        canvas.drawRect(full_frame, paint);
    }
    paint.setColor(SK_ColorGREEN);

    // Draw a sweeping circle to show an animation.
    const float end_angle = fmod(kPacmanAngularVelocity * elapsed_time.InSecondsF(), 361);
    const int radius = std::min(frame_size.width(), frame_size.height()) / 4;
    const SkRect rect = SkRect::MakeXYWH(frame_size.width() / 2 - radius,
        frame_size.height() / 2 - radius,
        2 * radius, 2 * radius);
    canvas.drawArc(rect, 0, end_angle, true, paint);

    // Draw current time.
    const int milliseconds = elapsed_time.InMilliseconds() % 1000;
    const int seconds = elapsed_time.InSeconds() % 60;
    const int minutes = elapsed_time.InMinutes() % 60;
    const int hours = elapsed_time.InHours();
    const int frame_count = elapsed_time.InMilliseconds() * frame_rate / 1000;

    const std::string time_string = base::StringPrintf("%d:%02d:%02d:%03d %d", hours, minutes, seconds,
        milliseconds, frame_count);
    canvas.scale(3, 3);
    canvas.drawText(time_string.data(), time_string.length(), 30, 20, paint);
}

FakeVideoCaptureDevice::FakeVideoCaptureDevice(BufferOwnership buffer_ownership,
    BufferPlanarity planarity,
    float fake_capture_rate)
    : buffer_ownership_(buffer_ownership)
    , planarity_(planarity)
    , fake_capture_rate_(fake_capture_rate)
    , weak_factory_(this)
{
}

FakeVideoCaptureDevice::~FakeVideoCaptureDevice()
{
    DCHECK(thread_checker_.CalledOnValidThread());
}

void FakeVideoCaptureDevice::AllocateAndStart(
    const VideoCaptureParams& params,
    scoped_ptr<VideoCaptureDevice::Client> client)
{
    DCHECK(thread_checker_.CalledOnValidThread());

    client_ = client.Pass();

    // Incoming |params| can be none of the supported formats, so we get the
    // closest thing rounded up. TODO(mcasas): Use the |params|, if they belong to
    // the supported ones, when http://crbug.com/309554 is verified.
    capture_format_.frame_rate = fake_capture_rate_;
    if (params.requested_format.frame_size.width() > 1280)
        capture_format_.frame_size.SetSize(1920, 1080);
    else if (params.requested_format.frame_size.width() > 640)
        capture_format_.frame_size.SetSize(1280, 720);
    else if (params.requested_format.frame_size.width() > 320)
        capture_format_.frame_size.SetSize(640, 480);
    else
        capture_format_.frame_size.SetSize(320, 240);

    if (buffer_ownership_ == BufferOwnership::CLIENT_BUFFERS) {
        if (planarity_ == BufferPlanarity::PACKED) {
            capture_format_.pixel_storage = PIXEL_STORAGE_CPU;
            capture_format_.pixel_format = PIXEL_FORMAT_ARGB;
            DVLOG(1) << "starting with client argb buffers";
        } else if (planarity_ == BufferPlanarity::TRIPLANAR) {
            capture_format_.pixel_storage = PIXEL_STORAGE_GPUMEMORYBUFFER;
            capture_format_.pixel_format = PIXEL_FORMAT_I420;
            DVLOG(1) << "starting with gmb I420 buffers";
        }
    } else if (buffer_ownership_ == BufferOwnership::OWN_BUFFERS) {
        capture_format_.pixel_storage = PIXEL_STORAGE_CPU;
        capture_format_.pixel_format = PIXEL_FORMAT_I420;
        DVLOG(1) << "starting with own I420 buffers";
    }

    if (capture_format_.pixel_format == PIXEL_FORMAT_I420) {
        fake_frame_.reset(new uint8[VideoFrame::AllocationSize(
            PIXEL_FORMAT_I420, capture_format_.frame_size)]);
    }

    beep_time_ = base::TimeDelta();
    elapsed_time_ = base::TimeDelta();

    if (buffer_ownership_ == BufferOwnership::CLIENT_BUFFERS)
        BeepAndScheduleNextCapture(
            base::TimeTicks::Now(),
            base::Bind(&FakeVideoCaptureDevice::CaptureUsingClientBuffers,
                weak_factory_.GetWeakPtr()));
    else if (buffer_ownership_ == BufferOwnership::OWN_BUFFERS)
        BeepAndScheduleNextCapture(
            base::TimeTicks::Now(),
            base::Bind(&FakeVideoCaptureDevice::CaptureUsingOwnBuffers,
                weak_factory_.GetWeakPtr()));
}

void FakeVideoCaptureDevice::StopAndDeAllocate()
{
    DCHECK(thread_checker_.CalledOnValidThread());
    client_.reset();
}

void FakeVideoCaptureDevice::CaptureUsingOwnBuffers(
    base::TimeTicks expected_execution_time)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    const size_t frame_size = capture_format_.ImageAllocationSize();
    memset(fake_frame_.get(), 0, frame_size);

    DrawPacman(false /* use_argb */, fake_frame_.get(), elapsed_time_,
        fake_capture_rate_, capture_format_.frame_size);

    // Give the captured frame to the client.
    if (planarity_ == BufferPlanarity::PACKED) {
        client_->OnIncomingCapturedData(fake_frame_.get(), frame_size,
            capture_format_, 0 /* rotation */,
            base::TimeTicks::Now());
    } else if (planarity_ == BufferPlanarity::TRIPLANAR) {
        client_->OnIncomingCapturedYuvData(
            fake_frame_.get(),
            fake_frame_.get() + capture_format_.frame_size.GetArea(),
            fake_frame_.get() + capture_format_.frame_size.GetArea() * 5 / 4,
            capture_format_.frame_size.width(),
            capture_format_.frame_size.width() / 2,
            capture_format_.frame_size.width() / 2, capture_format_,
            0 /* rotation */, base::TimeTicks::Now());
    }
    BeepAndScheduleNextCapture(
        expected_execution_time,
        base::Bind(&FakeVideoCaptureDevice::CaptureUsingOwnBuffers,
            weak_factory_.GetWeakPtr()));
}

void FakeVideoCaptureDevice::CaptureUsingClientBuffers(
    base::TimeTicks expected_execution_time)
{
    DCHECK(thread_checker_.CalledOnValidThread());

    scoped_ptr<VideoCaptureDevice::Client::Buffer> capture_buffer(
        client_->ReserveOutputBuffer(capture_format_.frame_size,
            capture_format_.pixel_format,
            capture_format_.pixel_storage));
    DLOG_IF(ERROR, !capture_buffer) << "Couldn't allocate Capture Buffer";
    DCHECK(capture_buffer->data()) << "Buffer has NO backing memory";

    if (capture_format_.pixel_storage == PIXEL_STORAGE_GPUMEMORYBUFFER && capture_format_.pixel_format == media::PIXEL_FORMAT_I420) {
        // Since SkBitmap expects a packed&continuous memory region for I420, we
        // need to use |fake_frame_| to draw onto.
        memset(fake_frame_.get(), 0, capture_format_.ImageAllocationSize());
        DrawPacman(false /* use_argb */, fake_frame_.get(), elapsed_time_,
            fake_capture_rate_, capture_format_.frame_size);

        // Copy data from |fake_frame_| into the reserved planes of GpuMemoryBuffer.
        size_t offset = 0;
        for (size_t i = 0; i < VideoFrame::NumPlanes(PIXEL_FORMAT_I420); ++i) {
            const size_t plane_size = VideoFrame::PlaneSize(PIXEL_FORMAT_I420, i,
                capture_format_.frame_size)
                                          .GetArea();
            memcpy(capture_buffer->data(i), fake_frame_.get() + offset, plane_size);
            offset += plane_size;
        }
    } else {
        DCHECK_EQ(capture_format_.pixel_storage, PIXEL_STORAGE_CPU);
        DCHECK_EQ(capture_format_.pixel_format, PIXEL_FORMAT_ARGB);
        uint8_t* data_ptr = static_cast<uint8_t*>(capture_buffer->data());
        memset(data_ptr, 0, capture_buffer->mapped_size());
        DrawPacman(true /* use_argb */, data_ptr, elapsed_time_, fake_capture_rate_,
            capture_format_.frame_size);
    }

    // Give the captured frame to the client.
    client_->OnIncomingCapturedBuffer(capture_buffer.Pass(), capture_format_,
        base::TimeTicks::Now());

    BeepAndScheduleNextCapture(
        expected_execution_time,
        base::Bind(&FakeVideoCaptureDevice::CaptureUsingClientBuffers,
            weak_factory_.GetWeakPtr()));
}

void FakeVideoCaptureDevice::BeepAndScheduleNextCapture(
    base::TimeTicks expected_execution_time,
    const base::Callback<void(base::TimeTicks)>& next_capture)
{
    const base::TimeDelta beep_interval = base::TimeDelta::FromMilliseconds(kBeepInterval);
    const base::TimeDelta frame_interval = base::TimeDelta::FromMicroseconds(1e6 / fake_capture_rate_);
    beep_time_ += frame_interval;
    elapsed_time_ += frame_interval;

    // Generate a synchronized beep twice per second.
    if (beep_time_ >= beep_interval) {
        FakeAudioInputStream::BeepOnce();
        beep_time_ -= beep_interval;
    }

    // Reschedule next CaptureTask.
    const base::TimeTicks current_time = base::TimeTicks::Now();
    // Don't accumulate any debt if we are lagging behind - just post the next
    // frame immediately and continue as normal.
    const base::TimeTicks next_execution_time = std::max(current_time, expected_execution_time + frame_interval);
    const base::TimeDelta delay = next_execution_time - current_time;
    base::MessageLoop::current()->PostDelayedTask(
        FROM_HERE, base::Bind(next_capture, next_execution_time), delay);
}

} // namespace media
