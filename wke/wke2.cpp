
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

namespace wke {

bool setDebugConfig(wkeWebView webview, const char* debugString, const char* param)
{
    return false;
}

}
