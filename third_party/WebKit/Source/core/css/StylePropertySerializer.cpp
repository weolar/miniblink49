/*
 * (C) 1999-2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2011 Research In Motion Limited. All rights reserved.
 * Copyright (C) 2013 Intel Corporation. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "config.h"
#include "core/css/StylePropertySerializer.h"

#include "core/CSSValueKeywords.h"
#include "core/StylePropertyShorthand.h"
#include "core/css/CSSPropertyMetadata.h"
#include "core/css/CSSValuePool.h"
#include "wtf/BitArray.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

static bool isInitialOrInherit(const String& value)
{
    DEFINE_STATIC_LOCAL(String, initial, ("initial"));
    DEFINE_STATIC_LOCAL(String, inherit, ("inherit"));
    return value.length() == 7 && (value == initial || value == inherit);
}

StylePropertySerializer::StylePropertySetForSerializer::StylePropertySetForSerializer(const StylePropertySet& properties)
    : m_propertySet(properties)
    , m_allIndex(m_propertySet.findPropertyIndex(CSSPropertyAll))
    , m_needToExpandAll(false)
{
    if (!hasAllProperty())
        return;

    StylePropertySet::PropertyReference allProperty = m_propertySet.propertyAt(m_allIndex);
    for (unsigned i = 0; i < m_propertySet.propertyCount(); ++i) {
        StylePropertySet::PropertyReference property = m_propertySet.propertyAt(i);
        if (CSSProperty::isAffectedByAllProperty(property.id())) {
            if (allProperty.isImportant() && !property.isImportant())
                continue;
            if (static_cast<unsigned>(m_allIndex) >= i)
                continue;
            if (property.value()->equals(*allProperty.value())
                && property.isImportant() == allProperty.isImportant())
                continue;
            m_needToExpandAll = true;
        }
        m_longhandPropertyUsed.set(property.id() - firstCSSProperty);
    }
}

unsigned StylePropertySerializer::StylePropertySetForSerializer::propertyCount() const
{
    if (!hasExpandedAllProperty())
        return m_propertySet.propertyCount();
    return lastCSSProperty - firstCSSProperty + 1;
}

StylePropertySerializer::PropertyValueForSerializer StylePropertySerializer::StylePropertySetForSerializer::propertyAt(unsigned index) const
{
    if (!hasExpandedAllProperty())
        return StylePropertySerializer::PropertyValueForSerializer(m_propertySet.propertyAt(index));

    CSSPropertyID propertyID = static_cast<CSSPropertyID>(index + firstCSSProperty);
    ASSERT(firstCSSProperty <= propertyID && propertyID <= lastCSSProperty);
    if (m_longhandPropertyUsed.get(index)) {
        int index = m_propertySet.findPropertyIndex(propertyID);
        ASSERT(index != -1);
        return StylePropertySerializer::PropertyValueForSerializer(m_propertySet.propertyAt(index));
    }

    StylePropertySet::PropertyReference property = m_propertySet.propertyAt(m_allIndex);
    return StylePropertySerializer::PropertyValueForSerializer(propertyID, property.value(), property.isImportant());
}

bool StylePropertySerializer::StylePropertySetForSerializer::shouldProcessPropertyAt(unsigned index) const
{
    // StylePropertySet has all valid longhands. We should process.
    if (!hasAllProperty())
        return true;

    // If all is not expanded, we need to process "all" and properties which
    // are not overwritten by "all".
    if (!m_needToExpandAll) {
        StylePropertySet::PropertyReference property = m_propertySet.propertyAt(index);
        if (property.id() == CSSPropertyAll || !CSSProperty::isAffectedByAllProperty(property.id()))
            return true;
        return m_longhandPropertyUsed.get(property.id() - firstCSSProperty);
    }

    CSSPropertyID propertyID = static_cast<CSSPropertyID>(index + firstCSSProperty);
    ASSERT(firstCSSProperty <= propertyID && propertyID <= lastCSSProperty);

    // Since "all" is expanded, we don't need to process "all".
    // We should not process expanded shorthands (e.g. font, background,
    // and so on) either.
    if (isShorthandProperty(propertyID) || propertyID == CSSPropertyAll)
        return false;

    // The all property is a shorthand that resets all CSS properties except
    // direction and unicode-bidi. It only accepts the CSS-wide keywords.
    // c.f. http://dev.w3.org/csswg/css-cascade/#all-shorthand
    if (!CSSProperty::isAffectedByAllProperty(propertyID))
        return m_longhandPropertyUsed.get(index);

    return true;
}

int StylePropertySerializer::StylePropertySetForSerializer::findPropertyIndex(CSSPropertyID propertyID) const
{
    if (!hasExpandedAllProperty())
        return m_propertySet.findPropertyIndex(propertyID);
    return propertyID - firstCSSProperty;
}

const CSSValue* StylePropertySerializer::StylePropertySetForSerializer::getPropertyCSSValue(CSSPropertyID propertyID) const
{
    int index = findPropertyIndex(propertyID);
    if (index == -1)
        return 0;
    StylePropertySerializer::PropertyValueForSerializer value = propertyAt(index);
    return value.value();
}

String StylePropertySerializer::StylePropertySetForSerializer::getPropertyValue(CSSPropertyID propertyID) const
{
    if (!hasExpandedAllProperty())
        return m_propertySet.getPropertyValue(propertyID);

    const CSSValue* value = getPropertyCSSValue(propertyID);
    if (!value)
        return String();
    return value->cssText();
}

bool StylePropertySerializer::StylePropertySetForSerializer::isPropertyImplicit(CSSPropertyID propertyID) const
{
    int index = findPropertyIndex(propertyID);
    if (index == -1)
        return false;
    StylePropertySerializer::PropertyValueForSerializer value = propertyAt(index);
    return value.isImplicit();
}

bool StylePropertySerializer::StylePropertySetForSerializer::propertyIsImportant(CSSPropertyID propertyID) const
{
    int index = findPropertyIndex(propertyID);
    if (index == -1)
        return false;
    StylePropertySerializer::PropertyValueForSerializer value = propertyAt(index);
    return value.isImportant();
}

StylePropertySerializer::StylePropertySerializer(const StylePropertySet& properties)
    : m_propertySet(properties)
{
}

String StylePropertySerializer::getPropertyText(CSSPropertyID propertyID, const String& value, bool isImportant, bool isNotFirstDecl) const
{
    StringBuilder result;
    if (isNotFirstDecl)
        result.append(' ');
    result.append(getPropertyName(propertyID));
    result.appendLiteral(": ");
    result.append(value);
    if (isImportant)
        result.appendLiteral(" !important");
    result.append(';');
    return result.toString();
}

String StylePropertySerializer::asText() const
{
    StringBuilder result;

    BitArray<numCSSProperties> shorthandPropertyUsed;
    BitArray<numCSSProperties> shorthandPropertyAppeared;

    unsigned size = m_propertySet.propertyCount();
    unsigned numDecls = 0;
    for (unsigned n = 0; n < size; ++n) {
        if (!m_propertySet.shouldProcessPropertyAt(n))
            continue;

        StylePropertySerializer::PropertyValueForSerializer property = m_propertySet.propertyAt(n);
        CSSPropertyID propertyID = property.id();
        // Only enabled properties should be part of the style.
        ASSERT(CSSPropertyMetadata::isEnabledProperty(propertyID));
        CSSPropertyID shorthandPropertyID = CSSPropertyInvalid;
        CSSPropertyID borderFallbackShorthandProperty = CSSPropertyInvalid;
        String value;
        ASSERT(!isShorthandProperty(propertyID));

        switch (propertyID) {
        case CSSPropertyBackgroundAttachment:
        case CSSPropertyBackgroundClip:
        case CSSPropertyBackgroundColor:
        case CSSPropertyBackgroundImage:
        case CSSPropertyBackgroundOrigin:
        case CSSPropertyBackgroundPositionX:
        case CSSPropertyBackgroundPositionY:
        case CSSPropertyBackgroundSize:
        case CSSPropertyBackgroundRepeatX:
        case CSSPropertyBackgroundRepeatY:
            shorthandPropertyAppeared.set(CSSPropertyBackground - firstCSSProperty);
            continue;
        case CSSPropertyBorderTopWidth:
        case CSSPropertyBorderRightWidth:
        case CSSPropertyBorderBottomWidth:
        case CSSPropertyBorderLeftWidth:
            if (!borderFallbackShorthandProperty)
                borderFallbackShorthandProperty = CSSPropertyBorderWidth;
        case CSSPropertyBorderTopStyle:
        case CSSPropertyBorderRightStyle:
        case CSSPropertyBorderBottomStyle:
        case CSSPropertyBorderLeftStyle:
            if (!borderFallbackShorthandProperty)
                borderFallbackShorthandProperty = CSSPropertyBorderStyle;
        case CSSPropertyBorderTopColor:
        case CSSPropertyBorderRightColor:
        case CSSPropertyBorderBottomColor:
        case CSSPropertyBorderLeftColor:
            if (!borderFallbackShorthandProperty)
                borderFallbackShorthandProperty = CSSPropertyBorderColor;

            // FIXME: Deal with cases where only some of border-(top|right|bottom|left) are specified.
            if (!shorthandPropertyAppeared.get(CSSPropertyBorder - firstCSSProperty)) {
                value = borderPropertyValue(ReturnNullOnUncommonValues);
                if (value.isNull())
                    shorthandPropertyAppeared.set(CSSPropertyBorder - firstCSSProperty);
                else
                    shorthandPropertyID = CSSPropertyBorder;
            } else if (shorthandPropertyUsed.get(CSSPropertyBorder - firstCSSProperty))
                shorthandPropertyID = CSSPropertyBorder;
            if (!shorthandPropertyID)
                shorthandPropertyID = borderFallbackShorthandProperty;
            break;
        case CSSPropertyBorderTopLeftRadius:
        case CSSPropertyBorderTopRightRadius:
        case CSSPropertyBorderBottomLeftRadius:
        case CSSPropertyBorderBottomRightRadius:
            shorthandPropertyID = CSSPropertyBorderRadius;
            break;
        case CSSPropertyWebkitBorderHorizontalSpacing:
        case CSSPropertyWebkitBorderVerticalSpacing:
            shorthandPropertyID = CSSPropertyBorderSpacing;
            break;
        case CSSPropertyFontFamily:
        case CSSPropertyLineHeight:
        case CSSPropertyFontSize:
        case CSSPropertyFontStretch:
        case CSSPropertyFontStyle:
        case CSSPropertyFontVariant:
        case CSSPropertyFontWeight:
            // Don't use CSSPropertyFont because old UAs can't recognize them but are important for editing.
            break;
        case CSSPropertyListStyleType:
        case CSSPropertyListStylePosition:
        case CSSPropertyListStyleImage:
            shorthandPropertyID = CSSPropertyListStyle;
            break;
        case CSSPropertyMarginTop:
        case CSSPropertyMarginRight:
        case CSSPropertyMarginBottom:
        case CSSPropertyMarginLeft:
            shorthandPropertyID = CSSPropertyMargin;
            break;
        case CSSPropertyMotionPath:
        case CSSPropertyMotionOffset:
        case CSSPropertyMotionRotation:
            shorthandPropertyID = CSSPropertyMotion;
            break;
        case CSSPropertyOutlineWidth:
        case CSSPropertyOutlineStyle:
        case CSSPropertyOutlineColor:
            shorthandPropertyID = CSSPropertyOutline;
            break;
        case CSSPropertyOverflowX:
        case CSSPropertyOverflowY:
            shorthandPropertyID = CSSPropertyOverflow;
            break;
        case CSSPropertyPaddingTop:
        case CSSPropertyPaddingRight:
        case CSSPropertyPaddingBottom:
        case CSSPropertyPaddingLeft:
            shorthandPropertyID = CSSPropertyPadding;
            break;
        case CSSPropertyTransitionProperty:
        case CSSPropertyTransitionDuration:
        case CSSPropertyTransitionTimingFunction:
        case CSSPropertyTransitionDelay:
            shorthandPropertyID = CSSPropertyTransition;
            break;
        case CSSPropertyAnimationName:
        case CSSPropertyAnimationDuration:
        case CSSPropertyAnimationTimingFunction:
        case CSSPropertyAnimationDelay:
        case CSSPropertyAnimationIterationCount:
        case CSSPropertyAnimationDirection:
        case CSSPropertyAnimationFillMode:
        case CSSPropertyAnimationPlayState:
            shorthandPropertyID = CSSPropertyAnimation;
            break;
        case CSSPropertyFlexDirection:
        case CSSPropertyFlexWrap:
            shorthandPropertyID = CSSPropertyFlexFlow;
            break;
        case CSSPropertyFlexBasis:
        case CSSPropertyFlexGrow:
        case CSSPropertyFlexShrink:
            shorthandPropertyID = CSSPropertyFlex;
            break;
        case CSSPropertyWebkitMaskPositionX:
        case CSSPropertyWebkitMaskPositionY:
        case CSSPropertyWebkitMaskRepeatX:
        case CSSPropertyWebkitMaskRepeatY:
        case CSSPropertyWebkitMaskImage:
        case CSSPropertyWebkitMaskRepeat:
        case CSSPropertyWebkitMaskPosition:
        case CSSPropertyWebkitMaskClip:
        case CSSPropertyWebkitMaskOrigin:
            shorthandPropertyID = CSSPropertyWebkitMask;
            break;
        case CSSPropertyAll:
            result.append(getPropertyText(propertyID, property.value()->cssText(), property.isImportant(), numDecls++));
            continue;
        default:
            break;
        }

        unsigned shortPropertyIndex = shorthandPropertyID - firstCSSProperty;
        if (shorthandPropertyID) {
            if (shorthandPropertyUsed.get(shortPropertyIndex))
                continue;
            if (!shorthandPropertyAppeared.get(shortPropertyIndex) && value.isNull())
                value = m_propertySet.getPropertyValue(shorthandPropertyID);
            shorthandPropertyAppeared.set(shortPropertyIndex);
        }

        if (!value.isNull()) {
            if (shorthandPropertyID) {
                propertyID = shorthandPropertyID;
                shorthandPropertyUsed.set(shortPropertyIndex);
            }
        } else {
            // We should not show "initial" when the "initial" is implicit.
            // If explicit "initial", we need to show.
            if (property.value()->isImplicitInitialValue())
                continue;
            value = property.value()->cssText();
        }

        result.append(getPropertyText(propertyID, value, property.isImportant(), numDecls++));
    }

    if (shorthandPropertyAppeared.get(CSSPropertyBackground - firstCSSProperty))
        appendBackgroundPropertyAsText(result, numDecls);

    ASSERT(!numDecls ^ !result.isEmpty());
    return result.toString();
}

String StylePropertySerializer::getPropertyValue(CSSPropertyID propertyID) const
{
    // Shorthand and 4-values properties
    switch (propertyID) {
    case CSSPropertyAnimation:
        return getLayeredShorthandValue(animationShorthand());
    case CSSPropertyBorderSpacing:
        return borderSpacingValue(borderSpacingShorthand());
    case CSSPropertyBackgroundPosition:
        return getLayeredShorthandValue(backgroundPositionShorthand());
    case CSSPropertyBackgroundRepeat:
        return backgroundRepeatPropertyValue();
    case CSSPropertyBackground:
        return getLayeredShorthandValue(backgroundShorthand());
    case CSSPropertyBorder:
        return borderPropertyValue(OmitUncommonValues);
    case CSSPropertyBorderTop:
        return getShorthandValue(borderTopShorthand());
    case CSSPropertyBorderRight:
        return getShorthandValue(borderRightShorthand());
    case CSSPropertyBorderBottom:
        return getShorthandValue(borderBottomShorthand());
    case CSSPropertyBorderLeft:
        return getShorthandValue(borderLeftShorthand());
    case CSSPropertyOutline:
        return getShorthandValue(outlineShorthand());
    case CSSPropertyBorderColor:
        return get4Values(borderColorShorthand());
    case CSSPropertyBorderWidth:
        return get4Values(borderWidthShorthand());
    case CSSPropertyBorderStyle:
        return get4Values(borderStyleShorthand());
    case CSSPropertyWebkitColumnRule:
        return getShorthandValue(webkitColumnRuleShorthand());
    case CSSPropertyWebkitColumns:
        return getShorthandValue(webkitColumnsShorthand());
    case CSSPropertyFlex:
        return getShorthandValue(flexShorthand());
    case CSSPropertyFlexFlow:
        return getShorthandValue(flexFlowShorthand());
    case CSSPropertyGridColumn:
        return getShorthandValue(gridColumnShorthand(), " / ");
    case CSSPropertyGridRow:
        return getShorthandValue(gridRowShorthand(), " / ");
    case CSSPropertyGridArea:
        return getShorthandValue(gridAreaShorthand(), " / ");
    case CSSPropertyFont:
        return fontValue();
    case CSSPropertyMargin:
        return get4Values(marginShorthand());
    case CSSPropertyMotion:
        return getShorthandValue(motionShorthand());
    case CSSPropertyWebkitMarginCollapse:
        return getShorthandValue(webkitMarginCollapseShorthand());
    case CSSPropertyOverflow:
        return getCommonValue(overflowShorthand());
    case CSSPropertyPadding:
        return get4Values(paddingShorthand());
    case CSSPropertyTransition:
        return getLayeredShorthandValue(transitionShorthand());
    case CSSPropertyListStyle:
        return getShorthandValue(listStyleShorthand());
    case CSSPropertyWebkitMaskPosition:
        return getLayeredShorthandValue(webkitMaskPositionShorthand());
    case CSSPropertyWebkitMaskRepeat:
        return getLayeredShorthandValue(webkitMaskRepeatShorthand());
    case CSSPropertyWebkitMask:
        return getLayeredShorthandValue(webkitMaskShorthand());
    case CSSPropertyWebkitTextEmphasis:
        return getShorthandValue(webkitTextEmphasisShorthand());
    case CSSPropertyWebkitTextStroke:
        return getShorthandValue(webkitTextStrokeShorthand());
    case CSSPropertyMarker: {
        if (const CSSValue* value = m_propertySet.getPropertyCSSValue(CSSPropertyMarkerStart))
            return value->cssText();
        return String();
    }
    case CSSPropertyBorderRadius:
        return get4Values(borderRadiusShorthand());
    default:
        return String();
    }
}

String StylePropertySerializer::borderSpacingValue(const StylePropertyShorthand& shorthand) const
{
    const CSSValue* horizontalValue = m_propertySet.getPropertyCSSValue(shorthand.properties()[0]);
    const CSSValue* verticalValue = m_propertySet.getPropertyCSSValue(shorthand.properties()[1]);

    // While standard border-spacing property does not allow specifying border-spacing-vertical without
    // specifying border-spacing-horizontal <http://www.w3.org/TR/CSS21/tables.html#separated-borders>,
    // -webkit-border-spacing-vertical can be set without -webkit-border-spacing-horizontal.
    if (!horizontalValue || !verticalValue)
        return String();

    String horizontalValueCSSText = horizontalValue->cssText();
    String verticalValueCSSText = verticalValue->cssText();
    if (horizontalValueCSSText == verticalValueCSSText)
        return horizontalValueCSSText;
    return horizontalValueCSSText + ' ' + verticalValueCSSText;
}

void StylePropertySerializer::appendFontLonghandValueIfNotNormal(CSSPropertyID propertyID, StringBuilder& result, String& commonValue) const
{
    int foundPropertyIndex = m_propertySet.findPropertyIndex(propertyID);
    if (foundPropertyIndex == -1)
        return;

    const CSSValue* val = m_propertySet.propertyAt(foundPropertyIndex).value();
    if (val->isPrimitiveValue() && toCSSPrimitiveValue(val)->getValueID() == CSSValueNormal) {
        commonValue = String();
        return;
    }

    char prefix = '\0';
    switch (propertyID) {
    case CSSPropertyFontStyle:
        break; // No prefix.
    case CSSPropertyFontFamily:
    case CSSPropertyFontStretch:
    case CSSPropertyFontVariant:
    case CSSPropertyFontWeight:
        prefix = ' ';
        break;
    case CSSPropertyLineHeight:
        prefix = '/';
        break;
    default:
        ASSERT_NOT_REACHED();
    }

    if (prefix && !result.isEmpty())
        result.append(prefix);
    String value = m_propertySet.propertyAt(foundPropertyIndex).value()->cssText();
    result.append(value);
    if (!commonValue.isNull() && commonValue != value)
        commonValue = String();
}

String StylePropertySerializer::fontValue() const
{
    int fontSizePropertyIndex = m_propertySet.findPropertyIndex(CSSPropertyFontSize);
    int fontFamilyPropertyIndex = m_propertySet.findPropertyIndex(CSSPropertyFontFamily);
    if (fontSizePropertyIndex == -1 || fontFamilyPropertyIndex == -1)
        return emptyString();

    PropertyValueForSerializer fontSizeProperty = m_propertySet.propertyAt(fontSizePropertyIndex);
    PropertyValueForSerializer fontFamilyProperty = m_propertySet.propertyAt(fontFamilyPropertyIndex);

    String commonValue = fontSizeProperty.value()->cssText();
    StringBuilder result;
    appendFontLonghandValueIfNotNormal(CSSPropertyFontStyle, result, commonValue);
    appendFontLonghandValueIfNotNormal(CSSPropertyFontVariant, result, commonValue);
    appendFontLonghandValueIfNotNormal(CSSPropertyFontWeight, result, commonValue);
    appendFontLonghandValueIfNotNormal(CSSPropertyFontStretch, result, commonValue);
    if (!result.isEmpty())
        result.append(' ');
    result.append(fontSizeProperty.value()->cssText());
    appendFontLonghandValueIfNotNormal(CSSPropertyLineHeight, result, commonValue);
    if (!result.isEmpty())
        result.append(' ');
    result.append(fontFamilyProperty.value()->cssText());
    if (isInitialOrInherit(commonValue))
        return commonValue;
    return result.toString();
}

String StylePropertySerializer::get4Values(const StylePropertyShorthand& shorthand) const
{
    // Assume the properties are in the usual order top, right, bottom, left.
    int topValueIndex = m_propertySet.findPropertyIndex(shorthand.properties()[0]);
    int rightValueIndex = m_propertySet.findPropertyIndex(shorthand.properties()[1]);
    int bottomValueIndex = m_propertySet.findPropertyIndex(shorthand.properties()[2]);
    int leftValueIndex = m_propertySet.findPropertyIndex(shorthand.properties()[3]);

    if (topValueIndex == -1 || rightValueIndex == -1 || bottomValueIndex == -1 || leftValueIndex == -1)
        return String();

    PropertyValueForSerializer top = m_propertySet.propertyAt(topValueIndex);
    PropertyValueForSerializer right = m_propertySet.propertyAt(rightValueIndex);
    PropertyValueForSerializer bottom = m_propertySet.propertyAt(bottomValueIndex);
    PropertyValueForSerializer left = m_propertySet.propertyAt(leftValueIndex);

    // All 4 properties must be specified.
    if (!top.value() || !right.value() || !bottom.value() || !left.value())
        return String();

    if (top.isImportant() != right.isImportant() || right.isImportant() != bottom.isImportant() || bottom.isImportant() != left.isImportant())
        return String();

    if (top.isInherited() && right.isInherited() && bottom.isInherited() && left.isInherited())
        return getValueName(CSSValueInherit);

    unsigned numInitial = top.value()->isInitialValue() + right.value()->isInitialValue() + bottom.value()->isInitialValue() + left.value()->isInitialValue();
    if (numInitial == 4)
        return getValueName(CSSValueInitial);
    if (numInitial > 0)
        return String();

    bool showLeft = !right.value()->equals(*left.value());
    bool showBottom = !top.value()->equals(*bottom.value()) || showLeft;
    bool showRight = !top.value()->equals(*right.value()) || showBottom;

    StringBuilder result;
    result.append(top.value()->cssText());
    if (showRight) {
        result.append(' ');
        result.append(right.value()->cssText());
    }
    if (showBottom) {
        result.append(' ');
        result.append(bottom.value()->cssText());
    }
    if (showLeft) {
        result.append(' ');
        result.append(left.value()->cssText());
    }
    return result.toString();
}

String StylePropertySerializer::getLayeredShorthandValue(const StylePropertyShorthand& shorthand) const
{
    const unsigned size = shorthand.length();

    // Begin by collecting the properties into a vector.
    WillBeHeapVector<const CSSValue*> values(size);
    // If the below loop succeeds, there should always be at minimum 1 layer.
    size_t numLayers = 1U;

    for (size_t i = 0; i < size; i++) {
        values[i] = m_propertySet.getPropertyCSSValue(shorthand.properties()[i]);
        // A shorthand is not available if getPropertyCSSValue didn't resolve to anything.
        if (!values[i])
            return String();
        if (values[i]->isBaseValueList()) {
            const CSSValueList* valueList = toCSSValueList(values[i]);
            numLayers = std::max(numLayers, valueList->length());
        }
    }

    StringBuilder result;
    // Tracks whether or not all the values are initial or all the values are inherit.
    // Start out assuming there is a common value. It will get set to false below if there isn't one.
    bool hasCommonValue = true;
    const CSSValue* commonValue = nullptr;

    // Now stitch the properties together. Implicit initial values are flagged as such and
    // can safely be omitted.
    for (size_t layer = 0; layer < numLayers; layer++) {
        StringBuilder layerResult;
        bool useRepeatXShorthand = false;
        bool useRepeatYShorthand = false;
        bool useSingleWordShorthand = false;
        bool foundPositionYCSSProperty = false;

        for (unsigned propertyIndex = 0; propertyIndex < size; propertyIndex++) {
            const CSSValue* value = nullptr;
            CSSPropertyID property = shorthand.properties()[propertyIndex];

            // Get a CSSValue for this property and layer.
            if (values[propertyIndex]->isBaseValueList()) {
                // Might return 0 if there is not an item for this layer for this property.
                value = toCSSValueList(values[propertyIndex])->itemWithBoundsCheck(layer);
            } else if (layer == 0 || (layer != numLayers - 1 && property == CSSPropertyBackgroundColor)) {
                // Singletons except background color belong in the 0th layer.
                // Background color belongs in the last layer.
                value = values[propertyIndex];
            }
            // No point proceeding if there's not a value to look at.
            if (!value)
                continue;

            // Special case for background-repeat.
            if ((propertyIndex < size - 1 && m_propertySet.isPropertyImplicit(property))
                && (property == CSSPropertyBackgroundRepeatX || property == CSSPropertyWebkitMaskRepeatX)) {
                ASSERT(shorthand.properties()[propertyIndex + 1] == CSSPropertyBackgroundRepeatY
                    || shorthand.properties()[propertyIndex + 1] == CSSPropertyWebkitMaskRepeatY);
                const CSSValue* yValue = values[propertyIndex + 1]->isValueList() ?
                    toCSSValueList(values[propertyIndex + 1])->item(layer) : values[propertyIndex + 1];


                // FIXME: At some point we need to fix this code to avoid returning an invalid shorthand,
                // since some longhand combinations are not serializable into a single shorthand.
                if (!value->isPrimitiveValue() || !yValue->isPrimitiveValue())
                    continue;

                CSSValueID xId = toCSSPrimitiveValue(value)->getValueID();
                CSSValueID yId = toCSSPrimitiveValue(yValue)->getValueID();
                // Maybe advance propertyIndex to look at the next CSSValue in the list for the checks below.
                if (xId == yId) {
                    useSingleWordShorthand = true;
                    property = shorthand.properties()[++propertyIndex];
                } else if (xId == CSSValueRepeat && yId == CSSValueNoRepeat) {
                    useRepeatXShorthand = true;
                    property = shorthand.properties()[++propertyIndex];
                } else if (xId == CSSValueNoRepeat && yId == CSSValueRepeat) {
                    useRepeatYShorthand = true;
                    property = shorthand.properties()[++propertyIndex];
                }
            }

            if (!(value->isInitialValue() && toCSSInitialValue(value)->isImplicit())) {
                if (property == CSSPropertyBackgroundSize || property == CSSPropertyWebkitMaskSize) {
                    if (foundPositionYCSSProperty)
                        layerResult.appendLiteral(" / ");
                    else
                        continue;
                } else if (!layerResult.isEmpty()) {
                    // Do this second to avoid ending up with an extra space in the output if we hit the continue above.
                    layerResult.append(' ');
                }

                if (useRepeatXShorthand) {
                    useRepeatXShorthand = false;
                    layerResult.append(getValueName(CSSValueRepeatX));
                } else if (useRepeatYShorthand) {
                    useRepeatYShorthand = false;
                    layerResult.append(getValueName(CSSValueRepeatY));
                } else {
                    if (useSingleWordShorthand)
                        useSingleWordShorthand = false;
                    layerResult.append(value->cssText());
                }
                if (property == CSSPropertyBackgroundPositionY || property == CSSPropertyWebkitMaskPositionY) {
                    foundPositionYCSSProperty = true;
                    // background-position is a special case. If only the first offset is specified,
                    // the second one defaults to "center", not the same value.
                    if (hasCommonValue && !value->isInitialValue() && !value->isInheritedValue())
                        hasCommonValue = false;
                }
            }

            if (hasCommonValue && !commonValue)
                commonValue = value;
            else if (!value->equals(*commonValue))
                hasCommonValue = false;
        }
        if (!layerResult.isEmpty()) {
            if (!result.isEmpty())
                result.appendLiteral(", ");
            result.append(layerResult);
        }
    }

    if (hasCommonValue && (commonValue->isInitialValue() || commonValue->isInheritedValue()))
        return commonValue->cssText();

    return result.toString();
}

String StylePropertySerializer::getShorthandValue(const StylePropertyShorthand& shorthand, String separator) const
{
    String commonValue;
    StringBuilder result;
    for (unsigned i = 0; i < shorthand.length(); ++i) {
        const CSSValue* value = m_propertySet.getPropertyCSSValue(shorthand.properties()[i]);
        if (!value)
            return String();
        String valueText = value->cssText();
        if (!i)
            commonValue = valueText;
        else if (!commonValue.isNull() && commonValue != valueText)
            commonValue = String();
        if (value->isInitialValue())
            continue;
        if (!result.isEmpty())
            result.append(separator);
        result.append(valueText);
    }
    if (isInitialOrInherit(commonValue))
        return commonValue;
    return result.toString();
}

// only returns a non-null value if all properties have the same, non-null value
String StylePropertySerializer::getCommonValue(const StylePropertyShorthand& shorthand) const
{
    String res;
    bool lastPropertyWasImportant = false;
    for (unsigned i = 0; i < shorthand.length(); ++i) {
        const CSSValue* value = m_propertySet.getPropertyCSSValue(shorthand.properties()[i]);
        // FIXME: CSSInitialValue::cssText should generate the right value.
        if (!value)
            return String();
        String text = value->cssText();
        if (text.isNull())
            return String();
        if (res.isNull())
            res = text;
        else if (res != text)
            return String();

        bool currentPropertyIsImportant = m_propertySet.propertyIsImportant(shorthand.properties()[i]);
        if (i && lastPropertyWasImportant != currentPropertyIsImportant)
            return String();
        lastPropertyWasImportant = currentPropertyIsImportant;
    }
    return res;
}

String StylePropertySerializer::borderPropertyValue(CommonValueMode valueMode) const
{
    const StylePropertyShorthand properties[3] = { borderWidthShorthand(), borderStyleShorthand(), borderColorShorthand() };
    String commonValue;
    StringBuilder result;
    for (size_t i = 0; i < WTF_ARRAY_LENGTH(properties); ++i) {
        String value = getCommonValue(properties[i]);
        if (value.isNull()) {
            if (valueMode == ReturnNullOnUncommonValues)
                return String();
            ASSERT(valueMode == OmitUncommonValues);
            continue;
        }
        if (!i)
            commonValue = value;
        else if (!commonValue.isNull() && commonValue != value)
            commonValue = String();
        if (value == "initial")
            continue;
        if (!result.isEmpty())
            result.append(' ');
        result.append(value);
    }
    if (isInitialOrInherit(commonValue))
        return commonValue;
    return result.isEmpty() ? String() : result.toString();
}

static void appendBackgroundRepeatValue(StringBuilder& builder, const CSSValue& repeatXCSSValue, const CSSValue& repeatYCSSValue)
{
    // FIXME: Ensure initial values do not appear in CSS_VALUE_LISTS.
    DEFINE_STATIC_REF_WILL_BE_PERSISTENT(CSSPrimitiveValue, initialRepeatValue, (CSSPrimitiveValue::create(CSSValueRepeat)));
    const CSSPrimitiveValue& repeatX = repeatXCSSValue.isInitialValue() ? *initialRepeatValue : toCSSPrimitiveValue(repeatXCSSValue);
    const CSSPrimitiveValue& repeatY = repeatYCSSValue.isInitialValue() ? *initialRepeatValue : toCSSPrimitiveValue(repeatYCSSValue);
    CSSValueID repeatXValueId = repeatX.getValueID();
    CSSValueID repeatYValueId = repeatY.getValueID();
    if (repeatXValueId == repeatYValueId) {
        builder.append(repeatX.cssText());
    } else if (repeatXValueId == CSSValueNoRepeat && repeatYValueId == CSSValueRepeat) {
        builder.appendLiteral("repeat-y");
    } else if (repeatXValueId == CSSValueRepeat && repeatYValueId == CSSValueNoRepeat) {
        builder.appendLiteral("repeat-x");
    } else {
        builder.append(repeatX.cssText());
        builder.appendLiteral(" ");
        builder.append(repeatY.cssText());
    }
}

String StylePropertySerializer::backgroundRepeatPropertyValue() const
{
    const CSSValue* repeatX = m_propertySet.getPropertyCSSValue(CSSPropertyBackgroundRepeatX);
    const CSSValue* repeatY = m_propertySet.getPropertyCSSValue(CSSPropertyBackgroundRepeatY);
    if (!repeatX || !repeatY)
        return String();
    if (m_propertySet.propertyIsImportant(CSSPropertyBackgroundRepeatX) != m_propertySet.propertyIsImportant(CSSPropertyBackgroundRepeatY))
        return String();
    if ((repeatX->isInitialValue() && repeatY->isInitialValue()) || (repeatX->isInheritedValue() && repeatY->isInheritedValue()))
        return repeatX->cssText();

    const CSSValueList* repeatXList = 0;
    int repeatXLength = 1;
    if (repeatX->isValueList()) {
        repeatXList = toCSSValueList(repeatX);
        repeatXLength = repeatXList->length();
    } else if (!repeatX->isPrimitiveValue()) {
        return String();
    }

    const CSSValueList* repeatYList = 0;
    int repeatYLength = 1;
    if (repeatY->isValueList()) {
        repeatYList = toCSSValueList(repeatY);
        repeatYLength = repeatYList->length();
    } else if (!repeatY->isPrimitiveValue()) {
        return String();
    }

    size_t shorthandLength = lowestCommonMultiple(repeatXLength, repeatYLength);
    StringBuilder builder;
    for (size_t i = 0; i < shorthandLength; ++i) {
        if (i)
            builder.appendLiteral(", ");

        const CSSValue* xValue = repeatXList ? repeatXList->item(i % repeatXList->length()) : repeatX;
        const CSSValue* yValue = repeatYList ? repeatYList->item(i % repeatYList->length()) : repeatY;
        appendBackgroundRepeatValue(builder, *xValue, *yValue);
    }
    return builder.toString();
}

void StylePropertySerializer::appendBackgroundPropertyAsText(StringBuilder& result, unsigned& numDecls) const
{
    if (isPropertyShorthandAvailable(backgroundShorthand())) {
        String backgroundValue = getPropertyValue(CSSPropertyBackground);
        bool isImportant = m_propertySet.propertyIsImportant(CSSPropertyBackgroundImage);
        result.append(getPropertyText(CSSPropertyBackground, backgroundValue, isImportant, numDecls++));
        return;
    }
    if (shorthandHasOnlyInitialOrInheritedValue(backgroundShorthand())) {
        const CSSValue* value = m_propertySet.getPropertyCSSValue(CSSPropertyBackgroundImage);
        bool isImportant = m_propertySet.propertyIsImportant(CSSPropertyBackgroundImage);
        result.append(getPropertyText(CSSPropertyBackground, value->cssText(), isImportant, numDecls++));
        return;
    }

    // backgroundShorthandProperty without layered shorhand properties
    const CSSPropertyID backgroundPropertyIds[] = {
        CSSPropertyBackgroundImage,
        CSSPropertyBackgroundAttachment,
        CSSPropertyBackgroundColor,
        CSSPropertyBackgroundSize,
        CSSPropertyBackgroundOrigin,
        CSSPropertyBackgroundClip
    };

    for (unsigned i = 0; i < WTF_ARRAY_LENGTH(backgroundPropertyIds); ++i) {
        CSSPropertyID propertyID = backgroundPropertyIds[i];
        const CSSValue* value = m_propertySet.getPropertyCSSValue(propertyID);
        if (!value)
            continue;
        result.append(getPropertyText(propertyID, value->cssText(), m_propertySet.propertyIsImportant(propertyID), numDecls++));
    }

    // FIXME: This is a not-so-nice way to turn x/y positions into single background-position in output.
    // It is required because background-position-x/y are non-standard properties and WebKit generated output
    // would not work in Firefox (<rdar://problem/5143183>)
    // It would be a better solution if background-position was CSS_PAIR.
    if (shorthandHasOnlyInitialOrInheritedValue(backgroundPositionShorthand())) {
        const CSSValue* value = m_propertySet.getPropertyCSSValue(CSSPropertyBackgroundPositionX);
        bool isImportant = m_propertySet.propertyIsImportant(CSSPropertyBackgroundPositionX);
        result.append(getPropertyText(CSSPropertyBackgroundPosition, value->cssText(), isImportant, numDecls++));
    } else if (isPropertyShorthandAvailable(backgroundPositionShorthand())) {
        String positionValue = m_propertySet.getPropertyValue(CSSPropertyBackgroundPosition);
        bool isImportant = m_propertySet.propertyIsImportant(CSSPropertyBackgroundPositionX);
        if (!positionValue.isNull())
            result.append(getPropertyText(CSSPropertyBackgroundPosition, positionValue, isImportant, numDecls++));
    } else {
        // should check background-position-x or background-position-y.
        if (const CSSValue* value = m_propertySet.getPropertyCSSValue(CSSPropertyBackgroundPositionX)) {
            if (!value->isImplicitInitialValue()) {
                bool isImportant = m_propertySet.propertyIsImportant(CSSPropertyBackgroundPositionX);
                result.append(getPropertyText(CSSPropertyBackgroundPositionX, value->cssText(), isImportant, numDecls++));
            }
        }
        if (const CSSValue* value = m_propertySet.getPropertyCSSValue(CSSPropertyBackgroundPositionY)) {
            if (!value->isImplicitInitialValue()) {
                bool isImportant = m_propertySet.propertyIsImportant(CSSPropertyBackgroundPositionY);
                result.append(getPropertyText(CSSPropertyBackgroundPositionY, value->cssText(), isImportant, numDecls++));
            }
        }
    }

    String repeatValue = m_propertySet.getPropertyValue(CSSPropertyBackgroundRepeat);
    if (!repeatValue.isNull())
        result.append(getPropertyText(CSSPropertyBackgroundRepeat, repeatValue, m_propertySet.propertyIsImportant(CSSPropertyBackgroundRepeatX), numDecls++));
}

bool StylePropertySerializer::isPropertyShorthandAvailable(const StylePropertyShorthand& shorthand) const
{
    ASSERT(shorthand.length() > 0);

    bool isImportant = m_propertySet.propertyIsImportant(shorthand.properties()[0]);
    for (unsigned i = 0; i < shorthand.length(); ++i) {
        const CSSValue* value = m_propertySet.getPropertyCSSValue(shorthand.properties()[i]);
        if (!value || (value->isInitialValue() && !value->isImplicitInitialValue()) || value->isInheritedValue())
            return false;
        if (isImportant != m_propertySet.propertyIsImportant(shorthand.properties()[i]))
            return false;
    }
    return true;
}

bool StylePropertySerializer::shorthandHasOnlyInitialOrInheritedValue(const StylePropertyShorthand& shorthand) const
{
    ASSERT(shorthand.length() > 0);
    bool isImportant = m_propertySet.propertyIsImportant(shorthand.properties()[0]);
    bool isInitialValue = true;
    bool isInheritedValue = true;
    for (unsigned i = 0; i < shorthand.length(); ++i) {
        const CSSValue* value = m_propertySet.getPropertyCSSValue(shorthand.properties()[i]);
        if (!value)
            return false;
        if (!value->isInitialValue())
            isInitialValue = false;
        if (!value->isInheritedValue())
            isInheritedValue = false;
        if (isImportant != m_propertySet.propertyIsImportant(shorthand.properties()[i]))
            return false;
    }
    return isInitialValue || isInheritedValue;
}

}
