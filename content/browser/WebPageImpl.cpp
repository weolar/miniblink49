
#include "content/browser/WebPageImpl.h"

#include "base/basictypes.h"
#include "base/rand_util.h"

#include "third_party/WebKit/Source/wtf/text/qt4/UnicodeQt4.h"
#include "third_party/WebKit/Source/core/frame/Frame.h"
#include "third_party/WebKit/Source/core/frame/Settings.h"
#include "third_party/WebKit/Source/core/frame/FrameView.h"
#include "third_party/WebKit/Source/core/frame/LocalFrame.h"
#include "third_party/WebKit/Source/core/page/Page.h"
#include "third_party/WebKit/Source/core/page/FocusController.h"
#include "third_party/WebKit/Source/core/input/EventHandler.h"
#include "third_party/WebKit/Source/core/loader/FrameLoadRequest.h"
#include "third_party/WebKit/Source/core/fetch/MemoryCache.h"
#include "third_party/WebKit/Source/core/editing/FrameSelection.h"
#include "third_party/WebKit/Source/core/editing/Editor.h"
#include "third_party/WebKit/Source/platform/Task.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/platform/weborigin/SecurityOrigin.h"
#include "third_party/WebKit/Source/platform/graphics/GraphicsContext.h"
#include "third_party/WebKit/Source/platform/graphics/paint/DisplayItemList.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "third_party/WebKit/Source/platform/geometry/win/IntRectWin.h"
#include "third_party/WebKit/Source/platform/win/PlatformMouseEventWin.h"
#include "third_party/WebKit/Source/platform/network/ResourceRequest.h"
#include "third_party/WebKit/Source/platform/image-encoders/gdiplus/GDIPlusImageEncoder.h" // TODO
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebWindowFeatures.h"
#include "third_party/WebKit/public/web/WebKit.h"
#include "third_party/WebKit/public/web/WebFrameClient.h"
#include "third_party/WebKit/Source/web/WebViewImpl.h"
#include "third_party/WebKit/Source/web/FrameLoaderClientImpl.h"
#include "third_party/WebKit/Source/web/WebLocalFrameImpl.h"
#include "third_party/WebKit/Source/web/WebSettingsImpl.h"
#include "third_party/WebKit/Source/wtf/MainThread.h"
#include "third_party/WebKit/Source/wtf/Functional.h"
#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "third_party/WebKit/Source/bindings/core/v8/V8GCController.h"

#include "skia/ext/bitmap_platform_device_win.h"

#include "content/browser/WebPage.h"
#include "content/browser/PlatformEventHandler.h"
#include "content/browser/PopupMenuWin.h"
#include "content/browser/WebFrameClientImpl.h"
#include "content/browser/NavigationController.h"
#include "content/browser/CheckReEnter.h"
#include "content/web_impl_win/BlinkPlatformImpl.h"
#include "content/web_impl_win/WebThreadImpl.h"
#include "content/web_impl_win/npapi/PluginDatabase.h"

#include "cc/trees/LayerTreeHost.h"
#include "cc/base/BdColor.h"

#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
#include "cef/libcef/browser/CefBrowserHostImpl.h"
#include "cef/libcef/browser/CefBrowserInfoManager.h"
#include "cef/libcef/browser/RequestImpl.h"
#include "cef/libcef/browser/ThreadUtil.h" // TODO
#include "cef/libcef/common/CefContentClient.h"
#include "cef/include/capi/cef_render_process_handler_capi.h"
#endif
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
#include "wke/wkeWebView.h"
#include "wke/wkeJsBindFreeTempObject.h"
#include "wke/wkeWebWindow.h"
extern bool wkeIsUpdataInOtherThread;
#endif

#include "content/browser/ToolTip.h"

using namespace blink;

extern bool g_drawDirtyDebugLine;

namespace blink {
bool saveDumpFile(const String& url, char* buffer, unsigned int size);
}

namespace content {

void WebPageImpl::initBlink()
{
    BlinkPlatformImpl::initialize();
}

void WebPageImpl::registerDestroyNotif(DestroyNotif* destroyNotif)
{
    size_t pos = m_destroyNotifs.find(destroyNotif);
    if (WTF::kNotFound == pos)
        m_destroyNotifs.append(destroyNotif);
}

void WebPageImpl::unregisterDestroyNotif(DestroyNotif* destroyNotif)
{
    size_t pos = m_destroyNotifs.find(destroyNotif);
    if (WTF::kNotFound != pos)
        m_destroyNotifs.remove(pos);
}

WebPageImpl::WebPageImpl()
{
    m_pagePtr = 0;
    m_bdColor = RGB(199, 237, 204) | 0xff000000;
    m_webViewImpl = nullptr;
    m_debugCount = 0;
    m_enterCount = 0;
    m_hWnd = NULL;
    m_state = pageUninited;
    m_platformEventHandler = nullptr;
    m_postMouseLeave = false;
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    m_browser = nullptr;
#endif
    m_needsCommit = 0;
    m_commitCount = 0;
    m_needsLayout = 1;
    m_layerDirty = 1;
    m_executeMainFrameCount = 0;
    m_lastFrameTimeMonotonic = 0;
    m_popupHandle = nullptr;
    m_postCloseWidgetSoonMessage = false;
    m_navigationController = new NavigationController(this);
    m_layerTreeHost = new cc::LayerTreeHost(this, this);
    m_memoryCanvasForUi = nullptr;
    m_disablePaint = false;
    m_firstDrawCount = 0;
    m_webFrameClient = new content::WebFrameClientImpl();

    m_toolTip = new ToolTip();
    m_toolTip->init();
    
    WebLocalFrameImpl* webLocalFrameImpl = (WebLocalFrameImpl*)WebLocalFrame::create(WebTreeScopeType::Document, m_webFrameClient);
    m_webViewImpl = WebViewImpl::create(this);
    m_webViewImpl->setMainFrame(webLocalFrameImpl);
    initSetting();

    m_platformEventHandler = new PlatformEventHandler(m_webViewImpl, m_webViewImpl);

    m_layerTreeHost->setWebGestureCurveTarget(m_webViewImpl);
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    if (CefContentClient::Get()) {
        CefRefPtr<CefApp> application = CefContentClient::Get()->rendererApplication();
        if (!application.get())
            return;

        CefRefPtr<CefRenderProcessHandler> handler = application->GetRenderProcessHandler();
        if (!handler.get())
            return;
        handler->OnWebKitInitialized();
    }
#endif
}

WebPageImpl::~WebPageImpl()
{
    ASSERT(pageDestroyed == m_state);
    m_state = pageDestroyed;

    delete m_toolTip;

    if (m_memoryCanvasForUi)
        delete m_memoryCanvasForUi;
    m_memoryCanvasForUi = nullptr;
    
    delete m_navigationController;
    m_navigationController = nullptr;

    delete m_layerTreeHost;
    m_layerTreeHost = nullptr;

    // 在Page::~Page()中销毁
    if (m_webFrameClient)
        delete m_webFrameClient;
    m_webFrameClient = nullptr;

    delete m_platformEventHandler;
    m_platformEventHandler = nullptr;

    m_pagePtr = 0;
    m_popupHandle = nullptr;

    BlinkPlatformImpl* platform = (BlinkPlatformImpl*)blink::Platform::current();
    platform->startGarbageCollectedThread(0);
}

bool WebPageImpl::checkForRepeatEnter()
{
    if (m_enterCount == 0 && 0 == CheckReEnter::s_kEnterContent)
        return true;
    return false;
}

class AutoRecordActions {
public:
    AutoRecordActions(WebPageImpl* page, cc::LayerTreeHost* host, bool isComefromMainFrame)
    {
        m_host = host;
        m_page = page;
        m_isComefromMainFrame = isComefromMainFrame;
        if (!m_host)
            return;

        m_lastFrameTimeMonotonic = WTF::monotonicallyIncreasingTime();

        int layerDirty = m_page->m_layerDirty; 
        int needsLayout = m_page->m_needsLayout; 
        if (m_isComefromMainFrame) {
            InterlockedExchange(reinterpret_cast<long volatile*>(&m_page->m_layerDirty), 0);
            InterlockedExchange(reinterpret_cast<long volatile*>(&m_page->m_needsLayout), 0);
        }
        m_isLayout = (0 != layerDirty || 0 != needsLayout);
        m_host->beginRecordActions(isComefromMainFrame);
    }

