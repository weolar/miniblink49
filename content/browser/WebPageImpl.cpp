
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
//#include "third_party/WebKit/Source/core/page/DragState.h"
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
// #include "third_party/WebKit/Source/web/ContextMenuClientImpl.h"
// #include "third_party/WebKit/Source/web/EditorClientImpl.h"
// #include "third_party/WebKit/Source/web/DragClientImpl.h"
#include "third_party/WebKit/Source/web/WebViewImpl.h"
#include "third_party/WebKit/Source/web/FrameLoaderClientImpl.h"
#include "third_party/WebKit/Source/web/WebLocalFrameImpl.h"
#include "third_party/WebKit/Source/web/WebSettingsImpl.h"
#include "third_party/WebKit/Source/wtf/MainThread.h"
#include "third_party/WebKit/Source/wtf/Functional.h"
#include "third_party/WebKit/Source/wtf/RefCountedLeakCounter.h"
#include "third_party/WebKit/Source/wtf/text/WTFStringUtil.h"
#include "third_party/WebKit/Source/bindings/core/v8/V8GCController.h"

#include "gin/public/isolate_holder.h"
#include "gin/array_buffer.h"
#include "ui/gfx/win/dpi.h"
#include "gen/blink/platform/RuntimeEnabledFeatures.h"

#include "skia/ext/bitmap_platform_device_win.h"

#include "content/browser/WebPage.h"
#include "content/browser/PlatformEventHandler.h"
#include "content/browser/PopupMenuWin.h"
#include "content/browser/SharedTimerWin.h"
#include "content/browser/WebFrameClientImpl.h"
#include "content/web_impl_win/BlinkPlatformImpl.h"
#include "content/web_impl_win/WebThreadImpl.h"
#include "content/web_impl_win/npapi/PluginDatabase.h"

#include "cc/trees/LayerTreeHost.h"

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
#endif
using namespace blink;

#if USING_VC6RT == 1
void scrt_initialize_thread_safe_statics();
#endif
extern "C" void x86_check_features(void);

namespace blink {

bool saveDumpFile(const String& url, char* buffer, unsigned int size);

}

namespace content {

static void setRuntimeEnabledFeatures();

void WebPageImpl::initBlink()
{
#if USING_VC6RT == 1
    scrt_initialize_thread_safe_statics();
#endif
	x86_check_features();
    ::CoInitializeEx(NULL, 0); // COINIT_MULTITHREADED

    setRuntimeEnabledFeatures();

    gfx::win::InitDeviceScaleFactor();
    content::BlinkPlatformImpl* platform = new content::BlinkPlatformImpl();
    blink::Platform::initialize(platform);
    gin::IsolateHolder::Initialize(gin::IsolateHolder::kNonStrictMode, gin::ArrayBufferAllocator::SharedInstance());
    blink::initialize(blink::Platform::current());

	initializeOffScreenTimerWindow();

    platform->startGarbageCollectedThread();

    OutputDebugStringW(L"WebPageImpl::initBlink\n");
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
    m_memoryCanvas = nullptr;
    m_needsCommit = true;
    m_needsLayout = true;
    m_layerTreeHost = nullptr;
    m_lastFrameTimeMonotonic = 0;
    m_webViewImpl = nullptr;
    m_debugCount = 0;
    m_enterCount = 0;
    m_hWnd = NULL;
    m_state = pageUninited;
    m_platformEventHandler = nullptr;
    m_scheduleMessageCount = 0;
    m_postpaintMessageCount = 0;
    m_hasResize = false;
    m_postMouseLeave = false;
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    m_browser = nullptr;
#endif
    m_postCloseWidgetSoonMessage = false;
    
    m_layerTreeHost = new cc::LayerTreeHost(this);

    m_webFrameClient = new content::WebFrameClientImpl();
    
    WebLocalFrameImpl* webLocalFrameImpl = (WebLocalFrameImpl*)WebLocalFrame::create(WebTreeScopeType::Document, m_webFrameClient);
    m_webViewImpl = WebViewImpl::create(this);
    m_webViewImpl->setMainFrame(webLocalFrameImpl);
    initSetting();

    //m_frameLoaderClient = new FrameLoaderClientImpl(webLocalFrameImpl);
    //m_frame->view()->setTransparent(m_useLayeredBuffer);

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

        //     CefRefPtr<CefListValue> extraInfo = CefListValue::Create();
        //     handler->OnRenderThreadCreated(extraInfo);
        handler->OnWebKitInitialized();
    }
#endif
}

