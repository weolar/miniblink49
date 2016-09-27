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
#include "core/html/forms/WeekInputType.h"

#include "core/HTMLNames.h"
#include "core/InputTypeNames.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/forms/DateTimeFieldsState.h"
#include "platform/DateComponents.h"
#include "platform/text/PlatformLocale.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

using namespace HTMLNames;

static const int weekDefaultStepBase = -259200000; // The first day of 1970-W01.
static const int weekDefaultStep = 1;
static const int weekStepScaleFactor = 604800000;

PassRefPtrWillBeRawPtr<InputType> WeekInputType::create(HTMLInputElement& element)
{
    return adoptRefWillBeNoop(new WeekInputType(element));
}

void WeekInputType::countUsage()
{
    countUsageIfVisible(UseCounter::InputTypeWeek);
}

const AtomicString& WeekInputType::formControlType() const
{
    return InputTypeNames::week;
}

StepRange WeekInputType::createStepRange(AnyStepHandling anyStepHandling) const
{
    DEFINE_STATIC_LOCAL(const StepRange::StepDescription, stepDescription, (weekDefaultStep, weekDefaultStepBase, weekStepScaleFactor, StepRange::ParsedStepValueShouldBeInteger));

    return InputType::createStepRange(anyStepHandling, weekDefaultStepBase, Decimal::fromDouble(DateComponents::minimumWeek()), Decimal::fromDouble(DateComponents::maximumWeek()), stepDescription);
}

bool WeekInputType::parseToDateComponentsInternal(const String& string, DateComponents* out) const
{
    ASSERT(out);
    unsigned end;
    return out->parseWeek(string, 0, end) && end == string.length();
}

bool WeekInputType::setMillisecondToDateComponents(double value, DateComponents* date) const
{
    ASSERT(date);
    return date->setMillisecondsSinceEpochForWeek(value);
}


#if ENABLE(INPUT_MULTIPLE_FIELDS_UI)
String WeekInputType::formatDateTimeFieldsState(const DateTimeFieldsState& dateTimeFieldsState) const
{
    if (!dateTimeFieldsState.hasYear() || !dateTimeFieldsState.hasWeekOfYear())
        return emptyString();
    return String::format("%04u-W%02u", dateTimeFieldsState.year(), dateTimeFieldsState.weekOfYear());
}

void WeekInputType::setupLayoutParameters(DateTimeEditElement::LayoutParameters& layoutParameters, const DateComponents&) const
{
    layoutParameters.dateTimeFormat = locale().weekFormatInLDML();
    layoutParameters.fallbackDateTimeFormat = "yyyy-'W'ww";
    if (!parseToDateComponents(element().fastGetAttribute(minAttr), &layoutParameters.minimum))
        layoutParameters.minimum = DateComponents();
    if (!parseToDateComponents(element().fastGetAttribute(maxAttr), &layoutParameters.maximum))
        layoutParameters.maximum = DateComponents();
    layoutParameters.placeholderForYear = "----";
}

bool WeekInputType::isValidFormat(bool hasYear, bool hasMonth, bool hasWeek, bool hasDay, bool hasAMPM, bool hasHour, bool hasMinute, bool hasSecond) const
{
    return hasYear && hasWeek;
}
#endif

} // namespace blink
