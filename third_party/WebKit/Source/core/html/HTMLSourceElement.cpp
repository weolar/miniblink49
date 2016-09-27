/*
 * Copyright (C) 2007, 2008, 2010 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/HTMLSourceElement.h"

#include "core/HTMLNames.h"
#include "core/css/MediaList.h"
#include "core/css/MediaQueryList.h"
#include "core/css/MediaQueryMatcher.h"
#include "core/dom/Document.h"
#include "core/events/Event.h"
#include "core/events/EventSender.h"
#include "core/html/HTMLMediaElement.h"
#include "core/html/HTMLPictureElement.h"
#include "platform/Logging.h"

namespace blink {

using namespace HTMLNames;

static SourceEventSender& sourceErrorEventSender()
{
    DEFINE_STATIC_LOCAL(SourceEventSender, sharedErrorEventSender, (EventTypeNames::error));
    return sharedErrorEventSender;
}

class HTMLSourceElement::Listener final : public MediaQueryListListener {
public:
    explicit Listener(HTMLSourceElement* element) : m_element(element) { }
    void notifyMediaQueryChanged() override
    {
        if (m_element)
            m_element->notifyMediaQueryChanged();
    }

    void clearElement() { m_element = nullptr; }
    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        visitor->trace(m_element);
        MediaQueryListListener::trace(visitor);
    }
private:
    RawPtrWillBeMember<HTMLSourceElement> m_element;
};

inline HTMLSourceElement::HTMLSourceElement(Document& document)
    : HTMLElement(sourceTag, document)
    , m_listener(adoptRefWillBeNoop(new Listener(this)))
{
    WTF_LOG(Media, "HTMLSourceElement::HTMLSourceElement - %p", this);
}

DEFINE_NODE_FACTORY(HTMLSourceElement)

HTMLSourceElement::~HTMLSourceElement()
{
    sourceErrorEventSender().cancelEvent(this);
#if !ENABLE(OILPAN)
    m_listener->clearElement();
#endif
}

void HTMLSourceElement::createMediaQueryList(const AtomicString& media)
{
    if (media.isEmpty())
        return;

    if (m_mediaQueryList)
        m_mediaQueryList->removeListener(m_listener);
    RefPtrWillBeRawPtr<MediaQuerySet> set = MediaQuerySet::create(media);
    m_mediaQueryList = MediaQueryList::create(&document(), &document().mediaQueryMatcher(), set.release());
    m_mediaQueryList->addListener(m_listener);
}

void HTMLSourceElement::didMoveToNewDocument(Document& oldDocument)
{
    createMediaQueryList(fastGetAttribute(mediaAttr));
    HTMLElement::didMoveToNewDocument(oldDocument);
}

Node::InsertionNotificationRequest HTMLSourceElement::insertedInto(ContainerNode* insertionPoint)
{
    HTMLElement::insertedInto(insertionPoint);
    Element* parent = parentElement();
    if (isHTMLMediaElement(parent))
        toHTMLMediaElement(parent)->sourceWasAdded(this);
    if (isHTMLPictureElement(parent))
        toHTMLPictureElement(parent)->sourceOrMediaChanged();
    return InsertionDone;
}

void HTMLSourceElement::removedFrom(ContainerNode* removalRoot)
{
    Element* parent = parentElement();
    if (!parent && removalRoot->isElementNode())
        parent = toElement(removalRoot);
    if (isHTMLMediaElement(parent))
        toHTMLMediaElement(parent)->sourceWasRemoved(this);
    if (isHTMLPictureElement(parent))
        toHTMLPictureElement(parent)->sourceOrMediaChanged();
    HTMLElement::removedFrom(removalRoot);
}

void HTMLSourceElement::setSrc(const String& url)
{
    setAttribute(srcAttr, AtomicString(url));
}

const AtomicString& HTMLSourceElement::type() const
{
    return getAttribute(typeAttr);
}

void HTMLSourceElement::setType(const AtomicString& type)
{
    setAttribute(typeAttr, type);
}

void HTMLSourceElement::scheduleErrorEvent()
{
    WTF_LOG(Media, "HTMLSourceElement::scheduleErrorEvent - %p", this);
    sourceErrorEventSender().dispatchEventSoon(this);
}

void HTMLSourceElement::cancelPendingErrorEvent()
{
    WTF_LOG(Media, "HTMLSourceElement::cancelPendingErrorEvent - %p", this);
    sourceErrorEventSender().cancelEvent(this);
}

void HTMLSourceElement::dispatchPendingEvent(SourceEventSender* eventSender)
{
    ASSERT_UNUSED(eventSender, eventSender == &sourceErrorEventSender());
    WTF_LOG(Media, "HTMLSourceElement::dispatchPendingEvent - %p", this);
    dispatchEvent(Event::createCancelable(EventTypeNames::error));
}

bool HTMLSourceElement::mediaQueryMatches() const
{
    if (!m_mediaQueryList)
        return true;

    return m_mediaQueryList->matches();
}

bool HTMLSourceElement::isURLAttribute(const Attribute& attribute) const
{
    return attribute.name() == srcAttr || HTMLElement::isURLAttribute(attribute);
}

void HTMLSourceElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    HTMLElement::parseAttribute(name, value);
    if (name == mediaAttr)
        createMediaQueryList(value);
    if (name == srcsetAttr || name == sizesAttr || name == mediaAttr || name == typeAttr) {
        Element* parent = parentElement();
        if (isHTMLPictureElement(parent))
            toHTMLPictureElement(parent)->sourceOrMediaChanged();
    }
}

void HTMLSourceElement::notifyMediaQueryChanged()
{
    Element* parent = parentElement();
    if (isHTMLPictureElement(parent))
        toHTMLPictureElement(parent)->sourceOrMediaChanged();
}

DEFINE_TRACE(HTMLSourceElement)
{
    visitor->trace(m_mediaQueryList);
    visitor->trace(m_listener);
    HTMLElement::trace(visitor);
}

}
