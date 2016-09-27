// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/layout/LayoutPagedFlowThread.h"

#include "core/layout/LayoutMultiColumnSet.h"

namespace blink {

LayoutPagedFlowThread* LayoutPagedFlowThread::createAnonymous(Document& document, const ComputedStyle& parentStyle)
{
    LayoutPagedFlowThread* LayoutObject = new LayoutPagedFlowThread();
    LayoutObject->setDocumentForAnonymous(&document);
    LayoutObject->setStyle(ComputedStyle::createAnonymousStyleWithDisplay(parentStyle, BLOCK));
    return LayoutObject;
}

int LayoutPagedFlowThread::pageCount()
{
    if (LayoutMultiColumnSet* columnSet = firstMultiColumnSet())
        return columnSet->actualColumnCount();
    return 1;
}

bool LayoutPagedFlowThread::needsNewWidth() const
{
    return progressionIsInline() != pagedBlockFlow()->style()->hasInlinePaginationAxis();
}

void LayoutPagedFlowThread::updateLogicalWidth()
{
    // As long as we inherit from LayoutMultiColumnFlowThread, we need to bypass its implementation
    // here. We're not split into columns, so the flow thread width will just be whatever is
    // available in the containing block.
    LayoutFlowThread::updateLogicalWidth();
}

void LayoutPagedFlowThread::layout()
{
    ASSERT(firstMultiColumnBox() == lastMultiColumnBox()); // There should either be zero or one of those for paged layout.
    setProgressionIsInline(pagedBlockFlow()->style()->hasInlinePaginationAxis());
    LayoutMultiColumnFlowThread::layout();

    LayoutMultiColumnSet* columnSet = firstMultiColumnSet();
    if (!columnSet)
        return;
    LayoutUnit pageLogicalHeight = columnSet->pageLogicalHeight();
    if (!pageLogicalHeight)
        return; // Page height not calculated yet. Happens in the first layout pass when height is auto.
    // Ensure uniform page height. We don't want the last page to be shorter than the others,
    // or it'll be impossible to scroll that whole page into view.
    LayoutUnit paddedLogicalBottomInFlowThread = pageLogicalHeight * pageCount();
    ASSERT(paddedLogicalBottomInFlowThread >= columnSet->logicalBottomInFlowThread());
    columnSet->endFlow(paddedLogicalBottomInFlowThread);
}

} // namespace blink
