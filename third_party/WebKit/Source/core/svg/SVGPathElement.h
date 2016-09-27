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

#ifndef SVGPathElement_h
#define SVGPathElement_h

#include "core/SVGNames.h"
#include "core/svg/SVGAnimatedBoolean.h"
#include "core/svg/SVGAnimatedNumber.h"
#include "core/svg/SVGAnimatedPath.h"
#include "core/svg/SVGGeometryElement.h"
#include "core/svg/SVGPathByteStream.h"
#include "platform/heap/Handle.h"

namespace blink {

class SVGPathSegArcAbs;
class SVGPathSegArcRel;
class SVGPathSegClosePath;
class SVGPathSegLinetoAbs;
class SVGPathSegLinetoRel;
class SVGPathSegMovetoAbs;
class SVGPathSegMovetoRel;
class SVGPathSegCurvetoCubicAbs;
class SVGPathSegCurvetoCubicRel;
class SVGPathSegLinetoVerticalAbs;
class SVGPathSegLinetoVerticalRel;
class SVGPathSegLinetoHorizontalAbs;
class SVGPathSegLinetoHorizontalRel;
class SVGPathSegCurvetoQuadraticAbs;
class SVGPathSegCurvetoQuadraticRel;
class SVGPathSegCurvetoCubicSmoothAbs;
class SVGPathSegCurvetoCubicSmoothRel;
class SVGPathSegCurvetoQuadraticSmoothAbs;
class SVGPathSegCurvetoQuadraticSmoothRel;

class SVGPathElement final : public SVGGeometryElement {
    DEFINE_WRAPPERTYPEINFO();
public:
    DECLARE_NODE_FACTORY(SVGPathElement);

    Path asPath() const override;

    float getTotalLength();
    PassRefPtrWillBeRawPtr<SVGPointTearOff> getPointAtLength(float distance);
    unsigned getPathSegAtLength(float distance);

    SVGAnimatedNumber* pathLength() { return m_pathLength.get(); }

    PassRefPtrWillBeRawPtr<SVGPathSegClosePath> createSVGPathSegClosePath();
    PassRefPtrWillBeRawPtr<SVGPathSegMovetoAbs> createSVGPathSegMovetoAbs(float x, float y);
    PassRefPtrWillBeRawPtr<SVGPathSegMovetoRel> createSVGPathSegMovetoRel(float x, float y);
    PassRefPtrWillBeRawPtr<SVGPathSegLinetoAbs> createSVGPathSegLinetoAbs(float x, float y);
    PassRefPtrWillBeRawPtr<SVGPathSegLinetoRel> createSVGPathSegLinetoRel(float x, float y);
    PassRefPtrWillBeRawPtr<SVGPathSegCurvetoCubicAbs> createSVGPathSegCurvetoCubicAbs(float x, float y, float x1, float y1, float x2, float y2);
    PassRefPtrWillBeRawPtr<SVGPathSegCurvetoCubicRel> createSVGPathSegCurvetoCubicRel(float x, float y, float x1, float y1, float x2, float y2);
    PassRefPtrWillBeRawPtr<SVGPathSegCurvetoQuadraticAbs> createSVGPathSegCurvetoQuadraticAbs(float x, float y, float x1, float y1);
    PassRefPtrWillBeRawPtr<SVGPathSegCurvetoQuadraticRel> createSVGPathSegCurvetoQuadraticRel(float x, float y, float x1, float y1);
    PassRefPtrWillBeRawPtr<SVGPathSegArcAbs> createSVGPathSegArcAbs(float x, float y, float r1, float r2, float angle, bool largeArcFlag, bool sweepFlag);
    PassRefPtrWillBeRawPtr<SVGPathSegArcRel> createSVGPathSegArcRel(float x, float y, float r1, float r2, float angle, bool largeArcFlag, bool sweepFlag);
    PassRefPtrWillBeRawPtr<SVGPathSegLinetoHorizontalAbs> createSVGPathSegLinetoHorizontalAbs(float x);
    PassRefPtrWillBeRawPtr<SVGPathSegLinetoHorizontalRel> createSVGPathSegLinetoHorizontalRel(float x);
    PassRefPtrWillBeRawPtr<SVGPathSegLinetoVerticalAbs> createSVGPathSegLinetoVerticalAbs(float y);
    PassRefPtrWillBeRawPtr<SVGPathSegLinetoVerticalRel> createSVGPathSegLinetoVerticalRel(float y);
    PassRefPtrWillBeRawPtr<SVGPathSegCurvetoCubicSmoothAbs> createSVGPathSegCurvetoCubicSmoothAbs(float x, float y, float x2, float y2);
    PassRefPtrWillBeRawPtr<SVGPathSegCurvetoCubicSmoothRel> createSVGPathSegCurvetoCubicSmoothRel(float x, float y, float x2, float y2);
    PassRefPtrWillBeRawPtr<SVGPathSegCurvetoQuadraticSmoothAbs> createSVGPathSegCurvetoQuadraticSmoothAbs(float x, float y);
    PassRefPtrWillBeRawPtr<SVGPathSegCurvetoQuadraticSmoothRel> createSVGPathSegCurvetoQuadraticSmoothRel(float x, float y);

    // Used in the bindings only.
    SVGPathSegListTearOff* pathSegList() { return m_pathSegList->baseVal(); }
    SVGPathSegListTearOff* animatedPathSegList() { return m_pathSegList->animVal(); }

    // FIXME: https://bugs.webkit.org/show_bug.cgi?id=15412 - Implement normalized path segment lists!
    SVGPathSegListTearOff* normalizedPathSegList() { return nullptr; }
    SVGPathSegListTearOff* animatedNormalizedPathSegList() { return nullptr; }

    const SVGPathByteStream* pathByteStream() const { return m_pathSegList->currentValue()->byteStream(); }

    void pathSegListChanged(ListModification = ListModificationUnknown);

    FloatRect getBBox() override;

    DECLARE_VIRTUAL_TRACE();

private:
    explicit SVGPathElement(Document&);

    void svgAttributeChanged(const QualifiedName&) override;

    Node::InsertionNotificationRequest insertedInto(ContainerNode*) override;
    void removedFrom(ContainerNode*) override;

    void invalidateMPathDependencies();

    RefPtrWillBeMember<SVGAnimatedNumber> m_pathLength;
    RefPtrWillBeMember<SVGAnimatedPath> m_pathSegList;
};

} // namespace blink

#endif // SVGPathElement_h
