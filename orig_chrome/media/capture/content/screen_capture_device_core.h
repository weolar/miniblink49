// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAPTURE_SCREEN_CAPTURE_DEVICE_CORE_H_
#define MEDIA_CAPTURE_SCREEN_CAPTURE_DEVICE_CORE_H_

#include <string>

#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/thread_checker.h"
#include "media/base/media_export.h"
#include "media/capture/content/thread_safe_capture_oracle.h"
#include "media/capture/video/video_capture_device.h"

namespace tracked_objects {
class Location;
} // namespace tracked_objects

namespace media {

struct VideoCaptureParams;

class ThreadSafeCaptureOracle;

// Keeps track of the video capture source frames and executes copying.
class MEDIA_EXPORT VideoCaptureMachine {
public:
    VideoCaptureMachine();
    virtual ~VideoCaptureMachine();

    // Starts capturing.
    // |callback| is invoked with true if succeeded. Otherwise, with false.
    virtual void Start(const scoped_refptr<ThreadSafeCaptureOracle>& oracle_proxy,
        const VideoCaptureParams& params,
        const base::Callback<void(bool)> callback)
        = 0;

    // Stops capturing.
    // |callback| is invoked after the capturing has stopped.
    virtual void Stop(const base::Closure& callback) = 0;

    // Returns true if the video capture is configured to monitor end-to-end
    // system utilization, and alter frame sizes and/or frame rates to mitigate
    // overloading or under-utilization.
    virtual bool IsAutoThrottlingEnabled() const;

private:
    DISALLOW_COPY_AND_ASSIGN(VideoCaptureMachine);
};

// The "meat" of a content video capturer.
//
// Separating this from the "shell classes" WebContentsVideoCaptureDevice and
// DesktopCaptureDeviceAura allows safe destruction without needing to block any
// threads, as well as code sharing.
//
// ScreenCaptureDeviceCore manages a simple state machine and the pipeline
// (see notes at top of this file).  It times the start of successive captures
// and facilitates the processing of each through the stages of the
// pipeline.
class MEDIA_EXPORT ScreenCaptureDeviceCore
    : public base::SupportsWeakPtr<ScreenCaptureDeviceCore> {
public:
    ScreenCaptureDeviceCore(scoped_ptr<VideoCaptureMachine> capture_machine);
    virtual ~ScreenCaptureDeviceCore();

    // Asynchronous requests to change ScreenCaptureDeviceCore state.
    void AllocateAndStart(const VideoCaptureParams& params,
        scoped_ptr<VideoCaptureDevice::Client> client);
    void StopAndDeAllocate();

private:
    // Flag indicating current state.
    enum State { kIdle,
        kCapturing,
        kError,
        kLastCaptureState };

    void TransitionStateTo(State next_state);

    // Called back in response to StartCaptureMachine().  |success| is true if
    // capture machine succeeded to start.
    void CaptureStarted(bool success);

    // Stops capturing and notifies client_ of an error state.
    void Error(const tracked_objects::Location& from_here,
        const std::string& reason);

    // Tracks that all activity occurs on the media stream manager's thread.
    base::ThreadChecker thread_checker_;

    // Current lifecycle state.
    State state_;

    // Tracks the CaptureMachine that's doing work on our behalf
    // on the device thread or UI thread.
    // This value should never be dereferenced by this class.
    scoped_ptr<VideoCaptureMachine> capture_machine_;

    // Our thread-safe capture oracle which serves as the gateway to the video
    // capture pipeline. Besides the VideoCaptureDevice itself, it is the only
    // component of the system with direct access to |client_|.
    scoped_refptr<ThreadSafeCaptureOracle> oracle_proxy_;

    DISALLOW_COPY_AND_ASSIGN(ScreenCaptureDeviceCore);
};

} // namespace media

#endif // MEDIA_CAPTURE_SCREEN_CAPTURE_DEVICE_CORE_H_
