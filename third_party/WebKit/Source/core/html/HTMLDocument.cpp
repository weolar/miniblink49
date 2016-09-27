/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
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
 *
 * Portions are Copyright (C) 2002 Netscape Communications Corporation.
 * Other contributors: David Baron <dbaron@fas.harvard.edu>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Alternatively, the document type parsing portions of this file may be used
 * under the terms of either the Mozilla Public License Version 1.1, found at
 * http://www.mozilla.org/MPL/ (the "MPL") or the GNU General Public
 * License Version 2.0, found at http://www.fsf.org/copyleft/gpl.html
 * (the "GPL"), in which case the provisions of the MPL or the GPL are
 * applicable instead of those above.  If you wish to allow use of your
 * version of this file only under the terms of one of those two
 * licenses (the MPL or the GPL) and not to allow others to use your
 * version of this file under the LGPL, indicate your decision by
 * deleting the provisions above and replace them with the notice and
 * other provisions required by the MPL or the GPL, as the case may be.
 * If you do not delete the provisions above, a recipient may use your
 * version of this file under any of the LGPL, the MPL or the GPL.
 */

#include "config.h"
#include "core/html/HTMLDocument.h"

#include "bindings/core/v8/ScriptController.h"
#include "core/HTMLNames.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLBodyElement.h"
#include "core/page/FocusController.h"
#include "core/page/FrameTree.h"
#include "core/page/Page.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

using namespace HTMLNames;

HTMLDocument::HTMLDocument(const DocumentInit& initializer, DocumentClassFlags extendedDocumentClasses)
    : Document(initializer, HTMLDocumentClass | extendedDocumentClasses)
{
    clearXMLVersion();
    if (isSrcdocDocument() || initializer.importsController()) {
        ASSERT(inNoQuirksMode());
        lockCompatibilityMode();
    }
}

HTMLDocument::~HTMLDocument()
{
}

HTMLBodyElement* HTMLDocument::htmlBodyElement() const
{
    HTMLElement* body = this->body();
    return isHTMLBodyElement(body) ? toHTMLBodyElement(body) : 0;
}

const AtomicString& HTMLDocument::bodyAttributeValue(const QualifiedName& name) const
{
    if (HTMLBodyElement* body = htmlBodyElement())
        return body->fastGetAttribute(name);
    return nullAtom;
}

void HTMLDocument::setBodyAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (HTMLBodyElement* body = htmlBodyElement()) {
        // FIXME: This check is apparently for benchmarks that set the same value repeatedly.
        // It's not clear what benchmarks though, it's also not clear why we don't avoid
        // causing a style recalc when setting the same value to a presentational attribute
        // in the common case.
        if (body->fastGetAttribute(name) != value)
            body->setAttribute(name, value);
    }
}

const AtomicString& HTMLDocument::bgColor() const
{
    return bodyAttributeValue(bgcolorAttr);
}

void HTMLDocument::setBgColor(const AtomicString& value)
{
    setBodyAttribute(bgcolorAttr, value);
}

const AtomicString& HTMLDocument::fgColor() const
{
    return bodyAttributeValue(textAttr);
}

void HTMLDocument::setFgColor(const AtomicString& value)
{
    setBodyAttribute(textAttr, value);
}

const AtomicString& HTMLDocument::alinkColor() const
{
    return bodyAttributeValue(alinkAttr);
}

void HTMLDocument::setAlinkColor(const AtomicString& value)
{
    setBodyAttribute(alinkAttr, value);
}

const AtomicString& HTMLDocument::linkColor() const
{
    return bodyAttributeValue(linkAttr);
}

void HTMLDocument::setLinkColor(const AtomicString& value)
{
    setBodyAttribute(linkAttr, value);
}

const AtomicString& HTMLDocument::vlinkColor() const
{
    return bodyAttributeValue(vlinkAttr);
}

void HTMLDocument::setVlinkColor(const AtomicString& value)
{
    setBodyAttribute(vlinkAttr, value);
}

PassRefPtrWillBeRawPtr<Document> HTMLDocument::cloneDocumentWithoutChildren()
{
    return create(DocumentInit(url()).withRegistrationContext(registrationContext()));
}

// --------------------------------------------------------------------------
// not part of the DOM
// --------------------------------------------------------------------------

void HTMLDocument::addItemToMap(HashCountedSet<AtomicString>& map, const AtomicString& name)
{
    if (name.isEmpty())
        return;
    map.add(name);
    if (LocalFrame* f = frame())
        f->script().namedItemAdded(this, name);
}

