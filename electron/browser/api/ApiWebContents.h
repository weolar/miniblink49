
#ifndef browser_api_ApiWebContents_h
#define browser_api_ApiWebContents_h

#include "node/nodeblink.h"
#include "browser/api/WindowState.h"
#include "common/api/EventEmitter.h"
#include "node/src/env.h"
#include "../mbvip/core/mb.h"
#include "gin/dictionary.h"
#include <set>

namespace node {
class Environment;
}

namespace base {
class ListValue;
}

namespace atom {

static const int kNotSetXYFlag = 400/*-8467*/;

class NodeBindings;
class WebContents;
class WindowInterface;

inline static bool isRectEqual(const RECT& a, const RECT& b)
{
    return (a.left == b.left) && (a.top == b.top) && (a.right == b.right) && (a.bottom == b.bottom);
}

inline static bool isPointInRect(const RECT& a, const POINT& b)
{
    return b.x >= a.left && b.x <= a.right
        && b.y >= a.top && b.y <= a.bottom;
}

class WebContentsObserver {
public:
    virtual void onWebContentsCreated(WebContents* contents) { }
    virtual void onWebContentsDeleted(WebContents* contents) { }
    virtual void onWebContentsReadyToShow(WebContents* contents) { }
};

class WebContents : public mate::EventEmitter<WebContents> {
public:
    struct CreateWindowParam {
        int x;
        int y;
        int width;
        int height;
        unsigned styles;
        unsigned styleEx;
        bool transparent;
        std::wstring title;
        bool isShow;
        bool isCenter;
        bool isResizable;
        bool isMinimizable;
        bool isMaximizable;
        bool isFrame;
        bool isMovable;

        bool isUseContentSize;
        bool isAlwaysOnTop;
        bool isClosable;

        int minWidth;
        int minHeight;
        int maxWidth;
        int maxHeight;

        CreateWindowParam()
        {
            x = 0;
            y = 0;
            width = 0;
            height = 0;
            styles = 0;
            styleEx = 0;
            transparent = false;

            isShow = true;
            isCenter = false;
            isResizable = true;
            isMinimizable = true;
            isMaximizable = true;
            isFrame = true;
            isMovable = true;

            isUseContentSize = false;
            isAlwaysOnTop = false;
            isClosable = true;

            minWidth = 100;
            minHeight = 100;
            maxWidth = 500;
            maxHeight = 500;
        }
    };

    static void init(v8::Isolate* isolate, v8::Local<v8::Object> target, node::Environment* env);
    static WebContents* create(v8::Isolate* isolate, gin::Dictionary options, WindowInterface* owner);

    explicit WebContents(v8::Isolate* isolate, v8::Local<v8::Object> wrapper, const gin::Dictionary& options);
    ~WebContents();

    void destroyed();
    void addObserver(WebContentsObserver* observer);
    void removeObserver(WebContentsObserver* observer);

    mbWebView getMbView() const { return m_view; }
    WindowInterface* getOwner() const { return m_owner; }

    //void onNewWindowInUiThread(int x, int y, int width, int height, const CreateWindowParam* createWindowParam);

    void rendererPostMessageToMain(const std::string& channel, const base::ListValue& listParams);
    void rendererSendMessageToMain(const std::string& channel, const base::ListValue& listParams, std::string* jsonRet);
    void anyPostMessageToRenderer(const std::string& channel, const base::ListValue& listParams);
    static void rendererSendMessageToRenderer(mbWebView view, mbWebFrameHandle frame, const std::string& channel, const base::ListValue& args);

    int getIdApi() const;
    static WebContents* fromId(int id);

private:
    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args);
    void getSessionApi(const v8::FunctionCallbackInfo<v8::Value>& info) const;
    void zoomFactorApi(const v8::FunctionCallbackInfo<v8::Value>& info) const;
    bool canGoBackApi() const;
    bool canGoForwardApi() const;
    void setZoomLevelApi(float level);
    float getZoomLevelApi() const;
    void printToPDFApi();

    void _loadURLApi(const std::string& url);
    int getProcessIdApi() const;
    bool equalApi() const;

