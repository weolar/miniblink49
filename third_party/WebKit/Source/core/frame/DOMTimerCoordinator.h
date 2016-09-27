// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DOMTimerCoordinator_h
#define DOMTimerCoordinator_h

#include "platform/heap/Handle.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class DOMTimer;
class ExecutionContext;
class ScheduledAction;

// Maintains a set of DOMTimers for a given page or
// worker. DOMTimerCoordinator assigns IDs to timers; these IDs are
// the ones returned to web authors from setTimeout or setInterval. It
// also tracks recursive creation or iterative scheduling of timers,
// which is used as a signal for throttling repetitive timers.
class DOMTimerCoordinator {
    DISALLOW_ALLOCATION();
    WTF_MAKE_NONCOPYABLE(DOMTimerCoordinator);
public:
    DOMTimerCoordinator();

    // Creates and installs a new timer. Returns the assigned ID.
    int installNewTimeout(ExecutionContext*, PassOwnPtrWillBeRawPtr<ScheduledAction>, int timeout, bool singleShot);

    // Removes and disposes the timer with the specified ID, if any. This may
    // destroy the timer.
    void removeTimeoutByID(int id);

    // Notifies registered timers that
    // ExecutionContext::timerAlignmentInterval has changed so that
    // timers can adjust their schedule to the new alignment interval.
    void didChangeTimerAlignmentInterval();

    // Timers created during the execution of other timers, and
    // repeating timers, are throttled. Timer nesting level tracks the
    // number of linked timers or repetitions of a timer. See
    // https://html.spec.whatwg.org/#timers
    int timerNestingLevel() { return m_timerNestingLevel; }

    // Sets the timer nesting level. Set when a timer executes so that
    // any timers created while the timer is executing will incur a
    // deeper timer nesting level, see DOMTimer::DOMTimer.
    void setTimerNestingLevel(int level) { m_timerNestingLevel = level; }

    DECLARE_TRACE(); // Oilpan.

private:
    int nextID();

    using TimeoutMap = WillBeHeapHashMap<int, RefPtrWillBeMember<DOMTimer>>;
    TimeoutMap m_timers;

    int m_circularSequentialID;
    int m_timerNestingLevel;
};

} // namespace blink

#endif // DOMTimerCoordinator_h
