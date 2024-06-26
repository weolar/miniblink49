// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/animation/layer_animation_controller.h"

#include <algorithm>
#include <vector>

#include "cc/animation/animation.h"
#include "cc/animation/animation_delegate.h"
#include "cc/animation/animation_registrar.h"
#include "cc/animation/keyframed_animation_curve.h"
#include "cc/animation/layer_animation_value_observer.h"
#include "cc/animation/layer_animation_value_provider.h"
#include "cc/animation/scroll_offset_animation_curve.h"
#include "cc/base/scoped_ptr_algorithm.h"
#include "cc/output/filter_operations.h"
#include "ui/gfx/geometry/box_f.h"
#include "ui/gfx/transform.h"

namespace cc {

LayerAnimationController::LayerAnimationController(int id)
    : registrar_(0)
    , id_(id)
    , is_active_(false)
    , value_provider_(nullptr)
    , layer_animation_delegate_(nullptr)
    , needs_to_start_animations_(false)
    , scroll_offset_animation_was_interrupted_(false)
    , potentially_animating_transform_for_active_observers_(false)
    , potentially_animating_transform_for_pending_observers_(false)
{
}

LayerAnimationController::~LayerAnimationController()
{
    if (registrar_)
        registrar_->UnregisterAnimationController(this);
}

scoped_refptr<LayerAnimationController> LayerAnimationController::Create(
    int id)
{
    return make_scoped_refptr(new LayerAnimationController(id));
}

void LayerAnimationController::PauseAnimation(int animation_id,
    base::TimeDelta time_offset)
{
    for (size_t i = 0; i < animations_.size(); ++i) {
        if (animations_[i]->id() == animation_id) {
            animations_[i]->SetRunState(Animation::PAUSED,
                time_offset + animations_[i]->start_time() + animations_[i]->time_offset());
        }
    }
}

struct HasAnimationId {
    explicit HasAnimationId(int id)
        : id_(id)
    {
    }
    bool operator()(Animation* animation) const
    {
        return animation->id() == id_;
    }

private:
    int id_;
};

void LayerAnimationController::UpdatePotentiallyAnimatingTransform()
{
    bool was_potentially_animating_transform_for_active_observers = potentially_animating_transform_for_active_observers_;
    bool was_potentially_animating_transform_for_pending_observers = potentially_animating_transform_for_pending_observers_;

    potentially_animating_transform_for_active_observers_ = false;
    potentially_animating_transform_for_pending_observers_ = false;

    for (Animation* animation : animations_) {
        if (!animation->is_finished() && animation->target_property() == Animation::TRANSFORM) {
            potentially_animating_transform_for_active_observers_ |= animation->affects_active_observers();
            potentially_animating_transform_for_pending_observers_ |= animation->affects_pending_observers();
        }
    }

    bool changed_for_active_observers = was_potentially_animating_transform_for_active_observers != potentially_animating_transform_for_active_observers_;
    bool changed_for_pending_observers = was_potentially_animating_transform_for_pending_observers != potentially_animating_transform_for_pending_observers_;

    if (!changed_for_active_observers && !changed_for_pending_observers)
        return;

    NotifyObserversTransformIsPotentiallyAnimatingChanged(
        changed_for_active_observers, changed_for_pending_observers);
}

void LayerAnimationController::RemoveAnimation(int animation_id)
{
    bool removed_transform_animation = false;
    auto animations_to_remove = animations_.remove_if(HasAnimationId(animation_id));
    for (auto it = animations_to_remove; it != animations_.end(); ++it) {
        if ((*it)->target_property() == Animation::SCROLL_OFFSET) {
            scroll_offset_animation_was_interrupted_ = true;
        } else if ((*it)->target_property() == Animation::TRANSFORM && !(*it)->is_finished()) {
            removed_transform_animation = true;
        }
    }

    animations_.erase(animations_to_remove, animations_.end());
    UpdateActivation(NORMAL_ACTIVATION);
    if (removed_transform_animation)
        UpdatePotentiallyAnimatingTransform();
}

struct HasAnimationIdAndProperty {
    HasAnimationIdAndProperty(int id, Animation::TargetProperty target_property)
        : id_(id)
        , target_property_(target_property)
    {
    }
    bool operator()(Animation* animation) const
    {
        return animation->id() == id_ && animation->target_property() == target_property_;
    }

private:
    int id_;
    Animation::TargetProperty target_property_;
};

void LayerAnimationController::RemoveAnimation(
    int animation_id,
    Animation::TargetProperty target_property)
{
    bool removed_transform_animation = false;
    auto animations_to_remove = animations_.remove_if(
        HasAnimationIdAndProperty(animation_id, target_property));
    if (animations_to_remove == animations_.end())
        return;

    if (target_property == Animation::SCROLL_OFFSET)
        scroll_offset_animation_was_interrupted_ = true;
    else if (target_property == Animation::TRANSFORM && !(*animations_to_remove)->is_finished())
        removed_transform_animation = true;

    animations_.erase(animations_to_remove, animations_.end());
    UpdateActivation(NORMAL_ACTIVATION);
    if (removed_transform_animation)
        UpdatePotentiallyAnimatingTransform();
}

void LayerAnimationController::AbortAnimations(
    Animation::TargetProperty target_property)
{
    bool aborted_transform_animation = false;
    for (size_t i = 0; i < animations_.size(); ++i) {
        if (animations_[i]->target_property() == target_property && !animations_[i]->is_finished()) {
            animations_[i]->SetRunState(Animation::ABORTED, last_tick_time_);
            if (target_property == Animation::TRANSFORM)
                aborted_transform_animation = true;
        }
    }
    if (aborted_transform_animation)
        UpdatePotentiallyAnimatingTransform();
}

// Ensures that the list of active animations on the main thread and the impl
// thread are kept in sync.
void LayerAnimationController::PushAnimationUpdatesTo(
    LayerAnimationController* controller_impl)
{
    DCHECK(this != controller_impl);
    if (!has_any_animation() && !controller_impl->has_any_animation())
        return;
    PurgeAnimationsMarkedForDeletion();
    PushNewAnimationsToImplThread(controller_impl);

    // Remove finished impl side animations only after pushing,
    // and only after the animations are deleted on the main thread
    // this insures we will never push an animation twice.
    RemoveAnimationsCompletedOnMainThread(controller_impl);

    PushPropertiesToImplThread(controller_impl);
    controller_impl->UpdateActivation(NORMAL_ACTIVATION);
    UpdateActivation(NORMAL_ACTIVATION);
}

void LayerAnimationController::Animate(base::TimeTicks monotonic_time)
{
    DCHECK(!monotonic_time.is_null());
    if (!HasValueObserver())
        return;

    if (needs_to_start_animations_)
        StartAnimations(monotonic_time);
    TickAnimations(monotonic_time);
    last_tick_time_ = monotonic_time;
}

void LayerAnimationController::AccumulatePropertyUpdates(
    base::TimeTicks monotonic_time,
    AnimationEventsVector* events)
{
    if (!events)
        return;

    for (size_t i = 0; i < animations_.size(); ++i) {
        Animation* animation = animations_[i];
        if (!animation->is_impl_only())
            continue;

        if (!animation->InEffect(monotonic_time))
            continue;

        base::TimeDelta trimmed = animation->TrimTimeToCurrentIteration(monotonic_time);
        switch (animation->target_property()) {
        case Animation::OPACITY: {
            AnimationEvent event(AnimationEvent::PROPERTY_UPDATE, id_,
                animation->group(), Animation::OPACITY,
                monotonic_time);
            const FloatAnimationCurve* float_animation_curve = animation->curve()->ToFloatAnimationCurve();
            event.opacity = float_animation_curve->GetValue(trimmed);
            event.is_impl_only = true;
            events->push_back(event);
            break;
        }

        case Animation::TRANSFORM: {
            AnimationEvent event(AnimationEvent::PROPERTY_UPDATE, id_,
                animation->group(), Animation::TRANSFORM,
                monotonic_time);
            const TransformAnimationCurve* transform_animation_curve = animation->curve()->ToTransformAnimationCurve();
            event.transform = transform_animation_curve->GetValue(trimmed);
            event.is_impl_only = true;
            events->push_back(event);
            break;
        }

        case Animation::FILTER: {
            AnimationEvent event(AnimationEvent::PROPERTY_UPDATE, id_,
                animation->group(), Animation::FILTER,
                monotonic_time);
            const FilterAnimationCurve* filter_animation_curve = animation->curve()->ToFilterAnimationCurve();
            event.filters = filter_animation_curve->GetValue(trimmed);
            event.is_impl_only = true;
            events->push_back(event);
            break;
        }

        case Animation::BACKGROUND_COLOR: {
            break;
        }

        case Animation::SCROLL_OFFSET: {
            // Impl-side changes to scroll offset are already sent back to the
            // main thread (e.g. for user-driven scrolling), so a PROPERTY_UPDATE
            // isn't needed.
            break;
        }
        }
    }
}

void LayerAnimationController::UpdateState(bool start_ready_animations,
    AnimationEventsVector* events)
{
    if (!HasActiveValueObserver())
        return;

    // Animate hasn't been called, this happens if an observer has been added
    // between the Commit and Draw phases.
    if (last_tick_time_ == base::TimeTicks())
        return;

    if (start_ready_animations)
        PromoteStartedAnimations(last_tick_time_, events);

    MarkFinishedAnimations(last_tick_time_);
    MarkAnimationsForDeletion(last_tick_time_, events);

    if (needs_to_start_animations_ && start_ready_animations) {
        StartAnimations(last_tick_time_);
        PromoteStartedAnimations(last_tick_time_, events);
    }

    AccumulatePropertyUpdates(last_tick_time_, events);

    UpdateActivation(NORMAL_ACTIVATION);
}

struct AffectsNoObservers {
    bool operator()(Animation* animation) const
    {
        return !animation->affects_active_observers() && !animation->affects_pending_observers();
    }
};

void LayerAnimationController::ActivateAnimations()
{
    bool changed_transform_animation = false;
    for (size_t i = 0; i < animations_.size(); ++i) {
        if (animations_[i]->affects_active_observers() != animations_[i]->affects_pending_observers() && animations_[i]->target_property() == Animation::TRANSFORM)
            changed_transform_animation = true;
        animations_[i]->set_affects_active_observers(
            animations_[i]->affects_pending_observers());
    }
    animations_.erase(cc::remove_if(&animations_,
                          animations_.begin(),
                          animations_.end(),
                          AffectsNoObservers()),
        animations_.end());
    scroll_offset_animation_was_interrupted_ = false;
    UpdateActivation(NORMAL_ACTIVATION);
    if (changed_transform_animation)
        UpdatePotentiallyAnimatingTransform();
}

void LayerAnimationController::AddAnimation(scoped_ptr<Animation> animation)
{
    bool added_transform_animation = animation->target_property() == Animation::TRANSFORM;
    animations_.push_back(animation.Pass());
    needs_to_start_animations_ = true;
    UpdateActivation(NORMAL_ACTIVATION);
    if (added_transform_animation)
        UpdatePotentiallyAnimatingTransform();
}

Animation* LayerAnimationController::GetAnimation(
    Animation::TargetProperty target_property) const
{
    for (size_t i = 0; i < animations_.size(); ++i) {
        size_t index = animations_.size() - i - 1;
        if (animations_[index]->target_property() == target_property)
            return animations_[index];
    }
    return 0;
}

Animation* LayerAnimationController::GetAnimationById(int animation_id) const
{
    for (size_t i = 0; i < animations_.size(); ++i)
        if (animations_[i]->id() == animation_id)
            return animations_[i];
    return nullptr;
}

bool LayerAnimationController::HasActiveAnimation() const
{
    for (size_t i = 0; i < animations_.size(); ++i) {
        if (!animations_[i]->is_finished())
            return true;
    }
    return false;
}

bool LayerAnimationController::IsPotentiallyAnimatingProperty(
    Animation::TargetProperty target_property,
    ObserverType observer_type) const
{
    for (size_t i = 0; i < animations_.size(); ++i) {
        if (!animations_[i]->is_finished() && animations_[i]->target_property() == target_property) {
            if ((observer_type == ObserverType::ACTIVE && animations_[i]->affects_active_observers()) || (observer_type == ObserverType::PENDING && animations_[i]->affects_pending_observers()))
                return true;
        }
    }
    return false;
}

bool LayerAnimationController::IsCurrentlyAnimatingProperty(
    Animation::TargetProperty target_property,
    ObserverType observer_type) const
{
    for (size_t i = 0; i < animations_.size(); ++i) {
        if (!animations_[i]->is_finished() && animations_[i]->InEffect(last_tick_time_) && animations_[i]->target_property() == target_property) {
            if ((observer_type == ObserverType::ACTIVE && animations_[i]->affects_active_observers()) || (observer_type == ObserverType::PENDING && animations_[i]->affects_pending_observers()))
                return true;
        }
    }
    return false;
}

void LayerAnimationController::SetAnimationRegistrar(
    AnimationRegistrar* registrar)
{
    if (registrar_ == registrar)
        return;

    if (registrar_)
        registrar_->UnregisterAnimationController(this);

    registrar_ = registrar;
    if (registrar_)
        registrar_->RegisterAnimationController(this);

    UpdateActivation(FORCE_ACTIVATION);
}

void LayerAnimationController::NotifyAnimationStarted(
    const AnimationEvent& event)
{
    if (event.is_impl_only) {
        FOR_EACH_OBSERVER(LayerAnimationEventObserver, event_observers_,
            OnAnimationStarted(event));
        if (layer_animation_delegate_)
            layer_animation_delegate_->NotifyAnimationStarted(
                event.monotonic_time, event.target_property, event.group_id);
        return;
    }

    for (size_t i = 0; i < animations_.size(); ++i) {
        if (animations_[i]->group() == event.group_id && animations_[i]->target_property() == event.target_property && animations_[i]->needs_synchronized_start_time()) {
            animations_[i]->set_needs_synchronized_start_time(false);
            if (!animations_[i]->has_set_start_time())
                animations_[i]->set_start_time(event.monotonic_time);

            FOR_EACH_OBSERVER(LayerAnimationEventObserver, event_observers_,
                OnAnimationStarted(event));
            if (layer_animation_delegate_)
                layer_animation_delegate_->NotifyAnimationStarted(
                    event.monotonic_time, event.target_property, event.group_id);

            return;
        }
    }
}

void LayerAnimationController::NotifyAnimationFinished(
    const AnimationEvent& event)
{
    if (event.is_impl_only) {
        if (layer_animation_delegate_)
            layer_animation_delegate_->NotifyAnimationFinished(
                event.monotonic_time, event.target_property, event.group_id);
        return;
    }

    for (size_t i = 0; i < animations_.size(); ++i) {
        if (animations_[i]->group() == event.group_id && animations_[i]->target_property() == event.target_property) {
            animations_[i]->set_received_finished_event(true);
            if (layer_animation_delegate_)
                layer_animation_delegate_->NotifyAnimationFinished(
                    event.monotonic_time, event.target_property, event.group_id);

            return;
        }
    }
}

void LayerAnimationController::NotifyAnimationAborted(
    const AnimationEvent& event)
{
    bool aborted_transform_animation = false;
    for (size_t i = 0; i < animations_.size(); ++i) {
        if (animations_[i]->group() == event.group_id && animations_[i]->target_property() == event.target_property) {
            animations_[i]->SetRunState(Animation::ABORTED, event.monotonic_time);
            if (event.target_property == Animation::TRANSFORM)
                aborted_transform_animation = true;
        }
    }
    if (aborted_transform_animation)
        UpdatePotentiallyAnimatingTransform();
}

void LayerAnimationController::NotifyAnimationPropertyUpdate(
    const AnimationEvent& event)
{
    bool notify_active_observers = true;
    bool notify_pending_observers = true;
    switch (event.target_property) {
    case Animation::OPACITY:
        NotifyObserversOpacityAnimated(
            event.opacity, notify_active_observers, notify_pending_observers);
        break;
    case Animation::TRANSFORM:
        NotifyObserversTransformAnimated(
            event.transform, notify_active_observers, notify_pending_observers);
        break;
    default:
        NOTREACHED();
    }
}

void LayerAnimationController::AddValueObserver(
    LayerAnimationValueObserver* observer)
{
    if (!value_observers_.HasObserver(observer))
        value_observers_.AddObserver(observer);
}

void LayerAnimationController::RemoveValueObserver(
    LayerAnimationValueObserver* observer)
{
    value_observers_.RemoveObserver(observer);
}

void LayerAnimationController::AddEventObserver(
    LayerAnimationEventObserver* observer)
{
    if (!event_observers_.HasObserver(observer))
        event_observers_.AddObserver(observer);
}

void LayerAnimationController::RemoveEventObserver(
    LayerAnimationEventObserver* observer)
{
    event_observers_.RemoveObserver(observer);
}

bool LayerAnimationController::HasFilterAnimationThatInflatesBounds() const
{
    for (size_t i = 0; i < animations_.size(); ++i) {
        if (!animations_[i]->is_finished() && animations_[i]->target_property() == Animation::FILTER && animations_[i]->curve()->ToFilterAnimationCurve()->HasFilterThatMovesPixels())
            return true;
    }

    return false;
}

bool LayerAnimationController::HasTransformAnimationThatInflatesBounds() const
{
    return IsCurrentlyAnimatingProperty(Animation::TRANSFORM,
               ObserverType::ACTIVE)
        || IsCurrentlyAnimatingProperty(Animation::TRANSFORM,
            ObserverType::PENDING);
}

bool LayerAnimationController::FilterAnimationBoundsForBox(
    const gfx::BoxF& box, gfx::BoxF* bounds) const
{
    // TODO(avallee): Implement.
    return false;
}

bool LayerAnimationController::TransformAnimationBoundsForBox(
    const gfx::BoxF& box,
    gfx::BoxF* bounds) const
{
    DCHECK(HasTransformAnimationThatInflatesBounds())
        << "TransformAnimationBoundsForBox will give incorrect results if there "
        << "are no transform animations affecting bounds, non-animated transform "
        << "is not known";

    // Compute bounds based on animations for which is_finished() is false.
    // Do nothing if there are no such animations; in this case, it is assumed
    // that callers will take care of computing bounds based on the owning layer's
    // actual transform.
    *bounds = gfx::BoxF();
    for (size_t i = 0; i < animations_.size(); ++i) {
        if (animations_[i]->is_finished() || animations_[i]->target_property() != Animation::TRANSFORM)
            continue;

        const TransformAnimationCurve* transform_animation_curve = animations_[i]->curve()->ToTransformAnimationCurve();
        gfx::BoxF animation_bounds;
        bool success = transform_animation_curve->AnimatedBoundsForBox(box, &animation_bounds);
        if (!success)
            return false;
        bounds->Union(animation_bounds);
    }

    return true;
}

bool LayerAnimationController::HasAnimationThatAffectsScale() const
{
    for (size_t i = 0; i < animations_.size(); ++i) {
        if (animations_[i]->is_finished() || animations_[i]->target_property() != Animation::TRANSFORM)
            continue;

        const TransformAnimationCurve* transform_animation_curve = animations_[i]->curve()->ToTransformAnimationCurve();
        if (transform_animation_curve->AffectsScale())
            return true;
    }

    return false;
}

bool LayerAnimationController::HasOnlyTranslationTransforms(
    ObserverType observer_type) const
{
    for (size_t i = 0; i < animations_.size(); ++i) {
        if (animations_[i]->is_finished() || animations_[i]->target_property() != Animation::TRANSFORM)
            continue;

        if ((observer_type == ObserverType::ACTIVE && !animations_[i]->affects_active_observers()) || (observer_type == ObserverType::PENDING && !animations_[i]->affects_pending_observers()))
            continue;

        const TransformAnimationCurve* transform_animation_curve = animations_[i]->curve()->ToTransformAnimationCurve();
        if (!transform_animation_curve->IsTranslation())
            return false;
    }

    return true;
}

bool LayerAnimationController::AnimationsPreserveAxisAlignment() const
{
    for (size_t i = 0; i < animations_.size(); ++i) {
        if (animations_[i]->is_finished() || animations_[i]->target_property() != Animation::TRANSFORM)
            continue;

        const TransformAnimationCurve* transform_animation_curve = animations_[i]->curve()->ToTransformAnimationCurve();
        if (!transform_animation_curve->PreservesAxisAlignment())
            return false;
    }

    return true;
}

bool LayerAnimationController::AnimationStartScale(ObserverType observer_type,
    float* start_scale) const
{
    *start_scale = 0.f;
    for (size_t i = 0; i < animations_.size(); ++i) {
        if (animations_[i]->is_finished() || animations_[i]->target_property() != Animation::TRANSFORM)
            continue;

        if ((observer_type == ObserverType::ACTIVE && !animations_[i]->affects_active_observers()) || (observer_type == ObserverType::PENDING && !animations_[i]->affects_pending_observers()))
            continue;

        bool forward_direction = true;
        switch (animations_[i]->direction()) {
        case Animation::DIRECTION_NORMAL:
        case Animation::DIRECTION_ALTERNATE:
            forward_direction = animations_[i]->playback_rate() >= 0.0;
            break;
        case Animation::DIRECTION_REVERSE:
        case Animation::DIRECTION_ALTERNATE_REVERSE:
            forward_direction = animations_[i]->playback_rate() < 0.0;
            break;
        }

        const TransformAnimationCurve* transform_animation_curve = animations_[i]->curve()->ToTransformAnimationCurve();
        float animation_start_scale = 0.f;
        if (!transform_animation_curve->AnimationStartScale(forward_direction,
                &animation_start_scale))
            return false;
        *start_scale = std::max(*start_scale, animation_start_scale);
    }
    return true;
}

bool LayerAnimationController::MaximumTargetScale(ObserverType observer_type,
    float* max_scale) const
{
    *max_scale = 0.f;
    for (size_t i = 0; i < animations_.size(); ++i) {
        if (animations_[i]->is_finished() || animations_[i]->target_property() != Animation::TRANSFORM)
            continue;

        if ((observer_type == ObserverType::ACTIVE && !animations_[i]->affects_active_observers()) || (observer_type == ObserverType::PENDING && !animations_[i]->affects_pending_observers()))
            continue;

        bool forward_direction = true;
        switch (animations_[i]->direction()) {
        case Animation::DIRECTION_NORMAL:
        case Animation::DIRECTION_ALTERNATE:
            forward_direction = animations_[i]->playback_rate() >= 0.0;
            break;
        case Animation::DIRECTION_REVERSE:
        case Animation::DIRECTION_ALTERNATE_REVERSE:
            forward_direction = animations_[i]->playback_rate() < 0.0;
            break;
        }

        const TransformAnimationCurve* transform_animation_curve = animations_[i]->curve()->ToTransformAnimationCurve();
        float animation_scale = 0.f;
        if (!transform_animation_curve->MaximumTargetScale(forward_direction,
                &animation_scale))
            return false;
        *max_scale = std::max(*max_scale, animation_scale);
    }

    return true;
}

void LayerAnimationController::PushNewAnimationsToImplThread(
    LayerAnimationController* controller_impl) const
{
    // Any new animations owned by the main thread's controller are cloned and
    // add to the impl thread's controller.
    for (size_t i = 0; i < animations_.size(); ++i) {
        // If the animation is already running on the impl thread, there is no
        // need to copy it over.
        if (controller_impl->GetAnimationById(animations_[i]->id()))
            continue;

        // Scroll animations always start at the current scroll offset.
        if (animations_[i]->target_property() == Animation::SCROLL_OFFSET) {
            gfx::ScrollOffset current_scroll_offset;
            if (controller_impl->value_provider_) {
                current_scroll_offset = controller_impl->value_provider_->ScrollOffsetForAnimation();
            } else {
                // The owning layer isn't yet in the active tree, so the main thread
                // scroll offset will be up-to-date.
                current_scroll_offset = value_provider_->ScrollOffsetForAnimation();
            }
            animations_[i]->curve()->ToScrollOffsetAnimationCurve()->SetInitialValue(
                current_scroll_offset);
        }

        // The new animation should be set to run as soon as possible.
        Animation::RunState initial_run_state = Animation::WAITING_FOR_TARGET_AVAILABILITY;
        scoped_ptr<Animation> to_add(
            animations_[i]->CloneAndInitialize(initial_run_state));
        DCHECK(!to_add->needs_synchronized_start_time());
        to_add->set_affects_active_observers(false);
        controller_impl->AddAnimation(to_add.Pass());
    }
}

static bool IsCompleted(
    Animation* animation,
    const LayerAnimationController* main_thread_controller)
{
    if (animation->is_impl_only()) {
        return (animation->run_state() == Animation::WAITING_FOR_DELETION);
    } else {
        return !main_thread_controller->GetAnimationById(animation->id());
    }
}

static bool AffectsActiveOnlyAndIsWaitingForDeletion(Animation* animation)
{
    return animation->run_state() == Animation::WAITING_FOR_DELETION && !animation->affects_pending_observers();
}

void LayerAnimationController::RemoveAnimationsCompletedOnMainThread(
    LayerAnimationController* controller_impl) const
{
    bool removed_transform_animation = false;
    // Animations removed on the main thread should no longer affect pending
    // observers, and should stop affecting active observers after the next call
    // to ActivateAnimations. If already WAITING_FOR_DELETION, they can be removed
    // immediately.
    ScopedPtrVector<Animation>& animations = controller_impl->animations_;
    for (size_t i = 0; i < animations.size(); ++i) {
        if (IsCompleted(animations[i], this)) {
            animations[i]->set_affects_pending_observers(false);
            if (animations[i]->target_property() == Animation::TRANSFORM)
                removed_transform_animation = true;
        }
    }
    animations.erase(cc::remove_if(&animations,
                         animations.begin(),
                         animations.end(),
                         AffectsActiveOnlyAndIsWaitingForDeletion),
        animations.end());

    if (removed_transform_animation)
        controller_impl->UpdatePotentiallyAnimatingTransform();
}

void LayerAnimationController::PushPropertiesToImplThread(
    LayerAnimationController* controller_impl)
{
    for (size_t i = 0; i < animations_.size(); ++i) {
        Animation* current_impl = controller_impl->GetAnimationById(animations_[i]->id());
        if (current_impl)
            animations_[i]->PushPropertiesTo(current_impl);
    }
    controller_impl->scroll_offset_animation_was_interrupted_ = scroll_offset_animation_was_interrupted_;
    scroll_offset_animation_was_interrupted_ = false;
}

void LayerAnimationController::StartAnimations(base::TimeTicks monotonic_time)
{
    DCHECK(needs_to_start_animations_);
    needs_to_start_animations_ = false;
    // First collect running properties affecting each type of observer.
    TargetProperties blocked_properties_for_active_observers;
    TargetProperties blocked_properties_for_pending_observers;
    std::vector<size_t> animations_waiting_for_target;

    animations_waiting_for_target.reserve(animations_.size());
    for (size_t i = 0; i < animations_.size(); ++i) {
        if (animations_[i]->run_state() == Animation::STARTING || animations_[i]->run_state() == Animation::RUNNING) {
            if (animations_[i]->affects_active_observers()) {
                blocked_properties_for_active_observers.insert(
                    animations_[i]->target_property());
            }
            if (animations_[i]->affects_pending_observers()) {
                blocked_properties_for_pending_observers.insert(
                    animations_[i]->target_property());
            }
        } else if (animations_[i]->run_state() == Animation::WAITING_FOR_TARGET_AVAILABILITY) {
            animations_waiting_for_target.push_back(i);
        }
    }

    for (size_t i = 0; i < animations_waiting_for_target.size(); ++i) {
        // Collect all properties for animations with the same group id (they
        // should all also be in the list of animations).
        size_t animation_index = animations_waiting_for_target[i];
        Animation* animation_waiting_for_target = animations_[animation_index];
        // Check for the run state again even though the animation was waiting
        // for target because it might have changed the run state while handling
        // previous animation in this loop (if they belong to same group).
        if (animation_waiting_for_target->run_state() == Animation::WAITING_FOR_TARGET_AVAILABILITY) {
            TargetProperties enqueued_properties;
            bool affects_active_observers = animation_waiting_for_target->affects_active_observers();
            bool affects_pending_observers = animation_waiting_for_target->affects_pending_observers();
            enqueued_properties.insert(
                animation_waiting_for_target->target_property());
            for (size_t j = animation_index + 1; j < animations_.size(); ++j) {
                if (animation_waiting_for_target->group() == animations_[j]->group()) {
                    enqueued_properties.insert(animations_[j]->target_property());
                    affects_active_observers |= animations_[j]->affects_active_observers();
                    affects_pending_observers |= animations_[j]->affects_pending_observers();
                }
            }

            // Check to see if intersection of the list of properties affected by
            // the group and the list of currently blocked properties is null, taking
            // into account the type(s) of observers affected by the group. In any
            // case, the group's target properties need to be added to the lists of
            // blocked properties.
            bool null_intersection = true;
            for (TargetProperties::iterator p_iter = enqueued_properties.begin();
                 p_iter != enqueued_properties.end();
                 ++p_iter) {
                if (affects_active_observers && !blocked_properties_for_active_observers.insert(*p_iter).second)
                    null_intersection = false;
                if (affects_pending_observers && !blocked_properties_for_pending_observers.insert(*p_iter).second)
                    null_intersection = false;
            }

            // If the intersection is null, then we are free to start the animations
            // in the group.
            if (null_intersection) {
                animation_waiting_for_target->SetRunState(Animation::STARTING,
                    monotonic_time);
                for (size_t j = animation_index + 1; j < animations_.size(); ++j) {
                    if (animation_waiting_for_target->group() == animations_[j]->group()) {
                        animations_[j]->SetRunState(Animation::STARTING, monotonic_time);
                    }
                }
            } else {
                needs_to_start_animations_ = true;
            }
        }
    }
}

void LayerAnimationController::PromoteStartedAnimations(
    base::TimeTicks monotonic_time,
    AnimationEventsVector* events)
{
    for (size_t i = 0; i < animations_.size(); ++i) {
        if (animations_[i]->run_state() == Animation::STARTING && animations_[i]->affects_active_observers()) {
            animations_[i]->SetRunState(Animation::RUNNING, monotonic_time);
            if (!animations_[i]->has_set_start_time() && !animations_[i]->needs_synchronized_start_time())
                animations_[i]->set_start_time(monotonic_time);
            if (events) {
                base::TimeTicks start_time;
                if (animations_[i]->has_set_start_time())
                    start_time = animations_[i]->start_time();
                else
                    start_time = monotonic_time;
                AnimationEvent started_event(
                    AnimationEvent::STARTED, id_, animations_[i]->group(),
                    animations_[i]->target_property(), start_time);
                started_event.is_impl_only = animations_[i]->is_impl_only();
                if (started_event.is_impl_only)
                    NotifyAnimationStarted(started_event);
                else
                    events->push_back(started_event);
            }
        }
    }
}

void LayerAnimationController::MarkFinishedAnimations(
    base::TimeTicks monotonic_time)
{
    bool finished_transform_animation = false;
    for (size_t i = 0; i < animations_.size(); ++i) {
        if (!animations_[i]->is_finished() && animations_[i]->IsFinishedAt(monotonic_time)) {
            animations_[i]->SetRunState(Animation::FINISHED, monotonic_time);
            if (animations_[i]->target_property() == Animation::TRANSFORM) {
                finished_transform_animation = true;
            }
        }
    }
    if (finished_transform_animation)
        UpdatePotentiallyAnimatingTransform();
}

void LayerAnimationController::MarkAnimationsForDeletion(
    base::TimeTicks monotonic_time,
    AnimationEventsVector* events)
{
    bool marked_animations_for_deletions = false;
    std::vector<size_t> animations_with_same_group_id;

    animations_with_same_group_id.reserve(animations_.size());
    // Non-aborted animations are marked for deletion after a corresponding
    // AnimationEvent::FINISHED event is sent or received. This means that if
    // we don't have an events vector, we must ensure that non-aborted animations
    // have received a finished event before marking them for deletion.
    for (size_t i = 0; i < animations_.size(); i++) {
        int group_id = animations_[i]->group();
        if (animations_[i]->run_state() == Animation::ABORTED) {
            if (events && !animations_[i]->is_impl_only()) {
                AnimationEvent aborted_event(AnimationEvent::ABORTED, id_, group_id,
                    animations_[i]->target_property(),
                    monotonic_time);
                events->push_back(aborted_event);
            }
            animations_[i]->SetRunState(Animation::WAITING_FOR_DELETION,
                monotonic_time);
            marked_animations_for_deletions = true;
            continue;
        }

        bool all_anims_with_same_id_are_finished = false;

        // Since deleting an animation on the main thread leads to its deletion
        // on the impl thread, we only mark a FINISHED main thread animation for
        // deletion once it has received a FINISHED event from the impl thread.
        bool animation_i_will_send_or_has_received_finish_event = animations_[i]->is_controlling_instance() || animations_[i]->is_impl_only() || animations_[i]->received_finished_event();
        // If an animation is finished, and not already marked for deletion,
        // find out if all other animations in the same group are also finished.
        if (animations_[i]->run_state() == Animation::FINISHED && animation_i_will_send_or_has_received_finish_event) {
            // Clear the animations_with_same_group_id if it was added for
            // the previous animation's iteration.
            if (animations_with_same_group_id.size() > 0)
                animations_with_same_group_id.clear();
            all_anims_with_same_id_are_finished = true;
            for (size_t j = 0; j < animations_.size(); ++j) {
                bool animation_j_will_send_or_has_received_finish_event = animations_[j]->is_controlling_instance() || animations_[j]->is_impl_only() || animations_[j]->received_finished_event();
                if (group_id == animations_[j]->group()) {
                    if (!animations_[j]->is_finished() || (animations_[j]->run_state() == Animation::FINISHED && !animation_j_will_send_or_has_received_finish_event)) {
                        all_anims_with_same_id_are_finished = false;
                        break;
                    } else if (j >= i && animations_[j]->run_state() != Animation::ABORTED) {
                        // Mark down the animations which belong to the same group
                        // and is not yet aborted. If this current iteration finds that all
                        // animations with same ID are finished, then the marked
                        // animations below will be set to WAITING_FOR_DELETION in next
                        // iteration.
                        animations_with_same_group_id.push_back(j);
                    }
                }
            }
        }
        if (all_anims_with_same_id_are_finished) {
            // We now need to remove all animations with the same group id as
            // group_id (and send along animation finished notifications, if
            // necessary).
            for (size_t j = 0; j < animations_with_same_group_id.size(); j++) {
                size_t animation_index = animations_with_same_group_id[j];
                if (events) {
                    AnimationEvent finished_event(
                        AnimationEvent::FINISHED, id_,
                        animations_[animation_index]->group(),
                        animations_[animation_index]->target_property(),
                        monotonic_time);
                    finished_event.is_impl_only = animations_[animation_index]->is_impl_only();
                    if (finished_event.is_impl_only)
                        NotifyAnimationFinished(finished_event);
                    else
                        events->push_back(finished_event);
                }
                animations_[animation_index]->SetRunState(
                    Animation::WAITING_FOR_DELETION, monotonic_time);
            }
            marked_animations_for_deletions = true;
        }
    }
    if (marked_animations_for_deletions)
        NotifyObserversAnimationWaitingForDeletion();
}

static bool IsWaitingForDeletion(Animation* animation)
{
    return animation->run_state() == Animation::WAITING_FOR_DELETION;
}

void LayerAnimationController::PurgeAnimationsMarkedForDeletion()
{
    animations_.erase(cc::remove_if(&animations_,
                          animations_.begin(),
                          animations_.end(),
                          IsWaitingForDeletion),
        animations_.end());
}

void LayerAnimationController::TickAnimations(base::TimeTicks monotonic_time)
{
    for (size_t i = 0; i < animations_.size(); ++i) {
        if (animations_[i]->run_state() == Animation::STARTING || animations_[i]->run_state() == Animation::RUNNING || animations_[i]->run_state() == Animation::PAUSED) {
            if (!animations_[i]->InEffect(monotonic_time))
                continue;

            base::TimeDelta trimmed = animations_[i]->TrimTimeToCurrentIteration(monotonic_time);

            switch (animations_[i]->target_property()) {
            case Animation::TRANSFORM: {
                const TransformAnimationCurve* transform_animation_curve = animations_[i]->curve()->ToTransformAnimationCurve();
                const gfx::Transform transform = transform_animation_curve->GetValue(trimmed);
                NotifyObserversTransformAnimated(
                    transform,
                    animations_[i]->affects_active_observers(),
                    animations_[i]->affects_pending_observers());
                break;
            }

            case Animation::OPACITY: {
                const FloatAnimationCurve* float_animation_curve = animations_[i]->curve()->ToFloatAnimationCurve();
                const float opacity = std::max(
                    std::min(float_animation_curve->GetValue(trimmed), 1.0f), 0.f);
                NotifyObserversOpacityAnimated(
                    opacity,
                    animations_[i]->affects_active_observers(),
                    animations_[i]->affects_pending_observers());
                break;
            }

            case Animation::FILTER: {
                const FilterAnimationCurve* filter_animation_curve = animations_[i]->curve()->ToFilterAnimationCurve();
                const FilterOperations filter = filter_animation_curve->GetValue(trimmed);
                NotifyObserversFilterAnimated(
                    filter,
                    animations_[i]->affects_active_observers(),
                    animations_[i]->affects_pending_observers());
                break;
            }

            case Animation::BACKGROUND_COLOR: {
                // Not yet implemented.
                break;
            }

            case Animation::SCROLL_OFFSET: {
                const ScrollOffsetAnimationCurve* scroll_offset_animation_curve = animations_[i]->curve()->ToScrollOffsetAnimationCurve();
                const gfx::ScrollOffset scroll_offset = scroll_offset_animation_curve->GetValue(trimmed);
                NotifyObserversScrollOffsetAnimated(
                    scroll_offset,
                    animations_[i]->affects_active_observers(),
                    animations_[i]->affects_pending_observers());
                break;
            }
            }
        }
    }
}

void LayerAnimationController::UpdateActivation(UpdateActivationType type)
{
    bool force = type == FORCE_ACTIVATION;
    if (registrar_) {
        bool was_active = is_active_;
        is_active_ = false;
        for (size_t i = 0; i < animations_.size(); ++i) {
            if (animations_[i]->run_state() != Animation::WAITING_FOR_DELETION) {
                is_active_ = true;
                break;
            }
        }

        if (is_active_ && (!was_active || force))
            registrar_->DidActivateAnimationController(this);
        else if (!is_active_ && (was_active || force))
            registrar_->DidDeactivateAnimationController(this);
    }
}

void LayerAnimationController::NotifyObserversOpacityAnimated(
    float opacity,
    bool notify_active_observers,
    bool notify_pending_observers)
{
    if (value_observers_.might_have_observers()) {
        base::ObserverListBase<LayerAnimationValueObserver>::Iterator it(
            &value_observers_);
        LayerAnimationValueObserver* obs;
        while ((obs = it.GetNext()) != nullptr) {
            if ((notify_active_observers && notify_pending_observers) || (notify_active_observers && obs->IsActive()) || (notify_pending_observers && !obs->IsActive()))
                obs->OnOpacityAnimated(opacity);
        }
    }
}

void LayerAnimationController::NotifyObserversTransformAnimated(
    const gfx::Transform& transform,
    bool notify_active_observers,
    bool notify_pending_observers)
{
    if (value_observers_.might_have_observers()) {
        base::ObserverListBase<LayerAnimationValueObserver>::Iterator it(
            &value_observers_);
        LayerAnimationValueObserver* obs;
        while ((obs = it.GetNext()) != nullptr) {
            if ((notify_active_observers && notify_pending_observers) || (notify_active_observers && obs->IsActive()) || (notify_pending_observers && !obs->IsActive()))
                obs->OnTransformAnimated(transform);
        }
    }
}

void LayerAnimationController::NotifyObserversFilterAnimated(
    const FilterOperations& filters,
    bool notify_active_observers,
    bool notify_pending_observers)
{
    if (value_observers_.might_have_observers()) {
        base::ObserverListBase<LayerAnimationValueObserver>::Iterator it(
            &value_observers_);
        LayerAnimationValueObserver* obs;
        while ((obs = it.GetNext()) != nullptr) {
            if ((notify_active_observers && notify_pending_observers) || (notify_active_observers && obs->IsActive()) || (notify_pending_observers && !obs->IsActive()))
                obs->OnFilterAnimated(filters);
        }
    }
}

void LayerAnimationController::NotifyObserversScrollOffsetAnimated(
    const gfx::ScrollOffset& scroll_offset,
    bool notify_active_observers,
    bool notify_pending_observers)
{
    if (value_observers_.might_have_observers()) {
        base::ObserverListBase<LayerAnimationValueObserver>::Iterator it(
            &value_observers_);
        LayerAnimationValueObserver* obs;
        while ((obs = it.GetNext()) != nullptr) {
            if ((notify_active_observers && notify_pending_observers) || (notify_active_observers && obs->IsActive()) || (notify_pending_observers && !obs->IsActive()))
                obs->OnScrollOffsetAnimated(scroll_offset);
        }
    }
}

void LayerAnimationController::NotifyObserversAnimationWaitingForDeletion()
{
    FOR_EACH_OBSERVER(LayerAnimationValueObserver,
        value_observers_,
        OnAnimationWaitingForDeletion());
}

void LayerAnimationController::
    NotifyObserversTransformIsPotentiallyAnimatingChanged(
        bool notify_active_observers,
        bool notify_pending_observers)
{
    if (value_observers_.might_have_observers()) {
        base::ObserverListBase<LayerAnimationValueObserver>::Iterator it(
            &value_observers_);
        LayerAnimationValueObserver* obs;
        while ((obs = it.GetNext()) != nullptr) {
            if (notify_active_observers && obs->IsActive())
                obs->OnTransformIsPotentiallyAnimatingChanged(
                    potentially_animating_transform_for_active_observers_);
            else if (notify_pending_observers && !obs->IsActive())
                obs->OnTransformIsPotentiallyAnimatingChanged(
                    potentially_animating_transform_for_pending_observers_);
        }
    }
}

bool LayerAnimationController::HasValueObserver()
{
    if (value_observers_.might_have_observers()) {
        base::ObserverListBase<LayerAnimationValueObserver>::Iterator it(
            &value_observers_);
        return it.GetNext() != nullptr;
    }
    return false;
}

bool LayerAnimationController::HasActiveValueObserver()
{
    if (value_observers_.might_have_observers()) {
        base::ObserverListBase<LayerAnimationValueObserver>::Iterator it(
            &value_observers_);
        LayerAnimationValueObserver* obs;
        while ((obs = it.GetNext()) != nullptr)
            if (obs->IsActive())
                return true;
    }
    return false;
}

} // namespace cc
