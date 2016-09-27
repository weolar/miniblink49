// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BlockPainter_h
#define BlockPainter_h

namespace blink {

struct PaintInfo;
class InlineBox;
class LayoutBlock;
class LayoutBox;
class LayoutFlexibleBox;
class LayoutObject;
class LayoutPoint;
class LayoutRect;

class BlockPainter {
public:
    BlockPainter(LayoutBlock& block) : m_layoutBlock(block) { }

    void paint(const PaintInfo&, const LayoutPoint& paintOffset);
    void paintObject(const PaintInfo&, const LayoutPoint&);
    void paintChildren(const PaintInfo&, const LayoutPoint&);
    void paintChild(LayoutBox&, const PaintInfo&, const LayoutPoint&);
    void paintChildAsInlineBlock(LayoutBox&, const PaintInfo&, const LayoutPoint&);
    void paintOverflowControlsIfNeeded(const PaintInfo&, const LayoutPoint&);

    // inline-block elements paint all phases atomically. This function ensures that. Certain other elements
    // (grid items, flex items) require this behavior as well, and this function exists as a helper for them.
    // It is expected that the caller will call this function independent of the value of paintInfo.phase.
    static void paintAsInlineBlock(LayoutObject&, const PaintInfo&, const LayoutPoint&);
    static void paintChildrenOfFlexibleBox(LayoutFlexibleBox&, const PaintInfo&, const LayoutPoint& paintOffset);
    static void paintInlineBox(InlineBox&, const PaintInfo&, const LayoutPoint& paintOffset);

private:
    LayoutRect overflowRectForPaintRejection() const;
    bool hasCaret() const;
    void paintCarets(const PaintInfo&, const LayoutPoint&);
    void paintContents(const PaintInfo&, const LayoutPoint&);
    void paintSelection(const PaintInfo&, const LayoutPoint&);
    void paintContinuationOutlines(const PaintInfo&, const LayoutPoint&);

    LayoutBlock& m_layoutBlock;
};

} // namespace blink

#endif // BlockPainter_h
