
#include "windows.h"
#include "commctrl.h"
#undef max
#undef min

#define UseKdMsgSystem 1
#define QueryPerformance 1

#include <base/basictypes.h>

#include "third_party/WebKit/Source/wtf/text/qt4/UnicodeQt4.h"
#include "third_party/WebKit/Source/platform/weborigin/KURL.h"
#include "third_party/WebKit/Source/core/frame/Frame.h"
#include "third_party/WebKit/Source/core/frame/Settings.h"
#include "third_party/WebKit/Source/core/frame/FrameView.h"
#include "third_party/WebKit/Source/core/page/Page.h"
#include "third_party/WebKit/Source/core/frame/LocalFrame.h"
#include "third_party/WebKit/Source/core/page/FocusController.h"
#include "third_party/WebKit/Source/core/input/EventHandler.h"
#include "third_party/WebKit/Source/core/loader/FrameLoadRequest.h"

#include "third_party/WebKit/Source/platform/graphics/GraphicsContext.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "third_party/WebKit/Source/platform/geometry/win/IntRectWin.h"
#include "third_party/WebKit/Source/platform/graphics/paint/DisplayItemList.h"
#include "third_party/WebKit/Source/platform/win/PlatformMouseEventWin.h"
#include "third_party/WebKit/Source/platform/network/ResourceRequest.h"
#include "third_party/WebKit/Source/platform/weborigin/SecurityOrigin.h"

#include "third_party/WebKit/Source/web/ChromeClientImpl.h"
#include "third_party/WebKit/Source/web/ContextMenuClientImpl.h"
#include "third_party/WebKit/Source/web/EditorClientImpl.h"
#include "third_party/WebKit/Source/web/DragClientImpl.h"
#include "third_party/WebKit/Source/web/WebViewImpl.h"
#include "third_party/WebKit/Source/web/FrameLoaderClientImpl.h"
#include "third_party/WebKit/public/web/WebFrameClient.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebWindowFeatures.h"
#include "third_party/WebKit/Source/web/WebLocalFrameImpl.h"
#include "third_party/WebKit/Source/web/WebSettingsImpl.h"

#include "content/win/BlinkPlatformImpl.h"
#include "third_party/WebKit/public/web/WebKit.h"
#include "third_party/WebKit/Source/wtf/MainThread.h"
#include "gin/public/isolate_holder.h"
#include "gin/array_buffer.h"
#include "ui/gfx/win/dpi.h"
#include "gen/blink/platform/RuntimeEnabledFeatures.h"

#include "WebFrameClientImpl.h"
#include "content/win/WebThreadImpl.h"
#include "skia/ext/platform_canvas.h"

#include "wtypes.h"
#include "WebPage.h"
#include "WebPagePaint.h"

extern WCHAR szTitle[];
extern WCHAR szWindowClass[];

