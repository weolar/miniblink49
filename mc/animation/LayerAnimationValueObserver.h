// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_animation_LayerAnimationValueObserver_h
#define mc_animation_LayerAnimationValueObserver_h

#include "mc/animation/FilterOperationsWrap.h"

namespace mc {

class LayerAnimationValueObserver {
 public:
  virtual ~LayerAnimationValueObserver() {}

  virtual void onFilterAnimated(const FilterOperationsWrap& filters) = 0;
  virtual void onOpacityAnimated(float opacity) = 0;
  virtual void onTransformAnimated(const SkMatrix44& transform) = 0;
  virtual void onScrollOffsetAnimated(const blink::FloatPoint& scrollOffset) = 0;
  virtual void onAnimationWaitingForDeletion() = 0;
  virtual void onTransformIsPotentiallyAnimatingChanged(bool isAnimating) = 0;
  virtual bool isActive() const = 0;
};

}  // namespace mc

#endif  // mc_animation_LayerAnimationValueObserver_h
