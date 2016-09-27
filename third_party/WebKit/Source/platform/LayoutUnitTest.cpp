/*
 * Copyright (c) 2012, Google Inc. All rights reserved.
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
#include "platform/LayoutUnit.h"

#include <gtest/gtest.h>
#include <limits.h>

namespace blink {

TEST(LayoutUnitTest, LayoutUnitInt)
{
    EXPECT_EQ(intMinForLayoutUnit, LayoutUnit(INT_MIN).toInt());
    EXPECT_EQ(intMinForLayoutUnit, LayoutUnit(INT_MIN / 2).toInt());
    EXPECT_EQ(intMinForLayoutUnit, LayoutUnit(intMinForLayoutUnit - 1).toInt());
    EXPECT_EQ(intMinForLayoutUnit, LayoutUnit(intMinForLayoutUnit).toInt());
    EXPECT_EQ(intMinForLayoutUnit + 1, LayoutUnit(intMinForLayoutUnit + 1).toInt());
    EXPECT_EQ(intMinForLayoutUnit / 2, LayoutUnit(intMinForLayoutUnit / 2).toInt());
    EXPECT_EQ(-10000, LayoutUnit(-10000).toInt());
    EXPECT_EQ(-1000, LayoutUnit(-1000).toInt());
    EXPECT_EQ(-100, LayoutUnit(-100).toInt());
    EXPECT_EQ(-10, LayoutUnit(-10).toInt());
    EXPECT_EQ(-1, LayoutUnit(-1).toInt());
    EXPECT_EQ(0, LayoutUnit(0).toInt());
    EXPECT_EQ(1, LayoutUnit(1).toInt());
    EXPECT_EQ(100, LayoutUnit(100).toInt());
    EXPECT_EQ(1000, LayoutUnit(1000).toInt());
    EXPECT_EQ(10000, LayoutUnit(10000).toInt());
    EXPECT_EQ(intMaxForLayoutUnit / 2, LayoutUnit(intMaxForLayoutUnit / 2).toInt());
    EXPECT_EQ(intMaxForLayoutUnit - 1, LayoutUnit(intMaxForLayoutUnit - 1).toInt());
    EXPECT_EQ(intMaxForLayoutUnit, LayoutUnit(intMaxForLayoutUnit).toInt());
    EXPECT_EQ(intMaxForLayoutUnit, LayoutUnit(intMaxForLayoutUnit + 1).toInt());
    EXPECT_EQ(intMaxForLayoutUnit, LayoutUnit(INT_MAX / 2).toInt());
    EXPECT_EQ(intMaxForLayoutUnit, LayoutUnit(INT_MAX).toInt());
}

TEST(LayoutUnitTest, LayoutUnitFloat)
{
    const float tolerance = 1.0f / kFixedPointDenominator;
    EXPECT_FLOAT_EQ(1.0f, LayoutUnit(1.0f).toFloat());
    EXPECT_FLOAT_EQ(1.25f, LayoutUnit(1.25f).toFloat());
    EXPECT_NEAR(LayoutUnit(1.1f).toFloat(), 1.1f, tolerance);
    EXPECT_NEAR(LayoutUnit(1.33f).toFloat(), 1.33f, tolerance);
    EXPECT_NEAR(LayoutUnit(1.3333f).toFloat(), 1.3333f, tolerance);
    EXPECT_NEAR(LayoutUnit(1.53434f).toFloat(), 1.53434f, tolerance);
    EXPECT_NEAR(LayoutUnit(345634).toFloat(), 345634.0f, tolerance);
    EXPECT_NEAR(LayoutUnit(345634.12335f).toFloat(), 345634.12335f, tolerance);
    EXPECT_NEAR(LayoutUnit(-345634.12335f).toFloat(), -345634.12335f, tolerance);
    EXPECT_NEAR(LayoutUnit(-345634).toFloat(), -345634.0f, tolerance);
}

TEST(LayoutUnitTest, LayoutUnitRounding)
{
    EXPECT_EQ(-2, LayoutUnit(-1.9f).round());
    EXPECT_EQ(-2, LayoutUnit(-1.6f).round());
    EXPECT_EQ(-2, LayoutUnit::fromFloatRound(-1.51f).round());
    EXPECT_EQ(-1, LayoutUnit::fromFloatRound(-1.5f).round());
    EXPECT_EQ(-1, LayoutUnit::fromFloatRound(-1.49f).round());
    EXPECT_EQ(-1, LayoutUnit(-1.0f).round());
    EXPECT_EQ(-1, LayoutUnit::fromFloatRound(-0.99f).round());
    EXPECT_EQ(-1, LayoutUnit::fromFloatRound(-0.51f).round());
    EXPECT_EQ(0, LayoutUnit::fromFloatRound(-0.50f).round());
    EXPECT_EQ(0, LayoutUnit::fromFloatRound(-0.49f).round());
    EXPECT_EQ(0, LayoutUnit(-0.1f).round());
    EXPECT_EQ(0, LayoutUnit(0.0f).round());
    EXPECT_EQ(0, LayoutUnit(0.1f).round());
    EXPECT_EQ(0, LayoutUnit::fromFloatRound(0.49f).round());
    EXPECT_EQ(1, LayoutUnit::fromFloatRound(0.50f).round());
    EXPECT_EQ(1, LayoutUnit::fromFloatRound(0.51f).round());
    EXPECT_EQ(1, LayoutUnit(0.99f).round());
    EXPECT_EQ(1, LayoutUnit(1.0f).round());
    EXPECT_EQ(1, LayoutUnit::fromFloatRound(1.49f).round());
    EXPECT_EQ(2, LayoutUnit::fromFloatRound(1.5f).round());
    EXPECT_EQ(2, LayoutUnit::fromFloatRound(1.51f).round());
}

TEST(LayoutUnitTest, LayoutUnitSnapSizeToPixel)
{
    EXPECT_EQ(1, snapSizeToPixel(LayoutUnit(1), LayoutUnit(0)));
    EXPECT_EQ(1, snapSizeToPixel(LayoutUnit(1), LayoutUnit(0.5)));
    EXPECT_EQ(2, snapSizeToPixel(LayoutUnit(1.5), LayoutUnit(0)));
    EXPECT_EQ(2, snapSizeToPixel(LayoutUnit(1.5), LayoutUnit(0.49)));
    EXPECT_EQ(1, snapSizeToPixel(LayoutUnit(1.5), LayoutUnit(0.5)));
    EXPECT_EQ(1, snapSizeToPixel(LayoutUnit(1.5), LayoutUnit(0.75)));
    EXPECT_EQ(1, snapSizeToPixel(LayoutUnit(1.5), LayoutUnit(0.99)));
    EXPECT_EQ(2, snapSizeToPixel(LayoutUnit(1.5), LayoutUnit(1)));

    EXPECT_EQ(0, snapSizeToPixel(LayoutUnit(0.5), LayoutUnit(1.5)));
    EXPECT_EQ(0, snapSizeToPixel(LayoutUnit(0.99), LayoutUnit(1.5)));
    EXPECT_EQ(1, snapSizeToPixel(LayoutUnit(1.0), LayoutUnit(1.5)));
    EXPECT_EQ(1, snapSizeToPixel(LayoutUnit(1.49), LayoutUnit(1.5)));
    EXPECT_EQ(1, snapSizeToPixel(LayoutUnit(1.5), LayoutUnit(1.5)));

    EXPECT_EQ(101, snapSizeToPixel(LayoutUnit(100.5), LayoutUnit(100)));
    EXPECT_EQ(intMaxForLayoutUnit, snapSizeToPixel(LayoutUnit(intMaxForLayoutUnit), LayoutUnit(0.3)));
    EXPECT_EQ(intMinForLayoutUnit, snapSizeToPixel(LayoutUnit(intMinForLayoutUnit), LayoutUnit(-0.3)));
}

TEST(LayoutUnitTest, LayoutUnitMultiplication)
{
    EXPECT_EQ(1, (LayoutUnit(1) * LayoutUnit(1)).toInt());
    EXPECT_EQ(2, (LayoutUnit(1) * LayoutUnit(2)).toInt());
    EXPECT_EQ(2, (LayoutUnit(2) * LayoutUnit(1)).toInt());
    EXPECT_EQ(1, (LayoutUnit(2) * LayoutUnit(0.5)).toInt());
    EXPECT_EQ(1, (LayoutUnit(0.5) * LayoutUnit(2)).toInt());
    EXPECT_EQ(100, (LayoutUnit(100) * LayoutUnit(1)).toInt());

    EXPECT_EQ(-1, (LayoutUnit(-1) * LayoutUnit(1)).toInt());
    EXPECT_EQ(-2, (LayoutUnit(-1) * LayoutUnit(2)).toInt());
    EXPECT_EQ(-2, (LayoutUnit(-2) * LayoutUnit(1)).toInt());
    EXPECT_EQ(-1, (LayoutUnit(-2) * LayoutUnit(0.5)).toInt());
    EXPECT_EQ(-1, (LayoutUnit(-0.5) * LayoutUnit(2)).toInt());
    EXPECT_EQ(-100, (LayoutUnit(-100) * LayoutUnit(1)).toInt());

    EXPECT_EQ(1, (LayoutUnit(-1) * LayoutUnit(-1)).toInt());
    EXPECT_EQ(2, (LayoutUnit(-1) * LayoutUnit(-2)).toInt());
    EXPECT_EQ(2, (LayoutUnit(-2) * LayoutUnit(-1)).toInt());
    EXPECT_EQ(1, (LayoutUnit(-2) * LayoutUnit(-0.5)).toInt());
    EXPECT_EQ(1, (LayoutUnit(-0.5) * LayoutUnit(-2)).toInt());
    EXPECT_EQ(100, (LayoutUnit(-100) * LayoutUnit(-1)).toInt());

    EXPECT_EQ(333, (LayoutUnit(100) * LayoutUnit(3.33)).round());
    EXPECT_EQ(-333, (LayoutUnit(-100) * LayoutUnit(3.33)).round());
    EXPECT_EQ(333, (LayoutUnit(-100) * LayoutUnit(-3.33)).round());

    size_t aHundredSizeT = 100;
    EXPECT_EQ(100, (LayoutUnit(aHundredSizeT) * LayoutUnit(1)).toInt());
    EXPECT_EQ(400, (aHundredSizeT * LayoutUnit(4)).toInt());
    EXPECT_EQ(400, (LayoutUnit(4) * aHundredSizeT).toInt());

    int quarterMax = intMaxForLayoutUnit / 4;
    EXPECT_EQ(quarterMax * 2, (LayoutUnit(quarterMax) * LayoutUnit(2)).toInt());
    EXPECT_EQ(quarterMax * 3, (LayoutUnit(quarterMax) * LayoutUnit(3)).toInt());
    EXPECT_EQ(quarterMax * 4, (LayoutUnit(quarterMax) * LayoutUnit(4)).toInt());
    EXPECT_EQ(intMaxForLayoutUnit, (LayoutUnit(quarterMax) * LayoutUnit(5)).toInt());

    size_t overflowIntSizeT = intMaxForLayoutUnit * 4;
    EXPECT_EQ(intMaxForLayoutUnit, (LayoutUnit(overflowIntSizeT) * LayoutUnit(2)).toInt());
    EXPECT_EQ(intMaxForLayoutUnit, (overflowIntSizeT * LayoutUnit(4)).toInt());
    EXPECT_EQ(intMaxForLayoutUnit, (LayoutUnit(4) * overflowIntSizeT).toInt());
}

TEST(LayoutUnitTest, LayoutUnitDivision)
{
    EXPECT_EQ(1, (LayoutUnit(1) / LayoutUnit(1)).toInt());
    EXPECT_EQ(0, (LayoutUnit(1) / LayoutUnit(2)).toInt());
    EXPECT_EQ(2, (LayoutUnit(2) / LayoutUnit(1)).toInt());
    EXPECT_EQ(4, (LayoutUnit(2) / LayoutUnit(0.5)).toInt());
    EXPECT_EQ(0, (LayoutUnit(0.5) / LayoutUnit(2)).toInt());
    EXPECT_EQ(10, (LayoutUnit(100) / LayoutUnit(10)).toInt());
    EXPECT_FLOAT_EQ(0.5f, (LayoutUnit(1) / LayoutUnit(2)).toFloat());
    EXPECT_FLOAT_EQ(0.25f, (LayoutUnit(0.5) / LayoutUnit(2)).toFloat());

    EXPECT_EQ(-1, (LayoutUnit(-1) / LayoutUnit(1)).toInt());
    EXPECT_EQ(0, (LayoutUnit(-1) / LayoutUnit(2)).toInt());
    EXPECT_EQ(-2, (LayoutUnit(-2) / LayoutUnit(1)).toInt());
    EXPECT_EQ(-4, (LayoutUnit(-2) / LayoutUnit(0.5)).toInt());
    EXPECT_EQ(0, (LayoutUnit(-0.5) / LayoutUnit(2)).toInt());
    EXPECT_EQ(-10, (LayoutUnit(-100) / LayoutUnit(10)).toInt());
    EXPECT_FLOAT_EQ(-0.5f, (LayoutUnit(-1) / LayoutUnit(2)).toFloat());
    EXPECT_FLOAT_EQ(-0.25f, (LayoutUnit(-0.5) / LayoutUnit(2)).toFloat());

    EXPECT_EQ(1, (LayoutUnit(-1) / LayoutUnit(-1)).toInt());
    EXPECT_EQ(0, (LayoutUnit(-1) / LayoutUnit(-2)).toInt());
    EXPECT_EQ(2, (LayoutUnit(-2) / LayoutUnit(-1)).toInt());
    EXPECT_EQ(4, (LayoutUnit(-2) / LayoutUnit(-0.5)).toInt());
    EXPECT_EQ(0, (LayoutUnit(-0.5) / LayoutUnit(-2)).toInt());
    EXPECT_EQ(10, (LayoutUnit(-100) / LayoutUnit(-10)).toInt());
    EXPECT_FLOAT_EQ(0.5f, (LayoutUnit(-1) / LayoutUnit(-2)).toFloat());
    EXPECT_FLOAT_EQ(0.25f, (LayoutUnit(-0.5) / LayoutUnit(-2)).toFloat());

    size_t aHundredSizeT = 100;
    EXPECT_EQ(50, (LayoutUnit(aHundredSizeT) / LayoutUnit(2)).toInt());
    EXPECT_EQ(25, (aHundredSizeT / LayoutUnit(4)).toInt());
    EXPECT_EQ(4, (LayoutUnit(400) / aHundredSizeT).toInt());

    EXPECT_EQ(intMaxForLayoutUnit / 2, (LayoutUnit(intMaxForLayoutUnit) / LayoutUnit(2)).toInt());
    EXPECT_EQ(intMaxForLayoutUnit, (LayoutUnit(intMaxForLayoutUnit) / LayoutUnit(0.5)).toInt());
}

TEST(LayoutUnitTest, LayoutUnitCeil)
{
    EXPECT_EQ(0, LayoutUnit(0).ceil());
    EXPECT_EQ(1, LayoutUnit(0.1).ceil());
    EXPECT_EQ(1, LayoutUnit(0.5).ceil());
    EXPECT_EQ(1, LayoutUnit(0.9).ceil());
    EXPECT_EQ(1, LayoutUnit(1.0).ceil());
    EXPECT_EQ(2, LayoutUnit(1.1).ceil());

    EXPECT_EQ(0, LayoutUnit(-0.1).ceil());
    EXPECT_EQ(0, LayoutUnit(-0.5).ceil());
    EXPECT_EQ(0, LayoutUnit(-0.9).ceil());
    EXPECT_EQ(-1, LayoutUnit(-1.0).ceil());

    EXPECT_EQ(intMaxForLayoutUnit, LayoutUnit(intMaxForLayoutUnit).ceil());
    EXPECT_EQ(intMaxForLayoutUnit, (LayoutUnit(intMaxForLayoutUnit) - LayoutUnit(0.5)).ceil());
    EXPECT_EQ(intMaxForLayoutUnit - 1, (LayoutUnit(intMaxForLayoutUnit) - LayoutUnit(1)).ceil());

    EXPECT_EQ(intMinForLayoutUnit, LayoutUnit(intMinForLayoutUnit).ceil());
}

TEST(LayoutUnitTest, LayoutUnitFloor)
{
    EXPECT_EQ(0, LayoutUnit(0).floor());
    EXPECT_EQ(0, LayoutUnit(0.1).floor());
    EXPECT_EQ(0, LayoutUnit(0.5).floor());
    EXPECT_EQ(0, LayoutUnit(0.9).floor());
    EXPECT_EQ(1, LayoutUnit(1.0).floor());
    EXPECT_EQ(1, LayoutUnit(1.1).floor());

    EXPECT_EQ(-1, LayoutUnit(-0.1).floor());
    EXPECT_EQ(-1, LayoutUnit(-0.5).floor());
    EXPECT_EQ(-1, LayoutUnit(-0.9).floor());
    EXPECT_EQ(-1, LayoutUnit(-1.0).floor());

    EXPECT_EQ(intMaxForLayoutUnit, LayoutUnit(intMaxForLayoutUnit).floor());

    EXPECT_EQ(intMinForLayoutUnit, LayoutUnit(intMinForLayoutUnit).floor());
    EXPECT_EQ(intMinForLayoutUnit, (LayoutUnit(intMinForLayoutUnit) + LayoutUnit(0.5)).floor());
    EXPECT_EQ(intMinForLayoutUnit + 1, (LayoutUnit(intMinForLayoutUnit) + LayoutUnit(1)).floor());
}

TEST(LayoutUnitTest, LayoutUnitFloatOverflow)
{
    // These should overflow to the max/min according to their sign.
    EXPECT_EQ(intMaxForLayoutUnit, LayoutUnit(176972000.0f).toInt());
    EXPECT_EQ(intMinForLayoutUnit, LayoutUnit(-176972000.0f).toInt());
    EXPECT_EQ(intMaxForLayoutUnit, LayoutUnit(176972000.0).toInt());
    EXPECT_EQ(intMinForLayoutUnit, LayoutUnit(-176972000.0).toInt());
}

} // namespace blink
