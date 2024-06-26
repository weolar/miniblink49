// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_RENDERER_RASTER_WORKER_POOL_H_
#define CONTENT_RENDERER_RASTER_WORKER_POOL_H_

#include "orig_chrome/base/callback.h"
#include "orig_chrome/base/containers/hash_tables.h"
#include "orig_chrome/base/memory/scoped_vector.h"
#include "orig_chrome/base/sequenced_task_runner.h"
#include "orig_chrome/base/task_runner.h"
#include "orig_chrome/base/threading/simple_thread.h"
#include "orig_chrome/cc/raster/task_graph_runner.h"

namespace content {

// A pool of threads used to run raster work.
// Work can be scheduled on the threads using different interfaces.
// The pool itself implements TaskRunner interface and tasks posted via that
// interface might run in parallel.
// CreateSequencedTaskRunner creates a sequenced task runner that might run in
// parallel with other instances of sequenced task runners.
// It's also possible to get the underlying TaskGraphRunner to schedule a graph
// of tasks with their dependencies.
// TODO(reveman): make TaskGraphRunner an abstract interface and have this
// WorkerPool class implement it.
class RasterWorkerPool
    : public base::TaskRunner,
      public base::DelegateSimpleThread::Delegate {
public:
    RasterWorkerPool();

    // Overridden from base::TaskRunner:
    bool PostDelayedTask(const tracked_objects::Location& from_here,
        const base::Closure& task,
        base::TimeDelta delay) override;
    bool RunsTasksOnCurrentThread() const override;

    // Overridden from base::DelegateSimpleThread::Delegate:
    void Run() override;

    // Spawn |num_threads| number of threads and start running work on the
    // worker threads.
    void Start(int num_threads,
        const base::SimpleThread::Options& thread_options);

    // Finish running all the posted tasks (and nested task posted by those tasks)
    // of all the associated task runners.
    // Once all the tasks are executed the method blocks until the threads are
    // terminated.
    void Shutdown();

    cc::TaskGraphRunner* GetTaskGraphRunner()
    {
        return &task_graph_runner_;
    }

    // Create a new sequenced task graph runner.
    scoped_refptr<base::SequencedTaskRunner> CreateSequencedTaskRunner();

protected:
    ~RasterWorkerPool() override;

private:
    class RasterWorkerPoolSequencedTaskRunner;
    friend class RasterWorkerPoolSequencedTaskRunner;

    // Simple Task for the TaskGraphRunner that wraps a closure.
    // This class is used to schedule TaskRunner tasks on the
    // |task_graph_runner_|.
    class ClosureTask : public cc::Task {
    public:
        explicit ClosureTask(const base::Closure& closure);

        // Overridden from cc::Task:
        void RunOnWorkerThread() override;

    protected:
        ~ClosureTask() override;

    private:
        base::Closure closure_;

        DISALLOW_COPY_AND_ASSIGN(ClosureTask);
    };

    // The actual threads where work is done.
    ScopedVector<base::DelegateSimpleThread> threads_;
    cc::TaskGraphRunner task_graph_runner_;

    // Lock to exclusively access all the following members that are used to
    // implement the TaskRunner interfaces.
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

} // namespace content

#endif // CONTENT_RENDERER_RASTER_WORKER_POOL_H_
