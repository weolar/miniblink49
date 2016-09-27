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
#include "core/xml/XPathValue.h"

#include "core/xml/XPathExpressionNode.h"
#include "core/xml/XPathUtil.h"
#include "wtf/MathExtras.h"
#include "wtf/StdLibExtras.h"
#include <limits>

namespace blink {
namespace XPath {

const Value::AdoptTag Value::adopt = { };

DEFINE_TRACE(ValueData)
{
    visitor->trace(m_nodeSet);
}

DEFINE_TRACE(Value)
{
    visitor->trace(m_data);
}

const NodeSet& Value::toNodeSet(EvaluationContext* context) const
{
    if (!isNodeSet() && context)
        context->hadTypeConversionError = true;

    if (!m_data) {
        DEFINE_STATIC_LOCAL(Persistent<NodeSet>, emptyNodeSet, (NodeSet::create()));
        return *emptyNodeSet;
    }

    return m_data->nodeSet();
}

NodeSet& Value::modifiableNodeSet(EvaluationContext& context)
{
    if (!isNodeSet())
        context.hadTypeConversionError = true;

    if (!m_data)
        m_data = ValueData::create();

    m_type = NodeSetValue;
    return m_data->nodeSet();
}

bool Value::toBoolean() const
{
    switch (m_type) {
    case NodeSetValue:
        return !m_data->nodeSet().isEmpty();
    case BooleanValue:
        return m_bool;
    case NumberValue:
        return m_number && !std::isnan(m_number);
    case StringValue:
        return !m_data->m_string.isEmpty();
    }
    ASSERT_NOT_REACHED();
    return false;
}

double Value::toNumber() const
{
    switch (m_type) {
    case NodeSetValue:
        return Value(toString()).toNumber();
    case NumberValue:
        return m_number;
    case StringValue: {
        const String& str = m_data->m_string.simplifyWhiteSpace();

        // String::toDouble() supports exponential notation, which is not
        // allowed in XPath.
        unsigned len = str.length();
        for (unsigned i = 0; i < len; ++i) {
            UChar c = str[i];
            if (!isASCIIDigit(c) && c != '.'  && c != '-')
                return std::numeric_limits<double>::quiet_NaN();
        }

        bool canConvert;
        double value = str.toDouble(&canConvert);
        if (canConvert)
            return value;
        return std::numeric_limits<double>::quiet_NaN();
    }
    case BooleanValue:
        return m_bool;
    }
    ASSERT_NOT_REACHED();
    return 0.0;
}

String Value::toString() const
{
    switch (m_type) {
    case NodeSetValue:
        if (m_data->nodeSet().isEmpty())
            return "";
        return stringValue(m_data->nodeSet().firstNode());
    case StringValue:
        return m_data->m_string;
    case NumberValue:
        if (std::isnan(m_number))
            return "NaN";
        if (m_number == 0)
            return "0";
        if (std::isinf(m_number))
            return std::signbit(m_number) ? "-Infinity" : "Infinity";
        return String::number(m_number);
    case BooleanValue:
        return m_bool ? "true" : "false";
    }
    ASSERT_NOT_REACHED();
    return String();
}

}
}
