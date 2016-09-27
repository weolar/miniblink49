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

#ifndef SVGPathSegCurvetoCubicAbs_h
#define SVGPathSegCurvetoCubicAbs_h

#include "core/svg/SVGPathSegCurvetoCubic.h"

namespace blink {

class SVGPathSegCurvetoCubicAbs final : public SVGPathSegCurvetoCubic {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<SVGPathSegCurvetoCubicAbs> create(SVGPathElement* element, float x, float y, float x1, float y1, float x2, float y2)
    {
        return adoptRefWillBeNoop(new SVGPathSegCurvetoCubicAbs(element, x, y, x1, y1, x2, y2));
    }

    PassRefPtrWillBeRawPtr<SVGPathSeg> clone() override
    {
        return adoptRefWillBeNoop(new SVGPathSegCurvetoCubicAbs(nullptr, x(), y(), x1(), y1(), x2(), y2()));
    }

private:
    SVGPathSegCurvetoCubicAbs(SVGPathElement* element, float x, float y, float x1, float y1, float x2, float y2)
        : SVGPathSegCurvetoCubic(element, x, y, x1, y1, x2, y2) { }

    unsigned short pathSegType() const override { return PATHSEG_CURVETO_CUBIC_ABS; }
    String pathSegTypeAsLetter() const override { return "C"; }
};

} // namespace blink

#endif // SVGPathSegCurvetoCubicAbs_h
