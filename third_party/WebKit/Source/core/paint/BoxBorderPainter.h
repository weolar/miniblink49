// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BoxBorderPainter_h
#define BoxBorderPainter_h

#include "core/layout/LayoutBoxModelObject.h"
#include "core/style/BorderEdge.h"
#include "platform/geometry/FloatRoundedRect.h"
#include "platform/heap/Heap.h"

namespace blink {

class ComputedStyle;
class IntRect;
class LayoutBox;
class LayoutRect;
struct PaintInfo;

class BoxBorderPainter {
    STACK_ALLOCATED();
public:
    BoxBorderPainter(const LayoutRect& borderRect, const ComputedStyle&, const IntRect& clipRect,
        BackgroundBleedAvoidance, bool includeLogicalLeftEdge, bool includeLogicalRightEdge);

    BoxBorderPainter(const ComputedStyle&, const LayoutRect& outer, const LayoutRect& inner,
        const BorderEdge& uniformEdgeInfo);

    void paintBorder(const PaintInfo&, const LayoutRect& borderRect) const;

private:
    struct ComplexBorderInfo;
    enum MiterType {
        NoMiter,
        SoftMiter, // Anti-aliased
        HardMiter, // Not anti-aliased
    };

    void computeBorderProperties();

    BorderEdgeFlags paintOpacityGroup(GraphicsContext*, const ComplexBorderInfo&, unsigned index,
        float accumulatedOpacity) const;
    void paintSide(GraphicsContext*, const ComplexBorderInfo&, BoxSide, unsigned alpha, BorderEdgeFlags) const;
    void paintOneBorderSide(GraphicsContext*, const FloatRect& sideRect, BoxSide, BoxSide adjacentSide1,
        BoxSide adjacentSide2, const Path*, bool antialias, Color, BorderEdgeFlags) const;
    bool paintBorderFastPath(GraphicsContext*, const LayoutRect& borderRect) const;
    void drawDoubleBorder(GraphicsContext*, const LayoutRect& borderRect) const;

    void drawBoxSideFromPath(GraphicsContext*, const LayoutRect&, const Path&, float thickness,
        float drawThickness, BoxSide, Color, EBorderStyle) const;
    void clipBorderSidePolygon(GraphicsContext*, BoxSide, MiterType miter1, MiterType miter2) const;
    void clipBorderSideForComplexInnerPath(GraphicsContext*, BoxSide) const;

    MiterType computeMiter(BoxSide, BoxSide adjacentSide, BorderEdgeFlags, bool antialias) const;
    static bool mitersRequireClipping(MiterType miter1, MiterType miter2, EBorderStyle, bool antialias);

    const BorderEdge& firstEdge() const
    {
        ASSERT(m_visibleEdgeSet);
        return m_edges[m_firstVisibleEdge];
    }

    // const inputs
    const ComputedStyle& m_style;
    const BackgroundBleedAvoidance m_bleedAvoidance;
    const bool m_includeLogicalLeftEdge;
    const bool m_includeLogicalRightEdge;

    // computed attributes
    FloatRoundedRect m_outer;
    FloatRoundedRect m_inner;
    BorderEdge m_edges[4];

    unsigned m_visibleEdgeCount;
    unsigned m_firstVisibleEdge;
    BorderEdgeFlags m_visibleEdgeSet;

    bool m_isUniformStyle;
    bool m_isUniformWidth;
    bool m_isUniformColor;
    bool m_isRounded;
    bool m_hasAlpha;
};

} // namespace blink

#endif
