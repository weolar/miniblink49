// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_SOLID_COLOR_SCROLLBAR_LAYER_H_
#define CC_LAYERS_SOLID_COLOR_SCROLLBAR_LAYER_H_

#include "cc/base/cc_export.h"
#include "cc/layers/layer.h"
#include "cc/layers/scrollbar_layer_interface.h"

namespace cc {

class CC_EXPORT SolidColorScrollbarLayer : public ScrollbarLayerInterface,
                                           public Layer {
public:
    scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;

    static scoped_refptr<SolidColorScrollbarLayer> Create(
        const LayerSettings& settings,
        ScrollbarOrientation orientation,
        int thumb_thickness,
        int track_start,
        bool is_left_side_vertical_scrollbar,
        int scroll_layer_id);

    // Layer overrides.
    bool OpacityCanAnimateOnImplThread() const override;
    ScrollbarLayerInterface* ToScrollbarLayer() override;

    void PushPropertiesTo(LayerImpl* layer) override;
    void PushScrollClipPropertiesTo(LayerImpl* layer) override;

    void SetNeedsDisplayRect(const gfx::Rect& rect) override;

    // ScrollbarLayerInterface
    int ScrollLayerId() const override;
    void SetScrollLayer(int layer_id) override;
    void SetClipLayer(int layer_id) override;

    ScrollbarOrientation orientation() const override;

protected:
    SolidColorScrollbarLayer(const LayerSettings& settings,
        ScrollbarOrientation orientation,
        int thumb_thickness,
        int track_start,
        bool is_left_side_vertical_scrollbar,
        int scroll_layer_id);
    ~SolidColorScrollbarLayer() override;

private:
    int scroll_layer_id_;
    int clip_layer_id_;
    ScrollbarOrientation orientation_;
    int thumb_thickness_;
    int track_start_;
    bool is_left_side_vertical_scrollbar_;

    DISALLOW_COPY_AND_ASSIGN(SolidColorScrollbarLayer);
};

} // namespace cc

#endif // CC_LAYERS_SOLID_COLOR_SCROLLBAR_LAYER_H_
