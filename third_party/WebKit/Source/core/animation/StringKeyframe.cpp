// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/animation/StringKeyframe.h"

#include "core/animation/AngleSVGInterpolation.h"
#include "core/animation/CSSValueInterpolationType.h"
#include "core/animation/ColorStyleInterpolation.h"
#include "core/animation/CompositorAnimations.h"
#include "core/animation/ConstantStyleInterpolation.h"
#include "core/animation/DefaultSVGInterpolation.h"
#include "core/animation/DeferredLegacyStyleInterpolation.h"
#include "core/animation/DoubleStyleInterpolation.h"
#include "core/animation/FilterStyleInterpolation.h"
#include "core/animation/ImageSliceStyleInterpolation.h"
#include "core/animation/ImageStyleInterpolation.h"
#include "core/animation/IntegerOptionalIntegerSVGInterpolation.h"
#include "core/animation/IntegerSVGInterpolation.h"
#include "core/animation/InterpolationType.h"
#include "core/animation/InvalidatableStyleInterpolation.h"
#include "core/animation/LegacyStyleInterpolation.h"
#include "core/animation/LengthBoxStyleInterpolation.h"
#include "core/animation/LengthPairStyleInterpolation.h"
#include "core/animation/LengthSVGInterpolation.h"
#include "core/animation/LengthStyleInterpolation.h"
#include "core/animation/ListSVGInterpolation.h"
#include "core/animation/ListStyleInterpolation.h"
#include "core/animation/NumberOptionalNumberSVGInterpolation.h"
#include "core/animation/NumberSVGInterpolation.h"
#include "core/animation/PathSVGInterpolation.h"
#include "core/animation/PointSVGInterpolation.h"
#include "core/animation/RectSVGInterpolation.h"
#include "core/animation/SVGStrokeDasharrayStyleInterpolation.h"
#include "core/animation/ShadowStyleInterpolation.h"
#include "core/animation/TransformSVGInterpolation.h"
#include "core/animation/VisibilityStyleInterpolation.h"
#include "core/animation/css/CSSAnimations.h"
#include "core/css/CSSPropertyMetadata.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/style/ComputedStyle.h"
#include "core/svg/SVGElement.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace blink {

StringKeyframe::StringKeyframe(const StringKeyframe& copyFrom)
    : Keyframe(copyFrom.m_offset, copyFrom.m_composite, copyFrom.m_easing)
    , m_propertySet(copyFrom.m_propertySet->mutableCopy())
    , m_svgPropertyMap(copyFrom.m_svgPropertyMap)
{
}

void StringKeyframe::setPropertyValue(CSSPropertyID property, const String& value, Element* element, StyleSheetContents* styleSheetContents)
{
    ASSERT(property != CSSPropertyInvalid);
    if (CSSAnimations::isAnimatableProperty(property))
        m_propertySet->setProperty(property, value, false, styleSheetContents);
}

void StringKeyframe::setPropertyValue(CSSPropertyID property, PassRefPtrWillBeRawPtr<CSSValue> value)
{
    ASSERT(property != CSSPropertyInvalid);
    ASSERT(CSSAnimations::isAnimatableProperty(property));
    m_propertySet->setProperty(property, value, false);
}

void StringKeyframe::setPropertyValue(const QualifiedName& attributeName, const String& value, Element* element)
{
    ASSERT(element->isSVGElement());
    m_svgPropertyMap.set(&attributeName, value);
}

PropertyHandleSet StringKeyframe::properties() const
{
    // This is not used in time-critical code, so we probably don't need to
    // worry about caching this result.
    PropertyHandleSet properties;
    for (unsigned i = 0; i < m_propertySet->propertyCount(); ++i)
        properties.add(PropertyHandle(m_propertySet->propertyAt(i).id()));

    for (const auto& key: m_svgPropertyMap.keys())
        properties.add(PropertyHandle(*key));

    return properties;
}

PassRefPtrWillBeRawPtr<Keyframe> StringKeyframe::clone() const
{
    return adoptRefWillBeNoop(new StringKeyframe(*this));
}

