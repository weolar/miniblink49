// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_PICTURE_RECORD_BENCHMARK_H_
#define CC_DEBUG_PICTURE_RECORD_BENCHMARK_H_

#include <map>
#include <utility>
#include <vector>

#include "base/time/time.h"
#include "cc/debug/micro_benchmark_controller.h"

namespace cc {

class LayerTreeHost;
class Layer;
class CC_EXPORT PictureRecordBenchmark : public MicroBenchmark {
public:
    explicit PictureRecordBenchmark(scoped_ptr<base::Value> value,
        const MicroBenchmark::DoneCallback& callback);
    ~PictureRecordBenchmark() override;

    // Implements MicroBenchmark interface.
    void DidUpdateLayers(LayerTreeHost* host) override;
    void RunOnLayer(PictureLayer* layer) override;

private:
    typedef std::pair<base::TimeDelta, unsigned> TotalTime;
    std::map<std::pair<int, int>, TotalTime> times_;
    std::vector<std::pair<int, int>> dimensions_;
};

} // namespace cc

#endif // CC_DEBUG_PICTURE_RECORD_BENCHMARK_H_
