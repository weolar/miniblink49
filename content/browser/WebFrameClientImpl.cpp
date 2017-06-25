
#include "third_party/WebKit/public/web/WebFrameClient.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/Source/web/WebLocalFrameImpl.h"
#include "third_party/WebKit/Source/web/WebViewImpl.h"
#include "content/browser/WebFrameClientImpl.h"
#include "content/browser/WebPage.h"
#include "content/web_impl_win/WebCookieJarCurlImpl.h"
#include "content/web_impl_win/WebMediaPlayerImpl.h"
#include "content/web_impl_win/npapi/WebPluginImpl.h"
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "cef/include/capi/cef_base_capi.h"
#include "cef/include/capi/cef_browser_capi.h"
#include "cef/include/capi/cef_client_capi.h"
#include "cef/include/internal/cef_ptr.h"
#include "cef/include/internal/cef_export.h"
#include "cef/include/cef_app.h"
#include "cef/include/cef_client.h"
#include "cef/libcef/common/CommonBase.h"
#include "cef/libcef/common/CefRequestImpl.h"
#include "cef/libcef/common/CefContentClient.h"
#include "cef/libcef/common/StringUtil.h"
#include "cef/libcef/browser/CefFrameHostImpl.h"
#include "cef/libcef/browser/CefBrowserHostImpl.h"
#include "cef/libcef/renderer/CefV8Impl.h"
#endif
#include "third_party/WebKit/Source/platform/Language.h"
#include "third_party/WebKit/Source/core/frame/Settings.h"
#include "third_party/WebKit/Source/core/page/Page.h"
#include "net/RequestExtraData.h"
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
#include "wke/wkeWebView.h"
#include "wke/wkeJsBind.h"
#endif

