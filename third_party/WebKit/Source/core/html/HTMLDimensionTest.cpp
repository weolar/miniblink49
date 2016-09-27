/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
#include "core/html/HTMLDimension.h"

#include "wtf/text/WTFString.h"
#include <gtest/gtest.h>

namespace blink {

// This assertion-prettify function needs to be in the blink namespace.
void PrintTo(const HTMLDimension& dimension, ::std::ostream* os)
{
    *os << "HTMLDimension => type: " << dimension.type() << ", value=" << dimension.value();
}

TEST(HTMLDimensionTest, parseListOfDimensionsEmptyString)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String(""));
    ASSERT_EQ(Vector<HTMLDimension>(), result);
}

TEST(HTMLDimensionTest, parseListOfDimensionsNoNumberAbsolute)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String(" \t"));
    ASSERT_EQ(1U, result.size());
    ASSERT_EQ(HTMLDimension(0, HTMLDimension::Relative), result[0]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsNoNumberPercent)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String(" \t%"));
    ASSERT_EQ(1U, result.size());
    ASSERT_EQ(HTMLDimension(0, HTMLDimension::Percentage), result[0]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsNoNumberRelative)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String("\t *"));
    ASSERT_EQ(1U, result.size());
    ASSERT_EQ(HTMLDimension(0, HTMLDimension::Relative), result[0]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsSingleAbsolute)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String("10"));

    ASSERT_EQ(1U, result.size());
    ASSERT_EQ(HTMLDimension(10, HTMLDimension::Absolute), result[0]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsSinglePercentageWithSpaces)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String("50  %"));

    ASSERT_EQ(1U, result.size());
    ASSERT_EQ(HTMLDimension(50, HTMLDimension::Percentage), result[0]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsSingleRelative)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String("25*"));

    ASSERT_EQ(1U, result.size());
    ASSERT_EQ(HTMLDimension(25, HTMLDimension::Relative), result[0]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsDoubleAbsolute)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String("10.054"));

    ASSERT_EQ(1U, result.size());
    ASSERT_EQ(HTMLDimension(10.054, HTMLDimension::Absolute), result[0]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsLeadingSpaceAbsolute)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String("\t \t 10"));

    ASSERT_EQ(1U, result.size());
    ASSERT_EQ(HTMLDimension(10, HTMLDimension::Absolute), result[0]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsLeadingSpaceRelative)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String(" \r25*"));

    ASSERT_EQ(1U, result.size());
    ASSERT_EQ(HTMLDimension(25, HTMLDimension::Relative), result[0]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsLeadingSpacePercentage)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String("\n 25%"));

    ASSERT_EQ(1U, result.size());
    ASSERT_EQ(HTMLDimension(25, HTMLDimension::Percentage), result[0]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsDoublePercentage)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String("10.054%"));

    ASSERT_EQ(1U, result.size());
    ASSERT_EQ(HTMLDimension(10.054, HTMLDimension::Percentage), result[0]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsDoubleRelative)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String("10.054*"));

    ASSERT_EQ(1U, result.size());
    ASSERT_EQ(HTMLDimension(10.054, HTMLDimension::Relative), result[0]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsSpacesInIntegerDoubleAbsolute)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String("1\n0 .025%"));

    ASSERT_EQ(1U, result.size());
    ASSERT_EQ(HTMLDimension(1, HTMLDimension::Absolute), result[0]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsSpacesInIntegerDoublePercent)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String("1\n0 .025%"));

    ASSERT_EQ(1U, result.size());
    ASSERT_EQ(HTMLDimension(1, HTMLDimension::Absolute), result[0]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsSpacesInIntegerDoubleRelative)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String("1\n0 .025*"));

    ASSERT_EQ(1U, result.size());
    ASSERT_EQ(HTMLDimension(1, HTMLDimension::Absolute), result[0]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsSpacesInFractionAfterDotDoublePercent)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String("10.  0 25%"));

    ASSERT_EQ(1U, result.size());
    ASSERT_EQ(HTMLDimension(10.025, HTMLDimension::Percentage), result[0]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsSpacesInFractionAfterDigitDoublePercent)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String("10.05\r25%"));

    ASSERT_EQ(1U, result.size());
    ASSERT_EQ(HTMLDimension(10.0525, HTMLDimension::Percentage), result[0]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsTrailingComma)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String("10,"));

    ASSERT_EQ(1U, result.size());
    ASSERT_EQ(HTMLDimension(10, HTMLDimension::Absolute), result[0]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsTwoDimensions)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String("10*,25 %"));

    ASSERT_EQ(2U, result.size());
    ASSERT_EQ(HTMLDimension(10, HTMLDimension::Relative), result[0]);
    ASSERT_EQ(HTMLDimension(25, HTMLDimension::Percentage), result[1]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsMultipleDimensionsWithSpaces)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String("10   *   ,\t25 , 10.05\n5%"));

    ASSERT_EQ(3U, result.size());
    ASSERT_EQ(HTMLDimension(10, HTMLDimension::Relative), result[0]);
    ASSERT_EQ(HTMLDimension(25, HTMLDimension::Absolute), result[1]);
    ASSERT_EQ(HTMLDimension(10.055, HTMLDimension::Percentage), result[2]);
}

TEST(HTMLDimensionTest, parseListOfDimensionsMultipleDimensionsWithOneEmpty)
{
    Vector<HTMLDimension> result = parseListOfDimensions(String("2*,,8.%"));

    ASSERT_EQ(3U, result.size());
    ASSERT_EQ(HTMLDimension(2, HTMLDimension::Relative), result[0]);
    ASSERT_EQ(HTMLDimension(0, HTMLDimension::Relative), result[1]);
    ASSERT_EQ(HTMLDimension(8., HTMLDimension::Percentage), result[2]);
}

} // namespace blink
