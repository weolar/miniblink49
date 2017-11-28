
#include "wke.h"
#include "CallMgr.h"

#include <windows.h>
#include <ShellAPI.h>
#include <set>
#include <vector>

std::string utf16ToUtf8(LPCWSTR lpszSrc);

namespace MtRender {

class Window;

class LiveIdDetect {
public:
    LiveIdDetect()
    {
        m_idGen = 0;
        ::InitializeCriticalSection(&m_liveSelfLock);
    }

    ~LiveIdDetect()
    {
    }

    int constructed()
    {
        ::EnterCriticalSection(&m_liveSelfLock);
        int id = ++m_idGen;
        m_liveSelf.insert(id);
        ::LeaveCriticalSection(&m_liveSelfLock);
        return id;
    }

    void deconstructed(int id)
    {
        ::EnterCriticalSection(&m_liveSelfLock);
        std::set<int>::const_iterator it = m_liveSelf.find(id);
        if (it != m_liveSelf.end()) {
            ::LeaveCriticalSection(&m_liveSelfLock);
            return;
        }
        m_liveSelf.erase(it);
        ::LeaveCriticalSection(&m_liveSelfLock);
    }

    static LiveIdDetect* get()
    {
        if (!m_inst)
            m_inst = new LiveIdDetect();
        return m_inst;
    }

    bool isLive(int id)
    {
        ::EnterCriticalSection(&m_liveSelfLock);
        std::set<int>::const_iterator it = m_liveSelf.find(id);
        bool b = it != m_liveSelf.end();
        ::LeaveCriticalSection(&m_liveSelfLock);
        return b;
    }

private:
    int m_idGen;
    std::set<int> m_liveSelf;
    CRITICAL_SECTION m_liveSelfLock;
    static LiveIdDetect* m_inst;
};

LiveIdDetect* LiveIdDetect::m_inst = nullptr;

//////////////////////////////////////////////////////////////////////////

class WindowsMgr {
public:
    typedef std::vector<Window*> WindowVector;
    typedef WindowVector::iterator iterator;
    typedef WindowVector::const_iterator const_iterator;

    // Windows are added to the list before they have constructed windows,
    // so the |window()| member function may return NULL.
    const_iterator begin() const { return m_windows.begin(); }
    const_iterator end() const { return m_windows.end(); }

    iterator begin() { return m_windows.begin(); }
    iterator end() { return m_windows.end(); }

    bool empty() const { return m_windows.empty(); }
    size_t size() const { return m_windows.size(); }

    Window* get(size_t index) const { return m_windows[index]; }

//     Window* find(int id) const
//     {
//         for (WindowVector::const_iterator it = m_windows.begin(); it != m_windows.end(); ++it) {
//             if ((*it)->getId() == id)
//                 return *it;
//         }
//         return nullptr;
//     }

    static WindowsMgr* getInstance()
    {
        if (!m_instance)
            m_instance = new WindowsMgr;
        return m_instance;
    }

    // Adds or removes |window| from the list it is associated with.
    static void addWindow(Window* window)
    {
        if (window) {
            // Push |window| on the appropriate list instance.
            WindowVector& windows = getInstance()->m_windows;
            windows.push_back(window);
        }
    }

    static void removeWindow(Window* window)
    {
        WindowVector& windows = getInstance()->m_windows;
        for (WindowVector::iterator i = windows.begin(); i != windows.begin(); ++i) {
            if (*i == window) {
                windows.erase(i);
                return;
            }
        }
    }

//     static void closeAllWindows()
//     {
//         WindowVector windows = getInstance()->m_windows;
//         for (Window* window : windows)
//             if (!window->isClosed())
//                 window->close();
//     }

    static void onWindowAllClosed()
    {
        CallMgr::callBlinkThreadSync([] {
            CallMgr::exitMessageLoop(::GetCurrentThreadId());
        });
        CallMgr::exitMessageLoop(::GetCurrentThreadId());
    }

private:

    WindowsMgr()
    {
    }

    ~WindowsMgr()
    {
    }

    // A vector of the windows in this list, in the order they were added.
    WindowVector m_windows;

    static WindowsMgr* m_instance;
};

WindowsMgr* WindowsMgr::m_instance = nullptr;

class Window {
public:
    struct CreateWindowParam {
        int x;
        int y;
        int width;
        int height;
        unsigned styles;
        unsigned styleEx;
        bool transparent;
        std::wstring title;
    };

