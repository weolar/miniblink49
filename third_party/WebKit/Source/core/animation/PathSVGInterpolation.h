// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PathSVGInterpolation_h
#define PathSVGInterpolation_h

#include "core/animation/SVGInterpolation.h"
#include "core/svg/SVGPathSegList.h"

namespace blink {

class PathSVGInterpolation : public SVGInterpolation {
public:
    static PassRefPtrWillBeRawPtr<PathSVGInterpolation> maybeCreate(SVGPropertyBase* start, SVGPropertyBase* end, PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase> attribute);

    virtual PassRefPtrWillBeRawPtr<SVGPropertyBase> interpolatedValue(SVGElement&) const override final;

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        SVGInterpolation::trace(visitor);
    }

private:
    PathSVGInterpolation(PassOwnPtrWillBeRawPtr<InterpolableValue> start, PassOwnPtrWillBeRawPtr<InterpolableValue> end, PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase> attribute,
        Vector<SVGPathSegType> pathSegTypes)
        : SVGInterpolation(start, end, attribute)
    {
        m_pathSegTypes.swap(pathSegTypes);
    }

    static PassRefPtrWillBeRawPtr<SVGPropertyBase> fromInterpolableValue(const InterpolableValue&, const Vector<SVGPathSegType>&, SVGPathElement*);

    Vector<SVGPathSegType> m_pathSegTypes;
};

}

#endif // NumberSVGInterpolation_h
