/*
 * Copyright (C) 2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2008, 2009, 2010 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2009 - 2010  Torch Mobile (Beijing) Co. Ltd. All rights reserved.
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

#ifndef CSSPropertyParser_h
#define CSSPropertyParser_h

#include "core/css/CSSGradientValue.h"
#include "core/css/CSSGridTemplateAreasValue.h"
#include "core/css/CSSPropertySourceData.h"
#include "platform/Length.h"

namespace blink {

class BorderImageParseContext;
class CSSBorderImageSliceValue;
class CSSBasicShape;
class CSSBasicShapeInset;
class CSSFunctionValue;
class CSSGradientValue;
class CSSGridLineNamesValue;
class CSSLineBoxContainValue;
struct CSSParserString;
struct CSSParserValue;
class CSSParserValueList;
class CSSPrimitiveValue;
class CSSProperty;
class CSSValue;
class CSSValueList;
class StylePropertyShorthand;

// Inputs: PropertyID, isImportant bool, CSSParserValueList.
// Outputs: Vector of CSSProperties

class CSSPropertyParser {
    STACK_ALLOCATED();
public:

    enum Units {
        FUnknown = 0x0000,
        FInteger = 0x0001,
        FNumber = 0x0002, // Real Numbers
        FPercent = 0x0004,
        FLength = 0x0008,
        FAngle = 0x0010,
        FTime = 0x0020,
        FFrequency = 0x0040,
        FPositiveInteger = 0x0080,
        FRelative = 0x0100,
        FResolution = 0x0200,
        FNonNeg = 0x0400,
        FUnitlessQuirk = 0x0800
    };

    static bool parseValue(CSSPropertyID, bool important,
        CSSParserValueList*, const CSSParserContext&,
        WillBeHeapVector<CSSProperty, 256>&, StyleRule::Type);

    static bool isSystemColor(CSSValueID);
    static bool isColorKeyword(CSSValueID);

private:
    CSSPropertyParser(CSSParserValueList*, const CSSParserContext&,
        WillBeHeapVector<CSSProperty, 256>&, StyleRule::Type);

    bool parseValue(CSSPropertyID, bool important);

    bool inShorthand() const { return m_inParseShorthand; }
    bool inQuirksMode() const { return isQuirksModeBehavior(m_context.mode()); }

    bool parseViewportProperty(CSSPropertyID propId, bool important);
    bool parseViewportShorthand(CSSPropertyID propId, CSSPropertyID first, CSSPropertyID second, bool important);
    bool parseFontFaceDescriptor(CSSPropertyID);

    KURL completeURL(const String& url) const;

    void addProperty(CSSPropertyID, PassRefPtrWillBeRawPtr<CSSValue>, bool important, bool implicit = false);
    void rollbackLastProperties(int num);
    void addExpandedPropertyForValue(CSSPropertyID propId, PassRefPtrWillBeRawPtr<CSSValue>, bool);

    PassRefPtrWillBeRawPtr<CSSPrimitiveValue> parseValidPrimitive(CSSValueID ident, CSSParserValue*);

    bool parseShorthand(CSSPropertyID, const StylePropertyShorthand&, bool important);
    bool parse4Values(CSSPropertyID, const CSSPropertyID* properties, bool important);
    PassRefPtrWillBeRawPtr<CSSValueList> parseContent();
    PassRefPtrWillBeRawPtr<CSSValue> parseQuotes();

    PassRefPtrWillBeRawPtr<CSSValue> parseAttr(CSSParserValueList* args);

    bool parseFillImage(CSSParserValueList*, RefPtrWillBeRawPtr<CSSValue>&);

    enum FillPositionFlag { InvalidFillPosition = 0, AmbiguousFillPosition = 1, XFillPosition = 2, YFillPosition = 4 };
    enum FillPositionParsingMode { ResolveValuesAsPercent = 0, ResolveValuesAsKeyword = 1 };
    PassRefPtrWillBeRawPtr<CSSPrimitiveValue> parseFillPositionComponent(CSSParserValueList*, unsigned& cumulativeFlags, FillPositionFlag& individualFlag, FillPositionParsingMode = ResolveValuesAsPercent, Units = FUnknown);
    PassRefPtrWillBeRawPtr<CSSValue> parseFillPositionX(CSSParserValueList*);
    PassRefPtrWillBeRawPtr<CSSValue> parseFillPositionY(CSSParserValueList*);
    void parse2ValuesFillPosition(CSSParserValueList*, RefPtrWillBeRawPtr<CSSValue>&, RefPtrWillBeRawPtr<CSSValue>&, Units = FUnknown);
    bool isPotentialPositionValue(CSSParserValue*);
    void parseFillPosition(CSSParserValueList*, RefPtrWillBeRawPtr<CSSValue>&, RefPtrWillBeRawPtr<CSSValue>&, Units = FUnknown);
    void parse3ValuesFillPosition(CSSParserValueList*, RefPtrWillBeRawPtr<CSSValue>&, RefPtrWillBeRawPtr<CSSValue>&, PassRefPtrWillBeRawPtr<CSSPrimitiveValue>, PassRefPtrWillBeRawPtr<CSSPrimitiveValue>);
    void parse4ValuesFillPosition(CSSParserValueList*, RefPtrWillBeRawPtr<CSSValue>&, RefPtrWillBeRawPtr<CSSValue>&, PassRefPtrWillBeRawPtr<CSSPrimitiveValue>, PassRefPtrWillBeRawPtr<CSSPrimitiveValue>);

    void parseFillRepeat(RefPtrWillBeRawPtr<CSSValue>&, RefPtrWillBeRawPtr<CSSValue>&);
    PassRefPtrWillBeRawPtr<CSSValue> parseFillSize(CSSPropertyID);

    bool parseFillProperty(CSSPropertyID propId, CSSPropertyID& propId1, CSSPropertyID& propId2, RefPtrWillBeRawPtr<CSSValue>&, RefPtrWillBeRawPtr<CSSValue>&);
    bool parseFillShorthand(CSSPropertyID, const CSSPropertyID* properties, int numProperties, bool important);

    void addFillValue(RefPtrWillBeRawPtr<CSSValue>& lval, PassRefPtrWillBeRawPtr<CSSValue> rval);

    PassRefPtrWillBeRawPtr<CSSValue> parseAnimationDelay();
    PassRefPtrWillBeRawPtr<CSSValue> parseAnimationDirection();
    PassRefPtrWillBeRawPtr<CSSValue> parseAnimationDuration();
    PassRefPtrWillBeRawPtr<CSSValue> parseAnimationFillMode();
    PassRefPtrWillBeRawPtr<CSSValue> parseAnimationIterationCount();
    PassRefPtrWillBeRawPtr<CSSValue> parseAnimationName(bool allowQuotedName);
    PassRefPtrWillBeRawPtr<CSSValue> parseAnimationPlayState();
    PassRefPtrWillBeRawPtr<CSSValue> parseAnimationProperty();
    PassRefPtrWillBeRawPtr<CSSValue> parseAnimationTimingFunction();

    bool parseCubicBezierTimingFunctionValue(CSSParserValueList*& args, double& result);

    // Legacy parsing allows <string>s for animation-name
    PassRefPtrWillBeRawPtr<CSSValue> parseAnimationProperty(CSSPropertyID, bool useLegacyParsing);
    PassRefPtrWillBeRawPtr<CSSValueList> parseAnimationPropertyList(CSSPropertyID, bool useLegacyParsing);
    bool parseAnimationShorthand(bool useLegacyParsing, bool important);
    bool parseTransitionShorthand(bool important);

    PassRefPtrWillBeRawPtr<CSSValue> parseColumnWidth();
    PassRefPtrWillBeRawPtr<CSSValue> parseColumnCount();
    bool parseColumnsShorthand(bool important);

    PassRefPtrWillBeRawPtr<CSSValue> parseGridPosition();
    bool parseIntegerOrCustomIdentFromGridPosition(RefPtrWillBeRawPtr<CSSPrimitiveValue>& numericValue, RefPtrWillBeRawPtr<CSSPrimitiveValue>& gridLineName);
    bool parseGridItemPositionShorthand(CSSPropertyID, bool important);
    bool parseGridTemplateRowsAndAreas(PassRefPtrWillBeRawPtr<CSSValue>, bool important);
    bool parseGridTemplateShorthand(bool important);
    bool parseGridShorthand(bool important);
    bool parseGridAreaShorthand(bool important);
    bool parseSingleGridAreaLonghand(RefPtrWillBeRawPtr<CSSValue>&);
    PassRefPtrWillBeRawPtr<CSSValue> parseGridTrackList();
    bool parseGridTrackRepeatFunction(CSSValueList&);
    PassRefPtrWillBeRawPtr<CSSValue> parseGridTrackSize(CSSParserValueList& inputList);
    PassRefPtrWillBeRawPtr<CSSPrimitiveValue> parseGridBreadth(CSSParserValue*);
    bool parseGridTemplateAreasRow(NamedGridAreaMap&, const size_t, size_t&);
    PassRefPtrWillBeRawPtr<CSSValue> parseGridTemplateAreas();
    bool parseGridLineNames(CSSParserValueList&, CSSValueList&, CSSGridLineNamesValue* = nullptr);
    PassRefPtrWillBeRawPtr<CSSValue> parseGridAutoFlow(CSSParserValueList&);

    PassRefPtrWillBeRawPtr<CSSPrimitiveValue> parseClipShape();

    bool parseLegacyPosition(CSSPropertyID, bool important);
    bool parseItemPositionOverflowPosition(CSSPropertyID, bool important);
    PassRefPtrWillBeRawPtr<CSSValue> parseContentDistributionOverflowPosition();

    PassRefPtrWillBeRawPtr<CSSValue> parseShapeProperty(CSSPropertyID propId);
    PassRefPtrWillBeRawPtr<CSSValue> parseBasicShapeAndOrBox();
    PassRefPtrWillBeRawPtr<CSSPrimitiveValue> parseBasicShape();
    PassRefPtrWillBeRawPtr<CSSPrimitiveValue> parseShapeRadius(CSSParserValue*);

    PassRefPtrWillBeRawPtr<CSSBasicShape> parseBasicShapeCircle(CSSParserValueList* args);
    PassRefPtrWillBeRawPtr<CSSBasicShape> parseBasicShapeEllipse(CSSParserValueList* args);
    PassRefPtrWillBeRawPtr<CSSBasicShape> parseBasicShapePolygon(CSSParserValueList* args);
    PassRefPtrWillBeRawPtr<CSSBasicShape> parseBasicShapeInset(CSSParserValueList* args);

    bool parseFont(bool important);
    void parseSystemFont(bool important);
    PassRefPtrWillBeRawPtr<CSSValueList> parseFontFamily();

    PassRefPtrWillBeRawPtr<CSSValue> parseCounter(int defaultValue);
    PassRefPtrWillBeRawPtr<CSSValue> parseCounterContent(CSSParserValueList* args, bool counters);

    bool parseColorParameters(const CSSParserValue*, int* colorValues, bool parseAlpha);
    bool parseHSLParameters(const CSSParserValue*, double* colorValues, bool parseAlpha);
    PassRefPtrWillBeRawPtr<CSSPrimitiveValue> parseColor(const CSSParserValue*, bool acceptQuirkyColors = false);
    bool parseColorFromValue(const CSSParserValue*, RGBA32&, bool acceptQuirkyColors = false);

    bool acceptQuirkyColors(CSSPropertyID) const;
    PassRefPtrWillBeRawPtr<CSSPrimitiveValue> parseBackgroundColor(const CSSParserValue*);
    PassRefPtrWillBeRawPtr<CSSPrimitiveValue> parseTapHighlightColor(const CSSParserValue*);
    PassRefPtrWillBeRawPtr<CSSPrimitiveValue> parseGradientStopColor(const CSSParserValue*);
    PassRefPtrWillBeRawPtr<CSSPrimitiveValue> parseDeprecatedGradientStopColor(const CSSParserValue*);

    PassRefPtrWillBeRawPtr<CSSPrimitiveValue> parseLineHeight();
    bool parseFontSize(bool important);
    bool parseFontVariant(bool important);
    bool parseFontWeight(bool important);
    PassRefPtrWillBeRawPtr<CSSValueList> parseFontFaceSrc();
    PassRefPtrWillBeRawPtr<CSSValueList> parseFontFaceUnicodeRange();

    bool parseSVGValue(CSSPropertyID propId, bool important);
    PassRefPtrWillBeRawPtr<CSSValue> parseSVGStrokeDasharray();

    PassRefPtrWillBeRawPtr<CSSValue> parsePaintOrder() const;

    // CSS3 Parsing Routines (for properties specific to CSS3)
    PassRefPtrWillBeRawPtr<CSSValueList> parseShadow(CSSParserValueList*, CSSPropertyID);
    bool parseBorderImageShorthand(CSSPropertyID, bool important);
    PassRefPtrWillBeRawPtr<CSSValue> parseBorderImage(CSSPropertyID);
    bool parseBorderImageRepeat(RefPtrWillBeRawPtr<CSSValue>&);
    bool parseBorderImageSlice(CSSPropertyID, RefPtrWillBeRawPtr<CSSBorderImageSliceValue>&);
    bool parseBorderImageWidth(RefPtrWillBeRawPtr<CSSPrimitiveValue>&);
    bool parseBorderImageOutset(RefPtrWillBeRawPtr<CSSPrimitiveValue>&);
    bool parseBorderRadius(CSSPropertyID, bool important);

    PassRefPtrWillBeRawPtr<CSSValue> parseReflect();

    bool parseFlex(CSSParserValueList* args, bool important);

    PassRefPtrWillBeRawPtr<CSSValue> parsePosition(CSSParserValueList*);
    PassRefPtrWillBeRawPtr<CSSValueList> parsePositionList(CSSParserValueList*);

    // Image generators
    bool parseCanvas(CSSParserValueList*, RefPtrWillBeRawPtr<CSSValue>&);

    bool parseDeprecatedGradient(CSSParserValueList*, RefPtrWillBeRawPtr<CSSValue>&);
    bool parseDeprecatedLinearGradient(CSSParserValueList*, RefPtrWillBeRawPtr<CSSValue>&, CSSGradientRepeat repeating);
    bool parseDeprecatedRadialGradient(CSSParserValueList*, RefPtrWillBeRawPtr<CSSValue>&, CSSGradientRepeat repeating);
    bool parseLinearGradient(CSSParserValueList*, RefPtrWillBeRawPtr<CSSValue>&, CSSGradientRepeat repeating);
    bool parseRadialGradient(CSSParserValueList*, RefPtrWillBeRawPtr<CSSValue>&, CSSGradientRepeat repeating);
    bool parseGradientColorStops(CSSParserValueList*, CSSGradientValue*, bool expectComma);

    bool parseCrossfade(CSSParserValueList*, RefPtrWillBeRawPtr<CSSValue>&);

    PassRefPtrWillBeRawPtr<CSSValue> parseImageSet(CSSParserValueList*);

    PassRefPtrWillBeRawPtr<CSSValue> parseWillChange();

    PassRefPtrWillBeRawPtr<CSSValueList> parseFilter();
    PassRefPtrWillBeRawPtr<CSSFunctionValue> parseBuiltinFilterArguments(CSSParserValueList*, CSSValueID);

    PassRefPtrWillBeRawPtr<CSSValueList> parseTransformOrigin();
    PassRefPtrWillBeRawPtr<CSSValueList> parseTransform(bool useLegacyParsing);
    PassRefPtrWillBeRawPtr<CSSValue> parseTransformValue(bool useLegacyParsing, CSSParserValue*);

    PassRefPtrWillBeRawPtr<CSSValue> parseMotionPath();
    PassRefPtrWillBeRawPtr<CSSValue> parseMotionRotation();

    PassRefPtrWillBeRawPtr<CSSValue> parseTextEmphasisStyle();

    PassRefPtrWillBeRawPtr<CSSValue> parseTouchAction();
    PassRefPtrWillBeRawPtr<CSSValue> parseScrollBlocksOn();

    void addTextDecorationProperty(CSSPropertyID, PassRefPtrWillBeRawPtr<CSSValue>, bool important);
    bool parseTextDecoration(CSSPropertyID propId, bool important);

    PassRefPtrWillBeRawPtr<CSSValue> parseTextIndent();

    PassRefPtrWillBeRawPtr<CSSLineBoxContainValue> parseLineBoxContain();
    bool parseCalculation(CSSParserValue*, ValueRange);

    bool parseFontFeatureTag(CSSValueList*);
    PassRefPtrWillBeRawPtr<CSSValue> parseFontFeatureSettings();

    bool parseFontVariantLigatures(bool important);

    bool parseGeneratedImage(CSSParserValueList*, RefPtrWillBeRawPtr<CSSValue>&);

    PassRefPtrWillBeRawPtr<CSSPrimitiveValue> createPrimitiveNumericValue(CSSParserValue*);
    PassRefPtrWillBeRawPtr<CSSPrimitiveValue> createPrimitiveStringValue(CSSParserValue*);
    PassRefPtrWillBeRawPtr<CSSPrimitiveValue> createPrimitiveCustomIdentValue(CSSParserValue*);

    PassRefPtrWillBeRawPtr<CSSValue> createCSSImageValueWithReferrer(const String& rawValue, const KURL&);

    PassRefPtrWillBeRawPtr<CSSBasicShape> parseInsetRoundedCorners(PassRefPtrWillBeRawPtr<CSSBasicShapeInset>, CSSParserValueList*);

    enum SizeParameterType {
        None,
        Auto,
        Length,
        PageSize,
        Orientation,
    };

    PassRefPtrWillBeRawPtr<CSSPrimitiveValue> parsePage();
    PassRefPtrWillBeRawPtr<CSSValueList> parseSize();
    SizeParameterType parseSizeParameter(CSSValueList* parsedValues, CSSParserValue*, SizeParameterType prevParamType);

    bool parseFontFaceSrcURI(CSSValueList*);
    bool parseFontFaceSrcLocal(CSSValueList*);

    class ImplicitScope {
        STACK_ALLOCATED();
        WTF_MAKE_NONCOPYABLE(ImplicitScope);
    public:
        ImplicitScope(CSSPropertyParser* parser)
            : m_parser(parser)
        {
            m_parser->m_implicitShorthand = true;
        }

        ~ImplicitScope()
        {
            m_parser->m_implicitShorthand = false;
        }

    private:
        CSSPropertyParser* m_parser;
    };

    class ShorthandScope {
        STACK_ALLOCATED();
    public:
        ShorthandScope(CSSPropertyParser* parser, CSSPropertyID propId) : m_parser(parser)
        {
            if (!(m_parser->m_inParseShorthand++))
                m_parser->m_currentShorthand = propId;
        }
        ~ShorthandScope()
        {
            if (!(--m_parser->m_inParseShorthand))
                m_parser->m_currentShorthand = CSSPropertyInvalid;
        }

    private:
        CSSPropertyParser* m_parser;
    };

    enum ReleaseParsedCalcValueCondition {
        ReleaseParsedCalcValue,
        DoNotReleaseParsedCalcValue
    };

    friend inline Units operator|(Units a, Units b)
    {
        return static_cast<Units>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
    }

    bool validWidthOrHeight(CSSParserValue*, Units);

    bool validCalculationUnit(CSSParserValue*, Units, ReleaseParsedCalcValueCondition releaseCalc = DoNotReleaseParsedCalcValue);

    bool shouldAcceptUnitLessValues(CSSParserValue*, Units, CSSParserMode);

    inline bool validUnit(CSSParserValue* value, Units unitflags, ReleaseParsedCalcValueCondition releaseCalc = DoNotReleaseParsedCalcValue) { return validUnit(value, unitflags, m_context.mode(), releaseCalc); }
    bool validUnit(CSSParserValue*, Units, CSSParserMode, ReleaseParsedCalcValueCondition releaseCalc = DoNotReleaseParsedCalcValue);

    bool parseBorderImageQuad(Units, RefPtrWillBeRawPtr<CSSPrimitiveValue>&);
    int colorIntFromValue(CSSParserValue*);
    bool isCalculation(CSSParserValue*);

    bool buildBorderImageParseContext(CSSPropertyID, BorderImageParseContext&);

    bool parseDeprecatedGradientColorStop(CSSParserValue*, CSSGradientColorStop&);

    void commitBorderImageProperty(CSSPropertyID, PassRefPtrWillBeRawPtr<CSSValue>, bool important);

    PassRefPtrWillBeRawPtr<CSSValue> parseScrollSnapPoints();
    PassRefPtrWillBeRawPtr<CSSValue> parseScrollSnapDestination();
    PassRefPtrWillBeRawPtr<CSSValue> parseScrollSnapCoordinate();
    PassRefPtrWillBeRawPtr<CSSValue> parseScrollSnapPosition();

private:
    // Inputs:
    CSSParserValueList* m_valueList;
    const CSSParserContext& m_context;

    // Outputs:
    WillBeHeapVector<CSSProperty, 256>& m_parsedProperties;
    StyleRule::Type m_ruleType;

    // Locals during parsing:
    int m_inParseShorthand;
    CSSPropertyID m_currentShorthand;
    bool m_implicitShorthand;
    RefPtrWillBeMember<CSSCalcValue> m_parsedCalculation;
};

CSSPropertyID unresolvedCSSPropertyID(const CSSParserString&);
CSSValueID cssValueKeywordID(const CSSParserString&);

} // namespace blink

#endif // CSSPropertyParser_h
