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
#include "public/platform/WebRTCICECandidate.h"

#include "public/platform/WebString.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"

namespace blink {

class WebRTCICECandidatePrivate final : public RefCounted<WebRTCICECandidatePrivate> {
public:
    static PassRefPtr<WebRTCICECandidatePrivate> create(const WebString& candidate, const WebString& sdpMid, unsigned short sdpMLineIndex)
    {
        return adoptRef(new WebRTCICECandidatePrivate(candidate, sdpMid, sdpMLineIndex));
    }

    const WebString& candidate() const { return m_candidate; }
    const WebString& sdpMid() const { return m_sdpMid; }
    unsigned short sdpMLineIndex() const { return m_sdpMLineIndex; }

    BLINK_PLATFORM_EXPORT void setCandidate(WebString candidate) { m_candidate = candidate; }
    BLINK_PLATFORM_EXPORT void setSdpMid(WebString sdpMid) { m_sdpMid = sdpMid; }
    BLINK_PLATFORM_EXPORT void setSdpMLineIndex(unsigned short sdpMLineIndex) { m_sdpMLineIndex = sdpMLineIndex; }

private:
    WebRTCICECandidatePrivate(const WebString& candidate, const WebString& sdpMid, unsigned short sdpMLineIndex);

    WebString m_candidate;
    WebString m_sdpMid;
    unsigned short m_sdpMLineIndex;
};

WebRTCICECandidatePrivate::WebRTCICECandidatePrivate(const WebString& candidate, const WebString& sdpMid, unsigned short sdpMLineIndex)
    : m_candidate(candidate)
    , m_sdpMid(sdpMid)
    , m_sdpMLineIndex(sdpMLineIndex)
{
}

void WebRTCICECandidate::assign(const WebRTCICECandidate& other)
{
    m_private = other.m_private;
}

void WebRTCICECandidate::reset()
{
    m_private.reset();
}

void WebRTCICECandidate::initialize(const WebString& candidate, const WebString& sdpMid, unsigned short sdpMLineIndex)
{
    m_private = WebRTCICECandidatePrivate::create(candidate, sdpMid, sdpMLineIndex);
}

WebString WebRTCICECandidate::candidate() const
{
    ASSERT(!m_private.isNull());
    return m_private->candidate();
}

WebString WebRTCICECandidate::sdpMid() const
{
    ASSERT(!m_private.isNull());
    return m_private->sdpMid();
}

unsigned short WebRTCICECandidate::sdpMLineIndex() const
{
    ASSERT(!m_private.isNull());
    return m_private->sdpMLineIndex();
}

void WebRTCICECandidate::setCandidate(WebString candidate)
{
    ASSERT(!m_private.isNull());
    m_private->setCandidate(candidate);
}

void WebRTCICECandidate::setSdpMid(WebString sdpMid)
{
    ASSERT(!m_private.isNull());
    m_private->setSdpMid(sdpMid);
}

void WebRTCICECandidate::setSdpMLineIndex(unsigned short sdpMLineIndex)
{
    ASSERT(!m_private.isNull());
    m_private->setSdpMLineIndex(sdpMLineIndex);
}

} // namespace blink