    ~AutoRecordActions()
    {
        if (!m_host)
            return;
        
        int layerDirty = m_page->m_layerDirty;
        int needsLayout = m_page->m_needsLayout;
        if (m_isComefromMainFrame) {
            InterlockedExchange(reinterpret_cast<long volatile*>(&m_page->m_layerDirty), 0);
            InterlockedExchange(reinterpret_cast<long volatile*>(&m_page->m_needsLayout), 0);
        }
        bool isLayout = 0 != layerDirty || 0 != needsLayout;

        if ((isLayout || m_isLayout) && m_isComefromMainFrame) {
            WebBeginFrameArgs frameArgs(m_lastFrameTimeMonotonic, 0, m_lastFrameTimeMonotonic - m_page->m_lastFrameTimeMonotonic);
            m_page->m_webViewImpl->beginFrame(frameArgs);
            m_page->m_webViewImpl->layout();
        }

        if (m_isComefromMainFrame)
            m_host->recordDraw();
        else if (isLayout || m_isLayout)
            m_page->setNeedsCommitAndNotLayout();

        m_host->endRecordActions();

        m_page->m_lastFrameTimeMonotonic = m_lastFrameTimeMonotonic;
    }

private:
    WebPageImpl* m_page;
    cc::LayerTreeHost* m_host;
    bool m_isLayout;
    bool m_isComefromMainFrame;
    double m_lastFrameTimeMonotonic;
};

#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
WebView* WebPageImpl::createCefView(WebLocalFrame* creator,
    const WebURLRequest& request,
    const WebWindowFeatures& features,
    const WebString& name,
    WebNavigationPolicy policy,
    bool suppressOpener)
{
    scoped_refptr<CefBrowserHostImpl> browserHostImpl = CefBrowserInfoManager::GetInstance()->CreateBrowserHostIfAllow(
        m_pagePtr, creator, request, features, name, policy, suppressOpener);
    if (!browserHostImpl)
        return nullptr;
    if (!browserHostImpl->webPage())
        return nullptr;
    return browserHostImpl->webPage()->webViewImpl();
}
#endif

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
static WebView* createWkeViewDefault(HWND parent, const WebString& name, const WTF::CString& url)
{
    wke::CWebWindow* window = new wke::CWebWindow();
    WTF::String nameString = name;
    Vector<UChar> nameBuf = WTF::ensureUTF16UChar(nameString, true);

    window->create(parent, WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 100, 100, 570, 570);

    WebPage* webPage = window->webPage();
    if (!webPage)
        return nullptr;

    window->setName((const wchar_t*)nameBuf.data());
    window->loadURL(url.data());
    ::ShowWindow(window->windowHandle(), 1);
    ::UpdateWindow(window->windowHandle());

    return webPage->webViewImpl();
}

WebView* WebPageImpl::createWkeView(WebLocalFrame* creator,
    const WebURLRequest& request,
    const WebWindowFeatures& features,
    const WebString& name,
    WebNavigationPolicy policy,
    bool suppressOpener)
{
    WTF::CString url = request.url().spec();

    wke::CWebViewHandler& handler = m_pagePtr->wkeHandler();
    if (!handler.createViewCallback)
        return createWkeViewDefault(m_hWnd, name, url);

    wkeNavigationType type = WKE_NAVIGATION_TYPE_LINKCLICK;
    wke::CString wkeUrl(url.data(), url.length());
    wkeWindowFeatures windowFeatures;
    windowFeatures.x = features.xSet ? features.x : CW_USEDEFAULT;
    windowFeatures.y = features.ySet ? features.y : CW_USEDEFAULT;
    windowFeatures.width = features.widthSet ? features.width : CW_USEDEFAULT;
    windowFeatures.height = features.heightSet ? features.height : CW_USEDEFAULT;
    windowFeatures.locationBarVisible = features.locationBarVisible;
    windowFeatures.menuBarVisible = features.menuBarVisible;
    windowFeatures.resizable = features.resizable;
    windowFeatures.statusBarVisible = features.statusBarVisible;
    windowFeatures.toolBarVisible = features.toolBarVisible;
    windowFeatures.fullscreen = features.fullscreen;

    wke::CWebView* createdWebView = handler.createViewCallback(m_pagePtr->wkeWebView(), handler.createViewCallbackParam, type, &wkeUrl, &windowFeatures);
    if (!createdWebView)
        //return createWkeViewDefault(m_hWnd, name, url);
        return nullptr;

    if (!createdWebView->webPage())
        return nullptr; 
    return createdWebView->webPage()->webViewImpl();
}
#endif

WebView* WebPageImpl::createView(WebLocalFrame* creator,
    const WebURLRequest& request,
    const WebWindowFeatures& features,
    const WebString& name,
    WebNavigationPolicy policy,
    bool suppressOpener)
{
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    if (m_pagePtr->wkeWebView())
        return createWkeView(creator, request, features, name, policy, suppressOpener);
#endif
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    return createCefView(creator, request, features, name, policy, suppressOpener);
#else
    return nullptr;
#endif
}

void WebPageImpl::init(WebPage* pagePtr, HWND hWnd)
{
    m_hWnd = hWnd;

    LONG windowStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
    bool useLayeredBuffer = !!(windowStyle & WS_EX_LAYERED);
    m_layerTreeHost->setHasTransparentBackground(useLayeredBuffer);

    m_pagePtr = pagePtr;
    m_webFrameClient->setWebPage(m_pagePtr);

    m_webViewImpl->setFocus(true);

    m_state = pageInited;
}

void WebPageImpl::testPaint()
{
}

void WebPageImpl::freeV8TempObejctOnOneFrameBefore()
{
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    wke::freeV8TempObejctOnOneFrameBefore();
#endif
}

void WebPageImpl::close()
{
    ASSERT(isMainThread());
    if (pageInited != m_state)
        return;

    if (m_enterCount == 0) { // 把dragState().m_dragSrc之类的占用抵消
        WTF::TemporaryChange<int> temporaryChange(m_enterCount, 0);
        LPARAM lParam = MAKELONG(-10000, -10000);
        fireMouseEvent(m_hWnd, WM_LBUTTONUP, 0, lParam, nullptr);
        fireMouseEvent(m_hWnd, WM_MOUSEMOVE, 0, lParam, nullptr);
        fireMouseEvent(m_hWnd, WM_LBUTTONDOWN, 0, lParam, nullptr);
        fireMouseEvent(m_hWnd, WM_MBUTTONUP, 0, lParam, nullptr);
        fireMouseEvent(m_hWnd, WM_RBUTTONUP, 0, lParam, nullptr);
    }

    m_state = pageDestroying;
    
    // 在KFrameLoaderClient::frameLoaderDestroyed()也会调用到此，所以在给脚本发消息的时候注意一下
    // WebCore::WebPage::windowCloseRequested+0x1a           
    // WebCore::KFrameLoaderClient::frameLoaderDestroyed+0xc  
    // WebCore::FrameLoader::~FrameLoader+0x7e                
    // WebCore::Frame::~Frame+0xd0                            
    // WTF::RefCounted<WebCore::Frame>::deref+0x16            
    // WebCore::Frame::lifeSupportTimerFired+0x5              
    // WebCore::Timer<WebCore::MainResourceLoader>::fired+0xc 
    // WebCore::ThreadTimers::sharedTimerFiredInternal+0x8b   
    // WebCore::ThreadTimers::sharedTimerFired+0xe            
    // WebCore::WebPage::windowCloseRequested+0x99
    // 所以这里可能有重入
    //SharedTimerKd::inst()->timerEvent(); // 很多异步清理资源的工作会放在定时器里，所以最后再执行一遍

    // TODO_Weolar
    // 发生unload消息给脚本

    // 清空webkit的资源缓存
    //memoryCache()->evictResources();

    if (m_enterCount <= 1)
        doClose();
}

void WebPageImpl::doClose()
{
    m_layerTreeHost->requestApplyActionsToRunIntoCompositeThread(false);
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    if (!m_pagePtr->wkeHandler().isWke) {
#endif
        if (m_hWnd) {
            ::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, 0);
            ::KillTimer(m_hWnd, (UINT_PTR)this);
            ::DestroyWindow(m_hWnd);
        }
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    }
#endif

