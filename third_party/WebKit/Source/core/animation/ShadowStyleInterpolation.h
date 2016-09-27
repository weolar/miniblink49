// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ShadowStyleInterpolation_h
#define ShadowStyleInterpolation_h

#include "core/CoreExport.h"
#include "core/animation/StyleInterpolation.h"
#include "core/css/CSSShadowValue.h"
#include "core/style/ComputedStyle.h"
#include "platform/Length.h"

namespace blink {

class CORE_EXPORT ShadowStyleInterpolation : public StyleInterpolation {
public:
    typedef bool NonInterpolableType;

    static bool canCreateFrom(const CSSValue& start, const CSSValue& end);

    static bool usesDefaultStyleInterpolation(const CSSValue& start, const CSSValue& end);

    static PassOwnPtrWillBeRawPtr<InterpolableValue> toInterpolableValue(const CSSValue&, NonInterpolableType&);
    static PassRefPtrWillBeRawPtr<CSSValue> fromInterpolableValue(const InterpolableValue&, NonInterpolableType, InterpolationRange = RangeAll);

private:
    ShadowStyleInterpolation(PassOwnPtrWillBeRawPtr<InterpolableValue> start, PassOwnPtrWillBeRawPtr<InterpolableValue> end, CSSPropertyID id)
        : StyleInterpolation(start, end, id)
    {
    }

    static PassOwnPtrWillBeRawPtr<InterpolableValue> shadowToInterpolableValue(const CSSValue& value, NonInterpolableType& type)
    {
        return toInterpolableValue(value, type);
    }
    static PassRefPtrWillBeRawPtr<CSSValue> interpolableValueToShadow(const InterpolableValue& value, NonInterpolableType type, InterpolationRange range = RangeAll)
    {
        return fromInterpolableValue(value, type, range);
    }

    static PassOwnPtrWillBeRawPtr<InterpolableValue> lengthToInterpolableValue(PassRefPtrWillBeRawPtr<CSSPrimitiveValue>);

    friend class AnimationShadowStyleInterpolationTest;
};

} // namespace blink

#endif // ShadowStyleInterpolation_h
