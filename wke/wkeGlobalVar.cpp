
#include "wke/wkeGlobalVar.h"
#include "third_party/libcurl/include/curl/curl.h"
#include <stdarg.h>

namespace wke {

bool g_isSetDragEnable = true;
bool g_isSetDragDropEnable = false;

const char* kUndefineStrValue = "__mb_undefine__";

WTF::OwnPtr<std::string> s_versionString;
bool wkeIsInit = false;

unsigned long g_kWakeMinInterval = 8;
double g_kDrawMinInterval = 0.003;
bool g_isDecodeUrlRequest = false;
void* g_tipPaintCallback = nullptr;
float g_contentScale = 1;
bool g_rendererAntiAlias = false;
bool g_diskCacheEnable = false;
bool g_smootTextEnable = true;
bool g_consoleOutputEnable = true;
bool g_cutOutsNpapiRectsEnable = false;
bool g_navigatorPluginsEnable = true;
bool g_drawTileLineEnable = false;
bool g_flashChineseEnable = false;
bool g_headlessEnable = false;
bool g_backKeydownEnable = true;
bool g_jsClickEnable = false;
bool g_enableNativeSetCapture = true;
bool g_enableNativeSetFocus = true;

extern int g_outerWidth = kUnuseIntValue;
extern int g_outerHeight = kUnuseIntValue;

int g_disableDownloadMask = 0;

WTF::OwnPtr<std::string> g_navigatorAppCodeName;
WTF::OwnPtr<std::string> g_navigatorAppName;
WTF::OwnPtr<std::string> g_navigatorAppVersion;
WTF::OwnPtr<std::string> g_navigatorProduct;
WTF::OwnPtr<std::string> g_navigatorProductSub;
WTF::OwnPtr<std::string> g_navigatorVendorSub;
WTF::OwnPtr<std::string> g_navigatorLanguage;
WTF::OwnPtr<WTF::Vector<String>> g_navigatorLanguages;
WTF::OwnPtr<std::string> g_language;
WTF::OwnPtr<std::string> g_navigatorOscpu;
WTF::OwnPtr<std::string> g_navigatorBuildID;
WTF::OwnPtr<std::string> g_defaultLocale;

int g_navigatorDeviceMemory = 8;
int g_navigatorHardwareConcurrency = kUnuseIntValue;
int g_navigatorMaxTouchPoints = kUnuseIntValue;

WTF::OwnPtr<wkeGeolocationPosition> g_geoPos;

WTF::OwnPtr<std::string> g_DNS;

wkeOnNetCurlSetoptCallback g_curlSetoptCallback = nullptr;

extern "C" CURLcode Curl_vsetopt(CURL * data, CURLoption option, va_list arg);

void WKE_CALL_TYPE curlSetopt(void* curlHandle, __int32 option, ...)
{
    static_assert(sizeof(CURLoption) == sizeof(__int32), "curlSetopt: size of CURLoption is error");
    va_list arg;
    va_start(arg, option);
    Curl_vsetopt(curlHandle, (CURLoption)option, arg);
    va_end(arg);
}

wkeGetPluginListCallback g_getPluginListCallback = nullptr;

wkeOnPluginFindCallback g_wkePluginFindCallback = nullptr;
void* g_wkePluginFindCallbackParam = nullptr;

wkeImageBufferToDataURL g_wkeImageBufferToDataUrlCallback = nullptr;
void* g_wkeImageBufferToDataUrlCallbackParam = nullptr;

wkeUiThreadPostTaskCallback g_wkeUiThreadPostTaskCallback = nullptr;

wkeWillMediaLoadCallback g_wkeWillMediaLoadCallback = nullptr;
void* g_wkeWillMediaLoadCallbackCallbackParam = nullptr;

wkeMediaPlayerFactory g_wkeMediaPlayerFactory = nullptr;
wkeOnIsMediaPlayerSupportsMIMEType g_onIsMediaPlayerSupportsMIMETypeCallback = nullptr;

wkeTempCallbackInfo g_wkeTempCallbackInfo;

std::set<wkeWebView> g_liveWebViews;

DWORD g_contextMenuItemMask = kWkeMenuSelectedAllId | kWkeMenuSelectedTextId | kWkeMenuUndoId | kWkeMenuCopyImageId | kWkeMenuSaveImageId | kWkeMenuInspectElementAtId |
    kWkeMenuCutId | kWkeMenuPasteId;

void releaseGlobalVar()
{
    s_versionString.clear();
    g_navigatorAppCodeName.clear();
    g_navigatorAppName.clear();
    g_navigatorAppVersion.clear();
    g_navigatorProduct.clear();
    g_navigatorProductSub.clear();
    g_navigatorVendorSub.clear();
    g_navigatorLanguage.clear();
    g_navigatorLanguages.clear();
    g_navigatorOscpu.clear();
    g_navigatorBuildID.clear();
    g_geoPos.clear();
    g_DNS.clear();
}

}

WKE_FILE_OPEN g_pfnOpen;
WKE_FILE_CLOSE g_pfnClose;