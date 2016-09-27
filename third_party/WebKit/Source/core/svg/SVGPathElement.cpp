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
#include "core/svg/SVGPathElement.h"

#include "core/layout/svg/LayoutSVGPath.h"
#include "core/svg/SVGDocumentExtensions.h"
#include "core/svg/SVGMPathElement.h"
#include "core/svg/SVGPathSegArcAbs.h"
#include "core/svg/SVGPathSegArcRel.h"
#include "core/svg/SVGPathSegClosePath.h"
#include "core/svg/SVGPathSegCurvetoCubicAbs.h"
#include "core/svg/SVGPathSegCurvetoCubicRel.h"
#include "core/svg/SVGPathSegCurvetoCubicSmoothAbs.h"
#include "core/svg/SVGPathSegCurvetoCubicSmoothRel.h"
#include "core/svg/SVGPathSegCurvetoQuadraticAbs.h"
#include "core/svg/SVGPathSegCurvetoQuadraticRel.h"
#include "core/svg/SVGPathSegCurvetoQuadraticSmoothAbs.h"
#include "core/svg/SVGPathSegCurvetoQuadraticSmoothRel.h"
#include "core/svg/SVGPathSegLinetoAbs.h"
#include "core/svg/SVGPathSegLinetoHorizontalAbs.h"
#include "core/svg/SVGPathSegLinetoHorizontalRel.h"
#include "core/svg/SVGPathSegLinetoRel.h"
#include "core/svg/SVGPathSegLinetoVerticalAbs.h"
#include "core/svg/SVGPathSegLinetoVerticalRel.h"
#include "core/svg/SVGPathSegMovetoAbs.h"
#include "core/svg/SVGPathSegMovetoRel.h"
#include "core/svg/SVGPathUtilities.h"
#include "core/svg/SVGPointTearOff.h"

namespace blink {

class SVGAnimatedPathLength final : public SVGAnimatedNumber {
public:
    static PassRefPtrWillBeRawPtr<SVGAnimatedPathLength> create(SVGPathElement* contextElement)
    {
        return adoptRefWillBeNoop(new SVGAnimatedPathLength(contextElement));
    }

