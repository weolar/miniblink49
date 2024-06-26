// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/animation/AnimationHost.h"

#include <algorithm>

#include "mc/animation/AnimationDelegate.h"
#include "mc/animation/AnimationIdProvider.h"
#include "mc/animation/AnimationPlayer.h"
#include "mc/animation/AnimationRegistrar.h"
#include "mc/animation/AnimationTimeline.h"
#include "mc/animation/ElementAnimations.h"
#include "mc/animation/ScrollOffsetAnimationCurve.h"
#include "mc/animation/TimingFunction.h"
#include "mc/base/FloatBox.h"

namespace mc {

class AnimationHost::ScrollOffsetAnimations : public AnimationDelegate {
public:
    explicit ScrollOffsetAnimations(AnimationHost* animation_host)
        : m_animationHost(animation_host)
        , m_scrollOffsetTimeline(AnimationTimeline::create(AnimationIdProvider::nextTimelineId()))
        , m_scrollOffsetAnimationPlayer(AnimationPlayer::create(AnimationIdProvider::nextPlayerId()))
    {
        m_scrollOffsetTimeline->setIsImplOnly(true);
        m_scrollOffsetAnimationPlayer->setLayerAnimationDelegate(this);

        m_animationHost->addAnimationTimeline(m_scrollOffsetTimeline.get());
        m_scrollOffsetTimeline->attachPlayer(m_scrollOffsetAnimationPlayer.get());
    }

    ~ScrollOffsetAnimations() override 
    {
        m_scrollOffsetTimeline->detachPlayer(m_scrollOffsetAnimationPlayer.get());
        m_animationHost->removeAnimationTimeline(m_scrollOffsetTimeline.get());
    }

    void scrollAnimationCreate(int layerId, const blink::FloatPoint& target_offset, const blink::FloatPoint& current_offset) {
        WTF::PassOwnPtr<ScrollOffsetAnimationCurve> curve = ScrollOffsetAnimationCurve::create(target_offset, EaseInOutTimingFunction::create());
        curve->setInitialValue(current_offset);

        WTF::PassOwnPtr<AnimationObj> animation = AnimationObj::create(curve, AnimationIdProvider::nextAnimationId(), AnimationIdProvider::nextGroupId(), AnimationObj::SCROLL_OFFSET);
        animation->setIsImplOnly(true);

        ASSERT(m_scrollOffsetAnimationPlayer);
        ASSERT(m_scrollOffsetAnimationPlayer->getAnimationTimeline());

        if (m_scrollOffsetAnimationPlayer->layerId() != layerId) {
            if (m_scrollOffsetAnimationPlayer->layerId())
                m_scrollOffsetAnimationPlayer->detachLayer();
            m_scrollOffsetAnimationPlayer->attachLayer(layerId);
        }

        m_scrollOffsetAnimationPlayer->addAnimation(animation);
    }

    static blink::FloatPoint setToMin(const blink::FloatPoint& me, const blink::FloatPoint& other)
    {
        blink::FloatPoint newPoint(me.x() <= other.x() ? me.x() : other.x(), me.y() <= other.y() ? me.y() : other.y());
        return newPoint;
    }

    static blink::FloatPoint setToMax(const blink::FloatPoint& me, const blink::FloatPoint& other)
    {
        blink::FloatPoint newPoint(me.x() >= other.x() ? me.x() : other.x(), me.y() >= other.y() ? me.y() : other.y());
        return newPoint;
    }

    bool scrollAnimationUpdateTarget(int layerId, const blink::FloatPoint& scrollDelta, const blink::FloatPoint& maxScrollOffset, base::TimeTicks frameMonotonicTime)
    {
        ASSERT(m_scrollOffsetAnimationPlayer);
        ASSERT(layerId ==  m_scrollOffsetAnimationPlayer->layerId());

        AnimationObj* animation = m_scrollOffsetAnimationPlayer->getElementAnimations()
            ->getLayerAnimationController()
            ->getAnimation(AnimationObj::SCROLL_OFFSET);
        if (!animation) {
            m_scrollOffsetAnimationPlayer->detachLayer();
            return false;
        }

        ScrollOffsetAnimationCurve* curve = animation->curve()->toScrollOffsetAnimationCurve();

        blink::FloatPoint newTarget = curve->targetValue() + scrollDelta;
        
        newTarget = setToMax(newTarget, blink::FloatPoint());
        newTarget = setToMin(newTarget, maxScrollOffset);

        curve->updateTarget(animation->trimTimeToCurrentIteration(frameMonotonicTime).InSecondsF(), newTarget);

        return true;
    }

