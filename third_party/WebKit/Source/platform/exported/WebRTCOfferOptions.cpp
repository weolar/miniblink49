// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/platform/WebRTCOfferOptions.h"

#include "platform/mediastream/RTCOfferOptions.h"

namespace blink {

WebRTCOfferOptions::WebRTCOfferOptions(RTCOfferOptions* options)
    : m_private(options)
{
}

void WebRTCOfferOptions::assign(const WebRTCOfferOptions& other)
{
    m_private = other.m_private;
}

void WebRTCOfferOptions::reset()
{
    m_private.reset();
}

int32_t WebRTCOfferOptions::offerToReceiveVideo() const
{
    ASSERT(!isNull());
    return m_private->offerToReceiveVideo();
}

int32_t WebRTCOfferOptions::offerToReceiveAudio() const
{
    ASSERT(!isNull());
    return m_private->offerToReceiveAudio();
}

bool WebRTCOfferOptions::voiceActivityDetection() const
{
    ASSERT(!isNull());
    return m_private->voiceActivityDetection();
}

bool WebRTCOfferOptions::iceRestart() const
{
    ASSERT(!isNull());
    return m_private->iceRestart();
}

} // namespace blink
