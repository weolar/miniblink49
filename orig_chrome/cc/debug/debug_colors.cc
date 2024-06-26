// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/macros.h"

#include "cc/debug/debug_colors.h"

#include "cc/trees/layer_tree_impl.h"

namespace cc {

static float Scale(float width, const LayerTreeImpl* tree_impl)
{
    return width * (tree_impl ? tree_impl->device_scale_factor() : 1);
}

// ======= Layer border colors =======

// Tiled content layers are orange.
SkColor DebugColors::TiledContentLayerBorderColor()
{
    return SkColorSetARGB(128, 255, 128, 0);
}
int DebugColors::TiledContentLayerBorderWidth(const LayerTreeImpl* tree_impl)
{
    return Scale(2, tree_impl);
}

// Image layers are olive.
SkColor DebugColors::ImageLayerBorderColor()
{
    return SkColorSetARGB(128, 128, 128, 0);
}
int DebugColors::ImageLayerBorderWidth(const LayerTreeImpl* tree_impl)
{
    return Scale(2, tree_impl);
}

// Non-tiled content layers area green.
SkColor DebugColors::ContentLayerBorderColor()
{
    return SkColorSetARGB(128, 0, 128, 32);
}
int DebugColors::ContentLayerBorderWidth(const LayerTreeImpl* tree_impl)
{
    return Scale(2, tree_impl);
}

// Masking layers are pale blue and wide.
SkColor DebugColors::MaskingLayerBorderColor()
{
    return SkColorSetARGB(48, 128, 255, 255);
}
int DebugColors::MaskingLayerBorderWidth(const LayerTreeImpl* tree_impl)
{
    return Scale(20, tree_impl);
}

// Other container layers are yellow.
SkColor DebugColors::ContainerLayerBorderColor()
{
    return SkColorSetARGB(192, 255, 255, 0);
}
int DebugColors::ContainerLayerBorderWidth(const LayerTreeImpl* tree_impl)
{
    return Scale(2, tree_impl);
}

// Surface layers are a blue-ish green.
SkColor DebugColors::SurfaceLayerBorderColor()
{
    return SkColorSetARGB(128, 0, 255, 136);
}
int DebugColors::SurfaceLayerBorderWidth(const LayerTreeImpl* tree_impl)
{
    return Scale(2, tree_impl);
}

// Render surfaces are blue.
SkColor DebugColors::SurfaceBorderColor()
{
    return SkColorSetARGB(100, 0, 0, 255);
}
int DebugColors::SurfaceBorderWidth(const LayerTreeImpl* tree_impl)
{
    return Scale(2, tree_impl);
}

// Replicas of render surfaces are purple.
SkColor DebugColors::SurfaceReplicaBorderColor()
{
    return SkColorSetARGB(100, 160, 0, 255);
}
int DebugColors::SurfaceReplicaBorderWidth(const LayerTreeImpl* tree_impl)
{
    return Scale(2, tree_impl);
}

// ======= Tile colors =======

// High-res tile borders are cyan.
SkColor DebugColors::HighResTileBorderColor()
{
    return SkColorSetARGB(100, 80, 200, 200);
}
int DebugColors::HighResTileBorderWidth(const LayerTreeImpl* tree_impl)
{
    return Scale(1, tree_impl);
}

// Low-res tile borders are purple.
SkColor DebugColors::LowResTileBorderColor()
{
    return SkColorSetARGB(100, 212, 83, 192);
}
int DebugColors::LowResTileBorderWidth(const LayerTreeImpl* tree_impl)
{
    return Scale(2, tree_impl);
}

// Other high-resolution tile borders are yellow.
SkColor DebugColors::ExtraHighResTileBorderColor()
{
    return SkColorSetARGB(100, 239, 231, 20);
}
int DebugColors::ExtraHighResTileBorderWidth(const LayerTreeImpl* tree_impl)
{
    return Scale(2, tree_impl);
}

// Other low-resolution tile borders are green.
SkColor DebugColors::ExtraLowResTileBorderColor()
{
    return SkColorSetARGB(100, 93, 186, 18);
}
int DebugColors::ExtraLowResTileBorderWidth(const LayerTreeImpl* tree_impl)
{
    return Scale(2, tree_impl);
}

// Missing tile borders are dark grey.
SkColor DebugColors::MissingTileBorderColor()
{
    return SkColorSetARGB(64, 64, 64, 0);
}
int DebugColors::MissingTileBorderWidth(const LayerTreeImpl* tree_impl)
{
    return Scale(1, tree_impl);
}

// Solid color tile borders are grey.
SkColor DebugColors::SolidColorTileBorderColor()
{
    return SkColorSetARGB(128, 128, 128, 128);
}
int DebugColors::SolidColorTileBorderWidth(const LayerTreeImpl* tree_impl)
{
    return Scale(1, tree_impl);
}

// OOM tile borders are red.
SkColor DebugColors::OOMTileBorderColor()
{
    return SkColorSetARGB(100, 255, 0, 0);
}
int DebugColors::OOMTileBorderWidth(const LayerTreeImpl* tree_impl)
{
    return Scale(1, tree_impl);
}

// Direct picture borders are chartreuse.
SkColor DebugColors::DirectPictureBorderColor()
{
    return SkColorSetARGB(255, 127, 255, 0);
}
int DebugColors::DirectPictureBorderWidth(const LayerTreeImpl* tree_impl)
{
    return Scale(1, tree_impl);
}

// ======= Checkerboard colors =======

// Non-debug checkerboards are grey.
SkColor DebugColors::DefaultCheckerboardColor()
{
    return SkColorSetRGB(241, 241, 241);
}

// Invalidated tiles get sky blue checkerboards.
SkColor DebugColors::InvalidatedTileCheckerboardColor()
{
    return SkColorSetRGB(128, 200, 245);
}

// Evicted tiles get pale red checkerboards.
SkColor DebugColors::EvictedTileCheckerboardColor()
{
    return SkColorSetRGB(255, 200, 200);
}

// ======= Debug rect colors =======

static SkColor FadedGreen(int initial_value, int step)
{
    DCHECK_GE(step, 0);
    DCHECK_LE(step, DebugColors::kFadeSteps);
    int value = step * initial_value / DebugColors::kFadeSteps;
    return SkColorSetARGB(value, 0, 195, 0);
}
// Paint rects in green.
SkColor DebugColors::PaintRectBorderColor(int step)
{
    return FadedGreen(255, step);
}
int DebugColors::PaintRectBorderWidth() { return 2; }
SkColor DebugColors::PaintRectFillColor(int step)
{
    return FadedGreen(60, step);
}

// Property-changed rects in blue.
SkColor DebugColors::PropertyChangedRectBorderColor()
{
    return SkColorSetARGB(255, 0, 0, 255);
}
int DebugColors::PropertyChangedRectBorderWidth() { return 2; }
SkColor DebugColors::PropertyChangedRectFillColor()
{
    return SkColorSetARGB(30, 0, 0, 255);
}

// Surface damage rects in yellow-orange.
SkColor DebugColors::SurfaceDamageRectBorderColor()
{
    return SkColorSetARGB(255, 200, 100, 0);
}
int DebugColors::SurfaceDamageRectBorderWidth() { return 2; }
SkColor DebugColors::SurfaceDamageRectFillColor()
{
    return SkColorSetARGB(30, 200, 100, 0);
}

// Surface replica screen space rects in green.
SkColor DebugColors::ScreenSpaceLayerRectBorderColor()
{
    return SkColorSetARGB(255, 100, 200, 0);
}
int DebugColors::ScreenSpaceLayerRectBorderWidth() { return 2; }
SkColor DebugColors::ScreenSpaceLayerRectFillColor()
{
    return SkColorSetARGB(30, 100, 200, 0);
}

// Layer screen space rects in purple.
SkColor DebugColors::ScreenSpaceSurfaceReplicaRectBorderColor()
{
    return SkColorSetARGB(255, 100, 0, 200);
}
int DebugColors::ScreenSpaceSurfaceReplicaRectBorderWidth() { return 2; }
SkColor DebugColors::ScreenSpaceSurfaceReplicaRectFillColor()
{
    return SkColorSetARGB(10, 100, 0, 200);
}

// Touch-event-handler rects in yellow.
SkColor DebugColors::TouchEventHandlerRectBorderColor()
{
    return SkColorSetARGB(255, 239, 229, 60);
}
int DebugColors::TouchEventHandlerRectBorderWidth() { return 2; }
SkColor DebugColors::TouchEventHandlerRectFillColor()
{
    return SkColorSetARGB(30, 239, 229, 60);
}

// Wheel-event-handler rects in green.
SkColor DebugColors::WheelEventHandlerRectBorderColor()
{
    return SkColorSetARGB(255, 189, 209, 57);
}
int DebugColors::WheelEventHandlerRectBorderWidth() { return 2; }
SkColor DebugColors::WheelEventHandlerRectFillColor()
{
    return SkColorSetARGB(30, 189, 209, 57);
}

// Scroll-event-handler rects in teal.
SkColor DebugColors::ScrollEventHandlerRectBorderColor()
{
    return SkColorSetARGB(255, 24, 167, 181);
}
int DebugColors::ScrollEventHandlerRectBorderWidth() { return 2; }
SkColor DebugColors::ScrollEventHandlerRectFillColor()
{
    return SkColorSetARGB(30, 24, 167, 181);
}

// Non-fast-scrollable rects in orange.
SkColor DebugColors::NonFastScrollableRectBorderColor()
{
    return SkColorSetARGB(255, 238, 163, 59);
}
int DebugColors::NonFastScrollableRectBorderWidth() { return 2; }
SkColor DebugColors::NonFastScrollableRectFillColor()
{
    return SkColorSetARGB(30, 238, 163, 59);
}

// Animation bounds are lime-green.
SkColor DebugColors::LayerAnimationBoundsBorderColor()
{
    return SkColorSetARGB(255, 112, 229, 0);
}
int DebugColors::LayerAnimationBoundsBorderWidth() { return 2; }
SkColor DebugColors::LayerAnimationBoundsFillColor()
{
    return SkColorSetARGB(30, 112, 229, 0);
}

// Non-Painted rects in cyan.
SkColor DebugColors::NonPaintedFillColor() { return SK_ColorCYAN; }

// Missing picture rects in magenta.
SkColor DebugColors::MissingPictureFillColor() { return SK_ColorMAGENTA; }

// Missing resize invalidations are in salmon pink.
SkColor DebugColors::MissingResizeInvalidations()
{
    return SkColorSetARGB(255, 255, 155, 170);
}

// Picture borders in transparent blue.
SkColor DebugColors::PictureBorderColor()
{
    return SkColorSetARGB(100, 0, 0, 200);
}

// ======= HUD widget colors =======

SkColor DebugColors::HUDBackgroundColor()
{
    return SkColorSetARGB(215, 17, 17, 17);
}
SkColor DebugColors::HUDSeparatorLineColor()
{
    return SkColorSetARGB(255, 130, 130, 130);
}
SkColor DebugColors::HUDIndicatorLineColor()
{
    return SkColorSetARGB(255, 80, 80, 80);
}

SkColor DebugColors::PlatformLayerTreeTextColor() { return SK_ColorRED; }
SkColor DebugColors::FPSDisplayTextAndGraphColor() { return SK_ColorRED; }
SkColor DebugColors::MemoryDisplayTextColor()
{
    return SkColorSetARGB(255, 220, 220, 220);
}

// Paint time display in green (similar to paint times in the WebInspector)
SkColor DebugColors::PaintTimeDisplayTextAndGraphColor()
{
    return SkColorSetRGB(75, 155, 55);
}

} // namespace cc
