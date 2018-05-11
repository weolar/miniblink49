#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "wtf/text/WTFStringUtil.h"

#include "third_party/WebKit/public/web/WebFrame.h"
#include "third_party/WebKit/public/web/WebFrameClient.h"
#include "third_party/WebKit/public/web/WebSecurityPolicy.h"
#include "third_party/WebKit/public/web/WebElement.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebHistoryItem.h"
#include "third_party/WebKit/public/web/WebDataSource.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebURLResponse.h"
#include "third_party/WebKit/Source/web/WebViewImpl.h"
#include "third_party/WebKit/Source/web/WebLocalFrameImpl.h"

#include "content/browser/WebPage.h"
#include "content/browser/WebFrameClientImpl.h"

#include "include/cef_life_span_handler.h"
#include "libcef/browser/ThreadUtil.h"
#include "libcef/browser/CefBrowserInfoManager.h"
#include "libcef/browser/CefBrowserHostImpl.h"
#include "libcef/browser/CefFrameHostImpl.h"
#include "libcef/browser/CefContext.h"
#include "libcef/common/CefTaskImpl.h"
#include "libcef/common/CefContentClient.h"
#include "libcef/common/StringUtil.h"
#include "libcef/common/CefMessages.h"
#include "libcef/common/GeometryUtil.h"

static int64 CefBrowserHostImplIdentifierCount = 0;

CefBrowserHostImpl::CefBrowserHostImpl(
    const CefBrowserSettings& settings,
    CefRefPtr<CefClient> client,
    scoped_refptr<CefBrowserInfo> browserInfo,
    CefRefPtr<CefBrowserHostImpl> opener,
    CefRefPtr<CefRequestContext> requestContext)
    : m_webPage(nullptr)
    , m_hasLMouseUp(true)
    , m_hasRMouseUp(true)
    , m_isWindowless(false)
    , m_lpfnOldWndProc(nullptr)
    , m_settings(settings)
    , m_client(client)
    , m_browserInfo(browserInfo)
    , m_opener(kNullWindowHandle)
    , m_isInOnsetfocus(false)
    , m_mouseCursorChangeDisabled(false)
    , m_windowDestroyed(false)
    , m_isLoading(false)
    , m_destructionState(DESTRUCTION_STATE_NONE)
    , m_frameDestructionPending(false)
    , m_identifier(CefBrowserHostImplIdentifierCount++) {
    DCHECK(requestContext.get());
    DCHECK(!browserInfo->Browser().get());
    browserInfo->SetBrowser(this);
    CefContext::Get()->RegisterBrowser(this);
}

CefBrowserHostImpl::~CefBrowserHostImpl() {
    CefContext::Get()->UnregisterBrowser(this);
}

void CefBrowserHostImpl::DestroyBrowser() {
    CEF_REQUIRE_UIT();

    m_destructionState = DESTRUCTION_STATE_COMPLETED;

    DetachAllFrames();

    m_browserInfo->SetBrowser(NULL);
}

void CefBrowserHostImpl::DetachAllFrames() {}
// CefBrowserHostImpl static methods.
// -----------------------------------------------------------------------------

struct CreateBrowserHostWindowArgs {
    CreateBrowserHostWindowArgs(
        const CefWindowInfo* windowInfo,
        CefRefPtr<CefClient>* client,
        const CefString* url,
        const CefBrowserSettings* settings,
        CefRefPtr<CefBrowserHostImpl>* opener,
        bool isPopup,
        CefRefPtr<CefRequestContext>* requestContext,
        CefRefPtr<CefBrowserHostImpl>* result,
        bool* waitForFinish) {
        this->windowInfo = windowInfo;
        this->client = client;
        this->url = url;
        this->settings = settings;
        this->opener = opener;
        this->isPopup = isPopup;
        this->requestContext = requestContext;
        this->result = result;
        this->waitForFinish = waitForFinish;
    }
    const CefWindowInfo* windowInfo;
    CefRefPtr<CefClient>* client;
    const CefString* url;
    const CefBrowserSettings* settings;
    CefRefPtr<CefBrowserHostImpl>* opener;
    bool isPopup;
    CefRefPtr<CefRequestContext>* requestContext;
    CefRefPtr<CefBrowserHostImpl>* result;
    bool* waitForFinish;
};

