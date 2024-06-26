// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_TASK_RUNNER_PROVIDER_H_
#define CC_TREES_TASK_RUNNER_PROVIDER_H_

#include <string>

#include "base/basictypes.h"
#include "base/logging.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/threading/platform_thread.h"
#include "base/time/time.h"
#include "base/values.h"
#include "cc/base/cc_export.h"

namespace base {
namespace trace_event {
    class TracedValue;
}
class SingleThreadTaskRunner;
}

namespace cc {
class BlockingTaskRunner;

// Class responsible for controlling access to the main and impl task runners.
// Useful for assertion checks.
class CC_EXPORT TaskRunnerProvider {
public:
    base::SingleThreadTaskRunner* MainThreadTaskRunner() const;
    bool HasImplThread() const;
    base::SingleThreadTaskRunner* ImplThreadTaskRunner() const;

    // Debug hooks.
    bool IsMainThread() const;
    bool IsImplThread() const;
    bool IsMainThreadBlocked() const;
#if DCHECK_IS_ON()
    void SetMainThreadBlocked(bool is_main_thread_blocked);
    void SetCurrentThreadIsImplThread(bool is_impl_thread);
#endif

    virtual ~TaskRunnerProvider();

    BlockingTaskRunner* blocking_main_thread_task_runner() const
    {
        return blocking_main_thread_task_runner_.get();
    }

    TaskRunnerProvider(
        scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
        scoped_refptr<base::SingleThreadTaskRunner> impl_task_runner);

protected:
    friend class DebugScopedSetImplThread;
    friend class DebugScopedSetMainThread;
    friend class DebugScopedSetMainThreadBlocked;

private:
    scoped_refptr<base::SingleThreadTaskRunner> main_task_runner_;
    scoped_refptr<base::SingleThreadTaskRunner> impl_task_runner_;
    scoped_ptr<BlockingTaskRunner> blocking_main_thread_task_runner_;

#if DCHECK_IS_ON()
    const base::PlatformThreadId main_thread_id_;
    bool impl_thread_is_overridden_;
    bool is_main_thread_blocked_;
#endif

    DISALLOW_COPY_AND_ASSIGN(TaskRunnerProvider);
};

#if DCHECK_IS_ON()
class DebugScopedSetMainThreadBlocked {
public:
    explicit DebugScopedSetMainThreadBlocked(
        TaskRunnerProvider* task_runner_provider)
        : task_runner_provider_(task_runner_provider)
    {
        DCHECK(!task_runner_provider_->IsMainThreadBlocked());
        task_runner_provider_->SetMainThreadBlocked(true);
    }
    ~DebugScopedSetMainThreadBlocked()
    {
        DCHECK(task_runner_provider_->IsMainThreadBlocked());
        task_runner_provider_->SetMainThreadBlocked(false);
    }

private:
    TaskRunnerProvider* task_runner_provider_;
    DISALLOW_COPY_AND_ASSIGN(DebugScopedSetMainThreadBlocked);
};
#else
class DebugScopedSetMainThreadBlocked {
public:
    explicit DebugScopedSetMainThreadBlocked(
        TaskRunnerProvider* task_runner_provider) { }
    ~DebugScopedSetMainThreadBlocked() { }

private:
    DISALLOW_COPY_AND_ASSIGN(DebugScopedSetMainThreadBlocked);
};
#endif

} // namespace cc

#endif // CC_TREES_TASK_RUNNER_PROVIDER_H_