    content::WebThreadImpl* threadImpl = nullptr;
    threadImpl = (content::WebThreadImpl*)(blink::Platform::current()->currentThread());

    //m_webViewImpl->mainFrameImpl()->close();
    m_webViewImpl->close();

    for (size_t i = 0; i < m_destroyNotifs.size(); ++i)
        m_destroyNotifs[i]->destroy();

    m_state = pageDestroyed;
}

void WebPageImpl::closeWidgetSoon()
{
    ASSERT(isMainThread());
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    if (m_browser && !m_postCloseWidgetSoonMessage)
        blink::Platform::current()->currentThread()->postTask(FROM_HERE, WTF::bind(&CefBrowserHostImpl::CloseBrowser, m_browser, true));
#endif
    m_postCloseWidgetSoonMessage = true;
}

void WebPageImpl::showDebugNodeData()
{
#ifndef NDEBUG
    m_webViewImpl->mainFrameImpl()->frame()->document()->showTreeForThis();
#endif
}

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, commitTaskCounter, ("contentCommitTaskCounter"));
#endif

class CommitTask : public blink::WebThread::Task, public WebPageImpl::DestroyNotif {
public:
    CommitTask(WebPageImpl* client)
        : m_client(client)
    {
        m_client->registerDestroyNotif(this);
#ifndef NDEBUG
        commitTaskCounter.increment();
#endif
    }

    virtual ~CommitTask() override
    {
        if (m_client) {
            m_client->unregisterDestroyNotif(this);
        }
#ifndef NDEBUG
        commitTaskCounter.decrement();
#endif
    }

    virtual void destroy() override
    {
        m_client = nullptr;
    }

    virtual void run() override
    {
        if (!m_client)
            return;
        
        atomicDecrement(&m_client->m_commitCount);
        if (!doRun())
            m_client->clearNeedsCommit();
    }

private:
    bool doRun()
    {
        m_client->beginMainFrame();
        return true;
    }

    WebPageImpl* m_client;
};