namespace blink {

void WebPage::initBlink()
{
    blink::RuntimeEnabledFeatures::setSlimmingPaintEnabled(false);
    blink::RuntimeEnabledFeatures::setXSLTEnabled(false);
    blink::RuntimeEnabledFeatures::setExperimentalStreamEnabled(false);
    blink::RuntimeEnabledFeatures::setFrameTimingSupportEnabled(false);
    blink::RuntimeEnabledFeatures::setFrameTimingSupportEnabled(false);
    blink::RuntimeEnabledFeatures::setSharedWorkerEnabled(false);
    blink::RuntimeEnabledFeatures::setOverlayScrollbarsEnabled(true);

    gfx::win::InitDeviceScaleFactor();
    blink::Platform::initialize(new content::BlinkPlatformImpl());
    gin::IsolateHolder::Initialize(gin::IsolateHolder::kNonStrictMode, gin::ArrayBufferAllocator::SharedInstance());
    blink::initialize(blink::Platform::current());
}

WebView* WebPage::createView(WebLocalFrame* creator,
    const WebURLRequest& request,
    const WebWindowFeatures& features,
    const WebString& name,
    WebNavigationPolicy policy,
    bool suppressOpener)
{
    return WebViewImpl::create(this);
}

WebPage::WebPage(KdGuiObjPtr kdGuiObj, void* foreignPtr)
{
    m_kdGuiObj = kdGuiObj;
    m_foreignPtr = foreignPtr;

    m_hWnd = NULL;
    m_state = UNINIT;

    m_frameLoaderClient = NULL;

    m_chromeClient = NULL;
    m_page = NULL;

    m_pagePaint = NULL;

    m_isAlert = false;
    m_isDraggableRegionNcHitTest = false;
    m_lastMouseDownTime = 0;

    m_canScheduleResourceLoader = false;

    m_callbacks.m_xmlHaveFinished = 0;

    //m_callbacks.m_javascriptCallCppPtr = 0;

//     m_debugInfo.addr = htonl (INADDR_ANY);
//     m_debugInfo.port = htons(0);
    //m_remoteScriptDbg = 0;
}

WebPage::~WebPage()
{
    m_state = DESTROYING;
    FrameLoader *loader = &localFrame()->loader();
    if (loader)
        loader->detach();

    if (m_pagePaint) {
        delete m_pagePaint;
        m_pagePaint = 0;
    }

    // 在Page::~Page()中销毁
    if (m_contextMenuClient)
        delete m_contextMenuClient;
    m_contextMenuClient = 0;
    
    
    if (m_editorClient)
        delete m_editorClient;
    m_editorClient = 0;

    if (m_dragClient)
        delete m_dragClient;
    m_dragClient = 0;
    
    if (m_page)
        delete m_page;
    m_page = 0;
}

void WebPage::setCanScheduleResourceLoader() 
{
    m_canScheduleResourceLoader = true;
}

bool WebPage::init(HWND hWnd)
{
    if (UNINIT != m_state)
        return true;

    m_bMouseTrack = true;

    memset(&m_callbacks, 0, sizeof(KWebApiCallbackSet));

    m_hWnd = hWnd;

    m_pagePaint = new WebPagePaint();
    m_pagePaint->init(this);
    
    WebWindowFeatures windowFeatures;
    windowFeatures.x = 0;
    windowFeatures.xSet = true;
    windowFeatures.y = 0;
    windowFeatures.ySet = true;
    windowFeatures.width = 300;
    windowFeatures.widthSet = true;
    windowFeatures.height = 300;//float height;
    windowFeatures.heightSet = true;
    windowFeatures.menuBarVisible = false;
    windowFeatures.statusBarVisible = false;
    windowFeatures.toolBarVisible = false;
    windowFeatures.locationBarVisible = false;
    windowFeatures.scrollbarsVisible = false;
    windowFeatures.resizable = false;
    windowFeatures.fullscreen = false;
    windowFeatures.dialog = true;

    WebURLRequest request;
    m_webFrameClient = new WebFrameClientImpl(this);
    WebLocalFrameImpl* webLocalFrame = (WebLocalFrameImpl*)WebLocalFrame::create(WebTreeScopeType::Document, m_webFrameClient);
    m_pagePaint->m_webViewImpl = (WebViewImpl*)createView(webLocalFrame, request, windowFeatures, WebString(L"miniblink", 9), WebNavigationPolicyCurrentTab, false);
    m_pagePaint->m_webViewImpl->setMainFrame(webLocalFrame);

    initSetting();

    //m_page->settings()->setDefaultTextEncodingName("iso-8859-1");
    WebLocalFrameImpl* webLocalFrameImpl = WebLocalFrameImpl::create(WebTreeScopeType::Document, m_webFrameClient);
    m_frameLoaderClient = new FrameLoaderClientImpl(webLocalFrameImpl);

    //m_frame->view()->setTransparent(m_pagePaint->m_useLayeredBuffer);

    m_state = INIT;

    return true;
}

void WebPage::setViewportSize(const IntSize& size)
{
    m_viewportSize = size;
    if (UNINIT == m_state)
        init(m_hWnd);
    else if (DESTROYING == m_state)
        return;
    
    if (size.isEmpty() || !localFrame())
        return;

    if (!m_pagePaint)
        return;
    m_pagePaint->m_webViewImpl->resize(WebSize(size));

    m_pagePaint->m_clientRect = IntRect(0, 0, size.width(), size.height());

    RECT rtWnd;
    ::GetWindowRect(m_hWnd, &rtWnd);
    m_pagePaint->m_winodwRect = winRectToIntRect(rtWnd);
    m_pagePaint->m_hasResize = true;
}

LocalFrame* WebPage::localFrame()
{
    return m_pagePaint->m_webViewImpl->mainFrameImpl()->frame();
}

static bool ResourceLoaderFromBuf(Vector<AsynchronousResLoadInfo*>& asynVec, const String url, KdValArray* resData)
{
//     for (size_t i = 0; i < asynVec.size(); ++i) {
//         AsynchronousResLoadInfo* pInfo = asynVec[i];
//         if (url == pInfo->m_url) {
//             if (pInfo->m_bNeedSavaRes) {
//                 resData->Resize(pInfo->m_pAlloc->GetSize());
//                 resData->SetSize(pInfo->m_pAlloc->GetSize());
//                 memcpy(resData->GetData(), pInfo->m_pAlloc->GetData(), resData->GetSize());
//             } else {
//                 resData->Resize(pInfo->m_nResBufLen);
//                 resData->SetSize(pInfo->m_nResBufLen);
//                 memcpy(resData->GetData(), pInfo->m_pResBuf, resData->GetSize());
//             }
//             asynVec.remove(i, 1);
//             delete pInfo;
//             return true;
//         }
//     }

    return false;
}

// 本次点击是一次模拟标题栏
void WebPage::setIsDraggableRegionNcHitTest()
{
    m_isDraggableRegionNcHitTest = true;
}

void WebPage::javaScriptAlert(String& message)
{
    if (true == m_isAlert)
        {return;}
    
    m_isAlert = true;
    //MessageBoxW(NULL, (LPCWSTR)message.charactersWithNullTermination(), L"KdGui", 0);
    m_isAlert = false;
}

void WebPage::windowCloseRequested()
{
    if (INIT != m_state)
        return;
    m_state = DESTROYING;

    void* pKdGuiForeignPtr = getKdGuiObjPtr() ? getKdGuiObjPtr()->pForeignPtr : 0;
    if (m_callbacks.m_unintCallBack) {
//         m_callbacks.m_unintCallBack(this, 
//             pKdGuiForeignPtr, 
//             getForeignPtr(),
//             m_hWnd);
    }

//     // 在这里给脚本发消息，而不是在Frame::pageDestroyed()
//     frame()->script()->willCloseScript();
// 
//     if (m_remoteScriptDbg) {
//         sq_rdbg_shutdown(m_remoteScriptDbg);}
    
    ::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, 0);

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

