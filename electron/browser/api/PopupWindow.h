
#ifndef browser_api_PopupWindow_h
#define browser_api_PopupWindow_h

#include "common/IdLiveDetect.h"
#include "common/ThreadCall.h"
#include "common/WinUserMsg.h"
#include <shellapi.h>
#include <ole2.h>

namespace atom {
    
#define kPopupClassName L"MbPopupClassName"

class PopupWindow {
public:
    PopupWindow() {
        m_state = WindowUninited;
        m_hWnd = nullptr;
        m_cursorInfoType = 0;
        m_isCursorInfoTypeAsynGetting = false;
        m_isCursorInfoTypeAsynChanged = false;
        m_memoryBMP = nullptr;
        m_memoryDC = nullptr;
        m_isLayerWindow = false;
        m_wkeWebview = nullptr;
        m_clientRect.left = 0;
        m_clientRect.top = 0;
        m_clientRect.right = 0;
        m_clientRect.bottom = 0;
        ::InitializeCriticalSection(&m_memoryCanvasLock);
        m_id = IdLiveDetect::get()->constructed(this);
    }

    ~PopupWindow() {
        if (m_memoryBMP)
            ::DeleteObject(m_memoryBMP);
        if (m_memoryDC)
            ::DeleteDC(m_memoryDC);

        ::SendMessage(m_hWnd, WM_CLOSE, 0, 0);

        IdLiveDetect::get()->deconstructed(m_id);

        ::DeleteCriticalSection(&m_memoryCanvasLock);
    }

    static wkeWebView onCreateViewCallbackStatic(wkeWebView webView, void* param, wkeNavigationType navigationType, const wkeString url, const wkeWindowFeatures* windowFeatures) {
        return onCreateViewInBlinkThread(webView, navigationType, url, windowFeatures);
    }

    void onPaintUpdatedInCompositeThread(const HDC hdc, int x, int y, int cx, int cy) {
        HWND hWnd = m_hWnd;
        RECT rectDest;
        ::GetClientRect(hWnd, &rectDest);
        SIZE sizeDest = { rectDest.right - rectDest.left, rectDest.bottom - rectDest.top };
        if (0 == sizeDest.cx * sizeDest.cy)
            return;

        if (!m_memoryDC)
            m_memoryDC = ::CreateCompatibleDC(nullptr);

        if (!m_memoryBMP || m_clientRect.top != rectDest.top || m_clientRect.bottom != rectDest.bottom ||
            m_clientRect.right != rectDest.right || m_clientRect.left != rectDest.left) {
            m_clientRect = rectDest;

            if (m_memoryBMP)
                ::DeleteObject((HGDIOBJ)m_memoryBMP);
            m_memoryBMP = ::CreateCompatibleBitmap(hdc, sizeDest.cx, sizeDest.cy);
        }

        HBITMAP hbmpOld = (HBITMAP)::SelectObject(m_memoryDC, m_memoryBMP);
        ::BitBlt(m_memoryDC, x, y, cx, cy, hdc, x, y, SRCCOPY);

        ::SelectObject(m_memoryDC, (HGDIOBJ)hbmpOld);
    }

    void onPaintUpdatedInUiThread(int x, int y, int cx, int cy) {
        ::EnterCriticalSection(&m_memoryCanvasLock);

        HDC hdcScreen = ::GetDC(m_hWnd);
        ::BitBlt(hdcScreen, x, y, cx, cy, m_memoryDC, x, y, SRCCOPY);
        ::ReleaseDC(m_hWnd, hdcScreen);

        ::LeaveCriticalSection(&m_memoryCanvasLock);
    }

