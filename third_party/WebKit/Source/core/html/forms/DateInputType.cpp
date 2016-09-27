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
#include "core/html/forms/DateInputType.h"

#include "core/HTMLNames.h"
#include "core/InputTypeNames.h"
#include "core/dom/Document.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/forms/DateTimeFieldsState.h"
#include "core/inspector/ConsoleMessage.h"
#include "platform/DateComponents.h"
#include "platform/JSONValues.h"
#include "platform/text/PlatformLocale.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

using blink::WebLocalizedString;
using namespace HTMLNames;

static const int dateDefaultStep = 1;
static const int dateDefaultStepBase = 0;
static const int dateStepScaleFactor = 86400000;

inline DateInputType::DateInputType(HTMLInputElement& element)
    : BaseDateInputType(element)
{
}

PassRefPtrWillBeRawPtr<InputType> DateInputType::create(HTMLInputElement& element)
{
    return adoptRefWillBeNoop(new DateInputType(element));
}

void DateInputType::countUsage()
{
    countUsageIfVisible(UseCounter::InputTypeDate);
}

const AtomicString& DateInputType::formControlType() const
{
    return InputTypeNames::date;
}

StepRange DateInputType::createStepRange(AnyStepHandling anyStepHandling) const
{
    DEFINE_STATIC_LOCAL(const StepRange::StepDescription, stepDescription, (dateDefaultStep, dateDefaultStepBase, dateStepScaleFactor, StepRange::ParsedStepValueShouldBeInteger));

    return InputType::createStepRange(anyStepHandling, dateDefaultStepBase, Decimal::fromDouble(DateComponents::minimumDate()), Decimal::fromDouble(DateComponents::maximumDate()), stepDescription);
}

bool DateInputType::parseToDateComponentsInternal(const String& string, DateComponents* out) const
{
    ASSERT(out);
    unsigned end;
    return out->parseDate(string, 0, end) && end == string.length();
}

bool DateInputType::setMillisecondToDateComponents(double value, DateComponents* date) const
{
    ASSERT(date);
    return date->setMillisecondsSinceEpochForDate(value);
}

void DateInputType::warnIfValueIsInvalid(const String& value) const
{
    if (value != element().sanitizeValue(value)) {
        element().document().addConsoleMessage(ConsoleMessage::create(RenderingMessageSource, WarningMessageLevel,
            String::format("The specified value %s does not conform to the required format, \"yyyy-MM-dd\".", JSONValue::quoteString(value).utf8().data())));
    }
}

#if ENABLE(INPUT_MULTIPLE_FIELDS_UI)
String DateInputType::formatDateTimeFieldsState(const DateTimeFieldsState& dateTimeFieldsState) const
{
    if (!dateTimeFieldsState.hasDayOfMonth() || !dateTimeFieldsState.hasMonth() || !dateTimeFieldsState.hasYear())
        return emptyString();

    return String::format("%04u-%02u-%02u", dateTimeFieldsState.year(), dateTimeFieldsState.month(), dateTimeFieldsState.dayOfMonth());
}

void DateInputType::setupLayoutParameters(DateTimeEditElement::LayoutParameters& layoutParameters, const DateComponents& date) const
{
    layoutParameters.dateTimeFormat = layoutParameters.locale.dateFormat();
    layoutParameters.fallbackDateTimeFormat = "yyyy-MM-dd";
    if (!parseToDateComponents(element().fastGetAttribute(minAttr), &layoutParameters.minimum))
        layoutParameters.minimum = DateComponents();
    if (!parseToDateComponents(element().fastGetAttribute(maxAttr), &layoutParameters.maximum))
        layoutParameters.maximum = DateComponents();
    layoutParameters.placeholderForDay = locale().queryString(WebLocalizedString::PlaceholderForDayOfMonthField);
    layoutParameters.placeholderForMonth = locale().queryString(WebLocalizedString::PlaceholderForMonthField);
    layoutParameters.placeholderForYear = locale().queryString(WebLocalizedString::PlaceholderForYearField);
}

bool DateInputType::isValidFormat(bool hasYear, bool hasMonth, bool hasWeek, bool hasDay, bool hasAMPM, bool hasHour, bool hasMinute, bool hasSecond) const
{
    return hasYear && hasMonth && hasDay;
}
#endif

} // namespace blink
