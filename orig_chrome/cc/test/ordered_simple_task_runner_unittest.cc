// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include "base/cancelable_callback.h"
#include "base/format_macros.h"
#include "base/memory/scoped_ptr.h"
#include "base/strings/stringprintf.h"
#include "base/test/test_pending_task.h"
#include "cc/test/ordered_simple_task_runner.h"
#include "testing/gtest/include/gtest/gtest.h"

// We pass EXPECT_TRUE / EXPECT_FALSE macros rather than a boolean as on some
// compilers EXPECT_EQ(false, XXXX) fails to compile as  gtest tries to convert
// the false value to null causing a -Werror=conversion-null error.
#define RUN_AND_CHECK_RESULT(                             \
    tasks_remain_expect_macro, run_func, expected_result) \
    tasks_remain_expect_macro(task_runner_->run_func);    \
    EXPECT_EQ(expected_result, executed_tasks_);          \
    executed_tasks_ = "";

namespace {

bool ReturnTrue()
{
    return true;
}

bool ReturnFalse()
{
    return false;
}
};

namespace cc {

TEST(TestOrderablePendingTask, Ordering)
{
    TestOrderablePendingTask a;
    TestOrderablePendingTask b;
    TestOrderablePendingTask c;

    EXPECT_EQ(a, a);
    EXPECT_EQ(b, b);
    EXPECT_EQ(c, c);
    EXPECT_LT(a, b);
    EXPECT_LT(b, c);
    EXPECT_LT(a, c);

    TestOrderablePendingTask a2 = a;
    EXPECT_EQ(a, a2);
    EXPECT_LT(a2, b);
    EXPECT_LT(b, c);
    EXPECT_LT(a2, c);
}

class OrderedSimpleTaskRunnerTest : public testing::Test {
public:
    OrderedSimpleTaskRunnerTest()
        : now_src_(new base::SimpleTestTickClock())
        , task_runner_(new OrderedSimpleTaskRunner(now_src_.get(), true))
    {
    }
    ~OrderedSimpleTaskRunnerTest() override { }

protected:
    std::string executed_tasks_;
    scoped_ptr<base::SimpleTestTickClock> now_src_;
    scoped_refptr<OrderedSimpleTaskRunner> task_runner_;

    void PostTask(int task_num, base::TimeDelta delay)
    {
        base::Closure test_task = base::Bind(&OrderedSimpleTaskRunnerTest::Task,
            base::Unretained(this),
            task_num);
        task_runner_->PostDelayedTask(FROM_HERE, test_task, delay);
    }

    void PostTaskWhichPostsInstantTask(int task_num, base::TimeDelta delay)
    {
        base::Closure test_task = base::Bind(&OrderedSimpleTaskRunnerTest::TaskWhichPostsInstantTask,
            base::Unretained(this),
            task_num);
        task_runner_->PostDelayedTask(FROM_HERE, test_task, delay);
    }

    void PostTaskWhichPostsDelayedTask(int task_num,
        base::TimeDelta delay1,
        base::TimeDelta delay2)
    {
        base::Closure test_task = base::Bind(&OrderedSimpleTaskRunnerTest::TaskWhichPostsDelayedTask,
            base::Unretained(this),
            task_num,
            delay2);
        task_runner_->PostDelayedTask(FROM_HERE, test_task, delay1);
    }

    void PostTaskWhichCallsRun(int task_num, base::TimeDelta delay)
    {
        base::Closure test_task = base::Bind(&OrderedSimpleTaskRunnerTest::TaskWhichCallsRun,
            base::Unretained(this),
            task_num);
        task_runner_->PostDelayedTask(FROM_HERE, test_task, delay);
    }

    void PostTaskWhichPostsTaskAgain(int task_num, base::TimeDelta delay)
    {
        base::Closure test_task = base::Bind(&OrderedSimpleTaskRunnerTest::TaskWhichPostsAgain,
            base::Unretained(this),
            task_num,
            delay);
        task_runner_->PostDelayedTask(FROM_HERE, test_task, delay);
    }

private:
    void Task(int task_num)
    {
        if (!executed_tasks_.empty())
            executed_tasks_ += " ";
        executed_tasks_ += base::StringPrintf(
            "%d(%" PRId64 "ms)", task_num,
            (now_src_->NowTicks() - base::TimeTicks()).InMilliseconds());
    }