    void setBaseValueAsString(const String& value, SVGParsingError& parseError) override
    {
        SVGAnimatedNumber::setBaseValueAsString(value, parseError);

        ASSERT(contextElement());
        if (parseError == NoError && baseValue()->value() < 0)
            contextElement()->document().accessSVGExtensions().reportError("A negative value for path attribute <pathLength> is not allowed");
    }

private:
    explicit SVGAnimatedPathLength(SVGPathElement* contextElement)
        : SVGAnimatedNumber(contextElement, SVGNames::pathLengthAttr, SVGNumber::create())
    {
    }
};

inline SVGPathElement::SVGPathElement(Document& document)
    : SVGGeometryElement(SVGNames::pathTag, document)
    , m_pathLength(SVGAnimatedPathLength::create(this))
    , m_pathSegList(SVGAnimatedPath::create(this, SVGNames::dAttr))
{
    addToPropertyMap(m_pathLength);
    addToPropertyMap(m_pathSegList);
}

DEFINE_TRACE(SVGPathElement)
{
    visitor->trace(m_pathLength);
    visitor->trace(m_pathSegList);
    SVGGeometryElement::trace(visitor);
}

DEFINE_NODE_FACTORY(SVGPathElement)

Path SVGPathElement::asPath() const
{
    Path path;
    buildPathFromByteStream(*pathByteStream(), path);
    return path;
}

float SVGPathElement::getTotalLength()
{
    return getTotalLengthOfSVGPathByteStream(*pathByteStream());
}

PassRefPtrWillBeRawPtr<SVGPointTearOff> SVGPathElement::getPointAtLength(float length)
{
    FloatPoint point = getPointAtLengthOfSVGPathByteStream(*pathByteStream(), length);
    return SVGPointTearOff::create(SVGPoint::create(point), 0, PropertyIsNotAnimVal);
}

unsigned SVGPathElement::getPathSegAtLength(float length)
{
    return getSVGPathSegAtLengthFromSVGPathByteStream(*pathByteStream(), length);
}

PassRefPtrWillBeRawPtr<SVGPathSegClosePath> SVGPathElement::createSVGPathSegClosePath()
{
    return SVGPathSegClosePath::create(0);
}

PassRefPtrWillBeRawPtr<SVGPathSegMovetoAbs> SVGPathElement::createSVGPathSegMovetoAbs(float x, float y)
{
    return SVGPathSegMovetoAbs::create(0, x, y);
}

PassRefPtrWillBeRawPtr<SVGPathSegMovetoRel> SVGPathElement::createSVGPathSegMovetoRel(float x, float y)
{
    return SVGPathSegMovetoRel::create(0, x, y);
}

PassRefPtrWillBeRawPtr<SVGPathSegLinetoAbs> SVGPathElement::createSVGPathSegLinetoAbs(float x, float y)
{
    return SVGPathSegLinetoAbs::create(0, x, y);
}

PassRefPtrWillBeRawPtr<SVGPathSegLinetoRel> SVGPathElement::createSVGPathSegLinetoRel(float x, float y)
{
    return SVGPathSegLinetoRel::create(0, x, y);
}

PassRefPtrWillBeRawPtr<SVGPathSegCurvetoCubicAbs> SVGPathElement::createSVGPathSegCurvetoCubicAbs(float x, float y, float x1, float y1, float x2, float y2)
{
    return SVGPathSegCurvetoCubicAbs::create(0, x, y, x1, y1, x2, y2);
}

PassRefPtrWillBeRawPtr<SVGPathSegCurvetoCubicRel> SVGPathElement::createSVGPathSegCurvetoCubicRel(float x, float y, float x1, float y1, float x2, float y2)
{
    return SVGPathSegCurvetoCubicRel::create(0, x, y, x1, y1, x2, y2);
}

PassRefPtrWillBeRawPtr<SVGPathSegCurvetoQuadraticAbs> SVGPathElement::createSVGPathSegCurvetoQuadraticAbs(float x, float y, float x1, float y1)
{
    return SVGPathSegCurvetoQuadraticAbs::create(0, x, y, x1, y1);
}

PassRefPtrWillBeRawPtr<SVGPathSegCurvetoQuadraticRel> SVGPathElement::createSVGPathSegCurvetoQuadraticRel(float x, float y, float x1, float y1)
{
    return SVGPathSegCurvetoQuadraticRel::create(0, x, y, x1, y1);
}

PassRefPtrWillBeRawPtr<SVGPathSegArcAbs> SVGPathElement::createSVGPathSegArcAbs(float x, float y, float r1, float r2, float angle, bool largeArcFlag, bool sweepFlag)
{
    return SVGPathSegArcAbs::create(0, x, y, r1, r2, angle, largeArcFlag, sweepFlag);
}

PassRefPtrWillBeRawPtr<SVGPathSegArcRel> SVGPathElement::createSVGPathSegArcRel(float x, float y, float r1, float r2, float angle, bool largeArcFlag, bool sweepFlag)
{
    return SVGPathSegArcRel::create(0, x, y, r1, r2, angle, largeArcFlag, sweepFlag);
}

PassRefPtrWillBeRawPtr<SVGPathSegLinetoHorizontalAbs> SVGPathElement::createSVGPathSegLinetoHorizontalAbs(float x)
{
    return SVGPathSegLinetoHorizontalAbs::create(0, x);
}

PassRefPtrWillBeRawPtr<SVGPathSegLinetoHorizontalRel> SVGPathElement::createSVGPathSegLinetoHorizontalRel(float x)
{
    return SVGPathSegLinetoHorizontalRel::create(0, x);
}

PassRefPtrWillBeRawPtr<SVGPathSegLinetoVerticalAbs> SVGPathElement::createSVGPathSegLinetoVerticalAbs(float y)
{
    return SVGPathSegLinetoVerticalAbs::create(0, y);
}

PassRefPtrWillBeRawPtr<SVGPathSegLinetoVerticalRel> SVGPathElement::createSVGPathSegLinetoVerticalRel(float y)
{
    return SVGPathSegLinetoVerticalRel::create(0, y);
}

PassRefPtrWillBeRawPtr<SVGPathSegCurvetoCubicSmoothAbs> SVGPathElement::createSVGPathSegCurvetoCubicSmoothAbs(float x, float y, float x2, float y2)
{
    return SVGPathSegCurvetoCubicSmoothAbs::create(0, x, y, x2, y2);
}

PassRefPtrWillBeRawPtr<SVGPathSegCurvetoCubicSmoothRel> SVGPathElement::createSVGPathSegCurvetoCubicSmoothRel(float x, float y, float x2, float y2)
{
    return SVGPathSegCurvetoCubicSmoothRel::create(0, x, y, x2, y2);
}

PassRefPtrWillBeRawPtr<SVGPathSegCurvetoQuadraticSmoothAbs> SVGPathElement::createSVGPathSegCurvetoQuadraticSmoothAbs(float x, float y)
{
    return SVGPathSegCurvetoQuadraticSmoothAbs::create(0, x, y);
}

PassRefPtrWillBeRawPtr<SVGPathSegCurvetoQuadraticSmoothRel> SVGPathElement::createSVGPathSegCurvetoQuadraticSmoothRel(float x, float y)
{
    return SVGPathSegCurvetoQuadraticSmoothRel::create(0, x, y);
}

void SVGPathElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (attrName == SVGNames::dAttr || attrName == SVGNames::pathLengthAttr) {
        SVGElement::InvalidationGuard invalidationGuard(this);

        LayoutSVGShape* layoutObject = toLayoutSVGShape(this->layoutObject());

        if (attrName == SVGNames::dAttr) {
            if (layoutObject)
                layoutObject->setNeedsShapeUpdate();

            invalidateMPathDependencies();
        }

        if (layoutObject)
            markForLayoutAndParentResourceInvalidation(layoutObject);

        return;
    }

