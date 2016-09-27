// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ColorStyleInterpolation_h
#define ColorStyleInterpolation_h

#include "core/CoreExport.h"
#include "core/animation/StyleInterpolation.h"
#include "core/css/CSSPrimitiveValue.h"
#include "platform/graphics/Color.h"

namespace blink {

class CORE_EXPORT ColorStyleInterpolation : public StyleInterpolation {
public:
    static PassRefPtrWillBeRawPtr<ColorStyleInterpolation> create(const CSSValue& start, const CSSValue& end, CSSPropertyID id)
    {
        return adoptRefWillBeNoop(new ColorStyleInterpolation(colorToInterpolableValue(start), colorToInterpolableValue(end), id));
    }

    static PassRefPtrWillBeRawPtr<ColorStyleInterpolation> maybeCreateFromColor(const CSSValue& start, const CSSValue& end, CSSPropertyID);

    static bool shouldUseLegacyStyleInterpolation(const CSSValue& start, const CSSValue& end);

    static bool canCreateFrom(const CSSValue&);

    virtual void apply(StyleResolverState&) const override;

    DECLARE_VIRTUAL_TRACE();

private:
    ColorStyleInterpolation(PassOwnPtrWillBeRawPtr<InterpolableValue> start, PassOwnPtrWillBeRawPtr<InterpolableValue> end, CSSPropertyID id)
        : StyleInterpolation(start, end, id)
    {
    }

    static PassOwnPtrWillBeRawPtr<InterpolableValue> colorToInterpolableValue(const CSSValue&);
    static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> interpolableValueToColor(const InterpolableValue&);

    friend class AnimationColorStyleInterpolationTest;
    friend class ShadowStyleInterpolation;
};
}

#endif
