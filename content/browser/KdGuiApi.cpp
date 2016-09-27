/*
 *  Copyright (C) 2000 Harri Porten (porten@kde.org)
 *  Copyright (C) 2006 Jon Shier (jshier@iastate.edu)
 *  Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reseved.
 *  Copyright (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 *  Copyright (C) 2009 Google Inc. All rights reseved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 *  USA
 */

// #include "SQ/DOMWrapperWorld.h"
// #include "SQ/SqDOMWindowShell.h"

#include "KWebPage.h"
#include "Document.h"
#include "ScriptController.h"
#include "KdGuiApi.h"
#include "KdGuiApiImp.h"

#include "AtomicString.h"
#include "HTMLNames.h"
#include "QualifiedName.h"
#include "SVGNames.h"
#include "XLinkNames.h"
#include "MathMLNames.h"
#include "XMLNSNames.h"
#include "XMLNames.h"

#include "IntRect.h"
#include "cpp/KdValArray.h"

#include "SVGTextMetrics.h"

#include <WTF/Vector.h>
#include <wtf/PageBlock.h>
#include <wtf/RandomNumber.h>
#include <platform/win/SystemInfo.h>

#ifdef _MANAGED
#pragma managed(push, off)
#endif

static ATOM _RegisterClass(HINSTANCE hInstance);
static LRESULT CALLBACK _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#define MAX_LOADSTRING 100
WCHAR szTitle[MAX_LOADSTRING] = L"";
WCHAR szWindowClass[MAX_LOADSTRING] = L"Internet Explorer Server";

#define WM_SHOWDEBUGNODEDATA (WM_APP + 0xcccc)

typedef blink::KWebPage* KdPagePtr;

KDEXPORT KdGuiObjPtr KDCALL
KdCreateGuiObj(void* pForeignPtr)
{
    KdGuiObjPtr kdHandle = new KdGuiObj;

    kdHandle->bHaveTimer = false;
    kdHandle->pForeignPtr = pForeignPtr;

    blink::InitializeLoggingChannelsIfNecessary();
    blink::ScriptController::initializeThreading();
    WTF::initializeMainThread();

    _RegisterClass(0);

    return kdHandle;
}

KDEXPORT KdPagePtr KDCALL
KdCreateRealWndAttachedWebPage(
    KdGuiObjPtr kdHandle,
    KdPageInfoPtr pPageInfo,
    void* pForeignPtr,
    bool bShow
    )
{
    _RegisterClass(0);

    blink::WebPage* page = blink::WebPage::createWindowByRealWnd(kdHandle, pPageInfo, pForeignPtr);
    if (bShow) {
        ::ShowWindow(page->getHWND(), SW_NORMAL);
        ::UpdateWindow(page->getHWND());
    }

    return page;
}

KDEXPORT HWND KDCALL
KdGetHWNDFromPagePtr(KdPagePtr kdPage)
{
    return kdPage->getHWND();
}

KDEXPORT void KDCALL
KdCloseRealWndAttachedWebPage(
    KdGuiObjPtr kdHandle,
    KdPagePtr pPage
    )
{
    HWND hWnd = pPage->getHWND();
    pPage->windowCloseRequested();

    ::CloseWindow(hWnd); // 这里面会post一个close消息,负责 delete pPage;
    
}

KDEXPORT bool KDCALL
KdLoadPageFormUrl(KdPagePtr kdPageHandle, LPCWSTR lpUrl)
{
    kdPageHandle->loadFormUrl(lpUrl);
    return true;
}

KDEXPORT bool KDCALL
KdLoadPageFormData(KdPagePtr kdPageHandle, const void* lpData, int nLen)
{
    kdPageHandle->loadFormData(lpData, nLen);
    return true;
}

KDEXPORT void KDCALL
KdPostResToAsynchronousLoad(KdPagePtr kdPageHandle, const WCHAR* pUrl, void* pResBuf, int nResBufLen, bool bNeedSavaRes)
{
    kdPageHandle->postResToAsynchronousLoad(pUrl, pResBuf, nResBufLen, bNeedSavaRes);
}

KDEXPORT void KDCALL
KdGetMainVM(KdGuiObjPtr kdHandle)
{
    //WebCore::mainThreadNormalWorld()->getScriptVM();
}

KDEXPORT void KDCALL
KdGetPageVM(KdPagePtr kdPageHandle)
{
    // 如果页面没调用到脚本，脚本可能没被初始化。不过windowShell会帮助做这事情
    //kdPageHandle->frame()->script()->windowShell(WebCore::mainThreadNormalWorld())->getSqThread()->getScriptVM();
}

