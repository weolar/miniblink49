// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/blink/web_compositor_animation_player_impl.h"

#include "cc/animation/animation_id_provider.h"
#include "cc/animation/animation_player.h"
#include "cc/blink/web_animation_impl.h"
#include "cc/blink/web_to_cc_animation_delegate_adapter.h"
#include "third_party/WebKit/public/platform/WebLayer.h"

using cc::AnimationPlayer;

namespace cc_blink {

WebCompositorAnimationPlayerImpl::WebCompositorAnimationPlayerImpl()
    : animation_player_(
        AnimationPlayer::Create(cc::AnimationIdProvider::NextPlayerId()))
{
}

WebCompositorAnimationPlayerImpl::~WebCompositorAnimationPlayerImpl()
{
}

CC_BLINK_EXPORT cc::AnimationPlayer*
WebCompositorAnimationPlayerImpl::animation_player() const
{
    return animation_player_.get();
}

void WebCompositorAnimationPlayerImpl::setAnimationDelegate(
    blink::WebCompositorAnimationDelegate* delegate)
{
    animation_delegate_adapter_.reset(
        new WebToCCAnimationDelegateAdapter(delegate));
    animation_player_->set_layer_animation_delegate(
        animation_delegate_adapter_.get());
}

void WebCompositorAnimationPlayerImpl::attachLayer(blink::WebLayer* web_layer)
{
    animation_player_->AttachLayer(web_layer->id());
}

void WebCompositorAnimationPlayerImpl::detachLayer()
{
    animation_player_->DetachLayer();
}

bool WebCompositorAnimationPlayerImpl::isLayerAttached() const
{
    return animation_player_->layer_id() != 0;
}

void WebCompositorAnimationPlayerImpl::addAnimation(
    blink::WebCompositorAnimation* animation)
{
    animation_player_->AddAnimation(
        static_cast<WebCompositorAnimationImpl*>(animation)->PassAnimation());
    delete animation;
}

void WebCompositorAnimationPlayerImpl::removeAnimation(int animation_id)
{
    animation_player_->RemoveAnimation(animation_id);
}

void WebCompositorAnimationPlayerImpl::pauseAnimation(int animation_id,
    double time_offset)
{
    animation_player_->PauseAnimation(animation_id, time_offset);
}

} // namespace cc_blink
