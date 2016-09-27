// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LengthPairStyleInterpolation_h
#define LengthPairStyleInterpolation_h

#include "core/CoreExport.h"
#include "core/animation/StyleInterpolation.h"
#include "platform/Length.h"

namespace blink {

class CORE_EXPORT LengthPairStyleInterpolation : public StyleInterpolation {
public:
    static PassRefPtrWillBeRawPtr<LengthPairStyleInterpolation> create(const CSSValue& start, const CSSValue& end, CSSPropertyID id, InterpolationRange range)
    {
        return adoptRefWillBeNoop(new LengthPairStyleInterpolation(lengthPairToInterpolableValue(start), lengthPairToInterpolableValue(end), id, range));
    }

    static bool canCreateFrom(const CSSValue&);

    virtual void apply(StyleResolverState&) const override;

private:
    LengthPairStyleInterpolation(PassOwnPtrWillBeRawPtr<InterpolableValue> start, PassOwnPtrWillBeRawPtr<InterpolableValue> end, CSSPropertyID id, InterpolationRange range)
        : StyleInterpolation(start, end, id), m_range(range)
    {
    }

    static PassOwnPtrWillBeRawPtr<InterpolableValue> lengthPairToInterpolableValue(const CSSValue&);
    static PassRefPtrWillBeRawPtr<CSSValue> interpolableValueToLengthPair(InterpolableValue*, InterpolationRange);

    InterpolationRange m_range;

    friend class LengthPairStyleInterpolationTest;
};

} // namespace blink

#endif // LengthPairStyleInterpolation_h
