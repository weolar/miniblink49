
#if ENABLE_IN_MB_MAIN
#if ENABLE_NODEJS
#include "nodeblink.h"
#include "node/src/node.h"
#include "node/src/env.h"
#include "node/src/env-inl.h"
#endif
#endif

#include "core/MbWebView.h"
#include "core/PromptWnd.h"

#if 1 // ENABLE_IN_MB_MAIN
#include "printing/PdfViewerPluginFunc.h"
#endif

#include "common/ThreadCall.h"
#include "common/LiveIdDetect.h"
#include "common/BindJsQuery.h"
#include "common/TimeUtil.h"
#if ENABLE_IN_MB_MAIN
#include "ffax/ffaxmain.h"
#include "core/RenderInit.h"
#endif
#include <vector>

namespace mb {

int atomicIncrement(int volatile* addend) { return InterlockedIncrement(reinterpret_cast<long volatile*>(addend)); }
int atomicDecrement(int volatile* addend) { return InterlockedDecrement(reinterpret_cast<long volatile*>(addend)); }
const wchar_t* kClassWndName = L"mbWebWindowClass";
extern unsigned int g_mbMask;
extern bool g_enableNativeSetCapture;
extern bool g_enableNativeSetFocus;

MbWebView::MbWebView()
{
    m_id = common::LiveIdDetect::get()->constructed(this);
    m_cursorInfoType = 0;
    m_isCursorInfoTypeAsynGetting = false;
    m_isCursorInfoTypeAsynChanged = false;
    m_state = kPageInited;
    m_wkeWebview = nullptr;
    m_isTransparent = false;
    m_isWebWindowMode = false;
    m_isAutoDrawToHwnd = true;
    m_isShow = false;
    m_hWnd = nullptr;
    m_isEnableNode = false;
    m_memoryBMP = nullptr;
    m_memoryDC = nullptr;
    m_isLayerWindow = false;
    m_clientSize.cx = 0;
    m_clientSize.cy = 0;
    m_clientSizeDirty = true;
    
    m_offset.x = 0;
    m_offset.y = 0;
    m_hasSetPaintUpdatedCallback = false;
    m_createWebViewRequestCount = 1;
    m_navigateIndex = 0;
    m_env = nullptr;
    m_draggableRegion = nullptr;
    //m_closeCallback = nullptr;
    m_destroyCallback = nullptr;
    m_printingCallback = nullptr;
    m_printing = nullptr;
    m_enableMouseKeyMessage = true;
    m_zoomFactor = 1.0f;
    m_backgroundColor = 0xffffffff;
    m_mainFrameId = 0;
    ::InitializeCriticalSection(&m_memoryCanvasLock);
    ::InitializeCriticalSection(&m_mouseMsgQueueLock);
    ::InitializeCriticalSection(&m_userKeyValuesLock);
    ::InitializeCriticalSection(&m_dirtyRectLock);
    ::InitializeCriticalSection(&m_clientSizeLock);
}

void MbWebView::preDestroy()
{
    common::LiveIdDetect::get()->deconstructed(m_id);
    m_state = kPageDestroying;
    ::RevokeDragDrop(m_hWnd);
}

MbWebView::~MbWebView()
{
    m_state = kPageDestroyed;

    int count = 0;
    while (m_hWnd && ::IsWindow(m_hWnd)) {
        ++count;
        ::Sleep(100);
#ifndef _DEBUG
        if (count > 6)
            break;
#endif // DEBUG
    }

    if (!common::ThreadCall::isBlinkThread())
        DebugBreak();

    m_hWnd = nullptr;

    if (m_memoryBMP)
        ::DeleteObject(m_memoryBMP);

    if (m_memoryDC)
        ::DeleteDC(m_memoryDC);

    if (m_draggableRegion)
        ::DeleteObject(m_draggableRegion);

    ::DeleteCriticalSection(&m_memoryCanvasLock);
    ::DeleteCriticalSection(&m_mouseMsgQueueLock);
    
    if (m_wkeWebview)
        wkeDestroyWebView(m_wkeWebview);
}

static void WKE_CALL_TYPE onDraggableRegionsChanged(wkeWebView webWindow, void* param, const wkeDraggableRegion* regions, int rectCount);

void WKE_CALL_TYPE onOnPrintCallback(wkeWebView webView, void* param, wkeWebFrameHandle frameId, void* printParams)
{
    int64_t id = (int64_t)param;
    common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [id, frameId] {
        MbWebView* self = (MbWebView*)common::LiveIdDetect::get()->getPtr(id);
        if (!self)
            return;
        mbUtilPrint(self->getWebviewHandle(), (mbWebFrameHandle)frameId, nullptr);
    });
}

void WKE_CALL_TYPE onTitleChanged(wkeWebView wkeWebview, void* param, const wkeString title)
{
    int64_t id = (int64_t)param;
    MbWebView* self = (MbWebView*)common::LiveIdDetect::get()->getPtr(id);
    if (!self)
        return;

    int index = wkeGetNavigateIndex(self->getWkeWebView());
    self->setNavigateIndex(index);

    std::string* titleString = new std::string(wkeGetString(title));
    common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [id, titleString] {
        MbWebView* self = (MbWebView*)common::LiveIdDetect::get()->getPtr(id);
        if (!self) {
            delete titleString;
            return;
        }

        self->setTitle(*titleString);
        if (self->getClosure().m_TitleChangedCallback)
            self->getClosure().m_TitleChangedCallback(self->getWebviewHandle(), self->getClosure().m_TitleChangedParam, titleString->c_str());

        delete titleString;
    });
}

void WKE_CALL_TYPE onURLChanged(wkeWebView wkeWebview, void* param, wkeWebFrameHandle frameId, const wkeString title)
{
    int64_t id = (int64_t)param;
    MbWebView* self = (MbWebView*)common::LiveIdDetect::get()->getPtr(id);
    if (!self)
        return;

    std::string* urlString = new std::string(wkeGetString(title));

    BOOL isMain = wkeIsMainFrame(wkeWebview, frameId);
    if (!isMain)
        return;

    BOOL canGoBack = wkeCanGoBack(wkeWebview);
    BOOL canGoForward = wkeCanGoForward(wkeWebview);
    int index = wkeGetNavigateIndex(self->getWkeWebView());
    self->setNavigateIndex(index);

    common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [id, urlString, canGoBack, canGoForward] {
        MbWebView* self = (MbWebView*)common::LiveIdDetect::get()->getPtr(id);
        if (!self) {
            delete urlString;
            return;
        }

        self->setUrl(*urlString);

        if (self->getClosure().m_URLChangedCallback)
            self->getClosure().m_URLChangedCallback(self->getWebviewHandle(), self->getClosure().m_URLChangedParam, urlString->c_str(), canGoBack, canGoForward);

        delete urlString;
    });
}

bool WKE_CALL_TYPE onPromptBox(wkeWebView wkeWebview, void* param, const wkeString msg, const wkeString defaultResult, wkeString result)
{
    int64_t id = (int64_t)param;
    MbWebView* self = (MbWebView*)common::LiveIdDetect::get()->getPtr(id);
    if (!self)
        return false;

    mbStringPtr resultVal = nullptr;
    if (self->getClosure().m_PromptBoxCallback)
        resultVal = self->getClosure().m_PromptBoxCallback(self->getWebviewHandle(), self->getClosure().m_PromptBoxParam, wkeGetString(msg), wkeGetString(defaultResult));
    else {
        PromptWnd prompt(self->getHostWnd());
        std::string result = prompt.run(wkeGetString(msg), wkeGetString(defaultResult));
        if (result.size() > 0)
            resultVal = mbCreateString(result.c_str(), result.size());
    }

    if (resultVal) {
        const utf8* str = wkeGetString((const wkeString)resultVal);
        wkeSetString(result, str, strlen(str));
        mbDeleteString(resultVal);
    }
    return true;
}

