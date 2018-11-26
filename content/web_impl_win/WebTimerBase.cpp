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
#include "WebTimerBase.h"

#include "WebThreadImpl.h"
#include <limits.h>
#include <limits>
#include <math.h>

#include "third_party/WebKit/Source/platform/Task.h"
#include "ActivatingTimerCheck.h"

namespace content {

// Timers are stored in a heap data structure, used to implement a priority queue.
// This allows us to efficiently determine which timer needs to fire the soonest.
// Then we set a single shared system timer to fire at that time.
//
// When a timer's "next fire time" changes, we need to move it around in the priority queue.

// Class to represent elements in the heap when calling the standard library heap algorithms.
// Maintains the m_heapIndex value in the timers themselves, which allows us to do efficient
// modification of the heap.
class TimerHeapElement {
public:
    explicit TimerHeapElement(int i, WebThreadImpl* threadTimers)
        : m_index(i)
    {
        m_threadTimers = (threadTimers);
        m_timer = (timerHeap()[m_index]);
        checkConsistency(); 
    }

    std::vector<WebTimerBase*>& timerHeap()
    {
        return m_threadTimers->timerHeap();
    }

    const std::vector<WebTimerBase*>& timerHeap() const
    {
        return m_threadTimers->timerHeap();
    }

    TimerHeapElement(const TimerHeapElement&);
    TimerHeapElement& operator=(const TimerHeapElement&);

    WebTimerBase* timer() const { return m_timer; }

    void checkConsistency() const
    {
        ASSERT(m_index >= 0);
        ASSERT(m_index < static_cast<int>(timerHeap().size()));
    }

private:
    TimerHeapElement();

    int m_index;
    WebTimerBase* m_timer;

    WebThreadImpl* m_threadTimers;
};

TimerHeapElement::TimerHeapElement(const TimerHeapElement& o)
    : m_index(-1), m_timer(o.timer())
{
}

TimerHeapElement& TimerHeapElement::operator=(const TimerHeapElement& o)
{
    WebTimerBase* t = o.timer();
    m_timer = t;
    if (m_index != -1) {
        checkConsistency();
        timerHeap()[m_index] = t;
        t->m_heapIndex = m_index;
    }
    return *this;
}

bool operator<(const TimerHeapElement& a, const TimerHeapElement& b)
{
    // The comparisons below are "backwards" because the heap puts the largest 
    // element first and we want the lowest time to be the first one in the heap.
    double aFireTime = a.timer()->m_nextFireTime;
    double bFireTime = b.timer()->m_nextFireTime;

    if (bFireTime != aFireTime)
        return bFireTime < aFireTime;

#if 0
    int aPriority = a.timer()->m_priority;
    int bPriority = b.timer()->m_priority;
    if (aPriority != bPriority)
        return aPriority < bPriority;
#endif

    // We need to look at the difference of the insertion orders instead of comparing the two 
    // outright in case of overflow. 
    unsigned difference = a.timer()->m_heapInsertionOrder - b.timer()->m_heapInsertionOrder;
    return difference < UINT_MAX / 2;
}

// ----------------

// Class to represent iterators in the heap when calling the standard library heap algorithms.
// Returns TimerHeapElement for elements in the heap rather than the WebTimerBase pointers themselves.
class TimerHeapIterator : public std::iterator<std::random_access_iterator_tag, TimerHeapElement, int> {
public:
    TimerHeapIterator(WebThreadImpl* threadTimers) : m_index(-1), m_threadTimers(threadTimers) { }
    TimerHeapIterator(int i, WebThreadImpl* threadTimers) : m_index(i), m_threadTimers(threadTimers) { checkConsistency(); }

    TimerHeapIterator& operator++() { checkConsistency(); ++m_index; checkConsistency(); return *this; }
    TimerHeapIterator operator++(int) 
    { 
        checkConsistency();
        checkConsistency(1);
        return TimerHeapIterator(m_index++, m_threadTimers);
    }

    TimerHeapIterator& operator--() { checkConsistency(); --m_index; checkConsistency(); return *this; }
    TimerHeapIterator operator--(int) { checkConsistency(); checkConsistency(-1); return TimerHeapIterator(m_index--, m_threadTimers); }

    TimerHeapIterator& operator+=(int i) { checkConsistency(); m_index += i; checkConsistency(); return *this; }
    TimerHeapIterator& operator-=(int i) { checkConsistency(); m_index -= i; checkConsistency(); return *this; }

