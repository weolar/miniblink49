/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Peter Kelly (pmk@post.com)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 *           (C) 2007 David Smith (catfish.man@gmail.com)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2012, 2013 Apple Inc. All rights reserved.
 *           (C) 2007 Eric Seidel (eric@webkit.org)
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
#include "core/dom/Element.h"

#include "bindings/core/v8/DOMDataStore.h"
#include "bindings/core/v8/Dictionary.h"
#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/V8DOMWrapper.h"
#include "bindings/core/v8/V8PerContextData.h"
#include "core/CSSValueKeywords.h"
#include "core/SVGNames.h"
#include "core/XLinkNames.h"
#include "core/XMLNames.h"
#include "core/animation/AnimationTimeline.h"
#include "core/animation/css/CSSAnimations.h"
#include "core/css/CSSImageValue.h"
#include "core/css/CSSStyleSheet.h"
#include "core/css/CSSValuePool.h"
#include "core/css/PropertySetCSSStyleDeclaration.h"
#include "core/css/StylePropertySet.h"
#include "core/css/parser/CSSParser.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/css/resolver/StyleResolverParentScope.h"
#include "core/dom/AXObjectCache.h"
#include "core/dom/Attr.h"
#include "core/dom/CSSSelectorWatch.h"
#include "core/dom/ClientRect.h"
#include "core/dom/ClientRectList.h"
#include "core/dom/DatasetDOMStringMap.h"
#include "core/dom/ElementDataCache.h"
#include "core/dom/ElementRareData.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/FirstLetterPseudoElement.h"
#include "core/dom/Fullscreen.h"
#include "core/dom/LayoutTreeBuilder.h"
#include "core/dom/MutationObserverInterestGroup.h"
#include "core/dom/MutationRecord.h"
#include "core/dom/NamedNodeMap.h"
#include "core/dom/NodeComputedStyle.h"
#include "core/dom/PresentationAttributeStyle.h"
#include "core/dom/PseudoElement.h"
#include "core/dom/ScriptableDocumentParser.h"
#include "core/dom/SelectorQuery.h"
#include "core/dom/StyleChangeReason.h"
#include "core/dom/StyleEngine.h"
#include "core/dom/Text.h"
#include "core/dom/custom/CustomElement.h"
#include "core/dom/custom/CustomElementRegistrationContext.h"
#include "core/dom/shadow/InsertionPoint.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/dom/shadow/ShadowRootInit.h"
#include "core/editing/FrameSelection.h"
#include "core/editing/htmlediting.h"
#include "core/editing/iterators/TextIterator.h"
#include "core/editing/markup.h"
#include "core/events/EventDispatcher.h"
#include "core/events/FocusEvent.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/OriginsUsingFeatures.h"
#include "core/frame/PinchViewport.h"
#include "core/frame/ScrollToOptions.h"
#include "core/frame/Settings.h"
#include "core/frame/UseCounter.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "core/html/ClassList.h"
#include "core/html/HTMLCanvasElement.h"
#include "core/html/HTMLCollection.h"
#include "core/html/HTMLDocument.h"
#include "core/html/HTMLElement.h"
#include "core/html/HTMLFormControlsCollection.h"
#include "core/html/HTMLFrameElementBase.h"
#include "core/html/HTMLFrameOwnerElement.h"
#include "core/html/HTMLOptionsCollection.h"
#include "core/html/HTMLPlugInElement.h"
#include "core/html/HTMLTableRowsCollection.h"
#include "core/html/HTMLTemplateElement.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/layout/LayoutTextFragment.h"
#include "core/layout/LayoutView.h"
#include "core/page/ChromeClient.h"
#include "core/page/FocusController.h"
#include "core/page/Page.h"
#include "core/page/PointerLockController.h"
#include "core/page/SpatialNavigation.h"
#include "core/page/scrolling/ScrollState.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/svg/SVGDocumentExtensions.h"
#include "core/svg/SVGElement.h"
#include "platform/EventDispatchForbiddenScope.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/UserGestureIndicator.h"
#include "platform/scroll/ScrollableArea.h"
#include "wtf/BitVector.h"
#include "wtf/HashFunctions.h"
#include "wtf/text/CString.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/TextPosition.h"
#include "wtf/text/WTFStringUtil.h" // weolar TODO

