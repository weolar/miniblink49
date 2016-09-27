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

#ifndef NumberInputType_h
#define NumberInputType_h

#include "core/html/forms/TextFieldInputType.h"

namespace blink {

class ExceptionState;

class NumberInputType final : public TextFieldInputType {
public:
    static PassRefPtrWillBeRawPtr<InputType> create(HTMLInputElement&);

private:
    NumberInputType(HTMLInputElement& element) : TextFieldInputType(element) { }
    void countUsage() override;
    const AtomicString& formControlType() const override;
    void setValue(const String&, bool valueChanged, TextFieldEventBehavior) override;
    double valueAsDouble() const override;
    void setValueAsDouble(double, TextFieldEventBehavior, ExceptionState&) const override;
    void setValueAsDecimal(const Decimal&, TextFieldEventBehavior, ExceptionState&) const override;
    bool typeMismatchFor(const String&) const override;
    bool typeMismatch() const override;
    bool sizeShouldIncludeDecoration(int defaultSize, int& preferredSize) const override;
    bool isSteppable() const override;
    StepRange createStepRange(AnyStepHandling) const override;
    void handleKeydownEvent(KeyboardEvent*) override;
    void handleBeforeTextInsertedEvent(BeforeTextInsertedEvent*) override;
    Decimal parseToNumber(const String&, const Decimal&) const override;
    String serialize(const Decimal&) const override;
    String localizeValue(const String&) const override;
    String visibleValue() const override;
    String convertFromVisibleValue(const String&) const override;
    String sanitizeValue(const String&) const override;
    bool hasBadInput() const override;
    String badInputText() const override;
    String rangeOverflowText(const Decimal& maxmum) const override;
    String rangeUnderflowText(const Decimal& minimum) const override;
    bool supportsPlaceholder() const override;
    void minOrMaxAttributeChanged() override;
    void stepAttributeChanged() override;
    bool supportsSelectionAPI() const override;
};

} // namespace blink

#endif // NumberInputType_h
