// TrayIcon.cpp: implementation of the SystemTray class.
//
// NON-MFC VERSION
//
// This class is a light wrapper around the windows system tray stuff. It
// adds an icon to the system tray with the specified ToolTip text and 
// callback notification value, which is sent back to the Parent window.
//
// Updated: 21 Sep 2000 - Added GetDoWndAnimation - animation only occurs if the system
//                        settings allow it (Matthew Ellis). Updated the GetTrayWndRect
//                        function to include more fallback logic (Matthew Ellis)
//
// Updated: 4 Aug 2003 - Fixed bug that was stopping icon from being recreated when
//                       Explorer crashed
//                       Fixed resource leak in setIcon
//						 animate() now checks for empty icon list - Anton Treskunov
//						 Added the virutal CustomizeMenu() method - Anton Treskunov
//
// Written by Chris Maunder (cmaunder@mail.com)
// Copyright (c) 1999-2003.
//
/////////////////////////////////////////////////////////////////////////////

#include "SystemTray.h"

#ifndef ASSERT
#include <assert.h>
#define ASSERT assert
#endif

#ifndef _countof
#define _countof(x) (sizeof(x)/sizeof(x[0]))
#endif

#undef max
#undef min

#include <algorithm>

using std::max;
using std::min;

#include <Unknwn.h>
#include <gdiplus.h>

#define TRAYICON_CLASS L"TrayIconClass"

// The option here is to maintain a list of all TrayIcon windows,
// and iterate through them, instead of only allowing a single 
// TrayIcon per application
SystemTray* SystemTray::m_pThis = NULL;

const UINT SystemTray::m_nTimerID = 4567;
UINT SystemTray::m_nMaxTooltipLength = 64;     // This may change...
UINT SystemTray::m_nTaskbarCreatedMsg = 0;
HWND  SystemTray::m_hWndInvisible;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SystemTray::SystemTray() {
    if (!m_nTaskbarCreatedMsg)
        m_nTaskbarCreatedMsg = ::RegisterWindowMessage(L"TaskbarCreated");

    initialise();
}

SystemTray::SystemTray(HINSTANCE hInst,			// Handle to application instance
    HWND hParent,				// The window that will recieve tray notifications
    UINT uCallbackMessage,     // the callback message to send to parent
    LPCTSTR szToolTip,         // tray icon tooltip
    HICON icon,                // Handle to icon
    UINT uID,                  // Identifier of tray icon
    BOOL bHidden /*=FALSE*/,   // Hidden on creation?                  
    LPCTSTR szBalloonTip /*=NULL*/,    // Ballon tip (w2k only)
    LPCTSTR szBalloonTitle /*=NULL*/,  // Balloon tip title (w2k)
    DWORD dwBalloonIcon /*=NIIF_NONE*/,// Ballon tip icon (w2k)
    UINT uBalloonTimeout /*=10*/)      // Balloon timeout (w2k)
{
    initialise();
    create(hInst, hParent, uCallbackMessage, szToolTip, icon, uID, bHidden,
        szBalloonTip, szBalloonTitle, dwBalloonIcon, uBalloonTimeout);
}

void SystemTray::initialise() {
    // If maintaining a list of all TrayIcon windows (instead of
    // only allowing a single TrayIcon per application) then add
    // this TrayIcon to the list
    m_pThis = this;

    memset(&m_tnd, 0, sizeof(m_tnd));
    m_bEnabled = FALSE;
    m_bHidden = TRUE;
    m_bRemoved = TRUE;

    m_DefaultMenuItemID = 0;
    m_DefaultMenuItemByPos = TRUE;

    m_bShowIconPending = FALSE;

    m_uIDTimer = 0;
    m_hSavedIcon = NULL;

    m_hTargetWnd = NULL;
    m_uCreationFlags = 0;

    m_bWin2K = FALSE;
}

ATOM SystemTray::registerClass(HINSTANCE hInstance) {
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc = (WNDPROC)WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = 0;
    wcex.hCursor = 0;
    wcex.hbrBackground = 0;
    wcex.lpszMenuName = 0;
    wcex.lpszClassName = TRAYICON_CLASS;
    wcex.hIconSm = 0;

    return RegisterClassEx(&wcex);
}

