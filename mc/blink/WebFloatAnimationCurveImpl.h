// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_blink_WebFloatAnimationCurveImpl_h
#define mc_blink_WebFloatAnimationCurveImpl_h

#include "third_party/WebKit/public/platform/WebFloatAnimationCurve.h"
#include "third_party/WebKit/Source/wtf/PassOwnPtr.h"
#include "third_party/WebKit/Source/wtf/OwnPtr.h"

namespace mc {
class AnimationCurve;
class KeyframedFloatAnimationCurve;
}

namespace blink {
struct WebFloatKeyframe;
}

namespace mc_blink {

class WebFloatAnimationCurveImpl : public blink::WebFloatAnimationCurve {
public:
    WebFloatAnimationCurveImpl();
    ~WebFloatAnimationCurveImpl() override;

    // WebCompositorAnimationCurve implementation.
    AnimationCurveType type() const override;

    // WebFloatAnimationCurve implementation.
    void add(const blink::WebFloatKeyframe& keyframe) override;
    void add(const blink::WebFloatKeyframe& keyframe,
        TimingFunctionType type) override;
    void add(const blink::WebFloatKeyframe& keyframe,
        double x1,
        double y1,
        double x2,
        double y2) override;
    void add(const blink::WebFloatKeyframe& keyframe,
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

    float getValue(double time) const override;

    WTF::PassOwnPtr<mc::AnimationCurve> cloneToAnimationCurve() const;

private:
    WTF::OwnPtr<mc::KeyframedFloatAnimationCurve> m_curve;
};

}  // namespace mc_blink

#endif  // mc_blink_WebFloatAnimationCurveImpl_h
