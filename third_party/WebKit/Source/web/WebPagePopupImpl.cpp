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
#include "web/WebPagePopupImpl.h"

#include "core/dom/ContextFeatures.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/PinchViewport.h"
#include "core/frame/Settings.h"
#include "core/input/EventHandler.h"
#include "core/layout/LayoutView.h"
#include "core/loader/EmptyClients.h"
#include "core/loader/FrameLoadRequest.h"
#include "core/page/DOMWindowPagePopup.h"
#include "core/page/FocusController.h"
#include "core/page/Page.h"
#include "core/page/PagePopupClient.h"
#include "modules/accessibility/AXObject.h"
#include "modules/accessibility/AXObjectCacheImpl.h"
#include "platform/EventDispatchForbiddenScope.h"
#include "platform/LayoutTestSupport.h"
#include "platform/ScriptForbiddenScope.h"
#include "platform/TraceEvent.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebCompositeAndReadbackAsyncCallback.h"
#include "public/platform/WebCursorInfo.h"
#include "public/web/WebAXObject.h"
#include "public/web/WebFrameClient.h"
#include "public/web/WebViewClient.h"
#include "public/web/WebWidgetClient.h"
#include "web/WebInputEventConversion.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebSettingsImpl.h"
#include "web/WebViewImpl.h"

namespace blink {

class PagePopupChromeClient final : public EmptyChromeClient {
    WTF_MAKE_NONCOPYABLE(PagePopupChromeClient);
    WTF_MAKE_FAST_ALLOCATED(PagePopupChromeClient);

public:
    explicit PagePopupChromeClient(WebPagePopupImpl* popup)
        : m_popup(popup)
    {
        ASSERT(m_popup->widgetClient());
    }

    void setWindowRect(const IntRect& rect) override
    {
        m_popup->m_windowRectInScreen = rect;
        m_popup->widgetClient()->setWindowRect(m_popup->m_windowRectInScreen);
    }

private:
    void closeWindowSoon() override
    {
        m_popup->closePopup();
    }

    IntRect windowRect() override
    {
        return IntRect(m_popup->m_windowRectInScreen.x, m_popup->m_windowRectInScreen.y, m_popup->m_windowRectInScreen.width, m_popup->m_windowRectInScreen.height);
    }

    IntRect viewportToScreen(const IntRect& rect) const override
    {
        IntRect rectInScreen(rect);
        rectInScreen.move(m_popup->m_windowRectInScreen.x, m_popup->m_windowRectInScreen.y);
        return rectInScreen;
    }

    void addMessageToConsole(LocalFrame*, MessageSource, MessageLevel, const String& message, unsigned lineNumber, const String&, const String&) override
    {
#ifndef NDEBUG
        fprintf(stderr, "CONSOLE MESSSAGE:%u: %s\n", lineNumber, message.utf8().data());
#endif
    }

    void invalidateRect(const IntRect& paintRect) override
    {
        if (!paintRect.isEmpty())
            m_popup->widgetClient()->didInvalidateRect(paintRect);
    }

    void scheduleAnimation() override
    {
        // Calling scheduleAnimation on m_webView so WebTestProxy will call beginFrame.
        if (LayoutTestSupport::isRunningLayoutTest())
            m_popup->m_webView->scheduleAnimation();

        if (m_popup->isAcceleratedCompositingActive()) {
            ASSERT(m_popup->m_layerTreeView);
            m_popup->m_layerTreeView->setNeedsAnimate();
            return;
        }
        m_popup->m_widgetClient->scheduleAnimation();
    }

    WebScreenInfo screenInfo() const override
    {
        return m_popup->m_webView->client() ? m_popup->m_webView->client()->screenInfo() : WebScreenInfo();
    }

    void* webView() const override
    {
        return m_popup->m_webView;
    }

    IntSize minimumWindowSize() const override
    {
        return IntSize(0, 0);
    }

    void setCursor(const Cursor& cursor) override
    {
        if (m_popup->m_webView->client())
            m_popup->m_webView->client()->didChangeCursor(WebCursorInfo(cursor));
    }

    void needTouchEvents(bool needsTouchEvents) override
    {
        m_popup->widgetClient()->hasTouchEventHandlers(needsTouchEvents);
    }

    GraphicsLayerFactory* graphicsLayerFactory() const override
    {
        return m_popup->m_webView->graphicsLayerFactory();
    }

    void attachRootGraphicsLayer(GraphicsLayer* graphicsLayer, LocalFrame* localRoot) override
    {
        m_popup->setRootGraphicsLayer(graphicsLayer);
    }