WebPageImpl::~WebPageImpl()
{
    ASSERT(pageDestroyed == m_state);
    m_state = pageDestroyed;

	delete m_layerTreeHost;
	m_layerTreeHost = nullptr;

    // 在Page::~Page()中销毁
    if (m_webFrameClient)
        delete m_webFrameClient;
    m_webFrameClient = nullptr;

    if (m_memoryCanvas)
        delete m_memoryCanvas;
    m_memoryCanvas = nullptr;

    delete m_platformEventHandler;
    m_platformEventHandler = nullptr;

    m_pagePtr = 0;
}

bool WebPageImpl::checkForRepeatEnter()
{
    if (m_enterCount == 0)
        return true;
    return false;
}

class CheckReEnter {
public:
    CheckReEnter(WebPageImpl* webPageImpl)
    {
        m_webPageImpl = webPageImpl;
        ++m_webPageImpl->m_enterCount;
    }

    ~CheckReEnter()
    {
        --m_webPageImpl->m_enterCount;

        if (WebPageImpl::pageDestroying == m_webPageImpl->m_state)
            m_webPageImpl->doClose();
    }

private:
    WebPageImpl* m_webPageImpl;
};

#define CHECK_FOR_REENTER(ret) \
    if (!checkForRepeatEnter()) \
        return ret; \
    if (pageInited != m_state) \
        return ret; \
    CheckReEnter checker(this);

#define CHECK_FOR_REENTER0() \
    if (!checkForRepeatEnter()) \
        return; \
    if (pageInited != m_state) \
        return; \
    CheckReEnter checker(this);

class AutoRecordActions {
public:
    AutoRecordActions(cc::LayerTreeHost* host)
    {
        m_host = host;
        if (m_host)
            m_host->beginRecordActions();
    }

    ~AutoRecordActions()
    {
        if (m_host)
            m_host->endRecordActions();
    }

private:
    cc::LayerTreeHost* m_host;
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
    Vector<UChar> nameBuf = WTF::ensureUTF16UChar(nameString);

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
    wke::CString wkeUrl(url.data());
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
        return createWkeViewDefault(m_hWnd, name, url);

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
    m_useLayeredBuffer = !!((windowStyle)& WS_EX_LAYERED);

    m_pagePtr = pagePtr;
    m_webFrameClient->setWebPage(m_pagePtr);

    m_webViewImpl->setFocus(true);

    m_state = pageInited;
}

void WebPageImpl::clearPaintWhenLayeredWindow(skia::PlatformCanvas* canvas, const IntRect& rect)
{
    if (!m_useLayeredBuffer)
        return;

    // When using transparency mode clear the rectangle before painting.
    SkPaint clearPaint;
    clearPaint.setARGB(0, 0xf0, 0, 0);
    clearPaint.setXfermodeMode(SkXfermode::kClear_Mode);

    SkRect skrc;
    skrc.set(rect.x(), rect.y(), rect.x() + rect.width(), rect.y() + rect.height());
    canvas->drawRect(skrc, clearPaint);
}

void WebPageImpl::mergeDirtyList()
{ 
    while (doMergeDirtyList(true)) {}; 
}

bool WebPageImpl::doMergeDirtyList(bool forceMerge)
{
    int nDirty = (int)m_paintMessageQueue.size();
    if (nDirty < 1) {
        return false;
    }

    int bestDelta = forceMerge ? 0x7FFFFFFF : 0;
    int mergeA = 0;
    int mergeB = 0;
    for (int i = 0; i < nDirty - 1; i++) {
        for (int j = i + 1; j < nDirty; j++) {
            int delta = intUnionArea(&m_paintMessageQueue[i], &m_paintMessageQueue[j]) -
                intRectArea(&m_paintMessageQueue[i]) - intRectArea(&m_paintMessageQueue[j]);
            if (bestDelta > delta) {
                mergeA = i;
                mergeB = j;
                bestDelta = delta;
            }
        }
    }

    if (mergeA != mergeB) {
        m_paintMessageQueue[mergeA].unite(m_paintMessageQueue[mergeB]);
        for (int i = mergeB + 1; i < nDirty; i++)
            m_paintMessageQueue[i - 1] = m_paintMessageQueue[i];

        m_paintMessageQueue.removeLast();
        return true;
    }

    return false;
}

