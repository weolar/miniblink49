// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/input/scroll_elasticity_helper.h"

#include "cc/layers/layer_impl.h"
#include "cc/trees/layer_tree_host_impl.h"
#include "cc/trees/layer_tree_impl.h"

namespace cc {

class ScrollElasticityHelperImpl : public ScrollElasticityHelper {
public:
    explicit ScrollElasticityHelperImpl(LayerTreeHostImpl* layer_tree_host_impl);
    ~ScrollElasticityHelperImpl() override;

    bool IsUserScrollable() const override;
    gfx::Vector2dF StretchAmount() const override;
    void SetStretchAmount(const gfx::Vector2dF& stretch_amount) override;
    gfx::ScrollOffset ScrollOffset() const override;
    gfx::ScrollOffset MaxScrollOffset() const override;
    void ScrollBy(const gfx::Vector2dF& delta) override;
    void RequestAnimate() override;

private:
    LayerTreeHostImpl* layer_tree_host_impl_;
};

ScrollElasticityHelperImpl::ScrollElasticityHelperImpl(
    LayerTreeHostImpl* layer_tree)
    : layer_tree_host_impl_(layer_tree)
{
}

ScrollElasticityHelperImpl::~ScrollElasticityHelperImpl()
{
}

bool ScrollElasticityHelperImpl::IsUserScrollable() const
{
    LayerImpl* layer = layer_tree_host_impl_->OuterViewportScrollLayer();
    if (!layer)
        return false;
    return layer->user_scrollable_horizontal() || layer->user_scrollable_vertical();
}

gfx::Vector2dF ScrollElasticityHelperImpl::StretchAmount() const
{
    return layer_tree_host_impl_->active_tree()->elastic_overscroll()->Current(
        true);
}

void ScrollElasticityHelperImpl::SetStretchAmount(
    const gfx::Vector2dF& stretch_amount)
{
    if (stretch_amount == StretchAmount())
        return;

    layer_tree_host_impl_->active_tree()->elastic_overscroll()->SetCurrent(
        stretch_amount);
    layer_tree_host_impl_->active_tree()->set_needs_update_draw_properties();
    layer_tree_host_impl_->SetNeedsCommit();
    layer_tree_host_impl_->SetNeedsRedraw();
    layer_tree_host_impl_->SetFullRootLayerDamage();
}

gfx::ScrollOffset ScrollElasticityHelperImpl::ScrollOffset() const
{
    return layer_tree_host_impl_->active_tree()->TotalScrollOffset();
}

gfx::ScrollOffset ScrollElasticityHelperImpl::MaxScrollOffset() const
{
    return layer_tree_host_impl_->active_tree()->TotalMaxScrollOffset();
}

void ScrollElasticityHelperImpl::ScrollBy(const gfx::Vector2dF& delta)
{
    LayerImpl* root_scroll_layer = layer_tree_host_impl_->OuterViewportScrollLayer()
        ? layer_tree_host_impl_->OuterViewportScrollLayer()
        : layer_tree_host_impl_->InnerViewportScrollLayer();
    if (root_scroll_layer)
        root_scroll_layer->ScrollBy(delta);
}

void ScrollElasticityHelperImpl::RequestAnimate()
{
    layer_tree_host_impl_->SetNeedsAnimate();
}

// static
ScrollElasticityHelper* ScrollElasticityHelper::CreateForLayerTreeHostImpl(
    LayerTreeHostImpl* layer_tree_host_impl)
{
    return new ScrollElasticityHelperImpl(layer_tree_host_impl);
}

} // namespace cc
