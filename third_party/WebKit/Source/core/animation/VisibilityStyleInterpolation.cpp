#include "config.h"
#include "core/animation/VisibilityStyleInterpolation.h"

#include "core/css/CSSPrimitiveValue.h"
#include "core/css/resolver/StyleBuilder.h"

namespace blink {

bool VisibilityStyleInterpolation::canCreateFrom(const CSSValue& value)
{
    return value.isPrimitiveValue() && toCSSPrimitiveValue(value).isValueID();
}

bool VisibilityStyleInterpolation::isVisible(const CSSValue& value)
{
    return toCSSPrimitiveValue(value).getValueID() == CSSValueVisible;
}

PassOwnPtrWillBeRawPtr<InterpolableValue> VisibilityStyleInterpolation::visibilityToInterpolableValue(const CSSValue& value)
{
    ASSERT(value.isPrimitiveValue());
    const CSSPrimitiveValue& primitive = toCSSPrimitiveValue(value);
    ASSERT(primitive.isValueID());
    const CSSValueID valueID = primitive.getValueID();

    if (valueID == CSSValueVisible) {
        return InterpolableNumber::create(1.0);
    }
    return InterpolableNumber::create(0.0);
}

PassRefPtrWillBeRawPtr<CSSValue> VisibilityStyleInterpolation::interpolableValueToVisibility(InterpolableValue* value, CSSValueID notVisible)
{
    ASSERT(value->isNumber());
    InterpolableNumber* number = toInterpolableNumber(value);

    if (number->value() > 0)
        return CSSPrimitiveValue::createIdentifier(CSSValueVisible);
    return CSSPrimitiveValue::createIdentifier(notVisible);
}

void VisibilityStyleInterpolation::apply(StyleResolverState& state) const
{
    StyleBuilder::applyProperty(m_id, state, interpolableValueToVisibility(m_cachedValue.get(), m_notVisible).get());
}

DEFINE_TRACE(VisibilityStyleInterpolation)
{
    StyleInterpolation::trace(visitor);
}
}
