
#define NODE_WANT_INTERNALS 1

#include "renderer/WebviewPluginImpl.h"

#include "browser/api/ApiWebContents.h"
#include "common/ThreadCall.h"
#include "common/IdLiveDetect.h"
#include "common/NodeBinding.h"
#include "common/api/EventEmitterCaller.h"
#include "node/src/env.h"
#include "node/src/env-inl.h"
#include "node/nodeblink.h"
#include "base/json/json_writer.h"
#include "v8.h"
#include <string>

namespace atom {

extern NPNetscapeFuncs* g_npBrowserFunctions;

void WebviewPluginImpl::staticDidCreateScriptContextCallback(wkeWebView webView, WebviewPluginImpl* self, wkeWebFrameHandle frame, void* context, int extensionGroup, int worldId) {
    v8::Local<v8::Context>& contextV8 = *(v8::Local<v8::Context>*)context;
    contextV8->SetAlignedPointerInEmbedderData(node::Environment::kContextEmbedderDataIndex, nullptr);

    BlinkMicrotaskSuppressionHandle handle = nodeBlinkMicrotaskSuppressionEnter(contextV8->GetIsolate());

    self->m_nodeBinding = new NodeBindings(false, ThreadCall::getBlinkLoop());
    node::Environment* env = self->m_nodeBinding->createEnvironment(contextV8);
    self->m_nodeBinding->loadEnvironment();

    nodeBlinkMicrotaskSuppressionLeave(handle);

    self->loadPreloadURL();
}

void WebviewPluginImpl::staticOnWillReleaseScriptContextCallback(wkeWebView webView, WebviewPluginImpl* self, wkeWebFrameHandle frame, void* context, int worldId) {
    v8::Local<v8::Context>& contextV8 = *(v8::Local<v8::Context>*)context;
    node::Environment* env = node::Environment::GetCurrent(contextV8);
    if (env)
        mate::emitEvent(env->isolate(), env->process_object(), "exit");

    delete self->m_nodeBinding;
    self->m_nodeBinding = nullptr;
}

void WebviewPluginImpl::onLoadingFinishCallback(wkeWebView webView, WebviewPluginImpl* self, const wkeString url, wkeLoadingResult result, const wkeString failedReason) {
    int id = self->m_id;
    wkeTempCallbackInfo* tempInfo = wkeGetTempCallbackInfo(webView);
    bool isMainFrame = wkeIsMainFrame(webView, tempInfo->frame);
    const utf8* failedReasonString = wkeGetString(failedReason);
    const utf8* urlString = wkeGetString(url);

    std::string jsonRet;
    base::ListValue listParams;
   
    if (result == WKE_LOADING_SUCCEEDED) {
        listParams.AppendBoolean(isMainFrame);
        self->guestSendMessageToHost("did-frame-finish-load", listParams);

        listParams.Clear();
        const utf8* url = wkeGetURL(self->m_webview);
        listParams.AppendString(url);
        listParams.AppendBoolean(isMainFrame);
        self->guestSendMessageToHost("load-commit", listParams);

        listParams.Clear();
        if (isMainFrame)
            self->guestSendMessageToHost("did-finish-load", listParams);
    } else {
        listParams.AppendInteger(0);
        listParams.AppendString(failedReasonString);
        listParams.AppendString(urlString);
        listParams.AppendBoolean(isMainFrame);
        self->guestSendMessageToHost("did-fail-provisional-load", listParams);

        listParams.Clear();
        if (result == WKE_LOADING_FAILED)
            self->guestSendMessageToHost("did-fail-load", listParams);
    }
}

void WebviewPluginImpl::onDocumentReady(wkeWebView webView, WebviewPluginImpl* self) {
    base::ListValue listParams;
    std::string jsonRet;
    self->guestSendMessageToHost("dom-ready", listParams);
}

void WebviewPluginImpl::onOtherLoadCallback(wkeWebView webView, WebviewPluginImpl* self, wkeOtherLoadType type, wkeTempCallbackInfo* info) {
    base::ListValue listParams;
    std::string jsonRet;
    
    if (WKE_DID_START_LOADING == type) {
        self->guestSendMessageToHost("did-start-loading", listParams);
    } else if (WKE_DID_STOP_LOADING == type) {
        self->guestSendMessageToHost("did-stop-loading", listParams);
    } else if (WKE_DID_GET_RESPONSE_DETAILS == type) {
        ;
    } else if (WKE_DID_NAVIGATE == type) {
        self->loadPreloadURL();
    }
}

WebviewPluginImpl::WebviewPluginImpl(wkeWebView parentWebview) {
    m_id = IdLiveDetect::get()->constructed(this);
    m_preloadcode = nullptr;
    m_guestId = -1;
    m_npObj = nullptr;
    m_instance = nullptr;

    m_parentWebview = parentWebview;
    m_webview = wkeCreateWebView();
    wkeOnPaintUpdated(m_webview, (wkePaintUpdatedCallback)staticOnPaintUpdated, this);
    wkeOnDidCreateScriptContext(m_webview, (wkeDidCreateScriptContextCallback)&staticDidCreateScriptContextCallback, this);
    wkeOnWillReleaseScriptContext(m_webview, (wkeWillReleaseScriptContextCallback)&staticOnWillReleaseScriptContextCallback, this);
    
    wkeOnLoadingFinish(m_webview, (wkeLoadingFinishCallback)onLoadingFinishCallback, this);
    wkeOnDocumentReady(m_webview, (wkeDocumentReadyCallback)onDocumentReady, this);
    wkeOnOtherLoad(m_webview, (wkeOnOtherLoadCallback)onOtherLoadCallback, this);
    
    wkeSetCspCheckEnable(m_webview, false);
    wkeSetUserKeyValue(m_webview, "WebviewPluginImpl", this);
    // wkeSetDebugConfig(m_webview, "drawDirtyDebugLine", nullptr);
}

WebviewPluginImpl::~WebviewPluginImpl() {
    IdLiveDetect::get()->deconstructed(m_id);
}

void WebviewPluginImpl::loadPreloadURL() {
    if (!m_preloadcode)
        return;

    std::string* preloadcode = m_preloadcode;
    m_preloadcode = nullptr;
    wkeRunJS(m_webview, preloadcode->c_str());
    delete preloadcode;
}

void WebviewPluginImpl::staticOnPaintUpdated(wkeWebView webView, WebviewPluginImpl* self, const HDC hdc, int x, int y, int cx, int cy) {
    int id = self->m_id;
    ThreadCall::callBlinkThreadAsync([self, id, x, y, cx, cy] {
        if (!IdLiveDetect::get()->isLive(id))
            return;
        NPRect rect = { (uint16_t)y, (uint16_t)x, (uint16_t)(y + cy), (uint16_t)(x + cx) };
        g_npBrowserFunctions->invalidaterect(self->m_instance, &rect);
    });
}

void WebviewPluginImpl::onSize(const WINDOWPOS& windowpos) {
    wkeResize(m_webview, windowpos.cx, windowpos.cy);
}

void WebviewPluginImpl::onPaint(HDC hdc) {
    HDC wkeDC = wkeGetViewDC(m_webview);
    ::BitBlt(hdc, m_npWindow.x, m_npWindow.y, m_npWindow.width, m_npWindow.height, wkeDC, 0, 0, SRCCOPY);
}

void WebviewPluginImpl::onSetWinow(const NPWindow& npWindow) {
    m_npWindow = npWindow;
}

void WebviewPluginImpl::onMouseEvt(uint32_t message, uint32_t wParam, uint32_t lParam) {
    int x = LOWORD(lParam) - m_npWindow.x;
    int y = HIWORD(lParam) - m_npWindow.y;

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

    int delta = GET_WHEEL_DELTA_WPARAM(wParam);
    int id = m_id;
    wkeWebView webview = m_webview;

    ThreadCall::callBlinkThreadAsync([webview, id, message, x, y, delta, flags] {
        if (!IdLiveDetect::get()->isLive(id))
            return;

        if (WM_MOUSEWHEEL == message)
            wkeFireMouseWheelEvent(webview, x, y, delta, flags);
        else
            wkeFireMouseEvent(webview, message, x, y, flags);
    });
}

void WebviewPluginImpl::onKey(uint32_t message, uint32_t wParam, uint32_t lParam) {
    unsigned int virtualKeyCode = wParam;
    unsigned int flags = 0;
    if (HIWORD(lParam) & KF_REPEAT)
        flags |= WKE_REPEAT;
    if (HIWORD(lParam) & KF_EXTENDED)
        flags |= WKE_EXTENDED;

    int id = m_id;
    wkeWebView webview = m_webview;
    ThreadCall::callBlinkThreadAsync([webview, id, message, virtualKeyCode, flags] {
        if (!IdLiveDetect::get()->isLive(id))
            return;
        switch (message) {
        case WM_KEYDOWN:
            wkeFireKeyDownEvent(webview, virtualKeyCode, flags, false);
            break;
        case WM_KEYUP:
            wkeFireKeyUpEvent(webview, virtualKeyCode, flags, false);
            break;
        case WM_CHAR:
            wkeFireKeyPressEvent(webview, virtualKeyCode, flags, false);
            break;
        }
    });
}

void WebviewPluginImpl::setPreloadURL(const std::string& preload) {
    std::string preloadURL = preload;
    if (preloadURL.size() > 9 && "file:///" == preloadURL.substr(0, 8))
        preloadURL = preloadURL.substr(8);
    
    m_preloadcode = new std::string("require('");
    m_preloadcode->append(preloadURL);
    m_preloadcode->append("');");
    //wkeRunJS(m_webview, code.c_str());
}

void WebviewPluginImpl::hostSendMessageToGuest(const std::string& channel, const base::ListValue& listParams) {

    std::string* channelCopy = new std::string(channel);
    base::ListValue* listParamsCopy = listParams.DeepCopy();
    wkeWebView webview = m_webview;
    int id = m_id;
    ThreadCall::callBlinkThreadAsync([id, webview, channelCopy, listParamsCopy] {
        if (!IdLiveDetect::get()->isLive(id))
            return;
        WebContents::rendererSendMessageToRenderer(webview, wkeWebFrameGetMainFrame(webview), *channelCopy, *listParamsCopy);
        delete channelCopy;
        delete listParamsCopy;
    });
}

void WebviewPluginImpl::guestSendMessageToHost(const std::string& channel, const base::ListValue& listParams) {
    NPVariant voidResponse;

    NPVariant channelNp;
    STRINGZ_TO_NPVARIANT(channel.c_str(), channelNp);

    std::string params;
    base::JSONWriter::Write(listParams, &params);
    NPVariant paramsNp;
    STRINGZ_TO_NPVARIANT(params.c_str(), paramsNp);

    NPVariant args[] = { channelNp, paramsNp };

    NPIdentifier funcID = g_npBrowserFunctions->getstringidentifier("onNativeMessage");

    NPObject* pluginScriptObject;
    NPError err = g_npBrowserFunctions->getvalue(m_instance, NPNVPluginElementNPObject, (void*)&pluginScriptObject);
    if (NPERR_GENERIC_ERROR == err)
        return;

    bool isOk = g_npBrowserFunctions->invoke(m_instance, pluginScriptObject, funcID, args, 2, &voidResponse);
    isOk = isOk;
}

void WebviewPluginImpl::loadURL(
    const std::string& urlString,
    const std::string& httpReferrerString,
    const std::string& userAgentString,
    const std::string& extraHeadersString) {
//     if (1) {
//         urlString = "data:text/html;charset=utf-8,%3C%21DOCTYPE%20html%3E%0D%0A%3Chtml%20lang%3D%22en%22%20style%3D%22width%3A%20100%25%3B%20height%3A%20100%25%22%3E%0D%0A%3Chead%3E%0D%0A%09%3Ctitle%3EVirtual%20Document%3C%2Ftitle%3E%0D%0A%3C%2Fhead%3E%0D%0A%3Cbody%20style%3D%22margin%3A%200%3B%20overflow%3A%20hidden%3B%20width%3A%20100%25%3B%20height%3A%20100%25%3bbgcolor%3argb(50%2c50%2c50)%22%3E%0D%0A%3C%2Fbody%3E%0D%0A%3C%2Fhtml%3E";
//     }
    wkeLoadURL(m_webview, urlString.c_str());
}

std::string WebviewPluginImpl::getURL() {
    return wkeGetURL(m_webview);
}

}

