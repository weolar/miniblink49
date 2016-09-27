/*
 * Copyright (C) 2004, 2005, 2006 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2010 Rob Buis <buis@kde.org>
 * Copyright (C) 2007 Apple Inc. All rights reserved.
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

#include "core/svg/SVGSVGElement.h"

#include "bindings/core/v8/ScriptEventListener.h"
#include "core/HTMLNames.h"
#include "core/SVGNames.h"
#include "core/css/CSSHelper.h"
#include "core/dom/Document.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/StaticNodeList.h"
#include "core/editing/FrameSelection.h"
#include "core/events/EventListener.h"
#include "core/frame/LocalFrame.h"
#include "core/page/FrameTree.h"
#include "core/frame/FrameView.h"
#include "core/frame/UseCounter.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/svg/LayoutSVGModelObject.h"
#include "core/layout/svg/LayoutSVGRoot.h"
#include "core/layout/svg/LayoutSVGViewportContainer.h"
#include "core/svg/SVGAngleTearOff.h"
#include "core/svg/SVGNumberTearOff.h"
#include "core/svg/SVGPreserveAspectRatio.h"
#include "core/svg/SVGRectTearOff.h"
#include "core/svg/SVGTransform.h"
#include "core/svg/SVGTransformList.h"
#include "core/svg/SVGTransformTearOff.h"
#include "core/svg/SVGViewElement.h"
#include "core/svg/SVGViewSpec.h"
#include "core/svg/animation/SMILTimeContainer.h"
#include "platform/FloatConversion.h"
#include "platform/LengthFunctions.h"
#include "platform/geometry/FloatRect.h"
#include "platform/transforms/AffineTransform.h"
#include "wtf/StdLibExtras.h"

namespace blink {

inline SVGSVGElement::SVGSVGElement(Document& doc)
    : SVGGraphicsElement(SVGNames::svgTag, doc)
    , SVGFitToViewBox(this)
    , m_x(SVGAnimatedLength::create(this, SVGNames::xAttr, SVGLength::create(SVGLengthMode::Width), AllowNegativeLengths))
    , m_y(SVGAnimatedLength::create(this, SVGNames::yAttr, SVGLength::create(SVGLengthMode::Height), AllowNegativeLengths))
    , m_width(SVGAnimatedLength::create(this, SVGNames::widthAttr, SVGLength::create(SVGLengthMode::Width), ForbidNegativeLengths))
    , m_height(SVGAnimatedLength::create(this, SVGNames::heightAttr, SVGLength::create(SVGLengthMode::Height), ForbidNegativeLengths))
    , m_useCurrentView(false)
    , m_timeContainer(SMILTimeContainer::create(*this))
    , m_translation(SVGPoint::create())
{
    m_width->setDefaultValueAsString("100%");
    m_height->setDefaultValueAsString("100%");

    addToPropertyMap(m_x);
    addToPropertyMap(m_y);
    addToPropertyMap(m_width);
    addToPropertyMap(m_height);

    UseCounter::count(doc, UseCounter::SVGSVGElement);
}

DEFINE_NODE_FACTORY(SVGSVGElement)

SVGSVGElement::~SVGSVGElement()
{
#if !ENABLE(OILPAN)
    if (m_viewSpec)
        m_viewSpec->detachContextElement();

    // There are cases where removedFromDocument() is not called.
    // see ContainerNode::removeAllChildren, called by its destructor.
    // With Oilpan, either removedFrom is called or the document
    // is dead as well and there is no reason to clear the extensions.
    document().accessSVGExtensions().removeTimeContainer(this);

    ASSERT(inDocument() || !accessDocumentSVGExtensions().isSVGRootWithRelativeLengthDescendents(this));
#endif
}

PassRefPtrWillBeRawPtr<SVGRectTearOff> SVGSVGElement::viewport() const
{
    // FIXME: This method doesn't follow the spec and is basically untested. Parent documents are not considered here.
    // As we have no test coverage for this, we're going to disable it completly for now.
    return SVGRectTearOff::create(SVGRect::create(), 0, PropertyIsNotAnimVal);
}

float SVGSVGElement::pixelUnitToMillimeterX() const
{
    return 1 / cssPixelsPerMillimeter;
}

float SVGSVGElement::pixelUnitToMillimeterY() const
{
    return 1 / cssPixelsPerMillimeter;
}

float SVGSVGElement::screenPixelToMillimeterX() const
{
    return pixelUnitToMillimeterX();
}

float SVGSVGElement::screenPixelToMillimeterY() const
{
    return pixelUnitToMillimeterY();
}

SVGViewSpec* SVGSVGElement::currentView()
{
    if (!m_viewSpec)
        m_viewSpec = SVGViewSpec::create(this);
    return m_viewSpec.get();
}

float SVGSVGElement::currentScale() const
{
    if (!inDocument() || !isOutermostSVGSVGElement())
        return 1;

    LocalFrame* frame = document().frame();
    if (!frame)
        return 1;

    const FrameTree& frameTree = frame->tree();

    // The behaviour of currentScale() is undefined, when we're dealing with non-standalone SVG documents.
    // If the svg is embedded, the scaling is handled by the host layoutObject, so when asking from inside
    // the SVG document, a scale value of 1 seems reasonable, as it doesn't know anything about the parent scale.
    return frameTree.parent() ? 1 : frame->pageZoomFactor();
}

void SVGSVGElement::setCurrentScale(float scale)
{
    ASSERT(std::isfinite(scale));
    if (!inDocument() || !isOutermostSVGSVGElement())
        return;

    LocalFrame* frame = document().frame();
    if (!frame)
        return;

    const FrameTree& frameTree = frame->tree();

    // The behaviour of setCurrentScale() is undefined, when we're dealing with non-standalone SVG documents.
    // We choose the ignore this call, it's pretty useless to support calling setCurrentScale() from within
    // an embedded SVG document, for the same reasons as in currentScale() - needs resolution by SVG WG.
    if (frameTree.parent())
        return;

    frame->setPageZoomFactor(scale);
}

class SVGCurrentTranslateTearOff : public SVGPointTearOff {
public:
    static PassRefPtrWillBeRawPtr<SVGCurrentTranslateTearOff> create(SVGSVGElement* contextElement)
    {
        return adoptRefWillBeNoop(new SVGCurrentTranslateTearOff(contextElement));
    }

    void commitChange() override
    {
        ASSERT(contextElement());
        toSVGSVGElement(contextElement())->updateCurrentTranslate();
    }

private:
    SVGCurrentTranslateTearOff(SVGSVGElement* contextElement)
        : SVGPointTearOff(contextElement->m_translation, contextElement, PropertyIsNotAnimVal)
    {
    }
};

PassRefPtrWillBeRawPtr<SVGPointTearOff> SVGSVGElement::currentTranslateFromJavascript()
{
    return SVGCurrentTranslateTearOff::create(this);
}

void SVGSVGElement::setCurrentTranslate(const FloatPoint& point)
{
    m_translation->setValue(point);
    updateCurrentTranslate();
}

void SVGSVGElement::updateCurrentTranslate()
{
    if (LayoutObject* object = layoutObject())
        object->setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::Unknown);
}

bool SVGSVGElement::zoomAndPanEnabled() const
{
    const SVGZoomAndPan* currentViewSpec = this;
    if (m_useCurrentView)
        currentViewSpec = m_viewSpec.get();
    return currentViewSpec && currentViewSpec->zoomAndPan() == SVGZoomAndPanMagnify;
}

void SVGSVGElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (!nearestViewportElement()) {
        bool setListener = true;

        // Only handle events if we're the outermost <svg> element
        if (name == HTMLNames::onunloadAttr)
            document().setWindowAttributeEventListener(EventTypeNames::unload, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
        else if (name == HTMLNames::onresizeAttr)
            document().setWindowAttributeEventListener(EventTypeNames::resize, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
        else if (name == HTMLNames::onscrollAttr)
            document().setWindowAttributeEventListener(EventTypeNames::scroll, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
        else if (name == SVGNames::onzoomAttr)
            document().setWindowAttributeEventListener(EventTypeNames::zoom, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
        else
            setListener = false;

        if (setListener)
            return;
    }

    if (name == HTMLNames::onabortAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::abort, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (name == HTMLNames::onerrorAttr) {
        document().setWindowAttributeEventListener(EventTypeNames::error, createAttributeEventListener(document().frame(), name, value, eventParameterName()));
    } else if (SVGZoomAndPan::parseAttribute(name, value)) {
    } else {
        SVGElement::parseAttribute(name, value);
    }
}

bool SVGSVGElement::isPresentationAttribute(const QualifiedName& name) const
{
    if (isOutermostSVGSVGElement() && (name == SVGNames::widthAttr || name == SVGNames::heightAttr))
        return true;
    else if (name == SVGNames::xAttr || name == SVGNames::yAttr)
        return true;

    return SVGGraphicsElement::isPresentationAttribute(name);
}

bool SVGSVGElement::isPresentationAttributeWithSVGDOM(const QualifiedName& attrName) const
{
    if (attrName == SVGNames::xAttr || attrName == SVGNames::yAttr)
        return true;
    return SVGGraphicsElement::isPresentationAttributeWithSVGDOM(attrName);
}

void SVGSVGElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    RefPtrWillBeRawPtr<SVGAnimatedPropertyBase> property = propertyFromAttribute(name);
    if (property == m_x) {
        addSVGLengthPropertyToPresentationAttributeStyle(style, CSSPropertyX, *m_x->currentValue());
    } else if (property == m_y) {
        addSVGLengthPropertyToPresentationAttributeStyle(style, CSSPropertyY, *m_y->currentValue());
    } else if (isOutermostSVGSVGElement() && (property == m_width || property == m_height)) {
        if (property == m_width)
            addSVGLengthPropertyToPresentationAttributeStyle(style, CSSPropertyWidth, *m_width->currentValue());
        else if (property == m_height)
            addSVGLengthPropertyToPresentationAttributeStyle(style, CSSPropertyHeight, *m_height->currentValue());
    } else {
        SVGGraphicsElement::collectStyleForPresentationAttribute(name, value, style);
    }
}

void SVGSVGElement::svgAttributeChanged(const QualifiedName& attrName)
{
    bool updateRelativeLengthsOrViewBox = false;
    bool widthChanged = attrName == SVGNames::widthAttr;
    bool heightChanged = attrName == SVGNames::heightAttr;
    if (widthChanged || heightChanged
        || attrName == SVGNames::xAttr
        || attrName == SVGNames::yAttr) {
        updateRelativeLengthsOrViewBox = true;
        updateRelativeLengthsInformation();
        invalidateRelativeLengthClients();

        // At the SVG/HTML boundary (aka LayoutSVGRoot), the width and
        // height attributes can affect the replaced size so we need
        // to mark it for updating.
        //
        // FIXME: For width/height animated as XML attributes on SVG
        // roots, there is an attribute synchronization missing. See
        // http://crbug.com/364807
        if (widthChanged || heightChanged) {
            LayoutObject* layoutObject = this->layoutObject();
            if (layoutObject && layoutObject->isSVGRoot()) {
                invalidateSVGPresentationAttributeStyle();
                setNeedsStyleRecalc(LocalStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::SVGContainerSizeChange));
            }
        } else {
            invalidateSVGPresentationAttributeStyle();
            setNeedsStyleRecalc(LocalStyleChange,
                StyleChangeReasonForTracing::fromAttribute(attrName));
        }
    }

    if (SVGFitToViewBox::isKnownAttribute(attrName)) {
        updateRelativeLengthsOrViewBox = true;
        invalidateRelativeLengthClients();
        if (LayoutObject* object = layoutObject())
            object->setNeedsTransformUpdate();
    }

    if (updateRelativeLengthsOrViewBox
        || SVGZoomAndPan::isKnownAttribute(attrName)) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        if (layoutObject())
            markForLayoutAndParentResourceInvalidation(layoutObject());
        return;
    }

    SVGGraphicsElement::svgAttributeChanged(attrName);
}

// FloatRect::intersects does not consider horizontal or vertical lines (because of isEmpty()).
static bool intersectsAllowingEmpty(const FloatRect& r1, const FloatRect& r2)
{
    if (r1.width() < 0 || r1.height() < 0 || r2.width() < 0 || r2.height() < 0)
        return false;

    return r1.x() < r2.maxX() && r2.x() < r1.maxX()
        && r1.y() < r2.maxY() && r2.y() < r1.maxY();
}

// One of the element types that can cause graphics to be drawn onto the target canvas.
// Specifically: circle, ellipse, image, line, path, polygon, polyline, rect, text and use.
static bool isIntersectionOrEnclosureTarget(LayoutObject* layoutObject)
{
    return layoutObject->isSVGShape()
        || layoutObject->isSVGText()
        || layoutObject->isSVGImage()
        || isSVGUseElement(*layoutObject->node());
}

bool SVGSVGElement::checkIntersectionOrEnclosure(const SVGElement& element, const FloatRect& rect,
    CheckIntersectionOrEnclosure mode) const
{
    LayoutObject* layoutObject = element.layoutObject();
    ASSERT(!layoutObject || layoutObject->style());
    if (!layoutObject || layoutObject->style()->pointerEvents() == PE_NONE)
        return false;

    if (!isIntersectionOrEnclosureTarget(layoutObject))
        return false;

    AffineTransform ctm = toSVGGraphicsElement(element).computeCTM(AncestorScope, DisallowStyleUpdate, this);
    FloatRect mappedRepaintRect = ctm.mapRect(layoutObject->paintInvalidationRectInLocalCoordinates());

    bool result = false;
    switch (mode) {
    case CheckIntersection:
        result = intersectsAllowingEmpty(rect, mappedRepaintRect);
        break;
    case CheckEnclosure:
        result = rect.contains(mappedRepaintRect);
        break;
    default:
        ASSERT_NOT_REACHED();
        break;
    }

    return result;
}

PassRefPtrWillBeRawPtr<StaticNodeList> SVGSVGElement::collectIntersectionOrEnclosureList(const FloatRect& rect,
    SVGElement* referenceElement, CheckIntersectionOrEnclosure mode) const
{
    WillBeHeapVector<RefPtrWillBeMember<Node>> nodes;

    const SVGElement* root = this;
    if (referenceElement) {
        // Only the common subtree needs to be traversed.
        if (contains(referenceElement)) {
            root = referenceElement;
        } else if (!isDescendantOf(referenceElement)) {
            // No common subtree.
            return StaticNodeList::adopt(nodes);
        }
    }

    for (SVGGraphicsElement& element : Traversal<SVGGraphicsElement>::descendantsOf(*root)) {
        if (checkIntersectionOrEnclosure(element, rect, mode))
            nodes.append(&element);
    }

    return StaticNodeList::adopt(nodes);
}

PassRefPtrWillBeRawPtr<StaticNodeList> SVGSVGElement::getIntersectionList(PassRefPtrWillBeRawPtr<SVGRectTearOff> rect, SVGElement* referenceElement) const
{
    document().updateLayoutIgnorePendingStylesheets();

    return collectIntersectionOrEnclosureList(rect->target()->value(), referenceElement, CheckIntersection);
}

PassRefPtrWillBeRawPtr<StaticNodeList> SVGSVGElement::getEnclosureList(PassRefPtrWillBeRawPtr<SVGRectTearOff> rect, SVGElement* referenceElement) const
{
    document().updateLayoutIgnorePendingStylesheets();

    return collectIntersectionOrEnclosureList(rect->target()->value(), referenceElement, CheckEnclosure);
}

bool SVGSVGElement::checkIntersection(SVGElement* element, PassRefPtrWillBeRawPtr<SVGRectTearOff> rect) const
{
    ASSERT(element);
    document().updateLayoutIgnorePendingStylesheets();

    return checkIntersectionOrEnclosure(*element, rect->target()->value(), CheckIntersection);
}

bool SVGSVGElement::checkEnclosure(SVGElement* element, PassRefPtrWillBeRawPtr<SVGRectTearOff> rect) const
{
    ASSERT(element);
    document().updateLayoutIgnorePendingStylesheets();

    return checkIntersectionOrEnclosure(*element, rect->target()->value(), CheckEnclosure);
}

void SVGSVGElement::deselectAll()
{
    if (LocalFrame* frame = document().frame())
        frame->selection().clear();
}

PassRefPtrWillBeRawPtr<SVGNumberTearOff> SVGSVGElement::createSVGNumber()
{
    return SVGNumberTearOff::create(SVGNumber::create(0.0f), 0, PropertyIsNotAnimVal);
}

PassRefPtrWillBeRawPtr<SVGLengthTearOff> SVGSVGElement::createSVGLength()
{
    return SVGLengthTearOff::create(SVGLength::create(), 0, PropertyIsNotAnimVal);
}

PassRefPtrWillBeRawPtr<SVGAngleTearOff> SVGSVGElement::createSVGAngle()
{
    return SVGAngleTearOff::create(SVGAngle::create(), 0, PropertyIsNotAnimVal);
}

PassRefPtrWillBeRawPtr<SVGPointTearOff> SVGSVGElement::createSVGPoint()
{
    return SVGPointTearOff::create(SVGPoint::create(), 0, PropertyIsNotAnimVal);
}

PassRefPtrWillBeRawPtr<SVGMatrixTearOff> SVGSVGElement::createSVGMatrix()
{
    return SVGMatrixTearOff::create(AffineTransform());
}

PassRefPtrWillBeRawPtr<SVGRectTearOff> SVGSVGElement::createSVGRect()
{
    return SVGRectTearOff::create(SVGRect::create(), 0, PropertyIsNotAnimVal);
}

PassRefPtrWillBeRawPtr<SVGTransformTearOff> SVGSVGElement::createSVGTransform()
{
    return SVGTransformTearOff::create(SVGTransform::create(SVG_TRANSFORM_MATRIX), 0, PropertyIsNotAnimVal);
}

PassRefPtrWillBeRawPtr<SVGTransformTearOff> SVGSVGElement::createSVGTransformFromMatrix(PassRefPtrWillBeRawPtr<SVGMatrixTearOff> matrix)
{
    return SVGTransformTearOff::create(SVGTransform::create(matrix->value()), 0, PropertyIsNotAnimVal);
}

AffineTransform SVGSVGElement::localCoordinateSpaceTransform(SVGElement::CTMScope mode) const
{
    AffineTransform viewBoxTransform;
    if (!hasEmptyViewBox()) {
        FloatSize size = currentViewportSize();
        viewBoxTransform = viewBoxToViewTransform(size.width(), size.height());
    }

    AffineTransform transform;
    if (!isOutermostSVGSVGElement()) {
        SVGLengthContext lengthContext(this);
        transform.translate(m_x->currentValue()->value(lengthContext), m_y->currentValue()->value(lengthContext));
    } else if (mode == SVGElement::ScreenScope) {
        if (LayoutObject* layoutObject = this->layoutObject()) {
            FloatPoint location;
            float zoomFactor = 1;

            // At the SVG/HTML boundary (aka LayoutSVGRoot), we apply the localToBorderBoxTransform
            // to map an element from SVG viewport coordinates to CSS box coordinates.
            // LayoutSVGRoot's localToAbsolute method expects CSS box coordinates.
            // We also need to adjust for the zoom level factored into CSS coordinates (bug #96361).
            if (layoutObject->isSVGRoot()) {
                location = toLayoutSVGRoot(layoutObject)->localToBorderBoxTransform().mapPoint(location);
                zoomFactor = 1 / layoutObject->style()->effectiveZoom();
            }

            // Translate in our CSS parent coordinate space
            // FIXME: This doesn't work correctly with CSS transforms.
            location = layoutObject->localToAbsolute(location, UseTransforms);
            location.scale(zoomFactor, zoomFactor);

            // Be careful here! localToBorderBoxTransform() included the x/y offset coming from the viewBoxToViewTransform(),
            // so we have to subtract it here (original cause of bug #27183)
            transform.translate(location.x() - viewBoxTransform.e(), location.y() - viewBoxTransform.f());

            // Respect scroll offset.
            if (FrameView* view = document().view()) {
                LayoutSize scrollOffset(view->scrollOffset());
                scrollOffset.scale(zoomFactor);
                transform.translate(-scrollOffset.width(), -scrollOffset.height());
            }
        }
    }

    return transform.multiply(viewBoxTransform);
}

bool SVGSVGElement::layoutObjectIsNeeded(const ComputedStyle& style)
{
    // FIXME: We should respect display: none on the documentElement svg element
    // but many things in FrameView and SVGImage depend on the LayoutSVGRoot when
    // they should instead depend on the LayoutView.
    // https://bugs.webkit.org/show_bug.cgi?id=103493
    if (document().documentElement() == this)
        return true;
    return Element::layoutObjectIsNeeded(style);
}

LayoutObject* SVGSVGElement::createLayoutObject(const ComputedStyle&)
{
    if (isOutermostSVGSVGElement())
        return new LayoutSVGRoot(this);

    return new LayoutSVGViewportContainer(this);
}

Node::InsertionNotificationRequest SVGSVGElement::insertedInto(ContainerNode* rootParent)
{
    if (rootParent->inDocument()) {
        UseCounter::count(document(), UseCounter::SVGSVGElementInDocument);
        if (rootParent->document().isXMLDocument())
            UseCounter::count(document(), UseCounter::SVGSVGElementInXMLDocument);

        document().accessSVGExtensions().addTimeContainer(this);

        // Animations are started at the end of document parsing and after firing the load event,
        // but if we miss that train (deferred programmatic element insertion for example) we need
        // to initialize the time container here.
        if (!document().parsing() && !document().processingLoadEvent() && document().loadEventFinished() && !timeContainer()->isStarted())
            timeContainer()->begin();
    }
    return SVGGraphicsElement::insertedInto(rootParent);
}

void SVGSVGElement::removedFrom(ContainerNode* rootParent)
{
    if (rootParent->inDocument()) {
        SVGDocumentExtensions& svgExtensions = document().accessSVGExtensions();
        svgExtensions.removeTimeContainer(this);
        svgExtensions.removeSVGRootWithRelativeLengthDescendents(this);
    }

    SVGGraphicsElement::removedFrom(rootParent);
}

void SVGSVGElement::pauseAnimations()
{
    if (!m_timeContainer->isPaused())
        m_timeContainer->pause();
}

void SVGSVGElement::unpauseAnimations()
{
    if (m_timeContainer->isPaused())
        m_timeContainer->resume();
}

bool SVGSVGElement::animationsPaused() const
{
    return m_timeContainer->isPaused();
}

float SVGSVGElement::getCurrentTime() const
{
    return narrowPrecisionToFloat(m_timeContainer->elapsed().value());
}

void SVGSVGElement::setCurrentTime(float seconds)
{
    ASSERT(std::isfinite(seconds));
    seconds = max(seconds, 0.0f);
    m_timeContainer->setElapsed(seconds);
}

bool SVGSVGElement::selfHasRelativeLengths() const
{
    return m_x->currentValue()->isRelative()
        || m_y->currentValue()->isRelative()
        || m_width->currentValue()->isRelative()
        || m_height->currentValue()->isRelative();
}

FloatRect SVGSVGElement::currentViewBoxRect() const
{
    if (m_useCurrentView)
        return m_viewSpec ? m_viewSpec->viewBox()->currentValue()->value() : FloatRect();

    FloatRect useViewBox = viewBox()->currentValue()->value();
    if (!useViewBox.isEmpty())
        return useViewBox;
    if (!layoutObject() || !layoutObject()->isSVGRoot())
        return FloatRect();
    if (!toLayoutSVGRoot(layoutObject())->isEmbeddedThroughSVGImage())
        return FloatRect();

    // If no viewBox is specified but non-relative width/height values, then we
    // should always synthesize a viewBox if we're embedded through a SVGImage.
    return FloatRect(FloatPoint(), FloatSize(floatValueForLength(intrinsicWidth(), 0), floatValueForLength(intrinsicHeight(), 0)));
}

FloatSize SVGSVGElement::currentViewportSize() const
{
    if (!layoutObject())
        return FloatSize();

    if (layoutObject()->isSVGRoot()) {
        LayoutRect contentBoxRect = toLayoutSVGRoot(layoutObject())->contentBoxRect();
        return FloatSize(contentBoxRect.width() / layoutObject()->style()->effectiveZoom(), contentBoxRect.height() / layoutObject()->style()->effectiveZoom());
    }

    FloatRect viewportRect = toLayoutSVGViewportContainer(layoutObject())->viewport();
    return FloatSize(viewportRect.width(), viewportRect.height());
}

bool SVGSVGElement::hasIntrinsicWidth() const
{
    return width()->currentValue()->unitType() != LengthTypePercentage;
}

bool SVGSVGElement::hasIntrinsicHeight() const
{
    return height()->currentValue()->unitType() != LengthTypePercentage;
}

Length SVGSVGElement::intrinsicWidth() const
{
    if (width()->currentValue()->unitType() == LengthTypePercentage)
        return Length(0, Fixed);

    SVGLengthContext lengthContext(this);
    return Length(width()->currentValue()->value(lengthContext), Fixed);
}

Length SVGSVGElement::intrinsicHeight() const
{
    if (height()->currentValue()->unitType() == LengthTypePercentage)
        return Length(0, Fixed);

    SVGLengthContext lengthContext(this);
    return Length(height()->currentValue()->value(lengthContext), Fixed);
}

AffineTransform SVGSVGElement::viewBoxToViewTransform(float viewWidth, float viewHeight) const
{
    if (!m_useCurrentView || !m_viewSpec)
        return SVGFitToViewBox::viewBoxToViewTransform(currentViewBoxRect(), preserveAspectRatio()->currentValue(), viewWidth, viewHeight);

    AffineTransform ctm = SVGFitToViewBox::viewBoxToViewTransform(currentViewBoxRect(), m_viewSpec->preserveAspectRatio()->currentValue(), viewWidth, viewHeight);
    RefPtrWillBeRawPtr<SVGTransformList> transformList = m_viewSpec->transform();
    if (transformList->isEmpty())
        return ctm;

    AffineTransform transform;
    if (transformList->concatenate(transform))
        ctm *= transform;

    return ctm;
}

void SVGSVGElement::setupInitialView(const String& fragmentIdentifier, Element* anchorNode)
{
    LayoutObject* layoutObject = this->layoutObject();
    SVGViewSpec* view = m_viewSpec.get();
    if (view)
        view->reset();

    bool hadUseCurrentView = m_useCurrentView;
    m_useCurrentView = false;

    if (fragmentIdentifier.startsWith("xpointer(")) {
        // FIXME: XPointer references are ignored (https://bugs.webkit.org/show_bug.cgi?id=17491)
        if (layoutObject && hadUseCurrentView)
            markForLayoutAndParentResourceInvalidation(layoutObject);
        return;
    }

    if (fragmentIdentifier.startsWith("svgView(")) {
        if (!view)
            view = currentView(); // Create the SVGViewSpec.

        view->inheritViewAttributesFromElement(this);

        if (view->parseViewSpec(fragmentIdentifier))
            m_useCurrentView = true;
        else
            view->reset();

        if (layoutObject && (hadUseCurrentView || m_useCurrentView))
            markForLayoutAndParentResourceInvalidation(layoutObject);
        return;
    }

    // Spec: If the SVG fragment identifier addresses a 'view' element within an SVG document (e.g., MyDrawing.svg#MyView
    // or MyDrawing.svg#xpointer(id('MyView'))) then the closest ancestor 'svg' element is displayed in the viewport.
    // Any view specification attributes included on the given 'view' element override the corresponding view specification
    // attributes on the closest ancestor 'svg' element.
    // TODO(ed): The spec text above is a bit unclear.
    // Should the transform from outermost svg to nested svg be applied to "display"
    // the inner svg in the viewport, then let the view element override the inner
    // svg's view specification attributes. Should it fill/override the outer viewport?
    if (isSVGViewElement(anchorNode)) {
        SVGViewElement& viewElement = toSVGViewElement(*anchorNode);

        if (SVGSVGElement* svg = viewElement.ownerSVGElement()) {
            svg->inheritViewAttributes(&viewElement);

            if (LayoutObject* layoutObject = svg->layoutObject())
                markForLayoutAndParentResourceInvalidation(layoutObject);

            return;
        }
    }

    // If we previously had a view and didn't get a new one, we need to
    // layout again.
    if (layoutObject && hadUseCurrentView)
        markForLayoutAndParentResourceInvalidation(layoutObject);

    // FIXME: We need to decide which <svg> to focus on, and zoom to it.
    // FIXME: We need to actually "highlight" the viewTarget(s).
}

void SVGSVGElement::inheritViewAttributes(SVGViewElement* viewElement)
{
    SVGViewSpec* view = currentView();
    m_useCurrentView = true;
    view->inheritViewAttributesFromElement(this);
    view->inheritViewAttributesFromElement(viewElement);
}

void SVGSVGElement::finishParsingChildren()
{
    SVGGraphicsElement::finishParsingChildren();

    // The outermost SVGSVGElement SVGLoad event is fired through Document::dispatchWindowLoadEvent.
    if (isOutermostSVGSVGElement())
        return;

    // finishParsingChildren() is called when the close tag is reached for an element (e.g. </svg>)
    // we send SVGLoad events here if we can, otherwise they'll be sent when any required loads finish
    sendSVGLoadEventIfPossible();
}

DEFINE_TRACE(SVGSVGElement)
{
    visitor->trace(m_x);
    visitor->trace(m_y);
    visitor->trace(m_width);
    visitor->trace(m_height);
    visitor->trace(m_translation);
    visitor->trace(m_timeContainer);
    visitor->trace(m_viewSpec);
    SVGGraphicsElement::trace(visitor);
    SVGFitToViewBox::trace(visitor);
}

} // namespace blink
