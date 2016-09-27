/*
 * Copyright (C) 2004, 2005, 2006, 2008 Nikolas Zimmermann <zimmermann@kde.org>
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
#include "core/svg/SVGCircleElement.h"

#include "core/layout/svg/LayoutSVGEllipse.h"
#include "core/svg/SVGLength.h"

namespace blink {

inline SVGCircleElement::SVGCircleElement(Document& document)
    : SVGGeometryElement(SVGNames::circleTag, document)
    , m_cx(SVGAnimatedLength::create(this, SVGNames::cxAttr, SVGLength::create(SVGLengthMode::Width), AllowNegativeLengths))
    , m_cy(SVGAnimatedLength::create(this, SVGNames::cyAttr, SVGLength::create(SVGLengthMode::Height), AllowNegativeLengths))
    , m_r(SVGAnimatedLength::create(this, SVGNames::rAttr, SVGLength::create(SVGLengthMode::Other), ForbidNegativeLengths))
{
    addToPropertyMap(m_cx);
    addToPropertyMap(m_cy);
    addToPropertyMap(m_r);
}

DEFINE_TRACE(SVGCircleElement)
{
    visitor->trace(m_cx);
    visitor->trace(m_cy);
    visitor->trace(m_r);
    SVGGeometryElement::trace(visitor);
}

DEFINE_NODE_FACTORY(SVGCircleElement)

Path SVGCircleElement::asPath() const
{
    Path path;

    SVGLengthContext lengthContext(this);
    ASSERT(layoutObject());
    const ComputedStyle& style = layoutObject()->styleRef();
    const SVGComputedStyle& svgStyle = style.svgStyle();

    float r = lengthContext.valueForLength(svgStyle.r(), style, SVGLengthMode::Other);
    if (r > 0) {
        path.addEllipse(FloatRect(
            lengthContext.valueForLength(svgStyle.cx(), style, SVGLengthMode::Width) - r,
            lengthContext.valueForLength(svgStyle.cy(), style, SVGLengthMode::Height) - r,
            r * 2, r * 2));
    }

    return path;
}

bool SVGCircleElement::isPresentationAttribute(const QualifiedName& attrName) const
{
    if (attrName == SVGNames::cxAttr || attrName == SVGNames::cyAttr
        || attrName == SVGNames::rAttr)
        return true;
    return SVGGeometryElement::isPresentationAttribute(attrName);
}

bool SVGCircleElement::isPresentationAttributeWithSVGDOM(const QualifiedName& attrName) const
{
    if (attrName == SVGNames::cxAttr || attrName == SVGNames::cyAttr
        || attrName == SVGNames::rAttr)
        return true;
    return SVGGeometryElement::isPresentationAttributeWithSVGDOM(attrName);
}

void SVGCircleElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    RefPtrWillBeRawPtr<SVGAnimatedPropertyBase> property = propertyFromAttribute(name);
    if (property == m_cx)
        addSVGLengthPropertyToPresentationAttributeStyle(style, CSSPropertyCx, *m_cx->currentValue());
    else if (property == m_cy)
        addSVGLengthPropertyToPresentationAttributeStyle(style, CSSPropertyCy, *m_cy->currentValue());
    else if (property == m_r)
        addSVGLengthPropertyToPresentationAttributeStyle(style, CSSPropertyR, *m_r->currentValue());
    else
        SVGGeometryElement::collectStyleForPresentationAttribute(name, value, style);
}

void SVGCircleElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::rAttr
        || attrName == SVGNames::cxAttr
        || attrName == SVGNames::cyAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);

        invalidateSVGPresentationAttributeStyle();
        setNeedsStyleRecalc(LocalStyleChange,
            StyleChangeReasonForTracing::fromAttribute(attrName));
        updateRelativeLengthsInformation();

        LayoutSVGShape* layoutObject = toLayoutSVGShape(this->layoutObject());
        if (!layoutObject)
            return;

        layoutObject->setNeedsShapeUpdate();
        markForLayoutAndParentResourceInvalidation(layoutObject);
        return;
    }

    SVGGraphicsElement::svgAttributeChanged(attrName);
}

bool SVGCircleElement::selfHasRelativeLengths() const
{
    return m_cx->currentValue()->isRelative()
        || m_cy->currentValue()->isRelative()
        || m_r->currentValue()->isRelative();
}

LayoutObject* SVGCircleElement::createLayoutObject(const ComputedStyle&)
{
    return new LayoutSVGEllipse(this);
}

}
