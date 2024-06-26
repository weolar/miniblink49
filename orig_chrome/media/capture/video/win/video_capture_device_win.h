// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Windows specific implementation of VideoCaptureDevice. DirectShow is used for
// capturing. DirectShow provide its own threads for capturing.

#ifndef MEDIA_VIDEO_CAPTURE_WIN_VIDEO_CAPTURE_DEVICE_WIN_H_
#define MEDIA_VIDEO_CAPTURE_WIN_VIDEO_CAPTURE_DEVICE_WIN_H_

// Avoid including strsafe.h via dshow as it will cause build warnings.
#define NO_DSHOW_STRSAFE
#include <dshow.h>

#include <map>
#include <string>

#include "base/threading/thread_checker.h"
#include "base/win/scoped_comptr.h"
#include "media/base/video_capture_types.h"
#include "media/capture/video/video_capture_device.h"
#include "media/capture/video/win/capability_list_win.h"
#include "media/capture/video/win/sink_filter_win.h"
#include "media/capture/video/win/sink_input_pin_win.h"

namespace tracked_objects {
class Location;
} // namespace tracked_objects

namespace media {

// All the methods in the class can only be run on a COM initialized thread.
class VideoCaptureDeviceWin : public VideoCaptureDevice,
                              public SinkFilterObserver {
public:
    // A utility class that wraps the AM_MEDIA_TYPE type and guarantees that
    // we free the structure when exiting the scope.  DCHECKing is also done to
    // avoid memory leaks.
    class ScopedMediaType {
    public:
        ScopedMediaType()
            : media_type_(NULL)
        {
        }
        ~ScopedMediaType() { Free(); }

        AM_MEDIA_TYPE* operator->() { return media_type_; }
        AM_MEDIA_TYPE* get() { return media_type_; }
        void Free();
        AM_MEDIA_TYPE** Receive();

    private:
        void FreeMediaType(AM_MEDIA_TYPE* mt);
        void DeleteMediaType(AM_MEDIA_TYPE* mt);

        AM_MEDIA_TYPE* media_type_;
    };

    static HRESULT GetDeviceFilter(const std::string& device_id,
        IBaseFilter** filter);
    static base::win::ScopedComPtr<IPin> GetPin(IBaseFilter* filter,
        PIN_DIRECTION pin_dir,
        REFGUID category,
        REFGUID major_type);
    static VideoPixelFormat TranslateMediaSubtypeToPixelFormat(
        const GUID& sub_type);

    explicit VideoCaptureDeviceWin(const Name& device_name);
    ~VideoCaptureDeviceWin() override;
    // Opens the device driver for this device.
    bool Init();

    // VideoCaptureDevice implementation.
    void AllocateAndStart(const VideoCaptureParams& params,
        scoped_ptr<VideoCaptureDevice::Client> client) override;
    void StopAndDeAllocate() override;

private:
    enum InternalState {
        kIdle, // The device driver is opened but camera is not in use.
        kCapturing, // Video is being captured.
        kError // Error accessing HW functions.
        // User needs to recover by destroying the object.
    };

    // Implements SinkFilterObserver.
    void FrameReceived(const uint8* buffer, int length,
        base::TimeTicks timestamp) override;

    bool CreateCapabilityMap();
    void SetAntiFlickerInCaptureFilter(const VideoCaptureParams& params);
    void SetErrorState(const tracked_objects::Location& from_here,
        const std::string& reason);

    const Name device_name_;
    InternalState state_;
    scoped_ptr<VideoCaptureDevice::Client> client_;

    base::win::ScopedComPtr<IBaseFilter> capture_filter_;

    base::win::ScopedComPtr<IGraphBuilder> graph_builder_;
    base::win::ScopedComPtr<ICaptureGraphBuilder2> capture_graph_builder_;

    base::win::ScopedComPtr<IMediaControl> media_control_;
    base::win::ScopedComPtr<IPin> input_sink_pin_;
    base::win::ScopedComPtr<IPin> output_capture_pin_;

    scoped_refptr<SinkFilter> sink_filter_;

    // Map of all capabilities this device support.
    CapabilityList capabilities_;
    VideoCaptureFormat capture_format_;

    base::ThreadChecker thread_checker_;

    DISALLOW_IMPLICIT_CONSTRUCTORS(VideoCaptureDeviceWin);
};

} // namespace media

#endif // MEDIA_VIDEO_CAPTURE_WIN_VIDEO_CAPTURE_DEVICE_WIN_H_
