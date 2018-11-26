/*
 * Copyright (C) 2003, 2004, 2005, 2006, 2007 Apple Inc. All rights reserved.
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
#include "core/layout/line/InlineBox.h"

#include "core/layout/HitTestLocation.h"
#include "core/layout/LayoutBlockFlow.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/line/InlineFlowBox.h"
#include "core/layout/line/RootInlineBox.h"
#include "core/paint/BlockPainter.h"
#include "core/paint/PaintInfo.h"
#include "platform/fonts/FontMetrics.h"
#include "wtf/Partitions.h"

#ifndef NDEBUG
#include <stdio.h>
#endif

namespace blink {

struct SameSizeAsInlineBox {
    virtual ~SameSizeAsInlineBox() { }
    void* a[4];
    LayoutPoint b;
    LayoutUnit c;
    uint32_t d : 32;
#if ENABLE(ASSERT)
    bool f;
#endif
};

static_assert(sizeof(InlineBox) == sizeof(SameSizeAsInlineBox), "InlineBox should stay small");

#if ENABLE(ASSERT)

InlineBox::~InlineBox()
{
    if (!m_hasBadParent && m_parent)
        m_parent->setHasBadChildList();
}

#endif

void InlineBox::destroy()
{
    m_layoutObject.invalidateDisplayItemClient(*this);
    delete this;
}

void InlineBox::remove(MarkLineBoxes markLineBoxes)
{
    if (parent())
        parent()->removeChild(this, markLineBoxes);
}

void* InlineBox::operator new(size_t sz)
{
    return partitionAlloc(WTF::Partitions::layoutPartition(), sz, "InlineBox::operator new");
}

void InlineBox::operator delete(void* ptr)
{
    partitionFree(ptr);
}

const char* InlineBox::boxName() const
{
    return "InlineBox";
}

String InlineBox::debugName() const
{
    return boxName();
}

#ifndef NDEBUG
void InlineBox::showTreeForThis() const
{
    layoutObject().showTreeForThis();
}

void InlineBox::showLineTreeForThis() const
{
    layoutObject().containingBlock()->showLineTreeAndMark(this, "*");
}

void InlineBox::showLineTreeAndMark(const InlineBox* markedBox1, const char* markedLabel1, const InlineBox* markedBox2, const char* markedLabel2, const LayoutObject* obj, int depth) const
{
    int printedCharacters = 0;
    if (this == markedBox1)
        printedCharacters += fprintf(stderr, "%s", markedLabel1);
    if (this == markedBox2)
        printedCharacters += fprintf(stderr, "%s", markedLabel2);
    if (&layoutObject() == obj)
        printedCharacters += fprintf(stderr, "*");
    for (; printedCharacters < depth * 2; printedCharacters++)
        fputc(' ', stderr);

    showBox(printedCharacters);
}

void InlineBox::showBox(int printedCharacters) const
{
    printedCharacters += fprintf(stderr, "%s %p", boxName(), this);
    for (; printedCharacters < showTreeCharacterOffset; printedCharacters++)
        fputc(' ', stderr);
    fprintf(stderr, "\t%s %p {pos=%g,%g size=%g,%g} baseline=%i/%i\n",
        layoutObject().decoratedName().ascii().data(), &layoutObject(),
        x().toFloat(), y().toFloat(), width().toFloat(), height().toFloat(),
        baselinePosition(AlphabeticBaseline), baselinePosition(IdeographicBaseline));
}
#endif

LayoutUnit InlineBox::logicalHeight() const
{
    if (hasVirtualLogicalHeight())
        return virtualLogicalHeight();

    if (layoutObject().isText())
        return m_bitfields.isText() ? LayoutUnit(layoutObject().style(isFirstLineStyle())->fontMetrics().height()) : LayoutUnit();
    if (layoutObject().isBox() && parent())
        return isHorizontal() ? toLayoutBox(layoutObject()).size().height() : toLayoutBox(layoutObject()).size().width();

    ASSERT(isInlineFlowBox());
    LayoutBoxModelObject* flowObject = boxModelObject();
    const FontMetrics& fontMetrics = layoutObject().style(isFirstLineStyle())->fontMetrics();
    LayoutUnit result = fontMetrics.height();
    if (parent())
        result += flowObject->borderAndPaddingLogicalHeight();
    return result;
}

int InlineBox::baselinePosition(FontBaseline baselineType) const
{
    return boxModelObject()->baselinePosition(baselineType, m_bitfields.firstLine(), isHorizontal() ? HorizontalLine : VerticalLine, PositionOnContainingLine);
}

LayoutUnit InlineBox::lineHeight() const
{
    return boxModelObject()->lineHeight(m_bitfields.firstLine(), isHorizontal() ? HorizontalLine : VerticalLine, PositionOnContainingLine);
}

int InlineBox::caretMinOffset() const
{
    return layoutObject().caretMinOffset();
}

int InlineBox::caretMaxOffset() const
{
    return layoutObject().caretMaxOffset();
}

void InlineBox::dirtyLineBoxes()
{
    markDirty();
    for (InlineFlowBox* curr = parent(); curr && !curr->isDirty(); curr = curr->parent())
        curr->markDirty();
}

void InlineBox::deleteLine()
{
    if (!m_bitfields.extracted() && layoutObject().isBox())
        toLayoutBox(layoutObject()).setInlineBoxWrapper(nullptr);
    destroy();
}

void InlineBox::extractLine()
{
    m_bitfields.setExtracted(true);
    if (layoutObject().isBox())
        toLayoutBox(layoutObject()).setInlineBoxWrapper(nullptr);
}

void InlineBox::attachLine()
{
    m_bitfields.setExtracted(false);
    if (layoutObject().isBox())
        toLayoutBox(layoutObject()).setInlineBoxWrapper(this);
}

void InlineBox::move(const LayoutSize& delta)
{
    m_topLeft.move(delta);

    if (layoutObject().isReplaced())
        toLayoutBox(layoutObject()).move(delta.width(), delta.height());
}

void InlineBox::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset, LayoutUnit /* lineTop */, LayoutUnit /* lineBottom */)
{
    // Text clips are painted only for the direct inline children of the object that has a text clip style on it, not block children.
    if (paintInfo.phase != PaintPhaseTextClip)
        BlockPainter::paintInlineBox(*this, paintInfo, paintOffset);
}

