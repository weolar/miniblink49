/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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
#include "public/web/WebSearchableFormData.h"

#include "core/HTMLNames.h"
#include "core/InputTypeNames.h"
#include "core/dom/Document.h"
#include "core/html/FormDataList.h"
#include "core/html/HTMLFormControlElement.h"
#include "core/html/HTMLFormElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLOptionElement.h"
#include "core/html/HTMLSelectElement.h"
#include "platform/network/FormDataBuilder.h"
#include "public/web/WebFormElement.h"
#include "public/web/WebInputElement.h"
#include "wtf/text/TextEncoding.h"

namespace blink {

using namespace HTMLNames;

namespace {

// Gets the encoding for the form.
void GetFormEncoding(const HTMLFormElement* form, WTF::TextEncoding* encoding)
{
    String str(form->getAttribute(HTMLNames::accept_charsetAttr));
    str.replace(',', ' ');
    Vector<String> charsets;
    str.split(' ', charsets);
    for (Vector<String>::const_iterator i(charsets.begin()); i != charsets.end(); ++i) {
        *encoding = WTF::TextEncoding(*i);
        if (encoding->isValid())
            return;
    }
    if (!form->document().loader())
         return;
    *encoding = WTF::TextEncoding(form->document().encoding());
}

// Returns true if the submit request results in an HTTP URL.
bool IsHTTPFormSubmit(const HTMLFormElement* form)
{
    // FIXME: This function is insane. This is an overly complicated way to get this information.
    String action(form->action());
    // The isNull() check is trying to avoid completeURL returning KURL() when passed a null string.
    return form->document().completeURL(action.isNull() ? "" : action).protocolIs("http");
}

// If the form does not have an activated submit button, the first submit
// button is returned.
HTMLFormControlElement* GetButtonToActivate(HTMLFormElement* form)
{
    HTMLFormControlElement* firstSubmitButton = nullptr;
    const FormAssociatedElement::List& element = form->associatedElements();
    for (FormAssociatedElement::List::const_iterator i(element.begin()); i != element.end(); ++i) {
        if (!(*i)->isFormControlElement())
            continue;
        HTMLFormControlElement* control = toHTMLFormControlElement(*i);
        if (control->isActivatedSubmit()) {
            // There's a button that is already activated for submit, return nullptr.
            return nullptr;
        }
        if (!firstSubmitButton && control->isSuccessfulSubmitButton())
            firstSubmitButton = control;
    }
    return firstSubmitButton;
}

// Returns true if the selected state of all the options matches the default
// selected state.
bool IsSelectInDefaultState(HTMLSelectElement* select)
{
    const WillBeHeapVector<RawPtrWillBeMember<HTMLElement>>& listItems = select->listItems();
    if (select->multiple() || select->size() > 1) {
        for (WillBeHeapVector<RawPtrWillBeMember<HTMLElement>>::const_iterator i(listItems.begin()); i != listItems.end(); ++i) {
            if (!isHTMLOptionElement(*i))
                continue;
            HTMLOptionElement* optionElement = toHTMLOptionElement(*i);
            if (optionElement->selected() != optionElement->hasAttribute(selectedAttr))
                return false;
        }
        return true;
    }

    // The select is rendered as a combobox (called menulist in WebKit). At
    // least one item is selected, determine which one.
    HTMLOptionElement* initialSelected = nullptr;
    for (WillBeHeapVector<RawPtrWillBeMember<HTMLElement>>::const_iterator i(listItems.begin()); i != listItems.end(); ++i) {
        if (!isHTMLOptionElement(*i))
            continue;
        HTMLOptionElement* optionElement = toHTMLOptionElement(*i);
        if (optionElement->hasAttribute(selectedAttr)) {
            // The page specified the option to select.
            initialSelected = optionElement;
            break;
        }
        if (!initialSelected)
            initialSelected = optionElement;
    }
    return !initialSelected || initialSelected->selected();
}

// Returns true if the form element is in its default state, false otherwise.
// The default state is the state of the form element on initial load of the
// page, and varies depending upon the form element. For example, a checkbox is
// in its default state if the checked state matches the state of the checked attribute.
bool IsInDefaultState(HTMLFormControlElement* formElement)
{
    ASSERT(formElement);
    if (isHTMLInputElement(*formElement)) {
        const HTMLInputElement& inputElement = toHTMLInputElement(*formElement);
        if (inputElement.type() == InputTypeNames::checkbox || inputElement.type() == InputTypeNames::radio)
            return inputElement.checked() == inputElement.hasAttribute(checkedAttr);
    } else if (isHTMLSelectElement(*formElement)) {
        return IsSelectInDefaultState(toHTMLSelectElement(formElement));
    }
    return true;
}

// Look for a suitable search text field in a given HTMLFormElement
// Return nothing if one of those items are found:
//  - A text area field
//  - A file upload field
//  - A Password field
//  - More than one text field
HTMLInputElement* findSuitableSearchInputElement(const HTMLFormElement* form)
{
    HTMLInputElement* textElement = nullptr;
    const FormAssociatedElement::List& element = form->associatedElements();
    for (FormAssociatedElement::List::const_iterator i(element.begin()); i != element.end(); ++i) {
        if (!(*i)->isFormControlElement())
            continue;

        HTMLFormControlElement* control = toHTMLFormControlElement(*i);

        if (control->isDisabledFormControl() || control->name().isNull())
            continue;

        if (!IsInDefaultState(control) || isHTMLTextAreaElement(*control))
            return nullptr;

        if (isHTMLInputElement(*control) && control->willValidate()) {
            const HTMLInputElement& input = toHTMLInputElement(*control);

            // Return nothing if a file upload field or a password field are found.
            if (input.type() == InputTypeNames::file || input.type() == InputTypeNames::password)
                return nullptr;

            if (input.isTextField()) {
                if (textElement) {
                    // The auto-complete bar only knows how to fill in one value.
                    // This form has multiple fields; don't treat it as searchable.
                    return nullptr;
                }
                textElement = toHTMLInputElement(control);
            }
        }
    }
    return textElement;
}

// Build a search string based on a given HTMLFormElement and HTMLInputElement
//
// Search string output example from www.google.com:
// "hl=en&source=hp&biw=1085&bih=854&q={searchTerms}&btnG=Google+Search&aq=f&aqi=&aql=&oq="
//
// Return false if the provided HTMLInputElement is not found in the form
bool buildSearchString(const HTMLFormElement* form, Vector<char>* encodedString, WTF::TextEncoding* encoding, const HTMLInputElement* textElement)
{
    bool isElementFound = false;

    const FormAssociatedElement::List& elements = form->associatedElements();
    for (FormAssociatedElement::List::const_iterator i(elements.begin()); i != elements.end(); ++i) {
        if (!(*i)->isFormControlElement())
            continue;

        HTMLFormControlElement* control = toHTMLFormControlElement(*i);

        if (control->isDisabledFormControl() || control->name().isNull())
            continue;

        FormDataList* dataList = FormDataList::create(*encoding);
        if (!control->appendFormData(*dataList, false))
            continue;

        const FormDataList::FormDataListItems& items = dataList->items();
        for (FormDataList::FormDataListItems::const_iterator j(items.begin()); j != items.end(); ++j) {
            if (!encodedString->isEmpty())
                encodedString->append('&');
            FormDataBuilder::encodeStringAsFormData(*encodedString, j->data());
            encodedString->append('=');
            ++j;
            if (control == textElement) {
                encodedString->append("{searchTerms}", 13);
                isElementFound = true;
            } else
                FormDataBuilder::encodeStringAsFormData(*encodedString, j->data());
        }
    }
    return isElementFound;
}
} // namespace

WebSearchableFormData::WebSearchableFormData(const WebFormElement& form, const WebInputElement& selectedInputElement)
{
    RefPtrWillBeRawPtr<HTMLFormElement> formElement = static_cast<PassRefPtrWillBeRawPtr<HTMLFormElement>>(form);
    HTMLInputElement* inputElement = static_cast<PassRefPtrWillBeRawPtr<HTMLInputElement>>(selectedInputElement).get();

    // Only consider forms that GET data.
    // Allow HTTPS only when an input element is provided.
    if (equalIgnoringCase(formElement->getAttribute(methodAttr), "post")
        || (!IsHTTPFormSubmit(formElement.get()) && !inputElement))
        return;

    Vector<char> encodedString;
    WTF::TextEncoding encoding;

    GetFormEncoding(formElement.get(), &encoding);
    if (!encoding.isValid()) {
        // Need a valid encoding to encode the form elements.
        // If the encoding isn't found webkit ends up replacing the params with
        // empty strings. So, we don't try to do anything here.
        return;
    }

    // Look for a suitable search text field in the form when a
    // selectedInputElement is not provided.
    if (!inputElement) {
        inputElement = findSuitableSearchInputElement(formElement.get());

        // Return if no suitable text element has been found.
        if (!inputElement)
            return;
    }

    HTMLFormControlElement* firstSubmitButton = GetButtonToActivate(formElement.get());
    if (firstSubmitButton) {
        // The form does not have an active submit button, make the first button
        // active. We need to do this, otherwise the URL will not contain the
        // name of the submit button.
        firstSubmitButton->setActivatedSubmit(true);
    }

    bool isValidSearchString = buildSearchString(formElement.get(), &encodedString, &encoding, inputElement);

    if (firstSubmitButton)
        firstSubmitButton->setActivatedSubmit(false);

    // Return if the search string is not valid.
    if (!isValidSearchString)
        return;

    String action(formElement->action());
    KURL url(formElement->document().completeURL(action.isNull() ? "" : action));
    RefPtr<FormData> formData = FormData::create(encodedString);
    url.setQuery(formData->flattenToString());
    m_url = url;
    m_encoding = String(encoding.name());
}

} // namespace blink