PassOwnPtrWillBeRawPtr<Keyframe::PropertySpecificKeyframe> StringKeyframe::createPropertySpecificKeyframe(PropertyHandle property) const
{
    if (property.isCSSProperty())
        return adoptPtrWillBeNoop(new CSSPropertySpecificKeyframe(offset(), &easing(), cssPropertyValue(property.cssProperty()), composite()));

    ASSERT(property.isSVGAttribute());
    return adoptPtrWillBeNoop(new SVGPropertySpecificKeyframe(offset(), &easing(), svgPropertyValue(*property.svgAttribute()), composite()));
}

DEFINE_TRACE(StringKeyframe)
{
    visitor->trace(m_propertySet);
    Keyframe::trace(visitor);
}

StringKeyframe::CSSPropertySpecificKeyframe::CSSPropertySpecificKeyframe(double offset, PassRefPtr<TimingFunction> easing, CSSValue* value, EffectModel::CompositeOperation op)
    : Keyframe::PropertySpecificKeyframe(offset, easing, op)
    , m_value(value)
{ }

StringKeyframe::CSSPropertySpecificKeyframe::CSSPropertySpecificKeyframe(double offset, PassRefPtr<TimingFunction> easing, CSSValue* value)
    : Keyframe::PropertySpecificKeyframe(offset, easing, EffectModel::CompositeReplace)
    , m_value(value)
{
    ASSERT(!isNull(m_offset));
}

void StringKeyframe::CSSPropertySpecificKeyframe::populateAnimatableValue(CSSPropertyID property, Element& element, const ComputedStyle* baseStyle) const
{
    if (!m_animatableValueCache && (baseStyle || !DeferredLegacyStyleInterpolation::interpolationRequiresStyleResolve(*m_value)))
        m_animatableValueCache = StyleResolver::createAnimatableValueSnapshot(element, baseStyle, property, m_value.get());
}

namespace {
InterpolationRange setRange(CSSPropertyID id)
{
    switch (id) {
    case CSSPropertyOrphans:
    case CSSPropertyWebkitColumnCount:
    case CSSPropertyWidows:
        return RangeRoundGreaterThanOrEqualToOne;
    case CSSPropertyWebkitColumnRuleWidth:
    case CSSPropertyZIndex:
        return RangeRound;
    case CSSPropertyFloodOpacity:
    case CSSPropertyStopOpacity:
    case CSSPropertyStrokeOpacity:
    case CSSPropertyShapeImageThreshold:
        return RangeZeroToOne;
    case CSSPropertyFillOpacity:
    case CSSPropertyOpacity:
        return RangeOpacityFIXME;
    case CSSPropertyStrokeMiterlimit:
        return RangeGreaterThanOrEqualToOne;
    case CSSPropertyFontSizeAdjust:
        return RangeNonNegative;
    default:
        ASSERT_NOT_REACHED();
        return RangeAll;
    }
}

const Vector<const InterpolationType*>* applicableTypesForProperty(CSSPropertyID property)
{
    using ApplicableTypesMap = HashMap<CSSPropertyID, const Vector<const InterpolationType*>*>;
    DEFINE_STATIC_LOCAL(ApplicableTypesMap, applicableTypesMap, ());
    auto entry = applicableTypesMap.find(property);
    if (entry != applicableTypesMap.end())
        return entry->value;

    // TODO(alancutter): Support all interpolable CSS properties here so we can stop falling back to the old StyleInterpolation implementation.
    if (CSSPropertyMetadata::isInterpolableProperty(property))
        return nullptr;

    auto applicableTypes = new Vector<const InterpolationType*>();
    applicableTypes->append(new CSSValueInterpolationType(property));
    applicableTypesMap.add(property, applicableTypes);
    return applicableTypes;
}

} // namespace

