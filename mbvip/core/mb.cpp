
#define _CRT_SECURE_NO_WARNINGS 1

//#define ENABLE_IN_MB_MAIN

#include "core/mb.h"
#include "core/MbWebView.h"
#include "wke/wkedefine.h"
#include "common/ThreadCall.h"
#include "common/StringUtil.h"
#include "common/LiveIdDetect.h"
#include "common/Util.h"
#if ENABLE_IN_MB_MAIN
#include "verify/SqBind.h"
#endif // ENABLE_IN_MB_MAIN

#include "printing/PdfViewerPlugin.h"
#include "printing/Printing.h"
#include "printing/PrintingSetting.h"
#include "download/SimpleDownload.h"

#include "content/browser/PostTaskHelper.h"

#include <vector>
#include <stdio.h>
#include <Shlwapi.h>
#include <tlhelp32.h>

bool g_mbIsInit = false;
DWORD g_uiThreadId = 0;
extern HMODULE g_hModule;

namespace mb {
unsigned int g_mbMask = 0;
bool g_enableNativeSetCapture = true;
bool g_enableNativeSetFocus = true;
}

mbStringPtr MB_CALL_TYPE mbCreateString(const utf8* str, size_t length)
{
    return (mbStringPtr)wkeCreateString(str, length);
}

mbStringPtr MB_CALL_TYPE mbCreateStringWithoutNullTermination(const utf8* str, size_t length)
{
    return (mbStringPtr)wkeCreateStringWithoutNullTermination(str, length);
}

void MB_CALL_TYPE mbDeleteString(mbStringPtr str)
{
    wkeDeleteString((wkeString)str);
}

size_t MB_CALL_TYPE mbGetStringLen(mbStringPtr str)
{
    return wkeGetStringLen((wkeString)str);
}

const utf8* MB_CALL_TYPE mbGetString(mbStringPtr str)
{
    return wkeGetString((wkeString)str);
}

static bool checkThreadCallIsValidImpl(const char* funcName, bool isBlinkThread)
{
    std::wstring textMsg;
    if (!g_mbIsInit) {
        textMsg = L"禁止未初始化调用此接口：";
        textMsg += common::utf8ToUtf16(funcName);
        ::MessageBoxW(nullptr, textMsg.c_str(), L"警告", MB_OK);
        ::TerminateProcess((HANDLE)-1, 5);
        return false;
    }

    if (isBlinkThread) {
        if (common::ThreadCall::isBlinkThread())
            return true;
    } else {
        if (common::ThreadCall::isUiThread())
            return true;
    }

    textMsg = L"禁止跨线程调用此接口：";
    textMsg += common::utf8ToUtf16(funcName);
    textMsg += L"，";

    wchar_t* temp = (wchar_t*)malloc(0x200);
    wsprintf(temp, L"当前线程:%d，主线程：%d", ::GetCurrentThreadId(), common::ThreadCall::getUiThreadId());
    textMsg += temp;
    free(temp);

    ::MessageBoxW(nullptr, textMsg.c_str(), L"警告", MB_OK);
    ::TerminateProcess((HANDLE)-1, 5);
    return false;
}

static bool checkThreadCallIsValid(const char* funcName)
{
    return checkThreadCallIsValidImpl(funcName, false);
}

mbSettings* MB_CALL_TYPE mbCreateInitSettings()
{
    mbSettings* settings = new mbSettings();
    memset(settings, 0, sizeof(mbSettings));
    settings->version = kMbVersion;
    return settings;
}

void MB_CALL_TYPE mbSetInitSettings(mbSettings* settings, const char* name, const char* value)
{
    if (0 == strcmp(name, "DisableCC"))
        settings->mask = MB_ENABLE_DISABLE_CC;
}

void MB_CALL_TYPE mbInit(const mbSettings* settings)
{
    if (g_mbIsInit)
        return;
    g_mbIsInit = true;

    //////////////////////////////////////////////////////////////////////////
    //settings = new mbSettings();
    //memset((void*)settings, 0, sizeof(settings));
    //((mbSettings*)(settings))->mask = MB_ENABLE_DISABLE_CC;
    //////////////////////////////////////////////////////////////////////////

    if (settings)
        mb::g_mbMask = settings->mask;
    
    common::ThreadCall::init((const mbSettings*)settings);
    OutputDebugStringA("mb.cpp, mbInit\n");
}

void MB_CALL_TYPE mbUninit()
{
//     SqBind::getInst()->uninit();
//     common::ThreadCall::shutdown();
}

#define ENABLE_VERIFY 1

typedef enum enumSYSTEM_INFORMATION_CLASS {
    SystemBasicInformation,
    SystemProcessorInformation,
    SystemPerformanceInformation,
    SystemTimeOfDayInformation,
}SYSTEM_INFORMATION_CLASS;

typedef struct tagPROCESS_BASIC_INFORMATION {
    DWORD ExitStatus;
    DWORD PebBaseAddress;
    DWORD AffinityMask;
    DWORD BasePriority;
    ULONG UniqueProcessId;
    ULONG InheritedFromUniqueProcessId;
}PROCESS_BASIC_INFORMATION;

typedef LONG(WINAPI* PNTQUERYINFORMATIONPROCESS)(HANDLE, UINT, PVOID, ULONG, PULONG);
PNTQUERYINFORMATIONPROCESS	pNtQueryInformationProcess = NULL;

static LPWSTR getProcessNameById(DWORD ProcessID)
{
    HANDLE hProcessSnap;

    LPWSTR result = new WCHAR[MAX_PATH + 1];
    PROCESSENTRY32 pe32;
    //获取系统中全部进程的快照 Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hProcessSnap) return(FALSE);

    pe32.dwSize = sizeof(PROCESSENTRY32);

    // 获取第一个进程的信息 Retrieve information about the first process,
    // 若获取失败则退出 and exit if unsuccessful
    if (!Process32First(hProcessSnap, &pe32)) {
        // 清除快照 clean the snapshot object
        CloseHandle(hProcessSnap);
        printf("!!! Failed to gather information on system processes! \n");
        return(NULL);
    }

    //匹配进程ID Matching Process ID
    do {
        if (ProcessID == pe32.th32ProcessID) {
            //拷贝进程名 Copy Process Name
            wcscpy(result, (LPWSTR)pe32.szExeFile);
            break;
        }
    } while (::Process32Next(hProcessSnap, &pe32));

    // 清除快照 clean the snapshot object
    ::CloseHandle(hProcessSnap);

    return result;
}

static int getParentProcessID(DWORD dwId)
{
    LONG                      status;
    DWORD                     dwParentPID = 0;
    HANDLE                    hProcess;
    PROCESS_BASIC_INFORMATION pbi;

    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, dwId);
    if (!hProcess)
        return -1;

    if (!pNtQueryInformationProcess)
        pNtQueryInformationProcess = (PNTQUERYINFORMATIONPROCESS)::GetProcAddress(::LoadLibraryW(L"ntdll.dll"), "NtQueryInformationProcess");

    status = pNtQueryInformationProcess(hProcess, SystemBasicInformation, (PVOID)&pbi, sizeof(PROCESS_BASIC_INFORMATION), NULL);
    if (!status)
        dwParentPID = pbi.InheritedFromUniqueProcessId;

    CloseHandle(hProcess);
    return dwParentPID;
}

const wchar_t* getParentName()
{
    DWORD parentProcessID = getParentProcessID(::GetCurrentProcessId());
    LPWSTR name = getProcessNameById(parentProcessID);

    return name;
}

static std::string hashRegister(const std::string& str, const std::string& key)
{
    std::string result;
    for (size_t i = 0; i < str.size(); ++i) {
        result += str[i] ^ (key[i % key.size()]);
    }
    return result;
}

static void licenseCheck();
static int s_licenseCheckCount = 0;

static void licenseCheckImpl()
{
    std::vector<wchar_t> path;
    path.resize(MAX_PATH + 1);
    memset(&path[0], 0, sizeof(wchar_t) * (MAX_PATH + 1));
    ::GetModuleFileNameW(g_hModule, &path[0], MAX_PATH);
    ::PathRemoveFileSpecW(&path[0]);
    ::PathAppendW(&path[0], L"license.key");

    std::vector<char> buffer;
    common::readFile(&path[0], &buffer);

    if (buffer.size() == 0) {
        const wchar_t* name = getParentName();
        const WCHAR* blackParentProcessList[] = {
            L"devenv.exe",
            L"e.exe",
            L"delphi32.exe",
            nullptr,
        };

        bool find = false;
        for (int i = 0; blackParentProcessList[i]; ++i) {
            if (0 == _wcsicmp(name, blackParentProcessList[i])) {
                find = true;
                break;
            }
        }
        delete name;

        HMODULE hMod = LoadLibraryW(L"Kernel32.dll");
        typedef BOOL(__stdcall* FN_IsDebuggerPresent)();
        FN_IsDebuggerPresent pIsDebuggerPresent = (FN_IsDebuggerPresent)GetProcAddress(hMod, "IsDebuggerPresent");

        if (find || pIsDebuggerPresent()) {
            ::MessageBoxW(nullptr, L"您使用的是未注册版本，请到miniblink.net注册后使用，支持下正版", L"未注册", 0);
            ::ExitProcess(-1);
        }
    }
    --s_licenseCheckCount;

    licenseCheck();
}

static void licenseCheck()
{
// #if ENABLE_VERIFY && ENABLE_IN_MB_MAIN
//     static bool isInit = false;
//     if (!isInit) {
//         isInit = true;
//         SqBind* verify = SqBind::getInst();
// 
// #if 1
// #if _DEBUG
//         ::DeleteFileW(L"p:\\license.key");
//         // std::string requestCode = verify->createRequestCode("email:weolar@qq.com");
//         std::string license = verify->createLicense("4gAAAMgBAAC6AQAAEAYAAJMCAAC6AQAAsgQAALoBAABMAgAAzwQAALoBAABOAgAAkwIAALgBAADEBAAAAwcAALoBAADwBQAA8AUAALACAABaAgAAbQAAALoAAADwBQAA8AUAAAMGAABSBgAAuAYAAM8AAACoAgAAUwAAAOcBAABLAAAAbAUAAFMAAACqBQAALgIAALoAAACbAQAA4gAAAAMAAAC6AAAAUwAAAGsGAABLAAAA5wEAAAwBAAADBwAAAwYAAHEEAADiAQAAlwQAAGsGAACLAwAAbAUAAEsAAABsBQAAUwAAAJcEAACAAAAAbAUAAFMAAACyBAAALgIAAFoCAAAHAAAAWgIAAHwAAAC4BgAAHgIAANoCAACLAwAAuAEAAC4CAACBBgAAOQEAALgGAAAHAAAA2gIAADkBAABaAgAAmAMAAKoFAABoBQAAWgIAAFMAAADiAAAAPQMAALoAAADnAQAAxAQAAO0DAADnAQAADAEAAAMHAAADBgAAcQQAALIEAAD9AAAA4gAAAJgDAABOAgAAOQEAALgBAAAuAgAAlwQAAE8AAABsBQAAUwAAALACAAA5AQAAbAUAAFMAAADnAQAAOQEAALgBAABqBgAAWgIAAM8AAACoAgAAHgIAANsBAADBAwAAuAYAAAwBAABSBgAATgIAALACAABtAAAAWgIAAGwFAABaAgAAmwEAAOcBAAA5AQAAsAIAAFMAAABsBQAAmAMAAGwFAADaAgAAAwcAALgBAAC4AQAAgAAAAIMGAAD4AQAAbAUAAD0DAADnAQAABQAAAIMGAAA9AwAAcQQAAOIBAAC4AQAALgIAAIMGAACYAwAAugAAAFMAAACwAgAAPQMAAFoCAACbAQAAaAUAAJcEAABsBQAAUwAAAGwFAAA5AQAAbAUAAJsBAADaAgAAwQMAAFoCAACbAQAAsAIAADkBAABsBQAALgIAAHEEAABLAAAAbAUAAFMAAADnAQAAOQEAAGwFAABTAAAA5wEAADkBAABsBQAAUwAAAOcBAAA5AQAAbAUAAFMAAADnAQAAOQEAAIMGAAAHAAAAsAIAADkBAABsBQAAawYAAMQEAAA5AQAAbAUAAFMAAADnAQAAOQEAAGwFAABTAAAA5wEAADkBAACDBgAAOQEAAMQEAAA5AQAAbAUAAFMAAADnAQAAOQEAAGwFAABTAAAAlwQAAHwAAAC4BgAAgQYAAMQEAAA5AQAAbAUAAFMAAADnAQAAOQEAAGwFAABTAAAA5wEAAD0DAAC4BgAAgQYAAMQEAAA5AQAAbAUAAFMAAADnAQAAOQEAAGwFAABTAAAAsAIAADkBAABsBQAA5wEAAMQEAAA5AQAAbAUAAFMAAADnAQAAOQEAAGwFAABTAAAA5wEAAOIBAABsBQAAmAMAAGsGAACYAwAAWgIAAJgDAACwAgAAmAMAAGwFAABTAAAA5wEAADkBAAC4AQAAgQYAAO0DAADtAwAA5wEAAAwBAAADBwAAAwYAAHEEAABaAgAAawYAAFIGAACbAQAA5wEAADkBAAC4AQAALgIAAOcBAAA5AQAAbAUAAFMAAADnAQAAOQEAAGwFAABTAAAA2gIAAJgDAAC4AQAAagYAAAMHAACAAAAAqAIAAAwBAABSBgAA/QAAAKgCAAB8AAAAUgYAAKoFAACoAgAADAEAALIEAACAAAAAqAIAAOcBAADEBAAA+AEAAGwFAABTAAAA5wEAADkBAABsBQAAUwAAAOcBAAA5AQAAugAAAOcBAAD9AAAAqgUAAKgCAAAMAQAAsgQAAIAAAACoAgAAuAEAAGgFAACtAQAASwAAAK0BAACXBAAAUwAAAKgCAACLAwAAqgUAAIAAAABLAAAAbQAAALIEAABaAgAASwAAAK0BAACXBAAA/QAAAFoCAACtAQAAAwAAAGsBAABaAgAAUwAAAOcBAAA5AQAAsgQAAE8AAACXBAAAUwAAAHEEAABtAAAAcQQAAIEGAABrBgAAuAEAAOcBAADBAwAAAwYAAC4CAABqBgAAOQEAALIEAAADAAAA");
// 
//         HANDLE hFile = CreateFileW(L"p:\\license.key", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL); // L"D:\\license.key"
//         if (INVALID_HANDLE_VALUE == hFile)
//             DebugBreak();
// 
//         DWORD numberOfBytesWrite = 0;
//         BOOL b = ::WriteFile(hFile, license.c_str(), license.size(), &numberOfBytesWrite, nullptr);
//         ::CloseHandle(hFile);
// #endif
// #endif
//         char* output = (char*)malloc(0x100);
//         sprintf_s(output, 0x99, "licenseCheck: %d\n", ::GetCurrentThreadId());
//         OutputDebugStringA(output);
//         free(output);
// 
//         verify->timerFire();
//     }
// #endif

    if (0 == s_licenseCheckCount) {
        ++s_licenseCheckCount;
        content::postDelayTaskToMainThread(FROM_HERE, [] { licenseCheckImpl(); }, 30000);
    }
}

