// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BLINK_WEB_FILTER_ANIMATION_CURVE_IMPL_H_
#define CC_BLINK_WEB_FILTER_ANIMATION_CURVE_IMPL_H_

#include "base/memory/scoped_ptr.h"
#include "cc/blink/cc_blink_export.h"
#include "third_party/WebKit/public/platform/WebFilterAnimationCurve.h"

namespace cc {
class AnimationCurve;
class KeyframedFilterAnimationCurve;
}

namespace blink {
class WebFilterKeyframe;
}

namespace cc_blink {

class WebFilterAnimationCurveImpl : public blink::WebFilterAnimationCurve {
public:
    CC_BLINK_EXPORT WebFilterAnimationCurveImpl();
    ~WebFilterAnimationCurveImpl() override;

    // blink::WebCompositorAnimationCurve implementation.
    AnimationCurveType type() const override;

    // blink::WebFilterAnimationCurve implementation.
    void add(const blink::WebFilterKeyframe& keyframe,
        TimingFunctionType type) override;
    void add(const blink::WebFilterKeyframe& keyframe,
        double x1,
        double y1,
        double x2,
        double y2) override;
    void add(const blink::WebFilterKeyframe& keyframe,
        int steps,
        float steps_start_offset) override;
    void setLinearTimingFunction() override;
    void setCubicBezierTimingFunction(TimingFunctionType) override;
    void setCubicBezierTimingFunction(double x1,
        double y1,
        double x2,
        double y2) override;
    void setStepsTimingFunction(int number_of_steps,
        float steps_start_offset) override;

    scoped_ptr<cc::AnimationCurve> CloneToAnimationCurve() const;

private:
    scoped_ptr<cc::KeyframedFilterAnimationCurve> curve_;

    DISALLOW_COPY_AND_ASSIGN(WebFilterAnimationCurveImpl);
};

} // namespace cc_blink

#endif // CC_BLINK_WEB_FILTER_ANIMATION_CURVE_IMPL_H_
