// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_ANIMATION_TIMING_FUNCTION_H_
#define CC_ANIMATION_TIMING_FUNCTION_H_

#include "cc/base/cc_export.h"
#include "ui/gfx/geometry/cubic_bezier.h"
#include "base/memory/scoped_ptr.h"

namespace cc {

// See http://www.w3.org/TR/css3-transitions/.
class CC_EXPORT TimingFunction {
public:
    virtual ~TimingFunction();

    virtual float GetValue(double t) const = 0;
    virtual float Velocity(double time) const = 0;
    // The smallest and largest values returned by GetValue for inputs in [0, 1].
    virtual void Range(float* min, float* max) const = 0;
    virtual scoped_ptr<TimingFunction> Clone() const = 0;

protected:
    TimingFunction();

private:
    DISALLOW_ASSIGN(TimingFunction);
};

class CC_EXPORT CubicBezierTimingFunction : public TimingFunction {
public:
    static scoped_ptr<CubicBezierTimingFunction> Create(double x1, double y1,
        double x2, double y2);
    ~CubicBezierTimingFunction() override;

    // TimingFunction implementation.
    float GetValue(double time) const override;
    float Velocity(double time) const override;
    void Range(float* min, float* max) const override;
    scoped_ptr<TimingFunction> Clone() const override;

protected:
    CubicBezierTimingFunction(double x1, double y1, double x2, double y2);

    gfx::CubicBezier bezier_;

private:
    DISALLOW_ASSIGN(CubicBezierTimingFunction);
};

class CC_EXPORT EaseTimingFunction {
public:
    static scoped_ptr<TimingFunction> Create();

private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(EaseTimingFunction);
};

class CC_EXPORT EaseInTimingFunction {
public:
    static scoped_ptr<TimingFunction> Create();

private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(EaseInTimingFunction);
};

class CC_EXPORT EaseOutTimingFunction {
public:
    static scoped_ptr<TimingFunction> Create();

private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(EaseOutTimingFunction);
};

class CC_EXPORT EaseInOutTimingFunction {
public:
    static scoped_ptr<TimingFunction> Create();

private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(EaseInOutTimingFunction);
};

class CC_EXPORT StepsTimingFunction : public TimingFunction {
public:
    static scoped_ptr<StepsTimingFunction> Create(int steps,
        float steps_start_offset);
    ~StepsTimingFunction() override;

    float GetValue(double t) const override;
    scoped_ptr<TimingFunction> Clone() const override;

    void Range(float* min, float* max) const override;
    float Velocity(double time) const override;

protected:
    StepsTimingFunction(int steps, float steps_start_offset);

private:
    int steps_;
    float steps_start_offset_;

    DISALLOW_ASSIGN(StepsTimingFunction);
};

} // namespace cc

#endif // CC_ANIMATION_TIMING_FUNCTION_H_
