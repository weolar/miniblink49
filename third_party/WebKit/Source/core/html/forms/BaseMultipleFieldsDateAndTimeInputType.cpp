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

#include "config.h"
#if ENABLE(INPUT_MULTIPLE_FIELDS_UI)
#include "core/html/forms/BaseMultipleFieldsDateAndTimeInputType.h"

#include "core/CSSValueKeywords.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/events/KeyboardEvent.h"
#include "core/events/ScopedEventQueue.h"
#include "core/html/HTMLDataListElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLOptionElement.h"
#include "core/html/forms/DateTimeFieldsState.h"
#include "core/html/forms/FormController.h"
#include "core/html/shadow/ShadowElementNames.h"
#include "core/layout/LayoutTheme.h"
#include "core/page/FocusController.h"
#include "core/page/Page.h"
#include "platform/DateComponents.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/text/DateTimeFormat.h"
#include "platform/text/PlatformLocale.h"
#include "wtf/DateMath.h"

namespace blink {

class DateTimeFormatValidator : public DateTimeFormat::TokenHandler {
public:
    DateTimeFormatValidator()
        : m_hasYear(false)
        , m_hasMonth(false)
        , m_hasWeek(false)
        , m_hasDay(false)
        , m_hasAMPM(false)
        , m_hasHour(false)
        , m_hasMinute(false)
        , m_hasSecond(false) { }

    void visitField(DateTimeFormat::FieldType, int) final;
    void visitLiteral(const String&) final { }

    bool validateFormat(const String& format, const BaseMultipleFieldsDateAndTimeInputType&);

private:
    bool m_hasYear;
    bool m_hasMonth;
    bool m_hasWeek;
    bool m_hasDay;
    bool m_hasAMPM;
    bool m_hasHour;
    bool m_hasMinute;
    bool m_hasSecond;
};

void DateTimeFormatValidator::visitField(DateTimeFormat::FieldType fieldType, int)
{
    switch (fieldType) {
    case DateTimeFormat::FieldTypeYear:
        m_hasYear = true;
        break;
    case DateTimeFormat::FieldTypeMonth: // Fallthrough.
    case DateTimeFormat::FieldTypeMonthStandAlone:
        m_hasMonth = true;
        break;
    case DateTimeFormat::FieldTypeWeekOfYear:
        m_hasWeek = true;
        break;
    case DateTimeFormat::FieldTypeDayOfMonth:
        m_hasDay = true;
        break;
    case DateTimeFormat::FieldTypePeriod:
        m_hasAMPM = true;
        break;
    case DateTimeFormat::FieldTypeHour11: // Fallthrough.
    case DateTimeFormat::FieldTypeHour12:
        m_hasHour = true;
        break;
    case DateTimeFormat::FieldTypeHour23: // Fallthrough.
    case DateTimeFormat::FieldTypeHour24:
        m_hasHour = true;
        m_hasAMPM = true;
        break;
    case DateTimeFormat::FieldTypeMinute:
        m_hasMinute = true;
        break;
    case DateTimeFormat::FieldTypeSecond:
        m_hasSecond = true;
        break;
    default:
        break;
    }
}

bool DateTimeFormatValidator::validateFormat(const String& format, const BaseMultipleFieldsDateAndTimeInputType& inputType)
{
    if (!DateTimeFormat::parse(format, *this))
        return false;
    return inputType.isValidFormat(m_hasYear, m_hasMonth, m_hasWeek, m_hasDay, m_hasAMPM, m_hasHour, m_hasMinute, m_hasSecond);
}

DateTimeEditElement* BaseMultipleFieldsDateAndTimeInputType::dateTimeEditElement() const
{
    return toDateTimeEditElement(element().userAgentShadowRoot()->getElementById(ShadowElementNames::dateTimeEdit()));
}

SpinButtonElement* BaseMultipleFieldsDateAndTimeInputType::spinButtonElement() const
{
    return toSpinButtonElement(element().userAgentShadowRoot()->getElementById(ShadowElementNames::spinButton()));
}

ClearButtonElement* BaseMultipleFieldsDateAndTimeInputType::clearButtonElement() const
{
    return toClearButtonElement(element().userAgentShadowRoot()->getElementById(ShadowElementNames::clearButton()));
}

PickerIndicatorElement* BaseMultipleFieldsDateAndTimeInputType::pickerIndicatorElement() const
{
    return toPickerIndicatorElement(element().userAgentShadowRoot()->getElementById(ShadowElementNames::pickerIndicator()));
}

inline bool BaseMultipleFieldsDateAndTimeInputType::containsFocusedShadowElement() const
{
    return element().userAgentShadowRoot()->contains(element().document().focusedElement());
}

void BaseMultipleFieldsDateAndTimeInputType::didBlurFromControl()
{
    // We don't need to call blur(). This function is called when control
    // lost focus.

    if (containsFocusedShadowElement())
        return;
    EventQueueScope scope;
    RefPtrWillBeRawPtr<HTMLInputElement> protector(element());
    // Remove focus ring by CSS "focus" pseudo class.
    element().setFocus(false);
    if (SpinButtonElement *spinButton = spinButtonElement())
        spinButton->releaseCapture();
}

void BaseMultipleFieldsDateAndTimeInputType::didFocusOnControl()
{
    // We don't need to call focus(). This function is called when control
    // got focus.

    if (!containsFocusedShadowElement())
        return;
    // Add focus ring by CSS "focus" pseudo class.
    // FIXME: Setting the focus flag to non-focused element is too tricky.
    element().setFocus(true);
}

void BaseMultipleFieldsDateAndTimeInputType::editControlValueChanged()
{
    RefPtrWillBeRawPtr<HTMLInputElement> input(element());
    String oldValue = input->value();
    String newValue = sanitizeValue(dateTimeEditElement()->value());
    // Even if oldValue is null and newValue is "", we should assume they are same.
    if ((oldValue.isEmpty() && newValue.isEmpty()) || oldValue == newValue) {
        input->setNeedsValidityCheck();
    } else {
        input->setValueInternal(newValue, DispatchNoEvent);
        input->setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::ControlValue));
        input->dispatchFormControlInputEvent();
    }
    input->notifyFormStateChanged();
    input->updateClearButtonVisibility();
}