namespace content {

WebFrameClientImpl::WebFrameClientImpl()
{
    m_loading = false;
    m_loadFailed = false;
    m_loaded = false;
    m_documentReady = false;
    m_webPage = nullptr;
}

WebFrameClientImpl::~WebFrameClientImpl()
{
    for (size_t i = 0; i < m_unusedFrames.size(); ++i) {
        WebFrame* frame = m_unusedFrames[i];
        frame->close();
    }
    m_unusedFrames.clear();
}

void WebFrameClientImpl::didAddMessageToConsole(const WebConsoleMessage& message,
    const WebString& sourceName, unsigned sourceLine, const WebString& stackTrace)
{
//     WTF::String outstr(L"console:");
// 
//     outstr.append((WTF::String)(message.text));
//     outstr.append(L" ;sourceName:");
//     outstr.append(sourceName);
// 
//     outstr.append(L" ;sourceLine:");
//     outstr.append(String::number(sourceLine));
//     outstr.append(L" \n");
//     OutputDebugStringW(outstr.charactersWithNullTermination().data());
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    wke::CWebViewHandler& handler = m_webPage->wkeHandler();
    if (handler.consoleCallback) {
        wke::CString text(message.text);
        wke::CString sourceNameStr(sourceName);
        wke::CString stackTraceStr(stackTrace);
        handler.consoleCallback(m_webPage->wkeWebView(), handler.consoleCallbackParam,
            (wkeConsoleLevel)message.level, &text, &sourceNameStr, sourceLine, &stackTraceStr);
    }
#endif
}

void WebFrameClientImpl::setWebPage(WebPage* webPage)
{
    m_webPage = webPage;
}

WebPage* WebFrameClientImpl::webPage()
{
    return m_webPage;
}

WebFrame* WebFrameClientImpl::createChildFrame(WebLocalFrame* parent, WebTreeScopeType, const WebString& frameName, WebSandboxFlags sandboxFlags)
{
    WebLocalFrameImpl* webLocalFrameImpl = WebLocalFrameImpl::create(WebTreeScopeType::Document, this);
    parent->appendChild(webLocalFrameImpl);
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    CefBrowserHostImpl* browser = m_webPage->browser();
    if (browser)
        browser->OnFrameIdentified(webLocalFrameImpl, parent);
#endif
    if (WTF::kNotFound == m_unusedFrames.find(webLocalFrameImpl))
        m_unusedFrames.append(webLocalFrameImpl);

    return webLocalFrameImpl; // TODO delete;
}

void WebFrameClientImpl::frameDetached(WebFrame* child, DetachType)
{
    if (WebFrame* parent = child->parent())
        parent->removeChild(child);
}

blink::WebPluginPlaceholder* WebFrameClientImpl::createPluginPlaceholder(WebLocalFrame*, const blink::WebPluginParams&) { return 0; }

blink::WebPlugin* WebFrameClientImpl::createPlugin(WebLocalFrame* frame, const WebPluginParams& params)
{
    PassRefPtr<WebPluginImpl> plugin = adoptRef(new WebPluginImpl(frame, params));
    plugin->setParentPlatformWidget(m_webPage->getHWND());
    plugin->setHwndRenderOffset(m_webPage->getHwndRenderOffset());
    return plugin.leakRef();
}

blink::WebMediaPlayer* WebFrameClientImpl::createMediaPlayer(WebLocalFrame* frame, const WebURL& url , WebMediaPlayerClient* client, WebContentDecryptionModule*)
{
    return new WebMediaPlayerImpl(frame, url, client);
}

blink::WebMediaPlayer* WebFrameClientImpl::createMediaPlayer(WebLocalFrame* frame, const WebURL& url, WebMediaPlayerClient* client, WebMediaPlayerEncryptedMediaClient*, WebContentDecryptionModule*)
{
    return new WebMediaPlayerImpl(frame, url, client);
}

blink::WebApplicationCacheHost* WebFrameClientImpl::createApplicationCacheHost(WebLocalFrame*, WebApplicationCacheHostClient*) { return 0; }

blink::WebServiceWorkerProvider* WebFrameClientImpl::createServiceWorkerProvider(WebLocalFrame* frame) { return 0; }

blink::WebWorkerContentSettingsClientProxy* WebFrameClientImpl::createWorkerContentSettingsClientProxy(WebLocalFrame* frame) { return 0; }

// Create a new WebPopupMenu. In the "createExternalPopupMenu" form, the
// client is responsible for rendering the contents of the popup menu.
WebExternalPopupMenu* WebFrameClientImpl::createExternalPopupMenu(const WebPopupMenuInfo&, WebExternalPopupMenuClient*) {
    return 0;
}

WebCookieJar* WebFrameClientImpl::cookieJar(WebLocalFrame*)
{
    return WebCookieJarImpl::inst();
}

void WebFrameClientImpl::onLoadingStateChange(bool isLoading, bool toDifferentDocument)
{
    m_loading = isLoading;

    if (!m_webPage)
        return;
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    CefBrowserHostImpl* browser = m_webPage->browser();
    if (!browser || !browser->client().get())
        return;

    browser->OnLoadingStateChange(isLoading, toDifferentDocument);
#endif
    WebViewImpl* webview = m_webPage->webViewImpl();
    if (!webview || !webview->client())
        return;

    bool canGoBack = webview->client()->historyBackListCount() > 0;
    bool canGoForward = webview->client()->historyForwardListCount() > 0;
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    CefRefPtr<CefLoadHandler> handler = browser->client()->GetLoadHandler();
    if (handler.get())
        handler->OnLoadingStateChange(browser, isLoading, canGoBack, canGoForward);
#endif
}

void WebFrameClientImpl::didStartLoading(bool toDifferentDocument)
{
    onLoadingStateChange(true, toDifferentDocument);
}

void WebFrameClientImpl::didStopLoading()
{
    onLoadingStateChange(false, true);
}

void WebFrameClientImpl::didChangeLoadProgress(double loadProgress)
{
    onLoadingStateChange(loadProgress != 1.0, true);
}

void WebFrameClientImpl::willSendSubmitEvent(WebLocalFrame*, const WebFormElement&) { }
void WebFrameClientImpl::willSubmitForm(WebLocalFrame*, const WebFormElement&) { }

void WebFrameClientImpl::didCreateDataSource(WebLocalFrame*, WebDataSource*) { }

void WebFrameClientImpl::didStartProvisionalLoad(WebLocalFrame* localFrame, double triggeringEventTime)
{
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    CefBrowserHostImpl* browser = m_webPage->browser();
    if (browser)
        browser->DidStartProvisionalLoad(localFrame, triggeringEventTime);
#endif
}

void WebFrameClientImpl::didReceiveServerRedirectForProvisionalLoad(WebLocalFrame*) { }

void WebFrameClientImpl::didFailProvisionalLoad(WebLocalFrame* frame, const WebURLError& error, WebHistoryCommitType type)
{
    m_loadFailed = true;
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    CefBrowserHostImpl* browser = m_webPage->browser();
    if (browser)
        browser->DidFailProvisionalLoad(frame, error, type);
#endif

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    wke::CWebViewHandler& handler = m_webPage->wkeHandler();
    if (handler.loadingFinishCallback) {
        wkeLoadingResult result = WKE_LOADING_FAILED;
        wke::CString failedReason(error.localizedDescription);
        wke::CString url(error.unreachableURL.string());

        if (error.isCancellation)
            result = WKE_LOADING_CANCELED;

        handler.loadingFinishCallback(m_webPage->wkeWebView(), handler.loadingFinishCallbackParam, &url, result, &failedReason);
    }
#endif
}

void WebFrameClientImpl::didCommitProvisionalLoad(WebLocalFrame* frame, const WebHistoryItem& history, WebHistoryCommitType type)
{
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    CefBrowserHostImpl* browser = m_webPage->browser();
    if (browser)
        browser->DidCommitProvisionalLoadForFrame(frame, history);
#endif

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    wke::CWebViewHandler& handler = m_webPage->wkeHandler();
    if (handler.urlChangedCallback) {
        String url = history.urlString();
        wke::CString string(url);
        handler.urlChangedCallback(m_webPage->wkeWebView(), handler.urlChangedCallbackParam, &string);
    }
#endif

    m_webPage->didCommitProvisionalLoad(frame, history, type);
}

void WebFrameClientImpl::didCreateNewDocument(WebLocalFrame* frame) { }

void WebFrameClientImpl::didClearWindowObject(WebLocalFrame* frame) { }

void WebFrameClientImpl::didCreateDocumentElement(WebLocalFrame* frame) { }

void WebFrameClientImpl::didReceiveTitle(WebLocalFrame* frame, const WebString& title, WebTextDirection direction)
{
    if (frame == m_webPage->mainFrame()) {
        m_title = title;
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
        wke::CWebViewHandler& handler = m_webPage->wkeHandler();
        if (handler.titleChangedCallback) {
            wke::CString string(title);
            handler.titleChangedCallback(m_webPage->wkeWebView(), handler.titleChangedCallbackParam, &string);
        }
#endif

#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
        CefBrowserHostImpl* browser = m_webPage->browser();
        if (browser)
            browser->OnTitleChange(frame, m_title);
#endif
    }
}

void WebFrameClientImpl::didChangeIcon(WebLocalFrame*, WebIconURL::Type) { }

void WebFrameClientImpl::didFinishDocumentLoad(WebLocalFrame*)
{
    m_documentReady = true;
    //     cef_load_handler_t* loadHandler = m_cefBrowserHostImpl->m_browserImpl->m_loadHandler;
    //     m_cefBrowserHostImpl->m_browserImpl->ref();
    //     loadHandler->on_loading_state_change(loadHandler, &m_cefBrowserHostImpl->m_browserImpl->m_baseClass, false, false, false);
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    wke::CWebViewHandler& handler = m_webPage->wkeHandler();
    if (handler.documentReadyCallback)
        handler.documentReadyCallback(m_webPage->wkeWebView(), handler.documentReadyCallbackParam);
#endif
}

void WebFrameClientImpl::didHandleOnloadEvents(WebLocalFrame*) { }

void WebFrameClientImpl::didFailLoad(WebLocalFrame* frame, const WebURLError& error, WebHistoryCommitType type)
{
    m_loadFailed = true;

#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    CefBrowserHostImpl* browser = m_webPage->browser();
    if (browser)
        browser->DidFailLoad(frame, error, type);
#endif

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    wke::CWebViewHandler& handler = m_webPage->wkeHandler();
    if (handler.loadingFinishCallback) {
        wkeLoadingResult result = WKE_LOADING_FAILED;
        wke::CString failedReason(error.localizedDescription);
        wke::CString url(error.unreachableURL.string());

        if (error.isCancellation)
            result = WKE_LOADING_CANCELED;

        handler.loadingFinishCallback(m_webPage->wkeWebView(), handler.loadingFinishCallbackParam, &url, result, &failedReason);
    }
#endif
}

void WebFrameClientImpl::didFinishLoad(WebLocalFrame* frame)
{
    m_loaded = true;
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    CefBrowserHostImpl* browser = m_webPage->browser();
    if (browser)
        browser->DidFinishLoad(frame);
#endif

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    wke::CWebViewHandler& handler = m_webPage->wkeHandler();
    if (handler.loadingFinishCallback) {
        wkeLoadingResult result = WKE_LOADING_SUCCEEDED;
        blink::WebFrame* frame = m_webPage->mainFrame();
        wke::CString url(frame->document().url().string());
        handler.loadingFinishCallback(m_webPage->wkeWebView(), handler.loadingFinishCallbackParam, &url, result, NULL);
    }
#endif
}

void WebFrameClientImpl::didNavigateWithinPage(WebLocalFrame*, const WebHistoryItem&, WebHistoryCommitType)
{
    //     cef_load_handler_t* loadHandler = m_cefBrowserHostImpl->m_browserImpl->m_loadHandler;
    //     m_cefBrowserHostImpl->m_browserImpl->ref();
    //     loadHandler->on_loading_state_change(loadHandler, &m_cefBrowserHostImpl->m_browserImpl->m_baseClass, false, false, false);
}

void WebFrameClientImpl::didUpdateCurrentHistoryItem(WebLocalFrame*) { }

void WebFrameClientImpl::didChangeManifest(WebLocalFrame*) { }

void WebFrameClientImpl::didChangeDefaultPresentation(WebLocalFrame*) { }
void WebFrameClientImpl::didChangeThemeColor() { }

void WebFrameClientImpl::dispatchLoad() { }

WebNavigationPolicy WebFrameClientImpl::decidePolicyForNavigation(const NavigationPolicyInfo& info)
{
    if (!m_webPage)
        return info.defaultPolicy;

#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    if (m_webPage->browser() && info.frame) {
        CefRefPtr<CefApp> application = CefContentClient::Get()->rendererApplication();
        if (!application.get())
            return info.defaultPolicy;

        CefRefPtr<CefRenderProcessHandler> handler = application->GetRenderProcessHandler();
        if (!handler.get())
            return info.defaultPolicy;

        CefStringUTF16 urlSpec;
        cef::WebStringToCefString(info.urlRequest.url().spec().utf16(), urlSpec);

        CefRefPtr<CefFrame> cefFrame = m_webPage->browser()->GetOrCreateFrame(info.frame, content::WebPage::kUnspecifiedFrameId, blink::KURL());

        CefRefPtr<CefRequest> requestPtr(CefRequest::Create());
        CefRequestImpl* requestImpl = static_cast<CefRequestImpl*>(requestPtr.get());
        requestImpl->Set(info.urlRequest);
        requestImpl->SetReadOnly(true);

        cef_navigation_type_t navigation_type = NAVIGATION_OTHER;
        switch (info.navigationType) {
        case blink::WebNavigationTypeLinkClicked:
            navigation_type = NAVIGATION_LINK_CLICKED;
            break;
        case blink::WebNavigationTypeFormSubmitted:
            navigation_type = NAVIGATION_FORM_SUBMITTED;
            break;
        case blink::WebNavigationTypeBackForward:
            navigation_type = NAVIGATION_BACK_FORWARD;
            break;
        case blink::WebNavigationTypeReload:
            navigation_type = NAVIGATION_RELOAD;
            break;
        case blink::WebNavigationTypeFormResubmitted:
            navigation_type = NAVIGATION_FORM_RESUBMITTED;
            break;
        case blink::WebNavigationTypeOther:
            navigation_type = NAVIGATION_OTHER;
            break;
        }

        if (!handler->OnBeforeNavigation((CefBrowser*)m_webPage->browser(), cefFrame.get(),
            requestPtr.get(), navigation_type, info.isRedirect))
            return info.defaultPolicy;
    }
#endif

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    if (m_webPage->wkeHandler().navigationCallback) {
        wkeNavigationType navigationType = WKE_NAVIGATION_TYPE_OTHER;
        switch (info.navigationType) {
        case blink::WebNavigationTypeLinkClicked:
            navigationType = WKE_NAVIGATION_TYPE_LINKCLICK;
            break;
        case blink::WebNavigationTypeFormSubmitted:
            navigationType = WKE_NAVIGATION_TYPE_FORMSUBMITTE;
            break;
        case blink::WebNavigationTypeBackForward:
            navigationType = WKE_NAVIGATION_TYPE_BACKFORWARD;
            break;
        case blink::WebNavigationTypeReload:
            navigationType = WKE_NAVIGATION_TYPE_RELOAD;
            break;
        case blink::WebNavigationTypeFormResubmitted:
            navigationType = WKE_NAVIGATION_TYPE_FORMRESUBMITT;
            break;
        case blink::WebNavigationTypeOther:
            navigationType = WKE_NAVIGATION_TYPE_OTHER;
            break;
        }

        wke::CString url(info.urlRequest.url().spec().utf16());
        bool ok = m_webPage->wkeHandler().navigationCallback(m_webPage->wkeWebView(), m_webPage->wkeHandler().navigationCallbackParam, navigationType, &url);
        if (!ok)
            return WebNavigationPolicyIgnore;
    }
#endif
    return info.defaultPolicy;
}

