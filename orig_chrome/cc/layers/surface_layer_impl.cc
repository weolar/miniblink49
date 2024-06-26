// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/surface_layer_impl.h"

#include "base/trace_event/trace_event_argument.h"
#include "cc/debug/debug_colors.h"
#include "cc/quads/solid_color_draw_quad.h"
#include "cc/quads/surface_draw_quad.h"
#include "cc/trees/occlusion.h"

namespace cc {

SurfaceLayerImpl::SurfaceLayerImpl(LayerTreeImpl* tree_impl, int id)
    : LayerImpl(tree_impl, id)
    , surface_scale_(0.f)
{
}

SurfaceLayerImpl::~SurfaceLayerImpl() { }

scoped_ptr<LayerImpl> SurfaceLayerImpl::CreateLayerImpl(
    LayerTreeImpl* tree_impl)
{
    return SurfaceLayerImpl::Create(tree_impl, id());
}

void SurfaceLayerImpl::SetSurfaceId(SurfaceId surface_id)
{
    if (surface_id_ == surface_id)
        return;

    surface_id_ = surface_id;
    NoteLayerPropertyChanged();
}

void SurfaceLayerImpl::SetSurfaceScale(float scale)
{
    if (surface_scale_ == scale)
        return;

    surface_scale_ = scale;
    NoteLayerPropertyChanged();
}

void SurfaceLayerImpl::SetSurfaceSize(const gfx::Size& size)
{
    if (surface_size_ == size)
        return;

    surface_size_ = size;
    NoteLayerPropertyChanged();
}

void SurfaceLayerImpl::PushPropertiesTo(LayerImpl* layer)
{
    LayerImpl::PushPropertiesTo(layer);
    SurfaceLayerImpl* layer_impl = static_cast<SurfaceLayerImpl*>(layer);

    layer_impl->SetSurfaceId(surface_id_);
    layer_impl->SetSurfaceSize(surface_size_);
    layer_impl->SetSurfaceScale(surface_scale_);
}

void SurfaceLayerImpl::AppendQuads(RenderPass* render_pass,
    AppendQuadsData* append_quads_data)
{
    AppendRainbowDebugBorder(render_pass);

    SharedQuadState* shared_quad_state = render_pass->CreateAndAppendSharedQuadState();
    PopulateScaledSharedQuadState(shared_quad_state, surface_scale_);

    if (surface_id_.is_null())
        return;

    gfx::Rect quad_rect(surface_size_);
    gfx::Rect visible_quad_rect = draw_properties().occlusion_in_content_space.GetUnoccludedContentRect(
        quad_rect);
    if (visible_quad_rect.IsEmpty())
        return;
    SurfaceDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<SurfaceDrawQuad>();
    quad->SetNew(shared_quad_state, quad_rect, visible_quad_rect, surface_id_);
    render_pass->referenced_surfaces.push_back(surface_id_);
}

void SurfaceLayerImpl::GetDebugBorderProperties(SkColor* color,
    float* width) const
{
    *color = DebugColors::SurfaceLayerBorderColor();
    *width = DebugColors::SurfaceLayerBorderWidth(layer_tree_impl());
}

void SurfaceLayerImpl::AppendRainbowDebugBorder(RenderPass* render_pass)
{
    if (!ShowDebugBorders())
        return;

    SharedQuadState* shared_quad_state = render_pass->CreateAndAppendSharedQuadState();
    PopulateSharedQuadState(shared_quad_state);

    SkColor color;
    float border_width;
    GetDebugBorderProperties(&color, &border_width);

    SkColor colors[] = {
        0x80ff0000, // Red.
        0x80ffa500, // Orange.
        0x80ffff00, // Yellow.
        0x80008000, // Green.
        0x800000ff, // Blue.
        0x80ee82ee, // Violet.
    };
    const int kNumColors = arraysize(colors);

    const int kStripeWidth = 300;
    const int kStripeHeight = 300;

    for (int i = 0;; ++i) {
        // For horizontal lines.
        int x = kStripeWidth * i;
        int width = std::min(kStripeWidth, bounds().width() - x - 1);

        // For vertical lines.
        int y = kStripeHeight * i;
        int height = std::min(kStripeHeight, bounds().height() - y - 1);

        gfx::Rect top(x, 0, width, border_width);
        gfx::Rect bottom(x, bounds().height() - border_width, width, border_width);
        gfx::Rect left(0, y, border_width, height);
        gfx::Rect right(bounds().width() - border_width, y, border_width, height);

        if (top.IsEmpty() && left.IsEmpty())
            break;

        if (!top.IsEmpty()) {
            bool force_anti_aliasing_off = false;
            SolidColorDrawQuad* top_quad = render_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
            top_quad->SetNew(shared_quad_state, top, top, colors[i % kNumColors],
                force_anti_aliasing_off);

            SolidColorDrawQuad* bottom_quad = render_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
            bottom_quad->SetNew(shared_quad_state, bottom, bottom,
                colors[kNumColors - 1 - (i % kNumColors)],
                force_anti_aliasing_off);

            if (contents_opaque()) {
                // Draws a stripe filling the layer vertically with the same color and
                // width as the horizontal stipes along the layer's top border.
                SolidColorDrawQuad* solid_quad = render_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
                // The inner fill is more transparent then the border.
                static const float kFillOpacity = 0.1f;
                SkColor fill_color = SkColorSetA(
                    colors[i % kNumColors],
                    static_cast<uint8_t>(SkColorGetA(colors[i % kNumColors]) * kFillOpacity));
                gfx::Rect fill_rect(x, 0, width, bounds().height());
                solid_quad->SetNew(shared_quad_state, fill_rect, fill_rect, fill_color,
                    force_anti_aliasing_off);
            }
        }
        if (!left.IsEmpty()) {
            bool force_anti_aliasing_off = false;
            SolidColorDrawQuad* left_quad = render_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
            left_quad->SetNew(shared_quad_state, left, left,
                colors[kNumColors - 1 - (i % kNumColors)],
                force_anti_aliasing_off);

            SolidColorDrawQuad* right_quad = render_pass->CreateAndAppendDrawQuad<SolidColorDrawQuad>();
            right_quad->SetNew(shared_quad_state, right, right,
                colors[i % kNumColors], force_anti_aliasing_off);
        }
    }
}

// void SurfaceLayerImpl::AsValueInto(base::trace_event::TracedValue* dict) const {
//   LayerImpl::AsValueInto(dict);
//   dict->SetInteger("surface_id", surface_id_.id);
// }

const char* SurfaceLayerImpl::LayerTypeAsString() const
{
    return "cc::SurfaceLayerImpl";
}

} // namespace cc