mbWebView MB_CALL_TYPE mbCreateWebWindow(mbWindowType type, HWND parent, int x, int y, int width, int height)
{
    checkThreadCallIsValid(__FUNCTION__);

    licenseCheck();

    mb::MbWebView* result = new mb::MbWebView();
    result->createWkeWebWindowInUiThread(type, parent, x, y, width, height);
    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [result] {
        result->createWkeWebWindowOrViewInBlinkThread(true);
    });

    return (int)result->getId();
}

mbWebView MB_CALL_TYPE mbCreateWebCustomWindow(HWND parent, DWORD style, DWORD styleEx, int x, int y, int width, int height)
{
    checkThreadCallIsValid(__FUNCTION__);

    licenseCheck();

    mb::MbWebView* result = new mb::MbWebView();
    result->createWkeWebWindowImplInUiThread(parent, style, styleEx, x, y, width, height);
    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [result] {
        result->createWkeWebWindowOrViewInBlinkThread(true);
    });

    return (int)result->getId();
}

mbWebView MB_CALL_TYPE mbCreateWebView()
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* result = new mb::MbWebView();

    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [result] {
        result->createWkeWebWindowOrViewInBlinkThread(false);
    });

    return (int)result->getId();
}

// 原则：如果在主线程或blink线程，判断完id是否存活可以不用锁mb::MbWebView。如果是其他线程，则必须锁

void MB_CALL_TYPE mbDestroyWebView(mbWebView webviewHandle)
{
    checkThreadCallIsValid(__FUNCTION__);

    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;

    webview->preDestroy();

    if (webview->m_destroyCallback)
        webview->m_destroyCallback(webviewHandle, webview->m_destroyCallbackParam, nullptr);

    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webview] {
        delete webview;
    });
}

void MB_CALL_TYPE mbMoveToCenter(mbWebView webviewHandle)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;

    int width = 0;
    int height = 0;

    HWND hWnd = webview->getHostWnd();

    RECT rect = { 0 };
    ::GetWindowRect(hWnd, &rect);
    width = rect.right - rect.left;
    height = rect.bottom - rect.top;

    int parentWidth = 0;
    int parentHeight = 0;
    if (WS_CHILD == ::GetWindowLong(hWnd, GWL_STYLE)) {
        HWND parent = ::GetParent(hWnd);
        RECT rect = { 0 };
        ::GetClientRect(parent, &rect);
        parentWidth = rect.right - rect.left;
        parentHeight = rect.bottom - rect.top;
    } else {
        parentWidth = ::GetSystemMetrics(SM_CXSCREEN);
        parentHeight = ::GetSystemMetrics(SM_CYSCREEN);
    }

    int x = (parentWidth - width) / 2;
    int y = (parentHeight - height) / 2;

    ::MoveWindow(hWnd, x, y, width, height, FALSE);
}

void MB_CALL_TYPE mbSetAutoDrawToHwnd(mbWebView webviewHandle, BOOL b)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->setAutoDrawToHwnd(!!b);
}

void MB_CALL_TYPE mbSetMouseEnabled(mbWebView webviewHandle, BOOL b)
{
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [b](mb::MbWebView* webview) {
        wkeSetMouseEnabled(webview->getWkeWebView(), !!b);
    });
}

void MB_CALL_TYPE mbSetTouchEnabled(mbWebView webviewHandle, BOOL b)
{
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [b](mb::MbWebView* webview) {
        wkeSetTouchEnabled(webview->getWkeWebView(), !!b);
    });
}

void MB_CALL_TYPE mbSetSystemTouchEnabled(mbWebView webviewHandle, BOOL b)
{
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [b](mb::MbWebView* webview) {
        wkeSetSystemTouchEnabled(webview->getWkeWebView(), !!b);
    });
}

void MB_CALL_TYPE mbSetContextMenuEnabled(mbWebView webviewHandle, BOOL b)
{
#if ENABLE_IN_MB_MAIN
    //checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [b](mb::MbWebView* webview) {
        wkeSetContextMenuEnabled(webview->getWkeWebView(), !!b);
    });
#endif
}

void MB_CALL_TYPE mbSetNavigationToNewWindowEnable(mbWebView webviewHandle, BOOL b)
{
    //checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [b](mb::MbWebView* webview) {
        wkeSetNavigationToNewWindowEnable(webview->getWkeWebView(), !!b);
    });
}

void MB_CALL_TYPE mbSetHeadlessEnabled(mbWebView webviewHandle, BOOL b)
{
    //checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [b](mb::MbWebView* webview) {
        wkeSetHeadlessEnabled(webview->getWkeWebView(), !!b);
    });
}

void MB_CALL_TYPE mbSetDragDropEnable(mbWebView webviewHandle, BOOL b)
{
    //checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [b](mb::MbWebView* webview) {
        wkeSetDragDropEnable(webview->getWkeWebView(), !!b);
    });
}

void MB_CALL_TYPE mbSetContextMenuItemShow(mbWebView webviewHandle, mbMenuItemId item, BOOL isShow)
{
    //checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [item, isShow](mb::MbWebView* webview) {
        wkeSetContextMenuItemShow(webview->getWkeWebView(), (wkeMenuItemId)item, !!isShow);
    });
}

void MB_CALL_TYPE mbSetDragEnable(mbWebView webviewHandle, BOOL b)
{
    //checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [b](mb::MbWebView* webview) {
        wkeSetDragEnable(webview->getWkeWebView(), !!b);
    });
}

const utf8* MB_CALL_TYPE mbGetTitle(mbWebView webviewHandle)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return "";
	return webview->getTitle().c_str();
}

const utf8* MB_CALL_TYPE mbGetUrl(mbWebView webviewHandle)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return "";
	return webview->getUrl().c_str();
}

static void canGoForwardOrBack(mbWebView webviewHandle, mbCanGoBackForwardCallback callback, void* param, BOOL isGoForward)
{
    if (!callback)
        return;

    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview) {
        callback(NULL_WEBVIEW, param, kMbAsynRequestStateFail, false);
        return;
    }

    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, callback, param, isGoForward] {
        BOOL b = false;
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
        if (webview)
            b = isGoForward ? wkeCanGoForward(webview->getWkeWebView()) : wkeCanGoBack(webview->getWkeWebView());

        common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [webviewHandle, callback, param, b] {
            mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
            if (!webview) {
                callback(NULL_WEBVIEW, param, kMbAsynRequestStateFail, false);
                return;
            }
            callback(webviewHandle, param, kMbAsynRequestStateOk, b);
        });
    });
}

void MB_CALL_TYPE mbCanGoForward(mbWebView webviewHandle, mbCanGoBackForwardCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    canGoForwardOrBack(webviewHandle, callback, param, TRUE);
}

void MB_CALL_TYPE mbCanGoBack(mbWebView webviewHandle, mbCanGoBackForwardCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    canGoForwardOrBack(webviewHandle, callback, param, FALSE);
}

void MB_CALL_TYPE mbGetCookie(mbWebView webviewHandle, mbGetCookieCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    if (!callback)
        return;

    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview) {
        callback(NULL_WEBVIEW, param, kMbAsynRequestStateFail, nullptr);
        return;
    }

    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, callback, param] {
        std::string* cookie = nullptr;
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
        if (webview) {
            cookie = new std::string(wkeGetCookie(webview->getWkeWebView()));
        } else
            cookie = new std::string("");
                
        common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [webviewHandle, callback, param, cookie] {
            mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
            if (!webview) {
                callback(NULL_WEBVIEW, param, kMbAsynRequestStateFail, nullptr);
                delete cookie;
                return;
            }
            callback(webviewHandle, param, kMbAsynRequestStateOk, cookie->c_str());
            delete cookie;
        });
    });
}

const utf8* MB_CALL_TYPE mbGetCookieOnBlinkThread(mbWebView webviewHandle)
{
    checkThreadCallIsValidImpl(__FUNCTION__, true);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return NULL;
    return wkeGetCookie(webview->getWkeWebView());
}

void MB_CALL_TYPE mbClearCookie(mbWebView webviewHandle)
{
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [](mb::MbWebView* webview) {
        wkeClearCookie(webview->getWkeWebView());
    });
}

void MB_CALL_TYPE mbSetViewSettings(mbWebView webviewHandle, const mbViewSettings* settings)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;

    mbViewSettings* settingsCopy = new mbViewSettings();
    memcpy(settingsCopy, settings, sizeof(mbViewSettings));

    webview->setBackgroundColor(settings->bgColor);

    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, settingsCopy]() {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
        if (webview)
            wkeSetViewSettings(webview->getWkeWebView(), (const wkeViewSettings*)settingsCopy);
        delete settingsCopy;
    });
}

void MB_CALL_TYPE mbSetTransparent(mbWebView webviewHandle, BOOL transparent)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->setIsTransparent(transparent);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [transparent](mb::MbWebView* webview) {
        wkeSetTransparent(webview->getWkeWebView(), transparent);
    });
}

void MB_CALL_TYPE mbSetHandle(mbWebView webviewHandle, HWND wnd)
{
    checkThreadCallIsValid(__FUNCTION__);
   
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [wnd](mb::MbWebView* webview) {
        webview->setHostWnd(wnd); // 必须在这设置，不能放闭包外。那样会导致提前设置的mbSetDragDropEnable无效，从而导致RegisterDragDrop被设置
        wkeSetHandle(webview->getWkeWebView(), wnd);
    });
}

HWND MB_CALL_TYPE mbGetHostHWND(mbWebView webviewHandle)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return NULL;
    return webview->getHostWnd();
}

void MB_CALL_TYPE mbSetHandleOffset(mbWebView webviewHandle, int x, int y)
{
    checkThreadCallIsValid(__FUNCTION__);

    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;
    webview->setOffset(x, y);

    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [x, y](mb::MbWebView* webview) {
        wkeSetHandleOffset(webview->getWkeWebView(), x, y);
    });
}

void MB_CALL_TYPE mbSetCspCheckEnable(mbWebView webviewHandle, BOOL b)
{
    //checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [b](mb::MbWebView* webview) {
        wkeSetCspCheckEnable(webview->getWkeWebView(), !!b);
    });
}

void MB_CALL_TYPE mbSetNpapiPluginsEnabled(mbWebView webviewHandle, BOOL b)
{
    //checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [b]() {
        wkeSetNpapiPluginsEnabled(nullptr, !!b);
    });
}

void MB_CALL_TYPE mbSetMemoryCacheEnable(mbWebView webviewHandle, BOOL b)
{
    //checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [b](mb::MbWebView* webview) {
        wkeSetMemoryCacheEnable(webview->getWkeWebView(), !!b);
    });
}

void MB_CALL_TYPE mbSetResourceGc(mbWebView webviewHandle, int intervalSec)
{
    //checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [intervalSec](mb::MbWebView* webview) {
        wkeSetResourceGc(webview->getWkeWebView(), intervalSec);
    });
}

void MB_CALL_TYPE mbSetCookie(mbWebView webviewHandle, const utf8* url, const utf8* cookie)
{
    //checkThreadCallIsValid(__FUNCTION__);

    //cookie = "cna22=111111; domain=.1688.com; path=/; expires=Tue, 23-Jan-2029 13:17:21 GMT;";

    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
    if (!webview)
        return;

    std::string* urlString = new std::string(url);
    std::string* cookieString = new std::string(cookie);

    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, urlString, cookieString] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
        if (webview)
            wkeSetCookie(webview->getWkeWebView(), urlString->c_str(), cookieString->c_str());
                
        OutputDebugStringA("mbSetCookie:");
        OutputDebugStringA(cookieString->c_str());
        OutputDebugStringA("\n");
        delete urlString;
        delete cookieString;
    });
}

void MB_CALL_TYPE mbSetCookieEnabled(mbWebView webviewHandle, BOOL b)
{
    //checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [b](mb::MbWebView* webview) {
        wkeSetCookieEnabled(webview->getWkeWebView(), !!b);
    });
}

static void setFullPathOnBlinkThread(mbWebView webviewHandle, std::wstring* pathString, bool isCookiePath)
{
    if (!webviewHandle) {
        isCookiePath ? wkeSetCookieJarFullPath(nullptr, pathString->c_str()) : wkeSetLocalStorageFullPath(nullptr, pathString->c_str());
    } else {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
        if (webview) {
            std::string pathUtf8 = common::utf16ToUtf8(pathString->c_str());
            wkeSetDebugConfig(webview->getWkeWebView(), isCookiePath ? "setCookieJarFullPath" : "setLocalStorageFullPath", pathUtf8.c_str());
        }
    }
   
    delete pathString;
}

void MB_CALL_TYPE setFullPath(mbWebView webviewHandle, const WCHAR* path, bool isCookiePath)
{
    //checkThreadCallIsValid(__FUNCTION__);
    if (!path)
        return;
    std::wstring* pathString = new std::wstring(path);
    if (0 == pathString->size()) {
        delete pathString;
        return;
    }

    if (common::ThreadCall::isBlinkThread()) {
        setFullPathOnBlinkThread(webviewHandle, pathString, isCookiePath);
    } else {
        common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, pathString, isCookiePath] {
            setFullPathOnBlinkThread(webviewHandle, pathString, isCookiePath);
        });
    }
}

// 这两api如果在blink线程被调用，必须立刻执行。否则会产生老cookie\storage路径文件
void MB_CALL_TYPE mbSetLocalStorageFullPath(mbWebView webviewHandle, const WCHAR* path)
{
    setFullPath(webviewHandle, path, false);
}

void MB_CALL_TYPE mbSetCookieJarFullPath(mbWebView webviewHandle, const WCHAR* path)
{
    setFullPath(webviewHandle, path, true);
}

