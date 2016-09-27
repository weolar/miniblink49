/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/animation/animatable/AnimatableLengthBoxAndBool.h"

#include "core/animation/animatable/AnimatableLength.h"
#include "core/animation/animatable/AnimatableLengthBox.h"

namespace blink {

static bool sidesHaveSameUnits(const AnimatableValue* sideA, const AnimatableValue* sideB)
{
    if (!sideA->isLength() || !sideB->isLength())
        return false;
    return toAnimatableLength(sideA)->hasSameUnits(toAnimatableLength(sideB));
}

// This is only used by *-image-slice properties which cannot interpolate between numbers and percentages.
// Numbers are internally represented by pixels on the ComputedStyle so we must manually type check both sides.
bool AnimatableLengthBoxAndBool::usesDefaultInterpolationWith(const AnimatableValue* value) const
{
    const AnimatableLengthBoxAndBool* lengthBoxAndBool = toAnimatableLengthBoxAndBool(value);
    if (lengthBoxAndBool->flag() != flag())
        return true;
    if (!box()->isLengthBox() || !lengthBoxAndBool->box()->isLengthBox())
        return AnimatableValue::usesDefaultInterpolation(box(), lengthBoxAndBool->box());
    const AnimatableLengthBox* boxA = toAnimatableLengthBox(box());
    const AnimatableLengthBox* boxB = toAnimatableLengthBox(lengthBoxAndBool->box());
    return !sidesHaveSameUnits(boxA->left(), boxB->left())
        || !sidesHaveSameUnits(boxA->right(), boxB->right())
        || !sidesHaveSameUnits(boxA->top(), boxB->top())
        || !sidesHaveSameUnits(boxA->bottom(), boxB->bottom());
}

PassRefPtrWillBeRawPtr<AnimatableValue> AnimatableLengthBoxAndBool::interpolateTo(const AnimatableValue* value, double fraction) const
{
    const AnimatableLengthBoxAndBool* lengthBoxAndBool = toAnimatableLengthBoxAndBool(value);
    if (usesDefaultInterpolationWith(lengthBoxAndBool))
        return defaultInterpolateTo(this, value, fraction);
    return AnimatableLengthBoxAndBool::create(
        AnimatableValue::interpolate(box(), lengthBoxAndBool->box(), fraction),
        flag());
}

bool AnimatableLengthBoxAndBool::equalTo(const AnimatableValue* value) const
{
    const AnimatableLengthBoxAndBool* lengthBox = toAnimatableLengthBoxAndBool(value);
    return box()->equals(lengthBox->box()) && flag() == lengthBox->flag();
}

DEFINE_TRACE(AnimatableLengthBoxAndBool)
{
    visitor->trace(m_box);
    AnimatableValue::trace(visitor);
}

}
