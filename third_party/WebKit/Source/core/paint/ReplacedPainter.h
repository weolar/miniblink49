// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ReplacedPainter_h
#define ReplacedPainter_h

namespace blink {

struct PaintInfo;
class LayoutPoint;
class LayoutReplaced;

class ReplacedPainter {
public:
    ReplacedPainter(LayoutReplaced& layoutReplaced) : m_layoutReplaced(layoutReplaced) { }

    void paint(const PaintInfo&, const LayoutPoint&);

private:
    LayoutReplaced& m_layoutReplaced;
};

} // namespace blink

#endif // ReplacedPainter_h
