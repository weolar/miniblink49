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
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
#include "core/css/resolver/SharedStyleFinder.h"

#include "core/HTMLNames.h"
#include "core/XMLNames.h"
#include "core/css/resolver/StyleResolver.h"
#include "core/css/resolver/StyleResolverStats.h"
#include "core/dom/ContainerNode.h"
#include "core/dom/Document.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/Node.h"
#include "core/dom/NodeComputedStyle.h"
#include "core/dom/QualifiedName.h"
#include "core/dom/SpaceSplitString.h"
#include "core/dom/shadow/ElementShadow.h"
#include "core/dom/shadow/InsertionPoint.h"
#include "core/html/HTMLElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLOptGroupElement.h"
#include "core/html/HTMLOptionElement.h"
#include "core/style/ComputedStyle.h"
#include "core/svg/SVGElement.h"
#include "platform/TraceEvent.h"
#include "wtf/HashSet.h"
#include "wtf/text/AtomicString.h"

namespace blink {

using namespace HTMLNames;

bool SharedStyleFinder::canShareStyleWithControl(Element& candidate) const
{
    if (!isHTMLInputElement(candidate) || !isHTMLInputElement(element()))
        return false;

    HTMLInputElement& candidateInput = toHTMLInputElement(candidate);
    HTMLInputElement& thisInput = toHTMLInputElement(element());

    if (candidateInput.isAutofilled() != thisInput.isAutofilled())
        return false;
    if (candidateInput.shouldAppearChecked() != thisInput.shouldAppearChecked())
        return false;
    if (candidateInput.shouldAppearIndeterminate() != thisInput.shouldAppearIndeterminate())
        return false;
    if (candidateInput.isRequired() != thisInput.isRequired())
        return false;

    if (candidate.isDisabledFormControl() != element().isDisabledFormControl())
        return false;

    if (candidate.isDefaultButtonForForm() != element().isDefaultButtonForForm())
        return false;

    if (document().containsValidityStyleRules()) {
        bool willValidate = candidate.willValidate();

        if (willValidate != element().willValidate())
            return false;

        if (willValidate && (candidate.isValidElement() != element().isValidElement()))
            return false;

        if (candidate.isInRange() != element().isInRange())
            return false;

        if (candidate.isOutOfRange() != element().isOutOfRange())
            return false;
    }

    return true;
}

bool SharedStyleFinder::classNamesAffectedByRules(const SpaceSplitString& classNames) const
{
    unsigned count = classNames.size();
    for (unsigned i = 0; i < count; ++i) {
        if (m_features.hasSelectorForClass(classNames[i]))
            return true;
    }
    return false;
}

static inline const AtomicString& typeAttributeValue(const Element& element)
{
    // type is animatable in SVG so we need to go down the slow path here.
    return element.isSVGElement() ? element.getAttribute(typeAttr) : element.fastGetAttribute(typeAttr);
}

bool SharedStyleFinder::sharingCandidateHasIdenticalStyleAffectingAttributes(Element& candidate) const
{
    if (element().sharesSameElementData(candidate))
        return true;
    if (element().fastGetAttribute(XMLNames::langAttr) != candidate.fastGetAttribute(XMLNames::langAttr))
        return false;
    if (element().fastGetAttribute(langAttr) != candidate.fastGetAttribute(langAttr))
        return false;

    // These two checks must be here since RuleSet has a special case to allow style sharing between elements
    // with type and readonly attributes whereas other attribute selectors prevent sharing.
    if (typeAttributeValue(element()) != typeAttributeValue(candidate))
        return false;
    if (element().fastGetAttribute(readonlyAttr) != candidate.fastGetAttribute(readonlyAttr))
        return false;

    if (!m_elementAffectedByClassRules) {
        if (candidate.hasClass() && classNamesAffectedByRules(candidate.classNames()))
            return false;
    } else if (candidate.hasClass()) {
        // SVG elements require a (slow!) getAttribute comparision because "class" is an animatable attribute for SVG.
        if (element().isSVGElement()) {
            if (element().getAttribute(classAttr) != candidate.getAttribute(classAttr))
                return false;
        } else if (element().classNames() != candidate.classNames()) {
            return false;
        }
    } else {
        return false;
    }

    if (element().presentationAttributeStyle() != candidate.presentationAttributeStyle())
        return false;

    // FIXME: Consider removing this, it's unlikely we'll have so many progress elements
    // that sharing the style makes sense. Instead we should just not support style sharing
    // for them.
    if (isHTMLProgressElement(element())) {
        if (element().shouldAppearIndeterminate() != candidate.shouldAppearIndeterminate())
            return false;
    }

    if (isHTMLOptGroupElement(element()) || isHTMLOptionElement(element())) {
        if (element().isDisabledFormControl() != candidate.isDisabledFormControl())
            return false;
        if (isHTMLOptionElement(element()) && toHTMLOptionElement(element()).selected() != toHTMLOptionElement(candidate).selected())
            return false;
    }

    return true;
}

bool SharedStyleFinder::sharingCandidateCanShareHostStyles(Element& candidate) const
{
    const ElementShadow* elementShadow = element().shadow();
    const ElementShadow* candidateShadow = candidate.shadow();

    if (!elementShadow && !candidateShadow)
        return true;

    if (static_cast<bool>(elementShadow) != static_cast<bool>(candidateShadow))
        return false;

    return elementShadow->hasSameStyles(candidateShadow);
}

bool SharedStyleFinder::sharingCandidateDistributedToSameInsertionPoint(Element& candidate) const
{
    WillBeHeapVector<RawPtrWillBeMember<InsertionPoint>, 8> insertionPoints, candidateInsertionPoints;
    collectDestinationInsertionPoints(element(), insertionPoints);
    collectDestinationInsertionPoints(candidate, candidateInsertionPoints);
    if (insertionPoints.size() != candidateInsertionPoints.size())
        return false;
    for (size_t i = 0; i < insertionPoints.size(); ++i) {
        if (insertionPoints[i] != candidateInsertionPoints[i])
            return false;
    }
    return true;
}

bool SharedStyleFinder::canShareStyleWithElement(Element& candidate) const
{
    if (element() == candidate)
        return false;
    Element* parent = candidate.parentOrShadowHostElement();
    const ComputedStyle* style = candidate.computedStyle();
    if (!style)
        return false;
    if (!style->isSharable())
        return false;
    if (!parent)
        return false;
    if (element().parentOrShadowHostElement()->computedStyle() != parent->computedStyle())
        return false;
    if (candidate.tagQName() != element().tagQName())
        return false;
    if (candidate.inlineStyle())
        return false;
    if (candidate.needsStyleRecalc())
        return false;
    if (candidate.isSVGElement() && toSVGElement(candidate).animatedSMILStyleProperties())
        return false;
    if (candidate.isLink() != element().isLink())
        return false;
    if (candidate.shadowPseudoId() != element().shadowPseudoId())
        return false;
    if (!sharingCandidateHasIdenticalStyleAffectingAttributes(candidate))
        return false;
    if (candidate.additionalPresentationAttributeStyle() != element().additionalPresentationAttributeStyle())
        return false;
    if (candidate.hasID() && m_features.hasSelectorForId(candidate.idForStyleResolution()))
        return false;
    if (!sharingCandidateCanShareHostStyles(candidate))
        return false;
    if (!sharingCandidateDistributedToSameInsertionPoint(candidate))
        return false;
    if (candidate.isInTopLayer() != element().isInTopLayer())
        return false;

    bool isControl = candidate.isFormControlElement();
    ASSERT(isControl == element().isFormControlElement());
    if (isControl && !canShareStyleWithControl(candidate))
        return false;

    if (isHTMLOptionElement(candidate) && isHTMLOptionElement(element())
        && (toHTMLOptionElement(candidate).selected() != toHTMLOptionElement(element()).selected()
        || toHTMLOptionElement(candidate).spatialNavigationFocused() != toHTMLOptionElement(element()).spatialNavigationFocused()))
        return false;

    // FIXME: This line is surprisingly hot, we may wish to inline hasDirectionAuto into StyleResolver.
    if (candidate.isHTMLElement() && toHTMLElement(candidate).hasDirectionAuto())
        return false;

    if (candidate.isLink() && m_context.elementLinkState() != style->insideLink())
        return false;

    if (candidate.isUnresolvedCustomElement() != element().isUnresolvedCustomElement())
        return false;

    if (element().parentOrShadowHostElement() != parent) {
        if (!parent->isStyledElement())
            return false;
        if (parent->inlineStyle())
            return false;
        if (parent->isSVGElement() && toSVGElement(parent)->animatedSMILStyleProperties())
            return false;
        if (parent->hasID() && m_features.hasSelectorForId(parent->idForStyleResolution()))
            return false;
        if (!parent->childrenSupportStyleSharing())
            return false;
    }

    if (document().containsValidityStyleRules()) {
        if (candidate.isValidElement() != element().isValidElement())
            return false;
    }

    return true;
}

bool SharedStyleFinder::documentContainsValidCandidate() const
{
    for (Element& element : ElementTraversal::startsAt(document().documentElement())) {
        if (element.supportsStyleSharing() && canShareStyleWithElement(element))
            return true;
    }
    return false;
}

inline Element* SharedStyleFinder::findElementForStyleSharing() const
{
    StyleSharingList& styleSharingList = m_styleResolver.styleSharingList();
    for (StyleSharingList::iterator it = styleSharingList.begin(); it != styleSharingList.end(); ++it) {
        Element& candidate = **it;
        if (!canShareStyleWithElement(candidate))
            continue;
        if (it != styleSharingList.begin()) {
            // Move the element to the front of the LRU
            styleSharingList.remove(it);
            styleSharingList.prepend(&candidate);
        }
        return &candidate;
    }
    m_styleResolver.addToStyleSharingList(element());
    return 0;
}

bool SharedStyleFinder::matchesRuleSet(RuleSet* ruleSet)
{
    if (!ruleSet)
        return false;
    ElementRuleCollector collector(m_context, m_styleResolver.selectorFilter());
    return collector.hasAnyMatchingRules(ruleSet);
}

ComputedStyle* SharedStyleFinder::findSharedStyle()
{
    INCREMENT_STYLE_STATS_COUNTER(m_styleResolver, sharedStyleLookups, 1);

    if (!element().supportsStyleSharing())
        return 0;

    // Cache whether context.element() is affected by any known class selectors.
    m_elementAffectedByClassRules = element().hasClass() && classNamesAffectedByRules(element().classNames());

    Element* shareElement = findElementForStyleSharing();

    if (!shareElement) {
        if (m_styleResolver.stats() && m_styleResolver.stats()->allCountersEnabled() && documentContainsValidCandidate())
            INCREMENT_STYLE_STATS_COUNTER(m_styleResolver, sharedStyleMissed, 1);
        return 0;
    }

    INCREMENT_STYLE_STATS_COUNTER(m_styleResolver, sharedStyleFound, 1);

    if (matchesRuleSet(m_siblingRuleSet)) {
        INCREMENT_STYLE_STATS_COUNTER(m_styleResolver, sharedStyleRejectedBySiblingRules, 1);
        return 0;
    }

    if (matchesRuleSet(m_uncommonAttributeRuleSet)) {
        INCREMENT_STYLE_STATS_COUNTER(m_styleResolver, sharedStyleRejectedByUncommonAttributeRules, 1);
        return 0;
    }

    // Tracking child index requires unique style for each node. This may get set by the sibling rule match above.
    if (!element().parentElementOrShadowRoot()->childrenSupportStyleSharing()) {
        INCREMENT_STYLE_STATS_COUNTER(m_styleResolver, sharedStyleRejectedByParent, 1);
        return 0;
    }

    return shareElement->mutableComputedStyle();
}

}
