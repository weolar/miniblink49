/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
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
 *
 */

#include "config.h"
#include "core/layout/LayoutInline.h"

#include "core/dom/Fullscreen.h"
#include "core/dom/StyleEngine.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutBlock.h"
#include "core/layout/LayoutFlowThread.h"
#include "core/layout/LayoutFullScreen.h"
#include "core/layout/LayoutGeometryMap.h"
#include "core/layout/LayoutTheme.h"
#include "core/layout/LayoutView.h"
#include "core/layout/line/InlineTextBox.h"
#include "core/paint/BoxPainter.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/InlinePainter.h"
#include "core/paint/ObjectPainter.h"
#include "core/style/StyleInheritedData.h"
#include "platform/geometry/FloatQuad.h"
#include "platform/geometry/Region.h"
#include "platform/geometry/TransformState.h"

namespace blink {

struct SameSizeAsLayoutInline : public LayoutBoxModelObject {
    virtual ~SameSizeAsLayoutInline() { }
    LayoutObjectChildList m_children;
    LineBoxList m_lineBoxes;
};

static_assert(sizeof(LayoutInline) == sizeof(SameSizeAsLayoutInline), "LayoutInline should stay small");

LayoutInline::LayoutInline(Element* element)
    : LayoutBoxModelObject(element)
{
    setChildrenInline(true);
}

LayoutInline* LayoutInline::createAnonymous(Document* document)
{
    LayoutInline* layoutObject = new LayoutInline(nullptr);
    layoutObject->setDocumentForAnonymous(document);
    return layoutObject;
}

void LayoutInline::willBeDestroyed()
{
#if ENABLE(ASSERT)
    // Make sure we do not retain "this" in the continuation outline table map of our containing blocks.
    if (parent() && style()->visibility() == VISIBLE && style()->hasOutline()) {
        bool containingBlockPaintsContinuationOutline = continuation() || isInlineElementContinuation();
        if (containingBlockPaintsContinuationOutline) {
            if (LayoutBlock* cb = containingBlock()) {
                if (LayoutBlock* cbCb = cb->containingBlock())
                    ASSERT(!cbCb->paintsContinuationOutline(this));
            }
        }
    }
#endif

    // Make sure to destroy anonymous children first while they are still connected to the rest of the tree, so that they will
    // properly dirty line boxes that they are removed from.  Effects that do :before/:after only on hover could crash otherwise.
    children()->destroyLeftoverChildren();

    // Destroy our continuation before anything other than anonymous children.
    // The reason we don't destroy it before anonymous children is that they may
    // have continuations of their own that are anonymous children of our continuation.
    LayoutBoxModelObject* continuation = this->continuation();
    if (continuation) {
        continuation->destroy();
        setContinuation(nullptr);
    }

    if (!documentBeingDestroyed()) {
        if (firstLineBox()) {
            // We can't wait for LayoutBoxModelObject::destroy to clear the selection,
            // because by then we will have nuked the line boxes.
            // FIXME: The FrameSelection should be responsible for this when it
            // is notified of DOM mutations.
            if (isSelectionBorder())
                view()->clearSelection();

            // If line boxes are contained inside a root, that means we're an inline.
            // In that case, we need to remove all the line boxes so that the parent
            // lines aren't pointing to deleted children. If the first line box does
            // not have a parent that means they are either already disconnected or
            // root lines that can just be destroyed without disconnecting.
            if (firstLineBox()->parent()) {
                for (InlineFlowBox* box = firstLineBox(); box; box = box->nextLineBox())
                    box->remove();
            }
        } else if (parent()) {
            parent()->dirtyLinesFromChangedChild(this);
        }
    }

    m_lineBoxes.deleteLineBoxes();

    LayoutBoxModelObject::willBeDestroyed();
}

LayoutInline* LayoutInline::inlineElementContinuation() const
{
    LayoutBoxModelObject* continuation = this->continuation();
    if (!continuation || continuation->isInline())
        return toLayoutInline(continuation);
    return toLayoutBlock(continuation)->inlineElementContinuation();
}

void LayoutInline::updateFromStyle()
{
    LayoutBoxModelObject::updateFromStyle();

    // FIXME: Is this still needed. Was needed for run-ins, since run-in is considered a block display type.
    setInline(true);

    // FIXME: Support transforms and reflections on inline flows someday.
    setHasTransformRelatedProperty(false);
    setHasReflection(false);
}

static LayoutObject* inFlowPositionedInlineAncestor(LayoutObject* p)
{
    while (p && p->isLayoutInline()) {
        if (p->isRelPositioned())
            return p;
        p = p->parent();
    }
    return nullptr;
}

static void updateStyleOfAnonymousBlockContinuations(LayoutObject* block, const ComputedStyle& newStyle, const ComputedStyle& oldStyle, LayoutObject* containingBlockOfEndOfContinuation)
{
    // If an inline's outline or in-flow positioning has changed then any descendant blocks will need to change their styles accordingly.
    bool updateOutline = !newStyle.isOutlineEquivalent(&oldStyle);
    bool updatePosition = newStyle.position() != oldStyle.position() && (newStyle.hasInFlowPosition() || oldStyle.hasInFlowPosition());
    if (!updateOutline && !updatePosition)
        return;

    for (; block && block != containingBlockOfEndOfContinuation && block->isAnonymousBlock(); block = block->nextSibling()) {
        if (!toLayoutBlock(block)->isAnonymousBlockContinuation())
            continue;

        RefPtr<ComputedStyle> newBlockStyle = ComputedStyle::clone(block->styleRef());

        if (updateOutline)
            newBlockStyle->setOutlineFromStyle(newStyle);

        if (updatePosition) {
            // If we are no longer in-flow positioned but our descendant block(s) still have an in-flow positioned ancestor then
            // their containing anonymous block should keep its in-flow positioning.
            if (oldStyle.hasInFlowPosition() && inFlowPositionedInlineAncestor(toLayoutBlock(block)->inlineElementContinuation()))
                continue;
            newBlockStyle->setPosition(newStyle.position());
        }

        block->setStyle(newBlockStyle);
    }
}

void LayoutInline::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    LayoutBoxModelObject::styleDidChange(diff, oldStyle);

    // Ensure that all of the split inlines pick up the new style. We
    // only do this if we're an inline, since we don't want to propagate
    // a block's style to the other inlines.
    // e.g., <font>foo <h4>goo</h4> moo</font>.  The <font> inlines before
    // and after the block share the same style, but the block doesn't
    // need to pass its style on to anyone else.
    const ComputedStyle& newStyle = styleRef();
    LayoutInline* continuation = inlineElementContinuation();
    LayoutInline* endOfContinuation = nullptr;
    for (LayoutInline* currCont = continuation; currCont; currCont = currCont->inlineElementContinuation()) {
        LayoutBoxModelObject* nextCont = currCont->continuation();
        currCont->setContinuation(nullptr);
        currCont->setStyle(mutableStyle());
        currCont->setContinuation(nextCont);
        endOfContinuation = currCont;
    }

    if (continuation && oldStyle) {
        ASSERT(endOfContinuation);
        LayoutObject* block = containingBlock()->nextSibling();
        if (block && block->isAnonymousBlock())
            updateStyleOfAnonymousBlockContinuations(block, newStyle, *oldStyle, endOfContinuation->containingBlock());
    }

