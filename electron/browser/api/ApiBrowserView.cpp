// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "browser/api/ApiBrowserView.h"

#include "node/nodeblink.h"
#include "common/NodeRegisterHelp.h"
#include "common/api/EventEmitter.h"
#include "common/OptionsSwitches.h"
#include "common/IdLiveDetect.h"
#include "common/ThreadCall.h"
#include "gin/object_template_builder.h"
#include "gin/wrappable.h"
#include "gin/dictionary.h"
#include "node/src/node.h"
#include "node/src/env.h"
#include "node/src/env-inl.h"

#include <vector>

namespace atom {

BrowserView::BrowserView(v8::Isolate* isolate, v8::Local<v8::Object> wrapper)
{
    gin::Wrappable<BrowserView>::InitWith(isolate, wrapper);
    m_state = WindowUninited;
    m_webContents = nullptr;
    m_createWindowParam = nullptr;
    m_hWnd = nullptr;
    m_memoryBMP = nullptr;
    m_memoryDC = nullptr;
    m_clientRect.left = 0;
    m_clientRect.top = 0;
    m_clientRect.right = 0;
    m_clientRect.bottom = 0;
    m_memoryBmpSize.cx = 0;
    m_memoryBmpSize.cy = 0;
    m_id = IdLiveDetect::get()->constructed(this);
    
    ::InitializeCriticalSection(&m_rectLock);
    ::InitializeCriticalSection(&m_memoryCanvasLock);
}

BrowserView::~BrowserView()
{
    OutputDebugStringA("BrowserView::~BrowserView\n");
}

void BrowserView::init(v8::Isolate* isolate, v8::Local<v8::Object> target)
{
    const char* className = "BrowserView";
    v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, newFunction);

    prototype->SetClassName(v8::String::NewFromUtf8(isolate, className));
    gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
    builder.SetMethod("_getWebContents", &BrowserView::_getWebContentsApi);
    builder.SetMethod("_setBounds", &BrowserView::_setBoundsApi);

    constructor.Reset(isolate, prototype->GetFunction());
    target->Set(v8::String::NewFromUtf8(isolate, className), prototype->GetFunction());
}

void BrowserView::newFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    if (!args.IsConstructCall())
        return;
    
    gin::Dictionary* options = nullptr;
    if (0 != args.Length())
        options = new gin::Dictionary(isolate, args[0]->ToObject());
    else
        options = new gin::Dictionary(gin::Dictionary::CreateEmpty(isolate));

    BrowserView* self = newBrowserView(options, args.This());
    args.GetReturnValue().Set(args.This());
    delete options;
}

void BrowserView::destroyed()
{
    m_webContents->destroyed();
}

v8::Local<v8::Value> BrowserView::_getWebContentsApi()
{
    if (!m_webContents)
        return v8::Null(isolate());

    return v8::Local<v8::Value>::New(isolate(), m_webContents->getWrapper());
}

void BrowserView::_setBoundsApi(int x, int y, int w, int h)
{
    RECT r = {x, y, x + w, y + h};
    setClientRect(r);

    mbResize(m_webContents->getMbView(), w, h);
}

