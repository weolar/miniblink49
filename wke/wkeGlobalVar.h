
#ifndef wke_wkeGlobalVar_h
#define wke_wkeGlobalVar_h

#include "wke.h"
#include "third_party/WebKit/Source/wtf/OwnPtr.h"
#include "third_party/WebKit/Source/wtf/Vector.h"
#include "third_party/WebKit/Source/wtf/text/WTFString.h"
#include <string>
#include <set>

typedef BOOL(__stdcall* wkeGetPluginListCallback)(BOOL refresh, void* pluginListBuilder, void* param);

namespace wke {

extern const char* kUndefineStrValue;
const int kUndefineIntValue = -22133534;
const int kUnuseIntValue = -2332265;

extern bool g_isSetDragEnable;
extern bool g_isSetDragDropEnable;

extern WTF::OwnPtr<std::string> s_versionString;
extern bool wkeIsInit;

extern unsigned long g_kWakeMinInterval;
extern double g_kDrawMinInterval;
extern bool g_isDecodeUrlRequest;
extern void* g_tipPaintCallback;
extern float g_contentScale;
extern bool g_rendererAntiAlias;
extern bool g_diskCacheEnable;
extern bool g_smootTextEnable;
extern bool g_consoleOutputEnable;
extern bool g_cutOutsNpapiRectsEnable;
extern bool g_navigatorPluginsEnable;
extern bool g_drawTileLineEnable;
extern bool g_flashChineseEnable;
extern bool g_headlessEnable;
extern bool g_backKeydownEnable;
extern bool g_jsClickEnable;
extern bool g_enableNativeSetCapture;
extern bool g_enableNativeSetFocus;

extern int g_outerWidth;
extern int g_outerHeight;

enum DisableDownloadType {
    kDisableImageDownload = 1 << 0,
    kDisableScriptDownload = 1 << 1,
    kDisableCssDownload = 1 << 2
};
extern int g_disableDownloadMask;

extern WTF::OwnPtr<std::string> g_navigatorAppCodeName;
extern WTF::OwnPtr<std::string> g_navigatorAppName;
extern WTF::OwnPtr<std::string> g_navigatorAppVersion;
extern WTF::OwnPtr<std::string> g_navigatorProduct;
extern WTF::OwnPtr<std::string> g_navigatorProductSub;
extern WTF::OwnPtr<std::string> g_navigatorVendorSub;
extern WTF::OwnPtr<std::string> g_navigatorLanguage;
extern WTF::OwnPtr<std::string> g_language;
extern WTF::OwnPtr<std::string> g_navigatorOscpu;
extern WTF::OwnPtr<std::string> g_navigatorBuildID;
extern WTF::OwnPtr<std::string> g_defaultLocale;

extern WTF::OwnPtr<WTF::Vector<String>> g_navigatorLanguages;

extern int g_navigatorDeviceMemory;
extern int g_navigatorHardwareConcurrency;
extern int g_navigatorMaxTouchPoints;

extern WTF::OwnPtr<wkeGeolocationPosition> g_geoPos;

extern wkeGetPluginListCallback g_getPluginListCallback;

extern wkeUiThreadPostTaskCallback g_wkeUiThreadPostTaskCallback;

extern wkeWillMediaLoadCallback g_wkeWillMediaLoadCallback;
extern void* g_wkeWillMediaLoadCallbackCallbackParam;

extern wkeMediaPlayerFactory g_wkeMediaPlayerFactory;
extern wkeOnIsMediaPlayerSupportsMIMEType g_onIsMediaPlayerSupportsMIMETypeCallback;

extern wkeOnPluginFindCallback g_wkePluginFindCallback;
extern void* g_wkePluginFindCallbackParam;

extern wkeImageBufferToDataURL g_wkeImageBufferToDataUrlCallback;
extern void* g_wkeImageBufferToDataUrlCallbackParam;

extern wkeTempCallbackInfo g_wkeTempCallbackInfo;

extern std::set<wkeWebView> g_liveWebViews;

extern DWORD g_contextMenuItemMask;

extern WTF::OwnPtr<std::string> g_DNS;

extern wkeOnNetCurlSetoptCallback g_curlSetoptCallback;
void curlSetopt(void* curlHandle, __int32 option, ...);

void releaseGlobalVar();

}

extern WKE_FILE_OPEN g_pfnOpen;
extern WKE_FILE_CLOSE g_pfnClose;

#endif