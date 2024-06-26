// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/animation/ElementAnimations.h"
#include "mc/animation/AnimationHost.h"
#include "mc/animation/AnimationPlayer.h"
#include "mc/animation/AnimationRegistrar.h"
#include "mc/animation/LayerAnimationValueObserver.h"
#include "mc/animation/MutatorHostClient.h"

namespace mc {

class ElementAnimations::ValueObserver : public LayerAnimationValueObserver {
public:
    ValueObserver(ElementAnimations* elementAnimation, LayerTreeType treeType)
        : m_elementAnimations(elementAnimation), m_treeType(treeType) {
        ASSERT(m_elementAnimations);
    }

    // LayerAnimationValueObserver implementation.
    void onFilterAnimated(const FilterOperations& filters) override 
    {
        m_elementAnimations->setFilterMutated(m_treeType, filters);
    }

    void onOpacityAnimated(float opacity) override
    {
        m_elementAnimations->setOpacityMutated(m_treeType, opacity);
    }

    void onTransformAnimated(const SkMatrix44& transform) override
    {
        m_elementAnimations->setTransformMutated(m_treeType, transform);
    }

    void onScrollOffsetAnimated(const blink::FloatPoint& scrollOffset) override 
    {
        m_elementAnimations->setScrollOffsetMutated(m_treeType, scrollOffset);
    }

    void onAnimationWaitingForDeletion() override 
    {
        // TODO(loyso): See Layer::OnAnimationWaitingForDeletion. But we always do
        // PushProperties for AnimationTimelines for now.
    }

    void onTransformIsPotentiallyAnimatingChanged(bool isAnimating) override 
    {
        m_elementAnimations->setTransformIsPotentiallyAnimatingChanged(m_treeType, isAnimating);
    }

