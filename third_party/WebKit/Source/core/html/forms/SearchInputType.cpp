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
#include "core/html/forms/SearchInputType.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/HTMLNames.h"
#include "core/InputTypeNames.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/events/KeyboardEvent.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/shadow/ShadowElementNames.h"
#include "core/html/shadow/TextControlInnerElements.h"
#include "core/layout/LayoutSearchField.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

using namespace HTMLNames;

inline SearchInputType::SearchInputType(HTMLInputElement& element)
    : BaseTextInputType(element)
    , m_searchEventTimer(this, &SearchInputType::searchEventTimerFired)
{
}

PassRefPtrWillBeRawPtr<InputType> SearchInputType::create(HTMLInputElement& element)
{
    return adoptRefWillBeNoop(new SearchInputType(element));
}

void SearchInputType::countUsage()
{
    countUsageIfVisible(UseCounter::InputTypeSearch);
}

LayoutObject* SearchInputType::createLayoutObject(const ComputedStyle&) const
{
    return new LayoutSearchField(&element());
}

const AtomicString& SearchInputType::formControlType() const
{
    return InputTypeNames::search;
}

bool SearchInputType::needsContainer() const
{
    return true;
}

void SearchInputType::createShadowSubtree()
{
    TextFieldInputType::createShadowSubtree();
    Element* container = containerElement();
    Element* viewPort = element().userAgentShadowRoot()->getElementById(ShadowElementNames::editingViewPort());
    ASSERT(container);
    ASSERT(viewPort);

    container->insertBefore(SearchFieldDecorationElement::create(element().document()), viewPort);
    container->insertBefore(SearchFieldCancelButtonElement::create(element().document()), viewPort->nextSibling());
}

void SearchInputType::handleKeydownEvent(KeyboardEvent* event)
{
    if (element().isDisabledOrReadOnly()) {
        TextFieldInputType::handleKeydownEvent(event);
        return;
    }

    const String& key = event->keyIdentifier();
    if (key == "U+001B") {
        RefPtrWillBeRawPtr<HTMLInputElement> input(element());
        input->setValueForUser("");
        input->onSearch();
        event->setDefaultHandled();
        return;
    }
    TextFieldInputType::handleKeydownEvent(event);
}

void SearchInputType::startSearchEventTimer()
{
    ASSERT(element().layoutObject());
    unsigned length = element().innerEditorValue().length();

    if (!length) {
        stopSearchEventTimer();
        element().onSearch();
        return;
    }

    // After typing the first key, we wait 0.5 seconds.
    // After the second key, 0.4 seconds, then 0.3, then 0.2 from then on.
    m_searchEventTimer.startOneShot(max(0.2, 0.6 - 0.1 * length), FROM_HERE);
}

void SearchInputType::stopSearchEventTimer()
{
    m_searchEventTimer.stop();
}

void SearchInputType::searchEventTimerFired(Timer<SearchInputType>*)
{
    element().onSearch();
}

bool SearchInputType::searchEventsShouldBeDispatched() const
{
    return element().hasAttribute(incrementalAttr);
}

void SearchInputType::didSetValueByUserEdit(ValueChangeState state)
{
    updateCancelButtonVisibility();

    // If the incremental attribute is set, then dispatch the search event
    if (searchEventsShouldBeDispatched())
        startSearchEventTimer();

    TextFieldInputType::didSetValueByUserEdit(state);
}

void SearchInputType::updateView()
{
    BaseTextInputType::updateView();
    updateCancelButtonVisibility();
}

const AtomicString& SearchInputType::defaultAutocapitalize() const
{
    DEFINE_STATIC_LOCAL(const AtomicString, sentences, ("sentences", AtomicString::ConstructFromLiteral));
    return sentences;
}

void SearchInputType::updateCancelButtonVisibility()
{
    Element* button = element().userAgentShadowRoot()->getElementById(ShadowElementNames::clearButton());
    if (!button)
        return;
    if (element().value().isEmpty()) {
        button->setInlineStyleProperty(CSSPropertyOpacity, 0.0, CSSPrimitiveValue::CSS_NUMBER);
        button->setInlineStyleProperty(CSSPropertyPointerEvents, CSSValueNone);
    } else {
        button->removeInlineStyleProperty(CSSPropertyOpacity);
        button->removeInlineStyleProperty(CSSPropertyPointerEvents);
    }
}

bool SearchInputType::supportsInputModeAttribute() const
{
    return true;
}

} // namespace blink
