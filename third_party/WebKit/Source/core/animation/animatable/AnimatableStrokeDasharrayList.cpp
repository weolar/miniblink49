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
#include "core/animation/animatable/AnimatableStrokeDasharrayList.h"

#include "core/animation/animatable/AnimatableLength.h"

namespace blink {

AnimatableStrokeDasharrayList::AnimatableStrokeDasharrayList(PassRefPtr<SVGDashArray> passLengths, float zoom)
{
    RefPtr<SVGDashArray> lengths = passLengths;
    for (const Length& dashLength : lengths->vector())
        m_values.append(AnimatableLength::create(dashLength, zoom));
}

PassRefPtr<SVGDashArray> AnimatableStrokeDasharrayList::toSVGDashArray(float zoom) const
{
    RefPtr<SVGDashArray> lengths = SVGDashArray::create();
    for (const auto& dashLength : m_values)
        lengths->append(toAnimatableLength(dashLength.get())->length(zoom, ValueRangeNonNegative));
    return lengths.release();
}

bool AnimatableStrokeDasharrayList::usesDefaultInterpolationWith(const AnimatableValue* value) const
{
    WillBeHeapVector<RefPtrWillBeMember<AnimatableValue>> from = m_values;
    WillBeHeapVector<RefPtrWillBeMember<AnimatableValue>> to = toAnimatableStrokeDasharrayList(value)->m_values;
    return !from.isEmpty() && !to.isEmpty() && AnimatableRepeatable::usesDefaultInterpolationWith(value);
}

PassRefPtrWillBeRawPtr<AnimatableValue> AnimatableStrokeDasharrayList::interpolateTo(const AnimatableValue* value, double fraction) const
{
    if (usesDefaultInterpolationWith(value))
        return defaultInterpolateTo(this, value, fraction);

    WillBeHeapVector<RefPtrWillBeMember<AnimatableValue>> from = m_values;
    WillBeHeapVector<RefPtrWillBeMember<AnimatableValue>> to = toAnimatableStrokeDasharrayList(value)->m_values;

    // The spec states that if the sum of all values is zero, this should be
    // treated like a value of 'none', which means that a solid line is drawn.
    // Since we animate to and from values of zero, treat a value of 'none' the
    // same. If both the two and from values are 'none', we return 'none'
    // rather than '0 0'.
    if (from.isEmpty() && to.isEmpty())
        return takeConstRef(this);
    if (from.isEmpty() || to.isEmpty()) {
        DEFINE_STATIC_REF_WILL_BE_PERSISTENT(AnimatableLength, zeroPixels, (AnimatableLength::create(Length(Fixed), 1)));
        if (from.isEmpty()) {
            from.append(zeroPixels);
            from.append(zeroPixels);
        }
        if (to.isEmpty()) {
            to.append(zeroPixels);
            to.append(zeroPixels);
        }
    }

    WillBeHeapVector<RefPtrWillBeMember<AnimatableValue>> interpolatedValues;
    bool success = interpolateLists(from, to, fraction, interpolatedValues);
    ASSERT_UNUSED(success, success);
    return adoptRefWillBeNoop(new AnimatableStrokeDasharrayList(interpolatedValues));
}

DEFINE_TRACE(AnimatableStrokeDasharrayList)
{
    AnimatableRepeatable::trace(visitor);
}

} // namespace blink
