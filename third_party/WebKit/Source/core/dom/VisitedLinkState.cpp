/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 2004-2005 Allan Sandfeld Jensen (kde@carewolf.com)
 * Copyright (C) 2006, 2007 Nicholas Shanks (webkit@nickshanks.com)
 * Copyright (C) 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
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
#include "core/dom/VisitedLinkState.h"

#include "core/HTMLNames.h"
#include "core/XLinkNames.h"
#include "core/dom/ElementTraversal.h"
#include "core/html/HTMLAnchorElement.h"
#include "public/platform/Platform.h"

namespace blink {

static inline const AtomicString& linkAttribute(const Element& element)
{
    ASSERT(element.isLink());
    if (element.isHTMLElement())
        return element.fastGetAttribute(HTMLNames::hrefAttr);
    ASSERT(element.isSVGElement());
    return element.getAttribute(XLinkNames::hrefAttr);
}

static inline LinkHash linkHashForElement(const Element& element, const AtomicString& attribute = AtomicString())
{
    ASSERT(attribute.isNull() || linkAttribute(element) == attribute);
    if (isHTMLAnchorElement(element))
        return toHTMLAnchorElement(element).visitedLinkHash();
    return visitedLinkHash(element.document().baseURL(), attribute.isNull() ? linkAttribute(element) : attribute);
}

VisitedLinkState::VisitedLinkState(const Document& document)
    : m_document(document)
{
}

void VisitedLinkState::invalidateStyleForAllLinks()
{
    if (m_linksCheckedForVisitedState.isEmpty())
        return;
    for (Node& node : NodeTraversal::startsAt(document().firstChild())) {
        if (node.isLink())
            node.setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::VisitedLink));
    }
}

void VisitedLinkState::invalidateStyleForLink(LinkHash linkHash)
{
    if (!m_linksCheckedForVisitedState.contains(linkHash))
        return;
    for (Node& node : NodeTraversal::startsAt(document().firstChild())) {
        if (node.isLink() && linkHashForElement(toElement(node)) == linkHash)
            node.setNeedsStyleRecalc(SubtreeStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::VisitedLink));
    }
}

EInsideLink VisitedLinkState::determineLinkStateSlowCase(const Element& element)
{
    ASSERT(element.isLink());
    ASSERT(document().isActive());
    ASSERT(document() == element.document());

    const AtomicString& attribute = linkAttribute(element);

    if (attribute.isNull())
        return NotInsideLink; // This can happen for <img usemap>

    // An empty attribute refers to the document itself which is always
    // visited. It is useful to check this explicitly so that visited
    // links can be tested in platform independent manner, without
    // explicit support in the test harness.
    if (attribute.isEmpty())
        return InsideVisitedLink;

    if (LinkHash hash = linkHashForElement(element, attribute)) {
        m_linksCheckedForVisitedState.add(hash);
        if (Platform::current()->isLinkVisited(hash))
            return InsideVisitedLink;
    }

    return InsideUnvisitedLink;
}

DEFINE_TRACE(VisitedLinkState)
{
    visitor->trace(m_document);
}

} // namespace blink
