// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_UNITTEST_ONLY_BENCHMARK_IMPL_H_
#define CC_DEBUG_UNITTEST_ONLY_BENCHMARK_IMPL_H_

#include "base/memory/weak_ptr.h"
#include "cc/debug/micro_benchmark_impl.h"

namespace base {
class SingleThreadTaskRunner;
class Value;
}

namespace cc {

class LayerTreeHostImpl;
class CC_EXPORT UnittestOnlyBenchmarkImpl : public MicroBenchmarkImpl {
public:
    UnittestOnlyBenchmarkImpl(
        scoped_refptr<base::SingleThreadTaskRunner> origin_task_runner,
        base::Value* settings,
        const DoneCallback& callback);
    ~UnittestOnlyBenchmarkImpl() override;

    void DidCompleteCommit(LayerTreeHostImpl* host) override;
};

} // namespace cc

#endif // CC_DEBUG_UNITTEST_ONLY_BENCHMARK_IMPL_H_
