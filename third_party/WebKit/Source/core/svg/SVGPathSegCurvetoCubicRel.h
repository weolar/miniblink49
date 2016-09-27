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

#ifndef SVGPathSegCurvetoCubicRel_h
#define SVGPathSegCurvetoCubicRel_h

#include "core/svg/SVGPathSegCurvetoCubic.h"

namespace blink {

class SVGPathSegCurvetoCubicRel final : public SVGPathSegCurvetoCubic {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<SVGPathSegCurvetoCubicRel> create(SVGPathElement* element, float x, float y, float x1, float y1, float x2, float y2)
    {
        return adoptRefWillBeNoop(new SVGPathSegCurvetoCubicRel(element, x, y, x1, y1, x2, y2));
    }

    PassRefPtrWillBeRawPtr<SVGPathSeg> clone() override
    {
        return adoptRefWillBeNoop(new SVGPathSegCurvetoCubicRel(nullptr, x(), y(), x1(), y1(), x2(), y2()));
    }

private:
    SVGPathSegCurvetoCubicRel(SVGPathElement* element, float x, float y, float x1, float y1, float x2, float y2)
        : SVGPathSegCurvetoCubic(element, x, y, x1, y1, x2, y2) { }

    unsigned short pathSegType() const override { return PATHSEG_CURVETO_CUBIC_REL; }
    String pathSegTypeAsLetter() const override { return "c"; }
};

} // namespace blink

#endif // SVGPathSegCurvetoCubicRel_h
