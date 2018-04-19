/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 *           (C) 2004 Allan Sandfeld Jensen (kde@carewolf.com)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Google Inc. All rights reserved.
 * Copyright (C) 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
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
#include "core/layout/LayoutObject.h"

#include "core/HTMLNames.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/dom/AXObjectCache.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/StyleEngine.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/EditingBoundary.h"
#include "core/editing/FrameSelection.h"
#include "core/editing/htmlediting.h"
#include "core/fetch/ResourceLoadPriorityOptimizer.h"
#include "core/fetch/ResourceLoader.h"
#include "core/frame/DeprecatedScheduleStyleRecalcDuringLayout.h"
#include "core/frame/EventHandlerRegistry.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/frame/UseCounter.h"
#include "core/html/HTMLAnchorElement.h"
#include "core/html/HTMLElement.h"
#include "core/html/HTMLHtmlElement.h"
#include "core/html/HTMLTableCellElement.h"
#include "core/html/HTMLTableElement.h"
#include "core/input/EventHandler.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutCounter.h"
#include "core/layout/LayoutDeprecatedFlexibleBox.h"
#include "core/layout/LayoutFlexibleBox.h"
#include "core/layout/LayoutFlowThread.h"
#include "core/layout/LayoutGeometryMap.h"
#include "core/layout/LayoutGrid.h"
#include "core/layout/LayoutImage.h"
#include "core/layout/LayoutImageResourceStyleImage.h"
#include "core/layout/LayoutInline.h"
#include "core/layout/LayoutListItem.h"
#include "core/layout/LayoutMultiColumnSpannerPlaceholder.h"
#include "core/layout/LayoutObjectInlines.h"
#include "core/layout/LayoutPart.h"
#include "core/layout/LayoutScrollbarPart.h"
#include "core/layout/LayoutTableCaption.h"
#include "core/layout/LayoutTableCell.h"
#include "core/layout/LayoutTableCol.h"
#include "core/layout/LayoutTableRow.h"
#include "core/layout/LayoutTheme.h"
#include "core/layout/LayoutView.h"
#include "core/layout/compositing/DeprecatedPaintLayerCompositor.h"
#include "core/page/AutoscrollController.h"
#include "core/page/Page.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/ObjectPainter.h"
#include "core/style/ContentData.h"
#include "core/style/ShadowList.h"
#include "platform/JSONValues.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/TraceEvent.h"
#include "platform/TracedValue.h"
#include "platform/geometry/TransformState.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/paint/DisplayItemList.h"
#include "wtf/Partitions.h"
#include "wtf/RefCountedLeakCounter.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/WTFString.h"
#include <algorithm>
#ifndef NDEBUG
#include <stdio.h>
#endif

namespace blink {

namespace {

static bool gModifyLayoutTreeStructureAnyState = false;

static bool gDisablePaintInvalidationStateAsserts = false;

} // namespace

using namespace HTMLNames;

const LayoutUnit& caretWidth()
{
    static LayoutUnit gCaretWidth(1);
    return gCaretWidth;
}

#if ENABLE(ASSERT)

LayoutObject::SetLayoutNeededForbiddenScope::SetLayoutNeededForbiddenScope(LayoutObject& layoutObject)
    : m_layoutObject(layoutObject)
    , m_preexistingForbidden(m_layoutObject.isSetNeedsLayoutForbidden())
{
    m_layoutObject.setNeedsLayoutIsForbidden(true);
}

LayoutObject::SetLayoutNeededForbiddenScope::~SetLayoutNeededForbiddenScope()
{
    m_layoutObject.setNeedsLayoutIsForbidden(m_preexistingForbidden);
}
#endif

struct SameSizeAsLayoutObject {
    virtual ~SameSizeAsLayoutObject() { } // Allocate vtable pointer.
    void* pointers[5];
#if ENABLE(ASSERT)
    unsigned m_debugBitfields : 2;
#endif
    unsigned m_bitfields;
    unsigned m_bitfields2;
    LayoutRect rect; // Stores the previous paint invalidation rect.
    LayoutPoint position; // Stores the previous position from the paint invalidation container.
};

static_assert(sizeof(LayoutObject) == sizeof(SameSizeAsLayoutObject), "LayoutObject should stay small");

bool LayoutObject::s_affectsParentBlock = false;

typedef HashMap<const LayoutObject*, LayoutRect> SelectionPaintInvalidationMap;
static SelectionPaintInvalidationMap* selectionPaintInvalidationMap = nullptr;

void* LayoutObject::operator new(size_t sz)
{
    ASSERT(isMainThread());
    return partitionAlloc(WTF::Partitions::layoutPartition(), sz);
}

void LayoutObject::operator delete(void* ptr)
{
    ASSERT(isMainThread());
    partitionFree(ptr);
}

LayoutObject* LayoutObject::createObject(Element* element, const ComputedStyle& style)
{
    ASSERT(isAllowedToModifyLayoutTreeStructure(element->document()));

    // Minimal support for content properties replacing an entire element.
    // Works only if we have exactly one piece of content and it's a URL.
    // Otherwise acts as if we didn't support this feature.
    const ContentData* contentData = style.contentData();
    if (contentData && !contentData->next() && contentData->isImage() && !element->isPseudoElement()) {
        LayoutImage* image = new LayoutImage(element);
        // LayoutImageResourceStyleImage requires a style being present on the image but we don't want to
        // trigger a style change now as the node is not fully attached. Moving this code to style change
        // doesn't make sense as it should be run once at layoutObject creation.
        image->setStyleInternal(const_cast<ComputedStyle*>(&style));
        if (const StyleImage* styleImage = toImageContentData(contentData)->image()) {
            image->setImageResource(LayoutImageResourceStyleImage::create(const_cast<StyleImage*>(styleImage)));
            image->setIsGeneratedContent();
        } else {
            image->setImageResource(LayoutImageResource::create());
        }
        image->setStyleInternal(nullptr);
        return image;
    }

    switch (style.display()) {
    case NONE:
        return nullptr;
    case INLINE:
        return new LayoutInline(element);
    case BLOCK:
    case INLINE_BLOCK:
        return new LayoutBlockFlow(element);
    case LIST_ITEM:
        return new LayoutListItem(element);
    case TABLE:
    case INLINE_TABLE:
        return new LayoutTable(element);
    case TABLE_ROW_GROUP:
    case TABLE_HEADER_GROUP:
    case TABLE_FOOTER_GROUP:
        return new LayoutTableSection(element);
    case TABLE_ROW:
        return new LayoutTableRow(element);
    case TABLE_COLUMN_GROUP:
    case TABLE_COLUMN:
        return new LayoutTableCol(element);
    case TABLE_CELL:
        return new LayoutTableCell(element);
    case TABLE_CAPTION:
        return new LayoutTableCaption(element);
    case BOX:
    case INLINE_BOX:
        return new LayoutDeprecatedFlexibleBox(*element);
    case FLEX:
    case INLINE_FLEX:
        return new LayoutFlexibleBox(element);
    case GRID:
    case INLINE_GRID:
        return new LayoutGrid(element);
    }

    return nullptr;
}

DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, layoutObjectCounter, ("LayoutObject"));

LayoutObject::LayoutObject(Node* node)
    : ImageResourceClient()
    , m_style(nullptr)
    , m_node(node)
    , m_parent(nullptr)
    , m_previous(nullptr)
    , m_next(nullptr)
#if ENABLE(ASSERT)
    , m_hasAXObject(false)
    , m_setNeedsLayoutForbidden(false)
#endif
    , m_bitfields(node)
{
#ifndef NDEBUG
    layoutObjectCounter.increment();
#endif
    InstanceCounters::incrementCounter(InstanceCounters::LayoutObjectCounter);
}

LayoutObject::~LayoutObject()
{
    ASSERT(!m_hasAXObject);
#ifndef NDEBUG
    layoutObjectCounter.decrement();
#endif
    InstanceCounters::decrementCounter(InstanceCounters::LayoutObjectCounter);
}

bool LayoutObject::isDescendantOf(const LayoutObject* obj) const
{
    for (const LayoutObject* r = this; r; r = r->m_parent) {
        if (r == obj)
            return true;
    }
    return false;
}

bool LayoutObject::isHR() const
{
    return isHTMLHRElement(node());
}

bool LayoutObject::isLegend() const
{
    return isHTMLLegendElement(node());
}

void LayoutObject::setIsInsideFlowThreadIncludingDescendants(bool insideFlowThread)
{
    LayoutObject* next;
    for (LayoutObject *object = this; object; object = next) {
        // If object is a fragmentation context it already updated the descendants flag accordingly.
        if (object->isLayoutFlowThread()) {
            next = object->nextInPreOrderAfterChildren(this);
            continue;
        }
        next = object->nextInPreOrder(this);
        ASSERT(insideFlowThread != object->isInsideFlowThread());
        object->setIsInsideFlowThread(insideFlowThread);
    }
}

bool LayoutObject::requiresAnonymousTableWrappers(const LayoutObject* newChild) const
{
    // Check should agree with:
    // CSS 2.1 Tables: 17.2.1 Anonymous table objects
    // http://www.w3.org/TR/CSS21/tables.html#anonymous-boxes
    if (newChild->isLayoutTableCol()) {
        const LayoutTableCol* newTableColumn = toLayoutTableCol(newChild);
        bool isColumnInColumnGroup = newTableColumn->isTableColumn() && isLayoutTableCol();
        return !isTable() && !isColumnInColumnGroup;
    }
    if (newChild->isTableCaption())
        return !isTable();
    if (newChild->isTableSection())
        return !isTable();
    if (newChild->isTableRow())
        return !isTableSection();
    if (newChild->isTableCell())
        return !isTableRow();
    return false;
}

void LayoutObject::addChild(LayoutObject* newChild, LayoutObject* beforeChild)
{
    ASSERT(isAllowedToModifyLayoutTreeStructure(document()));

    LayoutObjectChildList* children = virtualChildren();
    ASSERT(children);
    if (!children)
        return;

    if (requiresAnonymousTableWrappers(newChild)) {
        // Generate an anonymous table or reuse existing one from previous child
        // Per: 17.2.1 Anonymous table objects 3. Generate missing parents
        // http://www.w3.org/TR/CSS21/tables.html#anonymous-boxes
        LayoutTable* table;
        LayoutObject* afterChild = beforeChild ? beforeChild->previousSibling() : children->lastChild();
        if (afterChild && afterChild->isAnonymous() && afterChild->isTable() && !afterChild->isBeforeContent()) {
            table = toLayoutTable(afterChild);
        } else {
            table = LayoutTable::createAnonymousWithParent(this);
            addChild(table, beforeChild);
        }
        table->addChild(newChild);
    } else {
        children->insertChildNode(this, newChild, beforeChild);
    }

    if (newChild->isText() && newChild->style()->textTransform() == CAPITALIZE)
        toLayoutText(newChild)->transformText();

    // SVG creates layoutObjects for <g display="none">, as SVG requires children of hidden
    // <g>s to have layoutObjects - at least that's how our implementation works. Consider:
    // <g display="none"><foreignObject><body style="position: relative">FOO...
    // - layerTypeRequired() would return true for the <body>, creating a new Layer
    // - when the document is painted, both layers are painted. The <body> layer doesn't
    //   know that it's inside a "hidden SVG subtree", and thus paints, even if it shouldn't.
    // To avoid the problem alltogether, detect early if we're inside a hidden SVG subtree
    // and stop creating layers at all for these cases - they're not used anyways.
    if (newChild->hasLayer() && !layerCreationAllowedForSubtree())
        toLayoutBoxModelObject(newChild)->layer()->removeOnlyThisLayer();
}

void LayoutObject::removeChild(LayoutObject* oldChild)
{
    ASSERT(isAllowedToModifyLayoutTreeStructure(document()));

    LayoutObjectChildList* children = virtualChildren();
    ASSERT(children);
    if (!children)
        return;

    children->removeChildNode(this, oldChild);
}

void LayoutObject::registerSubtreeChangeListenerOnDescendants(bool value)
{
    // If we're set to the same value then we're done as that means it's
    // set down the tree that way already.
    if (m_bitfields.subtreeChangeListenerRegistered() == value)
        return;

    m_bitfields.setSubtreeChangeListenerRegistered(value);

    for (LayoutObject* curr = slowFirstChild(); curr; curr = curr->nextSibling())
        curr->registerSubtreeChangeListenerOnDescendants(value);
}

void LayoutObject::notifyAncestorsOfSubtreeChange()
{
    if (m_bitfields.notifiedOfSubtreeChange())
        return;

    m_bitfields.setNotifiedOfSubtreeChange(true);
    if (parent())
        parent()->notifyAncestorsOfSubtreeChange();
}

void LayoutObject::notifyOfSubtreeChange()
{
    if (!m_bitfields.subtreeChangeListenerRegistered())
        return;
    if (m_bitfields.notifiedOfSubtreeChange())
        return;

    notifyAncestorsOfSubtreeChange();

    // We can modify the layout tree during layout which means that we may
    // try to schedule this during performLayout. This should no longer
    // happen when crbug.com/370457 is fixed.
    DeprecatedScheduleStyleRecalcDuringLayout marker(document().lifecycle());
    document().scheduleLayoutTreeUpdateIfNeeded();
}

void LayoutObject::handleSubtreeModifications()
{
    ASSERT(wasNotifiedOfSubtreeChange());
    ASSERT(document().lifecycle().stateAllowsLayoutTreeNotifications());

    if (consumesSubtreeChangeNotification())
        subtreeDidChange();

    m_bitfields.setNotifiedOfSubtreeChange(false);

    for (LayoutObject* object = slowFirstChild(); object; object = object->nextSibling()) {
        if (!object->wasNotifiedOfSubtreeChange())
            continue;
        object->handleSubtreeModifications();
    }
}

LayoutObject* LayoutObject::nextInPreOrder() const
{
    if (LayoutObject* o = slowFirstChild())
        return o;

    return nextInPreOrderAfterChildren();
}

LayoutObject* LayoutObject::nextInPreOrderAfterChildren() const
{
    LayoutObject* o = nextSibling();
    if (!o) {
        o = parent();
        while (o && !o->nextSibling())
            o = o->parent();
        if (o)
            o = o->nextSibling();
    }

    return o;
}

LayoutObject* LayoutObject::nextInPreOrder(const LayoutObject* stayWithin) const
{
    if (LayoutObject* o = slowFirstChild())
        return o;

    return nextInPreOrderAfterChildren(stayWithin);
}

LayoutObject* LayoutObject::nextInPreOrderAfterChildren(const LayoutObject* stayWithin) const
{
    if (this == stayWithin)
        return nullptr;

    const LayoutObject* current = this;
    LayoutObject* next = current->nextSibling();
    for (; !next; next = current->nextSibling()) {
        current = current->parent();
        if (!current || current == stayWithin)
            return nullptr;
    }
    return next;
}

LayoutObject* LayoutObject::previousInPreOrder() const
{
    if (LayoutObject* o = previousSibling()) {
        while (LayoutObject* lastChild = o->slowLastChild())
            o = lastChild;
        return o;
    }

    return parent();
}

LayoutObject* LayoutObject::previousInPreOrder(const LayoutObject* stayWithin) const
{
    if (this == stayWithin)
        return nullptr;

    return previousInPreOrder();
}

LayoutObject* LayoutObject::childAt(unsigned index) const
{
    LayoutObject* child = slowFirstChild();
    for (unsigned i = 0; child && i < index; i++)
        child = child->nextSibling();
    return child;
}

LayoutObject* LayoutObject::lastLeafChild() const
{
    LayoutObject* r = slowLastChild();
    while (r) {
        LayoutObject* n = nullptr;
        n = r->slowLastChild();
        if (!n)
            break;
        r = n;
    }
    return r;
}

static void addLayers(LayoutObject* obj, DeprecatedPaintLayer* parentLayer, LayoutObject*& newObject,
    DeprecatedPaintLayer*& beforeChild)
{
    if (obj->hasLayer()) {
        if (!beforeChild && newObject) {
            // We need to figure out the layer that follows newObject. We only do
            // this the first time we find a child layer, and then we update the
            // pointer values for newObject and beforeChild used by everyone else.
            beforeChild = newObject->parent()->findNextLayer(parentLayer, newObject);
            newObject = nullptr;
        }
        parentLayer->addChild(toLayoutBoxModelObject(obj)->layer(), beforeChild);
        return;
    }

    for (LayoutObject* curr = obj->slowFirstChild(); curr; curr = curr->nextSibling())
        addLayers(curr, parentLayer, newObject, beforeChild);
}

void LayoutObject::addLayers(DeprecatedPaintLayer* parentLayer)
{
    if (!parentLayer)
        return;

    LayoutObject* object = this;
    DeprecatedPaintLayer* beforeChild = nullptr;
    blink::addLayers(this, parentLayer, object, beforeChild);
}

