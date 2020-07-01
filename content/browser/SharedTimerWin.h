
#ifndef content_browser_SharedTimerWin_h
#define content_browser_SharedTimerWin_h

#include "content/web_impl_win/WebThreadImpl.h"
#include "content/browser/CheckReEnter.h"

#include <mmsystem.h>

// These aren't in winuser.h with the MSVS 2003 Platform SDK, 
// so use default values in that case.
#ifndef USER_TIMER_MINIMUM
#define USER_TIMER_MINIMUM 0x0000000A
#endif

#ifndef USER_TIMER_MAXIMUM
#define USER_TIMER_MAXIMUM 0x7FFFFFFF
#endif

#ifndef QS_RAWINPUT
#define QS_RAWINPUT         0x0400
#endif

typedef HANDLE (WINAPI* PfnCreateTimerQueue)();
typedef BOOL (WINAPI* PfnDeleteTimerQueueTimer)(HANDLE TimerQueue, HANDLE Timer, HANDLE CompletionEvent);
typedef BOOL (WINAPI* PfnCreateTimerQueueTimer)(PHANDLE phNewTimer, HANDLE TimerQueue, void* Callback, PVOID Parameter, DWORD DueTime, DWORD Period, ULONG Flags);

PfnCreateTimerQueue pCreateTimerQueue = nullptr;
PfnDeleteTimerQueueTimer pDeleteTimerQueueTimer = nullptr;
PfnCreateTimerQueueTimer pCreateTimerQueueTimer = nullptr;

namespace content {

const LPCWSTR kTimerWindowClassName = L"MiniBlinkTimerWindowClass";
 
static UINT timerID;
static void sharedTimerFiredFunction();

static HANDLE timer;
static HWND timerWindowHandle = 0;

static UINT timerFiredMessage = 0;
static HANDLE timerQueue;
static bool highResTimerActive;
static bool processingCustomTimerMessage = false;
static LONG pendingTimers;
static double lastIntervalInMS = 0;

const int timerResolution = 1; // To improve timer resolution, we call timeBeginPeriod/timeEndPeriod with this value to increase timer resolution to 1ms.
const int highResolutionThresholdMsec = 16; // Only activate high-res timer for sub-16ms timers (Windows can fire timers at 16ms intervals without changing the system resolution).
const int stopHighResTimerInMsec = 300; // Stop high-res timer after 0.3 seconds to lessen power consumption (we don't use a smaller time since oscillating between high and low resolution breaks timer accuracy on XP).

enum {
    sharedTimerID = 1000,
    endHighResTimerID = 1001,
};

static LRESULT CALLBACK TimerWindowWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (0 != CheckReEnter::getEnterCount())
        return 0;
    CheckReEnter::incrementEnterCount();

    LRESULT result = 0;
    if (message == WM_TIMER) {
        if (wParam == sharedTimerID) {
            //KillTimer(timerWindowHandle, sharedTimerID);
            sharedTimerFiredFunction();
        } else if (wParam == endHighResTimerID) {
            //KillTimer(timerWindowHandle, endHighResTimerID);
            highResTimerActive = false;
            timeEndPeriod(timerResolution);
        }
    } else if (message == timerFiredMessage) {
        InterlockedExchange(&pendingTimers, 0);
        processingCustomTimerMessage = true;
        sharedTimerFiredFunction();
        processingCustomTimerMessage = false;
    } else
        result = DefWindowProc(hWnd, message, wParam, lParam);

    CheckReEnter::decrementEnterCount();
    return result;
}

static void sharedTimerFiredFunction()
{
    content::WebThreadImpl* threadImpl = nullptr;
    threadImpl = (content::WebThreadImpl*)(blink::Platform::current()->currentThread());
    threadImpl->fire();
}

static void NTAPI queueTimerProc(PVOID, BOOLEAN)
{
    if (InterlockedIncrement(&pendingTimers) == 1)
        ::PostMessage(timerWindowHandle, timerFiredMessage, 0, 0);
}

