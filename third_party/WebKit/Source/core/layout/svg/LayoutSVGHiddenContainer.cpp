/*
 * Copyright (C) 2007 Eric Seidel <eric@webkit.org>
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
#include "core/layout/svg/LayoutSVGHiddenContainer.h"

#include "core/layout/LayoutAnalyzer.h"
#include "core/layout/svg/SVGLayoutSupport.h"

namespace blink {

LayoutSVGHiddenContainer::LayoutSVGHiddenContainer(SVGElement* element)
    : LayoutSVGContainer(element)
{
}

void LayoutSVGHiddenContainer::layout()
{
    ASSERT(needsLayout());
    LayoutAnalyzer::Scope analyzer(*this);
    SVGLayoutSupport::layoutChildren(this, selfNeedsLayout());
    updateCachedBoundaries();
    clearNeedsLayout();
}

void LayoutSVGHiddenContainer::paint(const PaintInfo&, const LayoutPoint&)
{
    // This subtree does not paint.
}

void LayoutSVGHiddenContainer::absoluteQuads(Vector<FloatQuad>&, bool*) const
{
    // This subtree does not take up space or paint
}

bool LayoutSVGHiddenContainer::nodeAtFloatPoint(HitTestResult&, const FloatPoint&, HitTestAction)
{
    return false;
}

}
