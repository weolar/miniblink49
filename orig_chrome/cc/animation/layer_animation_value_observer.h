// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_ANIMATION_LAYER_ANIMATION_VALUE_OBSERVER_H_
#define CC_ANIMATION_LAYER_ANIMATION_VALUE_OBSERVER_H_

#include "cc/base/cc_export.h"

namespace gfx {
class ScrollOffset;
class Transform;
}

namespace cc {

class FilterOperations;

class CC_EXPORT LayerAnimationValueObserver {
public:
    virtual ~LayerAnimationValueObserver() { }

    virtual void OnFilterAnimated(const FilterOperations& filters) = 0;
    virtual void OnOpacityAnimated(float opacity) = 0;
    virtual void OnTransformAnimated(const gfx::Transform& transform) = 0;
    virtual void OnScrollOffsetAnimated(
        const gfx::ScrollOffset& scroll_offset)
        = 0;
    virtual void OnAnimationWaitingForDeletion() = 0;
    virtual void OnTransformIsPotentiallyAnimatingChanged(bool is_animating) = 0;
    virtual bool IsActive() const = 0;
};

} // namespace cc

#endif // CC_ANIMATION_LAYER_ANIMATION_VALUE_OBSERVER_H_