    if (!alwaysCreateLineBoxes()) {
        bool alwaysCreateLineBoxesNew = hasSelfPaintingLayer() || hasBoxDecorationBackground() || newStyle.hasPadding() || newStyle.hasMargin() || newStyle.hasOutline();
        if (oldStyle && alwaysCreateLineBoxesNew) {
            dirtyLineBoxes(false);
            setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::StyleChange);
        }
        setAlwaysCreateLineBoxes(alwaysCreateLineBoxesNew);
    }
}

void LayoutInline::updateAlwaysCreateLineBoxes(bool fullLayout)
{
    // Once we have been tainted once, just assume it will happen again. This way effects like hover highlighting that change the
    // background color will only cause a layout on the first rollover.
    if (alwaysCreateLineBoxes())
        return;

    const ComputedStyle& parentStyle = parent()->styleRef();
    LayoutInline* parentLayoutInline = parent()->isLayoutInline() ? toLayoutInline(parent()) : 0;
    bool checkFonts = document().inNoQuirksMode();
    bool alwaysCreateLineBoxesNew = (parentLayoutInline && parentLayoutInline->alwaysCreateLineBoxes())
        || (parentLayoutInline && parentStyle.verticalAlign() != BASELINE)
        || style()->verticalAlign() != BASELINE
        || style()->textEmphasisMark() != TextEmphasisMarkNone
        || (checkFonts && (!parentStyle.font().fontMetrics().hasIdenticalAscentDescentAndLineGap(style()->font().fontMetrics())
        || parentStyle.lineHeight() != style()->lineHeight()));

    if (!alwaysCreateLineBoxesNew && checkFonts && document().styleEngine().usesFirstLineRules()) {
        // Have to check the first line style as well.
        const ComputedStyle& firstLineParentStyle = parent()->styleRef(true);
        const ComputedStyle& childStyle = styleRef(true);
        alwaysCreateLineBoxesNew = !firstLineParentStyle.font().fontMetrics().hasIdenticalAscentDescentAndLineGap(childStyle.font().fontMetrics())
        || childStyle.verticalAlign() != BASELINE
        || firstLineParentStyle.lineHeight() != childStyle.lineHeight();
    }

    if (alwaysCreateLineBoxesNew) {
        if (!fullLayout)
            dirtyLineBoxes(false);
        setAlwaysCreateLineBoxes();
    }
}

LayoutRect LayoutInline::localCaretRect(InlineBox* inlineBox, int, LayoutUnit* extraWidthToEndOfLine)
{
    if (firstChild()) {
        // This condition is possible if the LayoutInline is at an editing boundary,
        // i.e. the VisiblePosition is:
        //   <LayoutInline editingBoundary=true>|<LayoutText> </LayoutText></LayoutInline>
        // FIXME: need to figure out how to make this return a valid rect, note that
        // there are no line boxes created in the above case.
        return LayoutRect();
    }

    ASSERT_UNUSED(inlineBox, !inlineBox);

    if (extraWidthToEndOfLine)
        *extraWidthToEndOfLine = 0;

    LayoutRect caretRect = localCaretRectForEmptyElement(borderAndPaddingWidth(), 0);

    if (InlineBox* firstBox = firstLineBox()) {
        // FIXME: the call to roundedLayoutPoint() below is temporary and should be removed once
        // the transition to LayoutUnit-based types is complete (crbug.com/321237)
        caretRect.moveBy(firstBox->topLeft());
    }

    return caretRect;
}

void LayoutInline::addChild(LayoutObject* newChild, LayoutObject* beforeChild)
{
    if (continuation())
        return addChildToContinuation(newChild, beforeChild);
    return addChildIgnoringContinuation(newChild, beforeChild);
}

static LayoutBoxModelObject* nextContinuation(LayoutObject* layoutObject)
{
    if (layoutObject->isInline() && !layoutObject->isReplaced())
        return toLayoutInline(layoutObject)->continuation();
    return toLayoutBlock(layoutObject)->inlineElementContinuation();
}

LayoutBoxModelObject* LayoutInline::continuationBefore(LayoutObject* beforeChild)
{
    if (beforeChild && beforeChild->parent() == this)
        return this;

    LayoutBoxModelObject* curr = nextContinuation(this);
    LayoutBoxModelObject* nextToLast = this;
    LayoutBoxModelObject* last = this;
    while (curr) {
        if (beforeChild && beforeChild->parent() == curr) {
            if (curr->slowFirstChild() == beforeChild)
                return last;
            return curr;
        }

        nextToLast = last;
        last = curr;
        curr = nextContinuation(curr);
    }

    if (!beforeChild && !last->slowFirstChild())
        return nextToLast;
    return last;
}

void LayoutInline::addChildIgnoringContinuation(LayoutObject* newChild, LayoutObject* beforeChild)
{
    // Make sure we don't append things after :after-generated content if we have it.
    if (!beforeChild && isAfterContent(lastChild()))
        beforeChild = lastChild();

    if (!newChild->isInline() && !newChild->isFloatingOrOutOfFlowPositioned()) {
        // We are placing a block inside an inline. We have to perform a split of this
        // inline into continuations.  This involves creating an anonymous block box to hold
        // |newChild|.  We then make that block box a continuation of this inline.  We take all of
        // the children after |beforeChild| and put them in a clone of this object.
        RefPtr<ComputedStyle> newStyle = ComputedStyle::createAnonymousStyleWithDisplay(styleRef(), BLOCK);

        // If inside an inline affected by in-flow positioning the block needs to be affected by it too.
        // Giving the block a layer like this allows it to collect the x/y offsets from inline parents later.
        if (LayoutObject* positionedAncestor = inFlowPositionedInlineAncestor(this))
            newStyle->setPosition(positionedAncestor->style()->position());

        // Push outline style to the block continuation.
        if (!newStyle->isOutlineEquivalent(style()))
            newStyle->setOutlineFromStyle(*style());

        LayoutBlockFlow* newBox = LayoutBlockFlow::createAnonymous(&document());
        newBox->setStyle(newStyle.release());
        LayoutBoxModelObject* oldContinuation = continuation();
        setContinuation(newBox);

        splitFlow(beforeChild, newBox, newChild, oldContinuation);
        return;
    }

    LayoutBoxModelObject::addChild(newChild, beforeChild);

    newChild->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::ChildChanged);
}

LayoutInline* LayoutInline::clone() const
{
    LayoutInline* cloneInline = new LayoutInline(node());
    cloneInline->setStyle(mutableStyle());
    cloneInline->setIsInsideFlowThread(isInsideFlowThread());
    return cloneInline;
}

void LayoutInline::moveChildrenToIgnoringContinuation(LayoutInline* to, LayoutObject* startChild)
{
    LayoutObject* child = startChild;
    while (child) {
        LayoutObject* currentChild = child;
        child = currentChild->nextSibling();
        to->addChildIgnoringContinuation(children()->removeChildNode(this, currentChild), nullptr);
    }
}