void MB_CALL_TYPE mbSetCookieJarPath(mbWebView webviewHandle, const WCHAR* path)
{
    if (!path)
        return;

    std::wstring pathString(path);
    if (0 == pathString.size())
        return;

    if (L'\\' != pathString[pathString.size() - 1])
        pathString += L'\\';
    pathString += L"cookies.dat";

    mbSetCookieJarFullPath(webviewHandle, pathString.c_str());
}

void MB_CALL_TYPE mbAddPluginDirectory(mbWebView webviewHandle, const WCHAR* path)
{
    checkThreadCallIsValid(__FUNCTION__);
    std::wstring* pathString = new std::wstring(path);
    common::setPluginDirectory(*pathString);

    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, pathString] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
        if (webview)
            wkeAddPluginDirectory(webview->getWkeWebView(), pathString->c_str());
        
        delete pathString;
    });
}

void MB_CALL_TYPE mbSetUserAgent(mbWebView webviewHandle, const utf8* userAgent)
{
    checkThreadCallIsValid(__FUNCTION__);
    if (!userAgent)
        return;

    std::string* userAgentString = new std::string(userAgent);

    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, userAgentString] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr((int64_t)webviewHandle);
        if (webview)
            wkeSetUserAgent(webview->getWkeWebView(), userAgentString->c_str()); 
        
        delete userAgentString;
    });
}

void MB_CALL_TYPE mbSetZoomFactor(mbWebView webviewHandle, float factor)
{
    //checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtrLocked(webviewHandle);
    if (!webview)
        return;
    webview->setZoomFactor(factor);
    common::LiveIdDetect::get()->unlock((int64_t)webviewHandle, webview);

    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [factor](mb::MbWebView* webview) {
        wkeSetZoomFactor(webview->getWkeWebView(), factor);
    });
}

float MB_CALL_TYPE mbGetZoomFactor(mbWebView webviewHandle)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return 1.0f;
    return webview->getZoomFactor();
}

void MB_CALL_TYPE mbSetDebugConfig(mbWebView webviewHandle, const char* debug, const char* param)
{
    checkThreadCallIsValid(__FUNCTION__);

    if (0 == strcmp(debug, "setPrintingEdgeDistance")) {
        std::string str(param);
        for (int i = 0; i < 7; ++i) {
            size_t pos = str.find(",");
            if (std::string::npos == pos)
                break;
            std::string numStr = str.substr(0, pos);
            str = str.substr(pos + 1);

            int num = atoi(numStr.c_str());

            if (i < 4) {
                if (num < 0 || num > 100)
                    continue;
                printing::g_edgeDistance[i] = num * 100;
            }
        }
        return;
    }

    if (0 == strcmp(debug, "disableNativeSetCapture")) {
        mb::g_enableNativeSetCapture = false;
    } else if (0 == strcmp(debug, "disableNativeSetFocus")) {
      mb::g_enableNativeSetFocus = false;
    }

    std::string* debugString = new std::string(debug);
    std::string* paramString = new std::string(param);

    if (*debugString == "imageEnable")
        *debugString = "imageMbEnable";
   
    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, debugString, paramString] {
        if (!webviewHandle)
            wkeSetDebugConfig(nullptr, debugString->c_str(), paramString->c_str());
        else {
            mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
            if (webview)
                wkeSetDebugConfig(webview->getWkeWebView(), debugString->c_str(), paramString->c_str());
        }        
        delete debugString;
        delete paramString;
    });
}

void MB_CALL_TYPE mbSetProxy(mbWebView webviewHandle, const mbProxy* proxy)
{
    checkThreadCallIsValid(__FUNCTION__);

    mbProxy* proxyCopy = new mbProxy();
    memcpy(proxyCopy, proxy, sizeof(mbProxy));

    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, proxyCopy] {
		if (!webviewHandle)
			wkeSetProxy((const wkeProxy*)proxyCopy);
        else {
            mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
            if (webview)
                wkeSetViewProxy(webview->getWkeWebView(), (wkeProxy*)proxyCopy);
        }
        delete proxyCopy;
    });
}

void MB_CALL_TYPE mbSetViewProxy(mbWebView webviewHandle, const mbProxy* proxy)
{
    mbProxy* proxyCopy = new mbProxy();
    *proxyCopy = *proxy;

    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, proxyCopy] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
        if (webview)
            wkeSetViewProxy(webview->getWkeWebView(), (wkeProxy*)proxyCopy);

        delete proxyCopy;
    });
}

void MB_CALL_TYPE mbResize(mbWebView webviewHandle, int w, int h)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (webview)
        webview->onResize(w, h, true);
}

static bool WKE_CALL_TYPE onNavigation(wkeWebView wkeWebview, void* param, wkeNavigationType navigationType, const wkeString url)
{
    const utf8* urlString = wkeGetString(url);
    BOOL result = true;
    mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return false;

    webview->setNavigateIndex(wkeGetNavigateIndex(webview->getWkeWebView()));

    result = webview->getClosure().m_NavigationCallback(webviewHandle, webview->getClosure().m_NavigationParam, (mbNavigationType)navigationType, urlString);
    return !!result;
}

static bool WKE_CALL_TYPE onNavigationSync(wkeWebView wkeWebview, void* param, wkeNavigationType navigationType, const wkeString url)
{
    mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return false;

    const utf8* urlString = wkeGetString(url);
    BOOL result = true;

    webview->setNavigateIndex(wkeGetNavigateIndex(webview->getWkeWebView()));

    common::ThreadCall::callUiThreadSync(MB_FROM_HERE, [&result, webviewHandle, navigationType, urlString] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
        if (!webview) {
            result = FALSE;
            return;
        }
        result = webview->getClosure().m_NavigationSyncCallback(webviewHandle, webview->getClosure().m_NavigationSyncParam, (mbNavigationType)navigationType, urlString);
    });

    return !!result;
}

static void WKE_CALL_TYPE onDocumentReady(wkeWebView wkeWebview, void* param, wkeWebFrameHandle frameId)
{
    mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;
    
    mbWebFrameHandle mbFrameId = mb::MbWebView::toMbFrameHandle(wkeWebview, frameId);
    bool isMainFrame = wkeIsMainFrame(wkeWebview, frameId);
    if (isMainFrame)
        webview->setMainFrameId(frameId);

    if (webview->getClosure().m_DocumentReadyInBlinkCallback)
        webview->getClosure().m_DocumentReadyInBlinkCallback(webviewHandle, webview->getClosure().m_DocumentReadyInBlinkParam, mbFrameId);

    if (!(webview->getClosure().m_DocumentReadyCallback))
        return;

    common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [webviewHandle, mbFrameId] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
        if (!webview)
            return;
        webview->getClosure().m_DocumentReadyCallback(webviewHandle, webview->getClosure().m_DocumentReadyParam, mbFrameId);
    });
}

void MB_CALL_TYPE mbOnPaintUpdated(mbWebView webviewHandle, mbPaintUpdatedCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;
    webview->setPaintUpdatedCallback(callback, param);
}

void MB_CALL_TYPE mbOnPaintBitUpdated(mbWebView webviewHandle, mbPaintBitUpdatedCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;
    webview->setPaintBitUpdatedCallback(callback, param);
}

HDC MB_CALL_TYPE mbGetLockedViewDC(mbWebView webviewHandle)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return NULL;
    return webview->getViewDC();
}

void MB_CALL_TYPE mbUnlockViewDC(mbWebView webviewHandle)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;
    webview->unlockViewDC();
}

void MB_CALL_TYPE mbOnCreateView(mbWebView webviewHandle, mbCreateViewCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;
    webview->getClosure().setCreateViewCallback(callback, param);
}

struct DownloadWrap {
    std::string url;
    std::string mime;
    std::string contentDisposition;
    blinkWebURLRequestPtr blinkRequest;

    ~DownloadWrap()
    {

    }
};

BOOL MB_CALL_TYPE mbPopupDownloadMgr(mbWebView webviewHandle, const char* url, void* downloadWrap)
{
// #if ENABLE_IN_MB_MAIN    
//     static DownloadMgr* downloadUi = nullptr;
//     if (!downloadUi)
//         downloadUi = new DownloadMgr();
// 
//     if (downloadWrap) {
//         DownloadWrap* job = (DownloadWrap*)downloadWrap;
//         downloadUi->onNewDownloadItem(job->url.c_str(), job->mime.c_str(), job->contentDisposition.c_str());
//     } else
//         downloadUi->createWnd();
// #endif
    return TRUE;
}

static mbDownloadOpt mbSimpleDownload(mbWebView mbWebview,
    const WCHAR* path,
    const mbDialogOptions* dialogOpt,
    const mbDownloadOptions* downloadOptions,
    size_t expectedContentLength,
    const char* url,
    const char* mime,
    const char* disposition,
    mbNetJob job,
    mbNetJobDataBind* dataBind,
    mbDownloadBind* callbackBind)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(mbWebview);
    if (!webview)
        return kMbDownloadOptCancel;

    webview->setIsMouseKeyMessageEnable(false);
    download::SimpleDownload* downloader = download::SimpleDownload::create(mbWebview, path, dialogOpt, downloadOptions,
        expectedContentLength,
        url,
        mime,
        disposition,
        job,
        dataBind,
        callbackBind);
    if (downloader)
        return kMbDownloadOptCacheData;

    webview->setIsMouseKeyMessageEnable(true);
    return kMbDownloadOptCancel;
}

mbDownloadOpt MB_CALL_TYPE mbPopupDialogAndDownload(mbWebView webviewHandle,
    const mbDialogOptions* dialogOpt,
    size_t expectedContentLength,
    const char* url,
    const char* mime,
    const char* disposition,
    mbNetJob job,
    mbNetJobDataBind* dataBind,
    mbDownloadBind* callbackBind)
{
#if 1 // ENABLE_IN_MB_MAIN
    return mbSimpleDownload(webviewHandle, nullptr, dialogOpt, nullptr, expectedContentLength, url, mime, disposition, job, dataBind, callbackBind);
#endif
    return kMbDownloadOptCancel;
}

mbDownloadOpt MB_CALL_TYPE mbDownloadByPath(mbWebView webviewHandle,
    const mbDownloadOptions* downloadOptions,
    const WCHAR* path,
    size_t expectedContentLength,
    const char* url,
    const char* mime,
    const char* disposition,
    mbNetJob job,
    mbNetJobDataBind* dataBind,
    mbDownloadBind* callbackBind)
{
#if 1 // ENABLE_IN_MB_MAIN
    return mbSimpleDownload(webviewHandle, path, nullptr, downloadOptions, expectedContentLength, url, mime, disposition, job, dataBind, callbackBind);
#endif
    return kMbDownloadOptCancel;
}

void WKE_CALL_TYPE wkeNetJobDataRecvWrap(void* ptr, wkeNetJob job, const char* data, int length)
{
    mbNetJobDataBind* bindWrap = (mbNetJobDataBind*)ptr;
    bindWrap->recvCallback(bindWrap->param, job, data, length);
}

void WKE_CALL_TYPE wkeNetJobDataFinishWrap(void* ptr, wkeNetJob job, wkeLoadingResult result)
{
    mbNetJobDataBind* bindWrap = (mbNetJobDataBind*)ptr;
    bindWrap->finishCallback(bindWrap->param, job, (mbLoadingResult)result);
    delete bindWrap;
}

static wkeDownloadOpt WKE_CALL_TYPE onDownloadInBlinkThread(wkeWebView wkeWebview, void* param, size_t expectedContentLength, const char* url, const char* mime, const char* disposition, wkeNetJob job, wkeNetJobDataBind* dataBind)
{
    mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return kWkeDownloadOptCancel;

    mbNetJobDataBind* bindWrap = new mbNetJobDataBind();
    dataBind->param = nullptr;
    dataBind->recvCallback = nullptr;
    dataBind->finishCallback = nullptr;

    wkeDownloadOpt opt = (wkeDownloadOpt)webview->getClosure().m_DownloadInBlinkThreadCallback(webviewHandle,
        webview->getClosure().m_DownloadInBlinkThreadParam,
        expectedContentLength,
        url, mime, disposition, (mbNetJob)job, bindWrap);

    if (bindWrap->param) {
        dataBind->param = bindWrap;
        dataBind->recvCallback = wkeNetJobDataRecvWrap;
        dataBind->finishCallback = wkeNetJobDataFinishWrap;
    }

    return opt;
}

static bool WKE_CALL_TYPE onDownload(wkeWebView wkeWebview, void* param, const char* url)
{
    mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return false;

    wkeTempCallbackInfo* temInfo = wkeGetTempCallbackInfo(wkeWebview);
    if (temInfo->size != sizeof(wkeTempCallbackInfo))
        return false;

    mbWebFrameHandle mbFrameId = mb::MbWebView::toMbFrameHandle(wkeWebview, temInfo->frame);
    const char* mime = wkeNetGetMIMEType(temInfo->job, nullptr);
    const char* contentDisposition = wkeNetGetHTTPHeaderFieldFromResponse(temInfo->job, "Content-Disposition");

    DownloadWrap* downloadWrap = new DownloadWrap();
    downloadWrap->url = url;
    downloadWrap->mime = mime;
    downloadWrap->contentDisposition = contentDisposition;
    downloadWrap->blinkRequest = wkeNetCopyWebUrlRequest(temInfo->job, false);

    common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [webviewHandle, mbFrameId, downloadWrap] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
        if (!webview) {
            delete downloadWrap;
            return;
        }
        webview->getClosure().m_DownloadCallback(webviewHandle, webview->getClosure().m_DownloadParam, mbFrameId, downloadWrap->url.c_str(), downloadWrap);
        delete downloadWrap;
    });

    return false;
}

static void WKE_CALL_TYPE onAlertBox(wkeWebView wkeWebview, void* param, const wkeString msg)
{
    mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;
    webview->getClosure().m_AlertBoxCallback(webviewHandle, webview->getClosure().m_AlertBoxParam, wkeGetString(msg));
}

static bool WKE_CALL_TYPE onConfirmBox(wkeWebView wkeWebview, void* param, const wkeString msg)
{
    mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return false;
    BOOL result = webview->getClosure().m_ConfirmBoxCallback(webviewHandle, webview->getClosure().m_ConfirmBoxParam, wkeGetString(msg));
    return !!result;
}

static void WKE_CALL_TYPE onConsole(wkeWebView wkeWebview, void* param, wkeConsoleLevel level, const wkeString message, const wkeString sourceName, unsigned sourceLine, const wkeString stackTrace)
{
    mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;

    webview->getClosure().m_ConsoleCallback(webviewHandle, webview->getClosure().m_ConsoleParam,
        (mbConsoleLevel)level, wkeGetString(message), wkeGetString(sourceName), sourceLine, wkeGetString(stackTrace));
}

