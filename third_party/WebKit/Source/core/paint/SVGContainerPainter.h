// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SVGContainerPainter_h
#define SVGContainerPainter_h

namespace blink {

struct PaintInfo;
class LayoutSVGContainer;

class SVGContainerPainter {
public:
    SVGContainerPainter(LayoutSVGContainer& layoutSVGContainer) : m_layoutSVGContainer(layoutSVGContainer) { }

    void paint(const PaintInfo&);

private:
    LayoutSVGContainer& m_layoutSVGContainer;
};

} // namespace blink

#endif // SVGContainerPainter_h
