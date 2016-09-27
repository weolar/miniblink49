/*
 * Copyright (C) 2006, 2008, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies)
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
 *
*/

#include "config.h"
#include "core/layout/HitTestResult.h"

#include "core/HTMLNames.h"
#include "core/dom/DocumentMarkerController.h"
#include "core/dom/PseudoElement.h"
#include "core/dom/shadow/ComposedTreeTraversal.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/FrameSelection.h"
#include "core/fetch/ImageResource.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLAnchorElement.h"
#include "core/html/HTMLAreaElement.h"
#include "core/html/HTMLImageElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLMapElement.h"
#include "core/html/HTMLMediaElement.h"
#include "core/html/HTMLTextAreaElement.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/layout/LayoutImage.h"
#include "core/layout/LayoutTextFragment.h"
#include "core/page/FrameTree.h"
#include "core/svg/SVGElement.h"
#include "platform/scroll/Scrollbar.h"

namespace blink {

using namespace HTMLNames;

HitTestResult::HitTestResult()
    : m_hitTestRequest(HitTestRequest::ReadOnly | HitTestRequest::Active)
    , m_cacheable(true)
    , m_isOverWidget(false)
{
}

HitTestResult::HitTestResult(const HitTestRequest& request, const LayoutPoint& point)
    : m_hitTestLocation(point)
    , m_hitTestRequest(request)
    , m_cacheable(true)
    , m_pointInInnerNodeFrame(point)
    , m_isOverWidget(false)
{
}

HitTestResult::HitTestResult(const HitTestRequest& request, const LayoutPoint& centerPoint, unsigned topPadding, unsigned rightPadding, unsigned bottomPadding, unsigned leftPadding)
    : m_hitTestLocation(centerPoint, topPadding, rightPadding, bottomPadding, leftPadding)
    , m_hitTestRequest(request)
    , m_cacheable(true)
    , m_pointInInnerNodeFrame(centerPoint)
    , m_isOverWidget(false)
{
}

HitTestResult::HitTestResult(const HitTestRequest& otherRequest, const HitTestLocation& other)
    : m_hitTestLocation(other)
    , m_hitTestRequest(otherRequest)
    , m_cacheable(true)
    , m_pointInInnerNodeFrame(m_hitTestLocation.point())
    , m_isOverWidget(false)
{
}

HitTestResult::HitTestResult(const HitTestResult& other)
    : m_hitTestLocation(other.m_hitTestLocation)
    , m_hitTestRequest(other.m_hitTestRequest)
    , m_cacheable(other.m_cacheable)
    , m_innerNode(other.innerNode())
    , m_innerPossiblyPseudoNode(other.m_innerPossiblyPseudoNode)
    , m_pointInInnerNodeFrame(other.m_pointInInnerNodeFrame)
    , m_localPoint(other.localPoint())
    , m_innerURLElement(other.URLElement())
    , m_scrollbar(other.scrollbar())
    , m_isOverWidget(other.isOverWidget())
{
    // Only copy the NodeSet in case of list hit test.
    m_listBasedTestResult = adoptPtrWillBeNoop(other.m_listBasedTestResult ? new NodeSet(*other.m_listBasedTestResult) : 0);
}

HitTestResult::~HitTestResult()
{
}

HitTestResult& HitTestResult::operator=(const HitTestResult& other)
{
    m_hitTestLocation = other.m_hitTestLocation;
    m_hitTestRequest = other.m_hitTestRequest;
    populateFromCachedResult(other);

    return *this;
}

unsigned HitTestResult::equalityScore(const HitTestResult& other) const
{
    return (m_hitTestRequest.equalForCacheability(other.m_hitTestRequest) << 7)
        | ((m_innerNode == other.innerNode()) << 6)
        | ((m_innerPossiblyPseudoNode == other.innerPossiblyPseudoNode()) << 5)
        | ((m_pointInInnerNodeFrame == other.m_pointInInnerNodeFrame) << 4)
        | ((m_localPoint == other.localPoint()) << 3)
        | ((m_innerURLElement == other.URLElement()) << 2)
        | ((m_scrollbar == other.scrollbar()) << 1)
        | (m_isOverWidget == other.isOverWidget());
}

bool HitTestResult::equalForCacheability(const HitTestResult& other) const
{
    return m_hitTestRequest.equalForCacheability(other.m_hitTestRequest)
        && m_innerNode == other.innerNode()
        && m_innerPossiblyPseudoNode == other.innerPossiblyPseudoNode()
        && m_pointInInnerNodeFrame == other.m_pointInInnerNodeFrame
        && m_localPoint == other.localPoint()
        && m_innerURLElement == other.URLElement()
        && m_scrollbar == other.scrollbar()
        && m_isOverWidget == other.isOverWidget();
}

void HitTestResult::cacheValues(const HitTestResult& other)
{
    *this = other;
    m_hitTestRequest = other.m_hitTestRequest.type() & ~HitTestRequest::AvoidCache;
}

void HitTestResult::populateFromCachedResult(const HitTestResult& other)
{
    m_innerNode = other.innerNode();
    m_innerPossiblyPseudoNode = other.innerPossiblyPseudoNode();
    m_pointInInnerNodeFrame = other.m_pointInInnerNodeFrame;
    m_localPoint = other.localPoint();
    m_innerURLElement = other.URLElement();
    m_scrollbar = other.scrollbar();
    m_isOverWidget = other.isOverWidget();
    m_cacheable = other.m_cacheable;

    // Only copy the NodeSet in case of list hit test.
    m_listBasedTestResult = adoptPtrWillBeNoop(other.m_listBasedTestResult ? new NodeSet(*other.m_listBasedTestResult) : 0);
}

DEFINE_TRACE(HitTestResult)
{
    visitor->trace(m_innerNode);
    visitor->trace(m_innerPossiblyPseudoNode);
    visitor->trace(m_innerURLElement);
    visitor->trace(m_scrollbar);
#if ENABLE(OILPAN)
    visitor->trace(m_listBasedTestResult);
#endif
}

PositionWithAffinity HitTestResult::position() const
{
    if (!m_innerPossiblyPseudoNode)
        return PositionWithAffinity();
    LayoutObject* layoutObject = this->layoutObject();
    if (!layoutObject)
        return PositionWithAffinity();
    if (m_innerPossiblyPseudoNode->isPseudoElement() && m_innerPossiblyPseudoNode->pseudoId() == BEFORE)
        return Position(m_innerNode, PositionAnchorType::BeforeChildren).downstream();
    return layoutObject->positionForPoint(localPoint());
}

LayoutObject* HitTestResult::layoutObject() const
{
    return m_innerNode ? m_innerNode->layoutObject() : 0;
}

void HitTestResult::setToShadowHostIfInUserAgentShadowRoot()
{
    if (Node* node = innerNode()) {
        if (ShadowRoot* containingShadowRoot = node->containingShadowRoot()) {
            if (containingShadowRoot->type() == ShadowRootType::UserAgent)
                setInnerNode(node->shadowHost());
        }
    }
}

HTMLAreaElement* HitTestResult::imageAreaForImage() const
{
    ASSERT(m_innerNode);
    HTMLImageElement* imageElement = nullptr;
    if (isHTMLImageElement(m_innerNode)) {
        imageElement = toHTMLImageElement(m_innerNode);
    } else if (m_innerNode->isInShadowTree()) {
        if (m_innerNode->containingShadowRoot()->type() == ShadowRootType::UserAgent) {
            if (isHTMLImageElement(m_innerNode->shadowHost()))
                imageElement = toHTMLImageElement(m_innerNode->shadowHost());
        }
    }

    if (!imageElement || !imageElement->layoutObject() || !imageElement->layoutObject()->isBox())
        return nullptr;

    HTMLMapElement* map = imageElement->treeScope().getImageMap(imageElement->fastGetAttribute(usemapAttr));
    if (!map)
        return nullptr;

    LayoutBox* box = toLayoutBox(imageElement->layoutObject());
    LayoutRect contentBox = box->contentBoxRect();
    float scaleFactor = 1 / box->style()->effectiveZoom();
    LayoutPoint location = localPoint();
    location.scale(scaleFactor, scaleFactor);

    return map->areaForPoint(location, contentBox.size());
}

void HitTestResult::setInnerNode(Node* n)
{
    m_innerPossiblyPseudoNode = n;
    if (n && n->isPseudoElement())
        n = toPseudoElement(n)->findAssociatedNode();
    m_innerNode = n;
    if (HTMLAreaElement* area = imageAreaForImage())
        m_innerNode = area;
}

void HitTestResult::setURLElement(Element* n)
{
    m_innerURLElement = n;
}

void HitTestResult::setScrollbar(Scrollbar* s)
{
    m_scrollbar = s;
}

LocalFrame* HitTestResult::innerNodeFrame() const
{
    if (m_innerNode)
        return m_innerNode->document().frame();
    return nullptr;
}

bool HitTestResult::isSelected() const
{
    if (!m_innerNode)
        return false;

    if (LocalFrame* frame = m_innerNode->document().frame())
        return frame->selection().contains(m_hitTestLocation.point());
    return false;
}

String HitTestResult::spellingToolTip(TextDirection& dir) const
{
    dir = LTR;
    // Return the tool tip string associated with this point, if any. Only markers associated with bad grammar
    // currently supply strings, but maybe someday markers associated with misspelled words will also.
    if (!m_innerNode)
        return String();

    DocumentMarker* marker = m_innerNode->document().markers().markerContainingPoint(m_hitTestLocation.point(), DocumentMarker::Grammar);
    if (!marker)
        return String();

    if (LayoutObject* layoutObject = m_innerNode->layoutObject())
        dir = layoutObject->style()->direction();
    return marker->description();
}

String HitTestResult::title(TextDirection& dir) const
{
    dir = LTR;
    // Find the title in the nearest enclosing DOM node.
    // For <area> tags in image maps, walk the tree for the <area>, not the <img> using it.
    if (m_innerNode.get())
        m_innerNode->updateDistribution();
    for (Node* titleNode = m_innerNode.get(); titleNode; titleNode = ComposedTreeTraversal::parent(*titleNode)) {
        if (titleNode->isElementNode()) {
            String title = toElement(titleNode)->title();
            if (!title.isNull()) {
                if (LayoutObject* layoutObject = titleNode->layoutObject())
                    dir = layoutObject->style()->direction();
                return title;
            }
        }
    }
    return String();
}

const AtomicString& HitTestResult::altDisplayString() const
{
    Node* innerNodeOrImageMapImage = this->innerNodeOrImageMapImage();
    if (!innerNodeOrImageMapImage)
        return nullAtom;

    if (isHTMLImageElement(*innerNodeOrImageMapImage)) {
        HTMLImageElement& image = toHTMLImageElement(*innerNodeOrImageMapImage);
        return image.getAttribute(altAttr);
    }

    if (isHTMLInputElement(*innerNodeOrImageMapImage)) {
        HTMLInputElement& input = toHTMLInputElement(*innerNodeOrImageMapImage);
        return input.alt();
    }

    return nullAtom;
}

Image* HitTestResult::image() const
{
    Node* innerNodeOrImageMapImage = this->innerNodeOrImageMapImage();
    if (!innerNodeOrImageMapImage)
        return nullptr;

    LayoutObject* layoutObject = innerNodeOrImageMapImage->layoutObject();
    if (layoutObject && layoutObject->isImage()) {
        LayoutImage* image = toLayoutImage(layoutObject);
        if (image->cachedImage() && !image->cachedImage()->errorOccurred())
            return image->cachedImage()->imageForLayoutObject(image);
    }

    return nullptr;
}

IntRect HitTestResult::imageRect() const
{
    if (!image())
        return IntRect();
    return innerNodeOrImageMapImage()->layoutBox()->absoluteContentQuad().enclosingBoundingBox();
}

KURL HitTestResult::absoluteImageURL() const
{
    Node* innerNodeOrImageMapImage = this->innerNodeOrImageMapImage();
    if (!innerNodeOrImageMapImage)
        return KURL();

    LayoutObject* layoutObject = innerNodeOrImageMapImage->layoutObject();
    if (!(layoutObject && layoutObject->isImage()))
        return KURL();

    AtomicString urlString;
    if (isHTMLEmbedElement(*innerNodeOrImageMapImage)
        || isHTMLImageElement(*innerNodeOrImageMapImage)
        || isHTMLInputElement(*innerNodeOrImageMapImage)
        || isHTMLObjectElement(*innerNodeOrImageMapImage)
        || isSVGImageElement(*innerNodeOrImageMapImage)) {
        urlString = toElement(*innerNodeOrImageMapImage).imageSourceURL();
    } else {
        return KURL();
    }

    return innerNodeOrImageMapImage->document().completeURL(stripLeadingAndTrailingHTMLSpaces(urlString));
}

KURL HitTestResult::absoluteMediaURL() const
{
    if (HTMLMediaElement* mediaElt = mediaElement())
        return mediaElt->currentSrc();
    return KURL();
}

HTMLMediaElement* HitTestResult::mediaElement() const
{
    if (!m_innerNode)
        return nullptr;

    if (!(m_innerNode->layoutObject() && m_innerNode->layoutObject()->isMedia()))
        return nullptr;

    if (isHTMLMediaElement(*m_innerNode))
        return toHTMLMediaElement(m_innerNode);
    return nullptr;
}

KURL HitTestResult::absoluteLinkURL() const
{
    if (!m_innerURLElement)
        return KURL();
    return m_innerURLElement->hrefURL();
}

bool HitTestResult::isLiveLink() const
{
    return m_innerURLElement && m_innerURLElement->isLiveLink();
}

bool HitTestResult::isMisspelled() const
{
    if (!innerNode() || !innerNode()->layoutObject())
        return false;
    VisiblePosition pos(innerNode()->layoutObject()->positionForPoint(localPoint()));
    if (pos.isNull())
        return false;
    return m_innerNode->document().markers().markersInRange(
        makeRange(pos, pos).get(), DocumentMarker::MisspellingMarkers()).size() > 0;
}

bool HitTestResult::isOverLink() const
{
    return m_innerURLElement && m_innerURLElement->isLink();
}

String HitTestResult::textContent() const
{
    if (!m_innerURLElement)
        return String();
    return m_innerURLElement->textContent();
}

// FIXME: This function needs a better name and may belong in a different class. It's not
// really isContentEditable(); it's more like needsEditingContextMenu(). In many ways, this
// function would make more sense in the ContextMenu class, except that WebElementDictionary
// hooks into it. Anyway, we should architect this better.
bool HitTestResult::isContentEditable() const
{
    if (!m_innerNode)
        return false;

    if (isHTMLTextAreaElement(*m_innerNode))
        return !toHTMLTextAreaElement(*m_innerNode).isDisabledOrReadOnly();

    if (isHTMLInputElement(*m_innerNode)) {
        HTMLInputElement& inputElement = toHTMLInputElement(*m_innerNode);
        return !inputElement.isDisabledOrReadOnly() && inputElement.isTextField();
    }

    return m_innerNode->hasEditableStyle();
}

bool HitTestResult::addNodeToListBasedTestResult(Node* node, const HitTestLocation& locationInContainer, const LayoutRect& rect)
{
    // If not a list-based test, this function should be a no-op.
    if (!hitTestRequest().listBased())
        return false;

    // If node is null, return true so the hit test can continue.
    if (!node)
        return true;

    mutableListBasedTestResult().add(node);

    if (hitTestRequest().penetratingList())
        return true;

    bool regionFilled = rect.contains(LayoutRect(locationInContainer.boundingBox()));
    return !regionFilled;
}

bool HitTestResult::addNodeToListBasedTestResult(Node* node, const HitTestLocation& locationInContainer, const FloatRect& rect)
{
    // If not a list-based test, this function should be a no-op.
    if (!hitTestRequest().listBased())
        return false;

    // If node is null, return true so the hit test can continue.
    if (!node)
        return true;

    mutableListBasedTestResult().add(node);

    if (hitTestRequest().penetratingList())
        return true;

    bool regionFilled = rect.contains(locationInContainer.boundingBox());
    return !regionFilled;
}

void HitTestResult::append(const HitTestResult& other)
{
    ASSERT(hitTestRequest().listBased());

    if (!m_scrollbar && other.scrollbar()) {
        setScrollbar(other.scrollbar());
    }

    if (!m_innerNode && other.innerNode()) {
        m_innerNode = other.innerNode();
        m_innerPossiblyPseudoNode = other.innerPossiblyPseudoNode();
        m_localPoint = other.localPoint();
        m_pointInInnerNodeFrame = other.m_pointInInnerNodeFrame;
        m_innerURLElement = other.URLElement();
        m_isOverWidget = other.isOverWidget();
    }

    if (other.m_listBasedTestResult) {
        NodeSet& set = mutableListBasedTestResult();
        for (NodeSet::const_iterator it = other.m_listBasedTestResult->begin(), last = other.m_listBasedTestResult->end(); it != last; ++it)
            set.add(it->get());
    }
}

const HitTestResult::NodeSet& HitTestResult::listBasedTestResult() const
{
    if (!m_listBasedTestResult)
        m_listBasedTestResult = adoptPtrWillBeNoop(new NodeSet);
    return *m_listBasedTestResult;
}

HitTestResult::NodeSet& HitTestResult::mutableListBasedTestResult()
{
    if (!m_listBasedTestResult)
        m_listBasedTestResult = adoptPtrWillBeNoop(new NodeSet);
    return *m_listBasedTestResult;
}

void HitTestResult::resolveRectBasedTest(Node* resolvedInnerNode, const LayoutPoint& resolvedPointInMainFrame)
{
    ASSERT(isRectBasedTest());
    ASSERT(m_hitTestLocation.containsPoint(FloatPoint(resolvedPointInMainFrame)));
    m_hitTestLocation = HitTestLocation(resolvedPointInMainFrame);
    m_pointInInnerNodeFrame = resolvedPointInMainFrame;
    m_innerNode = nullptr;
    m_innerPossiblyPseudoNode = nullptr;
    m_listBasedTestResult = nullptr;

    // Update the HitTestResult as if the supplied node had been hit in normal point-based hit-test.
    // Note that we don't know the local point after a rect-based hit-test, but we never use
    // it so shouldn't bother with the cost of computing it.
    resolvedInnerNode->layoutObject()->updateHitTestResult(*this, LayoutPoint());
    ASSERT(!isRectBasedTest());
}

Element* HitTestResult::innerElement() const
{
    for (Node* node = m_innerNode.get(); node; node = ComposedTreeTraversal::parent(*node)) {
        if (node->isElementNode())
            return toElement(node);
    }

    return nullptr;
}

Node* HitTestResult::innerNodeOrImageMapImage() const
{
    if (!m_innerNode)
        return nullptr;

    HTMLImageElement* imageMapImageElement = nullptr;
    if (isHTMLAreaElement(m_innerNode))
        imageMapImageElement = toHTMLAreaElement(m_innerNode)->imageElement();
    else if (isHTMLMapElement(m_innerNode))
        imageMapImageElement = toHTMLMapElement(m_innerNode)->imageElement();

    if (!imageMapImageElement)
        return m_innerNode.get();

    return imageMapImageElement;
}

} // namespace blink
