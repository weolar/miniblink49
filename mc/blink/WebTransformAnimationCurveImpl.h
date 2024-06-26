// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_blink_WebTransformAnimationCurveImpl_h
#define mc_blink_WebTransformAnimationCurveImpl_h

#include "third_party/WebKit/public/platform/WebTransformAnimationCurve.h"
#include "third_party/WebKit/Source/wtf/OwnPtr.h"
#include "third_party/WebKit/Source/wtf/PassOwnPtr.h"

namespace mc {
class AnimationCurve;
class KeyframedTransformAnimationCurve;
}

namespace blink {
class WebTransformKeyframe;
}

namespace mc_blink {

class WebTransformAnimationCurveImpl : public blink::WebTransformAnimationCurve {
public:
    WebTransformAnimationCurveImpl();
    ~WebTransformAnimationCurveImpl() override;

    // blink::WebCompositorAnimationCurve implementation.
    AnimationCurveType type() const override;

    // blink::WebTransformAnimationCurve implementation.
    void add(const blink::WebTransformKeyframe& keyframe) override;
    void add(const blink::WebTransformKeyframe& keyframe,
        TimingFunctionType type) override;
    void add(const blink::WebTransformKeyframe& keyframe,
        double x1,
        double y1,
        double x2,
        double y2) override;
    void add(const blink::WebTransformKeyframe& keyframe,
        int steps,
        float stepsStartOffset) override;
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
    WTF::OwnPtr<mc::KeyframedTransformAnimationCurve> m_curve;
};

}  // namespace cc_blink

#endif  // mc_blink_WebTransformAnimationCurveImpl_h
