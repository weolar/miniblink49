/*
 * Copyright (C) 2006 Oliver Hunt <ojh16@student.canterbury.ac.nz>
 * Copyright (C) 2006 Apple Computer Inc.
 * Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 * Copyright (C) 2011 Torch Mobile (Beijing) CO. Ltd. All rights reserved.
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
#include "core/layout/svg/line/SVGRootInlineBox.h"

#include "core/layout/svg/LayoutSVGInlineText.h"
#include "core/layout/svg/LayoutSVGText.h"
#include "core/layout/svg/line/SVGInlineFlowBox.h"
#include "core/layout/svg/line/SVGInlineTextBox.h"
#include "core/paint/SVGRootInlineBoxPainter.h"
#include "core/svg/SVGTextPathElement.h"

namespace blink {

void SVGRootInlineBox::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset, LayoutUnit, LayoutUnit)
{
    SVGRootInlineBoxPainter(*this).paint(paintInfo, paintOffset);
}

void SVGRootInlineBox::markDirty()
{
    for (InlineBox* child = firstChild(); child; child = child->nextOnLine())
        child->markDirty();
    RootInlineBox::markDirty();
}

void SVGRootInlineBox::computePerCharacterLayoutInformation()
{
    LayoutSVGText& textRoot = toLayoutSVGText(block());

    Vector<SVGTextLayoutAttributes*>& layoutAttributes = textRoot.layoutAttributes();
    if (layoutAttributes.isEmpty())
        return;

    if (textRoot.needsReordering())
        reorderValueLists(layoutAttributes);

    // Perform SVG text layout phase two (see SVGTextLayoutEngine for details).
    SVGTextLayoutEngine characterLayout(layoutAttributes);
    characterLayout.layoutCharactersInTextBoxes(this);

    // Perform SVG text layout phase three (see SVGTextChunkBuilder for details).
    characterLayout.finishLayout();

    // Perform SVG text layout phase four
    // Position & resize all SVGInlineText/FlowBoxes in the inline box tree, resize the root box as well as the LayoutSVGText parent block.
    LayoutRect childRect;
    layoutChildBoxes(this, &childRect);
    layoutRootBox(childRect);
}

void SVGRootInlineBox::layoutChildBoxes(InlineFlowBox* start, LayoutRect* childRect)
{
    for (InlineBox* child = start->firstChild(); child; child = child->nextOnLine()) {
        LayoutRect boxRect;
        if (child->isSVGInlineTextBox()) {
            ASSERT(child->layoutObject().isSVGInlineText());

            SVGInlineTextBox* textBox = toSVGInlineTextBox(child);
            boxRect = textBox->calculateBoundaries();
            textBox->setX(boxRect.x());
            textBox->setY(boxRect.y());
            textBox->setLogicalWidth(boxRect.width());
            textBox->setLogicalHeight(boxRect.height());
        } else {
            // Skip generated content.
            if (!child->layoutObject().node())
                continue;

            SVGInlineFlowBox* flowBox = toSVGInlineFlowBox(child);
            layoutChildBoxes(flowBox);

            boxRect = flowBox->calculateBoundaries();
            flowBox->setX(boxRect.x());
            flowBox->setY(boxRect.y());
            flowBox->setLogicalWidth(boxRect.width());
            flowBox->setLogicalHeight(boxRect.height());
        }
        if (childRect)
            childRect->unite(boxRect);
    }
}

void SVGRootInlineBox::layoutRootBox(const LayoutRect& childRect)
{
    LayoutBlockFlow& parentBlock = block();

    // Finally, assign the root block position, now that all content is laid out.
    LayoutRect boundingRect = childRect;
    parentBlock.setLocation(boundingRect.location());
    parentBlock.setSize(boundingRect.size());

    // Position all children relative to the parent block.
    for (InlineBox* child = firstChild(); child; child = child->nextOnLine()) {
        // Skip generated content.
        if (!child->layoutObject().node())
            continue;
        child->move(LayoutSize(-childRect.x(), -childRect.y()));
    }

    // Position ourselves.
    setX(0);
    setY(0);
    setLogicalWidth(childRect.width());
    setLogicalHeight(childRect.height());
    setLineTopBottomPositions(0, boundingRect.height(), 0, boundingRect.height());
}

InlineBox* SVGRootInlineBox::closestLeafChildForPosition(const LayoutPoint& point)
{
    InlineBox* firstLeaf = firstLeafChild();
    InlineBox* lastLeaf = lastLeafChild();
    if (firstLeaf == lastLeaf)
        return firstLeaf;

    // FIXME: Check for vertical text!
    InlineBox* closestLeaf = nullptr;
    for (InlineBox* leaf = firstLeaf; leaf; leaf = leaf->nextLeafChild()) {
        if (!leaf->isSVGInlineTextBox())
            continue;
        if (point.y() < leaf->y())
            continue;
        if (point.y() > leaf->y() + leaf->virtualLogicalHeight())
            continue;

        closestLeaf = leaf;
        if (point.x() < leaf->left() + leaf->logicalWidth())
            return leaf;
    }

    return closestLeaf ? closestLeaf : lastLeaf;
}

static inline void swapItemsInLayoutAttributes(SVGTextLayoutAttributes* firstAttributes, SVGTextLayoutAttributes* lastAttributes, unsigned firstPosition, unsigned lastPosition)
{
    SVGCharacterDataMap::iterator itFirst = firstAttributes->characterDataMap().find(firstPosition + 1);
    SVGCharacterDataMap::iterator itLast = lastAttributes->characterDataMap().find(lastPosition + 1);
    bool firstPresent = itFirst != firstAttributes->characterDataMap().end();
    bool lastPresent = itLast != lastAttributes->characterDataMap().end();
    // We only want to perform the swap if both inline boxes are absolutely
    // positioned.
    if (!firstPresent || !lastPresent)
        return;
    std::swap(itFirst->value, itLast->value);
}

static inline void findFirstAndLastAttributesInVector(Vector<SVGTextLayoutAttributes*>& attributes, LayoutSVGInlineText* firstContext, LayoutSVGInlineText* lastContext, SVGTextLayoutAttributes*& first, SVGTextLayoutAttributes*& last)
{
    first = 0;
    last = 0;

    unsigned attributesSize = attributes.size();
    for (unsigned i = 0; i < attributesSize; ++i) {
        SVGTextLayoutAttributes* current = attributes[i];
        if (!first && firstContext == current->context())
            first = current;
        if (!last && lastContext == current->context())
            last = current;
        if (first && last)
            break;
    }

    ASSERT(first);
    ASSERT(last);
}

static inline void reverseInlineBoxRangeAndValueListsIfNeeded(void* userData, Vector<InlineBox*>::iterator first, Vector<InlineBox*>::iterator last)
{
    ASSERT(userData);
    Vector<SVGTextLayoutAttributes*>& attributes = *reinterpret_cast<Vector<SVGTextLayoutAttributes*>*>(userData);

    // This is a copy of std::reverse(first, last). It additionally assures that the metrics map within the layoutObjects belonging to the InlineBoxes are reordered as well.
    while (true)  {
        if (first == last || first == --last)
            return;

        if (!(*last)->isSVGInlineTextBox() || !(*first)->isSVGInlineTextBox()) {
            InlineBox* temp = *first;
            *first = *last;
            *last = temp;
            ++first;
            continue;
        }

        SVGInlineTextBox* firstTextBox = toSVGInlineTextBox(*first);
        SVGInlineTextBox* lastTextBox = toSVGInlineTextBox(*last);

        // Reordering is only necessary for BiDi text that is _absolutely_ positioned.
        if (firstTextBox->len() == 1 && firstTextBox->len() == lastTextBox->len()) {
            LayoutSVGInlineText& firstContext = toLayoutSVGInlineText(firstTextBox->layoutObject());
            LayoutSVGInlineText& lastContext = toLayoutSVGInlineText(lastTextBox->layoutObject());

            SVGTextLayoutAttributes* firstAttributes = nullptr;
            SVGTextLayoutAttributes* lastAttributes = nullptr;
            findFirstAndLastAttributesInVector(attributes, &firstContext, &lastContext, firstAttributes, lastAttributes);
            swapItemsInLayoutAttributes(firstAttributes, lastAttributes, firstTextBox->start(), lastTextBox->start());
        }

        InlineBox* temp = *first;
        *first = *last;
        *last = temp;

        ++first;
    }
}

void SVGRootInlineBox::reorderValueLists(Vector<SVGTextLayoutAttributes*>& attributes)
{
    Vector<InlineBox*> leafBoxesInLogicalOrder;
    collectLeafBoxesInLogicalOrder(leafBoxesInLogicalOrder, reverseInlineBoxRangeAndValueListsIfNeeded, &attributes);
}

} // namespace blink
