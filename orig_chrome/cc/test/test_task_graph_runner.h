// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TEST_TEST_TASK_GRAPH_RUNNER_H_
#define CC_TEST_TEST_TASK_GRAPH_RUNNER_H_

#include "base/threading/simple_thread.h"
#include "cc/raster/task_graph_runner.h"

namespace cc {

class TestTaskGraphRunner : public TaskGraphRunner,
                            public base::DelegateSimpleThread::Delegate {
public:
    TestTaskGraphRunner();
    ~TestTaskGraphRunner() override;

    // Overridden from base::DelegateSimpleThread::Delegate:
    void Run() override;

private:
    base::DelegateSimpleThread worker_thread_;

    DISALLOW_COPY_AND_ASSIGN(TestTaskGraphRunner);
};

} // namespace cc

#endif // CC_TEST_TEST_TASK_GRAPH_RUNNER_H_
