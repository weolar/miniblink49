// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FieldsetPainter_h
#define FieldsetPainter_h

namespace blink {

struct PaintInfo;
class LayoutPoint;
class LayoutFieldset;

class FieldsetPainter {
public:
    FieldsetPainter(LayoutFieldset& layoutFieldset) : m_layoutFieldset(layoutFieldset) { }

    void paintBoxDecorationBackground(const PaintInfo&, const LayoutPoint&);
    void paintMask(const PaintInfo&, const LayoutPoint&);

private:
    LayoutFieldset& m_layoutFieldset;
};

} // namespace blink

#endif // FieldsetPainter_h
