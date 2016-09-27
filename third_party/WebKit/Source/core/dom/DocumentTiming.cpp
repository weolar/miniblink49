// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/DocumentTiming.h"

#include "platform/TraceEvent.h"

namespace blink {

DocumentTiming::DocumentTiming()
    : m_domLoading(0.0)
    , m_domInteractive(0.0)
    , m_domContentLoadedEventStart(0.0)
    , m_domContentLoadedEventEnd(0.0)
    , m_domComplete(0.0)
    , m_firstLayout(0.0)
{
}

void DocumentTiming::setDomLoading(double domLoading)
{
    TRACE_EVENT_MARK_WITH_TIMESTAMP("blink.user_timing", "domLoading", domLoading);
    m_domLoading = domLoading;
}

void DocumentTiming::setDomInteractive(double domInteractive)
{
    TRACE_EVENT_MARK_WITH_TIMESTAMP("blink.user_timing", "domInteractive", domInteractive);
    m_domInteractive = domInteractive;
}

void DocumentTiming::setDomContentLoadedEventStart(double domContentLoadedEventStart)
{
    TRACE_EVENT_MARK_WITH_TIMESTAMP("blink.user_timing", "domContentLoadedEventStart", domContentLoadedEventStart);
    m_domContentLoadedEventStart = domContentLoadedEventStart;
}

void DocumentTiming::setDomContentLoadedEventEnd(double domContentLoadedEventEnd)
{
    TRACE_EVENT_MARK_WITH_TIMESTAMP("blink.user_timing", "domContentLoadedEventEnd", domContentLoadedEventEnd);
    m_domContentLoadedEventEnd = domContentLoadedEventEnd;
}

void DocumentTiming::setDomComplete(double domComplete)
{
    TRACE_EVENT_MARK_WITH_TIMESTAMP("blink.user_timing", "domComplete", domComplete);
    m_domComplete = domComplete;
}

void DocumentTiming::setFirstLayout(double firstLayout)
{
    TRACE_EVENT_MARK_WITH_TIMESTAMP("blink.user_timing", "firstLayout", firstLayout);
    m_firstLayout = firstLayout;
}

} // namespace blink
