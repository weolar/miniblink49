// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_MICRO_BENCHMARK_H_
#define CC_DEBUG_MICRO_BENCHMARK_H_

#include "base/callback.h"
#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"

namespace base {
class SingleThreadTaskRunner;
class Value;
} // namespace base

namespace cc {

class LayerTreeHost;
class Layer;
class PictureLayer;
class MicroBenchmarkImpl;
class CC_EXPORT MicroBenchmark {
public:
    typedef base::Callback<void(scoped_ptr<base::Value>)> DoneCallback;

    explicit MicroBenchmark(const DoneCallback& callback);
    virtual ~MicroBenchmark();

    bool IsDone() const;
    virtual void DidUpdateLayers(LayerTreeHost* host);
    int id() const { return id_; }
    void set_id(int id) { id_ = id; }

    virtual void RunOnLayer(Layer* layer);
    virtual void RunOnLayer(PictureLayer* layer);

    virtual bool ProcessMessage(scoped_ptr<base::Value> value);

    bool ProcessedForBenchmarkImpl() const;
    scoped_ptr<MicroBenchmarkImpl> GetBenchmarkImpl(
        scoped_refptr<base::SingleThreadTaskRunner> origin_task_runner);

protected:
    void NotifyDone(scoped_ptr<base::Value> result);

    virtual scoped_ptr<MicroBenchmarkImpl> CreateBenchmarkImpl(
        scoped_refptr<base::SingleThreadTaskRunner> origin_task_runner);

private:
    DoneCallback callback_;
    bool is_done_;
    bool processed_for_benchmark_impl_;
    int id_;
};

} // namespace cc

#endif // CC_DEBUG_MICRO_BENCHMARK_H_
