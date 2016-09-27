// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSSValueInterpolationType_h
#define CSSValueInterpolationType_h

#include "core/animation/InterpolationType.h"

namespace blink {

// Never supports pairwise conversion while always supporting single conversion.
// A catch all for default for CSSValues.
class CSSValueInterpolationType : public InterpolationType {
public:
    CSSValueInterpolationType(CSSPropertyID property)
        : InterpolationType(property)
    { }

    virtual PassOwnPtrWillBeRawPtr<InterpolationValue> maybeConvertSingle(const CSSPropertySpecificKeyframe&, const StyleResolverState*, ConversionCheckers&) const override final;
    virtual void apply(const InterpolableValue&, const NonInterpolableValue*, StyleResolverState&) const override final;
};

class DefaultNonInterpolableValue : public NonInterpolableValue {
public:
    virtual ~DefaultNonInterpolableValue() { }
    static PassRefPtrWillBeRawPtr<DefaultNonInterpolableValue> create(PassRefPtrWillBeRawPtr<CSSValue> cssValue)
    {
        return adoptRefWillBeNoop(new DefaultNonInterpolableValue(cssValue));
    }

    CSSValue* cssValue() const { return m_cssValue.get(); }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        NonInterpolableValue::trace(visitor);
        visitor->trace(m_cssValue);
    }

    DECLARE_NON_INTERPOLABLE_VALUE_TYPE();

private:
    DefaultNonInterpolableValue(PassRefPtrWillBeRawPtr<CSSValue> cssValue)
        : m_cssValue(cssValue)
    { }

    RefPtrWillBeMember<CSSValue> m_cssValue;
};

DEFINE_NON_INTERPOLABLE_VALUE_TYPE_CASTS(DefaultNonInterpolableValue);

} // namespace blink

#endif // CSSValueInterpolationType_h
