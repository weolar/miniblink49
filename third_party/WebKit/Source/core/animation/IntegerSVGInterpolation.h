// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef IntegerSVGInterpolation_h
#define IntegerSVGInterpolation_h

#include "core/animation/SVGInterpolation.h"
#include "core/svg/SVGInteger.h"

namespace blink {

class IntegerSVGInterpolation : public SVGInterpolation {
public:
    static PassRefPtrWillBeRawPtr<IntegerSVGInterpolation> create(SVGPropertyBase* start, SVGPropertyBase* end, PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase> attribute)
    {
        return adoptRefWillBeNoop(new IntegerSVGInterpolation(toInterpolableValue(start), toInterpolableValue(end), attribute));
    }

    virtual PassRefPtrWillBeRawPtr<SVGPropertyBase> interpolatedValue(SVGElement&) const override final
    {
        return fromInterpolableValue(*m_cachedValue);
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        SVGInterpolation::trace(visitor);
    }

private:
    IntegerSVGInterpolation(PassOwnPtrWillBeRawPtr<InterpolableValue> start, PassOwnPtrWillBeRawPtr<InterpolableValue> end, PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase> attribute)
        : SVGInterpolation(start, end, attribute)
    {
    }

    static PassOwnPtrWillBeRawPtr<InterpolableValue> toInterpolableValue(SVGPropertyBase* value)
    {
        return InterpolableNumber::create(toSVGInteger(value)->value());
    }

    static PassRefPtrWillBeRawPtr<SVGInteger> fromInterpolableValue(const InterpolableValue& value)
    {
        return SVGInteger::create(clampTo<int>(roundf(toInterpolableNumber(value).value())));
    }
};

}

#endif // IntegerSVGInterpolation_h
