// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_DEBUG_COLORS_H_
#define CC_DEBUG_DEBUG_COLORS_H_

#include "base/basictypes.h"
#include "third_party/skia/include/core/SkColor.h"

namespace cc {

class LayerTreeImpl;

class DebugColors {
public:
    static SkColor TiledContentLayerBorderColor();
    static int TiledContentLayerBorderWidth(const LayerTreeImpl* tree_impl);

    static SkColor ImageLayerBorderColor();
    static int ImageLayerBorderWidth(const LayerTreeImpl* tree_impl);

    static SkColor ContentLayerBorderColor();
    static int ContentLayerBorderWidth(const LayerTreeImpl* tree_impl);

    static SkColor MaskingLayerBorderColor();
    static int MaskingLayerBorderWidth(const LayerTreeImpl* tree_impl);

    static SkColor ContainerLayerBorderColor();
    static int ContainerLayerBorderWidth(const LayerTreeImpl* tree_impl);

    static SkColor SurfaceLayerBorderColor();
    static int SurfaceLayerBorderWidth(const LayerTreeImpl* tree_impl);

    static SkColor SurfaceBorderColor();
    static int SurfaceBorderWidth(const LayerTreeImpl* tree_impl);

    static SkColor SurfaceReplicaBorderColor();
    static int SurfaceReplicaBorderWidth(const LayerTreeImpl* tree_impl);

    static SkColor HighResTileBorderColor();
    static int HighResTileBorderWidth(const LayerTreeImpl* tree_impl);

    static SkColor LowResTileBorderColor();
    static int LowResTileBorderWidth(const LayerTreeImpl* tree_impl);

    static SkColor ExtraHighResTileBorderColor();
    static int ExtraHighResTileBorderWidth(const LayerTreeImpl* tree_impl);

    static SkColor ExtraLowResTileBorderColor();
    static int ExtraLowResTileBorderWidth(const LayerTreeImpl* tree_impl);

    static SkColor MissingTileBorderColor();
    static int MissingTileBorderWidth(const LayerTreeImpl* tree_impl);

    static SkColor SolidColorTileBorderColor();
    static int SolidColorTileBorderWidth(const LayerTreeImpl* tree_impl);

    static SkColor OOMTileBorderColor();
    static int OOMTileBorderWidth(const LayerTreeImpl* tree_impl);

    static SkColor DirectPictureBorderColor();
    static int DirectPictureBorderWidth(const LayerTreeImpl* tree_impl);

    static SkColor DefaultCheckerboardColor();
    static SkColor EvictedTileCheckerboardColor();
    static SkColor InvalidatedTileCheckerboardColor();

    static const int kFadeSteps = 50;
    static SkColor PaintRectBorderColor(int step);
    static int PaintRectBorderWidth();
    static SkColor PaintRectFillColor(int step);

    static SkColor PropertyChangedRectBorderColor();
    static int PropertyChangedRectBorderWidth();
    static SkColor PropertyChangedRectFillColor();

    static SkColor SurfaceDamageRectBorderColor();
    static int SurfaceDamageRectBorderWidth();
    static SkColor SurfaceDamageRectFillColor();

    static SkColor ScreenSpaceLayerRectBorderColor();
    static int ScreenSpaceLayerRectBorderWidth();
    static SkColor ScreenSpaceLayerRectFillColor();

    static SkColor ScreenSpaceSurfaceReplicaRectBorderColor();
    static int ScreenSpaceSurfaceReplicaRectBorderWidth();
    static SkColor ScreenSpaceSurfaceReplicaRectFillColor();

    static SkColor TouchEventHandlerRectBorderColor();
    static int TouchEventHandlerRectBorderWidth();
    static SkColor TouchEventHandlerRectFillColor();

    static SkColor WheelEventHandlerRectBorderColor();
    static int WheelEventHandlerRectBorderWidth();
    static SkColor WheelEventHandlerRectFillColor();

    static SkColor ScrollEventHandlerRectBorderColor();
    static int ScrollEventHandlerRectBorderWidth();
    static SkColor ScrollEventHandlerRectFillColor();

    static SkColor NonFastScrollableRectBorderColor();
    static int NonFastScrollableRectBorderWidth();
    static SkColor NonFastScrollableRectFillColor();

    static SkColor LayerAnimationBoundsBorderColor();
    static int LayerAnimationBoundsBorderWidth();
    static SkColor LayerAnimationBoundsFillColor();

    static SkColor NonPaintedFillColor();
    static SkColor MissingPictureFillColor();
    static SkColor MissingResizeInvalidations();
    static SkColor PictureBorderColor();

    static SkColor HUDBackgroundColor();
    static SkColor HUDSeparatorLineColor();
    static SkColor HUDIndicatorLineColor();

    static SkColor PlatformLayerTreeTextColor();
    static SkColor FPSDisplayTextAndGraphColor();
    static SkColor MemoryDisplayTextColor();
    static SkColor PaintTimeDisplayTextAndGraphColor();

private:
    DISALLOW_IMPLICIT_CONSTRUCTORS(DebugColors);
};

} // namespace cc

#endif // CC_DEBUG_DEBUG_COLORS_H_
