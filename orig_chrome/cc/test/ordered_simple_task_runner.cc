// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/ordered_simple_task_runner.h"

#include <limits>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "base/auto_reset.h"
#include "base/numerics/safe_conversions.h"
#include "base/strings/string_number_conversions.h"
#include "base/trace_event/trace_event.h"
#include "base/trace_event/trace_event_argument.h"

#define TRACE_TASK(function, task) \
    TRACE_EVENT_INSTANT1(          \
        "cc", function, TRACE_EVENT_SCOPE_THREAD, "task", task.AsValue());

#define TRACE_TASK_RUN(function, tag, task)

namespace cc {

// TestOrderablePendingTask implementation
TestOrderablePendingTask::TestOrderablePendingTask()
    : base::TestPendingTask()
    , task_id_(TestOrderablePendingTask::task_id_counter++)
{
}

TestOrderablePendingTask::TestOrderablePendingTask(
    const tracked_objects::Location& location,
    const base::Closure& task,
    base::TimeTicks post_time,
    base::TimeDelta delay,
    TestNestability nestability)
    : base::TestPendingTask(location, task, post_time, delay, nestability)
    , task_id_(TestOrderablePendingTask::task_id_counter++)
{
}

size_t TestOrderablePendingTask::task_id_counter = 0;

TestOrderablePendingTask::~TestOrderablePendingTask()
{
}

bool TestOrderablePendingTask::operator==(
    const TestOrderablePendingTask& other) const
{
    return task_id_ == other.task_id_;
}

bool TestOrderablePendingTask::operator<(
    const TestOrderablePendingTask& other) const
{
    if (*this == other)
        return false;

    if (GetTimeToRun() == other.GetTimeToRun()) {
        return task_id_ < other.task_id_;
    }
    return ShouldRunBefore(other);
}

scoped_refptr<base::trace_event::ConvertableToTraceFormat>
TestOrderablePendingTask::AsValue() const
{
    scoped_refptr<base::trace_event::TracedValue> state = new base::trace_event::TracedValue();
    AsValueInto(state.get());
    return state;
}

void TestOrderablePendingTask::AsValueInto(
    base::trace_event::TracedValue* state) const
{
    state->SetInteger("id", base::saturated_cast<int>(task_id_));
    state->SetInteger("run_at", GetTimeToRun().ToInternalValue());
    state->SetString("posted_from", location.ToString());
}

OrderedSimpleTaskRunner::OrderedSimpleTaskRunner(
    base::SimpleTestTickClock* now_src,
    bool advance_now)
    : advance_now_(advance_now)
    , now_src_(now_src)
    , max_tasks_(kAbsoluteMaxTasks)
    , inside_run_tasks_until_(false)
{
}

OrderedSimpleTaskRunner::~OrderedSimpleTaskRunner() { }

// static
base::TimeTicks OrderedSimpleTaskRunner::AbsoluteMaxNow()
{
    return base::TimeTicks::FromInternalValue(
        std::numeric_limits<int64_t>::max());
}

// base::TestSimpleTaskRunner implementation
bool OrderedSimpleTaskRunner::PostDelayedTask(
    const tracked_objects::Location& from_here,
    const base::Closure& task,
    base::TimeDelta delay)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    TestOrderablePendingTask pt(from_here, task, now_src_->NowTicks(), delay,
        base::TestPendingTask::NESTABLE);

    TRACE_TASK("OrderedSimpleTaskRunner::PostDelayedTask", pt);
    pending_tasks_.insert(pt);
    return true;
}

bool OrderedSimpleTaskRunner::PostNonNestableDelayedTask(
    const tracked_objects::Location& from_here,
    const base::Closure& task,
    base::TimeDelta delay)
{
    DCHECK(thread_checker_.CalledOnValidThread());
    TestOrderablePendingTask pt(from_here, task, now_src_->NowTicks(), delay,
        base::TestPendingTask::NON_NESTABLE);

    TRACE_TASK("OrderedSimpleTaskRunner::PostNonNestableDelayedTask", pt);
    pending_tasks_.insert(pt);
    return true;
}

