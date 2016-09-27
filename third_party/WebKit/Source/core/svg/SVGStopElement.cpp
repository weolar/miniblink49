/*
 * Copyright (C) 2004, 2005, 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
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
#include "core/svg/SVGStopElement.h"

#include "core/layout/svg/LayoutSVGGradientStop.h"

namespace blink {

inline SVGStopElement::SVGStopElement(Document& document)
    : SVGElement(SVGNames::stopTag, document)
    , m_offset(SVGAnimatedNumber::create(this, SVGNames::offsetAttr, SVGNumberAcceptPercentage::create()))
{
    addToPropertyMap(m_offset);
}

DEFINE_TRACE(SVGStopElement)
{
    visitor->trace(m_offset);
    SVGElement::trace(visitor);
}

DEFINE_NODE_FACTORY(SVGStopElement)

void SVGStopElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::offsetAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);

        if (layoutObject())
            markForLayoutAndParentResourceInvalidation(layoutObject());
        return;
    }

    SVGElement::svgAttributeChanged(attrName);
}

LayoutObject* SVGStopElement::createLayoutObject(const ComputedStyle&)
{
    return new LayoutSVGGradientStop(this);
}

bool SVGStopElement::layoutObjectIsNeeded(const ComputedStyle&)
{
    return true;
}

Color SVGStopElement::stopColorIncludingOpacity() const
{
    const ComputedStyle* style = layoutObject() ? layoutObject()->style() : nullptr;
    // FIXME: This check for null style exists to address Bug WK 90814, a rare crash condition in
    // which the layoutObject or style is null. This entire class is scheduled for removal (Bug WK 86941)
    // and we will tolerate this null check until then.
    if (!style)
        return Color(Color::transparent); // Transparent black.

    const SVGComputedStyle& svgStyle = style->svgStyle();
    return svgStyle.stopColor().combineWithAlpha(svgStyle.stopOpacity());
}

} // namespace blink
