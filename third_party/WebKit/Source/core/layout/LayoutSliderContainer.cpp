/*
 * Copyright (C) 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/layout/LayoutSliderContainer.h"

#include "core/dom/shadow/ShadowRoot.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/html/shadow/ShadowElementNames.h"
#include "core/html/shadow/SliderThumbElement.h"
#include "core/layout/LayoutFlexibleBox.h"
#include "core/layout/LayoutSlider.h"
#include "core/layout/LayoutTheme.h"

namespace blink {

LayoutSliderContainer::LayoutSliderContainer(SliderContainerElement* element)
    : LayoutFlexibleBox(element)
{
}

inline static Decimal sliderPosition(HTMLInputElement* element)
{
    const StepRange stepRange(element->createStepRange(RejectAny));
    const Decimal oldValue = parseToDecimalForNumberType(element->value(), stepRange.defaultValue());
    return stepRange.proportionFromValue(stepRange.clampValue(oldValue));
}

inline static bool hasVerticalAppearance(HTMLInputElement* input)
{
    ASSERT(input->layoutObject());
    const ComputedStyle& sliderStyle = input->layoutObject()->styleRef();

    return sliderStyle.appearance() == SliderVerticalPart;
}

void LayoutSliderContainer::computeLogicalHeight(LayoutUnit logicalHeight, LayoutUnit logicalTop, LogicalExtentComputedValues& computedValues) const
{
    HTMLInputElement* input = toHTMLInputElement(node()->shadowHost());
    bool isVertical = hasVerticalAppearance(input);

    if (input->layoutObject()->isSlider() && !isVertical && input->list()) {
        int offsetFromCenter = LayoutTheme::theme().sliderTickOffsetFromTrackCenter();
        LayoutUnit trackHeight = 0;
        if (offsetFromCenter < 0) {
            trackHeight = -2 * offsetFromCenter;
        } else {
            int tickLength = LayoutTheme::theme().sliderTickSize().height();
            trackHeight = 2 * (offsetFromCenter + tickLength);
        }
        float zoomFactor = style()->effectiveZoom();
        if (zoomFactor != 1.0)
            trackHeight *= zoomFactor;

        // FIXME: The trackHeight should have been added before updateLogicalHeight was called to avoid this hack.
        setIntrinsicContentLogicalHeight(trackHeight);

        LayoutBox::computeLogicalHeight(trackHeight, logicalTop, computedValues);
        return;
    }
    if (isVertical)
        logicalHeight = LayoutSlider::defaultTrackLength;

    // FIXME: The trackHeight should have been added before updateLogicalHeight was called to avoid this hack.
    setIntrinsicContentLogicalHeight(logicalHeight);

    LayoutBox::computeLogicalHeight(logicalHeight, logicalTop, computedValues);
}

void LayoutSliderContainer::layout()
{
    HTMLInputElement* input = toHTMLInputElement(node()->shadowHost());
    bool isVertical = hasVerticalAppearance(input);
    mutableStyleRef().setFlexDirection(isVertical ? FlowColumn : FlowRow);
    TextDirection oldTextDirection = style()->direction();
    if (isVertical) {
        // FIXME: Work around rounding issues in RTL vertical sliders. We want them to
        // render identically to LTR vertical sliders. We can remove this work around when
        // subpixel rendering is enabled on all ports.
        mutableStyleRef().setDirection(LTR);
    }

    Element* thumbElement = input->userAgentShadowRoot()->getElementById(ShadowElementNames::sliderThumb());
    Element* trackElement = input->userAgentShadowRoot()->getElementById(ShadowElementNames::sliderTrack());
    LayoutBox* thumb = thumbElement ? thumbElement->layoutBox() : 0;
    LayoutBox* track = trackElement ? trackElement->layoutBox() : 0;

    SubtreeLayoutScope layoutScope(*this);
    // Force a layout to reset the position of the thumb so the code below doesn't move the thumb to the wrong place.
    // FIXME: Make a custom layout class for the track and move the thumb positioning code there.
    if (track)
        layoutScope.setChildNeedsLayout(track);

    LayoutFlexibleBox::layout();

    mutableStyleRef().setDirection(oldTextDirection);
    // These should always exist, unless someone mutates the shadow DOM (e.g., in the inspector).
    if (!thumb || !track)
        return;

    double percentageOffset = sliderPosition(input).toDouble();
    LayoutUnit availableExtent = isVertical ? track->contentHeight() : track->contentWidth();
    availableExtent -= isVertical ? thumb->size().height() : thumb->size().width();
    LayoutUnit offset = percentageOffset * availableExtent;
    LayoutPoint thumbLocation = thumb->location();
    if (isVertical)
        thumbLocation.setY(thumbLocation.y() + track->contentHeight() - thumb->size().height() - offset);
    else if (style()->isLeftToRightDirection())
        thumbLocation.setX(thumbLocation.x() + offset);
    else
        thumbLocation.setX(thumbLocation.x() - offset);
    thumb->setLocation(thumbLocation);

    // We need one-off invalidation code here because painting of the timeline element does not go through style.
    // Instead it has a custom implementation in C++ code.
    // Therefore the style system cannot understand when it needs to be paint invalidated.
    setShouldDoFullPaintInvalidation();
}

} // namespace blink
