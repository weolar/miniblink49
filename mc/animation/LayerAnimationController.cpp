// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/animation/LayerAnimationController.h"

#include <algorithm>
#include <vector>

#include "mc/animation/AnimationObj.h"
#include "mc/animation/AnimationDelegate.h"
#include "mc/animation/AnimationRegistrar.h"
#include "mc/animation/KeyframedAnimationCurve.h"
#include "mc/animation/LayerAnimationValueObserver.h"
#include "mc/animation/LayerAnimationValueProvider.h"
#include "mc/animation/ScrollOffsetAnimationCurve.h"
#include "mc/trees/LayerTreeHost.h"
#include "mc/blink/WebLayerImpl.h"
#include "mc/blink/WebFilterOperationsImpl.h"
#include "mc/base/FloatBox.h"
#include "third_party/skia/include/utils/SkMatrix44.h"
#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"

namespace mc {

template <class ForwardIterator, class Predicate, class ScopedContainer>
ForwardIterator remove_if(ScopedContainer* container, ForwardIterator first, ForwardIterator last, Predicate predicate)
{
    ForwardIterator result = first;
    for (; first != last; ++first) {
        if (!predicate(*first)) {
            container->swap(first, result);
            ++result;
        }
    }
    return result;
}

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, layerAnimationControllerCounter, ("mcLayerAnimationController"));
#endif

LayerAnimationController::LayerAnimationController(LayerTreeHost* host, int id)
    : m_registrar(0)
    , m_id(id)
    , m_isActive(false)
    , m_valueProvider(nullptr)
    , m_layerAnimationDelegate(nullptr)
    , m_needsToStartAnimations(false)
    , m_scrollOffsetAnimationWasInterrupted(false)
    , m_potentiallyAnimatingTransformForActiveObservers(false)
    , m_potentiallyAnimatingTransformForPendingObservers(false)
    , m_host(host)
{
#ifndef NDEBUG
    layerAnimationControllerCounter.increment();
#endif
}

LayerAnimationController::~LayerAnimationController()
{
#ifndef NDEBUG
    layerAnimationControllerCounter.decrement();
#endif
//     if (m_registrar)
//         m_registrar->unregisterAnimationController(this);
}

void LayerAnimationController::destroy()
{
    ASSERT(refCount() == 1);
    delete this;
}

WTF::PassRefPtr<LayerAnimationController> LayerAnimationController::create(LayerTreeHost* host, int id)
{
    return adoptRef(new LayerAnimationController(host, id));
}

void LayerAnimationController::pauseAnimation(int animation_id, base::TimeDelta timeOffset) 
{
    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (m_animations[i]->id() == animation_id) {
            m_animations[i]->setRunState(AnimationObj::PAUSED,
                timeOffset + m_animations[i]->startTime() + m_animations[i]->timeOffset());
        }
    }
}

struct HasAnimationId {
    explicit HasAnimationId(int id) : m_id(id) {}
    bool operator()(AnimationObj* animation) const {
        return animation->id() == m_id;
    }

private:
    int m_id;
};

void LayerAnimationController::updatePotentiallyAnimatingTransform()
{
    bool wasPotentiallyAnimatingTransformForActiveObservers = m_potentiallyAnimatingTransformForActiveObservers;
    bool wasPotentiallyAnimatingTransformForPendingObservers = m_potentiallyAnimatingTransformForPendingObservers;

    m_potentiallyAnimatingTransformForActiveObservers = false;
    m_potentiallyAnimatingTransformForPendingObservers = false;

    for (AnimationObj* animation : m_animations) {
        if (!animation->isFinished() && animation->getTargetProperty() == AnimationObj::TRANSFORM) {
            m_potentiallyAnimatingTransformForActiveObservers |= animation->affectsActiveObservers();
            m_potentiallyAnimatingTransformForPendingObservers |= animation->affectsPendingObservers();
        }
    }

    bool changedForActiveObservers = wasPotentiallyAnimatingTransformForActiveObservers != m_potentiallyAnimatingTransformForActiveObservers;
    bool changedForPendingObservers = wasPotentiallyAnimatingTransformForPendingObservers != m_potentiallyAnimatingTransformForPendingObservers;

    if (!changedForActiveObservers && !changedForPendingObservers)
        return;

    notifyObserversTransformIsPotentiallyAnimatingChanged(changedForActiveObservers, changedForPendingObservers);
}

void LayerAnimationController::removeAnimation(int animation_id)
{
    bool removedTransformAnimation = false;
    ScopedPtrVector<AnimationObj>::iterator animationsToRemove = m_animations.remove_if(HasAnimationId(animation_id));
    for (ScopedPtrVector<AnimationObj>::iterator it = animationsToRemove; it != m_animations.end(); ++it) {
        if ((*it)->getTargetProperty() == AnimationObj::SCROLL_OFFSET) {
            m_scrollOffsetAnimationWasInterrupted = true;
        } else if ((*it)->getTargetProperty() == AnimationObj::TRANSFORM && !(*it)->isFinished()) {
            removedTransformAnimation = true;
        }
    }

    m_animations.erase(animationsToRemove, m_animations.end());
    updateActivation(NORMAL_ACTIVATION);
    if (removedTransformAnimation)
        updatePotentiallyAnimatingTransform();
}

struct HasAnimationIdAndProperty {
    HasAnimationIdAndProperty(int id, AnimationObj::TargetProperty targetProperty)
        : m_id(id), m_targetProperty(targetProperty) {}

    bool operator()(AnimationObj* animation) const {
        return animation->id() == m_id &&
            animation->getTargetProperty() == m_targetProperty;
    }

private:
    int m_id;
    AnimationObj::TargetProperty m_targetProperty;
};

