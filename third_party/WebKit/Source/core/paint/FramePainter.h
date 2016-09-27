// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FramePainter_h
#define FramePainter_h

namespace blink {

class FrameView;
class GraphicsContext;
class IntRect;
class Scrollbar;

class FramePainter {
public:
    FramePainter(FrameView& frameView) : m_frameView(frameView) { }

    void paint(GraphicsContext*, const IntRect&);
    void paintScrollbars(GraphicsContext*, const IntRect&);
    void paintContents(GraphicsContext*, const IntRect& damageRect);
    void paintScrollCorner(GraphicsContext*, const IntRect& cornerRect);

private:
    void paintScrollbar(GraphicsContext*, Scrollbar*, const IntRect&);

    FrameView& m_frameView;
    static bool s_inPaintContents;
};

} // namespace blink

#endif // FramePainter_h
