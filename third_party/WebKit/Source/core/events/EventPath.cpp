/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
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
#include "core/events/EventPath.h"

#include "core/EventNames.h"
#include "core/dom/Document.h"
#include "core/dom/Touch.h"
#include "core/dom/TouchList.h"
#include "core/dom/shadow/InsertionPoint.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/events/TouchEvent.h"
#include "core/events/TouchEventContext.h"

namespace blink {

EventTarget* EventPath::eventTargetRespectingTargetRules(Node& referenceNode)
{
    if (referenceNode.isPseudoElement()) {
        ASSERT(referenceNode.parentNode());
        return referenceNode.parentNode();
    }

    return &referenceNode;
}

static inline bool shouldStopAtShadowRoot(Event& event, ShadowRoot& shadowRoot, EventTarget& target)
{
    // WebKit never allowed selectstart event to cross the the shadow DOM boundary.
    // Changing this breaks existing sites.
    // See https://bugs.webkit.org/show_bug.cgi?id=52195 for details.
    const AtomicString eventType = event.type();
    return target.toNode() && target.toNode()->shadowHost() == shadowRoot.host()
        && (eventType == EventTypeNames::abort
            || eventType == EventTypeNames::change
            || eventType == EventTypeNames::error
            || eventType == EventTypeNames::load
            || eventType == EventTypeNames::reset
            || eventType == EventTypeNames::resize
            || eventType == EventTypeNames::scroll
            || eventType == EventTypeNames::select
            || eventType == EventTypeNames::selectstart);
}

EventPath::EventPath(Node& node, Event* event)
    : m_node(node)
    , m_event(event)
{
    initialize();
}

void EventPath::initializeWith(Node& node, Event* event)
{
    m_node = &node;
    m_event = event;
    m_windowEventContext = nullptr;
    m_nodeEventContexts.clear();
    m_treeScopeEventContexts.clear();
    initialize();
}

static inline bool eventPathShouldBeEmptyFor(Node& node)
{
    return node.isPseudoElement() && !node.parentElement();
}

void EventPath::initialize()
{
    if (eventPathShouldBeEmptyFor(*m_node))
        return;
    calculatePath();
    calculateAdjustedTargets();
    calculateTreeScopePrePostOrderNumbers();
}

void EventPath::calculatePath()
{
    ASSERT(m_node);
    ASSERT(m_nodeEventContexts.isEmpty());
    m_node->updateDistribution();

    // For performance and memory usage reasons we want to store the
    // path using as few bytes as possible and with as few allocations
    // as possible which is why we gather the data on the stack before
    // storing it in a perfectly sized m_nodeEventContexts Vector.
    WillBeHeapVector<RawPtrWillBeMember<Node>, 64> nodesInPath;
    Node* current = m_node;
    nodesInPath.append(current);
    while (current) {
        if (m_event && current->keepEventInNode(m_event))
            break;
        WillBeHeapVector<RawPtrWillBeMember<InsertionPoint>, 8> insertionPoints;
        collectDestinationInsertionPoints(*current, insertionPoints);
        if (!insertionPoints.isEmpty()) {
            for (const auto& insertionPoint : insertionPoints) {
                if (insertionPoint->isShadowInsertionPoint()) {
                    ShadowRoot* containingShadowRoot = insertionPoint->containingShadowRoot();
                    ASSERT(containingShadowRoot);
                    if (!containingShadowRoot->isOldest())
                        nodesInPath.append(containingShadowRoot->olderShadowRoot());
                }
                nodesInPath.append(insertionPoint);
            }
            current = insertionPoints.last();
            continue;
        }
        if (current->isShadowRoot()) {
            if (m_event && shouldStopAtShadowRoot(*m_event, *toShadowRoot(current), *m_node))
                break;
            current = current->shadowHost();
            nodesInPath.append(current);
        } else {
            current = current->parentNode();
            if (current)
                nodesInPath.append(current);
        }
    }

    m_nodeEventContexts.reserveCapacity(nodesInPath.size());
    for (Node* nodeInPath : nodesInPath) {
        m_nodeEventContexts.append(NodeEventContext(nodeInPath, eventTargetRespectingTargetRules(*nodeInPath)));
    }
}

void EventPath::calculateTreeScopePrePostOrderNumbers()
{
    // Precondition:
    //   - TreeScopes in m_treeScopeEventContexts must be *connected* in the same tree of trees.
    //   - The root tree must be included.
    WillBeHeapHashMap<RawPtrWillBeMember<const TreeScope>, RawPtrWillBeMember<TreeScopeEventContext>> treeScopeEventContextMap;
    for (const auto& treeScopeEventContext : m_treeScopeEventContexts)
        treeScopeEventContextMap.add(&treeScopeEventContext->treeScope(), treeScopeEventContext.get());
    TreeScopeEventContext* rootTree = 0;
    for (const auto& treeScopeEventContext : m_treeScopeEventContexts) {
        // Use olderShadowRootOrParentTreeScope here for parent-child relationships.
        // See the definition of trees of trees in the Shado DOM spec: http://w3c.github.io/webcomponents/spec/shadow/
        TreeScope* parent = treeScopeEventContext.get()->treeScope().olderShadowRootOrParentTreeScope();
        if (!parent) {
            ASSERT(!rootTree);
            rootTree = treeScopeEventContext.get();
            continue;
        }
        ASSERT(treeScopeEventContextMap.find(parent) != treeScopeEventContextMap.end());
        treeScopeEventContextMap.find(parent)->value->addChild(*treeScopeEventContext.get());
    }
    ASSERT(rootTree);
    rootTree->calculatePrePostOrderNumber(0);
}

TreeScopeEventContext* EventPath::ensureTreeScopeEventContext(Node* currentTarget, TreeScope* treeScope, TreeScopeEventContextMap& treeScopeEventContextMap)
{
    if (!treeScope)
        return 0;
    TreeScopeEventContext* treeScopeEventContext;
    bool isNewEntry;
    {
        TreeScopeEventContextMap::AddResult addResult = treeScopeEventContextMap.add(treeScope, nullptr);
        isNewEntry = addResult.isNewEntry;
        if (isNewEntry)
            addResult.storedValue->value = TreeScopeEventContext::create(*treeScope);
        treeScopeEventContext = addResult.storedValue->value.get();
    }
    if (isNewEntry) {
        TreeScopeEventContext* parentTreeScopeEventContext = ensureTreeScopeEventContext(0, treeScope->olderShadowRootOrParentTreeScope(), treeScopeEventContextMap);
        if (parentTreeScopeEventContext && parentTreeScopeEventContext->target()) {
            treeScopeEventContext->setTarget(parentTreeScopeEventContext->target());
        } else if (currentTarget) {
            treeScopeEventContext->setTarget(eventTargetRespectingTargetRules(*currentTarget));
        }
    } else if (!treeScopeEventContext->target() && currentTarget) {
        treeScopeEventContext->setTarget(eventTargetRespectingTargetRules(*currentTarget));
    }
    return treeScopeEventContext;
}

void EventPath::calculateAdjustedTargets()
{
    const TreeScope* lastTreeScope = 0;

    TreeScopeEventContextMap treeScopeEventContextMap;
    TreeScopeEventContext* lastTreeScopeEventContext = 0;

    for (size_t i = 0; i < size(); ++i) {
        Node* currentNode = at(i).node();
        TreeScope& currentTreeScope = currentNode->treeScope();
        if (lastTreeScope != &currentTreeScope) {
            lastTreeScopeEventContext = ensureTreeScopeEventContext(currentNode, &currentTreeScope, treeScopeEventContextMap);
        }
        ASSERT(lastTreeScopeEventContext);
        at(i).setTreeScopeEventContext(lastTreeScopeEventContext);
        lastTreeScope = &currentTreeScope;
    }
    m_treeScopeEventContexts.appendRange(treeScopeEventContextMap.values().begin(), treeScopeEventContextMap.values().end());
}

void EventPath::buildRelatedNodeMap(const Node& relatedNode, RelatedTargetMap& relatedTargetMap)
{
    OwnPtrWillBeRawPtr<EventPath> relatedTargetEventPath = adoptPtrWillBeNoop(new EventPath(const_cast<Node&>(relatedNode)));
    for (size_t i = 0; i < relatedTargetEventPath->m_treeScopeEventContexts.size(); ++i) {
        TreeScopeEventContext* treeScopeEventContext = relatedTargetEventPath->m_treeScopeEventContexts[i].get();
        relatedTargetMap.add(&treeScopeEventContext->treeScope(), treeScopeEventContext->target());
    }
#if ENABLE(OILPAN)
    // Oilpan: It is important to explicitly clear the vectors to reuse
    // the memory in subsequent event dispatchings.
    relatedTargetEventPath->clear();
#endif
}

EventTarget* EventPath::findRelatedNode(TreeScope& scope, RelatedTargetMap& relatedTargetMap)
{
    WillBeHeapVector<RawPtrWillBeMember<TreeScope>, 32> parentTreeScopes;
    EventTarget* relatedNode = 0;
    for (TreeScope* current = &scope; current; current = current->olderShadowRootOrParentTreeScope()) {
        parentTreeScopes.append(current);
        RelatedTargetMap::const_iterator iter = relatedTargetMap.find(current);
        if (iter != relatedTargetMap.end() && iter->value) {
            relatedNode = iter->value;
            break;
        }
    }
    ASSERT(relatedNode);
    for (WillBeHeapVector<RawPtrWillBeMember<TreeScope>, 32>::iterator iter = parentTreeScopes.begin(); iter < parentTreeScopes.end(); ++iter)
        relatedTargetMap.add(*iter, relatedNode);
    return relatedNode;
}

void EventPath::adjustForRelatedTarget(Node& target, EventTarget* relatedTarget)
{
    if (!relatedTarget)
        return;
    Node* relatedNode = relatedTarget->toNode();
    if (!relatedNode)
        return;
    if (target.document() != relatedNode->document())
        return;
    if (!target.inDocument() || !relatedNode->inDocument())
        return;

    RelatedTargetMap relatedNodeMap;
    buildRelatedNodeMap(*relatedNode, relatedNodeMap);

    for (const auto& treeScopeEventContext : m_treeScopeEventContexts) {
        EventTarget* adjustedRelatedTarget = findRelatedNode(treeScopeEventContext->treeScope(), relatedNodeMap);
        ASSERT(adjustedRelatedTarget);
        treeScopeEventContext.get()->setRelatedTarget(adjustedRelatedTarget);
    }

    shrinkIfNeeded(target, *relatedTarget);
}

void EventPath::shrinkIfNeeded(const Node& target, const EventTarget& relatedTarget)
{
    // Synthetic mouse events can have a relatedTarget which is identical to the target.
    bool targetIsIdenticalToToRelatedTarget = (&target == &relatedTarget);

    for (size_t i = 0; i < size(); ++i) {
        if (targetIsIdenticalToToRelatedTarget) {
            if (target.treeScope().rootNode() == at(i).node()) {
                shrink(i + 1);
                break;
            }
        } else if (at(i).target() == at(i).relatedTarget()) {
            // Event dispatching should be stopped here.
            shrink(i);
            break;
        }
    }
}

void EventPath::adjustForTouchEvent(TouchEvent& touchEvent)
{
    WillBeHeapVector<RawPtrWillBeMember<TouchList>> adjustedTouches;
    WillBeHeapVector<RawPtrWillBeMember<TouchList>> adjustedTargetTouches;
    WillBeHeapVector<RawPtrWillBeMember<TouchList>> adjustedChangedTouches;
    WillBeHeapVector<RawPtrWillBeMember<TreeScope>> treeScopes;

    for (const auto& treeScopeEventContext : m_treeScopeEventContexts) {
        TouchEventContext* touchEventContext = treeScopeEventContext->ensureTouchEventContext();
        adjustedTouches.append(&touchEventContext->touches());
        adjustedTargetTouches.append(&touchEventContext->targetTouches());
        adjustedChangedTouches.append(&touchEventContext->changedTouches());
        treeScopes.append(&treeScopeEventContext->treeScope());
    }

    adjustTouchList(touchEvent.touches(), adjustedTouches, treeScopes);
    adjustTouchList(touchEvent.targetTouches(), adjustedTargetTouches, treeScopes);
    adjustTouchList(touchEvent.changedTouches(), adjustedChangedTouches, treeScopes);

#if ENABLE(ASSERT)
    for (const auto& treeScopeEventContext : m_treeScopeEventContexts) {
        TreeScope& treeScope = treeScopeEventContext->treeScope();
        TouchEventContext* touchEventContext = treeScopeEventContext->touchEventContext();
        checkReachability(treeScope, touchEventContext->touches());
        checkReachability(treeScope, touchEventContext->targetTouches());
        checkReachability(treeScope, touchEventContext->changedTouches());
    }
#endif
}

void EventPath::adjustTouchList(const TouchList* touchList, WillBeHeapVector<RawPtrWillBeMember<TouchList>> adjustedTouchList, const WillBeHeapVector<RawPtrWillBeMember<TreeScope>>& treeScopes)
{
    if (!touchList)
        return;
    for (size_t i = 0; i < touchList->length(); ++i) {
        const Touch& touch = *touchList->item(i);
        RelatedTargetMap relatedNodeMap;
        buildRelatedNodeMap(*touch.target()->toNode(), relatedNodeMap);
        for (size_t j = 0; j < treeScopes.size(); ++j) {
            adjustedTouchList[j]->append(touch.cloneWithNewTarget(findRelatedNode(*treeScopes[j], relatedNodeMap)));
        }
    }
}

const NodeEventContext& EventPath::topNodeEventContext()
{
    ASSERT(!isEmpty());
    return last();
}

void EventPath::ensureWindowEventContext()
{
    ASSERT(m_event);
    if (!m_windowEventContext)
        m_windowEventContext = adoptPtrWillBeNoop(new WindowEventContext(*m_event, topNodeEventContext()));
}

#if ENABLE(ASSERT)
void EventPath::checkReachability(TreeScope& treeScope, TouchList& touchList)
{
    for (size_t i = 0; i < touchList.length(); ++i)
        ASSERT(touchList.item(i)->target()->toNode()->treeScope().isInclusiveOlderSiblingShadowRootOrAncestorTreeScopeOf(treeScope));
}
#endif

DEFINE_TRACE(EventPath)
{
#if ENABLE(OILPAN)
    visitor->trace(m_nodeEventContexts);
    visitor->trace(m_node);
    visitor->trace(m_event);
    visitor->trace(m_treeScopeEventContexts);
    visitor->trace(m_windowEventContext);
#endif
}

} // namespace
