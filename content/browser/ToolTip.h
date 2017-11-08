
#include <windows.h>
#include <xstring>

namespace content {

#define kToolTipClassName L"MbToolTip"

class ToolTip {
public:
    void init()
    {
        m_isShow = false;

        registerClass();
        m_hTipWnd = CreateWindowEx(WS_EX_TOOLWINDOW, kToolTipClassName, kToolTipClassName, WS_POPUP | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 225, 140, HWND_DESKTOP, NULL, nullptr, this);
        ::SetPropW(m_hTipWnd, kToolTipClassName, (HANDLE)this);

        m_hFont = CreateFont(18, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE,
            GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, FIXED_PITCH | FF_MODERN, L"Î¢ÈíÑÅºÚ");
    }

    ~ToolTip()
    {
        ::SetPropW(m_hTipWnd, kToolTipClassName, (HANDLE)nullptr);
        ::DeleteObject(m_hFont);
        ::DestroyWindow(m_hTipWnd);
    }

    bool registerClass()
    {
        WNDCLASSEX wcex;

        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DROPSHADOW;
        wcex.lpfnWndProc = wndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = nullptr;
        wcex.hIcon = nullptr;
        wcex.hCursor = LoadCursor(0, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = 0;
        wcex.lpszClassName = kToolTipClassName;
        wcex.hIconSm = nullptr;
        return !!RegisterClassEx(&wcex);
    }

    void show(const WCHAR* text)
    {
        if (!text) {
            hide();
            return;
        }
        std::wstring textString = text;
        if (0 == textString.size()) {
            hide();
            return;
        }

        bool isSameText = textString == m_text;
        if (!isSameText) {
            m_text = textString;
            SIZE size;
            HDC hScreenDc = ::GetDC(m_hTipWnd);
            HFONT hOldFont = (HFONT)::SelectObject(hScreenDc, m_hFont);
            ::GetTextExtentPoint32(hScreenDc, m_text.c_str(), m_text.size(), &size);
            ::SelectObject(hScreenDc, hOldFont);
            ::ReleaseDC(m_hTipWnd, hScreenDc);

            POINT point;
            ::GetCursorPos(&point);
            ::SetWindowPos(m_hTipWnd, HWND_TOPMOST, point.x + 15, point.y + 15, size.cx, size.cy, SWP_NOACTIVATE);
        }

        ::ShowWindow(m_hTipWnd, SW_SHOWNOACTIVATE);
        ::UpdateWindow(m_hTipWnd);

        resetShowState();
    }

    static LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        PAINTSTRUCT ps;
        HDC hdc = nullptr;
        ToolTip* self = (ToolTip*)GetPropW(hWnd, kToolTipClassName);

        switch (uMsg) {
        case WM_TIMER:
            self->hide();
            break;

        case WM_PAINT: {
            hdc = ::BeginPaint(hWnd, &ps);
         
            ::SelectObject(hdc, self->m_hFont);
            ::SetTextColor(hdc, RGB(0x0, 0x0, 0x0));
            ::SetBkColor(hdc, RGB(255, 255, 225));
            ::TextOut(hdc, 0, 0, self->m_text.c_str(), self->m_text.size());

            ::EndPaint(hWnd, &ps);
        }
            break;
        case WM_CLOSE:
            break;
        }

        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    std::wstring getText() const { return m_text; }

private:
    void resetShowState()
    {
        m_isShow = true;
        ::KillTimer(m_hTipWnd, m_kTimerId);
        ::SetTimer(m_hTipWnd, m_kTimerId, 5000, nullptr);
    }

    void hide()
    {
        m_text = L"";
        m_isShow = false;
        ::ShowWindow(m_hTipWnd, SW_HIDE);
    }

    HWND m_hTipWnd;
    HFONT m_hFont;
    bool m_isShow;
    static const int m_kTimerId = 1;
    std::wstring m_text;
};

}