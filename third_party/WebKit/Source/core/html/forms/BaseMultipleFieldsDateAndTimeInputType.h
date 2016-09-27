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

#ifndef BaseMultipleFieldsDateAndTimeInputType_h
#define BaseMultipleFieldsDateAndTimeInputType_h

#if ENABLE(INPUT_MULTIPLE_FIELDS_UI)
#include "core/html/forms/BaseDateAndTimeInputType.h"

#include "core/html/shadow/ClearButtonElement.h"
#include "core/html/shadow/DateTimeEditElement.h"
#include "core/html/shadow/PickerIndicatorElement.h"
#include "core/html/shadow/SpinButtonElement.h"

namespace blink {

struct DateTimeChooserParameters;

class BaseMultipleFieldsDateAndTimeInputType
    : public BaseDateAndTimeInputType
    , protected DateTimeEditElement::EditControlOwner
    , protected PickerIndicatorElement::PickerIndicatorOwner
    , protected SpinButtonElement::SpinButtonOwner
    , protected ClearButtonElement::ClearButtonOwner {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(BaseMultipleFieldsDateAndTimeInputType);

public:
    virtual bool isValidFormat(bool hasYear, bool hasMonth, bool hasWeek, bool hasDay, bool hasAMPM, bool hasHour, bool hasMinute, bool hasSecond) const = 0;

    DEFINE_INLINE_VIRTUAL_TRACE() { BaseDateAndTimeInputType::trace(visitor); }

protected:
    BaseMultipleFieldsDateAndTimeInputType(HTMLInputElement&);
    ~BaseMultipleFieldsDateAndTimeInputType() override;

    virtual void setupLayoutParameters(DateTimeEditElement::LayoutParameters&, const DateComponents&) const = 0;

private:
    // DateTimeEditElement::EditControlOwner functions
    void didBlurFromControl() final;
    void didFocusOnControl() final;
    void editControlValueChanged() final;
    bool isEditControlOwnerDisabled() const final;
    bool isEditControlOwnerReadOnly() const final;
    AtomicString localeIdentifier() const final;
    void editControlDidChangeValueByKeyboard() final;

    // SpinButtonElement::SpinButtonOwner functions.
    void focusAndSelectSpinButtonOwner() override;
    bool shouldSpinButtonRespondToMouseEvents() override;
    bool shouldSpinButtonRespondToWheelEvents() override;
    void spinButtonStepDown() override;
    void spinButtonStepUp() override;
    void spinButtonDidReleaseMouseCapture(SpinButtonElement::EventDispatch) override;

    // PickerIndicatorElement::PickerIndicatorOwner functions
    bool isPickerIndicatorOwnerDisabledOrReadOnly() const final;
    void pickerIndicatorChooseValue(const String&) final;
    void pickerIndicatorChooseValue(double) final;
    Element& pickerOwnerElement() const final;
    bool setupDateTimeChooserParameters(DateTimeChooserParameters&) final;

    // ClearButtonElement::ClearButtonOwner functions.
    void focusAndSelectClearButtonOwner() override;
    bool shouldClearButtonRespondToMouseEvents() override;
    void clearValue() override;

    // InputType functions
    String badInputText() const override;
    void blur() final;
    PassRefPtr<ComputedStyle> customStyleForLayoutObject(PassRefPtr<ComputedStyle>) override;
    void createShadowSubtree() final;
    void destroyShadowSubtree() final;
    void disabledAttributeChanged() final;
    void forwardEvent(Event*) final;
    void handleFocusInEvent(Element* oldFocusedElement, WebFocusType) final;
    void handleKeydownEvent(KeyboardEvent*) final;
    bool hasBadInput() const override;
    bool hasCustomFocusLogic() const final;
    void minOrMaxAttributeChanged() final;
    void readonlyAttributeChanged() final;
    void requiredAttributeChanged() final;
    void restoreFormControlState(const FormControlState&) final;
    FormControlState saveFormControlState() const final;
    void setValue(const String&, bool valueChanged, TextFieldEventBehavior) final;
    void stepAttributeChanged() final;
    void updateView() final;
    void valueAttributeChanged() override;
    void listAttributeTargetChanged() final;
    void updateClearButtonVisibility() final;
    TextDirection computedTextDirection() final;
    AXObject* popupRootAXObject() final;

    DateTimeEditElement* dateTimeEditElement() const;
    SpinButtonElement* spinButtonElement() const;
    ClearButtonElement* clearButtonElement() const;
    PickerIndicatorElement* pickerIndicatorElement() const;
    bool containsFocusedShadowElement() const;
    void showPickerIndicator();
    void hidePickerIndicator();
    void updatePickerIndicatorVisibility();

    bool m_isDestroyingShadowSubtree;
    bool m_pickerIndicatorIsVisible;
    bool m_pickerIndicatorIsAlwaysVisible;
};

} // namespace blink

#endif
#endif // BaseMultipleFieldsDateAndTimeInputType_h