KDEXPORT void KDCALL
KdRegisterMsgHandle(KdPagePtr kdPageHandle, PFN_KdPageWinMsgCallback pPreCallBack, PFN_KdPageWinMsgCallback pPostCallBack)
{
    kdPageHandle->m_callbacks.m_msgPreCallBack = pPreCallBack;
    kdPageHandle->m_callbacks.m_msgPostCallBack = pPostCallBack;
}

// 注册资源处理回调, 比如读取本地一个文件的时候回调
KDEXPORT void KDCALL
KdRegisterResHandle(KdPagePtr kdPageHandle, PFN_KdResCallback pCallBack)
{
     kdPageHandle->m_callbacks.m_resHandle = pCallBack;
}

// 注册资源别名查询，一般用在调试脚本时
KDEXPORT void KDCALL
KdRegisterResOtherNameQuery(KdPagePtr kdPageHandle, PFN_KdResCallback pCallBack)
{
    kdPageHandle->m_callbacks.m_resOtherNameQuery = pCallBack;
}

// 注册绘图回调
KDEXPORT void KDCALL
KdRegisterPaintCallback(KdPagePtr kdPageHandle, PFN_KdPagePaintCallback pCallBack)
{
    kdPageHandle->m_callbacks.m_paint = pCallBack;
}

KDEXPORT void KDCALL
KdRepaintRequested(KdPagePtr kdPageHandle, const RECT* repaintRect)
{
    kdPageHandle->repaintRequested(WebCore::IntRect(*repaintRect));
}

// 注册XML解析完毕时候webcore发回的回调，相当于入口函数
KDEXPORT void KDCALL
KdRegisterXMLOnRealy(KdPagePtr kdPageHandle, PFN_KdPageCallback pCallBack)
{
    kdPageHandle->m_callbacks.m_xmlHaveFinished = pCallBack;
}

KDEXPORT void KDCALL
KdRegisterUninit(KdPagePtr kdPageHandle, PFN_KdPageCallback pCallBack)
{
    kdPageHandle->m_callbacks.m_unintCallBack = pCallBack;
}

KDEXPORT void KDCALL
KdRegisterScriptInit(KdPagePtr kdPageHandle, PFN_KdPageScriptInitCallback pCallBack)
{
    kdPageHandle->m_callbacks.m_scriptInitCallBack = pCallBack;
}

KDEXPORT void KDCALL
KdRegisterScriptCallback(KdPagePtr kdPageHandle, NPInvokeFunctionPtr pCallBack)
{
    kdPageHandle->m_callbacks.m_javascriptCallCppPtr = pCallBack;
}

//////////////////////////////////////////////////////////////////////////

KDEXPORT void KDCALL
KdSendTimerEvent(KdPagePtr kdPageHandle)
{
    kdPageHandle->timerFired();
}

KDEXPORT void KDCALL
KdSendResizeEvent(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    kdPageHandle->resizeEvent(hWnd, message, wParam, lParam);
}

KDEXPORT void KDCALL
KdSendPaintEvent(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    kdPageHandle->paintEvent(hWnd, message, wParam, lParam);
}

KDEXPORT void KDCALL
KdSendMouseEvent(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    kdPageHandle->mouseEvent(hWnd, message, wParam, lParam);
}

KDEXPORT void KDCALL
KdSendCaptureChanged(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    kdPageHandle->captureChangedEvent(hWnd, message, wParam, lParam);
}

KDEXPORT void KDCALL 
KdSendKillFocusEvent(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    kdPageHandle->killFocusEvent(hWnd, message, wParam, lParam);
}

