// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_PICTURE_LAYER_H_
#define CC_LAYERS_PICTURE_LAYER_H_

#include "cc/base/invalidation_region.h"
#include "cc/debug/devtools_instrumentation.h"
#include "cc/debug/micro_benchmark_controller.h"
#include "cc/layers/layer.h"

namespace cc {

class ContentLayerClient;
class RecordingSource;
class ResourceUpdateQueue;

class CC_EXPORT PictureLayer : public Layer {
public:
    static scoped_refptr<PictureLayer> Create(const LayerSettings& settings,
        ContentLayerClient* client);

    void ClearClient();

    void SetNearestNeighbor(bool nearest_neighbor);

    // Layer interface.
    scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;
    void SetLayerTreeHost(LayerTreeHost* host) override;
    void PushPropertiesTo(LayerImpl* layer) override;
    void SetNeedsDisplayRect(const gfx::Rect& layer_rect) override;
    bool Update() override;
    void SetIsMask(bool is_mask) override;
    skia::RefPtr<SkPicture> GetPicture() const override;
    bool IsSuitableForGpuRasterization() const override;

    void RunMicroBenchmark(MicroBenchmark* benchmark) override;

    ContentLayerClient* client() { return client_; }

    RecordingSource* GetRecordingSourceForTesting()
    {
        return recording_source_.get();
    }

protected:
    PictureLayer(const LayerSettings& settings, ContentLayerClient* client);
    // Allow tests to inject a recording source.
    PictureLayer(const LayerSettings& settings,
        ContentLayerClient* client,
        scoped_ptr<RecordingSource> source);
    ~PictureLayer() override;

    bool HasDrawableContent() const override;

    bool is_mask() const { return is_mask_; }

private:
    ContentLayerClient* client_;
    scoped_ptr<RecordingSource> recording_source_;
    devtools_instrumentation::
        ScopedLayerObjectTracker instrumentation_object_tracker_;
    // Invalidation to use the next time update is called.
    InvalidationRegion pending_invalidation_;
    // Invalidation from the last time update was called.
    Region recording_invalidation_;
    gfx::Rect last_updated_visible_layer_rect_;

    int update_source_frame_number_;
    bool is_mask_;
    bool nearest_neighbor_;

    DISALLOW_COPY_AND_ASSIGN(PictureLayer);
};

} // namespace cc

#endif // CC_LAYERS_PICTURE_LAYER_H_
