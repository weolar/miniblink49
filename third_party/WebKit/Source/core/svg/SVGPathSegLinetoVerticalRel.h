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

#ifndef SVGPathSegLinetoVerticalRel_h
#define SVGPathSegLinetoVerticalRel_h

#include "core/svg/SVGPathSegLinetoVertical.h"

namespace blink {

class SVGPathSegLinetoVerticalRel final : public SVGPathSegLinetoVertical {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<SVGPathSegLinetoVerticalRel> create(SVGPathElement* element, float y)
    {
        return adoptRefWillBeNoop(new SVGPathSegLinetoVerticalRel(element, y));
    }

    PassRefPtrWillBeRawPtr<SVGPathSeg> clone() override
    {
        return adoptRefWillBeNoop(new SVGPathSegLinetoVerticalRel(nullptr, y()));
    }

private:
    SVGPathSegLinetoVerticalRel(SVGPathElement* element, float y)
        : SVGPathSegLinetoVertical(element, y) { }

    unsigned short pathSegType() const override { return PATHSEG_LINETO_VERTICAL_REL; }
    String pathSegTypeAsLetter() const override { return "v"; }
};

} // namespace blink

#endif // SVGPathSegLinetoVerticalRel_h
