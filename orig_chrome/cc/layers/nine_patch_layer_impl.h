// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_NINE_PATCH_LAYER_IMPL_H_
#define CC_LAYERS_NINE_PATCH_LAYER_IMPL_H_

#include <string>

#include "cc/base/cc_export.h"
#include "cc/layers/layer_impl.h"
#include "cc/layers/ui_resource_layer_impl.h"
#include "cc/resources/resource_provider.h"
#include "cc/resources/ui_resource_client.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

namespace base {
class DictionaryValue;
}

namespace cc {

class CC_EXPORT NinePatchLayerImpl : public UIResourceLayerImpl {
public:
    static scoped_ptr<NinePatchLayerImpl> Create(LayerTreeImpl* tree_impl,
        int id)
    {
        return make_scoped_ptr(new NinePatchLayerImpl(tree_impl, id));
    }
    ~NinePatchLayerImpl() override;

    // The bitmap stretches out the bounds of the layer.  The following picture
    // illustrates the parameters associated with the dimensions.
    //
    // Layer space layout              Bitmap space layout
    //
    // ------------------------       ~~~~~~~~~~ W ~~~~~~~~~~
    // |          :           |       :     :                |
    // |          C           |       :     Y                |
    // |          :           |       :     :                |
    // |     ------------     |       :~~X~~------------     |
    // |     |          |     |       :     |          :     |
    // |     |          |     |       :     |          :     |
    // |~~A~~|          |~~B~~|       H     |          Q     |
    // |     |          |     |       :     |          :     |
    // |     ------------     |       :     ~~~~~P~~~~~      |
    // |          :           |       :                      |
    // |          D           |       :                      |
    // |          :           |       :                      |
    // ------------------------       ------------------------
    //
    // |image_bounds| = (W, H)
    // |image_aperture| = (X, Y, P, Q)
    // |border| = (A, C, A + B, C + D)
    // |fill_center| indicates whether to draw the center quad or not.
    void SetLayout(const gfx::Rect& image_aperture,
        const gfx::Rect& border,
        bool fill_center);

    scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;
    void PushPropertiesTo(LayerImpl* layer) override;

    void AppendQuads(RenderPass* render_pass,
        AppendQuadsData* append_quads_data) override;

    base::DictionaryValue* LayerTreeAsJson() const override;

protected:
    NinePatchLayerImpl(LayerTreeImpl* tree_impl, int id);

private:
    const char* LayerTypeAsString() const override;

    void CheckGeometryLimitations();

    // The transparent center region that shows the parent layer's contents in
    // image space.
    gfx::Rect image_aperture_;

    // An inset border that the patches will be mapped to.
    gfx::Rect border_;

    bool fill_center_;

    DISALLOW_COPY_AND_ASSIGN(NinePatchLayerImpl);
};

} // namespace cc

#endif // CC_LAYERS_NINE_PATCH_LAYER_IMPL_H_
