// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MultiColumnSetPainter_h
#define MultiColumnSetPainter_h

namespace blink {

struct PaintInfo;
class LayoutPoint;
class LayoutMultiColumnSet;

class MultiColumnSetPainter {
public:
    MultiColumnSetPainter(LayoutMultiColumnSet& layoutMultiColumnSet) : m_layoutMultiColumnSet(layoutMultiColumnSet) { }
    void paintObject(const PaintInfo&, const LayoutPoint& paintOffset);

private:
    void paintColumnRules(const PaintInfo&, const LayoutPoint& paintOffset);

    LayoutMultiColumnSet& m_layoutMultiColumnSet;
};

} // namespace blink

#endif // MultiColumnSetPainter_h