void LayoutInline::splitInlines(LayoutBlock* fromBlock, LayoutBlock* toBlock,
    LayoutBlock* middleBlock, LayoutObject* beforeChild, LayoutBoxModelObject* oldCont)
{
    ASSERT(isDescendantOf(fromBlock));

    // If we're splitting the inline containing the fullscreened element,
    // |beforeChild| may be the layoutObject for the fullscreened element. However,
    // that layoutObject is wrapped in a LayoutFullScreen, so |this| is not its
    // parent. Since the splitting logic expects |this| to be the parent, set
    // |beforeChild| to be the LayoutFullScreen.
    if (Fullscreen* fullscreen = Fullscreen::fromIfExists(document())) {
        const Element* fullScreenElement = fullscreen->webkitCurrentFullScreenElement();
        if (fullScreenElement && beforeChild && beforeChild->node() == fullScreenElement)
            beforeChild = fullscreen->fullScreenLayoutObject();
    }

    // FIXME: Because splitting is O(n^2) as tags nest pathologically, we cap the depth at which we're willing to clone.
    // There will eventually be a better approach to this problem that will let us nest to a much
    // greater depth (see bugzilla bug 13430) but for now we have a limit.  This *will* result in
    // incorrect rendering, but the alternative is to hang forever.
    const unsigned cMaxSplitDepth = 200;
    Vector<LayoutInline*> inlinesToClone;
    LayoutInline* topMostInline = this;
    for (LayoutObject* o = this; o != fromBlock; o = o->parent()) {
        topMostInline = toLayoutInline(o);
        if (inlinesToClone.size() < cMaxSplitDepth)
            inlinesToClone.append(topMostInline);
        // Keep walking up the chain to ensure |topMostInline| is a child of |fromBlock|,
        // to avoid assertion failure when |fromBlock|'s children are moved to |toBlock| below.
    }

    // Create a new clone of the top-most inline in |inlinesToClone|.
    LayoutInline* topMostInlineToClone = inlinesToClone.last();
    LayoutInline* cloneInline = topMostInlineToClone->clone();

    // Now we are at the block level. We need to put the clone into the |toBlock|.
    toBlock->children()->appendChildNode(toBlock, cloneInline);

    // Now take all the children after |topMostInline| and remove them from the |fromBlock|
    // and put them into the toBlock.
    fromBlock->moveChildrenTo(toBlock, topMostInline->nextSibling(), nullptr, true);

    LayoutInline* currentParent = topMostInlineToClone;
    LayoutInline* cloneInlineParent = cloneInline;

    // Clone the inlines from top to down to ensure any new object will be added into a rooted tree.
    // Note that we have already cloned the top-most one, so the loop begins from size - 2 (except if
    // we have reached |cMaxDepth| in which case we sacrifice correct rendering for performance).
    for (int i = static_cast<int>(inlinesToClone.size()) - 2; i >= 0; --i) {
        // Hook the clone up as a continuation of |currentInline|.
        LayoutBoxModelObject* oldCont = currentParent->continuation();
        currentParent->setContinuation(cloneInline);
        cloneInline->setContinuation(oldCont);

        // Create a new clone.
        LayoutInline* current = inlinesToClone[i];
        cloneInline = current->clone();

        // Insert our |cloneInline| as the first child of |cloneInlineParent|.
        cloneInlineParent->addChildIgnoringContinuation(cloneInline, nullptr);

        // Now we need to take all of the children starting from the first child
        // *after* |current| and append them all to the |cloneInlineParent|.
        currentParent->moveChildrenToIgnoringContinuation(cloneInlineParent, current->nextSibling());

        currentParent = current;
        cloneInlineParent = cloneInline;
    }

    // The last inline to clone is |this|, and the current |cloneInline| is cloned from |this|.
    ASSERT(this == inlinesToClone.first());

    // Hook |cloneInline| up as the continuation of the middle block.
    cloneInline->setContinuation(oldCont);
    middleBlock->setContinuation(cloneInline);

    // Now take all of the children from |beforeChild| to the end and remove
    // them from |this| and place them in the clone.
    moveChildrenToIgnoringContinuation(cloneInline, beforeChild);
}

void LayoutInline::splitFlow(LayoutObject* beforeChild, LayoutBlock* newBlockBox,
    LayoutObject* newChild, LayoutBoxModelObject* oldCont)
{
    LayoutBlock* pre = nullptr;
    LayoutBlock* block = containingBlock();

    // Delete our line boxes before we do the inline split into continuations.
    block->deleteLineBoxTree();

    bool madeNewBeforeBlock = false;
    if (block->isAnonymousBlock() && (!block->parent() || !block->parent()->createsAnonymousWrapper())) {
        // We can reuse this block and make it the preBlock of the next continuation.
        pre = block;
        pre->removePositionedObjects(nullptr);
        if (pre->isLayoutBlockFlow())
            toLayoutBlockFlow(pre)->removeFloatingObjects();
        block = block->containingBlock();
    } else {
        // No anonymous block available for use.  Make one.
        pre = block->createAnonymousBlock();
        madeNewBeforeBlock = true;
    }

    LayoutBlock* post = toLayoutBlock(pre->createAnonymousBoxWithSameTypeAs(block));

    LayoutObject* boxFirst = madeNewBeforeBlock ? block->firstChild() : pre->nextSibling();
    if (madeNewBeforeBlock)
        block->children()->insertChildNode(block, pre, boxFirst);
    block->children()->insertChildNode(block, newBlockBox, boxFirst);
    block->children()->insertChildNode(block, post, boxFirst);
    block->setChildrenInline(false);

    if (madeNewBeforeBlock) {
        LayoutObject* o = boxFirst;
        while (o) {
            LayoutObject* no = o;
            o = no->nextSibling();
            pre->children()->appendChildNode(pre, block->children()->removeChildNode(block, no));
            no->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::AnonymousBlockChange);
        }
    }

    splitInlines(pre, post, newBlockBox, beforeChild, oldCont);

    // We already know the newBlockBox isn't going to contain inline kids, so avoid wasting
    // time in makeChildrenNonInline by just setting this explicitly up front.
    newBlockBox->setChildrenInline(false);

    newBlockBox->addChild(newChild);

    // Always just do a full layout in order to ensure that line boxes (especially wrappers for images)
    // get deleted properly.  Because objects moves from the pre block into the post block, we want to
    // make new line boxes instead of leaving the old line boxes around.
    pre->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::AnonymousBlockChange);
    block->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::AnonymousBlockChange);
    post->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::AnonymousBlockChange);
}

void LayoutInline::addChildToContinuation(LayoutObject* newChild, LayoutObject* beforeChild)
{
    LayoutBoxModelObject* flow = continuationBefore(beforeChild);
    ASSERT(!beforeChild || beforeChild->parent()->isLayoutBlock() || beforeChild->parent()->isLayoutInline());
    LayoutBoxModelObject* beforeChildParent = nullptr;
    if (beforeChild) {
        beforeChildParent = toLayoutBoxModelObject(beforeChild->parent());
    } else {
        LayoutBoxModelObject* cont = nextContinuation(flow);
        if (cont)
            beforeChildParent = cont;
        else
            beforeChildParent = flow;
    }

    if (newChild->isFloatingOrOutOfFlowPositioned())
        return beforeChildParent->addChildIgnoringContinuation(newChild, beforeChild);

    // A continuation always consists of two potential candidates: an inline or an anonymous
    // block box holding block children.
    bool childInline = newChild->isInline();
    bool bcpInline = beforeChildParent->isInline();
    bool flowInline = flow->isInline();

    if (flow == beforeChildParent)
        return flow->addChildIgnoringContinuation(newChild, beforeChild);

    // The goal here is to match up if we can, so that we can coalesce and create the
    // minimal # of continuations needed for the inline.
    if (childInline == bcpInline || (beforeChild && beforeChild->isInline()))
        return beforeChildParent->addChildIgnoringContinuation(newChild, beforeChild);
    if (flowInline == childInline)
        return flow->addChildIgnoringContinuation(newChild, 0); // Just treat like an append.
    return beforeChildParent->addChildIgnoringContinuation(newChild, beforeChild);
}

