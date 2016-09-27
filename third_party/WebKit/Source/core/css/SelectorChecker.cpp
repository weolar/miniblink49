/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 2004-2005 Allan Sandfeld Jensen (kde@carewolf.com)
 * Copyright (C) 2006, 2007 Nicholas Shanks (webkit@nickshanks.com)
 * Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012, 2013 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Alexey Proskuryakov <ap@webkit.org>
 * Copyright (C) 2007, 2008 Eric Seidel <eric@webkit.org>
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 * Copyright (C) Research In Motion Limited 2011. All rights reserved.
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
#include "core/css/SelectorChecker.h"

#include "core/HTMLNames.h"
#include "core/css/CSSSelectorList.h"
#include "core/dom/Document.h"
#include "core/dom/Element.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/Fullscreen.h"
#include "core/dom/NodeComputedStyle.h"
#include "core/dom/NthIndexCache.h"
#include "core/dom/StyleEngine.h"
#include "core/dom/Text.h"
#include "core/dom/shadow/ComposedTreeTraversal.h"
#include "core/dom/shadow/InsertionPoint.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/FrameSelection.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLDocument.h"
#include "core/html/HTMLFrameElementBase.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLOptionElement.h"
#include "core/html/HTMLSelectElement.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/html/track/vtt/VTTElement.h"
#include "core/inspector/InspectorInstrumentation.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/LayoutScrollbar.h"
#include "core/page/FocusController.h"
#include "core/page/Page.h"
#include "core/style/ComputedStyle.h"
#include "platform/scroll/ScrollableArea.h"
#include "platform/scroll/ScrollbarTheme.h"

