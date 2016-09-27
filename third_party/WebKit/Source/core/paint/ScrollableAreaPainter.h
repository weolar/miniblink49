// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ScrollableAreaPainter_h
#define ScrollableAreaPainter_h

namespace blink {

class GraphicsContext;
class IntPoint;
class IntRect;
class DeprecatedPaintLayerScrollableArea;

class ScrollableAreaPainter {
public:
    ScrollableAreaPainter(DeprecatedPaintLayerScrollableArea& paintLayerScrollableArea) : m_scrollableArea(paintLayerScrollableArea) { }

    void paintResizer(GraphicsContext*, const IntPoint& paintOffset, const IntRect& damageRect);
    void paintOverflowControls(GraphicsContext*, const IntPoint& paintOffset, const IntRect& damageRect, bool paintingOverlayControls);
    void paintScrollCorner(GraphicsContext*, const IntPoint&, const IntRect& damageRect);

private:
    void drawPlatformResizerImage(GraphicsContext*, IntRect resizerCornerRect);
    bool overflowControlsIntersectRect(const IntRect& localRect) const;

    DeprecatedPaintLayerScrollableArea& m_scrollableArea;
};

} // namespace blink

#endif // ScrollableAreaPainter_h