PassRefPtrWillBeRawPtr<Interpolation> StringKeyframe::CSSPropertySpecificKeyframe::maybeCreateInterpolation(PropertyHandle propertyHandle, Keyframe::PropertySpecificKeyframe& end, Element* element, const ComputedStyle* baseStyle) const
{
    CSSPropertyID property = propertyHandle.cssProperty();
    const Vector<const InterpolationType*>* applicableTypes = applicableTypesForProperty(property);
    if (applicableTypes)
        return InvalidatableStyleInterpolation::create(*applicableTypes, *this, toCSSPropertySpecificKeyframe(end));

    // TODO(alancutter): Remove the remainder of this function.

    // FIXME: Refactor this into a generic piece that lives in InterpolationEffect, and a template parameter specific converter.
    CSSValue* fromCSSValue = m_value.get();
    CSSValue* toCSSValue = toCSSPropertySpecificKeyframe(end).value();
    InterpolationRange range = RangeAll;
    bool fallBackToLegacy = false;

    // FIXME: Remove this flag once we can rely on legacy's behaviour being correct.
    bool forceDefaultInterpolation = false;

    // FIXME: Remove this check once neutral keyframes are implemented in StringKeyframes.
    if (!fromCSSValue || !toCSSValue)
        return DeferredLegacyStyleInterpolation::create(fromCSSValue, toCSSValue, property);

    ASSERT(fromCSSValue && toCSSValue);

    if (!CSSPropertyMetadata::isInterpolableProperty(property)) {
        if (fromCSSValue == toCSSValue)
            return ConstantStyleInterpolation::create(fromCSSValue, property);

        return nullptr;
    }

    switch (property) {
    case CSSPropertyLineHeight:
        if (LengthStyleInterpolation::canCreateFrom(*fromCSSValue) && LengthStyleInterpolation::canCreateFrom(*toCSSValue))
            return LengthStyleInterpolation::create(*fromCSSValue, *toCSSValue, property, RangeNonNegative);

        if (DoubleStyleInterpolation::canCreateFrom(*fromCSSValue) && DoubleStyleInterpolation::canCreateFrom(*toCSSValue))
            return DoubleStyleInterpolation::create(*fromCSSValue, *toCSSValue, property, CSSPrimitiveValue::CSS_NUMBER, RangeNonNegative);

        break;
    case CSSPropertyBorderBottomWidth:
    case CSSPropertyBorderLeftWidth:
    case CSSPropertyBorderRightWidth:
    case CSSPropertyBorderTopWidth:
    case CSSPropertyFlexBasis:
    case CSSPropertyFontSize:
    case CSSPropertyHeight:
    case CSSPropertyMaxHeight:
    case CSSPropertyMaxWidth:
    case CSSPropertyMinHeight:
    case CSSPropertyMinWidth:
    case CSSPropertyOutlineWidth:
    case CSSPropertyPaddingBottom:
    case CSSPropertyPaddingLeft:
    case CSSPropertyPaddingRight:
    case CSSPropertyPaddingTop:
    case CSSPropertyPerspective:
    case CSSPropertyR:
    case CSSPropertyRx:
    case CSSPropertyRy:
    case CSSPropertyShapeMargin:
    case CSSPropertyStrokeWidth:
    case CSSPropertyWebkitBorderHorizontalSpacing:
    case CSSPropertyWebkitBorderVerticalSpacing:
    case CSSPropertyWebkitColumnGap:
    case CSSPropertyWebkitColumnWidth:
    case CSSPropertyWidth:
        range = RangeNonNegative;
        // Fall through
    case CSSPropertyBaselineShift:
    case CSSPropertyBottom:
    case CSSPropertyCx:
    case CSSPropertyCy:
    case CSSPropertyLeft:
    case CSSPropertyLetterSpacing:
    case CSSPropertyMarginBottom:
    case CSSPropertyMarginLeft:
    case CSSPropertyMarginRight:
    case CSSPropertyMarginTop:
    case CSSPropertyMotionOffset:
    case CSSPropertyOutlineOffset:
    case CSSPropertyRight:
    case CSSPropertyStrokeDashoffset:
    case CSSPropertyTop:
    case CSSPropertyVerticalAlign:
    case CSSPropertyWordSpacing:
    case CSSPropertyWebkitColumnRuleWidth:
    case CSSPropertyWebkitPerspectiveOriginX:
    case CSSPropertyWebkitPerspectiveOriginY:
    case CSSPropertyWebkitTransformOriginX:
    case CSSPropertyWebkitTransformOriginY:
    case CSSPropertyWebkitTransformOriginZ:
    case CSSPropertyX:
    case CSSPropertyY:
        if (LengthStyleInterpolation::canCreateFrom(*fromCSSValue, property) && LengthStyleInterpolation::canCreateFrom(*toCSSValue, property))
            return LengthStyleInterpolation::create(*fromCSSValue, *toCSSValue, property, range);

        // FIXME: Handle keywords e.g. 'smaller', 'larger'.
        if (property == CSSPropertyFontSize)
            fallBackToLegacy = true;

        // FIXME: Handle keywords e.g. 'baseline', 'sub'.
        if (property == CSSPropertyBaselineShift)
            fallBackToLegacy = true;

        break;
    case CSSPropertyOrphans:
    case CSSPropertyWidows:
    case CSSPropertyZIndex:
    case CSSPropertyWebkitColumnCount:
    case CSSPropertyShapeImageThreshold:
    case CSSPropertyFillOpacity:
    case CSSPropertyFloodOpacity:
    case CSSPropertyFontSizeAdjust:
    case CSSPropertyOpacity:
    case CSSPropertyStopOpacity:
    case CSSPropertyStrokeOpacity:
    case CSSPropertyStrokeMiterlimit:
        if (DoubleStyleInterpolation::canCreateFrom(*fromCSSValue) && DoubleStyleInterpolation::canCreateFrom(*toCSSValue))
            return DoubleStyleInterpolation::create(*fromCSSValue, *toCSSValue, property, toCSSPrimitiveValue(fromCSSValue)->primitiveType(), setRange(property));
        break;

    case CSSPropertyMotionRotation: {
        RefPtrWillBeRawPtr<Interpolation> interpolation = DoubleStyleInterpolation::maybeCreateFromMotionRotation(*fromCSSValue, *toCSSValue, property);
        if (interpolation)
            return interpolation.release();
            break;
        }
    case CSSPropertyVisibility:
        if (VisibilityStyleInterpolation::canCreateFrom(*fromCSSValue) && VisibilityStyleInterpolation::canCreateFrom(*toCSSValue) && (VisibilityStyleInterpolation::isVisible(*fromCSSValue) || VisibilityStyleInterpolation::isVisible(*toCSSValue)))
            return VisibilityStyleInterpolation::create(*fromCSSValue, *toCSSValue, property);

        break;

    case CSSPropertyBackgroundColor:
    case CSSPropertyBorderBottomColor:
    case CSSPropertyBorderLeftColor:
    case CSSPropertyBorderRightColor:
    case CSSPropertyBorderTopColor:
    case CSSPropertyColor:
    case CSSPropertyFill:
    case CSSPropertyFloodColor:
    case CSSPropertyLightingColor:
    case CSSPropertyOutlineColor:
    case CSSPropertyStopColor:
    case CSSPropertyStroke:
    case CSSPropertyTextDecorationColor:
    case CSSPropertyWebkitColumnRuleColor:
    case CSSPropertyWebkitTextStrokeColor:
        {
            RefPtrWillBeRawPtr<Interpolation> interpolation = ColorStyleInterpolation::maybeCreateFromColor(*fromCSSValue, *toCSSValue, property);
            if (interpolation)
                return interpolation.release();

            // Current color should use LegacyStyleInterpolation
            if (ColorStyleInterpolation::shouldUseLegacyStyleInterpolation(*fromCSSValue, *toCSSValue))
                fallBackToLegacy = true;

            break;
        }

    case CSSPropertyBorderImageSource:
    case CSSPropertyListStyleImage:
    case CSSPropertyWebkitMaskBoxImageSource:
        if (fromCSSValue == toCSSValue)
            return ConstantStyleInterpolation::create(fromCSSValue, property);

        if (ImageStyleInterpolation::canCreateFrom(*fromCSSValue) && ImageStyleInterpolation::canCreateFrom(*toCSSValue))
            return ImageStyleInterpolation::create(*fromCSSValue, *toCSSValue, property);

        forceDefaultInterpolation = true;
        break;
    case CSSPropertyBorderBottomLeftRadius:
    case CSSPropertyBorderBottomRightRadius:
    case CSSPropertyBorderTopLeftRadius:
    case CSSPropertyBorderTopRightRadius:
        range = RangeNonNegative;
        // Fall through
    case CSSPropertyObjectPosition:
        if (LengthPairStyleInterpolation::canCreateFrom(*fromCSSValue) && LengthPairStyleInterpolation::canCreateFrom(*toCSSValue))
            return LengthPairStyleInterpolation::create(*fromCSSValue, *toCSSValue, property, range);
        break;

    case CSSPropertyPerspectiveOrigin:
    case CSSPropertyTransformOrigin: {
        RefPtrWillBeRawPtr<Interpolation> interpolation = ListStyleInterpolation<LengthStyleInterpolation>::maybeCreateFromList(*fromCSSValue, *toCSSValue, property, range);
        if (interpolation)
            return interpolation.release();

        // FIXME: Handle keywords: top, right, left, center, bottom
        fallBackToLegacy = true;

        break;
    }

    case CSSPropertyBoxShadow:
    case CSSPropertyTextShadow: {
        RefPtrWillBeRawPtr<Interpolation> interpolation = ListStyleInterpolation<ShadowStyleInterpolation>::maybeCreateFromList(*fromCSSValue, *toCSSValue, property);
        if (interpolation)
            return interpolation.release();

        // FIXME: AnimatableShadow incorrectly animates between inset and non-inset values so it will never indicate it needs default interpolation
        if (ShadowStyleInterpolation::usesDefaultStyleInterpolation(*fromCSSValue, *toCSSValue)) {
            forceDefaultInterpolation = true;
            break;
        }

        // FIXME: Handle interpolation from/to none, unspecified color values
        fallBackToLegacy = true;

        break;

    }

    case CSSPropertyClip: {
        if (LengthBoxStyleInterpolation::usesDefaultInterpolation(*fromCSSValue, *toCSSValue)) {
            forceDefaultInterpolation = true;
            break;
        }
        RefPtrWillBeRawPtr<Interpolation> interpolation = LengthBoxStyleInterpolation::maybeCreateFrom(*fromCSSValue, *toCSSValue, property);
        if (interpolation)
            return interpolation.release();
        break;
    }

    case CSSPropertyBorderImageSlice:
    case CSSPropertyWebkitMaskBoxImageSlice: {
        RefPtrWillBeRawPtr<Interpolation> interpolation = ImageSliceStyleInterpolation::maybeCreate(*fromCSSValue, *toCSSValue, property);
        if (interpolation)
            return interpolation.release();
        if (ImageSliceStyleInterpolation::usesDefaultInterpolation(*fromCSSValue, *toCSSValue))
            forceDefaultInterpolation = true;

        break;
    }

    case CSSPropertyStrokeDasharray: {
        RefPtrWillBeRawPtr<Interpolation> interpolation = SVGStrokeDasharrayStyleInterpolation::maybeCreate(*fromCSSValue, *toCSSValue, property);
        if (interpolation)
            return interpolation.release();

        break;
    }

    case CSSPropertyWebkitFilter: {
        RefPtrWillBeRawPtr<Interpolation> interpolation = FilterStyleInterpolation::maybeCreateList(*fromCSSValue, *toCSSValue, property);
        if (interpolation)
            return interpolation.release();

        // FIXME: Support drop shadow interpolation.
        fallBackToLegacy = true;
        break;
    }

    case CSSPropertyTranslate: {
        RefPtrWillBeRawPtr<Interpolation> interpolation = ListStyleInterpolation<LengthStyleInterpolation>::maybeCreateFromList(*fromCSSValue, *toCSSValue, property, range);
        if (interpolation)
            return interpolation.release();

        // TODO(soonm): Legacy mode is used when from and to cssvaluelist length does not match.
        fallBackToLegacy = true;
        break;
    }

    case CSSPropertyScale: {
        RefPtrWillBeRawPtr<Interpolation> interpolation = ListStyleInterpolation<DoubleStyleInterpolation>::maybeCreateFromList(*fromCSSValue, *toCSSValue, property, range);
        if (interpolation)
            return interpolation.release();

        // TODO(soonm): Legacy mode is used when from and to cssvaluelist length does not match.
        fallBackToLegacy = true;
        break;
    }

    default:
        // Fall back to LegacyStyleInterpolation.
        fallBackToLegacy = true;
        break;
    }

    if (fromCSSValue == toCSSValue)
        return ConstantStyleInterpolation::create(fromCSSValue, property);

    if (forceDefaultInterpolation)
        return nullptr;

    if (fromCSSValue->isCSSWideKeyword() || toCSSValue->isCSSWideKeyword())
        fallBackToLegacy = true;

    if (fallBackToLegacy) {
        if (DeferredLegacyStyleInterpolation::interpolationRequiresStyleResolve(*fromCSSValue) || DeferredLegacyStyleInterpolation::interpolationRequiresStyleResolve(*toCSSValue)) {
            // FIXME: Handle these cases outside of DeferredLegacyStyleInterpolation.
            return DeferredLegacyStyleInterpolation::create(fromCSSValue, toCSSValue, property);
        }

        // FIXME: Remove the use of AnimatableValues and Elements here.
        ASSERT(element);
        populateAnimatableValue(property, *element, baseStyle);
        end.populateAnimatableValue(property, *element, baseStyle);
        return LegacyStyleInterpolation::create(getAnimatableValue(), end.getAnimatableValue(), property);
    }

    ASSERT(AnimatableValue::usesDefaultInterpolation(
        StyleResolver::createAnimatableValueSnapshot(*element, baseStyle, property, fromCSSValue).get(),
        StyleResolver::createAnimatableValueSnapshot(*element, baseStyle, property, toCSSValue).get()));

    return nullptr;

}