BrowserView* BrowserView::newBrowserView(const gin::Dictionary* options, v8::Local<v8::Object> wrapper)
{
    BrowserView* self = new BrowserView(options->isolate(), wrapper);
    WebContents::CreateWindowParam* createWindowParam = new WebContents::CreateWindowParam();
    createWindowParam->minWidth = 1;
    createWindowParam->minHeight = 1;
    createWindowParam->styles = 0;
    createWindowParam->styleEx = 0;
    createWindowParam->transparent = false;

    WebContents* webContents = nullptr;
    v8::Handle<v8::Object> webContentsV8;
    // If no WebContents was passed to the constructor, create it from options.
    if (!options->Get("webContents", &webContentsV8)) {
        // Use options.webPreferences to create WebContents.
        gin::Dictionary webPreferences = gin::Dictionary::CreateEmpty(options->isolate());
        bool b = options->Get(options::kWebPreferences, &webPreferences);

        v8::Local<v8::Value> preloadScriptValue;
        if (options->Get(options::kPreloadScript, &preloadScriptValue))
            webPreferences.Set(options::kPreloadScript, preloadScriptValue);

        // Copy the backgroundColor to webContents.
        v8::Local<v8::Value> backgroundColorValue;
        if (options->Get(options::kBackgroundColor, &backgroundColorValue))
            webPreferences.Set(options::kBackgroundColor, backgroundColorValue);

        webContents = WebContents::create(options->isolate(), webPreferences, self);

        //webPreferences.GetBydefaultVal("nodeIntegration", true, &webContents->m_isNodeIntegration);
    } else
        DebugBreak();
    self->m_webContents = webContents;

    options->GetBydefaultVal("x", kNotSetXYFlag, &createWindowParam->x);
    options->GetBydefaultVal("y", kNotSetXYFlag, &createWindowParam->y);
    options->GetBydefaultVal("width", 1, &createWindowParam->width);
    options->GetBydefaultVal("height", 1, &createWindowParam->height);

    if (createWindowParam->width < createWindowParam->minWidth)
        createWindowParam->width = createWindowParam->minWidth;

    if (createWindowParam->height < createWindowParam->minHeight)
        createWindowParam->height = createWindowParam->minHeight;

    mbWebView webview = webContents->getMbView();
    if (createWindowParam->transparent)
        mbSetTransparent(webview, true);
    mbResize(webview, createWindowParam->width, createWindowParam->height);
    mbOnPaintUpdated(webview, (mbPaintUpdatedCallback)staticOnBlinkPaintUpdatedInUiThread, self);
    // wkeOnConsole(webview, onConsoleCallback, nullptr);
    // wkeOnDocumentReady(webview, onDocumentReadyInBlinkThread, self);
    // wkeOnLoadUrlBegin(webview, handleLoadUrlBegin, self);
    // wkeOnTitleChanged(webview, onTitleChangedInBlinkThread, self);
    // wkeOnURLChanged2(webview, onURLChangedCallback, self);
    // wkeOnLoadingFinish(webview, onLoadingFinishCallback, self);
    // wkeOnOtherLoad(webview, onOtherLoadCallback, self);
    // wkeOnDraggableRegionsChanged(webview, onDraggableRegionsChanged, self);
    // wkeOnStartDragging(webview, onStartDraggingCallback, self);
    // wkeSetFocus(webview);
    // wkeSetDebugConfig(webview, "decodeUrlRequest", nullptr);
    // wkeSetDragDropEnable(webview, true);
    mbSetNavigationToNewWindowEnable(webview, true);
    //wkeOnCreateView(webview, PopupWindow::onCreateViewCallbackStatic, nullptr);

    self->m_createWindowParam = createWindowParam;
    webContents->setCreateWindowParam(createWindowParam);
    self->m_liveSelf.Reset(options->isolate(), wrapper);

    return self;
}

void BrowserView::attachBrowserWindow(HWND hWnd)
{
    m_hWnd = hWnd;

    int x = m_createWindowParam->x;
    int y = m_createWindowParam->y;
    int w = m_createWindowParam->width;
    int h = m_createWindowParam->height;
    RECT r = getClientRect();

    if (r.right - r.left != 0) {
        x = r.left;
        y = r.top;
        w = r.right - r.left;
        h = r.bottom - r.top;
    }

    mbWebView webview = m_webContents->getMbView();
    //self->m_webContents->onNewWindowInUiThread(x, y, w, h, self->m_createWindowParam);

    //matchDpi(webview);
    mbSetHandle(webview, hWnd);

    //::ShowWindow(m_hWnd, createWindowParam->isShow ? SW_SHOWNORMAL : SW_HIDE);
    m_state = WindowInited;
}

void BrowserView::detachBrowserWindow()
{
    m_state = WindowDestroying;
}

void BrowserView::onPaintInUiThread(const HDC hdc, int destX, int destY, int x, int y, int cx, int cy)
{
    if (!m_memoryDC)
        return;
    ::EnterCriticalSection(&m_memoryCanvasLock);
    DWORD flag = SRCCOPY;
    ::BitBlt(hdc, destX, destY, cx, cy, m_memoryDC, x, y, flag);
    ::LeaveCriticalSection(&m_memoryCanvasLock);

//     char* output = (char*)malloc(0x100);
//     sprintf_s(output, 0x99, "BrowserView::onPaintInUiThread: %d, %d, (%d, %d, %d, %d)\n", destX, destY, x, y, cx, cy);
//     OutputDebugStringA(output);
//     free(output);
}

