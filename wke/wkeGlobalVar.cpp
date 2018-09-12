#include "wke/wkeGlobalVar.h"

namespace wke {

bool g_isSetDragEnable = true;
bool g_isSetDragDropEnable = true;

std::string* s_versionString = nullptr;
bool wkeIsInit = false;

unsigned long g_kWakeMinInterval = 5;
double g_kDrawMinInterval = 0.003;
bool g_isDecodeUrlRequest = false;
void* g_tipPaintCallback = nullptr;
float g_contentScale = 1;
bool g_rendererAntiAlias = false;

wkeUiThreadPostTaskCallback g_wkeUiThreadPostTaskCallback = nullptr;

wkeWillMediaLoadCallback g_wkeWillMediaLoadCallback = nullptr;
void* g_wkeWillMediaLoadCallbackCallbackParam = nullptr;

wkeMediaPlayerFactory g_wkeMediaPlayerFactory = nullptr;

wkeTempCallbackInfo g_wkeTempCallbackInfo;

}

WKE_FILE_OPEN g_pfnOpen;
WKE_FILE_CLOSE g_pfnClose;