// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/capture/video/win/video_capture_device_mf_win.h"

#include <mfapi.h>
#include <mferror.h>

#include "base/location.h"
#include "base/memory/ref_counted.h"
#include "base/strings/stringprintf.h"
#include "base/strings/sys_string_conversions.h"
#include "base/synchronization/waitable_event.h"
#include "base/win/scoped_co_mem.h"
#include "base/win/windows_version.h"
#include "media/capture/video/win/capability_list_win.h"

using base::win::ScopedCoMem;
using base::win::ScopedComPtr;

namespace media {

// In Windows device identifiers, the USB VID and PID are preceded by the string
// "vid_" or "pid_".  The identifiers are each 4 bytes long.
const char kVidPrefix[] = "vid_"; // Also contains '\0'.
const char kPidPrefix[] = "pid_"; // Also contains '\0'.
const size_t kVidPidSize = 4;

static bool GetFrameSize(IMFMediaType* type, gfx::Size* frame_size)
{
    UINT32 width32, height32;
    if (FAILED(MFGetAttributeSize(type, MF_MT_FRAME_SIZE, &width32, &height32)))
        return false;
    frame_size->SetSize(width32, height32);
    return true;
}

static bool GetFrameRate(IMFMediaType* type, float* frame_rate)
{
    UINT32 numerator, denominator;
    if (FAILED(MFGetAttributeRatio(type, MF_MT_FRAME_RATE, &numerator,
            &denominator))
        || !denominator) {
        return false;
    }
    *frame_rate = static_cast<float>(numerator) / denominator;
    return true;
}

static bool FillFormat(IMFMediaType* type, VideoCaptureFormat* format)
{
    GUID type_guid;
    if (FAILED(type->GetGUID(MF_MT_SUBTYPE, &type_guid)) || !GetFrameSize(type, &format->frame_size) || !GetFrameRate(type, &format->frame_rate) || !VideoCaptureDeviceMFWin::FormatFromGuid(type_guid, &format->pixel_format)) {
        return false;
    }

    return true;
}

HRESULT FillCapabilities(IMFSourceReader* source,
    CapabilityList* capabilities)
{
    DWORD stream_index = 0;
    ScopedComPtr<IMFMediaType> type;
    HRESULT hr;
    while (SUCCEEDED(hr = source->GetNativeMediaType(
                         kFirstVideoStream, stream_index, type.Receive()))) {
        VideoCaptureFormat format;
        if (FillFormat(type.get(), &format))
            capabilities->emplace_back(stream_index, format);
        type.Release();
        ++stream_index;
    }

    if (capabilities->empty() && (SUCCEEDED(hr) || hr == MF_E_NO_MORE_TYPES))
        hr = HRESULT_FROM_WIN32(ERROR_EMPTY);

    return (hr == MF_E_NO_MORE_TYPES) ? S_OK : hr;
}

class MFReaderCallback final
    : public base::RefCountedThreadSafe<MFReaderCallback>,
      public IMFSourceReaderCallback {
public:
    MFReaderCallback(VideoCaptureDeviceMFWin* observer)
        : observer_(observer)
        , wait_event_(NULL)
    {
    }

    void SetSignalOnFlush(base::WaitableEvent* event) { wait_event_ = event; }

    STDMETHOD(QueryInterface)
    (REFIID riid, void** object) override
    {
        if (riid != IID_IUnknown && riid != IID_IMFSourceReaderCallback)
            return E_NOINTERFACE;
        *object = static_cast<IMFSourceReaderCallback*>(this);
        AddRef();
        return S_OK;
    }

    STDMETHOD_(ULONG, AddRef)
    () override
    {
        base::RefCountedThreadSafe<MFReaderCallback>::AddRef();
        return 1U;
    }

    STDMETHOD_(ULONG, Release)
    () override
    {
        base::RefCountedThreadSafe<MFReaderCallback>::Release();
        return 1U;
    }

    STDMETHOD(OnReadSample)
    (HRESULT status,
        DWORD stream_index,
        DWORD stream_flags,
        LONGLONG time_stamp,
        IMFSample* sample) override
    {
        base::TimeTicks stamp(base::TimeTicks::Now());
        if (!sample) {
            observer_->OnIncomingCapturedData(NULL, 0, 0, stamp);
            return S_OK;
        }

        DWORD count = 0;
        sample->GetBufferCount(&count);

        for (DWORD i = 0; i < count; ++i) {
            ScopedComPtr<IMFMediaBuffer> buffer;
            sample->GetBufferByIndex(i, buffer.Receive());
            if (buffer.get()) {
                DWORD length = 0, max_length = 0;
                BYTE* data = NULL;
                buffer->Lock(&data, &max_length, &length);
                observer_->OnIncomingCapturedData(data, length, 0, stamp);
                buffer->Unlock();
            }
        }
        return S_OK;
    }

    STDMETHOD(OnFlush)
    (DWORD stream_index) override
    {
        if (wait_event_) {
            wait_event_->Signal();
            wait_event_ = NULL;
        }
        return S_OK;
    }

    STDMETHOD(OnEvent)
    (DWORD stream_index, IMFMediaEvent* event) override
    {
        NOTIMPLEMENTED();
        return S_OK;
    }

private:
    friend class base::RefCountedThreadSafe<MFReaderCallback>;
    ~MFReaderCallback() { }

    VideoCaptureDeviceMFWin* observer_;
    base::WaitableEvent* wait_event_;
};

// static
bool VideoCaptureDeviceMFWin::FormatFromGuid(const GUID& guid,
    VideoPixelFormat* format)
{
    struct {
        const GUID& guid;
        const VideoPixelFormat format;
    } static const kFormatMap[] = {
        { MFVideoFormat_I420, PIXEL_FORMAT_I420 },
        { MFVideoFormat_YUY2, PIXEL_FORMAT_YUY2 },
        { MFVideoFormat_UYVY, PIXEL_FORMAT_UYVY },
        { MFVideoFormat_RGB24, PIXEL_FORMAT_RGB24 },
        { MFVideoFormat_ARGB32, PIXEL_FORMAT_ARGB },
        { MFVideoFormat_MJPG, PIXEL_FORMAT_MJPEG },
        { MFVideoFormat_YV12, PIXEL_FORMAT_YV12 },
    };

    for (int i = 0; i < arraysize(kFormatMap); ++i) {
        if (kFormatMap[i].guid == guid) {
            *format = kFormatMap[i].format;
            return true;
        }
    }

    return false;
}

const std::string VideoCaptureDevice::Name::GetModel() const
{
    const size_t vid_prefix_size = sizeof(kVidPrefix) - 1;
    const size_t pid_prefix_size = sizeof(kPidPrefix) - 1;
    const size_t vid_location = unique_id_.find(kVidPrefix);
    if (vid_location == std::string::npos || vid_location + vid_prefix_size + kVidPidSize > unique_id_.size()) {
        return std::string();
    }
    const size_t pid_location = unique_id_.find(kPidPrefix);
    if (pid_location == std::string::npos || pid_location + pid_prefix_size + kVidPidSize > unique_id_.size()) {
        return std::string();
    }
    std::string id_vendor = unique_id_.substr(vid_location + vid_prefix_size, kVidPidSize);
    std::string id_product = unique_id_.substr(pid_location + pid_prefix_size, kVidPidSize);
    return id_vendor + ":" + id_product;
}

VideoCaptureDeviceMFWin::VideoCaptureDeviceMFWin(const Name& device_name)
    : name_(device_name)
    , capture_(0)
{
    DetachFromThread();
}

VideoCaptureDeviceMFWin::~VideoCaptureDeviceMFWin()
{
    DCHECK(CalledOnValidThread());
}

bool VideoCaptureDeviceMFWin::Init(
    const base::win::ScopedComPtr<IMFMediaSource>& source)
{
    DCHECK(CalledOnValidThread());
    DCHECK(!reader_.get());

    ScopedComPtr<IMFAttributes> attributes;
    MFCreateAttributes(attributes.Receive(), 1);
    DCHECK(attributes.get());

    callback_ = new MFReaderCallback(this);
    attributes->SetUnknown(MF_SOURCE_READER_ASYNC_CALLBACK, callback_.get());

    return SUCCEEDED(MFCreateSourceReaderFromMediaSource(
        source.get(), attributes.get(), reader_.Receive()));
}

void VideoCaptureDeviceMFWin::AllocateAndStart(
    const VideoCaptureParams& params,
    scoped_ptr<VideoCaptureDevice::Client> client)
{
    DCHECK(CalledOnValidThread());

    base::AutoLock lock(lock_);

    client_ = client.Pass();
    DCHECK_EQ(capture_, false);

    CapabilityList capabilities;
    HRESULT hr = S_OK;
    if (reader_.get()) {
        hr = FillCapabilities(reader_.get(), &capabilities);
        if (SUCCEEDED(hr)) {
            const CapabilityWin found_capability = GetBestMatchedCapability(params.requested_format, capabilities);
            ScopedComPtr<IMFMediaType> type;
            hr = reader_->GetNativeMediaType(
                kFirstVideoStream, found_capability.stream_index, type.Receive());
            if (SUCCEEDED(hr)) {
                hr = reader_->SetCurrentMediaType(kFirstVideoStream, NULL, type.get());
                if (SUCCEEDED(hr)) {
                    hr = reader_->ReadSample(kFirstVideoStream, 0, NULL, NULL, NULL, NULL);
                    if (SUCCEEDED(hr)) {
                        capture_format_ = found_capability.supported_format;
                        capture_ = true;
                        return;
                    }
                }
            }
        }
    }

    OnError(FROM_HERE, hr);
}

void VideoCaptureDeviceMFWin::StopAndDeAllocate()
{
    DCHECK(CalledOnValidThread());
    base::WaitableEvent flushed(false, false);
    const int kFlushTimeOutInMs = 1000;
    bool wait = false;
    {
        base::AutoLock lock(lock_);
        if (capture_) {
            capture_ = false;
            callback_->SetSignalOnFlush(&flushed);
            wait = SUCCEEDED(
                reader_->Flush(static_cast<DWORD>(MF_SOURCE_READER_ALL_STREAMS)));
            if (!wait) {
                callback_->SetSignalOnFlush(NULL);
            }
        }
        client_.reset();
    }

    // If the device has been unplugged, the Flush() won't trigger the event
    // and a timeout will happen.
    // TODO(tommi): Hook up the IMFMediaEventGenerator notifications API and
    // do not wait at all after getting MEVideoCaptureDeviceRemoved event.
    // See issue/226396.
    if (wait)
        flushed.TimedWait(base::TimeDelta::FromMilliseconds(kFlushTimeOutInMs));
}

void VideoCaptureDeviceMFWin::OnIncomingCapturedData(
    const uint8* data,
    int length,
    int rotation,
    const base::TimeTicks& time_stamp)
{
    base::AutoLock lock(lock_);
    if (data && client_.get()) {
        client_->OnIncomingCapturedData(data, length, capture_format_, rotation,
            time_stamp);
    }

    if (capture_) {
        HRESULT hr = reader_->ReadSample(kFirstVideoStream, 0, NULL, NULL, NULL, NULL);
        if (FAILED(hr)) {
            // If running the *VideoCap* unit tests on repeat, this can sometimes
            // fail with HRESULT_FROM_WINHRESULT_FROM_WIN32(ERROR_INVALID_FUNCTION).
            // It's not clear to me why this is, but it is possible that it has
            // something to do with this bug:
            // http://support.microsoft.com/kb/979567
            OnError(FROM_HERE, hr);
        }
    }
}

void VideoCaptureDeviceMFWin::OnError(
    const tracked_objects::Location& from_here,
    HRESULT hr)
{
    if (client_.get()) {
        client_->OnError(
            from_here,
            base::StringPrintf("VideoCaptureDeviceMFWin: %s",
                logging::SystemErrorCodeToString(hr).c_str()));
    }
}

} // namespace media
