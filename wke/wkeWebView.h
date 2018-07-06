#ifndef WKE_WEB_VIEW_H
#define WKE_WEB_VIEW_H

#if (defined ENABLE_WKE) && (ENABLE_WKE == 1)
//////////////////////////////////////////////////////////////////////////

//cexer: 必须包含在后面，因为其中的 windows.h 会定义 max、min，导致 WebCore 内部的 max、min 出现错乱。
#include "wke/wkeString.h"
#include "wke/wkeJsBindFreeTempObject.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"
#include "net/WebURLLoaderManager.h"
#include <map>
#include <set>

//////////////////////////////////////////////////////////////////////////

namespace content {
 class WebPage;
}

namespace wke {

struct CWebViewHandler {
    wkeTitleChangedCallback titleChangedCallback;
    void* titleChangedCallbackParam;

    wkeTitleChangedCallback mouseOverUrlChangedCallback;
    void* mouseOverUrlChangedCallbackParam;

    wkeURLChangedCallback urlChangedCallback;
    void* urlChangedCallbackParam;

    wkeURLChangedCallback2 urlChangedCallback2;
    void* urlChangedCallback2Param;

    wkePaintUpdatedCallback paintUpdatedCallback;
    void* paintUpdatedCallbackParam;

    wkePaintBitUpdatedCallback paintBitUpdatedCallback;
    void* paintBitUpdatedCallbackParam;

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

    wkeDocumentReady2Callback documentReady2Callback;
    void* documentReady2CallbackParam;

    wkeLoadingFinishCallback loadingFinishCallback;
    void* loadingFinishCallbackParam;

    wkeDownloadCallback downloadCallback;
    void* downloadCallbackParam;

    wkeNetResponseCallback netResponseCallback;
    void* netResponseCallbackParam;

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

    wkeOnOtherLoadCallback otherLoadCallback;
    void* otherLoadCallbackParam;

    wkeWindowClosingCallback windowClosingCallback;
    void* windowClosingCallbackParam;
    
    wkeWindowDestroyCallback windowDestroyCallback;
    void* windowDestroyCallbackParam;

    wkeDraggableRegionsChangedCallback draggableRegionsChangedCallback;
    void* draggableRegionsChangedCallbackParam;

    wkeStartDraggingCallback startDraggingCallback;
    void* startDraggingCallbackParam;
    
    bool isWke; // 是否是使用的wke接口
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
    void loadHtmlWithBaseUrl(const utf8* html, const utf8* baseUrl);
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
    void goToOffset(int offset);
    void goToIndex(int index);

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
    void setViewSettings(const wkeViewSettings*);
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
    wkeRect caretRect();

    static int64_t wkeWebFrameHandleToFrameId(content::WebPage* page, wkeWebFrameHandle frameId);
    static wkeWebFrameHandle frameIdTowkeWebFrameHandle(content::WebPage* page, int64_t frameId);

    jsValue runJS(const wchar_t* script) override;
    jsValue runJS(const utf8* script) override;
    jsValue runJsInFrame(wkeWebFrameHandle frameId, const utf8* script, bool isInClosure);
    jsExecState globalExec() override;
    jsExecState globalExecByFrame(wkeWebFrameHandle frameId);
    
    void sleep() override;
    void wake() override;
    bool isAwake() const override;

    //virtual void awaken() override;

    void setZoomFactor(float factor) override;
    float zoomFactor() const override;

    void setEditable(bool editable) override;
    
    void onURLChanged(wkeURLChangedCallback callback, void* callbackParam);
    void onURLChanged2(wkeURLChangedCallback2 callback, void* callbackParam);
    void onUrlChanged(const wkeString url);
    void onTitleChanged(wkeTitleChangedCallback callback, void* callbackParam);
    void onMouseOverUrlChanged(wkeTitleChangedCallback callback, void* callbackParam);
    virtual void onPaintUpdated(wkePaintUpdatedCallback callback, void* callbackParam);
    void onPaintBitUpdated(wkePaintBitUpdatedCallback callback, void* callbackParam);

