// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_LAYERS_HEADS_UP_DISPLAY_LAYER_H_
#define CC_LAYERS_HEADS_UP_DISPLAY_LAYER_H_

#include <string>

#include "base/memory/scoped_ptr.h"
#include "cc/base/cc_export.h"
#include "cc/layers/layer.h"

namespace cc {

class CC_EXPORT HeadsUpDisplayLayer : public Layer {
public:
    static scoped_refptr<HeadsUpDisplayLayer> Create(
        const LayerSettings& settings);

    void PrepareForCalculateDrawProperties(
        const gfx::Size& device_viewport, float device_scale_factor);

    scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;

protected:
    explicit HeadsUpDisplayLayer(const LayerSettings& settings);
    bool HasDrawableContent() const override;

private:
    ~HeadsUpDisplayLayer() override;

    DISALLOW_COPY_AND_ASSIGN(HeadsUpDisplayLayer);

#ifndef NOT_QB_AERO
public:
    // Set position inset of hud layer
    void SetTopInset(int inset);

private:
    // When qb aero effect enabled the position of hud layer has to move down as
    // mush as |top_inset_|
    int top_inset_ = 0;
#endif // NOT_QB_AERO
};

} // namespace cc

#endif // CC_LAYERS_HEADS_UP_DISPLAY_LAYER_H_
