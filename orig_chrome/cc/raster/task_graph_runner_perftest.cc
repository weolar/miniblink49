// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/raster/task_graph_runner.h"

#include <vector>

#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"
#include "cc/base/completion_event.h"
#include "cc/debug/lap_timer.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/perf/perf_test.h"

namespace cc {
namespace {

    static const int kTimeLimitMillis = 2000;
    static const int kWarmupRuns = 5;
    static const int kTimeCheckInterval = 10;

    class PerfTaskImpl : public Task {
    public:
        typedef std::vector<scoped_refptr<PerfTaskImpl>> Vector;

        PerfTaskImpl() { }

        // Overridden from Task:
        void RunOnWorkerThread() override { }

        void Reset() { did_run_ = false; }

    private:
        ~PerfTaskImpl() override { }

        DISALLOW_COPY_AND_ASSIGN(PerfTaskImpl);
    };

    class TaskGraphRunnerPerfTest : public testing::Test {
    public:
        TaskGraphRunnerPerfTest()
            : timer_(kWarmupRuns,
                base::TimeDelta::FromMilliseconds(kTimeLimitMillis),
                kTimeCheckInterval)
        {
        }

        // Overridden from testing::Test:
        void SetUp() override
        {
            task_graph_runner_ = make_scoped_ptr(new TaskGraphRunner);
            namespace_token_ = task_graph_runner_->GetNamespaceToken();
        }
        void TearDown() override { task_graph_runner_ = nullptr; }

        void RunBuildTaskGraphTest(const std::string& test_name,
            int num_top_level_tasks,
            int num_tasks,
            int num_leaf_tasks)
        {
            PerfTaskImpl::Vector top_level_tasks;
            PerfTaskImpl::Vector tasks;
            PerfTaskImpl::Vector leaf_tasks;
            CreateTasks(num_top_level_tasks, &top_level_tasks);
            CreateTasks(num_tasks, &tasks);
            CreateTasks(num_leaf_tasks, &leaf_tasks);

            // Avoid unnecessary heap allocations by reusing the same graph.
            TaskGraph graph;

            timer_.Reset();
            do {
                graph.Reset();
                BuildTaskGraph(top_level_tasks, tasks, leaf_tasks, &graph);
                timer_.NextLap();
            } while (!timer_.HasTimeLimitExpired());

            perf_test::PrintResult("build_task_graph",
                TestModifierString(),
                test_name,
                timer_.LapsPerSecond(),
                "runs/s",
                true);
        }

        void RunScheduleTasksTest(const std::string& test_name,
            int num_top_level_tasks,
            int num_tasks,
            int num_leaf_tasks)
        {
            PerfTaskImpl::Vector top_level_tasks;
            PerfTaskImpl::Vector tasks;
            PerfTaskImpl::Vector leaf_tasks;
            CreateTasks(num_top_level_tasks, &top_level_tasks);
            CreateTasks(num_tasks, &tasks);
            CreateTasks(num_leaf_tasks, &leaf_tasks);

            // Avoid unnecessary heap allocations by reusing the same graph and
            // completed tasks vector.
            TaskGraph graph;
            Task::Vector completed_tasks;

            timer_.Reset();
            do {
                graph.Reset();
                BuildTaskGraph(top_level_tasks, tasks, leaf_tasks, &graph);
                task_graph_runner_->ScheduleTasks(namespace_token_, &graph);
                // Shouldn't be any tasks to collect as we reschedule the same set
                // of tasks.
                DCHECK_EQ(0u, CollectCompletedTasks(&completed_tasks));
                timer_.NextLap();
            } while (!timer_.HasTimeLimitExpired());

            TaskGraph empty;
            task_graph_runner_->ScheduleTasks(namespace_token_, &empty);
            CollectCompletedTasks(&completed_tasks);

            perf_test::PrintResult("schedule_tasks",
                TestModifierString(),
                test_name,
                timer_.LapsPerSecond(),
                "runs/s",
                true);
        }

