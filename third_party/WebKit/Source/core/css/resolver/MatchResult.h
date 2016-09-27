/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
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
 *
 */

#ifndef MatchResult_h
#define MatchResult_h

#include "core/css/RuleSet.h"
#include "core/css/SelectorChecker.h"
#include "platform/heap/Handle.h"
#include "wtf/RefPtr.h"
#include "wtf/Vector.h"

namespace blink {

class StylePropertySet;

struct MatchedProperties {
    ALLOW_ONLY_INLINE_ALLOCATION();
public:
    MatchedProperties();
    ~MatchedProperties();

    DECLARE_TRACE();

    RefPtrWillBeMember<StylePropertySet> properties;

    union {
        struct {
            unsigned linkMatchType : 2;
            unsigned whitelistType : 2;
        } m_types;
        // Used to make sure all memory is zero-initialized since we compute the hash over the bytes of this object.
        void* possiblyPaddedMember;
    };
};

} // namespace blink

WTF_ALLOW_MOVE_AND_INIT_WITH_MEM_FUNCTIONS(blink::MatchedProperties);

namespace blink {

class MatchResult {
    STACK_ALLOCATED();
public:
    void addMatchedProperties(const StylePropertySet* properties, unsigned linkMatchType = CSSSelector::MatchAll, PropertyWhitelistType = PropertyWhitelistNone);

    unsigned begin() const { return 0; }
    unsigned end() const { return matchedProperties.size(); }
    unsigned beginUA() const { return 0; }
    unsigned endUA() const { return uaEnd; }
    unsigned beginAuthor() const { return uaEnd; }
    unsigned endAuthor() const { return matchedProperties.size(); }

    WillBeHeapVector<MatchedProperties, 64> matchedProperties;
    unsigned uaEnd = 0;
    bool isCacheable = true;
};

inline bool operator==(const MatchedProperties& a, const MatchedProperties& b)
{
    return a.properties == b.properties && a.m_types.linkMatchType == b.m_types.linkMatchType;
}

inline bool operator!=(const MatchedProperties& a, const MatchedProperties& b)
{
    return !(a == b);
}

} // namespace blink

#endif // MatchResult_h