    void TaskWhichPostsInstantTask(int task_num)
    {
        Task(task_num);
        PostTask(-task_num, base::TimeDelta());
    }

    void TaskWhichPostsDelayedTask(int task_num, base::TimeDelta delay)
    {
        Task(task_num);
        PostTask(-task_num, delay);
    }

    void TaskWhichCallsRun(int task_num)
    {
        Task(task_num);
        task_runner_->RunPendingTasks();
    }

    void TaskWhichPostsAgain(int task_num, base::TimeDelta delay)
    {
        Task(task_num);
        PostTaskWhichPostsTaskAgain(task_num, delay);
    }

    DISALLOW_COPY_AND_ASSIGN(OrderedSimpleTaskRunnerTest);
};

TEST_F(OrderedSimpleTaskRunnerTest, SimpleOrderingTest)
{
    PostTask(1, base::TimeDelta());
    PostTask(2, base::TimeDelta());
    PostTask(3, base::TimeDelta());

    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunPendingTasks(), "1(0ms) 2(0ms) 3(0ms)");
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunPendingTasks(), "");

    PostTask(4, base::TimeDelta());
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunPendingTasks(), "4(0ms)");
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunPendingTasks(), "");
}

TEST_F(OrderedSimpleTaskRunnerTest, SimpleOrderingTestPostingTasks)
{
    PostTaskWhichPostsInstantTask(1, base::TimeDelta());
    PostTaskWhichPostsInstantTask(2, base::TimeDelta());
    PostTaskWhichPostsInstantTask(3, base::TimeDelta());

    RUN_AND_CHECK_RESULT(EXPECT_TRUE, RunPendingTasks(), "1(0ms) 2(0ms) 3(0ms)");
    RUN_AND_CHECK_RESULT(
        EXPECT_FALSE, RunPendingTasks(), "-1(0ms) -2(0ms) -3(0ms)");
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunPendingTasks(), "");
}

TEST_F(OrderedSimpleTaskRunnerTest, SimpleOrderingTestPostingDelayedTasks)
{
    PostTaskWhichPostsDelayedTask(
        1, base::TimeDelta(), base::TimeDelta::FromMilliseconds(1));
    PostTaskWhichPostsDelayedTask(
        2, base::TimeDelta(), base::TimeDelta::FromMilliseconds(1));
    PostTaskWhichPostsDelayedTask(
        3, base::TimeDelta(), base::TimeDelta::FromMilliseconds(1));

    RUN_AND_CHECK_RESULT(EXPECT_TRUE, RunPendingTasks(), "1(0ms) 2(0ms) 3(0ms)");
    RUN_AND_CHECK_RESULT(
        EXPECT_FALSE, RunPendingTasks(), "-1(1ms) -2(1ms) -3(1ms)");
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunPendingTasks(), "");
}

TEST_F(OrderedSimpleTaskRunnerTest,
    SimpleOrderingTestPostingReordingDelayedTasks)
{
    PostTaskWhichPostsDelayedTask(1,
        base::TimeDelta::FromMilliseconds(1),
        base::TimeDelta::FromMilliseconds(20));
    PostTaskWhichPostsDelayedTask(2,
        base::TimeDelta::FromMilliseconds(2),
        base::TimeDelta::FromMilliseconds(5));
    PostTaskWhichPostsDelayedTask(3,
        base::TimeDelta::FromMilliseconds(3),
        base::TimeDelta::FromMilliseconds(5));

    RUN_AND_CHECK_RESULT(EXPECT_TRUE, RunPendingTasks(), "1(1ms) 2(2ms) 3(3ms)");
    RUN_AND_CHECK_RESULT(
        EXPECT_FALSE, RunPendingTasks(), "-2(7ms) -3(8ms) -1(21ms)");
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunPendingTasks(), "");
}

TEST_F(OrderedSimpleTaskRunnerTest,
    SimpleOrderingTestPostingReordingDelayedTasksOverlap)
{
    PostTaskWhichPostsDelayedTask(1,
        base::TimeDelta::FromMilliseconds(1),
        base::TimeDelta::FromMilliseconds(5));
    PostTaskWhichPostsDelayedTask(2,
        base::TimeDelta::FromMilliseconds(5),
        base::TimeDelta::FromMilliseconds(10));
    PostTaskWhichPostsDelayedTask(3,
        base::TimeDelta::FromMilliseconds(10),
        base::TimeDelta::FromMilliseconds(1));

    RUN_AND_CHECK_RESULT(EXPECT_TRUE, RunPendingTasks(), "1(1ms) 2(5ms)");
    RUN_AND_CHECK_RESULT(EXPECT_TRUE, RunPendingTasks(), "-1(6ms) 3(10ms)");
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunPendingTasks(), "-3(11ms) -2(15ms)");
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunPendingTasks(), "");
}