void WebFrameClientImpl::willRequestResource(WebLocalFrame*, const WebCachedURLRequest&)
{

}

void WebFrameClientImpl::didDispatchPingLoader(WebLocalFrame* webFrame, const WebURL& url)
{

}

void WebFrameClientImpl::willSendRequest(WebLocalFrame* webFrame, unsigned identifier, WebURLRequest& request, const WebURLResponse& redirectResponse)
{
    if (request.extraData()) // ResourceLoader::willSendRequest会走到这
        return;

    net::RequestExtraData* requestExtraData = new net::RequestExtraData();
    requestExtraData->frame = webFrame; // 两种模式都需要此对象
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    requestExtraData->page = m_webPage;
#endif

#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    requestExtraData->browser = m_webPage->browser();
#endif

    request.setExtraData(requestExtraData);

//     String headerFieldValue = blink::defaultLanguage();
//     headerFieldValue.append(",en,*");
// 
//     CString value = headerFieldValue.latin1().data();
     request.addHTTPHeaderField("Accept-Language", "zh-cn,zh;q=0.5");
// 
//     WebViewImpl* viewImpl = m_webPage->webViewImpl();
//     if (!viewImpl)
//         return;
//     Page* page = viewImpl->page();
//     if (!page)
//         return;
// 
//     Settings& setting = page->settings();
//     headerFieldValue = "GBK"; // setting.defaultTextEncodingName();
//     headerFieldValue.append(",utf-8;q=0.7,*;q=0.3");
//     value = headerFieldValue.latin1().data();
//     request.addHTTPHeaderField("Accept-Charset", WebString::fromLatin1((const WebLChar*)value.data(), value.length()));
}

