// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FrameSetPainter_h
#define FrameSetPainter_h

namespace blink {

class IntRect;
struct PaintInfo;
class LayoutFrameSet;
class LayoutPoint;

class FrameSetPainter {
public:
    FrameSetPainter(LayoutFrameSet& layoutFrameSet) : m_layoutFrameSet(layoutFrameSet) { }

    void paint(const PaintInfo&, const LayoutPoint& paintOffset);

private:
    void paintBorders(const PaintInfo&, const LayoutPoint& adjustedPaintOffset);
    void paintChildren(const PaintInfo&, const LayoutPoint& adjustedPaintOffset);
    void paintRowBorder(const PaintInfo&, const IntRect&);
    void paintColumnBorder(const PaintInfo&, const IntRect&);

    LayoutFrameSet& m_layoutFrameSet;
};

} // namespace blink

#endif // FrameSetPainter_h
