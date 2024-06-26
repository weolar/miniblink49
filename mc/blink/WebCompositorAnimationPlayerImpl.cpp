// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/blink/WebCompositorAnimationPlayerImpl.h"

#include "mc/blink/WebLayerImpl.h"
#include "mc/trees/LayerTreeHost.h"
#include "mc/animation/AnimationIdProvider.h"
#include "mc/animation/AnimationPlayer.h"
#include "mc/animation/LayerAnimationController.h"
#include "mc/blink/WebAnimationImpl.h"
#include "mc/blink/WebToCcAnimationDelegateAdapter.h"
#include "third_party/WebKit/public/platform/WebLayer.h"
#include "third_party/WebKit/public/platform/WebCompositorAnimationDelegate.h"

using mc::AnimationPlayer;

namespace mc_blink {

WebCompositorAnimationPlayerImpl::WebCompositorAnimationPlayerImpl()
    : m_layerId(0)
    , m_host(nullptr)
    , m_animation(nullptr)
{
}

WebCompositorAnimationPlayerImpl::~WebCompositorAnimationPlayerImpl() 
{
    detachLayer();
}

void WebCompositorAnimationPlayerImpl::setAnimationDelegate(blink::WebCompositorAnimationDelegate* delegate) 
{
    m_animation = delegate;
}

void WebCompositorAnimationPlayerImpl::attachLayer(blink::WebLayer* webLayer)
{
    m_layerId = webLayer->id();

    WebLayerImpl* layer = (WebLayerImpl*)webLayer;
    mc::LayerTreeHost* host = m_host;

    if (!host)
        host = layer->layerTreeHost();

    if (host)
        setHost(host);
}

void WebCompositorAnimationPlayerImpl::detachLayer() 
{
    if (m_host)
        m_host->detachLayer(this);

    m_host = nullptr;
    m_layerId = 0; //TODO
}

bool WebCompositorAnimationPlayerImpl::isLayerAttached() const
{
    return m_layerId != 0;
}

void WebCompositorAnimationPlayerImpl::notifyAnimationStarted(double monotonicTime, int group)
{
    if (m_animation)
        m_animation->notifyAnimationStarted(monotonicTime, group);
}

void WebCompositorAnimationPlayerImpl::notifyAnimationFinished(double monotonicTime, int group)
{
    if (m_animation)
        m_animation->notifyAnimationFinished(monotonicTime, group);
}

void WebCompositorAnimationPlayerImpl::setHost(mc::LayerTreeHost* host)
{
    if (host)
        host->attachLayer(this);
    if (m_host)
        return;
    m_host = host;

    if (!host)
        return;
    bindElementAnimations();
}

void WebCompositorAnimationPlayerImpl::bindElementAnimations()
{
    ASSERT(m_host);

    if (!isLayerAttached())
        return;

    // Pass all accumulated animations to LAC.
    for (mc::ScopedPtrVector<mc::AnimationObj>::iterator it = m_animationsCache.begin(); it != m_animationsCache.end(); ++it)
        m_host->getOrCreateAnimationController(m_layerId)->addAnimation(m_animationsCache.take(it));

    if (!m_animationsCache.empty())
        m_host->setNeedsAnimate();
    m_animationsCache.clear();
}

void WebCompositorAnimationPlayerImpl::addAnimation(blink::WebCompositorAnimation* animation) 
{
    WTF::PassOwnPtr<mc::AnimationObj> aniObj = static_cast<WebCompositorAnimationImpl*>(animation)->passAnimation();

    if (!m_host || !isLayerAttached()) {
        m_animationsCache.push_back(aniObj.leakPtr());
    } else {
        m_host->getOrCreateAnimationController(m_layerId)->addAnimation(aniObj);
        m_host->setNeedsAnimate();
    }

    delete animation;
}

void WebCompositorAnimationPlayerImpl::removeAnimation(int animationId)
{
    mc::ScopedPtrVector<mc::AnimationObj>::iterator animationsToRemove = m_animationsCache.remove_if([animationId](mc::AnimationObj* animation) {
        return animation->id() == animationId; 
    });
    m_animationsCache.erase(animationsToRemove, m_animationsCache.end());

    if (m_host) {
        m_host->getOrCreateAnimationController(m_layerId)->removeAnimation(animationId);
        m_host->setNeedsAnimate();
    }
}

void WebCompositorAnimationPlayerImpl::pauseAnimation(int animationId, double timeOffset)
{
    if (!m_host)
        DebugBreak();

    if (!isLayerAttached())
        return;

    m_host->getOrCreateAnimationController(m_layerId)->pauseAnimation(animationId, base::TimeDelta::FromSecondsD(timeOffset));
    m_host->setNeedsAnimate();
}

}  // namespace mc_blink
