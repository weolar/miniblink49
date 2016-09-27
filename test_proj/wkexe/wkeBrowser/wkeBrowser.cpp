
#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0500

#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>
#include <ShellAPI.h>

#include <wke.h>

#include "wkeBrowser.h"
#include "render.h"
#include "hyperlink.h"


class CTimer
{
public:
    void Start()
    {
        QueryPerformanceCounter(&m_StartCounter);
    }

    void End()
    {
        QueryPerformanceCounter(&m_EndCounter);
    }

    unsigned int GetCounter()
    {
        return m_EndCounter.LowPart - m_StartCounter.LowPart;
    }

    unsigned int GetTime()
    {
        LARGE_INTEGER freq;
        QueryPerformanceFrequency(&freq);

        return unsigned int( (long double)(m_EndCounter.QuadPart - m_StartCounter.QuadPart) / (long double)freq.QuadPart * 1000.f );
    }

private:
    LARGE_INTEGER m_StartCounter;
    LARGE_INTEGER m_EndCounter;
};


#define MAX_LOADSTRING 100
#define URLBAR_HEIGHT  24

HINSTANCE hInst;
HWND hMainWnd;
HWND hURLBarWnd = NULL;
HWND hViewWindow = NULL;
TCHAR* szTitle = L"wkeBrowserTitle";
TCHAR* szWindowClass = L"wkeBrowser";

wkeWebView g_webView = NULL;
CRender* g_render = NULL;

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

WNDPROC DefEditProc = NULL;
LRESULT CALLBACK UrlEditProc(HWND, UINT, WPARAM, LPARAM);

void resizeSubViews();
LRESULT CALLBACK WebViewWndProc(HWND, UINT, WPARAM, LPARAM);
const LPCWSTR wkeWebViewClassName = L"wkeWebView";
bool registerWebViewWindowClass();

jsValue JS_CALL js_msgBox(jsExecState es)
{
    wchar_t text[1025] = { 0 };
    wcsncpy_s(text, jsToTempStringW(es, jsArg(es, 0)), 1024);

    wchar_t title[1025] = { 0 };
    wcsncpy_s(title, jsToTempStringW(es, jsArg(es, 1)), 1024);

    MessageBoxW(hMainWnd, text, title, MB_OK);

    return jsUndefined();
}

static int s_testCount = 0;
jsValue JS_CALL js_getTestCount(jsExecState es)
{
    return jsInt(s_testCount);
}

jsValue JS_CALL js_setTestCount(jsExecState es)
{
    s_testCount = jsToInt(es, jsArg(es, 0));

    return jsUndefined();
}

void onTitleChanged(wkeWebView webView, void* param, const wkeString title)
{
    SetWindowTextW(hMainWnd, wkeGetStringW(title));
}

void onURLChanged(wkeWebView webView, void* param, const wkeString url)
{
    SetWindowTextW(hURLBarWnd, wkeGetStringW(url));
}

bool onNavigation(wkeWebView webView, void* param, wkeNavigationType type, const wkeString url_)
{
    const wchar_t* url = wkeGetStringW(url_);
    if (wcsstr(url, L"baidu.com") != NULL)
        return false;

    if (wcsstr(url, L"exec://") == url)
    {
        PROCESS_INFORMATION processInfo = { 0 };
        STARTUPINFOW startupInfo = { 0 };
        startupInfo.cb = sizeof(startupInfo);
        BOOL succeeded = CreateProcessW(NULL, (LPWSTR)url + 7, NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInfo);
        if (succeeded)
        {
            CloseHandle(processInfo.hProcess);
            CloseHandle(processInfo.hThread);
        }
        return false;
    }

    return true;
}

/*
## 测试绑定对象功能
运行wkeWebBrowser.exe，在地址栏输入`inject`回车，即可注册JS对象`test`，注册后就JS中可访问`test`对象的成员变量`value`和成员函数`msgbox`了。
在地址栏输入`javascript:test.msgbox('1')`测试调用成员函数。
在地址栏输入`javascript:document.write(test.value)`测试访问成员变量。
*/
class BindTestObject : public jsData
{
public:
	BindTestObject()
		: m_value(0)
	{
		jsData* data = this;
        memset(data, 0, sizeof(jsData));
        strcpy_s(data->typeName, "Object");
        data->propertyGet = js_getObjectProp;
        data->propertySet = js_setObjectProp;
        data->finalize = js_releaseObject;
	}