TEST_F(OrderedSimpleTaskRunnerTest, SimpleOrderingTestPostingAndRentrantTasks)
{
    PostTaskWhichPostsInstantTask(1, base::TimeDelta());
    PostTaskWhichCallsRun(2, base::TimeDelta());
    PostTaskWhichPostsInstantTask(3, base::TimeDelta());

    RUN_AND_CHECK_RESULT(EXPECT_TRUE, RunPendingTasks(), "1(0ms) 2(0ms) 3(0ms)");
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunPendingTasks(), "-1(0ms) -3(0ms)");
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunPendingTasks(), "");
}

TEST_F(OrderedSimpleTaskRunnerTest,
    SimpleOrderingTestPostingDelayedAndRentrantTasks)
{
    PostTaskWhichPostsDelayedTask(
        1, base::TimeDelta(), base::TimeDelta::FromMilliseconds(1));
    PostTaskWhichCallsRun(2, base::TimeDelta());
    PostTaskWhichPostsDelayedTask(
        3, base::TimeDelta(), base::TimeDelta::FromMilliseconds(1));

    RUN_AND_CHECK_RESULT(EXPECT_TRUE, RunPendingTasks(), "1(0ms) 2(0ms) 3(0ms)");
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunPendingTasks(), "-1(1ms) -3(1ms)");
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunPendingTasks(), "");
}

TEST_F(OrderedSimpleTaskRunnerTest, OrderingTestWithDelayedTasks)
{
    PostTask(1, base::TimeDelta());
    PostTask(2, base::TimeDelta::FromMilliseconds(15));
    PostTask(3, base::TimeDelta());
    PostTask(4, base::TimeDelta::FromMilliseconds(8));

    RUN_AND_CHECK_RESULT(
        EXPECT_FALSE, RunPendingTasks(), "1(0ms) 3(0ms) 4(8ms) 2(15ms)");
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunPendingTasks(), "");
}

TEST_F(OrderedSimpleTaskRunnerTest, OrderingTestWithDelayedPostingTasks)
{
    PostTaskWhichPostsInstantTask(1, base::TimeDelta());
    PostTaskWhichPostsInstantTask(2, base::TimeDelta::FromMilliseconds(15));
    PostTaskWhichPostsInstantTask(3, base::TimeDelta());
    PostTaskWhichPostsInstantTask(4, base::TimeDelta::FromMilliseconds(8));

    RUN_AND_CHECK_RESULT(EXPECT_TRUE, RunPendingTasks(), "1(0ms) 3(0ms)");
    RUN_AND_CHECK_RESULT(
        EXPECT_TRUE, RunPendingTasks(), "-1(0ms) -3(0ms) 4(8ms)");
    RUN_AND_CHECK_RESULT(EXPECT_TRUE, RunPendingTasks(), "-4(8ms) 2(15ms)");
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunPendingTasks(), "-2(15ms)");
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunPendingTasks(), "");
}

TEST_F(OrderedSimpleTaskRunnerTest, OrderingTestWithDelayedTasksManualNow)
{
    task_runner_->SetAutoAdvanceNowToPendingTasks(false);
    PostTask(1, base::TimeDelta());
    PostTask(2, base::TimeDelta::FromMilliseconds(15));
    PostTask(3, base::TimeDelta());
    PostTask(4, base::TimeDelta::FromMilliseconds(8));

    RUN_AND_CHECK_RESULT(EXPECT_TRUE, RunPendingTasks(), "1(0ms) 3(0ms)");
    RUN_AND_CHECK_RESULT(EXPECT_TRUE, RunPendingTasks(), "");
    EXPECT_EQ(task_runner_->DelayToNextTaskTime(),
        base::TimeDelta::FromMilliseconds(8));
    now_src_->Advance(base::TimeDelta::FromMicroseconds(5000));
    EXPECT_EQ(task_runner_->DelayToNextTaskTime(),
        base::TimeDelta::FromMilliseconds(3));
    now_src_->Advance(base::TimeDelta::FromMicroseconds(20000));
    EXPECT_EQ(task_runner_->DelayToNextTaskTime(), base::TimeDelta());
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunPendingTasks(), "4(25ms) 2(25ms)");
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunPendingTasks(), "");
}

