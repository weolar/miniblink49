/*
 * Copyright (C) 2010 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/dom/DatasetDOMStringMap.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/Attribute.h"
#include "core/dom/Element.h"
#include "core/dom/ExceptionCode.h"
#include "wtf/ASCIICType.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

static bool isValidAttributeName(const String& name)
{
    if (!name.startsWith("data-"))
        return false;

    unsigned length = name.length();
    for (unsigned i = 5; i < length; ++i) {
        if (isASCIIUpper(name[i]))
            return false;
    }

    return true;
}

static String convertAttributeNameToPropertyName(const String& name)
{
    StringBuilder stringBuilder;

    unsigned length = name.length();
    for (unsigned i = 5; i < length; ++i) {
        UChar character = name[i];
        if (character != '-') {
            stringBuilder.append(character);
        } else {
            if ((i + 1 < length) && isASCIILower(name[i + 1])) {
                stringBuilder.append(toASCIIUpper(name[i + 1]));
                ++i;
            } else {
                stringBuilder.append(character);
            }
        }
    }

    return stringBuilder.toString();
}

template<typename CharType1, typename CharType2>
static bool propertyNameMatchesAttributeName(const CharType1* propertyName, const CharType2* attributeName, unsigned propertyLength, unsigned attributeLength)
{
    unsigned a = 5;
    unsigned p = 0;
    bool wordBoundary = false;
    while (a < attributeLength && p < propertyLength) {
        if (attributeName[a] == '-' && a + 1 < attributeLength && isASCIILower(attributeName[a + 1])) {
            wordBoundary = true;
        } else {
            if ((wordBoundary ? toASCIIUpper(attributeName[a]) : attributeName[a]) != propertyName[p])
                return false;
            p++;
            wordBoundary = false;
        }
        a++;
    }

    return (a == attributeLength && p == propertyLength);
}

static bool propertyNameMatchesAttributeName(const String& propertyName, const String& attributeName)
{
    if (!attributeName.startsWith("data-"))
        return false;

    unsigned propertyLength = propertyName.length();
    unsigned attributeLength = attributeName.length();

    if (propertyName.is8Bit()) {
        if (attributeName.is8Bit())
            return propertyNameMatchesAttributeName(propertyName.characters8(), attributeName.characters8(), propertyLength, attributeLength);
        return propertyNameMatchesAttributeName(propertyName.characters8(), attributeName.characters16(), propertyLength, attributeLength);
    }

    if (attributeName.is8Bit())
        return propertyNameMatchesAttributeName(propertyName.characters16(), attributeName.characters8(), propertyLength, attributeLength);
    return propertyNameMatchesAttributeName(propertyName.characters16(), attributeName.characters16(), propertyLength, attributeLength);
}

static bool isValidPropertyName(const String& name)
{
    unsigned length = name.length();
    for (unsigned i = 0; i < length; ++i) {
        if (name[i] == '-' && (i + 1 < length) && isASCIILower(name[i + 1]))
            return false;
    }
    return true;
}

// This returns an AtomicString because attribute names are always stored
// as AtomicString types in Element (see setAttribute()).
static AtomicString convertPropertyNameToAttributeName(const String& name)
{
    StringBuilder builder;
    builder.appendLiteral("data-");

    unsigned length = name.length();
    for (unsigned i = 0; i < length; ++i) {
        UChar character = name[i];
        if (isASCIIUpper(character)) {
            builder.append('-');
            builder.append(toASCIILower(character));
        } else {
            builder.append(character);
        }
    }

    return builder.toAtomicString();
}

#if !ENABLE(OILPAN)
void DatasetDOMStringMap::ref()
{
    m_element->ref();
}

void DatasetDOMStringMap::deref()
{
    m_element->deref();
}
#endif

void DatasetDOMStringMap::getNames(Vector<String>& names)
{
    AttributeCollection attributes = m_element->attributes();
    for (const Attribute& attr : attributes) {
        if (isValidAttributeName(attr.localName()))
            names.append(convertAttributeNameToPropertyName(attr.localName()));
    }
}

String DatasetDOMStringMap::item(const String& name)
{
    AttributeCollection attributes = m_element->attributes();
    for (const Attribute& attr : attributes) {
        if (propertyNameMatchesAttributeName(name, attr.localName()))
            return attr.value();
    }

    return String();
}

bool DatasetDOMStringMap::contains(const String& name)
{
    AttributeCollection attributes = m_element->attributes();
    for (const Attribute& attr : attributes) {
        if (propertyNameMatchesAttributeName(name, attr.localName()))
            return true;
    }
    return false;
}

void DatasetDOMStringMap::setItem(const String& name, const String& value, ExceptionState& exceptionState)
{
    if (!isValidPropertyName(name)) {
        exceptionState.throwDOMException(SyntaxError, "'" + name + "' is not a valid property name.");
        return;
    }

    m_element->setAttribute(convertPropertyNameToAttributeName(name), AtomicString(value), exceptionState);
}

bool DatasetDOMStringMap::deleteItem(const String& name)
{
    if (isValidPropertyName(name)) {
        AtomicString attributeName = convertPropertyNameToAttributeName(name);
        if (m_element->hasAttribute(attributeName)) {
            m_element->removeAttribute(attributeName);
            return true;
        }
    }
    return false;
}

DEFINE_TRACE(DatasetDOMStringMap)
{
    visitor->trace(m_element);
    DOMStringMap::trace(visitor);
}

} // namespace blink
