/*
 * Copyright (C) 2005 Frerich Raabe <raabe@kde.org>
 * Copyright (C) 2006, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Alexey Proskuryakov <ap@webkit.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/xml/XPathStep.h"

#include "core/XMLNSNames.h"
#include "core/dom/Attr.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/NodeTraversal.h"
#include "core/xml/XPathParser.h"
#include "core/xml/XPathUtil.h"

namespace blink {
namespace XPath {

Step::Step(Axis axis, const NodeTest& nodeTest)
    : m_axis(axis)
    , m_nodeTest(new NodeTest(nodeTest))
{
}

Step::Step(Axis axis, const NodeTest& nodeTest, HeapVector<Member<Predicate>>& predicates)
    : m_axis(axis)
    , m_nodeTest(new NodeTest(nodeTest))
{
    m_predicates.swap(predicates);
}

Step::~Step()
{
}

DEFINE_TRACE(Step)
{
    visitor->trace(m_nodeTest);
    visitor->trace(m_predicates);
    ParseNode::trace(visitor);
}

void Step::optimize()
{
    // Evaluate predicates as part of node test if possible to avoid building
    // unnecessary NodeSets.
    // E.g., there is no need to build a set of all "foo" nodes to evaluate
    // "foo[@bar]", we can check the predicate while enumerating.
    // This optimization can be applied to predicates that are not context node
    // list sensitive, or to first predicate that is only context position
    // sensitive, e.g. foo[position() mod 2 = 0].
    HeapVector<Member<Predicate>> remainingPredicates;
    for (size_t i = 0; i < m_predicates.size(); ++i) {
        Predicate* predicate = m_predicates[i];
        if ((!predicate->isContextPositionSensitive() || nodeTest().mergedPredicates().isEmpty()) && !predicate->isContextSizeSensitive() && remainingPredicates.isEmpty()) {
            nodeTest().mergedPredicates().append(predicate);
        } else {
            remainingPredicates.append(predicate);
        }
    }
    swap(remainingPredicates, m_predicates);
}

bool optimizeStepPair(Step* first, Step* second)
{
    if (first->m_axis == Step::DescendantOrSelfAxis
        && first->nodeTest().kind() == Step::NodeTest::AnyNodeTest
        && !first->m_predicates.size()
        && !first->nodeTest().mergedPredicates().size()) {

        ASSERT(first->nodeTest().data().isEmpty());
        ASSERT(first->nodeTest().namespaceURI().isEmpty());

        // Optimize the common case of "//" AKA
        // /descendant-or-self::node()/child::NodeTest to /descendant::NodeTest.
        if (second->m_axis == Step::ChildAxis && second->predicatesAreContextListInsensitive()) {
            first->m_axis = Step::DescendantAxis;
            first->nodeTest() = Step::NodeTest(second->nodeTest().kind(), second->nodeTest().data(), second->nodeTest().namespaceURI());
            swap(second->nodeTest().mergedPredicates(), first->nodeTest().mergedPredicates());
            swap(second->m_predicates, first->m_predicates);
            first->optimize();
            return true;
        }
    }
    return false;
}

bool Step::predicatesAreContextListInsensitive() const
{
    for (size_t i = 0; i < m_predicates.size(); ++i) {
        Predicate* predicate = m_predicates[i].get();
        if (predicate->isContextPositionSensitive() || predicate->isContextSizeSensitive())
            return false;
    }

    for (size_t i = 0; i < nodeTest().mergedPredicates().size(); ++i) {
        Predicate* predicate = nodeTest().mergedPredicates()[i].get();
        if (predicate->isContextPositionSensitive() || predicate->isContextSizeSensitive())
            return false;
    }

    return true;
}

void Step::evaluate(EvaluationContext& evaluationContext, Node* context, NodeSet& nodes) const
{
    evaluationContext.position = 0;

    nodesInAxis(evaluationContext, context, nodes);

    // Check predicates that couldn't be merged into node test.
    for (unsigned i = 0; i < m_predicates.size(); i++) {
        Predicate* predicate = m_predicates[i].get();

        NodeSet* newNodes = NodeSet::create();
        if (!nodes.isSorted())
            newNodes->markSorted(false);

        for (unsigned j = 0; j < nodes.size(); j++) {
            Node* node = nodes[j];

            evaluationContext.node = node;
            evaluationContext.size = nodes.size();
            evaluationContext.position = j + 1;
            if (predicate->evaluate(evaluationContext))
                newNodes->append(node);
        }

        nodes.swap(*newNodes);
    }
}

#if ENABLE(ASSERT)
static inline Node::NodeType primaryNodeType(Step::Axis axis)
{
    switch (axis) {
    case Step::AttributeAxis:
        return Node::ATTRIBUTE_NODE;
    default:
        return Node::ELEMENT_NODE;
    }
}
#endif

// Evaluate NodeTest without considering merged predicates.
static inline bool nodeMatchesBasicTest(Node* node, Step::Axis axis, const Step::NodeTest& nodeTest)
{
    switch (nodeTest.kind()) {
    case Step::NodeTest::TextNodeTest: {
        Node::NodeType type = node->nodeType();
        return type == Node::TEXT_NODE || type == Node::CDATA_SECTION_NODE;
    }
    case Step::NodeTest::CommentNodeTest:
        return node->nodeType() == Node::COMMENT_NODE;
    case Step::NodeTest::ProcessingInstructionNodeTest: {
        const AtomicString& name = nodeTest.data();
        return node->nodeType() == Node::PROCESSING_INSTRUCTION_NODE && (name.isEmpty() || node->nodeName() == name);
    }
    case Step::NodeTest::AnyNodeTest:
        return true;
    case Step::NodeTest::NameTest: {
        const AtomicString& name = nodeTest.data();
        const AtomicString& namespaceURI = nodeTest.namespaceURI();

        if (axis == Step::AttributeAxis) {
            ASSERT(node->isAttributeNode());

            // In XPath land, namespace nodes are not accessible on the
            // attribute axis.
            if (node->namespaceURI() == XMLNSNames::xmlnsNamespaceURI)
                return false;

            if (name == starAtom)
                return namespaceURI.isEmpty() || node->namespaceURI() == namespaceURI;

            return node->localName() == name && node->namespaceURI() == namespaceURI;
        }

        // Node test on the namespace axis is not implemented yet, the caller
        // has a check for it.
        ASSERT(axis != Step::NamespaceAxis);

        // For other axes, the principal node type is element.
        ASSERT(primaryNodeType(axis) == Node::ELEMENT_NODE);
        if (!node->isElementNode())
            return false;
        Element& element = toElement(*node);

        if (name == starAtom)
            return namespaceURI.isEmpty() || namespaceURI == element.namespaceURI();

        if (element.document().isHTMLDocument()) {
            if (element.isHTMLElement()) {
                // Paths without namespaces should match HTML elements in HTML
                // documents despite those having an XHTML namespace. Names are
                // compared case-insensitively.
                return equalIgnoringCase(element.localName(), name) && (namespaceURI.isNull() || namespaceURI == element.namespaceURI());
            }
            // An expression without any prefix shouldn't match no-namespace
            // nodes (because HTML5 says so).
            return element.hasLocalName(name) && namespaceURI == element.namespaceURI() && !namespaceURI.isNull();
        }
        return element.hasLocalName(name) && namespaceURI == element.namespaceURI();
    }
    }
    ASSERT_NOT_REACHED();
    return false;
}

static inline bool nodeMatches(EvaluationContext& evaluationContext, Node* node, Step::Axis axis, const Step::NodeTest& nodeTest)
{
    if (!nodeMatchesBasicTest(node, axis, nodeTest))
        return false;

    // Only the first merged predicate may depend on position.
    ++evaluationContext.position;

    const HeapVector<Member<Predicate>>& mergedPredicates = nodeTest.mergedPredicates();
    for (unsigned i = 0; i < mergedPredicates.size(); i++) {
        Predicate* predicate = mergedPredicates[i].get();

        evaluationContext.node = node;
        // No need to set context size - we only get here when evaluating
        // predicates that do not depend on it.
        if (!predicate->evaluate(evaluationContext))
            return false;
    }

    return true;
}

// Result nodes are ordered in axis order. Node test (including merged
// predicates) is applied.
void Step::nodesInAxis(EvaluationContext& evaluationContext, Node* context, NodeSet& nodes) const
{
    ASSERT(nodes.isEmpty());
    switch (m_axis) {
    case ChildAxis:
        // In XPath model, attribute nodes do not have children.
        if (context->isAttributeNode())
            return;

        for (Node* n = context->firstChild(); n; n = n->nextSibling()) {
            if (nodeMatches(evaluationContext, n, ChildAxis, nodeTest()))
                nodes.append(n);
        }
        return;

    case DescendantAxis:
        // In XPath model, attribute nodes do not have children.
        if (context->isAttributeNode())
            return;

        for (Node& n : NodeTraversal::descendantsOf(*context)) {
            if (nodeMatches(evaluationContext, &n, DescendantAxis, nodeTest()))
                nodes.append(&n);
        }
        return;

    case ParentAxis:
        if (context->isAttributeNode()) {
            Element* n = toAttr(context)->ownerElement();
            if (nodeMatches(evaluationContext, n, ParentAxis, nodeTest()))
                nodes.append(n);
        } else {
            ContainerNode* n = context->parentNode();
            if (n && nodeMatches(evaluationContext, n, ParentAxis, nodeTest()))
                nodes.append(n);
        }
        return;

    case AncestorAxis: {
        Node* n = context;
        if (context->isAttributeNode()) {
            n = toAttr(context)->ownerElement();
            if (nodeMatches(evaluationContext, n, AncestorAxis, nodeTest()))
                nodes.append(n);
        }
        for (n = n->parentNode(); n; n = n->parentNode()) {
            if (nodeMatches(evaluationContext, n, AncestorAxis, nodeTest()))
                nodes.append(n);
        }
        nodes.markSorted(false);
        return;
    }

    case FollowingSiblingAxis:
        if (context->nodeType() == Node::ATTRIBUTE_NODE)
            return;

        for (Node* n = context->nextSibling(); n; n = n->nextSibling()) {
            if (nodeMatches(evaluationContext, n, FollowingSiblingAxis, nodeTest()))
                nodes.append(n);
        }
        return;

    case PrecedingSiblingAxis:
        if (context->nodeType() == Node::ATTRIBUTE_NODE)
            return;

        for (Node* n = context->previousSibling(); n; n = n->previousSibling()) {
            if (nodeMatches(evaluationContext, n, PrecedingSiblingAxis, nodeTest()))
                nodes.append(n);
        }
        nodes.markSorted(false);
        return;

    case FollowingAxis:
        if (context->isAttributeNode()) {
            for (Node& p : NodeTraversal::startsAfter(*toAttr(context)->ownerElement())) {
                if (nodeMatches(evaluationContext, &p, FollowingAxis, nodeTest()))
                    nodes.append(&p);
            }
        } else {
            for (Node* p = context; !isRootDomNode(p); p = p->parentNode()) {
                for (Node* n = p->nextSibling(); n; n = n->nextSibling()) {
                    if (nodeMatches(evaluationContext, n, FollowingAxis, nodeTest()))
                        nodes.append(n);
                    for (Node& c : NodeTraversal::descendantsOf(*n)) {
                        if (nodeMatches(evaluationContext, &c, FollowingAxis, nodeTest()))
                            nodes.append(&c);
                    }
                }
            }
        }
        return;

    case PrecedingAxis: {
        if (context->isAttributeNode())
            context = toAttr(context)->ownerElement();

        Node* n = context;
        while (ContainerNode* parent = n->parentNode()) {
            for (n = NodeTraversal::previous(*n); n != parent; n = NodeTraversal::previous(*n)) {
                if (nodeMatches(evaluationContext, n, PrecedingAxis, nodeTest()))
                    nodes.append(n);
            }
            n = parent;
        }
        nodes.markSorted(false);
        return;
    }

    case AttributeAxis: {
        if (!context->isElementNode())
            return;

        Element* contextElement = toElement(context);
        // Avoid lazily creating attribute nodes for attributes that we do not
        // need anyway.
        if (nodeTest().kind() == NodeTest::NameTest && nodeTest().data() != starAtom) {
            RefPtrWillBeRawPtr<Node> n = contextElement->getAttributeNodeNS(nodeTest().namespaceURI(), nodeTest().data());
            // In XPath land, namespace nodes are not accessible on the attribute axis.
            if (n && n->namespaceURI() != XMLNSNames::xmlnsNamespaceURI) {
                // Still need to check merged predicates.
                if (nodeMatches(evaluationContext, n.get(), AttributeAxis, nodeTest()))
                    nodes.append(n.release());
            }
            return;
        }

        AttributeCollection attributes = contextElement->attributes();
        for (auto& attribute : attributes) {
            RefPtrWillBeRawPtr<Attr> attr = contextElement->ensureAttr(attribute.name());
            if (nodeMatches(evaluationContext, attr.get(), AttributeAxis, nodeTest()))
                nodes.append(attr.release());
        }
        return;
    }

    case NamespaceAxis:
        // XPath namespace nodes are not implemented.
        return;

    case SelfAxis:
        if (nodeMatches(evaluationContext, context, SelfAxis, nodeTest()))
            nodes.append(context);
        return;

    case DescendantOrSelfAxis:
        if (nodeMatches(evaluationContext, context, DescendantOrSelfAxis, nodeTest()))
            nodes.append(context);
        // In XPath model, attribute nodes do not have children.
        if (context->isAttributeNode())
            return;

        for (Node& n : NodeTraversal::descendantsOf(*context)) {
            if (nodeMatches(evaluationContext, &n, DescendantOrSelfAxis, nodeTest()))
                nodes.append(&n);
        }
        return;

    case AncestorOrSelfAxis: {
        if (nodeMatches(evaluationContext, context, AncestorOrSelfAxis, nodeTest()))
            nodes.append(context);
        Node* n = context;
        if (context->isAttributeNode()) {
            n = toAttr(context)->ownerElement();
            if (nodeMatches(evaluationContext, n, AncestorOrSelfAxis, nodeTest()))
                nodes.append(n);
        }
        for (n = n->parentNode(); n; n = n->parentNode()) {
            if (nodeMatches(evaluationContext, n, AncestorOrSelfAxis, nodeTest()))
                nodes.append(n);
        }
        nodes.markSorted(false);
        return;
    }
    }
    ASSERT_NOT_REACHED();
}

}

}
