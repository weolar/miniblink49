/*
 * Copyright (C) 2004, 2005, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007, 2008 Rob Buis <buis@kde.org>
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

#include "core/svg/SVGTextPositioningElement.h"

#include "core/SVGNames.h"
#include "core/layout/svg/LayoutSVGText.h"
#include "core/svg/SVGLengthList.h"
#include "core/svg/SVGNumberList.h"

namespace blink {

SVGTextPositioningElement::SVGTextPositioningElement(const QualifiedName& tagName, Document& document)
    : SVGTextContentElement(tagName, document)
    , m_x(SVGAnimatedLengthList::create(this, SVGNames::xAttr, SVGLengthList::create(SVGLengthMode::Width)))
    , m_y(SVGAnimatedLengthList::create(this, SVGNames::yAttr, SVGLengthList::create(SVGLengthMode::Height)))
    , m_dx(SVGAnimatedLengthList::create(this, SVGNames::dxAttr, SVGLengthList::create(SVGLengthMode::Width)))
    , m_dy(SVGAnimatedLengthList::create(this, SVGNames::dyAttr, SVGLengthList::create(SVGLengthMode::Height)))
    , m_rotate(SVGAnimatedNumberList::create(this, SVGNames::rotateAttr, SVGNumberList::create()))
{
    addToPropertyMap(m_x);
    addToPropertyMap(m_y);
    addToPropertyMap(m_dx);
    addToPropertyMap(m_dy);
    addToPropertyMap(m_rotate);
}

DEFINE_TRACE(SVGTextPositioningElement)
{
    visitor->trace(m_x);
    visitor->trace(m_y);
    visitor->trace(m_dx);
    visitor->trace(m_dy);
    visitor->trace(m_rotate);
    SVGTextContentElement::trace(visitor);
}

void SVGTextPositioningElement::svgAttributeChanged(const QualifiedName& attrName)
{
    bool updateRelativeLengths = attrName == SVGNames::xAttr
                              || attrName == SVGNames::yAttr
                              || attrName == SVGNames::dxAttr
                              || attrName == SVGNames::dyAttr;

    if (updateRelativeLengths)
        updateRelativeLengthsInformation();

    if (updateRelativeLengths || attrName == SVGNames::rotateAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);

        LayoutObject* layoutObject = this->layoutObject();
        if (!layoutObject)
            return;

        if (LayoutSVGText* textLayoutObject = LayoutSVGText::locateLayoutSVGTextAncestor(layoutObject))
            textLayoutObject->setNeedsPositioningValuesUpdate();
        markForLayoutAndParentResourceInvalidation(layoutObject);
        return;
    }

    SVGTextContentElement::svgAttributeChanged(attrName);
}

SVGTextPositioningElement* SVGTextPositioningElement::elementFromLayoutObject(LayoutObject& layoutObject)
{
    if (!layoutObject.isSVGText() && !layoutObject.isSVGInline())
        return nullptr;

    Node* node = layoutObject.node();
    ASSERT(node);
    ASSERT(node->isSVGElement());

    return isSVGTextPositioningElement(*node) ? toSVGTextPositioningElement(node) : 0;
}

}
