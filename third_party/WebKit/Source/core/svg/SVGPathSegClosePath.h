/*
 * Copyright (C) 2004, 2005, 2006 Nikolas Zimmermann <zimmermann@kde.org>
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

#ifndef SVGPathSegClosePath_h
#define SVGPathSegClosePath_h

#include "core/svg/SVGPathSeg.h"

namespace blink {

class SVGPathSegClosePath final : public SVGPathSeg {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<SVGPathSegClosePath> create(SVGPathElement* element)
    {
        return adoptRefWillBeNoop(new SVGPathSegClosePath(element));
    }

    PassRefPtrWillBeRawPtr<SVGPathSeg> clone() override
    {
        return adoptRefWillBeNoop(new SVGPathSegClosePath(nullptr));
    }

private:
    SVGPathSegClosePath(SVGPathElement* element)
        : SVGPathSeg(element) { }

    unsigned short pathSegType() const override { return PATHSEG_CLOSEPATH; }
    String pathSegTypeAsLetter() const override { return "Z"; }
};

} // namespace blink

#endif // SVGPathSegClosePath_h
