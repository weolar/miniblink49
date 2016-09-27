/*
 * Copyright (C) 2006 Nikolas Zimmermann <zimmermann@kde.org>
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

#include "config.h"

#include "core/layout/svg/LayoutSVGResourceLinearGradient.h"

#include "core/svg/SVGLinearGradientElement.h"

namespace blink {

LayoutSVGResourceLinearGradient::LayoutSVGResourceLinearGradient(SVGLinearGradientElement* node)
    : LayoutSVGResourceGradient(node)
#if ENABLE(OILPAN)
    , m_attributesWrapper(LinearGradientAttributesWrapper::create())
#endif
{
}

LayoutSVGResourceLinearGradient::~LayoutSVGResourceLinearGradient()
{
}

bool LayoutSVGResourceLinearGradient::collectGradientAttributes(SVGGradientElement* gradientElement)
{
#if ENABLE(OILPAN)
    m_attributesWrapper->set(LinearGradientAttributes());
#else
    m_attributes = LinearGradientAttributes();
#endif
    return toSVGLinearGradientElement(gradientElement)->collectGradientAttributes(mutableAttributes());
}

FloatPoint LayoutSVGResourceLinearGradient::startPoint(const LinearGradientAttributes& attributes) const
{
    return SVGLengthContext::resolvePoint(element(), attributes.gradientUnits(), *attributes.x1(), *attributes.y1());
}

FloatPoint LayoutSVGResourceLinearGradient::endPoint(const LinearGradientAttributes& attributes) const
{
    return SVGLengthContext::resolvePoint(element(), attributes.gradientUnits(), *attributes.x2(), *attributes.y2());
}

void LayoutSVGResourceLinearGradient::buildGradient(GradientData* gradientData) const
{
    const LinearGradientAttributes& attributes = this->attributes();
    gradientData->gradient = Gradient::create(startPoint(attributes), endPoint(attributes));
    gradientData->gradient->setSpreadMethod(platformSpreadMethodFromSVGType(attributes.spreadMethod()));
    addStops(gradientData, attributes.stops());
}

}