void LayoutObject::removeLayers(DeprecatedPaintLayer* parentLayer)
{
    if (!parentLayer)
        return;

    if (hasLayer()) {
        parentLayer->removeChild(toLayoutBoxModelObject(this)->layer());
        return;
    }

    for (LayoutObject* curr = slowFirstChild(); curr; curr = curr->nextSibling())
        curr->removeLayers(parentLayer);
}

void LayoutObject::moveLayers(DeprecatedPaintLayer* oldParent, DeprecatedPaintLayer* newParent)
{
    if (!newParent)
        return;

    if (hasLayer()) {
        DeprecatedPaintLayer* layer = toLayoutBoxModelObject(this)->layer();
        ASSERT(oldParent == layer->parent());
        if (oldParent)
            oldParent->removeChild(layer);
        newParent->addChild(layer);
        return;
    }

    for (LayoutObject* curr = slowFirstChild(); curr; curr = curr->nextSibling())
        curr->moveLayers(oldParent, newParent);
}

DeprecatedPaintLayer* LayoutObject::findNextLayer(DeprecatedPaintLayer* parentLayer, LayoutObject* startPoint, bool checkParent)
{
    // Error check the parent layer passed in. If it's null, we can't find anything.
    if (!parentLayer)
        return 0;

    // Step 1: If our layer is a child of the desired parent, then return our layer.
    DeprecatedPaintLayer* ourLayer = hasLayer() ? toLayoutBoxModelObject(this)->layer() : nullptr;
    if (ourLayer && ourLayer->parent() == parentLayer)
        return ourLayer;

    // Step 2: If we don't have a layer, or our layer is the desired parent, then descend
    // into our siblings trying to find the next layer whose parent is the desired parent.
    if (!ourLayer || ourLayer == parentLayer) {
        for (LayoutObject* curr = startPoint ? startPoint->nextSibling() : slowFirstChild();
            curr; curr = curr->nextSibling()) {
            DeprecatedPaintLayer* nextLayer = curr->findNextLayer(parentLayer, nullptr, false);
            if (nextLayer)
                return nextLayer;
        }
    }

    // Step 3: If our layer is the desired parent layer, then we're finished. We didn't
    // find anything.
    if (parentLayer == ourLayer)
        return nullptr;

    // Step 4: If |checkParent| is set, climb up to our parent and check its siblings that
    // follow us to see if we can locate a layer.
    if (checkParent && parent())
        return parent()->findNextLayer(parentLayer, this, true);

    return nullptr;
}

DeprecatedPaintLayer* LayoutObject::enclosingLayer() const
{
    for (const LayoutObject* current = this; current; current = current->parent()) {
        if (current->hasLayer())
            return toLayoutBoxModelObject(current)->layer();
    }
    // FIXME: we should get rid of detached layout subtrees, at which point this code should
    // not be reached. crbug.com/411429
    return nullptr;
}

bool LayoutObject::scrollRectToVisible(const LayoutRect& rect, const ScrollAlignment& alignX, const ScrollAlignment& alignY)
{
    LayoutBox* enclosingBox = this->enclosingBox();
    if (!enclosingBox)
        return false;

    enclosingBox->scrollRectToVisible(rect, alignX, alignY);
    return true;
}

LayoutBox* LayoutObject::enclosingBox() const
{
    LayoutObject* curr = const_cast<LayoutObject*>(this);
    while (curr) {
        if (curr->isBox())
            return toLayoutBox(curr);
        curr = curr->parent();
    }

    ASSERT_NOT_REACHED();
    return nullptr;
}

LayoutBoxModelObject* LayoutObject::enclosingBoxModelObject() const
{
    LayoutObject* curr = const_cast<LayoutObject*>(this);
    while (curr) {
        if (curr->isBoxModelObject())
            return toLayoutBoxModelObject(curr);
        curr = curr->parent();
    }

    ASSERT_NOT_REACHED();
    return nullptr;
}

LayoutBox* LayoutObject::enclosingScrollableBox() const
{
    for (LayoutObject* ancestor = parent(); ancestor; ancestor = ancestor->parent()) {
        if (!ancestor->isBox())
            continue;

        LayoutBox* ancestorBox = toLayoutBox(ancestor);
        if (ancestorBox->canBeScrolledAndHasScrollableArea())
            return ancestorBox;
    }

    return nullptr;
}

LayoutFlowThread* LayoutObject::locateFlowThreadContainingBlock() const
{
    ASSERT(isInsideFlowThread());

    // See if we have the thread cached because we're in the middle of layout.
    if (LayoutState* layoutState = view()->layoutState()) {
        if (LayoutFlowThread* flowThread = layoutState->flowThread())
            return flowThread;
    }

    // Not in the middle of layout so have to find the thread the slow way.
    LayoutObject* curr = const_cast<LayoutObject*>(this);
    while (curr) {
        if (curr->isSVG() && !curr->isSVGRoot())
            return nullptr;
        if (curr->isLayoutFlowThread())
            return toLayoutFlowThread(curr);
        LayoutObject* container = curr->container();
        curr = curr->parent();
        while (curr != container) {
            if (curr->isLayoutFlowThread()) {
                // The nearest ancestor flow thread isn't in our containing block chain. Then we
                // aren't really part of any flow thread, and we should stop looking. This happens
                // when there are out-of-flow objects or column spanners.
                return nullptr;
            }
            curr = curr->parent();
        }
    }
    return nullptr;
}

bool LayoutObject::skipInvalidationWhenLaidOutChildren() const
{
    if (!neededLayoutBecauseOfChildren())
        return false;

    // SVG layoutObjects need to be invalidated when their children are laid out.
    // LayoutBlocks with line boxes are responsible to invalidate them so we can't ignore them.
    if (isSVG() || (isLayoutBlockFlow() && toLayoutBlockFlow(this)->firstLineBox()))
        return false;

    // In case scrollbars got repositioned (which will typically happen if the layout object got
    // resized), we cannot skip invalidation.
    if (hasNonCompositedScrollbars())
        return false;

    return !hasBoxEffect();
}

LayoutBlock* LayoutObject::firstLineBlock() const
{
    return nullptr;
}

static inline bool objectIsRelayoutBoundary(const LayoutObject* object)
{
    // FIXME: In future it may be possible to broaden these conditions in order to improve performance.
    if (object->isTextControl())
        return true;

    if (object->isSVGRoot())
        return true;

    if (!object->hasOverflowClip())
        return false;

    if (object->style()->width().isIntrinsicOrAuto() || object->style()->height().isIntrinsicOrAuto() || object->style()->height().hasPercent())
        return false;

    // Table parts can't be relayout roots since the table is responsible for layouting all the parts.
    if (object->isTablePart())
        return false;

    return true;
}

void LayoutObject::markContainerChainForLayout(bool scheduleRelayout, SubtreeLayoutScope* layouter)
{
    ASSERT(!isSetNeedsLayoutForbidden());
    ASSERT(!layouter || this != layouter->root());

    LayoutObject* object = container();
    LayoutObject* last = this;

    bool simplifiedNormalFlowLayout = needsSimplifiedNormalFlowLayout() && !selfNeedsLayout() && !normalChildNeedsLayout();

    while (object) {
        if (object->selfNeedsLayout())
            return;

        // Don't mark the outermost object of an unrooted subtree. That object will be
        // marked when the subtree is added to the document.
        LayoutObject* container = object->container();
        if (!container && !object->isLayoutView())
            return;
        if (!last->isTextOrSVGChild() && last->style()->hasOutOfFlowPosition()) {
            bool willSkipRelativelyPositionedInlines = !object->isLayoutBlock() || object->isAnonymousBlock();
            // Skip relatively positioned inlines and anonymous blocks to get to the enclosing LayoutBlock.
            while (object && (!object->isLayoutBlock() || object->isAnonymousBlock()))
                object = object->container();
            if (!object || object->posChildNeedsLayout())
                return;
            if (willSkipRelativelyPositionedInlines)
                container = object->container();
            object->setPosChildNeedsLayout(true);
            simplifiedNormalFlowLayout = true;
            ASSERT(!object->isSetNeedsLayoutForbidden());
        } else if (simplifiedNormalFlowLayout) {
            if (object->needsSimplifiedNormalFlowLayout())
                return;
            object->setNeedsSimplifiedNormalFlowLayout(true);
            ASSERT(!object->isSetNeedsLayoutForbidden());
        } else {
            if (object->normalChildNeedsLayout())
                return;
            object->setNormalChildNeedsLayout(true);
            ASSERT(!object->isSetNeedsLayoutForbidden());
        }

        if (layouter) {
            layouter->recordObjectMarkedForLayout(object);
            if (object == layouter->root())
                return;
        }

        last = object;
        if (scheduleRelayout && objectIsRelayoutBoundary(last))
            break;
        object = container;
    }

    if (scheduleRelayout)
        last->scheduleRelayout();
}

#if ENABLE(ASSERT)
void LayoutObject::checkBlockPositionedObjectsNeedLayout()
{
    ASSERT(!needsLayout());

    if (isLayoutBlock())
        toLayoutBlock(this)->checkPositionedObjectsNeedLayout();
}
#endif

void LayoutObject::setPreferredLogicalWidthsDirty(MarkingBehavior markParents)
{
    m_bitfields.setPreferredLogicalWidthsDirty(true);
    if (markParents == MarkContainerChain && (isText() || !style()->hasOutOfFlowPosition()))
        invalidateContainerPreferredLogicalWidths();
}

void LayoutObject::clearPreferredLogicalWidthsDirty()
{
    m_bitfields.setPreferredLogicalWidthsDirty(false);
}

inline void LayoutObject::invalidateContainerPreferredLogicalWidths()
{
    // In order to avoid pathological behavior when inlines are deeply nested, we do include them
    // in the chain that we mark dirty (even though they're kind of irrelevant).
    LayoutObject* o = isTableCell() ? containingBlock() : container();
    while (o && !o->preferredLogicalWidthsDirty()) {
        // Don't invalidate the outermost object of an unrooted subtree. That object will be
        // invalidated when the subtree is added to the document.
        LayoutObject* container = o->isTableCell() ? o->containingBlock() : o->container();
        if (!container && !o->isLayoutView())
            break;

        o->m_bitfields.setPreferredLogicalWidthsDirty(true);
        if (o->style()->hasOutOfFlowPosition()) {
            // A positioned object has no effect on the min/max width of its containing block ever.
            // We can optimize this case and not go up any further.
            break;
        }
        o = container;
    }
}

LayoutBlock* LayoutObject::containerForFixedPosition(const LayoutBoxModelObject* paintInvalidationContainer, bool* paintInvalidationContainerSkipped) const
{
    ASSERT(!paintInvalidationContainerSkipped || !*paintInvalidationContainerSkipped);
    ASSERT(!isText());
    ASSERT(style()->position() == FixedPosition);

    LayoutObject* ancestor = parent();
    for (; ancestor && !ancestor->canContainFixedPositionObjects(); ancestor = ancestor->parent()) {
        if (paintInvalidationContainerSkipped && ancestor == paintInvalidationContainer)
            *paintInvalidationContainerSkipped = true;
    }

    ASSERT(!ancestor || !ancestor->isAnonymousBlock());
    return toLayoutBlock(ancestor);
}

LayoutObject* LayoutObject::containerForAbsolutePosition(const LayoutBoxModelObject* paintInvalidationContainer, bool* paintInvalidationContainerSkipped) const
{
    // We technically just want our containing block, but
    // we may not have one if we're part of an uninstalled
    // subtree. We'll climb as high as we can though.
    for (LayoutObject* object = parent(); object; object = object->parent()) {
        if (object->style()->position() != StaticPosition)
            return object;

        if (object->canContainFixedPositionObjects())
            return object;

        if (paintInvalidationContainerSkipped && object == paintInvalidationContainer)
            *paintInvalidationContainerSkipped = true;
    }
    return nullptr;
}

LayoutBlock* LayoutObject::containingBlockForAbsolutePosition() const
{
    // https://chromium.googlesource.com/chromium/src/+log/87c6ba998684a8e2f6866e55e8d6ee5229cbde36/third_party/WebKit/Source/core/layout/LayoutObject.cpp?s=6fe9d9762eb66a24bbbd3a5c2badf6e5ea98d1e2
    // https://chromium.googlesource.com/chromium/src/+/6f9a69691d825e9bb62802b2ea5ac65ed29eb0ba%5E%21/#F0
    LayoutObject* o = containerForAbsolutePosition();
    // For relpositioned inlines, we return the nearest non-anonymous enclosing block. We don't try
    // to return the inline itself.  This allows us to avoid having a positioned objects
    // list in all LayoutInlines and lets us return a strongly-typed LayoutBlock* result
    // from this method.  The container() method can actually be used to obtain the
    // inline directly.
    if (o && o->isInline() && !o->isReplaced()) {
        ASSERT(o->style()->hasInFlowPosition());
        o = o->containingBlock();
    }

    if (o && !o->isLayoutBlock())
        o = o->containingBlock();

    while (o && o->isAnonymousBlock())
        o = o->containingBlock();

    if (!o || !o->isLayoutBlock())
        return nullptr; // This can still happen in case of an orphaned tree

    return toLayoutBlock(o);
}

LayoutBlock* LayoutObject::containingBlock() const
{
    LayoutObject* o = parent();
    if (!o && isLayoutScrollbarPart())
        o = toLayoutScrollbarPart(this)->layoutObjectOwningScrollbar();
    if (!isTextOrSVGChild()) {
        if (m_style->position() == FixedPosition)
            return containerForFixedPosition();
        if (m_style->position() == AbsolutePosition)
            return containingBlockForAbsolutePosition();
    }
    if (isColumnSpanAll()) {
        o = spannerPlaceholder()->containingBlock();
    } else {
        while (o && ((o->isInline() && !o->isReplaced()) || !o->isLayoutBlock()))
            o = o->parent();
    }

    if (!o || !o->isLayoutBlock())
        return nullptr; // This can still happen in case of an orphaned tree

    return toLayoutBlock(o);
}

bool LayoutObject::mustInvalidateFillLayersPaintOnHeightChange(const FillLayer& layer) const
{
    // Nobody will use multiple layers without wanting fancy positioning.
    if (layer.next())
        return true;

    // Make sure we have a valid image.
    StyleImage* img = layer.image();
    if (!img || !img->canRender(*this, style()->effectiveZoom()))
        return false;

    if (layer.repeatY() != RepeatFill && layer.repeatY() != NoRepeatFill)
        return true;

    // TODO(alancutter): Make this work correctly for calc lengths.
    if (layer.yPosition().hasPercent() && !layer.yPosition().isZero())
        return true;

    if (layer.backgroundYOrigin() != TopEdge)
        return true;

    EFillSizeType sizeType = layer.sizeType();

    if (sizeType == Contain || sizeType == Cover)
        return true;

    if (sizeType == SizeLength) {
        // TODO(alancutter): Make this work correctly for calc lengths.
        if (layer.sizeLength().height().hasPercent() && !layer.sizeLength().height().isZero())
            return true;
        if (img->isGeneratedImage() && layer.sizeLength().height().isAuto())
            return true;
    } else if (img->usesImageContainerSize()) {
        return true;
    }

    return false;
}

FloatRect LayoutObject::absoluteBoundingBoxFloatRect() const
{
    Vector<FloatQuad> quads;
    absoluteQuads(quads);

    size_t n = quads.size();
    if (n == 0)
        return FloatRect();

    FloatRect result = quads[0].boundingBox();
    for (size_t i = 1; i < n; ++i)
        result.unite(quads[i].boundingBox());
    return result;
}

IntRect LayoutObject::absoluteBoundingBoxRect() const
{
    Vector<FloatQuad> quads;
    absoluteQuads(quads);

    size_t n = quads.size();
    if (!n)
        return IntRect();

    IntRect result = quads[0].enclosingBoundingBox();
    for (size_t i = 1; i < n; ++i)
        result.unite(quads[i].enclosingBoundingBox());
    return result;
}

IntRect LayoutObject::absoluteBoundingBoxRectIgnoringTransforms() const
{
    FloatPoint absPos = localToAbsolute();
    Vector<IntRect> rects;
    absoluteRects(rects, flooredLayoutPoint(absPos));

    size_t n = rects.size();
    if (!n)
        return IntRect();

    IntRect result = rects[0];
    for (size_t i = 1; i < n; ++i)
        result.unite(rects[i]);
    return result;
}

IntRect LayoutObject::absoluteFocusRingBoundingBoxRect() const
{
    Vector<LayoutRect> rects;
    const LayoutBoxModelObject* container = enclosingLayer()->layoutObject();
    addFocusRingRects(rects, LayoutPoint(localToContainerPoint(FloatPoint(), container)));
    return container->localToAbsoluteQuad(FloatQuad(unionRect(rects))).enclosingBoundingBox();
}