void LayoutInline::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    InlinePainter(*this).paint(paintInfo, paintOffset);
}

template<typename GeneratorContext>
void LayoutInline::generateLineBoxRects(GeneratorContext& yield) const
{
    if (!alwaysCreateLineBoxes()) {
        generateCulledLineBoxRects(yield, this);
    } else if (InlineFlowBox* curr = firstLineBox()) {
        for (; curr; curr = curr->nextLineBox())
            yield(LayoutRect(curr->topLeft(), curr->size()));
    } else {
        yield(LayoutRect());
    }
}

template<typename GeneratorContext>
void LayoutInline::generateCulledLineBoxRects(GeneratorContext& yield, const LayoutInline* container) const
{
    if (!culledInlineFirstLineBox()) {
        yield(LayoutRect());
        return;
    }

    bool isHorizontal = style()->isHorizontalWritingMode();

    for (LayoutObject* curr = firstChild(); curr; curr = curr->nextSibling()) {
        if (curr->isFloatingOrOutOfFlowPositioned())
            continue;

        // We want to get the margin box in the inline direction, and then use our font ascent/descent in the block
        // direction (aligned to the root box's baseline).
        if (curr->isBox()) {
            LayoutBox* currBox = toLayoutBox(curr);
            if (currBox->inlineBoxWrapper()) {
                RootInlineBox& rootBox = currBox->inlineBoxWrapper()->root();
                int logicalTop = rootBox.logicalTop() + (rootBox.layoutObject().style(rootBox.isFirstLineStyle())->font().fontMetrics().ascent() - container->style(rootBox.isFirstLineStyle())->font().fontMetrics().ascent());
                int logicalHeight = container->style(rootBox.isFirstLineStyle())->font().fontMetrics().height();
                if (isHorizontal)
                    yield(LayoutRect(currBox->inlineBoxWrapper()->x() - currBox->marginLeft(), logicalTop, currBox->size().width() + currBox->marginWidth(), logicalHeight));
                else
                    yield(LayoutRect(logicalTop, currBox->inlineBoxWrapper()->y() - currBox->marginTop(), logicalHeight, currBox->size().height() + currBox->marginHeight()));
            }
        } else if (curr->isLayoutInline()) {
            // If the child doesn't need line boxes either, then we can recur.
            LayoutInline* currInline = toLayoutInline(curr);
            if (!currInline->alwaysCreateLineBoxes()) {
                currInline->generateCulledLineBoxRects(yield, container);
            } else {
                for (InlineFlowBox* childLine = currInline->firstLineBox(); childLine; childLine = childLine->nextLineBox()) {
                    RootInlineBox& rootBox = childLine->root();
                    int logicalTop = rootBox.logicalTop() + (rootBox.layoutObject().style(rootBox.isFirstLineStyle())->font().fontMetrics().ascent() - container->style(rootBox.isFirstLineStyle())->font().fontMetrics().ascent());
                    int logicalHeight = container->style(rootBox.isFirstLineStyle())->font().fontMetrics().height();
                    if (isHorizontal) {
                        yield(LayoutRect(childLine->x() - childLine->marginLogicalLeft(),
                            logicalTop,
                            childLine->logicalWidth() + childLine->marginLogicalLeft() + childLine->marginLogicalRight(),
                            logicalHeight));
                    } else {
                        yield(LayoutRect(logicalTop,
                            childLine->y() - childLine->marginLogicalLeft(),
                            logicalHeight,
                            childLine->logicalWidth() + childLine->marginLogicalLeft() + childLine->marginLogicalRight()));
                    }
                }
            }
        } else if (curr->isText()) {
            LayoutText* currText = toLayoutText(curr);
            for (InlineTextBox* childText = currText->firstTextBox(); childText; childText = childText->nextTextBox()) {
                RootInlineBox& rootBox = childText->root();
                int logicalTop = rootBox.logicalTop() + (rootBox.layoutObject().style(rootBox.isFirstLineStyle())->font().fontMetrics().ascent() - container->style(rootBox.isFirstLineStyle())->font().fontMetrics().ascent());
                int logicalHeight = container->style(rootBox.isFirstLineStyle())->font().fontMetrics().height();
                if (isHorizontal)
                    yield(LayoutRect(childText->x(), logicalTop, childText->logicalWidth(), logicalHeight));
                else
                    yield(LayoutRect(logicalTop, childText->y(), logicalHeight, childText->logicalWidth()));
            }
        }
    }
}

namespace {

class AbsoluteRectsGeneratorContext {
public:
    AbsoluteRectsGeneratorContext(Vector<IntRect>& rects, const LayoutPoint& accumulatedOffset)
        : m_rects(rects)
        , m_accumulatedOffset(accumulatedOffset) { }

    void operator()(const LayoutRect& rect)
    {
        IntRect intRect = enclosingIntRect(rect);
        intRect.move(m_accumulatedOffset.x(), m_accumulatedOffset.y());
        m_rects.append(intRect);
    }
private:
    Vector<IntRect>& m_rects;
    const LayoutPoint& m_accumulatedOffset;
};

} // unnamed namespace

void LayoutInline::absoluteRects(Vector<IntRect>& rects, const LayoutPoint& accumulatedOffset) const
{
    AbsoluteRectsGeneratorContext context(rects, accumulatedOffset);
    generateLineBoxRects(context);

    if (const LayoutBoxModelObject* continuation = this->continuation()) {
        if (continuation->isBox()) {
            const LayoutBox* box = toLayoutBox(continuation);
            continuation->absoluteRects(rects, toLayoutPoint(accumulatedOffset - containingBlock()->location() + box->locationOffset()));
        } else {
            continuation->absoluteRects(rects, toLayoutPoint(accumulatedOffset - containingBlock()->location()));
        }
    }
}


namespace {

class AbsoluteQuadsGeneratorContext {
public:
    AbsoluteQuadsGeneratorContext(const LayoutInline* layoutObject, Vector<FloatQuad>& quads)
        : m_quads(quads)
        , m_geometryMap()
    {
        m_geometryMap.pushMappingsToAncestor(layoutObject, 0);
    }

    void operator()(const FloatRect& rect)
    {
        m_quads.append(m_geometryMap.absoluteRect(rect));
    }
private:
    Vector<FloatQuad>& m_quads;
    LayoutGeometryMap m_geometryMap;
};

} // unnamed namespace

void LayoutInline::absoluteQuads(Vector<FloatQuad>& quads, bool* wasFixed) const
{
    AbsoluteQuadsGeneratorContext context(this, quads);
    generateLineBoxRects(context);

    if (const LayoutBoxModelObject* continuation = this->continuation())
        continuation->absoluteQuads(quads, wasFixed);
}

LayoutUnit LayoutInline::offsetLeft() const
{
    LayoutPoint topLeft;
    if (InlineBox* firstBox = firstLineBoxIncludingCulling()) {
        topLeft = firstBox->topLeft();
    }
    return adjustedPositionRelativeToOffsetParent(topLeft).x();
}