TEST_F(OrderedSimpleTaskRunnerTest, RunUntilIdle)
{
    PostTaskWhichPostsInstantTask(1, base::TimeDelta());
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunUntilIdle(), "1(0ms) -1(0ms)");
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunUntilIdle(), "");
}

TEST_F(OrderedSimpleTaskRunnerTest, RunUntilTimeAutoNow)
{
    PostTaskWhichPostsInstantTask(1, base::TimeDelta());
    PostTaskWhichPostsInstantTask(2, base::TimeDelta::FromMilliseconds(2));
    PostTaskWhichPostsInstantTask(3, base::TimeDelta::FromMilliseconds(3));

    task_runner_->SetAutoAdvanceNowToPendingTasks(true);

    base::TimeTicks run_at = base::TimeTicks();

    run_at += base::TimeDelta::FromMilliseconds(2);
    RUN_AND_CHECK_RESULT(
        EXPECT_TRUE, RunUntilTime(run_at), "1(0ms) -1(0ms) 2(2ms) -2(2ms)");
    EXPECT_EQ(run_at, now_src_->NowTicks());

    run_at += base::TimeDelta::FromMilliseconds(1);
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunUntilTime(run_at), "3(3ms) -3(3ms)");
    EXPECT_EQ(run_at, now_src_->NowTicks());

    run_at += base::TimeDelta::FromMilliseconds(1);
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunUntilTime(run_at), "");
    EXPECT_EQ(run_at, now_src_->NowTicks());
}

TEST_F(OrderedSimpleTaskRunnerTest, RunUntilTimeManualNow)
{
    PostTaskWhichPostsInstantTask(1, base::TimeDelta());
    PostTaskWhichPostsInstantTask(2, base::TimeDelta::FromMilliseconds(2));
    PostTaskWhichPostsInstantTask(3, base::TimeDelta::FromMilliseconds(3));

    task_runner_->SetAutoAdvanceNowToPendingTasks(false);

    base::TimeTicks run_at = base::TimeTicks();

    run_at += base::TimeDelta::FromMilliseconds(2);
    RUN_AND_CHECK_RESULT(
        EXPECT_TRUE, RunUntilTime(run_at), "1(2ms) 2(2ms) -1(2ms) -2(2ms)");
    EXPECT_EQ(run_at, now_src_->NowTicks());

    run_at += base::TimeDelta::FromMilliseconds(1);
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunUntilTime(run_at), "3(3ms) -3(3ms)");
    EXPECT_EQ(run_at, now_src_->NowTicks());

    run_at += base::TimeDelta::FromMilliseconds(1);
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunUntilTime(run_at), "");
    EXPECT_EQ(run_at, now_src_->NowTicks());
}

TEST_F(OrderedSimpleTaskRunnerTest, RunForPeriod)
{
    PostTaskWhichPostsInstantTask(1, base::TimeDelta());
    PostTaskWhichPostsInstantTask(2, base::TimeDelta::FromMilliseconds(2));
    PostTaskWhichPostsInstantTask(3, base::TimeDelta::FromMilliseconds(3));

    RUN_AND_CHECK_RESULT(EXPECT_TRUE,
        RunForPeriod(base::TimeDelta::FromMilliseconds(2)),
        "1(0ms) -1(0ms) 2(2ms) -2(2ms)");
    EXPECT_EQ(base::TimeTicks() + base::TimeDelta::FromMilliseconds(2),
        now_src_->NowTicks());

    RUN_AND_CHECK_RESULT(EXPECT_FALSE,
        RunForPeriod(base::TimeDelta::FromMilliseconds(1)),
        "3(3ms) -3(3ms)");
    EXPECT_EQ(base::TimeTicks() + base::TimeDelta::FromMilliseconds(3),
        now_src_->NowTicks());

    RUN_AND_CHECK_RESULT(
        EXPECT_FALSE, RunForPeriod(base::TimeDelta::FromMilliseconds(1)), "");
    EXPECT_EQ(base::TimeTicks() + base::TimeDelta::FromMilliseconds(4),
        now_src_->NowTicks());
}

