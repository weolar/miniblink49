// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/blink/WebFloatAnimationCurveImpl.h"

#include "mc/animation/AnimationCurve.h"
#include "mc/animation/KeyframedAnimationCurve.h"
#include "mc/animation/TimingFunction.h"
#include "mc/blink/WebAnimationCurveCommon.h"

using blink::WebFloatKeyframe;

namespace mc_blink {

WebFloatAnimationCurveImpl::WebFloatAnimationCurveImpl()
    : m_curve(mc::KeyframedFloatAnimationCurve::create()) 
{
}

WebFloatAnimationCurveImpl::~WebFloatAnimationCurveImpl() {
}

blink::WebCompositorAnimationCurve::AnimationCurveType WebFloatAnimationCurveImpl::type() const
{
    return blink::WebCompositorAnimationCurve::AnimationCurveTypeFloat;
}

void WebFloatAnimationCurveImpl::add(const WebFloatKeyframe& keyframe) 
{
    add(keyframe, TimingFunctionTypeEase);
}

void WebFloatAnimationCurveImpl::add(const WebFloatKeyframe& keyframe, TimingFunctionType type) 
{
    m_curve->addKeyframe(mc::FloatKeyframe::create(base::TimeDelta::FromSecondsD(keyframe.time),
            keyframe.value, createTimingFunction(type)));
}

void WebFloatAnimationCurveImpl::add(const WebFloatKeyframe& keyframe, double x1, double y1, double x2, double y2) 
{
    m_curve->addKeyframe(mc::FloatKeyframe::create(
        base::TimeDelta::FromSecondsD(keyframe.time), keyframe.value,
        mc::CubicBezierTimingFunction::create(x1, y1, x2, y2)));
}

void WebFloatAnimationCurveImpl::add(const WebFloatKeyframe& keyframe, int steps, float steps_start_offset)
{
    m_curve->addKeyframe(mc::FloatKeyframe::create(
        base::TimeDelta::FromSecondsD(keyframe.time), keyframe.value,
        mc::StepsTimingFunction::create(steps, steps_start_offset)));
}

void WebFloatAnimationCurveImpl::setLinearTimingFunction() {
    m_curve->setTimingFunction(nullptr);
}

void WebFloatAnimationCurveImpl::setCubicBezierTimingFunction(TimingFunctionType type) 
{
    m_curve->setTimingFunction(createTimingFunction(type));
}

void WebFloatAnimationCurveImpl::setCubicBezierTimingFunction(double x1, double y1, double x2, double y2) 
{
    m_curve->setTimingFunction(mc::CubicBezierTimingFunction::create(x1, y1, x2, y2));
}

void WebFloatAnimationCurveImpl::setStepsTimingFunction(int number_of_steps, float steps_start_offset)
{
    m_curve->setTimingFunction(mc::StepsTimingFunction::create(number_of_steps, steps_start_offset));
}

float WebFloatAnimationCurveImpl::getValue(double time) const
{
    return m_curve->getValue(base::TimeDelta::FromSecondsD(time));
}

WTF::PassOwnPtr<mc::AnimationCurve> WebFloatAnimationCurveImpl::cloneToAnimationCurve() const
{
    return m_curve->clone();
}

}  // namespace cc_blink
