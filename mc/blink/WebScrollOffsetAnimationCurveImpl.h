// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BLINK_WEB_SCROLL_OFFSET_ANIMATION_CURVE_IMPL_H_
#define CC_BLINK_WEB_SCROLL_OFFSET_ANIMATION_CURVE_IMPL_H_

#include "third_party/WebKit/public/platform/WebScrollOffsetAnimationCurve.h"
#include "third_party/WebKit/Source/wtf/OwnPtr.h"
#include "third_party/WebKit/Source/wtf/PassOwnPtr.h"

namespace mc {
class AnimationCurve;
class ScrollOffsetAnimationCurve;
}

namespace mc_blink {

class WebScrollOffsetAnimationCurveImpl
    : public blink::WebScrollOffsetAnimationCurve {
public:
    WebScrollOffsetAnimationCurveImpl(
        blink::WebFloatPoint target_value,
        TimingFunctionType timing_function);
    ~WebScrollOffsetAnimationCurveImpl() override;

    // blink::WebCompositorAnimationCurve implementation.
    AnimationCurveType type() const override;

    // blink::WebScrollOffsetAnimationCurve implementation.
    void setInitialValue(blink::WebFloatPoint initial_value) override;
    blink::WebFloatPoint getValue(double time) const override;
    double duration() const override;

    virtual blink::WebFloatPoint targetValue() const;
    virtual void updateTarget(double time, blink::WebFloatPoint new_target);

    WTF::PassOwnPtr<mc::AnimationCurve> cloneToAnimationCurve() const;

private:
    WTF::OwnPtr<mc::ScrollOffsetAnimationCurve> m_curve;
};

}  // namespace cc_blink

#endif  // CC_BLINK_WEB_SCROLL_OFFSET_ANIMATION_CURVE_IMPL_H_
