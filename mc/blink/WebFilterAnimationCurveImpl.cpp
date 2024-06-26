// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/blink/WebFilterAnimationCurveImpl.h"

#include "mc/animation/KeyframedAnimationCurve.h"
#include "mc/animation/TimingFunction.h"
#include "mc/blink/WebAnimationCurveCommon.h"
#include "mc/blink/WebFilterOperationsImpl.h"

using blink::WebFilterKeyframe;

namespace mc_blink {

WebFilterAnimationCurveImpl::WebFilterAnimationCurveImpl()
    : m_curve(mc::KeyframedFilterAnimationCurve::create()) 
{
}

WebFilterAnimationCurveImpl::~WebFilterAnimationCurveImpl()
{
}

blink::WebCompositorAnimationCurve::AnimationCurveType WebFilterAnimationCurveImpl::type() const 
{
    return WebCompositorAnimationCurve::AnimationCurveTypeFilter;
}

void WebFilterAnimationCurveImpl::add(const WebFilterKeyframe& keyframe, TimingFunctionType type)
{
    const mc::FilterOperationsWrap& filterOperations = static_cast<const WebFilterOperationsImpl&>(keyframe.value()).asFilterOperations();
    m_curve->addKeyframe(mc::FilterKeyframe::create(base::TimeDelta::FromSecondsD(keyframe.time()), filterOperations, createTimingFunction(type)));
}

void WebFilterAnimationCurveImpl::add(const WebFilterKeyframe& keyframe, double x1, double y1, double x2, double y2) 
{
    const mc::FilterOperationsWrap& filterOperations = static_cast<const WebFilterOperationsImpl&>(keyframe.value()).asFilterOperations();
    m_curve->addKeyframe(mc::FilterKeyframe::create(base::TimeDelta::FromSecondsD(keyframe.time()), filterOperations,
        mc::CubicBezierTimingFunction::create(x1, y1, x2, y2)));
}

void WebFilterAnimationCurveImpl::add(const WebFilterKeyframe& keyframe, int steps, float steps_start_offset)
{
    const mc::FilterOperationsWrap& filterOperations = static_cast<const WebFilterOperationsImpl&>(keyframe.value()).asFilterOperations();
    m_curve->addKeyframe(mc::FilterKeyframe::create(
        base::TimeDelta::FromSecondsD(keyframe.time()),
        filterOperations,
        mc::StepsTimingFunction::create(steps, steps_start_offset)
        ));
}

void WebFilterAnimationCurveImpl::setLinearTimingFunction()
{
    m_curve->setTimingFunction(nullptr);
}

void WebFilterAnimationCurveImpl::setCubicBezierTimingFunction(TimingFunctionType type) 
{
    m_curve->setTimingFunction(createTimingFunction(type));
}

void WebFilterAnimationCurveImpl::setCubicBezierTimingFunction(double x1, double y1, double x2, double y2) 
{
    m_curve->setTimingFunction(mc::CubicBezierTimingFunction::create(x1, y1, x2, y2));
}

void WebFilterAnimationCurveImpl::setStepsTimingFunction(int number_of_steps, float steps_start_offset)
{
    m_curve->setTimingFunction(mc::StepsTimingFunction::create(number_of_steps, steps_start_offset));
}

WTF::PassOwnPtr<mc::AnimationCurve> WebFilterAnimationCurveImpl::cloneToAnimationCurve() const
{
    return m_curve->clone();
}

}  // namespace cc_blink
