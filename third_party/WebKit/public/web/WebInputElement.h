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

#ifndef WebInputElement_h
#define WebInputElement_h

#include "WebFormControlElement.h"

namespace blink {

class HTMLInputElement;
class WebElementCollection;

// Provides readonly access to some properties of a DOM input element node.
class WebInputElement : public WebFormControlElement {
public:
    WebInputElement() : WebFormControlElement() { }
    WebInputElement(const WebInputElement& element) : WebFormControlElement(element) { }

    WebInputElement& operator=(const WebInputElement& element)
    {
        WebFormControlElement::assign(element);
        return *this;
    }
    void assign(const WebInputElement& element) { WebFormControlElement::assign(element); }

    // This returns true for all of textfield-looking types such as text,
    // password, search, email, url, and number.
    BLINK_EXPORT bool isTextField() const;
    // This returns true only for type=text.
    BLINK_EXPORT bool isText() const;
    BLINK_EXPORT bool isEmailField() const;
    BLINK_EXPORT bool isPasswordField() const;
    BLINK_EXPORT bool isImageButton() const;
    BLINK_EXPORT bool isRadioButton() const;
    BLINK_EXPORT bool isCheckbox() const;
    BLINK_EXPORT int maxLength() const;
    BLINK_EXPORT void setActivatedSubmit(bool);
    BLINK_EXPORT int size() const;
    BLINK_EXPORT void setChecked(bool, bool sendEvents = false);
    // Sets the value inside the text field without being sanitized. Can't be
    // used if a renderer doesn't exist or on a non text field type. Caret will
    // be moved to the end.
    BLINK_EXPORT void setEditingValue(const WebString&);
    BLINK_EXPORT bool isValidValue(const WebString&) const;
    BLINK_EXPORT bool isChecked() const;
    BLINK_EXPORT bool isMultiple() const;

    BLINK_EXPORT WebElementCollection dataListOptions() const;

    // Return the localized value for this input type.
    BLINK_EXPORT WebString localizeValue(const WebString&) const;

    // Exposes the default value of the maxLength attribute.
    BLINK_EXPORT static int defaultMaxLength();

    // If true, forces the text of the element to be visible.
    BLINK_EXPORT void setShouldRevealPassword(bool value);

#if BLINK_IMPLEMENTATION
    WebInputElement(const PassRefPtrWillBeRawPtr<HTMLInputElement>&);
    WebInputElement& operator=(const PassRefPtrWillBeRawPtr<HTMLInputElement>&);
    operator PassRefPtrWillBeRawPtr<HTMLInputElement>() const;
#endif
};

// This returns 0 if the specified WebElement is not a WebInputElement.
BLINK_EXPORT WebInputElement* toWebInputElement(WebElement*);
// This returns 0 if the specified WebElement is not a WebInputElement.
inline const WebInputElement* toWebInputElement(const WebElement* element)
{
    return toWebInputElement(const_cast<WebElement*>(element));
}

} // namespace blink

#endif
