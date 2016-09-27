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

#ifndef DateInputType_h
#define DateInputType_h

#include "core/html/forms/BaseChooserOnlyDateAndTimeInputType.h"
#include "core/html/forms/BaseMultipleFieldsDateAndTimeInputType.h"

namespace blink {

#if ENABLE(INPUT_MULTIPLE_FIELDS_UI)
using BaseDateInputType = BaseMultipleFieldsDateAndTimeInputType;
#else
using BaseDateInputType = BaseChooserOnlyDateAndTimeInputType;
#endif

class DateInputType final : public BaseDateInputType {
public:
    static PassRefPtrWillBeRawPtr<InputType> create(HTMLInputElement&);

private:
    DateInputType(HTMLInputElement&);
    void countUsage() override;
    const AtomicString& formControlType() const override;
    StepRange createStepRange(AnyStepHandling) const override;
    bool parseToDateComponentsInternal(const String&, DateComponents*) const override;
    bool setMillisecondToDateComponents(double, DateComponents*) const override;
    void warnIfValueIsInvalid(const String&) const override;

#if ENABLE(INPUT_MULTIPLE_FIELDS_UI)
    // BaseMultipleFieldsDateAndTimeInputType functions
    String formatDateTimeFieldsState(const DateTimeFieldsState&) const override;
    void setupLayoutParameters(DateTimeEditElement::LayoutParameters&, const DateComponents&) const override;
    bool isValidFormat(bool hasYear, bool hasMonth, bool hasWeek, bool hasDay, bool hasAMPM, bool hasHour, bool hasMinute, bool hasSecond) const override;
#endif
};

} // namespace blink

#endif // DateInputType_h
