// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/test/animation_test_common.h"

#include "cc/animation/animation_id_provider.h"
#include "cc/animation/animation_player.h"
#include "cc/animation/keyframed_animation_curve.h"
#include "cc/animation/layer_animation_controller.h"
#include "cc/animation/transform_operations.h"
#include "cc/base/time_util.h"
#include "cc/layers/layer.h"
#include "cc/layers/layer_impl.h"

using cc::Animation;
using cc::AnimationCurve;
using cc::EaseTimingFunction;
using cc::FloatKeyframe;
using cc::KeyframedFloatAnimationCurve;
using cc::KeyframedTransformAnimationCurve;
using cc::TimingFunction;
using cc::TransformKeyframe;

namespace cc {

template <class Target>
int AddOpacityTransition(Target* target,
    double duration,
    float start_opacity,
    float end_opacity,
    bool use_timing_function)
{
    scoped_ptr<KeyframedFloatAnimationCurve>
        curve(KeyframedFloatAnimationCurve::Create());

    scoped_ptr<TimingFunction> func;
    if (!use_timing_function)
        func = EaseTimingFunction::Create();
    if (duration > 0.0)
        curve->AddKeyframe(
            FloatKeyframe::Create(base::TimeDelta(), start_opacity, func.Pass()));
    curve->AddKeyframe(FloatKeyframe::Create(
        base::TimeDelta::FromSecondsD(duration), end_opacity, nullptr));

    int id = AnimationIdProvider::NextAnimationId();

    scoped_ptr<Animation> animation(
        Animation::Create(curve.Pass(), id, AnimationIdProvider::NextGroupId(),
            Animation::OPACITY));
    animation->set_needs_synchronized_start_time(true);

    target->AddAnimation(animation.Pass());
    return id;
}

template <class Target>
int AddAnimatedTransform(Target* target,
    double duration,
    TransformOperations start_operations,
    TransformOperations operations)
{
    scoped_ptr<KeyframedTransformAnimationCurve>
        curve(KeyframedTransformAnimationCurve::Create());

    if (duration > 0.0) {
        curve->AddKeyframe(TransformKeyframe::Create(base::TimeDelta(),
            start_operations, nullptr));
    }

    curve->AddKeyframe(TransformKeyframe::Create(
        base::TimeDelta::FromSecondsD(duration), operations, nullptr));

    int id = AnimationIdProvider::NextAnimationId();

    scoped_ptr<Animation> animation(
        Animation::Create(curve.Pass(), id, AnimationIdProvider::NextGroupId(),
            Animation::TRANSFORM));
    animation->set_needs_synchronized_start_time(true);

    target->AddAnimation(animation.Pass());
    return id;
}

template <class Target>
int AddAnimatedTransform(Target* target,
    double duration,
    int delta_x,
    int delta_y)
{
    TransformOperations start_operations;
    if (duration > 0.0) {
        start_operations.AppendTranslate(0, 0, 0.0);
    }

    TransformOperations operations;
    operations.AppendTranslate(delta_x, delta_y, 0.0);
    return AddAnimatedTransform(target, duration, start_operations, operations);
}

template <class Target>
int AddAnimatedFilter(Target* target,
    double duration,
    float start_brightness,
    float end_brightness)
{
    scoped_ptr<KeyframedFilterAnimationCurve>
        curve(KeyframedFilterAnimationCurve::Create());

    if (duration > 0.0) {
        FilterOperations start_filters;
        start_filters.Append(
            FilterOperation::CreateBrightnessFilter(start_brightness));
        curve->AddKeyframe(
            FilterKeyframe::Create(base::TimeDelta(), start_filters, nullptr));
    }

    FilterOperations filters;
    filters.Append(FilterOperation::CreateBrightnessFilter(end_brightness));
    curve->AddKeyframe(FilterKeyframe::Create(
        base::TimeDelta::FromSecondsD(duration), filters, nullptr));

    int id = AnimationIdProvider::NextAnimationId();

    scoped_ptr<Animation> animation(Animation::Create(
        curve.Pass(), id, AnimationIdProvider::NextGroupId(), Animation::FILTER));
    animation->set_needs_synchronized_start_time(true);

    target->AddAnimation(animation.Pass());
    return id;
}

FakeFloatAnimationCurve::FakeFloatAnimationCurve()
    : duration_(base::TimeDelta::FromSecondsD(1.0))
{
}

FakeFloatAnimationCurve::FakeFloatAnimationCurve(double duration)
    : duration_(base::TimeDelta::FromSecondsD(duration))
{
}

FakeFloatAnimationCurve::~FakeFloatAnimationCurve() { }

base::TimeDelta FakeFloatAnimationCurve::Duration() const
{
    return duration_;
}

float FakeFloatAnimationCurve::GetValue(base::TimeDelta now) const
{
    return 0.0f;
}

scoped_ptr<AnimationCurve> FakeFloatAnimationCurve::Clone() const
{
    return make_scoped_ptr(new FakeFloatAnimationCurve);
}

FakeTransformTransition::FakeTransformTransition(double duration)
    : duration_(base::TimeDelta::FromSecondsD(duration))
{
}

FakeTransformTransition::~FakeTransformTransition() { }

base::TimeDelta FakeTransformTransition::Duration() const
{
    return duration_;
}

gfx::Transform FakeTransformTransition::GetValue(base::TimeDelta time) const
{
    return gfx::Transform();
}

bool FakeTransformTransition::AnimatedBoundsForBox(const gfx::BoxF& box,
    gfx::BoxF* bounds) const
{
    return false;
}

bool FakeTransformTransition::AffectsScale() const { return false; }

bool FakeTransformTransition::IsTranslation() const { return true; }

bool FakeTransformTransition::PreservesAxisAlignment() const
{
    return true;
}

bool FakeTransformTransition::AnimationStartScale(bool forward_direction,
    float* start_scale) const
{
    *start_scale = 1.f;
    return true;
}

bool FakeTransformTransition::MaximumTargetScale(bool forward_direction,
    float* max_scale) const
{
    *max_scale = 1.f;
    return true;
}

scoped_ptr<AnimationCurve> FakeTransformTransition::Clone() const
{
    return make_scoped_ptr(new FakeTransformTransition(*this));
}

FakeFloatTransition::FakeFloatTransition(double duration, float from, float to)
    : duration_(base::TimeDelta::FromSecondsD(duration))
    , from_(from)
    , to_(to)
{
}

FakeFloatTransition::~FakeFloatTransition() { }

base::TimeDelta FakeFloatTransition::Duration() const
{
    return duration_;
}

float FakeFloatTransition::GetValue(base::TimeDelta time) const
{
    double progress = TimeUtil::Divide(time, duration_);
    if (progress >= 1.0)
        progress = 1.0;
    return (1.0 - progress) * from_ + progress * to_;
}

FakeLayerAnimationValueObserver::FakeLayerAnimationValueObserver()
    : opacity_(0.0f)
    , animation_waiting_for_deletion_(false)
    , transform_is_animating_(false)
{
}

FakeLayerAnimationValueObserver::~FakeLayerAnimationValueObserver() { }

void FakeLayerAnimationValueObserver::OnFilterAnimated(
    const FilterOperations& filters)
{
    filters_ = filters;
}

void FakeLayerAnimationValueObserver::OnOpacityAnimated(float opacity)
{
    opacity_ = opacity;
}

void FakeLayerAnimationValueObserver::OnTransformAnimated(
    const gfx::Transform& transform)
{
    transform_ = transform;
}

void FakeLayerAnimationValueObserver::OnScrollOffsetAnimated(
    const gfx::ScrollOffset& scroll_offset)
{
    scroll_offset_ = scroll_offset;
}

void FakeLayerAnimationValueObserver::OnAnimationWaitingForDeletion()
{
    animation_waiting_for_deletion_ = true;
}

void FakeLayerAnimationValueObserver::OnTransformIsPotentiallyAnimatingChanged(
    bool is_animating)
{
    transform_is_animating_ = is_animating;
}

bool FakeLayerAnimationValueObserver::IsActive() const
{
    return true;
}

bool FakeInactiveLayerAnimationValueObserver::IsActive() const
{
    return false;
}

gfx::ScrollOffset FakeLayerAnimationValueProvider::ScrollOffsetForAnimation()
    const
{
    return scroll_offset_;
}

scoped_ptr<AnimationCurve> FakeFloatTransition::Clone() const
{
    return make_scoped_ptr(new FakeFloatTransition(*this));
}

int AddOpacityTransitionToController(LayerAnimationController* controller,
    double duration,
    float start_opacity,
    float end_opacity,
    bool use_timing_function)
{
    return AddOpacityTransition(controller,
        duration,
        start_opacity,
        end_opacity,
        use_timing_function);
}

int AddAnimatedTransformToController(LayerAnimationController* controller,
    double duration,
    int delta_x,
    int delta_y)
{
    return AddAnimatedTransform(controller,
        duration,
        delta_x,
        delta_y);
}

int AddAnimatedFilterToController(LayerAnimationController* controller,
    double duration,
    float start_brightness,
    float end_brightness)
{
    return AddAnimatedFilter(
        controller, duration, start_brightness, end_brightness);
}

int AddOpacityTransitionToLayer(Layer* layer,
    double duration,
    float start_opacity,
    float end_opacity,
    bool use_timing_function)
{
    return AddOpacityTransition(layer,
        duration,
        start_opacity,
        end_opacity,
        use_timing_function);
}

int AddOpacityTransitionToLayer(LayerImpl* layer,
    double duration,
    float start_opacity,
    float end_opacity,
    bool use_timing_function)
{
    return AddOpacityTransition(layer->layer_animation_controller(),
        duration,
        start_opacity,
        end_opacity,
        use_timing_function);
}

int AddAnimatedTransformToLayer(Layer* layer,
    double duration,
    int delta_x,
    int delta_y)
{
    return AddAnimatedTransform(layer, duration, delta_x, delta_y);
}

int AddAnimatedTransformToLayer(LayerImpl* layer,
    double duration,
    int delta_x,
    int delta_y)
{
    return AddAnimatedTransform(layer->layer_animation_controller(),
        duration,
        delta_x,
        delta_y);
}

int AddAnimatedTransformToLayer(Layer* layer,
    double duration,
    TransformOperations start_operations,
    TransformOperations operations)
{
    return AddAnimatedTransform(layer, duration, start_operations, operations);
}

int AddAnimatedTransformToLayer(LayerImpl* layer,
    double duration,
    TransformOperations start_operations,
    TransformOperations operations)
{
    return AddAnimatedTransform(layer->layer_animation_controller(),
        duration,
        start_operations,
        operations);
}

int AddAnimatedFilterToLayer(Layer* layer,
    double duration,
    float start_brightness,
    float end_brightness)
{
    return AddAnimatedFilter(layer, duration, start_brightness, end_brightness);
}

int AddAnimatedFilterToLayer(LayerImpl* layer,
    double duration,
    float start_brightness,
    float end_brightness)
{
    return AddAnimatedFilter(layer->layer_animation_controller(),
        duration,
        start_brightness,
        end_brightness);
}

int AddAnimatedTransformToPlayer(AnimationPlayer* player,
    double duration,
    int delta_x,
    int delta_y)
{
    return AddAnimatedTransform(player, duration, delta_x, delta_y);
}

int AddOpacityTransitionToPlayer(AnimationPlayer* player,
    double duration,
    float start_opacity,
    float end_opacity,
    bool use_timing_function)
{
    return AddOpacityTransition(player, duration, start_opacity, end_opacity,
        use_timing_function);
}

int AddAnimatedFilterToPlayer(AnimationPlayer* player,
    double duration,
    float start_brightness,
    float end_brightness)
{
    return AddAnimatedFilter(player, duration, start_brightness, end_brightness);
}

int AddOpacityStepsToController(LayerAnimationController* target,
    double duration,
    float start_opacity,
    float end_opacity,
    int num_steps)
{
    scoped_ptr<KeyframedFloatAnimationCurve> curve(
        KeyframedFloatAnimationCurve::Create());

    scoped_ptr<TimingFunction> func = StepsTimingFunction::Create(num_steps, 0.5f);
    if (duration > 0.0)
        curve->AddKeyframe(
            FloatKeyframe::Create(base::TimeDelta(), start_opacity, func.Pass()));
    curve->AddKeyframe(FloatKeyframe::Create(
        base::TimeDelta::FromSecondsD(duration), end_opacity, nullptr));

    int id = AnimationIdProvider::NextAnimationId();

    scoped_ptr<Animation> animation(
        Animation::Create(curve.Pass(), id, AnimationIdProvider::NextGroupId(),
            Animation::OPACITY));
    animation->set_needs_synchronized_start_time(true);

    target->AddAnimation(animation.Pass());
    return id;
}

} // namespace cc
