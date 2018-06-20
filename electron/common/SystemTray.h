// TrayIcon.h: interface for the SystemTray class.
//
// Written by Chris Maunder (cmaunder@mail.com)
// Copyright (c) 1998.
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then acknowledgement must be made to the author of this file 
// (in whatever form you wish).
//
// This file is provided "as is" with no expressed or implied warranty.
//
// Expect bugs.
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file. 
//
//////////////////////////////////////////////////////////////////////

#if !defined(common_SystemTray_h)
#define common_SystemTray_h

#include <windows.h>
#include <ShellAPI.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// The debugger can't handle symbols more than 255 characters long.
// STL often creates symbols longer than that.
// When symbols are longer than 255 characters, the warning is disabled.
#pragma warning(disable:4786) 
#include <time.h>
#include <vector>

typedef std::vector<HICON> ICONVECTOR;

class SystemTray {
// Construction/destruction
public:
    SystemTray();
    SystemTray(HINSTANCE hInst, HWND hParent, UINT uCallbackMessage, 
              LPCTSTR szTip, HICON icon, UINT uID, 
              BOOL bhidden = FALSE,
              LPCTSTR szBalloonTip = NULL, LPCTSTR szBalloonTitle = NULL, 
              DWORD dwBalloonIcon = NIIF_NONE, UINT uBalloonTimeout = 10);
    virtual ~SystemTray();

// Operations
public:
    BOOL enabled() { return m_bEnabled; }
    BOOL visible() { return !m_bHidden; }

    // create the tray icon
    BOOL create(HINSTANCE hInst, HWND hParent, UINT uCallbackMessage, LPCTSTR szTip,
		   HICON icon, UINT uID, BOOL bHidden = FALSE,
           LPCTSTR szBalloonTip = NULL, LPCTSTR szBalloonTitle = NULL, 
           DWORD dwBalloonIcon = NIIF_NONE, UINT uBalloonTimeout = 10);

    // Change or retrieve the Tooltip text
    BOOL   setTooltipText(LPCTSTR pszTooltipText);
    BOOL   setTooltipText(UINT nID);
    LPTSTR getTooltipText() const;

    // Change or retrieve the icon displayed
    BOOL  setIcon(HICON hIcon);
    BOOL  setIcon(LPCTSTR lpszIconName);
    BOOL  setIcon(UINT nIDResource);
    BOOL  setStandardIcon(LPCTSTR lpIconName);
    BOOL  setStandardIcon(UINT nIDResource);
    HICON getIcon() const;

    void  setFocus();
    BOOL  hideIcon();
    BOOL  showIcon();
    BOOL  addIcon();
    BOOL  removeIcon();
    BOOL  moveToRight();

    BOOL showBalloon(LPCTSTR szText, LPCTSTR szTitle = NULL,
                     DWORD dwIcon = NIIF_NONE, UINT uTimeout = 10);

    // For icon animation
    BOOL  setIconList(UINT uFirstIconID, UINT uLastIconID); 
    BOOL  setIconList(HICON* pHIconList, UINT nNumIcons); 
    BOOL  animate(UINT nDelayMilliSeconds, int nNumSeconds = -1);
    BOOL  stepAnimation();
    BOOL  stopAnimation();

    // Change menu default item
    void  getMenuDefaultItem(UINT& uItem, BOOL& bByPos);
    BOOL  setMenuDefaultItem(UINT uItem, BOOL bByPos);

    // Change or retrieve the window to send icon notification messages to
    BOOL  setNotificationWnd(HWND hNotifyWnd);
    HWND  getNotificationWnd() const;

    // Change or retrieve the window to send menu commands to
    BOOL  setTargetWnd(HWND hTargetWnd);
    HWND  getTargetWnd() const;

    // Change or retrieve  notification messages sent to the window
    BOOL  setCallbackMessage(UINT uCallbackMessage);
    UINT  getCallbackMessage() const;

    HWND  getSafeHwnd() const  { return (this)? m_hWnd : NULL; }
    UINT_PTR getTimerID() const   { return m_nTimerID; }

	// Static functions
public:
    static void minimiseToTray(HWND hWnd);
    static void maximiseFromTray(HWND hWnd);

public:
    // Default handler for tray notification message
    virtual LRESULT OnTrayNotification(WPARAM uID, LPARAM lEvent);

public:
    static LRESULT PASCAL WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    static SystemTray* m_pThis;

// Implementation
protected:
    void initialise();
    void installIconPending();
    ATOM registerClass(HINSTANCE hInstance);

// Implementation
protected:
    NOTIFYICONDATA  m_tnd;
    HINSTANCE       m_hInstance;
    HWND            m_hWnd;
    HWND            m_hTargetWnd;       // Window that menu commands are sent

    BOOL            m_bEnabled;         // does O/S support tray icon?
    BOOL            m_bHidden;          // Has the icon been hidden?
    BOOL            m_bRemoved;         // Has the icon been removed?
    BOOL            m_bShowIconPending; // Show the icon once tha taskbar has been created
    BOOL            m_bWin2K;           // Use new W2K features?

    ICONVECTOR      m_IconList; 
    UINT_PTR        m_uIDTimer;
    int				m_nCurrentIcon;
    time_t    m_StartTime;
    int				m_nAnimationPeriod;
    HICON			m_hSavedIcon;
    UINT			m_DefaultMenuItemID;
    BOOL			m_DefaultMenuItemByPos;
    UINT			m_uCreationFlags;

// Static data
protected:
    static BOOL RemoveTaskbarIcon(HWND hWnd);

    static const UINT_PTR m_nTimerID;
    static UINT  m_nMaxTooltipLength;
    static UINT m_nTaskbarCreatedMsg;
    static HWND  m_hWndInvisible;

    static BOOL GetW2K();
#ifndef _WIN32_WCE
    static void GetTrayWndRect(LPRECT lprect);
    static BOOL GetDoWndAnimation();
#endif

// message map functions
public:
    LRESULT OnTimer(UINT nIDEvent);
    LRESULT OnTaskbarCreated(WPARAM wParam, LPARAM lParam);
#ifndef _WIN32_WCE
    LRESULT OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
#endif
};


#endif // !defined(common_SystemTray_h)
