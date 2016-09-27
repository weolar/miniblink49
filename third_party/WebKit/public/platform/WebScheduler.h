// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebScheduler_h
#define WebScheduler_h

#include "WebCommon.h"
#include "public/platform/WebThread.h"

namespace blink {

class WebTraceLocation;

// This class is used to submit tasks and pass other information from Blink to
// the platform's scheduler.
class BLINK_PLATFORM_EXPORT WebScheduler {
public:
    virtual ~WebScheduler() { }

    // Called to prevent any more pending tasks from running. Must be called on
    // the associated WebThread.
    virtual void shutdown() { }

    // Returns true if there is high priority work pending on the associated WebThread
    // and the caller should yield to let the scheduler service that work.
    // Must be called on the associated WebThread.
    virtual bool shouldYieldForHighPriorityWork() { return false; }

    // Returns true if a currently running idle task could exceed its deadline
    // without impacting user experience too much. This should only be used if
    // there is a task which cannot be pre-empted and is likely to take longer
    // than the largest expected idle task deadline. It should NOT be polled to
    // check whether more work can be performed on the current idle task after
    // its deadline has expired - post a new idle task for the continuation of
    // the work in this case.
    // Must be called from the associated WebThread.
    virtual bool canExceedIdleDeadlineIfRequired() { return false; }

    // Schedule an idle task to run the associated WebThread. For non-critical
    // tasks which may be reordered relative to other task types and may be
    // starved for an arbitrarily long time if no idle time is available.
    // Takes ownership of |IdleTask|. Can be called from any thread.
    virtual void postIdleTask(const WebTraceLocation&, WebThread::IdleTask*) { }

    // Like postIdleTask but guarantees that the posted task will not run
    // nested within an already-running task. Posting an idle task as
    // non-nestable may not affect when the task gets run, or it could
    // make it run later than it normally would, but it won't make it
    // run earlier than it normally would.
    virtual void postNonNestableIdleTask(const WebTraceLocation&, WebThread::IdleTask*) { }

    // Like postIdleTask but does not run the idle task until after some other
    // task has run. This enables posting of a task which won't stop the Blink
    // main thread from sleeping, but will start running after it wakes up.
    // Takes ownership of |IdleTask|. Can be called from any thread.
    virtual void postIdleTaskAfterWakeup(const WebTraceLocation&, WebThread::IdleTask*) { }

    // Schedule a loading task to be run on the the associated WebThread. Loading
    // tasks usually have the default priority, but may be deprioritised
    // when the user is interacting with the device.
    // Takes ownership of |WebThread::Task|. Can be called from any thread.
    virtual void postLoadingTask(const WebTraceLocation&, WebThread::Task*) { }

    // Schedule a timer task to be run on the the associated WebThread. Timer Tasks
    // tasks usually have the default priority, but may be delayed
    // when the user is interacting with the device.
    // Takes ownership of |WebThread::Task|. Can be called from any thread.
    virtual void postTimerTask(const WebTraceLocation&, WebThread::Task*, long long delayMs) {}

    // Schedule a timer task to be run on the the associated WebThread. Timer Tasks
    // tasks usually have the default priority, but may be delayed
    // when the user is interacting with the device.
    // |monotonicTime| is in the timebase of WTF::monotonicallyIncreasingTime().
    // Takes ownership of |WebThread::Task|. Can be called from any thread.
    virtual void postTimerTaskAt(const WebTraceLocation&, WebThread::Task*, double monotonicTime) {}

    // Suspends the timer queue and increments the timer queue suspension count.
    // May only be called from the main thread.
    virtual void suspendTimerQueue() { }

    // Decrements the timer queue suspension count and re-enables the timer queue
    // if the suspension count is zero and the current scheduler policy allows it.
    virtual void resumeTimerQueue() { }

#ifdef INSIDE_BLINK
    // Helpers for posting bound functions as tasks.
    typedef Function<void(double deadlineSeconds)> IdleTask;
    typedef Function<void()> Task;

    void postIdleTask(const WebTraceLocation&, PassOwnPtr<IdleTask>);
    void postNonNestableIdleTask(const WebTraceLocation&, PassOwnPtr<IdleTask>);
    void postIdleTaskAfterWakeup(const WebTraceLocation&, PassOwnPtr<IdleTask>);
    void postLoadingTask(const WebTraceLocation&, PassOwnPtr<Task>);
#endif
};

} // namespace blink

#endif // WebScheduler_h