void HTMLDocument::removeItemFromMap(HashCountedSet<AtomicString>& map, const AtomicString& name)
{
    if (name.isEmpty())
        return;
    map.remove(name);
    if (LocalFrame* f = frame())
        f->script().namedItemRemoved(this, name);
}

void HTMLDocument::addNamedItem(const AtomicString& name)
{
    addItemToMap(m_namedItemCounts, name);
}

void HTMLDocument::removeNamedItem(const AtomicString& name)
{
    removeItemFromMap(m_namedItemCounts, name);
}

void HTMLDocument::addExtraNamedItem(const AtomicString& name)
{
    addItemToMap(m_extraNamedItemCounts, name);
}

void HTMLDocument::removeExtraNamedItem(const AtomicString& name)
{
    removeItemFromMap(m_extraNamedItemCounts, name);
}

static void addLocalNameToSet(HashSet<StringImpl*>* set, const QualifiedName& qName)
{
    set->add(qName.localName().impl());
}

static HashSet<StringImpl*>* createHtmlCaseInsensitiveAttributesSet()
{
    // This is the list of attributes in HTML 4.01 with values marked as "[CI]" or case-insensitive
    // Mozilla treats all other values as case-sensitive, thus so do we.
    HashSet<StringImpl*>* attrSet = new HashSet<StringImpl*>;

    addLocalNameToSet(attrSet, accept_charsetAttr);
    addLocalNameToSet(attrSet, acceptAttr);
    addLocalNameToSet(attrSet, alignAttr);
    addLocalNameToSet(attrSet, alinkAttr);
    addLocalNameToSet(attrSet, axisAttr);
    addLocalNameToSet(attrSet, bgcolorAttr);
    addLocalNameToSet(attrSet, charsetAttr);
    addLocalNameToSet(attrSet, checkedAttr);
    addLocalNameToSet(attrSet, clearAttr);
    addLocalNameToSet(attrSet, codetypeAttr);
    addLocalNameToSet(attrSet, colorAttr);
    addLocalNameToSet(attrSet, compactAttr);
    addLocalNameToSet(attrSet, declareAttr);
    addLocalNameToSet(attrSet, deferAttr);
    addLocalNameToSet(attrSet, dirAttr);
    addLocalNameToSet(attrSet, disabledAttr);
    addLocalNameToSet(attrSet, enctypeAttr);
    addLocalNameToSet(attrSet, faceAttr);
    addLocalNameToSet(attrSet, frameAttr);
    addLocalNameToSet(attrSet, hreflangAttr);
    addLocalNameToSet(attrSet, http_equivAttr);
    addLocalNameToSet(attrSet, langAttr);
    addLocalNameToSet(attrSet, languageAttr);
    addLocalNameToSet(attrSet, linkAttr);
    addLocalNameToSet(attrSet, mediaAttr);
    addLocalNameToSet(attrSet, methodAttr);
    addLocalNameToSet(attrSet, multipleAttr);
    addLocalNameToSet(attrSet, nohrefAttr);
    addLocalNameToSet(attrSet, noresizeAttr);
    addLocalNameToSet(attrSet, noshadeAttr);
    addLocalNameToSet(attrSet, nowrapAttr);
    addLocalNameToSet(attrSet, readonlyAttr);
    addLocalNameToSet(attrSet, relAttr);
    addLocalNameToSet(attrSet, revAttr);
    addLocalNameToSet(attrSet, rulesAttr);
    addLocalNameToSet(attrSet, scopeAttr);
    addLocalNameToSet(attrSet, scrollingAttr);
    addLocalNameToSet(attrSet, selectedAttr);
    addLocalNameToSet(attrSet, shapeAttr);
    addLocalNameToSet(attrSet, targetAttr);
    addLocalNameToSet(attrSet, textAttr);
    addLocalNameToSet(attrSet, typeAttr);
    addLocalNameToSet(attrSet, valignAttr);
    addLocalNameToSet(attrSet, valuetypeAttr);
    addLocalNameToSet(attrSet, vlinkAttr);

    return attrSet;
}

bool HTMLDocument::isCaseSensitiveAttribute(const QualifiedName& attributeName)
{
    static HashSet<StringImpl*>* htmlCaseInsensitiveAttributesSet = createHtmlCaseInsensitiveAttributesSet();
    bool isPossibleHTMLAttr = !attributeName.hasPrefix() && (attributeName.namespaceURI() == nullAtom);
    return !isPossibleHTMLAttr || !htmlCaseInsensitiveAttributesSet->contains(attributeName.localName().impl());
}

}
