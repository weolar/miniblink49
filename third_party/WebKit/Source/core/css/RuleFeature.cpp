/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 2004-2005 Allan Sandfeld Jensen (kde@carewolf.com)
 * Copyright (C) 2006, 2007 Nicholas Shanks (webkit@nickshanks.com)
 * Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012 Apple Inc. All rights reserved.
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
#include "core/css/RuleFeature.h"

#include "core/HTMLNames.h"
#include "core/css/CSSSelector.h"
#include "core/css/CSSSelectorList.h"
#include "core/css/CSSValueList.h"
#include "core/css/RuleSet.h"
#include "core/css/StylePropertySet.h"
#include "core/css/StyleRule.h"
#include "core/css/invalidation/DescendantInvalidationSet.h"
#include "core/dom/Element.h"
#include "core/dom/Node.h"
#include "core/inspector/InspectorTraceEvents.h"
#include "wtf/BitVector.h"

namespace blink {

#if ENABLE(ASSERT)

static bool supportsInvalidation(CSSSelector::Match match)
{
    switch (match) {
    case CSSSelector::Tag:
    case CSSSelector::Id:
    case CSSSelector::Class:
    case CSSSelector::AttributeExact:
    case CSSSelector::AttributeSet:
    case CSSSelector::AttributeHyphen:
    case CSSSelector::AttributeList:
    case CSSSelector::AttributeContain:
    case CSSSelector::AttributeBegin:
    case CSSSelector::AttributeEnd:
        return true;
    case CSSSelector::Unknown:
    case CSSSelector::PagePseudoClass:
        // These should not appear in StyleRule selectors.
        ASSERT_NOT_REACHED();
        return false;
    default:
        // New match type added. Figure out if it needs a subtree invalidation or not.
        ASSERT_NOT_REACHED();
        return false;
    }
}

static bool supportsInvalidation(CSSSelector::PseudoType type)
{
    switch (type) {
    case CSSSelector::PseudoEmpty:
    case CSSSelector::PseudoFirstChild:
    case CSSSelector::PseudoFirstOfType:
    case CSSSelector::PseudoLastChild:
    case CSSSelector::PseudoLastOfType:
    case CSSSelector::PseudoOnlyChild:
    case CSSSelector::PseudoOnlyOfType:
    case CSSSelector::PseudoNthChild:
    case CSSSelector::PseudoNthOfType:
    case CSSSelector::PseudoNthLastChild:
    case CSSSelector::PseudoNthLastOfType:
    case CSSSelector::PseudoLink:
    case CSSSelector::PseudoVisited:
    case CSSSelector::PseudoAny:
    case CSSSelector::PseudoAnyLink:
    case CSSSelector::PseudoAutofill:
    case CSSSelector::PseudoHover:
    case CSSSelector::PseudoDrag:
    case CSSSelector::PseudoFocus:
    case CSSSelector::PseudoActive:
    case CSSSelector::PseudoChecked:
    case CSSSelector::PseudoEnabled:
    case CSSSelector::PseudoFullPageMedia:
    case CSSSelector::PseudoDefault:
    case CSSSelector::PseudoDisabled:
    case CSSSelector::PseudoOptional:
    case CSSSelector::PseudoRequired:
    case CSSSelector::PseudoReadOnly:
    case CSSSelector::PseudoReadWrite:
    case CSSSelector::PseudoValid:
    case CSSSelector::PseudoInvalid:
    case CSSSelector::PseudoIndeterminate:
    case CSSSelector::PseudoTarget:
    case CSSSelector::PseudoBefore:
    case CSSSelector::PseudoAfter:
    case CSSSelector::PseudoBackdrop:
    case CSSSelector::PseudoLang:
    case CSSSelector::PseudoNot:
    case CSSSelector::PseudoResizer:
    case CSSSelector::PseudoRoot:
    case CSSSelector::PseudoScope:
    case CSSSelector::PseudoScrollbar:
    case CSSSelector::PseudoScrollbarButton:
    case CSSSelector::PseudoScrollbarCorner:
    case CSSSelector::PseudoScrollbarThumb:
    case CSSSelector::PseudoScrollbarTrack:
    case CSSSelector::PseudoScrollbarTrackPiece:
    case CSSSelector::PseudoWindowInactive:
    case CSSSelector::PseudoSelection:
    case CSSSelector::PseudoCornerPresent:
    case CSSSelector::PseudoDecrement:
    case CSSSelector::PseudoIncrement:
    case CSSSelector::PseudoHorizontal:
    case CSSSelector::PseudoVertical:
    case CSSSelector::PseudoStart:
    case CSSSelector::PseudoEnd:
    case CSSSelector::PseudoDoubleButton:
    case CSSSelector::PseudoSingleButton:
    case CSSSelector::PseudoNoButton:
    case CSSSelector::PseudoFullScreen:
    case CSSSelector::PseudoFullScreenDocument:
    case CSSSelector::PseudoFullScreenAncestor:
    case CSSSelector::PseudoInRange:
    case CSSSelector::PseudoOutOfRange:
    case CSSSelector::PseudoWebKitCustomElement:
    case CSSSelector::PseudoCue:
    case CSSSelector::PseudoFutureCue:
    case CSSSelector::PseudoPastCue:
    case CSSSelector::PseudoUnresolved:
    case CSSSelector::PseudoContent:
    case CSSSelector::PseudoHost:
    case CSSSelector::PseudoShadow:
    case CSSSelector::PseudoSpatialNavigationFocus:
    case CSSSelector::PseudoListBox:
        return true;
    case CSSSelector::PseudoUnknown:
    case CSSSelector::PseudoLeftPage:
    case CSSSelector::PseudoRightPage:
    case CSSSelector::PseudoFirstPage:
        // These should not appear in StyleRule selectors.
        ASSERT_NOT_REACHED();
        return false;
    default:
        // New pseudo type added. Figure out if it needs a subtree invalidation or not.
        ASSERT_NOT_REACHED();
        return false;
    }
}

static bool supportsInvalidationWithSelectorList(CSSSelector::PseudoType pseudo)
{
    return pseudo == CSSSelector::PseudoAny
        || pseudo == CSSSelector::PseudoCue
        || pseudo == CSSSelector::PseudoHost
        || pseudo == CSSSelector::PseudoHostContext
        || pseudo == CSSSelector::PseudoNot;
}

#endif // ENABLE(ASSERT)

static bool requiresSubtreeInvalidation(const CSSSelector& selector)
{
    if (selector.match() != CSSSelector::PseudoElement && selector.match() != CSSSelector::PseudoClass) {
        ASSERT(supportsInvalidation(selector.match()));
        return false;
    }

    switch (selector.pseudoType()) {
    case CSSSelector::PseudoFirstLine:
    case CSSSelector::PseudoFirstLetter:
        // FIXME: Most pseudo classes/elements above can be supported and moved
        // to assertSupportedPseudo(). Move on a case-by-case basis. If they
        // require subtree invalidation, document why.
    case CSSSelector::PseudoHostContext:
        // :host-context matches a shadow host, yet the simple selectors inside
        // :host-context matches an ancestor of the shadow host.
        return true;
    default:
        ASSERT(supportsInvalidation(selector.pseudoType()));
        return false;
    }
}

RuleFeature::RuleFeature(StyleRule* rule, unsigned selectorIndex, bool hasDocumentSecurityOrigin)
    : rule(rule)
    , selectorIndex(selectorIndex)
    , hasDocumentSecurityOrigin(hasDocumentSecurityOrigin)
{
}

DEFINE_TRACE(RuleFeature)
{
    visitor->trace(rule);
}

bool RuleFeatureSet::extractInvalidationSetFeature(const CSSSelector& selector, InvalidationSetFeatures& features)
{
    if (selector.match() == CSSSelector::Tag && selector.tagQName().localName() != starAtom)
        features.tagName = selector.tagQName().localName();
    else if (selector.match() == CSSSelector::Id)
        features.id = selector.value();
    else if (selector.match() == CSSSelector::Class)
        features.classes.append(selector.value());
    else if (selector.isAttributeSelector())
        features.attributes.append(selector.attribute().localName());
    else if (selector.pseudoType() == CSSSelector::PseudoWebKitCustomElement)
        features.customPseudoElement = true;
    else if (selector.pseudoType() == CSSSelector::PseudoBefore || selector.pseudoType() == CSSSelector::PseudoAfter)
        features.hasBeforeOrAfter = true;
    else
        return false;
    return true;
}

RuleFeatureSet::RuleFeatureSet()
{
}

RuleFeatureSet::~RuleFeatureSet()
{
}

DescendantInvalidationSet* RuleFeatureSet::invalidationSetForSelector(const CSSSelector& selector)
{
    if (selector.match() == CSSSelector::Class)
        return &ensureClassInvalidationSet(selector.value());
    if (selector.isAttributeSelector())
        return &ensureAttributeInvalidationSet(selector.attribute().localName());
    if (selector.match() == CSSSelector::Id)
        return &ensureIdInvalidationSet(selector.value());
    if (selector.match() == CSSSelector::PseudoClass) {
        switch (selector.pseudoType()) {
        case CSSSelector::PseudoEmpty:
        case CSSSelector::PseudoLink:
        case CSSSelector::PseudoVisited:
        case CSSSelector::PseudoAnyLink:
        case CSSSelector::PseudoAutofill:
        case CSSSelector::PseudoHover:
        case CSSSelector::PseudoFocus:
        case CSSSelector::PseudoActive:
        case CSSSelector::PseudoChecked:
        case CSSSelector::PseudoEnabled:
        case CSSSelector::PseudoDisabled:
        case CSSSelector::PseudoOptional:
        case CSSSelector::PseudoRequired:
        case CSSSelector::PseudoValid:
        case CSSSelector::PseudoInvalid:
        case CSSSelector::PseudoIndeterminate:
        case CSSSelector::PseudoTarget:
            return &ensurePseudoInvalidationSet(selector.pseudoType());
        default:
            break;
        }
    }
    return nullptr;
}

// Given a rule, update the descendant invalidation sets for the features found
// in its selector. The first step is to extract the features from the rightmost
// compound selector (extractInvalidationSetFeatures). Secondly, add those features
// to the invalidation sets for the features found in the other compound selectors
// (addFeaturesToInvalidationSets). If we find a feature in the right-most compound
// selector that requires a subtree recalc, we addFeaturesToInvalidationSets for the
// rightmost compound selector as well.

void RuleFeatureSet::updateInvalidationSets(const RuleData& ruleData)
{
    InvalidationSetFeatures features;
    auto result = extractInvalidationSetFeatures(ruleData.selector(), features, false);
    if (result.first) {
        features.forceSubtree = result.second == ForceSubtree;
        addFeaturesToInvalidationSets(*result.first, features);
    }

    // If any ::before and ::after rules specify 'content: attr(...)', we
    // need to create invalidation sets for those attributes.
    if (features.hasBeforeOrAfter)
        updateInvalidationSetsForContentAttribute(ruleData);
}

void RuleFeatureSet::updateInvalidationSetsForContentAttribute(const RuleData& ruleData)
{
    const StylePropertySet& propertySet = ruleData.rule()->properties();

    int propertyIndex = propertySet.findPropertyIndex(CSSPropertyContent);

    if (propertyIndex == -1)
        return;

    StylePropertySet::PropertyReference contentProperty = propertySet.propertyAt(propertyIndex);
    CSSValue* contentValue = contentProperty.value();

    if (!contentValue->isValueList())
        return;

    for (auto& item : toCSSValueList(*contentValue)) {
        if (!item->isPrimitiveValue())
            continue;
        CSSPrimitiveValue* primitiveItem = toCSSPrimitiveValue(item.get());
        if (!primitiveItem->isAttr())
            continue;
        ensureAttributeInvalidationSet(AtomicString(primitiveItem->getStringValue()));
    }
}

std::pair<const CSSSelector*, RuleFeatureSet::UseFeaturesType>
RuleFeatureSet::extractInvalidationSetFeatures(const CSSSelector& selector, InvalidationSetFeatures& features, bool negated)
{
    bool foundFeatures = false;
    for (const CSSSelector* current = &selector; current; current = current->tagHistory()) {
        if (!negated)
            foundFeatures |= extractInvalidationSetFeature(*current, features);
        // Initialize the entry in the invalidation set map, if supported.
        if (!invalidationSetForSelector(*current)) {
            if (requiresSubtreeInvalidation(*current)) {
                // Fall back to use subtree invalidations, even for features in the
                // rightmost compound selector. Returning the start &selector here
                // will make addFeaturesToInvalidationSets start marking invalidation
                // sets for subtree recalc for features in the rightmost compound
                // selector.
                return std::make_pair(&selector, ForceSubtree);
            }
            if (const CSSSelectorList* selectorList = current->selectorList()) {
                ASSERT(supportsInvalidationWithSelectorList(current->pseudoType()));
                const CSSSelector* subSelector = selectorList->first();
                bool allSubSelectorsHaveFeatures = !!subSelector;
                for (; subSelector; subSelector = CSSSelectorList::next(*subSelector)) {
                    auto result = extractInvalidationSetFeatures(*subSelector, features, current->pseudoType() == CSSSelector::PseudoNot);
                    if (result.first) {
                        // A non-null selector return means the sub-selector contained a
                        // selector which requiresSubtreeInvalidation(). Return the rightmost
                        // selector to mark for subtree recalcs like above.
                        return std::make_pair(&selector, ForceSubtree);
                    }
                    allSubSelectorsHaveFeatures &= result.second == UseFeatures;
                }
                foundFeatures |= allSubSelectorsHaveFeatures;
            }
        }

        if (current->relation() == CSSSelector::SubSelector)
            continue;

        features.treeBoundaryCrossing = current->isShadowSelector();
        features.adjacent = current->isAdjacentSelector();
        return std::make_pair(current->tagHistory(), foundFeatures ? UseFeatures : ForceSubtree);
    }
    return std::make_pair(nullptr,  foundFeatures ? UseFeatures : ForceSubtree);
}

// Add features extracted from the rightmost compound selector to descendant invalidation
// sets for features found in other compound selectors.
//
// Style invalidation is currently supported for descendants only, not for sibling subtrees.
// We use wholeSubtree invalidation for features found left of adjacent combinators as
// SubtreeStyleChange will force sibling subtree recalc in
// ContainerNode::checkForChildrenAdjacentRuleChanges.
//
// As we encounter a descendant type of combinator, the features only need to be checked
// against descendants in the same subtree only. features.adjacent is set to false, and
// we start adding features instead of calling setWholeSubtreeInvalid.

void RuleFeatureSet::addFeaturesToInvalidationSet(DescendantInvalidationSet& invalidationSet, const InvalidationSetFeatures& features)
{
    if (features.treeBoundaryCrossing)
        invalidationSet.setTreeBoundaryCrossing();
    if (features.insertionPointCrossing)
        invalidationSet.setInsertionPointCrossing();
    if (features.useSubtreeInvalidation()) {
        invalidationSet.setWholeSubtreeInvalid();
        return;
    }
    if (!features.id.isEmpty())
        invalidationSet.addId(features.id);
    if (!features.tagName.isEmpty())
        invalidationSet.addTagName(features.tagName);
    for (const auto& className : features.classes)
        invalidationSet.addClass(className);
    for (const auto& attribute : features.attributes)
        invalidationSet.addAttribute(attribute);
    if (features.customPseudoElement)
        invalidationSet.setCustomPseudoInvalid();
}

void RuleFeatureSet::addFeaturesToInvalidationSets(const CSSSelector& selector, InvalidationSetFeatures& features)
{
    for (const CSSSelector* current = &selector; current; current = current->tagHistory()) {
        if (DescendantInvalidationSet* invalidationSet = invalidationSetForSelector(*current)) {
            addFeaturesToInvalidationSet(*invalidationSet, features);
        } else {
            if (current->isTreeBoundaryCrossing())
                features.treeBoundaryCrossing = true;
            if (current->isInsertionPointCrossing())
                features.insertionPointCrossing = true;
            if (const CSSSelectorList* selectorList = current->selectorList()) {
                ASSERT(supportsInvalidationWithSelectorList(current->pseudoType()));
                for (const CSSSelector* selector = selectorList->first(); selector; selector = CSSSelectorList::next(*selector))
                    addFeaturesToInvalidationSets(*selector, features);
            }
        }

        if (current->relation() == CSSSelector::SubSelector)
            continue;

        if (current->isShadowSelector())
            features.treeBoundaryCrossing = true;

        features.adjacent = current->isAdjacentSelector();
    }
}

void RuleFeatureSet::collectFeaturesFromRuleData(const RuleData& ruleData)
{
    updateInvalidationSets(ruleData);

    FeatureMetadata metadata;
    collectFeaturesFromSelector(ruleData.selector(), metadata);
    m_metadata.add(metadata);

    if (metadata.foundSiblingSelector)
        siblingRules.append(RuleFeature(ruleData.rule(), ruleData.selectorIndex(), ruleData.hasDocumentSecurityOrigin()));
    if (ruleData.containsUncommonAttributeSelector())
        uncommonAttributeRules.append(RuleFeature(ruleData.rule(), ruleData.selectorIndex(), ruleData.hasDocumentSecurityOrigin()));
}

DescendantInvalidationSet& RuleFeatureSet::ensureClassInvalidationSet(const AtomicString& className)
{
    InvalidationSetMap::AddResult addResult = m_classInvalidationSets.add(className, nullptr);
    if (addResult.isNewEntry)
        addResult.storedValue->value = DescendantInvalidationSet::create();
    return *addResult.storedValue->value;
}

DescendantInvalidationSet& RuleFeatureSet::ensureAttributeInvalidationSet(const AtomicString& attributeName)
{
    InvalidationSetMap::AddResult addResult = m_attributeInvalidationSets.add(attributeName, nullptr);
    if (addResult.isNewEntry)
        addResult.storedValue->value = DescendantInvalidationSet::create();
    return *addResult.storedValue->value;
}

DescendantInvalidationSet& RuleFeatureSet::ensureIdInvalidationSet(const AtomicString& id)
{
    InvalidationSetMap::AddResult addResult = m_idInvalidationSets.add(id, nullptr);
    if (addResult.isNewEntry)
        addResult.storedValue->value = DescendantInvalidationSet::create();
    return *addResult.storedValue->value;
}

DescendantInvalidationSet& RuleFeatureSet::ensurePseudoInvalidationSet(CSSSelector::PseudoType pseudoType)
{
    PseudoTypeInvalidationSetMap::AddResult addResult = m_pseudoInvalidationSets.add(pseudoType, nullptr);
    if (addResult.isNewEntry)
        addResult.storedValue->value = DescendantInvalidationSet::create();
    return *addResult.storedValue->value;
}

void RuleFeatureSet::collectFeaturesFromSelector(const CSSSelector& selector, RuleFeatureSet::FeatureMetadata& metadata)
{
    unsigned maxDirectAdjacentSelectors = 0;

    for (const CSSSelector* current = &selector; current; current = current->tagHistory()) {
        if (current->pseudoType() == CSSSelector::PseudoFirstLine)
            metadata.usesFirstLineRules = true;
        if (current->pseudoType() == CSSSelector::PseudoWindowInactive)
            metadata.usesWindowInactiveSelector = true;
        if (current->relation() == CSSSelector::DirectAdjacent) {
            maxDirectAdjacentSelectors++;
        } else if (maxDirectAdjacentSelectors
            && ((current->relation() != CSSSelector::SubSelector) || current->isLastInTagHistory())) {
            if (maxDirectAdjacentSelectors > metadata.maxDirectAdjacentSelectors)
                metadata.maxDirectAdjacentSelectors = maxDirectAdjacentSelectors;
            maxDirectAdjacentSelectors = 0;
        }
        if (current->isSiblingSelector())
            metadata.foundSiblingSelector = true;

        const CSSSelectorList* selectorList = current->selectorList();
        if (!selectorList)
            continue;

        for (const CSSSelector* selector = selectorList->first(); selector; selector = CSSSelectorList::next(*selector))
            collectFeaturesFromSelector(*selector, metadata);
    }

    ASSERT(!maxDirectAdjacentSelectors);
}

void RuleFeatureSet::FeatureMetadata::add(const FeatureMetadata& other)
{
    usesFirstLineRules = usesFirstLineRules || other.usesFirstLineRules;
    usesWindowInactiveSelector = usesWindowInactiveSelector || other.usesWindowInactiveSelector;
    maxDirectAdjacentSelectors = std::max(maxDirectAdjacentSelectors, other.maxDirectAdjacentSelectors);
}

void RuleFeatureSet::FeatureMetadata::clear()
{
    usesFirstLineRules = false;
    usesWindowInactiveSelector = false;
    foundSiblingSelector = false;
    maxDirectAdjacentSelectors = 0;
}

void RuleFeatureSet::add(const RuleFeatureSet& other)
{
    for (const auto& invalidationSet : other.m_classInvalidationSets)
        ensureClassInvalidationSet(invalidationSet.key).combine(*invalidationSet.value);
    for (const auto& invalidationSet : other.m_attributeInvalidationSets)
        ensureAttributeInvalidationSet(invalidationSet.key).combine(*invalidationSet.value);
    for (const auto& invalidationSet : other.m_idInvalidationSets)
        ensureIdInvalidationSet(invalidationSet.key).combine(*invalidationSet.value);
    for (const auto& invalidationSet : other.m_pseudoInvalidationSets)
        ensurePseudoInvalidationSet(static_cast<CSSSelector::PseudoType>(invalidationSet.key)).combine(*invalidationSet.value);

    m_metadata.add(other.m_metadata);

    siblingRules.appendVector(other.siblingRules);
    uncommonAttributeRules.appendVector(other.uncommonAttributeRules);
}

void RuleFeatureSet::clear()
{
    siblingRules.clear();
    uncommonAttributeRules.clear();
    m_metadata.clear();
    m_classInvalidationSets.clear();
    m_attributeInvalidationSets.clear();
    m_idInvalidationSets.clear();
}

void RuleFeatureSet::collectInvalidationSetsForClass(InvalidationSetVector& invalidationSets, Element& element, const AtomicString& className) const
{
    if (RefPtrWillBeRawPtr<DescendantInvalidationSet> invalidationSet = m_classInvalidationSets.get(className)) {
        TRACE_SCHEDULE_STYLE_INVALIDATION(element, *invalidationSet, classChange, className);
        invalidationSets.append(invalidationSet);
    }
}

void RuleFeatureSet::collectInvalidationSetsForId(InvalidationSetVector& invalidationSets, Element& element, const AtomicString& id) const
{
    if (RefPtrWillBeRawPtr<DescendantInvalidationSet> invalidationSet = m_idInvalidationSets.get(id)) {
        TRACE_SCHEDULE_STYLE_INVALIDATION(element, *invalidationSet, idChange, id);
        invalidationSets.append(invalidationSet);
    }
}

void RuleFeatureSet::collectInvalidationSetsForAttribute(InvalidationSetVector& invalidationSets, Element& element, const QualifiedName& attributeName) const
{
    if (RefPtrWillBeRawPtr<DescendantInvalidationSet> invalidationSet = m_attributeInvalidationSets.get(attributeName.localName())) {
        TRACE_SCHEDULE_STYLE_INVALIDATION(element, *invalidationSet, attributeChange, attributeName);
        invalidationSets.append(invalidationSet);
    }
}

void RuleFeatureSet::collectInvalidationSetsForPseudoClass(InvalidationSetVector& invalidationSets, Element& element, CSSSelector::PseudoType pseudo) const
{
    if (RefPtrWillBeRawPtr<DescendantInvalidationSet> invalidationSet = m_pseudoInvalidationSets.get(pseudo)) {
        TRACE_SCHEDULE_STYLE_INVALIDATION(element, *invalidationSet, pseudoChange, pseudo);
        invalidationSets.append(invalidationSet);
    }
}

DEFINE_TRACE(RuleFeatureSet)
{
#if ENABLE(OILPAN)
    visitor->trace(siblingRules);
    visitor->trace(uncommonAttributeRules);
    visitor->trace(m_classInvalidationSets);
    visitor->trace(m_attributeInvalidationSets);
    visitor->trace(m_idInvalidationSets);
    visitor->trace(m_pseudoInvalidationSets);
#endif
}

} // namespace blink