FloatRect LayoutObject::absoluteBoundingBoxRectForRange(const Range* range)
{
    if (!range || !range->startContainer())
        return FloatRect();

    range->ownerDocument().updateLayout();

    Vector<FloatQuad> quads;
    range->textQuads(quads);

    FloatRect result;
    for (size_t i = 0; i < quads.size(); ++i)
        result.unite(quads[i].boundingBox());

    return result;
}

void LayoutObject::addAbsoluteRectForLayer(IntRect& result)
{
    if (hasLayer())
        result.unite(absoluteBoundingBoxRect());
    for (LayoutObject* current = slowFirstChild(); current; current = current->nextSibling())
        current->addAbsoluteRectForLayer(result);
}

IntRect LayoutObject::paintingRootRect(IntRect& topLevelRect)
{
    IntRect result = absoluteBoundingBoxRect();
    for (LayoutObject* current = slowFirstChild(); current; current = current->nextSibling())
        current->addAbsoluteRectForLayer(result);
    return result;
}

void LayoutObject::paint(const PaintInfo&, const LayoutPoint&)
{
}

const LayoutBoxModelObject* LayoutObject::containerForPaintInvalidation() const
{
    RELEASE_ASSERT(isRooted());
    return adjustCompositedContainerForSpecialAncestors(enclosingCompositedContainer());
}

const LayoutBoxModelObject& LayoutObject::containerForPaintInvalidationOnRootedTree() const
{
    RELEASE_ASSERT(isRooted());

    const LayoutBoxModelObject* paintInvalidationContainer = containerForPaintInvalidation();
    ASSERT(paintInvalidationContainer);

    return *paintInvalidationContainer;
}

const LayoutBoxModelObject* LayoutObject::enclosingCompositedContainer() const
{
    LayoutBoxModelObject* container = nullptr;
    // FIXME: CompositingState is not necessarily up to date for many callers of this function.
    DisableCompositingQueryAsserts disabler;

    if (DeprecatedPaintLayer* compositingLayer = enclosingLayer()->enclosingLayerForPaintInvalidationCrossingFrameBoundaries())
        container = compositingLayer->layoutObject();
    return container;
}

const LayoutBoxModelObject* LayoutObject::adjustCompositedContainerForSpecialAncestors(const LayoutBoxModelObject* paintInvalidationContainer) const
{
    if (paintInvalidationContainer)
        return paintInvalidationContainer;

    LayoutView* layoutView = view();
    while (layoutView->frame()->ownerLayoutObject())
        layoutView = layoutView->frame()->ownerLayoutObject()->view();
    return layoutView;
}

String LayoutObject::decoratedName() const
{
    StringBuilder name;
    name.append(this->name());

    if (isAnonymous())
        name.append(" (anonymous)");
    // FIXME: Remove the special case for LayoutView here (requires rebaseline of all tests).
    if (isOutOfFlowPositioned() && !isLayoutView())
        name.append(" (positioned)");
    if (isRelPositioned())
        name.append(" (relative positioned)");
    if (isFloating())
        name.append(" (floating)");
    if (spannerPlaceholder())
        name.append(" (column spanner)");

    return name.toString();
}

String LayoutObject::debugName() const
{
    StringBuilder name;
    name.append(decoratedName());

    if (const Node* node = this->node()) {
        name.append(' ');
        name.append(node->debugName());
    }
    return name.toString();
}

bool LayoutObject::isPaintInvalidationContainer() const
{
    return hasLayer() && toLayoutBoxModelObject(this)->layer()->isPaintInvalidationContainer();
}

template <typename T>
void addJsonObjectForRect(TracedValue* value, const char* name, const T& rect)
{
    value->beginDictionary(name);
    value->setDouble("x", rect.x());
    value->setDouble("y", rect.y());
    value->setDouble("width", rect.width());
    value->setDouble("height", rect.height());
    value->endDictionary();
}

template <typename T>
void addJsonObjectForPoint(TracedValue* value, const char* name, const T& point)
{
    value->beginDictionary(name);
    value->setDouble("x", point.x());
    value->setDouble("y", point.y());
    value->endDictionary();
}

static PassRefPtr<TraceEvent::ConvertableToTraceFormat> jsonObjectForPaintInvalidationInfo(const LayoutRect& rect, const String& invalidationReason)
{
    RefPtr<TracedValue> value = TracedValue::create();
    addJsonObjectForRect(value.get(), "rect", rect);
    value->setString("invalidation_reason", invalidationReason);
    return value;
}

LayoutRect LayoutObject::computePaintInvalidationRect(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* paintInvalidationState) const
{
    return clippedOverflowRectForPaintInvalidation(paintInvalidationContainer, paintInvalidationState);
}

void LayoutObject::invalidatePaintUsingContainer(const LayoutBoxModelObject& paintInvalidationContainer, const LayoutRect& r, PaintInvalidationReason invalidationReason) const
{
    ASSERT(gDisablePaintInvalidationStateAsserts || document().lifecycle().state() == DocumentLifecycle::InPaintInvalidation);

    if (r.isEmpty())
        return;

    RELEASE_ASSERT(isRooted());

    // FIXME: Unify "devtools.timeline.invalidationTracking" and "blink.invalidation". crbug.com/413527.
    TRACE_EVENT_INSTANT1(TRACE_DISABLED_BY_DEFAULT("devtools.timeline.invalidationTracking"),
        "PaintInvalidationTracking",
        TRACE_EVENT_SCOPE_THREAD,
        "data", InspectorPaintInvalidationTrackingEvent::data(this, paintInvalidationContainer));
    TRACE_EVENT2(TRACE_DISABLED_BY_DEFAULT("blink.invalidation"), "LayoutObject::invalidatePaintUsingContainer()",
        "object", this->debugName().ascii(),
        "info", jsonObjectForPaintInvalidationInfo(r, paintInvalidationReasonToString(invalidationReason)));

    if (paintInvalidationContainer.isLayoutView()) {
        toLayoutView(&paintInvalidationContainer)->invalidatePaintForRectangle(r, invalidationReason);
        return;
    }

    if (paintInvalidationContainer.view()->usesCompositing()) {
        ASSERT(paintInvalidationContainer.isPaintInvalidationContainer());
        paintInvalidationContainer.setBackingNeedsPaintInvalidationInRect(r, invalidationReason);
    }
}

void LayoutObject::invalidateDisplayItemClient(const DisplayItemClientWrapper& displayItemClient) const
{
    if (!RuntimeEnabledFeatures::slimmingPaintEnabled())
        return;

    // Not using enclosingCompositedContainer() directly because this object may be in an orphaned subtree.
    if (const DeprecatedPaintLayer* enclosingLayer = this->enclosingLayer()) {
        // This is valid because we want to invalidate the client in the display item list of the current backing.
        DisableCompositingQueryAsserts disabler;
        if (const DeprecatedPaintLayer* paintInvalidationLayer = enclosingLayer->enclosingLayerForPaintInvalidationCrossingFrameBoundaries())
            paintInvalidationLayer->layoutObject()->invalidateDisplayItemClientOnBacking(displayItemClient);
    }
}

void LayoutObject::invalidateDisplayItemClients(const LayoutBoxModelObject& paintInvalidationContainer) const
{
    ASSERT(RuntimeEnabledFeatures::slimmingPaintEnabled());
    paintInvalidationContainer.invalidateDisplayItemClientOnBacking(*this);
}

LayoutRect LayoutObject::boundsRectForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, const PaintInvalidationState* paintInvalidationState) const
{
    if (!paintInvalidationContainer)
        return computePaintInvalidationRect(paintInvalidationContainer, paintInvalidationState);
    return DeprecatedPaintLayer::computePaintInvalidationRect(this, paintInvalidationContainer->layer(), paintInvalidationState);
}

const LayoutBoxModelObject* LayoutObject::invalidatePaintRectangleInternal(const LayoutRect& r) const
{
    RELEASE_ASSERT(isRooted());

    if (r.isEmpty())
        return nullptr;

    if (view()->document().printing())
        return nullptr; // Don't invalidate paints if we're printing.

    LayoutRect dirtyRect(r);

    const LayoutBoxModelObject& paintInvalidationContainer = containerForPaintInvalidationOnRootedTree();
    DeprecatedPaintLayer::mapRectToPaintInvalidationBacking(this, &paintInvalidationContainer, dirtyRect);
    invalidatePaintUsingContainer(paintInvalidationContainer, dirtyRect, PaintInvalidationRectangle);
    return &paintInvalidationContainer;
}

void LayoutObject::invalidatePaintRectangle(const LayoutRect& r) const
{
    if (const LayoutBoxModelObject* paintInvalidationContainer = invalidatePaintRectangleInternal(r)) {
        if (RuntimeEnabledFeatures::slimmingPaintEnabled())
            invalidateDisplayItemClients(*paintInvalidationContainer);
    }
}

void LayoutObject::invalidateTreeIfNeeded(PaintInvalidationState& paintInvalidationState)
{
    ASSERT(!needsLayout());

    // If we didn't need paint invalidation then our children don't need as well.
    // Skip walking down the tree as everything should be fine below us.
    if (!shouldCheckForPaintInvalidation(paintInvalidationState))
        return;

    PaintInvalidationReason reason = invalidatePaintIfNeeded(paintInvalidationState, paintInvalidationState.paintInvalidationContainer());
    clearPaintInvalidationState(paintInvalidationState);

    if (reason == PaintInvalidationDelayedFull)
        paintInvalidationState.pushDelayedPaintInvalidationTarget(*this);

    invalidatePaintOfSubtreesIfNeeded(paintInvalidationState);
}

void LayoutObject::invalidatePaintOfSubtreesIfNeeded(PaintInvalidationState& childPaintInvalidationState)
{
    for (LayoutObject* child = slowFirstChild(); child; child = child->nextSibling()) {
        if (!child->isOutOfFlowPositioned())
            child->invalidateTreeIfNeeded(childPaintInvalidationState);
    }
}

static PassRefPtr<TraceEvent::ConvertableToTraceFormat> jsonObjectForOldAndNewRects(const LayoutRect& oldRect, const LayoutPoint& oldLocation, const LayoutRect& newRect, const LayoutPoint& newLocation)
{
    RefPtr<TracedValue> value = TracedValue::create();
    addJsonObjectForRect(value.get(), "oldRect", oldRect);
    addJsonObjectForPoint(value.get(), "oldLocation", oldLocation);
    addJsonObjectForRect(value.get(), "newRect", newRect);
    addJsonObjectForPoint(value.get(), "newLocation", newLocation);
    return value;
}

LayoutRect LayoutObject::selectionRectInViewCoordinates() const
{
    return selectionRectForPaintInvalidation(view());
}

LayoutRect LayoutObject::previousSelectionRectForPaintInvalidation() const
{
    if (!selectionPaintInvalidationMap)
        return LayoutRect();

    return selectionPaintInvalidationMap->get(this);
}

void LayoutObject::setPreviousSelectionRectForPaintInvalidation(const LayoutRect& selectionRect)
{
    if (!selectionPaintInvalidationMap) {
        if (selectionRect.isEmpty())
            return;
        selectionPaintInvalidationMap = new SelectionPaintInvalidationMap();
    }

    if (selectionRect.isEmpty())
        selectionPaintInvalidationMap->remove(this);
    else
        selectionPaintInvalidationMap->set(this, selectionRect);
}

inline void LayoutObject::invalidateSelectionIfNeeded(const LayoutBoxModelObject& paintInvalidationContainer, PaintInvalidationReason invalidationReason)
{
    // Update selection rect when we are doing full invalidation (in case that the object is moved, composite status changed, etc.)
    // or shouldInvalidationSelection is set (in case that the selection itself changed).
    bool fullInvalidation = view()->doingFullPaintInvalidation() || isFullPaintInvalidationReason(invalidationReason);
    if (!fullInvalidation && !shouldInvalidateSelection())
        return;

    LayoutRect oldSelectionRect = previousSelectionRectForPaintInvalidation();
    LayoutRect newSelectionRect = selectionRectForPaintInvalidation(&paintInvalidationContainer);
    setPreviousSelectionRectForPaintInvalidation(newSelectionRect);

    if (RuntimeEnabledFeatures::slimmingPaintEnabled() && shouldInvalidateSelection())
        invalidateDisplayItemClients(paintInvalidationContainer);

    if (fullInvalidation)
        return;

    fullyInvalidatePaint(paintInvalidationContainer, PaintInvalidationSelection, oldSelectionRect, newSelectionRect);
}

PaintInvalidationReason LayoutObject::invalidatePaintIfNeeded(PaintInvalidationState& paintInvalidationState, const LayoutBoxModelObject& paintInvalidationContainer)
{
    LayoutView* v = view();
    if (v->document().printing())
        return PaintInvalidationNone; // Don't invalidate paints if we're printing.

    const LayoutRect oldBounds = previousPaintInvalidationRect();
    const LayoutPoint oldLocation = previousPositionFromPaintInvalidationBacking();
    const LayoutRect newBounds = boundsRectForPaintInvalidation(&paintInvalidationContainer, &paintInvalidationState);
    const LayoutPoint newLocation = DeprecatedPaintLayer::positionFromPaintInvalidationBacking(this, &paintInvalidationContainer, &paintInvalidationState);
    setPreviousPaintInvalidationRect(newBounds);
    setPreviousPositionFromPaintInvalidationBacking(newLocation);

    PaintInvalidationReason invalidationReason = paintInvalidationReason(paintInvalidationContainer, oldBounds, oldLocation, newBounds, newLocation);

    // We need to invalidate the selection before checking for whether we are doing a full invalidation.
    // This is because we need to update the old rect regardless.
    invalidateSelectionIfNeeded(paintInvalidationContainer, invalidationReason);

    // If we are set to do a full paint invalidation that means the LayoutView will issue
    // paint invalidations. We can then skip issuing of paint invalidations for the child
    // layoutObjects as they'll be covered by the LayoutView.
    // However, slimming paint mode requires paint invalidation of the child layoutObjects.
    if (view()->doingFullPaintInvalidation() && !RuntimeEnabledFeatures::slimmingPaintEnabled())
        return invalidationReason;

    TRACE_EVENT2(TRACE_DISABLED_BY_DEFAULT("blink.invalidation"), "LayoutObject::invalidatePaintIfNeeded()",
        "object", this->debugName().ascii(),
        "info", jsonObjectForOldAndNewRects(oldBounds, oldLocation, newBounds, newLocation));

    if (RuntimeEnabledFeatures::slimmingPaintEnabled()) {
        bool boxDecorationBackgroundObscured = boxDecorationBackgroundIsKnownToBeObscured();
        if (!isFullPaintInvalidationReason(invalidationReason) && boxDecorationBackgroundObscured != m_bitfields.lastBoxDecorationBackgroundObscured())
            invalidationReason = PaintInvalidationBackgroundObscurationChange;
        m_bitfields.setLastBoxDecorationBackgroundObscured(boxDecorationBackgroundObscured);
    }

    if (invalidationReason == PaintInvalidationNone) {
        // TODO(trchen): Currently we don't keep track of paint offset of layout objects.
        // There are corner cases that the display items need to be invalidated for paint offset
        // mutation, but incurs no pixel difference (i.e. bounds stay the same) so no rect-based
        // invalidation is issued. See crbug.com/508383 and crbug.com/515977.
        // This is a workaround to force display items to update paint offset.
        if (RuntimeEnabledFeatures::slimmingPaintEnabled() && paintInvalidationState.ancestorHadPaintInvalidationForLocationChange())
            invalidateDisplayItemClients(paintInvalidationContainer);

        return invalidationReason;
    }

    if (RuntimeEnabledFeatures::slimmingPaintEnabled())
        invalidateDisplayItemClients(paintInvalidationContainer);

    if (invalidationReason == PaintInvalidationIncremental) {
        incrementallyInvalidatePaint(paintInvalidationContainer, oldBounds, newBounds, newLocation);
        return invalidationReason;
    }

    fullyInvalidatePaint(paintInvalidationContainer, invalidationReason, oldBounds, newBounds);
    return invalidationReason;
}