LayoutUnit LayoutInline::offsetTop() const
{
    LayoutPoint topLeft;
    if (InlineBox* firstBox = firstLineBoxIncludingCulling()) {
        topLeft = firstBox->topLeft();
    }
    return adjustedPositionRelativeToOffsetParent(topLeft).y();
}

static LayoutUnit computeMargin(const LayoutInline* layoutObject, const Length& margin)
{
    if (margin.isFixed())
        return margin.value();
    if (margin.hasPercent())
        return minimumValueForLength(margin, std::max(LayoutUnit(), layoutObject->containingBlock()->availableLogicalWidth()));
    return LayoutUnit();
}

LayoutRectOutsets LayoutInline::marginBoxOutsets() const
{
    return LayoutRectOutsets(marginTop(), marginRight(), marginBottom(), marginLeft());
}

LayoutUnit LayoutInline::marginLeft() const
{
    return computeMargin(this, style()->marginLeft());
}

LayoutUnit LayoutInline::marginRight() const
{
    return computeMargin(this, style()->marginRight());
}

LayoutUnit LayoutInline::marginTop() const
{
    return computeMargin(this, style()->marginTop());
}

LayoutUnit LayoutInline::marginBottom() const
{
    return computeMargin(this, style()->marginBottom());
}

LayoutUnit LayoutInline::marginStart(const ComputedStyle* otherStyle) const
{
    return computeMargin(this, style()->marginStartUsing(otherStyle ? otherStyle : style()));
}

LayoutUnit LayoutInline::marginEnd(const ComputedStyle* otherStyle) const
{
    return computeMargin(this, style()->marginEndUsing(otherStyle ? otherStyle : style()));
}

LayoutUnit LayoutInline::marginBefore(const ComputedStyle* otherStyle) const
{
    return computeMargin(this, style()->marginBeforeUsing(otherStyle ? otherStyle : style()));
}

LayoutUnit LayoutInline::marginAfter(const ComputedStyle* otherStyle) const
{
    return computeMargin(this, style()->marginAfterUsing(otherStyle ? otherStyle : style()));
}

bool LayoutInline::nodeAtPoint(HitTestResult& result,
    const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction hitTestAction)
{
    return m_lineBoxes.hitTest(this, result, locationInContainer, accumulatedOffset, hitTestAction);
}

namespace {

class HitTestCulledInlinesGeneratorContext {
public:
    HitTestCulledInlinesGeneratorContext(Region& region, const HitTestLocation& location) : m_intersected(false), m_region(region), m_location(location) { }
    void operator()(const FloatRect& rect)
    {
        m_intersected = m_intersected || m_location.intersects(rect);
        m_region.unite(enclosingIntRect(rect));
    }
    bool intersected() const { return m_intersected; }
private:
    bool m_intersected;
    Region& m_region;
    const HitTestLocation& m_location;
};

} // unnamed namespace

bool LayoutInline::hitTestCulledInline(HitTestResult& result, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset)
{
    ASSERT(!alwaysCreateLineBoxes());
    if (!visibleToHitTestRequest(result.hitTestRequest()))
        return false;

    HitTestLocation tmpLocation(locationInContainer, -toLayoutSize(accumulatedOffset));

    Region regionResult;
    HitTestCulledInlinesGeneratorContext context(regionResult, tmpLocation);
    generateCulledLineBoxRects(context, this);

    if (context.intersected()) {
        updateHitTestResult(result, tmpLocation.point());
        // We can not use addNodeToListBasedTestResult to determine if we fully enclose the hit-test area
        // because it can only handle rectangular targets.
        result.addNodeToListBasedTestResult(node(), locationInContainer);
        return regionResult.contains(tmpLocation.boundingBox());
    }
    return false;
}

PositionWithAffinity LayoutInline::positionForPoint(const LayoutPoint& point)
{
    // FIXME: Does not deal with relative positioned inlines (should it?)
    LayoutBlock* cb = containingBlock();
    if (firstLineBoxIncludingCulling()) {
        // This inline actually has a line box.  We must have clicked in the border/padding of one of these boxes.  We
        // should try to find a result by asking our containing block.
        return cb->positionForPoint(point);
    }

    // Translate the coords from the pre-anonymous block to the post-anonymous block.
    LayoutPoint parentBlockPoint = cb->location() + point;
    LayoutBoxModelObject* c = continuation();
    while (c) {
        LayoutBox* contBlock = c->isInline() ? c->containingBlock() : toLayoutBlock(c);
        if (c->isInline() || c->slowFirstChild())
            return c->positionForPoint(parentBlockPoint - contBlock->locationOffset());
        c = toLayoutBlock(c)->inlineElementContinuation();
    }

    return LayoutBoxModelObject::positionForPoint(point);
}

namespace {

class LinesBoundingBoxGeneratorContext {
public:
    LinesBoundingBoxGeneratorContext(FloatRect& rect) : m_rect(rect) { }
    void operator()(const FloatRect& rect)
    {
        m_rect.uniteIfNonZero(rect);
    }
private:
    FloatRect& m_rect;
};

} // unnamed namespace

IntRect LayoutInline::linesBoundingBox() const
{
    if (!alwaysCreateLineBoxes()) {
        ASSERT(!firstLineBox());
        FloatRect floatResult;
        LinesBoundingBoxGeneratorContext context(floatResult);
        generateCulledLineBoxRects(context, this);
        return enclosingIntRect(floatResult);
    }

    IntRect result;

    // See <rdar://problem/5289721>, for an unknown reason the linked list here is sometimes inconsistent, first is non-zero and last is zero.  We have been
    // unable to reproduce this at all (and consequently unable to figure ot why this is happening).  The assert will hopefully catch the problem in debug
    // builds and help us someday figure out why.  We also put in a redundant check of lastLineBox() to avoid the crash for now.
    ASSERT(!firstLineBox() == !lastLineBox()); // Either both are null or both exist.
    if (firstLineBox() && lastLineBox()) {
        // Return the width of the minimal left side and the maximal right side.
        LayoutUnit logicalLeftSide = 0;
        LayoutUnit logicalRightSide = 0;
        for (InlineFlowBox* curr = firstLineBox(); curr; curr = curr->nextLineBox()) {
            if (curr == firstLineBox() || curr->logicalLeft() < logicalLeftSide)
                logicalLeftSide = curr->logicalLeft();
            if (curr == firstLineBox() || curr->logicalRight() > logicalRightSide)
                logicalRightSide = curr->logicalRight();
        }

        bool isHorizontal = style()->isHorizontalWritingMode();

        LayoutUnit x = isHorizontal ? logicalLeftSide : firstLineBox()->x();
        LayoutUnit y = isHorizontal ? firstLineBox()->y() : logicalLeftSide;
        LayoutUnit width = isHorizontal ? logicalRightSide - logicalLeftSide : lastLineBox()->logicalBottom() - x;
        LayoutUnit height = isHorizontal ? lastLineBox()->logicalBottom() - y : logicalRightSide - logicalLeftSide;
        result = enclosingIntRect(LayoutRect(x, y, width, height));
    }

    return result;
}

