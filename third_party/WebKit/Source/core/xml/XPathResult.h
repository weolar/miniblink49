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

#ifndef XPathResult_h
#define XPathResult_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/xml/XPathValue.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"

namespace blink {

class Document;
class ExceptionState;
class Node;

namespace XPath {
struct EvaluationContext;
}

// TODO(Oilpan): remove Finalized when transition type for m_document is.
class XPathResult final : public GarbageCollectedFinalized<XPathResult>, public ScriptWrappable {
    DECLARE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(XPathResult);
    DEFINE_WRAPPERTYPEINFO();
public:
    enum XPathResultType {
        ANY_TYPE = 0,
        NUMBER_TYPE = 1,
        STRING_TYPE = 2,
        BOOLEAN_TYPE = 3,
        UNORDERED_NODE_ITERATOR_TYPE = 4,
        ORDERED_NODE_ITERATOR_TYPE = 5,
        UNORDERED_NODE_SNAPSHOT_TYPE = 6,
        ORDERED_NODE_SNAPSHOT_TYPE = 7,
        ANY_UNORDERED_NODE_TYPE = 8,
        FIRST_ORDERED_NODE_TYPE = 9
    };

    static XPathResult* create(XPath::EvaluationContext& context, const XPath::Value& value)
    {
        return new XPathResult(context, value);
    }

    void convertTo(unsigned short type, ExceptionState&);

    unsigned short resultType() const;

    double numberValue(ExceptionState&) const;
    String stringValue(ExceptionState&) const;
    bool booleanValue(ExceptionState&) const;
    Node* singleNodeValue(ExceptionState&) const;

    bool invalidIteratorState() const;
    unsigned snapshotLength(ExceptionState&) const;
    Node* iterateNext(ExceptionState&);
    Node* snapshotItem(unsigned index, ExceptionState&);

    const XPath::Value& value() const { return m_value; }

    DECLARE_TRACE();

private:
    XPathResult(XPath::EvaluationContext&, const XPath::Value&);
    XPath::NodeSet& nodeSet() { return *m_nodeSet; }

    XPath::Value m_value;
    unsigned m_nodeSetPosition;
    Member<XPath::NodeSet> m_nodeSet; // FIXME: why duplicate the node set stored in m_value?
    unsigned short m_resultType;
    RefPtrWillBeMember<Document> m_document;
    uint64_t m_domTreeVersion;
};

} // namespace blink

#endif // XPathResult_h
