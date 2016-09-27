// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ImageAnimationPolicy_h
#define ImageAnimationPolicy_h

namespace blink {

// ImageAnimationPolicy is used for controlling image animation
// when image frame is rendered for animation

enum ImageAnimationPolicy {
    // Animate the image (the default).
    ImageAnimationPolicyAllowed,
    // Animate image just once.
    ImageAnimationPolicyAnimateOnce,
    // Show the first frame and do not animate.
    ImageAnimationPolicyNoAnimation
};

} // namespace blink

#endif // ImageAnimationPolicy_h
