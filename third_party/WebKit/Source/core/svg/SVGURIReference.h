/*
 * Copyright (C) 2004, 2005, 2008, 2009 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005 Rob Buis <buis@kde.org>
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

#ifndef SVGURIReference_h
#define SVGURIReference_h

#include "core/CoreExport.h"
#include "core/dom/Document.h"
#include "core/svg/SVGAnimatedString.h"
#include "platform/heap/Handle.h"

namespace blink {

class Element;

class CORE_EXPORT SVGURIReference : public WillBeGarbageCollectedMixin {
public:
    virtual ~SVGURIReference() { }

    bool isKnownAttribute(const QualifiedName&);
    void addSupportedAttributes(HashSet<QualifiedName>&);

    static AtomicString fragmentIdentifierFromIRIString(const String&, const TreeScope&);
    static Element* targetElementFromIRIString(const String&, const TreeScope&, AtomicString* = 0, Document* = 0);

    static inline bool isExternalURIReference(const String& uri, const Document& document)
    {
        // Fragment-only URIs are always internal
        if (uri.startsWith('#'))
            return false;

        // If the URI matches our documents URL, we're dealing with a local reference.
        KURL url = document.completeURL(uri);
        return !equalIgnoringFragmentIdentifier(url, document.url());
    }

    const String& hrefString() const { return m_href->currentValue()->value(); }

    // JS API
    SVGAnimatedString* href() const { return m_href.get(); }

    DECLARE_VIRTUAL_TRACE();

protected:
    explicit SVGURIReference(SVGElement*);

private:
    RefPtrWillBeMember<SVGAnimatedString> m_href;
};

} // namespace blink

#endif // SVGURIReference_h
