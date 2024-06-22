
#include "content/browser/PostTaskHelper.h"
#include "orig_chrome/content/OrigChromeMgr.h"
#include "wke/wkeGlobalVar.h"
#include "third_party/WebKit/public/platform/WebThread.h"
#include "third_party/WebKit/public/platform/Platform.h"
#include "third_party/WebKit/public/platform/WebTraceLocation.h"

namespace content {

DWORD g_uiThreadId = 0;

void closureCallWrap(HWND hWnd, void* param)
{
    std::function<void(void)>* closure = (std::function<void(void)>*)param;
    (*closure)();
    delete closure;
}

void WKE_CALL_TYPE closureCall(HWND hWnd, void* param)
{
    closureCallWrap(hWnd, param);
}

void postTaskToUiThread(const blink::WebTraceLocation& location, HWND hWnd, std::function<void(void)>&& closure)
{
    bool uesOrigCC = false;
#ifndef NO_USE_ORIG_CHROME
    uesOrigCC = !!OrigChromeMgr::getInst();
#endif

    if (uesOrigCC) { // 如果不使用原版cc，则不管是否开启多线程渲染，窗口都创建在blink线程
        std::function<void(void)>* closureDummy = new std::function<void(void)>(std::move(closure));
        wke::g_wkeUiThreadPostTaskCallback(hWnd, &closureCall, closureDummy);
    } else {
        postTaskToMainThread(location, std::move(closure));
    }
}

void postTaskToMainThread(const blink::WebTraceLocation& location, std::function<void(void)>&& closure)
{
    std::function<void(void)>* closureDummy = new std::function<void(void)>(std::move(closure));
    blink::Platform::current()->mainThread()->postTask(location, WTF::bind(&closureCallWrap, nullptr, closureDummy));
}

void postDelayTaskToMainThread(const blink::WebTraceLocation& location, std::function<void(void)>&& closure, long long ms)
{
    std::function<void(void)>* closureDummy = new std::function<void(void)>(std::move(closure));
    blink::Platform::current()->mainThread()->postDelayedTask(location, WTF::bind(&closureCallWrap, nullptr, closureDummy), ms);
}

}