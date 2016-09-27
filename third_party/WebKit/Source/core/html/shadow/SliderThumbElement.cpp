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
#include "core/html/shadow/SliderThumbElement.h"

#include "core/events/Event.h"
#include "core/events/MouseEvent.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/forms/StepRange.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/html/shadow/ShadowElementNames.h"
#include "core/input/EventHandler.h"
#include "core/layout/LayoutFlexibleBox.h"
#include "core/layout/LayoutSlider.h"
#include "core/layout/LayoutSliderContainer.h"
#include "core/layout/LayoutSliderThumb.h"
#include "core/layout/LayoutTheme.h"

namespace blink {

using namespace HTMLNames;

inline static bool hasVerticalAppearance(HTMLInputElement* input)
{
    ASSERT(input->layoutObject());
    const ComputedStyle& sliderStyle = input->layoutObject()->styleRef();

    return sliderStyle.appearance() == SliderVerticalPart;
}

inline SliderThumbElement::SliderThumbElement(Document& document)
    : HTMLDivElement(document)
    , m_inDragMode(false)
{
}

PassRefPtrWillBeRawPtr<SliderThumbElement> SliderThumbElement::create(Document& document)
{
    RefPtrWillBeRawPtr<SliderThumbElement> element = adoptRefWillBeNoop(new SliderThumbElement(document));
    element->setAttribute(idAttr, ShadowElementNames::sliderThumb());
    return element.release();
}

void SliderThumbElement::setPositionFromValue()
{
    // Since the code to calculate position is in the LayoutSliderThumb layout
    // path, we don't actually update the value here. Instead, we poke at the
    // layoutObject directly to trigger layout.
    if (layoutObject())
        layoutObject()->setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::SliderValueChanged);
}

LayoutObject* SliderThumbElement::createLayoutObject(const ComputedStyle&)
{
    return new LayoutSliderThumb(this);
}

bool SliderThumbElement::isDisabledFormControl() const
{
    return hostInput() && hostInput()->isDisabledFormControl();
}

bool SliderThumbElement::matchesReadOnlyPseudoClass() const
{
    return hostInput() && hostInput()->matchesReadOnlyPseudoClass();
}

bool SliderThumbElement::matchesReadWritePseudoClass() const
{
    return hostInput() && hostInput()->matchesReadWritePseudoClass();
}

Node* SliderThumbElement::focusDelegate()
{
    return hostInput();
}

void SliderThumbElement::dragFrom(const LayoutPoint& point)
{
    RefPtrWillBeRawPtr<SliderThumbElement> protector(this);
    startDragging();
    setPositionFromPoint(point);
}

