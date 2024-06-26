// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_animation_AnimationIdProvider_h
#define mc_animation_AnimationIdProvider_h

namespace mc {

class AnimationIdProvider {
public:
    // These functions each return monotonically increasing values.
    static int nextAnimationId();
    static int nextGroupId();
    static int nextTimelineId();
    static int nextPlayerId();

private:
};

}  // namespace cc

#endif  // CC_ANIMATION_ANIMATION_ID_PROVIDER_H_
