// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef TablePainter_h
#define TablePainter_h

namespace blink {

class LayoutPoint;
class LayoutTable;
struct PaintInfo;

class TablePainter {
public:
    TablePainter(LayoutTable& layoutTable) : m_layoutTable(layoutTable) { }

    void paintObject(const PaintInfo&, const LayoutPoint&);
    void paintBoxDecorationBackground(const PaintInfo&, const LayoutPoint&);
    void paintMask(const PaintInfo&, const LayoutPoint&);

private:
    LayoutTable& m_layoutTable;
};

} // namespace blink

#endif // TablePainter_h
