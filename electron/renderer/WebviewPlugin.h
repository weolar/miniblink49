
#ifndef renderer_WebviewPlugin_h
#define renderer_WebviewPlugin_h

#include "third_party/npapi/bindings/npapi.h"
#include "third_party/npapi/bindings/npfunctions.h"
#include "third_party/npapi/bindings/npruntime.h"

namespace atom {

extern "C" NPError __stdcall Webview_NP_Initialize(NPNetscapeFuncs *browserFuncs);
extern "C" NPError __stdcall Webview_NP_GetEntryPoints(NPPluginFuncs *pluginFuncs);
extern "C" void __stdcall Webview_NP_Shutdown(void);

}

#endif // renderer_WebviewPlugin.h

