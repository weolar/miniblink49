/*
 * (C) 1999-2003 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2008 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Alexey Proskuryakov <ap@webkit.org>
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

#ifndef CSSPrimitiveValue_h
#define CSSPrimitiveValue_h

#include "core/CSSPropertyNames.h"
#include "core/CSSValueKeywords.h"
#include "core/CoreExport.h"
#include "core/css/CSSValue.h"
#include "platform/graphics/Color.h"
#include "wtf/BitVector.h"
#include "wtf/Forward.h"
#include "wtf/MathExtras.h"
#include "wtf/PassRefPtr.h"
#include "wtf/text/StringHash.h"

namespace blink {

class CSSBasicShape;
class CSSCalcValue;
class CSSToLengthConversionData;
class Counter;
class Length;
class LengthSize;
class Pair;
class Quad;
class RGBColor;
class Rect;
class ComputedStyle;

// Dimension calculations are imprecise, often resulting in values of e.g.
// 44.99998. We need to go ahead and round if we're really close to the next
// integer value.
template<typename T> inline T roundForImpreciseConversion(double value)
{
    value += (value < 0) ? -0.01 : +0.01;
    return ((value > std::numeric_limits<T>::max()) || (value < std::numeric_limits<T>::min())) ? 0 : static_cast<T>(value);
}

template<> inline float roundForImpreciseConversion(double value)
{
    double ceiledValue = ceil(value);
    double proximityToNextInt = ceiledValue - value;
    if (proximityToNextInt <= 0.01 && value > 0)
        return static_cast<float>(ceiledValue);
    if (proximityToNextInt >= 0.99 && value < 0)
        return static_cast<float>(floor(value));
    return static_cast<float>(value);
}

// CSSPrimitiveValues are immutable. This class has manual ref-counting
// of unioned types and does not have the code necessary
// to handle any kind of mutations.
class CORE_EXPORT CSSPrimitiveValue : public CSSValue {
public:
    enum UnitType {
        CSS_UNKNOWN = 0,
        CSS_NUMBER = 1,
        CSS_PERCENTAGE = 2,
        CSS_EMS = 3,
        CSS_EXS = 4,
        CSS_PX = 5,
        CSS_CM = 6,
        CSS_MM = 7,
        CSS_IN = 8,
        CSS_PT = 9,
        CSS_PC = 10,
        CSS_DEG = 11,
        CSS_RAD = 12,
        CSS_GRAD = 13,
        CSS_MS = 14,
        CSS_S = 15,
        CSS_HZ = 16,
        CSS_KHZ = 17,
        CSS_CUSTOM_IDENT = 19,
        CSS_URI = 20,
        CSS_IDENT = 21,
        CSS_ATTR = 22,
        CSS_COUNTER = 23,
        CSS_RECT = 24,
        CSS_RGBCOLOR = 25,
        CSS_VW = 26,
        CSS_VH = 27,
        CSS_VMIN = 28,
        CSS_VMAX = 29,
        CSS_DPPX = 30,
        CSS_DPI = 31,
        CSS_DPCM = 32,
        CSS_FR = 33,
        CSS_INTEGER = 34,
        CSS_PAIR = 100,
        CSS_TURN = 107,
        CSS_REMS = 108,
        CSS_CHS = 109,
        CSS_SHAPE = 111,
        CSS_QUAD = 112,
        CSS_CALC = 113,
        CSS_CALC_PERCENTAGE_WITH_NUMBER = 114,
        CSS_CALC_PERCENTAGE_WITH_LENGTH = 115,
        CSS_STRING = 116,
        CSS_PROPERTY_ID = 117,
        CSS_VALUE_ID = 118,
        CSS_QEM = 119
    };

    enum LengthUnitType {
        UnitTypePixels = 0,
        UnitTypePercentage,
        UnitTypeFontSize,
        UnitTypeFontXSize,
        UnitTypeRootFontSize,
        UnitTypeZeroCharacterWidth,
        UnitTypeViewportWidth,
        UnitTypeViewportHeight,
        UnitTypeViewportMin,
        UnitTypeViewportMax,

        // This value must come after the last length unit type to enable iteration over the length unit types.
        LengthUnitTypeCount,
    };

    typedef Vector<double, CSSPrimitiveValue::LengthUnitTypeCount> CSSLengthArray;
    typedef BitVector CSSLengthTypeArray;

    void accumulateLengthArray(CSSLengthArray&, double multiplier = 1) const;
    void accumulateLengthArray(CSSLengthArray&, CSSLengthTypeArray&, double multiplier = 1) const;

    enum UnitCategory {
        UNumber,
        UPercent,
        ULength,
        UAngle,
        UTime,
        UFrequency,
        UResolution,
        UOther
    };
    static UnitCategory unitCategory(UnitType);
    static float clampToCSSLengthRange(double);

    static void initUnitTable();

    static UnitType fromName(const String& unit);

    bool isAngle() const
    {
        return m_primitiveUnitType == CSS_DEG
               || m_primitiveUnitType == CSS_RAD
               || m_primitiveUnitType == CSS_GRAD
               || m_primitiveUnitType == CSS_TURN;
    }
    bool isAttr() const { return m_primitiveUnitType == CSS_ATTR; }
    bool isCounter() const { return m_primitiveUnitType == CSS_COUNTER; }
    bool isCustomIdent() const { return m_primitiveUnitType == CSS_CUSTOM_IDENT; }
    bool isFontRelativeLength() const
    {
        return m_primitiveUnitType == CSS_EMS
            || m_primitiveUnitType == CSS_EXS
            || m_primitiveUnitType == CSS_REMS
            || m_primitiveUnitType == CSS_CHS;
    }
    bool isViewportPercentageLength() const { return isViewportPercentageLength(static_cast<UnitType>(m_primitiveUnitType)); }
    static bool isViewportPercentageLength(UnitType type) { return type >= CSS_VW && type <= CSS_VMAX; }
    static bool isLength(UnitType type)
    {
        return (type >= CSS_EMS && type <= CSS_PC) || type == CSS_REMS || type == CSS_CHS || isViewportPercentageLength(type);
    }
    bool isLength() const { return isLength(primitiveType()); }
    bool isNumber() const { return primitiveType() == CSS_NUMBER || primitiveType() == CSS_INTEGER; }
    bool isPercentage() const { return primitiveType() == CSS_PERCENTAGE; }
    bool isPx() const { return primitiveType() == CSS_PX; }
    bool isRect() const { return m_primitiveUnitType == CSS_RECT; }
    bool isRGBColor() const { return m_primitiveUnitType == CSS_RGBCOLOR; }
    bool isShape() const { return m_primitiveUnitType == CSS_SHAPE; }
    bool isString() const { return m_primitiveUnitType == CSS_STRING; }
    bool isTime() const { return m_primitiveUnitType == CSS_S || m_primitiveUnitType == CSS_MS; }
    bool isURI() const { return m_primitiveUnitType == CSS_URI; }
    bool isCalculated() const { return m_primitiveUnitType == CSS_CALC; }
    bool isCalculatedPercentageWithNumber() const { return primitiveType() == CSS_CALC_PERCENTAGE_WITH_NUMBER; }
    bool isCalculatedPercentageWithLength() const { return primitiveType() == CSS_CALC_PERCENTAGE_WITH_LENGTH; }
    static bool isDotsPerInch(UnitType type) { return type == CSS_DPI; }
    static bool isDotsPerPixel(UnitType type) { return type == CSS_DPPX; }
    static bool isDotsPerCentimeter(UnitType type) { return type == CSS_DPCM; }
    static bool isResolution(UnitType type) { return type >= CSS_DPPX && type <= CSS_DPCM; }
    bool isFlex() const { return primitiveType() == CSS_FR; }
    bool isValueID() const { return m_primitiveUnitType == CSS_VALUE_ID; }
    bool colorIsDerivedFromElement() const;

    static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> createIdentifier(CSSValueID valueID)
    {
        return adoptRefWillBeNoop(new CSSPrimitiveValue(valueID));
    }
    static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> createIdentifier(CSSPropertyID propertyID)
    {
        return adoptRefWillBeNoop(new CSSPrimitiveValue(propertyID));
    }
    static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> createColor(RGBA32 rgbValue)
    {
        return adoptRefWillBeNoop(new CSSPrimitiveValue(rgbValue));
    }
    static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> create(double value, UnitType type)
    {
        return adoptRefWillBeNoop(new CSSPrimitiveValue(value, type));
    }
    static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> create(const String& value, UnitType type)
    {
        return adoptRefWillBeNoop(new CSSPrimitiveValue(value, type));
    }
    static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> create(const Length& value, float zoom)
    {
        return adoptRefWillBeNoop(new CSSPrimitiveValue(value, zoom));
    }
    static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> create(const LengthSize& value, const ComputedStyle& style)
    {
        return adoptRefWillBeNoop(new CSSPrimitiveValue(value, style));
    }
    template<typename T> static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> create(T value)
    {
        return adoptRefWillBeNoop(new CSSPrimitiveValue(value));
    }

    // This value is used to handle quirky margins in reflow roots (body, td, and th) like WinIE.
    // The basic idea is that a stylesheet can use the value __qem (for quirky em) instead of em.
    // When the quirky value is used, if you're in quirks mode, the margin will collapse away
    // inside a table cell.
    static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> createAllowingMarginQuirk(double value, UnitType type)
    {
        CSSPrimitiveValue* quirkValue = new CSSPrimitiveValue(value, type);
        quirkValue->m_isQuirkValue = true;
        return adoptRefWillBeNoop(quirkValue);
    }

    ~CSSPrimitiveValue();

    void cleanup();

    UnitType primitiveType() const;

    double computeDegrees() const;
    double computeSeconds();

    // Computes a length in pixels, resolving relative lengths
    template<typename T> T computeLength(const CSSToLengthConversionData&);

    // Converts to a Length (Fixed, Percent or Calculated)
    Length convertToLength(const CSSToLengthConversionData&);

    double getDoubleValue() const;
    float getFloatValue() const { return getValue<float>(); }
    int getIntValue() const { return getValue<int>(); }
    template<typename T> inline T getValue() const { return clampTo<T>(getDoubleValue()); }

    String getStringValue() const;

    Counter* getCounterValue() const { return m_primitiveUnitType != CSS_COUNTER ? 0 : m_value.counter; }

    Rect* getRectValue() const { return m_primitiveUnitType != CSS_RECT ? 0 : m_value.rect; }

    Quad* getQuadValue() const { return m_primitiveUnitType != CSS_QUAD ? 0 : m_value.quad; }

    RGBA32 getRGBA32Value() const { return m_primitiveUnitType != CSS_RGBCOLOR ? 0 : m_value.rgbcolor; }

    Pair* getPairValue() const { return m_primitiveUnitType != CSS_PAIR ? 0 : m_value.pair; }

    CSSBasicShape* getShapeValue() const { return m_primitiveUnitType != CSS_SHAPE ? 0 : m_value.shape; }

    CSSCalcValue* cssCalcValue() const { return m_primitiveUnitType != CSS_CALC ? 0 : m_value.calc; }

    CSSPropertyID getPropertyID() const { return m_primitiveUnitType == CSS_PROPERTY_ID ? m_value.propertyID : CSSPropertyInvalid; }
    CSSValueID getValueID() const { return m_primitiveUnitType == CSS_VALUE_ID ? m_value.valueID : CSSValueInvalid; }

    template<typename T> inline operator T() const; // Defined in CSSPrimitiveValueMappings.h

    static const char* unitTypeToString(UnitType);
    String customCSSText() const;

    bool isQuirkValue() { return m_isQuirkValue; }

    bool equals(const CSSPrimitiveValue&) const;

    DECLARE_TRACE_AFTER_DISPATCH();

    static UnitType canonicalUnitTypeForCategory(UnitCategory);
    static double conversionToCanonicalUnitsScaleFactor(UnitType);

    // Returns true and populates lengthUnitType, if unitType is a length unit. Otherwise, returns false.
    static bool unitTypeToLengthUnitType(UnitType, LengthUnitType&);
    static UnitType lengthUnitTypeToUnitType(LengthUnitType);

private:
    CSSPrimitiveValue(CSSValueID);
    CSSPrimitiveValue(CSSPropertyID);
    CSSPrimitiveValue(RGBA32 color);
    CSSPrimitiveValue(const Length&, float zoom);
    CSSPrimitiveValue(const LengthSize&, const ComputedStyle&);
    CSSPrimitiveValue(const String&, UnitType);
    CSSPrimitiveValue(double, UnitType);

    template<typename T> CSSPrimitiveValue(T); // Defined in CSSPrimitiveValueMappings.h
    template<typename T> CSSPrimitiveValue(T* val)
        : CSSValue(PrimitiveClass)
    {
        init(PassRefPtrWillBeRawPtr<T>(val));
    }

    template<typename T> CSSPrimitiveValue(PassRefPtrWillBeRawPtr<T> val)
        : CSSValue(PrimitiveClass)
    {
        init(val);
    }

    static void create(int); // compile-time guard
    static void create(unsigned); // compile-time guard
    template<typename T> operator T*(); // compile-time guard

    void init(const Length&);
    void init(const LengthSize&, const ComputedStyle&);
    void init(PassRefPtrWillBeRawPtr<Counter>);
    void init(PassRefPtrWillBeRawPtr<Rect>);
    void init(PassRefPtrWillBeRawPtr<Pair>);
    void init(PassRefPtrWillBeRawPtr<Quad>);
    void init(PassRefPtrWillBeRawPtr<CSSBasicShape>);
    void init(PassRefPtrWillBeRawPtr<CSSCalcValue>);

    double computeLengthDouble(const CSSToLengthConversionData&);

    union {
        CSSPropertyID propertyID;
        CSSValueID valueID;
        double num;
        StringImpl* string;
        RGBA32 rgbcolor;
        // FIXME: oilpan: Should be members, but no support for members in unions. Just trace the raw ptr for now.
        CSSBasicShape* shape;
        CSSCalcValue* calc;
        Counter* counter;
        Pair* pair;
        Rect* rect;
        Quad* quad;
    } m_value;
};

typedef CSSPrimitiveValue::CSSLengthArray CSSLengthArray;
typedef CSSPrimitiveValue::CSSLengthTypeArray CSSLengthTypeArray;

DEFINE_CSS_VALUE_TYPE_CASTS(CSSPrimitiveValue, isPrimitiveValue());

} // namespace blink

#endif // CSSPrimitiveValue_h