	void msgbox(const wchar_t* msg)
	{
		MessageBoxW(NULL, msg, NULL, MB_OK);
	}

protected:
	static bool js_setObjectProp(jsExecState es, jsValue object, const char* propertyName, jsValue value)
	{
		BindTestObject* pthis = (BindTestObject*)jsGetData(es, object);
		if (strcmp(propertyName, "value") == 0)
			return pthis->m_value = jsToInt(es, value), true;
		else
			return false;
	}

	static void js_releaseObject(jsData* data)
	{
		BindTestObject* pthis = (BindTestObject*)data;
		delete pthis;
	}

	class BindTestMsgbox : public jsData
	{
	public:
		BindTestMsgbox(BindTestObject* obj)
		{
			jsData* data = this;
			memset(data, 0, sizeof(jsData));
			strcpy_s(data->typeName, "Function");
			data->callAsFunction = js_callAsFunction;
			data->finalize = js_releaseFunction;

			m_obj = obj;
		}

	protected:
		static void js_releaseFunction(jsData* data)
		{
			BindTestMsgbox* pthis = (BindTestMsgbox*)data;
			delete pthis;
		}

		static jsValue js_callAsFunction(jsExecState es, jsValue object, jsValue* args, int argCount)
		{
			BindTestMsgbox* pthis = (BindTestMsgbox*)jsGetData(es, object);
    
            wchar_t text[1025] = { 0 };
            wchar_t title[1025] = { 0 };
            
			if (argCount >= 1)
				wcsncpy_s(text, jsToTempStringW(es, jsArg(es, 0)), 1024);
			if (argCount >= 2)
				wcsncpy_s(title, jsToTempStringW(es, jsArg(es, 1)), 1024);

			pthis->m_obj->msgbox(text);
			return jsInt(0);
		}

	protected:
		BindTestObject* m_obj;
	};

	static jsValue js_getObjectProp(jsExecState es, jsValue object, const char* propertyName)
	{
		BindTestObject* pthis = (BindTestObject*)jsGetData(es, object);
		if (strcmp(propertyName, "value") == 0)
			return jsInt(pthis->m_value);

		else if (strcmp(propertyName, "msgbox") == 0)
		{
			return jsFunction(es, new BindTestMsgbox(pthis));
		}
		else
			return jsUndefined();
	}

protected:
	int m_value;
};

//////////////////////////////////////////////////////////////////////////
//
#if 0

CURLM* m_curlMultiHandle = NULL;
CURLSH* m_curlShareHandle = NULL;

static void curl_lock_callback(CURL* handle, curl_lock_data data, curl_lock_access access, void* userPtr)
{
//     if (Mutex* mutex = sharedResourceMutex(data))
//         mutex->lock();
}

static void curl_unlock_callback(CURL* handle, curl_lock_data data, void* userPtr)
{
//     if (Mutex* mutex = sharedResourceMutex(data))
//         mutex->unlock();
}

void TestCURL() {
    curl_global_init(CURL_GLOBAL_ALL);
    m_curlMultiHandle = curl_multi_init();
    m_curlShareHandle = curl_share_init();
    curl_share_setopt(m_curlShareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
    curl_share_setopt(m_curlShareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
    curl_share_setopt(m_curlShareHandle, CURLSHOPT_LOCKFUNC, curl_lock_callback);
    curl_share_setopt(m_curlShareHandle, CURLSHOPT_UNLOCKFUNC, curl_unlock_callback);


}

#endif
//////////////////////////////////////////////////////////////////////////

extern int testMain();

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
    return testMain();

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow))
		return FALSE;

    CTimer t1, t2, t3;
    wkeInitialize();
    t1.Start();

    wkeSettings settings;
    memset(&settings, 0, sizeof(settings));

