
#include "browser/api/ApiWebContents.h"
#include "browser/api/WindowList.h"
#include "browser/api/ApiApp.h"
#include "browser/api/WindowInterface.h"
#include "browser/api/ProtocolInterface.h"
#include "browser/api/MenuEventNotif.h"
#include "browser/api/PopupWindow.h"
#include "common/OptionsSwitches.h"
#include "common/NodeRegisterHelp.h"
#include "common/ThreadCall.h"
#include "common/StringUtil.h"
#include "common/api/EventEmitter.h"
#include "common/IdLiveDetect.h"
#include "common/WinUserMsg.h"
#include "common/DragAction.h"
#include "common/asar/AsarUtil.h"
#include "renderer/WebviewPlugin.h"
#include "node/src/node.h"
#include "node/src/env.h"
#include "node/src/env-inl.h"
#include "node/uv/include/uv.h"
#include "wke.h"
#include "gin/per_isolate_data.h"
#include "gin/object_template_builder.h"
#include "base/files/file_path.h"
#include "base/win/windows_version.h"
#include "Resource.h"
#include <shellapi.h>
#include <ole2.h>

namespace atom {

const wchar_t WindowInterface::kElectronClassName[] = L"mb_electron_window";

class Window : public mate::EventEmitter<Window>, public WindowInterface {
public:
    explicit Window(v8::Isolate* isolate, v8::Local<v8::Object> wrapper) {
        gin::Wrappable<Window>::InitWith(isolate, wrapper);
        m_webContents = nullptr;
        m_state = WindowUninited;
        m_hWnd = nullptr;
        m_hIMC = nullptr;
        m_cursorInfoType = 0;
        m_isCursorInfoTypeAsynGetting = false;
        m_memoryBMP = nullptr;
        m_memoryDC = nullptr;
        m_isDestroyApiBeCalled = false;
        m_isMaximized = false;
        m_isDocumentEdited = false;
        m_isIgnoreMouseEvents = false;

        m_clientRect.left = 0;
        m_clientRect.top = 0;
        m_clientRect.right = 0;
        m_clientRect.bottom = 0;
        m_memoryBmpSize.cx = 0;
        m_memoryBmpSize.cy = 0;
        m_contentsSize.cx = 0;
        m_contentsSize.cy = 0;
        
        ::InitializeCriticalSection(&m_memoryCanvasLock);
        ::InitializeCriticalSection(&m_mouseMsgQueueLock);

        m_draggableRegion = ::CreateRectRgn(0, 0, 0, 0);
        m_dragAction = nullptr;
        
        m_id = IdLiveDetect::get()->constructed(this);
    }

    ~Window() {
        char* output = (char*)malloc(0x100);
        sprintf(output, "~Window %p\n", this);
        OutputDebugStringA(output);
        free(output);

        delete m_createWindowParam;

        ::DeleteObject(m_draggableRegion);

        if (m_memoryBMP)
            ::DeleteObject(m_memoryBMP);
        if (m_memoryDC)
            ::DeleteDC(m_memoryDC);

        //ThreadCall::callUiThreadSync([this] {
            //delete data->m_webContents;
            //::SendMessage(this->m_hWnd, WM_CLOSE, 0, 0);
        //});
        //WindowList::getInstance()->removeWindow(this);

        IdLiveDetect::get()->deconstructed(m_id);

        ::DeleteCriticalSection(&m_memoryCanvasLock);
    }

    static const int WM_COPYGLOBALDATA = 0x0049;
    static const int MSG_FLT_ADD = 1;
    typedef WINUSERAPI BOOL WINAPI CHANGEWINDOWMESSAGEFILTER(UINT message, DWORD dwFlag);
    static void changeMessageProi() {
        HINSTANCE hDllInst = LoadLibraryW(L"user32.dll");
        if (hDllInst) {
            CHANGEWINDOWMESSAGEFILTER *pAddMessageFilterFunc = (CHANGEWINDOWMESSAGEFILTER *)GetProcAddress(hDllInst, "ChangeWindowMessageFilter");
            if (pAddMessageFilterFunc) {
                pAddMessageFilterFunc(WM_DROPFILES, MSG_FLT_ADD);
                pAddMessageFilterFunc(WM_COPYDATA, MSG_FLT_ADD);
                pAddMessageFilterFunc(WM_COPYGLOBALDATA, MSG_FLT_ADD);
            }
            FreeLibrary(hDllInst);
        }
    }

    static void init(v8::Local<v8::Object> target, node::Environment* env) {
        changeMessageProi();

        v8::Isolate* isolate = env->isolate();
        gin::PerIsolateData* perIsolateData = new gin::PerIsolateData(isolate, nullptr);

        v8::Local<v8::FunctionTemplate> prototype = v8::FunctionTemplate::New(isolate, newFunction);
        
        prototype->SetClassName(v8::String::NewFromUtf8(isolate, "BrowserWindow"));
        gin::ObjectTemplateBuilder builder(isolate, prototype->InstanceTemplate());
        builder.SetMethod("_getWebContents", &Window::_getWebContentsApi);
        builder.SetMethod("close", &Window::closeApi);
        builder.SetMethod("focus", &Window::focusApi);
        builder.SetMethod("blur", &Window::blurApi);
        builder.SetMethod("isFocused", &Window::isFocusedApi);
        builder.SetMethod("show", &Window::showApi);
        builder.SetMethod("showInactive", &Window::showInactiveApi);
        builder.SetMethod("hide", &Window::hideApi);
        builder.SetMethod("destroy", &Window::destroyApi);
        builder.SetMethod("isVisible", &Window::isVisibleApi);
        builder.SetMethod("isEnabled", &Window::isEnabledApi);
        builder.SetMethod("maximize", &Window::maximizeApi);
        builder.SetMethod("unmaximize", &Window::unmaximizeApi);
        builder.SetMethod("isMaximized", &Window::isMaximizedApi);
        builder.SetMethod("minimize", &Window::minimizeApi);
        builder.SetMethod("restore", &Window::restoreApi);
        builder.SetMethod("isMinimized", &Window::isMinimizedApi);
        builder.SetMethod("setFullScreen", &Window::setFullScreenApi);
        builder.SetMethod("isFullScreen", &Window::isFullScreenApi);
        builder.SetMethod("setAspectRatio", &Window::nullFunction);
        builder.SetMethod("previewFile", &Window::nullFunction);
        builder.SetMethod("closeFilePreview", &Window::nullFunction);
        builder.SetMethod("setParentWindow", &Window::nullFunction);
        builder.SetMethod("getParentWindow", &Window::nullFunction);
        builder.SetMethod("getChildWindows", &Window::nullFunction);
        builder.SetMethod("isModal", &Window::isModalApi);
        builder.SetMethod("getNativeWindowHandle", &Window::getNativeWindowHandleApi);
        builder.SetMethod("getBounds", &Window::getBoundsApi);
        builder.SetMethod("setBounds", &Window::setBoundsApi);
        builder.SetMethod("getSize", &Window::getSizeApi);
        builder.SetMethod("setSize", &Window::setSizeApi);
        builder.SetMethod("getContentBounds", &Window::getContentBoundsApi);
        builder.SetMethod("setContentBounds", &Window::setContentBoundsApi);
        builder.SetMethod("getContentSize", &Window::getContentSizeApi);
        builder.SetMethod("setContentSize", &Window::setContentSizeApi);
        builder.SetMethod("setMinimumSize", &Window::setMinimumSizeApi);
        builder.SetMethod("getMinimumSize", &Window::getMinimumSizeApi);
        builder.SetMethod("setMaximumSize", &Window::setMaximumSizeApi);
        builder.SetMethod("getMaximumSize", &Window::getMaximumSizeApi);
        builder.SetMethod("setSheetOffset", &Window::nullFunction);
        builder.SetMethod("setResizable", &Window::setResizableApi);
        builder.SetMethod("isResizable", &Window::isResizableApi);
        builder.SetMethod("setMovable", &Window::setMovableApi);
        builder.SetMethod("isMovable", &Window::isMovableApi);
        builder.SetMethod("setMinimizable", &Window::setMinimizableApi);
        builder.SetMethod("isMinimizable", &Window::isMinimizableApi);
        builder.SetMethod("isMaximizable", &Window::isMaximizableApi);
        builder.SetMethod("setMaximizable", &Window::setMaximizableApi);
        builder.SetMethod("setFullScreenable", &Window::setFullScreenableApi);
        builder.SetMethod("isFullScreenable", &Window::isFullScreenableApi);
        builder.SetMethod("setClosable", &Window::setClosableApi);
        builder.SetMethod("isClosable", &Window::isClosableApi);
        builder.SetMethod("setAlwaysOnTop", &Window::setAlwaysOnTopApi);
        builder.SetMethod("isAlwaysOnTop", &Window::isAlwaysOnTopApi);
        builder.SetMethod("center", &Window::centerApi);
        builder.SetMethod("setPosition", &Window::setPositionApi);
        builder.SetMethod("getPosition", &Window::getPositionApi);
        builder.SetMethod("setTitle", &Window::setTitleApi);
        builder.SetMethod("getTitle", &Window::getTitleApi);
        builder.SetMethod("flashFrame", &Window::flashFrameApi);
        builder.SetMethod("setSkipTaskbar", &Window::setSkipTaskbarApi);
        builder.SetMethod("setKiosk", &Window::nullFunction);
        builder.SetMethod("isKiosk", &Window::nullFunction);
        builder.SetMethod("setBackgroundColor", &Window::setBackgroundColorApi);
        builder.SetMethod("setHasShadow", &Window::nullFunction);
        builder.SetMethod("hasShadow", &Window::nullFunction);
        builder.SetMethod("setRepresentedFilename", &Window::nullFunction);
        builder.SetMethod("getRepresentedFilename", &Window::nullFunction);
        builder.SetMethod("setDocumentEdited", &Window::setDocumentEditedApi);
        builder.SetMethod("isDocumentEdited", &Window::isDocumentEditedApi);
        builder.SetMethod("setIgnoreMouseEvents", &Window::setIgnoreMouseEventsApi);
        builder.SetMethod("setContentProtection", &Window::setContentProtectionApi);
        builder.SetMethod("setFocusable", &Window::setFocusableApi);
        builder.SetMethod("focusOnWebView", &Window::focusOnWebViewApi);
        builder.SetMethod("blurWebView", &Window::blurApi);
        builder.SetMethod("isWebViewFocused", &Window::isWebViewFocusedApi);
        builder.SetMethod("setOverlayIcon", &Window::setOverlayIconApi);
        builder.SetMethod("setThumbarButtons", &Window::setThumbarButtonsApi);
        builder.SetMethod("setMenu", &Window::setMenuApi);
        builder.SetMethod("setAutoHideMenuBar", &Window::setAutoHideMenuBarApi);
        builder.SetMethod("isMenuBarAutoHide", &Window::isMenuBarAutoHideApi);
        builder.SetMethod("setMenuBarVisibility", &Window::setMenuBarVisibilityApi);
        builder.SetMethod("isMenuBarVisible", &Window::isMenuBarVisibleApi);
        builder.SetMethod("setVisibleOnAllWorkspaces", &Window::setVisibleOnAllWorkspacesApi);
        builder.SetMethod("isVisibleOnAllWorkspaces", &Window::isVisibleOnAllWorkspacesApi);
        builder.SetMethod("setVibrancy", &Window::nullFunction);
        builder.SetMethod("hookWindowMessage", &Window::hookWindowMessageApi);
        builder.SetMethod("isWindowMessageHooked", &Window::isWindowMessageHookedApi);
        builder.SetMethod("unhookWindowMessage", &Window::unhookWindowMessageApi);
        builder.SetMethod("unhookAllWindowMessages", &Window::unhookAllWindowMessagesApi);
        builder.SetMethod("setThumbnailClip", &Window::setThumbnailClipApi);
        builder.SetMethod("setThumbnailToolTip", &Window::setThumbnailToolTipApi);
        builder.SetMethod("setAppDetails", &Window::setAppDetailsApi);
        builder.SetMethod("setIcon", &Window::setIconApi);
        builder.SetMethod("setProgressBar", &Window::setProgressBarApi);
        builder.SetMethod("isDestroyed", &Window::isDestroyedApi);
        
        builder.SetProperty("id", &Window::getIdApi);

        //NODE_SET_PROTOTYPE_METHOD(prototype, &Window::"id", &Window::nullFunction);
        builder.SetMethod("_getWebContents", &Window::_getWebContentsApi);

        gin::Dictionary browserWindowClass(isolate, prototype->GetFunction());
        browserWindowClass.SetMethod("getFocusedWindow", &Window::getFocusedWindowApi);
        browserWindowClass.SetMethod("fromId", &Window::fromIdApi);
        browserWindowClass.SetMethod("getAllWindows", &Window::getAllWindowsApi);
        browserWindowClass.SetMethod("fromWebContents", &Window::fromWebContentsApi);
        
        constructor.Reset(isolate, prototype->GetFunction());
        target->Set(v8::String::NewFromUtf8(isolate, "BrowserWindow"), prototype->GetFunction());
    }