void LayerAnimationController::removeAnimation(int animationId, AnimationObj::TargetProperty targetProperty) 
{
    bool removedTransformAnimation = false;
    ScopedPtrVector<AnimationObj>::iterator animationsToRemove = m_animations.remove_if(HasAnimationIdAndProperty(animationId, targetProperty));
    if (animationsToRemove == m_animations.end())
        return;

    if (targetProperty == AnimationObj::SCROLL_OFFSET)
        m_scrollOffsetAnimationWasInterrupted = true;
    else if (targetProperty == AnimationObj::TRANSFORM && !(*animationsToRemove)->isFinished())
        removedTransformAnimation = true;

    m_animations.erase(animationsToRemove, m_animations.end());
    updateActivation(NORMAL_ACTIVATION);
    if (removedTransformAnimation)
        updatePotentiallyAnimatingTransform();
}

void LayerAnimationController::abortAnimations(AnimationObj::TargetProperty targetProperty)
{
    bool abortedTransformAnimation = false;
    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (m_animations[i]->getTargetProperty() == targetProperty && !m_animations[i]->isFinished()) {
            m_animations[i]->setRunState(AnimationObj::ABORTED, m_lastTickTime);
            if (targetProperty == AnimationObj::TRANSFORM)
                abortedTransformAnimation = true;
        }
    }
    if (abortedTransformAnimation)
        updatePotentiallyAnimatingTransform();
}

// Ensures that the list of active animations on the main thread and the impl
// thread are kept in sync.
void LayerAnimationController::pushAnimationUpdatesTo(LayerAnimationController* controllerImpl)
{
    ASSERT(this != controllerImpl);
    if (!hasAnyAnimation() && !controllerImpl->hasAnyAnimation())
        return;
    purgeAnimationsMarkedForDeletion();
    pushNewAnimationsToImplThread(controllerImpl);

    // Remove finished impl side animations only after pushing,
    // and only after the animations are deleted on the main thread
    // this insures we will never push an animation twice.
    removeAnimationsCompletedOnMainThread(controllerImpl);

    pushPropertiesToImplThread(controllerImpl);
    controllerImpl->updateActivation(NORMAL_ACTIVATION);
    updateActivation(NORMAL_ACTIVATION);
}

void LayerAnimationController::animate(base::TimeTicks monotonicTime) {
    ASSERT(!monotonicTime.is_null());
    if (!hasValueObserver())
        return;

    if (m_needsToStartAnimations)
        startAnimations(monotonicTime);
    tickAnimations(monotonicTime);
    m_lastTickTime = monotonicTime;
}

void LayerAnimationController::accumulatePropertyUpdates(base::TimeTicks monotonicTime, AnimationEventsVector* events)
{
    if (!events)
        return;

    for (size_t i = 0; i < m_animations.size(); ++i) {
        AnimationObj* animation = m_animations[i];
        if (!animation->isImplOnly())
            continue;

        if (!animation->inEffect(monotonicTime))
            continue;

        base::TimeDelta trimmed = animation->trimTimeToCurrentIteration(monotonicTime);
        switch (animation->getTargetProperty()) {
        case AnimationObj::OPACITY:
        {
            AnimationEvent event(AnimationEvent::PROPERTY_UPDATE, m_id,
                animation->group(), AnimationObj::OPACITY,
                monotonicTime);
            const FloatAnimationCurve* floatAnimationCurve = animation->curve()->toFloatAnimationCurve();
            event.opacity = floatAnimationCurve->getValue(trimmed);
            event.isImplOnly = true;
            events->push_back(event);
            break;
        }

        case AnimationObj::TRANSFORM:
        {
            AnimationEvent event(AnimationEvent::PROPERTY_UPDATE, m_id,
                animation->group(), AnimationObj::TRANSFORM,
                monotonicTime);
            const TransformAnimationCurve* transformAnimationCurve =
                animation->curve()->toTransformAnimationCurve();
            event.transform = transformAnimationCurve->getValue(trimmed);
            event.isImplOnly = true;
            events->push_back(event);
            break;
        }

        case AnimationObj::FILTER:
        {
            AnimationEvent event(AnimationEvent::PROPERTY_UPDATE, m_id,
                animation->group(), AnimationObj::FILTER,
                monotonicTime);
//             const FilterAnimationCurve* filterAnimationCurve = animation->curve()->toFilterAnimationCurve();
//             event.filters = filterAnimationCurve->getValue(trimmed);
//             event.isImplOnly = true;
//             events->push_back(event);
            DebugBreak();
            break;
        }

        case AnimationObj::BACKGROUND_COLOR:
        {
            break;
        }

        case AnimationObj::SCROLL_OFFSET:
        {
            // Impl-side changes to scroll offset are already sent back to the
            // main thread (e.g. for user-driven scrolling), so a PROPERTY_UPDATE
            // isn't needed.
            break;
        }
        }
    }
}

void LayerAnimationController::updateState(bool startReadyAnimations, AnimationEventsVector* events) 
{
    if (!hasActiveValueObserver())
        return;

    // Animate hasn't been called, this happens if an observer has been added
    // between the Commit and Draw phases.
    if (m_lastTickTime == base::TimeTicks())
        return;

    if (startReadyAnimations)
        promoteStartedAnimations(m_lastTickTime, events);

    markFinishedAnimations(m_lastTickTime);
    markAnimationsForDeletion(m_lastTickTime, events);

    if (m_needsToStartAnimations && startReadyAnimations) {
        startAnimations(m_lastTickTime);
        promoteStartedAnimations(m_lastTickTime, events);
    }

    accumulatePropertyUpdates(m_lastTickTime, events);

    updateActivation(NORMAL_ACTIVATION);
}

struct AffectsNoObservers {
    bool operator()(AnimationObj* animation) const
    {
        return !animation->affectsActiveObservers() && !animation->affectsPendingObservers();
    }
};

