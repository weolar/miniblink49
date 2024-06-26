// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// The thread pool used in the POSIX implementation of WorkerPool dynamically
// adds threads as necessary to handle all tasks.  It keeps old threads around
// for a period of time to allow them to be reused.  After this waiting period,
// the threads exit.  Unless blocking termination is requested, worker threads
// are not joined during process shutdown.  This means that potentially long
// running tasks (such as DNS lookup) do not block process shutdown, but also
// means that process shutdown may "leak" objects.  Note that although
// PosixDynamicThreadPool spawns the worker threads and manages the task queue,
// it does not own the worker threads.  The worker threads ask the
// PosixDynamicThreadPool for work and eventually clean themselves up.  The
// worker threads all maintain scoped_refptrs to the PosixDynamicThreadPool
// instance, which prevents PosixDynamicThreadPool from disappearing before all
// worker threads exit.  The owner of PosixDynamicThreadPool should likewise
// maintain a scoped_refptr to the PosixDynamicThreadPool instance.
//
// NOTE: The classes defined in this file are only meant for use by the POSIX
// implementation of WorkerPool.  No one else should be using these classes.
// These symbols are exported in a header purely for testing purposes.

#ifndef BASE_THREADING_WORKER_POOL_POSIX_H_
#define BASE_THREADING_WORKER_POOL_POSIX_H_

#include <queue>
#include <string>
#include <vector>

#include "base/basictypes.h"
#include "base/callback_forward.h"
#include "base/location.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/pending_task.h"
#include "base/synchronization/condition_variable.h"
#include "base/synchronization/lock.h"
#include "base/threading/platform_thread.h"
#include "base/time/time.h"
#include "base/tracked_objects.h"

class Task;

namespace base {

class BASE_EXPORT PosixDynamicThreadPool
    : public RefCountedThreadSafe<PosixDynamicThreadPool> {
public:
    class PosixDynamicThreadPoolPeer;

    // All worker threads will share the same |name_prefix|.  They will exit after
    // |idle_time_before_exit|.
    PosixDynamicThreadPool(const std::string& name_prefix,
        TimeDelta idle_time_before_exit);

    // Indicates that the thread pool is going away.  Stops handing out tasks to
    // worker threads.  Wakes up all the idle threads to let them exit.  If
    // |blocking| is set to true, the call returns after all worker threads have
    // quit.
    // The second and subsequent calls to this method are ignored, regardless of
    // the value of |blocking|.
    void Terminate(bool blocking);

    // Adds |task| to the thread pool.
    void PostTask(const tracked_objects::Location& from_here,
        const Closure& task);

    // Worker thread method to wait for up to |idle_time_before_exit| for more
    // work from the thread pool.  Returns an empty task if no work is available.
    PendingTask WaitForTask();

    // Marks |worker| as dead and enqueues a cleanup task to join dead worker
    // threads. Unlike tasks enqueued by PostTask(), cleanup tasks never cause new
    // worker threads to be created.
    void NotifyWorkerIsGoingAway(PlatformThreadHandle worker);

private:
    friend class RefCountedThreadSafe<PosixDynamicThreadPool>;

    ~PosixDynamicThreadPool();

    // Adds pending_task to the thread pool.  This function will clear
    // |pending_task->task|.
    void AddTaskNoLock(PendingTask* pending_task);

    void CleanUpThreads();

    const std::string name_prefix_;
    const TimeDelta idle_time_before_exit_;

    Lock lock_; // Protects all the variables below.

    // Signal()s worker threads to let them know more tasks are available.
    // Also used for Broadcast()'ing to worker threads to let them know the pool
    // is being deleted and they can exit.
    ConditionVariable pending_tasks_available_cv_;
    size_t num_idle_threads_;
    bool has_pending_cleanup_task_;
    std::queue<PendingTask> pending_tasks_;
    bool terminated_;

    std::vector<PlatformThreadHandle> threads_to_cleanup_;
    std::vector<PlatformThreadHandle> worker_threads_;

    // Signaled when idle thread count or living thread count is changed. Please
    // note that it won't be signaled when Terminate() is called.
    //
    // Only used for tests to ensure correct thread ordering. It will always be
    // NULL in non-test code.
    scoped_ptr<ConditionVariable> num_threads_cv_;

    DISALLOW_COPY_AND_ASSIGN(PosixDynamicThreadPool);
};

} // namespace base

#endif // BASE_THREADING_WORKER_POOL_POSIX_H_