static void WKE_CALL_TYPE onNetGetFavicon(wkeWebView wkeWebview, void* param, const utf8* url, wkeMemBuf* buf)
{
    mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;

    std::string* urlString = new std::string(url ? url : "");

    wkeMemBuf* bufCopy = nullptr;
    if (buf && 0 != buf->length)
        bufCopy = wkeCreateMemBuf(wkeWebview, buf->data, buf->length);

    common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [webviewHandle, webview, urlString, bufCopy] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
        if (!webview) {
            delete urlString;
            wkeFreeMemBuf(bufCopy);
            return;
        }

        webview->getClosure().m_NetGetFaviconCallback(webviewHandle, webview->getClosure().m_NetGetFaviconParam, urlString->c_str(), (mbMemBuf*)(bufCopy));
        delete urlString;
        wkeFreeMemBuf(bufCopy);
    });
}

// int MB_CALL_TYPE mbNetGetFavicon(mbWebView webView, mbNetGetFaviconCallback callback, void* param)
// {
//     mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
//     if (!webview)
//         return;
//     webview->getClosure().setNetGetFaviconCallback(callback, param);
//     wkeNetGetFavicon(webview->getWkeWebView(), onNetGetFavicon, void* param);
// }

// int MB_CALL_TYPE wkeNetGetFavicon(mbWebView webView, wkeOnNetGetFaviconCallback callback, void* param)
// {
//     wke::checkThreadCallIsValid(__FUNCTION__);
//     return net::getFavicon(webView, callback, param);
// }

bool WKE_CALL_TYPE onWindowClosingCallback(wkeWebView webWindow, void* param)
{
	BOOL result = true;
	mbWebView webviewHandle = (mbWebView)param;
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return true;

	common::ThreadCall::callUiThreadSync(MB_FROM_HERE, [&result, webviewHandle] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
		if (!webview)
			return;

		result = webview->getClosure().m_ClosingCallback(webviewHandle, webview->getClosure().m_ClosingParam, nullptr);
	});

	return !!result;
}

#include "core/MbCallback.h"

// BOOL MB_CALL_TYPE mbOnClose(mbWebView webviewHandle, mbCloseCallback callback, void* param)
// {
// 	webview->m_closeCallback = callback;
// 	webview->m_closeCallbackParam = param;
// 	return TRUE;
// }

BOOL MB_CALL_TYPE mbOnDestroy(mbWebView webviewHandle, mbDestroyCallback callback, void* param)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return TRUE;
    webview->m_destroyCallback = callback;
    webview->m_destroyCallbackParam = param;
    return TRUE;
}

void MB_CALL_TYPE mbOnPluginList(mbWebView webviewHandle, mbGetPluginListCallback callback, void* param)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;
    wkeSetDebugConfig(nullptr, "setPluginListCallback", (const char*)callback);
}

void MB_CALL_TYPE mbPluginListBuilderAddPlugin(void* builder, const utf8* name, const utf8* description, const utf8* fileName)
{
    wkePluginListBuilderAddPlugin(builder, name, description, fileName);
}

void MB_CALL_TYPE mbPluginListBuilderAddMediaTypeToLastPlugin(void* builder, const utf8* name, const utf8* description)
{
    wkePluginListBuilderAddMediaTypeToLastPlugin(builder, name, description);
}

void MB_CALL_TYPE mbPluginListBuilderAddFileExtensionToLastMediaType(void* builder, const utf8* fileExtension)
{
    wkePluginListBuilderAddFileExtensionToLastMediaType(builder, fileExtension);
}

BOOL MB_CALL_TYPE mbOnPrinting(mbWebView webviewHandle, mbPrintingCallback callback, void* param)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return TRUE;
    webview->m_printingCallback = callback;
    webview->m_printingCallbackParam = param;
    return TRUE;
}

void MB_CALL_TYPE mbWake(mbWebView webviewHandle)
{
    //checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::wake();
}

void MB_CALL_TYPE mbGoBack(mbWebView webviewHandle)
{
    //checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [](mb::MbWebView* webview) {
        wkeGoBack(webview->getWkeWebView());
    });
}

void MB_CALL_TYPE mbGoForward(mbWebView webviewHandle)
{
    //checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [](mb::MbWebView* webview) {
        wkeGoForward(webview->getWkeWebView());
    });
}

void MB_CALL_TYPE mbNavigateAtIndex(mbWebView webviewHandle, int index)
{
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [index](mb::MbWebView* webview) {
        wkeNavigateAtIndex(webview->getWkeWebView(), index);
    });
}

int MB_CALL_TYPE mbGetNavigateIndex(mbWebView webviewHandle)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return 0;
    return webview->getNavigateIndex();
}

void MB_CALL_TYPE mbGoToOffset(mbWebView webviewHandle, int offset)
{
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [offset](mb::MbWebView* webview) {
        wkeGoToOffset(webview->getWkeWebView(), offset);
    });
}

void MB_CALL_TYPE mbGoToIndex(mbWebView webviewHandle, int index)
{
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [index](mb::MbWebView* webview) {
        wkeGoToIndex(webview->getWkeWebView(), index);
    });
}

void MB_CALL_TYPE mbStopLoading(mbWebView webviewHandle)
{
    //checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [](mb::MbWebView* webview) {
        wkeStopLoading(webview->getWkeWebView());
    });
}

void MB_CALL_TYPE mbReload(mbWebView webviewHandle)
{
    //checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [](mb::MbWebView* webview) {
        wkeReload(webview->getWkeWebView());
    });
}

void MB_CALL_TYPE mbPerformCookieCommand(mbWebView webviewHandle, mbCookieCommand command)
{
    checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [command](mb::MbWebView* webview) {
        wkePerformCookieCommand(webview->getWkeWebView(), (wkeCookieCommand)command);
    });
}

void MB_CALL_TYPE mbEditorSelectAll(mbWebView webviewHandle)
{
    checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [](mb::MbWebView* webview) {
        wkeSelectAll(webview->getWkeWebView());
    });
}

void MB_CALL_TYPE mbEditorUnSelect(mbWebView webviewHandle)
{
    checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [](mb::MbWebView* webview) {
        wkeEditorUnSelect(webview->getWkeWebView());
    });
}

void MB_CALL_TYPE mbEditorCopy(mbWebView webviewHandle)
{
    checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [](mb::MbWebView* webview) {
        wkeCopy(webview->getWkeWebView());
    });
}

void MB_CALL_TYPE mbEditorCut(mbWebView webviewHandle)
{
    checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [](mb::MbWebView* webview) {
        wkeCut(webview->getWkeWebView());
    });
}

void MB_CALL_TYPE mbEditorPaste(mbWebView webviewHandle)
{
    checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [](mb::MbWebView* webview) {
        wkePaste(webview->getWkeWebView());
    });
}

void MB_CALL_TYPE mbEditorDelete(mbWebView webviewHandle)
{
    checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [](mb::MbWebView* webview) {
        wkeEditorDelete(webview->getWkeWebView());
    });
}

void MB_CALL_TYPE mbEditorRedo(mbWebView webviewHandle)
{
    checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [](mb::MbWebView* webview) {
        wkeEditorRedo(webview->getWkeWebView());
    });
}

void MB_CALL_TYPE mbEditorUndo(mbWebView webviewHandle)
{
    checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [](mb::MbWebView* webview) {
        wkeEditorUndo(webview->getWkeWebView());
    });
}

BOOL MB_CALL_TYPE mbFireMouseEvent(mbWebView webviewHandle, unsigned int message, int x, int y, unsigned int flags)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (webview)
        webview->onMouseMessage(message, x, y, flags);
   
    return true;
}

BOOL MB_CALL_TYPE mbFireContextMenuEvent(mbWebView webviewHandle, int x, int y, unsigned int flags)
{
    checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [x, y, flags](mb::MbWebView* webview) {
        wkeFireContextMenuEvent(webview->getWkeWebView(), x, y, flags);
    });

    return true;
}

BOOL MB_CALL_TYPE mbFireMouseWheelEvent(mbWebView webviewHandle, int x, int y, int delta, unsigned int flags)
{
    checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [x, y, delta, flags](mb::MbWebView* webview) {
        wkeFireMouseWheelEvent(webview->getWkeWebView(), x, y, delta, flags);
    });

    return true;
}

BOOL MB_CALL_TYPE mbFireKeyUpEvent(mbWebView webviewHandle, unsigned int virtualKeyCode, unsigned int flags, BOOL isSystemKey)
{
    checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [virtualKeyCode, flags, isSystemKey](mb::MbWebView* webview) {
        wkeFireKeyUpEvent(webview->getWkeWebView(), virtualKeyCode, flags, !!isSystemKey);
    });

    return true;
}

BOOL MB_CALL_TYPE mbFireKeyDownEvent(mbWebView webviewHandle, unsigned int virtualKeyCode, unsigned int flags, BOOL isSystemKey)
{
    checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [virtualKeyCode, flags, isSystemKey](mb::MbWebView* webview) {
        wkeFireKeyDownEvent(webview->getWkeWebView(), virtualKeyCode, flags, !!isSystemKey);

//         if (113 == virtualKeyCode) {
//             wkeSetDebugConfig(webview->getWkeWebView(), "showDevTools", "E:/mycode/miniblink49/trunk/third_party/WebKit/Source/devtools/front_end/inspector.html");
//         }

//         char* output = (char*)malloc(0x100);
//         sprintf(output, "mbFireKeyDownEvent: %d\n", virtualKeyCode);
//         OutputDebugStringA(output);
//         free(output);
    });

    return true;
}

BOOL MB_CALL_TYPE mbFireKeyPressEvent(mbWebView webviewHandle, unsigned int charCode, unsigned int flags, BOOL isSystemKey)
{
    checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [charCode, flags, isSystemKey](mb::MbWebView* webview) {
        wkeFireKeyPressEvent(webview->getWkeWebView(), charCode, flags, !!isSystemKey);
    });

    return true;
}

BOOL MB_CALL_TYPE mbFireWindowsMessage(mbWebView webviewHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* result)
{
    checkThreadCallIsValid(__FUNCTION__);

    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return TRUE;

    if (WM_SETCURSOR == message) {
        if (webview->setCursorInfoTypeByCache()) {
            if (result)
                *result = 1;
            return TRUE;
        }
    } else if (WM_IME_STARTCOMPOSITION == message) {
        common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle, hWnd](mb::MbWebView* webview) {
            wkeRect caret = wkeGetCaretRect(webview->getWkeWebView());

            POINT offset = webview->getOffset();
            int x = caret.x + offset.x;
            int y = caret.y + offset.y;

            common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [hWnd, webviewHandle, x, y] {
                mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
                if (!webview)
                    return;

                COMPOSITIONFORM compositionForm;
                compositionForm.dwStyle = CFS_POINT | CFS_FORCE_POSITION;
                compositionForm.ptCurrentPos.x = x;
                compositionForm.ptCurrentPos.y = y;

                HIMC hIMC = ::ImmGetContext(hWnd);
                ::ImmSetCompositionWindow(hIMC, &compositionForm);
                ::ImmReleaseContext(hWnd, hIMC);
            });
        });
        return false;
    } else if (WM_IME_COMPOSITION == message) {
        if (lParam & GCS_RESULTSTR) {
            std::vector<WCHAR> buffer;
            HIMC hIMC = ::ImmGetContext(hWnd);
            buffer.resize(ImmGetCompositionStringW(hIMC, GCS_COMPSTR, NULL, 0) + 2);
            memset(&buffer[0], 0, buffer.size());
            ImmGetCompositionStringW(hIMC, GCS_COMPSTR, &buffer[0], buffer.size() - 2);
            ImmReleaseContext(hWnd, hIMC);
        }

    } else {
        common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [hWnd, message, wParam, lParam](mb::MbWebView* webview) {
            LRESULT result = 0;
            wkeFireWindowsMessage(webview->getWkeWebView(), hWnd, message, wParam, lParam, &result);
        });
    }
    return false;
}

void MB_CALL_TYPE mbSetFocus(mbWebView webviewHandle)
{
    checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [](mb::MbWebView* webview) {
        wkeSetFocus(webview->getWkeWebView());
    });
}

void MB_CALL_TYPE mbKillFocus(mbWebView webviewHandle)
{
    checkThreadCallIsValid(__FUNCTION__);
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [](mb::MbWebView* webview) {
        wkeKillFocus(webview->getWkeWebView());
    });
}

void MB_CALL_TYPE mbShowWindow(mbWebView webviewHandle, BOOL b)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;

    if (webview->getHostWnd()) {
        webview->setShow(!!b);
        return;
    }

    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, b]() {
        common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [webviewHandle, b] {
            mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
            if (!webview)
                return;
            webview->setShow(!!b);
        });
    });    
}

int MB_CALL_TYPE mbGetCursorInfoType(mbWebView webviewHandle)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return 0;
    return webview->getCursorInfoType();
}

//////////////////////////////////////////////////////////////////////////
// void readFile(const wchar_t* path, std::vector<char>* buffer)
// {
//     HANDLE hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
//     if (INVALID_HANDLE_VALUE == hFile) {
// 
//         std::vector<WCHAR> filenameBuffer;
//         filenameBuffer.resize(MAX_PATH + 3);
//         ::GetModuleFileNameW(NULL, filenameBuffer.data(), MAX_PATH);
//         ::PathRemoveFileSpecW(filenameBuffer.data());
// 
//         ::PathAppendW(filenameBuffer.data(), L"mtmb.exp");
//         if (::PathFileExistsW(filenameBuffer.data()))
//             DebugBreak();
//         return;
//     }
// 
//     DWORD fileSizeHigh;
//     const DWORD bufferSize = ::GetFileSize(hFile, &fileSizeHigh);
// 
//     DWORD numberOfBytesRead = 0;
//     buffer->resize(bufferSize);
//     BOOL b = ::ReadFile(hFile, &buffer->at(0), bufferSize, &numberOfBytesRead, nullptr);
//     ::CloseHandle(hFile);
//     b = b;
// }
// 
// static bool hookUrl(void* job, const char* url, const char* hookUrl, const wchar_t* localFile, const char* mime)
// {
//     if (0 == strstr(url, hookUrl))
//         return false;
// 
//     //mbNetSetMIMEType(job, (char*)mime);
// 
//     std::vector<char> buffer;
//     readFile(localFile, &buffer);
//     if (0 == buffer.size())
//         return false;
// 
//     mbNetSetData(job, &buffer[0], buffer.size());
// 
//     OutputDebugStringA("hookUrl:");
//     OutputDebugStringA(url);
//     OutputDebugStringA("\n");
// 
//     return true;
// }
// 
// static BOOL MB_CALL_TYPE handleLoadUrlBegin(mbWebView webviewHandle, void* param, const char* url, void* job)
// {
//     if (hookUrl(job, url, "ncpc/nc.js", L"F:\\test\\demo\\20190513\\DianTools\\nc.js", "text/javascript"))
//         return true;
// 
//     if (hookUrl(job, url, "uab/117.js", L"F:\\test\\demo\\20190513\\DianTools\\117.js", "text/javascript"))
//         return true;
// 
//     return false;
// }
//////////////////////////////////////////////////////////////////////////

