/*
 * Copyright (C) 2004, 2005, 2006, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
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

#ifndef SVGLength_h
#define SVGLength_h

#include "core/css/CSSPrimitiveValue.h"
#include "core/svg/SVGLengthContext.h"
#include "core/svg/properties/SVGProperty.h"
#include "platform/heap/Handle.h"

namespace blink {

class ExceptionState;
class QualifiedName;

enum SVGLengthNegativeValuesMode {
    AllowNegativeLengths,
    ForbidNegativeLengths
};

class SVGLengthTearOff;

class SVGLength final : public SVGPropertyBase {
public:
    typedef SVGLengthTearOff TearOffType;

    static PassRefPtrWillBeRawPtr<SVGLength> create(SVGLengthMode mode = SVGLengthMode::Other)
    {
        return adoptRefWillBeNoop(new SVGLength(mode));
    }

    PassRefPtrWillBeRawPtr<SVGLength> clone() const;
    PassRefPtrWillBeRawPtr<SVGPropertyBase> cloneForAnimation(const String&) const override;

    SVGLengthType unitType() const { return static_cast<SVGLengthType>(m_unitType); }
    CSSPrimitiveValue::UnitType cssUnitTypeQuirk() const
    {
        if (m_unitType == LengthTypeNumber)
            return CSSPrimitiveValue::UnitType::CSS_PX;

        if (m_unitType == LengthTypeREMS)
            return CSSPrimitiveValue::UnitType::CSS_REMS;
        if (m_unitType == LengthTypeCHS)
            return CSSPrimitiveValue::UnitType::CSS_CHS;

        return static_cast<CSSPrimitiveValue::UnitType>(m_unitType);
    }
    void setUnitType(SVGLengthType);
    SVGLengthMode unitMode() const { return static_cast<SVGLengthMode>(m_unitMode); }

    bool operator==(const SVGLength&) const;
    bool operator!=(const SVGLength& other) const { return !operator==(other); }

    float value(const SVGLengthContext&) const;
    void setValue(float, const SVGLengthContext&);

    float valueInSpecifiedUnits() const { return m_valueInSpecifiedUnits; }
    void setValueInSpecifiedUnits(float value) { m_valueInSpecifiedUnits = value; }

    // Resolves LengthTypePercentage into a normalized floating point number (full value is 1.0).
    float valueAsPercentage() const;

    // Returns a number to be used as percentage (so full value is 100)
    float valueAsPercentage100() const;

    // Scale the input value by this SVGLength. Higher precision than input * valueAsPercentage().
    float scaleByPercentage(float) const;

    String valueAsString() const override;
    void setValueAsString(const String&, ExceptionState&);

    void newValueSpecifiedUnits(SVGLengthType, float valueInSpecifiedUnits);
    void convertToSpecifiedUnits(SVGLengthType, const SVGLengthContext&);

    // Helper functions
    static inline bool isRelativeUnit(SVGLengthType unitType)
    {
        return unitType == LengthTypePercentage
            || unitType == LengthTypeEMS
            || unitType == LengthTypeEXS
            || unitType == LengthTypeREMS
            || unitType == LengthTypeCHS;
    }
    inline bool isRelative() const { return isRelativeUnit(unitType()); }

    bool isZero() const
    {
        return !m_valueInSpecifiedUnits;
    }

    static PassRefPtrWillBeRawPtr<SVGLength> fromCSSPrimitiveValue(CSSPrimitiveValue*);
    static PassRefPtrWillBeRawPtr<CSSPrimitiveValue> toCSSPrimitiveValue(PassRefPtrWillBeRawPtr<SVGLength>);
    static SVGLengthMode lengthModeForAnimatedLengthAttribute(const QualifiedName&);

    PassRefPtrWillBeRawPtr<SVGLength> blend(PassRefPtrWillBeRawPtr<SVGLength> from, float progress) const;

    void add(PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*) override;
    void calculateAnimatedValue(SVGAnimationElement*, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> from, PassRefPtrWillBeRawPtr<SVGPropertyBase> to, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDurationValue, SVGElement* contextElement) override;
    float calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> to, SVGElement* contextElement) override;

    static AnimatedPropertyType classType() { return AnimatedLength; }

private:
    SVGLength(SVGLengthMode);
    SVGLength(const SVGLength&);

    float m_valueInSpecifiedUnits;
    unsigned m_unitMode : 2;
    unsigned m_unitType : 4;
};

inline PassRefPtrWillBeRawPtr<SVGLength> toSVGLength(PassRefPtrWillBeRawPtr<SVGPropertyBase> passBase)
{
    RefPtrWillBeRawPtr<SVGPropertyBase> base = passBase;
    ASSERT(base->type() == SVGLength::classType());
    return static_pointer_cast<SVGLength>(base.release());
}

} // namespace blink

#endif // SVGLength_h