// static
CefRefPtr<CefBrowserHostImpl> CefBrowserHostImpl::Create(
    const CefWindowInfo& windowInfo,
    CefRefPtr<CefClient> client,
    const CefString& url,
    const CefBrowserSettings& settings,
    CefRefPtr<CefBrowserHostImpl> opener,
    bool isPopup,
    CefRefPtr<CefRequestContext> requestContext) {

    bool waitForFinish = false;
    CefRefPtr<CefBrowserHostImpl> browser;

    CreateBrowserHostWindowArgs args(&windowInfo, &client, &url, &settings, &opener, isPopup, &requestContext, &browser, &waitForFinish);
    if (CEF_CURRENTLY_ON_UIT()) {
        CreateAndLoadOnWebkitThread(&args);
    } else {
        CEF_POST_BLINK_TASK(CEF_UIT, WTF::bind(CreateAndLoadOnWebkitThread, &args));
    }

    // webkit线程创建窗口会向app线程的父窗口发消息，所以这里必须搞个消息循环接受一下子窗口的消息
    MSG msg = { 0 }; 
    while (!waitForFinish) {
        if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) && 0 == TranslateAccelerator(msg.hwnd, NULL, &msg)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
        }
        ::Sleep(20);
    }
    return browser.get();
}

void CefBrowserHostImpl::CreateAndLoadOnWebkitThread(CreateBrowserHostWindowArgs* args) {
    scoped_refptr<CefBrowserInfo> info = CefBrowserInfoManager::GetInstance()->CreateBrowserInfo(args->isPopup, false);
    CefRefPtr<CefBrowserHostImpl> self = CefBrowserHostImpl::CreateInternal(*(args->windowInfo), *(args->settings), *(args->client), info, *(args->opener), *(args->requestContext));
    *(args->result) = self;

    if (self.get() && !args->url->empty()) {
        self->LoadURL(content::WebPage::kMainFrameId, *(args->url), blink::Referrer(), CefString());
    }
    *(args->waitForFinish) = true;
}

CefRefPtr<CefBrowserHostImpl> CefBrowserHostImpl::CreateInternal(
    const CefWindowInfo& windowInfo,
    const CefBrowserSettings& settings,
    CefRefPtr<CefClient> client,
    scoped_refptr<CefBrowserInfo> browserInfo,
    CefRefPtr<CefBrowserHostImpl> opener,
    CefRefPtr<CefRequestContext> requestContext) {

    CEF_REQUIRE_UIT();

    content::WebPage* webPage = new content::WebPage(nullptr);

    CefRefPtr<CefBrowserHostImpl> browser = new CefBrowserHostImpl(settings, client, browserInfo, opener, requestContext);
    browser->m_isWindowless = windowInfo.windowless_rendering_enabled;
    if (!browser->CreateHostWindow(windowInfo))
        return nullptr;

    if (client.get()) {
        CefRefPtr<CefLifeSpanHandler> handler = client->GetLifeSpanHandler();
        if (handler.get())
            handler->OnAfterCreated(browser.get());
    }

    // Notify the render process handler.这本来应该在渲染进程被调用的，现在提到主进程
    CefRefPtr<CefApp> application = CefContentClient::Get()->rendererApplication();
    if (application.get()) {
        CefRefPtr<CefRenderProcessHandler> handler = application->GetRenderProcessHandler();
        if (handler.get())
            handler->OnBrowserCreated(browser.get());
    }

    return browser;
}

// static
bool CefBrowserHost::CreateBrowser(const CefWindowInfo& windowInfo,
    CefRefPtr<CefClient> client,
    const CefString& url,
    const CefBrowserSettings& settings,
    CefRefPtr<CefRequestContext> requestContext) {
    return CreateBrowserSync(windowInfo, client, url, settings, requestContext);
}