PassOwnPtrWillBeRawPtr<Keyframe::PropertySpecificKeyframe> StringKeyframe::CSSPropertySpecificKeyframe::neutralKeyframe(double offset, PassRefPtr<TimingFunction> easing) const
{
    return adoptPtrWillBeNoop(new CSSPropertySpecificKeyframe(offset, easing, static_cast<CSSValue*>(0), EffectModel::CompositeAdd));
}

PassOwnPtrWillBeRawPtr<Keyframe::PropertySpecificKeyframe> StringKeyframe::CSSPropertySpecificKeyframe::cloneWithOffset(double offset) const
{
    Keyframe::PropertySpecificKeyframe* theClone = new CSSPropertySpecificKeyframe(offset, m_easing, m_value.get());
    toCSSPropertySpecificKeyframe(theClone)->m_animatableValueCache = m_animatableValueCache;
    return adoptPtrWillBeNoop(theClone);
}

DEFINE_TRACE(StringKeyframe::CSSPropertySpecificKeyframe)
{
    visitor->trace(m_value);
    visitor->trace(m_animatableValueCache);
    Keyframe::PropertySpecificKeyframe::trace(visitor);
}

SVGPropertySpecificKeyframe::SVGPropertySpecificKeyframe(double offset, PassRefPtr<TimingFunction> easing, const String& value, EffectModel::CompositeOperation op)
    : Keyframe::PropertySpecificKeyframe(offset, easing, op)
    , m_value(value)
{
}

