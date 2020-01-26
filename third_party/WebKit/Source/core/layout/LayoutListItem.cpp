/**
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Andrew Wellington (proton@wiretapped.net)
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
#include "core/layout/LayoutListItem.h"

#include "core/HTMLNames.h"
#include "core/dom/shadow/ComposedTreeTraversal.h"
#include "core/html/HTMLOListElement.h"
#include "core/layout/LayoutListMarker.h"
#include "core/layout/LayoutView.h"
#include "wtf/StdLibExtras.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

using namespace HTMLNames;

LayoutListItem::LayoutListItem(Element* element)
    : LayoutBlockFlow(element)
    , m_marker(nullptr)
    , m_hasExplicitValue(false)
    , m_isValueUpToDate(false)
    , m_notInList(false)
{
    setInline(false);

    setConsumesSubtreeChangeNotification();
    registerSubtreeChangeListenerOnDescendants(true);
}

void LayoutListItem::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    LayoutBlockFlow::styleDidChange(diff, oldStyle);

    if (style()->listStyleType() != NoneListStyle
        || (style()->listStyleImage() && !style()->listStyleImage()->errorOccurred())) {
        if (!m_marker)
            m_marker = LayoutListMarker::createAnonymous(this);
        m_marker->listItemStyleDidChange();
        notifyOfSubtreeChange();
    } else if (m_marker) {
        m_marker->destroy();
        m_marker = nullptr;
    }
}

void LayoutListItem::willBeDestroyed()
{
    if (m_marker) {
        m_marker->destroy();
        m_marker = nullptr;
    }
    LayoutBlockFlow::willBeDestroyed();
}

void LayoutListItem::insertedIntoTree()
{
    LayoutBlockFlow::insertedIntoTree();

    updateListMarkerNumbers();
}

void LayoutListItem::willBeRemovedFromTree()
{
    LayoutBlockFlow::willBeRemovedFromTree();

    updateListMarkerNumbers();
}

void LayoutListItem::subtreeDidChange()
{
    if (!m_marker)
        return;

    if (!updateMarkerLocation())
        return;

    // If the marker is inside we need to redo the preferred width calculations
    // as the size of the item now includes the size of the list marker.
    if (m_marker->isInside())
        setPreferredLogicalWidthsDirty();
}

static bool isList(const Node& node)
{
    return isHTMLUListElement(node) || isHTMLOListElement(node);
}

// Returns the enclosing list with respect to the DOM order.
static Node* enclosingList(const LayoutListItem* listItem)
{
    Node* listItemNode = listItem->node();
    if (!listItemNode)
        return nullptr;
    Node* firstNode = nullptr;
    // We use parentNode because the enclosing list could be a ShadowRoot that's not Element.
    for (Node* parent = ComposedTreeTraversal::parent(*listItemNode); parent; parent = ComposedTreeTraversal::parent(*parent)) {
        if (isList(*parent))
            return parent;
        if (!firstNode)
            firstNode = parent;
    }

    // If there's no actual <ul> or <ol> list element, then the first found
    // node acts as our list for purposes of determining what other list items
    // should be numbered as part of the same list.
    return firstNode;
}

// Returns the next list item with respect to the DOM order.
static LayoutListItem* nextListItem(const Node* listNode, const LayoutListItem* item = nullptr)
{
    if (!listNode)
        return nullptr;

    const Node* current = item ? item->node() : listNode;
    ASSERT(current);
    ASSERT(!current->document().childNeedsDistributionRecalc());
    current = LayoutTreeBuilderTraversal::next(*current, listNode);

    while (current) {
        if (isList(*current)) {
            // We've found a nested, independent list: nothing to do here.
            current = LayoutTreeBuilderTraversal::nextSkippingChildren(*current, listNode);
            continue;
        }

        LayoutObject* layoutObject = current->layoutObject();
        if (layoutObject && layoutObject->isListItem())
            return toLayoutListItem(layoutObject);

        // FIXME: Can this be optimized to skip the children of the elements without a layoutObject?
        current = LayoutTreeBuilderTraversal::next(*current, listNode);
    }

    return nullptr;
}

// Returns the previous list item with respect to the DOM order.
static LayoutListItem* previousListItem(const Node* listNode, const LayoutListItem* item)
{
    Node* current = item->node();
    ASSERT(current);
    ASSERT(!current->document().childNeedsDistributionRecalc());
    for (current = LayoutTreeBuilderTraversal::previous(*current, listNode); current && current != listNode; current = LayoutTreeBuilderTraversal::previous(*current, listNode)) {
        LayoutObject* layoutObject = current->layoutObject();
        if (!layoutObject || (layoutObject && !layoutObject->isListItem()))
            continue;
        Node* otherList = enclosingList(toLayoutListItem(layoutObject));
        // This item is part of our current list, so it's what we're looking for.
        if (listNode == otherList)
            return toLayoutListItem(layoutObject);
        // We found ourself inside another list; lets skip the rest of it.
        // Use nextIncludingPseudo() here because the other list itself may actually
        // be a list item itself. We need to examine it, so we do this to counteract
        // the previousIncludingPseudo() that will be done by the loop.
        if (otherList)
            current = LayoutTreeBuilderTraversal::next(*otherList, listNode);
    }
    return nullptr;
}

void LayoutListItem::updateItemValuesForOrderedList(const HTMLOListElement* listNode)
{
    ASSERT(listNode);

    for (LayoutListItem* listItem = nextListItem(listNode); listItem; listItem = nextListItem(listNode, listItem))
        listItem->updateValue();
}

unsigned LayoutListItem::itemCountForOrderedList(const HTMLOListElement* listNode)
{
    ASSERT(listNode);

    unsigned itemCount = 0;
    for (LayoutListItem* listItem = nextListItem(listNode); listItem; listItem = nextListItem(listNode, listItem))
        itemCount++;

    return itemCount;
}

inline int LayoutListItem::calcValue() const
{
    if (m_hasExplicitValue)
        return m_explicitValue;

    Node* list = enclosingList(this);
    HTMLOListElement* oListElement = isHTMLOListElement(list) ? toHTMLOListElement(list) : 0;
    int valueStep = 1;
    if (oListElement && oListElement->isReversed())
        valueStep = -1;

    // FIXME: This recurses to a possible depth of the length of the list.
    // That's not good -- we need to change this to an iterative algorithm.
    if (LayoutListItem* previousItem = previousListItem(list, this))
        return previousItem->value() + valueStep;

    if (oListElement)
        return oListElement->start();

    return 1;
}

void LayoutListItem::updateValueNow() const
{
    m_value = calcValue();
    m_isValueUpToDate = true;
}

bool LayoutListItem::isEmpty() const
{
    return lastChild() == m_marker;
}

static LayoutObject* getParentOfFirstLineBox(LayoutBlockFlow* curr, LayoutObject* marker)
{
    LayoutObject* firstChild = curr->firstChild();
    if (!firstChild)
        return nullptr;

    bool inQuirksMode = curr->document().inQuirksMode();
    for (LayoutObject* currChild = firstChild; currChild; currChild = currChild->nextSibling()) {
        if (currChild == marker)
            continue;

        if (currChild->isInline() && (!currChild->isLayoutInline() || curr->generatesLineBoxesForInlineChild(currChild)))
            return curr;

        if (currChild->isFloating() || currChild->isOutOfFlowPositioned())
            continue;

        if (!currChild->isLayoutBlockFlow() || (currChild->isBox() && toLayoutBox(currChild)->isWritingModeRoot()))
            break;

        if (curr->isListItem() && inQuirksMode && currChild->node()
            && (isHTMLUListElement(*currChild->node()) || isHTMLOListElement(*currChild->node())))
            break;

        LayoutObject* lineBox = getParentOfFirstLineBox(toLayoutBlockFlow(currChild), marker);
        if (lineBox)
            return lineBox;
    }

    return nullptr;
}

void LayoutListItem::updateValue()
{
    if (!m_hasExplicitValue) {
        m_isValueUpToDate = false;
        if (m_marker)
            m_marker->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::ListValueChange);
    }
}

static LayoutObject* firstNonMarkerChild(LayoutObject* parent)
{
    LayoutObject* result = parent->slowFirstChild();
    while (result && result->isListMarker())
        result = result->nextSibling();
    return result;
}

bool LayoutListItem::updateMarkerLocation()
{
    ASSERT(m_marker);

    LayoutObject* markerParent = m_marker->parent();
    LayoutObject* lineBoxParent = m_marker->isInside() ? this : getParentOfFirstLineBox(this, m_marker);
    if (!lineBoxParent) {
        // If the marker is currently contained inside an anonymous box, then we
        // are the only item in that anonymous box (since no line box parent was
        // found). It's ok to just leave the marker where it is in this case.
        if (markerParent && markerParent->isAnonymousBlock())
            lineBoxParent = markerParent;
        else
            lineBoxParent = this;
    }

    if (markerParent != lineBoxParent) {
        m_marker->remove();
        lineBoxParent->addChild(m_marker, firstNonMarkerChild(lineBoxParent));
        m_marker->updateMarginsAndContent();
        // If markerParent is an anonymous block with no children, destroy it.
        if (markerParent && markerParent->isAnonymousBlock() && !toLayoutBlock(markerParent)->firstChild() && !toLayoutBlock(markerParent)->continuation())
            markerParent->destroy();
        return true;
    }

    return false;
}

void LayoutListItem::addOverflowFromChildren()
{
    LayoutBlockFlow::addOverflowFromChildren();
    positionListMarker();
}

void LayoutListItem::positionListMarker()
{
    if (m_marker && m_marker->parent() && m_marker->parent()->isBox() && !m_marker->isInside() && m_marker->inlineBoxWrapper()) {
        LayoutUnit markerOldLogicalLeft = m_marker->logicalLeft();
        LayoutUnit blockOffset = 0;
        LayoutUnit lineOffset = 0;
        for (LayoutBox* o = m_marker->parentBox(); o != this; o = o->parentBox()) {
            blockOffset += o->logicalTop();
            lineOffset += o->logicalLeft();
        }

        bool adjustOverflow = false;
        LayoutUnit markerLogicalLeft;
        RootInlineBox& root = m_marker->inlineBoxWrapper()->root();
        bool hitSelfPaintingLayer = false;

        LayoutUnit lineTop = root.lineTop();
        LayoutUnit lineBottom = root.lineBottom();

        // FIXME: Need to account for relative positioning in the layout overflow.
        if (style()->isLeftToRightDirection()) {
            LayoutUnit leftLineOffset = logicalLeftOffsetForLine(blockOffset, logicalLeftOffsetForLine(blockOffset, false), false);
            markerLogicalLeft = leftLineOffset - lineOffset - paddingStart() - borderStart() + m_marker->marginStart();
            m_marker->inlineBoxWrapper()->moveInInlineDirection((markerLogicalLeft - markerOldLogicalLeft).toFloat());
            for (InlineFlowBox* box = m_marker->inlineBoxWrapper()->parent(); box; box = box->parent()) {
                LayoutRect newLogicalVisualOverflowRect = box->logicalVisualOverflowRect(lineTop, lineBottom);
                LayoutRect newLogicalLayoutOverflowRect = box->logicalLayoutOverflowRect(lineTop, lineBottom);
                if (markerLogicalLeft < newLogicalVisualOverflowRect.x() && !hitSelfPaintingLayer) {
                    newLogicalVisualOverflowRect.setWidth(newLogicalVisualOverflowRect.maxX() - markerLogicalLeft);
                    newLogicalVisualOverflowRect.setX(markerLogicalLeft);
                    if (box == root)
                        adjustOverflow = true;
                }
                if (markerLogicalLeft < newLogicalLayoutOverflowRect.x()) {
                    newLogicalLayoutOverflowRect.setWidth(newLogicalLayoutOverflowRect.maxX() - markerLogicalLeft);
                    newLogicalLayoutOverflowRect.setX(markerLogicalLeft);
                    if (box == root)
                        adjustOverflow = true;
                }
                box->setOverflowFromLogicalRects(newLogicalLayoutOverflowRect, newLogicalVisualOverflowRect, lineTop, lineBottom);
                if (box->boxModelObject()->hasSelfPaintingLayer())
                    hitSelfPaintingLayer = true;
            }
        } else {
            LayoutUnit rightLineOffset = logicalRightOffsetForLine(blockOffset, logicalRightOffsetForLine(blockOffset, false), false);
            markerLogicalLeft = rightLineOffset - lineOffset + paddingStart() + borderStart() + m_marker->marginEnd();
            m_marker->inlineBoxWrapper()->moveInInlineDirection((markerLogicalLeft - markerOldLogicalLeft).toFloat());
            for (InlineFlowBox* box = m_marker->inlineBoxWrapper()->parent(); box; box = box->parent()) {
                LayoutRect newLogicalVisualOverflowRect = box->logicalVisualOverflowRect(lineTop, lineBottom);
                LayoutRect newLogicalLayoutOverflowRect = box->logicalLayoutOverflowRect(lineTop, lineBottom);
                if (markerLogicalLeft + m_marker->logicalWidth() > newLogicalVisualOverflowRect.maxX() && !hitSelfPaintingLayer) {
                    newLogicalVisualOverflowRect.setWidth(markerLogicalLeft + m_marker->logicalWidth() - newLogicalVisualOverflowRect.x());
                    if (box == root)
                        adjustOverflow = true;
                }
                if (markerLogicalLeft + m_marker->logicalWidth() > newLogicalLayoutOverflowRect.maxX()) {
                    newLogicalLayoutOverflowRect.setWidth(markerLogicalLeft + m_marker->logicalWidth() - newLogicalLayoutOverflowRect.x());
                    if (box == root)
                        adjustOverflow = true;
                }
                box->setOverflowFromLogicalRects(newLogicalLayoutOverflowRect, newLogicalVisualOverflowRect, lineTop, lineBottom);

                if (box->boxModelObject()->hasSelfPaintingLayer())
                    hitSelfPaintingLayer = true;
            }
        }

        if (adjustOverflow) {
            LayoutRect markerRect(LayoutPoint(markerLogicalLeft + lineOffset, blockOffset), m_marker->size());
            if (!style()->isHorizontalWritingMode())
                markerRect = markerRect.transposedRect();
            LayoutBox* o = m_marker;
            bool propagateVisualOverflow = true;
            bool propagateLayoutOverflow = true;
            do {
                o = o->parentBox();
                if (o->isLayoutBlock()) {
                    if (propagateVisualOverflow)
                        toLayoutBlock(o)->addContentsVisualOverflow(markerRect);
                    if (propagateLayoutOverflow)
                        toLayoutBlock(o)->addLayoutOverflow(markerRect);
                }
                if (o->hasOverflowClip()) {
                    propagateLayoutOverflow = false;
                    propagateVisualOverflow = false;
                }
                if (o->hasSelfPaintingLayer())
                    propagateVisualOverflow = false;
                markerRect.moveBy(-o->location());
            } while (o != this && propagateVisualOverflow && propagateLayoutOverflow);
        }
    }
}

void LayoutListItem::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (!logicalHeight() && hasOverflowClip())
        return;

    LayoutBlockFlow::paint(paintInfo, paintOffset);
}

const String& LayoutListItem::markerText() const
{
    if (m_marker)
        return m_marker->text();
    return nullAtom.string();
}

void LayoutListItem::explicitValueChanged()
{
    if (m_marker)
        m_marker->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::ListValueChange);
    Node* listNode = enclosingList(this);
    for (LayoutListItem* item = this; item; item = nextListItem(listNode, item))
        item->updateValue();
}

void LayoutListItem::setExplicitValue(int value)
{
    ASSERT(node());

    if (m_hasExplicitValue && m_explicitValue == value)
        return;
    m_explicitValue = value;
    m_value = value;
    m_hasExplicitValue = true;
    explicitValueChanged();
}

void LayoutListItem::clearExplicitValue()
{
    ASSERT(node());

    if (!m_hasExplicitValue)
        return;
    m_hasExplicitValue = false;
    m_isValueUpToDate = false;
    explicitValueChanged();
}

void LayoutListItem::setNotInList(bool notInList)
{
    m_notInList = notInList;
}

static LayoutListItem* previousOrNextItem(bool isListReversed, Node* list, LayoutListItem* item)
{
    return isListReversed ? previousListItem(list, item) : nextListItem(list, item);
}

void LayoutListItem::updateListMarkerNumbers()
{
    // If distribution recalc is needed, updateListMarkerNumber will be re-invoked
    // after distribution is calculated.
    if (node()->document().childNeedsDistributionRecalc())
        return;

    Node* listNode = enclosingList(this);
    ASSERT(listNode);

    bool isListReversed = false;
    HTMLOListElement* oListElement = isHTMLOListElement(listNode) ? toHTMLOListElement(listNode) : 0;
    if (oListElement) {
        oListElement->itemCountChanged();
        isListReversed = oListElement->isReversed();
    }

    // FIXME: The n^2 protection below doesn't help if the elements were inserted after the
    // the list had already been displayed.

    // Avoid an O(n^2) walk over the children below when they're all known to be attaching.
    if (listNode->needsAttach())
        return;

    for (LayoutListItem* item = previousOrNextItem(isListReversed, listNode, this); item; item = previousOrNextItem(isListReversed, listNode, item)) {
        if (!item->m_isValueUpToDate) {
            // If an item has been marked for update before, we can safely
            // assume that all the following ones have too.
            // This gives us the opportunity to stop here and avoid
            // marking the same nodes again.
            break;
        }
        item->updateValue();
    }
}

} // namespace blink
