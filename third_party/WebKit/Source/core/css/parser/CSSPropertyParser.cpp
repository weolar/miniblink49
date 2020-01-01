/*
 * Copyright (C) 2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2005 Allan Sandfeld Jensen (kde@carewolf.com)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Nicholas Shanks <webkit@nickshanks.com>
 * Copyright (C) 2008 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2012 Adobe Systems Incorporated. All rights reserved.
 * Copyright (C) 2012 Intel Corporation. All rights reserved.
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
#include "core/css/parser/CSSPropertyParser.h"

#include "core/StylePropertyShorthand.h"
#include "core/css/CSSBasicShapes.h"
#include "core/css/CSSBorderImage.h"
#include "core/css/CSSCanvasValue.h"
#include "core/css/CSSContentDistributionValue.h"
#include "core/css/CSSCrossfadeValue.h"
#include "core/css/CSSCursorImageValue.h"
#include "core/css/CSSFontFaceSrcValue.h"
#include "core/css/CSSFontFeatureValue.h"
#include "core/css/CSSFunctionValue.h"
#include "core/css/CSSGridLineNamesValue.h"
#include "core/css/CSSImageSetValue.h"
#include "core/css/CSSImageValue.h"
#include "core/css/CSSLineBoxContainValue.h"
#include "core/css/CSSPathValue.h"
#include "core/css/CSSPrimitiveValueMappings.h"
#include "core/css/CSSProperty.h"
#include "core/css/CSSPropertyMetadata.h"
#include "core/css/CSSReflectValue.h"
#include "core/css/CSSSVGDocumentValue.h"
#include "core/css/CSSShadowValue.h"
#include "core/css/CSSTimingFunctionValue.h"
#include "core/css/CSSUnicodeRangeValue.h"
#include "core/css/CSSValuePool.h"
#include "core/css/Counter.h"
#include "core/css/HashTools.h"
#include "core/css/Pair.h"
#include "core/css/Rect.h"
#include "core/css/CSSVariableReferenceValue.h"
#include "core/css/parser/CSSParserFastPaths.h"
#include "core/css/parser/CSSParserValues.h"
#include "core/frame/UseCounter.h"
#include "core/layout/LayoutTheme.h"
#include "core/style/GridCoordinate.h"
#include "core/svg/SVGPathUtilities.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace blink {

template <unsigned N>
static bool equalIgnoringCase(const CSSParserString& a, const char (&b)[N])
{
    unsigned length = N - 1; // Ignore the trailing null character
    if (a.length() != length)
        return false;

    return a.is8Bit() ? WTF::equalIgnoringCase(b, a.characters8(), length) : WTF::equalIgnoringCase(b, a.characters16(), length);
}

static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> createPrimitiveValuePair(PassRefPtrWillBeRawPtr<CSSPrimitiveValue> first, PassRefPtrWillBeRawPtr<CSSPrimitiveValue> second, Pair::IdenticalValuesPolicy identicalValuesPolicy = Pair::DropIdenticalValues)
{
    return cssValuePool().createValue(Pair::create(first, second, identicalValuesPolicy));
}

CSSPropertyParser::CSSPropertyParser(CSSParserValueList* valueList,
    const CSSParserContext& context, WillBeHeapVector<CSSProperty, 256>& parsedProperties,
    StyleRule::Type ruleType)
    : m_valueList(valueList)
    , m_context(context)
    , m_parsedProperties(parsedProperties)
    , m_ruleType(ruleType)
    , m_inParseShorthand(0)
    , m_currentShorthand(CSSPropertyInvalid)
    , m_implicitShorthand(false)
{
}

bool CSSPropertyParser::parseValue(CSSPropertyID unresolvedProperty, bool important,
    CSSParserValueList* valueList, const CSSParserContext& context,
    WillBeHeapVector<CSSProperty, 256>& parsedProperties, StyleRule::Type ruleType)
{
    if (!valueList->size())
        return false; // Parser error

    int parsedPropertiesSize = parsedProperties.size();

    CSSPropertyParser parser(valueList, context, parsedProperties, ruleType);
    CSSPropertyID resolvedProperty = resolveCSSPropertyID(unresolvedProperty);
    bool parseSuccess;

    if (ruleType == StyleRule::Viewport) {
        parseSuccess = (RuntimeEnabledFeatures::cssViewportEnabled() || isUASheetBehavior(context.mode()))
            && parser.parseViewportProperty(resolvedProperty, important);
    } else if (ruleType == StyleRule::FontFace) {
        parseSuccess = parser.parseFontFaceDescriptor(resolvedProperty);
    } else {
        parseSuccess = parser.parseValue(unresolvedProperty, important);
    }

    // This doesn't count UA style sheets
    if (parseSuccess && context.useCounter())
        context.useCounter()->count(context, unresolvedProperty);

    if (!parseSuccess)
        parser.rollbackLastProperties(parsedProperties.size() - parsedPropertiesSize);

    return parseSuccess;
}

void CSSPropertyParser::addProperty(CSSPropertyID propId, PassRefPtrWillBeRawPtr<CSSValue> value, bool important, bool implicit)
{
    ASSERT(!isPropertyAlias(propId));

    int shorthandIndex = 0;
    bool setFromShorthand = false;

    if (m_currentShorthand) {
        Vector<StylePropertyShorthand, 4> shorthands;
        getMatchingShorthandsForLonghand(propId, &shorthands);
        // Viewport descriptors have width and height as shorthands, but it doesn't
        // make sense for CSSProperties.in to consider them as such. The shorthand
        // index is only used by the inspector and doesn't affect viewport
        // descriptors.
        if (shorthands.isEmpty())
            ASSERT(m_currentShorthand == CSSPropertyWidth || m_currentShorthand == CSSPropertyHeight);
        else
            setFromShorthand = true;

        if (shorthands.size() > 1)
            shorthandIndex = indexOfShorthandForLonghand(m_currentShorthand, shorthands);
    }

    m_parsedProperties.append(CSSProperty(propId, value, important, setFromShorthand, shorthandIndex, m_implicitShorthand || implicit));
}

void CSSPropertyParser::rollbackLastProperties(int num)
{
    ASSERT(num >= 0);
    ASSERT(m_parsedProperties.size() >= static_cast<unsigned>(num));
    m_parsedProperties.shrink(m_parsedProperties.size() - num);
}

KURL CSSPropertyParser::completeURL(const String& url) const
{
    return m_context.completeURL(url);
}

bool CSSPropertyParser::validCalculationUnit(CSSParserValue* value, Units unitflags, ReleaseParsedCalcValueCondition releaseCalc)
{
    bool mustBeNonNegative = unitflags & (FNonNeg | FPositiveInteger);

    if (!parseCalculation(value, mustBeNonNegative ? ValueRangeNonNegative : ValueRangeAll))
        return false;

    bool b = false;
    switch (m_parsedCalculation->category()) {
    case CalcLength:
        b = (unitflags & FLength);
        break;
    case CalcNumber:
        b = (unitflags & FNumber);
        if (!b && (unitflags & (FInteger | FPositiveInteger)) && m_parsedCalculation->isInt())
            b = true;
        if (b && mustBeNonNegative && m_parsedCalculation->isNegative())
            b = false;
        // Always resolve calc() to a CSS_NUMBER in the CSSParserValue if there are no non-numbers specified in the unitflags.
        if (b && !(unitflags & ~(FInteger | FNumber | FPositiveInteger | FNonNeg))) {
            double number = m_parsedCalculation->doubleValue();
            if ((unitflags & FPositiveInteger) && number <= 0) {
                b = false;
            } else {
                delete value->function;
                value->unit = CSSPrimitiveValue::CSS_NUMBER;
                value->fValue = number;
                value->isInt = m_parsedCalculation->isInt();
            }
            m_parsedCalculation.release();
            return b;
        }
        break;
    case CalcPercent:
        b = (unitflags & FPercent);
        if (b && mustBeNonNegative && m_parsedCalculation->isNegative())
            b = false;
        break;
    case CalcPercentLength:
        b = (unitflags & FPercent) && (unitflags & FLength);
        break;
    case CalcPercentNumber:
        b = (unitflags & FPercent) && (unitflags & FNumber);
        break;
    case CalcAngle:
        b = (unitflags & FAngle);
        break;
    case CalcTime:
        b = (unitflags & FTime);
        break;
    case CalcFrequency:
        b = (unitflags & FFrequency);
        break;
    case CalcOther:
        break;
    }
    if (!b || releaseCalc == ReleaseParsedCalcValue)
        m_parsedCalculation.release();
    return b;
}

inline bool CSSPropertyParser::shouldAcceptUnitLessValues(CSSParserValue* value, Units unitflags, CSSParserMode cssParserMode)
{
    // Quirks mode for certain properties and presentation attributes accept unit-less values for certain units.
    return (unitflags & (FLength | FAngle))
        && (!value->fValue // 0 can always be unitless.
            || isUnitLessLengthParsingEnabledForMode(cssParserMode) // HTML and SVG attribute values can always be unitless.
            || (cssParserMode == HTMLQuirksMode && (unitflags & FUnitlessQuirk)));
}

bool CSSPropertyParser::validUnit(CSSParserValue* value, Units unitflags, CSSParserMode cssParserMode, ReleaseParsedCalcValueCondition releaseCalc)
{
    if (isCalculation(value))
        return validCalculationUnit(value, unitflags, releaseCalc);

    if (unitflags & FNonNeg && value->fValue < 0)
        return false;
    switch (value->unit) {
    case CSSPrimitiveValue::CSS_NUMBER:
        if (unitflags & FNumber)
            return true;
        if (shouldAcceptUnitLessValues(value, unitflags, cssParserMode)) {
            value->unit = (unitflags & FLength) ? CSSPrimitiveValue::CSS_PX : CSSPrimitiveValue::CSS_DEG;
            return true;
        }
        if ((unitflags & FInteger) && value->isInt)
            return true;
        if ((unitflags & FPositiveInteger) && value->isInt && value->fValue > 0)
            return true;
        return false;
    case CSSPrimitiveValue::CSS_PERCENTAGE:
        return unitflags & FPercent;
    case CSSPrimitiveValue::CSS_QEM:
        if (cssParserMode != UASheetMode)
            return false;
    /* fallthrough intentional */
    case CSSPrimitiveValue::CSS_EMS:
    case CSSPrimitiveValue::CSS_REMS:
    case CSSPrimitiveValue::CSS_CHS:
    case CSSPrimitiveValue::CSS_EXS:
    case CSSPrimitiveValue::CSS_PX:
    case CSSPrimitiveValue::CSS_CM:
    case CSSPrimitiveValue::CSS_MM:
    case CSSPrimitiveValue::CSS_IN:
    case CSSPrimitiveValue::CSS_PT:
    case CSSPrimitiveValue::CSS_PC:
    case CSSPrimitiveValue::CSS_VW:
    case CSSPrimitiveValue::CSS_VH:
    case CSSPrimitiveValue::CSS_VMIN:
    case CSSPrimitiveValue::CSS_VMAX:
        return unitflags & FLength;
    case CSSPrimitiveValue::CSS_MS:
    case CSSPrimitiveValue::CSS_S:
        return unitflags & FTime;
    case CSSPrimitiveValue::CSS_DEG:
    case CSSPrimitiveValue::CSS_RAD:
    case CSSPrimitiveValue::CSS_GRAD:
    case CSSPrimitiveValue::CSS_TURN:
        return unitflags & FAngle;
    case CSSPrimitiveValue::CSS_DPPX:
    case CSSPrimitiveValue::CSS_DPI:
    case CSSPrimitiveValue::CSS_DPCM:
        return unitflags & FResolution;
    default:
        return false;
    }
}

PassRefPtrWillBeRawPtr<CSSPrimitiveValue> CSSPropertyParser::createPrimitiveNumericValue(CSSParserValue* value)
{
    if (m_parsedCalculation) {
        ASSERT(isCalculation(value));
        return CSSPrimitiveValue::create(m_parsedCalculation.release());
    }

    ASSERT((value->unit >= CSSPrimitiveValue::CSS_NUMBER && value->unit <= CSSPrimitiveValue::CSS_KHZ)
        || (value->unit >= CSSPrimitiveValue::CSS_TURN && value->unit <= CSSPrimitiveValue::CSS_CHS)
        || (value->unit >= CSSPrimitiveValue::CSS_VW && value->unit <= CSSPrimitiveValue::CSS_VMAX)
        || (value->unit >= CSSPrimitiveValue::CSS_DPPX && value->unit <= CSSPrimitiveValue::CSS_DPCM));
    return cssValuePool().createValue(value->fValue, static_cast<CSSPrimitiveValue::UnitType>(value->unit));
}

inline PassRefPtrWillBeRawPtr<CSSPrimitiveValue> CSSPropertyParser::createPrimitiveStringValue(CSSParserValue* value)
{
    ASSERT(value->unit == CSSPrimitiveValue::CSS_STRING || value->unit == CSSPrimitiveValue::CSS_IDENT);
    return cssValuePool().createValue(value->string, CSSPrimitiveValue::CSS_STRING);
}

inline PassRefPtrWillBeRawPtr<CSSPrimitiveValue> CSSPropertyParser::createPrimitiveCustomIdentValue(CSSParserValue* value)
{
    ASSERT(value->unit == CSSPrimitiveValue::CSS_STRING || value->unit == CSSPrimitiveValue::CSS_IDENT);
    return cssValuePool().createValue(value->string, CSSPrimitiveValue::CSS_CUSTOM_IDENT);
}

inline PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::createCSSImageValueWithReferrer(const String& rawValue, const KURL& url)
{
    RefPtrWillBeRawPtr<CSSValue> imageValue = CSSImageValue::create(rawValue, url);
    toCSSImageValue(imageValue.get())->setReferrer(m_context.referrer());
    return imageValue;
}

static inline bool isComma(CSSParserValue* value)
{
    ASSERT(value);
    return value->unit == CSSParserValue::Operator && value->iValue == ',';
}

static bool consumeComma(CSSParserValueList* valueList)
{
    CSSParserValue* value = valueList->current();
    if (!value || !isComma(value))
        return false;
    valueList->next();
    return true;
}

static inline bool isForwardSlashOperator(CSSParserValue* value)
{
    ASSERT(value);
    return value->unit == CSSParserValue::Operator && value->iValue == '/';
}

static bool isGeneratedImageValue(CSSParserValue* val)
{
    if (val->unit != CSSParserValue::Function)
        return false;

    CSSValueID id = val->function->id;
    return id == CSSValueLinearGradient
        || id == CSSValueRadialGradient
        || id == CSSValueRepeatingLinearGradient
        || id == CSSValueRepeatingRadialGradient
        || id == CSSValueWebkitLinearGradient
        || id == CSSValueWebkitRadialGradient
        || id == CSSValueWebkitRepeatingLinearGradient
        || id == CSSValueWebkitRepeatingRadialGradient
        || id == CSSValueWebkitGradient
        || id == CSSValueWebkitCanvas
        || id == CSSValueWebkitCrossFade;
}

bool CSSPropertyParser::validWidthOrHeight(CSSParserValue* value, Units unitless)
{
    int id = value->id;
    if (id == CSSValueIntrinsic || id == CSSValueMinIntrinsic || id == CSSValueWebkitMinContent || id == CSSValueWebkitMaxContent || id == CSSValueWebkitFillAvailable || id == CSSValueWebkitFitContent) {
        if (m_context.useCounter()) {
            if (value->id == CSSValueIntrinsic)
                m_context.useCounter()->count(UseCounter::LegacyCSSValueIntrinsic);
            else if (value->id == CSSValueMinIntrinsic)
                m_context.useCounter()->count(UseCounter::LegacyCSSValueMinIntrinsic);
        }
        return true;
    }
    return validUnit(value, FLength | FPercent | FNonNeg | unitless);
}

inline PassRefPtrWillBeRawPtr<CSSPrimitiveValue> CSSPropertyParser::parseValidPrimitive(CSSValueID identifier, CSSParserValue* value)
{
    if (identifier)
        return cssValuePool().createIdentifierValue(identifier);
    if (value->unit >= CSSPrimitiveValue::CSS_NUMBER && value->unit <= CSSPrimitiveValue::CSS_KHZ)
        return createPrimitiveNumericValue(value);
    if (value->unit >= CSSPrimitiveValue::CSS_TURN && value->unit <= CSSPrimitiveValue::CSS_CHS)
        return createPrimitiveNumericValue(value);
    if (value->unit >= CSSPrimitiveValue::CSS_VW && value->unit <= CSSPrimitiveValue::CSS_VMAX)
        return createPrimitiveNumericValue(value);
    if (value->unit >= CSSPrimitiveValue::CSS_DPPX && value->unit <= CSSPrimitiveValue::CSS_DPCM)
        return createPrimitiveNumericValue(value);
    if (value->unit == CSSPrimitiveValue::CSS_QEM)
        return CSSPrimitiveValue::createAllowingMarginQuirk(value->fValue, CSSPrimitiveValue::CSS_EMS);
    if (isCalculation(value))
        return CSSPrimitiveValue::create(m_parsedCalculation.release());

    return nullptr;
}

void CSSPropertyParser::addExpandedPropertyForValue(CSSPropertyID propId, PassRefPtrWillBeRawPtr<CSSValue> prpValue, bool important)
{
    const StylePropertyShorthand& shorthand = shorthandForProperty(propId);
    unsigned shorthandLength = shorthand.length();
    if (!shorthandLength) {
        addProperty(propId, prpValue, important);
        return;
    }

    RefPtrWillBeRawPtr<CSSValue> value = prpValue;
    ShorthandScope scope(this, propId);
    const CSSPropertyID* longhands = shorthand.properties();
    for (unsigned i = 0; i < shorthandLength; ++i)
        addProperty(longhands[i], value, important);
}

