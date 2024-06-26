
#include "RootWindow.h"

#include <time.h> 
#include <windows.h>
#include <string>
#include <vector>
#include <time.h>
#include <process.h>
#include <shlobj.h>
#include <propvarutil.h>
#include <wininet.h>
#include <CommCtrl.h>
#include <tuple>
#include <crtdbg.h>
#include <psapi.h>
#include <iosfwd>
#include <sstream>
#include <set>
#include <TlHelp32.h>
#include <xmmintrin.h>

// #define VLD_FORCE_ENABLE 1
// #include "C:\\Program Files (x86)\\Visual Leak Detector\\include\\vld.h"

#pragma comment(lib,"Imm32.lib")

void testSynMain();
void testGuard();
int winMainPrint();
int APIENTRY wWinMainIE(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);
int APIENTRY wWinMain2(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow);

#define CLS_WINDOW L"mbTestWindow"
void readFile(const wchar_t* path, std::vector<char>* buffer);

LRESULT WINAPI testWindowProc(
    __in HWND hWnd,
    __in UINT msg,
    __in WPARAM wParam,
    __in LPARAM lParam)
{
    LRESULT result = 0;
    mbWebView view = (mbWebView)::GetProp(hWnd, L"mb");
    if (!view)
        return ::DefWindowProc(hWnd, msg, wParam, lParam);

    switch (msg) {
    case WM_NCDESTROY:
        if (::GetProp(hWnd, L"subView")) {
            RemoveProp(hWnd, L"subView");
        } else {
            printf("Destroying\n");
            mbDestroyWebView(view);
            printf("Destroyed\n");
            PostQuitMessage(0);
        }
        return 0;

    case WM_ERASEBKGND:
        return TRUE;

    case WM_PAINT:
    {
        if (WS_EX_LAYERED == (WS_EX_LAYERED & GetWindowLong(hWnd, GWL_EXSTYLE)))
            break;
        //mbRepaintIfNeeded(view);

        PAINTSTRUCT ps = { 0 };
        HDC hdc = ::BeginPaint(hWnd, &ps);

        RECT rcClip = ps.rcPaint;

        RECT rcClient;
        ::GetClientRect(hWnd, &rcClient);

        RECT rcInvalid = rcClient;
        if (rcClip.right != rcClip.left && rcClip.bottom != rcClip.top)
            ::IntersectRect(&rcInvalid, &rcClip, &rcClient);

        int srcX = rcInvalid.left - rcClient.left;
        int srcY = rcInvalid.top - rcClient.top;
        int destX = rcInvalid.left;
        int destY = rcInvalid.top;
        int width = rcInvalid.right - rcInvalid.left;
        int height = rcInvalid.bottom - rcInvalid.top;

        if (0 != width && 0 != height) {
            HDC hMbDC = mbGetLockedViewDC(view);
            ::BitBlt(hdc, destX, destY, width, height, hMbDC, srcX, srcY, SRCCOPY);
            mbUnlockViewDC(view);
        }

        ::EndPaint(hWnd, &ps);
        return 1;
        break;
    }
    case WM_SIZE:
    {
        RECT rc = { 0 };
        ::GetClientRect(hWnd, &rc);
        int width = rc.right - rc.left;
        int height = rc.bottom - rc.top;

        ::mbResize(view, width, height);
        // mbRepaintIfNeeded(view);
        ::mbWake(view);

        return 0;
    }
    case WM_KEYDOWN:
    {
        unsigned int virtualKeyCode = wParam;
        unsigned int flags = 0;
        if (HIWORD(lParam) & KF_REPEAT)
            flags |= MB_REPEAT;
        if (HIWORD(lParam) & KF_EXTENDED)
            flags |= MB_EXTENDED;

        if (mbFireKeyDownEvent(view, virtualKeyCode, flags, false))
            return 0;
        break;
    }
    case WM_KEYUP:
    {
        unsigned int virtualKeyCode = wParam;
        unsigned int flags = 0;
        if (HIWORD(lParam) & KF_REPEAT)
            flags |= MB_REPEAT;
        if (HIWORD(lParam) & KF_EXTENDED)
            flags |= MB_EXTENDED;

        if (mbFireKeyUpEvent(view, virtualKeyCode, flags, false))
            return 0;
        break;
    }
    case WM_CHAR:
    {
        unsigned int charCode = wParam;
        unsigned int flags = 0;
        if (HIWORD(lParam) & KF_REPEAT)
            flags |= MB_REPEAT;
        if (HIWORD(lParam) & KF_EXTENDED)
            flags |= MB_EXTENDED;

        if (mbFireKeyPressEvent(view, charCode, flags, false))
            return 0;
        break;
    }
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_MBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE:
    {
        if (msg == WM_LBUTTONDOWN || msg == WM_MBUTTONDOWN || msg == WM_RBUTTONDOWN) {
            if (::GetFocus() != hWnd)
                ::SetFocus(hWnd);
            ::SetCapture(hWnd);
        } else if (msg == WM_LBUTTONUP || msg == WM_MBUTTONUP || msg == WM_RBUTTONUP) {
            ReleaseCapture();
        }

        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

        unsigned int flags = 0;

        if (wParam & MK_CONTROL)
            flags |= MB_CONTROL;
        if (wParam & MK_SHIFT)
            flags |= MB_SHIFT;

        if (wParam & MK_LBUTTON)
            flags |= MB_LBUTTON;
        if (wParam & MK_MBUTTON)
            flags |= MB_MBUTTON;
        if (wParam & MK_RBUTTON)
            flags |= MB_RBUTTON;

        if (mbFireMouseEvent(view, msg, x, y, flags))
            return 0;
        break;
    }
    case WM_CONTEXTMENU:
    {
        POINT pt;
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);

        if (pt.x != -1 && pt.y != -1)
            ScreenToClient(hWnd, &pt);

        unsigned int flags = 0;

        if (wParam & MK_CONTROL)
            flags |= MB_CONTROL;
        if (wParam & MK_SHIFT)
            flags |= MB_SHIFT;

        if (wParam & MK_LBUTTON)
            flags |= MB_LBUTTON;
        if (wParam & MK_MBUTTON)
            flags |= MB_MBUTTON;
        if (wParam & MK_RBUTTON)
            flags |= MB_RBUTTON;

        if (mbFireContextMenuEvent(view, pt.x, pt.y, flags))
            return 0;
        break;
    }
    case WM_MOUSEWHEEL:
    {
        POINT pt;
        pt.x = LOWORD(lParam);
        pt.y = HIWORD(lParam);
        ::ScreenToClient(hWnd, &pt);

        int delta = GET_WHEEL_DELTA_WPARAM(wParam);

        unsigned int flags = 0;

        if (wParam & MK_CONTROL)
            flags |= MB_CONTROL;
        if (wParam & MK_SHIFT)
            flags |= MB_SHIFT;

        if (wParam & MK_LBUTTON)
            flags |= MB_LBUTTON;
        if (wParam & MK_MBUTTON)
            flags |= MB_MBUTTON;
        if (wParam & MK_RBUTTON)
            flags |= MB_RBUTTON;

        if (mbFireMouseWheelEvent(view, pt.x, pt.y, delta, flags))
            return 0;
        break;
    }
    case WM_SETFOCUS:
        mbSetFocus(view);
        return 0;

    case WM_KILLFOCUS:
        mbKillFocus(view);
        return 0;

    case WM_SETCURSOR:
        if (mbFireWindowsMessage(view, hWnd, WM_SETCURSOR, 0, 0, &result))
            return result;
        break;

    case WM_IME_STARTCOMPOSITION: {
        if (mbFireWindowsMessage(view, hWnd, WM_IME_STARTCOMPOSITION, 0, 0, &result))
            return result;
        break;
        }
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

BOOL RegWndClass(LPCTSTR lpcsClassName, DWORD dwStyle)
{
    WNDCLASS wndclass = { 0 };

    wndclass.style = dwStyle;
    wndclass.lpfnWndProc = testWindowProc;
    wndclass.cbClsExtra = 200;
    wndclass.cbWndExtra = 200;
    wndclass.hInstance = ::GetModuleHandle(NULL);
    wndclass.hIcon = NULL;
    //wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = lpcsClassName;

    ::RegisterClass(&wndclass);
    return TRUE;
}

BOOL UnregWndClass(LPCTSTR lpcsClassName)
{
    ::UnregisterClass(lpcsClassName, ::GetModuleHandle(NULL));
    return TRUE;
}

void MB_CALL_TYPE handlePaintUpdatedCallback(mbWebView webView, void* param, const HDC hdc, int x, int y, int cx, int cy)
{
    HWND hWnd = (HWND)param;
    BOOL callOk = FALSE;
    if (WS_EX_LAYERED == (WS_EX_LAYERED & GetWindowLong(hWnd, GWL_EXSTYLE))) {
        RECT rectDest;
        ::GetWindowRect(hWnd, &rectDest);

        SIZE sizeDest = { rectDest.right - rectDest.left, rectDest.bottom - rectDest.top };
        POINT pointDest = { 0, 0 }; // { rectDest.left, rectDest.top };
        POINT pointSource = { 0, 0 };

        BITMAP bmp = { 0 };
        HBITMAP hBmp = (HBITMAP)::GetCurrentObject(hdc, OBJ_BITMAP);
        ::GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp);

        sizeDest.cx = bmp.bmWidth;
        sizeDest.cy = bmp.bmHeight;

        HDC hdcScreen = GetDC(hWnd);

        BLENDFUNCTION blend = { 0 };
        blend.BlendOp = AC_SRC_OVER;
        blend.SourceConstantAlpha = 255;
        blend.AlphaFormat = AC_SRC_ALPHA;
        callOk = ::UpdateLayeredWindow(hWnd, hdcScreen, nullptr, &sizeDest, hdc, &pointSource, RGB(0xFF, 0xFF, 0xFF), &blend, ULW_ALPHA);
        if (!callOk) {
            HDC hdcMemory = ::CreateCompatibleDC(hdcScreen);
            HBITMAP hbmpMemory = ::CreateCompatibleBitmap(hdcScreen, sizeDest.cx, sizeDest.cy);
            HBITMAP hbmpOld = (HBITMAP)::SelectObject(hdcMemory, hbmpMemory);

            ::BitBlt(hdcMemory, 0, 0, sizeDest.cx, sizeDest.cy, hdc, 0, 0, SRCCOPY | CAPTUREBLT);

            ::BitBlt(hdc, 0, 0, sizeDest.cx, sizeDest.cy, hdcMemory, 0, 0, SRCCOPY | CAPTUREBLT); //!

            callOk = ::UpdateLayeredWindow(hWnd, hdcScreen, nullptr, &sizeDest, hdcMemory, &pointSource, RGB(0xFF, 0xFF, 0xFF), &blend, ULW_ALPHA);

            ::SelectObject(hdcMemory, (HGDIOBJ)hbmpOld);
            ::DeleteObject((HGDIOBJ)hbmpMemory);
            ::DeleteDC(hdcMemory);
        }

        ::ReleaseDC(hWnd, hdcScreen);
    } else {
        RECT rc = { x, y, x + cx, y + cy };
        ::InvalidateRect(hWnd, &rc, FALSE);
    }
}

