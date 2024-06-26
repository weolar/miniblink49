// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/blink/web_transform_animation_curve_impl.h"

#include "cc/animation/keyframed_animation_curve.h"
#include "cc/animation/timing_function.h"
#include "cc/animation/transform_operations.h"
#include "cc/blink/web_animation_curve_common.h"
#include "cc/blink/web_transform_operations_impl.h"

using blink::WebTransformKeyframe;

namespace cc_blink {

WebTransformAnimationCurveImpl::WebTransformAnimationCurveImpl()
    : curve_(cc::KeyframedTransformAnimationCurve::Create())
{
}

WebTransformAnimationCurveImpl::~WebTransformAnimationCurveImpl()
{
}

blink::WebCompositorAnimationCurve::AnimationCurveType
WebTransformAnimationCurveImpl::type() const
{
    return WebCompositorAnimationCurve::AnimationCurveTypeTransform;
}

void WebTransformAnimationCurveImpl::add(const WebTransformKeyframe& keyframe)
{
    add(keyframe, TimingFunctionTypeEase);
}

void WebTransformAnimationCurveImpl::add(const WebTransformKeyframe& keyframe,
    TimingFunctionType type)
{
    const cc::TransformOperations& transform_operations = static_cast<const WebTransformOperationsImpl&>(keyframe.value())
                                                              .AsTransformOperations();
    curve_->AddKeyframe(cc::TransformKeyframe::Create(
        base::TimeDelta::FromSecondsD(keyframe.time()), transform_operations,
        CreateTimingFunction(type)));
}

void WebTransformAnimationCurveImpl::add(const WebTransformKeyframe& keyframe,
    double x1,
    double y1,
    double x2,
    double y2)
{
    const cc::TransformOperations& transform_operations = static_cast<const WebTransformOperationsImpl&>(keyframe.value())
                                                              .AsTransformOperations();
    curve_->AddKeyframe(cc::TransformKeyframe::Create(
        base::TimeDelta::FromSecondsD(keyframe.time()), transform_operations,
        cc::CubicBezierTimingFunction::Create(x1, y1, x2, y2)));
}

void WebTransformAnimationCurveImpl::add(const WebTransformKeyframe& keyframe,
    int steps,
    float steps_start_offset)
{
    const cc::TransformOperations& transform_operations = static_cast<const WebTransformOperationsImpl&>(keyframe.value())
                                                              .AsTransformOperations();
    curve_->AddKeyframe(cc::TransformKeyframe::Create(
        base::TimeDelta::FromSecondsD(keyframe.time()), transform_operations,
        cc::StepsTimingFunction::Create(steps, steps_start_offset)));
}

void WebTransformAnimationCurveImpl::setLinearTimingFunction()
{
    curve_->SetTimingFunction(nullptr);
}

void WebTransformAnimationCurveImpl::setCubicBezierTimingFunction(
    TimingFunctionType type)
{
    curve_->SetTimingFunction(CreateTimingFunction(type));
}

void WebTransformAnimationCurveImpl::setCubicBezierTimingFunction(double x1,
    double y1,
    double x2,
    double y2)
{
    curve_->SetTimingFunction(
        cc::CubicBezierTimingFunction::Create(x1, y1, x2, y2).Pass());
}

void WebTransformAnimationCurveImpl::setStepsTimingFunction(
    int number_of_steps,
    float steps_start_offset)
{
    curve_->SetTimingFunction(cc::StepsTimingFunction::Create(
        number_of_steps, steps_start_offset)
                                  .Pass());
}

scoped_ptr<cc::AnimationCurve>
WebTransformAnimationCurveImpl::CloneToAnimationCurve() const
{
    return curve_->Clone();
}

} // namespace cc_blink
