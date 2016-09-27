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
#include "core/html/forms/DateTimeLocalInputType.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/HTMLNames.h"
#include "core/InputTypeNames.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/forms/DateTimeFieldsState.h"
#include "platform/DateComponents.h"
#include "platform/text/PlatformLocale.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

using blink::WebLocalizedString;
using namespace HTMLNames;

static const int dateTimeLocalDefaultStep = 60;
static const int dateTimeLocalDefaultStepBase = 0;
static const int dateTimeLocalStepScaleFactor = 1000;

PassRefPtrWillBeRawPtr<InputType> DateTimeLocalInputType::create(HTMLInputElement& element)
{
    return adoptRefWillBeNoop(new DateTimeLocalInputType(element));
}

void DateTimeLocalInputType::countUsage()
{
    countUsageIfVisible(UseCounter::InputTypeDateTimeLocal);
}

const AtomicString& DateTimeLocalInputType::formControlType() const
{
    return InputTypeNames::datetime_local;
}

double DateTimeLocalInputType::valueAsDate() const
{
    // valueAsDate doesn't work for the datetime-local type according to the standard.
    return DateComponents::invalidMilliseconds();
}

void DateTimeLocalInputType::setValueAsDate(double value, ExceptionState& exceptionState) const
{
    // valueAsDate doesn't work for the datetime-local type according to the standard.
    InputType::setValueAsDate(value, exceptionState);
}

StepRange DateTimeLocalInputType::createStepRange(AnyStepHandling anyStepHandling) const
{
    DEFINE_STATIC_LOCAL(const StepRange::StepDescription, stepDescription, (dateTimeLocalDefaultStep, dateTimeLocalDefaultStepBase, dateTimeLocalStepScaleFactor, StepRange::ScaledStepValueShouldBeInteger));

    return InputType::createStepRange(anyStepHandling, dateTimeLocalDefaultStepBase, Decimal::fromDouble(DateComponents::minimumDateTime()), Decimal::fromDouble(DateComponents::maximumDateTime()), stepDescription);
}

bool DateTimeLocalInputType::parseToDateComponentsInternal(const String& string, DateComponents* out) const
{
    ASSERT(out);
    unsigned end;
    return out->parseDateTimeLocal(string, 0, end) && end == string.length();
}

bool DateTimeLocalInputType::setMillisecondToDateComponents(double value, DateComponents* date) const
{
    ASSERT(date);
    return date->setMillisecondsSinceEpochForDateTimeLocal(value);
}

String DateTimeLocalInputType::localizeValue(const String& proposedValue) const
{
    DateComponents date;
    if (!parseToDateComponents(proposedValue, &date))
        return proposedValue;

    Locale::FormatType formatType = shouldHaveSecondField(date) ? Locale::FormatTypeMedium : Locale::FormatTypeShort;
    String localized = element().locale().formatDateTime(date, formatType);
    return localized.isEmpty() ? proposedValue : localized;
}

#if ENABLE(INPUT_MULTIPLE_FIELDS_UI)
// FIXME: It is better to share code for DateTimeInputType::formatDateTimeFieldsState()
// and DateTimeInputLocalType::formatDateTimeFieldsState().
String DateTimeLocalInputType::formatDateTimeFieldsState(const DateTimeFieldsState& dateTimeFieldsState) const
{
    if (!dateTimeFieldsState.hasDayOfMonth() || !dateTimeFieldsState.hasMonth() || !dateTimeFieldsState.hasYear()
        || !dateTimeFieldsState.hasHour() || !dateTimeFieldsState.hasMinute() || !dateTimeFieldsState.hasAMPM())
        return emptyString();

    if (dateTimeFieldsState.hasMillisecond() && dateTimeFieldsState.millisecond()) {
        return String::format("%04u-%02u-%02uT%02u:%02u:%02u.%03u",
            dateTimeFieldsState.year(),
            dateTimeFieldsState.month(),
            dateTimeFieldsState.dayOfMonth(),
            dateTimeFieldsState.hour23(),
            dateTimeFieldsState.minute(),
            dateTimeFieldsState.hasSecond() ? dateTimeFieldsState.second() : 0,
            dateTimeFieldsState.millisecond());
    }

    if (dateTimeFieldsState.hasSecond() && dateTimeFieldsState.second()) {
        return String::format("%04u-%02u-%02uT%02u:%02u:%02u",
            dateTimeFieldsState.year(),
            dateTimeFieldsState.month(),
            dateTimeFieldsState.dayOfMonth(),
            dateTimeFieldsState.hour23(),
            dateTimeFieldsState.minute(),
            dateTimeFieldsState.second());
    }

    return String::format("%04u-%02u-%02uT%02u:%02u",
        dateTimeFieldsState.year(),
        dateTimeFieldsState.month(),
        dateTimeFieldsState.dayOfMonth(),
        dateTimeFieldsState.hour23(),
        dateTimeFieldsState.minute());
}

void DateTimeLocalInputType::setupLayoutParameters(DateTimeEditElement::LayoutParameters& layoutParameters, const DateComponents& date) const
{
    if (shouldHaveSecondField(date)) {
        layoutParameters.dateTimeFormat = layoutParameters.locale.dateTimeFormatWithSeconds();
        layoutParameters.fallbackDateTimeFormat = "yyyy-MM-dd'T'HH:mm:ss";
    } else {
        layoutParameters.dateTimeFormat = layoutParameters.locale.dateTimeFormatWithoutSeconds();
        layoutParameters.fallbackDateTimeFormat = "yyyy-MM-dd'T'HH:mm";
    }
    if (!parseToDateComponents(element().fastGetAttribute(minAttr), &layoutParameters.minimum))
        layoutParameters.minimum = DateComponents();
    if (!parseToDateComponents(element().fastGetAttribute(maxAttr), &layoutParameters.maximum))
        layoutParameters.maximum = DateComponents();
    layoutParameters.placeholderForDay = locale().queryString(WebLocalizedString::PlaceholderForDayOfMonthField);
    layoutParameters.placeholderForMonth = locale().queryString(WebLocalizedString::PlaceholderForMonthField);
    layoutParameters.placeholderForYear = locale().queryString(WebLocalizedString::PlaceholderForYearField);
}

bool DateTimeLocalInputType::isValidFormat(bool hasYear, bool hasMonth, bool hasWeek, bool hasDay, bool hasAMPM, bool hasHour, bool hasMinute, bool hasSecond) const
{
    return hasYear && hasMonth && hasDay && hasAMPM && hasHour && hasMinute;
}
#endif

} // namespace blink