// static
CefRefPtr<CefBrowser> CefBrowserHost::CreateBrowserSync(
    const CefWindowInfo& windowInfo,
    CefRefPtr<CefClient> client,
    const CefString& url,
    const CefBrowserSettings& settings,
    CefRefPtr<CefRequestContext> requestContext) {
    // Verify that the context is in a valid state.
    if (!CONTEXT_STATE_VALID()) {
        NOTREACHED() << "context not valid";
        return false;
    }

    // Verify that the settings structure is a valid size.
    if (settings.size != sizeof(cef_browser_settings_t)) {
        NOTREACHED() << "invalid CefBrowserSettings structure size";
        return nullptr;
    }

    // Verify that this method is being called on the UI thread.
//     if (!CEF_CURRENTLY_ON_UIT()) {
//         NOTREACHED() << "called on invalid thread";
//         return nullptr;
//     }

    // Verify windowless rendering requirements.
    if (windowInfo.windowless_rendering_enabled && !client->GetRenderHandler().get()) {
        NOTREACHED() << "CefRenderHandler implementation is required";
        return nullptr;
    }

    CefRefPtr<CefBrowserHostImpl> browser = CefBrowserHostImpl::Create(windowInfo, client, url, settings, NULL, false, requestContext);
    return browser.get();
}

void CefBrowserHostImpl::LoadURL(int64 frameId, const CefString& url, const blink::Referrer& referrer, const CefString& extraHeaders) {
    if (!m_webPage)
        return;

    if (CEF_CURRENTLY_ON_UIT())
        m_webPage->loadURL(frameId, url.c_str(), referrer, extraHeaders.c_str());
    else {
        blink::KURL kurl(ParsedURLString, WTF::String(url.c_str(), url.length()));
        blink::WebURL webURL = kurl;
        blink::WebURLRequest request(webURL);
        CEF_POST_BLINK_TASK(CEF_UIT, WTF::bind(&(content::WebPage::loadRequest), m_webPage, frameId, request));
    }
}

void CefBrowserHostImpl::LoadRequest(int64 frameId, CefRefPtr<CefRequest> request) {
    if (!m_webPage)
        return;

    if (!(CEF_CURRENTLY_ON_UIT())) {
        CEF_POST_BLINK_TASK(CEF_UIT, WTF::bind(&CefBrowserHostImpl::LoadRequest, this, frameId, request));
        return;
    }

    WebURL webURL(KURL(ParsedURLString, request->GetURL().c_str()));
    WebURLRequest webRequest(webURL);

    if (!request->GetMethod().empty())
        webRequest.setHTTPMethod(WebString(request->GetMethod().c_str(), request->GetMethod().length()));

    if (!request->GetReferrerURL().empty()) {
        WebString referrer = blink::WebSecurityPolicy::generateReferrerHeader(
            static_cast<blink::WebReferrerPolicy>(request->GetReferrerPolicy()),
            webURL,
            WebString::fromUTF8(request->GetReferrerURL()));
        if (!referrer.isEmpty())
            webRequest.setHTTPHeaderField(WebString::fromUTF8("Referer"), referrer);
    }

    if (request->GetFirstPartyForCookies().empty())
        webRequest.setFirstPartyForCookies(WebURL(KURL(ParsedURLString, request->GetFirstPartyForCookies().c_str())));

    CefRequest::HeaderMap map;
    request->GetHeaderMap(map);
    for (CefRequest::HeaderMap::const_iterator header = map.begin(); header != map.end(); ++header) {
        const CefString& key = header->first;
        const CefString& value = header->second;

        if (key.empty())
            continue;

        // Delimit with "\r\n".
        webRequest.addHTTPHeaderField(WebString(key.c_str(), key.length()), WebString(value.c_str(), value.length()));
    }

    //     if (params.upload_data.get()) {
    //         base::string16 method = request.httpMethod();
    //         if (method == base::ASCIIToUTF16("GET") ||
    //             method == base::ASCIIToUTF16("HEAD")) {
    //             request.setHTTPMethod(base::ASCIIToUTF16("POST"));
    //         }
    // 
    //         if (request.httpHeaderField(
    //             base::ASCIIToUTF16("Content-Type")).length() == 0) {
    //             request.setHTTPHeaderField(
    //                 base::ASCIIToUTF16("Content-Type"),
    //                 base::ASCIIToUTF16("application/x-www-form-urlencoded"));
    //         }
    // 
    //         blink::WebHTTPBody body;
    //         body.initialize();
    // 
    //         const ScopedVector<net::UploadElement>& elements =
    //             params.upload_data->elements();
    //         ScopedVector<net::UploadElement>::const_iterator it =
    //             elements.begin();
    //         for (; it != elements.end(); ++it) {
    //             const net::UploadElement& element = **it;
    //             if (element.type() == net::UploadElement::TYPE_BYTES) {
    //                 blink::WebData data;
    //                 data.assign(element.bytes(), element.bytes_length());
    //                 body.appendData(data);
    //             }
    //             else if (element.type() == net::UploadElement::TYPE_FILE) {
    //                 body.appendFile(FilePathStringToWebString(element.file_path().value()));
    //             }
    //             else {
    //                 NOTREACHED();
    //             }
    //         }
    // 
    //         request.setHTTPBody(body);
    //     }

    m_webPage->loadRequest(frameId, webRequest);
}