    delete m_pagePaint;
    m_pagePaint = 0;

    // TODO_Weolar
    // 发生unload消息给脚本

    // 清空webkit的资源缓存
    //memoryCache()->evictResources();

    ::KillTimer(m_hWnd, (UINT_PTR)this);
}

#ifndef NDEBUG
BOOL gd_bShowTree = FALSE;
#endif

void WebPage::timerFired()
{
    content::WebThreadImpl* thread = (content::WebThreadImpl*)blink::Platform::current()->currentThread();
    thread->startTriggerTasks();
    thread->schedulerTasks();

    if (m_pagePaint)
        m_pagePaint->beginMainFrame();
    
#ifndef NDEBUG
    if (gd_bShowTree)
        showDebugNodeData();
#endif
}

void WebPage::showDebugNodeData()
{
#ifndef NDEBUG
    localFrame()->document()->showTreeForThis();
#endif
}

void WebPage::resizeEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (INIT != m_state || m_isAlert)
        return;

    UINT cx, cy;
//     cx = LOWORD(lParam);
//     cy = HIWORD(lParam);

    RECT rcClient;
    GetClientRect(hWnd, &rcClient);
    cx = rcClient.right - rcClient.left;
    cy = rcClient.bottom - rcClient.top;

    if (!m_hWnd)
        m_hWnd = hWnd;

    IntSize size(cx, cy);
    setViewportSize(size);
}

