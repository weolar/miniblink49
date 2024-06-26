// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BLINK_WEB_TO_CC_ANIMATION_DELEGATE_ADAPTER_H_
#define CC_BLINK_WEB_TO_CC_ANIMATION_DELEGATE_ADAPTER_H_

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "cc/animation/animation_delegate.h"

namespace blink {
class WebCompositorAnimationDelegate;
}

namespace cc_blink {

class WebToCCAnimationDelegateAdapter : public cc::AnimationDelegate {
public:
    explicit WebToCCAnimationDelegateAdapter(
        blink::WebCompositorAnimationDelegate* delegate);

private:
    void NotifyAnimationStarted(base::TimeTicks monotonic_time,
        cc::Animation::TargetProperty target_property,
        int group) override;
    void NotifyAnimationFinished(base::TimeTicks monotonic_time,
        cc::Animation::TargetProperty target_property,
        int group) override;

    blink::WebCompositorAnimationDelegate* delegate_;

    DISALLOW_COPY_AND_ASSIGN(WebToCCAnimationDelegateAdapter);
};

} // namespace cc_blink

#endif // CC_BLINK_WEB_TO_CC_ANIMATION_DELEGATE_ADAPTER_H_
