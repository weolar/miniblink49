// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/blink/web_animation_impl.h"

#include "cc/animation/animation.h"
#include "cc/animation/animation_curve.h"
#include "cc/animation/animation_id_provider.h"
#include "cc/blink/web_filter_animation_curve_impl.h"
#include "cc/blink/web_float_animation_curve_impl.h"
#include "cc/blink/web_scroll_offset_animation_curve_impl.h"
#include "cc/blink/web_transform_animation_curve_impl.h"
#include "third_party/WebKit/public/platform/WebCompositorAnimationCurve.h"

using cc::Animation;
using cc::AnimationIdProvider;

using blink::WebCompositorAnimation;
using blink::WebCompositorAnimationCurve;

namespace cc_blink {

WebCompositorAnimationImpl::WebCompositorAnimationImpl(
    const WebCompositorAnimationCurve& web_curve,
    TargetProperty target_property,
    int animation_id,
    int group_id)
{
    if (!animation_id)
        animation_id = AnimationIdProvider::NextAnimationId();
    if (!group_id)
        group_id = AnimationIdProvider::NextGroupId();

    WebCompositorAnimationCurve::AnimationCurveType curve_type = web_curve.type();
    scoped_ptr<cc::AnimationCurve> curve;
    switch (curve_type) {
    case WebCompositorAnimationCurve::AnimationCurveTypeFloat: {
        const WebFloatAnimationCurveImpl* float_curve_impl = static_cast<const WebFloatAnimationCurveImpl*>(&web_curve);
        curve = float_curve_impl->CloneToAnimationCurve();
        break;
    }
    case WebCompositorAnimationCurve::AnimationCurveTypeTransform: {
        const WebTransformAnimationCurveImpl* transform_curve_impl = static_cast<const WebTransformAnimationCurveImpl*>(&web_curve);
        curve = transform_curve_impl->CloneToAnimationCurve();
        break;
    }
    case WebCompositorAnimationCurve::AnimationCurveTypeFilter: {
        const WebFilterAnimationCurveImpl* filter_curve_impl = static_cast<const WebFilterAnimationCurveImpl*>(&web_curve);
        curve = filter_curve_impl->CloneToAnimationCurve();
        break;
    }
    case WebCompositorAnimationCurve::AnimationCurveTypeScrollOffset: {
        const WebScrollOffsetAnimationCurveImpl* scroll_curve_impl = static_cast<const WebScrollOffsetAnimationCurveImpl*>(&web_curve);
        curve = scroll_curve_impl->CloneToAnimationCurve();
        break;
    }
    }
    animation_ = Animation::Create(
        curve.Pass(),
        animation_id,
        group_id,
        static_cast<cc::Animation::TargetProperty>(target_property));
}

WebCompositorAnimationImpl::~WebCompositorAnimationImpl()
{
}

int WebCompositorAnimationImpl::id()
{
    return animation_->id();
}

int WebCompositorAnimationImpl::group()
{
    return animation_->group();
}

blink::WebCompositorAnimation::TargetProperty
WebCompositorAnimationImpl::targetProperty() const
{
    return static_cast<WebCompositorAnimationImpl::TargetProperty>(
        animation_->target_property());
}

double WebCompositorAnimationImpl::iterations() const
{
    return animation_->iterations();
}

void WebCompositorAnimationImpl::setIterations(double n)
{
    animation_->set_iterations(n);
}

double WebCompositorAnimationImpl::iterationStart() const
{
    return animation_->iteration_start();
}

void WebCompositorAnimationImpl::setIterationStart(double iteration_start)
{
    animation_->set_iteration_start(iteration_start);
}

double WebCompositorAnimationImpl::startTime() const
{
    return (animation_->start_time() - base::TimeTicks()).InSecondsF();
}

void WebCompositorAnimationImpl::setStartTime(double monotonic_time)
{
    animation_->set_start_time(base::TimeTicks::FromInternalValue(
        monotonic_time * base::Time::kMicrosecondsPerSecond));
}

double WebCompositorAnimationImpl::timeOffset() const
{
    return animation_->time_offset().InSecondsF();
}

void WebCompositorAnimationImpl::setTimeOffset(double monotonic_time)
{
    animation_->set_time_offset(base::TimeDelta::FromSecondsD(monotonic_time));
}

blink::WebCompositorAnimation::Direction WebCompositorAnimationImpl::direction()
    const
{
    switch (animation_->direction()) {
    case cc::Animation::DIRECTION_NORMAL:
        return DirectionNormal;
    case cc::Animation::DIRECTION_REVERSE:
        return DirectionReverse;
    case cc::Animation::DIRECTION_ALTERNATE:
        return DirectionAlternate;
    case cc::Animation::DIRECTION_ALTERNATE_REVERSE:
        return DirectionAlternateReverse;
    default:
        NOTREACHED();
    }
    return DirectionNormal;
}

void WebCompositorAnimationImpl::setDirection(Direction direction)
{
    switch (direction) {
    case DirectionNormal:
        animation_->set_direction(cc::Animation::DIRECTION_NORMAL);
        break;
    case DirectionReverse:
        animation_->set_direction(cc::Animation::DIRECTION_REVERSE);
        break;
    case DirectionAlternate:
        animation_->set_direction(cc::Animation::DIRECTION_ALTERNATE);
        break;
    case DirectionAlternateReverse:
        animation_->set_direction(cc::Animation::DIRECTION_ALTERNATE_REVERSE);
        break;
    }
}

double WebCompositorAnimationImpl::playbackRate() const
{
    return animation_->playback_rate();
}

void WebCompositorAnimationImpl::setPlaybackRate(double playback_rate)
{
    animation_->set_playback_rate(playback_rate);
}

blink::WebCompositorAnimation::FillMode WebCompositorAnimationImpl::fillMode()
    const
{
    switch (animation_->fill_mode()) {
    case cc::Animation::FILL_MODE_NONE:
        return FillModeNone;
    case cc::Animation::FILL_MODE_FORWARDS:
        return FillModeForwards;
    case cc::Animation::FILL_MODE_BACKWARDS:
        return FillModeBackwards;
    case cc::Animation::FILL_MODE_BOTH:
        return FillModeBoth;
    default:
        NOTREACHED();
    }
    return FillModeNone;
}

void WebCompositorAnimationImpl::setFillMode(FillMode fill_mode)
{
    switch (fill_mode) {
    case FillModeNone:
        animation_->set_fill_mode(cc::Animation::FILL_MODE_NONE);
        break;
    case FillModeForwards:
        animation_->set_fill_mode(cc::Animation::FILL_MODE_FORWARDS);
        break;
    case FillModeBackwards:
        animation_->set_fill_mode(cc::Animation::FILL_MODE_BACKWARDS);
        break;
    case FillModeBoth:
        animation_->set_fill_mode(cc::Animation::FILL_MODE_BOTH);
        break;
    }
}

scoped_ptr<cc::Animation> WebCompositorAnimationImpl::PassAnimation()
{
    animation_->set_needs_synchronized_start_time(true);
    return animation_.Pass();
}

} // namespace cc_blink
