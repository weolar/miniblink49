// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/blink/web_scroll_offset_animation_curve_impl.h"

#include "cc/animation/scroll_offset_animation_curve.h"
#include "cc/animation/timing_function.h"
#include "cc/blink/web_animation_curve_common.h"

using blink::WebFloatPoint;

namespace cc_blink {

WebScrollOffsetAnimationCurveImpl::WebScrollOffsetAnimationCurveImpl(
    WebFloatPoint target_value,
    TimingFunctionType timing_function)
    : curve_(cc::ScrollOffsetAnimationCurve::Create(
        gfx::ScrollOffset(target_value.x, target_value.y),
        CreateTimingFunction(timing_function)))
{
}

WebScrollOffsetAnimationCurveImpl::~WebScrollOffsetAnimationCurveImpl()
{
}

blink::WebCompositorAnimationCurve::AnimationCurveType
WebScrollOffsetAnimationCurveImpl::type() const
{
    return WebCompositorAnimationCurve::AnimationCurveTypeScrollOffset;
}

void WebScrollOffsetAnimationCurveImpl::setInitialValue(
    WebFloatPoint initial_value)
{
    curve_->SetInitialValue(gfx::ScrollOffset(initial_value.x, initial_value.y));
}

WebFloatPoint WebScrollOffsetAnimationCurveImpl::getValue(double time) const
{
    gfx::ScrollOffset value = curve_->GetValue(base::TimeDelta::FromSecondsD(time));
    return WebFloatPoint(value.x(), value.y());
}

double WebScrollOffsetAnimationCurveImpl::duration() const
{
    return curve_->Duration().InSecondsF();
}

WebFloatPoint WebScrollOffsetAnimationCurveImpl::targetValue() const
{
    gfx::ScrollOffset target = curve_->target_value();
    return WebFloatPoint(target.x(), target.y());
}

void WebScrollOffsetAnimationCurveImpl::updateTarget(double time,
    WebFloatPoint new_target)
{
    curve_->UpdateTarget(time, gfx::ScrollOffset(new_target.x, new_target.y));
}

scoped_ptr<cc::AnimationCurve>
WebScrollOffsetAnimationCurveImpl::CloneToAnimationCurve() const
{
    return curve_->Clone();
}

} // namespace cc_blink