    Window()
    {
        m_state = WindowUninited;
        m_hWnd = nullptr;
        m_cursorInfoType = 0;
        m_isCursorInfoTypeAsynGetting = false;
        m_memoryBMP = nullptr;
        m_memoryDC = nullptr;
        m_isLayerWindow = false;
        m_wkeWebview = nullptr;
        m_clientRect.left = 0;
        m_clientRect.top = 0;
        m_clientRect.right = 0;
        m_clientRect.bottom = 0;
        ::InitializeCriticalSection(&m_memoryCanvasLock);
        m_id = LiveIdDetect::get()->constructed();
    }

    ~Window()
    {
        if (m_memoryBMP)
            ::DeleteObject(m_memoryBMP);
        if (m_memoryDC)
            ::DeleteDC(m_memoryDC);

        ::SendMessage(m_hWnd, WM_CLOSE, 0, 0);

        LiveIdDetect::get()->deconstructed(m_id);

        ::DeleteCriticalSection(&m_memoryCanvasLock);
    }

    
    bool isClosed()
    {
        return m_state == WindowDestroyed;
    }

    void close()
    {
        ::DestroyWindow(m_hWnd);
    }

    int getId() const
    {
        return m_id;
    }

    void onPaintUpdatedInCompositeThread(const HDC hdc, int x, int y, int cx, int cy)
    {
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

    void onPaintUpdatedInUiThread(int x, int y, int cx, int cy)
    {
        ::EnterCriticalSection(&m_memoryCanvasLock);

        HDC hdcScreen = ::GetDC(m_hWnd);
        ::BitBlt(hdcScreen, x, y, cx, cy, m_memoryDC, x, y, SRCCOPY);
        ::ReleaseDC(m_hWnd, hdcScreen);

        ::LeaveCriticalSection(&m_memoryCanvasLock);
    }

    static void staticOnPaintUpdatedInCompositeThread(wkeWebView webView, Window* win, const HDC hdc, int x, int y, int cx, int cy)
    {
        ::EnterCriticalSection(&win->m_memoryCanvasLock);
        win->onPaintUpdatedInCompositeThread(hdc, x, y, cx, cy);
        ::LeaveCriticalSection(&win->m_memoryCanvasLock);

        if (win->m_isLayerWindow) {
            int id = win->m_id;
            CallMgr::callUiThreadAsync([id, win, x, y, cx, cy] {
                if (LiveIdDetect::get()->isLive(id))
                    win->onPaintUpdatedInUiThread(x, y, cx, cy);
            });
        } else {
            RECT rc = { x, y, x + cx, y + cy };
            ::InvalidateRect(win->m_hWnd, &rc, false);
        }
    }

    void onPaintMessage(HWND hWnd)
    {
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

    void onMouseMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
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

        CallMgr::callBlinkThreadAsync([id, wkeWebview, message, x, y, flags] {
            if (LiveIdDetect::get()->isLive(id))
                wkeFireMouseEvent(wkeWebview, message, x, y, flags);
        });
    }

    void onCursorChange()
    {
        if (m_isCursorInfoTypeAsynGetting)
            return;
        m_isCursorInfoTypeAsynGetting = true;

        int id = m_id;
        wkeWebView wkeWebview = m_wkeWebview;
        Window* win = this;
        CallMgr::callBlinkThreadAsync([wkeWebview, win, id] {
            if (!LiveIdDetect::get()->isLive(id))
                return;
            win->m_isCursorInfoTypeAsynGetting = false;
            int cursorType = wkeGetCursorInfoType(wkeWebview);
            if (cursorType == win->m_cursorInfoType)
                return;
            win->m_cursorInfoType = cursorType;
            ::PostMessage(win->m_hWnd, WM_SETCURSOR_ASYN, 0, 0);
        });
    }

    void setCursorInfoTypeByCache()
    {
        HCURSOR hCur = NULL;
        switch (m_cursorInfoType) {
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
        default:
            hCur = ::LoadCursor(NULL, IDC_ARROW);
            break;
        }

        if (hCur) {
            ::SetCursor(hCur);
        }
    }

    void onDragFiles(HDROP hDrop)
    {
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

        CallMgr::callBlinkThreadAsync([wkeWebview, id, fileNames, curPos, screenPos] {
            if (!LiveIdDetect::get()->isLive(id))
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

    static LRESULT CALLBACK windowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        int id = -1;
        Window* self = (Window *)::GetPropW(hWnd, kPrppW);
        wkeWebView wkeWebview = nullptr;
        if (!self) {
            if (message == WM_CREATE) {
                LPCREATESTRUCTW cs = (LPCREATESTRUCTW)lParam;
                self = (Window *)cs->lpCreateParams;
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
            CallMgr::callBlinkThreadSync([wkeWebview, self] {
                wkeDestroyWebView(wkeWebview);
                //self->m_webContents = nullptr;
            });

            self->m_state = WindowDestroyed;
            WindowsMgr::getInstance()->removeWindow(self);
            if (WindowsMgr::getInstance()->empty()) {
                WindowsMgr::getInstance()->onWindowAllClosed();
            }
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
            ::EnterCriticalSection(&self->m_memoryCanvasLock);
            //             if (self->m_memoryDC)
            //                 ::DeleteDC(self->m_memoryDC);
            //             self->m_memoryDC = nullptr;
            //
            //             if (self->m_memoryBMP)
            //                 ::DeleteObject((HGDIOBJ)self->m_memoryBMP);
            //             self->m_memoryBMP = nullptr;
            // 
            //             ::GetClientRect(hWnd, &self->m_clientRect);
            ::LeaveCriticalSection(&self->m_memoryCanvasLock);

            CallMgr::callBlinkThreadAsync([wkeWebview, lParam] {
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

            CallMgr::callBlinkThreadAsync([wkeWebview, virtualKeyCode, flags] {
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

            CallMgr::callBlinkThreadAsync([wkeWebview, virtualKeyCode, flags] {
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

            CallMgr::callBlinkThreadAsync([wkeWebview, id, charCode, flags] {
                if (LiveIdDetect::get()->isLive(id))
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

            CallMgr::callBlinkThreadAsync([id, wkeWebview, pt, flags] {
                if (LiveIdDetect::get()->isLive(id))
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

            CallMgr::callBlinkThreadAsync([id, wkeWebview, pt, delta, flags] {
                if (LiveIdDetect::get()->isLive(id))
                    wkeFireMouseWheelEvent(wkeWebview, pt.x, pt.y, delta, flags);
            });
            break;
        }
        case WM_SETFOCUS:
            CallMgr::callBlinkThreadAsync([id, wkeWebview] {
                if (LiveIdDetect::get()->isLive(id))
                    wkeSetFocus(wkeWebview);
            });
            return 0;

        case WM_KILLFOCUS:
            CallMgr::callBlinkThreadAsync([id, wkeWebview] {
                if (LiveIdDetect::get()->isLive(id))
                    wkeKillFocus(wkeWebview);
            });
            return 0;

        case WM_SETCURSOR:
            return 0;
            break;

        case WM_SETCURSOR_ASYN:
            self->setCursorInfoTypeByCache();

        case WM_IME_STARTCOMPOSITION:
        {
            CallMgr::callBlinkThreadAsync([wkeWebview, hWnd] {
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
    void closeApi()
    {
        ::SendMessage(m_hWnd, WM_CLOSE, 0, 0);
    }

    void focusApi()
    {
        ::SetFocus(m_hWnd);
    }

    void blurApi()
    {
        ::SetFocus(NULL);
    }

    bool isFocusedApi()
    {
        return ::GetFocus() == m_hWnd;
    }

    void showApi()
    {
        ::ShowWindow(m_hWnd, TRUE);
        ::SetFocus(m_hWnd);
    }

    void showInactiveApi()
    {
        ::ShowWindow(m_hWnd, TRUE);
    }

    void hideApi()
    {
        ::ShowWindow(m_hWnd, FALSE);
    }

    bool isVisibleApi()
    {
        return !!::IsWindowVisible(m_hWnd);
    }

    //isEnabled
    bool isEnabledApi()
    {
        return !!::IsWindowEnabled(m_hWnd);
    }

    //maximize
    void maximizeApi()
    {
        ::ShowWindow(m_hWnd, SW_MAXIMIZE);
    }

    void unmaximizeApi()
    {
        ::ShowWindow(m_hWnd, SW_RESTORE);
    }

    bool isMaximizedApi()
    {
        return !!::IsZoomed(m_hWnd);
    }

    void minimizeApi()
    {
        ::ShowWindow(m_hWnd, SW_MINIMIZE);
    }

    //restore
    void restoreApi()
    {
        ::ShowWindow(m_hWnd, SW_RESTORE);
    }

    //isMinimized
    bool isMinimizedApi()
    {
        return !!IsIconic(m_hWnd);
    }

    //setFullScreen
    void setFullScreenApi(bool b)
    {
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

    bool isFullScreenApi()
    {
        OutputDebugStringA("isFullScreenApi\n");
        return false;
    }

    void setParentWindowApi()
    {
        OutputDebugStringA("setParentWindowApi\n");
        DebugBreak();
    }

    void getParentWindowApi()
    {
        OutputDebugStringA("getParentWindowApi\n");
        DebugBreak();
    }

    void getChildWindowsApi()
    {
        OutputDebugStringA("getChildWindowsApi\n");
        DebugBreak();
    }

    bool isModalApi()
    {
        OutputDebugStringA("isModalApi\n");
        DebugBreak();
        return false;
    }

    UINT_PTR getNativeWindowHandleApi()
    {
        //args.GetReturnValue().Set(toBuffer(isolate, (void*)(&self->m_hWnd), sizeof(HWND)));
        return (UINT_PTR)m_hWnd;
    }

    void setSizeApi(int32_t width, int32_t height)
    {
        ::SetWindowPos(m_hWnd, NULL, 0, 0, width, height, SWP_NOMOVE);
    }

    void getContentBoundsApi()
    {
        ::OutputDebugStringA("getContentBoundsApi\n");
        ::DebugBreak();
    }

    void setContentBoundsApi()
    {
        ::OutputDebugStringA("setContentBoundsApi\n");
        ::DebugBreak();
    }

    std::vector<int> getContentSizeApi()
    {
        Window* self = this;
        int width;
        int height;
        wkeWebView wkeWebview = m_wkeWebview;
        CallMgr::callBlinkThreadSync([wkeWebview, &width, &height] {
            width = wkeGetContentWidth(wkeWebview);
            height = wkeGetContentHeight(wkeWebview);
        });
        std::vector<int> size = { width, height };
        return size;
    }

    void setContentSizeApi(int width, int height)
    {
        Window* self = this;
        int id = self->m_id;
        wkeWebView wkeWebview = m_wkeWebview;
        CallMgr::callBlinkThreadAsync([wkeWebview, id, width, height] {
            if (!LiveIdDetect::get()->isLive(id))
                return;
            wkeResize(wkeWebview, width, height);
            wkeRepaintIfNeeded(wkeWebview);
        });
    }

    void setMinimumSizeApi()
    {
    }

    void getMinimumSizeApi()
    {
    }

    void setMaximumSizeApi()
    {
    }

    void getMaximumSizeApi()
    {
    }

    void setResizableApi()
    {
    }

    void isResizableApi()
    {
    }

    void setMovableApi()
    {
    }

    void isMovableApi()
    {
    }

    void setMinimizableApi()
    {
    }

    void isMinimizableApi()
    {
    }

    void isMaximizableApi()
    {
    }

    void setFullScreenableApi()
    {
    }

    void isFullScreenableApi()
    {
    }

    void setClosableApi()
    {
    }

    void isClosableApi()
    {
    }

    void setAlwaysOnTopApi(bool b)
    {
        ::SetWindowPos(m_hWnd, b ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }

    bool isAlwaysOnTopApi()
    {
        return 0 == (::GetWindowLong(m_hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST);
    }

    void centerApi()
    {
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

    void setPositionApi(int x, int y)
    {
        ::SetWindowPos(m_hWnd, NULL, x, y, 0, 0, SWP_NOSIZE);
    }

    std::vector<int> getPositionApi()
    {
        RECT rect = { 0 };
        ::GetWindowRect(m_hWnd, &rect);
        std::vector<int> pos = { rect.left, rect.top };
        return pos;
    }

//     void setTitleApi(const std::string& title)
//     {
//         std::wstring titleW;
//         titleW = StringUtil::UTF8ToUTF16(title);
//         ::SetWindowText(m_hWnd, titleW.c_str());
//     }
//
//     std::string getTitleApi()
//     {
//         std::vector<wchar_t> titleW;
//         titleW.resize(MAX_PATH + 1);
//         ::GetWindowText(m_hWnd, &titleW[0], MAX_PATH);
//         std::string titleA;
//         titleA = StringUtil::UTF16ToUTF8(std::wstring(&titleW[0], titleW.size()));
//         return titleA;
//     }

    void flashFrameApi()
    {
    }

    void setSkipTaskbarApi(bool b)
    {
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

    void setBackgroundColorApi()
    {
    }

    void setDocumentEditedApi(bool b)
    {
        int id = m_id;
        wkeWebView wkeWebview = m_wkeWebview;
        CallMgr::callBlinkThreadSync([wkeWebview, id, b] {
            if (!LiveIdDetect::get()->isLive(id))
                return;
            wkeSetEditable(wkeWebview, b);
        });
    }

    void isDocumentEditedApi()
    {
    }

    void setIgnoreMouseEventsApi()
    {
    }

    void setContentProtectionApi()
    {
    }

    void setFocusableApi()
    {
    }

    void focusOnWebViewApi()
    {
        int id = m_id;
        wkeWebView wkeWebview = m_wkeWebview;
        CallMgr::callBlinkThreadSync([wkeWebview, id] {
            if (!LiveIdDetect::get()->isLive(id))
                return;
            wkeSetFocus(wkeWebview);
        });
    }

    void isWebViewFocusedApi()
    {
    }

    void setOverlayIconApi()
    {
    }

    void setThumbarButtonsApi()
    {
    }

    void setMenuApi()
    {
    }

    void setAutoHideMenuBarApi()
    {
    }

    void isMenuBarAutoHideApi()
    {
    }

    void setMenuBarVisibilityApi()
    {
    }

    void isMenuBarVisibleApi()
    {
    }

    void setVisibleOnAllWorkspacesApi()
    {
    }

    void isVisibleOnAllWorkspacesApi()
    {
    }

    void hookWindowMessageApi()
    {
    }

    void isWindowMessageHookedApi()
    {
    }

    void unhookWindowMessageApi()
    {
    }

    void unhookAllWindowMessagesApi()
    {
    }

    void setThumbnailClipApi()
    {
    }

    void setThumbnailToolTipApi()
    {
    }

    void setAppDetailsApi()
    {
    }

    void setIconApi()
    {
    }

    int getIdApi() const
    {
        return m_id;
    }

public:
    static Window* newWindow()
    {
        registerClass(nullptr);
        Window* self = new Window();
        CreateWindowParam* createWindowParam = new CreateWindowParam();
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

        self->newWindowTaskInUiThread(createWindowParam);

        return self;
    }

private:
    static void registerClass(HINSTANCE hInstance)
    {
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
        wcex.lpszClassName = L"mb_electron_window";
        wcex.hIconSm = nullptr; // ::LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

        ::RegisterClassExW(&wcex);
    }

    void newWindowTaskInUiThread(const CreateWindowParam* createWindowParam)
    {
        Window* self = this;
        int id = m_id;
        CallMgr::callBlinkThreadAsync([self, id] {
            if (!LiveIdDetect::get()->isLive(id))
                return;

            self->m_wkeWebview = wkeCreateWebView();
            wkeSetUserKeyValue(self->m_wkeWebview, "WebContents", self);
        });

        m_hWnd = ::CreateWindowEx(
            createWindowParam->styleEx,        // window ex-style
            L"mb_electron_window",    // window class name
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
        CallMgr::callBlinkThreadAsync([id, self, hWnd, createWindowParam, width, height] {
            if (!LiveIdDetect::get()->isLive(id))
                return;

            wkeSetHandle(self->m_wkeWebview, hWnd);

            self->onNewWindowInBlinkThread(width, height, createWindowParam);
            wkeOnPaintUpdated(self->m_wkeWebview, (wkePaintUpdatedCallback)staticOnPaintUpdatedInCompositeThread, self);
            delete createWindowParam;
        });

        ::ShowWindow(m_hWnd, TRUE);
        m_state = WindowInited;
    }

    void onNewWindowInBlinkThread(int width, int height, const CreateWindowParam* createWindowParam)
    {
        if (createWindowParam->transparent)
            wkeSetTransparent(m_wkeWebview, true);
        wkeSettings settings;
        settings.mask = WKE_SETTING_PAINTCALLBACK_IN_OTHER_THREAD;
        wkeConfigure(&settings);
        wkeResize(m_wkeWebview, width, height);
        wkeOnCreateView(m_wkeWebview, onCreateViewCallbackStatic, this);

        const wchar_t* htmlW = L"<html><head></head><body style='font-size:16px;'>"
            L"输入网址: &nbsp<input id=myurl style='width:400px;' onkeydown=if(event.keyCode==13)GoURL(); value='http://news.baidu.com'><button onclick=GoURL();>GO</button><hr />"
            L"<a href='http://www.taobao.com/'>淘宝</a><br />"
            L"<a href='http://www.youku.com/'>优酷</a><br />"
            L"<a href='http://kugou.id.sn.cn/parse.php'>audio元素测试</a><br />"
            L"<a href='http://www.youzu.com/'>游族</a><br />"
            L"<a href='http://www.baidu.com/'>百度</a><br />"
            L"<a href='http://pan.baidu.com/'>百度网盘</a><br />"
            L"<a href='http://map.baidu.com/'>百度地图</a><br />"
            L"<a href='http://www.le.com/'>乐视</a><br />"
            L"<a href='http://download.csdn.net/'>CSDN</a><br />"
            L"<a href='http://www.yvoschaap.com/chainrxn/'>2D测试</a><br />"
            L"<a href='https://www.benjoffe.com/code/demos/canvascape/textures'>3D测试</a><br />"
            L"<a href='http://www.jz5u.com/soft/softdown.asp?softid=18109'>下载测试</a><br />"
            L"<a href='http://workerman.net:8383/'>蝌蚪聊天室</a><br />"
            L"<a href='http://chat.workerman.net/'>Websocket聊天室</a><br />"
            L"<a href='http://www.workerman.net/demos/browserquest/'>Websocket在线游戏</a><br />"
            L"<span onclick=JsCall('')>JsCall</span>"
            L"</body><script>function GoURL(){document.location=document.getElementById('myurl').value;}</script></html>";
        std::string htmlA = utf16ToUtf8(htmlW);
        wkeLoadHTML(m_wkeWebview, htmlA.c_str());
    }

    static wkeWebView onCreateViewCallbackStatic(wkeWebView webView, void* param, wkeNavigationType navigationType, const wkeString url, const wkeWindowFeatures* windowFeatures)
    {
        Window* self = (Window*)param;
        return self->onCreateViewInBlinkThread(webView, navigationType, url, windowFeatures);
    }

    wkeWebView onCreateViewInBlinkThread(wkeWebView webView, wkeNavigationType navigationType, const wkeString url, const wkeWindowFeatures* windowFeatures)
    {
        wkeWebView wkeWebview = wkeCreateWebView();
        
        Window* newInstance = new Window();
        CreateWindowParam* createWindowParam = new CreateWindowParam();
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
        CallMgr::callUiThreadAsync([id, newInstance, createWindowParam] {
            if (!LiveIdDetect::get()->isLive(id))
                return;

            newInstance->newWindowInUiThreadWhenBlinkCall(createWindowParam);
        });
       
        return wkeWebview;
    }

    void newWindowInUiThreadWhenBlinkCall(const CreateWindowParam* createWindowParam)
    {
        Window* self = this;
        int id = m_id;

        m_hWnd = ::CreateWindowEx(
            createWindowParam->styleEx,        // window ex-style
            L"mb_electron_window",    // window class name
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
        CallMgr::callBlinkThreadAsync([id, self, hWnd, createWindowParam, width, height] {
            if (!LiveIdDetect::get()->isLive(id))
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
    CRITICAL_SECTION m_memoryCanvasLock;
    HBITMAP m_memoryBMP;
    HDC m_memoryDC;
    RECT m_clientRect;
    bool m_isLayerWindow;
    int m_id;
};

const WCHAR* Window::kPrppW = L"MtRenderMain";

}

int MtRenderMain()
{
    MtRender::CallMgr::init();

    MtRender::Window* win = MtRender::Window::newWindow();

    MtRender::CallMgr::messageLoop();

    return 0;
}