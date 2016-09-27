/*
 * Copyright (C) 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SVGInlineTextBox_h
#define SVGInlineTextBox_h

#include "core/layout/line/InlineTextBox.h"
#include "core/layout/svg/SVGTextLayoutEngine.h"

namespace blink {

class SVGInlineTextBox final : public InlineTextBox {
public:
    SVGInlineTextBox(LayoutObject&, int start, unsigned short length);

    virtual bool isSVGInlineTextBox() const override { return true; }

    virtual LayoutUnit virtualLogicalHeight() const override { return m_logicalHeight; }
    void setLogicalHeight(LayoutUnit height) { m_logicalHeight = height; }

    virtual int offsetForPosition(LayoutUnit x, bool includePartialGlyphs = true) const override;
    virtual LayoutUnit positionForOffset(int offset) const override;

    virtual void paint(const PaintInfo&, const LayoutPoint&, LayoutUnit lineTop, LayoutUnit lineBottom) override;
    virtual LayoutRect localSelectionRect(int startPosition, int endPosition) override;

    bool mapStartEndPositionsIntoFragmentCoordinates(const SVGTextFragment&, int& startPosition, int& endPosition) const;

    virtual LayoutRect calculateBoundaries() const override;

    void clearTextFragments() { m_textFragments.clear(); }
    Vector<SVGTextFragment>& textFragments() { return m_textFragments; }
    const Vector<SVGTextFragment>& textFragments() const { return m_textFragments; }

    virtual void dirtyLineBoxes() override;

    bool startsNewTextChunk() const { return m_startsNewTextChunk; }
    void setStartsNewTextChunk(bool newTextChunk) { m_startsNewTextChunk = newTextChunk; }

    int offsetForPositionInFragment(const SVGTextFragment&, LayoutUnit position, bool includePartialGlyphs) const;
    FloatRect selectionRectForTextFragment(const SVGTextFragment&, int fragmentStartPosition, int fragmentEndPosition, const ComputedStyle&);
    TextRun constructTextRun(const ComputedStyle&, const SVGTextFragment&) const;

private:
    virtual void paintDocumentMarker(GraphicsContext*, const LayoutPoint&, DocumentMarker*, const ComputedStyle&, const Font&, bool) override final;
    virtual void paintTextMatchMarker(GraphicsContext*, const LayoutPoint&, DocumentMarker*, const ComputedStyle&, const Font&) override final;

    virtual bool nodeAtPoint(HitTestResult&, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, LayoutUnit lineTop, LayoutUnit lineBottom) override;

    LayoutUnit m_logicalHeight;
    bool m_startsNewTextChunk : 1;
    Vector<SVGTextFragment> m_textFragments;
};

DEFINE_INLINE_BOX_TYPE_CASTS(SVGInlineTextBox);

} // namespace blink

#endif // SVGInlineTextBox_h
