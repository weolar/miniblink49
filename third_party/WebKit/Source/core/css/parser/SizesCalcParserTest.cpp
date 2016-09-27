// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/css/parser/SizesCalcParser.h"

#include "core/MediaTypeNames.h"
#include "core/css/CSSPrimitiveValue.h"
#include "core/css/MediaValuesCached.h"
#include "core/css/parser/CSSParser.h"
#include "core/css/parser/CSSTokenizer.h"

#include <gtest/gtest.h>

namespace blink {

struct TestCase {
    const char* input;
    const float output;
    const bool valid;
    const bool dontRunInCSSCalc;
};

static void initLengthArray(CSSLengthArray& lengthArray)
{
    lengthArray.resize(CSSPrimitiveValue::LengthUnitTypeCount);
    for (size_t i = 0; i < CSSPrimitiveValue::LengthUnitTypeCount; ++i)
        lengthArray.at(i) = 0;
}

static void verifyCSSCalc(String text, double value, bool valid, unsigned fontSize, unsigned viewportWidth, unsigned viewportHeight)
{
    CSSLengthArray lengthArray;
    initLengthArray(lengthArray);
    RefPtrWillBeRawPtr<CSSValue> cssValue = CSSParser::parseSingleValue(CSSPropertyLeft, text);
    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(cssValue.get());
    if (primitiveValue)
        primitiveValue->accumulateLengthArray(lengthArray);
    else
        ASSERT_EQ(valid, false);
    float length = lengthArray.at(CSSPrimitiveValue::UnitTypePixels);
    length += lengthArray.at(CSSPrimitiveValue::UnitTypeFontSize) * fontSize;
    length += lengthArray.at(CSSPrimitiveValue::UnitTypeViewportWidth) * viewportWidth / 100.0;
    length += lengthArray.at(CSSPrimitiveValue::UnitTypeViewportHeight) * viewportHeight / 100.0;
    ASSERT_EQ(value, length);
}


TEST(SizesCalcParserTest, Basic)
{
    TestCase testCases[] = {
        {"calc(500px + 10em)", 660, true, false},
        {"calc(500px / 8)", 62.5, true, false},
        {"calc(500px + 2 * 10em)", 820, true, false},
        {"calc(500px + 2*10em)", 820, true, false},
        {"calc(500px + 0.5*10em)", 580, true, false},
        {"calc(500px + (0.5*10em + 13px))", 593, true, false},
        {"calc(100vw + (0.5*10em + 13px))", 593, true, false},
        {"calc(100vh + (0.5*10em + 13px))", 736, true, false},
        {"calc(100vh + calc(0.5*10em + 13px))", 736, true, true}, // CSSCalculationValue does not parse internal "calc(".
        {"calc(100vh + (50%*10em + 13px))", 0, false, false},
        {"calc(50em+13px)", 0, false, false},
        {"calc(50em-13px)", 0, false, false},
        {"calc(500px + 10)", 0, false, false},
        {"calc(500 + 10)", 0, false, false},
        {"calc(500px + 10s)", 0, false, true}, // This test ASSERTs in CSSCalculationValue.
        {"calc(500px + 1cm)", 537.795276, true, false},
        {"calc(500px - 10s)", 0, false, true}, // This test ASSERTs in CSSCalculationValue.
        {"calc(500px - 1cm)", 462.204724, true, false},
        {"calc(500px - 1vw)", 495, true, false},
        {"calc(50px*10)", 500, true, false},
        {"calc(50px*10px)", 0, false, false},
        {"calc(50px/10px)", 0, false, false},
        {"calc(500px/10)", 50, true, false},
        {"calc(500/10)", 0, false, false},
        {"calc(500px/0.5)", 1000, true, false},
        {"calc(500px/.5)", 1000, true, false},
        {"calc(500/0)", 0, false, false},
        {"calc(500px/0)", 0, false, false},
        {"calc(-500px/10)", 0, true, true}, // CSSCalculationValue does not clamp negative values to 0.
        {"calc(((4) * ((10px))))", 40, true, false},
        {"calc(50px / 0)", 0, false, false},
        {"calc(50px / (10 + 10))", 2.5, true, false},
        {"calc(50px / (10 - 10))", 0, false, false},
        {"calc(50px / (10 * 10))", 0.5, true, false},
        {"calc(50px / (10 / 10))", 50, true, false},
        {"calc(200px*)", 0, false, false},
        {"calc(+ +200px)", 0, false, false},
        {"calc()", 0, false, false},
        {"calc(100px + + +100px)", 0, false, false},
        {"calc(200px 200px)", 0, false, false},
        {"calc(100px * * 2)", 0, false, false},
        {"calc(100px @ 2)", 0, false, false},
        {"calc(1 flim 2)", 0, false, false},
        {"calc(100px @ 2)", 0, false, false},
        {"calc(1 flim 2)", 0, false, false},
        {"calc(1 flim (2))", 0, false, false},
        {0, 0, true, false} // Do not remove the terminator line.
    };


    MediaValuesCached::MediaValuesCachedData data;
    data.viewportWidth = 500;
    data.viewportHeight = 643;
    data.deviceWidth = 500;
    data.deviceHeight = 643;
    data.devicePixelRatio = 2.0;
    data.colorBitsPerComponent = 24;
    data.monochromeBitsPerComponent = 0;
    data.primaryPointerType = PointerTypeFine;
    data.defaultFontSize = 16;
    data.threeDEnabled = true;
    data.mediaType = MediaTypeNames::screen;
    data.strictMode = true;
    data.displayMode = WebDisplayModeBrowser;
    RefPtr<MediaValues> mediaValues = MediaValuesCached::create(data);

    for (unsigned i = 0; testCases[i].input; ++i) {
        SizesCalcParser calcParser(CSSTokenizer::Scope(testCases[i].input).tokenRange(), mediaValues);
        ASSERT_EQ(testCases[i].valid, calcParser.isValid());
        if (calcParser.isValid())
            ASSERT_EQ(testCases[i].output, calcParser.result());
    }

    for (unsigned i = 0; testCases[i].input; ++i) {
        if (testCases[i].dontRunInCSSCalc)
            continue;
        verifyCSSCalc(testCases[i].input, testCases[i].output, testCases[i].valid, data.defaultFontSize, data.viewportWidth, data.viewportHeight);
    }
}

} // namespace