void WebPage::repaintRequested(const IntRect& windowRect)
{
    if (INIT != m_state || windowRect.isEmpty() || windowRect.maxY() < 0 || windowRect.maxX() < 0)
        return;

//  RECT winRect = {windowRect.x(), windowRect.y(), 
//    windowRect.x() + windowRect.width(), windowRect.y() + windowRect.height()};
//  WCHAR msg[200] = {0};
//  wsprintfW(msg, L"repaintRequested : %d %d %d %d\n", windowRect.x(), windowRect.y(), windowRect.width(), windowRect.maxY());
//  OutputDebugStringW(msg);

//  ::InvalidateRect(m_hWnd, &winRect, false);
    m_pagePaint->postPaintMessage(&windowRect);
}

// Called when a region of the WebWidget needs to be re-painted.
void WebPage::didInvalidateRect(const WebRect& r)
{
    IntRect windowRect(r);
    if (-1 == windowRect.width() || -1 == windowRect.height())
        windowRect = m_pagePaint->m_clientRect;
    repaintRequested(windowRect);
    m_pagePaint->setNeedsCommit();
}

// Called when the Widget has changed size as a result of an auto-resize.
void WebPage::didAutoResize(const WebSize& newSize)
{
    //notImplemented();
}

void WebPage::didUpdateLayoutSize(const WebSize& newSize)
{
    //notImplemented();
}

void WebPage::scheduleAnimation()
{
    if (m_pagePaint)
        m_pagePaint->setNeedsCommit();
}

void WebPage::initializeLayerTreeView()
{
    ;
}

// Return a compositing view used for this widget. This is owned by the
// WebWidgetClient.
WebLayerTreeView* WebPage::layerTreeView()
{
    if (m_pagePaint)
        return m_pagePaint->layerTreeHost();
    return nullptr;
}

void WebPage::paintEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (true == m_isAlert)
        return;

    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);
    m_pagePaint->paintEvent(ps.hdc, &ps.rcPaint);
    EndPaint(hWnd, &ps);
}

static void makeDraggableRegionNcHitTest(HWND hWnd, LPARAM lParam, bool* isDraggableRegionNcHitTest, IntPoint& lastPosForDrag)
{
    int xPos = ((int)(short)LOWORD(lParam));
    int yPos = ((int)(short)HIWORD(lParam));
    if (true == *isDraggableRegionNcHitTest) {
        //::PostMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(xPos, yPos));
        ::PostMessage(hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
        lastPosForDrag = IntPoint(xPos, yPos);
    } else {
        ::SetCapture(hWnd);
    }
    //*isDraggableRegionNcHitTest = false;
}

void WebPage::captureChangedEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//     const INT vkey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON;
//     LONG bLButtonIsDown = GetAsyncKeyState(vkey) & 0x8000;
//     
//     WCHAR msg[100] = {0};
//     swprintf(msg, L"WebPage::captureChangedEvent 1: %x %x\n", m_isDraggableRegionNcHitTest, bLButtonIsDown);
//     OutputDebugStringW(msg);

    if (m_isDraggableRegionNcHitTest /*&& 0 == bLButtonIsDown*/) {
        ::ReleaseCapture();
        m_isDraggableRegionNcHitTest = false;

//         POINT ptCursor;
//         ::GetCursorPos(&ptCursor);
//         ::ScreenToClient(hWnd, &ptCursor);
//         lParam = MAKELONG(ptCursor.x, ptCursor.y);
        lParam = MAKELONG(m_lastPosForDrag.x(), m_lastPosForDrag.y());
        mouseEvent(hWnd, WM_LBUTTONUP, wParam, lParam);
    }
}

void WebPage::killFocusEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // only set the focused frame inactive so that we stop painting the caret
    // and the focus frame. But don't tell the focus controller so that upon
    // focusInEvent() we can re-activate the frame.
    FocusController* focusController = &page()->focusController();
    // Call setFocused first so that window.onblur doesn't get called twice
    focusController->setFocused(false);
    focusController->setActive(false);
    focusController->focusDocumentView((localFrame()));
}