KDEXPORT void KDCALL
KdDeletePageNotCloseWindow(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{    
    kdPageHandle->windowCloseRequested();
    delete kdPageHandle;
}

KDEXPORT void KDCALL
KdIsDraggableRegionNcHitTest(KdPagePtr kdPageHandle)
{
    kdPageHandle->setIsDraggableRegionNcHitTest();
}

KDEXPORT int KDCALL
KdSendInputEvent(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return kdPageHandle->inputEvent(hWnd, message, wParam, lParam);
}

// KDEXPORT bool KDCALL
// KdSendOtherEvent(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
// {
//     return kdPageHandle->otherEvent(hWnd, message, wParam, lParam);
// }

KDEXPORT void KDCALL KdSetBackgroundColor(KdPagePtr kdPageHandle, COLORREF c)
{
    return kdPageHandle->setBackgroundColor(c);
}

KDEXPORT void KDCALL KdShowDebugNodeData(KdPagePtr kdPageHandle)
{
    return kdPageHandle->showDebugNodeData();
}

KDEXPORT void KDCALL
KdInitThread()
{
    //WTF_MAIN_THREAD_MUTEX();

    WTF::initializeThreading();
    WTF::ThreadSpecificInitTls();
    WTF::initializeMainThread();

    WTF::currentTime();
    StringImpl::empty();
    WTF::pageSize();
    WTF::randomNumber();
    WebCore::windowsVersionForUAString();

    AtomicString::init();
    WebCore::HTMLNames::init();
    WebCore::QualifiedName::init();
    WebCore::SVGNames::init();
    WebCore::XLinkNames::init();
    //MathMLNames::init();
    WebCore::XMLNSNames::init();
    WebCore::XMLNames::init();

//    WebCore::SVGTextMetrics::emptyMetrics();

    WebCore::InitializeLoggingChannelsIfNecessary();
    WebCore::ScriptController::initializeThreading();
}

KDEXPORT void KDCALL
KdUninitThread()
{
//    WTF::ThreadSpecificThreadExit();
}

static ATOM _RegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex = {0};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style			= CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc	= _WndProc;
    wcex.cbClsExtra		= 0;
    wcex.cbWndExtra		= 0;
    wcex.hInstance		= hInstance;
    //wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KDGUITEST));
    wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
    //wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_KDGUITEST);
    wcex.lpszClassName	= szWindowClass;
    //wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

#if 0
#define OUTPUTSTR(x) OutputDebugStringW(x)
#else
#define OUTPUTSTR(x) 
#endif


static bool IsInputEvent(UINT message)
{
    switch (message) {
        case WM_KEYUP:
        case WM_KEYDOWN:
        case WM_CHAR:
            //OutputDebugStringW(L"IsInputEvent\n");
            return true;
            
        case WM_IME_STARTCOMPOSITION:OUTPUTSTR(L"WM_IME_STARTCOMPOSITION\n");return true;
        case WM_IME_ENDCOMPOSITION:OUTPUTSTR(L"WM_IME_ENDCOMPOSITION\n");return true;
        case WM_IME_COMPOSITION:OUTPUTSTR(L"WM_IME_COMPOSITION\n");return true;
        case WM_IME_SETCONTEXT:OUTPUTSTR(L"WM_IME_SETCONTEXT\n");return true;
        case WM_IME_NOTIFY:OUTPUTSTR(L"WM_IME_NOTIFY\n");return true;
        case WM_IME_CONTROL:OUTPUTSTR(L"WM_IME_CONTROL\n");return true;
        case WM_IME_COMPOSITIONFULL:OUTPUTSTR(L"WM_IME_COMPOSITIONFULL\n");return true;
        case WM_IME_SELECT:OUTPUTSTR(L"WM_IME_SELECT\n");return true;
        case WM_IME_CHAR:OUTPUTSTR(L"WM_IME_CHAR\n");return true;
        case WM_IME_REQUEST:OUTPUTSTR(L"WM_IME_REQUEST\n");return true;
        case WM_IME_KEYDOWN:OUTPUTSTR(L"WM_IME_KEYDOWN\n");return true;
        case WM_IME_KEYUP:OUTPUTSTR(L"WM_IME_KEYUP\n");return true;
    }
    
    return false;
}

static LRESULT CALLBACK _WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    bool bNeedCallDefWindowProc = true;
    KdPagePtr pKdGUIPage = 0;
    LRESULT lResult = 0;

    if( message == WM_NCCREATE ) {
        LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        pKdGUIPage = static_cast<KdPagePtr>(lpcs->lpCreateParams);
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pKdGUIPage));
        
        pKdGUIPage->Init(hWnd);
        pKdGUIPage->m_messageStackVar = NULL;
    } else {
        pKdGUIPage = reinterpret_cast<KdPagePtr>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
//         if( message == WM_NCDESTROY && pKdGUIPage != NULL ) {
//             return 0;}
    }

    if (!pKdGUIPage) {
        goto Exit1; }

    PFN_KdPageWinMsgCallback msgPostCallBack = pKdGUIPage->m_callbacks.m_msgPostCallBack;
    void* pMainContext = pKdGUIPage->getKdGuiObjPtr() ? pKdGUIPage->getKdGuiObjPtr()->pForeignPtr : 0;
    void* pPageContext = pKdGUIPage->getForeignPtr();

    if (pKdGUIPage->m_callbacks.m_msgPreCallBack) {
        lResult = pKdGUIPage->m_callbacks.m_msgPreCallBack(pKdGUIPage, 
            pKdGUIPage->getKdGuiObjPtr() ? pKdGUIPage->getKdGuiObjPtr()->pForeignPtr : 0, 
            pKdGUIPage->getForeignPtr(),
            hWnd, message, wParam, lParam, &bNeedCallDefWindowProc);
        if (false == bNeedCallDefWindowProc) {
            return lResult;}
    }

    if (IsInputEvent(message)) {
        bNeedCallDefWindowProc = true;
        if (1 != KdSendInputEvent(pKdGUIPage, hWnd, message, wParam, lParam)) {
            bNeedCallDefWindowProc = false;}
              
        goto Exit0;
    }

    switch (message) {
    case WM_TIMER:
        KdSendTimerEvent(pKdGUIPage);
        break;
    case WM_SIZE:
        KdSendResizeEvent(pKdGUIPage, hWnd, message, wParam, lParam);
        break;
    case WM_PAINT:
        KdSendPaintEvent(pKdGUIPage, hWnd, message, wParam, lParam);
        break;
    case WM_NCDESTROY:
        KdDeletePageNotCloseWindow(pKdGUIPage, hWnd, message, wParam, lParam);
        pKdGUIPage = 0;
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
        KdSendMouseEvent(pKdGUIPage, hWnd, message, wParam, lParam);
        break;
    case WM_CAPTURECHANGED: // 这条消息会在WM_NCLBUTTONDOWN后收到，要做点特殊处理~详情见：http://hi.baidu.com/killdbg/item/03a64ea738382d038919d3f0
        KdSendCaptureChanged(pKdGUIPage, hWnd, message, wParam, lParam);
        break;
    case WM_NCCALCSIZE:
        if ((BOOL)wParam) {
            NCCALCSIZE_PARAMS* lpncsp = (NCCALCSIZE_PARAMS FAR*)lParam;
            lpncsp = 0;
        }
        
        break;
    case WM_CLOSE: // no break
//         KdDeletePageNotCloseWindow(pKdGUIPage, hWnd, message, wParam, lParam);
//         pKdGUIPage = 0;
        break;
    case WM_KILLFOCUS:
        KdSendKillFocusEvent(pKdGUIPage, hWnd, message, wParam, lParam);
        break;
    case WM_SHOWDEBUGNODEDATA: // 0x14CCC = 85196
        KdShowDebugNodeData(pKdGUIPage);
        break;
    default:
//        bNeedCallDefWindowProc = !KdSendOtherEvent(pKdGUIPage, hWnd, message, wParam, lParam);
        bNeedCallDefWindowProc = true;
        break;
    }

