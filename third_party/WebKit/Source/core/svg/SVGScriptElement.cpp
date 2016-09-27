/*
 * Copyright (C) 2004, 2005, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2007 Rob Buis <buis@kde.org>
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

#include "core/svg/SVGScriptElement.h"

#include "bindings/core/v8/ScriptEventListener.h"
#include "core/HTMLNames.h"
#include "core/XLinkNames.h"
#include "core/dom/Attribute.h"
#include "core/dom/ScriptLoader.h"
#include "core/dom/ScriptRunner.h"
#include "core/events/Event.h"

namespace blink {

inline SVGScriptElement::SVGScriptElement(Document& document, bool wasInsertedByParser, bool alreadyStarted)
    : SVGElement(SVGNames::scriptTag, document)
    , SVGURIReference(this)
    , m_loader(ScriptLoader::create(this, wasInsertedByParser, alreadyStarted))
{
}

PassRefPtrWillBeRawPtr<SVGScriptElement> SVGScriptElement::create(Document& document, bool insertedByParser)
{
    return adoptRefWillBeNoop(new SVGScriptElement(document, insertedByParser, false));
}

void SVGScriptElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == HTMLNames::onerrorAttr)
        setAttributeEventListener(EventTypeNames::error, createAttributeEventListener(this, name, value, eventParameterName()));
    else
        SVGElement::parseAttribute(name, value);
}

void SVGScriptElement::svgAttributeChanged(const QualifiedName& attrName)
{
    if (SVGURIReference::isKnownAttribute(attrName)) {
        SVGElement::InvalidationGuard invalidationGuard(this);
        m_loader->handleSourceAttribute(hrefString());
        return;
    }

    SVGElement::svgAttributeChanged(attrName);
}

Node::InsertionNotificationRequest SVGScriptElement::insertedInto(ContainerNode* rootParent)
{
    SVGElement::insertedInto(rootParent);
    return InsertionShouldCallDidNotifySubtreeInsertions;
}

void SVGScriptElement::didNotifySubtreeInsertionsToDocument()
{
    m_loader->didNotifySubtreeInsertionsToDocument();

    if (!m_loader->isParserInserted())
        m_loader->setHaveFiredLoadEvent(true);
}

void SVGScriptElement::childrenChanged(const ChildrenChange& change)
{
    SVGElement::childrenChanged(change);
    m_loader->childrenChanged();
}

void SVGScriptElement::didMoveToNewDocument(Document& oldDocument)
{
    ScriptRunner::movePendingAsyncScript(oldDocument, document(), m_loader.get());
    SVGElement::didMoveToNewDocument(oldDocument);
}

bool SVGScriptElement::isURLAttribute(const Attribute& attribute) const
{
    return attribute.name() == AtomicString(sourceAttributeValue());
}

void SVGScriptElement::finishParsingChildren()
{
    SVGElement::finishParsingChildren();
    m_loader->setHaveFiredLoadEvent(true);
}

bool SVGScriptElement::haveLoadedRequiredResources()
{
    return m_loader->haveFiredLoadEvent();
}

String SVGScriptElement::sourceAttributeValue() const
{
    return hrefString();
}

String SVGScriptElement::charsetAttributeValue() const
{
    return String();
}

String SVGScriptElement::typeAttributeValue() const
{
    return getAttribute(SVGNames::typeAttr).string();
}

String SVGScriptElement::languageAttributeValue() const
{
    return String();
}

String SVGScriptElement::forAttributeValue() const
{
    return String();
}

String SVGScriptElement::eventAttributeValue() const
{
    return String();
}

bool SVGScriptElement::asyncAttributeValue() const
{
    return false;
}

bool SVGScriptElement::deferAttributeValue() const
{
    return false;
}

bool SVGScriptElement::hasSourceAttribute() const
{
    return href()->isSpecified();
}

PassRefPtrWillBeRawPtr<Element> SVGScriptElement::cloneElementWithoutAttributesAndChildren()
{
    return adoptRefWillBeNoop(new SVGScriptElement(document(), false, m_loader->alreadyStarted()));
}

void SVGScriptElement::dispatchLoadEvent()
{
    dispatchEvent(Event::create(EventTypeNames::load));
}

#if ENABLE(ASSERT)
bool SVGScriptElement::isAnimatableAttribute(const QualifiedName& name) const
{
    if (name == SVGNames::typeAttr)
        return false;

    return SVGElement::isAnimatableAttribute(name);
}
#endif

DEFINE_TRACE(SVGScriptElement)
{
    visitor->trace(m_loader);
    SVGElement::trace(visitor);
    SVGURIReference::trace(visitor);
}

} // namespace blink
