/*
 * Copyright (C) 2004, 2005, 2006, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2008 Rob Buis <buis@kde.org>
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

#ifndef SVGPathSeg_h
#define SVGPathSeg_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/geometry/FloatPoint.h"
#include "platform/heap/Handle.h"
#include "wtf/RefCounted.h"
#include "wtf/text/WTFString.h"

namespace blink {

enum ListModification {
    ListModificationUnknown = 0,
    ListModificationInsert = 1,
    ListModificationReplace = 2,
    ListModificationRemove = 3,
    ListModificationAppend = 4
};

enum SVGPathSegType {
    PathSegUnknown = 0,
    PathSegClosePath = 1,
    PathSegMoveToAbs = 2,
    PathSegMoveToRel = 3,
    PathSegLineToAbs = 4,
    PathSegLineToRel = 5,
    PathSegCurveToCubicAbs = 6,
    PathSegCurveToCubicRel = 7,
    PathSegCurveToQuadraticAbs = 8,
    PathSegCurveToQuadraticRel = 9,
    PathSegArcAbs = 10,
    PathSegArcRel = 11,
    PathSegLineToHorizontalAbs = 12,
    PathSegLineToHorizontalRel = 13,
    PathSegLineToVerticalAbs = 14,
    PathSegLineToVerticalRel = 15,
    PathSegCurveToCubicSmoothAbs = 16,
    PathSegCurveToCubicSmoothRel = 17,
    PathSegCurveToQuadraticSmoothAbs = 18,
    PathSegCurveToQuadraticSmoothRel = 19
};

static inline SVGPathSegType toAbsolutePathSegType(const SVGPathSegType type)
{
    // Clear the LSB to get the absolute command.
    return type >= PathSegMoveToAbs ? static_cast<SVGPathSegType>(type & ~1u) : type;
}

static inline bool isAbsolutePathSegType(const SVGPathSegType type)
{
    // For commands with an ordinal >= PathSegMoveToAbs, and odd number => relative command.
    return type < PathSegMoveToAbs || type % 2 == 0;
}

struct PathSegmentData {
    PathSegmentData()
        : command(PathSegUnknown)
        , arcSweep(false)
        , arcLarge(false)
    {
    }

    const FloatPoint& arcRadii() const { return point1; }
    float arcAngle() const { return point2.x(); }

    SVGPathSegType command;
    FloatPoint targetPoint;
    FloatPoint point1;
    FloatPoint point2;
    bool arcSweep;
    bool arcLarge;
};

class SVGPropertyBase;
class SVGPathElement;
class SVGElement;

class SVGPathSeg : public RefCountedWillBeGarbageCollectedFinalized<SVGPathSeg>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    // SVGPathSeg itself is used as a tear-off type.
    // FIXME: A tear-off type should be introduced to distinguish animVal/baseVal
    typedef SVGPathSeg TearOffType;

    explicit SVGPathSeg(SVGPathElement* contextElement);

    virtual ~SVGPathSeg() { }

    // Forward declare these enums in the w3c naming scheme, for IDL generation
    enum {
        PATHSEG_UNKNOWN = PathSegUnknown,
        PATHSEG_CLOSEPATH = PathSegClosePath,
        PATHSEG_MOVETO_ABS = PathSegMoveToAbs,
        PATHSEG_MOVETO_REL = PathSegMoveToRel,
        PATHSEG_LINETO_ABS = PathSegLineToAbs,
        PATHSEG_LINETO_REL = PathSegLineToRel,
        PATHSEG_CURVETO_CUBIC_ABS = PathSegCurveToCubicAbs,
        PATHSEG_CURVETO_CUBIC_REL = PathSegCurveToCubicRel,
        PATHSEG_CURVETO_QUADRATIC_ABS = PathSegCurveToQuadraticAbs,
        PATHSEG_CURVETO_QUADRATIC_REL = PathSegCurveToQuadraticRel,
        PATHSEG_ARC_ABS = PathSegArcAbs,
        PATHSEG_ARC_REL = PathSegArcRel,
        PATHSEG_LINETO_HORIZONTAL_ABS = PathSegLineToHorizontalAbs,
        PATHSEG_LINETO_HORIZONTAL_REL = PathSegLineToHorizontalRel,
        PATHSEG_LINETO_VERTICAL_ABS = PathSegLineToVerticalAbs,
        PATHSEG_LINETO_VERTICAL_REL = PathSegLineToVerticalRel,
        PATHSEG_CURVETO_CUBIC_SMOOTH_ABS = PathSegCurveToCubicSmoothAbs,
        PATHSEG_CURVETO_CUBIC_SMOOTH_REL = PathSegCurveToCubicSmoothRel,
        PATHSEG_CURVETO_QUADRATIC_SMOOTH_ABS = PathSegCurveToQuadraticSmoothAbs,
        PATHSEG_CURVETO_QUADRATIC_SMOOTH_REL = PathSegCurveToQuadraticSmoothRel
    };

    virtual unsigned short pathSegType() const = 0;
    virtual String pathSegTypeAsLetter() const = 0;

    SVGPropertyBase* ownerList() const
    {
        return m_ownerList;
    }

    void setOwnerList(SVGPropertyBase* ownerList)
    {
        // Previous owner list must be cleared before setting new owner list.
        ASSERT((!ownerList && m_ownerList) || (ownerList && !m_ownerList));

        m_ownerList = ownerList;
    }

    void setContextElement(SVGElement* contextElement)
    {
        m_contextElement = contextElement;
    }

    static PassRefPtrWillBeRawPtr<SVGPathSeg> create() { ASSERT_NOT_REACHED(); return nullptr; }
    virtual PassRefPtrWillBeRawPtr<SVGPathSeg> clone() = 0;

    DECLARE_VIRTUAL_TRACE();

protected:
    void commitChange();

private:
    RawPtrWillBeMember<SVGPropertyBase> m_ownerList;
    RawPtrWillBeMember<SVGElement> m_contextElement;
};

class SVGPathSegSingleCoordinate : public SVGPathSeg {
public:
    float x() const { return m_x; }
    void setX(float x)
    {
        m_x = x;
        commitChange();
    }

    float y() const { return m_y; }
    void setY(float y)
    {
        m_y = y;
        commitChange();
    }

protected:
    SVGPathSegSingleCoordinate(SVGPathElement* element, float x, float y)
        : SVGPathSeg(element)
        , m_x(x)
        , m_y(y)
    {
    }

private:
    float m_x;
    float m_y;
};

} // namespace blink

#endif // SVGPathSeg_h