void WebFrameClientImpl::didReceiveResponse(WebLocalFrame*, unsigned identifier, const WebURLResponse&)
{

}

void WebFrameClientImpl::didChangeResourcePriority(
    WebLocalFrame* webFrame, unsigned identifier, const WebURLRequest::Priority& priority, int)
{
}

bool WebFrameClientImpl::runModalConfirmDialog(const WebString& message)
{
    return false;
}

void WebFrameClientImpl::didCreateScriptContext(WebLocalFrame* frame, v8::Local<v8::Context> context, int extensionGroup, int worldId)
{
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    if (frame->top() == frame)
        wke::onCreateGlobalObject(this, frame, context, extensionGroup, worldId);

    if (m_webPage->wkeHandler().didCreateScriptContextCallback)
        m_webPage->wkeHandler().didCreateScriptContextCallback(m_webPage->wkeWebView(), m_webPage->wkeHandler().didCreateScriptContextCallbackParam,
            frame, &context, extensionGroup, worldId);
#endif
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    if (!CefContentClient::Get())
        return;

    CefRefPtr<CefApp> application = CefContentClient::Get()->rendererApplication();
    if (!application.get())
        return;

    CefRefPtr<CefRenderProcessHandler> handler = application->GetRenderProcessHandler();
    if (!handler.get())
        return;

    CefRefPtr<CefFrame> cefFrame = m_webPage->browser()->GetOrCreateFrame(frame, content::WebPage::kUnspecifiedFrameId, blink::KURL());
    CefRefPtr<CefV8Context> cefV8Context = new CefV8ContextImpl(context->GetIsolate(), context);
    handler->OnContextCreated((CefBrowser*)m_webPage->browser(), cefFrame.get(), cefV8Context);
#endif
}

