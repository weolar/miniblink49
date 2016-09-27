// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/scheduler/CancellableTaskFactory.h"

#include <gtest/gtest.h>

namespace blink {

using CancellableTaskFactoryTest = testing::Test;

TEST_F(CancellableTaskFactoryTest, IsPending_TaskNotCreated)
{
    CancellableTaskFactory factory(nullptr);

    EXPECT_FALSE(factory.isPending());
}

TEST_F(CancellableTaskFactoryTest, IsPending_TaskCreated)
{
    CancellableTaskFactory factory(nullptr);
    OwnPtr<WebThread::Task> task = adoptPtr(factory.cancelAndCreate());

    EXPECT_TRUE(factory.isPending());
}

void EmptyFn()
{
}

TEST_F(CancellableTaskFactoryTest, IsPending_TaskCreatedAndRun)
{
    CancellableTaskFactory factory(WTF::bind(&EmptyFn));
    {
        OwnPtr<WebThread::Task> task = adoptPtr(factory.cancelAndCreate());
        task->run();
    }

    EXPECT_FALSE(factory.isPending());
}

TEST_F(CancellableTaskFactoryTest, IsPending_TaskCreatedAndDestroyed)
{
    CancellableTaskFactory factory(nullptr);
    delete factory.cancelAndCreate();

    EXPECT_FALSE(factory.isPending());
}

TEST_F(CancellableTaskFactoryTest, IsPending_TaskCreatedAndCancelled)
{
    CancellableTaskFactory factory(nullptr);
    OwnPtr<WebThread::Task> task = adoptPtr(factory.cancelAndCreate());
    factory.cancel();

    EXPECT_FALSE(factory.isPending());
}

class TestClass {
public:
    CancellableTaskFactory m_factory;

    TestClass()
        : m_factory(WTF::bind(&TestClass::TestFn, this))
    {
    }

    void TestFn()
    {
        EXPECT_FALSE(m_factory.isPending());
    }
};

TEST_F(CancellableTaskFactoryTest, IsPending_InCallback)
{
    TestClass testClass;
    OwnPtr<WebThread::Task> task = adoptPtr(testClass.m_factory.cancelAndCreate());
    task->run();
}

void AddOne(int* ptr)
{
    *ptr += 1;
}

TEST_F(CancellableTaskFactoryTest, Run_ClosureIsExecuted)
{
    int executionCount = 0;
    CancellableTaskFactory factory(WTF::bind(&AddOne, &executionCount));
    OwnPtr<WebThread::Task> task = adoptPtr(factory.cancelAndCreate());
    task->run();

    EXPECT_EQ(1, executionCount);
}

TEST_F(CancellableTaskFactoryTest, Run_ClosureIsExecutedOnlyOnce)
{
    int executionCount = 0;
    CancellableTaskFactory factory(WTF::bind(&AddOne, &executionCount));
    OwnPtr<WebThread::Task> task = adoptPtr(factory.cancelAndCreate());
    task->run();
    task->run();
    task->run();
    task->run();

    EXPECT_EQ(1, executionCount);
}

TEST_F(CancellableTaskFactoryTest, Run_FactoryDestructionPreventsExecution)
{
    int executionCount = 0;
    OwnPtr<WebThread::Task> task;
    {
        CancellableTaskFactory factory(WTF::bind(&AddOne, &executionCount));
        task = adoptPtr(factory.cancelAndCreate());
    }
    task->run();

    EXPECT_EQ(0, executionCount);
}

TEST_F(CancellableTaskFactoryTest, Run_TasksInSequence)
{
    int executionCount = 0;
    CancellableTaskFactory factory(WTF::bind(&AddOne, &executionCount));

    OwnPtr<WebThread::Task> taskA = adoptPtr(factory.cancelAndCreate());
    taskA->run();
    EXPECT_EQ(1, executionCount);

    OwnPtr<WebThread::Task> taskB = adoptPtr(factory.cancelAndCreate());
    taskB->run();
    EXPECT_EQ(2, executionCount);

    OwnPtr<WebThread::Task> taskC = adoptPtr(factory.cancelAndCreate());
    taskC->run();
    EXPECT_EQ(3, executionCount);
}

TEST_F(CancellableTaskFactoryTest, Cancel)
{
    int executionCount = 0;
    CancellableTaskFactory factory(WTF::bind(&AddOne, &executionCount));
    OwnPtr<WebThread::Task> task = adoptPtr(factory.cancelAndCreate());
    factory.cancel();
    task->run();

    EXPECT_EQ(0, executionCount);
}

TEST_F(CancellableTaskFactoryTest, CreatingANewTaskCancelsPreviousOnes)
{
    int executionCount = 0;
    CancellableTaskFactory factory(WTF::bind(&AddOne, &executionCount));

    OwnPtr<WebThread::Task> taskA = adoptPtr(factory.cancelAndCreate());
    OwnPtr<WebThread::Task> taskB = adoptPtr(factory.cancelAndCreate());

    taskA->run();
    EXPECT_EQ(0, executionCount);

    taskB->run();
    EXPECT_EQ(1, executionCount);
}

} // namespace blink
