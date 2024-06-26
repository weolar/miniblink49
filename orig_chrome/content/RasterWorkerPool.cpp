// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "orig_chrome/content/RasterWorkerPool.h"

#include "base/strings/stringprintf.h"

namespace content {

// A sequenced task runner which posts tasks to a RasterWorkerPool.
class RasterWorkerPool::RasterWorkerPoolSequencedTaskRunner
    : public base::SequencedTaskRunner {
public:
    RasterWorkerPoolSequencedTaskRunner(cc::TaskGraphRunner* task_graph_runner)
        : task_graph_runner_(task_graph_runner)
        , namespace_token_(task_graph_runner->GetNamespaceToken())
    {
    }

    // Overridden from base::TaskRunner:
    bool PostDelayedTask(const tracked_objects::Location& from_here,
        const base::Closure& task,
        base::TimeDelta delay) override
    {
        return PostNonNestableDelayedTask(from_here, task, delay);
    }
    bool RunsTasksOnCurrentThread() const override
    {
        return true;
    }

    // Overridden from base::SequencedTaskRunner:
    bool PostNonNestableDelayedTask(const tracked_objects::Location& from_here,
        const base::Closure& task,
        base::TimeDelta delay) override
    {
        base::AutoLock lock(lock_);

        // Remove completed tasks.
        DCHECK(completed_tasks_.empty());
        task_graph_runner_->CollectCompletedTasks(namespace_token_,
            &completed_tasks_);

        tasks_.erase(tasks_.begin(), tasks_.begin() + completed_tasks_.size());

        tasks_.push_back(make_scoped_refptr(new ClosureTask(task)));
        graph_.Reset();
        for (const auto& graph_task : tasks_) {
            int dependencies = 0;
            if (!graph_.nodes.empty())
                dependencies = 1;

            cc::TaskGraph::Node node(graph_task.get(), 0, dependencies);
            if (dependencies) {
                graph_.edges.push_back(
                    cc::TaskGraph::Edge(graph_.nodes.back().task, node.task));
            }
            graph_.nodes.push_back(node);
        }
        task_graph_runner_->ScheduleTasks(namespace_token_, &graph_);
        completed_tasks_.clear();
        return true;
    }

private:
    ~RasterWorkerPoolSequencedTaskRunner() override
    {
        task_graph_runner_->WaitForTasksToFinishRunning(namespace_token_);
        task_graph_runner_->CollectCompletedTasks(namespace_token_,
            &completed_tasks_);
    };

    cc::TaskGraphRunner* const task_graph_runner_;

    // Lock to exclusively access all the following members that are used to
    // implement the SequencedTaskRunner interfaces.
    base::Lock lock_;
    // Namespace used to schedule tasks in the task graph runner.
    cc::NamespaceToken namespace_token_;
    // List of tasks currently queued up for execution.
    cc::Task::Vector tasks_;
    // Graph object used for scheduling tasks.
    cc::TaskGraph graph_;
    // Cached vector to avoid allocation when getting the list of complete
    // tasks.
    cc::Task::Vector completed_tasks_;
};

RasterWorkerPool::RasterWorkerPool()
    : namespace_token_(task_graph_runner_.GetNamespaceToken())
{
}

void RasterWorkerPool::Start(
    int num_threads,
    const base::SimpleThread::Options& thread_options)
{
    DCHECK(threads_.empty());
    while (threads_.size() < static_cast<size_t>(num_threads)) {
        scoped_ptr<base::DelegateSimpleThread> thread(new base::DelegateSimpleThread(
            this, base::StringPrintf("CompositorTileWorker%u", static_cast<unsigned>(threads_.size() + 1)).c_str(),
            thread_options));
        thread->Start();
        threads_.push_back(thread.Pass());
    }
}

void RasterWorkerPool::Shutdown()
{
    task_graph_runner_.WaitForTasksToFinishRunning(namespace_token_);
    task_graph_runner_.CollectCompletedTasks(namespace_token_, &completed_tasks_);
    // Shutdown raster threads.
    task_graph_runner_.Shutdown();
    while (!threads_.empty()) {
        threads_.back()->Join();
        threads_.pop_back();
    }
}

// Overridden from base::TaskRunner:
bool RasterWorkerPool::PostDelayedTask(
    const tracked_objects::Location& from_here,
    const base::Closure& task,
    base::TimeDelta delay)
{
    base::AutoLock lock(lock_);

    // Remove completed tasks.
    DCHECK(completed_tasks_.empty());
    task_graph_runner_.CollectCompletedTasks(namespace_token_, &completed_tasks_);

    cc::Task::Vector::iterator end = std::remove_if(
        tasks_.begin(), tasks_.end(), [this](const scoped_refptr<cc::Task>& e) {
            return std::find(this->completed_tasks_.begin(),
                       this->completed_tasks_.end(),
                       e)
                != this->completed_tasks_.end();
        });
    tasks_.erase(end, tasks_.end());

    tasks_.push_back(make_scoped_refptr(new ClosureTask(task)));
    graph_.Reset();
    for (const auto& graph_task : tasks_)
        graph_.nodes.push_back(cc::TaskGraph::Node(graph_task.get(), 0, 0));

    task_graph_runner_.ScheduleTasks(namespace_token_, &graph_);
    completed_tasks_.clear();
    return true;
}

bool RasterWorkerPool::RunsTasksOnCurrentThread() const
{
    return true;
}

// Overridden from base::DelegateSimpleThread::Delegate:
void RasterWorkerPool::Run()
{
    task_graph_runner_.Run();
}

scoped_refptr<base::SequencedTaskRunner>
RasterWorkerPool::CreateSequencedTaskRunner()
{
    return new RasterWorkerPoolSequencedTaskRunner(&task_graph_runner_);
}

RasterWorkerPool::~RasterWorkerPool() { }

RasterWorkerPool::ClosureTask::ClosureTask(const base::Closure& closure)
    : closure_(closure)
{
}

// Overridden from cc::Task:
void RasterWorkerPool::ClosureTask::RunOnWorkerThread()
{
    closure_.Run();
    closure_.Reset();
};

RasterWorkerPool::ClosureTask::~ClosureTask() { }

} // namespace content