namespace blink {

using namespace HTMLNames;
using namespace XMLNames;

PassRefPtrWillBeRawPtr<Element> Element::create(const QualifiedName& tagName, Document* document)
{
    return adoptRefWillBeNoop(new Element(tagName, document, CreateElement));
}

Element::Element(const QualifiedName& tagName, Document* document, ConstructionType type)
    : ContainerNode(document, type)
    , m_tagName(tagName)
{
}

Element::~Element()
{
    ASSERT(needsAttach());

#if !ENABLE(OILPAN)
    if (hasRareData()) {
        elementRareData()->clearShadow();
        detachAllAttrNodesFromElement();
    }

    if (isCustomElement()) {
        CustomElement::wasDestroyed(this);
    }

    // With Oilpan, either the Element has been removed from the Document
    // or the Document is dead as well. If the Element has been removed from
    // the Document the element has already been removed from the pending
    // resources. If the document is also dead, there is no need to remove
    // the element from the pending resources.
    if (hasPendingResources()) {
        document().accessSVGExtensions().removeElementFromPendingResources(this);
        ASSERT(!hasPendingResources());
    }
#endif
}

inline ElementRareData* Element::elementRareData() const
{
    ASSERT(hasRareData());
    return static_cast<ElementRareData*>(rareData());
}

inline ElementRareData& Element::ensureElementRareData()
{
    return static_cast<ElementRareData&>(ensureRareData());
}

bool Element::hasElementFlagInternal(ElementFlags mask) const
{
    return elementRareData()->hasElementFlag(mask);
}

void Element::setElementFlag(ElementFlags mask, bool value)
{
    if (!hasRareData() && !value)
        return;
    ensureElementRareData().setElementFlag(mask, value);
}

void Element::clearElementFlag(ElementFlags mask)
{
    if (!hasRareData())
        return;
    elementRareData()->clearElementFlag(mask);
}

void Element::clearTabIndexExplicitlyIfNeeded()
{
    if (hasRareData())
        elementRareData()->clearTabIndexExplicitly();
}

void Element::setTabIndexExplicitly(short tabIndex)
{
    ensureElementRareData().setTabIndexExplicitly(tabIndex);
}

void Element::setTabIndex(int value)
{
    setIntegralAttribute(tabindexAttr, value);
}

short Element::tabIndex() const
{
    return hasRareData() ? elementRareData()->tabIndex() : 0;
}

bool Element::layoutObjectIsFocusable() const
{
    // Elements in canvas fallback content are not rendered, but they are allowed to be
    // focusable as long as their canvas is displayed and visible.
    if (isInCanvasSubtree()) {
        const HTMLCanvasElement* canvas = Traversal<HTMLCanvasElement>::firstAncestorOrSelf(*this);
        ASSERT(canvas);
        return canvas->layoutObject() && canvas->layoutObject()->style()->visibility() == VISIBLE;
    }

    // FIXME: These asserts should be in Node::isFocusable, but there are some
    // callsites like Document::setFocusedElement that would currently fail on
    // them. See crbug.com/251163
    if (!layoutObject()) {
        // We can't just use needsStyleRecalc() because if the node is in a
        // display:none tree it might say it needs style recalc but the whole
        // document is actually up to date.
        // In addition, style cannot be cleared out for non-active documents,
        // so in that case the childNeedsStyleRecalc check is invalid.
        ASSERT(!document().isActive() || !document().childNeedsStyleRecalc());
    }

    // FIXME: Even if we are not visible, we might have a child that is visible.
    // Hyatt wants to fix that some day with a "has visible content" flag or the like.
    if (!layoutObject() || layoutObject()->style()->visibility() != VISIBLE)
        return false;

    return true;
}

PassRefPtrWillBeRawPtr<Node> Element::cloneNode(bool deep)
{
    return deep ? cloneElementWithChildren() : cloneElementWithoutChildren();
}

PassRefPtrWillBeRawPtr<Element> Element::cloneElementWithChildren()
{
    RefPtrWillBeRawPtr<Element> clone = cloneElementWithoutChildren();
    cloneChildNodes(clone.get());
    return clone.release();
}

PassRefPtrWillBeRawPtr<Element> Element::cloneElementWithoutChildren()
{
    RefPtrWillBeRawPtr<Element> clone = cloneElementWithoutAttributesAndChildren();
    // This will catch HTML elements in the wrong namespace that are not correctly copied.
    // This is a sanity check as HTML overloads some of the DOM methods.
    ASSERT(isHTMLElement() == clone->isHTMLElement());

    clone->cloneDataFromElement(*this);
    return clone.release();
}

PassRefPtrWillBeRawPtr<Element> Element::cloneElementWithoutAttributesAndChildren()
{
    return document().createElement(tagQName(), false);
}

PassRefPtrWillBeRawPtr<Attr> Element::detachAttribute(size_t index)
{
    ASSERT(elementData());
    const Attribute& attribute = elementData()->attributes().at(index);
    RefPtrWillBeRawPtr<Attr> attrNode = attrIfExists(attribute.name());
    if (attrNode) {
        detachAttrNodeAtIndex(attrNode.get(), index);
    } else {
        attrNode = Attr::create(document(), attribute.name(), attribute.value());
        removeAttributeInternal(index, NotInSynchronizationOfLazyAttribute);
    }
    return attrNode.release();
}

void Element::detachAttrNodeAtIndex(Attr* attr, size_t index)
{
    ASSERT(attr);
    ASSERT(elementData());

    const Attribute& attribute = elementData()->attributes().at(index);
    ASSERT(attribute.name() == attr->qualifiedName());
    detachAttrNodeFromElementWithValue(attr, attribute.value());
    removeAttributeInternal(index, NotInSynchronizationOfLazyAttribute);
}

void Element::removeAttribute(const QualifiedName& name)
{
    if (!elementData())
        return;

    size_t index = elementData()->attributes().findIndex(name);
    if (index == kNotFound)
        return;

    removeAttributeInternal(index, NotInSynchronizationOfLazyAttribute);
}

void Element::setBooleanAttribute(const QualifiedName& name, bool value)
{
    if (value)
        setAttribute(name, emptyAtom);
    else
        removeAttribute(name);
}

NamedNodeMap* Element::attributesForBindings() const
{
    ElementRareData& rareData = const_cast<Element*>(this)->ensureElementRareData();
    if (NamedNodeMap* attributeMap = rareData.attributeMap())
        return attributeMap;

    rareData.setAttributeMap(NamedNodeMap::create(const_cast<Element*>(this)));
    return rareData.attributeMap();
}

ElementAnimations* Element::elementAnimations() const
{
    if (hasRareData())
        return elementRareData()->elementAnimations();
    return nullptr;
}

ElementAnimations& Element::ensureElementAnimations()
{
    ElementRareData& rareData = ensureElementRareData();
    if (!rareData.elementAnimations())
        rareData.setElementAnimations(adoptPtrWillBeNoop(new ElementAnimations()));
    return *rareData.elementAnimations();
}

bool Element::hasAnimations() const
{
    if (!hasRareData())
        return false;

    ElementAnimations* elementAnimations = elementRareData()->elementAnimations();
    return elementAnimations && !elementAnimations->isEmpty();
}

Node::NodeType Element::nodeType() const
{
    return ELEMENT_NODE;
}

bool Element::hasAttribute(const QualifiedName& name) const
{
    return hasAttributeNS(name.namespaceURI(), name.localName());
}

void Element::synchronizeAllAttributes() const
{
    if (!elementData())
        return;
    // NOTE: anyAttributeMatches in SelectorChecker.cpp
    // currently assumes that all lazy attributes have a null namespace.
    // If that ever changes we'll need to fix that code.
    if (elementData()->m_styleAttributeIsDirty) {
        ASSERT(isStyledElement());
        synchronizeStyleAttributeInternal();
    }
    if (elementData()->m_animatedSVGAttributesAreDirty) {
        ASSERT(isSVGElement());
        toSVGElement(this)->synchronizeAnimatedSVGAttribute(anyQName());
    }
}

inline void Element::synchronizeAttribute(const QualifiedName& name) const
{
    if (!elementData())
        return;
    if (UNLIKELY(name == styleAttr && elementData()->m_styleAttributeIsDirty)) {
        ASSERT(isStyledElement());
        synchronizeStyleAttributeInternal();
        return;
    }
    if (UNLIKELY(elementData()->m_animatedSVGAttributesAreDirty)) {
        ASSERT(isSVGElement());
        // See comment in the AtomicString version of synchronizeAttribute()
        // also.
        toSVGElement(this)->synchronizeAnimatedSVGAttribute(name);
    }
}

void Element::synchronizeAttribute(const AtomicString& localName) const
{
    // This version of synchronizeAttribute() is streamlined for the case where you don't have a full QualifiedName,
    // e.g when called from DOM API.
    if (!elementData())
        return;
    if (elementData()->m_styleAttributeIsDirty && equalPossiblyIgnoringCase(localName, styleAttr.localName(), shouldIgnoreAttributeCase())) {
        ASSERT(isStyledElement());
        synchronizeStyleAttributeInternal();
        return;
    }
    if (elementData()->m_animatedSVGAttributesAreDirty) {
        // We're not passing a namespace argument on purpose. SVGNames::*Attr are defined w/o namespaces as well.

        // FIXME: this code is called regardless of whether name is an
        // animated SVG Attribute. It would seem we should only call this method
        // if SVGElement::isAnimatableAttribute is true, but the list of
        // animatable attributes in isAnimatableAttribute does not suffice to
        // pass all layout tests. Also, m_animatedSVGAttributesAreDirty stays
        // dirty unless synchronizeAnimatedSVGAttribute is called with
        // anyQName(). This means that even if Element::synchronizeAttribute()
        // is called on all attributes, m_animatedSVGAttributesAreDirty remains
        // true.
        toSVGElement(this)->synchronizeAnimatedSVGAttribute(QualifiedName(nullAtom, localName, nullAtom));
    }
}

const AtomicString& Element::getAttribute(const QualifiedName& name) const
{
    if (!elementData())
        return nullAtom;
    synchronizeAttribute(name);
    if (const Attribute* attribute = elementData()->attributes().find(name))
        return attribute->value();
    return nullAtom;
}

bool Element::shouldIgnoreAttributeCase() const
{
    return isHTMLElement() && document().isHTMLDocument();
}

void Element::scrollIntoView(bool alignToTop)
{
    document().updateLayoutIgnorePendingStylesheets();

    if (!layoutObject())
        return;

    LayoutRect bounds = boundingBox();
    // Align to the top / bottom and to the closest edge.
    if (alignToTop)
        layoutObject()->scrollRectToVisible(bounds, ScrollAlignment::alignToEdgeIfNeeded, ScrollAlignment::alignTopAlways);
    else
        layoutObject()->scrollRectToVisible(bounds, ScrollAlignment::alignToEdgeIfNeeded, ScrollAlignment::alignBottomAlways);
}

void Element::scrollIntoViewIfNeeded(bool centerIfNeeded)
{
    document().updateLayoutIgnorePendingStylesheets();

    if (!layoutObject())
        return;

    LayoutRect bounds = boundingBox();
    if (centerIfNeeded)
        layoutObject()->scrollRectToVisible(bounds, ScrollAlignment::alignCenterIfNeeded, ScrollAlignment::alignCenterIfNeeded);
    else
        layoutObject()->scrollRectToVisible(bounds, ScrollAlignment::alignToEdgeIfNeeded, ScrollAlignment::alignToEdgeIfNeeded);
}

void Element::distributeScroll(ScrollState& scrollState)
{
    ASSERT(RuntimeEnabledFeatures::scrollCustomizationEnabled());
    if (scrollState.fullyConsumed())
        return;

    scrollState.distributeToScrollChainDescendant();

    // If the scroll doesn't propagate, and we're currently scrolling
    // an element other than this one, prevent the scroll from
    // propagating to this element.
    if (!scrollState.shouldPropagate()
        && scrollState.deltaConsumedForScrollSequence()
        && scrollState.currentNativeScrollingElement() != this) {
        return;
    }

    const double deltaX = scrollState.deltaX();
    const double deltaY = scrollState.deltaY();

    applyScroll(scrollState);

    if (deltaX != scrollState.deltaX() || deltaY != scrollState.deltaY())
        scrollState.setCurrentNativeScrollingElement(this);
}

void Element::applyScroll(ScrollState& scrollState)
{
    ASSERT(RuntimeEnabledFeatures::scrollCustomizationEnabled());
    if (scrollState.fullyConsumed())
        return;

    const double deltaX = scrollState.deltaX();
    const double deltaY = scrollState.deltaY();
    bool scrolled = false;

    // Handle the documentElement separately, as it scrolls the FrameView.
    if (this == document().documentElement()) {
        FloatSize delta(deltaX, deltaY);
        if (document().frame()->applyScrollDelta(delta, scrollState.isBeginning()).didScroll()) {
            scrolled = true;
            scrollState.consumeDeltaNative(scrollState.deltaX(), scrollState.deltaY());
        }
    } else {
        if (!layoutObject())
            return;
        LayoutBox* curBox = layoutObject()->enclosingBox();
        // FIXME: Native scrollers should only consume the scroll they
        // apply. See crbug.com/457765.
        if (deltaX && curBox->scroll(ScrollLeft, ScrollByPrecisePixel, deltaX).didScroll) {
            scrollState.consumeDeltaNative(scrollState.deltaX(), 0);
            scrolled = true;
        }

        if (deltaY && curBox->scroll(ScrollUp, ScrollByPrecisePixel, deltaY).didScroll) {
            scrollState.consumeDeltaNative(0, scrollState.deltaY());
            scrolled = true;
        }
    }

    if (!scrolled)
        return;

    // We need to setCurrentNativeScrollingElement in both the
    // distributeScroll and applyScroll default implementations so
    // that if JS overrides one of these methods, but not the
    // other, this bookkeeping remains accurate.
    scrollState.setCurrentNativeScrollingElement(this);
    if (scrollState.fromUserInput())
        document().frame()->view()->setWasScrolledByUser(true);
};

static float localZoomForLayoutObject(LayoutObject& layoutObject)
{
    // FIXME: This does the wrong thing if two opposing zooms are in effect and canceled each
    // other out, but the alternative is that we'd have to crawl up the whole layout tree every
    // time (or store an additional bit in the ComputedStyle to indicate that a zoom was specified).
    float zoomFactor = 1;
    if (layoutObject.style()->effectiveZoom() != 1) {
        // Need to find the nearest enclosing LayoutObject that set up
        // a differing zoom, and then we divide our result by it to eliminate the zoom.
        LayoutObject* prev = &layoutObject;
        for (LayoutObject* curr = prev->parent(); curr; curr = curr->parent()) {
            if (curr->style()->effectiveZoom() != prev->style()->effectiveZoom()) {
                zoomFactor = prev->style()->zoom();
                break;
            }
            prev = curr;
        }
        if (prev->isLayoutView())
            zoomFactor = prev->style()->zoom();
    }
    return zoomFactor;
}

static double adjustForLocalZoom(LayoutUnit value, LayoutObject& layoutObject)
{
    float zoomFactor = localZoomForLayoutObject(layoutObject);
    if (zoomFactor == 1)
        return value.toDouble();
    return value.toDouble() / zoomFactor;
}

int Element::offsetLeft()
{
    document().updateLayoutIgnorePendingStylesheets();
    if (LayoutBoxModelObject* layoutObject = layoutBoxModelObject())
        return lroundf(adjustForLocalZoom(layoutObject->offsetLeft(), *layoutObject));
    return 0;
}

int Element::offsetTop()
{
    document().updateLayoutIgnorePendingStylesheets();
    if (LayoutBoxModelObject* layoutObject = layoutBoxModelObject())
        return lroundf(adjustForLocalZoom(layoutObject->pixelSnappedOffsetTop(), *layoutObject));
    return 0;
}

int Element::offsetWidth()
{
    document().updateLayoutIgnorePendingStylesheets();
    if (LayoutBoxModelObject* layoutObject = layoutBoxModelObject())
        return adjustLayoutUnitForAbsoluteZoom(layoutObject->pixelSnappedOffsetWidth(), *layoutObject).round();
    return 0;
}

int Element::offsetHeight()
{
    document().updateLayoutIgnorePendingStylesheets();
    if (LayoutBoxModelObject* layoutObject = layoutBoxModelObject())
        return adjustLayoutUnitForAbsoluteZoom(layoutObject->pixelSnappedOffsetHeight(), *layoutObject).round();
    return 0;
}

Element* Element::offsetParent()
{
    document().updateLayoutIgnorePendingStylesheets();

    LayoutObject* layoutObject = this->layoutObject();
    if (!layoutObject)
        return nullptr;

    Element* element = layoutObject->offsetParent();
    if (!element)
        return nullptr;

    if (element->isInShadowTree() && !element->containingShadowRoot()->shouldExposeToBindings())
        return nullptr;

    return element;
}

int Element::clientLeft()
{
    document().updateLayoutIgnorePendingStylesheets();

    if (LayoutBox* layoutObject = layoutBox())
        return adjustLayoutUnitForAbsoluteZoom(roundToInt(layoutObject->clientLeft()), *layoutObject);
    return 0;
}

int Element::clientTop()
{
    document().updateLayoutIgnorePendingStylesheets();

    if (LayoutBox* layoutObject = layoutBox())
        return adjustLayoutUnitForAbsoluteZoom(roundToInt(layoutObject->clientTop()), *layoutObject);
    return 0;
}

int Element::clientWidth()
{
    document().updateLayoutIgnorePendingStylesheets();

    // When in strict mode, clientWidth for the document element should return the width of the containing frame.
    // When in quirks mode, clientWidth for the body element should return the width of the containing frame.
    bool inQuirksMode = document().inQuirksMode();
    if ((!inQuirksMode && document().documentElement() == this)
        || (inQuirksMode && isHTMLElement() && document().body() == this)) {
        if (FrameView* view = document().view()) {
            if (LayoutView* layoutView = document().layoutView()) {
                if (document().page()->settings().forceZeroLayoutHeight())
                    return adjustLayoutUnitForAbsoluteZoom(view->visibleContentSize().width(), *layoutView);
                return adjustLayoutUnitForAbsoluteZoom(view->layoutSize().width(), *layoutView);
            }
        }
    }

    if (LayoutBox* layoutObject = layoutBox())
        return adjustLayoutUnitForAbsoluteZoom(layoutObject->pixelSnappedClientWidth(), *layoutObject).round();
    return 0;
}

int Element::clientHeight()
{
    document().updateLayoutIgnorePendingStylesheets();

    // When in strict mode, clientHeight for the document element should return the height of the containing frame.
    // When in quirks mode, clientHeight for the body element should return the height of the containing frame.
    bool inQuirksMode = document().inQuirksMode();

    if ((!inQuirksMode && document().documentElement() == this)
        || (inQuirksMode && isHTMLElement() && document().body() == this)) {
        if (FrameView* view = document().view()) {
            if (LayoutView* layoutView = document().layoutView()) {
                if (document().page()->settings().forceZeroLayoutHeight())
                    return adjustLayoutUnitForAbsoluteZoom(view->visibleContentSize().height(), *layoutView);
                return adjustLayoutUnitForAbsoluteZoom(view->layoutSize().height(), *layoutView);
            }
        }
    }

    if (LayoutBox* layoutObject = layoutBox())
        return adjustLayoutUnitForAbsoluteZoom(layoutObject->pixelSnappedClientHeight(), *layoutObject).round();
    return 0;
}

double Element::scrollLeft()
{
    document().updateLayoutIgnorePendingStylesheets();

    if (document().scrollingElement() == this) {
        if (document().domWindow())
            return document().domWindow()->scrollX();
        return 0;
    }

    if (LayoutBox* box = layoutBox())
        return adjustScrollForAbsoluteZoom(box->scrollLeft(), *box);

    return 0;
}

double Element::scrollTop()
{
    document().updateLayoutIgnorePendingStylesheets();

    if (document().scrollingElement() == this) {
        if (document().domWindow())
            return document().domWindow()->scrollY();
        return 0;
    }

    if (LayoutBox* box = layoutBox())
        return adjustScrollForAbsoluteZoom(box->scrollTop(), *box);

    return 0;
}

void Element::setScrollLeft(double newLeft)
{
    document().updateLayoutIgnorePendingStylesheets();

    newLeft = ScrollableArea::normalizeNonFiniteScroll(newLeft);

    if (document().scrollingElement() == this) {
        if (LocalDOMWindow* window = document().domWindow())
            window->scrollTo(newLeft, window->scrollY());
    } else {
        LayoutBox* box = layoutBox();
        if (box)
            box->setScrollLeft(LayoutUnit::fromFloatRound(newLeft * box->style()->effectiveZoom()));
    }
}

void Element::setScrollTop(double newTop)
{
    document().updateLayoutIgnorePendingStylesheets();

    newTop = ScrollableArea::normalizeNonFiniteScroll(newTop);

    if (document().scrollingElement() == this) {
        if (LocalDOMWindow* window = document().domWindow())
            window->scrollTo(window->scrollX(), newTop);
    } else {
        LayoutBox* box = layoutBox();
        if (box)
            box->setScrollTop(LayoutUnit::fromFloatRound(newTop * box->style()->effectiveZoom()));
    }
}

int Element::scrollWidth()
{
    document().updateLayoutIgnorePendingStylesheets();

    if (document().scrollingElement() == this) {
        if (document().view())
            return adjustForAbsoluteZoom(document().view()->contentsWidth(), document().frame()->pageZoomFactor());
        return 0;
    }

    if (LayoutBox* box = layoutBox())
        return adjustLayoutUnitForAbsoluteZoom(box->scrollWidth(), *box).round();
    return 0;
}

int Element::scrollHeight()
{
    document().updateLayoutIgnorePendingStylesheets();

    if (document().scrollingElement() == this) {
        if (document().view())
            return adjustForAbsoluteZoom(document().view()->contentsHeight(), document().frame()->pageZoomFactor());
        return 0;
    }

    if (LayoutBox* box = layoutBox())
        return adjustLayoutUnitForAbsoluteZoom(box->scrollHeight(), *box).round();
    return 0;
}

void Element::scrollBy(double x, double y)
{
    ScrollToOptions scrollToOptions;
    scrollToOptions.setLeft(x);
    scrollToOptions.setTop(y);
    scrollBy(scrollToOptions);
}

void Element::scrollBy(const ScrollToOptions& scrollToOptions)
{
    // FIXME: This should be removed once scroll updates are processed only after
    // the compositing update. See http://crbug.com/420741.
    document().updateLayoutIgnorePendingStylesheets();

    if (document().scrollingElement() == this) {
        scrollFrameBy(scrollToOptions);
    } else {
        scrollLayoutBoxBy(scrollToOptions);
    }
}

void Element::scrollTo(double x, double y)
{
    ScrollToOptions scrollToOptions;
    scrollToOptions.setLeft(x);
    scrollToOptions.setTop(y);
    scrollTo(scrollToOptions);
}

void Element::scrollTo(const ScrollToOptions& scrollToOptions)
{
    // FIXME: This should be removed once scroll updates are processed only after
    // the compositing update. See http://crbug.com/420741.
    document().updateLayoutIgnorePendingStylesheets();

    if (document().scrollingElement() == this) {
        scrollFrameTo(scrollToOptions);
    } else {
        scrollLayoutBoxTo(scrollToOptions);
    }
}

void Element::scrollLayoutBoxBy(const ScrollToOptions& scrollToOptions)
{
    double left = scrollToOptions.hasLeft() ? ScrollableArea::normalizeNonFiniteScroll(scrollToOptions.left()) : 0.0;
    double top = scrollToOptions.hasTop() ? ScrollableArea::normalizeNonFiniteScroll(scrollToOptions.top()) : 0.0;

    ScrollBehavior scrollBehavior = ScrollBehaviorAuto;
    ScrollableArea::scrollBehaviorFromString(scrollToOptions.behavior(), scrollBehavior);
    LayoutBox* box = layoutBox();
    if (box) {
        double currentScaledLeft = box->scrollLeft();
        double currentScaledTop = box->scrollTop();
        double newScaledLeft = left * box->style()->effectiveZoom() + currentScaledLeft;
        double newScaledTop = top * box->style()->effectiveZoom() + currentScaledTop;
        box->scrollToOffset(DoubleSize(newScaledLeft, newScaledTop), scrollBehavior);
    }
}

void Element::scrollLayoutBoxTo(const ScrollToOptions& scrollToOptions)
{
    ScrollBehavior scrollBehavior = ScrollBehaviorAuto;
    ScrollableArea::scrollBehaviorFromString(scrollToOptions.behavior(), scrollBehavior);

    LayoutBox* box = layoutBox();
    if (box) {
        double scaledLeft = box->scrollLeft();
        double scaledTop = box->scrollTop();
        if (scrollToOptions.hasLeft())
            scaledLeft = ScrollableArea::normalizeNonFiniteScroll(scrollToOptions.left()) * box->style()->effectiveZoom();
        if (scrollToOptions.hasTop())
            scaledTop = ScrollableArea::normalizeNonFiniteScroll(scrollToOptions.top()) * box->style()->effectiveZoom();
        box->scrollToOffset(DoubleSize(scaledLeft, scaledTop), scrollBehavior);
    }
}

void Element::scrollFrameBy(const ScrollToOptions& scrollToOptions)
{
    double left = scrollToOptions.hasLeft() ? ScrollableArea::normalizeNonFiniteScroll(scrollToOptions.left()) : 0.0;
    double top = scrollToOptions.hasTop() ? ScrollableArea::normalizeNonFiniteScroll(scrollToOptions.top()) : 0.0;

    ScrollBehavior scrollBehavior = ScrollBehaviorAuto;
    ScrollableArea::scrollBehaviorFromString(scrollToOptions.behavior(), scrollBehavior);
    LocalFrame* frame = document().frame();
    if (!frame)
        return;
    FrameView* view = frame->view();
    if (!view)
        return;

    double newScaledLeft = left * frame->pageZoomFactor() + view->scrollPositionDouble().x();
    double newScaledTop = top * frame->pageZoomFactor() + view->scrollPositionDouble().y();
    view->setScrollPosition(DoublePoint(newScaledLeft, newScaledTop), ProgrammaticScroll, scrollBehavior);
}

void Element::scrollFrameTo(const ScrollToOptions& scrollToOptions)
{
    ScrollBehavior scrollBehavior = ScrollBehaviorAuto;
    ScrollableArea::scrollBehaviorFromString(scrollToOptions.behavior(), scrollBehavior);
    LocalFrame* frame = document().frame();
    if (!frame)
        return;
    FrameView* view = frame->view();
    if (!view)
        return;

    double scaledLeft = view->scrollPositionDouble().x();
    double scaledTop = view->scrollPositionDouble().y();
    if (scrollToOptions.hasLeft())
        scaledLeft = ScrollableArea::normalizeNonFiniteScroll(scrollToOptions.left()) * frame->pageZoomFactor();
    if (scrollToOptions.hasTop())
        scaledTop = ScrollableArea::normalizeNonFiniteScroll(scrollToOptions.top()) * frame->pageZoomFactor();
    view->setScrollPosition(DoublePoint(scaledLeft, scaledTop), ProgrammaticScroll, scrollBehavior);
}

void Element::incrementProxyCount()
{
    if (ensureElementRareData().incrementProxyCount() == 1)
        setNeedsStyleRecalc(LocalStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::CompositorProxy));
}

