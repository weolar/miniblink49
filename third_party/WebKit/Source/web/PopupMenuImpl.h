// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PopupMenuImpl_h
#define PopupMenuImpl_h

#include "core/html/forms/PopupMenuClient.h"
#include "core/page/PagePopupClient.h"
#include "platform/PopupMenu.h"

namespace blink {

class ChromeClientImpl;
class PagePopup;
class HTMLElement;
class HTMLHRElement;
class HTMLOptGroupElement;
class HTMLOptionElement;

class PopupMenuImpl final : public PopupMenu, public PagePopupClient {
public:
    static PassRefPtrWillBeRawPtr<PopupMenuImpl> create(ChromeClientImpl*, PopupMenuClient*);
    ~PopupMenuImpl() override;

    void update();

    void dispose();

private:
    PopupMenuImpl(ChromeClientImpl*, PopupMenuClient*);

    class ItemIterationContext;
    void addOption(ItemIterationContext&, HTMLOptionElement&);
    void addOptGroup(ItemIterationContext&, HTMLOptGroupElement&);
    void addSeparator(ItemIterationContext&, HTMLHRElement&);
    void addElementStyle(ItemIterationContext&, HTMLElement&);

    // PopupMenu functions:
    void show(const FloatQuad& controlPosition, const IntSize& controlSize, int index) override;
    void hide() override;
    void disconnectClient() override;
    void updateFromElement() override;

    // PagePopupClient functions:
    IntSize contentSize() override;
    void writeDocument(SharedBuffer*) override;
    void selectFontsFromOwnerDocument(Document&) override;
    void setValueAndClosePopup(int, const String&) override;
    void setValue(const String&) override;
    void closePopup() override;
    Element& ownerElement() override;
    Locale& locale() override;
    void didClosePopup() override;

    ChromeClientImpl* m_chromeClient;
    PopupMenuClient* m_client;
    PagePopup* m_popup;
    bool m_needsUpdate;
};

}

#endif // PopupMenuImpl_h