namespace blink {

using namespace HTMLNames;

SelectorChecker::SelectorChecker(Mode mode)
    : m_mode(mode)
{
}

static bool isFrameFocused(const Element& element)
{
    return element.document().frame() && element.document().frame()->selection().isFocusedAndActive();
}

static bool matchesSpatialNavigationFocusPseudoClass(const Element& element)
{
    return isHTMLOptionElement(element) && toHTMLOptionElement(element).spatialNavigationFocused() && isFrameFocused(element);
}

static bool matchesListBoxPseudoClass(const Element& element)
{
    return isHTMLSelectElement(element) && !toHTMLSelectElement(element).usesMenuList();
}

static bool matchesTagName(const Element& element, const QualifiedName& tagQName)
{
    if (tagQName == anyQName())
        return true;
    const AtomicString& localName = tagQName.localName();
    if (localName != starAtom && localName != element.localName()) {
        if (element.isHTMLElement() || !element.document().isHTMLDocument())
            return false;
        // Non-html elements in html documents are normalized to their camel-cased
        // version during parsing if applicable. Yet, type selectors are lower-cased
        // for selectors in html documents. Compare the upper case converted names
        // instead to allow matching SVG elements like foreignObject.
        if (element.tagQName().localNameUpper() != tagQName.localNameUpper())
            return false;
    }
    const AtomicString& namespaceURI = tagQName.namespaceURI();
    return namespaceURI == starAtom || namespaceURI == element.namespaceURI();
}

static Element* parentElement(const SelectorChecker::SelectorCheckingContext& context)
{
    // - If context.scope is a shadow root, we should walk up to its shadow host.
    // - If context.scope is some element in some shadow tree and querySelector initialized the context,
    //   e.g. shadowRoot.querySelector(':host *'),
    //   (a) context.element has the same treescope as context.scope, need to walk up to its shadow host.
    //   (b) Otherwise, should not walk up from a shadow root to a shadow host.
    if (context.scope && (context.scope == context.element->containingShadowRoot() || context.scope->treeScope() == context.element->treeScope()))
        return context.element->parentOrShadowHostElement();
    return context.element->parentElement();
}

static bool scopeContainsLastMatchedElement(const SelectorChecker::SelectorCheckingContext& context)
{
    if (!context.scopeContainsLastMatchedElement)
        return true;

    ASSERT(context.scope);
    if (context.scope->treeScope() == context.element->treeScope())
        return true;

    // Because Blink treats a shadow host's TreeScope as a separate one from its descendent shadow roots,
    // if the last matched element is a shadow host, the condition above isn't met, even though it
    // should be.
    return context.element == context.scope->shadowHost() && (!context.previousElement || context.previousElement->isInDescendantTreeOf(context.element));
}

static inline bool nextSelectorExceedsScope(const SelectorChecker::SelectorCheckingContext& context)
{
    if (context.scope && context.scope->isInShadowTree())
        return context.element == context.scope->shadowHost();

    return false;
}

static bool shouldMatchHoverOrActive(const SelectorChecker::SelectorCheckingContext& context)
{
    // If we're in quirks mode, then :hover and :active should never match anchors with no
    // href and *:hover and *:active should not match anything. This is specified in
    // https://quirks.spec.whatwg.org/#the-:active-and-:hover-quirk
    if (!context.element->document().inQuirksMode())
        return true;
    if (context.isSubSelector)
        return true;
    if (context.selector->relation() == CSSSelector::SubSelector && context.selector->tagHistory())
        return true;
    return context.element->isLink();
}

static bool isFirstChild(Element& element)
{
    return !ElementTraversal::previousSibling(element);
}

static bool isLastChild(Element& element)
{
    return !ElementTraversal::nextSibling(element);
}

static bool isFirstOfType(Element& element, const QualifiedName& type)
{
    return !ElementTraversal::previousSibling(element, HasTagName(type));
}

static bool isLastOfType(Element& element, const QualifiedName& type)
{
    return !ElementTraversal::nextSibling(element, HasTagName(type));
}

static int nthChildIndex(Element& element)
{
    if (NthIndexCache* nthIndexCache = element.document().nthIndexCache())
        return nthIndexCache->nthChildIndex(element);

    int index = 1;
    for (const Element* sibling = ElementTraversal::previousSibling(element); sibling; sibling = ElementTraversal::previousSibling(*sibling))
        index++;

    return index;
}

static int nthOfTypeIndex(Element& element, const QualifiedName& type)
{
    if (NthIndexCache* nthIndexCache = element.document().nthIndexCache())
        return nthIndexCache->nthChildIndexOfType(element, type);
    int index = 1;
    for (const Element* sibling = ElementTraversal::previousSibling(element, HasTagName(type)); sibling; sibling = ElementTraversal::previousSibling(*sibling, HasTagName(type)))
        ++index;
    return index;
}

static int nthLastChildIndex(Element& element)
{
    if (NthIndexCache* nthIndexCache = element.document().nthIndexCache())
        return nthIndexCache->nthLastChildIndex(element);

    int index = 1;
    for (const Element* sibling = ElementTraversal::nextSibling(element); sibling; sibling = ElementTraversal::nextSibling(*sibling))
        ++index;
    return index;
}

static int nthLastOfTypeIndex(Element& element, const QualifiedName& type)
{
    if (NthIndexCache* nthIndexCache = element.document().nthIndexCache())
        return nthIndexCache->nthLastChildIndexOfType(element, type);

    int index = 1;
    for (const Element* sibling = ElementTraversal::nextSibling(element, HasTagName(type)); sibling; sibling = ElementTraversal::nextSibling(*sibling, HasTagName(type)))
        ++index;
    return index;
}

bool SelectorChecker::match(const SelectorCheckingContext& context, MatchResult& result) const
{
    ASSERT(context.selector);
    return matchSelector(context, result) == SelectorMatches;
}

bool SelectorChecker::match(const SelectorCheckingContext& context) const
{
    MatchResult ignoreResult;
    return match(context, ignoreResult);
}

// Recursive check of selectors and combinators
// It can return 4 different values:
// * SelectorMatches          - the selector matches the element e
// * SelectorFailsLocally     - the selector fails for the element e
// * SelectorFailsAllSiblings - the selector fails for e and any sibling of e
// * SelectorFailsCompletely  - the selector fails for e and any sibling or ancestor of e
SelectorChecker::Match SelectorChecker::matchSelector(const SelectorCheckingContext& context, MatchResult& result) const
{
    MatchResult subResult;
    if (!checkOne(context, subResult))
        return SelectorFailsLocally;

    if (subResult.dynamicPseudo != NOPSEUDO)
        result.dynamicPseudo = subResult.dynamicPseudo;

    if (context.selector->isLastInTagHistory()) {
        if (scopeContainsLastMatchedElement(context)) {
            result.specificity += subResult.specificity;
            return SelectorMatches;
        }
        return SelectorFailsLocally;
    }

    Match match;
    if (context.selector->relation() != CSSSelector::SubSelector) {
        if (nextSelectorExceedsScope(context))
            return SelectorFailsCompletely;

        if (context.pseudoId != NOPSEUDO && context.pseudoId != result.dynamicPseudo)
            return SelectorFailsCompletely;

        TemporaryChange<PseudoId> dynamicPseudoScope(result.dynamicPseudo, NOPSEUDO);
        match = matchForRelation(context, result);
    } else {
        match = matchForSubSelector(context, result);
    }
    if (match == SelectorMatches)
        result.specificity += subResult.specificity;
    return match;
}

static inline SelectorChecker::SelectorCheckingContext prepareNextContextForRelation(const SelectorChecker::SelectorCheckingContext& context)
{
    SelectorChecker::SelectorCheckingContext nextContext(context);
    ASSERT(context.selector->tagHistory());
    nextContext.selector = context.selector->tagHistory();
    return nextContext;
}

SelectorChecker::Match SelectorChecker::matchForSubSelector(const SelectorCheckingContext& context, MatchResult& result) const
{
    SelectorCheckingContext nextContext = prepareNextContextForRelation(context);

    PseudoId dynamicPseudo = result.dynamicPseudo;
    // a selector is invalid if something follows a pseudo-element
    // We make an exception for scrollbar pseudo elements and allow a set of pseudo classes (but nothing else)
    // to follow the pseudo elements.
    nextContext.hasScrollbarPseudo = dynamicPseudo != NOPSEUDO && (context.scrollbar || dynamicPseudo == SCROLLBAR_CORNER || dynamicPseudo == RESIZER);
    nextContext.hasSelectionPseudo = dynamicPseudo == SELECTION;
    if ((context.inRightmostCompound || m_mode == CollectingCSSRules || m_mode == CollectingStyleRules || m_mode == QueryingRules) && dynamicPseudo != NOPSEUDO
        && !nextContext.hasSelectionPseudo
        && !(nextContext.hasScrollbarPseudo && nextContext.selector->match() == CSSSelector::PseudoClass))
        return SelectorFailsCompletely;

    nextContext.isSubSelector = true;
    return matchSelector(nextContext, result);
}

static inline bool isOpenShadowRoot(const Node* node)
{
    return node && node->isShadowRoot() && toShadowRoot(node)->type() == ShadowRootType::Open;
}

SelectorChecker::Match SelectorChecker::matchForPseudoShadow(const SelectorCheckingContext& context, const ContainerNode* node, MatchResult& result) const
{
    if (!isOpenShadowRoot(node))
        return SelectorFailsCompletely;
    if (!context.previousElement)
        return SelectorFailsCompletely;
    return matchSelector(context, result);
}

SelectorChecker::Match SelectorChecker::matchForRelation(const SelectorCheckingContext& context, MatchResult& result) const
{
    SelectorCheckingContext nextContext = prepareNextContextForRelation(context);
    nextContext.previousElement = context.element;

    CSSSelector::Relation relation = context.selector->relation();

    // Disable :visited matching when we see the first link or try to match anything else than an ancestors.
    if (!context.isSubSelector && (context.element->isLink() || (relation != CSSSelector::Descendant && relation != CSSSelector::Child)))
        nextContext.visitedMatchType = VisitedMatchDisabled;

    nextContext.pseudoId = NOPSEUDO;

    switch (relation) {
    case CSSSelector::Descendant:
        if (context.selector->relationIsAffectedByPseudoContent()) {
            for (Element* element = context.element; element; element = element->parentElement()) {
                if (matchForShadowDistributed(nextContext, *element, result) == SelectorMatches)
                    return SelectorMatches;
            }
            return SelectorFailsCompletely;
        }
        nextContext.isSubSelector = false;
        nextContext.inRightmostCompound = false;

        if (nextContext.selector->pseudoType() == CSSSelector::PseudoShadow)
            return matchForPseudoShadow(nextContext, context.element->containingShadowRoot(), result);

        for (nextContext.element = parentElement(context); nextContext.element; nextContext.element = parentElement(nextContext)) {
            Match match = matchSelector(nextContext, result);
            if (match == SelectorMatches || match == SelectorFailsCompletely)
                return match;
            if (nextSelectorExceedsScope(nextContext))
                return SelectorFailsCompletely;
        }
        return SelectorFailsCompletely;
    case CSSSelector::Child:
        {
            if (context.selector->relationIsAffectedByPseudoContent())
                return matchForShadowDistributed(nextContext, *context.element, result);

            nextContext.isSubSelector = false;
            nextContext.inRightmostCompound = false;

            if (nextContext.selector->pseudoType() == CSSSelector::PseudoShadow)
                return matchForPseudoShadow(nextContext, context.element->parentNode(), result);

            nextContext.element = parentElement(context);
            if (!nextContext.element)
                return SelectorFailsCompletely;
            return matchSelector(nextContext, result);
        }
    case CSSSelector::DirectAdjacent:
        // Shadow roots can't have sibling elements
        if (nextContext.selector->pseudoType() == CSSSelector::PseudoShadow)
            return SelectorFailsCompletely;

        if (m_mode == ResolvingStyle) {
            if (ContainerNode* parent = context.element->parentElementOrShadowRoot())
                parent->setChildrenAffectedByDirectAdjacentRules();
        }
        nextContext.element = ElementTraversal::previousSibling(*context.element);
        if (!nextContext.element)
            return SelectorFailsAllSiblings;
        nextContext.isSubSelector = false;
        nextContext.inRightmostCompound = false;
        return matchSelector(nextContext, result);

    case CSSSelector::IndirectAdjacent:
        // Shadow roots can't have sibling elements
        if (nextContext.selector->pseudoType() == CSSSelector::PseudoShadow)
            return SelectorFailsCompletely;

        if (m_mode == ResolvingStyle) {
            if (ContainerNode* parent = context.element->parentElementOrShadowRoot())
                parent->setChildrenAffectedByIndirectAdjacentRules();
        }
        nextContext.element = ElementTraversal::previousSibling(*context.element);
        nextContext.isSubSelector = false;
        nextContext.inRightmostCompound = false;
        for (; nextContext.element; nextContext.element = ElementTraversal::previousSibling(*nextContext.element)) {
            Match match = matchSelector(nextContext, result);
            if (match == SelectorMatches || match == SelectorFailsAllSiblings || match == SelectorFailsCompletely)
                return match;
        }
        return SelectorFailsAllSiblings;

    case CSSSelector::ShadowPseudo:
        {
            if (!context.isUARule && context.selector->pseudoType() == CSSSelector::PseudoShadow)
                UseCounter::countDeprecation(context.element->document(), UseCounter::CSSSelectorPseudoShadow);
            // If we're in the same tree-scope as the scoping element, then following a shadow descendant combinator would escape that and thus the scope.
            if (context.scope && context.scope->shadowHost() && context.scope->shadowHost()->treeScope() == context.element->treeScope())
                return SelectorFailsCompletely;

            Element* shadowHost = context.element->shadowHost();
            if (!shadowHost)
                return SelectorFailsCompletely;
            nextContext.element = shadowHost;
            nextContext.isSubSelector = false;
            nextContext.inRightmostCompound = false;
            return matchSelector(nextContext, result);
        }

    case CSSSelector::ShadowDeep:
        {
            if (!context.isUARule)
                UseCounter::countDeprecation(context.element->document(), UseCounter::CSSDeepCombinator);
            if (ShadowRoot* root = context.element->containingShadowRoot()) {
                if (root->type() == ShadowRootType::UserAgent)
                    return SelectorFailsCompletely;
            }

            if (context.selector->relationIsAffectedByPseudoContent()) {
                for (Element* element = context.element; element; element = element->parentOrShadowHostElement()) {
                    if (matchForShadowDistributed(nextContext, *element, result) == SelectorMatches)
                        return SelectorMatches;
                }
                return SelectorFailsCompletely;
            }

            nextContext.isSubSelector = false;
            nextContext.inRightmostCompound = false;
            for (nextContext.element = context.element->parentOrShadowHostElement(); nextContext.element; nextContext.element = nextContext.element->parentOrShadowHostElement()) {
                Match match = matchSelector(nextContext, result);
                if (match == SelectorMatches || match == SelectorFailsCompletely)
                    return match;
                if (nextSelectorExceedsScope(nextContext))
                    return SelectorFailsCompletely;
            }
            return SelectorFailsCompletely;
        }

    case CSSSelector::SubSelector:
        ASSERT_NOT_REACHED();
    }

    ASSERT_NOT_REACHED();
    return SelectorFailsCompletely;
}

SelectorChecker::Match SelectorChecker::matchForShadowDistributed(const SelectorCheckingContext& context, const Element& element, MatchResult& result) const
{
    WillBeHeapVector<RawPtrWillBeMember<InsertionPoint>, 8> insertionPoints;
    collectDestinationInsertionPoints(element, insertionPoints);
    SelectorCheckingContext nextContext(context);
    nextContext.isSubSelector = false;
    nextContext.inRightmostCompound = false;
    for (const auto& insertionPoint : insertionPoints) {
        nextContext.element = insertionPoint;
        // TODO(esprehn): Why does SharingRules have a special case?
        if (m_mode == SharingRules)
            nextContext.scope = insertionPoint->containingShadowRoot();
        if (match(nextContext, result))
            return SelectorMatches;
    }
    return SelectorFailsLocally;
}

template<typename CharType>
static inline bool containsHTMLSpaceTemplate(const CharType* string, unsigned length)
{
    for (unsigned i = 0; i < length; ++i) {
        if (isHTMLSpace<CharType>(string[i]))
            return true;
    }
    return false;
}

static inline bool containsHTMLSpace(const AtomicString& string)
{
    if (LIKELY(string.is8Bit()))
        return containsHTMLSpaceTemplate<LChar>(string.characters8(), string.length());
    return containsHTMLSpaceTemplate<UChar>(string.characters16(), string.length());
}

static bool attributeValueMatches(const Attribute& attributeItem, CSSSelector::Match match, const AtomicString& selectorValue, TextCaseSensitivity caseSensitivity)
{
    // TODO(esprehn): How do we get here with a null value?
    const AtomicString& value = attributeItem.value();
    if (value.isNull())
        return false;

    switch (match) {
    case CSSSelector::AttributeExact:
        if (caseSensitivity == TextCaseSensitive)
            return selectorValue == value;
        return equalIgnoringCase(selectorValue, value);
    case CSSSelector::AttributeSet:
        return true;
    case CSSSelector::AttributeList:
        {
            // Ignore empty selectors or selectors containing HTML spaces
            if (selectorValue.isEmpty() || containsHTMLSpace(selectorValue))
                return false;

            unsigned startSearchAt = 0;
            while (true) {
                size_t foundPos = value.find(selectorValue, startSearchAt, caseSensitivity);
                if (foundPos == kNotFound)
                    return false;
                if (!foundPos || isHTMLSpace<UChar>(value[foundPos - 1])) {
                    unsigned endStr = foundPos + selectorValue.length();
                    if (endStr == value.length() || isHTMLSpace<UChar>(value[endStr]))
                        break; // We found a match.
                }

                // No match. Keep looking.
                startSearchAt = foundPos + 1;
            }
            return true;
        }
    case CSSSelector::AttributeContain:
        if (selectorValue.isEmpty())
            return false;
        return value.contains(selectorValue, caseSensitivity);
    case CSSSelector::AttributeBegin:
        if (selectorValue.isEmpty())
            return false;
        return value.startsWith(selectorValue, caseSensitivity);
    case CSSSelector::AttributeEnd:
        if (selectorValue.isEmpty())
            return false;
        return value.endsWith(selectorValue, caseSensitivity);
    case CSSSelector::AttributeHyphen:
        if (value.length() < selectorValue.length())
            return false;
        if (!value.startsWith(selectorValue, caseSensitivity))
            return false;
        // It they start the same, check for exact match or following '-':
        if (value.length() != selectorValue.length() && value[selectorValue.length()] != '-')
            return false;
        return true;
    default:
        break;
    }

    ASSERT_NOT_REACHED();
    return true;
}

static bool anyAttributeMatches(Element& element, CSSSelector::Match match, const CSSSelector& selector)
{
    const QualifiedName& selectorAttr = selector.attribute();
    ASSERT(selectorAttr.localName() != starAtom); // Should not be possible from the CSS grammar.

    // Synchronize the attribute in case it is lazy-computed.
    // Currently all lazy properties have a null namespace, so only pass localName().
    element.synchronizeAttribute(selectorAttr.localName());

    const AtomicString& selectorValue = selector.value();
    TextCaseSensitivity caseSensitivity = (selector.attributeMatchType() == CSSSelector::CaseInsensitive) ? TextCaseInsensitive : TextCaseSensitive;

    AttributeCollection attributes = element.attributesWithoutUpdate();
    for (const auto& attributeItem: attributes) {
        if (!attributeItem.matches(selectorAttr))
            continue;

        if (attributeValueMatches(attributeItem, match, selectorValue, caseSensitivity))
            return true;

        if (caseSensitivity == TextCaseInsensitive) {
            if (selectorAttr.namespaceURI() != starAtom)
                return false;
            continue;
        }

        // Legacy dictates that values of some attributes should be compared in
        // a case-insensitive manner regardless of whether the case insensitive
        // flag is set or not.
        bool legacyCaseInsensitive = element.document().isHTMLDocument() && !HTMLDocument::isCaseSensitiveAttribute(selectorAttr);

        // If case-insensitive, re-check, and count if result differs.
        // See http://code.google.com/p/chromium/issues/detail?id=327060
        if (legacyCaseInsensitive && attributeValueMatches(attributeItem, match, selectorValue, TextCaseInsensitive)) {
            UseCounter::count(element.document(), UseCounter::CaseInsensitiveAttrSelectorMatch);
            return true;
        }
        if (selectorAttr.namespaceURI() != starAtom)
            return false;
    }

    return false;
}

bool SelectorChecker::checkOne(const SelectorCheckingContext& context, MatchResult& result) const
{
    ASSERT(context.element);
    Element& element = *context.element;
    ASSERT(context.selector);
    const CSSSelector& selector = *context.selector;

    // Only :host and :host-context() should match the host: http://drafts.csswg.org/css-scoping/#host-element
    if (context.scope && context.scope->shadowHost() == element && (!selector.isHostPseudoClass()
        && !context.treatShadowHostAsNormalScope
        && selector.match() != CSSSelector::PseudoElement))
            return false;

    switch (selector.match()) {
    case CSSSelector::Tag:
        return matchesTagName(element, selector.tagQName());
    case CSSSelector::Class:
        return element.hasClass() && element.classNames().contains(selector.value());
    case CSSSelector::Id:
        return element.hasID() && element.idForStyleResolution() == selector.value();

    // Attribute selectors
    case CSSSelector::AttributeExact:
    case CSSSelector::AttributeSet:
    case CSSSelector::AttributeHyphen:
    case CSSSelector::AttributeList:
    case CSSSelector::AttributeContain:
    case CSSSelector::AttributeBegin:
    case CSSSelector::AttributeEnd:
        return anyAttributeMatches(element, selector.match(), selector);

    case CSSSelector::PseudoClass:
        return checkPseudoClass(context, result);
    case CSSSelector::PseudoElement:
        return checkPseudoElement(context, result);

    case CSSSelector::PagePseudoClass:
        // FIXME: what?
        return true;
    case CSSSelector::Unknown:
        // FIXME: what?
        return true;
    }
    ASSERT_NOT_REACHED();
    return true;
}

bool SelectorChecker::checkPseudoNot(const SelectorCheckingContext& context, MatchResult& result) const
{
    const CSSSelector& selector = *context.selector;

    SelectorCheckingContext subContext(context);
    subContext.isSubSelector = true;
    ASSERT(selector.selectorList());
    for (subContext.selector = selector.selectorList()->first(); subContext.selector; subContext.selector = subContext.selector->tagHistory()) {
        // :not cannot nest. I don't really know why this is a
        // restriction in CSS3, but it is, so let's honor it.
        // the parser enforces that this never occurs
        ASSERT(subContext.selector->pseudoType() != CSSSelector::PseudoNot);
        // We select between :visited and :link when applying. We don't know which one applied (or not) yet.
        if (subContext.selector->pseudoType() == CSSSelector::PseudoVisited || (subContext.selector->pseudoType() == CSSSelector::PseudoLink && subContext.visitedMatchType == VisitedMatchEnabled))
            return true;
        // context.scope is not available if m_mode == SharingRules.
        // We cannot determine whether :host or :scope matches a given element or not.
        if (m_mode == SharingRules && (subContext.selector->isHostPseudoClass() || subContext.selector->pseudoType() == CSSSelector::PseudoScope))
            return true;
        if (!checkOne(subContext, result))
            return true;
    }
    return false;
}

bool SelectorChecker::checkPseudoClass(const SelectorCheckingContext& context, MatchResult& result) const
{
    Element& element = *context.element;
    const CSSSelector& selector = *context.selector;

    if (context.hasScrollbarPseudo) {
        // CSS scrollbars match a specific subset of pseudo classes, and they have specialized rules for each
        // (since there are no elements involved).
        return checkScrollbarPseudoClass(context, result);
    }

    switch (selector.pseudoType()) {
    case CSSSelector::PseudoNot:
        return checkPseudoNot(context, result);
    case CSSSelector::PseudoEmpty:
        {
            bool result = true;
            for (Node* n = element.firstChild(); n; n = n->nextSibling()) {
                if (n->isElementNode()) {
                    result = false;
                    break;
                }
                if (n->isTextNode()) {
                    Text* textNode = toText(n);
                    if (!textNode->data().isEmpty()) {
                        result = false;
                        break;
                    }
                }
            }
            if (m_mode == ResolvingStyle) {
                element.setStyleAffectedByEmpty();
                if (context.inRightmostCompound)
                    context.elementStyle->setEmptyState(result);
                else if (element.computedStyle() && (element.document().styleEngine().usesSiblingRules() || element.computedStyle()->unique()))
                    element.mutableComputedStyle()->setEmptyState(result);
            }
            return result;
        }
    case CSSSelector::PseudoFirstChild:
        if (ContainerNode* parent = element.parentElementOrDocumentFragment()) {
            if (m_mode == ResolvingStyle) {
                parent->setChildrenAffectedByFirstChildRules();
                element.setAffectedByFirstChildRules();
            }
            return isFirstChild(element);
        }
        break;
    case CSSSelector::PseudoFirstOfType:
        if (ContainerNode* parent = element.parentElementOrDocumentFragment()) {
            if (m_mode == ResolvingStyle)
                parent->setChildrenAffectedByForwardPositionalRules();
            return isFirstOfType(element, element.tagQName());
        }
        break;
    case CSSSelector::PseudoLastChild:
        if (ContainerNode* parent = element.parentElementOrDocumentFragment()) {
            if (m_mode == ResolvingStyle) {
                parent->setChildrenAffectedByLastChildRules();
                element.setAffectedByLastChildRules();
            }
            if (!parent->isFinishedParsingChildren())
                return false;
            return isLastChild(element);
        }
        break;
    case CSSSelector::PseudoLastOfType:
        if (ContainerNode* parent = element.parentElementOrDocumentFragment()) {
            if (m_mode == ResolvingStyle)
                parent->setChildrenAffectedByBackwardPositionalRules();
            if (!parent->isFinishedParsingChildren())
                return false;
            return isLastOfType(element, element.tagQName());
        }
        break;
    case CSSSelector::PseudoOnlyChild:
        if (ContainerNode* parent = element.parentElementOrDocumentFragment()) {
            if (m_mode == ResolvingStyle) {
                parent->setChildrenAffectedByFirstChildRules();
                parent->setChildrenAffectedByLastChildRules();
                element.setAffectedByFirstChildRules();
                element.setAffectedByLastChildRules();
            }
            if (!parent->isFinishedParsingChildren())
                return false;
            return isFirstChild(element) && isLastChild(element);
        }
        break;
    case CSSSelector::PseudoOnlyOfType:
        // FIXME: This selector is very slow.
        if (ContainerNode* parent = element.parentElementOrDocumentFragment()) {
            if (m_mode == ResolvingStyle) {
                parent->setChildrenAffectedByForwardPositionalRules();
                parent->setChildrenAffectedByBackwardPositionalRules();
            }
            if (!parent->isFinishedParsingChildren())
                return false;
            return isFirstOfType(element, element.tagQName()) && isLastOfType(element, element.tagQName());
        }
        break;
    case CSSSelector::PseudoNthChild:
        if (ContainerNode* parent = element.parentElementOrDocumentFragment()) {
            if (m_mode == ResolvingStyle)
                parent->setChildrenAffectedByForwardPositionalRules();
            return selector.matchNth(nthChildIndex(element));
        }
        break;
    case CSSSelector::PseudoNthOfType:
        if (ContainerNode* parent = element.parentElementOrDocumentFragment()) {
            if (m_mode == ResolvingStyle)
                parent->setChildrenAffectedByForwardPositionalRules();
            return selector.matchNth(nthOfTypeIndex(element, element.tagQName()));
        }
        break;
    case CSSSelector::PseudoNthLastChild:
        if (ContainerNode* parent = element.parentElementOrDocumentFragment()) {
            if (m_mode == ResolvingStyle)
                parent->setChildrenAffectedByBackwardPositionalRules();
            if (!parent->isFinishedParsingChildren())
                return false;
            return selector.matchNth(nthLastChildIndex(element));
        }
        break;
    case CSSSelector::PseudoNthLastOfType:
        if (ContainerNode* parent = element.parentElementOrDocumentFragment()) {
            if (m_mode == ResolvingStyle)
                parent->setChildrenAffectedByBackwardPositionalRules();
            if (!parent->isFinishedParsingChildren())
                return false;
            return selector.matchNth(nthLastOfTypeIndex(element, element.tagQName()));
        }
        break;
    case CSSSelector::PseudoTarget:
        return element == element.document().cssTarget();
    case CSSSelector::PseudoAny:
        {
            SelectorCheckingContext subContext(context);
            subContext.isSubSelector = true;
            ASSERT(selector.selectorList());
            for (subContext.selector = selector.selectorList()->first(); subContext.selector; subContext.selector = CSSSelectorList::next(*subContext.selector)) {
                if (match(subContext))
                    return true;
            }
        }
        break;
    case CSSSelector::PseudoAutofill:
        return element.isFormControlElement() && toHTMLFormControlElement(element).isAutofilled();
    case CSSSelector::PseudoAnyLink:
    case CSSSelector::PseudoLink:
        return element.isLink();
    case CSSSelector::PseudoVisited:
        return element.isLink() && context.visitedMatchType == VisitedMatchEnabled;
    case CSSSelector::PseudoDrag:
        if (m_mode == ResolvingStyle) {
            if (context.inRightmostCompound)
                context.elementStyle->setAffectedByDrag();
            else
                element.setChildrenOrSiblingsAffectedByDrag();
        }
        return element.layoutObject() && element.layoutObject()->isDragging();
    case CSSSelector::PseudoFocus:
        if (m_mode == ResolvingStyle) {
            if (context.inRightmostCompound)
                context.elementStyle->setAffectedByFocus();
            else
                element.setChildrenOrSiblingsAffectedByFocus();
        }
        return matchesFocusPseudoClass(element);
    case CSSSelector::PseudoHover:
        if (m_mode == ResolvingStyle) {
            if (context.inRightmostCompound)
                context.elementStyle->setAffectedByHover();
            else
                element.setChildrenOrSiblingsAffectedByHover();
        }
        if (!shouldMatchHoverOrActive(context))
            return false;
        if (InspectorInstrumentation::forcePseudoState(&element, CSSSelector::PseudoHover))
            return true;
        return element.hovered();
    case CSSSelector::PseudoActive:
        if (m_mode == ResolvingStyle) {
            if (context.inRightmostCompound)
                context.elementStyle->setAffectedByActive();
            else
                element.setChildrenOrSiblingsAffectedByActive();
        }
        if (!shouldMatchHoverOrActive(context))
            return false;
        if (InspectorInstrumentation::forcePseudoState(&element, CSSSelector::PseudoActive))
            return true;
        return element.active();
    case CSSSelector::PseudoEnabled:
        if (element.isFormControlElement() || isHTMLOptionElement(element) || isHTMLOptGroupElement(element))
            return !element.isDisabledFormControl();
        if (isHTMLAnchorElement(element) || isHTMLAreaElement(element))
            return element.isLink();
        break;
    case CSSSelector::PseudoFullPageMedia:
        return element.document().isMediaDocument();
    case CSSSelector::PseudoDefault:
        return element.isDefaultButtonForForm();
    case CSSSelector::PseudoDisabled:
        // TODO(esprehn): Why not just always return isDisabledFormControl()?
        // Can it be true for elements not in the list below?
        if (element.isFormControlElement() || isHTMLOptionElement(element) || isHTMLOptGroupElement(element))
            return element.isDisabledFormControl();
        break;
    case CSSSelector::PseudoReadOnly:
        return element.matchesReadOnlyPseudoClass();
    case CSSSelector::PseudoReadWrite:
        return element.matchesReadWritePseudoClass();
    case CSSSelector::PseudoOptional:
        return element.isOptionalFormControl();
    case CSSSelector::PseudoRequired:
        return element.isRequiredFormControl();
    case CSSSelector::PseudoValid:
        if (m_mode == ResolvingStyle)
            element.document().setContainsValidityStyleRules();
        return element.matchesValidityPseudoClasses() && element.isValidElement();
    case CSSSelector::PseudoInvalid:
        if (m_mode == ResolvingStyle)
            element.document().setContainsValidityStyleRules();
        return element.matchesValidityPseudoClasses() && !element.isValidElement();
    case CSSSelector::PseudoChecked:
        {
            if (isHTMLInputElement(element)) {
                HTMLInputElement& inputElement = toHTMLInputElement(element);
                // Even though WinIE allows checked and indeterminate to
                // co-exist, the CSS selector spec says that you can't be
                // both checked and indeterminate. We will behave like WinIE
                // behind the scenes and just obey the CSS spec here in the
                // test for matching the pseudo.
                if (inputElement.shouldAppearChecked() && !inputElement.shouldAppearIndeterminate())
                    return true;
            } else if (isHTMLOptionElement(element) && toHTMLOptionElement(element).selected()) {
                return true;
            }
            break;
        }
    case CSSSelector::PseudoIndeterminate:
        return element.shouldAppearIndeterminate();
    case CSSSelector::PseudoRoot:
        return element == element.document().documentElement();
    case CSSSelector::PseudoLang:
        {
            AtomicString value;
            if (element.isVTTElement())
                value = toVTTElement(element).language();
            else
                value = element.computeInheritedLanguage();
            const AtomicString& argument = selector.argument();
            if (value.isEmpty() || !startsWithIgnoringASCIICase(value, argument))
                break;
            if (value.length() != argument.length() && value[argument.length()] != '-')
                break;
            return true;
        }
    case CSSSelector::PseudoFullScreen:
        // While a Document is in the fullscreen state, and the document's current fullscreen
        // element is an element in the document, the 'full-screen' pseudoclass applies to
        // that element. Also, an <iframe>, <object> or <embed> element whose child browsing
        // context's Document is in the fullscreen state has the 'full-screen' pseudoclass applied.
        if (isHTMLFrameElementBase(element) && element.containsFullScreenElement())
            return true;
        return Fullscreen::isActiveFullScreenElement(element);
    case CSSSelector::PseudoFullScreenAncestor:
        return element.containsFullScreenElement();
    case CSSSelector::PseudoFullScreenDocument:
        // While a Document is in the fullscreen state, the 'full-screen-document' pseudoclass applies
        // to all elements of that Document.
        return Fullscreen::isFullScreen(element.document());
    case CSSSelector::PseudoInRange:
        if (m_mode == ResolvingStyle)
            element.document().setContainsValidityStyleRules();
        return element.isInRange();
    case CSSSelector::PseudoOutOfRange:
        if (m_mode == ResolvingStyle)
            element.document().setContainsValidityStyleRules();
        return element.isOutOfRange();
    case CSSSelector::PseudoFutureCue:
        return element.isVTTElement() && !toVTTElement(element).isPastNode();
    case CSSSelector::PseudoPastCue:
        return element.isVTTElement() && toVTTElement(element).isPastNode();
    case CSSSelector::PseudoScope:
        if (m_mode == SharingRules)
            return true;
        if (context.scope)
            return context.scope == element;
        return element == element.document().documentElement();
    case CSSSelector::PseudoUnresolved:
        return element.isUnresolvedCustomElement();
    case CSSSelector::PseudoHost:
    case CSSSelector::PseudoHostContext:
        return checkPseudoHost(context, result);
    case CSSSelector::PseudoSpatialNavigationFocus:
        return context.isUARule && matchesSpatialNavigationFocusPseudoClass(element);
    case CSSSelector::PseudoListBox:
        return context.isUARule && matchesListBoxPseudoClass(element);
    case CSSSelector::PseudoWindowInactive:
        if (!context.hasSelectionPseudo)
            return false;
        return !element.document().page()->focusController().isActive();
    case CSSSelector::PseudoHorizontal:
    case CSSSelector::PseudoVertical:
    case CSSSelector::PseudoDecrement:
    case CSSSelector::PseudoIncrement:
    case CSSSelector::PseudoStart:
    case CSSSelector::PseudoEnd:
    case CSSSelector::PseudoDoubleButton:
    case CSSSelector::PseudoSingleButton:
    case CSSSelector::PseudoNoButton:
    case CSSSelector::PseudoCornerPresent:
        return false;
    case CSSSelector::PseudoUnknown:
    default:
        ASSERT_NOT_REACHED();
        break;
    }
    return false;
}

bool SelectorChecker::checkPseudoElement(const SelectorCheckingContext& context, MatchResult& result) const
{
    const CSSSelector& selector = *context.selector;
    Element& element = *context.element;

    switch (selector.pseudoType()) {
    case CSSSelector::PseudoCue:
        {
            SelectorCheckingContext subContext(context);
            subContext.isSubSelector = true;
            subContext.scopeContainsLastMatchedElement = false;
            subContext.treatShadowHostAsNormalScope = false;

            for (subContext.selector = selector.selectorList()->first(); subContext.selector; subContext.selector = CSSSelectorList::next(*subContext.selector)) {
                if (match(subContext))
                    return true;
            }
            return false;
        }
    case CSSSelector::PseudoWebKitCustomElement:
        {
            if (ShadowRoot* root = element.containingShadowRoot())
                return root->type() == ShadowRootType::UserAgent && element.shadowPseudoId() == selector.value();
            return false;
        }
    case CSSSelector::PseudoContent:
        return element.isInShadowTree() && element.isInsertionPoint();
    case CSSSelector::PseudoShadow:
        return element.isInShadowTree() && context.previousElement;
    default:
        break;
    }

    if (!context.inRightmostCompound && m_mode == ResolvingStyle)
        return false;

    if (m_mode == QueryingRules)
        return false;

    PseudoId pseudoId = CSSSelector::pseudoId(selector.pseudoType());
    if (pseudoId != NOPSEUDO && m_mode != SharingRules)
        result.dynamicPseudo = pseudoId;

    // ::before, ::after, etc.
    return true;
}

bool SelectorChecker::checkPseudoHost(const SelectorCheckingContext& context, MatchResult& result) const
{
    const CSSSelector& selector = *context.selector;
    Element& element = *context.element;

    if (m_mode == SharingRules)
        return true;
    // :host only matches a shadow host when :host is in a shadow tree of the shadow host.
    if (!context.scope)
        return false;
    const ContainerNode* shadowHost = context.scope->shadowHost();
    if (!shadowHost || shadowHost != element)
        return false;
    ASSERT(element.shadow());

    // For empty parameter case, i.e. just :host or :host().
    if (!selector.selectorList()) // Use *'s specificity. So just 0.
        return true;

    SelectorCheckingContext subContext(context);
    subContext.isSubSelector = true;

    bool matched = false;
    unsigned maxSpecificity = 0;

    // If one of simple selectors matches an element, returns SelectorMatches. Just "OR".
    for (subContext.selector = selector.selectorList()->first(); subContext.selector; subContext.selector = CSSSelectorList::next(*subContext.selector)) {
        subContext.treatShadowHostAsNormalScope = true;
        subContext.scope = context.scope;
        // Use ComposedTreeTraversal to traverse a composed ancestor list of a given element.
        Element* nextElement = &element;
        SelectorCheckingContext hostContext(subContext);
        do {
            MatchResult subResult;
            hostContext.element = nextElement;
            if (match(hostContext, subResult)) {
                matched = true;
                // Consider div:host(div:host(div:host(div:host...))).
                maxSpecificity = std::max(maxSpecificity, hostContext.selector->specificity() + subResult.specificity);
                break;
            }
            hostContext.scopeContainsLastMatchedElement = false;
            hostContext.treatShadowHostAsNormalScope = false;
            hostContext.scope = nullptr;

            if (selector.pseudoType() == CSSSelector::PseudoHost)
                break;

            hostContext.inRightmostCompound = false;
            nextElement = ComposedTreeTraversal::parentElement(*nextElement);
        } while (nextElement);
    }
    if (matched) {
        result.specificity += maxSpecificity;
        return true;
    }

    // FIXME: this was a fallthrough condition.
    return false;
}

bool SelectorChecker::checkScrollbarPseudoClass(const SelectorCheckingContext& context, MatchResult& result) const
{
    const CSSSelector& selector = *context.selector;
    LayoutScrollbar* scrollbar = context.scrollbar;
    ScrollbarPart part = context.scrollbarPart;

    if (selector.pseudoType() == CSSSelector::PseudoNot)
        return checkPseudoNot(context, result);

    // FIXME: This is a temporary hack for resizers and scrollbar corners. Eventually :window-inactive should become a real
    // pseudo class and just apply to everything.
    if (selector.pseudoType() == CSSSelector::PseudoWindowInactive)
        return !context.element->document().page()->focusController().isActive();

    if (!scrollbar)
        return false;

    switch (selector.pseudoType()) {
    case CSSSelector::PseudoEnabled:
        return scrollbar->enabled();
    case CSSSelector::PseudoDisabled:
        return !scrollbar->enabled();
    case CSSSelector::PseudoHover:
        {
            ScrollbarPart hoveredPart = scrollbar->hoveredPart();
            if (part == ScrollbarBGPart)
                return hoveredPart != NoPart;
            if (part == TrackBGPart)
                return hoveredPart == BackTrackPart || hoveredPart == ForwardTrackPart || hoveredPart == ThumbPart;
            return part == hoveredPart;
        }
    case CSSSelector::PseudoActive:
        {
            ScrollbarPart pressedPart = scrollbar->pressedPart();
            if (part == ScrollbarBGPart)
                return pressedPart != NoPart;
            if (part == TrackBGPart)
                return pressedPart == BackTrackPart || pressedPart == ForwardTrackPart || pressedPart == ThumbPart;
            return part == pressedPart;
        }
    case CSSSelector::PseudoHorizontal:
        return scrollbar->orientation() == HorizontalScrollbar;
    case CSSSelector::PseudoVertical:
        return scrollbar->orientation() == VerticalScrollbar;
    case CSSSelector::PseudoDecrement:
        return part == BackButtonStartPart || part == BackButtonEndPart || part == BackTrackPart;
    case CSSSelector::PseudoIncrement:
        return part == ForwardButtonStartPart || part == ForwardButtonEndPart || part == ForwardTrackPart;
    case CSSSelector::PseudoStart:
        return part == BackButtonStartPart || part == ForwardButtonStartPart || part == BackTrackPart;
    case CSSSelector::PseudoEnd:
        return part == BackButtonEndPart || part == ForwardButtonEndPart || part == ForwardTrackPart;
    case CSSSelector::PseudoDoubleButton:
        {
            ScrollbarButtonsPlacement buttonsPlacement = scrollbar->theme()->buttonsPlacement();
            if (part == BackButtonStartPart || part == ForwardButtonStartPart || part == BackTrackPart)
                return buttonsPlacement == ScrollbarButtonsDoubleStart || buttonsPlacement == ScrollbarButtonsDoubleBoth;
            if (part == BackButtonEndPart || part == ForwardButtonEndPart || part == ForwardTrackPart)
                return buttonsPlacement == ScrollbarButtonsDoubleEnd || buttonsPlacement == ScrollbarButtonsDoubleBoth;
            return false;
        }
    case CSSSelector::PseudoSingleButton:
        {
            ScrollbarButtonsPlacement buttonsPlacement = scrollbar->theme()->buttonsPlacement();
            if (part == BackButtonStartPart || part == ForwardButtonEndPart || part == BackTrackPart || part == ForwardTrackPart)
                return buttonsPlacement == ScrollbarButtonsSingle;
            return false;
        }
    case CSSSelector::PseudoNoButton:
        {
            ScrollbarButtonsPlacement buttonsPlacement = scrollbar->theme()->buttonsPlacement();
            if (part == BackTrackPart)
                return buttonsPlacement == ScrollbarButtonsNone || buttonsPlacement == ScrollbarButtonsDoubleEnd;
            if (part == ForwardTrackPart)
                return buttonsPlacement == ScrollbarButtonsNone || buttonsPlacement == ScrollbarButtonsDoubleStart;
            return false;
        }
    case CSSSelector::PseudoCornerPresent:
        return scrollbar->scrollableArea() && scrollbar->scrollableArea()->isScrollCornerVisible();
    default:
        return false;
    }
}

bool SelectorChecker::matchesFocusPseudoClass(const Element& element)
{
    if (InspectorInstrumentation::forcePseudoState(const_cast<Element*>(&element), CSSSelector::PseudoFocus))
        return true;
    return element.focused() && isFrameFocused(element);
}

}