void MB_CALL_TYPE handleDocumentReady(mbWebView webView, void* param, mbWebFrameHandle frameId)
{
}

void MB_CALL_TYPE handleLoadingFinish(mbWebView webView, void* param, mbWebFrameHandle frameId, const utf8* url, mbLoadingResult result, const utf8* failedReason)
{
    //printf("HandleLoadingFinish url:%s result:%d\n", ::mbToString(url), result);
    //if(result == MB_LOADING_SUCCEEDED)
    //::mbNetGetFavicon(webView, HandleFaviconReceived, param);
    OutputDebugStringA("handleLoadingFinish \n");
}

mbWebView MB_CALL_TYPE handleCreateView(mbWebView webView, void* param, mbNavigationType navigationType, const utf8* url, const mbWindowFeatures* windowFeatures)
{
    mbWebView view = mbCreateWebView();
    HWND hWnd = ::CreateWindowEx(WS_EX_APPWINDOW, CLS_WINDOW, NULL, WS_OVERLAPPEDWINDOW | WS_VISIBLE, windowFeatures->x, windowFeatures->y, windowFeatures->width, windowFeatures->height, NULL, NULL, ::GetModuleHandle(NULL), NULL);
    ::SetProp(hWnd, L"mb", (HANDLE)view);
    ::SetProp(hWnd, L"subView", (HANDLE)TRUE);
    ::mbSetHandle(view, hWnd);
    ::mbOnPaintUpdated(view, handlePaintUpdatedCallback, hWnd);
    ::mbOnLoadingFinish(view, handleLoadingFinish, (void*)view);
    ::mbOnCreateView(view, handleCreateView, (void*)view);
    ::mbSetNavigationToNewWindowEnable(view, true);
    ::mbSetCspCheckEnable(view, true);

    RECT rc = { 0 };
    ::GetClientRect(hWnd, &rc);
    ::mbResize(view, rc.right, rc.bottom);

    //mbShowWindow(view, TRUE);
    return view;
}

