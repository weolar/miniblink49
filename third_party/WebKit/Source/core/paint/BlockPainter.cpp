// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/BlockPainter.h"

#include "core/editing/FrameSelection.h"
#include "core/frame/Settings.h"
#include "core/layout/LayoutFlexibleBox.h"
#include "core/layout/LayoutInline.h"
#include "core/page/Page.h"
#include "core/paint/BoxClipper.h"
#include "core/paint/BoxPainter.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/InlinePainter.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/LineBoxListPainter.h"
#include "core/paint/PaintInfo.h"
#include "core/paint/ScopeRecorder.h"
#include "core/paint/ScrollRecorder.h"
#include "core/paint/ScrollableAreaPainter.h"
#include "platform/graphics/paint/ClipRecorder.h"
#include "wtf/Optional.h"

namespace blink {

void BlockPainter::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    PaintInfo localPaintInfo(paintInfo);

    LayoutPoint adjustedPaintOffset = paintOffset + m_layoutBlock.location();

    PaintPhase originalPhase = localPaintInfo.phase;

    // Check if we need to do anything at all.
    // TODO(schenney): Could eliminate the isDocumentElement() check if we fix background painting so
    // that the LayoutView paints the root's background.
    // TODO(schenney): Test this code path, as breaking changes have failed to show up in testing.
    // crbug.com/509737
    Node* blockNode = m_layoutBlock.node();
    ASSERT(blockNode || m_layoutBlock.isAnonymous());
    if (blockNode) {
        if (!m_layoutBlock.isDocumentElement()) {
            LayoutRect overflowBox = overflowRectForPaintRejection();
            m_layoutBlock.flipForWritingMode(overflowBox);
            overflowBox.moveBy(adjustedPaintOffset);
            if (!overflowBox.intersects(LayoutRect(localPaintInfo.rect)))
                return;
        }
    }

    // There are some cases where not all clipped visual overflow is accounted for.
    // FIXME: reduce the number of such cases.
    ContentsClipBehavior contentsClipBehavior = ForceContentsClip;
    if (m_layoutBlock.hasOverflowClip() && !m_layoutBlock.hasControlClip() && !(m_layoutBlock.shouldPaintSelectionGaps() && originalPhase == PaintPhaseForeground) && !hasCaret())
        contentsClipBehavior = SkipContentsClipIfPossible;

    if (localPaintInfo.phase == PaintPhaseOutline) {
        localPaintInfo.phase = PaintPhaseChildOutlines;
    } else if (localPaintInfo.phase == PaintPhaseChildBlockBackground) {
        localPaintInfo.phase = PaintPhaseBlockBackground;
        m_layoutBlock.paintObject(localPaintInfo, adjustedPaintOffset);
        localPaintInfo.phase = PaintPhaseChildBlockBackgrounds;
    }

    {
        BoxClipper boxClipper(m_layoutBlock, localPaintInfo, adjustedPaintOffset, contentsClipBehavior);
        m_layoutBlock.paintObject(localPaintInfo, adjustedPaintOffset);
    }

    if (originalPhase == PaintPhaseOutline) {
        localPaintInfo.phase = PaintPhaseSelfOutline;
        m_layoutBlock.paintObject(localPaintInfo, adjustedPaintOffset);
        localPaintInfo.phase = originalPhase;
    } else if (originalPhase == PaintPhaseChildBlockBackground) {
        localPaintInfo.phase = originalPhase;
    }

    // Our scrollbar widgets paint exactly when we tell them to, so that they work properly with
    // z-index. We paint after we painted the background/border, so that the scrollbars will
    // sit above the background/border.
    paintOverflowControlsIfNeeded(localPaintInfo, adjustedPaintOffset);
}

void BlockPainter::paintOverflowControlsIfNeeded(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    PaintPhase phase = paintInfo.phase;
    if (m_layoutBlock.hasOverflowClip() && m_layoutBlock.style()->visibility() == VISIBLE && (phase == PaintPhaseBlockBackground || phase == PaintPhaseChildBlockBackground) && paintInfo.shouldPaintWithinRoot(&m_layoutBlock) && !paintInfo.paintRootBackgroundOnly()) {
        ScrollableAreaPainter(*m_layoutBlock.layer()->scrollableArea()).paintOverflowControls(paintInfo.context, roundedIntPoint(paintOffset), paintInfo.rect, false /* paintingOverlayControls */);
    }
}

void BlockPainter::paintChildren(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    for (LayoutBox* child = m_layoutBlock.firstChildBox(); child; child = child->nextSiblingBox())
        paintChild(*child, paintInfo, paintOffset);
}