BOOL SystemTray::create(HINSTANCE hInst, HWND hParent, UINT uCallbackMessage,
    LPCTSTR szToolTip, HICON icon, UINT uID, BOOL bHidden /*=FALSE*/,
    LPCTSTR szBalloonTip /*=NULL*/,
    LPCTSTR szBalloonTitle /*=NULL*/,
    DWORD dwBalloonIcon /*=NIIF_NONE*/,
    UINT uBalloonTimeout /*=10*/) {

    // this is only for Windows 95 (or higher)
    m_bEnabled = (GetVersion() & 0xff) >= 4;
    if (!m_bEnabled) {
        ASSERT(FALSE);
        return FALSE;
    }

    m_nMaxTooltipLength = _countof(m_tnd.szTip);

    // Make sure we avoid conflict with other messages
    ASSERT(uCallbackMessage >= WM_APP);

    // Tray only supports tooltip text up to m_nMaxTooltipLength) characters
    ASSERT(wcslen(szToolTip) <= m_nMaxTooltipLength);

    m_hInstance = hInst;

    registerClass(hInst);

    // create an invisible window
    m_hWnd = ::CreateWindowW(TRAYICON_CLASS, L"", WS_POPUP,
        CW_USEDEFAULT, CW_USEDEFAULT,
        CW_USEDEFAULT, CW_USEDEFAULT,
        NULL, 0,
        hInst, 0);

    // load up the NOTIFYICONDATA structure
    m_tnd.cbSize = sizeof(NOTIFYICONDATA);
    m_tnd.hWnd = (hParent) ? hParent : m_hWnd;
    m_tnd.uID = uID;
    m_tnd.hIcon = icon;
    m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
    m_tnd.uCallbackMessage = uCallbackMessage;

    wcsncpy(m_tnd.szTip, szToolTip, m_nMaxTooltipLength);

    m_bHidden = bHidden;
    m_hTargetWnd = m_tnd.hWnd;

    m_uCreationFlags = m_tnd.uFlags;	// Store in case we need to recreate in OnTaskBarCreate

    BOOL bResult = TRUE;
    if (!m_bHidden || m_bWin2K) {
        bResult = Shell_NotifyIcon(NIM_ADD, &m_tnd);
        m_bShowIconPending = m_bHidden = m_bRemoved = !bResult;
    }

    return bResult;
}

SystemTray::~SystemTray() {
    removeIcon();
    m_IconList.clear();
    if (m_hWnd)
        ::DestroyWindow(m_hWnd);
}

/////////////////////////////////////////////////////////////////////////////
// SystemTray icon manipulation

void SystemTray::setFocus() {

}

BOOL SystemTray::moveToRight() {
    removeIcon();
    return addIcon();
}

BOOL SystemTray::addIcon() {
    if (!m_bRemoved)
        removeIcon();

    if (m_bEnabled) {
        m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
        if (!Shell_NotifyIcon(NIM_ADD, &m_tnd))
            m_bShowIconPending = TRUE;
        else
            m_bRemoved = m_bHidden = FALSE;
    }
    return (m_bRemoved == FALSE);
}

BOOL SystemTray::removeIcon() {
    m_bShowIconPending = FALSE;

    if (!m_bEnabled || m_bRemoved)
        return TRUE;

    m_tnd.uFlags = 0;
    if (Shell_NotifyIcon(NIM_DELETE, &m_tnd))
        m_bRemoved = m_bHidden = TRUE;

    return (m_bRemoved == TRUE);
}

BOOL SystemTray::hideIcon() {
    if (!m_bEnabled || m_bRemoved || m_bHidden)
        return TRUE;

    removeIcon();

    return (m_bHidden == TRUE);
}

BOOL SystemTray::showIcon() {
    if (m_bRemoved)
        return addIcon();

    if (!m_bHidden)
        return TRUE;

    addIcon();

    return (m_bHidden == FALSE);
}

