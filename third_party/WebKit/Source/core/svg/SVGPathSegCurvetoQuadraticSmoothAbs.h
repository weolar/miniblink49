/*
 * Copyright (C) 2004, 2005, 2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2008 Rob Buis <buis@kde.org>
 * Copyright (C) 2013 Samsung Electronics. All rights reserved.
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

#ifndef SVGPathSegCurvetoQuadraticSmoothAbs_h
#define SVGPathSegCurvetoQuadraticSmoothAbs_h

#include "core/svg/SVGPathSeg.h"

namespace blink {

class SVGPathSegCurvetoQuadraticSmoothAbs final : public SVGPathSegSingleCoordinate {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<SVGPathSegCurvetoQuadraticSmoothAbs> create(SVGPathElement* element, float x, float y)
    {
        return adoptRefWillBeNoop(new SVGPathSegCurvetoQuadraticSmoothAbs(element, x, y));
    }

    PassRefPtrWillBeRawPtr<SVGPathSeg> clone() override
    {
        return adoptRefWillBeNoop(new SVGPathSegCurvetoQuadraticSmoothAbs(nullptr, x(), y()));
    }

private:
    SVGPathSegCurvetoQuadraticSmoothAbs(SVGPathElement* element, float x, float y)
        : SVGPathSegSingleCoordinate(element, x, y) { }

    unsigned short pathSegType() const override { return PATHSEG_CURVETO_QUADRATIC_SMOOTH_ABS; }
    String pathSegTypeAsLetter() const override { return "T"; }
};

} // namespace blink

#endif // SVGPathSegCurvetoQuadraticSmoothAbs_h
