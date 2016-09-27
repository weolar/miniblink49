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
#include "public/web/WebFormControlElement.h"

#include "core/dom/NodeComputedStyle.h"
#include "core/html/HTMLFormControlElement.h"
#include "core/html/HTMLFormElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLSelectElement.h"
#include "core/html/HTMLTextAreaElement.h"

#include "wtf/PassRefPtr.h"

namespace blink {

bool WebFormControlElement::isEnabled() const
{
    return !constUnwrap<HTMLFormControlElement>()->isDisabledFormControl();
}

bool WebFormControlElement::isReadOnly() const
{
    return constUnwrap<HTMLFormControlElement>()->isReadOnly();
}

WebString WebFormControlElement::formControlName() const
{
    return constUnwrap<HTMLFormControlElement>()->name();
}

WebString WebFormControlElement::formControlType() const
{
    return constUnwrap<HTMLFormControlElement>()->type();
}

bool WebFormControlElement::isAutofilled() const
{
    return constUnwrap<HTMLFormControlElement>()->isAutofilled();
}

void WebFormControlElement::setAutofilled(bool autofilled)
{
    unwrap<HTMLFormControlElement>()->setAutofilled(autofilled);
}

WebString WebFormControlElement::nameForAutofill() const
{
    return constUnwrap<HTMLFormControlElement>()->nameForAutofill();
}

bool WebFormControlElement::autoComplete() const
{
    if (isHTMLInputElement(*m_private))
        return constUnwrap<HTMLInputElement>()->shouldAutocomplete();
    if (isHTMLTextAreaElement(*m_private))
        return constUnwrap<HTMLTextAreaElement>()->shouldAutocomplete();
    return false;
}

void WebFormControlElement::setValue(const WebString& value, bool sendEvents)
{
    if (isHTMLInputElement(*m_private))
        unwrap<HTMLInputElement>()->setValue(value, sendEvents ? DispatchInputAndChangeEvent : DispatchNoEvent);
    else if (isHTMLTextAreaElement(*m_private))
        unwrap<HTMLTextAreaElement>()->setValue(value, sendEvents ? DispatchInputAndChangeEvent : DispatchNoEvent);
    else if (isHTMLSelectElement(*m_private))
        unwrap<HTMLSelectElement>()->setValue(value, sendEvents);
}

WebString WebFormControlElement::value() const
{
    if (isHTMLInputElement(*m_private))
        return constUnwrap<HTMLInputElement>()->value();
    if (isHTMLTextAreaElement(*m_private))
        return constUnwrap<HTMLTextAreaElement>()->value();
    if (isHTMLSelectElement(*m_private))
        return constUnwrap<HTMLSelectElement>()->value();
    return WebString();
}

void WebFormControlElement::setSuggestedValue(const WebString& value)
{
    if (isHTMLInputElement(*m_private))
        unwrap<HTMLInputElement>()->setSuggestedValue(value);
    else if (isHTMLTextAreaElement(*m_private))
        unwrap<HTMLTextAreaElement>()->setSuggestedValue(value);
    else if (isHTMLSelectElement(*m_private))
        unwrap<HTMLSelectElement>()->setSuggestedValue(value);
}

WebString WebFormControlElement::suggestedValue() const
{
    if (isHTMLInputElement(*m_private))
        return constUnwrap<HTMLInputElement>()->suggestedValue();
    if (isHTMLTextAreaElement(*m_private))
        return constUnwrap<HTMLTextAreaElement>()->suggestedValue();
    if (isHTMLSelectElement(*m_private))
        return constUnwrap<HTMLSelectElement>()->suggestedValue();
    return WebString();
}

WebString WebFormControlElement::editingValue() const
{
    if (isHTMLInputElement(*m_private))
        return constUnwrap<HTMLInputElement>()->innerEditorValue();
    if (isHTMLTextAreaElement(*m_private))
        return constUnwrap<HTMLTextAreaElement>()->innerEditorValue();
    return WebString();
}

void WebFormControlElement::setSelectionRange(int start, int end)
{
    if (isHTMLInputElement(*m_private))
        unwrap<HTMLInputElement>()->setSelectionRange(start, end, SelectionHasNoDirection, NotDispatchSelectEvent);
    else if (isHTMLTextAreaElement(*m_private))
        unwrap<HTMLTextAreaElement>()->setSelectionRange(start, end, SelectionHasNoDirection, NotDispatchSelectEvent);
}

int WebFormControlElement::selectionStart() const
{
    if (isHTMLInputElement(*m_private))
        return constUnwrap<HTMLInputElement>()->selectionStart();
    if (isHTMLTextAreaElement(*m_private))
        return constUnwrap<HTMLTextAreaElement>()->selectionStart();
    return 0;
}

int WebFormControlElement::selectionEnd() const
{
    if (isHTMLInputElement(*m_private))
        return constUnwrap<HTMLInputElement>()->selectionEnd();
    if (isHTMLTextAreaElement(*m_private))
        return constUnwrap<HTMLTextAreaElement>()->selectionEnd();
    return 0;
}

WebString WebFormControlElement::directionForFormData() const
{
    if (const ComputedStyle* style = constUnwrap<HTMLFormControlElement>()->computedStyle())
        return style->isLeftToRightDirection() ? WebString::fromUTF8("ltr") : WebString::fromUTF8("rtl");
    return WebString::fromUTF8("ltr");
}

bool WebFormControlElement::isActivatedSubmit() const
{
    return constUnwrap<HTMLFormControlElement>()->isActivatedSubmit();
}

WebFormElement WebFormControlElement::form() const
{
    return WebFormElement(constUnwrap<HTMLFormControlElement>()->form());
}

WebFormControlElement::WebFormControlElement(const PassRefPtrWillBeRawPtr<HTMLFormControlElement>& elem)
    : WebElement(elem)
{
}

WebFormControlElement& WebFormControlElement::operator=(const PassRefPtrWillBeRawPtr<HTMLFormControlElement>& elem)
{
    m_private = elem;
    return *this;
}

WebFormControlElement::operator PassRefPtrWillBeRawPtr<HTMLFormControlElement>() const
{
    return toHTMLFormControlElement(m_private.get());
}

} // namespace blink
