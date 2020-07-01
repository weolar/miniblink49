#ifndef WKE_WEB_WINDOW_H
#define WKE_WEB_WINDOW_H

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)

#include "wkeWebView.h"

//////////////////////////////////////////////////////////////////////////

namespace wke {

class CWebWindow : public CWebView {
public:
    CWebWindow(COLORREF c);
    ~CWebWindow();

    bool createWindow(const wkeWindowCreateInfo* info);
    bool createWindow(HWND parent, wkeWindowType type, int x, int y, int width, int height);
    
    virtual void destroy() override;

    void show(bool b);
    void enable(bool b);
    void move(int x, int y, int width, int height);
    void moveToCenter();
    virtual void resize(int width, int height) override;

    void setTitle(const wchar_t* text);
    void setTitle(const utf8* text);

    void onClosing(wkeWindowClosingCallback callback, void* param);
    void onDestroy(wkeWindowDestroyCallback callback, void* param);
    
    virtual void setTransparent(bool transparent) override;

protected:
    virtual void onPaintUpdated(wkePaintUpdatedCallback callback, void* callbackParam) override;
    virtual void onLoadingFinish(wkeLoadingFinishCallback callback, void* callbackParam) override;
    virtual void onDocumentReady(wkeDocumentReadyCallback callback, void* callbackParam) override;

    bool _createWindow(const wkeWindowCreateInfo* info);
    void _destroyWindow();
    void _initCallbacks();

    static LRESULT CALLBACK _staticWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    LRESULT _windowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

    static void WKE_CALL_TYPE _staticOnPaintUpdated(wkeWebView webView, void* param, const HDC hdc, int x, int y, int cx, int cy);
    void _onPaintUpdated(const HDC hdc, int x, int y, int cx, int cy);

    static void WKE_CALL_TYPE _staticOnLoadingFinish(wkeWebView webView, void* param, const wkeString url, wkeLoadingResult result, const wkeString failedReason);
    void _onLoadingFinish(const wkeString url, wkeLoadingResult result, const wkeString failedReason);

    static void WKE_CALL_TYPE _staticOnDocumentReady(wkeWebView webView, void* param);
    void _onDocumentReady();

    bool m_acceptDrags;

    wkePaintUpdatedCallback m_originalPaintUpdatedCallback;
    void* m_originalPaintUpdatedCallbackParam;

    wkeDocumentReadyCallback m_originalDocumentReadyCallback;
    void* m_originalDocumentReadyCallbackParam;

    wkeLoadingFinishCallback m_originalLoadingFinishCallback;
    void* m_originalLoadingFinishCallbackParam;

//     wkeWindowClosingCallback m_windowClosingCallback;
//     void* m_windowClosingCallbackParam;
// 
//     wkeWindowDestroyCallback m_windowDestroyCallback;
//     void* m_windowDestroyCallbackParam;

    enum State {
        kWkeWebWindowUninit,
        kWkeWebWindowInit,
        kWkeWebWindowDestroing,
        kWkeWebWindowDestroyed,
    };
    State m_state;
};

};//namespace wke

#endif
#endif // #ifndef WKE_WEB_WINDOW_H