// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/threading/sequenced_task_runner_handle.h"

#include "base/logging.h"
#include "base/sequenced_task_runner.h"
#include "base/thread_task_runner_handle.h"
#include "base/threading/sequenced_worker_pool.h"

namespace base {

// static
scoped_refptr<SequencedTaskRunner> SequencedTaskRunnerHandle::Get()
{
    // If we are on a worker thread for a SequencedBlockingPool that is running a
    // sequenced task, return a SequencedTaskRunner for it.
    scoped_refptr<base::SequencedWorkerPool> pool = SequencedWorkerPool::GetWorkerPoolForCurrentThread();
    if (pool) {
        SequencedWorkerPool::SequenceToken sequence_token = SequencedWorkerPool::GetSequenceTokenForCurrentThread();
        DCHECK(sequence_token.IsValid());
        DCHECK(pool->IsRunningSequenceOnCurrentThread(sequence_token));
        return pool->GetSequencedTaskRunner(sequence_token);
    }

    // Otherwise, return a SingleThreadTaskRunner for the current thread.
    return base::ThreadTaskRunnerHandle::Get();
}

// static
bool SequencedTaskRunnerHandle::IsSet()
{
    return (SequencedWorkerPool::GetWorkerPoolForCurrentThread() && SequencedWorkerPool::GetSequenceTokenForCurrentThread().IsValid()) || base::ThreadTaskRunnerHandle::IsSet();
}

} // namespace base
