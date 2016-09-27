// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/WindowNameCollection.h"

#include "core/html/HTMLImageElement.h"

namespace blink {

WindowNameCollection::WindowNameCollection(ContainerNode& document, const AtomicString& name)
    : HTMLNameCollection(document, WindowNamedItems, name)
{
}

bool WindowNameCollection::elementMatches(const Element& element) const
{
    // Match only images, forms, applets, embeds and objects by name,
    // but anything by id
    if (isHTMLImageElement(element)
        || isHTMLFormElement(element)
        || isHTMLAppletElement(element)
        || isHTMLEmbedElement(element)
        || isHTMLObjectElement(element)) {
        if (element.getNameAttribute() == m_name)
            return true;
    }
    return element.getIdAttribute() == m_name;
}

} // namespace blink
