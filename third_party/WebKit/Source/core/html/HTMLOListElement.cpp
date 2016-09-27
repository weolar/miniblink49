/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2010 Apple Inc. All rights reserved.
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
 *
 */

#include "config.h"
#include "core/html/HTMLOListElement.h"

#include "core/CSSPropertyNames.h"
#include "core/CSSValueKeywords.h"
#include "core/HTMLNames.h"
#include "core/layout/LayoutListItem.h"

namespace blink {

using namespace HTMLNames;

inline HTMLOListElement::HTMLOListElement(Document& document)
    : HTMLElement(olTag, document)
    , m_start(0xBADBEEF)
    , m_itemCount(0)
    , m_hasExplicitStart(false)
    , m_isReversed(false)
    , m_shouldRecalculateItemCount(false)
{
}

DEFINE_NODE_FACTORY(HTMLOListElement)

bool HTMLOListElement::isPresentationAttribute(const QualifiedName& name) const
{
    if (name == typeAttr)
        return true;
    return HTMLElement::isPresentationAttribute(name);
}

void HTMLOListElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    if (name == typeAttr) {
        if (value == "a")
            addPropertyToPresentationAttributeStyle(style, CSSPropertyListStyleType, CSSValueLowerAlpha);
        else if (value == "A")
            addPropertyToPresentationAttributeStyle(style, CSSPropertyListStyleType, CSSValueUpperAlpha);
        else if (value == "i")
            addPropertyToPresentationAttributeStyle(style, CSSPropertyListStyleType, CSSValueLowerRoman);
        else if (value == "I")
            addPropertyToPresentationAttributeStyle(style, CSSPropertyListStyleType, CSSValueUpperRoman);
        else if (value == "1")
            addPropertyToPresentationAttributeStyle(style, CSSPropertyListStyleType, CSSValueDecimal);
    } else {
        HTMLElement::collectStyleForPresentationAttribute(name, value, style);
    }
}

void HTMLOListElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == startAttr) {
        int oldStart = start();
        bool canParse;
        int parsedStart = value.toInt(&canParse);
        m_hasExplicitStart = canParse;
        m_start = canParse ? parsedStart : 0xBADBEEF;
        if (oldStart == start())
            return;
        updateItemValues();
    } else if (name == reversedAttr) {
        bool reversed = !value.isNull();
        if (reversed == m_isReversed)
            return;
        m_isReversed = reversed;
        updateItemValues();
    } else {
        HTMLElement::parseAttribute(name, value);
    }
}

void HTMLOListElement::setStart(int start)
{
    setIntegralAttribute(startAttr, start);
}

void HTMLOListElement::updateItemValues()
{
    if (!layoutObject())
        return;
    updateDistribution();
    LayoutListItem::updateItemValuesForOrderedList(this);
}

void HTMLOListElement::recalculateItemCount()
{
    m_itemCount = LayoutListItem::itemCountForOrderedList(this);
    m_shouldRecalculateItemCount = false;
}

}
