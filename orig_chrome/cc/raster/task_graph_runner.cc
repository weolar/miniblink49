// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/raster/task_graph_runner.h"

#include <algorithm>

#include "base/containers/hash_tables.h"
#include "base/strings/stringprintf.h"
#include "base/threading/thread_restrictions.h"
#include "base/trace_event/trace_event.h"

namespace cc {
namespace {

    // Helper class for iterating over all dependents of a task.
    class DependentIterator {
    public:
        DependentIterator(TaskGraph* graph, const Task* task)
            : graph_(graph)
            , task_(task)
            , current_index_(static_cast<size_t>(-1))
            , current_node_(NULL)
        {
            ++(*this);
        }

        TaskGraph::Node& operator->() const
        {
            DCHECK_LT(current_index_, graph_->edges.size());
            DCHECK_EQ(graph_->edges[current_index_].task, task_);
            DCHECK(current_node_);
            return *current_node_;
        }

        TaskGraph::Node& operator*() const
        {
            DCHECK_LT(current_index_, graph_->edges.size());
            DCHECK_EQ(graph_->edges[current_index_].task, task_);
            DCHECK(current_node_);
            return *current_node_;
        }

        // Note: Performance can be improved by keeping edges sorted.
        DependentIterator& operator++()
        {
            // Find next dependency edge for |task_|.
            do {
                ++current_index_;
                if (current_index_ == graph_->edges.size())
                    return *this;
            } while (graph_->edges[current_index_].task != task_);

            // Now find the node for the dependent of this edge.
            TaskGraph::Node::Vector::iterator it = std::find_if(graph_->nodes.begin(),
                graph_->nodes.end(),
                TaskGraph::Node::TaskComparator(
                    graph_->edges[current_index_].dependent));
            DCHECK(it != graph_->nodes.end());
            current_node_ = &(*it);

            return *this;
        }

        operator bool() const { return current_index_ < graph_->edges.size(); }

    private:
        TaskGraph* graph_;
        const Task* task_;
        size_t current_index_;
        TaskGraph::Node* current_node_;
    };

