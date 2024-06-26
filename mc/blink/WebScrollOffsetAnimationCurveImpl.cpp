// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/blink/WebScrollOffsetAnimationCurveImpl.h"

#include "mc/animation/ScrollOffsetAnimationCurve.h"
#include "mc/animation/TimingFunction.h"
#include "mc/blink/WebAnimationCurveCommon.h"
#include "third_party/WebKit/Source/platform/geometry/FloatPoint.h"

using blink::WebFloatPoint;

namespace mc_blink {

WebScrollOffsetAnimationCurveImpl::WebScrollOffsetAnimationCurveImpl(WebFloatPoint target_value, TimingFunctionType timing_function)
    : m_curve(mc::ScrollOffsetAnimationCurve::create(blink::FloatPoint(target_value.x, target_value.y), createTimingFunction(timing_function))) 
{
}

WebScrollOffsetAnimationCurveImpl::~WebScrollOffsetAnimationCurveImpl()
{
}

blink::WebCompositorAnimationCurve::AnimationCurveType WebScrollOffsetAnimationCurveImpl::type() const
{
    return WebCompositorAnimationCurve::AnimationCurveTypeScrollOffset;
}

void WebScrollOffsetAnimationCurveImpl::setInitialValue(WebFloatPoint initial_value)
{
    m_curve->setInitialValue(blink::FloatPoint(initial_value.x, initial_value.y));
}

WebFloatPoint WebScrollOffsetAnimationCurveImpl::getValue(double time) const
{
    blink::FloatPoint value = m_curve->getValue(base::TimeDelta::FromSecondsD(time));
    return value;
}

double WebScrollOffsetAnimationCurveImpl::duration() const
{
    return m_curve->duration().InSecondsF();
}

WebFloatPoint WebScrollOffsetAnimationCurveImpl::targetValue() const
{
    blink::FloatPoint target = m_curve->targetValue();
    return target;
}

void WebScrollOffsetAnimationCurveImpl::updateTarget(double time, WebFloatPoint new_target)
{
    m_curve->updateTarget(time, blink::FloatPoint(new_target.x, new_target.y));
}

WTF::PassOwnPtr<mc::AnimationCurve> WebScrollOffsetAnimationCurveImpl::cloneToAnimationCurve() const
{
    return m_curve->clone();
}

}  // namespace mc_blink