void WebPageImpl::setNeedsCommitAndNotLayout()
{
    if (0 != m_needsCommit)
        return;
    atomicIncrement(&m_needsCommit);

#if 0 // (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    if (m_browser) {
        m_browser->SetNeedHeartbeat();
    } else {
#endif
        if (0 == m_commitCount) {
            atomicIncrement(&m_commitCount);
            blink::Platform* platfrom = blink::Platform::current();
            WebThreadImpl* threadImpl = (WebThreadImpl*)platfrom->mainThread();
            threadImpl->postTask(FROM_HERE, new CommitTask(this));
        }
#if 0 // (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    }
#endif
}

void WebPageImpl::setNeedsCommit()
{
    InterlockedExchange(reinterpret_cast<long volatile*>(&m_needsLayout), 1);
    setNeedsCommitAndNotLayout();
}

void WebPageImpl::clearNeedsCommit()
{
    atomicDecrement(&m_needsCommit);
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    if (m_browser)
        m_browser->ClearNeedHeartbeat();
#endif
}

void WebPageImpl::onLayerTreeDirty()
{
    InterlockedExchange(reinterpret_cast<long volatile*>(&m_layerDirty), 1);
    setNeedsCommitAndNotLayout();
}

void WebPageImpl::didUpdateLayout()
{
    //onLayerTreeDirty();
    setNeedsCommit();
}

void WebPageImpl::beginMainFrame()
{
    bool needsCommit = m_needsCommit;
    if (pageInited != m_state)
        return;

    if (needsCommit) {
        executeMainFrame();
        m_layerTreeHost->requestDrawFrameToRunIntoCompositeThread();
    }
}

void WebPageImpl::executeMainFrame()
{
    freeV8TempObejctOnOneFrameBefore();
    clearNeedsCommit();

    if (0 != m_executeMainFrameCount)
        return;
    atomicIncrement(&m_executeMainFrameCount);

    double lastFrameTimeMonotonic = WTF::monotonicallyIncreasingTime();

    if (!m_layerTreeHost->canRecordActions()) {
        setNeedsCommitAndNotLayout();
        atomicDecrement(&m_executeMainFrameCount);
        return;
    }

    //OutputDebugStringW(L"WebPageImpl::executeMainFrame ------------------------\n");
    {
        AutoRecordActions autoRecordActions(this, m_layerTreeHost, true);
    }
    //OutputDebugStringW(L"WebPageImpl::executeMainFrame end =====================\n\n");
#ifndef NDEBUG
    if (0) {
        showDebugNodeData();
        m_layerTreeHost->showDebug();
        blink::memoryCache()->evictResources();
        // V8GCController::collectGarbage(v8::Isolate::GetCurrent());
        Heap::collectGarbage(ThreadState::HeapPointersOnStack, ThreadState::GCWithSweep, Heap::ForcedGC);
        v8::Isolate::GetCurrent()->LowMemoryNotification();
    }
#endif
    atomicDecrement(&m_executeMainFrameCount);
}

bool WebPageImpl::fireTimerEvent()
{
    CHECK_FOR_REENTER(this, false);
        
    beginMainFrame();
    return false;
}

void WebPageImpl::fireResizeEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    freeV8TempObejctOnOneFrameBefore();
    if (pageInited != m_state)
        return;

    UINT cx, cy;
    cx = LOWORD(lParam);
    cy = HIWORD(lParam);

    if (!m_hWnd)
        m_hWnd = hWnd;

    IntSize size(cx, cy);
    setViewportSize(size);
}

IntRect WebPageImpl::caretRect() const
{
    blink::LocalFrame* targetFrame = blink::toLocalFrame(m_webViewImpl->focusedCoreFrame());
    if (!targetFrame)
        return IntRect();

    blink::IntRect caret;
    if (RefPtrWillBeRawPtr<Range> range = targetFrame->selection().selection().toNormalizedRange()) {
        caret = targetFrame->editor().firstRectForRange(range.get());
    }

    caret = targetFrame->view()->contentsToViewport(caret);
    return caret;
}

void WebPageImpl::setViewportSize(const IntSize& size)
{
    if (m_viewportSize == size)
        return;
    m_viewportSize = size;

    ASSERT(pageInited == m_state);
    if (pageDestroying == m_state)
        return;

    if (size.isEmpty())
        return;

    if (checkForRepeatEnter()) {
        CheckReEnter checker(this);
        AutoRecordActions autoRecordActions(this, m_layerTreeHost, false);
    }

    if (m_layerTreeHost)
        m_layerTreeHost->setViewportSize(size);

    m_webViewImpl->resize(size);
}

void WebPageImpl::firePaintEvent(HDC hdc, const RECT* paintRect)
{
    CHECK_FOR_REENTER(this, (void)0);
    freeV8TempObejctOnOneFrameBefore();

    beginMainFrame();

    if (m_layerTreeHost)
        m_layerTreeHost->firePaintEvent(hdc, paintRect);
}

HDC WebPageImpl::viewDC()
{
    bool useLayeredBuffer = m_layerTreeHost->getHasTransparentBackground();
    if (!m_memoryCanvasForUi && !m_viewportSize.isEmpty()) {
        m_memoryCanvasForUi = skia::CreatePlatformCanvas(m_viewportSize.width(), m_viewportSize.height(), !useLayeredBuffer);
        cc::LayerTreeHost::clearCanvas(m_memoryCanvasForUi, IntRect(0, 0, m_viewportSize.width(), m_viewportSize.height()), useLayeredBuffer);
    }
    if (!m_memoryCanvasForUi)
        return nullptr;

    skia::BitmapPlatformDevice* device = (skia::BitmapPlatformDevice*)skia::GetPlatformDevice(skia::GetTopDevice(*m_memoryCanvasForUi));
    if (!device)
        return nullptr;
    HDC hDC = device->GetBitmapDCUgly();
    return hDC;
}

