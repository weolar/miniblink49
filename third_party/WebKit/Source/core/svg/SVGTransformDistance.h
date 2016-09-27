/*
 * Copyright (C) 2007 Eric Seidel <eric@webkit.org>
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

#ifndef SVGTransformDistance_h
#define SVGTransformDistance_h

#include "core/svg/SVGTransform.h"

namespace blink {

class AffineTransform;

class SVGTransformDistance {
public:
    SVGTransformDistance();
    SVGTransformDistance(PassRefPtrWillBeRawPtr<SVGTransform> fromTransform, PassRefPtrWillBeRawPtr<SVGTransform> toTransform);

    SVGTransformDistance scaledDistance(float scaleFactor) const;
    PassRefPtrWillBeRawPtr<SVGTransform> addToSVGTransform(PassRefPtrWillBeRawPtr<SVGTransform>) const;

    static PassRefPtrWillBeRawPtr<SVGTransform> addSVGTransforms(PassRefPtrWillBeRawPtr<SVGTransform>, PassRefPtrWillBeRawPtr<SVGTransform>, unsigned repeatCount = 1);

    float distance() const;

private:
    SVGTransformDistance(SVGTransformType, float angle, float cx, float cy, const AffineTransform&);

    SVGTransformType m_transformType;
    float m_angle;
    float m_cx;
    float m_cy;
    AffineTransform m_transform; // for storing scale, translation or matrix transforms
};
}

#endif // SVGTransformDistance_h
