/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include "public/platform/WebRTCConfiguration.h"

#include "platform/mediastream/RTCConfiguration.h"
#include "public/platform/WebString.h"
#include "public/platform/WebURL.h"

namespace blink {

WebRTCICEServer::WebRTCICEServer(RTCIceServer* iceServer)
    : m_private(iceServer)
{
}

void WebRTCICEServer::assign(const WebRTCICEServer& other)
{
    m_private = other.m_private;
}

void WebRTCICEServer::reset()
{
    m_private.reset();
}

WebURL WebRTCICEServer::uri() const
{
    ASSERT(!isNull());
    return m_private->uri();
}

WebString WebRTCICEServer::username() const
{
    ASSERT(!isNull());
    return m_private->username();
}

WebString WebRTCICEServer::credential() const
{
    ASSERT(!isNull());
    return m_private->credential();
}

WebRTCConfiguration::WebRTCConfiguration(RTCConfiguration* configuration)
    : m_private(configuration)
{
}

void WebRTCConfiguration::assign(const WebRTCConfiguration& other)
{
    m_private = other.m_private;
}

void WebRTCConfiguration::reset()
{
    m_private.reset();
}

size_t WebRTCConfiguration::numberOfServers() const
{
    ASSERT(!isNull());
    return m_private->numberOfServers();
}

WebRTCICEServer WebRTCConfiguration::server(size_t index) const
{
    ASSERT(!isNull());
    return WebRTCICEServer(m_private->server(index));
}

WebRTCIceTransports WebRTCConfiguration::iceTransports() const
{
    ASSERT(!isNull());
    switch (m_private->iceTransports()) {
    case RTCIceTransportsNone:
        return WebRTCIceTransportsNone;
    case RTCIceTransportsRelay:
        return WebRTCIceTransportsRelay;
    case RTCIceTransportsAll:
        return WebRTCIceTransportsAll;
    default:
        ASSERT_NOT_REACHED();
    }
    return WebRTCIceTransportsAll;
}

WebRTCBundlePolicy WebRTCConfiguration::bundlePolicy() const
{
    ASSERT(!isNull());
    switch (m_private->bundlePolicy()) {
    case RTCBundlePolicyBalanced:
        return WebRTCBundlePolicyBalanced;
    case RTCBundlePolicyMaxCompat:
        return WebRTCBundlePolicyMaxCompat;
    case RTCBundlePolicyMaxBundle:
        return WebRTCBundlePolicyMaxBundle;
    default:
        ASSERT_NOT_REACHED();
    }
    return WebRTCBundlePolicyBalanced;
}

WebRTCRtcpMuxPolicy WebRTCConfiguration::rtcpMuxPolicy() const
{
    ASSERT(!isNull());
    switch (m_private->rtcpMuxPolicy()) {
    case RTCRtcpMuxPolicyNegotiate:
        return WebRTCRtcpMuxPolicyNegotiate;
    case RTCRtcpMuxPolicyRequire:
        return WebRTCRtcpMuxPolicyRequire;
    default:
        ASSERT_NOT_REACHED();
    }
    return WebRTCRtcpMuxPolicyNegotiate;
}

} // namespace blink
