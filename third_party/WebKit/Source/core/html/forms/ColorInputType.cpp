/*
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
#include "core/html/forms/ColorInputType.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "bindings/core/v8/ScriptController.h"
#include "core/CSSPropertyNames.h"
#include "core/InputTypeNames.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/events/MouseEvent.h"
#include "core/events/ScopedEventQueue.h"
#include "core/html/HTMLDataListElement.h"
#include "core/html/HTMLDataListOptionsCollection.h"
#include "core/html/HTMLDivElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLOptionElement.h"
#include "core/html/forms/ColorChooser.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/layout/LayoutTheme.h"
#include "core/layout/LayoutView.h"
#include "core/page/ChromeClient.h"
#include "platform/JSONValues.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/UserGestureIndicator.h"
#include "platform/graphics/Color.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

using namespace HTMLNames;

// Upper limit of number of datalist suggestions shown.
static const unsigned maxSuggestions = 1000;
// Upper limit for the length of the labels for datalist suggestions.
static const unsigned maxSuggestionLabelLength = 1000;

static bool isValidColorString(const String& value)
{
    if (value.isEmpty())
        return false;
    if (value[0] != '#')
        return false;

    // We don't accept #rgb and #aarrggbb formats.
    if (value.length() != 7)
        return false;
    Color color;
    return color.setFromString(value) && !color.hasAlpha();
}

PassRefPtrWillBeRawPtr<InputType> ColorInputType::create(HTMLInputElement& element)
{
    return adoptRefWillBeNoop(new ColorInputType(element));
}

ColorInputType::~ColorInputType()
{
}

DEFINE_TRACE(ColorInputType)
{
    visitor->trace(m_chooser);
    BaseClickableWithKeyInputType::trace(visitor);
    ColorChooserClient::trace(visitor);
}

void ColorInputType::countUsage()
{
    countUsageIfVisible(UseCounter::InputTypeColor);
}

const AtomicString& ColorInputType::formControlType() const
{
    return InputTypeNames::color;
}

bool ColorInputType::supportsRequired() const
{
    return false;
}

String ColorInputType::fallbackValue() const
{
    return String("#000000");
}

String ColorInputType::sanitizeValue(const String& proposedValue) const
{
    if (!isValidColorString(proposedValue))
        return fallbackValue();

    return proposedValue.lower();
}

Color ColorInputType::valueAsColor() const
{
    Color color;
    bool success = color.setFromString(element().value());
    ASSERT_UNUSED(success, success);
    return color;
}

void ColorInputType::createShadowSubtree()
{
    ASSERT(element().shadow());

    Document& document = element().document();
    RefPtrWillBeRawPtr<HTMLDivElement> wrapperElement = HTMLDivElement::create(document);
    wrapperElement->setShadowPseudoId(AtomicString("-webkit-color-swatch-wrapper", AtomicString::ConstructFromLiteral));
    RefPtrWillBeRawPtr<HTMLDivElement> colorSwatch = HTMLDivElement::create(document);
    colorSwatch->setShadowPseudoId(AtomicString("-webkit-color-swatch", AtomicString::ConstructFromLiteral));
    wrapperElement->appendChild(colorSwatch.release());
    element().userAgentShadowRoot()->appendChild(wrapperElement.release());

    element().updateView();
}

void ColorInputType::setValue(const String& value, bool valueChanged, TextFieldEventBehavior eventBehavior)
{
    InputType::setValue(value, valueChanged, eventBehavior);

    if (!valueChanged)
        return;

    element().updateView();
    if (m_chooser)
        m_chooser->setSelectedColor(valueAsColor());
}

void ColorInputType::handleDOMActivateEvent(Event* event)
{
    if (element().isDisabledFormControl() || !element().layoutObject())
        return;

    if (!UserGestureIndicator::processingUserGesture())
        return;

    ChromeClient* chromeClient = this->chromeClient();
    if (chromeClient && !m_chooser)
        m_chooser = chromeClient->openColorChooser(element().document().frame(), this, valueAsColor());

    event->setDefaultHandled();
}

void ColorInputType::closePopupView()
{
    endColorChooser();
}

bool ColorInputType::shouldRespectListAttribute()
{
    return true;
}

bool ColorInputType::typeMismatchFor(const String& value) const
{
    return !isValidColorString(value);
}

void ColorInputType::warnIfValueIsInvalid(const String& value) const
{
    if (!equalIgnoringCase(value, element().sanitizeValue(value))) {
        element().document().addConsoleMessage(ConsoleMessage::create(RenderingMessageSource, WarningMessageLevel,
            String::format("The specified value %s does not conform to the required format.  The format is \"#rrggbb\" where rr, gg, bb are two-digit hexadecimal numbers.", JSONValue::quoteString(value).utf8().data())));
    }
}

void ColorInputType::didChooseColor(const Color& color)
{
    if (element().isDisabledFormControl() || color == valueAsColor())
        return;
    element().setValueFromRenderer(color.serialized());
    element().updateView();
    if (!LayoutTheme::theme().isModalColorChooser())
        element().dispatchFormControlChangeEvent();
}

void ColorInputType::didEndChooser()
{
    EventQueueScope scope;
    if (LayoutTheme::theme().isModalColorChooser())
        element().dispatchFormControlChangeEvent();
    m_chooser.clear();
}

void ColorInputType::endColorChooser()
{
    if (m_chooser)
        m_chooser->endChooser();
}

void ColorInputType::updateView()
{
    HTMLElement* colorSwatch = shadowColorSwatch();
    if (!colorSwatch)
        return;

    colorSwatch->setInlineStyleProperty(CSSPropertyBackgroundColor, element().value());
}

HTMLElement* ColorInputType::shadowColorSwatch() const
{
    ShadowRoot* shadow = element().userAgentShadowRoot();
    return shadow ? toHTMLElement(shadow->firstChild()->firstChild()) : 0;
}

Element& ColorInputType::ownerElement() const
{
    return element();
}

IntRect ColorInputType::elementRectRelativeToViewport() const
{
    return element().document().view()->contentsToViewport(element().pixelSnappedBoundingBox());
}

Color ColorInputType::currentColor()
{
    return valueAsColor();
}

bool ColorInputType::shouldShowSuggestions() const
{
    return element().fastHasAttribute(listAttr);
}

Vector<ColorSuggestion> ColorInputType::suggestions() const
{
    Vector<ColorSuggestion> suggestions;
    HTMLDataListElement* dataList = element().dataList();
    if (dataList) {
        RefPtrWillBeRawPtr<HTMLDataListOptionsCollection> options = dataList->options();
        for (unsigned i = 0; HTMLOptionElement* option = options->item(i); i++) {
            if (!element().isValidValue(option->value()))
                continue;
            Color color;
            if (!color.setFromString(option->value()))
                continue;
            ColorSuggestion suggestion(color, option->label().left(maxSuggestionLabelLength));
            suggestions.append(suggestion);
            if (suggestions.size() >= maxSuggestions)
                break;
        }
    }
    return suggestions;
}

AXObject* ColorInputType::popupRootAXObject()
{
    return m_chooser ? m_chooser->rootAXObject() : nullptr;
}

ColorChooserClient* ColorInputType::colorChooserClient()
{
    return this;
}

} // namespace blink
