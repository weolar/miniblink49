
#ifndef common_HideWndHelp_h
#define common_HideWndHelp_h

#include <windows.h>
#include <functional>

namespace atom {

class HideWndHelp {
public:
    HideWndHelp(
        const wchar_t* className,
        std::function<LRESULT(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)>&& messageCallback)
        : m_messageCallback(messageCallback)
        , m_className(className)
        , m_hHideWindow(nullptr) {
        createWindow();
    }

    HWND getWnd() const {
        return m_hHideWindow;
    }

private:
    static LRESULT APIENTRY staticWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        const WCHAR* kPropW = L"HideWindow";
        HideWndHelp* self = (HideWndHelp*)::GetPropW(hWnd, kPropW);
        if (!self && uMsg == WM_CREATE) {
            LPCREATESTRUCTW cs = (LPCREATESTRUCTW)lParam;
            self = (HideWndHelp*)cs->lpCreateParams;

            ::SetPropW(hWnd, kPropW, (HANDLE)self);
        }
        if (!self)
            return ::DefWindowProcW(hWnd, uMsg, wParam, lParam);

        return self->wndProc(hWnd, uMsg, wParam, lParam);
    }

    LRESULT wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        return m_messageCallback(hWnd, uMsg, wParam, lParam);
    }

    void createWindow() {
        if (m_hHideWindow)
            return;

        WNDCLASS wc = { 0 };
        wc.style = 0;
        wc.lpfnWndProc = (WNDPROC)staticWndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = NULL;
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.lpszClassName = m_className.c_str();
        ::RegisterClass(&wc);

        m_hHideWindow = ::CreateWindow(m_className.c_str(), m_className.c_str(),
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1, 1, NULL, NULL, NULL, this);

        ::ShowWindow(m_hHideWindow, SW_HIDE);
    }

private:
    std::function<LRESULT(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)> m_messageCallback;
    std::wstring m_className;
    HWND m_hHideWindow;

};

}

#endif // common_HideWndHelp_h