void LayerAnimationController::activateAnimations() {
    bool changed_transform_animation = false;
    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (m_animations[i]->affectsActiveObservers() !=
            m_animations[i]->affectsPendingObservers() &&
            m_animations[i]->getTargetProperty() == AnimationObj::TRANSFORM)
            changed_transform_animation = true;
        m_animations[i]->setAffectsActiveObservers(
            m_animations[i]->affectsPendingObservers());
    }
    m_animations.erase(remove_if(&m_animations,
        m_animations.begin(),
        m_animations.end(),
        AffectsNoObservers()),
        m_animations.end());
    m_scrollOffsetAnimationWasInterrupted = false;
    updateActivation(NORMAL_ACTIVATION);
    if (changed_transform_animation)
        updatePotentiallyAnimatingTransform();
}

void LayerAnimationController::addAnimation(WTF::PassOwnPtr<AnimationObj> animation)
{
    bool added_transform_animation = animation->getTargetProperty() == AnimationObj::TRANSFORM;
    m_animations.push_back(animation.leakPtr());
    m_needsToStartAnimations = true;
    updateActivation(NORMAL_ACTIVATION);
    if (added_transform_animation)
        updatePotentiallyAnimatingTransform();
}

AnimationObj* LayerAnimationController::getAnimation(AnimationObj::TargetProperty targetProperty) const
{
    for (size_t i = 0; i < m_animations.size(); ++i) {
        size_t index = m_animations.size() - i - 1;
        if (m_animations[index]->getTargetProperty() == targetProperty)
            return m_animations[index];
    }
    return 0;
}

AnimationObj* LayerAnimationController::getAnimationById(int animation_id) const {
    for (size_t i = 0; i < m_animations.size(); ++i)
        if (m_animations[i]->id() == animation_id)
            return m_animations[i];
    return nullptr;
}

bool LayerAnimationController::hasActiveAnimation() const {
    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (!m_animations[i]->isFinished())
            return true;
    }
    return false;
}

bool LayerAnimationController::isPotentiallyAnimatingProperty(AnimationObj::TargetProperty targetProperty, ObserverType observer_type) const
{
    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (!m_animations[i]->isFinished() && m_animations[i]->getTargetProperty() == targetProperty) {
            if ((observer_type == ObserverType::ACTIVE && m_animations[i]->affectsActiveObservers()) || 
                (observer_type == ObserverType::PENDING && m_animations[i]->affectsPendingObservers()))
                return true;
        }
    }
    return false;
}

bool LayerAnimationController::isCurrentlyAnimatingProperty(AnimationObj::TargetProperty targetProperty, ObserverType observer_type) const 
{
    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (!m_animations[i]->isFinished() &&
            m_animations[i]->inEffect(m_lastTickTime) &&
            m_animations[i]->getTargetProperty() == targetProperty) {
            if ((observer_type == ObserverType::ACTIVE && m_animations[i]->affectsActiveObservers()) ||
                (observer_type == ObserverType::PENDING && m_animations[i]->affectsPendingObservers()))
                return true;
        }
    }
    return false;
}

void LayerAnimationController::setAnimationRegistrar(AnimationRegistrar* registrar) {
//     if (m_registrar == registrar)
//         return;
// 
//     if (m_registrar)
//         m_registrar->unregisterAnimationController(this);
// 
//     m_registrar = registrar;
//     if (m_registrar)
//         m_registrar->registerAnimationController(this);
// 
//     updateActivation(FORCE_ACTIVATION);
}

void LayerAnimationController::notifyAnimationStarted(const AnimationEvent& event) 
{
//     if (event.isImplOnly) {
//         FOR_EACH_OBSERVER(LayerAnimationEventObserver, event_observers_, OnAnimationStarted(event));
//         if (m_layerAnimationDelegate)
//             m_layerAnimationDelegate->notifyAnimationStarted(event.monotonicTime, event.targetProperty, event.groupId);
//         return;
//     }
//
    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (m_animations[i]->group() == event.groupId && m_animations[i]->getTargetProperty() == event.targetProperty && m_animations[i]->needsSynchronizedStartTime()) {
            m_animations[i]->setNeedsSynchronizedStartTime(false);
            if (!m_animations[i]->hasSetStartTime())
                m_animations[i]->setStartTime(event.monotonicTime);

            //FOR_EACH_OBSERVER(LayerAnimationEventObserver, event_observers_, OnAnimationStarted(event));
//             if (m_layerAnimationDelegate)
//                 m_layerAnimationDelegate->notifyAnimationStarted(event.monotonicTime, event.targetProperty, event.groupId);
            if (m_host)
                m_host->notifyAnimationEvent(true, (event.monotonicTime - base::TimeTicks()).InSecondsF(), event.groupId);

            return;
        }
    }
}

void LayerAnimationController::notifyAnimationFinished(const AnimationEvent& event) 
{
//     if (event.isImplOnly) {
//         if (m_layerAnimationDelegate)
//             m_layerAnimationDelegate->notifyAnimationFinished(event.monotonicTime, event.targetProperty, event.groupId);
//         return;
//     }

    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (m_animations[i]->group() == event.groupId &&
            m_animations[i]->getTargetProperty() == event.targetProperty) {
            m_animations[i]->setReceivedFinishedEvent(true);
//             if (m_layerAnimationDelegate)
//                 m_layerAnimationDelegate->notifyAnimationFinished(event.monotonicTime, event.targetProperty, event.groupId);
            m_host->notifyAnimationEvent(false, (event.monotonicTime - base::TimeTicks()).InSecondsF(), event.groupId);

            return;
        }
    }
}

void LayerAnimationController::notifyAnimationAborted(const AnimationEvent& event)
{
    bool abortedTransformAnimation = false;
    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (m_animations[i]->group() == event.groupId && m_animations[i]->getTargetProperty() == event.targetProperty) {
            m_animations[i]->setRunState(AnimationObj::ABORTED, event.monotonicTime);
            if (event.targetProperty == AnimationObj::TRANSFORM)
                abortedTransformAnimation = true;
        }
    }
    if (abortedTransformAnimation)
        updatePotentiallyAnimatingTransform();
}