bool WKE_CALL_TYPE onContextMenuItemClickCallback(wkeWebView webView, void* param, wkeOnContextMenuItemClickType type, wkeOnContextMenuItemClickStep step, wkeWebFrameHandle frameId, void* info)
{
    if (kWkeContextMenuItemClickTypePrint == type) {
        if (kWkeContextMenuItemClickStepShow == step)
            return true;
        if (kWkeContextMenuItemClickStepClick == step)
            onOnPrintCallback(webView, param, frameId, nullptr);
        return true;
    }

    return false;
}

void WKE_CALL_TYPE MbWebView::onCaretChangedCallback(wkeWebView webView, void* param, const wkeRect* r)
{
    int64_t id = (int64_t)param;
    MbWebView* self = (MbWebView*)common::LiveIdDetect::get()->getPtr(id);
    if (!self)
        return;

    ::EnterCriticalSection(&(self->m_clientSizeLock));
    self->m_caretPos = *(mbRect*)r;
    ::LeaveCriticalSection(&(self->m_clientSizeLock));
}

void MbWebView::initWebviewInBlinkThread(wkeWebView wkeWebview)
{
    if (m_wkeWebview)
        DebugBreak();
    setWkeWebView(wkeWebview);
    decrementCreateWebViewRequestCount();

    //wkeSetDebugConfig(wkeWebview, "drawTileLine", "1");
    wkeSetDebugConfig(wkeWebview, "cutOutsNpapiRects", "1");
    wkeSetUserKeyValue(wkeWebview, "MbWebView", (void*)m_id);
    wkeSetCspCheckEnable(wkeWebview, false);
    wkeOnDidCreateScriptContext(wkeWebview, onDidCreateScriptContext, (void*)m_id);
    wkeOnWillReleaseScriptContext(wkeWebview, onWillReleaseScriptContext, (void*)m_id);
    wkeOnPaintUpdated(wkeWebview, onPaintUpdated, (void*)m_id);
    wkeOnCreateView(wkeWebview, onCreateView, (void*)m_id);
    if (m_hWnd)
        wkeSetHandle(wkeWebview, m_hWnd);
    wkeOnDraggableRegionsChanged(wkeWebview, &onDraggableRegionsChanged, (void*)m_id);
    wkeOnPrint(wkeWebview, onOnPrintCallback, (void*)m_id);

    wkeOnTitleChanged(wkeWebview, onTitleChanged, (void*)m_id);
    wkeOnURLChanged2(wkeWebview, onURLChanged, (void*)m_id);
    wkeOnPromptBox(wkeWebview, onPromptBox, (void*)m_id);
    wkeOnContextMenuItemClick(wkeWebview, onContextMenuItemClickCallback, (void*)m_id);
    wkeOnCaretChanged(wkeWebview, onCaretChangedCallback, (void*)m_id);

    if (m_isTransparent)
        wkeSetTransparent(wkeWebview, true);

#if 1 // ENABLE_IN_MB_MAIN
    //wkeAddNpapiPlugin(wkeWebview, FFAX_NP_Initialize, FFAX_NP_GetEntryPoints, FFAX_NP_Shutdown);

//     if (!(g_mbMask & MB_ENABLE_DISABLE_PDFVIEW))
//         wkeAddNpapiPlugin(wkeWebview, printing::PdfViewerPluginNPInitialize, printing::PdfViewerPluginNPGetEntryPoints, printing::PdfViewerPluginNPShutdown);
#endif
}

void MbWebView::setHostWnd(HWND hWnd)
{
    m_hWnd = hWnd;

    if (!m_isTransparent)
        m_isTransparent = !!(::GetWindowLong(hWnd, GWL_EXSTYLE) & WS_EX_LAYERED);

    mbWebView webviewHandle = (mbWebView)m_id;
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [](MbWebView* self) {
        if (self->m_wkeWebview)
            wkeSetTransparent(self->m_wkeWebview, self->m_isTransparent);
    });
}

wkeWebView MbWebView::onCreateView(wkeWebView wkeWebview, void* param, wkeNavigationType navigationType, const wkeString url, const wkeWindowFeatures* windowFeatures)
{
    int64_t id = (int64_t)param;
    MbWebView* self = (MbWebView*)common::LiveIdDetect::get()->getPtr(id);
    if (!self)
        return nullptr;

    const utf8* urlString = wkeGetString(url);
    mbWebView result = NULL_WEBVIEW;

    common::ThreadCall::callUiThreadSync(MB_FROM_HERE, [&result, id, navigationType, urlString, windowFeatures] {
        MbWebView* self = (MbWebView*)common::LiveIdDetect::get()->getPtr(id);
        if (!self)
            return;

        mbWebView webviewHandle = (mbWebView)id;
        mbNavigationType type = (mbNavigationType)navigationType;
        if (self->getClosure().m_CreateViewCallback)
            result = self->getClosure().m_CreateViewCallback(webviewHandle, self->getClosure().m_CreateViewParam, type, urlString, (const mbWindowFeatures*)windowFeatures);

        return;
    });

    if (!result)
        return nullptr;
    MbWebView* newWebview = (MbWebView*)common::LiveIdDetect::get()->getPtr(result);
    if (!newWebview)
        return nullptr;

    self = (MbWebView*)common::LiveIdDetect::get()->getPtr(id);
    if (!self)
        return nullptr;

    wkeWebView newWkeWebview = newWebview->getWkeWebViewOrCreateWhenOnCreateView();

    common::ThreadCall::wake(); // 让ui线程调用的api同步执行一遍，防止例如set cookit path这种异步设置会导致漏掉网络请求没被设置path的情况

    return newWkeWebview;
}

void MbWebView::decrementCreateWebViewRequestCount()
{
    atomicDecrement(&m_createWebViewRequestCount);
}

wkeWebView MbWebView::getWkeWebViewOrCreateWhenOnCreateView()
{
    if (!m_wkeWebview && m_createWebViewRequestCount > 0) {
        //setWkeWebView(wkeCreateWebView());
        createWkeWebWindowOrViewInBlinkThread(false);
        //decrementCreateWebViewRequestCount();
    }
    return m_wkeWebview;
}

void MbWebView::createWkeWebWindowOrViewInBlinkThread(bool isWebWindowMode)
{
    wkeWebView wkeWebview = nullptr;
    m_isWebWindowMode = isWebWindowMode;
    if (getCreateWebViewRequestCount() == 0)
        return;
    
    wkeWebview = wkeCreateWebView();
    initWebviewInBlinkThread(wkeWebview);
}

bool MbWebView::isDraggableRegionNcHitTest(HWND hWnd)
{
    bool handle = false;
    if (!m_draggableRegion)
        return handle;

    POINT pos;
    ::GetCursorPos(&pos);
    ::ScreenToClient(hWnd, &pos);

    handle = !!::PtInRegion(m_draggableRegion, pos.x, pos.y);
    return handle;
}

