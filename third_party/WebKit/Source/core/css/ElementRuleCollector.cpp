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
 * Copyright (C) 2012 Google Inc. All rights reserved.
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
#include "core/css/ElementRuleCollector.h"

#include "core/css/CSSImportRule.h"
#include "core/css/CSSKeyframesRule.h"
#include "core/css/CSSMediaRule.h"
#include "core/css/CSSRuleList.h"
#include "core/css/CSSSelector.h"
#include "core/css/CSSStyleRule.h"
#include "core/css/CSSStyleSheet.h"
#include "core/css/CSSSupportsRule.h"
#include "core/css/StylePropertySet.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/css/resolver/StyleResolverStats.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/style/StyleInheritedData.h"

namespace blink {

ElementRuleCollector::ElementRuleCollector(const ElementResolveContext& context,
    const SelectorFilter& filter, ComputedStyle* style)
    : m_context(context)
    , m_selectorFilter(filter)
    , m_style(style)
    , m_pseudoStyleRequest(NOPSEUDO)
    , m_mode(SelectorChecker::ResolvingStyle)
    , m_canUseFastReject(m_selectorFilter.parentStackIsConsistent(context.parentNode()))
    , m_sameOriginOnly(false)
    , m_matchingUARules(false)
    , m_scopeContainsLastMatchedElement(false)
{ }

ElementRuleCollector::~ElementRuleCollector()
{
}

const MatchResult& ElementRuleCollector::matchedResult() const
{
    return m_result;
}

PassRefPtrWillBeRawPtr<StyleRuleList> ElementRuleCollector::matchedStyleRuleList()
{
    ASSERT(m_mode == SelectorChecker::CollectingStyleRules);
    return m_styleRuleList.release();
}

PassRefPtrWillBeRawPtr<CSSRuleList> ElementRuleCollector::matchedCSSRuleList()
{
    ASSERT(m_mode == SelectorChecker::CollectingCSSRules);
    return m_cssRuleList.release();
}

void ElementRuleCollector::clearMatchedRules()
{
    m_matchedRules.clear();
}

inline StyleRuleList* ElementRuleCollector::ensureStyleRuleList()
{
#if ENABLE(OILPAN)
    if (!m_styleRuleList)
        m_styleRuleList = new StyleRuleList();
    return m_styleRuleList;
#else
    if (!m_styleRuleList)
        m_styleRuleList = StyleRuleList::create();
    return m_styleRuleList.get();
#endif
}

inline StaticCSSRuleList* ElementRuleCollector::ensureRuleList()
{
    if (!m_cssRuleList)
        m_cssRuleList = StaticCSSRuleList::create();
    return m_cssRuleList.get();
}

void ElementRuleCollector::addElementStyleProperties(const StylePropertySet* propertySet, bool isCacheable)
{
    if (!propertySet)
        return;
    m_result.addMatchedProperties(propertySet);
    if (!isCacheable)
        m_result.isCacheable = false;
}

static bool rulesApplicableInCurrentTreeScope(const Element* element, const ContainerNode* scopingNode, bool matchingTreeBoundaryRules)
{
    // [skipped, because already checked] a) it's a UA rule
    // b) We're mathcing tree boundary rules.
    if (matchingTreeBoundaryRules)
        return true;
    // c) the rules comes from a scoped style sheet within the same tree scope
    if (!scopingNode || element->treeScope() == scopingNode->treeScope())
        return true;
    // d) the rules comes from a scoped style sheet within an active shadow root whose host is the given element
    if (element == scopingNode->shadowHost())
        return true;
    return false;
}

template<typename RuleDataListType>
void ElementRuleCollector::collectMatchingRulesForList(const RuleDataListType* rules, CascadeOrder cascadeOrder, const MatchRequest& matchRequest)
{
    if (!rules)
        return;

    SelectorChecker checker(m_mode);
    SelectorChecker::SelectorCheckingContext checkerContext(m_context.element(), SelectorChecker::VisitedMatchEnabled);
    checkerContext.elementStyle = m_style.get();
    checkerContext.scope = matchRequest.scope;
    checkerContext.pseudoId = m_pseudoStyleRequest.pseudoId;
    checkerContext.scrollbar = m_pseudoStyleRequest.scrollbar;
    checkerContext.scrollbarPart = m_pseudoStyleRequest.scrollbarPart;
    checkerContext.isUARule = m_matchingUARules;
    checkerContext.scopeContainsLastMatchedElement = m_scopeContainsLastMatchedElement;

    unsigned rejected = 0;
    unsigned fastRejected = 0;
    unsigned matched = 0;

    for (const auto& ruleData : *rules) {
        if (m_canUseFastReject && m_selectorFilter.fastRejectSelector<RuleData::maximumIdentifierCount>(ruleData.descendantSelectorIdentifierHashes())) {
            fastRejected++;
            continue;
        }

        // FIXME: Exposing the non-standard getMatchedCSSRules API to web is the only reason this is needed.
        if (m_sameOriginOnly && !ruleData.hasDocumentSecurityOrigin())
            continue;

        StyleRule* rule = ruleData.rule();

        // If the rule has no properties to apply, then ignore it in the non-debug mode.
        const StylePropertySet& properties = rule->properties();
        if (properties.isEmpty() && !matchRequest.includeEmptyRules)
            continue;

        SelectorChecker::MatchResult result;
        checkerContext.selector = &ruleData.selector();
        if (!checker.match(checkerContext, result)) {
            rejected++;
            continue;
        }
        if (m_pseudoStyleRequest.pseudoId != NOPSEUDO && m_pseudoStyleRequest.pseudoId != result.dynamicPseudo) {
            rejected++;
            continue;
        }

        matched++;
        didMatchRule(ruleData, result, cascadeOrder, matchRequest);
    }

    if (StyleResolver* resolver = m_context.element()->document().styleResolver()) {
        INCREMENT_STYLE_STATS_COUNTER(*resolver, rulesRejected, rejected);
        INCREMENT_STYLE_STATS_COUNTER(*resolver, rulesFastRejected, fastRejected);
        INCREMENT_STYLE_STATS_COUNTER(*resolver, rulesMatched, matched);
    }
}

void ElementRuleCollector::collectMatchingRules(const MatchRequest& matchRequest, CascadeOrder cascadeOrder, bool matchingTreeBoundaryRules)
{
    ASSERT(matchRequest.ruleSet);
    ASSERT(m_context.element());

    Element& element = *m_context.element();
    const AtomicString& pseudoId = element.shadowPseudoId();
    if (!pseudoId.isEmpty()) {
        ASSERT(element.isStyledElement());
        collectMatchingRulesForList(matchRequest.ruleSet->shadowPseudoElementRules(pseudoId), cascadeOrder, matchRequest);
    }

    if (element.isVTTElement())
        collectMatchingRulesForList(matchRequest.ruleSet->cuePseudoRules(), cascadeOrder, matchRequest);
    // Check whether other types of rules are applicable in the current tree scope. Criteria for this:
    // a) it's a UA rule
    // b) the rules comes from a scoped style sheet within the same tree scope
    // c) the rules comes from a scoped style sheet within an active shadow root whose host is the given element
    // d) the rules can cross boundaries
    // b)-e) is checked in rulesApplicableInCurrentTreeScope.
    if (!m_matchingUARules && !rulesApplicableInCurrentTreeScope(&element, matchRequest.scope, matchingTreeBoundaryRules))
        return;

    // We need to collect the rules for id, class, tag, and everything else into a buffer and
    // then sort the buffer.
    if (element.hasID())
        collectMatchingRulesForList(matchRequest.ruleSet->idRules(element.idForStyleResolution()), cascadeOrder, matchRequest);
    if (element.isStyledElement() && element.hasClass()) {
        for (size_t i = 0; i < element.classNames().size(); ++i)
            collectMatchingRulesForList(matchRequest.ruleSet->classRules(element.classNames()[i]), cascadeOrder, matchRequest);
    }

    if (element.isLink())
        collectMatchingRulesForList(matchRequest.ruleSet->linkPseudoClassRules(), cascadeOrder, matchRequest);
    if (SelectorChecker::matchesFocusPseudoClass(element))
        collectMatchingRulesForList(matchRequest.ruleSet->focusPseudoClassRules(), cascadeOrder, matchRequest);
    collectMatchingRulesForList(matchRequest.ruleSet->tagRules(element.localNameForSelectorMatching()), cascadeOrder, matchRequest);
    collectMatchingRulesForList(matchRequest.ruleSet->universalRules(), cascadeOrder, matchRequest);
}

void ElementRuleCollector::collectMatchingShadowHostRules(const MatchRequest& matchRequest, CascadeOrder cascadeOrder, bool matchingTreeBoundaryRules)
{
    collectMatchingRulesForList(matchRequest.ruleSet->shadowHostRules(), cascadeOrder, matchRequest);
}

template<class CSSRuleCollection>
CSSRule* ElementRuleCollector::findStyleRule(CSSRuleCollection* cssRules, StyleRule* styleRule)
{
    if (!cssRules)
        return 0;
    CSSRule* result = 0;
    for (unsigned i = 0; i < cssRules->length() && !result; ++i) {
        CSSRule* cssRule = cssRules->item(i);
        CSSRule::Type cssRuleType = cssRule->type();
        if (cssRuleType == CSSRule::STYLE_RULE) {
            CSSStyleRule* cssStyleRule = toCSSStyleRule(cssRule);
            if (cssStyleRule->styleRule() == styleRule)
                result = cssRule;
        } else if (cssRuleType == CSSRule::IMPORT_RULE) {
            CSSImportRule* cssImportRule = toCSSImportRule(cssRule);
            result = findStyleRule(cssImportRule->styleSheet(), styleRule);
        } else {
            result = findStyleRule(cssRule->cssRules(), styleRule);
        }
    }
    return result;
}

void ElementRuleCollector::appendCSSOMWrapperForRule(CSSStyleSheet* parentStyleSheet, StyleRule* rule)
{
    // |parentStyleSheet| is 0 if and only if the |rule| is coming from User Agent. In this case,
    // it is safe to create CSSOM wrappers without parentStyleSheets as they will be used only
    // by inspector which will not try to edit them.
    RefPtrWillBeRawPtr<CSSRule> cssRule = nullptr;
    if (parentStyleSheet)
        cssRule = findStyleRule(parentStyleSheet, rule);
    else
        cssRule = rule->createCSSOMWrapper();
    ASSERT(!parentStyleSheet || cssRule);
    ensureRuleList()->rules().append(cssRule);
}

void ElementRuleCollector::sortAndTransferMatchedRules()
{
    if (m_matchedRules.isEmpty())
        return;

    sortMatchedRules();

    if (m_mode == SelectorChecker::CollectingStyleRules) {
        for (unsigned i = 0; i < m_matchedRules.size(); ++i)
            ensureStyleRuleList()->append(m_matchedRules[i].ruleData()->rule());
        return;
    }

    if (m_mode == SelectorChecker::CollectingCSSRules) {
        for (unsigned i = 0; i < m_matchedRules.size(); ++i)
            appendCSSOMWrapperForRule(const_cast<CSSStyleSheet*>(m_matchedRules[i].parentStyleSheet()), m_matchedRules[i].ruleData()->rule());
        return;
    }

    // Now transfer the set of matched rules over to our list of declarations.
    for (unsigned i = 0; i < m_matchedRules.size(); i++) {
        const RuleData* ruleData = m_matchedRules[i].ruleData();
        m_result.addMatchedProperties(&ruleData->rule()->properties(), ruleData->linkMatchType(), ruleData->propertyWhitelistType(m_matchingUARules));
    }

    if (m_matchingUARules)
        m_result.uaEnd = m_result.matchedProperties.size();
}

void ElementRuleCollector::didMatchRule(const RuleData& ruleData, const SelectorChecker::MatchResult& result, CascadeOrder cascadeOrder, const MatchRequest& matchRequest)
{
    PseudoId dynamicPseudo = result.dynamicPseudo;
    // If we're matching normal rules, set a pseudo bit if
    // we really just matched a pseudo-element.
    if (dynamicPseudo != NOPSEUDO && m_pseudoStyleRequest.pseudoId == NOPSEUDO) {
        if (m_mode == SelectorChecker::CollectingCSSRules || m_mode == SelectorChecker::CollectingStyleRules)
            return;
        // FIXME: Matching should not modify the style directly.
        if (!m_style || dynamicPseudo >= FIRST_INTERNAL_PSEUDOID)
            return;
        if ((dynamicPseudo == BEFORE || dynamicPseudo == AFTER) && !ruleData.rule()->properties().hasProperty(CSSPropertyContent))
            return;
        m_style->setHasPseudoStyle(dynamicPseudo);
    } else {
        if (m_style && ruleData.containsUncommonAttributeSelector())
            m_style->setUnique();

        m_matchedRules.append(MatchedRule(&ruleData, result.specificity, cascadeOrder, matchRequest.styleSheetIndex, matchRequest.styleSheet));
    }
}

static inline bool compareRules(const MatchedRule& matchedRule1, const MatchedRule& matchedRule2)
{
    unsigned specificity1 = matchedRule1.specificity();
    unsigned specificity2 = matchedRule2.specificity();
    if (specificity1 != specificity2)
        return specificity1 < specificity2;

    return matchedRule1.position() < matchedRule2.position();
}

void ElementRuleCollector::sortMatchedRules()
{
    std::sort(m_matchedRules.begin(), m_matchedRules.end(), compareRules);
}

bool ElementRuleCollector::hasAnyMatchingRules(RuleSet* ruleSet)
{
    clearMatchedRules();

    m_mode = SelectorChecker::SharingRules;
    // To check whether a given RuleSet has any rule matching a given element,
    // should not see the element's treescope. Because RuleSet has no
    // information about "scope".
    MatchRequest matchRequest(ruleSet);
    collectMatchingRules(matchRequest);
    collectMatchingShadowHostRules(matchRequest);

    return !m_matchedRules.isEmpty();
}

} // namespace blink
