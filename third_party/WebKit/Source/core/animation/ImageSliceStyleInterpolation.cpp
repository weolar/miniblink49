// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/ImageSliceStyleInterpolation.h"

#include "core/css/CSSBorderImageSliceValue.h"
#include "core/css/CSSPrimitiveValue.h"
#include "core/css/Rect.h"
#include "core/css/resolver/StyleBuilder.h"

namespace blink {

bool ImageSliceStyleInterpolation::usesDefaultInterpolation(const CSSValue& start, const CSSValue& end)
{
    if (!start.isBorderImageSliceValue() || !end.isBorderImageSliceValue())
        return true;
    const CSSBorderImageSliceValue& startSlice = toCSSBorderImageSliceValue(start);
    const CSSBorderImageSliceValue& endSlice = toCSSBorderImageSliceValue(end);
    return startSlice.slices()->top()->isPercentage() != endSlice.slices()->top()->isPercentage()
        || startSlice.m_fill != endSlice.m_fill;
}

namespace {

class Decomposition {
    STACK_ALLOCATED();
public:
    Decomposition(const CSSBorderImageSliceValue& value)
    {
        decompose(value);
    }

    OwnPtrWillBeMember<InterpolableValue> interpolableValue;
    ImageSliceStyleInterpolation::Metadata metadata;

private:
    void decompose(const CSSBorderImageSliceValue& value)
    {
        const size_t kQuadSides = 4;
        OwnPtrWillBeRawPtr<InterpolableList> interpolableList = InterpolableList::create(kQuadSides);
        const Quad& quad = *value.slices();
        interpolableList->set(0, InterpolableNumber::create(quad.top()->getDoubleValue()));
        interpolableList->set(1, InterpolableNumber::create(quad.right()->getDoubleValue()));
        interpolableList->set(2, InterpolableNumber::create(quad.bottom()->getDoubleValue()));
        interpolableList->set(3, InterpolableNumber::create(quad.left()->getDoubleValue()));
        bool isPercentage = quad.top()->isPercentage();
        ASSERT(quad.bottom()->isPercentage() == isPercentage
            && quad.left()->isPercentage() == isPercentage
            && quad.right()->isPercentage() == isPercentage);

        interpolableValue = interpolableList.release();
        metadata = ImageSliceStyleInterpolation::Metadata {isPercentage, value.m_fill};
    }
};

PassRefPtrWillBeRawPtr<CSSBorderImageSliceValue> compose(const InterpolableValue& value, const ImageSliceStyleInterpolation::Metadata& metadata)
{
    const InterpolableList& interpolableList = toInterpolableList(value);
    CSSPrimitiveValue::UnitType type = metadata.isPercentage ? CSSPrimitiveValue::CSS_PERCENTAGE : CSSPrimitiveValue::CSS_NUMBER;
    RefPtrWillBeRawPtr<Quad> quad = Quad::create();
    quad->setTop(CSSPrimitiveValue::create(clampTo<double>(toInterpolableNumber(interpolableList.get(0))->value(), 0), type));
    quad->setRight(CSSPrimitiveValue::create(clampTo<double>(toInterpolableNumber(interpolableList.get(1))->value(), 0), type));
    quad->setBottom(CSSPrimitiveValue::create(clampTo<double>(toInterpolableNumber(interpolableList.get(2))->value(), 0), type));
    quad->setLeft(CSSPrimitiveValue::create(clampTo<double>(toInterpolableNumber(interpolableList.get(3))->value(), 0), type));
    return CSSBorderImageSliceValue::create(CSSPrimitiveValue::create(quad.release()), metadata.fill);
}

} // namespace

PassRefPtrWillBeRawPtr<ImageSliceStyleInterpolation> ImageSliceStyleInterpolation::maybeCreate(const CSSValue& start, const CSSValue& end, CSSPropertyID property)
{
    if (!start.isBorderImageSliceValue() || !end.isBorderImageSliceValue())
        return nullptr;

    Decomposition startDecompose(toCSSBorderImageSliceValue(start));
    Decomposition endDecompose(toCSSBorderImageSliceValue(end));
    if (!(startDecompose.metadata == endDecompose.metadata))
        return nullptr;

    return adoptRefWillBeNoop(new ImageSliceStyleInterpolation(
        startDecompose.interpolableValue.release(),
        endDecompose.interpolableValue.release(),
        property,
        startDecompose.metadata
    ));
}

void ImageSliceStyleInterpolation::apply(StyleResolverState& state) const
{
    StyleBuilder::applyProperty(m_id, state, compose(*m_cachedValue, m_metadata).get());
}

DEFINE_TRACE(ImageSliceStyleInterpolation)
{
    StyleInterpolation::trace(visitor);
}

} // namespace blink
