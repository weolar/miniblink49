/*
 * Copyright (C) 2012 Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "wtf/MathExtras.h"

#include <gtest/gtest.h>

namespace WTF {

TEST(MathExtrasTest, Lrint)
{
    EXPECT_EQ(-8, lrint(-7.5));
    EXPECT_EQ(-8, lrint(-8.5));
    EXPECT_EQ(0, lrint(-0.5));
    EXPECT_EQ(0, lrint(0.5));
    EXPECT_EQ(0, lrint(-0.5));
    EXPECT_EQ(1, lrint(1.3));
    EXPECT_EQ(2, lrint(1.7));
    EXPECT_EQ(0, lrint(0));
    EXPECT_EQ(0, lrint(-0));
    if (sizeof(long int) == 8) {
        // Largest double number with 0.5 precision and one halfway rounding case below.
        EXPECT_EQ(pow(2.0, 52), lrint(pow(2.0, 52) - 0.5));
        EXPECT_EQ(pow(2.0, 52) - 2, lrint(pow(2.0, 52) - 1.5));
        // Smallest double number with 0.5 precision and one halfway rounding case above.
        EXPECT_EQ(-pow(2.0, 52), lrint(-pow(2.0, 52) + 0.5));
        EXPECT_EQ(-pow(2.0, 52) + 2, lrint(-pow(2.0, 52) + 1.5));
    }
}

TEST(MathExtrasTest, clampToIntLong)
{
    if (sizeof(long) == sizeof(int))
        return;

    long maxInt = std::numeric_limits<int>::max();
    long minInt = std::numeric_limits<int>::min();
    long overflowInt = maxInt + 1;
    long underflowInt = minInt - 1;

    EXPECT_GT(overflowInt, maxInt);
    EXPECT_LT(underflowInt, minInt);

    EXPECT_EQ(maxInt, clampTo<int>(maxInt));
    EXPECT_EQ(minInt, clampTo<int>(minInt));

    EXPECT_EQ(maxInt, clampTo<int>(overflowInt));
    EXPECT_EQ(minInt, clampTo<int>(underflowInt));
}

TEST(MathExtrasTest, clampToIntLongLong)
{
    long long maxInt = std::numeric_limits<int>::max();
    long long minInt = std::numeric_limits<int>::min();
    long long overflowInt = maxInt + 1;
    long long underflowInt = minInt - 1;

    EXPECT_GT(overflowInt, maxInt);
    EXPECT_LT(underflowInt, minInt);

    EXPECT_EQ(maxInt, clampTo<int>(maxInt));
    EXPECT_EQ(minInt, clampTo<int>(minInt));

    EXPECT_EQ(maxInt, clampTo<int>(overflowInt));
    EXPECT_EQ(minInt, clampTo<int>(underflowInt));
}

TEST(MathExtrasTest, clampToIntFloat)
{
    float maxInt = static_cast<float>(std::numeric_limits<int>::max());
    float minInt = static_cast<float>(std::numeric_limits<int>::min());
    float overflowInt = maxInt * 1.1f;
    float underflowInt = minInt * 1.1f;

    EXPECT_GT(overflowInt, maxInt);
    EXPECT_LT(underflowInt, minInt);

    EXPECT_EQ(maxInt, clampTo<int>(maxInt));
    EXPECT_EQ(minInt, clampTo<int>(minInt));

    EXPECT_EQ(maxInt, clampTo<int>(overflowInt));
    EXPECT_EQ(minInt, clampTo<int>(underflowInt));

    // This value and the value one greater are typically represented the same
    // way when stored in a 32-bit float.  Make sure clamping does not cause us
    // to erroneously jump to the larger value.
    int nearFloatPrecisionLimit = 2147483520;
    EXPECT_EQ(nearFloatPrecisionLimit, clampTo<int>(static_cast<float>(nearFloatPrecisionLimit), 0, nearFloatPrecisionLimit + 1));
    EXPECT_EQ(-nearFloatPrecisionLimit, clampTo<int>(static_cast<float>(-nearFloatPrecisionLimit), -nearFloatPrecisionLimit - 1, 0));
}

TEST(MathExtrasTest, clampToIntDouble)
{
    int maxInt = std::numeric_limits<int>::max();
    int minInt = std::numeric_limits<int>::min();
    double almostOverflowInt = maxInt - 0.5;
    double overflowInt = maxInt + 0.5;
    double almostUnderflowInt = minInt + 0.5;
    double underflowInt = minInt - 0.5;

    EXPECT_LT(almostOverflowInt, maxInt);
    EXPECT_GT(overflowInt, maxInt);
    EXPECT_GT(almostUnderflowInt, minInt);
    EXPECT_LT(underflowInt, minInt);

    EXPECT_EQ(maxInt, clampTo<int>(static_cast<double>(maxInt)));
    EXPECT_EQ(minInt, clampTo<int>(static_cast<double>(minInt)));

    EXPECT_EQ(maxInt - 1, clampTo<int>(almostOverflowInt));
    EXPECT_EQ(maxInt, clampTo<int>(overflowInt));
    EXPECT_EQ(minInt + 1, clampTo<int>(almostUnderflowInt));
    EXPECT_EQ(minInt, clampTo<int>(underflowInt));
}

TEST(MathExtrasTest, clampToFloatDouble)
{
    double maxFloat = std::numeric_limits<float>::max();
    double minFloat = -maxFloat;
    double overflowFloat = maxFloat * 1.1;
    double underflowFloat = minFloat * 1.1;

    EXPECT_GT(overflowFloat, maxFloat);
    EXPECT_LT(underflowFloat, minFloat);

    EXPECT_EQ(maxFloat, clampTo<float>(maxFloat));
    EXPECT_EQ(minFloat, clampTo<float>(minFloat));

    EXPECT_EQ(maxFloat, clampTo<float>(overflowFloat));
    EXPECT_EQ(minFloat, clampTo<float>(underflowFloat));

    EXPECT_EQ(maxFloat, clampTo<float>(std::numeric_limits<float>::infinity()));
    EXPECT_EQ(minFloat, clampTo<float>(-std::numeric_limits<float>::infinity()));
}

TEST(MathExtrasTest, clampToDouble)
{
    EXPECT_EQ(0.0, clampTo<double>(0));
    EXPECT_EQ(0.0, clampTo<double>(0.0f));
    EXPECT_EQ(0.0, clampTo<double>(0ULL));
    EXPECT_EQ(3.5, clampTo<double>(std::numeric_limits<unsigned long long>::max(), 0.0, 3.5));
}

TEST(MathExtrasText, clampToLongLongDouble)
{
    double overflowLL = static_cast<double>(std::numeric_limits<long long>::max()) * 2;
    EXPECT_EQ(std::numeric_limits<long long>::max(), clampTo<long long>(overflowLL));
    EXPECT_EQ(std::numeric_limits<long long>::min(), clampTo<long long>(-overflowLL));
}

TEST(MathExtrasText, clampToUnsignedLongLongDouble)
{
    double overflowULL = static_cast<double>(std::numeric_limits<unsigned long long>::max()) * 2;
    EXPECT_EQ(std::numeric_limits<unsigned long long>::max(), clampTo<unsigned long long>(overflowULL));
    EXPECT_EQ(std::numeric_limits<unsigned long long>::min(), clampTo<unsigned long long>(-overflowULL));
}

TEST(MathExtrasTest, clampToUnsignedUnsignedLong)
{
    if (sizeof(unsigned long) == sizeof(unsigned))
        return;

    unsigned long maxUnsigned = std::numeric_limits<unsigned>::max();
    unsigned long overflowUnsigned = maxUnsigned + 1;

    EXPECT_GT(overflowUnsigned, maxUnsigned);

    EXPECT_EQ(maxUnsigned, clampTo<unsigned>(maxUnsigned));

    EXPECT_EQ(maxUnsigned, clampTo<unsigned>(overflowUnsigned));
    EXPECT_EQ(0u, clampTo<unsigned>(-1));
}

TEST(MathExtrasTest, clampToUnsignedUnsignedLongLong)
{
    unsigned long long maxUnsigned = std::numeric_limits<unsigned>::max();
    unsigned long long overflowUnsigned = maxUnsigned + 1;

    EXPECT_GT(overflowUnsigned, maxUnsigned);

    EXPECT_EQ(maxUnsigned, clampTo<unsigned>(maxUnsigned));

    EXPECT_EQ(maxUnsigned, clampTo<unsigned>(overflowUnsigned));
    EXPECT_EQ(0u, clampTo<unsigned>(-1));
}

TEST(MathExtrasTest, clampToLongLongUnsignedLongLong)
{
    long long maxLongLongLL = std::numeric_limits<long long>::max();
    unsigned long long maxLongLongULL = maxLongLongLL;
    unsigned long long overflowLongLong = maxLongLongULL + 1;

    EXPECT_GT(overflowLongLong, maxLongLongULL);

    EXPECT_EQ(maxLongLongLL, clampTo<long long>(maxLongLongULL));
    EXPECT_EQ(maxLongLongLL - 1, clampTo<long long>(maxLongLongULL - 1));
    EXPECT_EQ(maxLongLongLL, clampTo<long long>(overflowLongLong));

    EXPECT_EQ(-3LL, clampTo<long long>(2ULL, -5LL, -3LL));
}

TEST(MathExtrasTest, clampToUnsignedLongLongInt)
{
    EXPECT_EQ(0ULL, clampTo<unsigned long long>(-1));
    EXPECT_EQ(0ULL, clampTo<unsigned long long>(0));
    EXPECT_EQ(1ULL, clampTo<unsigned long long>(1));
}

TEST(MathExtrasTest, clampToUnsignedLongLongUnsignedLongLong)
{
    EXPECT_EQ(0ULL, clampTo<unsigned long long>(0ULL));
    EXPECT_EQ(1ULL, clampTo<unsigned long long>(0ULL, 1ULL, 2ULL));
    EXPECT_EQ(2ULL, clampTo<unsigned long long>(3ULL, 1ULL, 2ULL));
    EXPECT_EQ(0xFFFFFFFFFFFFFFF5ULL, clampTo<unsigned long long>(0xFFFFFFFFFFFFFFF5ULL));
}

// Make sure that various +-inf cases are handled properly (they aren't
// by default on VS).
TEST(MathExtrasTest, infinityMath)
{
    double posInf = std::numeric_limits<double>::infinity();
    double negInf = -std::numeric_limits<double>::infinity();
    double nan = std::numeric_limits<double>::quiet_NaN();

    EXPECT_EQ(M_PI_4, atan2(posInf, posInf));
    EXPECT_EQ(3.0 * M_PI_4, atan2(posInf, negInf));
    EXPECT_EQ(-M_PI_4, atan2(negInf, posInf));
    EXPECT_EQ(-3.0 * M_PI_4, atan2(negInf, negInf));

    EXPECT_EQ(0.0, fmod(0.0, posInf));
    EXPECT_EQ(7.0, fmod(7.0, posInf));
    EXPECT_EQ(-7.0, fmod(-7.0, posInf));
    EXPECT_EQ(0.0, fmod(0.0, negInf));
    EXPECT_EQ(7.0, fmod(7.0, negInf));
    EXPECT_EQ(-7.0, fmod(-7.0, negInf));

    EXPECT_EQ(1.0, pow(5.0, 0.0));
    EXPECT_EQ(1.0, pow(-5.0, 0.0));
    EXPECT_EQ(1.0, pow(nan, 0.0));
}

} // namespace WTF
