﻿
#ifndef browser_api_ApiWebContents_h
#define browser_api_ApiWebContents_h

#include "nodeblink.h"
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
    };

    static void init(v8::Isolate* isolate, v8::Local<v8::Object> target, node::Environment* env);
    static WebContents* create(v8::Isolate* isolate, gin::Dictionary options, WindowInterface* owner);

    explicit WebContents(v8::Isolate* isolate, v8::Local<v8::Object> wrapper);
    ~WebContents();

    void addObserver(WebContentsObserver* observer);
    void removeObserver(WebContentsObserver* observer);

    wkeWebView getWkeView() const { return m_view; }

    void onNewWindowInBlinkThread(int width, int height, const CreateWindowParam* createWindowParam);

    void rendererPostMessageToMain(const std::string& channel, const base::ListValue& listParams);
    void rendererSendMessageToMain(const std::string& channel, const base::ListValue& listParams, std::string* jsonRet);
    void anyPostMessageToRenderer(const std::string& channel, const base::ListValue& listParams);
private:
    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args);

    int getIdApi() const;
    void _loadURLApi(const std::string& url);
    int getProcessIdApi() const;
    bool equalApi() const;

    std::string _getURLApi();

    std::string getTitleApi();

    bool isLoadingApi();

    bool isLoadingMainFrameApi();

    bool isWaitingForResponseApi();

    void _stopApi();

    void _goBackApi();

    void _goForwardApi();

    void _goToOffsetApi();

    bool isCrashedApi();

    void setUserAgentApi(const std::string userAgent);

    std::string getUserAgentApi();

    void insertCSSApi();

    void savePageApi();

    void openDevToolsApi();

    void closeDevToolsApi();

    void isDevToolsOpenedApi();

    void isDevToolsFocusedApi();

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

    void setFrameRateApi();

    void getFrameRateApi();

    void invalidateApi();

    void getTypeApi();

    void getWebPreferencesApi();

    v8::Local<v8::Value> getOwnerBrowserWindowApi();

    void hasServiceWorkerApi();

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
    
    void nullFunction();

    static void staticDidCreateScriptContextCallback(wkeWebView webView, void* param, void* frame, void* context, int extensionGroup, int worldId);
    void onDidCreateScriptContext(wkeWebView webView, void* frame, v8::Local<v8::Context>* context, int extensionGroup, int worldId);
    static void staticOnWillReleaseScriptContextCallback(wkeWebView webView, void* param, void* frame, void* context, int worldId);
    void onWillReleaseScriptContextCallback(wkeWebView webView, void* frame, v8::Local<v8::Context>* context, int worldId);
public:
    static v8::Persistent<v8::Function> constructor;
    static gin::WrapperInfo kWrapperInfo;

private:
    NodeBindings* m_nodeBinding;
    int m_id;
    std::set<WebContentsObserver*> m_observers;
    wkeWebView m_view;
    WindowInterface* m_owner;
};

} // atom

#endif // browser_api_ApiWebContents_h