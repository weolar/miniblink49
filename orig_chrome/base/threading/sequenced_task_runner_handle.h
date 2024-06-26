// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_THREADING_SEQUENCED_TASK_RUNNER_HANDLE_H_
#define BASE_THREADING_SEQUENCED_TASK_RUNNER_HANDLE_H_

#include "base/compiler_specific.h"
#include "base/memory/ref_counted.h"

namespace base {

class SequencedTaskRunner;

class BASE_EXPORT SequencedTaskRunnerHandle {
public:
    // Returns a SequencedTaskRunner which guarantees that posted tasks will only
    // run after the current task is finished and will satisfy a SequenceChecker.
    // It should only be called if IsSet() returns true (see the comment there for
    // the requirements).
    static scoped_refptr<SequencedTaskRunner> Get();

    // Returns true if one of the following conditions is fulfilled:
    // a) The current thread has a ThreadTaskRunnerHandle (which includes any
    //    thread that has a MessageLoop associated with it), or
    // b) The current thread is a worker thread belonging to a SequencedWorkerPool
    //    *and* is currently running a sequenced task.
    static bool IsSet();

private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(SequencedTaskRunnerHandle);
};

} // namespace base

#endif // BASE_THREADING_SEQUENCED_TASK_RUNNER_HANDLE_H_