void MB_CALL_TYPE mbLoadURL(mbWebView webviewHandle, const utf8* url)
{
    //url = "https://www.baidu.com/";
    std::string* urlString = new std::string(url);
    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, urlString] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
        if (webview)
            wkeLoadURL(webview->getWkeWebView(), urlString->c_str());
        delete urlString;
    });
}

void MB_CALL_TYPE mbLoadHtmlWithBaseUrl(mbWebView webviewHandle, const utf8* html, const utf8* baseUrl)
{
    checkThreadCallIsValid(__FUNCTION__);
    std::string* htmlString = new std::string(html);
    std::string* baseUrlString = new std::string(baseUrl);
    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, htmlString, baseUrlString] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
        if (webview)
            wkeLoadHtmlWithBaseUrl(webview->getWkeWebView(), htmlString->c_str(), baseUrlString->c_str());
        delete htmlString;
        delete baseUrlString;
    });
}

void MB_CALL_TYPE mbPostURL(mbWebView webviewHandle, const utf8* url, const char* postData, int postLen)
{
    checkThreadCallIsValid(__FUNCTION__);
    std::string* urlString = new std::string(url);
    std::vector<char>* data = new std::vector<char>();
    data->resize(postLen);
    memcpy(data->data(), postData, postLen);
    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, urlString, data] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
        if (webview)
            wkePostURL(webview->getWkeWebView(), urlString->c_str(), data->data(), (int)data->size());
        delete urlString;
        delete data;
    });
}

mbWebFrameHandle MB_CALL_TYPE mbWebFrameGetMainFrame(mbWebView webviewHandle)
{
    return (mbWebFrameHandle)-2;
}

BOOL MB_CALL_TYPE mbIsMainFrame(mbWebView webviewHandle, mbWebFrameHandle frameId)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return FALSE;

    if (frameId == (mbWebFrameHandle)-2)
        return TRUE;
    //return wkeIsMainFrame(webview->getWkeWebView(), (mbWebFrameHandle)(frameId));
    return webview->getMainFrameId() == frameId;
}

static void getSourceOrMHTML(mbWebView webviewHandle, mbGetSourceCallback calback, void* param, bool isSource)
{
    if (!calback)
        return;

    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle, calback, param, isSource](mb::MbWebView* webview) {
        const utf8* content = isSource ? wkeGetSource(webview->getWkeWebView()) : wkeUtilSerializeToMHTML(webview->getWkeWebView());
        std::vector<char>* contentCopy = new std::vector<char>();
        if (content) {
            size_t size = strlen(content);
            contentCopy->resize(size);
            memcpy(&contentCopy->at(0), content, size);
        }
        
        common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [webviewHandle, calback, param, contentCopy] {
            mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
            if (webview) {
                if (contentCopy->size() > 0) {
                    contentCopy->push_back(0);
                    calback(webviewHandle, param, &contentCopy->at(0));
                } else
                    calback(webviewHandle, param, nullptr);
            }
            delete contentCopy;
        });
    });
}

void MB_CALL_TYPE mbGetSource(mbWebView webviewHandle, mbGetSourceCallback calback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    getSourceOrMHTML(webviewHandle, calback, param, true);
}

void MB_CALL_TYPE mbUtilSerializeToMHTML(mbWebView webviewHandle, mbGetSourceCallback calback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);
    getSourceOrMHTML(webviewHandle, calback, param, false);
}

void MB_CALL_TYPE mbGetContentAsMarkup(mbWebView webviewHandle, mbGetContentAsMarkupCallback calback, void* param, mbWebFrameHandle frameId)
{
    checkThreadCallIsValid(__FUNCTION__);
    if (!calback)
        return;

    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [webviewHandle, calback, param, frameId](mb::MbWebView* webview) {
        wkeWebFrameHandle wkeFrameId = (wkeWebFrameHandle)frameId;
        if ((mbWebFrameHandle)-2 == frameId)
            wkeFrameId = wkeWebFrameGetMainFrame(webview->getWkeWebView());

        size_t size = 0;
        const utf8* content = wkeGetContentAsMarkup(webview->getWkeWebView(), wkeFrameId, &size);
        std::vector<char>* contentCopy = new std::vector<char>();
        if (size > 0) {
            contentCopy->resize(size);
            memcpy(&contentCopy->at(0), content, size);
        }
        common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [webviewHandle, calback, param, contentCopy, size] {
            mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
            if (webview) {
                if (size > 0)
                    calback(webviewHandle, param, &contentCopy->at(0), size);
                else
                    calback(webviewHandle, param, nullptr, 0);
            }
            delete contentCopy;
        });
    });
}

const char* MB_CALL_TYPE mbUtilCreateRequestCode(const char* registerInfo)
{
    if (!registerInfo || 1000 < strlen(registerInfo))
        return nullptr;

#if ENABLE_VERIFY && ENABLE_IN_MB_MAIN
    SqBind* verify = SqBind::getInst();
    std::string* requestCode = new std::string(verify->createRequestCode(registerInfo));

    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [requestCode] {
        delete requestCode;
    });

    OutputDebugStringA("mbUtilCreateRequestCode\n");

    return requestCode->c_str();
#else
    return "4gAAAMgBAAC6AQAAEAYAAJMCAAC6AQAAsgQAALoBAABMAgAAzwQAALoBAABOAgAAkwIAALgBAADEBAAAAwcAALoBAADwBQAA8AUAALACAABaAgAAbQAAALoAAADwBQAA8AUAAAMGAABSBgAAuAYAAM8AAACoAgAAUwAAAOcBAABLAAAAbAUAAFMAAACqBQAALgIAALoAAACbAQAA4gAAAAMAAAC6AAAAUwAAAGsGAABLAAAA5wEAAAwBAAADBwAAAwYAAHEEAADiAQAAlwQAAGsGAACLAwAAbAUAAEsAAABsBQAAUwAAAJcEAACAAAAAbAUAAFMAAACyBAAALgIAAFoCAAAHAAAAWgIAAHwAAAC4BgAAHgIAANoCAACLAwAAuAEAAC4CAACBBgAAOQEAALgGAAAHAAAA2gIAADkBAABaAgAAmAMAAKoFAABoBQAAWgIAAFMAAADiAAAAPQMAALoAAADnAQAAxAQAAO0DAADnAQAADAEAAAMHAAADBgAAcQQAALIEAAD9AAAA4gAAAJgDAABOAgAAOQEAALgBAAAuAgAAlwQAAE8AAABsBQAAUwAAALACAAA5AQAAbAUAAFMAAADnAQAAOQEAALgBAABqBgAAWgIAAM8AAACoAgAAHgIAANsBAADBAwAAuAYAAAwBAABSBgAATgIAALACAABtAAAAWgIAAGwFAABaAgAAmwEAAOcBAAA5AQAAsAIAAFMAAABsBQAAmAMAAGwFAADaAgAAAwcAALgBAAC4AQAAgAAAAIMGAAD4AQAAbAUAAD0DAADnAQAABQAAAIMGAAA9AwAAcQQAAOIBAAC4AQAALgIAAIMGAACYAwAAugAAAFMAAACwAgAAPQMAAFoCAACbAQAAaAUAAJcEAABsBQAAUwAAAGwFAAA5AQAAbAUAAJsBAADaAgAAwQMAAFoCAACbAQAAsAIAADkBAABsBQAALgIAAHEEAABLAAAAbAUAAFMAAADnAQAAOQEAAGwFAABTAAAA5wEAADkBAABsBQAAUwAAAOcBAAA5AQAAbAUAAFMAAADnAQAAOQEAAIMGAAAHAAAAsAIAADkBAABsBQAAawYAAMQEAAA5AQAAbAUAAFMAAADnAQAAOQEAAGwFAABTAAAA5wEAADkBAACDBgAAOQEAAMQEAAA5AQAAbAUAAFMAAADnAQAAOQEAAGwFAABTAAAAlwQAAHwAAAC4BgAAgQYAAMQEAAA5AQAAbAUAAFMAAADnAQAAOQEAAGwFAABTAAAA5wEAAD0DAAC4BgAAgQYAAMQEAAA5AQAAbAUAAFMAAADnAQAAOQEAAGwFAABTAAAAsAIAADkBAABsBQAA5wEAAMQEAAA5AQAAbAUAAFMAAADnAQAAOQEAAGwFAABTAAAA5wEAAOIBAABsBQAAmAMAAGsGAACYAwAAWgIAAJgDAACwAgAAmAMAAGwFAABTAAAA5wEAADkBAAC4AQAAgQYAAO0DAADtAwAA5wEAAAwBAAADBwAAAwYAAHEEAABaAgAAawYAAFIGAACbAQAA5wEAADkBAAC4AQAALgIAAOcBAAA5AQAAbAUAAFMAAADnAQAAOQEAAGwFAABTAAAA2gIAAJgDAAC4AQAAagYAAAMHAACAAAAAqAIAAAwBAABSBgAA/QAAAKgCAAB8AAAAUgYAAKoFAACoAgAADAEAALIEAACAAAAAqAIAAOcBAADEBAAA+AEAAGwFAABTAAAA5wEAADkBAABsBQAAUwAAAOcBAAA5AQAAugAAAOcBAAD9AAAAqgUAAKgCAAAMAQAAsgQAAIAAAACoAgAAuAEAAGgFAACtAQAASwAAAK0BAACXBAAAUwAAAKgCAACLAwAAqgUAAIAAAABLAAAAbQAAALIEAABaAgAASwAAAK0BAACXBAAA/QAAAFoCAACtAQAAAwAAAGsBAABaAgAAUwAAAOcBAAA5AQAAsgQAAE8AAACXBAAAUwAAAHEEAABtAAAAcQQAAIEGAABrBgAAuAEAAOcBAADBAwAAAwYAAC4CAABqBgAAOQEAALIEAAADAAAA";
#endif
}

BOOL MB_CALL_TYPE mbUtilIsRegistered(const wchar_t* defaultPath)
{
#if ENABLE_VERIFY && ENABLE_IN_MB_MAIN
    return SqBind::getInst()->loadLicenseAndVerify(std::wstring(defaultPath));
#else
    return TRUE;
#endif
}

BOOL MB_CALL_TYPE mbUtilPrint(mbWebView webviewHandle, mbWebFrameHandle frameId, const mbPrintSettings* settings)
{
#if 1 // ENABLE_IN_MB_MAIN
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return FALSE;
    if (webview->m_printing)
        return FALSE;

    BOOL b = TRUE;
    if (webview->m_printingCallback)
        b = webview->m_printingCallback(webviewHandle, webview->m_printingCallbackParam, kPrintintStepStart, nullptr, nullptr, 0);
    if (!b)
        return FALSE;

    webview->m_printing = new printing::Printing(webviewHandle, frameId);
    webview->setIsMouseKeyMessageEnable(false);
    webview->m_printing->run(settings);
#endif
    return TRUE;
}

class MbJsValue {
public:
    MbJsValue()
    {
        m_id = common::LiveIdDetect::get()->constructed(this);
    }

    ~MbJsValue()
    {
        common::LiveIdDetect::get()->deconstructed(m_id);
    }

    static MbJsValue* wkeJsValueSaveToMb(jsExecState es, jsValue v)
    {
        MbJsValue* mbVal = new MbJsValue();
        mbVal->m_type = kMbJsTypeV8Value;
        mbVal->m_v = v;
        return mbVal;
    }

    static MbJsValue* wkeJsValueToMb(jsExecState es, jsValue v)
    {
        MbJsValue* result = new MbJsValue();
        jsType wkeType = jsTypeOf(v);
        switch (wkeType) {
        case JSTYPE_NUMBER:
            result->m_type = kMbJsTypeNumber;
            result->m_doubleVal = jsToDouble(es, v);
            break;
        case JSTYPE_STRING:
            result->m_type = kMbJsTypeString;
            result->m_strVal = jsToString(es, v);
            break;
        case JSTYPE_BOOLEAN:
            result->m_type = kMbJsTypeBool;
            result->m_boolVal = !!jsToBoolean(es, v);
            break;
        case JSTYPE_OBJECT:
            result->m_type = kMbJsTypeString;
            result->m_strVal = "[Object]";
            break;
        case JSTYPE_FUNCTION:
            result->m_type = kMbJsTypeString;
            result->m_strVal = "[Function]";
            break;
        case JSTYPE_UNDEFINED:
            result->m_type = kMbJsTypeUndefined;
            break;
        case JSTYPE_ARRAY:
            result->m_type = kMbJsTypeString;
            result->m_strVal = "[Array]";
            break;
        case JSTYPE_NULL:
            result->m_type = kMbJsTypeNull;
            break;
        }
        return result;
    }

    int64_t getId() const { return m_id; }
    mbJsType getType() const { return m_type; }

    double getDoubleVal() const { return m_doubleVal; }
    std::string getStrVal() const { return m_strVal; }
    BOOL getBoolVal() const { return m_boolVal; }

    jsValue getJsVal() const { return m_v; }
private:
    int64_t m_id;
    mbJsType m_type;

    double m_doubleVal;
    std::string m_strVal;
    BOOL m_boolVal;

    jsValue m_v;
};

double MB_CALL_TYPE mbJsToDouble(mbJsExecState es, mbJsValue v)
{
    MbJsValue* jsV = (MbJsValue*)common::LiveIdDetect::get()->getPtrLocked(v);
    if (!jsV)
        return 0;

    double result = 0;
    if (kMbJsTypeNumber == jsV->getType())
        result = jsV->getDoubleVal();
    common::LiveIdDetect::get()->unlock(v, jsV);

    return result;
}

