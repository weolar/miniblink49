/*
 * Copyright (C) 2004, 2005 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2007 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2008 Apple Inc. All rights reserved.
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

#include "core/svg/SVGAnimateTransformElement.h"

#include "core/SVGNames.h"
#include "core/svg/SVGParserUtilities.h"

namespace blink {

inline SVGAnimateTransformElement::SVGAnimateTransformElement(Document& document)
    : SVGAnimateElement(SVGNames::animateTransformTag, document)
    , m_type(SVG_TRANSFORM_UNKNOWN)
{
}

DEFINE_NODE_FACTORY(SVGAnimateTransformElement)

bool SVGAnimateTransformElement::hasValidAttributeType()
{
    SVGElement* targetElement = this->targetElement();
    if (!targetElement)
        return false;

    if (attributeType() == AttributeTypeCSS)
        return false;

    return animatedPropertyType() == AnimatedTransformList;
}

void SVGAnimateTransformElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == SVGNames::typeAttr) {
        m_type = parseTransformType(value);
        if (m_type == SVG_TRANSFORM_MATRIX)
            m_type = SVG_TRANSFORM_UNKNOWN;
        return;
    }

    SVGAnimateElement::parseAttribute(name, value);
}

}
