// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PartPainter_h
#define PartPainter_h

namespace blink {

struct PaintInfo;
class LayoutPoint;
class LayoutPart;

class PartPainter {
public:
    PartPainter(LayoutPart& layoutPart) : m_layoutPart(layoutPart) { }

    void paint(const PaintInfo&, const LayoutPoint&);
    void paintContents(const PaintInfo&, const LayoutPoint&);

private:
    LayoutPart& m_layoutPart;
};

} // namespace blink

#endif // PartPainter_h
