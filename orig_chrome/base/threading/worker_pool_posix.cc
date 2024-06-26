// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/threading/worker_pool_posix.h"

#include "base/bind.h"
#include "base/callback.h"
#include "base/lazy_instance.h"
#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "base/strings/stringprintf.h"
#include "base/threading/platform_thread.h"
#include "base/threading/thread_local.h"
#include "base/threading/worker_pool.h"
#include "base/trace_event/trace_event.h"
#include "base/tracked_objects.h"

using tracked_objects::TrackedTime;

namespace base {

namespace {

    LazyInstance<ThreadLocalBoolean>::Leaky g_worker_pool_running_on_this_thread = LAZY_INSTANCE_INITIALIZER;

    const int64 kIdleSecondsBeforeExit = 10 * 60;

    class WorkerPoolImpl {
    public:
        WorkerPoolImpl();
        ~WorkerPoolImpl();

        void PostTask(const tracked_objects::Location& from_here,
            const Closure& task,
            bool task_is_slow);

        void ShutDownCleanly();

    private:
        scoped_refptr<PosixDynamicThreadPool> pool_;
    };

    WorkerPoolImpl::WorkerPoolImpl()
        : pool_(new PosixDynamicThreadPool(
            "WorkerPool",
            TimeDelta::FromSeconds(kIdleSecondsBeforeExit)))
    {
    }

    WorkerPoolImpl::~WorkerPoolImpl()
    {
        pool_->Terminate(false);
    }

    void WorkerPoolImpl::PostTask(const tracked_objects::Location& from_here,
        const Closure& task,
        bool task_is_slow)
    {
        pool_->PostTask(from_here, task);
    }

    void WorkerPoolImpl::ShutDownCleanly()
    {
        pool_->Terminate(true);
    }

    LazyInstance<WorkerPoolImpl> g_lazy_worker_pool = LAZY_INSTANCE_INITIALIZER;

    class WorkerThread : public PlatformThread::Delegate {
    public:
        WorkerThread(const std::string& name_prefix, PosixDynamicThreadPool* pool)
            : name_prefix_(name_prefix)
            , pool_(pool)
        {
        }

        void ThreadMain() override;

    private:
        const std::string name_prefix_;
        scoped_refptr<PosixDynamicThreadPool> pool_;

        DISALLOW_COPY_AND_ASSIGN(WorkerThread);
    };