void LayerAnimationController::notifyAnimationPropertyUpdate(const AnimationEvent& event) 
{
    bool notify_active_observers = true;
    bool notify_pending_observers = true;
    switch (event.targetProperty) {
    case AnimationObj::OPACITY:
        notifyObserversOpacityAnimated(event.opacity, notify_active_observers, notify_pending_observers);
        break;
    case AnimationObj::TRANSFORM:
        notifyObserversTransformAnimated(event.transform, notify_active_observers, notify_pending_observers);
        break;
    default:
        DebugBreak();
    }
}

void LayerAnimationController::addValueObserver(LayerAnimationValueObserver* observer)
{
//     if (!value_observers_.HasObserver(observer))
//         value_observers_.AddObserver(observer);
}

void LayerAnimationController::removeValueObserver(LayerAnimationValueObserver* observer) 
{
//     value_observers_.RemoveObserver(observer);
}

void LayerAnimationController::addEventObserver(LayerAnimationEventObserver* observer) 
{
//     if (!event_observers_.HasObserver(observer))
//         event_observers_.AddObserver(observer);
}

void LayerAnimationController::removeEventObserver(LayerAnimationEventObserver* observer) 
{
//    event_observers_.RemoveObserver(observer);
}

bool LayerAnimationController::hasFilterAnimationThatInflatesBounds() const
{
    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (!m_animations[i]->isFinished() &&
            m_animations[i]->getTargetProperty() == AnimationObj::FILTER &&
            m_animations[i]->curve()->toFilterAnimationCurve()->hasFilterThatMovesPixels())
            return true;
    }

    return false;
}

bool LayerAnimationController::hasTransformAnimationThatInflatesBounds() const {
    return isCurrentlyAnimatingProperty(AnimationObj::TRANSFORM, ObserverType::ACTIVE) || isCurrentlyAnimatingProperty(AnimationObj::TRANSFORM, ObserverType::PENDING);
}

bool LayerAnimationController::filterAnimationBoundsForBox(const FloatBox& box, FloatBox* bounds) const 
{
    // TODO(avallee): Implement.
    return false;
}

bool LayerAnimationController::transformAnimationBoundsForBox(const FloatBox& box, FloatBox* bounds) const
{
    ASSERT(hasTransformAnimationThatInflatesBounds());
//         << "TransformAnimationBoundsForBox will give incorrect results if there "
//         << "are no transform animations affecting bounds, non-animated transform "
//         << "is not known";

    // Compute bounds based on animations for which isFinished() is false.
    // Do nothing if there are no such animations; in this case, it is assumed
    // that callers will take care of computing bounds based on the owning layer's
    // actual transform.
    *bounds = FloatBox();
    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (m_animations[i]->isFinished() || m_animations[i]->getTargetProperty() != AnimationObj::TRANSFORM)
            continue;

        const TransformAnimationCurve* transform_animation_curve = m_animations[i]->curve()->toTransformAnimationCurve();
        FloatBox animation_bounds;
        bool success =
            transform_animation_curve->animatedBoundsForBox(box, &animation_bounds);
        if (!success)
            return false;
        bounds->Union(animation_bounds);
    }

    return true;
}

bool LayerAnimationController::hasAnimationThatAffectsScale() const 
{
    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (m_animations[i]->isFinished() || m_animations[i]->getTargetProperty() != AnimationObj::TRANSFORM)
            continue;

        const TransformAnimationCurve* transform_animation_curve = m_animations[i]->curve()->toTransformAnimationCurve();
        if (transform_animation_curve->affectsScale())
            return true;
    }

    return false;
}

bool LayerAnimationController::hasOnlyTranslationTransforms(ObserverType observer_type) const 
{
    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (m_animations[i]->isFinished() || m_animations[i]->getTargetProperty() != AnimationObj::TRANSFORM)
            continue;

        if ((observer_type == ObserverType::ACTIVE && !m_animations[i]->affectsActiveObservers()) ||
            (observer_type == ObserverType::PENDING && !m_animations[i]->affectsPendingObservers()))
            continue;

        const TransformAnimationCurve* transform_animation_curve = m_animations[i]->curve()->toTransformAnimationCurve();
        if (!transform_animation_curve->isTranslation())
            return false;
    }

    return true;
}

bool LayerAnimationController::animationsPreserveAxisAlignment() const
{
    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (m_animations[i]->isFinished() ||
            m_animations[i]->getTargetProperty() != AnimationObj::TRANSFORM)
            continue;

        const TransformAnimationCurve* transform_animation_curve = m_animations[i]->curve()->toTransformAnimationCurve();
        if (!transform_animation_curve->preservesAxisAlignment())
            return false;
    }

    return true;
}

bool LayerAnimationController::animationStartScale(ObserverType observer_type, float* start_scale) const
{
    *start_scale = 0.f;
    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (m_animations[i]->isFinished() ||
            m_animations[i]->getTargetProperty() != AnimationObj::TRANSFORM)
            continue;

        if ((observer_type == ObserverType::ACTIVE && !m_animations[i]->affectsActiveObservers()) ||
            (observer_type == ObserverType::PENDING && !m_animations[i]->affectsPendingObservers()))
            continue;

        bool forward_direction = true;
        switch (m_animations[i]->direction()) {
        case AnimationObj::DIRECTION_NORMAL:
        case AnimationObj::DIRECTION_ALTERNATE:
            forward_direction = m_animations[i]->playbackRate() >= 0.0;
            break;
        case AnimationObj::DIRECTION_REVERSE:
        case AnimationObj::DIRECTION_ALTERNATE_REVERSE:
            forward_direction = m_animations[i]->playbackRate() < 0.0;
            break;
        }

        const TransformAnimationCurve* transform_animation_curve = m_animations[i]->curve()->toTransformAnimationCurve();
        float animation_start_scale = 0.f;
        if (!transform_animation_curve->animationStartScale(forward_direction, &animation_start_scale))
            return false;
        *start_scale = std::max(*start_scale, animation_start_scale);
    }
    return true;
}

