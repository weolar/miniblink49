// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PointSVGInterpolation_h
#define PointSVGInterpolation_h

#include "core/animation/SVGInterpolation.h"
#include "core/svg/SVGPointList.h"

namespace blink {

class PointSVGInterpolation {
public:
    typedef SVGPointList ListType;
    typedef void NonInterpolableType;

    static PassOwnPtrWillBeRawPtr<InterpolableList> toInterpolableValue(SVGPropertyBase* item)
    {
        OwnPtrWillBeRawPtr<InterpolableList> result = InterpolableList::create(2);
        result->set(0, InterpolableNumber::create(toSVGPoint(item)->x()));
        result->set(1, InterpolableNumber::create(toSVGPoint(item)->y()));
        return result.release();
    }

    static PassRefPtrWillBeRawPtr<SVGPoint> fromInterpolableValue(const InterpolableValue& value)
    {
        const InterpolableList& list = toInterpolableList(value);
        return SVGPoint::create(FloatPoint(
            toInterpolableNumber(list.get(0))->value(),
            toInterpolableNumber(list.get(1))->value()));
    }
};

}

#endif // PointSVGInterpolation_h
