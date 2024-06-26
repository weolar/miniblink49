// Copyright 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_TREES_LAYER_TREE_HOST_COMMON_H_
#define CC_TREES_LAYER_TREE_HOST_COMMON_H_

#include <limits>
#include <vector>

#include "base/bind.h"
#include "base/memory/ref_counted.h"
#include "cc/base/cc_export.h"
#include "cc/base/scoped_ptr_vector.h"
#include "cc/layers/layer_lists.h"
#include "cc/trees/property_tree.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/vector2d.h"
#include "ui/gfx/transform.h"

namespace cc {

class LayerImpl;
class Layer;
class SwapPromise;
class PropertyTrees;

class CC_EXPORT LayerTreeHostCommon {
public:
    static gfx::Rect CalculateVisibleRect(const gfx::Rect& target_surface_rect,
        const gfx::Rect& layer_bound_rect,
        const gfx::Transform& transform);

    struct CC_EXPORT CalcDrawPropsMainInputs {
    public:
        CalcDrawPropsMainInputs(Layer* root_layer,
            const gfx::Size& device_viewport_size,
            const gfx::Transform& device_transform,
            float device_scale_factor,
            float page_scale_factor,
            const Layer* page_scale_layer,
            const Layer* inner_viewport_scroll_layer,
            const Layer* outer_viewport_scroll_layer);
        CalcDrawPropsMainInputs(Layer* root_layer,
            const gfx::Size& device_viewport_size,
            const gfx::Transform& device_transform);
        CalcDrawPropsMainInputs(Layer* root_layer,
            const gfx::Size& device_viewport_size);
        Layer* root_layer;
        gfx::Size device_viewport_size;
        gfx::Transform device_transform;
        float device_scale_factor;
        float page_scale_factor;
        const Layer* page_scale_layer;
        const Layer* inner_viewport_scroll_layer;
        const Layer* outer_viewport_scroll_layer;
    };

    struct CC_EXPORT CalcDrawPropsImplInputs {
    public:
        CalcDrawPropsImplInputs(
            LayerImpl* root_layer,
            const gfx::Size& device_viewport_size,
            const gfx::Transform& device_transform,
            float device_scale_factor,
            float page_scale_factor,
            const LayerImpl* page_scale_layer,
            const LayerImpl* inner_viewport_scroll_layer,
            const LayerImpl* outer_viewport_scroll_layer,
            const gfx::Vector2dF& elastic_overscroll,
            const LayerImpl* elastic_overscroll_application_layer,
            int max_texture_size,
            bool can_use_lcd_text,
            bool layers_always_allowed_lcd_text,
            bool can_render_to_separate_surface,
            bool can_adjust_raster_scales,
            bool verify_property_trees,
            LayerImplList* render_surface_layer_list,
            int current_render_surface_layer_list_id,
            PropertyTrees* property_trees);

        LayerImpl* root_layer;
        gfx::Size device_viewport_size;
        gfx::Transform device_transform;
        float device_scale_factor;
        float page_scale_factor;
        const LayerImpl* page_scale_layer;
        const LayerImpl* inner_viewport_scroll_layer;
        const LayerImpl* outer_viewport_scroll_layer;
        gfx::Vector2dF elastic_overscroll;
        const LayerImpl* elastic_overscroll_application_layer;
        int max_texture_size;
        bool can_use_lcd_text;
        bool layers_always_allowed_lcd_text;
        bool can_render_to_separate_surface;
        bool can_adjust_raster_scales;
        bool verify_property_trees;
        LayerImplList* render_surface_layer_list;
        int current_render_surface_layer_list_id;
        PropertyTrees* property_trees;
    };

    struct CC_EXPORT CalcDrawPropsImplInputsForTesting
        : public CalcDrawPropsImplInputs {
        CalcDrawPropsImplInputsForTesting(LayerImpl* root_layer,
            const gfx::Size& device_viewport_size,
            const gfx::Transform& device_transform,
            LayerImplList* render_surface_layer_list);
        CalcDrawPropsImplInputsForTesting(LayerImpl* root_layer,
            const gfx::Size& device_viewport_size,
            LayerImplList* render_surface_layer_list);
    };

