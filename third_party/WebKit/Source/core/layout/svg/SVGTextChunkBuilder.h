/*
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

#ifndef SVGTextChunkBuilder_h
#define SVGTextChunkBuilder_h

#include "wtf/Vector.h"

namespace blink {

class AffineTransform;
class SVGInlineTextBox;
struct SVGTextFragment;

// SVGTextChunkBuilder performs the third layout phase for SVG text.
//
// Phase one built the layout information from the SVG DOM stored in the LayoutSVGInlineText objects (SVGTextLayoutAttributes).
// Phase two performed the actual per-character layout, computing the final positions for each character, stored in the SVGInlineTextBox objects (SVGTextFragment).
// Phase three performs all modifications that have to be applied to each individual text chunk (text-anchor & textLength).

class SVGTextChunkBuilder {
    WTF_MAKE_NONCOPYABLE(SVGTextChunkBuilder);
public:
    SVGTextChunkBuilder();

    void processTextChunks(const Vector<SVGInlineTextBox*>&);

protected:
    typedef Vector<SVGInlineTextBox*>::const_iterator BoxListConstIterator;

    virtual void handleTextChunk(BoxListConstIterator boxStart, BoxListConstIterator boxEnd);

private:
    void processTextLengthSpacingCorrection(bool isVerticalText, float textLengthShift, Vector<SVGTextFragment>&, unsigned& atCharacter);
    void applyTextLengthScaleAdjustment(const AffineTransform&, Vector<SVGTextFragment>&);
    void processTextAnchorCorrection(bool isVerticalText, float textAnchorShift, Vector<SVGTextFragment>&);
};

class SVGTextPathChunkBuilder final : public SVGTextChunkBuilder {
    WTF_MAKE_NONCOPYABLE(SVGTextPathChunkBuilder);
public:
    SVGTextPathChunkBuilder();

    float totalLength() const { return m_totalLength; }
    unsigned totalCharacters() const { return m_totalCharacters; }
    float totalTextAnchorShift() const { return m_totalTextAnchorShift; }

private:
    void handleTextChunk(BoxListConstIterator boxStart, BoxListConstIterator boxEnd) override;

    float m_totalLength;
    unsigned m_totalCharacters;
    float m_totalTextAnchorShift;
};

} // namespace blink

#endif