bool MbWebView::doDraggableRegionNcHitTest(HWND hWnd)
{
    bool handle = isDraggableRegionNcHitTest(hWnd);
    if (handle) {
        HWND hRootWnd = hWnd;
        while (true) {
            hWnd = ::GetParent(hWnd);
            if (!hWnd)
                break;
            hRootWnd = hWnd;
        }

        if (hRootWnd) {
            ::ReleaseCapture();
            ::PostMessage(hRootWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
        }
    }
    return handle;
}

void MbWebView::onDraggableRegionsChangedImpl(wkeDraggableRegion* newRegions, int rectCount)
{
    if (!newRegions)
        return;

    if (!m_draggableRegion)
        m_draggableRegion = ::CreateRectRgn(0, 0, 0, 0);
    else
        ::SetRectRgn(m_draggableRegion, 0, 0, 0, 0);

    if (!newRegions)
        return;

    for (int i = 0; i < rectCount; ++i) {
        RECT r = newRegions[i].bounds;
        HRGN region = ::CreateRectRgn(r.left, r.top, r.right, r.bottom);
        ::CombineRgn(m_draggableRegion, m_draggableRegion, region, newRegions[i].draggable ? RGN_OR : RGN_DIFF);
        ::DeleteObject(region);
    }
}

static void WKE_CALL_TYPE onDraggableRegionsChanged(wkeWebView webWindow, void* param, const wkeDraggableRegion* regions, int rectCount)
{
    int64_t id = (int64_t)param;
    MbWebView* self = (MbWebView*)common::LiveIdDetect::get()->getPtr(id);
    if (!self)
        return;

    wkeDraggableRegion* newRegions = nullptr;
    if (regions) {
        newRegions = new wkeDraggableRegion[rectCount];
        memcpy(newRegions, regions, rectCount * sizeof(wkeDraggableRegion));
    }

    common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [id, newRegions, rectCount] {
        MbWebView* self = (MbWebView*)common::LiveIdDetect::get()->getPtr(id);
        if (!self)
            return;
        self->onDraggableRegionsChangedImpl(newRegions, rectCount);
    });
}

void MbWebView::createWkeWebWindowImplInUiThread(HWND parent, DWORD style, DWORD styleEx, int x, int y, int width, int height)
{
    const wchar_t* szClassName = L"MtMbWebWindow";
    MSG msg = { 0 };
    WNDCLASSW wndClass = { 0 };
    static bool isFirstRegister = true;
    if (isFirstRegister) {
        isFirstRegister = false;
        wndClass.style = CS_HREDRAW | CS_VREDRAW;
        wndClass.lpfnWndProc = &MbWebView::windowProc;
        wndClass.cbClsExtra = 200;
        wndClass.cbWndExtra = 200;
        wndClass.hInstance = GetModuleHandleW(NULL);
        wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndClass.hbrBackground = NULL;
        wndClass.lpszMenuName = NULL;
        wndClass.lpszClassName = szClassName;
        RegisterClassW(&wndClass);
    }

    m_hWnd = CreateWindowExW(
        styleEx,        // window ex-style
        szClassName,    // window class name
        kClassWndName, // window caption
        style,         // window style
        x,              // initial x position
        y,              // initial y position
        width,          // initial x size
        height,         // initial y size
        parent,         // parent window handle
        NULL,           // window menu handle
        GetModuleHandleW(NULL),           // program instance handle
        this);         // creation parameters

    if (!IsWindow(m_hWnd))
        return;

    if (m_isShow)
        mbShowWindow(getWebviewHandle(), true);

    m_isWebWindowMode = true; // TODO
}

void MbWebView::createWkeWebWindowInUiThread(mbWindowType type, HWND parent, int x, int y, int width, int height)
{
    if (IsWindow(m_hWnd))
        return;

    DWORD style = 0;
    DWORD styleEx = 0;
    switch (type) {
    case MB_WINDOW_TYPE_CONTROL:
        style = WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        styleEx = 0;
        //wkeSetTransparent(this, false);
        break;

    case MB_WINDOW_TYPE_TRANSPARENT:
        m_isTransparent = true;
        style = WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        styleEx = WS_EX_LAYERED;
        break;

    case MB_WINDOW_TYPE_POPUP:
    default:
        style = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
        styleEx = 0;
        //wkeSetTransparent(this, false);
    }

    createWkeWebWindowImplInUiThread(parent, style, styleEx, x, y, width, height);
}

void MbWebView::setShow(bool b)
{
    m_isShow = b;
    ::ShowWindow(m_hWnd, b ? SW_SHOW : SW_HIDE);
}

void MbWebView::setBlinkSize()
{
    mbWebView webviewHandle = (mbWebView)m_id;
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [](MbWebView* self) {
        SIZE clientSize = self->getClientSizeLocked();
        if (self->m_wkeWebview)
            wkeResize(self->m_wkeWebview, clientSize.cx, clientSize.cy);
        else
            self->setBlinkSize();
    });
}

void MbWebView::setClientSizeLocked(int w, int h)
{
    ::EnterCriticalSection(&m_clientSizeLock);
    m_clientSize.cx = w;
    m_clientSize.cy = h;
    m_clientSizeDirty = true;
    ::LeaveCriticalSection(&m_clientSizeLock);
}

SIZE MbWebView::getClientSizeLocked()
{
    SIZE size = { 0 };
    ::EnterCriticalSection(&m_clientSizeLock);
    size = m_clientSize;
    ::LeaveCriticalSection(&m_clientSizeLock);
    return size;
}

void MbWebView::fillBackgroundColor(HDC hdc, int w, int h)
{
    if (!m_isTransparent) {
        RECT r = { 0, 0, w, w };
        HBRUSH hbr = ::CreateSolidBrush(m_backgroundColor);
        ::FillRect(hdc, &r, hbr);
        ::DeleteObject(hbr);
    } else {
        COLORREF c = m_backgroundColor & 0x00ffffff;
        for (int j = 0; j < h; ++j) {
            for (int i = 0; i < w; ++i) {
                *(m_bits + i + j * (w)) = c;
            }
        }
    }
}

void MbWebView::copyBitmapWhenResize(int w, int h, const SIZE& clientSize)
{
    ::EnterCriticalSection(&m_clientSizeLock);
    if (!m_memoryDC || !m_memoryBMP) {
        ::LeaveCriticalSection(&m_clientSizeLock);
        return;
    }

    ::EnterCriticalSection(&m_memoryCanvasLock);
    //HBITMAP newMemoryBMP = ::CreateCompatibleBitmap(m_memoryDC, w, h);
    HDC newMemoryDC = ::CreateCompatibleDC(m_memoryDC);

    BITMAPINFO bmInfo = { 0 };
    bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmInfo.bmiHeader.biWidth = w;
    bmInfo.bmiHeader.biHeight = -h;  // sets vertical orientation
    bmInfo.bmiHeader.biPlanes = 1;
    bmInfo.bmiHeader.biBitCount = 32;
    bmInfo.bmiHeader.biCompression = BI_RGB;

    HBITMAP newMemoryBMP = ::CreateDIBSection(newMemoryDC, &bmInfo, DIB_RGB_COLORS, (void**)&m_bits, NULL, 0);
    ::SelectObject(newMemoryDC, newMemoryBMP);

    int xoriginDest = 0;
    int yoriginDest = 0;
    int wDest = w;
    int hDest = h;

    int xoriginSrc = 0;
    int yoriginSrc = 0;
    int wSrc = clientSize.cx;
    int hSrc = clientSize.cy;

    fillBackgroundColor(newMemoryDC, w, h);

    BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    ::AlphaBlend(newMemoryDC, xoriginDest, yoriginDest, /*wDest*/wSrc, /*hDest*/hSrc, m_memoryDC, xoriginSrc, yoriginSrc, wSrc, hSrc, bf);

    ::DeleteObject((HGDIOBJ)m_memoryBMP);
    ::DeleteObject((HGDIOBJ)m_memoryDC);

    m_memoryDC = newMemoryDC;
    m_memoryBMP = newMemoryBMP;

    ::LeaveCriticalSection(&m_memoryCanvasLock);
    ::LeaveCriticalSection(&m_clientSizeLock);
}

