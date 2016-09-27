// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SVGRootInlineBoxPainter_h
#define SVGRootInlineBoxPainter_h

namespace blink {

struct PaintInfo;
class LayoutPoint;
class SVGRootInlineBox;

class SVGRootInlineBoxPainter {
public:
    SVGRootInlineBoxPainter(SVGRootInlineBox& svgRootInlineBox) : m_svgRootInlineBox(svgRootInlineBox) { }

    void paint(const PaintInfo&, const LayoutPoint&);

private:
    SVGRootInlineBox& m_svgRootInlineBox;
};

} // namespace blink

#endif // SVGRootInlineBoxPainter_h
