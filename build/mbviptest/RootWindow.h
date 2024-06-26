#ifndef RootWindow_h
#define RootWindow_h

#include "G:/mycode/mbvip/mtmb/core/mb.h"

#define IDC_NAV_BACK                    200
#define IDC_NAV_FORWARD                 201
#define IDC_NAV_RELOAD                  202
#define IDC_NAV_STOP                    203
#define IDC_NAV_TEST                    204

const int WM_EXIT_MB_LOOP = (WM_APP + 0x13543);
class HideWndHelp;
extern int g_quitCount;

class RootWindow {
public:
    const static int buttonWidth = 72;
    const static int urlbarHeight = 24;
    const static int fontHeight = 14;

    static int m_globalCount;

    RootWindow();
    ~RootWindow();

    bool onCommand(UINT id);
    void onMoreCommand();
    void onPaint();
    void onSize(bool minimized);
    void onShowDevtools();
    void onFocus();
    void onWheel(WPARAM wParam, LPARAM lParam);
    void onTimer();
    void onDestroyed();

    mbWebView createRootWindow();

    void setMBView(mbWebView mbView) { m_mbView = mbView; }
    mbWebView getMBView() const { return m_mbView; }

    HWND getHwnd() const { return m_hWnd; }

    void createChildControl(HWND parentHwnd);

    static LRESULT CALLBACK editWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    friend class RegWnd;
    LRESULT hideWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    void initSettings();

    static void MB_CALL_TYPE onUrlChangedCallback(mbWebView webView, void* param, const utf8* url, BOOL canGoBack, BOOL canGoForward);
    static void MB_CALL_TYPE onCanGoForwardCallback(mbWebView webView, void* param, MbAsynRequestState state, BOOL b);
    static void MB_CALL_TYPE onCanGoBackCallback(mbWebView webView, void* param, MbAsynRequestState state, BOOL b);
    static mbWebView MB_CALL_TYPE onCreateView(mbWebView parentWebviwe, void* param, mbNavigationType navType, const utf8* url, const mbWindowFeatures* features);
    static void MB_CALL_TYPE onDocumentReady(mbWebView webView, void* param, mbWebFrameHandle frameId);
    static void MB_CALL_TYPE onLoadingFinish(mbWebView webView, void* param, mbWebFrameHandle frameId, const utf8* url, mbLoadingResult result, const utf8* failedReason);
    //std::string* m_initUrl;

    HWND m_hWnd;
    mbWebView m_mbView;

    HFONT m_font;
    HWND m_backHwnd;
    HWND m_forwardHwnd;
    HWND m_reloadHwnd;
    HWND m_stopHwnd;
    HWND m_testHwnd;
    HWND m_editHwnd;
    HBRUSH m_hbrush;

    HMENU m_hMenu;
    HideWndHelp* m_hideWndHelp;
};

#endif // RootWindow_h