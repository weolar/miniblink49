// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_MICRO_BENCHMARK_IMPL_H_
#define CC_DEBUG_MICRO_BENCHMARK_IMPL_H_

#include "base/callback.h"
#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"

namespace base {
class SingleThreadTaskRunner;
class Value;
} // namespace base

namespace cc {

class LayerTreeHostImpl;
class LayerImpl;
class PictureLayerImpl;
class CC_EXPORT MicroBenchmarkImpl {
public:
    typedef base::Callback<void(scoped_ptr<base::Value>)> DoneCallback;

    explicit MicroBenchmarkImpl(
        const DoneCallback& callback,
        scoped_refptr<base::SingleThreadTaskRunner> origin_task_runner);
    virtual ~MicroBenchmarkImpl();

    bool IsDone() const;
    virtual void DidCompleteCommit(LayerTreeHostImpl* host);

    virtual void RunOnLayer(LayerImpl* layer);
    virtual void RunOnLayer(PictureLayerImpl* layer);

protected:
    void NotifyDone(scoped_ptr<base::Value> result);

private:
    DoneCallback callback_;
    bool is_done_;
    scoped_refptr<base::SingleThreadTaskRunner> origin_task_runner_;
};

} // namespace cc

#endif // CC_DEBUG_MICRO_BENCHMARK_IMPL_H_
