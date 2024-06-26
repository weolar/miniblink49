// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/debug/micro_benchmark_controller.h"

#include <limits>
#include <string>

#include "base/callback.h"
#include "base/thread_task_runner_handle.h"
#include "base/values.h"
#include "cc/debug/invalidation_benchmark.h"
#include "cc/debug/rasterize_and_record_benchmark.h"
#include "cc/debug/unittest_only_benchmark.h"
#include "cc/trees/layer_tree_host.h"
#include "cc/trees/layer_tree_host_impl.h"

namespace cc {

int MicroBenchmarkController::next_id_ = 1;

namespace {

    scoped_ptr<MicroBenchmark> CreateBenchmark(
        const std::string& name,
        scoped_ptr<base::Value> value,
        const MicroBenchmark::DoneCallback& callback)
    {
        if (name == "invalidation_benchmark") {
            return make_scoped_ptr(new InvalidationBenchmark(value.Pass(), callback));
        } else if (name == "rasterize_and_record_benchmark") {
            return make_scoped_ptr(
                new RasterizeAndRecordBenchmark(value.Pass(), callback));
        } else if (name == "unittest_only_benchmark") {
            return make_scoped_ptr(new UnittestOnlyBenchmark(value.Pass(), callback));
        }
        return nullptr;
    }

    class IsDonePredicate {
    public:
        typedef const MicroBenchmark* argument_type;
        typedef bool result_type;

        result_type operator()(argument_type benchmark) const
        {
            return benchmark->IsDone();
        }
    };

} // namespace

MicroBenchmarkController::MicroBenchmarkController(LayerTreeHost* host)
    : host_(host)
    , main_controller_task_runner_(base::ThreadTaskRunnerHandle::IsSet()
              ? base::ThreadTaskRunnerHandle::Get()
              : nullptr)
{
    DCHECK(host_);
}

MicroBenchmarkController::~MicroBenchmarkController() { }

int MicroBenchmarkController::ScheduleRun(
    const std::string& micro_benchmark_name,
    scoped_ptr<base::Value> value,
    const MicroBenchmark::DoneCallback& callback)
{
    scoped_ptr<MicroBenchmark> benchmark = CreateBenchmark(micro_benchmark_name, value.Pass(), callback);
    if (benchmark.get()) {
        int id = GetNextIdAndIncrement();
        benchmark->set_id(id);
        benchmarks_.push_back(benchmark.Pass());
        host_->SetNeedsCommit();
        return id;
    }
    return 0;
}

int MicroBenchmarkController::GetNextIdAndIncrement()
{
    int id = next_id_++;
    // Wrap around to 1 if we overflow (very unlikely).
    if (next_id_ == std::numeric_limits<int>::max())
        next_id_ = 1;
    return id;
}

bool MicroBenchmarkController::SendMessage(int id,
    scoped_ptr<base::Value> value)
{
    for (ScopedPtrVector<MicroBenchmark>::iterator it = benchmarks_.begin();
         it != benchmarks_.end();
         ++it) {
        if ((*it)->id() == id)
            return (*it)->ProcessMessage(value.Pass());
    }
    return false;
}

void MicroBenchmarkController::ScheduleImplBenchmarks(
    LayerTreeHostImpl* host_impl)
{
    for (ScopedPtrVector<MicroBenchmark>::iterator it = benchmarks_.begin();
         it != benchmarks_.end();
         ++it) {
        scoped_ptr<MicroBenchmarkImpl> benchmark_impl;
        if (!(*it)->ProcessedForBenchmarkImpl()) {
            benchmark_impl = (*it)->GetBenchmarkImpl(main_controller_task_runner_);
        }

        if (benchmark_impl.get())
            host_impl->ScheduleMicroBenchmark(benchmark_impl.Pass());
    }
}

void MicroBenchmarkController::DidUpdateLayers()
{
    for (ScopedPtrVector<MicroBenchmark>::iterator it = benchmarks_.begin();
         it != benchmarks_.end();
         ++it) {
        if (!(*it)->IsDone())
            (*it)->DidUpdateLayers(host_);
    }

    CleanUpFinishedBenchmarks();
}

void MicroBenchmarkController::CleanUpFinishedBenchmarks()
{
    benchmarks_.erase(
        benchmarks_.partition(std::not1(IsDonePredicate())),
        benchmarks_.end());
}

} // namespace cc