    // AnimationDelegate implementation.
    void notifyAnimationStarted(base::TimeTicks monotonic_time, AnimationObj::TargetProperty target_property, int group) override {}
    void notifyAnimationFinished(base::TimeTicks monotonic_time, AnimationObj::TargetProperty target_property, int group) override 
    {
        ASSERT(target_property == AnimationObj::SCROLL_OFFSET);
        ASSERT(m_animationHost->getMutatorHostClient());
        m_animationHost->getMutatorHostClient()->scrollOffsetAnimationFinished();
    }

private:
    AnimationHost* m_animationHost;
    WTF::RefPtr<AnimationTimeline> m_scrollOffsetTimeline;

    // We have just one player for impl-only scroll offset animations.
    // I.e. only one layer can have an impl-only scroll offset animation at
    // any given time.
    WTF::RefPtr<AnimationPlayer> m_scrollOffsetAnimationPlayer;
};

PassOwnPtr<AnimationHost> AnimationHost::create(ThreadInstance threadInstance) 
{
    return adoptPtr(new AnimationHost(threadInstance));
}

AnimationHost::AnimationHost(ThreadInstance threadInstance)
    : m_animationRegistrar(AnimationRegistrar::create())
    , m_mutatorHostClient(nullptr)
    , m_threadInstance(threadInstance) 
{
    if (m_threadInstance == ThreadInstance::IMPL)
        m_scrollOffsetAnimations = adoptPtr(new ScrollOffsetAnimations(this));
}

AnimationHost::~AnimationHost() 
{
    m_scrollOffsetAnimations = nullptr;

    clearTimelines();
    ASSERT(!getMutatorHostClient());
    ASSERT(m_layerToElementAnimationsMap.isEmpty());
}

AnimationTimeline* AnimationHost::getTimelineById(int timelineId) const 
{
    for (size_t i = 0; i < m_timelines.size(); ++i) {
        AnimationTimeline* timeline = m_timelines[i];
        if (timeline->id() == timelineId)
            return timeline;
    }
    return nullptr;
}

void AnimationHost::clearTimelines()
{
    eraseTimelines(m_timelines.begin(), m_timelines.end());
}

void AnimationHost::eraseTimelines(AnimationTimelineList::iterator begin, AnimationTimelineList::iterator end)
{
    for (AnimationTimelineList::iterator i = begin; i != end; ++i) {
        AnimationTimeline* timeline = *i;
        timeline->clearPlayers();
        timeline->setAnimationHost(nullptr);
        timeline->deref();
    }

    m_timelines.erase(begin, end);
}

void AnimationHost::addAnimationTimeline(WTF::PassRefPtr<AnimationTimeline> timeline) 
{
    timeline->setAnimationHost(this);
    AnimationTimeline* timelinePtr = timeline.leakRef();
    timelinePtr->ref();
    m_timelines.push_back(timelinePtr);
}

void AnimationHost::removeAnimationTimeline(WTF::PassRefPtr<AnimationTimeline> timeline)
{
    for (AnimationTimelineList::iterator iter = m_timelines.begin(); iter != m_timelines.end(); ++iter) {
        AnimationTimeline* it = *iter;
        if (it != timeline.get())
            continue;

        eraseTimelines(iter, iter + 1);
        break;
    }
}

void AnimationHost::registerLayer(int layerId, LayerTreeType treeType) 
{
    ElementAnimations* elementAnimations = getElementAnimationsForLayerId(layerId);
    if (elementAnimations)
        elementAnimations->layerRegistered(layerId, treeType);
}

void AnimationHost::unregisterLayer(int layerId, LayerTreeType treeType) 
{
    ElementAnimations* elementAnimations = getElementAnimationsForLayerId(layerId);
    if (elementAnimations)
        elementAnimations->layerUnregistered(layerId, treeType);
}

void AnimationHost::registerPlayerForLayer(int layerId, AnimationPlayer* player) 
{
    ASSERT(layerId);
    ASSERT(player);

    ElementAnimations* elementAnimations = getElementAnimationsForLayerId(layerId);
    if (!elementAnimations) {
        WTF::OwnPtr<ElementAnimations> newElementAnimations = ElementAnimations::create(this);
        elementAnimations = newElementAnimations.leakPtr();

        m_layerToElementAnimationsMap.add(layerId, elementAnimations);
        elementAnimations->createLayerAnimationController(layerId);
    }

    ASSERT(elementAnimations);
    elementAnimations->addPlayer(player);
}

void AnimationHost::unregisterPlayerForLayer(int layerId, AnimationPlayer* player) {
    ASSERT(layerId);
    ASSERT(player);

    ElementAnimations* elementAnimations = getElementAnimationsForLayerId(layerId);
    ASSERT(elementAnimations);
    elementAnimations->removePlayer(player);

    if (elementAnimations->isEmpty()) {
        elementAnimations->destroyLayerAnimationController();

        LayerToElementAnimationsMap::iterator it = m_layerToElementAnimationsMap.find(layerId);
        if (it != m_layerToElementAnimationsMap.end()) {
            ElementAnimations* elementAnimationsPtr = it->value;
            ASSERT(elementAnimationsPtr == elementAnimations);
            delete elementAnimationsPtr;
            m_layerToElementAnimationsMap.remove(it);
        }
        elementAnimations = nullptr;
    }
}

void AnimationHost::setMutatorHostClient(MutatorHostClient* client)
{
    if (m_mutatorHostClient == client)
        return;

    m_mutatorHostClient = client;
}

void AnimationHost::setNeedsCommit() 
{
    ASSERT(m_mutatorHostClient);
    m_mutatorHostClient->setMutatorsNeedCommit();
}

void AnimationHost::pushPropertiesTo(AnimationHost* hostImpl) 
{
    pushTimelinesToImplThread(hostImpl);
    removeTimelinesFromImplThread(hostImpl);
    pushPropertiesToImplThread(hostImpl);
}

void AnimationHost::pushTimelinesToImplThread(AnimationHost* hostImpl) const 
{
    for (size_t i = 0; i < m_timelines.size(); ++i) {
        AnimationTimeline* timeline = m_timelines[0];
        
        AnimationTimeline* timelineImpl = hostImpl->getTimelineById(timeline->id());
        if (timelineImpl)
            continue;

        WTF::PassRefPtr<AnimationTimeline> to_add = timeline->createImplInstance();
        hostImpl->addAnimationTimeline(to_add);
    }
}

void AnimationHost::removeTimelinesFromImplThread(AnimationHost* hostImpl) const 
{
    AnimationTimelineList& timelinesImpl = hostImpl->m_timelines;

    auto to_erase = std::partition(timelinesImpl.begin(), timelinesImpl.end(), [this](AnimationTimeline* timelineImpl) {
        return timelineImpl->getIsImplOnly() || getTimelineById(timelineImpl->id());
    });

    hostImpl->eraseTimelines(to_erase, timelinesImpl.end());
}

void AnimationHost::pushPropertiesToImplThread(AnimationHost* hostImpl) 
{
    // Firstly, sync all players with impl thread to create ElementAnimations and
    // layer animation controllers.
    for (auto& timeline : m_timelines) {
        AnimationTimeline* timelineImpl = hostImpl->getTimelineById(timeline->id());
        if (timelineImpl)
            timeline->pushPropertiesTo(timelineImpl);
    }

    // Secondly, sync properties for created layer animation controllers.
    for (LayerToElementAnimationsMap::iterator kv = m_layerToElementAnimationsMap.begin(); kv != m_layerToElementAnimationsMap.end(); ++kv) {
        ElementAnimations* elementAnimations = kv->value;
        ElementAnimations* elementAnimationsImpl = hostImpl->getElementAnimationsForLayerId(kv->key);
        if (elementAnimationsImpl)
            elementAnimations->pushPropertiesTo(elementAnimationsImpl);
    }
}

LayerAnimationController* AnimationHost::getControllerForLayerId(int layerId) const
{
    const ElementAnimations* elementAnimations = getElementAnimationsForLayerId(layerId);
    if (!elementAnimations)
        return nullptr;

    return elementAnimations->getLayerAnimationController();
}

ElementAnimations* AnimationHost::getElementAnimationsForLayerId(int layerId) const 
{
    ASSERT(layerId);
    LayerToElementAnimationsMap::const_iterator iter = m_layerToElementAnimationsMap.find(layerId);
    return iter == m_layerToElementAnimationsMap.end() ? nullptr : iter->value;
}

void AnimationHost::setSupportsScrollAnimations(bool supports_scroll_animations) 
{
    m_animationRegistrar->setSupportsScrollAnimations(supports_scroll_animations);
}

bool AnimationHost::supportsScrollAnimations() const 
{
    return m_animationRegistrar->supportsScrollAnimations();
}

bool AnimationHost::needsAnimateLayers() const 
{
    return m_animationRegistrar->needsAnimateLayers();
}

bool AnimationHost::activateAnimations()
{
    return m_animationRegistrar->activateAnimations();
}

bool AnimationHost::animateLayers(base::TimeTicks monotonic_time)
{
    return m_animationRegistrar->animateLayers(monotonic_time);
}

bool AnimationHost::updateAnimationState(bool start_ready_animations, AnimationEventsVector* events)
{
    return m_animationRegistrar->updateAnimationState(start_ready_animations, events);
}

PassOwnPtr<AnimationEventsVector> AnimationHost::createEvents()
{
    return m_animationRegistrar->createEvents();
}

void AnimationHost::setAnimationEvents(PassOwnPtr<AnimationEventsVector> events) 
{
    return m_animationRegistrar->setAnimationEvents(events);
}

bool AnimationHost::scrollOffsetAnimationWasInterrupted(int layerId) const 
{
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    return controller ? controller->scrollOffsetAnimationWasInterrupted() : false;
}

static LayerAnimationController::ObserverType observerTypeFromTreeType(LayerTreeType treeType) 
{
    return treeType == LayerTreeType::ACTIVE
        ? LayerAnimationController::ObserverType::ACTIVE
        : LayerAnimationController::ObserverType::PENDING;
}

bool AnimationHost::isAnimatingFilterProperty(int layerId, LayerTreeType treeType) const
{
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    return controller ? controller->isCurrentlyAnimatingProperty(AnimationObj::FILTER, observerTypeFromTreeType(treeType)) : false;
}

bool AnimationHost::isAnimatingOpacityProperty(int layerId, LayerTreeType treeType) const 
{
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    return controller ? controller->isCurrentlyAnimatingProperty(AnimationObj::OPACITY, observerTypeFromTreeType(treeType)) : false;
}

bool AnimationHost::isAnimatingTransformProperty(int layerId, LayerTreeType treeType) const 
{
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    return controller ? controller->isCurrentlyAnimatingProperty(AnimationObj::TRANSFORM, observerTypeFromTreeType(treeType)) : false;
}

bool AnimationHost::hasPotentiallyRunningFilterAnimation(int layerId, LayerTreeType treeType) const 
{
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    return controller ? controller->isPotentiallyAnimatingProperty(AnimationObj::FILTER, observerTypeFromTreeType(treeType)) : false;
}

bool AnimationHost::hasPotentiallyRunningOpacityAnimation(int layerId, LayerTreeType treeType) const
{
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    return controller ? controller->isPotentiallyAnimatingProperty(AnimationObj::OPACITY, observerTypeFromTreeType(treeType)) : false;
}

bool AnimationHost::hasPotentiallyRunningTransformAnimation(
    int layerId,
    LayerTreeType treeType) const {
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    return controller ? controller->isPotentiallyAnimatingProperty(AnimationObj::TRANSFORM, observerTypeFromTreeType(treeType)) : false;
}

bool AnimationHost::hasAnyAnimationTargetingProperty(int layerId, AnimationObj::TargetProperty property) const
{
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    if (!controller)
        return false;

    return !!controller->getAnimation(property);
}

bool AnimationHost::filterIsAnimatingOnImplOnly(int layerId) const 
{
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    if (!controller)
        return false;

    AnimationObj* animation = controller->getAnimation(AnimationObj::FILTER);
    return animation && animation->isImplOnly();
}

bool AnimationHost::opacityIsAnimatingOnImplOnly(int layerId) const
{
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    if (!controller)
        return false;

    AnimationObj* animation = controller->getAnimation(AnimationObj::OPACITY);
    return animation && animation->isImplOnly();
}

bool AnimationHost::transformIsAnimatingOnImplOnly(int layerId) const 
{
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    if (!controller)
        return false;

    AnimationObj* animation = controller->getAnimation(AnimationObj::TRANSFORM);
    return animation && animation->isImplOnly();
}

bool AnimationHost::hasFilterAnimationThatInflatesBounds(int layerId) const 
{
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    return controller ? controller->hasFilterAnimationThatInflatesBounds() : false;
}

bool AnimationHost::hasTransformAnimationThatInflatesBounds(int layerId) const
{
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    return controller ? controller->hasTransformAnimationThatInflatesBounds() : false;
}

bool AnimationHost::hasAnimationThatInflatesBounds(int layerId) const 
{
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    return controller ? controller->hasAnimationThatInflatesBounds() : false;
}

bool AnimationHost::filterAnimationBoundsForBox(int layerId, const FloatBox& box, FloatBox* bounds) const 
{
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    return controller ? controller->filterAnimationBoundsForBox(box, bounds) : false;
}

bool AnimationHost::transformAnimationBoundsForBox(int layerId, const FloatBox& box, FloatBox* bounds) const 
{
    *bounds = FloatBox();
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    return controller ? controller->transformAnimationBoundsForBox(box, bounds) : true;
}

bool AnimationHost::hasOnlyTranslationTransforms(int layerId, LayerTreeType treeType) const 
{
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    return controller ? controller->hasOnlyTranslationTransforms(observerTypeFromTreeType(treeType)) : true;
}

bool AnimationHost::animationsPreserveAxisAlignment(int layerId) const 
{
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    return controller ? controller->animationsPreserveAxisAlignment() : true;
}

bool AnimationHost::maximumTargetScale(int layerId, LayerTreeType treeType, float* max_scale) const
{
    *max_scale = 0.f;
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    return controller ? controller->maximumTargetScale(observerTypeFromTreeType(treeType), max_scale) : true;
}

bool AnimationHost::animationStartScale(int layerId, LayerTreeType treeType, float* start_scale) const 
{
    *start_scale = 0.f;
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    return controller ? controller->animationStartScale(observerTypeFromTreeType(treeType), start_scale) : true;
}

bool AnimationHost::hasAnyAnimation(int layerId) const 
{
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    return controller ? controller->hasAnyAnimation() : false;
}

bool AnimationHost::hasActiveAnimation(int layerId) const 
{
    LayerAnimationController* controller = getControllerForLayerId(layerId);
    return controller ? controller->hasActiveAnimation() : false;
}

void AnimationHost::implOnlyScrollAnimationCreate(int layerId, const blink::FloatPoint& target_offset, const blink::FloatPoint& current_offset) 
{
    ASSERT(m_scrollOffsetAnimations);
    m_scrollOffsetAnimations->scrollAnimationCreate(layerId, target_offset,  current_offset);
}

bool AnimationHost::implOnlyScrollAnimationUpdateTarget(
    int layerId, 
    const blink::FloatPoint& scroll_delta,
    const blink::FloatPoint& maxScrollOffset,
    base::TimeTicks frameMonotonicTime
    )
{
    ASSERT(m_scrollOffsetAnimations);
    return m_scrollOffsetAnimations->scrollAnimationUpdateTarget(layerId, scroll_delta, maxScrollOffset, frameMonotonicTime);
}

}  // namespace cc