// Load the specified string.
void CefBrowserHostImpl::LoadString(int64 frameId, const CefString& string, const CefString& url) {
    if (!(CEF_CURRENTLY_ON_UIT())) {
        CEF_POST_BLINK_TASK(CEF_UIT, WTF::bind(&CefBrowserHostImpl::LoadString, this, frameId, string, url));
        return;
    }

    if (0 == string.length())
        return;

    Vector<char> data = WTF::ensureStringToUTF8(WTF::String(string.c_str(), string.length()), false);
    WebData html(data.data(), data.size());

    WTF::String urlStr = ensureStringToUTF8String(WTF::String(url.c_str(), url.length()));
    WebURL baseURL(KURL(ParsedURLString, urlStr));
    m_webPage->loadHTMLString(frameId, html, baseURL);
}

CefRefPtr<CefBrowserHost> CefBrowserHostImpl::GetHost() {
    return this;
}

CefWindowHandle CefBrowserHostImpl::GetWindowHandle() {
    if (!m_webPage)
        return kNullWindowHandle;
    return m_webPage->getHWND();
}

void CefBrowserHostImpl::OnPaintUpdated(const uint32_t* buffer, const CefRect& r, int width, int height) {
    if (!m_client || !m_client->GetRenderHandler().get())
        return;
    CefRefPtr<CefRenderHandler> render = m_client->GetRenderHandler();

    CefRenderHandler::RectList dirtyRects;
    dirtyRects.push_back(r);
    render->OnPaint(this, PET_VIEW, dirtyRects, buffer, width, height);
}

void CefBrowserHostImpl::OnLoadingStateChange(bool isLoading, bool toDifferentDocument) {
    MutexLocker locker(m_stateLock);
    m_isLoading = isLoading;
}

bool CefBrowserHostImpl::IsLoading() {
    bool loading = false;
    {
        MutexLocker locker(m_stateLock);
        loading = m_isLoading;
    }
    return loading;
}

void CefBrowserHostImpl::Reload() {
    if (!(CEF_CURRENTLY_ON_UIT())) {
        CEF_POST_BLINK_TASK(CEF_UIT, WTF::bind(&CefBrowserHostImpl::Reload, this));
        return;
    }

    if (!m_webPage || !m_webPage->webViewImpl())
        return;
    blink::WebFrame* frame = m_webPage->mainFrame();
    if (!frame)
        return;

    frame->reload(true);
}

bool CefBrowserHostImpl::CanGoBack() {
    if (!m_webPage || !m_webPage->webViewImpl())
        return false;
    return m_webPage->canGoBack();
}

void CefBrowserHostImpl::GoBack() {
    if (!m_webPage || !m_webPage->webViewImpl())
        return;
    m_webPage->goBack();
}

bool CefBrowserHostImpl::CanGoForward() {
    if (!m_webPage || !m_webPage->webViewImpl())
        return false;
    return m_webPage->canGoForward();
}

void CefBrowserHostImpl::GoForward() {
    if (!m_webPage || !m_webPage->webViewImpl())
        return;
    m_webPage->goForward();
}

void CefBrowserHostImpl::ReloadIgnoreCache() {}

