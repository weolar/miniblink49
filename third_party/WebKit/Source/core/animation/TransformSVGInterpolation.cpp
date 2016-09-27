// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/TransformSVGInterpolation.h"

namespace blink {

namespace {

PassOwnPtrWillBeRawPtr<InterpolableValue> translateToInterpolableValue(SVGTransform* transform)
{
    FloatPoint translate = transform->translate();
    OwnPtrWillBeRawPtr<InterpolableList> result = InterpolableList::create(2);
    result->set(0, InterpolableNumber::create(translate.x()));
    result->set(1, InterpolableNumber::create(translate.y()));
    return result.release();
}

PassRefPtrWillBeRawPtr<SVGTransform> translateFromInterpolableValue(const InterpolableValue& value)
{
    const InterpolableList& list = toInterpolableList(value);

    RefPtrWillBeRawPtr<SVGTransform> transform = SVGTransform::create(SVG_TRANSFORM_TRANSLATE);
    transform->setTranslate(
        toInterpolableNumber(list.get(0))->value(),
        toInterpolableNumber(list.get(1))->value());
    return transform.release();
}

PassOwnPtrWillBeRawPtr<InterpolableValue> scaleToInterpolableValue(SVGTransform* transform)
{
    FloatSize scale = transform->scale();
    OwnPtrWillBeRawPtr<InterpolableList> result = InterpolableList::create(2);
    result->set(0, InterpolableNumber::create(scale.width()));
    result->set(1, InterpolableNumber::create(scale.height()));
    return result.release();
}

PassRefPtrWillBeRawPtr<SVGTransform> scaleFromInterpolableValue(const InterpolableValue& value)
{
    const InterpolableList& list = toInterpolableList(value);

    RefPtrWillBeRawPtr<SVGTransform> transform = SVGTransform::create(SVG_TRANSFORM_SCALE);
    transform->setScale(
        toInterpolableNumber(list.get(0))->value(),
        toInterpolableNumber(list.get(1))->value());
    return transform.release();
}

PassOwnPtrWillBeRawPtr<InterpolableValue> rotateToInterpolableValue(SVGTransform* transform)
{
    FloatPoint rotationCenter = transform->rotationCenter();
    OwnPtrWillBeRawPtr<InterpolableList> result = InterpolableList::create(3);
    result->set(0, InterpolableNumber::create(transform->angle()));
    result->set(1, InterpolableNumber::create(rotationCenter.x()));
    result->set(2, InterpolableNumber::create(rotationCenter.y()));
    return result.release();
}

PassRefPtrWillBeRawPtr<SVGTransform> rotateFromInterpolableValue(const InterpolableValue& value)
{
    const InterpolableList& list = toInterpolableList(value);

    RefPtrWillBeRawPtr<SVGTransform> transform = SVGTransform::create(SVG_TRANSFORM_ROTATE);
    transform->setRotate(
        toInterpolableNumber(list.get(0))->value(),
        toInterpolableNumber(list.get(1))->value(),
        toInterpolableNumber(list.get(2))->value());
    return transform.release();
}

PassOwnPtrWillBeRawPtr<InterpolableValue> skewXToInterpolableValue(SVGTransform* transform)
{
    return InterpolableNumber::create(transform->angle());
}

PassRefPtrWillBeRawPtr<SVGTransform> skewXFromInterpolableValue(const InterpolableValue& value)
{
    RefPtrWillBeRawPtr<SVGTransform> transform = SVGTransform::create(SVG_TRANSFORM_SKEWX);
    transform->setSkewX(toInterpolableNumber(value).value());
    return transform.release();
}

PassOwnPtrWillBeRawPtr<InterpolableValue> skewYToInterpolableValue(SVGTransform* transform)
{
    return InterpolableNumber::create(transform->angle());
}

PassRefPtrWillBeRawPtr<SVGTransform> skewYFromInterpolableValue(const InterpolableValue& value)
{
    RefPtrWillBeRawPtr<SVGTransform> transform = SVGTransform::create(SVG_TRANSFORM_SKEWY);
    transform->setSkewY(toInterpolableNumber(value).value());
    return transform.release();
}

} // namespace

bool TransformSVGInterpolation::canCreateFrom(SVGTransform* start, SVGTransform* end)
{
    // TODO(ericwilligers): Support matrix interpolation.
    return start->transformType() == end->transformType()
        && start->transformType() != SVG_TRANSFORM_MATRIX;
}

PassOwnPtrWillBeRawPtr<InterpolableValue> TransformSVGInterpolation::toInterpolableValue(SVGTransform* transform, const SVGAnimatedPropertyBase*, SVGTransformType* transformTypePtr)
{
    SVGTransformType transformType = static_cast<SVGTransformType>(transform->transformType());
    if (transformTypePtr)
        *transformTypePtr = transformType;

    switch (transformType) {
    case SVG_TRANSFORM_TRANSLATE:
        return translateToInterpolableValue(transform);
    case SVG_TRANSFORM_SCALE:
        return scaleToInterpolableValue(transform);
    case SVG_TRANSFORM_ROTATE:
        return rotateToInterpolableValue(transform);
    case SVG_TRANSFORM_SKEWX:
        return skewXToInterpolableValue(transform);
    case SVG_TRANSFORM_SKEWY:
        return skewYToInterpolableValue(transform);
    case SVG_TRANSFORM_MATRIX:
    case SVG_TRANSFORM_UNKNOWN:
        ASSERT_NOT_REACHED();
    }
    ASSERT_NOT_REACHED();
    return nullptr;
}

PassRefPtrWillBeRawPtr<SVGTransform> TransformSVGInterpolation::fromInterpolableValue(const InterpolableValue& value, SVGTransformType transformType, const SVGElement*)
{
    switch (transformType) {
    case SVG_TRANSFORM_TRANSLATE:
        return translateFromInterpolableValue(value);
    case SVG_TRANSFORM_SCALE:
        return scaleFromInterpolableValue(value);
    case SVG_TRANSFORM_ROTATE:
        return rotateFromInterpolableValue(value);
    case SVG_TRANSFORM_SKEWX:
        return skewXFromInterpolableValue(value);
    case SVG_TRANSFORM_SKEWY:
        return skewYFromInterpolableValue(value);
    case SVG_TRANSFORM_MATRIX:
    case SVG_TRANSFORM_UNKNOWN:
        ASSERT_NOT_REACHED();
    }
    ASSERT_NOT_REACHED();
    return nullptr;
}

} // namespace blink
