/**
 * Copyright (C) 2000 Lars Knoll (knoll@kde.org)
 * Copyright (C) 2006 Apple Computer, Inc.
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

#include "config.h"
#include "core/layout/LayoutBR.h"

#include "core/dom/Document.h"
#include "core/dom/StyleEngine.h"
#include "core/editing/PositionWithAffinity.h"
#include "core/layout/LayoutView.h"

namespace blink {

static PassRefPtr<StringImpl> newlineString()
{
    DEFINE_STATIC_LOCAL(const String, string, ("\n"));
    return string.impl();
}

LayoutBR::LayoutBR(Node* node)
    : LayoutText(node, newlineString())
{
}

LayoutBR::~LayoutBR()
{
}

int LayoutBR::lineHeight(bool firstLine) const
{
    const ComputedStyle& style = styleRef(firstLine && document().styleEngine().usesFirstLineRules());
    return style.computedLineHeight();
}

void LayoutBR::styleDidChange(StyleDifference diff, const ComputedStyle* oldStyle)
{
    LayoutText::styleDidChange(diff, oldStyle);
}

int LayoutBR::caretMinOffset() const
{
    return 0;
}

int LayoutBR::caretMaxOffset() const
{
    return 1;
}

PositionWithAffinity LayoutBR::positionForPoint(const LayoutPoint&)
{
    return createPositionWithAffinity(0, DOWNSTREAM);
}

} // namespace blink