    bool isActive() const override 
    {
        return m_treeType == LayerTreeType::ACTIVE;
    }

private:
    ElementAnimations* m_elementAnimations;
    const LayerTreeType m_treeType;
};

WTF::PassOwnPtr<ElementAnimations> ElementAnimations::create(AnimationHost* host)
{
    return adoptPtr(new ElementAnimations(host));
}

ElementAnimations::ElementAnimations(AnimationHost* host)
    : m_playersList(adoptPtr(new PlayersList())), m_animationHost(host) 
{
    ASSERT(m_animationHost);
}

ElementAnimations::~ElementAnimations() 
{
    ASSERT(!m_layerAnimationController);
}

void ElementAnimations::createLayerAnimationController(int layerId) 
{
    ASSERT(layerId);
    ASSERT(!m_layerAnimationController);
    ASSERT(m_animationHost);

    AnimationRegistrar* registrar = m_animationHost->getAnimationRegistrar();
    ASSERT(registrar);

    m_layerAnimationController = registrar->getAnimationControllerForId(layerId);
    m_layerAnimationController->setAnimationRegistrar(registrar);
    m_layerAnimationController->setLayerAnimationDelegate(this);
    m_layerAnimationController->setValueProvider(this);

    ASSERT(m_animationHost->getMutatorHostClient());
    if (m_animationHost->getMutatorHostClient()->isLayerInTree(layerId, LayerTreeType::ACTIVE))
        createActiveValueObserver();
    if (m_animationHost->getMutatorHostClient()->isLayerInTree(layerId, LayerTreeType::PENDING))
        createPendingValueObserver();
}

void ElementAnimations::destroyLayerAnimationController() 
{
    ASSERT(m_animationHost);

    destroyPendingValueObserver();
    destroyActiveValueObserver();

    if (m_layerAnimationController) {
        m_layerAnimationController->removeValueProvider(this);
        m_layerAnimationController->removeLayerAnimationDelegate(this);
        m_layerAnimationController->setAnimationRegistrar(nullptr);
        m_layerAnimationController = nullptr;
    }
}

void ElementAnimations::layerRegistered(int layerId, LayerTreeType treeType)
{
    ASSERT(m_layerAnimationController);
    ASSERT(m_layerAnimationController->id() == layerId);

    if (treeType == LayerTreeType::ACTIVE) {
        if (!m_activeValueObserver)
            createActiveValueObserver();
    } else {
        if (!m_pendingValueObserver)
            createPendingValueObserver();
    }
}

void ElementAnimations::layerUnregistered(int layerId, LayerTreeType treeType) 
{
    ASSERT(this->getLayerId() == layerId);
    treeType == LayerTreeType::ACTIVE ? destroyActiveValueObserver() : destroyPendingValueObserver();
}

void ElementAnimations::addPlayer(AnimationPlayer* player)
{
    m_playersList->append(player);
}

void ElementAnimations::removePlayer(AnimationPlayer* player)
{
    for (PlayersListNode* node = m_playersList->head(); node != m_playersList->end(); node = node->next()) {
        if (node->value() == player) {
            node->removeFromList();
            return;
        }
    }
}

bool ElementAnimations::isEmpty() const
{
    return m_playersList->empty();
}

void ElementAnimations::pushPropertiesTo(ElementAnimations* elementAnimationsImpl)
{
    ASSERT(m_layerAnimationController);
    ASSERT(elementAnimationsImpl->getLayerAnimationController());

    m_layerAnimationController->pushAnimationUpdatesTo(elementAnimationsImpl->getLayerAnimationController());
}

void ElementAnimations::setFilterMutated(LayerTreeType treeType, const FilterOperations& filters)
{
    ASSERT(getLayerId());
    ASSERT(getAnimationHost());
    ASSERT(getAnimationHost()->getMutatorHostClient());
    getAnimationHost()->getMutatorHostClient()->setLayerFilterMutated(getLayerId(), treeType, filters);
}

void ElementAnimations::setOpacityMutated(LayerTreeType treeType, float opacity) 
{
    ASSERT(getLayerId());
    ASSERT(getAnimationHost());
    ASSERT(getAnimationHost()->getMutatorHostClient());
    getAnimationHost()->getMutatorHostClient()->setLayerOpacityMutated(getLayerId(), treeType, opacity);
}

void ElementAnimations::setTransformMutated(LayerTreeType treeType, const SkMatrix44& transform)
{
    ASSERT(getLayerId());
    ASSERT(getAnimationHost());
    ASSERT(getAnimationHost()->getMutatorHostClient());
    getAnimationHost()->getMutatorHostClient()->setLayerTransformMutated(getLayerId(), treeType, transform);
}

void ElementAnimations::setScrollOffsetMutated(LayerTreeType treeType, const blink::FloatPoint& scrollOffset)
{
    ASSERT(getLayerId());
    ASSERT(getAnimationHost());
    ASSERT(getAnimationHost()->getMutatorHostClient());
    getAnimationHost()->getMutatorHostClient()->setLayerScrollOffsetMutated(getLayerId(), treeType, scrollOffset);
}

void ElementAnimations::setTransformIsPotentiallyAnimatingChanged(LayerTreeType treeType, bool is_animating)
{
    ASSERT(getLayerId());
    ASSERT(getAnimationHost());
    ASSERT(getAnimationHost()->getMutatorHostClient());
    getAnimationHost()
        ->getMutatorHostClient()
        ->layerTransformIsPotentiallyAnimatingChanged(getLayerId(), treeType, is_animating);
}

void ElementAnimations::createActiveValueObserver() 
{
    ASSERT(m_layerAnimationController);
    ASSERT(!m_activeValueObserver);
    m_activeValueObserver = adoptPtr(new ValueObserver(this, LayerTreeType::ACTIVE));
    m_layerAnimationController->addValueObserver(m_activeValueObserver.get());
}

void ElementAnimations::destroyActiveValueObserver() 
{
    if (m_layerAnimationController && m_activeValueObserver)
        m_layerAnimationController->removeValueObserver(m_activeValueObserver.get());
    m_activeValueObserver = nullptr;
}

void ElementAnimations::createPendingValueObserver() 
{
    ASSERT(m_layerAnimationController);
    ASSERT(!m_pendingValueObserver);
    m_pendingValueObserver = adoptPtr(new ValueObserver(this, LayerTreeType::PENDING));
    m_layerAnimationController->addValueObserver(m_pendingValueObserver.get());
}

void ElementAnimations::destroyPendingValueObserver() 
{
    if (m_layerAnimationController && m_pendingValueObserver)
        m_layerAnimationController->removeValueObserver(m_pendingValueObserver.get());
    m_pendingValueObserver = nullptr;
}

void ElementAnimations::notifyAnimationStarted(base::TimeTicks monotonic_time, AnimationObj::TargetProperty target_property, int group) 
{
    for (PlayersListNode* node = m_playersList->head();
    node != m_playersList->end(); node = node->next()) {
        AnimationPlayer* player = node->value();
        player->notifyAnimationStarted(monotonic_time, target_property, group);
    }
}

void ElementAnimations::notifyAnimationFinished(base::TimeTicks monotonic_time, AnimationObj::TargetProperty target_property, int group)
{
    for (PlayersListNode* node = m_playersList->head();
    node != m_playersList->end(); node = node->next()) {
        AnimationPlayer* player = node->value();
        player->notifyAnimationFinished(monotonic_time, target_property, group);
    }
}

blink::FloatPoint ElementAnimations::scrollOffsetForAnimation() const
{
    ASSERT(m_layerAnimationController);
    if (getAnimationHost()) {
        ASSERT(getAnimationHost()->getMutatorHostClient());
        return getAnimationHost()->getMutatorHostClient()->getScrollOffsetForAnimation(getLayerId());
    }

    return blink::FloatPoint();
}

}  // namespace cc
