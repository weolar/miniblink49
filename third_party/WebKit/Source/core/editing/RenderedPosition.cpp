/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/editing/RenderedPosition.h"

#include "core/dom/Position.h"
#include "core/editing/VisiblePosition.h"
#include "core/layout/compositing/CompositedSelectionBound.h"
#include "core/paint/DeprecatedPaintLayer.h"

namespace blink {

static inline LayoutObject* layoutObjectFromPosition(const Position& position)
{
    ASSERT(position.isNotNull());
    Node* layoutObjectNode = nullptr;
    switch (position.anchorType()) {
    case PositionAnchorType::OffsetInAnchor:
        layoutObjectNode = position.computeNodeAfterPosition();
        if (!layoutObjectNode || !layoutObjectNode->layoutObject())
            layoutObjectNode = position.anchorNode()->lastChild();
        break;

    case PositionAnchorType::BeforeAnchor:
    case PositionAnchorType::AfterAnchor:
        break;

    case PositionAnchorType::BeforeChildren:
        layoutObjectNode = position.anchorNode()->firstChild();
        break;
    case PositionAnchorType::AfterChildren:
        layoutObjectNode = position.anchorNode()->lastChild();
        break;
    }
    if (!layoutObjectNode || !layoutObjectNode->layoutObject())
        layoutObjectNode = position.anchorNode();
    return layoutObjectNode->layoutObject();
}

RenderedPosition::RenderedPosition(const VisiblePosition& position)
    : m_layoutObject(nullptr)
    , m_inlineBox(nullptr)
    , m_offset(0)
    , m_prevLeafChild(uncachedInlineBox())
    , m_nextLeafChild(uncachedInlineBox())
{
    if (position.isNull())
        return;
    InlineBoxPosition boxPosition = position.computeInlineBoxPosition();
    m_inlineBox = boxPosition.inlineBox;
    m_offset = boxPosition.offsetInBox;
    if (m_inlineBox)
        m_layoutObject = &m_inlineBox->layoutObject();
    else
        m_layoutObject = layoutObjectFromPosition(position.deepEquivalent());
}

RenderedPosition::RenderedPosition(const Position& position, EAffinity affinity)
    : m_layoutObject(nullptr)
    , m_inlineBox(nullptr)
    , m_offset(0)
    , m_prevLeafChild(uncachedInlineBox())
    , m_nextLeafChild(uncachedInlineBox())
{
    if (position.isNull())
        return;
    InlineBoxPosition boxPosition = position.computeInlineBoxPosition(affinity);
    m_inlineBox = boxPosition.inlineBox;
    m_offset = boxPosition.offsetInBox;
    if (m_inlineBox)
        m_layoutObject = &m_inlineBox->layoutObject();
    else
        m_layoutObject = layoutObjectFromPosition(position);
}

RenderedPosition::RenderedPosition(const PositionInComposedTree& position, EAffinity affinity)
    : RenderedPosition(toPositionInDOMTree(position), affinity)
{
}

InlineBox* RenderedPosition::prevLeafChild() const
{
    if (m_prevLeafChild == uncachedInlineBox())
        m_prevLeafChild = m_inlineBox->prevLeafChildIgnoringLineBreak();
    return m_prevLeafChild;
}

InlineBox* RenderedPosition::nextLeafChild() const
{
    if (m_nextLeafChild == uncachedInlineBox())
        m_nextLeafChild = m_inlineBox->nextLeafChildIgnoringLineBreak();
    return m_nextLeafChild;
}

bool RenderedPosition::isEquivalent(const RenderedPosition& other) const
{
    return (m_layoutObject == other.m_layoutObject && m_inlineBox == other.m_inlineBox && m_offset == other.m_offset)
        || (atLeftmostOffsetInBox() && other.atRightmostOffsetInBox() && prevLeafChild() == other.m_inlineBox)
        || (atRightmostOffsetInBox() && other.atLeftmostOffsetInBox() && nextLeafChild() == other.m_inlineBox);
}

unsigned char RenderedPosition::bidiLevelOnLeft() const
{
    InlineBox* box = atLeftmostOffsetInBox() ? prevLeafChild() : m_inlineBox;
    return box ? box->bidiLevel() : 0;
}

unsigned char RenderedPosition::bidiLevelOnRight() const
{
    InlineBox* box = atRightmostOffsetInBox() ? nextLeafChild() : m_inlineBox;
    return box ? box->bidiLevel() : 0;
}

RenderedPosition RenderedPosition::leftBoundaryOfBidiRun(unsigned char bidiLevelOfRun)
{
    if (!m_inlineBox || bidiLevelOfRun > m_inlineBox->bidiLevel())
        return RenderedPosition();

    InlineBox* box = m_inlineBox;
    do {
        InlineBox* prev = box->prevLeafChildIgnoringLineBreak();
        if (!prev || prev->bidiLevel() < bidiLevelOfRun)
            return RenderedPosition(&box->layoutObject(), box, box->caretLeftmostOffset());
        box = prev;
    } while (box);

    ASSERT_NOT_REACHED();
    return RenderedPosition();
}

RenderedPosition RenderedPosition::rightBoundaryOfBidiRun(unsigned char bidiLevelOfRun)
{
    if (!m_inlineBox || bidiLevelOfRun > m_inlineBox->bidiLevel())
        return RenderedPosition();

    InlineBox* box = m_inlineBox;
    do {
        InlineBox* next = box->nextLeafChildIgnoringLineBreak();
        if (!next || next->bidiLevel() < bidiLevelOfRun)
            return RenderedPosition(&box->layoutObject(), box, box->caretRightmostOffset());
        box = next;
    } while (box);

    ASSERT_NOT_REACHED();
    return RenderedPosition();
}

bool RenderedPosition::atLeftBoundaryOfBidiRun(ShouldMatchBidiLevel shouldMatchBidiLevel, unsigned char bidiLevelOfRun) const
{
    if (!m_inlineBox)
        return false;

    if (atLeftmostOffsetInBox()) {
        if (shouldMatchBidiLevel == IgnoreBidiLevel)
            return !prevLeafChild() || prevLeafChild()->bidiLevel() < m_inlineBox->bidiLevel();
        return m_inlineBox->bidiLevel() >= bidiLevelOfRun && (!prevLeafChild() || prevLeafChild()->bidiLevel() < bidiLevelOfRun);
    }

    if (atRightmostOffsetInBox()) {
        if (shouldMatchBidiLevel == IgnoreBidiLevel)
            return nextLeafChild() && m_inlineBox->bidiLevel() < nextLeafChild()->bidiLevel();
        return nextLeafChild() && m_inlineBox->bidiLevel() < bidiLevelOfRun && nextLeafChild()->bidiLevel() >= bidiLevelOfRun;
    }

    return false;
}

bool RenderedPosition::atRightBoundaryOfBidiRun(ShouldMatchBidiLevel shouldMatchBidiLevel, unsigned char bidiLevelOfRun) const
{
    if (!m_inlineBox)
        return false;

    if (atRightmostOffsetInBox()) {
        if (shouldMatchBidiLevel == IgnoreBidiLevel)
            return !nextLeafChild() || nextLeafChild()->bidiLevel() < m_inlineBox->bidiLevel();
        return m_inlineBox->bidiLevel() >= bidiLevelOfRun && (!nextLeafChild() || nextLeafChild()->bidiLevel() < bidiLevelOfRun);
    }

    if (atLeftmostOffsetInBox()) {
        if (shouldMatchBidiLevel == IgnoreBidiLevel)
            return prevLeafChild() && m_inlineBox->bidiLevel() < prevLeafChild()->bidiLevel();
        return prevLeafChild() && m_inlineBox->bidiLevel() < bidiLevelOfRun && prevLeafChild()->bidiLevel() >= bidiLevelOfRun;
    }

    return false;
}

Position RenderedPosition::positionAtLeftBoundaryOfBiDiRun() const
{
    ASSERT(atLeftBoundaryOfBidiRun());

    if (atLeftmostOffsetInBox())
        return createLegacyEditingPosition(m_layoutObject->node(), m_offset);

    return createLegacyEditingPosition(nextLeafChild()->layoutObject().node(), nextLeafChild()->caretLeftmostOffset());
}

Position RenderedPosition::positionAtRightBoundaryOfBiDiRun() const
{
    ASSERT(atRightBoundaryOfBidiRun());

    if (atRightmostOffsetInBox())
        return createLegacyEditingPosition(m_layoutObject->node(), m_offset);

    return createLegacyEditingPosition(prevLeafChild()->layoutObject().node(), prevLeafChild()->caretRightmostOffset());
}

IntRect RenderedPosition::absoluteRect(LayoutUnit* extraWidthToEndOfLine) const
{
    if (isNull())
        return IntRect();

    IntRect localRect = pixelSnappedIntRect(m_layoutObject->localCaretRect(m_inlineBox, m_offset, extraWidthToEndOfLine));
    return localRect == IntRect() ? IntRect() : m_layoutObject->localToAbsoluteQuad(FloatRect(localRect)).enclosingBoundingBox();
}

void RenderedPosition::positionInGraphicsLayerBacking(CompositedSelectionBound& bound) const
{
    bound.layer = nullptr;
    bound.edgeTopInLayer = bound.edgeBottomInLayer = FloatPoint();

    if (isNull())
        return;

    LayoutRect rect = m_layoutObject->localCaretRect(m_inlineBox, m_offset);
    DeprecatedPaintLayer* layer = nullptr;
    bound.edgeTopInLayer = m_layoutObject->localToInvalidationBackingPoint(rect.minXMinYCorner(), &layer);
    bound.edgeBottomInLayer = m_layoutObject->localToInvalidationBackingPoint(rect.minXMaxYCorner(), nullptr);
    bound.layer = layer ? layer->graphicsLayerBacking() : nullptr;
}

bool layoutObjectContainsPosition(LayoutObject* target, const Position& position)
{
    for (LayoutObject* layoutObject = layoutObjectFromPosition(position); layoutObject && layoutObject->node(); layoutObject = layoutObject->parent()) {
        if (layoutObject == target)
            return true;
    }
    return false;
}

};
