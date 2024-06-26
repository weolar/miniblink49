// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/animation/TimingFunction.h"
#include "mc/base/MathUtil.h"

namespace mc {

TimingFunction::TimingFunction() {}

TimingFunction::~TimingFunction() {}

WTF::PassOwnPtr<CubicBezierTimingFunction> CubicBezierTimingFunction::create(double x1, double y1, double x2, double y2) 
{
    return adoptPtr(new CubicBezierTimingFunction(x1, y1, x2, y2));
}

CubicBezierTimingFunction::CubicBezierTimingFunction(double x1, double y1, double x2, double y2)
    : m_bezier(x1, y1, x2, y2) {}

CubicBezierTimingFunction::~CubicBezierTimingFunction() {}

float CubicBezierTimingFunction::getValue(double x) const 
{
    return static_cast<float>(m_bezier.solve(x));
}

float CubicBezierTimingFunction::velocity(double x) const
{
    return static_cast<float>(m_bezier.slope(x));
}

void CubicBezierTimingFunction::range(float* min, float* max) const
{
    double min_d = 0;
    double max_d = 1;
    m_bezier.range(&min_d, &max_d);
    *min = static_cast<float>(min_d);
    *max = static_cast<float>(max_d);
}

WTF::PassOwnPtr<TimingFunction> CubicBezierTimingFunction::clone() const
{
    return adoptPtr(new CubicBezierTimingFunction(*this));
}

// These numbers come from
// http://www.w3.org/TR/css3-transitions/#transition-timing-function_tag.
WTF::PassOwnPtr<TimingFunction> EaseTimingFunction::create()
{
    return CubicBezierTimingFunction::create(0.25, 0.1, 0.25, 1.0);
}

WTF::PassOwnPtr<TimingFunction> EaseInTimingFunction::create()
{
    return CubicBezierTimingFunction::create(0.42, 0.0, 1.0, 1.0);
}

WTF::PassOwnPtr<TimingFunction> EaseOutTimingFunction::create()
{
    return CubicBezierTimingFunction::create(0.0, 0.0, 0.58, 1.0);
}

WTF::PassOwnPtr<TimingFunction> EaseInOutTimingFunction::create() 
{
    return CubicBezierTimingFunction::create(0.42, 0.0, 0.58, 1);
}

WTF::PassOwnPtr<StepsTimingFunction> StepsTimingFunction::create(int steps, float steps_start_offset)
{
    return adoptPtr(new StepsTimingFunction(steps, steps_start_offset));
}

StepsTimingFunction::StepsTimingFunction(int steps, float stepsStartOffset)
    : m_steps(steps), m_stepsStartOffset(stepsStartOffset)
{
    // Restrict it to CSS presets: step_start, step_end and step_middle.
    // See the Web Animations specification, 3.12.4. Timing in discrete steps.
    ASSERT(m_stepsStartOffset == 0 || m_stepsStartOffset == 1 || m_stepsStartOffset == 0.5);
}

StepsTimingFunction::~StepsTimingFunction() {
}

float StepsTimingFunction::getValue(double t) const 
{
    const double steps = static_cast<double>(m_steps);
    const double value = MathUtil::ClampToRange(std::floor((steps * t) + m_stepsStartOffset) / steps, 0.0, 1.0);
    return static_cast<float>(value);
}

WTF::PassOwnPtr<TimingFunction> StepsTimingFunction::clone() const
{
    return adoptPtr(new StepsTimingFunction(*this));
}

void StepsTimingFunction::range(float* min, float* max) const 
{
    *min = 0.0f;
    *max = 1.0f;
}

float StepsTimingFunction::velocity(double x) const 
{
    return 0.0f;
}

}  // namespace mc