void WebPage::mouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    m_isDraggableRegionNcHitTest = false;
    if (true == m_isAlert)
        return;

    if (m_bMouseTrack && m_pagePaint && !m_pagePaint->m_postMouseLeave) { // 若允许追踪，则
        TRACKMOUSEEVENT csTME;
        csTME.cbSize = sizeof(csTME);
        csTME.dwFlags = TME_LEAVE|TME_HOVER;
        csTME.hwndTrack = m_hWnd;  // 指定要追踪的窗口
        csTME.dwHoverTime = 10;    // 鼠标在按钮上停留超过10ms，才认为状态为HOVER
        ::TrackMouseEvent(&csTME); // 开启Windows的WM_MOUSELEAVE，WM_MOUSEHOVER事件支持
        m_bMouseTrack = false;     // 若已经追踪，则停止追踪
    }

    bool shift = false, ctrl = false, alt = false, meta = false;
    int clickCount = 0;

    IntPoint pos;
    IntPoint globalPos;

    if (!localFrame())
        return;

    if (WM_MOUSELEAVE == message) {
//  if (m_pagePaint)
//      m_pagePaint->m_postMouseLeave = true;

        POINT ptCursor;
        ::GetCursorPos(&ptCursor);
        globalPos = IntPoint(ptCursor.x, ptCursor.y);
        ::ScreenToClient(hWnd, &ptCursor);
        if (ptCursor.x < 2)
            ptCursor.x = -1;
        else if (ptCursor.x > 10)
            ptCursor.x += 2;

        if (ptCursor.y < 2)
            ptCursor.y = -1;
        else if (ptCursor.y > 10)
            ptCursor.y += 2;

        pos = IntPoint(ptCursor.x, ptCursor.y);

        lParam = MAKELPARAM(ptCursor.x, ptCursor.y);
    } else {
        pos.setX(((int)(short)LOWORD(lParam)));
        pos.setY(((int)(short)HIWORD(lParam)));

        POINT widgetpt = {pos.x(), pos.y()};
        ::ClientToScreen(hWnd, &widgetpt);
        globalPos.setX(widgetpt.x);
        globalPos.setY(widgetpt.y);
    }

    if (WM_MOUSELEAVE == message)
        m_bMouseTrack = true;

    PlatformMouseEventWin ev(hWnd, message, wParam, lParam, FALSE);

    double time = WTF::currentTime();
    WebMouseEvent webMouseEvent;
    webMouseEvent.timeStampSeconds = WTF::currentTime();
    webMouseEvent.size = sizeof(WebMouseEvent);
    webMouseEvent.modifiers = 0;
    webMouseEvent.x = pos.x();
    webMouseEvent.y = pos.y();
    webMouseEvent.movementX = pos.x();
    webMouseEvent.movementY = pos.y();
    webMouseEvent.windowX = pos.x();
    webMouseEvent.windowY = pos.y();
    webMouseEvent.globalX = globalPos.x();
    webMouseEvent.globalY = globalPos.y();
    webMouseEvent.clickCount = 1;

    if (WM_LBUTTONDOWN == message || WM_MBUTTONDOWN == message || WM_RBUTTONDOWN == message) {
        switch (message) {
        case WM_LBUTTONDOWN:
            webMouseEvent.button = WebMouseEvent::ButtonLeft;
            break;
        case WM_MBUTTONDOWN:
            webMouseEvent.button = WebMouseEvent::ButtonMiddle;
            break;
        case WM_RBUTTONDOWN:
            webMouseEvent.button = WebMouseEvent::ButtonRight;
            break;
        }
        m_isDraggableRegionNcHitTest = false;
        webMouseEvent.type = WebInputEvent::MouseDown;
        if (time - m_lastMouseDownTime < 0.45) {
            webMouseEvent.clickCount = 2;
            ev.setClickCount(2);
        }
        m_lastMouseDownTime = time;
        m_pagePaint->m_webViewImpl->handleInputEvent(webMouseEvent);
        //localFrame()->eventHandler().handleMousePressEvent(ev);
        makeDraggableRegionNcHitTest(hWnd, lParam, &m_isDraggableRegionNcHitTest, m_lastPosForDrag);
    } else if (WM_LBUTTONUP == message || WM_MBUTTONUP == message || WM_RBUTTONUP == message) {
        switch (message) {
        case WM_LBUTTONUP:
            webMouseEvent.button = WebMouseEvent::ButtonLeft;
            break;
        case WM_MBUTTONUP:
            webMouseEvent.button = WebMouseEvent::ButtonMiddle;
            break;
        case WM_RBUTTONUP:
            webMouseEvent.button = WebMouseEvent::ButtonRight;
            break;
        }
        //localFrame()->eventHandler().handleMouseReleaseEvent(ev);
        ::ReleaseCapture();
        webMouseEvent.type = WebInputEvent::MouseUp;

        if (m_pagePaint->m_dragging) {
            m_pagePaint->m_webViewImpl->dragSourceSystemDragEnded();
            m_pagePaint->m_dragging = false;
        }

        m_pagePaint->m_webViewImpl->handleInputEvent(webMouseEvent);
        
    } else if (WM_MOUSEMOVE == message || WM_MOUSELEAVE == message) {
        if (wParam & MK_LBUTTON)
            webMouseEvent.button = WebMouseEvent::ButtonLeft;
        else if (wParam & MK_MBUTTON)
            webMouseEvent.button = WebMouseEvent::ButtonMiddle;
        else if (wParam & MK_RBUTTON)
            webMouseEvent.button = WebMouseEvent::ButtonRight;
        else
            webMouseEvent.button = WebMouseEvent::ButtonNone;

        switch (message) {
        case WM_MOUSEMOVE:
            if (!m_pagePaint->m_mouseInWindow) {
                webMouseEvent.type = WebInputEvent::MouseEnter;
                m_pagePaint->m_mouseInWindow = true;
            } else
                webMouseEvent.type = WebInputEvent::MouseMove;
            m_pagePaint->m_webViewImpl->handleInputEvent(webMouseEvent);
            break;
        case WM_MOUSELEAVE:
            //localFrame()->eventHandler().handleMouseMoveEvent(ev);
            webMouseEvent.type = WebInputEvent::MouseLeave;
            m_pagePaint->m_webViewImpl->handleInputEvent(webMouseEvent);
            m_pagePaint->m_mouseInWindow = false;
            break;
        }
    }
}