PaintInvalidationReason LayoutObject::paintInvalidationReason(const LayoutBoxModelObject& paintInvalidationContainer,
    const LayoutRect& oldBounds, const LayoutPoint& oldPositionFromPaintInvalidationBacking,
    const LayoutRect& newBounds, const LayoutPoint& newPositionFromPaintInvalidationBacking) const
{
    // First check for InvalidationLocationChange to avoid it from being hidden by other
    // invalidation reasons because we'll need to force check for paint invalidation for
    // children when location of this object changed.
    if (newPositionFromPaintInvalidationBacking != oldPositionFromPaintInvalidationBacking)
        return PaintInvalidationLocationChange;

    if (shouldDoFullPaintInvalidation())
        return m_bitfields.fullPaintInvalidationReason();

    // The focus ring may change because of position change of descendants. For simplicity,
    // just force full paint invalidation if this object is marked for checking paint invalidation
    // for any reason.
    if (styleRef().outlineStyleIsAuto())
        return PaintInvalidationFocusRing;

    // If the bounds are the same then we know that none of the statements below
    // can match, so we can early out since we will not need to do any
    // invalidation.
    if (oldBounds == newBounds)
        return PaintInvalidationNone;

    // If we shifted, we don't know the exact reason so we are conservative and trigger a full invalidation. Shifting could
    // be caused by some layout property (left / top) or some in-flow layoutObject inserted / removed before us in the tree.
    if (newBounds.location() != oldBounds.location())
        return PaintInvalidationBoundsChange;

    // This covers the case where we mark containing blocks for layout
    // and they change size but don't have anything to paint. This is
    // a pretty common case for <body> as we add / remove children
    // (and the default background is done by FrameView).
    if (skipInvalidationWhenLaidOutChildren())
        return PaintInvalidationNone;

    // If the size is zero on one of our bounds then we know we're going to have
    // to do a full invalidation of either old bounds or new bounds. If we fall
    // into the incremental invalidation we'll issue two invalidations instead
    // of one.
    if (oldBounds.isEmpty())
        return PaintInvalidationBecameVisible;
    if (newBounds.isEmpty())
        return PaintInvalidationBecameInvisible;

    return PaintInvalidationIncremental;
}

void LayoutObject::incrementallyInvalidatePaint(const LayoutBoxModelObject& paintInvalidationContainer, const LayoutRect& oldBounds, const LayoutRect& newBounds, const LayoutPoint& positionFromPaintInvalidationBacking)
{
    ASSERT(oldBounds.location() == newBounds.location());

    LayoutUnit deltaRight = newBounds.maxX() - oldBounds.maxX();
    if (deltaRight > 0)
        invalidatePaintUsingContainer(paintInvalidationContainer, LayoutRect(oldBounds.maxX(), newBounds.y(), deltaRight, newBounds.height()), PaintInvalidationIncremental);
    else if (deltaRight < 0)
        invalidatePaintUsingContainer(paintInvalidationContainer, LayoutRect(newBounds.maxX(), oldBounds.y(), -deltaRight, oldBounds.height()), PaintInvalidationIncremental);

    LayoutUnit deltaBottom = newBounds.maxY() - oldBounds.maxY();
    if (deltaBottom > 0)
        invalidatePaintUsingContainer(paintInvalidationContainer, LayoutRect(newBounds.x(), oldBounds.maxY(), newBounds.width(), deltaBottom), PaintInvalidationIncremental);
    else if (deltaBottom < 0)
        invalidatePaintUsingContainer(paintInvalidationContainer, LayoutRect(oldBounds.x(), newBounds.maxY(), oldBounds.width(), -deltaBottom), PaintInvalidationIncremental);
}

void LayoutObject::fullyInvalidatePaint(const LayoutBoxModelObject& paintInvalidationContainer, PaintInvalidationReason invalidationReason, const LayoutRect& oldBounds, const LayoutRect& newBounds)
{
    // Otherwise do full paint invalidation.
    invalidatePaintUsingContainer(paintInvalidationContainer, oldBounds, invalidationReason);
    if (newBounds != oldBounds)
        invalidatePaintUsingContainer(paintInvalidationContainer, newBounds, invalidationReason);
}

void LayoutObject::invalidatePaintForOverflow()
{
}

void LayoutObject::invalidatePaintForOverflowIfNeeded()
{
    if (shouldInvalidateOverflowForPaint())
        invalidatePaintForOverflow();
}

LayoutRect LayoutObject::rectWithOutlineForPaintInvalidation(const LayoutBoxModelObject* paintInvalidationContainer, LayoutUnit outlineWidth, const PaintInvalidationState* paintInvalidationState) const
{
    LayoutRect r(clippedOverflowRectForPaintInvalidation(paintInvalidationContainer, paintInvalidationState));
    r.inflate(outlineWidth);
    return r;
}

LayoutRect LayoutObject::absoluteClippedOverflowRect() const
{
    return clippedOverflowRectForPaintInvalidation(view());
}

LayoutRect LayoutObject::clippedOverflowRectForPaintInvalidation(const LayoutBoxModelObject*, const PaintInvalidationState*) const
{
    ASSERT_NOT_REACHED();
    return LayoutRect();
}

void LayoutObject::mapRectToPaintInvalidationBacking(const LayoutBoxModelObject* paintInvalidationContainer, LayoutRect& rect, const PaintInvalidationState* paintInvalidationState) const
{
    if (paintInvalidationContainer == this)
        return;

    if (paintInvalidationState && paintInvalidationState->canMapToContainer(paintInvalidationContainer)) {
        rect.move(paintInvalidationState->paintOffset());
        if (paintInvalidationState->isClipped())
            rect.intersect(paintInvalidationState->clipRect());
        return;
    }

    if (LayoutObject* o = parent()) {
        if (o->hasOverflowClip()) {
            LayoutBox* boxParent = toLayoutBox(o);
            boxParent->applyCachedClipAndScrollOffsetForPaintInvalidation(rect);
            if (rect.isEmpty())
                return;
        }

        o->mapRectToPaintInvalidationBacking(paintInvalidationContainer, rect, paintInvalidationState);
    }
}

void LayoutObject::dirtyLinesFromChangedChild(LayoutObject*)
{
}

#ifndef NDEBUG

void LayoutObject::showTreeForThis() const
{
    if (node())
        node()->showTreeForThis();
}

void LayoutObject::showLayoutTreeForThis() const
{
    showLayoutTree(this, 0);
}

void LayoutObject::showLineTreeForThis() const
{
    if (containingBlock())
        containingBlock()->showLineTreeAndMark(0, 0, 0, 0, this);
}

void LayoutObject::showLayoutObject() const
{
    showLayoutObject(0);
}

void LayoutObject::showLayoutObject(int printedCharacters) const
{
    printedCharacters += fprintf(stderr, "%s %p", decoratedName().ascii().data(), this);

    if (isText() && toLayoutText(this)->isTextFragment())
        printedCharacters += fprintf(stderr, " \"%s\" ", toLayoutText(this)->text().ascii().data());

    if (virtualContinuation())
        printedCharacters += fprintf(stderr, " continuation=%p", virtualContinuation());

    if (node()) {
        if (printedCharacters)
            for (; printedCharacters < showTreeCharacterOffset; printedCharacters++)
                fputc(' ', stderr);
        fputc('\t', stderr);
        node()->showNode();
    } else {
        fputc('\n', stderr);
    }
}

void LayoutObject::showLayoutTreeAndMark(const LayoutObject* markedObject1, const char* markedLabel1, const LayoutObject* markedObject2, const char* markedLabel2, int depth) const
{
    int printedCharacters = 0;
    if (markedObject1 == this && markedLabel1)
        printedCharacters += fprintf(stderr, "%s", markedLabel1);
    if (markedObject2 == this && markedLabel2)
        printedCharacters += fprintf(stderr, "%s", markedLabel2);
    for (; printedCharacters < depth * 2; printedCharacters++)
        fputc(' ', stderr);

    showLayoutObject(printedCharacters);

    for (const LayoutObject* child = slowFirstChild(); child; child = child->nextSibling())
        child->showLayoutTreeAndMark(markedObject1, markedLabel1, markedObject2, markedLabel2, depth + 1);
}

#endif // NDEBUG

bool LayoutObject::isSelectable() const
{
    return !isInert() && !(style()->userSelect() == SELECT_NONE && style()->userModify() == READ_ONLY);
}

Color LayoutObject::selectionBackgroundColor() const
{
    if (!isSelectable())
        return Color::transparent;

    if (RefPtr<ComputedStyle> pseudoStyle = getUncachedPseudoStyleFromParentOrShadowHost())
        return resolveColor(*pseudoStyle, CSSPropertyBackgroundColor).blendWithWhite();
    return frame()->selection().isFocusedAndActive() ?
        LayoutTheme::theme().activeSelectionBackgroundColor() :
        LayoutTheme::theme().inactiveSelectionBackgroundColor();
}

Color LayoutObject::selectionColor(int colorProperty) const
{
    // If the element is unselectable, or we are only painting the selection,
    // don't override the foreground color with the selection foreground color.
    if (!isSelectable() || (frame()->view()->paintBehavior() & PaintBehaviorSelectionOnly))
        return resolveColor(colorProperty);

    if (RefPtr<ComputedStyle> pseudoStyle = getUncachedPseudoStyleFromParentOrShadowHost())
        return resolveColor(*pseudoStyle, colorProperty);
    if (!LayoutTheme::theme().supportsSelectionForegroundColors())
        return resolveColor(colorProperty);
    return frame()->selection().isFocusedAndActive() ?
        LayoutTheme::theme().activeSelectionForegroundColor() :
        LayoutTheme::theme().inactiveSelectionForegroundColor();
}

Color LayoutObject::selectionForegroundColor() const
{
    return selectionColor(CSSPropertyWebkitTextFillColor);
}

Color LayoutObject::selectionEmphasisMarkColor() const
{
    return selectionColor(CSSPropertyWebkitTextEmphasisColor);
}

void LayoutObject::selectionStartEnd(int& spos, int& epos) const
{
    view()->selectionStartEnd(spos, epos);
}

// Called when an object that was floating or positioned becomes a normal flow object
// again.  We have to make sure the layout tree updates as needed to accommodate the new
// normal flow object.
static inline void handleDynamicFloatPositionChange(LayoutObject* object)
{
    // We have gone from not affecting the inline status of the parent flow to suddenly
    // having an impact.  See if there is a mismatch between the parent flow's
    // childrenInline() state and our state.
    object->setInline(object->style()->isDisplayInlineType());
    if (object->isInline() != object->parent()->childrenInline()) {
        if (!object->isInline()) {
            toLayoutBoxModelObject(object->parent())->childBecameNonInline(object);
        } else {
            // An anonymous block must be made to wrap this inline.
            LayoutBlock* block = toLayoutBlock(object->parent())->createAnonymousBlock();
            LayoutObjectChildList* childlist = object->parent()->virtualChildren();
            childlist->insertChildNode(object->parent(), block, object);
            block->children()->appendChildNode(block, childlist->removeChildNode(object->parent(), object));
        }
    }
}

StyleDifference LayoutObject::adjustStyleDifference(StyleDifference diff) const
{
    if (diff.transformChanged() && isSVG())
        diff.setNeedsFullLayout();

    // If transform changed, and the layer does not paint into its own separate backing, then we need to invalidate paints.
    if (diff.transformChanged()) {
        // Text nodes share style with their parents but transforms don't apply to them,
        // hence the !isText() check.
        if (!isText() && (!hasLayer() || !toLayoutBoxModelObject(this)->layer()->hasStyleDeterminedDirectCompositingReasons()))
            diff.setNeedsPaintInvalidationLayer();
    }

    // If opacity or zIndex changed, and the layer does not paint into its own separate backing, then we need to invalidate paints (also
    // ignoring text nodes)
    if (diff.opacityChanged() || diff.zIndexChanged()) {
        if (!isText() && (!hasLayer() || !toLayoutBoxModelObject(this)->layer()->hasStyleDeterminedDirectCompositingReasons()))
            diff.setNeedsPaintInvalidationLayer();
    }

    // If filter changed, and the layer does not paint into its own separate backing or it paints with filters, then we need to invalidate paints.
    if (diff.filterChanged() && hasLayer()) {
        DeprecatedPaintLayer* layer = toLayoutBoxModelObject(this)->layer();
        if (!layer->hasStyleDeterminedDirectCompositingReasons() || layer->paintsWithFilters())
            diff.setNeedsPaintInvalidationLayer();
    }

    if (diff.textOrColorChanged() && !diff.needsPaintInvalidation()) {
        if (style()->hasBorder() || style()->hasOutline()
            || (isText() && !toLayoutText(this)->isAllCollapsibleWhitespace()))
            diff.setNeedsPaintInvalidationObject();
    }

    // The answer to layerTypeRequired() for plugins, iframes, and canvas can change without the actual
    // style changing, since it depends on whether we decide to composite these elements. When the
    // layer status of one of these elements changes, we need to force a layout.
    if (!diff.needsFullLayout() && style() && isBoxModelObject()) {
        bool requiresLayer = toLayoutBoxModelObject(this)->layerTypeRequired() != NoDeprecatedPaintLayer;
        if (hasLayer() != requiresLayer)
            diff.setNeedsFullLayout();
    }

    // If we have no layer(), just treat a PaintInvalidationLayer hint as a normal paint invalidation.
    if (diff.needsPaintInvalidationLayer() && !hasLayer()) {
        diff.clearNeedsPaintInvalidation();
        diff.setNeedsPaintInvalidationObject();
    }

    return diff;
}

void LayoutObject::setPseudoStyle(PassRefPtr<ComputedStyle> pseudoStyle)
{
    ASSERT(pseudoStyle->styleType() == BEFORE || pseudoStyle->styleType() == AFTER || pseudoStyle->styleType() == FIRST_LETTER);

    // FIXME: We should consider just making all pseudo items use an inherited style.

    // Images are special and must inherit the pseudoStyle so the width and height of
    // the pseudo element doesn't change the size of the image. In all other cases we
    // can just share the style.
    //
    // Quotes are also LayoutInline, so we need to create an inherited style to avoid
    // getting an inline with positioning or an invalid display.
    //
    if (isImage() || isQuote()) {
        RefPtr<ComputedStyle> style = ComputedStyle::create();
        style->inheritFrom(*pseudoStyle);
        setStyle(style.release());
        return;
    }

    setStyle(pseudoStyle);
}

void LayoutObject::firstLineStyleDidChange(const ComputedStyle& oldStyle, const ComputedStyle& newStyle)
{
    StyleDifference diff = oldStyle.visualInvalidationDiff(newStyle);
    if (diff.hasDifference()) {
        // TODO(rune@opera.com): We should use the diff to determine whether a repaint vs. layout
        // is needed, but for now just assume a layout will be required. The diff code
        // in LayoutObject::setStyle would need to be factored out so that it could be reused.
        setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::StyleChange);
    }
}

void LayoutObject::markContainingBlocksForOverflowRecalc()
{
    for (LayoutBlock* container = containingBlock(); container && !container->childNeedsOverflowRecalcAfterStyleChange(); container = container->containingBlock())
        container->setChildNeedsOverflowRecalcAfterStyleChange(true);
}

void LayoutObject::setNeedsOverflowRecalcAfterStyleChange()
{
    bool neededRecalc = needsOverflowRecalcAfterStyleChange();
    setSelfNeedsOverflowRecalcAfterStyleChange(true);
    if (!neededRecalc)
        markContainingBlocksForOverflowRecalc();
}

void LayoutObject::setStyle(PassRefPtr<ComputedStyle> style)
{
    ASSERT(style);

    if (m_style == style) {
        // We need to run through adjustStyleDifference() for iframes, plugins, and canvas so
        // style sharing is disabled for them. That should ensure that we never hit this code path.
        ASSERT(!isLayoutIFrame() && !isEmbeddedObject() && !isCanvas());
        return;
    }

    StyleDifference diff;
    if (m_style)
        diff = m_style->visualInvalidationDiff(*style);

    diff = adjustStyleDifference(diff);

    if (m_style) {
        LayoutFlowThread* flowThread = flowThreadContainingBlock();
        if (flowThread && flowThread != this)
            flowThread->flowThreadDescendantStyleWillChange(this, diff, *style);
    }
    styleWillChange(diff, *style);

    RefPtr<ComputedStyle> oldStyle = m_style.release();
    setStyleInternal(style);

    updateFillImages(oldStyle ? &oldStyle->backgroundLayers() : 0, m_style->backgroundLayers());
    updateFillImages(oldStyle ? &oldStyle->maskLayers() : 0, m_style->maskLayers());

    updateImage(oldStyle ? oldStyle->borderImage().image() : 0, m_style->borderImage().image());
    updateImage(oldStyle ? oldStyle->maskBoxImage().image() : 0, m_style->maskBoxImage().image());

    updateShapeImage(oldStyle ? oldStyle->shapeOutside() : 0, m_style->shapeOutside());

    bool doesNotNeedLayoutOrPaintInvalidation = !m_parent;

    styleDidChange(diff, oldStyle.get());
    if (oldStyle.get()) {
        LayoutFlowThread* flowThread = flowThreadContainingBlock();
        if (flowThread && flowThread != this)
            flowThread->flowThreadDescendantStyleDidChange(this, diff, *oldStyle.get());
    }

    // FIXME: |this| might be destroyed here. This can currently happen for a LayoutTextFragment when
    // its first-letter block gets an update in LayoutTextFragment::styleDidChange. For LayoutTextFragment(s),
    // we will safely bail out with the doesNotNeedLayoutOrPaintInvalidation flag. We might want to broaden
    // this condition in the future as we move layoutObject changes out of layout and into style changes.
    if (doesNotNeedLayoutOrPaintInvalidation)
        return;

    // Now that the layer (if any) has been updated, we need to adjust the diff again,
    // check whether we should layout now, and decide if we need to invalidate paints.
    StyleDifference updatedDiff = adjustStyleDifference(diff);

    if (!diff.needsFullLayout()) {
        if (updatedDiff.needsFullLayout())
            setNeedsLayoutAndPrefWidthsRecalc(LayoutInvalidationReason::StyleChange);
        else if (updatedDiff.needsPositionedMovementLayout())
            setNeedsPositionedMovementLayout();
    }

    if (diff.transformChanged() && !needsLayout()) {
        if (LayoutBlock* container = containingBlock())
            container->setNeedsOverflowRecalcAfterStyleChange();
    }

    if (updatedDiff.needsPaintInvalidationLayer())
        setShouldDoFullPaintInvalidationIncludingNonCompositingDescendants();
    else if (diff.needsPaintInvalidationObject() || updatedDiff.needsPaintInvalidationObject())
        setShouldDoFullPaintInvalidation();
}

