// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Implement a simple base class for a DirectShow input pin. It may only be
// used in a single threaded apartment.

#ifndef MEDIA_VIDEO_CAPTURE_WIN_PIN_BASE_WIN_H_
#define MEDIA_VIDEO_CAPTURE_WIN_PIN_BASE_WIN_H_

// Avoid including strsafe.h via dshow as it will cause build warnings.
#define NO_DSHOW_STRSAFE
#include <dshow.h>

#include "base/memory/ref_counted.h"
#include "base/win/scoped_comptr.h"

namespace media {

class PinBase : public IPin,
                public IMemInputPin,
                public base::RefCounted<PinBase> {
public:
    explicit PinBase(IBaseFilter* owner);

    // Function used for changing the owner.
    // If the owner is deleted the owner should first call this function
    // with owner = NULL.
    void SetOwner(IBaseFilter* owner);

    // Checks if a media type is acceptable. This is called when this pin is
    // connected to an output pin. Must return true if the media type is
    // acceptable, false otherwise.
    virtual bool IsMediaTypeValid(const AM_MEDIA_TYPE* media_type) = 0;

    // Enumerates valid media types.
    virtual bool GetValidMediaType(int index, AM_MEDIA_TYPE* media_type) = 0;

    // Called when new media is received. Note that this is not on the same
    // thread as where the pin is created.
    STDMETHOD(Receive)
    (IMediaSample* sample) override = 0;

    STDMETHOD(Connect)
    (IPin* receive_pin,
        const AM_MEDIA_TYPE* media_type) override;

    STDMETHOD(ReceiveConnection)
    (IPin* connector,
        const AM_MEDIA_TYPE* media_type) override;

    STDMETHOD(Disconnect)
    () override;

    STDMETHOD(ConnectedTo)
    (IPin** pin) override;

    STDMETHOD(ConnectionMediaType)
    (AM_MEDIA_TYPE* media_type) override;

    STDMETHOD(QueryPinInfo)
    (PIN_INFO* info) override;

    STDMETHOD(QueryDirection)
    (PIN_DIRECTION* pin_dir) override;

    STDMETHOD(QueryId)
    (LPWSTR* id) override;

    STDMETHOD(QueryAccept)
    (const AM_MEDIA_TYPE* media_type) override;

    STDMETHOD(EnumMediaTypes)
    (IEnumMediaTypes** types) override;

    STDMETHOD(QueryInternalConnections)
    (IPin** pins, ULONG* no_pins) override;

    STDMETHOD(EndOfStream)
    () override;

    STDMETHOD(BeginFlush)
    () override;

    STDMETHOD(EndFlush)
    () override;

    STDMETHOD(NewSegment)
    (REFERENCE_TIME start,
        REFERENCE_TIME stop,
        double dRate) override;

    // Inherited from IMemInputPin.
    STDMETHOD(GetAllocator)
    (IMemAllocator** allocator) override;

    STDMETHOD(NotifyAllocator)
    (IMemAllocator* allocator, BOOL read_only) override;

    STDMETHOD(GetAllocatorRequirements)
    (
        ALLOCATOR_PROPERTIES* properties) override;

    STDMETHOD(ReceiveMultiple)
    (IMediaSample** samples,
        long sample_count,
        long* processed) override;
    STDMETHOD(ReceiveCanBlock)
    () override;

    // Inherited from IUnknown.
    STDMETHOD(QueryInterface)
    (REFIID id, void** object_ptr) override;

    STDMETHOD_(ULONG, AddRef)
    () override;

    STDMETHOD_(ULONG, Release)
    () override;

protected:
    friend class base::RefCounted<PinBase>;
    virtual ~PinBase();

private:
    AM_MEDIA_TYPE current_media_type_;
    base::win::ScopedComPtr<IPin> connected_pin_;
    // owner_ is the filter owning this pin. We don't reference count it since
    // that would create a circular reference count.
    IBaseFilter* owner_;
};

} // namespace media

#endif // MEDIA_VIDEO_CAPTURE_WIN_PIN_BASE_WIN_H_
