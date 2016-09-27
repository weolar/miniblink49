/*
 * Copyright (C) 2004, 2005, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "core/svg/SVGStringList.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/svg/SVGElement.h"
#include "core/svg/SVGParserUtilities.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

SVGStringList::SVGStringList()
{
}

SVGStringList::~SVGStringList()
{
}

void SVGStringList::initialize(const String& item)
{
    m_values.clear();
    m_values.append(item);
}

String SVGStringList::getItem(size_t index, ExceptionState& exceptionState)
{
    if (!checkIndexBound(index, exceptionState))
        return String();

    return m_values.at(index);
}

void SVGStringList::insertItemBefore(const String& newItem, size_t index)
{
    // Spec: If the index is greater than or equal to numberOfItems, then the new item is appended to the end of the list.
    if (index > m_values.size())
        index = m_values.size();

    // Spec: Inserts a new item into the list at the specified position. The index of the item before which the new item is to be
    // inserted. The first item is number 0. If the index is equal to 0, then the new item is inserted at the front of the list.
    m_values.insert(index, newItem);
}

String SVGStringList::removeItem(size_t index, ExceptionState& exceptionState)
{
    if (!checkIndexBound(index, exceptionState))
        return String();

    String oldItem = m_values.at(index);
    m_values.remove(index);
    return oldItem;
}

void SVGStringList::appendItem(const String& newItem)
{
    m_values.append(newItem);
}

void SVGStringList::replaceItem(const String& newItem, size_t index, ExceptionState& exceptionState)
{
    if (!checkIndexBound(index, exceptionState))
        return;

    // Update the value at the desired position 'index'.
    m_values[index] = newItem;
}

template<typename CharType>
void SVGStringList::parseInternal(const CharType*& ptr, const CharType* end)
{
    const UChar delimiter = ' ';

    while (ptr < end) {
        const CharType* start = ptr;
        while (ptr < end && *ptr != delimiter && !isHTMLSpace<CharType>(*ptr))
            ptr++;
        if (ptr == start)
            break;
        m_values.append(String(start, ptr - start));
        skipOptionalSVGSpacesOrDelimiter(ptr, end, delimiter);
    }
}

void SVGStringList::setValueAsString(const String& data, ExceptionState&)
{
    // FIXME: Add more error checking and reporting.
    m_values.clear();
    if (data.isEmpty())
        return;
    if (data.is8Bit()) {
        const LChar* ptr = data.characters8();
        const LChar* end = ptr + data.length();
        parseInternal(ptr, end);
    } else {
        const UChar* ptr = data.characters16();
        const UChar* end = ptr + data.length();
        parseInternal(ptr, end);
    }
}

String SVGStringList::valueAsString() const
{
    StringBuilder builder;

    Vector<String>::const_iterator it = m_values.begin();
    Vector<String>::const_iterator itEnd = m_values.end();
    if (it != itEnd) {
        builder.append(*it);
        ++it;

        for (; it != itEnd; ++it) {
            builder.append(' ');
            builder.append(*it);
        }
    }

    return builder.toString();
}

bool SVGStringList::checkIndexBound(size_t index, ExceptionState& exceptionState)
{
    if (index >= m_values.size()) {
        exceptionState.throwDOMException(IndexSizeError, ExceptionMessages::indexExceedsMaximumBound("index", index, m_values.size()));
        return false;
    }

    return true;
}

void SVGStringList::add(PassRefPtrWillBeRawPtr<SVGPropertyBase> other, SVGElement* contextElement)
{
    // SVGStringList is never animated.
    ASSERT_NOT_REACHED();
}

void SVGStringList::calculateAnimatedValue(SVGAnimationElement*, float, unsigned, PassRefPtrWillBeRawPtr<SVGPropertyBase>, PassRefPtrWillBeRawPtr<SVGPropertyBase>, PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*)
{
    // SVGStringList is never animated.
    ASSERT_NOT_REACHED();
}

float SVGStringList::calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*)
{
    // SVGStringList is never animated.
    ASSERT_NOT_REACHED();

    return -1.0f;
}

}