bool LayerAnimationController::maximumTargetScale(ObserverType observer_type, float* max_scale) const
{
    *max_scale = 0.f;
    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (m_animations[i]->isFinished() || m_animations[i]->getTargetProperty() != AnimationObj::TRANSFORM)
            continue;

        if ((observer_type == ObserverType::ACTIVE && !m_animations[i]->affectsActiveObservers()) ||
            (observer_type == ObserverType::PENDING && !m_animations[i]->affectsPendingObservers()))
            continue;

        bool forward_direction = true;
        switch (m_animations[i]->direction()) {
        case AnimationObj::DIRECTION_NORMAL:
        case AnimationObj::DIRECTION_ALTERNATE:
            forward_direction = m_animations[i]->playbackRate() >= 0.0;
            break;
        case AnimationObj::DIRECTION_REVERSE:
        case AnimationObj::DIRECTION_ALTERNATE_REVERSE:
            forward_direction = m_animations[i]->playbackRate() < 0.0;
            break;
        }

        const TransformAnimationCurve* transform_animation_curve =
            m_animations[i]->curve()->toTransformAnimationCurve();
        float animation_scale = 0.f;
        if (!transform_animation_curve->maximumTargetScale(forward_direction, &animation_scale))
            return false;
        *max_scale = std::max(*max_scale, animation_scale);
    }

    return true;
}

void LayerAnimationController::pushNewAnimationsToImplThread(LayerAnimationController* controllerImpl) const
{
    // Any new animations owned by the main thread's controller are cloned and
    // add to the impl thread's controller.
    for (size_t i = 0; i < m_animations.size(); ++i) {
        // If the animation is already running on the impl thread, there is no
        // need to copy it over.
        if (controllerImpl->getAnimationById(m_animations[i]->id()))
            continue;

        // Scroll animations always start at the current scroll offset.
        if (m_animations[i]->getTargetProperty() == AnimationObj::SCROLL_OFFSET) {
            blink::FloatPoint current_scroll_offset;
            if (controllerImpl->m_valueProvider) {
                current_scroll_offset = controllerImpl->m_valueProvider->scrollOffsetForAnimation();
            } else {
                // The owning layer isn't yet in the active tree, so the main thread
                // scroll offset will be up-to-date.
                current_scroll_offset = m_valueProvider->scrollOffsetForAnimation();
            }
            m_animations[i]->curve()->toScrollOffsetAnimationCurve()->setInitialValue(current_scroll_offset);
        }

        // The new animation should be set to run as soon as possible.
        AnimationObj::RunState initial_run_state = AnimationObj::WAITING_FOR_TARGET_AVAILABILITY;
        WTF::PassOwnPtr<AnimationObj> to_add(m_animations[i]->cloneAndInitialize(initial_run_state));
        ASSERT(!to_add->needsSynchronizedStartTime());
        to_add->setAffectsActiveObservers(false);
        controllerImpl->addAnimation(to_add);
    }
}

static bool isCompleted(AnimationObj* animation, const LayerAnimationController* main_thread_controller) 
{
    if (animation->isImplOnly()) {
        return (animation->runState() == AnimationObj::WAITING_FOR_DELETION);
    } else {
        return !main_thread_controller->getAnimationById(animation->id());
    }
}

static bool affectsActiveOnlyAndIsWaitingForDeletion(AnimationObj* animation)
{
    return animation->runState() == AnimationObj::WAITING_FOR_DELETION && !animation->affectsPendingObservers();
}

void LayerAnimationController::removeAnimationsCompletedOnMainThread(LayerAnimationController* controllerImpl) const
{
    bool removedTransformAnimation = false;
    // Animations removed on the main thread should no longer affect pending
    // observers, and should stop affecting active observers after the next call
    // to activateAnimations. If already WAITING_FOR_DELETION, they can be removed
    // immediately.
    ScopedPtrVector<AnimationObj>& animations = controllerImpl->m_animations;
    for (size_t i = 0; i < animations.size(); ++i) {
        if (isCompleted(animations[i], this)) {
            animations[i]->setAffectsPendingObservers(false);
            if (animations[i]->getTargetProperty() == AnimationObj::TRANSFORM)
                removedTransformAnimation = true;
        }
    }
    animations.erase(remove_if(&animations, animations.begin(), animations.end(), affectsActiveOnlyAndIsWaitingForDeletion), animations.end());

    if (removedTransformAnimation)
        controllerImpl->updatePotentiallyAnimatingTransform();
}

void LayerAnimationController::pushPropertiesToImplThread(
    LayerAnimationController* controllerImpl) {
    for (size_t i = 0; i < m_animations.size(); ++i) {
        AnimationObj* current_impl = controllerImpl->getAnimationById(m_animations[i]->id());
        if (current_impl)
            m_animations[i]->pushPropertiesTo(current_impl);
    }
    controllerImpl->m_scrollOffsetAnimationWasInterrupted = m_scrollOffsetAnimationWasInterrupted;
    m_scrollOffsetAnimationWasInterrupted = false;
}

