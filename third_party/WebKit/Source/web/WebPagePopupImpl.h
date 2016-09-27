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

#ifndef WebPagePopupImpl_h
#define WebPagePopupImpl_h

#include "core/page/PagePopup.h"
#include "public/web/WebPagePopup.h"
#include "web/PageWidgetDelegate.h"
#include "wtf/OwnPtr.h"
#include "wtf/RefCounted.h"

namespace blink {

class GraphicsLayer;
class Page;
class PagePopupChromeClient;
class PagePopupClient;
class PlatformKeyboardEvent;
class WebLayerTreeView;
class WebLayer;
class WebViewImpl;
class LocalDOMWindow;

class WebPagePopupImpl final
    : public WebPagePopup
    , public PageWidgetEventHandler
    , public PagePopup
    , public RefCounted<WebPagePopupImpl> {
    WTF_MAKE_NONCOPYABLE(WebPagePopupImpl);
    WTF_MAKE_FAST_ALLOCATED(WebPagePopupImpl);

public:
    ~WebPagePopupImpl() override;
    bool initialize(WebViewImpl*, PagePopupClient*);
    bool handleKeyEvent(const PlatformKeyboardEvent&);
    void closePopup();
    WebWidgetClient* widgetClient() const { return m_widgetClient; }
    bool hasSamePopupClient(WebPagePopupImpl* other) { return other && m_popupClient == other->m_popupClient; }
    LocalDOMWindow* window();
    void layoutAndPaintAsync(WebLayoutAndPaintAsyncCallback*) override;
    void compositeAndReadbackAsync(WebCompositeAndReadbackAsyncCallback*) override;
    WebPoint positionRelativeToOwner() override;
    void postMessage(const String& message) override;
    void cancel();

private:
    // WebWidget functions
    WebSize size() override;
    void beginFrame(const WebBeginFrameArgs&) override;
    void layout() override;
    void willCloseLayerTreeView() override;
    void paint(WebCanvas*, const WebRect&) override;
    void resize(const WebSize&) override;
    void close() override;
    bool handleInputEvent(const WebInputEvent&) override;
    void setFocus(bool) override;
    bool isPagePopup() const override { return true; }
    bool isAcceleratedCompositingActive() const override { return m_isAcceleratedCompositingActive; }

    // PageWidgetEventHandler functions
    bool handleKeyEvent(const WebKeyboardEvent&) override;
    bool handleCharEvent(const WebKeyboardEvent&) override;
    bool handleGestureEvent(const WebGestureEvent&) override;
    void handleMouseDown(LocalFrame& mainFrame, const WebMouseEvent&) override;
    bool handleMouseWheel(LocalFrame& mainFrame, const WebMouseWheelEvent&) override;

    bool isMouseEventInWindow(const WebMouseEvent&);

    // PagePopup function
    AXObject* rootAXObject() override;
    void setWindowRect(const IntRect&) override;

    explicit WebPagePopupImpl(WebWidgetClient*);
    bool initializePage();
    void destroyPage();
    void setRootGraphicsLayer(GraphicsLayer*);
    void setIsAcceleratedCompositingActive(bool enter);

    WebWidgetClient* m_widgetClient;
    WebRect m_windowRectInScreen;
    WebViewImpl* m_webView;
    OwnPtrWillBePersistent<Page> m_page;
    OwnPtr<PagePopupChromeClient> m_chromeClient;
    PagePopupClient* m_popupClient;
    bool m_closing;

    WebLayerTreeView* m_layerTreeView;
    WebLayer* m_rootLayer;
    GraphicsLayer* m_rootGraphicsLayer;
    bool m_isAcceleratedCompositingActive;

    friend class WebPagePopup;
    friend class PagePopupChromeClient;
};

DEFINE_TYPE_CASTS(WebPagePopupImpl, WebWidget, widget, widget->isPagePopup(), widget.isPagePopup());
// WebPagePopupImpl is the only implementation of PagePopup, so no
// further checking required.
DEFINE_TYPE_CASTS(WebPagePopupImpl, PagePopup, popup, true, true);

} // namespace blink
#endif // WebPagePopupImpl_h
