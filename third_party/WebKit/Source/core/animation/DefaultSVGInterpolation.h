// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DefaultSVGInterpolation_h
#define DefaultSVGInterpolation_h

#include "core/animation/SVGInterpolation.h"

namespace blink {

class DefaultSVGInterpolation : public SVGInterpolation {
public:
    static PassRefPtrWillBeRawPtr<DefaultSVGInterpolation> create(SVGPropertyBase* start, SVGPropertyBase* end, PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase> attribute)
    {
        return adoptRefWillBeNoop(new DefaultSVGInterpolation(start, end, attribute));
    }

    virtual PassRefPtrWillBeRawPtr<SVGPropertyBase> interpolatedValue(SVGElement&) const override final;

    DECLARE_VIRTUAL_TRACE();

private:
    DefaultSVGInterpolation(SVGPropertyBase* start, SVGPropertyBase* end, PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase> attribute);

    RefPtrWillBeMember<SVGPropertyBase> m_start;
    RefPtrWillBeMember<SVGPropertyBase> m_end;
};

}

#endif // DefaultSVGInterpolation_h
