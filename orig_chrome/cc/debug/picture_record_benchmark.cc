// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/debug/picture_record_benchmark.h"

#include <algorithm>

#include "base/basictypes.h"
#include "base/values.h"
#include "cc/layers/layer.h"
#include "cc/layers/picture_layer.h"
#include "cc/playback/picture.h"
#include "cc/trees/layer_tree_host.h"
#include "cc/trees/layer_tree_host_common.h"
#include "ui/gfx/geometry/rect.h"

namespace cc {

namespace {

    const int kPositionIncrement = 100;
    const int kTileGridSize = 512;

} // namespace

PictureRecordBenchmark::PictureRecordBenchmark(
    scoped_ptr<base::Value> value,
    const MicroBenchmark::DoneCallback& callback)
    : MicroBenchmark(callback)
{
    if (!value)
        return;

    base::ListValue* list = nullptr;
    value->GetAsList(&list);
    if (!list)
        return;

    for (base::ListValue::iterator it = list->begin(); it != list->end(); ++it) {
        base::DictionaryValue* dictionary = nullptr;
        (*it)->GetAsDictionary(&dictionary);
        if (!dictionary || !dictionary->HasKey("width") || !dictionary->HasKey("height"))
            continue;

        int width, height;
        dictionary->GetInteger("width", &width);
        dictionary->GetInteger("height", &height);

        dimensions_.push_back(std::make_pair(width, height));
    }
}

PictureRecordBenchmark::~PictureRecordBenchmark() { }

void PictureRecordBenchmark::DidUpdateLayers(LayerTreeHost* host)
{
    LayerTreeHostCommon::CallFunctionForSubtree(
        host->root_layer(),
        [this](Layer* layer) { layer->RunMicroBenchmark(this); });

    scoped_ptr<base::ListValue> results(new base::ListValue());
    for (std::map<std::pair<int, int>, TotalTime>::iterator it = times_.begin();
         it != times_.end();
         ++it) {
        std::pair<int, int> dimensions = it->first;
        base::TimeDelta total_time = it->second.first;
        unsigned total_count = it->second.second;

        double average_time = 0.0;
        if (total_count > 0)
            average_time = total_time.InMillisecondsF() / total_count;

        scoped_ptr<base::DictionaryValue> result(new base::DictionaryValue());
        result->SetInteger("width", dimensions.first);
        result->SetInteger("height", dimensions.second);
        result->SetInteger("samples_count", total_count);
        result->SetDouble("time_ms", average_time);

        results->Append(result.release());
    }

    NotifyDone(results.Pass());
}

void PictureRecordBenchmark::RunOnLayer(PictureLayer* layer)
{
    ContentLayerClient* painter = layer->client();
    gfx::Size bounds = layer->bounds();

    gfx::Size tile_grid_size(kTileGridSize, kTileGridSize);

    for (size_t i = 0; i < dimensions_.size(); ++i) {
        std::pair<int, int> dimensions = dimensions_[i];
        int width = dimensions.first;
        int height = dimensions.second;

        int y_limit = std::max(1, bounds.height() - height);
        int x_limit = std::max(1, bounds.width() - width);
        for (int y = 0; y < y_limit; y += kPositionIncrement) {
            for (int x = 0; x < x_limit; x += kPositionIncrement) {
                gfx::Rect rect = gfx::Rect(x, y, width, height);

                base::TimeTicks start = base::TimeTicks::Now();

                scoped_refptr<Picture> picture = Picture::Create(rect, painter, tile_grid_size, false,
                    RecordingSource::RECORD_NORMALLY);

                base::TimeTicks end = base::TimeTicks::Now();
                base::TimeDelta duration = end - start;
                TotalTime& total_time = times_[dimensions];
                total_time.first += duration;
                total_time.second++;
            }
        }
    }
}

} // namespace cc
