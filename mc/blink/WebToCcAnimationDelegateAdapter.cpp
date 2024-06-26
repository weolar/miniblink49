// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/blink/WebToCCAnimationDelegateAdapter.h"
#include "third_party/WebKit/public/platform/WebCompositorAnimationDelegate.h"

namespace mc_blink {

WebToCCAnimationDelegateAdapter::WebToCCAnimationDelegateAdapter(blink::WebCompositorAnimationDelegate* delegate)
    : m_delegate(delegate) 
{
}

void WebToCCAnimationDelegateAdapter::notifyAnimationStarted(base::TimeTicks monotonic_time, mc::AnimationObj::TargetProperty target_property, int group)
{
    m_delegate->notifyAnimationStarted(
        (monotonic_time - base::TimeTicks()).InSecondsF(),
#if WEB_COMPOSITOR_ANIMATION_DELEGATE_TAKES_GROUP
        group
#else
        static_cast<blink::WebCompositorAnimation::TargetProperty>(target_property)
#endif
  );
}

void WebToCCAnimationDelegateAdapter::notifyAnimationFinished(base::TimeTicks monotonic_time, mc::AnimationObj::TargetProperty target_property, int group) 
{
    m_delegate->notifyAnimationFinished(
        (monotonic_time - base::TimeTicks()).InSecondsF(),
#if WEB_COMPOSITOR_ANIMATION_DELEGATE_TAKES_GROUP
        group
#else
        static_cast<blink::WebCompositorAnimation::TargetProperty>(target_property)
#endif
        );
}

}  // namespace mc_blink
