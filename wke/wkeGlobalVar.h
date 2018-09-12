
#ifndef wke_wkeGlobalVar_h

#include <string>
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

extern wkeUiThreadPostTaskCallback g_wkeUiThreadPostTaskCallback;

extern wkeWillMediaLoadCallback g_wkeWillMediaLoadCallback;
extern void* g_wkeWillMediaLoadCallbackCallbackParam;

extern wkeMediaPlayerFactory g_wkeMediaPlayerFactory;

extern wkeTempCallbackInfo g_wkeTempCallbackInfo;

}

extern WKE_FILE_OPEN g_pfnOpen;
extern WKE_FILE_CLOSE g_pfnClose;

#endif