void WebFrameClientImpl::willReleaseScriptContext(WebLocalFrame* frame, v8::Local<v8::Context> context, int worldId)
{
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    if (frame->top() == frame)
        wke::onReleaseGlobalObject(this, frame, context, worldId);

    if (m_webPage->wkeHandler().willReleaseScriptContextCallback)
        m_webPage->wkeHandler().willReleaseScriptContextCallback(m_webPage->wkeWebView(), m_webPage->wkeHandler().willReleaseScriptContextCallbackParam,
            frame, &context, worldId);
#endif
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    if (!CefContentClient::Get())
        return;

    CefRefPtr<CefApp> application = CefContentClient::Get()->rendererApplication();
    if (!application.get())
        return;

    CefRefPtr<CefRenderProcessHandler> handler = application->GetRenderProcessHandler();
    if (!handler.get())
        return;

    CefRefPtr<CefFrame> cefFrame = m_webPage->browser()->GetOrCreateFrame(frame, content::WebPage::kUnspecifiedFrameId, blink::KURL());
    CefRefPtr<CefV8Context> cefV8Context = new CefV8ContextImpl(context->GetIsolate(), context);
    handler->OnContextReleased((CefBrowser*)m_webPage->browser(), cefFrame.get(), cefV8Context);
#endif
}

} // namespace blink