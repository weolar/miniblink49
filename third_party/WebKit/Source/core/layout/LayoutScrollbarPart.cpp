/*
 * Copyright (C) 2008 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/layout/LayoutScrollbarPart.h"

#include "core/frame/UseCounter.h"
#include "core/layout/LayoutScrollbar.h"
#include "core/layout/LayoutScrollbarTheme.h"
#include "core/layout/LayoutView.h"
#include "platform/LengthFunctions.h"

namespace blink {

LayoutScrollbarPart::LayoutScrollbarPart(LayoutScrollbar* scrollbar, ScrollbarPart part)
    : LayoutBlock(nullptr)
    , m_scrollbar(scrollbar)
    , m_part(part)
{
}

LayoutScrollbarPart::~LayoutScrollbarPart()
{
}

static void recordScrollbarPartStats(Document& document, ScrollbarPart part)
{
    switch (part) {
    case BackButtonStartPart:
    case ForwardButtonStartPart:
    case BackButtonEndPart:
    case ForwardButtonEndPart:
        UseCounter::count(document, UseCounter::CSSSelectorPseudoScrollbarButton);
        break;
    case BackTrackPart:
    case ForwardTrackPart:
        UseCounter::count(document, UseCounter::CSSSelectorPseudoScrollbarTrackPiece);
        break;
    case ThumbPart:
        UseCounter::count(document, UseCounter::CSSSelectorPseudoScrollbarThumb);
        break;
    case TrackBGPart:
        UseCounter::count(document, UseCounter::CSSSelectorPseudoScrollbarTrack);
        break;
    case ScrollbarBGPart:
        UseCounter::count(document, UseCounter::CSSSelectorPseudoScrollbar);
        break;
    case NoPart:
    case AllParts:
        break;
    }
}

LayoutScrollbarPart* LayoutScrollbarPart::createAnonymous(Document* document, LayoutScrollbar* scrollbar, ScrollbarPart part)
{
    LayoutScrollbarPart* layoutObject = new LayoutScrollbarPart(scrollbar, part);
    recordScrollbarPartStats(*document, part);
    layoutObject->setDocumentForAnonymous(document);
    return layoutObject;
}

void LayoutScrollbarPart::layout()
{
    setLocation(LayoutPoint()); // We don't worry about positioning ourselves. We're just determining our minimum width/height.
    if (m_scrollbar->orientation() == HorizontalScrollbar)
        layoutHorizontalPart();
    else
        layoutVerticalPart();

    clearNeedsLayout();
}

void LayoutScrollbarPart::layoutHorizontalPart()
{
    if (m_part == ScrollbarBGPart) {
        setWidth(m_scrollbar->width());
        computeScrollbarHeight();
    } else {
        computeScrollbarWidth();
        setHeight(m_scrollbar->height());
    }
}

void LayoutScrollbarPart::layoutVerticalPart()
{
    if (m_part == ScrollbarBGPart) {
        computeScrollbarWidth();
        setHeight(m_scrollbar->height());
    } else {
        setWidth(m_scrollbar->width());
        computeScrollbarHeight();
    }
}

static int calcScrollbarThicknessUsing(SizeType sizeType, const Length& length, int containingLength)
{
    if (!length.isIntrinsicOrAuto() || (sizeType == MinSize && length.isAuto()))
        return minimumValueForLength(length, containingLength);
    return ScrollbarTheme::theme()->scrollbarThickness();
}

void LayoutScrollbarPart::computeScrollbarWidth()
{
    if (!m_scrollbar->owningLayoutObject())
        return;
    // FIXME: We are querying layout information but nothing guarantees that it's up-to-date, especially since we are called at style change.
    // FIXME: Querying the style's border information doesn't work on table cells with collapsing borders.
    int visibleSize = m_scrollbar->owningLayoutObject()->size().width() - m_scrollbar->owningLayoutObject()->style()->borderLeftWidth() - m_scrollbar->owningLayoutObject()->style()->borderRightWidth();
    int w = calcScrollbarThicknessUsing(MainOrPreferredSize, style()->width(), visibleSize);
    int minWidth = calcScrollbarThicknessUsing(MinSize, style()->minWidth(), visibleSize);
    int maxWidth = style()->maxWidth().isMaxSizeNone() ? w : calcScrollbarThicknessUsing(MaxSize, style()->maxWidth(), visibleSize);
    setWidth(std::max(minWidth, std::min(maxWidth, w)));

    // Buttons and track pieces can all have margins along the axis of the scrollbar.
    setMarginLeft(minimumValueForLength(style()->marginLeft(), visibleSize));
    setMarginRight(minimumValueForLength(style()->marginRight(), visibleSize));
}

void LayoutScrollbarPart::computeScrollbarHeight()
{
    if (!m_scrollbar->owningLayoutObject())
        return;
    // FIXME: We are querying layout information but nothing guarantees that it's up-to-date, especially since we are called at style change.
    // FIXME: Querying the style's border information doesn't work on table cells with collapsing borders.
    int visibleSize = m_scrollbar->owningLayoutObject()->size().height() -  m_scrollbar->owningLayoutObject()->style()->borderTopWidth() - m_scrollbar->owningLayoutObject()->style()->borderBottomWidth();
    int h = calcScrollbarThicknessUsing(MainOrPreferredSize, style()->height(), visibleSize);
    int minHeight = calcScrollbarThicknessUsing(MinSize, style()->minHeight(), visibleSize);
    int maxHeight = style()->maxHeight().isMaxSizeNone() ? h : calcScrollbarThicknessUsing(MaxSize, style()->maxHeight(), visibleSize);
    setHeight(std::max(minHeight, std::min(maxHeight, h)));

    // Buttons and track pieces can all have margins along the axis of the scrollbar.
    setMarginTop(minimumValueForLength(style()->marginTop(), visibleSize));
    setMarginBottom(minimumValueForLength(style()->marginBottom(), visibleSize));
}

void LayoutScrollbarPart::computePreferredLogicalWidths()
{
    if (!preferredLogicalWidthsDirty())
        return;

    m_minPreferredLogicalWidth = m_maxPreferredLogicalWidth = 0;

    clearPreferredLogicalWidthsDirty();
}

void LayoutScrollbarPart::styleWillChange(StyleDifference diff, const ComputedStyle& newStyle)
{
    LayoutBlock::styleWillChange(diff, newStyle);
    setInline(false);
}

void LayoutScrollbarPart::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    LayoutBlock::styleDidChange(diff, oldStyle);
    setInline(false);
    clearPositionedState();
    setFloating(false);
    setHasOverflowClip(false);
    if (oldStyle && m_scrollbar && m_part != NoPart && (diff.needsPaintInvalidation() || diff.needsLayout()))
        m_scrollbar->theme()->invalidatePart(m_scrollbar, m_part);
}

void LayoutScrollbarPart::imageChanged(WrappedImagePtr image, const IntRect* rect)
{
    if (m_scrollbar && m_part != NoPart) {
        m_scrollbar->theme()->invalidatePart(m_scrollbar, m_part);
    } else {
        if (FrameView* frameView = view()->frameView()) {
            if (frameView->isFrameViewScrollCorner(this)) {
                frameView->invalidateScrollCorner(frameView->scrollCornerRect());
                return;
            }
        }

        LayoutBlock::imageChanged(image, rect);
    }
}

LayoutObject* LayoutScrollbarPart::layoutObjectOwningScrollbar() const
{
    return (!m_scrollbar) ? nullptr : m_scrollbar->owningLayoutObject();
}

}
