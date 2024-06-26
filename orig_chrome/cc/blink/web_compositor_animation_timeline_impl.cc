// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/blink/web_compositor_animation_timeline_impl.h"

#include "cc/animation/animation_id_provider.h"
#include "cc/animation/animation_player.h"
#include "cc/animation/animation_timeline.h"
#include "cc/blink/web_compositor_animation_player_impl.h"
#include "third_party/WebKit/public/platform/WebCompositorAnimationPlayerClient.h"

using cc::AnimationTimeline;

namespace cc_blink {

WebCompositorAnimationTimelineImpl::WebCompositorAnimationTimelineImpl()
    : animation_timeline_(AnimationTimeline::Create(
        cc::AnimationIdProvider::NextTimelineId()))
{
}

WebCompositorAnimationTimelineImpl::~WebCompositorAnimationTimelineImpl()
{
}

cc::AnimationTimeline* WebCompositorAnimationTimelineImpl::animation_timeline()
    const
{
    return animation_timeline_.get();
}

void WebCompositorAnimationTimelineImpl::playerAttached(
    const blink::WebCompositorAnimationPlayerClient& client)
{
    if (client.compositorPlayer())
        animation_timeline_->AttachPlayer(
            static_cast<WebCompositorAnimationPlayerImpl*>(
                client.compositorPlayer())
                ->animation_player());
}

void WebCompositorAnimationTimelineImpl::playerDestroyed(
    const blink::WebCompositorAnimationPlayerClient& client)
{
    if (client.compositorPlayer())
        animation_timeline_->DetachPlayer(
            static_cast<WebCompositorAnimationPlayerImpl*>(
                client.compositorPlayer())
                ->animation_player());
}

} // namespace cc_blink
