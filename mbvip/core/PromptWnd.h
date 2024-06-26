
#ifndef core_PromptWnd_h
#define core_PromptWnd_h

#include "common/StringUtil.h"
#include <process.h>

namespace mb {
    
class PromptWnd {
public:
    PromptWnd(HWND hParantWnd)
    {
        m_running = TRUE;
        m_hParantWnd = hParantWnd;
    }

    ~PromptWnd()
    {

    }

private:
    static void registerClass(const std::wstring& windowClass)
    {
        // Only register the class one time.
        static bool classRegistered = false;
        if (classRegistered)
            return;
        classRegistered = true;

        WNDCLASSEX wcex;
        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = promptWndProc;
        wcex.cbClsExtra = 0;
        wcex.cbWndExtra = 0;
        wcex.hInstance = nullptr;
        wcex.hIcon = nullptr; // LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WKEXE));
        wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground = nullptr;
        wcex.lpszMenuName = nullptr;
        wcex.lpszClassName = windowClass.c_str();
        wcex.hIconSm = nullptr;

        RegisterClassEx(&wcex);
    }

    const static int wndWidth = 388;
    const static int wndHeight = 215;

    const static int IDC_OK_ID = 1;
    const static int IDC_CANCEL_ID = 2;

    bool onCommand(UINT id)
    {
        if (IDC_OK_ID == id) {
            std::vector<WCHAR> text;
            text.resize(260);
            memset(&text.at(0), 0, sizeof(WCHAR) * 260);
            ::GetWindowText(m_editHwnd, &text.at(0), 256);

            m_result = &text.at(0);
            ::PostQuitMessage(0);
        } else if (IDC_CANCEL_ID == id) {
            ::PostQuitMessage(0);
        }
        return true;
    }

    static LRESULT CALLBACK promptWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        PromptWnd* self = reinterpret_cast<PromptWnd*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (!self)
            return ::DefWindowProc(hWnd, message, wParam, lParam);

        switch (message) {
        case WM_COMMAND:
            if (self->onCommand(LOWORD(wParam)))
                return 0;
            break;
        case WM_CTLCOLORSTATIC:
        {
            HDC hdcStatic = (HDC)wParam;
            ::SetTextColor(hdcStatic, RGB(0, 0, 0));
            ::SetBkColor(hdcStatic, RGB(0xff, 0xff, 0xfF));
            return (INT_PTR)self->m_hBrush;
        }
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = ::BeginPaint(hWnd, &ps);
            self->onPaint(hdc);
            ::EndPaint(hWnd, &ps);
        }
        break;
        case WM_LBUTTONDOWN:
            ::PostMessage(hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
            break;
        }

        return ::DefWindowProc(hWnd, message, wParam, lParam);
    }

    void onPaint(HDC hdc)
    {
        ::SelectObject(hdc, m_hBrush);
        ::Rectangle(hdc, 1, 1, wndWidth - 1, wndHeight - 1);
    }

    static unsigned int __stdcall threadFunc(void* param)
    {
        PromptWnd* self = (PromptWnd*)param;
        self->show(self->m_title, self->m_text);

        MSG msg = { 0 };
        while (true) {
            if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
                if (WM_QUIT == msg.message)
                    break;
                ::TranslateMessage(&msg);
                ::DispatchMessageW(&msg);
            }
            ::Sleep(5);
        }

        ::DestroyWindow(self->m_hWnd);
        ::DeleteObject(self->m_hBrush);
        ::DeleteObject(self->m_font);

        self->m_running = FALSE;
        return 0;
    }

public:
    std::string run(const std::string& title, const std::string& text)
    {
        m_title = common::utf8ToUtf16(title);
        m_text = common::utf8ToUtf16(text);

        unsigned int threadIdentifier = 0;
        HANDLE threadHandle = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, threadFunc, this, 0, &threadIdentifier));
        ::CloseHandle(threadHandle);

        while (m_running) {
            ::Sleep(50);
        }

        return common::utf16ToUtf8(m_result.c_str());
    }

private:
    void show(const std::wstring& title, const std::wstring& text)
    {
        const static int buttonWidth = 72;
        const static int urlbarHeight = 24;
        const static int fontHeight = 14;

        const std::wstring& windowClass = L"MbPromptWindow";
        registerClass(windowClass);

        m_hBrush = ::CreateSolidBrush(RGB(0xff, 0xff, 0xfF));

        HWND hWnd = ::CreateWindowW(windowClass.c_str(), L"promptBox",
            WS_POPUP | WS_CLIPCHILDREN,
            700, 300, wndWidth, wndHeight,
            m_hParantWnd, NULL, nullptr, NULL);
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        SetForegroundWindow(hWnd);
        m_hWnd = hWnd;

        m_font = ::CreateFont(
            -fontHeight, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial");

        m_editHwnd = ::CreateWindow(
            L"EDIT", 0,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT | ES_AUTOVSCROLL |
            ES_AUTOHSCROLL,
            14, 135, 360, 22,
            hWnd, 0, NULL, 0);
        ::SendMessage(m_editHwnd, WM_SETFONT, reinterpret_cast<WPARAM>(m_font), TRUE);
        ::SetWindowText(m_editHwnd, text.c_str());

        m_okHwnd = ::CreateWindow(
            L"BUTTON", L"yes",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            180, 174, buttonWidth, urlbarHeight,
            hWnd, reinterpret_cast<HMENU>((intptr_t)IDC_OK_ID), nullptr, 0);
        ::SendMessage(m_okHwnd, WM_SETFONT, reinterpret_cast<WPARAM>(m_font), TRUE);

        m_cancelHwnd = ::CreateWindow(
            L"BUTTON", L"no",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            290, 174, buttonWidth, urlbarHeight,
            hWnd, reinterpret_cast<HMENU>((intptr_t)IDC_CANCEL_ID), nullptr, 0);
        ::SendMessage(m_cancelHwnd, WM_SETFONT, reinterpret_cast<WPARAM>(m_font), TRUE);

        m_staticHwnd = CreateWindow(L"static", L"C”Ô—‘÷–ŒƒÕ¯",
            WS_CHILD | WS_VISIBLE | /*WS_BORDER |*/ SS_CENTER | SS_CENTERIMAGE,
            20, 20, 342, 100,
            hWnd, NULL, NULL, NULL);
        SendMessage(m_staticHwnd, WM_SETFONT, (WPARAM)m_font, NULL);
        SetWindowText(m_staticHwnd, title.c_str());

        ::ShowWindow(hWnd, SW_SHOW);
    }

private:
    HFONT m_font;
    HBRUSH m_hBrush;

    HWND m_hParantWnd;
    HWND m_hWnd;
    HWND m_editHwnd;
    HWND m_okHwnd;
    HWND m_cancelHwnd;
    HWND m_staticHwnd;

    std::wstring m_title;
    std::wstring m_text;
    std::wstring m_result;
    BOOL m_running;
};

}

#endif // core_PromptWnd_h