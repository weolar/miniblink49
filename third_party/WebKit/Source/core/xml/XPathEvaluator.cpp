/*
 * Copyright 2005 Frerich Raabe <raabe@kde.org>
 * Copyright (C) 2006 Apple Computer, Inc.
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
#include "core/xml/XPathEvaluator.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/Node.h"
#include "core/xml/NativeXPathNSResolver.h"
#include "core/xml/XPathExpression.h"
#include "core/xml/XPathResult.h"
#include "core/xml/XPathUtil.h"

namespace blink {

using namespace XPath;

XPathExpression* XPathEvaluator::createExpression(const String& expression, XPathNSResolver* resolver, ExceptionState& exceptionState)
{
    return XPathExpression::createExpression(expression, resolver, exceptionState);
}

XPathNSResolver* XPathEvaluator::createNSResolver(Node* nodeResolver)
{
    return NativeXPathNSResolver::create(nodeResolver);
}

XPathResult* XPathEvaluator::evaluate(const String& expression, Node* contextNode, XPathNSResolver* resolver, unsigned short type, const ScriptValue&, ExceptionState& exceptionState)
{
    if (!isValidContextNode(contextNode)) {
        exceptionState.throwDOMException(NotSupportedError, "The node provided is '" + contextNode->nodeName() + "', which is not a valid context node type.");
        return nullptr;
    }

    XPathExpression* expr = createExpression(expression, resolver, exceptionState);
    if (exceptionState.hadException())
        return nullptr;

    return expr->evaluate(contextNode, type, ScriptValue(), exceptionState);
}

} // namespace blink
