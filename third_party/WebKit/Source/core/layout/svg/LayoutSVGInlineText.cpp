/*
 * Copyright (C) 2006 Oliver Hunt <ojh16@student.canterbury.ac.nz>
 * Copyright (C) 2006 Apple Computer Inc.
 * Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2008 Rob Buis <buis@kde.org>
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

#include "config.h"

#include "core/layout/svg/LayoutSVGInlineText.h"

#include "core/css/CSSFontSelector.h"
#include "core/css/FontSize.h"
#include "core/dom/StyleEngine.h"
#include "core/editing/VisiblePosition.h"
#include "core/layout/svg/LayoutSVGText.h"
#include "core/layout/svg/SVGLayoutSupport.h"
#include "core/layout/svg/line/SVGInlineTextBox.h"

namespace blink {

static PassRefPtr<StringImpl> applySVGWhitespaceRules(PassRefPtr<StringImpl> string, bool preserveWhiteSpace)
{
    if (preserveWhiteSpace) {
        // Spec: When xml:space="preserve", the SVG user agent will do the following using a
        // copy of the original character data content. It will convert all newline and tab
        // characters into space characters. Then, it will draw all space characters, including
        // leading, trailing and multiple contiguous space characters.
        RefPtr<StringImpl> newString = string->replace('\t', ' ');
        newString = newString->replace('\n', ' ');
        newString = newString->replace('\r', ' ');
        return newString.release();
    }

    // Spec: When xml:space="default", the SVG user agent will do the following using a
    // copy of the original character data content. First, it will remove all newline
    // characters. Then it will convert all tab characters into space characters.
    // Then, it will strip off all leading and trailing space characters.
    // Then, all contiguous space characters will be consolidated.
    RefPtr<StringImpl> newString = string->replace('\n', StringImpl::empty());
    newString = newString->replace('\r', StringImpl::empty());
    newString = newString->replace('\t', ' ');
    return newString.release();
}

static float squaredDistanceToClosestPoint(const FloatRect& rect, const FloatPoint& point)
{
    FloatPoint closestPoint;
    closestPoint.setX(std::max(std::min(point.x(), rect.maxX()), rect.x()));
    closestPoint.setY(std::max(std::min(point.y(), rect.maxY()), rect.y()));
    return (point - closestPoint).diagonalLengthSquared();
}

LayoutSVGInlineText::LayoutSVGInlineText(Node* n, PassRefPtr<StringImpl> string)
    : LayoutText(n, applySVGWhitespaceRules(string, false))
    , m_scalingFactor(1)
    , m_layoutAttributes(this)
{
}

void LayoutSVGInlineText::setTextInternal(PassRefPtr<StringImpl> text)
{
    LayoutText::setTextInternal(text);
    if (LayoutSVGText* textLayoutObject = LayoutSVGText::locateLayoutSVGTextAncestor(this))
        textLayoutObject->subtreeTextDidChange(this);
}

void LayoutSVGInlineText::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    LayoutText::styleDidChange(diff, oldStyle);
    updateScaledFont();

    bool newPreserves = style() ? style()->whiteSpace() == PRE : false;
    bool oldPreserves = oldStyle ? oldStyle->whiteSpace() == PRE : false;
    if (oldPreserves != newPreserves) {
        setText(originalText(), true);
        return;
    }

    if (!diff.needsFullLayout())
        return;

    // The text metrics may be influenced by style changes.
    if (LayoutSVGText* textLayoutObject = LayoutSVGText::locateLayoutSVGTextAncestor(this))
        textLayoutObject->setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::StyleChange);
}

InlineTextBox* LayoutSVGInlineText::createTextBox(int start, unsigned short length)
{
    InlineTextBox* box = new SVGInlineTextBox(*this, start, length);
    box->setHasVirtualLogicalHeight();
    return box;
}

LayoutRect LayoutSVGInlineText::localCaretRect(InlineBox* box, int caretOffset, LayoutUnit*)
{
    if (!box || !box->isInlineTextBox())
        return LayoutRect();

    InlineTextBox* textBox = toInlineTextBox(box);
    if (static_cast<unsigned>(caretOffset) < textBox->start() || static_cast<unsigned>(caretOffset) > textBox->start() + textBox->len())
        return LayoutRect();

    // Use the edge of the selection rect to determine the caret rect.
    if (static_cast<unsigned>(caretOffset) < textBox->start() + textBox->len()) {
        LayoutRect rect = textBox->localSelectionRect(caretOffset, caretOffset + 1);
        LayoutUnit x = box->isLeftToRightDirection() ? rect.x() : rect.maxX();
        return LayoutRect(x, rect.y(), caretWidth(), rect.height());
    }

    LayoutRect rect = textBox->localSelectionRect(caretOffset - 1, caretOffset);
    LayoutUnit x = box->isLeftToRightDirection() ? rect.maxX() : rect.x();
    return LayoutRect(x, rect.y(), caretWidth(), rect.height());
}

FloatRect LayoutSVGInlineText::floatLinesBoundingBox() const
{
    FloatRect boundingBox;
    for (InlineTextBox* box = firstTextBox(); box; box = box->nextTextBox())
        boundingBox.unite(FloatRect(box->calculateBoundaries()));
    return boundingBox;
}

IntRect LayoutSVGInlineText::linesBoundingBox() const
{
    return enclosingIntRect(floatLinesBoundingBox());
}

bool LayoutSVGInlineText::characterStartsNewTextChunk(int position) const
{
    ASSERT(position >= 0);
    ASSERT(position < static_cast<int>(textLength()));

    // Each <textPath> element starts a new text chunk, regardless of any x/y values.
    if (!position && parent()->isSVGTextPath() && !previousSibling())
        return true;

    const SVGCharacterDataMap::const_iterator it = m_layoutAttributes.characterDataMap().find(static_cast<unsigned>(position + 1));
    if (it == m_layoutAttributes.characterDataMap().end())
        return false;

    return !SVGTextLayoutAttributes::isEmptyValue(it->value.x) || !SVGTextLayoutAttributes::isEmptyValue(it->value.y);
}

PositionWithAffinity LayoutSVGInlineText::positionForPoint(const LayoutPoint& point)
{
    if (!firstTextBox() || !textLength())
        return createPositionWithAffinity(0, DOWNSTREAM);

    ASSERT(m_scalingFactor);
    float baseline = m_scaledFont.fontMetrics().floatAscent() / m_scalingFactor;

    LayoutBlock* containingBlock = this->containingBlock();
    ASSERT(containingBlock);

    // Map local point to absolute point, as the character origins stored in the text fragments use absolute coordinates.
    FloatPoint absolutePoint(point);
    absolutePoint.moveBy(containingBlock->location());

    float closestDistance = std::numeric_limits<float>::max();
    float closestDistancePosition = 0;
    const SVGTextFragment* closestDistanceFragment = nullptr;
    SVGInlineTextBox* closestDistanceBox = nullptr;

    AffineTransform fragmentTransform;
    for (InlineTextBox* box = firstTextBox(); box; box = box->nextTextBox()) {
        if (!box->isSVGInlineTextBox())
            continue;

        SVGInlineTextBox* textBox = toSVGInlineTextBox(box);
        Vector<SVGTextFragment>& fragments = textBox->textFragments();

        unsigned textFragmentsSize = fragments.size();
        for (unsigned i = 0; i < textFragmentsSize; ++i) {
            const SVGTextFragment& fragment = fragments.at(i);
            FloatRect fragmentRect(fragment.x, fragment.y - baseline, fragment.width, fragment.height);
            fragment.buildFragmentTransform(fragmentTransform);
            if (!fragmentTransform.isIdentity())
                fragmentRect = fragmentTransform.mapRect(fragmentRect);

            float distance = 0;
            if (!fragmentRect.contains(absolutePoint))
                distance = squaredDistanceToClosestPoint(fragmentRect, absolutePoint);

            if (distance <= closestDistance) {
                closestDistance = distance;
                closestDistanceBox = textBox;
                closestDistanceFragment = &fragment;
                closestDistancePosition = fragmentRect.x();
            }
        }
    }

    if (!closestDistanceFragment)
        return createPositionWithAffinity(0, DOWNSTREAM);

    int offset = closestDistanceBox->offsetForPositionInFragment(*closestDistanceFragment, absolutePoint.x() - closestDistancePosition, true);
    return createPositionWithAffinity(offset + closestDistanceBox->start(), offset > 0 ? VP_UPSTREAM_IF_POSSIBLE : DOWNSTREAM);
}

void LayoutSVGInlineText::updateScaledFont()
{
    computeNewScaledFontForStyle(this, style(), m_scalingFactor, m_scaledFont);
}

void LayoutSVGInlineText::computeNewScaledFontForStyle(LayoutObject* layoutObject, const ComputedStyle* style, float& scalingFactor, Font& scaledFont)
{
    ASSERT(style);
    ASSERT(layoutObject);

    // Alter font-size to the right on-screen value to avoid scaling the glyphs themselves, except when GeometricPrecision is specified.
    scalingFactor = SVGLayoutSupport::calculateScreenFontSizeScalingFactor(layoutObject);
    if (style->effectiveZoom() == 1 && (scalingFactor == 1 || !scalingFactor)) {
        scalingFactor = 1;
        scaledFont = style->font();
        return;
    }

    if (style->fontDescription().textRendering() == GeometricPrecision)
        scalingFactor = 1;

    FontDescription fontDescription(style->fontDescription());

    Document& document = layoutObject->document();
    // FIXME: We need to better handle the case when we compute very small fonts below (below 1pt).
    fontDescription.setComputedSize(FontSize::getComputedSizeFromSpecifiedSize(&document, scalingFactor, fontDescription.isAbsoluteSize(), fontDescription.specifiedSize(), DoNotUseSmartMinimumForFontSize));

    scaledFont = Font(fontDescription);
    scaledFont.update(document.styleEngine().fontSelector());
}

LayoutRect LayoutSVGInlineText::clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* paintInvalidationState) const
{
    // FIXME: The following works because LayoutSVGBlock has forced slow rect mapping of the paintInvalidationState.
    // Should let this really work with paintInvalidationState's fast mapping and remove the assert.
    ASSERT(!paintInvalidationState || !paintInvalidationState->canMapToContainer(paintInvalidationContainer));
    return parent()->clippedOverflowRectForPaintInvalidation(paintInvalidationContainer, paintInvalidationState);
}

PassRefPtr<StringImpl> LayoutSVGInlineText::originalText() const
{
    RefPtr<StringImpl> result = LayoutText::originalText();
    if (!result)
        return nullptr;
    return applySVGWhitespaceRules(result, style() && style()->whiteSpace() == PRE);
}

}
