// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_ORDERED_SIMPLE_TASK_RUNNER_H_
#define CC_TEST_ORDERED_SIMPLE_TASK_RUNNER_H_

#include <limits>
#include <set>
#include <vector>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/logging.h"
#include "base/test/simple_test_tick_clock.h"
#include "base/test/test_simple_task_runner.h"
#include "base/trace_event/trace_event.h"

namespace cc {

// Subclass of TestPendingTask which has a unique ID for every task, supports
// being used inside a std::set and has debug tracing support.
class TestOrderablePendingTask : public base::TestPendingTask {
public:
    TestOrderablePendingTask();
    TestOrderablePendingTask(const tracked_objects::Location& location,
        const base::Closure& task,
        base::TimeTicks post_time,
        base::TimeDelta delay,
        TestNestability nestability);
    ~TestOrderablePendingTask();

    // operators needed by std::set and comparison
    bool operator==(const TestOrderablePendingTask& other) const;
    bool operator<(const TestOrderablePendingTask& other) const;

    // base::trace_event tracing functionality
    scoped_refptr<base::trace_event::ConvertableToTraceFormat> AsValue() const;
    void AsValueInto(base::trace_event::TracedValue* state) const;

private:
    static size_t task_id_counter;
    const size_t task_id_;
};

// This runs pending tasks based on task's post_time + delay.
// We should not execute a delayed task sooner than some of the queued tasks
// which don't have a delay even though it is queued early.
class OrderedSimpleTaskRunner : public base::SingleThreadTaskRunner {
public:
    OrderedSimpleTaskRunner(base::SimpleTestTickClock* now_src, bool advance_now);

    // base::TestSimpleTaskRunner implementation:
    bool PostDelayedTask(const tracked_objects::Location& from_here,
        const base::Closure& task,
        base::TimeDelta delay) override;
    bool PostNonNestableDelayedTask(const tracked_objects::Location& from_here,
        const base::Closure& task,
        base::TimeDelta delay) override;

    bool RunsTasksOnCurrentThread() const override;

    static base::TimeTicks AbsoluteMaxNow();

    // Set a maximum number of tasks to run at once. Useful as a timeout to
    // prevent infinite task loops.
    static const size_t kAbsoluteMaxTasks;
    void SetRunTaskLimit(size_t max_tasks) { max_tasks_ = max_tasks; }
    void ClearRunTaskLimit() { max_tasks_ = kAbsoluteMaxTasks; }

    // Allow task runner to advance now when running tasks.
    void SetAutoAdvanceNowToPendingTasks(bool advance_now)
    {
        advance_now_ = advance_now;
    }

    size_t NumPendingTasks() const;
    bool HasPendingTasks() const;
    base::TimeTicks NextTaskTime();
    base::TimeDelta DelayToNextTaskTime();

    // Run tasks while the callback returns true or too many tasks have been run.
    // Returns true if there are still pending tasks left.
    bool RunTasksWhile(base::Callback<bool(void)> condition);

    // Run tasks while *all* of the callbacks return true or too many tasks have
    // been run. Exits on the *first* condition which returns false, skipping
    // calling all remaining conditions. Conditions can have side effects,
    // including modifying the task queue.
    // Returns true if there are still pending tasks left.
    bool RunTasksWhile(const std::vector<base::Callback<bool(void)>>& conditions);

    // Convenience functions to run tasks with common conditions.

    // Run tasks which existed at the start of this call.
    // Return code indicates tasks still exist to run.
    bool RunPendingTasks();
    // Keep running tasks until no tasks are left.
    // Return code indicates tasks still exist to run which also indicates if
    // runner reached idle.
    bool RunUntilIdle();
    // Keep running tasks until given time period.
    // Return code indicates tasks still exist to run.
    bool RunUntilTime(base::TimeTicks time);
    bool RunForPeriod(base::TimeDelta period);

    // base::trace_event tracing functionality
    scoped_refptr<base::trace_event::ConvertableToTraceFormat> AsValue() const;
    virtual void AsValueInto(base::trace_event::TracedValue* state) const;

    // Common conditions to run for, exposed publicly to allow external users to
    // use their own combinations.
    // -------------------------------------------------------------------------

    // Keep running until the given number of tasks have run.
    // You generally shouldn't use this check as it will cause your tests to fail
    // when code is changed adding a new task. It is useful as a "timeout" type
    // solution.
    base::Callback<bool(void)> TaskRunCountBelow(size_t max_tasks);

    // Keep running until a task which didn't exist initially would run.
    base::Callback<bool(void)> TaskExistedInitially();

    // Stop running tasks when NextTaskTime() >= stop_at
    base::Callback<bool(void)> NowBefore(base::TimeTicks stop_at);

    // Advance Now() to the next task to run.
    base::Callback<bool(void)> AdvanceNow();

protected:
    static bool TaskRunCountBelowCallback(size_t max_tasks, size_t* task_run);
    bool TaskExistedInitiallyCallback(
        const std::set<TestOrderablePendingTask>& existing_tasks);
    bool NowBeforeCallback(base::TimeTicks stop_at);
    bool AdvanceNowCallback();

    ~OrderedSimpleTaskRunner() override;

    base::ThreadChecker thread_checker_;

    bool advance_now_;
    // Not owned.
    base::SimpleTestTickClock* now_src_;

    size_t max_tasks_;

    bool inside_run_tasks_until_;
    std::set<TestOrderablePendingTask> pending_tasks_;

private:
    DISALLOW_COPY_AND_ASSIGN(OrderedSimpleTaskRunner);
};

} // namespace cc

#endif // CC_TEST_ORDERED_SIMPLE_TASK_RUNNER_H_