void CefBrowserHostImpl::StopLoad() {
    if (m_webPage->mainFrame())
        m_webPage->mainFrame()->stopLoading();
}

int CefBrowserHostImpl::GetIdentifier() {
//     if (!m_webPage)
//         return content::WebPage::kInvalidFrameId;
//     
//     blink::WebFrame* frame = m_webPage->mainFrame();
//     return CefFrameHostImpl::GetFrameIdByBlinkFrame(frame);
    return m_identifier;
}

bool CefBrowserHostImpl::IsSame(CefRefPtr<CefBrowser> that) {
    return GetIdentifier() == that->GetIdentifier();
}

bool CefBrowserHostImpl::IsPopup() { return false; }
bool CefBrowserHostImpl::HasDocument() { return false; }

CefRefPtr<CefFrame> CefBrowserHostImpl::GetOrCreateFrame(const blink::WebLocalFrame* webFrame, int64 parentFrameId, const blink::KURL& frameUrl) {
    int64 frameId = CefFrameHostImpl::GetFrameIdByBlinkFrame(webFrame);
    String frameName = CefFrameHostImpl::GetFrameNameByBlinkFrame(webFrame);
    bool isMainFrame = (nullptr == webFrame->parent());

    //ASSERT(frameId > content::WebPage::kInvalidFrameId);
    if (frameId <= content::WebPage::kInvalidFrameId)
        return NULL;

    CefString url;
    if (frameUrl.isValid())
        url = WTFStringToStdString(frameUrl.string());

    CefString name;
    if (!frameName.isEmpty())
         cef::WTFStringToCefString(frameName, name);

    CefRefPtr<CefFrameHostImpl> frame;
    bool frameCreated = false;

    {
        MutexLocker locker(m_stateLock);

        if (isMainFrame)
            m_mainFrameId = frameId;

        // Check if a frame object already exists.
        FrameMap::const_iterator it = m_frames.find(frameId);
        if (it != m_frames.end())
            frame = it->second.get();

        if (!frame.get()) {
            frame = new CefFrameHostImpl(this, frameId, isMainFrame, url, name, parentFrameId);
            frameCreated = true;
            m_frames.insert(std::make_pair(frameId, frame));
        }
    }

    if (!frameCreated)
        frame->SetAttributes(url, name, parentFrameId);

    return frame.get();
}

CefRefPtr<CefFrame> CefBrowserHostImpl::GetMainFrame() {
    return GetFrame(content::WebPage::kMainFrameId);
}

CefRefPtr<CefFrame> CefBrowserHostImpl::GetFocusedFrame() {
    return GetFrame(content::WebPage::kFocusedFrameId);
}

CefRefPtr<CefFrame> CefBrowserHostImpl::GetFrame(int64 identifier) { 
    MutexLocker locker(m_stateLock);

    if (m_mainFrameId == content::WebPage::kInvalidFrameId) {
        // A main frame does not exist yet. Return the placeholder frame that
        // provides limited functionality.
        return nullptr; // placeholder_frame_.get();
    }

    if (identifier == content::WebPage::kMainFrameId) {
        identifier = m_mainFrameId;
    } else if (identifier == content::WebPage::kFocusedFrameId) {
        // Return the main frame if no focused frame is currently identified.
        if (m_focusedFrameId == content::WebPage::kInvalidFrameId)
            identifier = m_mainFrameId;
        else
            identifier = m_focusedFrameId;
    }

    if (identifier == content::WebPage::kInvalidFrameId)
        return nullptr;

    FrameMap::const_iterator it = m_frames.find(identifier);
    if (it != m_frames.end())
        return it->second.get();

    return nullptr;
}

CefRefPtr<CefFrame> CefBrowserHostImpl::GetFrame(const CefString& name) { 
    MutexLocker locker(m_stateLock);

    FrameMap::const_iterator it = m_frames.begin();
    for (; it != m_frames.end(); ++it) {
        if (it->second->GetName() == name)
            return it->second.get();
    }

    return nullptr;
}

