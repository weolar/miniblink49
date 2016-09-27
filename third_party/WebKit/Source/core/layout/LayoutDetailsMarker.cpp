/*
 * Copyright (C) 2010, 2011 Nokia Corporation and/or its subsidiary(-ies)
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
#include "core/layout/LayoutDetailsMarker.h"

#include "core/HTMLNames.h"
#include "core/dom/Element.h"
#include "core/html/HTMLElement.h"
#include "core/paint/DetailsMarkerPainter.h"

namespace blink {

using namespace HTMLNames;

LayoutDetailsMarker::LayoutDetailsMarker(Element* element)
    : LayoutBlockFlow(element)
{
}

LayoutDetailsMarker::Orientation LayoutDetailsMarker::orientation() const
{
    switch (style()->writingMode()) {
    case TopToBottomWritingMode:
        if (style()->isLeftToRightDirection())
            return isOpen() ? Down : Right;
        return isOpen() ? Down : Left;
    case RightToLeftWritingMode:
        if (style()->isLeftToRightDirection())
            return isOpen() ? Left : Down;
        return isOpen() ? Left : Up;
    case LeftToRightWritingMode:
        if (style()->isLeftToRightDirection())
            return isOpen() ? Right : Down;
        return isOpen() ? Right : Up;
    case BottomToTopWritingMode:
        if (style()->isLeftToRightDirection())
            return isOpen() ? Up : Right;
        return isOpen() ? Up : Left;
    }
    return Right;
}

void LayoutDetailsMarker::paint(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    DetailsMarkerPainter(*this).paint(paintInfo, paintOffset);
}

bool LayoutDetailsMarker::isOpen() const
{
    for (LayoutObject* layoutObject = parent(); layoutObject; layoutObject = layoutObject->parent()) {
        if (!layoutObject->node())
            continue;
        if (isHTMLDetailsElement(*layoutObject->node()))
            return !toElement(layoutObject->node())->getAttribute(openAttr).isNull();
        if (isHTMLInputElement(*layoutObject->node()))
            return true;
    }

    return false;
}

}
