// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SVGShapePainter_h
#define SVGShapePainter_h

#include "third_party/skia/include/core/SkPath.h"

class SkPaint;

namespace blink {

class FloatRect;
struct MarkerPosition;
struct PaintInfo;
class FloatPoint;
class GraphicsContext;
class LayoutSVGResourceMarker;
class LayoutSVGShape;

class SVGShapePainter {
public:
    SVGShapePainter(LayoutSVGShape& layoutSVGShape) : m_layoutSVGShape(layoutSVGShape) { }

    void paint(const PaintInfo&);

private:
    void fillShape(GraphicsContext*, const SkPaint&, SkPath::FillType);
    void strokeShape(GraphicsContext*, const SkPaint&);

    void paintMarkers(const PaintInfo&, const FloatRect& boundingBox);
    void paintMarker(const PaintInfo&, LayoutSVGResourceMarker&, const MarkerPosition&, float);
    void strokeZeroLengthLineCaps(GraphicsContext*, const SkPaint&);

    LayoutSVGShape& m_layoutSVGShape;
};

} // namespace blink

#endif // SVGShapePainter_h