    static void staticOnPaintUpdatedInCompositeThread(wkeWebView webView, PopupWindow* win, const HDC hdc, int x, int y, int cx, int cy) {
        ::EnterCriticalSection(&win->m_memoryCanvasLock);
        win->onPaintUpdatedInCompositeThread(hdc, x, y, cx, cy);
        ::LeaveCriticalSection(&win->m_memoryCanvasLock);

        if (win->m_isLayerWindow) {
            int id = win->m_id;
            ThreadCall::callUiThreadAsync([id, win, x, y, cx, cy] {
                if (IdLiveDetect::get()->isLive(id))
                    win->onPaintUpdatedInUiThread(x, y, cx, cy);
            });
        } else {
            RECT rc = { x, y, x + cx, y + cy };
            ::InvalidateRect(win->m_hWnd, &rc, false);
        }
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

        ::EnterCriticalSection(&m_memoryCanvasLock);
        if (0 != width && 0 != height && m_memoryBMP && m_memoryDC) {
            HBITMAP hbmpOld = (HBITMAP)::SelectObject(m_memoryDC, m_memoryBMP);
            BOOL b = ::BitBlt(hdc, destX, destY, width, height, m_memoryDC, srcX, srcY, SRCCOPY);
            ::SelectObject(m_memoryDC, hbmpOld);
            b = b;
        }
        ::LeaveCriticalSection(&m_memoryCanvasLock);

        ::EndPaint(hWnd, &ps);
    }

    void onMouseMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        int id = m_id;
        wkeWebView wkeWebview = m_wkeWebview;
        if (message == WM_LBUTTONDOWN || message == WM_MBUTTONDOWN || message == WM_RBUTTONDOWN) {
            ::SetFocus(hWnd);
            ::SetCapture(hWnd);
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

        ThreadCall::callBlinkThreadAsync([id, wkeWebview, message, x, y, flags] {
            if (IdLiveDetect::get()->isLive(id))
                wkeFireMouseEvent(wkeWebview, message, x, y, flags);
        });
    }

    void onCursorChange() {
        if (m_isCursorInfoTypeAsynGetting)
            return;
        m_isCursorInfoTypeAsynGetting = true;

        int id = m_id;
        wkeWebView wkeWebview = m_wkeWebview;
        PopupWindow* win = this;
        HWND hWnd = m_hWnd;
        ThreadCall::callBlinkThreadAsync([wkeWebview, win, hWnd, id] {
            if (!IdLiveDetect::get()->isLive(id))
                return;
            win->m_isCursorInfoTypeAsynGetting = false;
            win->m_isCursorInfoTypeAsynChanged = true;
            int cursorType = wkeGetCursorInfoType(wkeWebview);
            if (cursorType == win->m_cursorInfoType)
                return;
            win->m_cursorInfoType = cursorType;
            ::PostMessage(win->m_hWnd, WM_SETCURSOR, 0, 0);
        });
    }

