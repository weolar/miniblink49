
#ifndef wke_wkeGlobalVar_h

#include <string>
#include <set>
#include "wke.h"

namespace wke {

extern bool g_isSetDragEnable;
extern bool g_isSetDragDropEnable;

extern std::string* s_versionString;
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

extern wkeUiThreadPostTaskCallback g_wkeUiThreadPostTaskCallback;

extern wkeWillMediaLoadCallback g_wkeWillMediaLoadCallback;
extern void* g_wkeWillMediaLoadCallbackCallbackParam;

extern wkeMediaPlayerFactory g_wkeMediaPlayerFactory;
extern wkeOnIsMediaPlayerSupportsMIMEType g_onIsMediaPlayerSupportsMIMETypeCallback;

extern wkeOnPluginFindCallback g_wkePluginFindcallback;
extern void* g_wkePluginFindcallbackParam;

extern wkeTempCallbackInfo g_wkeTempCallbackInfo;

extern std::set<wkeWebView> g_liveWebViews;

extern DWORD g_contextMenuItemMask;

}

extern WKE_FILE_OPEN g_pfnOpen;
extern WKE_FILE_CLOSE g_pfnClose;

#endif