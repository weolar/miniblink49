// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/blink/WebTransformAnimationCurveImpl.h"

#include "mc/animation/KeyframedAnimationCurve.h"
#include "mc/animation/TimingFunction.h"
#include "mc/animation/TransformOperations.h"
#include "mc/blink/WebAnimationCurveCommon.h"
#include "mc/blink/WebTransformOperationsImpl.h"

using blink::WebTransformKeyframe;

namespace mc_blink {

WebTransformAnimationCurveImpl::WebTransformAnimationCurveImpl()
    : m_curve(mc::KeyframedTransformAnimationCurve::create()) 
{
}

WebTransformAnimationCurveImpl::~WebTransformAnimationCurveImpl() 
{
}

blink::WebCompositorAnimationCurve::AnimationCurveType WebTransformAnimationCurveImpl::type() const 
{
    return WebCompositorAnimationCurve::AnimationCurveTypeTransform;
}

void WebTransformAnimationCurveImpl::add(const WebTransformKeyframe& keyframe) 
{
    add(keyframe, TimingFunctionTypeEase);
}

void WebTransformAnimationCurveImpl::add(const WebTransformKeyframe& keyframe, TimingFunctionType type)
{
    const mc::TransformOperations& transform_operations = static_cast<const WebTransformOperationsImpl&>(keyframe.value()).asTransformOperations();
    m_curve->addKeyframe(mc::TransformKeyframe::create(base::TimeDelta::FromSecondsD(keyframe.time()), transform_operations,        createTimingFunction(type)));
}

void WebTransformAnimationCurveImpl::add(const WebTransformKeyframe& keyframe, double x1, double y1, double x2, double y2) 
{
    const mc::TransformOperations& transform_operations = static_cast<const WebTransformOperationsImpl&>(keyframe.value()).asTransformOperations();
    m_curve->addKeyframe(mc::TransformKeyframe::create(
        base::TimeDelta::FromSecondsD(keyframe.time()), transform_operations,
        mc::CubicBezierTimingFunction::create(x1, y1, x2, y2)));
}

void WebTransformAnimationCurveImpl::add(const WebTransformKeyframe& keyframe, int steps, float steps_start_offset) 
{
    const mc::TransformOperations& transform_operations = static_cast<const WebTransformOperationsImpl&>(keyframe.value()).asTransformOperations();
    m_curve->addKeyframe(mc::TransformKeyframe::create(
        base::TimeDelta::FromSecondsD(keyframe.time()), transform_operations,
        mc::StepsTimingFunction::create(steps, steps_start_offset)));
}

void WebTransformAnimationCurveImpl::setLinearTimingFunction() 
{
    m_curve->setTimingFunction(nullptr);
}

void WebTransformAnimationCurveImpl::setCubicBezierTimingFunction(TimingFunctionType type)
{
    m_curve->setTimingFunction(createTimingFunction(type));
}

void WebTransformAnimationCurveImpl::setCubicBezierTimingFunction(double x1, double y1, double x2, double y2) 
{
    m_curve->setTimingFunction(mc::CubicBezierTimingFunction::create(x1, y1, x2, y2));
}

void WebTransformAnimationCurveImpl::setStepsTimingFunction(int number_of_steps, float steps_start_offset) 
{
    m_curve->setTimingFunction(mc::StepsTimingFunction::create(number_of_steps, steps_start_offset));
}

WTF::PassOwnPtr<mc::AnimationCurve> WebTransformAnimationCurveImpl::cloneToAnimationCurve() const
{
    return m_curve->clone();
}

}  // namespace mc_blink
