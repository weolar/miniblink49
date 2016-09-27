/*
 * Copyright (C) 2004, 2005, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
 * Copyright (C) 2014 Google, Inc.
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

#include "core/svg/SVGGraphicsElement.h"

#include "core/SVGNames.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/layout/svg/LayoutSVGPath.h"
#include "core/svg/SVGElementRareData.h"
#include "platform/transforms/AffineTransform.h"

namespace blink {

SVGGraphicsElement::SVGGraphicsElement(const QualifiedName& tagName, Document& document, ConstructionType constructionType)
    : SVGElement(tagName, document, constructionType)
    , SVGTests(this)
    , m_transform(SVGAnimatedTransformList::create(this, SVGNames::transformAttr, SVGTransformList::create()))
{
    addToPropertyMap(m_transform);
}

SVGGraphicsElement::~SVGGraphicsElement()
{
}

DEFINE_TRACE(SVGGraphicsElement)
{
    visitor->trace(m_transform);
    SVGElement::trace(visitor);
    SVGTests::trace(visitor);
}

PassRefPtrWillBeRawPtr<SVGMatrixTearOff> SVGGraphicsElement::getTransformToElement(SVGElement* target, ExceptionState& exceptionState)
{
    AffineTransform ctm = getCTM(AllowStyleUpdate);

    if (target && target->isSVGGraphicsElement()) {
        AffineTransform targetCTM = toSVGGraphicsElement(target)->getCTM(AllowStyleUpdate);
        if (!targetCTM.isInvertible()) {
            exceptionState.throwDOMException(InvalidStateError, "The target transformation is not invertable.");
            return nullptr;
        }
        ctm = targetCTM.inverse() * ctm;
    }

    return SVGMatrixTearOff::create(ctm);
}

static bool isViewportElement(const Element& element)
{
    return (isSVGSVGElement(element)
        || isSVGSymbolElement(element)
        || isSVGForeignObjectElement(element)
        || isSVGImageElement(element));
}

AffineTransform SVGGraphicsElement::computeCTM(SVGElement::CTMScope mode,
    SVGGraphicsElement::StyleUpdateStrategy styleUpdateStrategy, const SVGGraphicsElement* ancestor) const
{
    if (styleUpdateStrategy == AllowStyleUpdate)
        document().updateLayoutIgnorePendingStylesheets();

    AffineTransform ctm;
    bool done = false;

    for (const Element* currentElement = this; currentElement && !done;
        currentElement = currentElement->parentOrShadowHostElement()) {
        if (!currentElement->isSVGElement())
            break;

        ctm = toSVGElement(currentElement)->localCoordinateSpaceTransform(mode).multiply(ctm);

        switch (mode) {
        case NearestViewportScope:
            // Stop at the nearest viewport ancestor.
            done = currentElement != this && isViewportElement(*currentElement);
            break;
        case AncestorScope:
            // Stop at the designated ancestor.
            done = currentElement == ancestor;
            break;
        default:
            ASSERT(mode == ScreenScope);
            break;
        }
    }

    return ctm;
}

AffineTransform SVGGraphicsElement::getCTM(StyleUpdateStrategy styleUpdateStrategy)
{
    return computeCTM(NearestViewportScope, styleUpdateStrategy);
}

AffineTransform SVGGraphicsElement::getScreenCTM(StyleUpdateStrategy styleUpdateStrategy)
{
    return computeCTM(ScreenScope, styleUpdateStrategy);
}

PassRefPtrWillBeRawPtr<SVGMatrixTearOff> SVGGraphicsElement::getCTMFromJavascript()
{
    return SVGMatrixTearOff::create(getCTM());
}

PassRefPtrWillBeRawPtr<SVGMatrixTearOff> SVGGraphicsElement::getScreenCTMFromJavascript()
{
    return SVGMatrixTearOff::create(getScreenCTM());
}

bool SVGGraphicsElement::hasAnimatedLocalTransform() const
{
    const ComputedStyle* style = layoutObject() ? layoutObject()->style() : nullptr;

    // Each of these is used in SVGGraphicsElement::calculateAnimatedLocalTransform to create an animated local transform.
    return (style && style->hasTransform()) || !m_transform->currentValue()->isEmpty() || hasSVGRareData();
}

AffineTransform SVGGraphicsElement::calculateAnimatedLocalTransform() const
{
    AffineTransform matrix;
    const ComputedStyle* style = layoutObject() ? layoutObject()->style() : nullptr;

    // If CSS property was set, use that, otherwise fallback to attribute (if set).
    if (style && style->hasTransform()) {
        TransformationMatrix transform;
        float zoom = style->effectiveZoom();

        // SVGTextElements need special handling for the text positioning code.
        if (isSVGTextElement(this)) {
            // Do not take into account SVG's zoom rules, transform-origin, or percentage values.
            style->applyTransform(transform, LayoutSize(0, 0), ComputedStyle::ExcludeTransformOrigin, ComputedStyle::IncludeMotionPath, ComputedStyle::IncludeIndependentTransformProperties);
        } else {
            // CSS transforms operate with pre-scaled lengths. To make this work with SVG
            // (which applies the zoom factor globally, at the root level) we
            //
            //   * pre-scale the bounding box (to bring it into the same space as the other CSS values)
            //   * invert the zoom factor (to effectively compute the CSS transform under a 1.0 zoom)
            //
            // Note: objectBoundingBox is an emptyRect for elements like pattern or clipPath.
            // See the "Object bounding box units" section of http://dev.w3.org/csswg/css3-transforms/
            if (zoom != 1) {
                FloatRect scaledBBox = layoutObject()->objectBoundingBox();
                scaledBBox.scale(zoom);
                transform.scale(1 / zoom);
                style->applyTransform(transform, scaledBBox, ComputedStyle::IncludeTransformOrigin, ComputedStyle::IncludeMotionPath, ComputedStyle::IncludeIndependentTransformProperties);
                transform.scale(zoom);
            } else {
                style->applyTransform(transform, layoutObject()->objectBoundingBox(), ComputedStyle::IncludeTransformOrigin, ComputedStyle::IncludeMotionPath, ComputedStyle::IncludeIndependentTransformProperties);
            }
        }

        // Flatten any 3D transform.
        matrix = transform.toAffineTransform();
    } else {
        m_transform->currentValue()->concatenate(matrix);
    }

    if (hasSVGRareData())
        return *svgRareData()->animateMotionTransform() * matrix;
    return matrix;
}

AffineTransform* SVGGraphicsElement::animateMotionTransform()
{
    return ensureSVGRareData()->animateMotionTransform();
}

void SVGGraphicsElement::svgAttributeChanged(const QualifiedName& attrName)
{
    // Reattach so the isValid() check will be run again during layoutObject creation.
    if (SVGTests::isKnownAttribute(attrName)) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        lazyReattachIfAttached();
        return;
    }

    if (attrName == SVGNames::transformAttr) {
        LayoutObject* object = layoutObject();
        if (!object)
            return;

        SVGElement::InvalidationGuard invalidationGuard(this);
        object->setNeedsTransformUpdate();
        markForLayoutAndParentResourceInvalidation(object);
        return;
    }

    SVGElement::svgAttributeChanged(attrName);
}

SVGElement* SVGGraphicsElement::nearestViewportElement() const
{
    for (Element* current = parentOrShadowHostElement(); current; current = current->parentOrShadowHostElement()) {
        if (isViewportElement(*current))
            return toSVGElement(current);
    }

    return nullptr;
}

SVGElement* SVGGraphicsElement::farthestViewportElement() const
{
    SVGElement* farthest = 0;
    for (Element* current = parentOrShadowHostElement(); current; current = current->parentOrShadowHostElement()) {
        if (isViewportElement(*current))
            farthest = toSVGElement(current);
    }
    return farthest;
}

FloatRect SVGGraphicsElement::getBBox()
{
    document().updateLayoutIgnorePendingStylesheets();

    // FIXME: Eventually we should support getBBox for detached elements.
    if (!layoutObject())
        return FloatRect();

    return layoutObject()->objectBoundingBox();
}

PassRefPtrWillBeRawPtr<SVGRectTearOff> SVGGraphicsElement::getBBoxFromJavascript()
{
    return SVGRectTearOff::create(SVGRect::create(getBBox()), 0, PropertyIsNotAnimVal);
}

}
