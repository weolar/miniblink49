/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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
#include "core/dom/shadow/ElementShadow.h"

#include "core/css/StyleSheetList.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/NodeTraversal.h"
#include "core/dom/shadow/DistributedNodes.h"
#include "core/frame/UseCounter.h"
#include "core/html/HTMLContentElement.h"
#include "core/html/HTMLShadowElement.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "platform/EventDispatchForbiddenScope.h"
#include "platform/ScriptForbiddenScope.h"

namespace blink {

class DistributionPool final {
    STACK_ALLOCATED();
public:
    explicit DistributionPool(const ContainerNode&);
    void clear();
    ~DistributionPool();
    void distributeTo(InsertionPoint*, ElementShadow*);
    void populateChildren(const ContainerNode&);

private:
    void detachNonDistributedNodes();
    WillBeHeapVector<RawPtrWillBeMember<Node>, 32> m_nodes;
    Vector<bool, 32> m_distributed;
};

inline DistributionPool::DistributionPool(const ContainerNode& parent)
{
    populateChildren(parent);
}

inline void DistributionPool::clear()
{
    detachNonDistributedNodes();
    m_nodes.clear();
    m_distributed.clear();
}

inline void DistributionPool::populateChildren(const ContainerNode& parent)
{
    clear();
    for (Node* child = parent.firstChild(); child; child = child->nextSibling()) {
        if (isActiveInsertionPoint(*child)) {
            InsertionPoint* insertionPoint = toInsertionPoint(child);
            for (size_t i = 0; i < insertionPoint->distributedNodesSize(); ++i)
                m_nodes.append(insertionPoint->distributedNodeAt(i));
        } else {
            m_nodes.append(child);
        }
    }
    m_distributed.resize(m_nodes.size());
    m_distributed.fill(false);
}

void DistributionPool::distributeTo(InsertionPoint* insertionPoint, ElementShadow* elementShadow)
{
    DistributedNodes distributedNodes;

    for (size_t i = 0; i < m_nodes.size(); ++i) {
        if (m_distributed[i])
            continue;

        if (isHTMLContentElement(*insertionPoint) && !toHTMLContentElement(insertionPoint)->canSelectNode(m_nodes, i))
            continue;

        Node* node = m_nodes[i];
        distributedNodes.append(node);
        elementShadow->didDistributeNode(node, insertionPoint);
        m_distributed[i] = true;
    }

    // Distributes fallback elements
    if (insertionPoint->isContentInsertionPoint() && distributedNodes.isEmpty()) {
        for (Node* fallbackNode = insertionPoint->firstChild(); fallbackNode; fallbackNode = fallbackNode->nextSibling()) {
            distributedNodes.append(fallbackNode);
            elementShadow->didDistributeNode(fallbackNode, insertionPoint);
        }
    }
    insertionPoint->setDistributedNodes(distributedNodes);
}

inline DistributionPool::~DistributionPool()
{
    detachNonDistributedNodes();
}

inline void DistributionPool::detachNonDistributedNodes()
{
    for (size_t i = 0; i < m_nodes.size(); ++i) {
        if (m_distributed[i])
            continue;
        if (m_nodes[i]->layoutObject())
            m_nodes[i]->lazyReattachIfAttached();
    }
}

PassOwnPtrWillBeRawPtr<ElementShadow> ElementShadow::create()
{
    return adoptPtrWillBeNoop(new ElementShadow());
}

ElementShadow::ElementShadow()
    : m_needsDistributionRecalc(false)
    , m_needsSelectFeatureSet(false)
{
}

ElementShadow::~ElementShadow()
{
#if !ENABLE(OILPAN)
    removeDetachedShadowRoots();
#endif
}

ShadowRoot& ElementShadow::addShadowRoot(Element& shadowHost, ShadowRootType type)
{
    EventDispatchForbiddenScope assertNoEventDispatch;
    ScriptForbiddenScope forbidScript;

    if (type == ShadowRootType::Open) {
        if (!youngestShadowRoot()) {
            shadowHost.willAddFirstAuthorShadowRoot();
        } else if (youngestShadowRoot()->type() == ShadowRootType::UserAgent) {
            shadowHost.willAddFirstAuthorShadowRoot();
            UseCounter::countDeprecation(shadowHost.document(), UseCounter::ElementCreateShadowRootMultipleWithUserAgentShadowRoot);
        } else {
            UseCounter::countDeprecation(shadowHost.document(), UseCounter::ElementCreateShadowRootMultiple);
        }
    }

    for (ShadowRoot* root = youngestShadowRoot(); root; root = root->olderShadowRoot())
        root->lazyReattachIfAttached();

    RefPtrWillBeRawPtr<ShadowRoot> shadowRoot = ShadowRoot::create(shadowHost.document(), type);
    shadowRoot->setParentOrShadowHostNode(&shadowHost);
    shadowRoot->setParentTreeScope(shadowHost.treeScope());
    m_shadowRoots.push(shadowRoot.get());
    setNeedsDistributionRecalc();

    shadowRoot->insertedInto(&shadowHost);
    shadowHost.setChildNeedsStyleRecalc();
    shadowHost.setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::Shadow));

    InspectorInstrumentation::didPushShadowRoot(&shadowHost, shadowRoot.get());

    return *shadowRoot;
}

