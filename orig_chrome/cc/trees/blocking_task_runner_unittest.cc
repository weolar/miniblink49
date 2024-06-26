// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/trees/blocking_task_runner.h"

#include "base/bind.h"
#include "base/location.h"
#include "base/run_loop.h"
#include "base/single_thread_task_runner.h"
#include "base/thread_task_runner_handle.h"
#include "cc/test/ordered_simple_task_runner.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    void TestTask(bool* result)
    {
        *result = true;
    }

    TEST(BlockingTaskRunnerTest, NoCapture)
    {
        bool did_run = false;
        scoped_ptr<BlockingTaskRunner> runner(
            BlockingTaskRunner::Create(base::ThreadTaskRunnerHandle::Get()));
        runner->PostTask(FROM_HERE, base::Bind(&TestTask, &did_run));
        EXPECT_FALSE(did_run);
        base::RunLoop().RunUntilIdle();
        EXPECT_TRUE(did_run);
    }

    TEST(BlockingTaskRunnerTest, Capture)
    {
        bool did_run = false;
        scoped_ptr<BlockingTaskRunner> runner(
            BlockingTaskRunner::Create(base::ThreadTaskRunnerHandle::Get()));
        {
            BlockingTaskRunner::CapturePostTasks capture(runner.get());
            runner->PostTask(FROM_HERE, base::Bind(&TestTask, &did_run));
            EXPECT_FALSE(did_run);
        }
        EXPECT_TRUE(did_run);
    }

} // namespace
} // namespace cc
