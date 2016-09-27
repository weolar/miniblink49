/*
 * Copyright (C) 2011 Google, Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY GOOGLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL GOOGLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/loader/DocumentLoadTiming.h"

#include "platform/TraceEvent.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "wtf/RefPtr.h"

namespace blink {

DocumentLoadTiming::DocumentLoadTiming()
    : m_referenceMonotonicTime(0.0)
    , m_referenceWallTime(0.0)
    , m_navigationStart(0.0)
    , m_unloadEventStart(0.0)
    , m_unloadEventEnd(0.0)
    , m_redirectStart(0.0)
    , m_redirectEnd(0.0)
    , m_redirectCount(0)
    , m_fetchStart(0.0)
    , m_responseEnd(0.0)
    , m_loadEventStart(0.0)
    , m_loadEventEnd(0.0)
    , m_hasCrossOriginRedirect(false)
    , m_hasSameOriginAsPreviousDocument(false)
{
}

double DocumentLoadTiming::monotonicTimeToZeroBasedDocumentTime(double monotonicTime) const
{
    if (!monotonicTime)
        return 0.0;
    return monotonicTime - m_referenceMonotonicTime;
}

double DocumentLoadTiming::monotonicTimeToPseudoWallTime(double monotonicTime) const
{
    if (!monotonicTime)
        return 0.0;
    return m_referenceWallTime + monotonicTime - m_referenceMonotonicTime;
}

double DocumentLoadTiming::pseudoWallTimeToMonotonicTime(double pseudoWallTime) const
{
    if (!pseudoWallTime)
        return 0.0;
    return m_referenceMonotonicTime + pseudoWallTime - m_referenceWallTime;
}

void DocumentLoadTiming::markNavigationStart()
{
    TRACE_EVENT_MARK("blink.user_timing", "navigationStart");
    ASSERT(!m_navigationStart && !m_referenceMonotonicTime && !m_referenceWallTime);

    m_navigationStart = m_referenceMonotonicTime = monotonicallyIncreasingTime();
    m_referenceWallTime = currentTime();
}

void DocumentLoadTiming::setNavigationStart(double navigationStart)
{
    TRACE_EVENT_MARK_WITH_TIMESTAMP("blink.user_timing", "navigationStart", navigationStart);
    ASSERT(m_referenceMonotonicTime && m_referenceWallTime);
    m_navigationStart = navigationStart;

    // |m_referenceMonotonicTime| and |m_referenceWallTime| represent
    // navigationStart. When the embedder sets navigationStart (because the
    // navigation started earlied on the browser side), we need to adjust these
    // as well.
    m_referenceWallTime = monotonicTimeToPseudoWallTime(navigationStart);
    m_referenceMonotonicTime = navigationStart;
}

void DocumentLoadTiming::addRedirect(const KURL& redirectingUrl, const KURL& redirectedUrl)
{
    m_redirectCount++;
    if (!m_redirectStart) {
        setRedirectStart(m_fetchStart);
    }
    markRedirectEnd();
    markFetchStart();

    // Check if the redirected url is allowed to access the redirecting url's timing information.
    RefPtr<SecurityOrigin> redirectedSecurityOrigin = SecurityOrigin::create(redirectedUrl);
    m_hasCrossOriginRedirect |= !redirectedSecurityOrigin->canRequest(redirectingUrl);
}

void DocumentLoadTiming::markUnloadEventStart()
{
    TRACE_EVENT_MARK("blink.user_timing", "unloadEventStart");
    m_unloadEventStart = monotonicallyIncreasingTime();
}

void DocumentLoadTiming::markUnloadEventEnd()
{
    TRACE_EVENT_MARK("blink.user_timing", "unloadEventEnd");
    m_unloadEventEnd = monotonicallyIncreasingTime();
}

void DocumentLoadTiming::markFetchStart()
{
    TRACE_EVENT_MARK("blink.user_timing", "fetchStart");
    m_fetchStart = monotonicallyIncreasingTime();
}

void DocumentLoadTiming::setResponseEnd(double responseEnd)
{
    TRACE_EVENT_MARK_WITH_TIMESTAMP("blink.user_timing", "responseEnd", responseEnd);
    m_responseEnd = responseEnd;
}

void DocumentLoadTiming::markLoadEventStart()
{
    TRACE_EVENT_MARK("blink.user_timing", "loadEventStart");
    m_loadEventStart = monotonicallyIncreasingTime();
}

void DocumentLoadTiming::markLoadEventEnd()
{
    TRACE_EVENT_MARK("blink.user_timing", "loadEventEnd");
    m_loadEventEnd = monotonicallyIncreasingTime();
}

void DocumentLoadTiming::setRedirectStart(double redirectStart)
{
    TRACE_EVENT_MARK_WITH_TIMESTAMP("blink.user_timing", "redirectStart", redirectStart);
    m_redirectStart = m_fetchStart;
}

void DocumentLoadTiming::markRedirectEnd()
{
    TRACE_EVENT_MARK("blink.user_timing", "redirectEnd");
    m_redirectEnd = monotonicallyIncreasingTime();
}

} // namespace blink