SVGPropertySpecificKeyframe::SVGPropertySpecificKeyframe(double offset, PassRefPtr<TimingFunction> easing, const String& value)
    : Keyframe::PropertySpecificKeyframe(offset, easing, EffectModel::CompositeReplace)
    , m_value(value)
{
    ASSERT(!isNull(m_offset));
}

DEFINE_TRACE(StringKeyframe::SVGPropertySpecificKeyframe)
{
    Keyframe::PropertySpecificKeyframe::trace(visitor);
}

PassOwnPtrWillBeRawPtr<Keyframe::PropertySpecificKeyframe> SVGPropertySpecificKeyframe::cloneWithOffset(double offset) const
{
    return adoptPtrWillBeNoop(new SVGPropertySpecificKeyframe(offset, m_easing, m_value));
}

PassOwnPtrWillBeRawPtr<Keyframe::PropertySpecificKeyframe> SVGPropertySpecificKeyframe::neutralKeyframe(double offset, PassRefPtr<TimingFunction> easing) const
{
    return adoptPtrWillBeNoop(new SVGPropertySpecificKeyframe(offset, easing, "", EffectModel::CompositeAdd));
}

namespace {

PassRefPtrWillBeRawPtr<Interpolation> createSVGInterpolation(SVGPropertyBase* fromValue, SVGPropertyBase* toValue, SVGAnimatedPropertyBase* attribute)
{
    RefPtrWillBeRawPtr<Interpolation> interpolation = nullptr;
    ASSERT(fromValue->type() == toValue->type());
    switch (fromValue->type()) {
    case AnimatedAngle:
        if (AngleSVGInterpolation::canCreateFrom(fromValue) && AngleSVGInterpolation::canCreateFrom(toValue))
            return AngleSVGInterpolation::create(fromValue, toValue, attribute);
        break;
    case AnimatedInteger:
        return IntegerSVGInterpolation::create(fromValue, toValue, attribute);
    case AnimatedIntegerOptionalInteger: {
        int min = &attribute->attributeName() == &SVGNames::orderAttr ? 1 : 0;
        return IntegerOptionalIntegerSVGInterpolation::create(fromValue, toValue, attribute, min);
    }
    case AnimatedLength:
        return LengthSVGInterpolation::create(fromValue, toValue, attribute);
    case AnimatedLengthList:
        interpolation = ListSVGInterpolation<LengthSVGInterpolation>::maybeCreate(fromValue, toValue, attribute);
        break;
    case AnimatedNumber: {
        SVGNumberNegativeValuesMode negativeValuesMode = &attribute->attributeName() == &SVGNames::pathLengthAttr ? ForbidNegativeNumbers : AllowNegativeNumbers;
        return NumberSVGInterpolation::create(fromValue, toValue, attribute, negativeValuesMode);
    }
    case AnimatedNumberOptionalNumber:
        return NumberOptionalNumberSVGInterpolation::create(fromValue, toValue, attribute);
    case AnimatedNumberList:
        interpolation = ListSVGInterpolation<NumberSVGInterpolation>::maybeCreate(fromValue, toValue, attribute);
        break;
    case AnimatedPath:
        interpolation = PathSVGInterpolation::maybeCreate(fromValue, toValue, attribute);
        break;
    case AnimatedPoints:
        interpolation = ListSVGInterpolation<PointSVGInterpolation>::maybeCreate(fromValue, toValue, attribute);
        break;
    case AnimatedRect:
        return RectSVGInterpolation::create(fromValue, toValue, attribute);
    case AnimatedTransformList:
        interpolation = ListSVGInterpolation<TransformSVGInterpolation>::maybeCreate(fromValue, toValue, attribute);
        break;

    // TODO(ericwilligers): Support more animation types.
    default:
        break;
    }
    if (interpolation)
        return interpolation.release();

    return DefaultSVGInterpolation::create(fromValue, toValue, attribute);
}

} // namespace

PassRefPtrWillBeRawPtr<Interpolation> SVGPropertySpecificKeyframe::maybeCreateInterpolation(PropertyHandle propertyHandle, Keyframe::PropertySpecificKeyframe& end, Element* element, const ComputedStyle* baseStyle) const
{
    ASSERT(element);
    RefPtrWillBeRawPtr<SVGAnimatedPropertyBase> attribute = toSVGElement(element)->propertyFromAttribute(*propertyHandle.svgAttribute());
    ASSERT(attribute);

    RefPtrWillBeRawPtr<SVGPropertyBase> fromValue = attribute->currentValueBase()->cloneForAnimation(m_value);
    RefPtrWillBeRawPtr<SVGPropertyBase> toValue = attribute->currentValueBase()->cloneForAnimation(toSVGPropertySpecificKeyframe(end).value());

    if (!fromValue || !toValue)
        return nullptr;

    return createSVGInterpolation(fromValue.get(), toValue.get(), attribute.get());
}

} // namespace blink
