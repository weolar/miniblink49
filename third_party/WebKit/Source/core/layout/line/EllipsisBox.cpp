/**
 * Copyright (C) 2003, 2006 Apple Computer, Inc.
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
#include "core/layout/line/EllipsisBox.h"

#include "core/layout/HitTestResult.h"
#include "core/layout/TextRunConstructor.h"
#include "core/layout/line/InlineTextBox.h"
#include "core/layout/line/RootInlineBox.h"
#include "core/paint/EllipsisBoxPainter.h"
#include "core/style/ShadowList.h"
#include "platform/fonts/Font.h"
#include "platform/text/TextRun.h"

namespace blink {

void EllipsisBox::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset, LayoutUnit lineTop, LayoutUnit lineBottom)
{
    EllipsisBoxPainter(*this).paint(paintInfo, paintOffset, lineTop, lineBottom);
}

IntRect EllipsisBox::selectionRect()
{
    const ComputedStyle& style = layoutObject().styleRef(isFirstLineStyle());
    const Font& font = style.font();
    return enclosingIntRect(font.selectionRectForText(constructTextRun(&layoutObject(), font, m_str, style, TextRun::AllowTrailingExpansion), IntPoint(logicalLeft(), logicalTop() + root().selectionTopAdjustedForPrecedingBlock()), root().selectionHeightAdjustedForPrecedingBlock()));
}

bool EllipsisBox::nodeAtPoint(HitTestResult& result, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, LayoutUnit lineTop, LayoutUnit lineBottom)
{
    // FIXME: the call to roundedLayoutPoint() below is temporary and should be removed once
    // the transition to LayoutUnit-based types is complete (crbug.com/321237)
    LayoutPoint adjustedLocation = accumulatedOffset + topLeft();

    LayoutPoint boxOrigin = locationIncludingFlipping();
    boxOrigin.moveBy(accumulatedOffset);
    LayoutRect boundsRect(boxOrigin, size());
    if (visibleToHitTestRequest(result.hitTestRequest()) && boundsRect.intersects(LayoutRect(HitTestLocation::rectForPoint(locationInContainer.point(), 0, 0, 0, 0)))) {
        layoutObject().updateHitTestResult(result, locationInContainer.point() - toLayoutSize(adjustedLocation));
        // FIXME: the call to rawValue() below is temporary and should be removed once the transition
        // to LayoutUnit-based types is complete (crbug.com/321237)
        if (!result.addNodeToListBasedTestResult(layoutObject().node(), locationInContainer, boundsRect))
            return true;
    }

    return false;
}

const char* EllipsisBox::boxName() const
{
    return "EllipsisBox";
}

} // namespace blink
