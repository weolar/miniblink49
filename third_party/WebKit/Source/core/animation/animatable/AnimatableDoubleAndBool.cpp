// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/animatable/AnimatableDoubleAndBool.h"

#include "platform/animation/AnimationUtilities.h"

namespace blink {

bool AnimatableDoubleAndBool::usesDefaultInterpolationWith(const AnimatableValue* value) const
{
    const AnimatableDoubleAndBool* other = toAnimatableDoubleAndBool(value);
    return flag() != other->flag();
}

PassRefPtrWillBeRawPtr<AnimatableValue> AnimatableDoubleAndBool::interpolateTo(const AnimatableValue* value, double fraction) const
{
    const AnimatableDoubleAndBool* other = toAnimatableDoubleAndBool(value);
    if (flag() == other->flag())
        return AnimatableDoubleAndBool::create(blend(m_number, other->m_number, fraction), flag());

    return defaultInterpolateTo(this, value, fraction);
}

bool AnimatableDoubleAndBool::equalTo(const AnimatableValue* value) const
{
    const AnimatableDoubleAndBool* other = toAnimatableDoubleAndBool(value);
    return toDouble() == other->toDouble() && flag() == other->flag();
}

}
