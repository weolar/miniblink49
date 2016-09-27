/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/css/resolver/ScopedStyleResolver.h"

#include "core/HTMLNames.h"
#include "core/css/CSSFontSelector.h"
#include "core/css/CSSStyleSheet.h"
#include "core/css/FontFace.h"
#include "core/css/PageRuleCollector.h"
#include "core/css/RuleFeature.h"
#include "core/css/StyleRule.h"
#include "core/css/StyleSheetContents.h"
#include "core/css/resolver/MatchRequest.h"
#include "core/css/resolver/ViewportStyleResolver.h"
#include "core/dom/Document.h"
#include "core/dom/StyleEngine.h"
#include "core/dom/shadow/ElementShadow.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/html/HTMLStyleElement.h"
#include "core/svg/SVGStyleElement.h"

namespace blink {

TreeScope* ScopedStyleResolver::treeScopeFor(Document& document, const CSSStyleSheet* sheet)
{
    ASSERT(sheet);

    if (!sheet->ownerDocument())
        return 0;

    Node* ownerNode = sheet->ownerNode();
    if (!isHTMLStyleElement(ownerNode) && !isSVGStyleElement(ownerNode))
        return &document;

    Element& styleElement = toElement(*ownerNode);
    if (styleElement.isInShadowTree())
        return styleElement.containingShadowRoot();
    return &document;
}

ScopedStyleResolver* ScopedStyleResolver::parent() const
{
    for (TreeScope* scope = treeScope().parentTreeScope(); scope; scope = scope->parentTreeScope()) {
        if (ScopedStyleResolver* resolver = scope->scopedStyleResolver())
            return resolver;
    }
    return 0;
}

void ScopedStyleResolver::addKeyframeRules(const RuleSet& ruleSet)
{
    const WillBeHeapVector<RawPtrWillBeMember<StyleRuleKeyframes>> keyframesRules = ruleSet.keyframesRules();
    for (unsigned i = 0; i < keyframesRules.size(); ++i)
        addKeyframeStyle(keyframesRules[i]);
}

void ScopedStyleResolver::addFontFaceRules(const RuleSet& ruleSet)
{
    // FIXME(BUG 72461): We don't add @font-face rules of scoped style sheets for the moment.
    if (!treeScope().rootNode().isDocumentNode())
        return;

    Document& document = treeScope().document();
    CSSFontSelector* cssFontSelector = document.styleEngine().fontSelector();
    const WillBeHeapVector<RawPtrWillBeMember<StyleRuleFontFace>> fontFaceRules = ruleSet.fontFaceRules();
    for (auto& fontFaceRule : fontFaceRules) {
        if (RefPtrWillBeRawPtr<FontFace> fontFace = FontFace::create(&document, fontFaceRule))
            cssFontSelector->fontFaceCache()->add(cssFontSelector, fontFaceRule, fontFace);
    }
    if (fontFaceRules.size())
        document.styleResolver()->invalidateMatchedPropertiesCache();
}

void ScopedStyleResolver::appendCSSStyleSheet(CSSStyleSheet& cssSheet, const MediaQueryEvaluator& medium)
{
    unsigned index = m_authorStyleSheets.size();
    m_authorStyleSheets.append(&cssSheet);
    StyleSheetContents* sheet = cssSheet.contents();
    AddRuleFlags addRuleFlags = treeScope().document().securityOrigin()->canRequest(sheet->baseURL()) ? RuleHasDocumentSecurityOrigin : RuleHasNoSpecialState;
    const RuleSet& ruleSet = sheet->ensureRuleSet(medium, addRuleFlags);

    addKeyframeRules(ruleSet);
    addFontFaceRules(ruleSet);
    addTreeBoundaryCrossingRules(ruleSet, &cssSheet, index);
    treeScope().document().styleResolver()->addMediaQueryResults(ruleSet.viewportDependentMediaQueryResults());
}

void ScopedStyleResolver::collectFeaturesTo(RuleFeatureSet& features, HashSet<const StyleSheetContents*>& visitedSharedStyleSheetContents) const
{
    for (size_t i = 0; i < m_authorStyleSheets.size(); ++i) {
        ASSERT(m_authorStyleSheets[i]->ownerNode());
        StyleSheetContents* contents = m_authorStyleSheets[i]->contents();
        if (contents->hasOneClient() || visitedSharedStyleSheetContents.add(contents).isNewEntry)
            features.add(contents->ruleSet().features());
    }

    if (!m_treeBoundaryCrossingRuleSet)
        return;

    for (const auto& rules : *m_treeBoundaryCrossingRuleSet)
        features.add(rules->m_ruleSet->features());
}

void ScopedStyleResolver::resetAuthorStyle()
{
    m_authorStyleSheets.clear();
    m_keyframesRuleMap.clear();
    m_treeBoundaryCrossingRuleSet = nullptr;
}

StyleRuleKeyframes* ScopedStyleResolver::keyframeStylesForAnimation(const StringImpl* animationName)
{
    if (m_keyframesRuleMap.isEmpty())
        return nullptr;

    KeyframesRuleMap::iterator it = m_keyframesRuleMap.find(animationName);
    if (it == m_keyframesRuleMap.end())
        return nullptr;

    return it->value.get();
}

void ScopedStyleResolver::addKeyframeStyle(PassRefPtrWillBeRawPtr<StyleRuleKeyframes> rule)
{
    AtomicString s(rule->name());

    if (rule->isVendorPrefixed()) {
        KeyframesRuleMap::iterator it = m_keyframesRuleMap.find(s.impl());
        if (it == m_keyframesRuleMap.end())
            m_keyframesRuleMap.set(s.impl(), rule);
        else if (it->value->isVendorPrefixed())
            m_keyframesRuleMap.set(s.impl(), rule);
    } else {
        m_keyframesRuleMap.set(s.impl(), rule);
    }
}

void ScopedStyleResolver::collectMatchingAuthorRules(ElementRuleCollector& collector, bool includeEmptyRules, CascadeOrder cascadeOrder)
{
    ASSERT(!collector.scopeContainsLastMatchedElement());
    collector.setScopeContainsLastMatchedElement(true);
    for (size_t i = 0; i < m_authorStyleSheets.size(); ++i) {
        ASSERT(m_authorStyleSheets[i]->ownerNode());
        MatchRequest matchRequest(&m_authorStyleSheets[i]->contents()->ruleSet(), includeEmptyRules, &m_scope->rootNode(), m_authorStyleSheets[i], i);
        collector.collectMatchingRules(matchRequest, cascadeOrder);
    }
    collector.setScopeContainsLastMatchedElement(false);
}

void ScopedStyleResolver::collectMatchingShadowHostRules(ElementRuleCollector& collector, bool includeEmptyRules, CascadeOrder cascadeOrder)
{
    ASSERT(!collector.scopeContainsLastMatchedElement());
    collector.setScopeContainsLastMatchedElement(true);
    for (size_t i = 0; i < m_authorStyleSheets.size(); ++i) {
        ASSERT(m_authorStyleSheets[i]->ownerNode());
        MatchRequest matchRequest(&m_authorStyleSheets[i]->contents()->ruleSet(), includeEmptyRules, &m_scope->rootNode(), m_authorStyleSheets[i], i);
        collector.collectMatchingShadowHostRules(matchRequest, cascadeOrder);
    }
    collector.setScopeContainsLastMatchedElement(false);
}

void ScopedStyleResolver::collectMatchingTreeBoundaryCrossingRules(ElementRuleCollector& collector, bool includeEmptyRules, CascadeOrder cascadeOrder)
{
    for (const auto& rules : *m_treeBoundaryCrossingRuleSet) {
        MatchRequest request(rules->m_ruleSet.get(), includeEmptyRules, &treeScope().rootNode(), rules->m_parentStyleSheet, rules->m_parentIndex);
        collector.collectMatchingRules(request, cascadeOrder, true);
    }
}

void ScopedStyleResolver::matchPageRules(PageRuleCollector& collector)
{
    // Only consider the global author RuleSet for @page rules, as per the HTML5 spec.
    ASSERT(m_scope->rootNode().isDocumentNode());
    for (size_t i = 0; i < m_authorStyleSheets.size(); ++i)
        collector.matchPageRules(&m_authorStyleSheets[i]->contents()->ruleSet());
}

void ScopedStyleResolver::collectViewportRulesTo(ViewportStyleResolver* resolver) const
{
    if (!m_scope->rootNode().isDocumentNode())
        return;
    for (size_t i = 0; i < m_authorStyleSheets.size(); ++i)
        resolver->collectViewportRules(&m_authorStyleSheets[i]->contents()->ruleSet(), ViewportStyleResolver::AuthorOrigin);
}

DEFINE_TRACE(ScopedStyleResolver)
{
#if ENABLE(OILPAN)
    visitor->trace(m_scope);
    visitor->trace(m_authorStyleSheets);
    visitor->trace(m_keyframesRuleMap);
    visitor->trace(m_treeBoundaryCrossingRuleSet);
#endif
}

static void addRules(RuleSet* ruleSet, const WillBeHeapVector<MinimalRuleData>& rules)
{
    for (unsigned i = 0; i < rules.size(); ++i) {
        const MinimalRuleData& info = rules[i];
        ruleSet->addRule(info.m_rule, info.m_selectorIndex, info.m_flags);
    }
}

void ScopedStyleResolver::addTreeBoundaryCrossingRules(const RuleSet& authorRules, CSSStyleSheet* parentStyleSheet, unsigned sheetIndex)
{
    bool isDocumentScope = treeScope().rootNode().isDocumentNode();
    if (authorRules.treeBoundaryCrossingRules().isEmpty() && (isDocumentScope || authorRules.shadowDistributedRules().isEmpty()))
        return;

    OwnPtrWillBeRawPtr<RuleSet> ruleSetForScope = RuleSet::create();
    addRules(ruleSetForScope.get(), authorRules.treeBoundaryCrossingRules());

    if (!isDocumentScope)
        addRules(ruleSetForScope.get(), authorRules.shadowDistributedRules());

    if (!m_treeBoundaryCrossingRuleSet) {
        m_treeBoundaryCrossingRuleSet = adoptPtrWillBeNoop(new CSSStyleSheetRuleSubSet());
        treeScope().document().styleResolver()->addTreeBoundaryCrossingScope(treeScope().rootNode());
    }

    m_treeBoundaryCrossingRuleSet->append(RuleSubSet::create(parentStyleSheet, sheetIndex, ruleSetForScope.release()));
}

DEFINE_TRACE(ScopedStyleResolver::RuleSubSet)
{
    visitor->trace(m_ruleSet);
}

} // namespace blink
