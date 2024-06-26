// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/viewport.h"

#include "base/logging.h"
#include "cc/input/top_controls_manager.h"
#include "cc/trees/layer_tree_host_impl.h"
#include "cc/trees/layer_tree_impl.h"
#include "ui/gfx/geometry/vector2d_conversions.h"
#include "ui/gfx/geometry/vector2d_f.h"

namespace cc {

// static
scoped_ptr<Viewport> Viewport::Create(
    LayerTreeHostImpl* host_impl)
{
    return make_scoped_ptr(new Viewport(host_impl));
}

Viewport::Viewport(LayerTreeHostImpl* host_impl)
    : host_impl_(host_impl)
    , pinch_zoom_active_(false)
{
    DCHECK(host_impl_);
}

void Viewport::Pan(const gfx::Vector2dF& delta)
{
    gfx::Vector2dF pending_delta = delta;
    float page_scale = host_impl_->active_tree()->current_page_scale_factor();
    pending_delta.Scale(1 / page_scale);
    InnerScrollLayer()->ScrollBy(pending_delta);
}

Viewport::ScrollResult Viewport::ScrollBy(const gfx::Vector2dF& delta,
    const gfx::Point& viewport_point,
    bool is_direct_manipulation,
    bool affect_top_controls)
{
    gfx::Vector2dF content_delta = delta;

    if (affect_top_controls && ShouldTopControlsConsumeScroll(delta))
        content_delta -= ScrollTopControls(delta);

    gfx::Vector2dF pending_content_delta = content_delta;

    pending_content_delta -= host_impl_->ScrollLayer(InnerScrollLayer(),
        pending_content_delta,
        viewport_point,
        is_direct_manipulation);

    ScrollResult result;

    pending_content_delta -= host_impl_->ScrollLayer(OuterScrollLayer(),
        pending_content_delta,
        viewport_point,
        is_direct_manipulation);
    result.consumed_delta = delta - AdjustOverscroll(pending_content_delta);

    result.content_scrolled_delta = content_delta - pending_content_delta;
    return result;
}

void Viewport::SnapPinchAnchorIfWithinMargin(const gfx::Point& anchor)
{
    gfx::SizeF viewport_size = gfx::SizeF(
        host_impl_->active_tree()->InnerViewportContainerLayer()->bounds());

    if (anchor.x() < kPinchZoomSnapMarginDips)
        pinch_anchor_adjustment_.set_x(-anchor.x());
    else if (anchor.x() > viewport_size.width() - kPinchZoomSnapMarginDips)
        pinch_anchor_adjustment_.set_x(viewport_size.width() - anchor.x());

    if (anchor.y() < kPinchZoomSnapMarginDips)
        pinch_anchor_adjustment_.set_y(-anchor.y());
    else if (anchor.y() > viewport_size.height() - kPinchZoomSnapMarginDips)
        pinch_anchor_adjustment_.set_y(viewport_size.height() - anchor.y());
}

void Viewport::PinchUpdate(float magnify_delta, const gfx::Point& anchor)
{
    if (!pinch_zoom_active_) {
        // If this is the first pinch update and the pinch is within a margin-
        // length of the screen edge, offset all updates by the amount so that we
        // effectively snap the pinch zoom to the edge of the screen. This makes it
        // easy to zoom in on position: fixed elements.
        SnapPinchAnchorIfWithinMargin(anchor);

        pinch_zoom_active_ = true;
    }

    LayerTreeImpl* active_tree = host_impl_->active_tree();

    // Keep the center-of-pinch anchor specified by (x, y) in a stable
    // position over the course of the magnify.
    gfx::Point adjusted_anchor = anchor + pinch_anchor_adjustment_;
    float page_scale = active_tree->current_page_scale_factor();
    gfx::PointF previous_scale_anchor = gfx::ScalePoint(gfx::PointF(adjusted_anchor), 1.f / page_scale);
    active_tree->SetPageScaleOnActiveTree(page_scale * magnify_delta);
    page_scale = active_tree->current_page_scale_factor();
    gfx::PointF new_scale_anchor = gfx::ScalePoint(gfx::PointF(adjusted_anchor), 1.f / page_scale);
    gfx::Vector2dF move = previous_scale_anchor - new_scale_anchor;

    // Scale back to viewport space since that's the coordinate space ScrollBy
    // uses.
    move.Scale(page_scale);

    // If clamping the inner viewport scroll offset causes a change, it should
    // be accounted for from the intended move.
    move -= InnerScrollLayer()->ClampScrollToMaxScrollOffset();

    Pan(move);
}

void Viewport::PinchEnd()
{
    pinch_anchor_adjustment_ = gfx::Vector2d();
    pinch_zoom_active_ = false;
}

gfx::Vector2dF Viewport::ScrollTopControls(const gfx::Vector2dF& delta)
{
    gfx::Vector2dF excess_delta = host_impl_->top_controls_manager()->ScrollBy(delta);

    return delta - excess_delta;
}

bool Viewport::ShouldTopControlsConsumeScroll(
    const gfx::Vector2dF& scroll_delta) const
{
    // Always consume if it's in the direction to show the top controls.
    if (scroll_delta.y() < 0)
        return true;

    if (TotalScrollOffset().y() < MaxTotalScrollOffset().y())
        return true;

    return false;
}

gfx::Vector2dF Viewport::AdjustOverscroll(const gfx::Vector2dF& delta) const
{
    // TODO(tdresser): Use a more rational epsilon. See crbug.com/510550 for
    // details.
    const float kEpsilon = 0.1f;
    gfx::Vector2dF adjusted = delta;

    if (std::abs(adjusted.x()) < kEpsilon)
        adjusted.set_x(0.0f);
    if (std::abs(adjusted.y()) < kEpsilon)
        adjusted.set_y(0.0f);

    return adjusted;
}

gfx::ScrollOffset Viewport::MaxTotalScrollOffset() const
{
    gfx::ScrollOffset offset;

    offset += InnerScrollLayer()->MaxScrollOffset();

    if (OuterScrollLayer())
        offset += OuterScrollLayer()->MaxScrollOffset();

    return offset;
}

gfx::ScrollOffset Viewport::TotalScrollOffset() const
{
    gfx::ScrollOffset offset;

    offset += InnerScrollLayer()->CurrentScrollOffset();

    if (OuterScrollLayer())
        offset += OuterScrollLayer()->CurrentScrollOffset();

    return offset;
}

LayerImpl* Viewport::InnerScrollLayer() const
{
    return host_impl_->InnerViewportScrollLayer();
}

LayerImpl* Viewport::OuterScrollLayer() const
{
    return host_impl_->OuterViewportScrollLayer();
}

} // namespace cc
