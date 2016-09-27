/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2010 Apple Inc. All rights reserved.
 *           (C) 2006 Alexey Proskuryakov (ap@nypop.com)
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
#include "core/html/HTMLFieldSetElement.h"

#include "core/HTMLNames.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/NodeListsNodeData.h"
#include "core/html/HTMLCollection.h"
#include "core/html/HTMLFormControlsCollection.h"
#include "core/html/HTMLLegendElement.h"
#include "core/html/HTMLObjectElement.h"
#include "core/layout/LayoutFieldset.h"
#include "wtf/StdLibExtras.h"

namespace blink {

using namespace HTMLNames;

inline HTMLFieldSetElement::HTMLFieldSetElement(Document& document, HTMLFormElement* form)
    : HTMLFormControlElement(fieldsetTag, document, form)
    , m_documentVersion(0)
{
}

PassRefPtrWillBeRawPtr<HTMLFieldSetElement> HTMLFieldSetElement::create(Document& document, HTMLFormElement* form)
{
    return adoptRefWillBeNoop(new HTMLFieldSetElement(document, form));
}

DEFINE_TRACE(HTMLFieldSetElement)
{
#if ENABLE(OILPAN)
    visitor->trace(m_associatedElements);
#endif
    HTMLFormControlElement::trace(visitor);
}

bool HTMLFieldSetElement::matchesValidityPseudoClasses() const
{
    return true;
}

bool HTMLFieldSetElement::isValidElement()
{
    const FormAssociatedElement::List& elements = associatedElements();
    for (unsigned i = 0; i < elements.size(); ++i) {
        if (elements[i]->isFormControlElement()) {
            HTMLFormControlElement* control = toHTMLFormControlElement(elements[i].get());
            if (!control->checkValidity(0, CheckValidityDispatchNoEvent))
                return false;
        }
    }
    return true;
}

bool HTMLFieldSetElement::isSubmittableElement()
{
    return false;
}

void HTMLFieldSetElement::invalidateDisabledStateUnder(Element& base)
{
    for (HTMLFormControlElement& element : Traversal<HTMLFormControlElement>::descendantsOf(base))
        element.ancestorDisabledStateWasChanged();
}

void HTMLFieldSetElement::disabledAttributeChanged()
{
    // This element must be updated before the style of nodes in its subtree gets recalculated.
    HTMLFormControlElement::disabledAttributeChanged();
    invalidateDisabledStateUnder(*this);
}

void HTMLFieldSetElement::childrenChanged(const ChildrenChange& change)
{
    HTMLFormControlElement::childrenChanged(change);
    for (HTMLLegendElement& legend : Traversal<HTMLLegendElement>::childrenOf(*this))
        invalidateDisabledStateUnder(legend);
}

bool HTMLFieldSetElement::supportsFocus() const
{
    return HTMLElement::supportsFocus();
}

const AtomicString& HTMLFieldSetElement::formControlType() const
{
    DEFINE_STATIC_LOCAL(const AtomicString, fieldset, ("fieldset", AtomicString::ConstructFromLiteral));
    return fieldset;
}

LayoutObject* HTMLFieldSetElement::createLayoutObject(const ComputedStyle&)
{
    return new LayoutFieldset(this);
}

HTMLLegendElement* HTMLFieldSetElement::legend() const
{
    return Traversal<HTMLLegendElement>::firstChild(*this);
}

PassRefPtrWillBeRawPtr<HTMLFormControlsCollection> HTMLFieldSetElement::elements()
{
    return ensureCachedCollection<HTMLFormControlsCollection>(FormControls);
}

void HTMLFieldSetElement::refreshElementsIfNeeded() const
{
    uint64_t docVersion = document().domTreeVersion();
    if (m_documentVersion == docVersion)
        return;

    m_documentVersion = docVersion;

    m_associatedElements.clear();

    for (HTMLElement& element : Traversal<HTMLElement>::descendantsOf(*this)) {
        if (isHTMLObjectElement(element)) {
            m_associatedElements.append(toHTMLObjectElement(&element));
            continue;
        }

        if (!element.isFormControlElement())
            continue;

        m_associatedElements.append(toHTMLFormControlElement(&element));
    }
}

const FormAssociatedElement::List& HTMLFieldSetElement::associatedElements() const
{
    refreshElementsIfNeeded();
    return m_associatedElements;
}

} // namespace
