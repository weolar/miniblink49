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
#include "core/css/TreeBoundaryCrossingRules.h"

#include "core/css/ElementRuleCollector.h"
#include "core/css/StylePropertySet.h"
#include "core/css/resolver/ScopedStyleResolver.h"

namespace blink {

void TreeBoundaryCrossingRules::collectTreeBoundaryCrossingRules(Element* element, ElementRuleCollector& collector, bool includeEmptyRules)
{
    if (m_scopingNodes.isEmpty())
        return;

    // When comparing rules declared in outer treescopes, outer's rules win.
    CascadeOrder outerCascadeOrder = size() + size();
    // When comparing rules declared in inner treescopes, inner's rules win.
    CascadeOrder innerCascadeOrder = size();

    ASSERT(!collector.scopeContainsLastMatchedElement());
    collector.setScopeContainsLastMatchedElement(true);

    for (const auto& scope : m_scopingNodes) {

        bool isInnerTreeScope = element->treeScope().isInclusiveAncestorOf(scope->treeScope());
        CascadeOrder cascadeOrder = isInnerTreeScope ? innerCascadeOrder : outerCascadeOrder;

        scope->treeScope().scopedStyleResolver()->collectMatchingTreeBoundaryCrossingRules(collector, includeEmptyRules, cascadeOrder);

        ++innerCascadeOrder;
        --outerCascadeOrder;
    }

    collector.setScopeContainsLastMatchedElement(false);
}

void TreeBoundaryCrossingRules::addScope(ContainerNode& scopingNode)
{
    m_scopingNodes.add(&scopingNode);
}

void TreeBoundaryCrossingRules::removeScope(const ContainerNode& scopingNode)
{
    m_scopingNodes.remove(&scopingNode);
}

DEFINE_TRACE(TreeBoundaryCrossingRules)
{
#if ENABLE(OILPAN)
    visitor->trace(m_scopingNodes);
#endif
}

} // namespace blink