void Element::decrementProxyCount()
{
    if (ensureElementRareData().decrementProxyCount() == 0)
        setNeedsStyleRecalc(LocalStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::CompositorProxy));
}

bool Element::hasNonEmptyLayoutSize() const
{
    document().updateLayoutIgnorePendingStylesheets();

    if (LayoutBoxModelObject* box = layoutBoxModelObject())
        return box->hasNonEmptyLayoutSize();
    return false;
}

IntRect Element::boundsInViewportSpace()
{
    document().updateLayoutIgnorePendingStylesheets();

    FrameView* view = document().view();
    if (!view)
        return IntRect();

    Vector<FloatQuad> quads;
    if (isSVGElement() && layoutObject()) {
        // Get the bounding rectangle from the SVG model.
        SVGElement* svgElement = toSVGElement(this);
        FloatRect localRect;
        if (svgElement->getBoundingBox(localRect))
            quads.append(layoutObject()->localToAbsoluteQuad(localRect));
    } else {
        // Get the bounding rectangle from the box model.
        if (layoutBoxModelObject())
            layoutBoxModelObject()->absoluteQuads(quads);
    }

    if (quads.isEmpty())
        return IntRect();

    IntRect result = quads[0].enclosingBoundingBox();
    for (size_t i = 1; i < quads.size(); ++i)
        result.unite(quads[i].enclosingBoundingBox());

    return view->soonToBeRemovedContentsToUnscaledViewport(result);
}

ClientRectList* Element::getClientRects()
{
    document().updateLayoutIgnorePendingStylesheets();

    LayoutObject* elementLayoutObject = layoutObject();
    if (!elementLayoutObject || (!elementLayoutObject->isBoxModelObject() && !elementLayoutObject->isBR()))
        return ClientRectList::create();

    // FIXME: Handle SVG elements.
    // FIXME: Handle table/inline-table with a caption.

    Vector<FloatQuad> quads;
    elementLayoutObject->absoluteQuads(quads);
    document().adjustFloatQuadsForScrollAndAbsoluteZoom(quads, *elementLayoutObject);
    return ClientRectList::create(quads);
}

ClientRect* Element::getBoundingClientRect()
{
    document().updateLayoutIgnorePendingStylesheets();

    Vector<FloatQuad> quads;
    LayoutObject* elementLayoutObject = layoutObject();
    if (elementLayoutObject) {
        if (isSVGElement() && !elementLayoutObject->isSVGRoot()) {
            // Get the bounding rectangle from the SVG model.
            SVGElement* svgElement = toSVGElement(this);
            FloatRect localRect;
            if (svgElement->getBoundingBox(localRect))
                quads.append(elementLayoutObject->localToAbsoluteQuad(localRect));
        } else if (elementLayoutObject->isBoxModelObject() || elementLayoutObject->isBR()) {
            elementLayoutObject->absoluteQuads(quads);
        }
    }

    if (quads.isEmpty())
        return ClientRect::create();

    FloatRect result = quads[0].boundingBox();
    for (size_t i = 1; i < quads.size(); ++i)
        result.unite(quads[i].boundingBox());

    ASSERT(elementLayoutObject);
    document().adjustFloatRectForScrollAndAbsoluteZoom(result, *elementLayoutObject);

    return ClientRect::create(result);
}

IntRect Element::screenRect() const
{
    if (!layoutObject())
        return IntRect();
    // FIXME: this should probably respect transforms
    return document().view()->contentsToScreen(layoutObject()->absoluteBoundingBoxRectIgnoringTransforms());
}

const AtomicString& Element::computedRole()
{
    document().updateLayoutIgnorePendingStylesheets();
    OwnPtr<ScopedAXObjectCache> cache = ScopedAXObjectCache::create(document());
    return cache->get()->computedRoleForNode(this);
}

String Element::computedName()
{
    document().updateLayoutIgnorePendingStylesheets();
    OwnPtr<ScopedAXObjectCache> cache = ScopedAXObjectCache::create(document());
    return cache->get()->computedNameForNode(this);
}

const AtomicString& Element::getAttribute(const AtomicString& localName) const
{
    if (!elementData())
        return nullAtom;
    synchronizeAttribute(localName);
    if (const Attribute* attribute = elementData()->attributes().find(localName, shouldIgnoreAttributeCase()))
        return attribute->value();
    return nullAtom;
}

const AtomicString& Element::getAttributeNS(const AtomicString& namespaceURI, const AtomicString& localName) const
{
    return getAttribute(QualifiedName(nullAtom, localName, namespaceURI));
}

void Element::setAttribute(const AtomicString& localName, const AtomicString& value, ExceptionState& exceptionState)
{
    if (!Document::isValidName(localName)) {
        exceptionState.throwDOMException(InvalidCharacterError, "'" + localName + "' is not a valid attribute name.");
        return;
    }

    synchronizeAttribute(localName);
    const AtomicString& caseAdjustedLocalName = shouldIgnoreAttributeCase() ? localName.lower() : localName;

    if (!elementData()) {
        setAttributeInternal(kNotFound, QualifiedName(nullAtom, caseAdjustedLocalName, nullAtom), value, NotInSynchronizationOfLazyAttribute);
        return;
    }

    AttributeCollection attributes = elementData()->attributes();
    size_t index = attributes.findIndex(caseAdjustedLocalName, false);
    const QualifiedName& qName = index != kNotFound ? attributes[index].name() : QualifiedName(nullAtom, caseAdjustedLocalName, nullAtom);
    setAttributeInternal(index, qName, value, NotInSynchronizationOfLazyAttribute);
}

void Element::setAttribute(const QualifiedName& name, const AtomicString& value)
{
    synchronizeAttribute(name);
    size_t index = elementData() ? elementData()->attributes().findIndex(name) : kNotFound;
    setAttributeInternal(index, name, value, NotInSynchronizationOfLazyAttribute);
}

void Element::setSynchronizedLazyAttribute(const QualifiedName& name, const AtomicString& value)
{
    size_t index = elementData() ? elementData()->attributes().findIndex(name) : kNotFound;
    setAttributeInternal(index, name, value, InSynchronizationOfLazyAttribute);
}

ALWAYS_INLINE void Element::setAttributeInternal(size_t index, const QualifiedName& name, const AtomicString& newValue, SynchronizationOfLazyAttribute inSynchronizationOfLazyAttribute)
{
    if (newValue.isNull()) {
        if (index != kNotFound)
            removeAttributeInternal(index, inSynchronizationOfLazyAttribute);
        return;
    }

    if (index == kNotFound) {
        appendAttributeInternal(name, newValue, inSynchronizationOfLazyAttribute);
        return;
    }

    const Attribute& existingAttribute = elementData()->attributes().at(index);
    AtomicString existingAttributeValue = existingAttribute.value();
    QualifiedName existingAttributeName = existingAttribute.name();

    if (!inSynchronizationOfLazyAttribute)
        willModifyAttribute(existingAttributeName, existingAttributeValue, newValue);
    if (newValue != existingAttributeValue)
        ensureUniqueElementData().attributes().at(index).setValue(newValue);
    if (!inSynchronizationOfLazyAttribute)
        didModifyAttribute(existingAttributeName, existingAttributeValue, newValue);
}

static inline AtomicString makeIdForStyleResolution(const AtomicString& value, bool inQuirksMode)
{
    if (inQuirksMode)
        return value.lower();
    return value;
}

void Element::attributeChanged(const QualifiedName& name, const AtomicString& newValue, AttributeModificationReason reason)
{
    if (ElementShadow* parentElementShadow = shadowWhereNodeCanBeDistributed(*this)) {
        if (shouldInvalidateDistributionWhenAttributeChanged(parentElementShadow, name, newValue))
            parentElementShadow->setNeedsDistributionRecalc();
    }

    parseAttribute(name, newValue);

    document().incDOMTreeVersion();

    StyleResolver* styleResolver = document().styleResolver();

    if (name == HTMLNames::idAttr) {
        AtomicString oldId = elementData()->idForStyleResolution();
        AtomicString newId = makeIdForStyleResolution(newValue, document().inQuirksMode());
        if (newId != oldId) {
            elementData()->setIdForStyleResolution(newId);
            if (inActiveDocument() && styleResolver && styleChangeType() < SubtreeStyleChange)
                document().styleEngine().idChangedForElement(oldId, newId, *this);
        }
    } else if (name == classAttr) {
        classAttributeChanged(newValue);
    } else if (name == HTMLNames::nameAttr) {
        setHasName(!newValue.isNull());
    } else if (isStyledElement()) {
        if (name == styleAttr) {
            styleAttributeChanged(newValue, reason);
        } else if (isPresentationAttribute(name)) {
            elementData()->m_presentationAttributeStyleIsDirty = true;
            setNeedsStyleRecalc(LocalStyleChange, StyleChangeReasonForTracing::fromAttribute(name));
        }
    }

    invalidateNodeListCachesInAncestors(&name, this);

    // If there is currently no StyleResolver, we can't be sure that this attribute change won't affect style.
    if (!styleResolver)
        setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::fromAttribute(name));

    if (inDocument()) {
        if (AXObjectCache* cache = document().existingAXObjectCache())
            cache->handleAttributeChanged(name, this);
    }
}

bool Element::hasLegalLinkAttribute(const QualifiedName&) const
{
    return false;
}

const QualifiedName& Element::subResourceAttributeName() const
{
    return QualifiedName::null();
}

inline void Element::attributeChangedFromParserOrByCloning(const QualifiedName& name, const AtomicString& newValue, AttributeModificationReason reason)
{
    if (name == isAttr)
        CustomElementRegistrationContext::setTypeExtension(this, newValue);
    attributeChanged(name, newValue, reason);
}

template <typename CharacterType>
static inline bool classStringHasClassName(const CharacterType* characters, unsigned length)
{
    ASSERT(length > 0);

    unsigned i = 0;
    do {
        if (isNotHTMLSpace<CharacterType>(characters[i]))
            break;
        ++i;
    } while (i < length);

    return i < length;
}

static inline bool classStringHasClassName(const AtomicString& newClassString)
{
    unsigned length = newClassString.length();

    if (!length)
        return false;

    if (newClassString.is8Bit())
        return classStringHasClassName(newClassString.characters8(), length);
    return classStringHasClassName(newClassString.characters16(), length);
}

void Element::classAttributeChanged(const AtomicString& newClassString)
{
    StyleResolver* styleResolver = document().styleResolver();
    bool testShouldInvalidateStyle = inActiveDocument() && styleResolver && styleChangeType() < SubtreeStyleChange;

    ASSERT(elementData());
    if (classStringHasClassName(newClassString)) {
        const bool shouldFoldCase = document().inQuirksMode();
        const SpaceSplitString oldClasses = elementData()->classNames();
        elementData()->setClass(newClassString, shouldFoldCase);
        const SpaceSplitString& newClasses = elementData()->classNames();
        if (testShouldInvalidateStyle)
            document().styleEngine().classChangedForElement(oldClasses, newClasses, *this);
    } else {
        const SpaceSplitString& oldClasses = elementData()->classNames();
        if (testShouldInvalidateStyle)
            document().styleEngine().classChangedForElement(oldClasses, *this);
        elementData()->clearClass();
    }

    if (hasRareData())
        elementRareData()->clearClassListValueForQuirksMode();
}

bool Element::shouldInvalidateDistributionWhenAttributeChanged(ElementShadow* elementShadow, const QualifiedName& name, const AtomicString& newValue)
{
    ASSERT(elementShadow);
    const SelectRuleFeatureSet& featureSet = elementShadow->ensureSelectFeatureSet();

    if (name == HTMLNames::idAttr) {
        AtomicString oldId = elementData()->idForStyleResolution();
        AtomicString newId = makeIdForStyleResolution(newValue, document().inQuirksMode());
        if (newId != oldId) {
            if (!oldId.isEmpty() && featureSet.hasSelectorForId(oldId))
                return true;
            if (!newId.isEmpty() && featureSet.hasSelectorForId(newId))
                return true;
        }
    }

    if (name == HTMLNames::classAttr) {
        const AtomicString& newClassString = newValue;
        if (classStringHasClassName(newClassString)) {
            const SpaceSplitString& oldClasses = elementData()->classNames();
            const SpaceSplitString newClasses(newClassString, document().inQuirksMode() ? SpaceSplitString::ShouldFoldCase : SpaceSplitString::ShouldNotFoldCase);
            if (featureSet.checkSelectorsForClassChange(oldClasses, newClasses))
                return true;
        } else {
            const SpaceSplitString& oldClasses = elementData()->classNames();
            if (featureSet.checkSelectorsForClassChange(oldClasses))
                return true;
        }
    }

    return featureSet.hasSelectorForAttribute(name.localName());
}

// Returns true is the given attribute is an event handler.
// We consider an event handler any attribute that begins with "on".
// It is a simple solution that has the advantage of not requiring any
// code or configuration change if a new event handler is defined.

static inline bool isEventHandlerAttribute(const Attribute& attribute)
{
    return attribute.name().namespaceURI().isNull() && attribute.name().localName().startsWith("on");
}

bool Element::attributeValueIsJavaScriptURL(const Attribute& attribute)
{
    return protocolIsJavaScript(stripLeadingAndTrailingHTMLSpaces(attribute.value()));
}

bool Element::isJavaScriptURLAttribute(const Attribute& attribute) const
{
    return isURLAttribute(attribute) && attributeValueIsJavaScriptURL(attribute);
}

void Element::stripScriptingAttributes(Vector<Attribute>& attributeVector) const
{
    size_t destination = 0;
    for (size_t source = 0; source < attributeVector.size(); ++source) {
        if (isEventHandlerAttribute(attributeVector[source])
            || isJavaScriptURLAttribute(attributeVector[source])
            || isHTMLContentAttribute(attributeVector[source])
            || isSVGAnimationAttributeSettingJavaScriptURL(attributeVector[source]))
            continue;

        if (source != destination)
            attributeVector[destination] = attributeVector[source];

        ++destination;
    }
    attributeVector.shrink(destination);
}

void Element::parserSetAttributes(const Vector<Attribute>& attributeVector)
{
    ASSERT(!inDocument());
    ASSERT(!parentNode());
    ASSERT(!m_elementData);

    if (!attributeVector.isEmpty()) {
        if (document().elementDataCache())
            m_elementData = document().elementDataCache()->cachedShareableElementDataWithAttributes(attributeVector);
        else
            m_elementData = ShareableElementData::createWithAttributes(attributeVector);
    }

    parserDidSetAttributes();

    // Use attributeVector instead of m_elementData because attributeChanged might modify m_elementData.
    for (const auto& attribute : attributeVector)
        attributeChangedFromParserOrByCloning(attribute.name(), attribute.value(), ModifiedDirectly);
}

bool Element::hasEquivalentAttributes(const Element* other) const
{
    synchronizeAllAttributes();
    other->synchronizeAllAttributes();
    if (elementData() == other->elementData())
        return true;
    if (elementData())
        return elementData()->isEquivalent(other->elementData());
    if (other->elementData())
        return other->elementData()->isEquivalent(elementData());
    return true;
}

