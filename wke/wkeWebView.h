﻿#ifndef WKE_WEB_VIEW_H
#define WKE_WEB_VIEW_H

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
//////////////////////////////////////////////////////////////////////////

//cexer: 必须包含在后面，因为其中的 windows.h 会定义 max、min，导致 WebCore 内部的 max、min 出现错乱。
#include "wke/wkeString.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "net/WebURLLoaderManager.h"
#include <map>

//////////////////////////////////////////////////////////////////////////

namespace content {
 class WebPage;
}

namespace wke {

struct CWebViewHandler {
    wkeTitleChangedCallback titleChangedCallback;
    void* titleChangedCallbackParam;

    wkeURLChangedCallback urlChangedCallback;
    void* urlChangedCallbackParam;

    wkePaintUpdatedCallback paintUpdatedCallback;
    void* paintUpdatedCallbackParam;

    wkeAlertBoxCallback alertBoxCallback;
    void* alertBoxCallbackParam;

    wkeConfirmBoxCallback confirmBoxCallback;
    void* confirmBoxCallbackParam;

    wkePromptBoxCallback promptBoxCallback;
    void* promptBoxCallbackParam;

    wkeNavigationCallback navigationCallback;
    void* navigationCallbackParam;

    wkeCreateViewCallback createViewCallback;
    void* createViewCallbackParam;

    wkeDocumentReadyCallback documentReadyCallback;
    void* documentReadyCallbackParam;

    wkeLoadingFinishCallback loadingFinishCallback;
    void* loadingFinishCallbackParam;

	wkeDownloadCallback downloadCallback;
	void* downloadCallbackParam;

    wkeConsoleCallback consoleCallback;
    void* consoleCallbackParam;

    wkeCallUiThread callUiThreadCallback;
    void* callUiThreadCallbackParam;
    
	wkeLoadUrlBeginCallback loadUrlBeginCallback;
	void* loadUrlBeginCallbackParam;

	wkeLoadUrlEndCallback loadUrlEndCallback;
	void* loadUrlEndCallbackParam;

    wkeDidCreateScriptContextCallback didCreateScriptContextCallback;
    void* didCreateScriptContextCallbackParam;

    wkeWillReleaseScriptContextCallback willReleaseScriptContextCallback;
    void* willReleaseScriptContextCallbackParam;

	bool isWke;//是否是使用的wke接口
};

class CWebView : public IWebView {
public:
    CWebView();
    virtual ~CWebView();

    virtual bool create();
    virtual void destroy() override;

    const utf8* name() const override;
    const wchar_t* nameW() const;

    void setName(const utf8* name) override;
    void setName(const wchar_t* name);

    virtual bool isTransparent() const override;
    virtual void setTransparent(bool transparent) override;

    void loadURL(const utf8* inUrl) override;
    void loadURL(const wchar_t* url) override;
    
    void loadPostURL(const utf8* inUrl,const char * poastData,int nLen);
    void loadPostURL(const wchar_t * inUrl,const char * poastData,int nLen);

    void loadHTML(const utf8* html) override;
    void loadHTML(const wchar_t* html) override;

    void loadFile(const utf8* filename) override;
    void loadFile(const wchar_t* filename) override;

    const utf8* url() const override;

	void setUserAgent(const utf8 * useragent);
    void setUserAgent(const wchar_t * useragent);

    bool isLoading() const;
    bool isLoadingSucceeded() const;
    bool isLoadingFailed() const;
    bool isLoadingCompleted() const;
    virtual bool isDocumentReady() const override;
    void stopLoading();
    void reload();

    const utf8* title() override;
    const wchar_t* titleW() override;
    
    virtual void resize(int w, int h) override;
    int width() const override;
    int height() const override;

    int contentWidth() const;
    int contentHeight() const;

    virtual int contentsWidth() const override;
    virtual int contentsHeight() const override;
    
    void setDirty(bool dirty) override;
    bool isDirty() const override;
    void addDirtyArea(int x, int y, int w, int h) override;

    void layoutIfNeeded() override;
    void paint(void* bits, int pitch) override;
    void paint(void* bits, int bufWid, int bufHei, int xDst, int yDst, int w, int h, int xSrc, int ySrc, bool fKeepAlpha);
	void repaintIfNeeded();
    HDC viewDC();
    HWND windowHandle() const;
	void setHandle(HWND wnd);
	void setHandleOffset(int x, int y);
    bool canGoBack() const override;
    bool goBack() override;
    bool canGoForward() const override;
    bool goForward() override;
    
    void editorSelectAll() override;
    void editorUnSelect() override;
    void editorCopy() override;
    void editorCut() override;
    void editorPaste() override;
    void editorDelete() override;
    void editorUndo() override;
    void editorRedo() override;