// in ui thread
void MbWebView::onResize(int w, int h, bool needSetHostWnd)
{
    if (0 >= w * h)
        return;
    SIZE clientSize = getClientSizeLocked();
    if (clientSize.cx == w && clientSize.cy == h)
        return;

    setClientSizeLocked(w, h);
    setBlinkSize();

    copyBitmapWhenResize(w, h, clientSize);

    if (m_isWebWindowMode && needSetHostWnd)
        ::SetWindowPos(m_hWnd, NULL, 0, 0, w, h, SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
}

void readFile(const wchar_t* path, std::vector<char>* buffer)
{
    HANDLE hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (INVALID_HANDLE_VALUE == hFile)
        return;

    DWORD fileSizeHigh;
    const DWORD bufferSize = ::GetFileSize(hFile, &fileSizeHigh);

    DWORD numberOfBytesRead = 0;
    buffer->resize(bufferSize);
    BOOL b = ::ReadFile(hFile, &buffer->at(0), bufferSize, &numberOfBytesRead, nullptr);
    ::CloseHandle(hFile);
}

#if ENABLE_IN_MB_MAIN
#if ENABLE_NODEJS

static void getPreloadScript(const v8::FunctionCallbackInfo<v8::Value>& args)
{
//     std::vector<char> buffer;
//     readFile(L"render_init.js", &buffer);

    v8::Local<v8::String> ret = v8::String::NewFromUtf8(args.GetIsolate(), kRenderInitJs, v8::NewStringType::kNormal, sizeof(kRenderInitJs)).ToLocalChecked();
    args.GetReturnValue().Set(ret);
}

static void isInElectronEnv(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Local<v8::Boolean> ret = v8::Boolean::New(args.GetIsolate(), false);
    args.GetReturnValue().Set(ret);
}

class NodeBindings {
public:
    NodeBindings()
    {
        m_callNextTickAsync = (new uv_async_t());

        uv_async_init(common::ThreadCall::getBlinkLoop(), m_callNextTickAsync, onCallNextTick);
        m_callNextTickAsync->data = this;
    }

    static void onCallNextTick(uv_async_t* handle)
    {
        NodeBindings* self = static_cast<NodeBindings*>(handle->data);

        for (std::list<node::Environment*>::const_iterator it = self->m_pendingNextTicks.begin();
        it != self->m_pendingNextTicks.end(); ++it) {
            node::Environment* env = *it;
            v8::HandleScope handleScope(env->isolate());

            // KickNextTick, copied from node.cc:
            node::Environment::AsyncCallbackScope callbackScope(env);
            if (callbackScope.in_makecallback())
                continue;

            v8::Context::Scope contextScope(env->context());

            node::Environment::TickInfo* tickInfo = env->tick_info();
            if (tickInfo->length() == 0)
                env->isolate()->RunMicrotasks();

            v8::Local<v8::Object> process = env->process_object();
            if (tickInfo->length() == 0)
                tickInfo->set_index(0);

            env->tick_callback_function()->Call(process, 0, nullptr).IsEmpty();
        }

        self->m_pendingNextTicks.clear();
    }

    uv_async_t* m_callNextTickAsync;
    std::list<node::Environment*> m_pendingNextTicks;
};

NodeBindings* m_nodeBindings;
#endif
#endif

#if ENABLE_IN_MB_MAIN
#if ENABLE_NODEJS
void MbWebView::startupNodejsEnv(v8::Local<v8::Context>& v8context)
{

    v8::HandleScope handleScope(v8context->GetIsolate());
    BlinkMicrotaskSuppressionHandle handle = nodeBlinkMicrotaskSuppressionEnter(v8context->GetIsolate());

    //m_nodeBindings = new NodeBindings();

    //const char* argv1[] = { "fast-mb.exe", "E:\\test\\weibo\\render_init.js" };
    const char* argv1[] = { "fast-mb.exe" };
    m_env = node::CreateEnvironment(v8context->GetIsolate(), common::ThreadCall::getBlinkLoop(), v8context, 1, argv1, 0, nullptr);

    m_env->SetMethod(m_env->process_object(), "_isInElectronEnv", isInElectronEnv);
    m_env->SetMethod(m_env->process_object(), "_getPreloadScript", getPreloadScript);
    
//     std::vector<char> buffer;
//     readFile(L"E:\\test\\weibo\\render_init.js", &buffer);

    node::LoadEnvironment(m_env);

    v8::Local<v8::Value> arg = v8::String::NewFromUtf8(v8context->GetIsolate(), "loaded");
    node::MakeCallback(v8context->GetIsolate(), m_env->process_object(), "emit", 1, &arg);

    nodeBlinkMicrotaskSuppressionLeave(handle);
}
#endif
#endif

mbWebFrameHandle MbWebView::toMbFrameHandle(wkeWebView wkeWebview, wkeWebFrameHandle frameId)
{
    if (wkeIsMainFrame(wkeWebview, frameId))
        return (mbWebFrameHandle)-2;
    return (wkeWebFrameHandle)frameId;
}

void MbWebView::onDidCreateScriptContext(wkeWebView webView, void* param, wkeWebFrameHandle frameId, void* context, int extensionGroup, int worldId)
{
    int64_t id = (int64_t)param;
    MbWebView* self = (MbWebView*)common::LiveIdDetect::get()->getPtr(id);
    if (!self)
        return;

    common::BindJsQuery::bindFun(self->m_id, self->getClosure().m_jsQueryClosure, webView, frameId);

    mbWebFrameHandle mbFrameId = toMbFrameHandle(webView, frameId);
    bool isMainFrame = wkeIsMainFrame(webView, frameId);
    if (isMainFrame)
        self->setMainFrameId(frameId);

#if ENABLE_IN_MB_MAIN
#if ENABLE_NODEJS
    //TODO old onDidCreateScriptContext
    v8::Local<v8::Context>& v8context = *(v8::Local<v8::Context>*)context;
    if (self->m_isEnableNode)
        self->startupNodejsEnv(v8context);
#endif
#endif

    void* origParam = self->getClosure().m_DidCreateScriptContextParam;
    mbDidCreateScriptContextCallback callback = self->getClosure().m_DidCreateScriptContextCallback;
    if (callback)        
        callback((mbWebView)id, origParam, (mbWebFrameHandle)frameId, context, extensionGroup, worldId);
}

void MbWebView::onWillReleaseScriptContext(wkeWebView webView, void* param, wkeWebFrameHandle frameId, void* context, int worldId)
{
    int64_t id = (int64_t)param;
    MbWebView* self = (MbWebView*)common::LiveIdDetect::get()->getPtr(id);
    if (!self)
        return;

    void* origParam = self->getClosure().m_WillReleaseScriptContextParam;
    mbWillReleaseScriptContextCallback callback = self->getClosure().m_WillReleaseScriptContextCallback;
    if (callback)
        callback((mbWebView)id, origParam, (mbWebFrameHandle)frameId, context, worldId);
}

void MbWebView::onPaintUpdated(wkeWebView wkeWebview, void* param, const HDC hdc, int x, int y, int cx, int cy)
{
    //OutputDebugStringA("MbWebView::onPaintUpdated InComposite\n");

    int64_t id = (int64_t)param;
    MbWebView* self = (MbWebView*)common::LiveIdDetect::get()->getPtr(id);
    if (!self)
        return;

    self->onPrePaintUpdatedInCompositeThread(hdc, x, y, cx, cy);
}

void MbWebView::setPaintUpdatedCallback(mbPaintUpdatedCallback callback, void* param)
{
    getClosure().setPaintUpdatedCallback(callback, param);
    m_hasSetPaintUpdatedCallback = true;
}

void MbWebView::setPaintBitUpdatedCallback(mbPaintBitUpdatedCallback callback, void* param)
{
    getClosure().setPaintBitUpdatedCallback(callback, param);
    m_hasSetPaintUpdatedCallback = true;
}

void MbWebView::onPaintUpdatedInCompositeThread(const HDC hdc, int x, int y, int cx, int cy)
{
    SIZE clientSize = getClientSizeLocked();
    if (0 == clientSize.cx * clientSize.cy)
        return;

    if (!m_memoryDC)
        m_memoryDC = ::CreateCompatibleDC(nullptr);

    if ((!m_memoryBMP /*|| m_clientSizeDirty*/) ||
        (clientSize.cx * clientSize.cy != 0) && (clientSize.cx == cx && clientSize.cy == cy && 0 == x && 0 == y)) {
        //m_clientSizeDirty = false;

        if (m_memoryBMP)
            ::DeleteObject((HGDIOBJ)m_memoryBMP);

        BITMAPINFO bmInfo = { 0 };
        bmInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmInfo.bmiHeader.biWidth = clientSize.cx;
        bmInfo.bmiHeader.biHeight = -clientSize.cy;  // sets vertical orientation
        bmInfo.bmiHeader.biPlanes = 1;
        bmInfo.bmiHeader.biBitCount = 32;
        bmInfo.bmiHeader.biCompression = BI_RGB;

        m_memoryBMP = ::CreateDIBSection(hdc, &bmInfo, DIB_RGB_COLORS, (void**)&m_bits, NULL, 0);
        ::SelectObject(m_memoryDC, m_memoryBMP);

        fillBackgroundColor(m_memoryDC, clientSize.cx, clientSize.cy);
    } else if (m_isTransparent && m_bits) {
        //memset(m_bits, 0, clientSize.cx * clientSize.cy * 4);
    }

    if (m_memoryDC)
        ::BitBlt(m_memoryDC, x, y, cx, cy, hdc, x, y, SRCCOPY);        
}

static void drawLayeredWindow(HWND hWnd, HDC hdc, HDC hMemoryDC, const POINT& dstPoint)
{
    BITMAP bmp = { 0 };
    HBITMAP hBmp = (HBITMAP)::GetCurrentObject(hMemoryDC, OBJ_BITMAP);
    ::GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp);

    POINT pointSource = { 0, 0 };
    SIZE sizeDest = { 0 };
    sizeDest.cx = bmp.bmWidth;
    sizeDest.cy = bmp.bmHeight;

    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 255;
    blend.AlphaFormat = AC_SRC_ALPHA;

    ::UpdateLayeredWindow(hWnd, hdc, nullptr, &sizeDest, hMemoryDC, &pointSource, RGB(0xFF, 0xFF, 0xFF), &blend, ULW_ALPHA);
}

