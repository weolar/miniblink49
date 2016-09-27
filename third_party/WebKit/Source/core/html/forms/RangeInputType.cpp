/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 * Copyright (C) 2011 Apple Inc. All rights reserved.
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
#include "core/html/forms/RangeInputType.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/HTMLNames.h"
#include "core/InputTypeNames.h"
#include "core/dom/AXObjectCache.h"
#include "core/dom/Touch.h"
#include "core/dom/TouchList.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/events/KeyboardEvent.h"
#include "core/events/MouseEvent.h"
#include "core/events/ScopedEventQueue.h"
#include "core/events/TouchEvent.h"
#include "core/html/HTMLDataListElement.h"
#include "core/html/HTMLDataListOptionsCollection.h"
#include "core/html/HTMLDivElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLOptionElement.h"
#include "core/html/forms/StepRange.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/html/shadow/ShadowElementNames.h"
#include "core/html/shadow/SliderThumbElement.h"
#include "core/layout/LayoutSlider.h"
#include "platform/PlatformMouseEvent.h"
#include "wtf/MathExtras.h"
#include "wtf/NonCopyingSort.h"
#include "wtf/PassOwnPtr.h"
#include <limits>

namespace blink {

using namespace HTMLNames;

static const int rangeDefaultMinimum = 0;
static const int rangeDefaultMaximum = 100;
static const int rangeDefaultStep = 1;
static const int rangeDefaultStepBase = 0;
static const int rangeStepScaleFactor = 1;

static Decimal ensureMaximum(const Decimal& proposedValue, const Decimal& minimum, const Decimal& fallbackValue)
{
    return proposedValue >= minimum ? proposedValue : std::max(minimum, fallbackValue);
}

PassRefPtrWillBeRawPtr<InputType> RangeInputType::create(HTMLInputElement& element)
{
    return adoptRefWillBeNoop(new RangeInputType(element));
}

RangeInputType::RangeInputType(HTMLInputElement& element)
    : InputType(element)
    , m_tickMarkValuesDirty(true)
{
}

void RangeInputType::countUsage()
{
    countUsageIfVisible(UseCounter::InputTypeRange);
}

const AtomicString& RangeInputType::formControlType() const
{
    return InputTypeNames::range;
}

double RangeInputType::valueAsDouble() const
{
    return parseToDoubleForNumberType(element().value());
}

void RangeInputType::setValueAsDouble(double newValue, TextFieldEventBehavior eventBehavior, ExceptionState& exceptionState) const
{
    setValueAsDecimal(Decimal::fromDouble(newValue), eventBehavior, exceptionState);
}

bool RangeInputType::typeMismatchFor(const String& value) const
{
    return !value.isEmpty() && !std::isfinite(parseToDoubleForNumberType(value));
}

bool RangeInputType::supportsRequired() const
{
    return false;
}

StepRange RangeInputType::createStepRange(AnyStepHandling anyStepHandling) const
{
    DEFINE_STATIC_LOCAL(const StepRange::StepDescription, stepDescription, (rangeDefaultStep, rangeDefaultStepBase, rangeStepScaleFactor));

    const Decimal stepBase = findStepBase(rangeDefaultStepBase);
    const Decimal minimum = parseToNumber(element().fastGetAttribute(minAttr), rangeDefaultMinimum);
    const Decimal maximum = ensureMaximum(parseToNumber(element().fastGetAttribute(maxAttr), rangeDefaultMaximum), minimum, rangeDefaultMaximum);

    const Decimal step = StepRange::parseStep(anyStepHandling, stepDescription, element().fastGetAttribute(stepAttr));
    return StepRange(stepBase, minimum, maximum, step, stepDescription);
}

bool RangeInputType::isSteppable() const
{
    return true;
}

void RangeInputType::handleMouseDownEvent(MouseEvent* event)
{
    if (element().isDisabledOrReadOnly())
        return;

    Node* targetNode = event->target()->toNode();
    if (event->button() != LeftButton || !targetNode)
        return;
    ASSERT(element().shadow());
    if (targetNode != element() && !targetNode->isDescendantOf(element().userAgentShadowRoot()))
        return;
    SliderThumbElement* thumb = sliderThumbElement();
    if (targetNode == thumb)
        return;
    thumb->dragFrom(event->absoluteLocation());
}

void RangeInputType::handleTouchEvent(TouchEvent* event)
{
    if (element().isDisabledOrReadOnly())
        return;

    if (event->type() == EventTypeNames::touchend) {
        element().dispatchFormControlChangeEvent();
        event->setDefaultHandled();
        return;
    }

    TouchList* touches = event->targetTouches();
    if (touches->length() == 1) {
        sliderThumbElement()->setPositionFromPoint(touches->item(0)->absoluteLocation());
        event->setDefaultHandled();
    }
}

bool RangeInputType::hasTouchEventHandler() const
{
    return true;
}

void RangeInputType::handleKeydownEvent(KeyboardEvent* event)
{
    if (element().isDisabledOrReadOnly())
        return;

    const String& key = event->keyIdentifier();

    const Decimal current = parseToNumberOrNaN(element().value());
    ASSERT(current.isFinite());

    StepRange stepRange(createStepRange(RejectAny));


    // FIXME: We can't use stepUp() for the step value "any". So, we increase
    // or decrease the value by 1/100 of the value range. Is it reasonable?
    const Decimal step = equalIgnoringCase(element().fastGetAttribute(stepAttr), "any") ? (stepRange.maximum() - stepRange.minimum()) / 100 : stepRange.step();
    const Decimal bigStep = std::max((stepRange.maximum() - stepRange.minimum()) / 10, step);

    bool isVertical = false;
    if (element().layoutObject()) {
        ControlPart part = element().layoutObject()->style()->appearance();
        isVertical = part == SliderVerticalPart;
    }

    Decimal newValue;
    if (key == "Up")
        newValue = current + step;
    else if (key == "Down")
        newValue = current - step;
    else if (key == "Left")
        newValue = isVertical ? current + step : current - step;
    else if (key == "Right")
        newValue = isVertical ? current - step : current + step;
    else if (key == "PageUp")
        newValue = current + bigStep;
    else if (key == "PageDown")
        newValue = current - bigStep;
    else if (key == "Home")
        newValue = isVertical ? stepRange.maximum() : stepRange.minimum();
    else if (key == "End")
        newValue = isVertical ? stepRange.minimum() : stepRange.maximum();
    else
        return; // Did not match any key binding.

    newValue = stepRange.clampValue(newValue);

    if (newValue != current) {
        EventQueueScope scope;
        TextFieldEventBehavior eventBehavior = DispatchInputAndChangeEvent;
        setValueAsDecimal(newValue, eventBehavior, IGNORE_EXCEPTION);

        if (AXObjectCache* cache = element().document().existingAXObjectCache())
            cache->handleValueChanged(&element());
    }

    event->setDefaultHandled();
}

void RangeInputType::createShadowSubtree()
{
    ASSERT(element().shadow());

    Document& document = element().document();
    RefPtrWillBeRawPtr<HTMLDivElement> track = HTMLDivElement::create(document);
    track->setShadowPseudoId(AtomicString("-webkit-slider-runnable-track", AtomicString::ConstructFromLiteral));
    track->setAttribute(idAttr, ShadowElementNames::sliderTrack());
    track->appendChild(SliderThumbElement::create(document));
    RefPtrWillBeRawPtr<HTMLElement> container = SliderContainerElement::create(document);
    container->appendChild(track.release());
    element().userAgentShadowRoot()->appendChild(container.release());
}

LayoutObject* RangeInputType::createLayoutObject(const ComputedStyle&) const
{
    return new LayoutSlider(&element());
}

Decimal RangeInputType::parseToNumber(const String& src, const Decimal& defaultValue) const
{
    return parseToDecimalForNumberType(src, defaultValue);
}

String RangeInputType::serialize(const Decimal& value) const
{
    if (!value.isFinite())
        return String();
    return serializeForNumberType(value);
}

// FIXME: Could share this with BaseClickableWithKeyInputType and BaseCheckableInputType if we had a common base class.
void RangeInputType::accessKeyAction(bool sendMouseEvents)
{
    InputType::accessKeyAction(sendMouseEvents);

    element().dispatchSimulatedClick(0, sendMouseEvents ? SendMouseUpDownEvents : SendNoEvents);
}

void RangeInputType::sanitizeValueInResponseToMinOrMaxAttributeChange()
{
    if (element().hasDirtyValue())
        element().setValue(element().value());
    element().updateView();
}

void RangeInputType::setValue(const String& value, bool valueChanged, TextFieldEventBehavior eventBehavior)
{
    InputType::setValue(value, valueChanged, eventBehavior);

    if (valueChanged)
        element().updateView();
}

void RangeInputType::updateView()
{
    sliderThumbElement()->setPositionFromValue();
}

String RangeInputType::fallbackValue() const
{
    return serializeForNumberType(createStepRange(RejectAny).defaultValue());
}

String RangeInputType::sanitizeValue(const String& proposedValue) const
{
    StepRange stepRange(createStepRange(RejectAny));
    const Decimal proposedNumericValue = parseToNumber(proposedValue, stepRange.defaultValue());
    return serializeForNumberType(stepRange.clampValue(proposedNumericValue));
}

void RangeInputType::disabledAttributeChanged()
{
    if (element().isDisabledFormControl())
        sliderThumbElement()->stopDragging();
}

bool RangeInputType::shouldRespectListAttribute()
{
    return true;
}

inline SliderThumbElement* RangeInputType::sliderThumbElement() const
{
    return toSliderThumbElement(element().userAgentShadowRoot()->getElementById(ShadowElementNames::sliderThumb()));
}

inline Element* RangeInputType::sliderTrackElement() const
{
    return element().userAgentShadowRoot()->getElementById(ShadowElementNames::sliderTrack());
}

void RangeInputType::listAttributeTargetChanged()
{
    m_tickMarkValuesDirty = true;
    Element* sliderTrackElement = this->sliderTrackElement();
    if (sliderTrackElement->layoutObject())
        sliderTrackElement->layoutObject()->setNeedsLayoutAndFullPaintInvalidation(LayoutInvalidationReason::AttributeChanged);
}

static bool decimalCompare(const Decimal& a, const Decimal& b)
{
    return a < b;
}

void RangeInputType::updateTickMarkValues()
{
    if (!m_tickMarkValuesDirty)
        return;
    m_tickMarkValues.clear();
    m_tickMarkValuesDirty = false;
    HTMLDataListElement* dataList = element().dataList();
    if (!dataList)
        return;
    RefPtrWillBeRawPtr<HTMLDataListOptionsCollection> options = dataList->options();
    m_tickMarkValues.reserveCapacity(options->length());
    for (unsigned i = 0; i < options->length(); ++i) {
        HTMLOptionElement* optionElement = options->item(i);
        String optionValue = optionElement->value();
        if (!this->element().isValidValue(optionValue))
            continue;
        m_tickMarkValues.append(parseToNumber(optionValue, Decimal::nan()));
    }
    m_tickMarkValues.shrinkToFit();
    nonCopyingSort(m_tickMarkValues.begin(), m_tickMarkValues.end(), decimalCompare);
}

Decimal RangeInputType::findClosestTickMarkValue(const Decimal& value)
{
    updateTickMarkValues();
    if (!m_tickMarkValues.size())
        return Decimal::nan();

    size_t left = 0;
    size_t right = m_tickMarkValues.size();
    size_t middle;
    while (true) {
        ASSERT(left <= right);
        middle = left + (right - left) / 2;
        if (!middle)
            break;
        if (middle == m_tickMarkValues.size() - 1 && m_tickMarkValues[middle] < value) {
            middle++;
            break;
        }
        if (m_tickMarkValues[middle - 1] <= value && m_tickMarkValues[middle] >= value)
            break;

        if (m_tickMarkValues[middle] < value)
            left = middle;
        else
            right = middle;
    }
    const Decimal closestLeft = middle ? m_tickMarkValues[middle - 1] : Decimal::infinity(Decimal::Negative);
    const Decimal closestRight = middle != m_tickMarkValues.size() ? m_tickMarkValues[middle] : Decimal::infinity(Decimal::Positive);
    if (closestRight - value < value - closestLeft)
        return closestRight;
    return closestLeft;
}

} // namespace blink