static void initializeOffScreenTimerWindow();

void setSharedTimerFireInterval(double interval)
{
    unsigned intervalInMS;
    interval *= 1000;
    if (interval > USER_TIMER_MAXIMUM)
        intervalInMS = USER_TIMER_MAXIMUM;
    else
        intervalInMS = static_cast<unsigned>(interval);

    if (lastIntervalInMS == intervalInMS)
        return;
    lastIntervalInMS = intervalInMS;

    initializeOffScreenTimerWindow();
    bool timerSet = false;

    // shouldUseHighResolutionTimers begin
#if 0
    if (interval < highResolutionThresholdMsec) {
        if (!highResTimerActive) {
            highResTimerActive = true;
            timeBeginPeriod(timerResolution);
        }
        SetTimer(timerWindowHandle, endHighResTimerID, stopHighResTimerInMsec, 0);
    }

    DWORD queueStatus = LOWORD(GetQueueStatus(QS_PAINT | QS_MOUSEBUTTON | QS_KEY | QS_RAWINPUT));

    // Win32 has a tri-level queue with application messages > user input > WM_PAINT/WM_TIMER.

    // If the queue doesn't contains input events, we use a higher priorty timer event posting mechanism.
    if (!(queueStatus & (QS_MOUSEBUTTON | QS_KEY | QS_RAWINPUT))) {
        if (intervalInMS < USER_TIMER_MINIMUM && !processingCustomTimerMessage && !(queueStatus & QS_PAINT)) {
            // Call PostMessage immediately if the timer is already expired, unless a paint is pending.
            // (we prioritize paints over timers)
            if (InterlockedIncrement(&pendingTimers) == 1)
                PostMessage(timerWindowHandle, timerFiredMessage, 0, 0);
            timerSet = true;
        } else {
            // Otherwise, delay the PostMessage via a CreateTimerQueueTimer
            if (!timerQueue)
                timerQueue = ::pCreateTimerQueue();
            if (timer)
                ::pDeleteTimerQueueTimer(timerQueue, timer, 0);
            timerSet = ::pCreateTimerQueueTimer(&timer, timerQueue, queueTimerProc, 0, intervalInMS, 0, WT_EXECUTEINTIMERTHREAD | WT_EXECUTEONLYONCE);
        }
    }
    // shouldUseHighResolutionTimers end
#endif
    if (timerSet) {
        if (timerID) {
            ::KillTimer(timerWindowHandle, timerID);
            timerID = 0;
        }
    } else {
        timerID = SetTimer(timerWindowHandle, sharedTimerID, intervalInMS, 0);
        timer = 0;
    }
}

void stopSharedTimer()
{
    if (timerQueue && timer) {
        ::pDeleteTimerQueueTimer(timerQueue, timer, 0);
        timer = 0;
    }

    if (timerID) {
        ::KillTimer(timerWindowHandle, timerID);
        timerID = 0;
    }

    ::DestroyWindow(timerWindowHandle);
    ::UnregisterClassW(kTimerWindowClassName, NULL);
}

static void initializeOffScreenTimerWindow()
{
    if (timerWindowHandle)
        return;

    HMODULE hMod = GetModuleHandle(L"Kernel32.dll");
    pCreateTimerQueue = (PfnCreateTimerQueue)::GetProcAddress(hMod, "CreateTimerQueue");
    pDeleteTimerQueueTimer = (PfnDeleteTimerQueueTimer)::GetProcAddress(hMod, "DeleteTimerQueueTimer");
    pCreateTimerQueueTimer = (PfnCreateTimerQueueTimer)::GetProcAddress(hMod, "CreateTimerQueueTimer");

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
                       //::SetTimer(timerWindowHandle, (UINT_PTR)timerWindowHandle, 50, 0);

    timerFiredMessage = RegisterWindowMessage(L"com.weolar.Miniblink.TimerFired");
}

}

#endif // SharedTimerWin_h