// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_blink_WebToCcAnimationDelegateAdapter_h
#define mc_blink_WebToCcAnimationDelegateAdapter_h

#include "mc/animation/AnimationDelegate.h"

namespace blink {
class WebCompositorAnimationDelegate;
}

namespace mc_blink {

class WebToCCAnimationDelegateAdapter : public mc::AnimationDelegate {
public:
    explicit WebToCCAnimationDelegateAdapter(blink::WebCompositorAnimationDelegate* delegate);

private:
    void notifyAnimationStarted(base::TimeTicks monotonic_time,
        mc::AnimationObj::TargetProperty target_property,
        int group) override;
    void notifyAnimationFinished(base::TimeTicks monotonic_time,
        mc::AnimationObj::TargetProperty target_property,
        int group) override;

    blink::WebCompositorAnimationDelegate* m_delegate;
};

}  // namespace mc_blink

#endif  // mc_blink_WebToCcAnimationDelegateAdapter_h