void SliderThumbElement::setPositionFromPoint(const LayoutPoint& point)
{
    RefPtrWillBeRawPtr<HTMLInputElement> input(hostInput());
    Element* trackElement = input->userAgentShadowRoot()->getElementById(ShadowElementNames::sliderTrack());

    if (!input->layoutObject() || !layoutBox() || !trackElement->layoutBox())
        return;

    LayoutPoint offset = roundedLayoutPoint(input->layoutObject()->absoluteToLocal(FloatPoint(point), UseTransforms));
    bool isVertical = hasVerticalAppearance(input.get());
    bool isLeftToRightDirection = layoutBox()->style()->isLeftToRightDirection();
    LayoutUnit trackSize;
    LayoutUnit position;
    LayoutUnit currentPosition;
    // We need to calculate currentPosition from absolute points becaue the
    // layoutObject for this node is usually on a layer and layoutBox()->x() and
    // y() are unusable.
    // FIXME: This should probably respect transforms.
    LayoutPoint absoluteThumbOrigin = layoutBox()->absoluteBoundingBoxRectIgnoringTransforms().location();
    LayoutPoint absoluteSliderContentOrigin = roundedLayoutPoint(input->layoutObject()->localToAbsolute());
    IntRect trackBoundingBox = trackElement->layoutObject()->absoluteBoundingBoxRectIgnoringTransforms();
    IntRect inputBoundingBox = input->layoutObject()->absoluteBoundingBoxRectIgnoringTransforms();
    if (isVertical) {
        trackSize = trackElement->layoutBox()->contentHeight() - layoutBox()->size().height();
        position = offset.y() - layoutBox()->size().height() / 2 - trackBoundingBox.y() + inputBoundingBox.y() - layoutBox()->marginBottom();
        currentPosition = absoluteThumbOrigin.y() - absoluteSliderContentOrigin.y();
    } else {
        trackSize = trackElement->layoutBox()->contentWidth() - layoutBox()->size().width();
        position = offset.x() - layoutBox()->size().width() / 2 - trackBoundingBox.x() + inputBoundingBox.x();
        position -= isLeftToRightDirection ? layoutBox()->marginLeft() : layoutBox()->marginRight();
        currentPosition = absoluteThumbOrigin.x() - absoluteSliderContentOrigin.x();
    }
    position = std::max<LayoutUnit>(0, std::min(position, trackSize));
    const Decimal ratio = Decimal::fromDouble(static_cast<double>(position) / trackSize);
    const Decimal fraction = isVertical || !isLeftToRightDirection ? Decimal(1) - ratio : ratio;
    StepRange stepRange(input->createStepRange(RejectAny));
    Decimal value = stepRange.clampValue(stepRange.valueFromProportion(fraction));

    Decimal closest = input->findClosestTickMarkValue(value);
    if (closest.isFinite()) {
        double closestFraction = stepRange.proportionFromValue(closest).toDouble();
        double closestRatio = isVertical || !isLeftToRightDirection ? 1.0 - closestFraction : closestFraction;
        LayoutUnit closestPosition = trackSize * closestRatio;
        const LayoutUnit snappingThreshold = 5;
        if ((closestPosition - position).abs() <= snappingThreshold)
            value = closest;
    }

    String valueString = serializeForNumberType(value);
    if (valueString == input->value())
        return;

    // FIXME: This is no longer being set from renderer. Consider updating the method name.
    input->setValueFromRenderer(valueString);
    if (layoutObject())
        layoutObject()->setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::SliderValueChanged);
}

void SliderThumbElement::startDragging()
{
    if (LocalFrame* frame = document().frame()) {
        frame->eventHandler().setCapturingMouseEventsNode(this);
        m_inDragMode = true;
    }
}

void SliderThumbElement::stopDragging()
{
    if (!m_inDragMode)
        return;

    if (LocalFrame* frame = document().frame())
        frame->eventHandler().setCapturingMouseEventsNode(nullptr);
    m_inDragMode = false;
    if (layoutObject())
        layoutObject()->setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::SliderValueChanged);
    if (hostInput())
        hostInput()->dispatchFormControlChangeEvent();
}

void SliderThumbElement::defaultEventHandler(Event* event)
{
    if (!event->isMouseEvent()) {
        HTMLDivElement::defaultEventHandler(event);
        return;
    }

    // FIXME: Should handle this readonly/disabled check in more general way.
    // Missing this kind of check is likely to occur elsewhere if adding it in each shadow element.
    HTMLInputElement* input = hostInput();
    if (!input || input->isDisabledOrReadOnly()) {
        stopDragging();
        HTMLDivElement::defaultEventHandler(event);
        return;
    }

    MouseEvent* mouseEvent = toMouseEvent(event);
    bool isLeftButton = mouseEvent->button() == LeftButton;
    const AtomicString& eventType = event->type();

    // We intentionally do not call event->setDefaultHandled() here because
    // MediaControlTimelineElement::defaultEventHandler() wants to handle these
    // mouse events.
    if (eventType == EventTypeNames::mousedown && isLeftButton) {
        startDragging();
        return;
    } else if (eventType == EventTypeNames::mouseup && isLeftButton) {
        stopDragging();
        return;
    } else if (eventType == EventTypeNames::mousemove) {
        if (m_inDragMode)
            setPositionFromPoint(mouseEvent->absoluteLocation());
        return;
    }

    HTMLDivElement::defaultEventHandler(event);
}

