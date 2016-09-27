/*
 * Copyright (C) 2004, 2005, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
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
#include "core/svg/SVGURIReference.h"

#include "core/XLinkNames.h"
#include "core/svg/SVGElement.h"
#include "platform/weborigin/KURL.h"

namespace blink {

SVGURIReference::SVGURIReference(SVGElement* element)
    : m_href(SVGAnimatedString::create(element, XLinkNames::hrefAttr, SVGString::create()))
{
    ASSERT(element);
    element->addToPropertyMap(m_href);
}

DEFINE_TRACE(SVGURIReference)
{
    visitor->trace(m_href);
}

bool SVGURIReference::isKnownAttribute(const QualifiedName& attrName)
{
    return attrName.matches(XLinkNames::hrefAttr);
}

AtomicString SVGURIReference::fragmentIdentifierFromIRIString(const String& url, const TreeScope& treeScope)
{
    size_t start = url.find('#');
    if (start == kNotFound)
        return emptyAtom;

    const Document& document = treeScope.document();
    KURL base = start ? KURL(document.baseURI(), url.substring(0, start)) : document.baseURI();
    if (equalIgnoringFragmentIdentifier(base, document.url()))
        return AtomicString(url.substring(start + 1));

    return emptyAtom;
}

static inline KURL urlFromIRIStringWithFragmentIdentifier(const String& url, const TreeScope& treeScope, AtomicString& fragmentIdentifier)
{
    size_t startOfFragmentIdentifier = url.find('#');
    if (startOfFragmentIdentifier == kNotFound)
        return KURL();

    const Document& document = treeScope.document();

    // Exclude the '#' character when determining the fragmentIdentifier.
    fragmentIdentifier = AtomicString(url.substring(startOfFragmentIdentifier + 1));
    if (startOfFragmentIdentifier) {
        KURL base(document.baseURI(), url.substring(0, startOfFragmentIdentifier));
        return KURL(base, url.substring(startOfFragmentIdentifier));
    }

    return KURL(document.baseURI(), url.substring(startOfFragmentIdentifier));
}

Element* SVGURIReference::targetElementFromIRIString(const String& iri, const TreeScope& treeScope, AtomicString* fragmentIdentifier, Document* externalDocument)
{
    const Document& document = treeScope.document();

    // If there's no fragment identifier contained within the IRI string, we can't lookup an element.
    AtomicString id;
    KURL url = urlFromIRIStringWithFragmentIdentifier(iri, document, id);
    if (url == KURL())
        return nullptr;

    if (fragmentIdentifier)
        *fragmentIdentifier = id;

    if (id.isEmpty())
        return nullptr;

    if (externalDocument) {
        // Enforce that the referenced url matches the url of the document that we've loaded for it!
        ASSERT(equalIgnoringFragmentIdentifier(url, externalDocument->url()));
        return externalDocument->getElementById(id);
    }

    // Exit early if the referenced url is external, and we have no externalDocument given.
    if (isExternalURIReference(iri, document))
        return nullptr;

    return treeScope.getElementById(id);
}

void SVGURIReference::addSupportedAttributes(HashSet<QualifiedName>& supportedAttributes)
{
    supportedAttributes.add(XLinkNames::hrefAttr);
}

}
