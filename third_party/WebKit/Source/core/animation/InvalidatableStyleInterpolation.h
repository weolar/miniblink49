// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InvalidatableStyleInterpolation_h
#define InvalidatableStyleInterpolation_h

#include "core/animation/InterpolationType.h"
#include "core/animation/InterpolationValue.h"
#include "core/animation/PrimitiveInterpolation.h"
#include "core/animation/StyleInterpolation.h"

namespace blink {

// TODO(alancutter): This class will replace *StyleInterpolation, SVGInterpolation, Interpolation.
// For now it needs to distinguish itself during the refactor and temporarily has an ugly name.
// TODO(alancutter): Make this class generic for any animation environment so it can be reused for SVG animations.
class CORE_EXPORT InvalidatableStyleInterpolation : public StyleInterpolation {
public:
    static PassRefPtrWillBeRawPtr<InvalidatableStyleInterpolation> create(
        const Vector<const InterpolationType*>& InterpolationTypes,
        const CSSPropertySpecificKeyframe& startKeyframe,
        const CSSPropertySpecificKeyframe& endKeyframe)
    {
        return adoptRefWillBeNoop(new InvalidatableStyleInterpolation(InterpolationTypes, startKeyframe, endKeyframe));
    }

    virtual void interpolate(int iteration, double fraction);
    virtual void apply(StyleResolverState&) const;

    virtual bool isInvalidatableStyleInterpolation() const { return true; }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_cachedConversion);
        visitor->trace(m_conversionCheckers);
        visitor->trace(m_cachedValue);
        StyleInterpolation::trace(visitor);
    }

private:
    InvalidatableStyleInterpolation(
        const Vector<const InterpolationType*>& InterpolationTypes,
        const CSSPropertySpecificKeyframe& startKeyframe,
        const CSSPropertySpecificKeyframe& endKeyframe);

    void ensureValidInterpolation(const StyleResolverState&) const;
    bool isCacheValid(const StyleResolverState&) const;
    bool maybeCachePairwiseConversion(const StyleResolverState*) const;
    PassOwnPtrWillBeRawPtr<InterpolationValue> convertSingleKeyframe(const CSSPropertySpecificKeyframe&, const StyleResolverState&) const;

    const Vector<const InterpolationType*>& m_interpolationTypes;
    const CSSPropertySpecificKeyframe& m_startKeyframe;
    const CSSPropertySpecificKeyframe& m_endKeyframe;
    double m_currentFraction;
    mutable OwnPtrWillBeMember<PrimitiveInterpolation> m_cachedConversion;
    mutable InterpolationType::ConversionCheckers m_conversionCheckers;
    mutable OwnPtrWillBeMember<InterpolationValue> m_cachedValue;
};

DEFINE_TYPE_CASTS(InvalidatableStyleInterpolation, Interpolation, value, value->isInvalidatableStyleInterpolation(), value.isInvalidatableStyleInterpolation());

} // namespace blink

#endif // InvalidatableStyleInterpolation_h
