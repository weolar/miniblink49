// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ObjectPainter_h
#define ObjectPainter_h

#include "core/style/ComputedStyleConstants.h"
#include "wtf/Vector.h"

namespace blink {

class Color;
class GraphicsContext;
class LayoutPoint;
class LayoutRect;
struct PaintInfo;
class LayoutObject;
class ComputedStyle;

class ObjectPainter {
public:
    ObjectPainter(LayoutObject& layoutObject) : m_layoutObject(layoutObject) { }

    void paintOutline(const PaintInfo&, const LayoutRect& objectBounds, const LayoutRect& visualOverflowBounds);
    void paintFocusRing(const PaintInfo&, const ComputedStyle&, const Vector<LayoutRect>& focusRingRects);
    void addPDFURLRectIfNeeded(const PaintInfo&, const LayoutPoint& paintOffset);

    static void drawLineForBoxSide(GraphicsContext*, int x1, int y1, int x2, int y2, BoxSide, Color, EBorderStyle, int adjbw1, int adjbw2, bool antialias = false);

    static LayoutRect outlineBounds(const LayoutRect& objectBounds, const ComputedStyle&);

private:
    static void drawDashedOrDottedBoxSide(GraphicsContext*, int x1, int y1, int x2, int y2,
        BoxSide, Color, int thickness, EBorderStyle, bool antialias);
    static void drawDoubleBoxSide(GraphicsContext*, int x1, int y1, int x2, int y2,
        int length, BoxSide, Color, int thickness, int adjacentWidth1, int adjacentWidth2, bool antialias);
    static void drawRidgeOrGrooveBoxSide(GraphicsContext*, int x1, int y1, int x2, int y2,
        BoxSide, Color, EBorderStyle, int adjacentWidth1, int adjacentWidth2, bool antialias);
    static void drawSolidBoxSide(GraphicsContext*, int x1, int y1, int x2, int y2,
        BoxSide, Color, int adjacentWidth1, int adjacentWidth2, bool antialias);

    LayoutObject& m_layoutObject;
};

} // namespace blink

#endif