bool InlineBox::nodeAtPoint(HitTestResult& result, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, LayoutUnit /* lineTop */, LayoutUnit /* lineBottom */)
{
    // Hit test all phases of replaced elements atomically, as though the replaced element established its
    // own stacking context.  (See Appendix E.2, section 6.4 on inline block/table elements in the CSS2.1
    // specification.)
    LayoutPoint childPoint = accumulatedOffset;
    if (parent()->layoutObject().hasFlippedBlocksWritingMode()) // Faster than calling containingBlock().
        childPoint = layoutObject().containingBlock()->flipForWritingModeForChild(&toLayoutBox(layoutObject()), childPoint);

    if (layoutObject().style()->hasBorderRadius()) {
        LayoutRect borderRect = logicalFrameRect();
        borderRect.moveBy(accumulatedOffset);
        FloatRoundedRect border = layoutObject().style()->getRoundedBorderFor(borderRect);
        if (!locationInContainer.intersects(border))
            return false;
    }

    return layoutObject().hitTest(result, locationInContainer, childPoint);
}

const RootInlineBox& InlineBox::root() const
{
    if (m_parent)
        return m_parent->root();
    ASSERT(isRootInlineBox());
    return static_cast<const RootInlineBox&>(*this);
}

RootInlineBox& InlineBox::root()
{
    if (m_parent)
        return m_parent->root();
    ASSERT(isRootInlineBox());
    return static_cast<RootInlineBox&>(*this);
}

bool InlineBox::nextOnLineExists() const
{
    if (!m_bitfields.determinedIfNextOnLineExists()) {
        m_bitfields.setDeterminedIfNextOnLineExists(true);

        if (!parent())
            m_bitfields.setNextOnLineExists(false);
        else if (nextOnLine())
            m_bitfields.setNextOnLineExists(true);
        else
            m_bitfields.setNextOnLineExists(parent()->nextOnLineExists());
    }
    return m_bitfields.nextOnLineExists();
}

InlineBox* InlineBox::nextLeafChild() const
{
    InlineBox* leaf = nullptr;
    for (InlineBox* box = nextOnLine(); box && !leaf; box = box->nextOnLine())
        leaf = box->isLeaf() ? box : toInlineFlowBox(box)->firstLeafChild();
    if (!leaf && parent())
        leaf = parent()->nextLeafChild();
    return leaf;
}

