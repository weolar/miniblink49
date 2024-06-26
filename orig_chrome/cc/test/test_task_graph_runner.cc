// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/test_task_graph_runner.h"

namespace cc {

TestTaskGraphRunner::TestTaskGraphRunner()
    : worker_thread_(this, "CompositorTileWorker1")
{
    worker_thread_.Start();
}

TestTaskGraphRunner::~TestTaskGraphRunner()
{
    TaskGraphRunner::Shutdown();
    worker_thread_.Join();
}

void TestTaskGraphRunner::Run()
{
    TaskGraphRunner::Run();
}

} // namespace cc
