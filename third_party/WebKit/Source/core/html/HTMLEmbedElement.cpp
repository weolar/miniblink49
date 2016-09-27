/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Stefan Schimanski (1Stein@gmx.de)
 * Copyright (C) 2004, 2005, 2006, 2008, 2009, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
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
#include "core/html/HTMLEmbedElement.h"

#include "core/CSSPropertyNames.h"
#include "core/HTMLNames.h"
#include "core/dom/Attribute.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/html/HTMLImageLoader.h"
#include "core/html/HTMLObjectElement.h"
#include "core/html/PluginDocument.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/layout/LayoutEmbeddedObject.h"
#include "core/layout/LayoutPart.h"

namespace blink {

using namespace HTMLNames;

inline HTMLEmbedElement::HTMLEmbedElement(Document& document, bool createdByParser)
    : HTMLPlugInElement(embedTag, document, createdByParser, ShouldPreferPlugInsForImages)
{
}

PassRefPtrWillBeRawPtr<HTMLEmbedElement> HTMLEmbedElement::create(Document& document, bool createdByParser)
{
    RefPtrWillBeRawPtr<HTMLEmbedElement> element = adoptRefWillBeNoop(new HTMLEmbedElement(document, createdByParser));
    element->ensureUserAgentShadowRoot();
    return element.release();
}

static inline LayoutPart* findPartLayoutObject(const Node* n)
{
    if (!n->layoutObject())
        n = Traversal<HTMLObjectElement>::firstAncestor(*n);

    if (n && n->layoutObject() && n->layoutObject()->isLayoutPart())
        return toLayoutPart(n->layoutObject());

    return nullptr;
}

LayoutPart* HTMLEmbedElement::existingLayoutPart() const
{
    return findPartLayoutObject(this);
}

bool HTMLEmbedElement::isPresentationAttribute(const QualifiedName& name) const
{
    if (name == hiddenAttr)
        return true;
    return HTMLPlugInElement::isPresentationAttribute(name);
}

void HTMLEmbedElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    if (name == hiddenAttr) {
        if (equalIgnoringCase(value, "yes") || equalIgnoringCase(value, "true")) {
            addPropertyToPresentationAttributeStyle(style, CSSPropertyWidth, 0, CSSPrimitiveValue::CSS_PX);
            addPropertyToPresentationAttributeStyle(style, CSSPropertyHeight, 0, CSSPrimitiveValue::CSS_PX);
        }
    } else {
        HTMLPlugInElement::collectStyleForPresentationAttribute(name, value, style);
    }
}

void HTMLEmbedElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == typeAttr) {
        m_serviceType = value.lower();
        size_t pos = m_serviceType.find(";");
        if (pos != kNotFound)
            m_serviceType = m_serviceType.left(pos);
        if (!layoutObject())
            requestPluginCreationWithoutLayoutObjectIfPossible();
    } else if (name == codeAttr) {
        m_url = stripLeadingAndTrailingHTMLSpaces(value);
    } else if (name == srcAttr) {
        m_url = stripLeadingAndTrailingHTMLSpaces(value);
        if (layoutObject() && isImageType()) {
            if (!m_imageLoader)
                m_imageLoader = HTMLImageLoader::create(this);
            m_imageLoader->updateFromElement(ImageLoader::UpdateIgnorePreviousError);
        }
    } else {
        HTMLPlugInElement::parseAttribute(name, value);
    }
}

void HTMLEmbedElement::parametersForPlugin(Vector<String>& paramNames, Vector<String>& paramValues)
{
    AttributeCollection attributes = this->attributes();
    for (const Attribute& attribute : attributes) {
        paramNames.append(attribute.localName().string());
        paramValues.append(attribute.value().string());
    }
}

// FIXME: This should be unified with HTMLObjectElement::updateWidget and
// moved down into HTMLPluginElement.cpp
void HTMLEmbedElement::updateWidgetInternal()
{
    ASSERT(!layoutEmbeddedObject()->showsUnavailablePluginIndicator());
    ASSERT(needsWidgetUpdate());
    setNeedsWidgetUpdate(false);

    if (m_url.isEmpty() && m_serviceType.isEmpty())
        return;

    // Note these pass m_url and m_serviceType to allow better code sharing with
    // <object> which modifies url and serviceType before calling these.
    if (!allowedToLoadFrameURL(m_url))
        return;

    // FIXME: These should be joined into a PluginParameters class.
    Vector<String> paramNames;
    Vector<String> paramValues;
    parametersForPlugin(paramNames, paramValues);

    RefPtrWillBeRawPtr<HTMLEmbedElement> protect(this); // Loading the plugin might remove us from the document.

    // FIXME: Can we not have layoutObject here now that beforeload events are gone?
    if (!layoutObject())
        return;

    requestObject(m_url, m_serviceType, paramNames, paramValues);
}

bool HTMLEmbedElement::layoutObjectIsNeeded(const ComputedStyle& style)
{
    if (isImageType())
        return HTMLPlugInElement::layoutObjectIsNeeded(style);

    // If my parent is an <object> and is not set to use fallback content, I
    // should be ignored and not get a layoutObject.
    ContainerNode* p = parentNode();
    if (isHTMLObjectElement(p)) {
        ASSERT(p->layoutObject());
        if (!toHTMLObjectElement(p)->useFallbackContent()) {
            ASSERT(!p->layoutObject()->isEmbeddedObject());
            return false;
        }
    }
    return HTMLPlugInElement::layoutObjectIsNeeded(style);
}

bool HTMLEmbedElement::isURLAttribute(const Attribute& attribute) const
{
    return attribute.name() == srcAttr || HTMLPlugInElement::isURLAttribute(attribute);
}

const QualifiedName& HTMLEmbedElement::subResourceAttributeName() const
{
    return srcAttr;
}

bool HTMLEmbedElement::isInteractiveContent() const
{
    return true;
}

bool HTMLEmbedElement::isExposed() const
{
    // http://www.whatwg.org/specs/web-apps/current-work/#exposed
    for (HTMLObjectElement* object = Traversal<HTMLObjectElement>::firstAncestor(*this); object; object = Traversal<HTMLObjectElement>::firstAncestor(*object)) {
        if (object->isExposed())
            return false;
    }
    return true;
}

}