BOOL MB_CALL_TYPE mbJsToBoolean(mbJsExecState es, mbJsValue v)
{
    MbJsValue* jsV = (MbJsValue*)common::LiveIdDetect::get()->getPtrLocked(v);
    if (!jsV)
        return false;

    BOOL result = FALSE;
    if (kMbJsTypeBool == jsV->getType())
        result = jsV->getBoolVal();
    common::LiveIdDetect::get()->unlock(v, jsV);

    return result;
}

std::vector<std::vector<char>*>* s_sharedStringBuffers = nullptr;
static const char* createTempCharString(const char* str, size_t length)
{
    if (!str || 0 == length)
        return "";
    std::vector<char>* stringBuffer = new std::vector<char>(length);
    memcpy(&stringBuffer->at(0), str, length * sizeof(char));
    stringBuffer->push_back('\0');

    if (!s_sharedStringBuffers)
        s_sharedStringBuffers = new std::vector<std::vector<char>*>();
    s_sharedStringBuffers->push_back(stringBuffer);
    return &stringBuffer->at(0);
}

template<class T>
static void freeShareds(std::vector<T*>* s_shared)
{
    if (!s_shared)
        return;

    for (size_t i = 0; i < s_shared->size(); ++i) {
        delete s_shared->at(i);
    }
    s_shared->clear();
}

static void freeTempCharStrings()
{
    freeShareds(s_sharedStringBuffers);
}

std::vector<mbJsValue>* s_jsValues;

mbJsType MB_CALL_TYPE mbGetJsValueType(mbJsExecState es, mbJsValue v)
{
    MbJsValue* jsV = (MbJsValue*)common::LiveIdDetect::get()->getPtrLocked(v);
    if (!jsV)
        return kMbJsTypeUndefined;

    mbJsType type = jsV->getType();
    common::LiveIdDetect::get()->unlock(v, jsV);
    return type;
}

const utf8* MB_CALL_TYPE mbJsToString(mbJsExecState es, mbJsValue v)
{
    MbJsValue* jsV = (MbJsValue*)common::LiveIdDetect::get()->getPtrLocked(v);
    if (!jsV)
        return "";

    std::string result;
    if (kMbJsTypeString == jsV->getType())
        result = jsV->getStrVal();
    common::LiveIdDetect::get()->unlock(v, jsV);

    if (0 == result.size())
        return "";

    return createTempCharString(result.c_str(), result.size());
}

void MB_CALL_TYPE mbOnJsQuery(mbWebView webviewHandle, mbJsQueryCallback callback, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);

    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;

    std::function<void(mbJsExecState es, int64_t queryId, int customMsg, const utf8* request)>* closure = nullptr;
    closure = new std::function<void(mbJsExecState es, int64_t queryId, int customMsg, const utf8* request)>(
        /*std::move*/(MB_FROM_HERE, [webviewHandle, callback, param](mbJsExecState es, int64_t queryId, int customMsg, const utf8* request) {
            return callback(webviewHandle, param, es, queryId, customMsg, request);
    }));
    webview->getClosure().setJsQueryClosure(closure);
}

void MB_CALL_TYPE mbResponseQuery(mbWebView webviewHandle, int64_t queryId, int customMsg, const utf8* response)
{
    std::string* requestString = new std::string(response ? response: "");
    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, queryId, customMsg, requestString] {
        std::pair<wkeWebFrameHandle, int>* idInfo = (std::pair<wkeWebFrameHandle, int>*)queryId;
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
        if (!webview) {
            delete idInfo;
            delete requestString;
            return;
        }

//         char queryIdString[30] = { 0 };
//         sprintf(queryIdString, "%d, %d, `", idInfo->second, customMsg);

        wkeWebView wkeWebview = webview->getWkeWebView();
//         std::string injectScript = "window.__onMbQuery__(";
//         injectScript += queryIdString;
//         injectScript += requestString->c_str();
//         injectScript += "`);";
//         wkeRunJsByFrame(wkeWebview, idInfo->first, injectScript.c_str(), false);

        jsExecState es = wkeGetGlobalExecByFrame(wkeWebview, idInfo->first);
        jsValue windowVal = jsGlobalObject(es);
        jsValue jsFunc = jsGet(es, windowVal, "__onMbQuery__");

        jsValue args[3];
        args[0] = jsInt(idInfo->second);
        args[1] = jsInt(customMsg);
        args[2] = jsString(es, requestString->c_str());
        jsCallGlobal(es, jsFunc, args, 3);       

        delete idInfo;
        delete requestString;
    });
}

mbJsExecState MB_CALL_TYPE mbGetGlobalExecByFrame(mbWebView webviewHandle, mbWebFrameHandle frameId)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return nullptr;

    wkeWebView wkeWebview = webview->getWkeWebView();
    return wkeGetGlobalExecByFrame(wkeWebview, (wkeWebFrameHandle)frameId);
}

void* MB_CALL_TYPE mbJsToV8Value(mbJsExecState es, mbJsValue v)
{
    MbJsValue* mbVal = (MbJsValue*)v;
    if (mbVal->getType() != kMbJsTypeV8Value)
        return nullptr;
    return jsToV8Value((jsExecState)es, mbVal->getJsVal());
}

mbJsValue MB_CALL_TYPE mbRunJsSync(mbWebView webviewHandle, mbWebFrameHandle frameId, const utf8* script, BOOL isInClosure)
{
    checkThreadCallIsValidImpl(__FUNCTION__, true);

//     int64_t id = webview->getId();
//     std::string* scriptString = new std::string(script);
//     MbJsValue* mbVal = nullptr;
//     common::ThreadCall::callBlinkThreadSync(MB_FROM_HERE, [id, webview, frameId, scriptString, isInClosure, &mbVal] {
//         if (!common::LiveIdDetect::get()->isLive(id)) {
//             delete scriptString;
//             return;
//         }
//         wkeWebView wkeWebview = webview->getWkeWebView();
//         wkeWebFrameHandle wkeFrameId = (wkeWebFrameHandle)frameId;
//         if ((mbWebFrameHandle)-2 == frameId)
//             wkeFrameId = wkeWebFrameGetMainFrame(wkeWebview);
//         jsValue ret = wkeRunJsByFrame(wkeWebview, wkeFrameId, scriptString->c_str(), !!isInClosure);
// 
//         jsExecState es = wkeGetGlobalExecByFrame(wkeWebview, wkeFrameId);
//         mbVal = MbJsValue::wkeJsValueToMb(es, ret);
//     });
// 
//     if (!mbVal)
//         return 0;
// 
//     common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [] {
//         freeTempCharStrings();
//     });
// 
//     return mbVal->getId();

    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return 0;

    wkeWebView wkeWebview = webview->getWkeWebView();
    wkeWebFrameHandle wkeFrameId = (wkeWebFrameHandle)frameId;
    if ((mbWebFrameHandle)-2 == frameId)
        wkeFrameId = wkeWebFrameGetMainFrame(wkeWebview);
    jsValue ret = wkeRunJsByFrame(wkeWebview, wkeFrameId, script, !!isInClosure);

    jsExecState es = wkeGetGlobalExecByFrame(wkeWebview, wkeFrameId);
    MbJsValue* mbVal = MbJsValue::wkeJsValueSaveToMb(es, ret);

    return (mbJsValue)mbVal;
}

static void callRunJsCallbackOnUiThread(mbWebView webviewHandle, void* param, mbRunJsCallback callback, jsExecState es, MbJsValue* mbVal)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (webview)
        callback(webviewHandle, param, es, mbVal->getId());

    freeTempCharStrings();
    delete mbVal;
}

jsValue wkeRunJsByFrame2(wkeWebView webView, wkeWebFrameHandle frameId, const utf8* script, bool isInClosure, int worldId);

static void runJsOnBlinkThread(mbWebView webviewHandle, mbWebFrameHandle frameId, std::string* scriptString, BOOL isInClosure, mbRunJsCallback callback, void* param, int worldID)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview) {
        delete scriptString;
        return;
    }

    wkeWebView wkeWebview = webview->getWkeWebView();
    wkeWebFrameHandle wkeFrameId = (wkeWebFrameHandle)frameId;
    if ((mbWebFrameHandle)-2 == frameId)
        wkeFrameId = wkeWebFrameGetMainFrame(wkeWebview);

    jsValue ret = wkeRunJsByFrame2(wkeWebview, wkeFrameId, scriptString->c_str(), !!isInClosure, worldID);
    jsExecState es = wkeGetGlobalExecByFrame(wkeWebview, wkeFrameId);

    if (callback) {
        MbJsValue* mbVal = MbJsValue::wkeJsValueToMb(es, ret);
        common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [webviewHandle, param, callback, es, mbVal] {
            callRunJsCallbackOnUiThread(webviewHandle, param, callback, es, mbVal);
        });
    }
    delete scriptString;    
}

void MB_CALL_TYPE mbRunJs(mbWebView webviewHandle, mbWebFrameHandle frameId, const utf8* script, BOOL isInClosure, mbRunJsCallback callback, void* param, void* unuse)
{
    int32_t worldID = (int32_t)unuse;
    std::string* scriptString = new std::string(script);
    if (common::ThreadCall::isBlinkThread())
        runJsOnBlinkThread(webviewHandle, frameId, scriptString, isInClosure, callback, param, worldID);
    else {
        common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, frameId, scriptString, isInClosure, callback, param, worldID] {
            runJsOnBlinkThread(webviewHandle, frameId, scriptString, isInClosure, callback, param, worldID);
        });
    }
}

void MB_CALL_TYPE mbNetHookRequest(mbNetJob jobPtr)
{
    wkeNetHookRequest(jobPtr);
}

const utf8* MB_CALL_TYPE mbNetGetHTTPHeaderField(mbNetJob jobPtr, const char* key, BOOL fromRequestOrResponse)
{
    if (fromRequestOrResponse)
        return wkeNetGetHTTPHeaderField((wkeNetJob)jobPtr, key);
    return wkeNetGetHTTPHeaderFieldFromResponse((wkeNetJob)jobPtr, key);
}

void MB_CALL_TYPE mbNetSetHTTPHeaderField(mbNetJob jobPtr, const wchar_t* key, const wchar_t* value, BOOL response)
{
    if (common::ThreadCall::isBlinkThread())
        wkeNetSetHTTPHeaderField((wkeNetJob)jobPtr, key, value, !!response);
    else {
        std::wstring* keyCopy = new std::wstring((key));
        std::wstring* valueCopy = new std::wstring((value));

        common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [jobPtr, keyCopy, valueCopy, response] {
            wkeNetSetHTTPHeaderField((wkeNetJob)jobPtr, keyCopy->c_str(), valueCopy->c_str(), !!response);
            delete keyCopy;
            delete valueCopy;
        });
    }
}

const char* MB_CALL_TYPE mbNetGetReferrer(mbNetJob jobPtr)
{
    return wkeNetGetReferrer((wkeNetJob)jobPtr);
}

void MB_CALL_TYPE mbNetSetData(mbNetJob jobPtr, void* buf, int len)
{
    if (common::ThreadCall::isBlinkThread())
        wkeNetSetData(jobPtr, buf, len);
    else {
        std::vector<char>* bufferCopy = new std::vector<char>();
        bufferCopy->resize(len);
        memcpy(&bufferCopy->at(0), buf, len);
        common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [jobPtr, bufferCopy] {
            wkeNetSetData(jobPtr, &bufferCopy->at(0), (int)bufferCopy->size());
            delete bufferCopy;
        });
    }    
}

void MB_CALL_TYPE mbNetChangeRequestUrl(mbNetJob jobPtr, const char* url)
{
    //wkeNetChangeRequestUrl(jobPtr, url);
    if (common::ThreadCall::isBlinkThread())
        wkeSetDebugConfig((wkeWebView)jobPtr, "changeRequestUrl", url);
    else {
        std::string* urlCopy = new std::string(url);
        common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [jobPtr, urlCopy] {
            wkeSetDebugConfig((wkeWebView)jobPtr, "changeRequestUrl", urlCopy->c_str());
            delete urlCopy;
        });
    }
}

void MB_CALL_TYPE mbNetSetMIMEType(mbNetJob jobPtr, const char* type)
{
    if (common::ThreadCall::isBlinkThread())
        wkeNetSetMIMEType(jobPtr, type);
    else {
        std::string* typeCopy = new std::string(type);
        common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [jobPtr, typeCopy] {
            wkeNetSetMIMEType(jobPtr, typeCopy->c_str());
            delete typeCopy;
        });
    }
}

const char* MB_CALL_TYPE mbNetGetMIMEType(mbNetJob jobPtr)
{
    return wkeNetGetMIMEType((wkeNetJob)jobPtr, nullptr);
}

void MB_CALL_TYPE mbNetContinueJob(mbNetJob jobPtr)
{
    if (common::ThreadCall::isBlinkThread())
        wkeNetContinueJob(jobPtr);
    else {
        common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [jobPtr] {
            wkeNetContinueJob(jobPtr);
        });
    }
}

const mbSlist* MB_CALL_TYPE mbNetGetRawHttpHeadInBlinkThread(mbNetJob jobPtr)
{
    if (common::ThreadCall::isBlinkThread())
        return (const mbSlist*)wkeNetGetRawHttpHead((wkeNetJob)jobPtr);
    return nullptr;
}

const mbSlist* MB_CALL_TYPE mbNetGetRawResponseHeadInBlinkThread(mbNetJob jobPtr)
{
    if (common::ThreadCall::isBlinkThread())
        return (const mbSlist*)wkeNetGetRawResponseHead((wkeNetJob)jobPtr);
    return nullptr;
}

BOOL MB_CALL_TYPE mbNetHoldJobToAsynCommit(mbNetJob jobPtr)
{
    return wkeNetHoldJobToAsynCommit(jobPtr);
//     if (common::ThreadCall::isBlinkThread())
//         wkeNetHoldJobToAsynCommit(jobPtr);
//     else {
//         common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [jobPtr] {
//             wkeNetHoldJobToAsynCommit(jobPtr);
//         });
//     }
}

void MB_CALL_TYPE mbNetCancelRequest(mbNetJob jobPtr)
{
    wkeNetCancelRequest(jobPtr);
//     if (common::ThreadCall::isBlinkThread())
//         wkeNetCancelRequest(jobPtr);
//     else {
//         common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [jobPtr] {
//             wkeNetCancelRequest(jobPtr);
//         });
//     }
}

