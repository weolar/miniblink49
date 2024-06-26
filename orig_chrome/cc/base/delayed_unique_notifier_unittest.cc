// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <deque>

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/test/test_pending_task.h"
#include "base/test/test_simple_task_runner.h"
#include "cc/base/delayed_unique_notifier.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    class TestNotifier : public DelayedUniqueNotifier {
    public:
        TestNotifier(base::SequencedTaskRunner* task_runner,
            const base::Closure& closure,
            const base::TimeDelta& delay)
            : DelayedUniqueNotifier(task_runner, closure, delay)
        {
        }
        ~TestNotifier() override { }

        // Overridden from DelayedUniqueNotifier:
        base::TimeTicks Now() const override { return now_; }

        void SetNow(base::TimeTicks now) { now_ = now; }

    private:
        base::TimeTicks now_;
    };

    class DelayedUniqueNotifierTest : public testing::Test {
    public:
        DelayedUniqueNotifierTest()
            : notification_count_(0)
        {
        }

        void SetUp() override
        {
            notification_count_ = 0;
            task_runner_ = make_scoped_refptr(new base::TestSimpleTaskRunner);
        }

        void Notify() { ++notification_count_; }

        int NotificationCount() const { return notification_count_; }

        std::deque<base::TestPendingTask> TakePendingTasks()
        {
            std::deque<base::TestPendingTask> tasks = task_runner_->GetPendingTasks();
            task_runner_->ClearPendingTasks();
            return tasks;
        }

    protected:
        int notification_count_;
        scoped_refptr<base::TestSimpleTaskRunner> task_runner_;
    };

    TEST_F(DelayedUniqueNotifierTest, ZeroDelay)
    {
        base::TimeDelta delay = base::TimeDelta::FromInternalValue(0);
        TestNotifier notifier(
            task_runner_.get(),
            base::Bind(&DelayedUniqueNotifierTest::Notify, base::Unretained(this)),
            delay);

        EXPECT_EQ(0, NotificationCount());

        // Basic schedule for |delay| from now.
        base::TimeTicks schedule_time = base::TimeTicks() + base::TimeDelta::FromInternalValue(10);

        notifier.SetNow(schedule_time);
        notifier.Schedule();

        std::deque<base::TestPendingTask> tasks = TakePendingTasks();
        ASSERT_EQ(1u, tasks.size());
        EXPECT_EQ(base::TimeTicks() + delay, tasks[0].GetTimeToRun());

        tasks[0].task.Run();
        EXPECT_EQ(1, NotificationCount());

        // 5 schedules should result in only one run.
        for (int i = 0; i < 5; ++i)
            notifier.Schedule();

        tasks = TakePendingTasks();
        ASSERT_EQ(1u, tasks.size());
        EXPECT_EQ(base::TimeTicks() + delay, tasks[0].GetTimeToRun());

        tasks[0].task.Run();
        EXPECT_EQ(2, NotificationCount());
    }

    TEST_F(DelayedUniqueNotifierTest, SmallDelay)
    {
        base::TimeDelta delay = base::TimeDelta::FromInternalValue(20);
        TestNotifier notifier(
            task_runner_.get(),
            base::Bind(&DelayedUniqueNotifierTest::Notify, base::Unretained(this)),
            delay);

        EXPECT_EQ(0, NotificationCount());

        // Basic schedule for |delay| from now (now: 30, run time: 50).
        base::TimeTicks schedule_time = base::TimeTicks() + base::TimeDelta::FromInternalValue(30);

        notifier.SetNow(schedule_time);
        notifier.Schedule();

        std::deque<base::TestPendingTask> tasks = TakePendingTasks();

        ASSERT_EQ(1u, tasks.size());
        EXPECT_EQ(base::TimeTicks() + delay, tasks[0].GetTimeToRun());

        // It's not yet time to run, so we expect no notifications.
        tasks[0].task.Run();
        EXPECT_EQ(0, NotificationCount());

        tasks = TakePendingTasks();

        ASSERT_EQ(1u, tasks.size());
        // Now the time should be delay minus whatever the value of now happens to be
        // (now: 30, run time: 50).
        base::TimeTicks scheduled_run_time = notifier.Now() + delay;
        base::TimeTicks scheduled_delay = base::TimeTicks() + (scheduled_run_time - notifier.Now());
        EXPECT_EQ(scheduled_delay, tasks[0].GetTimeToRun());

        // Move closer to the run time (time: 49, run time: 50).
        notifier.SetNow(notifier.Now() + base::TimeDelta::FromInternalValue(19));

        // It's not yet time to run, so we expect no notifications.
        tasks[0].task.Run();
        EXPECT_EQ(0, NotificationCount());

        tasks = TakePendingTasks();
        ASSERT_EQ(1u, tasks.size());

        // Now the time should be delay minus whatever the value of now happens to be.
        scheduled_delay = base::TimeTicks() + (scheduled_run_time - notifier.Now());
        EXPECT_EQ(scheduled_delay, tasks[0].GetTimeToRun());

        // Move to exactly the run time (time: 50, run time: 50).
        notifier.SetNow(notifier.Now() + base::TimeDelta::FromInternalValue(1));

        // It's time to run!
        tasks[0].task.Run();
        EXPECT_EQ(1, NotificationCount());

        tasks = TakePendingTasks();
        EXPECT_EQ(0u, tasks.size());
    }

    TEST_F(DelayedUniqueNotifierTest, RescheduleDelay)
    {
        base::TimeDelta delay = base::TimeDelta::FromInternalValue(20);
        TestNotifier notifier(
            task_runner_.get(),
            base::Bind(&DelayedUniqueNotifierTest::Notify, base::Unretained(this)),
            delay);

        base::TimeTicks schedule_time;
        // Move time 19 units forward and reschedule, expecting that we still need to
        // run in |delay| time and we don't get a notification.
        for (int i = 0; i < 10; ++i) {
            EXPECT_EQ(0, NotificationCount());

            // Move time forward 19 units.
            schedule_time = notifier.Now() + base::TimeDelta::FromInternalValue(19);
            notifier.SetNow(schedule_time);
            notifier.Schedule();

            std::deque<base::TestPendingTask> tasks = TakePendingTasks();

            ASSERT_EQ(1u, tasks.size());
            EXPECT_EQ(base::TimeTicks() + delay, tasks[0].GetTimeToRun());

            // It's not yet time to run, so we expect no notifications.
            tasks[0].task.Run();
            EXPECT_EQ(0, NotificationCount());
        }

        // Move time forward 20 units, expecting a notification.
        schedule_time = notifier.Now() + base::TimeDelta::FromInternalValue(20);
        notifier.SetNow(schedule_time);

        std::deque<base::TestPendingTask> tasks = TakePendingTasks();

        ASSERT_EQ(1u, tasks.size());
        EXPECT_EQ(base::TimeTicks() + delay, tasks[0].GetTimeToRun());

        // Time to run!
        tasks[0].task.Run();
        EXPECT_EQ(1, NotificationCount());
    }

    TEST_F(DelayedUniqueNotifierTest, CancelAndHasPendingNotification)
    {
        base::TimeDelta delay = base::TimeDelta::FromInternalValue(20);
        TestNotifier notifier(
            task_runner_.get(),
            base::Bind(&DelayedUniqueNotifierTest::Notify, base::Unretained(this)),
            delay);

        EXPECT_EQ(0, NotificationCount());

        // Schedule for |delay| seconds from now.
        base::TimeTicks schedule_time = notifier.Now() + base::TimeDelta::FromInternalValue(10);
        notifier.SetNow(schedule_time);
        notifier.Schedule();
        EXPECT_TRUE(notifier.HasPendingNotification());

        // Cancel the run.
        notifier.Cancel();
        EXPECT_FALSE(notifier.HasPendingNotification());

        std::deque<base::TestPendingTask> tasks = TakePendingTasks();

        ASSERT_EQ(1u, tasks.size());
        EXPECT_EQ(base::TimeTicks() + delay, tasks[0].GetTimeToRun());

        // Time to run, but a canceled task!
        tasks[0].task.Run();
        EXPECT_EQ(0, NotificationCount());
        EXPECT_FALSE(notifier.HasPendingNotification());

        tasks = TakePendingTasks();
        EXPECT_EQ(0u, tasks.size());

        notifier.Schedule();
        EXPECT_TRUE(notifier.HasPendingNotification());
        tasks = TakePendingTasks();

        ASSERT_EQ(1u, tasks.size());
        EXPECT_EQ(base::TimeTicks() + delay, tasks[0].GetTimeToRun());

        // Advance the time.
        notifier.SetNow(notifier.Now() + delay);

        // This should run since it wasn't canceled.
        tasks[0].task.Run();
        EXPECT_EQ(1, NotificationCount());
        EXPECT_FALSE(notifier.HasPendingNotification());

        for (int i = 0; i < 10; ++i) {
            notifier.Schedule();
            EXPECT_TRUE(notifier.HasPendingNotification());
            notifier.Cancel();
            EXPECT_FALSE(notifier.HasPendingNotification());
        }

        tasks = TakePendingTasks();

        ASSERT_EQ(1u, tasks.size());
        EXPECT_EQ(base::TimeTicks() + delay, tasks[0].GetTimeToRun());

        // Time to run, but a canceled task!
        notifier.SetNow(notifier.Now() + delay);
        tasks[0].task.Run();
        EXPECT_EQ(1, NotificationCount());

        tasks = TakePendingTasks();
        EXPECT_EQ(0u, tasks.size());
        EXPECT_FALSE(notifier.HasPendingNotification());
    }

    TEST_F(DelayedUniqueNotifierTest, ShutdownWithScheduledTask)
    {
        base::TimeDelta delay = base::TimeDelta::FromInternalValue(20);
        TestNotifier notifier(
            task_runner_.get(),
            base::Bind(&DelayedUniqueNotifierTest::Notify, base::Unretained(this)),
            delay);

        EXPECT_EQ(0, NotificationCount());

        // Schedule for |delay| seconds from now.
        base::TimeTicks schedule_time = notifier.Now() + base::TimeDelta::FromInternalValue(10);
        notifier.SetNow(schedule_time);
        notifier.Schedule();
        EXPECT_TRUE(notifier.HasPendingNotification());

        // Shutdown the notifier.
        notifier.Shutdown();

        // The task is still there, but...
        std::deque<base::TestPendingTask> tasks = TakePendingTasks();
        ASSERT_EQ(1u, tasks.size());

        // Running the task after shutdown does nothing since it's cancelled.
        tasks[0].task.Run();
        EXPECT_EQ(0, NotificationCount());

        tasks = TakePendingTasks();
        EXPECT_EQ(0u, tasks.size());

        // We are no longer able to schedule tasks.
        notifier.Schedule();
        tasks = TakePendingTasks();
        ASSERT_EQ(0u, tasks.size());

        // Verify after the scheduled time happens there is still no task.
        notifier.SetNow(notifier.Now() + delay);
        tasks = TakePendingTasks();
        ASSERT_EQ(0u, tasks.size());
    }

    TEST_F(DelayedUniqueNotifierTest, ShutdownPreventsSchedule)
    {
        base::TimeDelta delay = base::TimeDelta::FromInternalValue(20);
        TestNotifier notifier(
            task_runner_.get(),
            base::Bind(&DelayedUniqueNotifierTest::Notify, base::Unretained(this)),
            delay);

        EXPECT_EQ(0, NotificationCount());

        // Schedule for |delay| seconds from now.
        base::TimeTicks schedule_time = notifier.Now() + base::TimeDelta::FromInternalValue(10);
        notifier.SetNow(schedule_time);

        // Shutdown the notifier.
        notifier.Shutdown();

        // Scheduling a task no longer does anything.
        notifier.Schedule();
        std::deque<base::TestPendingTask> tasks = TakePendingTasks();
        ASSERT_EQ(0u, tasks.size());

        // Verify after the scheduled time happens there is still no task.
        notifier.SetNow(notifier.Now() + delay);
        tasks = TakePendingTasks();
        ASSERT_EQ(0u, tasks.size());
    }

} // namespace
} // namespace cc
