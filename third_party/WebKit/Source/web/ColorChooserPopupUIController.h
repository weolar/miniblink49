/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ColorChooserPopupUIController_h
#define ColorChooserPopupUIController_h

#include "core/page/PagePopupClient.h"
#include "web/ColorChooserUIController.h"
#include "wtf/OwnPtr.h"

namespace blink {

class ChromeClientImpl;
class ColorChooserClient;
class PagePopup;

class ColorChooserPopupUIController final : public ColorChooserUIController, public PagePopupClient  {

public:
    static PassOwnPtrWillBeRawPtr<ColorChooserPopupUIController> create(LocalFrame* frame, ChromeClientImpl* chromeClient, ColorChooserClient* client)
    {
        return adoptPtrWillBeNoop(new ColorChooserPopupUIController(frame, chromeClient, client));
    }

    ~ColorChooserPopupUIController() override;

    // ColorChooserUIController functions:
    void openUI() override;

    // ColorChooser functions
    void endChooser() override;
    AXObject* rootAXObject() override;

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

private:
    ColorChooserPopupUIController(LocalFrame*, ChromeClientImpl*, ColorChooserClient*);

    void openPopup();

    ChromeClientImpl* m_chromeClient;
    PagePopup* m_popup;
    Locale& m_locale;
};

} // namespace blink

#endif // ColorChooserPopupUIController_h
