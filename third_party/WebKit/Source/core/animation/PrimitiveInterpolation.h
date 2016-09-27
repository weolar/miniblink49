// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PrimitiveInterpolation_h
#define PrimitiveInterpolation_h

#include "core/animation/InterpolationValue.h"
#include "platform/heap/Handle.h"
#include "wtf/Vector.h"
#include <cmath>

namespace blink {

class StyleResolverState;

// Represents a conversion from a pair of keyframes to something compatible with interpolation.
// This is agnostic to whether the keyframes are compatible with each other or not.
class PrimitiveInterpolation : public NoBaseWillBeGarbageCollectedFinalized<PrimitiveInterpolation> {
public:
    virtual ~PrimitiveInterpolation() { }

    virtual void interpolate(double fraction, OwnPtrWillBeMember<InterpolationValue>& result) const = 0;

    DEFINE_INLINE_VIRTUAL_TRACE() { }
};

// Represents a pair of keyframes that are compatible for "smooth" interpolation eg. "0px" and "100px".
class PairwisePrimitiveInterpolation : public PrimitiveInterpolation {
public:
    virtual ~PairwisePrimitiveInterpolation() { }

    static PassOwnPtrWillBeRawPtr<PairwisePrimitiveInterpolation> create(const InterpolationType& type, PassOwnPtrWillBeRawPtr<InterpolableValue> start, PassOwnPtrWillBeRawPtr<InterpolableValue> end, PassRefPtrWillBeRawPtr<NonInterpolableValue> nonInterpolableValue)
    {
        return adoptPtrWillBeNoop(new PairwisePrimitiveInterpolation(type, start, end, nonInterpolableValue));
    }

    PassOwnPtrWillBeRawPtr<InterpolationValue> initialValue() const
    {
        return InterpolationValue::create(m_type, m_start->clone(), m_nonInterpolableValue);
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_start);
        visitor->trace(m_end);
        visitor->trace(m_nonInterpolableValue);
        PrimitiveInterpolation::trace(visitor);
    }

private:
    PairwisePrimitiveInterpolation(const InterpolationType& type, PassOwnPtrWillBeRawPtr<InterpolableValue> start, PassOwnPtrWillBeRawPtr<InterpolableValue> end, PassRefPtrWillBeRawPtr<NonInterpolableValue> nonInterpolableValue)
        : m_type(type)
        , m_start(start)
        , m_end(end)
        , m_nonInterpolableValue(nonInterpolableValue)
    { }

    virtual void interpolate(double fraction, OwnPtrWillBeMember<InterpolationValue>& result) const override final
    {
        ASSERT(result);
        ASSERT(&result->type() == &m_type);
        ASSERT(result->nonInterpolableValue() == m_nonInterpolableValue.get());
        m_start->interpolate(*m_end, fraction, result->interpolableValue());
    }

    const InterpolationType& m_type;
    OwnPtrWillBeMember<InterpolableValue> m_start;
    OwnPtrWillBeMember<InterpolableValue> m_end;
    RefPtrWillBeMember<NonInterpolableValue> m_nonInterpolableValue;
};

// Represents a pair of incompatible keyframes that fall back to 50% flip behaviour eg. "auto" and "0px".
class FlipPrimitiveInterpolation : public PrimitiveInterpolation {
public:
    virtual ~FlipPrimitiveInterpolation() { }

    static PassOwnPtrWillBeRawPtr<FlipPrimitiveInterpolation> create(PassOwnPtrWillBeRawPtr<InterpolationValue> start, PassOwnPtrWillBeRawPtr<InterpolationValue> end)
    {
        return adoptPtrWillBeNoop(new FlipPrimitiveInterpolation(start, end));
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_start);
        visitor->trace(m_end);
        PrimitiveInterpolation::trace(visitor);
    }

private:
    FlipPrimitiveInterpolation(PassOwnPtrWillBeRawPtr<InterpolationValue> start, PassOwnPtrWillBeRawPtr<InterpolationValue> end)
        : m_start(start)
        , m_end(end)
        , m_lastFraction(std::numeric_limits<double>::quiet_NaN())
    {
        ASSERT(m_start);
        ASSERT(m_end);
    }

    virtual void interpolate(double fraction, OwnPtrWillBeMember<InterpolationValue>& result) const override final
    {
        // TODO(alancutter): Remove this optimisation once Oilpan is default.
        if (!std::isnan(m_lastFraction) && (fraction < 0.5) == (m_lastFraction < 0.5))
            return;
        result = ((fraction < 0.5) ? m_start : m_end)->clone();
        m_lastFraction = fraction;
    }

    OwnPtrWillBeMember<InterpolationValue> m_start;
    OwnPtrWillBeMember<InterpolationValue> m_end;
    mutable double m_lastFraction;
};

} // namespace blink

#endif // PrimitiveInterpolation_h
