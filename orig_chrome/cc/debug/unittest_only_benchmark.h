// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_UNITTEST_ONLY_BENCHMARK_H_
#define CC_DEBUG_UNITTEST_ONLY_BENCHMARK_H_

#include "base/memory/weak_ptr.h"
#include "cc/debug/micro_benchmark.h"

namespace base {
class SingleThreadIdleTaskRunner;
}

namespace cc {

class CC_EXPORT UnittestOnlyBenchmark : public MicroBenchmark {
public:
    UnittestOnlyBenchmark(scoped_ptr<base::Value> value,
        const DoneCallback& callback);
    ~UnittestOnlyBenchmark() override;

    void DidUpdateLayers(LayerTreeHost* host) override;
    bool ProcessMessage(scoped_ptr<base::Value> value) override;

protected:
    scoped_ptr<MicroBenchmarkImpl> CreateBenchmarkImpl(
        scoped_refptr<base::SingleThreadTaskRunner> origin_task_runner) override;

private:
    void RecordImplResults(scoped_ptr<base::Value> results);

    bool create_impl_benchmark_;
    base::WeakPtrFactory<UnittestOnlyBenchmark> weak_ptr_factory_;
};

} // namespace cc

#endif // CC_DEBUG_UNITTEST_ONLY_BENCHMARK_H_