BOOL SystemTray::setIcon(HICON hIcon) {
    if (!m_bEnabled)
        return FALSE;

    m_tnd.uFlags = NIF_ICON;
    m_tnd.hIcon = hIcon;

    if (m_bHidden)
        return TRUE;
    else
        return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

BOOL SystemTray::setIcon(LPCTSTR lpszIconName) {

    HICON hIcon = nullptr;
    Gdiplus::Bitmap* gdipBitmap = Gdiplus::Bitmap::FromFile(lpszIconName, false);
    if (gdipBitmap)
        gdipBitmap->GetHICON(&hIcon);

    if (!hIcon)
        return FALSE;
    BOOL returnCode = setIcon(hIcon);
    ::DestroyIcon(hIcon);
    delete gdipBitmap;
    return returnCode;
}

BOOL SystemTray::setIcon(UINT nIDResource) {
    HICON hIcon = (HICON) ::LoadImage(m_hInstance,
        MAKEINTRESOURCE(nIDResource),
        IMAGE_ICON,
        0, 0,
        LR_DEFAULTCOLOR);

    BOOL returnCode = setIcon(hIcon);
    ::DestroyIcon(hIcon);
    return returnCode;
}

BOOL SystemTray::setStandardIcon(LPCTSTR lpIconName) {
    HICON hIcon = ::LoadIcon(NULL, lpIconName);

    return setIcon(hIcon);
}

BOOL SystemTray::setStandardIcon(UINT nIDResource) {
    HICON hIcon = ::LoadIcon(NULL, MAKEINTRESOURCE(nIDResource));

    return setIcon(hIcon);
}

HICON SystemTray::getIcon() const {
    return (m_bEnabled) ? m_tnd.hIcon : NULL;
}

BOOL SystemTray::setIconList(UINT uFirstIconID, UINT uLastIconID) {
    if (uFirstIconID > uLastIconID)
        return FALSE;

    UINT uIconArraySize = uLastIconID - uFirstIconID + 1;

    m_IconList.clear();
    try {
        for (UINT i = uFirstIconID; i <= uLastIconID; i++)
            m_IconList.push_back(::LoadIcon(m_hInstance, MAKEINTRESOURCE(i)));
    } catch (...) {
        m_IconList.clear();
        return FALSE;
    }

    return TRUE;
}

BOOL SystemTray::setIconList(HICON* pHIconList, UINT nNumIcons) {
    m_IconList.clear();

    try {
        for (UINT i = 0; i <= nNumIcons; i++)
            m_IconList.push_back(pHIconList[i]);
    } catch (...) {
        m_IconList.clear();
        return FALSE;
    }

    return TRUE;
}

BOOL SystemTray::animate(UINT nDelayMilliSeconds, int nNumSeconds /*=-1*/) {
    if (m_IconList.empty())
        return FALSE;

    stopAnimation();

    m_nCurrentIcon = 0;
    time(&m_StartTime);
    m_nAnimationPeriod = nNumSeconds;
    m_hSavedIcon = getIcon();

    // Setup a timer for the animation
    m_uIDTimer = ::SetTimer(m_hWnd, m_nTimerID, nDelayMilliSeconds, NULL);
    return (m_uIDTimer != 0);
}

BOOL SystemTray::stepAnimation() {
    if (!m_IconList.size())
        return FALSE;

    m_nCurrentIcon++;
    if (m_nCurrentIcon >= (int)m_IconList.size())
        m_nCurrentIcon = 0;

    return setIcon(m_IconList[m_nCurrentIcon]);
}

BOOL SystemTray::stopAnimation() {
    BOOL bResult = FALSE;

    if (m_uIDTimer)
        bResult = ::KillTimer(m_hWnd, m_uIDTimer);
    m_uIDTimer = 0;

    if (m_hSavedIcon)
        setIcon(m_hSavedIcon);
    m_hSavedIcon = NULL;

    return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// SystemTray tooltip text manipulation

BOOL SystemTray::setTooltipText(LPCTSTR pszTip) {
    ASSERT(wcslen(pszTip) < m_nMaxTooltipLength);

    if (!m_bEnabled)
        return FALSE;

    m_tnd.uFlags = NIF_TIP;
    wcsncpy(m_tnd.szTip, pszTip, m_nMaxTooltipLength - 1);

    if (m_bHidden)
        return TRUE;
    else
        return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

BOOL SystemTray::setTooltipText(UINT nID) {
    TCHAR strBuffer[1024];
    ASSERT(1024 >= m_nMaxTooltipLength);

    if (!LoadString(m_hInstance, nID, strBuffer, m_nMaxTooltipLength - 1))
        return FALSE;

    return setTooltipText(strBuffer);
}

LPTSTR SystemTray::getTooltipText() const {
    if (!m_bEnabled)
        return FALSE;

    static WCHAR strBuffer[1024];
    ASSERT(1024 >= m_nMaxTooltipLength);

    wcsncpy(strBuffer, m_tnd.szTip, m_nMaxTooltipLength - 1);

    return strBuffer;
}

//////////////////////////////////////////////////////////////////////////
//
// Function:    showBalloon
//
// Description:
//  Shows a balloon tooltip over the tray icon.
//
// Input:
//  szText: [in] Text for the balloon tooltip.
//  szTitle: [in] Title for the balloon.  This text is shown in bold above
//           the tooltip text (szText).  Pass "" if you don't want a title.
//  dwIcon: [in] Specifies an icon to appear in the balloon.  Legal values are:
//                 NIIF_NONE: No icon
//                 NIIF_INFO: Information
//                 NIIF_WARNING: Exclamation
//                 NIIF_ERROR: Critical error (red circle with X)
//  uTimeout: [in] Number of seconds for the balloon to remain visible.  Can
//            be between 10 and 30 inclusive.
//
// Returns:
//  TRUE if successful, FALSE if not.
//
//////////////////////////////////////////////////////////////////////////
// Added by Michael Dunn, November 1999
//////////////////////////////////////////////////////////////////////////

BOOL SystemTray::showBalloon(LPCTSTR szText,
    LPCTSTR szTitle  /*=NULL*/,
    DWORD   dwIcon   /*=NIIF_NONE*/,
    UINT    uTimeout /*=10*/) {
    m_tnd.uFlags = NIF_INFO;
//     m_tnd.dwInfoFlags = NIIF_USER | NIIF_LARGE_ICON;
    wcsncpy(m_tnd.szInfoTitle, szTitle, wcslen(szTitle));
    wcsncpy(m_tnd.szInfo, szText, wcslen(szText));
    //LoadIconMetric(g_hInst, MAKEINTRESOURCE(dwIcon), LIM_LARGE, &nid.hBalloonIcon);
    Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
    Shell_NotifyIcon(NIN_BALLOONSHOW, &m_tnd);
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// SystemTray notification window stuff

BOOL SystemTray::setNotificationWnd(HWND hNotifyWnd) {
    if (!m_bEnabled)
        return FALSE;

    // Make sure Notification window is valid
    if (!hNotifyWnd || !::IsWindow(hNotifyWnd)) {
        ASSERT(FALSE);
        return FALSE;
    }

    m_tnd.hWnd = hNotifyWnd;
    m_tnd.uFlags = 0;

    if (m_bHidden)
        return TRUE;
    else
        return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

HWND SystemTray::getNotificationWnd() const {
    return m_tnd.hWnd;
}

// Hatr added

// Change or retrive the window to send menu commands to
BOOL SystemTray::setTargetWnd(HWND hTargetWnd) {
    m_hTargetWnd = hTargetWnd;
    return TRUE;
} // SystemTray::setTargetWnd()

HWND SystemTray::getTargetWnd() const {
    if (m_hTargetWnd)
        return m_hTargetWnd;
    else
        return m_tnd.hWnd;
} // SystemTray::getTargetWnd()

  /////////////////////////////////////////////////////////////////////////////
  // SystemTray notification message stuff

BOOL SystemTray::setCallbackMessage(UINT uCallbackMessage) {
    if (!m_bEnabled)
        return FALSE;

    // Make sure we avoid conflict with other messages
    ASSERT(uCallbackMessage >= WM_APP);

    m_tnd.uCallbackMessage = uCallbackMessage;
    m_tnd.uFlags = NIF_MESSAGE;

    if (m_bHidden)
        return TRUE;
    else
        return Shell_NotifyIcon(NIM_MODIFY, &m_tnd);
}

UINT SystemTray::getCallbackMessage() const {
    return m_tnd.uCallbackMessage;
}

/////////////////////////////////////////////////////////////////////////////
// SystemTray menu manipulation

BOOL SystemTray::setMenuDefaultItem(UINT uItem, BOOL bByPos) {
    if ((m_DefaultMenuItemID == uItem) && (m_DefaultMenuItemByPos == bByPos))
        return TRUE;

    m_DefaultMenuItemID = uItem;
    m_DefaultMenuItemByPos = bByPos;

    HMENU hMenu = ::LoadMenu(m_hInstance, MAKEINTRESOURCE(m_tnd.uID));
    if (!hMenu)
        return FALSE;

    HMENU hSubMenu = ::GetSubMenu(hMenu, 0);
    if (!hSubMenu) {
        ::DestroyMenu(hMenu);
        return FALSE;
    }

    ::SetMenuDefaultItem(hSubMenu, m_DefaultMenuItemID, m_DefaultMenuItemByPos);

    ::DestroyMenu(hSubMenu);
    ::DestroyMenu(hMenu);

    return TRUE;
}

void SystemTray::getMenuDefaultItem(UINT& uItem, BOOL& bByPos) {
    uItem = m_DefaultMenuItemID;
    bByPos = m_DefaultMenuItemByPos;
}

/////////////////////////////////////////////////////////////////////////////
// SystemTray message handlers

LRESULT SystemTray::OnTimer(UINT nIDEvent) {
    if (nIDEvent != m_uIDTimer) {
        ASSERT(FALSE);
        return 0L;
    }

    time_t CurrentTime;
    time(&CurrentTime);

    time_t period = CurrentTime - m_StartTime;
    if (m_nAnimationPeriod > 0 && m_nAnimationPeriod < period) {
        stopAnimation();
        return 0L;
    }

    stepAnimation();

    return 0L;
}

// This is called whenever the taskbar is created (eg after explorer crashes
// and restarts. Please note that the WM_TASKBARCREATED message is only passed
// to TOP LEVEL windows (like WM_QUERYNEWPALETTE)
LRESULT SystemTray::OnTaskbarCreated(WPARAM wParam, LPARAM lParam) {
    installIconPending();
    return 0L;
}


LRESULT SystemTray::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) {
    if (uFlags == SPI_SETWORKAREA)
        installIconPending();
    return 0L;
}

LRESULT SystemTray::OnTrayNotification(UINT wParam, LONG lParam) {
    //Return quickly if its not for this tray icon
    if (wParam != m_tnd.uID)
        return 0L;

    HWND hTargetWnd = getTargetWnd();
    if (!hTargetWnd)
        return 0L;

    // Clicking with right button brings up a context menu

    if (LOWORD(lParam) == WM_RBUTTONUP) {
        HMENU hMenu = ::LoadMenu(m_hInstance, MAKEINTRESOURCE(m_tnd.uID));
        if (!hMenu)
            return 0;

        HMENU hSubMenu = ::GetSubMenu(hMenu, 0);
        if (!hSubMenu) {
            ::DestroyMenu(hMenu);        //Be sure to Destroy Menu Before Returning
            return 0;
        }

        // Make chosen menu item the default (bold font)
        ::SetMenuDefaultItem(hSubMenu, m_DefaultMenuItemID, m_DefaultMenuItemByPos);

        // Display and track the popup menu
        POINT pos;
        GetCursorPos(&pos);

        ::SetForegroundWindow(m_tnd.hWnd);
        ::TrackPopupMenu(hSubMenu, 0, pos.x, pos.y, 0, hTargetWnd, NULL);

        // BUGFIX: See "PRB: Menus for Notification Icons Don't Work Correctly"
        ::PostMessage(m_tnd.hWnd, WM_NULL, 0, 0);

        DestroyMenu(hMenu);
    } else if (LOWORD(lParam) == WM_LBUTTONDBLCLK) {
        // double click received, the default action is to execute default menu item
        ::SetForegroundWindow(m_tnd.hWnd);

        UINT uItem;
        if (m_DefaultMenuItemByPos) {
            HMENU hMenu = ::LoadMenu(m_hInstance, MAKEINTRESOURCE(m_tnd.uID));
            if (!hMenu)
                return 0;

            HMENU hSubMenu = ::GetSubMenu(hMenu, 0);
            if (!hSubMenu)
                return 0;
            uItem = ::GetMenuItemID(hSubMenu, m_DefaultMenuItemID);

            DestroyMenu(hMenu);
        } else
            uItem = m_DefaultMenuItemID;

        ::PostMessage(hTargetWnd, WM_COMMAND, uItem, 0);
    }

    return 1;
}

// This is the global (static) callback function for all TrayIcon windows
LRESULT PASCAL SystemTray::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    // The option here is to maintain a list of all TrayIcon windows,
    // and iterate through them. If you do this, remove these 3 lines.
    SystemTray* pTrayIcon = m_pThis;
    if (pTrayIcon->getSafeHwnd() != hWnd)
        return ::DefWindowProc(hWnd, message, wParam, lParam);

    // If maintaining a list of TrayIcon windows, then the following...
    // pTrayIcon = GetFirstTrayIcon()
    // while (pTrayIcon != NULL)
    // {
    //    if (pTrayIcon->getSafeHwnd() != hWnd) continue;

    // Taskbar has been recreated - all TrayIcons must process this.
    if (message == SystemTray::m_nTaskbarCreatedMsg)
        return pTrayIcon->OnTaskbarCreated(wParam, lParam);

    // Animation timer
    if (message == WM_TIMER && wParam == pTrayIcon->getTimerID())
        return pTrayIcon->OnTimer(wParam);

    // Settings changed
    if (message == WM_SETTINGCHANGE && wParam == pTrayIcon->getTimerID())
        return pTrayIcon->OnSettingChange(wParam, (LPCTSTR)lParam);

    // Is the message from the icon for this TrayIcon?
    if (message == pTrayIcon->getCallbackMessage())
        return pTrayIcon->OnTrayNotification(wParam, lParam);

    //    pTrayIcon = GetNextTrayIcon();
    // }

    // Message has not been processed, so default.
    return ::DefWindowProc(hWnd, message, wParam, lParam);
}

void SystemTray::installIconPending() {
    // Is the icon display pending, and it's not been set as "hidden"?
    if (!m_bShowIconPending || m_bHidden)
        return;

    // Reset the flags to what was used at creation
    m_tnd.uFlags = m_uCreationFlags;

    // Try and recreate the icon
    m_bHidden = !Shell_NotifyIcon(NIM_ADD, &m_tnd);

    // If it's STILL hidden, then have another go next time...
    m_bShowIconPending = !m_bHidden;

    ASSERT(m_bHidden == FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// For minimising/maximising from system tray

BOOL CALLBACK FindTrayWnd(HWND hwnd, LPARAM lParam) {
    TCHAR szClassName[256];
    GetClassName(hwnd, szClassName, 255);

    // Did we find the Main System Tray? If so, then get its size and keep going
    if (wcscmp(szClassName, L"TrayNotifyWnd") == 0) {
        LPRECT lpRect = (LPRECT)lParam;
        ::GetWindowRect(hwnd, lpRect);
        return TRUE;
    }

    // Did we find the System Clock? If so, then adjust the size of the rectangle
    // we have and quit (clock will be found after the system tray)
    if (wcscmp(szClassName, L"TrayClockWClass") == 0) {
        LPRECT lpRect = (LPRECT)lParam;
        RECT rectClock;
        ::GetWindowRect(hwnd, &rectClock);
        // if clock is above system tray adjust accordingly
        if (rectClock.bottom < lpRect->bottom - 5) // 10 = random fudge factor.
            lpRect->top = rectClock.bottom;
        else
            lpRect->right = rectClock.left;
        return FALSE;
    }

    return TRUE;
}

void SystemTray::GetTrayWndRect(LPRECT lprect) {
#define DEFAULT_RECT_WIDTH 150
#define DEFAULT_RECT_HEIGHT 30

    HWND hShellTrayWnd = FindWindow(L"Shell_TrayWnd", NULL);
    if (hShellTrayWnd) {
        GetWindowRect(hShellTrayWnd, lprect);
        EnumChildWindows(hShellTrayWnd, FindTrayWnd, (LPARAM)lprect);
        return;
    }
    // OK, we failed to get the rect from the quick hack. Either explorer isn't
    // running or it's a new version of the shell with the window class names
    // changed (how dare Microsoft change these undocumented class names!) So, we
    // try to find out what side of the screen the taskbar is connected to. We
    // know that the system tray is either on the right or the bottom of the
    // taskbar, so we can make a good guess at where to minimize to
    APPBARDATA appBarData;
    appBarData.cbSize = sizeof(appBarData);
    if (SHAppBarMessage(ABM_GETTASKBARPOS, &appBarData)) {
        // We know the edge the taskbar is connected to, so guess the rect of the
        // system tray. Use various fudge factor to make it look good
        switch (appBarData.uEdge) {
        case ABE_LEFT:
        case ABE_RIGHT:
            // We want to minimize to the bottom of the taskbar
            lprect->top = appBarData.rc.bottom - 100;
            lprect->bottom = appBarData.rc.bottom - 16;
            lprect->left = appBarData.rc.left;
            lprect->right = appBarData.rc.right;
            break;

        case ABE_TOP:
        case ABE_BOTTOM:
            // We want to minimize to the right of the taskbar
            lprect->top = appBarData.rc.top;
            lprect->bottom = appBarData.rc.bottom;
            lprect->left = appBarData.rc.right - 100;
            lprect->right = appBarData.rc.right - 16;
            break;
        }
        return;
    }

    // Blimey, we really aren't in luck. It's possible that a third party shell
    // is running instead of explorer. This shell might provide support for the
    // system tray, by providing a Shell_TrayWnd window (which receives the
    // messages for the icons) So, look for a Shell_TrayWnd window and work out
    // the rect from that. Remember that explorer's taskbar is the Shell_TrayWnd,
    // and stretches either the width or the height of the screen. We can't rely
    // on the 3rd party shell's Shell_TrayWnd doing the same, in fact, we can't
    // rely on it being any size. The best we can do is just blindly use the
    // window rect, perhaps limiting the width and height to, say 150 square.
    // Note that if the 3rd party shell supports the same configuraion as
    // explorer (the icons hosted in NotifyTrayWnd, which is a child window of
    // Shell_TrayWnd), we would already have caught it above
    if (hShellTrayWnd) {
        ::GetWindowRect(hShellTrayWnd, lprect);
        if (lprect->right - lprect->left > DEFAULT_RECT_WIDTH)
            lprect->left = lprect->right - DEFAULT_RECT_WIDTH;
        if (lprect->bottom - lprect->top > DEFAULT_RECT_HEIGHT)
            lprect->top = lprect->bottom - DEFAULT_RECT_HEIGHT;

        return;
    }

    // OK. Haven't found a thing. Provide a default rect based on the current work
    // area
    SystemParametersInfo(SPI_GETWORKAREA, 0, lprect, 0);
    lprect->left = lprect->right - DEFAULT_RECT_WIDTH;
    lprect->top = lprect->bottom - DEFAULT_RECT_HEIGHT;
}

// Check to see if the animation has been disabled (Matthew Ellis <m.t.ellis@bigfoot.com>)
BOOL SystemTray::GetDoWndAnimation() {
    ANIMATIONINFO ai;

    ai.cbSize = sizeof(ai);
    SystemParametersInfo(SPI_GETANIMATION, sizeof(ai), &ai, 0);

    return ai.iMinAnimate ? TRUE : FALSE;
}

BOOL SystemTray::RemoveTaskbarIcon(HWND hWnd) {
    // create static invisible window
    if (!::IsWindow(m_hWndInvisible)) {
        m_hWndInvisible = CreateWindowExW(0, L"Static", L"", WS_POPUP,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            NULL, 0, NULL, 0);

        if (!m_hWndInvisible)
            return FALSE;
    }

    SetParent(hWnd, m_hWndInvisible);

    return TRUE;
}

void SystemTray::minimiseToTray(HWND hWnd) {
    if (GetDoWndAnimation()) {
        RECT rectFrom, rectTo;

        GetWindowRect(hWnd, &rectFrom);
        GetTrayWndRect(&rectTo);

        DrawAnimatedRects(hWnd, IDANI_CAPTION, &rectFrom, &rectTo);
    }

    RemoveTaskbarIcon(hWnd);
    SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) &~WS_VISIBLE);
}

void SystemTray::maximiseFromTray(HWND hWnd) {
    if (GetDoWndAnimation()) {
        RECT rectTo;
        ::GetWindowRect(hWnd, &rectTo);

        RECT rectFrom;
        GetTrayWndRect(&rectFrom);

        ::SetParent(hWnd, NULL);
        DrawAnimatedRects(hWnd, IDANI_CAPTION, &rectFrom, &rectTo);
    } else
        ::SetParent(hWnd, NULL);

    SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) | WS_VISIBLE);
    RedrawWindow(hWnd, NULL, NULL, RDW_UPDATENOW | RDW_ALLCHILDREN | RDW_FRAME |
        RDW_INVALIDATE | RDW_ERASE);

    // Move focus away and back again to ensure taskbar icon is recreated
    if (::IsWindow(m_hWndInvisible))
        SetActiveWindow(m_hWndInvisible);
    SetActiveWindow(hWnd);
    SetForegroundWindow(hWnd);
}