/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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
#if ENABLE(INPUT_MULTIPLE_FIELDS_UI)
#include "core/html/shadow/PickerIndicatorElement.h"

#include "core/events/Event.h"
#include "core/events/KeyboardEvent.h"
#include "core/frame/Settings.h"
#include "core/html/shadow/ShadowElementNames.h"
#include "core/layout/LayoutDetailsMarker.h"
#include "core/page/ChromeClient.h"
#include "core/page/Page.h"
#include "platform/LayoutTestSupport.h"

using namespace WTF::Unicode;

namespace blink {

using namespace HTMLNames;

inline PickerIndicatorElement::PickerIndicatorElement(Document& document, PickerIndicatorOwner& pickerIndicatorOwner)
    : HTMLDivElement(document)
    , m_pickerIndicatorOwner(&pickerIndicatorOwner)
{
}

PassRefPtrWillBeRawPtr<PickerIndicatorElement> PickerIndicatorElement::create(Document& document, PickerIndicatorOwner& pickerIndicatorOwner)
{
    RefPtrWillBeRawPtr<PickerIndicatorElement> element = adoptRefWillBeNoop(new PickerIndicatorElement(document, pickerIndicatorOwner));
    element->setShadowPseudoId(AtomicString("-webkit-calendar-picker-indicator", AtomicString::ConstructFromLiteral));
    element->setAttribute(idAttr, ShadowElementNames::pickerIndicator());
    return element.release();
}

PickerIndicatorElement::~PickerIndicatorElement()
{
    closePopup();
    ASSERT(!m_chooser);
}

LayoutObject* PickerIndicatorElement::createLayoutObject(const ComputedStyle&)
{
    return new LayoutDetailsMarker(this);
}

void PickerIndicatorElement::defaultEventHandler(Event* event)
{
    if (!layoutObject())
        return;
    if (!m_pickerIndicatorOwner || m_pickerIndicatorOwner->isPickerIndicatorOwnerDisabledOrReadOnly())
        return;

    if (event->type() == EventTypeNames::click) {
        openPopup();
        event->setDefaultHandled();
    } else if (event->type() == EventTypeNames::keypress && event->isKeyboardEvent()) {
        int charCode = toKeyboardEvent(event)->charCode();
        if (charCode == ' ' || charCode == '\r') {
            openPopup();
            event->setDefaultHandled();
        }
    }

    if (!event->defaultHandled())
        HTMLDivElement::defaultEventHandler(event);
}

bool PickerIndicatorElement::willRespondToMouseClickEvents()
{
    if (layoutObject() && m_pickerIndicatorOwner && !m_pickerIndicatorOwner->isPickerIndicatorOwnerDisabledOrReadOnly())
        return true;

    return HTMLDivElement::willRespondToMouseClickEvents();
}

void PickerIndicatorElement::didChooseValue(const String& value)
{
    if (!m_pickerIndicatorOwner)
        return;
    m_pickerIndicatorOwner->pickerIndicatorChooseValue(value);
}

void PickerIndicatorElement::didChooseValue(double value)
{
    if (m_pickerIndicatorOwner)
        m_pickerIndicatorOwner->pickerIndicatorChooseValue(value);
}

void PickerIndicatorElement::didEndChooser()
{
    m_chooser.clear();
}

void PickerIndicatorElement::openPopup()
{
    if (m_chooser)
        return;
    if (!document().page())
        return;
    if (!m_pickerIndicatorOwner)
        return;
    DateTimeChooserParameters parameters;
    if (!m_pickerIndicatorOwner->setupDateTimeChooserParameters(parameters))
        return;
    m_chooser = document().page()->chromeClient().openDateTimeChooser(this, parameters);
}

Element& PickerIndicatorElement::ownerElement() const
{
    ASSERT(m_pickerIndicatorOwner);
    return m_pickerIndicatorOwner->pickerOwnerElement();
}

void PickerIndicatorElement::closePopup()
{
    if (!m_chooser)
        return;
    m_chooser->endChooser();
}

void PickerIndicatorElement::detach(const AttachContext& context)
{
    closePopup();
    HTMLDivElement::detach(context);
}

AXObject* PickerIndicatorElement::popupRootAXObject() const
{
    return m_chooser ? m_chooser->rootAXObject() : 0;
}

bool PickerIndicatorElement::isPickerIndicatorElement() const
{
    return true;
}

Node::InsertionNotificationRequest PickerIndicatorElement::insertedInto(ContainerNode* insertionPoint)
{
    HTMLDivElement::insertedInto(insertionPoint);
    return InsertionShouldCallDidNotifySubtreeInsertions;
}

void PickerIndicatorElement::didNotifySubtreeInsertionsToDocument()
{
    if (!document().settings() || !document().settings()->accessibilityEnabled())
        return;
    // Don't make this focusable if we are in layout tests in order to avoid to
    // break existing tests.
    // FIXME: We should have a way to disable accessibility in layout tests.
    if (LayoutTestSupport::isRunningLayoutTest())
        return;
    setAttribute(tabindexAttr, "0");
    setAttribute(aria_haspopupAttr, "true");
    setAttribute(roleAttr, "button");
}

DEFINE_TRACE(PickerIndicatorElement)
{
    visitor->trace(m_pickerIndicatorOwner);
    HTMLDivElement::trace(visitor);
}

}

#endif
