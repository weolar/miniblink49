// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BoxPainter_h
#define BoxPainter_h

#include "core/layout/LayoutBoxModelObject.h"
#include "core/paint/ObjectPainter.h"

namespace blink {

class BackgroundImageGeometry;
class FloatRoundedRect;
class LayoutPoint;
struct PaintInfo;
class LayoutBox;
class LayoutObject;

class BoxPainter {
public:
    BoxPainter(LayoutBox& layoutBox) : m_layoutBox(layoutBox) { }
    void paint(const PaintInfo&, const LayoutPoint&);

    void paintBoxDecorationBackground(const PaintInfo&, const LayoutPoint&);
    void paintMask(const PaintInfo&, const LayoutPoint&);
    void paintClippingMask(const PaintInfo&, const LayoutPoint&);

    typedef Vector<const FillLayer*, 8> FillLayerOcclusionOutputList;
    // Returns true if the result fill layers have non-associative blending or compositing mode.
    // (i.e. The rendering will be different without creating isolation group by context.saveLayer().)
    // Note that the output list will be in top-bottom order.
    bool calculateFillLayerOcclusionCulling(FillLayerOcclusionOutputList &reversedPaintList, const FillLayer&);

    void paintFillLayers(const PaintInfo&, const Color&, const FillLayer&, const LayoutRect&, BackgroundBleedAvoidance = BackgroundBleedNone, SkXfermode::Mode = SkXfermode::kSrcOver_Mode, LayoutObject* backgroundObject = nullptr);
    void paintMaskImages(const PaintInfo&, const LayoutRect&);
    void paintBoxDecorationBackgroundWithRect(const PaintInfo&, const LayoutPoint&, const LayoutRect&);
    static void paintFillLayerExtended(LayoutBoxModelObject&, const PaintInfo&, const Color&, const FillLayer&, const LayoutRect&, BackgroundBleedAvoidance, InlineFlowBox* = nullptr, const LayoutSize& = LayoutSize(), SkXfermode::Mode = SkXfermode::kSrcOver_Mode, LayoutObject* backgroundObject = nullptr);
    static void calculateBackgroundImageGeometry(LayoutBoxModelObject&, const LayoutBoxModelObject* paintContainer, const FillLayer&, const LayoutRect& paintRect, BackgroundImageGeometry&, LayoutObject* backgroundObject = nullptr);
    static InterpolationQuality chooseInterpolationQuality(LayoutObject&, GraphicsContext*, Image*, const void*, const LayoutSize&);
    static bool paintNinePieceImage(LayoutBoxModelObject&, GraphicsContext*, const LayoutRect&, const ComputedStyle&, const NinePieceImage&, SkXfermode::Mode = SkXfermode::kSrcOver_Mode);
    static void paintBorder(LayoutBoxModelObject&, const PaintInfo&, const LayoutRect&, const ComputedStyle&, BackgroundBleedAvoidance = BackgroundBleedNone, bool includeLogicalLeftEdge = true, bool includeLogicalRightEdge = true);
    static void paintBoxShadow(const PaintInfo&, const LayoutRect&, const ComputedStyle&, ShadowStyle, bool includeLogicalLeftEdge = true, bool includeLogicalRightEdge = true);
    static bool shouldAntialiasLines(GraphicsContext*);
    static bool allCornersClippedOut(const FloatRoundedRect& border, const IntRect& clipRect);
    static bool shouldForceWhiteBackgroundForPrintEconomy(const ComputedStyle&, const Document&);

private:
    void paintBackground(const PaintInfo&, const LayoutRect&, const Color& backgroundColor, BackgroundBleedAvoidance = BackgroundBleedNone);
    void paintFillLayer(const PaintInfo&, const Color&, const FillLayer&, const LayoutRect&, BackgroundBleedAvoidance, SkXfermode::Mode, LayoutObject* backgroundObject = nullptr);
    static FloatRoundedRect backgroundRoundedRectAdjustedForBleedAvoidance(LayoutObject&, const LayoutRect&, BackgroundBleedAvoidance, InlineFlowBox*, const LayoutSize&, bool includeLogicalLeftEdge, bool includeLogicalRightEdge);
    static FloatRoundedRect getBackgroundRoundedRect(LayoutObject&, const LayoutRect&, InlineFlowBox*, LayoutUnit inlineBoxWidth, LayoutUnit inlineBoxHeight,
        bool includeLogicalLeftEdge, bool includeLogicalRightEdge);
    static void applyBoxShadowForBackground(GraphicsContext*, LayoutObject&);
    static bool fixedBackgroundPaintsInLocalCoordinates(const LayoutObject&);
    static IntSize calculateFillTileSize(const LayoutBoxModelObject&, const FillLayer&, const IntSize& scaledPositioningAreaSize);

    LayoutRect boundsForDrawingRecorder(const LayoutPoint& paintOffset);

    // FIXME: this should be const.
    LayoutBox& m_layoutBox;
};

} // namespace blink

#endif