void MB_CALL_TYPE onJsQuery(mbWebView webView, void* param, mbJsExecState es, int64_t queryId, int customMsg, const utf8* request)
{
    mbResponseQuery(webView, queryId, customMsg, request/*"I am response"*/);
}

mbWebView g_viewForReload;

void WINAPI onTimerReload(HWND, UINT, UINT_PTR, DWORD)
{
    //mbReload(g_viewForReload);
    //::ExitProcess(11);
}

void MB_CALL_TYPE onPaintBitUpdatedCallback(mbWebView webView, void* param, const void* buffer, const mbRect* r, int width, int height);

void createSimpleMb()
{
    //mbWebView view = mbCreateWebWindow(MB_WINDOW_TYPE_POPUP, NULL, 0, 0, 840, 680);

    mbWebView view = mbCreateWebView();
    HWND hWnd = ::CreateWindowEx(/*WS_EX_APPWINDOW*/0, CLS_WINDOW, NULL, WS_OVERLAPPEDWINDOW, 0, 0, 840, 680, NULL, NULL, ::GetModuleHandle(NULL), NULL);
    ::SetProp(hWnd, L"mb", (HANDLE)view);
    ::mbSetTransparent(view, true);
    ::mbSetHandle(view, hWnd);
    ::mbOnPaintUpdated(view, handlePaintUpdatedCallback, hWnd);
    ::mbOnPaintBitUpdated(view, onPaintBitUpdatedCallback, nullptr);

//     ::mbOnDocumentReady(view, handleDocumentReady, (void*)view);
//     ::mbOnLoadingFinish(view, handleLoadingFinish, (void*)view);
//     ::mbOnCreateView(view, handleCreateView, (void*)view);
//     ::mbSetNavigationToNewWindowEnable(view, true);
//     ::mbSetCspCheckEnable(view, false);

    hWnd = mbGetHostHWND(view);
    RECT rc = { 0 };
    ::GetClientRect(hWnd, &rc);
    ::mbResize(view, rc.right, rc.bottom);

    mbMoveToCenter(view);
    mbShowWindow(view, TRUE);
    ::ShowWindow(hWnd, SW_SHOW);
    //mbSetHeadlessEnabled(view, TRUE);

    g_viewForReload = view;
    ::SetTimer(hWnd, (UINT_PTR)hWnd, 5000, onTimerReload);

    //::mbLoadHTML(view, "<html><head><style></style><script type=\"text/javascript\">var test = 'test';</script></head><body></body></html>");
    //https://www.baidu.com/s?wd=123

    char* randUrl = (char*)malloc(0x100);
    sprintf_s(randUrl, 0x99, "https://www.baidu.com/s?wd=%d", ::GetTickCount());

    ::mbLoadURL(view, "file:///G:/mycode/mbvip/out/x86/Release/1234.htm");
//     //::mbLoadURL(view, "file:///C:/Users/weo/AppData/Local/AllMobilize/parent.htm");
//     //::mbLoadURL(view, "file:///E:/mycode/mtmb/Debug/guiji.htm");
//     //::mbLoadURL(view, "https://passport.csdn.net/account/login");//http://www.17sucai.com/pins/demo-show?id=23150
// 
//     mbOnJsQuery(view, onJsQuery, (void*)1);

    free(randUrl);
}

