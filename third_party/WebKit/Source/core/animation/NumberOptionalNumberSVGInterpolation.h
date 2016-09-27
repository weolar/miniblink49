// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NumberOptionalNumberSVGInterpolation_h
#define NumberOptionalNumberSVGInterpolation_h

#include "core/animation/SVGInterpolation.h"
#include "core/svg/SVGNumber.h"
#include "core/svg/SVGNumberOptionalNumber.h"

namespace blink {

class SVGNumberOptionalNumber;

class NumberOptionalNumberSVGInterpolation : public SVGInterpolation {
public:
    static PassRefPtrWillBeRawPtr<NumberOptionalNumberSVGInterpolation> create(SVGPropertyBase* start, SVGPropertyBase* end, PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase> attribute)
    {
        return adoptRefWillBeNoop(new NumberOptionalNumberSVGInterpolation(toInterpolableValue(start), toInterpolableValue(end), attribute));
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
    NumberOptionalNumberSVGInterpolation(PassOwnPtrWillBeRawPtr<InterpolableValue> start, PassOwnPtrWillBeRawPtr<InterpolableValue> end, PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase> attribute)
        : SVGInterpolation(start, end, attribute)
    {
    }

    static PassOwnPtrWillBeRawPtr<InterpolableValue> toInterpolableValue(SVGPropertyBase* value)
    {
        RefPtrWillBeRawPtr<SVGNumberOptionalNumber> numberOptionalNumber = toSVGNumberOptionalNumber(value);
        OwnPtrWillBeRawPtr<InterpolableList> result = InterpolableList::create(2);
        result->set(0, InterpolableNumber::create(numberOptionalNumber->firstNumber()->value()));
        result->set(1, InterpolableNumber::create(numberOptionalNumber->secondNumber()->value()));
        return result.release();
    }

    static PassRefPtrWillBeRawPtr<SVGNumberOptionalNumber> fromInterpolableValue(const InterpolableValue& value)
    {
        const InterpolableList& list = toInterpolableList(value);
        return SVGNumberOptionalNumber::create(
            SVGNumber::create(toInterpolableNumber(list.get(0))->value()),
            SVGNumber::create(toInterpolableNumber(list.get(1))->value()));
    }
};

}

#endif // NumberOptionalNumberSVGInterpolation_h
