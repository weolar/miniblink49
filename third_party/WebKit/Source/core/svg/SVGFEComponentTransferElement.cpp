/*
 * Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
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

#include "core/svg/SVGFEComponentTransferElement.h"

#include "core/SVGNames.h"
#include "core/dom/ElementTraversal.h"
#include "core/svg/SVGFEFuncAElement.h"
#include "core/svg/SVGFEFuncBElement.h"
#include "core/svg/SVGFEFuncGElement.h"
#include "core/svg/SVGFEFuncRElement.h"
#include "core/svg/graphics/filters/SVGFilterBuilder.h"
#include "platform/graphics/filters/FilterEffect.h"

namespace blink {

inline SVGFEComponentTransferElement::SVGFEComponentTransferElement(Document& document)
    : SVGFilterPrimitiveStandardAttributes(SVGNames::feComponentTransferTag, document)
    , m_in1(SVGAnimatedString::create(this, SVGNames::inAttr, SVGString::create()))
{
    addToPropertyMap(m_in1);
}

DEFINE_TRACE(SVGFEComponentTransferElement)
{
    visitor->trace(m_in1);
    SVGFilterPrimitiveStandardAttributes::trace(visitor);
}

DEFINE_NODE_FACTORY(SVGFEComponentTransferElement)

void SVGFEComponentTransferElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::inAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        invalidate();
        return;
    }

    SVGFilterPrimitiveStandardAttributes::svgAttributeChanged(attrName);
}

PassRefPtrWillBeRawPtr<FilterEffect> SVGFEComponentTransferElement::build(SVGFilterBuilder* filterBuilder, Filter* filter)
{
    FilterEffect* input1 = filterBuilder->getEffectById(AtomicString(m_in1->currentValue()->value()));

    if (!input1)
        return nullptr;

    ComponentTransferFunction red;
    ComponentTransferFunction green;
    ComponentTransferFunction blue;
    ComponentTransferFunction alpha;

    for (SVGElement* element = Traversal<SVGElement>::firstChild(*this); element; element = Traversal<SVGElement>::nextSibling(*element)) {
        if (isSVGFEFuncRElement(*element))
            red = toSVGFEFuncRElement(*element).transferFunction();
        else if (isSVGFEFuncGElement(*element))
            green = toSVGFEFuncGElement(*element).transferFunction();
        else if (isSVGFEFuncBElement(*element))
            blue = toSVGFEFuncBElement(*element).transferFunction();
        else if (isSVGFEFuncAElement(*element))
            alpha = toSVGFEFuncAElement(*element).transferFunction();
    }

    RefPtrWillBeRawPtr<FilterEffect> effect = FEComponentTransfer::create(filter, red, green, blue, alpha);
    effect->inputEffects().append(input1);
    return effect.release();
}

}
