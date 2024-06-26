// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_BLOCKING_TASK_RUNNER_H_
#define CC_TREES_BLOCKING_TASK_RUNNER_H_

#include <vector>

#include "base/location.h"
#include "base/memory/scoped_ptr.h"
#include "base/single_thread_task_runner.h"
#include "base/synchronization/lock.h"
#include "base/threading/platform_thread.h"
#include "cc/base/cc_export.h"

namespace cc {

// This class wraps a SingleThreadTaskRunner but allows posted tasks to be
// run without a round trip through the message loop. This shortcutting
// removes guarantees about ordering. Tasks posted while the
// BlockingTaskRunner is in a capturing state will run in order, and tasks
// posted while the BlockingTaskRunner is /not/ in a capturing state will
// run in order, but the two sets of tasks will *not* run in order relative
// to when they were posted.
//
// To use this class, post tasks to the task runner returned by
// BlockingTaskRunner::Create(). The thread it is created on identifies the
// thread you want the tasks to run on. The SingleThreadTaskRunner which is
// passed into Create() is used to run tasks that are posted when not in a
// capturing state.
//
// Then, on the thread that the given task runner belongs to, you may
// instantiate a BlockingTaskRunner::CapturePostTasks. While this object
// exists, the task runner will collect any PostTasks called on it, posting
// tasks to that thread from anywhere. This CapturePostTasks object provides
// a window in time where tasks can shortcut past the MessageLoop. As soon
// as the CapturePostTasks object is destroyed (goes out of scope), all
// tasks that had been posted to the thread during the window will be executed
// immediately.
//
// Beware of re-entrancy, make sure the CapturePostTasks object is destroyed at
// a time when it makes sense for the embedder to call arbitrary things.
class CC_EXPORT BlockingTaskRunner {
public:
    // Creates a BlockingTaskRunner for a given SingleThreadTaskRunner.
    // |task_runner| will be used to run the tasks which are posted while we are
    // not capturing. |task_runner| should belong to same the thread on which
    // capturing is done.
    static scoped_ptr<BlockingTaskRunner> Create(
        scoped_refptr<base::SingleThreadTaskRunner> task_runner);

    ~BlockingTaskRunner();

    // While an object of this type is held alive on a thread, any tasks
    // posted to the thread will be captured and run as soon as the object
    // is destroyed, shortcutting past the task runner.
    class CC_EXPORT CapturePostTasks {
    public:
        explicit CapturePostTasks(BlockingTaskRunner* blocking_runner);
        ~CapturePostTasks();

    private:
        BlockingTaskRunner* blocking_runner_;

        DISALLOW_COPY_AND_ASSIGN(CapturePostTasks);
    };

    // True if tasks posted to the BlockingTaskRunner will run on the current
    // thread.
    bool BelongsToCurrentThread();

    // Posts a task using the contained SingleThreadTaskRunner unless |capture_|
    // is true. When |capture_| is true, tasks posted will be caught and stored
    // until the capturing stops. At that time the tasks will be run directly
    // instead of being posted to the SingleThreadTaskRunner.
    bool PostTask(const tracked_objects::Location& from_here,
        const base::Closure& task);

private:
    explicit BlockingTaskRunner(
        scoped_refptr<base::SingleThreadTaskRunner> task_runner);

    void SetCapture(bool capture);

    base::PlatformThreadId thread_id_;
    scoped_refptr<base::SingleThreadTaskRunner> task_runner_;

    base::Lock lock_;
    int capture_;
    std::vector<base::Closure> captured_tasks_;

    DISALLOW_COPY_AND_ASSIGN(BlockingTaskRunner);
};

} // namespace cc

#endif // CC_TREES_BLOCKING_TASK_RUNNER_H_
