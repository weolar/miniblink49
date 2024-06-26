// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_animation_LayerAnimationValueProvider_h
#define mc_animation_LayerAnimationValueProvider_h

#include "third_party/WebKit/Source/platform/geometry/FloatPoint.h"

namespace mc {

// A LayerAnimationValueProvider is used for determining the starting value
// for animations that start at their 'current' value rather than at a
// pre-specified value.
class LayerAnimationValueProvider {
public:
    virtual ~LayerAnimationValueProvider() {}

    virtual blink::FloatPoint scrollOffsetForAnimation() const = 0;
};

}  // namespace mc

#endif  // mc_animation_LayerAnimationValueProvider_h