String Element::nodeName() const
{
    return m_tagName.toString();
}

const AtomicString& Element::locateNamespacePrefix(const AtomicString& namespaceToLocate) const
{
    if (!prefix().isNull() && namespaceURI() == namespaceToLocate)
        return prefix();

    AttributeCollection attributes = this->attributes();
    for (const Attribute& attr : attributes) {
        if (attr.prefix() == xmlnsAtom && attr.value() == namespaceToLocate)
            return attr.localName();
    }

    if (Element* parent = parentElement())
        return parent->locateNamespacePrefix(namespaceToLocate);

    return nullAtom;
}

const AtomicString Element::imageSourceURL() const
{
    return getAttribute(srcAttr);
}

bool Element::layoutObjectIsNeeded(const ComputedStyle& style)
{
    return style.display() != NONE;
}

LayoutObject* Element::createLayoutObject(const ComputedStyle& style)
{
    return LayoutObject::createObject(this, style);
}

Node::InsertionNotificationRequest Element::insertedInto(ContainerNode* insertionPoint)
{
    // need to do superclass processing first so inDocument() is true
    // by the time we reach updateId
    ContainerNode::insertedInto(insertionPoint);

    if (containsFullScreenElement() && parentElement() && !parentElement()->containsFullScreenElement())
        setContainsFullScreenElementOnAncestorsCrossingFrameBoundaries(true);

    ASSERT(!hasRareData() || !elementRareData()->hasPseudoElements());

    if (!insertionPoint->isInTreeScope())
        return InsertionDone;

    if (hasRareData())
        elementRareData()->clearClassListValueForQuirksMode();

    if (isUpgradedCustomElement() && inDocument())
        CustomElement::didAttach(this, document());

    TreeScope& scope = insertionPoint->treeScope();
    if (scope != treeScope())
        return InsertionDone;

    const AtomicString& idValue = getIdAttribute();
    if (!idValue.isNull())
        updateId(scope, nullAtom, idValue);

    const AtomicString& nameValue = getNameAttribute();
    if (!nameValue.isNull())
        updateName(nullAtom, nameValue);

    if (parentElement() && parentElement()->isInCanvasSubtree())
        setIsInCanvasSubtree(true);

    return InsertionDone;
}

void Element::removedFrom(ContainerNode* insertionPoint)
{
    bool wasInDocument = insertionPoint->inDocument();

    ASSERT(!hasRareData() || !elementRareData()->hasPseudoElements());

    if (Fullscreen::isActiveFullScreenElement(*this)) {
        setContainsFullScreenElementOnAncestorsCrossingFrameBoundaries(false);
        if (insertionPoint->isElementNode()) {
            toElement(insertionPoint)->setContainsFullScreenElement(false);
            toElement(insertionPoint)->setContainsFullScreenElementOnAncestorsCrossingFrameBoundaries(false);
        }
    }

    if (Fullscreen* fullscreen = Fullscreen::fromIfExists(document()))
        fullscreen->elementRemoved(*this);

    if (document().page())
        document().page()->pointerLockController().elementRemoved(this);

    setSavedLayerScrollOffset(IntSize());

    if (insertionPoint->isInTreeScope() && treeScope() == document()) {
        const AtomicString& idValue = getIdAttribute();
        if (!idValue.isNull())
            updateId(insertionPoint->treeScope(), idValue, nullAtom);

        const AtomicString& nameValue = getNameAttribute();
        if (!nameValue.isNull())
            updateName(nameValue, nullAtom);
    }

    ContainerNode::removedFrom(insertionPoint);
    if (wasInDocument) {
        if (this == document().cssTarget())
            document().setCSSTarget(nullptr);

        if (hasPendingResources())
            document().accessSVGExtensions().removeElementFromPendingResources(this);

#ifdef MINIBLINK_NOT_IMPLEMENTED
        if (isUpgradedCustomElement())
            CustomElement::didDetach(this, insertionPoint->document());
#endif // MINIBLINK_NOT_IMPLEMENTED
    }

    document().removeFromTopLayer(this);

    clearElementFlag(IsInCanvasSubtree);

    if (hasRareData()) {
        ElementRareData* data = elementRareData();

        data->clearRestyleFlags();

        if (ElementAnimations* elementAnimations = data->elementAnimations())
            elementAnimations->cssAnimations().cancel();
    }
}

void Element::attach(const AttachContext& context)
{
    ASSERT(document().inStyleRecalc());

    // We've already been through detach when doing an attach, but we might
    // need to clear any state that's been added since then.
    if (hasRareData() && styleChangeType() == NeedsReattachStyleChange) {
        ElementRareData* data = elementRareData();
        data->clearComputedStyle();
    }

    if (!isActiveInsertionPoint(*this))
        LayoutTreeBuilderForElement(*this, context.resolvedStyle).createLayoutObjectIfNeeded();

    addCallbackSelectors();

    if (hasRareData() && !layoutObject()) {
        if (ElementAnimations* elementAnimations = elementRareData()->elementAnimations()) {
            elementAnimations->cssAnimations().cancel();
            elementAnimations->setAnimationStyleChange(false);
        }
    }

    StyleResolverParentScope parentScope(*this);

    createPseudoElementIfNeeded(BEFORE);

    // When a shadow root exists, it does the work of attaching the children.
    if (ElementShadow* shadow = this->shadow())
        shadow->attach(context);

    ContainerNode::attach(context);

    createPseudoElementIfNeeded(AFTER);
    createPseudoElementIfNeeded(BACKDROP);

    // We create the first-letter element after the :before, :after and
    // children are attached because the first letter text could come
    // from any of them.
    createPseudoElementIfNeeded(FIRST_LETTER);
}

void Element::detach(const AttachContext& context)
{
    HTMLFrameOwnerElement::UpdateSuspendScope suspendWidgetHierarchyUpdates;
    cancelFocusAppearanceUpdate();
    removeCallbackSelectors();
    if (hasRareData()) {
        ElementRareData* data = elementRareData();
        data->clearPseudoElements();

        // attach() will clear the computed style for us when inside recalcStyle.
        if (!document().inStyleRecalc())
            data->clearComputedStyle();

        if (ElementAnimations* elementAnimations = data->elementAnimations()) {
            if (context.performingReattach) {
                // FIXME: We call detach from within style recalc, so compositingState is not up to date.
                // https://code.google.com/p/chromium/issues/detail?id=339847
                DisableCompositingQueryAsserts disabler;

                // FIXME: restart compositor animations rather than pull back to the main thread
                elementAnimations->restartAnimationOnCompositor();
            } else {
                elementAnimations->cssAnimations().cancel();
                elementAnimations->setAnimationStyleChange(false);
            }
            elementAnimations->clearBaseComputedStyle();
        }

        if (ElementShadow* shadow = data->shadow())
            shadow->detach(context);
    }

    ContainerNode::detach(context);

    if (!context.performingReattach && isUserActionElement()) {
        if (hovered())
            document().hoveredNodeDetached(*this);
        if (inActiveChain())
            document().activeChainNodeDetached(*this);
        document().userActionElements().didDetach(*this);
    }

    document().styleEngine().styleInvalidator().clearInvalidation(*this);

    if (svgFilterNeedsLayerUpdate())
        document().unscheduleSVGFilterLayerUpdateHack(*this);

    ASSERT(needsAttach());
}

bool Element::pseudoStyleCacheIsInvalid(const ComputedStyle* currentStyle, ComputedStyle* newStyle)
{
    ASSERT(currentStyle == computedStyle());
    ASSERT(layoutObject());

    if (!currentStyle)
        return false;

    const PseudoStyleCache* pseudoStyleCache = currentStyle->cachedPseudoStyles();
    if (!pseudoStyleCache)
        return false;

    size_t cacheSize = pseudoStyleCache->size();
    for (size_t i = 0; i < cacheSize; ++i) {
        RefPtr<ComputedStyle> newPseudoStyle;
        RefPtr<ComputedStyle> oldPseudoStyle = pseudoStyleCache->at(i);
        PseudoId pseudoId = oldPseudoStyle->styleType();
        if (pseudoId == FIRST_LINE || pseudoId == FIRST_LINE_INHERITED)
            newPseudoStyle = layoutObject()->uncachedFirstLineStyle(newStyle);
        else
            newPseudoStyle = layoutObject()->getUncachedPseudoStyle(PseudoStyleRequest(pseudoId), newStyle, newStyle);
        if (!newPseudoStyle)
            return true;
        if (*oldPseudoStyle != *newPseudoStyle || oldPseudoStyle->font().loadingCustomFonts() != newPseudoStyle->font().loadingCustomFonts()) {
            if (pseudoId < FIRST_INTERNAL_PSEUDOID)
                newStyle->setHasPseudoStyle(pseudoId);
            newStyle->addCachedPseudoStyle(newPseudoStyle);
            if (pseudoId == FIRST_LINE || pseudoId == FIRST_LINE_INHERITED)
                layoutObject()->firstLineStyleDidChange(*oldPseudoStyle, *newPseudoStyle);
            return true;
        }
    }
    return false;
}

PassRefPtr<ComputedStyle> Element::styleForLayoutObject()
{
    ASSERT(document().inStyleRecalc());

    RefPtr<ComputedStyle> style;

    // FIXME: Instead of clearing updates that may have been added from calls to styleForElement
    // outside recalcStyle, we should just never set them if we're not inside recalcStyle.
    if (ElementAnimations* elementAnimations = this->elementAnimations())
        elementAnimations->cssAnimations().setPendingUpdate(nullptr);

    if (hasCustomStyleCallbacks())
        style = customStyleForLayoutObject();
    if (!style)
        style = originalStyleForLayoutObject();
    ASSERT(style);

    // styleForElement() might add active animations so we need to get it again.
    if (ElementAnimations* elementAnimations = this->elementAnimations()) {
        elementAnimations->cssAnimations().maybeApplyPendingUpdate(this);
        elementAnimations->updateAnimationFlags(*style);
    }

    if (style->hasTransform()) {
        if (const StylePropertySet* inlineStyle = this->inlineStyle())
            style->setHasInlineTransform(inlineStyle->hasProperty(CSSPropertyTransform));
    }

    if (hasRareData() && elementRareData()->proxyCount() > 0)
        style->setHasCompositorProxy(true);

    document().didRecalculateStyleForElement();
    return style.release();
}

PassRefPtr<ComputedStyle> Element::originalStyleForLayoutObject()
{
    ASSERT(document().inStyleRecalc());
    return document().ensureStyleResolver().styleForElement(this);
}

void Element::recalcStyle(StyleRecalcChange change, Text* nextTextSibling)
{
    ASSERT(document().inStyleRecalc());
    ASSERT(!document().lifecycle().inDetach());
    ASSERT(!parentOrShadowHostNode()->needsStyleRecalc());
    ASSERT(inActiveDocument());

    String outstr;

    if (hasCustomStyleCallbacks())
        willRecalcStyle(change);

    if (change >= Inherit || needsStyleRecalc()) {
        if (hasRareData()) {
            ElementRareData* data = elementRareData();
            data->clearComputedStyle();

            if (change >= Inherit) {
                if (ElementAnimations* elementAnimations = data->elementAnimations())
                    elementAnimations->setAnimationStyleChange(false);
            }
        }
        if (parentComputedStyle())
            change = recalcOwnStyle(change);
        clearNeedsStyleRecalc();
    }

    // If we reattached we don't need to recalc the style of our descendants anymore.
    if ((change >= UpdatePseudoElements && change < Reattach) || childNeedsStyleRecalc()) {
        StyleResolverParentScope parentScope(*this);

        updatePseudoElement(BEFORE, change);

        if (change > UpdatePseudoElements || childNeedsStyleRecalc()) {
            for (ShadowRoot* root = youngestShadowRoot(); root; root = root->olderShadowRoot()) {
                if (root->shouldCallRecalcStyle(change))
                    root->recalcStyle(change);
            }
            
            recalcChildStyle(change);
        }

        updatePseudoElement(AFTER, change);
        updatePseudoElement(BACKDROP, change);

        // If our children have changed then we need to force the first-letter
        // checks as we don't know if they effected the first letter or not.
        // This can be seen when a child transitions from floating to
        // non-floating we have to take it into account for the first letter.
        updatePseudoElement(FIRST_LETTER, childNeedsStyleRecalc() ? Force : change);

        clearChildNeedsStyleRecalc();
    }

    if (hasCustomStyleCallbacks())
        didRecalcStyle(change);

    if (change == Reattach)
        reattachWhitespaceSiblingsIfNeeded(nextTextSibling);
}

StyleRecalcChange Element::recalcOwnStyle(StyleRecalcChange change)
{
    ASSERT(document().inStyleRecalc());
    ASSERT(!parentOrShadowHostNode()->needsStyleRecalc());
    ASSERT(change >= Inherit || needsStyleRecalc());
    ASSERT(parentComputedStyle());

    RefPtr<ComputedStyle> oldStyle = mutableComputedStyle();
    RefPtr<ComputedStyle> newStyle = styleForLayoutObject();
    StyleRecalcChange localChange = ComputedStyle::stylePropagationDiff(oldStyle.get(), newStyle.get());

    ASSERT(newStyle);

    if (localChange == Reattach) {
        AttachContext reattachContext;
        reattachContext.resolvedStyle = newStyle.get();
        bool layoutObjectWillChange = needsAttach() || layoutObject();
        reattach(reattachContext);
        if (layoutObjectWillChange || layoutObject())
            return Reattach;
        return ReattachNoLayoutObject;
    }

    ASSERT(oldStyle);

    if (localChange != NoChange)
        updateCallbackSelectors(oldStyle.get(), newStyle.get());

    if (LayoutObject* layoutObject = this->layoutObject()) {
        if (localChange != NoChange || pseudoStyleCacheIsInvalid(oldStyle.get(), newStyle.get()) || svgFilterNeedsLayerUpdate()) {
            layoutObject->setStyle(newStyle.get());
        } else {
            // Although no change occurred, we use the new style so that the cousin style sharing code won't get
            // fooled into believing this style is the same.
            // FIXME: We may be able to remove this hack, see discussion in
            // https://codereview.chromium.org/30453002/
            layoutObject->setStyleInternal(newStyle.get());
        }
    }

    if (styleChangeType() >= SubtreeStyleChange)
        return Force;

    if (change > Inherit || localChange > Inherit)
        return max(localChange, change);

    if (localChange < Inherit) {
        if (oldStyle->hasChildDependentFlags()) {
            if (childNeedsStyleRecalc())
                return Inherit;
            newStyle->copyChildDependentFlagsFrom(*oldStyle);
        }
        if (oldStyle->hasPseudoElementStyle() || newStyle->hasPseudoElementStyle())
            return UpdatePseudoElements;
    }

    return localChange;
}

void Element::updateCallbackSelectors(const ComputedStyle* oldStyle, const ComputedStyle* newStyle)
{
    Vector<String> emptyVector;
    const Vector<String>& oldCallbackSelectors = oldStyle ? oldStyle->callbackSelectors() : emptyVector;
    const Vector<String>& newCallbackSelectors = newStyle ? newStyle->callbackSelectors() : emptyVector;
    if (oldCallbackSelectors.isEmpty() && newCallbackSelectors.isEmpty())
        return;
    if (oldCallbackSelectors != newCallbackSelectors)
        CSSSelectorWatch::from(document()).updateSelectorMatches(oldCallbackSelectors, newCallbackSelectors);
}