void WebPage::startDragging(WebLocalFrame*, const WebDragData&, WebDragOperationsMask, const WebImage&, const WebPoint& dragImageOffset)
{
    m_pagePaint->m_dragging = true;
}

#if 1

// 返回1表示调用def函数。这是因为1是S_FALSE
int WebPage::inputEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
//     bool handled = false;
//     WebCore::Frame* frame = m_page->focusController()->focusedOrMainFrame();
//     
//     // we forward the key event to WebCore first to handle potential DOM
//     // defined event handlers and later on end up in EditorClientQt::handleKeyboardEvent
//     // to trigger editor commands via triggerAction().
// 
//     PlatformKeyboardEvent::Type type = (PlatformKeyboardEvent::Type)-1;
//     if (WM_KEYUP == message) {
//         type = PlatformKeyboardEvent::KeyUp;
//     } else if (WM_KEYDOWN == message) {
//         type = PlatformKeyboardEvent::RawKeyDown;
//     } else if (WM_CHAR == message) {
//         type = PlatformKeyboardEvent::Char;
//     }
// 
//     if ((PlatformKeyboardEvent::Type)-1 != type) {
//         PlatformKeyboardEvent evt(hWnd, wParam, lParam, type, true);
//         handled = frame->eventHandler()->keyEvent(evt);
//         if (handled)
//             return 1;
//     }

    return inputEventToRichEdit(hWnd, message, wParam, lParam);
}

// 一般都是richedit的事件
bool WebPage::inputEventToRichEdit(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    bool handled = false;

//     Node* node = m_frame->document()->focusedNode();
//     if (node && node->localName() == *SVGNames::svgricheditbaseTag) {
//         SVGRichEditBaseElement* edit = (SVGRichEditBaseElement*)node;
//         PassRefPtr<WinNativeEvent> evt = WinNativeEvent::create(hWnd, message, wParam, lParam);
//         edit->defaultEventHandler((Event*)evt.get());
//         return evt->m_hRes;
//     } else {
//         node = node;
//     }
    
    return 1;
}