    static void UpdateRenderSurfaces(Layer* root_layer,
        bool can_render_to_separate_surface,
        const gfx::Transform& transform,
        bool preserves_2d_axis_alignment);
    static void UpdateRenderSurface(Layer* layer,
        bool can_render_to_separate_surface,
        gfx::Transform* transform,
        bool* animation_preserves_axis_alignment);
    static void CalculateDrawProperties(CalcDrawPropsMainInputs* inputs);
    static void PreCalculateMetaInformation(Layer* root_layer);
    static void PreCalculateMetaInformationForTesting(LayerImpl* root_layer);
    static void PreCalculateMetaInformationForTesting(Layer* root_layer);

    static void CalculateDrawProperties(CalcDrawPropsImplInputs* inputs);
    static void CalculateDrawProperties(
        CalcDrawPropsImplInputsForTesting* inputs);

    template <typename LayerType>
    static bool RenderSurfaceContributesToTarget(LayerType*,
        int target_surface_layer_id);

    template <typename LayerType, typename Function>
    static void CallFunctionForSubtree(LayerType* layer,
        const Function& function);

    // Returns a layer with the given id if one exists in the subtree starting
    // from the given root layer (including mask and replica layers).
    template <typename LayerType>
    static LayerType* FindLayerInSubtree(LayerType* root_layer, int layer_id);

    static Layer* get_layer_as_raw_ptr(const LayerList& layers, size_t index)
    {
        return layers[index].get();
    }

    static LayerImpl* get_layer_as_raw_ptr(const OwnedLayerImplList& layers,
        size_t index)
    {
        return layers[index];
    }

    static LayerImpl* get_layer_as_raw_ptr(const LayerImplList& layers,
        size_t index)
    {
        return layers[index];
    }

    struct ScrollUpdateInfo {
        int layer_id;
        // TODO(miletus): Use ScrollOffset once LayerTreeHost/Blink fully supports
        // franctional scroll offset.
        gfx::Vector2d scroll_delta;
    };
};

struct CC_EXPORT ScrollAndScaleSet {
    ScrollAndScaleSet();
    ~ScrollAndScaleSet();

    std::vector<LayerTreeHostCommon::ScrollUpdateInfo> scrolls;
    float page_scale_delta;
    gfx::Vector2dF elastic_overscroll_delta;
    float top_controls_delta;
    ScopedPtrVector<SwapPromise> swap_promises;
};

template <typename LayerType>
bool LayerTreeHostCommon::RenderSurfaceContributesToTarget(
    LayerType* layer,
    int target_surface_layer_id)
{
    // A layer will either contribute its own content, or its render surface's
    // content, to the target surface. The layer contributes its surface's content
    // when both the following are true:
    //  (1) The layer actually has a render surface and rendering into that
    //      surface, and
    //  (2) The layer's render surface is not the same as the target surface.
    //
    // Otherwise, the layer just contributes itself to the target surface.

    return layer->render_target() == layer && layer->id() != target_surface_layer_id;
}

template <typename LayerType>
LayerType* LayerTreeHostCommon::FindLayerInSubtree(LayerType* root_layer,
    int layer_id)
{
    if (!root_layer)
        return NULL;

    if (root_layer->id() == layer_id)
        return root_layer;

    if (root_layer->mask_layer() && root_layer->mask_layer()->id() == layer_id)
        return root_layer->mask_layer();

    if (root_layer->replica_layer() && root_layer->replica_layer()->id() == layer_id)
        return root_layer->replica_layer();

    for (size_t i = 0; i < root_layer->children().size(); ++i) {
        if (LayerType* found = FindLayerInSubtree(
                get_layer_as_raw_ptr(root_layer->children(), i), layer_id))
            return found;
    }
    return NULL;
}

template <typename LayerType, typename Function>
void LayerTreeHostCommon::CallFunctionForSubtree(LayerType* layer,
    const Function& function)
{
    function(layer);

    if (LayerType* mask_layer = layer->mask_layer())
        function(mask_layer);
    if (LayerType* replica_layer = layer->replica_layer()) {
        function(replica_layer);
        if (LayerType* mask_layer = replica_layer->mask_layer())
            function(mask_layer);
    }

    for (size_t i = 0; i < layer->children().size(); ++i) {
        CallFunctionForSubtree(get_layer_as_raw_ptr(layer->children(), i),
            function);
    }
}

CC_EXPORT PropertyTrees* GetPropertyTrees(Layer* layer);
CC_EXPORT PropertyTrees* GetPropertyTrees(LayerImpl* layer);

} // namespace cc

#endif // CC_TREES_LAYER_TREE_HOST_COMMON_H_
