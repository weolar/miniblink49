// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TableSectionPainter_h
#define TableSectionPainter_h

namespace blink {

class LayoutPoint;
class LayoutTableCell;
class LayoutTableSection;
struct PaintInfo;

class TableSectionPainter {
public:
    TableSectionPainter(LayoutTableSection& layoutTableSection) : m_layoutTableSection(layoutTableSection) { }

    void paint(const PaintInfo&, const LayoutPoint&);

private:
    void paintObject(const PaintInfo&, const LayoutPoint&);
    void paintCell(LayoutTableCell*, const PaintInfo&, const LayoutPoint&);

    LayoutTableSection& m_layoutTableSection;
};

} // namespace blink

#endif // TableSectionPainter_h