void LayerAnimationController::startAnimations(base::TimeTicks monotonicTime) 
{
    ASSERT(m_needsToStartAnimations);
    m_needsToStartAnimations = false;
    // First collect running properties affecting each type of observer.
    TargetProperties blockedPropertiesForActiveObservers;
    TargetProperties blockedPropertiesForPendingObservers;
    std::vector<size_t> animationsWaitingForTarget;

    animationsWaitingForTarget.reserve(m_animations.size());
    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (m_animations[i]->runState() == AnimationObj::STARTING || m_animations[i]->runState() == AnimationObj::RUNNING) {
            if (m_animations[i]->affectsActiveObservers()) {
                blockedPropertiesForActiveObservers.insert(m_animations[i]->getTargetProperty());
            }
            if (m_animations[i]->affectsPendingObservers()) {
                blockedPropertiesForPendingObservers.insert(m_animations[i]->getTargetProperty());
            }
        } else if (m_animations[i]->runState() == AnimationObj::WAITING_FOR_TARGET_AVAILABILITY) {
            animationsWaitingForTarget.push_back(i);
        }
    }

    for (size_t i = 0; i < animationsWaitingForTarget.size(); ++i) {
        // Collect all properties for animations with the same group id (they
        // should all also be in the list of animations).
        size_t animationIndex = animationsWaitingForTarget[i];
        AnimationObj* animationWaitingForTarget = m_animations[animationIndex];

        // Check for the run state again even though the animation was waiting
        // for target because it might have changed the run state while handling
        // previous animation in this loop (if they belong to same group).
        if (animationWaitingForTarget->runState() == AnimationObj::WAITING_FOR_TARGET_AVAILABILITY) {
            TargetProperties enqueuedProperties;
            bool affectsActiveObservers = animationWaitingForTarget->affectsActiveObservers();
            bool affectsPendingObservers = animationWaitingForTarget->affectsPendingObservers();
            enqueuedProperties.insert(animationWaitingForTarget->getTargetProperty());

            for (size_t j = animationIndex + 1; j < m_animations.size(); ++j) {
                if (animationWaitingForTarget->group() == m_animations[j]->group()) {
                    enqueuedProperties.insert(m_animations[j]->getTargetProperty());
                    affectsActiveObservers |= m_animations[j]->affectsActiveObservers();
                    affectsPendingObservers |= m_animations[j]->affectsPendingObservers();
                }
            }

            // Check to see if intersection of the list of properties affected by
            // the group and the list of currently blocked properties is null, taking
            // into account the type(s) of observers affected by the group. In any
            // case, the group's target properties need to be added to the lists of
            // blocked properties.
            bool nullIntersection = true;
            for (TargetProperties::iterator p_iter = enqueuedProperties.begin(); p_iter != enqueuedProperties.end(); ++p_iter) {
//                 if (affectsActiveObservers && !blockedPropertiesForActiveObservers.add(*p_iter).second)
//                     nullIntersection = false;
//                 if (affectsPendingObservers && !blockedPropertiesForPendingObservers.add(*p_iter).second)
//                     nullIntersection = false;
                int temp = *p_iter;
                blockedPropertiesForActiveObservers.insert(temp);
                blockedPropertiesForPendingObservers.insert(temp);
            }

            // If the intersection is null, then we are free to start the animations
            // in the group.
            if (nullIntersection) {
                animationWaitingForTarget->setRunState(AnimationObj::STARTING, monotonicTime);
                for (size_t j = animationIndex + 1; j < m_animations.size(); ++j) {
                    if (animationWaitingForTarget->group() == m_animations[j]->group()) {
                        m_animations[j]->setRunState(AnimationObj::STARTING, monotonicTime);
                    }
                }
            } else {
                m_needsToStartAnimations = true;
            }
        }
    }
}

void LayerAnimationController::promoteStartedAnimations(base::TimeTicks monotonicTime, AnimationEventsVector* events) 
{
    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (m_animations[i]->runState() == AnimationObj::STARTING && m_animations[i]->affectsActiveObservers()) {
            m_animations[i]->setRunState(AnimationObj::RUNNING, monotonicTime);
            if (!m_animations[i]->hasSetStartTime() && !m_animations[i]->needsSynchronizedStartTime())
                m_animations[i]->setStartTime(monotonicTime);

            if (events) {
                base::TimeTicks startTime;
                if (m_animations[i]->hasSetStartTime())
                    startTime = m_animations[i]->startTime();
                else
                    startTime = monotonicTime;
                AnimationEvent startedEvent(AnimationEvent::STARTED, m_id, m_animations[i]->group(), m_animations[i]->getTargetProperty(), startTime);
                startedEvent.isImplOnly = m_animations[i]->isImplOnly();
                if (startedEvent.isImplOnly)
                    notifyAnimationStarted(startedEvent);
                else
                    events->push_back(startedEvent);
            }
        }
    }
}

void LayerAnimationController::markFinishedAnimations(base::TimeTicks monotonicTime)
{
    bool finishedTransFormAnimation = false;
    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (!m_animations[i]->isFinished() && m_animations[i]->isFinishedAt(monotonicTime)) {
            m_animations[i]->setRunState(AnimationObj::FINISHED, monotonicTime);
            if (m_animations[i]->getTargetProperty() == AnimationObj::TRANSFORM) {
                finishedTransFormAnimation = true;
            }
        }
    }
    if (finishedTransFormAnimation)
        updatePotentiallyAnimatingTransform();
}