    // WindowInterface impl
    virtual bool isClosed() override {
        return m_state == WindowDestroyed;
    }

    virtual void close() override {
        ::DestroyWindow(m_hWnd); // go to WM_NCDESTROY
    }

    virtual v8::Local<v8::Object> getWrapper() override {
        return GetWrapper(isolate());
    }

    virtual int getId() const override {
        return m_id;
    }

    virtual WebContents* getWebContents() const override {
        return m_webContents;
    }

    virtual HWND getHWND() const override {
        return m_hWnd;
    }
    
    static bool isRectEqual(const RECT& a, const RECT& b) {
        return (a.left == b.left) && (a.top == b.top) && (a.right == b.right) && (a.bottom == b.bottom);
    }

    static bool isPointInRect(const RECT& a, const POINT& b) {
        return b.x >= a.left && b.x <= a.right
            && b.y >= a.top && b.y <= a.bottom;
    }

    void onPaintUpdatedInCompositeThread(const HDC hdc, int x, int y, int cx, int cy) {
        HWND hWnd = m_hWnd;
        RECT rectDest;
        ::GetClientRect(hWnd, &rectDest);
        SIZE sizeDest = { rectDest.right - rectDest.left, rectDest.bottom - rectDest.top };
        if (0 == sizeDest.cx * sizeDest.cy)
            return;
            
        HDC hSreenDC = ::GetWindowDC(hWnd);
        if (!m_memoryDC)
            m_memoryDC = ::CreateCompatibleDC(hSreenDC);

        if (!m_memoryBMP || !isRectEqual(m_clientRect, rectDest)) {
            m_clientRect = rectDest;
            m_memoryBmpSize = sizeDest;

            if (m_memoryBMP)
                ::DeleteObject((HGDIOBJ)m_memoryBMP);
            m_memoryBMP = ::CreateCompatibleBitmap(hSreenDC, sizeDest.cx, sizeDest.cy);
        }
        ::ReleaseDC(hWnd, hSreenDC);

        DWORD flag = SRCCOPY;
        if (m_createWindowParam->transparent)
            flag |= CAPTUREBLT;

        BOOL b = FALSE;
        HBITMAP hbmpOld = (HBITMAP)::SelectObject(m_memoryDC, m_memoryBMP);
        //if (!m_createWindowParam->transparent) {
            ::BitBlt(m_memoryDC, x, y, cx, cy, hdc, x, y, flag);
//         } else {
//             int copy_width = cx;
//             int copy_height = cy;
//             BLENDFUNCTION blendFunction = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
//             b = ::GdiAlphaBlend(m_memoryDC, x, y, copy_width, copy_height, hdc, x, y, copy_width, copy_height, blendFunction);
//         }

    }

    typedef struct _UPDATELAYEREDWINDOWINFO {
        DWORD cbSize;
        HDC hdcDst;
        const POINT* pptDst;
        const SIZE* psize;
        HDC hdcSrc;
        const POINT* pptSrc;
        COLORREF crKey;
        const BLENDFUNCTION* pblend;
        DWORD dwFlags;
        const RECT* prcDirty;
    } STR_UPDATELAYEREDWINDOWINFO;

    bool drawToNativeLayeredContext(HDC dc, HDC source_dc, const RECT* srcRect, const RECT* clientRect) {
        BOOL b = FALSE;

        int clientWidth = clientRect->right - clientRect->left;
        int clientHeight = clientRect->bottom - clientRect->top;

        BLENDFUNCTION blendFunction = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
        typedef BOOL(WINAPI* PFN_UpdateLayeredWindow) (HWND hWnd, HDC hdcDst, POINT *pptDst, SIZE *psize, HDC hdcSrc, POINT *pptSrc, COLORREF crKey, BLENDFUNCTION *pblend, DWORD dwFlags);
        static PFN_UpdateLayeredWindow s_pUpdateLayeredWindow = NULL;
        if (NULL == s_pUpdateLayeredWindow)
            s_pUpdateLayeredWindow = reinterpret_cast<PFN_UpdateLayeredWindow>(GetProcAddress(GetModuleHandleW(L"user32.dll"), "UpdateLayeredWindow"));

        SIZE clientSize = { clientWidth, clientHeight };
        POINT zero = { 0 };

        typedef int(WINAPI* PFN_UpdateLayeredWindowIndirect) (HWND hWnd, STR_UPDATELAYEREDWINDOWINFO const* pULWInfo);
        static PFN_UpdateLayeredWindowIndirect s_pUpdateLayeredWindowIndirect = NULL;
        if (NULL == s_pUpdateLayeredWindowIndirect)
            s_pUpdateLayeredWindowIndirect = reinterpret_cast<PFN_UpdateLayeredWindowIndirect>(GetProcAddress(GetModuleHandleW(L"user32.dll"), "UpdateLayeredWindowIndirect"));

        if (0 && s_pUpdateLayeredWindowIndirect) {
            STR_UPDATELAYEREDWINDOWINFO info = { sizeof(STR_UPDATELAYEREDWINDOWINFO), dc, nullptr, &clientSize, source_dc, nullptr, RGB(0xFF, 0xFF, 0xFF), &blendFunction, ULW_ALPHA, srcRect };
            b = s_pUpdateLayeredWindowIndirect(m_hWnd, &info);
        } else {
            b = s_pUpdateLayeredWindow(m_hWnd, dc, nullptr, &clientSize, source_dc, &zero, RGB(0xFF, 0xFF, 0xFF), &blendFunction, ULW_ALPHA);
        }
        return !!b;
    }

    void saveContentWidthHeight() {
        m_contentsSize.cx = wkeGetContentWidth(m_webContents->getWkeView());
        m_contentsSize.cy = wkeGetContentHeight(m_webContents->getWkeView());
    }

    void onPaintUpdatedInUiThread(int x, int y, int cx, int cy) {
        ::EnterCriticalSection(&m_memoryCanvasLock);

        HDC hdcScreen = ::GetDC(m_hWnd);
        DWORD flag = SRCCOPY;
        BOOL b = FALSE;
        if (m_createWindowParam->transparent) {
            RECT srcRect = { x, y, cx, cy };
            RECT clientRect;
            ::GetClientRect(m_hWnd, &clientRect);

            drawToNativeLayeredContext(hdcScreen, m_memoryDC, &srcRect, &clientRect);

            ::ReleaseDC(m_hWnd, hdcScreen);
        } else
            b = ::BitBlt(hdcScreen, x, y, cx, cy, m_memoryDC, x, y, flag);
        ::ReleaseDC(m_hWnd, hdcScreen);

        ::LeaveCriticalSection(&m_memoryCanvasLock);
    }

    static void staticOnPaintUpdatedInCompositeThread(wkeWebView webView, Window* self, const HDC hdc, int x, int y, int cx, int cy) {
        ::EnterCriticalSection(&self->m_memoryCanvasLock);
        self->onPaintUpdatedInCompositeThread(hdc, x, y, cx, cy);
        ::LeaveCriticalSection(&self->m_memoryCanvasLock);

        int id = self->m_id;
        if (self->m_createWindowParam->transparent) {
            ThreadCall::callUiThreadAsync([id, self, x, y, cx, cy] {
                if (IdLiveDetect::get()->isLive(id))
                    self->onPaintUpdatedInUiThread(x, y, cx, cy);
            });
        } else {
            RECT rc = { x, y, x + cx, y + cy };
            ::InvalidateRect(self->m_hWnd, &rc, false);
        }

        ThreadCall::callBlinkThreadAsync([id, self] {
            if (!IdLiveDetect::get()->isLive(id))
                return;
            ::EnterCriticalSection(&self->m_memoryCanvasLock);
            self->saveContentWidthHeight();
            ::LeaveCriticalSection(&self->m_memoryCanvasLock);
        });
    }

