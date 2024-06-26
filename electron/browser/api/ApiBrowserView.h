// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef electron_browser_api_ApiBrowserView_h
#define electron_browser_api_ApiBrowserView_h

#include "node/nodeblink.h"
#include "browser/api/ApiWebContents.h"
#include "browser/api/WindowInterface.h"
#include "browser/api/WindowState.h"
#include "common/api/EventEmitter.h"
#include "gin/wrappable.h"
#include "gin/dictionary.h"
#include "../mbvip/core/mb.h"
#include <vector>
#include <windows.h>

namespace atom {

class WebContents;

class BrowserView 
    : public mate::EventEmitter<BrowserView>
    , public WindowInterface {
public:
    BrowserView(v8::Isolate* isolate, v8::Local<v8::Object> wrapper);
    ~BrowserView();

    static void init(v8::Isolate* isolate, v8::Local<v8::Object> target);

    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args);

    void destroyed();

    v8::Local<v8::Value> _getWebContentsApi();
    void _setBoundsApi(int x, int y, int w, int h);

    static BrowserView* newBrowserView(const gin::Dictionary* options, v8::Local<v8::Object> wrapper);

    void attachBrowserWindow(HWND hWnd);
    void detachBrowserWindow();
    void handleMouseMsgInUiThread(unsigned int message, int xInParent, int yInParent, unsigned int flags);

    // WindowInterface
    bool isClosed() override;
    void close() override;
    v8::Local<v8::Object> getWrapper() override;
    int getId() const override;
    WebContents* getWebContents() const override;
    HWND getHWND() const override;

    RECT getClientRect() const
    {
        RECT r;
        ::EnterCriticalSection(&m_rectLock);
        r = m_clientRect;
        ::LeaveCriticalSection(&m_rectLock);
        return r;
    }

    void setClientRect(const RECT& r)
    {
        ::EnterCriticalSection(&m_rectLock);
        m_clientRect = r;
        ::LeaveCriticalSection(&m_rectLock);
    }

    mbWebView getMbView() const
    {
        if (!m_webContents)
            return NULL_WEBVIEW;
        return m_webContents->getMbView();
    }

    void onPaintInUiThread(const HDC hdc, int destX, int destY, int x, int y, int cx, int cy);

private:
    static void staticOnBlinkPaintUpdatedInUiThread(mbWebView webView, BrowserView* self, const HDC hdc, int x, int y, int cx, int cy);
    void onBlinkPaintUpdatedInUiThread(const HDC hdc, int x, int y, int cx, int cy);

public:
    WebContents* m_webContents;
    WebContents::CreateWindowParam* m_createWindowParam;
    HWND m_hWnd;
    WindowState m_state;
    int m_id;

    mutable CRITICAL_SECTION m_memoryCanvasLock;
    mutable CRITICAL_SECTION m_rectLock;
    HBITMAP m_memoryBMP;
    HDC m_memoryDC;
    RECT m_clientRect;
    SIZE m_memoryBmpSize;
    v8::Persistent<v8::Object> m_liveSelf;

    static gin::WrapperInfo kWrapperInfo;
    static v8::Persistent<v8::Function> constructor;
};

} // atom namespace

#endif // electron_browser_api_ApiBrowserView_h
