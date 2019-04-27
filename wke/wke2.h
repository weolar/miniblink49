
#ifndef wke2_h
#define wke2_h

#include "wke/wkedefine.h"

namespace wke {

bool setDebugConfig(wkeWebView webview, const char* debugString, const char* param);
bool getDebugConfig(wkeWebView webview, const char* debugString, void** ret);
}

#endif
