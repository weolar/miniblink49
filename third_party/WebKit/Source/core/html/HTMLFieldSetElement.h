/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2010 Apple Inc. All rights reserved.
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

#ifndef HTMLFieldSetElement_h
#define HTMLFieldSetElement_h

#include "core/CoreExport.h"
#include "core/html/HTMLFormControlElement.h"

namespace blink {

class HTMLFormControlsCollection;

class CORE_EXPORT HTMLFieldSetElement final : public HTMLFormControlElement {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<HTMLFieldSetElement> create(Document&, HTMLFormElement*);
    DECLARE_VIRTUAL_TRACE();
    HTMLLegendElement* legend() const;

    PassRefPtrWillBeRawPtr<HTMLFormControlsCollection> elements();

    const FormAssociatedElement::List& associatedElements() const;

protected:
    void disabledAttributeChanged() override;

private:
    HTMLFieldSetElement(Document&, HTMLFormElement*);

    bool isEnumeratable() const override { return true; }
    bool supportsFocus() const override;
    LayoutObject* createLayoutObject(const ComputedStyle&) override;
    const AtomicString& formControlType() const override;
    bool recalcWillValidate() const override { return false; }
    bool matchesValidityPseudoClasses() const final;
    bool isValidElement() final;
    void childrenChanged(const ChildrenChange&) override;
    bool areAuthorShadowsAllowed() const override { return false; }
    bool isSubmittableElement() override;
    bool alwaysCreateUserAgentShadowRoot() const override { return false; }

    static void invalidateDisabledStateUnder(Element&);
    void refreshElementsIfNeeded() const;

    mutable FormAssociatedElement::List m_associatedElements;
    // When dom tree is modified, we have to refresh the m_associatedElements array.
    mutable uint64_t m_documentVersion;
};

} // namespace blink

#endif // HTMLFieldSetElement_h
