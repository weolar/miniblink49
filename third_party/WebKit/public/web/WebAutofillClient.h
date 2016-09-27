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

#ifndef WebAutofillClient_h
#define WebAutofillClient_h

namespace blink {

class WebFormControlElement;
class WebFormElement;
class WebInputElement;
class WebKeyboardEvent;
class WebNode;

template <typename T> class WebVector;

class WebAutofillClient {
public:
    // Informs the browser an interactive autocomplete has been requested.
    virtual void didRequestAutocomplete(const WebFormElement&) { }

    // These methods are called when the users edits a text-field.
    virtual void textFieldDidEndEditing(const WebInputElement&) { }
    virtual void textFieldDidChange(const WebFormControlElement&) { }
    virtual void textFieldDidReceiveKeyDown(const WebInputElement&, const WebKeyboardEvent&) { }
    // This is called when a datalist indicator is clicked.
    virtual void openTextDataListChooser(const WebInputElement&) { }
    // This is called when the datalist for an input has changed.
    virtual void dataListOptionsChanged(const WebInputElement&) { }
    // Called the first time the user interacts with the page after a load.
    virtual void firstUserGestureObserved() { }

    // Informs the client whether or not any subsequent text changes should be ignored.
    virtual void setIgnoreTextChanges(bool ignore) { }

    virtual void didAssociateFormControls(const WebVector<WebNode>&) { }
    virtual void xhrSucceeded() { }
    virtual void ajaxSucceeded() { }

protected:
    virtual ~WebAutofillClient() { }
};

} // namespace blink

#endif
