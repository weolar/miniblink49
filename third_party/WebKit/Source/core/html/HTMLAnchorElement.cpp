/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Simon Hausmann <hausmann@kde.org>
 * Copyright (C) 2003, 2006, 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
 *           (C) 2006 Graham Dennis (graham.dennis@gmail.com)
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
#include "core/html/HTMLAnchorElement.h"

#include "bindings/core/v8/V8DOMActivityLogger.h"
#include "core/events/KeyboardEvent.h"
#include "core/events/MouseEvent.h"
#include "core/frame/FrameHost.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLImageElement.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/layout/LayoutImage.h"
#include "core/loader/FrameLoadRequest.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/loader/PingLoader.h"
#include "core/page/ChromeClient.h"
#include "platform/network/NetworkHints.h"

namespace blink {

using namespace HTMLNames;

HTMLAnchorElement::HTMLAnchorElement(const QualifiedName& tagName, Document& document)
    : HTMLElement(tagName, document)
    , m_linkRelations(0)
    , m_cachedVisitedLinkHash(0)
    , m_wasFocusedByMouse(false)
{
}

PassRefPtrWillBeRawPtr<HTMLAnchorElement> HTMLAnchorElement::create(Document& document)
{
    return adoptRefWillBeNoop(new HTMLAnchorElement(aTag, document));
}

HTMLAnchorElement::~HTMLAnchorElement()
{
}

bool HTMLAnchorElement::supportsFocus() const
{
    if (hasEditableStyle())
        return HTMLElement::supportsFocus();
    // If not a link we should still be able to focus the element if it has tabIndex.
    return isLink() || HTMLElement::supportsFocus();
}

bool HTMLAnchorElement::shouldHaveFocusAppearance() const
{
    return !m_wasFocusedByMouse || HTMLElement::supportsFocus();
}

void HTMLAnchorElement::dispatchFocusEvent(Element* oldFocusedElement, WebFocusType type)
{
    if (type != WebFocusTypePage)
        m_wasFocusedByMouse = type == WebFocusTypeMouse;
    HTMLElement::dispatchFocusEvent(oldFocusedElement, type);
}

void HTMLAnchorElement::dispatchBlurEvent(Element* newFocusedElement, WebFocusType type)
{
    if (type != WebFocusTypePage)
        m_wasFocusedByMouse = false;
    HTMLElement::dispatchBlurEvent(newFocusedElement, type);
}

bool HTMLAnchorElement::isMouseFocusable() const
{
    if (isLink())
        return supportsFocus();

    return HTMLElement::isMouseFocusable();
}

bool HTMLAnchorElement::isKeyboardFocusable() const
{
    ASSERT(document().isActive());

    if (isFocusable() && Element::supportsFocus())
        return HTMLElement::isKeyboardFocusable();

    if (isLink() && !document().frameHost()->chromeClient().tabsToLinks())
        return false;
    return HTMLElement::isKeyboardFocusable();
}

static void appendServerMapMousePosition(StringBuilder& url, Event* event)
{
    if (!event->isMouseEvent())
        return;

    ASSERT(event->target());
    Node* target = event->target()->toNode();
    ASSERT(target);
    if (!isHTMLImageElement(*target))
        return;

    HTMLImageElement& imageElement = toHTMLImageElement(*target);
    if (!imageElement.isServerMap())
        return;

    LayoutObject* layoutObject = imageElement.layoutObject();
    if (!layoutObject || !layoutObject->isBox())
        return;

    // The coordinates sent in the query string are relative to the height and
    // width of the image element, ignoring CSS transform/zoom.
    LayoutPoint mapPoint(layoutObject->absoluteToLocal(FloatPoint(toMouseEvent(event)->absoluteLocation()), UseTransforms));

    // The origin (0,0) is at the upper left of the content area, inside the
    // padding and border.
    mapPoint -= toLayoutBox(layoutObject)->contentBoxOffset();

    // CSS zoom is not reflected in the map coordinates.
    float scaleFactor = 1 / layoutObject->style()->effectiveZoom();
    mapPoint.scale(scaleFactor, scaleFactor);

    // Negative coordinates are clamped to 0 such that clicks in the left and
    // top padding/border areas receive an X or Y coordinate of 0.
    IntPoint clampedPoint(roundedIntPoint(mapPoint));
    clampedPoint.clampNegativeToZero();

    url.append('?');
    url.appendNumber(clampedPoint.x());
    url.append(',');
    url.appendNumber(clampedPoint.y());
}

void HTMLAnchorElement::defaultEventHandler(Event* event)
{
    if (isLink()) {
        if (focused() && isEnterKeyKeydownEvent(event) && isLiveLink()) {
            event->setDefaultHandled();
            dispatchSimulatedClick(event);
            return;
        }

        if (isLinkClick(event) && isLiveLink()) {
            handleClick(event);
            return;
        }
    }

    HTMLElement::defaultEventHandler(event);
}

void HTMLAnchorElement::setActive(bool down)
{
    if (hasEditableStyle())
        return;

    ContainerNode::setActive(down);
}

void HTMLAnchorElement::attributeWillChange(const QualifiedName& name, const AtomicString& oldValue, const AtomicString& newValue)
{
    if (name == hrefAttr && inDocument()) {
        V8DOMActivityLogger* activityLogger = V8DOMActivityLogger::currentActivityLoggerIfIsolatedWorld();
        if (activityLogger) {
            Vector<String> argv;
            argv.append("a");
            argv.append(hrefAttr.toString());
            argv.append(oldValue);
            argv.append(newValue);
            activityLogger->logEvent("blinkSetAttribute", argv.size(), argv.data());
        }
    }
    HTMLElement::attributeWillChange(name, oldValue, newValue);
}

void HTMLAnchorElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == hrefAttr) {
        bool wasLink = isLink();
        setIsLink(!value.isNull());
        if (wasLink || isLink()) {
            pseudoStateChanged(CSSSelector::PseudoLink);
            pseudoStateChanged(CSSSelector::PseudoVisited);
            pseudoStateChanged(CSSSelector::PseudoAnyLink);
        }
        if (wasLink && !isLink() && treeScope().adjustedFocusedElement() == this) {
            // We might want to call blur(), but it's dangerous to dispatch
            // events here.
            document().setNeedsFocusedElementCheck();
        }
        if (isLink()) {
            String parsedURL = stripLeadingAndTrailingHTMLSpaces(value);
            if (document().isDNSPrefetchEnabled()) {
                if (protocolIs(parsedURL, "http") || protocolIs(parsedURL, "https") || parsedURL.startsWith("//"))
                    prefetchDNS(document().completeURL(parsedURL).host());
            }
        }
        invalidateCachedVisitedLinkHash();
    } else if (name == nameAttr || name == titleAttr) {
        // Do nothing.
    } else if (name == relAttr) {
        setRel(value);
    } else {
        HTMLElement::parseAttribute(name, value);
    }
}