#if defined(WKE_BROWSER_USE_LOCAL_PROXY)
    settings.proxy.type = WKE_PROXY_SOCKS5;
    strcpy(settings.proxy.hostname, "127.0.0.1");
    settings.proxy.port = 1080;
    settings.mask |= WKE_SETTING_PROXY;
#endif
    //wkeInitializeEx(&settings);
    wkeConfigure(&settings);

    t1.End();

	//jsBindObject("testObj", js_getObjectProp, js_setObjectProp);
    jsBindFunction("msgBox", js_msgBox, 2);
    jsBindGetter("testCount", js_getTestCount);
    jsBindSetter("testCount", js_setTestCount);

    t2.Start();
    g_webView = wkeCreateWebView();
    t2.End();

    //////////////////////////////////////////////////////////////////////////
    //weolar
    BindTestObject* testObj = new BindTestObject();
    jsExecState es = wkeGlobalExec(g_webView);
    jsValue obj = jsObject(es, testObj);
    jsSetGlobal(es, "test", obj);
    //////////////////////////////////////////////////////////////////////////


    t3.Start();
    wkeSetTransparent(g_webView, false);
    wkeOnTitleChanged(g_webView, onTitleChanged, NULL);
    wkeOnURLChanged(g_webView, onURLChanged, NULL);
    wkeOnNavigation(g_webView, onNavigation, NULL);
    //wkeLoadUrl("file:///test/test.html");

    //设置UserAgent
    wkeSetUserAgent(g_webView, "Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/41.0.2228.0 Safari/537.36");

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (argc > 1)
        wkeLoadURLW(g_webView, argv[1]);
    else
        wkeLoadHTMLW(g_webView, L"<p style=\"background-color: #00FF00\">Testing</p><img id=\"webkit logo\" src=\"http://webkit.org/images/icon-gold.png\" alt=\"Face\"><div style=\"border: solid blue; background: white;\" contenteditable=\"true\">div with blue border</div><ul><li>foo<li>bar<li>baz</ul>");
    LocalFree(argv);
    t3.End();

    unsigned int ms1 = t1.GetTime();
    unsigned int ms2 = t2.GetTime();
    unsigned int ms3 = t3.GetTime();

    hURLBarWnd = CreateWindow(L"EDIT", 0,
        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOVSCROLL, 
        0, 0, 0, 0,
        hMainWnd,
        0,
        hInstance, 0);

    registerWebViewWindowClass();
    hViewWindow = CreateWindow(wkeWebViewClassName, 0, 
        WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        0, 0, 0, 0, 
        hMainWnd, 
        0, 
        hInstance, 0);

    resizeSubViews();

    DefEditProc = reinterpret_cast<WNDPROC>(GetWindowLongPtr(hURLBarWnd, GWL_WNDPROC));
    SetWindowLongPtr(hURLBarWnd, GWL_WNDPROC, reinterpret_cast<LONG_PTR>(UrlEditProc));
    SetFocus(hURLBarWnd);

    g_render = CRender::create(CRender::GDI_RENDER);
    g_render->init(hViewWindow);


	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WKEBROWSER));

    MSG msg;
    msg.message = WM_NULL;
	while (msg.message != WM_QUIT)
	{
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else if (!IsIconic(hMainWnd) && wkeIsDirty(g_webView))
        {
            g_render->render(g_webView);
        }
        else
        {
            Sleep(10);
        }
    }

    g_render->destroy();
    wkeDestroyWebView(g_webView);
    wkeFinalize();

	return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WKEBROWSER));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WKEBROWSER);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   hMainWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hMainWnd)
      return FALSE;

   ShowWindow(hMainWnd, nCmdShow);
   UpdateWindow(hMainWnd);

   return TRUE;
}

static int s_currentZoom = 100;
static int s_zoomLevels[] = {30, 50, 67, 80, 90, 100, 110, 120, 133, 150, 170, 200, 240, 300};

