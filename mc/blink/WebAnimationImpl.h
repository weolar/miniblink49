// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_blink_WebAnimationImpl_h
#define mc_blink_WebAnimationImpl_h

#include "third_party/WebKit/public/platform/WebCompositorAnimation.h"
#include "third_party/WebKit/Source/wtf/OwnPtr.h"
#include "third_party/WebKit/Source/wtf/PassOwnPtr.h"

namespace mc {
class AnimationObj;
}

namespace blink {
class WebCompositorAnimationCurve;
}

namespace mc_blink {

class WebCompositorAnimationImpl : public blink::WebCompositorAnimation {
public:
    WebCompositorAnimationImpl(
        const blink::WebCompositorAnimationCurve& curve,
        TargetProperty target,
        int animation_id,
        int group_id);
    ~WebCompositorAnimationImpl() override;

    // blink::WebCompositorAnimation implementation
    int id() override;
    int group() override;
    TargetProperty targetProperty() const override;
    double iterations() const override;
    void setIterations(double iterations) override;
    double iterationStart() const override;
    void setIterationStart(double iteration_start) override;
    double startTime() const override;
    void setStartTime(double monotonic_time) override;
    double timeOffset() const override;
    void setTimeOffset(double monotonic_time) override;
    Direction direction() const override;
    void setDirection(Direction) override;
    double playbackRate() const override;
    void setPlaybackRate(double playback_rate) override;
    FillMode fillMode() const override;
    void setFillMode(blink::WebCompositorAnimation::FillMode fill_mode) override;
    WTF::PassOwnPtr<mc::AnimationObj> passAnimation();

private:
    WTF::OwnPtr<mc::AnimationObj> m_animation;
};

}  // namespace mc_blink

#endif  // mc_blink_WebAnimationImpl_h
