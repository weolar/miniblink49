// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/InterpolationEffect.h"

namespace blink {

void InterpolationEffect::getActiveInterpolations(double fraction, double iterationDuration, OwnPtrWillBeRawPtr<WillBeHeapVector<RefPtrWillBeMember<Interpolation>>>& result) const
{
    if (!result)
        result = adoptPtrWillBeNoop(new WillBeHeapVector<RefPtrWillBeMember<Interpolation>>());

    size_t existingSize = result->size();
    size_t resultIndex = 0;

    for (const auto& record : m_interpolations) {
        if (fraction >= record->m_applyFrom && fraction < record->m_applyTo) {
            RefPtrWillBeRawPtr<Interpolation> interpolation = record->m_interpolation;
            double localFraction = (fraction - record->m_start) / (record->m_end - record->m_start);
            if (record->m_easing)
                localFraction = record->m_easing->evaluate(localFraction, accuracyForDuration(iterationDuration));
            interpolation->interpolate(0, localFraction);
            if (resultIndex < existingSize)
                (*result)[resultIndex++] = interpolation;
            else
                result->append(interpolation);
        }
    }
    if (resultIndex < existingSize)
        result->shrink(resultIndex);
}

void InterpolationEffect::addInterpolationsFromKeyframes(PropertyHandle property, Element* element, const ComputedStyle* baseStyle, Keyframe::PropertySpecificKeyframe& keyframeA, Keyframe::PropertySpecificKeyframe& keyframeB, double applyFrom, double applyTo)
{
    RefPtrWillBeRawPtr<Interpolation> interpolation = keyframeA.maybeCreateInterpolation(property, keyframeB, element, baseStyle);

    if (interpolation) {
        addInterpolation(interpolation, &keyframeA.easing(), keyframeA.offset(), keyframeB.offset(), applyFrom, applyTo);
    } else {
        RefPtrWillBeRawPtr<Interpolation> interpolationA = keyframeA.maybeCreateInterpolation(property, keyframeA, element, baseStyle);
        RefPtrWillBeRawPtr<Interpolation> interpolationB = keyframeB.maybeCreateInterpolation(property, keyframeB, element, baseStyle);

        Vector<TimingFunction::PartitionRegion> regions = Vector<TimingFunction::PartitionRegion>();
        keyframeA.easing().partition(regions);

        size_t regionIndex = 0;
        for (const auto& region : regions) {
            double regionStart = blend(keyframeA.offset(), keyframeB.offset(), region.start);
            double regionEnd = blend(keyframeA.offset(), keyframeB.offset(), region.end);

            double regionApplyFrom = regionIndex == 0 ? applyFrom : regionStart;
            double regionApplyTo = regionIndex == regions.size() - 1 ? applyTo : regionEnd;

            if (region.half == TimingFunction::RangeHalf::Lower) {
                interpolation = interpolationA;
            } else if (region.half == TimingFunction::RangeHalf::Upper) {
                interpolation = interpolationB;
            } else {
                ASSERT_NOT_REACHED();
                continue;
            }

            if (interpolation) {
                addInterpolation(interpolation.release(),
                    &keyframeA.easing(), regionStart, regionEnd, regionApplyFrom, regionApplyTo);
            }

            regionIndex++;
        }
    }
}

DEFINE_TRACE(InterpolationEffect::InterpolationRecord)
{
    visitor->trace(m_interpolation);
}

DEFINE_TRACE(InterpolationEffect)
{
    visitor->trace(m_interpolations);
}

} // namespace blink