bool OrderedSimpleTaskRunner::RunsTasksOnCurrentThread() const
{
    DCHECK(thread_checker_.CalledOnValidThread());
    return true;
}

size_t OrderedSimpleTaskRunner::NumPendingTasks() const
{
    return pending_tasks_.size();
}

bool OrderedSimpleTaskRunner::HasPendingTasks() const
{
    return pending_tasks_.size() > 0;
}

base::TimeTicks OrderedSimpleTaskRunner::NextTaskTime()
{
    if (pending_tasks_.size() <= 0) {
        return AbsoluteMaxNow();
    }

    return pending_tasks_.begin()->GetTimeToRun();
}

base::TimeDelta OrderedSimpleTaskRunner::DelayToNextTaskTime()
{
    DCHECK(thread_checker_.CalledOnValidThread());

    if (pending_tasks_.size() <= 0) {
        return AbsoluteMaxNow() - base::TimeTicks();
    }

    base::TimeDelta delay = NextTaskTime() - now_src_->NowTicks();
    if (delay > base::TimeDelta())
        return delay;
    return base::TimeDelta();
}

const size_t OrderedSimpleTaskRunner::kAbsoluteMaxTasks = std::numeric_limits<size_t>::max();

bool OrderedSimpleTaskRunner::RunTasksWhile(
    base::Callback<bool(void)> condition)
{
    std::vector<base::Callback<bool(void)>> conditions(1);
    conditions[0] = condition;
    return RunTasksWhile(conditions);
}

bool OrderedSimpleTaskRunner::RunTasksWhile(
    const std::vector<base::Callback<bool(void)>>& conditions)
{
    TRACE_EVENT2("cc",
        "OrderedSimpleTaskRunner::RunPendingTasks",
        "this",
        AsValue(),
        "nested",
        inside_run_tasks_until_);
    DCHECK(thread_checker_.CalledOnValidThread());

    if (inside_run_tasks_until_)
        return true;

    base::AutoReset<bool> reset_inside_run_tasks_until_(&inside_run_tasks_until_,
        true);

    // Make a copy so we can append some extra run checks.
    std::vector<base::Callback<bool(void)>> modifiable_conditions(conditions);

    // Provide a timeout base on number of tasks run so this doesn't loop
    // forever.
    modifiable_conditions.push_back(TaskRunCountBelow(max_tasks_));

    // If to advance now or not
    if (!advance_now_) {
        modifiable_conditions.push_back(NowBefore(now_src_->NowTicks()));
    } else {
        modifiable_conditions.push_back(AdvanceNow());
    }

    while (pending_tasks_.size() > 0) {
        // Check if we should continue to run pending tasks.
        bool condition_success = true;
        for (std::vector<base::Callback<bool(void)>>::iterator it = modifiable_conditions.begin();
             it != modifiable_conditions.end();
             it++) {
            condition_success = it->Run();
            if (!condition_success)
                break;
        }

        // Conditions could modify the pending task length, so we need to recheck
        // that there are tasks to run.
        if (!condition_success || !HasPendingTasks()) {
            break;
        }

        std::set<TestOrderablePendingTask>::iterator task_to_run = pending_tasks_.begin();
        {
            TRACE_EVENT1("cc",
                "OrderedSimpleTaskRunner::RunPendingTasks running",
                "task",
                task_to_run->AsValue());
            task_to_run->task.Run();
        }

        pending_tasks_.erase(task_to_run);
    }

    return HasPendingTasks();
}

bool OrderedSimpleTaskRunner::RunPendingTasks()
{
    return RunTasksWhile(TaskExistedInitially());
}

bool OrderedSimpleTaskRunner::RunUntilIdle()
{
    return RunTasksWhile(std::vector<base::Callback<bool(void)>>());
}