struct WebsocketHooks {
    void* param;
    bool(WKE_CALL_TYPE* onConnected)(wkeWebView webView, void* param, mbWebSocketChannel channel);
    bool(WKE_CALL_TYPE* onReceive)(wkeWebView webView, void* param, mbWebSocketChannel channel, int opCode, const char* buf, size_t len, wkeString newData);
    bool(WKE_CALL_TYPE* onSend)(wkeWebView webView, void* param, mbWebSocketChannel channel, int opCode, const char* buf, size_t len, wkeString newData);
    void(WKE_CALL_TYPE* onError)(wkeWebView webView, void* param, mbWebSocketChannel channel);
};

typedef void (WKE_CALL_TYPE* SetHookFn)(mbWebSocketChannel channel, WebsocketHooks* hook);

struct WebsocketHookCallbackParamWrap {
    mbWebView webview;
    void* param;
    mbWebSocketChannel channel;
    mbWebsocketHookCallbacks callbacks;

    static bool WKE_CALL_TYPE onWillConnect(wkeWebView webView, void* param, mbWebSocketChannel channel, wkeString url)
    {
        WebsocketHookCallbackParamWrap* self = (WebsocketHookCallbackParamWrap*)param;
        const utf8* urlStr = wkeToString(url);
        self->channel = channel;
        BOOL needHook = FALSE;
        mbStringPtr newUrl = self->callbacks.onWillConnect(self->webview, self->param, self, urlStr, &needHook);
        if (newUrl)
            wkeSetString(url, mbGetString(newUrl), mbGetStringLen(newUrl));

        if (needHook) {
            SetHookFn setHook = (SetHookFn)wkeGetDebugConfig(nullptr, "setWsHook");

            WebsocketHooks hookInfo;
            hookInfo.param = self;
            hookInfo.onConnected = WebsocketHookCallbackParamWrap::onConnected;
            hookInfo.onReceive = WebsocketHookCallbackParamWrap::onReceive;
            hookInfo.onSend = WebsocketHookCallbackParamWrap::onSend;
            hookInfo.onError = WebsocketHookCallbackParamWrap::onError;
            setHook(channel, &hookInfo);
        }
        
        return !!newUrl;
    }

    static bool WKE_CALL_TYPE onConnected(wkeWebView webView, void* param, mbWebSocketChannel channel)
    {
        WebsocketHookCallbackParamWrap* self = (WebsocketHookCallbackParamWrap*)param;
        return !!self->callbacks.onConnected(self->webview, self->param, channel);
    }

    static bool WKE_CALL_TYPE onReceive(wkeWebView webView, void* param, mbWebSocketChannel channel, int opCode, const char* buf, size_t len, wkeString newData)
    {
        WebsocketHookCallbackParamWrap* self = (WebsocketHookCallbackParamWrap*)param;
        BOOL isContinue = TRUE;
        mbStringPtr newDataRet = self->callbacks.onReceive(self->webview, self->param, channel, opCode, buf, len, &isContinue);
        if (!isContinue) {
            if (newData)
                mbDeleteString(newDataRet);
            return true;
        }

        if (newDataRet) {
            wkeSetStringWithoutNullTermination(newData, mbGetString(newDataRet), mbGetStringLen(newDataRet));
            mbDeleteString(newDataRet);
        }

        return false;
    }

    static bool WKE_CALL_TYPE onSend(wkeWebView webView, void* param, mbWebSocketChannel channel, int opCode, const char* buf, size_t len, wkeString newData)
    {
        WebsocketHookCallbackParamWrap* self = (WebsocketHookCallbackParamWrap*)param;
        BOOL isContinue = TRUE;
        mbStringPtr newDataRet = self->callbacks.onSend(self->webview, self->param, channel, opCode, buf, len, &isContinue);
        if (!isContinue) {
            if (newData)
                mbDeleteString(newDataRet);
            return true;
        }

        if (newDataRet) {
            wkeSetStringWithoutNullTermination(newData, mbGetString(newDataRet), mbGetStringLen(newDataRet));
            mbDeleteString(newDataRet);
        }

        return false;
    }

    static void WKE_CALL_TYPE onError(wkeWebView webView, void* param, mbWebSocketChannel channel)
    {
        WebsocketHookCallbackParamWrap* self = (WebsocketHookCallbackParamWrap*)param;
        self->callbacks.onError(self->webview, self->param, channel);
    }
};

void MB_CALL_TYPE mbNetSetWebsocketCallback(mbWebView webviewHandle, const mbWebsocketHookCallbacks* callbacks, void* param)
{
    checkThreadCallIsValid(__FUNCTION__);

    WebsocketHookCallbackParamWrap* wrap = new WebsocketHookCallbackParamWrap();
    wrap->param = param;
    wrap->callbacks = *callbacks;

    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [wrap](mb::MbWebView* webview) {
        wkeSetDebugConfig(webview->getWkeWebView(), "wsCallback", (const char*)WebsocketHookCallbackParamWrap::onWillConnect);
        wkeSetDebugConfig(webview->getWkeWebView(), "wsCallbackParam", (const char*)wrap);
    });
}

typedef void (WKE_CALL_TYPE* SendWsTextFn)(mbWebSocketChannel channel, const char* buf, size_t len);

void MB_CALL_TYPE mbNetSendWsText(void* channel, const char* buf, size_t len)
{
    SendWsTextFn sendText = (SendWsTextFn)wkeGetDebugConfig(nullptr, "sendWsText");
    sendText(channel, buf, len);
}

void MB_CALL_TYPE mbNetSendWsBlob(void* channel, const char* buf, size_t len)
{
    SendWsTextFn sendBlob = (SendWsTextFn)wkeGetDebugConfig(nullptr, "sendWsBlob");
    sendBlob(channel, buf, len);
}

const utf8* MB_CALL_TYPE mbUtilBase64Encode(const utf8* str)
{
    return wkeUtilBase64Encode(str);
}

const utf8* MB_CALL_TYPE mbUtilBase64Decode(const utf8* str)
{
    return wkeUtilBase64Decode(str);
}

struct UrlRequestWrap {
    mbWebView webviewHandle;
    mbUrlRequestCallbacks callbacks;
    void* param;
};

static void WKE_CALL_TYPE onUrlRequestWillRedirectCallback(wkeWebView webview, void* param, wkeWebUrlRequestPtr oldRequest, wkeWebUrlRequestPtr request, wkeWebUrlResponsePtr redirectResponse)
{
    UrlRequestWrap* self = (UrlRequestWrap*)param;
    mb::MbWebView* mbWebview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(self->webviewHandle);
    if (!webview)
        return;
  
    self->callbacks.willRedirectCallback(self->webviewHandle, self->param,
        (mbWebUrlRequestPtr)oldRequest, (mbWebUrlRequestPtr)request, (mbWebUrlResponsePtr)redirectResponse);
}

static void WKE_CALL_TYPE onUrlRequestDidReceiveResponseCallback(wkeWebView webview, void* param, wkeWebUrlRequestPtr request, wkeWebUrlResponsePtr response)
{
    UrlRequestWrap* self = (UrlRequestWrap*)param;
    mb::MbWebView* mbWebview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(self->webviewHandle);
    if (!webview)
        return;
    self->callbacks.didReceiveResponseCallback(self->webviewHandle, self->param,
        (mbWebUrlRequestPtr)request, (mbWebUrlResponsePtr)response);
}

static void WKE_CALL_TYPE onUrlRequestDidReceiveDataCallback(wkeWebView webview, void* param, wkeWebUrlRequestPtr request, const char* data, int dataLength)
{
    UrlRequestWrap* self = (UrlRequestWrap*)param;
    mb::MbWebView* mbWebview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(self->webviewHandle);
    if (!webview)
        return;
    self->callbacks.didReceiveDataCallback(self->webviewHandle, self->param, (mbWebUrlRequestPtr)request, data, dataLength);
}

static void WKE_CALL_TYPE onUrlRequestDidFailCallback(wkeWebView webview, void* param, wkeWebUrlRequestPtr request, const utf8* error)
{
    UrlRequestWrap* self = (UrlRequestWrap*)param;
    mb::MbWebView* mbWebview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(self->webviewHandle);
    if (!webview)
        return;
    self->callbacks.didFailCallback(self->webviewHandle, self->param, (mbWebUrlRequestPtr)request, error);
}

static void WKE_CALL_TYPE onUrlRequestDidFinishLoadingCallback(wkeWebView webview, void* param, wkeWebUrlRequestPtr request, double finishTime)
{
    UrlRequestWrap* self = (UrlRequestWrap*)param;
    mb::MbWebView* mbWebview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(self->webviewHandle);
    if (!webview)
        return;
    self->callbacks.didFinishLoadingCallback(self->webviewHandle, self->param, (mbWebUrlRequestPtr)request, finishTime);
}

mbWebUrlRequestPtr MB_CALL_TYPE mbNetCreateWebUrlRequest(const utf8* url, const utf8* method, const utf8* mime)
{
    return (mbWebUrlRequestPtr)wkeNetCreateWebUrlRequest(/*webview ? webview->getWkeWebView() : nullptr , */url, method, mime);
}

void MB_CALL_TYPE mbNetAddHTTPHeaderFieldToUrlRequest(mbWebUrlRequestPtr request, const utf8* name, const utf8* value)
{
    wkeNetAddHTTPHeaderFieldToUrlRequest((wkeWebUrlRequestPtr)request, name, value);
}

int MB_CALL_TYPE mbNetGetHttpStatusCode(mbWebUrlResponsePtr response)
{
    return wkeNetGetHttpStatusCode((wkeWebUrlResponsePtr)response);
}

mbRequestType MB_CALL_TYPE mbNetGetRequestMethod(mbNetJob jobPtr)
{
    return (mbRequestType)wkeNetGetRequestMethod((wkeNetJob)jobPtr);
}

long long MB_CALL_TYPE mbNetGetExpectedContentLength(mbWebUrlResponsePtr response)
{
    return wkeNetGetExpectedContentLength((wkeWebUrlResponsePtr)response);
}

const utf8* MB_CALL_TYPE mbNetGetResponseUrl(mbWebUrlResponsePtr response)
{
    return wkeNetGetResponseUrl((wkeWebUrlResponsePtr)response);
}

int MB_CALL_TYPE mbNetStartUrlRequest(mbWebView webviewHandle, mbWebUrlRequestPtr request, void* param, const mbUrlRequestCallbacks* callbacks)
{
    wkeUrlRequestCallbacks callbacksWrap = {
        onUrlRequestWillRedirectCallback ,
        onUrlRequestDidReceiveResponseCallback,
        onUrlRequestDidReceiveDataCallback,
        onUrlRequestDidFailCallback,
        onUrlRequestDidFinishLoadingCallback,
    };

    UrlRequestWrap* wrap = new UrlRequestWrap();

    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);

    wrap->webviewHandle = webviewHandle;
    wrap->callbacks = *callbacks;
    wrap->param = param;
    return wkeNetStartUrlRequest(webview ? webview->getWkeWebView() : nullptr, (wkeWebUrlRequestPtr)request, wrap, &callbacksWrap);
}

void MB_CALL_TYPE mbNetCancelWebUrlRequest(int requestId)
{
    wkeNetCancelWebUrlRequest(requestId);
}

void MB_CALL_TYPE mbSetNodeJsEnable(mbWebView webviewHandle, BOOL b)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (webview)
        webview->setEnableNode(!!b);

    if (b)
        mbSetDebugConfig(webviewHandle, "enableNodejs", "1");
}

void MB_CALL_TYPE mbSetDeviceParameter(mbWebView webviewHandle, const char* device, const char* paramStr, int paramInt, float paramFloat)
{
    std::string* deviceCopy = new std::string(device);
    std::string* paramStrCopy = nullptr;
    if (paramStr)
        paramStrCopy = new std::string(paramStr);

    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, deviceCopy, paramStrCopy, paramInt, paramFloat] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
        if (webview)
            wkeSetDeviceParameter(webview->getWkeWebView(), deviceCopy->c_str(), paramStrCopy ? paramStrCopy->c_str() : nullptr, paramInt, paramFloat);
                
        delete deviceCopy;
        if (paramStrCopy)
            delete paramStrCopy;
    });
}

const utf8* MB_CALL_TYPE mbUtilDecodeURLEscape(const utf8* str)
{
    return wkeUtilDecodeURLEscape(str);
}

const utf8* MB_CALL_TYPE mbUtilEncodeURLEscape(const utf8* str)
{
    return wkeUtilEncodeURLEscape(str);
}

const mbMemBuf* MB_CALL_TYPE mbUtilCreateV8Snapshot(const utf8* str)
{
#if ENABLE_IN_MB_MAIN
    return (const mbMemBuf*)wkeUtilCreateV8Snapshot(str);
#else
    return nullptr;
#endif
}

void MB_CALL_TYPE mbUtilPrintToPdf(mbWebView webviewHandle, mbWebFrameHandle frameId, const mbPrintSettings* settings, mbPrintPdfDataCallback callback, void* param)
{
#if ENABLE_IN_MB_MAIN
    mbPrintSettings* settingsWrap = new mbPrintSettings();
    *settingsWrap = *settings;

    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, settingsWrap, callback, param] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
        if (webview) {
            const wkePdfDatas* datas = wkeUtilPrintToPdf(webview->getWkeWebView(), nullptr, (const wkePrintSettings*)settingsWrap);
            callback(webviewHandle, param, (const mbPdfDatas*)datas);
            wkeUtilRelasePrintPdfDatas(datas);
        }
        delete settingsWrap;
    });
#endif
}

void MB_CALL_TYPE mbUtilPrintToBitmap(mbWebView webviewHandle, mbWebFrameHandle frameId, const mbScreenshotSettings* settings, mbPrintBitmapCallback callback, void* param)
{
#if ENABLE_IN_MB_MAIN
    mbScreenshotSettings* settingsWrap = nullptr;
    if (settings) {
        new mbScreenshotSettings();
        *settingsWrap = *settings;
    }

    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, settingsWrap, callback, param] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
        if (webview) {
            const wkeMemBuf* data = wkePrintToBitmap(webview->getWkeWebView(), nullptr, (const wkeScreenshotSettings*)settingsWrap);
            callback(webviewHandle, param, (const char*)data->data, data->length);
            wkeFreeMemBuf((wkeMemBuf*)data);
        }
        delete settingsWrap;
    });
#endif
}

class ScreenshotWrap {
public:
    ScreenshotWrap(mbWebView webviewHandle, mbOnScreenshot callback, void* param)
    {
        m_webviewHandle = webviewHandle;
        m_callback = callback;
        m_param = param;
    }

