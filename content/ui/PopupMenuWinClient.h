
#ifndef content_browser_PopupMenuWinClient_h
#define content_browser_PopupMenuWinClient_h

namespace content {

class PopupMenuWinClient {
public:
    virtual void onPopupMenuCreate(HWND hWnd) = 0;
    virtual void onPopupMenuHide() = 0;
};

}

#endif // content_browser_PopupMenuWinClient_h