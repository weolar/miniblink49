/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2010 Apple Inc. All rights reserved.
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

#ifndef HTMLTextAreaElement_h
#define HTMLTextAreaElement_h

#include "core/CoreExport.h"
#include "core/html/HTMLTextFormControlElement.h"

namespace blink {

class BeforeTextInsertedEvent;
class ExceptionState;

class CORE_EXPORT HTMLTextAreaElement final : public HTMLTextFormControlElement {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<HTMLTextAreaElement> create(Document&, HTMLFormElement*);

    unsigned cols() const { return m_cols; }
    unsigned rows() const { return m_rows; }

    bool shouldWrapText() const { return m_wrap != NoWrap; }

    String value() const override;
    void setValue(const String&, TextFieldEventBehavior = DispatchNoEvent);
    String defaultValue() const;
    void setDefaultValue(const String&);
    int textLength() const { return value().length(); }
    int maxLength() const;
    int minLength() const;
    void setMaxLength(int, ExceptionState&);
    void setMinLength(int, ExceptionState&);

    String suggestedValue() const;
    void setSuggestedValue(const String&);

    // For ValidityState
    String validationMessage() const override;
    bool valueMissing() const override;
    bool tooLong() const override;
    bool tooShort() const override;
    bool isValidValue(const String&) const;

    void setCols(unsigned);
    void setRows(unsigned);

private:
    HTMLTextAreaElement(Document&, HTMLFormElement*);

    enum WrapMethod { NoWrap, SoftWrap, HardWrap };
    enum SetValueCommonOption {
        NotSetSelection,
        SetSeletion
    };

    void didAddUserAgentShadowRoot(ShadowRoot&) override;
    // FIXME: Author shadows should be allowed
    // https://bugs.webkit.org/show_bug.cgi?id=92608
    bool areAuthorShadowsAllowed() const override { return false; }

    void handleBeforeTextInsertedEvent(BeforeTextInsertedEvent*) const;
    static String sanitizeUserInputValue(const String&, unsigned maxLength);
    void updateValue() const;
    void setInnerEditorValue(const String&) override;
    void setNonDirtyValue(const String&);
    void setValueCommon(const String&, TextFieldEventBehavior, SetValueCommonOption = NotSetSelection);

    bool supportsPlaceholder() const override { return true; }
    void updatePlaceholderText() override;
    bool isEmptyValue() const override { return value().isEmpty(); }
    bool isEmptySuggestedValue() const final { return suggestedValue().isEmpty(); }
    bool supportsAutocapitalize() const override { return true; }
    const AtomicString& defaultAutocapitalize() const override;

    bool isOptionalFormControl() const override { return !isRequiredFormControl(); }
    bool isRequiredFormControl() const override { return isRequired(); }

    void defaultEventHandler(Event*) override;
    void handleFocusEvent(Element* oldFocusedNode, WebFocusType) override;

    void subtreeHasChanged() override;

    bool isEnumeratable() const override { return true; }
    bool isInteractiveContent() const override;
    bool supportsAutofocus() const override;
    bool supportLabels() const override { return true; }

    const AtomicString& formControlType() const override;

    FormControlState saveFormControlState() const override;
    void restoreFormControlState(const FormControlState&) override;

    bool isTextFormControl() const override { return true; }

    void childrenChanged(const ChildrenChange&) override;
    void parseAttribute(const QualifiedName&, const AtomicString&) override;
    bool isPresentationAttribute(const QualifiedName&) const override;
    void collectStyleForPresentationAttribute(const QualifiedName&, const AtomicString&, MutableStylePropertySet*) override;
    LayoutObject* createLayoutObject(const ComputedStyle&) override;
    bool appendFormData(FormDataList&, bool) override;
    void resetImpl() override;
    bool hasCustomFocusLogic() const override;
    bool shouldShowFocusRingOnMouseFocus() const override;
    bool isKeyboardFocusable() const override;
    void updateFocusAppearance(bool restorePreviousSelection) override;

    void accessKeyAction(bool sendMouseEvents) override;

    bool matchesReadOnlyPseudoClass() const override;
    bool matchesReadWritePseudoClass() const override;
    void copyNonAttributePropertiesFromElement(const Element&) final;

    // If the String* argument is 0, apply this->value().
    bool valueMissing(const String*) const;
    bool tooLong(const String*, NeedsToCheckDirtyFlag) const;
    bool tooShort(const String*, NeedsToCheckDirtyFlag) const;

    unsigned m_rows;
    unsigned m_cols;
    WrapMethod m_wrap;
    mutable String m_value;
    mutable bool m_isDirty;
    bool m_valueIsUpToDate;
    String m_suggestedValue;
};

} // namespace blink

#endif // HTMLTextAreaElement_h
