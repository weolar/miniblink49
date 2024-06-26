// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/debug/micro_benchmark_controller_impl.h"

#include <string>

#include "base/callback.h"
#include "base/values.h"
#include "cc/trees/layer_tree_host_impl.h"

namespace cc {

namespace {

    class IsDonePredicate {
    public:
        typedef const MicroBenchmarkImpl* argument_type;
        typedef bool result_type;

        result_type operator()(argument_type benchmark) const
        {
            return benchmark->IsDone();
        }
    };

} // namespace

MicroBenchmarkControllerImpl::MicroBenchmarkControllerImpl(
    LayerTreeHostImpl* host)
    : host_(host)
{
    DCHECK(host_);
}

MicroBenchmarkControllerImpl::~MicroBenchmarkControllerImpl() { }

void MicroBenchmarkControllerImpl::ScheduleRun(
    scoped_ptr<MicroBenchmarkImpl> benchmark)
{
    benchmarks_.push_back(benchmark.Pass());
}

void MicroBenchmarkControllerImpl::DidCompleteCommit()
{
    for (ScopedPtrVector<MicroBenchmarkImpl>::iterator it = benchmarks_.begin();
         it != benchmarks_.end();
         ++it) {
        DCHECK(!(*it)->IsDone());
        (*it)->DidCompleteCommit(host_);
    }

    CleanUpFinishedBenchmarks();
}

void MicroBenchmarkControllerImpl::CleanUpFinishedBenchmarks()
{
    benchmarks_.erase(
        benchmarks_.partition(std::not1(IsDonePredicate())),
        benchmarks_.end());
}

} // namespace cc