    TimerHeapElement operator*() const { return TimerHeapElement(m_index, m_threadTimers); }
    TimerHeapElement operator[](int i) const { return TimerHeapElement(m_index + i, m_threadTimers); }

    int index() const { return m_index; }

    void checkConsistency(int offset = 0) const
    {
        ASSERT_UNUSED(offset, m_index + offset >= 0);
        ASSERT_UNUSED(offset, m_index + offset <= static_cast<int>(timerHeap().size()));
    }
    
    std::vector<WebTimerBase*>& timerHeap() { return m_threadTimers->timerHeap(); }
    const std::vector<WebTimerBase*>& timerHeap() const  { return m_threadTimers->timerHeap(); }

    WebThreadImpl* threadTimers() { return m_threadTimers; }

private:
    int m_index;

    WebThreadImpl* m_threadTimers;
};

inline bool operator==(TimerHeapIterator a, TimerHeapIterator b) { return a.index() == b.index(); }
inline bool operator!=(TimerHeapIterator a, TimerHeapIterator b) { return a.index() != b.index(); }
inline bool operator<(TimerHeapIterator a, TimerHeapIterator b) { return a.index() < b.index(); }

inline TimerHeapIterator operator+(TimerHeapIterator a, int b) { return TimerHeapIterator(a.index() + b, a.threadTimers()); }
inline TimerHeapIterator operator+(int a, TimerHeapIterator b) { return TimerHeapIterator(a + b.index(), b.threadTimers()); }

inline TimerHeapIterator operator-(TimerHeapIterator a, int b) { return TimerHeapIterator(a.index() - b, a.threadTimers()); }
inline int operator-(TimerHeapIterator a, TimerHeapIterator b) { return a.index() - b.index(); }

// ----------------

std::vector<WebTimerBase*>& WebTimerBase::timerHeap()
{
    return m_threadTimers->timerHeap();
}

const std::vector<WebTimerBase*>& WebTimerBase::timerHeap() const
{
    return m_threadTimers->timerHeap();
}

#ifndef NDEBUG
extern ActivatingTimerCheck* gActivatingTimerCheck;
#endif

WebTimerBase::WebTimerBase(WebThreadImpl* threadTimers, const blink::WebTraceLocation& location, blink::WebThread::Task* task, int priority)
    : m_nextFireTime(0)
    , m_threadTimers(threadTimers)
    , m_repeatInterval(0)
    , m_heapIndex(-1)
    , m_location(location)
    , m_task(task)
    , m_ref(0)
    , m_priority(priority)
#ifndef NDEBUG
    , m_thread(currentThread())
#endif
{
//     String out = String::format(" WebTimerBase::WebTimerBase: %p %x\n", this, ::GetCurrentThreadId());
//     OutputDebugStringW(out.charactersWithNullTermination().data());
#ifndef NDEBUG
    gActivatingTimerCheck->add(this);
#endif
}

WebTimerBase::~WebTimerBase()
{
    stop();
    ASSERT(!inHeap());

    if (m_task)
        delete m_task;
    m_task = nullptr;
#ifndef NDEBUG
    gActivatingTimerCheck->remove(this);
#endif
//     String out = String::format(" WebTimerBase::~WebTimerBase: %p %x\n", this, ::GetCurrentThreadId());
//     OutputDebugStringW(out.charactersWithNullTermination().data());
}

void WebTimerBase::ref()
{
    ++m_ref;
}

void WebTimerBase::deref()
{
    --m_ref;
    if (0 == m_ref)
        delete this;
}

int WebTimerBase::refCount() const
{
    return m_ref;
}

void WebTimerBase::startFromOtherThread(double interval, double* createTimeOnOtherThread, unsigned* heapInsertionOrder)
{
    if (!createTimeOnOtherThread || !heapInsertionOrder) {
        start(interval, 0);
        return;
    }

    ASSERT(m_thread == currentThread());
    m_repeatInterval = 0;
    setNextFireTime(*createTimeOnOtherThread + interval, heapInsertionOrder);
}

void WebTimerBase::start(double nextFireInterval, double repeatInterval)
{
    ASSERT(m_thread == currentThread());

    m_repeatInterval = repeatInterval;
    setNextFireTime(currentTime() + nextFireInterval, nullptr);
}

void WebTimerBase::stop()
{
    ASSERT(m_thread == currentThread());

    m_repeatInterval = 0;
    setNextFireTime(0, nullptr);

    ASSERT(m_nextFireTime == 0);
    ASSERT(m_repeatInterval == 0);
    ASSERT(!inHeap());
}

double WebTimerBase::nextFireInterval() const
{
    ASSERT(isActive());
    double current = currentTime();
    if (m_nextFireTime < current)
        return 0;
    return m_nextFireTime - current;
}

void WebTimerBase::checkHeapIndex() const
{
    ASSERT(!timerHeap().empty());
    ASSERT(m_heapIndex >= 0);
    ASSERT(m_heapIndex < static_cast<int>(timerHeap().size()));
    ASSERT(timerHeap()[m_heapIndex] == this);
}

void WebTimerBase::checkConsistency() const
{
    // Timers should be in the heap if and only if they have a non-zero next fire time.
    ASSERT(inHeap() == (m_nextFireTime != 0));
    if (inHeap())
        checkHeapIndex();
}

void WebTimerBase::heapDecreaseKey()
{
    ASSERT(m_nextFireTime != 0);
    checkHeapIndex();
    std::push_heap(TimerHeapIterator(0, m_threadTimers), TimerHeapIterator(m_heapIndex + 1, m_threadTimers));
    checkHeapIndex();
}

void WebTimerBase::deleteLastOne()
{
    if (0 == timerHeap().size())
        return;
    WebTimerBase* timer = timerHeap().back();
    timerHeap().pop_back();
    m_threadTimers->appendUnusedTimerToDelete(timer);
}

void WebTimerBase::heapDelete()
{
    ASSERT(m_nextFireTime == 0);
    heapPop();
    deleteLastOne();
    m_heapIndex = -1;
}

void WebTimerBase::heapDeleteMin()
{
    ASSERT(m_nextFireTime == 0);
    heapPopMin();
    deleteLastOne();
    m_heapIndex = -1;
}

void WebTimerBase::heapIncreaseKey()
{
    ASSERT(m_nextFireTime != 0);
    heapPop();
    heapDecreaseKey();
}

void WebTimerBase::heapInsert()
{
    ASSERT(!inHeap());
    timerHeap().push_back(this);
    ref();
    m_heapIndex = timerHeap().size() - 1;
    heapDecreaseKey();
}

void WebTimerBase::heapPop()
{
    // Temporarily force this timer to have the minimum key so we can pop it.
    double fireTime = m_nextFireTime;
    m_nextFireTime = -std::numeric_limits<double>::infinity();
    heapDecreaseKey();
    heapPopMin();
    m_nextFireTime = fireTime;
}

void WebTimerBase::heapPopMin()
{
    ASSERT(this == timerHeap().front());
    checkHeapIndex();
    std::pop_heap(TimerHeapIterator(0, m_threadTimers), TimerHeapIterator(timerHeap().size(), m_threadTimers));
    checkHeapIndex();
    ASSERT(this == timerHeap().back());
}

void WebTimerBase::setNextFireTime(double newTime, unsigned* heapInsertionOrder)
{
    ASSERT(m_thread == currentThread());

    // Keep heap valid while changing the next-fire time.
    double oldTime = m_nextFireTime;
    if (oldTime != newTime) {
        m_nextFireTime = newTime;

//         static unsigned currentHeapInsertionOrder = 0;
//         m_heapInsertionOrder = atomicIncrement((volatile int *)&currentHeapInsertionOrder);
        if (heapInsertionOrder)
            m_heapInsertionOrder = *heapInsertionOrder;
        else
            m_heapInsertionOrder = WebThreadImpl::getNewCurrentHeapInsertionOrder();

        bool wasFirstTimerInHeap = m_heapIndex == 0;

        if (oldTime == 0)
            heapInsert();
        else if (newTime == 0)
            heapDelete();
        else if (newTime < oldTime)
            heapDecreaseKey();
        else
            heapIncreaseKey();

        bool isFirstTimerInHeap = m_heapIndex == 0;

        if (wasFirstTimerInHeap || isFirstTimerInHeap)
            m_threadTimers->updateSharedTimer();
    }

    checkConsistency();
}

void WebTimerBase::fireTimersInNestedEventLoop()
{
    // Redirect to WebThreadImpl.
    //threadGlobalData().threadTimers().fireTimersInNestedEventLoop();
}

void WebTimerBase::fired()
{
    m_task->run();
}

} // namespace content