void zoom(bool zoomIn)
{
    if (!g_webView)
        return;

    int count = sizeof(s_zoomLevels) / sizeof(int);

    int i = 0;
    for (i = 0; i < count; ++i)
    {
        if (s_zoomLevels[i] == s_currentZoom)
            break;
    }

    if (zoomIn)
        i = i + 1;
    else
        i = i - 1;

    if (i < 0)
        i = 0;

    if (i >= count)
        i = count -1;

    s_currentZoom = s_zoomLevels[i];

    wkeSetZoomFactor(g_webView, s_currentZoom / 100.f);
}

void resetZoom()
{
    s_currentZoom = 100;
    if (g_webView)
        wkeSetZoomFactor(g_webView, s_currentZoom / 100.f);
}

void convertFilename(wchar_t* filename)
{
    int i;
    for (i = 0; filename[i]; ++i)
    {
        if( filename[i] == L'\\'
         || filename[i] == L'/'
         || filename[i] == L':'
         || filename[i] == L'*'
         || filename[i] == L'?'
         || filename[i] == L'\"'
         || filename[i] == L'<'
         || filename[i] == L'>'
         || filename[i] == L'|' )
         {
            filename[i] = L'_';
         }
    }
}

void saveBitmap(void* pixels, int w, int h, const wchar_t* title)
{
    BITMAPFILEHEADER fileHdr = {0};
    BITMAPINFOHEADER infoHdr = {0};
    FILE * fp = NULL;
    
    fileHdr.bfType = 0x4d42; //'BM'
    fileHdr.bfOffBits = sizeof(fileHdr) + sizeof(infoHdr);
    fileHdr.bfSize = w * h * 4 + fileHdr.bfOffBits;

    infoHdr.biSize = sizeof(BITMAPINFOHEADER);
    infoHdr.biWidth = w;
    infoHdr.biHeight = -h;
    infoHdr.biPlanes = 1;
    infoHdr.biBitCount = 32;
    infoHdr.biCompression = 0;
    infoHdr.biSizeImage = w * h * 4;
    infoHdr.biXPelsPerMeter = 3780;
    infoHdr.biYPelsPerMeter = 3780;

    struct tm t;
    time_t utc_time;
    time(&utc_time);
    localtime_s(&t, &utc_time);

    wchar_t name[1024];
    swprintf(name, 1024, L"%s_%4d%02d%02d_%02d%02d%02d.bmp", title,
        t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);

    convertFilename(name);

    wchar_t pathname[1024];
    swprintf(pathname, 1024, L"screenshots\\%s", name);
    _wmkdir(L"screenshots");
    _wfopen_s(&fp, pathname, L"wb");
    if (fp == NULL)
        return;

    fwrite(&fileHdr, sizeof(fileHdr), 1, fp);
    fwrite(&infoHdr, sizeof(infoHdr), 1, fp);
    fwrite(pixels, infoHdr.biSizeImage, 1, fp);
    fclose(fp);
}

void takeScreenshot()
{
    if (g_webView == NULL)
        return;

    wkeRunJS(g_webView, "document.body.style.overflow='hidden'");	
	int w = wkeGetContentWidth(g_webView);
	int h = wkeGetContentHeight(g_webView);
    
    int oldwidth = wkeGetWidth(g_webView);
    int oldheight = wkeGetHeight(g_webView);
    wkeResize(g_webView, w, h);
    wkeUpdate();

    void* pixels = malloc(w*h*4);
    wkePaint2(g_webView, pixels, 0);

    //save bitmap
    saveBitmap(pixels, w, h, wkeGetTitleW(g_webView));

    free(pixels);

    wkeResize(g_webView, oldwidth, oldheight);
    wkeRunJS(g_webView, "document.body.style.overflow='visible'");
}

