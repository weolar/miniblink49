
#ifndef browser_api_ApiWebContents_h
#define browser_api_ApiWebContents_h

#include "node/nodeblink.h"
#include "wke.h"
#include "gin/dictionary.h"
#include "common/api/EventEmitter.h"
#include <set>

namespace base {
class ListValue;
}

namespace atom {

class NodeBindings;
class WebContents;
class WindowInterface;

class WebContentsObserver {
public:
    virtual void onWebContentsCreated(WebContents* contents) {}
    virtual void onWebContentsDeleted(WebContents* contents) {}
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

        CreateWindowParam() {
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

    explicit WebContents(v8::Isolate* isolate, v8::Local<v8::Object> wrapper);
    ~WebContents();

    void addObserver(WebContentsObserver* observer);
    void removeObserver(WebContentsObserver* observer);

    wkeWebView getWkeView() const { return m_view; }
    WindowInterface* getOwner() const { return m_owner; }

    void onNewWindowInBlinkThread(int width, int height, const CreateWindowParam* createWindowParam);

    void rendererPostMessageToMain(const std::string& channel, const base::ListValue& listParams);
    void rendererSendMessageToMain(const std::string& channel, const base::ListValue& listParams, std::string* jsonRet);
    void anyPostMessageToRenderer(const std::string& channel, const base::ListValue& listParams);
    static void rendererSendMessageToRenderer(wkeWebView view, wkeWebFrameHandle frame, const std::string& channel, const base::ListValue& args);

private:
    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args);

    int getIdApi() const;
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
    
    void nullFunction();

    static void staticDidCreateScriptContextCallback(wkeWebView webView, void* param, void* frame, void* context, int extensionGroup, int worldId);
    void onDidCreateScriptContext(wkeWebView webView, void* frame, v8::Local<v8::Context>* context, int extensionGroup, int worldId);
    static void staticOnWillReleaseScriptContextCallback(wkeWebView webView, void* param, void* frame, void* context, int worldId);
    void onWillReleaseScriptContextCallback(wkeWebView webView, void* frame, v8::Local<v8::Context>* context, int worldId);

    void onUrlChange(const std::string& url) { m_url = url; }
    void onTitleChange(const std::string& title) { m_title = title; }

public:
    static v8::Persistent<v8::Function> constructor;
    static gin::WrapperInfo kWrapperInfo;

private:
    friend class Window;

    NodeBindings* m_nodeBinding;
    int m_id;
    std::set<WebContentsObserver*> m_observers;
    wkeWebView m_view;
    WindowInterface* m_owner;

    bool m_isNodeIntegration;
    bool m_isLoading;

    std::string m_ua;
    std::string m_url;
    std::string m_title;
    int m_frameRate;

    v8::Persistent<v8::Object> m_liveSelf;
};

} // atom

#endif // browser_api_ApiWebContents_h