void Element::addCallbackSelectors()
{
    updateCallbackSelectors(0, computedStyle());
}

void Element::removeCallbackSelectors()
{
    updateCallbackSelectors(computedStyle(), 0);
}

ElementShadow* Element::shadow() const
{
    return hasRareData() ? elementRareData()->shadow() : nullptr;
}

ElementShadow& Element::ensureShadow()
{
    return ensureElementRareData().ensureShadow();
}

void Element::pseudoStateChanged(CSSSelector::PseudoType pseudo)
{
    // We can't schedule invaliation sets from inside style recalc otherwise
    // we'd never process them.
    // TODO(esprehn): Make this an ASSERT and fix places that call into this
    // like HTMLSelectElement.
    if (document().inStyleRecalc())
        return;
    StyleResolver* styleResolver = document().styleResolver();
    if (inActiveDocument() && styleResolver && styleChangeType() < SubtreeStyleChange)
        document().styleEngine().pseudoStateChangedForElement(pseudo, *this);
}

void Element::setAnimationStyleChange(bool animationStyleChange)
{
    if (animationStyleChange && document().inStyleRecalc())
        return;
    if (!hasRareData())
        return;
    if (ElementAnimations* elementAnimations = elementRareData()->elementAnimations())
        elementAnimations->setAnimationStyleChange(animationStyleChange);
}

void Element::clearAnimationStyleChange()
{
    if (!hasRareData())
        return;
    if (ElementAnimations* elementAnimations = elementRareData()->elementAnimations())
        elementAnimations->setAnimationStyleChange(false);
}

void Element::setNeedsAnimationStyleRecalc()
{
    if (styleChangeType() != NoStyleChange)
        return;

    setNeedsStyleRecalc(LocalStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::Animation));
    setAnimationStyleChange(true);
}

void Element::setNeedsCompositingUpdate()
{
    if (!document().isActive())
        return;
    LayoutBoxModelObject* layoutObject = layoutBoxModelObject();
    if (!layoutObject)
        return;
    if (!layoutObject->hasLayer())
        return;
    layoutObject->layer()->setNeedsCompositingInputsUpdate();
}

void Element::setCustomElementDefinition(PassRefPtrWillBeRawPtr<CustomElementDefinition> definition)
{
    if (!hasRareData() && !definition)
        return;
    ASSERT(!customElementDefinition());
    ensureElementRareData().setCustomElementDefinition(definition);
}

CustomElementDefinition* Element::customElementDefinition() const
{
    if (hasRareData())
        return elementRareData()->customElementDefinition();
    return nullptr;
}

PassRefPtrWillBeRawPtr<ShadowRoot> Element::createShadowRoot(const ScriptState* scriptState, ExceptionState& exceptionState)
{
    OriginsUsingFeatures::count(scriptState, document(), OriginsUsingFeatures::Feature::ElementCreateShadowRoot);
    return createShadowRoot(exceptionState);
}

PassRefPtrWillBeRawPtr<ShadowRoot> Element::createShadowRoot(const ScriptState* scriptState, const ShadowRootInit& shadowRootInitDict, ExceptionState& exceptionState)
{
    ASSERT(RuntimeEnabledFeatures::createShadowRootWithParameterEnabled());
    UseCounter::count(document(), UseCounter::ElementCreateShadowRootWithParameter);

    OriginsUsingFeatures::count(scriptState, document(), OriginsUsingFeatures::Feature::ElementCreateShadowRoot);

    if (shadowRootInitDict.hasMode()) {
        if (shadowRoot()) {
            exceptionState.throwDOMException(InvalidStateError, "Shadow root cannot be created on a host which already hosts a shadow tree.");
            return nullptr;
        }
        // TODO(kochi): Add support for closed shadow root. crbug.com/459136
        if (shadowRootInitDict.mode() == "closed") {
            exceptionState.throwDOMException(NotSupportedError, "Closed shadow root is not implemented yet.");
            return nullptr;
        }
    }

    RefPtrWillBeRawPtr<ShadowRoot> shadowRoot = createShadowRoot(exceptionState);

    if (shadowRootInitDict.hasDelegatesFocus())
        shadowRoot->setDelegatesFocus(shadowRootInitDict.delegatesFocus());

    return shadowRoot.release();
}

PassRefPtrWillBeRawPtr<ShadowRoot> Element::createShadowRoot(ExceptionState& exceptionState)
{
    if (alwaysCreateUserAgentShadowRoot())
        ensureUserAgentShadowRoot();

    // Some elements make assumptions about what kind of layoutObjects they allow
    // as children so we can't allow author shadows on them for now. An override
    // flag is provided for testing how author shadows interact on these elements.
    if (!areAuthorShadowsAllowed() && !RuntimeEnabledFeatures::authorShadowDOMForAnyElementEnabled()) {
        exceptionState.throwDOMException(HierarchyRequestError, "Author-created shadow roots are disabled for this element.");
        return nullptr;
    }

    return PassRefPtrWillBeRawPtr<ShadowRoot>(ensureShadow().addShadowRoot(*this, ShadowRootType::Open));
}

ShadowRoot* Element::shadowRoot() const
{
    ElementShadow* elementShadow = shadow();
    if (!elementShadow)
        return nullptr;
    ShadowRoot* shadowRoot = elementShadow->youngestShadowRoot();
    if (shadowRoot->type() == ShadowRootType::Open)
        return shadowRoot;
    return nullptr;
}

ShadowRoot* Element::userAgentShadowRoot() const
{
    if (ElementShadow* elementShadow = shadow()) {
        if (ShadowRoot* shadowRoot = elementShadow->oldestShadowRoot()) {
            ASSERT(shadowRoot->type() == ShadowRootType::UserAgent);
            return shadowRoot;
        }
    }

    return nullptr;
}

ShadowRoot& Element::ensureUserAgentShadowRoot()
{
    if (ShadowRoot* shadowRoot = userAgentShadowRoot())
        return *shadowRoot;
    ShadowRoot& shadowRoot = ensureShadow().addShadowRoot(*this, ShadowRootType::UserAgent);
    didAddUserAgentShadowRoot(shadowRoot);
    return shadowRoot;
}

bool Element::childTypeAllowed(NodeType type) const
{
    switch (type) {
    case ELEMENT_NODE:
    case TEXT_NODE:
    case COMMENT_NODE:
    case PROCESSING_INSTRUCTION_NODE:
    case CDATA_SECTION_NODE:
        return true;
    default:
        break;
    }
    return false;
}

void Element::checkForEmptyStyleChange()
{
    const ComputedStyle* style = computedStyle();

    if (!style && !styleAffectedByEmpty())
        return;
    if (styleChangeType() >= SubtreeStyleChange)
        return;
    if (!inActiveDocument())
        return;
    if (!document().styleResolver())
        return;

    if (!style || (styleAffectedByEmpty() && (!style->emptyState() || hasChildren())))
        document().styleEngine().pseudoStateChangedForElement(CSSSelector::PseudoEmpty, *this);
}

void Element::childrenChanged(const ChildrenChange& change)
{
    ContainerNode::childrenChanged(change);

    checkForEmptyStyleChange();
    if (!change.byParser && change.isChildElementChange())
        checkForSiblingStyleChanges(change.type == ElementRemoved ? SiblingElementRemoved : SiblingElementInserted, change.siblingBeforeChange, change.siblingAfterChange);

    if (ElementShadow* shadow = this->shadow())
        shadow->setNeedsDistributionRecalc();
}

void Element::finishParsingChildren()
{
    setIsFinishedParsingChildren(true);
    checkForEmptyStyleChange();
    checkForSiblingStyleChanges(FinishedParsingChildren, lastChild(), nullptr);
}

#ifndef NDEBUG
void Element::formatForDebugger(char* buffer, unsigned length) const
{
    StringBuilder result;
    String s;

    result.append(nodeName());

    s = getIdAttribute();
    if (s.length() > 0) {
        if (result.length() > 0)
            result.appendLiteral("; ");
        result.appendLiteral("id=");
        result.append(s);
    }

    s = getAttribute(classAttr);
    if (s.length() > 0) {
        if (result.length() > 0)
            result.appendLiteral("; ");
        result.appendLiteral("class=");
        result.append(s);
    }

    strncpy(buffer, result.toString().utf8().data(), length - 1);
}
#endif

AttrNodeList* Element::attrNodeList()
{
    return hasRareData() ? elementRareData()->attrNodeList() : nullptr;
}

AttrNodeList& Element::ensureAttrNodeList()
{
    return ensureElementRareData().ensureAttrNodeList();
}

void Element::removeAttrNodeList()
{
    ASSERT(attrNodeList());
    if (hasRareData())
        elementRareData()->removeAttrNodeList();
}

PassRefPtrWillBeRawPtr<Attr> Element::setAttributeNode(Attr* attrNode, ExceptionState& exceptionState)
{
    RefPtrWillBeRawPtr<Attr> oldAttrNode = attrIfExists(attrNode->qualifiedName());
    if (oldAttrNode.get() == attrNode)
        return attrNode; // This Attr is already attached to the element.

    // InUseAttributeError: Raised if node is an Attr that is already an attribute of another Element object.
    // The DOM user must explicitly clone Attr nodes to re-use them in other elements.
    if (attrNode->ownerElement()) {
        exceptionState.throwDOMException(InUseAttributeError, "The node provided is an attribute node that is already an attribute of another Element; attribute nodes must be explicitly cloned.");
        return nullptr;
    }

    if (!isHTMLElement() && attrNode->document().isHTMLDocument() && attrNode->name() != attrNode->name().lower())
        UseCounter::count(document(), UseCounter::NonHTMLElementSetAttributeNodeFromHTMLDocumentNameNotLowercase);

    synchronizeAllAttributes();
    const UniqueElementData& elementData = ensureUniqueElementData();

    AttributeCollection attributes = elementData.attributes();
    size_t index = attributes.findIndex(attrNode->qualifiedName(), shouldIgnoreAttributeCase());
    AtomicString localName;
    if (index != kNotFound) {
        const Attribute& attr = attributes[index];

        // If the name of the ElementData attribute doesn't
        // (case-sensitively) match that of the Attr node, record it
        // on the Attr so that it can correctly resolve the value on
        // the Element.
        if (!attr.name().matches(attrNode->qualifiedName()))
            localName = attr.localName();

        if (oldAttrNode) {
            detachAttrNodeFromElementWithValue(oldAttrNode.get(), attr.value());
        } else {
            // FIXME: using attrNode's name rather than the
            // Attribute's for the replaced Attr is compatible with
            // all but Gecko (and, arguably, the DOM Level1 spec text.)
            // Consider switching.
            oldAttrNode = Attr::create(document(), attrNode->qualifiedName(), attr.value());
        }
    }

    setAttributeInternal(index, attrNode->qualifiedName(), attrNode->value(), NotInSynchronizationOfLazyAttribute);

    attrNode->attachToElement(this, localName);
    treeScope().adoptIfNeeded(*attrNode);
    ensureAttrNodeList().append(attrNode);

    return oldAttrNode.release();
}

PassRefPtrWillBeRawPtr<Attr> Element::setAttributeNodeNS(Attr* attr, ExceptionState& exceptionState)
{
    return setAttributeNode(attr, exceptionState);
}

PassRefPtrWillBeRawPtr<Attr> Element::removeAttributeNode(Attr* attr, ExceptionState& exceptionState)
{
    if (attr->ownerElement() != this) {
        exceptionState.throwDOMException(NotFoundError, "The node provided is owned by another element.");
        return nullptr;
    }

    ASSERT(document() == attr->document());

    synchronizeAttribute(attr->qualifiedName());

    size_t index = elementData()->attributes().findIndex(attr);
    if (index == kNotFound) {
        exceptionState.throwDOMException(NotFoundError, "The attribute was not found on this element.");
        return nullptr;
    }

    RefPtrWillBeRawPtr<Attr> guard(attr);
    detachAttrNodeAtIndex(attr, index);
    return guard.release();
}

void Element::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == tabindexAttr) {
        int tabindex = 0;
        if (value.isEmpty()) {
            clearTabIndexExplicitlyIfNeeded();
            if (treeScope().adjustedFocusedElement() == this) {
                // We might want to call blur(), but it's dangerous to dispatch
                // events here.
                document().setNeedsFocusedElementCheck();
            }
        } else if (parseHTMLInteger(value, tabindex)) {
            // Clamp tabindex to the range of 'short' to match Firefox's behavior.
            setTabIndexExplicitly(max(static_cast<int>(std::numeric_limits<short>::min()), std::min(tabindex, static_cast<int>(std::numeric_limits<short>::max()))));
        }
    }
}

bool Element::parseAttributeName(QualifiedName& out, const AtomicString& namespaceURI, const AtomicString& qualifiedName, ExceptionState& exceptionState)
{
    AtomicString prefix, localName;
    if (!Document::parseQualifiedName(qualifiedName, prefix, localName, exceptionState))
        return false;
    ASSERT(!exceptionState.hadException());

    QualifiedName qName(prefix, localName, namespaceURI);

    if (!Document::hasValidNamespaceForAttributes(qName)) {
        exceptionState.throwDOMException(NamespaceError, "'" + namespaceURI + "' is an invalid namespace for attributes.");
        return false;
    }

    out = qName;
    return true;
}

void Element::setAttributeNS(const AtomicString& namespaceURI, const AtomicString& qualifiedName, const AtomicString& value, ExceptionState& exceptionState)
{
    QualifiedName parsedName = anyName;
    if (!parseAttributeName(parsedName, namespaceURI, qualifiedName, exceptionState))
        return;
    setAttribute(parsedName, value);
}

void Element::removeAttributeInternal(size_t index, SynchronizationOfLazyAttribute inSynchronizationOfLazyAttribute)
{
    MutableAttributeCollection attributes = ensureUniqueElementData().attributes();
    ASSERT_WITH_SECURITY_IMPLICATION(index < attributes.size());

    QualifiedName name = attributes[index].name();
    AtomicString valueBeingRemoved = attributes[index].value();

    if (!inSynchronizationOfLazyAttribute) {
        if (!valueBeingRemoved.isNull())
            willModifyAttribute(name, valueBeingRemoved, nullAtom);
    }

    if (RefPtrWillBeRawPtr<Attr> attrNode = attrIfExists(name))
        detachAttrNodeFromElementWithValue(attrNode.get(), attributes[index].value());

    attributes.remove(index);

    if (!inSynchronizationOfLazyAttribute)
        didRemoveAttribute(name, valueBeingRemoved);
}

void Element::appendAttributeInternal(const QualifiedName& name, const AtomicString& value, SynchronizationOfLazyAttribute inSynchronizationOfLazyAttribute)
{
    if (!inSynchronizationOfLazyAttribute)
        willModifyAttribute(name, nullAtom, value);
    ensureUniqueElementData().attributes().append(name, value);
    if (!inSynchronizationOfLazyAttribute)
        didAddAttribute(name, value);
}

void Element::removeAttribute(const AtomicString& name)
{
    if (!elementData())
        return;

    AtomicString localName = shouldIgnoreAttributeCase() ? name.lower() : name;
    size_t index = elementData()->attributes().findIndex(localName, false);
    if (index == kNotFound) {
        if (UNLIKELY(localName == styleAttr) && elementData()->m_styleAttributeIsDirty && isStyledElement())
            removeAllInlineStyleProperties();
        return;
    }

    removeAttributeInternal(index, NotInSynchronizationOfLazyAttribute);
}

void Element::removeAttributeNS(const AtomicString& namespaceURI, const AtomicString& localName)
{
    removeAttribute(QualifiedName(nullAtom, localName, namespaceURI));
}

