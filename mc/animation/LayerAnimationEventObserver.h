// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_animation_LayerAnimationEventObserver_h
#define mc_animation_LayerAnimationEventObserver_h

namespace mc {

struct AnimationEvent;

class LayerAnimationEventObserver {
public:
    virtual void onAnimationStarted(const AnimationEvent& event) = 0;

protected:
    virtual ~LayerAnimationEventObserver() {}
};

}  // namespace mc

#endif  // mc_animation_LayerAnimationEventObserver_h