void LayerAnimationController::markAnimationsForDeletion(base::TimeTicks monotonicTime, AnimationEventsVector* events) 
{
    bool markedAnimationsForDeletions = false;
    std::vector<size_t> animationsWithAameGroupId;

    animationsWithAameGroupId.reserve(m_animations.size());
    // Non-aborted animations are marked for deletion after a corresponding
    // AnimationEvent::FINISHED event is sent or received. This means that if
    // we don't have an events vector, we must ensure that non-aborted animations
    // have received a finished event before marking them for deletion.
    for (size_t i = 0; i < m_animations.size(); i++) {
        int groupId = m_animations[i]->group();
        if (m_animations[i]->runState() == AnimationObj::ABORTED) {
            if (events && !m_animations[i]->isImplOnly()) {
                AnimationEvent aborted_event(AnimationEvent::ABORTED, m_id, groupId, m_animations[i]->getTargetProperty(), monotonicTime);
                events->push_back(aborted_event);
            }

            m_animations[i]->setRunState(AnimationObj::WAITING_FOR_DELETION, monotonicTime);
            markedAnimationsForDeletions = true;
            continue;
        }

        bool allAnimsWithSameIdAreFinished = false;

        // Since deleting an animation on the main thread leads to its deletion
        // on the impl thread, we only mark a FINISHED main thread animation for
        // deletion once it has received a FINISHED event from the impl thread.
        bool animationIWillSendOrHasReceivedFinishEvent =
            m_animations[i]->isControllingInstance() ||
            m_animations[i]->isImplOnly() ||
            m_animations[i]->receivedFinishedEvent();

        // If an animation is finished, and not already marked for deletion,
        // find out if all other animations in the same group are also finished.
        if (m_animations[i]->runState() == AnimationObj::FINISHED && animationIWillSendOrHasReceivedFinishEvent) {
            // Clear the animationsWithAameGroupId if it was added for
            // the previous animation's iteration.
            if (animationsWithAameGroupId.size() > 0)
                animationsWithAameGroupId.clear();
            allAnimsWithSameIdAreFinished = true;

            for (size_t j = 0; j < m_animations.size(); ++j) {
                bool animationJWillSendOrHasReceivedFinishEvent =
                    m_animations[j]->isControllingInstance() ||
                    m_animations[j]->isImplOnly() ||
                    m_animations[j]->receivedFinishedEvent();

                if (groupId == m_animations[j]->group()) {
                    if (!m_animations[j]->isFinished() || (m_animations[j]->runState() == AnimationObj::FINISHED && !animationJWillSendOrHasReceivedFinishEvent)) {
                        allAnimsWithSameIdAreFinished = false;
                        break;
                    } else if (j >= i && m_animations[j]->runState() != AnimationObj::ABORTED) {
                        // Mark down the animations which belong to the same group
                        // and is not yet aborted. If this current iteration finds that all
                        // animations with same ID are finished, then the marked
                        // animations below will be set to WAITING_FOR_DELETION in next
                        // iteration.
                        animationsWithAameGroupId.push_back(j);
                    }
                }
            }
        }

        if (allAnimsWithSameIdAreFinished) {
            // We now need to remove all animations with the same group id as
            // groupId (and send along animation finished notifications, if
            // necessary).
            for (size_t j = 0; j < animationsWithAameGroupId.size(); j++) {
                size_t animationIndex = animationsWithAameGroupId[j];
                if (events) {
                    AnimationEvent finishedEvent(
                        AnimationEvent::FINISHED, m_id,
                        m_animations[animationIndex]->group(),
                        m_animations[animationIndex]->getTargetProperty(),
                        monotonicTime);
                    finishedEvent.isImplOnly = m_animations[animationIndex]->isImplOnly();
                    if (finishedEvent.isImplOnly)
                        notifyAnimationFinished(finishedEvent);
                    else
                        events->push_back(finishedEvent);
                }
                m_animations[animationIndex]->setRunState(AnimationObj::WAITING_FOR_DELETION, monotonicTime);
            }
            markedAnimationsForDeletions = true;
        }
    }

    if (markedAnimationsForDeletions)
        notifyObserversAnimationWaitingForDeletion();
}

static bool isWaitingForDeletion(AnimationObj* animation) 
{
    return animation->runState() == AnimationObj::WAITING_FOR_DELETION;
}

void LayerAnimationController::purgeAnimationsMarkedForDeletion()
{
    m_animations.erase(remove_if(&m_animations, m_animations.begin(), m_animations.end(), isWaitingForDeletion), m_animations.end());
}

void LayerAnimationController::tickAnimations(base::TimeTicks monotonicTime)
{
    for (size_t i = 0; i < m_animations.size(); ++i) {
        if (m_animations[i]->runState() == AnimationObj::STARTING ||
            m_animations[i]->runState() == AnimationObj::RUNNING ||
            m_animations[i]->runState() == AnimationObj::PAUSED) {
            if (!m_animations[i]->inEffect(monotonicTime))
                continue;

            base::TimeDelta trimmed = m_animations[i]->trimTimeToCurrentIteration(monotonicTime);

            switch (m_animations[i]->getTargetProperty()) {
                case AnimationObj::TRANSFORM: {
                    const TransformAnimationCurve* transformAnimationCurve = m_animations[i]->curve()->toTransformAnimationCurve();
                    SkMatrix44 transform = transformAnimationCurve->getValue(trimmed);
                    notifyObserversTransformAnimated(
                        transform,
                        m_animations[i]->affectsActiveObservers(),
                        m_animations[i]->affectsPendingObservers());
                    break;
                }

                case AnimationObj::OPACITY: {
                    const FloatAnimationCurve* floatAnimationCurve = m_animations[i]->curve()->toFloatAnimationCurve();
                    const float opacity = std::max(std::min(floatAnimationCurve->getValue(trimmed), 1.0f), 0.f);
                    notifyObserversOpacityAnimated(opacity, m_animations[i]->affectsActiveObservers(), m_animations[i]->affectsPendingObservers());
                    break;
                }

                case AnimationObj::FILTER: {
                    const FilterAnimationCurve* filterAnimationCurve = m_animations[i]->curve()->toFilterAnimationCurve();
                    const FilterOperationsWrap filter = filterAnimationCurve->getValue(trimmed);
                    notifyObserversFilterAnimated(filter, m_animations[i]->affectsActiveObservers(), m_animations[i]->affectsPendingObservers());
                    break;
                }

                case AnimationObj::BACKGROUND_COLOR: {
                    // Not yet implemented.
                    break;
                }

                case AnimationObj::SCROLL_OFFSET: {
                    const ScrollOffsetAnimationCurve* scroll_offset_animation_curve = m_animations[i]->curve()->toScrollOffsetAnimationCurve();
                    const blink::FloatPoint scroll_offset = scroll_offset_animation_curve->getValue(trimmed);
                    notifyObserversScrollOffsetAnimated(scroll_offset, m_animations[i]->affectsActiveObservers(), m_animations[i]->affectsPendingObservers());
                    break;
                }
            }
        }
    }
}