void createMbClient()
{
    RootWindow* rootWin = new RootWindow();
    rootWin->createRootWindow();
}

void SearchHex()
{
    unsigned char code[] = {
        0x48,0x31,0xE0,// xor rax,rsp
        0x48, 0x89, 0x44, 0x24, 0x48,//       mov         qword ptr[rsp + 48h],rax };
        0x48, 0x8B, 0x81, 0x50, 0x04, 0x00, 0x00,// mov         rax, qword ptr[rcx + 450h]
        0x8B, 0x08,//                mov         ecx, dword ptr[rax]      
        0x89, 0x4C, 0x24, 0x30,//          mov         dword ptr[rsp + 30h], ecx     
        0x48, 0x8B, 0x48, 0x08,//          mov         rcx, qword ptr[rax + 8]      
        0x48, 0x89, 0x4C, 0x24, 0x38,//       mov         qword ptr[rsp + 38h], rcx
        0x48, 0x85, 0xC9,// test        rcx, rcx
    };

    const wchar_t* path = L"E:\\chroium\\M105\\src\\out\\rx64\\chrome.dll";
    HMODULE hMod = LoadLibraryW(path);
    //wkeInitialize();

    std::vector<char> buffer;
    readFile(path, &buffer);
    for (size_t i = 0; i < buffer.size(); ++i) {
        //unsigned char* addr = (unsigned char*)buffer.data();
        unsigned char* addr = (unsigned char*)hMod;
        addr += i;

        bool notFind = false;
        for (size_t j = 0; j < sizeof(code); ++j) {
            if (addr[j] != code[j]) {
                notFind = true;
                break;
            }
        }
        if (!notFind) {
            char* output = (char*)malloc(0x100);
            sprintf_s(output, 0x99, "SearchHex find: 0x%p\n", (addr));
            OutputDebugStringA(output);
            free(output);
        }
    }
    OutputDebugStringA("SearchHex end\n");
}


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
//     SearchHex();
//     return 0;

    RegWndClass(CLS_WINDOW, CS_HREDRAW | CS_VREDRAW);
    
    mbSettings* settings = new mbSettings();
    memset(settings, 0, sizeof(settings));

    settings->mask |= MB_SETTING_PROXY;
    settings->proxy.type = MB_PROXY_HTTP;
    strcpy(settings->proxy.hostname, "proxy.tigerproxy.top");
    settings->proxy.port = 52222;
    strcpy(settings->proxy.username, "customer-");
    strcpy(settings->proxy.password, "HGFoyZeVt");

    //settings->mask = MB_ENABLE_NODEJS;
    //settings->mask = MB_ENABLE_DISABLE_CC;
    //settings->mask |= MB_ENABLE_DISABLE_H5VIDEO;
    //settings->mask |= MB_ENABLE_ENABLE_SWIFTSHAER;
    //settings->mask |= MB_ENABLE_ENABLE_EGLGLES2;
    settings->version = kMbVersion;
    //settings->mainDllPath = L"E:\\mycode\\mtmb\\Debug\\node.dll";

