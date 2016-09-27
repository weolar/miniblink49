/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2006, 2007, 2010 Apple Inc. All rights reserved.
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
#include "core/html/HTMLLIElement.h"

#include "core/CSSPropertyNames.h"
#include "core/CSSValueKeywords.h"
#include "core/HTMLNames.h"
#include "core/dom/LayoutTreeBuilderTraversal.h"
#include "core/layout/LayoutListItem.h"

namespace blink {

using namespace HTMLNames;

inline HTMLLIElement::HTMLLIElement(Document& document)
    : HTMLElement(liTag, document)
{
}

DEFINE_NODE_FACTORY(HTMLLIElement)

bool HTMLLIElement::isPresentationAttribute(const QualifiedName& name) const
{
    if (name == typeAttr)
        return true;
    return HTMLElement::isPresentationAttribute(name);
}

void HTMLLIElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
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
        else
            addPropertyToPresentationAttributeStyle(style, CSSPropertyListStyleType, value);
    } else {
        HTMLElement::collectStyleForPresentationAttribute(name, value, style);
    }
}

void HTMLLIElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == valueAttr) {
        if (layoutObject() && layoutObject()->isListItem())
            parseValue(value);
    } else {
        HTMLElement::parseAttribute(name, value);
    }
}

void HTMLLIElement::attach(const AttachContext& context)
{
    HTMLElement::attach(context);

    if (layoutObject() && layoutObject()->isListItem()) {
        LayoutListItem* listItemLayoutObject = toLayoutListItem(layoutObject());

        ASSERT(!document().childNeedsDistributionRecalc());

        // Find the enclosing list node.
        Element* listNode = 0;
        Element* current = this;
        while (!listNode) {
            current = LayoutTreeBuilderTraversal::parentElement(*current);
            if (!current)
                break;
            if (isHTMLUListElement(*current) || isHTMLOListElement(*current))
                listNode = current;
        }

        // If we are not in a list, tell the layoutObject so it can position us inside.
        // We don't want to change our style to say "inside" since that would affect nested nodes.
        if (!listNode)
            listItemLayoutObject->setNotInList(true);

        parseValue(fastGetAttribute(valueAttr));
    }
}

inline void HTMLLIElement::parseValue(const AtomicString& value)
{
    ASSERT(layoutObject() && layoutObject()->isListItem());

    bool valueOK;
    int requestedValue = value.toInt(&valueOK);
    if (valueOK)
        toLayoutListItem(layoutObject())->setExplicitValue(requestedValue);
    else
        toLayoutListItem(layoutObject())->clearExplicitValue();
}

}
