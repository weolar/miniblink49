/*
 * Copyright (C) 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef WTF_MathExtras_h
#define WTF_MathExtras_h

#include "wtf/Assertions.h"
#include "wtf/CPU.h"
#include <cmath>
#include <limits>

#if COMPILER(MSVC)
#include <stdint.h>
#endif

#if OS(OPENBSD)
#include <sys/types.h>
#include <machine/ieee.h>
#endif

const double piDouble = M_PI;
const float piFloat = static_cast<float>(M_PI);

const double piOverTwoDouble = M_PI_2;
const float piOverTwoFloat = static_cast<float>(M_PI_2);

const double piOverFourDouble = M_PI_4;
const float piOverFourFloat = static_cast<float>(M_PI_4);

const double twoPiDouble = piDouble * 2.0;
const float twoPiFloat = piFloat * 2.0f;

#if OS(ANDROID) || COMPILER(MSVC)
// ANDROID and MSVC's math.h does not currently supply log2 or log2f.
inline double log2(double num)
{
    // This constant is roughly M_LN2, which is not provided by default on Windows and Android.
    return log(num) / 0.693147180559945309417232121458176568;
}

inline float log2f(float num)
{
    // This constant is roughly M_LN2, which is not provided by default on Windows and Android.
    return logf(num) / 0.693147180559945309417232121458176568f;
}
#endif

#if COMPILER(MSVC)

// VS2013 has most of the math functions now, but we still need to work
// around various differences in behavior of Inf.

// Work around a bug in Win, where atan2(+-infinity, +-infinity) yields NaN instead of specific values.
inline double wtf_atan2(double x, double y)
{
    double posInf = std::numeric_limits<double>::infinity();
    double negInf = -std::numeric_limits<double>::infinity();
    double nan = std::numeric_limits<double>::quiet_NaN();

    double result = nan;

    if (x == posInf && y == posInf)
        result = piOverFourDouble;
    else if (x == posInf && y == negInf)
        result = 3 * piOverFourDouble;
    else if (x == negInf && y == posInf)
        result = -piOverFourDouble;
    else if (x == negInf && y == negInf)
        result = -3 * piOverFourDouble;
    else
        result = ::atan2(x, y);

    return result;
}

// Work around a bug in the Microsoft CRT, where fmod(x, +-infinity) yields NaN instead of x.
inline double wtf_fmod(double x, double y) { return (!std::isinf(x) && std::isinf(y)) ? x : fmod(x, y); }

// Work around a bug in the Microsoft CRT, where pow(NaN, 0) yields NaN instead of 1.
inline double wtf_pow(double x, double y) { return y == 0 ? 1 : pow(x, y); }

#define atan2(x, y) wtf_atan2(x, y)
#define fmod(x, y) wtf_fmod(x, y)
#define pow(x, y) wtf_pow(x, y)

#endif // COMPILER(MSVC)

inline double deg2rad(double d)  { return d * piDouble / 180.0; }
inline double rad2deg(double r)  { return r * 180.0 / piDouble; }
inline double deg2grad(double d) { return d * 400.0 / 360.0; }
inline double grad2deg(double g) { return g * 360.0 / 400.0; }
inline double turn2deg(double t) { return t * 360.0; }
inline double deg2turn(double d) { return d / 360.0; }
inline double rad2grad(double r) { return r * 200.0 / piDouble; }
inline double grad2rad(double g) { return g * piDouble / 200.0; }
inline double turn2grad(double t) { return t * 400; }
inline double grad2turn(double g) { return g / 400; }

inline float deg2rad(float d)  { return d * piFloat / 180.0f; }
inline float rad2deg(float r)  { return r * 180.0f / piFloat; }
inline float deg2grad(float d) { return d * 400.0f / 360.0f; }
inline float grad2deg(float g) { return g * 360.0f / 400.0f; }
inline float turn2deg(float t) { return t * 360.0f; }
inline float deg2turn(float d) { return d / 360.0f; }
inline float rad2grad(float r) { return r * 200.0f / piFloat; }
inline float grad2rad(float g) { return g * piFloat / 200.0f; }
inline float turn2grad(float t) { return t * 400; }
inline float grad2turn(float g) { return g / 400; }

// clampTo() is implemented by templated helper classes (to allow for partial
// template specialization) as well as several helper functions.

// This helper function can be called when we know that:
// (1) The type signednesses match so the compiler will not produce signed vs.
//     unsigned warnings
// (2) The default type promotions/conversions are sufficient to handle things
//     correctly
template<typename LimitType, typename ValueType> inline LimitType clampToDirectComparison(ValueType value, LimitType min, LimitType max)
{
    if (value >= max)
        return max;
    return (value <= min) ? min : static_cast<LimitType>(value);
}

// For any floating-point limits, or integral limits smaller than long long, we
// can cast the limits to double without losing precision; then the only cases
// where |value| can't be represented accurately as a double are the ones where
// it's outside the limit range anyway.  So doing all comparisons as doubles
// will give correct results.
//
// In some cases, we can get better performance by using
// clampToDirectComparison().  We use a templated class to switch between these
// two cases (instead of simply using a conditional within one function) in
// order to only compile the clampToDirectComparison() code for cases where it
// will actually be used; this prevents the compiler from emitting warnings
// about unsafe code (even though we wouldn't actually be executing that code).
template<bool canUseDirectComparison, typename LimitType, typename ValueType> class ClampToNonLongLongHelper;
template<typename LimitType, typename ValueType> class ClampToNonLongLongHelper<true, LimitType, ValueType> {
public:
    static inline LimitType clampTo(ValueType value, LimitType min, LimitType max)
    {
        return clampToDirectComparison(value, min, max);
    }
};

template<typename LimitType, typename ValueType> class ClampToNonLongLongHelper<false, LimitType, ValueType> {
public:
    static inline LimitType clampTo(ValueType value, LimitType min, LimitType max)
    {
        const double doubleValue = static_cast<double>(value);
        if (doubleValue >= static_cast<double>(max))
            return max;
        if (doubleValue <= static_cast<double>(min))
            return min;
        // If the limit type is integer, we might get better performance by
        // casting |value| (as opposed to |doubleValue|) to the limit type.
        return std::numeric_limits<LimitType>::is_integer ? static_cast<LimitType>(value) : static_cast<LimitType>(doubleValue);
    }
};

// The unspecialized version of this templated class handles clamping to
// anything other than [unsigned] long long int limits.  It simply uses the
// class above to toggle between the "fast" and "safe" clamp implementations.
template<typename LimitType, typename ValueType> class ClampToHelper {
public:
    static inline LimitType clampTo(ValueType value, LimitType min, LimitType max)
    {
        // We only use clampToDirectComparison() when the integerness and
        // signedness of the two types matches.
        //
        // If the integerness of the types doesn't match, then at best
        // clampToDirectComparison() won't be much more efficient than the
        // cast-everything-to-double method, since we'll need to convert to
        // floating point anyway; at worst, we risk incorrect results when
        // clamping a float to a 32-bit integral type due to potential precision
        // loss.
        //
        // If the signedness doesn't match, clampToDirectComparison() will
        // produce warnings about comparing signed vs. unsigned, which are apt
        // since negative signed values will be converted to large unsigned ones
        // and we'll get incorrect results.
        return ClampToNonLongLongHelper<std::numeric_limits<LimitType>::is_integer == std::numeric_limits<ValueType>::is_integer && std::numeric_limits<LimitType>::is_signed == std::numeric_limits<ValueType>::is_signed, LimitType, ValueType>::clampTo(value, min, max);
    }
};

// Clamping to [unsigned] long long int limits requires more care.  These may
// not be accurately representable as doubles, so instead we cast |value| to the
// limit type.  But that cast is undefined if |value| is floating point and
// outside the representable range of the limit type, so we also have to check
// for that case explicitly.
template<typename ValueType> class ClampToHelper<long long int, ValueType> {
public:
    static inline long long int clampTo(ValueType value, long long int min, long long int max)
    {
        if (!std::numeric_limits<ValueType>::is_integer) {
            if (value > 0) {
                if (static_cast<double>(value) >= static_cast<double>(std::numeric_limits<long long int>::max()))
                    return max;
            } else if (static_cast<double>(value) <= static_cast<double>(std::numeric_limits<long long int>::min())) {
                return min;
            }
        }
        // Note: If |value| were unsigned long long int, it could be larger than
        // the largest long long int, and this code would be wrong; we handle
        // this case with a separate full specialization below.
        return clampToDirectComparison(static_cast<long long int>(value), min, max);
    }
};

// This specialization handles the case where the above partial specialization
// would be potentially incorrect.
template<> class ClampToHelper<long long int, unsigned long long int> {
public:
    static inline long long int clampTo(unsigned long long int value, long long int min, long long int max)
    {
        if (max <= 0 || value >= static_cast<unsigned long long int>(max))
            return max;
        const long long int longLongValue = static_cast<long long int>(value);
        return (longLongValue <= min) ? min : longLongValue;
    }
};

// This is similar to the partial specialization that clamps to long long int,
// but because the lower-bound check is done for integer value types as well, we
// don't need a <unsigned long long int, long long int> full specialization.
template<typename ValueType> class ClampToHelper<unsigned long long int, ValueType> {
public:
    static inline unsigned long long int clampTo(ValueType value, unsigned long long int min, unsigned long long int max)
    {
        if (value <= 0)
            return min;
        if (!std::numeric_limits<ValueType>::is_integer) {
            if (static_cast<double>(value) >= static_cast<double>(std::numeric_limits<unsigned long long int>::max()))
                return max;
        }
        return clampToDirectComparison(static_cast<unsigned long long int>(value), min, max);
    }
};

template<typename T> inline T defaultMaximumForClamp() { return std::numeric_limits<T>::max(); }
// This basically reimplements C++11's std::numeric_limits<T>::lowest().
template<typename T> inline T defaultMinimumForClamp() { return std::numeric_limits<T>::min(); }
template<> inline float defaultMinimumForClamp<float>() { return -std::numeric_limits<float>::max(); }
template<> inline double defaultMinimumForClamp<double>() { return -std::numeric_limits<double>::max(); }

// And, finally, the actual function for people to call.
template<typename LimitType, typename ValueType> inline LimitType clampTo(ValueType value, LimitType min = defaultMinimumForClamp<LimitType>(), LimitType max = defaultMaximumForClamp<LimitType>())
{
    ASSERT(!std::isnan(static_cast<double>(value)));
    ASSERT(min <= max); // This also ensures |min| and |max| aren't NaN.
    return ClampToHelper<LimitType, ValueType>::clampTo(value, min, max);
}

inline bool isWithinIntRange(float x)
{
    return x > static_cast<float>(std::numeric_limits<int>::min()) && x < static_cast<float>(std::numeric_limits<int>::max());
}

static size_t greatestCommonDivisor(size_t a, size_t b)
{
    return b ? greatestCommonDivisor(b, a % b) : a;
}

inline size_t lowestCommonMultiple(size_t a, size_t b)
{
    return a && b ? a / greatestCommonDivisor(a, b) * b : 0;
}

#ifndef UINT64_C
#if COMPILER(MSVC)
#define UINT64_C(c) c ## ui64
#else
#define UINT64_C(c) c ## ull
#endif
#endif

// Calculate d % 2^{64}.
inline void doubleToInteger(double d, unsigned long long& value)
{
    if (std::isnan(d) || std::isinf(d))
        value = 0;
    else {
        // -2^{64} < fmodValue < 2^{64}.
        double fmodValue = fmod(trunc(d), std::numeric_limits<unsigned long long>::max() + 1.0);
        if (fmodValue >= 0) {
            // 0 <= fmodValue < 2^{64}.
            // 0 <= value < 2^{64}. This cast causes no loss.
            value = static_cast<unsigned long long>(fmodValue);
        } else {
            // -2^{64} < fmodValue < 0.
            // 0 < fmodValueInUnsignedLongLong < 2^{64}. This cast causes no loss.
            unsigned long long fmodValueInUnsignedLongLong = static_cast<unsigned long long>(-fmodValue);
            // -1 < (std::numeric_limits<unsigned long long>::max() - fmodValueInUnsignedLongLong) < 2^{64} - 1.
            // 0 < value < 2^{64}.
            value = std::numeric_limits<unsigned long long>::max() - fmodValueInUnsignedLongLong + 1;
        }
    }
}

namespace WTF {

inline unsigned fastLog2(unsigned i)
{
    unsigned log2 = 0;
    if (i & (i - 1))
        log2 += 1;
    if (i >> 16)
        log2 += 16, i >>= 16;
    if (i >> 8)
        log2 += 8, i >>= 8;
    if (i >> 4)
        log2 += 4, i >>= 4;
    if (i >> 2)
        log2 += 2, i >>= 2;
    if (i >> 1)
        log2 += 1;
    return log2;
}

} // namespace WTF

#endif // #ifndef WTF_MathExtras_h
