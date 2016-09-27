// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SVGRootPainter_h
#define SVGRootPainter_h

namespace blink {

struct PaintInfo;
class LayoutPoint;
class LayoutSVGRoot;

class SVGRootPainter {
public:
    SVGRootPainter(LayoutSVGRoot& layoutSVGRoot) : m_layoutSVGRoot(layoutSVGRoot) { }

    void paint(const PaintInfo&, const LayoutPoint&);

private:
    LayoutSVGRoot& m_layoutSVGRoot;
};

} // namespace blink

#endif // SVGRootPainter_h
