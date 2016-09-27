/**
 * Copyright (C) 1997 Martin Jones (mjones@kde.org)
 *           (C) 1997 Torben Weis (weis@kde.org)
 *           (C) 1998 Waldo Bastian (bastian@kde.org)
 *           (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006 Apple Computer, Inc.
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
#include "core/html/HTMLTablePartElement.h"

#include "core/CSSPropertyNames.h"
#include "core/CSSValueKeywords.h"
#include "core/HTMLNames.h"
#include "core/css/CSSImageValue.h"
#include "core/css/StylePropertySet.h"
#include "core/dom/Document.h"
#include "core/dom/shadow/ComposedTreeTraversal.h"
#include "core/html/HTMLTableElement.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "platform/weborigin/Referrer.h"

namespace blink {

using namespace HTMLNames;

bool HTMLTablePartElement::isPresentationAttribute(const QualifiedName& name) const
{
    if (name == bgcolorAttr || name == backgroundAttr || name == valignAttr || name == alignAttr || name == heightAttr)
        return true;
    return HTMLElement::isPresentationAttribute(name);
}

void HTMLTablePartElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    if (name == bgcolorAttr) {
        addHTMLColorToStyle(style, CSSPropertyBackgroundColor, value);
    } else if (name == backgroundAttr) {
        String url = stripLeadingAndTrailingHTMLSpaces(value);
        if (!url.isEmpty()) {
            RefPtrWillBeRawPtr<CSSImageValue> imageValue = CSSImageValue::create(url, document().completeURL(url));
            imageValue->setReferrer(Referrer(document().outgoingReferrer(), document().referrerPolicy()));
            style->setProperty(CSSProperty(CSSPropertyBackgroundImage, imageValue.release()));
        }
    } else if (name == valignAttr) {
        if (equalIgnoringCase(value, "top"))
            addPropertyToPresentationAttributeStyle(style, CSSPropertyVerticalAlign, CSSValueTop);
        else if (equalIgnoringCase(value, "middle"))
            addPropertyToPresentationAttributeStyle(style, CSSPropertyVerticalAlign, CSSValueMiddle);
        else if (equalIgnoringCase(value, "bottom"))
            addPropertyToPresentationAttributeStyle(style, CSSPropertyVerticalAlign, CSSValueBottom);
        else if (equalIgnoringCase(value, "baseline"))
            addPropertyToPresentationAttributeStyle(style, CSSPropertyVerticalAlign, CSSValueBaseline);
        else
            addPropertyToPresentationAttributeStyle(style, CSSPropertyVerticalAlign, value);
    } else if (name == alignAttr) {
        if (equalIgnoringCase(value, "middle") || equalIgnoringCase(value, "center"))
            addPropertyToPresentationAttributeStyle(style, CSSPropertyTextAlign, CSSValueWebkitCenter);
        else if (equalIgnoringCase(value, "absmiddle"))
            addPropertyToPresentationAttributeStyle(style, CSSPropertyTextAlign, CSSValueCenter);
        else if (equalIgnoringCase(value, "left"))
            addPropertyToPresentationAttributeStyle(style, CSSPropertyTextAlign, CSSValueWebkitLeft);
        else if (equalIgnoringCase(value, "right"))
            addPropertyToPresentationAttributeStyle(style, CSSPropertyTextAlign, CSSValueWebkitRight);
        else
            addPropertyToPresentationAttributeStyle(style, CSSPropertyTextAlign, value);
    } else if (name == heightAttr) {
        if (!value.isEmpty())
            addHTMLLengthToStyle(style, CSSPropertyHeight, value);
    } else {
        HTMLElement::collectStyleForPresentationAttribute(name, value, style);
    }
}

HTMLTableElement* HTMLTablePartElement::findParentTable() const
{
    ContainerNode* parent = ComposedTreeTraversal::parent(*this);
    while (parent && !isHTMLTableElement(*parent))
        parent = ComposedTreeTraversal::parent(*parent);
    return toHTMLTableElement(parent);
}

}
