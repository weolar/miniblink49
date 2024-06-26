// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_animation_ScrollOffsetAnimationCurve_h
#define mc_animation_ScrollOffsetAnimationCurve_h

#include "mc/animation/AnimationCurve.h"

#include "base/time/time.h"
#include "third_party/WebKit/Source/wtf/OwnPtr.h"
#include "third_party/WebKit/Source/wtf/PassOwnPtr.h"
#include "third_party/WebKit/Source/platform/geometry/FloatPoint.h"

namespace mc {

class TimingFunction;

class ScrollOffsetAnimationCurve : public AnimationCurve {
public:
    static WTF::PassOwnPtr<ScrollOffsetAnimationCurve> create(const blink::FloatPoint& targetValue, WTF::PassOwnPtr<TimingFunction> timingFunction);

    ~ScrollOffsetAnimationCurve() override;

    void setInitialValue(const blink::FloatPoint& initialValue);
    blink::FloatPoint getValue(base::TimeDelta t) const;
    blink::FloatPoint targetValue() const
    {
        return m_targetValue;
    }
    void updateTarget(double t, const blink::FloatPoint& newTarget);

    // AnimationCurve implementation
    base::TimeDelta duration() const override;
    CurveType type() const override;
    WTF::PassOwnPtr<AnimationCurve> clone() const override;

private:
    ScrollOffsetAnimationCurve(const blink::FloatPoint& targetValue, WTF::PassOwnPtr<TimingFunction> timingFunction);

    blink::FloatPoint m_initialValue;
    blink::FloatPoint m_targetValue;
    base::TimeDelta m_totalAnimationDuration;

    // Time from animation start to most recent UpdateTarget.
    base::TimeDelta m_lastRetarget;

    WTF::OwnPtr<TimingFunction> m_timingFunction;
};

}  // namespace cc

#endif  // mc_animation_ScrollOffsetAnimationCurve_h