    void postAccessibilityNotification(AXObject* obj, AXObjectCache::AXNotification notification) override
    {
#ifdef MINIBLINK_NOT_IMPLEMENTED
        WebLocalFrameImpl* frame = WebLocalFrameImpl::fromFrame(m_popup->m_popupClient->ownerElement().document().frame());
        if (obj && frame && frame->client())
            frame->client()->postAccessibilityEvent(WebAXObject(obj), static_cast<WebAXEvent>(notification));
#endif // MINIBLINK_NOT_IMPLEMENTED
        notImplemented();
    }

    void setToolTip(const String& tooltipText, TextDirection dir) override
    {
        if (m_popup->widgetClient())
            m_popup->widgetClient()->setToolTipText(tooltipText, toWebTextDirection(dir));
    }

    WebPagePopupImpl* m_popup;
};

class PagePopupFeaturesClient : public ContextFeaturesClient {
    bool isEnabled(Document*, ContextFeatures::FeatureType, bool) override;
};

bool PagePopupFeaturesClient::isEnabled(Document*, ContextFeatures::FeatureType type, bool defaultValue)
{
    if (type == ContextFeatures::PagePopup)
        return true;
    return defaultValue;
}

// WebPagePopupImpl ----------------------------------------------------------------

WebPagePopupImpl::WebPagePopupImpl(WebWidgetClient* client)
    : m_widgetClient(client)
    , m_closing(false)
    , m_layerTreeView(0)
    , m_rootLayer(0)
    , m_rootGraphicsLayer(0)
    , m_isAcceleratedCompositingActive(false)
{
    ASSERT(client);
}

WebPagePopupImpl::~WebPagePopupImpl()
{
    ASSERT(!m_page);
}

bool WebPagePopupImpl::initialize(WebViewImpl* webView, PagePopupClient* popupClient)
{
    ASSERT(webView);
    ASSERT(popupClient);
    m_webView = webView;
    m_popupClient = popupClient;

    resize(m_popupClient->contentSize());

    if (!m_widgetClient || !initializePage())
        return false;
    m_widgetClient->scheduleAnimation();
    m_widgetClient->show(WebNavigationPolicy());
    setFocus(true);
    return true;
}

bool WebPagePopupImpl::initializePage()
{
    Page::PageClients pageClients;
    fillWithEmptyClients(pageClients);
    m_chromeClient = adoptPtr(new PagePopupChromeClient(this));
    pageClients.chromeClient = m_chromeClient.get();

    m_page = adoptPtrWillBeNoop(new Page(pageClients));
    m_page->settings().setScriptEnabled(true);
    m_page->settings().setAllowScriptsToCloseWindows(true);
    m_page->setDeviceScaleFactor(m_webView->deviceScaleFactor());
    m_page->settings().setDeviceSupportsTouch(m_webView->page()->settings().deviceSupportsTouch());
    // FIXME: Should we support enabling a11y while a popup is shown?
    m_page->settings().setAccessibilityEnabled(m_webView->page()->settings().accessibilityEnabled());
    m_page->settings().setScrollAnimatorEnabled(m_webView->page()->settings().scrollAnimatorEnabled());

    provideContextFeaturesTo(*m_page, adoptPtr(new PagePopupFeaturesClient()));
    static FrameLoaderClient* emptyFrameLoaderClient = new EmptyFrameLoaderClient();
    RefPtrWillBeRawPtr<LocalFrame> frame = LocalFrame::create(emptyFrameLoaderClient, &m_page->frameHost(), 0);
    frame->setPagePopupOwner(m_popupClient->ownerElement());
    frame->setView(FrameView::create(frame.get()));
    frame->init();
    frame->view()->resize(m_popupClient->contentSize());
    frame->view()->setTransparent(false);
    if (AXObjectCache* cache = m_popupClient->ownerElement().document().existingAXObjectCache())
        cache->childrenChanged(&m_popupClient->ownerElement());

    ASSERT(frame->localDOMWindow());
    DOMWindowPagePopup::install(*frame->localDOMWindow(), *this, m_popupClient);
    ASSERT(m_popupClient->ownerElement().document().existingAXObjectCache() == frame->document()->existingAXObjectCache());

    RefPtr<SharedBuffer> data = SharedBuffer::create();
    m_popupClient->writeDocument(data.get());
    //////////////////////////////////////////////////////////////////////////
//     OutputDebugStringW(L"WebPagePopupImpl::initializePage:\n");
//     String xx = String::fromUTF8(data->data(), data->size());
//     xx.append('\n');
//     OutputDebugStringW(xx.charactersWithNullTermination().data());
//     OutputDebugStringW(L"WebPagePopupImpl::initializePage ------ \n");

//     data->clear();
//     Vector<char> buffer;
//     readScript(L"E:\\test\\select.htm", buffer);
//     data->append(buffer.data(), buffer.size());
//     PagePopupClient::addString(testWebPagePopupImpl, data.get());
    //////////////////////////////////////////////////////////////////////////

    frame->loader().load(FrameLoadRequest(0, blankURL(), SubstituteData(data, "text/html", "UTF-8", KURL(), ForceSynchronousLoad)));
    return true;
}

void WebPagePopupImpl::postMessage(const String& message)
{
    if (!m_page)
        return;
    ScriptForbiddenScope::AllowUserAgentScript allowScript;
    if (LocalDOMWindow* window = toLocalFrame(m_page->mainFrame())->localDOMWindow())
        window->dispatchEvent(MessageEvent::create(message));
}

void WebPagePopupImpl::destroyPage()
{
    if (!m_page)
        return;

    m_page->willBeDestroyed();
    m_page.clear();
}

AXObject* WebPagePopupImpl::rootAXObject()
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    if (!m_page || !m_page->mainFrame())
        return 0;
    Document* document = toLocalFrame(m_page->mainFrame())->document();
    if (!document)
        return 0;
    AXObjectCache* cache = document->axObjectCache();
    ASSERT(cache);
    return toAXObjectCacheImpl(cache)->getOrCreate(document->layoutView());
#endif // MINIBLINK_NOT_IMPLEMENTED
    return 0;
}