bool BaseMultipleFieldsDateAndTimeInputType::hasCustomFocusLogic() const
{
    return false;
}

bool BaseMultipleFieldsDateAndTimeInputType::isEditControlOwnerDisabled() const
{
    return element().isDisabledFormControl();
}

bool BaseMultipleFieldsDateAndTimeInputType::isEditControlOwnerReadOnly() const
{
    return element().isReadOnly();
}

void BaseMultipleFieldsDateAndTimeInputType::focusAndSelectSpinButtonOwner()
{
    if (DateTimeEditElement* edit = dateTimeEditElement())
        edit->focusIfNoFocus();
}

bool BaseMultipleFieldsDateAndTimeInputType::shouldSpinButtonRespondToMouseEvents()
{
    return !element().isDisabledOrReadOnly();
}

bool BaseMultipleFieldsDateAndTimeInputType::shouldSpinButtonRespondToWheelEvents()
{
    if (!shouldSpinButtonRespondToMouseEvents())
        return false;
    if (DateTimeEditElement* edit = dateTimeEditElement())
        return edit->hasFocusedField();
    return false;
}

void BaseMultipleFieldsDateAndTimeInputType::spinButtonStepDown()
{
    if (DateTimeEditElement* edit = dateTimeEditElement())
        edit->stepDown();
}

void BaseMultipleFieldsDateAndTimeInputType::spinButtonStepUp()
{
    if (DateTimeEditElement* edit = dateTimeEditElement())
        edit->stepUp();
}

void BaseMultipleFieldsDateAndTimeInputType::spinButtonDidReleaseMouseCapture(SpinButtonElement::EventDispatch eventDispatch)
{
    if (eventDispatch == SpinButtonElement::EventDispatchAllowed)
        element().dispatchFormControlChangeEvent();
}

bool BaseMultipleFieldsDateAndTimeInputType::isPickerIndicatorOwnerDisabledOrReadOnly() const
{
    return element().isDisabledOrReadOnly();
}

void BaseMultipleFieldsDateAndTimeInputType::pickerIndicatorChooseValue(const String& value)
{
    if (element().isValidValue(value)) {
        element().setValue(value, DispatchInputAndChangeEvent);
        return;
    }

    DateTimeEditElement* edit = this->dateTimeEditElement();
    if (!edit)
        return;
    EventQueueScope scope;
    DateComponents date;
    unsigned end;
    if (date.parseDate(value, 0, end) && end == value.length())
        edit->setOnlyYearMonthDay(date);
    element().dispatchFormControlChangeEvent();
}

void BaseMultipleFieldsDateAndTimeInputType::pickerIndicatorChooseValue(double value)
{
    ASSERT(std::isfinite(value) || std::isnan(value));
    if (std::isnan(value))
        element().setValue(emptyString(), DispatchInputAndChangeEvent);
    else
        element().setValueAsNumber(value, ASSERT_NO_EXCEPTION, DispatchInputAndChangeEvent);
}

