/*
 * Copyright (C) 2004, 2005 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2006 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Cameron McCormack <cam@mcc.id.au>
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
#include "core/svg/SVGStyleElement.h"

#include "core/MediaTypeNames.h"
#include "core/css/CSSStyleSheet.h"
#include "core/events/Event.h"
#include "wtf/StdLibExtras.h"

namespace blink {

static SVGStyleEventSender& styleErrorEventSender()
{
    DEFINE_STATIC_LOCAL(SVGStyleEventSender, sharedErrorEventSender, (EventTypeNames::error));
    return sharedErrorEventSender;
}

inline SVGStyleElement::SVGStyleElement(Document& document, bool createdByParser)
    : SVGElement(SVGNames::styleTag, document)
    , StyleElement(&document, createdByParser)
{
}

SVGStyleElement::~SVGStyleElement()
{
#if !ENABLE(OILPAN)
    StyleElement::clearDocumentData(document(), this);
#endif

    styleErrorEventSender().cancelEvent(this);
}

PassRefPtrWillBeRawPtr<SVGStyleElement> SVGStyleElement::create(Document& document, bool createdByParser)
{
    return adoptRefWillBeNoop(new SVGStyleElement(document, createdByParser));
}

bool SVGStyleElement::disabled() const
{
    if (!m_sheet)
        return false;

    return m_sheet->disabled();
}

void SVGStyleElement::setDisabled(bool setDisabled)
{
    if (CSSStyleSheet* styleSheet = sheet())
        styleSheet->setDisabled(setDisabled);
}

const AtomicString& SVGStyleElement::type() const
{
    DEFINE_STATIC_LOCAL(const AtomicString, defaultValue, ("text/css", AtomicString::ConstructFromLiteral));
    const AtomicString& n = getAttribute(SVGNames::typeAttr);
    return n.isNull() ? defaultValue : n;
}

void SVGStyleElement::setType(const AtomicString& type)
{
    setAttribute(SVGNames::typeAttr, type);
}

const AtomicString& SVGStyleElement::media() const
{
    const AtomicString& n = fastGetAttribute(SVGNames::mediaAttr);
    return n.isNull() ? MediaTypeNames::all : n;
}

void SVGStyleElement::setMedia(const AtomicString& media)
{
    setAttribute(SVGNames::mediaAttr, media);
}

String SVGStyleElement::title() const
{
    return fastGetAttribute(SVGNames::titleAttr);
}

void SVGStyleElement::setTitle(const AtomicString& title)
{
    setAttribute(SVGNames::titleAttr, title);
}

void SVGStyleElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == SVGNames::titleAttr) {
        if (m_sheet)
            m_sheet->setTitle(value);

        return;
    }

    SVGElement::parseAttribute(name, value);
}

void SVGStyleElement::finishParsingChildren()
{
    StyleElement::ProcessingResult result = StyleElement::finishParsingChildren(this);
    SVGElement::finishParsingChildren();
    if (result == StyleElement::ProcessingFatalError)
        notifyLoadedSheetAndAllCriticalSubresources(ErrorOccurredLoadingSubresource);
}

Node::InsertionNotificationRequest SVGStyleElement::insertedInto(ContainerNode* insertionPoint)
{
    SVGElement::insertedInto(insertionPoint);
    StyleElement::insertedInto(this, insertionPoint);
    return InsertionShouldCallDidNotifySubtreeInsertions;
}

void SVGStyleElement::didNotifySubtreeInsertionsToDocument()
{
    if (StyleElement::processStyleSheet(document(), this) == StyleElement::ProcessingFatalError)
        notifyLoadedSheetAndAllCriticalSubresources(ErrorOccurredLoadingSubresource);
}

void SVGStyleElement::removedFrom(ContainerNode* insertionPoint)
{
    SVGElement::removedFrom(insertionPoint);
    StyleElement::removedFrom(this, insertionPoint);
}

void SVGStyleElement::childrenChanged(const ChildrenChange& change)
{
    SVGElement::childrenChanged(change);
    if (StyleElement::childrenChanged(this) == StyleElement::ProcessingFatalError)
        notifyLoadedSheetAndAllCriticalSubresources(ErrorOccurredLoadingSubresource);
}

void SVGStyleElement::notifyLoadedSheetAndAllCriticalSubresources(LoadedSheetErrorStatus errorStatus)
{
    if (errorStatus != NoErrorLoadingSubresource)
        styleErrorEventSender().dispatchEventSoon(this);
}

void SVGStyleElement::dispatchPendingEvent(SVGStyleEventSender* eventSender)
{
    ASSERT_UNUSED(eventSender, eventSender == &styleErrorEventSender());
    dispatchEvent(Event::create(EventTypeNames::error));
}

DEFINE_TRACE(SVGStyleElement)
{
    StyleElement::trace(visitor);
    SVGElement::trace(visitor);
}

}