void WebPagePopupImpl::setWindowRect(const IntRect& rect)
{
    m_chromeClient->setWindowRect(rect);
}

void WebPagePopupImpl::setRootGraphicsLayer(GraphicsLayer* layer)
{
    m_rootGraphicsLayer = layer;
    m_rootLayer = layer ? layer->platformLayer() : 0;

    setIsAcceleratedCompositingActive(layer);
    if (m_layerTreeView) {
        if (m_rootLayer) {
            m_layerTreeView->setRootLayer(*m_rootLayer);
        } else {
            m_layerTreeView->clearRootLayer();
        }
    }
}

void WebPagePopupImpl::setIsAcceleratedCompositingActive(bool enter)
{
    if (m_isAcceleratedCompositingActive == enter)
        return;

    if (!enter) {
        m_isAcceleratedCompositingActive = false;
    } else if (m_layerTreeView) {
        m_isAcceleratedCompositingActive = true;
    } else {
        TRACE_EVENT0("blink", "WebPagePopupImpl::setIsAcceleratedCompositingActive(true)");

        m_widgetClient->initializeLayerTreeView();
        m_layerTreeView = m_widgetClient->layerTreeView();
        if (m_layerTreeView) {
            m_layerTreeView->setVisible(true);
            m_isAcceleratedCompositingActive = true;
            m_layerTreeView->setDeviceScaleFactor(m_widgetClient->deviceScaleFactor());
        } else {
            m_isAcceleratedCompositingActive = false;
        }
    }
}

WebSize WebPagePopupImpl::size()
{
    return m_popupClient->contentSize();
}

void WebPagePopupImpl::beginFrame(const WebBeginFrameArgs& frameTime)
{
    if (!m_page)
        return;
    // FIXME: This should use frameTime.lastFrameTimeMonotonic but doing so
    // breaks tests.
    PageWidgetDelegate::animate(*m_page, monotonicallyIncreasingTime(), *m_page->deprecatedLocalMainFrame());
}

void WebPagePopupImpl::willCloseLayerTreeView()
{
    setIsAcceleratedCompositingActive(false);
    m_layerTreeView = 0;
}

void WebPagePopupImpl::layout()
{
    if (!m_page)
        return;
    PageWidgetDelegate::layout(*m_page, *m_page->deprecatedLocalMainFrame());
}

void WebPagePopupImpl::paint(WebCanvas* canvas, const WebRect& rect)
{
    if (!m_closing)
        PageWidgetDelegate::paint(*m_page, 0, canvas, rect, *m_page->deprecatedLocalMainFrame());
}

void WebPagePopupImpl::resize(const WebSize& newSize)
{
    m_windowRectInScreen = WebRect(m_windowRectInScreen.x, m_windowRectInScreen.y, newSize.width, newSize.height);
    m_widgetClient->setWindowRect(m_windowRectInScreen);

    if (m_page) {
        toLocalFrame(m_page->mainFrame())->view()->resize(newSize);
        m_page->frameHost().pinchViewport().setSize(newSize);
    }

    m_widgetClient->didInvalidateRect(WebRect(0, 0, newSize.width, newSize.height));
}

bool WebPagePopupImpl::handleKeyEvent(const WebKeyboardEvent& event)
{
    return handleKeyEvent(PlatformKeyboardEventBuilder(event));
}

bool WebPagePopupImpl::handleCharEvent(const WebKeyboardEvent& event)
{
    return handleKeyEvent(PlatformKeyboardEventBuilder(event));
}

