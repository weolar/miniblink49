// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef StyleInterpolation_h
#define StyleInterpolation_h

#include "core/CSSPropertyNames.h"
#include "core/CoreExport.h"
#include "core/animation/Interpolation.h"
#include "core/animation/PropertyHandle.h"

namespace blink {

class StyleResolverState;

enum InterpolationRange {
    RangeAll,
    RangeFloor,
    RangeGreaterThanOrEqualToOne,
    RangeNonNegative,
    RangeRound,
    RangeRoundGreaterThanOrEqualToOne,
    RangeOpacityFIXME,
    RangeZeroToOne
};

class CORE_EXPORT StyleInterpolation : public Interpolation {
public:
    // 1) convert m_cachedValue into an X
    // 2) shove X into StyleResolverState
    // X can be:
    // (1) a CSSValue (and applied via StyleBuilder::applyProperty)
    // (2) an AnimatableValue (and applied via // AnimatedStyleBuilder::applyProperty)
    // (3) a custom value that is inserted directly into the StyleResolverState.
    virtual void apply(StyleResolverState&) const = 0;

    virtual bool isStyleInterpolation() const override final { return true; }
    virtual bool isDeferredLegacyStyleInterpolation() const { return false; }

    CSSPropertyID id() const { return m_id; }

    virtual PropertyHandle property() const override final
    {
        return PropertyHandle(id());
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        Interpolation::trace(visitor);
    }

protected:
    CSSPropertyID m_id;

    StyleInterpolation(PassOwnPtrWillBeRawPtr<InterpolableValue> start, PassOwnPtrWillBeRawPtr<InterpolableValue> end, CSSPropertyID id)
        : Interpolation(start, end)
        , m_id(id)
    {
    }
};

DEFINE_TYPE_CASTS(StyleInterpolation, Interpolation, value, value->isStyleInterpolation(), value.isStyleInterpolation());

}

#endif
