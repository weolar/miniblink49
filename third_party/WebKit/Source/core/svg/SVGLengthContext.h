/*
 * Copyright (C) Research In Motion Limited 2011. All rights reserved.
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

#ifndef SVGLengthContext_h
#define SVGLengthContext_h

#include "core/svg/SVGUnitTypes.h"
#include "platform/Length.h"
#include "platform/geometry/FloatRect.h"

namespace blink {

class ComputedStyle;
class SVGElement;
class SVGLength;
class UnzoomedLength;

enum SVGLengthType {
    LengthTypeUnknown = 0,
    LengthTypeNumber,
    LengthTypePercentage,
    LengthTypeEMS,
    LengthTypeEXS,
    LengthTypePX,
    LengthTypeCM,
    LengthTypeMM,
    LengthTypeIN,
    LengthTypePT,
    LengthTypePC,
    LengthTypeREMS,
    LengthTypeCHS
};

enum class SVGLengthMode {
    Width,
    Height,
    Other
};

class SVGLengthContext {
    STACK_ALLOCATED();
public:
    explicit SVGLengthContext(const SVGElement*);

    template<typename T>
    static FloatRect resolveRectangle(const T* context, SVGUnitTypes::SVGUnitType type, const FloatRect& viewport)
    {
        return resolveRectangle(context, type, viewport, *context->x()->currentValue(), *context->y()->currentValue(), *context->width()->currentValue(), *context->height()->currentValue());
    }

    static FloatRect resolveRectangle(const SVGElement*, SVGUnitTypes::SVGUnitType, const FloatRect& viewport, const SVGLength& x, const SVGLength& y, const SVGLength& width, const SVGLength& height);
    static FloatPoint resolvePoint(const SVGElement*, SVGUnitTypes::SVGUnitType, const SVGLength& x, const SVGLength& y);
    static float resolveLength(const SVGElement*, SVGUnitTypes::SVGUnitType, const SVGLength&);

    float convertValueToUserUnits(float, SVGLengthMode, SVGLengthType fromUnit) const;
    float convertValueFromUserUnits(float, SVGLengthMode, SVGLengthType toUnit) const;

    float valueForLength(const UnzoomedLength&, SVGLengthMode = SVGLengthMode::Other) const;
    float valueForLength(const Length&, const ComputedStyle&, SVGLengthMode = SVGLengthMode::Other) const;
    static float valueForLength(const Length&, const ComputedStyle&, float dimension);

    bool determineViewport(FloatSize&) const;

private:
    float valueForLength(const Length&, float zoom, SVGLengthMode) const;
    static float valueForLength(const Length&, float zoom, float dimension);

    float convertValueFromUserUnitsToEXS(float value) const;
    float convertValueFromEXSToUserUnits(float value) const;

    float convertValueFromUserUnitsToCHS(float value) const;
    float convertValueFromCHSToUserUnits(float value) const;

    RawPtrWillBeMember<const SVGElement> m_context;
};

} // namespace blink

#endif // SVGLengthContext_h
