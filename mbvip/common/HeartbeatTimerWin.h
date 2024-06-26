
namespace common {

static HANDLE s_timer;
static HWND s_timerWindowHandle = 0;
const LPCWSTR kTimerWindowClassName = L"MbVipTimerWindowClass";
static LONG s_pendingTimers = 0;
static UINT s_timerFiredMessage = 0;

static LRESULT CALLBACK timerWindowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
    if (message == s_timerFiredMessage) {
        InterlockedExchange(&s_pendingTimers, 0);
        DWORD threadId = ::GetCurrentThreadId();
        ThreadCall::runNoWait(threadId);
    } else
        result = ::DefWindowProc(hWnd, message, wParam, lParam);

    return result;
}

static void WINAPI heartbeatTimerProc()
{
    if (InterlockedIncrement(&s_pendingTimers) == 1)
        ::PostMessage(s_timerWindowHandle, s_timerFiredMessage, 0, 0);
}

void initializeHeartbeatTimerWindow() {
    if (s_timerWindowHandle)
        return;

    WNDCLASSEX wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.lpfnWndProc = timerWindowWndProc;
    wcex.hInstance = ::GetModuleHandle(NULL);
    wcex.lpszClassName = kTimerWindowClassName;
    ::RegisterClassEx(&wcex);

    s_timerWindowHandle = CreateWindowExW(
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
                       //::SetTimer(s_timerWindowHandle, (UINT_PTR)s_timerWindowHandle, 50, 0);

    s_timerFiredMessage = RegisterWindowMessage(L"com.weolar.MbVip.TimerFired");
}

}