// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "UnionTypesTest.h"

#include "wtf/text/StringBuilder.h"

namespace blink {

void UnionTypesTest::doubleOrStringOrStringArrayAttribute(DoubleOrStringOrStringArray& doubleOrStringOrStringArray)
{
    switch (m_attributeType) {
    case SpecificTypeNone:
        // Default value is zero (of double).
        doubleOrStringOrStringArray.setDouble(0);
        break;
    case SpecificTypeDouble:
        doubleOrStringOrStringArray.setDouble(m_attributeDouble);
        break;
    case SpecificTypeString:
        doubleOrStringOrStringArray.setString(m_attributeString);
        break;
    case SpecificTypeStringArray:
        doubleOrStringOrStringArray.setStringArray(m_attributeStringArray);
        break;
    default:
        ASSERT_NOT_REACHED();
    }
}

void UnionTypesTest::setDoubleOrStringOrStringArrayAttribute(const DoubleOrStringOrStringArray& doubleOrStringOrStringArray)
{
    if (doubleOrStringOrStringArray.isDouble()) {
        m_attributeDouble = doubleOrStringOrStringArray.getAsDouble();
        m_attributeType = SpecificTypeDouble;
    } else if (doubleOrStringOrStringArray.isString()) {
        m_attributeString = doubleOrStringOrStringArray.getAsString();
        m_attributeType = SpecificTypeString;
    } else if (doubleOrStringOrStringArray.isStringArray()) {
        m_attributeStringArray = doubleOrStringOrStringArray.getAsStringArray();
        m_attributeType = SpecificTypeStringArray;
    } else {
        ASSERT_NOT_REACHED();
    }
}

String UnionTypesTest::doubleOrStringArg(DoubleOrString& doubleOrString)
{
    if (doubleOrString.isNull())
        return "null is passed";
    if (doubleOrString.isDouble())
        return "double is passed: " + String::numberToStringECMAScript(doubleOrString.getAsDouble());
    if (doubleOrString.isString())
        return "string is passed: " + doubleOrString.getAsString();
    ASSERT_NOT_REACHED();
    return String();
}

String UnionTypesTest::doubleOrInternalEnumArg(DoubleOrInternalEnum& doubleOrInternalEnum)
{
    if (doubleOrInternalEnum.isDouble())
        return "double is passed: " + String::numberToStringECMAScript(doubleOrInternalEnum.getAsDouble());
    if (doubleOrInternalEnum.isInternalEnum())
        return "InternalEnum is passed: " + doubleOrInternalEnum.getAsInternalEnum();
    ASSERT_NOT_REACHED();
    return String();
}

String UnionTypesTest::doubleOrStringArrayArg(HeapVector<DoubleOrString>& array)
{
    if (!array.size())
        return "";

    StringBuilder builder;
    for (DoubleOrString& doubleOrString : array) {
        ASSERT(!doubleOrString.isNull());
        if (doubleOrString.isDouble())
            builder.append("double: " + String::numberToStringECMAScript(doubleOrString.getAsDouble()));
        else if (doubleOrString.isString())
            builder.append("string: " + doubleOrString.getAsString());
        else
            ASSERT_NOT_REACHED();
        builder.append(", ");
    }
    return builder.substring(0, builder.length() - 2);
}

String UnionTypesTest::doubleOrStringSequenceArg(HeapVector<DoubleOrString>& sequence)
{
    return doubleOrStringArrayArg(sequence);
}

String UnionTypesTest::nodeListOrElementArg(NodeListOrElement& nodeListOrElement)
{
    ASSERT(!nodeListOrElement.isNull());
    return nodeListOrElementOrNullArg(nodeListOrElement);
}

String UnionTypesTest::nodeListOrElementOrNullArg(NodeListOrElement& nodeListOrElementOrNull)
{
    if (nodeListOrElementOrNull.isNull())
        return "null or undefined is passed";
    if (nodeListOrElementOrNull.isNodeList())
        return "nodelist is passed";
    if (nodeListOrElementOrNull.isElement())
        return "element is passed";
    ASSERT_NOT_REACHED();
    return String();
}

String UnionTypesTest::doubleOrStringOrStringArrayArg(const DoubleOrStringOrStringArray& doubleOrStringOrStringArray)
{
    if (doubleOrStringOrStringArray.isNull())
        return "null";

    if (doubleOrStringOrStringArray.isDouble())
        return "double: " + String::numberToStringECMAScript(doubleOrStringOrStringArray.getAsDouble());

    if (doubleOrStringOrStringArray.isString())
        return "string: " + doubleOrStringOrStringArray.getAsString();

    ASSERT(doubleOrStringOrStringArray.isStringArray());
    const Vector<String>& array = doubleOrStringOrStringArray.getAsStringArray();
    if (!array.size())
        return "array: []";
    StringBuilder builder;
    builder.append("array: [");
    for (const String& item : array) {
        ASSERT(!item.isNull());
        builder.append(item);
        builder.append(", ");
    }
    return builder.substring(0, builder.length() - 2) + "]";
}

String UnionTypesTest::doubleOrStringOrStringSequenceArg(const DoubleOrStringOrStringSequence& doubleOrStringOrStringSequence)
{
    if (doubleOrStringOrStringSequence.isNull())
        return "null";

    if (doubleOrStringOrStringSequence.isDouble())
        return "double: " + String::numberToStringECMAScript(doubleOrStringOrStringSequence.getAsDouble());

    if (doubleOrStringOrStringSequence.isString())
        return "string: " + doubleOrStringOrStringSequence.getAsString();

    ASSERT(doubleOrStringOrStringSequence.isStringSequence());
    const Vector<String>& sequence = doubleOrStringOrStringSequence.getAsStringSequence();
    if (!sequence.size())
        return "sequence: []";
    StringBuilder builder;
    builder.append("sequence: [");
    for (const String& item : sequence) {
        ASSERT(!item.isNull());
        builder.append(item);
        builder.append(", ");
    }
    return builder.substring(0, builder.length() - 2) + "]";
}

}