Exit0:
    if (msgPostCallBack) {
        //bNeedCallDefWindowProc = true;
        lResult = msgPostCallBack(pKdGUIPage, pMainContext, pPageContext,
            hWnd, message, wParam, lParam, &bNeedCallDefWindowProc);
    }

    if (false == bNeedCallDefWindowProc) {
        return lResult;}

Exit1:
    return DefWindowProc(hWnd, message, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////

void KDCALL KVAEmpty(void* pThis)
{
    ((KdValArray*)pThis)->Empty();
}

bool KDCALL KVAIsEmpty(void* pThis)
{
    return ((KdValArray*)pThis)->IsEmpty();
}

bool KDCALL KVAAdd(void* pThis, LPCVOID pData)
{
    return ((KdValArray*)pThis)->Add(pData);
}

bool KDCALL KVARemove(void* pThis, int iIndex)
{
    return ((KdValArray*)pThis)->Remove(iIndex);
}

bool KDCALL KVAResize(void* pThis, int iIndex)
{
    return ((KdValArray*)pThis)->Resize(iIndex);
}

int KDCALL KVAGetSize(void* pThis)
{
    return ((KdValArray*)pThis)->GetSize();
}

void KDCALL KVASetSize(void* pThis, int nSize)
{
    ((KdValArray*)pThis)->SetSize(nSize);
}

LPVOID KDCALL KVAGetData(void* pThis)
{
    return ((KdValArray*)pThis)->GetData();
}

LPVOID KDCALL KVAGetAt(void* pThis, int iIndex)
{
    return ((KdValArray*)pThis)->GetAt(iIndex);
}

KDEXPORT bool KDCALL
KdInvokeScript(KdPagePtr kdPageHandle, NPIdentifier methodName, const NPVariant* args, uint32_t argCount, NPVariant* result)
{
    return kdPageHandle->invokeScript(methodName, args, argCount, result); 
}

BOOL APIENTRY DllMain( 
    HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
    )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}