bool OrderedSimpleTaskRunner::RunUntilTime(base::TimeTicks time)
{
    // If we are not auto advancing, force now forward to the time.
    if (!advance_now_ && now_src_->NowTicks() < time)
        now_src_->Advance(time - now_src_->NowTicks());

    // Run tasks
    bool result = RunTasksWhile(NowBefore(time));

    // If the next task is after the stopping time and auto-advancing now, then
    // force time to be the stopping time.
    if (!result && advance_now_ && now_src_->NowTicks() < time) {
        now_src_->Advance(time - now_src_->NowTicks());
    }

    return result;
}

bool OrderedSimpleTaskRunner::RunForPeriod(base::TimeDelta period)
{
    return RunUntilTime(now_src_->NowTicks() + period);
}

// base::trace_event tracing functionality
scoped_refptr<base::trace_event::ConvertableToTraceFormat>
OrderedSimpleTaskRunner::AsValue() const
{
    scoped_refptr<base::trace_event::TracedValue> state = new base::trace_event::TracedValue();
    AsValueInto(state.get());
    return state;
}

void OrderedSimpleTaskRunner::AsValueInto(
    base::trace_event::TracedValue* state) const
{
    state->SetInteger("pending_tasks",
        base::saturated_cast<int>(pending_tasks_.size()));

    state->BeginArray("tasks");
    for (std::set<TestOrderablePendingTask>::const_iterator it = pending_tasks_.begin();
         it != pending_tasks_.end();
         ++it) {
        state->BeginDictionary();
        it->AsValueInto(state);
        state->EndDictionary();
    }
    state->EndArray();

    state->BeginDictionary("now_src");
    state->SetDouble("now_in_ms", (now_src_->NowTicks() - base::TimeTicks()).InMillisecondsF());
    state->EndDictionary();

    state->SetBoolean("advance_now", advance_now_);
    state->SetBoolean("inside_run_tasks_until", inside_run_tasks_until_);
    state->SetString("max_tasks", base::SizeTToString(max_tasks_));
}

base::Callback<bool(void)> OrderedSimpleTaskRunner::TaskRunCountBelow(
    size_t max_tasks)
{
    return base::Bind(&OrderedSimpleTaskRunner::TaskRunCountBelowCallback,
        max_tasks,
        base::Owned(new size_t(0)));
}

bool OrderedSimpleTaskRunner::TaskRunCountBelowCallback(size_t max_tasks,
    size_t* tasks_run)
{
    return (*tasks_run)++ < max_tasks;
}

base::Callback<bool(void)> OrderedSimpleTaskRunner::TaskExistedInitially()
{
    // base::Bind takes a copy of pending_tasks_
    return base::Bind(&OrderedSimpleTaskRunner::TaskExistedInitiallyCallback,
        base::Unretained(this),
        pending_tasks_);
}

bool OrderedSimpleTaskRunner::TaskExistedInitiallyCallback(
    const std::set<TestOrderablePendingTask>& existing_tasks)
{
    return existing_tasks.find(*pending_tasks_.begin()) != existing_tasks.end();
}

base::Callback<bool(void)> OrderedSimpleTaskRunner::NowBefore(
    base::TimeTicks stop_at)
{
    return base::Bind(&OrderedSimpleTaskRunner::NowBeforeCallback,
        base::Unretained(this),
        stop_at);
}
bool OrderedSimpleTaskRunner::NowBeforeCallback(base::TimeTicks stop_at)
{
    return NextTaskTime() <= stop_at;
}

base::Callback<bool(void)> OrderedSimpleTaskRunner::AdvanceNow()
{
    return base::Bind(&OrderedSimpleTaskRunner::AdvanceNowCallback,
        base::Unretained(this));
}

bool OrderedSimpleTaskRunner::AdvanceNowCallback()
{
    base::TimeTicks next_task_time = NextTaskTime();
    if (now_src_->NowTicks() < next_task_time) {
        now_src_->Advance(next_task_time - now_src_->NowTicks());
    }
    return true;
}

} // namespace cc
