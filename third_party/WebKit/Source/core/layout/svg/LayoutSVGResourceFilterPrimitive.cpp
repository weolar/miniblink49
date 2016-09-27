/*
 * Copyright (C) 2010 University of Szeged
 * Copyright (C) 2010 Zoltan Herczeg
 * Copyright (C) 2011 Renata Hodovan (reni@webkit.org)
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY UNIVERSITY OF SZEGED ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL UNIVERSITY OF SZEGED OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#include "core/layout/svg/LayoutSVGResourceFilterPrimitive.h"

namespace blink {

void LayoutSVGResourceFilterPrimitive::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    LayoutSVGHiddenContainer::styleDidChange(diff, oldStyle);

    LayoutObject* filter = parent();
    if (!filter)
        return;
    ASSERT(filter->isSVGResourceFilter());

    if (!oldStyle)
        return;

    const SVGComputedStyle& newStyle = this->style()->svgStyle();
    ASSERT(element());
    if (isSVGFEFloodElement(*element())) {
        if (newStyle.floodColor() != oldStyle->svgStyle().floodColor())
            toLayoutSVGResourceFilter(filter)->primitiveAttributeChanged(this, SVGNames::flood_colorAttr);
        if (newStyle.floodOpacity() != oldStyle->svgStyle().floodOpacity())
            toLayoutSVGResourceFilter(filter)->primitiveAttributeChanged(this, SVGNames::flood_opacityAttr);
    } else if (isSVGFEDiffuseLightingElement(*element()) || isSVGFESpecularLightingElement(*element())) {
        if (newStyle.lightingColor() != oldStyle->svgStyle().lightingColor())
            toLayoutSVGResourceFilter(filter)->primitiveAttributeChanged(this, SVGNames::lighting_colorAttr);
    }
}

} // namespace blink