PassRefPtrWillBeRawPtr<Attr> Element::getAttributeNode(const AtomicString& localName)
{
    if (!elementData())
        return nullptr;
    synchronizeAttribute(localName);
    const Attribute* attribute = elementData()->attributes().find(localName, shouldIgnoreAttributeCase());
    if (!attribute)
        return nullptr;
    return ensureAttr(attribute->name());
}

PassRefPtrWillBeRawPtr<Attr> Element::getAttributeNodeNS(const AtomicString& namespaceURI, const AtomicString& localName)
{
    if (!elementData())
        return nullptr;
    QualifiedName qName(nullAtom, localName, namespaceURI);
    synchronizeAttribute(qName);
    const Attribute* attribute = elementData()->attributes().find(qName);
    if (!attribute)
        return nullptr;
    return ensureAttr(attribute->name());
}

bool Element::hasAttribute(const AtomicString& localName) const
{
    if (!elementData())
        return false;
    synchronizeAttribute(localName);
    return elementData()->attributes().findIndex(shouldIgnoreAttributeCase() ? localName.lower() : localName, false) != kNotFound;
}

bool Element::hasAttributeNS(const AtomicString& namespaceURI, const AtomicString& localName) const
{
    if (!elementData())
        return false;
    QualifiedName qName(nullAtom, localName, namespaceURI);
    synchronizeAttribute(qName);
    return elementData()->attributes().find(qName);
}

void Element::focus(bool restorePreviousSelection, WebFocusType type)
{
    if (!inDocument())
        return;

    if (document().focusedElement() == this)
        return;

    if (!document().isActive())
        return;

    document().updateLayoutIgnorePendingStylesheets();
    if (!isFocusable())
        return;

    if (shadowRoot() && shadowRoot()->delegatesFocus()) {
        if (containsIncludingShadowDOM(document().focusedElement()))
            return;

        // Slide the focus to its inner node.
        Element* next = document().page()->focusController().findFocusableElement(WebFocusTypeForward, *this);
        if (next && containsIncludingShadowDOM(next)) {
            next->focus(false, WebFocusTypeForward);
            return;
        }
    }

    RefPtrWillBeRawPtr<Node> protect(this);
    if (!document().page()->focusController().setFocusedElement(this, document().frame(), type))
        return;

    // Setting the focused node above might have invalidated the layout due to scripts.
    document().updateLayoutIgnorePendingStylesheets();
    if (!isFocusable())
        return;

    cancelFocusAppearanceUpdate();
    updateFocusAppearance(restorePreviousSelection);

    if (UserGestureIndicator::processedUserGestureSinceLoad()) {
        // Bring up the keyboard in the context of anything triggered by a user
        // gesture. Since tracking that across arbitrary boundaries (eg.
        // animations) is difficult, for now we match IE's heuristic and bring
        // up the keyboard if there's been any gesture since load.
        document().page()->chromeClient().showImeIfNeeded();
    }
}

void Element::updateFocusAppearance(bool /*restorePreviousSelection*/)
{
    if (isRootEditableElement()) {
        // Taking the ownership since setSelection() may release the last reference to |frame|.
        RefPtrWillBeRawPtr<LocalFrame> frame(document().frame());
        if (!frame)
            return;

        // When focusing an editable element in an iframe, don't reset the selection if it already contains a selection.
        if (this == frame->selection().rootEditableElement())
            return;

        // FIXME: We should restore the previous selection if there is one.
        VisibleSelection newSelection = VisibleSelection(firstPositionInOrBeforeNode(this), DOWNSTREAM);
        // Passing DoNotSetFocus as this function is called after FocusController::setFocusedElement()
        // and we don't want to change the focus to a new Element.
        frame->selection().setSelection(newSelection,  FrameSelection::CloseTyping | FrameSelection::ClearTypingStyle | FrameSelection::DoNotSetFocus);
        frame->selection().revealSelection();
    } else if (layoutObject() && !layoutObject()->isLayoutPart()) {
        layoutObject()->scrollRectToVisible(boundingBox());
    }
}

void Element::blur()
{
    cancelFocusAppearanceUpdate();
    if (treeScope().adjustedFocusedElement() == this) {
        Document& doc = document();
        if (doc.page())
            doc.page()->focusController().setFocusedElement(0, doc.frame());
        else
            doc.setFocusedElement(nullptr);
    }
}

bool Element::supportsFocus() const
{
    // FIXME: supportsFocus() can be called when layout is not up to date.
    // Logic that deals with the layoutObject should be moved to layoutObjectIsFocusable().
    // But supportsFocus must return true when the element is editable, or else
    // it won't be focusable. Furthermore, supportsFocus cannot just return true
    // always or else tabIndex() will change for all HTML elements.
    return hasElementFlag(TabIndexWasSetExplicitly) || (hasEditableStyle() && parentNode() && !parentNode()->hasEditableStyle())
        || (isShadowHost(this) && shadowRoot() && shadowRoot()->delegatesFocus())
        || supportsSpatialNavigationFocus();
}

bool Element::supportsSpatialNavigationFocus() const
{
    // This function checks whether the element satisfies the extended criteria
    // for the element to be focusable, introduced by spatial navigation feature,
    // i.e. checks if click or keyboard event handler is specified.
    // This is the way to make it possible to navigate to (focus) elements
    // which web designer meant for being active (made them respond to click events).
    if (!isSpatialNavigationEnabled(document().frame()) || spatialNavigationIgnoresEventHandlers(document().frame()))
        return false;
    if (hasEventListeners(EventTypeNames::click)
        || hasEventListeners(EventTypeNames::keydown)
        || hasEventListeners(EventTypeNames::keypress)
        || hasEventListeners(EventTypeNames::keyup))
        return true;
    if (!isSVGElement())
        return false;
    return (hasEventListeners(EventTypeNames::focus)
        || hasEventListeners(EventTypeNames::blur)
        || hasEventListeners(EventTypeNames::focusin)
        || hasEventListeners(EventTypeNames::focusout));
}

bool Element::isFocusable() const
{
    return inDocument() && supportsFocus() && !isInert() && layoutObjectIsFocusable();
}

bool Element::isKeyboardFocusable() const
{
    return isFocusable() && tabIndex() >= 0;
}

bool Element::isMouseFocusable() const
{
    return isFocusable();
}

bool Element::isFocusedElementInDocument() const
{
    return this == document().focusedElement();
}

void Element::dispatchFocusEvent(Element* oldFocusedElement, WebFocusType type)
{
    RefPtrWillBeRawPtr<FocusEvent> event = FocusEvent::create(EventTypeNames::focus, false, false, document().domWindow(), 0, oldFocusedElement);
    EventDispatcher::dispatchEvent(*this, FocusEventDispatchMediator::create(event.release()));
}

void Element::dispatchBlurEvent(Element* newFocusedElement, WebFocusType type)
{
    RefPtrWillBeRawPtr<FocusEvent> event = FocusEvent::create(EventTypeNames::blur, false, false, document().domWindow(), 0, newFocusedElement);
    EventDispatcher::dispatchEvent(*this, BlurEventDispatchMediator::create(event.release()));
}

void Element::dispatchFocusInEvent(const AtomicString& eventType, Element* oldFocusedElement, WebFocusType)
{
    ASSERT(!EventDispatchForbiddenScope::isEventDispatchForbidden());
    ASSERT(eventType == EventTypeNames::focusin || eventType == EventTypeNames::DOMFocusIn);
    dispatchScopedEventDispatchMediator(FocusInEventDispatchMediator::create(FocusEvent::create(eventType, true, false, document().domWindow(), 0, oldFocusedElement)));
}

void Element::dispatchFocusOutEvent(const AtomicString& eventType, Element* newFocusedElement)
{
    ASSERT(!EventDispatchForbiddenScope::isEventDispatchForbidden());
    ASSERT(eventType == EventTypeNames::focusout || eventType == EventTypeNames::DOMFocusOut);
    dispatchScopedEventDispatchMediator(FocusOutEventDispatchMediator::create(FocusEvent::create(eventType, true, false, document().domWindow(), 0, newFocusedElement)));
}

String Element::innerHTML() const
{
    return createMarkup(this, ChildrenOnly);
}

String Element::outerHTML() const
{
    return createMarkup(this);
}

void Element::setInnerHTML(const String& html, ExceptionState& exceptionState)
{
    InspectorInstrumentation::willSetInnerHTML(this);

    if (RefPtrWillBeRawPtr<DocumentFragment> fragment = createFragmentForInnerOuterHTML(html, this, AllowScriptingContent, "innerHTML", exceptionState)) {
        ContainerNode* container = this;
        if (isHTMLTemplateElement(*this))
            container = toHTMLTemplateElement(this)->content();
        replaceChildrenWithFragment(container, fragment.release(), exceptionState);
    }
}

void Element::setOuterHTML(const String& html, ExceptionState& exceptionState)
{
    Node* p = parentNode();
    if (!p) {
        exceptionState.throwDOMException(NoModificationAllowedError, "This element has no parent node.");
        return;
    }
    if (!p->isElementNode()) {
        exceptionState.throwDOMException(NoModificationAllowedError, "This element's parent is of type '" + p->nodeName() + "', which is not an element node.");
        return;
    }

    RefPtrWillBeRawPtr<Element> parent = toElement(p);
    RefPtrWillBeRawPtr<Node> prev = previousSibling();
    RefPtrWillBeRawPtr<Node> next = nextSibling();

    RefPtrWillBeRawPtr<DocumentFragment> fragment = createFragmentForInnerOuterHTML(html, parent.get(), AllowScriptingContent, "outerHTML", exceptionState);
    if (exceptionState.hadException())
        return;

    parent->replaceChild(fragment.release(), this, exceptionState);
    RefPtrWillBeRawPtr<Node> node = next ? next->previousSibling() : nullptr;
    if (!exceptionState.hadException() && node && node->isTextNode())
        mergeWithNextTextNode(toText(node.get()), exceptionState);

    if (!exceptionState.hadException() && prev && prev->isTextNode())
        mergeWithNextTextNode(toText(prev.get()), exceptionState);
}

Node* Element::insertAdjacent(const String& where, Node* newChild, ExceptionState& exceptionState)
{
    if (equalIgnoringCase(where, "beforeBegin")) {
        if (ContainerNode* parent = this->parentNode()) {
            parent->insertBefore(newChild, this, exceptionState);
            if (!exceptionState.hadException())
                return newChild;
        }
        return nullptr;
    }

    if (equalIgnoringCase(where, "afterBegin")) {
        insertBefore(newChild, firstChild(), exceptionState);
        return exceptionState.hadException() ? nullptr : newChild;
    }

    if (equalIgnoringCase(where, "beforeEnd")) {
        appendChild(newChild, exceptionState);
        return exceptionState.hadException() ? nullptr : newChild;
    }

    if (equalIgnoringCase(where, "afterEnd")) {
        if (ContainerNode* parent = this->parentNode()) {
            parent->insertBefore(newChild, nextSibling(), exceptionState);
            if (!exceptionState.hadException())
                return newChild;
        }
        return nullptr;
    }

    exceptionState.throwDOMException(SyntaxError, "The value provided ('" + where + "') is not one of 'beforeBegin', 'afterBegin', 'beforeEnd', or 'afterEnd'.");
    return nullptr;
}

// Step 1 of http://domparsing.spec.whatwg.org/#insertadjacenthtml()
static Element* contextElementForInsertion(const String& where, Element* element, ExceptionState& exceptionState)
{
    if (equalIgnoringCase(where, "beforeBegin") || equalIgnoringCase(where, "afterEnd")) {
        Element* parent = element->parentElement();
        if (!parent) {
            exceptionState.throwDOMException(NoModificationAllowedError, "The element has no parent.");
            return nullptr;
        }
        return parent;
    }
    if (equalIgnoringCase(where, "afterBegin") || equalIgnoringCase(where, "beforeEnd"))
        return element;
    exceptionState.throwDOMException(SyntaxError, "The value provided ('" + where + "') is not one of 'beforeBegin', 'afterBegin', 'beforeEnd', or 'afterEnd'.");
    return nullptr;
}

Element* Element::insertAdjacentElement(const String& where, Element* newChild, ExceptionState& exceptionState)
{
    Node* returnValue = insertAdjacent(where, newChild, exceptionState);
    return toElement(returnValue);
}

void Element::insertAdjacentText(const String& where, const String& text, ExceptionState& exceptionState)
{
    insertAdjacent(where, document().createTextNode(text).get(), exceptionState);
}

void Element::insertAdjacentHTML(const String& where, const String& markup, ExceptionState& exceptionState)
{
    RefPtrWillBeRawPtr<Element> contextElement = contextElementForInsertion(where, this, exceptionState);
    if (!contextElement)
        return;

    RefPtrWillBeRawPtr<DocumentFragment> fragment = createFragmentForInnerOuterHTML(markup, contextElement.get(), AllowScriptingContent, "insertAdjacentHTML", exceptionState);
    if (!fragment)
        return;
    insertAdjacent(where, fragment.get(), exceptionState);
}

String Element::innerText()
{
    // We need to update layout, since plainText uses line boxes in the layout tree.
    document().updateLayoutIgnorePendingStylesheets();

    if (!layoutObject())
        return textContent(true);

    return plainText(Position(this, PositionAnchorType::BeforeChildren), Position(this, PositionAnchorType::AfterChildren), TextIteratorForInnerText);
}

String Element::outerText()
{
    // Getting outerText is the same as getting innerText, only
    // setting is different. You would think this should get the plain
    // text for the outer range, but this is wrong, <br> for instance
    // would return different values for inner and outer text by such
    // a rule, but it doesn't in WinIE, and we want to match that.
    return innerText();
}

String Element::textFromChildren()
{
    Text* firstTextNode = 0;
    bool foundMultipleTextNodes = false;
    unsigned totalLength = 0;

    for (Node* child = firstChild(); child; child = child->nextSibling()) {
        if (!child->isTextNode())
            continue;
        Text* text = toText(child);
        if (!firstTextNode)
            firstTextNode = text;
        else
            foundMultipleTextNodes = true;
        unsigned length = text->data().length();
        if (length > std::numeric_limits<unsigned>::max() - totalLength)
            return emptyString();
        totalLength += length;
    }

    if (!firstTextNode)
        return emptyString();

    if (firstTextNode && !foundMultipleTextNodes) {
        firstTextNode->atomize();
        return firstTextNode->data();
    }

    StringBuilder content;
    content.reserveCapacity(totalLength);
    for (Node* child = firstTextNode; child; child = child->nextSibling()) {
        if (!child->isTextNode())
            continue;
        content.append(toText(child)->data());
    }

    ASSERT(content.length() == totalLength);
    return content.toString();
}

const AtomicString& Element::shadowPseudoId() const
{
    if (ShadowRoot* root = containingShadowRoot()) {
        if (root->type() == ShadowRootType::UserAgent)
            return fastGetAttribute(pseudoAttr);
    }
    return nullAtom;
}

void Element::setShadowPseudoId(const AtomicString& id)
{
    ASSERT(CSSSelector::parsePseudoType(id, false) == CSSSelector::PseudoWebKitCustomElement);
    setAttribute(pseudoAttr, id);
}

bool Element::isInDescendantTreeOf(const Element* shadowHost) const
{
    ASSERT(shadowHost);
    ASSERT(isShadowHost(shadowHost));

    const ShadowRoot* shadowRoot = containingShadowRoot();
    while (shadowRoot) {
        const Element* ancestorShadowHost = shadowRoot->shadowHost();
        if (ancestorShadowHost == shadowHost)
            return true;
        shadowRoot = ancestorShadowHost->containingShadowRoot();
    }
    return false;
}

