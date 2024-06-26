// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/debug/unittest_only_benchmark.h"

#include "base/bind.h"
#include "base/single_thread_task_runner.h"
#include "base/values.h"
#include "cc/debug/unittest_only_benchmark_impl.h"

namespace cc {

UnittestOnlyBenchmark::UnittestOnlyBenchmark(scoped_ptr<base::Value> value,
    const DoneCallback& callback)
    : MicroBenchmark(callback)
    , create_impl_benchmark_(false)
    , weak_ptr_factory_(this)
{
    if (!value)
        return;

    base::DictionaryValue* settings = nullptr;
    value->GetAsDictionary(&settings);
    if (!settings)
        return;

    if (settings->HasKey("run_benchmark_impl"))
        settings->GetBoolean("run_benchmark_impl", &create_impl_benchmark_);
}

UnittestOnlyBenchmark::~UnittestOnlyBenchmark()
{
    weak_ptr_factory_.InvalidateWeakPtrs();
}

void UnittestOnlyBenchmark::DidUpdateLayers(LayerTreeHost* host)
{
    NotifyDone(nullptr);
}

bool UnittestOnlyBenchmark::ProcessMessage(scoped_ptr<base::Value> value)
{
    base::DictionaryValue* message = nullptr;
    value->GetAsDictionary(&message);
    bool can_handle;
    if (message->HasKey("can_handle")) {
        message->GetBoolean("can_handle", &can_handle);
        if (can_handle)
            return true;
    }
    return false;
}

void UnittestOnlyBenchmark::RecordImplResults(scoped_ptr<base::Value> results)
{
    NotifyDone(results.Pass());
}

scoped_ptr<MicroBenchmarkImpl> UnittestOnlyBenchmark::CreateBenchmarkImpl(
    scoped_refptr<base::SingleThreadTaskRunner> origin_task_runner)
{
    if (!create_impl_benchmark_)
        return make_scoped_ptr<MicroBenchmarkImpl>(nullptr);

    return make_scoped_ptr(new UnittestOnlyBenchmarkImpl(
        origin_task_runner, nullptr,
        base::Bind(&UnittestOnlyBenchmark::RecordImplResults,
            weak_ptr_factory_.GetWeakPtr())));
}

} // namespace cc
