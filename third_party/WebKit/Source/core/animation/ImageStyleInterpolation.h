// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ImageStyleInterpolation_h
#define ImageStyleInterpolation_h

#include "core/animation/StyleInterpolation.h"
#include "core/css/CSSCrossfadeValue.h"

namespace blink {
class ImageStyleInterpolation : public StyleInterpolation {
public:
    static PassRefPtrWillBeRawPtr<ImageStyleInterpolation> create(CSSValue& start, CSSValue& end, CSSPropertyID id)
    {
        return adoptRefWillBeNoop(new ImageStyleInterpolation(&start, &end, id));
    }

    DECLARE_VIRTUAL_TRACE();

    static bool canCreateFrom(const CSSValue&);
    virtual void apply(StyleResolverState&) const override;

private:
    ImageStyleInterpolation(PassRefPtrWillBeRawPtr<CSSValue> start, PassRefPtrWillBeRawPtr<CSSValue> end, CSSPropertyID id)
        : StyleInterpolation(InterpolableNumber::create(0.0), InterpolableNumber::create(1.0), id)
        , m_initialImage(start)
        , m_finalImage(end)
    {
    }

    RefPtrWillBeMember<CSSValue> m_initialImage;
    RefPtrWillBeMember<CSSValue> m_finalImage;
};
}

#endif // ImageStyleInterpolation_h
