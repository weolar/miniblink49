// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SVGStrokeDasharrayStyleInterpolation_h
#define SVGStrokeDasharrayStyleInterpolation_h

#include "core/CoreExport.h"
#include "core/animation/StyleInterpolation.h"
#include "core/css/CSSPrimitiveValue.h"

namespace blink {

class CSSValueList;

class CORE_EXPORT SVGStrokeDasharrayStyleInterpolation : public StyleInterpolation {
public:
    static PassRefPtrWillBeRawPtr<SVGStrokeDasharrayStyleInterpolation> maybeCreate(const CSSValue& start, const CSSValue& end, CSSPropertyID);

    virtual void apply(StyleResolverState&) const override;

    virtual void trace(Visitor* visitor) override
    {
        StyleInterpolation::trace(visitor);
    }

private:
    SVGStrokeDasharrayStyleInterpolation(PassOwnPtrWillBeRawPtr<InterpolableValue> start, PassOwnPtrWillBeRawPtr<InterpolableValue> end, CSSPropertyID id)
        : StyleInterpolation(start, end, id)
    { }

    static bool canCreateFrom(const CSSValue&);
    static PassRefPtrWillBeRawPtr<CSSValueList> interpolableValueToStrokeDasharray(const InterpolableValue&);

    friend class AnimationSVGStrokeDasharrayStyleInterpolationTest;
};

}

#endif // SVGStrokeDasharrayStyleInterpolation_h
