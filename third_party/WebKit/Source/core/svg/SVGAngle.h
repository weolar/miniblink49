/*
 * Copyright (C) 2004, 2005, 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
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

#ifndef SVGAngle_h
#define SVGAngle_h

#include "core/svg/SVGEnumeration.h"
#include "core/svg/properties/SVGPropertyHelper.h"
#include "platform/heap/Handle.h"

namespace blink {

class SVGAngle;
class SVGAngleTearOff;

enum SVGMarkerOrientType {
    SVGMarkerOrientUnknown = 0,
    SVGMarkerOrientAuto,
    SVGMarkerOrientAngle,
    SVGMarkerOrientAutoStartReverse
};
template<> const SVGEnumerationStringEntries& getStaticStringEntries<SVGMarkerOrientType>();
template<> unsigned short getMaxExposedEnumValue<SVGMarkerOrientType>();

class SVGMarkerOrientEnumeration : public SVGEnumeration<SVGMarkerOrientType> {
public:
    static PassRefPtrWillBeRawPtr<SVGMarkerOrientEnumeration> create(SVGAngle* angle)
    {
        return adoptRefWillBeNoop(new SVGMarkerOrientEnumeration(angle));
    }

    ~SVGMarkerOrientEnumeration() override;

    void add(PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*) override;
    void calculateAnimatedValue(SVGAnimationElement*, float, unsigned, PassRefPtrWillBeRawPtr<SVGPropertyBase>, PassRefPtrWillBeRawPtr<SVGPropertyBase>, PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*) override;
    float calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*) override;

    DECLARE_VIRTUAL_TRACE();

private:
    SVGMarkerOrientEnumeration(SVGAngle*);

    void notifyChange() override;

    RawPtrWillBeMember<SVGAngle> m_angle;
};

class SVGAngle : public SVGPropertyHelper<SVGAngle> {
public:
    typedef SVGAngleTearOff TearOffType;

    enum SVGAngleType {
        SVG_ANGLETYPE_UNKNOWN = 0,
        SVG_ANGLETYPE_UNSPECIFIED = 1,
        SVG_ANGLETYPE_DEG = 2,
        SVG_ANGLETYPE_RAD = 3,
        SVG_ANGLETYPE_GRAD = 4,
        SVG_ANGLETYPE_TURN = 5
    };

    static PassRefPtrWillBeRawPtr<SVGAngle> create()
    {
        return adoptRefWillBeNoop(new SVGAngle());
    }

    ~SVGAngle() override;

    SVGAngleType unitType() const { return m_unitType; }

    void setValue(float);
    float value() const;

    void setValueInSpecifiedUnits(float valueInSpecifiedUnits) { m_valueInSpecifiedUnits = valueInSpecifiedUnits; }
    float valueInSpecifiedUnits() const { return m_valueInSpecifiedUnits; }

    void newValueSpecifiedUnits(SVGAngleType unitType, float valueInSpecifiedUnits);
    void convertToSpecifiedUnits(SVGAngleType unitType, ExceptionState&);

    SVGEnumeration<SVGMarkerOrientType>* orientType() { return m_orientType.get(); }
    const SVGEnumeration<SVGMarkerOrientType>* orientType() const { return m_orientType.get(); }
    void orientTypeChanged();

    // SVGPropertyBase:

    PassRefPtrWillBeRawPtr<SVGAngle> clone() const;

    String valueAsString() const override;
    void setValueAsString(const String&, ExceptionState&);

    void add(PassRefPtrWillBeRawPtr<SVGPropertyBase>, SVGElement*) override;
    void calculateAnimatedValue(SVGAnimationElement*, float percentage, unsigned repeatCount, PassRefPtrWillBeRawPtr<SVGPropertyBase> from, PassRefPtrWillBeRawPtr<SVGPropertyBase> to, PassRefPtrWillBeRawPtr<SVGPropertyBase> toAtEndOfDurationValue, SVGElement* contextElement) override;
    float calculateDistance(PassRefPtrWillBeRawPtr<SVGPropertyBase> to, SVGElement* contextElement) override;

    static AnimatedPropertyType classType() { return AnimatedAngle; }

    DECLARE_VIRTUAL_TRACE();

private:
    SVGAngle();
    SVGAngle(SVGAngleType, float, SVGMarkerOrientType);

    void assign(const SVGAngle&);

    SVGAngleType m_unitType;
    float m_valueInSpecifiedUnits;
    RefPtrWillBeMember<SVGMarkerOrientEnumeration> m_orientType;
};

inline PassRefPtrWillBeRawPtr<SVGAngle> toSVGAngle(PassRefPtrWillBeRawPtr<SVGPropertyBase> passBase)
{
    RefPtrWillBeRawPtr<SVGPropertyBase> base = passBase;
    ASSERT(base->type() == SVGAngle::classType());
    return static_pointer_cast<SVGAngle>(base.release());
}

} // namespace blink

#endif // SVGAngle_h
