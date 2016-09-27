// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/LineBoxListPainter.h"

#include "core/layout/LayoutBoxModelObject.h"
#include "core/layout/line/InlineFlowBox.h"
#include "core/layout/line/LineBoxList.h"
#include "core/layout/line/RootInlineBox.h"
#include "core/paint/InlinePainter.h"
#include "core/paint/ObjectPainter.h"
#include "core/paint/PaintInfo.h"

namespace blink {

static void addPDFURLRectsForInlineChildrenRecursively(LayoutObject* layoutObject, const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    for (LayoutObject* child = layoutObject->slowFirstChild(); child; child = child->nextSibling()) {
        if (!child->isLayoutInline() || toLayoutBoxModelObject(child)->hasSelfPaintingLayer())
            continue;
        ObjectPainter(*child).addPDFURLRectIfNeeded(paintInfo, paintOffset);
        addPDFURLRectsForInlineChildrenRecursively(child, paintInfo, paintOffset);
    }
}

void LineBoxListPainter::paint(LayoutBoxModelObject* layoutObject, const PaintInfo& paintInfo, const LayoutPoint& paintOffset) const
{
    // Only paint during the foreground/selection phases.
    if (paintInfo.phase != PaintPhaseForeground && paintInfo.phase != PaintPhaseSelection && paintInfo.phase != PaintPhaseOutline
        && paintInfo.phase != PaintPhaseSelfOutline && paintInfo.phase != PaintPhaseChildOutlines && paintInfo.phase != PaintPhaseTextClip
        && paintInfo.phase != PaintPhaseMask)
        return;

    ASSERT(layoutObject->isLayoutBlock() || (layoutObject->isLayoutInline() && layoutObject->hasLayer())); // The only way an inline could paint like this is if it has a layer.

    // FIXME: When Skia supports annotation rect covering (https://code.google.com/p/skia/issues/detail?id=3872),
    // these rects may be covered line box drawings. Then we may need a dedicated paint phase.
    if (paintInfo.phase == PaintPhaseForeground && paintInfo.context->printing())
        addPDFURLRectsForInlineChildrenRecursively(layoutObject, paintInfo, paintOffset);

    // If we have no lines then we have no work to do.
    if (!m_lineBoxList.firstLineBox())
        return;

    if (!m_lineBoxList.anyLineIntersectsRect(layoutObject, LayoutRect(paintInfo.rect), paintOffset))
        return;

    PaintInfo info(paintInfo);
    ListHashSet<LayoutInline*> outlineObjects;
    info.setOutlineObjects(&outlineObjects);

    // See if our root lines intersect with the dirty rect. If so, then we paint
    // them. Note that boxes can easily overlap, so we can't make any assumptions
    // based off positions of our first line box or our last line box.
    for (InlineFlowBox* curr = m_lineBoxList.firstLineBox(); curr; curr = curr->nextLineBox()) {
        if (m_lineBoxList.lineIntersectsDirtyRect(layoutObject, curr, info, paintOffset)) {
            RootInlineBox& root = curr->root();
            curr->paint(info, paintOffset, root.lineTop(), root.lineBottom());
        }
    }

    if (info.phase == PaintPhaseOutline || info.phase == PaintPhaseSelfOutline || info.phase == PaintPhaseChildOutlines) {
        for (LayoutInline* flow : *info.outlineObjects())
            InlinePainter(*flow).paintOutline(info, paintOffset);
        info.outlineObjects()->clear();
    }
}

} // namespace blink
