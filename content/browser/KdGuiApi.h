#ifndef KDGUI_h
#define KDGUI_h

#include "KdPageInfo.h"
//#include "npruntime.h"

#ifdef WEBKIT_DLL
#define KDEXPORT extern "C" _declspec(dllexport)
#else
#define KDEXPORT extern "C"
#endif

#ifndef KDCALL
#define KDCALL __stdcall
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

typedef struct _KdGuiObj* KdGuiObjPtr;

namespace blink {
    class WebPage;
}

typedef blink::WebPage* KdPagePtr;
class KdValArray;
class IKdGuiBuffer;

typedef LRESULT (__stdcall* PFN_KdPageWinMsgCallback) (
    KdPagePtr pKdPagePtr,
    void* pMainContext,
    void* pPageContext,
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam,
    bool* pbNeedContinue
    );

typedef LRESULT (__stdcall* PFN_KdPageCallback) (
    KdPagePtr pKdPagePtr,
    void* pMainContext,
    void* pPageContext,
    HWND hWnd
    );

typedef LRESULT (__stdcall* PFN_KdResCallback) (
    KdPagePtr pKdPagePtr,
    void* pMainContext,
    void* pPageContext,
    HWND hWnd,
    const WCHAR* pURL,
    void* pAllocate
    );

typedef struct SQVM* HSQUIRRELVM;
typedef LRESULT (__stdcall* PFN_KdPageScriptInitCallback) (
    KdPagePtr pKdPagePtr,
    void* pMainContext,
    void* pPageContext,
    HWND hWnd,
    HSQUIRRELVM v
    );

enum KdPagePaintStep {
    KDPPaintStepPrePaintToMemoryDC,
    KDPPaintStepPostPaintToMemoryDC,
    KDPPaintStepPostPaintToScreenDC
};
typedef LRESULT (__stdcall* PFN_KdPagePaintCallback) (
    KdPagePtr pKdPagePtr,
    void* pMainContext,
    void* pPageContext,
    HWND hWnd,
    KdPagePaintStep emKdPagePaintStep,
    void** ppCallBackContext,
    bool* pbNeedContinue,
    const RECT* rtPaint,
    HDC hMemoryDC,
    HDC hPaintDC
    );

KDEXPORT KdGuiObjPtr KDCALL KdCreateGuiObj(void* pForeignPtr);

KDEXPORT KdPagePtr KDCALL KdCreateRealWndAttachedWebPage(
    KdGuiObjPtr kdHandle, 
    KdPageInfoPtr pPageInfo,
    void* pForeignPtr,
    bool bShow
    );

KDEXPORT HWND KDCALL KdGetHWNDFromPagePtr(KdPagePtr kdPage);

KDEXPORT void KDCALL
KdCloseRealWndAttachedWebPage(
    KdGuiObjPtr kdHandle,
    KdPagePtr pPage
    );

KDEXPORT bool KDCALL KdLoadPageFormUrl(KdPagePtr kdPageHandle, LPCWSTR lpUrl);
KDEXPORT bool KDCALL KdLoadPageFormData(KdPagePtr kdPageHandle, const void* lpData, int nLen);

KDEXPORT void KDCALL KdPostResToAsynchronousLoad(KdPagePtr kdPageHandle, const WCHAR* pUrl, void* pResBuf, int nResBufLen, bool bNeedSavaRes);

KDEXPORT void KDCALL KdRegisterXMLOnRealy(KdPagePtr kdPageHandle, PFN_KdPageCallback pCallBack);
KDEXPORT void KDCALL KdRegisterUninit(KdPagePtr kdPageHandle, PFN_KdPageCallback pCallBack);
KDEXPORT void KDCALL KdRegisterScriptInit(KdPagePtr kdPageHandle, PFN_KdPageScriptInitCallback pCallBack);
KDEXPORT void KDCALL KdRegisterResHandle(KdPagePtr kdPageHandle, PFN_KdResCallback pCallBack);
KDEXPORT void KDCALL KdRegisterResOtherNameQuery(KdPagePtr kdPageHandle, PFN_KdResCallback pCallBack);
KDEXPORT void KDCALL KdRegisterPaintCallback(KdPagePtr kdPageHandle, PFN_KdPagePaintCallback pCallBack);
//KDEXPORT void KDCALL KdRegisterScriptCallback(KdPagePtr kdPageHandle, NPInvokeFunctionPtr pCallBack);

KDEXPORT void KDCALL KdRepaintRequested(KdPagePtr kdPageHandle, const RECT* repaintRect);

KDEXPORT void KDCALL KdSendTimerEvent(KdPagePtr kdPageHandle);
KDEXPORT void KDCALL KdSendResizeEvent(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
KDEXPORT void KDCALL KdSendPaintEvent(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
KDEXPORT void KDCALL KdSendMouseEvent(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
KDEXPORT int  KDCALL KdSendInputEvent(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

KDEXPORT void KDCALL KdSendCaptureChanged(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
KDEXPORT void KDCALL KdSendKillFocusEvent(KdPagePtr kdPageHandle, HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

KDEXPORT void KDCALL KdRegisterMsgHandle(KdPagePtr kdPageHandle, PFN_KdPageWinMsgCallback pPreCallBack, PFN_KdPageWinMsgCallback pPostCallBack);

KDEXPORT void KDCALL KdIsDraggableRegionNcHitTest(KdPagePtr kdPageHandle);

KDEXPORT void KDCALL KdInitThread();
KDEXPORT void KDCALL KdUninitThread();

KDEXPORT void KDCALL KdSetBackgroundColor(KdPagePtr kdPageHandle, COLORREF c);

KDEXPORT void KDCALL KdShowDebugNodeData(KdPagePtr kdPageHandle);

//KDEXPORT bool KDCALL KdInvokeScript(KdPagePtr kdPageHandle, NPIdentifier methodName, const NPVariant* args, uint32_t argCount, NPVariant* result);

//////////////////////////////////////////////////////////////////////////

KDEXPORT void KDCALL KVAEmpty(void* pThis);
KDEXPORT bool KDCALL KVAIsEmpty(void* pThis);
KDEXPORT bool KDCALL KVAAdd(void* pThis, LPCVOID pData);
KDEXPORT bool KDCALL KVARemove(void* pThis, int iIndex);
KDEXPORT bool KDCALL KVAResize(void* pThis, int iIndex);
KDEXPORT int KDCALL KVAGetSize(void* pThis);
KDEXPORT void KDCALL KVASetSize(void* pThis, int nSize);
KDEXPORT LPVOID KDCALL KVAGetData(void* pThis);
KDEXPORT LPVOID KDCALL KVAGetAt(void* pThis, int iIndex);

#endif // KDGUI_h