    bool DependencyMismatch(const TaskGraph* graph)
    {
        // Value storage will be 0-initialized.
        base::hash_map<const Task*, size_t> dependents;
        for (const TaskGraph::Edge& edge : graph->edges)
            dependents[edge.dependent]++;

        for (const TaskGraph::Node& node : graph->nodes) {
            if (dependents[node.task] != node.dependencies)
                return true;
        }

        return false;
    }

} // namespace

Task::Task()
    : will_run_(false)
    , did_run_(false)
{
}

Task::~Task()
{
    DCHECK(!will_run_);
}

void Task::WillRun()
{
    DCHECK(!will_run_);
    DCHECK(!did_run_);
    will_run_ = true;
}

void Task::DidRun()
{
    DCHECK(will_run_);
    will_run_ = false;
    did_run_ = true;
}

bool Task::HasFinishedRunning() const { return did_run_; }

TaskGraph::TaskGraph() { }

TaskGraph::~TaskGraph() { }

void TaskGraph::Swap(TaskGraph* other)
{
    nodes.swap(other->nodes);
    edges.swap(other->edges);
}

void TaskGraph::Reset()
{
    nodes.clear();
    edges.clear();
}

TaskGraphRunner::TaskNamespace::TaskNamespace() { }

TaskGraphRunner::TaskNamespace::~TaskNamespace() { }

TaskGraphRunner::TaskGraphRunner()
    : lock_()
    , has_ready_to_run_tasks_cv_(&lock_)
    , has_namespaces_with_finished_running_tasks_cv_(&lock_)
    , next_namespace_id_(1)
    , shutdown_(false)
{
}

TaskGraphRunner::~TaskGraphRunner()
{
    {
        base::AutoLock lock(lock_);

        DCHECK_EQ(0u, ready_to_run_namespaces_.size());
        DCHECK_EQ(0u, namespaces_.size());
    }
}

NamespaceToken TaskGraphRunner::GetNamespaceToken()
{
    base::AutoLock lock(lock_);

    NamespaceToken token(next_namespace_id_++);
    DCHECK(namespaces_.find(token.id_) == namespaces_.end());
    return token;
}

void TaskGraphRunner::ScheduleTasks(NamespaceToken token, TaskGraph* graph)
{
    TRACE_EVENT2("cc",
        "TaskGraphRunner::ScheduleTasks",
        "num_nodes",
        graph->nodes.size(),
        "num_edges",
        graph->edges.size());

    DCHECK(token.IsValid());
    DCHECK(!DependencyMismatch(graph));

    {
        base::AutoLock lock(lock_);

        DCHECK(!shutdown_);

        TaskNamespace& task_namespace = namespaces_[token.id_];

        // First adjust number of dependencies to reflect completed tasks.
        for (Task::Vector::iterator it = task_namespace.completed_tasks.begin();
             it != task_namespace.completed_tasks.end();
             ++it) {
            for (DependentIterator node_it(graph, it->get()); node_it; ++node_it) {
                TaskGraph::Node& node = *node_it;
                DCHECK_LT(0u, node.dependencies);
                node.dependencies--;
            }
        }

        // Build new "ready to run" queue and remove nodes from old graph.
        task_namespace.ready_to_run_tasks.clear();
        for (TaskGraph::Node::Vector::iterator it = graph->nodes.begin();
             it != graph->nodes.end();
             ++it) {
            TaskGraph::Node& node = *it;

            // Remove any old nodes that are associated with this task. The result is
            // that the old graph is left with all nodes not present in this graph,
            // which we use below to determine what tasks need to be canceled.
            TaskGraph::Node::Vector::iterator old_it = std::find_if(task_namespace.graph.nodes.begin(),
                task_namespace.graph.nodes.end(),
                TaskGraph::Node::TaskComparator(node.task));
            if (old_it != task_namespace.graph.nodes.end()) {
                std::swap(*old_it, task_namespace.graph.nodes.back());
                task_namespace.graph.nodes.pop_back();
            }

            // Task is not ready to run if dependencies are not yet satisfied.
            if (node.dependencies)
                continue;

            // Skip if already finished running task.
            if (node.task->HasFinishedRunning())
                continue;

            // Skip if already running.
            if (std::find(task_namespace.running_tasks.begin(),
                    task_namespace.running_tasks.end(),
                    node.task)
                != task_namespace.running_tasks.end())
                continue;

            task_namespace.ready_to_run_tasks.push_back(
                PrioritizedTask(node.task, node.priority));
        }

        // Rearrange the elements in |ready_to_run_tasks| in such a way that they
        // form a heap.
        std::make_heap(task_namespace.ready_to_run_tasks.begin(),
            task_namespace.ready_to_run_tasks.end(),
            CompareTaskPriority);

        // Swap task graph.
        task_namespace.graph.Swap(graph);

        // Determine what tasks in old graph need to be canceled.
        for (TaskGraph::Node::Vector::iterator it = graph->nodes.begin();
             it != graph->nodes.end();
             ++it) {
            TaskGraph::Node& node = *it;

            // Skip if already finished running task.
            if (node.task->HasFinishedRunning())
                continue;

            // Skip if already running.
            if (std::find(task_namespace.running_tasks.begin(),
                    task_namespace.running_tasks.end(),
                    node.task)
                != task_namespace.running_tasks.end())
                continue;

            DCHECK(std::find(task_namespace.completed_tasks.begin(),
                       task_namespace.completed_tasks.end(),
                       node.task)
                == task_namespace.completed_tasks.end());
            task_namespace.completed_tasks.push_back(node.task);
        }

        // Build new "ready to run" task namespaces queue.
        ready_to_run_namespaces_.clear();
        for (TaskNamespaceMap::iterator it = namespaces_.begin();
             it != namespaces_.end();
             ++it) {
            if (!it->second.ready_to_run_tasks.empty())
                ready_to_run_namespaces_.push_back(&it->second);
        }

        // Rearrange the task namespaces in |ready_to_run_namespaces_| in such a way
        // that they form a heap.
        std::make_heap(ready_to_run_namespaces_.begin(),
            ready_to_run_namespaces_.end(),
            CompareTaskNamespacePriority);

        // If there is more work available, wake up worker thread.
        if (!ready_to_run_namespaces_.empty())
            has_ready_to_run_tasks_cv_.Signal();
    }
}

void TaskGraphRunner::WaitForTasksToFinishRunning(NamespaceToken token)
{
    TRACE_EVENT0("cc", "TaskGraphRunner::WaitForTasksToFinishRunning");

    DCHECK(token.IsValid());

    {
        base::AutoLock lock(lock_);
        base::ThreadRestrictions::ScopedAllowWait allow_wait;

        TaskNamespaceMap::const_iterator it = namespaces_.find(token.id_);
        if (it == namespaces_.end())
            return;

        const TaskNamespace& task_namespace = it->second;

        while (!HasFinishedRunningTasksInNamespace(&task_namespace))
            has_namespaces_with_finished_running_tasks_cv_.Wait();

        // There may be other namespaces that have finished running tasks, so wake
        // up another origin thread.
        has_namespaces_with_finished_running_tasks_cv_.Signal();
    }
}

void TaskGraphRunner::CollectCompletedTasks(NamespaceToken token,
    Task::Vector* completed_tasks)
{
    TRACE_EVENT0("cc", "TaskGraphRunner::CollectCompletedTasks");

    DCHECK(token.IsValid());

    {
        base::AutoLock lock(lock_);

        TaskNamespaceMap::iterator it = namespaces_.find(token.id_);
        if (it == namespaces_.end())
            return;

        TaskNamespace& task_namespace = it->second;

        DCHECK_EQ(0u, completed_tasks->size());
        completed_tasks->swap(task_namespace.completed_tasks);
        if (!HasFinishedRunningTasksInNamespace(&task_namespace))
            return;

        // Remove namespace if finished running tasks.
        DCHECK_EQ(0u, task_namespace.completed_tasks.size());
        DCHECK_EQ(0u, task_namespace.ready_to_run_tasks.size());
        DCHECK_EQ(0u, task_namespace.running_tasks.size());
        namespaces_.erase(it);
    }
}

void TaskGraphRunner::Shutdown()
{
    base::AutoLock lock(lock_);

    DCHECK_EQ(0u, ready_to_run_namespaces_.size());
    DCHECK_EQ(0u, namespaces_.size());

    DCHECK(!shutdown_);
    shutdown_ = true;

    // Wake up a worker so it knows it should exit. This will cause all workers
    // to exit as each will wake up another worker before exiting.
    has_ready_to_run_tasks_cv_.Signal();
}

void TaskGraphRunner::FlushForTesting()
{
    base::AutoLock lock(lock_);

    while (std::find_if(namespaces_.begin(), namespaces_.end(),
               [](const TaskNamespaceMap::value_type& entry) {
                   return !HasFinishedRunningTasksInNamespace(
                       &entry.second);
               })
        != namespaces_.end()) {
        has_namespaces_with_finished_running_tasks_cv_.Wait();
    }
}

void TaskGraphRunner::Run()
{
    base::AutoLock lock(lock_);

    while (true) {
        if (ready_to_run_namespaces_.empty()) {
            // Exit when shutdown is set and no more tasks are pending.
            if (shutdown_)
                break;

            // Wait for more tasks.
            has_ready_to_run_tasks_cv_.Wait();
            continue;
        }

        RunTaskWithLockAcquired();
    }

    // We noticed we should exit. Wake up the next worker so it knows it should
    // exit as well (because the Shutdown() code only signals once).
    has_ready_to_run_tasks_cv_.Signal();
}

void TaskGraphRunner::RunUntilIdle()
{
    base::AutoLock lock(lock_);

    while (!ready_to_run_namespaces_.empty())
        RunTaskWithLockAcquired();
}

void TaskGraphRunner::RunTaskWithLockAcquired()
{
    TRACE_EVENT0("toplevel", "TaskGraphRunner::RunTask");

    lock_.AssertAcquired();
    DCHECK(!ready_to_run_namespaces_.empty());

    // Take top priority TaskNamespace from |ready_to_run_namespaces_|.
    std::pop_heap(ready_to_run_namespaces_.begin(),
        ready_to_run_namespaces_.end(),
        CompareTaskNamespacePriority);
    TaskNamespace* task_namespace = ready_to_run_namespaces_.back();
    ready_to_run_namespaces_.pop_back();
    DCHECK(!task_namespace->ready_to_run_tasks.empty());

    // Take top priority task from |ready_to_run_tasks|.
    std::pop_heap(task_namespace->ready_to_run_tasks.begin(),
        task_namespace->ready_to_run_tasks.end(),
        CompareTaskPriority);
    scoped_refptr<Task> task(task_namespace->ready_to_run_tasks.back().task);
    task_namespace->ready_to_run_tasks.pop_back();

    // Add task namespace back to |ready_to_run_namespaces_| if not empty after
    // taking top priority task.
    if (!task_namespace->ready_to_run_tasks.empty()) {
        ready_to_run_namespaces_.push_back(task_namespace);
        std::push_heap(ready_to_run_namespaces_.begin(),
            ready_to_run_namespaces_.end(),
            CompareTaskNamespacePriority);
    }

    // Add task to |running_tasks|.
    task_namespace->running_tasks.push_back(task.get());

    // There may be more work available, so wake up another worker thread.
    has_ready_to_run_tasks_cv_.Signal();

    // Call WillRun() before releasing |lock_| and running task.
    task->WillRun();

    {
        base::AutoUnlock unlock(lock_);

        task->RunOnWorkerThread();
    }

    // This will mark task as finished running.
    task->DidRun();

    // Remove task from |running_tasks|.
    TaskVector::iterator it = std::find(task_namespace->running_tasks.begin(),
        task_namespace->running_tasks.end(),
        task.get());
    DCHECK(it != task_namespace->running_tasks.end());
    std::swap(*it, task_namespace->running_tasks.back());
    task_namespace->running_tasks.pop_back();

    // Now iterate over all dependents to decrement dependencies and check if they
    // are ready to run.
    bool ready_to_run_namespaces_has_heap_properties = true;
    for (DependentIterator it(&task_namespace->graph, task.get()); it; ++it) {
        TaskGraph::Node& dependent_node = *it;

        DCHECK_LT(0u, dependent_node.dependencies);
        dependent_node.dependencies--;
        // Task is ready if it has no dependencies. Add it to |ready_to_run_tasks_|.
        if (!dependent_node.dependencies) {
            bool was_empty = task_namespace->ready_to_run_tasks.empty();
            task_namespace->ready_to_run_tasks.push_back(
                PrioritizedTask(dependent_node.task, dependent_node.priority));
            std::push_heap(task_namespace->ready_to_run_tasks.begin(),
                task_namespace->ready_to_run_tasks.end(),
                CompareTaskPriority);
            // Task namespace is ready if it has at least one ready to run task. Add
            // it to |ready_to_run_namespaces_| if it just become ready.
            if (was_empty) {
                DCHECK(std::find(ready_to_run_namespaces_.begin(),
                           ready_to_run_namespaces_.end(),
                           task_namespace)
                    == ready_to_run_namespaces_.end());
                ready_to_run_namespaces_.push_back(task_namespace);
            }
            ready_to_run_namespaces_has_heap_properties = false;
        }
    }

    // Rearrange the task namespaces in |ready_to_run_namespaces_| in such a way
    // that they yet again form a heap.
    if (!ready_to_run_namespaces_has_heap_properties) {
        std::make_heap(ready_to_run_namespaces_.begin(),
            ready_to_run_namespaces_.end(),
            CompareTaskNamespacePriority);
    }

    // Finally add task to |completed_tasks_|.
    task_namespace->completed_tasks.push_back(task);

    // If namespace has finished running all tasks, wake up origin thread.
    if (HasFinishedRunningTasksInNamespace(task_namespace))
        has_namespaces_with_finished_running_tasks_cv_.Signal();
}

} // namespace cc