InlineBox* InlineBox::prevLeafChild() const
{
    InlineBox* leaf = nullptr;
    for (InlineBox* box = prevOnLine(); box && !leaf; box = box->prevOnLine())
        leaf = box->isLeaf() ? box : toInlineFlowBox(box)->lastLeafChild();
    if (!leaf && parent())
        leaf = parent()->prevLeafChild();
    return leaf;
}

InlineBox* InlineBox::nextLeafChildIgnoringLineBreak() const
{
    InlineBox* leaf = nextLeafChild();
    return (leaf && leaf->isLineBreak()) ? nullptr : leaf;
}

InlineBox* InlineBox::prevLeafChildIgnoringLineBreak() const
{
    InlineBox* leaf = prevLeafChild();
    return (leaf && leaf->isLineBreak()) ? nullptr : leaf;
}

LayoutObject::SelectionState InlineBox::selectionState() const
{
    return layoutObject().selectionState();
}

bool InlineBox::canAccommodateEllipsis(bool ltr, int blockEdge, int ellipsisWidth) const
{
    // Non-replaced elements can always accommodate an ellipsis.
    if (!layoutObject().isReplaced())
        return true;

    IntRect boxRect(left(), 0, m_logicalWidth, 10);
    IntRect ellipsisRect(ltr ? blockEdge - ellipsisWidth : blockEdge, 0, ellipsisWidth, 10);
    return !(boxRect.intersects(ellipsisRect));
}

LayoutUnit InlineBox::placeEllipsisBox(bool, LayoutUnit, LayoutUnit, LayoutUnit, LayoutUnit& truncatedWidth, bool&)
{
    // Use -1 to mean "we didn't set the position."
    truncatedWidth += logicalWidth();
    return -1;
}

void InlineBox::clearKnownToHaveNoOverflow()
{
    m_bitfields.setKnownToHaveNoOverflow(false);
    if (parent() && parent()->knownToHaveNoOverflow())
        parent()->clearKnownToHaveNoOverflow();
}

LayoutPoint InlineBox::locationIncludingFlipping()
{
    return logicalPositionToPhysicalPoint(m_topLeft, size());
}

LayoutPoint InlineBox::logicalPositionToPhysicalPoint(const LayoutPoint& point, const LayoutSize& size)
{
    if (!UNLIKELY(layoutObject().hasFlippedBlocksWritingMode()))
        return LayoutPoint(point.x(), point.y());

    LayoutBlockFlow& block = root().block();
    if (block.style()->isHorizontalWritingMode())
        return LayoutPoint(point.x(), block.size().height() - size.height() - point.y());

    return LayoutPoint(block.size().width() - size.width() - point.x(), point.y());
}

void InlineBox::logicalRectToPhysicalRect(LayoutRect& current)
{
    if (isHorizontal() && !layoutObject().hasFlippedBlocksWritingMode())
        return;

    if (!isHorizontal()) {
        current = current.transposedRect();
    }
    current.setLocation(logicalPositionToPhysicalPoint(current.location(), current.size()));
    return;
}

void InlineBox::flipForWritingMode(FloatRect& rect)
{
    if (!UNLIKELY(layoutObject().hasFlippedBlocksWritingMode()))
        return;
    root().block().flipForWritingMode(rect);
}

FloatPoint InlineBox::flipForWritingMode(const FloatPoint& point)
{
    if (!UNLIKELY(layoutObject().hasFlippedBlocksWritingMode()))
        return point;
    return root().block().flipForWritingMode(point);
}

void InlineBox::flipForWritingMode(LayoutRect& rect)
{
    if (!UNLIKELY(layoutObject().hasFlippedBlocksWritingMode()))
        return;
    root().block().flipForWritingMode(rect);
}

LayoutPoint InlineBox::flipForWritingMode(const LayoutPoint& point)
{
    if (!UNLIKELY(layoutObject().hasFlippedBlocksWritingMode()))
        return point;
    return root().block().flipForWritingMode(point);
}

} // namespace blink

#ifndef NDEBUG

void showTree(const blink::InlineBox* b)
{
    if (b)
        b->showTreeForThis();
    else
        fprintf(stderr, "Cannot showTree for (nil) InlineBox.\n");
}

void showLineTree(const blink::InlineBox* b)
{
    if (b)
        b->showLineTreeForThis();
    else
        fprintf(stderr, "Cannot showLineTree for (nil) InlineBox.\n");
}

#endif
