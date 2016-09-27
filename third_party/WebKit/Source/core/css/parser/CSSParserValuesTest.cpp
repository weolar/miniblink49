/*
 * Copyright 2013, Google Inc. All rights reserved.
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
#include "core/css/parser/CSSParserValues.h"

#include "core/css/parser/CSSParserTokenRange.h"
#include "core/css/parser/CSSTokenizer.h"
#include <gtest/gtest.h>

namespace blink {

TEST(CSSParserValuesTest, InitWithEmptyString)
{
    String string;

    CSSParserString cssParserString;
    cssParserString.init(string);
    EXPECT_EQ(0u, cssParserString.length());
    EXPECT_EQ(string, String(cssParserString));
    EXPECT_TRUE(String(cssParserString).isNull());

    string = "";
    cssParserString.init(string);
    EXPECT_EQ(0u, cssParserString.length());
    EXPECT_EQ(string, String(cssParserString));
    EXPECT_FALSE(String(cssParserString).isNull());

    string.ensure16Bit();
    cssParserString.init(string);
    EXPECT_EQ(0u, cssParserString.length());
    EXPECT_EQ(string, String(cssParserString));
    EXPECT_FALSE(String(cssParserString).isNull());
}

TEST(CSSParserValuesTest, EqualIgnoringCase8BitsString)
{
    CSSParserString cssParserString;
    String string8bit("sHaDOw");
    cssParserString.init(string8bit);

    ASSERT_TRUE(cssParserString.equalIgnoringCase("shadow"));
    ASSERT_TRUE(cssParserString.equalIgnoringCase("ShaDow"));
    ASSERT_FALSE(cssParserString.equalIgnoringCase("shadow-all"));
    ASSERT_FALSE(cssParserString.equalIgnoringCase("sha"));
    ASSERT_FALSE(cssParserString.equalIgnoringCase("abCD"));
}

TEST(CSSParserValuesTest, EqualIgnoringCase16BitsString)
{
    String string16bit("sHaDOw");
    string16bit.ensure16Bit();

    CSSParserString cssParserString;
    cssParserString.init(string16bit);

    ASSERT_TRUE(cssParserString.equalIgnoringCase("shadow"));
    ASSERT_TRUE(cssParserString.equalIgnoringCase("ShaDow"));
    ASSERT_FALSE(cssParserString.equalIgnoringCase("shadow-all"));
    ASSERT_FALSE(cssParserString.equalIgnoringCase("sha"));
    ASSERT_FALSE(cssParserString.equalIgnoringCase("abCD"));
}

TEST(CSSParserValuesTest, CSSParserValuelistClear)
{
    CSSParserValueList list;
    for (int i = 0; i < 3; ++i) {
        CSSParserValue value;
        value.setFromNumber(3);
        list.addValue(value);
    }
    list.clearAndLeakValues();
    ASSERT_FALSE(list.size());
    ASSERT_FALSE(list.currentIndex());
}

TEST(CSSParserValuesTest, CSSParserTokenUrlConversion)
{
    CSSTokenizer::Scope scope("url(some) uRL('test') UrL(  \"words\" /**/ ) URl( /**/hi/**/ )");
    bool usesRemUnits;
    CSSParserValueList valueList(scope.tokenRange(), usesRemUnits);
    ASSERT_EQ(valueList.size(), 4u);
    ASSERT_EQ(valueList.valueAt(0)->unit, CSSPrimitiveValue::CSS_URI);
    EXPECT_EQ(String(valueList.valueAt(0)->string), "some");
    ASSERT_EQ(valueList.valueAt(1)->unit, CSSPrimitiveValue::CSS_URI);
    EXPECT_EQ(String(valueList.valueAt(1)->string), "test");
    ASSERT_EQ(valueList.valueAt(2)->unit, CSSPrimitiveValue::CSS_URI);
    EXPECT_EQ(String(valueList.valueAt(2)->string), "words");
    ASSERT_EQ(valueList.valueAt(3)->unit, CSSPrimitiveValue::CSS_URI);
    EXPECT_EQ(String(valueList.valueAt(3)->string), "/**/hi/**/");
}

} // namespace blink
