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

#ifndef RangeInputType_h
#define RangeInputType_h

#include "core/html/forms/InputType.h"

namespace blink {

class ExceptionState;
class SliderThumbElement;

class RangeInputType final : public InputType {
public:
    static PassRefPtrWillBeRawPtr<InputType> create(HTMLInputElement&);

private:
    RangeInputType(HTMLInputElement&);
    void countUsage() override;
    const AtomicString& formControlType() const override;
    double valueAsDouble() const override;
    void setValueAsDouble(double, TextFieldEventBehavior, ExceptionState&) const override;
    bool typeMismatchFor(const String&) const override;
    bool supportsRequired() const override;
    StepRange createStepRange(AnyStepHandling) const override;
    bool isSteppable() const override;
    void handleMouseDownEvent(MouseEvent*) override;
    void handleTouchEvent(TouchEvent*) override;
    bool hasTouchEventHandler() const override;
    void handleKeydownEvent(KeyboardEvent*) override;
    LayoutObject* createLayoutObject(const ComputedStyle&) const override;
    void createShadowSubtree() override;
    Decimal parseToNumber(const String&, const Decimal&) const override;
    String serialize(const Decimal&) const override;
    void accessKeyAction(bool sendMouseEvents) override;
    void sanitizeValueInResponseToMinOrMaxAttributeChange() override;
    void setValue(const String&, bool valueChanged, TextFieldEventBehavior) override;
    String fallbackValue() const override;
    String sanitizeValue(const String& proposedValue) const override;
    bool shouldRespectListAttribute() override;
    void disabledAttributeChanged() override;
    void listAttributeTargetChanged() override;
    Decimal findClosestTickMarkValue(const Decimal&) override;

    SliderThumbElement* sliderThumbElement() const;
    Element* sliderTrackElement() const;
    void updateTickMarkValues();

    // InputTypeView function:
    void updateView() override;

    bool m_tickMarkValuesDirty;
    Vector<Decimal> m_tickMarkValues;
};

} // namespace blink

#endif // RangeInputType_h
