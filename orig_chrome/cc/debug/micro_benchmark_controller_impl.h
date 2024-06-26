// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_MICRO_BENCHMARK_CONTROLLER_IMPL_H_
#define CC_DEBUG_MICRO_BENCHMARK_CONTROLLER_IMPL_H_

#include <string>

#include "base/basictypes.h"
#include "cc/base/scoped_ptr_vector.h"
#include "cc/debug/micro_benchmark_impl.h"

namespace cc {

class LayerTreeHostImpl;
class CC_EXPORT MicroBenchmarkControllerImpl {
public:
    explicit MicroBenchmarkControllerImpl(LayerTreeHostImpl* host);
    ~MicroBenchmarkControllerImpl();

    void DidCompleteCommit();

    void ScheduleRun(scoped_ptr<MicroBenchmarkImpl> benchmark);

private:
    void CleanUpFinishedBenchmarks();

    LayerTreeHostImpl* host_;
    ScopedPtrVector<MicroBenchmarkImpl> benchmarks_;

    DISALLOW_COPY_AND_ASSIGN(MicroBenchmarkControllerImpl);
};

} // namespace cc

#endif // CC_DEBUG_MICRO_BENCHMARK_CONTROLLER_IMPL_H_
