/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 * Copyright (C) 2013 Google Inc. All Rights Reserved.
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
 *
 */

#include "config.h"
#include "core/dom/MainThreadTaskRunner.h"

#include "core/dom/ExecutionContextTask.h"
#include "core/testing/NullExecutionContext.h"
#include "platform/heap/Handle.h"
#include "platform/testing/UnitTestHelpers.h"
#include "wtf/Forward.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include <gtest/gtest.h>

namespace blink {

class MarkingBooleanTask final : public ExecutionContextTask {
public:
    static PassOwnPtr<MarkingBooleanTask> create(bool* toBeMarked)
    {
        return adoptPtr(new MarkingBooleanTask(toBeMarked));
    }


    ~MarkingBooleanTask() override { }

private:
    MarkingBooleanTask(bool* toBeMarked) : m_toBeMarked(toBeMarked) { }

    void performTask(ExecutionContext* context) override
    {
        *m_toBeMarked = true;
    }

    bool* m_toBeMarked;
};

TEST(MainThreadTaskRunnerTest, PostTask)
{
    RefPtrWillBeRawPtr<NullExecutionContext> context = adoptRefWillBeNoop(new NullExecutionContext());
    OwnPtr<MainThreadTaskRunner> runner = MainThreadTaskRunner::create(context.get());
    bool isMarked = false;

    runner->postTask(FROM_HERE, MarkingBooleanTask::create(&isMarked));
    EXPECT_FALSE(isMarked);
    blink::testing::runPendingTasks();
    EXPECT_TRUE(isMarked);
}

TEST(MainThreadTaskRunnerTest, SuspendTask)
{
    RefPtrWillBeRawPtr<NullExecutionContext> context = adoptRefWillBeNoop(new NullExecutionContext());
    OwnPtr<MainThreadTaskRunner> runner = MainThreadTaskRunner::create(context.get());
    bool isMarked = false;

    context->setTasksNeedSuspension(true);
    runner->postTask(FROM_HERE, MarkingBooleanTask::create(&isMarked));
    runner->suspend();
    blink::testing::runPendingTasks();
    EXPECT_FALSE(isMarked);

    context->setTasksNeedSuspension(false);
    runner->resume();
    blink::testing::runPendingTasks();
    EXPECT_TRUE(isMarked);
}

TEST(MainThreadTaskRunnerTest, RemoveRunner)
{
    RefPtrWillBeRawPtr<NullExecutionContext> context = adoptRefWillBeNoop(new NullExecutionContext());
    OwnPtr<MainThreadTaskRunner> runner = MainThreadTaskRunner::create(context.get());
    bool isMarked = false;

    context->setTasksNeedSuspension(true);
    runner->postTask(FROM_HERE, MarkingBooleanTask::create(&isMarked));
    runner.clear();
    blink::testing::runPendingTasks();
    EXPECT_FALSE(isMarked);
}

} // namespace blink
