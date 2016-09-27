/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2003, 2010 Apple Inc. All rights reserved.
 *           (C) 2007 Rob Buis (buis@kde.org)
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
#include "core/html/HTMLStyleElement.h"

#include "core/HTMLNames.h"
#include "core/css/MediaList.h"
#include "core/dom/Document.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/events/Event.h"
#include "core/events/EventSender.h"

namespace blink {

using namespace HTMLNames;

static StyleEventSender& styleLoadEventSender()
{
    DEFINE_STATIC_LOCAL(StyleEventSender, sharedLoadEventSender, (EventTypeNames::load));
    return sharedLoadEventSender;
}

inline HTMLStyleElement::HTMLStyleElement(Document& document, bool createdByParser)
    : HTMLElement(styleTag, document)
    , StyleElement(&document, createdByParser)
    , m_firedLoad(false)
    , m_loadedSheet(false)
{
}

HTMLStyleElement::~HTMLStyleElement()
{
#if !ENABLE(OILPAN)
    StyleElement::clearDocumentData(document(), this);
#endif

    styleLoadEventSender().cancelEvent(this);
}

PassRefPtrWillBeRawPtr<HTMLStyleElement> HTMLStyleElement::create(Document& document, bool createdByParser)
{
    return adoptRefWillBeNoop(new HTMLStyleElement(document, createdByParser));
}

void HTMLStyleElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == titleAttr && m_sheet) {
        m_sheet->setTitle(value);
    } else if (name == mediaAttr && inDocument() && document().isActive() && m_sheet) {
        m_sheet->setMediaQueries(MediaQuerySet::create(value));
        document().modifiedStyleSheet(m_sheet.get());
    } else {
        HTMLElement::parseAttribute(name, value);
    }
}

void HTMLStyleElement::finishParsingChildren()
{
    StyleElement::ProcessingResult result = StyleElement::finishParsingChildren(this);
    HTMLElement::finishParsingChildren();
    if (result == StyleElement::ProcessingFatalError)
        notifyLoadedSheetAndAllCriticalSubresources(ErrorOccurredLoadingSubresource);
}

Node::InsertionNotificationRequest HTMLStyleElement::insertedInto(ContainerNode* insertionPoint)
{
    HTMLElement::insertedInto(insertionPoint);
    StyleElement::insertedInto(this, insertionPoint);
    return InsertionShouldCallDidNotifySubtreeInsertions;
}

void HTMLStyleElement::removedFrom(ContainerNode* insertionPoint)
{
    HTMLElement::removedFrom(insertionPoint);
    StyleElement::removedFrom(this, insertionPoint);
}

void HTMLStyleElement::didNotifySubtreeInsertionsToDocument()
{
    if (StyleElement::processStyleSheet(document(), this) == StyleElement::ProcessingFatalError)
        notifyLoadedSheetAndAllCriticalSubresources(ErrorOccurredLoadingSubresource);
}

void HTMLStyleElement::childrenChanged(const ChildrenChange& change)
{
    HTMLElement::childrenChanged(change);
    if (StyleElement::childrenChanged(this) == StyleElement::ProcessingFatalError)
        notifyLoadedSheetAndAllCriticalSubresources(ErrorOccurredLoadingSubresource);
}

const AtomicString& HTMLStyleElement::media() const
{
    return getAttribute(mediaAttr);
}

const AtomicString& HTMLStyleElement::type() const
{
    return getAttribute(typeAttr);
}

ContainerNode* HTMLStyleElement::scopingNode()
{
    if (!inDocument())
        return nullptr;

    if (isInShadowTree())
        return containingShadowRoot();

    return &document();
}

void HTMLStyleElement::dispatchPendingLoadEvents()
{
    styleLoadEventSender().dispatchPendingEvents();
}

void HTMLStyleElement::dispatchPendingEvent(StyleEventSender* eventSender)
{
    ASSERT_UNUSED(eventSender, eventSender == &styleLoadEventSender());
    dispatchEvent(Event::create(m_loadedSheet ? EventTypeNames::load : EventTypeNames::error));
}

void HTMLStyleElement::notifyLoadedSheetAndAllCriticalSubresources(LoadedSheetErrorStatus errorStatus)
{
    bool isLoadEvent = errorStatus == NoErrorLoadingSubresource;
    if (m_firedLoad && isLoadEvent)
        return;
    m_loadedSheet = isLoadEvent;
    styleLoadEventSender().dispatchEventSoon(this);
    m_firedLoad = true;
}

bool HTMLStyleElement::disabled() const
{
    if (!m_sheet)
        return false;

    return m_sheet->disabled();
}

void HTMLStyleElement::setDisabled(bool setDisabled)
{
    if (CSSStyleSheet* styleSheet = sheet())
        styleSheet->setDisabled(setDisabled);
}

DEFINE_TRACE(HTMLStyleElement)
{
    StyleElement::trace(visitor);
    HTMLElement::trace(visitor);
}

}