        void RunScheduleAlternateTasksTest(const std::string& test_name,
            int num_top_level_tasks,
            int num_tasks,
            int num_leaf_tasks)
        {
            const size_t kNumVersions = 2;
            PerfTaskImpl::Vector top_level_tasks[kNumVersions];
            PerfTaskImpl::Vector tasks[kNumVersions];
            PerfTaskImpl::Vector leaf_tasks[kNumVersions];
            for (size_t i = 0; i < kNumVersions; ++i) {
                CreateTasks(num_top_level_tasks, &top_level_tasks[i]);
                CreateTasks(num_tasks, &tasks[i]);
                CreateTasks(num_leaf_tasks, &leaf_tasks[i]);
            }

            // Avoid unnecessary heap allocations by reusing the same graph and
            // completed tasks vector.
            TaskGraph graph;
            Task::Vector completed_tasks;

            size_t count = 0;
            timer_.Reset();
            do {
                graph.Reset();
                BuildTaskGraph(top_level_tasks[count % kNumVersions],
                    tasks[count % kNumVersions],
                    leaf_tasks[count % kNumVersions],
                    &graph);
                task_graph_runner_->ScheduleTasks(namespace_token_, &graph);
                CollectCompletedTasks(&completed_tasks);
                completed_tasks.clear();
                ++count;
                timer_.NextLap();
            } while (!timer_.HasTimeLimitExpired());

            TaskGraph empty;
            task_graph_runner_->ScheduleTasks(namespace_token_, &empty);
            CollectCompletedTasks(&completed_tasks);

            perf_test::PrintResult("schedule_alternate_tasks",
                TestModifierString(),
                test_name,
                timer_.LapsPerSecond(),
                "runs/s",
                true);
        }

        void RunScheduleAndExecuteTasksTest(const std::string& test_name,
            int num_top_level_tasks,
            int num_tasks,
            int num_leaf_tasks)
        {
            PerfTaskImpl::Vector top_level_tasks;
            PerfTaskImpl::Vector tasks;
            PerfTaskImpl::Vector leaf_tasks;
            CreateTasks(num_top_level_tasks, &top_level_tasks);
            CreateTasks(num_tasks, &tasks);
            CreateTasks(num_leaf_tasks, &leaf_tasks);

            // Avoid unnecessary heap allocations by reusing the same graph and
            // completed tasks vector.
            TaskGraph graph;
            Task::Vector completed_tasks;

            timer_.Reset();
            do {
                graph.Reset();
                BuildTaskGraph(top_level_tasks, tasks, leaf_tasks, &graph);
                task_graph_runner_->ScheduleTasks(namespace_token_, &graph);
                task_graph_runner_->RunUntilIdle();
                CollectCompletedTasks(&completed_tasks);
                completed_tasks.clear();
                ResetTasks(&top_level_tasks);
                ResetTasks(&tasks);
                ResetTasks(&leaf_tasks);
                timer_.NextLap();
            } while (!timer_.HasTimeLimitExpired());

            perf_test::PrintResult("execute_tasks",
                TestModifierString(),
                test_name,
                timer_.LapsPerSecond(),
                "runs/s",
                true);
        }

    private:
        static std::string TestModifierString()
        {
            return std::string("_task_graph_runner");
        }

        void CreateTasks(int num_tasks, PerfTaskImpl::Vector* tasks)
        {
            for (int i = 0; i < num_tasks; ++i)
                tasks->push_back(make_scoped_refptr(new PerfTaskImpl));
        }

        void ResetTasks(PerfTaskImpl::Vector* tasks)
        {
            for (PerfTaskImpl::Vector::iterator it = tasks->begin(); it != tasks->end();
                 ++it) {
                PerfTaskImpl* task = it->get();
                task->Reset();
            }
        }