void MbWebView::onPaintUpdatedInUiThread(int x, int y, int cx, int cy)
{
    SIZE clientSize = getClientSizeLocked();

    ::EnterCriticalSection(&m_memoryCanvasLock);

    if (m_hWnd && m_isAutoDrawToHwnd) {
        HDC hdcScreen = ::GetDC(m_hWnd);
        if (!m_isTransparent) {
            ::BitBlt(hdcScreen, x + m_offset.x, y + m_offset.y, cx, cy, m_memoryDC, x, y, SRCCOPY);
        } else
            drawLayeredWindow(m_hWnd, hdcScreen, m_memoryDC, m_offset);

#if 0
        HBRUSH hbrush = ::CreateSolidBrush(rand());
        ::SelectObject(hdcScreen, hbrush);
        ::Rectangle(hdcScreen, 1220, 40, 366, 266);
        ::DeleteObject(hbrush);
#endif

        ::ReleaseDC(m_hWnd, hdcScreen);
    }

    mbPaintUpdatedCallback paintUpdatedCallback = getClosure().m_PaintUpdatedCallback;
    if (paintUpdatedCallback)
        paintUpdatedCallback(getWebviewHandle(), getClosure().m_PaintUpdatedParam, m_memoryDC, x, y, cx, cy);

    mbPaintBitUpdatedCallback paintBitUpdatedCallback = getClosure().m_PaintBitUpdatedCallback;
    if (paintBitUpdatedCallback) {
        mbRect r = { x, y, cx, cy };
        paintBitUpdatedCallback(getWebviewHandle(), getClosure().m_PaintBitUpdatedParam, m_bits, &r, clientSize.cx, clientSize.cy);
    }

    ::LeaveCriticalSection(&m_memoryCanvasLock);

    ::EnterCriticalSection(&m_clientSizeLock);
    m_clientSizeDirty = false;
    ::LeaveCriticalSection(&m_clientSizeLock);
}

void MbWebView::onPrePaintUpdatedInCompositeThread(const HDC hdc, int x, int y, int cx, int cy)
{
    ::EnterCriticalSection(&m_memoryCanvasLock);
    onPaintUpdatedInCompositeThread(hdc, x, y, cx, cy);
    ::LeaveCriticalSection(&m_memoryCanvasLock);

    int64_t id = m_id;
    common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [id, x, y, cx, cy] {
        MbWebView* self = (MbWebView*)common::LiveIdDetect::get()->getPtr(id);
        if (!self)
            return;
            
        self->onPaintUpdatedInUiThread(x, y, cx, cy);
    });

    if (m_isWebWindowMode) {
        RECT rc = { x, y, x + cx, y + cy };
        ::InvalidateRect(m_hWnd, &rc, false);
    }
}

HDC MbWebView::getViewDC()
{
    ::EnterCriticalSection(&m_memoryCanvasLock);
    return m_memoryDC;
}

void MbWebView::unlockViewDC()
{
    ::LeaveCriticalSection(&m_memoryCanvasLock);
}

void MbWebView::delayDoMouseMsgInBlinkThread()
{
    ::EnterCriticalSection(&m_mouseMsgQueueLock);
    if (0 == m_mouseMsgQueue.size()) {
        ::LeaveCriticalSection(&m_mouseMsgQueueLock);
        return;
    }

    std::list<MouseMsg*> mouseMsgQueue;
    bool hasMouseMove = false;
    MouseMsg* mouseMsg = nullptr;

    // 先找到最左边的鼠标消息。找不到也没事，反正其他消息是必须进入新队列的
    size_t firsrMousePos = 0;
    std::list<MouseMsg*>::iterator itLeft = m_mouseMsgQueue.begin();
    for (; itLeft != m_mouseMsgQueue.end(); ++itLeft, ++firsrMousePos) {
        if (WM_MOUSEMOVE == (*itLeft)->message)
            break;
    }

    // 除了最左边和最右边的鼠标消息，其他鼠标消息都不加入新队列，以节省不必要的鼠标消息所消耗的性能
    std::list<MouseMsg*>::reverse_iterator it = m_mouseMsgQueue.rbegin();
    for (size_t i = m_mouseMsgQueue.size() - 1; it != m_mouseMsgQueue.rend(); ++it, --i) {
        mouseMsg = *it;
        if (WM_MOUSEMOVE != (*it)->message) {
            mouseMsgQueue.push_back(new MouseMsg(mouseMsg->message, mouseMsg->x, mouseMsg->y, mouseMsg->flags));
        } else if (!hasMouseMove || (i == firsrMousePos)) {
            hasMouseMove = true;
            mouseMsgQueue.push_back(new MouseMsg(mouseMsg->message, mouseMsg->x, mouseMsg->y, mouseMsg->flags));
        }
        delete mouseMsg;
    }
    m_mouseMsgQueue.clear();
    ::LeaveCriticalSection(&m_mouseMsgQueueLock);

    for (it = mouseMsgQueue.rbegin(); it != mouseMsgQueue.rend(); ++it) {
        mouseMsg = *it;
        wkeFireMouseEvent(m_wkeWebview, mouseMsg->message, mouseMsg->x /*+ m_offset.x*/, mouseMsg->y /*+ m_offset.y*/, mouseMsg->flags);
        delete mouseMsg;
    }
}

