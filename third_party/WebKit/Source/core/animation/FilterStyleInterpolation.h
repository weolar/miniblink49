// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FilterStyleInterpolation_h
#define FilterStyleInterpolation_h

#include "core/CoreExport.h"
#include "core/animation/ListStyleInterpolation.h"
#include "core/css/CSSFunctionValue.h"

namespace blink {

class CORE_EXPORT FilterStyleInterpolation : public StyleInterpolation {
public:
    typedef CSSValueID NonInterpolableType;
    typedef ListStyleInterpolationImpl<FilterStyleInterpolation, NonInterpolableType> FilterListStyleInterpolation;

    static PassRefPtrWillBeRawPtr<FilterListStyleInterpolation> maybeCreateList(const CSSValue& start, const CSSValue& end, CSSPropertyID);

    // The following are called by ListStyleInterpolation.
    static bool canCreateFrom(const CSSValue& start, const CSSValue& end);
    static PassOwnPtrWillBeRawPtr<InterpolableValue> toInterpolableValue(const CSSValue&, CSSValueID&);
    static PassRefPtrWillBeRawPtr<CSSFunctionValue> fromInterpolableValue(const InterpolableValue&, CSSValueID, InterpolationRange = RangeAll);

private:
    FilterStyleInterpolation(PassOwnPtrWillBeRawPtr<InterpolableValue> start, PassOwnPtrWillBeRawPtr<InterpolableValue> end, CSSPropertyID id)
        : StyleInterpolation(start, end, id)
    {
    }

    friend class FilterStyleInterpolationTest;
};

} // namespace blink

#endif // FilterStyleInterpolation_h
