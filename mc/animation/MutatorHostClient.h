// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_animation_MutatorHostClient_h
#define mc_animation_MutatorHostClient_h

#include "third_party/WebKit/Source/platform/geometry/FloatPoint.h"
#include "third_party/skia/include/utils/SkMatrix44.h"

namespace mc {

class Layer;

enum class LayerTreeType { ACTIVE, PENDING };

class MutatorHostClient {
 public:
  virtual bool isLayerInTree(int layerId, LayerTreeType treeType) const = 0;
  virtual void setMutatorsNeedCommit() = 0;

  virtual void setLayerFilterMutated(int layerId, LayerTreeType treeType, const FilterOperations& filters) = 0;
  virtual void setLayerOpacityMutated(int layerId, LayerTreeType treeType, float opacity) = 0;
  virtual void setLayerTransformMutated(int layerId, LayerTreeType treeType, const SkMatrix44& transform) = 0;
  virtual void setLayerScrollOffsetMutated(int layerId, LayerTreeType treeType, const blink::FloatPoint& scrollOffset) = 0;

  virtual void layerTransformIsPotentiallyAnimatingChanged(int layerId, LayerTreeType treeType, bool isAnimating) = 0;

  virtual void scrollOffsetAnimationFinished() = 0;
  virtual blink::FloatPoint getScrollOffsetForAnimation(int layerId) const = 0;
};

}  // namespace cc

#endif  // CC_TREES_MUTATOR_HOST_CLIENT_H_
