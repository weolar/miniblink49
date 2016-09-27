// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SafePoint_h
#define SafePoint_h

#include "platform/heap/ThreadState.h"
#include "wtf/ThreadingPrimitives.h"

namespace blink {

class SafePointScope final {
    WTF_MAKE_NONCOPYABLE(SafePointScope);
public:
    explicit SafePointScope(ThreadState::StackState stackState, ThreadState* state = ThreadState::current())
        : m_state(state)
    {
        if (m_state) {
            RELEASE_ASSERT(!m_state->isAtSafePoint());
            m_state->enterSafePoint(stackState, this);
        }
    }

    ~SafePointScope()
    {
        if (m_state)
            m_state->leaveSafePoint();
    }

private:
    ThreadState* m_state;
};

// The SafePointAwareMutexLocker is used to enter a safepoint while waiting for
// a mutex lock. It also ensures that the lock is not held while waiting for a GC
// to complete in the leaveSafePoint method, by releasing the lock if the
// leaveSafePoint method cannot complete without blocking, see
// SafePointBarrier::checkAndPark.
class SafePointAwareMutexLocker final {
    WTF_MAKE_NONCOPYABLE(SafePointAwareMutexLocker);
public:
    explicit SafePointAwareMutexLocker(MutexBase& mutex, ThreadState::StackState stackState = ThreadState::HeapPointersOnStack)
        : m_mutex(mutex)
        , m_locked(false)
    {
        ThreadState* state = ThreadState::current();
        do {
            bool leaveSafePoint = false;
            // We cannot enter a safepoint if we are currently sweeping. In that
            // case we just try to acquire the lock without being at a safepoint.
            // If another thread tries to do a GC at that time it might time out
            // due to this thread not being at a safepoint and waiting on the lock.
            if (!state->sweepForbidden() && !state->isAtSafePoint()) {
                state->enterSafePoint(stackState, this);
                leaveSafePoint = true;
            }
            m_mutex.lock();
            m_locked = true;
            if (leaveSafePoint) {
                // When leaving the safepoint we might end up release the mutex
                // if another thread is requesting a GC, see
                // SafePointBarrier::checkAndPark. This is the case where we
                // loop around to reacquire the lock.
                state->leaveSafePoint(this);
            }
        } while (!m_locked);
    }

    ~SafePointAwareMutexLocker()
    {
        ASSERT(m_locked);
        m_mutex.unlock();
    }

private:
    friend class SafePointBarrier;

    void reset()
    {
        ASSERT(m_locked);
        m_mutex.unlock();
        m_locked = false;
    }

    MutexBase& m_mutex;
    bool m_locked;
};

class SafePointBarrier final {
    WTF_MAKE_NONCOPYABLE(SafePointBarrier);
public:
    SafePointBarrier();
    ~SafePointBarrier();

    // Request other attached threads that are not at safe points to park
    // themselves on safepoints.
    bool parkOthers();

    // Resume executions of other attached threads that are parked at
    // the safe points.
    void resumeOthers(bool barrierLocked = false);

    // Park this thread if there exists a request to park attached threads.
    // This method must be called at a safe point.
    void checkAndPark(ThreadState*, SafePointAwareMutexLocker* = nullptr);

    void enterSafePoint(ThreadState*);
    void leaveSafePoint(ThreadState*, SafePointAwareMutexLocker* = nullptr);

private:
    void doPark(ThreadState*, intptr_t* stackEnd);
    static void parkAfterPushRegisters(SafePointBarrier* barrier, ThreadState* state, intptr_t* stackEnd)
    {
        barrier->doPark(state, stackEnd);
    }
    void doEnterSafePoint(ThreadState*, intptr_t* stackEnd);
    static void enterSafePointAfterPushRegisters(SafePointBarrier* barrier, ThreadState* state, intptr_t* stackEnd)
    {
        barrier->doEnterSafePoint(state, stackEnd);
    }

    volatile int m_canResume;
    volatile int m_unparkedThreadCount;
    Mutex m_mutex;
    ThreadCondition m_parked;
    ThreadCondition m_resume;
};

} // namespace blink

#endif