#if !ENABLE(OILPAN)
void ElementShadow::removeDetachedShadowRoots()
{
    // Dont protect this ref count.
    Element* shadowHost = host();
    ASSERT(shadowHost);

    while (RefPtrWillBeRawPtr<ShadowRoot> oldRoot = m_shadowRoots.head()) {
        InspectorInstrumentation::willPopShadowRoot(shadowHost, oldRoot.get());
        shadowHost->document().removeFocusedElementOfSubtree(oldRoot.get());
        m_shadowRoots.removeHead();
        oldRoot->setParentOrShadowHostNode(0);
        oldRoot->setParentTreeScope(shadowHost->document());
        oldRoot->setPrev(0);
        oldRoot->setNext(0);
    }
}
#endif

void ElementShadow::attach(const Node::AttachContext& context)
{
    Node::AttachContext childrenContext(context);
    childrenContext.resolvedStyle = 0;

    for (ShadowRoot* root = youngestShadowRoot(); root; root = root->olderShadowRoot()) {
        if (root->needsAttach())
            root->attach(childrenContext);
    }
}

void ElementShadow::detach(const Node::AttachContext& context)
{
    Node::AttachContext childrenContext(context);
    childrenContext.resolvedStyle = 0;

    for (ShadowRoot* root = youngestShadowRoot(); root; root = root->olderShadowRoot())
        root->detach(childrenContext);
}

void ElementShadow::setNeedsDistributionRecalc()
{
    if (m_needsDistributionRecalc)
        return;
    m_needsDistributionRecalc = true;
    host()->markAncestorsWithChildNeedsDistributionRecalc();
    clearDistribution();
}

bool ElementShadow::hasSameStyles(const ElementShadow* other) const
{
    ShadowRoot* root = youngestShadowRoot();
    ShadowRoot* otherRoot = other->youngestShadowRoot();
    while (root || otherRoot) {
        if (!root || !otherRoot)
            return false;

        StyleSheetList* list = root->styleSheets();
        StyleSheetList* otherList = otherRoot->styleSheets();

        if (list->length() != otherList->length())
            return false;

        for (size_t i = 0; i < list->length(); i++) {
            if (toCSSStyleSheet(list->item(i))->contents() != toCSSStyleSheet(otherList->item(i))->contents())
                return false;
        }
        root = root->olderShadowRoot();
        otherRoot = otherRoot->olderShadowRoot();
    }

    return true;
}

const InsertionPoint* ElementShadow::finalDestinationInsertionPointFor(const Node* key) const
{
    ASSERT(key && !key->needsDistributionRecalc());
    NodeToDestinationInsertionPoints::const_iterator it = m_nodeToInsertionPoints.find(key);
#if ENABLE(OILPAN)
    return it == m_nodeToInsertionPoints.end() ? nullptr : it->value->last().get();
#else
    return it == m_nodeToInsertionPoints.end() ? nullptr : it->value.last().get();
#endif
}

const DestinationInsertionPoints* ElementShadow::destinationInsertionPointsFor(const Node* key) const
{
    ASSERT(key && !key->needsDistributionRecalc());
    NodeToDestinationInsertionPoints::const_iterator it = m_nodeToInsertionPoints.find(key);
#if ENABLE(OILPAN)
    return it == m_nodeToInsertionPoints.end() ? nullptr : it->value.get();
#else
    return it == m_nodeToInsertionPoints.end() ? nullptr : &it->value;
#endif
}