void WebPageImpl::copyToMemoryCanvasForUi()
{
    bool useLayeredBuffer = m_layerTreeHost->getHasTransparentBackground();
    if (m_hWnd) {
        LONG windowStyle = GetWindowLongPtr(m_hWnd, GWL_EXSTYLE);
        useLayeredBuffer = !!(windowStyle & WS_EX_LAYERED);
        m_layerTreeHost->setHasTransparentBackground(useLayeredBuffer);
    }

    SkCanvas* memoryCanvas = m_layerTreeHost->getMemoryCanvasLocked();
    if (!memoryCanvas) {
        m_layerTreeHost->releaseMemoryCanvasLocked();
        return;
    }
    
    bool isCreatedThisTime = false;
    int width = memoryCanvas->imageInfo().width();
    int height = memoryCanvas->imageInfo().height();
    if (0 != width && 0 != height) {
        if ((!m_memoryCanvasForUi && 0 != width * height) || 
            (m_memoryCanvasForUi && (width != m_memoryCanvasForUi->imageInfo().width() || height != m_memoryCanvasForUi->imageInfo().height()))) {
            if (m_memoryCanvasForUi)
                delete m_memoryCanvasForUi;
            m_memoryCanvasForUi = skia::CreatePlatformCanvas(width, height, !useLayeredBuffer);
            cc::LayerTreeHost::clearCanvas(m_memoryCanvasForUi, IntRect(0, 0, width, height), useLayeredBuffer);
            isCreatedThisTime = true;
        }
    } else if (m_memoryCanvasForUi) {
        delete m_memoryCanvasForUi;
        m_memoryCanvasForUi = nullptr;
    }

    if (!m_memoryCanvasForUi) {
        m_layerTreeHost->releaseMemoryCanvasLocked();
        return;
    }

    if (useLayeredBuffer && !isCreatedThisTime)
        cc::LayerTreeHost::clearCanvas(m_memoryCanvasForUi, IntRect(0, 0, width, height), useLayeredBuffer);

    SkBaseDevice* device = skia::GetTopDevice(*memoryCanvas);
    if (device) {
        const SkBitmap& memoryCanvasBitmap = device->accessBitmap(false);
        m_memoryCanvasForUi->drawBitmap(memoryCanvasBitmap, 0, 0, nullptr);
    }
    m_layerTreeHost->releaseMemoryCanvasLocked();
}

bool WebPageImpl::isDrawDirty()
{
    if (m_layerTreeHost)
        return m_layerTreeHost->isDrawDirty();
    return false;
}

void drawDebugLine(SkCanvas* memoryCanvas, const IntRect& paintRect)
{
    static int g_debugCount = 0;
    ++g_debugCount;

#if 0
    HBRUSH hbrush;
    HPEN hpen;
    hbrush = ::CreateSolidBrush(rand()); // 创建蓝色画刷
    ::SelectObject(hdc, hbrush);
    //::Rectangle(hdc, m_paintRect.x(), m_paintRect.y(), m_paintRect.maxX(), m_paintRect.maxY());
    ::Rectangle(hdc, 220, 40, 366, 266);
    ::DeleteObject(hbrush);
#endif

#if 1 // debug
    if (g_drawDirtyDebugLine) {
        OwnPtr<GraphicsContext> context = GraphicsContext::deprecatedCreateWithCanvas(memoryCanvas, GraphicsContext::NothingDisabled);
        context->setStrokeStyle(SolidStroke);
        context->setStrokeColor(0xff000000 | (::GetTickCount() + base::RandInt(0, 0x1223345)));
        context->drawLine(IntPoint(paintRect.x(), paintRect.y()), IntPoint(paintRect.maxX(), paintRect.maxY()));
        context->drawLine(IntPoint(paintRect.maxX(), paintRect.y()), IntPoint(paintRect.x(), paintRect.maxY()));
        context->strokeRect(paintRect, 2);
    }
#endif

#if 0
    String outString = String::format("drawDebugLine:%d %d %d %d, %d\n", paintRect.x(), paintRect.y(), paintRect.width(), paintRect.height(), g_debugCount);
    OutputDebugStringW(outString.charactersWithNullTermination().data());
#endif
}

void WebPageImpl::disablePaint()
{
    m_disablePaint = true;
}

void WebPageImpl::enablePaint()
{
    m_disablePaint = false;
}

static bool canPaintToScreen(blink::WebViewImpl* webViewImpl)
{
    blink::Page* page = webViewImpl->page();
    if (!page)
        return true;

    blink::LocalFrame* frame = page->deprecatedLocalMainFrame();
    Document* document = frame->document();
    if (!document)
        return true;

    if (!document->timing().firstLayout())
        return false;

    if (document->hasActiveParser())
        return false;

    return true;
}

// 本函数可能被调用在ui线程，也可以是合成线程
void WebPageImpl::paintToMemoryCanvasInUiThread(SkCanvas* canvas, const IntRect& paintRect)
{
    if (m_disablePaint)
        return;

    if (0 == m_firstDrawCount && !canPaintToScreen(m_webViewImpl)) { }
    ++m_firstDrawCount;

//     String outString = String::format("WebPageImpl::paintToMemoryCanvasInUiThread:%d %d %d %d\n",
//         paintRect.x(), paintRect.y(), paintRect.width(), paintRect.height());
//     OutputDebugStringW(outString.charactersWithNullTermination().data());

    HDC hMemoryDC = nullptr;
    hMemoryDC = skia::BeginPlatformPaint(canvas);

    drawDebugLine(canvas, paintRect);

    bool drawToScreen = false;
#if ENABLE_CEF == 1
    drawToScreen = !!m_browser;
#endif
    if (drawToScreen) { // 使用wke接口不由此上屏
        HDC hdc = GetDC(m_pagePtr->getHWND());
        if (m_layerTreeHost->getHasTransparentBackground()) {
            RECT rtWnd;
            ::GetWindowRect(m_pagePtr->getHWND(), &rtWnd);
            IntRect winodwRect = winRectToIntRect(rtWnd);
            skia::DrawToNativeLayeredContext(canvas, hdc, &intRectToWinRect(paintRect), &intRectToWinRect(paintRect));
        } else
            skia::DrawToNativeContext(canvas, hdc, paintRect.x(), paintRect.y(), &intRectToWinRect(paintRect));
        ::ReleaseDC(m_pagePtr->getHWND(), hdc);
    } else {
        copyToMemoryCanvasForUi();
    }
    
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    if (m_pagePtr->wkeHandler().paintUpdatedCallback) {
        m_pagePtr->wkeHandler().paintUpdatedCallback(
            m_pagePtr->wkeWebView(),
            m_pagePtr->wkeHandler().paintUpdatedCallbackParam,
            hMemoryDC, paintRect.x(), paintRect.y(), paintRect.width(), paintRect.height());
    }
#endif
    skia::EndPlatformPaint(canvas);
}

