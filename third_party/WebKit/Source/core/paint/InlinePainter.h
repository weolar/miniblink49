// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InlinePainter_h
#define InlinePainter_h

#include "core/style/ComputedStyleConstants.h"
#include "wtf/Vector.h"

namespace blink {

class Color;
class GraphicsContext;
class LayoutPoint;
class LayoutRect;
struct PaintInfo;
class LayoutInline;

class InlinePainter {
public:
    InlinePainter(LayoutInline& layoutInline) : m_layoutInline(layoutInline) { }

    void paint(const PaintInfo&, const LayoutPoint& paintOffset);
    void paintOutline(const PaintInfo&, const LayoutPoint& paintOffset);

private:
    LayoutRect outlinePaintRect(const Vector<LayoutRect>&, const LayoutPoint& paintOffset) const;
    void paintOutlineForLine(GraphicsContext*, const LayoutPoint&, const LayoutRect& prevLine, const LayoutRect& thisLine,
        const LayoutRect& nextLine, const Color);
    LayoutInline& m_layoutInline;
};

} // namespace blink

#endif // InlinePainter_h
