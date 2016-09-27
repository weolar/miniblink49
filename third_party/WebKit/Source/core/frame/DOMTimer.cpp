/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "config.h"
#include "core/frame/DOMTimer.h"

#include "core/dom/ExecutionContext.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/inspector/InspectorTraceEvents.h"
#include "platform/TraceEvent.h"
#include "wtf/CurrentTime.h"

namespace blink {

static const int maxIntervalForUserGestureForwarding = 1000; // One second matches Gecko.
static const int maxTimerNestingLevel = 5;
static const double oneMillisecond = 0.001;
// Chromium uses a minimum timer interval of 4ms. We'd like to go
// lower; however, there are poorly coded websites out there which do
// create CPU-spinning loops.  Using 4ms prevents the CPU from
// spinning too busily and provides a balance between CPU spinning and
// the smallest possible interval timer.
static const double minimumInterval = 0.004;

static inline bool shouldForwardUserGesture(int interval, int nestingLevel)
{
    return UserGestureIndicator::processingUserGesture()
        && interval <= maxIntervalForUserGestureForwarding
        && nestingLevel == 1; // Gestures should not be forwarded to nested timers.
}

double DOMTimer::hiddenPageAlignmentInterval()
{
    // Timers on hidden pages are aligned so that they fire once per
    // second at most.
    return 1.0;
}

double DOMTimer::visiblePageAlignmentInterval()
{
    // Alignment does not apply to timers on visible pages.
    return 0;
}

int DOMTimer::install(ExecutionContext* context, PassOwnPtrWillBeRawPtr<ScheduledAction> action, int timeout, bool singleShot)
{
    int timeoutID = context->timers()->installNewTimeout(context, action, timeout, singleShot);
    TRACE_EVENT_INSTANT1("devtools.timeline", "TimerInstall", TRACE_EVENT_SCOPE_THREAD, "data", InspectorTimerInstallEvent::data(context, timeoutID, timeout, singleShot));
    InspectorInstrumentation::didInstallTimer(context, timeoutID, timeout, singleShot);
    return timeoutID;
}

void DOMTimer::removeByID(ExecutionContext* context, int timeoutID)
{
    context->timers()->removeTimeoutByID(timeoutID);
    TRACE_EVENT_INSTANT1("devtools.timeline", "TimerRemove", TRACE_EVENT_SCOPE_THREAD, "data", InspectorTimerRemoveEvent::data(context, timeoutID));
    InspectorInstrumentation::didRemoveTimer(context, timeoutID);
}

DOMTimer::DOMTimer(ExecutionContext* context, PassOwnPtrWillBeRawPtr<ScheduledAction> action, int interval, bool singleShot, int timeoutID)
    : SuspendableTimer(context)
    , m_timeoutID(timeoutID)
    , m_nestingLevel(context->timers()->timerNestingLevel() + 1)
    , m_action(action)
{
    ASSERT(timeoutID > 0);
    if (shouldForwardUserGesture(interval, m_nestingLevel))
        m_userGestureToken = UserGestureIndicator::currentToken();

    double intervalMilliseconds = std::max(oneMillisecond, interval * oneMillisecond);
    if (intervalMilliseconds < minimumInterval && m_nestingLevel >= maxTimerNestingLevel)
        intervalMilliseconds = minimumInterval;
    if (singleShot)
        startOneShot(intervalMilliseconds, FROM_HERE);
    else
        startRepeating(intervalMilliseconds, FROM_HERE);
}

DOMTimer::~DOMTimer()
{
}

void DOMTimer::disposeTimer()
{
    m_action = nullptr;
    m_userGestureToken = nullptr;
    stop();
}

void DOMTimer::fired()
{
    ExecutionContext* context = executionContext();
    ASSERT(context);
    context->timers()->setTimerNestingLevel(m_nestingLevel);
    ASSERT(!context->activeDOMObjectsAreSuspended());
    // Only the first execution of a multi-shot timer should get an affirmative user gesture indicator.
    UserGestureIndicator gestureIndicator(m_userGestureToken.release());

    TRACE_EVENT1("devtools.timeline", "TimerFire", "data", InspectorTimerFireEvent::data(context, m_timeoutID));
    InspectorInstrumentationCookie cookie = InspectorInstrumentation::willFireTimer(context, m_timeoutID);

    // Simple case for non-one-shot timers.
    if (isActive()) {
        if (repeatInterval() && repeatInterval() < minimumInterval) {
            m_nestingLevel++;
            if (m_nestingLevel >= maxTimerNestingLevel)
                augmentRepeatInterval(minimumInterval - repeatInterval());
        }

        // No access to member variables after this point, it can delete the timer.
        m_action->execute(context);

        InspectorInstrumentation::didFireTimer(cookie);

        return;
    }

    RefPtrWillBeRawPtr<DOMTimer> protect(this);

    // Unregister the timer from ExecutionContext before executing the action
    // for one-shot timers.
    OwnPtrWillBeRawPtr<ScheduledAction> action = m_action.release();
    context->timers()->removeTimeoutByID(m_timeoutID);

    action->execute(context);

    InspectorInstrumentation::didFireTimer(cookie);
    TRACE_EVENT_INSTANT1(TRACE_DISABLED_BY_DEFAULT("devtools.timeline"), "UpdateCounters", TRACE_EVENT_SCOPE_THREAD, "data", InspectorUpdateCountersEvent::data());

    // ExecutionContext might be already gone when we executed action->execute().
    if (executionContext())
        executionContext()->timers()->setTimerNestingLevel(0);
}

void DOMTimer::stop()
{
    SuspendableTimer::stop();
    // Need to release JS objects potentially protected by ScheduledAction
    // because they can form circular references back to the ExecutionContext
    // which will cause a memory leak.
    m_action.clear();
}

double DOMTimer::alignedFireTime(double fireTime) const
{
    double alignmentInterval = executionContext()->timerAlignmentInterval();
    if (alignmentInterval) {
        double currentTime = monotonicallyIncreasingTime();
        if (fireTime <= currentTime)
            return fireTime;

        // When a repeating timer is scheduled for exactly the
        // background page alignment interval, because it's impossible
        // for the timer to be rescheduled instantaneously, it misses
        // every other fire time. Avoid this by looking at the next
        // fire time rounded both down and up.

        double alignedTimeRoundedDown = floor(fireTime / alignmentInterval) * alignmentInterval;
        double alignedTimeRoundedUp = ceil(fireTime / alignmentInterval) * alignmentInterval;

        // If the version rounded down is in the past, discard it
        // immediately.

        if (alignedTimeRoundedDown <= currentTime)
            return alignedTimeRoundedUp;

        // Only use the rounded-down time if it's within a certain
        // tolerance of the fire time. This avoids speeding up timers
        // on background pages in the common case.

        if (fireTime - alignedTimeRoundedDown < minimumInterval)
            return alignedTimeRoundedDown;

        return alignedTimeRoundedUp;
    }

    return fireTime;
}

DEFINE_TRACE(DOMTimer)
{
    visitor->trace(m_action);
    SuspendableTimer::trace(visitor);
}

} // namespace blink
