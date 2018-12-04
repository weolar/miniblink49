/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007 Apple Inc. All rights reserved.
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
#include "core/html/FormAssociatedElement.h"

#include "core/HTMLNames.h"
#include "core/dom/IdTargetObserver.h"
#include "core/dom/NodeTraversal.h"
#include "core/html/HTMLFormControlElement.h"
#include "core/html/HTMLFormElement.h"
#include "core/html/HTMLLabelElement.h"
#include "core/html/HTMLObjectElement.h"
#include "core/html/ValidityState.h"

namespace blink {

using namespace HTMLNames;

class FormAttributeTargetObserver : public IdTargetObserver {
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(FormAttributeTargetObserver);
public:
    static PassOwnPtrWillBeRawPtr<FormAttributeTargetObserver> create(const AtomicString& id, FormAssociatedElement*);
    DECLARE_VIRTUAL_TRACE();
    void idTargetChanged() override;

private:
    FormAttributeTargetObserver(const AtomicString& id, FormAssociatedElement*);

    RawPtrWillBeMember<FormAssociatedElement> m_element;
};

FormAssociatedElement::FormAssociatedElement()
    : m_formWasSetByParser(false)
    , m_customValidationMessage()
{
}

FormAssociatedElement::~FormAssociatedElement()
{
    // We can't call setForm here because it contains virtual calls.
}

DEFINE_TRACE(FormAssociatedElement)
{
    visitor->trace(m_formAttributeTargetObserver);
    visitor->trace(m_form);
    visitor->trace(m_validityState);
}

ValidityState* FormAssociatedElement::validity()
{
    if (!m_validityState)
        m_validityState = ValidityState::create(this);

    return m_validityState.get();
}

void FormAssociatedElement::didMoveToNewDocument(Document& oldDocument)
{
    HTMLElement* element = toHTMLElement(this);
    if (element->fastHasAttribute(formAttr))
        setFormAttributeTargetObserver(nullptr);
}

void FormAssociatedElement::insertedInto(ContainerNode* insertionPoint)
{
    if (!m_formWasSetByParser || !m_form || NodeTraversal::highestAncestorOrSelf(*insertionPoint) != NodeTraversal::highestAncestorOrSelf(*m_form.get()))
        resetFormOwner();

    if (!insertionPoint->inDocument())
        return;

    HTMLElement* element = toHTMLElement(this);
    if (element->fastHasAttribute(formAttr))
        resetFormAttributeTargetObserver();
}

void FormAssociatedElement::removedFrom(ContainerNode* insertionPoint)
{
    HTMLElement* element = toHTMLElement(this);
    if (insertionPoint->inDocument() && element->fastHasAttribute(formAttr)) {
        setFormAttributeTargetObserver(nullptr);
        resetFormOwner();
        return;
    }
    // If the form and element are both in the same tree, preserve the connection to the form.
    // Otherwise, null out our form and remove ourselves from the form's list of elements.
    if (m_form && NodeTraversal::highestAncestorOrSelf(*element) != NodeTraversal::highestAncestorOrSelf(*m_form.get()))
        resetFormOwner();
}

HTMLFormElement* FormAssociatedElement::findAssociatedForm(const HTMLElement* element)
{
    const AtomicString& formId(element->fastGetAttribute(formAttr));
    // 3. If the element is reassociateable, has a form content attribute, and
    // is itself in a Document, then run these substeps:
    if (!formId.isNull() && element->inDocument()) {
        // 3.1. If the first element in the Document to have an ID that is
        // case-sensitively equal to the element's form content attribute's
        // value is a form element, then associate the form-associated element
        // with that form element.
        // 3.2. Abort the "reset the form owner" steps.
        Element* newFormCandidate = element->treeScope().getElementById(formId);
        return isHTMLFormElement(newFormCandidate) ? toHTMLFormElement(newFormCandidate) : 0;
    }
    // 4. Otherwise, if the form-associated element in question has an ancestor
    // form element, then associate the form-associated element with the nearest
    // such ancestor form element.
    return element->findFormAncestor();
}

void FormAssociatedElement::formRemovedFromTree(const Node& formRoot)
{
    ASSERT(m_form);
    if (NodeTraversal::highestAncestorOrSelf(toHTMLElement(*this)) == formRoot)
        return;
    resetFormOwner();
}

void FormAssociatedElement::associateByParser(HTMLFormElement* form)
{
    if (form && form->inDocument()) {
        m_formWasSetByParser = true;
        setForm(form);
        form->didAssociateByParser();
    }
}

void FormAssociatedElement::setForm(HTMLFormElement* newForm)
{
    if (m_form.get() == newForm)
        return;
    willChangeForm();
    if (m_form)
        m_form->disassociate(*this);
    if (newForm) {
#if ENABLE(OILPAN)
        m_form = newForm;
#else
        m_form = newForm->createWeakPtr();
#endif
        m_form->associate(*this);
    } else {
#if ENABLE(OILPAN)
        m_form = nullptr;
#else
        m_form = WeakPtr<HTMLFormElement>();
#endif
    }
    didChangeForm();
}

void FormAssociatedElement::willChangeForm()
{
}

void FormAssociatedElement::didChangeForm()
{
    if (!m_formWasSetByParser && m_form && m_form->inDocument()) {
        HTMLElement* element = toHTMLElement(this);
        element->document().didAssociateFormControl(element);
    }
}

void FormAssociatedElement::resetFormOwner()
{
    m_formWasSetByParser = false;
    HTMLElement* element = toHTMLElement(this);
    const AtomicString& formId(element->fastGetAttribute(formAttr));
    HTMLFormElement* nearestForm = element->findFormAncestor();
    // 1. If the element's form owner is not null, and either the element is not
    // reassociateable or its form content attribute is not present, and the
    // element's form owner is its nearest form element ancestor after the
    // change to the ancestor chain, then do nothing, and abort these steps.
    if (m_form && formId.isNull() && m_form.get() == nearestForm)
        return;

    setForm(findAssociatedForm(element));
}

void FormAssociatedElement::formAttributeChanged()
{
    resetFormOwner();
    resetFormAttributeTargetObserver();
}

bool FormAssociatedElement::customError() const
{
    const HTMLElement* element = toHTMLElement(this);
    return element->willValidate() && !m_customValidationMessage.isEmpty();
}

bool FormAssociatedElement::hasBadInput() const
{
    return false;
}

bool FormAssociatedElement::patternMismatch() const
{
    return false;
}

bool FormAssociatedElement::rangeOverflow() const
{
    return false;
}

bool FormAssociatedElement::rangeUnderflow() const
{
    return false;
}

bool FormAssociatedElement::stepMismatch() const
{
    return false;
}

bool FormAssociatedElement::tooLong() const
{
    return false;
}

bool FormAssociatedElement::tooShort() const
{
    return false;
}

bool FormAssociatedElement::typeMismatch() const
{
    return false;
}

bool FormAssociatedElement::valid() const
{
    bool someError = typeMismatch() || stepMismatch() || rangeUnderflow() || rangeOverflow()
        || tooLong() || tooShort() || patternMismatch() || valueMissing() || hasBadInput()
        || customError();
    return !someError;
}

bool FormAssociatedElement::valueMissing() const
{
    return false;
}

String FormAssociatedElement::customValidationMessage() const
{
    return m_customValidationMessage;
}

String FormAssociatedElement::validationMessage() const
{
    return customError() ? m_customValidationMessage : String();
}

void FormAssociatedElement::setCustomValidity(const String& error)
{
    m_customValidationMessage = error;
}

void FormAssociatedElement::setFormAttributeTargetObserver(PassOwnPtrWillBeRawPtr<FormAttributeTargetObserver> newObserver)
{
    if (m_formAttributeTargetObserver)
        m_formAttributeTargetObserver->unregister();
    m_formAttributeTargetObserver = newObserver;
}

void FormAssociatedElement::resetFormAttributeTargetObserver()
{
    HTMLElement* element = toHTMLElement(this);
    const AtomicString& formId(element->fastGetAttribute(formAttr));
    if (!formId.isNull() && element->inDocument())
        setFormAttributeTargetObserver(FormAttributeTargetObserver::create(formId, this));
    else
        setFormAttributeTargetObserver(nullptr);
}

void FormAssociatedElement::formAttributeTargetChanged()
{
    resetFormOwner();
}

const AtomicString& FormAssociatedElement::name() const
{
    const AtomicString& name = toHTMLElement(this)->getNameAttribute();
    return name.isNull() ? emptyAtom : name;
}

bool FormAssociatedElement::isFormControlElementWithState() const
{
    return false;
}

const HTMLElement& toHTMLElement(const FormAssociatedElement& associatedElement)
{
    if (associatedElement.isFormControlElement())
        return toHTMLFormControlElement(associatedElement);
    if (associatedElement.isLabelElement())
        return toHTMLLabelElement(associatedElement);
    return toHTMLObjectElement(associatedElement);
}

const HTMLElement* toHTMLElement(const FormAssociatedElement* associatedElement)
{
    ASSERT(associatedElement);
    return &toHTMLElement(*associatedElement);
}

HTMLElement* toHTMLElement(FormAssociatedElement* associatedElement)
{
    return const_cast<HTMLElement*>(toHTMLElement(static_cast<const FormAssociatedElement*>(associatedElement)));
}

HTMLElement& toHTMLElement(FormAssociatedElement& associatedElement)
{
    return const_cast<HTMLElement&>(toHTMLElement(static_cast<const FormAssociatedElement&>(associatedElement)));
}

PassOwnPtrWillBeRawPtr<FormAttributeTargetObserver> FormAttributeTargetObserver::create(const AtomicString& id, FormAssociatedElement* element)
{
    return adoptPtrWillBeNoop(new FormAttributeTargetObserver(id, element));
}

FormAttributeTargetObserver::FormAttributeTargetObserver(const AtomicString& id, FormAssociatedElement* element)
    : IdTargetObserver(toHTMLElement(element)->treeScope().idTargetObserverRegistry(), id)
    , m_element(element)
{
}

DEFINE_TRACE(FormAttributeTargetObserver)
{
    visitor->trace(m_element);
    IdTargetObserver::trace(visitor);
}

void FormAttributeTargetObserver::idTargetChanged()
{
    m_element->formAttributeTargetChanged();
}

} // namespace blink