        void BuildTaskGraph(const PerfTaskImpl::Vector& top_level_tasks,
            const PerfTaskImpl::Vector& tasks,
            const PerfTaskImpl::Vector& leaf_tasks,
            TaskGraph* graph)
        {
            DCHECK(graph->nodes.empty());
            DCHECK(graph->edges.empty());

            for (PerfTaskImpl::Vector::const_iterator it = leaf_tasks.begin();
                 it != leaf_tasks.end();
                 ++it) {
                graph->nodes.push_back(TaskGraph::Node(it->get(), 0u, 0u));
            }

            for (PerfTaskImpl::Vector::const_iterator it = tasks.begin();
                 it != tasks.end();
                 ++it) {
                graph->nodes.push_back(TaskGraph::Node(it->get(), 0u, leaf_tasks.size()));

                for (PerfTaskImpl::Vector::const_iterator leaf_it = leaf_tasks.begin();
                     leaf_it != leaf_tasks.end();
                     ++leaf_it) {
                    graph->edges.push_back(TaskGraph::Edge(leaf_it->get(), it->get()));
                }

                for (PerfTaskImpl::Vector::const_iterator top_level_it = top_level_tasks.begin();
                     top_level_it != top_level_tasks.end();
                     ++top_level_it) {
                    graph->edges.push_back(TaskGraph::Edge(it->get(), top_level_it->get()));
                }
            }

            for (PerfTaskImpl::Vector::const_iterator it = top_level_tasks.begin();
                 it != top_level_tasks.end();
                 ++it) {
                graph->nodes.push_back(TaskGraph::Node(it->get(), 0u, tasks.size()));
            }
        }

        size_t CollectCompletedTasks(Task::Vector* completed_tasks)
        {
            DCHECK(completed_tasks->empty());
            task_graph_runner_->CollectCompletedTasks(namespace_token_,
                completed_tasks);
            return completed_tasks->size();
        }

        scoped_ptr<TaskGraphRunner> task_graph_runner_;
        NamespaceToken namespace_token_;
        LapTimer timer_;
    };

    TEST_F(TaskGraphRunnerPerfTest, BuildTaskGraph)
    {
        RunBuildTaskGraphTest("0_1_0", 0, 1, 0);
        RunBuildTaskGraphTest("0_32_0", 0, 32, 0);
        RunBuildTaskGraphTest("2_1_0", 2, 1, 0);
        RunBuildTaskGraphTest("2_32_0", 2, 32, 0);
        RunBuildTaskGraphTest("2_1_1", 2, 1, 1);
        RunBuildTaskGraphTest("2_32_1", 2, 32, 1);
    }

    TEST_F(TaskGraphRunnerPerfTest, ScheduleTasks)
    {
        RunScheduleTasksTest("0_1_0", 0, 1, 0);
        RunScheduleTasksTest("0_32_0", 0, 32, 0);
        RunScheduleTasksTest("2_1_0", 2, 1, 0);
        RunScheduleTasksTest("2_32_0", 2, 32, 0);
        RunScheduleTasksTest("2_1_1", 2, 1, 1);
        RunScheduleTasksTest("2_32_1", 2, 32, 1);
    }

    TEST_F(TaskGraphRunnerPerfTest, ScheduleAlternateTasks)
    {
        RunScheduleAlternateTasksTest("0_1_0", 0, 1, 0);
        RunScheduleAlternateTasksTest("0_32_0", 0, 32, 0);
        RunScheduleAlternateTasksTest("2_1_0", 2, 1, 0);
        RunScheduleAlternateTasksTest("2_32_0", 2, 32, 0);
        RunScheduleAlternateTasksTest("2_1_1", 2, 1, 1);
        RunScheduleAlternateTasksTest("2_32_1", 2, 32, 1);
    }

    TEST_F(TaskGraphRunnerPerfTest, ScheduleAndExecuteTasks)
    {
        RunScheduleAndExecuteTasksTest("0_1_0", 0, 1, 0);
        RunScheduleAndExecuteTasksTest("0_32_0", 0, 32, 0);
        RunScheduleAndExecuteTasksTest("2_1_0", 2, 1, 0);
        RunScheduleAndExecuteTasksTest("2_32_0", 2, 32, 0);
        RunScheduleAndExecuteTasksTest("2_1_1", 2, 1, 1);
        RunScheduleAndExecuteTasksTest("2_32_1", 2, 32, 1);
    }

} // namespace
} // namespace cc