Element& BaseMultipleFieldsDateAndTimeInputType::pickerOwnerElement() const
{
    return element();
}

bool BaseMultipleFieldsDateAndTimeInputType::setupDateTimeChooserParameters(DateTimeChooserParameters& parameters)
{
    return element().setupDateTimeChooserParameters(parameters);
}

BaseMultipleFieldsDateAndTimeInputType::BaseMultipleFieldsDateAndTimeInputType(HTMLInputElement& element)
    : BaseDateAndTimeInputType(element)
    , m_isDestroyingShadowSubtree(false)
    , m_pickerIndicatorIsVisible(false)
    , m_pickerIndicatorIsAlwaysVisible(false)
{
}

BaseMultipleFieldsDateAndTimeInputType::~BaseMultipleFieldsDateAndTimeInputType()
{
#if !ENABLE(OILPAN)
    if (SpinButtonElement* element = spinButtonElement())
        element->removeSpinButtonOwner();
    if (ClearButtonElement* element = clearButtonElement())
        element->removeClearButtonOwner();
    if (DateTimeEditElement* element = dateTimeEditElement())
        element->removeEditControlOwner();
    if (PickerIndicatorElement* element = pickerIndicatorElement())
        element->removePickerIndicatorOwner();
#endif
}

String BaseMultipleFieldsDateAndTimeInputType::badInputText() const
{
    return locale().queryString(WebLocalizedString::ValidationBadInputForDateTime);
}

void BaseMultipleFieldsDateAndTimeInputType::blur()
{
    if (DateTimeEditElement* edit = dateTimeEditElement())
        edit->blurByOwner();
}

PassRefPtr<ComputedStyle> BaseMultipleFieldsDateAndTimeInputType::customStyleForLayoutObject(PassRefPtr<ComputedStyle> originalStyle)
{
    EDisplay originalDisplay = originalStyle->display();
    EDisplay newDisplay = originalDisplay;
    if (originalDisplay == INLINE || originalDisplay == INLINE_BLOCK)
        newDisplay = INLINE_FLEX;
    else if (originalDisplay == BLOCK)
        newDisplay = FLEX;
    TextDirection contentDirection = computedTextDirection();
    if (originalStyle->direction() == contentDirection && originalDisplay == newDisplay)
        return originalStyle;

    RefPtr<ComputedStyle> style = ComputedStyle::clone(*originalStyle);
    style->setDirection(contentDirection);
    style->setDisplay(newDisplay);
    style->setUnique();
    return style.release();
}

void BaseMultipleFieldsDateAndTimeInputType::createShadowSubtree()
{
    ASSERT(element().shadow());

    // Element must not have a layoutObject here, because if it did
    // DateTimeEditElement::customStyleForLayoutObject() is called in appendChild()
    // before the field wrapper element is created.
    // FIXME: This code should not depend on such craziness.
    ASSERT(!element().layoutObject());

    Document& document = element().document();
    ContainerNode* container = element().userAgentShadowRoot();

    container->appendChild(DateTimeEditElement::create(document, *this));
    element().updateView();
    container->appendChild(ClearButtonElement::create(document, *this));
    container->appendChild(SpinButtonElement::create(document, *this));

    if (LayoutTheme::theme().supportsCalendarPicker(formControlType()))
        m_pickerIndicatorIsAlwaysVisible = true;
    container->appendChild(PickerIndicatorElement::create(document, *this));
    m_pickerIndicatorIsVisible = true;
    updatePickerIndicatorVisibility();
}

void BaseMultipleFieldsDateAndTimeInputType::destroyShadowSubtree()
{
    ASSERT(!m_isDestroyingShadowSubtree);
    m_isDestroyingShadowSubtree = true;
    if (SpinButtonElement* element = spinButtonElement())
        element->removeSpinButtonOwner();
    if (ClearButtonElement* element = clearButtonElement())
        element->removeClearButtonOwner();
    if (DateTimeEditElement* element = dateTimeEditElement())
        element->removeEditControlOwner();
    if (PickerIndicatorElement* element = pickerIndicatorElement())
        element->removePickerIndicatorOwner();

    // If a field element has focus, set focus back to the <input> itself before
    // deleting the field. This prevents unnecessary focusout/blur events.
    if (containsFocusedShadowElement())
        element().focus();

    BaseDateAndTimeInputType::destroyShadowSubtree();
    m_isDestroyingShadowSubtree = false;
}

