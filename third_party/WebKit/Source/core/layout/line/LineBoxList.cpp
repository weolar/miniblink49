/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/layout/line/LineBoxList.h"

#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutInline.h"
#include "core/layout/LayoutView.h"
#include "core/layout/line/InlineTextBox.h"
#include "core/layout/line/RootInlineBox.h"
#include "core/paint/InlinePainter.h"
#include "core/paint/PaintInfo.h"

namespace blink {

#if ENABLE(ASSERT)
LineBoxList::~LineBoxList()
{
    ASSERT(!m_firstLineBox);
    ASSERT(!m_lastLineBox);
}
#endif

void LineBoxList::appendLineBox(InlineFlowBox* box)
{
    checkConsistency();

    if (!m_firstLineBox) {
        m_firstLineBox = m_lastLineBox = box;
    } else {
        m_lastLineBox->setNextLineBox(box);
        box->setPreviousLineBox(m_lastLineBox);
        m_lastLineBox = box;
    }

    checkConsistency();
}

void LineBoxList::deleteLineBoxTree()
{
    InlineFlowBox* line = m_firstLineBox;
    InlineFlowBox* nextLine;
    while (line) {
        nextLine = line->nextLineBox();
        line->deleteLine();
        line = nextLine;
    }
    m_firstLineBox = m_lastLineBox = nullptr;
}

void LineBoxList::extractLineBox(InlineFlowBox* box)
{
    checkConsistency();

    m_lastLineBox = box->prevLineBox();
    if (box == m_firstLineBox)
        m_firstLineBox = nullptr;
    if (box->prevLineBox())
        box->prevLineBox()->setNextLineBox(nullptr);
    box->setPreviousLineBox(nullptr);
    for (InlineFlowBox* curr = box; curr; curr = curr->nextLineBox())
        curr->setExtracted();

    checkConsistency();
}

void LineBoxList::attachLineBox(InlineFlowBox* box)
{
    checkConsistency();

    if (m_lastLineBox) {
        m_lastLineBox->setNextLineBox(box);
        box->setPreviousLineBox(m_lastLineBox);
    } else {
        m_firstLineBox = box;
    }
    InlineFlowBox* last = box;
    for (InlineFlowBox* curr = box; curr; curr = curr->nextLineBox()) {
        curr->setExtracted(false);
        last = curr;
    }
    m_lastLineBox = last;

    checkConsistency();
}

void LineBoxList::removeLineBox(InlineFlowBox* box)
{
    checkConsistency();

    if (box == m_firstLineBox)
        m_firstLineBox = box->nextLineBox();
    if (box == m_lastLineBox)
        m_lastLineBox = box->prevLineBox();
    if (box->nextLineBox())
        box->nextLineBox()->setPreviousLineBox(box->prevLineBox());
    if (box->prevLineBox())
        box->prevLineBox()->setNextLineBox(box->nextLineBox());

    checkConsistency();
}

void LineBoxList::deleteLineBoxes()
{
    if (m_firstLineBox) {
        InlineFlowBox* next;
        for (InlineFlowBox* curr = m_firstLineBox; curr; curr = next) {
            next = curr->nextLineBox();
            curr->destroy();
        }
        m_firstLineBox = nullptr;
        m_lastLineBox = nullptr;
    }
}

void LineBoxList::dirtyLineBoxes()
{
    for (InlineFlowBox* curr = firstLineBox(); curr; curr = curr->nextLineBox())
        curr->dirtyLineBoxes();
}

bool LineBoxList::rangeIntersectsRect(LayoutBoxModelObject* layoutObject, LayoutUnit logicalTop, LayoutUnit logicalBottom, const LayoutRect& rect, const LayoutPoint& offset) const
{
    LayoutBox* block;
    if (layoutObject->isBox())
        block = toLayoutBox(layoutObject);
    else
        block = layoutObject->containingBlock();
    LayoutUnit physicalStart = block->flipForWritingMode(logicalTop);
    LayoutUnit physicalEnd = block->flipForWritingMode(logicalBottom);
    LayoutUnit physicalExtent = absoluteValue(physicalEnd - physicalStart);
    physicalStart = std::min(physicalStart, physicalEnd);

    if (layoutObject->style()->isHorizontalWritingMode()) {
        physicalStart += offset.y();
        if (physicalStart >= rect.maxY() || physicalStart + physicalExtent <= rect.y())
            return false;
    } else {
        physicalStart += offset.x();
        if (physicalStart >= rect.maxX() || physicalStart + physicalExtent <= rect.x())
            return false;
    }

    return true;
}

bool LineBoxList::anyLineIntersectsRect(LayoutBoxModelObject* layoutObject, const LayoutRect& rect, const LayoutPoint& offset) const
{
    // We can check the first box and last box and avoid painting/hit testing if we don't
    // intersect.  This is a quick short-circuit that we can take to avoid walking any lines.
    // FIXME: This check is flawed in the following extremely obscure way:
    // if some line in the middle has a huge overflow, it might actually extend below the last line.
    RootInlineBox& firstRootBox = firstLineBox()->root();
    RootInlineBox& lastRootBox = lastLineBox()->root();
    LayoutUnit firstLineTop = firstLineBox()->logicalTopVisualOverflow(firstRootBox.lineTop());
    LayoutUnit lastLineBottom = lastLineBox()->logicalBottomVisualOverflow(lastRootBox.lineBottom());

    return rangeIntersectsRect(layoutObject, firstLineTop, lastLineBottom, rect, offset);
}

bool LineBoxList::lineIntersectsDirtyRect(LayoutBoxModelObject* layoutObject, InlineFlowBox* box, const PaintInfo& paintInfo, const LayoutPoint& offset) const
{
    RootInlineBox& root = box->root();
    LayoutUnit logicalTop = std::min<LayoutUnit>(box->logicalTopVisualOverflow(root.lineTop()), root.selectionTop());
    LayoutUnit logicalBottom = box->logicalBottomVisualOverflow(root.lineBottom());

    return rangeIntersectsRect(layoutObject, logicalTop, logicalBottom, LayoutRect(paintInfo.rect), offset);
}

bool LineBoxList::hitTest(LayoutBoxModelObject* layoutObject, HitTestResult& result, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction hitTestAction) const
{
    if (hitTestAction != HitTestForeground)
        return false;

    ASSERT(layoutObject->isLayoutBlock() || (layoutObject->isLayoutInline() && layoutObject->hasLayer())); // The only way an inline could hit test like this is if it has a layer.

    // If we have no lines then we have no work to do.
    if (!firstLineBox())
        return false;

    LayoutPoint point = locationInContainer.point();
    LayoutRect rect(firstLineBox()->isHorizontal() ?
        IntRect(point.x(), point.y() - locationInContainer.topPadding(), 1, locationInContainer.topPadding() + locationInContainer.bottomPadding() + 1) :
        IntRect(point.x() - locationInContainer.leftPadding(), point.y(), locationInContainer.rightPadding() + locationInContainer.leftPadding() + 1, 1));

    if (!anyLineIntersectsRect(layoutObject, rect, accumulatedOffset))
        return false;

    // See if our root lines contain the point.  If so, then we hit test
    // them further.  Note that boxes can easily overlap, so we can't make any assumptions
    // based off positions of our first line box or our last line box.
    for (InlineFlowBox* curr = lastLineBox(); curr; curr = curr->prevLineBox()) {
        RootInlineBox& root = curr->root();
        if (rangeIntersectsRect(layoutObject, curr->logicalTopVisualOverflow(root.lineTop()), curr->logicalBottomVisualOverflow(root.lineBottom()), rect, accumulatedOffset)) {
            bool inside = curr->nodeAtPoint(result, locationInContainer, accumulatedOffset, root.lineTop(), root.lineBottom());
            if (inside) {
                layoutObject->updateHitTestResult(result, locationInContainer.point() - toLayoutSize(accumulatedOffset));
                return true;
            }
        }
    }

    return false;
}

void LineBoxList::dirtyLinesFromChangedChild(LayoutObject* container, LayoutObject* child)
{
    if (!container->parent() || (container->isLayoutBlock() && (container->selfNeedsLayout() || !container->isLayoutBlockFlow())))
        return;

    LayoutInline* inlineContainer = container->isLayoutInline() ? toLayoutInline(container) : 0;
    InlineBox* firstBox = inlineContainer ? inlineContainer->firstLineBoxIncludingCulling() : firstLineBox();

    // If we have no first line box, then just bail early.
    if (!firstBox) {
        // For an empty inline, go ahead and propagate the check up to our parent, unless the parent
        // is already dirty.
        if (container->isInline() && !container->ancestorLineBoxDirty()) {
            container->parent()->dirtyLinesFromChangedChild(container);
            container->setAncestorLineBoxDirty(); // Mark the container to avoid dirtying the same lines again across multiple destroy() calls of the same subtree.
        }
        return;
    }

    // Try to figure out which line box we belong in.  First try to find a previous
    // line box by examining our siblings.  If we didn't find a line box, then use our
    // parent's first line box.
    RootInlineBox* box = nullptr;
    LayoutObject* curr = nullptr;
    for (curr = child->previousSibling(); curr; curr = curr->previousSibling()) {
        if (curr->isFloatingOrOutOfFlowPositioned())
            continue;

        if (curr->isReplaced()) {
            InlineBox* wrapper = toLayoutBox(curr)->inlineBoxWrapper();
            if (wrapper)
                box = &wrapper->root();
        } else if (curr->isText()) {
            InlineTextBox* textBox = toLayoutText(curr)->lastTextBox();
            if (textBox)
                box = &textBox->root();
        } else if (curr->isLayoutInline()) {
            InlineBox* lastSiblingBox = toLayoutInline(curr)->lastLineBoxIncludingCulling();
            if (lastSiblingBox)
                box = &lastSiblingBox->root();
        }

        if (box)
            break;
    }
    if (!box) {
        if (inlineContainer && !inlineContainer->alwaysCreateLineBoxes()) {
            // https://bugs.webkit.org/show_bug.cgi?id=60778
            // We may have just removed a <br> with no line box that was our first child. In this case
            // we won't find a previous sibling, but firstBox can be pointing to a following sibling.
            // This isn't good enough, since we won't locate the root line box that encloses the removed
            // <br>. We have to just over-invalidate a bit and go up to our parent.
            if (!inlineContainer->ancestorLineBoxDirty()) {
                inlineContainer->parent()->dirtyLinesFromChangedChild(inlineContainer);
                inlineContainer->setAncestorLineBoxDirty(); // Mark the container to avoid dirtying the same lines again across multiple destroy() calls of the same subtree.
            }
            return;
        }
        box = &firstBox->root();
    }

    // If we found a line box, then dirty it.
    if (box) {
        box->markDirty();

        // dirty the adjacent lines that might be affected
        // NOTE: we dirty the previous line because RootInlineBox objects cache
        // the address of the first object on the next line after a BR, which we may be
        // invalidating here.  For more info, see how LayoutBlock::layoutInlineChildren
        // calls setLineBreakInfo with the result of findNextLineBreak.  findNextLineBreak,
        // despite the name, actually returns the first LayoutObject after the BR.
        // <rdar://problem/3849947> "Typing after pasting line does not appear until after window resize."
        if (RootInlineBox* prevRootBox = box->prevRootBox())
            prevRootBox->markDirty();
        // If |child| or any of its immediately previous siblings with culled lineboxes is the object after a line-break in |box| or the linebox after it
        // then that means |child| actually sits on the linebox after |box| (or is its line-break object) and so we need to dirty it as well.
        if (RootInlineBox* nextRootBox = box->nextRootBox())
            nextRootBox->markDirty();
    }
}

#if ENABLE(ASSERT)

void LineBoxList::checkConsistency() const
{
#ifdef CHECK_CONSISTENCY
    const InlineFlowBox* prev = nullptr;
    for (const InlineFlowBox* child = m_firstLineBox; child; child = child->nextLineBox()) {
        ASSERT(child->prevLineBox() == prev);
        prev = child;
    }
    ASSERT(prev == m_lastLineBox);
#endif
}

#endif

}
