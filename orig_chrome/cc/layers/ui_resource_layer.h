// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_UI_RESOURCE_LAYER_H_
#define CC_LAYERS_UI_RESOURCE_LAYER_H_

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/layers/layer.h"
#include "cc/resources/ui_resource_client.h"
#include "ui/gfx/geometry/rect.h"

namespace cc {

class LayerTreeHost;
class ScopedUIResource;

class CC_EXPORT UIResourceLayer : public Layer {
public:
    static scoped_refptr<UIResourceLayer> Create(const LayerSettings& settings);

    void PushPropertiesTo(LayerImpl* layer) override;

    void SetLayerTreeHost(LayerTreeHost* host) override;

    void SetBitmap(const SkBitmap& skbitmap);

    // An alternative way of setting the resource to allow for sharing. If you use
    // this method, you are responsible for updating the ID if the layer moves
    // between compositors.
    void SetUIResourceId(UIResourceId resource_id);

    // Sets a UV transform to be used at draw time. Defaults to (0, 0) and (1, 1).
    void SetUV(const gfx::PointF& top_left, const gfx::PointF& bottom_right);

    // Sets an opacity value per vertex. It will be multiplied by the layer
    // opacity value.
    void SetVertexOpacity(float bottom_left,
        float top_left,
        float top_right,
        float bottom_right);

    class UIResourceHolder {
    public:
        virtual UIResourceId id() = 0;
        virtual ~UIResourceHolder();
    };

protected:
    explicit UIResourceLayer(const LayerSettings& settings);
    ~UIResourceLayer() override;

    bool HasDrawableContent() const override;

    scoped_ptr<UIResourceHolder> ui_resource_holder_;
    SkBitmap bitmap_;

    gfx::PointF uv_top_left_;
    gfx::PointF uv_bottom_right_;
    float vertex_opacity_[4];

private:
    scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;
    void RecreateUIResourceHolder();

    DISALLOW_COPY_AND_ASSIGN(UIResourceLayer);
};

} // namespace cc

#endif // CC_LAYERS_UI_RESOURCE_LAYER_H_