    SVGGeometryElement::svgAttributeChanged(attrName);
}

void SVGPathElement::invalidateMPathDependencies()
{
    // <mpath> can only reference <path> but this dependency is not handled in
    // markForLayoutAndParentResourceInvalidation so we update any mpath dependencies manually.
    if (SVGElementSet* dependencies = setOfIncomingReferences()) {
        for (SVGElement* element : *dependencies) {
            if (isSVGMPathElement(*element))
                toSVGMPathElement(element)->targetPathChanged();
        }
    }
}

Node::InsertionNotificationRequest SVGPathElement::insertedInto(ContainerNode* rootParent)
{
    SVGGeometryElement::insertedInto(rootParent);
    invalidateMPathDependencies();
    return InsertionDone;
}

void SVGPathElement::removedFrom(ContainerNode* rootParent)
{
    SVGGeometryElement::removedFrom(rootParent);
    invalidateMPathDependencies();
}

void SVGPathElement::pathSegListChanged(ListModification listModification)
{
    m_pathSegList->baseValue()->clearByteStream();

    invalidateSVGAttributes();

    SVGElement::InvalidationGuard invalidationGuard(this);
    LayoutSVGShape* layoutObject = toLayoutSVGShape(this->layoutObject());
    if (!layoutObject)
        return;

    layoutObject->setNeedsShapeUpdate();
    markForLayoutAndParentResourceInvalidation(layoutObject);
}

FloatRect SVGPathElement::getBBox()
{
    // By default, getBBox() returns objectBoundingBox but that will include
    // markers so we override it to return just the path's bounding rect.

    document().updateLayoutIgnorePendingStylesheets();

    // FIXME: Eventually we should support getBBox for detached elements.
    if (!layoutObject())
        return FloatRect();

    LayoutSVGShape* layoutObject = toLayoutSVGShape(this->layoutObject());
    return layoutObject->path().boundingRect();
}

} // namespace blink
