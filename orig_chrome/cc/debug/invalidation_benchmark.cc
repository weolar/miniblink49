// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/debug/invalidation_benchmark.h"

#include <algorithm>
#include <limits>

#include "base/basictypes.h"
#include "base/rand_util.h"
#include "base/values.h"
#include "cc/layers/layer.h"
#include "cc/layers/picture_layer.h"
#include "cc/trees/layer_tree_host.h"
#include "cc/trees/layer_tree_host_common.h"
#include "ui/gfx/geometry/rect.h"

namespace cc {

namespace {

    const char* kDefaultInvalidationMode = "viewport";

} // namespace

InvalidationBenchmark::InvalidationBenchmark(
    scoped_ptr<base::Value> value,
    const MicroBenchmark::DoneCallback& callback)
    : MicroBenchmark(callback)
    , seed_(0)
{
    base::DictionaryValue* settings = nullptr;
    value->GetAsDictionary(&settings);
    if (!settings)
        return;

    std::string mode_string = kDefaultInvalidationMode;

    if (settings->HasKey("mode"))
        settings->GetString("mode", &mode_string);

    if (mode_string == "fixed_size") {
        mode_ = FIXED_SIZE;
        CHECK(settings->HasKey("width"))
            << "Must provide a width for fixed_size mode.";
        CHECK(settings->HasKey("height"))
            << "Must provide a height for fixed_size mode.";
        settings->GetInteger("width", &width_);
        settings->GetInteger("height", &height_);
    } else if (mode_string == "layer") {
        mode_ = LAYER;
    } else if (mode_string == "random") {
        mode_ = RANDOM;
    } else if (mode_string == "viewport") {
        mode_ = VIEWPORT;
    } else {
        CHECK(false) << "Invalid mode: " << mode_string
                     << ". One of {fixed_size, layer, viewport, random} expected.";
    }
}

InvalidationBenchmark::~InvalidationBenchmark()
{
}

void InvalidationBenchmark::DidUpdateLayers(LayerTreeHost* host)
{
    LayerTreeHostCommon::CallFunctionForSubtree(
        host->root_layer(),
        [this](Layer* layer) { layer->RunMicroBenchmark(this); });
}

void InvalidationBenchmark::RunOnLayer(PictureLayer* layer)
{
    switch (mode_) {
    case FIXED_SIZE: {
        // Invalidation with a random position and fixed size.
        gfx::Rect visible_layer_rect = layer->visible_layer_rect();
        int x = LCGRandom() * (visible_layer_rect.width() - width_);
        int y = LCGRandom() * (visible_layer_rect.height() - height_);
        gfx::Rect invalidation_rect(x, y, width_, height_);
        layer->SetNeedsDisplayRect(invalidation_rect);
        break;
    }
    case LAYER: {
        // Invalidate entire layer.
        layer->SetNeedsDisplay();
        break;
    }
    case RANDOM: {
        // Random invalidation inside the viewport.
        gfx::Rect visible_layer_rect = layer->visible_layer_rect();
        int x_min = LCGRandom() * visible_layer_rect.width();
        int x_max = LCGRandom() * visible_layer_rect.width();
        int y_min = LCGRandom() * visible_layer_rect.height();
        int y_max = LCGRandom() * visible_layer_rect.height();
        if (x_min > x_max)
            std::swap(x_min, x_max);
        if (y_min > y_max)
            std::swap(y_min, y_max);
        gfx::Rect invalidation_rect(x_min, y_min, x_max - x_min, y_max - y_min);
        layer->SetNeedsDisplayRect(invalidation_rect);
        break;
    }
    case VIEWPORT: {
        // Invalidate entire viewport.
        layer->SetNeedsDisplayRect(layer->visible_layer_rect());
        break;
    }
    }
}

bool InvalidationBenchmark::ProcessMessage(scoped_ptr<base::Value> value)
{
    base::DictionaryValue* message = nullptr;
    value->GetAsDictionary(&message);
    if (!message)
        return false;

    bool notify_done;
    if (message->HasKey("notify_done")) {
        message->GetBoolean("notify_done", &notify_done);
        if (notify_done)
            NotifyDone(base::Value::CreateNullValue());
        return true;
    }
    return false;
}

// A simple linear congruential generator. The random numbers don't need to be
// high quality, but they need to be identical in each run. Therefore, we use a
// LCG and keep the state locally in the benchmark.
float InvalidationBenchmark::LCGRandom()
{
    const uint32 a = 1664525;
    const uint32 c = 1013904223;
    seed_ = a * seed_ + c;
    return static_cast<float>(seed_) / std::numeric_limits<uint32>::max();
}

} // namespace cc