    void onAlertBox(wkeAlertBoxCallback callback, void* callbackParam);
    void onConfirmBox(wkeConfirmBoxCallback callback, void* callbackParam);
    void onPromptBox(wkePromptBoxCallback callback, void* callbackParam);

    void onNavigation(wkeNavigationCallback callback, void* callbackParam);
    void onCreateView(wkeCreateViewCallback callback, void* callbackParam);

    virtual void onLoadingFinish(wkeLoadingFinishCallback callback, void* callbackParam);
    virtual void onDocumentReady(wkeDocumentReadyCallback callback, void* callbackParam);
    void onDocumentReady2(wkeDocumentReady2Callback callback, void* callbackParam);
    virtual void onDownload(wkeDownloadCallback callback, void* callbackParam);
    virtual void onConsole(wkeConsoleCallback callback, void* callbackParam);
    virtual void onCallUiThread(wkeCallUiThread callback, void* callbackParam);
    void onNetResponse(wkeNetResponseCallback callback, void* callbackParam);
    
    void onLoadUrlBegin(wkeLoadUrlBeginCallback callback, void* callbackParam);
    void onLoadUrlEnd(wkeLoadUrlEndCallback callback, void* callbackParam);

    void onDidCreateScriptContext(wkeDidCreateScriptContextCallback callback, void* callbackParam);
    void onWillReleaseScriptContext(wkeWillReleaseScriptContextCallback callback, void* callbackParam);

    void onStartDragging(wkeStartDraggingCallback callback, void* callbackParam);
    
    void onOtherLoad(wkeOnOtherLoadCallback callback, void* callbackParam);

    void onDraggableRegionsChanged(wkeDraggableRegionsChangedCallback callback, void* param);

    void setClientHandler(const wkeClientHandler* handler) override;
    const wkeClientHandler* getClientHandler() const override;

    CWebViewHandler* getWkeHandler() const;

    content::WebPage* webPage() { return m_webPage; }

    void setUserKeyValue(const char* key, void* value);
    void* getUserKeyValue(const char* key);

    int getCursorInfoType();

    void setDragFiles(const POINT* clintPos, const POINT* screenPos, wkeString files[], int filesCount);

    void setNetInterface(const char* netInterface);
    String getNetInterface() const { return m_netInterface; }

    void setProxyInfo(const String& host, unsigned long port, net::ProxyType type, const String& username, const String& password);
    String getProxy() const { return m_proxy; }
    net::ProxyType getProxyType() const { return m_proxyType; }

    void showDevTools(const utf8* url, wkeOnShowDevtoolsCallback callback, void* param);

    content::WebPage* getWebPage() const { return m_webPage; }

    std::set<jsValue>& getPersistentJsValue() { return m_persistentJsValue; }

protected:
    friend class ShowDevToolsTaskObserver;

    HWND m_hWnd;
    void _initHandler();
    void _initPage();
    void _initMemoryDC();

    void _loadURL(const utf8* inUrl, bool isFile);

    std::map<std::string, void*> m_userKeyValues;
    std::set<jsValue> m_persistentJsValue;

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
    bool m_isCokieEnabled;

    int m_width;
    int m_height;

    blink::IntRect m_dirtyArea;

    content::WebPage* m_webPage;

    float m_zoomFactor;

    OwnPtr<HDC> m_hdc;
    OwnPtr<HBITMAP> m_hbitmap;
    //void* m_pixels;

    bool m_awake;

    String m_netInterface;

    String m_proxy;
    net::ProxyType m_proxyType;

    friend class ShowDevToolsTaskObserver;
    bool m_isCreatedDevTools;
    wkeWebView m_devToolsWebView;

    wkeViewSettings m_settings;
};

};//namespace wke
#endif
#endif//#ifndef WKE_WEB_VIEW_H
