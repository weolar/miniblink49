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

#ifndef SelectorChecker_h
#define SelectorChecker_h

#include "core/css/CSSSelector.h"
#include "core/dom/Element.h"
#include "platform/scroll/ScrollTypes.h"

namespace blink {

class CSSSelector;
class ContainerNode;
class Element;
class LayoutScrollbar;
class ComputedStyle;

class SelectorChecker {
    WTF_MAKE_NONCOPYABLE(SelectorChecker);
public:
    enum VisitedMatchType { VisitedMatchDisabled, VisitedMatchEnabled };
    enum Mode { ResolvingStyle = 0, CollectingStyleRules, CollectingCSSRules, QueryingRules, SharingRules };

    explicit SelectorChecker(Mode);

    struct SelectorCheckingContext {
        STACK_ALLOCATED();
    public:
        // Initial selector constructor
        SelectorCheckingContext(Element* element, VisitedMatchType visitedMatchType)
            : selector(0)
            , element(element)
            , previousElement(nullptr)
            , scope(nullptr)
            , visitedMatchType(visitedMatchType)
            , pseudoId(NOPSEUDO)
            , elementStyle(nullptr)
            , scrollbar(nullptr)
            , scrollbarPart(NoPart)
            , isSubSelector(false)
            , inRightmostCompound(true)
            , hasScrollbarPseudo(false)
            , hasSelectionPseudo(false)
            , isUARule(false)
            , scopeContainsLastMatchedElement(false)
            , treatShadowHostAsNormalScope(false)
        {
        }

        const CSSSelector* selector;
        RawPtrWillBeMember<Element> element;
        RawPtrWillBeMember<Element> previousElement;
        RawPtrWillBeMember<const ContainerNode> scope;
        VisitedMatchType visitedMatchType;
        PseudoId pseudoId;
        ComputedStyle* elementStyle;
        RawPtrWillBeMember<LayoutScrollbar> scrollbar;
        ScrollbarPart scrollbarPart;
        bool isSubSelector;
        bool inRightmostCompound;
        bool hasScrollbarPseudo;
        bool hasSelectionPseudo;
        bool isUARule;
        bool scopeContainsLastMatchedElement;
        bool treatShadowHostAsNormalScope;
    };

    struct MatchResult {
        MatchResult()
            : dynamicPseudo(NOPSEUDO)
            , specificity(0) { }

        PseudoId dynamicPseudo;
        unsigned specificity;
    };

    bool match(const SelectorCheckingContext&, MatchResult&) const;
    bool match(const SelectorCheckingContext&) const;

    static bool matchesFocusPseudoClass(const Element&);

private:
    bool checkOne(const SelectorCheckingContext&, MatchResult&) const;

    enum Match { SelectorMatches, SelectorFailsLocally, SelectorFailsAllSiblings, SelectorFailsCompletely };

    Match matchSelector(const SelectorCheckingContext&, MatchResult&) const;
    Match matchForSubSelector(const SelectorCheckingContext&, MatchResult&) const;
    Match matchForRelation(const SelectorCheckingContext&, MatchResult&) const;
    Match matchForShadowDistributed(const SelectorCheckingContext&, const Element&, MatchResult&) const;
    Match matchForPseudoShadow(const SelectorCheckingContext&, const ContainerNode*, MatchResult&) const;
    bool checkPseudoClass(const SelectorCheckingContext&, MatchResult&) const;
    bool checkPseudoElement(const SelectorCheckingContext&, MatchResult&) const;
    bool checkScrollbarPseudoClass(const SelectorCheckingContext&, MatchResult&) const;
    bool checkPseudoHost(const SelectorCheckingContext&, MatchResult&) const;
    bool checkPseudoNot(const SelectorCheckingContext&, MatchResult&) const;

    Mode m_mode;
};

}

#endif