void HTMLAnchorElement::accessKeyAction(bool sendMouseEvents)
{
    dispatchSimulatedClick(0, sendMouseEvents ? SendMouseUpDownEvents : SendNoEvents);
}

bool HTMLAnchorElement::isURLAttribute(const Attribute& attribute) const
{
    return attribute.name().localName() == hrefAttr || HTMLElement::isURLAttribute(attribute);
}

bool HTMLAnchorElement::hasLegalLinkAttribute(const QualifiedName& name) const
{
    return name == hrefAttr || HTMLElement::hasLegalLinkAttribute(name);
}

bool HTMLAnchorElement::canStartSelection() const
{
    if (!isLink())
        return HTMLElement::canStartSelection();
    return hasEditableStyle();
}

bool HTMLAnchorElement::draggable() const
{
    // Should be draggable if we have an href attribute.
    const AtomicString& value = getAttribute(draggableAttr);
    if (equalIgnoringCase(value, "true"))
        return true;
    if (equalIgnoringCase(value, "false"))
        return false;
    return hasAttribute(hrefAttr);
}

KURL HTMLAnchorElement::href() const
{
    return document().completeURL(stripLeadingAndTrailingHTMLSpaces(getAttribute(hrefAttr)));
}

void HTMLAnchorElement::setHref(const AtomicString& value)
{
    setAttribute(hrefAttr, value);
}

KURL HTMLAnchorElement::url() const
{
    return href();
}

void HTMLAnchorElement::setURL(const KURL& url)
{
    setHref(AtomicString(url.string()));
}

String HTMLAnchorElement::input() const
{
    return getAttribute(hrefAttr);
}

void HTMLAnchorElement::setInput(const String& value)
{
    setHref(AtomicString(value));
}

