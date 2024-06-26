// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Implement a DirectShow input pin used for receiving captured frames from
// a DirectShow Capture filter.

#ifndef MEDIA_VIDEO_CAPTURE_WIN_SINK_INPUT_PIN_WIN_H_
#define MEDIA_VIDEO_CAPTURE_WIN_SINK_INPUT_PIN_WIN_H_

#include "media/base/video_capture_types.h"
#include "media/capture/video/video_capture_device.h"
#include "media/capture/video/win/pin_base_win.h"
#include "media/capture/video/win/sink_filter_win.h"

namespace media {

// Const used for converting Seconds to REFERENCE_TIME.
extern const REFERENCE_TIME kSecondsToReferenceTime;

// Input pin of the SinkFilter.
class SinkInputPin : public PinBase {
public:
    SinkInputPin(IBaseFilter* filter, SinkFilterObserver* observer);

    void SetRequestedMediaFormat(VideoPixelFormat pixel_format,
        float frame_rate,
        const BITMAPINFOHEADER& info_header);
    // Returns the capability that is negotiated when this
    // pin is connected to a media filter.
    const VideoCaptureFormat& resulting_format() const
    {
        return resulting_format_;
    }

    // Implement PinBase.
    bool IsMediaTypeValid(const AM_MEDIA_TYPE* media_type) override;
    bool GetValidMediaType(int index, AM_MEDIA_TYPE* media_type) override;

    STDMETHOD(Receive)
    (IMediaSample* media_sample) override;

private:
    ~SinkInputPin() override;

    VideoPixelFormat requested_pixel_format_;
    float requested_frame_rate_;
    BITMAPINFOHEADER requested_info_header_;
    VideoCaptureFormat resulting_format_;
    SinkFilterObserver* observer_;

    DISALLOW_IMPLICIT_CONSTRUCTORS(SinkInputPin);
};

} // namespace media

#endif // MEDIA_VIDEO_CAPTURE_WIN_SINK_INPUT_PIN_WIN_H_
