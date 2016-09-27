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
#include "core/html/forms/TimeInputType.h"

#include "core/HTMLNames.h"
#include "core/InputTypeNames.h"
#include "core/dom/Document.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/forms/DateTimeFieldsState.h"
#include "core/inspector/ConsoleMessage.h"
#include "platform/DateComponents.h"
#include "platform/text/PlatformLocale.h"
#include "wtf/CurrentTime.h"
#include "wtf/DateMath.h"
#include "wtf/MathExtras.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

using namespace HTMLNames;

static const int timeDefaultStep = 60;
static const int timeDefaultStepBase = 0;
static const int timeStepScaleFactor = 1000;

TimeInputType::TimeInputType(HTMLInputElement& element)
    : BaseTimeInputType(element)
{
}

PassRefPtrWillBeRawPtr<InputType> TimeInputType::create(HTMLInputElement& element)
{
    return adoptRefWillBeNoop(new TimeInputType(element));
}

void TimeInputType::countUsage()
{
    countUsageIfVisible(UseCounter::InputTypeTime);
}

const AtomicString& TimeInputType::formControlType() const
{
    return InputTypeNames::time;
}

Decimal TimeInputType::defaultValueForStepUp() const
{
    DateComponents date;
    date.setMillisecondsSinceMidnight(convertToLocalTime(currentTimeMS()));
    double milliseconds = date.millisecondsSinceEpoch();
    ASSERT(std::isfinite(milliseconds));
    return Decimal::fromDouble(milliseconds);
}

StepRange TimeInputType::createStepRange(AnyStepHandling anyStepHandling) const
{
    DEFINE_STATIC_LOCAL(const StepRange::StepDescription, stepDescription, (timeDefaultStep, timeDefaultStepBase, timeStepScaleFactor, StepRange::ScaledStepValueShouldBeInteger));

    return InputType::createStepRange(anyStepHandling, timeDefaultStepBase, Decimal::fromDouble(DateComponents::minimumTime()), Decimal::fromDouble(DateComponents::maximumTime()), stepDescription);
}

bool TimeInputType::parseToDateComponentsInternal(const String& string, DateComponents* out) const
{
    ASSERT(out);
    unsigned end;
    return out->parseTime(string, 0, end) && end == string.length();
}

bool TimeInputType::setMillisecondToDateComponents(double value, DateComponents* date) const
{
    ASSERT(date);
    return date->setMillisecondsSinceMidnight(value);
}

void TimeInputType::warnIfValueIsInvalid(const String& value) const
{
    if (value != element().sanitizeValue(value)) {
        element().document().addConsoleMessage(ConsoleMessage::create(RenderingMessageSource, WarningMessageLevel,
            String::format("The specified value %s does not conform to the required format.  The format is \"HH:mm\", \"HH:mm:ss\" or \"HH:mm:ss.SSS\" where HH is 00-23, mm is 00-59, ss is 00-59, and SSS is 000-999.", JSONValue::quoteString(value).utf8().data())));
    }
}

String TimeInputType::localizeValue(const String& proposedValue) const
{
    DateComponents date;
    if (!parseToDateComponents(proposedValue, &date))
        return proposedValue;

    Locale::FormatType formatType = shouldHaveSecondField(date) ? Locale::FormatTypeMedium : Locale::FormatTypeShort;

    String localized = element().locale().formatDateTime(date, formatType);
    return localized.isEmpty() ? proposedValue : localized;
}

#if ENABLE(INPUT_MULTIPLE_FIELDS_UI)

String TimeInputType::formatDateTimeFieldsState(const DateTimeFieldsState& dateTimeFieldsState) const
{
    if (!dateTimeFieldsState.hasHour() || !dateTimeFieldsState.hasMinute() || !dateTimeFieldsState.hasAMPM())
        return emptyString();
    if (dateTimeFieldsState.hasMillisecond() && dateTimeFieldsState.millisecond()) {
        return String::format("%02u:%02u:%02u.%03u",
            dateTimeFieldsState.hour23(),
            dateTimeFieldsState.minute(),
            dateTimeFieldsState.hasSecond() ? dateTimeFieldsState.second() : 0,
            dateTimeFieldsState.millisecond());
    }
    if (dateTimeFieldsState.hasSecond() && dateTimeFieldsState.second()) {
        return String::format("%02u:%02u:%02u",
            dateTimeFieldsState.hour23(),
            dateTimeFieldsState.minute(),
            dateTimeFieldsState.second());
    }
    return String::format("%02u:%02u", dateTimeFieldsState.hour23(), dateTimeFieldsState.minute());
}

void TimeInputType::setupLayoutParameters(DateTimeEditElement::LayoutParameters& layoutParameters, const DateComponents& date) const
{
    if (shouldHaveSecondField(date)) {
        layoutParameters.dateTimeFormat = layoutParameters.locale.timeFormat();
        layoutParameters.fallbackDateTimeFormat = "HH:mm:ss";
    } else {
        layoutParameters.dateTimeFormat = layoutParameters.locale.shortTimeFormat();
        layoutParameters.fallbackDateTimeFormat = "HH:mm";
    }
    if (!parseToDateComponents(element().fastGetAttribute(minAttr), &layoutParameters.minimum))
        layoutParameters.minimum = DateComponents();
    if (!parseToDateComponents(element().fastGetAttribute(maxAttr), &layoutParameters.maximum))
        layoutParameters.maximum = DateComponents();
}

bool TimeInputType::isValidFormat(bool hasYear, bool hasMonth, bool hasWeek, bool hasDay, bool hasAMPM, bool hasHour, bool hasMinute, bool hasSecond) const
{
    return hasHour && hasMinute && hasAMPM;
}
#endif

} // namespace blink
