// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_animation_AnimationDelegate_h
#define mc_animation_AnimationDelegate_h

#include "base/time/time.h"
#include "mc/animation/AnimationObj.h"

namespace mc {

class AnimationDelegate {
public:
    virtual void notifyAnimationStarted(base::TimeTicks monotonicTime, AnimationObj::TargetProperty targetProperty, int group) = 0;
    virtual void notifyAnimationFinished(base::TimeTicks monotonicTime, AnimationObj::TargetProperty targetProperty, int group) = 0;

protected:
    virtual ~AnimationDelegate() {}
};

}  // namespace cc

#endif  // CC_ANIMATION_ANIMATION_DELEGATE_H_
