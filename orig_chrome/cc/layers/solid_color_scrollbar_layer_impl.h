// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_SOLID_COLOR_SCROLLBAR_LAYER_IMPL_H_
#define CC_LAYERS_SOLID_COLOR_SCROLLBAR_LAYER_IMPL_H_

#include "cc/base/cc_export.h"
#include "cc/layers/scrollbar_layer_impl_base.h"

namespace cc {

class CC_EXPORT SolidColorScrollbarLayerImpl : public ScrollbarLayerImplBase {
public:
    static scoped_ptr<SolidColorScrollbarLayerImpl> Create(
        LayerTreeImpl* tree_impl,
        int id,
        ScrollbarOrientation orientation,
        int thumb_thickness,
        int track_start,
        bool is_left_side_vertical_scrollbar,
        bool is_overlay);
    ~SolidColorScrollbarLayerImpl() override;

    // LayerImpl overrides.
    scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;
    void PushPropertiesTo(LayerImpl* layer) override;

    void AppendQuads(RenderPass* render_pass,
        AppendQuadsData* append_quads_data) override;

protected:
    SolidColorScrollbarLayerImpl(LayerTreeImpl* tree_impl,
        int id,
        ScrollbarOrientation orientation,
        int thumb_thickness,
        int track_start,
        bool is_left_side_vertical_scrollbar,
        bool is_overlay);

    // ScrollbarLayerImplBase implementation.
    int ThumbThickness() const override;
    int ThumbLength() const override;
    float TrackLength() const override;
    int TrackStart() const override;
    bool IsThumbResizable() const override;

private:
    const char* LayerTypeAsString() const override;

    int thumb_thickness_;
    int track_start_;
    SkColor color_;
};

} // namespace cc

#endif // CC_LAYERS_SOLID_COLOR_SCROLLBAR_LAYER_IMPL_H_
