// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NumberSVGInterpolation_h
#define NumberSVGInterpolation_h

#include "core/animation/SVGInterpolation.h"
#include "core/svg/SVGNumber.h"
#include "core/svg/SVGNumberList.h"

namespace blink {

enum SVGNumberNegativeValuesMode {
    AllowNegativeNumbers,
    ForbidNegativeNumbers
};

class NumberSVGInterpolation : public SVGInterpolation {
public:
    typedef SVGNumberList ListType;
    typedef void NonInterpolableType;

    static PassRefPtrWillBeRawPtr<NumberSVGInterpolation> create(SVGPropertyBase* start, SVGPropertyBase* end, PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase> attribute, SVGNumberNegativeValuesMode negativeValuesMode)
    {
        return adoptRefWillBeNoop(new NumberSVGInterpolation(toInterpolableValue(start), toInterpolableValue(end), attribute, negativeValuesMode));
    }

    static bool canCreateFrom(SVGPropertyBase* value)
    {
        return true;
    }

    virtual PassRefPtrWillBeRawPtr<SVGPropertyBase> interpolatedValue(SVGElement&) const override final
    {
        return fromInterpolableValue(*m_cachedValue, m_negativeValuesMode);
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        SVGInterpolation::trace(visitor);
    }

    static PassOwnPtrWillBeRawPtr<InterpolableNumber> toInterpolableValue(SVGPropertyBase* value)
    {
        return InterpolableNumber::create(toSVGNumber(value)->value());
    }

    static PassRefPtrWillBeRawPtr<SVGNumber> fromInterpolableValue(const InterpolableValue&, SVGNumberNegativeValuesMode = AllowNegativeNumbers);

private:
    NumberSVGInterpolation(PassOwnPtrWillBeRawPtr<InterpolableValue> start, PassOwnPtrWillBeRawPtr<InterpolableValue> end, PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase> attribute, SVGNumberNegativeValuesMode negativeValuesMode)
        : SVGInterpolation(start, end, attribute)
        , m_negativeValuesMode(negativeValuesMode)
    {
    }

    const SVGNumberNegativeValuesMode m_negativeValuesMode;
};

}

#endif // NumberSVGInterpolation_h
