
#ifndef SharedTimerWin_h
#define SharedTimerWin_h

#include "content/web_impl_win/WebThreadImpl.h"
#include "content/browser/CheckReEnter.h"

namespace content {

static HWND timerWindowHandle = 0;
const LPCWSTR kTimerWindowClassName = L"MiniBlinkTimerWindowClass";
 
static LRESULT CALLBACK TimerWindowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (0 != CheckReEnter::s_kEnterContent)
        return 0;
    ++CheckReEnter::s_kEnterContent;

    if (WM_TIMER ==  message) {
        content::WebThreadImpl* threadImpl = nullptr;
        threadImpl = (content::WebThreadImpl*)(blink::Platform::current()->currentThread());
        threadImpl->fire();
    }

    LRESULT result = ::DefWindowProc(hWnd, message, wParam, lParam);
    --CheckReEnter::s_kEnterContent;
    return result;
}

static void initializeOffScreenTimerWindow()
{
    if (timerWindowHandle)
        return;

    WNDCLASSEX wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.lpfnWndProc = TimerWindowWndProc;
    wcex.hInstance = ::GetModuleHandle(NULL);
    wcex.lpszClassName = kTimerWindowClassName;
    ::RegisterClassEx(&wcex);
    
    timerWindowHandle = CreateWindowExW(
        0,        // window ex-style
        kTimerWindowClassName,    // window class name
        L"OffScreenTimer", // window caption
        WS_POPUP,         // window style
        1,              // initial x position
        1,              // initial y position
        1,          // initial x size
        1,         // initial y size
        NULL,         // parent window handle
        NULL,           // window menu handle
        GetModuleHandleW(NULL),           // program instance handle
        NULL);         // creation parameters
    ::SetTimer(timerWindowHandle, (UINT_PTR)timerWindowHandle, 50, 0);
}

}

#endif // SharedTimerWin_h