void MbWebView::onMouseMessage(unsigned int message, int x, int y, unsigned int flags)
{
    if (!m_enableMouseKeyMessage)
        return;

    if (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_RBUTTONDOWN) {
        bool isNotInDraggableRegion = !doDraggableRegionNcHitTest(m_hWnd);
        if (::GetFocus() != m_hWnd && isNotInDraggableRegion && g_enableNativeSetFocus)
            ::SetFocus(m_hWnd);
        if (isNotInDraggableRegion && g_enableNativeSetCapture)
            ::SetCapture(m_hWnd);
    } else if (message == WM_LBUTTONUP || message == WM_MBUTTONUP || message == WM_RBUTTONUP) {
        ::ReleaseCapture();
    }

    onCursorChange();

    bool needCommit = true;
    ::EnterCriticalSection(&m_mouseMsgQueueLock);
    needCommit = m_mouseMsgQueue.size() == 0;
    m_mouseMsgQueue.push_back(new MouseMsg(message, x, y, flags));
    ::LeaveCriticalSection(&m_mouseMsgQueueLock);

    if (!needCommit)
        return;
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, getWebviewHandle(), [](MbWebView* self) {
        self->delayDoMouseMsgInBlinkThread();
    });
}

mbRect MbWebView::getCaretRect()
{
    mbRect caretPos;
    ::EnterCriticalSection(&m_clientSizeLock);
    caretPos = m_caretPos;
    ::LeaveCriticalSection(&m_clientSizeLock);
    return caretPos;
}

void MbWebView::onCursorChange()
{
    if (m_isCursorInfoTypeAsynGetting || !m_wkeWebview)
        return;
    m_isCursorInfoTypeAsynGetting = true;

    int64_t id = m_id;
    wkeWebView wkeWebview = m_wkeWebview;
    MbWebView* self = this;

    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [wkeWebview, self, id] {
        if (!common::LiveIdDetect::get()->isLive(id))
            return;

        self->m_isCursorInfoTypeAsynGetting = false;
        
        int cursorType = wkeGetCursorInfoType(wkeWebview);
        if (cursorType == self->m_cursorInfoType)
            return;

        self->m_isCursorInfoTypeAsynChanged = true;
        self->m_cursorInfoType = cursorType;
        //::PostMessage(hWnd, WM_SETCURSOR, (WPARAM)hWnd, MAKELONG(HTCLIENT, 0));
    });
}

bool MbWebView::setCursorInfoTypeByCache()
{
//     if (!m_isCursorInfoTypeAsynChanged)
//         return false;
    m_isCursorInfoTypeAsynChanged = false;

    HCURSOR hCur = NULL;
    switch (m_cursorInfoType) {
    case WkeCursorInfoPointer:
        hCur = ::LoadCursor(NULL, IDC_ARROW);
        break;
    case WkeCursorInfoIBeam:
        hCur = ::LoadCursor(NULL, IDC_IBEAM);
        break;
    case WkeCursorInfoProgress:
        hCur = ::LoadCursor(NULL, IDC_APPSTARTING);
        break;
    case WkeCursorInfoCross:
        hCur = ::LoadCursor(NULL, IDC_CROSS);
        break;
    case WkeCursorInfoMove:
        hCur = ::LoadCursor(NULL, IDC_SIZEALL);
        break;

    case WkeCursorInfoColumnResize:
        hCur = ::LoadCursor(NULL, IDC_SIZEWE);
        break;
    case WkeCursorInfoRowResize:
        hCur = ::LoadCursor(NULL, IDC_SIZENS);
        break;
    case WkeCursorInfoHand:
        hCur = ::LoadCursor(NULL, IDC_HAND);
        break;
    case WkeCursorInfoWait:
        hCur = ::LoadCursor(NULL, IDC_WAIT);
        break;
    case WkeCursorInfoHelp:
        hCur = ::LoadCursor(NULL, IDC_HELP);
        break;
    case WkeCursorInfoEastResize:
        hCur = ::LoadCursor(NULL, IDC_SIZEWE);
        break;
    case WkeCursorInfoNorthResize:
        hCur = ::LoadCursor(NULL, IDC_SIZENS);
        break;
    case WkeCursorInfoSouthWestResize:
    case WkeCursorInfoNorthEastResize:
        hCur = ::LoadCursor(NULL, IDC_SIZENESW);
        break;
    case WkeCursorInfoSouthResize:
    case WkeCursorInfoNorthSouthResize:
        hCur = ::LoadCursor(NULL, IDC_SIZENS);
        break;
    case WkeCursorInfoNorthWestResize:
    case WkeCursorInfoSouthEastResize:
        hCur = ::LoadCursor(NULL, IDC_SIZENWSE);
        break;
    case WkeCursorInfoWestResize:
    case WkeCursorInfoEastWestResize:
        hCur = ::LoadCursor(NULL, IDC_SIZEWE);
        break;
    case WkeCursorInfoNorthEastSouthWestResize:
    case WkeCursorInfoNorthWestSouthEastResize:
        hCur = ::LoadCursor(NULL, IDC_SIZEALL);
        break;
    case WkeCursorInfoNoDrop:
    case WkeCursorInfoNotAllowed:
        hCur = ::LoadCursor(NULL, IDC_NO);
        break;
    }

    if (hCur) {
        ::SetCursor(hCur);
        return true;
    }

    return false;
}

LRESULT CALLBACK MbWebView::windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    MbWebView* self = (MbWebView*)GetPropW(hWnd, kClassWndName);
    if (!self) {
        if (message == WM_CREATE) {
            LPCREATESTRUCTW cs = (LPCREATESTRUCTW)lParam;
            self = (MbWebView*)cs->lpCreateParams;
            ((MbWebView*)cs->lpCreateParams)->setHostWnd(hWnd);
            ::SetPropW(hWnd, kClassWndName, (HANDLE)self);
        }
    }

    if (self)
        return self->windowProcImpl(hWnd, message, wParam, lParam);
    else
        return ::DefWindowProcW(hWnd, message, wParam, lParam);
}

static mbRect joinRect(const mbRect& r1, const mbRect& r2)
{
    int minLeft = r1.x < r2.x ? r1.x : r2.x;
    int maxRight = r1.x + r1.w > r2.x + r2.w ? r1.x + r1.w : r2.x + r2.w;

    int minTop = r1.y < r2.y ? r1.y : r2.y;
    int maxBottom = r1.y + r1.h > r2.y + r2.h ? r1.y + r1.h : r2.y + r2.h;

    return { minLeft, minTop,  maxRight - minLeft, maxBottom - minTop };
}