void WebPageImpl::postPaintMessage(const IntRect* paintRect)
{
    if (!paintRect || paintRect->isEmpty() || !m_clientRect.intersects(*paintRect))
        return;

    IntRect dirtyRect = *paintRect;
    dirtyRect.intersect(m_clientRect);

// 	String outString = String::format("WebPageImpl::postPaintMessage: (%d %d)(%d %d)\n", dirtyRect.x(), dirtyRect.y(), dirtyRect.width(), dirtyRect.height());
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());

    m_dirtyRects.append(dirtyRect);
    setNeedsCommitAndNotLayout();
}

void WebPageImpl::testPaint()
{
    for (size_t index = 0; index < m_paintMessageQueue.size(); ++index) {
        IntRect* paintRect = &m_paintMessageQueue[index];
        WCHAR msg[100] = { 0 };
        swprintf(msg, L"testPaint: %d %d %x\n", paintRect->y(), paintRect->height(), index);
        OutputDebugStringW(msg);
    }
}

void WebPageImpl::freeV8TempObejctOnOneFrameBefore()
{
#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    wke::freeV8TempObejctOnOneFrameBefore();
#endif
}

bool WebPageImpl::drawFrame()
{
    if (0 != m_scheduleMessageCount)
        DebugBreak();

    bool needClearCommit = m_layerTreeHost->preDrawFrame(); // 这里也会发起Commit

    m_scheduleMessageCount++;

    for (size_t i = 0; i < m_dirtyRects.size(); ++i) {
        const blink::IntRect& r = m_dirtyRects[i];
        paintToPlatformContext(r);
    }

	m_layerTreeHost->postDrawFrame();

    m_dirtyRects.clear();
    m_paintMessageQueue.clear();
    m_scheduleMessageCount--;

    return needClearCommit;
}

void WebPageImpl::firePaintEvent(HDC hdc, const RECT* paintRect)
{
    CHECK_FOR_REENTER0();
    freeV8TempObejctOnOneFrameBefore();

    if (!m_memoryCanvas || m_clientRect.isEmpty())
        return;

    //////////////////////////////////////////////////////////////////////////
//     HPEN hpen = CreatePen(PS_SOLID, 10, RGB(11, 22, 33));
//     HBRUSH hbrush = CreateSolidBrush(RGB(0xf3, 22, 33));
// 
//     SelectObject(hdc, hpen);
//     SelectObject(hdc, hbrush);
// 
//     Rectangle(hdc, paintRect->left, paintRect->top, paintRect->right, paintRect->bottom);
// 
//     DeleteObject(hpen);
//     DeleteObject(hbrush);
    //////////////////////////////////////////////////////////////////////////

    beginMainFrame();

    skia::DrawToNativeContext(m_memoryCanvas, hdc, paintRect->left, paintRect->top, paintRect);
}

