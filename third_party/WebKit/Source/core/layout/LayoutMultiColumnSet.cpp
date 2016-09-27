/*
 * Copyright (C) 2012 Apple Inc.  All rights reserved.
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
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/layout/LayoutMultiColumnSet.h"

#include "core/editing/PositionWithAffinity.h"
#include "core/layout/LayoutMultiColumnFlowThread.h"
#include "core/layout/MultiColumnFragmentainerGroup.h"
#include "core/paint/MultiColumnSetPainter.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace blink {

LayoutMultiColumnSet::LayoutMultiColumnSet(LayoutFlowThread* flowThread)
    : LayoutBlockFlow(nullptr)
    , m_fragmentainerGroups(*this)
    , m_flowThread(flowThread)
{
}

LayoutMultiColumnSet* LayoutMultiColumnSet::createAnonymous(LayoutFlowThread& flowThread, const ComputedStyle& parentStyle)
{
    Document& document = flowThread.document();
    LayoutMultiColumnSet* layoutObject = new LayoutMultiColumnSet(&flowThread);
    layoutObject->setDocumentForAnonymous(&document);
    layoutObject->setStyle(ComputedStyle::createAnonymousStyleWithDisplay(parentStyle, BLOCK));
    return layoutObject;
}

MultiColumnFragmentainerGroup& LayoutMultiColumnSet::fragmentainerGroupAtFlowThreadOffset(LayoutUnit)
{
    // FIXME: implement this, once we have support for multiple rows.
    return m_fragmentainerGroups.first();
}

const MultiColumnFragmentainerGroup& LayoutMultiColumnSet::fragmentainerGroupAtFlowThreadOffset(LayoutUnit) const
{
    // FIXME: implement this, once we have support for multiple rows.
    return m_fragmentainerGroups.first();
}

const MultiColumnFragmentainerGroup& LayoutMultiColumnSet::fragmentainerGroupAtVisualPoint(const LayoutPoint&) const
{
    // FIXME: implement this, once we have support for multiple rows.
    return m_fragmentainerGroups.first();
}

LayoutUnit LayoutMultiColumnSet::pageLogicalHeight() const
{
    // FIXME: pageLogicalHeight() needs to take a flow thread offset parameter, so that we can
    // locate the right row.
    return firstFragmentainerGroup().logicalHeight();
}

LayoutMultiColumnSet* LayoutMultiColumnSet::nextSiblingMultiColumnSet() const
{
    for (LayoutObject* sibling = nextSibling(); sibling; sibling = sibling->nextSibling()) {
        if (sibling->isLayoutMultiColumnSet())
            return toLayoutMultiColumnSet(sibling);
    }
    return nullptr;
}

LayoutMultiColumnSet* LayoutMultiColumnSet::previousSiblingMultiColumnSet() const
{
    for (LayoutObject* sibling = previousSibling(); sibling; sibling = sibling->previousSibling()) {
        if (sibling->isLayoutMultiColumnSet())
            return toLayoutMultiColumnSet(sibling);
    }
    return nullptr;
}

LayoutUnit LayoutMultiColumnSet::logicalTopInFlowThread() const
{
    return firstFragmentainerGroup().logicalTopInFlowThread();
}

LayoutUnit LayoutMultiColumnSet::logicalBottomInFlowThread() const
{
    return lastFragmentainerGroup().logicalBottomInFlowThread();
}

LayoutRect LayoutMultiColumnSet::flowThreadPortionOverflowRect() const
{
    return overflowRectForFlowThreadPortion(flowThreadPortionRect(), !previousSiblingMultiColumnSet(), !nextSiblingMultiColumnSet());
}

LayoutRect LayoutMultiColumnSet::overflowRectForFlowThreadPortion(const LayoutRect& flowThreadPortionRect, bool isFirstPortion, bool isLastPortion) const
{
    if (hasOverflowClip())
        return flowThreadPortionRect;

    LayoutRect flowThreadOverflow = m_flowThread->visualOverflowRect();

    // Only clip along the flow thread axis.
    LayoutRect clipRect;
    if (m_flowThread->isHorizontalWritingMode()) {
        LayoutUnit minY = isFirstPortion ? flowThreadOverflow.y() : flowThreadPortionRect.y();
        LayoutUnit maxY = isLastPortion ? std::max(flowThreadPortionRect.maxY(), flowThreadOverflow.maxY()) : flowThreadPortionRect.maxY();
        LayoutUnit minX = std::min(flowThreadPortionRect.x(), flowThreadOverflow.x());
        LayoutUnit maxX = std::max(flowThreadPortionRect.maxX(), flowThreadOverflow.maxX());
        clipRect = LayoutRect(minX, minY, maxX - minX, maxY - minY);
    } else {
        LayoutUnit minX = isFirstPortion ? flowThreadOverflow.x() : flowThreadPortionRect.x();
        LayoutUnit maxX = isLastPortion ? std::max(flowThreadPortionRect.maxX(), flowThreadOverflow.maxX()) : flowThreadPortionRect.maxX();
        LayoutUnit minY = std::min(flowThreadPortionRect.y(), (flowThreadOverflow.y()));
        LayoutUnit maxY = std::max(flowThreadPortionRect.y(), (flowThreadOverflow.maxY()));
        clipRect = LayoutRect(minX, minY, maxX - minX, maxY - minY);
    }

    return clipRect;
}

bool LayoutMultiColumnSet::heightIsAuto() const
{
    LayoutMultiColumnFlowThread* flowThread = multiColumnFlowThread();
    if (!flowThread->isLayoutPagedFlowThread()) {
        // If support for the column-fill property isn't enabled, we want to behave as if
        // column-fill were auto, so that multicol containers with specified height don't get their
        // columns balanced (auto-height multicol containers will still get their columns balanced,
        // even if column-fill isn't 'balance' - in accordance with the spec). Pretending that
        // column-fill is auto also matches the old multicol implementation, which has no support
        // for this property.
        if (RuntimeEnabledFeatures::columnFillEnabled()) {
            if (multiColumnBlockFlow()->style()->columnFill() == ColumnFillBalance)
                return true;
        }
        if (LayoutBox* next = nextSiblingBox()) {
            if (next->isLayoutMultiColumnSpannerPlaceholder()) {
                // If we're followed by a spanner, we need to balance.
                return true;
            }
        }
    }
    return !flowThread->columnHeightAvailable();
}

LayoutSize LayoutMultiColumnSet::flowThreadTranslationAtOffset(LayoutUnit blockOffset) const
{
    return fragmentainerGroupAtFlowThreadOffset(blockOffset).flowThreadTranslationAtOffset(blockOffset);
}

LayoutPoint LayoutMultiColumnSet::visualPointToFlowThreadPoint(const LayoutPoint& visualPoint) const
{
    const MultiColumnFragmentainerGroup& row = fragmentainerGroupAtVisualPoint(visualPoint);
    return row.visualPointToFlowThreadPoint(visualPoint - row.offsetFromColumnSet());
}

void LayoutMultiColumnSet::updateMinimumColumnHeight(LayoutUnit offsetInFlowThread, LayoutUnit height)
{
    fragmentainerGroupAtFlowThreadOffset(offsetInFlowThread).updateMinimumColumnHeight(height);
}

LayoutUnit LayoutMultiColumnSet::pageLogicalTopForOffset(LayoutUnit offset) const
{
    return fragmentainerGroupAtFlowThreadOffset(offset).columnLogicalTopForOffset(offset);
}

void LayoutMultiColumnSet::addContentRun(LayoutUnit endOffsetFromFirstPage)
{
    if (!heightIsAuto())
        return;
    fragmentainerGroupAtFlowThreadOffset(endOffsetFromFirstPage).addContentRun(endOffsetFromFirstPage);
}

bool LayoutMultiColumnSet::recalculateColumnHeight(BalancedColumnHeightCalculation calculationMode)
{
    bool changed = false;
    for (auto& group : m_fragmentainerGroups)
        changed = group.recalculateColumnHeight(calculationMode) || changed;
    return changed;
}

void LayoutMultiColumnSet::recordSpaceShortage(LayoutUnit offsetInFlowThread, LayoutUnit spaceShortage)
{
    fragmentainerGroupAtFlowThreadOffset(offsetInFlowThread).recordSpaceShortage(spaceShortage);
}

void LayoutMultiColumnSet::resetColumnHeight()
{
    m_fragmentainerGroups.deleteExtraGroups();
    m_fragmentainerGroups.first().resetColumnHeight();
}

void LayoutMultiColumnSet::beginFlow(LayoutUnit offsetInFlowThread)
{
    // At this point layout is exactly at the beginning of this set. Store block offset from flow
    // thread start.
    m_fragmentainerGroups.first().setLogicalTopInFlowThread(offsetInFlowThread);
}

void LayoutMultiColumnSet::endFlow(LayoutUnit offsetInFlowThread)
{
    // At this point layout is exactly at the end of this set. Store block offset from flow thread
    // start. This set is now considered "flowed", although we may have to revisit it later (with
    // beginFlow()), e.g. if a subtree in the flow thread has to be laid out over again because the
    // initial margin collapsing estimates were wrong.
    m_fragmentainerGroups.last().setLogicalBottomInFlowThread(offsetInFlowThread);
}

void LayoutMultiColumnSet::expandToEncompassFlowThreadContentsIfNeeded()
{
    ASSERT(multiColumnFlowThread()->lastMultiColumnSet() == this);
    // FIXME: this may result in the need for creating additional rows, since there may not be
    // enough space remaining in the currently last row.
    m_fragmentainerGroups.last().expandToEncompassFlowThreadOverflow();
}

void LayoutMultiColumnSet::computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const
{
    minLogicalWidth = m_flowThread->minPreferredLogicalWidth();
    maxLogicalWidth = m_flowThread->maxPreferredLogicalWidth();
}

void LayoutMultiColumnSet::computeLogicalHeight(LayoutUnit, LayoutUnit logicalTop, LogicalExtentComputedValues& computedValues) const
{
    LayoutUnit logicalHeight;
    for (const auto& group : m_fragmentainerGroups)
        logicalHeight += group.logicalHeight();
    computedValues.m_extent = logicalHeight;
    computedValues.m_position = logicalTop;
}

PositionWithAffinity LayoutMultiColumnSet::positionForPoint(const LayoutPoint& point)
{
    // Convert the visual point to a flow thread point.
    const MultiColumnFragmentainerGroup& row = fragmentainerGroupAtVisualPoint(point);
    LayoutPoint flowThreadPoint = row.visualPointToFlowThreadPoint(point + row.offsetFromColumnSet());
    // Then drill into the flow thread, where we'll find the actual content.
    return flowThread()->positionForPoint(flowThreadPoint);
}

LayoutUnit LayoutMultiColumnSet::columnGap() const
{
    LayoutBlockFlow* parentBlock = multiColumnBlockFlow();
    if (parentBlock->style()->hasNormalColumnGap())
        return parentBlock->style()->fontDescription().computedPixelSize(); // "1em" is recommended as the normal gap setting. Matches <p> margins.
    return parentBlock->style()->columnGap();
}

unsigned LayoutMultiColumnSet::actualColumnCount() const
{
    // FIXME: remove this method. It's a meaningless question to ask the set "how many columns do
    // you actually have?", since that may vary for each row.
    return firstFragmentainerGroup().actualColumnCount();
}

void LayoutMultiColumnSet::paintObject(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    MultiColumnSetPainter(*this).paintObject(paintInfo, paintOffset);
}

void LayoutMultiColumnSet::collectLayerFragments(DeprecatedPaintLayerFragments& fragments, const LayoutRect& layerBoundingBox, const LayoutRect& dirtyRect)
{
    for (const auto& group : m_fragmentainerGroups)
        group.collectLayerFragments(fragments, layerBoundingBox, dirtyRect);
}

void LayoutMultiColumnSet::addOverflowFromChildren()
{
    LayoutRect overflowRect;
    for (const auto& group : m_fragmentainerGroups) {
        LayoutRect rect = group.calculateOverflow();
        rect.move(group.offsetFromColumnSet());
        overflowRect.unite(rect);
    }
    addLayoutOverflow(overflowRect);
    if (!hasOverflowClip())
        addVisualOverflow(overflowRect);
}

void LayoutMultiColumnSet::insertedIntoTree()
{
    LayoutBlockFlow::insertedIntoTree();
    attachToFlowThread();
}

void LayoutMultiColumnSet::willBeRemovedFromTree()
{
    LayoutBlockFlow::willBeRemovedFromTree();
    detachFromFlowThread();
}

void LayoutMultiColumnSet::attachToFlowThread()
{
    if (documentBeingDestroyed())
        return;

    if (!m_flowThread)
        return;

    m_flowThread->addColumnSetToThread(this);
}

void LayoutMultiColumnSet::detachFromFlowThread()
{
    if (m_flowThread) {
        m_flowThread->removeColumnSetFromThread(this);
        m_flowThread = 0;
    }
}

LayoutRect LayoutMultiColumnSet::flowThreadPortionRect() const
{
    LayoutRect portionRect(LayoutUnit(), logicalTopInFlowThread(), pageLogicalWidth(), logicalHeightInFlowThread());
    if (!isHorizontalWritingMode())
        return portionRect.transposedRect();
    return portionRect;
}

}
