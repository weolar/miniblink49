// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebRTCOfferOptions_h
#define WebRTCOfferOptions_h

#include "WebCommon.h"
#include "WebNonCopyable.h"
#include "WebPrivatePtr.h"

namespace blink {

class RTCOfferOptions;

class WebRTCOfferOptions {
public:
    WebRTCOfferOptions() { }
    WebRTCOfferOptions(const WebRTCOfferOptions& other) { assign(other); }
    ~WebRTCOfferOptions() { reset(); }

    WebRTCOfferOptions& operator=(const WebRTCOfferOptions& other)
    {
        assign(other);
        return *this;
    }

    BLINK_PLATFORM_EXPORT void assign(const WebRTCOfferOptions&);

    BLINK_PLATFORM_EXPORT void reset();
    bool isNull() const { return m_private.isNull(); }

    BLINK_PLATFORM_EXPORT int32_t offerToReceiveVideo() const;
    BLINK_PLATFORM_EXPORT int32_t offerToReceiveAudio() const;
    BLINK_PLATFORM_EXPORT bool voiceActivityDetection() const;
    BLINK_PLATFORM_EXPORT bool iceRestart() const;

#if INSIDE_BLINK
    BLINK_PLATFORM_EXPORT WebRTCOfferOptions(RTCOfferOptions*);
#endif

private:
    WebPrivatePtr<RTCOfferOptions> m_private;
};

} // namespace blink

#endif // WebRTCOfferOptions_h