void LayoutObject::styleWillChange(StyleDifference diff, const ComputedStyle& newStyle)
{
    if (m_style) {
        // If our z-index changes value or our visibility changes,
        // we need to dirty our stacking context's z-order list.
        bool visibilityChanged = m_style->visibility() != newStyle.visibility()
            || m_style->zIndex() != newStyle.zIndex()
            || m_style->hasAutoZIndex() != newStyle.hasAutoZIndex();
        if (visibilityChanged) {
            document().setAnnotatedRegionsDirty(true);
            if (AXObjectCache* cache = document().existingAXObjectCache())
                cache->childrenChanged(parent());
        }

        // Keep layer hierarchy visibility bits up to date if visibility changes.
        if (m_style->visibility() != newStyle.visibility()) {
            // We might not have an enclosing layer yet because we might not be in the tree.
            if (DeprecatedPaintLayer* layer = enclosingLayer())
                layer->potentiallyDirtyVisibleContentStatus(newStyle.visibility());
        }

        if (isFloating() && (m_style->floating() != newStyle.floating())) {
            // For changes in float styles, we need to conceivably remove ourselves
            // from the floating objects list.
            toLayoutBox(this)->removeFloatingOrPositionedChildFromBlockLists();
        } else if (isOutOfFlowPositioned() && (m_style->position() != newStyle.position())) {
            // For changes in positioning styles, we need to conceivably remove ourselves
            // from the positioned objects list.
            toLayoutBox(this)->removeFloatingOrPositionedChildFromBlockLists();
        }

        s_affectsParentBlock = isFloatingOrOutOfFlowPositioned()
            && (!newStyle.isFloating() && !newStyle.hasOutOfFlowPosition())
            && parent() && (parent()->isLayoutBlockFlow() || parent()->isLayoutInline());

        // Clearing these bits is required to avoid leaving stale layoutObjects.
        // FIXME: We shouldn't need that hack if our logic was totally correct.
        if (diff.needsLayout()) {
            setFloating(false);
            clearPositionedState();
        }
    } else {
        s_affectsParentBlock = false;
    }

    // Elements with non-auto touch-action will send a SetTouchAction message
    // on touchstart in EventHandler::handleTouchEvent, and so effectively have
    // a touchstart handler that must be reported.
    //
    // Since a CSS property cannot be applied directly to a text node, a
    // handler will have already been added for its parent so ignore it.
    TouchAction oldTouchAction = m_style ? m_style->touchAction() : TouchActionAuto;
    if (node() && !node()->isTextNode() && (oldTouchAction == TouchActionAuto) != (newStyle.touchAction() == TouchActionAuto)) {
        EventHandlerRegistry& registry = document().frameHost()->eventHandlerRegistry();
        if (newStyle.touchAction() != TouchActionAuto)
            registry.didAddEventHandler(*node(), EventHandlerRegistry::TouchEvent);
        else
            registry.didRemoveEventHandler(*node(), EventHandlerRegistry::TouchEvent);
    }
}

static bool areNonIdenticalCursorListsEqual(const ComputedStyle* a, const ComputedStyle* b)
{
    ASSERT(a->cursors() != b->cursors());
    return a->cursors() && b->cursors() && *a->cursors() == *b->cursors();
}

static inline bool areCursorsEqual(const ComputedStyle* a, const ComputedStyle* b)
{
    return a->cursor() == b->cursor() && (a->cursors() == b->cursors() || areNonIdenticalCursorListsEqual(a, b));
}

void LayoutObject::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    if (s_affectsParentBlock)
        handleDynamicFloatPositionChange(this);

    if (!m_parent)
        return;

    if (diff.needsFullLayout()) {
        LayoutCounter::layoutObjectStyleChanged(*this, oldStyle, *m_style);

        // If the object already needs layout, then setNeedsLayout won't do
        // any work. But if the containing block has changed, then we may need
        // to mark the new containing blocks for layout. The change that can
        // directly affect the containing block of this object is a change to
        // the position style.
        if (needsLayout() && oldStyle->position() != m_style->position())
            markContainerChainForLayout();

        // Ditto.
        if (needsOverflowRecalcAfterStyleChange() && oldStyle->position() != m_style->position())
            markContainingBlocksForOverflowRecalc();

        setNeedsLayoutAndPrefWidthsRecalc(LayoutInvalidationReason::StyleChange);
    } else if (diff.needsPositionedMovementLayout()) {
        setNeedsPositionedMovementLayout();
    }

    // Don't check for paint invalidation here; we need to wait until the layer has been
    // updated by subclasses before we know if we have to invalidate paints (in setStyle()).

    if (oldStyle && !areCursorsEqual(oldStyle, style())) {
        if (LocalFrame* frame = this->frame()) {
            // Cursor update scheduling is done by the local root, which is the main frame if there
            // are no RemoteFrame ancestors in the frame tree. Use of localFrameRoot() is
            // discouraged but will change when cursor update scheduling is moved from EventHandler
            // to PageEventHandler.
            frame->localFrameRoot()->eventHandler().scheduleCursorUpdate();
        }
    }
}

void LayoutObject::propagateStyleToAnonymousChildren(bool blockChildrenOnly)
{
    // FIXME: We could save this call when the change only affected non-inherited properties.
    for (LayoutObject* child = slowFirstChild(); child; child = child->nextSibling()) {
        if (!child->isAnonymous() || child->style()->styleType() != NOPSEUDO)
            continue;

        if (blockChildrenOnly && !child->isLayoutBlock())
            continue;

        if (child->isLayoutFullScreen() || child->isLayoutFullScreenPlaceholder())
            continue;

        RefPtr<ComputedStyle> newStyle = ComputedStyle::createAnonymousStyleWithDisplay(styleRef(), child->style()->display());

        // Preserve the position style of anonymous block continuations as they can have relative position when
        // they contain block descendants of relative positioned inlines.
        if (child->isRelPositioned() && toLayoutBlock(child)->isAnonymousBlockContinuation())
            newStyle->setPosition(child->style()->position());

        updateAnonymousChildStyle(*child, *newStyle);

        child->setStyle(newStyle.release());
    }
}

void LayoutObject::updateFillImages(const FillLayer* oldLayers, const FillLayer& newLayers)
{
    // Optimize the common case
    if (oldLayers && !oldLayers->next() && !newLayers.next() && (oldLayers->image() == newLayers.image()))
        return;

    // Go through the new layers and addClients first, to avoid removing all clients of an image.
    for (const FillLayer* currNew = &newLayers; currNew; currNew = currNew->next()) {
        if (currNew->image())
            currNew->image()->addClient(this);
    }

    for (const FillLayer* currOld = oldLayers; currOld; currOld = currOld->next()) {
        if (currOld->image())
            currOld->image()->removeClient(this);
    }
}

void LayoutObject::updateImage(StyleImage* oldImage, StyleImage* newImage)
{
    if (oldImage != newImage) {
        if (oldImage)
            oldImage->removeClient(this);
        if (newImage)
            newImage->addClient(this);
    }
}

void LayoutObject::updateShapeImage(const ShapeValue* oldShapeValue, const ShapeValue* newShapeValue)
{
    if (oldShapeValue || newShapeValue)
        updateImage(oldShapeValue ? oldShapeValue->image() : 0, newShapeValue ? newShapeValue->image() : 0);
}

LayoutRect LayoutObject::viewRect() const
{
    return view()->viewRect();
}

FloatPoint LayoutObject::localToAbsolute(const FloatPoint& localPoint, MapCoordinatesFlags mode) const
{
    TransformState transformState(TransformState::ApplyTransformDirection, localPoint);
    mapLocalToContainer(0, transformState, mode | ApplyContainerFlip);
    transformState.flatten();

    return transformState.lastPlanarPoint();
}

FloatPoint LayoutObject::absoluteToLocal(const FloatPoint& containerPoint, MapCoordinatesFlags mode) const
{
    TransformState transformState(TransformState::UnapplyInverseTransformDirection, containerPoint);
    mapAbsoluteToLocalPoint(mode, transformState);
    transformState.flatten();

    return transformState.lastPlanarPoint();
}

FloatQuad LayoutObject::absoluteToLocalQuad(const FloatQuad& quad, MapCoordinatesFlags mode) const
{
    TransformState transformState(TransformState::UnapplyInverseTransformDirection, quad.boundingBox().center(), quad);
    mapAbsoluteToLocalPoint(mode, transformState);
    transformState.flatten();
    return transformState.lastPlanarQuad();
}

void LayoutObject::mapLocalToContainer(const LayoutBoxModelObject* paintInvalidationContainer, TransformState& transformState, MapCoordinatesFlags mode, bool* wasFixed, const PaintInvalidationState* paintInvalidationState) const
{
    if (paintInvalidationContainer == this)
        return;

    LayoutObject* o = parent();
    if (!o)
        return;

    // FIXME: this should call offsetFromContainer to share code, but I'm not sure it's ever called.
    LayoutPoint centerPoint = roundedLayoutPoint(transformState.mappedPoint());
    if (mode & ApplyContainerFlip && o->isBox()) {
        if (o->style()->isFlippedBlocksWritingMode())
            transformState.move(toLayoutBox(o)->flipForWritingMode(roundedLayoutPoint(transformState.mappedPoint())) - centerPoint);
        mode &= ~ApplyContainerFlip;
    }

    transformState.move(o->columnOffset(roundedLayoutPoint(transformState.mappedPoint())));

    if (o->hasOverflowClip())
        transformState.move(-toLayoutBox(o)->scrolledContentOffset());

    o->mapLocalToContainer(paintInvalidationContainer, transformState, mode, wasFixed, paintInvalidationState);
}

const LayoutObject* LayoutObject::pushMappingToContainer(const LayoutBoxModelObject* ancestorToStopAt, LayoutGeometryMap& geometryMap) const
{
    ASSERT_NOT_REACHED();
    return nullptr;
}

void LayoutObject::mapAbsoluteToLocalPoint(MapCoordinatesFlags mode, TransformState& transformState) const
{
    LayoutObject* o = parent();
    if (o) {
        o->mapAbsoluteToLocalPoint(mode, transformState);
        if (o->hasOverflowClip())
            transformState.move(toLayoutBox(o)->scrolledContentOffset());
    }
}

bool LayoutObject::shouldUseTransformFromContainer(const LayoutObject* containerObject) const
{
    // hasTransform() indicates whether the object has transform, transform-style or perspective. We just care about transform,
    // so check the layer's transform directly.
    return (hasLayer() && toLayoutBoxModelObject(this)->layer()->transform()) || (containerObject && containerObject->style()->hasPerspective());
}

void LayoutObject::getTransformFromContainer(const LayoutObject* containerObject, const LayoutSize& offsetInContainer, TransformationMatrix& transform) const
{
    transform.makeIdentity();
    transform.translate(offsetInContainer.width().toFloat(), offsetInContainer.height().toFloat());
    DeprecatedPaintLayer* layer = hasLayer() ? toLayoutBoxModelObject(this)->layer() : 0;
    if (layer && layer->transform())
        transform.multiply(layer->currentTransform());

    if (containerObject && containerObject->hasLayer() && containerObject->style()->hasPerspective()) {
        // Perpsective on the container affects us, so we have to factor it in here.
        ASSERT(containerObject->hasLayer());
        FloatPoint perspectiveOrigin = toLayoutBoxModelObject(containerObject)->layer()->perspectiveOrigin();

        TransformationMatrix perspectiveMatrix;
        perspectiveMatrix.applyPerspective(containerObject->style()->perspective());

        transform.translateRight3d(-perspectiveOrigin.x(), -perspectiveOrigin.y(), 0);
        transform = perspectiveMatrix * transform;
        transform.translateRight3d(perspectiveOrigin.x(), perspectiveOrigin.y(), 0);
    }
}

FloatQuad LayoutObject::localToContainerQuad(const FloatQuad& localQuad, const LayoutBoxModelObject* paintInvalidationContainer, MapCoordinatesFlags mode, bool* wasFixed) const
{
    // Track the point at the center of the quad's bounding box. As mapLocalToContainer() calls offsetFromContainer(),
    // it will use that point as the reference point to decide which column's transform to apply in multiple-column blocks.
    TransformState transformState(TransformState::ApplyTransformDirection, localQuad.boundingBox().center(), localQuad);
    mapLocalToContainer(paintInvalidationContainer, transformState, mode | ApplyContainerFlip | UseTransforms, wasFixed);
    transformState.flatten();

    return transformState.lastPlanarQuad();
}

FloatPoint LayoutObject::localToContainerPoint(const FloatPoint& localPoint, const LayoutBoxModelObject* paintInvalidationContainer, MapCoordinatesFlags mode, bool* wasFixed, const PaintInvalidationState* paintInvalidationState) const
{
    TransformState transformState(TransformState::ApplyTransformDirection, localPoint);
    mapLocalToContainer(paintInvalidationContainer, transformState, mode | ApplyContainerFlip | UseTransforms, wasFixed, paintInvalidationState);
    transformState.flatten();

    return transformState.lastPlanarPoint();
}

FloatPoint LayoutObject::localToInvalidationBackingPoint(const LayoutPoint& localPoint, DeprecatedPaintLayer** backingLayer)
{
    const LayoutBoxModelObject& paintInvalidationContainer = containerForPaintInvalidationOnRootedTree();
    ASSERT(paintInvalidationContainer.layer());

    if (backingLayer)
        *backingLayer = paintInvalidationContainer.layer();
    FloatPoint containerPoint = localToContainerPoint(FloatPoint(localPoint), &paintInvalidationContainer, TraverseDocumentBoundaries);

    // A layoutObject can have no invalidation backing if it is from a detached frame,
    // or when forced compositing is disabled.
    if (paintInvalidationContainer.layer()->compositingState() == NotComposited)
        return containerPoint;

    DeprecatedPaintLayer::mapPointToPaintBackingCoordinates(&paintInvalidationContainer, containerPoint);
    return containerPoint;
}

LayoutSize LayoutObject::offsetFromContainer(const LayoutObject* o, const LayoutPoint& point, bool* offsetDependsOnPoint) const
{
    ASSERT(o == container());

    LayoutSize offset = o->columnOffset(point);

    if (o->hasOverflowClip())
        offset -= toLayoutBox(o)->scrolledContentOffset();

    if (offsetDependsOnPoint)
        *offsetDependsOnPoint = o->isLayoutFlowThread();

    return offset;
}

LayoutSize LayoutObject::offsetFromAncestorContainer(const LayoutObject* container) const
{
    if (container == this)
        return LayoutSize();

    LayoutSize offset;
    LayoutPoint referencePoint;
    const LayoutObject* currContainer = this;
    do {
        const LayoutObject* nextContainer = currContainer->container();
        ASSERT(nextContainer); // This means we reached the top without finding container.
        if (!nextContainer)
            break;
        ASSERT(!currContainer->hasTransformRelatedProperty());
        LayoutSize currentOffset = currContainer->offsetFromContainer(nextContainer, referencePoint);
        offset += currentOffset;
        referencePoint.move(currentOffset);
        currContainer = nextContainer;
    } while (currContainer != container);

    return offset;
}

LayoutRect LayoutObject::localCaretRect(InlineBox*, int, LayoutUnit* extraWidthToEndOfLine)
{
    if (extraWidthToEndOfLine)
        *extraWidthToEndOfLine = 0;

    return LayoutRect();
}

