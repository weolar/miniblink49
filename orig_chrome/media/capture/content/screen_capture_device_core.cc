// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/capture/content/screen_capture_device_core.h"

#include "base/bind.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/threading/thread_checker.h"

namespace media {

namespace {

    void DeleteCaptureMachine(scoped_ptr<VideoCaptureMachine> capture_machine)
    {
        capture_machine.reset();
    }

} // namespace

VideoCaptureMachine::VideoCaptureMachine()
{
}

VideoCaptureMachine::~VideoCaptureMachine()
{
}

bool VideoCaptureMachine::IsAutoThrottlingEnabled() const
{
    return false;
}

void ScreenCaptureDeviceCore::AllocateAndStart(
    const VideoCaptureParams& params,
    scoped_ptr<VideoCaptureDevice::Client> client)
{
    DCHECK(thread_checker_.CalledOnValidThread());

    if (state_ != kIdle) {
        DVLOG(1) << "Allocate() invoked when not in state Idle.";
        return;
    }

    if (!(params.requested_format.pixel_format == PIXEL_FORMAT_I420 && params.requested_format.pixel_storage == PIXEL_STORAGE_CPU) && !(params.requested_format.pixel_format == PIXEL_FORMAT_ARGB && params.requested_format.pixel_storage == PIXEL_STORAGE_TEXTURE)) {
        client->OnError(
            FROM_HERE,
            base::StringPrintf(
                "unsupported format: %s",
                VideoCaptureFormat::ToString(params.requested_format).c_str()));
        return;
    }

    oracle_proxy_ = new ThreadSafeCaptureOracle(
        client.Pass(), params, capture_machine_->IsAutoThrottlingEnabled());

    capture_machine_->Start(
        oracle_proxy_, params,
        base::Bind(&ScreenCaptureDeviceCore::CaptureStarted, AsWeakPtr()));

    TransitionStateTo(kCapturing);
}

void ScreenCaptureDeviceCore::StopAndDeAllocate()
{
    DCHECK(thread_checker_.CalledOnValidThread());

    if (state_ != kCapturing)
        return;

    oracle_proxy_->Stop();
    oracle_proxy_ = NULL;

    TransitionStateTo(kIdle);

    capture_machine_->Stop(base::Bind(&base::DoNothing));
}

void ScreenCaptureDeviceCore::CaptureStarted(bool success)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    if (!success)
        Error(FROM_HERE, "Failed to start capture machine.");
}

ScreenCaptureDeviceCore::ScreenCaptureDeviceCore(
    scoped_ptr<VideoCaptureMachine> capture_machine)
    : state_(kIdle)
    , capture_machine_(capture_machine.Pass())
{
    DCHECK(capture_machine_.get());
}

ScreenCaptureDeviceCore::~ScreenCaptureDeviceCore()
{
    DCHECK(thread_checker_.CalledOnValidThread());
    DCHECK_NE(state_, kCapturing);
    if (capture_machine_) {
        capture_machine_->Stop(
            base::Bind(&DeleteCaptureMachine, base::Passed(&capture_machine_)));
    }
    DVLOG(1) << "ScreenCaptureDeviceCore@" << this << " destroying.";
}

void ScreenCaptureDeviceCore::TransitionStateTo(State next_state)
{
    DCHECK(thread_checker_.CalledOnValidThread());

#ifndef NDEBUG
    static const char* kStateNames[] = { "Idle", "Capturing", "Error" };
    static_assert(arraysize(kStateNames) == kLastCaptureState,
        "Different number of states and textual descriptions");
    DVLOG(1) << "State change: " << kStateNames[state_] << " --> "
             << kStateNames[next_state];
#endif

    state_ = next_state;
}

void ScreenCaptureDeviceCore::Error(const tracked_objects::Location& from_here,
    const std::string& reason)
{
    DCHECK(thread_checker_.CalledOnValidThread());

    if (state_ == kIdle)
        return;

    if (oracle_proxy_.get())
        oracle_proxy_->ReportError(from_here, reason);

    StopAndDeAllocate();
    TransitionStateTo(kError);
}

} // namespace media
