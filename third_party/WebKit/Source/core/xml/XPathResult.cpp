/*
 * Copyright (C) 2005 Frerich Raabe <raabe@kde.org>
 * Copyright (C) 2006, 2009 Apple Inc. All rights reserved.
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
#include "core/xml/XPathResult.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/xml/XPathEvaluator.h"
#include "core/xml/XPathExpressionNode.h"

namespace blink {

using namespace XPath;

DEFINE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(XPathResult);

XPathResult::XPathResult(EvaluationContext& context, const Value& value)
    : m_value(value)
    , m_nodeSetPosition(0)
    , m_domTreeVersion(0)
{
    switch (m_value.type()) {
    case Value::BooleanValue:
        m_resultType = BOOLEAN_TYPE;
        return;
    case Value::NumberValue:
        m_resultType = NUMBER_TYPE;
        return;
    case Value::StringValue:
        m_resultType = STRING_TYPE;
        return;
    case Value::NodeSetValue:
        m_resultType = UNORDERED_NODE_ITERATOR_TYPE;
        m_nodeSetPosition = 0;
        m_nodeSet = NodeSet::create(m_value.toNodeSet(&context));
        m_document = &context.node->document();
        m_domTreeVersion = m_document->domTreeVersion();
        return;
    }
    ASSERT_NOT_REACHED();
}

DEFINE_TRACE(XPathResult)
{
    visitor->trace(m_value);
    visitor->trace(m_nodeSet);
    visitor->trace(m_document);
}

void XPathResult::convertTo(unsigned short type, ExceptionState& exceptionState)
{
    switch (type) {
    case ANY_TYPE:
        break;
    case NUMBER_TYPE:
        m_resultType = type;
        m_value = m_value.toNumber();
        break;
    case STRING_TYPE:
        m_resultType = type;
        m_value = m_value.toString();
        break;
    case BOOLEAN_TYPE:
        m_resultType = type;
        m_value = m_value.toBoolean();
        break;
    case UNORDERED_NODE_ITERATOR_TYPE:
    case UNORDERED_NODE_SNAPSHOT_TYPE:
    case ANY_UNORDERED_NODE_TYPE:
    // This is correct - singleNodeValue() will take care of ordering.
    case FIRST_ORDERED_NODE_TYPE:
        if (!m_value.isNodeSet()) {
            exceptionState.throwTypeError("The result is not a node set, and therefore cannot be converted to the desired type.");
            return;
        }
        m_resultType = type;
        break;
    case ORDERED_NODE_ITERATOR_TYPE:
        if (!m_value.isNodeSet()) {
            exceptionState.throwTypeError("The result is not a node set, and therefore cannot be converted to the desired type.");
            return;
        }
        nodeSet().sort();
        m_resultType = type;
        break;
    case ORDERED_NODE_SNAPSHOT_TYPE:
        if (!m_value.isNodeSet()) {
            exceptionState.throwTypeError("The result is not a node set, and therefore cannot be converted to the desired type.");
            return;
        }
        m_value.toNodeSet(0).sort();
        m_resultType = type;
        break;
    }
}

unsigned short XPathResult::resultType() const
{
    return m_resultType;
}

double XPathResult::numberValue(ExceptionState& exceptionState) const
{
    if (resultType() != NUMBER_TYPE) {
        exceptionState.throwTypeError("The result type is not a number.");
        return 0.0;
    }
    return m_value.toNumber();
}

String XPathResult::stringValue(ExceptionState& exceptionState) const
{
    if (resultType() != STRING_TYPE) {
        exceptionState.throwTypeError("The result type is not a string.");
        return String();
    }
    return m_value.toString();
}

bool XPathResult::booleanValue(ExceptionState& exceptionState) const
{
    if (resultType() != BOOLEAN_TYPE) {
        exceptionState.throwTypeError("The result type is not a boolean.");
        return false;
    }
    return m_value.toBoolean();
}

Node* XPathResult::singleNodeValue(ExceptionState& exceptionState) const
{
    if (resultType() != ANY_UNORDERED_NODE_TYPE && resultType() != FIRST_ORDERED_NODE_TYPE) {
        exceptionState.throwTypeError("The result type is not a single node.");
        return nullptr;
    }

    const NodeSet& nodes = m_value.toNodeSet(0);
    if (resultType() == FIRST_ORDERED_NODE_TYPE)
        return nodes.firstNode();
    return nodes.anyNode();
}

bool XPathResult::invalidIteratorState() const
{
    if (resultType() != UNORDERED_NODE_ITERATOR_TYPE && resultType() != ORDERED_NODE_ITERATOR_TYPE)
        return false;

    ASSERT(m_document);
    return m_document->domTreeVersion() != m_domTreeVersion;
}

unsigned XPathResult::snapshotLength(ExceptionState& exceptionState) const
{
    if (resultType() != UNORDERED_NODE_SNAPSHOT_TYPE && resultType() != ORDERED_NODE_SNAPSHOT_TYPE) {
        exceptionState.throwTypeError("The result type is not a snapshot.");
        return 0;
    }

    return m_value.toNodeSet(0).size();
}

Node* XPathResult::iterateNext(ExceptionState& exceptionState)
{
    if (resultType() != UNORDERED_NODE_ITERATOR_TYPE && resultType() != ORDERED_NODE_ITERATOR_TYPE) {
        exceptionState.throwTypeError("The result type is not an iterator.");
        return nullptr;
    }

    if (invalidIteratorState()) {
        exceptionState.throwDOMException(InvalidStateError, "The document has mutated since the result was returned.");
        return nullptr;
    }

    if (m_nodeSetPosition + 1 > nodeSet().size())
        return nullptr;

    Node* node = nodeSet()[m_nodeSetPosition];

    m_nodeSetPosition++;

    return node;
}

Node* XPathResult::snapshotItem(unsigned index, ExceptionState& exceptionState)
{
    if (resultType() != UNORDERED_NODE_SNAPSHOT_TYPE && resultType() != ORDERED_NODE_SNAPSHOT_TYPE) {
        exceptionState.throwTypeError("The result type is not a snapshot.");
        return nullptr;
    }

    const NodeSet& nodes = m_value.toNodeSet(0);
    if (index >= nodes.size())
        return nullptr;

    return nodes[index];
}

}