void WebPageImpl::paintToBit(void* bits, int pitch)
{
    CHECK_FOR_REENTER(this, (void)0);

    beginMainFrame();

    if (m_layerTreeHost)
        m_layerTreeHost->paintToBit(bits, pitch);
}

void WebPageImpl::repaintRequested(const IntRect& windowRect)
{
    freeV8TempObejctOnOneFrameBefore();
    if (pageInited != m_state || windowRect.isEmpty() || windowRect.maxY() < 0 || windowRect.maxX() < 0)
        return;

    if (m_layerTreeHost)
        m_layerTreeHost->postPaintMessage(windowRect);
    setNeedsCommitAndNotLayout();
}

// Called when a region of the WebWidget needs to be re-painted.
void WebPageImpl::didInvalidateRect(const WebRect& r)
{
    IntRect windowRect(r);
    if (-1 == windowRect.width() || -1 == windowRect.height())
        windowRect = m_layerTreeHost->getClientRect();
    repaintRequested(windowRect);
}

// Called when the Widget has changed size as a result of an auto-resize.
void WebPageImpl::didAutoResize(const WebSize& newSize)
{
    //notImplemented();
}

void WebPageImpl::didUpdateLayoutSize(const WebSize& newSize)
{
    //notImplemented();
}

void WebPageImpl::scheduleAnimation()
{
    setNeedsCommit/*AndNotLayout*/();
}

void WebPageImpl::onLayerTreeSetNeedsCommit()
{
    setNeedsCommit();
}

void WebPageImpl::onLayerTreeInvalidateRect(const blink::IntRect& r)
{
    didInvalidateRect(r);
}

void WebPageImpl::initializeLayerTreeView()
{
    ;
}

WebLayerTreeView* WebPageImpl::layerTreeView()
{
    return layerTreeHost();
}

void WebPageImpl::didChangeCursor(const WebCursorInfo& cursor)
{
    if (m_cursorType == cursor.type)
        return;
    m_cursorType = cursor.type;

    if (m_hWnd)
        ::PostMessage(m_hWnd, WM_SETCURSOR, 0, 0);
}

int WebPageImpl::getCursorInfoType() const
{
    return (int)m_cursorType;
}

void WebPageImpl::fireCursorEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL* handle)
{
    CHECK_FOR_REENTER(this, (void)0);
    freeV8TempObejctOnOneFrameBefore();

    if (handle)
        *handle = FALSE;
    HCURSOR hCur = NULL;
    switch (m_cursorType) {
    case WebCursorInfo::TypeIBeam:
        hCur = ::LoadCursor(NULL, IDC_IBEAM);
        break;
    case WebCursorInfo::TypeHand:
        hCur = ::LoadCursor(NULL, IDC_HAND);
        break;
    case WebCursorInfo::TypeWait:
        hCur = ::LoadCursor(NULL, IDC_WAIT);
        break;
    case WebCursorInfo::TypeHelp:
        hCur = ::LoadCursor(NULL, IDC_HELP);
        break;
    case WebCursorInfo::TypeEastResize:
        hCur = ::LoadCursor(NULL, IDC_SIZEWE);
        break;
    case WebCursorInfo::TypeNorthResize:
        hCur = ::LoadCursor(NULL, IDC_SIZENS);
        break;
    case WebCursorInfo::TypeSouthWestResize:
    case WebCursorInfo::TypeNorthEastResize:
        hCur = ::LoadCursor(NULL, IDC_SIZENESW);
        break;
    case WebCursorInfo::TypeSouthResize:
    case WebCursorInfo::TypeNorthSouthResize:
        hCur = ::LoadCursor(NULL, IDC_SIZENS);
        break;
    case WebCursorInfo::TypeNorthWestResize:
    case WebCursorInfo::TypeSouthEastResize:
        hCur = ::LoadCursor(NULL, IDC_SIZENWSE);
        break;
    case WebCursorInfo::TypeWestResize:
    case WebCursorInfo::TypeEastWestResize:
        hCur = ::LoadCursor(NULL, IDC_SIZEWE);
        break;
    case WebCursorInfo::TypeNorthEastSouthWestResize:
    case WebCursorInfo::TypeNorthWestSouthEastResize:
        hCur = ::LoadCursor(NULL, IDC_SIZEALL);
        break;
    }

    if (hCur) {
        ::SetCursor(hCur);
        if (handle)
            *handle = TRUE;
    }
}

LRESULT WebPageImpl::fireWheelEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CHECK_FOR_REENTER(this, 0);
    freeV8TempObejctOnOneFrameBefore();
    AutoRecordActions autoRecordActions(this, m_layerTreeHost, false);
    
    if (m_popup && m_popup->isVisible())
        m_popup->fireWheelEvent(message, wParam, lParam);
    else
        m_platformEventHandler->fireWheelEvent(hWnd, message, wParam, lParam);

    return 0;
}

bool WebPageImpl::fireKeyUpEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CHECK_FOR_REENTER(this, false);
    freeV8TempObejctOnOneFrameBefore();
    AutoRecordActions autoRecordActions(this, m_layerTreeHost, false);
    
    WebKeyboardEvent keyEvent = PlatformEventHandler::buildKeyboardEvent(WebInputEvent::KeyUp, message, wParam, lParam);
    return m_webViewImpl->handleInputEvent(keyEvent);
}