void WebPageImpl::paintToPlatformContext(const IntRect& paintRect)
{
    m_paintRect = paintRect;

    bool needsFullTreeSync = true; // false; 先全部层都更新，这样滚动条才能被刷新到
    if ((!m_memoryCanvas || m_hasResize) && !m_clientRect.isEmpty()) {
        m_hasResize = false;
        needsFullTreeSync = true;
        m_paintRect = m_clientRect;

        if (m_memoryCanvas)
            delete m_memoryCanvas;
        m_memoryCanvas = skia::CreatePlatformCanvas(m_clientRect.width(), m_clientRect.height(), !m_useLayeredBuffer);

        SkPaint clearColorPaint;
        clearColorPaint.setColor(0x0); // 0xfff0504a
        clearColorPaint.setXfermodeMode(SkXfermode::kSrcOver_Mode); // SkXfermode::kSrcOver_Mode
        m_memoryCanvas->drawRect((SkRect)m_clientRect, clearColorPaint);
    }

    m_paintRect.intersect(m_clientRect);
    if (m_paintRect.isEmpty())
        return;
    
    if (!m_memoryCanvas) {
        ASSERT(false);
        return;
    }

    clearPaintWhenLayeredWindow(m_memoryCanvas, m_paintRect);

    HDC hMemoryDC = skia::BeginPlatformPaint(m_memoryCanvas);

    drawToCanvas(m_paintRect, m_memoryCanvas, needsFullTreeSync); // 绘制脏矩形

    if (m_useLayeredBuffer) { // 再把内存dc画到hdc上
        RECT rtWnd;
        ::GetWindowRect(m_pagePtr->getHWND(), &rtWnd);
        m_winodwRect = winRectToIntRect(rtWnd);
        //skia::DrawToNativeLayeredContext(m_memoryCanvas.get(), hdc, m_winodwRect.x(), m_winodwRect.y(), &((RECT)m_clientRect));
    } else {
        //drawDebugLine(m_memoryCanvas, m_paintRect);
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
		if (m_browser) { // 使用wke接口不由此上屏
			HDC hdc = GetDC(m_pagePtr->getHWND());
			skia::DrawToNativeContext(m_memoryCanvas, hdc, m_paintRect.x(), m_paintRect.y(), &intRectToWinRect(m_paintRect));
			ReleaseDC(m_pagePtr->getHWND(), hdc);
		}
#endif
    }

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
    if (m_pagePtr->wkeHandler().paintUpdatedCallback) {
        m_pagePtr->wkeHandler().paintUpdatedCallback(
            m_pagePtr->wkeWebView(), 
            m_pagePtr->wkeHandler().paintUpdatedCallbackParam, 
            hMemoryDC, m_paintRect.x(), m_paintRect.y(), m_paintRect.width(), m_paintRect.height());
    }
#endif

    skia::EndPlatformPaint(m_memoryCanvas);
}

void WebPageImpl::drawDebugLine(skia::PlatformCanvas* memoryCanvas, const IntRect& paintRect)
{
   m_debugCount++;

    //     HBRUSH hbrush;
    //     HPEN hpen;
    //     hbrush = ::CreateSolidBrush(rand()); // 创建蓝色画刷
    //     ::SelectObject(hdc, hbrush);
    //     //::Rectangle(hdc, m_paintRect.x(), m_paintRect.y(), m_paintRect.maxX(), m_paintRect.maxY());
    //     ::Rectangle(hdc, 220, 40, 366, 266);
    //     ::DeleteObject(hbrush);

    OwnPtr<GraphicsContext> context = GraphicsContext::deprecatedCreateWithCanvas(memoryCanvas, GraphicsContext::NothingDisabled);
    context->setStrokeStyle(SolidStroke);
    context->setStrokeColor(0xff000000 | (::GetTickCount() + base::RandInt(0, 0x1223345)));
    context->drawLine(IntPoint(paintRect.x(), paintRect.y()), IntPoint(paintRect.maxX(), paintRect.maxY()));
    context->drawLine(IntPoint(paintRect.maxX(), paintRect.y()), IntPoint(paintRect.x(), paintRect.maxY()));
    context->strokeRect(paintRect, 2);

// 	String outString = String::format("drawDebugLine:%d %d %d %d, %d\n", m_paintRect.x(), m_paintRect.y(), m_paintRect.width(), m_paintRect.height(), m_debugCount);
// 	OutputDebugStringW(outString.charactersWithNullTermination().data());
}