void LayoutObject::computeLayerHitTestRects(LayerHitTestRects& layerRects) const
{
    // Figure out what layer our container is in. Any offset (or new layer) for this
    // layoutObject within it's container will be applied in addLayerHitTestRects.
    LayoutPoint layerOffset;
    const DeprecatedPaintLayer* currentLayer = nullptr;

    if (!hasLayer()) {
        LayoutObject* container = this->container();
        currentLayer = container->enclosingLayer();
        if (container && currentLayer->layoutObject() != container) {
            layerOffset.move(container->offsetFromAncestorContainer(currentLayer->layoutObject()));
            // If the layer itself is scrolled, we have to undo the subtraction of its scroll
            // offset since we want the offset relative to the scrolling content, not the
            // element itself.
            if (currentLayer->layoutObject()->hasOverflowClip())
                layerOffset.move(currentLayer->layoutBox()->scrolledContentOffset());
        }
    }

    this->addLayerHitTestRects(layerRects, currentLayer, layerOffset, LayoutRect());
}

void LayoutObject::addLayerHitTestRects(LayerHitTestRects& layerRects, const DeprecatedPaintLayer* currentLayer, const LayoutPoint& layerOffset, const LayoutRect& containerRect) const
{
    ASSERT(currentLayer);
    ASSERT(currentLayer == this->enclosingLayer());

    // Compute the rects for this layoutObject only and add them to the results.
    // Note that we could avoid passing the offset and instead adjust each result, but this
    // seems slightly simpler.
    Vector<LayoutRect> ownRects;
    LayoutRect newContainerRect;
    computeSelfHitTestRects(ownRects, layerOffset);

    // When we get to have a lot of rects on a layer, the performance cost of tracking those
    // rects outweighs the benefit of doing compositor thread hit testing.
    // FIXME: This limit needs to be low due to the O(n^2) algorithm in
    // WebLayer::setTouchEventHandlerRegion - crbug.com/300282.
    const size_t maxRectsPerLayer = 100;

    LayerHitTestRects::iterator iter = layerRects.find(currentLayer);
    Vector<LayoutRect>* iterValue;
    if (iter == layerRects.end())
        iterValue = &layerRects.add(currentLayer, Vector<LayoutRect>()).storedValue->value;
    else
        iterValue = &iter->value;
    for (size_t i = 0; i < ownRects.size(); i++) {
        if (!containerRect.contains(ownRects[i])) {
            iterValue->append(ownRects[i]);
            if (iterValue->size() > maxRectsPerLayer) {
                // Just mark the entire layer instead, and switch to walking the layer
                // tree instead of the layout tree.
                layerRects.remove(currentLayer);
                currentLayer->addLayerHitTestRects(layerRects);
                return;
            }
            if (newContainerRect.isEmpty())
                newContainerRect = ownRects[i];
        }
    }
    if (newContainerRect.isEmpty())
        newContainerRect = containerRect;

    // If it's possible for children to have rects outside our bounds, then we need to descend into
    // the children and compute them.
    // Ideally there would be other cases where we could detect that children couldn't have rects
    // outside our bounds and prune the tree walk.
    // Note that we don't use Region here because Union is O(N) - better to just keep a list of
    // partially redundant rectangles. If we find examples where this is expensive, then we could
    // rewrite Region to be more efficient. See https://bugs.webkit.org/show_bug.cgi?id=100814.
    if (!isLayoutView()) {
        for (LayoutObject* curr = slowFirstChild(); curr; curr = curr->nextSibling()) {
            curr->addLayerHitTestRects(layerRects, currentLayer,  layerOffset, newContainerRect);
        }
    }
}

bool LayoutObject::isRooted() const
{
    const LayoutObject* object = this;
    while (object->parent() && !object->hasLayer())
        object = object->parent();
    if (object->hasLayer())
        return toLayoutBoxModelObject(object)->layer()->root()->isRootLayer();
    return false;
}

RespectImageOrientationEnum LayoutObject::shouldRespectImageOrientation() const
{
    // Respect the image's orientation if it's being used as a full-page image or it's
    // an <img> and the setting to respect it everywhere is set.
    return document().isImageDocument()
        || (document().settings() && document().settings()->shouldRespectImageOrientation() && isHTMLImageElement(node())) ? RespectImageOrientation : DoNotRespectImageOrientation;
}

LayoutObject* LayoutObject::container(const LayoutBoxModelObject* paintInvalidationContainer, bool* paintInvalidationContainerSkipped) const
{
    if (paintInvalidationContainerSkipped)
        *paintInvalidationContainerSkipped = false;

    // This method is extremely similar to containingBlock(), but with a few notable
    // exceptions.
    // (1) It can be used on orphaned subtrees, i.e., it can be called safely even when
    // the object is not part of the primary document subtree yet.
    // (2) For normal flow elements, it just returns the parent.
    // (3) For absolute positioned elements, it will return a relative positioned inline.
    // containingBlock() simply skips relpositioned inlines and lets an enclosing block handle
    // the layout of the positioned object.  This does mean that computePositionedLogicalWidth and
    // computePositionedLogicalHeight have to use container().
    LayoutObject* o = parent();

    if (isTextOrSVGChild())
        return o;

    EPosition pos = m_style->position();
    if (pos == FixedPosition)
        return containerForFixedPosition(paintInvalidationContainer, paintInvalidationContainerSkipped);

    if (pos == AbsolutePosition)
        return containerForAbsolutePosition(paintInvalidationContainer, paintInvalidationContainerSkipped);
        
    if (isColumnSpanAll()) {
        LayoutObject* multicolContainer = spannerPlaceholder()->container();
        if (paintInvalidationContainerSkipped && paintInvalidationContainer) {
            // We jumped directly from the spanner to the multicol container. Need to check if
            // we skipped |paintInvalidationContainer| on the way.
            for (LayoutObject* walker = parent(); walker && walker != multicolContainer; walker = walker->parent()) {
                if (walker == paintInvalidationContainer) {
                    *paintInvalidationContainerSkipped = true;
                    break;
                }
            }
        }
        return multicolContainer;
    }

    return o;
}

LayoutObject* LayoutObject::containerCrossingFrameBoundaries() const
{
    return isLayoutView() ? frame()->ownerLayoutObject() : container();
}

bool LayoutObject::isSelectionBorder() const
{
    SelectionState st = selectionState();
    return st == SelectionStart || st == SelectionEnd || st == SelectionBoth;
}

inline void LayoutObject::clearLayoutRootIfNeeded() const
{
    if (FrameView* view = frameView()) {
        if (!documentBeingDestroyed())
            view->clearLayoutSubtreeRoot(*this);
    }
}

void LayoutObject::willBeDestroyed()
{
    // Destroy any leftover anonymous children.
    LayoutObjectChildList* children = virtualChildren();
    if (children)
        children->destroyLeftoverChildren();

    if (LocalFrame* frame = this->frame()) {
        // If this layoutObject is being autoscrolled, stop the autoscrolling.
        if (frame->page())
            frame->page()->autoscrollController().stopAutoscrollIfNeeded(this);
    }

    // For accessibility management, notify the parent of the imminent change to its child set.
    // We do it now, before remove(), while the parent pointer is still available.
    if (AXObjectCache* cache = document().existingAXObjectCache())
        cache->childrenChanged(this->parent());

    remove();

    // The remove() call above may invoke axObjectCache()->childrenChanged() on the parent, which may require the AX layout
    // object for this layoutObject. So we remove the AX layout object now, after the layoutObject is removed.
    if (AXObjectCache* cache = document().existingAXObjectCache())
        cache->remove(this);

    // If this layoutObject had a parent, remove should have destroyed any counters
    // attached to this layoutObject and marked the affected other counters for
    // reevaluation. This apparently redundant check is here for the case when
    // this layoutObject had no parent at the time remove() was called.

    if (hasCounterNodeMap())
        LayoutCounter::destroyCounterNodes(*this);

    // Remove the handler if node had touch-action set. Handlers are not added
    // for text nodes so don't try removing for one too. Need to check if
    // m_style is null in cases of partial construction. Any handler we added
    // previously may have already been removed by the Document independently.
    if (node() && !node()->isTextNode() && m_style && m_style->touchAction() != TouchActionAuto) {
        EventHandlerRegistry& registry = document().frameHost()->eventHandlerRegistry();
        if (registry.eventHandlerTargets(EventHandlerRegistry::TouchEvent)->contains(node()))
            registry.didRemoveEventHandler(*node(), EventHandlerRegistry::TouchEvent);
    }

    setAncestorLineBoxDirty(false);

    if (selectionPaintInvalidationMap)
        selectionPaintInvalidationMap->remove(this);

    clearLayoutRootIfNeeded();

    if (m_style) {
        for (const FillLayer* bgLayer = &m_style->backgroundLayers(); bgLayer; bgLayer = bgLayer->next()) {
            if (StyleImage* backgroundImage = bgLayer->image())
                backgroundImage->removeClient(this);
        }

        for (const FillLayer* maskLayer = &m_style->maskLayers(); maskLayer; maskLayer = maskLayer->next()) {
            if (StyleImage* maskImage = maskLayer->image())
                maskImage->removeClient(this);
        }

        if (StyleImage* borderImage = m_style->borderImage().image())
            borderImage->removeClient(this);

        if (StyleImage* maskBoxImage = m_style->maskBoxImage().image())
            maskBoxImage->removeClient(this);

        removeShapeImageClient(m_style->shapeOutside());
    }
    ResourceLoadPriorityOptimizer::resourceLoadPriorityOptimizer()->removeLayoutObject(this);

    if (frameView())
        setIsSlowRepaintObject(false);
}

void LayoutObject::insertedIntoTree()
{
    // FIXME: We should ASSERT(isRooted()) here but generated content makes some out-of-order insertion.

    // Keep our layer hierarchy updated. Optimize for the common case where we don't have any children
    // and don't have a layer attached to ourselves.
    DeprecatedPaintLayer* layer = nullptr;
    if (slowFirstChild() || hasLayer()) {
        layer = parent()->enclosingLayer();
        addLayers(layer);
    }

    // If |this| is visible but this object was not, tell the layer it has some visible content
    // that needs to be drawn and layer visibility optimization can't be used
    if (parent()->style()->visibility() != VISIBLE && style()->visibility() == VISIBLE && !hasLayer()) {
        if (!layer)
            layer = parent()->enclosingLayer();
        if (layer)
            layer->dirtyVisibleContentStatus();
    }

    if (!isFloating() && parent()->childrenInline())
        parent()->dirtyLinesFromChangedChild(this);

    if (LayoutFlowThread* flowThread = flowThreadContainingBlock())
        flowThread->flowThreadDescendantWasInserted(this);
}

void LayoutObject::willBeRemovedFromTree()
{
    // FIXME: We should ASSERT(isRooted()) but we have some out-of-order removals which would need to be fixed first.

    // If we remove a visible child from an invisible parent, we don't know the layer visibility any more.
    DeprecatedPaintLayer* layer = nullptr;
    if (parent()->style()->visibility() != VISIBLE && style()->visibility() == VISIBLE && !hasLayer()) {
        layer = parent()->enclosingLayer();
        if (layer)
            layer->dirtyVisibleContentStatus();
    }

    // Keep our layer hierarchy updated.
    if (slowFirstChild() || hasLayer()) {
        if (!layer)
            layer = parent()->enclosingLayer();
        removeLayers(layer);
    }

    if (isOutOfFlowPositioned() && parent()->childrenInline())
        parent()->dirtyLinesFromChangedChild(this);

    removeFromLayoutFlowThread();

    // Update cached boundaries in SVG layoutObjects if a child is removed.
    if (parent()->isSVG())
        parent()->setNeedsBoundariesUpdate();
}

void LayoutObject::removeFromLayoutFlowThread()
{
    if (!isInsideFlowThread())
        return;

    // Sometimes we remove the element from the flow, but it's not destroyed at that time.
    // It's only until later when we actually destroy it and remove all the children from it.
    // Currently, that happens for firstLetter elements and list markers.
    // Pass in the flow thread so that we don't have to look it up for all the children.
    // If we're a column spanner, we need to use our parent to find the flow thread, since a spanner
    // doesn't have the flow thread in its containing block chain. We still need to notify the flow
    // thread when the layoutObject removed happens to be a spanner, so that we get rid of the spanner
    // placeholder, and column sets around the placeholder get merged.
    LayoutFlowThread* flowThread = isColumnSpanAll() ? parent()->flowThreadContainingBlock() : flowThreadContainingBlock();
    removeFromLayoutFlowThreadRecursive(flowThread);
}

void LayoutObject::removeFromLayoutFlowThreadRecursive(LayoutFlowThread* layoutFlowThread)
{
    if (const LayoutObjectChildList* children = virtualChildren()) {
        for (LayoutObject* child = children->firstChild(); child; child = child->nextSibling()) {
            if (child->isLayoutFlowThread())
                continue; // Don't descend into inner fragmentation contexts.
            child->removeFromLayoutFlowThreadRecursive(child->isLayoutFlowThread() ? toLayoutFlowThread(child) : layoutFlowThread);
        }
    }

    if (layoutFlowThread && layoutFlowThread != this)
        layoutFlowThread->flowThreadDescendantWillBeRemoved(this);
    setIsInsideFlowThread(false);
    RELEASE_ASSERT(!spannerPlaceholder());
}

void LayoutObject::destroyAndCleanupAnonymousWrappers()
{
    // If the tree is destroyed, there is no need for a clean-up phase.
    if (documentBeingDestroyed()) {
        destroy();
        return;
    }

    LayoutObject* destroyRoot = this;
    for (LayoutObject* destroyRootParent = destroyRoot->parent(); destroyRootParent && destroyRootParent->isAnonymous(); destroyRoot = destroyRootParent, destroyRootParent = destroyRootParent->parent()) {
        // Anonymous block continuations are tracked and destroyed elsewhere (see the bottom of LayoutBlock::removeChild)
        if (destroyRootParent->isLayoutBlock() && toLayoutBlock(destroyRootParent)->isAnonymousBlockContinuation())
            break;
        // A flow thread is tracked by its containing block. Whether its children are removed or not is irrelevant.
        if (destroyRootParent->isLayoutFlowThread())
            break;

        if (destroyRootParent->slowFirstChild() != destroyRoot || destroyRootParent->slowLastChild() != destroyRoot)
            break; // Need to keep the anonymous parent, since it won't become empty by the removal of this layoutObject.
    }

    destroyRoot->destroy();

    // WARNING: |this| is deleted here.
}

void LayoutObject::destroy()
{
    willBeDestroyed();
    delete this;
}

void LayoutObject::removeShapeImageClient(ShapeValue* shapeValue)
{
    if (!shapeValue)
        return;
    if (StyleImage* shapeImage = shapeValue->image())
        shapeImage->removeClient(this);
}

PositionWithAffinity LayoutObject::positionForPoint(const LayoutPoint&)
{
    return createPositionWithAffinity(caretMinOffset(), DOWNSTREAM);
}

void LayoutObject::updateDragState(bool dragOn)
{
    bool valueChanged = (dragOn != isDragging());
    setIsDragging(dragOn);
    if (valueChanged && node()) {
        if (node()->isElementNode() && toElement(node())->childrenOrSiblingsAffectedByDrag())
            node()->setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::Drag));
        else if (style()->affectedByDrag())
            node()->setNeedsStyleRecalc(LocalStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::Drag));
    }
    for (LayoutObject* curr = slowFirstChild(); curr; curr = curr->nextSibling())
        curr->updateDragState(dragOn);
}

CompositingState LayoutObject::compositingState() const
{
    return hasLayer() ? toLayoutBoxModelObject(this)->layer()->compositingState() : NotComposited;
}

CompositingReasons LayoutObject::additionalCompositingReasons() const
{
    return CompositingReasonNone;
}

bool LayoutObject::hitTest(HitTestResult& result, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestFilter hitTestFilter)
{
    bool inside = false;
    if (hitTestFilter != HitTestSelf) {
        // First test the foreground layer (lines and inlines).
        inside = nodeAtPoint(result, locationInContainer, accumulatedOffset, HitTestForeground);

        // Test floats next.
        if (!inside)
            inside = nodeAtPoint(result, locationInContainer, accumulatedOffset, HitTestFloat);

        // Finally test to see if the mouse is in the background (within a child block's background).
        if (!inside)
            inside = nodeAtPoint(result, locationInContainer, accumulatedOffset, HitTestChildBlockBackgrounds);
    }

    // See if the mouse is inside us but not any of our descendants
    if (hitTestFilter != HitTestDescendants && !inside)
        inside = nodeAtPoint(result, locationInContainer, accumulatedOffset, HitTestBlockBackground);

    return inside;
}

void LayoutObject::updateHitTestResult(HitTestResult& result, const LayoutPoint& point)
{
    if (result.innerNode())
        return;

    Node* node = this->node();

    // If we hit the anonymous layoutObjects inside generated content we should
    // actually hit the generated content so walk up to the PseudoElement.
    if (!node && parent() && parent()->isBeforeOrAfterContent()) {
        for (LayoutObject* layoutObject = parent(); layoutObject && !node; layoutObject = layoutObject->parent())
            node = layoutObject->node();
    }

    if (node)
        result.setNodeAndPosition(node, point);
}

