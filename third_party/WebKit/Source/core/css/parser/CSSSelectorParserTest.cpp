// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/parser/CSSSelectorParser.h"

#include "core/css/CSSSelectorList.h"
#include "core/css/parser/CSSTokenizer.h"
#include <gtest/gtest.h>

namespace blink {

typedef struct {
    const char* input;
    const int a;
    const int b;
} ANPlusBTestCase;

TEST(CSSSelectorParserTest, ValidANPlusB)
{
    ANPlusBTestCase testCases[] = {
        {"odd", 2, 1},
        {"OdD", 2, 1},
        {"even", 2, 0},
        {"EveN", 2, 0},
        {"0", 0, 0},
        {"8", 0, 8},
        {"+12", 0, 12},
        {"-14", 0, -14},

        {"0n", 0, 0},
        {"16N", 16, 0},
        {"-19n", -19, 0},
        {"+23n", 23, 0},
        {"n", 1, 0},
        {"N", 1, 0},
        {"+n", 1, 0},
        {"-n", -1, 0},
        {"-N", -1, 0},

        {"6n-3", 6, -3},
        {"-26N-33", -26, -33},
        {"n-18", 1, -18},
        {"+N-5", 1, -5},
        {"-n-7", -1, -7},

        {"0n+0", 0, 0},
        {"10n+5", 10, 5},
        {"10N +5", 10, 5},
        {"10n -5", 10, -5},
        {"N+6", 1, 6},
        {"n +6", 1, 6},
        {"+n -7", 1, -7},
        {"-N -8", -1, -8},
        {"-n+9", -1, 9},

        {"33N- 22", 33, -22},
        {"+n- 25", 1, -25},
        {"N- 46", 1, -46},
        {"n- 0", 1, 0},
        {"-N- 951", -1, -951},
        {"-n- 951", -1, -951},

        {"29N + 77", 29, 77},
        {"29n - 77", 29, -77},
        {"+n + 61", 1, 61},
        {"+N - 63", 1, -63},
        {"+n/**/- 48", 1, -48},
        {"-n + 81", -1, 81},
        {"-N - 88", -1, -88},
    };

    for (unsigned i = 0; i < WTF_ARRAY_LENGTH(testCases); ++i) {
        SCOPED_TRACE(testCases[i].input);

        std::pair<int, int> ab;
        CSSTokenizer::Scope scope(testCases[i].input);
        CSSParserTokenRange range = scope.tokenRange();
        bool passed = CSSSelectorParser::consumeANPlusB(range, ab);
        EXPECT_TRUE(passed);
        EXPECT_EQ(ab.first, testCases[i].a);
        EXPECT_EQ(ab.second, testCases[i].b);
    }
}

TEST(CSSSelectorParserTest, InvalidANPlusB)
{
    // Some of these have token range prefixes which are valid <an+b> and could
    // in theory be valid in consumeANPlusB, but this behaviour isn't needed
    // anywhere and not implemented.
    const char* testCases[] = {
        " odd",
        "+ n",
        "3m+4",
        "12n--34",
        "12n- -34",
        "12n- +34",
        "23n-+43",
        "10n 5",
        "10n + +5",
        "10n + -5",
    };

    for (unsigned i = 0; i < WTF_ARRAY_LENGTH(testCases); ++i) {
        SCOPED_TRACE(testCases[i]);

        std::pair<int, int> ab;
        CSSTokenizer::Scope scope(testCases[i]);
        CSSParserTokenRange range = scope.tokenRange();
        bool passed = CSSSelectorParser::consumeANPlusB(range, ab);
        EXPECT_FALSE(passed);
    }
}

TEST(CSSSelectorParserTest, ContentPseudoInCompound)
{
    const char* testCases[][2] = {
        { "::content", "*::content" }, // crbug.com/478969
        { ".a::content", ".a::content" },
        { "::content.a", ".a::content" },
        { "::content.a.b", ".b.a::content" },
        { ".a::content.b", ".b.a::content" },
    };

    for (unsigned i = 0; i < WTF_ARRAY_LENGTH(testCases); ++i) {
        SCOPED_TRACE(testCases[i][0]);
        CSSTokenizer::Scope scope(testCases[i][0]);
        CSSParserTokenRange range = scope.tokenRange();
        CSSSelectorList list;
        CSSSelectorParser::parseSelector(range, CSSParserContext(HTMLStandardMode, nullptr), nullAtom, nullptr, list);
        EXPECT_STREQ(testCases[i][1], list.selectorsText().ascii().data());
    }
}

} // namespace
