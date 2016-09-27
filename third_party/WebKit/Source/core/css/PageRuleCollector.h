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

#ifndef PageRuleCollector_h
#define PageRuleCollector_h

#include "core/css/resolver/MatchResult.h"
#include "wtf/Vector.h"

namespace blink {

class StyleRulePage;

class PageRuleCollector {
    STACK_ALLOCATED();
public:
    PageRuleCollector(const ComputedStyle* rootElementStyle, int pageIndex);

    void matchPageRules(RuleSet* rules);
    const MatchResult& matchedResult() { return m_result; }

private:
    bool isLeftPage(const ComputedStyle* rootElementStyle, int pageIndex) const;
    bool isRightPage(const ComputedStyle* rootElementStyle, int pageIndex) const { return !isLeftPage(rootElementStyle, pageIndex); }
    bool isFirstPage(int pageIndex) const;
    String pageName(int pageIndex) const;

    void matchPageRulesForList(WillBeHeapVector<RawPtrWillBeMember<StyleRulePage>>& matchedRules, const WillBeHeapVector<RawPtrWillBeMember<StyleRulePage>>& rules, bool isLeftPage, bool isFirstPage, const String& pageName);

    const bool m_isLeftPage;
    const bool m_isFirstPage;
    const String m_pageName;

    MatchResult m_result;
};

} // namespace blink

#endif // PageRuleCollector_h