bool CSSPropertyParser::parseValue(CSSPropertyID unresolvedProperty, bool important)
{
    CSSPropertyID propId = resolveCSSPropertyID(unresolvedProperty);

    CSSParserValue* value = m_valueList->current();

    // Note: m_parsedCalculation is used to pass the calc value to validUnit and then cleared at the end of this function.
    // FIXME: This is to avoid having to pass parsedCalc to all validUnit callers.
    ASSERT(!m_parsedCalculation);

    CSSValueID id = value->id;

    if (id == CSSValueInherit) {
        if (m_valueList->size() != 1)
            return false;
        addExpandedPropertyForValue(propId, cssValuePool().createInheritedValue(), important);
        return true;
    } else if (id == CSSValueInitial) {
        if (m_valueList->size() != 1)
            return false;
        addExpandedPropertyForValue(propId, cssValuePool().createExplicitInitialValue(), important);
        return true;
    } else if (id == CSSValueUnset) {
        if (m_valueList->size() != 1)
            return false;
        addExpandedPropertyForValue(propId, cssValuePool().createUnsetValue(), important);
        return true;
    }

    int num = inShorthand() ? 1 : m_valueList->size();

    if (RuntimeEnabledFeatures::cssVariablesEnabled() && value->id == CSSValueInternalVariableValue) {
        // We don't expand the shorthand here because crazypants.
        m_parsedProperties.append(CSSProperty(propId, CSSVariableReferenceValue::create(value->variableData), important, false, 0, m_implicitShorthand));
        m_valueList->next();
        return true;
    }

    if (CSSParserFastPaths::isKeywordPropertyID(propId)) {
        if (!CSSParserFastPaths::isValidKeywordPropertyAndValue(propId, id))
            return false;
        if (m_valueList->next() && !inShorthand())
            return false;
        addProperty(propId, cssValuePool().createIdentifierValue(id), important);
        return true;
    }

    bool validPrimitive = false;
    Units unitless = FUnknown;
    RefPtrWillBeRawPtr<CSSValue> parsedValue = nullptr;

    switch (propId) {
    case CSSPropertySize: // <length>{1,2} | auto | [ <page-size> || [ portrait | landscape] ]
        parsedValue = parseSize();
        break;
    case CSSPropertyQuotes: // [<string> <string>]+ | none
        if (id == CSSValueNone)
            validPrimitive = true;
        else
            parsedValue = parseQuotes();
        break;

    case CSSPropertyContent:              // [ <string> | <uri> | <counter> | attr(X) | open-quote |
        // close-quote | no-open-quote | no-close-quote ]+ | inherit
        parsedValue = parseContent();
        break;
    case CSSPropertyClip:                 // <shape> | auto | inherit
        if (id == CSSValueAuto)
            validPrimitive = true;
        else if (value->unit == CSSParserValue::Function)
            parsedValue = parseClipShape();
        break;

    /* Start of supported CSS properties with validation. This is needed for parseShorthand to work
     * correctly and allows optimization in blink::applyRule(..)
     */
    case CSSPropertyOverflow: {
        ShorthandScope scope(this, propId);
        if (num != 1 || !parseValue(CSSPropertyOverflowY, important))
            return false;

        RefPtrWillBeRawPtr<CSSValue> overflowXValue = nullptr;

        // FIXME: -webkit-paged-x or -webkit-paged-y only apply to overflow-y. If this value has been
        // set using the shorthand, then for now overflow-x will default to auto, but once we implement
        // pagination controls, it should default to hidden. If the overflow-y value is anything but
        // paged-x or paged-y, then overflow-x and overflow-y should have the same value.
        if (id == CSSValueWebkitPagedX || id == CSSValueWebkitPagedY)
            overflowXValue = cssValuePool().createIdentifierValue(CSSValueAuto);
        else
            overflowXValue = m_parsedProperties.last().value();
        addProperty(CSSPropertyOverflowX, overflowXValue.release(), important);
        return true;
    }

    case CSSPropertyTextAlign:
        // left | right | center | justify | -webkit-left | -webkit-right | -webkit-center | -webkit-match-parent
        // | start | end | <string> | inherit | -webkit-auto (converted to start)
        // FIXME: <string> not supported right now
        if ((id >= CSSValueWebkitAuto && id <= CSSValueWebkitMatchParent) || id == CSSValueStart || id == CSSValueEnd) {
            validPrimitive = true;
        }
        break;

    case CSSPropertyFontWeight:  { // normal | bold | bolder | lighter | 100 | 200 | 300 | 400 | 500 | 600 | 700 | 800 | 900 | inherit
        if (m_valueList->size() != 1)
            return false;
        return parseFontWeight(important);
    }

    case CSSPropertyBorderSpacing: {
        if (num == 1) {
            ShorthandScope scope(this, CSSPropertyBorderSpacing);
            if (!parseValue(CSSPropertyWebkitBorderHorizontalSpacing, important))
                return false;
            CSSValue* value = m_parsedProperties.last().value();
            addProperty(CSSPropertyWebkitBorderVerticalSpacing, value, important);
            return true;
        }
        else if (num == 2) {
            ShorthandScope scope(this, CSSPropertyBorderSpacing);
            if (!parseValue(CSSPropertyWebkitBorderHorizontalSpacing, important) || !parseValue(CSSPropertyWebkitBorderVerticalSpacing, important))
                return false;
            return true;
        }
        return false;
    }
    case CSSPropertyWebkitBorderHorizontalSpacing:
    case CSSPropertyWebkitBorderVerticalSpacing:
        unitless = FLength | FNonNeg;
        if (inShorthand() && m_currentShorthand == CSSPropertyBorderSpacing)
            unitless = unitless | FUnitlessQuirk;
        validPrimitive = validUnit(value, unitless);
        break;
    case CSSPropertyOutlineColor:        // <color> | invert | inherit
        // Outline color has "invert" as additional keyword.
        // Also, we want to allow the special focus color even in HTML Standard parsing mode.
        if (id == CSSValueInvert || id == CSSValueWebkitFocusRingColor) {
            validPrimitive = true;
            break;
        }
        /* nobreak */
    case CSSPropertyBackgroundColor: // <color> | inherit
    case CSSPropertyBorderTopColor: // <color> | inherit
    case CSSPropertyBorderRightColor:
    case CSSPropertyBorderBottomColor:
    case CSSPropertyBorderLeftColor:
    case CSSPropertyWebkitBorderStartColor:
    case CSSPropertyWebkitBorderEndColor:
    case CSSPropertyWebkitBorderBeforeColor:
    case CSSPropertyWebkitBorderAfterColor:
    case CSSPropertyColor: // <color> | inherit
    case CSSPropertyTextDecorationColor: // CSS3 text decoration colors
    case CSSPropertyWebkitColumnRuleColor:
    case CSSPropertyWebkitTextEmphasisColor:
    case CSSPropertyWebkitTextFillColor:
    case CSSPropertyWebkitTextStrokeColor:
        ASSERT(propId != CSSPropertyTextDecorationColor || RuntimeEnabledFeatures::css3TextDecorationsEnabled());
        parsedValue = parseColor(m_valueList->current(), acceptQuirkyColors(propId));
        if (parsedValue)
            m_valueList->next();
        break;

    case CSSPropertyCursor: {
        // Grammar defined by CSS3 UI and modified by CSS4 images:
        // [ [<image> [<x> <y>]?,]*
        // [ auto | crosshair | default | pointer | progress | move | e-resize | ne-resize |
        // nw-resize | n-resize | se-resize | sw-resize | s-resize | w-resize | ew-resize |
        // ns-resize | nesw-resize | nwse-resize | col-resize | row-resize | text | wait | help |
        // vertical-text | cell | context-menu | alias | copy | no-drop | not-allowed | all-scroll |
        // zoom-in | zoom-out | -webkit-grab | -webkit-grabbing | -webkit-zoom-in | -webkit-zoom-out ] ] | inherit
        RefPtrWillBeRawPtr<CSSValueList> list = nullptr;
        while (value) {
            RefPtrWillBeRawPtr<CSSValue> image = nullptr;
            if (value->unit == CSSPrimitiveValue::CSS_URI) {
                String uri = value->string;
                if (!uri.isNull())
                    image = createCSSImageValueWithReferrer(uri, completeURL(uri));
            } else if (value->unit == CSSParserValue::Function && value->function->id == CSSValueWebkitImageSet) {
                image = parseImageSet(m_valueList);
                if (!image)
                    break;
            } else
                break;

            Vector<int> coords;
            value = m_valueList->next();
            while (value && validUnit(value, FNumber)) {
                coords.append(int(value->fValue));
                value = m_valueList->next();
            }
            bool hotSpotSpecified = false;
            IntPoint hotSpot(-1, -1);
            int nrcoords = coords.size();
            if (nrcoords > 0 && nrcoords != 2)
                return false;
            if (nrcoords == 2) {
                hotSpotSpecified = true;
                hotSpot = IntPoint(coords[0], coords[1]);
            }

            if (!list)
                list = CSSValueList::createCommaSeparated();

            if (image)
                list->append(CSSCursorImageValue::create(image, hotSpotSpecified, hotSpot));

            if (!consumeComma(m_valueList))
                return false;
            value = m_valueList->current();
        }
        if (value && m_context.useCounter()) {
            if (value->id == CSSValueWebkitZoomIn)
                m_context.useCounter()->count(UseCounter::PrefixedCursorZoomIn);
            else if (value->id == CSSValueWebkitZoomOut)
                m_context.useCounter()->count(UseCounter::PrefixedCursorZoomOut);
        }
        if (list) {
            if (!value)
                return false;
            if (inQuirksMode() && value->id == CSSValueHand) // MSIE 5 compatibility :/
                list->append(cssValuePool().createIdentifierValue(CSSValuePointer));
            else if ((value->id >= CSSValueAuto && value->id <= CSSValueWebkitZoomOut) || value->id == CSSValueCopy || value->id == CSSValueNone)
                list->append(cssValuePool().createIdentifierValue(value->id));
            m_valueList->next();
            parsedValue = list.release();
            break;
        } else if (value) {
            id = value->id;
            if (inQuirksMode() && value->id == CSSValueHand) { // MSIE 5 compatibility :/
                id = CSSValuePointer;
                validPrimitive = true;
            } else if ((value->id >= CSSValueAuto && value->id <= CSSValueWebkitZoomOut) || value->id == CSSValueCopy || value->id == CSSValueNone)
                validPrimitive = true;
        } else {
            ASSERT_NOT_REACHED();
            return false;
        }
        break;
    }

    case CSSPropertyBackgroundBlendMode:
    case CSSPropertyBackgroundAttachment:
    case CSSPropertyBackgroundClip:
    case CSSPropertyWebkitBackgroundClip:
    //case CSSPropertyWebkitBackgroundComposite:
    case CSSPropertyBackgroundImage:
    case CSSPropertyBackgroundOrigin:
    case CSSPropertyMaskSourceType:
    case CSSPropertyWebkitBackgroundOrigin:
    case CSSPropertyBackgroundPosition:
    case CSSPropertyBackgroundPositionX:
    case CSSPropertyBackgroundPositionY:
    case CSSPropertyBackgroundSize:
    case CSSPropertyBackgroundRepeat:
    case CSSPropertyWebkitMaskClip:
    case CSSPropertyWebkitMaskComposite:
    case CSSPropertyWebkitMaskImage:
    case CSSPropertyWebkitMaskOrigin:
    case CSSPropertyWebkitMaskPosition:
    case CSSPropertyWebkitMaskPositionX:
    case CSSPropertyWebkitMaskPositionY:
    case CSSPropertyWebkitMaskSize:
    case CSSPropertyWebkitMaskRepeat:
    case CSSPropertyWebkitMaskRepeatX:
    case CSSPropertyWebkitMaskRepeatY:
    {
        RefPtrWillBeRawPtr<CSSValue> val1 = nullptr;
        RefPtrWillBeRawPtr<CSSValue> val2 = nullptr;
        CSSPropertyID propId1, propId2;
        bool result = false;
        if (parseFillProperty(unresolvedProperty, propId1, propId2, val1, val2)) {
            if (propId == CSSPropertyBackgroundPosition ||
                propId == CSSPropertyBackgroundRepeat ||
                propId == CSSPropertyWebkitMaskPosition ||
                propId == CSSPropertyWebkitMaskRepeat) {
                ShorthandScope scope(this, propId);
                addProperty(propId1, val1.release(), important);
                if (val2)
                    addProperty(propId2, val2.release(), important);
            } else {
                addProperty(propId1, val1.release(), important);
                if (val2)
                    addProperty(propId2, val2.release(), important);
            }
            result = true;
        }
        m_implicitShorthand = false;
        return result;
    }
    case CSSPropertyObjectPosition:
        parsedValue = parsePosition(m_valueList);
        break;
    case CSSPropertyListStyleImage:     // <uri> | none | inherit
    case CSSPropertyBorderImageSource:
    case CSSPropertyWebkitMaskBoxImageSource:
        if (id == CSSValueNone) {
            parsedValue = cssValuePool().createIdentifierValue(CSSValueNone);
            m_valueList->next();
        } else if (value->unit == CSSPrimitiveValue::CSS_URI) {
            parsedValue = createCSSImageValueWithReferrer(value->string, completeURL(value->string));
            m_valueList->next();
        } else if (isGeneratedImageValue(value)) {
            if (parseGeneratedImage(m_valueList, parsedValue))
                m_valueList->next();
            else
                return false;
        } else if (value->unit == CSSParserValue::Function && value->function->id == CSSValueWebkitImageSet) {
            parsedValue = parseImageSet(m_valueList);
            if (!parsedValue)
                return false;
            m_valueList->next();
        }
        break;

    case CSSPropertyBorderTopWidth:     //// <border-width> | inherit
    case CSSPropertyBorderRightWidth:   //   Which is defined as
    case CSSPropertyBorderBottomWidth:  //   thin | medium | thick | <length>
    case CSSPropertyBorderLeftWidth:
        if (!inShorthand() || m_currentShorthand == CSSPropertyBorderWidth)
            unitless = FUnitlessQuirk;
        // fall through
    case CSSPropertyWebkitTextStrokeWidth:
    case CSSPropertyOutlineWidth: // <border-width> | inherit
    case CSSPropertyWebkitBorderStartWidth:
    case CSSPropertyWebkitBorderEndWidth:
    case CSSPropertyWebkitBorderBeforeWidth:
    case CSSPropertyWebkitBorderAfterWidth:
    case CSSPropertyWebkitColumnRuleWidth:
        if (id == CSSValueThin || id == CSSValueMedium || id == CSSValueThick)
            validPrimitive = true;
        else
            validPrimitive = validUnit(value, FLength | FNonNeg | unitless);
        break;

    case CSSPropertyLetterSpacing:       // normal | <length> | inherit
    case CSSPropertyWordSpacing:         // normal | <length> | inherit
        if (id == CSSValueNormal)
            validPrimitive = true;
        else
            validPrimitive = validUnit(value, FLength | FUnitlessQuirk);
        break;

    case CSSPropertyTextIndent:
        parsedValue = parseTextIndent();
        break;

    case CSSPropertyPaddingTop:          //// <padding-width> | inherit
    case CSSPropertyPaddingRight:        //   Which is defined as
    case CSSPropertyPaddingBottom:       //   <length> | <percentage>
    case CSSPropertyPaddingLeft:         ////
        unitless = FUnitlessQuirk;
        // fall through
    case CSSPropertyWebkitPaddingStart:
    case CSSPropertyWebkitPaddingEnd:
    case CSSPropertyWebkitPaddingBefore:
    case CSSPropertyWebkitPaddingAfter:
        validPrimitive = validUnit(value, FLength | FPercent | FNonNeg | unitless);
        break;

    case CSSPropertyMaxWidth:
    case CSSPropertyMaxHeight:
        unitless = FUnitlessQuirk;
        // fall through
    case CSSPropertyWebkitMaxLogicalWidth:
    case CSSPropertyWebkitMaxLogicalHeight:
        validPrimitive = (id == CSSValueNone || validWidthOrHeight(value, unitless));
        break;

    case CSSPropertyMinWidth:
    case CSSPropertyMinHeight:
    case CSSPropertyWidth:
    case CSSPropertyHeight:
        unitless = FUnitlessQuirk;
        // fall through
    case CSSPropertyWebkitMinLogicalWidth:
    case CSSPropertyWebkitMinLogicalHeight:
    case CSSPropertyWebkitLogicalWidth:
    case CSSPropertyWebkitLogicalHeight:
        validPrimitive = (id == CSSValueAuto || validWidthOrHeight(value, unitless));
        break;

    case CSSPropertyFontSize:
        return parseFontSize(important);

    case CSSPropertyFontVariant:         // normal | small-caps | inherit
        return parseFontVariant(important);

    case CSSPropertyVerticalAlign:
        // baseline | sub | super | top | text-top | middle | bottom | text-bottom |
        // <percentage> | <length> | inherit

        if (id >= CSSValueBaseline && id <= CSSValueWebkitBaselineMiddle)
            validPrimitive = true;
        else
            validPrimitive = validUnit(value, FLength | FPercent | FUnitlessQuirk);
        break;

    case CSSPropertyBottom:               // <length> | <percentage> | auto | inherit
    case CSSPropertyLeft:                 // <length> | <percentage> | auto | inherit
    case CSSPropertyRight:                // <length> | <percentage> | auto | inherit
    case CSSPropertyTop:                  // <length> | <percentage> | auto | inherit
    case CSSPropertyMarginTop:           //// <margin-width> | inherit
    case CSSPropertyMarginRight:         //   Which is defined as
    case CSSPropertyMarginBottom:        //   <length> | <percentage> | auto | inherit
    case CSSPropertyMarginLeft:          ////
        unitless = FUnitlessQuirk;
        // fall through
    case CSSPropertyWebkitMarginStart:
    case CSSPropertyWebkitMarginEnd:
    case CSSPropertyWebkitMarginBefore:
    case CSSPropertyWebkitMarginAfter:
        if (id == CSSValueAuto)
            validPrimitive = true;
        else
            validPrimitive = validUnit(value, FLength | FPercent | unitless);
        break;

    case CSSPropertyOrphans: // <integer> | inherit | auto (We've added support for auto for backwards compatibility)
    case CSSPropertyWidows: // <integer> | inherit | auto (Ditto)
        if (id == CSSValueAuto)
            validPrimitive = true;
        else
            validPrimitive = validUnit(value, FPositiveInteger);
        break;

    case CSSPropertyZIndex: // auto | <integer> | inherit
        if (id == CSSValueAuto) {
            validPrimitive = true;
        } else if (validUnit(value, FInteger)) {
            addProperty(propId, cssValuePool().createValue(value->fValue, CSSPrimitiveValue::CSS_INTEGER), important);
            return true;
        }
        break;

    case CSSPropertyLineHeight:
        parsedValue = parseLineHeight();
        break;
    case CSSPropertyCounterIncrement:
        if (id == CSSValueNone)
            validPrimitive = true;
        else
            parsedValue = parseCounter(1);
        break;
    case CSSPropertyCounterReset:
        if (id == CSSValueNone)
            validPrimitive = true;
        else
            parsedValue = parseCounter(0);
        break;
    case CSSPropertyFontFamily:
        // [[ <family-name> | <generic-family> ],]* [<family-name> | <generic-family>] | inherit
    {
        parsedValue = parseFontFamily();
        break;
    }

    case CSSPropertyTextDecoration:
        // Fall through 'text-decoration-line' parsing if CSS 3 Text Decoration
        // is disabled to match CSS 2.1 rules for parsing 'text-decoration'.
        if (RuntimeEnabledFeatures::css3TextDecorationsEnabled()) {
            // [ <text-decoration-line> || <text-decoration-style> || <text-decoration-color> ] | inherit
            return parseShorthand(CSSPropertyTextDecoration, textDecorationShorthand(), important);
        }
    case CSSPropertyWebkitTextDecorationsInEffect:
    case CSSPropertyTextDecorationLine:
        // none | [ underline || overline || line-through || blink ] | inherit
        return parseTextDecoration(propId, important);

    case CSSPropertyTextUnderlinePosition:
        // auto | [ under || [ left | right ] ], but we only support auto | under for now
        ASSERT(RuntimeEnabledFeatures::css3TextDecorationsEnabled());
        validPrimitive = (id == CSSValueAuto || id == CSSValueUnder);
        break;

    case CSSPropertyZoom:          // normal | reset | document | <number> | <percentage> | inherit
        if (id == CSSValueNormal || id == CSSValueReset || id == CSSValueDocument)
            validPrimitive = true;
        else
            validPrimitive = validUnit(value, FNumber | FPercent | FNonNeg);
        if (validPrimitive && m_context.useCounter()
            && !(id == CSSValueNormal
                || (value->unit == CSSPrimitiveValue::CSS_NUMBER && value->fValue == 1)
                || (value->unit == CSSPrimitiveValue::CSS_PERCENTAGE && value->fValue == 100)))
            m_context.useCounter()->count(UseCounter::CSSZoomNotEqualToOne);
        break;

    case CSSPropertySrc:
    case CSSPropertyUnicodeRange:
        /* @font-face only descriptors */
        break;

    /* CSS3 properties */

    case CSSPropertyBorderImage:
    case CSSPropertyWebkitMaskBoxImage:
        return parseBorderImageShorthand(propId, important);
    case CSSPropertyWebkitBorderImage: {
        if (RefPtrWillBeRawPtr<CSSValue> result = parseBorderImage(propId)) {
            addProperty(propId, result, important);
            return true;
        }
        return false;
    }

    case CSSPropertyBorderImageOutset:
    case CSSPropertyWebkitMaskBoxImageOutset: {
        RefPtrWillBeRawPtr<CSSPrimitiveValue> result = nullptr;
        if (parseBorderImageOutset(result)) {
            addProperty(propId, result, important);
            return true;
        }
        break;
    }
    case CSSPropertyBorderImageRepeat:
    case CSSPropertyWebkitMaskBoxImageRepeat: {
        RefPtrWillBeRawPtr<CSSValue> result = nullptr;
        if (parseBorderImageRepeat(result)) {
            addProperty(propId, result, important);
            return true;
        }
        break;
    }
    case CSSPropertyBorderImageSlice:
    case CSSPropertyWebkitMaskBoxImageSlice: {
        RefPtrWillBeRawPtr<CSSBorderImageSliceValue> result = nullptr;
        if (parseBorderImageSlice(propId, result)) {
            addProperty(propId, result, important);
            return true;
        }
        break;
    }
    case CSSPropertyBorderImageWidth:
    case CSSPropertyWebkitMaskBoxImageWidth: {
        RefPtrWillBeRawPtr<CSSPrimitiveValue> result = nullptr;
        if (parseBorderImageWidth(result)) {
            addProperty(propId, result, important);
            return true;
        }
        break;
    }
    case CSSPropertyBorderTopRightRadius:
    case CSSPropertyBorderTopLeftRadius:
    case CSSPropertyBorderBottomLeftRadius:
    case CSSPropertyBorderBottomRightRadius: {
        if (num != 1 && num != 2)
            return false;
        validPrimitive = validUnit(value, FLength | FPercent | FNonNeg);
        if (!validPrimitive)
            return false;
        RefPtrWillBeRawPtr<CSSPrimitiveValue> parsedValue1 = createPrimitiveNumericValue(value);
        RefPtrWillBeRawPtr<CSSPrimitiveValue> parsedValue2 = nullptr;
        if (num == 2) {
            value = m_valueList->next();
            validPrimitive = validUnit(value, FLength | FPercent | FNonNeg);
            if (!validPrimitive)
                return false;
            parsedValue2 = createPrimitiveNumericValue(value);
        } else
            parsedValue2 = parsedValue1;

        addProperty(propId, createPrimitiveValuePair(parsedValue1.release(), parsedValue2.release()), important);
        return true;
    }
    case CSSPropertyTabSize:
        // May be specified as a unit-less non-negative integer or length indicating number of space characters.
        validPrimitive = validUnit(value, FInteger | FLength | FNonNeg);
        break;
    case CSSPropertyBorderRadius:
    case CSSPropertyAliasWebkitBorderRadius:
        return parseBorderRadius(unresolvedProperty, important);
    case CSSPropertyOutlineOffset:
        validPrimitive = validUnit(value, FLength);
        break;
    case CSSPropertyTextShadow: // CSS2 property, dropped in CSS2.1, back in CSS3, so treat as CSS3
    case CSSPropertyBoxShadow:
        if (id == CSSValueNone)
            validPrimitive = true;
        else {
            RefPtrWillBeRawPtr<CSSValueList> shadowValueList = parseShadow(m_valueList, propId);
            if (shadowValueList) {
                addProperty(propId, shadowValueList.release(), important);
                m_valueList->next();
                return true;
            }
            return false;
        }
        break;
    case CSSPropertyWebkitBoxReflect:
        if (id == CSSValueNone)
            validPrimitive = true;
        else
            parsedValue = parseReflect();
        break;
    case CSSPropertyFontSizeAdjust: // none | <number>
        ASSERT(RuntimeEnabledFeatures::cssFontSizeAdjustEnabled());
        validPrimitive = (id == CSSValueNone) ? true : validUnit(value, FNumber | FNonNeg);
        break;
    case CSSPropertyOpacity:
    case CSSPropertyWebkitBoxFlex:
        validPrimitive = validUnit(value, FNumber);
        break;
    case CSSPropertyWebkitBoxFlexGroup:
        validPrimitive = validUnit(value, FInteger | FNonNeg);
        break;
    case CSSPropertyWebkitBoxOrdinalGroup:
        validPrimitive = validUnit(value, FInteger | FNonNeg) && value->fValue;
        break;
    case CSSPropertyWebkitFilter:
        if (id == CSSValueNone)
            validPrimitive = true;
        else {
            RefPtrWillBeRawPtr<CSSValue> val = parseFilter();
            if (val) {
                addProperty(propId, val, important);
                return true;
            }
            return false;
        }
        break;
    case CSSPropertyFlex: {
        ShorthandScope scope(this, propId);
        if (id == CSSValueNone) {
            addProperty(CSSPropertyFlexGrow, cssValuePool().createValue(0, CSSPrimitiveValue::CSS_NUMBER), important);
            addProperty(CSSPropertyFlexShrink, cssValuePool().createValue(0, CSSPrimitiveValue::CSS_NUMBER), important);
            addProperty(CSSPropertyFlexBasis, cssValuePool().createIdentifierValue(CSSValueAuto), important);
            return true;
        }
        return parseFlex(m_valueList, important);
    }
    case CSSPropertyFlexBasis:
        // FIXME: Support intrinsic dimensions too.
        if (id == CSSValueAuto)
            validPrimitive = true;
        else
            validPrimitive = validUnit(value, FLength | FPercent | FNonNeg);
        break;
    case CSSPropertyFlexGrow:
    case CSSPropertyFlexShrink:
        validPrimitive = validUnit(value, FNumber | FNonNeg);
        break;
    case CSSPropertyOrder:
        validPrimitive = validUnit(value, FInteger);
        break;
    case CSSPropertyTransform:
        if (id == CSSValueNone)
            validPrimitive = true;
        else
            parsedValue = parseTransform(unresolvedProperty == CSSPropertyAliasWebkitTransform);
        break;
    case CSSPropertyTransformOrigin: {
        RefPtrWillBeRawPtr<CSSValueList> list = parseTransformOrigin();
        if (!list)
            return false;
        // These values are added to match gecko serialization.
        if (list->length() == 1)
            list->append(cssValuePool().createValue(50, CSSPrimitiveValue::CSS_PERCENTAGE));
        if (list->length() == 2)
            list->append(cssValuePool().createValue(0, CSSPrimitiveValue::CSS_PX));
        addProperty(propId, list.release(), important);
        return true;
    }

    case CSSPropertyTranslate: {
        // translate : [ <length> | <percentage> ] [[ <length> | <percentage> ] <length>? ]?
        // defaults to 0 on all axis, note that the last value CANNOT be a percentage
        ASSERT(RuntimeEnabledFeatures::cssIndependentTransformPropertiesEnabled());
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
        if (!validUnit(value, FLength | FPercent))
            return false;

        list->append(createPrimitiveNumericValue(value));
        value = m_valueList->next();

        if (value) {
            if (!validUnit(value, FLength | FPercent))
                return false;

            list->append(createPrimitiveNumericValue(value));
            value = m_valueList->next();

            if (value) {
                if (!validUnit(value, FLength))
                    return false;

                list->append(createPrimitiveNumericValue(value));
                value = m_valueList->next();
            }
        }

        parsedValue = list.release();
        break;
    }

    case CSSPropertyRotate: { // rotate : <angle> <number>{3}? defaults to a 0 0 1
        ASSERT(RuntimeEnabledFeatures::cssIndependentTransformPropertiesEnabled());
        RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();

        if (!validUnit(value, FAngle))
            return false;
        list->append(createPrimitiveNumericValue(value));
        value = m_valueList->next();

        if (!value) {
            parsedValue = list.release();
            break;
        }

        for (unsigned i = 0; i < 3; i++) { // 3 dimensions of rotation
            if (!value || !validUnit(value, FNumber))
                return false;
            list->append(createPrimitiveNumericValue(value));
            value = m_valueList->next();
        }

        parsedValue = list.release();
        break;
    }

    case CSSPropertyScale: { // scale: <number>{1,3}, default scale for all axis is 1
        ASSERT(RuntimeEnabledFeatures::cssIndependentTransformPropertiesEnabled());
        RefPtrWillBeRawPtr<CSSValueList> scaleList = CSSValueList::createSpaceSeparated();

        for (unsigned i = 0; value && i < 3; i++) { // up to 3 dimensions of scale
            if (!validUnit(value, FNumber))
                return false;
            scaleList->append(createPrimitiveNumericValue(value));
            value = m_valueList->next();
        }

        parsedValue = scaleList.release();
        break;
    }

    case CSSPropertyWebkitPerspectiveOriginX:
    case CSSPropertyWebkitTransformOriginX:
        parsedValue = parseFillPositionX(m_valueList);
        if (parsedValue)
            m_valueList->next();
        break;
    case CSSPropertyWebkitPerspectiveOriginY:
    case CSSPropertyWebkitTransformOriginY:
        parsedValue = parseFillPositionY(m_valueList);
        if (parsedValue)
            m_valueList->next();
        break;
    case CSSPropertyWebkitTransformOriginZ:
        validPrimitive = validUnit(value, FLength);
        break;
    case CSSPropertyPerspective:
        if (id == CSSValueNone) {
            validPrimitive = true;
        } else if (validUnit(value, FLength) && (m_parsedCalculation || value->fValue > 0)) {
            validPrimitive = true;
        } else if (unresolvedProperty == CSSPropertyAliasWebkitPerspective && validUnit(value, FNumber) && value->fValue > 0) {
            value->unit = CSSPrimitiveValue::CSS_PX;
            validPrimitive = true;
        } else {
            return false;
        }
        break;
    case CSSPropertyPerspectiveOrigin: {
        RefPtrWillBeRawPtr<CSSValueList> list = parseTransformOrigin();
        if (!list || list->length() == 3)
            return false;
        // This values are added to match gecko serialization.
        if (list->length() == 1)
            list->append(cssValuePool().createValue(50, CSSPrimitiveValue::CSS_PERCENTAGE));
        addProperty(propId, list.release(), important);
        return true;
    }

    case CSSPropertyMotion:
        // <motion-path> && <motion-offset> && <motion-rotation>
        ASSERT(RuntimeEnabledFeatures::cssMotionPathEnabled());
        return parseShorthand(propId, motionShorthand(), important);
    case CSSPropertyMotionPath:
        ASSERT(RuntimeEnabledFeatures::cssMotionPathEnabled());
        if (id == CSSValueNone)
            validPrimitive = true;
        else
            parsedValue = parseMotionPath();
        break;
    case CSSPropertyMotionOffset:
        ASSERT(RuntimeEnabledFeatures::cssMotionPathEnabled());
        validPrimitive = validUnit(value, FLength | FPercent);
        break;
    case CSSPropertyMotionRotation:
        ASSERT(RuntimeEnabledFeatures::cssMotionPathEnabled());
        parsedValue = parseMotionRotation();
        break;

    case CSSPropertyAnimationDelay:
    case CSSPropertyAnimationDirection:
    case CSSPropertyAnimationDuration:
    case CSSPropertyAnimationFillMode:
    case CSSPropertyAnimationName:
    case CSSPropertyAnimationPlayState:
    case CSSPropertyAnimationIterationCount:
    case CSSPropertyAnimationTimingFunction:
    case CSSPropertyTransitionDelay:
    case CSSPropertyTransitionDuration:
    case CSSPropertyTransitionTimingFunction:
    case CSSPropertyTransitionProperty:
        parsedValue = parseAnimationPropertyList(propId, unresolvedProperty == CSSPropertyAliasWebkitAnimationName);
        break;

    case CSSPropertyJustifyContent:
        parsedValue = parseContentDistributionOverflowPosition();
        break;
    case CSSPropertyJustifySelf:
        ASSERT(RuntimeEnabledFeatures::cssGridLayoutEnabled());
        return parseItemPositionOverflowPosition(propId, important);
    case CSSPropertyJustifyItems:
        ASSERT(RuntimeEnabledFeatures::cssGridLayoutEnabled());

        if (parseLegacyPosition(propId, important))
            return true;

        m_valueList->setCurrentIndex(0);
        return parseItemPositionOverflowPosition(propId, important);
    case CSSPropertyGridAutoFlow:
        ASSERT(RuntimeEnabledFeatures::cssGridLayoutEnabled());
        parsedValue = parseGridAutoFlow(*m_valueList);
        break;
    case CSSPropertyGridAutoColumns:
    case CSSPropertyGridAutoRows:
        ASSERT(RuntimeEnabledFeatures::cssGridLayoutEnabled());
        parsedValue = parseGridTrackSize(*m_valueList);
        break;

    case CSSPropertyGridTemplateColumns:
    case CSSPropertyGridTemplateRows:
        ASSERT(RuntimeEnabledFeatures::cssGridLayoutEnabled());
        parsedValue = parseGridTrackList();
        break;

    case CSSPropertyGridColumnEnd:
    case CSSPropertyGridColumnStart:
    case CSSPropertyGridRowEnd:
    case CSSPropertyGridRowStart:
        ASSERT(RuntimeEnabledFeatures::cssGridLayoutEnabled());
        parsedValue = parseGridPosition();
        break;

    case CSSPropertyGridColumn:
    case CSSPropertyGridRow:
        ASSERT(RuntimeEnabledFeatures::cssGridLayoutEnabled());
        return parseGridItemPositionShorthand(propId, important);

    case CSSPropertyGridArea:
        ASSERT(RuntimeEnabledFeatures::cssGridLayoutEnabled());
        return parseGridAreaShorthand(important);

    case CSSPropertyGridTemplateAreas:
        ASSERT(RuntimeEnabledFeatures::cssGridLayoutEnabled());
        parsedValue = parseGridTemplateAreas();
        break;

    case CSSPropertyGridTemplate:
        ASSERT(RuntimeEnabledFeatures::cssGridLayoutEnabled());
        return parseGridTemplateShorthand(important);

    case CSSPropertyGrid:
        ASSERT(RuntimeEnabledFeatures::cssGridLayoutEnabled());
        return parseGridShorthand(important);

    case CSSPropertyWebkitMarginCollapse: {
        if (num == 1) {
            ShorthandScope scope(this, CSSPropertyWebkitMarginCollapse);
            if (!parseValue(webkitMarginCollapseShorthand().properties()[0], important))
                return false;
            CSSValue* value = m_parsedProperties.last().value();
            addProperty(webkitMarginCollapseShorthand().properties()[1], value, important);
            return true;
        }
        else if (num == 2) {
            ShorthandScope scope(this, CSSPropertyWebkitMarginCollapse);
            if (!parseValue(webkitMarginCollapseShorthand().properties()[0], important) || !parseValue(webkitMarginCollapseShorthand().properties()[1], important))
                return false;
            return true;
        }
        return false;
    }
    case CSSPropertyWebkitColumnCount:
        parsedValue = parseColumnCount();
        break;
    case CSSPropertyWebkitColumnGap:         // normal | <length>
        if (id == CSSValueNormal)
            validPrimitive = true;
        else
            validPrimitive = validUnit(value, FLength | FNonNeg);
        break;
    case CSSPropertyWebkitColumnSpan: // none | all | 1 (will be dropped in the unprefixed property)
        validPrimitive = id == CSSValueAll || id == CSSValueNone || (value->unit == CSSPrimitiveValue::CSS_NUMBER && value->fValue == 1);
        break;
    case CSSPropertyWebkitColumnWidth:         // auto | <length>
        parsedValue = parseColumnWidth();
        break;
    case CSSPropertyWillChange:
        parsedValue = parseWillChange();
        break;
    // End of CSS3 properties

    // Apple specific properties.  These will never be standardized and are purely to
    // support custom WebKit-based Apple applications.
    case CSSPropertyWebkitLineClamp:
        // When specifying number of lines, don't allow 0 as a valid value
        // When specifying either type of unit, require non-negative integers
        validPrimitive = (!id && (value->unit == CSSPrimitiveValue::CSS_PERCENTAGE || value->fValue) && validUnit(value, FInteger | FPercent | FNonNeg));
        break;

    case CSSPropertyWebkitFontSizeDelta: // <length>
        validPrimitive = validUnit(value, FLength | FUnitlessQuirk);
        break;

    case CSSPropertyWebkitHighlight:
        if (id == CSSValueNone) {
            validPrimitive = true;
        } else if (value->unit == CSSPrimitiveValue::CSS_STRING) {
            parsedValue = createPrimitiveStringValue(value);
            m_valueList->next();
        }
        break;

    case CSSPropertyWebkitHyphenateCharacter:
    case CSSPropertyWebkitLocale:
        if (id == CSSValueAuto) {
            validPrimitive = true;
        } else if (value->unit == CSSPrimitiveValue::CSS_STRING) {
            parsedValue = createPrimitiveStringValue(value);
            m_valueList->next();
        }
        break;

    // End Apple-specific properties

    case CSSPropertyWebkitAppRegion:
        if (id >= CSSValueDrag && id <= CSSValueNoDrag)
            validPrimitive = true;
        break;

    case CSSPropertyWebkitTapHighlightColor:
        parsedValue = parseTapHighlightColor(m_valueList->current());
        if (parsedValue)
            m_valueList->next();
        break;

        /* shorthand properties */
    case CSSPropertyBackground: {
        // Position must come before color in this array because a plain old "0" is a legal color
        // in quirks mode but it's usually the X coordinate of a position.
        const CSSPropertyID properties[] = { CSSPropertyBackgroundImage, CSSPropertyBackgroundRepeat,
                                   CSSPropertyBackgroundAttachment, CSSPropertyBackgroundPosition, CSSPropertyBackgroundOrigin,
                                   CSSPropertyBackgroundClip, CSSPropertyBackgroundColor, CSSPropertyBackgroundSize };
        return parseFillShorthand(propId, properties, WTF_ARRAY_LENGTH(properties), important);
    }
    case CSSPropertyWebkitMask: {
        const CSSPropertyID properties[] = { CSSPropertyWebkitMaskImage, CSSPropertyWebkitMaskRepeat,
            CSSPropertyWebkitMaskPosition, CSSPropertyWebkitMaskOrigin, CSSPropertyWebkitMaskClip, CSSPropertyWebkitMaskSize };
        return parseFillShorthand(propId, properties, WTF_ARRAY_LENGTH(properties), important);
    }
    case CSSPropertyBorder:
        // [ 'border-width' || 'border-style' || <color> ] | inherit
    {
        if (parseShorthand(propId, borderShorthandForParsing(), important)) {
            // The CSS3 Borders and Backgrounds specification says that border also resets border-image. It's as
            // though a value of none was specified for the image.
            addExpandedPropertyForValue(CSSPropertyBorderImage, cssValuePool().createImplicitInitialValue(), important);
            return true;
        }
        return false;
    }
    case CSSPropertyBorderTop:
        // [ 'border-top-width' || 'border-style' || <color> ] | inherit
        return parseShorthand(propId, borderTopShorthand(), important);
    case CSSPropertyBorderRight:
        // [ 'border-right-width' || 'border-style' || <color> ] | inherit
        return parseShorthand(propId, borderRightShorthand(), important);
    case CSSPropertyBorderBottom:
        // [ 'border-bottom-width' || 'border-style' || <color> ] | inherit
        return parseShorthand(propId, borderBottomShorthand(), important);
    case CSSPropertyBorderLeft:
        // [ 'border-left-width' || 'border-style' || <color> ] | inherit
        return parseShorthand(propId, borderLeftShorthand(), important);
    case CSSPropertyWebkitBorderStart:
        return parseShorthand(propId, webkitBorderStartShorthand(), important);
    case CSSPropertyWebkitBorderEnd:
        return parseShorthand(propId, webkitBorderEndShorthand(), important);
    case CSSPropertyWebkitBorderBefore:
        return parseShorthand(propId, webkitBorderBeforeShorthand(), important);
    case CSSPropertyWebkitBorderAfter:
        return parseShorthand(propId, webkitBorderAfterShorthand(), important);
    case CSSPropertyOutline:
        // [ 'outline-color' || 'outline-style' || 'outline-width' ] | inherit
        return parseShorthand(propId, outlineShorthand(), important);
    case CSSPropertyBorderColor:
        // <color>{1,4} | inherit
        return parse4Values(propId, borderColorShorthand().properties(), important);
    case CSSPropertyBorderWidth:
        // <border-width>{1,4} | inherit
        return parse4Values(propId, borderWidthShorthand().properties(), important);
    case CSSPropertyBorderStyle:
        // <border-style>{1,4} | inherit
        return parse4Values(propId, borderStyleShorthand().properties(), important);
    case CSSPropertyMargin:
        // <margin-width>{1,4} | inherit
        return parse4Values(propId, marginShorthand().properties(), important);
    case CSSPropertyPadding:
        // <padding-width>{1,4} | inherit
        return parse4Values(propId, paddingShorthand().properties(), important);
    case CSSPropertyFlexFlow:
        return parseShorthand(propId, flexFlowShorthand(), important);
    case CSSPropertyFont:
        // [ [ 'font-style' || 'font-variant' || 'font-weight' ]? 'font-size' [ / 'line-height' ]?
        // 'font-family' ] | caption | icon | menu | message-box | small-caption | status-bar | inherit
        if (num == 1 && id >= CSSValueCaption && id <= CSSValueStatusBar) {
            parseSystemFont(important);
            return true;
        }
        return parseFont(important);
    case CSSPropertyListStyle:
        return parseShorthand(propId, listStyleShorthand(), important);
    case CSSPropertyWebkitColumns:
        return parseColumnsShorthand(important);
    case CSSPropertyWebkitColumnRule:
        return parseShorthand(propId, webkitColumnRuleShorthand(), important);
    case CSSPropertyWebkitTextStroke:
        return parseShorthand(propId, webkitTextStrokeShorthand(), important);
    case CSSPropertyAnimation:
        return parseAnimationShorthand(unresolvedProperty == CSSPropertyAliasWebkitAnimation, important);
    case CSSPropertyTransition:
        return parseTransitionShorthand(important);
    case CSSPropertyInvalid:
        return false;
    case CSSPropertyPage:
        parsedValue = parsePage();
        break;
    // CSS Text Layout Module Level 3: Vertical writing support
    case CSSPropertyWebkitTextEmphasis:
        return parseShorthand(propId, webkitTextEmphasisShorthand(), important);

    case CSSPropertyWebkitTextEmphasisStyle:
        parsedValue = parseTextEmphasisStyle();
        break;

    case CSSPropertyWebkitTextOrientation:
        // FIXME: For now just support sideways, sideways-right, upright and vertical-right.
        if (id == CSSValueSideways || id == CSSValueSidewaysRight || id == CSSValueVerticalRight || id == CSSValueUpright)
            validPrimitive = true;
        break;

//     case CSSPropertyWebkitLineBoxContain:
//         if (id == CSSValueNone)
//             validPrimitive = true;
//         else
//             parsedValue = parseLineBoxContain();
//         break;
    case CSSPropertyWebkitFontFeatureSettings:
        if (id == CSSValueNormal)
            validPrimitive = true;
        else
            parsedValue = parseFontFeatureSettings();
        break;

    case CSSPropertyFontVariantLigatures:
        if (id == CSSValueNormal)
            validPrimitive = true;
        else
            return parseFontVariantLigatures(important);
        break;
    case CSSPropertyWebkitClipPath:
        if (id == CSSValueNone) {
            validPrimitive = true;
        } else if (value->unit == CSSParserValue::Function) {
            parsedValue = parseBasicShape();
        } else if (value->unit == CSSPrimitiveValue::CSS_URI) {
            parsedValue = CSSPrimitiveValue::create(value->string, CSSPrimitiveValue::CSS_URI);
            addProperty(propId, parsedValue.release(), important);
            return true;
        }
        break;
    case CSSPropertyShapeOutside:
        if (id == CSSValueNone)
            validPrimitive = true;
        else
            parsedValue = parseShapeProperty(propId);
        break;
    case CSSPropertyShapeMargin:
        validPrimitive = validUnit(value, FLength | FPercent | FNonNeg);
        break;
    case CSSPropertyShapeImageThreshold:
        validPrimitive = validUnit(value, FNumber);
        break;

    case CSSPropertyTouchAction:
        parsedValue = parseTouchAction();
        break;

//     case CSSPropertyScrollBlocksOn:
//         if (id == CSSValueNone)
//             validPrimitive = true;
//         else
//             parsedValue = parseScrollBlocksOn();
//         break;

    case CSSPropertyAlignContent:
        parsedValue = parseContentDistributionOverflowPosition();
        break;

    case CSSPropertyAlignSelf:
        ASSERT(RuntimeEnabledFeatures::cssGridLayoutEnabled());
        return parseItemPositionOverflowPosition(propId, important);

    case CSSPropertyAlignItems:
        ASSERT(RuntimeEnabledFeatures::cssGridLayoutEnabled());
        return parseItemPositionOverflowPosition(propId, important);

    // Properties below are validated inside parseViewportProperty, because we
    // check for parser state. We need to invalidate if someone adds them outside
    // a @viewport rule.
    case CSSPropertyMaxZoom:
    case CSSPropertyMinZoom:
    case CSSPropertyOrientation:
    case CSSPropertyUserZoom:
        validPrimitive = false;
        break;

    case CSSPropertyScrollSnapPointsX:
    case CSSPropertyScrollSnapPointsY:
        parsedValue = parseScrollSnapPoints();
        break;
    case CSSPropertyScrollSnapCoordinate:
        parsedValue = parseScrollSnapCoordinate();
        break;
    case CSSPropertyScrollSnapDestination:
        parsedValue = parsePosition(m_valueList);
        break;

    case CSSPropertyBackdropFilter:
    case CSSPropertyFontFeatureSettings:
    case CSSPropertyImageOrientation:
        break;

    default:
        return parseSVGValue(propId, important);
    }

    if (validPrimitive) {
        parsedValue = parseValidPrimitive(id, value);
        m_valueList->next();
    }
    ASSERT(!m_parsedCalculation);
    if (parsedValue) {
        if (!m_valueList->current() || inShorthand()) {
            addProperty(propId, parsedValue.release(), important);
            return true;
        }
    }
    return false;
}

void CSSPropertyParser::addFillValue(RefPtrWillBeRawPtr<CSSValue>& lval, PassRefPtrWillBeRawPtr<CSSValue> rval)
{
    if (lval) {
        if (lval->isBaseValueList())
            toCSSValueList(lval.get())->append(rval);
        else {
            PassRefPtrWillBeRawPtr<CSSValue> oldlVal(lval.release());
            PassRefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
            list->append(oldlVal);
            list->append(rval);
            lval = list;
        }
    }
    else
        lval = rval;
}

static bool parseBackgroundClip(CSSParserValue* parserValue, RefPtrWillBeRawPtr<CSSValue>& cssValue)
{
    if (parserValue->id == CSSValueBorderBox || parserValue->id == CSSValuePaddingBox
        || parserValue->id == CSSValueContentBox || parserValue->id == CSSValueWebkitText) {
        cssValue = cssValuePool().createIdentifierValue(parserValue->id);
        return true;
    }
    return false;
}

const int cMaxFillProperties = 9;

bool CSSPropertyParser::parseFillShorthand(CSSPropertyID propId, const CSSPropertyID* properties, int numProperties, bool important)
{
    ASSERT(numProperties <= cMaxFillProperties);
    if (numProperties > cMaxFillProperties)
        return false;

    ShorthandScope scope(this, propId);

    bool parsedProperty[cMaxFillProperties] = { false };
    RefPtrWillBeRawPtr<CSSValue> values[cMaxFillProperties];
#if ENABLE(OILPAN)
    // Zero initialize the array of raw pointers.
    memset(&values, 0, sizeof(values));
#endif
    RefPtrWillBeRawPtr<CSSValue> clipValue = nullptr;
    RefPtrWillBeRawPtr<CSSValue> positionYValue = nullptr;
    RefPtrWillBeRawPtr<CSSValue> repeatYValue = nullptr;
    bool foundClip = false;
    int i;
    bool foundPositionCSSProperty = false;

    while (m_valueList->current()) {
        CSSParserValue* val = m_valueList->current();
        if (isComma(val)) {
            // We hit the end.  Fill in all remaining values with the initial value.
            m_valueList->next();
            for (i = 0; i < numProperties; ++i) {
                if (properties[i] == CSSPropertyBackgroundColor && parsedProperty[i])
                    // Color is not allowed except as the last item in a list for backgrounds.
                    // Reject the entire property.
                    return false;

                if (!parsedProperty[i] && properties[i] != CSSPropertyBackgroundColor) {
                    addFillValue(values[i], cssValuePool().createImplicitInitialValue());
                    if (properties[i] == CSSPropertyBackgroundPosition || properties[i] == CSSPropertyWebkitMaskPosition)
                        addFillValue(positionYValue, cssValuePool().createImplicitInitialValue());
                    if (properties[i] == CSSPropertyBackgroundRepeat || properties[i] == CSSPropertyWebkitMaskRepeat)
                        addFillValue(repeatYValue, cssValuePool().createImplicitInitialValue());
                    if ((properties[i] == CSSPropertyBackgroundOrigin || properties[i] == CSSPropertyWebkitMaskOrigin) && !parsedProperty[i]) {
                        // If background-origin wasn't present, then reset background-clip also.
                        addFillValue(clipValue, cssValuePool().createImplicitInitialValue());
                    }
                }
                parsedProperty[i] = false;
            }
            if (!m_valueList->current())
                break;
        }

        bool sizeCSSPropertyExpected = false;
        if (isForwardSlashOperator(val) && foundPositionCSSProperty) {
            sizeCSSPropertyExpected = true;
            m_valueList->next();
        }

        foundPositionCSSProperty = false;
        bool found = false;
        for (i = 0; !found && i < numProperties; ++i) {

            if (sizeCSSPropertyExpected && (properties[i] != CSSPropertyBackgroundSize && properties[i] != CSSPropertyWebkitMaskSize))
                continue;
            if (!sizeCSSPropertyExpected && (properties[i] == CSSPropertyBackgroundSize || properties[i] == CSSPropertyWebkitMaskSize))
                continue;

            if (!parsedProperty[i]) {
                RefPtrWillBeRawPtr<CSSValue> val1 = nullptr;
                RefPtrWillBeRawPtr<CSSValue> val2 = nullptr;
                CSSPropertyID propId1, propId2;
                CSSParserValue* parserValue = m_valueList->current();
                // parseFillProperty() may modify m_implicitShorthand, so we MUST reset it
                // before EACH return below.
                if (parserValue && parseFillProperty(properties[i], propId1, propId2, val1, val2)) {
                    parsedProperty[i] = found = true;
                    addFillValue(values[i], val1.release());
                    if (properties[i] == CSSPropertyBackgroundPosition || properties[i] == CSSPropertyWebkitMaskPosition)
                        addFillValue(positionYValue, val2.release());
                    if (properties[i] == CSSPropertyBackgroundRepeat || properties[i] == CSSPropertyWebkitMaskRepeat)
                        addFillValue(repeatYValue, val2.release());
                    if (properties[i] == CSSPropertyBackgroundOrigin || properties[i] == CSSPropertyWebkitMaskOrigin) {
                        // Reparse the value as a clip, and see if we succeed.
                        if (parseBackgroundClip(parserValue, val1))
                            addFillValue(clipValue, val1.release()); // The property parsed successfully.
                        else
                            addFillValue(clipValue, cssValuePool().createImplicitInitialValue()); // Some value was used for origin that is not supported by clip. Just reset clip instead.
                    }
                    if (properties[i] == CSSPropertyBackgroundClip || properties[i] == CSSPropertyWebkitMaskClip) {
                        // Update clipValue
                        addFillValue(clipValue, val1.release());
                        foundClip = true;
                    }
                    if (properties[i] == CSSPropertyBackgroundPosition || properties[i] == CSSPropertyWebkitMaskPosition)
                        foundPositionCSSProperty = true;
                }
            }
        }

        // if we didn't find at least one match, this is an
        // invalid shorthand and we have to ignore it
        if (!found) {
            m_implicitShorthand = false;
            return false;
        }
    }

    // Now add all of the properties we found.
    for (i = 0; i < numProperties; i++) {
        // Fill in any remaining properties with the initial value.
        if (!parsedProperty[i]) {
            addFillValue(values[i], cssValuePool().createImplicitInitialValue());
            if (properties[i] == CSSPropertyBackgroundPosition || properties[i] == CSSPropertyWebkitMaskPosition)
                addFillValue(positionYValue, cssValuePool().createImplicitInitialValue());
            if (properties[i] == CSSPropertyBackgroundRepeat || properties[i] == CSSPropertyWebkitMaskRepeat)
                addFillValue(repeatYValue, cssValuePool().createImplicitInitialValue());
            if (properties[i] == CSSPropertyBackgroundOrigin || properties[i] == CSSPropertyWebkitMaskOrigin) {
                // If background-origin wasn't present, then reset background-clip also.
                addFillValue(clipValue, cssValuePool().createImplicitInitialValue());
            }
        }
        if (properties[i] == CSSPropertyBackgroundPosition) {
            addProperty(CSSPropertyBackgroundPositionX, values[i].release(), important);
            // it's OK to call positionYValue.release() since we only see CSSPropertyBackgroundPosition once
            addProperty(CSSPropertyBackgroundPositionY, positionYValue.release(), important);
        } else if (properties[i] == CSSPropertyWebkitMaskPosition) {
            addProperty(CSSPropertyWebkitMaskPositionX, values[i].release(), important);
            // it's OK to call positionYValue.release() since we only see CSSPropertyWebkitMaskPosition once
            addProperty(CSSPropertyWebkitMaskPositionY, positionYValue.release(), important);
        } else if (properties[i] == CSSPropertyBackgroundRepeat) {
            addProperty(CSSPropertyBackgroundRepeatX, values[i].release(), important);
            // it's OK to call repeatYValue.release() since we only see CSSPropertyBackgroundPosition once
            addProperty(CSSPropertyBackgroundRepeatY, repeatYValue.release(), important);
        } else if (properties[i] == CSSPropertyWebkitMaskRepeat) {
            addProperty(CSSPropertyWebkitMaskRepeatX, values[i].release(), important);
            // it's OK to call repeatYValue.release() since we only see CSSPropertyBackgroundPosition once
            addProperty(CSSPropertyWebkitMaskRepeatY, repeatYValue.release(), important);
        } else if ((properties[i] == CSSPropertyBackgroundClip || properties[i] == CSSPropertyWebkitMaskClip) && !foundClip)
            // Value is already set while updating origin
            continue;
        else if (properties[i] == CSSPropertyBackgroundSize && !parsedProperty[i] && m_context.useLegacyBackgroundSizeShorthandBehavior())
            continue;
        else
            addProperty(properties[i], values[i].release(), important);

        // Add in clip values when we hit the corresponding origin property.
        if (properties[i] == CSSPropertyBackgroundOrigin && !foundClip)
            addProperty(CSSPropertyBackgroundClip, clipValue.release(), important);
        else if (properties[i] == CSSPropertyWebkitMaskOrigin && !foundClip)
            addProperty(CSSPropertyWebkitMaskClip, clipValue.release(), important);
    }

    m_implicitShorthand = false;
    return true;
}

static bool isValidTransitionPropertyList(CSSValueList* value)
{
    if (value->length() < 2)
        return true;
    for (auto& property : *value) {
        // FIXME: Shorthand parsing shouldn't add initial to the list since it won't round-trip
        if (property->isInitialValue())
            continue;
        CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(property.get());
        if (primitiveValue->isValueID() && primitiveValue->getValueID() == CSSValueNone)
            return false;
    }
    return true;
}

bool CSSPropertyParser::parseAnimationShorthand(bool useLegacyparsing, bool important)
{
    const StylePropertyShorthand& animationProperties = animationShorthandForParsing();
    const unsigned numProperties = 8;

    // The list of properties in the shorthand should be the same
    // length as the list with animation name in last position, even though they are
    // in a different order.
    ASSERT(numProperties == animationProperties.length());
    ASSERT(numProperties == animationShorthand().length());

    ShorthandScope scope(this, CSSPropertyAnimation);

    bool parsedProperty[numProperties] = { false };
    RefPtrWillBeRawPtr<CSSValueList> values[numProperties];
    for (size_t i = 0; i < numProperties; ++i)
        values[i] = CSSValueList::createCommaSeparated();

    while (m_valueList->current()) {
        if (consumeComma(m_valueList)) {
            // We hit the end. Fill in all remaining values with the initial value.
            for (size_t i = 0; i < numProperties; ++i) {
                if (!parsedProperty[i])
                    values[i]->append(cssValuePool().createImplicitInitialValue());
                parsedProperty[i] = false;
            }
            if (!m_valueList->current())
                break;
        }

        bool found = false;
        for (size_t i = 0; i < numProperties; ++i) {
            if (parsedProperty[i])
                continue;
            if (RefPtrWillBeRawPtr<CSSValue> val = parseAnimationProperty(animationProperties.properties()[i], useLegacyparsing)) {
                parsedProperty[i] = found = true;
                values[i]->append(val.release());
                break;
            }
        }

        // if we didn't find at least one match, this is an
        // invalid shorthand and we have to ignore it
        if (!found)
            return false;
    }

    for (size_t i = 0; i < numProperties; ++i) {
        // If we didn't find the property, set an intial value.
        if (!parsedProperty[i])
            values[i]->append(cssValuePool().createImplicitInitialValue());

        addProperty(animationProperties.properties()[i], values[i].release(), important);
    }

    return true;
}