void LayerAnimationController::updateActivation(UpdateActivationType type)
{
//     bool force = type == FORCE_ACTIVATION;
//     if (m_registrar) {
//         bool was_active = m_isActive;
//         m_isActive = false;
//         for (size_t i = 0; i < m_animations.size(); ++i) {
//             if (m_animations[i]->runState() != AnimationObj::WAITING_FOR_DELETION) {
//                 m_isActive = true;
//                 break;
//             }
//         }
// 
//         if (m_isActive && (!was_active || force))
//             m_registrar->didActivateAnimationController(this);
//         else if (!m_isActive && (was_active || force))
//             m_registrar->didDeactivateAnimationController(this);
//     }
}

void LayerAnimationController::notifyObserversOpacityAnimated(float opacity, bool notify_active_observers, bool notify_pending_observers)
{
    mc_blink::WebLayerImpl* layer = m_host->getLayerById(m_id);
    if (layer)
        layer->setOpacity(opacity);

//     if (value_observers_.might_have_observers()) {
//         base::ObserverListBase<LayerAnimationValueObserver>::Iterator it(
//             &value_observers_);
//         LayerAnimationValueObserver* obs;
//         while ((obs = it.GetNext()) != nullptr) {
//             if ((notify_active_observers && notify_pending_observers) ||
//                 (notify_active_observers && obs->IsActive()) ||
//                 (notify_pending_observers && !obs->IsActive()))
//                 obs->OnOpacityAnimated(opacity);
//         }
//     }
}

void LayerAnimationController::notifyObserversTransformAnimated(const SkMatrix44& transform, bool notify_active_observers, bool notify_pending_observers) 
{
    mc_blink::WebLayerImpl* layer = m_host->getLayerById(m_id);
    if (layer)
        layer->setTransform(transform);

//     if (value_observers_.might_have_observers()) {
//         base::ObserverListBase<LayerAnimationValueObserver>::Iterator it(
//             &value_observers_);
//         LayerAnimationValueObserver* obs;
//         while ((obs = it.GetNext()) != nullptr) {
//             if ((notify_active_observers && notify_pending_observers) ||
//                 (notify_active_observers && obs->IsActive()) ||
//                 (notify_pending_observers && !obs->IsActive()))
//                 obs->OnTransformAnimated(transform);
//         }
//     }
}

void LayerAnimationController::notifyObserversFilterAnimated(const FilterOperationsWrap& filters, bool notify_active_observers, bool notify_pending_observers)
{
//     if (value_observers_.might_have_observers()) {
//         base::ObserverListBase<LayerAnimationValueObserver>::Iterator it(
//             &value_observers_);
//         LayerAnimationValueObserver* obs;
//         while ((obs = it.GetNext()) != nullptr) {
//             if ((notify_active_observers && notify_pending_observers) ||
//                 (notify_active_observers && obs->IsActive()) ||
//                 (notify_pending_observers && !obs->IsActive()))
//                 obs->OnFilterAnimated(filters);
//         }
//     }
    mc_blink::WebLayerImpl* layer = m_host->getLayerById(m_id);
    if (!layer)
        return;
    layer->setFilters(mc_blink::WebFilterOperationsImpl(filters));
}

void LayerAnimationController::notifyObserversScrollOffsetAnimated(const blink::FloatPoint& scroll_offset, bool notify_active_observers, bool notify_pending_observers)
{
//     if (value_observers_.might_have_observers()) {
//         base::ObserverListBase<LayerAnimationValueObserver>::Iterator it(
//             &value_observers_);
//         LayerAnimationValueObserver* obs;
//         while ((obs = it.GetNext()) != nullptr) {
//             if ((notify_active_observers && notify_pending_observers) ||
//                 (notify_active_observers && obs->IsActive()) ||
//                 (notify_pending_observers && !obs->IsActive()))
//                 obs->OnScrollOffsetAnimated(scroll_offset);
//         }
//     }
    DebugBreak();
}

void LayerAnimationController::notifyObserversAnimationWaitingForDeletion()
{
//     FOR_EACH_OBSERVER(LayerAnimationValueObserver,
//         value_observers_,
//         OnAnimationWaitingForDeletion());
    DebugBreak();
}

void LayerAnimationController::notifyObserversTransformIsPotentiallyAnimatingChanged(bool notify_active_observers, bool notify_pending_observers) 
{
//     if (value_observers_.might_have_observers()) {
//         base::ObserverListBase<LayerAnimationValueObserver>::Iterator it(
//             &value_observers_);
//         LayerAnimationValueObserver* obs;
//         while ((obs = it.GetNext()) != nullptr) {
//             if (notify_active_observers && obs->IsActive())
//                 obs->OnTransformIsPotentiallyAnimatingChanged(
//                     m_potentiallyAnimatingTransformForActiveObservers);
//             else if (notify_pending_observers && !obs->IsActive())
//                 obs->OnTransformIsPotentiallyAnimatingChanged(
//                     m_potentiallyAnimatingTransformForPendingObservers);
//         }
//     }
}

bool LayerAnimationController::hasValueObserver() 
{
//     if (value_observers_.might_have_observers()) {
//         base::ObserverListBase<LayerAnimationValueObserver>::Iterator it(
//             &value_observers_);
//         return it.GetNext() != nullptr;
//     }

    return true;
}

bool LayerAnimationController::hasActiveValueObserver()
{
//     if (value_observers_.might_have_observers()) {
//         base::ObserverListBase<LayerAnimationValueObserver>::Iterator it(
//             &value_observers_);
//         LayerAnimationValueObserver* obs;
//         while ((obs = it.GetNext()) != nullptr)
//             if (obs->IsActive())
//                 return true;
//     }

    return true;
}

}  // namespace cc
