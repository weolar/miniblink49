// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/raster/task_graph_runner.h"

#include <vector>

#include "base/bind.h"
#include "base/synchronization/lock.h"
#include "base/threading/simple_thread.h"
#include "cc/base/scoped_ptr_deque.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    const int kNamespaceCount = 3;

    class TaskGraphRunnerTestBase {
    public:
        struct TaskInfo {
            TaskInfo(int namespace_index,
                unsigned id,
                unsigned dependent_id,
                unsigned dependent_count,
                unsigned priority)
                : namespace_index(namespace_index)
                , id(id)
                , dependent_id(dependent_id)
                , dependent_count(dependent_count)
                , priority(priority)
            {
            }

            int namespace_index;
            unsigned id;
            unsigned dependent_id;
            unsigned dependent_count;
            unsigned priority;
        };

        TaskGraphRunnerTestBase()
            : task_graph_runner_(new TaskGraphRunner)
        {
        }

        void ResetIds(int namespace_index)
        {
            run_task_ids_[namespace_index].clear();
            on_task_completed_ids_[namespace_index].clear();
        }

        void RunAllTasks(int namespace_index)
        {
            task_graph_runner_->WaitForTasksToFinishRunning(
                namespace_token_[namespace_index]);

            Task::Vector completed_tasks;
            task_graph_runner_->CollectCompletedTasks(namespace_token_[namespace_index],
                &completed_tasks);
            for (Task::Vector::const_iterator it = completed_tasks.begin();
                 it != completed_tasks.end();
                 ++it) {
                FakeTaskImpl* task = static_cast<FakeTaskImpl*>(it->get());
                task->CompleteOnOriginThread();
            }
        }

        void RunTaskOnWorkerThread(int namespace_index, unsigned id)
        {
            base::AutoLock lock(run_task_ids_lock_);
            run_task_ids_[namespace_index].push_back(id);
        }

        void OnTaskCompleted(int namespace_index, unsigned id)
        {
            on_task_completed_ids_[namespace_index].push_back(id);
        }

        const std::vector<unsigned>& run_task_ids(int namespace_index)
        {
            return run_task_ids_[namespace_index];
        }

        const std::vector<unsigned>& on_task_completed_ids(int namespace_index)
        {
            return on_task_completed_ids_[namespace_index];
        }

        void ScheduleTasks(int namespace_index, const std::vector<TaskInfo>& tasks)
        {
            Task::Vector new_tasks;
            Task::Vector new_dependents;
            TaskGraph new_graph;

            for (std::vector<TaskInfo>::const_iterator it = tasks.begin();
                 it != tasks.end();
                 ++it) {
                scoped_refptr<FakeTaskImpl> new_task(
                    new FakeTaskImpl(this, it->namespace_index, it->id));
                new_graph.nodes.push_back(
                    TaskGraph::Node(new_task.get(), it->priority, 0u));
                for (unsigned i = 0; i < it->dependent_count; ++i) {
                    scoped_refptr<FakeDependentTaskImpl> new_dependent_task(
                        new FakeDependentTaskImpl(
                            this, it->namespace_index, it->dependent_id));
                    new_graph.nodes.push_back(
                        TaskGraph::Node(new_dependent_task.get(), it->priority, 1u));
                    new_graph.edges.push_back(
                        TaskGraph::Edge(new_task.get(), new_dependent_task.get()));

                    new_dependents.push_back(new_dependent_task.get());
                }

                new_tasks.push_back(new_task.get());
            }

            task_graph_runner_->ScheduleTasks(namespace_token_[namespace_index],
                &new_graph);

            dependents_[namespace_index].swap(new_dependents);
            tasks_[namespace_index].swap(new_tasks);
        }

    protected:
        class FakeTaskImpl : public Task {
        public:
            FakeTaskImpl(TaskGraphRunnerTestBase* test, int namespace_index, int id)
                : test_(test)
                , namespace_index_(namespace_index)
                , id_(id)
            {
            }

            // Overridden from Task:
            void RunOnWorkerThread() override
            {
                test_->RunTaskOnWorkerThread(namespace_index_, id_);
            }

            virtual void CompleteOnOriginThread()
            {
                test_->OnTaskCompleted(namespace_index_, id_);
            }

        protected:
            ~FakeTaskImpl() override { }

        private:
            TaskGraphRunnerTestBase* test_;
            int namespace_index_;
            int id_;

            DISALLOW_COPY_AND_ASSIGN(FakeTaskImpl);
        };

        class FakeDependentTaskImpl : public FakeTaskImpl {
        public:
            FakeDependentTaskImpl(TaskGraphRunnerTestBase* test,
                int namespace_index,
                int id)
                : FakeTaskImpl(test, namespace_index, id)
            {
            }

            // Overridden from FakeTaskImpl:
            void CompleteOnOriginThread() override { }

        private:
            ~FakeDependentTaskImpl() override { }

            DISALLOW_COPY_AND_ASSIGN(FakeDependentTaskImpl);
        };

        scoped_ptr<TaskGraphRunner> task_graph_runner_;
        NamespaceToken namespace_token_[kNamespaceCount];
        Task::Vector tasks_[kNamespaceCount];
        Task::Vector dependents_[kNamespaceCount];
        std::vector<unsigned> run_task_ids_[kNamespaceCount];
        base::Lock run_task_ids_lock_;
        std::vector<unsigned> on_task_completed_ids_[kNamespaceCount];
    };

    class TaskGraphRunnerTest : public TaskGraphRunnerTestBase,
                                public testing::TestWithParam<int>,
                                public base::DelegateSimpleThread::Delegate {
    public:
        // Overridden from testing::Test:
        void SetUp() override
        {
            const size_t num_threads = GetParam();
            while (workers_.size() < num_threads) {
                scoped_ptr<base::DelegateSimpleThread> worker = make_scoped_ptr(new base::DelegateSimpleThread(this, "TestWorker"));
                worker->Start();
                workers_.push_back(worker.Pass());
            }

            for (int i = 0; i < kNamespaceCount; ++i)
                namespace_token_[i] = task_graph_runner_->GetNamespaceToken();
        }
        void TearDown() override
        {
            task_graph_runner_->Shutdown();
            while (workers_.size()) {
                scoped_ptr<base::DelegateSimpleThread> worker = workers_.take_front();
                worker->Join();
            }
        }

    private:
        // Overridden from base::DelegateSimpleThread::Delegate:
        void Run() override { task_graph_runner_->Run(); }

        ScopedPtrDeque<base::DelegateSimpleThread> workers_;
    };

    TEST_P(TaskGraphRunnerTest, Basic)
    {
        for (int i = 0; i < kNamespaceCount; ++i) {
            EXPECT_EQ(0u, run_task_ids(i).size());
            EXPECT_EQ(0u, on_task_completed_ids(i).size());

            ScheduleTasks(i, std::vector<TaskInfo>(1, TaskInfo(i, 0u, 0u, 0u, 0u)));
        }

        for (int i = 0; i < kNamespaceCount; ++i) {
            RunAllTasks(i);

            EXPECT_EQ(1u, run_task_ids(i).size());
            EXPECT_EQ(1u, on_task_completed_ids(i).size());
        }

        for (int i = 0; i < kNamespaceCount; ++i)
            ScheduleTasks(i, std::vector<TaskInfo>(1, TaskInfo(i, 0u, 0u, 1u, 0u)));

        for (int i = 0; i < kNamespaceCount; ++i) {
            RunAllTasks(i);

            EXPECT_EQ(3u, run_task_ids(i).size());
            EXPECT_EQ(2u, on_task_completed_ids(i).size());
        }

        for (int i = 0; i < kNamespaceCount; ++i)
            ScheduleTasks(i, std::vector<TaskInfo>(1, TaskInfo(i, 0u, 0u, 2u, 0u)));

        for (int i = 0; i < kNamespaceCount; ++i) {
            RunAllTasks(i);

            EXPECT_EQ(6u, run_task_ids(i).size());
            EXPECT_EQ(3u, on_task_completed_ids(i).size());
        }
    }

    TEST_P(TaskGraphRunnerTest, Dependencies)
    {
        for (int i = 0; i < kNamespaceCount; ++i) {
            ScheduleTasks(i,
                std::vector<TaskInfo>(1,
                    TaskInfo(i,
                        0u,
                        1u,
                        1u, // 1 dependent
                        0u)));
        }

        for (int i = 0; i < kNamespaceCount; ++i) {
            RunAllTasks(i);

            // Check if task ran before dependent.
            ASSERT_EQ(2u, run_task_ids(i).size());
            EXPECT_EQ(0u, run_task_ids(i)[0]);
            EXPECT_EQ(1u, run_task_ids(i)[1]);
            ASSERT_EQ(1u, on_task_completed_ids(i).size());
            EXPECT_EQ(0u, on_task_completed_ids(i)[0]);
        }

        for (int i = 0; i < kNamespaceCount; ++i) {
            ScheduleTasks(i,
                std::vector<TaskInfo>(1,
                    TaskInfo(i,
                        2u,
                        3u,
                        2u, // 2 dependents
                        0u)));
        }

        for (int i = 0; i < kNamespaceCount; ++i) {
            RunAllTasks(i);

            // Task should only run once.
            ASSERT_EQ(5u, run_task_ids(i).size());
            EXPECT_EQ(2u, run_task_ids(i)[2]);
            EXPECT_EQ(3u, run_task_ids(i)[3]);
            EXPECT_EQ(3u, run_task_ids(i)[4]);
            ASSERT_EQ(2u, on_task_completed_ids(i).size());
            EXPECT_EQ(2u, on_task_completed_ids(i)[1]);
        }
    }

    INSTANTIATE_TEST_CASE_P(TaskGraphRunnerTests,
        TaskGraphRunnerTest,
        ::testing::Range(1, 5));

    class TaskGraphRunnerSingleThreadTest
        : public TaskGraphRunnerTestBase,
          public testing::Test,
          public base::DelegateSimpleThread::Delegate {
    public:
        // Overridden from testing::Test:
        void SetUp() override
        {
            worker_.reset(new base::DelegateSimpleThread(this, "TestWorker"));
            worker_->Start();

            for (int i = 0; i < kNamespaceCount; ++i)
                namespace_token_[i] = task_graph_runner_->GetNamespaceToken();
        }
        void TearDown() override
        {
            task_graph_runner_->Shutdown();
            worker_->Join();
        }

    private:
        // Overridden from base::DelegateSimpleThread::Delegate:
        void Run() override { task_graph_runner_->Run(); }

        scoped_ptr<base::DelegateSimpleThread> worker_;
    };

    TEST_F(TaskGraphRunnerSingleThreadTest, Priority)
    {
        for (int i = 0; i < kNamespaceCount; ++i) {
            TaskInfo tasks[] = {
                TaskInfo(i, 0u, 2u, 1u, 1u), // Priority 1
                TaskInfo(i, 1u, 3u, 1u, 0u) // Priority 0
            };
            ScheduleTasks(i, std::vector<TaskInfo>(tasks, tasks + arraysize(tasks)));
        }

        for (int i = 0; i < kNamespaceCount; ++i) {
            RunAllTasks(i);

            // Check if tasks ran in order of priority.
            ASSERT_EQ(4u, run_task_ids(i).size());
            EXPECT_EQ(1u, run_task_ids(i)[0]);
            EXPECT_EQ(3u, run_task_ids(i)[1]);
            EXPECT_EQ(0u, run_task_ids(i)[2]);
            EXPECT_EQ(2u, run_task_ids(i)[3]);
            ASSERT_EQ(2u, on_task_completed_ids(i).size());
            EXPECT_EQ(1u, on_task_completed_ids(i)[0]);
            EXPECT_EQ(0u, on_task_completed_ids(i)[1]);
        }
    }

} // namespace
} // namespace cc
