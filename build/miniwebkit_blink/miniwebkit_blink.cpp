// miniwebkit_blink.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "config.h"
#include "miniwebkit_blink.h"

#include "content/browser/WebPage.h"

#include <Shlwapi.h>
#include <vector>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
LPWSTR szTitle;					// The title bar text
const WCHAR* szWindowClass = L"MINIWEBKIT_BLINK";			// the main window class name

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

static bool GetUrlDataFromFile(
    /*in*/ LPCWSTR Url,
    /*in*/ LPWSTR lpPath
    )
{
    HANDLE        hFile = NULL;
    DWORD         bytesReaded = 0;
    UINT          DataSize = 8;
    bool          bRet = false;
    LARGE_INTEGER FileSize = { 0 };

    hFile = CreateFileW(Url, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
    if (!hFile || INVALID_HANDLE_VALUE == hFile)
        return false;

    bRet = GetFileSizeEx(hFile, &FileSize);
    if (!bRet || (0 == FileSize.HighPart && 0 == FileSize.LowPart))
        goto Exit0;

    memset(lpPath, 0, sizeof(WCHAR) * MAX_PATH);
    if (!::ReadFile(hFile, lpPath, FileSize.LowPart, &bytesReaded, NULL))
        goto Exit0;

    bRet = true;

Exit0:
    if (hFile)
        ::CloseHandle(hFile);

    return bRet;
}

// int APIENTRY _tWinMain(HINSTANCE hInstance,
//                      HINSTANCE hPrevInstance,
//                      LPTSTR    lpCmdLine,
//                      int       nCmdShow)
// {
// 	UNREFERENCED_PARAMETER(hPrevInstance);
// 	UNREFERENCED_PARAMETER(lpCmdLine);
// 
// 	MSG msg;
// 	HACCEL hAccelTable;
// 
//     szTitle = L"blinkÄ£ÄâÆ÷";
// 	LoadString(hInstance, IDC_MINIWEBKIT_BLINK, szWindowClass, MAX_LOADSTRING);
// 	MyRegisterClass(hInstance);
// 
// 	// Perform application initialization:
//     if (!InitInstance(hInstance, lpCmdLine, nCmdShow))
// 		return FALSE;
// 
// 	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MINIWEBKIT_BLINK));
// 
// 	// Main message loop:
// 	while (GetMessage(&msg, NULL, 0, 0)) {
// 		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
// 			TranslateMessage(&msg);
// 			DispatchMessage(&msg);
// 		}
// 	}
// 
// 	return (int) msg.wParam;
// }

ATOM MyRegisterClass(HINSTANCE hInstance)
{
// 	WNDCLASSEX wcex;
// 
// 	wcex.cbSize = sizeof(WNDCLASSEX);
// 
// 	wcex.style			= CS_HREDRAW | CS_VREDRAW;
// 	wcex.lpfnWndProc	= WndProc;
// 	wcex.cbClsExtra		= 0;
// 	wcex.cbWndExtra		= 0;
// 	wcex.hInstance		= hInstance;
// 	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MINIWEBKIT_BLINK));
// 	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
// 	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
// 	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MINIWEBKIT_BLINK);
// 	wcex.lpszClassName	= szWindowClass;
// 	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
// 
// 	return RegisterClassEx(&wcex);
    DebugBreak();
    return 0;
}

static void ConvPath(/* in out */ LPWSTR lpPath)
{
    int len = wcslen(lpPath);
    for (int i = 0; i < len; ++i) {
        if (L'\\' == lpPath[i])
            lpPath[i] = L'/';
    }
}

void GetUrlFromFile(HINSTANCE hInstance, LPTSTR lpCmdLine, std::vector<WCHAR>& szPath)
{
    std::vector<WCHAR> szModPath;
    szModPath.resize(MAX_PATH + 1);
    ZeroMemory(szModPath.data(), sizeof(WCHAR)*(MAX_PATH + 1));

    szPath.resize(MAX_PATH + 1);
    ZeroMemory(szPath.data(), sizeof(WCHAR)*(MAX_PATH + 1));

    GetModuleFileNameW(hInstance, szModPath.data(), MAX_PATH);
    LPWSTR pPath = L"";

    pPath = lpCmdLine;
    pPath++;

    if (0 != wcsnicmp(pPath, L"file:///", 8)) {
        PathRemoveFileSpecW(szModPath.data());
        wcscpy(szPath.data(), szModPath.data());
        PathAppendW(szPath.data(), L"main.txt");
        GetUrlDataFromFile(szPath.data(), szPath.data());
        if (!PathMatchSpecW(szPath.data(), L"*:*")) {
            GetModuleFileNameW(hInstance, szModPath.data(), MAX_PATH);
            PathRemoveFileSpecW(szModPath.data());
            PathAppendW(szModPath.data(), szPath.data());
            pPath = szModPath.data();
        } else {
            pPath = szPath.data();
        }
    } else {
        wcscpy(szPath.data(), pPath);
        pPath = szPath.data();
        pPath[wcslen(pPath) - 1] = 0;
    }

    ConvPath(pPath);
}

BOOL InitInstance(HINSTANCE hInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    std::vector<WCHAR> szPath;
    GetUrlFromFile(hInstance, lpCmdLine, szPath);
    HWND hWnd;

    content::WebPage::initBlink();

    content::WebPage* webPage = new content::WebPage(NULL);
    
    hInst = hInstance; // Store instance handle in our global variable

    hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        460, 250, 1008, 691, NULL, NULL, hInstance, webPage);

    if (!hWnd)
        return FALSE;
    
    //webPage->loadFormUrl(szPath.data(), -1);
    //webPage->loadFormUrl("http://www.baidu.com/");
    //webPage->loadFormUrl("file:///C:/Users/debugwang/Desktop/test/page_bad.html");
    //webPage->loadFormUrl("file:///C:/Users/debugwang/Desktop/test/page51535651_1.html");

    ::ShowWindow(hWnd, nCmdShow);
    ::UpdateWindow(hWnd);
    ::SetTimer(hWnd, 10010, 50, NULL);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
    content::WebPage* webPage = 0;
    if (message == WM_NCCREATE) {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        webPage = static_cast<content::WebPage*>(lpcs->lpCreateParams);
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(webPage));

        webPage->init(hWnd);
    } else {
        webPage = reinterpret_cast<content::WebPage*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        // if( message == WM_NCDESTROY && webPage != NULL )
        //             return 0;
    }

    BOOL bHandle = FALSE;
    LRESULT lResult = 0;

	switch (message) {
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId) {
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
        webPage->firePaintEvent(hWnd, message, wParam, lParam);
		break;

    case WM_TIMER:
        webPage->fireTimerEvent();
        break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
    case WM_SIZE:
        webPage->fireResizeEvent(hWnd, message, wParam, lParam);
        break;
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_XBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE:
    case WM_MOUSELEAVE:
        webPage->fireMouseEvent(hWnd, message, wParam, lParam, nullptr);
        break;
    case WM_SETCURSOR:
        lResult = webPage->fireCursorEvent(hWnd, message, wParam, lParam, &bHandle);
        if (bHandle)
            return lResult;
        break;
    case WM_MOUSEWHEEL:
        lResult = webPage->fireWheelEvent(hWnd, message, wParam, lParam);
        break;
	}

    if (!bHandle)
        return DefWindowProc(hWnd, message, wParam, lParam);
    return lResult;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message) {
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
