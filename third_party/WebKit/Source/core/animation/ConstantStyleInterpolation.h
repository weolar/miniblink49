// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ConstantStyleInterpolation_h
#define ConstantStyleInterpolation_h

#include "core/animation/StyleInterpolation.h"
#include "core/css/resolver/StyleBuilder.h"

namespace blink {

class ConstantStyleInterpolation : public StyleInterpolation {
public:
    static PassRefPtrWillBeRawPtr<ConstantStyleInterpolation> create(CSSValue* value, CSSPropertyID id)
    {
        return adoptRefWillBeNoop(new ConstantStyleInterpolation(value, id));
    }

    virtual void apply(StyleResolverState& state) const override
    {
        StyleBuilder::applyProperty(m_id, state, m_value.get());
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        StyleInterpolation::trace(visitor);
        visitor->trace(m_value);
    }

private:
    ConstantStyleInterpolation(CSSValue* value, CSSPropertyID id)
        : StyleInterpolation(InterpolableList::create(0), InterpolableList::create(0), id)
        , m_value(value)
    { }

    RefPtrWillBeMember<CSSValue> m_value;
};

}

#endif
