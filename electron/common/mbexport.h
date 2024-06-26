
#ifndef electron_common_mbexport_h
#define electron_common_mbexport_h

#include "mbvip/core/mb.h"

typedef void (MB_CALL_TYPE* FN_mbGetWorldScriptContextByWebFrame)(mbWebView webviewHandle, mbWebFrameHandle frameId, int worldID, v8ContextPtr contextOut);
__declspec(selectany) FN_mbGetWorldScriptContextByWebFrame s_mbGetWorldScriptContextByWebFrame = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbPassWebElementValueToOtherContext)(void* val, void* destCtx, void* outVal);
__declspec(selectany) FN_mbPassWebElementValueToOtherContext s_mbPassWebElementValueToOtherContext = nullptr;

typedef BOOL(MB_CALL_TYPE* FN_mbGetContextByV8Object)(void* isolate, void* obj, int worldID, void* cxtOut);
__declspec(selectany) FN_mbGetContextByV8Object s_mbGetContextByV8Object = nullptr;

#endif // electron_common_mbexport_h