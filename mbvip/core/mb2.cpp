
#include "core/mb.h"
#include "wke/wkedefine.h"
#include "wke/wkeWebView.h"
#include "mbvip/core/MbWebView.h"
#include "mbvip/common/LiveIdDetect.h"
#include "mbvip/common/ThreadCall.h"
#include "content/browser/WebPage.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebElement.h"
#include "third_party/WebKit/Source/bindings/core/v8/ToV8.h"
#include "gen/blink/bindings/core/v8/V8Element.h"
#include "third_party/WebKit/Source/wtf/text/Base64.h"
#include "v8.h"
#include <io.h>

mbMemBuf* MB_CALL_TYPE mbEncodeBase64(const char* data, unsigned len, int policy)
{
    Vector<char> out;
    WTF::base64Encode(data, len, out, (WTF::Base64EncodePolicy)policy);
    if (out.size() == 0)
        return nullptr;
    return mbCreateMemBuf(NULL_WEBVIEW, out.data(), out.size());
}

void WKE_CALL_TYPE onOtherLoadCallback(wkeWebView webView, void* param, wkeOtherLoadType type, wkeTempCallbackInfo* info)
{
    mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;

    webview->getClosure().m_NetViewLoadInfoCallback(webviewHandle, webview->getClosure().m_NetViewLoadInfoParam, (mbViewLoadType)type, (mbViewLoadCallbackInfo*)info);
}

void MB_CALL_TYPE mbNetOnViewLoadInfo(mbWebView webviewHandle, mbNetViewLoadInfoCallback callback, void* param)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setNetViewLoadInfoCallback(callback, param);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle](mb::MbWebView* webview) {
        wkeOnOtherLoad(webview->getWkeWebView(), onOtherLoadCallback, (void*)webviewHandle);
    });
}

BOOL MB_CALL_TYPE mbGetContextByV8Object(void* isolate, void* obj, int worldID, v8ContextPtr cxtOut)
{
    v8::Local<v8::Object>* object = (v8::Local<v8::Object>*)obj;

    v8::Local<v8::Context> context = (*object)->CreationContext();
    if (context.IsEmpty())
        return false;
    blink::WebLocalFrame* frame = blink::WebLocalFrame::frameForContext(context);
    if (!frame)
        return false;

    *(v8::Local<v8::Context>*)cxtOut = frame->getScriptContextFromWorldId((v8::Isolate*)isolate, worldID);
    return true;
}

static blink::Element* webElementFromV8Value(v8::Local<v8::Value> value)
{
    if (value.IsEmpty())
        return nullptr;
    blink::Element* element = blink::V8Element::toImplWithTypeCheck(v8::Isolate::GetCurrent(), value);
    return element;
}

static v8::Local<v8::Value> nodeToV8Value(blink::Element* elem, v8::Local<v8::Object> creation_context, v8::Isolate* isolate)
{
    // We no longer use |creation_context| because it's often misused and points
    // to a context faked by user script.
    ASSERT(creation_context->CreationContext() == isolate->GetCurrentContext());
    if (!elem)
        return v8::Local<v8::Value>();
    return toV8(elem, isolate->GetCurrentContext()->Global(), isolate);
}

BOOL MB_CALL_TYPE mbPassWebElementValueToOtherContext(void* val, void* destCtx, void* outVal)
{
    v8::Local<v8::Value>* value = (v8::Local<v8::Value>*)val;
    v8::Local<v8::Context>* destContext = (v8::Local<v8::Context>*)destCtx;

    blink::Element* elem = webElementFromV8Value(*value);
    if (!elem)
        return false;

    v8::Context::Scope destination_context_scope(*destContext);
    *(v8::Local<v8::Value>*)outVal = nodeToV8Value(elem, (*destContext)->Global(), (*destContext)->GetIsolate());
    return true;
}

void MB_CALL_TYPE mbGetWorldScriptContextByWebFrame(mbWebView webviewHandle, mbWebFrameHandle frameId, int worldID, v8ContextPtr contextOut)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;

    wkeWebFrameHandle wkeFrameId = (wkeWebFrameHandle)frameId;
    if ((mbWebFrameHandle)-2 == frameId)
        wkeFrameId = wkeWebFrameGetMainFrame(webview->getWkeWebView());

    //return wkeWebFrameGetMainWorldScriptContext(webview->getWkeWebView(), wkeFrameId, contextOut);
    content::WebPage* page = webview->getWkeWebView()->webPage();
    if (!page)
        return;
    blink::WebFrame* webFrame = page->getWebFrameFromFrameId(wke::CWebView::wkeWebFrameHandleToFrameId(page, wkeFrameId));
    if (!webFrame)
        return;
    v8::Local<v8::Context> result = webFrame->getScriptContextFromWorldId(v8::Isolate::GetCurrent(), worldID);
    v8::Local<v8::Context>* contextOutPtr = (v8::Local<v8::Context>*)contextOut;
    *contextOutPtr = result;
}

int MB_CALL_TYPE mb_open_osfhandle(intptr_t osfhandle, int sourceFlags)
{
    return _open_osfhandle(osfhandle, sourceFlags);
}

void* MB_CALL_TYPE mbGetProcAddr(const char* name)
{
    if (0 == strcmp(name, "mbGetContextByV8Object"))
        return mbGetContextByV8Object;
    if (0 == strcmp(name, "mbPassWebElementValueToOtherContext"))
        return mbPassWebElementValueToOtherContext;
    if (0 == strcmp(name, "mbGetWorldScriptContextByWebFrame"))
        return mbGetWorldScriptContextByWebFrame;
    if (0 == strcmp(name, "mbNetOnViewLoadInfo"))
        return mbNetOnViewLoadInfo;
    if (0 == strcmp(name, "mb_open_osfhandle"))
        return mb_open_osfhandle;
    if (0 == strcmp(name, "mbEncodeBase64"))
        return mbEncodeBase64;
    
    return nullptr;
}