LayoutSize Element::minimumSizeForResizing() const
{
    return hasRareData() ? elementRareData()->minimumSizeForResizing() : defaultMinimumSizeForResizing();
}

void Element::setMinimumSizeForResizing(const LayoutSize& size)
{
    if (!hasRareData() && size == defaultMinimumSizeForResizing())
        return;
    ensureElementRareData().setMinimumSizeForResizing(size);
}

const ComputedStyle* Element::ensureComputedStyle(PseudoId pseudoElementSpecifier)
{
    if (PseudoElement* element = pseudoElement(pseudoElementSpecifier))
        return element->ensureComputedStyle();

    if (!inActiveDocument()) {
        // FIXME: Try to do better than this. Ensure that styleForElement() works for elements that are not in the
        // document tree and figure out when to destroy the computed style for such elements.
        return nullptr;
    }

    // FIXME: Find and use the layoutObject from the pseudo element instead of the actual element so that the 'length'
    // properties, which are only known by the layoutObject because it did the layout, will be correct and so that the
    // values returned for the ":selection" pseudo-element will be correct.
    ComputedStyle* elementStyle = mutableComputedStyle();
    if (!elementStyle) {
        ElementRareData& rareData = ensureElementRareData();
        if (!rareData.ensureComputedStyle())
            rareData.setComputedStyle(document().styleForElementIgnoringPendingStylesheets(this));
        elementStyle = rareData.ensureComputedStyle();
    }

    if (!pseudoElementSpecifier)
        return elementStyle;

    if (ComputedStyle* pseudoElementStyle = elementStyle->getCachedPseudoStyle(pseudoElementSpecifier))
        return pseudoElementStyle;

    RefPtr<ComputedStyle> result = document().ensureStyleResolver().pseudoStyleForElement(this, PseudoStyleRequest(pseudoElementSpecifier, PseudoStyleRequest::ForComputedStyle), elementStyle);
    ASSERT(result);
    return elementStyle->addCachedPseudoStyle(result.release());
}

AtomicString Element::computeInheritedLanguage() const
{
    const Node* n = this;
    AtomicString value;
    // The language property is inherited, so we iterate over the parents to find the first language.
    do {
        if (n->isElementNode()) {
            if (const ElementData* elementData = toElement(n)->elementData()) {
                AttributeCollection attributes = elementData->attributes();
                // Spec: xml:lang takes precedence -- http://www.w3.org/TR/xhtml1/#C_7
                if (const Attribute* attribute = attributes.find(XMLNames::langAttr))
                    value = attribute->value();
                else if (const Attribute* attribute = attributes.find(HTMLNames::langAttr))
                    value = attribute->value();
            }
        } else if (n->isDocumentNode()) {
            // checking the MIME content-language
            value = toDocument(n)->contentLanguage();
        }

        n = n->parentOrShadowHostNode();
    } while (n && value.isNull());

    return value;
}

Locale& Element::locale() const
{
    return document().getCachedLocale(computeInheritedLanguage());
}

void Element::cancelFocusAppearanceUpdate()
{
    if (document().focusedElement() == this)
        document().cancelFocusAppearanceUpdate();
}

void Element::updatePseudoElement(PseudoId pseudoId, StyleRecalcChange change)
{
    ASSERT(!needsStyleRecalc());
    PseudoElement* element = pseudoElement(pseudoId);

    if (element && (change == UpdatePseudoElements || element->shouldCallRecalcStyle(change))) {
        if (pseudoId == FIRST_LETTER && updateFirstLetter(element))
            return;

        // Need to clear the cached style if the PseudoElement wants a recalc so it
        // computes a new style.
        if (element->needsStyleRecalc())
            layoutObject()->mutableStyle()->removeCachedPseudoStyle(pseudoId);

        // PseudoElement styles hang off their parent element's style so if we needed
        // a style recalc we should Force one on the pseudo.
        // FIXME: We should figure out the right text sibling to pass.
        element->recalcStyle(change == UpdatePseudoElements ? Force : change);

        // Wait until our parent is not displayed or pseudoElementLayoutObjectIsNeeded
        // is false, otherwise we could continuously create and destroy PseudoElements
        // when LayoutObject::isChildAllowed on our parent returns false for the
        // PseudoElement's layoutObject for each style recalc.
        if (!layoutObject() || !pseudoElementLayoutObjectIsNeeded(layoutObject()->getCachedPseudoStyle(pseudoId)))
            elementRareData()->setPseudoElement(pseudoId, nullptr);
    } else if (pseudoId == FIRST_LETTER && element && change >= UpdatePseudoElements && !FirstLetterPseudoElement::firstLetterTextLayoutObject(*element)) {
        // This can happen if we change to a float, for example. We need to cleanup the
        // first-letter pseudoElement and then fix the text of the original remaining
        // text layoutObject.
        // This can be seen in Test 7 of fast/css/first-letter-removed-added.html
        elementRareData()->setPseudoElement(pseudoId, nullptr);
    } else if (change >= UpdatePseudoElements) {
        createPseudoElementIfNeeded(pseudoId);
    }
}

// If we're updating first letter, and the current first letter layoutObject
// is not the same as the one we're currently using we need to re-create
// the first letter layoutObject.
bool Element::updateFirstLetter(Element* element)
{
    LayoutObject* remainingTextLayoutObject = FirstLetterPseudoElement::firstLetterTextLayoutObject(*element);
    if (!remainingTextLayoutObject || remainingTextLayoutObject != toFirstLetterPseudoElement(element)->remainingTextLayoutObject()) {
        // We have to clear out the old first letter here because when it is
        // disposed it will set the original text back on the remaining text
        // layoutObject. If we dispose after creating the new one we will get
        // incorrect results due to setting the first letter back.
        if (remainingTextLayoutObject)
            element->reattach();
        else
            elementRareData()->setPseudoElement(FIRST_LETTER, nullptr);
        return true;
    }
    return false;
}

void Element::createPseudoElementIfNeeded(PseudoId pseudoId)
{
    if (isPseudoElement())
        return;

    // Document::ensureStyleResolver is not inlined and shows up on profiles, avoid it here.
    RefPtrWillBeRawPtr<PseudoElement> element = document().styleEngine().ensureResolver().createPseudoElementIfNeeded(*this, pseudoId);
    if (!element)
        return;

    if (pseudoId == BACKDROP)
        document().addToTopLayer(element.get(), this);
    element->insertedInto(this);
    element->attach();

    InspectorInstrumentation::pseudoElementCreated(element.get());

    ensureElementRareData().setPseudoElement(pseudoId, element.release());
}

PseudoElement* Element::pseudoElement(PseudoId pseudoId) const
{
    return hasRareData() ? elementRareData()->pseudoElement(pseudoId) : nullptr;
}

LayoutObject* Element::pseudoElementLayoutObject(PseudoId pseudoId) const
{
    if (PseudoElement* element = pseudoElement(pseudoId))
        return element->layoutObject();
    return nullptr;
}

bool Element::matches(const String& selectors, ExceptionState& exceptionState)
{
    SelectorQuery* selectorQuery = document().selectorQueryCache().add(AtomicString(selectors), document(), exceptionState);
    if (!selectorQuery)
        return false;
    return selectorQuery->matches(*this);
}

Element* Element::closest(const String& selectors, ExceptionState& exceptionState)
{
    SelectorQuery* selectorQuery = document().selectorQueryCache().add(AtomicString(selectors), document(), exceptionState);
    if (!selectorQuery)
        return nullptr;
    return selectorQuery->closest(*this);
}

DOMTokenList& Element::classList()
{
    ElementRareData& rareData = ensureElementRareData();
    if (!rareData.classList())
        rareData.setClassList(ClassList::create(this));
    return *rareData.classList();
}

DOMStringMap& Element::dataset()
{
    ElementRareData& rareData = ensureElementRareData();
    if (!rareData.dataset())
        rareData.setDataset(DatasetDOMStringMap::create(this));
    return *rareData.dataset();
}

KURL Element::hrefURL() const
{
    // FIXME: These all have href() or url(), but no common super class. Why doesn't
    // <link> implement URLUtils?
    if (isHTMLAnchorElement(*this) || isHTMLAreaElement(*this) || isHTMLLinkElement(*this))
        return getURLAttribute(hrefAttr);
    if (isSVGAElement(*this))
        return getURLAttribute(XLinkNames::hrefAttr);
    return KURL();
}

KURL Element::getURLAttribute(const QualifiedName& name) const
{
#if ENABLE(ASSERT)
    if (elementData()) {
        if (const Attribute* attribute = attributes().find(name))
            ASSERT(isURLAttribute(*attribute));
    }
#endif
    return document().completeURL(stripLeadingAndTrailingHTMLSpaces(getAttribute(name)));
}

KURL Element::getNonEmptyURLAttribute(const QualifiedName& name) const
{
#if ENABLE(ASSERT)
    if (elementData()) {
        if (const Attribute* attribute = attributes().find(name))
            ASSERT(isURLAttribute(*attribute));
    }
#endif
    String value = stripLeadingAndTrailingHTMLSpaces(getAttribute(name));
    if (value.isEmpty())
        return KURL();
    return document().completeURL(value);
}

int Element::getIntegralAttribute(const QualifiedName& attributeName) const
{
    return getAttribute(attributeName).toInt();
}

void Element::setIntegralAttribute(const QualifiedName& attributeName, int value)
{
    setAttribute(attributeName, AtomicString::number(value));
}

void Element::setUnsignedIntegralAttribute(const QualifiedName& attributeName, unsigned value)
{
    // Range restrictions are enforced for unsigned IDL attributes that
    // reflect content attributes,
    //   http://www.whatwg.org/specs/web-apps/current-work/multipage/common-dom-interfaces.html#reflecting-content-attributes-in-idl-attributes
    if (value > 0x7fffffffu)
        value = 0;
    setAttribute(attributeName, AtomicString::number(value));
}

double Element::getFloatingPointAttribute(const QualifiedName& attributeName, double fallbackValue) const
{
    return parseToDoubleForNumberType(getAttribute(attributeName), fallbackValue);
}

void Element::setFloatingPointAttribute(const QualifiedName& attributeName, double value)
{
    setAttribute(attributeName, AtomicString::number(value));
}

void Element::setContainsFullScreenElement(bool flag)
{
    setElementFlag(ContainsFullScreenElement, flag);
    setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::FullScreen));
}

static Element* parentCrossingFrameBoundaries(Element* element)
{
    ASSERT(element);
    return element->parentElement() ? element->parentElement() : element->document().ownerElement();
}

void Element::setContainsFullScreenElementOnAncestorsCrossingFrameBoundaries(bool flag)
{
    for (Element* element = parentCrossingFrameBoundaries(this); element; element = parentCrossingFrameBoundaries(element))
        element->setContainsFullScreenElement(flag);
}

void Element::setIsInTopLayer(bool inTopLayer)
{
    if (isInTopLayer() == inTopLayer)
        return;
    setElementFlag(IsInTopLayer, inTopLayer);

    // We must ensure a reattach occurs so the layoutObject is inserted in the correct sibling order under LayoutView according to its
    // top layer position, or in its usual place if not in the top layer.
    lazyReattachIfAttached();
}

void Element::requestPointerLock()
{
    if (document().page())
        document().page()->pointerLockController().requestPointerLock(this);
}

SpellcheckAttributeState Element::spellcheckAttributeState() const
{
    const AtomicString& value = fastGetAttribute(spellcheckAttr);
    if (value == nullAtom)
        return SpellcheckAttributeDefault;
    if (equalIgnoringCase(value, "true") || equalIgnoringCase(value, ""))
        return SpellcheckAttributeTrue;
    if (equalIgnoringCase(value, "false"))
        return SpellcheckAttributeFalse;

    return SpellcheckAttributeDefault;
}

bool Element::isSpellCheckingEnabled() const
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    for (const Element* element = this; element; element = element->parentOrShadowHostElement()) {
        switch (element->spellcheckAttributeState()) {
        case SpellcheckAttributeTrue:
            return true;
        case SpellcheckAttributeFalse:
            return false;
        case SpellcheckAttributeDefault:
            break;
        }
    }

    return true;
#else
    return false; // weolar
#endif
}

#if ENABLE(ASSERT)
bool Element::fastAttributeLookupAllowed(const QualifiedName& name) const
{
    if (name == HTMLNames::styleAttr)
        return false;

    if (isSVGElement())
        return !toSVGElement(this)->isAnimatableAttribute(name);

    return true;
}
#endif

#ifdef DUMP_NODE_STATISTICS
bool Element::hasNamedNodeMap() const
{
    return hasRareData() && elementRareData()->attributeMap();
}
#endif

inline void Element::updateName(const AtomicString& oldName, const AtomicString& newName)
{
    if (!inDocument() || isInShadowTree())
        return;

    if (oldName == newName)
        return;

    if (shouldRegisterAsNamedItem())
        updateNamedItemRegistration(oldName, newName);
}

inline void Element::updateId(const AtomicString& oldId, const AtomicString& newId)
{
    if (!isInTreeScope())
        return;

    if (oldId == newId)
        return;

    updateId(treeScope(), oldId, newId);
}

inline void Element::updateId(TreeScope& scope, const AtomicString& oldId, const AtomicString& newId)
{
    ASSERT(isInTreeScope());
    ASSERT(oldId != newId);

    if (!oldId.isEmpty())
        scope.removeElementById(oldId, this);
    if (!newId.isEmpty())
        scope.addElementById(newId, this);

    if (shouldRegisterAsExtraNamedItem())
        updateExtraNamedItemRegistration(oldId, newId);
}

void Element::willModifyAttribute(const QualifiedName& name, const AtomicString& oldValue, const AtomicString& newValue)
{
    if (name == HTMLNames::nameAttr) {
        updateName(oldValue, newValue);
    }

    if (oldValue != newValue) {
        if (inActiveDocument() && document().styleResolver() && styleChangeType() < SubtreeStyleChange)
            document().styleEngine().attributeChangedForElement(name, *this);

        if (isUpgradedCustomElement()) {
//#ifdef MINIBLINK_NOT_IMPLEMENTED
            CustomElement::attributeDidChange(this, name.localName(), oldValue, newValue);
//#endif // MINIBLINK_NOT_IMPLEMENTED
		
        }
    }

    if (OwnPtrWillBeRawPtr<MutationObserverInterestGroup> recipients = MutationObserverInterestGroup::createForAttributesMutation(*this, name))
        recipients->enqueueMutationRecord(MutationRecord::createAttributes(this, name, oldValue));

    attributeWillChange(name, oldValue, newValue);

    InspectorInstrumentation::willModifyDOMAttr(this, oldValue, newValue);
}

void Element::didAddAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == HTMLNames::idAttr)
        updateId(nullAtom, value);
    attributeChanged(name, value);
    InspectorInstrumentation::didModifyDOMAttr(this, name, value);
    dispatchSubtreeModifiedEvent();
}

void Element::didModifyAttribute(const QualifiedName& name, const AtomicString& oldValue, const AtomicString& newValue)
{
    if (name == HTMLNames::idAttr)
        updateId(oldValue, newValue);
    attributeChanged(name, newValue);
    InspectorInstrumentation::didModifyDOMAttr(this, name, newValue);
    // Do not dispatch a DOMSubtreeModified event here; see bug 81141.
}

