/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2010 Apple Inc. All rights reserved.
 *           (C) 2006 Alexey Proskuryakov (ap@nypop.com)
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
 */

#include "config.h"
#include "core/html/HTMLKeygenElement.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/HTMLNames.h"
#include "core/dom/Document.h"
#include "core/dom/Text.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/frame/UseCounter.h"
#include "core/html/FormDataList.h"
#include "core/html/HTMLOptionElement.h"
#include "core/html/HTMLSelectElement.h"
#include "core/layout/LayoutBlockFlow.h"
#include "platform/text/PlatformLocale.h"
#include "public/platform/Platform.h"
#include "public/platform/WebLocalizedString.h"
#include "wtf/StdLibExtras.h"

using namespace blink;

namespace blink {

using namespace HTMLNames;

HTMLKeygenElement::HTMLKeygenElement(Document& document, HTMLFormElement* form)
    : HTMLFormControlElementWithState(keygenTag, document, form)
{
    UseCounter::count(document, UseCounter::HTMLKeygenElement);
}

PassRefPtrWillBeRawPtr<HTMLKeygenElement> HTMLKeygenElement::create(Document& document, HTMLFormElement* form)
{
    RefPtrWillBeRawPtr<HTMLKeygenElement> keygen = adoptRefWillBeNoop(new HTMLKeygenElement(document, form));
    keygen->ensureUserAgentShadowRoot();
    return keygen.release();
}

LayoutObject* HTMLKeygenElement::createLayoutObject(const ComputedStyle& style)
{
    // TODO(mstensho): While it's harmful and meaningless to allow most display types on replaced
    // content (e.g. table, table-row or flex), it would be useful to honor at least some of
    // them. Table-cell (and maybe table-caption too), for instance. See crbug.com/335040
    return new LayoutBlockFlow(this);
}

void HTMLKeygenElement::didAddUserAgentShadowRoot(ShadowRoot& root)
{
    DEFINE_STATIC_LOCAL(AtomicString, keygenSelectPseudoId, ("-webkit-keygen-select", AtomicString::ConstructFromLiteral));

    Vector<String> keys;
    keys.reserveCapacity(2);
    keys.append(locale().queryString(WebLocalizedString::KeygenMenuHighGradeKeySize));
    keys.append(locale().queryString(WebLocalizedString::KeygenMenuMediumGradeKeySize));

    // Create a select element with one option element for each key size.
    RefPtrWillBeRawPtr<HTMLSelectElement> select = HTMLSelectElement::create(document());
    select->setShadowPseudoId(keygenSelectPseudoId);
    for (const String& key : keys) {
        RefPtrWillBeRawPtr<HTMLOptionElement> option = HTMLOptionElement::create(document());
        option->appendChild(Text::create(document(), key));
        select->appendChild(option);
    }

    root.appendChild(select);
}

void HTMLKeygenElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    // Reflect disabled attribute on the shadow select element
    if (name == disabledAttr)
        shadowSelect()->setAttribute(name, value);

    HTMLFormControlElement::parseAttribute(name, value);
}

bool HTMLKeygenElement::appendFormData(FormDataList& encoding, bool)
{
    // Only RSA is supported at this time.
    const AtomicString& keyType = fastGetAttribute(keytypeAttr);
    if (!keyType.isNull() && !equalIgnoringCase(keyType, "rsa"))
        return false;
    String value = Platform::current()->signedPublicKeyAndChallengeString(shadowSelect()->selectedIndex(), fastGetAttribute(challengeAttr), document().baseURL());
    if (value.isNull())
        return false;
    encoding.appendData(name(), value.utf8());
    return true;
}

const AtomicString& HTMLKeygenElement::formControlType() const
{
    DEFINE_STATIC_LOCAL(const AtomicString, keygen, ("keygen", AtomicString::ConstructFromLiteral));
    return keygen;
}

void HTMLKeygenElement::resetImpl()
{
    shadowSelect()->reset();
}

HTMLSelectElement* HTMLKeygenElement::shadowSelect() const
{
    ShadowRoot* root = userAgentShadowRoot();
    return root ? toHTMLSelectElement(root->firstChild()) : 0;
}

bool HTMLKeygenElement::isInteractiveContent() const
{
    return true;
}

bool HTMLKeygenElement::supportsAutofocus() const
{
    return true;
}

} // namespace