bool WebPageImpl::fireKeyDownEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CHECK_FOR_REENTER(this, false);
    freeV8TempObejctOnOneFrameBefore();
    AutoRecordActions autoRecordActions(this, m_layerTreeHost, false);

    if (m_popup && m_popup->isVisible())
        return m_popup->fireKeyUpEvent(message, wParam, lParam);

    unsigned int virtualKeyCode = wParam;
    WebKeyboardEvent keyEvent = PlatformEventHandler::buildKeyboardEvent(WebInputEvent::RawKeyDown, message, wParam, lParam);
    bool handled = m_webViewImpl->handleInputEvent(keyEvent);
    bool systemKey = false;
    // These events cannot be canceled, and we have no default handling for them.
    // FIXME: match IE list more closely, see <http://msdn2.microsoft.com/en-us/library/ms536938.aspx>.
    if (systemKey && virtualKeyCode != VK_RETURN)
        return false;

    if (handled) {
        MSG msg;
        ::PeekMessage(&msg, NULL, WM_CHAR, WM_CHAR, PM_REMOVE);
        return true;
    }

    // We need to handle back/forward using either Ctrl+Left/Right Arrow keys.
    // FIXME: This logic should probably be in EventHandler::defaultArrowEventHandler().
    // FIXME: Should check that other modifiers aren't pressed.
//     if (virtualKeyCode == VK_RIGHT && keyEvent.modifiers & WebInputEvent::ControlKey)
//         return page()->goForward();
//     if (virtualKeyCode == VK_LEFT && keyEvent.modifiers & WebInputEvent::ControlKey)
//         return page()->goBack();

    // Need to scroll the page if the arrow keys, pgup/dn, or home/end are hit.
//     WebCore::ScrollDirection direction;
//     WebCore::ScrollGranularity granularity;
//     switch (virtualKeyCode) {
//     case VK_LEFT:
//         granularity = WebCore::ScrollByLine;
//         direction = WebCore::ScrollLeft;
//         break;
// 
//     case VK_RIGHT:
//         granularity = WebCore::ScrollByLine;
//         direction = WebCore::ScrollRight;
//         break;
// 
//     case VK_UP:
//         granularity = WebCore::ScrollByLine;
//         direction = WebCore::ScrollUp;
//         break;
// 
//     case VK_DOWN:
//         granularity = WebCore::ScrollByLine;
//         direction = WebCore::ScrollDown;
//         break;
// 
//     case VK_HOME:
//         granularity = WebCore::ScrollByDocument;
//         direction = WebCore::ScrollUp;
//         break;
// 
//     case VK_END:
//         granularity = WebCore::ScrollByDocument;
//         direction = WebCore::ScrollDown;
//         break;
// 
//     case VK_PRIOR:
//         granularity = WebCore::ScrollByPage;
//         direction = WebCore::ScrollUp;
//         break;
// 
//     case VK_NEXT:
//         granularity = WebCore::ScrollByPage;
//         direction = WebCore::ScrollDown;
//         break;
// 
//     default:
//         return false;
//     }
// 
//     if (frame->eventHandler()->scrollRecursively(direction, granularity)) {
//         MSG msg;
//         ::PeekMessage(&msg, NULL, WM_CHAR, WM_CHAR, PM_REMOVE);
//         return true;
//     }

    return false;
}

bool WebPageImpl::handleCurrentKeyboardEvent()
{
    return false;
}

bool WebPageImpl::fireKeyPressEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CHECK_FOR_REENTER(this, false);
    freeV8TempObejctOnOneFrameBefore();
    AutoRecordActions autoRecordActions(this, m_layerTreeHost, false);

    unsigned int charCode = wParam;
    unsigned int flags = 0;
    if (HIWORD(lParam) & KF_REPEAT)
        flags |= KF_REPEAT;
    if (HIWORD(lParam) & KF_EXTENDED)
        flags |= KF_EXTENDED;
    LPARAM keyData = MAKELPARAM(0, (WORD)flags);
    bool systemKey = false;
    WebKeyboardEvent keyEvent = PlatformEventHandler::buildKeyboardEvent(WebInputEvent::Char, message, wParam, lParam);
    return m_webViewImpl->handleInputEvent(keyEvent);
}

void WebPageImpl::fireCaptureChangedEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CHECK_FOR_REENTER(this, (void)0);
    freeV8TempObejctOnOneFrameBefore();
    AutoRecordActions autoRecordActions(this, m_layerTreeHost, false);

    m_platformEventHandler->fireCaptureChangedEvent(hWnd, message, wParam, lParam);
}

void WebPageImpl::fireSetFocusEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CHECK_FOR_REENTER(this, (void)0);
    freeV8TempObejctOnOneFrameBefore();
    m_webViewImpl->setFocus(true);
    m_webViewImpl->setIsActive(true);
}

void WebPageImpl::fireKillFocusEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CHECK_FOR_REENTER(this, (void)0);
    freeV8TempObejctOnOneFrameBefore();

    HWND currentFocus = ::GetFocus();
    if (currentFocus == m_popupHandle)
        return;
    m_webViewImpl->setFocus(false);
    m_popupHandle = nullptr;
}

void WebPageImpl::fireTouchEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    freeV8TempObejctOnOneFrameBefore();

    m_platformEventHandler->fireTouchEvent(hWnd, message, wParam, lParam);
}

LRESULT WebPageImpl::fireMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL* bHandle)
{
    CHECK_FOR_REENTER(this, 0);
    freeV8TempObejctOnOneFrameBefore();
    AutoRecordActions autoRecordActions(this, m_layerTreeHost, false);

    bool handle = false;
//     fireTouchEvent(hWnd, message, wParam, lParam);
//     return; // TODO

    m_platformEventHandler->fireMouseEvent(hWnd, message, wParam, lParam, true, bHandle);
    return 0;
}

void WebPageImpl::loadHistoryItem(int64 frameId, const WebHistoryItem& item, WebHistoryLoadType type, WebURLRequest::CachePolicy policy)
{
    WebFrame* webFrame = getWebFrameFromFrameId(frameId);
    if (!webFrame)
        return;

    //AutoRecordActions autoRecordActions(this, m_layerTreeHost, false);
    webFrame->loadHistoryItem(item, type, policy);
    m_webViewImpl->setFocus(true);
}

void WebPageImpl::loadURL(int64 frameId, const wchar_t* url, const blink::Referrer& referrer, const wchar_t* extraHeaders)
{
    int length = wcslen(url);
    String urlW((const UChar*)url, length);
    blink::KURL kurl(ParsedURLString, urlW.utf8().data());

    blink::WebURL webURL = kurl;
    blink::WebURLRequest request(webURL);
    request.setHTTPReferrer(referrer.referrer, blink::WebReferrerPolicyOrigin);
    loadRequest(frameId, request);
}

