/*
 * Copyright (C) 2004, 2005, 2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007, 2010 Rob Buis <buis@kde.org>
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

#ifndef SVGSVGElement_h
#define SVGSVGElement_h

#include "core/svg/SVGAnimatedBoolean.h"
#include "core/svg/SVGAnimatedLength.h"
#include "core/svg/SVGFitToViewBox.h"
#include "core/svg/SVGGraphicsElement.h"
#include "core/svg/SVGLengthTearOff.h"
#include "core/svg/SVGPointTearOff.h"
#include "core/svg/SVGZoomAndPan.h"
#include "platform/heap/Handle.h"

namespace blink {

class SVGMatrixTearOff;
class SVGAngleTearOff;
class SVGNumberTearOff;
class SVGTransformTearOff;
class SVGViewSpec;
class SVGViewElement;
class SMILTimeContainer;

class SVGSVGElement final : public SVGGraphicsElement,
                            public SVGFitToViewBox,
                            public SVGZoomAndPan {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(SVGSVGElement);
public:
    DECLARE_NODE_FACTORY(SVGSVGElement);

#if !ENABLE(OILPAN)
    using SVGGraphicsElement::ref;
    using SVGGraphicsElement::deref;
#endif

    // 'SVGSVGElement' functions
    PassRefPtrWillBeRawPtr<SVGRectTearOff> viewport() const;

    float pixelUnitToMillimeterX() const;
    float pixelUnitToMillimeterY() const;
    float screenPixelToMillimeterX() const;
    float screenPixelToMillimeterY() const;

    bool useCurrentView() const { return m_useCurrentView; }
    SVGViewSpec* currentView();

    Length intrinsicWidth() const;
    Length intrinsicHeight() const;
    FloatSize currentViewportSize() const;
    FloatRect currentViewBoxRect() const;

    float currentScale() const;
    void setCurrentScale(float scale);

    FloatPoint currentTranslate() { return m_translation->value(); }
    void setCurrentTranslate(const FloatPoint&);
    PassRefPtrWillBeRawPtr<SVGPointTearOff> currentTranslateFromJavascript();

    SMILTimeContainer* timeContainer() const { return m_timeContainer.get(); }

    void pauseAnimations();
    void unpauseAnimations();
    bool animationsPaused() const;

    float getCurrentTime() const;
    void setCurrentTime(float seconds);

    // Stubs for the deprecated 'redraw' interface.
    unsigned suspendRedraw(unsigned) { return 1; }
    void unsuspendRedraw(unsigned) { }
    void unsuspendRedrawAll() { }
    void forceRedraw() { }

    PassRefPtrWillBeRawPtr<StaticNodeList> getIntersectionList(PassRefPtrWillBeRawPtr<SVGRectTearOff>, SVGElement* referenceElement) const;
    PassRefPtrWillBeRawPtr<StaticNodeList> getEnclosureList(PassRefPtrWillBeRawPtr<SVGRectTearOff>, SVGElement* referenceElement) const;
    bool checkIntersection(SVGElement*, PassRefPtrWillBeRawPtr<SVGRectTearOff>) const;
    bool checkEnclosure(SVGElement*, PassRefPtrWillBeRawPtr<SVGRectTearOff>) const;
    void deselectAll();

    static PassRefPtrWillBeRawPtr<SVGNumberTearOff> createSVGNumber();
    static PassRefPtrWillBeRawPtr<SVGLengthTearOff> createSVGLength();
    static PassRefPtrWillBeRawPtr<SVGAngleTearOff> createSVGAngle();
    static PassRefPtrWillBeRawPtr<SVGPointTearOff> createSVGPoint();
    static PassRefPtrWillBeRawPtr<SVGMatrixTearOff> createSVGMatrix();
    static PassRefPtrWillBeRawPtr<SVGRectTearOff> createSVGRect();
    static PassRefPtrWillBeRawPtr<SVGTransformTearOff> createSVGTransform();
    static PassRefPtrWillBeRawPtr<SVGTransformTearOff> createSVGTransformFromMatrix(PassRefPtrWillBeRawPtr<SVGMatrixTearOff>);

    AffineTransform viewBoxToViewTransform(float viewWidth, float viewHeight) const;

    void setupInitialView(const String& fragmentIdentifier, Element* anchorNode);
    bool zoomAndPanEnabled() const;

    bool hasIntrinsicWidth() const;
    bool hasIntrinsicHeight() const;

    SVGAnimatedLength* x() const { return m_x.get(); }
    SVGAnimatedLength* y() const { return m_y.get(); }
    SVGAnimatedLength* width() const { return m_width.get(); }
    SVGAnimatedLength* height() const { return m_height.get(); }

    DECLARE_VIRTUAL_TRACE();

private:
    explicit SVGSVGElement(Document&);
    ~SVGSVGElement() override;

    void parseAttribute(const QualifiedName&, const AtomicString&) override;
    bool isPresentationAttribute(const QualifiedName&) const override;
    bool isPresentationAttributeWithSVGDOM(const QualifiedName&) const override;
    void collectStyleForPresentationAttribute(const QualifiedName&, const AtomicString&, MutableStylePropertySet*) override;

    bool layoutObjectIsNeeded(const ComputedStyle&) override;
    LayoutObject* createLayoutObject(const ComputedStyle&) override;

    InsertionNotificationRequest insertedInto(ContainerNode*) override;
    void removedFrom(ContainerNode*) override;

    void svgAttributeChanged(const QualifiedName&) override;

    bool selfHasRelativeLengths() const override;

    void inheritViewAttributes(SVGViewElement*);

    void updateCurrentTranslate();

    void finishParsingChildren() override;

    enum CheckIntersectionOrEnclosure {
        CheckIntersection,
        CheckEnclosure
    };

    bool checkIntersectionOrEnclosure(const SVGElement&, const FloatRect&, CheckIntersectionOrEnclosure) const;
    PassRefPtrWillBeRawPtr<StaticNodeList> collectIntersectionOrEnclosureList(const FloatRect&, SVGElement*, CheckIntersectionOrEnclosure) const;

    RefPtrWillBeMember<SVGAnimatedLength> m_x;
    RefPtrWillBeMember<SVGAnimatedLength> m_y;
    RefPtrWillBeMember<SVGAnimatedLength> m_width;
    RefPtrWillBeMember<SVGAnimatedLength> m_height;

    AffineTransform localCoordinateSpaceTransform(SVGElement::CTMScope) const override;

    bool m_useCurrentView;
    RefPtrWillBeMember<SMILTimeContainer> m_timeContainer;
    RefPtrWillBeMember<SVGPoint> m_translation;
    RefPtrWillBeMember<SVGViewSpec> m_viewSpec;

    friend class SVGCurrentTranslateTearOff;
};

} // namespace blink

#endif // SVGSVGElement_h
