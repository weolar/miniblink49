// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/NumberSVGInterpolation.h"

namespace blink {

PassRefPtrWillBeRawPtr<SVGNumber> NumberSVGInterpolation::fromInterpolableValue(const InterpolableValue& value, SVGNumberNegativeValuesMode negativeValuesMode)
{
    double doubleValue = toInterpolableNumber(value).value();
    if (negativeValuesMode == ForbidNegativeNumbers && doubleValue < 0)
        doubleValue = 0;
    return SVGNumber::create(doubleValue);
}

}
