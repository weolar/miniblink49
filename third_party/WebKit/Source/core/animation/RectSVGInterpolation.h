// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef RectSVGInterpolation_h
#define RectSVGInterpolation_h

#include "core/animation/SVGInterpolation.h"
#include "core/svg/SVGRect.h"

namespace blink {

class SVGRect;

class RectSVGInterpolation : public SVGInterpolation {
public:
    static PassRefPtrWillBeRawPtr<RectSVGInterpolation> create(SVGPropertyBase* start, SVGPropertyBase* end, PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase> attribute)
    {
        return adoptRefWillBeNoop(new RectSVGInterpolation(toInterpolableValue(start), toInterpolableValue(end), attribute));
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
    RectSVGInterpolation(PassOwnPtrWillBeRawPtr<InterpolableValue> start, PassOwnPtrWillBeRawPtr<InterpolableValue> end, PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase> attribute)
        : SVGInterpolation(start, end, attribute)
    {
    }

    static PassOwnPtrWillBeRawPtr<InterpolableValue> toInterpolableValue(SVGPropertyBase*);

    static PassRefPtrWillBeRawPtr<SVGRect> fromInterpolableValue(const InterpolableValue&);
};

}

#endif // RectSVGInterpolation_h
