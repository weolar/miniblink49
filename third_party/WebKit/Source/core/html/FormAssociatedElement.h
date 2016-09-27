/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
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

#ifndef FormAssociatedElement_h
#define FormAssociatedElement_h

#include "core/CoreExport.h"
#include "platform/heap/Handle.h"
#include "wtf/WeakPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ContainerNode;
class Document;
class FormAttributeTargetObserver;
class FormDataList;
class HTMLElement;
class HTMLFormElement;
class Node;
class ValidityState;

class CORE_EXPORT FormAssociatedElement : public WillBeGarbageCollectedMixin {
public:
    virtual ~FormAssociatedElement();

#if !ENABLE(OILPAN)
    void ref() { refFormAssociatedElement(); }
    void deref() { derefFormAssociatedElement(); }
#endif

    static HTMLFormElement* findAssociatedForm(const HTMLElement*);
    HTMLFormElement* form() const { return m_form.get(); }
    ValidityState* validity();

    virtual bool isFormControlElement() const = 0;
    virtual bool isFormControlElementWithState() const;
    virtual bool isEnumeratable() const = 0;
    virtual bool isLabelElement() const { return false; }

    // Returns the 'name' attribute value. If this element has no name
    // attribute, it returns an empty string instead of null string.
    // Note that the 'name' IDL attribute doesn't use this function.
    virtual const AtomicString& name() const;

    // Override in derived classes to get the encoded name=value pair for submitting.
    // Return true for a successful control (see HTML4-17.13.2).
    virtual bool appendFormData(FormDataList&, bool) { return false; }

    void resetFormOwner();

    void formRemovedFromTree(const Node& formRoot);

    // ValidityState attribute implementations
    bool customError() const;

    // Override functions for patterMismatch, rangeOverflow, rangerUnderflow,
    // stepMismatch, tooLong, tooShort and valueMissing must call willValidate method.
    virtual bool hasBadInput() const;
    virtual bool patternMismatch() const;
    virtual bool rangeOverflow() const;
    virtual bool rangeUnderflow() const;
    virtual bool stepMismatch() const;
    virtual bool tooLong() const;
    virtual bool tooShort() const;
    virtual bool typeMismatch() const;
    virtual bool valueMissing() const;
    virtual String validationMessage() const;
    bool valid() const;
    virtual void setCustomValidity(const String&);

    void formAttributeTargetChanged();

    typedef WillBeHeapVector<RawPtrWillBeMember<FormAssociatedElement>> List;

    DECLARE_VIRTUAL_TRACE();

protected:
    FormAssociatedElement();

    void insertedInto(ContainerNode*);
    void removedFrom(ContainerNode*);
    void didMoveToNewDocument(Document& oldDocument);

    // FIXME: Remove usage of setForm. resetFormOwner should be enough, and
    // setForm is confusing.
    void setForm(HTMLFormElement*);
    void associateByParser(HTMLFormElement*);
    void formAttributeChanged();

    // If you add an override of willChangeForm() or didChangeForm() to a class
    // derived from this one, you will need to add a call to setForm(0) to the
    // destructor of that class.
    virtual void willChangeForm();
    virtual void didChangeForm();

    String customValidationMessage() const;

private:
#if !ENABLE(OILPAN)
    virtual void refFormAssociatedElement() = 0;
    virtual void derefFormAssociatedElement() = 0;
#endif

    void setFormAttributeTargetObserver(PassOwnPtrWillBeRawPtr<FormAttributeTargetObserver>);
    void resetFormAttributeTargetObserver();

    OwnPtrWillBeMember<FormAttributeTargetObserver> m_formAttributeTargetObserver;
#if ENABLE(OILPAN)
    Member<HTMLFormElement> m_form;
#else
    WeakPtr<HTMLFormElement> m_form;
#endif
    OwnPtrWillBeMember<ValidityState> m_validityState;
    String m_customValidationMessage;
    // Non-Oilpan: Even if m_formWasSetByParser is true, m_form can be null
    // because parentNode is not a strong reference and |this| and m_form don't
    // die together.
    // Oilpan: If m_formWasSetByParser is true, m_form is always non-null.
    bool m_formWasSetByParser;
};

HTMLElement* toHTMLElement(FormAssociatedElement*);
HTMLElement& toHTMLElement(FormAssociatedElement&);
const HTMLElement* toHTMLElement(const FormAssociatedElement*);
const HTMLElement& toHTMLElement(const FormAssociatedElement&);

} // namespace

#endif // FormAssociatedElement_h