InlineBox* LayoutInline::culledInlineFirstLineBox() const
{
    for (LayoutObject* curr = firstChild(); curr; curr = curr->nextSibling()) {
        if (curr->isFloatingOrOutOfFlowPositioned())
            continue;

        // We want to get the margin box in the inline direction, and then use our font ascent/descent in the block
        // direction (aligned to the root box's baseline).
        if (curr->isBox())
            return toLayoutBox(curr)->inlineBoxWrapper();
        if (curr->isLayoutInline()) {
            LayoutInline* currInline = toLayoutInline(curr);
            InlineBox* result = currInline->firstLineBoxIncludingCulling();
            if (result)
                return result;
        } else if (curr->isText()) {
            LayoutText* currText = toLayoutText(curr);
            if (currText->firstTextBox())
                return currText->firstTextBox();
        }
    }
    return nullptr;
}

InlineBox* LayoutInline::culledInlineLastLineBox() const
{
    for (LayoutObject* curr = lastChild(); curr; curr = curr->previousSibling()) {
        if (curr->isFloatingOrOutOfFlowPositioned())
            continue;

        // We want to get the margin box in the inline direction, and then use our font ascent/descent in the block
        // direction (aligned to the root box's baseline).
        if (curr->isBox())
            return toLayoutBox(curr)->inlineBoxWrapper();
        if (curr->isLayoutInline()) {
            LayoutInline* currInline = toLayoutInline(curr);
            InlineBox* result = currInline->lastLineBoxIncludingCulling();
            if (result)
                return result;
        } else if (curr->isText()) {
            LayoutText* currText = toLayoutText(curr);
            if (currText->lastTextBox())
                return currText->lastTextBox();
        }
    }
    return nullptr;
}

LayoutRect LayoutInline::culledInlineVisualOverflowBoundingBox() const
{
    FloatRect floatResult;
    LinesBoundingBoxGeneratorContext context(floatResult);
    generateCulledLineBoxRects(context, this);
    LayoutRect result(enclosingLayoutRect(floatResult));
    bool isHorizontal = style()->isHorizontalWritingMode();
    for (LayoutObject* curr = firstChild(); curr; curr = curr->nextSibling()) {
        if (curr->isFloatingOrOutOfFlowPositioned())
            continue;

        // For overflow we just have to propagate by hand and recompute it all.
        if (curr->isBox()) {
            LayoutBox* currBox = toLayoutBox(curr);
            if (!currBox->hasSelfPaintingLayer() && currBox->inlineBoxWrapper()) {
                LayoutRect logicalRect = currBox->logicalVisualOverflowRectForPropagation(styleRef());
                if (isHorizontal) {
                    logicalRect.moveBy(currBox->location());
                    result.uniteIfNonZero(logicalRect);
                } else {
                    logicalRect.moveBy(currBox->location());
                    result.uniteIfNonZero(logicalRect.transposedRect());
                }
            }
        } else if (curr->isLayoutInline()) {
            // If the child doesn't need line boxes either, then we can recur.
            LayoutInline* currInline = toLayoutInline(curr);
            if (!currInline->alwaysCreateLineBoxes())
                result.uniteIfNonZero(currInline->culledInlineVisualOverflowBoundingBox());
            else if (!currInline->hasSelfPaintingLayer())
                result.uniteIfNonZero(currInline->linesVisualOverflowBoundingBox());
        } else if (curr->isText()) {
            // FIXME; Overflow from text boxes is lost. We will need to cache this information in
            // InlineTextBoxes.
            LayoutText* currText = toLayoutText(curr);
            result.uniteIfNonZero(currText->linesVisualOverflowBoundingBox());
        }
    }
    return result;
}

LayoutRect LayoutInline::linesVisualOverflowBoundingBox() const
{
    if (!alwaysCreateLineBoxes())
        return culledInlineVisualOverflowBoundingBox();

    if (!firstLineBox() || !lastLineBox())
        return LayoutRect();

    // Return the width of the minimal left side and the maximal right side.
    LayoutUnit logicalLeftSide = LayoutUnit::max();
    LayoutUnit logicalRightSide = LayoutUnit::min();
    for (InlineFlowBox* curr = firstLineBox(); curr; curr = curr->nextLineBox()) {
        logicalLeftSide = std::min(logicalLeftSide, curr->logicalLeftVisualOverflow());
        logicalRightSide = std::max(logicalRightSide, curr->logicalRightVisualOverflow());
    }

    RootInlineBox& firstRootBox = firstLineBox()->root();
    RootInlineBox& lastRootBox = lastLineBox()->root();

    LayoutUnit logicalTop = firstLineBox()->logicalTopVisualOverflow(firstRootBox.lineTop());
    LayoutUnit logicalWidth = logicalRightSide - logicalLeftSide;
    LayoutUnit logicalHeight = lastLineBox()->logicalBottomVisualOverflow(lastRootBox.lineBottom()) - logicalTop;

    LayoutRect rect(logicalLeftSide, logicalTop, logicalWidth, logicalHeight);
    if (!style()->isHorizontalWritingMode())
        rect = rect.transposedRect();
    return rect;
}

LayoutRect LayoutInline::absoluteClippedOverflowRect() const
{
    if (!continuation())
        return clippedOverflowRect(view());

    FloatRect floatResult;
    LinesBoundingBoxGeneratorContext context(floatResult);

    LayoutInline* endContinuation = inlineElementContinuation();
    while (LayoutInline* nextContinuation = endContinuation->inlineElementContinuation())
        endContinuation = nextContinuation;

    for (LayoutBlock* currBlock = containingBlock(); currBlock && currBlock->isAnonymousBlock(); currBlock = toLayoutBlock(currBlock->nextSibling())) {
        for (LayoutObject* curr = currBlock->firstChild(); curr; curr = curr->nextSibling()) {
            LayoutRect rect = curr->clippedOverflowRectForPaintInvalidation(view());
            context(rect);
            if (curr == endContinuation)
                return LayoutRect(enclosingIntRect(floatResult));
        }
    }
    return LayoutRect();
}

LayoutRect LayoutInline::clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* paintInvalidationState) const
{
    // If we don't create line boxes, we don't have any invalidations to do.
    if (!alwaysCreateLineBoxes())
        return LayoutRect();
    return clippedOverflowRect(paintInvalidationContainer);
}

LayoutRect LayoutInline::clippedOverflowRect(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* paintInvalidationState) const
{
    const LayoutBoxModelObject* continuation = this->continuation();
    if ((!firstLineBoxIncludingCulling() && !continuation) || style()->visibility() != VISIBLE)
        return LayoutRect();

    LayoutRect overflowRect(linesVisualOverflowBoundingBox());
    mapRectToPaintInvalidationBacking(paintInvalidationContainer, overflowRect, paintInvalidationState);

    LayoutUnit outlineSize = style()->outlineSize();
    if (outlineSize) {
        for (LayoutObject* curr = firstChild(); curr; curr = curr->nextSibling()) {
            if (!curr->isText())
                overflowRect.unite(curr->rectWithOutlineForPaintInvalidation(paintInvalidationContainer, outlineSize));
        }

        if (continuation && !continuation->isInline() && continuation->parent())
            overflowRect.unite(continuation->rectWithOutlineForPaintInvalidation(paintInvalidationContainer, outlineSize));
    }

    return overflowRect;
}

LayoutRect LayoutInline::rectWithOutlineForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, LayoutUnit outlineWidth, const PaintInvalidationState* paintInvalidationState) const
{
    LayoutRect r(LayoutBoxModelObject::rectWithOutlineForPaintInvalidation(paintInvalidationContainer, outlineWidth, paintInvalidationState));
    for (LayoutObject* curr = firstChild(); curr; curr = curr->nextSibling()) {
        if (!curr->isText())
            r.unite(curr->rectWithOutlineForPaintInvalidation(paintInvalidationContainer, outlineWidth, paintInvalidationState));
    }
    return r;
}

