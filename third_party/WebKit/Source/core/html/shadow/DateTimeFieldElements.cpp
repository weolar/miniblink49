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

#include "config.h"
#if ENABLE(INPUT_MULTIPLE_FIELDS_UI)
#include "core/html/shadow/DateTimeFieldElements.h"

#include "core/html/forms/DateTimeFieldsState.h"
#include "platform/DateComponents.h"
#include "platform/text/PlatformLocale.h"
#include "wtf/CurrentTime.h"
#include "wtf/DateMath.h"

namespace blink {

using blink::WebLocalizedString;

static String queryString(WebLocalizedString::Name name)
{
    return Locale::defaultLocale().queryString(name);
}

DateTimeAMPMFieldElement::DateTimeAMPMFieldElement(Document& document, FieldOwner& fieldOwner, const Vector<String>& ampmLabels)
    : DateTimeSymbolicFieldElement(document, fieldOwner, ampmLabels, 0, 1)
{
}

PassRefPtrWillBeRawPtr<DateTimeAMPMFieldElement> DateTimeAMPMFieldElement::create(Document& document, FieldOwner& fieldOwner, const Vector<String>& ampmLabels)
{
    DEFINE_STATIC_LOCAL(AtomicString, ampmPseudoId, ("-webkit-datetime-edit-ampm-field", AtomicString::ConstructFromLiteral));
    RefPtrWillBeRawPtr<DateTimeAMPMFieldElement> field = adoptRefWillBeNoop(new DateTimeAMPMFieldElement(document, fieldOwner, ampmLabels));
    field->initialize(ampmPseudoId, queryString(WebLocalizedString::AXAMPMFieldText));
    return field.release();
}

void DateTimeAMPMFieldElement::populateDateTimeFieldsState(DateTimeFieldsState& dateTimeFieldsState)
{
    if (hasValue())
        dateTimeFieldsState.setAMPM(valueAsInteger() ? DateTimeFieldsState::AMPMValuePM : DateTimeFieldsState::AMPMValueAM);
    else
        dateTimeFieldsState.setAMPM(DateTimeFieldsState::AMPMValueEmpty);
}

void DateTimeAMPMFieldElement::setValueAsDate(const DateComponents& date)
{
    setValueAsInteger(date.hour() >= 12 ? 1 : 0);
}

void DateTimeAMPMFieldElement::setValueAsDateTimeFieldsState(const DateTimeFieldsState& dateTimeFieldsState)
{
    if (dateTimeFieldsState.hasAMPM())
        setValueAsInteger(dateTimeFieldsState.ampm());
    else
        setEmptyValue();
}

// ----------------------------

DateTimeDayFieldElement::DateTimeDayFieldElement(Document& document, FieldOwner& fieldOwner, const String& placeholder, const Range& range)
    : DateTimeNumericFieldElement(document, fieldOwner, range, Range(1, 31), placeholder)
{
}

PassRefPtrWillBeRawPtr<DateTimeDayFieldElement> DateTimeDayFieldElement::create(Document& document, FieldOwner& fieldOwner, const String& placeholder, const Range& range)
{
    DEFINE_STATIC_LOCAL(AtomicString, dayPseudoId, ("-webkit-datetime-edit-day-field", AtomicString::ConstructFromLiteral));
    RefPtrWillBeRawPtr<DateTimeDayFieldElement> field = adoptRefWillBeNoop(new DateTimeDayFieldElement(document, fieldOwner, placeholder.isEmpty() ? "--" : placeholder, range));
    field->initialize(dayPseudoId, queryString(WebLocalizedString::AXDayOfMonthFieldText));
    return field.release();
}

void DateTimeDayFieldElement::populateDateTimeFieldsState(DateTimeFieldsState& dateTimeFieldsState)
{
    dateTimeFieldsState.setDayOfMonth(hasValue() ? valueAsInteger() : DateTimeFieldsState::emptyValue);
}

void DateTimeDayFieldElement::setValueAsDate(const DateComponents& date)
{
    setValueAsInteger(date.monthDay());
}

void DateTimeDayFieldElement::setValueAsDateTimeFieldsState(const DateTimeFieldsState& dateTimeFieldsState)
{
    if (!dateTimeFieldsState.hasDayOfMonth()) {
        setEmptyValue();
        return;
    }

    const unsigned value = dateTimeFieldsState.dayOfMonth();
    if (range().isInRange(static_cast<int>(value))) {
        setValueAsInteger(value);
        return;
    }

    setEmptyValue();
}

// ----------------------------

DateTimeHourFieldElementBase::DateTimeHourFieldElementBase(Document& document, FieldOwner& fieldOwner, const Range& range, const Range& hardLimits, const Step& step)
    : DateTimeNumericFieldElement(document, fieldOwner, range, hardLimits, "--", step)
{
}

void DateTimeHourFieldElementBase::initialize()
{
    DEFINE_STATIC_LOCAL(AtomicString, hourPseudoId, ("-webkit-datetime-edit-hour-field", AtomicString::ConstructFromLiteral));
    DateTimeNumericFieldElement::initialize(hourPseudoId, queryString(WebLocalizedString::AXHourFieldText));
}

void DateTimeHourFieldElementBase::setValueAsDate(const DateComponents& date)
{
    setValueAsInteger(date.hour());
}

void DateTimeHourFieldElementBase::setValueAsDateTimeFieldsState(const DateTimeFieldsState& dateTimeFieldsState)
{
    if (!dateTimeFieldsState.hasHour()) {
        setEmptyValue();
        return;
    }

    const int hour12 = dateTimeFieldsState.hour();
    if (hour12 < 1 || hour12 > 12) {
        setEmptyValue();
        return;
    }

    const int hour11 = hour12 == 12 ? 0 : hour12;
    const int hour23 = dateTimeFieldsState.ampm() == DateTimeFieldsState::AMPMValuePM ? hour11 + 12 : hour11;
    setValueAsInteger(hour23);
}
// ----------------------------

DateTimeHour11FieldElement::DateTimeHour11FieldElement(Document& document, FieldOwner& fieldOwner, const Range& range, const Step& step)
    : DateTimeHourFieldElementBase(document, fieldOwner, range, Range(0, 11), step)
{
}

PassRefPtrWillBeRawPtr<DateTimeHour11FieldElement> DateTimeHour11FieldElement::create(Document& document, FieldOwner& fieldOwner, const Range& hour23Range, const Step& step)
{
    ASSERT(hour23Range.minimum >= 0);
    ASSERT(hour23Range.maximum <= 23);
    ASSERT(hour23Range.minimum <= hour23Range.maximum);
    Range range(0, 11);
    if (hour23Range.maximum < 12)
        range = hour23Range;
    else if (hour23Range.minimum >= 12) {
        range.minimum = hour23Range.minimum - 12;
        range.maximum = hour23Range.maximum - 12;
    }

    RefPtrWillBeRawPtr<DateTimeHour11FieldElement> field = adoptRefWillBeNoop(new DateTimeHour11FieldElement(document, fieldOwner, range, step));
    field->initialize();
    return field.release();
}

void DateTimeHour11FieldElement::populateDateTimeFieldsState(DateTimeFieldsState& dateTimeFieldsState)
{
    if (!hasValue()) {
        dateTimeFieldsState.setHour(DateTimeFieldsState::emptyValue);
        return;
    }
    const int value = valueAsInteger();
    dateTimeFieldsState.setHour(value ? value : 12);
}

void DateTimeHour11FieldElement::setValueAsInteger(int value, EventBehavior eventBehavior)
{
    value = Range(0, 23).clampValue(value) % 12;
    DateTimeNumericFieldElement::setValueAsInteger(value, eventBehavior);
}

// ----------------------------

DateTimeHour12FieldElement::DateTimeHour12FieldElement(Document& document, FieldOwner& fieldOwner, const Range& range, const Step& step)
    : DateTimeHourFieldElementBase(document, fieldOwner, range, Range(1, 12), step)
{
}

PassRefPtrWillBeRawPtr<DateTimeHour12FieldElement> DateTimeHour12FieldElement::create(Document& document, FieldOwner& fieldOwner, const Range& hour23Range, const Step& step)
{
    ASSERT(hour23Range.minimum >= 0);
    ASSERT(hour23Range.maximum <= 23);
    ASSERT(hour23Range.minimum <= hour23Range.maximum);
    Range range(1, 12);
    if (hour23Range.maximum < 12)
        range = hour23Range;
    else if (hour23Range.minimum >= 12) {
        range.minimum = hour23Range.minimum - 12;
        range.maximum = hour23Range.maximum - 12;
    }
    if (!range.minimum)
        range.minimum = 12;
    if (!range.maximum)
        range.maximum = 12;
    if (range.minimum > range.maximum) {
        range.minimum = 1;
        range.maximum = 12;
    }
    RefPtrWillBeRawPtr<DateTimeHour12FieldElement> field = adoptRefWillBeNoop(new DateTimeHour12FieldElement(document, fieldOwner, range, step));
    field->initialize();
    return field.release();
}

void DateTimeHour12FieldElement::populateDateTimeFieldsState(DateTimeFieldsState& dateTimeFieldsState)
{
    dateTimeFieldsState.setHour(hasValue() ? valueAsInteger() : DateTimeFieldsState::emptyValue);
}

void DateTimeHour12FieldElement::setValueAsInteger(int value, EventBehavior eventBehavior)
{
    value = Range(0, 24).clampValue(value) % 12;
    DateTimeNumericFieldElement::setValueAsInteger(value ? value : 12, eventBehavior);
}

// ----------------------------

DateTimeHour23FieldElement::DateTimeHour23FieldElement(Document& document, FieldOwner& fieldOwner, const Range& range, const Step& step)
    : DateTimeHourFieldElementBase(document, fieldOwner, range, Range(0, 23), step)
{
}

PassRefPtrWillBeRawPtr<DateTimeHour23FieldElement> DateTimeHour23FieldElement::create(Document& document, FieldOwner& fieldOwner, const Range& hour23Range, const Step& step)
{
    ASSERT(hour23Range.minimum >= 0);
    ASSERT(hour23Range.maximum <= 23);
    ASSERT(hour23Range.minimum <= hour23Range.maximum);
    RefPtrWillBeRawPtr<DateTimeHour23FieldElement> field = adoptRefWillBeNoop(new DateTimeHour23FieldElement(document, fieldOwner, hour23Range, step));
    field->initialize();
    return field.release();
}

void DateTimeHour23FieldElement::populateDateTimeFieldsState(DateTimeFieldsState& dateTimeFieldsState)
{
    if (!hasValue()) {
        dateTimeFieldsState.setHour(DateTimeFieldsState::emptyValue);
        return;
    }

    const int value = valueAsInteger();

    dateTimeFieldsState.setHour(value % 12 ? value % 12 : 12);
    dateTimeFieldsState.setAMPM(value >= 12 ? DateTimeFieldsState::AMPMValuePM : DateTimeFieldsState::AMPMValueAM);
}

void DateTimeHour23FieldElement::setValueAsInteger(int value, EventBehavior eventBehavior)
{
    value = Range(0, 23).clampValue(value);
    DateTimeNumericFieldElement::setValueAsInteger(value, eventBehavior);
}

// ----------------------------

DateTimeHour24FieldElement::DateTimeHour24FieldElement(Document& document, FieldOwner& fieldOwner, const Range& range, const Step& step)
    : DateTimeHourFieldElementBase(document, fieldOwner, range, Range(1, 24), step)
{
}

PassRefPtrWillBeRawPtr<DateTimeHour24FieldElement> DateTimeHour24FieldElement::create(Document& document, FieldOwner& fieldOwner, const Range& hour23Range, const Step& step)
{
    ASSERT(hour23Range.minimum >= 0);
    ASSERT(hour23Range.maximum <= 23);
    ASSERT(hour23Range.minimum <= hour23Range.maximum);
    Range range(hour23Range.minimum ? hour23Range.minimum : 24, hour23Range.maximum ? hour23Range.maximum : 24);
    if (range.minimum > range.maximum) {
        range.minimum = 1;
        range.maximum = 24;
    }

    RefPtrWillBeRawPtr<DateTimeHour24FieldElement> field = adoptRefWillBeNoop(new DateTimeHour24FieldElement(document, fieldOwner, range, step));
    field->initialize();
    return field.release();
}

void DateTimeHour24FieldElement::populateDateTimeFieldsState(DateTimeFieldsState& dateTimeFieldsState)
{
    if (!hasValue()) {
        dateTimeFieldsState.setHour(DateTimeFieldsState::emptyValue);
        return;
    }

    const int value = valueAsInteger();

    if (value == 24) {
        dateTimeFieldsState.setHour(12);
        dateTimeFieldsState.setAMPM(DateTimeFieldsState::AMPMValueAM);
    } else {
        dateTimeFieldsState.setHour(value == 12 ? 12 : value % 12);
        dateTimeFieldsState.setAMPM(value >= 12 ? DateTimeFieldsState::AMPMValuePM : DateTimeFieldsState::AMPMValueAM);
    }
}

void DateTimeHour24FieldElement::setValueAsInteger(int value, EventBehavior eventBehavior)
{
    value = Range(0, 24).clampValue(value);
    DateTimeNumericFieldElement::setValueAsInteger(value ? value : 24, eventBehavior);
}

// ----------------------------

DateTimeMillisecondFieldElement::DateTimeMillisecondFieldElement(Document& document, FieldOwner& fieldOwner, const Range& range, const Step& step)
    : DateTimeNumericFieldElement(document, fieldOwner, range, Range(0, 999), "---", step)
{
}

PassRefPtrWillBeRawPtr<DateTimeMillisecondFieldElement> DateTimeMillisecondFieldElement::create(Document& document, FieldOwner& fieldOwner, const Range& range, const Step& step)
{
    DEFINE_STATIC_LOCAL(AtomicString, millisecondPseudoId, ("-webkit-datetime-edit-millisecond-field", AtomicString::ConstructFromLiteral));
    RefPtrWillBeRawPtr<DateTimeMillisecondFieldElement> field = adoptRefWillBeNoop(new DateTimeMillisecondFieldElement(document, fieldOwner, range, step));
    field->initialize(millisecondPseudoId, queryString(WebLocalizedString::AXMillisecondFieldText));
    return field.release();
}

void DateTimeMillisecondFieldElement::populateDateTimeFieldsState(DateTimeFieldsState& dateTimeFieldsState)
{
    dateTimeFieldsState.setMillisecond(hasValue() ? valueAsInteger() : DateTimeFieldsState::emptyValue);
}

void DateTimeMillisecondFieldElement::setValueAsDate(const DateComponents& date)
{
    setValueAsInteger(date.millisecond());
}

void DateTimeMillisecondFieldElement::setValueAsDateTimeFieldsState(const DateTimeFieldsState& dateTimeFieldsState)
{
    if (!dateTimeFieldsState.hasMillisecond()) {
        setEmptyValue();
        return;
    }

    const unsigned value = dateTimeFieldsState.millisecond();
    if (value > static_cast<unsigned>(maximum())) {
        setEmptyValue();
        return;
    }

    setValueAsInteger(value);
}

// ----------------------------

DateTimeMinuteFieldElement::DateTimeMinuteFieldElement(Document& document, FieldOwner& fieldOwner, const Range& range, const Step& step)
    : DateTimeNumericFieldElement(document, fieldOwner, range, Range(0, 59), "--", step)
{
}

PassRefPtrWillBeRawPtr<DateTimeMinuteFieldElement> DateTimeMinuteFieldElement::create(Document& document, FieldOwner& fieldOwner, const Range& range, const Step& step)
{
    DEFINE_STATIC_LOCAL(AtomicString, minutePseudoId, ("-webkit-datetime-edit-minute-field", AtomicString::ConstructFromLiteral));
    RefPtrWillBeRawPtr<DateTimeMinuteFieldElement> field = adoptRefWillBeNoop(new DateTimeMinuteFieldElement(document, fieldOwner, range, step));
    field->initialize(minutePseudoId, queryString(WebLocalizedString::AXMinuteFieldText));
    return field.release();
}

void DateTimeMinuteFieldElement::populateDateTimeFieldsState(DateTimeFieldsState& dateTimeFieldsState)
{
    dateTimeFieldsState.setMinute(hasValue() ? valueAsInteger() : DateTimeFieldsState::emptyValue);
}

void DateTimeMinuteFieldElement::setValueAsDate(const DateComponents& date)
{
    setValueAsInteger(date.minute());
}

void DateTimeMinuteFieldElement::setValueAsDateTimeFieldsState(const DateTimeFieldsState& dateTimeFieldsState)
{
    if (!dateTimeFieldsState.hasMinute()) {
        setEmptyValue();
        return;
    }

    const unsigned value = dateTimeFieldsState.minute();
    if (value > static_cast<unsigned>(maximum())) {
        setEmptyValue();
        return;
    }

    setValueAsInteger(value);
}

// ----------------------------

DateTimeMonthFieldElement::DateTimeMonthFieldElement(Document& document, FieldOwner& fieldOwner, const String& placeholder, const Range& range)
    : DateTimeNumericFieldElement(document, fieldOwner, range, Range(1, 12), placeholder)
{
}

PassRefPtrWillBeRawPtr<DateTimeMonthFieldElement> DateTimeMonthFieldElement::create(Document& document, FieldOwner& fieldOwner, const String& placeholder, const Range& range)
{
    DEFINE_STATIC_LOCAL(AtomicString, monthPseudoId, ("-webkit-datetime-edit-month-field", AtomicString::ConstructFromLiteral));
    RefPtrWillBeRawPtr<DateTimeMonthFieldElement> field = adoptRefWillBeNoop(new DateTimeMonthFieldElement(document, fieldOwner, placeholder.isEmpty() ? "--" : placeholder, range));
    field->initialize(monthPseudoId, queryString(WebLocalizedString::AXMonthFieldText));
    return field.release();
}

void DateTimeMonthFieldElement::populateDateTimeFieldsState(DateTimeFieldsState& dateTimeFieldsState)
{
    dateTimeFieldsState.setMonth(hasValue() ? valueAsInteger() : DateTimeFieldsState::emptyValue);
}

void DateTimeMonthFieldElement::setValueAsDate(const DateComponents& date)
{
    setValueAsInteger(date.month() + 1);
}

void DateTimeMonthFieldElement::setValueAsDateTimeFieldsState(const DateTimeFieldsState& dateTimeFieldsState)
{
    if (!dateTimeFieldsState.hasMonth()) {
        setEmptyValue();
        return;
    }

    const unsigned value = dateTimeFieldsState.month();
    if (range().isInRange(static_cast<int>(value))) {
        setValueAsInteger(value);
        return;
    }

    setEmptyValue();
}

// ----------------------------

DateTimeSecondFieldElement::DateTimeSecondFieldElement(Document& document, FieldOwner& fieldOwner, const Range& range, const Step& step)
    : DateTimeNumericFieldElement(document, fieldOwner, range, Range(0, 59), "--", step)
{
}

PassRefPtrWillBeRawPtr<DateTimeSecondFieldElement> DateTimeSecondFieldElement::create(Document& document, FieldOwner& fieldOwner, const Range& range, const Step& step)
{
    DEFINE_STATIC_LOCAL(AtomicString, secondPseudoId, ("-webkit-datetime-edit-second-field", AtomicString::ConstructFromLiteral));
    RefPtrWillBeRawPtr<DateTimeSecondFieldElement> field = adoptRefWillBeNoop(new DateTimeSecondFieldElement(document, fieldOwner, range, step));
    field->initialize(secondPseudoId, queryString(WebLocalizedString::AXSecondFieldText));
    return field.release();
}

void DateTimeSecondFieldElement::populateDateTimeFieldsState(DateTimeFieldsState& dateTimeFieldsState)
{
    dateTimeFieldsState.setSecond(hasValue() ? valueAsInteger() : DateTimeFieldsState::emptyValue);
}

void DateTimeSecondFieldElement::setValueAsDate(const DateComponents& date)
{
    setValueAsInteger(date.second());
}

void DateTimeSecondFieldElement::setValueAsDateTimeFieldsState(const DateTimeFieldsState& dateTimeFieldsState)
{
    if (!dateTimeFieldsState.hasSecond()) {
        setEmptyValue();
        return;
    }

    const unsigned value = dateTimeFieldsState.second();
    if (value > static_cast<unsigned>(maximum())) {
        setEmptyValue();
        return;
    }

    setValueAsInteger(value);
}

// ----------------------------

DateTimeSymbolicMonthFieldElement::DateTimeSymbolicMonthFieldElement(Document& document, FieldOwner& fieldOwner, const Vector<String>& labels, int minimum, int maximum)
    : DateTimeSymbolicFieldElement(document, fieldOwner, labels, minimum, maximum)
{
}

PassRefPtrWillBeRawPtr<DateTimeSymbolicMonthFieldElement> DateTimeSymbolicMonthFieldElement::create(Document& document, FieldOwner& fieldOwner, const Vector<String>& labels, int minimum, int maximum)
{
    DEFINE_STATIC_LOCAL(AtomicString, monthPseudoId, ("-webkit-datetime-edit-month-field", AtomicString::ConstructFromLiteral));
    RefPtrWillBeRawPtr<DateTimeSymbolicMonthFieldElement> field = adoptRefWillBeNoop(new DateTimeSymbolicMonthFieldElement(document, fieldOwner, labels, minimum, maximum));
    field->initialize(monthPseudoId, queryString(WebLocalizedString::AXMonthFieldText));
    return field.release();
}

void DateTimeSymbolicMonthFieldElement::populateDateTimeFieldsState(DateTimeFieldsState& dateTimeFieldsState)
{
    if (!hasValue())
        dateTimeFieldsState.setMonth(DateTimeFieldsState::emptyValue);
    ASSERT(valueAsInteger() < static_cast<int>(symbolsSize()));
    dateTimeFieldsState.setMonth(valueAsInteger() + 1);
}

void DateTimeSymbolicMonthFieldElement::setValueAsDate(const DateComponents& date)
{
    setValueAsInteger(date.month());
}

void DateTimeSymbolicMonthFieldElement::setValueAsDateTimeFieldsState(const DateTimeFieldsState& dateTimeFieldsState)
{
    if (!dateTimeFieldsState.hasMonth()) {
        setEmptyValue();
        return;
    }

    const unsigned value = dateTimeFieldsState.month() - 1;
    if (value >= symbolsSize()) {
        setEmptyValue();
        return;
    }

    setValueAsInteger(value);
}

// ----------------------------

DateTimeWeekFieldElement::DateTimeWeekFieldElement(Document& document, FieldOwner& fieldOwner, const Range& range)
    : DateTimeNumericFieldElement(document, fieldOwner, range, Range(DateComponents::minimumWeekNumber, DateComponents::maximumWeekNumber), "--")
{
}

PassRefPtrWillBeRawPtr<DateTimeWeekFieldElement> DateTimeWeekFieldElement::create(Document& document, FieldOwner& fieldOwner, const Range& range)
{
    DEFINE_STATIC_LOCAL(AtomicString, weekPseudoId, ("-webkit-datetime-edit-week-field", AtomicString::ConstructFromLiteral));
    RefPtrWillBeRawPtr<DateTimeWeekFieldElement> field = adoptRefWillBeNoop(new DateTimeWeekFieldElement(document, fieldOwner, range));
    field->initialize(weekPseudoId, queryString(WebLocalizedString::AXWeekOfYearFieldText));
    return field.release();
}

void DateTimeWeekFieldElement::populateDateTimeFieldsState(DateTimeFieldsState& dateTimeFieldsState)
{
    dateTimeFieldsState.setWeekOfYear(hasValue() ? valueAsInteger() : DateTimeFieldsState::emptyValue);
}

void DateTimeWeekFieldElement::setValueAsDate(const DateComponents& date)
{
    setValueAsInteger(date.week());
}

void DateTimeWeekFieldElement::setValueAsDateTimeFieldsState(const DateTimeFieldsState& dateTimeFieldsState)
{
    if (!dateTimeFieldsState.hasWeekOfYear()) {
        setEmptyValue();
        return;
    }

    const unsigned value = dateTimeFieldsState.weekOfYear();
    if (range().isInRange(static_cast<int>(value))) {
        setValueAsInteger(value);
        return;
    }

    setEmptyValue();
}

// ----------------------------

DateTimeYearFieldElement::DateTimeYearFieldElement(Document& document, FieldOwner& fieldOwner, const DateTimeYearFieldElement::Parameters& parameters)
    : DateTimeNumericFieldElement(document, fieldOwner, Range(parameters.minimumYear, parameters.maximumYear), Range(DateComponents::minimumYear(), DateComponents::maximumYear()), parameters.placeholder.isEmpty() ? "----" : parameters.placeholder)
    , m_minIsSpecified(parameters.minIsSpecified)
    , m_maxIsSpecified(parameters.maxIsSpecified)
{
    ASSERT(parameters.minimumYear >= DateComponents::minimumYear());
    ASSERT(parameters.maximumYear <= DateComponents::maximumYear());
}

PassRefPtrWillBeRawPtr<DateTimeYearFieldElement> DateTimeYearFieldElement::create(Document& document, FieldOwner& fieldOwner, const DateTimeYearFieldElement::Parameters& parameters)
{
    DEFINE_STATIC_LOCAL(AtomicString, yearPseudoId, ("-webkit-datetime-edit-year-field", AtomicString::ConstructFromLiteral));
    RefPtrWillBeRawPtr<DateTimeYearFieldElement> field = adoptRefWillBeNoop(new DateTimeYearFieldElement(document, fieldOwner, parameters));
    field->initialize(yearPseudoId, queryString(WebLocalizedString::AXYearFieldText));
    return field.release();
}

static int currentFullYear()
{
    DateComponents date;
    date.setMillisecondsSinceEpochForMonth(convertToLocalTime(currentTimeMS()));
    return date.fullYear();
}

int DateTimeYearFieldElement::defaultValueForStepDown() const
{
    return m_maxIsSpecified ? DateTimeNumericFieldElement::defaultValueForStepDown() : currentFullYear();
}

int DateTimeYearFieldElement::defaultValueForStepUp() const
{
    return m_minIsSpecified ? DateTimeNumericFieldElement::defaultValueForStepUp() : currentFullYear();
}

void DateTimeYearFieldElement::populateDateTimeFieldsState(DateTimeFieldsState& dateTimeFieldsState)
{
    dateTimeFieldsState.setYear(hasValue() ? valueAsInteger() : DateTimeFieldsState::emptyValue);
}

void DateTimeYearFieldElement::setValueAsDate(const DateComponents& date)
{
    setValueAsInteger(date.fullYear());
}

void DateTimeYearFieldElement::setValueAsDateTimeFieldsState(const DateTimeFieldsState& dateTimeFieldsState)
{
    if (!dateTimeFieldsState.hasYear()) {
        setEmptyValue();
        return;
    }

    const unsigned value = dateTimeFieldsState.year();
    if (range().isInRange(static_cast<int>(value))) {
        setValueAsInteger(value);
        return;
    }

    setEmptyValue();
}

} // namespace blink

#endif
