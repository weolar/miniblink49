/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Simon Hausmann (hausmann@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2006, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2009 Ericsson AB. All rights reserved.
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
#include "core/html/HTMLIFrameElement.h"

#include "bindings/core/v8/V8DOMActivityLogger.h"
#include "core/CSSPropertyNames.h"
#include "core/HTMLNames.h"
#include "core/frame/UseCounter.h"
#include "core/html/HTMLDocument.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/layout/LayoutIFrame.h"

namespace blink {

using namespace HTMLNames;

inline HTMLIFrameElement::HTMLIFrameElement(Document& document)
    : HTMLFrameElementBase(iframeTag, document)
    , m_didLoadNonEmptyDocument(false)
    , m_sandbox(DOMSettableTokenList::create(this))
{
}

DEFINE_NODE_FACTORY(HTMLIFrameElement)

DEFINE_TRACE(HTMLIFrameElement)
{
    visitor->trace(m_sandbox);
    HTMLFrameElementBase::trace(visitor);
    DOMSettableTokenListObserver::trace(visitor);
}

HTMLIFrameElement::~HTMLIFrameElement()
{
#if !ENABLE(OILPAN)
    m_sandbox->setObserver(nullptr);
#endif
}

DOMSettableTokenList* HTMLIFrameElement::sandbox() const
{
    return m_sandbox.get();
}

bool HTMLIFrameElement::isPresentationAttribute(const QualifiedName& name) const
{
    if (name == widthAttr || name == heightAttr || name == alignAttr || name == frameborderAttr)
        return true;
    return HTMLFrameElementBase::isPresentationAttribute(name);
}

void HTMLIFrameElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    if (name == widthAttr) {
        addHTMLLengthToStyle(style, CSSPropertyWidth, value);
    } else if (name == heightAttr) {
        addHTMLLengthToStyle(style, CSSPropertyHeight, value);
    } else if (name == alignAttr) {
        applyAlignmentAttributeToStyle(value, style);
    } else if (name == frameborderAttr) {
        // LocalFrame border doesn't really match the HTML4 spec definition for iframes. It simply adds
        // a presentational hint that the border should be off if set to zero.
        if (!value.toInt()) {
            // Add a rule that nulls out our border width.
            addPropertyToPresentationAttributeStyle(style, CSSPropertyBorderWidth, 0, CSSPrimitiveValue::CSS_PX);
        }
    } else {
        HTMLFrameElementBase::collectStyleForPresentationAttribute(name, value, style);
    }
}

void HTMLIFrameElement::attributeWillChange(const QualifiedName& name, const AtomicString& oldValue, const AtomicString& newValue)
{
    if (name == srcAttr && inDocument()) {
        V8DOMActivityLogger* activityLogger = V8DOMActivityLogger::currentActivityLoggerIfIsolatedWorld();
        if (activityLogger) {
            Vector<String> argv;
            argv.append("iframe");
            argv.append(srcAttr.toString());
            argv.append(oldValue);
            argv.append(newValue);
            activityLogger->logEvent("blinkSetAttribute", argv.size(), argv.data());
        }
    }
    HTMLFrameElementBase::attributeWillChange(name, oldValue, newValue);
}

void HTMLIFrameElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == nameAttr) {
        if (inDocument() && document().isHTMLDocument() && !isInShadowTree()) {
            HTMLDocument& document = toHTMLDocument(this->document());
            document.removeExtraNamedItem(m_name);
            document.addExtraNamedItem(value);
        }
        m_name = value;
    } else if (name == sandboxAttr) {
        m_sandbox->setValue(value);
        UseCounter::count(document(), UseCounter::SandboxViaIFrame);
    } else {
        HTMLFrameElementBase::parseAttribute(name, value);
    }
}

bool HTMLIFrameElement::layoutObjectIsNeeded(const ComputedStyle& style)
{
    return isURLAllowed() && HTMLElement::layoutObjectIsNeeded(style);
}

LayoutObject* HTMLIFrameElement::createLayoutObject(const ComputedStyle&)
{
    return new LayoutIFrame(this);
}

Node::InsertionNotificationRequest HTMLIFrameElement::insertedInto(ContainerNode* insertionPoint)
{
    if (insertionPoint->inDocument()) {
        V8DOMActivityLogger* activityLogger = V8DOMActivityLogger::currentActivityLoggerIfIsolatedWorld();
        if (activityLogger) {
            Vector<String> argv;
            argv.append("iframe");
            argv.append(fastGetAttribute(srcAttr));
            activityLogger->logEvent("blinkAddElement", argv.size(), argv.data());
        }
    }
    InsertionNotificationRequest result = HTMLFrameElementBase::insertedInto(insertionPoint);
    if (insertionPoint->inDocument() && document().isHTMLDocument() && !insertionPoint->isInShadowTree())
        toHTMLDocument(document()).addExtraNamedItem(m_name);
    return result;
}

void HTMLIFrameElement::removedFrom(ContainerNode* insertionPoint)
{
    HTMLFrameElementBase::removedFrom(insertionPoint);
    if (insertionPoint->inDocument() && document().isHTMLDocument() && !insertionPoint->isInShadowTree())
        toHTMLDocument(document()).removeExtraNamedItem(m_name);
}

bool HTMLIFrameElement::isInteractiveContent() const
{
    return true;
}

void HTMLIFrameElement::valueChanged()
{
    String invalidTokens;
    setSandboxFlags(m_sandbox->value().isNull() ? SandboxNone : parseSandboxPolicy(m_sandbox->tokens(), invalidTokens));
    if (!invalidTokens.isNull())
        document().addConsoleMessage(ConsoleMessage::create(OtherMessageSource, ErrorMessageLevel, "Error while parsing the 'sandbox' attribute: " + invalidTokens));
    setSynchronizedLazyAttribute(sandboxAttr, m_sandbox->value());
}

}