void BrowserView::staticOnBlinkPaintUpdatedInUiThread(mbWebView webView, BrowserView* self, const HDC hdc, int x, int y, int cx, int cy)
{
    self->onBlinkPaintUpdatedInUiThread(hdc, x, y, cx, cy);
}

void BrowserView::onBlinkPaintUpdatedInUiThread(const HDC hdc, int x, int y, int cx, int cy)
{
    ::EnterCriticalSection(&m_memoryCanvasLock);
    HWND hWnd = m_hWnd;
    RECT r = getClientRect();
    SIZE sizeDest = { r.right - r.left, r.bottom - r.top };

    HDC hSreenDC = ::GetWindowDC(hWnd);
    if (!m_memoryDC)
        m_memoryDC = ::CreateCompatibleDC(hSreenDC);

    if (!m_memoryBMP /*|| !isRectEqual(m_clientRect, rectDest)*/) {
        //m_clientRect = rectDest;
        m_memoryBmpSize = sizeDest;

        if (m_memoryBMP)
            ::DeleteObject((HGDIOBJ)m_memoryBMP);
        m_memoryBMP = ::CreateCompatibleBitmap(hSreenDC, sizeDest.cx, sizeDest.cy);
    }
    ::ReleaseDC(hWnd, hSreenDC);

    DWORD flag = SRCCOPY;
    if (m_createWindowParam->transparent)
        flag |= CAPTUREBLT;

//     char* output = (char*)malloc(0x100);
//     sprintf_s(output, 0x99, "BrowserView::onBlinkPaintUpdatedInUiThread: %d, %d, %d, %d\n", x, y, cx, cy);
//     OutputDebugStringA(output);
//     free(output);

    BOOL b = FALSE;
    HBITMAP hbmpOld = (HBITMAP)::SelectObject(m_memoryDC, m_memoryBMP);
    ::BitBlt(m_memoryDC, x, y, cx, cy, hdc, x, y, flag);

    ::LeaveCriticalSection(&m_memoryCanvasLock);

    if (m_createWindowParam->transparent) {
        //         ThreadCall::callUiThreadAsync([id, self, x, y, cx, cy] {
        //             if (IdLiveDetect::get()->isLive(id))
        //                 self->onPaintUpdatedInUiThread(x, y, cx, cy);
        //             });
    } else {
        RECT r = getClientRect();
        int rcX = r.left + x;
        int rcY = r.top + y;
        RECT rc = { rcX, rcY, rcX + cx, rcY + cy };
        ::InvalidateRect(m_hWnd, &rc, false);
    }
}

void BrowserView::handleMouseMsgInUiThread(unsigned int message, int xInParent, int yInParent, unsigned int flags)
{
    RECT r = getClientRect();
    int xInView = xInParent - r.left;
    int yInView = yInParent - r.top;

    mbWebView webview = m_webContents->getMbView();
    mbFireMouseEvent(webview, message, xInView, yInView, flags);
}

bool BrowserView::isClosed()
{
    return m_state == WindowDestroyed;
}

void BrowserView::close()
{

}

v8::Local<v8::Object> BrowserView::getWrapper()
{
    return GetWrapper(isolate());
}

int BrowserView::getId() const
{
    return m_id;
}

WebContents* BrowserView::getWebContents() const
{
    return m_webContents;
}

HWND BrowserView::getHWND() const
{
    return m_hWnd;
}

gin::WrapperInfo BrowserView::kWrapperInfo = { gin::kEmbedderNativeGin };
v8::Persistent<v8::Function> BrowserView::constructor;

void initializeBrowseviewApi(v8::Local<v8::Object> exports, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, void* priv)
{
    node::Environment* env = node::Environment::GetCurrent(context);
    BrowserView::init(env->isolate(), exports);
}

static const char nativeBrowserViewNativeScript[] = "console.log('nativeBrowserViewNative');;";
static NodeNative nativeBrowserViewNative{ "BrowserView", nativeBrowserViewNativeScript, sizeof(nativeBrowserViewNativeScript) - 1 };
NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_browserview, atom::initializeBrowseviewApi, &nativeBrowserViewNative)

} // atom namespace