void WebPageImpl::drawToCanvas(const IntRect& dirtyRect, skia::PlatformCanvas* canvas, bool needsFullTreeSync)
{
    if (dirtyRect.isEmpty() || !m_layerTreeHost)
        return;

    setPainting(true);

    canvas->save();
    canvas->clipRect(dirtyRect);

    SkPaint paint;
    paint.setAntiAlias(false);
    paint.setColor(0xffffffff); // 0xfff0504a
    paint.setXfermodeMode(SkXfermode::kSrcOver_Mode); // SkXfermode::kSrcOver_Mode
    canvas->drawRect((SkRect)dirtyRect, paint);

#if 0
    m_layerTreeHost->updateLayers(canvas, dirtyRect, needsFullTreeSync);
#else
    m_layerTreeHost->drawToCanvas(canvas, dirtyRect);
#endif

    canvas->restore();
    setPainting(false);
}

HDC WebPageImpl::viewDC()
{
    skia::BitmapPlatformDevice* device = (skia::BitmapPlatformDevice*)skia::GetPlatformDevice(skia::GetTopDevice(*m_memoryCanvas));
    if (device)
        return device->GetBitmapDCUgly();
    return nullptr;
}

void WebPageImpl::paintToBit(void* bits, int pitch)
{
    if (0 == pitch)
        return;

    CHECK_FOR_REENTER0();

    beginMainFrame();

    if (!m_memoryCanvas)
        return;

    int width = m_clientRect.width();
    int height = m_clientRect.height();

    DWORD cBytes = width * height * 4;
    SkBaseDevice* device = (SkBaseDevice*)m_memoryCanvas->getTopDevice();
    if (!device)
        return;
    const SkBitmap& bitmap = device->accessBitmap(false);
    if (bitmap.info().width() != width || bitmap.info().height() != height)
        return;
    uint32_t* pixels = bitmap.getAddr32(0, 0);

    if (pitch == 0 || pitch == width * 4) {
        memcpy(bits, pixels, width * height * 4);
    } else {
        unsigned char* src = (unsigned char*)pixels;
        unsigned char* dst = (unsigned char*)bits;
        for (int i = 0; i < height; ++i) {
            memcpy(dst, src, width * 4);
            src += width * 4;
            dst += pitch;
        }
    }
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
    for (size_t i = 0; i < m_destroyNotifs.size(); ++i)
        m_destroyNotifs[i]->destroy();

    m_layerTreeHost->applyActions(false);
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

    //blink::V8GCController::collectGarbage(v8::Isolate::GetCurrent());
    m_webViewImpl->mainFrameImpl()->close();
    m_webViewImpl->close();
    //v8::Isolate::GetCurrent()->CollectAllGarbage("WebPage close");

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

    virtual ~CommitTask() OVERRIDE
    {
        if (m_client)
            m_client->unregisterDestroyNotif(this);
#ifndef NDEBUG
		commitTaskCounter.decrement();
#endif
	}

    virtual void destroy()
    {
        m_client = nullptr;
    }

    virtual void run() OVERRIDE
    {
        if (m_client)
            m_client->beginMainFrame();
    }

private:
    WebPageImpl* m_client;
};

void WebPageImpl::setNeedsCommitAndNotLayout()
{
	if (m_needsCommit)
		return;
	m_needsCommit = true;
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
	if (m_browser) {
        m_browser->SetNeedHeartbeat();
	} else {
#endif
        blink::Platform* platfrom = blink::Platform::current();
        WebThreadImpl* threadImpl = (WebThreadImpl*)platfrom->mainThread();
        threadImpl->postTask(FROM_HERE, new CommitTask(this));
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    }
#endif
}

void WebPageImpl::setNeedsCommit()
{
    m_needsLayout = true;
    setNeedsCommitAndNotLayout();
}

void WebPageImpl::clearNeedsCommit()
{
    m_needsCommit = false;
#if (defined ENABLE_CEF) && (ENABLE_CEF == 1)
    if (m_browser)
        m_browser->ClearNeedHeartbeat();
#endif
}

void WebPageImpl::beginMainFrame()
{
	bool needsCommit = m_needsCommit;
	if (pageInited != m_state)
		return;
		
	if (needsCommit) {
		executeMainFrame();
		drawFrame();
	}
}

