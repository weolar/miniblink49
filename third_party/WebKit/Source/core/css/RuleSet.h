/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
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

#ifndef RuleSet_h
#define RuleSet_h

#include "core/CoreExport.h"
#include "core/css/CSSKeyframesRule.h"
#include "core/css/MediaQueryEvaluator.h"
#include "core/css/RuleFeature.h"
#include "core/css/StyleRule.h"
#include "core/css/resolver/MediaQueryResult.h"
#include "platform/heap/HeapLinkedStack.h"
#include "platform/heap/HeapTerminatedArray.h"
#include "wtf/Forward.h"
#include "wtf/HashMap.h"
#include "wtf/LinkedStack.h"
#include "wtf/TerminatedArray.h"

namespace blink {

enum AddRuleFlags {
    RuleHasNoSpecialState         = 0,
    RuleHasDocumentSecurityOrigin = 1,
};

enum PropertyWhitelistType {
    PropertyWhitelistNone,
    PropertyWhitelistCue,
    PropertyWhitelistFirstLetter,
};

class CSSSelector;
class MediaQueryEvaluator;
class StyleSheetContents;

class MinimalRuleData {
    ALLOW_ONLY_INLINE_ALLOCATION();
public:
    MinimalRuleData(StyleRule* rule, unsigned selectorIndex, AddRuleFlags flags)
    : m_rule(rule)
    , m_selectorIndex(selectorIndex)
    , m_flags(flags)
    {
    }

    DECLARE_TRACE();

    RawPtrWillBeMember<StyleRule> m_rule;
    unsigned m_selectorIndex;
    AddRuleFlags m_flags;
};

class RuleData {
    ALLOW_ONLY_INLINE_ALLOCATION();
public:
    RuleData(StyleRule*, unsigned selectorIndex, unsigned position, AddRuleFlags);

    unsigned position() const { return m_position; }
    StyleRule* rule() const { return m_rule; }
    const CSSSelector& selector() const { return m_rule->selectorList().selectorAt(m_selectorIndex); }
    unsigned selectorIndex() const { return m_selectorIndex; }

    bool isLastInArray() const { return m_isLastInArray; }
    void setLastInArray(bool flag) { m_isLastInArray = flag; }

    bool containsUncommonAttributeSelector() const { return m_containsUncommonAttributeSelector; }
    unsigned specificity() const { return m_specificity; }
    unsigned linkMatchType() const { return m_linkMatchType; }
    bool hasDocumentSecurityOrigin() const { return m_hasDocumentSecurityOrigin; }
    PropertyWhitelistType propertyWhitelistType(bool isMatchingUARules = false) const { return isMatchingUARules ? PropertyWhitelistNone : static_cast<PropertyWhitelistType>(m_propertyWhitelistType); }
    // Try to balance between memory usage (there can be lots of RuleData objects) and good filtering performance.
    static const unsigned maximumIdentifierCount = 4;
    const unsigned* descendantSelectorIdentifierHashes() const { return m_descendantSelectorIdentifierHashes; }

    DECLARE_TRACE();

private:
    RawPtrWillBeMember<StyleRule> m_rule;
    unsigned m_selectorIndex : 12;
    unsigned m_isLastInArray : 1; // We store an array of RuleData objects in a primitive array.
    // This number was picked fairly arbitrarily. We can probably lower it if we need to.
    // Some simple testing showed <100,000 RuleData's on large sites.
    unsigned m_position : 18;
    unsigned m_specificity : 24;
    unsigned m_containsUncommonAttributeSelector : 1;
    unsigned m_linkMatchType : 2; //  CSSSelector::LinkMatchMask
    unsigned m_hasDocumentSecurityOrigin : 1;
    unsigned m_propertyWhitelistType : 2;
    // Use plain array instead of a Vector to minimize memory overhead.
    unsigned m_descendantSelectorIdentifierHashes[maximumIdentifierCount];
};

struct SameSizeAsRuleData {
    void* a;
    unsigned b;
    unsigned c;
    unsigned d[4];
};

static_assert(sizeof(RuleData) == sizeof(SameSizeAsRuleData), "RuleData should stay small");

class CORE_EXPORT RuleSet : public NoBaseWillBeGarbageCollectedFinalized<RuleSet> {
    WTF_MAKE_NONCOPYABLE(RuleSet);
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(RuleSet);
public:
    static PassOwnPtrWillBeRawPtr<RuleSet> create() { return adoptPtrWillBeNoop(new RuleSet); }

    void addRulesFromSheet(StyleSheetContents*, const MediaQueryEvaluator&, AddRuleFlags = RuleHasNoSpecialState);
    void addStyleRule(StyleRule*, AddRuleFlags);
    void addRule(StyleRule*, unsigned selectorIndex, AddRuleFlags);

    const RuleFeatureSet& features() const { return m_features; }

