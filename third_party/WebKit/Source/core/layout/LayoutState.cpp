/*
 * Copyright (C) 2007 Apple Inc.  All rights reserved.
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
#include "core/layout/LayoutState.h"

#include "core/layout/LayoutFlowThread.h"
#include "core/layout/LayoutInline.h"
#include "core/layout/LayoutView.h"

namespace blink {

LayoutState::LayoutState(LayoutUnit pageLogicalHeight, bool pageLogicalHeightChanged, LayoutView& view)
    : m_isPaginated(pageLogicalHeight)
    , m_pageLogicalHeightChanged(pageLogicalHeightChanged)
    , m_containingBlockLogicalWidthChanged(false)
    , m_flowThread(nullptr)
    , m_next(nullptr)
    , m_pageLogicalHeight(pageLogicalHeight)
    , m_layoutObject(view)
{
    ASSERT(!view.layoutState());
    view.pushLayoutState(*this);
}

LayoutState::LayoutState(LayoutBox& layoutObject, const LayoutSize& offset, LayoutUnit pageLogicalHeight, bool pageLogicalHeightChanged, bool containingBlockLogicalWidthChanged)
    : m_containingBlockLogicalWidthChanged(containingBlockLogicalWidthChanged)
    , m_next(layoutObject.view()->layoutState())
    , m_layoutObject(layoutObject)
{
    if (layoutObject.isLayoutFlowThread())
        m_flowThread = toLayoutFlowThread(&layoutObject);
    else if (!layoutObject.isOutOfFlowPositioned() && !layoutObject.isColumnSpanAll())
        m_flowThread = m_next->flowThread();
    else
        m_flowThread = nullptr;
    layoutObject.view()->pushLayoutState(*this);
    bool fixed = layoutObject.isOutOfFlowPositioned() && layoutObject.style()->position() == FixedPosition;
    if (fixed) {
        // FIXME: This doesn't work correctly with transforms.
        FloatPoint fixedOffset = layoutObject.view()->localToAbsolute(FloatPoint(), IsFixed);
        m_layoutOffset = LayoutSize(fixedOffset.x(), fixedOffset.y()) + offset;
    } else {
        m_layoutOffset = m_next->m_layoutOffset + offset;
    }

    if (layoutObject.isOutOfFlowPositioned() && !fixed) {
        if (LayoutObject* container = layoutObject.container()) {
            if (container->style()->hasInFlowPosition() && container->isLayoutInline())
                m_layoutOffset += toLayoutInline(container)->offsetForInFlowPositionedInline(layoutObject);
        }
    }
    // If we establish a new page height, then cache the offset to the top of the first page.
    // We can compare this later on to figure out what part of the page we're actually on,
    if (pageLogicalHeight || layoutObject.isLayoutFlowThread()) {
        m_pageLogicalHeight = pageLogicalHeight;
        bool isFlipped = layoutObject.style()->isFlippedBlocksWritingMode();
        m_pageOffset = LayoutSize(m_layoutOffset.width() + (!isFlipped ? layoutObject.borderLeft() + layoutObject.paddingLeft() : layoutObject.borderRight() + layoutObject.paddingRight()),
            m_layoutOffset.height() + (!isFlipped ? layoutObject.borderTop() + layoutObject.paddingTop() : layoutObject.borderBottom() + layoutObject.paddingBottom()));
        m_pageLogicalHeightChanged = pageLogicalHeightChanged;
        m_isPaginated = true;
    } else if (m_layoutObject.isSVG() && !m_layoutObject.isSVGRoot()) {
        // Pagination inside SVG is not allowed.
        m_flowThread = nullptr;
        m_pageLogicalHeightChanged = false;
        m_isPaginated = false;
    } else {
        // If we don't establish a new page height, then propagate the old page height and offset down.
        m_pageLogicalHeight = m_next->m_pageLogicalHeight;
        m_pageLogicalHeightChanged = m_next->m_pageLogicalHeightChanged;
        m_pageOffset = m_next->m_pageOffset;

        // Disable pagination for objects we don't support. For now this includes overflow:scroll/auto, inline blocks and
        // writing mode roots.
        if (layoutObject.isUnsplittableForPagination()) {
            m_pageLogicalHeight = 0;
            m_isPaginated = false;
        } else {
            m_isPaginated = m_pageLogicalHeight || layoutObject.flowThreadContainingBlock();
        }
    }

    // FIXME: <http://bugs.webkit.org/show_bug.cgi?id=13443> Apply control clip if present.
}

LayoutState::LayoutState(LayoutObject& root)
    : m_isPaginated(false)
    , m_pageLogicalHeightChanged(false)
    , m_containingBlockLogicalWidthChanged(false)
    , m_flowThread(nullptr)
    , m_next(root.view()->layoutState())
    , m_pageLogicalHeight(0)
    , m_layoutObject(root)
{
    ASSERT(!m_next);
    // We'll end up pushing in LayoutView itself, so don't bother adding it.
    if (root.isLayoutView())
        return;

    root.view()->pushLayoutState(*this);

    LayoutObject* container = root.container();
    FloatPoint absContentPoint = container->localToAbsolute(FloatPoint(), UseTransforms);
    m_layoutOffset = LayoutSize(absContentPoint.x(), absContentPoint.y());
}

LayoutState::~LayoutState()
{
    if (m_layoutObject.view()->layoutState()) {
        ASSERT(m_layoutObject.view()->layoutState() == this);
        m_layoutObject.view()->popLayoutState();
    }
}

void LayoutState::clearPaginationInformation()
{
    m_pageLogicalHeight = m_next->m_pageLogicalHeight;
    m_pageOffset = m_next->m_pageOffset;
}

LayoutUnit LayoutState::pageLogicalOffset(const LayoutBox& child, const LayoutUnit& childLogicalOffset) const
{
    if (child.isHorizontalWritingMode())
        return m_layoutOffset.height() + childLogicalOffset - m_pageOffset.height();
    return m_layoutOffset.width() + childLogicalOffset - m_pageOffset.width();
}

} // namespace blink
