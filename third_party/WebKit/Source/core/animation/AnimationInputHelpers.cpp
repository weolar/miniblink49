// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/AnimationInputHelpers.h"

#include "core/css/CSSValueList.h"
#include "core/css/parser/CSSParser.h"
#include "core/css/resolver/CSSToStyleMap.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

CSSPropertyID AnimationInputHelpers::keyframeAttributeToCSSPropertyID(const String& propertyName)
{
    // Disallow prefixed properties.
    if (propertyName[0] == '-' || isASCIIUpper(propertyName[0]))
        return CSSPropertyInvalid;
    if (propertyName == "cssFloat")
        return CSSPropertyFloat;
    StringBuilder builder;
    for (size_t i = 0; i < propertyName.length(); ++i) {
        if (isASCIIUpper(propertyName[i]))
            builder.append('-');
        builder.append(propertyName[i]);
    }
    return cssPropertyID(builder.toString());
}

PassRefPtr<TimingFunction> AnimationInputHelpers::parseTimingFunction(const String& string)
{
    if (string.isEmpty())
        return nullptr;

    RefPtrWillBeRawPtr<CSSValue> value = CSSParser::parseSingleValue(CSSPropertyTransitionTimingFunction, string);
    if (!value || !value->isValueList()) {
        ASSERT(!value || value->isCSSWideKeyword());
        return nullptr;
    }
    CSSValueList* valueList = toCSSValueList(value.get());
    if (valueList->length() > 1)
        return nullptr;
    return CSSToStyleMap::mapAnimationTimingFunction(valueList->item(0), true);
}

} // namespace blink
