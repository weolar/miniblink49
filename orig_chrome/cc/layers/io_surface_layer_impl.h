// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_IO_SURFACE_LAYER_IMPL_H_
#define CC_LAYERS_IO_SURFACE_LAYER_IMPL_H_

#include <string>

#include "cc/base/cc_export.h"
#include "cc/layers/layer_impl.h"
#include "ui/gfx/geometry/size.h"

namespace cc {

class CC_EXPORT IOSurfaceLayerImpl : public LayerImpl {
public:
    static scoped_ptr<IOSurfaceLayerImpl> Create(LayerTreeImpl* tree_impl,
        int id)
    {
        return make_scoped_ptr(new IOSurfaceLayerImpl(tree_impl, id));
    }
    ~IOSurfaceLayerImpl() override;

    void SetIOSurfaceProperties(unsigned io_surface_id, const gfx::Size& size);

    scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;
    void PushPropertiesTo(LayerImpl* layer_tree_impl) override;

    void AppendQuads(RenderPass* render_pass,
        AppendQuadsData* append_quads_data) override;

    bool WillDraw(DrawMode draw_mode,
        ResourceProvider* resource_provider) override;
    void ReleaseResources() override;

private:
    IOSurfaceLayerImpl(LayerTreeImpl* tree_impl, int id);

    void DestroyResource();

    const char* LayerTypeAsString() const override;

    unsigned io_surface_id_;
    gfx::Size io_surface_size_;
    bool io_surface_changed_;
    unsigned io_surface_resource_id_;

    DISALLOW_COPY_AND_ASSIGN(IOSurfaceLayerImpl);
};

} // namespace cc

#endif // CC_LAYERS_IO_SURFACE_LAYER_IMPL_H_
