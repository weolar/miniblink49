/*
 * Copyright (C) 2010 Google, Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ResourceLoadTiming_h
#define ResourceLoadTiming_h

#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/RefPtr.h"

namespace blink {

class ResourceLoadTiming : public RefCounted<ResourceLoadTiming> {
public:
    static PassRefPtr<ResourceLoadTiming> create()
    {
        return adoptRef(new ResourceLoadTiming);
    }

    PassRefPtr<ResourceLoadTiming> deepCopy()
    {
        RefPtr<ResourceLoadTiming> timing = create();
        timing->m_requestTime = m_requestTime;
        timing->m_proxyStart = m_proxyStart;
        timing->m_proxyEnd = m_proxyEnd;
        timing->m_dnsStart = m_dnsStart;
        timing->m_dnsEnd = m_dnsEnd;
        timing->m_connectStart = m_connectStart;
        timing->m_connectEnd = m_connectEnd;
        timing->m_workerStart = m_workerStart;
        timing->m_workerReady = m_workerReady;
        timing->m_sendStart = m_sendStart;
        timing->m_sendEnd = m_sendEnd;
        timing->m_receiveHeadersEnd = m_receiveHeadersEnd;
        timing->m_sslStart = m_sslStart;
        timing->m_sslEnd = m_sslEnd;
        return timing.release();
    }

    bool operator==(const ResourceLoadTiming& other) const
    {
        return m_requestTime == other.m_requestTime
            && m_proxyStart == other.m_proxyStart
            && m_proxyEnd == other.m_proxyEnd
            && m_dnsStart == other.m_dnsStart
            && m_dnsEnd == other.m_dnsEnd
            && m_connectStart == other.m_connectStart
            && m_connectEnd == other.m_connectEnd
            && m_workerStart == other.m_workerStart
            && m_workerReady == other.m_workerReady
            && m_sendStart == other.m_sendStart
            && m_sendEnd == other.m_sendEnd
            && m_receiveHeadersEnd == other.m_receiveHeadersEnd
            && m_sslStart == other.m_sslStart
            && m_sslEnd == other.m_sslEnd;
    }

    bool operator!=(const ResourceLoadTiming& other) const
    {
        return !(*this == other);
    }

    void setDnsStart(double dnsStart) { m_dnsStart = dnsStart; }
    void setRequestTime(double requestTime) { m_requestTime = requestTime; }
    void setProxyStart(double proxyStart) { m_proxyStart = proxyStart; }
    void setProxyEnd(double proxyEnd) { m_proxyEnd = proxyEnd; }
    void setDnsEnd(double dnsEnd) { m_dnsEnd = dnsEnd; }
    void setConnectStart(double connectStart) { m_connectStart = connectStart; }
    void setConnectEnd(double connectEnd) { m_connectEnd = connectEnd; }
    void setWorkerStart(double workerStart) { m_workerStart = workerStart; }
    void setWorkerReady(double workerReady) { m_workerReady = workerReady; }
    void setSendStart(double sendStart) { m_sendStart = sendStart; }
    void setSendEnd(double sendEnd) { m_sendEnd = sendEnd; }
    void setReceiveHeadersEnd(double receiveHeadersEnd) { m_receiveHeadersEnd = receiveHeadersEnd; }
    void setSslStart(double sslStart) { m_sslStart = sslStart; }
    void setSslEnd(double sslEnd) { m_sslEnd = sslEnd; }

    double dnsStart() const { return m_dnsStart; }
    double requestTime() const { return m_requestTime; }
    double proxyStart() const { return m_proxyStart; }
    double proxyEnd() const { return m_proxyEnd; }
    double dnsEnd() const { return m_dnsEnd; }
    double connectStart() const { return m_connectStart; }
    double connectEnd() const { return m_connectEnd; }
    double workerStart() const { return m_workerStart; }
    double workerReady() const { return m_workerReady; }
    double sendStart() const { return m_sendStart; }
    double sendEnd() const { return m_sendEnd; }
    double receiveHeadersEnd() const { return m_receiveHeadersEnd; }
    double sslStart() const { return m_sslStart; }
    double sslEnd() const { return m_sslEnd; }

    double calculateMillisecondDelta(double time) const { return time ? (time - m_requestTime) * 1000 : -1; }

private:
    ResourceLoadTiming()
        : m_requestTime(0)
        , m_proxyStart(0)
        , m_proxyEnd(0)
        , m_dnsStart(0)
        , m_dnsEnd(0)
        , m_connectStart(0)
        , m_connectEnd(0)
        , m_workerStart(0)
        , m_workerReady(0)
        , m_sendStart(0)
        , m_sendEnd(0)
        , m_receiveHeadersEnd(0)
        , m_sslStart(0)
        , m_sslEnd(0)
    {
    }

    // We want to present a unified timeline to Javascript. Using walltime is problematic, because the clock may skew while resources
    // load. To prevent that skew, we record a single reference walltime when root document navigation begins. All other times are
    // recorded using monotonicallyIncreasingTime(). When a time needs to be presented to Javascript, we build a pseudo-walltime
    // using the following equation (m_requestTime as example):
    //   pseudo time = document wall reference + (m_requestTime - document monotonic reference).

    // All monotonicallyIncreasingTime() in seconds
    double m_requestTime;
    double m_proxyStart;
    double m_proxyEnd;
    double m_dnsStart;
    double m_dnsEnd;
    double m_connectStart;
    double m_connectEnd;
    double m_workerStart;
    double m_workerReady;
    double m_sendStart;
    double m_sendEnd;
    double m_receiveHeadersEnd;
    double m_sslStart;
    double m_sslEnd;
};

}

#endif
