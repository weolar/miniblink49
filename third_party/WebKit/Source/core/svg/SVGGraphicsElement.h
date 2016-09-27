/*
 * Copyright (C) 2004, 2005, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
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

#ifndef SVGGraphicsElement_h
#define SVGGraphicsElement_h

#include "core/CoreExport.h"
#include "core/svg/SVGAnimatedTransformList.h"
#include "core/svg/SVGElement.h"
#include "core/svg/SVGRectTearOff.h"
#include "core/svg/SVGTests.h"
#include "platform/heap/Handle.h"

namespace blink {

class AffineTransform;
class Path;
class SVGMatrixTearOff;

class CORE_EXPORT SVGGraphicsElement : public SVGElement, public SVGTests {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(SVGGraphicsElement);
public:
    ~SVGGraphicsElement() override;

    enum StyleUpdateStrategy { AllowStyleUpdate, DisallowStyleUpdate };

    AffineTransform getCTM(StyleUpdateStrategy = AllowStyleUpdate);
    AffineTransform getScreenCTM(StyleUpdateStrategy = AllowStyleUpdate);
    PassRefPtrWillBeRawPtr<SVGMatrixTearOff> getCTMFromJavascript();
    PassRefPtrWillBeRawPtr<SVGMatrixTearOff> getScreenCTMFromJavascript();

    PassRefPtrWillBeRawPtr<SVGMatrixTearOff> getTransformToElement(SVGElement*, ExceptionState&);

    SVGElement* nearestViewportElement() const;
    SVGElement* farthestViewportElement() const;

    AffineTransform localCoordinateSpaceTransform(SVGElement::CTMScope) const override { return calculateAnimatedLocalTransform(); }
    bool hasAnimatedLocalTransform() const;
    AffineTransform calculateAnimatedLocalTransform() const;
    AffineTransform* animateMotionTransform() override;

    virtual FloatRect getBBox();
    PassRefPtrWillBeRawPtr<SVGRectTearOff> getBBoxFromJavascript();

    bool isValid() const final { return SVGTests::isValid(document()); }

    SVGAnimatedTransformList* transform() { return m_transform.get(); }
    const SVGAnimatedTransformList* transform() const { return m_transform.get(); }

    AffineTransform computeCTM(SVGElement::CTMScope mode, SVGGraphicsElement::StyleUpdateStrategy,
        const SVGGraphicsElement* ancestor = 0) const;

    DECLARE_VIRTUAL_TRACE();

protected:
    SVGGraphicsElement(const QualifiedName&, Document&, ConstructionType = CreateSVGElement);

    bool supportsFocus() const override { return Element::supportsFocus() || hasFocusEventListeners(); }

    void svgAttributeChanged(const QualifiedName&) override;

    RefPtrWillBeMember<SVGAnimatedTransformList> m_transform;

private:
    bool isSVGGraphicsElement() const final { return true; }
};

inline bool isSVGGraphicsElement(const SVGElement& element)
{
    return element.isSVGGraphicsElement();
}

DEFINE_SVGELEMENT_TYPE_CASTS_WITH_FUNCTION(SVGGraphicsElement);

} // namespace blink

#endif // SVGGraphicsElement_h
