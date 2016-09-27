// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InterpolationType_h
#define InterpolationType_h

#include "core/animation/InterpolableValue.h"
#include "core/animation/NonInterpolableValue.h"
#include "core/animation/PrimitiveInterpolation.h"
#include "core/animation/StringKeyframe.h"
#include "platform/heap/Handle.h"

namespace blink {

class StyleResolverState;

// A singleton that:
// - Converts from animation keyframe(s) to interpolation compatible representations: maybeConvertPairwise() and maybeConvertSingle()
// - Applies interpolation compatible representations of values to a StyleResolverState: apply()
class InterpolationType {
public:
    CSSPropertyID property() const { return m_property; }

    // Represents logic for determining whether a conversion decision is no longer valid given the current environment.
    class ConversionChecker : public NoBaseWillBeGarbageCollectedFinalized<ConversionChecker> {
    public:
        virtual ~ConversionChecker() { }
        virtual bool isValid(const StyleResolverState&) const = 0;
        DEFINE_INLINE_VIRTUAL_TRACE() { }
    };
    using ConversionCheckers = WillBeHeapVector<OwnPtrWillBeMember<ConversionChecker>>;

    virtual PassOwnPtrWillBeRawPtr<PairwisePrimitiveInterpolation> maybeConvertPairwise(const CSSPropertySpecificKeyframe& startKeyframe, const CSSPropertySpecificKeyframe& endKeyframe, const StyleResolverState*, ConversionCheckers&) const
    {
        return nullptr;
    }

    virtual PassOwnPtrWillBeRawPtr<InterpolationValue> maybeConvertSingle(const CSSPropertySpecificKeyframe&, const StyleResolverState*, ConversionCheckers&) const = 0;

    virtual void apply(const InterpolableValue&, const NonInterpolableValue*, StyleResolverState&) const = 0;

protected:
    InterpolationType(CSSPropertyID property)
        : m_property(property)
    { }

    const CSSPropertyID m_property;
};

} // namespace blink

#endif // InterpolationType_h
