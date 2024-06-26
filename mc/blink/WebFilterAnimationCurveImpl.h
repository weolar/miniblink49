// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_blink_WebFilterAnimationCurveImpl_h
#define mc_blink_WebFilterAnimationCurveImpl_h

#include "third_party/WebKit/public/platform/WebFilterAnimationCurve.h"
#include "third_party/WebKit/Source/wtf/PassOwnPtr.h"
#include "third_party/WebKit/Source/wtf/OwnPtr.h"

namespace mc {
class AnimationCurve;
class KeyframedFilterAnimationCurve;
}

namespace blink {
class WebFilterKeyframe;
}

namespace mc_blink {

class WebFilterAnimationCurveImpl : public blink::WebFilterAnimationCurve {
public:
    WebFilterAnimationCurveImpl();
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

    WTF::PassOwnPtr<mc::AnimationCurve> cloneToAnimationCurve() const;

private:
    WTF::OwnPtr<mc::KeyframedFilterAnimationCurve> m_curve;
};

}  // namespace mc_blink

#endif  // mc_blink_WebFilterAnimationCurveImpl_h
