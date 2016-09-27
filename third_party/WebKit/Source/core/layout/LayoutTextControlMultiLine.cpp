/**
 * Copyright (C) 2006, 2007 Apple Inc. All rights reserved.
 *           (C) 2008 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
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
#include "core/layout/LayoutTextControlMultiLine.h"

#include "core/html/HTMLTextAreaElement.h"
#include "core/layout/HitTestResult.h"

namespace blink {

LayoutTextControlMultiLine::LayoutTextControlMultiLine(HTMLTextAreaElement* element)
    : LayoutTextControl(element)
{
    ASSERT(element);
}

LayoutTextControlMultiLine::~LayoutTextControlMultiLine()
{
}

bool LayoutTextControlMultiLine::nodeAtPoint(HitTestResult& result, const HitTestLocation& locationInContainer, const LayoutPoint& accumulatedOffset, HitTestAction hitTestAction)
{
    if (!LayoutTextControl::nodeAtPoint(result, locationInContainer, accumulatedOffset, hitTestAction))
        return false;

    if (result.innerNode() == node() || result.innerNode() == innerEditorElement())
        hitInnerEditorElement(result, locationInContainer.point(), accumulatedOffset);

    return true;
}

float LayoutTextControlMultiLine::getAvgCharWidth(AtomicString family)
{
    // Since Lucida Grande is the default font, we want this to match the width
    // of Courier New, the default font for textareas in IE, Firefox and Safari Win.
    // 1229 is the avgCharWidth value in the OS/2 table for Courier New.
    if (family == "Lucida Grande")
        return scaleEmToUnits(1229);

    return LayoutTextControl::getAvgCharWidth(family);
}

LayoutUnit LayoutTextControlMultiLine::preferredContentLogicalWidth(float charWidth) const
{
    int factor = toHTMLTextAreaElement(node())->cols();
    return static_cast<LayoutUnit>(ceilf(charWidth * factor)) + scrollbarThickness();
}

LayoutUnit LayoutTextControlMultiLine::computeControlLogicalHeight(LayoutUnit lineHeight, LayoutUnit nonContentHeight) const
{
    return lineHeight * toHTMLTextAreaElement(node())->rows() + nonContentHeight;
}

int LayoutTextControlMultiLine::baselinePosition(FontBaseline baselineType, bool firstLine, LineDirectionMode direction, LinePositionMode linePositionMode) const
{
    return LayoutBox::baselinePosition(baselineType, firstLine, direction, linePositionMode);
}

PassRefPtr<ComputedStyle> LayoutTextControlMultiLine::createInnerEditorStyle(const ComputedStyle& startStyle) const
{
    RefPtr<ComputedStyle> textBlockStyle = ComputedStyle::create();
    textBlockStyle->inheritFrom(startStyle);
    adjustInnerEditorStyle(*textBlockStyle);
    textBlockStyle->setDisplay(BLOCK);
    textBlockStyle->setUnique();

    return textBlockStyle.release();
}

LayoutObject* LayoutTextControlMultiLine::layoutSpecialExcludedChild(bool relayoutChildren, SubtreeLayoutScope& layoutScope)
{
    LayoutObject* placeholderLayoutObject = LayoutTextControl::layoutSpecialExcludedChild(relayoutChildren, layoutScope);
    if (!placeholderLayoutObject)
        return nullptr;
    if (!placeholderLayoutObject->isBox())
        return placeholderLayoutObject;
    LayoutBox* placeholderBox = toLayoutBox(placeholderLayoutObject);
    placeholderBox->mutableStyleRef().setLogicalWidth(Length(contentLogicalWidth() - placeholderBox->borderAndPaddingLogicalWidth(), Fixed));
    placeholderBox->layoutIfNeeded();
    placeholderBox->setX(borderLeft() + paddingLeft());
    placeholderBox->setY(borderTop() + paddingTop());
    return placeholderLayoutObject;
}

}
