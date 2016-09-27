/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2010 Apple Inc. All rights reserved.
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
#include "core/html/HTMLOptGroupElement.h"

#include "core/HTMLNames.h"
#include "core/dom/NodeComputedStyle.h"
#include "core/dom/Text.h"
#include "core/editing/htmlediting.h"
#include "core/html/HTMLContentElement.h"
#include "core/html/HTMLDivElement.h"
#include "core/html/HTMLSelectElement.h"
#include "core/html/shadow/ShadowElementNames.h"
#include "wtf/StdLibExtras.h"
#include "wtf/text/CharacterNames.h"

namespace blink {

using namespace HTMLNames;

inline HTMLOptGroupElement::HTMLOptGroupElement(Document& document)
    : HTMLElement(optgroupTag, document)
{
    setHasCustomStyleCallbacks();
}

PassRefPtrWillBeRawPtr<HTMLOptGroupElement> HTMLOptGroupElement::create(Document& document)
{
    RefPtrWillBeRawPtr<HTMLOptGroupElement> optGroupElement = adoptRefWillBeNoop(new HTMLOptGroupElement(document));
    optGroupElement->ensureUserAgentShadowRoot();
    return optGroupElement.release();
}

bool HTMLOptGroupElement::isDisabledFormControl() const
{
    return fastHasAttribute(disabledAttr);
}

void HTMLOptGroupElement::childrenChanged(const ChildrenChange& change)
{
    recalcSelectOptions();
    HTMLElement::childrenChanged(change);
}

void HTMLOptGroupElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    HTMLElement::parseAttribute(name, value);
    recalcSelectOptions();

    if (name == disabledAttr) {
        pseudoStateChanged(CSSSelector::PseudoDisabled);
        pseudoStateChanged(CSSSelector::PseudoEnabled);
    } else if (name == labelAttr) {
        updateGroupLabel();
    }
}

void HTMLOptGroupElement::recalcSelectOptions()
{
    if (HTMLSelectElement* select = Traversal<HTMLSelectElement>::firstAncestor(*this))
        select->setRecalcListItems();
}

void HTMLOptGroupElement::attach(const AttachContext& context)
{
    if (context.resolvedStyle) {
        ASSERT(!m_style || m_style == context.resolvedStyle);
        m_style = context.resolvedStyle;
    }
    HTMLElement::attach(context);
}

void HTMLOptGroupElement::detach(const AttachContext& context)
{
    m_style.clear();
    HTMLElement::detach(context);
}

bool HTMLOptGroupElement::supportsFocus() const
{
    RefPtrWillBeRawPtr<HTMLSelectElement> select = ownerSelectElement();
    if (select && select->usesMenuList())
        return false;
    return HTMLElement::supportsFocus();
}

void HTMLOptGroupElement::updateNonComputedStyle()
{
    m_style = originalStyleForLayoutObject();
    if (layoutObject()) {
        if (HTMLSelectElement* select = ownerSelectElement())
            select->updateListOnLayoutObject();
    }
}

ComputedStyle* HTMLOptGroupElement::nonLayoutObjectComputedStyle() const
{
    return m_style.get();
}

PassRefPtr<ComputedStyle> HTMLOptGroupElement::customStyleForLayoutObject()
{
    updateNonComputedStyle();
    return m_style;
}

String HTMLOptGroupElement::groupLabelText() const
{
    String itemText = getAttribute(labelAttr);

    // In WinIE, leading and trailing whitespace is ignored in options and optgroups. We match this behavior.
    itemText = itemText.stripWhiteSpace();
    // We want to collapse our whitespace too.  This will match other browsers.
    itemText = itemText.simplifyWhiteSpace();

    return itemText;
}

HTMLSelectElement* HTMLOptGroupElement::ownerSelectElement() const
{
    return Traversal<HTMLSelectElement>::firstAncestor(*this);
}

void HTMLOptGroupElement::accessKeyAction(bool)
{
    HTMLSelectElement* select = ownerSelectElement();
    // send to the parent to bring focus to the list box
    if (select && !select->focused())
        select->accessKeyAction(false);
}

void HTMLOptGroupElement::didAddUserAgentShadowRoot(ShadowRoot& root)
{
    DEFINE_STATIC_LOCAL(AtomicString, labelPadding, ("0 2px 1px 2px", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(AtomicString, labelMinHeight, ("1.2em", AtomicString::ConstructFromLiteral));
    RefPtrWillBeRawPtr<HTMLDivElement> label = HTMLDivElement::create(document());
    label->setAttribute(roleAttr, AtomicString("group", AtomicString::ConstructFromLiteral));
    label->setAttribute(aria_labelAttr, AtomicString());
    label->setInlineStyleProperty(CSSPropertyPadding, labelPadding);
    label->setInlineStyleProperty(CSSPropertyMinHeight, labelMinHeight);
    label->setIdAttribute(ShadowElementNames::optGroupLabel());
    root.appendChild(label);

    RefPtrWillBeRawPtr<HTMLContentElement> content = HTMLContentElement::create(document());
    content->setAttribute(selectAttr, "option,optgroup,hr");
    root.appendChild(content);
}

void HTMLOptGroupElement::updateGroupLabel()
{
    const String& labelText = groupLabelText();
    HTMLDivElement& label = optGroupLabelElement();
    label.setTextContent(labelText);
    label.setAttribute(aria_labelAttr, AtomicString(labelText));
}

HTMLDivElement& HTMLOptGroupElement::optGroupLabelElement() const
{
    return *toHTMLDivElement(userAgentShadowRoot()->getElementById(ShadowElementNames::optGroupLabel()));
}

} // namespace