    void onPaintMessage(HWND hWnd) {
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

        Window* self = this;
        int id = self->m_id;
        ThreadCall::callBlinkThreadAsync([id, self] {
            if (IdLiveDetect::get()->isLive(id))
                self->saveContentWidthHeight();
        });

        bool isResied = !isRectEqual(rcClient, m_clientRect);
        ::EnterCriticalSection(&m_memoryCanvasLock);
        
        if (m_memoryBmpSize.cx < width)
            width = m_memoryBmpSize.cx;
        if (m_memoryBmpSize.cy < height)
            height = m_memoryBmpSize.cy;

        if (0 != width && 0 != height && m_memoryBMP && m_memoryDC) {
            HBITMAP hbmpOld = (HBITMAP)::SelectObject(m_memoryDC, m_memoryBMP);
            BOOL b = ::BitBlt(hdc, destX, destY, width, height, m_memoryDC, srcX, srcY, SRCCOPY);
            ::SelectObject(m_memoryDC, hbmpOld);
        }
        ::LeaveCriticalSection(&m_memoryCanvasLock);

        ::EndPaint(hWnd, &ps);
    }

    bool isDraggableRegionNcHitTest(HWND hWnd) {
        bool handle = false;
        if (!m_draggableRegion)
            return handle;

        POINT pos;
        ::GetCursorPos(&pos);
        ::ScreenToClient(hWnd, &pos);

        handle = !!::PtInRegion(m_draggableRegion, pos.x, pos.y);
        return handle;
    }

    bool doDraggableRegionNcHitTest(HWND hWnd) {
        bool handle = isDraggableRegionNcHitTest(hWnd);
        if (handle && !m_createWindowParam->isFrame)
            ::PostMessage(hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
        return handle;
    }

    void delayDoMouseMsg(wkeWebView webview) {
        ::EnterCriticalSection(&m_mouseMsgQueueLock);
        if (0 == m_mouseMsgQueue.size()) {
            ::LeaveCriticalSection(&m_mouseMsgQueueLock);
            return;
        }

        std::list<MouseMsg*> mouseMsgQueue;
        bool hasMouseMove = false; 
        MouseMsg* mouseMsg = nullptr;
        std::list<MouseMsg*>::reverse_iterator it = m_mouseMsgQueue.rbegin();
        for (; it != m_mouseMsgQueue.rend(); ++it) {
            mouseMsg = *it;
            if (WM_MOUSEMOVE != (*it)->message) {
                mouseMsgQueue.push_back(new MouseMsg(mouseMsg->message, mouseMsg->x, mouseMsg->y, mouseMsg->flags));
            } else if (!hasMouseMove) {
                hasMouseMove = true;
                mouseMsgQueue.push_back(new MouseMsg(mouseMsg->message, mouseMsg->x, mouseMsg->y, mouseMsg->flags));
            }
            delete mouseMsg;
        }
        m_mouseMsgQueue.clear();
        ::LeaveCriticalSection(&m_mouseMsgQueueLock);

        for (it = mouseMsgQueue.rbegin(); it != mouseMsgQueue.rend(); ++it) {
            mouseMsg = *it;
            wkeFireMouseEvent(webview, mouseMsg->message, mouseMsg->x, mouseMsg->y, mouseMsg->flags);
            delete mouseMsg;
        }
    }

    void onMouseMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        if (m_isIgnoreMouseEvents)
            return;

        int id = m_id;
        wkeWebView webview = m_webContents->getWkeView();
        if (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_RBUTTONDOWN) {
            if (!doDraggableRegionNcHitTest(hWnd)) {
                ::SetFocus(hWnd);
                ::SetCapture(hWnd);
            }
        } else if (message == WM_LBUTTONUP || message == WM_MBUTTONUP || message == WM_RBUTTONUP) {
            ::ReleaseCapture();
        }

        int x = LOWORD(lParam);
        int y = HIWORD(lParam);

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

        ::EnterCriticalSection(&m_mouseMsgQueueLock);
        //MouseMsg mouseMsg(message, x, y, flags);
        m_mouseMsgQueue.push_back(new MouseMsg(message, x, y, flags));
        ::LeaveCriticalSection(&m_mouseMsgQueueLock);

        Window* self = this;
        ThreadCall::callBlinkThreadAsync([self, id, webview] {
            if (!IdLiveDetect::get()->isLive(id))
                return;
            self->delayDoMouseMsg(webview);
        });
        if (WM_LBUTTONDOWN == message)
            doDraggableRegionNcHitTest(hWnd);
    }

    void onCursorChange() {
        if (m_isCursorInfoTypeAsynGetting)
            return;
        m_isCursorInfoTypeAsynGetting = true;

        int id = m_id;
        wkeWebView webview = m_webContents->getWkeView();
        Window* self = this;
        ThreadCall::callBlinkThreadAsync([webview, self, id] {
            if (!IdLiveDetect::get()->isLive(id))
                return;
            self->m_isCursorInfoTypeAsynGetting = false;
            int cursorType = wkeGetCursorInfoType(webview);
            if (cursorType == self->m_cursorInfoType)
                return;
            self->m_cursorInfoType = cursorType;
            ::PostMessage(self->m_hWnd, WM_SETCURSOR/*_ASYN*/, 0, 0);
        });
    }

    bool setCursorInfoTypeByCache() {
        RECT rc;
        ::GetClientRect(m_hWnd, &rc);

        POINT pt;
        ::GetCursorPos(&pt);
        ::ScreenToClient(m_hWnd, &pt);
        if (!::PtInRect(&rc, pt))
            return false;

        HCURSOR hCur = NULL;
        switch (m_cursorInfoType) {
        case WkeCursorInfoPointer:
            hCur = ::LoadCursor(NULL, IDC_ARROW);
            break;
        case WkeCursorInfoIBeam:
            hCur = ::LoadCursor(NULL, IDC_IBEAM);
            break;
        case WkeCursorInfoHand:
            hCur = ::LoadCursor(NULL, IDC_HAND);
            break;
        case WkeCursorInfoWait:
            hCur = ::LoadCursor(NULL, IDC_WAIT);
            break;
        case WkeCursorInfoHelp:
            hCur = ::LoadCursor(NULL, IDC_HELP);
            break;
        case WkeCursorInfoNorthResize:
            hCur = ::LoadCursor(NULL, IDC_SIZENS);
            break;
        case WkeCursorInfoSouthWestResize:
        case WkeCursorInfoNorthEastResize:
            hCur = ::LoadCursor(NULL, IDC_SIZENESW);
            break;
        case WkeCursorInfoSouthResize:
        case WkeCursorInfoNorthSouthResize:
            hCur = ::LoadCursor(NULL, IDC_SIZENS);
            break;
        case WkeCursorInfoNorthWestResize:
        case WkeCursorInfoSouthEastResize:
            hCur = ::LoadCursor(NULL, IDC_SIZENWSE);
            break;
        case WkeCursorInfoWestResize:
        case WkeCursorInfoEastWestResize:
        case WkeCursorInfoColumnResize:
        case WkeCursorInfoEastResize:
            hCur = ::LoadCursor(NULL, IDC_SIZEWE);
            break;
        case WkeCursorInfoNorthEastSouthWestResize:
        case WkeCursorInfoNorthWestSouthEastResize:
            hCur = ::LoadCursor(NULL, IDC_SIZEALL);
            break;
        default:
            hCur = ::LoadCursor(NULL, IDC_ARROW);
            break;
        }

        if (hCur) {
            ::SetCursor(hCur);
            return true;
        }

        return false;
    }

    void onDragFiles(HDROP hDrop) {
        int count = ::DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0); // How many files were dropped? 

        int id = m_id;
        WebContents* webContents = m_webContents;
        std::vector<std::vector<wchar_t>*>* fileNames = new std::vector<std::vector<wchar_t>*>();
        for (int i = 0; i <count; i++) {
            int pathlength = ::DragQueryFile(hDrop, i, NULL, 0) + 1;
            if (pathlength >= MAX_PATH || pathlength <= 1)
                continue;

            fileNames->push_back(new std::vector<wchar_t>());
            fileNames->at(i)->resize(pathlength);
            ::DragQueryFile(hDrop, i,&(fileNames->at(i)->at(0)), pathlength);
        }

        ::DragFinish(hDrop);

        POINT* screenPos = new POINT();
        ::GetCursorPos(screenPos);

        POINT* clientPos = new POINT();
        clientPos->x = screenPos->x;
        clientPos->y = screenPos->y;
        ::ScreenToClient(m_hWnd, clientPos);

        ThreadCall::callBlinkThreadAsync([webContents, id, fileNames, clientPos, screenPos] {
            if (!IdLiveDetect::get()->isLive(id))
                return;

            std::vector<wkeString> files;
            for (size_t i = 0; i < fileNames->size(); ++i) {
                files.push_back(wkeCreateStringW(&(fileNames->at(i)->at(0)), fileNames->at(i)->size()));
            }
            wkeSetDragFiles(webContents->getWkeView(), clientPos, screenPos, &files[0], files.size());
            
            delete clientPos;
            delete screenPos;
            for (size_t i = 0; i < fileNames->size(); ++i) {
                wkeDeleteString(files.at(i));
                delete fileNames->at(i);
            }
            delete fileNames;
        });
    }

    LRESULT windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        wkeWebView webview = m_webContents->getWkeView();
        Window* self = this;
        int id = m_id;