size_t CefBrowserHostImpl::GetFrameCount() {
    MutexLocker locker(m_stateLock);
    return m_frames.size();
}
void CefBrowserHostImpl::GetFrameIdentifiers(std::vector<int64>& identifiers) {
    MutexLocker locker(m_stateLock);

    if (identifiers.size() > 0)
        identifiers.clear();

    FrameMap::const_iterator it = m_frames.begin();
    for (; it != m_frames.end(); ++it)
        identifiers.push_back(it->first);
}

void CefBrowserHostImpl::GetFrameNames(std::vector<CefString>& names) {
    MutexLocker locker(m_stateLock);

    if (names.size() > 0)
        names.clear();

    FrameMap::const_iterator it = m_frames.begin();
    for (; it != m_frames.end(); ++it)
        names.push_back(it->second->GetName());
}

bool CefBrowserHostImpl::SendProcessMessage(CefProcessId targetProcess, CefRefPtr<CefProcessMessage> message) {
    bool b = false;
    if (PID_BROWSER == targetProcess) {
        b = m_client->OnProcessMessageReceived(this, PID_RENDERER, message);
    } else if (PID_RENDERER == targetProcess) {
        CefRefPtr<CefApp> application = CefContentClient::Get()->rendererApplication();
        if (application.get()) {
            CefRefPtr<CefRenderProcessHandler> handler = application->GetRenderProcessHandler();
            if (handler.get())
                b = handler->OnProcessMessageReceived(this, PID_BROWSER, message);
        }
    }
    
    return b;
}

void CefBrowserHostImpl::SendCommand(const Cef_Request_Params* request, Cef_Response_Params* response) {
    if (!(CEF_CURRENTLY_ON_UIT())) {
        CEF_POST_BLINK_TASK(CEF_UIT, WTF::bind(&CefBrowserHostImpl::SendCommand, this, request, response));
        return;
    }

    int64 frameId = request->frameId;
    blink::WebFrame* webFrame = m_webPage->getWebFrameFromFrameId(frameId);
    if (!webFrame) {
        delete request;
        return;
    }
    
    bool success = false;
    if ("execute-command" == request->name) {
        if (0 == request->arguments.GetSize()) {
            delete request;
            return;
        }

        const base::Value* argument;
        if (!request->arguments.Get(0, &argument)) {
            delete request;
            return;
        }

        std::string outValue;
        if (!argument->GetAsString(&outValue) || 0 == outValue.size()) {
            delete request;
            return;
        }

        String command = outValue.c_str();
 
        if (equalIgnoringCase(command.impl(), "getsource") && response) {
            response->response = webFrame->contentAsMarkup();
            success = true;
        } else if (equalIgnoringCase(command.impl(), "gettext") && response) {
            blink::WebElement documentElement = webFrame->document().documentElement();
            if (!documentElement.isNull()) {
                blink::Element* element = documentElement.unwrap<blink::Element>();

                response->response = element->innerText();
                success = true;
            }
        } else if (webFrame->executeCommand(blink::WebString(command)) && response) {
            success = true;
        }
    }
    if (response)
        response->success = success;
}

CefRefPtr<CefBrowser> CefBrowserHostImpl::GetBrowser() {
    return this; 
}

void CefBrowserHostImpl::CloseBrowser(bool forceClose) {
    bool closeBrowser = false;
    if (m_client.get()) {
        CefRefPtr<CefLifeSpanHandler> handler = m_client->GetLifeSpanHandler();

        if (handler.get())
            closeBrowser = !handler->DoClose(this);
    }

    if (!closeBrowser)
        return;

    if (!m_windowDestroyed) {
        CloseHostWindow();
        return;
    }

    CEF_REQUIRE_UIT();

    if (m_client.get()) {
        CefRefPtr<CefLifeSpanHandler> handler = m_client->GetLifeSpanHandler();
        if (handler.get()) {
            // Notify the handler that the window is about to be closed.
            handler->OnBeforeClose(this);
        }
    }

    if (m_webPage)
        m_webPage->close();

    DestroyBrowser();
}

void CefBrowserHostImpl::CloseHostWindow() {
    if (m_webPage && m_webPage->getHWND() && !IsWindowless()) {
        HWND frameWnd = ::GetAncestor(m_webPage->getHWND(), GA_ROOT);
        ::PostMessage(frameWnd, WM_CLOSE, 0, 0);
    } else if (IsWindowless()) {
        ::PostMessage(m_webPage->getHWND(), WM_CLOSE, 0, 0);
    }
}

