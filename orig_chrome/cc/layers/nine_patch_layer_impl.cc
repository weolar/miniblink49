// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/nine_patch_layer_impl.h"

#include "base/strings/stringprintf.h"
#include "base/values.h"
#include "cc/base/math_util.h"
#include "cc/quads/texture_draw_quad.h"
#include "cc/trees/layer_tree_impl.h"
#include "cc/trees/occlusion.h"
#include "ui/gfx/geometry/rect_f.h"

namespace cc {

NinePatchLayerImpl::NinePatchLayerImpl(LayerTreeImpl* tree_impl, int id)
    : UIResourceLayerImpl(tree_impl, id)
    , fill_center_(false)
{
}

NinePatchLayerImpl::~NinePatchLayerImpl() { }

scoped_ptr<LayerImpl> NinePatchLayerImpl::CreateLayerImpl(
    LayerTreeImpl* tree_impl)
{
    return NinePatchLayerImpl::Create(tree_impl, id());
}

void NinePatchLayerImpl::PushPropertiesTo(LayerImpl* layer)
{
    UIResourceLayerImpl::PushPropertiesTo(layer);
    NinePatchLayerImpl* layer_impl = static_cast<NinePatchLayerImpl*>(layer);

    layer_impl->SetLayout(image_aperture_, border_, fill_center_);
}

static gfx::RectF NormalizedRect(float x,
    float y,
    float width,
    float height,
    float total_width,
    float total_height)
{
    return gfx::RectF(x / total_width,
        y / total_height,
        width / total_width,
        height / total_height);
}

void NinePatchLayerImpl::SetLayout(const gfx::Rect& aperture,
    const gfx::Rect& border,
    bool fill_center)
{
    // This check imposes an ordering on the call sequence.  An UIResource must
    // exist before SetLayout can be called.
    DCHECK(ui_resource_id_);

    if (image_aperture_ == aperture && border_ == border && fill_center_ == fill_center)
        return;

    image_aperture_ = aperture;
    border_ = border;
    fill_center_ = fill_center;

    NoteLayerPropertyChanged();
}

void NinePatchLayerImpl::CheckGeometryLimitations()
{
    // |border| is in layer space.  It cannot exceed the bounds of the layer.
    DCHECK_GE(bounds().width(), border_.width());
    DCHECK_GE(bounds().height(), border_.height());

    // Sanity Check on |border|
    DCHECK_LE(border_.x(), border_.width());
    DCHECK_LE(border_.y(), border_.height());
    DCHECK_GE(border_.x(), 0);
    DCHECK_GE(border_.y(), 0);

    // |aperture| is in image space.  It cannot exceed the bounds of the bitmap.
    DCHECK(!image_aperture_.size().IsEmpty());
    DCHECK(gfx::Rect(image_bounds_).Contains(image_aperture_))
        << "image_bounds_ " << gfx::Rect(image_bounds_).ToString()
        << " image_aperture_ " << image_aperture_.ToString();
}

void NinePatchLayerImpl::AppendQuads(
    RenderPass* render_pass,
    AppendQuadsData* append_quads_data)
{
    CheckGeometryLimitations();
    SharedQuadState* shared_quad_state = render_pass->CreateAndAppendSharedQuadState();
    PopulateSharedQuadState(shared_quad_state);

    AppendDebugBorderQuad(render_pass, bounds(), shared_quad_state,
        append_quads_data);

    if (!ui_resource_id_)
        return;

    ResourceId resource = layer_tree_impl()->ResourceIdForUIResource(ui_resource_id_);

    if (!resource)
        return;

    static const bool flipped = false;
    static const bool nearest_neighbor = false;
    static const bool premultiplied_alpha = true;

    DCHECK(!bounds().IsEmpty());

    // NinePatch border widths in layer space.
    int layer_left_width = border_.x();
    int layer_top_height = border_.y();
    int layer_right_width = border_.width() - layer_left_width;
    int layer_bottom_height = border_.height() - layer_top_height;

    int layer_middle_width = bounds().width() - border_.width();
    int layer_middle_height = bounds().height() - border_.height();

    // Patch positions in layer space
    gfx::Rect layer_top_left(0, 0, layer_left_width, layer_top_height);
    gfx::Rect layer_top_right(bounds().width() - layer_right_width,
        0,
        layer_right_width,
        layer_top_height);
    gfx::Rect layer_bottom_left(0,
        bounds().height() - layer_bottom_height,
        layer_left_width,
        layer_bottom_height);
    gfx::Rect layer_bottom_right(layer_top_right.x(),
        layer_bottom_left.y(),
        layer_right_width,
        layer_bottom_height);
    gfx::Rect layer_top(
        layer_top_left.right(), 0, layer_middle_width, layer_top_height);
    gfx::Rect layer_left(
        0, layer_top_left.bottom(), layer_left_width, layer_middle_height);
    gfx::Rect layer_right(layer_top_right.x(),
        layer_top_right.bottom(),
        layer_right_width,
        layer_left.height());
    gfx::Rect layer_bottom(layer_top.x(),
        layer_bottom_left.y(),
        layer_top.width(),
        layer_bottom_height);
    gfx::Rect layer_center(layer_left_width,
        layer_top_height,
        layer_middle_width,
        layer_middle_height);

    // Note the following values are in image (bitmap) space.
    float image_width = image_bounds_.width();
    float image_height = image_bounds_.height();

    int image_aperture_left_width = image_aperture_.x();
    int image_aperture_top_height = image_aperture_.y();
    int image_aperture_right_width = image_width - image_aperture_.right();
    int image_aperture_bottom_height = image_height - image_aperture_.bottom();
    // Patch positions in bitmap UV space (from zero to one)
    gfx::RectF uv_top_left = NormalizedRect(0,
        0,
        image_aperture_left_width,
        image_aperture_top_height,
        image_width,
        image_height);
    gfx::RectF uv_top_right = NormalizedRect(image_width - image_aperture_right_width,
        0,
        image_aperture_right_width,
        image_aperture_top_height,
        image_width,
        image_height);
    gfx::RectF uv_bottom_left = NormalizedRect(0,
        image_height - image_aperture_bottom_height,
        image_aperture_left_width,
        image_aperture_bottom_height,
        image_width,
        image_height);
    gfx::RectF uv_bottom_right = NormalizedRect(image_width - image_aperture_right_width,
        image_height - image_aperture_bottom_height,
        image_aperture_right_width,
        image_aperture_bottom_height,
        image_width,
        image_height);
    gfx::RectF uv_top(
        uv_top_left.right(),
        0,
        (image_width - image_aperture_left_width - image_aperture_right_width) / image_width,
        (image_aperture_top_height) / image_height);
    gfx::RectF uv_left(0,
        uv_top_left.bottom(),
        image_aperture_left_width / image_width,
        (image_height - image_aperture_top_height - image_aperture_bottom_height) / image_height);
    gfx::RectF uv_right(uv_top_right.x(),
        uv_top_right.bottom(),
        image_aperture_right_width / image_width,
        uv_left.height());
    gfx::RectF uv_bottom(uv_top.x(),
        uv_bottom_left.y(),
        uv_top.width(),
        image_aperture_bottom_height / image_height);
    gfx::RectF uv_center(uv_top_left.right(),
        uv_top_left.bottom(),
        uv_top.width(),
        uv_left.height());

    gfx::Rect opaque_rect;
    gfx::Rect visible_rect;
    const float vertex_opacity[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    const bool opaque = layer_tree_impl()->IsUIResourceOpaque(ui_resource_id_);

    visible_rect = draw_properties().occlusion_in_content_space.GetUnoccludedContentRect(
        layer_top_left);
    opaque_rect = opaque ? visible_rect : gfx::Rect();
    if (!visible_rect.IsEmpty()) {
        TextureDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
        quad->SetNew(shared_quad_state,
            layer_top_left,
            opaque_rect,
            visible_rect,
            resource,
            premultiplied_alpha,
            uv_top_left.origin(),
            uv_top_left.bottom_right(),
            SK_ColorTRANSPARENT,
            vertex_opacity,
            flipped,
            nearest_neighbor);
        ValidateQuadResources(quad);
    }

    visible_rect = draw_properties().occlusion_in_content_space.GetUnoccludedContentRect(
        layer_top_right);
    opaque_rect = opaque ? visible_rect : gfx::Rect();
    if (!visible_rect.IsEmpty()) {
        TextureDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
        quad->SetNew(shared_quad_state,
            layer_top_right,
            opaque_rect,
            visible_rect,
            resource,
            premultiplied_alpha,
            uv_top_right.origin(),
            uv_top_right.bottom_right(),
            SK_ColorTRANSPARENT,
            vertex_opacity,
            flipped,
            nearest_neighbor);
        ValidateQuadResources(quad);
    }

    visible_rect = draw_properties().occlusion_in_content_space.GetUnoccludedContentRect(
        layer_bottom_left);
    opaque_rect = opaque ? visible_rect : gfx::Rect();
    if (!visible_rect.IsEmpty()) {
        TextureDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
        quad->SetNew(shared_quad_state,
            layer_bottom_left,
            opaque_rect,
            visible_rect,
            resource,
            premultiplied_alpha,
            uv_bottom_left.origin(),
            uv_bottom_left.bottom_right(),
            SK_ColorTRANSPARENT,
            vertex_opacity,
            flipped,
            nearest_neighbor);
        ValidateQuadResources(quad);
    }

    visible_rect = draw_properties().occlusion_in_content_space.GetUnoccludedContentRect(
        layer_bottom_right);
    opaque_rect = opaque ? visible_rect : gfx::Rect();
    if (!visible_rect.IsEmpty()) {
        TextureDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
        quad->SetNew(shared_quad_state,
            layer_bottom_right,
            opaque_rect,
            visible_rect,
            resource,
            premultiplied_alpha,
            uv_bottom_right.origin(),
            uv_bottom_right.bottom_right(),
            SK_ColorTRANSPARENT,
            vertex_opacity,
            flipped,
            nearest_neighbor);
        ValidateQuadResources(quad);
    }

    visible_rect = draw_properties().occlusion_in_content_space.GetUnoccludedContentRect(
        layer_top);
    opaque_rect = opaque ? visible_rect : gfx::Rect();
    if (!visible_rect.IsEmpty()) {
        TextureDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
        quad->SetNew(shared_quad_state,
            layer_top,
            opaque_rect,
            visible_rect,
            resource,
            premultiplied_alpha,
            uv_top.origin(),
            uv_top.bottom_right(),
            SK_ColorTRANSPARENT,
            vertex_opacity,
            flipped,
            nearest_neighbor);
        ValidateQuadResources(quad);
    }

    visible_rect = draw_properties().occlusion_in_content_space.GetUnoccludedContentRect(
        layer_left);
    opaque_rect = opaque ? visible_rect : gfx::Rect();
    if (!visible_rect.IsEmpty()) {
        TextureDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
        quad->SetNew(shared_quad_state,
            layer_left,
            opaque_rect,
            visible_rect,
            resource,
            premultiplied_alpha,
            uv_left.origin(),
            uv_left.bottom_right(),
            SK_ColorTRANSPARENT,
            vertex_opacity,
            flipped,
            nearest_neighbor);
        ValidateQuadResources(quad);
    }

    visible_rect = draw_properties().occlusion_in_content_space.GetUnoccludedContentRect(
        layer_right);
    opaque_rect = opaque ? visible_rect : gfx::Rect();
    if (!visible_rect.IsEmpty()) {
        TextureDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
        quad->SetNew(shared_quad_state,
            layer_right,
            opaque_rect,
            layer_right,
            resource,
            premultiplied_alpha,
            uv_right.origin(),
            uv_right.bottom_right(),
            SK_ColorTRANSPARENT,
            vertex_opacity,
            flipped,
            nearest_neighbor);
        ValidateQuadResources(quad);
    }

    visible_rect = draw_properties().occlusion_in_content_space.GetUnoccludedContentRect(
        layer_bottom);
    opaque_rect = opaque ? visible_rect : gfx::Rect();
    if (!visible_rect.IsEmpty()) {
        TextureDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
        quad->SetNew(shared_quad_state,
            layer_bottom,
            opaque_rect,
            visible_rect,
            resource,
            premultiplied_alpha,
            uv_bottom.origin(),
            uv_bottom.bottom_right(),
            SK_ColorTRANSPARENT,
            vertex_opacity,
            flipped,
            nearest_neighbor);
        ValidateQuadResources(quad);
    }

    if (fill_center_) {
        visible_rect = draw_properties().occlusion_in_content_space.GetUnoccludedContentRect(
            layer_center);
        opaque_rect = opaque ? visible_rect : gfx::Rect();
        if (!visible_rect.IsEmpty()) {
            TextureDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
            quad->SetNew(shared_quad_state,
                layer_center,
                opaque_rect,
                visible_rect,
                resource,
                premultiplied_alpha,
                uv_center.origin(),
                uv_center.bottom_right(),
                SK_ColorTRANSPARENT,
                vertex_opacity,
                flipped,
                nearest_neighbor);
            ValidateQuadResources(quad);
        }
    }
}

const char* NinePatchLayerImpl::LayerTypeAsString() const
{
    return "cc::NinePatchLayerImpl";
}

base::DictionaryValue* NinePatchLayerImpl::LayerTreeAsJson() const
{
    base::DictionaryValue* result = LayerImpl::LayerTreeAsJson();

    base::ListValue* list = new base::ListValue;
    list->AppendInteger(image_aperture_.origin().x());
    list->AppendInteger(image_aperture_.origin().y());
    list->AppendInteger(image_aperture_.size().width());
    list->AppendInteger(image_aperture_.size().height());
    result->Set("ImageAperture", list);

    list = new base::ListValue;
    list->AppendInteger(image_bounds_.width());
    list->AppendInteger(image_bounds_.height());
    result->Set("ImageBounds", list);

    result->Set("Border", MathUtil::AsValue(border_).release());

    result->SetBoolean("FillCenter", fill_center_);

    return result;
}

} // namespace cc