        switch (message) {
        case WM_CLOSE: {
            m_state = WindowDestroying;
            if (!m_isDestroyApiBeCalled) {
                bool isPreventDefault = mate::EventEmitter<Window>::emit("close");
                if (isPreventDefault)
                    return 0;
            }
            ::ShowWindow(hWnd, SW_HIDE);
        }
        break;

        case WM_NCDESTROY:
            mate::EventEmitter<Window>::emit("closed");
            ::KillTimer(hWnd, (UINT_PTR)this);
            ::RemovePropW(hWnd, kPropW);
            ::RevokeDragDrop(m_hWnd);

            ThreadCall::callBlinkThreadAsync([webview, self, hWnd] {
                while (::IsWindow(hWnd)) {
                    ::Sleep(100);
                }
                wkeDestroyWebView(webview);
                self->m_webContents = nullptr;
            });

            WindowList::getInstance()->removeWindow(self);
            if (WindowList::getInstance()->empty())
                App::getInstance()->onWindowAllClosed();
            
            m_state = WindowDestroyed;
            m_webContents = nullptr;

            {
                char* output = (char*)malloc(0x100);
                sprintf(output, "WM_NCDESTROY %p %d\n", this, m_id);
                OutputDebugStringA(output);
                free(output);
            }

            IdLiveDetect::get()->deconstructed(m_id);
            break;

        case WM_TIMER:
            //wkeRepaintIfNeeded(webview);
            return 0;

        case WM_COMMAND:
            MenuEventNotif::onMenuCommon(message, wParam, lParam);
            break;

        case WM_PAINT:
            onPaintMessage(hWnd);
            break;

        case WM_ERASEBKGND:
            return TRUE;

        case WM_GETMINMAXINFO: {
            MINMAXINFO* minmaxInfo = (MINMAXINFO*)lParam;
            minmaxInfo->ptMinTrackSize.x = m_createWindowParam->minWidth;
            minmaxInfo->ptMinTrackSize.y = m_createWindowParam->minHeight;

            minmaxInfo->ptMaxTrackSize.x = m_createWindowParam->maxWidth;
            minmaxInfo->ptMaxTrackSize.y = m_createWindowParam->maxHeight;
        }
        break;

        case WM_MOVE:
            mate::EventEmitter<Window>::emit("move");
            break;

        case WM_SIZE: {
            ThreadCall::callBlinkThreadAsync([webview, lParam] {
                wkeResize(webview, LOWORD(lParam), HIWORD(lParam));
                wkeRepaintIfNeeded(webview);
            });

            if (WindowInited == m_state)
                mate::EventEmitter<Window>::emit("resize");

            if (SIZE_MAXIMIZED == wParam) {
                m_isMaximized = true;
                mate::EventEmitter<Window>::emit("maximize");
            }
            if (SIZE_MINIMIZED == wParam)
                mate::EventEmitter<Window>::emit("minimize");
            if (SIZE_RESTORED == wParam) {
                if (m_isMaximized)
                    mate::EventEmitter<Window>::emit("unmaximize");
                m_isMaximized = false;
            }

            setRoundWindow();
        }
            return 0;
        case WM_KEYDOWN: {

            if (m_hIMC) {
                ::ImmAssociateContext(hWnd, m_hIMC);
                m_hIMC = nullptr;
            }
            
            unsigned int virtualKeyCode = wParam;
            unsigned int flags = 0;
            if (HIWORD(lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            ThreadCall::callBlinkThreadAsync([webview, virtualKeyCode, flags] {
                wkeFireKeyDownEvent(webview, virtualKeyCode, flags, false);

                if (113 == virtualKeyCode) {
                    wkeSetDebugConfig(webview, "showDevTools", "E:/mycode/miniblink49/trunk/third_party/WebKit/Source/devtools/front_end/inspector.html");
                }
            });

            return 0;
            break;
        }
        case WM_KEYUP: {
            unsigned int virtualKeyCode = wParam;
            unsigned int flags = 0;
            if (HIWORD(lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            ThreadCall::callBlinkThreadAsync([webview, virtualKeyCode, flags] {
                wkeFireKeyUpEvent(webview, virtualKeyCode, flags, false);
            });

            return 0;
            break;
        }
        case WM_CHAR: {
            unsigned int charCode = wParam;
            unsigned int flags = 0;
            if (HIWORD(lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            ThreadCall::callBlinkThreadAsync([webview, id, charCode, flags] {
                if (IdLiveDetect::get()->isLive(id))
                    wkeFireKeyPressEvent(webview, charCode, flags, false);
            });
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
            onCursorChange();
            onMouseMessage(hWnd, message, wParam, lParam);
            break;
        case WM_CONTEXTMENU: {
            POINT pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);

            if (pt.x != -1 && pt.y != -1)
                ::ScreenToClient(hWnd, &pt);

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

            ThreadCall::callBlinkThreadAsync([id, webview, pt, flags] {
                if (IdLiveDetect::get()->isLive(id))
                    wkeFireContextMenuEvent(webview, pt.x, pt.y, flags);
            });
            break;
        }
        case WM_MOUSEWHEEL: {
            if (m_isIgnoreMouseEvents)
                break;
            POINT pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);
            ::ScreenToClient(hWnd, &pt);

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

            ThreadCall::callBlinkThreadAsync([id, webview, pt, delta, flags] {
                if (IdLiveDetect::get()->isLive(id))
                    wkeFireMouseWheelEvent(webview, pt.x, pt.y, delta, flags);
            });
            break;
        }
        case WM_SETFOCUS:
            mate::EventEmitter<Window>::emit("focus");
            ThreadCall::callBlinkThreadAsync([id, webview] {
                if (IdLiveDetect::get()->isLive(id))
                    wkeSetFocus(webview);
            });
            return 0;

        case WM_KILLFOCUS:
            mate::EventEmitter<Window>::emit("blur");

            ThreadCall::callBlinkThreadAsync([id, webview] {
                if (IdLiveDetect::get()->isLive(id))
                    wkeKillFocus(webview);
            });
            return 0;

        case WM_SETCURSOR:
            if (setCursorInfoTypeByCache())
                return 0;
            break;
        case WM_IME_STARTCOMPOSITION: {
            ThreadCall::callBlinkThreadAsync([webview, hWnd] {
                wkeRect* caret = new wkeRect();
                *caret = wkeGetCaretRect(webview);
                ::PostMessage(hWnd, WM_IME_STARTCOMPOSITION_ASYN, (WPARAM)caret, 0);
            });
        }
            return 0;
        case WM_IME_STARTCOMPOSITION_ASYN: {
            wkeRect* caret = (wkeRect*)wParam;
            COMPOSITIONFORM compositionForm;
            compositionForm.dwStyle = CFS_POINT | CFS_FORCE_POSITION;
            compositionForm.ptCurrentPos.x = caret->x;
            compositionForm.ptCurrentPos.y = caret->y;

            HIMC hIMC = ::ImmGetContext(hWnd);

            if (0 == caret->h) {
                ::ImmAssociateContext(hWnd, nullptr);
                m_hIMC = hIMC;
            }

            ::ImmSetCompositionWindow(hIMC, &compositionForm);
            ::ImmReleaseContext(hWnd, hIMC);

            delete caret;
        }
        break;
        case WM_DROPFILES:
            //onDragFiles((HDROP)wParam);
            break;
        }

        return ::DefWindowProcW(hWnd, message, wParam, lParam);
    }

    static LRESULT CALLBACK staticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        int id = -1;
        Window* self = (Window*)::GetPropW(hWnd, kPropW);
        if (!self && message == WM_CREATE) {
            
            LPCREATESTRUCTW cs = (LPCREATESTRUCTW)lParam;
            self = (Window *)cs->lpCreateParams;
            id = self->m_id;
            ::SetPropW(hWnd, kPropW, (HANDLE)self);
            ::SetTimer(hWnd, (UINT_PTR)self, 70, NULL);
            return 0;
            
        }
        if (!self)
            return ::DefWindowProcW(hWnd, message, wParam, lParam);

        id = self->m_id;
        wkeWebView webview = self->m_webContents->getWkeView();
        if (!webview)
            return ::DefWindowProcW(hWnd, message, wParam, lParam);

        return self->windowProc(hWnd, message, wParam, lParam);
    }

private:
    void closeApi() {
        m_state = WindowDestroying;
        ::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
    }

    void destroyApi() {
        m_isDestroyApiBeCalled = true;
        closeApi();
    }

    void focusApi() {
        ::SetFocus(m_hWnd);
    }

    void blurApi() {
        ::SetFocus(NULL);
    }

    bool isFocusedApi() {
        return ::GetFocus() == m_hWnd;
    }

    void showApi() {
        ::ShowWindow(m_hWnd, TRUE);
        ::SetFocus(m_hWnd);
    }

    void showInactiveApi() {
        ::ShowWindow(m_hWnd, TRUE);
    }

    void hideApi() {
        ::ShowWindow(m_hWnd, FALSE);
    }

    bool isVisibleApi() {
        return !!::IsWindowVisible(m_hWnd);
    }

    bool isEnabledApi() {
        return !!::IsWindowEnabled(m_hWnd);
    }

    //maximize
    void maximizeApi() {
        ::ShowWindow(m_hWnd, SW_MAXIMIZE);
    }

    void unmaximizeApi() {
        ::ShowWindow(m_hWnd, SW_RESTORE);
    }

    bool isMaximizedApi() {
        return !!::IsZoomed(m_hWnd);
    }

    void minimizeApi() {
        ::ShowWindow(m_hWnd, SW_MINIMIZE);
    }

    //restore
    void restoreApi() {
        ::ShowWindow(m_hWnd, SW_RESTORE);
    }

    //isMinimized
    bool isMinimizedApi() {
        return !!IsIconic(m_hWnd);
    }

    //setFullScreen
    void setFullScreenApi(bool b) {
        if (b) {
            RECT rc;
            HWND hDesk = ::GetDesktopWindow();
            ::GetWindowRect(hDesk, &rc);
            ::SetWindowLong(m_hWnd, GWL_STYLE, GetWindowLong(m_hWnd, GWL_STYLE) | WS_BORDER);
            ::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, rc.right, rc.bottom, SWP_SHOWWINDOW);
        } else {
            ::SetWindowLong(m_hWnd, GWL_STYLE, GetWindowLong(m_hWnd, GWL_STYLE) ^ WS_BORDER);
        }
    }

    bool isFullScreenApi() {
        OutputDebugStringA("isFullScreenApi\n");
        return false;
    }

    void setParentWindowApi() {
        OutputDebugStringA("setParentWindowApi\n");
        DebugBreak();
    }

    void getParentWindowApi() {
        OutputDebugStringA("getParentWindowApi\n");
        DebugBreak();
    }

    void getChildWindowsApi() {
        OutputDebugStringA("getChildWindowsApi\n");
        DebugBreak();
    }

    bool isModalApi() {
        OutputDebugStringA("isModalApi\n");
        DebugBreak();
        return false;
    }

    UINT_PTR getNativeWindowHandleApi() {
        //args.GetReturnValue().Set(toBuffer(isolate, (void*)(&self->m_hWnd), sizeof(HWND)));
        return (UINT_PTR)m_hWnd;
    }

    v8::Local<v8::Object> getBoundsApi() {
        RECT clientRect;
        ::GetClientRect(m_hWnd, &clientRect);

        v8::Local<v8::Integer> x = v8::Integer::New(isolate(), clientRect.left);
        v8::Local<v8::Integer> y = v8::Integer::New(isolate(), clientRect.top);
        v8::Local<v8::Integer> width = v8::Integer::New(isolate(), clientRect.right - clientRect.left);
        v8::Local<v8::Integer> height = v8::Integer::New(isolate(), clientRect.bottom - clientRect.top);
        v8::Local<v8::Object> bounds = v8::Object::New(isolate());
        bounds->Set(v8::String::NewFromUtf8(isolate(), "x"), x);
        bounds->Set(v8::String::NewFromUtf8(isolate(), "y"), y);
        bounds->Set(v8::String::NewFromUtf8(isolate(), "width"), width);
        bounds->Set(v8::String::NewFromUtf8(isolate(), "height"), height);
        return bounds;
    }

    void setBoundsApi(v8::Local<v8::Object> bounds) {
        LONG x = (LONG)bounds->Get(v8::String::NewFromUtf8(isolate(), "x"))->NumberValue();
        LONG y = (LONG)bounds->Get(v8::String::NewFromUtf8(isolate(), "y"))->NumberValue();
        LONG width = (LONG)bounds->Get(v8::String::NewFromUtf8(isolate(), "width"))->NumberValue();
        LONG height = (LONG)bounds->Get(v8::String::NewFromUtf8(isolate(), "height"))->NumberValue();
        ::MoveWindow(m_hWnd, x, y, width, height, TRUE);
    }

    v8::Local<v8::Object> getSizeApi() {
        RECT clientRect;
        ::GetClientRect(m_hWnd, &clientRect);

        v8::Local<v8::Integer> width = v8::Integer::New(isolate(), clientRect.right - clientRect.left);
        v8::Local<v8::Integer> height = v8::Integer::New(isolate(), clientRect.bottom - clientRect.top);
        v8::Local<v8::Array> size = v8::Array::New(isolate(), 2);
        size->Set(0, width);
        size->Set(1, height);
        return size;
    }

    void setSizeApi(int32_t width, int32_t height) {
        ::SetWindowPos(m_hWnd, NULL, 0, 0, width, height, SWP_NOMOVE);
    }

    void getContentBoundsApi() {
        ::OutputDebugStringA("getContentBoundsApi\n");
        ::DebugBreak();
    }

    void setContentBoundsApi() {
        ::OutputDebugStringA("setContentBoundsApi\n");
        ::DebugBreak();
    }

    std::vector<int> getContentSizeApi() {
        Window* self = this;

        SIZE contentsSize;
        ::EnterCriticalSection(&m_memoryCanvasLock);
        contentsSize = m_contentsSize;
        ::LeaveCriticalSection(&m_memoryCanvasLock);

        int width = 300;
        int height = 450;
        std::vector<int> size = { contentsSize.cx, contentsSize.cy };
        return size;
    }

    void setContentSizeApi(int width, int height) {
        Window* self = this;
        int id = self->m_id;
        ThreadCall::callBlinkThreadAsync([self, id, width, height] {
            if (!IdLiveDetect::get()->isLive(id))
                return;
            wkeResize(self->m_webContents->getWkeView(), width, height);
            wkeRepaintIfNeeded(self->m_webContents->getWkeView());
        });
    }

    void setMinimumSizeApi(int width, int height) {
        m_createWindowParam->minWidth = width;
        m_createWindowParam->minHeight = height;
    }

    std::vector<int> getMinimumSizeApi() {
        std::vector<int> size = { m_createWindowParam->minWidth, m_createWindowParam->minHeight };
        return size;
    }

    void setMaximumSizeApi(int width, int height) {
        m_createWindowParam->maxWidth = width;
        m_createWindowParam->maxHeight = height;
    }

    std::vector<int> getMaximumSizeApi() {
        std::vector<int> size = { m_createWindowParam->maxWidth, m_createWindowParam->maxHeight };
        return size;
    }

    void setResizableApi(bool resizable) {
        m_createWindowParam->isResizable = resizable;

        DWORD style = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
        if (resizable)
            style |= WS_THICKFRAME;
        else
            style &= (~WS_THICKFRAME);
        ::SetWindowLong(m_hWnd, GWL_EXSTYLE, style);
        m_createWindowParam->styles = style;
    }

    bool isResizableApi() {
        return m_createWindowParam->isResizable;
    }

    void setMovableApi(bool isMovable) {
        m_createWindowParam->isMovable = isMovable;
    }

    bool isMovableApi() {
        return m_createWindowParam->isMovable;
    }

    void setMinimizableApi(bool isMinimizable) {
        DWORD style = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
        if (isMinimizable)
            style |= WS_MINIMIZEBOX;
        else
            style &= (~WS_MINIMIZEBOX);
        m_createWindowParam->styles = style;
        m_createWindowParam->isMaximizable;
        ::SetWindowLong(m_hWnd, GWL_EXSTYLE, style);
    }

    bool isMinimizableApi() {
        return m_createWindowParam->isMinimizable;
    }

    bool isMaximizableApi() {
        return m_createWindowParam->isMaximizable;
    }

    void setMaximizableApi(bool isMaximizable) {
        DWORD style = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
        if (isMaximizable)
            style |= WS_MAXIMIZEBOX;
        else
            style &= (~WS_MAXIMIZEBOX);
        m_createWindowParam->styles = style;
        m_createWindowParam->isMaximizable;
        ::SetWindowLong(m_hWnd, GWL_EXSTYLE, style);
    }

    void setFullScreenableApi(bool isFullScreenable) {
    }

    bool isFullScreenableApi() {
        return false;
    }

    void setClosableApi(bool isClosable) {
        m_createWindowParam->isClosable = isClosable;
    }

    bool isClosableApi() {
        return m_createWindowParam->isClosable;
    }

    void setAlwaysOnTopApi(bool b) {
        ::SetWindowPos(m_hWnd, b ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }

    bool isAlwaysOnTopApi() {
        return 0 == (::GetWindowLong(m_hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST);
    }

    void centerApi() {
        int screenX, screenY;
        screenX = ::GetSystemMetrics(SM_CXSCREEN);  //取得屏幕的宽度
        screenY = ::GetSystemMetrics(SM_CYSCREEN);  //取得屏幕的高度

        RECT rect;
        ::GetWindowRect(m_hWnd, &rect);
        rect.left = (screenX - rect.right) / 2;
        rect.top = (screenY - rect.bottom) / 2;

        //设置窗体位置
        ::SetWindowPos(m_hWnd, NULL, rect.left, rect.top, rect.right, rect.bottom, SWP_NOSIZE);
    }

    void setPositionApi(int x, int y) {
        ::SetWindowPos(m_hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);
    }

    std::vector<int> getPositionApi() {
        RECT rect = {0};
        ::GetWindowRect(m_hWnd, &rect);
        std::vector<int> pos = { rect.left, rect.top };
        return pos;
    }

    void setTitleApi(const std::string& title) {
        std::wstring titleW;
        titleW = StringUtil::UTF8ToUTF16(title);
        ::SetWindowText(m_hWnd, titleW.c_str());
    }

    std::string getTitleApi() {
        std::vector<wchar_t> titleW;
        titleW.resize(MAX_PATH + 1);
        ::GetWindowText(m_hWnd, &titleW[0], MAX_PATH);
        std::string titleA;
        titleA = StringUtil::UTF16ToUTF8(std::wstring(&titleW[0], titleW.size()));
        return titleA;
    }

    void flashFrameApi() {
    }

    void setSkipTaskbarApi(bool b) {
        DWORD style = ::GetWindowLong(m_hWnd, GWL_STYLE);
        if (b) {
            style |= WS_EX_TOOLWINDOW;
            style &= ~WS_EX_APPWINDOW;
        } else {  //todo 如果窗口原来的style没有WS_EX_APPWINDOW，就可能有问题
            style &= ~WS_EX_TOOLWINDOW;
            style |= WS_EX_APPWINDOW;
        }
        ::SetWindowLong(m_hWnd, GWL_EXSTYLE, style);
    }

    void setBackgroundColorApi() {
    }

    void setDocumentEditedApi(bool b) {
        Window* self = this;
        ThreadCall::callBlinkThreadSync([self, b] {
            wkeSetEditable(self->m_webContents->getWkeView(), b);
        });
        m_isDocumentEdited = true;
    }

    bool isDocumentEditedApi() {
        return m_isDocumentEdited;
    }

    void setIgnoreMouseEventsApi(const v8::FunctionCallbackInfo<v8::Value>& info) {
        if (0 == info.Length())
            return;
        if (info[0]->IsBoolean())
            m_isIgnoreMouseEvents = info[0]->ToBoolean()->Value();
    }

    void setContentProtectionApi() {
    }

    void setFocusableApi() {
    }

    void focusOnWebViewApi() {
        Window* self = this;
        ThreadCall::callBlinkThreadSync([self] {
            wkeSetFocus(self->m_webContents->getWkeView());
        });
    }

    void isWebViewFocusedApi() {
    }

    void setOverlayIconApi() {
    }

    void setThumbarButtonsApi() {
    }

    void setMenuApi() {
    }

    void setAutoHideMenuBarApi() {
    }

    void isMenuBarAutoHideApi() {
    }

    void setMenuBarVisibilityApi() {
    }

    void isMenuBarVisibleApi() {
    }

    void setVisibleOnAllWorkspacesApi() {
    }

    void isVisibleOnAllWorkspacesApi() {
    }

    void hookWindowMessageApi() {
    }

    void isWindowMessageHookedApi() {
    }

    void unhookWindowMessageApi() {
    }

    void unhookAllWindowMessagesApi() {
    }

    void setThumbnailClipApi() {
    }

    void setThumbnailToolTipApi() {
    }

    void setAppDetailsApi() {
    }

    void setIconApi() {
    }

    void setProgressBarApi(double progress) {

    }

    bool isDestroyedApi() const {
        return false;
    }

    static void getFocusedWindowApi(const v8::FunctionCallbackInfo<v8::Value>& info) {
        v8::Local<v8::Value> result = WindowInterface::getFocusedWindow(info.GetIsolate());
        info.GetReturnValue().Set(result);
    }

    static void fromIdApi(const v8::FunctionCallbackInfo<v8::Value>& info) {
        OutputDebugStringA("fromIdApi\n");
        if (1 != info.Length())
            return;
        v8::Local<v8::Value> arg0 = info[0];
        if (!arg0->IsInt32())
            return;

        int32_t id = arg0->Int32Value();

        v8::Isolate* isolate = v8::Isolate::GetCurrent();
        Window* self = (Window*)WindowList::getInstance()->find(id);
        if (!self) {
            info.GetReturnValue().Set(v8::Null(isolate));
            return;
        }

        v8::Local<v8::Value> result = v8::Local<v8::Value>::New(isolate, self->GetWrapper(isolate));
        info.GetReturnValue().Set(result);
    }

    static void getAllWindowsApi(const v8::FunctionCallbackInfo<v8::Value>& info) {
        size_t size = WindowList::getInstance()->size();
        v8::Local<v8::Array> result = v8::Array::New(info.GetIsolate(), size);
        
        for (size_t i = 0; i < size; ++i) {
            Window* self = (Window*)WindowList::getInstance()->get(i);
            result->Set(i, self->GetWrapper(info.GetIsolate()));
        }

        info.GetReturnValue().Set(result);
    }
    
    static void fromWebContentsApi(const v8::FunctionCallbackInfo<v8::Value>& info) {
        if (1 != info.Length())
            return;
        v8::Local<v8::Value> arg0 = info[0];
        if (!arg0->IsObject())
            return;

        v8::Local<v8::Object> webContents = arg0->ToObject();

        WrappableBase* webContentsPtr = GetNativePtr(webContents, &WebContents::kWrapperInfo);
        if (!webContentsPtr)
            return;

        WebContents* contents = (WebContents*)webContentsPtr;
        WindowInterface* win = contents->getOwner();
        if (!win)
            return;
        Window* self = (Window*)win;
        v8::Local<v8::Object> winObject = self->GetWrapperImpl(info.GetIsolate(), &Window::kWrapperInfo);
        info.GetReturnValue().Set(winObject);
    }

    int getIdApi() const {
        return m_id;
    }

    v8::Local<v8::Value> _getWebContentsApi() {
        if (!m_webContents)
            return v8::Null(isolate());
        
        return v8::Local<v8::Value>::New(isolate(), m_webContents->getWrapper());
    }

    // 空实现
    void nullFunction() {
        OutputDebugStringA("nullFunction\n");
        DebugBreak();
    }

    static void readJsFile(const wchar_t* path, std::vector<char>* buffer) {
        HANDLE hFile = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (INVALID_HANDLE_VALUE == hFile) {
            DebugBreak();
            return;
        }

        DWORD fileSizeHigh;
        const DWORD bufferSize = ::GetFileSize(hFile, &fileSizeHigh);

        DWORD numberOfBytesRead = 0;
        buffer->resize(bufferSize);
        BOOL b = ::ReadFile(hFile, &buffer->at(0), bufferSize, &numberOfBytesRead, nullptr);
        ::CloseHandle(hFile);
        b = b;
    }

    static bool hookUrl(void* job, const char* url, const char* hookUrl, const wchar_t* localFile, const char* mime) {
        if (0 != strstr(url, hookUrl)) {
            wkeNetSetMIMEType(job, (char*)mime);

            std::vector<char> buffer;
            readJsFile(localFile, &buffer);
            wkeNetSetData(job, &buffer[0], buffer.size());

            OutputDebugStringA("hookUrl:");
            OutputDebugStringA(url);
            OutputDebugStringA("\n");

            return true;
        }

        return false;
    }
    
    static bool handleLoadUrlBegin(wkeWebView webView, void* param, const char* url, void* job) {
//         if (hookUrl(job, url, "frameworks-4a55ab3fcf005abef1e8b859483f3cce.js", L"D:\\ProgramData\\Lepton\\resources\\frameworks-4a55ab3fcf005abef1e8b859483f3cce.js", "text/javascript"))
//             return true;

        //OutputDebugStringA("apiwindow.load:");
        //OutputDebugStringA(url);
        //OutputDebugStringA("\n");

        if (ProtocolInterface::inst()->handleLoadUrlBegin(param, url, job))
            return true;

        if (0 == strstr(url, "file:///") || 0 == strstr(url, ".asar"))
            return false;

        int urlLength = strlen(url);
        int urlHostLength = urlLength;
        for (int i = 0; i < urlLength; ++i) {
            if ('?' != url[i])
                continue;
            urlHostLength = i;
            break;
        }

        size_t fileHeadLength = sizeof("file:///") - 1;
        base::StringPiece urlString(url, urlHostLength);
        urlString = urlString.substr(fileHeadLength, urlString.size() - fileHeadLength);
        base::FilePath path = base::FilePath::FromUTF8Unsafe(urlString);
        std::string contents;
        if (!asar::ReadFileToString(path, &contents) || 0 == contents.size())
            return false;
        wkeNetSetData(job, &contents.at(0), contents.size());

        return true;
    }

    static void onConsoleCallback(wkeWebView webView, void* param, wkeConsoleLevel level, const wkeString message, const wkeString sourceName, unsigned sourceLine, const wkeString stackTrace) {
        const utf8* msg = wkeToString(stackTrace);
        //std::string* outString = new std::string();
    }

    void moveToCenter() {
        int width = 0;
        int height = 0;

        RECT rect = { 0 };
        ::GetWindowRect(m_hWnd, &rect);
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;

        int parentWidth = 0;
        int parentHeight = 0;

        parentWidth = ::GetSystemMetrics(SM_CXSCREEN);
        parentHeight = ::GetSystemMetrics(SM_CYSCREEN);

        int x = (parentWidth - width) / 2;
        int y = (parentHeight - height) / 2;

        ::MoveWindow(m_hWnd, x, y, width, height, TRUE);
    }

    static const int kNotSetXYFlag = -8467;

    void setRoundWindow() {
        if (m_createWindowParam->isFrame)
            return;

        RECT windowRect;
        ::GetWindowRect(m_hWnd, &windowRect);
        int nWidthEllipse = 7;
        int nHeightEllipse = 7;
        HRGN hRgn = ::CreateRoundRectRgn(0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nWidthEllipse, nHeightEllipse);
        ::SetWindowRgn(m_hWnd, hRgn, TRUE);
    }

    static void onDocumentReadyInBlinkThread(wkeWebView webWindow, void* param) {
        int width = wkeGetContentWidth(webWindow);
        int height = wkeGetContentHeight(webWindow);
        Window* self = (Window*)param;
        int id = self->m_id;

        bool needSetPos = false;
        if (self->m_createWindowParam->isUseContentSize && 0 != width && 0 != height) {
            needSetPos = true;
        }

        ThreadCall::callUiThreadAsync([self, id, needSetPos, width, height] {
            if (!IdLiveDetect::get()->isLive(id))
                return;

            self->m_dragAction = new DragAction(self->m_webContents->getWkeView(), self->m_hWnd, id);
            HRESULT hr = ::RegisterDragDrop(self->m_hWnd, self->m_dragAction);
            //::DragAcceptFiles(self->m_hWnd, true);

            RECT rect = { 0 };
            ::GetWindowRect(self->m_hWnd, &rect);
            if (rect.left == kNotSetXYFlag || rect.top == kNotSetXYFlag)
                self->moveToCenter();

            if (needSetPos)
                ::SetWindowPos(self->m_hWnd, HWND_NOTOPMOST, 0, 0, width, height, SWP_NOMOVE | SWP_NOREPOSITION);

            self->mate::EventEmitter<Window>::emit("ready-to-show");

            if (self->m_webContents) {
                self->m_webContents->mate::EventEmitter<WebContents>::emit("dom-ready");
            }
        });
    }

    static void onTitleChangedInBlinkThread(wkeWebView webWindow, void* param, const wkeString title) {
        std::wstring* titleW = new std::wstring(wkeGetStringW(title));
        std::string* titleA = new std::string(wkeGetString(title));

        Window* self = (Window*)param;
        int id = self->m_id;
        ThreadCall::callUiThreadAsync([id, self, titleW, titleA] {
            if (!IdLiveDetect::get()->isLive(id))
                return;

            self->getWebContents()->onTitleChange(*titleA);
            ::SetWindowText(self->m_hWnd, titleW->c_str());
            self->mate::EventEmitter<Window>::emit("page-title-updated");
            delete titleW;
            delete titleA;
        });
    }

    static void onURLChangedCallback(wkeWebView webView, void* param, wkeWebFrameHandle frameId, const wkeString url) {
        Window* self = (Window*)param;
        std::string* urlStr = new std::string(wkeGetString(url));
        if (!wkeIsMainFrame(webView, frameId))
            return;
        int id = self->m_id;
        ThreadCall::callUiThreadAsync([id, self, urlStr] {
            if (!IdLiveDetect::get()->isLive(id))
                return;

            self->getWebContents()->onUrlChange(*urlStr);
            delete urlStr;
        });
    }

    static bool onNavigationCallback(wkeWebView webView, void* param, wkeNavigationType navigationType, const wkeString url) {
        Window* self = (Window*)param; 
        std::string* urlString = new std::string(wkeGetString(url));
        int id = self->m_id;
        bool allow = true;
        ThreadCall::callUiThreadSync([id, self, urlString, &allow] {
            if (!IdLiveDetect::get()->isLive(id))
                return;

            if (!self->m_webContents->m_isLoading)
                allow = !(self->m_webContents->mate::EventEmitter<WebContents>::emit("will-navigate", *urlString));
            self->m_webContents->m_isLoading = false;
            delete urlString;
        });
        return allow;
    }

    static void onOtherLoadCallback(wkeWebView webView, void* param, wkeOtherLoadType type, wkeTempCallbackInfo* info) {
        Window* self = (Window*)param;
        int id = self->m_id;
        WindowState state = self->m_state;
        bool isDestroyApiBeCalled = self->m_isDestroyApiBeCalled;

        std::string* url = nullptr;
        std::string* newUrl = nullptr;
        std::string* method = nullptr;
        std::string* referrer = nullptr;
        wkeResourceType resourceType = WKE_RESOURCE_TYPE_MAIN_FRAME;
        int httpResponseCode = 0;

        wkeWillSendRequestInfo* willSendRequestInfo = info->willSendRequestInfo;
        if (WKE_DID_GET_REDIRECT_REQUEST == type && willSendRequestInfo) {
            //willSendRequestInfo->isHolded = true;
            url = new std::string(wkeGetString(willSendRequestInfo->url));
            newUrl = new std::string(wkeGetString(willSendRequestInfo->newUrl));
            method = new std::string(wkeGetString(willSendRequestInfo->method));
            referrer = new std::string(wkeGetString(willSendRequestInfo->referrer));

            resourceType = willSendRequestInfo->resourceType;
            httpResponseCode = willSendRequestInfo->resourceType;
        }

        if (WKE_DID_NAVIGATE == type)
            wkeRunJS(self->m_webContents->getWkeView(), ";"); // 为了<webview>标签，强制触发js创建回调

        ThreadCall::callUiThreadAsync([id, self, state, isDestroyApiBeCalled, type, resourceType, httpResponseCode, url, newUrl, method, referrer] {
            if (!(!IdLiveDetect::get()->isLive(id) || WindowDestroying == state || WindowDestroyed == state || isDestroyApiBeCalled)) {

            } else if (WKE_DID_START_LOADING == type)
                self->m_webContents->mate::EventEmitter<WebContents>::emit("did-start-loading");
            else if (WKE_DID_STOP_LOADING == type)
                self->m_webContents->mate::EventEmitter<WebContents>::emit("did-stop-loading");
            else if (WKE_DID_GET_RESPONSE_DETAILS == type) {

            } else if (WKE_DID_GET_REDIRECT_REQUEST == type) {
                self->m_webContents->mate::EventEmitter<WebContents>::emit("did-get-redirect-request",
                    *url, *newUrl, resourceType == WKE_RESOURCE_TYPE_MAIN_FRAME,
                    httpResponseCode, *method, *referrer);
            }

            if (url) {
                delete url;
                delete newUrl;
                delete method;
                delete referrer;
            }
        });
    }

    static void onLoadingFinishCallback(wkeWebView webView, void* param, const wkeString url, wkeLoadingResult result, const wkeString failedReason) {
        Window* self = (Window*)param;
        int id = self->m_id;
        WindowState state = self->m_state;
        bool isDestroyApiBeCalled = self->m_isDestroyApiBeCalled;
        wkeTempCallbackInfo* tempInfo = wkeGetTempCallbackInfo(webView);
        bool isMainFrame = wkeIsMainFrame(webView, tempInfo->frame);
        std::string* failedReasonString = new std::string(wkeGetString(failedReason));
        std::string* urlString = new std::string(wkeGetString(url));

        ThreadCall::callUiThreadAsync([id, self, result, state, isDestroyApiBeCalled, failedReasonString, urlString, isMainFrame] {
            if (!IdLiveDetect::get()->isLive(id) ||
                WindowDestroying == state ||
                WindowDestroyed == state ||
                isDestroyApiBeCalled) {
                delete failedReasonString;
                delete urlString;
                return;
            }

            if (result == WKE_LOADING_SUCCEEDED) {
                self->m_webContents->mate::EventEmitter<WebContents>::emit("did-frame-finish-load", isMainFrame);
                if (isMainFrame)
                    self->m_webContents->mate::EventEmitter<WebContents>::emit("did-finish-load");
            } else {
                self->m_webContents->mate::EventEmitter<WebContents>::emit("did-fail-provisional-load", 0, *failedReasonString, *urlString, isMainFrame);

                if (result == WKE_LOADING_FAILED)
                    self->m_webContents->mate::EventEmitter<WebContents>::emit("did-fail-load", 0, *failedReasonString, *urlString, isMainFrame);
            }
            delete failedReasonString;
            delete urlString;
        });
    }

    static void onDraggableRegionsChanged(wkeWebView webWindow, void* param, const wkeDraggableRegion* regions, int rectCount) {
        Window* self = (Window*)param;
        int id = self->m_id;
        wkeDraggableRegion* newRegions = nullptr;
        if (regions) {
            newRegions = new wkeDraggableRegion[rectCount];
            memcpy(newRegions, regions, rectCount * sizeof(wkeDraggableRegion));
        }

        ThreadCall::callUiThreadAsync([id, self, newRegions, rectCount] {
            if (!IdLiveDetect::get()->isLive(id))
                return;

            ::SetRectRgn(self->m_draggableRegion, 0, 0, 0, 0);

            if (newRegions) {
                for (int i = 0; i < rectCount; ++i) {
                    RECT r = newRegions[i].bounds;
                    HRGN region = ::CreateRectRgn(r.left, r.top, r.right, r.bottom);
                    ::CombineRgn(self->m_draggableRegion, self->m_draggableRegion, region,
                        newRegions[i].draggable ? RGN_OR : RGN_DIFF);
                    ::DeleteObject(region);
                }
            }
        });
    }

    static void onStartDraggingCallback(
        wkeWebView webView,
        void* param,
        wkeWebFrameHandle frame,
        const wkeWebDragData* data,
        wkeWebDragOperationsMask mask,
        const void* image,
        const wkePoint* dragImageOffset
        ) {
        Window* self = (Window*)param;
        
        ThreadCall::callUiThreadAsync([self, webView, param, frame, data, mask, image, dragImageOffset] {
            self->m_dragAction->onStartDragging(webView, param, frame, data, mask, image, dragImageOffset);
            ThreadCall::exitReEnterMessageLoop(ThreadCall::getBlinkThreadId());
        });

        ThreadCall::messageLoop(ThreadCall::getBlinkLoop(), ThreadCall::getBlinkThreadV8Platform(), v8::Isolate::GetCurrent());
    }
    
    static Window* newWindow(gin::Dictionary* options, v8::Local<v8::Object> wrapper) {
        Window* self = new Window(options->isolate(), wrapper);
        WebContents::CreateWindowParam* createWindowParam = new WebContents::CreateWindowParam();
        createWindowParam->styles = 0;
        createWindowParam->styleEx = 0;
        createWindowParam->transparent = false;

        WebContents* webContents = nullptr;
        v8::Handle<v8::Object> webContentsV8;
        // If no WebContents was passed to the constructor, create it from options.
        if (!options->Get("webContents", &webContentsV8)) {
            // Use options.webPreferences to create WebContents.
            gin::Dictionary webPreferences = gin::Dictionary::CreateEmpty(options->isolate());
            options->Get(options::kWebPreferences, &webPreferences);

            // Copy the backgroundColor to webContents.
            v8::Local<v8::Value> value;
            if (options->Get(options::kBackgroundColor, &value))
                webPreferences.Set(options::kBackgroundColor, value);

            v8::Local<v8::Value> transparent;
            if (options->Get("transparent", &transparent))
                webPreferences.Set("transparent", transparent);

            // Offscreen windows are always created frameless.
            bool offscreen;
            if (webPreferences.Get("offscreen", &offscreen) && offscreen)
                options->Set(options::kFrame, false);
            
            webContents = WebContents::create(options->isolate(), webPreferences, self);

            webPreferences.GetBydefaultVal("nodeIntegration", true, &webContents->m_isNodeIntegration);
        } else
            DebugBreak();
        self->m_webContents = webContents;
        
        options->GetBydefaultVal("minWidth", 1, &createWindowParam->minWidth);
        options->GetBydefaultVal("minHeight", 1, &createWindowParam->minHeight);
        options->GetBydefaultVal("maxWidth", ::GetSystemMetrics(SM_CXSCREEN), &createWindowParam->maxWidth);
        options->GetBydefaultVal("maxHeight", ::GetSystemMetrics(SM_CYSCREEN), &createWindowParam->maxHeight);
        options->GetBydefaultVal("transparent", false, &createWindowParam->transparent);
        options->GetBydefaultVal("center", false, &createWindowParam->isCenter);
        options->GetBydefaultVal("resizable", true, &createWindowParam->isResizable);
        options->GetBydefaultVal("show", true, &createWindowParam->isShow);
        options->GetBydefaultVal("minimizable", true, &createWindowParam->isMinimizable);
        options->GetBydefaultVal("maximizable", true, &createWindowParam->isMaximizable);
        options->GetBydefaultVal("frame", true, &createWindowParam->isFrame);
        
        options->GetBydefaultVal("useContentSize", false, &createWindowParam->isUseContentSize);
        options->GetBydefaultVal("alwaysOnTop", false, &createWindowParam->isAlwaysOnTop);
        options->GetBydefaultVal("closable ", true, &createWindowParam->isClosable);
        
        options->GetBydefaultVal("x", kNotSetXYFlag, &createWindowParam->x);
        options->GetBydefaultVal("y", kNotSetXYFlag, &createWindowParam->y);
        options->GetBydefaultVal("width", 1, &createWindowParam->width);
        options->GetBydefaultVal("height", 1, &createWindowParam->height);

        if (createWindowParam->width < createWindowParam->minWidth)
            createWindowParam->width = createWindowParam->minWidth;

        if (createWindowParam->height < createWindowParam->minHeight)
            createWindowParam->height = createWindowParam->minHeight;

        std::string title;
        options->GetBydefaultVal("title", "Electron", &title);
        createWindowParam->title = StringUtil::UTF8ToUTF16(title);

        if (createWindowParam->transparent) {
            createWindowParam->styles = WS_POPUP;
            createWindowParam->styleEx = WS_EX_LAYERED;
        } else {
            createWindowParam->styles = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
            createWindowParam->styleEx = 0;
        }

        if (createWindowParam->isMinimizable)
            createWindowParam->styles |= WS_MINIMIZEBOX;
        if (createWindowParam->isMaximizable)
            createWindowParam->styles |= WS_MAXIMIZEBOX;

        if (createWindowParam->isResizable)
            createWindowParam->styles |= WS_THICKFRAME;
        createWindowParam->styleEx |= WS_EX_ACCEPTFILES;

        if (!createWindowParam->isFrame)
            createWindowParam->styles = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP;

        self->newWindowTaskInUiThread(createWindowParam);

        return self;
    }

    static void matchDpi(wkeWebView webview) {
        if (base::win::OSInfo::GetInstance()->version() < base::win::VERSION_WIN8)
            return;

        HDC hdc = ::GetDC(nullptr);
        int t = ::GetDeviceCaps(hdc, DESKTOPHORZRES);
        int d = ::GetDeviceCaps(hdc, HORZRES);
        int logPixelsx = ::GetDeviceCaps(hdc, LOGPIXELSX);
        float s_kScaleX = (float)t / (float)d;
        s_kScaleX = logPixelsx / 96.0f;

        wkeSetZoomFactor(webview, s_kScaleX);
    }

    void newWindowTaskInUiThread(WebContents::CreateWindowParam* createWindowParam) {
        m_createWindowParam = createWindowParam;

        m_hWnd = ::CreateWindowEx(createWindowParam->styleEx,
            kElectronClassName, createWindowParam->title.c_str(),
            createWindowParam->styles, createWindowParam->x,
            createWindowParam->y, createWindowParam->width,
            createWindowParam->height, NULL, NULL, ::GetModuleHandleW(NULL), this);

        if (!::IsWindow(m_hWnd))
            return;

        HWND dwFlag = HWND_NOTOPMOST;
        if (createWindowParam->isAlwaysOnTop)
            dwFlag = HWND_TOPMOST;
        ::SetWindowPos(m_hWnd, dwFlag, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOREPOSITION);

        setRoundWindow();

        if (!createWindowParam->isClosable)
            ::EnableMenuItem(::GetSystemMenu(m_hWnd, false), SC_CLOSE, MF_BYCOMMAND | MF_GRAYED);

        ::GetClientRect(m_hWnd, &m_clientRect);

        if (createWindowParam->isCenter)
            moveToCenter();

        int width = m_clientRect.right - m_clientRect.left;
        int height = m_clientRect.bottom - m_clientRect.top;

        Window* self = this;
        int id = self->m_id;
        HWND hWnd = m_hWnd;
        ThreadCall::callBlinkThreadAsync([id, self, createWindowParam, hWnd, width, height] {
            if (!IdLiveDetect::get()->isLive(id))
                return;

            wkeWebView webview = self->m_webContents->getWkeView();
            self->m_webContents->onNewWindowInBlinkThread(width, height, createWindowParam);

            matchDpi(webview);

            //wkeSetDebugConfig(webview, "drawDirtyDebugLine", nullptr);
            wkeSetHandle(webview, hWnd);
            wkeOnPaintUpdated(webview, (wkePaintUpdatedCallback)staticOnPaintUpdatedInCompositeThread, self);
            wkeOnConsole(webview, onConsoleCallback, nullptr);
            wkeOnDocumentReady(webview, onDocumentReadyInBlinkThread, self);
            wkeOnLoadUrlBegin(webview, handleLoadUrlBegin, self);
            wkeOnTitleChanged(webview, onTitleChangedInBlinkThread, self);
            wkeOnURLChanged2(webview, onURLChangedCallback, self);
            wkeOnLoadingFinish(webview, onLoadingFinishCallback, self);
            wkeOnOtherLoad(webview, onOtherLoadCallback, self);
            wkeOnNavigation(webview, onNavigationCallback, self);
            wkeOnDraggableRegionsChanged(webview, onDraggableRegionsChanged, self);
            wkeOnStartDragging(webview, onStartDraggingCallback, self);
            wkeSetFocus(webview);
            wkeSetDebugConfig(webview, "decodeUrlRequest", nullptr);
            wkeSetDragDropEnable(webview, true);
            wkeAddNpapiPlugin(webview, /*"application/browser-plugin",*/ &Webview_NP_Initialize, &Webview_NP_GetEntryPoints, &Webview_NP_Shutdown);
            wkeSetDebugConfig(webview, "wakeMinInterval", "1");
            wkeSetDebugConfig(webview, "drawMinInterval", "20");
            //wkeSetDebugConfig(webview, "contentScale", "50");
            wkeSetNavigationToNewWindowEnable(webview, true);
            wkeOnCreateView(webview, PopupWindow::onCreateViewCallbackStatic, nullptr);

            if (createWindowParam->transparent)
                wkeSetTransparent(webview, true);
        });

        MenuEventNotif::onWindowDidCreated(this);

        ::ShowWindow(m_hWnd, createWindowParam->isShow ? SW_SHOWNORMAL : SW_HIDE);

        m_state = WindowInited;
    }

    static void newFunction(const v8::FunctionCallbackInfo<v8::Value>& args) {
        v8::Isolate* isolate = args.GetIsolate();

        if (args.IsConstructCall()) {
            if (args.Length() > 1)
                return;

            gin::Dictionary options(isolate, args[0]->ToObject());
            Window* self = newWindow(&options, args.This());
            WindowList::getInstance()->addWindow(self);

            args.GetReturnValue().Set(args.This());
        }
    }

    static v8::Persistent<v8::Function> constructor;
    WebContents* getWebContents() { return m_webContents; }

public:
    static gin::WrapperInfo kWrapperInfo;
    static const WCHAR* kPropW;

private:
    friend class WindowInterface;

    enum WindowState {
        WindowUninited,
        WindowInited,
        WindowDestroying,
        WindowDestroyed
    };
    WindowState m_state;
    WebContents* m_webContents;

    bool m_isDestroyApiBeCalled;
    
    HWND m_hWnd;
    HIMC m_hIMC;

    int m_cursorInfoType;
    bool m_isCursorInfoTypeAsynGetting;
    CRITICAL_SECTION m_memoryCanvasLock;
    HBITMAP m_memoryBMP;
    HDC m_memoryDC;
    RECT m_clientRect;
    SIZE m_memoryBmpSize;

    HRGN m_draggableRegion;

    bool m_isMaximized;
    bool m_isDocumentEdited;
    bool m_isIgnoreMouseEvents;

    SIZE m_contentsSize;

    WebContents::CreateWindowParam* m_createWindowParam;
    DragAction* m_dragAction;

    CRITICAL_SECTION m_mouseMsgQueueLock;
    struct MouseMsg {
        MouseMsg(const MouseMsg& other) {
            init(message, x, y, flags);
        }

        MouseMsg(unsigned int message, int x, int y, unsigned int flags) {
            init(message, x, y, flags);
        }

        void init(unsigned int message, int x, int y, unsigned int flags) {
            this->message = message;
            this->x = x;
            this->y = y;
            this->flags = flags;
        }
        unsigned int message;
        int x;
        int y;
        unsigned int flags;
    };
    std::list<MouseMsg*> m_mouseMsgQueue;

    int m_id;
};

v8::Local<v8::Value> WindowInterface::getFocusedWindow(v8::Isolate* isolate) {
    v8::Local<v8::Value> result;
    HWND focusWnd = ::GetFocus();
    Window* self = (Window*)::GetPropW(focusWnd, Window::kPropW);
    if (!self)
        result = v8::Null(isolate);
    else
        result = self->GetWrapper(isolate);
    return result;
}

v8::Local<v8::Value> WindowInterface::getFocusedContents(v8::Isolate* isolate) {
    v8::Local<v8::Value> result;
    HWND focusWnd = ::GetFocus();
    Window* self = (Window*)::GetPropW(focusWnd, Window::kPropW);
    if (!self)
       return v8::Null(isolate);
    
    WebContents* content = self->getWebContents();
    if (!content)
        return v8::Null(isolate);
    result = content->GetWrapper(isolate);
    return result;
}

const WCHAR* Window::kPropW = L"ElectronWindow";
v8::Persistent<v8::Function> Window::constructor;
gin::WrapperInfo Window::kWrapperInfo = { gin::kEmbedderNativeGin };

static void initializeWindowApi(v8::Local<v8::Object> target, v8::Local<v8::Value> unused, v8::Local<v8::Context> context, const NodeNative* native) {
    node::Environment* env = node::Environment::GetCurrent(context);
    Window::init(target, env);
    WNDCLASSEXW wndClass = { 0 };

    static bool isInitClass = false;

    if (!isInitClass) {
        isInitClass = true;

        wndClass.style = CS_HREDRAW | CS_VREDRAW;

#if 0
        if (base::win::OSInfo::GetInstance()->version() < base::win::VERSION_WIN8)
            wndClass.style |= CS_DROPSHADOW;
#endif
        HMODULE hMod = ::GetModuleHandleW(NULL);
        wndClass.cbSize = sizeof(WNDCLASSEX);
        wndClass.lpfnWndProc = &Window::staticWindowProc;
        wndClass.cbClsExtra = 0;
        wndClass.cbWndExtra = 0;
        wndClass.hInstance = hMod;
        wndClass.hIcon = LoadIcon(hMod, MAKEINTRESOURCE(IDC_SMALL));
        wndClass.hCursor = LoadCursor(hMod, IDC_ARROW);
        wndClass.hbrBackground = NULL;
        wndClass.lpszMenuName = NULL;
        wndClass.lpszClassName = WindowInterface::kElectronClassName;
        wndClass.hIconSm = LoadIcon(hMod, MAKEINTRESOURCE(IDC_SMALL));
        ::RegisterClassExW(&wndClass);
    }
}

#pragma warning(push)
#pragma warning(disable:4309)
#pragma warning(disable:4838)
static const char BrowserWindowNative[] =
"//const {EventEmitter} = require('events');"
"//const {BrowserWindow} = process.binding('atom_browser_window');"
"//Object.setPrototypeOf(BrowserWindow.prototype, EventEmitter.prototype);"
"//module.exports = BrowserWindow;";
#pragma warning(pop)

static NodeNative nativeBrowserWindowNative{ "BrowserWindow", BrowserWindowNative, sizeof(BrowserWindowNative) - 1 };

NODE_MODULE_CONTEXT_AWARE_BUILTIN_SCRIPT_MANUAL(atom_browser_window, initializeWindowApi, &nativeBrowserWindowNative)

} // atom﻿