#endif

void WebPage::loadFormUrl(LPCWSTR lpUrl)
{
    String urlW((const UChar*)lpUrl);
    KURL url(ParsedURLString, urlW.utf8().data());
    
//     WebURL webURL = url;
//     WebSecurityOrigin webSecurityOrigin = WebSecurityOrigin::create(webURL);
//     webSecurityOrigin.grantLoadLocalResources();
//     m_webViewImpl->mainFrameImpl()->setIsolatedWorldSecurityOrigin(0, webSecurityOrigin);

    ResourceRequest request(url);
    FrameLoadRequest loadRequest(localFrame()->document(), request);
    localFrame()->loader().load(loadRequest); // 在ResourceHandle::start(Frame* frame)中接受数据

    setCanScheduleResourceLoader();
}

void WebPage::loadFormData(const void* lpData, int nLen)
{
//     String sUrl;
//     sUrl = sUrl.format("res:///kdData_%f.svg", randomNumber());
// 
//     postResToAsynchronousLoad((const WCHAR *)sUrl.charactersWithNullTermination(), lpData, nLen, true);
//     loadFormUrl((const WCHAR *)sUrl.charactersWithNullTermination());
}

void WebPage::postResToAsynchronousLoad(const WCHAR* pUrl, const void* pResBuf, int nResBufLen, bool bNeedSavaRes)
{
//     AsynchronousResLoadInfo* pInfo = new AsynchronousResLoadInfo((const UChar *)pUrl, pResBuf, nResBufLen, bNeedSavaRes);
//     m_asynResQueue.append(pInfo);
}

void WebPage::setBackgroundColor(COLORREF c) {
    m_pagePaint->m_bdColor = c;
}

// bool WebPage::invokeScript(NPIdentifier methodName, const NPVariant* args, uint32_t argCount, NPVariant* result)
// {
// //     NPObject* o = frame()->script()->windowScriptNPObject();
// //     return _NPN_Invoke(0, o, methodName, args, argCount, result);
// }