void CefBrowserHostImpl::WindowDestroyed() {
    CEF_REQUIRE_UIT();
    DCHECK(!m_windowDestroyed);
    m_windowDestroyed = true;
    CloseBrowser(true);
}

void CefBrowserHostImpl::OnSetFocus(cef_focus_source_t source) {
    if (CEF_CURRENTLY_ON_UIT()) {
        // SetFocus() might be called while inside the OnSetFocus() callback. If so,
        // don't re-enter the callback.
        if (!m_isInOnsetfocus) {
            if (m_client.get()) {
                CefRefPtr<CefFocusHandler> handler = m_client->GetFocusHandler();
                if (handler.get()) {
                    m_isInOnsetfocus = true;
                    bool handled = handler->OnSetFocus(this, source);
                    m_isInOnsetfocus = false;

                    if (handled)
                        return;
                }
            }
        }

        PlatformSetFocus(true);
    } else {
        CEF_POST_BLINK_TASK(CEF_UIT, WTF::bind(&CefBrowserHostImpl::OnSetFocus, this, source));
    }
}

void CefBrowserHostImpl::SetFocus(bool focus) {
    if (focus) {
        OnSetFocus(FOCUS_SOURCE_SYSTEM);
    } else {
        if (CEF_CURRENTLY_ON_UIT()) {
            PlatformSetFocus(false);
        } else {
            CEF_POST_BLINK_TASK(CEF_UIT, WTF::bind(&CefBrowserHostImpl::PlatformSetFocus, this, false));
        }
    }
}

void CefBrowserHostImpl::SetMouseCursorChangeDisabled(bool disabled) {
    MutexLocker locker(m_stateLock);
    m_mouseCursorChangeDisabled = disabled;
}

bool CefBrowserHostImpl::IsMouseCursorChangeDisabled() {
    MutexLocker locker(m_stateLock);
    return m_mouseCursorChangeDisabled;
}

void CefBrowserHostImpl::WasResized() {
    if (!m_client || !m_client->GetRenderHandler().get())
        return;
    CefRefPtr<CefRenderHandler> render = m_client->GetRenderHandler();
    CefRect rect;

    render->GetScreenInfo(this, m_screenInfo);
    rect = m_screenInfo.rect;
    float scale = m_screenInfo.device_scale_factor;
    m_webPage->fireResizeEvent(nullptr, WM_SIZE, 0, 
        MAKELPARAM(cef::LogicalToDevice(rect.width, scale), cef::LogicalToDevice(rect.height, scale)));
}

void CefBrowserHostImpl::WasHidden(bool hidden) {
}

CefBrowserHostImpl* CefBrowserHostImpl::GetBrowserForMainFrame(blink::WebFrame* webFrame) {
    CEF_REQUIRE_UIT();

    blink::Frame* frame = blink::toWebLocalFrameImpl(webFrame)->frame();
    if (!frame)
        return NULL;
    ChromeClient& client = frame->chromeClient();
    blink::WebViewImpl* webViewImpl = (blink::WebViewImpl*)client.webView();
    if (!webViewImpl)
        return NULL;

    const CefContext::BrowserList& list = CefContext::Get()->GetBrowserList();
    for (auto it = list.begin(); it != list.end(); ++it) {
        CefBrowserHostImpl* browser = *it;
        if (!browser->m_webPage)
            continue;
        if (webViewImpl == browser->m_webPage->webViewImpl())
            return browser;
    }
    return NULL;
}

void CefBrowserHostImpl::DidCommitProvisionalLoadForFrame(blink::WebLocalFrame* frame, const blink::WebHistoryItem& history) {
    CefString url;
    cef::WebStringToCefString(history.urlString(), url);
    bool isMainFrame = (nullptr == frame->parent());

    CefRefPtr<CefFrame> cefFrame = GetOrCreateFrame(frame, content::WebPage::kUnspecifiedFrameId, blink::KURL(blink::ParsedURLString, history.urlString()));

    OnLoadStart(cefFrame);
    if (isMainFrame)
        OnAddressChange(cefFrame, url);
}