static int rectArea(const mbRect& r)
{
    return (r.w) * (r.h);
}

static int unionArea(const mbRect& r1, const mbRect& r2)
{
    mbRect c = joinRect(r1, r2);
    return rectArea(c);
}

static void addAndMergeDirty(std::vector<mbRect*>* dirtyRects, const mbRect& r)
{
    size_t dirtySize = dirtyRects->size();
    if (0 == dirtySize) {
        dirtyRects->push_back(new mbRect(r));
        return;
    }

    int bestDelta = 0;
    size_t mergePos = (size_t)-1;
    for (size_t i = 0; i < dirtySize; i++) {
        int delta = unionArea(*(dirtyRects->at(i)), r) - rectArea(*(dirtyRects->at(i))) - rectArea(r);
        if (delta > bestDelta)
            continue;
        bestDelta = delta;
        mergePos = i;
    }

    if (mergePos == (size_t)-1) {
        dirtyRects->push_back(new mbRect(r));
    } else {
        mbRect newR = joinRect(*(dirtyRects->at(mergePos)), r);
        *(dirtyRects->at(mergePos)) = newR;
    }
}

void MbWebView::onBlinkThreadPaint()
{
    ::EnterCriticalSection(&m_dirtyRectLock);
    for (size_t i = 0; i < m_dirtyRect.size(); ++i) {
        mbRect* r = m_dirtyRect[i];
        wkeAddDirtyArea(m_wkeWebview, r->x, r->y, r->w, r->y);
    }
    m_dirtyRect.clear();
    ::LeaveCriticalSection(&m_dirtyRectLock);
    wkeRepaintIfNeeded(m_wkeWebview);

    //////////////////////////////////////////////////////////////////////////
//     static int s_allPaintCount = 0;
//     static int64_t s_paintTime = 0;
//     if (0 == s_paintTime)
//         s_paintTime = common::myGetTime();
//     int64_t time1 = common::myGetTime();
//     s_allPaintCount++;
    //////////////////////////////////////////////////////////////////////////
}

void MbWebView::onPaint(HWND hWnd)
{
    if (WS_EX_LAYERED == (WS_EX_LAYERED & GetWindowLong(hWnd, GWL_EXSTYLE)))
        return;
    
    PAINTSTRUCT ps = { 0 };
    HDC hdc = ::BeginPaint(hWnd, &ps);

    RECT rcClip = ps.rcPaint;

    RECT rcClient;
    ::GetClientRect(hWnd, &rcClient);

    RECT rcInvalid = rcClient;
    if (rcClip.right != rcClip.left && rcClip.bottom != rcClip.top)
        ::IntersectRect(&rcInvalid, &rcClip, &rcClient);

    int srcX = rcInvalid.left - rcClient.left;
    int srcY = rcInvalid.top - rcClient.top;
    int destX = rcInvalid.left;
    int destY = rcInvalid.top;
    int width = rcInvalid.right - rcInvalid.left;
    int height = rcInvalid.bottom - rcInvalid.top;

    if (0 != width && 0 != height) {
        HDC hMbDC = getViewDC();
        if (hMbDC) {
            ::BitBlt(hdc, destX, destY, width, height, hMbDC, srcX, srcY, SRCCOPY);
        } else
            ::FillRect(hdc, &ps.rcPaint, (HBRUSH)::GetStockObject(WHITE_BRUSH));
        unlockViewDC();
    }

    ::EndPaint(hWnd, &ps);

    ::EnterCriticalSection(&m_dirtyRectLock);
    bool needCommit = m_dirtyRect.size() == 0;

    mbRect dirtyRect = { destX, destY, width, height };
    addAndMergeDirty(&m_dirtyRect, dirtyRect);

    int64_t id = m_id;
    MbWebView* self = this;
    if (needCommit) {
        common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [self, id] {
            if (!common::LiveIdDetect::get()->isLive(id))
                return;

            self->onBlinkThreadPaint();
        });
    }
    ::LeaveCriticalSection(&m_dirtyRectLock);
}

