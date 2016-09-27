/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SMILTime_h
#define SMILTime_h

#include "wtf/Assertions.h"
#include "wtf/HashTraits.h"
#include "wtf/MathExtras.h"

namespace blink {

class SMILTime {
public:
    SMILTime() : m_time(0) { }
    SMILTime(double time) : m_time(time) { }

    static SMILTime unresolved() { return std::numeric_limits<double>::quiet_NaN(); }
    static SMILTime indefinite() { return std::numeric_limits<double>::infinity(); }

    double value() const { return m_time; }

    bool isFinite() const { return std::isfinite(m_time); }
    bool isIndefinite() const { return std::isinf(m_time); }
    bool isUnresolved() const { return std::isnan(m_time); }

private:
    double m_time;
};

class SMILTimeWithOrigin {
public:
    enum Origin {
        ParserOrigin,
        ScriptOrigin
    };

    SMILTimeWithOrigin()
        : m_origin(ParserOrigin)
    {
    }

    SMILTimeWithOrigin(const SMILTime& time, Origin origin)
        : m_time(time)
        , m_origin(origin)
    {
    }

    const SMILTime& time() const { return m_time; }
    bool originIsScript() const { return m_origin == ScriptOrigin; }

private:
    SMILTime m_time;
    Origin m_origin;
};

struct SMILInterval {
    SMILInterval() { }
    SMILInterval(const SMILTime& begin, const SMILTime& end) : begin(begin), end(end) { }

    SMILTime begin;
    SMILTime end;
};

inline bool operator==(const SMILTime& a, const SMILTime& b) { return (a.isUnresolved() && b.isUnresolved()) || a.value() == b.value(); }
inline bool operator!(const SMILTime& a) { return !a.isFinite() || !a.value(); }
inline bool operator!=(const SMILTime& a, const SMILTime& b) { return !operator==(a, b); }

// Ordering of SMILTimes has to follow: finite < indefinite (Inf) < unresolved (NaN). The first comparison is handled by IEEE754 but
// NaN values must be checked explicitly to guarantee that unresolved is ordered correctly too.
inline bool operator>(const SMILTime& a, const SMILTime& b) { return a.isUnresolved() || (a.value() > b.value()); }
inline bool operator<(const SMILTime& a, const SMILTime& b) { return operator>(b, a); }
inline bool operator>=(const SMILTime& a, const SMILTime& b) { return operator>(a, b) || operator==(a, b); }
inline bool operator<=(const SMILTime& a, const SMILTime& b) { return operator<(a, b) || operator==(a, b); }
inline bool operator<(const SMILTimeWithOrigin& a, const SMILTimeWithOrigin& b) { return a.time() < b.time(); }

inline SMILTime operator+(const SMILTime& a, const SMILTime& b) { return a.value() + b.value(); }
inline SMILTime operator-(const SMILTime& a, const SMILTime& b) { return a.value() - b.value(); }
// So multiplying times does not make too much sense but SMIL defines it for duration * repeatCount
SMILTime operator*(const SMILTime&, const SMILTime&);

inline bool operator!=(const SMILInterval& a, const SMILInterval& b)
{
    // Compare the "raw" values since the operator!= for SMILTime always return
    // true for non-finite times.
    return a.begin.value() != b.begin.value() || a.end.value() != b.end.value();
}

struct SMILTimeHash {
    static unsigned hash(const SMILTime& key) { return WTF::FloatHash<double>::hash(key.value()); }
    static bool equal(const SMILTime& a, const SMILTime& b) { return WTF::FloatHash<double>::equal(a.value(), b.value()); }
    static const bool safeToCompareToEmptyOrDeleted = true;
};

} // namespace blink

namespace WTF {

template<> struct DefaultHash<blink::SMILTime> {
    typedef blink::SMILTimeHash Hash;
};

template<> struct HashTraits<blink::SMILTime> : GenericHashTraits<blink::SMILTime> {
    static blink::SMILTime emptyValue() { return blink::SMILTime::unresolved(); }
    static void constructDeletedValue(blink::SMILTime& slot, bool) { slot = -std::numeric_limits<double>::infinity(); }
    static bool isDeletedValue(blink::SMILTime value) { return value == -std::numeric_limits<double>::infinity(); }
};

} // namespace WTF

#endif // SMILTime_h