void BlockPainter::paintChild(LayoutBox& child, const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    LayoutPoint childPoint = m_layoutBlock.flipForWritingModeForChild(&child, paintOffset);
    if (!child.hasSelfPaintingLayer() && !child.isFloating() && !child.isColumnSpanAll())
        child.paint(paintInfo, childPoint);
}

void BlockPainter::paintChildrenOfFlexibleBox(LayoutFlexibleBox& layoutFlexibleBox, const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    for (LayoutBox* child = layoutFlexibleBox.orderIterator().first(); child; child = layoutFlexibleBox.orderIterator().next())
        BlockPainter(layoutFlexibleBox).paintChildAsInlineBlock(*child, paintInfo, paintOffset);
}

void BlockPainter::paintChildAsInlineBlock(LayoutBox& child, const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    LayoutPoint childPoint = m_layoutBlock.flipForWritingModeForChild(&child, paintOffset);
    if (!child.hasSelfPaintingLayer() && !child.isFloating())
        paintAsInlineBlock(child, paintInfo, childPoint);
}

void BlockPainter::paintInlineBox(InlineBox& inlineBox, const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (!paintInfo.shouldPaintWithinRoot(&inlineBox.layoutObject()) || (paintInfo.phase != PaintPhaseForeground && paintInfo.phase != PaintPhaseSelection))
        return;

    LayoutPoint childPoint = paintOffset;
    if (inlineBox.parent()->layoutObject().style()->isFlippedBlocksWritingMode()) // Faster than calling containingBlock().
        childPoint = inlineBox.layoutObject().containingBlock()->flipForWritingModeForChild(&toLayoutBox(inlineBox.layoutObject()), childPoint);

    paintAsInlineBlock(inlineBox.layoutObject(), paintInfo, childPoint);
}

void BlockPainter::paintAsInlineBlock(LayoutObject& layoutObject, const PaintInfo& paintInfo, const LayoutPoint& childPoint)
{
    if (paintInfo.phase != PaintPhaseForeground && paintInfo.phase != PaintPhaseSelection)
        return;

    // Paint all phases atomically, as though the element established its own
    // stacking context.  (See Appendix E.2, section 7.2.1.4 on
    // inline block/table/replaced elements in the CSS2.1 specification.)
    // This is also used by other elements (e.g. flex items and grid items).
    bool preservePhase = paintInfo.phase == PaintPhaseSelection || paintInfo.phase == PaintPhaseTextClip;
    PaintInfo info(paintInfo);
    info.phase = preservePhase ? paintInfo.phase : PaintPhaseBlockBackground;
    layoutObject.paint(info, childPoint);
    if (!preservePhase) {
        info.phase = PaintPhaseChildBlockBackgrounds;
        layoutObject.paint(info, childPoint);
        info.phase = PaintPhaseFloat;
        layoutObject.paint(info, childPoint);
        info.phase = PaintPhaseForeground;
        layoutObject.paint(info, childPoint);
        info.phase = PaintPhaseOutline;
        layoutObject.paint(info, childPoint);
    }
}

static inline LayoutRect visualOverflowRectWithPaintOffset(const LayoutBlock& layoutBox, const LayoutPoint& paintOffset)
{
    if (!RuntimeEnabledFeatures::slimmingPaintEnabled())
        return LayoutRect();
    LayoutRect bounds = layoutBox.visualOverflowRect();
    bounds.moveBy(paintOffset);
    return bounds;
}

