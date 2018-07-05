#ifndef content_browser_ContextMeun_h
#define content_browser_ContextMeun_h

#include "content/browser/WebPage.h"
#include "third_party/WebKit/Source/web/WebViewImpl.h"
#include "third_party/WebKit/Source/platform/Timer.h"
#include "third_party/WebKit/public/web/WebContextMenuData.h"
#include "third_party/WebKit/public/web/WebFrame.h"
#include <windows.h>

namespace content {

#define kContextMenuClassName L"MbContextMenu"

class ContextMenu {
public:
    ContextMenu(WebPage* webPage)
    {
        m_popMenu = nullptr;
        m_hWnd = nullptr;
        m_imagePos = nullptr;
        m_webPage = webPage;

        init();
    }

    void init()
    {
        if (m_hWnd)
            return;

        registerClass();
        m_hWnd = CreateWindowEx(WS_EX_TOOLWINDOW, kContextMenuClassName, kContextMenuClassName, WS_POPUP | WS_MINIMIZEBOX, 
            CW_USEDEFAULT, CW_USEDEFAULT, 1, 1, HWND_DESKTOP, NULL, nullptr, this);
        ::SetPropW(m_hWnd, kContextMenuClassName, (HANDLE)this);
    }

    ~ContextMenu()
    {
        ::SetPropW(m_hWnd, kContextMenuClassName, (HANDLE)nullptr);
        ::DestroyWindow(m_hWnd);
        if (m_popMenu)
            ::DestroyMenu(m_popMenu);
        if (m_imagePos)
            delete m_imagePos;
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
        wcex.lpszClassName = kContextMenuClassName;
        wcex.hIconSm = nullptr;
        return !!RegisterClassEx(&wcex);
    }

    enum MenuId {
        kCopySelectedTextId,
        kCopyImageId,
        kInspectElementAtId,
        kCutId,
        kPasteId
    };
//     static const int kCopySelectedTextId = 1;
//     static const int kCopySelectedTextId = 1;
//     static const int kInspectElementAtId = 2;
//     static const int kCutId = 3;
//     static const int kPasteId = 4;

    void show(const blink::WebContextMenuData& data)
    {
        POINT screenPt = { 0 };
        ::GetCursorPos(&screenPt);

        POINT clientPt = screenPt;
        ::ScreenToClient(m_hWnd, &clientPt);

        if (m_imagePos)
            delete m_imagePos;
        m_imagePos = nullptr;

        if (m_popMenu)
            ::DestroyMenu(m_popMenu);
        m_popMenu = ::CreatePopupMenu();
        
        m_data = blink::WebContextMenuData();

        if ((!data.selectedText.isNull() && !data.selectedText.isEmpty()))
            ::AppendMenu(m_popMenu, MF_STRING, kCopySelectedTextId, L"¸´ÖÆ");

        if (data.hasImageContents) {
            ::AppendMenu(m_popMenu, MF_STRING, kCopyImageId, L"¸´ÖÆÍ¼Æ¬");
            m_imagePos = new blink::IntPoint(data.mousePosition);
        }

        if (m_webPage->isDevtoolsConneted())
            ::AppendMenu(m_popMenu, MF_STRING, kInspectElementAtId, L"¼ì²é");

        if (data.isEditable) {
            ::AppendMenu(m_popMenu, MF_STRING, kCutId, L"¼ôÇÐ");
            ::AppendMenu(m_popMenu, MF_STRING, kPasteId, L"Õ³Ìù");
        }

        if (0 == ::GetMenuItemCount(m_popMenu)) {
            ::DestroyMenu(m_popMenu);
            m_popMenu = nullptr;
            return;
        }

        m_data = data;

        UINT flags = TPM_RIGHTBUTTON | TPM_TOPALIGN | TPM_VERPOSANIMATION | TPM_HORIZONTAL | TPM_LEFTALIGN | TPM_HORPOSANIMATION;
        ::TrackPopupMenuEx(m_popMenu, flags, clientPt.x, clientPt.y, m_hWnd, 0);
    }

    void onCommand(UINT itemID)
    {
        if (kCopySelectedTextId == itemID) {
            m_webPage->webViewImpl()->focusedFrame()->executeCommand("Copy");
        } else if (kCopyImageId == itemID) {
            m_webPage->webViewImpl()->copyImageAt(*m_imagePos);
            delete m_imagePos;
            m_imagePos = nullptr;
        } else if (kInspectElementAtId == itemID) {
            m_webPage->inspectElementAt(m_data.mousePosition.x, m_data.mousePosition.y);
        } else if (kCutId == itemID) {
            m_webPage->webViewImpl()->focusedFrame()->executeCommand("Cut");
        } else if (kPasteId == itemID) {
            m_webPage->webViewImpl()->focusedFrame()->executeCommand("Paste");
        }
    }

    static LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
//         String output = String::format("ContextMenu.wndProc: %d\n", uMsg);
//         OutputDebugStringA(output.utf8().data());

        PAINTSTRUCT ps;
        HDC hdc = nullptr;
        ContextMenu* self = (ContextMenu*)GetPropW(hWnd, kContextMenuClassName);

        switch (uMsg) {
        case WM_TIMER:
            //self->hide();
            break;

        case WM_PAINT:
            hdc = ::BeginPaint(hWnd, &ps);
            self->onPaint(hWnd, hdc);
            ::EndPaint(hWnd, &ps);
            break;
        case WM_COMMAND: {
            UINT itemID = LOWORD(wParam);
            self->onCommand(itemID);
        }
            break;
        case WM_CLOSE:
            break;
        }

        return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    void onPaint(HWND hWnd, HDC hdc)
    {

    }

    HWND m_hWnd;
    HMENU m_popMenu;
    blink::WebContextMenuData m_data;
    WebPage* m_webPage;

    blink::IntPoint* m_imagePos;

    int m_lastX;
    int m_lastY;
};

}

#endif // content_browser_ContextMeun_h