void BaseMultipleFieldsDateAndTimeInputType::handleFocusInEvent(Element* oldFocusedElement, WebFocusType type)
{
    DateTimeEditElement* edit = dateTimeEditElement();
    if (!edit || m_isDestroyingShadowSubtree)
        return;
    if (type == WebFocusTypeBackward) {
        if (element().document().page())
            element().document().page()->focusController().advanceFocus(type);
    } else if (type == WebFocusTypeNone || type == WebFocusTypeMouse || type == WebFocusTypePage) {
        edit->focusByOwner(oldFocusedElement);
    } else {
        edit->focusByOwner();
    }
}

void BaseMultipleFieldsDateAndTimeInputType::forwardEvent(Event* event)
{
    if (SpinButtonElement* element = spinButtonElement()) {
        element->forwardEvent(event);
        if (event->defaultHandled())
            return;
    }

    if (DateTimeEditElement* edit = dateTimeEditElement())
        edit->defaultEventHandler(event);
}

void BaseMultipleFieldsDateAndTimeInputType::disabledAttributeChanged()
{
    EventQueueScope scope;
    spinButtonElement()->releaseCapture();
    if (DateTimeEditElement* edit = dateTimeEditElement())
        edit->disabledStateChanged();
}

void BaseMultipleFieldsDateAndTimeInputType::requiredAttributeChanged()
{
    updateClearButtonVisibility();
}

void BaseMultipleFieldsDateAndTimeInputType::handleKeydownEvent(KeyboardEvent* event)
{
    if (m_pickerIndicatorIsVisible
        && ((event->keyIdentifier() == "Down" && event->getModifierState("Alt")) || (LayoutTheme::theme().shouldOpenPickerWithF4Key() && event->keyIdentifier() == "F4"))) {
        if (PickerIndicatorElement* element = pickerIndicatorElement())
            element->openPopup();
        event->setDefaultHandled();
    } else {
        forwardEvent(event);
    }
}

bool BaseMultipleFieldsDateAndTimeInputType::hasBadInput() const
{
    DateTimeEditElement* edit = dateTimeEditElement();
    return element().value().isEmpty() && edit && edit->anyEditableFieldsHaveValues();
}

AtomicString BaseMultipleFieldsDateAndTimeInputType::localeIdentifier() const
{
    return element().computeInheritedLanguage();
}

void BaseMultipleFieldsDateAndTimeInputType::editControlDidChangeValueByKeyboard()
{
    element().dispatchFormControlChangeEvent();
}

void BaseMultipleFieldsDateAndTimeInputType::minOrMaxAttributeChanged()
{
    updateView();
}

void BaseMultipleFieldsDateAndTimeInputType::readonlyAttributeChanged()
{
    EventQueueScope scope;
    spinButtonElement()->releaseCapture();
    if (DateTimeEditElement* edit = dateTimeEditElement())
        edit->readOnlyStateChanged();
}

void BaseMultipleFieldsDateAndTimeInputType::restoreFormControlState(const FormControlState& state)
{
    DateTimeEditElement* edit = dateTimeEditElement();
    if (!edit)
        return;
    DateTimeFieldsState dateTimeFieldsState = DateTimeFieldsState::restoreFormControlState(state);
    edit->setValueAsDateTimeFieldsState(dateTimeFieldsState);
    element().setValueInternal(sanitizeValue(edit->value()), DispatchNoEvent);
    updateClearButtonVisibility();
}

FormControlState BaseMultipleFieldsDateAndTimeInputType::saveFormControlState() const
{
    if (DateTimeEditElement* edit = dateTimeEditElement())
        return edit->valueAsDateTimeFieldsState().saveFormControlState();
    return FormControlState();
}

void BaseMultipleFieldsDateAndTimeInputType::setValue(const String& sanitizedValue, bool valueChanged, TextFieldEventBehavior eventBehavior)
{
    InputType::setValue(sanitizedValue, valueChanged, eventBehavior);
    DateTimeEditElement* edit = dateTimeEditElement();
    if (valueChanged || (sanitizedValue.isEmpty() && edit && edit->anyEditableFieldsHaveValues())) {
        element().updateView();
        element().setNeedsValidityCheck();
    }
}

void BaseMultipleFieldsDateAndTimeInputType::stepAttributeChanged()
{
    updateView();
}

