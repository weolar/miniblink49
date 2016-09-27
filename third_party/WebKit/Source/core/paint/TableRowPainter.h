// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TableRowPainter_h
#define TableRowPainter_h

#include "core/layout/LayoutTableRow.h"

namespace blink {

class TableRowPainter {
public:
    TableRowPainter(LayoutTableRow& layoutTableRow) : m_layoutTableRow(layoutTableRow) { }

    void paint(const PaintInfo&, const LayoutPoint&);
    void paintOutlineForRowIfNeeded(const PaintInfo&, const LayoutPoint&);

private:
    LayoutTableRow& m_layoutTableRow;
};

} // namespace blink

#endif // TableRowPainter_h