void Element::didRemoveAttribute(const QualifiedName& name, const AtomicString& oldValue)
{
    if (name == HTMLNames::idAttr)
        updateId(oldValue, nullAtom);
    attributeChanged(name, nullAtom);
    InspectorInstrumentation::didRemoveDOMAttr(this, name);
    dispatchSubtreeModifiedEvent();
}

static bool needsURLResolutionForInlineStyle(const Element& element, const Document& oldDocument, const Document& newDocument)
{
    if (oldDocument == newDocument)
        return false;
    if (oldDocument.baseURL() == newDocument.baseURL())
        return false;
    const StylePropertySet* style = element.inlineStyle();
    if (!style)
        return false;
    for (unsigned i = 0; i < style->propertyCount(); ++i) {
        // FIXME: Should handle all URL-based properties: CSSImageSetValue, CSSCursorImageValue, etc.
        if (style->propertyAt(i).value()->isImageValue())
            return true;
    }
    return false;
}

static void reResolveURLsInInlineStyle(const Document& document, MutableStylePropertySet& style)
{
    for (unsigned i = 0; i < style.propertyCount(); ++i) {
        StylePropertySet::PropertyReference property = style.propertyAt(i);
        // FIXME: Should handle all URL-based properties: CSSImageSetValue, CSSCursorImageValue, etc.
        if (property.value()->isImageValue())
            toCSSImageValue(property.value())->reResolveURL(document);
    }
}

void Element::didMoveToNewDocument(Document& oldDocument)
{
    Node::didMoveToNewDocument(oldDocument);

    // If the documents differ by quirks mode then they differ by case sensitivity
    // for class and id names so we need to go through the attribute change logic
    // to pick up the new casing in the ElementData.
    if (oldDocument.inQuirksMode() != document().inQuirksMode()) {
        if (hasID())
            setIdAttribute(getIdAttribute());
        if (hasClass())
            setAttribute(HTMLNames::classAttr, getClassAttribute());
    }

    if (needsURLResolutionForInlineStyle(*this, oldDocument, document()))
        reResolveURLsInInlineStyle(document(), ensureMutableInlineStyle());
}

void Element::updateNamedItemRegistration(const AtomicString& oldName, const AtomicString& newName)
{
    if (!document().isHTMLDocument())
        return;

    if (!oldName.isEmpty())
        toHTMLDocument(document()).removeNamedItem(oldName);

    if (!newName.isEmpty())
        toHTMLDocument(document()).addNamedItem(newName);
}

void Element::updateExtraNamedItemRegistration(const AtomicString& oldId, const AtomicString& newId)
{
    if (!document().isHTMLDocument())
        return;

    if (!oldId.isEmpty())
        toHTMLDocument(document()).removeExtraNamedItem(oldId);

    if (!newId.isEmpty())
        toHTMLDocument(document()).addExtraNamedItem(newId);
}

void Element::scheduleSVGFilterLayerUpdateHack()
{
    document().scheduleSVGFilterLayerUpdateHack(*this);
}

IntSize Element::savedLayerScrollOffset() const
{
    return hasRareData() ? elementRareData()->savedLayerScrollOffset() : IntSize();
}

void Element::setSavedLayerScrollOffset(const IntSize& size)
{
    if (size.isZero() && !hasRareData())
        return;
    ensureElementRareData().setSavedLayerScrollOffset(size);
}

PassRefPtrWillBeRawPtr<Attr> Element::attrIfExists(const QualifiedName& name)
{
    if (AttrNodeList* attrNodeList = this->attrNodeList()) {
        bool shouldIgnoreCase = shouldIgnoreAttributeCase();
        for (const auto& attr : *attrNodeList) {
            if (attr->qualifiedName().matchesPossiblyIgnoringCase(name, shouldIgnoreCase))
                return attr.get();
        }
    }
    return nullptr;
}

PassRefPtrWillBeRawPtr<Attr> Element::ensureAttr(const QualifiedName& name)
{
    RefPtrWillBeRawPtr<Attr> attrNode = attrIfExists(name);
    if (!attrNode) {
        attrNode = Attr::create(*this, name);
        treeScope().adoptIfNeeded(*attrNode);
        ensureAttrNodeList().append(attrNode);
    }
    return attrNode.release();
}

void Element::detachAttrNodeFromElementWithValue(Attr* attrNode, const AtomicString& value)
{
    ASSERT(attrNodeList());
    attrNode->detachFromElementWithValue(value);

    AttrNodeList* list = attrNodeList();
    size_t index = list->find(attrNode);
    ASSERT(index != kNotFound);
    list->remove(index);
    if (list->isEmpty())
        removeAttrNodeList();
}

void Element::detachAllAttrNodesFromElement()
{
    AttrNodeList* list = this->attrNodeList();
    if (!list)
        return;

    AttributeCollection attributes = elementData()->attributes();
    for (const Attribute& attr : attributes) {
        if (RefPtrWillBeRawPtr<Attr> attrNode = attrIfExists(attr.name()))
            attrNode->detachFromElementWithValue(attr.value());
    }

    removeAttrNodeList();
}

void Element::willRecalcStyle(StyleRecalcChange)
{
    ASSERT(hasCustomStyleCallbacks());
}

void Element::didRecalcStyle(StyleRecalcChange)
{
    ASSERT(hasCustomStyleCallbacks());
}


PassRefPtr<ComputedStyle> Element::customStyleForLayoutObject()
{
    ASSERT(hasCustomStyleCallbacks());
    return nullptr;
}

void Element::cloneAttributesFromElement(const Element& other)
{
    if (hasRareData())
        detachAllAttrNodesFromElement();

    other.synchronizeAllAttributes();
    if (!other.m_elementData) {
        m_elementData.clear();
        return;
    }

    const AtomicString& oldID = getIdAttribute();
    const AtomicString& newID = other.getIdAttribute();

    if (!oldID.isNull() || !newID.isNull())
        updateId(oldID, newID);

    const AtomicString& oldName = getNameAttribute();
    const AtomicString& newName = other.getNameAttribute();

    if (!oldName.isNull() || !newName.isNull())
        updateName(oldName, newName);

    // Quirks mode makes class and id not case sensitive. We can't share the ElementData
    // if the idForStyleResolution and the className need different casing.
    bool ownerDocumentsHaveDifferentCaseSensitivity = false;
    if (other.hasClass() || other.hasID())
        ownerDocumentsHaveDifferentCaseSensitivity = other.document().inQuirksMode() != document().inQuirksMode();

    // If 'other' has a mutable ElementData, convert it to an immutable one so we can share it between both elements.
    // We can only do this if there are no presentation attributes and sharing the data won't result in different case sensitivity of class or id.
    if (other.m_elementData->isUnique()
        && !ownerDocumentsHaveDifferentCaseSensitivity
        && !other.m_elementData->presentationAttributeStyle())
        const_cast<Element&>(other).m_elementData = toUniqueElementData(other.m_elementData)->makeShareableCopy();

    if (!other.m_elementData->isUnique() && !ownerDocumentsHaveDifferentCaseSensitivity && !needsURLResolutionForInlineStyle(other, other.document(), document()))
        m_elementData = other.m_elementData;
    else
        m_elementData = other.m_elementData->makeUniqueCopy();

    AttributeCollection attributes = m_elementData->attributes();
    for (const Attribute& attr : attributes)
        attributeChangedFromParserOrByCloning(attr.name(), attr.value(), ModifiedByCloning);
}

void Element::cloneDataFromElement(const Element& other)
{
    cloneAttributesFromElement(other);
    copyNonAttributePropertiesFromElement(other);
}

void Element::createUniqueElementData()
{
    if (!m_elementData) {
        m_elementData = UniqueElementData::create();
    } else {
        ASSERT(!m_elementData->isUnique());
        m_elementData = toShareableElementData(m_elementData)->makeUniqueCopy();
    }
}

void Element::synchronizeStyleAttributeInternal() const
{
    ASSERT(isStyledElement());
    ASSERT(elementData());
    ASSERT(elementData()->m_styleAttributeIsDirty);
    elementData()->m_styleAttributeIsDirty = false;
    const StylePropertySet* inlineStyle = this->inlineStyle();
    const_cast<Element*>(this)->setSynchronizedLazyAttribute(styleAttr,
        inlineStyle ? AtomicString(inlineStyle->asText()) : nullAtom);
}

CSSStyleDeclaration* Element::style()
{
    if (!isStyledElement())
        return nullptr;
    return &ensureElementRareData().ensureInlineCSSStyleDeclaration(this);
}

MutableStylePropertySet& Element::ensureMutableInlineStyle()
{
    ASSERT(isStyledElement());
    RefPtrWillBeMember<StylePropertySet>& inlineStyle = ensureUniqueElementData().m_inlineStyle;
    if (!inlineStyle) {
        CSSParserMode mode = (!isHTMLElement() || document().inQuirksMode()) ? HTMLQuirksMode : HTMLStandardMode;
        inlineStyle = MutableStylePropertySet::create(mode);
    } else if (!inlineStyle->isMutable()) {
        inlineStyle = inlineStyle->mutableCopy();
    }
    return *toMutableStylePropertySet(inlineStyle);
}

void Element::clearMutableInlineStyleIfEmpty()
{
    if (ensureMutableInlineStyle().isEmpty()) {
        ensureUniqueElementData().m_inlineStyle.clear();
    }
}

inline void Element::setInlineStyleFromString(const AtomicString& newStyleString)
{
    ASSERT(isStyledElement());
    RefPtrWillBeMember<StylePropertySet>& inlineStyle = elementData()->m_inlineStyle;

    // Avoid redundant work if we're using shared attribute data with already parsed inline style.
    if (inlineStyle && !elementData()->isUnique())
        return;

    // We reconstruct the property set instead of mutating if there is no CSSOM wrapper.
    // This makes wrapperless property sets immutable and so cacheable.
    if (inlineStyle && !inlineStyle->isMutable())
        inlineStyle.clear();

    if (!inlineStyle) {
        inlineStyle = CSSParser::parseInlineStyleDeclaration(newStyleString, this);
    } else {
        ASSERT(inlineStyle->isMutable());
        static_cast<MutableStylePropertySet*>(inlineStyle.get())->parseDeclarationList(newStyleString, document().elementSheet().contents());
    }
}

void Element::styleAttributeChanged(const AtomicString& newStyleString, AttributeModificationReason modificationReason)
{
    ASSERT(isStyledElement());
    WTF::OrdinalNumber startLineNumber = WTF::OrdinalNumber::beforeFirst();
    if (document().scriptableDocumentParser() && !document().isInDocumentWrite())
        startLineNumber = document().scriptableDocumentParser()->lineNumber();

    if (newStyleString.isNull()) {
        ensureUniqueElementData().m_inlineStyle.clear();
    } else if (modificationReason == ModifiedByCloning || document().contentSecurityPolicy()->allowInlineStyle(document().url(), startLineNumber, newStyleString)) {
        setInlineStyleFromString(newStyleString);
    }

    elementData()->m_styleAttributeIsDirty = false;

    setNeedsStyleRecalc(LocalStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::StyleSheetChange));
    InspectorInstrumentation::didInvalidateStyleAttr(this);
}

void Element::inlineStyleChanged()
{
    ASSERT(isStyledElement());
    setNeedsStyleRecalc(LocalStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::Inline));
    ASSERT(elementData());
    elementData()->m_styleAttributeIsDirty = true;
    InspectorInstrumentation::didInvalidateStyleAttr(this);
}

bool Element::setInlineStyleProperty(CSSPropertyID propertyID, CSSValueID identifier, bool important)
{
    ASSERT(isStyledElement());
    ensureMutableInlineStyle().setProperty(propertyID, cssValuePool().createIdentifierValue(identifier), important);
    inlineStyleChanged();
    return true;
}

bool Element::setInlineStyleProperty(CSSPropertyID propertyID, double value, CSSPrimitiveValue::UnitType unit, bool important)
{
    ASSERT(isStyledElement());
    ensureMutableInlineStyle().setProperty(propertyID, cssValuePool().createValue(value, unit), important);
    inlineStyleChanged();
    return true;
}

bool Element::setInlineStyleProperty(CSSPropertyID propertyID, const String& value, bool important)
{
    ASSERT(isStyledElement());
    bool changes = ensureMutableInlineStyle().setProperty(propertyID, value, important, document().elementSheet().contents());
    if (changes)
        inlineStyleChanged();
    return changes;
}

bool Element::removeInlineStyleProperty(CSSPropertyID propertyID)
{
    ASSERT(isStyledElement());
    if (!inlineStyle())
        return false;
    bool changes = ensureMutableInlineStyle().removeProperty(propertyID);
    if (changes)
        inlineStyleChanged();
    return changes;
}

void Element::removeAllInlineStyleProperties()
{
    ASSERT(isStyledElement());
    if (!inlineStyle())
        return;
    ensureMutableInlineStyle().clear();
    inlineStyleChanged();
}

void Element::updatePresentationAttributeStyle()
{
    synchronizeAllAttributes();
    // ShareableElementData doesn't store presentation attribute style, so make sure we have a UniqueElementData.
    UniqueElementData& elementData = ensureUniqueElementData();
    elementData.m_presentationAttributeStyleIsDirty = false;
    elementData.m_presentationAttributeStyle = computePresentationAttributeStyle(*this);
}

void Element::addPropertyToPresentationAttributeStyle(MutableStylePropertySet* style, CSSPropertyID propertyID, CSSValueID identifier)
{
    ASSERT(isStyledElement());
    style->setProperty(propertyID, cssValuePool().createIdentifierValue(identifier));
}

void Element::addPropertyToPresentationAttributeStyle(MutableStylePropertySet* style, CSSPropertyID propertyID, double value, CSSPrimitiveValue::UnitType unit)
{
    ASSERT(isStyledElement());
    style->setProperty(propertyID, cssValuePool().createValue(value, unit));
}

void Element::addPropertyToPresentationAttributeStyle(MutableStylePropertySet* style, CSSPropertyID propertyID, const String& value)
{
    ASSERT(isStyledElement());
    style->setProperty(propertyID, value, false);
}

bool Element::supportsStyleSharing() const
{
    if (!isStyledElement() || !parentOrShadowHostElement())
        return false;
    // If the element has inline style it is probably unique.
    if (inlineStyle())
        return false;
    if (isSVGElement() && toSVGElement(this)->animatedSMILStyleProperties())
        return false;
    // Ids stop style sharing if they show up in the stylesheets.
    if (hasID() && document().ensureStyleResolver().hasRulesForId(idForStyleResolution()))
        return false;
    // :active and :hover elements always make a chain towards the document node
    // and no siblings or cousins will have the same state. There's also only one
    // :focus element per scope so we don't need to attempt to share.
    if (isUserActionElement())
        return false;
    if (!parentOrShadowHostElement()->childrenSupportStyleSharing())
        return false;
    if (this == document().cssTarget())
        return false;
    if (isHTMLElement() && toHTMLElement(this)->hasDirectionAuto())
        return false;
    if (hasAnimations())
        return false;
    // Turn off style sharing for elements that can gain layers for reasons outside of the style system.
    // See comments in LayoutObject::setStyle().
    // FIXME: Why does gaining a layer from outside the style system require disabling sharing?
    if (isHTMLFrameElementBase(*this) || isHTMLPlugInElement(*this) || isHTMLCanvasElement(*this))
        return false;
    if (Fullscreen::isActiveFullScreenElement(*this))
        return false;
    return true;
}

DEFINE_TRACE(Element)
{
#if ENABLE(OILPAN)
    if (hasRareData())
        visitor->trace(elementRareData());
    visitor->trace(m_elementData);
#endif
    ContainerNode::trace(visitor);
}

} // namespace blink
