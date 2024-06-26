// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_ANIMATION_LAYER_ANIMATION_CONTROLLER_H_
#define CC_ANIMATION_LAYER_ANIMATION_CONTROLLER_H_

#include "base/basictypes.h"
#include "base/containers/hash_tables.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/observer_list.h"
#include "base/time/time.h"
#include "cc/animation/animation_events.h"
#include "cc/animation/layer_animation_event_observer.h"
#include "cc/base/cc_export.h"
#include "cc/base/scoped_ptr_vector.h"
#include "ui/gfx/geometry/scroll_offset.h"
#include "ui/gfx/transform.h"

namespace gfx {
class BoxF;
class Transform;
}

namespace cc {

class Animation;
class AnimationDelegate;
class AnimationRegistrar;
class FilterOperations;
class KeyframeValueList;
class LayerAnimationValueObserver;
class LayerAnimationValueProvider;

class CC_EXPORT LayerAnimationController
    : public base::RefCounted<LayerAnimationController> {
public:
    enum class ObserverType { ACTIVE,
        PENDING };

    static scoped_refptr<LayerAnimationController> Create(int id);

    int id() const { return id_; }

    void AddAnimation(scoped_ptr<Animation> animation);
    void PauseAnimation(int animation_id, base::TimeDelta time_offset);
    void RemoveAnimation(int animation_id);
    void RemoveAnimation(int animation_id,
        Animation::TargetProperty target_property);
    void AbortAnimations(Animation::TargetProperty target_property);

    // Ensures that the list of active animations on the main thread and the impl
    // thread are kept in sync. This function does not take ownership of the impl
    // thread controller. This method is virtual for testing.
    virtual void PushAnimationUpdatesTo(
        LayerAnimationController* controller_impl);

    void Animate(base::TimeTicks monotonic_time);
    void AccumulatePropertyUpdates(base::TimeTicks monotonic_time,
        AnimationEventsVector* events);

    void UpdateState(bool start_ready_animations,
        AnimationEventsVector* events);

    // Make animations affect active observers if and only if they affect
    // pending observers. Any animations that no longer affect any observers
    // are deleted.
    void ActivateAnimations();

    // Returns the active animation animating the given property that is either
    // running, or is next to run, if such an animation exists.
    Animation* GetAnimation(Animation::TargetProperty target_property) const;

    // Returns the active animation for the given unique animation id.
    Animation* GetAnimationById(int animation_id) const;

    // Returns true if there are any animations that have neither finished nor
    // aborted.
    bool HasActiveAnimation() const;

    // Returns true if there are any animations at all to process.
    bool has_any_animation() const { return !animations_.empty(); }

    // Returns true if there is an animation that is either currently animating
    // the given property or scheduled to animate this property in the future, and
    // that affects the given observer type.
    bool IsPotentiallyAnimatingProperty(Animation::TargetProperty target_property,
        ObserverType observer_type) const;

    // Returns true if there is an animation that is currently animating the given
    // property and that affects the given observer type.
    bool IsCurrentlyAnimatingProperty(Animation::TargetProperty target_property,
        ObserverType observer_type) const;

    void SetAnimationRegistrar(AnimationRegistrar* registrar);
    AnimationRegistrar* animation_registrar() { return registrar_; }

    void NotifyAnimationStarted(const AnimationEvent& event);
    void NotifyAnimationFinished(const AnimationEvent& event);
    void NotifyAnimationAborted(const AnimationEvent& event);
    void NotifyAnimationPropertyUpdate(const AnimationEvent& event);

    void AddValueObserver(LayerAnimationValueObserver* observer);
    void RemoveValueObserver(LayerAnimationValueObserver* observer);

    void AddEventObserver(LayerAnimationEventObserver* observer);
    void RemoveEventObserver(LayerAnimationEventObserver* observer);

    void set_value_provider(LayerAnimationValueProvider* provider)
    {
        value_provider_ = provider;
    }

    void remove_value_provider(LayerAnimationValueProvider* provider)
    {
        if (value_provider_ == provider)
            value_provider_ = nullptr;
    }

    void set_layer_animation_delegate(AnimationDelegate* delegate)
    {
        layer_animation_delegate_ = delegate;
    }

    void remove_layer_animation_delegate(AnimationDelegate* delegate)
    {
        if (layer_animation_delegate_ == delegate)
            layer_animation_delegate_ = nullptr;
    }

    bool HasFilterAnimationThatInflatesBounds() const;
    bool HasTransformAnimationThatInflatesBounds() const;
    bool HasAnimationThatInflatesBounds() const
    {
        return HasTransformAnimationThatInflatesBounds() || HasFilterAnimationThatInflatesBounds();
    }

    bool FilterAnimationBoundsForBox(const gfx::BoxF& box,
        gfx::BoxF* bounds) const;
    bool TransformAnimationBoundsForBox(const gfx::BoxF& box,
        gfx::BoxF* bounds) const;

    bool HasAnimationThatAffectsScale() const;

    bool HasOnlyTranslationTransforms(ObserverType observer_type) const;

    bool AnimationsPreserveAxisAlignment() const;

    // Sets |start_scale| to the maximum of starting animation scale along any
    // dimension at any destination in active animations. Returns false if the
    // starting scale cannot be computed.
    bool AnimationStartScale(ObserverType observer_type,
        float* start_scale) const;

    // Sets |max_scale| to the maximum scale along any dimension at any
    // destination in active animations. Returns false if the maximum scale cannot
    // be computed.
    bool MaximumTargetScale(ObserverType event_observers_,
        float* max_scale) const;

    // When a scroll animation is removed on the main thread, its compositor
    // thread counterpart continues producing scroll deltas until activation.
    // These scroll deltas need to be cleared at activation, so that the active
    // layer's scroll offset matches the offset provided by the main thread
    // rather than a combination of this offset and scroll deltas produced by
    // the removed animation. This is to provide the illusion of synchronicity to
    // JS that simultaneously removes an animation and sets the scroll offset.
    bool scroll_offset_animation_was_interrupted() const
    {
        return scroll_offset_animation_was_interrupted_;
    }

    bool needs_to_start_animations_for_testing()
    {
        return needs_to_start_animations_;
    }

protected:
    friend class base::RefCounted<LayerAnimationController>;

    explicit LayerAnimationController(int id);
    virtual ~LayerAnimationController();

private:
    typedef base::hash_set<int> TargetProperties;

    void PushNewAnimationsToImplThread(
        LayerAnimationController* controller_impl) const;
    void RemoveAnimationsCompletedOnMainThread(
        LayerAnimationController* controller_impl) const;
    void PushPropertiesToImplThread(LayerAnimationController* controller_impl);

    void StartAnimations(base::TimeTicks monotonic_time);
    void PromoteStartedAnimations(base::TimeTicks monotonic_time,
        AnimationEventsVector* events);
    void MarkFinishedAnimations(base::TimeTicks monotonic_time);
    void MarkAnimationsForDeletion(base::TimeTicks monotonic_time,
        AnimationEventsVector* events);
    void PurgeAnimationsMarkedForDeletion();

    void TickAnimations(base::TimeTicks monotonic_time);

    enum UpdateActivationType { NORMAL_ACTIVATION,
        FORCE_ACTIVATION };
    void UpdateActivation(UpdateActivationType type);

    void NotifyObserversOpacityAnimated(float opacity,
        bool notify_active_observers,
        bool notify_pending_observers);
    void NotifyObserversTransformAnimated(const gfx::Transform& transform,
        bool notify_active_observers,
        bool notify_pending_observers);
    void NotifyObserversFilterAnimated(const FilterOperations& filter,
        bool notify_active_observers,
        bool notify_pending_observers);
    void NotifyObserversScrollOffsetAnimated(
        const gfx::ScrollOffset& scroll_offset,
        bool notify_active_observers,
        bool notify_pending_observers);

    void NotifyObserversAnimationWaitingForDeletion();

    void NotifyObserversTransformIsPotentiallyAnimatingChanged(
        bool notify_active_observers,
        bool notify_pending_observers);

    void UpdatePotentiallyAnimatingTransform();

    bool HasValueObserver();
    bool HasActiveValueObserver();

    AnimationRegistrar* registrar_;
    int id_;
    ScopedPtrVector<Animation> animations_;

    // This is used to ensure that we don't spam the registrar.
    bool is_active_;

    base::TimeTicks last_tick_time_;

    base::ObserverList<LayerAnimationValueObserver> value_observers_;
    base::ObserverList<LayerAnimationEventObserver> event_observers_;

    LayerAnimationValueProvider* value_provider_;

    AnimationDelegate* layer_animation_delegate_;

    // Only try to start animations when new animations are added or when the
    // previous attempt at starting animations failed to start all animations.
    bool needs_to_start_animations_;

    bool scroll_offset_animation_was_interrupted_;

    bool potentially_animating_transform_for_active_observers_;
    bool potentially_animating_transform_for_pending_observers_;

    DISALLOW_COPY_AND_ASSIGN(LayerAnimationController);
};

} // namespace cc

#endif // CC_ANIMATION_LAYER_ANIMATION_CONTROLLER_H_
