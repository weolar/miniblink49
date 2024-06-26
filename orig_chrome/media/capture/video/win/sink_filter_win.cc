// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/capture/video/win/sink_filter_win.h"

#include "base/logging.h"
#include "media/capture/video/win/sink_input_pin_win.h"

namespace media {

// Define GUID for I420. This is the color format we would like to support but
// it is not defined in the DirectShow SDK.
// http://msdn.microsoft.com/en-us/library/dd757532.aspx
// 30323449-0000-0010-8000-00AA00389B71.
GUID kMediaSubTypeI420 = { 0x30323449,
    0x0000,
    0x0010,
    { 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 } };

// UYVY synonym with BT709 color components, used in HD video. This variation
// might appear in non-USB capture cards and it's implemented as a normal YUV
// pixel format with the characters HDYC encoded in the first array word.
GUID kMediaSubTypeHDYC = { 0x43594448,
    0x0000,
    0x0010,
    { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };

SinkFilterObserver::~SinkFilterObserver()
{
}

SinkFilter::SinkFilter(SinkFilterObserver* observer)
    : input_pin_(NULL)
{
    input_pin_ = new SinkInputPin(this, observer);
}

void SinkFilter::SetRequestedMediaFormat(VideoPixelFormat pixel_format,
    float frame_rate,
    const BITMAPINFOHEADER& info_header)
{
    input_pin_->SetRequestedMediaFormat(pixel_format, frame_rate, info_header);
}

const VideoCaptureFormat& SinkFilter::ResultingFormat() const
{
    return input_pin_->resulting_format();
}

size_t SinkFilter::NoOfPins()
{
    return 1;
}

IPin* SinkFilter::GetPin(int index)
{
    return index == 0 ? input_pin_.get() : NULL;
}

STDMETHODIMP SinkFilter::GetClassID(CLSID* clsid)
{
    *clsid = __uuidof(SinkFilter);
    return S_OK;
}

SinkFilter::~SinkFilter()
{
    input_pin_->SetOwner(NULL);
}

} // namespace media
