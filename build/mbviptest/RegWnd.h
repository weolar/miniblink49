
#ifndef RegWnd_h
#define RegWnd_h

#include "RegWndHtml.h"
#include "RootWindow.h"

#include <xstring>
#include <vector>
#include <functional>

class RegWnd {
public:
    RegWnd()
    {
        m_mbView = NULL_WEBVIEW;
    }

    static RegWnd* getInst()
    {
        if (!m_inst)
            m_inst = new RegWnd();
        return m_inst;
    }

    void onClose()
    {
        m_mbView = NULL_WEBVIEW;
    }

    void createRegWnd()
    {
        HWND hWnd = nullptr;
        if (m_mbView) {
            hWnd = mbGetHostHWND(m_mbView);
            SetForegroundWindow(hWnd);
            return;
        }
        
        m_mbView = mbCreateWebWindow(MB_WINDOW_TYPE_TRANSPARENT, NULL, 220, 220, 520, 640);
        mbMoveToCenter(m_mbView);

        hWnd = mbGetHostHWND(m_mbView);
        DWORD dwStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
        dwStyle = dwStyle | WS_EX_TOOLWINDOW;
        ::SetWindowLong(hWnd, GWL_EXSTYLE, dwStyle);
        ::SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

        mbOnCreateView(m_mbView, RootWindow::onCreateView, this);
        mbOnJsQuery(m_mbView, onJsQueryCallback, this);

        //mbLoadURL(m_mbView, "file:///E:/mycode/mtmb/testdll/RegWnd.html");
        mbLoadHtmlWithBaseUrl(m_mbView, (const utf8 *)kRegWndHtml, "RegWnd.html");
        mbShowWindow(m_mbView, TRUE);

        ::ShowWindow(hWnd, SW_SHOW);
        ::UpdateWindow(hWnd);
    }

private:
    static void MB_CALL_TYPE onJsQueryCallback(mbWebView webView, void* param, mbJsExecState es, int64_t queryId, int customMsg, const utf8* request)
    {
        HWND hWnd = mbGetHostHWND(webView);
        if (0 == strcmp(request, "close")) {
            RegWnd* self = (RegWnd*)param;
            self->onClose();
            ::PostMessage(hWnd, WM_CLOSE, 0, 0);
        } else if (0 == strcmp(request, "drag")) {
            ::ReleaseCapture();
            ::PostMessage(hWnd, WM_SYSCOMMAND, SC_MOVE | HTCAPTION, 0);
        } else {
            std::string requestStr(request);
            size_t pos = requestStr.find("createRequestCode");
            if (-1 != pos) {
                std::string requestInfo = requestStr.substr(pos);
                if (requestInfo.empty())
                    return;
                const char* requestCode = mbUtilCreateRequestCode(requestInfo.c_str());
                mbResponseQuery(webView, queryId, 0, requestCode);
            }
        }
    }

    static RegWnd* m_inst;
    bool m_isShow;
    mbWebView m_mbView;

};

RegWnd* RegWnd::m_inst = nullptr;

#endif // RegWnd_h