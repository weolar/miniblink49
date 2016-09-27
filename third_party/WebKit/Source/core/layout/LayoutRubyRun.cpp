/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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

#include "core/layout/LayoutRubyRun.h"

#include "core/layout/LayoutRubyBase.h"
#include "core/layout/LayoutRubyText.h"
#include "core/layout/LayoutText.h"

namespace blink {

LayoutRubyRun::LayoutRubyRun()
    : LayoutBlockFlow(nullptr)
{
    setReplaced(true);
    setInline(true);
}

LayoutRubyRun::~LayoutRubyRun()
{
}

bool LayoutRubyRun::hasRubyText() const
{
    // The only place where a ruby text can be is in the first position
    // Note: As anonymous blocks, ruby runs do not have ':before' or ':after' content themselves.
    return firstChild() && firstChild()->isRubyText();
}

bool LayoutRubyRun::hasRubyBase() const
{
    // The only place where a ruby base can be is in the last position
    // Note: As anonymous blocks, ruby runs do not have ':before' or ':after' content themselves.
    return lastChild() && lastChild()->isRubyBase();
}

LayoutRubyText* LayoutRubyRun::rubyText() const
{
    LayoutObject* child = firstChild();
    // If in future it becomes necessary to support floating or positioned ruby text,
    // layout will have to be changed to handle them properly.
    ASSERT(!child || !child->isRubyText() || !child->isFloatingOrOutOfFlowPositioned());
    return child && child->isRubyText() ? static_cast<LayoutRubyText*>(child) : 0;
}

LayoutRubyBase* LayoutRubyRun::rubyBase() const
{
    LayoutObject* child = lastChild();
    return child && child->isRubyBase() ? static_cast<LayoutRubyBase*>(child) : 0;
}

LayoutRubyBase* LayoutRubyRun::rubyBaseSafe()
{
    LayoutRubyBase* base = rubyBase();
    if (!base) {
        base = createRubyBase();
        LayoutBlockFlow::addChild(base);
    }
    return base;
}

bool LayoutRubyRun::isChildAllowed(LayoutObject* child, const ComputedStyle&) const
{
    return child->isRubyText() || child->isInline();
}

void LayoutRubyRun::addChild(LayoutObject* child, LayoutObject* beforeChild)
{
    ASSERT(child);

    if (child->isRubyText()) {
        if (!beforeChild) {
            // LayoutRuby has already ascertained that we can add the child here.
            ASSERT(!hasRubyText());
            // prepend ruby texts as first child
            LayoutBlockFlow::addChild(child, firstChild());
        }  else if (beforeChild->isRubyText()) {
            // New text is inserted just before another.
            // In this case the new text takes the place of the old one, and
            // the old text goes into a new run that is inserted as next sibling.
            ASSERT(beforeChild->parent() == this);
            LayoutObject* ruby = parent();
            ASSERT(ruby->isRuby());
            LayoutBlock* newRun = staticCreateRubyRun(ruby);
            ruby->addChild(newRun, nextSibling());
            // Add the new ruby text and move the old one to the new run
            // Note: Doing it in this order and not using LayoutRubyRun's methods,
            // in order to avoid automatic removal of the ruby run in case there is no
            // other child besides the old ruby text.
            LayoutBlockFlow::addChild(child, beforeChild);
            LayoutBlockFlow::removeChild(beforeChild);
            newRun->addChild(beforeChild);
        } else if (hasRubyBase()) {
            // Insertion before a ruby base object.
            // In this case we need insert a new run before the current one and split the base.
            LayoutObject* ruby = parent();
            LayoutRubyRun* newRun = staticCreateRubyRun(ruby);
            ruby->addChild(newRun, this);
            newRun->addChild(child);
            rubyBaseSafe()->moveChildren(newRun->rubyBaseSafe(), beforeChild);
        }
    } else {
        // child is not a text -> insert it into the base
        // (append it instead if beforeChild is the ruby text)
        LayoutRubyBase* base = rubyBaseSafe();
        if (beforeChild == base)
            beforeChild = base->firstChild();
        if (beforeChild && beforeChild->isRubyText())
            beforeChild = 0;
        ASSERT(!beforeChild || beforeChild->isDescendantOf(base));
        base->addChild(child, beforeChild);
    }
}

void LayoutRubyRun::removeChild(LayoutObject* child)
{
    // If the child is a ruby text, then merge the ruby base with the base of
    // the right sibling run, if possible.
    if (!beingDestroyed() && !documentBeingDestroyed() && child->isRubyText()) {
        LayoutRubyBase* base = rubyBase();
        LayoutObject* rightNeighbour = nextSibling();
        if (base && rightNeighbour && rightNeighbour->isRubyRun()) {
            // Ruby run without a base can happen only at the first run.
            LayoutRubyRun* rightRun = toLayoutRubyRun(rightNeighbour);
            if (rightRun->hasRubyBase()) {
                LayoutRubyBase* rightBase = rightRun->rubyBaseSafe();
                // Collect all children in a single base, then swap the bases.
                rightBase->moveChildren(base);
                moveChildTo(rightRun, base);
                rightRun->moveChildTo(this, rightBase);
                // The now empty ruby base will be removed below.
                ASSERT(!rubyBase()->firstChild());
            }
        }
    }

    LayoutBlockFlow::removeChild(child);

    if (!beingDestroyed() && !documentBeingDestroyed()) {
        // Check if our base (if any) is now empty. If so, destroy it.
        LayoutBlock* base = rubyBase();
        if (base && !base->firstChild()) {
            LayoutBlockFlow::removeChild(base);
            base->deleteLineBoxTree();
            base->destroy();
        }

        // If any of the above leaves the run empty, destroy it as well.
        if (!hasRubyText() && !hasRubyBase()) {
            deleteLineBoxTree();
            destroy();
        }
    }
}

LayoutRubyBase* LayoutRubyRun::createRubyBase() const
{
    LayoutRubyBase* layoutObject = LayoutRubyBase::createAnonymous(&document());
    RefPtr<ComputedStyle> newStyle = ComputedStyle::createAnonymousStyleWithDisplay(styleRef(), BLOCK);
    newStyle->setTextAlign(CENTER); // FIXME: use WEBKIT_CENTER?
    layoutObject->setStyle(newStyle.release());
    return layoutObject;
}

LayoutRubyRun* LayoutRubyRun::staticCreateRubyRun(const LayoutObject* parentRuby)
{
    ASSERT(parentRuby && parentRuby->isRuby());
    LayoutRubyRun* rr = new LayoutRubyRun();
    rr->setDocumentForAnonymous(&parentRuby->document());
    RefPtr<ComputedStyle> newStyle = ComputedStyle::createAnonymousStyleWithDisplay(parentRuby->styleRef(), INLINE_BLOCK);
    rr->setStyle(newStyle.release());
    return rr;
}

LayoutObject* LayoutRubyRun::layoutSpecialExcludedChild(bool relayoutChildren, SubtreeLayoutScope& layoutScope)
{
    // Don't bother positioning the LayoutRubyRun yet.
    LayoutRubyText* rt = rubyText();
    if (!rt)
        return nullptr;
    if (relayoutChildren)
        layoutScope.setChildNeedsLayout(rt);
    rt->layoutIfNeeded();
    return rt;
}

void LayoutRubyRun::layout()
{
    LayoutBlockFlow::layout();

    LayoutRubyText* rt = rubyText();
    if (!rt)
        return;

    rt->setLogicalLeft(0);

    // Place the LayoutRubyText such that its bottom is flush with the lineTop of the first line of the LayoutRubyBase.
    LayoutUnit lastLineRubyTextBottom = rt->logicalHeight();
    LayoutUnit firstLineRubyTextTop = 0;
    RootInlineBox* rootBox = rt->lastRootBox();
    if (rootBox) {
        // In order to align, we have to ignore negative leading.
        firstLineRubyTextTop = rt->firstRootBox()->logicalTopLayoutOverflow();
        lastLineRubyTextBottom = rootBox->logicalBottomLayoutOverflow();
    }

    if (style()->isFlippedLinesWritingMode() == (style()->rubyPosition() == RubyPositionAfter)) {
        LayoutUnit firstLineTop = 0;
        if (LayoutRubyBase* rb = rubyBase()) {
            RootInlineBox* rootBox = rb->firstRootBox();
            if (rootBox)
                firstLineTop = rootBox->logicalTopLayoutOverflow();
            firstLineTop += rb->logicalTop();
        }

        rt->setLogicalTop(-lastLineRubyTextBottom + firstLineTop);
    } else {
        LayoutUnit lastLineBottom = logicalHeight();
        if (LayoutRubyBase* rb = rubyBase()) {
            RootInlineBox* rootBox = rb->lastRootBox();
            if (rootBox)
                lastLineBottom = rootBox->logicalBottomLayoutOverflow();
            lastLineBottom += rb->logicalTop();
        }

        rt->setLogicalTop(-firstLineRubyTextTop + lastLineBottom);
    }

    // Update our overflow to account for the new LayoutRubyText position.
    computeOverflow(clientLogicalBottom());
}

void LayoutRubyRun::getOverhang(bool firstLine, LayoutObject* startLayoutObject, LayoutObject* endLayoutObject, int& startOverhang, int& endOverhang) const
{
    ASSERT(!needsLayout());

    startOverhang = 0;
    endOverhang = 0;

    LayoutRubyBase* rubyBase = this->rubyBase();
    LayoutRubyText* rubyText = this->rubyText();

    if (!rubyBase || !rubyText)
        return;

    if (!rubyBase->firstRootBox())
        return;

    int logicalWidth = this->logicalWidth();
    int logicalLeftOverhang = std::numeric_limits<int>::max();
    int logicalRightOverhang = std::numeric_limits<int>::max();
    for (RootInlineBox* rootInlineBox = rubyBase->firstRootBox(); rootInlineBox; rootInlineBox = rootInlineBox->nextRootBox()) {
        logicalLeftOverhang = std::min<int>(logicalLeftOverhang, rootInlineBox->logicalLeft());
        logicalRightOverhang = std::min<int>(logicalRightOverhang, logicalWidth - rootInlineBox->logicalRight());
    }

    startOverhang = style()->isLeftToRightDirection() ? logicalLeftOverhang : logicalRightOverhang;
    endOverhang = style()->isLeftToRightDirection() ? logicalRightOverhang : logicalLeftOverhang;

    if (!startLayoutObject || !startLayoutObject->isText() || startLayoutObject->style(firstLine)->fontSize() > rubyBase->style(firstLine)->fontSize())
        startOverhang = 0;

    if (!endLayoutObject || !endLayoutObject->isText() || endLayoutObject->style(firstLine)->fontSize() > rubyBase->style(firstLine)->fontSize())
        endOverhang = 0;

    // We overhang a ruby only if the neighboring layout object is a text.
    // We can overhang the ruby by no more than half the width of the neighboring text
    // and no more than half the font size.
    int halfWidthOfFontSize = rubyText->style(firstLine)->fontSize() / 2;
    if (startOverhang)
        startOverhang = std::min<int>(startOverhang, std::min<int>(toLayoutText(startLayoutObject)->minLogicalWidth(), halfWidthOfFontSize));
    if (endOverhang)
        endOverhang = std::min<int>(endOverhang, std::min<int>(toLayoutText(endLayoutObject)->minLogicalWidth(), halfWidthOfFontSize));
}

} // namespace blink
