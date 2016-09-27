// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SVGInlineFlowBoxPainter_h
#define SVGInlineFlowBoxPainter_h

namespace blink {

struct PaintInfo;
class LayoutPoint;
class SVGInlineFlowBox;

class SVGInlineFlowBoxPainter {
public:
    SVGInlineFlowBoxPainter(SVGInlineFlowBox& svgInlineFlowBox) : m_svgInlineFlowBox(svgInlineFlowBox) { }

    void paintSelectionBackground(const PaintInfo&);
    void paint(const PaintInfo&, const LayoutPoint&);

private:
    SVGInlineFlowBox& m_svgInlineFlowBox;
};

} // namespace blink

#endif // SVGInlineFlowBoxPainter_h