void WebPageImpl::loadRequest(int64 frameId, const blink::WebURLRequest& request)
{
    if (!m_webViewImpl || !m_webViewImpl->mainFrame())
        return;

    blink::WebURLRequest requestWrap(request);
    WebFrame* webFrame = getWebFrameFromFrameId(frameId);
    if (!webFrame)
        return;

    //AutoRecordActions autoRecordActions(this, m_layerTreeHost, false);
    
    requestWrap.setHTTPHeaderField(WebString::fromLatin1("Accept"), WebString::fromLatin1("text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"));
    webFrame->loadRequest(requestWrap);
    m_webViewImpl->setFocus(true);
}

void WebPageImpl::loadHTMLString(int64 frameId, const WebData& html, const WebURL& baseURL, const WebURL& unreachableURL, bool replace)
{
    if (!m_webViewImpl || !m_webViewImpl->mainFrame())
        return;

    WebFrame* webFrame = getWebFrameFromFrameId(frameId);
    if (!webFrame)
        return;

    //AutoRecordActions autoRecordActions(this, m_layerTreeHost, false);
    webFrame->loadHTMLString(html, baseURL, unreachableURL, replace);
    m_webViewImpl->setFocus(true);
}

void WebPageImpl::setTransparent(bool transparent)
{
    m_layerTreeHost->setHasTransparentBackground(transparent);
    m_layerTreeHost->setBackgroundColor(cc::getRealColor(transparent, cc::s_kBgColor));
    m_webViewImpl->setIsTransparent(transparent);
    m_webViewImpl->setBaseBackgroundColor(cc::getRealColor(transparent, cc::s_kBgColor));
}

WebPageImpl* WebPageImpl::getSelfForCurrentContext()
{
    blink::WebLocalFrame* frame = blink::WebLocalFrame::frameForCurrentContext();
    if (!frame)
        return nullptr;
    blink::WebViewImpl* impl = (blink::WebViewImpl*)frame->view();
    if (!impl)
        return nullptr;

    content::WebPageImpl* page = (content::WebPageImpl*)impl->client();
    return page;
}

#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
CefBrowserHostImpl* WebPageImpl::browser() const 
{
    return m_browser;
}

void WebPageImpl::setBrowser(CefBrowserHostImpl* browser)
{
    m_browser = browser;
}
#endif

void WebPageImpl::didCommitProvisionalLoad(blink::WebLocalFrame* frame, const blink::WebHistoryItem& history, blink::WebHistoryCommitType type)
{
    m_navigationController->insertOrReplaceEntry(history, type);
}

void WebPageImpl::navigateBackForwardSoon(int offset)
{
    m_navigationController->navigateBackForwardSoon(offset);
}

int WebPageImpl::historyBackListCount()
{
    return m_navigationController->historyBackListCount();
}

int WebPageImpl::historyForwardListCount()
{
    return m_navigationController->historyForwardListCount();
}

WebFrame* WebPageImpl::getWebFrameFromFrameId(int64 frameId)
{
    blink::WebFrame* webFrame = nullptr;
    if (WebPage::kMainFrameId != frameId) {
        Frame* frame = toCoreFrame(m_webViewImpl->mainFrame());

        while (frame && frame->frameID() != frameId)
            frame = frame->tree().traverseNext();
        if (!frame)
            return nullptr;

        webFrame = WebFrame::fromFrame(frame);
    } else
        webFrame = m_webViewImpl->mainFrame();

    return webFrame;
}

WebStorageNamespace* WebPageImpl::createSessionStorageNamespace()
{
    return ((content::BlinkPlatformImpl*)Platform::current())->createSessionStorageNamespace();
}

WebString WebPageImpl::acceptLanguages()
{
    return WebString::fromUTF8("zh-CN,zh");
}

WebScreenInfo WebPageImpl::screenInfo()
{
    POINT pt = { 0, 0 };
    HMONITOR hMonitor = ::MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);

    MONITORINFO mi = { 0 };
    mi.cbSize = sizeof(MONITORINFO);
    ::GetMonitorInfo(hMonitor, &mi);

    WebScreenInfo info;
    info.rect = WebRect(winRectToIntRect(mi.rcMonitor));
    info.availableRect = WebRect(winRectToIntRect(mi.rcWork));

    return info;
}

void WebPageImpl::setToolTipText(const blink::WebString& toolTip, blink::WebTextDirection hint)
{
    m_toolTip->show(WTF::ensureUTF16UChar((String)toolTip, true).data());
}

WebWidget* WebPageImpl::createPopupMenu(WebPopupType type)
{
    if (!m_hWnd)
        m_hWnd = ::GetActiveWindow();
    
    PopupMenuWin* popup = nullptr;
    blink::WebWidget* result = PopupMenuWin::create(this, m_hWnd, m_hwndRenderOffset, m_webViewImpl, type, &popup);
    m_popup = popup;
    m_popupHandle = popup->popupHandle();
    return result;
}

void WebPageImpl::onPopupMenuCreate(HWND hWnd)
{
    m_popupHandle = hWnd;
}

void WebPageImpl::onPopupMenuHide()
{
    //m_popup = nullptr;
}

void WebPageImpl::didStartProvisionalLoad()
{
    m_firstDrawCount = 0;
}

bool WebPageImpl::initSetting()
{
    WebSettingsImpl* settings = m_webViewImpl->settingsImpl();
    if (!settings)
        return false;
    settings->setTextAreasAreResizable(true);

    settings->setStandardFontFamily(WebString(L"微软雅黑", 4));
    settings->setUsesEncodingDetector(true);
    settings->setJavaScriptEnabled(true);
    settings->setAllowFileAccessFromFileURLs(true);
    settings->setAcceleratedCompositingEnabled(true);
    settings->setUseSolidColorScrollbars(false);
    settings->setPinchOverlayScrollbarThickness(8);
    //settings->setSpatialNavigationEnabled(true);
    settings->setLocalStorageEnabled(true);
    settings->setMinimumFontSize(10);
    settings->setMinimumLogicalFontSize(10);
    settings->setDefaultFontSize(16);
    settings->setDefaultFixedFontSize(16);
    settings->setLoadsImagesAutomatically(true);
    settings->setPluginsEnabled(true);

    PluginDatabase::installedPlugins()->refresh();

    return true;
}

} // blink