// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_animation_layerAnimationController_h
#define mc_animation_layerAnimationController_h


#include "mc/animation/AnimationEvents.h"
#include "mc/animation/LayerAnimationEventObserver.h"
#include "mc/base/ScopedPtrVector.h"
#include "third_party/WebKit/Source/wtf/RefCounted.h"
#include "third_party/WebKit/Source/wtf/HashSet.h"
#include "third_party/WebKit/Source/platform/geometry/FloatPoint.h"
#include "base/time/time.h"
#include <set>

class SkMatrix44;

namespace mc {

class AnimationObj;
class AnimationDelegate;
class AnimationRegistrar;
class KeyframeValueList;
class LayerAnimationValueObserver;
class LayerAnimationValueProvider;
class FloatBox;
class LayerTreeHost;
class FilterOperationsWrap;

class LayerAnimationController : public WTF::RefCounted<LayerAnimationController> {
public:
    enum class ObserverType {
        ACTIVE, PENDING
    };

    static WTF::PassRefPtr<LayerAnimationController> create(LayerTreeHost* host, int id);

    int id() const 
    {
        return m_id;
    }

    void destroy();

    void addAnimation(WTF::PassOwnPtr<AnimationObj> animation);
    void pauseAnimation(int animation_id, base::TimeDelta time_offset);
    void removeAnimation(int animation_id);
    void removeAnimation(int animation_id, AnimationObj::TargetProperty target_property);
    void abortAnimations(AnimationObj::TargetProperty target_property);

    // Ensures that the list of active animations on the main thread and the impl
    // thread are kept in sync. This function does not take ownership of the impl
    // thread controller. This method is virtual for testing.
    virtual void pushAnimationUpdatesTo(LayerAnimationController* controller_impl);

    void animate(base::TimeTicks monotonic_time);
    void accumulatePropertyUpdates(base::TimeTicks monotonic_time, AnimationEventsVector* events);

    void updateState(bool startReadyAnimations, AnimationEventsVector* events);

    // Make animations affect active observers if and only if they affect
    // pending observers. Any animations that no longer affect any observers
    // are deleted.
    void activateAnimations();

    // Returns the active animation animating the given property that is either
    // running, or is next to run, if such an animation exists.
    AnimationObj* getAnimation(AnimationObj::TargetProperty target_property) const;

    // Returns the active animation for the given unique animation id.
    AnimationObj* getAnimationById(int animation_id) const;

    // Returns true if there are any animations that have neither finished nor
    // aborted.
    bool hasActiveAnimation() const;

    // Returns true if there are any animations at all to process.
    bool hasAnyAnimation() const
    {
        return !m_animations.empty();
    }

    size_t getAnimationsSize() const
    {
        return m_animations.size();
    }

    // Returns true if there is an animation that is either currently animating
    // the given property or scheduled to animate this property in the future, and
    // that affects the given observer type.
    bool isPotentiallyAnimatingProperty(AnimationObj::TargetProperty target_property, ObserverType observer_type) const;

    // Returns true if there is an animation that is currently animating the given
    // property and that affects the given observer type.
    bool isCurrentlyAnimatingProperty(AnimationObj::TargetProperty target_property, ObserverType observer_type) const;

    void setAnimationRegistrar(AnimationRegistrar* registrar);
    AnimationRegistrar* getAnimationRegistrar() { return m_registrar; }

    void notifyAnimationStarted(const AnimationEvent& event);
    void notifyAnimationFinished(const AnimationEvent& event);
    void notifyAnimationAborted(const AnimationEvent& event);
    void notifyAnimationPropertyUpdate(const AnimationEvent& event);

    void addValueObserver(LayerAnimationValueObserver* observer);
    void removeValueObserver(LayerAnimationValueObserver* observer);

    void addEventObserver(LayerAnimationEventObserver* observer);
    void removeEventObserver(LayerAnimationEventObserver* observer);

    void setValueProvider(LayerAnimationValueProvider* provider)
    {
        m_valueProvider = provider;
    }

    void removeValueProvider(LayerAnimationValueProvider* provider)
    {
        if (m_valueProvider == provider)
            m_valueProvider = nullptr;
    }

    void setLayerAnimationDelegate(AnimationDelegate* delegate)
    {
        m_layerAnimationDelegate = delegate;
    }

    void removeLayerAnimationDelegate(AnimationDelegate* delegate)
    {
        if (m_layerAnimationDelegate == delegate)
            m_layerAnimationDelegate = nullptr;
    }

