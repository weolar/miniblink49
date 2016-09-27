/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 * Copyright (C) 2012 Intel Inc. All rights reserved.
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

#ifndef PerformanceResourceTiming_h
#define PerformanceResourceTiming_h

#include "core/timing/PerformanceEntry.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"

namespace blink {

class ResourceLoadTiming;
class ResourceTimingInfo;

class PerformanceResourceTiming final : public PerformanceEntry {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PerformanceResourceTiming* create(const ResourceTimingInfo& info, double timeOrigin, double startTime, double lastRedirectEndTime, bool m_allowTimingDetails, bool m_allowRedirectDetails)
    {
        return new PerformanceResourceTiming(info, timeOrigin, startTime, lastRedirectEndTime, m_allowTimingDetails, m_allowRedirectDetails);
    }

    static PerformanceResourceTiming* create(const ResourceTimingInfo& info, double timeOrigin, double startTime, bool m_allowTimingDetails)
    {
        return new PerformanceResourceTiming(info, timeOrigin, startTime, 0.0, m_allowTimingDetails, false);
    }

    AtomicString initiatorType() const;

    double workerStart() const;
    double redirectStart() const;
    double redirectEnd() const;
    double fetchStart() const;
    double domainLookupStart() const;
    double domainLookupEnd() const;
    double connectStart() const;
    double connectEnd() const;
    double secureConnectionStart() const;
    double requestStart() const;
    double responseStart() const;
    double responseEnd() const;

    virtual bool isResource() override { return true; }

private:
    PerformanceResourceTiming(const ResourceTimingInfo&, double timeOrigin, double startTime, double lastRedirectEndTime, bool m_allowTimingDetails, bool m_allowRedirectDetails);
    virtual ~PerformanceResourceTiming();

    double workerReady() const;

    AtomicString m_initiatorType;
    double m_timeOrigin;
    RefPtr<ResourceLoadTiming> m_timing;
    double m_lastRedirectEndTime;
    double m_finishTime;
    bool m_didReuseConnection;
    bool m_allowTimingDetails;
    bool m_allowRedirectDetails;
};

} // namespace blink

#endif // PerformanceResourceTiming_h
