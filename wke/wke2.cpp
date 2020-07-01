
#include "wke/wke2.h"
#include "wke/wkeString.h"
#include "wke/wkeWebView.h"
#include "wke/wkeWebWindow.h"
#include "wke/wkeGlobalVar.h"
#include "content/browser/WebPage.h"
#include "content/web_impl_win/BlinkPlatformImpl.h"
#include "content/web_impl_win/WebThreadImpl.h"
#include "content/resources/HeaderFooterHtml.h"
#include "third_party/WebKit/Source/web/WebViewImpl.h"
#include "third_party/WebKit/Source/web/WebSettingsImpl.h"
#include "third_party/WebKit/public/web/WebPrintScalingOption.h"
#include "third_party/WebKit/public/web/WebPrintParams.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebScriptSource.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/skia/include/core/SkStream.h"
#include "third_party/skia/include/core/SkDocument.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "skia/ext/refptr.h"
#include "wtf/text/WTFString.h"
#include "wtf/text/WTFStringUtil.h"
#include "base/values.h"
#include "base/json/json_writer.h"
#include "net/WebURLLoaderInternal.h"
#include "net/InitializeHandleInfo.h"
#include <v8.h>
#include <shlwapi.h>

namespace net {

void WSCI_setHook(void* j, void* hook);
void WSCI_sendtext(void* j, char* buf, size_t len);
void WSCI_sendblob(void* j, char* buf, size_t len);

}

namespace wke {

bool setDebugConfig(wkeWebView webview, const char* debugString, const char* param)
{
    return false;
}

bool getDebugConfig(wkeWebView webview, const char* debugString, void **ret)
{
    return false;
}


const wkePdfDatas* printToPdf(wkeWebView webView, blink::WebFrame* webFrame, const wkePrintSettings* params)
{
    return nullptr;
}

const wkeMemBuf* printToBitmap(wkeWebView webView, const wkeScreenshotSettings* settings)
{
    return nullptr;
}

}

void wkeUtilRelasePrintPdfDatas(const wkePdfDatas* datas)
{
    for (int i = 0; i < datas->count; ++i) {
        free((void *)(datas->datas[i]));
    }

    free((void *)(datas->sizes));
    free((void *)(datas->datas));
    delete datas;
}

const wkePdfDatas* wkeUtilPrintToPdf(wkeWebView webView, wkeWebFrameHandle frameId, const wkePrintSettings* settings)
{
    content::WebPage* webPage = webView->webPage();
    blink::WebFrame* webFrame = webPage->getWebFrameFromFrameId(wke::CWebView::wkeWebFrameHandleToFrameId(webPage, frameId));
    return wke::printToPdf(webView, webFrame, settings);
}

const wkeMemBuf* wkePrintToBitmap(wkeWebView webView, wkeWebFrameHandle frameId, const wkeScreenshotSettings* settings)
{
    return wke::printToBitmap(webView, settings);
}