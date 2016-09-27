// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LineBoxListPainter_h
#define LineBoxListPainter_h

#include "core/style/ComputedStyleConstants.h"

namespace blink {

class LayoutPoint;
struct PaintInfo;
class LayoutBoxModelObject;
class LineBoxList;

class LineBoxListPainter {
public:
    LineBoxListPainter(LineBoxList& lineBoxList) : m_lineBoxList(lineBoxList) { }

    void paint(LayoutBoxModelObject*, const PaintInfo&, const LayoutPoint&) const;

private:
    LineBoxList& m_lineBoxList;
};

} // namespace blink

#endif // LineBoxListPainter_h
