/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef DateTimeNumericFieldElement_h
#define DateTimeNumericFieldElement_h

#if ENABLE(INPUT_MULTIPLE_FIELDS_UI)
#include "core/html/shadow/DateTimeFieldElement.h"

#include "wtf/text/StringBuilder.h"
#include "wtf/text/WTFString.h"

namespace blink {

// DateTimeNumericFieldElement represents numeric field of date time format,
// such as:
//  - hour
//  - minute
//  - millisecond
//  - second
//  - year
class DateTimeNumericFieldElement : public DateTimeFieldElement {
    WTF_MAKE_NONCOPYABLE(DateTimeNumericFieldElement);

public:
    struct Step {
        Step(int step = 1, int stepBase = 0) : step(step), stepBase(stepBase) { }
        int step;
        int stepBase;
    };

    struct Range {
        Range(int minimum, int maximum) : minimum(minimum), maximum(maximum) { }
        int clampValue(int) const;
        bool isInRange(int) const;
        bool isSingleton() const { return minimum == maximum; }

        int minimum;
        int maximum;
    };

protected:
    DateTimeNumericFieldElement(Document&, FieldOwner&, const Range&, const Range& hardLimits, const String& placeholder, const Step& = Step());

    int clampValue(int value) const { return m_range.clampValue(value); }
    virtual int defaultValueForStepDown() const;
    virtual int defaultValueForStepUp() const;
    const Range& range() const { return m_range; }

    // DateTimeFieldElement functions.
    bool hasValue() const final;
    void initialize(const AtomicString& pseudo, const String& axHelpText);
    int maximum() const;
    void setEmptyValue(EventBehavior = DispatchNoEvent) final;
    void setValueAsInteger(int, EventBehavior = DispatchNoEvent) override;
    int valueAsInteger() const final;
    String visibleValue() const final;

private:
    // DateTimeFieldElement functions.
    void handleKeyboardEvent(KeyboardEvent*) final;
    float maximumWidth(const Font&) override;
    void stepDown() final;
    void stepUp() final;
    String value() const final;

    // Node functions.
    void setFocus(bool) final;

    String formatValue(int) const;
    int roundUp(int) const;
    int roundDown(int) const;
    int typeAheadValue() const;

    const String m_placeholder;
    const Range m_range;
    const Range m_hardLimits;
    const Step m_step;
    int m_value;
    bool m_hasValue;
    mutable StringBuilder m_typeAheadBuffer;
};

} // namespace blink

#endif
#endif
