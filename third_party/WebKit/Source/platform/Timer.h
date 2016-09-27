/*
 * Copyright (C) 2006 Apple Computer, Inc.  All rights reserved.
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

#ifndef Timer_h
#define Timer_h

#include "platform/PlatformExport.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebTraceLocation.h"
#include "wtf/AddressSanitizer.h"
#include "wtf/Noncopyable.h"
#include "wtf/Threading.h"
#include "wtf/Vector.h"

namespace blink {

// Time intervals are all in seconds.

class PLATFORM_EXPORT TimerBase {
    WTF_MAKE_NONCOPYABLE(TimerBase);
public:
    TimerBase();
    virtual ~TimerBase();

    void start(double nextFireInterval, double repeatInterval, const WebTraceLocation&);

    void startRepeating(double repeatInterval, const WebTraceLocation& caller)
    {
        start(repeatInterval, repeatInterval, caller);
    }
    void startOneShot(double interval, const WebTraceLocation& caller)
    {
        start(interval, 0, caller);
    }

    void stop();
    bool isActive() const;
    const WebTraceLocation& location() const { return m_location; }

    double nextFireInterval() const;
    double nextUnalignedFireInterval() const;
    double repeatInterval() const { return m_repeatInterval; }

    void augmentRepeatInterval(double delta) {
        double now = monotonicallyIncreasingTime();
        setNextFireTime(now, m_nextFireTime - now + delta);
        m_repeatInterval += delta;
    }

    void didChangeAlignmentInterval(double now);

    struct PLATFORM_EXPORT Comparator {
        bool operator()(const TimerBase* a, const TimerBase* b) const;
    };

private:
    virtual void fired() = 0;

    NO_LAZY_SWEEP_SANITIZE_ADDRESS
    virtual bool canFire() const { return true; }

    virtual double alignedFireTime(double fireTime) const { return fireTime; }

    void setNextFireTime(double now, double delay);

    void runInternal();

    class CancellableTimerTask final : public WebThread::Task {
        WTF_MAKE_NONCOPYABLE(CancellableTimerTask);
    public:
        explicit CancellableTimerTask(TimerBase* timer) : m_timer(timer) { }

        ~CancellableTimerTask() override
        {
            if (m_timer)
                m_timer->m_cancellableTimerTask = nullptr;
        }

        NO_LAZY_SWEEP_SANITIZE_ADDRESS
        void run() override
        {
            if (m_timer) {
                m_timer->m_cancellableTimerTask = nullptr;
                m_timer->runInternal();
                m_timer = nullptr;
            }
        }

        void cancel()
        {
            m_timer = nullptr;
        }

    private:
        TimerBase* m_timer; // NOT OWNED
    };

    double m_nextFireTime; // 0 if inactive
    double m_unalignedNextFireTime; // m_nextFireTime not considering alignment interval
    double m_repeatInterval; // 0 if not repeating
    WebTraceLocation m_location;
    CancellableTimerTask* m_cancellableTimerTask; // NOT OWNED
    WebScheduler* m_webScheduler; // Not owned.

#if ENABLE(ASSERT)
    ThreadIdentifier m_thread;
#endif

    friend class ThreadTimers;
    friend class TimerHeapLessThanFunction;
    friend class TimerHeapReference;
};

template<typename T, bool = IsGarbageCollectedType<T>::value>
class TimerIsObjectAliveTrait {
public:
    static bool isHeapObjectAlive(T*) { return true; }
};

template<typename T>
class TimerIsObjectAliveTrait<T, true> {
public:
    static bool isHeapObjectAlive(T* objectPointer)
    {
        return !Heap::willObjectBeLazilySwept(objectPointer);
    }
};

template <typename TimerFiredClass>
class Timer : public TimerBase {
public:
    typedef void (TimerFiredClass::*TimerFiredFunction)(Timer*);

    Timer(TimerFiredClass* o, TimerFiredFunction f)
        : m_object(o), m_function(f)
    {
    }

protected:
    void fired() override
    {
        (m_object->*m_function)(this);
    }

    NO_LAZY_SWEEP_SANITIZE_ADDRESS
    bool canFire() const override
    {
        // Oilpan: if a timer fires while Oilpan heaps are being lazily
        // swept, it is not safe to proceed if the object is about to
        // be swept (and this timer will be stopped while doing so.)
        return TimerIsObjectAliveTrait<TimerFiredClass>::isHeapObjectAlive(m_object);
    }

private:
    // FIXME: Oilpan: TimerBase should be moved to the heap and m_object should be traced.
    // This raw pointer is safe as long as Timer<X> is held by the X itself (That's the case
    // in the current code base).
    GC_PLUGIN_IGNORE("363031")
    TimerFiredClass* m_object;
    TimerFiredFunction m_function;
};

NO_LAZY_SWEEP_SANITIZE_ADDRESS
inline bool TimerBase::isActive() const
{
    ASSERT(m_thread == currentThread());
    return m_cancellableTimerTask;
}

} // namespace blink

#endif // Timer_h