bool LayoutObject::nodeAtPoint(HitTestResult&, const HitTestLocation& /*locationInContainer*/, const LayoutPoint& /*accumulatedOffset*/, HitTestAction)
{
    return false;
}

void LayoutObject::scheduleRelayout()
{
    if (isLayoutView()) {
        FrameView* view = toLayoutView(this)->frameView();
        if (view)
            view->scheduleRelayout();
    } else {
        if (isRooted()) {
            if (LayoutView* layoutView = view()) {
                if (FrameView* frameView = layoutView->frameView())
                    frameView->scheduleRelayoutOfSubtree(this);
            }
        }
    }
}

void LayoutObject::forceLayout()
{
    setSelfNeedsLayout(true);
    setShouldDoFullPaintInvalidation();
    layout();
}

// FIXME: Does this do anything different than forceLayout given that we don't walk
// the containing block chain. If not, we should change all callers to use forceLayout.
void LayoutObject::forceChildLayout()
{
    setNormalChildNeedsLayout(true);
    layout();
}

enum StyleCacheState {
    Cached,
    Uncached
};

static PassRefPtr<ComputedStyle> firstLineStyleForCachedUncachedType(StyleCacheState type, const LayoutObject* layoutObject, ComputedStyle* style)
{
    const LayoutObject* layoutObjectForFirstLineStyle = layoutObject;
    if (layoutObject->isBeforeOrAfterContent())
        layoutObjectForFirstLineStyle = layoutObject->parent();

    if (layoutObjectForFirstLineStyle->isLayoutBlockFlow() || layoutObjectForFirstLineStyle->isLayoutButton()) {
        if (LayoutBlock* firstLineBlock = layoutObjectForFirstLineStyle->firstLineBlock()) {
            if (type == Cached)
                return firstLineBlock->getCachedPseudoStyle(FIRST_LINE, style);
            return firstLineBlock->getUncachedPseudoStyle(PseudoStyleRequest(FIRST_LINE), style, firstLineBlock == layoutObject ? style : 0);
        }
    } else if (!layoutObjectForFirstLineStyle->isAnonymous() && layoutObjectForFirstLineStyle->isLayoutInline()
        && !layoutObjectForFirstLineStyle->node()->isFirstLetterPseudoElement()) {
        const ComputedStyle* parentStyle = layoutObjectForFirstLineStyle->parent()->firstLineStyle();
        if (parentStyle != layoutObjectForFirstLineStyle->parent()->style()) {
            if (type == Cached) {
                // A first-line style is in effect. Cache a first-line style for ourselves.
                layoutObjectForFirstLineStyle->mutableStyleRef().setHasPseudoStyle(FIRST_LINE_INHERITED);
                return layoutObjectForFirstLineStyle->getCachedPseudoStyle(FIRST_LINE_INHERITED, parentStyle);
            }
            return layoutObjectForFirstLineStyle->getUncachedPseudoStyle(PseudoStyleRequest(FIRST_LINE_INHERITED), parentStyle, style);
        }
    }
    return nullptr;
}

PassRefPtr<ComputedStyle> LayoutObject::uncachedFirstLineStyle(ComputedStyle* style) const
{
    if (!document().styleEngine().usesFirstLineRules())
        return nullptr;

    ASSERT(!isText());

    return firstLineStyleForCachedUncachedType(Uncached, this, style);
}

ComputedStyle* LayoutObject::cachedFirstLineStyle() const
{
    ASSERT(document().styleEngine().usesFirstLineRules());

    if (RefPtr<ComputedStyle> style = firstLineStyleForCachedUncachedType(Cached, isText() ? parent() : this, m_style.get()))
        return style.get();

    return m_style.get();
}

ComputedStyle* LayoutObject::getCachedPseudoStyle(PseudoId pseudo, const ComputedStyle* parentStyle) const
{
    if (pseudo < FIRST_INTERNAL_PSEUDOID && !style()->hasPseudoStyle(pseudo))
        return nullptr;

    ComputedStyle* cachedStyle = style()->getCachedPseudoStyle(pseudo);
    if (cachedStyle)
        return cachedStyle;

    RefPtr<ComputedStyle> result = getUncachedPseudoStyle(PseudoStyleRequest(pseudo), parentStyle);
    if (result)
        return mutableStyleRef().addCachedPseudoStyle(result.release());
    return nullptr;
}

PassRefPtr<ComputedStyle> LayoutObject::getUncachedPseudoStyle(const PseudoStyleRequest& pseudoStyleRequest, const ComputedStyle* parentStyle, const ComputedStyle* ownStyle) const
{
    if (pseudoStyleRequest.pseudoId < FIRST_INTERNAL_PSEUDOID && !ownStyle && !style()->hasPseudoStyle(pseudoStyleRequest.pseudoId))
        return nullptr;

    if (!parentStyle) {
        ASSERT(!ownStyle);
        parentStyle = style();
    }

    if (!node())
        return nullptr;

    Element* element = Traversal<Element>::firstAncestorOrSelf(*node());
    if (!element)
        return nullptr;

    if (pseudoStyleRequest.pseudoId == FIRST_LINE_INHERITED) {
        RefPtr<ComputedStyle> result = document().ensureStyleResolver().styleForElement(element, parentStyle, DisallowStyleSharing);
        result->setStyleType(FIRST_LINE_INHERITED);
        return result.release();
    }

    return document().ensureStyleResolver().pseudoStyleForElement(element, pseudoStyleRequest, parentStyle);
}

PassRefPtr<ComputedStyle> LayoutObject::getUncachedPseudoStyleFromParentOrShadowHost() const
{
    if (!node())
        return nullptr;

    if (ShadowRoot* root = node()->containingShadowRoot()) {
        if (root->type() == ShadowRootType::UserAgent) {
            if (Element* shadowHost = node()->shadowHost()) {
                return shadowHost->layoutObject()->getUncachedPseudoStyle(PseudoStyleRequest(SELECTION));
            }
        }
    }

    return getUncachedPseudoStyle(PseudoStyleRequest(SELECTION));
}

void LayoutObject::getTextDecorations(unsigned decorations, AppliedTextDecoration& underline, AppliedTextDecoration& overline, AppliedTextDecoration& linethrough, bool quirksMode, bool firstlineStyle)
{
    LayoutObject* curr = this;
    const ComputedStyle* styleToUse = nullptr;
    unsigned currDecs = TextDecorationNone;
    Color resultColor;
    TextDecorationStyle resultStyle;
    do {
        styleToUse = curr->style(firstlineStyle);
        currDecs = styleToUse->textDecoration();
        currDecs &= decorations;
        resultColor = styleToUse->visitedDependentColor(CSSPropertyTextDecorationColor);
        resultStyle = styleToUse->textDecorationStyle();
        // Parameter 'decorations' is cast as an int to enable the bitwise operations below.
        if (currDecs) {
            if (currDecs & TextDecorationUnderline) {
                decorations &= ~TextDecorationUnderline;
                underline.color = resultColor;
                underline.style = resultStyle;
            }
            if (currDecs & TextDecorationOverline) {
                decorations &= ~TextDecorationOverline;
                overline.color = resultColor;
                overline.style = resultStyle;
            }
            if (currDecs & TextDecorationLineThrough) {
                decorations &= ~TextDecorationLineThrough;
                linethrough.color = resultColor;
                linethrough.style = resultStyle;
            }
        }
        if (curr->isRubyText())
            return;
        curr = curr->parent();
        if (curr && curr->isAnonymousBlock() && toLayoutBlock(curr)->continuation())
            curr = toLayoutBlock(curr)->continuation();
    } while (curr && decorations && (!quirksMode || !curr->node() || (!isHTMLAnchorElement(*curr->node()) && !isHTMLFontElement(*curr->node()))));

    // If we bailed out, use the element we bailed out at (typically a <font> or <a> element).
    if (decorations && curr) {
        styleToUse = curr->style(firstlineStyle);
        resultColor = styleToUse->visitedDependentColor(CSSPropertyTextDecorationColor);
        if (decorations & TextDecorationUnderline) {
            underline.color = resultColor;
            underline.style = resultStyle;
        }
        if (decorations & TextDecorationOverline) {
            overline.color = resultColor;
            overline.style = resultStyle;
        }
        if (decorations & TextDecorationLineThrough) {
            linethrough.color = resultColor;
            linethrough.style = resultStyle;
        }
    }
}

void LayoutObject::addAnnotatedRegions(Vector<AnnotatedRegionValue>& regions)
{
    // Convert the style regions to absolute coordinates.
    if (style()->visibility() != VISIBLE || !isBox())
        return;

    if (style()->getDraggableRegionMode() == DraggableRegionNone)
        return;

    LayoutBox* box = toLayoutBox(this);
    FloatRect localBounds(FloatPoint(), FloatSize(box->size()));
    FloatRect absBounds = localToAbsoluteQuad(localBounds).boundingBox();

    AnnotatedRegionValue region;
    region.draggable = style()->getDraggableRegionMode() == DraggableRegionDrag;
    region.bounds = LayoutRect(absBounds);
    regions.append(region);
}

bool LayoutObject::willRenderImage(ImageResource*)
{
    // Without visibility we won't render (and therefore don't care about animation).
    if (style()->visibility() != VISIBLE)
        return false;

    // We will not render a new image when Active DOM is suspended
    if (document().activeDOMObjectsAreSuspended())
        return false;

    // If we're not in a window (i.e., we're dormant from being in a background tab)
    // then we don't want to render either.
    return document().view()->isVisible();
}

bool LayoutObject::getImageAnimationPolicy(ImageResource*, ImageAnimationPolicy& policy)
{
    if (!document().settings())
        return false;
    policy = document().settings()->imageAnimationPolicy();
    return true;
}

int LayoutObject::caretMinOffset() const
{
    return 0;
}

int LayoutObject::caretMaxOffset() const
{
    if (isReplaced())
        return node() ? std::max(1U, node()->countChildren()) : 1;
    if (isHR())
        return 1;
    return 0;
}

int LayoutObject::previousOffset(int current) const
{
    return current - 1;
}

int LayoutObject::previousOffsetForBackwardDeletion(int current) const
{
    return current - 1;
}

int LayoutObject::nextOffset(int current) const
{
    return current + 1;
}

bool LayoutObject::isInert() const
{
    const LayoutObject* layoutObject = this;
    while (!layoutObject->node())
        layoutObject = layoutObject->parent();
    return layoutObject->node()->isInert();
}

void LayoutObject::imageChanged(ImageResource* image, const IntRect* rect)
{
    ASSERT(m_node);
    imageChanged(static_cast<WrappedImagePtr>(image), rect);
}

Element* LayoutObject::offsetParent() const
{
    if (isDocumentElement() || isBody())
        return nullptr;

    if (isOutOfFlowPositioned() && style()->position() == FixedPosition)
        return nullptr;

    float effectiveZoom = style()->effectiveZoom();
    Node* node = nullptr;
    for (LayoutObject* ancestor = parent(); ancestor; ancestor = ancestor->parent()) {
        // Spec: http://www.w3.org/TR/cssom-view/#offset-attributes

        node = ancestor->node();

        if (!node)
            continue;

        if (ancestor->isPositioned())
            break;

        if (isHTMLBodyElement(*node))
            break;

        if (!isPositioned() && (isHTMLTableElement(*node) || isHTMLTableCellElement(*node)))
            break;

        // Webkit specific extension where offsetParent stops at zoom level changes.
        if (effectiveZoom != ancestor->style()->effectiveZoom())
            break;
    }

    return node && node->isElementNode() ? toElement(node) : nullptr;
}

PositionWithAffinity LayoutObject::createPositionWithAffinity(int offset, EAffinity affinity)
{
    // If this is a non-anonymous layoutObject in an editable area, then it's simple.
    if (Node* node = nonPseudoNode()) {
        if (!node->hasEditableStyle()) {
            // If it can be found, we prefer a visually equivalent position that is editable.
            Position position = createLegacyEditingPosition(node, offset);
            Position candidate = position.downstream(CanCrossEditingBoundary);
            if (candidate.deprecatedNode()->hasEditableStyle())
                return PositionWithAffinity(candidate, affinity);
            candidate = position.upstream(CanCrossEditingBoundary);
            if (candidate.deprecatedNode()->hasEditableStyle())
                return PositionWithAffinity(candidate, affinity);
        }
        // FIXME: Eliminate legacy editing positions
        return PositionWithAffinity(createLegacyEditingPosition(node, offset), affinity);
    }

    // We don't want to cross the boundary between editable and non-editable
    // regions of the document, but that is either impossible or at least
    // extremely unlikely in any normal case because we stop as soon as we
    // find a single non-anonymous layoutObject.

    // Find a nearby non-anonymous layoutObject.
    LayoutObject* child = this;
    while (LayoutObject* parent = child->parent()) {
        // Find non-anonymous content after.
        for (LayoutObject* layoutObject = child->nextInPreOrder(parent); layoutObject; layoutObject = layoutObject->nextInPreOrder(parent)) {
            if (Node* node = layoutObject->nonPseudoNode())
                return PositionWithAffinity(firstPositionInOrBeforeNode(node), DOWNSTREAM);
        }

        // Find non-anonymous content before.
        for (LayoutObject* layoutObject = child->previousInPreOrder(); layoutObject; layoutObject = layoutObject->previousInPreOrder()) {
            if (layoutObject == parent)
                break;
            if (Node* node = layoutObject->nonPseudoNode())
                return PositionWithAffinity(lastPositionInOrAfterNode(node), DOWNSTREAM);
        }

        // Use the parent itself unless it too is anonymous.
        if (Node* node = parent->nonPseudoNode())
            return PositionWithAffinity(firstPositionInOrBeforeNode(node), DOWNSTREAM);

        // Repeat at the next level up.
        child = parent;
    }

    // Everything was anonymous. Give up.
    return PositionWithAffinity();
}

PositionWithAffinity LayoutObject::createPositionWithAffinity(const Position& position)
{
    if (position.isNotNull())
        return PositionWithAffinity(position);

    ASSERT(!node());
    return createPositionWithAffinity(0, DOWNSTREAM);
}

CursorDirective LayoutObject::getCursor(const LayoutPoint&, Cursor&) const
{
    return SetCursorBasedOnStyle;
}

bool LayoutObject::canUpdateSelectionOnRootLineBoxes() const
{
    if (needsLayout())
        return false;

    const LayoutBlock* containingBlock = this->containingBlock();
    return containingBlock ? !containingBlock->needsLayout() : false;
}

void LayoutObject::setNeedsBoundariesUpdate()
{
    if (LayoutObject* layoutObject = parent())
        layoutObject->setNeedsBoundariesUpdate();
}

FloatRect LayoutObject::objectBoundingBox() const
{
    ASSERT_NOT_REACHED();
    return FloatRect();
}

FloatRect LayoutObject::strokeBoundingBox() const
{
    ASSERT_NOT_REACHED();
    return FloatRect();
}

// Returns the smallest rectangle enclosing all of the painted content
// respecting clipping, masking, filters, opacity, stroke-width and markers
FloatRect LayoutObject::paintInvalidationRectInLocalCoordinates() const
{
    ASSERT_NOT_REACHED();
    return FloatRect();
}

AffineTransform LayoutObject::localTransform() const
{
    static const AffineTransform identity;
    return identity;
}

const AffineTransform& LayoutObject::localToParentTransform() const
{
    static const AffineTransform identity;
    return identity;
}

bool LayoutObject::nodeAtFloatPoint(HitTestResult&, const FloatPoint&, HitTestAction)
{
    ASSERT_NOT_REACHED();
    return false;
}

bool LayoutObject::isRelayoutBoundaryForInspector() const
{
    return objectIsRelayoutBoundary(this);
}

static PaintInvalidationReason documentLifecycleBasedPaintInvalidationReason(const DocumentLifecycle& documentLifecycle)
{
    switch (documentLifecycle.state()) {
    case DocumentLifecycle::InStyleRecalc:
        return PaintInvalidationStyleChange;
    case DocumentLifecycle::InPreLayout:
    case DocumentLifecycle::InPerformLayout:
    case DocumentLifecycle::AfterPerformLayout:
        return PaintInvalidationForcedByLayout;
    case DocumentLifecycle::InCompositingUpdate:
        return PaintInvalidationCompositingUpdate;
    default:
        return PaintInvalidationFull;
    }
}

inline void LayoutObject::markContainerChainForPaintInvalidation()
{
    for (LayoutObject* container = this->containerCrossingFrameBoundaries(); container && !container->shouldCheckForPaintInvalidationRegardlessOfPaintInvalidationState(); container = container->containerCrossingFrameBoundaries())
        container->setSelfMayNeedPaintInvalidation();
}

