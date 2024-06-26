// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_PICTURE_IMAGE_LAYER_IMPL_H_
#define CC_LAYERS_PICTURE_IMAGE_LAYER_IMPL_H_

#include "cc/layers/picture_layer_impl.h"

namespace cc {

class CC_EXPORT PictureImageLayerImpl : public PictureLayerImpl {
public:
    static scoped_ptr<PictureImageLayerImpl> Create(LayerTreeImpl* tree_impl,
        int id,
        bool is_mask)
    {
        return make_scoped_ptr(new PictureImageLayerImpl(tree_impl, id, is_mask));
    }
    ~PictureImageLayerImpl() override;

    // LayerImpl overrides.
    const char* LayerTypeAsString() const override;
    scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;

protected:
    PictureImageLayerImpl(LayerTreeImpl* tree_impl, int id, bool is_mask);

    bool ShouldAdjustRasterScale() const override;
    void RecalculateRasterScales() override;
    void GetDebugBorderProperties(SkColor* color, float* width) const override;

    void UpdateIdealScales() override;

private:
    DISALLOW_COPY_AND_ASSIGN(PictureImageLayerImpl);
};

} // namespace cc

#endif // CC_LAYERS_PICTURE_IMAGE_LAYER_IMPL_H_