void BaseMultipleFieldsDateAndTimeInputType::updateView()
{
    DateTimeEditElement* edit = dateTimeEditElement();
    if (!edit)
        return;

    DateTimeEditElement::LayoutParameters layoutParameters(element().locale(), createStepRange(AnyIsDefaultStep));

    DateComponents date;
    bool hasValue = false;
    if (!element().suggestedValue().isNull())
        hasValue = parseToDateComponents(element().suggestedValue(), &date);
    else
        hasValue = parseToDateComponents(element().value(), &date);
    if (!hasValue)
        setMillisecondToDateComponents(layoutParameters.stepRange.minimum().toDouble(), &date);

    setupLayoutParameters(layoutParameters, date);

    DEFINE_STATIC_LOCAL(AtomicString, datetimeformatAttr, ("datetimeformat", AtomicString::ConstructFromLiteral));
    edit->setAttribute(datetimeformatAttr, AtomicString(layoutParameters.dateTimeFormat), ASSERT_NO_EXCEPTION);
    const AtomicString pattern = edit->fastGetAttribute(HTMLNames::patternAttr);
    if (!pattern.isEmpty())
        layoutParameters.dateTimeFormat = pattern;

    if (!DateTimeFormatValidator().validateFormat(layoutParameters.dateTimeFormat, *this))
        layoutParameters.dateTimeFormat = layoutParameters.fallbackDateTimeFormat;

    if (hasValue)
        edit->setValueAsDate(layoutParameters, date);
    else
        edit->setEmptyValue(layoutParameters, date);
    updateClearButtonVisibility();
}

void BaseMultipleFieldsDateAndTimeInputType::valueAttributeChanged()
{
    if (!element().hasDirtyValue())
        updateView();
}

void BaseMultipleFieldsDateAndTimeInputType::listAttributeTargetChanged()
{
    updatePickerIndicatorVisibility();
}

void BaseMultipleFieldsDateAndTimeInputType::updatePickerIndicatorVisibility()
{
    if (m_pickerIndicatorIsAlwaysVisible) {
        showPickerIndicator();
        return;
    }
    if (element().hasValidDataListOptions())
        showPickerIndicator();
    else
        hidePickerIndicator();
}

void BaseMultipleFieldsDateAndTimeInputType::hidePickerIndicator()
{
    if (!m_pickerIndicatorIsVisible)
        return;
    m_pickerIndicatorIsVisible = false;
    ASSERT(pickerIndicatorElement());
    pickerIndicatorElement()->setInlineStyleProperty(CSSPropertyDisplay, CSSValueNone);
}

void BaseMultipleFieldsDateAndTimeInputType::showPickerIndicator()
{
    if (m_pickerIndicatorIsVisible)
        return;
    m_pickerIndicatorIsVisible = true;
    ASSERT(pickerIndicatorElement());
    pickerIndicatorElement()->removeInlineStyleProperty(CSSPropertyDisplay);
}

void BaseMultipleFieldsDateAndTimeInputType::focusAndSelectClearButtonOwner()
{
    element().focus();
}

bool BaseMultipleFieldsDateAndTimeInputType::shouldClearButtonRespondToMouseEvents()
{
    return !element().isDisabledOrReadOnly() && !element().isRequired();
}

void BaseMultipleFieldsDateAndTimeInputType::clearValue()
{
    RefPtrWillBeRawPtr<HTMLInputElement> input(element());
    input->setValue("", DispatchInputAndChangeEvent);
    input->updateClearButtonVisibility();
}

void BaseMultipleFieldsDateAndTimeInputType::updateClearButtonVisibility()
{
    ClearButtonElement* clearButton = clearButtonElement();
    if (!clearButton)
        return;

    if (element().isRequired() || !dateTimeEditElement()->anyEditableFieldsHaveValues()) {
        clearButton->setInlineStyleProperty(CSSPropertyOpacity, 0.0, CSSPrimitiveValue::CSS_NUMBER);
        clearButton->setInlineStyleProperty(CSSPropertyPointerEvents, CSSValueNone);
    } else {
        clearButton->removeInlineStyleProperty(CSSPropertyOpacity);
        clearButton->removeInlineStyleProperty(CSSPropertyPointerEvents);
    }
}

TextDirection BaseMultipleFieldsDateAndTimeInputType::computedTextDirection()
{
    return element().locale().isRTL() ? RTL : LTR;
}

AXObject* BaseMultipleFieldsDateAndTimeInputType::popupRootAXObject()
{
    if (PickerIndicatorElement* picker = pickerIndicatorElement())
        return picker->popupRootAXObject();
    return nullptr;
}

} // namespace blink

#endif