void WebPageImpl::executeMainFrame()
{
    freeV8TempObejctOnOneFrameBefore();

    clearNeedsCommit();

    double lastFrameTimeMonotonic = WTF::monotonicallyIncreasingTime();
    
    m_layerTreeHost->beginRecordActions();

    if (m_needsLayout) {
        WebBeginFrameArgs frameArgs(lastFrameTimeMonotonic, 0, lastFrameTimeMonotonic - m_lastFrameTimeMonotonic);
        m_webViewImpl->beginFrame(frameArgs);
        m_webViewImpl->layout();
        m_needsLayout = false;
    }

    m_layerTreeHost->recordDraw();
    m_layerTreeHost->endRecordActions();

    m_lastFrameTimeMonotonic = lastFrameTimeMonotonic;

#ifndef NDEBUG
    if (0) {
        showDebugNodeData();
        m_layerTreeHost->showDebug();
        blink::memoryCache()->evictResources();
        V8GCController::collectGarbage(v8::Isolate::GetCurrent());
        Heap::collectGarbage(ThreadState::HeapPointersOnStack, ThreadState::GCWithSweep, Heap::ForcedGC);
        v8::Isolate::GetCurrent()->LowMemoryNotification();
    }
#endif    
}

class TestTask : public blink::WebThread::Task, public WebPageImpl::DestroyNotif {
public:
    TestTask(WebPageImpl* client)
        : m_client(client)
    {
        m_client->registerDestroyNotif(this);
#ifndef NDEBUG
        commitTaskCounter.increment();
#endif
    }

    virtual ~TestTask() OVERRIDE
    {
        if (m_client)
            m_client->unregisterDestroyNotif(this);
#ifndef NDEBUG
        commitTaskCounter.decrement();
#endif
    }

    virtual void destroy()
    {
        m_client = nullptr;
    }

    virtual void run() OVERRIDE
    {
        if (m_client)
            m_client->loadURL(WebPage::kMainFrameId, L"https://map.baidu.com/", blink::Referrer(), nullptr);
    }

private:
    WebPageImpl* m_client;
};

bool WebPageImpl::fireTimerEvent()
{
    CHECK_FOR_REENTER(false);
        
    beginMainFrame();
    return false;
}

void WebPageImpl::fireResizeEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CHECK_FOR_REENTER0();
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
        blink::ExceptionCode ec = 0;
       // RefPtr<blink::Range> tempRange = range->cloneRange(ec);
        caret = targetFrame->editor().firstRectForRange(range.get());
    }

    caret = targetFrame->view()->contentsToViewport(caret);
    return caret;
}

void WebPageImpl::setViewportSize(const IntSize& size)
{
    m_viewportSize = size;

    ASSERT(pageInited == m_state);
    if (pageDestroying == m_state)
        return;

    if (size.isEmpty())
        return;

    AutoRecordActions autoRecordActions(m_layerTreeHost);

//     String outString = String::format("WebPageImpl::setViewportSize: %d %d\n", size.width(), size.height());
//     OutputDebugStringW(outString.charactersWithNullTermination().data());

    if (m_layerTreeHost)
        m_layerTreeHost->setViewportSize(size);

    m_webViewImpl->resize(size);

    m_clientRect = IntRect(0, 0, size.width(), size.height());
    m_hasResize = true;
}

void WebPageImpl::repaintRequested(const IntRect& windowRect)
{
    freeV8TempObejctOnOneFrameBefore();
    if (pageInited != m_state || windowRect.isEmpty() || windowRect.maxY() < 0 || windowRect.maxX() < 0)
        return;

    postPaintMessage(&windowRect);
}

