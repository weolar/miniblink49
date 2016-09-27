// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/style/SVGComputedStyle.h"

#include <gtest/gtest.h>

namespace blink {

// Ensures RefPtr values are compared by their values, not by pointers.
#define TEST_STYLE_REFPTR_VALUE_NO_DIFF(type, fieldName) \
    { \
        RefPtr<SVGComputedStyle> svg1 = SVGComputedStyle::create(); \
        RefPtr<SVGComputedStyle> svg2 = SVGComputedStyle::create(); \
        RefPtr<type> value1 = type::create(); \
        RefPtr<type> value2 = value1->copy(); \
        svg1->set##fieldName(value1); \
        svg2->set##fieldName(value2); \
        EXPECT_FALSE(svg1->diff(svg2.get()).hasDifference()); \
    }

// This is not very useful for fields directly stored by values, because they can only be
// compared by values. This macro mainly ensures that we update the comparisons and tests
// when we change some field to RefPtr in the future.
#define TEST_STYLE_VALUE_NO_DIFF(type, fieldName) \
    { \
        RefPtr<SVGComputedStyle> svg1 = SVGComputedStyle::create(); \
        RefPtr<SVGComputedStyle> svg2 = SVGComputedStyle::create(); \
        svg1->set##fieldName(SVGComputedStyle::initial##fieldName()); \
        svg2->set##fieldName(SVGComputedStyle::initial##fieldName()); \
        EXPECT_FALSE(svg1->diff(svg2.get()).hasDifference()); \
    }

TEST(SVGComputedStyleTest, StrokeStyleShouldCompareValue)
{
    TEST_STYLE_VALUE_NO_DIFF(float, StrokeOpacity);
    TEST_STYLE_VALUE_NO_DIFF(float, StrokeMiterLimit);
    TEST_STYLE_VALUE_NO_DIFF(UnzoomedLength, StrokeWidth);
    TEST_STYLE_VALUE_NO_DIFF(Length, StrokeDashOffset);
    TEST_STYLE_REFPTR_VALUE_NO_DIFF(SVGDashArray, StrokeDashArray);

    {
        RefPtr<SVGComputedStyle> svg1 = SVGComputedStyle::create();
        RefPtr<SVGComputedStyle> svg2 = SVGComputedStyle::create();
        svg1->setStrokePaint(SVGComputedStyle::initialStrokePaintType(), SVGComputedStyle::initialStrokePaintColor(), SVGComputedStyle::initialStrokePaintUri(), true, false);
        svg2->setStrokePaint(SVGComputedStyle::initialStrokePaintType(), SVGComputedStyle::initialStrokePaintColor(), SVGComputedStyle::initialStrokePaintUri(), true, false);
        EXPECT_FALSE(svg1->diff(svg2.get()).hasDifference());
    }
    {
        RefPtr<SVGComputedStyle> svg1 = SVGComputedStyle::create();
        RefPtr<SVGComputedStyle> svg2 = SVGComputedStyle::create();
        svg1->setStrokePaint(SVGComputedStyle::initialStrokePaintType(), SVGComputedStyle::initialStrokePaintColor(), SVGComputedStyle::initialStrokePaintUri(), false, true);
        svg2->setStrokePaint(SVGComputedStyle::initialStrokePaintType(), SVGComputedStyle::initialStrokePaintColor(), SVGComputedStyle::initialStrokePaintUri(), false, true);
        EXPECT_FALSE(svg1->diff(svg2.get()).hasDifference());
    }
}

TEST(SVGComputedStyleTest, MiscStyleShouldCompareValue)
{
    TEST_STYLE_VALUE_NO_DIFF(Color, FloodColor);
    TEST_STYLE_VALUE_NO_DIFF(float, FloodOpacity);
    TEST_STYLE_VALUE_NO_DIFF(Color, LightingColor);
    TEST_STYLE_VALUE_NO_DIFF(Length, BaselineShiftValue);
}

} // namespace blink