bool HTMLAnchorElement::hasRel(uint32_t relation) const
{
    return m_linkRelations & relation;
}

void HTMLAnchorElement::setRel(const AtomicString& value)
{
    m_linkRelations = 0;
    SpaceSplitString newLinkRelations(value, SpaceSplitString::ShouldFoldCase);
    // FIXME: Add link relations as they are implemented
    if (newLinkRelations.contains("noreferrer"))
        m_linkRelations |= RelationNoReferrer;
}

const AtomicString& HTMLAnchorElement::name() const
{
    return getNameAttribute();
}

short HTMLAnchorElement::tabIndex() const
{
    // Skip the supportsFocus check in HTMLElement.
    return Element::tabIndex();
}

bool HTMLAnchorElement::isLiveLink() const
{
    return isLink() && !hasEditableStyle();
}

void HTMLAnchorElement::sendPings(const KURL& destinationURL) const
{
    const AtomicString& pingValue = getAttribute(pingAttr);
    if (pingValue.isNull() || !document().settings() || !document().settings()->hyperlinkAuditingEnabled())
        return;

    UseCounter::count(document(), UseCounter::HTMLAnchorElementPingAttribute);

    SpaceSplitString pingURLs(pingValue, SpaceSplitString::ShouldNotFoldCase);
    for (unsigned i = 0; i < pingURLs.size(); i++)
        PingLoader::sendLinkAuditPing(document().frame(), document().completeURL(pingURLs[i]), destinationURL);
}

void HTMLAnchorElement::handleClick(Event* event)
{
    event->setDefaultHandled();

    LocalFrame* frame = document().frame();
    if (!frame)
        return;

    StringBuilder url;
    url.append(stripLeadingAndTrailingHTMLSpaces(fastGetAttribute(hrefAttr)));
    appendServerMapMousePosition(url, event);
    KURL completedURL = document().completeURL(url.toString());

    // Schedule the ping before the frame load. Prerender in Chrome may kill the renderer as soon as the navigation is
    // sent out.
    sendPings(completedURL);

    ResourceRequest request(completedURL);
    request.setUIStartTime(event->uiCreateTime());
    request.setInputPerfMetricReportPolicy(InputToLoadPerfMetricReportPolicy::ReportLink);
    if (hasAttribute(downloadAttr)) {
        request.setRequestContext(WebURLRequest::RequestContextDownload);
        bool isSameOrigin = completedURL.protocolIsData() || document().securityOrigin()->canRequest(completedURL);
        const AtomicString& suggestedName = (isSameOrigin ? fastGetAttribute(downloadAttr) : nullAtom);

        frame->loader().client()->loadURLExternally(request, NavigationPolicyDownload, suggestedName);
    } else {
        request.setRequestContext(WebURLRequest::RequestContextHyperlink);
        FrameLoadRequest frameRequest(&document(), request, getAttribute(targetAttr));
        frameRequest.setTriggeringEvent(event);
        if (hasRel(RelationNoReferrer))
            frameRequest.setShouldSendReferrer(NeverSendReferrer);
        frame->loader().load(frameRequest);
    }
}

bool isEnterKeyKeydownEvent(Event* event)
{
    return event->type() == EventTypeNames::keydown && event->isKeyboardEvent() && toKeyboardEvent(event)->keyIdentifier() == "Enter";
}

bool isLinkClick(Event* event)
{
    return event->type() == EventTypeNames::click && (!event->isMouseEvent() || toMouseEvent(event)->button() != RightButton);
}

bool HTMLAnchorElement::willRespondToMouseClickEvents()
{
    return isLink() || HTMLElement::willRespondToMouseClickEvents();
}

bool HTMLAnchorElement::isInteractiveContent() const
{
    return isLink();
}

Node::InsertionNotificationRequest HTMLAnchorElement::insertedInto(ContainerNode* insertionPoint)
{
    if (insertionPoint->inDocument()) {
        V8DOMActivityLogger* activityLogger = V8DOMActivityLogger::currentActivityLoggerIfIsolatedWorld();
        if (activityLogger) {
            Vector<String> argv;
            argv.append("a");
            argv.append(fastGetAttribute(hrefAttr));
            activityLogger->logEvent("blinkAddElement", argv.size(), argv.data());
        }
    }
    return HTMLElement::insertedInto(insertionPoint);
}

} // namespace blink
