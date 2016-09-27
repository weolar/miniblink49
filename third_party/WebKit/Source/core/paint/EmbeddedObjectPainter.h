// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EmbeddedObjectPainter_h
#define EmbeddedObjectPainter_h

namespace blink {

struct PaintInfo;
class FloatRect;
class Font;
class LayoutEmbeddedObject;
class LayoutPoint;
class Path;
class TextRun;

class EmbeddedObjectPainter {
public:
    EmbeddedObjectPainter(LayoutEmbeddedObject& layoutEmbeddedObject) : m_layoutEmbeddedObject(layoutEmbeddedObject) { }

    void paintReplaced(const PaintInfo&, const LayoutPoint& paintOffset);

private:
    LayoutEmbeddedObject& m_layoutEmbeddedObject;
};

} // namespace blink

#endif // EmbeddedObjectPainter_h