// Called when a region of the WebWidget needs to be re-painted.
void WebPageImpl::didInvalidateRect(const WebRect& r)
{
    IntRect windowRect(r);
    if (-1 == windowRect.width() || -1 == windowRect.height())
        windowRect = m_clientRect;
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
    setNeedsCommit();
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

void WebPageImpl::fireCursorEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL* handle)
{
    CHECK_FOR_REENTER0();
    freeV8TempObejctOnOneFrameBefore();

    if (handle)
        *handle = FALSE;
    HCURSOR hCur = NULL;
    switch (m_cursorType) {
    case WebCursorInfo::TypeIBeam:
        //OutputDebugStringW(L"WebPageImpl::fireCursorEvent  IDC_IBEAM\n");
        hCur = ::LoadCursor(NULL, IDC_IBEAM);
        break;
    case WebCursorInfo::TypeHand:
        //OutputDebugStringW(L"WebPageImpl::fireCursorEvent  IDC_HAND\n");
        hCur = ::LoadCursor(NULL, IDC_HAND);
        break;
    case WebCursorInfo::TypeWait:
        //OutputDebugStringW(L"WebPageImpl::fireCursorEvent  IDC_WAIT\n");
        hCur = ::LoadCursor(NULL, IDC_WAIT);
        break;
    case WebCursorInfo::TypeHelp:
        //OutputDebugStringW(L"WebPageImpl::fireCursorEvent  IDC_HELP\n");
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

static int verticalScrollLines()
{
    static ULONG scrollLines;
    if (!scrollLines && !SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &scrollLines, 0))
        scrollLines = 3;
    return scrollLines;
}

static int horizontalScrollChars()
{
    static ULONG scrollChars;
    if (!scrollChars && !SystemParametersInfo(SPI_GETWHEELSCROLLCHARS, 0, &scrollChars, 0))
        scrollChars = 1;
    return scrollChars;
}

LRESULT WebPageImpl::fireWheelEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CHECK_FOR_REENTER(0);
    freeV8TempObejctOnOneFrameBefore();
    AutoRecordActions autoRecordActions(m_layerTreeHost);

    int x = LOWORD(lParam);
    int y = HIWORD(lParam);
    POINT point = {x, y};
    ::ScreenToClient(hWnd, &point);
    x = point.x;
    y = point.y;

    int wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);

    static const float cScrollbarPixelsPerLine = 100.0f / 3.0f;
    float delta = wheelDelta / static_cast<float>(WHEEL_DELTA);

    float deltaX = 0.f;
    float deltaY = 0.f;

    bool shiftKey = wParam & MK_SHIFT;
    bool ctrlKey = wParam & MK_CONTROL;

    blink::PlatformWheelEventGranularity granularity = blink::ScrollByPageWheelEvent;

    if (shiftKey) {
        deltaX = delta * static_cast<float>(horizontalScrollChars()) * cScrollbarPixelsPerLine;
        deltaY = 0;
        granularity = blink::ScrollByPixelWheelEvent;
    } else {
        deltaX = 0;
        deltaY = delta;
        int verticalMultiplier = verticalScrollLines();
        granularity = (verticalMultiplier == WHEEL_PAGESCROLL) ? blink::ScrollByPageWheelEvent : blink::ScrollByPixelWheelEvent;
        if (granularity == blink::ScrollByPixelWheelEvent)
            deltaY *= static_cast<float>(verticalMultiplier)* cScrollbarPixelsPerLine;
    }

    WebMouseWheelEvent webWheelEvent;
    webWheelEvent.type = WebInputEvent::MouseWheel;
    webWheelEvent.x = x;
    webWheelEvent.y = y;
    webWheelEvent.globalX = x;
    webWheelEvent.globalY = y;
    webWheelEvent.deltaX = deltaX;
    webWheelEvent.deltaY = deltaY;
    webWheelEvent.wheelTicksX = 0.f;
    webWheelEvent.wheelTicksY = delta;
    webWheelEvent.hasPreciseScrollingDeltas = true;
    m_webViewImpl->handleInputEvent(webWheelEvent);

    return 0;
}

bool WebPageImpl::fireKeyUpEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CHECK_FOR_REENTER(false);
    freeV8TempObejctOnOneFrameBefore();
    AutoRecordActions autoRecordActions(m_layerTreeHost);

    WebKeyboardEvent keyEvent = PlatformEventHandler::buildKeyboardEvent(WebInputEvent::KeyUp, message, wParam, lParam);
    return m_webViewImpl->handleInputEvent(keyEvent);
}