void BlockPainter::paintObject(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    const PaintPhase paintPhase = paintInfo.phase;

    if ((paintPhase == PaintPhaseBlockBackground || paintPhase == PaintPhaseChildBlockBackground)
        && m_layoutBlock.style()->visibility() == VISIBLE
        && m_layoutBlock.hasBoxDecorationBackground())
        m_layoutBlock.paintBoxDecorationBackground(paintInfo, paintOffset);

    if (paintPhase == PaintPhaseMask && m_layoutBlock.style()->visibility() == VISIBLE) {
        m_layoutBlock.paintMask(paintInfo, paintOffset);
        return;
    }

    if (paintPhase == PaintPhaseClippingMask && m_layoutBlock.style()->visibility() == VISIBLE) {
        BoxPainter(m_layoutBlock).paintClippingMask(paintInfo, paintOffset);
        return;
    }

    // FIXME: When Skia supports annotation rect covering (https://code.google.com/p/skia/issues/detail?id=3872),
    // this rect may be covered by foreground and descendant drawings. Then we may need a dedicated paint phase.
    if (paintPhase == PaintPhaseForeground && paintInfo.context->printing())
        ObjectPainter(m_layoutBlock).addPDFURLRectIfNeeded(paintInfo, paintOffset);

    {
        Optional<ScrollRecorder> scrollRecorder;
        Optional<PaintInfo> scrolledPaintInfo;
        if (m_layoutBlock.hasOverflowClip()) {
            IntSize scrollOffset = m_layoutBlock.scrolledContentOffset();
            if (m_layoutBlock.layer()->scrollsOverflow() || !scrollOffset.isZero()) {
                scrollRecorder.emplace(*paintInfo.context, m_layoutBlock, paintPhase, scrollOffset);
                scrolledPaintInfo.emplace(paintInfo);
                scrolledPaintInfo->rect.move(scrollOffset);
            }
        }

        // We're done. We don't bother painting any children.
        if (paintPhase == PaintPhaseBlockBackground || paintInfo.paintRootBackgroundOnly())
            return;

        const PaintInfo& contentsPaintInfo = scrolledPaintInfo ? *scrolledPaintInfo : paintInfo;

        if (paintPhase != PaintPhaseSelfOutline)
            paintContents(contentsPaintInfo, paintOffset);

        if (paintPhase == PaintPhaseForeground && !m_layoutBlock.document().printing())
            m_layoutBlock.paintSelection(contentsPaintInfo, paintOffset); // Fill in gaps in selection on lines and between blocks.

        if (paintPhase == PaintPhaseFloat || paintPhase == PaintPhaseSelection || paintPhase == PaintPhaseTextClip)
            m_layoutBlock.paintFloats(contentsPaintInfo, paintOffset, paintPhase == PaintPhaseSelection || paintPhase == PaintPhaseTextClip);
    }

    if ((paintPhase == PaintPhaseOutline || paintPhase == PaintPhaseSelfOutline) && m_layoutBlock.style()->hasOutline() && m_layoutBlock.style()->visibility() == VISIBLE) {
        // Don't paint focus ring for anonymous block continuation because the
        // inline element having outline-style:auto paints the whole focus ring.
        if (!m_layoutBlock.style()->outlineStyleIsAuto() || !m_layoutBlock.isAnonymousBlockContinuation())
            ObjectPainter(m_layoutBlock).paintOutline(paintInfo, LayoutRect(paintOffset, m_layoutBlock.size()), visualOverflowRectWithPaintOffset(m_layoutBlock, paintOffset));
    }

    if (paintPhase == PaintPhaseOutline || paintPhase == PaintPhaseChildOutlines)
        paintContinuationOutlines(paintInfo, paintOffset);

    // If the caret's node's layout object's containing block is this block, and the paint action is PaintPhaseForeground,
    // then paint the caret.
    if (paintPhase == PaintPhaseForeground && hasCaret() && !LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*paintInfo.context, m_layoutBlock, DisplayItem::Caret)) {
        LayoutObjectDrawingRecorder recorder(*paintInfo.context, m_layoutBlock, DisplayItem::Caret, visualOverflowRectWithPaintOffset(m_layoutBlock, paintOffset));
        paintCarets(paintInfo, paintOffset);
    }
}

static inline bool caretBrowsingEnabled(const LocalFrame* frame)
{
    Settings* settings = frame->settings();
    return settings && settings->caretBrowsingEnabled();
}

static inline bool hasCursorCaret(const FrameSelection& selection, const LayoutBlock* block, const LocalFrame* frame)
{
    return selection.caretLayoutObject() == block && (selection.hasEditableStyle() || caretBrowsingEnabled(frame));
}

static inline bool hasDragCaret(const DragCaretController& dragCaretController, const LayoutBlock* block, const LocalFrame* frame)
{
    return dragCaretController.caretLayoutObject() == block && (dragCaretController.isContentEditable() || caretBrowsingEnabled(frame));
}

void BlockPainter::paintCarets(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    LocalFrame* frame = m_layoutBlock.frame();

    FrameSelection& selection = frame->selection();
    if (hasCursorCaret(selection, &m_layoutBlock, frame))
        selection.paintCaret(paintInfo.context, paintOffset, LayoutRect(paintInfo.rect));

    DragCaretController& dragCaretController = frame->page()->dragCaretController();
    if (hasDragCaret(dragCaretController, &m_layoutBlock, frame))
        dragCaretController.paintDragCaret(frame, paintInfo.context, paintOffset, LayoutRect(paintInfo.rect));
}

