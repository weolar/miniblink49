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

#include "config.h"

#include "core/layout/svg/LayoutSVGGradientStop.h"

#include "core/layout/svg/LayoutSVGResourceContainer.h"
#include "core/svg/SVGGradientElement.h"
#include "core/svg/SVGStopElement.h"

namespace blink {

LayoutSVGGradientStop::LayoutSVGGradientStop(SVGStopElement* element)
    : LayoutObject(element)
{
}

LayoutSVGGradientStop::~LayoutSVGGradientStop()
{
}

void LayoutSVGGradientStop::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    LayoutObject::styleDidChange(diff, oldStyle);
    if (!diff.hasDifference())
        return;

    // <stop> elements should only be allowed to make layoutObjects under gradient elements
    // but I can imagine a few cases we might not be catching, so let's not crash if our parent isn't a gradient.
    SVGGradientElement* gradient = gradientElement();
    if (!gradient)
        return;

    LayoutObject* layoutObject = gradient->layoutObject();
    if (!layoutObject)
        return;

    LayoutSVGResourceContainer* container = toLayoutSVGResourceContainer(layoutObject);
    container->removeAllClientsFromCache();
}

void LayoutSVGGradientStop::layout()
{
    clearNeedsLayout();
}

SVGGradientElement* LayoutSVGGradientStop::gradientElement() const
{
    ContainerNode* parentNode = node()->parentNode();
    ASSERT(parentNode);
    return isSVGGradientElement(*parentNode) ? toSVGGradientElement(parentNode) : 0;
}

}