    const WillBeHeapTerminatedArray<RuleData>* idRules(const AtomicString& key) const { ASSERT(!m_pendingRules); return m_idRules.get(key); }
    const WillBeHeapTerminatedArray<RuleData>* classRules(const AtomicString& key) const { ASSERT(!m_pendingRules); return m_classRules.get(key); }
    const WillBeHeapTerminatedArray<RuleData>* tagRules(const AtomicString& key) const { ASSERT(!m_pendingRules); return m_tagRules.get(key); }
    const WillBeHeapTerminatedArray<RuleData>* shadowPseudoElementRules(const AtomicString& key) const { ASSERT(!m_pendingRules); return m_shadowPseudoElementRules.get(key); }
    const WillBeHeapVector<RuleData>* linkPseudoClassRules() const { ASSERT(!m_pendingRules); return &m_linkPseudoClassRules; }
    const WillBeHeapVector<RuleData>* cuePseudoRules() const { ASSERT(!m_pendingRules); return &m_cuePseudoRules; }
    const WillBeHeapVector<RuleData>* focusPseudoClassRules() const { ASSERT(!m_pendingRules); return &m_focusPseudoClassRules; }
    const WillBeHeapVector<RuleData>* universalRules() const { ASSERT(!m_pendingRules); return &m_universalRules; }
    const WillBeHeapVector<RuleData>* shadowHostRules() const { ASSERT(!m_pendingRules); return &m_shadowHostRules; }
    const WillBeHeapVector<RawPtrWillBeMember<StyleRulePage>>& pageRules() const { ASSERT(!m_pendingRules); return m_pageRules; }
    const WillBeHeapVector<RawPtrWillBeMember<StyleRuleViewport>>& viewportRules() const { ASSERT(!m_pendingRules); return m_viewportRules; }
    const WillBeHeapVector<RawPtrWillBeMember<StyleRuleFontFace>>& fontFaceRules() const { return m_fontFaceRules; }
    const WillBeHeapVector<RawPtrWillBeMember<StyleRuleKeyframes>>& keyframesRules() const { return m_keyframesRules; }
    const WillBeHeapVector<MinimalRuleData>& treeBoundaryCrossingRules() const { return m_treeBoundaryCrossingRules; }
    const WillBeHeapVector<MinimalRuleData>& shadowDistributedRules() const { return m_shadowDistributedRules; }
    const MediaQueryResultList& viewportDependentMediaQueryResults() const { return m_viewportDependentMediaQueryResults; }

    unsigned ruleCount() const { return m_ruleCount; }

    void compactRulesIfNeeded()
    {
        if (!m_pendingRules)
            return;
        compactRules();
    }

#ifndef NDEBUG
    void show();
#endif

    DECLARE_TRACE();

private:
    typedef WillBeHeapHashMap<AtomicString, OwnPtrWillBeMember<WillBeHeapLinkedStack<RuleData>>> PendingRuleMap;
    typedef WillBeHeapHashMap<AtomicString, OwnPtrWillBeMember<WillBeHeapTerminatedArray<RuleData>>> CompactRuleMap;

    RuleSet()
        : m_ruleCount(0)
    {
    }

    void addToRuleSet(const AtomicString& key, PendingRuleMap&, const RuleData&);
    void addPageRule(StyleRulePage*);
    void addViewportRule(StyleRuleViewport*);
    void addFontFaceRule(StyleRuleFontFace*);
    void addKeyframesRule(StyleRuleKeyframes*);

    void addChildRules(const WillBeHeapVector<RefPtrWillBeMember<StyleRuleBase>>&, const MediaQueryEvaluator& medium, AddRuleFlags);
    bool findBestRuleSetAndAdd(const CSSSelector&, RuleData&);

    void compactRules();
    static void compactPendingRules(PendingRuleMap&, CompactRuleMap&);

    class PendingRuleMaps : public NoBaseWillBeGarbageCollected<PendingRuleMaps> {
    public:
        static PassOwnPtrWillBeRawPtr<PendingRuleMaps> create() { return adoptPtrWillBeNoop(new PendingRuleMaps); }

        PendingRuleMap idRules;
        PendingRuleMap classRules;
        PendingRuleMap tagRules;
        PendingRuleMap shadowPseudoElementRules;

        DECLARE_TRACE();

    private:
        PendingRuleMaps() { }
    };

    PendingRuleMaps* ensurePendingRules()
    {
        if (!m_pendingRules)
            m_pendingRules = PendingRuleMaps::create();
        return m_pendingRules.get();
    }

    CompactRuleMap m_idRules;
    CompactRuleMap m_classRules;
    CompactRuleMap m_tagRules;
    CompactRuleMap m_shadowPseudoElementRules;
    WillBeHeapVector<RuleData> m_linkPseudoClassRules;
    WillBeHeapVector<RuleData> m_cuePseudoRules;
    WillBeHeapVector<RuleData> m_focusPseudoClassRules;
    WillBeHeapVector<RuleData> m_universalRules;
    WillBeHeapVector<RuleData> m_shadowHostRules;
    RuleFeatureSet m_features;
    WillBeHeapVector<RawPtrWillBeMember<StyleRulePage>> m_pageRules;
    WillBeHeapVector<RawPtrWillBeMember<StyleRuleViewport>> m_viewportRules;
    WillBeHeapVector<RawPtrWillBeMember<StyleRuleFontFace>> m_fontFaceRules;
    WillBeHeapVector<RawPtrWillBeMember<StyleRuleKeyframes>> m_keyframesRules;
    WillBeHeapVector<MinimalRuleData> m_treeBoundaryCrossingRules;
    WillBeHeapVector<MinimalRuleData> m_shadowDistributedRules;

    MediaQueryResultList m_viewportDependentMediaQueryResults;

    unsigned m_ruleCount;
    OwnPtrWillBeMember<PendingRuleMaps> m_pendingRules;

#ifndef NDEBUG
    WillBeHeapVector<RuleData> m_allRules;
#endif
};

} // namespace blink

WTF_ALLOW_MOVE_AND_INIT_WITH_MEM_FUNCTIONS(blink::RuleData);
WTF_ALLOW_MOVE_AND_INIT_WITH_MEM_FUNCTIONS(blink::MinimalRuleData);

#endif // RuleSet_h
