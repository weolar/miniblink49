// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/blink/WebCompositorAnimationTimelineImpl.h"

#include "mc/animation/AnimationIdProvider.h"
#include "mc/animation/AnimationPlayer.h"
#include "mc/animation/AnimationTimeline.h"
#include "mc/blink/WebCompositorAnimationPlayerImpl.h"
#include "third_party/WebKit/public/platform/WebCompositorAnimationPlayerClient.h"

using mc::AnimationTimeline;

namespace mc_blink {

WebCompositorAnimationTimelineImpl::WebCompositorAnimationTimelineImpl()
    //: m_animationTimeline(AnimationTimeline::create(mc::AnimationIdProvider::nextTimelineId())) 
{
    m_host = nullptr;
}

WebCompositorAnimationTimelineImpl::~WebCompositorAnimationTimelineImpl() 
{
}

mc::AnimationTimeline* WebCompositorAnimationTimelineImpl::getAnimationTimeline() const
{
    //return m_animationTimeline.get();
    DebugBreak();
    return nullptr;
}

void WebCompositorAnimationTimelineImpl::playerAttached(const blink::WebCompositorAnimationPlayerClient& client) 
{
    if (client.compositorPlayer()) {
        //m_animationTimeline->attachPlayer(static_cast<WebCompositorAnimationPlayerImpl*>(client.compositorPlayer())->getAnimationPlayer());
        WebCompositorAnimationPlayerImpl* player = static_cast<WebCompositorAnimationPlayerImpl*>(client.compositorPlayer());
        player->setHost(m_host);
    }
}

void WebCompositorAnimationTimelineImpl::playerDestroyed(const blink::WebCompositorAnimationPlayerClient& client)
{
    if (client.compositorPlayer()) {
        //m_animationTimeline->detachPlayer(static_cast<WebCompositorAnimationPlayerImpl*>(client.compositorPlayer())->getAnimationPlayer());
        WebCompositorAnimationPlayerImpl* player = static_cast<WebCompositorAnimationPlayerImpl*>(client.compositorPlayer());
        player->setHost(nullptr);
    }
}

}  // namespace cc_blink