void viewCookie()
{
    if (g_webView == NULL)
        return;

    const wchar_t* cookie = wkeGetCookieW(g_webView);
    MessageBoxW(NULL, cookie, L"Cookie", MB_OK|MB_ICONINFORMATION);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;

        case ID_FILE_GOBACK:
            if (g_webView)
                wkeGoBack(g_webView);
            break;

        case ID_FILE_GOFORWARD:
            if (g_webView)
                wkeGoForward(g_webView);
            break;

        case ID_ZOOM_IN:
            zoom(true);
            break;

        case ID_ZOOM_OUT:
            zoom(false);
            break;

        case ID_RESET_ZOOM:
            resetZoom();
            break;

        case ID_TAKE_SCREENSHOT:
            takeScreenshot();
            break;

        case ID_TOOLS_VIEWCOOKIE:
            viewCookie();
            break;

        case ID_SET_EDITABLE:
            if (g_webView)
            {
                HMENU hMenu = GetMenu(hWnd);
                UINT state = GetMenuState(hMenu, ID_SET_EDITABLE, MF_BYCOMMAND);
                if (state & MF_CHECKED)
                {
                    wkeSetEditable(g_webView, false);
                    CheckMenuItem(hMenu, ID_SET_EDITABLE, MF_BYCOMMAND | MF_UNCHECKED); 
                }
                else
                {
                    wkeSetEditable(g_webView, true);
                    CheckMenuItem(hMenu, ID_SET_EDITABLE, MF_BYCOMMAND | MF_CHECKED); 
                }
            }
            break;

        case ID_URL_SF:
            SetWindowText(hURLBarWnd, L"http://wke.sf.net");
            SendMessage(hURLBarWnd, WM_CHAR, L'\r', 0);
            break;

        case ID_URL_GITHUB:
            SetWindowText(hURLBarWnd, L"http://www.github.com/BlzFans/wke");
            SendMessage(hURLBarWnd, WM_CHAR, L'\r', 0);
            break;

		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

    case WM_INITMENU:
        {
            bool canGoBack = false;
            bool canGoForward = false;

            if (g_webView && wkeCanGoBack(g_webView))
                canGoBack = true;

            if (g_webView && wkeCanGoForward(g_webView))
                canGoForward = true;

            EnableMenuItem((HMENU)wParam, ID_FILE_GOBACK, canGoBack ? MF_ENABLED : MF_DISABLED);
            EnableMenuItem((HMENU)wParam, ID_FILE_GOFORWARD, canGoForward ? MF_ENABLED : MF_DISABLED);
        }
        break;


	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
        if (g_render) {
            g_render->dc();
            BitBlt(hdc, 0, 0, g_render->width(), g_render->height(), g_render->dc(), 0, 0, SRCCOPY);
        }
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

    case WM_SIZE:
        resizeSubViews();
        break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
        {
            RECT rect;
            rect.left = 63;
            rect.top = 40;
            rect.right = 166;
            rect.bottom = 58;
            CHyperlink::create(hDlg, rect, L"http://wke.sf.net", ID_URL_SF);

            rect.left = 63;
            rect.top = 60;
            rect.right = 290;
            rect.bottom = 78;
            CHyperlink::create(hDlg, rect, L"http://www.github.com/BlzFans/wke", ID_URL_GITHUB);
        }
        return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

LRESULT CALLBACK WebViewWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    bool handled = true;
	switch (message)
	{
    //cexer 增加拖放加载
    case WM_CREATE:
        {
            DragAcceptFiles(hWnd, TRUE);
        }
        return 0;

    case WM_DROPFILES:
        {
            if (g_webView)
            {
                wchar_t szFile[MAX_PATH + 8] = {0};
                wcscpy_s(szFile, L"file:///");

                HDROP hDrop = reinterpret_cast<HDROP>(wParam);

                UINT uFilesCount = DragQueryFileW(hDrop, 0xFFFFFFFF, szFile, MAX_PATH);
                if (uFilesCount != 0)
                {
                    UINT uRet = DragQueryFileW(hDrop, 0, (wchar_t*)szFile + 8, MAX_PATH);
                    if ( uRet != 0)
                    {
                        wkeLoadURLW(g_webView, szFile);
                        SetWindowTextW(hWnd, szFile);
                    }
                }
                DragFinish(hDrop);
            }
        }
        return 0;

    case WM_COMMAND:
        SendMessage(GetParent(hWnd), message, wParam, lParam);
        return 0;

    case WM_SIZE:
        if (g_webView && g_render)
        {
            wkeResize(g_webView, LOWORD(lParam), HIWORD(lParam));
            g_render->resize(LOWORD(lParam), HIWORD(lParam));
        }
        break;

    case WM_KEYDOWN:
        {
            unsigned int virtualKeyCode = wParam;
            unsigned int flags = 0;
            if (HIWORD(lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            //flags = HIWORD(lParam);

            handled = wkeFireKeyDownEvent(g_webView, virtualKeyCode, flags, false);
        }
        break;

    case WM_KEYUP:
        {
            unsigned int virtualKeyCode = wParam;
            unsigned int flags = 0;
            if (HIWORD(lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            //flags = HIWORD(lParam);

            handled = wkeFireKeyUpEvent(g_webView, virtualKeyCode, flags, false);
        }
        break;

    case WM_CHAR:
        {
            unsigned int charCode = wParam;
            unsigned int flags = 0;
            if (HIWORD(lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            //flags = HIWORD(lParam);

            handled = wkeFireKeyPressEvent(g_webView, charCode, flags, false);
        }
        break;

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
            if (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_RBUTTONDOWN)
            {
                SetFocus(hWnd);
                SetCapture(hWnd);
            }
            else if (message == WM_LBUTTONUP || message == WM_MBUTTONUP || message == WM_RBUTTONUP)
            {
                ReleaseCapture();
            }

            int x = GET_X_LPARAM(lParam);
            int y = GET_Y_LPARAM(lParam);

            unsigned int flags = 0;

            if (wParam & MK_CONTROL)
                flags |= WKE_CONTROL;
            if (wParam & MK_SHIFT)
                flags |= WKE_SHIFT;

            if (wParam & MK_LBUTTON)
                flags |= WKE_LBUTTON;
            if (wParam & MK_MBUTTON)
                flags |= WKE_MBUTTON;
            if (wParam & MK_RBUTTON)
                flags |= WKE_RBUTTON;

            //flags = wParam;

            handled = wkeFireMouseEvent(g_webView, message, x, y, flags);
        }
        break;

    case WM_CONTEXTMENU:
        {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);

            if (pt.x != -1 && pt.y != -1)
                ScreenToClient(hWnd, &pt);

            unsigned int flags = 0;

            if (wParam & MK_CONTROL)
                flags |= WKE_CONTROL;
            if (wParam & MK_SHIFT)
                flags |= WKE_SHIFT;

            if (wParam & MK_LBUTTON)
                flags |= WKE_LBUTTON;
            if (wParam & MK_MBUTTON)
                flags |= WKE_MBUTTON;
            if (wParam & MK_RBUTTON)
                flags |= WKE_RBUTTON;

            handled = wkeFireContextMenuEvent(g_webView, pt.x, pt.y, flags);
        }
        break;

    case WM_MOUSEWHEEL:
        {
            POINT pt;
            pt.x = GET_X_LPARAM(lParam);
            pt.y = GET_Y_LPARAM(lParam);
            ScreenToClient(hWnd, &pt);

            int delta = GET_WHEEL_DELTA_WPARAM(wParam);

            unsigned int flags = 0;

            if (wParam & MK_CONTROL)
                flags |= WKE_CONTROL;
            if (wParam & MK_SHIFT)
                flags |= WKE_SHIFT;

            if (wParam & MK_LBUTTON)
                flags |= WKE_LBUTTON;
            if (wParam & MK_MBUTTON)
                flags |= WKE_MBUTTON;
            if (wParam & MK_RBUTTON)
                flags |= WKE_RBUTTON;

            //flags = wParam;

            handled = wkeFireMouseWheelEvent(g_webView, pt.x, pt.y, delta, flags);
        }
        break;

    case WM_SETFOCUS:
        wkeSetFocus(g_webView);
        break;

    case WM_KILLFOCUS:
        wkeKillFocus(g_webView);
        break;

    case WM_IME_STARTCOMPOSITION:
        {
            wkeRect caret = wkeGetCaretRect(g_webView);

            CANDIDATEFORM form;
            form.dwIndex = 0;
            form.dwStyle = CFS_EXCLUDE;
            form.ptCurrentPos.x = caret.x;
            form.ptCurrentPos.y = caret.y + caret.h;
            form.rcArea.top = caret.y;
            form.rcArea.bottom = caret.y + caret.h;
            form.rcArea.left = caret.x;
            form.rcArea.right = caret.x + caret.w;

            HIMC hIMC = ImmGetContext(hWnd);
            ImmSetCandidateWindow(hIMC, &form);
            ImmReleaseContext(hWnd, hIMC);
        }
        break;
    case WM_SETCURSOR:
        handled = wkeFireWindowsMessage(g_webView, hWnd, WM_SETCURSOR, 0, 0, nullptr);
        break;

    default:
        handled = false;
        break;
	}
    
    if (!handled)
        return DefWindowProc(hWnd, message, wParam, lParam);

    return 0;
}

void resizeSubViews()
{
    if (hURLBarWnd && hViewWindow)
    {
        RECT rcClient;
        GetClientRect(hMainWnd, &rcClient);
        MoveWindow(hURLBarWnd, 0, 0, rcClient.right, URLBAR_HEIGHT, TRUE);
        MoveWindow(hViewWindow, 0, URLBAR_HEIGHT, rcClient.right, rcClient.bottom - URLBAR_HEIGHT, TRUE);

        wkeResize(g_webView, rcClient.right, rcClient.bottom - URLBAR_HEIGHT);
    }
}

bool registerWebViewWindowClass()
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_DBLCLKS;
    wcex.lpfnWndProc    = WebViewWndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInst;
    wcex.hIcon          = 0;
    wcex.hCursor        = LoadCursor(0, IDC_ARROW);
    wcex.hbrBackground  = 0;
    wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = wkeWebViewClassName;
    wcex.hIconSm        = 0;

    return !!RegisterClassEx(&wcex);
}

#define MAX_URL_LENGTH  1024

LRESULT CALLBACK UrlEditProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_CHAR && wParam == 13) //Enter Key
    {
		wchar_t url[MAX_URL_LENGTH] = {0};
        *((LPWORD)url) = MAX_URL_LENGTH; 
        int len = SendMessage(hDlg, EM_GETLINE, 0, (LPARAM)url);
        if (len == 0)
            return 0;

		if (wcsstr(url, L"inject") == url)
		{
			//jsObjectData* data = new jsObjectData();
   //         memset(data, 0, sizeof(jsObjectData));
   //         strcpy(data->typeName, "Test");
   //         data->propertyGet = js_getObjectProp;
   //         data->propertySet = js_setObjectProp;
   //         data->finalize = js_releaseObject;

			BindTestObject* testObj = new BindTestObject();
            jsExecState es = wkeGlobalExec(g_webView);
            jsValue obj = jsObject(es, testObj);
            jsSetGlobal(es, "test", obj);
		}
		else if (wcsstr(url, L"javascript:") == url)
		{
			url[len] = L'\0';
			wkeRunJSW(g_webView, url + wcslen(L"javascript:"));
		}
		else if (wcsstr(url, L"call") == url)
		{
			jsExecState es = wkeGlobalExec(g_webView);
			jsValue jsDocument = jsGet(es, jsGlobalObject(es), "document");

			{
				char prop[10] = { 0 };
				strcpy_s(prop, "URL");
				jsValue jsUrl = jsGet(es, jsDocument, prop);
				MessageBoxW(NULL, jsToTempStringW(es, jsUrl), NULL, MB_OK);
			}

			{
				char prop[10] = { 0 };
                strcpy_s(prop, "title");
				jsValue jsTitle = jsGet(es, jsDocument, prop);
				MessageBoxW(NULL, jsToTempStringW(es, jsTitle), NULL, MB_OK);
			}

			{
				char prop[10] = { 0 };
                strcpy_s(prop, "cookie");
				jsValue jsCookie = jsGet(es, jsDocument, prop);
				MessageBoxW(NULL, jsToTempStringW(es, jsCookie), NULL, MB_OK);
			}

		}
		else
		{
			url[len] = L'\0';
			wkeLoadURLW(g_webView, url);
			SetFocus(hViewWindow);
		}
        return 0;
    }
    
    return (LRESULT)CallWindowProc((WNDPROC)DefEditProc,hDlg,message,wParam,lParam);
}

