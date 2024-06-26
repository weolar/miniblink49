// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/blink/web_filter_animation_curve_impl.h"

#include "cc/animation/keyframed_animation_curve.h"
#include "cc/animation/timing_function.h"
#include "cc/blink/web_animation_curve_common.h"
#include "cc/blink/web_filter_operations_impl.h"
#include "cc/output/filter_operations.h"

using blink::WebFilterKeyframe;

namespace cc_blink {

WebFilterAnimationCurveImpl::WebFilterAnimationCurveImpl()
    : curve_(cc::KeyframedFilterAnimationCurve::Create())
{
}

WebFilterAnimationCurveImpl::~WebFilterAnimationCurveImpl()
{
}

blink::WebCompositorAnimationCurve::AnimationCurveType
WebFilterAnimationCurveImpl::type() const
{
    return WebCompositorAnimationCurve::AnimationCurveTypeFilter;
}

void WebFilterAnimationCurveImpl::add(const WebFilterKeyframe& keyframe,
    TimingFunctionType type)
{
    const cc::FilterOperations& filter_operations = static_cast<const WebFilterOperationsImpl&>(keyframe.value())
                                                        .AsFilterOperations();
    curve_->AddKeyframe(cc::FilterKeyframe::Create(
        base::TimeDelta::FromSecondsD(keyframe.time()), filter_operations,
        CreateTimingFunction(type)));
}

void WebFilterAnimationCurveImpl::add(const WebFilterKeyframe& keyframe,
    double x1,
    double y1,
    double x2,
    double y2)
{
    const cc::FilterOperations& filter_operations = static_cast<const WebFilterOperationsImpl&>(keyframe.value())
                                                        .AsFilterOperations();
    curve_->AddKeyframe(cc::FilterKeyframe::Create(
        base::TimeDelta::FromSecondsD(keyframe.time()), filter_operations,
        cc::CubicBezierTimingFunction::Create(x1, y1, x2, y2)));
}

void WebFilterAnimationCurveImpl::add(const WebFilterKeyframe& keyframe,
    int steps,
    float steps_start_offset)
{
    const cc::FilterOperations& filter_operations = static_cast<const WebFilterOperationsImpl&>(keyframe.value())
                                                        .AsFilterOperations();
    curve_->AddKeyframe(cc::FilterKeyframe::Create(
        base::TimeDelta::FromSecondsD(keyframe.time()), filter_operations,
        cc::StepsTimingFunction::Create(steps, steps_start_offset)));
}

void WebFilterAnimationCurveImpl::setLinearTimingFunction()
{
    curve_->SetTimingFunction(nullptr);
}

void WebFilterAnimationCurveImpl::setCubicBezierTimingFunction(
    TimingFunctionType type)
{
    curve_->SetTimingFunction(CreateTimingFunction(type));
}

void WebFilterAnimationCurveImpl::setCubicBezierTimingFunction(double x1,
    double y1,
    double x2,
    double y2)
{
    curve_->SetTimingFunction(
        cc::CubicBezierTimingFunction::Create(x1, y1, x2, y2).Pass());
}

void WebFilterAnimationCurveImpl::setStepsTimingFunction(
    int number_of_steps,
    float steps_start_offset)
{
    curve_->SetTimingFunction(cc::StepsTimingFunction::Create(
        number_of_steps, steps_start_offset)
                                  .Pass());
}

scoped_ptr<cc::AnimationCurve>
WebFilterAnimationCurveImpl::CloneToAnimationCurve() const
{
    return curve_->Clone();
}

} // namespace cc_blink
