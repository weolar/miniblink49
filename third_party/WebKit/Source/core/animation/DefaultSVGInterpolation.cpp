// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/DefaultSVGInterpolation.h"

#include "core/svg/SVGElement.h"

namespace blink {

DefaultSVGInterpolation::DefaultSVGInterpolation(SVGPropertyBase* start, SVGPropertyBase* end, PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase> attribute)
    : SVGInterpolation(InterpolableBool::create(false), InterpolableBool::create(true), attribute)
    , m_start(start)
    , m_end(end)
{
}

PassRefPtrWillBeRawPtr<SVGPropertyBase> DefaultSVGInterpolation::interpolatedValue(SVGElement& element) const
{
    if (toInterpolableBool(m_cachedValue.get())->value())
        return m_end;

    return m_start;
}

DEFINE_TRACE(DefaultSVGInterpolation)
{
    visitor->trace(m_start);
    visitor->trace(m_end);
    SVGInterpolation::trace(visitor);
}

}