void LayoutInline::mapRectToPaintInvalidationBacking(const LayoutBoxModelObject* paintInvalidationContainer, LayoutRect& rect, const PaintInvalidationState* paintInvalidationState) const
{
    if (paintInvalidationState && paintInvalidationState->canMapToContainer(paintInvalidationContainer)) {
        if (style()->hasInFlowPosition() && layer())
            rect.move(layer()->offsetForInFlowPosition());
        rect.move(paintInvalidationState->paintOffset());
        if (paintInvalidationState->isClipped())
            rect.intersect(paintInvalidationState->clipRect());
        return;
    }

    if (paintInvalidationContainer == this)
        return;

    bool containerSkipped;
    LayoutObject* o = container(paintInvalidationContainer, &containerSkipped);
    if (!o)
        return;

    LayoutPoint topLeft = rect.location();

    if (style()->hasInFlowPosition() && layer()) {
        // Apply the in-flow position offset when invalidating a rectangle. The layer
        // is translated, but the layout box isn't, so we need to do this to get the
        // right dirty rect. Since this is called from LayoutObject::setStyle, the relative position
        // flag on the LayoutObject has been cleared, so use the one on the style().
        topLeft += layer()->offsetForInFlowPosition();
    }

    // FIXME: We ignore the lightweight clipping rect that controls use, since if |o| is in mid-layout,
    // its controlClipRect will be wrong. For overflow clip we use the values cached by the layer.
    rect.setLocation(topLeft);
    if (o->hasOverflowClip()) {
        LayoutBox* containerBox = toLayoutBox(o);
        containerBox->applyCachedClipAndScrollOffsetForPaintInvalidation(rect);
        if (rect.isEmpty())
            return;
    }

    if (containerSkipped) {
        // If the paintInvalidationContainer is below o, then we need to map the rect into paintInvalidationContainer's coordinates.
        LayoutSize containerOffset = paintInvalidationContainer->offsetFromAncestorContainer(o);
        rect.move(-containerOffset);
        return;
    }

    o->mapRectToPaintInvalidationBacking(paintInvalidationContainer, rect, paintInvalidationState);
}

LayoutSize LayoutInline::offsetFromContainer(const LayoutObject* container, const LayoutPoint& point, bool* offsetDependsOnPoint) const
{
    ASSERT(container == this->container());

    LayoutSize offset;
    if (isRelPositioned())
        offset += offsetForInFlowPosition();

    offset += container->columnOffset(point);

    if (container->hasOverflowClip())
        offset -= toLayoutBox(container)->scrolledContentOffset();

    if (offsetDependsOnPoint)
        *offsetDependsOnPoint = (container->isBox() && container->style()->isFlippedBlocksWritingMode()) || container->isLayoutFlowThread();

    return offset;
}

void LayoutInline::mapLocalToContainer(const LayoutBoxModelObject* paintInvalidationContainer, TransformState& transformState, MapCoordinatesFlags mode, bool* wasFixed, const PaintInvalidationState* paintInvalidationState) const
{
    if (paintInvalidationContainer == this)
        return;

    if (paintInvalidationState && paintInvalidationState->canMapToContainer(paintInvalidationContainer)) {
        LayoutSize offset = paintInvalidationState->paintOffset();
        if (style()->hasInFlowPosition() && layer())
            offset += layer()->offsetForInFlowPosition();
        transformState.move(offset);
        return;
    }

    bool containerSkipped;
    LayoutObject* o = container(paintInvalidationContainer, &containerSkipped);
    if (!o)
        return;

    if (mode & ApplyContainerFlip && o->isBox()) {
        if (o->style()->isFlippedBlocksWritingMode()) {
            IntPoint centerPoint = roundedIntPoint(transformState.mappedPoint());
            transformState.move(toLayoutBox(o)->flipForWritingMode(LayoutPoint(centerPoint)) - centerPoint);
        }
        mode &= ~ApplyContainerFlip;
    }

    LayoutSize containerOffset = offsetFromContainer(o, roundedLayoutPoint(transformState.mappedPoint()));

    bool preserve3D = mode & UseTransforms && (o->style()->preserves3D() || style()->preserves3D());
    if (mode & UseTransforms && shouldUseTransformFromContainer(o)) {
        TransformationMatrix t;
        getTransformFromContainer(o, containerOffset, t);
        transformState.applyTransform(t, preserve3D ? TransformState::AccumulateTransform : TransformState::FlattenTransform);
    } else {
        transformState.move(containerOffset.width(), containerOffset.height(), preserve3D ? TransformState::AccumulateTransform : TransformState::FlattenTransform);
    }

    if (containerSkipped) {
        // There can't be a transform between paintInvalidationContainer and o, because transforms create containers, so it should be safe
        // to just subtract the delta between the paintInvalidationContainer and o.
        LayoutSize containerOffset = paintInvalidationContainer->offsetFromAncestorContainer(o);
        transformState.move(-containerOffset.width(), -containerOffset.height(), preserve3D ? TransformState::AccumulateTransform : TransformState::FlattenTransform);
        return;
    }

    o->mapLocalToContainer(paintInvalidationContainer, transformState, mode, wasFixed, paintInvalidationState);
}

void LayoutInline::updateDragState(bool dragOn)
{
    LayoutBoxModelObject::updateDragState(dragOn);
    if (LayoutBoxModelObject* continuation = this->continuation())
        continuation->updateDragState(dragOn);
}

void LayoutInline::childBecameNonInline(LayoutObject* child)
{
    // We have to split the parent flow.
    LayoutBlock* newBox = containingBlock()->createAnonymousBlock();
    LayoutBoxModelObject* oldContinuation = continuation();
    setContinuation(newBox);
    LayoutObject* beforeChild = child->nextSibling();
    children()->removeChildNode(this, child);
    splitFlow(beforeChild, newBox, child, oldContinuation);
}

void LayoutInline::updateHitTestResult(HitTestResult& result, const LayoutPoint& point)
{
    if (result.innerNode())
        return;

    Node* n = node();
    LayoutPoint localPoint(point);
    if (n) {
        if (isInlineElementContinuation()) {
            // We're in the continuation of a split inline.  Adjust our local point to be in the coordinate space
            // of the principal layoutObject's containing block.  This will end up being the innerNode.
            LayoutBlock* firstBlock = n->layoutObject()->containingBlock();

            // Get our containing block.
            LayoutBox* block = containingBlock();
            localPoint.moveBy(block->location() - firstBlock->locationOffset());
        }

        result.setNodeAndPosition(n, localPoint);
    }
}

void LayoutInline::dirtyLineBoxes(bool fullLayout)
{
    if (fullLayout) {
        m_lineBoxes.deleteLineBoxes();
        return;
    }

    if (!alwaysCreateLineBoxes()) {
        // We have to grovel into our children in order to dirty the appropriate lines.
        for (LayoutObject* curr = firstChild(); curr; curr = curr->nextSibling()) {
            if (curr->isFloatingOrOutOfFlowPositioned())
                continue;
            if (curr->isBox() && !curr->needsLayout()) {
                LayoutBox* currBox = toLayoutBox(curr);
                if (currBox->inlineBoxWrapper())
                    currBox->inlineBoxWrapper()->root().markDirty();
            } else if (!curr->selfNeedsLayout()) {
                if (curr->isLayoutInline()) {
                    LayoutInline* currInline = toLayoutInline(curr);
                    for (InlineFlowBox* childLine = currInline->firstLineBox(); childLine; childLine = childLine->nextLineBox())
                        childLine->root().markDirty();
                } else if (curr->isText()) {
                    LayoutText* currText = toLayoutText(curr);
                    for (InlineTextBox* childText = currText->firstTextBox(); childText; childText = childText->nextTextBox())
                        childText->root().markDirty();
                }
            }
        }
    } else {
        m_lineBoxes.dirtyLineBoxes();
    }
}