bool SliderThumbElement::willRespondToMouseMoveEvents()
{
    const HTMLInputElement* input = hostInput();
    if (input && !input->isDisabledOrReadOnly() && m_inDragMode)
        return true;

    return HTMLDivElement::willRespondToMouseMoveEvents();
}

bool SliderThumbElement::willRespondToMouseClickEvents()
{
    const HTMLInputElement* input = hostInput();
    if (input && !input->isDisabledOrReadOnly())
        return true;

    return HTMLDivElement::willRespondToMouseClickEvents();
}

void SliderThumbElement::detach(const AttachContext& context)
{
    if (m_inDragMode) {
        if (LocalFrame* frame = document().frame())
            frame->eventHandler().setCapturingMouseEventsNode(nullptr);
    }
    HTMLDivElement::detach(context);
}

HTMLInputElement* SliderThumbElement::hostInput() const
{
    // Only HTMLInputElement creates SliderThumbElement instances as its shadow nodes.
    // So, shadowHost() must be an HTMLInputElement.
    return toHTMLInputElement(shadowHost());
}

static const AtomicString& sliderThumbShadowPartId()
{
    DEFINE_STATIC_LOCAL(const AtomicString, sliderThumb, ("-webkit-slider-thumb", AtomicString::ConstructFromLiteral));
    return sliderThumb;
}

static const AtomicString& mediaSliderThumbShadowPartId()
{
    DEFINE_STATIC_LOCAL(const AtomicString, mediaSliderThumb, ("-webkit-media-slider-thumb", AtomicString::ConstructFromLiteral));
    return mediaSliderThumb;
}

const AtomicString& SliderThumbElement::shadowPseudoId() const
{
    HTMLInputElement* input = hostInput();
    if (!input || !input->layoutObject())
        return sliderThumbShadowPartId();

    const ComputedStyle& sliderStyle = input->layoutObject()->styleRef();
    switch (sliderStyle.appearance()) {
    case MediaSliderPart:
    case MediaSliderThumbPart:
    case MediaVolumeSliderPart:
    case MediaVolumeSliderThumbPart:
    case MediaFullScreenVolumeSliderPart:
    case MediaFullScreenVolumeSliderThumbPart:
        return mediaSliderThumbShadowPartId();
    default:
        return sliderThumbShadowPartId();
    }
}

// --------------------------------

inline SliderContainerElement::SliderContainerElement(Document& document)
    : HTMLDivElement(document)
{
}

DEFINE_NODE_FACTORY(SliderContainerElement)

LayoutObject* SliderContainerElement::createLayoutObject(const ComputedStyle&)
{
    return new LayoutSliderContainer(this);
}

const AtomicString& SliderContainerElement::shadowPseudoId() const
{
    DEFINE_STATIC_LOCAL(const AtomicString, mediaSliderContainer, ("-webkit-media-slider-container", AtomicString::ConstructFromLiteral));
    DEFINE_STATIC_LOCAL(const AtomicString, sliderContainer, ("-webkit-slider-container", AtomicString::ConstructFromLiteral));

    if (!shadowHost() || !shadowHost()->layoutObject())
        return sliderContainer;

    const ComputedStyle& sliderStyle = shadowHost()->layoutObject()->styleRef();
    switch (sliderStyle.appearance()) {
    case MediaSliderPart:
    case MediaSliderThumbPart:
    case MediaVolumeSliderPart:
    case MediaVolumeSliderThumbPart:
    case MediaFullScreenVolumeSliderPart:
    case MediaFullScreenVolumeSliderThumbPart:
        return mediaSliderContainer;
    default:
        return sliderContainer;
    }
}

}
