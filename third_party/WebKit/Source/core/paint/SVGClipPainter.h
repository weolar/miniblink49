// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SVGClipPainter_h
#define SVGClipPainter_h

#include "platform/geometry/FloatRect.h"
#include "platform/graphics/paint/DisplayItemClient.h"

namespace blink {

class AffineTransform;
class GraphicsContext;
class LayoutObject;
class LayoutSVGResourceClipper;

class SVGClipPainter {
public:
    enum ClipperState {
        ClipperNotApplied,
        ClipperAppliedPath,
        ClipperAppliedMask
    };

    SVGClipPainter(LayoutSVGResourceClipper& clip) : m_clip(clip) { }

    // FIXME: Filters are also stateful resources that could benefit from having their state managed
    //        on the caller stack instead of the current hashmap. We should look at refactoring these
    //        into a general interface that can be shared.
    bool applyStatefulResource(const LayoutObject&, GraphicsContext*, ClipperState&);
    void postApplyStatefulResource(const LayoutObject&, GraphicsContext*, ClipperState&);

    // clipPath can be clipped too, but don't have a boundingBox or paintInvalidationRect. So we can't call
    // applyResource directly and use the rects from the object, since they are empty for LayoutSVGResources
    // FIXME: We made applyClippingToContext public because we cannot call applyResource on HTML elements (it asserts on LayoutObject::objectBoundingBox)
    bool applyClippingToContext(const LayoutObject&, const FloatRect&, const FloatRect&, GraphicsContext*, ClipperState&);

private:
    void drawClipMaskContent(GraphicsContext*, const LayoutObject&, const FloatRect& targetBoundingBox, const FloatRect& targetPaintInvalidationRect);

    LayoutSVGResourceClipper& m_clip;
};

} // namespace blink

#endif // SVGClipPainter_h