    static void WKE_CALL_TYPE onScreenshotWrap(wkeWebView webview, void* param, const char* data, size_t size)
    {
        ScreenshotWrap* self = (ScreenshotWrap*)param;
        self->m_callback(self->m_webviewHandle, self->m_param, data, size);
    }

private:
    mbWebView m_webviewHandle;
    mbOnScreenshot m_callback;
    void* m_param;
};

void MB_CALL_TYPE mbUtilScreenshot(mbWebView webviewHandle, const mbScreenshotSettings* settings, mbOnScreenshot callback, void* param)
{
#if ENABLE_IN_MB_MAIN
    mbScreenshotSettings* settingsWrap = nullptr;
    if (settings) {
        settingsWrap = new mbScreenshotSettings();
        *settingsWrap = *settings;
    }

    ScreenshotWrap* wrap = new ScreenshotWrap(webviewHandle, callback, param);

    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, settingsWrap, wrap, callback, param] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
        if (webview) {
            wkeScreenshot(webview->getWkeWebView(), (const wkeScreenshotSettings*)settingsWrap, ScreenshotWrap::onScreenshotWrap, wrap);
        } else {
            callback(NULL_WEBVIEW, param, nullptr, 0);
        }
        delete settingsWrap;
    });
#endif
}

BOOL MB_CALL_TYPE mbUtilsSilentPrint(mbWebView webView, const char* settings)
{
    return FALSE;
}

mbMemBuf* MB_CALL_TYPE mbCreateMemBuf(mbWebView webView, void* buf, size_t length)
{
    return (mbMemBuf*)wkeCreateMemBuf(nullptr, buf, length);
}

void MB_CALL_TYPE mbFreeMemBuf(mbMemBuf* buf)
{
    wkeFreeMemBuf((wkeMemBuf*)buf);
}

void MB_CALL_TYPE mbSetAudioMuted(mbWebView webview, BOOL b)
{
    
}

BOOL MB_CALL_TYPE mbIsAudioMuted(mbWebView webview)
{
    return FALSE;
}

void MB_CALL_TYPE mbOnAcceleratedPaint(mbWebView webView, mbAcceleratedPaintCallback callback, void* callbackParam)
{

}

static void onGetPdfPageDataCallback(mbWebView webviewHandle, mbOnGetPdfPageDataCallback callback, void* param, void* data, size_t size)
{
    std::vector<char>* dataCopy = nullptr;
    if (data) { 
        dataCopy = new std::vector<char>();
        dataCopy->resize(size);
        memcpy(&dataCopy->at(0), data, size);
    }

    common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [webviewHandle, dataCopy, callback, param] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);

        if (dataCopy && webview)
            callback(webviewHandle, param, &dataCopy->at(0), dataCopy->size());
        else
            callback(webviewHandle, param, nullptr, 0);

        if (dataCopy)
            delete dataCopy;
    });
}

void MB_CALL_TYPE mbGetPdfPageData(mbWebView webviewHandle, mbOnGetPdfPageDataCallback callback, void* param)
{
#if ENABLE_IN_MB_MAIN
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (webview)
        return;

    if (!webview->getWkeWebView())
        return callback(webviewHandle, param, nullptr, 0);
    
    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [webviewHandle, callback, param] {
        mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
        if (!webview)
            return onGetPdfPageDataCallback(NULL_WEBVIEW, callback, param, nullptr, 0);

        printing::PdfViewerPlugin* plugin = (printing::PdfViewerPlugin*)wkeGetUserKeyValue(webview->getWkeWebView(), "ChildPdfViewerPlugin");
        if (!plugin)
            return onGetPdfPageDataCallback(webviewHandle, callback, param, nullptr, 0);
        
        std::vector<char>* data = plugin->getPdfData();
        if (!data || 0 == data->size())
            return onGetPdfPageDataCallback(webviewHandle, callback, param, nullptr, 0);

        onGetPdfPageDataCallback(webviewHandle, callback, param, &data->at(0), data->size());
    });
#endif // if ENABLE_IN_MB_MAIN
}

void MB_CALL_TYPE mbSetUserKeyValue(mbWebView webviewHandle, const char* key, void* value)
{
	checkThreadCallIsValid(__FUNCTION__);
    if (!key)
        return;

    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;
    webview->setUserKeyValue(key, value);
}

void* MB_CALL_TYPE mbGetUserKeyValue(mbWebView webviewHandle, const char* key)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return nullptr;
    return webview->getUserKeyValue(key);
}

mbPostBodyElements* MB_CALL_TYPE mbNetGetPostBody(void* jobPtr)
{
    return (mbPostBodyElements*)wkeNetGetPostBody(jobPtr);
}

mbPostBodyElements* MB_CALL_TYPE mbNetCreatePostBodyElements(mbWebView webviewHandle, size_t length)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return nullptr;
    return (mbPostBodyElements*)wkeNetCreatePostBodyElements(webview->getWkeWebView(), length);
}

void MB_CALL_TYPE mbNetFreePostBodyElements(mbPostBodyElements* elements)
{
    wkeNetFreePostBodyElements((wkePostBodyElements*)elements);
}

mbPostBodyElement* MB_CALL_TYPE mbNetCreatePostBodyElement(mbWebView webviewHandle)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return nullptr;
    return (mbPostBodyElement*)wkeNetCreatePostBodyElement(webview->getWkeWebView());
}

void MB_CALL_TYPE mbNetFreePostBodyElement(mbPostBodyElement* element)
{
    return wkeNetFreePostBodyElement((wkePostBodyElement*)element);
}

void MB_CALL_TYPE mbSetDiskCacheEnabled(mbWebView webviewHandle, BOOL enable)
{
    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [enable] {
        wkeSetDebugConfig(nullptr, "diskCache", (enable ? "1" : "0"));
    });
}

void MB_CALL_TYPE mbSetDiskCachePath(mbWebView webviewHandle, const WCHAR* path)
{
    std::wstring* pathString = new std::wstring(path);
    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [pathString] {
        std::string pathUtf8 = common::utf16ToUtf8(pathString->c_str());
        wkeSetDebugConfig(nullptr, "diskCachePath", pathUtf8.c_str());

        delete pathString;
    });
}

void MB_CALL_TYPE mbSetDiskCacheLimit(mbWebView webviewHandle, size_t limit)
{
//     common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [limit] {
//         std::string pathUtf8 = std::to_string(limit);
//         wkeSetDebugConfig(nullptr, "diskCacheLimit", pathUtf8.c_str());
//     });
}

void MB_CALL_TYPE mbSetDiskCacheLimitDisk(mbWebView webviewHandle, size_t limit)
{
//     common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [limit] {
//         std::string pathUtf8 = std::to_string(limit);
//         wkeSetDebugConfig(nullptr, "diskCacheLimitDisk", pathUtf8.c_str());
//     });
}

void MB_CALL_TYPE mbSetDiskCacheLevel(mbWebView webviewHandle, int Level)
{
//     common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [Level] {
//         std::string pathUtf8 = std::to_string(Level);
//         wkeSetDebugConfig(nullptr, "diskCacheLevel", pathUtf8.c_str());
//     });
}

void MB_CALL_TYPE mbSetWindowTitle(mbWebView webviewHandle, const utf8* title)
{
    HWND hwnd = mbGetHostHWND(webviewHandle);
    if (!hwnd)
        return;
    std::wstring titleW = common::utf8ToUtf16(title);
    ::SetWindowTextW(hwnd, titleW.c_str());
}

void MB_CALL_TYPE mbSetWindowTitleW(mbWebView webviewHandle, const wchar_t* title)
{
    HWND hwnd = mbGetHostHWND(webviewHandle);
    if (!hwnd)
        return;
    ::SetWindowTextW(hwnd, title);
}

void MB_CALL_TYPE mbMoveWindow(mbWebView webviewHandle, int x, int y, int w, int h)
{
    HWND hwnd = mbGetHostHWND(webviewHandle);
    if (!hwnd)
        return;
    ::SetWindowPos(hwnd, NULL, x, y, w, h, SWP_NOZORDER | SWP_NOACTIVATE);
}

void MB_CALL_TYPE mbUtilSetDefaultPrinterSettings(mbWebView webviewHandle, const mbDefaultPrinterSettings* settings)
{
    if (!printing::s_defaultPrinterSettings)
        printing::s_defaultPrinterSettings = new mbDefaultPrinterSettings();
    *printing::s_defaultPrinterSettings = *settings;
}

void MB_CALL_TYPE mbEnableHighDPISupport()
{
    //     if (!IsUser32AndGdi32Available())
    //         return;

    // Enable per-monitor V2 if it is available (Win10 1703 or later).
    //     if (EnablePerMonitorV2())
    //         return;

    // Fall back to per-monitor DPI for older versions of Win10 instead of
    // Win8.1 since Win8.1 does not have EnableChildWindowDpiMessage,
    // necessary for correct non-client area scaling across monitors.
//     XP_PROCESS_DPI_AWARENESS processDpiAwareness =
//         GetVersion() >= Version::WIN10 ? 
//          XP_PROCESS_PER_MONITOR_DPI_AWARE
//         : 
//         XP_PROCESS_SYSTEM_DPI_AWARE;
//     if (S_OK != SetProcessDpiAwarenessXp(processDpiAwareness)) {
//         // For windows versions where SetProcessDpiAwareness is not available or
//         // failed, try its predecessor.
//         BOOL result = ::SetProcessDPIAwareXp();
//     }
    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [] {
        wkeEnableHighDPISupport();
    });
}

void MB_CALL_TYPE mbRunMessageLoop()
{
//     MSG msg = { 0 };
//     while (true) {
//         if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
//             if (WM_QUIT == msg.message)
//                 break;
//             ::TranslateMessage(&msg);
//             ::DispatchMessageW(&msg);
//         }
//         mbWake(NULL_WEBVIEW);
//         ::Sleep(2);
//     }
    common::ThreadCall::uiMessageLoop();
}

void MB_CALL_TYPE mbGetCaretRect(mbWebView webviewHandle, mbRect* r)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;
    *r = webview->getCaretRect();
}

void MB_CALL_TYPE mbNetEnableResPacket(mbWebView webviewHandle, const WCHAR* pathName)
{
    checkThreadCallIsValid(__FUNCTION__);
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;

    webview->setPacketPathName(pathName);
}

void MB_CALL_TYPE mbWebFrameGetMainWorldScriptContext(mbWebView webviewHandle, mbWebFrameHandle frameId, v8ContextPtr contextOut)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return;

    wkeWebFrameHandle wkeFrameId = (wkeWebFrameHandle)frameId;
    if ((mbWebFrameHandle)-2 == frameId)
        wkeFrameId = wkeWebFrameGetMainFrame(webview->getWkeWebView());
    return wkeWebFrameGetMainWorldScriptContext(webview->getWkeWebView(), wkeFrameId, contextOut);
}

v8Isolate MB_CALL_TYPE mbGetBlinkMainThreadIsolate()
{
    return wkeGetBlinkMainThreadIsolate();
}

void MB_CALL_TYPE mbInsertCSSByFrame(mbWebView webviewHandle, mbWebFrameHandle frameId, const utf8* cssText)
{
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [frameId, cssText](mb::MbWebView* webview) {
        wkeInsertCSSByFrame(webview->getWkeWebView(), (wkeWebFrameHandle)frameId, cssText);
    });
}

void MB_CALL_TYPE mbSetEditable(mbWebView webviewHandle, bool editable)
{
    common::ThreadCall::callBlinkThreadAsyncWithValid(MB_FROM_HERE, webviewHandle, [editable](mb::MbWebView* webview) {
        wkeSetEditable(webview->getWkeWebView(), editable);
    });
}

void MB_CALL_TYPE mbOnNodeCreateProcess(mbWebView webviewHandle, mbNodeOnCreateProcessCallback callback, void* param)
{
    wkeNodeOnCreateProcess(nullptr, (wkeNodeOnCreateProcessCallback)callback, param);
}

mbWebView MB_CALL_TYPE mbGetWebViewForCurrentContext()
{
    wkeWebView wkeWebview = wkeGetWebViewForCurrentContext();
    mbWebView webviewHandle = (mbWebView)wkeGetUserKeyValue(wkeWebview, "MbWebView");
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return NULL_WEBVIEW;
    return webviewHandle;
}

int MB_CALL_TYPE mbGetContentWidth(mbWebView webviewHandle)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return 1;
    return wkeGetContentWidth(webview->getWkeWebView());
}

int MB_CALL_TYPE mbGetContentHeight(mbWebView webviewHandle)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return 1;
    return wkeGetContentHeight(webview->getWkeWebView());
}

BOOL MB_CALL_TYPE mbRegisterEmbedderCustomElement(mbWebView webviewHandle, mbWebFrameHandle frameId, const char* name, void* options, void* outResult)
{
    mb::MbWebView* webview = (mb::MbWebView*)common::LiveIdDetect::get()->getPtr(webviewHandle);
    if (!webview)
        return FALSE;
    return wkeRegisterEmbedderCustomElement(webview->getWkeWebView(), (mbWebFrameHandle)frameId, name, options, outResult);
}

void MB_CALL_TYPE mbOnThreadIdle(mbThreadCallback callback, void* param1, void* param2)
{
    common::ThreadCall::setThreadIdle(callback, param1, param2);
}

void MB_CALL_TYPE mbOnBlinkThreadInit(mbThreadCallback callback, void* param1, void* param2)
{
    common::ThreadCall::setBlinkThreadInited(callback, param1, param2);
}

void MB_CALL_TYPE mbCallBlinkThreadAsync(mbThreadCallback callback, void* param1, void* param2)
{
    common::ThreadCall::callBlinkThreadAsync(MB_FROM_HERE, [callback, param1, param2]() {
        callback(param1, param2);
    });
}

void MB_CALL_TYPE mbCallBlinkThreadSync(mbThreadCallback callback, void* param1, void* param2)
{
    common::ThreadCall::callBlinkThreadSync(MB_FROM_HERE, [callback, param1, param2]() {
        callback(param1, param2);
    });
}

void MB_CALL_TYPE mbCallUiThreadSync(mbThreadCallback callback, void* param1, void* param2)
{
    common::ThreadCall::callUiThreadSync(MB_FROM_HERE, [callback, param1, param2]() {
        callback(param1, param2);
    });
}

void MB_CALL_TYPE mbCallUiThreadAsync(mbThreadCallback callback, void* param1, void* param2)
{
    common::ThreadCall::callUiThreadAsync(MB_FROM_HERE, [callback, param1, param2]() {
        callback(param1, param2);
    });
}
