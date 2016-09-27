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

#ifndef DateTimeChooserImpl_h
#define DateTimeChooserImpl_h

#if ENABLE(INPUT_MULTIPLE_FIELDS_UI)
#include "core/html/forms/DateTimeChooser.h"
#include "core/page/PagePopupClient.h"

namespace blink {

class ChromeClientImpl;
class DateTimeChooserClient;
class PagePopup;

class DateTimeChooserImpl final : public DateTimeChooser, public PagePopupClient {
public:
    static PassRefPtr<DateTimeChooserImpl> create(ChromeClientImpl*, DateTimeChooserClient*, const DateTimeChooserParameters&);
    ~DateTimeChooserImpl() override;

    // DateTimeChooser functions:
    void endChooser() override;
    AXObject* rootAXObject() override;

private:
    DateTimeChooserImpl(ChromeClientImpl*, DateTimeChooserClient*, const DateTimeChooserParameters&);
    // PagePopupClient functions:
    IntSize contentSize() override;
    void writeDocument(SharedBuffer*) override;
    void selectFontsFromOwnerDocument(Document&) override { }
    Locale& locale() override;
    void setValueAndClosePopup(int, const String&) override;
    void setValue(const String&) override;
    void closePopup() override;
    Element& ownerElement() override;
    void didClosePopup() override;

    ChromeClientImpl* m_chromeClient;
    DateTimeChooserClient* m_client;
    PagePopup* m_popup;
    DateTimeChooserParameters m_parameters;
    OwnPtr<Locale> m_locale;
};

}

#endif // ENABLE(INPUT_MULTIPLE_FIELDS_UI)

#endif // DateTimeChooserImpl_h
