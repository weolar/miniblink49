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
#include "core/svg/SVGRectElement.h"

#include "core/layout/svg/LayoutSVGRect.h"
#include "core/svg/SVGLength.h"

namespace blink {

inline SVGRectElement::SVGRectElement(Document& document)
    : SVGGeometryElement(SVGNames::rectTag, document)
    , m_x(SVGAnimatedLength::create(this, SVGNames::xAttr, SVGLength::create(SVGLengthMode::Width), AllowNegativeLengths))
    , m_y(SVGAnimatedLength::create(this, SVGNames::yAttr, SVGLength::create(SVGLengthMode::Height), AllowNegativeLengths))
    , m_width(SVGAnimatedLength::create(this, SVGNames::widthAttr, SVGLength::create(SVGLengthMode::Width), ForbidNegativeLengths))
    , m_height(SVGAnimatedLength::create(this, SVGNames::heightAttr, SVGLength::create(SVGLengthMode::Height), ForbidNegativeLengths))
    , m_rx(SVGAnimatedLength::create(this, SVGNames::rxAttr, SVGLength::create(SVGLengthMode::Width), ForbidNegativeLengths))
    , m_ry(SVGAnimatedLength::create(this, SVGNames::ryAttr, SVGLength::create(SVGLengthMode::Height), ForbidNegativeLengths))
{
    addToPropertyMap(m_x);
    addToPropertyMap(m_y);
    addToPropertyMap(m_width);
    addToPropertyMap(m_height);
    addToPropertyMap(m_rx);
    addToPropertyMap(m_ry);
}

DEFINE_TRACE(SVGRectElement)
{
    visitor->trace(m_x);
    visitor->trace(m_y);
    visitor->trace(m_width);
    visitor->trace(m_height);
    visitor->trace(m_rx);
    visitor->trace(m_ry);
    SVGGeometryElement::trace(visitor);
}

DEFINE_NODE_FACTORY(SVGRectElement)

Path SVGRectElement::asPath() const
{
    Path path;

    SVGLengthContext lengthContext(this);
    ASSERT(layoutObject());
    const ComputedStyle& style = layoutObject()->styleRef();
    const SVGComputedStyle& svgStyle = style.svgStyle();

    float width = lengthContext.valueForLength(style.width(), style, SVGLengthMode::Width);
    if (width < 0)
        return path;
    float height = lengthContext.valueForLength(style.height(), style, SVGLengthMode::Height);
    if (height < 0)
        return path;
    if (!width && !height)
        return path;

    float x = lengthContext.valueForLength(svgStyle.x(), style, SVGLengthMode::Width);
    float y = lengthContext.valueForLength(svgStyle.y(), style, SVGLengthMode::Height);
    float rx = lengthContext.valueForLength(svgStyle.rx(), style, SVGLengthMode::Width);
    float ry = lengthContext.valueForLength(svgStyle.ry(), style, SVGLengthMode::Height);
    bool hasRx = rx > 0;
    bool hasRy = ry > 0;
    if (hasRx || hasRy) {
        if (!hasRx)
            rx = ry;
        else if (!hasRy)
            ry = rx;

        path.addRoundedRect(FloatRect(x, y, width, height), FloatSize(rx, ry));
        return path;
    }

    path.addRect(FloatRect(x, y, width, height));
    return path;
}

bool SVGRectElement::isPresentationAttribute(const QualifiedName& attrName) const
{
    if (attrName == SVGNames::xAttr || attrName == SVGNames::yAttr
        || attrName == SVGNames::widthAttr || attrName == SVGNames::heightAttr
        || attrName == SVGNames::rxAttr || attrName == SVGNames::ryAttr)
        return true;
    return SVGGeometryElement::isPresentationAttribute(attrName);
}

bool SVGRectElement::isPresentationAttributeWithSVGDOM(const QualifiedName& attrName) const
{
    if (attrName == SVGNames::xAttr || attrName == SVGNames::yAttr
        || attrName == SVGNames::widthAttr || attrName == SVGNames::heightAttr
        || attrName == SVGNames::rxAttr || attrName == SVGNames::ryAttr)
        return true;
    return SVGGeometryElement::isPresentationAttributeWithSVGDOM(attrName);
}

void SVGRectElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    RefPtrWillBeRawPtr<SVGAnimatedPropertyBase> property = propertyFromAttribute(name);
    if (property == m_x)
        addSVGLengthPropertyToPresentationAttributeStyle(style, CSSPropertyX, *m_x->currentValue());
    else if (property == m_y)
        addSVGLengthPropertyToPresentationAttributeStyle(style, CSSPropertyY, *m_y->currentValue());
    else if (property == m_width)
        addSVGLengthPropertyToPresentationAttributeStyle(style, CSSPropertyWidth, *m_width->currentValue());
    else if (property == m_height)
        addSVGLengthPropertyToPresentationAttributeStyle(style, CSSPropertyHeight, *m_height->currentValue());
    else if (property == m_rx)
        addSVGLengthPropertyToPresentationAttributeStyle(style, CSSPropertyRx, *m_rx->currentValue());
    else if (property == m_ry)
        addSVGLengthPropertyToPresentationAttributeStyle(style, CSSPropertyRy, *m_ry->currentValue());
    else
        SVGGeometryElement::collectStyleForPresentationAttribute(name, value, style);
}

void SVGRectElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::xAttr || attrName == SVGNames::yAttr
        || attrName == SVGNames::widthAttr || attrName == SVGNames::heightAttr
        || attrName == SVGNames::rxAttr || attrName == SVGNames::ryAttr) {
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

    SVGGeometryElement::svgAttributeChanged(attrName);
}

bool SVGRectElement::selfHasRelativeLengths() const
{
    return m_x->currentValue()->isRelative()
        || m_y->currentValue()->isRelative()
        || m_width->currentValue()->isRelative()
        || m_height->currentValue()->isRelative()
        || m_rx->currentValue()->isRelative()
        || m_ry->currentValue()->isRelative();
}

LayoutObject* SVGRectElement::createLayoutObject(const ComputedStyle&)
{
    return new LayoutSVGRect(this);
}

} // namespace blink