LayoutRect BlockPainter::overflowRectForPaintRejection() const
{
    LayoutRect overflowRect = m_layoutBlock.visualOverflowRect();
    if (!m_layoutBlock.hasOverflowModel() || !m_layoutBlock.usesCompositedScrolling())
        return overflowRect;

    overflowRect.unite(m_layoutBlock.layoutOverflowRect());
    overflowRect.move(-m_layoutBlock.scrolledContentOffset());
    return overflowRect;
}

bool BlockPainter::hasCaret() const
{
    LocalFrame* frame = m_layoutBlock.frame();
    return hasCursorCaret(frame->selection(), &m_layoutBlock, frame)
        || hasDragCaret(frame->page()->dragCaretController(), &m_layoutBlock, frame);
}

void BlockPainter::paintContents(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    // Avoid painting descendants of the root element when stylesheets haven't loaded. This eliminates FOUC.
    // It's ok not to draw, because later on, when all the stylesheets do load, styleResolverChanged() on the Document
    // will do a full paint invalidation.
    if (m_layoutBlock.document().didLayoutWithPendingStylesheets() && !m_layoutBlock.isLayoutView())
        return;

    if (m_layoutBlock.childrenInline()) {
        LineBoxListPainter(*m_layoutBlock.lineBoxes()).paint(&m_layoutBlock, paintInfo, paintOffset);
    } else {
        PaintPhase newPhase = (paintInfo.phase == PaintPhaseChildOutlines) ? PaintPhaseOutline : paintInfo.phase;
        newPhase = (newPhase == PaintPhaseChildBlockBackgrounds) ? PaintPhaseChildBlockBackground : newPhase;

        // We don't paint our own background, but we do let the kids paint their backgrounds.
        PaintInfo paintInfoForChild(paintInfo);
        paintInfoForChild.phase = newPhase;
        paintInfoForChild.updatePaintingRootForChildren(&m_layoutBlock);
        m_layoutBlock.paintChildren(paintInfoForChild, paintOffset);
    }
}

void BlockPainter::paintContinuationOutlines(const PaintInfo& info, const LayoutPoint& paintOffset)
{
    LayoutInline* inlineCont = m_layoutBlock.inlineElementContinuation();
    if (inlineCont && inlineCont->style()->hasOutline() && inlineCont->style()->visibility() == VISIBLE) {
        LayoutInline* inlineLayoutObject = toLayoutInline(inlineCont->node()->layoutObject());
        LayoutBlock* cb = m_layoutBlock.containingBlock();

        bool inlineEnclosedInSelfPaintingLayer = false;
        for (LayoutBoxModelObject* box = inlineLayoutObject; box != cb; box = box->parent()->enclosingBoxModelObject()) {
            if (box->hasSelfPaintingLayer()) {
                inlineEnclosedInSelfPaintingLayer = true;
                break;
            }
        }

        // Do not add continuations for outline painting by our containing block if we are a relative positioned
        // anonymous block (i.e. have our own layer), paint them straightaway instead. This is because a block depends on layoutObjects in its continuation table being
        // in the same layer.
        if (!inlineEnclosedInSelfPaintingLayer && !m_layoutBlock.hasLayer()) {
            cb->addContinuationWithOutline(inlineLayoutObject);
        } else if (!inlineLayoutObject->firstLineBox() || (!inlineEnclosedInSelfPaintingLayer && m_layoutBlock.hasLayer())) {
            // The outline might be painted multiple times if multiple blocks have the same inline element continuation, and the inline has a self-painting layer.
            ScopeRecorder scopeRecorder(*info.context, *inlineLayoutObject);
            InlinePainter(*inlineLayoutObject).paintOutline(info, paintOffset - m_layoutBlock.locationOffset() + inlineLayoutObject->containingBlock()->location());
        }
    }

    ContinuationOutlineTableMap* table = continuationOutlineTable();
    if (table->isEmpty())
        return;

    OwnPtr<ListHashSet<LayoutInline*>> continuations = table->take(&m_layoutBlock);
    if (!continuations)
        return;

    LayoutPoint accumulatedPaintOffset = paintOffset;
    // Paint each continuation outline.
    ListHashSet<LayoutInline*>::iterator end = continuations->end();
    for (ListHashSet<LayoutInline*>::iterator it = continuations->begin(); it != end; ++it) {
        // Need to add in the coordinates of the intervening blocks.
        LayoutInline* flow = *it;
        LayoutBlock* block = flow->containingBlock();
        for ( ; block && block != &m_layoutBlock; block = block->containingBlock())
            accumulatedPaintOffset.moveBy(block->location());
        ASSERT(block);
        InlinePainter(*flow).paintOutline(info, accumulatedPaintOffset);
    }
}


} // namespace blink
