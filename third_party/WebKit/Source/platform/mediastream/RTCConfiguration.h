/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Google Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
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

#ifndef RTCConfiguration_h
#define RTCConfiguration_h

#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "wtf/PassRefPtr.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

namespace blink {

class RTCIceServer final : public GarbageCollectedFinalized<RTCIceServer> {
public:
    static RTCIceServer* create(const KURL& uri, const String& username, const String& credential)
    {
        return new RTCIceServer(uri, username, credential);
    }

    const KURL& uri() { return m_uri; }
    const String& username() { return m_username; }
    const String& credential() { return m_credential; }

    DEFINE_INLINE_TRACE() { }

private:
    RTCIceServer(const KURL& uri, const String& username, const String& credential)
        : m_uri(uri)
        , m_username(username)
        , m_credential(credential)
    {
    }

    KURL m_uri;
    String m_username;
    String m_credential;
};

enum RTCIceTransports {
    RTCIceTransportsNone,
    RTCIceTransportsRelay,
    RTCIceTransportsAll
};

enum RTCBundlePolicy {
    RTCBundlePolicyBalanced,
    RTCBundlePolicyMaxCompat,
    RTCBundlePolicyMaxBundle
};

enum RTCRtcpMuxPolicy {
    RTCRtcpMuxPolicyNegotiate,
    RTCRtcpMuxPolicyRequire
};

class RTCConfiguration final : public GarbageCollected<RTCConfiguration> {
public:
    static RTCConfiguration* create() { return new RTCConfiguration(); }

    void appendServer(RTCIceServer* server) { m_servers.append(server); }
    size_t numberOfServers() { return m_servers.size(); }
    RTCIceServer* server(size_t index) { return m_servers[index].get(); }
    void setIceTransports(RTCIceTransports iceTransports) { m_iceTransports = iceTransports; }
    RTCIceTransports iceTransports() { return m_iceTransports; }
    void setBundlePolicy(RTCBundlePolicy bundlePolicy) { m_bundlePolicy = bundlePolicy; }
    RTCBundlePolicy bundlePolicy() { return m_bundlePolicy; }
    void setRtcpMuxPolicy(RTCRtcpMuxPolicy rtcpMuxPolicy) { m_rtcpMuxPolicy = rtcpMuxPolicy; }
    RTCRtcpMuxPolicy rtcpMuxPolicy() { return m_rtcpMuxPolicy; }

    DEFINE_INLINE_TRACE() { visitor->trace(m_servers); }

private:
    RTCConfiguration() :
        m_iceTransports(RTCIceTransportsAll),
        m_bundlePolicy(RTCBundlePolicyBalanced),
        m_rtcpMuxPolicy(RTCRtcpMuxPolicyNegotiate) { }

    HeapVector<Member<RTCIceServer>> m_servers;
    RTCIceTransports m_iceTransports;
    RTCBundlePolicy m_bundlePolicy;
    RTCRtcpMuxPolicy m_rtcpMuxPolicy;
};

} // namespace blink

#endif // RTCConfiguration_h
