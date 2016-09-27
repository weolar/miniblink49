/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef TextFieldInputType_h
#define TextFieldInputType_h

#include "core/html/forms/InputType.h"
#include "core/html/shadow/SpinButtonElement.h"

namespace blink {

class FormDataList;

// The class represents types of which UI contain text fields.
// It supports not only the types for BaseTextInputType but also type=number.
class TextFieldInputType : public InputType, protected SpinButtonElement::SpinButtonOwner {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(TextFieldInputType);
public:
    DEFINE_INLINE_VIRTUAL_TRACE() { InputType::trace(visitor); }

protected:
    TextFieldInputType(HTMLInputElement&);
    ~TextFieldInputType() override;
    bool canSetSuggestedValue() override;
    void handleKeydownEvent(KeyboardEvent*) override;

    void createShadowSubtree() override;
    void destroyShadowSubtree() override;
    void attributeChanged() override;
    void disabledAttributeChanged() override;
    void readonlyAttributeChanged() override;
    bool supportsReadOnly() const override;
    void handleFocusEvent(Element* oldFocusedNode, WebFocusType) final;
    void handleBlurEvent() final;
    void setValue(const String&, bool valueChanged, TextFieldEventBehavior) override;
    void updateView() override;

    virtual bool needsContainer() const { return false; }
    virtual String convertFromVisibleValue(const String&) const;
    enum ValueChangeState {
        ValueChangeStateNone,
        ValueChangeStateChanged
    };
    virtual void didSetValueByUserEdit(ValueChangeState);

    void handleKeydownEventForSpinButton(KeyboardEvent*);
    bool shouldHaveSpinButton() const;
    Element* containerElement() const;

private:
    bool shouldShowFocusRingOnMouseFocus() const final;
    bool isTextField() const final;
    bool valueMissing(const String&) const override;
    void handleBeforeTextInsertedEvent(BeforeTextInsertedEvent*) override;
    void forwardEvent(Event*) final;
    bool shouldSubmitImplicitly(Event*) final;
    LayoutObject* createLayoutObject(const ComputedStyle&) const override;
    String sanitizeValue(const String&) const override;
    bool shouldRespectListAttribute() override;
    void listAttributeTargetChanged() override;
    void updatePlaceholderText() final;
    bool appendFormData(FormDataList&, bool multipart) const override;
    void subtreeHasChanged() final;

    // SpinButtonElement::SpinButtonOwner functions.
    void focusAndSelectSpinButtonOwner() final;
    bool shouldSpinButtonRespondToMouseEvents() final;
    bool shouldSpinButtonRespondToWheelEvents() final;
    void spinButtonStepDown() final;
    void spinButtonStepUp() final;
    void spinButtonDidReleaseMouseCapture(SpinButtonElement::EventDispatch) final;

    SpinButtonElement* spinButtonElement() const;
};

} // namespace blink

#endif // TextFieldInputType_h
