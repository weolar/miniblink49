
#ifndef renderer_WebviewPluginImpl_h
#define renderer_WebviewPluginImpl_h

#include "third_party/npapi/bindings/npapi.h"
#include "third_party/npapi/bindings/npfunctions.h"
#include "third_party/npapi/bindings/npruntime.h"
#include "wke.h"
#include <string>

namespace base {
class ListValue;
}

namespace atom {

class NodeBindings;

class WebviewPluginImpl {
public:
    WebviewPluginImpl(wkeWebView parentWebview);
    ~WebviewPluginImpl();

    int getGuestId() const { return m_guestId; }
    void setGuestId(int id) { m_guestId = id; }

    void setNpObj(NPObject* npObj) { m_npObj = npObj; }
    NPObject* getNpObj() const { return m_npObj; }

    wkeWebView getWebview() const { return m_webview; }

    void setInstance(NPP instance) { m_instance = instance; }
    NPP getInstance() const { return m_instance; }

    void onSize(const WINDOWPOS& windowpos);
    void onPaint(HDC hdc);
    void onSetWinow(const NPWindow& npWindow);

    void onMouseEvt(uint32_t message, uint32_t wParam, uint32_t lParam);
    void onKey(uint32_t message, uint32_t wParam, uint32_t lParam);

    void loadURL(
        const std::string& urlString, 
        const std::string& httpReferrerString, 
        const std::string& userAgentString, 
        const std::string& extraHeadersString
        );

    std::string getURL();

    void setPreloadURL(const std::string& preloadURL);

    void hostSendMessageToGuest(const std::string& channel, const base::ListValue& listParams);
    void guestSendMessageToHost(const std::string& channel, const base::ListValue& listParams);

private:
    static void staticOnPaintUpdated(wkeWebView webView, WebviewPluginImpl* self, const HDC hdc, int x, int y, int cx, int cy);
    static void staticDidCreateScriptContextCallback(wkeWebView webView, WebviewPluginImpl* self, wkeWebFrameHandle frame, void* context, int extensionGroup, int worldId);
    static void staticOnWillReleaseScriptContextCallback(wkeWebView webView, WebviewPluginImpl* self, wkeWebFrameHandle frame, void* context, int worldId);

    static void onDocumentReady(wkeWebView webView, WebviewPluginImpl* self);
    static void onLoadingFinishCallback(wkeWebView webView, WebviewPluginImpl* self, const wkeString url, wkeLoadingResult result, const wkeString failedReason);
    static void onOtherLoadCallback(wkeWebView webView, WebviewPluginImpl* self, wkeOtherLoadType type, wkeTempCallbackInfo* info);

    void loadPreloadURL();

    wkeWebView m_parentWebview;
    NPObject* m_npObj;
    NPWindow m_npWindow;
    NPP m_instance;

    NodeBindings* m_nodeBinding;

    wkeWebView m_webview;
    int m_id;
    int m_guestId;

    std::string* m_preloadcode;
};

}

#endif // renderer_WebviewPlugin.h