bool CSSPropertyParser::parseTransitionShorthand(bool important)
{
    const unsigned numProperties = 4;
    const StylePropertyShorthand& shorthand = transitionShorthandForParsing();
    ASSERT(numProperties == shorthand.length());

    ShorthandScope scope(this, CSSPropertyTransition);

    bool parsedProperty[numProperties] = { false };
    RefPtrWillBeRawPtr<CSSValueList> values[numProperties];
    for (size_t i = 0; i < numProperties; ++i)
        values[i] = CSSValueList::createCommaSeparated();

    while (m_valueList->current()) {
        if (consumeComma(m_valueList)) {
            // We hit the end. Fill in all remaining values with the initial value.
            for (size_t i = 0; i < numProperties; ++i) {
                if (!parsedProperty[i])
                    values[i]->append(cssValuePool().createImplicitInitialValue());
                parsedProperty[i] = false;
            }
            if (!m_valueList->current())
                break;
        }

        bool found = false;
        for (size_t i = 0; i < numProperties; ++i) {
            if (parsedProperty[i])
                continue;
            if (RefPtrWillBeRawPtr<CSSValue> val = parseAnimationProperty(shorthand.properties()[i], false)) {
                parsedProperty[i] = found = true;
                values[i]->append(val.release());
                break;
            }
        }

        // if we didn't find at least one match, this is an
        // invalid shorthand and we have to ignore it
        if (!found)
            return false;
    }

    ASSERT(shorthand.properties()[3] == CSSPropertyTransitionProperty);
    if (!isValidTransitionPropertyList(values[3].get()))
        return false;

    // Fill in any remaining properties with the initial value and add
    for (size_t i = 0; i < numProperties; ++i) {
        if (!parsedProperty[i])
            values[i]->append(cssValuePool().createImplicitInitialValue());
        addProperty(shorthand.properties()[i], values[i].release(), important);
    }

    return true;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseColumnWidth()
{
    CSSParserValue* value = m_valueList->current();
    // Always parse lengths in strict mode here, since it would be ambiguous otherwise when used in
    // the 'columns' shorthand property.
    if (value->id == CSSValueAuto || (validUnit(value, FLength | FNonNeg, HTMLStandardMode) && (m_parsedCalculation || value->fValue != 0))) {
        RefPtrWillBeRawPtr<CSSValue> parsedValue = parseValidPrimitive(value->id, value);
        m_valueList->next();
        return parsedValue;
    }
    return nullptr;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseColumnCount()
{
    CSSParserValue* value = m_valueList->current();
    if (value->id == CSSValueAuto || validUnit(value, FPositiveInteger)) {
        RefPtrWillBeRawPtr<CSSValue> parsedValue = parseValidPrimitive(value->id, value);
        m_valueList->next();
        return parsedValue;
    }
    return nullptr;
}

bool CSSPropertyParser::parseColumnsShorthand(bool important)
{
    RefPtrWillBeRawPtr<CSSValue> columnWidth = nullptr;
    RefPtrWillBeRawPtr<CSSValue> columnCount = nullptr;
    bool hasPendingExplicitAuto = false;

    for (unsigned propertiesParsed = 0; CSSParserValue* value = m_valueList->current(); propertiesParsed++) {
        if (propertiesParsed >= 2)
            return false; // Too many values for this shorthand. Invalid declaration.
        if (!propertiesParsed && value->id == CSSValueAuto) {
            // 'auto' is a valid value for any of the two longhands, and at this point we
            // don't know which one(s) it is meant for. We need to see if there are other
            // values first.
            m_valueList->next();
            hasPendingExplicitAuto = true;
        } else {
            if (!columnWidth) {
                if ((columnWidth = parseColumnWidth()))
                    continue;
            }
            if (!columnCount) {
                if ((columnCount = parseColumnCount()))
                    continue;
            }
            // If we didn't find at least one match, this is an
            // invalid shorthand and we have to ignore it.
            return false;
        }
    }
    if (hasPendingExplicitAuto) {
        // Time to assign the previously skipped 'auto' value to a property. If both properties are
        // unassigned at this point (i.e. 'columns:auto'), it doesn't matter that much which one we
        // set (although it does make a slight difference to web-inspector). The one we don't set
        // here will get an implicit 'auto' value further down.
        if (!columnWidth) {
            columnWidth = cssValuePool().createIdentifierValue(CSSValueAuto);
        } else {
            ASSERT(!columnCount);
            columnCount = cssValuePool().createIdentifierValue(CSSValueAuto);
        }
    }
    ASSERT(columnCount || columnWidth);

    // Any unassigned property at this point will become implicit 'auto'.
    if (columnWidth)
        addProperty(CSSPropertyWebkitColumnWidth, columnWidth, important);
    else
        addProperty(CSSPropertyWebkitColumnWidth, cssValuePool().createIdentifierValue(CSSValueAuto), important, true /* implicit */);
    if (columnCount)
        addProperty(CSSPropertyWebkitColumnCount, columnCount, important);
    else
        addProperty(CSSPropertyWebkitColumnCount, cssValuePool().createIdentifierValue(CSSValueAuto), important, true /* implicit */);
    return true;
}

bool CSSPropertyParser::parseShorthand(CSSPropertyID propId, const StylePropertyShorthand& shorthand, bool important)
{
    // We try to match as many properties as possible
    // We set up an array of booleans to mark which property has been found,
    // and we try to search for properties until it makes no longer any sense.
    ShorthandScope scope(this, propId);

    bool found = false;
    unsigned propertiesParsed = 0;
    bool propertyFound[6] = { false, false, false, false, false, false }; // 6 is enough size.

    while (m_valueList->current()) {
        found = false;
        for (unsigned propIndex = 0; !found && propIndex < shorthand.length(); ++propIndex) {
            if (!propertyFound[propIndex] && parseValue(shorthand.properties()[propIndex], important)) {
                propertyFound[propIndex] = found = true;
                propertiesParsed++;
            }
        }

        // if we didn't find at least one match, this is an
        // invalid shorthand and we have to ignore it
        if (!found)
            return false;
    }

    if (propertiesParsed == shorthand.length())
        return true;

    // Fill in any remaining properties with the initial value.
    ImplicitScope implicitScope(this);
    const StylePropertyShorthand* const* const propertiesForInitialization = shorthand.propertiesForInitialization();
    for (unsigned i = 0; i < shorthand.length(); ++i) {
        if (propertyFound[i])
            continue;

        if (propertiesForInitialization) {
            const StylePropertyShorthand& initProperties = *(propertiesForInitialization[i]);
            for (unsigned propIndex = 0; propIndex < initProperties.length(); ++propIndex)
                addProperty(initProperties.properties()[propIndex], cssValuePool().createImplicitInitialValue(), important);
        } else
            addProperty(shorthand.properties()[i], cssValuePool().createImplicitInitialValue(), important);
    }

    return true;
}

bool CSSPropertyParser::parse4Values(CSSPropertyID propId, const CSSPropertyID *properties,  bool important)
{
    /* From the CSS 2 specs, 8.3
     * If there is only one value, it applies to all sides. If there are two values, the top and
     * bottom margins are set to the first value and the right and left margins are set to the second.
     * If there are three values, the top is set to the first value, the left and right are set to the
     * second, and the bottom is set to the third. If there are four values, they apply to the top,
     * right, bottom, and left, respectively.
     */

    int num = inShorthand() ? 1 : m_valueList->size();

    ShorthandScope scope(this, propId);

    // the order is top, right, bottom, left
    switch (num) {
        case 1: {
            if (!parseValue(properties[0], important))
                return false;
            CSSValue* value = m_parsedProperties.last().value();
            ImplicitScope implicitScope(this);
            addProperty(properties[1], value, important);
            addProperty(properties[2], value, important);
            addProperty(properties[3], value, important);
            break;
        }
        case 2: {
            if (!parseValue(properties[0], important) || !parseValue(properties[1], important))
                return false;
            CSSValue* value = m_parsedProperties[m_parsedProperties.size() - 2].value();
            ImplicitScope implicitScope(this);
            addProperty(properties[2], value, important);
            value = m_parsedProperties[m_parsedProperties.size() - 2].value();
            addProperty(properties[3], value, important);
            break;
        }
        case 3: {
            if (!parseValue(properties[0], important) || !parseValue(properties[1], important) || !parseValue(properties[2], important))
                return false;
            CSSValue* value = m_parsedProperties[m_parsedProperties.size() - 2].value();
            ImplicitScope implicitScope(this);
            addProperty(properties[3], value, important);
            break;
        }
        case 4: {
            if (!parseValue(properties[0], important) || !parseValue(properties[1], important) ||
                !parseValue(properties[2], important) || !parseValue(properties[3], important))
                return false;
            break;
        }
        default: {
            return false;
        }
    }

    return true;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseScrollSnapPoints()
{
    CSSParserValue* value = m_valueList->current();

    if (value->id == CSSValueNone) {
        m_valueList->next();
        return cssValuePool().createIdentifierValue(CSSValueNone);
    }

    if (value->unit == CSSParserValue::Function && value->function->id == CSSValueRepeat) {
        // The spec defines the following grammar: repeat( <length>)
        CSSParserValueList* arguments = value->function->args.get();
        if (!arguments || arguments->size() != 1)
            return nullptr;

        CSSParserValue* repeatValue = arguments->valueAt(0);
        if (validUnit(repeatValue, FNonNeg | FLength | FPercent) && (m_parsedCalculation || repeatValue->fValue > 0)) {
            RefPtrWillBeRawPtr<CSSFunctionValue> result = CSSFunctionValue::create(CSSValueRepeat);
            result->append(parseValidPrimitive(repeatValue->id, repeatValue));
            m_valueList->next();
            return result.release();
        }
    }

    return nullptr;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseScrollSnapCoordinate()
{
    if (m_valueList->current()->id == CSSValueNone) {
        m_valueList->next();
        return cssValuePool().createIdentifierValue(CSSValueNone);
    }

    return parsePositionList(m_valueList);
}

PassRefPtrWillBeRawPtr<CSSPrimitiveValue> CSSPropertyParser::parsePage()
{
    CSSParserValue* value = m_valueList->current();
    m_valueList->next();
    ASSERT(value);

    if (value->id == CSSValueAuto)
        return cssValuePool().createIdentifierValue(value->id);
    if (value->unit == CSSPrimitiveValue::CSS_IDENT)
        return createPrimitiveCustomIdentValue(value);
    return nullptr;
}

// <length>{1,2} | auto | [ <page-size> || [ portrait | landscape] ]
PassRefPtrWillBeRawPtr<CSSValueList> CSSPropertyParser::parseSize()
{
    CSSParserValue* value = m_valueList->current();
    ASSERT(value);

    RefPtrWillBeRawPtr<CSSValueList> parsedValues = CSSValueList::createSpaceSeparated();

    // First parameter.
    SizeParameterType paramType = parseSizeParameter(parsedValues.get(), value, None);
    if (paramType == None)
        return nullptr;
    value = m_valueList->next();

    // Second parameter, if any.
    if (value) {
        paramType = parseSizeParameter(parsedValues.get(), value, paramType);
        if (paramType == None)
            return nullptr;
        m_valueList->next();
    }

    return parsedValues.release();
}

CSSPropertyParser::SizeParameterType CSSPropertyParser::parseSizeParameter(CSSValueList* parsedValues, CSSParserValue* value, SizeParameterType prevParamType)
{
    switch (value->id) {
    case CSSValueAuto:
        if (prevParamType == None) {
            parsedValues->append(cssValuePool().createIdentifierValue(value->id));
            return Auto;
        }
        return None;
    case CSSValueLandscape:
    case CSSValuePortrait:
        if (prevParamType == None || prevParamType == PageSize) {
            parsedValues->append(cssValuePool().createIdentifierValue(value->id));
            return Orientation;
        }
        return None;
    case CSSValueA3:
    case CSSValueA4:
    case CSSValueA5:
    case CSSValueB4:
    case CSSValueB5:
    case CSSValueLedger:
    case CSSValueLegal:
    case CSSValueLetter:
        if (prevParamType == None || prevParamType == Orientation) {
            // Normalize to Page Size then Orientation order by prepending for simpler StyleBuilder handling
            parsedValues->prepend(cssValuePool().createIdentifierValue(value->id));
            return PageSize;
        }
        return None;
    case 0:
        if (validUnit(value, FLength | FNonNeg) && (prevParamType == None || prevParamType == Length)) {
            parsedValues->append(createPrimitiveNumericValue(value));
            return Length;
        }
        return None;
    default:
        return None;
    }
}

// [ <string> <string> ]+ | none, but none is handled in parseValue
PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseQuotes()
{
    RefPtrWillBeRawPtr<CSSValueList> values = CSSValueList::createSpaceSeparated();
    while (CSSParserValue* val = m_valueList->current()) {
        RefPtrWillBeRawPtr<CSSValue> parsedValue = nullptr;
        if (val->unit != CSSPrimitiveValue::CSS_STRING)
            return nullptr;
        parsedValue = createPrimitiveStringValue(val);
        values->append(parsedValue.release());
        m_valueList->next();
    }
    if (values->length() && values->length() % 2 == 0)
        return values.release();
    return nullptr;
}

// [ <string> | <uri> | <counter> | attr(X) | open-quote | close-quote | no-open-quote | no-close-quote ]+ | inherit
// in CSS 2.1 this got somewhat reduced:
// [ <string> | attr(X) | open-quote | close-quote | no-open-quote | no-close-quote ]+ | inherit
PassRefPtrWillBeRawPtr<CSSValueList> CSSPropertyParser::parseContent()
{
    RefPtrWillBeRawPtr<CSSValueList> values = CSSValueList::createSpaceSeparated();

    while (CSSParserValue* val = m_valueList->current()) {
        RefPtrWillBeRawPtr<CSSValue> parsedValue = nullptr;
        if (val->unit == CSSPrimitiveValue::CSS_URI) {
            // url
            parsedValue = createCSSImageValueWithReferrer(val->string, completeURL(val->string));
        } else if (val->unit == CSSParserValue::Function) {
            // attr(X) | counter(X [,Y]) | counters(X, Y, [,Z]) | -webkit-gradient(...)
            CSSParserValueList* args = val->function->args.get();
            if (!args)
                return nullptr;
            if (val->function->id == CSSValueAttr) {
                parsedValue = parseAttr(args);
            } else if (val->function->id == CSSValueCounter) {
                parsedValue = parseCounterContent(args, false);
            } else if (val->function->id == CSSValueCounters) {
                parsedValue = parseCounterContent(args, true);
            } else if (val->function->id == CSSValueWebkitImageSet) {
                parsedValue = parseImageSet(m_valueList);
            } else if (isGeneratedImageValue(val)) {
                if (!parseGeneratedImage(m_valueList, parsedValue))
                    return nullptr;
            }
        } else if (val->unit == CSSPrimitiveValue::CSS_IDENT) {
            switch (val->id) {
            case CSSValueOpenQuote:
            case CSSValueCloseQuote:
            case CSSValueNoOpenQuote:
            case CSSValueNoCloseQuote:
            case CSSValueNone:
            case CSSValueNormal:
                parsedValue = cssValuePool().createIdentifierValue(val->id);
            default:
                break;
            }
        } else if (val->unit == CSSPrimitiveValue::CSS_STRING) {
            parsedValue = createPrimitiveStringValue(val);
        }
        if (!parsedValue)
            return nullptr;
        values->append(parsedValue.release());
        m_valueList->next();
    }

    return values.release();
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseAttr(CSSParserValueList* args)
{
    if (args->size() != 1)
        return nullptr;

    CSSParserValue* a = args->current();

    if (a->unit != CSSPrimitiveValue::CSS_IDENT)
        return nullptr;

    String attrName = a->string;
    // CSS allows identifiers with "-" at the start, like "-webkit-mask-image".
    // But HTML attribute names can't have those characters, and we should not
    // even parse them inside attr().
    if (attrName[0] == '-')
        return nullptr;

    if (m_context.isHTMLDocument())
        attrName = attrName.lower();

    return cssValuePool().createValue(attrName, CSSPrimitiveValue::CSS_ATTR);
}

bool CSSPropertyParser::acceptQuirkyColors(CSSPropertyID propertyId) const
{
    if (!inQuirksMode())
        return false;
    switch (propertyId) {
    case CSSPropertyBackgroundColor:
    case CSSPropertyBorderBottomColor:
    case CSSPropertyBorderLeftColor:
    case CSSPropertyBorderRightColor:
    case CSSPropertyBorderTopColor:
        return !inShorthand() || m_currentShorthand == CSSPropertyBorderColor;
    case CSSPropertyColor:
        return true;
    default:
        break;
    }
    return false;
}

bool CSSPropertyParser::isColorKeyword(CSSValueID id)
{
    // Named colors and color keywords:
    //
    // <named-color>
    //   'aqua', 'black', 'blue', ..., 'yellow' (CSS3: "basic color keywords")
    //   'aliceblue', ..., 'yellowgreen'        (CSS3: "extended color keywords")
    //   'transparent'
    //
    // 'currentcolor'
    //
    // <deprecated-system-color>
    //   'ActiveBorder', ..., 'WindowText'
    //
    // WebKit proprietary/internal:
    //   '-webkit-link'
    //   '-webkit-activelink'
    //   '-internal-active-list-box-selection'
    //   '-internal-active-list-box-selection-text'
    //   '-internal-inactive-list-box-selection'
    //   '-internal-inactive-list-box-selection-text'
    //   '-webkit-focus-ring-color'
    //
    // TODO(fs): The "extended color keywords" are not included.
    return (id >= CSSValueAqua && id <= CSSValueWebkitText) || id == CSSValueMenu;
}

PassRefPtrWillBeRawPtr<CSSPrimitiveValue> CSSPropertyParser::parseColor(const CSSParserValue* value, bool acceptQuirkyColors)
{
    CSSValueID id = value->id;
    if (isColorKeyword(id)) {
        if (!isValueAllowedInMode(id, m_context.mode()))
            return nullptr;
        return cssValuePool().createIdentifierValue(id);
    }
    RGBA32 c = Color::transparent;
    if (!parseColorFromValue(value, c, acceptQuirkyColors))
        return nullptr;
    return cssValuePool().createColorValue(c);
}

// Used to parse background-color when part of a shorthand.
PassRefPtrWillBeRawPtr<CSSPrimitiveValue> CSSPropertyParser::parseBackgroundColor(const CSSParserValue* value)
{
    CSSValueID id = value->id;
    // Allow -webkit-text regardless of quirks.
    if (id == CSSValueWebkitText)
        return cssValuePool().createIdentifierValue(id);
    return parseColor(value);
}

// Used to parse the '-webkit-tap-highlight-color' property.
PassRefPtrWillBeRawPtr<CSSPrimitiveValue> CSSPropertyParser::parseTapHighlightColor(const CSSParserValue* value)
{
    CSSValueID id = value->id;
    // Disallow -webkit-text regardless of quirks.
    if (id == CSSValueWebkitText)
        return nullptr;
    return parseColor(value);
}

// Used to parse <color> for CSS gradients.
PassRefPtrWillBeRawPtr<CSSPrimitiveValue> CSSPropertyParser::parseGradientStopColor(const CSSParserValue* value)
{
    CSSValueID id = value->id;
    // Allow -webkit-text regardless of quirks.
    if (id == CSSValueWebkitText)
        return cssValuePool().createIdentifierValue(id);
    return parseColor(value);
}

// Used to parse colors for -webkit-gradient(...).
PassRefPtrWillBeRawPtr<CSSPrimitiveValue> CSSPropertyParser::parseDeprecatedGradientStopColor(const CSSParserValue* value)
{
    // Disallow currentcolor.
    if (value->id == CSSValueCurrentcolor)
        return nullptr;
    return parseGradientStopColor(value);
}

bool CSSPropertyParser::parseFillImage(CSSParserValueList* valueList, RefPtrWillBeRawPtr<CSSValue>& value)
{
    if (valueList->current()->id == CSSValueNone) {
        value = cssValuePool().createIdentifierValue(CSSValueNone);
        return true;
    }
    if (valueList->current()->unit == CSSPrimitiveValue::CSS_URI) {
        value = createCSSImageValueWithReferrer(valueList->current()->string, completeURL(valueList->current()->string));
        return true;
    }

    if (isGeneratedImageValue(valueList->current()))
        return parseGeneratedImage(valueList, value);

    if (valueList->current()->unit == CSSParserValue::Function && valueList->current()->function->id == CSSValueWebkitImageSet) {
        value = parseImageSet(m_valueList);
        if (value)
            return true;
    }

    return false;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseFillPositionX(CSSParserValueList* valueList)
{
    int id = valueList->current()->id;
    if (id == CSSValueLeft || id == CSSValueRight || id == CSSValueCenter) {
        int percent = 0;
        if (id == CSSValueRight)
            percent = 100;
        else if (id == CSSValueCenter)
            percent = 50;
        return cssValuePool().createValue(percent, CSSPrimitiveValue::CSS_PERCENTAGE);
    }
    if (validUnit(valueList->current(), FPercent | FLength))
        return createPrimitiveNumericValue(valueList->current());
    return nullptr;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseFillPositionY(CSSParserValueList* valueList)
{
    int id = valueList->current()->id;
    if (id == CSSValueTop || id == CSSValueBottom || id == CSSValueCenter) {
        int percent = 0;
        if (id == CSSValueBottom)
            percent = 100;
        else if (id == CSSValueCenter)
            percent = 50;
        return cssValuePool().createValue(percent, CSSPrimitiveValue::CSS_PERCENTAGE);
    }
    if (validUnit(valueList->current(), FPercent | FLength))
        return createPrimitiveNumericValue(valueList->current());
    return nullptr;
}

PassRefPtrWillBeRawPtr<CSSPrimitiveValue> CSSPropertyParser::parseFillPositionComponent(CSSParserValueList* valueList, unsigned& cumulativeFlags, FillPositionFlag& individualFlag, FillPositionParsingMode parsingMode, Units unitless)
{
    CSSValueID id = valueList->current()->id;
    if (id == CSSValueLeft || id == CSSValueTop || id == CSSValueRight || id == CSSValueBottom || id == CSSValueCenter) {
        int percent = 0;
        if (id == CSSValueLeft || id == CSSValueRight) {
            if (cumulativeFlags & XFillPosition)
                return nullptr;
            cumulativeFlags |= XFillPosition;
            individualFlag = XFillPosition;
            if (id == CSSValueRight)
                percent = 100;
        }
        else if (id == CSSValueTop || id == CSSValueBottom) {
            if (cumulativeFlags & YFillPosition)
                return nullptr;
            cumulativeFlags |= YFillPosition;
            individualFlag = YFillPosition;
            if (id == CSSValueBottom)
                percent = 100;
        } else if (id == CSSValueCenter) {
            // Center is ambiguous, so we're not sure which position we've found yet, an x or a y.
            percent = 50;
            cumulativeFlags |= AmbiguousFillPosition;
            individualFlag = AmbiguousFillPosition;
        }

        if (parsingMode == ResolveValuesAsKeyword)
            return cssValuePool().createIdentifierValue(id);

        return cssValuePool().createValue(percent, CSSPrimitiveValue::CSS_PERCENTAGE);
    }
    if (validUnit(valueList->current(), FPercent | FLength | unitless)) {
        if (!cumulativeFlags) {
            cumulativeFlags |= XFillPosition;
            individualFlag = XFillPosition;
        } else if (cumulativeFlags & (XFillPosition | AmbiguousFillPosition)) {
            cumulativeFlags |= YFillPosition;
            individualFlag = YFillPosition;
        } else {
            if (m_parsedCalculation)
                m_parsedCalculation.release();
            return nullptr;
        }
        return createPrimitiveNumericValue(valueList->current());
    }
    return nullptr;
}

static bool isValueConflictingWithCurrentEdge(int value1, int value2)
{
    if ((value1 == CSSValueLeft || value1 == CSSValueRight) && (value2 == CSSValueLeft || value2 == CSSValueRight))
        return true;

    if ((value1 == CSSValueTop || value1 == CSSValueBottom) && (value2 == CSSValueTop || value2 == CSSValueBottom))
        return true;

    return false;
}

static bool isFillPositionKeyword(CSSValueID value)
{
    return value == CSSValueLeft || value == CSSValueTop || value == CSSValueBottom || value == CSSValueRight || value == CSSValueCenter;
}

void CSSPropertyParser::parse4ValuesFillPosition(CSSParserValueList* valueList, RefPtrWillBeRawPtr<CSSValue>& value1, RefPtrWillBeRawPtr<CSSValue>& value2, PassRefPtrWillBeRawPtr<CSSPrimitiveValue> parsedValue1, PassRefPtrWillBeRawPtr<CSSPrimitiveValue> parsedValue2)
{
    // [ left | right ] [ <percentage] | <length> ] && [ top | bottom ] [ <percentage> | <length> ]
    // In the case of 4 values <position> requires the second value to be a length or a percentage.
    if (isFillPositionKeyword(parsedValue2->getValueID()))
        return;

    unsigned cumulativeFlags = 0;
    FillPositionFlag value3Flag = InvalidFillPosition;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> value3 = parseFillPositionComponent(valueList, cumulativeFlags, value3Flag, ResolveValuesAsKeyword);
    if (!value3)
        return;

    CSSValueID ident1 = parsedValue1->getValueID();
    CSSValueID ident3 = value3->getValueID();

    if (ident1 == CSSValueCenter)
        return;

    if (!isFillPositionKeyword(ident3) || ident3 == CSSValueCenter)
        return;

    // We need to check if the values are not conflicting, e.g. they are not on the same edge. It is
    // needed as the second call to parseFillPositionComponent was on purpose not checking it. In the
    // case of two values top 20px is invalid but in the case of 4 values it becomes valid.
    if (isValueConflictingWithCurrentEdge(ident1, ident3))
        return;

    valueList->next();

    cumulativeFlags = 0;
    FillPositionFlag value4Flag = InvalidFillPosition;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> value4 = parseFillPositionComponent(valueList, cumulativeFlags, value4Flag, ResolveValuesAsKeyword);
    if (!value4)
        return;

    // 4th value must be a length or a percentage.
    if (isFillPositionKeyword(value4->getValueID()))
        return;

    value1 = createPrimitiveValuePair(parsedValue1, parsedValue2);
    value2 = createPrimitiveValuePair(value3, value4);

    if (ident1 == CSSValueTop || ident1 == CSSValueBottom)
        value1.swap(value2);

    valueList->next();
}
void CSSPropertyParser::parse3ValuesFillPosition(CSSParserValueList* valueList, RefPtrWillBeRawPtr<CSSValue>& value1, RefPtrWillBeRawPtr<CSSValue>& value2, PassRefPtrWillBeRawPtr<CSSPrimitiveValue> parsedValue1, PassRefPtrWillBeRawPtr<CSSPrimitiveValue> parsedValue2)
{
    unsigned cumulativeFlags = 0;
    FillPositionFlag value3Flag = InvalidFillPosition;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> value3 = parseFillPositionComponent(valueList, cumulativeFlags, value3Flag, ResolveValuesAsKeyword);

    // value3 is not an expected value, we return.
    if (!value3)
        return;

    valueList->next();

    bool swapNeeded = false;
    CSSValueID ident1 = parsedValue1->getValueID();
    CSSValueID ident2 = parsedValue2->getValueID();
    CSSValueID ident3 = value3->getValueID();

    CSSValueID firstPositionKeyword;
    CSSValueID secondPositionKeyword;

    if (ident1 == CSSValueCenter) {
        // <position> requires the first 'center' to be followed by a keyword.
        if (!isFillPositionKeyword(ident2))
            return;

        // If 'center' is the first keyword then the last one needs to be a length.
        if (isFillPositionKeyword(ident3))
            return;

        firstPositionKeyword = CSSValueLeft;
        if (ident2 == CSSValueLeft || ident2 == CSSValueRight) {
            firstPositionKeyword = CSSValueTop;
            swapNeeded = true;
        }
        value1 = createPrimitiveValuePair(cssValuePool().createIdentifierValue(firstPositionKeyword), cssValuePool().createValue(50, CSSPrimitiveValue::CSS_PERCENTAGE));
        value2 = createPrimitiveValuePair(parsedValue2, value3);
    } else if (ident3 == CSSValueCenter) {
        if (isFillPositionKeyword(ident2))
            return;

        secondPositionKeyword = CSSValueTop;
        if (ident1 == CSSValueTop || ident1 == CSSValueBottom) {
            secondPositionKeyword = CSSValueLeft;
            swapNeeded = true;
        }
        value1 = createPrimitiveValuePair(parsedValue1, parsedValue2);
        value2 = createPrimitiveValuePair(cssValuePool().createIdentifierValue(secondPositionKeyword), cssValuePool().createValue(50, CSSPrimitiveValue::CSS_PERCENTAGE));
    } else {
        RefPtrWillBeRawPtr<CSSPrimitiveValue> firstPositionValue = nullptr;
        RefPtrWillBeRawPtr<CSSPrimitiveValue> secondPositionValue = nullptr;

        if (isFillPositionKeyword(ident2)) {
            // To match CSS grammar, we should only accept: [ center | left | right | bottom | top ] [ left | right | top | bottom ] [ <percentage> | <length> ].
            ASSERT(ident2 != CSSValueCenter);

            if (isFillPositionKeyword(ident3))
                return;

            secondPositionValue = value3;
            secondPositionKeyword = ident2;
            firstPositionValue = cssValuePool().createValue(0, CSSPrimitiveValue::CSS_PERCENTAGE);
        } else {
            // Per CSS, we should only accept: [ right | left | top | bottom ] [ <percentage> | <length> ] [ center | left | right | bottom | top ].
            if (!isFillPositionKeyword(ident3))
                return;

            firstPositionValue = parsedValue2;
            secondPositionKeyword = ident3;
            secondPositionValue = cssValuePool().createValue(0, CSSPrimitiveValue::CSS_PERCENTAGE);
        }

        if (isValueConflictingWithCurrentEdge(ident1, secondPositionKeyword))
            return;

        value1 = createPrimitiveValuePair(parsedValue1, firstPositionValue);
        value2 = createPrimitiveValuePair(cssValuePool().createIdentifierValue(secondPositionKeyword), secondPositionValue);
    }

    if (ident1 == CSSValueTop || ident1 == CSSValueBottom || swapNeeded)
        value1.swap(value2);

#if ENABLE(ASSERT)
    CSSPrimitiveValue* first = toCSSPrimitiveValue(value1.get());
    CSSPrimitiveValue* second = toCSSPrimitiveValue(value2.get());
    ident1 = first->getPairValue()->first()->getValueID();
    ident2 = second->getPairValue()->first()->getValueID();
    ASSERT(ident1 == CSSValueLeft || ident1 == CSSValueRight);
    ASSERT(ident2 == CSSValueBottom || ident2 == CSSValueTop);
#endif
}

inline bool CSSPropertyParser::isPotentialPositionValue(CSSParserValue* value)
{
    return isFillPositionKeyword(value->id) || validUnit(value, FPercent | FLength, ReleaseParsedCalcValue);
}

void CSSPropertyParser::parseFillPosition(CSSParserValueList* valueList, RefPtrWillBeRawPtr<CSSValue>& value1, RefPtrWillBeRawPtr<CSSValue>& value2, Units unitless)
{
    unsigned numberOfValues = 0;
    for (unsigned i = valueList->currentIndex(); i < valueList->size(); ++i, ++numberOfValues) {
        CSSParserValue* current = valueList->valueAt(i);
        if (!current || isComma(current) || isForwardSlashOperator(current) || !isPotentialPositionValue(current))
            break;
    }

    if (numberOfValues > 4)
        return;

    // If we are parsing two values, we can safely call the CSS 2.1 parsing function and return.
    if (numberOfValues <= 2) {
        parse2ValuesFillPosition(valueList, value1, value2, unitless);
        return;
    }

    ASSERT(numberOfValues > 2 && numberOfValues <= 4);

    CSSParserValue* value = valueList->current();

    // <position> requires the first value to be a background keyword.
    if (!isFillPositionKeyword(value->id))
        return;

    // Parse the first value. We're just making sure that it is one of the valid keywords or a percentage/length.
    unsigned cumulativeFlags = 0;
    FillPositionFlag value1Flag = InvalidFillPosition;
    FillPositionFlag value2Flag = InvalidFillPosition;
    value1 = parseFillPositionComponent(valueList, cumulativeFlags, value1Flag, ResolveValuesAsKeyword);
    if (!value1)
        return;

    valueList->next();

    // In case we are parsing more than two values, relax the check inside of parseFillPositionComponent. top 20px is
    // a valid start for <position>.
    cumulativeFlags = AmbiguousFillPosition;
    value2 = parseFillPositionComponent(valueList, cumulativeFlags, value2Flag, ResolveValuesAsKeyword);
    if (value2)
        valueList->next();
    else {
        value1.clear();
        return;
    }

    RefPtrWillBeRawPtr<CSSPrimitiveValue> parsedValue1 = toCSSPrimitiveValue(value1.get());
    RefPtrWillBeRawPtr<CSSPrimitiveValue> parsedValue2 = toCSSPrimitiveValue(value2.get());

    value1.clear();
    value2.clear();

    // Per CSS3 syntax, <position> can't have 'center' as its second keyword as we have more arguments to follow.
    if (parsedValue2->getValueID() == CSSValueCenter)
        return;

    if (numberOfValues == 3)
        parse3ValuesFillPosition(valueList, value1, value2, parsedValue1.release(), parsedValue2.release());
    else
        parse4ValuesFillPosition(valueList, value1, value2, parsedValue1.release(), parsedValue2.release());
}

void CSSPropertyParser::parse2ValuesFillPosition(CSSParserValueList* valueList, RefPtrWillBeRawPtr<CSSValue>& value1, RefPtrWillBeRawPtr<CSSValue>& value2, Units unitless)
{
    // Parse the first value.  We're just making sure that it is one of the valid keywords or a percentage/length.
    unsigned cumulativeFlags = 0;
    FillPositionFlag value1Flag = InvalidFillPosition;
    FillPositionFlag value2Flag = InvalidFillPosition;
    value1 = parseFillPositionComponent(valueList, cumulativeFlags, value1Flag, ResolveValuesAsPercent, unitless);
    if (!value1)
        return;

    // It only takes one value for background-position to be correctly parsed if it was specified in a shorthand (since we
    // can assume that any other values belong to the rest of the shorthand).  If we're not parsing a shorthand, though, the
    // value was explicitly specified for our property.
    CSSParserValue* value = valueList->next();

    // First check for the comma.  If so, we are finished parsing this value or value pair.
    if (value && isComma(value))
        value = 0;

    if (value) {
        value2 = parseFillPositionComponent(valueList, cumulativeFlags, value2Flag, ResolveValuesAsPercent, unitless);
        if (value2)
            valueList->next();
        else {
            if (!inShorthand()) {
                value1.clear();
                return;
            }
        }
    }

    if (!value2)
        // Only one value was specified. If that value was not a keyword, then it sets the x position, and the y position
        // is simply 50%. This is our default.
        // For keywords, the keyword was either an x-keyword (left/right), a y-keyword (top/bottom), or an ambiguous keyword (center).
        // For left/right/center, the default of 50% in the y is still correct.
        value2 = cssValuePool().createValue(50, CSSPrimitiveValue::CSS_PERCENTAGE);

    if (value1Flag == YFillPosition || value2Flag == XFillPosition)
        value1.swap(value2);
}

void CSSPropertyParser::parseFillRepeat(RefPtrWillBeRawPtr<CSSValue>& value1, RefPtrWillBeRawPtr<CSSValue>& value2)
{
    CSSValueID id = m_valueList->current()->id;
    if (id == CSSValueRepeatX) {
        m_implicitShorthand = true;
        value1 = cssValuePool().createIdentifierValue(CSSValueRepeat);
        value2 = cssValuePool().createIdentifierValue(CSSValueNoRepeat);
        m_valueList->next();
        return;
    }
    if (id == CSSValueRepeatY) {
        m_implicitShorthand = true;
        value1 = cssValuePool().createIdentifierValue(CSSValueNoRepeat);
        value2 = cssValuePool().createIdentifierValue(CSSValueRepeat);
        m_valueList->next();
        return;
    }
    if (id == CSSValueRepeat || id == CSSValueNoRepeat || id == CSSValueRound || id == CSSValueSpace)
        value1 = cssValuePool().createIdentifierValue(id);
    else {
        value1 = nullptr;
        return;
    }

    CSSParserValue* value = m_valueList->next();

    // Parse the second value if one is available
    if (value && !isComma(value)) {
        id = value->id;
        if (id == CSSValueRepeat || id == CSSValueNoRepeat || id == CSSValueRound || id == CSSValueSpace) {
            value2 = cssValuePool().createIdentifierValue(id);
            m_valueList->next();
            return;
        }
    }

    // If only one value was specified, value2 is the same as value1.
    m_implicitShorthand = true;
    value2 = cssValuePool().createIdentifierValue(toCSSPrimitiveValue(value1.get())->getValueID());
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseFillSize(CSSPropertyID unresolvedProperty)
{
    CSSParserValue* value = m_valueList->current();
    m_valueList->next();

    if (value->id == CSSValueContain || value->id == CSSValueCover)
        return cssValuePool().createIdentifierValue(value->id);

    RefPtrWillBeRawPtr<CSSPrimitiveValue> parsedValue1 = nullptr;

    if (value->id == CSSValueAuto)
        parsedValue1 = cssValuePool().createIdentifierValue(CSSValueAuto);
    else {
        if (!validUnit(value, FLength | FPercent))
            return nullptr;
        parsedValue1 = createPrimitiveNumericValue(value);
    }

    RefPtrWillBeRawPtr<CSSPrimitiveValue> parsedValue2 = nullptr;
    value = m_valueList->current();
    if (value) {
        if (value->id == CSSValueAuto) {
            // `auto' is the default
            m_valueList->next();
        } else if (validUnit(value, FLength | FPercent)) {
            parsedValue2 = createPrimitiveNumericValue(value);
            m_valueList->next();
        }
    } else if (unresolvedProperty == CSSPropertyAliasWebkitBackgroundSize) {
        // For backwards compatibility we set the second value to the first if it is omitted.
        // We only need to do this for -webkit-background-size. It should be safe to let masks match
        // the real property.
        parsedValue2 = parsedValue1;
    }

    if (!parsedValue2)
        return parsedValue1;

    return createPrimitiveValuePair(parsedValue1.release(), parsedValue2.release(), Pair::KeepIdenticalValues);
}

bool CSSPropertyParser::parseFillProperty(CSSPropertyID propId, CSSPropertyID& propId1, CSSPropertyID& propId2,
    RefPtrWillBeRawPtr<CSSValue>& retValue1, RefPtrWillBeRawPtr<CSSValue>& retValue2)
{
    // We initially store the first value in value/value2, and only create
    // CSSValueLists if we have more values.
    RefPtrWillBeRawPtr<CSSValueList> values = nullptr;
    RefPtrWillBeRawPtr<CSSValueList> values2 = nullptr;
    RefPtrWillBeRawPtr<CSSValue> value = nullptr;
    RefPtrWillBeRawPtr<CSSValue> value2 = nullptr;

    retValue1 = retValue2 = nullptr;
    propId1 = resolveCSSPropertyID(propId);
    propId2 = propId1;
    if (propId == CSSPropertyBackgroundPosition) {
        propId1 = CSSPropertyBackgroundPositionX;
        propId2 = CSSPropertyBackgroundPositionY;
    } else if (propId == CSSPropertyWebkitMaskPosition) {
        propId1 = CSSPropertyWebkitMaskPositionX;
        propId2 = CSSPropertyWebkitMaskPositionY;
    } else if (propId == CSSPropertyBackgroundRepeat) {
        propId1 = CSSPropertyBackgroundRepeatX;
        propId2 = CSSPropertyBackgroundRepeatY;
    } else if (propId == CSSPropertyWebkitMaskRepeat) {
        propId1 = CSSPropertyWebkitMaskRepeatX;
        propId2 = CSSPropertyWebkitMaskRepeatY;
    }

    while (true) {
        RefPtrWillBeRawPtr<CSSValue> currValue = nullptr;
        RefPtrWillBeRawPtr<CSSValue> currValue2 = nullptr;

        Units unitless = FUnknown;
        CSSParserValue* val = m_valueList->current();
        ASSERT(val);

        switch (propId) {
        case CSSPropertyBackgroundColor:
            currValue = parseBackgroundColor(val);
            if (currValue)
                m_valueList->next();
            break;
        case CSSPropertyBackgroundAttachment:
            if (val->id == CSSValueScroll || val->id == CSSValueFixed || val->id == CSSValueLocal) {
                currValue = cssValuePool().createIdentifierValue(val->id);
                m_valueList->next();
            }
            break;
        case CSSPropertyBackgroundImage:
        case CSSPropertyWebkitMaskImage:
            if (parseFillImage(m_valueList, currValue))
                m_valueList->next();
            break;
        case CSSPropertyWebkitBackgroundClip:
        case CSSPropertyWebkitBackgroundOrigin:
        case CSSPropertyWebkitMaskClip:
        case CSSPropertyWebkitMaskOrigin:
            // The first three values here are deprecated and do not apply to the version of the property that has
            // the -webkit- prefix removed.
            if (val->id == CSSValueBorder || val->id == CSSValuePadding || val->id == CSSValueContent
                || val->id == CSSValueBorderBox || val->id == CSSValuePaddingBox || val->id == CSSValueContentBox
                || ((propId == CSSPropertyWebkitBackgroundClip || propId == CSSPropertyWebkitMaskClip)
                    && (val->id == CSSValueText || val->id == CSSValueWebkitText))) {
                currValue = cssValuePool().createIdentifierValue(val->id);
                m_valueList->next();
            }
            break;
        case CSSPropertyBackgroundClip:
            if (parseBackgroundClip(val, currValue))
                m_valueList->next();
            break;
        case CSSPropertyBackgroundOrigin:
            if (val->id == CSSValueBorderBox || val->id == CSSValuePaddingBox || val->id == CSSValueContentBox) {
                currValue = cssValuePool().createIdentifierValue(val->id);
                m_valueList->next();
            }
            break;
        case CSSPropertyBackgroundPosition:
            if (!inShorthand())
                unitless = FUnitlessQuirk;
            // fall-through
        case CSSPropertyWebkitMaskPosition:
            parseFillPosition(m_valueList, currValue, currValue2, unitless);
            // parseFillPosition advances the m_valueList pointer.
            break;
        case CSSPropertyBackgroundPositionX:
        case CSSPropertyWebkitMaskPositionX: {
            currValue = parseFillPositionX(m_valueList);
            if (currValue)
                m_valueList->next();
            break;
        }
        case CSSPropertyBackgroundPositionY:
        case CSSPropertyWebkitMaskPositionY: {
            currValue = parseFillPositionY(m_valueList);
            if (currValue)
                m_valueList->next();
            break;
        }
        //case CSSPropertyWebkitBackgroundComposite:
        case CSSPropertyWebkitMaskComposite:
            if (val->id >= CSSValueClear && val->id <= CSSValuePlusLighter) {
                currValue = cssValuePool().createIdentifierValue(val->id);
                m_valueList->next();
            }
            break;
        case CSSPropertyBackgroundBlendMode:
            if (val->id == CSSValueNormal || val->id == CSSValueMultiply
                || val->id == CSSValueScreen || val->id == CSSValueOverlay || val->id == CSSValueDarken
                || val->id == CSSValueLighten ||  val->id == CSSValueColorDodge || val->id == CSSValueColorBurn
                || val->id == CSSValueHardLight || val->id == CSSValueSoftLight || val->id == CSSValueDifference
                || val->id == CSSValueExclusion || val->id == CSSValueHue || val->id == CSSValueSaturation
                || val->id == CSSValueColor || val->id == CSSValueLuminosity) {
                currValue = cssValuePool().createIdentifierValue(val->id);
                m_valueList->next();
            }
            break;
        case CSSPropertyBackgroundRepeat:
        case CSSPropertyWebkitMaskRepeat:
            parseFillRepeat(currValue, currValue2);
            // parseFillRepeat advances the m_valueList pointer
            break;
        case CSSPropertyBackgroundSize:
        case CSSPropertyAliasWebkitBackgroundSize:
        case CSSPropertyWebkitMaskSize: {
            currValue = parseFillSize(propId);
            break;
        }
        case CSSPropertyMaskSourceType: {
            ASSERT(RuntimeEnabledFeatures::cssMaskSourceTypeEnabled());
            if (val->id == CSSValueAuto || val->id == CSSValueAlpha || val->id == CSSValueLuminance) {
                currValue = cssValuePool().createIdentifierValue(val->id);
                m_valueList->next();
            } else {
                currValue = nullptr;
            }
            break;
        }
        default:
            break;
        }
        if (!currValue)
            return false;

        if (value && !values) {
            values = CSSValueList::createCommaSeparated();
            values->append(value.release());
        }

        if (value2 && !values2) {
            values2 = CSSValueList::createCommaSeparated();
            values2->append(value2.release());
        }

        if (values)
            values->append(currValue.release());
        else
            value = currValue.release();
        if (currValue2) {
            if (values2)
                values2->append(currValue2.release());
            else
                value2 = currValue2.release();
        }

        // When parsing any fill shorthand property, we let it handle building up the lists for all
        // properties.
        if (inShorthand())
            break;

        if (!m_valueList->current())
            break;
        if (!consumeComma(m_valueList) || !m_valueList->current())
            return false;
    }

    if (values) {
        ASSERT(values->length());
        retValue1 = values.release();
        if (values2) {
            ASSERT(values2->length());
            retValue2 = values2.release();
        }
    } else {
        ASSERT(value);
        retValue1 = value.release();
        retValue2 = value2.release();
    }

    return true;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseAnimationDelay()
{
    CSSParserValue* value = m_valueList->current();
    if (validUnit(value, FTime))
        return createPrimitiveNumericValue(value);
    return nullptr;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseAnimationDirection()
{
    CSSParserValue* value = m_valueList->current();
    if (value->id == CSSValueNormal || value->id == CSSValueAlternate || value->id == CSSValueReverse || value->id == CSSValueAlternateReverse)
        return cssValuePool().createIdentifierValue(value->id);
    return nullptr;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseAnimationDuration()
{
    CSSParserValue* value = m_valueList->current();
    if (validUnit(value, FTime | FNonNeg))
        return createPrimitiveNumericValue(value);
    return nullptr;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseAnimationFillMode()
{
    CSSParserValue* value = m_valueList->current();
    if (value->id == CSSValueNone || value->id == CSSValueForwards || value->id == CSSValueBackwards || value->id == CSSValueBoth)
        return cssValuePool().createIdentifierValue(value->id);
    return nullptr;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseAnimationIterationCount()
{
    CSSParserValue* value = m_valueList->current();
    if (value->id == CSSValueInfinite)
        return cssValuePool().createIdentifierValue(value->id);
    if (validUnit(value, FNumber | FNonNeg))
        return createPrimitiveNumericValue(value);
    return nullptr;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseAnimationName(bool allowQuotedName)
{
    CSSParserValue* value = m_valueList->current();

    if (value->id == CSSValueNone)
        return cssValuePool().createIdentifierValue(CSSValueNone);

    if (value->unit == CSSPrimitiveValue::CSS_IDENT)
        return createPrimitiveCustomIdentValue(value);

    if (allowQuotedName && value->unit == CSSPrimitiveValue::CSS_STRING) {
        // Legacy support for strings in prefixed animations
        if (m_context.useCounter())
            m_context.useCounter()->count(UseCounter::QuotedAnimationName);
        if (equalIgnoringCase(value->string, "none"))
            return cssValuePool().createIdentifierValue(CSSValueNone);
        return createPrimitiveCustomIdentValue(value);
    }

    return nullptr;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseAnimationPlayState()
{
    CSSParserValue* value = m_valueList->current();
    if (value->id == CSSValueRunning || value->id == CSSValuePaused)
        return cssValuePool().createIdentifierValue(value->id);
    return nullptr;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseAnimationProperty()
{
    CSSParserValue* value = m_valueList->current();
    if (value->unit != CSSPrimitiveValue::CSS_IDENT)
        return nullptr;
    // Since all is valid css property keyword, cssPropertyID for all
    // returns non-null value. We need to check "all" before
    // cssPropertyID check.
    if (value->id == CSSValueAll)
        return cssValuePool().createIdentifierValue(CSSValueAll);
    CSSPropertyID property = unresolvedCSSPropertyID(value->string);
    if (property) {
        ASSERT(CSSPropertyMetadata::isEnabledProperty(property));
        return cssValuePool().createIdentifierValue(property);
    }
    if (value->id == CSSValueNone)
        return cssValuePool().createIdentifierValue(CSSValueNone);
    if (value->id == CSSValueInitial || value->id == CSSValueInherit)
        return nullptr;
    return createPrimitiveCustomIdentValue(value);
}

bool CSSPropertyParser::parseCubicBezierTimingFunctionValue(CSSParserValueList*& args, double& result)
{
    CSSParserValue* v = args->current();
    if (!validUnit(v, FNumber))
        return false;
    result = v->fValue;
    v = args->next();
    if (!v)
        // The last number in the function has no comma after it, so we're done.
        return true;
    return consumeComma(args);
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseAnimationTimingFunction()
{
    CSSParserValue* value = m_valueList->current();
    if (value->id == CSSValueEase || value->id == CSSValueLinear || value->id == CSSValueEaseIn || value->id == CSSValueEaseOut
        || value->id == CSSValueEaseInOut || value->id == CSSValueStepStart || value->id == CSSValueStepEnd
        || value->id == CSSValueStepMiddle)
        return cssValuePool().createIdentifierValue(value->id);

    // We must be a function.
    if (value->unit != CSSParserValue::Function)
        return nullptr;

    CSSParserValueList* args = value->function->args.get();

    if (value->function->id == CSSValueSteps) {
        // For steps, 1 or 2 params must be specified (comma-separated)
        if (!args || (args->size() != 1 && args->size() != 3))
            return nullptr;

        // There are two values.
        int numSteps;
        StepsTimingFunction::StepAtPosition stepAtPosition = StepsTimingFunction::End;

        CSSParserValue* v = args->current();
        if (!validUnit(v, FInteger))
            return nullptr;
        numSteps = clampTo<int>(v->fValue);
        if (numSteps < 1)
            return nullptr;

        if (args->next()) {
            // There is a comma so we need to parse the second value
            if (!consumeComma(args))
                return nullptr;
            switch (args->current()->id) {
            case CSSValueMiddle:
                if (!RuntimeEnabledFeatures::webAnimationsAPIEnabled())
                    return nullptr;
                stepAtPosition = StepsTimingFunction::Middle;
                break;
            case CSSValueStart:
                stepAtPosition = StepsTimingFunction::Start;
                break;
            case CSSValueEnd:
                stepAtPosition = StepsTimingFunction::End;
                break;
            default:
                return nullptr;
            }
        }

        return CSSStepsTimingFunctionValue::create(numSteps, stepAtPosition);
    }

    if (value->function->id == CSSValueCubicBezier) {
        // For cubic bezier, 4 values must be specified.
        if (!args || args->size() != 7)
            return nullptr;

        // There are two points specified. The x values must be between 0 and 1 but the y values can exceed this range.
        double x1, y1, x2, y2;

        if (!parseCubicBezierTimingFunctionValue(args, x1))
            return nullptr;
        if (x1 < 0 || x1 > 1)
            return nullptr;
        if (!parseCubicBezierTimingFunctionValue(args, y1))
            return nullptr;
        if (!parseCubicBezierTimingFunctionValue(args, x2))
            return nullptr;
        if (x2 < 0 || x2 > 1)
            return nullptr;
        if (!parseCubicBezierTimingFunctionValue(args, y2))
            return nullptr;

        return CSSCubicBezierTimingFunctionValue::create(x1, y1, x2, y2);
    }

    return nullptr;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseAnimationProperty(CSSPropertyID propId, bool useLegacyParsing)
{
    RefPtrWillBeRawPtr<CSSValue> value = nullptr;
    switch (propId) {
    case CSSPropertyAnimationDelay:
    case CSSPropertyTransitionDelay:
        value = parseAnimationDelay();
        break;
    case CSSPropertyAnimationDirection:
        value = parseAnimationDirection();
        break;
    case CSSPropertyAnimationDuration:
    case CSSPropertyTransitionDuration:
        value = parseAnimationDuration();
        break;
    case CSSPropertyAnimationFillMode:
        value = parseAnimationFillMode();
        break;
    case CSSPropertyAnimationIterationCount:
        value = parseAnimationIterationCount();
        break;
    case CSSPropertyAnimationName:
        value = parseAnimationName(useLegacyParsing);
        break;
    case CSSPropertyAnimationPlayState:
        value = parseAnimationPlayState();
        break;
    case CSSPropertyTransitionProperty:
        value = parseAnimationProperty();
        break;
    case CSSPropertyAnimationTimingFunction:
    case CSSPropertyTransitionTimingFunction:
        value = parseAnimationTimingFunction();
        break;
    default:
        ASSERT_NOT_REACHED();
        return nullptr;
    }

    if (value)
        m_valueList->next();
    return value.release();
}

PassRefPtrWillBeRawPtr<CSSValueList> CSSPropertyParser::parseAnimationPropertyList(CSSPropertyID propId, bool useLegacyParsing)
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
    while (true) {
        RefPtrWillBeRawPtr<CSSValue> value = parseAnimationProperty(propId, useLegacyParsing);
        if (!value)
            return nullptr;
        list->append(value.release());
        if (!m_valueList->current())
            break;
        if (!consumeComma(m_valueList) || !m_valueList->current())
            return nullptr;
    }
    if (propId == CSSPropertyTransitionProperty && !isValidTransitionPropertyList(list.get()))
        return nullptr;
    ASSERT(list->length());
    return list.release();
}

static inline bool isCSSWideKeyword(const CSSParserValue& value)
{
    return value.id == CSSValueInitial || value.id == CSSValueInherit || value.id == CSSValueUnset || value.id == CSSValueDefault;
}

static inline bool isValidCustomIdentForGridPositions(const CSSParserValue& value)
{
    // FIXME: we need a more general solution for <custom-ident> in all properties.
    return value.unit == CSSPrimitiveValue::CSS_IDENT && value.id != CSSValueSpan && value.id != CSSValueAuto && !isCSSWideKeyword(value);
}

// The function parses [ <integer> || <custom-ident> ] in <grid-line> (which can be stand alone or with 'span').
bool CSSPropertyParser::parseIntegerOrCustomIdentFromGridPosition(RefPtrWillBeRawPtr<CSSPrimitiveValue>& numericValue, RefPtrWillBeRawPtr<CSSPrimitiveValue>& gridLineName)
{
    CSSParserValue* value = m_valueList->current();
    if (validUnit(value, FInteger) && value->fValue) {
        numericValue = createPrimitiveNumericValue(value);
        value = m_valueList->next();
        if (value && isValidCustomIdentForGridPositions(*value)) {
            gridLineName = createPrimitiveCustomIdentValue(m_valueList->current());
            m_valueList->next();
        }
        return true;
    }

    if (isValidCustomIdentForGridPositions(*value)) {
        gridLineName = createPrimitiveCustomIdentValue(m_valueList->current());
        value = m_valueList->next();
        if (value && validUnit(value, FInteger) && value->fValue) {
            numericValue = createPrimitiveNumericValue(value);
            m_valueList->next();
        }
        return true;
    }

    return false;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseGridPosition()
{
    ASSERT(RuntimeEnabledFeatures::cssGridLayoutEnabled());

    CSSParserValue* value = m_valueList->current();
    if (value->id == CSSValueAuto) {
        m_valueList->next();
        return cssValuePool().createIdentifierValue(CSSValueAuto);
    }

    RefPtrWillBeRawPtr<CSSPrimitiveValue> numericValue = nullptr;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> gridLineName = nullptr;
    bool hasSeenSpanKeyword = false;

    if (parseIntegerOrCustomIdentFromGridPosition(numericValue, gridLineName)) {
        value = m_valueList->current();
        if (value && value->id == CSSValueSpan) {
            hasSeenSpanKeyword = true;
            m_valueList->next();
        }
    } else if (value->id == CSSValueSpan) {
        hasSeenSpanKeyword = true;
        if (CSSParserValue* nextValue = m_valueList->next()) {
            if (!isForwardSlashOperator(nextValue) && !parseIntegerOrCustomIdentFromGridPosition(numericValue, gridLineName))
                return nullptr;
        }
    }

    // Check that we have consumed all the value list. For shorthands, the parser will pass
    // the whole value list (including the opposite position).
    if (m_valueList->current() && !isForwardSlashOperator(m_valueList->current()))
        return nullptr;

    // If we didn't parse anything, this is not a valid grid position.
    if (!hasSeenSpanKeyword && !gridLineName && !numericValue)
        return nullptr;

    // Negative numbers are not allowed for span (but are for <integer>).
    if (hasSeenSpanKeyword && numericValue && numericValue->getIntValue() < 0)
        return nullptr;

    // For the <custom-ident> case.
    if (gridLineName && !numericValue && !hasSeenSpanKeyword)
        return cssValuePool().createValue(gridLineName->getStringValue(), CSSPrimitiveValue::CSS_CUSTOM_IDENT);

    RefPtrWillBeRawPtr<CSSValueList> values = CSSValueList::createSpaceSeparated();
    if (hasSeenSpanKeyword)
        values->append(cssValuePool().createIdentifierValue(CSSValueSpan));
    if (numericValue)
        values->append(numericValue.release());
    if (gridLineName)
        values->append(gridLineName.release());
    ASSERT(values->length());
    return values.release();
}

static PassRefPtrWillBeRawPtr<CSSValue> gridMissingGridPositionValue(CSSValue* value)
{
    if (value->isPrimitiveValue() && toCSSPrimitiveValue(value)->isCustomIdent())
        return value;

    return cssValuePool().createIdentifierValue(CSSValueAuto);
}

bool CSSPropertyParser::parseGridItemPositionShorthand(CSSPropertyID shorthandId, bool important)
{
    ShorthandScope scope(this, shorthandId);
    const StylePropertyShorthand& shorthand = shorthandForProperty(shorthandId);
    ASSERT(shorthand.length() == 2);

    RefPtrWillBeRawPtr<CSSValue> startValue = parseGridPosition();
    if (!startValue)
        return false;

    RefPtrWillBeRawPtr<CSSValue> endValue = nullptr;
    if (m_valueList->current()) {
        if (!isForwardSlashOperator(m_valueList->current()))
            return false;

        if (!m_valueList->next())
            return false;

        endValue = parseGridPosition();
        if (!endValue || m_valueList->current())
            return false;
    } else {
        endValue = gridMissingGridPositionValue(startValue.get());
    }

    addProperty(shorthand.properties()[0], startValue, important);
    addProperty(shorthand.properties()[1], endValue, important);
    return true;
}

bool CSSPropertyParser::parseGridTemplateRowsAndAreas(PassRefPtrWillBeRawPtr<CSSValue> templateColumns, bool important)
{
    NamedGridAreaMap gridAreaMap;
    size_t rowCount = 0;
    size_t columnCount = 0;
    bool trailingIdentWasAdded = false;
    RefPtrWillBeRawPtr<CSSValueList> templateRows = CSSValueList::createSpaceSeparated();

    // At least template-areas strings must be defined.
    if (!m_valueList->current())
        return false;

    while (m_valueList->current()) {
        // Handle leading <custom-ident>*.
        if (!parseGridLineNames(*m_valueList, *templateRows, trailingIdentWasAdded ? toCSSGridLineNamesValue(templateRows->item(templateRows->length() - 1)) : nullptr))
            return false;

        // Handle a template-area's row.
        if (!parseGridTemplateAreasRow(gridAreaMap, rowCount, columnCount))
            return false;
        ++rowCount;

        // Handle template-rows's track-size.
        if (m_valueList->current() && m_valueList->current()->unit != CSSPrimitiveValue::CSS_STRING) {
            RefPtrWillBeRawPtr<CSSValue> value = parseGridTrackSize(*m_valueList);
            if (!value)
                return false;
            templateRows->append(value);
        } else {
            templateRows->append(cssValuePool().createIdentifierValue(CSSValueAuto));
        }

        // This will handle the trailing/leading <custom-ident>* in the grammar.
        if (!parseGridLineNames(*m_valueList, *templateRows))
            return false;
        trailingIdentWasAdded = templateRows->item(templateRows->length() - 1)->isGridLineNamesValue();
    }

    // [<track-list> /]?
    if (templateColumns)
        addProperty(CSSPropertyGridTemplateColumns, templateColumns, important);
    else
        addProperty(CSSPropertyGridTemplateColumns,  cssValuePool().createIdentifierValue(CSSValueNone), important);

    // [<line-names>? <string> [<track-size> <line-names>]? ]+
    RefPtrWillBeRawPtr<CSSValue> templateAreas = CSSGridTemplateAreasValue::create(gridAreaMap, rowCount, columnCount);
    addProperty(CSSPropertyGridTemplateAreas, templateAreas.release(), important);
    addProperty(CSSPropertyGridTemplateRows, templateRows.release(), important);

    return true;
}


bool CSSPropertyParser::parseGridTemplateShorthand(bool important)
{
    ASSERT(RuntimeEnabledFeatures::cssGridLayoutEnabled());

    ShorthandScope scope(this, CSSPropertyGridTemplate);
    ASSERT(gridTemplateShorthand().length() == 3);

    // At least "none" must be defined.
    if (!m_valueList->current())
        return false;

    bool firstValueIsNone = m_valueList->current()->id == CSSValueNone;

    // 1- 'none' case.
    if (firstValueIsNone && !m_valueList->next()) {
        addProperty(CSSPropertyGridTemplateColumns, cssValuePool().createIdentifierValue(CSSValueNone), important);
        addProperty(CSSPropertyGridTemplateRows, cssValuePool().createIdentifierValue(CSSValueNone), important);
        addProperty(CSSPropertyGridTemplateAreas, cssValuePool().createIdentifierValue(CSSValueNone), important);
        return true;
    }

    unsigned index = 0;
    RefPtrWillBeRawPtr<CSSValue> columnsValue = nullptr;
    if (firstValueIsNone) {
        columnsValue = cssValuePool().createIdentifierValue(CSSValueNone);
    } else {
        columnsValue = parseGridTrackList();
    }

    // 2- <grid-template-columns> / <grid-template-columns> syntax.
    if (columnsValue) {
        if (!(m_valueList->current() && isForwardSlashOperator(m_valueList->current()) && m_valueList->next()))
            return false;
        index = m_valueList->currentIndex();
        if (RefPtrWillBeRawPtr<CSSValue> rowsValue = parseGridTrackList()) {
            if (m_valueList->current())
                return false;
            addProperty(CSSPropertyGridTemplateColumns, columnsValue, important);
            addProperty(CSSPropertyGridTemplateRows, rowsValue, important);
            addProperty(CSSPropertyGridTemplateAreas, cssValuePool().createIdentifierValue(CSSValueNone), important);
            return true;
        }
    }


    // 3- [<track-list> /]? [<line-names>? <string> [<track-size> <line-names>]? ]+ syntax.
    // The template-columns <track-list> can't be 'none'.
    if (firstValueIsNone)
        return false;
    // It requires to rewind parsing due to previous syntax failures.
    m_valueList->setCurrentIndex(index);
    return parseGridTemplateRowsAndAreas(columnsValue, important);
}

bool CSSPropertyParser::parseGridShorthand(bool important)
{
    ShorthandScope scope(this, CSSPropertyGrid);
    ASSERT(shorthandForProperty(CSSPropertyGrid).length() == 6);

    // 1- <grid-template>
    if (parseGridTemplateShorthand(important)) {
        // It can only be specified the explicit or the implicit grid properties in a single grid declaration.
        // The sub-properties not specified are set to their initial value, as normal for shorthands.
        addProperty(CSSPropertyGridAutoFlow, cssValuePool().createImplicitInitialValue(), important);
        addProperty(CSSPropertyGridAutoColumns, cssValuePool().createImplicitInitialValue(), important);
        addProperty(CSSPropertyGridAutoRows, cssValuePool().createImplicitInitialValue(), important);
        return true;
    }

    // Need to rewind parsing to explore the alternative syntax of this shorthand.
    m_valueList->setCurrentIndex(0);

    // 2- <grid-auto-flow> [ <grid-auto-columns> [ / <grid-auto-rows> ]? ]
    if (!parseValue(CSSPropertyGridAutoFlow, important))
        return false;

    RefPtrWillBeRawPtr<CSSValue> autoColumnsValue = nullptr;
    RefPtrWillBeRawPtr<CSSValue> autoRowsValue = nullptr;

    if (m_valueList->current()) {
        autoColumnsValue = parseGridTrackSize(*m_valueList);
        if (!autoColumnsValue)
            return false;
        if (m_valueList->current()) {
            if (!isForwardSlashOperator(m_valueList->current()) || !m_valueList->next())
                return false;
            autoRowsValue = parseGridTrackSize(*m_valueList);
            if (!autoRowsValue)
                return false;
        }
        if (m_valueList->current())
            return false;
    } else {
        // Other omitted values are set to their initial values.
        autoColumnsValue = cssValuePool().createImplicitInitialValue();
        autoRowsValue = cssValuePool().createImplicitInitialValue();
    }

    // if <grid-auto-rows> value is omitted, it is set to the value specified for grid-auto-columns.
    if (!autoRowsValue)
        autoRowsValue = autoColumnsValue;

    addProperty(CSSPropertyGridAutoColumns, autoColumnsValue, important);
    addProperty(CSSPropertyGridAutoRows, autoRowsValue, important);

    // It can only be specified the explicit or the implicit grid properties in a single grid declaration.
    // The sub-properties not specified are set to their initial value, as normal for shorthands.
    addProperty(CSSPropertyGridTemplateColumns, cssValuePool().createImplicitInitialValue(), important);
    addProperty(CSSPropertyGridTemplateRows, cssValuePool().createImplicitInitialValue(), important);
    addProperty(CSSPropertyGridTemplateAreas, cssValuePool().createImplicitInitialValue(), important);

    return true;
}

bool CSSPropertyParser::parseGridAreaShorthand(bool important)
{
    ASSERT(RuntimeEnabledFeatures::cssGridLayoutEnabled());

    ShorthandScope scope(this, CSSPropertyGridArea);
    const StylePropertyShorthand& shorthand = gridAreaShorthand();
    ASSERT_UNUSED(shorthand, shorthand.length() == 4);

    RefPtrWillBeRawPtr<CSSValue> rowStartValue = parseGridPosition();
    if (!rowStartValue)
        return false;

    RefPtrWillBeRawPtr<CSSValue> columnStartValue = nullptr;
    if (!parseSingleGridAreaLonghand(columnStartValue))
        return false;

    RefPtrWillBeRawPtr<CSSValue> rowEndValue = nullptr;
    if (!parseSingleGridAreaLonghand(rowEndValue))
        return false;

    RefPtrWillBeRawPtr<CSSValue> columnEndValue = nullptr;
    if (!parseSingleGridAreaLonghand(columnEndValue))
        return false;

    if (!columnStartValue)
        columnStartValue = gridMissingGridPositionValue(rowStartValue.get());

    if (!rowEndValue)
        rowEndValue = gridMissingGridPositionValue(rowStartValue.get());

    if (!columnEndValue)
        columnEndValue = gridMissingGridPositionValue(columnStartValue.get());

    addProperty(CSSPropertyGridRowStart, rowStartValue, important);
    addProperty(CSSPropertyGridColumnStart, columnStartValue, important);
    addProperty(CSSPropertyGridRowEnd, rowEndValue, important);
    addProperty(CSSPropertyGridColumnEnd, columnEndValue, important);
    return true;
}

bool CSSPropertyParser::parseSingleGridAreaLonghand(RefPtrWillBeRawPtr<CSSValue>& property)
{
    if (!m_valueList->current())
        return true;

    if (!isForwardSlashOperator(m_valueList->current()))
        return false;

    if (!m_valueList->next())
        return false;

    property = parseGridPosition();
    return true;
}

static inline bool isClosingBracket(const CSSParserValue& value)
{
    return value.unit == CSSParserValue::Operator && value.iValue == ']';
}

bool CSSPropertyParser::parseGridLineNames(CSSParserValueList& inputList, CSSValueList& valueList, CSSGridLineNamesValue* previousNamedAreaTrailingLineNames)
{
    if (!inputList.current() || inputList.current()->unit != CSSParserValue::Operator || inputList.current()->iValue != '[')
        return true;

    // Skip '['
    inputList.next();

    RefPtrWillBeRawPtr<CSSGridLineNamesValue> lineNames = previousNamedAreaTrailingLineNames;
    if (!lineNames)
        lineNames = CSSGridLineNamesValue::create();

    while (CSSParserValue* identValue = inputList.current()) {
        if (isClosingBracket(*identValue))
            break;

        if (!isValidCustomIdentForGridPositions(*identValue))
            return false;

        RefPtrWillBeRawPtr<CSSPrimitiveValue> lineName = createPrimitiveCustomIdentValue(identValue);
        lineNames->append(lineName.release());
        inputList.next();
    }

    if (!inputList.current() || !isClosingBracket(*inputList.current()))
        return false;

    if (!previousNamedAreaTrailingLineNames)
        valueList.append(lineNames.release());

    // Consume ']'
    inputList.next();
    return true;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseGridTrackList()
{
    ASSERT(RuntimeEnabledFeatures::cssGridLayoutEnabled());

    CSSParserValue* value = m_valueList->current();
    if (value->id == CSSValueNone) {
        m_valueList->next();
        return cssValuePool().createIdentifierValue(CSSValueNone);
    }

    RefPtrWillBeRawPtr<CSSValueList> values = CSSValueList::createSpaceSeparated();
    // Handle leading  <custom-ident>*.
    if (!parseGridLineNames(*m_valueList, *values))
        return nullptr;

    bool seenTrackSizeOrRepeatFunction = false;
    while (CSSParserValue* currentValue = m_valueList->current()) {
        if (isForwardSlashOperator(currentValue))
            break;
        if (currentValue->unit == CSSParserValue::Function && currentValue->function->id == CSSValueRepeat) {
            if (!parseGridTrackRepeatFunction(*values))
                return nullptr;
            seenTrackSizeOrRepeatFunction = true;
        } else {
            RefPtrWillBeRawPtr<CSSValue> value = parseGridTrackSize(*m_valueList);
            if (!value)
                return nullptr;
            values->append(value);
            seenTrackSizeOrRepeatFunction = true;
        }
        // This will handle the trailing <custom-ident>* in the grammar.
        if (!parseGridLineNames(*m_valueList, *values))
            return nullptr;
    }

    // We should have found a <track-size> or else it is not a valid <track-list>
    if (!seenTrackSizeOrRepeatFunction)
        return nullptr;

    return values;
}

bool CSSPropertyParser::parseGridTrackRepeatFunction(CSSValueList& list)
{
    CSSParserValueList* arguments = m_valueList->current()->function->args.get();
    if (!arguments || arguments->size() < 3 || !validUnit(arguments->valueAt(0), FPositiveInteger) || !isComma(arguments->valueAt(1)))
        return false;

    ASSERT(arguments->valueAt(0)->fValue > 0);
    size_t repetitions = clampTo<size_t>(arguments->valueAt(0)->fValue, 0, kGridMaxTracks);

    RefPtrWillBeRawPtr<CSSValueList> repeatedValues = CSSValueList::createSpaceSeparated();
    arguments->next(); // Skip the repetition count.
    arguments->next(); // Skip the comma.

    // Handle leading <custom-ident>*.
    if (!parseGridLineNames(*arguments, *repeatedValues))
        return false;

    size_t numberOfTracks = 0;
    while (arguments->current()) {
        RefPtrWillBeRawPtr<CSSValue> trackSize = parseGridTrackSize(*arguments);
        if (!trackSize)
            return false;

        repeatedValues->append(trackSize);
        ++numberOfTracks;

        // This takes care of any trailing <custom-ident>* in the grammar.
        if (!parseGridLineNames(*arguments, *repeatedValues))
            return false;
    }

    // We should have found at least one <track-size> or else it is not a valid <track-list>.
    if (!numberOfTracks)
        return false;

    // We clamp the number of repetitions to a multiple of the repeat() track list's size, while staying below the max
    // grid size.
    repetitions = std::min(repetitions, kGridMaxTracks / numberOfTracks);

    for (size_t i = 0; i < repetitions; ++i) {
        for (size_t j = 0; j < repeatedValues->length(); ++j)
            list.append(repeatedValues->item(j));
    }

    // parseGridTrackSize iterated over the repeat arguments, move to the next value.
    m_valueList->next();
    return true;
}


PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseGridTrackSize(CSSParserValueList& inputList)
{
    ASSERT(RuntimeEnabledFeatures::cssGridLayoutEnabled());

    CSSParserValue* currentValue = inputList.current();
    inputList.next();

    if (currentValue->id == CSSValueAuto)
        return cssValuePool().createIdentifierValue(CSSValueAuto);

    if (currentValue->unit == CSSParserValue::Function && currentValue->function->id == CSSValueMinmax) {
        // The spec defines the following grammar: minmax( <track-breadth> , <track-breadth> )
        CSSParserValueList* arguments = currentValue->function->args.get();
        if (!arguments || arguments->size() != 3 || !isComma(arguments->valueAt(1)))
            return nullptr;

        RefPtrWillBeRawPtr<CSSPrimitiveValue> minTrackBreadth = parseGridBreadth(arguments->valueAt(0));
        if (!minTrackBreadth)
            return nullptr;

        RefPtrWillBeRawPtr<CSSPrimitiveValue> maxTrackBreadth = parseGridBreadth(arguments->valueAt(2));
        if (!maxTrackBreadth)
            return nullptr;

        RefPtrWillBeRawPtr<CSSFunctionValue> result = CSSFunctionValue::create(CSSValueMinmax);
        result->append(minTrackBreadth);
        result->append(maxTrackBreadth);
        return result.release();
    }

    return parseGridBreadth(currentValue);
}

PassRefPtrWillBeRawPtr<CSSPrimitiveValue> CSSPropertyParser::parseGridBreadth(CSSParserValue* currentValue)
{
    if (currentValue->id == CSSValueMinContent || currentValue->id == CSSValueMaxContent || currentValue->id == CSSValueAuto)
        return cssValuePool().createIdentifierValue(currentValue->id);

    if (currentValue->unit == CSSPrimitiveValue::CSS_FR) {
        double flexValue = currentValue->fValue;

        // Fractional unit is a non-negative dimension.
        if (flexValue <= 0)
            return nullptr;

        return cssValuePool().createValue(flexValue, CSSPrimitiveValue::CSS_FR);
    }

    if (!validUnit(currentValue, FNonNeg | FLength | FPercent))
        return nullptr;

    return createPrimitiveNumericValue(currentValue);
}

static Vector<String> parseGridTemplateAreasColumnNames(const String& gridRowNames)
{
    ASSERT(!gridRowNames.isEmpty());
    Vector<String> columnNames;
    // Using StringImpl to avoid checks and indirection in every call to String::operator[].
    StringImpl& text = *gridRowNames.impl();

    StringBuilder areaName;
    for (unsigned i = 0; i < text.length(); ++i) {
        if (text[i] == ' ') {
            if (!areaName.isEmpty()) {
                columnNames.append(areaName.toString());
                areaName.clear();
            }
            continue;
        }
        if (text[i] == '.') {
            if (areaName == ".")
                continue;
            if (!areaName.isEmpty()) {
                columnNames.append(areaName.toString());
                areaName.clear();
            }
        } else {
            if (areaName == ".") {
                columnNames.append(areaName.toString());
                areaName.clear();
            }
        }

        areaName.append(text[i]);
    }

    if (!areaName.isEmpty())
        columnNames.append(areaName.toString());

    return columnNames;
}

bool CSSPropertyParser::parseGridTemplateAreasRow(NamedGridAreaMap& gridAreaMap, const size_t rowCount, size_t& columnCount)
{
    CSSParserValue* currentValue = m_valueList->current();
    if (!currentValue || currentValue->unit != CSSPrimitiveValue::CSS_STRING)
        return false;

    String gridRowNames = currentValue->string;
    if (gridRowNames.isEmpty() || gridRowNames.containsOnlyWhitespace())
        return false;

    Vector<String> columnNames = parseGridTemplateAreasColumnNames(gridRowNames);
    if (!columnCount) {
        columnCount = columnNames.size();
        ASSERT(columnCount);
    } else if (columnCount != columnNames.size()) {
        // The declaration is invalid is all the rows don't have the number of columns.
        return false;
    }

    for (size_t currentCol = 0; currentCol < columnCount; ++currentCol) {
        const String& gridAreaName = columnNames[currentCol];

        // Unamed areas are always valid (we consider them to be 1x1).
        if (gridAreaName == ".")
            continue;

        // We handle several grid areas with the same name at once to simplify the validation code.
        size_t lookAheadCol;
        for (lookAheadCol = currentCol; lookAheadCol < (columnCount - 1); ++lookAheadCol) {
            if (columnNames[lookAheadCol + 1] != gridAreaName)
                break;
        }

        NamedGridAreaMap::iterator gridAreaIt = gridAreaMap.find(gridAreaName);
        if (gridAreaIt == gridAreaMap.end()) {
            gridAreaMap.add(gridAreaName, GridCoordinate(GridSpan(rowCount, rowCount), GridSpan(currentCol, lookAheadCol)));
        } else {
            GridCoordinate& gridCoordinate = gridAreaIt->value;

            // The following checks test that the grid area is a single filled-in rectangle.
            // 1. The new row is adjacent to the previously parsed row.
            if (rowCount != gridCoordinate.rows.resolvedFinalPosition.next().toInt())
                return false;

            // 2. The new area starts at the same position as the previously parsed area.
            if (currentCol != gridCoordinate.columns.resolvedInitialPosition.toInt())
                return false;

            // 3. The new area ends at the same position as the previously parsed area.
            if (lookAheadCol != gridCoordinate.columns.resolvedFinalPosition.toInt())
                return false;

            ++gridCoordinate.rows.resolvedFinalPosition;
        }
        currentCol = lookAheadCol;
    }

    m_valueList->next();
    return true;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseGridTemplateAreas()
{
    if (m_valueList->current() && m_valueList->current()->id == CSSValueNone) {
        m_valueList->next();
        return cssValuePool().createIdentifierValue(CSSValueNone);
    }

    NamedGridAreaMap gridAreaMap;
    size_t rowCount = 0;
    size_t columnCount = 0;

    while (m_valueList->current()) {
        if (!parseGridTemplateAreasRow(gridAreaMap, rowCount, columnCount))
            return nullptr;
        ++rowCount;
    }

    if (!rowCount || !columnCount)
        return nullptr;

    return CSSGridTemplateAreasValue::create(gridAreaMap, rowCount, columnCount);
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseGridAutoFlow(CSSParserValueList& list)
{
    // [ row | column ] || dense
    ASSERT(RuntimeEnabledFeatures::cssGridLayoutEnabled());

    CSSParserValue* value = list.current();
    if (!value)
        return nullptr;

    RefPtrWillBeRawPtr<CSSValueList> parsedValues = CSSValueList::createSpaceSeparated();

    // First parameter.
    CSSValueID firstId = value->id;
    if (firstId != CSSValueRow && firstId != CSSValueColumn && firstId != CSSValueDense)
        return nullptr;
    parsedValues->append(cssValuePool().createIdentifierValue(firstId));

    // Second parameter, if any.
    value = list.next();
    if (value) {
        switch (firstId) {
        case CSSValueRow:
        case CSSValueColumn:
            if (value->id != CSSValueDense)
                return parsedValues;
            break;
        case CSSValueDense:
            if (value->id != CSSValueRow && value->id != CSSValueColumn)
                return parsedValues;
            break;
        default:
            return parsedValues;
        }
        parsedValues->append(cssValuePool().createIdentifierValue(value->id));
        list.next();
    }

    return parsedValues;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseCounterContent(CSSParserValueList* args, bool counters)
{
    unsigned numArgs = args->size();
    if (counters && numArgs != 3 && numArgs != 5)
        return nullptr;
    if (!counters && numArgs != 1 && numArgs != 3)
        return nullptr;

    CSSParserValue* i = args->current();
    if (i->unit != CSSPrimitiveValue::CSS_IDENT)
        return nullptr;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> identifier = createPrimitiveCustomIdentValue(i);

    RefPtrWillBeRawPtr<CSSPrimitiveValue> separator = nullptr;
    if (!counters)
        separator = cssValuePool().createValue(String(), CSSPrimitiveValue::CSS_CUSTOM_IDENT);
    else {
        args->next();
        if (!consumeComma(args))
            return nullptr;

        i = args->current();
        if (i->unit != CSSPrimitiveValue::CSS_STRING)
            return nullptr;

        separator = createPrimitiveCustomIdentValue(i);
    }

    RefPtrWillBeRawPtr<CSSPrimitiveValue> listStyle = nullptr;
    i = args->next();
    if (!i) // Make the list style default decimal
        listStyle = cssValuePool().createIdentifierValue(CSSValueDecimal);
    else {
        if (!consumeComma(args))
            return nullptr;

        i = args->current();
        if (i->unit != CSSPrimitiveValue::CSS_IDENT)
            return nullptr;

        CSSValueID listStyleID = CSSValueInvalid;
        if (i->id == CSSValueNone || (i->id >= CSSValueDisc && i->id <= CSSValueKatakanaIroha))
            listStyleID = i->id;
        else
            return nullptr;

        listStyle = cssValuePool().createIdentifierValue(listStyleID);
    }

    return cssValuePool().createValue(Counter::create(identifier.release(), listStyle.release(), separator.release()));
}

PassRefPtrWillBeRawPtr<CSSPrimitiveValue> CSSPropertyParser::parseClipShape()
{
    CSSParserValue* value = m_valueList->current();
    CSSParserValueList* args = value->function->args.get();

    if (value->function->id != CSSValueRect || !args)
        return nullptr;

    // rect(t, r, b, l) || rect(t r b l)
    if (args->size() != 4 && args->size() != 7)
        return nullptr;
    RefPtrWillBeRawPtr<Rect> rect = Rect::create();
    int i = 0;
    CSSParserValue* a = args->current();
    while (a) {
        if (a->id != CSSValueAuto && !validUnit(a, FLength | FUnitlessQuirk))
            return nullptr;
        RefPtrWillBeRawPtr<CSSPrimitiveValue> length = a->id == CSSValueAuto ?
            cssValuePool().createIdentifierValue(CSSValueAuto) :
            createPrimitiveNumericValue(a);
        if (i == 0)
            rect->setTop(length);
        else if (i == 1)
            rect->setRight(length);
        else if (i == 2)
            rect->setBottom(length);
        else
            rect->setLeft(length);
        a = args->next();
        if (a && args->size() == 7) {
            if (!consumeComma(args))
                return nullptr;
            a = args->current();
        }
        i++;
    }
    m_valueList->next();
    return cssValuePool().createValue(rect.release());
}

static void completeBorderRadii(RefPtrWillBeRawPtr<CSSPrimitiveValue> radii[4])
{
    if (radii[3])
        return;
    if (!radii[2]) {
        if (!radii[1])
            radii[1] = radii[0];
        radii[2] = radii[0];
    }
    radii[3] = radii[1];
}

// FIXME: This should be refactored with parseBorderRadius.
// parseBorderRadius contains support for some legacy radius construction.
PassRefPtrWillBeRawPtr<CSSBasicShape> CSSPropertyParser::parseInsetRoundedCorners(PassRefPtrWillBeRawPtr<CSSBasicShapeInset> shape, CSSParserValueList* args)
{
    CSSParserValue* argument = args->next();

    if (!argument)
        return nullptr;

    Vector<CSSParserValue*> radiusArguments;
    while (argument) {
        radiusArguments.append(argument);
        argument = args->next();
    }

    unsigned num = radiusArguments.size();
    if (!num || num > 9)
        return nullptr;

    // FIXME: Refactor completeBorderRadii and the array
    RefPtrWillBeRawPtr<CSSPrimitiveValue> radii[2][4];
#if ENABLE(OILPAN)
    // Zero initialize the array of raw pointers.
    memset(&radii, 0, sizeof(radii));
#endif

    unsigned indexAfterSlash = 0;
    for (unsigned i = 0; i < num; ++i) {
        CSSParserValue* value = radiusArguments.at(i);
        if (value->unit == CSSParserValue::Operator) {
            if (value->iValue != '/')
                return nullptr;

            if (!i || indexAfterSlash || i + 1 == num)
                return nullptr;

            indexAfterSlash = i + 1;
            completeBorderRadii(radii[0]);
            continue;
        }

        if (i - indexAfterSlash >= 4)
            return nullptr;

        if (!validUnit(value, FLength | FPercent | FNonNeg))
            return nullptr;

        RefPtrWillBeRawPtr<CSSPrimitiveValue> radius = createPrimitiveNumericValue(value);

        if (!indexAfterSlash)
            radii[0][i] = radius;
        else
            radii[1][i - indexAfterSlash] = radius.release();
    }

    if (!indexAfterSlash) {
        completeBorderRadii(radii[0]);
        for (unsigned i = 0; i < 4; ++i)
            radii[1][i] = radii[0][i];
    } else {
        completeBorderRadii(radii[1]);
    }
    shape->setTopLeftRadius(createPrimitiveValuePair(radii[0][0].release(), radii[1][0].release()));
    shape->setTopRightRadius(createPrimitiveValuePair(radii[0][1].release(), radii[1][1].release()));
    shape->setBottomRightRadius(createPrimitiveValuePair(radii[0][2].release(), radii[1][2].release()));
    shape->setBottomLeftRadius(createPrimitiveValuePair(radii[0][3].release(), radii[1][3].release()));

    return shape;
}

PassRefPtrWillBeRawPtr<CSSBasicShape> CSSPropertyParser::parseBasicShapeInset(CSSParserValueList* args)
{
    ASSERT(args);

    RefPtrWillBeRawPtr<CSSBasicShapeInset> shape = CSSBasicShapeInset::create();

    CSSParserValue* argument = args->current();
    WillBeHeapVector<RefPtrWillBeMember<CSSPrimitiveValue>> widthArguments;
    bool hasRoundedInset = false;

    while (argument) {
        if (argument->unit == CSSPrimitiveValue::CSS_IDENT && argument->id == CSSValueRound) {
            hasRoundedInset = true;
            break;
        }

        Units unitFlags = FLength | FPercent;
        if (!validUnit(argument, unitFlags) || widthArguments.size() > 4)
            return nullptr;

        widthArguments.append(createPrimitiveNumericValue(argument));
        argument = args->next();
    }

    switch (widthArguments.size()) {
    case 1: {
        shape->updateShapeSize1Value(widthArguments[0].get());
        break;
    }
    case 2: {
        shape->updateShapeSize2Values(widthArguments[0].get(), widthArguments[1].get());
        break;
        }
    case 3: {
        shape->updateShapeSize3Values(widthArguments[0].get(), widthArguments[1].get(), widthArguments[2].get());
        break;
    }
    case 4: {
        shape->updateShapeSize4Values(widthArguments[0].get(), widthArguments[1].get(), widthArguments[2].get(), widthArguments[3].get());
        break;
    }
    default:
        return nullptr;
    }

    if (hasRoundedInset)
        return parseInsetRoundedCorners(shape, args);
    return shape;
}

static bool isContentDistributionKeyword(CSSValueID id)
{
    return id == CSSValueSpaceBetween || id == CSSValueSpaceAround
        || id == CSSValueSpaceEvenly || id == CSSValueStretch;
}

static bool isContentPositionKeyword(CSSValueID id)
{
    return id == CSSValueStart || id == CSSValueEnd || id == CSSValueCenter
        || id == CSSValueFlexStart || id == CSSValueFlexEnd
        || id == CSSValueLeft || id == CSSValueRight;
}

static bool isBaselinePositionKeyword(CSSValueID id)
{
    return id == CSSValueBaseline || id == CSSValueLastBaseline;
}

static bool isAlignmentOverflowKeyword(CSSValueID id)
{
    return id == CSSValueTrue || id == CSSValueSafe;
}

static bool isItemPositionKeyword(CSSValueID id)
{
    return id == CSSValueStart || id == CSSValueEnd || id == CSSValueCenter
        || id == CSSValueSelfStart || id == CSSValueSelfEnd || id == CSSValueFlexStart
        || id == CSSValueFlexEnd || id == CSSValueLeft || id == CSSValueRight;
}

bool CSSPropertyParser::parseLegacyPosition(CSSPropertyID propId, bool important)
{
    // [ legacy && [ left | right | center ]

    CSSParserValue* value = m_valueList->current();
    ASSERT(value);

    if (value->id == CSSValueLegacy) {
        value = m_valueList->next();
        if (!value)
            return false;
        if (value->id != CSSValueCenter && value->id != CSSValueLeft && value->id != CSSValueRight)
            return false;
    } else if (value->id == CSSValueCenter || value->id == CSSValueLeft || value->id == CSSValueRight) {
        if (!m_valueList->next() || m_valueList->current()->id != CSSValueLegacy)
            return false;
    } else {
        return false;
    }

    addProperty(propId, createPrimitiveValuePair(cssValuePool().createIdentifierValue(CSSValueLegacy), cssValuePool().createIdentifierValue(value->id)), important);
    return !m_valueList->next();
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseContentDistributionOverflowPosition()
{
    // auto | <baseline-position> | <content-distribution> || [ <overflow-position>? && <content-position> ]
    // <baseline-position> = baseline | last-baseline;
    // <content-distribution> = space-between | space-around | space-evenly | stretch;
    // <content-position> = center | start | end | flex-start | flex-end | left | right;
    // <overflow-position> = true | safe

    // auto | <baseline-position>
    CSSParserValue* value = m_valueList->current();
    if (value->id == CSSValueAuto || isBaselinePositionKeyword(value->id)) {
        m_valueList->next();
        return CSSContentDistributionValue::create(CSSValueInvalid, value->id, CSSValueInvalid);
    }

    CSSValueID distribution = CSSValueInvalid;
    CSSValueID position = CSSValueInvalid;
    CSSValueID overflow = CSSValueInvalid;
    while (value) {
        if (isContentDistributionKeyword(value->id)) {
            if (distribution != CSSValueInvalid)
                return nullptr;
            distribution = value->id;
        } else if (isContentPositionKeyword(value->id)) {
            if (position != CSSValueInvalid)
                return nullptr;
            position = value->id;
        } else if (isAlignmentOverflowKeyword(value->id)) {
            if (overflow != CSSValueInvalid)
                return nullptr;
            overflow = value->id;
        } else {
            return nullptr;
        }
        value = m_valueList->next();
    }

    // The grammar states that we should have at least <content-distribution> or
    // <content-position> ( <content-distribution> || <content-position> ).
    if (position == CSSValueInvalid && distribution == CSSValueInvalid)
        return nullptr;

    // The grammar states that <overflow-position> must be associated to <content-position>.
    if (overflow != CSSValueInvalid && position == CSSValueInvalid)
        return nullptr;

    return CSSContentDistributionValue::create(distribution, position, overflow);
}

bool CSSPropertyParser::parseItemPositionOverflowPosition(CSSPropertyID propId, bool important)
{
    // auto | stretch | <baseline-position> | [<item-position> && <overflow-position>? ]
    // <baseline-position> = baseline | last-baseline;
    // <item-position> = center | start | end | self-start | self-end | flex-start | flex-end | left | right;
    // <overflow-position> = true | safe

    CSSParserValue* value = m_valueList->current();
    ASSERT(value);

    if (value->id == CSSValueAuto || value->id == CSSValueStretch || isBaselinePositionKeyword(value->id)) {
        if (m_valueList->next())
            return false;

        addProperty(propId, cssValuePool().createIdentifierValue(value->id), important);
        return true;
    }

    RefPtrWillBeRawPtr<CSSPrimitiveValue> position = nullptr;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> overflowAlignmentKeyword = nullptr;
    if (isItemPositionKeyword(value->id)) {
        position = cssValuePool().createIdentifierValue(value->id);
        value = m_valueList->next();
        if (value) {
            if (isAlignmentOverflowKeyword(value->id))
                overflowAlignmentKeyword = cssValuePool().createIdentifierValue(value->id);
            else
                return false;
        }
    } else if (isAlignmentOverflowKeyword(value->id)) {
        overflowAlignmentKeyword = cssValuePool().createIdentifierValue(value->id);
        value = m_valueList->next();
        if (value && isItemPositionKeyword(value->id))
            position = cssValuePool().createIdentifierValue(value->id);
        else
            return false;
    } else {
        return false;
    }

    if (m_valueList->next())
        return false;

    ASSERT(position);
    if (overflowAlignmentKeyword)
        addProperty(propId, createPrimitiveValuePair(position, overflowAlignmentKeyword), important);
    else
        addProperty(propId, position.release(), important);

    return true;
}

PassRefPtrWillBeRawPtr<CSSPrimitiveValue> CSSPropertyParser::parseShapeRadius(CSSParserValue* value)
{
    if (value->id == CSSValueClosestSide || value->id == CSSValueFarthestSide)
        return cssValuePool().createIdentifierValue(value->id);

    if (!validUnit(value, FLength | FPercent | FNonNeg))
        return nullptr;

    return createPrimitiveNumericValue(value);
}

PassRefPtrWillBeRawPtr<CSSBasicShape> CSSPropertyParser::parseBasicShapeCircle(CSSParserValueList* args)
{
    ASSERT(args);

    // circle(radius)
    // circle(radius at <position>)
    // circle(at <position>)
    // where position defines centerX and centerY using a CSS <position> data type.
    RefPtrWillBeRawPtr<CSSBasicShapeCircle> shape = CSSBasicShapeCircle::create();

    for (CSSParserValue* argument = args->current(); argument; argument = args->next()) {
        // The call to parseFillPosition below should consume all of the
        // arguments except the first two. Thus, and index greater than one
        // indicates an invalid production.
        if (args->currentIndex() > 1)
            return nullptr;

        if (!args->currentIndex() && argument->id != CSSValueAt) {
            if (RefPtrWillBeRawPtr<CSSPrimitiveValue> radius = parseShapeRadius(argument)) {
                shape->setRadius(radius);
                continue;
            }

            return nullptr;
        }

        if (argument->id == CSSValueAt && args->next()) {
            RefPtrWillBeRawPtr<CSSValue> centerX = nullptr;
            RefPtrWillBeRawPtr<CSSValue> centerY = nullptr;
            parseFillPosition(args, centerX, centerY);
            if (centerX && centerY && !args->current()) {
                ASSERT(centerX->isPrimitiveValue());
                ASSERT(centerY->isPrimitiveValue());
                shape->setCenterX(toCSSPrimitiveValue(centerX.get()));
                shape->setCenterY(toCSSPrimitiveValue(centerY.get()));
            } else {
                return nullptr;
            }
        } else {
            return nullptr;
        }
    }

    return shape;
}

PassRefPtrWillBeRawPtr<CSSBasicShape> CSSPropertyParser::parseBasicShapeEllipse(CSSParserValueList* args)
{
    ASSERT(args);

    // ellipse(radiusX)
    // ellipse(radiusX at <position>)
    // ellipse(radiusX radiusY)
    // ellipse(radiusX radiusY at <position>)
    // ellipse(at <position>)
    // where position defines centerX and centerY using a CSS <position> data type.
    RefPtrWillBeRawPtr<CSSBasicShapeEllipse> shape = CSSBasicShapeEllipse::create();

    for (CSSParserValue* argument = args->current(); argument; argument = args->next()) {
        // The call to parseFillPosition below should consume all of the
        // arguments except the first three. Thus, an index greater than two
        // indicates an invalid production.
        if (args->currentIndex() > 2)
            return nullptr;

        if (args->currentIndex() < 2 && argument->id != CSSValueAt) {
            if (RefPtrWillBeRawPtr<CSSPrimitiveValue> radius = parseShapeRadius(argument)) {
                if (!shape->radiusX())
                    shape->setRadiusX(radius);
                else
                    shape->setRadiusY(radius);
                continue;
            }

            return nullptr;
        }

        if (argument->id != CSSValueAt || !args->next()) // expecting ellipse(.. at <position>)
            return nullptr;
        RefPtrWillBeRawPtr<CSSValue> centerX = nullptr;
        RefPtrWillBeRawPtr<CSSValue> centerY = nullptr;
        parseFillPosition(args, centerX, centerY);
        if (!centerX || !centerY || args->current())
            return nullptr;

        ASSERT(centerX->isPrimitiveValue());
        ASSERT(centerY->isPrimitiveValue());
        shape->setCenterX(toCSSPrimitiveValue(centerX.get()));
        shape->setCenterY(toCSSPrimitiveValue(centerY.get()));
    }

    return shape;
}

PassRefPtrWillBeRawPtr<CSSBasicShape> CSSPropertyParser::parseBasicShapePolygon(CSSParserValueList* args)
{
    ASSERT(args);

    unsigned size = args->size();
    if (!size)
        return nullptr;

    RefPtrWillBeRawPtr<CSSBasicShapePolygon> shape = CSSBasicShapePolygon::create();

    CSSParserValue* argument = args->current();
    if (argument->id == CSSValueEvenodd || argument->id == CSSValueNonzero) {
        shape->setWindRule(argument->id == CSSValueEvenodd ? RULE_EVENODD : RULE_NONZERO);
        args->next();

        if (!consumeComma(args))
            return nullptr;

        size -= 2;
    }

    // <length> <length>, ... <length> <length> -> each pair has 3 elements except the last one
    if (!size || (size % 3) - 2)
        return nullptr;

    while (true) {
        CSSParserValue* argumentX = args->current();
        if (!argumentX || !validUnit(argumentX, FLength | FPercent))
            return nullptr;
        RefPtrWillBeRawPtr<CSSPrimitiveValue> xLength = createPrimitiveNumericValue(argumentX);

        CSSParserValue* argumentY = args->next();
        if (!argumentY || !validUnit(argumentY, FLength | FPercent))
            return nullptr;
        RefPtrWillBeRawPtr<CSSPrimitiveValue> yLength = createPrimitiveNumericValue(argumentY);

        shape->appendPoint(xLength.release(), yLength.release());

        if (!args->next())
            break;
        if (!consumeComma(args))
            return nullptr;
    }

    return shape;
}

static bool isBoxValue(CSSValueID valueId)
{
    switch (valueId) {
    case CSSValueContentBox:
    case CSSValuePaddingBox:
    case CSSValueBorderBox:
    case CSSValueMarginBox:
        return true;
    default:
        break;
    }

    return false;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseShapeProperty(CSSPropertyID propId)
{
    RefPtrWillBeRawPtr<CSSValue> imageValue = nullptr;
    if (parseFillImage(m_valueList, imageValue)) {
        m_valueList->next();
        return imageValue.release();
    }

    return parseBasicShapeAndOrBox();
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseBasicShapeAndOrBox()
{
    CSSParserValue* value = m_valueList->current();

    bool shapeFound = false;
    bool boxFound = false;
    CSSValueID valueId;

    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    for (unsigned i = 0; i < 2; ++i) {
        if (!value)
            break;
        valueId = value->id;
        if (value->unit == CSSParserValue::Function && !shapeFound) {
            // parseBasicShape already asks for the next value list item.
            RefPtrWillBeRawPtr<CSSPrimitiveValue> shapeValue = parseBasicShape();
            if (!shapeValue)
                return nullptr;
            list->append(shapeValue.release());
            shapeFound = true;
        } else if (isBoxValue(valueId) && !boxFound) {
            list->append(parseValidPrimitive(valueId, value));
            boxFound = true;
            m_valueList->next();
        } else {
            return nullptr;
        }

        value = m_valueList->current();
    }

    if (m_valueList->current())
        return nullptr;
    return list.release();
}

PassRefPtrWillBeRawPtr<CSSPrimitiveValue> CSSPropertyParser::parseBasicShape()
{
    CSSParserValue* value = m_valueList->current();
    ASSERT(value->unit == CSSParserValue::Function);
    CSSParserValueList* args = value->function->args.get();

    if (!args)
        return nullptr;

    RefPtrWillBeRawPtr<CSSBasicShape> shape = nullptr;
    if (value->function->id == CSSValueCircle)
        shape = parseBasicShapeCircle(args);
    else if (value->function->id == CSSValueEllipse)
        shape = parseBasicShapeEllipse(args);
    else if (value->function->id == CSSValuePolygon)
        shape = parseBasicShapePolygon(args);
    else if (value->function->id == CSSValueInset)
        shape = parseBasicShapeInset(args);

    if (!shape)
        return nullptr;

    m_valueList->next();

    return cssValuePool().createValue(shape.release());
}

// [ 'font-style' || 'font-variant' || 'font-weight' ]? 'font-size' [ / 'line-height' ]? 'font-family'
bool CSSPropertyParser::parseFont(bool important)
{
    // Let's check if there is an inherit or initial somewhere in the shorthand.
    for (unsigned i = 0; i < m_valueList->size(); ++i) {
        if (m_valueList->valueAt(i)->id == CSSValueInherit || m_valueList->valueAt(i)->id == CSSValueInitial)
            return false;
    }

    ShorthandScope scope(this, CSSPropertyFont);
    // Optional font-style, font-variant and font-weight.
    bool fontStyleParsed = false;
    bool fontVariantParsed = false;
    bool fontWeightParsed = false;
    bool fontStretchParsed = false;
    CSSParserValue* value = m_valueList->current();
    for (; value; value = m_valueList->next()) {
        if (!fontStyleParsed && CSSParserFastPaths::isValidKeywordPropertyAndValue(CSSPropertyFontStyle, value->id)) {
            addProperty(CSSPropertyFontStyle, cssValuePool().createIdentifierValue(value->id), important);
            fontStyleParsed = true;
        } else if (!fontVariantParsed && (value->id == CSSValueNormal || value->id == CSSValueSmallCaps)) {
            // Font variant in the shorthand is particular, it only accepts normal or small-caps.
            addProperty(CSSPropertyFontVariant, cssValuePool().createIdentifierValue(value->id), important);
            fontVariantParsed = true;
        } else if (!fontWeightParsed && parseFontWeight(important)) {
            fontWeightParsed = true;
        } else if (!fontStretchParsed && CSSParserFastPaths::isValidKeywordPropertyAndValue(CSSPropertyFontStretch, value->id)) {
            addProperty(CSSPropertyFontStretch, cssValuePool().createIdentifierValue(value->id), important);
            fontStretchParsed = true;
        } else {
            break;
        }
    }

    if (!value)
        return false;

    if (!fontStyleParsed)
        addProperty(CSSPropertyFontStyle, cssValuePool().createIdentifierValue(CSSValueNormal), important, true);
    if (!fontVariantParsed)
        addProperty(CSSPropertyFontVariant, cssValuePool().createIdentifierValue(CSSValueNormal), important, true);
    if (!fontWeightParsed)
        addProperty(CSSPropertyFontWeight, cssValuePool().createIdentifierValue(CSSValueNormal), important, true);
    if (!fontStretchParsed)
        addProperty(CSSPropertyFontStretch, cssValuePool().createIdentifierValue(CSSValueNormal), important, true);

    // Now a font size _must_ come.
    // <absolute-size> | <relative-size> | <length> | <percentage> | inherit
    if (!parseFontSize(important))
        return false;

    value = m_valueList->current();
    if (!value)
        return false;

    if (isForwardSlashOperator(value)) {
        // The line-height property.
        value = m_valueList->next();
        if (!value)
            return false;
        RefPtrWillBeRawPtr<CSSPrimitiveValue> lineHeight = parseLineHeight();
        if (!lineHeight)
            return false;
        addProperty(CSSPropertyLineHeight, lineHeight.release(), important);
    } else {
        addProperty(CSSPropertyLineHeight, cssValuePool().createIdentifierValue(CSSValueNormal), important, true);
    }

    // Font family must come now.
    RefPtrWillBeRawPtr<CSSValue> parsedFamilyValue = parseFontFamily();
    if (!parsedFamilyValue)
        return false;

    addProperty(CSSPropertyFontFamily, parsedFamilyValue.release(), important);

    // FIXME: http://www.w3.org/TR/2011/WD-css3-fonts-20110324/#font-prop requires that
    // "font-stretch", "font-size-adjust", and "font-kerning" be reset to their initial values
    // but we don't seem to support them at the moment. They should also be added here once implemented.
    if (m_valueList->current())
        return false;

    return true;
}

void CSSPropertyParser::parseSystemFont(bool important)
{
    ASSERT(m_valueList->size() == 1);
    CSSValueID systemFontID = m_valueList->valueAt(0)->id;
    ASSERT(systemFontID >= CSSValueCaption && systemFontID <= CSSValueStatusBar);
    m_valueList->next();

    FontStyle fontStyle = FontStyleNormal;
    FontWeight fontWeight = FontWeightNormal;
    float fontSize = 0;
    AtomicString fontFamily;
    LayoutTheme::theme().systemFont(systemFontID, fontStyle, fontWeight, fontSize, fontFamily);

    ShorthandScope scope(this, CSSPropertyFont);
    addProperty(CSSPropertyFontStyle, cssValuePool().createIdentifierValue(fontStyle == FontStyleItalic ? CSSValueItalic : CSSValueNormal), important);
    addProperty(CSSPropertyFontWeight, cssValuePool().createValue(fontWeight), important);
    addProperty(CSSPropertyFontSize, cssValuePool().createValue(fontSize, CSSPrimitiveValue::CSS_PX), important);
    RefPtrWillBeRawPtr<CSSValueList> fontFamilyList = CSSValueList::createCommaSeparated();
    fontFamilyList->append(cssValuePool().createFontFamilyValue(fontFamily));
    addProperty(CSSPropertyFontFamily, fontFamilyList.release(), important);

    addProperty(CSSPropertyFontStretch, cssValuePool().createIdentifierValue(CSSValueNormal), important);
    addProperty(CSSPropertyFontVariant, cssValuePool().createIdentifierValue(CSSValueNormal), important);
    addProperty(CSSPropertyLineHeight, cssValuePool().createIdentifierValue(CSSValueNormal), important);
}

class FontFamilyValueBuilder {
    DISALLOW_ALLOCATION();
public:
    FontFamilyValueBuilder(CSSValueList* list)
        : m_list(list)
    {
    }

    void add(const CSSParserString& string)
    {
        if (!m_builder.isEmpty())
            m_builder.append(' ');

        if (string.is8Bit()) {
            m_builder.append(string.characters8(), string.length());
            return;
        }

        m_builder.append(string.characters16(), string.length());
    }

    void commit()
    {
        if (m_builder.isEmpty())
            return;
        m_list->append(cssValuePool().createFontFamilyValue(m_builder.toString()));
        m_builder.clear();
    }

private:
    StringBuilder m_builder;
    CSSValueList* m_list;
};

PassRefPtrWillBeRawPtr<CSSValueList> CSSPropertyParser::parseFontFamily()
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();
    CSSParserValue* value = m_valueList->current();

    FontFamilyValueBuilder familyBuilder(list.get());
    bool inFamily = false;

    while (value) {
        CSSParserValue* nextValue = m_valueList->next();
        bool nextValBreaksFont = !nextValue || isComma(nextValue);
        bool nextValIsFontName = nextValue &&
            ((nextValue->id >= CSSValueSerif && nextValue->id <= CSSValueWebkitBody) ||
            (nextValue->unit == CSSPrimitiveValue::CSS_STRING || nextValue->unit == CSSPrimitiveValue::CSS_IDENT));

        if (isCSSWideKeyword(*value) && !inFamily) {
            if (nextValBreaksFont)
                return nullptr;
            else if (nextValIsFontName)
                value = nextValue;
            continue;
        }

        if (value->id >= CSSValueSerif && value->id <= CSSValueWebkitBody) {
            if (inFamily)
                familyBuilder.add(value->string);
            else if (nextValBreaksFont || !nextValIsFontName)
                list->append(cssValuePool().createIdentifierValue(value->id));
            else {
                familyBuilder.commit();
                familyBuilder.add(value->string);
                inFamily = true;
            }
        } else if (value->unit == CSSPrimitiveValue::CSS_STRING) {
            // Strings never share in a family name.
            inFamily = false;
            familyBuilder.commit();
            list->append(cssValuePool().createFontFamilyValue(value->string));
        } else if (value->unit == CSSPrimitiveValue::CSS_IDENT) {
            if (inFamily)
                familyBuilder.add(value->string);
            else if (nextValBreaksFont || !nextValIsFontName)
                list->append(cssValuePool().createFontFamilyValue(value->string));
            else {
                familyBuilder.commit();
                familyBuilder.add(value->string);
                inFamily = true;
            }
        } else {
            break;
        }

        if (!nextValue)
            break;

        if (nextValBreaksFont) {
            value = m_valueList->next();
            familyBuilder.commit();
            inFamily = false;
        }
        else if (nextValIsFontName)
            value = nextValue;
        else
            break;
    }
    familyBuilder.commit();
    if (!list->length() || (m_ruleType == StyleRule::FontFace && list->length() > 1))
        list = nullptr;
    return list.release();
}

PassRefPtrWillBeRawPtr<CSSPrimitiveValue> CSSPropertyParser::parseLineHeight()
{
    CSSParserValue* value = m_valueList->current();
    CSSValueID id = value->id;

    // normal | <number> | <length> | <percentage> | inherit
    if (id == CSSValueNormal) {
        m_valueList->next();
        return cssValuePool().createIdentifierValue(id);
    }

    if (!validUnit(value, FNumber | FLength | FPercent | FNonNeg))
        return nullptr;
    // The line-height property can accept both percents and numbers but additive opertaions are
    // not permitted on them in calc() expressions.
    if (m_parsedCalculation && m_parsedCalculation->category() == CalcPercentNumber) {
        m_parsedCalculation.release();
        return nullptr;
    }
    m_valueList->next();
    return createPrimitiveNumericValue(value);
}

bool CSSPropertyParser::parseFontSize(bool important)
{
    CSSParserValue* value = m_valueList->current();
    CSSValueID id = value->id;
    bool validPrimitive = false;
    // <absolute-size> | <relative-size> | <length> | <percentage> | inherit
    if (id >= CSSValueXxSmall && id <= CSSValueLarger)
        validPrimitive = true;
    else
        validPrimitive = validUnit(value, FLength | FPercent | FNonNeg | (inShorthand() ? FUnknown : FUnitlessQuirk));
    if (validPrimitive && (!m_valueList->next() || inShorthand()))
        addProperty(CSSPropertyFontSize, parseValidPrimitive(id, value), important);
    return validPrimitive;
}

bool CSSPropertyParser::parseFontVariant(bool important)
{
    RefPtrWillBeRawPtr<CSSValueList> values = nullptr;
    if (m_valueList->size() > 1)
        values = CSSValueList::createCommaSeparated();
    bool expectComma = false;
    for (CSSParserValue* val = m_valueList->current(); val; val = m_valueList->current()) {
        RefPtrWillBeRawPtr<CSSPrimitiveValue> parsedValue = nullptr;
        if (!expectComma) {
            expectComma = true;
            if (val->id == CSSValueNormal || val->id == CSSValueSmallCaps)
                parsedValue = cssValuePool().createIdentifierValue(val->id);
            else if (val->id == CSSValueAll && !values) {
                // FIXME: CSSPropertyParser::parseFontVariant() implements
                // the old css3 draft:
                // http://www.w3.org/TR/2002/WD-css3-webfonts-20020802/#font-variant
                // 'all' is only allowed in @font-face and with no other values. Make a value list to
                // indicate that we are in the @font-face case.
                values = CSSValueList::createCommaSeparated();
                parsedValue = cssValuePool().createIdentifierValue(val->id);
            }
        } else if (consumeComma(m_valueList)) {
            expectComma = false;
            continue;
        }

        if (!parsedValue)
            return false;

        m_valueList->next();

        if (values)
            values->append(parsedValue.release());
        else {
            addProperty(CSSPropertyFontVariant, parsedValue.release(), important);
            return true;
        }
    }

    if (values && values->length()) {
        if (m_ruleType != StyleRule::FontFace)
            return false;
        addProperty(CSSPropertyFontVariant, values.release(), important);
        return true;
    }

    return false;
}

bool CSSPropertyParser::parseFontWeight(bool important)
{
    CSSParserValue* value = m_valueList->current();
    if (value->id >= CSSValueNormal && value->id <= CSSValueLighter) {
        addProperty(CSSPropertyFontWeight, cssValuePool().createIdentifierValue(value->id), important);
        return true;
    }
    if (value->unit == CSSPrimitiveValue::CSS_NUMBER) {
        int weight = static_cast<int>(value->fValue);
        if (!(weight % 100) && weight >= 100 && weight <= 900) {
            addProperty(CSSPropertyFontWeight, cssValuePool().createIdentifierValue(static_cast<CSSValueID>(CSSValue100 + weight / 100 - 1)), important);
            return true;
        }
    }
    return false;
}

bool CSSPropertyParser::parseFontFaceSrcURI(CSSValueList* valueList)
{
    RefPtrWillBeRawPtr<CSSFontFaceSrcValue> uriValue(CSSFontFaceSrcValue::create(completeURL(m_valueList->current()->string), m_context.shouldCheckContentSecurityPolicy()));
    uriValue->setReferrer(m_context.referrer());

    CSSParserValue* value = m_valueList->next();
    if (!value || value->unit != CSSParserValue::Function || value->function->id != CSSValueFormat) {
        valueList->append(uriValue.release());
        return true;
    }

    // FIXME: http://www.w3.org/TR/2011/WD-css3-fonts-20111004/ says that format() contains a comma-separated list of strings,
    // but CSSFontFaceSrcValue stores only one format. Allowing one format for now.
    CSSParserValueList* args = value->function->args.get();
    if (!args || args->size() != 1 || (args->current()->unit != CSSPrimitiveValue::CSS_STRING && args->current()->unit != CSSPrimitiveValue::CSS_IDENT))
        return false;
    uriValue->setFormat(args->current()->string);
    valueList->append(uriValue.release());
    m_valueList->next();
    return true;
}

bool CSSPropertyParser::parseFontFaceSrcLocal(CSSValueList* valueList)
{
    CSSParserValueList* args = m_valueList->current()->function->args.get();
    if (!args || !args->size())
        return false;
    m_valueList->next();

    ContentSecurityPolicyDisposition shouldCheckContentSecurityPolicy = m_context.shouldCheckContentSecurityPolicy();
    if (args->size() == 1 && args->current()->unit == CSSPrimitiveValue::CSS_STRING)
        valueList->append(CSSFontFaceSrcValue::createLocal(args->current()->string, shouldCheckContentSecurityPolicy));
    else if (args->current()->unit == CSSPrimitiveValue::CSS_IDENT) {
        StringBuilder builder;
        for (CSSParserValue* localValue = args->current(); localValue; localValue = args->next()) {
            if (localValue->unit != CSSPrimitiveValue::CSS_IDENT)
                return false;
            if (!builder.isEmpty())
                builder.append(' ');
            builder.append(localValue->string);
        }
        valueList->append(CSSFontFaceSrcValue::createLocal(builder.toString(), shouldCheckContentSecurityPolicy));
    } else
        return false;

    return true;
}

PassRefPtrWillBeRawPtr<CSSValueList> CSSPropertyParser::parseFontFaceSrc()
{
    RefPtrWillBeRawPtr<CSSValueList> values(CSSValueList::createCommaSeparated());

    while (true) {
        CSSParserValue* value = m_valueList->current();
        if (!value)
            return nullptr;
        if (value->unit == CSSPrimitiveValue::CSS_URI) {
            if (!parseFontFaceSrcURI(values.get()))
                return nullptr;
        } else if (value->unit == CSSParserValue::Function && value->function->id == CSSValueLocal) {
            if (!parseFontFaceSrcLocal(values.get()))
                return nullptr;
        } else {
            return nullptr;
        }

        if (!m_valueList->current())
            return values.release();
        if (!consumeComma(m_valueList))
            return nullptr;
    }
}

PassRefPtrWillBeRawPtr<CSSValueList> CSSPropertyParser::parseFontFaceUnicodeRange()
{
    RefPtrWillBeRawPtr<CSSValueList> values = CSSValueList::createCommaSeparated();

    do {
        CSSParserValue* current = m_valueList->current();
        if (!current || current->unit != CSSParserValue::UnicodeRange)
            return nullptr;

        UChar32 start = current->m_unicodeRange.start;
        UChar32 end = current->m_unicodeRange.end;
        if (start > end)
            return nullptr;
        values->append(CSSUnicodeRangeValue::create(start, end));
        m_valueList->next();
    } while (consumeComma(m_valueList));

    return values.release();
}


bool CSSPropertyParser::isCalculation(CSSParserValue* value)
{
    return (value->unit == CSSParserValue::Function)
        && (value->function->id == CSSValueCalc
            || value->function->id == CSSValueWebkitCalc);
}

inline int CSSPropertyParser::colorIntFromValue(CSSParserValue* v)
{
    bool isPercent;
    double value;

    if (m_parsedCalculation) {
        isPercent = m_parsedCalculation->category() == CalcPercent;
        value = m_parsedCalculation->doubleValue();
        m_parsedCalculation.release();
    } else {
        isPercent = v->unit == CSSPrimitiveValue::CSS_PERCENTAGE;
        value = v->fValue;
    }

    if (value <= 0.0)
        return 0;

    if (isPercent) {
        if (value >= 100.0)
            return 255;
        return static_cast<int>(value * 256.0 / 100.0);
    }

    if (value >= 255.0)
        return 255;

    return static_cast<int>(value);
}

bool CSSPropertyParser::parseColorParameters(const CSSParserValue* value, int* colorArray, bool parseAlpha)
{
    CSSParserValueList* args = value->function->args.get();
    CSSParserValue* v = args->current();
    Units unitType = FUnknown;
    // Get the first value and its type
    if (validUnit(v, FInteger))
        unitType = FInteger;
    else if (validUnit(v, FPercent))
        unitType = FPercent;
    else
        return false;

    colorArray[0] = colorIntFromValue(v);
    for (int i = 1; i < 3; i++) {
        args->next();
        if (!consumeComma(args))
            return false;
        v = args->current();
        if (!validUnit(v, unitType))
            return false;
        colorArray[i] = colorIntFromValue(v);
    }
    if (parseAlpha) {
        args->next();
        if (!consumeComma(args))
            return false;
        v = args->current();
        if (!validUnit(v, FNumber))
            return false;
        // Convert the floating pointer number of alpha to an integer in the range [0, 256),
        // with an equal distribution across all 256 values.
        colorArray[3] = static_cast<int>(std::max(0.0, std::min(1.0, v->fValue)) * nextafter(256.0, 0.0));
    }
    return true;
}

// The CSS3 specification defines the format of a HSL color as
// hsl(<number>, <percent>, <percent>)
// and with alpha, the format is
// hsla(<number>, <percent>, <percent>, <number>)
// The first value, HUE, is in an angle with a value between 0 and 360
bool CSSPropertyParser::parseHSLParameters(const CSSParserValue* value, double* colorArray, bool parseAlpha)
{
    CSSParserValueList* args = value->function->args.get();
    CSSParserValue* v = args->current();
    // Get the first value
    if (!validUnit(v, FNumber))
        return false;
    // normalize the Hue value and change it to be between 0 and 1.0
    colorArray[0] = (((static_cast<int>(v->fValue) % 360) + 360) % 360) / 360.0;
    for (int i = 1; i < 3; i++) {
        args->next();
        if (!consumeComma(args))
            return false;
        v = args->current();
        if (!validUnit(v, FPercent))
            return false;
        double percentValue = m_parsedCalculation ? m_parsedCalculation.release()->doubleValue() : v->fValue;
        colorArray[i] = std::max(0.0, std::min(100.0, percentValue)) / 100.0; // needs to be value between 0 and 1.0
    }
    if (parseAlpha) {
        args->next();
        if (!consumeComma(args))
            return false;
        v = args->current();
        if (!validUnit(v, FNumber))
            return false;
        colorArray[3] = std::max(0.0, std::min(1.0, v->fValue));
    }
    return true;
}

bool CSSPropertyParser::parseColorFromValue(const CSSParserValue* value, RGBA32& result, bool acceptQuirkyColors)
{
    if (acceptQuirkyColors && value->unit == CSSPrimitiveValue::CSS_NUMBER
        && value->fValue >= 0. && value->fValue < 1000000. && value->isInt) {
        String str = String::format("%06d", static_cast<int>(value->fValue));
        return Color::parseHexColor(str, result);
    } else if (acceptQuirkyColors && value->unit == CSSParserValue::DimensionList) {
        CSSParserValue* numberToken = value->valueList->valueAt(0);
        CSSParserValue* unitToken = value->valueList->valueAt(1);
        ASSERT(numberToken->unit == CSSPrimitiveValue::CSS_NUMBER);
        ASSERT(unitToken->unit == CSSPrimitiveValue::CSS_IDENT);
        if (!numberToken->isInt || numberToken->fValue < 0)
            return false;
        String color = String::number(numberToken->fValue) + String(unitToken->string);
        if (color.length() > 6)
            return false;
        while (color.length() < 6)
            color = "0" + color;
        return Color::parseHexColor(color, result);
    } else if (value->unit == CSSPrimitiveValue::CSS_IDENT) {
        Color color;
        if (!color.setNamedColor(value->string))
            return acceptQuirkyColors && Color::parseHexColor(value->string, result);
        result = color.rgb();
        return true;
    } else if (value->unit == CSSParserValue::HexColor) {
        if (value->string.is8Bit())
            return Color::parseHexColor(value->string.characters8(), value->string.length(), result);
        return Color::parseHexColor(value->string.characters16(), value->string.length(), result);
    } else if (value->unit == CSSParserValue::Function &&
                value->function->args != 0 &&
                value->function->args->size() == 5 /* rgb + two commas */ &&
                value->function->id == CSSValueRgb) {
        int colorValues[3];
        if (!parseColorParameters(value, colorValues, false))
            return false;
        result = makeRGB(colorValues[0], colorValues[1], colorValues[2]);
    } else {
        if (value->unit == CSSParserValue::Function &&
                value->function->args != 0 &&
                value->function->args->size() == 7 /* rgba + three commas */ &&
                value->function->id == CSSValueRgba) {
            int colorValues[4];
            if (!parseColorParameters(value, colorValues, true))
                return false;
            result = makeRGBA(colorValues[0], colorValues[1], colorValues[2], colorValues[3]);
        } else if (value->unit == CSSParserValue::Function &&
                    value->function->args != 0 &&
                    value->function->args->size() == 5 /* hsl + two commas */ &&
                    value->function->id == CSSValueHsl) {
            double colorValues[3];
            if (!parseHSLParameters(value, colorValues, false))
                return false;
            result = makeRGBAFromHSLA(colorValues[0], colorValues[1], colorValues[2], 1.0);
        } else if (value->unit == CSSParserValue::Function &&
                    value->function->args != 0 &&
                    value->function->args->size() == 7 /* hsla + three commas */ &&
                    value->function->id == CSSValueHsla) {
            double colorValues[4];
            if (!parseHSLParameters(value, colorValues, true))
                return false;
            result = makeRGBAFromHSLA(colorValues[0], colorValues[1], colorValues[2], colorValues[3]);
        } else
            return false;
    }

    return true;
}

// This class tracks parsing state for shadow values.  If it goes out of scope (e.g., due to an early return)
// without the allowBreak bit being set, then it will clean up all of the objects and destroy them.
class ShadowParseContext {
    STACK_ALLOCATED();
public:
    ShadowParseContext(CSSPropertyID prop)
        : property(prop)
        , allowX(true)
        , allowY(false)
        , allowBlur(false)
        , allowSpread(false)
        , allowColor(true)
        , allowStyle(prop == CSSPropertyBoxShadow)
        , allowBreak(true)
    {
    }

    bool allowLength() { return allowX || allowY || allowBlur || allowSpread; }

    void commitValue()
    {
        // Handle the ,, case gracefully by doing nothing.
        if (x || y || blur || spread || color || style) {
            if (!values)
                values = CSSValueList::createCommaSeparated();

            // Construct the current shadow value and add it to the list.
            values->append(CSSShadowValue::create(x.release(), y.release(), blur.release(), spread.release(), style.release(), color.release()));
        }

        // Now reset for the next shadow value.
        x = nullptr;
        y = nullptr;
        blur = nullptr;
        spread = nullptr;
        style = nullptr;
        color = nullptr;

        allowX = true;
        allowColor = true;
        allowBreak = true;
        allowY = false;
        allowBlur = false;
        allowSpread = false;
        allowStyle = property == CSSPropertyBoxShadow;
    }

    void commitLength(PassRefPtrWillBeRawPtr<CSSPrimitiveValue> val)
    {
        if (allowX) {
            x = val;
            allowX = false;
            allowY = true;
            allowColor = false;
            allowStyle = false;
            allowBreak = false;
        } else if (allowY) {
            y = val;
            allowY = false;
            allowBlur = true;
            allowColor = true;
            allowStyle = property == CSSPropertyBoxShadow;
            allowBreak = true;
        } else if (allowBlur) {
            blur = val;
            allowBlur = false;
            allowSpread = property == CSSPropertyBoxShadow;
        } else if (allowSpread) {
            spread = val;
            allowSpread = false;
        }
    }

    void commitColor(PassRefPtrWillBeRawPtr<CSSPrimitiveValue> val)
    {
        color = val;
        allowColor = false;
        if (allowX) {
            allowStyle = false;
            allowBreak = false;
        } else {
            allowBlur = false;
            allowSpread = false;
            allowStyle = property == CSSPropertyBoxShadow;
        }
    }

    void commitStyle(CSSParserValue* v)
    {
        style = cssValuePool().createIdentifierValue(v->id);
        allowStyle = false;
        if (allowX)
            allowBreak = false;
        else {
            allowBlur = false;
            allowSpread = false;
            allowColor = false;
        }
    }

    CSSPropertyID property;

    RefPtrWillBeMember<CSSValueList> values;
    RefPtrWillBeMember<CSSPrimitiveValue> x;
    RefPtrWillBeMember<CSSPrimitiveValue> y;
    RefPtrWillBeMember<CSSPrimitiveValue> blur;
    RefPtrWillBeMember<CSSPrimitiveValue> spread;
    RefPtrWillBeMember<CSSPrimitiveValue> style;
    RefPtrWillBeMember<CSSPrimitiveValue> color;

    bool allowX;
    bool allowY;
    bool allowBlur;
    bool allowSpread;
    bool allowColor;
    bool allowStyle; // inset or not.
    bool allowBreak;
};

PassRefPtrWillBeRawPtr<CSSValueList> CSSPropertyParser::parseShadow(CSSParserValueList* valueList, CSSPropertyID propId)
{
    ShadowParseContext context(propId);
    for (CSSParserValue* val = valueList->current(); val; val = valueList->next()) {
        // Check for a comma break first.
        if (val->unit == CSSParserValue::Operator) {
            if (val->iValue != ',' || !context.allowBreak) {
                // Other operators aren't legal or we aren't done with the current shadow
                // value.  Treat as invalid.
                return nullptr;
            }
            // The value is good.  Commit it.
            context.commitValue();
        } else if (validUnit(val, FLength, HTMLStandardMode)) {
            // We required a length and didn't get one. Invalid.
            if (!context.allowLength())
                return nullptr;

            // Blur radius must be non-negative.
            if (context.allowBlur && (m_parsedCalculation ? m_parsedCalculation->isNegative() : !validUnit(val, FLength | FNonNeg, HTMLStandardMode)))
                return nullptr;

            // A length is allowed here.  Construct the value and add it.
            RefPtrWillBeRawPtr<CSSPrimitiveValue> length = createPrimitiveNumericValue(val);
            context.commitLength(length.release());
        } else if (val->id == CSSValueInset) {
            if (!context.allowStyle)
                return nullptr;

            context.commitStyle(val);
        } else {
            if (!context.allowColor)
                return nullptr;

            // The only other type of value that's ok is a color value.
            RefPtrWillBeRawPtr<CSSPrimitiveValue> parsedColor = parseColor(val);
            if (!parsedColor)
                return nullptr;

            context.commitColor(parsedColor.release());
        }
    }

    if (context.allowBreak) {
        context.commitValue();
        if (context.values && context.values->length())
            return context.values.release();
    }

    return nullptr;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseReflect()
{
    // box-reflect: <direction> <offset> <mask>

    // Direction comes first.
    CSSParserValue* val = m_valueList->current();
    RefPtrWillBeRawPtr<CSSPrimitiveValue> direction = nullptr;
    switch (val->id) {
    case CSSValueAbove:
    case CSSValueBelow:
    case CSSValueLeft:
    case CSSValueRight:
        direction = cssValuePool().createIdentifierValue(val->id);
        break;
    default:
        return nullptr;
    }

    // The offset comes next.
    val = m_valueList->next();
    RefPtrWillBeRawPtr<CSSPrimitiveValue> offset = nullptr;
    if (!val)
        offset = cssValuePool().createValue(0, CSSPrimitiveValue::CSS_PX);
    else {
        if (!validUnit(val, FLength | FPercent))
            return nullptr;
        offset = createPrimitiveNumericValue(val);
    }

    // Now for the mask.
    RefPtrWillBeRawPtr<CSSValue> mask = nullptr;
    val = m_valueList->next();
    if (val) {
        mask = parseBorderImage(CSSPropertyWebkitBoxReflect);
        if (!mask)
            return nullptr;
    }

    return CSSReflectValue::create(direction.release(), offset.release(), mask.release());
}

static bool isFlexBasisMiddleArg(double flexGrow, double flexShrink, double unsetValue, int argSize)
{
    return flexGrow != unsetValue && flexShrink == unsetValue &&  argSize == 3;
}

bool CSSPropertyParser::parseFlex(CSSParserValueList* args, bool important)
{
    if (!args || !args->size() || args->size() > 3)
        return false;
    static const double unsetValue = -1;
    double flexGrow = unsetValue;
    double flexShrink = unsetValue;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> flexBasis = nullptr;

    while (CSSParserValue* arg = args->current()) {
        if (validUnit(arg, FNumber | FNonNeg)) {
            if (flexGrow == unsetValue)
                flexGrow = arg->fValue;
            else if (flexShrink == unsetValue)
                flexShrink = arg->fValue;
            else if (!arg->fValue) {
                // flex only allows a basis of 0 (sans units) if flex-grow and flex-shrink values have already been set.
                flexBasis = cssValuePool().createValue(0, CSSPrimitiveValue::CSS_PX);
            } else {
                // We only allow 3 numbers without units if the last value is 0. E.g., flex:1 1 1 is invalid.
                return false;
            }
        } else if (!flexBasis && (arg->id == CSSValueAuto || validUnit(arg, FLength | FPercent | FNonNeg)) && !isFlexBasisMiddleArg(flexGrow, flexShrink, unsetValue, args->size()))
            flexBasis = parseValidPrimitive(arg->id, arg);
        else {
            // Not a valid arg for flex.
            return false;
        }
        args->next();
    }

    if (flexGrow == unsetValue)
        flexGrow = 1;
    if (flexShrink == unsetValue)
        flexShrink = 1;
    if (!flexBasis)
        flexBasis = cssValuePool().createValue(0, CSSPrimitiveValue::CSS_PERCENTAGE);

    addProperty(CSSPropertyFlexGrow, cssValuePool().createValue(clampTo<float>(flexGrow), CSSPrimitiveValue::CSS_NUMBER), important);
    addProperty(CSSPropertyFlexShrink, cssValuePool().createValue(clampTo<float>(flexShrink), CSSPrimitiveValue::CSS_NUMBER), important);
    addProperty(CSSPropertyFlexBasis, flexBasis, important);
    return true;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parsePosition(CSSParserValueList* valueList)
{
    RefPtrWillBeRawPtr<CSSValue> xValue = nullptr;
    RefPtrWillBeRawPtr<CSSValue> yValue = nullptr;
    parseFillPosition(valueList, xValue, yValue);
    if (!xValue || !yValue)
        return nullptr;
    return createPrimitiveValuePair(toCSSPrimitiveValue(xValue.get()), toCSSPrimitiveValue(yValue.get()), Pair::KeepIdenticalValues);
}

// Parses a list of comma separated positions. i.e., <position>#
PassRefPtrWillBeRawPtr<CSSValueList> CSSPropertyParser::parsePositionList(CSSParserValueList* valueList)
{
    RefPtrWillBeRawPtr<CSSValueList> positions = CSSValueList::createCommaSeparated();
    while (true) {
        // parsePosition consumes values until it reaches a separator [,/],
        // an invalid token, or end of the list
        RefPtrWillBeRawPtr<CSSValue> position = parsePosition(valueList);
        if (!position)
            return nullptr;
        positions->append(position);

        if (!valueList->current())
            break;
        if (!consumeComma(valueList) || !valueList->current())
            return nullptr;
    }

    return positions.release();
}

class BorderImageParseContext {
    STACK_ALLOCATED();
public:
    BorderImageParseContext()
    : m_canAdvance(false)
    , m_allowCommit(true)
    , m_allowImage(true)
    , m_allowImageSlice(true)
    , m_allowRepeat(true)
    , m_allowForwardSlashOperator(false)
    , m_allowWidth(false)
    , m_requireOutset(false)
    {}

    bool canAdvance() const { return m_canAdvance; }
    void setCanAdvance(bool canAdvance) { m_canAdvance = canAdvance; }

    bool allowCommit() const { return m_allowCommit; }
    bool allowImage() const { return m_allowImage; }
    bool allowImageSlice() const { return m_allowImageSlice; }
    bool allowRepeat() const { return m_allowRepeat; }
    bool allowForwardSlashOperator() const { return m_allowForwardSlashOperator; }

    bool allowWidth() const { return m_allowWidth; }
    bool requireOutset() const { return m_requireOutset; }

    void commitImage(PassRefPtrWillBeRawPtr<CSSValue> image)
    {
        m_image = image;
        m_canAdvance = true;
        m_allowCommit = true;
        m_allowImage = false;
        m_allowForwardSlashOperator = false;
        m_allowWidth = false;
        m_requireOutset = false;
        m_allowImageSlice = !m_imageSlice;
        m_allowRepeat = !m_repeat;
    }
    void commitImageSlice(PassRefPtrWillBeRawPtr<CSSBorderImageSliceValue> slice)
    {
        m_imageSlice = slice;
        m_canAdvance = true;
        m_allowCommit = true;
        m_allowForwardSlashOperator = true;
        m_allowImageSlice = false;
        m_allowWidth = false;
        m_requireOutset = false;
        m_allowImage = !m_image;
        m_allowRepeat = !m_repeat;
    }
    void commitForwardSlashOperator()
    {
        m_canAdvance = true;
        m_allowCommit = false;
        m_allowImage = false;
        m_allowImageSlice = false;
        m_allowRepeat = false;
        if (!m_borderWidth && !m_allowWidth) {
            m_allowForwardSlashOperator = true;
            m_allowWidth = true;
            m_requireOutset = false;
        } else {
            m_allowForwardSlashOperator = false;
            m_requireOutset = true;
            m_allowWidth = false;
        }
    }
    void commitBorderWidth(PassRefPtrWillBeRawPtr<CSSPrimitiveValue> width)
    {
        m_borderWidth = width;
        m_canAdvance = true;
        m_allowCommit = true;
        m_allowForwardSlashOperator = true;
        m_allowImageSlice = false;
        m_allowWidth = false;
        m_requireOutset = false;
        m_allowImage = !m_image;
        m_allowRepeat = !m_repeat;
    }
    void commitBorderOutset(PassRefPtrWillBeRawPtr<CSSPrimitiveValue> outset)
    {
        m_outset = outset;
        m_canAdvance = true;
        m_allowCommit = true;
        m_allowImageSlice = false;
        m_allowForwardSlashOperator = false;
        m_allowWidth = false;
        m_requireOutset = false;
        m_allowImage = !m_image;
        m_allowRepeat = !m_repeat;
    }
    void commitRepeat(PassRefPtrWillBeRawPtr<CSSValue> repeat)
    {
        m_repeat = repeat;
        m_canAdvance = true;
        m_allowCommit = true;
        m_allowRepeat = false;
        m_allowForwardSlashOperator = false;
        m_allowWidth = false;
        m_requireOutset = false;
        m_allowImageSlice = !m_imageSlice;
        m_allowImage = !m_image;
    }

    PassRefPtrWillBeRawPtr<CSSValue> commitCSSValue()
    {
        return createBorderImageValue(m_image, m_imageSlice.get(), m_borderWidth.get(), m_outset.get(), m_repeat.get());
    }

    bool m_canAdvance;

    bool m_allowCommit;
    bool m_allowImage;
    bool m_allowImageSlice;
    bool m_allowRepeat;
    bool m_allowForwardSlashOperator;

    bool m_allowWidth;
    bool m_requireOutset;

    RefPtrWillBeMember<CSSValue> m_image;
    RefPtrWillBeMember<CSSBorderImageSliceValue> m_imageSlice;
    RefPtrWillBeMember<CSSPrimitiveValue> m_borderWidth;
    RefPtrWillBeMember<CSSPrimitiveValue> m_outset;

    RefPtrWillBeMember<CSSValue> m_repeat;
};

bool CSSPropertyParser::buildBorderImageParseContext(CSSPropertyID propId, BorderImageParseContext& context)
{
    CSSPropertyParser::ShorthandScope scope(this, propId);
    while (CSSParserValue* val = m_valueList->current()) {
        context.setCanAdvance(false);

        if (!context.canAdvance() && context.allowForwardSlashOperator() && isForwardSlashOperator(val))
            context.commitForwardSlashOperator();

        if (!context.canAdvance() && context.allowImage()) {
            if (val->unit == CSSPrimitiveValue::CSS_URI) {
                context.commitImage(createCSSImageValueWithReferrer(val->string, m_context.completeURL(val->string)));
            } else if (isGeneratedImageValue(val)) {
                RefPtrWillBeRawPtr<CSSValue> value = nullptr;
                if (parseGeneratedImage(m_valueList, value))
                    context.commitImage(value.release());
                else
                    return false;
            } else if (val->unit == CSSParserValue::Function && val->function->id == CSSValueWebkitImageSet) {
                RefPtrWillBeRawPtr<CSSValue> value = parseImageSet(m_valueList);
                if (value)
                    context.commitImage(value.release());
                else
                    return false;
            } else if (val->id == CSSValueNone)
                context.commitImage(cssValuePool().createIdentifierValue(CSSValueNone));
        }

        if (!context.canAdvance() && context.allowImageSlice()) {
            RefPtrWillBeRawPtr<CSSBorderImageSliceValue> imageSlice = nullptr;
            if (parseBorderImageSlice(propId, imageSlice))
                context.commitImageSlice(imageSlice.release());
        }

        if (!context.canAdvance() && context.allowRepeat()) {
            RefPtrWillBeRawPtr<CSSValue> repeat = nullptr;
            if (parseBorderImageRepeat(repeat))
                context.commitRepeat(repeat.release());
        }

        if (!context.canAdvance() && context.allowWidth()) {
            RefPtrWillBeRawPtr<CSSPrimitiveValue> borderWidth = nullptr;
            if (parseBorderImageWidth(borderWidth))
                context.commitBorderWidth(borderWidth.release());
        }

        if (!context.canAdvance() && context.requireOutset()) {
            RefPtrWillBeRawPtr<CSSPrimitiveValue> borderOutset = nullptr;
            if (parseBorderImageOutset(borderOutset))
                context.commitBorderOutset(borderOutset.release());
        }

        if (!context.canAdvance())
            return false;

        m_valueList->next();
    }

    return context.allowCommit();
}

void CSSPropertyParser::commitBorderImageProperty(CSSPropertyID propId, PassRefPtrWillBeRawPtr<CSSValue> value, bool important)
{
    if (value)
        addProperty(propId, value, important);
    else
        addProperty(propId, cssValuePool().createImplicitInitialValue(), important, true);
}

bool CSSPropertyParser::parseBorderImageShorthand(CSSPropertyID propId, bool important)
{
    BorderImageParseContext context;
    if (buildBorderImageParseContext(propId, context)) {
        switch (propId) {
        case CSSPropertyWebkitMaskBoxImage:
            commitBorderImageProperty(CSSPropertyWebkitMaskBoxImageSource, context.m_image, important);
            commitBorderImageProperty(CSSPropertyWebkitMaskBoxImageSlice, context.m_imageSlice.get(), important);
            commitBorderImageProperty(CSSPropertyWebkitMaskBoxImageWidth, context.m_borderWidth.get(), important);
            commitBorderImageProperty(CSSPropertyWebkitMaskBoxImageOutset, context.m_outset.get(), important);
            commitBorderImageProperty(CSSPropertyWebkitMaskBoxImageRepeat, context.m_repeat.get(), important);
            return true;
        case CSSPropertyBorderImage:
            commitBorderImageProperty(CSSPropertyBorderImageSource, context.m_image, important);
            commitBorderImageProperty(CSSPropertyBorderImageSlice, context.m_imageSlice.get(), important);
            commitBorderImageProperty(CSSPropertyBorderImageWidth, context.m_borderWidth.get(), important);
            commitBorderImageProperty(CSSPropertyBorderImageOutset, context.m_outset.get(), important);
            commitBorderImageProperty(CSSPropertyBorderImageRepeat, context.m_repeat, important);
            return true;
        default:
            ASSERT_NOT_REACHED();
            return false;
        }
    }
    return false;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseBorderImage(CSSPropertyID propId)
{
    BorderImageParseContext context;
    if (buildBorderImageParseContext(propId, context)) {
        return context.commitCSSValue();
    }
    return nullptr;
}

static bool isBorderImageRepeatKeyword(int id)
{
    return id == CSSValueStretch || id == CSSValueRepeat || id == CSSValueSpace || id == CSSValueRound;
}

bool CSSPropertyParser::parseBorderImageRepeat(RefPtrWillBeRawPtr<CSSValue>& result)
{
    RefPtrWillBeRawPtr<CSSPrimitiveValue> firstValue = nullptr;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> secondValue = nullptr;
    CSSParserValue* val = m_valueList->current();
    if (!val)
        return false;
    if (isBorderImageRepeatKeyword(val->id))
        firstValue = cssValuePool().createIdentifierValue(val->id);
    else
        return false;

    val = m_valueList->next();
    if (val) {
        if (isBorderImageRepeatKeyword(val->id))
            secondValue = cssValuePool().createIdentifierValue(val->id);
        else if (!inShorthand()) {
            // If we're not parsing a shorthand then we are invalid.
            return false;
        } else {
            // We need to rewind the value list, so that when its advanced we'll
            // end up back at this value.
            m_valueList->previous();
            secondValue = firstValue;
        }
    } else
        secondValue = firstValue;

    result = createPrimitiveValuePair(firstValue, secondValue);
    return true;
}

class BorderImageSliceParseContext {
    STACK_ALLOCATED();
public:
    BorderImageSliceParseContext()
    : m_allowNumber(true)
    , m_allowFill(true)
    , m_allowFinalCommit(false)
    , m_fill(false)
    { }

    bool allowNumber() const { return m_allowNumber; }
    bool allowFill() const { return m_allowFill; }
    bool allowFinalCommit() const { return m_allowFinalCommit; }
    CSSPrimitiveValue* top() const { return m_top.get(); }

    void commitNumber(PassRefPtrWillBeRawPtr<CSSPrimitiveValue> val)
    {
        if (!m_top)
            m_top = val;
        else if (!m_right)
            m_right = val;
        else if (!m_bottom)
            m_bottom = val;
        else {
            ASSERT(!m_left);
            m_left = val;
        }

        m_allowNumber = !m_left;
        m_allowFinalCommit = true;
    }

    void commitFill() { m_fill = true; m_allowFill = false; m_allowNumber = !m_top; }

    PassRefPtrWillBeRawPtr<CSSBorderImageSliceValue> commitBorderImageSlice()
    {
        // We need to clone and repeat values for any omissions.
        ASSERT(m_top);
        if (!m_right) {
            m_right = m_top;
            m_bottom = m_top;
            m_left = m_top;
        }
        if (!m_bottom) {
            m_bottom = m_top;
            m_left = m_right;
        }
        if (!m_left)
            m_left = m_right;

        // Now build a rect value to hold all four of our primitive values.
        RefPtrWillBeRawPtr<Quad> quad = Quad::create();
        quad->setTop(m_top);
        quad->setRight(m_right);
        quad->setBottom(m_bottom);
        quad->setLeft(m_left);

        // Make our new border image value now.
        return CSSBorderImageSliceValue::create(cssValuePool().createValue(quad.release()), m_fill);
    }

private:
    bool m_allowNumber;
    bool m_allowFill;
    bool m_allowFinalCommit;

    RefPtrWillBeMember<CSSPrimitiveValue> m_top;
    RefPtrWillBeMember<CSSPrimitiveValue> m_right;
    RefPtrWillBeMember<CSSPrimitiveValue> m_bottom;
    RefPtrWillBeMember<CSSPrimitiveValue> m_left;

    bool m_fill;
};

bool CSSPropertyParser::parseBorderImageSlice(CSSPropertyID propId, RefPtrWillBeRawPtr<CSSBorderImageSliceValue>& result)
{
    BorderImageSliceParseContext context;
    for (CSSParserValue* val = m_valueList->current(); val; val = m_valueList->next()) {
        // FIXME calc() http://webkit.org/b/16662 : calc is parsed but values are not created yet.
        if (context.allowNumber() && !isCalculation(val) && validUnit(val, FInteger | FNonNeg | FPercent)) {
            context.commitNumber(createPrimitiveNumericValue(val));
        } else if (context.allowFill() && val->id == CSSValueFill) {
            context.commitFill();
        } else if (!inShorthand()) {
            // If we're not parsing a shorthand then we are invalid.
            return false;
        } else {
            if (context.allowFinalCommit()) {
                // We're going to successfully parse, but we don't want to consume this token.
                m_valueList->previous();
            }
            break;
        }
    }

    if (context.allowFinalCommit()) {
        // FIXME: For backwards compatibility, -webkit-border-image, -webkit-mask-box-image and -webkit-box-reflect have to do a fill by default.
        // FIXME: What do we do with -webkit-box-reflect and -webkit-mask-box-image? Probably just have to leave them filling...
        if (propId == CSSPropertyWebkitBorderImage || propId == CSSPropertyWebkitMaskBoxImage || propId == CSSPropertyWebkitBoxReflect)
            context.commitFill();

        // Need to fully commit as a single value.
        result = context.commitBorderImageSlice();
        return true;
    }

    return false;
}

class BorderImageQuadParseContext {
    STACK_ALLOCATED();
public:
    BorderImageQuadParseContext()
    : m_allowNumber(true)
    , m_allowFinalCommit(false)
    { }

    bool allowNumber() const { return m_allowNumber; }
    bool allowFinalCommit() const { return m_allowFinalCommit; }
    CSSPrimitiveValue* top() const { return m_top.get(); }

    void commitNumber(PassRefPtrWillBeRawPtr<CSSPrimitiveValue> val)
    {
        if (!m_top)
            m_top = val;
        else if (!m_right)
            m_right = val;
        else if (!m_bottom)
            m_bottom = val;
        else {
            ASSERT(!m_left);
            m_left = val;
        }

        m_allowNumber = !m_left;
        m_allowFinalCommit = true;
    }

    void setTop(PassRefPtrWillBeRawPtr<CSSPrimitiveValue> val) { m_top = val; }

    PassRefPtrWillBeRawPtr<CSSPrimitiveValue> commitBorderImageQuad()
    {
        // We need to clone and repeat values for any omissions.
        ASSERT(m_top);
        if (!m_right) {
            m_right = m_top;
            m_bottom = m_top;
            m_left = m_top;
        }
        if (!m_bottom) {
            m_bottom = m_top;
            m_left = m_right;
        }
        if (!m_left)
            m_left = m_right;

        // Now build a quad value to hold all four of our primitive values.
        RefPtrWillBeRawPtr<Quad> quad = Quad::create();
        quad->setTop(m_top);
        quad->setRight(m_right);
        quad->setBottom(m_bottom);
        quad->setLeft(m_left);

        // Make our new value now.
        return cssValuePool().createValue(quad.release());
    }

private:
    bool m_allowNumber;
    bool m_allowFinalCommit;

    RefPtrWillBeMember<CSSPrimitiveValue> m_top;
    RefPtrWillBeMember<CSSPrimitiveValue> m_right;
    RefPtrWillBeMember<CSSPrimitiveValue> m_bottom;
    RefPtrWillBeMember<CSSPrimitiveValue> m_left;
};

bool CSSPropertyParser::parseBorderImageQuad(Units validUnits, RefPtrWillBeRawPtr<CSSPrimitiveValue>& result)
{
    BorderImageQuadParseContext context;
    for (CSSParserValue* val = m_valueList->current(); val; val = m_valueList->next()) {
        if (context.allowNumber() && (validUnit(val, validUnits, HTMLStandardMode) || val->id == CSSValueAuto)) {
            if (val->id == CSSValueAuto)
                context.commitNumber(cssValuePool().createIdentifierValue(val->id));
            else
                context.commitNumber(createPrimitiveNumericValue(val));
        } else if (!inShorthand()) {
            // If we're not parsing a shorthand then we are invalid.
            return false;
        } else {
            if (context.allowFinalCommit())
                m_valueList->previous(); // The shorthand loop will advance back to this point.
            break;
        }
    }

    if (context.allowFinalCommit()) {
        // Need to fully commit as a single value.
        result = context.commitBorderImageQuad();
        return true;
    }
    return false;
}

bool CSSPropertyParser::parseBorderImageWidth(RefPtrWillBeRawPtr<CSSPrimitiveValue>& result)
{
    return parseBorderImageQuad(FLength | FNumber | FNonNeg | FPercent, result);
}

bool CSSPropertyParser::parseBorderImageOutset(RefPtrWillBeRawPtr<CSSPrimitiveValue>& result)
{
    return parseBorderImageQuad(FLength | FNumber | FNonNeg, result);
}

bool CSSPropertyParser::parseBorderRadius(CSSPropertyID unresolvedProperty, bool important)
{
    unsigned num = m_valueList->size();
    if (num > 9)
        return false;

    ShorthandScope scope(this, unresolvedProperty);
    RefPtrWillBeRawPtr<CSSPrimitiveValue> radii[2][4];
#if ENABLE(OILPAN)
    // Zero initialize the array of raw pointers.
    memset(&radii, 0, sizeof(radii));
#endif

    unsigned indexAfterSlash = 0;
    for (unsigned i = 0; i < num; ++i) {
        CSSParserValue* value = m_valueList->valueAt(i);
        if (value->unit == CSSParserValue::Operator) {
            if (value->iValue != '/')
                return false;

            if (!i || indexAfterSlash || i + 1 == num || num > i + 5)
                return false;

            indexAfterSlash = i + 1;
            completeBorderRadii(radii[0]);
            continue;
        }

        if (i - indexAfterSlash >= 4)
            return false;

        if (!validUnit(value, FLength | FPercent | FNonNeg))
            return false;

        RefPtrWillBeRawPtr<CSSPrimitiveValue> radius = createPrimitiveNumericValue(value);

        if (!indexAfterSlash) {
            radii[0][i] = radius;

            // Legacy syntax: -webkit-border-radius: l1 l2; is equivalent to border-radius: l1 / l2;
            if (num == 2 && unresolvedProperty == CSSPropertyAliasWebkitBorderRadius) {
                indexAfterSlash = 1;
                completeBorderRadii(radii[0]);
            }
        } else
            radii[1][i - indexAfterSlash] = radius.release();
    }

    if (!indexAfterSlash) {
        completeBorderRadii(radii[0]);
        for (unsigned i = 0; i < 4; ++i)
            radii[1][i] = radii[0][i];
    } else
        completeBorderRadii(radii[1]);

    ImplicitScope implicitScope(this);
    addProperty(CSSPropertyBorderTopLeftRadius, createPrimitiveValuePair(radii[0][0].release(), radii[1][0].release()), important);
    addProperty(CSSPropertyBorderTopRightRadius, createPrimitiveValuePair(radii[0][1].release(), radii[1][1].release()), important);
    addProperty(CSSPropertyBorderBottomRightRadius, createPrimitiveValuePair(radii[0][2].release(), radii[1][2].release()), important);
    addProperty(CSSPropertyBorderBottomLeftRadius, createPrimitiveValuePair(radii[0][3].release(), radii[1][3].release()), important);
    return true;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseCounter(int defaultValue)
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createCommaSeparated();

    while (m_valueList->current()) {
        CSSParserValue* val = m_valueList->current();
        if (val->unit != CSSPrimitiveValue::CSS_IDENT)
            return nullptr;
        RefPtrWillBeRawPtr<CSSPrimitiveValue> counterName = createPrimitiveCustomIdentValue(val);
        m_valueList->next();

        val = m_valueList->current();
        int i = defaultValue;
        if (val && validUnit(val, FInteger)) {
            i = clampTo<int>(val->fValue);
            m_valueList->next();
        }

        list->append(createPrimitiveValuePair(counterName.release(),
            cssValuePool().createValue(i, CSSPrimitiveValue::CSS_NUMBER)));
    }

    if (!list->length())
        return nullptr;
    return list.release();
}

// This should go away once we drop support for -webkit-gradient
static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> parseDeprecatedGradientPoint(CSSParserValue* a, bool horizontal)
{
    RefPtrWillBeRawPtr<CSSPrimitiveValue> result = nullptr;
    if (a->unit == CSSPrimitiveValue::CSS_IDENT) {
        if ((a->id == CSSValueLeft && horizontal)
            || (a->id == CSSValueTop && !horizontal))
            result = cssValuePool().createValue(0., CSSPrimitiveValue::CSS_PERCENTAGE);
        else if ((a->id == CSSValueRight && horizontal)
            || (a->id == CSSValueBottom && !horizontal))
            result = cssValuePool().createValue(100., CSSPrimitiveValue::CSS_PERCENTAGE);
        else if (a->id == CSSValueCenter)
            result = cssValuePool().createValue(50., CSSPrimitiveValue::CSS_PERCENTAGE);
    } else if (a->unit == CSSPrimitiveValue::CSS_NUMBER || a->unit == CSSPrimitiveValue::CSS_PERCENTAGE) {
        result = cssValuePool().createValue(a->fValue, static_cast<CSSPrimitiveValue::UnitType>(a->unit));
    }
    return result;
}

bool CSSPropertyParser::parseDeprecatedGradientColorStop(CSSParserValue* a, CSSGradientColorStop& stop)
{
    if (a->unit != CSSParserValue::Function)
        return false;

    if (a->function->id != CSSValueFrom
        && a->function->id != CSSValueTo
        && a->function->id != CSSValueColorStop)
        return false;

    CSSParserValueList* args = a->function->args.get();
    if (!args)
        return false;

    if (a->function->id == CSSValueFrom || a->function->id == CSSValueTo) {
        // The "from" and "to" stops expect 1 argument.
        if (args->size() != 1)
            return false;

        if (a->function->id == CSSValueFrom)
            stop.m_position = cssValuePool().createValue(0, CSSPrimitiveValue::CSS_NUMBER);
        else
            stop.m_position = cssValuePool().createValue(1, CSSPrimitiveValue::CSS_NUMBER);

        stop.m_color = parseDeprecatedGradientStopColor(args->current());
        if (!stop.m_color)
            return false;
    }

    // The "color-stop" function expects 3 arguments.
    if (a->function->id == CSSValueColorStop) {
        if (args->size() != 3)
            return false;

        CSSParserValue* stopArg = args->current();
        if (stopArg->unit == CSSPrimitiveValue::CSS_PERCENTAGE)
            stop.m_position = cssValuePool().createValue(stopArg->fValue / 100, CSSPrimitiveValue::CSS_NUMBER);
        else if (stopArg->unit == CSSPrimitiveValue::CSS_NUMBER)
            stop.m_position = cssValuePool().createValue(stopArg->fValue, CSSPrimitiveValue::CSS_NUMBER);
        else
            return false;

        args->next();
        if (!consumeComma(args))
            return false;

        stop.m_color = parseDeprecatedGradientStopColor(args->current());
        if (!stop.m_color)
            return false;
    }

    return true;
}

bool CSSPropertyParser::parseDeprecatedGradient(CSSParserValueList* valueList, RefPtrWillBeRawPtr<CSSValue>& gradient)
{
    // Walk the arguments.
    CSSParserValueList* args = valueList->current()->function->args.get();
    if (!args || args->size() == 0)
        return false;

    // The first argument is the gradient type.  It is an identifier.
    CSSGradientType gradientType;
    CSSParserValue* a = args->current();
    if (!a || a->unit != CSSPrimitiveValue::CSS_IDENT)
        return false;
    if (a->id == CSSValueLinear)
        gradientType = CSSDeprecatedLinearGradient;
    else if (a->id == CSSValueRadial)
        gradientType = CSSDeprecatedRadialGradient;
    else
        return false;

    RefPtrWillBeRawPtr<CSSGradientValue> result = nullptr;
    switch (gradientType) {
    case CSSDeprecatedLinearGradient:
        result = CSSLinearGradientValue::create(NonRepeating, gradientType);
        break;
    case CSSDeprecatedRadialGradient:
        result = CSSRadialGradientValue::create(NonRepeating, gradientType);
        break;
    default:
        // The rest of the gradient types shouldn't appear here.
        ASSERT_NOT_REACHED();
    }
    args->next();

    if (!consumeComma(args))
        return false;

    // Next comes the starting point for the gradient as an x y pair.  There is no
    // comma between the x and the y values.
    // First X.  It can be left, right, number or percent.
    a = args->current();
    if (!a)
        return false;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> point = parseDeprecatedGradientPoint(a, true);
    if (!point)
        return false;
    result->setFirstX(point.release());

    // First Y.  It can be top, bottom, number or percent.
    a = args->next();
    if (!a)
        return false;
    point = parseDeprecatedGradientPoint(a, false);
    if (!point)
        return false;
    result->setFirstY(point.release());

    // Comma after the first point.
    args->next();
    if (!consumeComma(args))
        return false;

    // For radial gradients only, we now expect a numeric radius.
    if (gradientType == CSSDeprecatedRadialGradient) {
        a = args->current();
        if (!a || a->unit != CSSPrimitiveValue::CSS_NUMBER)
            return false;
        toCSSRadialGradientValue(result.get())->setFirstRadius(createPrimitiveNumericValue(a));

        // Comma after the first radius.
        args->next();
        if (!consumeComma(args))
            return false;
    }

    // Next is the ending point for the gradient as an x, y pair.
    // Second X.  It can be left, right, number or percent.
    a = args->current();
    if (!a)
        return false;
    point = parseDeprecatedGradientPoint(a, true);
    if (!point)
        return false;
    result->setSecondX(point.release());

    // Second Y.  It can be top, bottom, number or percent.
    a = args->next();
    if (!a)
        return false;
    point = parseDeprecatedGradientPoint(a, false);
    if (!point)
        return false;
    result->setSecondY(point.release());
    args->next();

    // For radial gradients only, we now expect the second radius.
    if (gradientType == CSSDeprecatedRadialGradient) {
        // Comma after the second point.
        if (!consumeComma(args))
            return false;

        a = args->current();
        if (!a || a->unit != CSSPrimitiveValue::CSS_NUMBER)
            return false;
        toCSSRadialGradientValue(result.get())->setSecondRadius(createPrimitiveNumericValue(a));
        args->next();
    }

    // We now will accept any number of stops (0 or more).
    a = args->current();
    while (a) {
        // Look for the comma before the next stop.
        if (!consumeComma(args))
            return false;

        // Now examine the stop itself.
        a = args->current();
        if (!a)
            return false;

        // The function name needs to be one of "from", "to", or "color-stop."
        CSSGradientColorStop stop;
        if (!parseDeprecatedGradientColorStop(a, stop))
            return false;
        result->addStop(stop);

        // Advance
        a = args->next();
    }

    gradient = result.release();
    return true;
}

static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> valueFromSideKeyword(CSSParserValue* a, bool& isHorizontal)
{
    if (a->unit != CSSPrimitiveValue::CSS_IDENT)
        return nullptr;

    switch (a->id) {
        case CSSValueLeft:
        case CSSValueRight:
            isHorizontal = true;
            break;
        case CSSValueTop:
        case CSSValueBottom:
            isHorizontal = false;
            break;
        default:
            return nullptr;
    }
    return cssValuePool().createIdentifierValue(a->id);
}

bool CSSPropertyParser::parseDeprecatedLinearGradient(CSSParserValueList* valueList, RefPtrWillBeRawPtr<CSSValue>& gradient, CSSGradientRepeat repeating)
{
    RefPtrWillBeRawPtr<CSSLinearGradientValue> result = CSSLinearGradientValue::create(repeating, CSSPrefixedLinearGradient);

    // Walk the arguments.
    CSSParserValueList* args = valueList->current()->function->args.get();
    if (!args || !args->size())
        return false;

    CSSParserValue* a = args->current();
    if (!a)
        return false;

    bool expectComma = false;
    // Look for angle.
    if (validUnit(a, FAngle, HTMLStandardMode)) {
        result->setAngle(createPrimitiveNumericValue(a));

        args->next();
        expectComma = true;
    } else {
        // Look one or two optional keywords that indicate a side or corner.
        RefPtrWillBeRawPtr<CSSPrimitiveValue> startX = nullptr;
        RefPtrWillBeRawPtr<CSSPrimitiveValue> startY = nullptr;

        RefPtrWillBeRawPtr<CSSPrimitiveValue> location = nullptr;
        bool isHorizontal = false;
        if ((location = valueFromSideKeyword(a, isHorizontal))) {
            if (isHorizontal)
                startX = location;
            else
                startY = location;

            a = args->next();
            if (a) {
                if ((location = valueFromSideKeyword(a, isHorizontal))) {
                    if (isHorizontal) {
                        if (startX)
                            return false;
                        startX = location;
                    } else {
                        if (startY)
                            return false;
                        startY = location;
                    }

                    args->next();
                }
            }

            expectComma = true;
        }

        if (!startX && !startY)
            startY = cssValuePool().createIdentifierValue(CSSValueTop);

        result->setFirstX(startX.release());
        result->setFirstY(startY.release());
    }

    if (!parseGradientColorStops(args, result.get(), expectComma))
        return false;

    if (!result->stopCount())
        return false;

    gradient = result.release();
    return true;
}

bool CSSPropertyParser::parseDeprecatedRadialGradient(CSSParserValueList* valueList, RefPtrWillBeRawPtr<CSSValue>& gradient, CSSGradientRepeat repeating)
{
    RefPtrWillBeRawPtr<CSSRadialGradientValue> result = CSSRadialGradientValue::create(repeating, CSSPrefixedRadialGradient);

    // Walk the arguments.
    CSSParserValueList* args = valueList->current()->function->args.get();
    if (!args || !args->size())
        return false;

    CSSParserValue* a = args->current();
    if (!a)
        return false;

    bool expectComma = false;

    // Optional background-position
    RefPtrWillBeRawPtr<CSSValue> centerX = nullptr;
    RefPtrWillBeRawPtr<CSSValue> centerY = nullptr;
    // parse2ValuesFillPosition advances the args next pointer.
    parse2ValuesFillPosition(args, centerX, centerY);

    if ((centerX || centerY) && !consumeComma(args))
        return false;

    a = args->current();
    if (!a)
        return false;

    result->setFirstX(toCSSPrimitiveValue(centerX.get()));
    result->setSecondX(toCSSPrimitiveValue(centerX.get()));
    // CSS3 radial gradients always share the same start and end point.
    result->setFirstY(toCSSPrimitiveValue(centerY.get()));
    result->setSecondY(toCSSPrimitiveValue(centerY.get()));

    RefPtrWillBeRawPtr<CSSPrimitiveValue> shapeValue = nullptr;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> sizeValue = nullptr;

    // Optional shape and/or size in any order.
    for (int i = 0; i < 2; ++i) {
        if (a->unit != CSSPrimitiveValue::CSS_IDENT)
            break;

        bool foundValue = false;
        switch (a->id) {
        case CSSValueCircle:
        case CSSValueEllipse:
            shapeValue = cssValuePool().createIdentifierValue(a->id);
            foundValue = true;
            break;
        case CSSValueClosestSide:
        case CSSValueClosestCorner:
        case CSSValueFarthestSide:
        case CSSValueFarthestCorner:
        case CSSValueContain:
        case CSSValueCover:
            sizeValue = cssValuePool().createIdentifierValue(a->id);
            foundValue = true;
            break;
        default:
            break;
        }

        if (foundValue) {
            a = args->next();
            if (!a)
                return false;

            expectComma = true;
        }
    }

    result->setShape(shapeValue);
    result->setSizingBehavior(sizeValue);

    // Or, two lengths or percentages
    RefPtrWillBeRawPtr<CSSPrimitiveValue> horizontalSize = nullptr;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> verticalSize = nullptr;

    if (!shapeValue && !sizeValue) {
        if (validUnit(a, FLength | FPercent)) {
            horizontalSize = createPrimitiveNumericValue(a);
            a = args->next();
            if (!a)
                return false;

            expectComma = true;
        }

        if (validUnit(a, FLength | FPercent)) {
            verticalSize = createPrimitiveNumericValue(a);

            a = args->next();
            if (!a)
                return false;
            expectComma = true;
        }
    }

    // Must have neither or both.
    if (!horizontalSize != !verticalSize)
        return false;

    result->setEndHorizontalSize(horizontalSize);
    result->setEndVerticalSize(verticalSize);

    if (!parseGradientColorStops(args, result.get(), expectComma))
        return false;

    gradient = result.release();
    return true;
}

bool CSSPropertyParser::parseLinearGradient(CSSParserValueList* valueList, RefPtrWillBeRawPtr<CSSValue>& gradient, CSSGradientRepeat repeating)
{
    RefPtrWillBeRawPtr<CSSLinearGradientValue> result = CSSLinearGradientValue::create(repeating, CSSLinearGradient);

    CSSParserFunction* function = valueList->current()->function;
    CSSParserValueList* args = function->args.get();
    if (!args || !args->size())
        return false;

    CSSParserValue* a = args->current();
    if (!a)
        return false;

    bool expectComma = false;
    // Look for angle.
    if (validUnit(a, FAngle, HTMLStandardMode)) {
        result->setAngle(createPrimitiveNumericValue(a));

        args->next();
        expectComma = true;
    } else if (a->unit == CSSPrimitiveValue::CSS_IDENT && a->id == CSSValueTo) {
        // to [ [left | right] || [top | bottom] ]
        a = args->next();
        if (!a)
            return false;

        RefPtrWillBeRawPtr<CSSPrimitiveValue> endX = nullptr;
        RefPtrWillBeRawPtr<CSSPrimitiveValue> endY = nullptr;
        RefPtrWillBeRawPtr<CSSPrimitiveValue> location = nullptr;
        bool isHorizontal = false;

        location = valueFromSideKeyword(a, isHorizontal);
        if (!location)
            return false;

        if (isHorizontal)
            endX = location;
        else
            endY = location;

        a = args->next();
        if (!a)
            return false;

        location = valueFromSideKeyword(a, isHorizontal);
        if (location) {
            if (isHorizontal) {
                if (endX)
                    return false;
                endX = location;
            } else {
                if (endY)
                    return false;
                endY = location;
            }

            args->next();
        }

        expectComma = true;
        result->setFirstX(endX.release());
        result->setFirstY(endY.release());
    }

    if (!parseGradientColorStops(args, result.get(), expectComma))
        return false;

    if (!result->stopCount())
        return false;

    gradient = result.release();
    return true;
}

bool CSSPropertyParser::parseRadialGradient(CSSParserValueList* valueList, RefPtrWillBeRawPtr<CSSValue>& gradient, CSSGradientRepeat repeating)
{
    RefPtrWillBeRawPtr<CSSRadialGradientValue> result = CSSRadialGradientValue::create(repeating, CSSRadialGradient);

    CSSParserValueList* args = valueList->current()->function->args.get();
    if (!args || !args->size())
        return false;

    CSSParserValue* a = args->current();
    if (!a)
        return false;

    bool expectComma = false;

    RefPtrWillBeRawPtr<CSSPrimitiveValue> shapeValue = nullptr;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> sizeValue = nullptr;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> horizontalSize = nullptr;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> verticalSize = nullptr;

    // First part of grammar, the size/shape clause:
    // [ circle || <length> ] |
    // [ ellipse || [ <length> | <percentage> ]{2} ] |
    // [ [ circle | ellipse] || <size-keyword> ]
    for (int i = 0; i < 3; ++i) {
        if (a->unit == CSSPrimitiveValue::CSS_IDENT) {
            bool badIdent = false;
            switch (a->id) {
            case CSSValueCircle:
            case CSSValueEllipse:
                if (shapeValue)
                    return false;
                shapeValue = cssValuePool().createIdentifierValue(a->id);
                break;
            case CSSValueClosestSide:
            case CSSValueClosestCorner:
            case CSSValueFarthestSide:
            case CSSValueFarthestCorner:
                if (sizeValue || horizontalSize)
                    return false;
                sizeValue = cssValuePool().createIdentifierValue(a->id);
                break;
            default:
                badIdent = true;
            }

            if (badIdent)
                break;

            a = args->next();
            if (!a)
                return false;
        } else if (validUnit(a, FLength | FPercent)) {

            if (sizeValue || horizontalSize)
                return false;
            horizontalSize = createPrimitiveNumericValue(a);

            a = args->next();
            if (!a)
                return false;

            if (validUnit(a, FLength | FPercent)) {
                verticalSize = createPrimitiveNumericValue(a);
                ++i;
                a = args->next();
                if (!a)
                    return false;
            }
        } else
            break;
    }

    // You can specify size as a keyword or a length/percentage, not both.
    if (sizeValue && horizontalSize)
        return false;
    // Circles must have 0 or 1 lengths.
    if (shapeValue && shapeValue->getValueID() == CSSValueCircle && verticalSize)
        return false;
    // Ellipses must have 0 or 2 length/percentages.
    if (shapeValue && shapeValue->getValueID() == CSSValueEllipse && horizontalSize && !verticalSize)
        return false;
    // If there's only one size, it must be a length.
    if (!verticalSize && horizontalSize && horizontalSize->isPercentage())
        return false;

    result->setShape(shapeValue);
    result->setSizingBehavior(sizeValue);
    result->setEndHorizontalSize(horizontalSize);
    result->setEndVerticalSize(verticalSize);

    // Second part of grammar, the center-position clause:
    // at <position>
    RefPtrWillBeRawPtr<CSSValue> centerX = nullptr;
    RefPtrWillBeRawPtr<CSSValue> centerY = nullptr;
    if (a->unit == CSSPrimitiveValue::CSS_IDENT && a->id == CSSValueAt) {
        a = args->next();
        if (!a)
            return false;

        parseFillPosition(args, centerX, centerY);
        if (!(centerX && centerY))
            return false;

        a = args->current();
        if (!a)
            return false;
        result->setFirstX(toCSSPrimitiveValue(centerX.get()));
        result->setFirstY(toCSSPrimitiveValue(centerY.get()));
        // Right now, CSS radial gradients have the same start and end centers.
        result->setSecondX(toCSSPrimitiveValue(centerX.get()));
        result->setSecondY(toCSSPrimitiveValue(centerY.get()));
    }

    if (shapeValue || sizeValue || horizontalSize || centerX || centerY)
        expectComma = true;

    if (!parseGradientColorStops(args, result.get(), expectComma))
        return false;

    gradient = result.release();
    return true;
}

bool CSSPropertyParser::parseGradientColorStops(CSSParserValueList* valueList, CSSGradientValue* gradient, bool expectComma)
{
    CSSParserValue* a = valueList->current();

    // Now look for color stops.
    // <color-stop-list> = [ <color-stop> , <color-hint>? ]# , <color-stop>
    bool supportsColorHints = gradient->gradientType() == CSSLinearGradient
        || gradient->gradientType() == CSSRadialGradient;

    // The first color stop cannot be a color hint.
    bool previousStopWasColorHint = true;
    while (a) {
        // Look for the comma before the next stop.
        if (expectComma) {
            if (!isComma(a))
                return false;

            a = valueList->next();
            if (!a)
                return false;
        }

        // <color-stop> = <color> [ <percentage> | <length> ]?
        // <color-hint> = <length> | <percentage>
        CSSGradientColorStop stop;
        stop.m_color = parseGradientStopColor(a);

        // Two hints in a row are not allowed.
        if (!stop.m_color && (!supportsColorHints || previousStopWasColorHint))
            return false;
        previousStopWasColorHint = !stop.m_color;

        if (stop.m_color)
            a = valueList->next();

        if (a) {
            if (validUnit(a, FLength | FPercent)) {
                stop.m_position = createPrimitiveNumericValue(a);
                a = valueList->next();
            }
        }

        if (!stop.m_color && !stop.m_position)
            return false;

        gradient->addStop(stop);
        expectComma = true;
    }

    // The last color stop cannot be a color hint.
    if (previousStopWasColorHint)
        return false;

    // Must have 2 or more stops to be valid.
    return gradient->stopCount() >= 2;
}

bool CSSPropertyParser::parseGeneratedImage(CSSParserValueList* valueList, RefPtrWillBeRawPtr<CSSValue>& value)
{
    CSSParserValue* val = valueList->current();

    if (val->unit != CSSParserValue::Function)
        return false;

    if (val->function->id == CSSValueWebkitGradient) {
        // FIXME: This should send a deprecation message.
        if (m_context.useCounter())
            m_context.useCounter()->count(UseCounter::DeprecatedWebKitGradient);
        return parseDeprecatedGradient(valueList, value);
    }

    if (val->function->id == CSSValueWebkitLinearGradient) {
        // FIXME: This should send a deprecation message.
        if (m_context.useCounter())
            m_context.useCounter()->count(UseCounter::DeprecatedWebKitLinearGradient);
        return parseDeprecatedLinearGradient(valueList, value, NonRepeating);
    }

    if (val->function->id == CSSValueLinearGradient)
        return parseLinearGradient(valueList, value, NonRepeating);

    if (val->function->id == CSSValueWebkitRepeatingLinearGradient) {
        // FIXME: This should send a deprecation message.
        if (m_context.useCounter())
            m_context.useCounter()->count(UseCounter::DeprecatedWebKitRepeatingLinearGradient);
        return parseDeprecatedLinearGradient(valueList, value, Repeating);
    }

    if (val->function->id == CSSValueRepeatingLinearGradient)
        return parseLinearGradient(valueList, value, Repeating);

    if (val->function->id == CSSValueWebkitRadialGradient) {
        // FIXME: This should send a deprecation message.
        if (m_context.useCounter())
            m_context.useCounter()->count(UseCounter::DeprecatedWebKitRadialGradient);
        return parseDeprecatedRadialGradient(valueList, value, NonRepeating);
    }

    if (val->function->id == CSSValueRadialGradient)
        return parseRadialGradient(valueList, value, NonRepeating);

    if (val->function->id == CSSValueWebkitRepeatingRadialGradient) {
        if (m_context.useCounter())
            m_context.useCounter()->count(UseCounter::DeprecatedWebKitRepeatingRadialGradient);
        return parseDeprecatedRadialGradient(valueList, value, Repeating);
    }

    if (val->function->id == CSSValueRepeatingRadialGradient)
        return parseRadialGradient(valueList, value, Repeating);

    if (val->function->id == CSSValueWebkitCanvas)
        return parseCanvas(valueList, value);

    if (val->function->id == CSSValueWebkitCrossFade)
        return parseCrossfade(valueList, value);

    return false;
}

bool CSSPropertyParser::parseCrossfade(CSSParserValueList* valueList, RefPtrWillBeRawPtr<CSSValue>& crossfade)
{
    // Walk the arguments.
    CSSParserValueList* args = valueList->current()->function->args.get();
    if (!args || args->size() != 5)
        return false;
    RefPtrWillBeRawPtr<CSSValue> fromImageValue = nullptr;
    RefPtrWillBeRawPtr<CSSValue> toImageValue = nullptr;

    // The first argument is the "from" image. It is a fill image.
    if (!args->current() || !parseFillImage(args, fromImageValue))
        return false;
    args->next();

    if (!consumeComma(args))
        return false;

    // The second argument is the "to" image. It is a fill image.
    if (!args->current() || !parseFillImage(args, toImageValue))
        return false;
    args->next();

    if (!consumeComma(args))
        return false;

    // The third argument is the crossfade value. It is a percentage or a fractional number.
    RefPtrWillBeRawPtr<CSSPrimitiveValue> percentage = nullptr;
    CSSParserValue* value = args->current();
    if (!value)
        return false;

    if (value->unit == CSSPrimitiveValue::CSS_PERCENTAGE)
        percentage = cssValuePool().createValue(clampTo<double>(value->fValue / 100, 0, 1), CSSPrimitiveValue::CSS_NUMBER);
    else if (value->unit == CSSPrimitiveValue::CSS_NUMBER)
        percentage = cssValuePool().createValue(clampTo<double>(value->fValue, 0, 1), CSSPrimitiveValue::CSS_NUMBER);
    else
        return false;

    RefPtrWillBeRawPtr<CSSCrossfadeValue> result = CSSCrossfadeValue::create(fromImageValue, toImageValue);
    result->setPercentage(percentage);

    crossfade = result;

    return true;
}

bool CSSPropertyParser::parseCanvas(CSSParserValueList* valueList, RefPtrWillBeRawPtr<CSSValue>& canvas)
{
    // Walk the arguments.
    CSSParserValueList* args = valueList->current()->function->args.get();
    if (!args || args->size() != 1)
        return false;

    // The first argument is the canvas name.  It is an identifier.
    CSSParserValue* value = args->current();
    if (!value || value->unit != CSSPrimitiveValue::CSS_IDENT)
        return false;

    canvas = CSSCanvasValue::create(value->string);
    return true;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseImageSet(CSSParserValueList* valueList)
{
    CSSParserValue* function = valueList->current();

    if (function->unit != CSSParserValue::Function)
        return nullptr;

    CSSParserValueList* functionArgs = valueList->current()->function->args.get();
    if (!functionArgs || !functionArgs->size() || !functionArgs->current())
        return nullptr;

    RefPtrWillBeRawPtr<CSSImageSetValue> imageSet = CSSImageSetValue::create();

    while (functionArgs->current()) {
        CSSParserValue* arg = functionArgs->current();
        if (arg->unit != CSSPrimitiveValue::CSS_URI)
            return nullptr;

        RefPtrWillBeRawPtr<CSSValue> image = createCSSImageValueWithReferrer(arg->string, completeURL(arg->string));
        imageSet->append(image);

        arg = functionArgs->next();
        if (!arg)
            return nullptr;

        if (arg->unit != CSSParserValue::DimensionList)
            return nullptr;
        ASSERT(arg->valueList->valueAt(0)->unit == CSSPrimitiveValue::CSS_NUMBER);
        ASSERT(arg->valueList->valueAt(1)->unit == CSSPrimitiveValue::CSS_IDENT);
        if (String(arg->valueList->valueAt(1)->string) != "x")
            return nullptr;
        double imageScaleFactor = arg->valueList->valueAt(0)->fValue;
        if (imageScaleFactor <= 0)
            return nullptr;
        imageSet->append(cssValuePool().createValue(imageScaleFactor, CSSPrimitiveValue::CSS_NUMBER));
        functionArgs->next();

        // If there are no more arguments, we're done.
        if (!functionArgs->current())
            break;

        // If there are more arguments, they should be after a comma.
        if (!consumeComma(functionArgs))
            return nullptr;
    }

    return imageSet.release();
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseWillChange()
{
    RefPtrWillBeRawPtr<CSSValueList> values = CSSValueList::createCommaSeparated();
    if (m_valueList->current()->id == CSSValueAuto) {
        // FIXME: This will be read back as an empty string instead of auto
        return values.release();
    }

    // Every comma-separated list of CSS_IDENTs is a valid will-change value,
    // unless the list includes an explicitly disallowed CSS_IDENT.
    while (true) {
        CSSParserValue* currentValue = m_valueList->current();
        if (!currentValue || currentValue->unit != CSSPrimitiveValue::CSS_IDENT)
            return nullptr;

        CSSPropertyID unresolvedProperty = unresolvedCSSPropertyID(currentValue->string);
        if (unresolvedProperty) {
            ASSERT(CSSPropertyMetadata::isEnabledProperty(unresolvedProperty));
            // Now "all" is used by both CSSValue and CSSPropertyValue.
            // Need to return nullptr when currentValue is CSSPropertyAll.
            if (unresolvedProperty == CSSPropertyWillChange || unresolvedProperty == CSSPropertyAll)
                return nullptr;
            values->append(cssValuePool().createIdentifierValue(unresolvedProperty));
        } else {
            switch (currentValue->id) {
            case CSSValueNone:
            case CSSValueAll:
            case CSSValueAuto:
            case CSSValueDefault:
            case CSSValueInitial:
            case CSSValueInherit:
                return nullptr;
            case CSSValueContents:
            case CSSValueScrollPosition:
                values->append(cssValuePool().createIdentifierValue(currentValue->id));
                break;
            default:
                break;
            }
        }

        if (!m_valueList->next())
            break;
        if (!consumeComma(m_valueList))
            return nullptr;
    }

    return values.release();
}

PassRefPtrWillBeRawPtr<CSSFunctionValue> CSSPropertyParser::parseBuiltinFilterArguments(CSSParserValueList* args, CSSValueID filterType)
{
    RefPtrWillBeRawPtr<CSSFunctionValue> filterValue = CSSFunctionValue::create(filterType);
    ASSERT(args);

    switch (filterType) {
    case CSSValueGrayscale:
    case CSSValueSepia:
    case CSSValueSaturate:
    case CSSValueInvert:
    case CSSValueOpacity:
    case CSSValueContrast: {
        // One optional argument, 0-1 or 0%-100%, if missing use 100%.
        if (args->size()) {
            CSSParserValue* value = args->current();
            // FIXME (crbug.com/397061): Support calc expressions like calc(10% + 0.5)
            if (value->unit != CSSPrimitiveValue::CSS_PERCENTAGE && !validUnit(value, FNumber | FNonNeg))
                return nullptr;

            double amount = value->fValue;
            if (amount < 0)
                return nullptr;

            // Saturate and Contrast allow values over 100%.
            if (filterType != CSSValueSaturate
                && filterType != CSSValueContrast) {
                double maxAllowed = value->unit == CSSPrimitiveValue::CSS_PERCENTAGE ? 100.0 : 1.0;
                if (amount > maxAllowed)
                    return nullptr;
            }

            filterValue->append(cssValuePool().createValue(amount, static_cast<CSSPrimitiveValue::UnitType>(value->unit)));
        }
        break;
    }
    case CSSValueBrightness: {
        // One optional argument, if missing use 100%.
        if (args->size()) {
            CSSParserValue* value = args->current();
            // FIXME (crbug.com/397061): Support calc expressions like calc(10% + 0.5)
            if (value->unit != CSSPrimitiveValue::CSS_PERCENTAGE && !validUnit(value, FNumber))
                return nullptr;

            filterValue->append(cssValuePool().createValue(value->fValue, static_cast<CSSPrimitiveValue::UnitType>(value->unit)));
        }
        break;
    }
    case CSSValueHueRotate: {
        // hue-rotate() takes one optional angle.
        if (args->size()) {
            CSSParserValue* argument = args->current();
            if (!validUnit(argument, FAngle, HTMLStandardMode))
                return nullptr;

            filterValue->append(createPrimitiveNumericValue(argument));
        }
        break;
    }
    case CSSValueBlur: {
        // Blur takes a single length. Zero parameters are allowed.
        if (args->size()) {
            CSSParserValue* argument = args->current();
            if (!validUnit(argument, FLength | FNonNeg, HTMLStandardMode))
                return nullptr;

            filterValue->append(createPrimitiveNumericValue(argument));
        }
        break;
    }
    case CSSValueDropShadow: {
        // drop-shadow() takes a single shadow.
        RefPtrWillBeRawPtr<CSSValueList> shadowValueList = parseShadow(args, CSSPropertyWebkitFilter);
        if (!shadowValueList || shadowValueList->length() != 1)
            return nullptr;

        filterValue->append((shadowValueList.release())->item(0));
        break;
    }
    default:
        return nullptr;
    }
    return filterValue.release();
}

PassRefPtrWillBeRawPtr<CSSValueList> CSSPropertyParser::parseFilter()
{
    if (!m_valueList)
        return nullptr;

    // The filter is a list of functional primitives that specify individual operations.
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    for (CSSParserValue* value = m_valueList->current(); value; value = m_valueList->next()) {
        if (value->unit != CSSPrimitiveValue::CSS_URI && (value->unit != CSSParserValue::Function || !value->function))
            return nullptr;

        // See if the specified primitive is one we understand.
        if (value->unit == CSSPrimitiveValue::CSS_URI) {
            RefPtrWillBeRawPtr<CSSFunctionValue> referenceFilterValue = CSSFunctionValue::create(CSSValueUrl);
            referenceFilterValue->append(CSSSVGDocumentValue::create(value->string));
            list->append(referenceFilterValue.release());
        } else {
            CSSValueID filterType = value->function->id;
            unsigned maximumArgumentCount = filterType == CSSValueDropShadow ? 4 : 1;

            CSSParserValueList* args = value->function->args.get();
            if (!args || args->size() > maximumArgumentCount)
                return nullptr;

            RefPtrWillBeRawPtr<CSSFunctionValue> filterValue = parseBuiltinFilterArguments(args, filterType);
            if (!filterValue)
                return nullptr;

            list->append(filterValue);
        }
    }

    return list.release();
}

PassRefPtrWillBeRawPtr<CSSValueList> CSSPropertyParser::parseTransformOrigin()
{
    CSSParserValue* value = m_valueList->current();
    CSSValueID id = value->id;
    RefPtrWillBeRawPtr<CSSValue> xValue = nullptr;
    RefPtrWillBeRawPtr<CSSValue> yValue = nullptr;
    RefPtrWillBeRawPtr<CSSValue> zValue = nullptr;
    if (id == CSSValueLeft || id == CSSValueRight) {
        xValue = cssValuePool().createIdentifierValue(id);
    } else if (id == CSSValueTop || id == CSSValueBottom) {
        yValue = cssValuePool().createIdentifierValue(id);
    } else if (id == CSSValueCenter) {
        // Unresolved as to whether this is X or Y.
    } else if (validUnit(value, FPercent | FLength)) {
        xValue = createPrimitiveNumericValue(value);
    } else {
        return nullptr;
    }

    value = m_valueList->next();
    if (value) {
        id = value->id;
        if (!xValue && (id == CSSValueLeft || id == CSSValueRight)) {
            xValue = cssValuePool().createIdentifierValue(id);
        } else if (!yValue && (id == CSSValueTop || id == CSSValueBottom)) {
            yValue = cssValuePool().createIdentifierValue(id);
        } else if (id == CSSValueCenter) {
            // Resolved below.
        } else if (!yValue && validUnit(value, FPercent | FLength)) {
            yValue = createPrimitiveNumericValue(value);
        } else {
            return nullptr;
        }

        // If X or Y have not been resolved, they must be center.
        if (!xValue)
            xValue = cssValuePool().createIdentifierValue(CSSValueCenter);
        if (!yValue)
            yValue = cssValuePool().createIdentifierValue(CSSValueCenter);

        value = m_valueList->next();
        if (value) {
            if (!validUnit(value, FLength))
                return nullptr;
            zValue = createPrimitiveNumericValue(value);

            value = m_valueList->next();
            if (value)
                return nullptr;
        }
    } else if (!xValue) {
        if (yValue) {
            xValue = cssValuePool().createValue(50, CSSPrimitiveValue::CSS_PERCENTAGE);
        } else {
            xValue = cssValuePool().createIdentifierValue(CSSValueCenter);
        }
    }

    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    list->append(xValue.release());
    if (yValue)
        list->append(yValue.release());
    if (zValue)
        list->append(zValue.release());
    return list.release();
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseTouchAction()
{
    CSSParserValue* value = m_valueList->current();
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    if (m_valueList->size() == 1 && value && (value->id == CSSValueAuto || value->id == CSSValueNone || value->id == CSSValueManipulation)) {
        list->append(cssValuePool().createIdentifierValue(value->id));
        m_valueList->next();
        return list.release();
    }

    bool xSet = false;
    bool ySet = false;
    while (value) {
        switch (value->id) {
        case CSSValuePanX:
        case CSSValuePanRight:
        case CSSValuePanLeft: {
            if (xSet)
                return nullptr;
            xSet = true;
            if (value->id != CSSValuePanX && !RuntimeEnabledFeatures::cssTouchActionPanDirectionsEnabled())
                return nullptr;

            RefPtrWillBeRawPtr<CSSValue> panValue = cssValuePool().createIdentifierValue(value->id);
            list->append(panValue.release());
            break;
        }
        case CSSValuePanY:
        case CSSValuePanDown:
        case CSSValuePanUp: {
            if (ySet)
                return nullptr;
            ySet = true;
            if (value->id != CSSValuePanY && !RuntimeEnabledFeatures::cssTouchActionPanDirectionsEnabled())
                return nullptr;
            RefPtrWillBeRawPtr<CSSValue> panValue = cssValuePool().createIdentifierValue(value->id);
            list->append(panValue.release());
            break;
        }
        default:
            return nullptr;
        }
        value = m_valueList->next();
    }

    if (list->length())
        return list.release();

    return nullptr;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseScrollBlocksOn()
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    CSSParserValue* value = m_valueList->current();
    while (value) {
        switch (value->id) {
        case CSSValueStartTouch:
        case CSSValueWheelEvent:
        case CSSValueScrollEvent: {
            RefPtrWillBeRawPtr<CSSValue> flagValue = cssValuePool().createIdentifierValue(value->id);
            if (list->hasValue(flagValue.get()))
                return nullptr;
            list->append(flagValue.release());
            break;
        }
        default:
            return nullptr;
        }
        value = m_valueList->next();
    }

    ASSERT(list->length());
    return list.release();
}

void CSSPropertyParser::addTextDecorationProperty(CSSPropertyID propId, PassRefPtrWillBeRawPtr<CSSValue> value, bool important)
{
    // The text-decoration-line property takes priority over text-decoration, unless the latter has important priority set.
    if (propId == CSSPropertyTextDecoration && !important && !inShorthand()) {
        for (unsigned i = 0; i < m_parsedProperties.size(); ++i) {
            if (m_parsedProperties[i].id() == CSSPropertyTextDecorationLine)
                return;
        }
    }
    addProperty(propId, value, important);
}

bool CSSPropertyParser::parseTextDecoration(CSSPropertyID propId, bool important)
{
    ASSERT(propId != CSSPropertyTextDecorationLine || RuntimeEnabledFeatures::css3TextDecorationsEnabled());

    CSSParserValue* value = m_valueList->current();
    if (value && value->id == CSSValueNone) {
        addTextDecorationProperty(propId, cssValuePool().createIdentifierValue(CSSValueNone), important);
        m_valueList->next();
        return true;
    }

    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    bool isValid = true;
    while (isValid && value) {
        switch (value->id) {
        case CSSValueUnderline:
        case CSSValueOverline:
        case CSSValueLineThrough:
        case CSSValueBlink:
            list->append(cssValuePool().createIdentifierValue(value->id));
            break;
        default:
            isValid = false;
            break;
        }
        if (isValid)
            value = m_valueList->next();
    }

    // Values are either valid or in shorthand scope.
    if (list->length() && (isValid || inShorthand())) {
        addTextDecorationProperty(propId, list.release(), important);
        return true;
    }

    return false;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseTextEmphasisStyle()
{
    RefPtrWillBeRawPtr<CSSPrimitiveValue> fill = nullptr;
    RefPtrWillBeRawPtr<CSSPrimitiveValue> shape = nullptr;

    for (CSSParserValue* value = m_valueList->current(); value; value = m_valueList->next()) {
        if (value->unit == CSSPrimitiveValue::CSS_STRING) {
            if (fill || shape)
                return nullptr;
            m_valueList->next();
            return createPrimitiveStringValue(value);
        }

        if (value->id == CSSValueNone) {
            if (fill || shape)
                return nullptr;
            m_valueList->next();
            return cssValuePool().createIdentifierValue(CSSValueNone);
        }

        if (value->id == CSSValueOpen || value->id == CSSValueFilled) {
            if (fill)
                return nullptr;
            fill = cssValuePool().createIdentifierValue(value->id);
        } else if (value->id == CSSValueDot || value->id == CSSValueCircle || value->id == CSSValueDoubleCircle || value->id == CSSValueTriangle || value->id == CSSValueSesame) {
            if (shape)
                return nullptr;
            shape = cssValuePool().createIdentifierValue(value->id);
        } else {
            break;
        }
    }

    if (fill && shape) {
        RefPtrWillBeRawPtr<CSSValueList> parsedValues = CSSValueList::createSpaceSeparated();
        parsedValues->append(fill.release());
        parsedValues->append(shape.release());
        return parsedValues.release();
    }
    if (fill)
        return fill.release();
    if (shape)
        return shape.release();

    return nullptr;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseTextIndent()
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();

    bool hasLengthOrPercentage = false;
    bool hasEachLine = false;
    bool hasHanging = false;

    for (CSSParserValue* value = m_valueList->current(); value; value = m_valueList->next()) {
        // <length> | <percentage> | inherit when RuntimeEnabledFeatures::css3TextEnabled() returns false
        if (!hasLengthOrPercentage && validUnit(value, FLength | FPercent | FUnitlessQuirk)) {
            list->append(createPrimitiveNumericValue(value));
            hasLengthOrPercentage = true;
            continue;
        }

        // [ <length> | <percentage> ] && hanging? && each-line? | inherit
        // when RuntimeEnabledFeatures::css3TextEnabled() returns true
        if (RuntimeEnabledFeatures::css3TextEnabled()) {
            if (!hasEachLine && value->id == CSSValueEachLine) {
                list->append(cssValuePool().createIdentifierValue(CSSValueEachLine));
                hasEachLine = true;
                continue;
            }
            if (!hasHanging && value->id == CSSValueHanging) {
                list->append(cssValuePool().createIdentifierValue(CSSValueHanging));
                hasHanging = true;
                continue;
            }
        }
        return nullptr;
    }

    if (!hasLengthOrPercentage)
        return nullptr;

    return list.release();
}

PassRefPtrWillBeRawPtr<CSSLineBoxContainValue> CSSPropertyParser::parseLineBoxContain()
{
    LineBoxContain lineBoxContain = LineBoxContainNone;

    for (CSSParserValue* value = m_valueList->current(); value; value = m_valueList->next()) {
        LineBoxContainFlags flag;
        if (value->id == CSSValueBlock) {
            flag = LineBoxContainBlock;
        } else if (value->id == CSSValueInline) {
            flag = LineBoxContainInline;
        } else if (value->id == CSSValueFont) {
            flag = LineBoxContainFont;
        } else if (value->id == CSSValueGlyphs) {
            flag = LineBoxContainGlyphs;
        } else if (value->id == CSSValueReplaced) {
            flag = LineBoxContainReplaced;
        } else if (value->id == CSSValueInlineBox) {
            flag = LineBoxContainInlineBox;
        } else {
            return nullptr;
        }
        if (lineBoxContain & flag)
            return nullptr;
        lineBoxContain |= flag;
    }

    ASSERT(lineBoxContain);
    return CSSLineBoxContainValue::create(lineBoxContain);
}

bool CSSPropertyParser::parseFontFeatureTag(CSSValueList* settings)
{
    // Feature tag name consists of 4-letter characters.
    static const unsigned tagNameLength = 4;

    CSSParserValue* value = m_valueList->current();
    // Feature tag name comes first
    if (value->unit != CSSPrimitiveValue::CSS_STRING)
        return false;
    if (value->string.length() != tagNameLength)
        return false;
    for (unsigned i = 0; i < tagNameLength; ++i) {
        // Limits the range of characters to 0x20-0x7E, following the tag name rules defiend in the OpenType specification.
        UChar character = value->string[i];
        if (character < 0x20 || character > 0x7E)
            return false;
    }

    AtomicString tag = value->string;
    int tagValue = 1;
    // Feature tag values could follow: <integer> | on | off
    value = m_valueList->next();
    if (value) {
        if (value->unit == CSSPrimitiveValue::CSS_NUMBER && value->isInt && value->fValue >= 0) {
            tagValue = clampTo<int>(value->fValue);
            if (tagValue < 0)
                return false;
            m_valueList->next();
        } else if (value->id == CSSValueOn || value->id == CSSValueOff) {
            tagValue = value->id == CSSValueOn;
            m_valueList->next();
        }
    }
    settings->append(CSSFontFeatureValue::create(tag, tagValue));
    return true;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseFontFeatureSettings()
{
    RefPtrWillBeRawPtr<CSSValueList> settings = CSSValueList::createCommaSeparated();
    while (true) {
        if (!m_valueList->current() || !parseFontFeatureTag(settings.get()))
            return nullptr;
        if (!m_valueList->current())
            break;
        if (!consumeComma(m_valueList))
            return nullptr;
    }
    return settings.release();
}

bool CSSPropertyParser::parseFontVariantLigatures(bool important)
{
    RefPtrWillBeRawPtr<CSSValueList> ligatureValues = CSSValueList::createSpaceSeparated();
    bool sawCommonLigaturesValue = false;
    bool sawDiscretionaryLigaturesValue = false;
    bool sawHistoricalLigaturesValue = false;
    bool sawContextualLigaturesValue = false;

    for (CSSParserValue* value = m_valueList->current(); value; value = m_valueList->next()) {
        if (value->unit != CSSPrimitiveValue::CSS_IDENT)
            return false;

        switch (value->id) {
        case CSSValueNoCommonLigatures:
        case CSSValueCommonLigatures:
            if (sawCommonLigaturesValue)
                return false;
            sawCommonLigaturesValue = true;
            ligatureValues->append(cssValuePool().createIdentifierValue(value->id));
            break;
        case CSSValueNoDiscretionaryLigatures:
        case CSSValueDiscretionaryLigatures:
            if (sawDiscretionaryLigaturesValue)
                return false;
            sawDiscretionaryLigaturesValue = true;
            ligatureValues->append(cssValuePool().createIdentifierValue(value->id));
            break;
        case CSSValueNoHistoricalLigatures:
        case CSSValueHistoricalLigatures:
            if (sawHistoricalLigaturesValue)
                return false;
            sawHistoricalLigaturesValue = true;
            ligatureValues->append(cssValuePool().createIdentifierValue(value->id));
            break;
        case CSSValueNoContextual:
        case CSSValueContextual:
            if (sawContextualLigaturesValue)
                return false;
            sawContextualLigaturesValue = true;
            ligatureValues->append(cssValuePool().createIdentifierValue(value->id));
            break;
        default:
            return false;
        }
    }

    if (!ligatureValues->length())
        return false;

    addProperty(CSSPropertyFontVariantLigatures, ligatureValues.release(), important);
    return true;
}

bool CSSPropertyParser::parseCalculation(CSSParserValue* value, ValueRange range)
{
    ASSERT(isCalculation(value));

    CSSParserValueList* args = value->function->args.get();
    if (!args || !args->size())
        return false;

    ASSERT(!m_parsedCalculation);
    m_parsedCalculation = CSSCalcValue::create(args, range);

    if (!m_parsedCalculation)
        return false;

    return true;
}

bool CSSPropertyParser::parseFontFaceDescriptor(CSSPropertyID propId)
{
    CSSParserValue* value = m_valueList->current();
    ASSERT(value);
    CSSValueID id = value->id;
    RefPtrWillBeRawPtr<CSSValue> parsedValue = nullptr;

    switch (propId) {
    case CSSPropertyFontFamily:
        // <family-name>
        // TODO(rwlbuis): check there is only one family-name
        parsedValue = parseFontFamily();
        break;
    case CSSPropertySrc: // This is a list of urls or local references.
        parsedValue = parseFontFaceSrc();
        break;
    case CSSPropertyUnicodeRange:
        parsedValue = parseFontFaceUnicodeRange();
        break;
    case CSSPropertyFontWeight: // normal | bold | 100 | 200 | 300 | 400 | 500 | 600 | 700 | 800 | 900
        if (m_valueList->size() != 1)
            return false;
        return parseFontWeight(false);
    case CSSPropertyFontStretch:
    case CSSPropertyFontStyle:
        if (!CSSParserFastPaths::isValidKeywordPropertyAndValue(propId, id))
            return false;
        addProperty(propId, cssValuePool().createIdentifierValue(id), false);
        return true;
    case CSSPropertyFontVariant: // normal | small-caps | inherit
        return parseFontVariant(false);
    case CSSPropertyWebkitFontFeatureSettings:
        if (id == CSSValueNormal) {
            parsedValue = cssValuePool().createIdentifierValue(CSSValueNormal);
            m_valueList->next();
        } else {
            parsedValue = parseFontFeatureSettings();
        }
        break;
    default:
        break;
    }

    if (!parsedValue || m_valueList->current())
        return false;

    addProperty(propId, parsedValue.release(), false);
    return true;
}

bool CSSPropertyParser::parseViewportProperty(CSSPropertyID propId, bool important)
{
    ASSERT(RuntimeEnabledFeatures::cssViewportEnabled() || isUASheetBehavior(m_context.mode()));

    CSSParserValue* value = m_valueList->current();
    if (!value)
        return false;

    CSSValueID id = value->id;
    bool validPrimitive = false;

    switch (propId) {
    case CSSPropertyMinWidth: // auto | extend-to-zoom | <length> | <percentage>
    case CSSPropertyMaxWidth:
    case CSSPropertyMinHeight:
    case CSSPropertyMaxHeight:
        if (id == CSSValueAuto || id == CSSValueInternalExtendToZoom)
            validPrimitive = true;
        else
            validPrimitive = validUnit(value, FLength | FPercent | FNonNeg);
        break;
    case CSSPropertyWidth: // shorthand
        return parseViewportShorthand(propId, CSSPropertyMinWidth, CSSPropertyMaxWidth, important);
    case CSSPropertyHeight:
        return parseViewportShorthand(propId, CSSPropertyMinHeight, CSSPropertyMaxHeight, important);
    case CSSPropertyMinZoom: // auto | <number> | <percentage>
    case CSSPropertyMaxZoom:
    case CSSPropertyZoom:
        if (id == CSSValueAuto)
            validPrimitive = true;
        else
            validPrimitive = validUnit(value, FNumber | FPercent | FNonNeg);
        break;
    case CSSPropertyUserZoom: // zoom | fixed
        if (id == CSSValueZoom || id == CSSValueFixed)
            validPrimitive = true;
        break;
    case CSSPropertyOrientation: // auto | portrait | landscape
        if (id == CSSValueAuto || id == CSSValuePortrait || id == CSSValueLandscape)
            validPrimitive = true;
    default:
        break;
    }

    RefPtrWillBeRawPtr<CSSValue> parsedValue = nullptr;
    if (validPrimitive) {
        parsedValue = parseValidPrimitive(id, value);
        m_valueList->next();
    }

    if (parsedValue) {
        if (!m_valueList->current() || inShorthand()) {
            addProperty(propId, parsedValue.release(), important);
            return true;
        }
    }

    return false;
}

bool CSSPropertyParser::parseViewportShorthand(CSSPropertyID propId, CSSPropertyID first, CSSPropertyID second, bool important)
{
    ASSERT(RuntimeEnabledFeatures::cssViewportEnabled() || isUASheetBehavior(m_context.mode()));
    unsigned numValues = m_valueList->size();

    if (numValues > 2)
        return false;

    ShorthandScope scope(this, propId);

    if (!parseViewportProperty(first, important))
        return false;

    // If just one value is supplied, the second value
    // is implicitly initialized with the first value.
    if (numValues == 1)
        m_valueList->previous();

    return parseViewportProperty(second, important);
}

template <typename CharacterType>
static CSSPropertyID unresolvedCSSPropertyID(const CharacterType* propertyName, unsigned length)
{
    if (length == 0)
        return CSSPropertyInvalid;
//     if (length >= 2 && propertyName[0] == '-' && propertyName[1] == '-')
//         return CSSPropertyVariable;
    if (length > maxCSSPropertyNameLength)
        return CSSPropertyInvalid;

    char buffer[maxCSSPropertyNameLength + 1]; // 1 for null character

    for (unsigned i = 0; i != length; ++i) {
        CharacterType c = propertyName[i];
        if (c == 0 || c >= 0x7F)
            return CSSPropertyInvalid; // illegal character
        buffer[i] = toASCIILower(c);
    }
    buffer[length] = '\0';

    const char* name = buffer;
    const Property* hashTableEntry = findProperty(name, length);
    if (!hashTableEntry)
        return CSSPropertyInvalid;
    CSSPropertyID property = static_cast<CSSPropertyID>(hashTableEntry->id);
    if (!CSSPropertyMetadata::isEnabledProperty(property))
        return CSSPropertyInvalid;
    return property;
}

CSSPropertyID unresolvedCSSPropertyID(const String& string)
{
    unsigned length = string.length();

    if (!length)
        return CSSPropertyInvalid;
    if (length > maxCSSPropertyNameLength)
        return CSSPropertyInvalid;

    return string.is8Bit() ? unresolvedCSSPropertyID(string.characters8(), length) : unresolvedCSSPropertyID(string.characters16(), length);
}

CSSPropertyID unresolvedCSSPropertyID(const CSSParserString& string)
{
    unsigned length = string.length();

    if (!length)
        return CSSPropertyInvalid;
    if (length > maxCSSPropertyNameLength)
        return CSSPropertyInvalid;

    return string.is8Bit() ? unresolvedCSSPropertyID(string.characters8(), length) : unresolvedCSSPropertyID(string.characters16(), length);
}

template <typename CharacterType>
static CSSValueID cssValueKeywordID(const CharacterType* valueKeyword, unsigned length)
{
    char buffer[maxCSSValueKeywordLength + 1]; // 1 for null character

    for (unsigned i = 0; i != length; ++i) {
        CharacterType c = valueKeyword[i];
        if (c == 0 || c >= 0x7F)
            return CSSValueInvalid; // illegal character
        buffer[i] = WTF::toASCIILower(c);
    }
    buffer[length] = '\0';

    const Value* hashTableEntry = findValue(buffer, length);
    return hashTableEntry ? static_cast<CSSValueID>(hashTableEntry->id) : CSSValueInvalid;
}

CSSValueID cssValueKeywordID(const CSSParserString& string)
{
    unsigned length = string.length();
    if (!length)
        return CSSValueInvalid;
    if (length > maxCSSValueKeywordLength)
        return CSSValueInvalid;

    return string.is8Bit() ? cssValueKeywordID(string.characters8(), length) : cssValueKeywordID(string.characters16(), length);
}

bool CSSPropertyParser::isSystemColor(CSSValueID id)
{
    return (id >= CSSValueActiveborder && id <= CSSValueWindowtext) || id == CSSValueMenu;
}

bool CSSPropertyParser::parseSVGValue(CSSPropertyID propId, bool important)
{
    CSSParserValue* value = m_valueList->current();
    ASSERT(value);

    CSSValueID id = value->id;

    bool validPrimitive = false;
    RefPtrWillBeRawPtr<CSSValue> parsedValue = nullptr;

    switch (propId) {
    /* The comment to the right defines all valid value of these
     * properties as defined in SVG 1.1, Appendix N. Property index */
    case CSSPropertyAlignmentBaseline:
    // auto | baseline | before-edge | text-before-edge | middle |
    // central | after-edge | text-after-edge | ideographic | alphabetic |
    // hanging | mathematical | inherit
        if (id == CSSValueAuto || id == CSSValueBaseline || id == CSSValueMiddle
            || (id >= CSSValueBeforeEdge && id <= CSSValueMathematical))
            validPrimitive = true;
        break;

    case CSSPropertyBaselineShift:
    // baseline | super | sub | <percentage> | <length> | inherit
        if (id == CSSValueBaseline || id == CSSValueSub || id == CSSValueSuper)
            validPrimitive = true;
        else
            validPrimitive = validUnit(value, FLength | FPercent, SVGAttributeMode);
        break;

    case CSSPropertyDominantBaseline:
    // auto | use-script | no-change | reset-size | ideographic |
    // alphabetic | hanging | mathematical | central | middle |
    // text-after-edge | text-before-edge | inherit
        if (id == CSSValueAuto || id == CSSValueMiddle
            || (id >= CSSValueUseScript && id <= CSSValueResetSize)
            || (id >= CSSValueCentral && id <= CSSValueMathematical))
            validPrimitive = true;
        break;

//     case CSSPropertyEnableBackground:
//     // accumulate | new [x] [y] [width] [height] | inherit
//         if (id == CSSValueAccumulate) // TODO : new
//             validPrimitive = true;
//         break;

    case CSSPropertyClipPath:
    case CSSPropertyFilter:
    case CSSPropertyMarkerStart:
    case CSSPropertyMarkerMid:
    case CSSPropertyMarkerEnd:
    case CSSPropertyMask:
        if (id == CSSValueNone) {
            validPrimitive = true;
        } else if (value->unit == CSSPrimitiveValue::CSS_URI) {
            parsedValue = CSSPrimitiveValue::create(value->string, CSSPrimitiveValue::CSS_URI);
            if (parsedValue)
                m_valueList->next();
        }
        break;

    case CSSPropertyClipRule: // nonzero | evenodd | inherit
    case CSSPropertyFillRule:
        if (id == CSSValueNonzero || id == CSSValueEvenodd)
            validPrimitive = true;
        break;

    case CSSPropertyStrokeMiterlimit: // <miterlimit> | inherit
        validPrimitive = validUnit(value, FNumber | FNonNeg, SVGAttributeMode);
        break;

    case CSSPropertyStrokeLinejoin: // miter | round | bevel | inherit
        if (id == CSSValueMiter || id == CSSValueRound || id == CSSValueBevel)
            validPrimitive = true;
        break;

    case CSSPropertyStrokeLinecap: // butt | round | square | inherit
        if (id == CSSValueButt || id == CSSValueRound || id == CSSValueSquare)
            validPrimitive = true;
        break;

    case CSSPropertyStrokeOpacity: // <opacity-value> | inherit
    case CSSPropertyFillOpacity:
    case CSSPropertyStopOpacity:
    case CSSPropertyFloodOpacity:
        validPrimitive = validUnit(value, FNumber | FPercent, SVGAttributeMode);
        break;

    case CSSPropertyShapeRendering:
    // auto | optimizeSpeed | crispEdges | geometricPrecision | inherit
        if (id == CSSValueAuto || id == CSSValueOptimizeSpeed
            || id == CSSValueCrispEdges || id == CSSValueGeometricPrecision)
            validPrimitive = true;
        break;

    case CSSPropertyColorRendering: // optimizeQuality | inherit
        if (id == CSSValueAuto || id == CSSValueOptimizeSpeed
            || id == CSSValueOptimizeQuality)
            validPrimitive = true;
        break;

    case CSSPropertyBufferedRendering: // auto | dynamic | static
        if (id == CSSValueAuto || id == CSSValueDynamic || id == CSSValueStatic)
            validPrimitive = true;
        break;

    case CSSPropertyColorInterpolation: // auto | sRGB | linearRGB | inherit
    case CSSPropertyColorInterpolationFilters:
        if (id == CSSValueAuto || id == CSSValueSRGB || id == CSSValueLinearRGB)
            validPrimitive = true;
        break;

    /* Start of supported CSS properties with validation. This is needed for parseShortHand to work
     * correctly and allows optimization in applyRule(..)
     */

    case CSSPropertyTextAnchor: // start | middle | end | inherit
        if (id == CSSValueStart || id == CSSValueMiddle || id == CSSValueEnd)
            validPrimitive = true;
        break;

//     case CSSPropertyGlyphOrientationVertical: // auto | <angle> | inherit
//         if (id == CSSValueAuto) {
//             validPrimitive = true;
//             break;
//         }
    /* fallthrough intentional */
//     case CSSPropertyGlyphOrientationHorizontal: // <angle> (restricted to _deg_ per SVG 1.1 spec) | inherit
//         if (value->unit == CSSPrimitiveValue::CSS_DEG || value->unit == CSSPrimitiveValue::CSS_NUMBER) {
//             parsedValue = CSSPrimitiveValue::create(value->fValue, CSSPrimitiveValue::CSS_DEG);
// 
//             if (parsedValue)
//                 m_valueList->next();
//         }
//         break;

    case CSSPropertyFill: // <paint> | inherit
    case CSSPropertyStroke: // <paint> | inherit
        {
            if (id == CSSValueNone) {
                parsedValue = cssValuePool().createIdentifierValue(id);
            } else if (value->unit == CSSPrimitiveValue::CSS_URI) {
                if (m_valueList->next()) {
                    RefPtrWillBeRawPtr<CSSValueList> values = CSSValueList::createSpaceSeparated();
                    values->append(CSSPrimitiveValue::create(value->string, CSSPrimitiveValue::CSS_URI));
                    if (m_valueList->current()->id == CSSValueNone)
                        parsedValue = cssValuePool().createIdentifierValue(m_valueList->current()->id);
                    else
                        parsedValue = parseColor(m_valueList->current());
                    if (parsedValue) {
                        values->append(parsedValue);
                        parsedValue = values;
                    }
                }
                if (!parsedValue)
                    parsedValue = CSSPrimitiveValue::create(value->string, CSSPrimitiveValue::CSS_URI);
            } else {
                parsedValue = parseColor(m_valueList->current());
            }

            if (parsedValue)
                m_valueList->next();
        }
        break;

    case CSSPropertyStopColor: // TODO : icccolor
    case CSSPropertyFloodColor:
    case CSSPropertyLightingColor:
        parsedValue = parseColor(m_valueList->current());
        if (parsedValue)
            m_valueList->next();

        break;

    case CSSPropertyPaintOrder:
        if (m_valueList->size() == 1 && id == CSSValueNormal)
            validPrimitive = true;
        else if ((parsedValue = parsePaintOrder()))
            m_valueList->next();
        break;

    case CSSPropertyVectorEffect: // none | non-scaling-stroke | inherit
        if (id == CSSValueNone || id == CSSValueNonScalingStroke)
            validPrimitive = true;
        break;

    case CSSPropertyWritingMode:
    // lr-tb | rl_tb | tb-rl | lr | rl | tb | inherit
        if (id == CSSValueLrTb || id == CSSValueRlTb || id == CSSValueTbRl || id == CSSValueLr || id == CSSValueRl || id == CSSValueTb)
            validPrimitive = true;
        break;

    case CSSPropertyStrokeWidth: // <length> | inherit
    case CSSPropertyStrokeDashoffset:
    case CSSPropertyCx:
    case CSSPropertyCy:
    case CSSPropertyX:
    case CSSPropertyY:
    case CSSPropertyR:
    case CSSPropertyRx:
    case CSSPropertyRy:
        validPrimitive = validUnit(value, FLength | FPercent, SVGAttributeMode);
        break;
    case CSSPropertyStrokeDasharray: // none | <dasharray> | inherit
        if (id == CSSValueNone)
            validPrimitive = true;
        else
            parsedValue = parseSVGStrokeDasharray();
        break;

    case CSSPropertyMaskType: // luminance | alpha | inherit
        if (id == CSSValueLuminance || id == CSSValueAlpha)
            validPrimitive = true;
        break;

    /* shorthand properties */
    case CSSPropertyMarker: {
        ShorthandScope scope(this, propId);
        CSSPropertyParser::ImplicitScope implicitScope(this);
        if (!parseValue(CSSPropertyMarkerStart, important))
            return false;
        if (m_valueList->current()) {
            rollbackLastProperties(1);
            return false;
        }
        CSSValue* value = m_parsedProperties.last().value();
        addProperty(CSSPropertyMarkerMid, value, important);
        addProperty(CSSPropertyMarkerEnd, value, important);
        return true;
    }
    default:
        // If you crash here, it's because you added a css property and are not handling it
        // in either this switch statement or the one in CSSPropertyParser::parseValue
        ASSERT_WITH_MESSAGE(0, "unimplemented propertyID: %d", propId);
        return false;
    }

    if (validPrimitive) {
        if (id)
            parsedValue = CSSPrimitiveValue::createIdentifier(id);
        else if (value->unit == CSSPrimitiveValue::CSS_STRING)
            parsedValue = CSSPrimitiveValue::create(value->string, (CSSPrimitiveValue::UnitType) value->unit);
        else if (value->unit >= CSSPrimitiveValue::CSS_NUMBER && value->unit <= CSSPrimitiveValue::CSS_KHZ)
            parsedValue = CSSPrimitiveValue::create(value->fValue, (CSSPrimitiveValue::UnitType) value->unit);
        else if (value->unit == CSSPrimitiveValue::CSS_REMS || value->unit == CSSPrimitiveValue::CSS_CHS)
            parsedValue = CSSPrimitiveValue::create(value->fValue, (CSSPrimitiveValue::UnitType)value->unit);
        else if (value->unit == CSSPrimitiveValue::CSS_QEM)
            parsedValue = CSSPrimitiveValue::createAllowingMarginQuirk(value->fValue, CSSPrimitiveValue::CSS_EMS);
        if (isCalculation(value)) {
            // FIXME calc() http://webkit.org/b/16662 : actually create a CSSPrimitiveValue here, ie
            // parsedValue = CSSPrimitiveValue::create(m_parsedCalculation.release());
            m_parsedCalculation.release();
            parsedValue = nullptr;
        }
        m_valueList->next();
    }
    if (!parsedValue || (m_valueList->current() && !inShorthand()))
        return false;

    addProperty(propId, parsedValue.release(), important);
    return true;
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseSVGStrokeDasharray()
{
    RefPtrWillBeRawPtr<CSSValueList> ret = CSSValueList::createCommaSeparated();
    CSSParserValue* value = m_valueList->current();
    bool validPrimitive = true;
    while (value) {
        validPrimitive = validUnit(value, FLength | FPercent | FNonNeg, SVGAttributeMode);
        if (!validPrimitive)
            break;
        if (value->id)
            ret->append(CSSPrimitiveValue::createIdentifier(value->id));
        else if (value->unit >= CSSPrimitiveValue::CSS_NUMBER && value->unit <= CSSPrimitiveValue::CSS_KHZ)
            ret->append(CSSPrimitiveValue::create(value->fValue, (CSSPrimitiveValue::UnitType) value->unit));
        else if (value->unit == CSSPrimitiveValue::CSS_REMS || value->unit == CSSPrimitiveValue::CSS_CHS)
            ret->append(CSSPrimitiveValue::create(value->fValue, (CSSPrimitiveValue::UnitType)value->unit));
        value = m_valueList->next();
        bool commaConsumed = consumeComma(m_valueList);
        value = m_valueList->current();
        if (commaConsumed && !value)
            return nullptr;
    }
    if (!validPrimitive)
        return nullptr;
    return ret.release();
}

// normal | [ fill || stroke || markers ]
PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parsePaintOrder() const
{
    if (m_valueList->size() > 3)
        return nullptr;

    CSSParserValue* value = m_valueList->current();
    ASSERT(value);

    RefPtrWillBeRawPtr<CSSValueList> parsedValues = CSSValueList::createSpaceSeparated();

    // The default paint-order is: Fill, Stroke, Markers.
    bool seenFill = false, seenStroke = false, seenMarkers = false;

    for (; value; value = m_valueList->next()) {
        switch (value->id) {
        case CSSValueNormal:
            // normal inside [fill || stroke || markers] not valid
            return nullptr;
        case CSSValueFill:
            if (seenFill)
                return nullptr;

            seenFill = true;
            break;
        case CSSValueStroke:
            if (seenStroke)
                return nullptr;

            seenStroke = true;
            break;
        case CSSValueMarkers:
            if (seenMarkers)
                return nullptr;

            seenMarkers = true;
            break;
        default:
            return nullptr;
        }

        parsedValues->append(CSSPrimitiveValue::createIdentifier(value->id));
    }

    // fill out the rest of the paint order
    if (!seenFill)
        parsedValues->append(CSSPrimitiveValue::createIdentifier(CSSValueFill));
    if (!seenStroke)
        parsedValues->append(CSSPrimitiveValue::createIdentifier(CSSValueStroke));
    if (!seenMarkers)
        parsedValues->append(CSSPrimitiveValue::createIdentifier(CSSValueMarkers));

    return parsedValues.release();
}

class TransformOperationInfo {
public:
    TransformOperationInfo(CSSValueID id)
        : m_validID(true)
        , m_allowSingleArgument(false)
        , m_argCount(1)
        , m_unit(CSSPropertyParser::FUnknown)
    {
        switch (id) {
        case CSSValueSkew:
            m_unit = CSSPropertyParser::FAngle;
            m_allowSingleArgument = true;
            m_argCount = 3;
            break;
        case CSSValueScale:
            m_unit = CSSPropertyParser::FNumber;
            m_allowSingleArgument = true;
            m_argCount = 3;
            break;
        case CSSValueSkewX:
            m_unit = CSSPropertyParser::FAngle;
            break;
        case CSSValueSkewY:
            m_unit = CSSPropertyParser::FAngle;
            break;
        case CSSValueMatrix:
            m_unit = CSSPropertyParser::FNumber;
            m_argCount = 11;
            break;
        case CSSValueRotate:
            m_unit = CSSPropertyParser::FAngle;
            break;
        case CSSValueScaleX:
            m_unit = CSSPropertyParser::FNumber;
            break;
        case CSSValueScaleY:
            m_unit = CSSPropertyParser::FNumber;
            break;
        case CSSValueScaleZ:
            m_unit = CSSPropertyParser::FNumber;
            break;
        case CSSValueScale3d:
            m_unit = CSSPropertyParser::FNumber;
            m_argCount = 5;
            break;
        case CSSValueRotateX:
            m_unit = CSSPropertyParser::FAngle;
            break;
        case CSSValueRotateY:
            m_unit = CSSPropertyParser::FAngle;
            break;
        case CSSValueRotateZ:
            m_unit = CSSPropertyParser::FAngle;
            break;
        case CSSValueMatrix3d:
            m_unit = CSSPropertyParser::FNumber;
            m_argCount = 31;
            break;
        case CSSValueRotate3d:
            m_unit = CSSPropertyParser::FNumber;
            m_argCount = 7;
            break;
        case CSSValueTranslate:
            m_unit = CSSPropertyParser::FLength | CSSPropertyParser::FPercent;
            m_allowSingleArgument = true;
            m_argCount = 3;
            break;
        case CSSValueTranslateX:
            m_unit = CSSPropertyParser::FLength | CSSPropertyParser::FPercent;
            break;
        case CSSValueTranslateY:
            m_unit = CSSPropertyParser::FLength | CSSPropertyParser::FPercent;
            break;
        case CSSValueTranslateZ:
            m_unit = CSSPropertyParser::FLength | CSSPropertyParser::FPercent;
            break;
        case CSSValuePerspective:
            m_unit = CSSPropertyParser::FNumber;
            break;
        case CSSValueTranslate3d:
            m_unit = CSSPropertyParser::FLength | CSSPropertyParser::FPercent;
            m_argCount = 5;
            break;
        default:
            m_validID = false;
            break;
        }
    }

    bool validID() const { return m_validID; }
    unsigned argCount() const { return m_argCount; }
    CSSPropertyParser::Units unit() const { return m_unit; }

    bool hasCorrectArgCount(unsigned argCount) { return m_argCount == argCount || (m_allowSingleArgument && argCount == 1); }

private:
    bool m_validID;
    bool m_allowSingleArgument;
    unsigned m_argCount;
    CSSPropertyParser::Units m_unit;
};

PassRefPtrWillBeRawPtr<CSSValueList> CSSPropertyParser::parseTransform(bool useLegacyParsing)
{
    if (!m_valueList)
        return nullptr;

    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    for (CSSParserValue* value = m_valueList->current(); value; value = m_valueList->next()) {
        RefPtrWillBeRawPtr<CSSValue> parsedTransformValue = parseTransformValue(useLegacyParsing, value);
        if (!parsedTransformValue)
            return nullptr;

        list->append(parsedTransformValue.release());
    }

    return list.release();
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseTransformValue(bool useLegacyParsing, CSSParserValue *value)
{
    if (value->unit != CSSParserValue::Function || !value->function)
        return nullptr;

    // Every primitive requires at least one argument.
    CSSParserValueList* args = value->function->args.get();
    if (!args)
        return nullptr;

    // See if the specified primitive is one we understand.
    CSSValueID type = value->function->id;
    TransformOperationInfo info(type);
    if (!info.validID())
        return nullptr;

    if (!info.hasCorrectArgCount(args->size()))
        return nullptr;

    // The transform is a list of functional primitives that specify transform operations.
    // We collect a list of CSSFunctionValues, where each value specifies a single operation.

    // Create the new CSSFunctionValue for this operation and add it to our list.
    RefPtrWillBeRawPtr<CSSFunctionValue> transformValue = CSSFunctionValue::create(type);

    // Snag our values.
    CSSParserValue* a = args->current();
    unsigned argNumber = 0;
    while (a) {
        CSSPropertyParser::Units unit = info.unit();

        if (type == CSSValueRotate3d && argNumber == 3) {
            // 4th param of rotate3d() is an angle rather than a bare number, validate it as such
            if (!validUnit(a, FAngle, HTMLStandardMode))
                return nullptr;
        } else if (type == CSSValueTranslate3d && argNumber == 2) {
            // 3rd param of translate3d() cannot be a percentage
            if (!validUnit(a, FLength, HTMLStandardMode))
                return nullptr;
        } else if (type == CSSValueTranslateZ && !argNumber) {
            // 1st param of translateZ() cannot be a percentage
            if (!validUnit(a, FLength, HTMLStandardMode))
                return nullptr;
        } else if (type == CSSValuePerspective && !argNumber) {
            // 1st param of perspective() must be a non-negative number (deprecated) or length.
            if (!validUnit(a, FLength | FNonNeg, HTMLStandardMode)) {
                if (useLegacyParsing && validUnit(a, FNumber | FNonNeg, HTMLStandardMode)) {
                    a->unit = CSSPrimitiveValue::CSS_PX;
                } else {
                    return nullptr;
                }
            }
        } else if (!validUnit(a, unit, HTMLStandardMode)) {
            return nullptr;
        }

        // Add the value to the current transform operation.
        transformValue->append(createPrimitiveNumericValue(a));

        a = args->next();
        if (!a)
            break;
        if (!isComma(a))
            return nullptr;
        a = args->next();

        argNumber++;
    }

    return transformValue.release();
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseMotionPath()
{
    CSSParserValue* value = m_valueList->current();

    // FIXME: Add support for <url>, <basic-shape>, <geometry-box>.
    if (value->unit != CSSParserValue::Function || value->function->id != CSSValuePath)
        return nullptr;

    // FIXME: Add support for <fill-rule>.
    CSSParserValueList* functionArgs = value->function->args.get();
    if (!functionArgs || functionArgs->size() != 1 || !functionArgs->current())
        return nullptr;

    CSSParserValue* arg = functionArgs->current();
    if (arg->unit != CSSPrimitiveValue::CSS_STRING)
        return nullptr;

    String pathString = arg->string;
    Path path;
    if (!buildPathFromString(pathString, path))
        return nullptr;

    m_valueList->next();
    return CSSPathValue::create(pathString);
}

PassRefPtrWillBeRawPtr<CSSValue> CSSPropertyParser::parseMotionRotation()
{
    RefPtrWillBeRawPtr<CSSValueList> list = CSSValueList::createSpaceSeparated();
    bool hasAutoOrReverse = false;
    bool hasAngle = false;

    for (CSSParserValue* value = m_valueList->current(); value; value = m_valueList->next()) {
        if ((value->id == CSSValueAuto || value->id == CSSValueReverse) && !hasAutoOrReverse) {
            list->append(cssValuePool().createIdentifierValue(value->id));
            hasAutoOrReverse = true;
        } else if (validUnit(value, FAngle) && !hasAngle) {
            list->append(createPrimitiveNumericValue(value));
            hasAngle = true;
        } else {
            break;
        }
    }

    if (!list->length())
        return nullptr;

    return list.release();
}

} // namespace blink
