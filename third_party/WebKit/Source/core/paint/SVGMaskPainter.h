// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SVGMaskPainter_h
#define SVGMaskPainter_h

#include "platform/geometry/FloatRect.h"

namespace blink {

class GraphicsContext;
class LayoutObject;
class LayoutSVGResourceMasker;

class SVGMaskPainter {
public:
    SVGMaskPainter(LayoutSVGResourceMasker& mask) : m_mask(mask) { }

    bool prepareEffect(const LayoutObject&, GraphicsContext*);
    void finishEffect(const LayoutObject&, GraphicsContext*);

private:
    void drawMaskForLayoutObject(GraphicsContext*, const LayoutObject&, const FloatRect& targetBoundingBox, const FloatRect& targetPaintInvalidationRect);

    LayoutSVGResourceMasker& m_mask;
};

} // namespace blink

#endif // SVGMaskPainter_h