bool WebPageImpl::fireKeyDownEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CHECK_FOR_REENTER(false);
    freeV8TempObejctOnOneFrameBefore();
    AutoRecordActions autoRecordActions(m_layerTreeHost);

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
    CHECK_FOR_REENTER(false);
    freeV8TempObejctOnOneFrameBefore();
    AutoRecordActions autoRecordActions(m_layerTreeHost);

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
    CHECK_FOR_REENTER0();
    freeV8TempObejctOnOneFrameBefore();
    AutoRecordActions autoRecordActions(m_layerTreeHost);

    m_platformEventHandler->fireCaptureChangedEvent(hWnd, message, wParam, lParam);
}

void WebPageImpl::fireKillFocusEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    CHECK_FOR_REENTER0();
    freeV8TempObejctOnOneFrameBefore();
}

void WebPageImpl::fireTouchEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    freeV8TempObejctOnOneFrameBefore();

    m_platformEventHandler->fireTouchEvent(hWnd, message, wParam, lParam);
}

LRESULT WebPageImpl::fireMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, BOOL* bHandle)
{
    CHECK_FOR_REENTER(0);
    freeV8TempObejctOnOneFrameBefore();
    AutoRecordActions autoRecordActions(m_layerTreeHost);

    bool handle = false;
//     fireTouchEvent(hWnd, message, wParam, lParam);
//     return; // TODO

    m_platformEventHandler->fireMouseEvent(hWnd, message, wParam, lParam, bHandle);
    return 0;
}

void WebPageImpl::loadURL(int64 frameId, const wchar_t* url, const blink::Referrer& referrer, const wchar_t* extraHeaders)
{
    int length = wcslen(url);
    String urlW((const UChar*)url, length);
	blink::KURL kurl(ParsedURLString, urlW.utf8().data());

	blink::WebURL webURL = kurl;
    blink::WebURLRequest request(webURL);
    loadRequest(frameId, request);
}

void WebPageImpl::loadRequest(int64 frameId, const blink::WebURLRequest& request)
{
    CHECK_FOR_REENTER0();
    if (!m_webViewImpl || !m_webViewImpl->mainFrame())
        return;

    blink::WebURLRequest requestWrap(request);
    WebFrame* webFrame = getWebFrameFromFrameId(frameId);
    if (!webFrame)
        return;

    AutoRecordActions autoRecordActions(m_layerTreeHost);
    
    requestWrap.setHTTPHeaderField(WebString::fromLatin1("Accept"), WebString::fromLatin1("text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"));
    webFrame->loadRequest(requestWrap);
}

void WebPageImpl::loadHTMLString(int64 frameId, const WebData& html, const WebURL& baseURL, const WebURL& unreachableURL, bool replace)
{
    CHECK_FOR_REENTER0();
    if (!m_webViewImpl || !m_webViewImpl->mainFrame())
        return;

    WebFrame* webFrame = getWebFrameFromFrameId(frameId);
    if (!webFrame)
        return;

    AutoRecordActions autoRecordActions(m_layerTreeHost);
    webFrame->loadHTMLString(html, baseURL, unreachableURL, replace);
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

WebWidget* WebPageImpl::createPopupMenu(WebPopupType type)
{
    return PopupMenuWin::create(m_hWnd, m_hWndoffset, m_webViewImpl, type);
}

static void setRuntimeEnabledFeatures()
{
    blink::RuntimeEnabledFeatures::setSlimmingPaintEnabled(false);
    blink::RuntimeEnabledFeatures::setXSLTEnabled(false);
    blink::RuntimeEnabledFeatures::setExperimentalStreamEnabled(false);
    blink::RuntimeEnabledFeatures::setFrameTimingSupportEnabled(false);
    blink::RuntimeEnabledFeatures::setSharedWorkerEnabled(false);
    blink::RuntimeEnabledFeatures::setOverlayScrollbarsEnabled(false);
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
    settings->setSpatialNavigationEnabled(true);
    settings->setLocalStorageEnabled(true);
    settings->setMinimumFontSize(10);
    settings->setMinimumLogicalFontSize(10);
    settings->setDefaultFontSize(16);
    settings->setDefaultFixedFontSize(16);
    settings->setLoadsImagesAutomatically(true);
    settings->setPluginsEnabled(true);

    PluginDatabase::installedPlugins()->refresh();

    // settings->setFontRenderingMode(AlternateRenderingMode);

    return true;
}

} // blink