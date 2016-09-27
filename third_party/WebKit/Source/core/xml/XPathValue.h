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

#ifndef XPathValue_h
#define XPathValue_h

#include "core/xml/XPathNodeSet.h"
#include "wtf/text/WTFString.h"

namespace blink {

namespace XPath {

struct EvaluationContext;

class ValueData : public GarbageCollectedFinalized<ValueData> {
public:
    static ValueData* create() { return new ValueData; }
    static ValueData* create(const NodeSet& nodeSet) { return new ValueData(nodeSet); }
    static ValueData* create(NodeSet* nodeSet) { return new ValueData(nodeSet); }
    static ValueData* create(const String& string) { return new ValueData(string); }
    DECLARE_TRACE();
    NodeSet& nodeSet() { return *m_nodeSet; }

    String m_string;

private:
    ValueData() : m_nodeSet(NodeSet::create()) { }
    explicit ValueData(const NodeSet& nodeSet) : m_nodeSet(NodeSet::create(nodeSet)) { }
    explicit ValueData(NodeSet* nodeSet) : m_nodeSet(nodeSet) { }
    explicit ValueData(const String& string) : m_string(string), m_nodeSet(NodeSet::create()) { }

    Member<NodeSet> m_nodeSet;
};

// Copying Value objects makes their data partially shared, so care has to be taken when dealing with copies.
class Value {
    DISALLOW_ALLOCATION();
public:
    enum Type { NodeSetValue, BooleanValue, NumberValue, StringValue };

    Value(unsigned value) : m_type(NumberValue), m_bool(false), m_number(value) { }
    Value(unsigned long value) : m_type(NumberValue), m_bool(false), m_number(value) { }
    Value(double value) : m_type(NumberValue), m_bool(false), m_number(value) { }

    Value(const char* value) : m_type(StringValue), m_bool(false), m_number(0), m_data(ValueData::create(value)) { }
    Value(const String& value) : m_type(StringValue), m_bool(false), m_number(0), m_data(ValueData::create(value)) { }
    Value(const NodeSet& value) : m_type(NodeSetValue), m_bool(false), m_number(0), m_data(ValueData::create(value)) { }
    Value(Node* value) : m_type(NodeSetValue), m_bool(false), m_number(0), m_data(ValueData::create()) { m_data->nodeSet().append(value); }
    DECLARE_TRACE();

    // This is needed to safely implement constructing from bool - with normal
    // function overloading, any pointer type would match.
    template<typename T> Value(T);

    static const struct AdoptTag { } adopt;
    Value(NodeSet* value, const AdoptTag&) : m_type(NodeSetValue), m_bool(false), m_number(0),  m_data(ValueData::create(value)) { }

    Type type() const { return m_type; }

    bool isNodeSet() const { return m_type == NodeSetValue; }
    bool isBoolean() const { return m_type == BooleanValue; }
    bool isNumber() const { return m_type == NumberValue; }
    bool isString() const { return m_type == StringValue; }

    // If this is called during XPathExpression::evaluate(), EvaluationContext
    // should be passed.
    const NodeSet& toNodeSet(EvaluationContext*) const;
    NodeSet& modifiableNodeSet(EvaluationContext&);
    bool toBoolean() const;
    double toNumber() const;
    String toString() const;

private:
    Type m_type;
    bool m_bool;
    double m_number;
    Member<ValueData> m_data;
};

template<>
inline Value::Value(bool value)
    : m_type(BooleanValue)
    , m_bool(value)
    , m_number(0)
{
}

} // namespace XPath

} // namespace blink

#endif // XPathValue_h
