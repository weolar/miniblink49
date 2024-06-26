// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/bind.h"
#include "base/callback.h"
#include "base/location.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/message_loop/message_loop.h"
#include "base/run_loop.h"
#include "base/sequence_checker_impl.h"
#include "base/sequenced_task_runner.h"
#include "base/synchronization/waitable_event.h"
#include "base/test/sequenced_worker_pool_owner.h"
#include "base/threading/sequenced_task_runner_handle.h"
#include "base/threading/sequenced_worker_pool.h"
#include "base/threading/simple_thread.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace base {
namespace {

    class SequencedTaskRunnerHandleTest : public ::testing::Test {
    protected:
        static void GetTaskRunner(const Closure& callback)
        {
            // Use SequenceCheckerImpl to make sure it's not a no-op in Release builds.
            scoped_ptr<SequenceCheckerImpl> sequence_checker(new SequenceCheckerImpl);
            ASSERT_TRUE(SequencedTaskRunnerHandle::IsSet());
            scoped_refptr<SequencedTaskRunner> task_runner = SequencedTaskRunnerHandle::Get();
            ASSERT_TRUE(task_runner);
            task_runner->PostTask(
                FROM_HERE, base::Bind(&SequencedTaskRunnerHandleTest::CheckValidThread, base::Passed(&sequence_checker), callback));
        }

    private:
        static void CheckValidThread(scoped_ptr<SequenceCheckerImpl> sequence_checker,
            const Closure& callback)
        {
            EXPECT_TRUE(sequence_checker->CalledOnValidSequencedThread());
            callback.Run();
        }

        MessageLoop message_loop_;
    };

    TEST_F(SequencedTaskRunnerHandleTest, FromMessageLoop)
    {
        RunLoop run_loop;
        GetTaskRunner(run_loop.QuitClosure());
        run_loop.Run();
    }

    TEST_F(SequencedTaskRunnerHandleTest, FromSequencedWorkerPool)
    {
        // Wrap the SequencedWorkerPool to avoid leaks due to its asynchronous
        // destruction.
        SequencedWorkerPoolOwner owner(3, "Test");
        WaitableEvent event(false, false);
        owner.pool()->PostSequencedWorkerTask(
            owner.pool()->GetSequenceToken(), FROM_HERE,
            base::Bind(&SequencedTaskRunnerHandleTest::GetTaskRunner,
                base::Bind(&WaitableEvent::Signal, base::Unretained(&event))));
        event.Wait();
        owner.pool()->Shutdown();
    }

    class ThreadRunner : public DelegateSimpleThread::Delegate {
    public:
        void Run() override
        {
            ASSERT_FALSE(SequencedTaskRunnerHandle::IsSet());
        }

    private:
        Closure callback_;
    };

    TEST_F(SequencedTaskRunnerHandleTest, FromSimpleThread)
    {
        ThreadRunner thread_runner;
        DelegateSimpleThread thread(&thread_runner, "Background thread");
        thread.Start();
        thread.Join();
    }

} // namespace
} // namespace base
