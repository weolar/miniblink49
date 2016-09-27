/**
 * Copyright (C) 2004 Allan Sandfeld Jensen (kde@carewolf.com)
 * Copyright (C) 2006, 2007 Apple Inc. All rights reserved.
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
#include "core/layout/LayoutCounter.h"

#include "core/HTMLNames.h"
#include "core/dom/Element.h"
#include "core/dom/ElementTraversal.h"
#include "core/html/HTMLOListElement.h"
#include "core/layout/CounterNode.h"
#include "core/layout/LayoutListItem.h"
#include "core/layout/LayoutListMarker.h"
#include "core/layout/LayoutView.h"
#include "core/style/ComputedStyle.h"
#include "wtf/StdLibExtras.h"

#ifndef NDEBUG
#include <stdio.h>
#endif

namespace blink {

using namespace HTMLNames;

typedef HashMap<AtomicString, RefPtr<CounterNode>> CounterMap;
typedef HashMap<const LayoutObject*, OwnPtr<CounterMap>> CounterMaps;

static CounterNode* makeCounterNode(LayoutObject&, const AtomicString& identifier, bool alwaysCreateCounter);

static CounterMaps& counterMaps()
{
    DEFINE_STATIC_LOCAL(CounterMaps, staticCounterMaps, ());
    return staticCounterMaps;
}

// This function processes the layoutObject tree in the order of the DOM tree
// including pseudo elements as defined in CSS 2.1.
static LayoutObject* previousInPreOrder(const LayoutObject& object)
{
    Element* self = toElement(object.node());
    ASSERT(self);
    Element* previous = ElementTraversal::previousIncludingPseudo(*self);
    while (previous && !previous->layoutObject())
        previous = ElementTraversal::previousIncludingPseudo(*previous);
    return previous ? previous->layoutObject() : 0;
}

// This function processes the layoutObject tree in the order of the DOM tree
// including pseudo elements as defined in CSS 2.1.
static LayoutObject* previousSiblingOrParent(const LayoutObject& object)
{
    Element* self = toElement(object.node());
    ASSERT(self);
    Element* previous = ElementTraversal::pseudoAwarePreviousSibling(*self);
    while (previous && !previous->layoutObject())
        previous = ElementTraversal::pseudoAwarePreviousSibling(*previous);
    if (previous)
        return previous->layoutObject();
    previous = self->parentElement();
    return previous ? previous->layoutObject() : 0;
}

static inline Element* parentElement(LayoutObject& object)
{
    return toElement(object.node())->parentElement();
}

static inline bool areLayoutObjectsElementsSiblings(LayoutObject& first, LayoutObject& second)
{
    return parentElement(first) == parentElement(second);
}

// This function processes the layoutObject tree in the order of the DOM tree
// including pseudo elements as defined in CSS 2.1.
static LayoutObject* nextInPreOrder(const LayoutObject& object, const Element* stayWithin, bool skipDescendants = false)
{
    Element* self = toElement(object.node());
    ASSERT(self);
    Element* next = skipDescendants ? ElementTraversal::nextIncludingPseudoSkippingChildren(*self, stayWithin) : ElementTraversal::nextIncludingPseudo(*self, stayWithin);
    while (next && !next->layoutObject())
        next = skipDescendants ? ElementTraversal::nextIncludingPseudoSkippingChildren(*next, stayWithin) : ElementTraversal::nextIncludingPseudo(*next, stayWithin);
    return next ? next->layoutObject() : 0;
}

static bool planCounter(LayoutObject& object, const AtomicString& identifier, bool& isReset, int& value)
{
    // Real text nodes don't have their own style so they can't have counters.
    // We can't even look at their styles or we'll see extra resets and increments!
    if (object.isText() && !object.isBR())
        return false;
    Node* generatingNode = object.generatingNode();
    // We must have a generating node or else we cannot have a counter.
    if (!generatingNode)
        return false;
    const ComputedStyle& style = object.styleRef();

    switch (style.styleType()) {
    case NOPSEUDO:
        // Sometimes nodes have more then one layoutObject. Only the first one gets the counter
        // LayoutTests/http/tests/css/counter-crash.html
        if (generatingNode->layoutObject() != &object)
            return false;
        break;
    case BEFORE:
    case AFTER:
        break;
    default:
        return false; // Counters are forbidden from all other pseudo elements.
    }

    const CounterDirectives directives = style.getCounterDirectives(identifier);
    if (directives.isDefined()) {
        value = directives.combinedValue();
        isReset = directives.isReset();
        return true;
    }

    if (identifier == "list-item") {
        if (object.isListItem()) {
            if (toLayoutListItem(object).hasExplicitValue()) {
                value = toLayoutListItem(object).explicitValue();
                isReset = true;
                return true;
            }
            value = 1;
            isReset = false;
            return true;
        }
        if (Node* e = object.node()) {
            if (isHTMLOListElement(*e)) {
                value = toHTMLOListElement(e)->start();
                isReset = true;
                return true;
            }
            if (isHTMLUListElement(*e) || isHTMLMenuElement(*e) || isHTMLDirectoryElement(*e)) {
                value = 0;
                isReset = true;
                return true;
            }
        }
    }

    return false;
}

// - Finds the insertion point for the counter described by counterOwner, isReset and
// identifier in the CounterNode tree for identifier and sets parent and
// previousSibling accordingly.
// - The function returns true if the counter whose insertion point is searched is NOT
// the root of the tree.
// - The root of the tree is a counter reference that is not in the scope of any other
// counter with the same identifier.
// - All the counter references with the same identifier as this one that are in
// children or subsequent siblings of the layoutObject that owns the root of the tree
// form the rest of of the nodes of the tree.
// - The root of the tree is always a reset type reference.
// - A subtree rooted at any reset node in the tree is equivalent to all counter
// references that are in the scope of the counter or nested counter defined by that
// reset node.
// - Non-reset CounterNodes cannot have descendants.

static bool findPlaceForCounter(LayoutObject& counterOwner, const AtomicString& identifier, bool isReset, RefPtr<CounterNode>& parent, RefPtr<CounterNode>& previousSibling)
{
    // We cannot stop searching for counters with the same identifier before we also
    // check this layoutObject, because it may affect the positioning in the tree of our counter.
    LayoutObject* searchEndLayoutObject = previousSiblingOrParent(counterOwner);
    // We check layoutObjects in preOrder from the layoutObject that our counter is attached to
    // towards the begining of the document for counters with the same identifier as the one
    // we are trying to find a place for. This is the next layoutObject to be checked.
    LayoutObject* currentLayoutObject = previousInPreOrder(counterOwner);
    previousSibling = nullptr;
    RefPtr<CounterNode> previousSiblingProtector = nullptr;

    while (currentLayoutObject) {
        CounterNode* currentCounter = makeCounterNode(*currentLayoutObject, identifier, false);
        if (searchEndLayoutObject == currentLayoutObject) {
            // We may be at the end of our search.
            if (currentCounter) {
                // We have a suitable counter on the EndSearchLayoutObject.
                if (previousSiblingProtector) { // But we already found another counter that we come after.
                    if (currentCounter->actsAsReset()) {
                        // We found a reset counter that is on a layoutObject that is a sibling of ours or a parent.
                        if (isReset && areLayoutObjectsElementsSiblings(*currentLayoutObject, counterOwner)) {
                            // We are also a reset counter and the previous reset was on a sibling layoutObject
                            // hence we are the next sibling of that counter if that reset is not a root or
                            // we are a root node if that reset is a root.
                            parent = currentCounter->parent();
                            previousSibling = parent ? currentCounter : 0;
                            return parent;
                        }
                        // We are not a reset node or the previous reset must be on an ancestor of our owner layoutObject
                        // hence we must be a child of that reset counter.
                        parent = currentCounter;
                        // In some cases layoutObjects can be reparented (ex. nodes inside a table but not in a column or row).
                        // In these cases the identified previousSibling will be invalid as its parent is different from
                        // our identified parent.
                        if (previousSiblingProtector->parent() != currentCounter)
                            previousSiblingProtector = nullptr;

                        previousSibling = previousSiblingProtector.get();
                        return true;
                    }
                    // CurrentCounter, the counter at the EndSearchLayoutObject, is not reset.
                    if (!isReset || !areLayoutObjectsElementsSiblings(*currentLayoutObject, counterOwner)) {
                        // If the node we are placing is not reset or we have found a counter that is attached
                        // to an ancestor of the placed counter's owner layoutObject we know we are a sibling of that node.
                        if (currentCounter->parent() != previousSiblingProtector->parent())
                            return false;

                        parent = currentCounter->parent();
                        previousSibling = previousSiblingProtector.get();
                        return true;
                    }
                } else {
                    // We are at the potential end of the search, but we had no previous sibling candidate
                    // In this case we follow pretty much the same logic as above but no ASSERTs about
                    // previousSibling, and when we are a sibling of the end counter we must set previousSibling
                    // to currentCounter.
                    if (currentCounter->actsAsReset()) {
                        if (isReset && areLayoutObjectsElementsSiblings(*currentLayoutObject, counterOwner)) {
                            parent = currentCounter->parent();
                            previousSibling = currentCounter;
                            return parent;
                        }
                        parent = currentCounter;
                        previousSibling = previousSiblingProtector.get();
                        return true;
                    }
                    if (!isReset || !areLayoutObjectsElementsSiblings(*currentLayoutObject, counterOwner)) {
                        parent = currentCounter->parent();
                        previousSibling = currentCounter;
                        return true;
                    }
                    previousSiblingProtector = currentCounter;
                }
            }
            // We come here if the previous sibling or parent of our owner layoutObject had no
            // good counter, or we are a reset node and the counter on the previous sibling
            // of our owner layoutObject was not a reset counter.
            // Set a new goal for the end of the search.
            searchEndLayoutObject = previousSiblingOrParent(*currentLayoutObject);
        } else {
            // We are searching descendants of a previous sibling of the layoutObject that the
            // counter being placed is attached to.
            if (currentCounter) {
                // We found a suitable counter.
                if (previousSiblingProtector) {
                    // Since we had a suitable previous counter before, we should only consider this one as our
                    // previousSibling if it is a reset counter and hence the current previousSibling is its child.
                    if (currentCounter->actsAsReset()) {
                        previousSiblingProtector = currentCounter;
                        // We are no longer interested in previous siblings of the currentLayoutObject or their children
                        // as counters they may have attached cannot be the previous sibling of the counter we are placing.
                        currentLayoutObject = parentElement(*currentLayoutObject)->layoutObject();
                        continue;
                    }
                } else {
                    previousSiblingProtector = currentCounter;
                }
                currentLayoutObject = previousSiblingOrParent(*currentLayoutObject);
                continue;
            }
        }
        // This function is designed so that the same test is not done twice in an iteration, except for this one
        // which may be done twice in some cases. Rearranging the decision points though, to accommodate this
        // performance improvement would create more code duplication than is worthwhile in my oppinion and may further
        // impede the readability of this already complex algorithm.
        if (previousSiblingProtector)
            currentLayoutObject = previousSiblingOrParent(*currentLayoutObject);
        else
            currentLayoutObject = previousInPreOrder(*currentLayoutObject);
    }
    return false;
}

static CounterNode* makeCounterNode(LayoutObject& object, const AtomicString& identifier, bool alwaysCreateCounter)
{
    if (object.hasCounterNodeMap()) {
        if (CounterMap* nodeMap = counterMaps().get(&object)) {
            if (CounterNode* node = nodeMap->get(identifier))
                return node;
        }
    }

    bool isReset = false;
    int value = 0;
    if (!planCounter(object, identifier, isReset, value) && !alwaysCreateCounter)
        return nullptr;

    RefPtr<CounterNode> newParent = nullptr;
    RefPtr<CounterNode> newPreviousSibling = nullptr;
    RefPtr<CounterNode> newNode = CounterNode::create(object, isReset, value);
    if (findPlaceForCounter(object, identifier, isReset, newParent, newPreviousSibling))
        newParent->insertAfter(newNode.get(), newPreviousSibling.get(), identifier);
    CounterMap* nodeMap;
    if (object.hasCounterNodeMap()) {
        nodeMap = counterMaps().get(&object);
    } else {
        nodeMap = new CounterMap;
        counterMaps().set(&object, adoptPtr(nodeMap));
        object.setHasCounterNodeMap(true);
    }
    nodeMap->set(identifier, newNode);
    if (newNode->parent())
        return newNode.get();
    // Checking if some nodes that were previously counter tree root nodes
    // should become children of this node now.
    CounterMaps& maps = counterMaps();
    Element* stayWithin = parentElement(object);
    bool skipDescendants;
    for (LayoutObject* currentLayoutObject = nextInPreOrder(object, stayWithin); currentLayoutObject; currentLayoutObject = nextInPreOrder(*currentLayoutObject, stayWithin, skipDescendants)) {
        skipDescendants = false;
        if (!currentLayoutObject->hasCounterNodeMap())
            continue;
        CounterNode* currentCounter = maps.get(currentLayoutObject)->get(identifier);
        if (!currentCounter)
            continue;
        skipDescendants = true;
        if (currentCounter->parent())
            continue;
        if (stayWithin == parentElement(*currentLayoutObject) && currentCounter->hasResetType())
            break;
        newNode->insertAfter(currentCounter, newNode->lastChild(), identifier);
    }
    return newNode.get();
}

LayoutCounter::LayoutCounter(Document* node, const CounterContent& counter)
    : LayoutText(node, StringImpl::empty())
    , m_counter(counter)
    , m_counterNode(nullptr)
    , m_nextForSameCounter(nullptr)
{
    view()->addLayoutCounter();
}

LayoutCounter::~LayoutCounter()
{
}

void LayoutCounter::willBeDestroyed()
{
    if (m_counterNode) {
        m_counterNode->removeLayoutObject(this);
        ASSERT(!m_counterNode);
    }
    if (view())
        view()->removeLayoutCounter();
    LayoutText::willBeDestroyed();
}

PassRefPtr<StringImpl> LayoutCounter::originalText() const
{
    if (!m_counterNode) {
        LayoutObject* beforeAfterContainer = parent();
        while (true) {
            if (!beforeAfterContainer)
                return nullptr;
            if (!beforeAfterContainer->isAnonymous() && !beforeAfterContainer->isPseudoElement())
                return nullptr; // LayoutCounters are restricted to before and after pseudo elements
            PseudoId containerStyle = beforeAfterContainer->style()->styleType();
            if ((containerStyle == BEFORE) || (containerStyle == AFTER))
                break;
            beforeAfterContainer = beforeAfterContainer->parent();
        }
        makeCounterNode(*beforeAfterContainer, m_counter.identifier(), true)->addLayoutObject(const_cast<LayoutCounter*>(this));
        ASSERT(m_counterNode);
    }
    CounterNode* child = m_counterNode;
    int value = child->actsAsReset() ? child->value() : child->countInParent();

    String text = listMarkerText(m_counter.listStyle(), value);

    if (!m_counter.separator().isNull()) {
        if (!child->actsAsReset())
            child = child->parent();
        while (CounterNode* parent = child->parent()) {
            text = listMarkerText(m_counter.listStyle(), child->countInParent())
                + m_counter.separator() + text;
            child = parent;
        }
    }

    return text.impl();
}

void LayoutCounter::updateCounter()
{
    setText(originalText());
}

void LayoutCounter::invalidate()
{
    m_counterNode->removeLayoutObject(this);
    ASSERT(!m_counterNode);
    if (documentBeingDestroyed())
        return;
    setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::CountersChanged);
}

static void destroyCounterNodeWithoutMapRemoval(const AtomicString& identifier, CounterNode* node)
{
    CounterNode* previous;
    for (RefPtr<CounterNode> child = node->lastDescendant(); child && child != node; child = previous) {
        previous = child->previousInPreOrder();
        child->parent()->removeChild(child.get());
        ASSERT(counterMaps().get(&child->owner())->get(identifier) == child);
        counterMaps().get(&child->owner())->remove(identifier);
    }
    if (CounterNode* parent = node->parent())
        parent->removeChild(node);
}

void LayoutCounter::destroyCounterNodes(LayoutObject& owner)
{
    CounterMaps& maps = counterMaps();
    CounterMaps::iterator mapsIterator = maps.find(&owner);
    if (mapsIterator == maps.end())
        return;
    CounterMap* map = mapsIterator->value.get();
    CounterMap::const_iterator end = map->end();
    for (CounterMap::const_iterator it = map->begin(); it != end; ++it) {
        destroyCounterNodeWithoutMapRemoval(it->key, it->value.get());
    }
    maps.remove(mapsIterator);
    owner.setHasCounterNodeMap(false);
}

void LayoutCounter::destroyCounterNode(LayoutObject& owner, const AtomicString& identifier)
{
    CounterMap* map = counterMaps().get(&owner);
    if (!map)
        return;
    CounterMap::iterator mapIterator = map->find(identifier);
    if (mapIterator == map->end())
        return;
    destroyCounterNodeWithoutMapRemoval(identifier, mapIterator->value.get());
    map->remove(mapIterator);
    // We do not delete "map" here even if empty because we expect to reuse
    // it soon. In order for a layoutObject to lose all its counters permanently,
    // a style change for the layoutObject involving removal of all counter
    // directives must occur, in which case, LayoutCounter::destroyCounterNodes()
    // must be called.
    // The destruction of the LayoutObject (possibly caused by the removal of its
    // associated DOM node) is the other case that leads to the permanent
    // destruction of all counters attached to a LayoutObject. In this case
    // LayoutCounter::destroyCounterNodes() must be and is now called, too.
    // LayoutCounter::destroyCounterNodes() handles destruction of the counter
    // map associated with a layoutObject, so there is no risk in leaking the map.
}

void LayoutCounter::layoutObjectSubtreeWillBeDetached(LayoutObject* layoutObject)
{
    ASSERT(layoutObject->view());
    if (!layoutObject->view()->hasLayoutCounters())
        return;
    LayoutObject* currentLayoutObject = layoutObject->lastLeafChild();
    if (!currentLayoutObject)
        currentLayoutObject = layoutObject;
    while (true) {
        destroyCounterNodes(*currentLayoutObject);
        if (currentLayoutObject == layoutObject)
            break;
        currentLayoutObject = currentLayoutObject->previousInPreOrder();
    }
}

static void updateCounters(LayoutObject& layoutObject)
{
    ASSERT(layoutObject.style());
    const CounterDirectiveMap* directiveMap = layoutObject.style()->counterDirectives();
    if (!directiveMap)
        return;
    CounterDirectiveMap::const_iterator end = directiveMap->end();
    if (!layoutObject.hasCounterNodeMap()) {
        for (CounterDirectiveMap::const_iterator it = directiveMap->begin(); it != end; ++it)
            makeCounterNode(layoutObject, it->key, false);
        return;
    }
    CounterMap* counterMap = counterMaps().get(&layoutObject);
    ASSERT(counterMap);
    for (CounterDirectiveMap::const_iterator it = directiveMap->begin(); it != end; ++it) {
        RefPtr<CounterNode> node = counterMap->get(it->key);
        if (!node) {
            makeCounterNode(layoutObject, it->key, false);
            continue;
        }
        RefPtr<CounterNode> newParent = nullptr;
        RefPtr<CounterNode> newPreviousSibling = nullptr;

        findPlaceForCounter(layoutObject, it->key, node->hasResetType(), newParent, newPreviousSibling);
        if (node != counterMap->get(it->key))
            continue;
        CounterNode* parent = node->parent();
        if (newParent == parent && newPreviousSibling == node->previousSibling())
            continue;
        if (parent)
            parent->removeChild(node.get());
        if (newParent)
            newParent->insertAfter(node.get(), newPreviousSibling.get(), it->key);
    }
}

void LayoutCounter::layoutObjectSubtreeAttached(LayoutObject* layoutObject)
{
    ASSERT(layoutObject->view());
    if (!layoutObject->view()->hasLayoutCounters())
        return;
    Node* node = layoutObject->node();
    if (node)
        node = node->parentNode();
    else
        node = layoutObject->generatingNode();
    if (node && node->needsAttach())
        return; // No need to update if the parent is not attached yet
    for (LayoutObject* descendant = layoutObject; descendant; descendant = descendant->nextInPreOrder(layoutObject))
        updateCounters(*descendant);
}

void LayoutCounter::layoutObjectStyleChanged(LayoutObject& layoutObject, const ComputedStyle* oldStyle, const ComputedStyle& newStyle)
{
    Node* node = layoutObject.generatingNode();
    if (!node || node->needsAttach())
        return; // cannot have generated content or if it can have, it will be handled during attaching
    const CounterDirectiveMap* oldCounterDirectives = oldStyle ? oldStyle->counterDirectives() : 0;
    const CounterDirectiveMap* newCounterDirectives = newStyle.counterDirectives();
    if (oldCounterDirectives) {
        if (newCounterDirectives) {
            CounterDirectiveMap::const_iterator newMapEnd = newCounterDirectives->end();
            CounterDirectiveMap::const_iterator oldMapEnd = oldCounterDirectives->end();
            for (CounterDirectiveMap::const_iterator it = newCounterDirectives->begin(); it != newMapEnd; ++it) {
                CounterDirectiveMap::const_iterator oldMapIt = oldCounterDirectives->find(it->key);
                if (oldMapIt != oldMapEnd) {
                    if (oldMapIt->value == it->value)
                        continue;
                    LayoutCounter::destroyCounterNode(layoutObject, it->key);
                }
                // We must create this node here, because the changed node may be a node with no display such as
                // as those created by the increment or reset directives and the re-layout that will happen will
                // not catch the change if the node had no children.
                makeCounterNode(layoutObject, it->key, false);
            }
            // Destroying old counters that do not exist in the new counterDirective map.
            for (CounterDirectiveMap::const_iterator it = oldCounterDirectives->begin(); it !=oldMapEnd; ++it) {
                if (!newCounterDirectives->contains(it->key))
                    LayoutCounter::destroyCounterNode(layoutObject, it->key);
            }
        } else {
            if (layoutObject.hasCounterNodeMap())
                LayoutCounter::destroyCounterNodes(layoutObject);
        }
    } else if (newCounterDirectives) {
        if (layoutObject.hasCounterNodeMap())
            LayoutCounter::destroyCounterNodes(layoutObject);
        CounterDirectiveMap::const_iterator newMapEnd = newCounterDirectives->end();
        for (CounterDirectiveMap::const_iterator it = newCounterDirectives->begin(); it != newMapEnd; ++it) {
            // We must create this node here, because the added node may be a node with no display such as
            // as those created by the increment or reset directives and the re-layout that will happen will
            // not catch the change if the node had no children.
            makeCounterNode(layoutObject, it->key, false);
        }
    }
}

} // namespace blink

#ifndef NDEBUG

void showCounterLayoutObjectTree(const blink::LayoutObject* layoutObject, const char* counterName)
{
    if (!layoutObject)
        return;
    const blink::LayoutObject* root = layoutObject;
    while (root->parent())
        root = root->parent();

    AtomicString identifier(counterName);
    for (const blink::LayoutObject* current = root; current; current = current->nextInPreOrder()) {
        fprintf(stderr, "%c", (current == layoutObject) ? '*' : ' ');
        for (const blink::LayoutObject* parent = current; parent && parent != root; parent = parent->parent())
            fprintf(stderr, "    ");
        fprintf(stderr, "%p N:%p P:%p PS:%p NS:%p C:%p\n",
            current, current->node(), current->parent(), current->previousSibling(),
            current->nextSibling(), current->hasCounterNodeMap() ?
            counterName ? blink::counterMaps().get(current)->get(identifier) : (blink::CounterNode*)1 : (blink::CounterNode*)0);
    }
    fflush(stderr);
}

#endif // NDEBUG
