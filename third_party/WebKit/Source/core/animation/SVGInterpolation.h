// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SVGInterpolation_h
#define SVGInterpolation_h

#include "core/animation/Interpolation.h"
#include "core/animation/PropertyHandle.h"
#include "core/svg/properties/SVGAnimatedProperty.h"

namespace blink {

class SVGInterpolation : public Interpolation {
public:
    virtual bool isSVGInterpolation() const override final { return true; }

    SVGAnimatedPropertyBase* attribute() const { return m_attribute.get(); }

    const QualifiedName& attributeName() const { return m_attribute->attributeName(); }

    virtual PropertyHandle property() const override final
    {
        return PropertyHandle(attributeName());
    }

    void apply(SVGElement&) const;

    virtual PassRefPtrWillBeRawPtr<SVGPropertyBase> interpolatedValue(SVGElement&) const = 0;

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_attribute);
        Interpolation::trace(visitor);
    }

protected:
    SVGInterpolation(PassOwnPtrWillBeRawPtr<InterpolableValue> start, PassOwnPtrWillBeRawPtr<InterpolableValue> end, PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase> attribute)
        : Interpolation(start, end)
        , m_attribute(attribute)
    {
    }

    RefPtrWillBeMember<SVGAnimatedPropertyBase> m_attribute;
};

DEFINE_TYPE_CASTS(SVGInterpolation, Interpolation, value, value->isSVGInterpolation(), value.isSVGInterpolation());

}

#endif // SVGInterpolation_h