bool WebPage::initSetting()
{
    WebSettingsImpl* settings = m_pagePaint->m_webViewImpl->settingsImpl();
    if (!settings)
        return false;
    settings->setTextAreasAreResizable(true);

    settings->setStandardFontFamily(WebString(L"微软雅黑", 4));
    settings->setUsesEncodingDetector(true);
    settings->setJavaScriptEnabled(true);
    settings->setAllowFileAccessFromFileURLs(true);
    settings->setAcceleratedCompositingEnabled(true);
    settings->setUseSolidColorScrollbars(true);

    //         QWebSettingsPrivate *global = QWebSettings::globalSettings()->d;
    // 
    //         QString family = fontFamilies.value(QWebSettings::StandardFont,
    //             global->fontFamilies.value(QWebSettings::StandardFont));
    //         settings->setStandardFontFamily(family);
    // 
    //         family = fontFamilies.value(QWebSettings::FixedFont,
    //             global->fontFamilies.value(QWebSettings::FixedFont));
    //         settings->setFixedFontFamily(family);
    // 
    //         family = fontFamilies.value(QWebSettings::SerifFont,
    //             global->fontFamilies.value(QWebSettings::SerifFont));
    //         settings->setSerifFontFamily(family);
    // 
    //         family = fontFamilies.value(QWebSettings::SansSerifFont,
    //             global->fontFamilies.value(QWebSettings::SansSerifFont));
    //         settings->setSansSerifFontFamily(family);
    // 
    //         family = fontFamilies.value(QWebSettings::CursiveFont,
    //             global->fontFamilies.value(QWebSettings::CursiveFont));
    //         settings->setCursiveFontFamily(family);
    // 
    //         family = fontFamilies.value(QWebSettings::FantasyFont,
    //             global->fontFamilies.value(QWebSettings::FantasyFont));
    //         settings->setFantasyFontFamily(family);
    // 
    //         int size = fontSizes.value(QWebSettings::MinimumFontSize,
    //             global->fontSizes.value(QWebSettings::MinimumFontSize));
    settings->setMinimumFontSize(10);
    // 
    //         size = fontSizes.value(QWebSettings::MinimumLogicalFontSize,
    //             global->fontSizes.value(QWebSettings::MinimumLogicalFontSize));
    settings->setMinimumLogicalFontSize(10);
    // 
    //         size = fontSizes.value(QWebSettings::DefaultFontSize,
    //             global->fontSizes.value(QWebSettings::DefaultFontSize));
    settings->setDefaultFontSize(16);
    // 
    //         size = fontSizes.value(QWebSettings::DefaultFixedFontSize,
    //             global->fontSizes.value(QWebSettings::DefaultFixedFontSize));
    settings->setDefaultFixedFontSize(16);
    // 
    //         bool value = attributes.value(QWebSettings::AutoLoadImages,
    //             global->attributes.value(QWebSettings::AutoLoadImages));
    settings->setLoadsImagesAutomatically(true);
    // 
    //         value = attributes.value(QWebSettings::JavascriptEnabled,
    //             global->attributes.value(QWebSettings::JavascriptEnabled));
    settings->setJavaScriptEnabled(true);
    // 
    //         value = attributes.value(QWebSettings::JavascriptCanOpenWindows,
    //             global->attributes.value(QWebSettings::JavascriptCanOpenWindows));
    //         settings->setJavaScriptCanOpenWindowsAutomatically(value);
    // 
    //         value = attributes.value(QWebSettings::JavaEnabled,
    //             global->attributes.value(QWebSettings::JavaEnabled));
    //         settings->setJavaEnabled(value);
    // 
    //         value = attributes.value(QWebSettings::PluginsEnabled,
    //             global->attributes.value(QWebSettings::PluginsEnabled));
    //         settings->setPluginsEnabled(value);
    // 
    //         value = attributes.value(QWebSettings::PrivateBrowsingEnabled,
    //             global->attributes.value(QWebSettings::PrivateBrowsingEnabled));
    //         settings->setPrivateBrowsingEnabled(value);
    // 
    //         value = attributes.value(QWebSettings::JavascriptCanAccessClipboard,
    //             global->attributes.value(QWebSettings::JavascriptCanAccessClipboard));
    //         settings->setDOMPasteAllowed(value);
    // 
    //         value = attributes.value(QWebSettings::DeveloperExtrasEnabled,
    //             global->attributes.value(QWebSettings::DeveloperExtrasEnabled));
    //         settings->setDeveloperExtrasEnabled(value);
    // 
    //         QUrl location = !userStyleSheetLocation.isEmpty() ? userStyleSheetLocation : global->userStyleSheetLocation;
    //         settings->setUserStyleSheetLocation(WebCore::KURL(location));
    // 
    //         QString localStoragePath = !localStorageDatabasePath.isEmpty() ? localStorageDatabasePath : global->localStorageDatabasePath;
    //         settings->setLocalStorageDatabasePath(localStoragePath);
    // 
    //         value = attributes.value(QWebSettings::ZoomTextOnly,
    //             global->attributes.value(QWebSettings::ZoomTextOnly));
    //         settings->setZoomsTextOnly(value);
    // 
    //         value = attributes.value(QWebSettings::PrintElementBackgrounds,
    //             global->attributes.value(QWebSettings::PrintElementBackgrounds));
    //         settings->setShouldPrintBackgrounds(value);
    // 
    //         value = attributes.value(QWebSettings::OfflineStorageDatabaseEnabled,
    //             global->attributes.value(QWebSettings::OfflineStorageDatabaseEnabled));
    //         settings->setDatabasesEnabled(value);
    // 
    //         value = attributes.value(QWebSettings::OfflineWebApplicationCacheEnabled,
    //             global->attributes.value(QWebSettings::OfflineWebApplicationCacheEnabled));
    //         settings->setOfflineWebApplicationCacheEnabled(value);
    // 
    //         value = attributes.value(QWebSettings::LocalStorageDatabaseEnabled,
    //             global->attributes.value(QWebSettings::LocalStorageDatabaseEnabled));
    //         settings->setLocalStorageEnabled(value);

    // settings->setFontRenderingMode(AlternateRenderingMode);

    return true;
}

} // namespace WebCore