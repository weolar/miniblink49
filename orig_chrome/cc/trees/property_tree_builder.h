// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_PROPERTY_TREE_BUILDER_H_
#define CC_TREES_PROPERTY_TREE_BUILDER_H_

#include <vector>

#include "cc/trees/layer_tree_host_common.h"
#include "cc/trees/property_tree.h"

namespace cc {

class LayerTreeHost;

class PropertyTreeBuilder {
public:
    static void BuildPropertyTrees(Layer* root_layer,
        const Layer* page_scale_layer,
        const Layer* inner_viewport_scroll_layer,
        const Layer* outer_viewport_scroll_layer,
        float page_scale_factor,
        float device_scale_factor,
        const gfx::Rect& viewport,
        const gfx::Transform& device_transform,
        PropertyTrees* property_trees);
    static void BuildPropertyTrees(LayerImpl* root_layer,
        const LayerImpl* page_scale_layer,
        const LayerImpl* inner_viewport_scroll_layer,
        const LayerImpl* outer_viewport_scroll_layer,
        float page_scale_factor,
        float device_scale_factor,
        const gfx::Rect& viewport,
        const gfx::Transform& device_transform,
        PropertyTrees* property_trees);
};

} // namespace cc

#endif // CC_TREES_PROPERTY_TREE_BUILDER_H_
