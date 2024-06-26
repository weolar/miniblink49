// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/animation/timing_function.h"
#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "cc/base/math_util.h"

namespace cc {

TimingFunction::TimingFunction() { }

TimingFunction::~TimingFunction() { }

scoped_ptr<CubicBezierTimingFunction> CubicBezierTimingFunction::Create(
    double x1, double y1, double x2, double y2)
{
    return make_scoped_ptr(new CubicBezierTimingFunction(x1, y1, x2, y2));
}

CubicBezierTimingFunction::CubicBezierTimingFunction(double x1,
    double y1,
    double x2,
    double y2)
    : bezier_(x1, y1, x2, y2)
{
}

CubicBezierTimingFunction::~CubicBezierTimingFunction() { }

float CubicBezierTimingFunction::GetValue(double x) const
{
    return static_cast<float>(bezier_.Solve(x));
}

float CubicBezierTimingFunction::Velocity(double x) const
{
    return static_cast<float>(bezier_.Slope(x));
}

void CubicBezierTimingFunction::Range(float* min, float* max) const
{
    double min_d = 0;
    double max_d = 1;
    bezier_.Range(&min_d, &max_d);
    *min = static_cast<float>(min_d);
    *max = static_cast<float>(max_d);
}

scoped_ptr<TimingFunction> CubicBezierTimingFunction::Clone() const
{
    return make_scoped_ptr(new CubicBezierTimingFunction(*this));
}

// These numbers come from
// http://www.w3.org/TR/css3-transitions/#transition-timing-function_tag.
scoped_ptr<TimingFunction> EaseTimingFunction::Create()
{
    return CubicBezierTimingFunction::Create(0.25, 0.1, 0.25, 1.0);
}

scoped_ptr<TimingFunction> EaseInTimingFunction::Create()
{
    return CubicBezierTimingFunction::Create(0.42, 0.0, 1.0, 1.0);
}

scoped_ptr<TimingFunction> EaseOutTimingFunction::Create()
{
    return CubicBezierTimingFunction::Create(0.0, 0.0, 0.58, 1.0);
}

scoped_ptr<TimingFunction> EaseInOutTimingFunction::Create()
{
    return CubicBezierTimingFunction::Create(0.42, 0.0, 0.58, 1);
}

scoped_ptr<StepsTimingFunction> StepsTimingFunction::Create(
    int steps,
    float steps_start_offset)
{
    return make_scoped_ptr(new StepsTimingFunction(steps, steps_start_offset));
}

StepsTimingFunction::StepsTimingFunction(int steps, float steps_start_offset)
    : steps_(steps)
    , steps_start_offset_(steps_start_offset)
{
    // Restrict it to CSS presets: step_start, step_end and step_middle.
    // See the Web Animations specification, 3.12.4. Timing in discrete steps.
    DCHECK(steps_start_offset_ == 0 || steps_start_offset_ == 1 || steps_start_offset_ == 0.5);
}

StepsTimingFunction::~StepsTimingFunction()
{
}

float StepsTimingFunction::GetValue(double t) const
{
    const double steps = static_cast<double>(steps_);
    const double value = MathUtil::ClampToRange(
        std::floor((steps * t) + steps_start_offset_) / steps, 0.0, 1.0);
    return static_cast<float>(value);
}

scoped_ptr<TimingFunction> StepsTimingFunction::Clone() const
{
    return make_scoped_ptr(new StepsTimingFunction(*this));
}

void StepsTimingFunction::Range(float* min, float* max) const
{
    *min = 0.0f;
    *max = 1.0f;
}

float StepsTimingFunction::Velocity(double x) const
{
    return 0.0f;
}

} // namespace cc
