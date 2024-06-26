// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/debug/rasterize_and_record_benchmark.h"

#include <algorithm>
#include <limits>
#include <string>

#include "base/basictypes.h"
#include "base/strings/stringprintf.h"
#include "base/values.h"
#include "cc/debug/lap_timer.h"
#include "cc/debug/rasterize_and_record_benchmark_impl.h"
#include "cc/layers/content_layer_client.h"
#include "cc/layers/layer.h"
#include "cc/layers/picture_layer.h"
#include "cc/playback/display_item_list.h"
#include "cc/trees/layer_tree_host.h"
#include "cc/trees/layer_tree_host_common.h"
#include "skia/ext/analysis_canvas.h"
#include "third_party/skia/include/utils/SkPictureUtils.h"
#include "ui/gfx/geometry/rect.h"

namespace cc {

namespace {

    const int kDefaultRecordRepeatCount = 100;

    // Parameters for LapTimer.
    const int kTimeLimitMillis = 1;
    const int kWarmupRuns = 0;
    const int kTimeCheckInterval = 1;

    const char* kModeSuffixes[RecordingSource::RECORDING_MODE_COUNT] = {
        "",
        "_sk_null_canvas",
        "_painting_disabled",
        "_caching_disabled",
        "_construction_disabled"
    };

} // namespace

RasterizeAndRecordBenchmark::RasterizeAndRecordBenchmark(
    scoped_ptr<base::Value> value,
    const MicroBenchmark::DoneCallback& callback)
    : MicroBenchmark(callback)
    , record_repeat_count_(kDefaultRecordRepeatCount)
    , settings_(value.Pass())
    , main_thread_benchmark_done_(false)
    , host_(nullptr)
    , weak_ptr_factory_(this)
{
    base::DictionaryValue* settings = nullptr;
    settings_->GetAsDictionary(&settings);
    if (!settings)
        return;

    if (settings->HasKey("record_repeat_count"))
        settings->GetInteger("record_repeat_count", &record_repeat_count_);
}

RasterizeAndRecordBenchmark::~RasterizeAndRecordBenchmark()
{
    weak_ptr_factory_.InvalidateWeakPtrs();
}

void RasterizeAndRecordBenchmark::DidUpdateLayers(LayerTreeHost* host)
{
    host_ = host;
    LayerTreeHostCommon::CallFunctionForSubtree(
        host->root_layer(),
        [this](Layer* layer) { layer->RunMicroBenchmark(this); });

    DCHECK(!results_.get());
    results_ = make_scoped_ptr(new base::DictionaryValue);
    results_->SetInteger("pixels_recorded", record_results_.pixels_recorded);
    results_->SetInteger("picture_memory_usage",
        static_cast<int>(record_results_.bytes_used));

    for (int i = 0; i < RecordingSource::RECORDING_MODE_COUNT; i++) {
        std::string name = base::StringPrintf("record_time%s_ms", kModeSuffixes[i]);
        results_->SetDouble(name,
            record_results_.total_best_time[i].InMillisecondsF());
    }
    main_thread_benchmark_done_ = true;
}

void RasterizeAndRecordBenchmark::RecordRasterResults(
    scoped_ptr<base::Value> results_value)
{
    DCHECK(main_thread_benchmark_done_);

    base::DictionaryValue* results = nullptr;
    results_value->GetAsDictionary(&results);
    DCHECK(results);

    results_->MergeDictionary(results);

    NotifyDone(results_.Pass());
}

scoped_ptr<MicroBenchmarkImpl> RasterizeAndRecordBenchmark::CreateBenchmarkImpl(
    scoped_refptr<base::SingleThreadTaskRunner> origin_task_runner)
{
    return make_scoped_ptr(new RasterizeAndRecordBenchmarkImpl(
        origin_task_runner, settings_.get(),
        base::Bind(&RasterizeAndRecordBenchmark::RecordRasterResults,
            weak_ptr_factory_.GetWeakPtr())));
}

void RasterizeAndRecordBenchmark::RunOnLayer(PictureLayer* layer)
{
    DCHECK(host_);

    gfx::Rect visible_layer_rect = layer->visible_layer_rect();
    if (visible_layer_rect.IsEmpty())
        return;

    RunOnDisplayListLayer(layer, visible_layer_rect);
}

void RasterizeAndRecordBenchmark::RunOnDisplayListLayer(
    PictureLayer* layer,
    const gfx::Rect& visible_layer_rect)
{
    ContentLayerClient* painter = layer->client();

    for (int mode_index = 0; mode_index < RecordingSource::RECORDING_MODE_COUNT;
         mode_index++) {
        ContentLayerClient::PaintingControlSetting painting_control = ContentLayerClient::PAINTING_BEHAVIOR_NORMAL;
        switch (static_cast<RecordingSource::RecordingMode>(mode_index)) {
        case RecordingSource::RECORD_NORMALLY:
            // Already setup for normal recording.
            break;
        case RecordingSource::RECORD_WITH_SK_NULL_CANVAS:
            // Not supported for Display List recording.
            continue;
        case RecordingSource::RECORD_WITH_PAINTING_DISABLED:
            painting_control = ContentLayerClient::DISPLAY_LIST_PAINTING_DISABLED;
            break;
        case RecordingSource::RECORD_WITH_CACHING_DISABLED:
            painting_control = ContentLayerClient::DISPLAY_LIST_CACHING_DISABLED;
            break;
        case RecordingSource::RECORD_WITH_CONSTRUCTION_DISABLED:
            painting_control = ContentLayerClient::DISPLAY_LIST_CONSTRUCTION_DISABLED;
            break;
        default:
            NOTREACHED();
        }
        base::TimeDelta min_time = base::TimeDelta::Max();
        size_t memory_used = 0;

        scoped_refptr<DisplayItemList> display_list;
        for (int i = 0; i < record_repeat_count_; ++i) {
            // Run for a minimum amount of time to avoid problems with timer
            // quantization when the layer is very small.
            LapTimer timer(kWarmupRuns,
                base::TimeDelta::FromMilliseconds(kTimeLimitMillis),
                kTimeCheckInterval);

            do {
                display_list = painter->PaintContentsToDisplayList(visible_layer_rect,
                    painting_control);
                if (display_list->ShouldBeAnalyzedForSolidColor()) {
                    gfx::Size layer_size = layer->paint_properties().bounds;
                    skia::AnalysisCanvas canvas(layer_size.width(), layer_size.height());
                    display_list->Raster(&canvas, nullptr, gfx::Rect(), 1.f);
                }

                if (memory_used) {
                    // Verify we are recording the same thing each time.
                    DCHECK_EQ(memory_used, display_list->ApproximateMemoryUsage());
                } else {
                    memory_used = display_list->ApproximateMemoryUsage();
                }

                timer.NextLap();
            } while (!timer.HasTimeLimitExpired());
            base::TimeDelta duration = base::TimeDelta::FromMillisecondsD(timer.MsPerLap());
            if (duration < min_time)
                min_time = duration;
        }

        if (mode_index == RecordingSource::RECORD_NORMALLY) {
            record_results_.bytes_used += memory_used + painter->GetApproximateUnsharedMemoryUsage();
            record_results_.pixels_recorded += visible_layer_rect.width() * visible_layer_rect.height();
        }
        record_results_.total_best_time[mode_index] += min_time;
    }
}

RasterizeAndRecordBenchmark::RecordResults::RecordResults()
    : pixels_recorded(0)
    , bytes_used(0)
{
}

RasterizeAndRecordBenchmark::RecordResults::~RecordResults() { }

} // namespace cc
