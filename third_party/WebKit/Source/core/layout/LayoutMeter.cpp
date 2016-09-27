/*
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "core/layout/LayoutMeter.h"

#include "core/html/HTMLMeterElement.h"
#include "core/layout/LayoutTheme.h"

namespace blink {

using namespace HTMLNames;

LayoutMeter::LayoutMeter(HTMLElement* element)
    : LayoutBlockFlow(element)
{
}

LayoutMeter::~LayoutMeter()
{
}

HTMLMeterElement* LayoutMeter::meterElement() const
{
    ASSERT(node());

    if (isHTMLMeterElement(*node()))
        return toHTMLMeterElement(node());

    ASSERT(node()->shadowHost());
    return toHTMLMeterElement(node()->shadowHost());
}

void LayoutMeter::updateLogicalWidth()
{
    LayoutBox::updateLogicalWidth();

    IntSize frameSize = LayoutTheme::theme().meterSizeForBounds(this, pixelSnappedIntRect(frameRect()));
    setLogicalWidth(isHorizontalWritingMode() ? frameSize.width() : frameSize.height());
}

void LayoutMeter::computeLogicalHeight(LayoutUnit logicalHeight, LayoutUnit logicalTop, LogicalExtentComputedValues& computedValues) const
{
    LayoutBox::computeLogicalHeight(logicalHeight, logicalTop, computedValues);

    LayoutRect frame = frameRect();
    if (isHorizontalWritingMode())
        frame.setHeight(computedValues.m_extent);
    else
        frame.setWidth(computedValues.m_extent);
    IntSize frameSize = LayoutTheme::theme().meterSizeForBounds(this, pixelSnappedIntRect(frame));
    computedValues.m_extent = isHorizontalWritingMode() ? frameSize.height() : frameSize.width();
}

void LayoutMeter::updateFromElement()
{
    setShouldDoFullPaintInvalidation();
}

} // namespace blink
