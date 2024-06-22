
#ifndef content_browser_PostTaskHelper_h
#define content_browser_PostTaskHelper_h

#include <functional>
#include <windows.h>
#include "third_party/WebKit/public/platform/WebTraceLocation.h"

namespace content {

extern DWORD g_uiThreadId;

void postTaskToUiThread(const blink::WebTraceLocation& location, HWND hWnd, std::function<void(void)>&& closure);
void postTaskToMainThread(const blink::WebTraceLocation& location, std::function<void(void)>&& closure);
void postDelayTaskToMainThread(const blink::WebTraceLocation& location, std::function<void(void)>&& closure, long long ms);

}

#endif