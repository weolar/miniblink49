// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BLINK_WEB_ANIMATION_IMPL_H_
#define CC_BLINK_WEB_ANIMATION_IMPL_H_

#include "base/memory/scoped_ptr.h"
#include "cc/blink/cc_blink_export.h"
#include "third_party/WebKit/public/platform/WebCompositorAnimation.h"

namespace cc {
class Animation;
}

namespace blink {
class WebCompositorAnimationCurve;
}

namespace cc_blink {

class WebCompositorAnimationImpl : public blink::WebCompositorAnimation {
public:
    CC_BLINK_EXPORT WebCompositorAnimationImpl(
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
    scoped_ptr<cc::Animation> PassAnimation();

private:
    scoped_ptr<cc::Animation> animation_;

    DISALLOW_COPY_AND_ASSIGN(WebCompositorAnimationImpl);
};

} // namespace cc_blink

#endif // CC_BLINK_WEB_ANIMATION_IMPL_H_