bool WebPagePopupImpl::handleGestureEvent(const WebGestureEvent& event)
{
    if (m_closing || !m_page || !m_page->mainFrame() || !toLocalFrame(m_page->mainFrame())->view())
        return false;
    LocalFrame& frame = *toLocalFrame(m_page->mainFrame());
    return frame.eventHandler().handleGestureEvent(PlatformGestureEventBuilder(frame.view(), event));
}

void WebPagePopupImpl::handleMouseDown(LocalFrame& mainFrame, const WebMouseEvent& event)
{
    if (isMouseEventInWindow(event))
        PageWidgetEventHandler::handleMouseDown(mainFrame, event);
    else
        cancel();
}

bool WebPagePopupImpl::handleMouseWheel(LocalFrame& mainFrame, const WebMouseWheelEvent& event)
{
    if (isMouseEventInWindow(event))
        return PageWidgetEventHandler::handleMouseWheel(mainFrame, event);
    cancel();
    return false;
}

bool WebPagePopupImpl::isMouseEventInWindow(const WebMouseEvent& event)
{
    return IntRect(0, 0, m_windowRectInScreen.width, m_windowRectInScreen.height).contains(IntPoint(event.x, event.y));
}

bool WebPagePopupImpl::handleInputEvent(const WebInputEvent& event)
{
    if (m_closing)
        return false;
    return PageWidgetDelegate::handleInputEvent(*this, event, m_page->deprecatedLocalMainFrame());
}

bool WebPagePopupImpl::handleKeyEvent(const PlatformKeyboardEvent& event)
{
    if (m_closing || !m_page->mainFrame() || !toLocalFrame(m_page->mainFrame())->view())
        return false;
    return toLocalFrame(m_page->mainFrame())->eventHandler().keyEvent(event);
}

void WebPagePopupImpl::setFocus(bool enable)
{
    if (!m_page)
        return;
    m_page->focusController().setFocused(enable);
    if (enable)
        m_page->focusController().setActive(true);
}

void WebPagePopupImpl::close()
{
    m_closing = true;
    destroyPage(); // In case closePopup() was not called.
    m_widgetClient = 0;
    deref();
}

void WebPagePopupImpl::closePopup()
{
    // This function can be called in EventDispatchForbiddenScope for the main
    // document, and the following operations dispatch some events.  It's safe
    // because web authors can't listen the events.
    EventDispatchForbiddenScope::AllowUserAgentEvents allowEvents;

    if (m_page) {
        toLocalFrame(m_page->mainFrame())->loader().stopAllLoaders();
        ASSERT(m_page->deprecatedLocalMainFrame()->localDOMWindow());
        DOMWindowPagePopup::uninstall(*m_page->deprecatedLocalMainFrame()->localDOMWindow());
    }
    m_closing = true;

    destroyPage();

    // m_widgetClient might be 0 because this widget might be already closed.
    if (m_widgetClient) {
        // closeWidgetSoon() will call this->close() later.
        m_widgetClient->closeWidgetSoon();
    }

    m_popupClient->didClosePopup();
}

LocalDOMWindow* WebPagePopupImpl::window()
{
    return m_page->deprecatedLocalMainFrame()->localDOMWindow();
}

void WebPagePopupImpl::layoutAndPaintAsync(WebLayoutAndPaintAsyncCallback* callback)
{
    m_layerTreeView->layoutAndPaintAsync(callback);
}

void WebPagePopupImpl::compositeAndReadbackAsync(WebCompositeAndReadbackAsyncCallback* callback)
{
    ASSERT(isAcceleratedCompositingActive());
    m_layerTreeView->compositeAndReadbackAsync(callback);
}

WebPoint WebPagePopupImpl::positionRelativeToOwner()
{
    WebRect windowRect = m_webView->client()->rootWindowRect();
    return WebPoint(m_windowRectInScreen.x - windowRect.x, m_windowRectInScreen.y - windowRect.y);
}

void WebPagePopupImpl::cancel()
{
    if (m_popupClient)
        m_popupClient->closePopup();
}

// WebPagePopup ----------------------------------------------------------------

WebPagePopup* WebPagePopup::create(WebWidgetClient* client)
{
    if (!client)
        CRASH();
    // A WebPagePopupImpl instance usually has two references.
    //  - One owned by the instance itself. It represents the visible widget.
    //  - One owned by a WebViewImpl. It's released when the WebViewImpl ask the
    //    WebPagePopupImpl to close.
    // We need them because the closing operation is asynchronous and the widget
    // can be closed while the WebViewImpl is unaware of it.
    return adoptRef(new WebPagePopupImpl(client)).leakRef();
}

} // namespace blink