#ifdef _DEBUG
    mbSetMbDllPath(L"miniblink_d.dll");
#else
# ifdef _WIN64
    mbSetMbDllPath(L"miniblink_4975_x64.dll");
# else
#  ifdef MB_V857
    mbSetMbDllPath(L"miniblink_4957_x32.dll");
#  else
    mbSetMbDllPath(L"miniblink_4975_x32.dll");
#  endif    
# endif
#endif
    
    //mbSetMbMainDllPath(L"G:\\mycode\\mb\\out\\Debug\\node.dll");

    //settings->mainDllPath = L"node_v8_7_5.dll";
    //mbSetMbMainDllPath(L"node_v8_7_5.dll");

//     mbFillFuncPtr();
//     mbSettings* settings = mbCreateInitSettings();
//     mbSetInitSettings(settings, "DisableCC", nullptr);
    mbInit(settings);
    
    //mbEnableHighDPISupport();
    mbSetNpapiPluginsEnabled(NULL_WEBVIEW, true);

//     mbDefaultPrinterSettings printerSettings;
//     printerSettings.copies = 2;
//     printerSettings.paperType = 3;
//     printerSettings.isLandscape = TRUE;
//     mbUtilSetDefaultPrinterSettings(NULL_WEBVIEW, &printerSettings);

    createMbClient();
    //createSimpleMb();
    
//     MSG msg = { 0 };
//     while (true) {
//         if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
//             if (g_quitCount > 0) {
//                 ++g_quitCount;
//                 
//             }
//             if (g_quitCount > 100) {
//                 ::PostThreadMessage(::GetCurrentThreadId(), WM_QUIT, 0, 0);
//             }
//             if (WM_QUIT == msg.message)
//                 break;                
// 
//             ::TranslateMessage(&msg);
//             ::DispatchMessageW(&msg);
//         }
//         ::mbWake(NULL_WEBVIEW);
//         ::Sleep(5);
//     }

    //mbRunMessageLoop();

    MSG msg = { 0 };
    while (::GetMessageW(&msg, NULL, 0, 0)) {
        ::TranslateMessage(&msg);
        ::DispatchMessageW(&msg);

        ::mbWake(NULL_WEBVIEW);
    }
    
#ifdef _DEBUG
    mbUninit();
#endif

    //VLDReportLeaks();

    UnregWndClass(CLS_WINDOW);

    return 0;
}
