/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008 Apple Inc. All rights reserved.
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
#include "core/html/HTMLScriptElement.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "bindings/core/v8/ScriptEventListener.h"
#include "bindings/core/v8/V8DOMActivityLogger.h"
#include "core/HTMLNames.h"
#include "core/dom/Attribute.h"
#include "core/dom/Document.h"
#include "core/dom/ScriptLoader.h"
#include "core/dom/ScriptRunner.h"
#include "core/dom/Text.h"
#include "core/events/Event.h"
#include "core/frame/UseCounter.h"

namespace blink {

using namespace HTMLNames;

inline HTMLScriptElement::HTMLScriptElement(Document& document, bool wasInsertedByParser, bool alreadyStarted)
    : HTMLElement(scriptTag, document)
    , m_loader(ScriptLoader::create(this, wasInsertedByParser, alreadyStarted))
{
}

PassRefPtrWillBeRawPtr<HTMLScriptElement> HTMLScriptElement::create(Document& document, bool wasInsertedByParser, bool alreadyStarted)
{
    return adoptRefWillBeNoop(new HTMLScriptElement(document, wasInsertedByParser, alreadyStarted));
}

bool HTMLScriptElement::isURLAttribute(const Attribute& attribute) const
{
    return attribute.name() == srcAttr || HTMLElement::isURLAttribute(attribute);
}

bool HTMLScriptElement::hasLegalLinkAttribute(const QualifiedName& name) const
{
    return name == srcAttr || HTMLElement::hasLegalLinkAttribute(name);
}

const QualifiedName& HTMLScriptElement::subResourceAttributeName() const
{
    return srcAttr;
}

void HTMLScriptElement::childrenChanged(const ChildrenChange& change)
{
    HTMLElement::childrenChanged(change);
    m_loader->childrenChanged();
}

void HTMLScriptElement::didMoveToNewDocument(Document& oldDocument)
{
    ScriptRunner::movePendingAsyncScript(oldDocument, document(), m_loader.get());
    HTMLElement::didMoveToNewDocument(oldDocument);
}

void HTMLScriptElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == srcAttr)
        m_loader->handleSourceAttribute(value);
    else if (name == asyncAttr)
        m_loader->handleAsyncAttribute();
    else
        HTMLElement::parseAttribute(name, value);
}

void HTMLScriptElement::attributeWillChange(const QualifiedName& name, const AtomicString& oldValue, const AtomicString& newValue)
{
    if (name == srcAttr && inDocument()) {
        V8DOMActivityLogger* activityLogger = V8DOMActivityLogger::currentActivityLoggerIfIsolatedWorld();
        if (activityLogger) {
            Vector<String> argv;
            argv.append("script");
            argv.append(srcAttr.toString());
            argv.append(oldValue);
            argv.append(newValue);
            activityLogger->logEvent("blinkSetAttribute", argv.size(), argv.data());
        }
    }
    HTMLElement::attributeWillChange(name, oldValue, newValue);
}

Node::InsertionNotificationRequest HTMLScriptElement::insertedInto(ContainerNode* insertionPoint)
{
    if (insertionPoint->inDocument()) {
        V8DOMActivityLogger* activityLogger = V8DOMActivityLogger::currentActivityLoggerIfIsolatedWorld();
        if (activityLogger) {
            Vector<String> argv;
            argv.append("script");
            argv.append(fastGetAttribute(srcAttr));
            activityLogger->logEvent("blinkAddElement", argv.size(), argv.data());
        }

        if (hasSourceAttribute() && !loader()->isScriptTypeSupported(ScriptLoader::DisallowLegacyTypeInTypeAttribute))
            UseCounter::count(document(), UseCounter::ScriptElementWithInvalidTypeHasSrc);
    }
    HTMLElement::insertedInto(insertionPoint);
    return InsertionShouldCallDidNotifySubtreeInsertions;
}

void HTMLScriptElement::didNotifySubtreeInsertionsToDocument()
{
    m_loader->didNotifySubtreeInsertionsToDocument();
}

void HTMLScriptElement::setText(const String &value)
{
    RefPtrWillBeRawPtr<Node> protectFromMutationEvents(this);

    if (hasOneTextChild()) {
        toText(firstChild())->setData(value);
        return;
    }

    removeChildren();
    appendChild(document().createTextNode(value.impl()), IGNORE_EXCEPTION);
}

void HTMLScriptElement::setAsync(bool async)
{
    setBooleanAttribute(asyncAttr, async);
    m_loader->handleAsyncAttribute();
}

bool HTMLScriptElement::async() const
{
    return fastHasAttribute(asyncAttr) || (m_loader->forceAsync());
}

KURL HTMLScriptElement::src() const
{
    return document().completeURL(sourceAttributeValue());
}

String HTMLScriptElement::sourceAttributeValue() const
{
    return getAttribute(srcAttr).string();
}

String HTMLScriptElement::charsetAttributeValue() const
{
    return getAttribute(charsetAttr).string();
}

String HTMLScriptElement::typeAttributeValue() const
{
    return getAttribute(typeAttr).string();
}

String HTMLScriptElement::languageAttributeValue() const
{
    return getAttribute(languageAttr).string();
}

String HTMLScriptElement::forAttributeValue() const
{
    return getAttribute(forAttr).string();
}

String HTMLScriptElement::eventAttributeValue() const
{
    return getAttribute(eventAttr).string();
}

bool HTMLScriptElement::asyncAttributeValue() const
{
    return fastHasAttribute(asyncAttr);
}

bool HTMLScriptElement::deferAttributeValue() const
{
    return fastHasAttribute(deferAttr);
}

bool HTMLScriptElement::hasSourceAttribute() const
{
    return fastHasAttribute(srcAttr);
}

void HTMLScriptElement::dispatchLoadEvent()
{
    ASSERT(!m_loader->haveFiredLoadEvent());
    dispatchEvent(Event::create(EventTypeNames::load));
}

PassRefPtrWillBeRawPtr<Element> HTMLScriptElement::cloneElementWithoutAttributesAndChildren()
{
    return adoptRefWillBeNoop(new HTMLScriptElement(document(), false, m_loader->alreadyStarted()));
}

DEFINE_TRACE(HTMLScriptElement)
{
    visitor->trace(m_loader);
    HTMLElement::trace(visitor);
}

}
