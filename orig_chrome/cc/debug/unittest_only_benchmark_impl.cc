// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/debug/unittest_only_benchmark_impl.h"

#include "base/single_thread_task_runner.h"
#include "base/values.h"

namespace cc {

UnittestOnlyBenchmarkImpl::UnittestOnlyBenchmarkImpl(
    scoped_refptr<base::SingleThreadTaskRunner> origin_task_runner,
    base::Value* settings,
    const DoneCallback& callback)
    : MicroBenchmarkImpl(callback, origin_task_runner)
{
}

UnittestOnlyBenchmarkImpl::~UnittestOnlyBenchmarkImpl() { }

void UnittestOnlyBenchmarkImpl::DidCompleteCommit(LayerTreeHostImpl* host)
{
    NotifyDone(nullptr);
}

} // namespace cc
