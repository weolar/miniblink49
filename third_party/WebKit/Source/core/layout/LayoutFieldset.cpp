/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006 Apple Computer, Inc.
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
#include "core/layout/LayoutFieldset.h"

#include "core/CSSPropertyNames.h"
#include "core/HTMLNames.h"
#include "core/html/HTMLLegendElement.h"
#include "core/paint/FieldsetPainter.h"

using namespace std;

namespace blink {

using namespace HTMLNames;

LayoutFieldset::LayoutFieldset(Element* element)
    : LayoutBlockFlow(element)
{
}

void LayoutFieldset::computePreferredLogicalWidths()
{
    LayoutBlockFlow::computePreferredLogicalWidths();
    if (LayoutBox* legend = findInFlowLegend()) {
        int legendMinWidth = legend->minPreferredLogicalWidth();

        Length legendMarginLeft = legend->style()->marginLeft();
        Length legendMarginRight = legend->style()->marginLeft();

        if (legendMarginLeft.isFixed())
            legendMinWidth += legendMarginLeft.value();

        if (legendMarginRight.isFixed())
            legendMinWidth += legendMarginRight.value();

        m_minPreferredLogicalWidth = max(m_minPreferredLogicalWidth, legendMinWidth + borderAndPaddingWidth());
    }
}

LayoutObject* LayoutFieldset::layoutSpecialExcludedChild(bool relayoutChildren, SubtreeLayoutScope&)
{
    LayoutBox* legend = findInFlowLegend();
    if (legend) {
        LayoutRect oldLegendFrameRect = legend->frameRect();

        if (relayoutChildren)
            legend->setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::FieldsetChanged);
        legend->layoutIfNeeded();

        LayoutUnit logicalLeft;
        if (style()->isLeftToRightDirection()) {
            switch (legend->style()->textAlign()) {
            case CENTER:
                logicalLeft = (logicalWidth() - logicalWidthForChild(*legend)) / 2;
                break;
            case RIGHT:
                logicalLeft = logicalWidth() - borderEnd() - paddingEnd() - logicalWidthForChild(*legend);
                break;
            default:
                logicalLeft = borderStart() + paddingStart() + marginStartForChild(*legend);
                break;
            }
        } else {
            switch (legend->style()->textAlign()) {
            case LEFT:
                logicalLeft = borderStart() + paddingStart();
                break;
            case CENTER: {
                // Make sure that the extra pixel goes to the end side in RTL (since it went to the end side
                // in LTR).
                LayoutUnit centeredWidth = logicalWidth() - logicalWidthForChild(*legend);
                logicalLeft = centeredWidth - centeredWidth / 2;
                break;
            }
            default:
                logicalLeft = logicalWidth() - borderStart() - paddingStart() - marginStartForChild(*legend) - logicalWidthForChild(*legend);
                break;
            }
        }

        setLogicalLeftForChild(*legend, logicalLeft);

        LayoutUnit fieldsetBorderBefore = borderBefore();
        LayoutUnit legendLogicalHeight = logicalHeightForChild(*legend);

        LayoutUnit legendLogicalTop;
        LayoutUnit collapsedLegendExtent;
        // FIXME: We need to account for the legend's margin before too.
        if (fieldsetBorderBefore > legendLogicalHeight) {
            // The <legend> is smaller than the associated fieldset before border
            // so the latter determines positioning of the <legend>. The sizing depends
            // on the legend's margins as we want to still follow the author's cues.
            // Firefox completely ignores the margins in this case which seems wrong.
            legendLogicalTop = (fieldsetBorderBefore - legendLogicalHeight) / 2;
            collapsedLegendExtent = max<LayoutUnit>(fieldsetBorderBefore, legendLogicalTop + legendLogicalHeight + marginAfterForChild(*legend));
        } else {
            collapsedLegendExtent = legendLogicalHeight + marginAfterForChild(*legend);
        }

        setLogicalTopForChild(*legend, legendLogicalTop);
        setLogicalHeight(paddingBefore() + collapsedLegendExtent);

        if (legend->frameRect() != oldLegendFrameRect) {
            // We need to invalidate the fieldset border if the legend's frame changed.
            setShouldDoFullPaintInvalidation();
        }
    }
    return legend;
}

LayoutBox* LayoutFieldset::findInFlowLegend() const
{
    for (LayoutObject* legend = firstChild(); legend; legend = legend->nextSibling()) {
        if (legend->isFloatingOrOutOfFlowPositioned())
            continue;

        if (isHTMLLegendElement(legend->node()))
            return toLayoutBox(legend);
    }
    return nullptr;
}

void LayoutFieldset::paintBoxDecorationBackground(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    FieldsetPainter(*this).paintBoxDecorationBackground(paintInfo, paintOffset);
}

void LayoutFieldset::paintMask(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    FieldsetPainter(*this).paintMask(paintInfo, paintOffset);
}

} // namespace blink
