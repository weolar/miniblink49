/*
 * Copyright (C) 1997 Martin Jones (mjones@kde.org)
 *           (C) 1997 Torben Weis (weis@kde.org)
 *           (C) 1998 Waldo Bastian (bastian@kde.org)
 *           (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2010 Apple Inc. All rights reserved.
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
#include "core/html/HTMLTableColElement.h"

#include "core/CSSPropertyNames.h"
#include "core/HTMLNames.h"
#include "core/html/HTMLTableElement.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/layout/LayoutTableCol.h"

namespace blink {

using namespace HTMLNames;

inline HTMLTableColElement::HTMLTableColElement(const QualifiedName& tagName, Document& document)
    : HTMLTablePartElement(tagName, document)
    , m_span(1)
{
}

DEFINE_ELEMENT_FACTORY_WITH_TAGNAME(HTMLTableColElement)

bool HTMLTableColElement::isPresentationAttribute(const QualifiedName& name) const
{
    if (name == widthAttr)
        return true;
    return HTMLTablePartElement::isPresentationAttribute(name);
}

void HTMLTableColElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    if (name == widthAttr)
        addHTMLLengthToStyle(style, CSSPropertyWidth, value);
    else
        HTMLTablePartElement::collectStyleForPresentationAttribute(name, value, style);
}

void HTMLTableColElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == spanAttr) {
        unsigned newSpan = 0;
        if (value.isEmpty() || !parseHTMLNonNegativeInteger(value, newSpan) || newSpan < 1) {
            // If the value of span is not a valid non-negative integer greater than zero,
            // set it to 1.
            newSpan = 1;
        }
        m_span = newSpan;
        if (layoutObject() && layoutObject()->isLayoutTableCol())
            layoutObject()->updateFromElement();
    } else if (name == widthAttr) {
        if (!value.isEmpty()) {
            if (layoutObject() && layoutObject()->isLayoutTableCol()) {
                LayoutTableCol* col = toLayoutTableCol(layoutObject());
                int newWidth = width().toInt();
                if (newWidth != col->size().width())
                    col->setNeedsLayoutAndPrefWidthsRecalcAndFullPaintInvalidation(LayoutInvalidationReason::AttributeChanged);
            }
        }
    } else {
        HTMLTablePartElement::parseAttribute(name, value);
    }
}

const StylePropertySet* HTMLTableColElement::additionalPresentationAttributeStyle()
{
    if (!hasTagName(colgroupTag))
        return nullptr;
    if (HTMLTableElement* table = findParentTable())
        return table->additionalGroupStyle(false);
    return nullptr;
}

void HTMLTableColElement::setSpan(unsigned n)
{
    setUnsignedIntegralAttribute(spanAttr, n);
}

const AtomicString& HTMLTableColElement::width() const
{
    return getAttribute(widthAttr);
}

}
