
#include "third_party/WebKit/Source/platform/Timer.h"
#include <windows.h>
#include <xstring>

namespace content {

#define kToolTipClassName L"MbToolTip"

class ToolTip {
public:
    ToolTip()
        : m_delayShowTimer(this, &ToolTip::delayShowTimerFired)
        , m_delayHideTimer(this, &ToolTip::delayHideTimerFired)
    {
        m_delayShowCount = 0;
        m_delayHideCount = 0;
    }
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

            HDC hScreenDc = ::GetDC(m_hTipWnd);
            HFONT hOldFont = (HFONT)::SelectObject(hScreenDc, m_hFont);
            ::GetTextExtentPoint32(hScreenDc, m_text.c_str(), m_text.size(), &m_size);
            ::SelectObject(hScreenDc, hOldFont);
            ::ReleaseDC(m_hTipWnd, hScreenDc);

            ::GetCursorPos(&m_pos);

            resetShowState();
        }
    }

    static LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        PAINTSTRUCT ps;
        HDC hdc = nullptr;
        ToolTip* self = (ToolTip*)GetPropW(hWnd, kToolTipClassName);

        switch (uMsg) {
        case WM_TIMER:
            //self->hide();
            break;

        case WM_PAINT:
            hdc = ::BeginPaint(hWnd, &ps);
            self->onPaint(hWnd, hdc);
            ::EndPaint(hWnd, &ps);
            break;
        case WM_CLOSE:
            break;
        }

        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    void onPaint(HWND hWnd, HDC hdc)
    {
        RECT rc = { 0 };
        ::GetClientRect(hWnd, &rc);
        ::Rectangle(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top);
        HBRUSH hbrush = ::CreateSolidBrush(RGB(0x10, 0x10, 0x10));
        ::SelectObject(hdc, hbrush);
        ::Rectangle(hdc, 220, 40, 366, 266);
        ::DeleteObject(hbrush);

        ::SelectObject(hdc, m_hFont);
        ::SetTextColor(hdc, RGB(0x0, 0x0, 0x0));
        ::SetBkColor(hdc, RGB(255, 255, 225));
        ::TextOut(hdc, 2, 2, m_text.c_str(), m_text.size());
    }

    std::wstring getText() const { return m_text; }

private:
    void resetShowState()
    {
        if (m_delayShowTimer.isActive())
            m_delayShowTimer.stop();
        m_delayShowCount = 0;
        m_delayShowTimer.start(0.02, 0.02, FROM_HERE);
    }

    bool isNearPos(const POINT& a, const POINT& b)
    {
        return std::abs(a.x - b.x) + std::abs(a.y - b.y) < 15;
    }

    void delayShowTimerFired(blink::Timer<ToolTip>*)
    {
        ++m_delayShowCount;

        POINT point;
        ::GetCursorPos(&point);
        if (!isNearPos(point, m_pos))
            return hide();

        if (15 < m_delayShowCount && !m_isShow) {
            m_isShow = true;

            ::SetWindowPos(m_hTipWnd, HWND_TOPMOST, point.x + 15, point.y + 15, m_size.cx + 5, m_size.cy + 5, SWP_NOACTIVATE);
            ::ShowWindow(m_hTipWnd, SW_SHOWNOACTIVATE);
            ::UpdateWindow(m_hTipWnd);
//             if (m_delayShowTimer.isActive())
//                 m_delayShowTimer.stop();
//             m_delayShowCount = 0;

//             if (m_delayHideTimer.isActive())
//                 m_delayHideTimer.stop();
//             m_delayHideCount = 0;
//             m_delayHideTimer.start(0.02, 0.02, FROM_HERE);
        }
    }

    void delayHideTimerFired(blink::Timer<ToolTip>*)
    {
        ++m_delayHideCount;

        POINT point;
        ::GetCursorPos(&point);
        if (!isNearPos(point, m_pos) || 30 < m_delayHideCount)
            return hide();
    }

    void hide()
    {
        m_text = L"";
        m_isShow = false;
        ::ShowWindow(m_hTipWnd, SW_HIDE);

        if (m_delayShowTimer.isActive())
            m_delayShowTimer.stop();
        m_delayShowCount = 0;

        if (m_delayHideTimer.isActive())
            m_delayHideTimer.stop();
        m_delayHideCount = 0;
    }

    HWND m_hTipWnd;
    HFONT m_hFont;
    bool m_isShow;
    std::wstring m_text;

    blink::Timer<ToolTip> m_delayShowTimer;
    int m_delayShowCount;
    blink::Timer<ToolTip> m_delayHideTimer;
    int m_delayHideCount;
    POINT m_pos;
    SIZE m_size;
};

}