void CefBrowserHostImpl::DidStartProvisionalLoad(blink::WebLocalFrame* frame, double triggeringEventTime) {
    // Send the frame creation notification if necessary.
    GetOrCreateFrame(frame, content::WebPage::kUnspecifiedFrameId, blink::KURL());
}

void CefBrowserHostImpl::DidFailProvisionalLoad(blink::WebLocalFrame* frame, const blink::WebURLError& error, blink::WebHistoryCommitType) {
    bool isMainFrame = (nullptr == frame->parent());
    CefRefPtr<CefFrame> cefFrame = GetOrCreateFrame(frame, content::WebPage::kUnspecifiedFrameId, blink::KURL());
    OnLoadError(cefFrame, error.unreachableURL, error.reason, error.localizedDescription);
}

void CefBrowserHostImpl::OnFrameIdentified(blink::WebLocalFrame* frame, blink::WebLocalFrame* parent) {
    CefRefPtr<CefFrame> cefFrame = GetOrCreateFrame(frame, CefFrameHostImpl::GetFrameIdByBlinkFrame(parent), blink::KURL());
}

void CefBrowserHostImpl::DidFinishLoad(blink::WebLocalFrame* frame) {
    CefRefPtr<CefFrame> cefFrame = GetOrCreateFrame(frame, content::WebPage::kUnspecifiedFrameId, blink::KURL());
    if (!m_client.get())
        return;

    CefRefPtr<CefLoadHandler> handler = m_client->GetLoadHandler();
    if (!handler.get())
        return;

    int httpStatusCode = frame->dataSource()->response().httpStatusCode();
    handler->OnLoadEnd(this, cefFrame, 200);
}

void CefBrowserHostImpl::DidFailLoad(blink::WebLocalFrame* frame, const WebURLError& error, WebHistoryCommitType type) {
    CefRefPtr<CefFrame> cefFrame = GetOrCreateFrame(frame, content::WebPage::kUnspecifiedFrameId, blink::KURL());

    if (!m_client.get())
        return;

    CefRefPtr<CefLoadHandler> handler = m_client->GetLoadHandler();
    if (!handler.get())
        return;
}

void CefBrowserHostImpl::OnLoadError(CefRefPtr<CefFrame> frame, const blink::KURL& kurl, int errorCode, const WTF::String& errorDescription) {
    if (!m_client.get())
        return;

    CefRefPtr<CefLoadHandler> handler = m_client->GetLoadHandler();
    if (!handler.get())
        return;

    m_frameDestructionPending = true;
    // Notify the handler that loading has failed.
    CefString url;
    cef::WTFStringToCefString(kurl.string(), url);

    CefString cefErrorDescription;
    cef::WTFStringToCefString(errorDescription, cefErrorDescription);
    handler->OnLoadError(this, frame, static_cast<cef_errorcode_t>(errorCode), CefString(cefErrorDescription), url);
    m_frameDestructionPending = false;
}

void CefBrowserHostImpl::OnLoadStart(CefRefPtr<CefFrame> frame) {
    if (m_client.get()) {
        CefRefPtr<CefLoadHandler> handler = m_client->GetLoadHandler();
        if (handler.get()) {
            // Notify the handler that loading has started.
            handler->OnLoadStart(this, frame);
        }
    }
}

void CefBrowserHostImpl::OnAddressChange(CefRefPtr<CefFrame> frame, const CefString& url) {
    if (m_client.get()) {
        CefRefPtr<CefDisplayHandler> handler = m_client->GetDisplayHandler();
        if (handler.get()) {
            // Notify the handler of an address change.
            handler->OnAddressChange(this, frame, url);
        }
    }
}

void CefBrowserHostImpl::OnTitleChange(blink::WebLocalFrame* frame, const String& title) {
    if (!m_client.get())
        return;

    CefString cefTitle;

    CefRefPtr<CefDisplayHandler> handler = m_client->GetDisplayHandler();
    if (handler.get()) {
        CefString url;
        cef::WebStringToCefString(title, cefTitle);
        handler->OnTitleChange(GetBrowser(), cefTitle);
    }
}

#endif