LRESULT MbWebView::windowProcImpl(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_NCPAINT:
        break;

    case WM_ERASEBKGND:
        break;
    case WM_GETDLGCODE: // 使得MB控件作为对话框子窗口时可接收到键盘消息
        return DLGC_WANTARROWS | DLGC_WANTALLKEYS | DLGC_WANTCHARS;

    case WM_CREATE:
        ::DragAcceptFiles(hWnd, TRUE);
        ::SetTimer(hWnd, (UINT_PTR)this, 20, NULL);
        break;

    case WM_CLOSE:
        if (getClosure().m_ClosingCallback) {
            if (!getClosure().m_ClosingCallback(getWebviewHandle(), getClosure().m_ClosingParam, nullptr))
                return 0;
        }
        ::ShowWindow(hWnd, SW_HIDE);
        ::DestroyWindow(hWnd);
        return 0;

    case WM_NCDESTROY:
        ::KillTimer(hWnd, (UINT_PTR)this);
        ::RemovePropW(hWnd, kClassWndName);
        m_state = kPageDestroying;
        mbDestroyWebView(getWebviewHandle());
        break;

    case WM_TIMER:
        break;

    case WM_PAINT:
        onPaint(hWnd);
        break;

    case WM_SIZE:
    {
        RECT rc = { 0 };
        ::GetClientRect(hWnd, &rc);
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;

        onResize(width, height, false);
        common::ThreadCall::wake();
        return 0;
    }
    case WM_DROPFILES:
        //         if (wke::g_isSetDragEnable) {
        //             Vector<wchar_t> szFile;
        //             szFile.resize(2 * MAX_PATH);
        //             memset(szFile.data(), 0, sizeof(wchar_t) * 2 * (MAX_PATH));
        // 
        //             wcscpy(szFile.data(), L"file:///");
        // 
        //             HDROP hDrop = reinterpret_cast<HDROP>(wParam);
        // 
        //             UINT uFilesCount = ::DragQueryFileW(hDrop, 0xFFFFFFFF, szFile.data(), MAX_PATH);
        //             if (uFilesCount != 0) {
        //                 UINT uRet = ::DragQueryFileW(hDrop, 0, (wchar_t*)szFile.data() + 8, MAX_PATH);
        //                 if (uRet != 0) {
        //                     wkeLoadURLW(this, szFile.data());
        //                     ::SetWindowTextW(hWnd, szFile.data());
        //                 }
        //             }
        //             ::DragFinish(hDrop);
        //         }
        return 0;

    case WM_SYSKEYDOWN: // no break
    case WM_KEYDOWN:
    {
        unsigned int virtualKeyCode = (unsigned int)wParam;
        unsigned int flags = 0;
        if (HIWORD(lParam) & KF_REPEAT)
            flags |= WKE_REPEAT;
        if (HIWORD(lParam) & KF_EXTENDED)
            flags |= WKE_EXTENDED;

        if (mbFireKeyDownEvent(getWebviewHandle(), virtualKeyCode, flags, false))
            return 0;
        break;
    }
    case WM_SYSKEYUP:
    case WM_KEYUP:
    {
        unsigned int virtualKeyCode = (unsigned int)wParam;
        unsigned int flags = 0;
        if (HIWORD(lParam) & KF_REPEAT)
            flags |= WKE_REPEAT;
        if (HIWORD(lParam) & KF_EXTENDED)
            flags |= WKE_EXTENDED;

        if (mbFireKeyUpEvent(getWebviewHandle(), virtualKeyCode, flags, false))
            return 0;
        break;
    }
    case WM_CHAR:
    case WM_IME_CHAR:
    {
        unsigned int charCode = (unsigned int)wParam;
        unsigned int flags = 0;
        if (HIWORD(lParam) & KF_REPEAT)
            flags |= WKE_REPEAT;
        if (HIWORD(lParam) & KF_EXTENDED)
            flags |= WKE_EXTENDED;

        if (mbFireKeyPressEvent(getWebviewHandle(), charCode, flags, WM_IME_CHAR == message))
            return 0;
        break;
    }
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        unsigned int flags = 0;

        if (wParam & MK_CONTROL)
            flags |= WKE_CONTROL;
        if (wParam & MK_SHIFT)
            flags |= WKE_SHIFT;

        if (wParam & MK_LBUTTON)
            flags |= WKE_LBUTTON;
        if (wParam & MK_MBUTTON)
            flags |= WKE_MBUTTON;
        if (wParam & MK_RBUTTON)
            flags |= WKE_RBUTTON;

        if (mbFireMouseEvent(getWebviewHandle(), message, x, y, flags))
            return 0;
        break;
    }
    case 0x0240/*WM_TOUCH*/:
    {
        LRESULT result = 0;
        if (mbFireWindowsMessage(getWebviewHandle(), hWnd, message, wParam, lParam, &result))
            return 0;
        break;
    }
    case WM_CONTEXTMENU:
    {
        POINT pt;
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);

        if (pt.x != -1 && pt.y != -1)
            ScreenToClient(hWnd, &pt);

        unsigned int flags = 0;

        if (wParam & MK_CONTROL)
            flags |= WKE_CONTROL;
        if (wParam & MK_SHIFT)
            flags |= WKE_SHIFT;

        if (wParam & MK_LBUTTON)
            flags |= WKE_LBUTTON;
        if (wParam & MK_MBUTTON)
            flags |= WKE_MBUTTON;
        if (wParam & MK_RBUTTON)
            flags |= WKE_RBUTTON;

        if (mbFireContextMenuEvent(getWebviewHandle(), pt.x, pt.y, flags))
            return 0;
        break;
    }
    case WM_MOUSEWHEEL:
    {
        POINT pt;
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);
        ::ScreenToClient(hWnd, &pt);

        int delta = GET_WHEEL_DELTA_WPARAM(wParam);

        unsigned int flags = 0;

        if (wParam & MK_CONTROL)
            flags |= WKE_CONTROL;
        if (wParam & MK_SHIFT)
            flags |= WKE_SHIFT;

        if (wParam & MK_LBUTTON)
            flags |= WKE_LBUTTON;
        if (wParam & MK_MBUTTON)
            flags |= WKE_MBUTTON;
        if (wParam & MK_RBUTTON)
            flags |= WKE_RBUTTON;

        if (mbFireMouseWheelEvent(getWebviewHandle(), pt.x, pt.y, delta, flags))
            return 0;
        break;
    }
    case WM_CAPTURECHANGED:
        //OutputDebugStringA("WM_CAPTURECHANGED\n");
        break;
    case WM_SETFOCUS:
        //OutputDebugStringA("WM_SETFOCUS\n");
        mbSetFocus(getWebviewHandle());
        return 0;

    case WM_KILLFOCUS:
        //OutputDebugStringA("WM_KILLFOCUS\n");
        mbKillFocus(getWebviewHandle());
        return 0;

    case WM_SETCURSOR:
        if (mbFireWindowsMessage(getWebviewHandle(), hWnd, WM_SETCURSOR, 0, 0, nullptr))
            return 0;
        break;

    case WM_IME_STARTCOMPOSITION:
        if (mbFireWindowsMessage(getWebviewHandle(), hWnd, WM_IME_STARTCOMPOSITION, 0, 0, nullptr))
            return 0;
    case WM_IME_COMPOSITION:
        if (mbFireWindowsMessage(getWebviewHandle(), hWnd, WM_IME_COMPOSITION, 0, 0, nullptr))
            return 0;
        break;
    }

    return ::DefWindowProcW(hWnd, message, wParam, lParam);
}

void MbWebView::setPacketPathName(const WCHAR* pathName)
{
    m_packetPathName = pathName;
}

static const char kResPacketPrefix[] = "mbpack:///";
static const size_t kResPacketPrefixLen = sizeof(kResPacketPrefix) - 1;

static std::vector<std::wstring> splitUrl(const std::string& text)
{
    std::vector<std::wstring> result;
    size_t pos = 0;
    std::string temp = text;
    while (true) {
        size_t posTemp = pos;
        pos = temp.find('\\', posTemp);
        if (pos == std::string::npos) {
            pos = temp.find('/', posTemp);

            if (pos == std::string::npos) {
                std::wstring tokenW = common::utf8ToUtf16(temp);
                result.push_back(tokenW);
                break;
            }
        }

        std::string token = temp.substr(0, pos);
        std::wstring tokenW = common::utf8ToUtf16(token);

        result.push_back(tokenW);
        temp = temp.substr(pos + 1, std::string::npos);
    }

    return result;
}

bool MbWebView::handleResPacket(const char* url, void* job)
{
    HRESULT hr;
    IStorage* tempStorage = nullptr;
    IStorage* subStorage = nullptr;
    IStream* pStm = nullptr;

    std::string text(url);
    if (text.size() <= kResPacketPrefixLen)
        return false;
    
    if (text.substr(0, kResPacketPrefixLen) != kResPacketPrefix)
        return false;
    text = text.substr(kResPacketPrefixLen, std::string::npos);

    hr = ::StgOpenStorage(m_packetPathName.c_str(), NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, 0, &tempStorage);
    if (!SUCCEEDED(hr))
        return false;

    std::vector<std::wstring> tokens = splitUrl(text);

    std::vector<IStorage*> storages;
    storages.push_back(tempStorage);

    for (size_t i = 0; i < tokens.size(); ++i) {
        std::wstring token = tokens[i];
        if (i == tokens.size() - 1) {
            hr = tempStorage->OpenStream(token.c_str(), NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, 0, &pStm);
            if (!SUCCEEDED(hr))
                break;

            std::vector<char> buffer;
            const size_t bufferSize = 1024 * 1024 * 3;
            buffer.resize(bufferSize);
            ULONG actRead = 0;
            hr = pStm->Read(buffer.data(), bufferSize, &actRead);
            if (!SUCCEEDED(hr))
                break;

            mbNetSetData(job, buffer.data(), actRead);
            break;
        }
        hr = tempStorage->OpenStorage(token.c_str(), NULL, STGM_READ | STGM_SHARE_EXCLUSIVE, NULL, 0, &subStorage);
        if (!SUCCEEDED(hr))
            break;
        storages.push_back(subStorage);
        tempStorage = subStorage;
    }

    for (size_t i = 0; i < storages.size(); ++i) {
        tempStorage = storages[i];

        if (tempStorage) 
            tempStorage->Release();
    }

    if (pStm)
        pStm->Release();

    return true;
}

void MbWebView::setUserKeyValue(const char* key, void* value)
{
    ::EnterCriticalSection(&m_userKeyValuesLock);
    m_userKeyValues[key] = value;
    ::LeaveCriticalSection(&m_userKeyValuesLock);
}

void* MbWebView::getUserKeyValue(const char* key) const
{
    ::EnterCriticalSection(&m_userKeyValuesLock);
    std::map<std::string, void*>::const_iterator it = m_userKeyValues.find(key);
    if (m_userKeyValues.end() == it)
        return nullptr;
    void* ret = it->second;
    ::LeaveCriticalSection(&m_userKeyValuesLock);
    return ret;
}

}