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

#ifndef ScopedStyleResolver_h
#define ScopedStyleResolver_h

#include "core/css/ElementRuleCollector.h"
#include "core/css/RuleSet.h"
#include "core/dom/TreeScope.h"
#include "wtf/HashMap.h"
#include "wtf/HashSet.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class PageRuleCollector;
class StyleSheetContents;
class ViewportStyleResolver;

// This class selects a ComputedStyle for a given element based on a collection of stylesheets.
class ScopedStyleResolver final : public NoBaseWillBeGarbageCollected<ScopedStyleResolver> {
    WTF_MAKE_NONCOPYABLE(ScopedStyleResolver);
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(ScopedStyleResolver);
public:
    static PassOwnPtrWillBeRawPtr<ScopedStyleResolver> create(TreeScope& scope)
    {
        return adoptPtrWillBeNoop(new ScopedStyleResolver(scope));
    }

    static TreeScope* treeScopeFor(Document&, const CSSStyleSheet*);

    const TreeScope& treeScope() const { return *m_scope; }
    ScopedStyleResolver* parent() const;

    StyleRuleKeyframes* keyframeStylesForAnimation(const StringImpl* animationName);

    void appendCSSStyleSheet(CSSStyleSheet&, const MediaQueryEvaluator&);
    void collectMatchingAuthorRules(ElementRuleCollector&, bool includeEmptyRules, CascadeOrder = ignoreCascadeOrder);
    void collectMatchingShadowHostRules(ElementRuleCollector&, bool includeEmptyRules, CascadeOrder = ignoreCascadeOrder);
    void collectMatchingTreeBoundaryCrossingRules(ElementRuleCollector&, bool includeEmptyRules, CascadeOrder);
    void matchPageRules(PageRuleCollector&);
    void collectFeaturesTo(RuleFeatureSet&, HashSet<const StyleSheetContents*>& visitedSharedStyleSheetContents) const;
    void resetAuthorStyle();
    void collectViewportRulesTo(ViewportStyleResolver*) const;

    DECLARE_TRACE();

private:
    explicit ScopedStyleResolver(TreeScope& scope)
        : m_scope(scope)
    {
    }

    void addTreeBoundaryCrossingRules(const RuleSet&, CSSStyleSheet*, unsigned sheetIndex);
    void addKeyframeRules(const RuleSet&);
    void addFontFaceRules(const RuleSet&);
    void addKeyframeStyle(PassRefPtrWillBeRawPtr<StyleRuleKeyframes>);

    RawPtrWillBeMember<TreeScope> m_scope;

    WillBeHeapVector<RawPtrWillBeMember<CSSStyleSheet>> m_authorStyleSheets;

    typedef WillBeHeapHashMap<const StringImpl*, RefPtrWillBeMember<StyleRuleKeyframes>> KeyframesRuleMap;
    KeyframesRuleMap m_keyframesRuleMap;

    class RuleSubSet final : public NoBaseWillBeGarbageCollected<RuleSubSet> {
    public:
        static PassOwnPtrWillBeRawPtr<RuleSubSet> create(CSSStyleSheet* sheet, unsigned index, PassOwnPtrWillBeRawPtr<RuleSet> rules)
        {
            return adoptPtrWillBeNoop(new RuleSubSet(sheet, index, rules));
        }

        CSSStyleSheet* m_parentStyleSheet;
        unsigned m_parentIndex;
        OwnPtrWillBeMember<RuleSet> m_ruleSet;

        DECLARE_TRACE();

    private:
        RuleSubSet(CSSStyleSheet* sheet, unsigned index, PassOwnPtrWillBeRawPtr<RuleSet> rules)
            : m_parentStyleSheet(sheet)
            , m_parentIndex(index)
            , m_ruleSet(rules)
        {
        }
    };
    typedef WillBeHeapVector<OwnPtrWillBeMember<RuleSubSet>> CSSStyleSheetRuleSubSet;

    OwnPtrWillBeMember<CSSStyleSheetRuleSubSet> m_treeBoundaryCrossingRuleSet;
};

} // namespace blink

#endif // ScopedStyleResolver_h
