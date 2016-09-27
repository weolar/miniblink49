/*
 * Copyright (C) 2006, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Google Inc. All rights reserved.
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
 */

#include "config.h"
#include "platform/Timer.h"

#include "platform/TraceEvent.h"
#include "public/platform/Platform.h"
#include "public/platform/WebScheduler.h"
#include "wtf/AddressSanitizer.h"
#include "wtf/Atomics.h"
#include "wtf/CurrentTime.h"
#include "wtf/HashSet.h"
#include <algorithm>
#include <limits.h>
#include <limits>
#include <math.h>

namespace blink {

TimerBase::TimerBase()
    : m_nextFireTime(0)
    , m_unalignedNextFireTime(0)
    , m_repeatInterval(0)
    , m_cancellableTimerTask(nullptr)
    , m_webScheduler(Platform::current()->currentThread()->scheduler())
#if ENABLE(ASSERT)
    , m_thread(currentThread())
#endif
{
}

TimerBase::~TimerBase()
{
    stop();
}

void TimerBase::start(double nextFireInterval, double repeatInterval, const WebTraceLocation& caller)
{
    ASSERT(m_thread == currentThread());

    m_location = caller;
    m_repeatInterval = repeatInterval;
    setNextFireTime(monotonicallyIncreasingTime(), nextFireInterval);
}

void TimerBase::stop()
{
    ASSERT(m_thread == currentThread());

    m_repeatInterval = 0;
    m_nextFireTime = 0;
    if (m_cancellableTimerTask)
        m_cancellableTimerTask->cancel();
    m_cancellableTimerTask = nullptr;
}

double TimerBase::nextFireInterval() const
{
    ASSERT(isActive());
    double current = monotonicallyIncreasingTime();
    if (m_nextFireTime < current)
        return 0;
    return m_nextFireTime - current;
}

void TimerBase::setNextFireTime(double now, double delay)
{
    ASSERT(m_thread == currentThread());

    m_unalignedNextFireTime = now + delay;

    double newTime = alignedFireTime(m_unalignedNextFireTime);
    if (m_nextFireTime != newTime) {
        m_nextFireTime = newTime;
        if (m_cancellableTimerTask)
            m_cancellableTimerTask->cancel();
        m_cancellableTimerTask = new CancellableTimerTask(this);
        if (newTime != m_unalignedNextFireTime) {
            // If the timer is being aligned, use postTimerTaskAt() to schedule it
            // so that the relative order of aligned timers is preserved.
            // TODO(skyostil): Move timer alignment into the scheduler.
            m_webScheduler->postTimerTaskAt(m_location, m_cancellableTimerTask, m_nextFireTime);
        } else {
            // Round the delay up to the nearest millisecond to be consistant with the
            // previous behavior of BlinkPlatformImpl::setSharedTimerFireInterval.
            long long delayMs = static_cast<long long>(ceil((newTime - now) * 1000.0));
            if (delayMs < 0)
                delayMs = 0;
            m_webScheduler->postTimerTask(m_location, m_cancellableTimerTask, delayMs);
        }
    }
}

NO_LAZY_SWEEP_SANITIZE_ADDRESS
void TimerBase::runInternal()
{
    if (!canFire())
        return;

    TRACE_EVENT0("blink", "TimerBase::run");
    ASSERT_WITH_MESSAGE(m_thread == currentThread(), "Timer posted by %s %s was run on a different thread", m_location.functionName(), m_location.fileName());
    TRACE_EVENT_SET_SAMPLING_STATE("blink", "BlinkInternal");

    m_nextFireTime = 0;
    // Note: repeating timers drift, but this is preserving the functionality of the old timer heap.
    // See crbug.com/328700.
    if (m_repeatInterval)
        setNextFireTime(monotonicallyIncreasingTime(), m_repeatInterval);
    fired();
    TRACE_EVENT_SET_SAMPLING_STATE("blink", "Sleeping");
}

void TimerBase::didChangeAlignmentInterval(double now)
{
    setNextFireTime(now, m_unalignedNextFireTime - now);
}

double TimerBase::nextUnalignedFireInterval() const
{
    ASSERT(isActive());
    return std::max(m_unalignedNextFireTime - monotonicallyIncreasingTime(), 0.0);
}

bool TimerBase::Comparator::operator()(const TimerBase* a, const TimerBase* b) const
{
    return a->m_unalignedNextFireTime < b->m_unalignedNextFireTime;
}

} // namespace blink