InlineFlowBox* LayoutInline::createInlineFlowBox()
{
    return new InlineFlowBox(*this);
}

InlineFlowBox* LayoutInline::createAndAppendInlineFlowBox()
{
    setAlwaysCreateLineBoxes();
    InlineFlowBox* flowBox = createInlineFlowBox();
    m_lineBoxes.appendLineBox(flowBox);
    return flowBox;
}

LayoutUnit LayoutInline::lineHeight(bool firstLine, LineDirectionMode /*direction*/, LinePositionMode /*linePositionMode*/) const
{
    if (firstLine && document().styleEngine().usesFirstLineRules()) {
        const ComputedStyle* s = style(firstLine);
        if (s != style())
            return s->computedLineHeight();
    }

    return style()->computedLineHeight();
}

int LayoutInline::baselinePosition(FontBaseline baselineType, bool firstLine, LineDirectionMode direction, LinePositionMode linePositionMode) const
{
    ASSERT(linePositionMode == PositionOnContainingLine);
    const FontMetrics& fontMetrics = style(firstLine)->fontMetrics();
    return fontMetrics.ascent(baselineType) + (lineHeight(firstLine, direction, linePositionMode) - fontMetrics.height()) / 2;
}

LayoutSize LayoutInline::offsetForInFlowPositionedInline(const LayoutBox& child) const
{
    // FIXME: This function isn't right with mixed writing modes.

    ASSERT(isRelPositioned());
    if (!isRelPositioned())
        return LayoutSize();

    // When we have an enclosing relpositioned inline, we need to add in the offset of the first line
    // box from the rest of the content, but only in the cases where we know we're positioned
    // relative to the inline itself.

    LayoutSize logicalOffset;
    LayoutUnit inlinePosition;
    LayoutUnit blockPosition;
    if (firstLineBox()) {
        inlinePosition = firstLineBox()->logicalLeft();
        blockPosition = firstLineBox()->logicalTop();
    } else {
        inlinePosition = layer()->staticInlinePosition();
        blockPosition = layer()->staticBlockPosition();
    }

    // Per http://www.w3.org/TR/CSS2/visudet.html#abs-non-replaced-width an absolute positioned box
    // with a static position should locate itself as though it is a normal flow box in relation to
    // its containing block. If this relative-positioned inline has a negative offset we need to
    // compensate for it so that we align the positioned object with the edge of its containing block.
    if (child.style()->hasStaticInlinePosition(style()->isHorizontalWritingMode()))
        logicalOffset.setWidth(std::max(LayoutUnit(), -offsetForInFlowPosition().width()));
    else
        logicalOffset.setWidth(inlinePosition);

    if (!child.style()->hasStaticBlockPosition(style()->isHorizontalWritingMode()))
        logicalOffset.setHeight(blockPosition);

    return style()->isHorizontalWritingMode() ? logicalOffset : logicalOffset.transposedSize();
}

void LayoutInline::imageChanged(WrappedImagePtr, const IntRect*)
{
    if (!parent())
        return;

    // FIXME: We can do better.
    setShouldDoFullPaintInvalidation();
}

namespace {

class AbsoluteLayoutRectsGeneratorContext {
public:
    AbsoluteLayoutRectsGeneratorContext(Vector<LayoutRect>& rects, const LayoutPoint& accumulatedOffset)
        : m_rects(rects)
        , m_accumulatedOffset(accumulatedOffset) { }

    void operator()(const FloatRect& rect)
    {
        LayoutRect layoutRect(rect);
        layoutRect.move(m_accumulatedOffset.x(), m_accumulatedOffset.y());
        m_rects.append(layoutRect);
    }
private:
    Vector<LayoutRect>& m_rects;
    const LayoutPoint& m_accumulatedOffset;
};

class AbsoluteLayoutRectsIgnoringEmptyRectsGeneratorContext : public AbsoluteLayoutRectsGeneratorContext {
public:
    AbsoluteLayoutRectsIgnoringEmptyRectsGeneratorContext(Vector<LayoutRect>& rects, const LayoutPoint& accumulatedOffset)
        : AbsoluteLayoutRectsGeneratorContext(rects, accumulatedOffset) { }

    void operator()(const FloatRect& rect)
    {
        if (!rect.isEmpty())
            AbsoluteLayoutRectsGeneratorContext::operator()(rect);
    }
};

} // unnamed namespace

void LayoutInline::addFocusRingRects(Vector<LayoutRect>& rects, const LayoutPoint& additionalOffset) const
{
    // Add line boxes only if this object is the first object of addFocusRingRects().
    // Otherwise the parent (LayoutBlockFlow or LayoutInline) should have added line box rects
    // covering those of this object.
    if (rects.isEmpty()) {
        AbsoluteLayoutRectsIgnoringEmptyRectsGeneratorContext context(rects, additionalOffset);
        generateLineBoxRects(context);
    }

    addFocusRingRectsForNormalChildren(rects, additionalOffset);

    if (LayoutBoxModelObject* continuation = this->continuation()) {
        if (continuation->isInline())
            continuation->addFocusRingRects(rects, additionalOffset + (continuation->containingBlock()->location() - containingBlock()->location()));
        else
            continuation->addFocusRingRects(rects, additionalOffset + (toLayoutBox(continuation)->location() - containingBlock()->location()));
    }
}

void LayoutInline::computeSelfHitTestRects(Vector<LayoutRect>& rects, const LayoutPoint& layerOffset) const
{
    AbsoluteLayoutRectsGeneratorContext context(rects, layerOffset);
    generateLineBoxRects(context);
}

void LayoutInline::addAnnotatedRegions(Vector<AnnotatedRegionValue>& regions)
{
    // Convert the style regions to absolute coordinates.
    if (style()->visibility() != VISIBLE)
        return;

    if (style()->getDraggableRegionMode() == DraggableRegionNone)
        return;

    AnnotatedRegionValue region;
    region.draggable = style()->getDraggableRegionMode() == DraggableRegionDrag;
    region.bounds = LayoutRect(linesBoundingBox());

    LayoutObject* container = containingBlock();
    if (!container)
        container = this;

    FloatPoint absPos = container->localToAbsolute();
    region.bounds.setX(absPos.x() + region.bounds.x());
    region.bounds.setY(absPos.y() + region.bounds.y());

    regions.append(region);
}

void LayoutInline::invalidateDisplayItemClients(const LayoutBoxModelObject& paintInvalidationContainer) const
{
    LayoutBoxModelObject::invalidateDisplayItemClients(paintInvalidationContainer);
    for (InlineFlowBox* box = firstLineBox(); box; box = box->nextLineBox())
        paintInvalidationContainer.invalidateDisplayItemClientOnBacking(*box);
}

} // namespace blink