    static void getFocusedWebContentsApi(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void getAllWebContentsApi(const v8::FunctionCallbackInfo<v8::Value>& info);
    static void fromIdApi(const v8::FunctionCallbackInfo<v8::Value>& info);
    
    std::string _getURLApi();

    std::string getTitleApi();

    bool isLoadingApi();
    bool isLoadingMainFrameApi();
    bool isWaitingForResponseApi();
    void stopApi();
    void goBackApi();
    void goForwardApi();
    void goToOffsetApi(int offset);
    void goToIndexApi(int index);
    bool isCrashedApi();
    void setUserAgentApi(const std::string userAgent);
    std::string getUserAgentApi();
    void insertCSSApi(const std::string& cssText);
    void savePageApi();
    void openDevToolsApi();
    void closeDevToolsApi();
    bool isDevToolsOpenedApi();
    bool isDevToolsFocusedApi();
    void enableDeviceEmulationApi();
    void disableDeviceEmulationApi();
    void toggleDevToolsApi();
    void inspectElementApi();
    void setAudioMutedApi();
    void isAudioMutedApi();
    void undoApi();
    void redoApi();
    void cutApi();
    void copyApi();
    void pasteApi();
    void pasteAndMatchStyleApi();
    void _deleteApi();
    void selectAllApi();
    void unselectApi();
    void replaceApi();
    void replaceMisspellingApi();
    void findInPageApi();
    void stopFindInPageApi();
    void focusApi();
    bool isFocusedApi();
    void tabTraverseApi();
    bool _sendApi(bool isAllFrames, const std::string& channel, const base::ListValue& args);
    void sendInputEventApi();
    void beginFrameSubscriptionApi();
    void endFrameSubscriptionApi();
    void startDragApi();
    void setSizeApi();
    bool isGuestApi();
    bool isOffscreenApi();
    void startPaintingApi();
    void stopPaintingApi();
    bool isPaintingApi();
    void setFrameRateApi(int frameRate);
    int getFrameRateApi();
    void invalidateApi();
    void getTypeApi();
    void getWebPreferencesApi();
    v8::Local<v8::Value> getOwnerBrowserWindowApi();
    bool hasServiceWorkerApi();
    void unregisterServiceWorkerApi();
    void inspectServiceWorkerApi();
    void printApi();
    void _printToPDFApi();
    void addWorkSpaceApi();
    void reNullWorkSpaceApi();
    void showDefinitionForSelectionApi();
    void copyImageAtApi();
    void capturePageApi();
    void setEmbedderApi();
    bool isDestroyedApi() const;
    void reloadIgnoringCacheApi();

    void nullFunction();

    static void __stdcall staticDidCreateScriptContextCallback(mbWebView webView, void* param, void* frame, void* context, int extensionGroup, int worldId);
    void onDidCreateScriptContext(mbWebView webView, void* frame, v8::Local<v8::Context>* context, int extensionGroup, int worldId);
    static void __stdcall staticOnWillReleaseScriptContextCallback(mbWebView webView, void* param, void* frame, void* context, int worldId);
    void onWillReleaseScriptContextCallback(mbWebView webView, void* frame, v8::Local<v8::Context>* context, int worldId);
    static mbDownloadOpt __stdcall staticOnDownloadCallback(mbWebView, void*, size_t, const char*, const char*, const char*, mbNetJob, mbNetJobDataBind*);
    static void MB_CALL_TYPE onDocumentReadyInBlinkThread(mbWebView webView, void* param, mbWebFrameHandle frameId);
    static BOOL MB_CALL_TYPE onNavigationCallback(mbWebView webView, void* param, mbNavigationType navigationType, const utf8* url);
    static mbWebView MB_CALL_TYPE onCreateViewCallback(mbWebView webView, void* param, mbNavigationType navigationType, const utf8* url, const mbWindowFeatures* windowFeatures);

    void onUrlChange(const std::string& url) { m_url = url; }
    static void MB_CALL_TYPE onTitleChanged(mbWebView webView, void* param, const utf8* title);
    static void MB_CALL_TYPE onURLChanged(mbWebView webView, void* param, const utf8* url, BOOL canGoBack, BOOL canGoForward);
    static void MB_CALL_TYPE onLoadingFinishCallback(mbWebView webView, void* param, mbWebFrameHandle frameId, const utf8* url, mbLoadingResult result, const utf8* failedReason);

    void setCreateWindowParam(CreateWindowParam* createWindowParam)
    {
        m_createWindowParam = createWindowParam;
    }

public:
    static v8::Persistent<v8::Function> constructor;
    static gin::WrapperInfo kWrapperInfo;

private:
    friend class BrowserView;
    friend class BrowserWindow;

    NodeBindings* m_nodeBinding;
    int m_id;
    std::set<WebContentsObserver*> m_observers;
    mbWebView m_view;
    WindowInterface* m_owner;

    CreateWindowParam* m_createWindowParam;

    bool m_isNodeIntegration;
    bool m_isNodeIntegrationInSubframes;
    bool m_isContextIsolation;
    bool m_isLoading;

    bool m_canGoBack;
    bool m_canGoForward;

    std::string m_ua;
    std::string m_url;
    std::string m_title;
    std::string m_preloadScriptPath;
    std::string m_sessionName;
    int m_frameRate;

    v8::Persistent<v8::Object> m_liveSelf;
};

} // atom

namespace gin {
v8::Local<v8::Value> ConvertToV8(v8::Isolate* isolate, const atom::WebContents& content);
}

#endif // browser_api_ApiWebContents_h