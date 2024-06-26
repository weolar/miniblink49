// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_PAINTED_SCROLLBAR_LAYER_H_
#define CC_LAYERS_PAINTED_SCROLLBAR_LAYER_H_

#include "cc/base/cc_export.h"
#include "cc/input/scrollbar.h"
#include "cc/layers/layer.h"
#include "cc/layers/scrollbar_layer_interface.h"
#include "cc/layers/scrollbar_theme_painter.h"
#include "cc/resources/scoped_ui_resource.h"

namespace cc {
class ScrollbarThemeComposite;

class CC_EXPORT PaintedScrollbarLayer : public ScrollbarLayerInterface,
                                        public Layer {
public:
    scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;

    static scoped_refptr<PaintedScrollbarLayer> Create(
        const LayerSettings& settings,
        scoped_ptr<Scrollbar> scrollbar,
        int scroll_layer_id);

    bool OpacityCanAnimateOnImplThread() const override;
    ScrollbarLayerInterface* ToScrollbarLayer() override;

    // ScrollbarLayerInterface
    int ScrollLayerId() const override;
    void SetScrollLayer(int layer_id) override;
    void SetClipLayer(int layer_id) override;

    ScrollbarOrientation orientation() const override;

    // Layer interface
    bool Update() override;
    void SetLayerTreeHost(LayerTreeHost* host) override;
    void PushPropertiesTo(LayerImpl* layer) override;
    void PushScrollClipPropertiesTo(LayerImpl* layer) override;

    const gfx::Size& internal_content_bounds() const
    {
        return internal_content_bounds_;
    }

protected:
    PaintedScrollbarLayer(const LayerSettings& settings,
        scoped_ptr<Scrollbar> scrollbar,
        int scroll_layer_id);
    ~PaintedScrollbarLayer() override;

    // For unit tests
    UIResourceId track_resource_id()
    {
        return track_resource_.get() ? track_resource_->id() : 0;
    }
    UIResourceId thumb_resource_id()
    {
        return thumb_resource_.get() ? thumb_resource_->id() : 0;
    }
    void UpdateInternalContentScale();
    void UpdateThumbAndTrackGeometry();

private:
    gfx::Rect ScrollbarLayerRectToContentRect(const gfx::Rect& layer_rect) const;
    gfx::Rect OriginThumbRect() const;

    template <typename T>
    bool UpdateProperty(T value, T* prop)
    {
        if (*prop == value)
            return false;
        *prop = value;
        SetNeedsPushProperties();
        return true;
    }

    int MaxTextureSize();
    float ClampScaleToMaxTextureSize(float scale);

    UIResourceBitmap RasterizeScrollbarPart(const gfx::Rect& layer_rect,
        const gfx::Rect& content_rect,
        ScrollbarPart part);

    scoped_ptr<Scrollbar> scrollbar_;
    int scroll_layer_id_;
    int clip_layer_id_;

    float internal_contents_scale_;
    gfx::Size internal_content_bounds_;

    // Snapshot of properties taken in UpdateThumbAndTrackGeometry and used in
    // PushPropertiesTo.
    int thumb_thickness_;
    int thumb_length_;
    gfx::Point location_;
    gfx::Rect track_rect_;
    bool is_overlay_;
    bool has_thumb_;

    scoped_ptr<ScopedUIResource> track_resource_;
    scoped_ptr<ScopedUIResource> thumb_resource_;

    DISALLOW_COPY_AND_ASSIGN(PaintedScrollbarLayer);
};

} // namespace cc

#endif // CC_LAYERS_PAINTED_SCROLLBAR_LAYER_H_
