// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_ANIMATION_SCROLL_OFFSET_ANIMATION_CURVE_H_
#define CC_ANIMATION_SCROLL_OFFSET_ANIMATION_CURVE_H_

#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"
#include "cc/animation/animation_curve.h"
#include "cc/base/cc_export.h"
#include "ui/gfx/geometry/scroll_offset.h"

namespace cc {

class TimingFunction;

class CC_EXPORT ScrollOffsetAnimationCurve : public AnimationCurve {
public:
    static scoped_ptr<ScrollOffsetAnimationCurve> Create(
        const gfx::ScrollOffset& target_value,
        scoped_ptr<TimingFunction> timing_function);

    ~ScrollOffsetAnimationCurve() override;

    void SetInitialValue(const gfx::ScrollOffset& initial_value);
    gfx::ScrollOffset GetValue(base::TimeDelta t) const;
    gfx::ScrollOffset target_value() const { return target_value_; }
    void UpdateTarget(double t, const gfx::ScrollOffset& new_target);

    // AnimationCurve implementation
    base::TimeDelta Duration() const override;
    CurveType Type() const override;
    scoped_ptr<AnimationCurve> Clone() const override;

private:
    ScrollOffsetAnimationCurve(const gfx::ScrollOffset& target_value,
        scoped_ptr<TimingFunction> timing_function);

    gfx::ScrollOffset initial_value_;
    gfx::ScrollOffset target_value_;
    base::TimeDelta total_animation_duration_;

    // Time from animation start to most recent UpdateTarget.
    base::TimeDelta last_retarget_;

    scoped_ptr<TimingFunction> timing_function_;

    DISALLOW_COPY_AND_ASSIGN(ScrollOffsetAnimationCurve);
};

} // namespace cc

#endif // CC_ANIMATION_SCROLL_OFFSET_ANIMATION_CURVE_H_