void ElementShadow::distribute()
{
    host()->setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::Shadow));
    WillBeHeapVector<RawPtrWillBeMember<HTMLShadowElement>, 32> shadowInsertionPoints;
    DistributionPool pool(*host());

    for (ShadowRoot* root = youngestShadowRoot(); root; root = root->olderShadowRoot()) {
        HTMLShadowElement* shadowInsertionPoint = 0;
        const WillBeHeapVector<RefPtrWillBeMember<InsertionPoint>>& insertionPoints = root->descendantInsertionPoints();
        for (size_t i = 0; i < insertionPoints.size(); ++i) {
            InsertionPoint* point = insertionPoints[i].get();
            if (!point->isActive())
                continue;
            if (isHTMLShadowElement(*point)) {
                ASSERT(!shadowInsertionPoint);
                shadowInsertionPoint = toHTMLShadowElement(point);
                shadowInsertionPoints.append(shadowInsertionPoint);
            } else {
                pool.distributeTo(point, this);
                if (ElementShadow* shadow = shadowWhereNodeCanBeDistributed(*point))
                    shadow->setNeedsDistributionRecalc();
            }
        }
    }

    for (size_t i = shadowInsertionPoints.size(); i > 0; --i) {
        HTMLShadowElement* shadowInsertionPoint = shadowInsertionPoints[i - 1];
        ShadowRoot* root = shadowInsertionPoint->containingShadowRoot();
        ASSERT(root);
        if (root->isOldest()) {
            pool.distributeTo(shadowInsertionPoint, this);
        } else if (root->olderShadowRoot()->type() == root->type()) {
            // Only allow reprojecting older shadow roots between the same type to
            // disallow reprojecting UA elements into author shadows.
            DistributionPool olderShadowRootPool(*root->olderShadowRoot());
            olderShadowRootPool.distributeTo(shadowInsertionPoint, this);
            root->olderShadowRoot()->setShadowInsertionPointOfYoungerShadowRoot(shadowInsertionPoint);
        }
        if (ElementShadow* shadow = shadowWhereNodeCanBeDistributed(*shadowInsertionPoint))
            shadow->setNeedsDistributionRecalc();
    }
    InspectorInstrumentation::didPerformElementShadowDistribution(host());
}

void ElementShadow::didDistributeNode(const Node* node, InsertionPoint* insertionPoint)
{
#if ENABLE(OILPAN)
    NodeToDestinationInsertionPoints::AddResult result = m_nodeToInsertionPoints.add(node, nullptr);
    if (result.isNewEntry)
        result.storedValue->value = adoptPtrWillBeNoop(new DestinationInsertionPoints());
    result.storedValue->value->append(insertionPoint);
#else
    NodeToDestinationInsertionPoints::AddResult result = m_nodeToInsertionPoints.add(node, DestinationInsertionPoints());
    result.storedValue->value.append(insertionPoint);
#endif
}

const SelectRuleFeatureSet& ElementShadow::ensureSelectFeatureSet()
{
    if (!m_needsSelectFeatureSet)
        return m_selectFeatures;

    m_selectFeatures.clear();
    for (ShadowRoot* root = oldestShadowRoot(); root; root = root->youngerShadowRoot())
        collectSelectFeatureSetFrom(*root);
    m_needsSelectFeatureSet = false;
    return m_selectFeatures;
}

void ElementShadow::collectSelectFeatureSetFrom(ShadowRoot& root)
{
    if (!root.containsShadowRoots() && !root.containsContentElements())
        return;

    for (Element& element : ElementTraversal::descendantsOf(root)) {
        if (ElementShadow* shadow = element.shadow())
            m_selectFeatures.add(shadow->ensureSelectFeatureSet());
        if (!isHTMLContentElement(element))
            continue;
        const CSSSelectorList& list = toHTMLContentElement(element).selectorList();
        for (const CSSSelector* selector = list.first(); selector; selector = CSSSelectorList::next(*selector)) {
            for (const CSSSelector* component = selector; component; component = component->tagHistory())
                m_selectFeatures.collectFeaturesFromSelector(*component);
        }
    }
}

void ElementShadow::willAffectSelector()
{
    for (ElementShadow* shadow = this; shadow; shadow = shadow->containingShadow()) {
        if (shadow->needsSelectFeatureSet())
            break;
        shadow->setNeedsSelectFeatureSet();
    }
    setNeedsDistributionRecalc();
}

void ElementShadow::clearDistribution()
{
    m_nodeToInsertionPoints.clear();

    for (ShadowRoot* root = youngestShadowRoot(); root; root = root->olderShadowRoot())
        root->setShadowInsertionPointOfYoungerShadowRoot(nullptr);
}

DEFINE_TRACE(ElementShadow)
{
#if ENABLE(OILPAN)
    visitor->trace(m_nodeToInsertionPoints);
    visitor->trace(m_selectFeatures);
    // Shadow roots are linked with previous and next pointers which are traced.
    // It is therefore enough to trace one of the shadow roots here and the
    // rest will be traced from there.
    visitor->trace(m_shadowRoots.head());
#endif
}

} // namespace
