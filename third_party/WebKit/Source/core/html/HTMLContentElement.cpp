/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/HTMLContentElement.h"

#include "core/HTMLNames.h"
#include "core/css/SelectorChecker.h"
#include "core/css/parser/CSSParser.h"
#include "core/dom/QualifiedName.h"
#include "core/dom/shadow/ElementShadow.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace blink {

using namespace HTMLNames;

PassRefPtrWillBeRawPtr<HTMLContentElement> HTMLContentElement::create(Document& document, PassOwnPtrWillBeRawPtr<HTMLContentSelectFilter> filter)
{
    return adoptRefWillBeNoop(new HTMLContentElement(document, filter));
}

inline HTMLContentElement::HTMLContentElement(Document& document, PassOwnPtrWillBeRawPtr<HTMLContentSelectFilter> filter)
    : InsertionPoint(contentTag, document)
    , m_shouldParseSelect(false)
    , m_isValidSelector(true)
    , m_filter(filter)
{
}

HTMLContentElement::~HTMLContentElement()
{
}

DEFINE_TRACE(HTMLContentElement)
{
    visitor->trace(m_filter);
    InsertionPoint::trace(visitor);
}

void HTMLContentElement::parseSelect()
{
    ASSERT(m_shouldParseSelect);

    CSSParser::parseSelector(CSSParserContext(document(), 0), m_select, m_selectorList);
    m_shouldParseSelect = false;
    m_isValidSelector = validateSelect();
    if (!m_isValidSelector) {
        CSSSelectorList emptyList;
        m_selectorList.adopt(emptyList);
    }
}

void HTMLContentElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == selectAttr) {
        if (ShadowRoot* root = containingShadowRoot())
            root->owner()->willAffectSelector();
        m_shouldParseSelect = true;
        m_select = value;
    } else {
        InsertionPoint::parseAttribute(name, value);
    }
}

static inline bool includesDisallowedPseudoClass(const CSSSelector& selector)
{
    if (selector.pseudoType() == CSSSelector::PseudoNot) {
        const CSSSelector* subSelector = selector.selectorList()->first();
        return subSelector->match() == CSSSelector::PseudoClass;
    }
    return selector.match() == CSSSelector::PseudoClass;
}

bool HTMLContentElement::validateSelect() const
{
    ASSERT(!m_shouldParseSelect);

    if (m_select.isNull() || m_select.isEmpty())
        return true;

    if (!m_selectorList.isValid())
        return false;

    for (const CSSSelector* selector = m_selectorList.first(); selector; selector = m_selectorList.next(*selector)) {
        if (!selector->isCompound())
            return false;
        for (const CSSSelector* subSelector = selector; subSelector; subSelector = subSelector->tagHistory()) {
            if (includesDisallowedPseudoClass(*subSelector))
                return false;
        }
    }
    return true;
}

// TODO(esprehn): element should really be const, but matching a selector is not
// const for some SelectorCheckingModes (mainly ResolvingStyle) where it sets
// dynamic restyle flags on elements.
bool HTMLContentElement::matchSelector(Element& element) const
{
    SelectorChecker selectorChecker(SelectorChecker::QueryingRules);
    SelectorChecker::SelectorCheckingContext context(&element, SelectorChecker::VisitedMatchDisabled);
    for (const CSSSelector* selector = selectorList().first(); selector; selector = CSSSelectorList::next(*selector)) {
        context.selector = selector;
        if (selectorChecker.match(context))
            return true;
    }
    return false;
}

}
