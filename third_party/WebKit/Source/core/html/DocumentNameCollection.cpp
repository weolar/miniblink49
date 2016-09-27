// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/DocumentNameCollection.h"

#include "core/html/HTMLEmbedElement.h"
#include "core/html/HTMLObjectElement.h"

namespace blink {

DocumentNameCollection::DocumentNameCollection(ContainerNode& document, const AtomicString& name)
    : HTMLNameCollection(document, DocumentNamedItems, name)
{
}

bool DocumentNameCollection::elementMatches(const HTMLElement& element) const
{
    // Match images, forms, applets, embeds, objects and iframes by name,
    // applets and object by id, and images by id but only if they have
    // a name attribute (this very strange rule matches IE)
    if (isHTMLFormElement(element) || isHTMLIFrameElement(element) || (isHTMLEmbedElement(element) && toHTMLEmbedElement(element).isExposed()))
        return element.getNameAttribute() == m_name;
    if (isHTMLAppletElement(element) || (isHTMLObjectElement(element) && toHTMLObjectElement(element).isExposed()))
        return element.getNameAttribute() == m_name || element.getIdAttribute() == m_name;
    if (isHTMLImageElement(element))
        return element.getNameAttribute() == m_name || (element.getIdAttribute() == m_name && element.hasName());
    return false;
}

} // namespace blink
