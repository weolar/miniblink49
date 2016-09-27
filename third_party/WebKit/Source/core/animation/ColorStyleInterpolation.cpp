// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/ColorStyleInterpolation.h"

#include "core/CSSValueKeywords.h"
#include "core/css/CSSPrimitiveValue.h"
#include "core/css/parser/CSSPropertyParser.h"
#include "core/css/resolver/StyleBuilder.h"
#include "core/layout/LayoutTheme.h"
#include "platform/graphics/Color.h"
#include "wtf/MathExtras.h"

#include <algorithm>

namespace blink {

bool ColorStyleInterpolation::canCreateFrom(const CSSValue& value)
{
    return value.isPrimitiveValue() && (toCSSPrimitiveValue(value).isValueID() || toCSSPrimitiveValue(value).isRGBColor());
}

PassOwnPtrWillBeRawPtr<InterpolableValue> ColorStyleInterpolation::colorToInterpolableValue(const CSSValue& value)
{
    ASSERT(value.isPrimitiveValue());
    const CSSPrimitiveValue& primitive = toCSSPrimitiveValue(value);
    RGBA32 color;
    if (primitive.isValueID()) {
        if (CSSPropertyParser::isSystemColor(primitive.getValueID())) {
            color = LayoutTheme::theme().systemColor(primitive.getValueID()).rgb();
        } else {
            Color colorFromID;
            colorFromID.setNamedColor(getValueName(primitive.getValueID()));
            color = colorFromID.rgb();
        }
    } else {
        color = primitive.getRGBA32Value();
    }

    int alpha = alphaChannel(color);

    OwnPtrWillBeRawPtr<InterpolableList> list = InterpolableList::create(4);
    list->set(0, InterpolableNumber::create(redChannel(color) * alpha));
    list->set(1, InterpolableNumber::create(greenChannel(color) * alpha));
    list->set(2, InterpolableNumber::create(blueChannel(color) * alpha));
    list->set(3, InterpolableNumber::create(alpha));

    return list->clone();
}

PassRefPtrWillBeRawPtr<CSSPrimitiveValue> ColorStyleInterpolation::interpolableValueToColor(const InterpolableValue& value)
{
    ASSERT(value.isList());
    const InterpolableList* list = toInterpolableList(&value);

    double alpha = toInterpolableNumber(list->get(3))->value();
    if (!alpha)
        return CSSPrimitiveValue::createColor(Color::transparent);

    // Clamping is inside makeRGBA.
    unsigned rgba = makeRGBA(
        round(toInterpolableNumber(list->get(0))->value() / alpha),
        round(toInterpolableNumber(list->get(1))->value() / alpha),
        round(toInterpolableNumber(list->get(2))->value() / alpha),
        round(alpha));

    return CSSPrimitiveValue::createColor(rgba);
}

void ColorStyleInterpolation::apply(StyleResolverState& state) const
{
    StyleBuilder::applyProperty(m_id, state, interpolableValueToColor(*m_cachedValue).get());
}

DEFINE_TRACE(ColorStyleInterpolation)
{
    StyleInterpolation::trace(visitor);
}

PassRefPtrWillBeRawPtr<ColorStyleInterpolation> ColorStyleInterpolation::maybeCreateFromColor(const CSSValue& start, const CSSValue& end, CSSPropertyID id)
{
    if (canCreateFrom(start) && !toCSSPrimitiveValue(start).colorIsDerivedFromElement() && canCreateFrom(end) && !toCSSPrimitiveValue(end).colorIsDerivedFromElement())
        return adoptRefWillBeNoop(new ColorStyleInterpolation(colorToInterpolableValue(start), colorToInterpolableValue(end), id));
    return nullptr;
}

bool ColorStyleInterpolation::shouldUseLegacyStyleInterpolation(const CSSValue& start, const CSSValue& end)
{
    if (ColorStyleInterpolation::canCreateFrom(start) && ColorStyleInterpolation::canCreateFrom(end)) {
        if (toCSSPrimitiveValue(start).colorIsDerivedFromElement() || toCSSPrimitiveValue(end).colorIsDerivedFromElement())
            return true;
    }
    return false;
}

}