    bool setCursorInfoTypeByCache() {
        if (!m_isCursorInfoTypeAsynChanged)
            return false;
        m_isCursorInfoTypeAsynChanged = false;

        HCURSOR hCur = NULL;
        switch (m_cursorInfoType) {
        case WkeCursorInfoIBeam:
            hCur = ::LoadCursor(NULL, IDC_IBEAM);
            break;
            //////////////////////////////////////////////////////////////////////////
        case WkeCursorInfoProgress:
            hCur = ::LoadCursor(NULL, IDC_APPSTARTING);
            break;
        case WkeCursorInfoCross:
            hCur = ::LoadCursor(NULL, IDC_CROSS);
            break;
        case WkeCursorInfoMove:
            hCur = ::LoadCursor(NULL, IDC_SIZEALL);
            break;

        case WkeCursorInfoColumnResize:
            hCur = ::LoadCursor(NULL, IDC_SIZEWE);
            break;
        case WkeCursorInfoRowResize:
            hCur = ::LoadCursor(NULL, IDC_SIZENS);
            break;
            //////////////////////////////////////////////////////////////////////////

        case WkeCursorInfoHand:
            hCur = ::LoadCursor(NULL, IDC_HAND);
            break;
        case WkeCursorInfoWait:
            hCur = ::LoadCursor(NULL, IDC_WAIT);
            break;
        case WkeCursorInfoHelp:
            hCur = ::LoadCursor(NULL, IDC_HELP);
            break;
        case WkeCursorInfoEastResize:
            hCur = ::LoadCursor(NULL, IDC_SIZEWE);
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
            hCur = ::LoadCursor(NULL, IDC_SIZEWE);
            break;
        case WkeCursorInfoNorthEastSouthWestResize:
        case WkeCursorInfoNorthWestSouthEastResize:
            hCur = ::LoadCursor(NULL, IDC_SIZEALL);
            break;
        case WkeCursorInfoNoDrop:
        case WkeCursorInfoNotAllowed:
            hCur = ::LoadCursor(NULL, IDC_NO);
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
        std::vector<std::vector<wchar_t>*>* fileNames = new std::vector<std::vector<wchar_t>*>();
        for (int i = 0; i < count; i++) {
            int pathlength = ::DragQueryFile(hDrop, i, NULL, 0) + 1;
            if (pathlength >= MAX_PATH || pathlength <= 1)
                continue;

            fileNames->push_back(new std::vector<wchar_t>());
            fileNames->at(i)->resize(pathlength);
            ::DragQueryFile(hDrop, i, fileNames->at(i)->data(), pathlength);
        }

        ::DragFinish(hDrop);

        POINT* curPos = new POINT();
        ::GetCursorPos(curPos);

        POINT* screenPos = new POINT();
        screenPos->x = curPos->x;
        screenPos->y = curPos->y;
        ::ScreenToClient(m_hWnd, screenPos);

        wkeWebView wkeWebview = m_wkeWebview;

        ThreadCall::callBlinkThreadAsync([wkeWebview, id, fileNames, curPos, screenPos] {
            if (!IdLiveDetect::get()->isLive(id))
                return;

            std::vector<wkeString> files;
            for (size_t i = 0; i < fileNames->size(); ++i) {
                files.push_back(wkeCreateStringW(fileNames->at(i)->data(), fileNames->at(i)->size()));
            }
            wkeSetDragFiles(wkeWebview, curPos, screenPos, files.data(), files.size());

            delete curPos;
            delete screenPos;
            for (size_t i = 0; i < fileNames->size(); ++i) {
                wkeDeleteString(files.at(i));
                delete fileNames->at(i);
            }
            delete fileNames;
        });
    }

    static LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
        int id = -1;
        PopupWindow* self = (PopupWindow *)::GetPropW(hWnd, kPrppW);
        wkeWebView wkeWebview = nullptr;
        if (!self) {
            if (message == WM_CREATE) {
                LPCREATESTRUCTW cs = (LPCREATESTRUCTW)lParam;
                self = (PopupWindow *)cs->lpCreateParams;
                id = self->m_id;
                ::SetPropW(hWnd, kPrppW, (HANDLE)self);
                ::SetTimer(hWnd, (UINT_PTR)self, 70, NULL);
                return 0;
            }
        }
        if (!self)
            return ::DefWindowProcW(hWnd, message, wParam, lParam);

        id = self->m_id;
        wkeWebview = self->m_wkeWebview;
        if (!wkeWebview)
            return ::DefWindowProcW(hWnd, message, wParam, lParam);
        switch (message) {
        case WM_CLOSE:
            self->m_state = WindowDestroying;
            ::ShowWindow(hWnd, SW_HIDE);
            break;

        case WM_NCDESTROY:
            ::KillTimer(hWnd, (UINT_PTR)self);
            ::RemovePropW(hWnd, kPrppW);
            ::RevokeDragDrop(hWnd);
            ThreadCall::callBlinkThreadSync([wkeWebview, self] {
                wkeDestroyWebView(wkeWebview);
            });

            self->m_state = WindowDestroyed;
            delete self;
            return 0;

        case WM_TIMER:
            //wkeRepaintIfNeeded(pthis);
            return 0;

        case WM_PAINT:
            self->onPaintMessage(hWnd);
            break;

        case WM_ERASEBKGND:
            return TRUE;

        case WM_SIZE:
        {
            ThreadCall::callBlinkThreadAsync([wkeWebview, lParam] {
                wkeResize(wkeWebview, LOWORD(lParam), HIWORD(lParam));
                wkeRepaintIfNeeded(wkeWebview);
            });

            return 0;
        }
        case WM_KEYDOWN:
        {
            unsigned int virtualKeyCode = wParam;
            unsigned int flags = 0;
            if (HIWORD(lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            ThreadCall::callBlinkThreadAsync([wkeWebview, virtualKeyCode, flags] {
                wkeFireKeyDownEvent(wkeWebview, virtualKeyCode, flags, false);
            });

            return 0;
            break;
        }
        case WM_KEYUP:
        {
            unsigned int virtualKeyCode = wParam;
            unsigned int flags = 0;
            if (HIWORD(lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            ThreadCall::callBlinkThreadAsync([wkeWebview, virtualKeyCode, flags] {
                wkeFireKeyUpEvent(wkeWebview, virtualKeyCode, flags, false);
            });

            return 0;
            break;
        }
        case WM_CHAR:
        {
            unsigned int charCode = wParam;
            unsigned int flags = 0;
            if (HIWORD(lParam) & KF_REPEAT)
                flags |= WKE_REPEAT;
            if (HIWORD(lParam) & KF_EXTENDED)
                flags |= WKE_EXTENDED;

            ThreadCall::callBlinkThreadAsync([wkeWebview, id, charCode, flags] {
                if (IdLiveDetect::get()->isLive(id))
                    wkeFireKeyPressEvent(wkeWebview, charCode, flags, false);
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
        {
            self->onCursorChange();
            self->onMouseMessage(hWnd, message, wParam, lParam);
            break;
        }
        case WM_CONTEXTMENU:
        {
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

            ThreadCall::callBlinkThreadAsync([id, wkeWebview, pt, flags] {
                if (IdLiveDetect::get()->isLive(id))
                    wkeFireContextMenuEvent(wkeWebview, pt.x, pt.y, flags);
            });
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
                flags |= WKE_CONTROL;
            if (wParam & MK_SHIFT)
                flags |= WKE_SHIFT;

            if (wParam & MK_LBUTTON)
                flags |= WKE_LBUTTON;
            if (wParam & MK_MBUTTON)
                flags |= WKE_MBUTTON;
            if (wParam & MK_RBUTTON)
                flags |= WKE_RBUTTON;

            ThreadCall::callBlinkThreadAsync([id, wkeWebview, pt, delta, flags] {
                if (IdLiveDetect::get()->isLive(id))
                    wkeFireMouseWheelEvent(wkeWebview, pt.x, pt.y, delta, flags);
            });
            break;
        }
        case WM_SETFOCUS:
            ThreadCall::callBlinkThreadAsync([id, wkeWebview] {
                if (IdLiveDetect::get()->isLive(id))
                    wkeSetFocus(wkeWebview);
            });
            return 0;

        case WM_KILLFOCUS:
            ThreadCall::callBlinkThreadAsync([id, wkeWebview] {
                if (IdLiveDetect::get()->isLive(id))
                    wkeKillFocus(wkeWebview);
            });
            return 0;

        case WM_SETCURSOR:
            if (self->setCursorInfoTypeByCache())
                return 1;
            break;

        case WM_IME_STARTCOMPOSITION:
        {
            ThreadCall::callBlinkThreadAsync([wkeWebview, hWnd] {
                wkeRect* caret = new wkeRect();
                *caret = wkeGetCaretRect(wkeWebview);
                ::PostMessage(hWnd, WM_IME_STARTCOMPOSITION_ASYN, (WPARAM)caret, 0);
            });
        }
        return 0;
        case WM_IME_STARTCOMPOSITION_ASYN:
        {
            wkeRect* caret = (wkeRect*)wParam;
            COMPOSITIONFORM compositionForm;
            compositionForm.dwStyle = CFS_POINT | CFS_FORCE_POSITION;
            compositionForm.ptCurrentPos.x = caret->x;
            compositionForm.ptCurrentPos.y = caret->y;

            delete caret;

            HIMC hIMC = ::ImmGetContext(hWnd);
            ::ImmSetCompositionWindow(hIMC, &compositionForm);
            ::ImmReleaseContext(hWnd, hIMC);
        }
        break;
        case WM_DROPFILES:
            self->onDragFiles((HDROP)wParam);
            break;
        }

        return ::DefWindowProcW(hWnd, message, wParam, lParam);
    }

private:
    static void registerClass(HINSTANCE hInstance) {
        static bool isInit = false;
        if (isInit)
            return;
        isInit = true;

        WNDCLASSEXW wcex = { 0 };

        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = windowProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = nullptr; // ::LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TESTCALL));
        wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = nullptr; //MAKEINTRESOURCEW(IDC_TESTCALL);
        wcex.lpszClassName = kPopupClassName;
        wcex.hIconSm = nullptr; // ::LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

        ::RegisterClassExW(&wcex);
    }

    static wkeWebView onCreateViewInBlinkThread(wkeWebView webView, wkeNavigationType navigationType, const wkeString url, const wkeWindowFeatures* windowFeatures) {
        wkeWebView wkeWebview = wkeCreateWebView();

        PopupWindow* newInstance = new PopupWindow();
        WebContents::CreateWindowParam* createWindowParam = new WebContents::CreateWindowParam();
        createWindowParam->styles = 0;
        createWindowParam->styleEx = 0;
        createWindowParam->transparent = false;
        createWindowParam->title = L"Electron";
        createWindowParam->x = 200;
        createWindowParam->y = 300;
        createWindowParam->width = 800;
        createWindowParam->height = 800;

        if (0) {
            createWindowParam->transparent = true;
            createWindowParam->styles = WS_POPUP;
            createWindowParam->styleEx = WS_EX_LAYERED;
        } else {
            createWindowParam->styles = WS_OVERLAPPEDWINDOW;
            createWindowParam->styleEx = 0;
        }

        wkeSettings settings;
        settings.mask = WKE_SETTING_PAINTCALLBACK_IN_OTHER_THREAD;
        wkeConfigure(&settings);

        wkeOnCreateView(wkeWebview, onCreateViewCallbackStatic, newInstance);
        wkeOnPaintUpdated(wkeWebview, (wkePaintUpdatedCallback)staticOnPaintUpdatedInCompositeThread, newInstance);
        wkeResize(wkeWebview, createWindowParam->width, createWindowParam->height);

        newInstance->m_wkeWebview = wkeWebview;

        int id = newInstance->m_id;
        ThreadCall::callUiThreadAsync([id, newInstance, createWindowParam] {
            if (!IdLiveDetect::get()->isLive(id))
                return;

            newInstance->newWindowInUiThreadWhenBlinkCall(createWindowParam);
        });

        return wkeWebview;
    }

    void newWindowInUiThreadWhenBlinkCall(const WebContents::CreateWindowParam* createWindowParam) {
        PopupWindow* self = this;
        int id = m_id;

        registerClass(nullptr);

        m_hWnd = ::CreateWindowEx(
            createWindowParam->styleEx,        // window ex-style
            kPopupClassName,    // window class name
            createWindowParam->title.c_str(), // window caption
            createWindowParam->styles,         // window style
            createWindowParam->x,              // initial x position
            createWindowParam->y,              // initial y position
            createWindowParam->width,          // initial x size
            createWindowParam->height,         // initial y size
            NULL,         // parent window handle
            NULL,           // window menu handle
            ::GetModuleHandleW(NULL),           // program instance handle
            this);         // creation parameters

        if (!::IsWindow(m_hWnd))
            return;
        ::DragAcceptFiles(m_hWnd, true);

        RECT clientRect;
        ::GetClientRect(m_hWnd, &clientRect);

        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;
        m_clientRect.right = width;
        m_clientRect.bottom = height;

        HWND hWnd = m_hWnd;
        ThreadCall::callBlinkThreadAsync([id, self, hWnd, createWindowParam, width, height] {
            if (!IdLiveDetect::get()->isLive(id))
                return;

            wkeSetHandle(self->m_wkeWebview, hWnd);
            delete createWindowParam;
        });

        ::ShowWindow(m_hWnd, TRUE);
        m_state = WindowInited;
    }

private:
    enum WindowState {
        WindowUninited,
        WindowInited,
        WindowDestroying,
        WindowDestroyed
    };
    WindowState m_state;
    static const WCHAR* kPrppW;

    wkeWebView m_wkeWebview;

    HWND m_hWnd;
    int m_cursorInfoType;
    bool m_isCursorInfoTypeAsynGetting;
    bool m_isCursorInfoTypeAsynChanged;
    CRITICAL_SECTION m_memoryCanvasLock;
    HBITMAP m_memoryBMP;
    HDC m_memoryDC;
    RECT m_clientRect;
    bool m_isLayerWindow;
    int m_id;
};

const WCHAR* PopupWindow::kPrppW = L"MtRenderMain";


}

#endif // browser_api_PopupWindow_h