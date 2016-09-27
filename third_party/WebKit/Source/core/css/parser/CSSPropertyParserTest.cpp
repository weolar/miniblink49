// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/parser/CSSPropertyParser.h"

#include "core/css/CSSValueList.h"
#include "core/css/parser/CSSParser.h"

#include <gtest/gtest.h>

namespace blink {

static unsigned computeNumberOfTracks(CSSValueList* valueList)
{
    unsigned numberOfTracks = 0;
    for (auto& value : *valueList) {
        if (value->isGridLineNamesValue())
            continue;
        ++numberOfTracks;
    }
    return numberOfTracks;
}

TEST(CSSPropertyParserTest, GridTrackLimits)
{
    struct {
        const CSSPropertyID propertyID;
        const char* input;
        const size_t output;
    } testCases[] = {
        {CSSPropertyGridTemplateColumns, "repeat(999999, 20px)", 999999},
        {CSSPropertyGridTemplateRows, "repeat(999999, 20px)", 999999},
        {CSSPropertyGridTemplateColumns, "repeat(1000000, 10%)", 1000000},
        {CSSPropertyGridTemplateRows, "repeat(1000000, 10%)", 1000000},
        {CSSPropertyGridTemplateColumns, "repeat(1000000, [first] min-content [last])", 1000000},
        {CSSPropertyGridTemplateRows, "repeat(1000000, [first] min-content [last])", 1000000},
        {CSSPropertyGridTemplateColumns, "repeat(1000001, auto)", 1000000},
        {CSSPropertyGridTemplateRows, "repeat(1000001, auto)", 1000000},
        {CSSPropertyGridTemplateColumns, "repeat(400000, 2em minmax(10px, max-content) 0.5fr)", 999999},
        {CSSPropertyGridTemplateRows, "repeat(400000, 2em minmax(10px, max-content) 0.5fr)", 999999},
        {CSSPropertyGridTemplateColumns, "repeat(600000, [first] 3vh 10% 2fr [nav] 10px auto 1fr 6em [last])", 999999},
        {CSSPropertyGridTemplateRows, "repeat(600000, [first] 3vh 10% 2fr [nav] 10px auto 1fr 6em [last])", 999999},
        {CSSPropertyGridTemplateColumns, "repeat(100000000000000000000, 10% 1fr)", 1000000},
        {CSSPropertyGridTemplateRows, "repeat(100000000000000000000, 10% 1fr)", 1000000},
        {CSSPropertyGridTemplateColumns, "repeat(100000000000000000000, 10% 5em 1fr auto auto 15px min-content)", 999999},
        {CSSPropertyGridTemplateRows, "repeat(100000000000000000000, 10% 5em 1fr auto auto 15px min-content)", 999999},
    };

    for (unsigned i = 0; i < WTF_ARRAY_LENGTH(testCases); ++i) {
        RefPtrWillBeRawPtr<CSSValue> value = CSSParser::parseSingleValue(testCases[i].propertyID, testCases[i].input);
        ASSERT_TRUE(value);
        ASSERT_TRUE(value->isValueList());
        EXPECT_EQ(computeNumberOfTracks(toCSSValueList(value.get())), testCases[i].output);
    }
}

} // namespace blink