    const wchar_t* cookieW();
    const utf8* cookie();

    void setCookieEnabled(bool enable) override;
    bool isCookieEnabled() const override;
    
    void setMediaVolume(float volume) override;
    float mediaVolume() const override;
   
    virtual bool fireMouseEvent(unsigned int message, int x, int y, unsigned int flags) override;
    virtual bool fireContextMenuEvent(int x, int y, unsigned int flags) override;
    virtual bool fireMouseWheelEvent(int x, int y, int delta, unsigned int flags) override;
    virtual bool fireKeyUpEvent(unsigned int virtualKeyCode, unsigned int flags, bool systemKey) override;
    virtual bool fireKeyDownEvent(unsigned int virtualKeyCode, unsigned int flags, bool systemKey) override;
    virtual bool fireKeyPressEvent(unsigned int charCode, unsigned int flags, bool systemKey) override;
    bool fireWindowsMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result);

    virtual void setFocus() override;
    virtual void killFocus() override;
    
    virtual wkeRect getCaret() override;
    wkeRect caretRect() ;
    
    jsValue runJS(const wchar_t* script) override;
    jsValue runJS(const utf8* script) override;
    jsExecState globalExec() override;
    
    void sleep() override;
    void wake() override;
    bool isAwake() const override;

    //virtual void awaken() override;

    void setZoomFactor(float factor) override;
    float zoomFactor() const override;

    void setEditable(bool editable) override;
    
    void onURLChanged(wkeURLChangedCallback callback, void* callbackParam);
    void onTitleChanged(wkeTitleChangedCallback callback, void* callbackParam);
    virtual void onPaintUpdated(wkePaintUpdatedCallback callback, void* callbackParam);

    void onAlertBox(wkeAlertBoxCallback callback, void* callbackParam);
    void onConfirmBox(wkeConfirmBoxCallback callback, void* callbackParam);
    void onPromptBox(wkePromptBoxCallback callback, void* callbackParam);

    void onNavigation(wkeNavigationCallback callback, void* callbackParam);
    void onCreateView(wkeCreateViewCallback callback, void* callbackParam);

    virtual void onLoadingFinish(wkeLoadingFinishCallback callback, void* callbackParam);
    virtual void onDocumentReady(wkeDocumentReadyCallback callback, void* callbackParam);
	virtual void onDownload(wkeDownloadCallback callback, void* callbackParam);
    virtual void onConsole(wkeConsoleCallback callback, void* callbackParam);
    virtual void onCallUiThread(wkeCallUiThread callback, void* callbackParam);
    
    void onLoadUrlBegin(wkeLoadUrlBeginCallback callback, void* callbackParam);
    void onLoadUrlEnd(wkeLoadUrlEndCallback callback, void* callbackParam);

    void onDidCreateScriptContext(wkeDidCreateScriptContextCallback callback, void* callbackParam);
    void onWillReleaseScriptContext(wkeWillReleaseScriptContextCallback callback, void* callbackParam);

    void setClientHandler(const wkeClientHandler* handler) override;
    const wkeClientHandler* getClientHandler() const override;

    content::WebPage* webPage() { return m_webPage; }

    void setUserKayValue(const char* key, void* value);
    void* getUserKayValue(const char* key);

    int getCursorInfoType();

    void setDragFiles(const POINT* clintPos, const POINT* screenPos, wkeString files[], int filesCount);

	void setProxyInfo(const String& host, unsigned long port, net::WebURLLoaderManager::ProxyType type, const String& username, const String& password);

protected:
    HWND m_hWnd;
    void _initHandler();
    void _initPage();
    void _initMemoryDC();

    std::map<std::string, void*> m_userKayValues;

    //按理这些接口应该使用CWebView来实现的，可以把它们想像成一个类，因此设置为友员符合情理。
//     friend class ToolTip;
//     friend class ChromeClient;
//     friend class ContextMenuClient;
//     friend class DrawClient;
//     friend class EditorClient;
//     friend class FrameLoaderClient;
//     friend class InspectorClient;
//     friend class PlatformStrategies;

//     OwnPtr<WebCore::Page> m_page;
//     WebCore::Frame* m_mainFrame;
    wke::CString m_url;  //记录url地址
    wke::CString m_title;
    wke::CString m_cookie;
    wke::CString m_name;
    bool m_transparent;

    int m_width;
    int m_height;

    blink::IntRect m_dirtyArea;

    content::WebPage* m_webPage;

    OwnPtr<HDC> m_hdc;
    OwnPtr<HBITMAP> m_hbitmap;
    //void* m_pixels;

    bool m_awake;
public:
	String m_proxy;
	net::WebURLLoaderManager::ProxyType m_proxyType;
};

};//namespace wke
#endif
#endif//#ifndef WKE_WEB_VIEW_H