TEST_F(OrderedSimpleTaskRunnerTest, RunTasksWhileWithCallback)
{
    base::Callback<bool(void)> return_true = base::Bind(&ReturnTrue);
    base::Callback<bool(void)> return_false = base::Bind(&ReturnFalse);

    PostTask(1, base::TimeDelta());

    RUN_AND_CHECK_RESULT(EXPECT_TRUE, RunTasksWhile(return_false), "");
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunTasksWhile(return_true), "1(0ms)");
}

TEST_F(OrderedSimpleTaskRunnerTest, EmptyTaskList)
{
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunPendingTasks(), "");
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunUntilIdle(), "");

    ASSERT_EQ(base::TimeTicks(), now_src_->NowTicks());

    RUN_AND_CHECK_RESULT(
        EXPECT_FALSE, RunUntilTime(base::TimeTicks::FromInternalValue(100)), "");
    EXPECT_EQ(base::TimeTicks::FromInternalValue(100), now_src_->NowTicks());

    RUN_AND_CHECK_RESULT(
        EXPECT_FALSE, RunForPeriod(base::TimeDelta::FromInternalValue(100)), "");
    EXPECT_EQ(base::TimeTicks::FromInternalValue(200), now_src_->NowTicks());

    base::Callback<bool(void)> return_true = base::Bind(&ReturnTrue);
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunTasksWhile(return_true), "");
}

TEST_F(OrderedSimpleTaskRunnerTest, RunPendingTasksTimeout)
{
    PostTask(1, base::TimeDelta());
    PostTask(2, base::TimeDelta());
    PostTask(3, base::TimeDelta());
    PostTask(4, base::TimeDelta());
    PostTask(5, base::TimeDelta());
    PostTask(6, base::TimeDelta());

    task_runner_->SetRunTaskLimit(3);
    RUN_AND_CHECK_RESULT(EXPECT_TRUE, RunPendingTasks(), "1(0ms) 2(0ms) 3(0ms)");

    task_runner_->SetRunTaskLimit(2);
    RUN_AND_CHECK_RESULT(EXPECT_TRUE, RunPendingTasks(), "4(0ms) 5(0ms)");

    task_runner_->SetRunTaskLimit(0);
    RUN_AND_CHECK_RESULT(EXPECT_TRUE, RunPendingTasks(), "");
}

TEST_F(OrderedSimpleTaskRunnerTest, RunUntilIdleTimeout)
{
    PostTaskWhichPostsTaskAgain(1, base::TimeDelta::FromMilliseconds(3));

    task_runner_->SetRunTaskLimit(3);
    RUN_AND_CHECK_RESULT(EXPECT_TRUE, RunUntilIdle(), "1(3ms) 1(6ms) 1(9ms)");

    task_runner_->SetRunTaskLimit(2);
    RUN_AND_CHECK_RESULT(EXPECT_TRUE, RunUntilIdle(), "1(12ms) 1(15ms)");

    task_runner_->SetRunTaskLimit(0);
    RUN_AND_CHECK_RESULT(EXPECT_TRUE, RunUntilIdle(), "");
}

TEST_F(OrderedSimpleTaskRunnerTest, RunUntilTimeout)
{
    base::TimeTicks run_to = base::TimeTicks() + base::TimeDelta::FromSeconds(1);

    PostTask(1, base::TimeDelta::FromMilliseconds(1));
    PostTask(2, base::TimeDelta::FromMilliseconds(2));
    PostTask(3, base::TimeDelta::FromMilliseconds(3));
    PostTask(4, base::TimeDelta::FromMilliseconds(4));
    PostTask(5, base::TimeDelta::FromMilliseconds(5));

    EXPECT_EQ(base::TimeTicks(), now_src_->NowTicks());
    task_runner_->SetRunTaskLimit(3);
    RUN_AND_CHECK_RESULT(
        EXPECT_TRUE, RunUntilTime(run_to), "1(1ms) 2(2ms) 3(3ms)");
    EXPECT_EQ(base::TimeTicks() + base::TimeDelta::FromMilliseconds(3),
        now_src_->NowTicks());

    task_runner_->SetRunTaskLimit(0);
    RUN_AND_CHECK_RESULT(EXPECT_TRUE, RunUntilTime(run_to), "");

    task_runner_->SetRunTaskLimit(100);
    RUN_AND_CHECK_RESULT(EXPECT_FALSE, RunUntilTime(run_to), "4(4ms) 5(5ms)");
    EXPECT_EQ(run_to, now_src_->NowTicks());
}

} // namespace cc
