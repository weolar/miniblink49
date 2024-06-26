// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_animation_TimingFunction_h
#define mc_animation_TimingFunction_h

#include "mc/base/CubicBezier.h"
#include "third_party/WebKit/Source/wtf/PassOwnPtr.h"

namespace mc {

// See http://www.w3.org/TR/css3-transitions/.
class TimingFunction {
public:
    virtual ~TimingFunction();

    virtual float getValue(double t) const = 0;
    virtual float velocity(double time) const = 0;
    // The smallest and largest values returned by GetValue for inputs in [0, 1].
    virtual void range(float* min, float* max) const = 0;
    virtual WTF::PassOwnPtr<TimingFunction> clone() const = 0;

protected:
    TimingFunction();

private:
};

class CubicBezierTimingFunction : public TimingFunction {
public:
    static WTF::PassOwnPtr<CubicBezierTimingFunction> create(double x1, double y1, double x2, double y2);
    ~CubicBezierTimingFunction() override;

    // TimingFunction implementation.
    float getValue(double time) const override;
    float velocity(double time) const override;
    void range(float* min, float* max) const override;
    WTF::PassOwnPtr<TimingFunction> clone() const override;

protected:
    CubicBezierTimingFunction(double x1, double y1, double x2, double y2);

    CubicBezier m_bezier;

private:
};

class EaseTimingFunction {
public:
    static WTF::PassOwnPtr<TimingFunction> create();

private:
};

class EaseInTimingFunction {
public:
    static WTF::PassOwnPtr<TimingFunction> create();

private:
};

class EaseOutTimingFunction {
public:
    static WTF::PassOwnPtr<TimingFunction> create();

private:
};

class EaseInOutTimingFunction {
public:
    static WTF::PassOwnPtr<TimingFunction> create();

private:
};

class StepsTimingFunction : public TimingFunction {
public:
    static WTF::PassOwnPtr<StepsTimingFunction> create(int steps, float stepsStartOffset);
    ~StepsTimingFunction() override;

    float getValue(double t) const override;
    WTF::PassOwnPtr<TimingFunction> clone() const override;

    void range(float* min, float* max) const override;
    float velocity(double time) const override;

protected:
    StepsTimingFunction(int steps, float stepsStartOffset);

private:
    int m_steps;
    float m_stepsStartOffset;
};

}  // namespace mc

#endif  // mc_animation_TimingFunction_h