    bool hasFilterAnimationThatInflatesBounds() const;
    bool hasTransformAnimationThatInflatesBounds() const;
    bool hasAnimationThatInflatesBounds() const
    {
        return hasTransformAnimationThatInflatesBounds() || hasFilterAnimationThatInflatesBounds();
    }

    bool filterAnimationBoundsForBox(const FloatBox& box, FloatBox* bounds) const;
    bool transformAnimationBoundsForBox(const FloatBox& box, FloatBox* bounds) const;

    bool hasAnimationThatAffectsScale() const;

    bool hasOnlyTranslationTransforms(ObserverType observer_type) const;

    bool animationsPreserveAxisAlignment() const;

    // Sets |start_scale| to the maximum of starting animation scale along any
    // dimension at any destination in active animations. Returns false if the
    // starting scale cannot be computed.
    bool animationStartScale(ObserverType observer_type, float* start_scale) const;

    // Sets |max_scale| to the maximum scale along any dimension at any
    // destination in active animations. Returns false if the maximum scale cannot
    // be computed.
    bool maximumTargetScale(ObserverType event_observers, float* max_scale) const;

    // When a scroll animation is removed on the main thread, its compositor
    // thread counterpart continues producing scroll deltas until activation.
    // These scroll deltas need to be cleared at activation, so that the active
    // layer's scroll offset matches the offset provided by the main thread
    // rather than a combination of this offset and scroll deltas produced by
    // the removed animation. This is to provide the illusion of synchronicity to
    // JS that simultaneously removes an animation and sets the scroll offset.
    bool scrollOffsetAnimationWasInterrupted() const {
        return m_scrollOffsetAnimationWasInterrupted;
    }

    bool needsToStartAnimationsForTesting() {
        return m_needsToStartAnimations;
    }

protected:
    friend class WTF::RefCounted<LayerAnimationController>;

    explicit LayerAnimationController(LayerTreeHost* host, int id);
    virtual ~LayerAnimationController();

private:
    typedef std::set<int> TargetProperties;

    void pushNewAnimationsToImplThread(LayerAnimationController* controller_impl) const;
    void removeAnimationsCompletedOnMainThread(LayerAnimationController* controller_impl) const;
    void pushPropertiesToImplThread(LayerAnimationController* controller_impl);

    void startAnimations(base::TimeTicks monotonic_time);
    void promoteStartedAnimations(base::TimeTicks monotonic_time, AnimationEventsVector* events);
    void markFinishedAnimations(base::TimeTicks monotonic_time);
    void markAnimationsForDeletion(base::TimeTicks monotonic_time, AnimationEventsVector* events);
    void purgeAnimationsMarkedForDeletion();

    void tickAnimations(base::TimeTicks monotonic_time);

    enum UpdateActivationType {
        NORMAL_ACTIVATION, FORCE_ACTIVATION
    };
    void updateActivation(UpdateActivationType type);

    void notifyObserversOpacityAnimated(float opacity,
        bool notify_active_observers,
        bool notify_pending_observers);
    void notifyObserversTransformAnimated(const SkMatrix44& transform,
        bool notify_active_observers,
        bool notify_pending_observers);
    void notifyObserversFilterAnimated(const FilterOperationsWrap& filter,
        bool notify_active_observers,
        bool notify_pending_observers);
    void notifyObserversScrollOffsetAnimated(
        const blink::FloatPoint& scroll_offset,
        bool notify_active_observers,
        bool notify_pending_observers);

    void notifyObserversAnimationWaitingForDeletion();

    void notifyObserversTransformIsPotentiallyAnimatingChanged(
        bool notify_active_observers,
        bool notify_pending_observers);

    void updatePotentiallyAnimatingTransform();

    bool hasValueObserver();
    bool hasActiveValueObserver();

    AnimationRegistrar* m_registrar;
    int m_id;
    ScopedPtrVector<AnimationObj> m_animations;

    // This is used to ensure that we don't spam the registrar.
    bool m_isActive;

    base::TimeTicks m_lastTickTime;

//     base::ObserverList<LayerAnimationValueObserver> value_observers_;
//     base::ObserverList<LayerAnimationEventObserver> event_observers_;

    LayerAnimationValueProvider* m_valueProvider;

    AnimationDelegate* m_layerAnimationDelegate;

    // Only try to start animations when new animations are added or when the
    // previous attempt at starting animations failed to start all animations.
    bool m_needsToStartAnimations;

    bool m_scrollOffsetAnimationWasInterrupted;

    bool m_potentiallyAnimatingTransformForActiveObservers;
    bool m_potentiallyAnimatingTransformForPendingObservers;

    LayerTreeHost* m_host;
};

}  // namespace mc

#endif  // mc_animation_layerAnimationController_h