    void WorkerThread::ThreadMain()
    {
        g_worker_pool_running_on_this_thread.Get().Set(true);
        const std::string name = StringPrintf("%s/%d", name_prefix_.c_str(), PlatformThread::CurrentId());
        // Note |name.c_str()| must remain valid for for the whole life of the thread.
        PlatformThread::SetName(name);

        for (;;) {
            PendingTask pending_task = pool_->WaitForTask();
            if (pending_task.task.is_null())
                break;
            TRACE_EVENT2("toplevel", "WorkerThread::ThreadMain::Run",
                "src_file", pending_task.posted_from.file_name(),
                "src_func", pending_task.posted_from.function_name());

            tracked_objects::TaskStopwatch stopwatch;
            stopwatch.Start();
            pending_task.task.Run();
            stopwatch.Stop();

            tracked_objects::ThreadData::TallyRunOnWorkerThreadIfTracking(
                pending_task.birth_tally, pending_task.time_posted, stopwatch);
        }

        pool_->NotifyWorkerIsGoingAway(PlatformThread::CurrentHandle());
        delete this;
    }

} // namespace

// static
bool WorkerPool::PostTask(const tracked_objects::Location& from_here,
    const Closure& task,
    bool task_is_slow)
{
    g_lazy_worker_pool.Pointer()->PostTask(from_here, task, task_is_slow);
    return true;
}

// static
bool WorkerPool::RunsTasksOnCurrentThread()
{
    return g_worker_pool_running_on_this_thread.Get().Get();
}

// static
void WorkerPool::ShutDownCleanly()
{
    g_lazy_worker_pool.Pointer()->ShutDownCleanly();
}

PosixDynamicThreadPool::PosixDynamicThreadPool(const std::string& name_prefix,
    TimeDelta idle_time_before_exit)
    : name_prefix_(name_prefix)
    , idle_time_before_exit_(idle_time_before_exit)
    , pending_tasks_available_cv_(&lock_)
    , num_idle_threads_(0)
    , has_pending_cleanup_task_(false)
    , terminated_(false)
{
}

PosixDynamicThreadPool::~PosixDynamicThreadPool()
{
    while (!pending_tasks_.empty())
        pending_tasks_.pop();
}

void PosixDynamicThreadPool::Terminate(bool blocking)
{
    std::vector<PlatformThreadHandle> threads_to_cleanup;
    std::vector<PlatformThreadHandle> worker_threads;
    {
        AutoLock locked(lock_);
        if (terminated_)
            return;
        terminated_ = true;

        threads_to_cleanup.swap(threads_to_cleanup_);
        worker_threads.swap(worker_threads_);
    }
    pending_tasks_available_cv_.Broadcast();

    if (blocking) {
        for (const auto& item : threads_to_cleanup)
            PlatformThread::Join(item);

        for (const auto& item : worker_threads)
            PlatformThread::Join(item);

        // No need to take the lock. No one else should be accessing these members.
        DCHECK_EQ(0u, num_idle_threads_);
        // The following members should not have new elements added after
        // |terminated_| is set to true.
        DCHECK(threads_to_cleanup_.empty());
        DCHECK(worker_threads_.empty());
    }
}

void PosixDynamicThreadPool::PostTask(
    const tracked_objects::Location& from_here,
    const Closure& task)
{
    PendingTask pending_task(from_here, task);
    AutoLock locked(lock_);
    AddTaskNoLock(&pending_task);
}

PendingTask PosixDynamicThreadPool::WaitForTask()
{
    AutoLock locked(lock_);

    if (terminated_)
        return PendingTask(FROM_HERE, Closure());

    if (pending_tasks_.empty()) { // No work available, wait for work.
        num_idle_threads_++;
        if (num_threads_cv_)
            num_threads_cv_->Broadcast();
        pending_tasks_available_cv_.TimedWait(idle_time_before_exit_);
        num_idle_threads_--;
        if (num_threads_cv_)
            num_threads_cv_->Broadcast();
        if (pending_tasks_.empty()) {
            // We waited for work, but there's still no work.  Return an empty task to
            // signal the thread to terminate.
            return PendingTask(FROM_HERE, Closure());
        }
    }

    PendingTask pending_task = pending_tasks_.front();
    pending_tasks_.pop();
    return pending_task;
}

void PosixDynamicThreadPool::NotifyWorkerIsGoingAway(
    PlatformThreadHandle worker)
{
    AutoLock locked(lock_);
    if (terminated_)
        return;

    auto new_end = std::remove_if(worker_threads_.begin(), worker_threads_.end(),
        [worker](PlatformThreadHandle handle) {
            return handle.is_equal(worker);
        });
    DCHECK_EQ(1, worker_threads_.end() - new_end);
    worker_threads_.erase(new_end, worker_threads_.end());

    threads_to_cleanup_.push_back(worker);

    if (num_threads_cv_)
        num_threads_cv_->Broadcast();

    if (!has_pending_cleanup_task_) {
        has_pending_cleanup_task_ = true;
        PendingTask pending_task(
            FROM_HERE,
            base::Bind(&PosixDynamicThreadPool::CleanUpThreads, Unretained(this)));
        AddTaskNoLock(&pending_task);
    }
}

void PosixDynamicThreadPool::AddTaskNoLock(PendingTask* pending_task)
{
    lock_.AssertAcquired();

    if (terminated_) {
        LOG(WARNING)
            << "This thread pool is already terminated.  Do not post new tasks.";
        return;
    }

    pending_tasks_.push(*pending_task);
    pending_task->task.Reset();

    // We have enough worker threads.
    if (num_idle_threads_ >= pending_tasks_.size() - (has_pending_cleanup_task_ ? 1 : 0)) {
        pending_tasks_available_cv_.Signal();
    } else {
        // The new PlatformThread will take ownership of the WorkerThread object,
        // which will delete itself on exit.
        WorkerThread* worker = new WorkerThread(name_prefix_, this);
        PlatformThreadHandle handle;
        PlatformThread::Create(0, worker, &handle);
        worker_threads_.push_back(handle);

        if (num_threads_cv_)
            num_threads_cv_->Broadcast();
    }
}

void PosixDynamicThreadPool::CleanUpThreads()
{
    std::vector<PlatformThreadHandle> threads_to_cleanup;
    {
        AutoLock locked(lock_);
        DCHECK(has_pending_cleanup_task_);
        has_pending_cleanup_task_ = false;
        threads_to_cleanup.swap(threads_to_cleanup_);
    }
    for (const auto& item : threads_to_cleanup)
        PlatformThread::Join(item);
}

} // namespace base