void LayoutObject::setLayoutDidGetCalledSinceLastFrame()
{
    m_bitfields.setLayoutDidGetCalledSinceLastFrame(true);

    // Make sure our parent is marked as needing invalidation.
    // This would be unneeded if we allowed sub-tree invalidation (akin to sub-tree layouts).
    markContainerChainForPaintInvalidation();
}

void LayoutObject::setShouldInvalidateSelection()
{
    if (!canUpdateSelectionOnRootLineBoxes())
        return;

    m_bitfields.setShouldInvalidateSelection(true);
    markContainerChainForPaintInvalidation();
}

void LayoutObject::setShouldDoFullPaintInvalidation(PaintInvalidationReason reason)
{
    // Only full invalidation reasons are allowed.
    ASSERT(isFullPaintInvalidationReason(reason));

    bool isUpgradingDelayedFullToFull = m_bitfields.fullPaintInvalidationReason() == PaintInvalidationDelayedFull && reason != PaintInvalidationDelayedFull;

    if (m_bitfields.fullPaintInvalidationReason() == PaintInvalidationNone || isUpgradingDelayedFullToFull) {
        if (reason == PaintInvalidationFull)
            reason = documentLifecycleBasedPaintInvalidationReason(document().lifecycle());
        m_bitfields.setFullPaintInvalidationReason(reason);
    }

    if (!isUpgradingDelayedFullToFull) {
        ASSERT(document().lifecycle().state() != DocumentLifecycle::InPaintInvalidation);
        frame()->page()->animator().scheduleVisualUpdate(); // In case that this is called outside of FrameView::updateLayoutAndStyleForPainting().
        markContainerChainForPaintInvalidation();
    }
}

void LayoutObject::setMayNeedPaintInvalidation()
{
    if (mayNeedPaintInvalidation())
        return;
    m_bitfields.setMayNeedPaintInvalidation(true);
    // Make sure our parent is marked as needing invalidation.
    markContainerChainForPaintInvalidation();
    frame()->page()->animator().scheduleVisualUpdate(); // In case that this is called outside of FrameView::updateLayoutAndStyleForPainting().
}

void LayoutObject::clearMayNeedPaintInvalidation()
{
    m_bitfields.setMayNeedPaintInvalidation(false);
}

void LayoutObject::setSelfMayNeedPaintInvalidation()
{
    m_bitfields.setMayNeedPaintInvalidation(true);
}

void LayoutObject::clearPaintInvalidationState(const PaintInvalidationState& paintInvalidationState)
{
    // paintInvalidationStateIsDirty should be kept in sync with the
    // booleans that are cleared below.
    ASSERT(paintInvalidationState.ancestorHadPaintInvalidationForLocationChange() || paintInvalidationStateIsDirty());
    clearShouldDoFullPaintInvalidation();
    setNeededLayoutBecauseOfChildren(false);
    setShouldInvalidateOverflowForPaint(false);
    clearLayoutDidGetCalledSinceLastFrame();
    clearMayNeedPaintInvalidation();
    clearShouldInvalidateSelection();
}

bool LayoutObject::isAllowedToModifyLayoutTreeStructure(Document& document)
{
    return DeprecatedDisableModifyLayoutTreeStructureAsserts::canModifyLayoutTreeStateInAnyState()
        || document.lifecycle().stateAllowsLayoutTreeMutations();
}

DeprecatedDisableModifyLayoutTreeStructureAsserts::DeprecatedDisableModifyLayoutTreeStructureAsserts()
    : m_disabler(gModifyLayoutTreeStructureAnyState, true)
{
}

bool DeprecatedDisableModifyLayoutTreeStructureAsserts::canModifyLayoutTreeStateInAnyState()
{
    return gModifyLayoutTreeStructureAnyState;
}

DisablePaintInvalidationStateAsserts::DisablePaintInvalidationStateAsserts()
    : m_disabler(gDisablePaintInvalidationStateAsserts, true)
{
}

namespace {

// TODO(trchen): Use std::function<void, LayoutObject&> and lambda when available.
class LayoutObjectTraversalFunctor {
public:
    virtual void operator()(LayoutObject&) const = 0;
};

void traverseNonCompositingDescendants(LayoutObject&, const LayoutObjectTraversalFunctor&);

void findNonCompositedDescendantLayerToTraverse(LayoutObject& object, const LayoutObjectTraversalFunctor& functor)
{
    LayoutObject* descendant = object.nextInPreOrder(&object);
    while (descendant) {
        // Case 1: If the descendant has no layer, keep searching until we find a layer.
        if (!descendant->hasLayer()) {
            descendant = descendant->nextInPreOrder(&object);
            continue;
        }
        // Case 2: The descendant has a layer and is not composited.
        // The invalidation container of its subtree is our parent,
        // thus recur into the subtree.
        if (!descendant->isPaintInvalidationContainer()) {
            traverseNonCompositingDescendants(*descendant, functor);
            descendant = descendant->nextInPreOrderAfterChildren(&object);
            continue;
        }
        // Case 3: The descendant is an invalidation container and is a stacking context.
        // No objects in the subtree can have invalidation container outside of it,
        // thus skip the whole subtree.
        if (descendant->styleRef().isStackingContext()) {
            descendant = descendant->nextInPreOrderAfterChildren(&object);
            continue;
        }
        // Case 4: The descendant is an invalidation container but not a stacking context.
        // This is the same situation as the root, thus keep searching.
        descendant = descendant->nextInPreOrder(&object);
    }
}

void traverseNonCompositingDescendants(LayoutObject& object, const LayoutObjectTraversalFunctor& functor)
{
    functor(object);
    LayoutObject* descendant = object.nextInPreOrder(&object);
    while (descendant) {
        if (!descendant->isPaintInvalidationContainer()) {
            functor(*descendant);
            descendant = descendant->nextInPreOrder(&object);
            continue;
        }
        if (descendant->styleRef().isStackingContext()) {
            descendant = descendant->nextInPreOrderAfterChildren(&object);
            continue;
        }

        // If a paint invalidation container is not a stacking context,
        // some of its descendants may belong to the parent container.
        findNonCompositedDescendantLayerToTraverse(*descendant, functor);
        descendant = descendant->nextInPreOrderAfterChildren(&object);
    }
}

} // unnamed namespace

void LayoutObject::invalidateDisplayItemClientForNonCompositingDescendantsOf(const LayoutObject& object) const
{
    ASSERT(RuntimeEnabledFeatures::slimmingPaintEnabled());

    // Not using enclosingCompositedContainer() directly because this object may be in an orphaned subtree.
    const DeprecatedPaintLayer* enclosingLayer = this->enclosingLayer();
    if (!enclosingLayer)
        return;

    // This is valid because we want to invalidate the client in the display item list of the current backing.
    DisableCompositingQueryAsserts disabler;
    const DeprecatedPaintLayer* paintInvalidationLayer = enclosingLayer->enclosingLayerForPaintInvalidationCrossingFrameBoundaries();
    if (!paintInvalidationLayer)
        return;

    class Functor : public LayoutObjectTraversalFunctor {
    public:
        explicit Functor(const LayoutBoxModelObject& paintInvalidationContainer) : m_paintInvalidationContainer(paintInvalidationContainer) { }
        void operator()(LayoutObject& object) const override
        {
            m_paintInvalidationContainer.invalidateDisplayItemClientOnBacking(object);
        }
    private:
        const LayoutBoxModelObject& m_paintInvalidationContainer;
    };

    const LayoutBoxModelObject& paintInvalidationContainer = *paintInvalidationLayer->layoutObject();
    traverseNonCompositingDescendants(const_cast<LayoutObject&>(object), Functor(paintInvalidationContainer));
}

void LayoutObject::invalidatePaintIncludingNonCompositingDescendants()
{
    class Functor : public LayoutObjectTraversalFunctor {
    public:
        explicit Functor(const LayoutBoxModelObject& paintInvalidationContainer) : m_paintInvalidationContainer(paintInvalidationContainer) { }
        void operator()(LayoutObject& object) const override
        {
            object.invalidatePaintUsingContainer(m_paintInvalidationContainer, object.previousPaintInvalidationRect(), PaintInvalidationLayer);
            if (RuntimeEnabledFeatures::slimmingPaintEnabled())
                object.invalidateDisplayItemClients(m_paintInvalidationContainer);
        }
    private:
        const LayoutBoxModelObject& m_paintInvalidationContainer;
    };

    // Since we're only painting non-composited layers, we know that they all share the same paintInvalidationContainer.
    const LayoutBoxModelObject& paintInvalidationContainer = containerForPaintInvalidationOnRootedTree();
    traverseNonCompositingDescendants(*this, Functor(paintInvalidationContainer));
}

// FIXME: If we had a flag to force invalidations in a whole subtree, we could get rid of this function (crbug.com/410097).
void LayoutObject::setShouldDoFullPaintInvalidationIncludingNonCompositingDescendants()
{
    class Functor : public LayoutObjectTraversalFunctor {
    public:
        void operator()(LayoutObject& object) const override
        {
            object.setShouldDoFullPaintInvalidation();
        }
    };

    // Need to access the current compositing status.
    DisableCompositingQueryAsserts disabler;
    traverseNonCompositingDescendants(*this, Functor());
}

void LayoutObject::setIsSlowRepaintObject(bool isSlowRepaintObject)
{
    ASSERT(frameView());
    if (m_bitfields.isSlowRepaintObject() == isSlowRepaintObject)
        return;
    m_bitfields.setIsSlowRepaintObject(isSlowRepaintObject);
    if (isSlowRepaintObject)
        frameView()->addSlowRepaintObject();
    else
        frameView()->removeSlowRepaintObject();
}


void LayoutObject::setSelfNeedsLayout(bool b)
{
    m_bitfields.setSelfNeedsLayout(b);
    // rt_rt_
    Node* n = node();
    if (n && n->isElementNode()) {
        Element* e = (Element*)n;
        String id = e->getIdAttribute();
        if (WTF::kNotFound != id.find("rt_rt_")) {
            OutputDebugStringA("");
        }
    }
}

void LayoutObject::setNeedsPositionedMovementLayout()
{
    bool alreadyNeededLayout = needsPositionedMovementLayout();
    setNeedsPositionedMovementLayout(true);
    ASSERT(!isSetNeedsLayoutForbidden());
    if (!alreadyNeededLayout)
        markContainerChainForLayout();
}

void LayoutObject::setNeedsPositionedMovementLayout(bool b)
{
    m_bitfields.setNeedsPositionedMovementLayout(b);
}

void LayoutObject::setNormalChildNeedsLayout(bool b)
{
    m_bitfields.setNormalChildNeedsLayout(b);

//     if (isLayoutView())
//         OutputDebugStringA("");
// 
//     Node* n = node();
//     if (!n || !n->isElementNode())
//         return;
//     Element* e = (Element*)n;
//     const AtomicString& id = e->getIdAttribute();
//     if (id == "js-player-decorator") {
//         if (b)
//             OutputDebugStringA("setNormalChildNeedsLayout.js-player-decorator.true\n");
//         else
//             OutputDebugStringA("setNormalChildNeedsLayout.js-player-decorator.false\n");
// 
//         if (0) {
//             n->showTreeForThis();
//         }
//     }
}

void LayoutObject::setPosChildNeedsLayout(bool b) { m_bitfields.setPosChildNeedsLayout(b); }
void LayoutObject::setNeedsSimplifiedNormalFlowLayout(bool b) { m_bitfields.setNeedsSimplifiedNormalFlowLayout(b); }
void LayoutObject::setIsDragging(bool b) { m_bitfields.setIsDragging(b); }
void LayoutObject::setEverHadLayout(bool b) { m_bitfields.setEverHadLayout(b); }
void LayoutObject::setShouldInvalidateOverflowForPaint(bool b) { m_bitfields.setShouldInvalidateOverflowForPaint(b); }
void LayoutObject::setSelfNeedsOverflowRecalcAfterStyleChange(bool b) { m_bitfields.setSelfNeedsOverflowRecalcAfterStyleChange(b); }
void LayoutObject::setChildNeedsOverflowRecalcAfterStyleChange(bool b) { m_bitfields.setChildNeedsOverflowRecalcAfterStyleChange(b); }

void LayoutObject::setNeedsLayout(LayoutInvalidationReasonForTracing reason, MarkingBehavior markParents, SubtreeLayoutScope* layouter)
{
    ASSERT(!isSetNeedsLayoutForbidden());
    bool alreadyNeededLayout = m_bitfields.selfNeedsLayout();
    setSelfNeedsLayout(true);
    if (!alreadyNeededLayout) {
        TRACE_EVENT_INSTANT1(
            TRACE_DISABLED_BY_DEFAULT("devtools.timeline.invalidationTracking"),
            "LayoutInvalidationTracking",
            TRACE_EVENT_SCOPE_THREAD,
            "data",
            InspectorLayoutInvalidationTrackingEvent::data(this, reason));
        if (markParents == MarkContainerChain && (!layouter || layouter->root() != this))
            markContainerChainForLayout(true, layouter);
    }
}

void LayoutObject::setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReasonForTracing reason, MarkingBehavior markParents, SubtreeLayoutScope* layouter)
{
    setNeedsLayout(reason, markParents, layouter);
    setShouldDoFullPaintInvalidation();
}

void LayoutObject::setChildNeedsLayout(MarkingBehavior markParents, SubtreeLayoutScope* layouter)
{
    ASSERT(!isSetNeedsLayoutForbidden());
    bool alreadyNeededLayout = normalChildNeedsLayout();
    setNormalChildNeedsLayout(true);
    // FIXME: Replace MarkOnlyThis with the SubtreeLayoutScope code path and remove the MarkingBehavior argument entirely.
    if (!alreadyNeededLayout && markParents == MarkContainerChain && (!layouter || layouter->root() != this))
        markContainerChainForLayout(true, layouter);
}

void LayoutObject::clearNeedsLayout()
{
    setNeededLayoutBecauseOfChildren(needsLayoutBecauseOfChildren());
    setLayoutDidGetCalledSinceLastFrame();
    setSelfNeedsLayout(false);
    setEverHadLayout(true);
    setPosChildNeedsLayout(false);
    setNeedsSimplifiedNormalFlowLayout(false);
    setNormalChildNeedsLayout(false);
    setNeedsPositionedMovementLayout(false);
    setAncestorLineBoxDirty(false);
#if ENABLE(ASSERT)
    checkBlockPositionedObjectsNeedLayout();
#endif
}

bool LayoutObject::normalChildNeedsLayout() const
{ 
    return m_bitfields.normalChildNeedsLayout();
}

bool LayoutObject::selfNeedsLayout() const
{
    return m_bitfields.selfNeedsLayout();
}

#if ENABLE(ASSERT)

void LayoutObject::assertLaidOut() const
{
#ifndef NDEBUG
    if (needsLayout()) {
        //showLayoutTreeForThis();
        OutputDebugStringA("LayoutObject::assertLaidOut fail! \n");
    }
#endif
    //ASSERT_WITH_SECURITY_IMPLICATION(!needsLayout());
}

void LayoutObject::assertSubtreeIsLaidOut() const
{
    for (const LayoutObject* layoutObject = this; layoutObject; layoutObject = layoutObject->nextInPreOrder())
        layoutObject->assertLaidOut();
}

void LayoutObject::assertClearedPaintInvalidationState() const
{
#ifndef NDEBUG
    if (paintInvalidationStateIsDirty()) {
        showLayoutTreeForThis();
        ASSERT_NOT_REACHED();
    }
#endif
}

void LayoutObject::assertSubtreeClearedPaintInvalidationState() const
{
    for (const LayoutObject* layoutObject = this; layoutObject; layoutObject = layoutObject->nextInPreOrder())
        layoutObject->assertClearedPaintInvalidationState();
}

#endif // ENABLE(ASSERT)

} // namespace blink

#ifndef NDEBUG

void showTree(const blink::LayoutObject* object)
{
    if (object)
        object->showTreeForThis();
    else
        fprintf(stderr, "Cannot showTree. Root is (nil)\n");
}

void showLineTree(const blink::LayoutObject* object)
{
    if (object)
        object->showLineTreeForThis();
    else
        fprintf(stderr, "Cannot showLineTree. Root is (nil)\n");
}

void showLayoutTree(const blink::LayoutObject* object1)
{
    showLayoutTree(object1, 0);
}

void showLayoutTree(const blink::LayoutObject* object1, const blink::LayoutObject* object2)
{
    if (object1) {
        const blink::LayoutObject* root = object1;
        while (root->parent())
            root = root->parent();
        root->showLayoutTreeAndMark(object1, "*", object2, "-", 0);
    } else {
        fprintf(stderr, "Cannot showLayoutTree. Root is (nil)\n");
    }
}

#endif