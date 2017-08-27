/****************************************************************************
*                                                                           *
* winuser.h -- USER procedure declarations, constant definitions and macros *
*                                                                           *
* Copyright (c) Microsoft Corporation. All rights reserved.                          *
*                                                                           *
****************************************************************************/



#ifndef _WINUSER_
#define _WINUSER_



//
// Define API decoration for direct importing of DLL references.
//

#if !defined(_USER32_)
#define WINUSERAPI DECLSPEC_IMPORT
#else
#define WINUSERAPI
#endif

#ifdef _MAC
#include <macwin32.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef WINVER
#define WINVER  0x0500      /* version 5.0 */
#endif /* !WINVER */

#include <stdarg.h>

#ifndef NOUSER

typedef HANDLE HDWP;
typedef VOID MENUTEMPLATEA;
typedef VOID MENUTEMPLATEW;
#ifdef UNICODE
typedef MENUTEMPLATEW MENUTEMPLATE;
#else
typedef MENUTEMPLATEA MENUTEMPLATE;
#endif // UNICODE
typedef PVOID LPMENUTEMPLATEA;
typedef PVOID LPMENUTEMPLATEW;
#ifdef UNICODE
typedef LPMENUTEMPLATEW LPMENUTEMPLATE;
#else
typedef LPMENUTEMPLATEA LPMENUTEMPLATE;
#endif // UNICODE

typedef LRESULT (CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

#ifdef STRICT

typedef INT_PTR (CALLBACK* DLGPROC)(HWND, UINT, WPARAM, LPARAM);
typedef VOID (CALLBACK* TIMERPROC)(HWND, UINT, UINT_PTR, DWORD);
typedef BOOL (CALLBACK* GRAYSTRINGPROC)(HDC, LPARAM, int);
typedef BOOL (CALLBACK* WNDENUMPROC)(HWND, LPARAM);
typedef LRESULT (CALLBACK* HOOKPROC)(int code, WPARAM wParam, LPARAM lParam);
typedef VOID (CALLBACK* SENDASYNCPROC)(HWND, UINT, ULONG_PTR, LRESULT);

typedef BOOL (CALLBACK* PROPENUMPROCA)(HWND, LPCSTR, HANDLE);
typedef BOOL (CALLBACK* PROPENUMPROCW)(HWND, LPCWSTR, HANDLE);

typedef BOOL (CALLBACK* PROPENUMPROCEXA)(HWND, LPSTR, HANDLE, ULONG_PTR);
typedef BOOL (CALLBACK* PROPENUMPROCEXW)(HWND, LPWSTR, HANDLE, ULONG_PTR);

typedef int (CALLBACK* EDITWORDBREAKPROCA)(LPSTR lpch, int ichCurrent, int cch, int code);
typedef int (CALLBACK* EDITWORDBREAKPROCW)(LPWSTR lpch, int ichCurrent, int cch, int code);

#if(WINVER >= 0x0400)
typedef BOOL (CALLBACK* DRAWSTATEPROC)(HDC hdc, LPARAM lData, WPARAM wData, int cx, int cy);
#endif /* WINVER >= 0x0400 */
#else /* !STRICT */

typedef FARPROC DLGPROC;
typedef FARPROC TIMERPROC;
typedef FARPROC GRAYSTRINGPROC;
typedef FARPROC WNDENUMPROC;
typedef FARPROC HOOKPROC;
typedef FARPROC SENDASYNCPROC;

typedef FARPROC EDITWORDBREAKPROCA;
typedef FARPROC EDITWORDBREAKPROCW;

typedef FARPROC PROPENUMPROCA;
typedef FARPROC PROPENUMPROCW;

typedef FARPROC PROPENUMPROCEXA;
typedef FARPROC PROPENUMPROCEXW;

#if(WINVER >= 0x0400)
typedef FARPROC DRAWSTATEPROC;
#endif /* WINVER >= 0x0400 */
#endif /* !STRICT */

#ifdef UNICODE
typedef PROPENUMPROCW        PROPENUMPROC;
typedef PROPENUMPROCEXW      PROPENUMPROCEX;
typedef EDITWORDBREAKPROCW   EDITWORDBREAKPROC;
#else  /* !UNICODE */
typedef PROPENUMPROCA        PROPENUMPROC;
typedef PROPENUMPROCEXA      PROPENUMPROCEX;
typedef EDITWORDBREAKPROCA   EDITWORDBREAKPROC;
#endif /* UNICODE */

#ifdef STRICT

typedef BOOL (CALLBACK* NAMEENUMPROCA)(LPSTR, LPARAM);
typedef BOOL (CALLBACK* NAMEENUMPROCW)(LPWSTR, LPARAM);

typedef NAMEENUMPROCA   WINSTAENUMPROCA;
typedef NAMEENUMPROCA   DESKTOPENUMPROCA;
typedef NAMEENUMPROCW   WINSTAENUMPROCW;
typedef NAMEENUMPROCW   DESKTOPENUMPROCW;


#else /* !STRICT */

typedef FARPROC NAMEENUMPROCA;
typedef FARPROC NAMEENUMPROCW;
typedef FARPROC WINSTAENUMPROCA;
typedef FARPROC DESKTOPENUMPROCA;
typedef FARPROC WINSTAENUMPROCW;
typedef FARPROC DESKTOPENUMPROCW;


#endif /* !STRICT */

#ifdef UNICODE
typedef WINSTAENUMPROCW     WINSTAENUMPROC;
typedef DESKTOPENUMPROCW    DESKTOPENUMPROC;


#else  /* !UNICODE */
typedef WINSTAENUMPROCA     WINSTAENUMPROC;
typedef DESKTOPENUMPROCA    DESKTOPENUMPROC;

#endif /* UNICODE */

#define IS_INTRESOURCE(_r) ((((ULONG_PTR)(_r)) >> 16) == 0)
#define MAKEINTRESOURCEA(i) ((LPSTR)((ULONG_PTR)((WORD)(i))))
#define MAKEINTRESOURCEW(i) ((LPWSTR)((ULONG_PTR)((WORD)(i))))
#ifdef UNICODE
#define MAKEINTRESOURCE  MAKEINTRESOURCEW
#else
#define MAKEINTRESOURCE  MAKEINTRESOURCEA
#endif // !UNICODE

#ifndef NORESOURCE

/*
 * Predefined Resource Types
 */
#define RT_CURSOR           MAKEINTRESOURCE(1)
#define RT_BITMAP           MAKEINTRESOURCE(2)
#define RT_ICON             MAKEINTRESOURCE(3)
#define RT_MENU             MAKEINTRESOURCE(4)
#define RT_DIALOG           MAKEINTRESOURCE(5)
#define RT_STRING           MAKEINTRESOURCE(6)
#define RT_FONTDIR          MAKEINTRESOURCE(7)
#define RT_FONT             MAKEINTRESOURCE(8)
#define RT_ACCELERATOR      MAKEINTRESOURCE(9)
#define RT_RCDATA           MAKEINTRESOURCE(10)
#define RT_MESSAGETABLE     MAKEINTRESOURCE(11)

#define DIFFERENCE     11
#define RT_GROUP_CURSOR MAKEINTRESOURCE((ULONG_PTR)RT_CURSOR + DIFFERENCE)
#define RT_GROUP_ICON   MAKEINTRESOURCE((ULONG_PTR)RT_ICON + DIFFERENCE)
#define RT_VERSION      MAKEINTRESOURCE(16)
#define RT_DLGINCLUDE   MAKEINTRESOURCE(17)
#if(WINVER >= 0x0400)
#define RT_PLUGPLAY     MAKEINTRESOURCE(19)
#define RT_VXD          MAKEINTRESOURCE(20)
#define RT_ANICURSOR    MAKEINTRESOURCE(21)
#define RT_ANIICON      MAKEINTRESOURCE(22)
#endif /* WINVER >= 0x0400 */
#define RT_HTML         MAKEINTRESOURCE(23)
#ifdef RC_INVOKED
#define RT_MANIFEST                        24
#define CREATEPROCESS_MANIFEST_RESOURCE_ID  1
#define ISOLATIONAWARE_MANIFEST_RESOURCE_ID 2
#define ISOLATIONAWARE_NOSTATICIMPORT_MANIFEST_RESOURCE_ID 3
#define MINIMUM_RESERVED_MANIFEST_RESOURCE_ID 1   /* inclusive */
#define MAXIMUM_RESERVED_MANIFEST_RESOURCE_ID 16  /* inclusive */
#else  /* RC_INVOKED */
#define RT_MANIFEST                        MAKEINTRESOURCE(24)
#define CREATEPROCESS_MANIFEST_RESOURCE_ID MAKEINTRESOURCE( 1)
#define ISOLATIONAWARE_MANIFEST_RESOURCE_ID MAKEINTRESOURCE(2)
#define ISOLATIONAWARE_NOSTATICIMPORT_MANIFEST_RESOURCE_ID MAKEINTRESOURCE(3)
#define MINIMUM_RESERVED_MANIFEST_RESOURCE_ID MAKEINTRESOURCE( 1 /*inclusive*/)
#define MAXIMUM_RESERVED_MANIFEST_RESOURCE_ID MAKEINTRESOURCE(16 /*inclusive*/)
#endif /* RC_INVOKED */


#endif /* !NORESOURCE */

WINUSERAPI
int
WINAPI
wvsprintfA(
    __out LPSTR,
    __in __format_string LPCSTR,
    __in va_list arglist);
WINUSERAPI
int
WINAPI
wvsprintfW(
    __out LPWSTR,
    __in __format_string LPCWSTR,
    __in va_list arglist);
#ifdef UNICODE
#define wvsprintf  wvsprintfW
#else
#define wvsprintf  wvsprintfA
#endif // !UNICODE

WINUSERAPI
int
WINAPIV
wsprintfA(
    __out LPSTR,
    __in __format_string LPCSTR,
    ...);
WINUSERAPI
int
WINAPIV
wsprintfW(
    __out LPWSTR,
    __in __format_string LPCWSTR,
    ...);
#ifdef UNICODE
#define wsprintf  wsprintfW
#else
#define wsprintf  wsprintfA
#endif // !UNICODE


/*
 * SPI_SETDESKWALLPAPER defined constants
 */
#define SETWALLPAPER_DEFAULT    ((LPWSTR)-1)

#ifndef NOSCROLL

/*
 * Scroll Bar Constants
 */
#define SB_HORZ             0
#define SB_VERT             1
#define SB_CTL              2
#define SB_BOTH             3

/*
 * Scroll Bar Commands
 */
#define SB_LINEUP           0
#define SB_LINELEFT         0
#define SB_LINEDOWN         1
#define SB_LINERIGHT        1
#define SB_PAGEUP           2
#define SB_PAGELEFT         2
#define SB_PAGEDOWN         3
#define SB_PAGERIGHT        3
#define SB_THUMBPOSITION    4
#define SB_THUMBTRACK       5
#define SB_TOP              6
#define SB_LEFT             6
#define SB_BOTTOM           7
#define SB_RIGHT            7
#define SB_ENDSCROLL        8

#endif /* !NOSCROLL */

#ifndef NOSHOWWINDOW


/*
 * ShowWindow() Commands
 */
#define SW_HIDE             0
#define SW_SHOWNORMAL       1
#define SW_NORMAL           1
#define SW_SHOWMINIMIZED    2
#define SW_SHOWMAXIMIZED    3
#define SW_MAXIMIZE         3
#define SW_SHOWNOACTIVATE   4
#define SW_SHOW             5
#define SW_MINIMIZE         6
#define SW_SHOWMINNOACTIVE  7
#define SW_SHOWNA           8
#define SW_RESTORE          9
#define SW_SHOWDEFAULT      10
#define SW_FORCEMINIMIZE    11
#define SW_MAX              11

/*
 * Old ShowWindow() Commands
 */
#define HIDE_WINDOW         0
#define SHOW_OPENWINDOW     1
#define SHOW_ICONWINDOW     2
#define SHOW_FULLSCREEN     3
#define SHOW_OPENNOACTIVATE 4

/*
 * Identifiers for the WM_SHOWWINDOW message
 */
#define SW_PARENTCLOSING    1
#define SW_OTHERZOOM        2
#define SW_PARENTOPENING    3
#define SW_OTHERUNZOOM      4


#endif /* !NOSHOWWINDOW */

#if(WINVER >= 0x0500)
/*
 * AnimateWindow() Commands
 */
#define AW_HOR_POSITIVE             0x00000001
#define AW_HOR_NEGATIVE             0x00000002
#define AW_VER_POSITIVE             0x00000004
#define AW_VER_NEGATIVE             0x00000008
#define AW_CENTER                   0x00000010
#define AW_HIDE                     0x00010000
#define AW_ACTIVATE                 0x00020000
#define AW_SLIDE                    0x00040000
#define AW_BLEND                    0x00080000

#endif /* WINVER >= 0x0500 */


/*
 * WM_KEYUP/DOWN/CHAR HIWORD(lParam) flags
 */
#define KF_EXTENDED       0x0100
#define KF_DLGMODE        0x0800
#define KF_MENUMODE       0x1000
#define KF_ALTDOWN        0x2000
#define KF_REPEAT         0x4000
#define KF_UP             0x8000

#ifndef NOVIRTUALKEYCODES


/*
 * Virtual Keys, Standard Set
 */
#define VK_LBUTTON        0x01
#define VK_RBUTTON        0x02
#define VK_CANCEL         0x03
#define VK_MBUTTON        0x04    /* NOT contiguous with L & RBUTTON */

#if(_WIN32_WINNT >= 0x0500)
#define VK_XBUTTON1       0x05    /* NOT contiguous with L & RBUTTON */
#define VK_XBUTTON2       0x06    /* NOT contiguous with L & RBUTTON */
#endif /* _WIN32_WINNT >= 0x0500 */

/*
 * 0x07 : unassigned
 */

#define VK_BACK           0x08
#define VK_TAB            0x09

/*
 * 0x0A - 0x0B : reserved
 */

#define VK_CLEAR          0x0C
#define VK_RETURN         0x0D

#define VK_SHIFT          0x10
#define VK_CONTROL        0x11
#define VK_MENU           0x12
#define VK_PAUSE          0x13
#define VK_CAPITAL        0x14

#define VK_KANA           0x15
#define VK_HANGEUL        0x15  /* old name - should be here for compatibility */
#define VK_HANGUL         0x15
#define VK_JUNJA          0x17
#define VK_FINAL          0x18
#define VK_HANJA          0x19
#define VK_KANJI          0x19

#define VK_ESCAPE         0x1B

#define VK_CONVERT        0x1C
#define VK_NONCONVERT     0x1D
#define VK_ACCEPT         0x1E
#define VK_MODECHANGE     0x1F

#define VK_SPACE          0x20
#define VK_PRIOR          0x21
#define VK_NEXT           0x22
#define VK_END            0x23
#define VK_HOME           0x24
#define VK_LEFT           0x25
#define VK_UP             0x26
#define VK_RIGHT          0x27
#define VK_DOWN           0x28
#define VK_SELECT         0x29
#define VK_PRINT          0x2A
#define VK_EXECUTE        0x2B
#define VK_SNAPSHOT       0x2C
#define VK_INSERT         0x2D
#define VK_DELETE         0x2E
#define VK_HELP           0x2F

/*
 * VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
 * 0x40 : unassigned
 * VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
 */

#define VK_LWIN           0x5B
#define VK_RWIN           0x5C
#define VK_APPS           0x5D

/*
 * 0x5E : reserved
 */

#define VK_SLEEP          0x5F

#define VK_NUMPAD0        0x60
#define VK_NUMPAD1        0x61
#define VK_NUMPAD2        0x62
#define VK_NUMPAD3        0x63
#define VK_NUMPAD4        0x64
#define VK_NUMPAD5        0x65
#define VK_NUMPAD6        0x66
#define VK_NUMPAD7        0x67
#define VK_NUMPAD8        0x68
#define VK_NUMPAD9        0x69
#define VK_MULTIPLY       0x6A
#define VK_ADD            0x6B
#define VK_SEPARATOR      0x6C
#define VK_SUBTRACT       0x6D
#define VK_DECIMAL        0x6E
#define VK_DIVIDE         0x6F
#define VK_F1             0x70
#define VK_F2             0x71
#define VK_F3             0x72
#define VK_F4             0x73
#define VK_F5             0x74
#define VK_F6             0x75
#define VK_F7             0x76
#define VK_F8             0x77
#define VK_F9             0x78
#define VK_F10            0x79
#define VK_F11            0x7A
#define VK_F12            0x7B
#define VK_F13            0x7C
#define VK_F14            0x7D
#define VK_F15            0x7E
#define VK_F16            0x7F
#define VK_F17            0x80
#define VK_F18            0x81
#define VK_F19            0x82
#define VK_F20            0x83
#define VK_F21            0x84
#define VK_F22            0x85
#define VK_F23            0x86
#define VK_F24            0x87

/*
 * 0x88 - 0x8F : unassigned
 */

#define VK_NUMLOCK        0x90
#define VK_SCROLL         0x91

/*
 * NEC PC-9800 kbd definitions
 */
#define VK_OEM_NEC_EQUAL  0x92   // '=' key on numpad

/*
 * Fujitsu/OASYS kbd definitions
 */
#define VK_OEM_FJ_JISHO   0x92   // 'Dictionary' key
#define VK_OEM_FJ_MASSHOU 0x93   // 'Unregister word' key
#define VK_OEM_FJ_TOUROKU 0x94   // 'Register word' key
#define VK_OEM_FJ_LOYA    0x95   // 'Left OYAYUBI' key
#define VK_OEM_FJ_ROYA    0x96   // 'Right OYAYUBI' key

/*
 * 0x97 - 0x9F : unassigned
 */

/*
 * VK_L* & VK_R* - left and right Alt, Ctrl and Shift virtual keys.
 * Used only as parameters to GetAsyncKeyState() and GetKeyState().
 * No other API or message will distinguish left and right keys in this way.
 */
#define VK_LSHIFT         0xA0
#define VK_RSHIFT         0xA1
#define VK_LCONTROL       0xA2
#define VK_RCONTROL       0xA3
#define VK_LMENU          0xA4
#define VK_RMENU          0xA5

#if(_WIN32_WINNT >= 0x0500)
#define VK_BROWSER_BACK        0xA6
#define VK_BROWSER_FORWARD     0xA7
#define VK_BROWSER_REFRESH     0xA8
#define VK_BROWSER_STOP        0xA9
#define VK_BROWSER_SEARCH      0xAA
#define VK_BROWSER_FAVORITES   0xAB
#define VK_BROWSER_HOME        0xAC

#define VK_VOLUME_MUTE         0xAD
#define VK_VOLUME_DOWN         0xAE
#define VK_VOLUME_UP           0xAF
#define VK_MEDIA_NEXT_TRACK    0xB0
#define VK_MEDIA_PREV_TRACK    0xB1
#define VK_MEDIA_STOP          0xB2
#define VK_MEDIA_PLAY_PAUSE    0xB3
#define VK_LAUNCH_MAIL         0xB4
#define VK_LAUNCH_MEDIA_SELECT 0xB5
#define VK_LAUNCH_APP1         0xB6
#define VK_LAUNCH_APP2         0xB7

#endif /* _WIN32_WINNT >= 0x0500 */

/*
 * 0xB8 - 0xB9 : reserved
 */

#define VK_OEM_1          0xBA   // ';:' for US
#define VK_OEM_PLUS       0xBB   // '+' any country
#define VK_OEM_COMMA      0xBC   // ',' any country
#define VK_OEM_MINUS      0xBD   // '-' any country
#define VK_OEM_PERIOD     0xBE   // '.' any country
#define VK_OEM_2          0xBF   // '/?' for US
#define VK_OEM_3          0xC0   // '`~' for US

/*
 * 0xC1 - 0xD7 : reserved
 */

/*
 * 0xD8 - 0xDA : unassigned
 */

#define VK_OEM_4          0xDB  //  '[{' for US
#define VK_OEM_5          0xDC  //  '\|' for US
#define VK_OEM_6          0xDD  //  ']}' for US
#define VK_OEM_7          0xDE  //  ''"' for US
#define VK_OEM_8          0xDF

/*
 * 0xE0 : reserved
 */

/*
 * Various extended or enhanced keyboards
 */
#define VK_OEM_AX         0xE1  //  'AX' key on Japanese AX kbd
#define VK_OEM_102        0xE2  //  "<>" or "\|" on RT 102-key kbd.
#define VK_ICO_HELP       0xE3  //  Help key on ICO
#define VK_ICO_00         0xE4  //  00 key on ICO

#if(WINVER >= 0x0400)
#define VK_PROCESSKEY     0xE5
#endif /* WINVER >= 0x0400 */

#define VK_ICO_CLEAR      0xE6


#if(_WIN32_WINNT >= 0x0500)
#define VK_PACKET         0xE7
#endif /* _WIN32_WINNT >= 0x0500 */

/*
 * 0xE8 : unassigned
 */

/*
 * Nokia/Ericsson definitions
 */
#define VK_OEM_RESET      0xE9
#define VK_OEM_JUMP       0xEA
#define VK_OEM_PA1        0xEB
#define VK_OEM_PA2        0xEC
#define VK_OEM_PA3        0xED
#define VK_OEM_WSCTRL     0xEE
#define VK_OEM_CUSEL      0xEF
#define VK_OEM_ATTN       0xF0
#define VK_OEM_FINISH     0xF1
#define VK_OEM_COPY       0xF2
#define VK_OEM_AUTO       0xF3
#define VK_OEM_ENLW       0xF4
#define VK_OEM_BACKTAB    0xF5

#define VK_ATTN           0xF6
#define VK_CRSEL          0xF7
#define VK_EXSEL          0xF8
#define VK_EREOF          0xF9
#define VK_PLAY           0xFA
#define VK_ZOOM           0xFB
#define VK_NONAME         0xFC
#define VK_PA1            0xFD
#define VK_OEM_CLEAR      0xFE

/*
 * 0xFF : reserved
 */


#endif /* !NOVIRTUALKEYCODES */

#ifndef NOWH

/*
 * SetWindowsHook() codes
 */
#define WH_MIN              (-1)
#define WH_MSGFILTER        (-1)
#define WH_JOURNALRECORD    0
#define WH_JOURNALPLAYBACK  1
#define WH_KEYBOARD         2
#define WH_GETMESSAGE       3
#define WH_CALLWNDPROC      4
#define WH_CBT              5
#define WH_SYSMSGFILTER     6
#define WH_MOUSE            7
#if defined(_WIN32_WINDOWS)
#define WH_HARDWARE         8
#endif
#define WH_DEBUG            9
#define WH_SHELL           10
#define WH_FOREGROUNDIDLE  11
#if(WINVER >= 0x0400)
#define WH_CALLWNDPROCRET  12
#endif /* WINVER >= 0x0400 */

#if (_WIN32_WINNT >= 0x0400)
#define WH_KEYBOARD_LL     13
#define WH_MOUSE_LL        14
#endif // (_WIN32_WINNT >= 0x0400)

#if(WINVER >= 0x0400)
#if (_WIN32_WINNT >= 0x0400)
#define WH_MAX             14
#else
#define WH_MAX             12
#endif // (_WIN32_WINNT >= 0x0400)
#else
#define WH_MAX             11
#endif

#define WH_MINHOOK         WH_MIN
#define WH_MAXHOOK         WH_MAX

/*
 * Hook Codes
 */
#define HC_ACTION           0
#define HC_GETNEXT          1
#define HC_SKIP             2
#define HC_NOREMOVE         3
#define HC_NOREM            HC_NOREMOVE
#define HC_SYSMODALON       4
#define HC_SYSMODALOFF      5

/*
 * CBT Hook Codes
 */
#define HCBT_MOVESIZE       0
#define HCBT_MINMAX         1
#define HCBT_QS             2
#define HCBT_CREATEWND      3
#define HCBT_DESTROYWND     4
#define HCBT_ACTIVATE       5
#define HCBT_CLICKSKIPPED   6
#define HCBT_KEYSKIPPED     7
#define HCBT_SYSCOMMAND     8
#define HCBT_SETFOCUS       9

/*
 * HCBT_CREATEWND parameters pointed to by lParam
 */
typedef struct tagCBT_CREATEWNDA
{
    struct tagCREATESTRUCTA *lpcs;
    HWND           hwndInsertAfter;
} CBT_CREATEWNDA, *LPCBT_CREATEWNDA;
/*
 * HCBT_CREATEWND parameters pointed to by lParam
 */
typedef struct tagCBT_CREATEWNDW
{
    struct tagCREATESTRUCTW *lpcs;
    HWND           hwndInsertAfter;
} CBT_CREATEWNDW, *LPCBT_CREATEWNDW;
#ifdef UNICODE
typedef CBT_CREATEWNDW CBT_CREATEWND;
typedef LPCBT_CREATEWNDW LPCBT_CREATEWND;
#else
typedef CBT_CREATEWNDA CBT_CREATEWND;
typedef LPCBT_CREATEWNDA LPCBT_CREATEWND;
#endif // UNICODE

/*
 * HCBT_ACTIVATE structure pointed to by lParam
 */
typedef struct tagCBTACTIVATESTRUCT
{
    BOOL    fMouse;
    HWND    hWndActive;
} CBTACTIVATESTRUCT, *LPCBTACTIVATESTRUCT;

#if(_WIN32_WINNT >= 0x0501)
/*
 * WTSSESSION_NOTIFICATION struct pointed by lParam, for WM_WTSSESSION_CHANGE
 */
typedef struct tagWTSSESSION_NOTIFICATION
{
    DWORD cbSize;
    DWORD dwSessionId;

} WTSSESSION_NOTIFICATION, *PWTSSESSION_NOTIFICATION;

/*
 * codes passed in WPARAM for WM_WTSSESSION_CHANGE
 */

#define WTS_CONSOLE_CONNECT                0x1
#define WTS_CONSOLE_DISCONNECT             0x2
#define WTS_REMOTE_CONNECT                 0x3
#define WTS_REMOTE_DISCONNECT              0x4
#define WTS_SESSION_LOGON                  0x5
#define WTS_SESSION_LOGOFF                 0x6
#define WTS_SESSION_LOCK                   0x7
#define WTS_SESSION_UNLOCK                 0x8
#define WTS_SESSION_REMOTE_CONTROL         0x9

#endif /* _WIN32_WINNT >= 0x0501 */

/*
 * WH_MSGFILTER Filter Proc Codes
 */
#define MSGF_DIALOGBOX      0
#define MSGF_MESSAGEBOX     1
#define MSGF_MENU           2
#define MSGF_SCROLLBAR      5
#define MSGF_NEXTWINDOW     6
#define MSGF_MAX            8                       // unused
#define MSGF_USER           4096

/*
 * Shell support
 */
#define HSHELL_WINDOWCREATED        1
#define HSHELL_WINDOWDESTROYED      2
#define HSHELL_ACTIVATESHELLWINDOW  3

#if(WINVER >= 0x0400)
#define HSHELL_WINDOWACTIVATED      4
#define HSHELL_GETMINRECT           5
#define HSHELL_REDRAW               6
#define HSHELL_TASKMAN              7
#define HSHELL_LANGUAGE             8
#define HSHELL_SYSMENU              9
#define HSHELL_ENDTASK              10
#endif /* WINVER >= 0x0400 */
#if(_WIN32_WINNT >= 0x0500)
#define HSHELL_ACCESSIBILITYSTATE   11
#define HSHELL_APPCOMMAND           12
#endif /* _WIN32_WINNT >= 0x0500 */

#if(_WIN32_WINNT >= 0x0501)
#define HSHELL_WINDOWREPLACED       13
#define HSHELL_WINDOWREPLACING      14
#endif /* _WIN32_WINNT >= 0x0501 */


#define HSHELL_HIGHBIT            0x8000
#define HSHELL_FLASH              (HSHELL_REDRAW|HSHELL_HIGHBIT)
#define HSHELL_RUDEAPPACTIVATED   (HSHELL_WINDOWACTIVATED|HSHELL_HIGHBIT)

#if(_WIN32_WINNT >= 0x0500)
/* wparam for HSHELL_ACCESSIBILITYSTATE */
#define    ACCESS_STICKYKEYS            0x0001
#define    ACCESS_FILTERKEYS            0x0002
#define    ACCESS_MOUSEKEYS             0x0003

/* cmd for HSHELL_APPCOMMAND and WM_APPCOMMAND */
#define APPCOMMAND_BROWSER_BACKWARD       1
#define APPCOMMAND_BROWSER_FORWARD        2
#define APPCOMMAND_BROWSER_REFRESH        3
#define APPCOMMAND_BROWSER_STOP           4
#define APPCOMMAND_BROWSER_SEARCH         5
#define APPCOMMAND_BROWSER_FAVORITES      6
#define APPCOMMAND_BROWSER_HOME           7
#define APPCOMMAND_VOLUME_MUTE            8
#define APPCOMMAND_VOLUME_DOWN            9
#define APPCOMMAND_VOLUME_UP              10
#define APPCOMMAND_MEDIA_NEXTTRACK        11
#define APPCOMMAND_MEDIA_PREVIOUSTRACK    12
#define APPCOMMAND_MEDIA_STOP             13
#define APPCOMMAND_MEDIA_PLAY_PAUSE       14
#define APPCOMMAND_LAUNCH_MAIL            15
#define APPCOMMAND_LAUNCH_MEDIA_SELECT    16
#define APPCOMMAND_LAUNCH_APP1            17
#define APPCOMMAND_LAUNCH_APP2            18
#define APPCOMMAND_BASS_DOWN              19
#define APPCOMMAND_BASS_BOOST             20
#define APPCOMMAND_BASS_UP                21
#define APPCOMMAND_TREBLE_DOWN            22
#define APPCOMMAND_TREBLE_UP              23
#if(_WIN32_WINNT >= 0x0501)
#define APPCOMMAND_MICROPHONE_VOLUME_MUTE 24
#define APPCOMMAND_MICROPHONE_VOLUME_DOWN 25
#define APPCOMMAND_MICROPHONE_VOLUME_UP   26
#define APPCOMMAND_HELP                   27
#define APPCOMMAND_FIND                   28
#define APPCOMMAND_NEW                    29
#define APPCOMMAND_OPEN                   30
#define APPCOMMAND_CLOSE                  31
#define APPCOMMAND_SAVE                   32
#define APPCOMMAND_PRINT                  33
#define APPCOMMAND_UNDO                   34
#define APPCOMMAND_REDO                   35
#define APPCOMMAND_COPY                   36
#define APPCOMMAND_CUT                    37
#define APPCOMMAND_PASTE                  38
#define APPCOMMAND_REPLY_TO_MAIL          39
#define APPCOMMAND_FORWARD_MAIL           40
#define APPCOMMAND_SEND_MAIL              41
#define APPCOMMAND_SPELL_CHECK            42
#define APPCOMMAND_DICTATE_OR_COMMAND_CONTROL_TOGGLE    43
#define APPCOMMAND_MIC_ON_OFF_TOGGLE      44
#define APPCOMMAND_CORRECTION_LIST        45
#define APPCOMMAND_MEDIA_PLAY             46
#define APPCOMMAND_MEDIA_PAUSE            47
#define APPCOMMAND_MEDIA_RECORD           48
#define APPCOMMAND_MEDIA_FAST_FORWARD     49
#define APPCOMMAND_MEDIA_REWIND           50
#define APPCOMMAND_MEDIA_CHANNEL_UP       51
#define APPCOMMAND_MEDIA_CHANNEL_DOWN     52
#endif /* _WIN32_WINNT >= 0x0501 */

#define FAPPCOMMAND_MOUSE 0x8000
#define FAPPCOMMAND_KEY   0
#define FAPPCOMMAND_OEM   0x1000
#define FAPPCOMMAND_MASK  0xF000

#define GET_APPCOMMAND_LPARAM(lParam) ((short)(HIWORD(lParam) & ~FAPPCOMMAND_MASK))
#define GET_DEVICE_LPARAM(lParam)     ((WORD)(HIWORD(lParam) & FAPPCOMMAND_MASK))
#define GET_MOUSEORKEY_LPARAM         GET_DEVICE_LPARAM
#define GET_FLAGS_LPARAM(lParam)      (LOWORD(lParam))
#define GET_KEYSTATE_LPARAM(lParam)   GET_FLAGS_LPARAM(lParam)
#endif /* _WIN32_WINNT >= 0x0500 */

typedef struct
{
    HWND    hwnd;
    RECT    rc;
} SHELLHOOKINFO, *LPSHELLHOOKINFO;

/*
 * Message Structure used in Journaling
 */
typedef struct tagEVENTMSG {
    UINT    message;
    UINT    paramL;
    UINT    paramH;
    DWORD    time;
    HWND     hwnd;
} EVENTMSG, *PEVENTMSGMSG, NEAR *NPEVENTMSGMSG, FAR *LPEVENTMSGMSG;

typedef struct tagEVENTMSG *PEVENTMSG, NEAR *NPEVENTMSG, FAR *LPEVENTMSG;

/*
 * Message structure used by WH_CALLWNDPROC
 */
typedef struct tagCWPSTRUCT {
    LPARAM  lParam;
    WPARAM  wParam;
    UINT    message;
    HWND    hwnd;
} CWPSTRUCT, *PCWPSTRUCT, NEAR *NPCWPSTRUCT, FAR *LPCWPSTRUCT;

#if(WINVER >= 0x0400)
/*
 * Message structure used by WH_CALLWNDPROCRET
 */
typedef struct tagCWPRETSTRUCT {
    LRESULT lResult;
    LPARAM  lParam;
    WPARAM  wParam;
    UINT    message;
    HWND    hwnd;
} CWPRETSTRUCT, *PCWPRETSTRUCT, NEAR *NPCWPRETSTRUCT, FAR *LPCWPRETSTRUCT;

#endif /* WINVER >= 0x0400 */

#if (_WIN32_WINNT >= 0x0400)

/*
 * Low level hook flags
 */

#define LLKHF_EXTENDED       (KF_EXTENDED >> 8)
#define LLKHF_INJECTED       0x00000010
#define LLKHF_ALTDOWN        (KF_ALTDOWN >> 8)
#define LLKHF_UP             (KF_UP >> 8)

#define LLMHF_INJECTED       0x00000001

/*
 * Structure used by WH_KEYBOARD_LL
 */
typedef struct tagKBDLLHOOKSTRUCT {
    DWORD   vkCode;
    DWORD   scanCode;
    DWORD   flags;
    DWORD   time;
    ULONG_PTR dwExtraInfo;
} KBDLLHOOKSTRUCT, FAR *LPKBDLLHOOKSTRUCT, *PKBDLLHOOKSTRUCT;

/*
 * Structure used by WH_MOUSE_LL
 */
typedef struct tagMSLLHOOKSTRUCT {
    POINT   pt;
    DWORD   mouseData;
    DWORD   flags;
    DWORD   time;
    ULONG_PTR dwExtraInfo;
} MSLLHOOKSTRUCT, FAR *LPMSLLHOOKSTRUCT, *PMSLLHOOKSTRUCT;

#endif // (_WIN32_WINNT >= 0x0400)

/*
 * Structure used by WH_DEBUG
 */
typedef struct tagDEBUGHOOKINFO
{
    DWORD   idThread;
    DWORD   idThreadInstaller;
    LPARAM  lParam;
    WPARAM  wParam;
    int     code;
} DEBUGHOOKINFO, *PDEBUGHOOKINFO, NEAR *NPDEBUGHOOKINFO, FAR* LPDEBUGHOOKINFO;

/*
 * Structure used by WH_MOUSE
 */
typedef struct tagMOUSEHOOKSTRUCT {
    POINT   pt;
    HWND    hwnd;
    UINT    wHitTestCode;
    ULONG_PTR dwExtraInfo;
} MOUSEHOOKSTRUCT, FAR *LPMOUSEHOOKSTRUCT, *PMOUSEHOOKSTRUCT;

#if(_WIN32_WINNT >= 0x0500)
#ifdef __cplusplus
typedef struct tagMOUSEHOOKSTRUCTEX : public tagMOUSEHOOKSTRUCT
{
    DWORD   mouseData;
} MOUSEHOOKSTRUCTEX, *LPMOUSEHOOKSTRUCTEX, *PMOUSEHOOKSTRUCTEX;
#else // ndef __cplusplus
typedef struct tagMOUSEHOOKSTRUCTEX
{
    MOUSEHOOKSTRUCT;
    DWORD   mouseData;
} MOUSEHOOKSTRUCTEX, *LPMOUSEHOOKSTRUCTEX, *PMOUSEHOOKSTRUCTEX;
#endif
#endif /* _WIN32_WINNT >= 0x0500 */

#if(WINVER >= 0x0400)
/*
 * Structure used by WH_HARDWARE
 */
typedef struct tagHARDWAREHOOKSTRUCT {
    HWND    hwnd;
    UINT    message;
    WPARAM  wParam;
    LPARAM  lParam;
} HARDWAREHOOKSTRUCT, FAR *LPHARDWAREHOOKSTRUCT, *PHARDWAREHOOKSTRUCT;
#endif /* WINVER >= 0x0400 */
#endif /* !NOWH */

/*
 * Keyboard Layout API
 */
#define HKL_PREV            0
#define HKL_NEXT            1


#define KLF_ACTIVATE        0x00000001
#define KLF_SUBSTITUTE_OK   0x00000002
#define KLF_REORDER         0x00000008
#if(WINVER >= 0x0400)
#define KLF_REPLACELANG     0x00000010
#define KLF_NOTELLSHELL     0x00000080
#endif /* WINVER >= 0x0400 */
#define KLF_SETFORPROCESS   0x00000100
#if(_WIN32_WINNT >= 0x0500)
#define KLF_SHIFTLOCK       0x00010000
#define KLF_RESET           0x40000000
#endif /* _WIN32_WINNT >= 0x0500 */


#if(WINVER >= 0x0500)
/*
 * Bits in wParam of WM_INPUTLANGCHANGEREQUEST message
 */
#define INPUTLANGCHANGE_SYSCHARSET 0x0001
#define INPUTLANGCHANGE_FORWARD    0x0002
#define INPUTLANGCHANGE_BACKWARD   0x0004
#endif /* WINVER >= 0x0500 */

/*
 * Size of KeyboardLayoutName (number of characters), including nul terminator
 */
#define KL_NAMELENGTH 9

WINUSERAPI
HKL
WINAPI
LoadKeyboardLayoutA(
    __in LPCSTR pwszKLID,
    __in UINT Flags);
WINUSERAPI
HKL
WINAPI
LoadKeyboardLayoutW(
    __in LPCWSTR pwszKLID,
    __in UINT Flags);
#ifdef UNICODE
#define LoadKeyboardLayout  LoadKeyboardLayoutW
#else
#define LoadKeyboardLayout  LoadKeyboardLayoutA
#endif // !UNICODE


#if(WINVER >= 0x0400)
WINUSERAPI
HKL
WINAPI
ActivateKeyboardLayout(
    __in HKL hkl,
    __in UINT Flags);
#else
WINUSERAPI
BOOL
WINAPI
ActivateKeyboardLayout(
    __in HKL hkl,
    __in UINT Flags);
#endif /* WINVER >= 0x0400 */

#if(WINVER >= 0x0400)
WINUSERAPI
int
WINAPI
ToUnicodeEx(
    __in UINT wVirtKey,
    __in UINT wScanCode,
    __in_bcount(256) CONST BYTE *lpKeyState,
    __out_ecount(cchBuff) LPWSTR pwszBuff,
    __in int cchBuff,
    __in UINT wFlags,
    __in_opt HKL dwhkl);
#endif /* WINVER >= 0x0400 */

WINUSERAPI
BOOL
WINAPI
UnloadKeyboardLayout(
    __in HKL hkl);

WINUSERAPI
BOOL
WINAPI
GetKeyboardLayoutNameA(
    __out_ecount(KL_NAMELENGTH) LPSTR pwszKLID);
WINUSERAPI
BOOL
WINAPI
GetKeyboardLayoutNameW(
    __out_ecount(KL_NAMELENGTH) LPWSTR pwszKLID);
#ifdef UNICODE
#define GetKeyboardLayoutName  GetKeyboardLayoutNameW
#else
#define GetKeyboardLayoutName  GetKeyboardLayoutNameA
#endif // !UNICODE

#if(WINVER >= 0x0400)
WINUSERAPI
int
WINAPI
GetKeyboardLayoutList(
    __in int nBuff,
    __out_ecount_part_opt(nBuff, return) HKL FAR *lpList);

WINUSERAPI
HKL
WINAPI
GetKeyboardLayout(
    __in DWORD idThread);

#endif /* WINVER >= 0x0400 */

#if(WINVER >= 0x0500)

typedef struct tagMOUSEMOVEPOINT {
    int   x;
    int   y;
    DWORD time;
    ULONG_PTR dwExtraInfo;
} MOUSEMOVEPOINT, *PMOUSEMOVEPOINT, FAR* LPMOUSEMOVEPOINT;

/*
 * Values for resolution parameter of GetMouseMovePointsEx
 */
#define GMMP_USE_DISPLAY_POINTS          1
#define GMMP_USE_HIGH_RESOLUTION_POINTS  2

WINUSERAPI
int
WINAPI
GetMouseMovePointsEx(
    __in UINT cbSize,
    __in LPMOUSEMOVEPOINT lppt,
    __out_ecount(nBufPoints) LPMOUSEMOVEPOINT lpptBuf,
    __in int nBufPoints,
    __in DWORD resolution);

#endif /* WINVER >= 0x0500 */

#ifndef NODESKTOP
/*
 * Desktop-specific access flags
 */
#define DESKTOP_READOBJECTS         0x0001L
#define DESKTOP_CREATEWINDOW        0x0002L
#define DESKTOP_CREATEMENU          0x0004L
#define DESKTOP_HOOKCONTROL         0x0008L
#define DESKTOP_JOURNALRECORD       0x0010L
#define DESKTOP_JOURNALPLAYBACK     0x0020L
#define DESKTOP_ENUMERATE           0x0040L
#define DESKTOP_WRITEOBJECTS        0x0080L
#define DESKTOP_SWITCHDESKTOP       0x0100L

/*
 * Desktop-specific control flags
 */
#define DF_ALLOWOTHERACCOUNTHOOK    0x0001L

#ifdef _WINGDI_
#ifndef NOGDI

WINUSERAPI
HDESK
WINAPI
CreateDesktopA(
    __in LPCSTR lpszDesktop,
    __reserved LPCSTR lpszDevice,
    __reserved LPDEVMODEA pDevmode,
    __in DWORD dwFlags,
    __in ACCESS_MASK dwDesiredAccess,
    __in_opt LPSECURITY_ATTRIBUTES lpsa);
WINUSERAPI
HDESK
WINAPI
CreateDesktopW(
    __in LPCWSTR lpszDesktop,
    __reserved LPCWSTR lpszDevice,
    __reserved LPDEVMODEW pDevmode,
    __in DWORD dwFlags,
    __in ACCESS_MASK dwDesiredAccess,
    __in_opt LPSECURITY_ATTRIBUTES lpsa);
#ifdef UNICODE
#define CreateDesktop  CreateDesktopW
#else
#define CreateDesktop  CreateDesktopA
#endif // !UNICODE

#endif /* NOGDI */
#endif /* _WINGDI_ */

WINUSERAPI
HDESK
WINAPI
OpenDesktopA(
    __in LPCSTR lpszDesktop,
    __in DWORD dwFlags,
    __in BOOL fInherit,
    __in ACCESS_MASK dwDesiredAccess);
WINUSERAPI
HDESK
WINAPI
OpenDesktopW(
    __in LPCWSTR lpszDesktop,
    __in DWORD dwFlags,
    __in BOOL fInherit,
    __in ACCESS_MASK dwDesiredAccess);
#ifdef UNICODE
#define OpenDesktop  OpenDesktopW
#else
#define OpenDesktop  OpenDesktopA
#endif // !UNICODE

WINUSERAPI
HDESK
WINAPI
OpenInputDesktop(
    __in DWORD dwFlags,
    __in BOOL fInherit,
    __in ACCESS_MASK dwDesiredAccess);


WINUSERAPI
BOOL
WINAPI
EnumDesktopsA(
    __in_opt HWINSTA hwinsta,
    __in DESKTOPENUMPROCA lpEnumFunc,
    __in LPARAM lParam);
WINUSERAPI
BOOL
WINAPI
EnumDesktopsW(
    __in_opt HWINSTA hwinsta,
    __in DESKTOPENUMPROCW lpEnumFunc,
    __in LPARAM lParam);
#ifdef UNICODE
#define EnumDesktops  EnumDesktopsW
#else
#define EnumDesktops  EnumDesktopsA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
EnumDesktopWindows(
    __in_opt HDESK hDesktop,
    __in WNDENUMPROC lpfn,
    __in LPARAM lParam);

WINUSERAPI
BOOL
WINAPI
SwitchDesktop(
    __in HDESK hDesktop);

WINUSERAPI
BOOL
WINAPI
SetThreadDesktop(
     __in HDESK hDesktop);

WINUSERAPI
BOOL
WINAPI
CloseDesktop(
    __in HDESK hDesktop);

WINUSERAPI
HDESK
WINAPI
GetThreadDesktop(
    __in DWORD dwThreadId);

#endif  /* !NODESKTOP */

#ifndef NOWINDOWSTATION
/*
 * Windowstation-specific access flags
 */
#define WINSTA_ENUMDESKTOPS         0x0001L
#define WINSTA_READATTRIBUTES       0x0002L
#define WINSTA_ACCESSCLIPBOARD      0x0004L
#define WINSTA_CREATEDESKTOP        0x0008L
#define WINSTA_WRITEATTRIBUTES      0x0010L
#define WINSTA_ACCESSGLOBALATOMS    0x0020L
#define WINSTA_EXITWINDOWS          0x0040L
#define WINSTA_ENUMERATE            0x0100L
#define WINSTA_READSCREEN           0x0200L

#define WINSTA_ALL_ACCESS           (WINSTA_ENUMDESKTOPS  | WINSTA_READATTRIBUTES  | WINSTA_ACCESSCLIPBOARD | \
                                     WINSTA_CREATEDESKTOP | WINSTA_WRITEATTRIBUTES | WINSTA_ACCESSGLOBALATOMS | \
                                     WINSTA_EXITWINDOWS   | WINSTA_ENUMERATE       | WINSTA_READSCREEN)

/*
 * Windowstation creation flags.
 */
#define CWF_CREATE_ONLY          0x0001L

/*
 * Windowstation-specific attribute flags
 */
#define WSF_VISIBLE                 0x0001L

WINUSERAPI
HWINSTA
WINAPI
CreateWindowStationA(
    __in_opt LPCSTR lpwinsta,
    __in DWORD dwFlags,
    __in ACCESS_MASK dwDesiredAccess,
    __in_opt LPSECURITY_ATTRIBUTES lpsa);
WINUSERAPI
HWINSTA
WINAPI
CreateWindowStationW(
    __in_opt LPCWSTR lpwinsta,
    __in DWORD dwFlags,
    __in ACCESS_MASK dwDesiredAccess,
    __in_opt LPSECURITY_ATTRIBUTES lpsa);
#ifdef UNICODE
#define CreateWindowStation  CreateWindowStationW
#else
#define CreateWindowStation  CreateWindowStationA
#endif // !UNICODE

WINUSERAPI
HWINSTA
WINAPI
OpenWindowStationA(
    __in LPCSTR lpszWinSta,
    __in BOOL fInherit,
    __in ACCESS_MASK dwDesiredAccess);
WINUSERAPI
HWINSTA
WINAPI
OpenWindowStationW(
    __in LPCWSTR lpszWinSta,
    __in BOOL fInherit,
    __in ACCESS_MASK dwDesiredAccess);
#ifdef UNICODE
#define OpenWindowStation  OpenWindowStationW
#else
#define OpenWindowStation  OpenWindowStationA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
EnumWindowStationsA(
    __in WINSTAENUMPROCA lpEnumFunc,
    __in LPARAM lParam);
WINUSERAPI
BOOL
WINAPI
EnumWindowStationsW(
    __in WINSTAENUMPROCW lpEnumFunc,
    __in LPARAM lParam);
#ifdef UNICODE
#define EnumWindowStations  EnumWindowStationsW
#else
#define EnumWindowStations  EnumWindowStationsA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
CloseWindowStation(
    __in HWINSTA hWinSta);

WINUSERAPI
BOOL
WINAPI
SetProcessWindowStation(
    __in HWINSTA hWinSta);

WINUSERAPI
HWINSTA
WINAPI
GetProcessWindowStation(
    VOID);
#endif  /* !NOWINDOWSTATION */

#ifndef NOSECURITY

WINUSERAPI
BOOL
WINAPI
SetUserObjectSecurity(
    __in HANDLE hObj,
    __in PSECURITY_INFORMATION pSIRequested,
    __in PSECURITY_DESCRIPTOR pSID);

WINUSERAPI
BOOL
WINAPI
GetUserObjectSecurity(
    __in HANDLE hObj,
    __in PSECURITY_INFORMATION pSIRequested,
    __out_bcount_opt(nLength) PSECURITY_DESCRIPTOR pSID,
    __in DWORD nLength,
    __out LPDWORD lpnLengthNeeded);

#define UOI_FLAGS       1
#define UOI_NAME        2
#define UOI_TYPE        3
#define UOI_USER_SID    4

typedef struct tagUSEROBJECTFLAGS {
    BOOL fInherit;
    BOOL fReserved;
    DWORD dwFlags;
} USEROBJECTFLAGS, *PUSEROBJECTFLAGS;

WINUSERAPI
BOOL
WINAPI
GetUserObjectInformationA(
    __in HANDLE hObj,
    __in int nIndex,
    __out_bcount_opt(nLength) PVOID pvInfo,
    __in DWORD nLength,
    __out_opt LPDWORD lpnLengthNeeded);
WINUSERAPI
BOOL
WINAPI
GetUserObjectInformationW(
    __in HANDLE hObj,
    __in int nIndex,
    __out_bcount_opt(nLength) PVOID pvInfo,
    __in DWORD nLength,
    __out_opt LPDWORD lpnLengthNeeded);
#ifdef UNICODE
#define GetUserObjectInformation  GetUserObjectInformationW
#else
#define GetUserObjectInformation  GetUserObjectInformationA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
SetUserObjectInformationA(
    __in HANDLE hObj,
    __in int nIndex,
    __in_bcount(nLength) PVOID pvInfo,
    __in DWORD nLength);
WINUSERAPI
BOOL
WINAPI
SetUserObjectInformationW(
    __in HANDLE hObj,
    __in int nIndex,
    __in_bcount(nLength) PVOID pvInfo,
    __in DWORD nLength);
#ifdef UNICODE
#define SetUserObjectInformation  SetUserObjectInformationW
#else
#define SetUserObjectInformation  SetUserObjectInformationA
#endif // !UNICODE

#endif  /* !NOSECURITY */

#if(WINVER >= 0x0400)
typedef struct tagWNDCLASSEXA {
    UINT        cbSize;
    /* Win 3.x */
    UINT        style;
    WNDPROC     lpfnWndProc;
    int         cbClsExtra;
    int         cbWndExtra;
    HINSTANCE   hInstance;
    HICON       hIcon;
    HCURSOR     hCursor;
    HBRUSH      hbrBackground;
    LPCSTR      lpszMenuName;
    LPCSTR      lpszClassName;
    /* Win 4.0 */
    HICON       hIconSm;
} WNDCLASSEXA, *PWNDCLASSEXA, NEAR *NPWNDCLASSEXA, FAR *LPWNDCLASSEXA;
typedef struct tagWNDCLASSEXW {
    UINT        cbSize;
    /* Win 3.x */
    UINT        style;
    WNDPROC     lpfnWndProc;
    int         cbClsExtra;
    int         cbWndExtra;
    HINSTANCE   hInstance;
    HICON       hIcon;
    HCURSOR     hCursor;
    HBRUSH      hbrBackground;
    LPCWSTR     lpszMenuName;
    LPCWSTR     lpszClassName;
    /* Win 4.0 */
    HICON       hIconSm;
} WNDCLASSEXW, *PWNDCLASSEXW, NEAR *NPWNDCLASSEXW, FAR *LPWNDCLASSEXW;
#ifdef UNICODE
typedef WNDCLASSEXW WNDCLASSEX;
typedef PWNDCLASSEXW PWNDCLASSEX;
typedef NPWNDCLASSEXW NPWNDCLASSEX;
typedef LPWNDCLASSEXW LPWNDCLASSEX;
#else
typedef WNDCLASSEXA WNDCLASSEX;
typedef PWNDCLASSEXA PWNDCLASSEX;
typedef NPWNDCLASSEXA NPWNDCLASSEX;
typedef LPWNDCLASSEXA LPWNDCLASSEX;
#endif // UNICODE
#endif /* WINVER >= 0x0400 */

typedef struct tagWNDCLASSA {
    UINT        style;
    WNDPROC     lpfnWndProc;
    int         cbClsExtra;
    int         cbWndExtra;
    HINSTANCE   hInstance;
    HICON       hIcon;
    HCURSOR     hCursor;
    HBRUSH      hbrBackground;
    LPCSTR      lpszMenuName;
    LPCSTR      lpszClassName;
} WNDCLASSA, *PWNDCLASSA, NEAR *NPWNDCLASSA, FAR *LPWNDCLASSA;
typedef struct tagWNDCLASSW {
    UINT        style;
    WNDPROC     lpfnWndProc;
    int         cbClsExtra;
    int         cbWndExtra;
    HINSTANCE   hInstance;
    HICON       hIcon;
    HCURSOR     hCursor;
    HBRUSH      hbrBackground;
    LPCWSTR     lpszMenuName;
    LPCWSTR     lpszClassName;
} WNDCLASSW, *PWNDCLASSW, NEAR *NPWNDCLASSW, FAR *LPWNDCLASSW;
#ifdef UNICODE
typedef WNDCLASSW WNDCLASS;
typedef PWNDCLASSW PWNDCLASS;
typedef NPWNDCLASSW NPWNDCLASS;
typedef LPWNDCLASSW LPWNDCLASS;
#else
typedef WNDCLASSA WNDCLASS;
typedef PWNDCLASSA PWNDCLASS;
typedef NPWNDCLASSA NPWNDCLASS;
typedef LPWNDCLASSA LPWNDCLASS;
#endif // UNICODE

WINUSERAPI
BOOL
WINAPI
IsHungAppWindow(
    __in HWND hwnd);


#if(WINVER >= 0x0501)
WINUSERAPI
VOID
WINAPI
DisableProcessWindowsGhosting(
    VOID);
#endif /* WINVER >= 0x0501 */


#ifndef NOMSG

/*
 * Message structure
 */
typedef struct tagMSG {
    HWND        hwnd;
    UINT        message;
    WPARAM      wParam;
    LPARAM      lParam;
    DWORD       time;
    POINT       pt;
#ifdef _MAC
    DWORD       lPrivate;
#endif
} MSG, *PMSG, NEAR *NPMSG, FAR *LPMSG;

#define POINTSTOPOINT(pt, pts)                          \
        { (pt).x = (LONG)(SHORT)LOWORD(*(LONG*)&pts);   \
          (pt).y = (LONG)(SHORT)HIWORD(*(LONG*)&pts); }

#define POINTTOPOINTS(pt)      (MAKELONG((short)((pt).x), (short)((pt).y)))
#define MAKEWPARAM(l, h)      ((WPARAM)(DWORD)MAKELONG(l, h))
#define MAKELPARAM(l, h)      ((LPARAM)(DWORD)MAKELONG(l, h))
#define MAKELRESULT(l, h)     ((LRESULT)(DWORD)MAKELONG(l, h))


#endif /* !NOMSG */

#ifndef NOWINOFFSETS

/*
 * Window field offsets for GetWindowLong()
 */
#define GWL_WNDPROC         (-4)
#define GWL_HINSTANCE       (-6)
#define GWL_HWNDPARENT      (-8)
#define GWL_STYLE           (-16)
#define GWL_EXSTYLE         (-20)
#define GWL_USERDATA        (-21)
#define GWL_ID              (-12)

#ifdef _WIN64

#undef GWL_WNDPROC
#undef GWL_HINSTANCE
#undef GWL_HWNDPARENT
#undef GWL_USERDATA

#endif /* _WIN64 */

#define GWLP_WNDPROC        (-4)
#define GWLP_HINSTANCE      (-6)
#define GWLP_HWNDPARENT     (-8)
#define GWLP_USERDATA       (-21)
#define GWLP_ID             (-12)

/*
 * Class field offsets for GetClassLong()
 */
#define GCL_MENUNAME        (-8)
#define GCL_HBRBACKGROUND   (-10)
#define GCL_HCURSOR         (-12)
#define GCL_HICON           (-14)
#define GCL_HMODULE         (-16)
#define GCL_CBWNDEXTRA      (-18)
#define GCL_CBCLSEXTRA      (-20)
#define GCL_WNDPROC         (-24)
#define GCL_STYLE           (-26)
#define GCW_ATOM            (-32)

#if(WINVER >= 0x0400)
#define GCL_HICONSM         (-34)
#endif /* WINVER >= 0x0400 */

#ifdef _WIN64

#undef GCL_MENUNAME
#undef GCL_HBRBACKGROUND
#undef GCL_HCURSOR
#undef GCL_HICON
#undef GCL_HMODULE
#undef GCL_WNDPROC
#undef GCL_HICONSM

#endif /* _WIN64 */

#define GCLP_MENUNAME       (-8)
#define GCLP_HBRBACKGROUND  (-10)
#define GCLP_HCURSOR        (-12)
#define GCLP_HICON          (-14)
#define GCLP_HMODULE        (-16)
#define GCLP_WNDPROC        (-24)
#define GCLP_HICONSM        (-34)

#endif /* !NOWINOFFSETS */

#ifndef NOWINMESSAGES


/*
 * Window Messages
 */

#define WM_NULL                         0x0000
#define WM_CREATE                       0x0001
#define WM_DESTROY                      0x0002
#define WM_MOVE                         0x0003
#define WM_SIZE                         0x0005

#define WM_ACTIVATE                     0x0006
/*
 * WM_ACTIVATE state values
 */
#define     WA_INACTIVE     0
#define     WA_ACTIVE       1
#define     WA_CLICKACTIVE  2

#define WM_SETFOCUS                     0x0007
#define WM_KILLFOCUS                    0x0008
#define WM_ENABLE                       0x000A
#define WM_SETREDRAW                    0x000B
#define WM_SETTEXT                      0x000C
#define WM_GETTEXT                      0x000D
#define WM_GETTEXTLENGTH                0x000E
#define WM_PAINT                        0x000F
#define WM_CLOSE                        0x0010
#ifndef _WIN32_WCE
#define WM_QUERYENDSESSION              0x0011
#define WM_QUERYOPEN                    0x0013
#define WM_ENDSESSION                   0x0016
#endif
#define WM_QUIT                         0x0012
#define WM_ERASEBKGND                   0x0014
#define WM_SYSCOLORCHANGE               0x0015
#define WM_SHOWWINDOW                   0x0018
#define WM_WININICHANGE                 0x001A
#if(WINVER >= 0x0400)
#define WM_SETTINGCHANGE                WM_WININICHANGE
#endif /* WINVER >= 0x0400 */


#define WM_DEVMODECHANGE                0x001B
#define WM_ACTIVATEAPP                  0x001C
#define WM_FONTCHANGE                   0x001D
#define WM_TIMECHANGE                   0x001E
#define WM_CANCELMODE                   0x001F
#define WM_SETCURSOR                    0x0020
#define WM_MOUSEACTIVATE                0x0021
#define WM_CHILDACTIVATE                0x0022
#define WM_QUEUESYNC                    0x0023

#define WM_GETMINMAXINFO                0x0024
/*
 * Struct pointed to by WM_GETMINMAXINFO lParam
 */
typedef struct tagMINMAXINFO {
    POINT ptReserved;
    POINT ptMaxSize;
    POINT ptMaxPosition;
    POINT ptMinTrackSize;
    POINT ptMaxTrackSize;
} MINMAXINFO, *PMINMAXINFO, *LPMINMAXINFO;

#define WM_PAINTICON                    0x0026
#define WM_ICONERASEBKGND               0x0027
#define WM_NEXTDLGCTL                   0x0028
#define WM_SPOOLERSTATUS                0x002A
#define WM_DRAWITEM                     0x002B
#define WM_MEASUREITEM                  0x002C
#define WM_DELETEITEM                   0x002D
#define WM_VKEYTOITEM                   0x002E
#define WM_CHARTOITEM                   0x002F
#define WM_SETFONT                      0x0030
#define WM_GETFONT                      0x0031
#define WM_SETHOTKEY                    0x0032
#define WM_GETHOTKEY                    0x0033
#define WM_QUERYDRAGICON                0x0037
#define WM_COMPAREITEM                  0x0039
#if(WINVER >= 0x0500)
#ifndef _WIN32_WCE
#define WM_GETOBJECT                    0x003D
#endif
#endif /* WINVER >= 0x0500 */
#define WM_COMPACTING                   0x0041
#define WM_COMMNOTIFY                   0x0044  /* no longer suported */
#define WM_WINDOWPOSCHANGING            0x0046
#define WM_WINDOWPOSCHANGED             0x0047

#define WM_POWER                        0x0048
/*
 * wParam for WM_POWER window message and DRV_POWER driver notification
 */
#define PWR_OK              1
#define PWR_FAIL            (-1)
#define PWR_SUSPENDREQUEST  1
#define PWR_SUSPENDRESUME   2
#define PWR_CRITICALRESUME  3

#define WM_COPYDATA                     0x004A
#define WM_CANCELJOURNAL                0x004B


/*
 * lParam of WM_COPYDATA message points to...
 */
typedef struct tagCOPYDATASTRUCT {
    ULONG_PTR dwData;
    DWORD cbData;
    PVOID lpData;
} COPYDATASTRUCT, *PCOPYDATASTRUCT;

#if(WINVER >= 0x0400)
typedef struct tagMDINEXTMENU
{
    HMENU   hmenuIn;
    HMENU   hmenuNext;
    HWND    hwndNext;
} MDINEXTMENU, * PMDINEXTMENU, FAR * LPMDINEXTMENU;
#endif /* WINVER >= 0x0400 */


#if(WINVER >= 0x0400)
#define WM_NOTIFY                       0x004E
#define WM_INPUTLANGCHANGEREQUEST       0x0050
#define WM_INPUTLANGCHANGE              0x0051
#define WM_TCARD                        0x0052
#define WM_HELP                         0x0053
#define WM_USERCHANGED                  0x0054
#define WM_NOTIFYFORMAT                 0x0055

#define NFR_ANSI                             1
#define NFR_UNICODE                          2
#define NF_QUERY                             3
#define NF_REQUERY                           4

#define WM_CONTEXTMENU                  0x007B
#define WM_STYLECHANGING                0x007C
#define WM_STYLECHANGED                 0x007D
#define WM_DISPLAYCHANGE                0x007E
#define WM_GETICON                      0x007F
#define WM_SETICON                      0x0080
#endif /* WINVER >= 0x0400 */

#define WM_NCCREATE                     0x0081
#define WM_NCDESTROY                    0x0082
#define WM_NCCALCSIZE                   0x0083
#define WM_NCHITTEST                    0x0084
#define WM_NCPAINT                      0x0085
#define WM_NCACTIVATE                   0x0086
#define WM_GETDLGCODE                   0x0087
#ifndef _WIN32_WCE
#define WM_SYNCPAINT                    0x0088
#endif
#define WM_NCMOUSEMOVE                  0x00A0
#define WM_NCLBUTTONDOWN                0x00A1
#define WM_NCLBUTTONUP                  0x00A2
#define WM_NCLBUTTONDBLCLK              0x00A3
#define WM_NCRBUTTONDOWN                0x00A4
#define WM_NCRBUTTONUP                  0x00A5
#define WM_NCRBUTTONDBLCLK              0x00A6
#define WM_NCMBUTTONDOWN                0x00A7
#define WM_NCMBUTTONUP                  0x00A8
#define WM_NCMBUTTONDBLCLK              0x00A9



#if(_WIN32_WINNT >= 0x0500)
#define WM_NCXBUTTONDOWN                0x00AB
#define WM_NCXBUTTONUP                  0x00AC
#define WM_NCXBUTTONDBLCLK              0x00AD
#endif /* _WIN32_WINNT >= 0x0500 */


#if(_WIN32_WINNT >= 0x0501)
#define WM_INPUT                        0x00FF
#endif /* _WIN32_WINNT >= 0x0501 */

#define WM_KEYFIRST                     0x0100
#define WM_KEYDOWN                      0x0100
#define WM_KEYUP                        0x0101
#define WM_CHAR                         0x0102
#define WM_DEADCHAR                     0x0103
#define WM_SYSKEYDOWN                   0x0104
#define WM_SYSKEYUP                     0x0105
#define WM_SYSCHAR                      0x0106
#define WM_SYSDEADCHAR                  0x0107
#if(_WIN32_WINNT >= 0x0501)
#define WM_UNICHAR                      0x0109
#define WM_KEYLAST                      0x0109
#define UNICODE_NOCHAR                  0xFFFF
#else
#define WM_KEYLAST                      0x0108
#endif /* _WIN32_WINNT >= 0x0501 */

#if(WINVER >= 0x0400)
#define WM_IME_STARTCOMPOSITION         0x010D
#define WM_IME_ENDCOMPOSITION           0x010E
#define WM_IME_COMPOSITION              0x010F
#define WM_IME_KEYLAST                  0x010F
#endif /* WINVER >= 0x0400 */

#define WM_INITDIALOG                   0x0110
#define WM_COMMAND                      0x0111
#define WM_SYSCOMMAND                   0x0112
#define WM_TIMER                        0x0113
#define WM_HSCROLL                      0x0114
#define WM_VSCROLL                      0x0115
#define WM_INITMENU                     0x0116
#define WM_INITMENUPOPUP                0x0117
#define WM_MENUSELECT                   0x011F
#define WM_MENUCHAR                     0x0120
#define WM_ENTERIDLE                    0x0121
#if(WINVER >= 0x0500)
#ifndef _WIN32_WCE
#define WM_MENURBUTTONUP                0x0122
#define WM_MENUDRAG                     0x0123
#define WM_MENUGETOBJECT                0x0124
#define WM_UNINITMENUPOPUP              0x0125
#define WM_MENUCOMMAND                  0x0126

#ifndef _WIN32_WCE
#if(_WIN32_WINNT >= 0x0500)
#define WM_CHANGEUISTATE                0x0127
#define WM_UPDATEUISTATE                0x0128
#define WM_QUERYUISTATE                 0x0129

/*
 * LOWORD(wParam) values in WM_*UISTATE*
 */
#define UIS_SET                         1
#define UIS_CLEAR                       2
#define UIS_INITIALIZE                  3

/*
 * HIWORD(wParam) values in WM_*UISTATE*
 */
#define UISF_HIDEFOCUS                  0x1
#define UISF_HIDEACCEL                  0x2
#if(_WIN32_WINNT >= 0x0501)
#define UISF_ACTIVE                     0x4
#endif /* _WIN32_WINNT >= 0x0501 */
#endif /* _WIN32_WINNT >= 0x0500 */
#endif

#endif
#endif /* WINVER >= 0x0500 */

#define WM_CTLCOLORMSGBOX               0x0132
#define WM_CTLCOLOREDIT                 0x0133
#define WM_CTLCOLORLISTBOX              0x0134
#define WM_CTLCOLORBTN                  0x0135
#define WM_CTLCOLORDLG                  0x0136
#define WM_CTLCOLORSCROLLBAR            0x0137
#define WM_CTLCOLORSTATIC               0x0138
#define MN_GETHMENU                     0x01E1

#define WM_MOUSEFIRST                   0x0200
#define WM_MOUSEMOVE                    0x0200
#define WM_LBUTTONDOWN                  0x0201
#define WM_LBUTTONUP                    0x0202
#define WM_LBUTTONDBLCLK                0x0203
#define WM_RBUTTONDOWN                  0x0204
#define WM_RBUTTONUP                    0x0205
#define WM_RBUTTONDBLCLK                0x0206
#define WM_MBUTTONDOWN                  0x0207
#define WM_MBUTTONUP                    0x0208
#define WM_MBUTTONDBLCLK                0x0209
#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
#define WM_MOUSEWHEEL                   0x020A
#endif
#if (_WIN32_WINNT >= 0x0500)
#define WM_XBUTTONDOWN                  0x020B
#define WM_XBUTTONUP                    0x020C
#define WM_XBUTTONDBLCLK                0x020D
#endif
#if (_WIN32_WINNT >= 0x0500)
#define WM_MOUSELAST                    0x020D
#elif (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
#define WM_MOUSELAST                    0x020A
#else
#define WM_MOUSELAST                    0x0209
#endif /* (_WIN32_WINNT >= 0x0500) */


#if(_WIN32_WINNT >= 0x0400)
/* Value for rolling one detent */
#define WHEEL_DELTA                     120
#define GET_WHEEL_DELTA_WPARAM(wParam)  ((short)HIWORD(wParam))

/* Setting to scroll one page for SPI_GET/SETWHEELSCROLLLINES */
#define WHEEL_PAGESCROLL                (UINT_MAX)
#endif /* _WIN32_WINNT >= 0x0400 */

#if(_WIN32_WINNT >= 0x0500)
#define GET_KEYSTATE_WPARAM(wParam)     (LOWORD(wParam))
#define GET_NCHITTEST_WPARAM(wParam)    ((short)LOWORD(wParam))
#define GET_XBUTTON_WPARAM(wParam)      (HIWORD(wParam))

/* XButton values are WORD flags */
#define XBUTTON1      0x0001
#define XBUTTON2      0x0002
/* Were there to be an XBUTTON3, its value would be 0x0004 */
#endif /* _WIN32_WINNT >= 0x0500 */

#define WM_PARENTNOTIFY                 0x0210
#define WM_ENTERMENULOOP                0x0211
#define WM_EXITMENULOOP                 0x0212

#if(WINVER >= 0x0400)
#define WM_NEXTMENU                     0x0213
#define WM_SIZING                       0x0214
#define WM_CAPTURECHANGED               0x0215
#define WM_MOVING                       0x0216
#endif /* WINVER >= 0x0400 */

#if(WINVER >= 0x0400)


#define WM_POWERBROADCAST               0x0218

#ifndef _WIN32_WCE
#define PBT_APMQUERYSUSPEND             0x0000
#define PBT_APMQUERYSTANDBY             0x0001

#define PBT_APMQUERYSUSPENDFAILED       0x0002
#define PBT_APMQUERYSTANDBYFAILED       0x0003

#define PBT_APMSUSPEND                  0x0004
#define PBT_APMSTANDBY                  0x0005

#define PBT_APMRESUMECRITICAL           0x0006
#define PBT_APMRESUMESUSPEND            0x0007
#define PBT_APMRESUMESTANDBY            0x0008

#define PBTF_APMRESUMEFROMFAILURE       0x00000001

#define PBT_APMBATTERYLOW               0x0009
#define PBT_APMPOWERSTATUSCHANGE        0x000A

#define PBT_APMOEMEVENT                 0x000B
#define PBT_APMRESUMEAUTOMATIC          0x0012
#endif

#endif /* WINVER >= 0x0400 */

#if(WINVER >= 0x0400)
#define WM_DEVICECHANGE                 0x0219
#endif /* WINVER >= 0x0400 */

#define WM_MDICREATE                    0x0220
#define WM_MDIDESTROY                   0x0221
#define WM_MDIACTIVATE                  0x0222
#define WM_MDIRESTORE                   0x0223
#define WM_MDINEXT                      0x0224
#define WM_MDIMAXIMIZE                  0x0225
#define WM_MDITILE                      0x0226
#define WM_MDICASCADE                   0x0227
#define WM_MDIICONARRANGE               0x0228
#define WM_MDIGETACTIVE                 0x0229


#define WM_MDISETMENU                   0x0230
#define WM_ENTERSIZEMOVE                0x0231
#define WM_EXITSIZEMOVE                 0x0232
#define WM_DROPFILES                    0x0233
#define WM_MDIREFRESHMENU               0x0234


#if(WINVER >= 0x0400)
#define WM_IME_SETCONTEXT               0x0281
#define WM_IME_NOTIFY                   0x0282
#define WM_IME_CONTROL                  0x0283
#define WM_IME_COMPOSITIONFULL          0x0284
#define WM_IME_SELECT                   0x0285
#define WM_IME_CHAR                     0x0286
#endif /* WINVER >= 0x0400 */
#if(WINVER >= 0x0500)
#define WM_IME_REQUEST                  0x0288
#endif /* WINVER >= 0x0500 */
#if(WINVER >= 0x0400)
#define WM_IME_KEYDOWN                  0x0290
#define WM_IME_KEYUP                    0x0291
#endif /* WINVER >= 0x0400 */

#if((_WIN32_WINNT >= 0x0400) || (WINVER >= 0x0500))
#define WM_MOUSEHOVER                   0x02A1
#define WM_MOUSELEAVE                   0x02A3
#endif
#if(WINVER >= 0x0500)
#define WM_NCMOUSEHOVER                 0x02A0
#define WM_NCMOUSELEAVE                 0x02A2
#endif /* WINVER >= 0x0500 */

#if(_WIN32_WINNT >= 0x0501)
#define WM_WTSSESSION_CHANGE            0x02B1

#define WM_TABLET_FIRST                 0x02c0
#define WM_TABLET_LAST                  0x02df
#endif /* _WIN32_WINNT >= 0x0501 */

#define WM_CUT                          0x0300
#define WM_COPY                         0x0301
#define WM_PASTE                        0x0302
#define WM_CLEAR                        0x0303
#define WM_UNDO                         0x0304
#define WM_RENDERFORMAT                 0x0305
#define WM_RENDERALLFORMATS             0x0306
#define WM_DESTROYCLIPBOARD             0x0307
#define WM_DRAWCLIPBOARD                0x0308
#define WM_PAINTCLIPBOARD               0x0309
#define WM_VSCROLLCLIPBOARD             0x030A
#define WM_SIZECLIPBOARD                0x030B
#define WM_ASKCBFORMATNAME              0x030C
#define WM_CHANGECBCHAIN                0x030D
#define WM_HSCROLLCLIPBOARD             0x030E
#define WM_QUERYNEWPALETTE              0x030F
#define WM_PALETTEISCHANGING            0x0310
#define WM_PALETTECHANGED               0x0311
#define WM_HOTKEY                       0x0312

#if(WINVER >= 0x0400)
#define WM_PRINT                        0x0317
#define WM_PRINTCLIENT                  0x0318
#endif /* WINVER >= 0x0400 */

#if(_WIN32_WINNT >= 0x0500)
#define WM_APPCOMMAND                   0x0319
#endif /* _WIN32_WINNT >= 0x0500 */

#if(_WIN32_WINNT >= 0x0501)
#define WM_THEMECHANGED                 0x031A
#endif /* _WIN32_WINNT >= 0x0501 */


#if(WINVER >= 0x0400)

#define WM_HANDHELDFIRST                0x0358
#define WM_HANDHELDLAST                 0x035F

#define WM_AFXFIRST                     0x0360
#define WM_AFXLAST                      0x037F
#endif /* WINVER >= 0x0400 */

#define WM_PENWINFIRST                  0x0380
#define WM_PENWINLAST                   0x038F


#if(WINVER >= 0x0400)
#define WM_APP                          0x8000
#endif /* WINVER >= 0x0400 */


/*
 * NOTE: All Message Numbers below 0x0400 are RESERVED.
 *
 * Private Window Messages Start Here:
 */
#define WM_USER                         0x0400

#if(WINVER >= 0x0400)

/*  wParam for WM_SIZING message  */
#define WMSZ_LEFT           1
#define WMSZ_RIGHT          2
#define WMSZ_TOP            3
#define WMSZ_TOPLEFT        4
#define WMSZ_TOPRIGHT       5
#define WMSZ_BOTTOM         6
#define WMSZ_BOTTOMLEFT     7
#define WMSZ_BOTTOMRIGHT    8
#endif /* WINVER >= 0x0400 */

#ifndef NONCMESSAGES

/*
 * WM_NCHITTEST and MOUSEHOOKSTRUCT Mouse Position Codes
 */
#define HTERROR             (-2)
#define HTTRANSPARENT       (-1)
#define HTNOWHERE           0
#define HTCLIENT            1
#define HTCAPTION           2
#define HTSYSMENU           3
#define HTGROWBOX           4
#define HTSIZE              HTGROWBOX
#define HTMENU              5
#define HTHSCROLL           6
#define HTVSCROLL           7
#define HTMINBUTTON         8
#define HTMAXBUTTON         9
#define HTLEFT              10
#define HTRIGHT             11
#define HTTOP               12
#define HTTOPLEFT           13
#define HTTOPRIGHT          14
#define HTBOTTOM            15
#define HTBOTTOMLEFT        16
#define HTBOTTOMRIGHT       17
#define HTBORDER            18
#define HTREDUCE            HTMINBUTTON
#define HTZOOM              HTMAXBUTTON
#define HTSIZEFIRST         HTLEFT
#define HTSIZELAST          HTBOTTOMRIGHT
#if(WINVER >= 0x0400)
#define HTOBJECT            19
#define HTCLOSE             20
#define HTHELP              21
#endif /* WINVER >= 0x0400 */


/*
 * SendMessageTimeout values
 */
#define SMTO_NORMAL         0x0000
#define SMTO_BLOCK          0x0001
#define SMTO_ABORTIFHUNG    0x0002
#if(WINVER >= 0x0500)
#define SMTO_NOTIMEOUTIFNOTHUNG 0x0008
#endif /* WINVER >= 0x0500 */
#endif /* !NONCMESSAGES */

/*
 * WM_MOUSEACTIVATE Return Codes
 */
#define MA_ACTIVATE         1
#define MA_ACTIVATEANDEAT   2
#define MA_NOACTIVATE       3
#define MA_NOACTIVATEANDEAT 4

/*
 * WM_SETICON / WM_GETICON Type Codes
 */
#define ICON_SMALL          0
#define ICON_BIG            1
#if(_WIN32_WINNT >= 0x0501)
#define ICON_SMALL2         2
#endif /* _WIN32_WINNT >= 0x0501 */


WINUSERAPI
UINT
WINAPI
RegisterWindowMessageA(
    __in LPCSTR lpString);
WINUSERAPI
UINT
WINAPI
RegisterWindowMessageW(
    __in LPCWSTR lpString);
#ifdef UNICODE
#define RegisterWindowMessage  RegisterWindowMessageW
#else
#define RegisterWindowMessage  RegisterWindowMessageA
#endif // !UNICODE


/*
 * WM_SIZE message wParam values
 */
#define SIZE_RESTORED       0
#define SIZE_MINIMIZED      1
#define SIZE_MAXIMIZED      2
#define SIZE_MAXSHOW        3
#define SIZE_MAXHIDE        4

/*
 * Obsolete constant names
 */
#define SIZENORMAL          SIZE_RESTORED
#define SIZEICONIC          SIZE_MINIMIZED
#define SIZEFULLSCREEN      SIZE_MAXIMIZED
#define SIZEZOOMSHOW        SIZE_MAXSHOW
#define SIZEZOOMHIDE        SIZE_MAXHIDE

/*
 * WM_WINDOWPOSCHANGING/CHANGED struct pointed to by lParam
 */
typedef struct tagWINDOWPOS {
    HWND    hwnd;
    HWND    hwndInsertAfter;
    int     x;
    int     y;
    int     cx;
    int     cy;
    UINT    flags;
} WINDOWPOS, *LPWINDOWPOS, *PWINDOWPOS;

/*
 * WM_NCCALCSIZE parameter structure
 */
typedef struct tagNCCALCSIZE_PARAMS {
    RECT       rgrc[3];
    PWINDOWPOS lppos;
} NCCALCSIZE_PARAMS, *LPNCCALCSIZE_PARAMS;

/*
 * WM_NCCALCSIZE "window valid rect" return values
 */
#define WVR_ALIGNTOP        0x0010
#define WVR_ALIGNLEFT       0x0020
#define WVR_ALIGNBOTTOM     0x0040
#define WVR_ALIGNRIGHT      0x0080
#define WVR_HREDRAW         0x0100
#define WVR_VREDRAW         0x0200
#define WVR_REDRAW         (WVR_HREDRAW | \
                            WVR_VREDRAW)
#define WVR_VALIDRECTS      0x0400


#ifndef NOKEYSTATES

/*
 * Key State Masks for Mouse Messages
 */
#define MK_LBUTTON          0x0001
#define MK_RBUTTON          0x0002
#define MK_SHIFT            0x0004
#define MK_CONTROL          0x0008
#define MK_MBUTTON          0x0010
#if(_WIN32_WINNT >= 0x0500)
#define MK_XBUTTON1         0x0020
#define MK_XBUTTON2         0x0040
#endif /* _WIN32_WINNT >= 0x0500 */

#endif /* !NOKEYSTATES */


#if(_WIN32_WINNT >= 0x0400)
#ifndef NOTRACKMOUSEEVENT

#define TME_HOVER       0x00000001
#define TME_LEAVE       0x00000002
#if(WINVER >= 0x0500)
#define TME_NONCLIENT   0x00000010
#endif /* WINVER >= 0x0500 */
#define TME_QUERY       0x40000000
#define TME_CANCEL      0x80000000


#define HOVER_DEFAULT   0xFFFFFFFF
#endif /* _WIN32_WINNT >= 0x0400 */

#if(_WIN32_WINNT >= 0x0400)
typedef struct tagTRACKMOUSEEVENT {
    DWORD cbSize;
    DWORD dwFlags;
    HWND  hwndTrack;
    DWORD dwHoverTime;
} TRACKMOUSEEVENT, *LPTRACKMOUSEEVENT;

WINUSERAPI
BOOL
WINAPI
TrackMouseEvent(
    __inout LPTRACKMOUSEEVENT lpEventTrack);
#endif /* _WIN32_WINNT >= 0x0400 */

#if(_WIN32_WINNT >= 0x0400)

#endif /* !NOTRACKMOUSEEVENT */
#endif /* _WIN32_WINNT >= 0x0400 */


#endif /* !NOWINMESSAGES */

#ifndef NOWINSTYLES


/*
 * Window Styles
 */
#define WS_OVERLAPPED       0x00000000L
#define WS_POPUP            0x80000000L
#define WS_CHILD            0x40000000L
#define WS_MINIMIZE         0x20000000L
#define WS_VISIBLE          0x10000000L
#define WS_DISABLED         0x08000000L
#define WS_CLIPSIBLINGS     0x04000000L
#define WS_CLIPCHILDREN     0x02000000L
#define WS_MAXIMIZE         0x01000000L
#define WS_CAPTION          0x00C00000L     /* WS_BORDER | WS_DLGFRAME  */
#define WS_BORDER           0x00800000L
#define WS_DLGFRAME         0x00400000L
#define WS_VSCROLL          0x00200000L
#define WS_HSCROLL          0x00100000L
#define WS_SYSMENU          0x00080000L
#define WS_THICKFRAME       0x00040000L
#define WS_GROUP            0x00020000L
#define WS_TABSTOP          0x00010000L

#define WS_MINIMIZEBOX      0x00020000L
#define WS_MAXIMIZEBOX      0x00010000L


#define WS_TILED            WS_OVERLAPPED
#define WS_ICONIC           WS_MINIMIZE
#define WS_SIZEBOX          WS_THICKFRAME
#define WS_TILEDWINDOW      WS_OVERLAPPEDWINDOW

/*
 * Common Window Styles
 */
#define WS_OVERLAPPEDWINDOW (WS_OVERLAPPED     | \
                             WS_CAPTION        | \
                             WS_SYSMENU        | \
                             WS_THICKFRAME     | \
                             WS_MINIMIZEBOX    | \
                             WS_MAXIMIZEBOX)

#define WS_POPUPWINDOW      (WS_POPUP          | \
                             WS_BORDER         | \
                             WS_SYSMENU)

#define WS_CHILDWINDOW      (WS_CHILD)

/*
 * Extended Window Styles
 */
#define WS_EX_DLGMODALFRAME     0x00000001L
#define WS_EX_NOPARENTNOTIFY    0x00000004L
#define WS_EX_TOPMOST           0x00000008L
#define WS_EX_ACCEPTFILES       0x00000010L
#define WS_EX_TRANSPARENT       0x00000020L
#if(WINVER >= 0x0400)
#define WS_EX_MDICHILD          0x00000040L
#define WS_EX_TOOLWINDOW        0x00000080L
#define WS_EX_WINDOWEDGE        0x00000100L
#define WS_EX_CLIENTEDGE        0x00000200L
#define WS_EX_CONTEXTHELP       0x00000400L

#endif /* WINVER >= 0x0400 */
#if(WINVER >= 0x0400)

#define WS_EX_RIGHT             0x00001000L
#define WS_EX_LEFT              0x00000000L
#define WS_EX_RTLREADING        0x00002000L
#define WS_EX_LTRREADING        0x00000000L
#define WS_EX_LEFTSCROLLBAR     0x00004000L
#define WS_EX_RIGHTSCROLLBAR    0x00000000L

#define WS_EX_CONTROLPARENT     0x00010000L
#define WS_EX_STATICEDGE        0x00020000L
#define WS_EX_APPWINDOW         0x00040000L


#define WS_EX_OVERLAPPEDWINDOW  (WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE)
#define WS_EX_PALETTEWINDOW     (WS_EX_WINDOWEDGE | WS_EX_TOOLWINDOW | WS_EX_TOPMOST)

#endif /* WINVER >= 0x0400 */

#if(_WIN32_WINNT >= 0x0500)
#define WS_EX_LAYERED           0x00080000

#endif /* _WIN32_WINNT >= 0x0500 */


#if(WINVER >= 0x0500)
#define WS_EX_NOINHERITLAYOUT   0x00100000L // Disable inheritence of mirroring by children
#define WS_EX_LAYOUTRTL         0x00400000L // Right to left mirroring
#endif /* WINVER >= 0x0500 */

#if(_WIN32_WINNT >= 0x0501)
#define WS_EX_COMPOSITED        0x02000000L
#endif /* _WIN32_WINNT >= 0x0501 */
#if(WINVER >= 0x0500)
#define WS_EX_NOACTIVATE        0x08000000L
#endif /* _WIN32_WINNT >= 0x0500 */


/*
 * Class styles
 */
#define CS_VREDRAW          0x0001
#define CS_HREDRAW          0x0002
#define CS_DBLCLKS          0x0008
#define CS_OWNDC            0x0020
#define CS_CLASSDC          0x0040
#define CS_PARENTDC         0x0080
#define CS_NOCLOSE          0x0200
#define CS_SAVEBITS         0x0800
#define CS_BYTEALIGNCLIENT  0x1000
#define CS_BYTEALIGNWINDOW  0x2000
#define CS_GLOBALCLASS      0x4000

#define CS_IME              0x00010000
#if(_WIN32_WINNT >= 0x0501)
#define CS_DROPSHADOW       0x00020000
#endif /* _WIN32_WINNT >= 0x0501 */



#endif /* !NOWINSTYLES */
#if(WINVER >= 0x0400)
/* WM_PRINT flags */
#define PRF_CHECKVISIBLE    0x00000001L
#define PRF_NONCLIENT       0x00000002L
#define PRF_CLIENT          0x00000004L
#define PRF_ERASEBKGND      0x00000008L
#define PRF_CHILDREN        0x00000010L
#define PRF_OWNED           0x00000020L

/* 3D border styles */
#define BDR_RAISEDOUTER 0x0001
#define BDR_SUNKENOUTER 0x0002
#define BDR_RAISEDINNER 0x0004
#define BDR_SUNKENINNER 0x0008

#define BDR_OUTER       (BDR_RAISEDOUTER | BDR_SUNKENOUTER)
#define BDR_INNER       (BDR_RAISEDINNER | BDR_SUNKENINNER)
#define BDR_RAISED      (BDR_RAISEDOUTER | BDR_RAISEDINNER)
#define BDR_SUNKEN      (BDR_SUNKENOUTER | BDR_SUNKENINNER)


#define EDGE_RAISED     (BDR_RAISEDOUTER | BDR_RAISEDINNER)
#define EDGE_SUNKEN     (BDR_SUNKENOUTER | BDR_SUNKENINNER)
#define EDGE_ETCHED     (BDR_SUNKENOUTER | BDR_RAISEDINNER)
#define EDGE_BUMP       (BDR_RAISEDOUTER | BDR_SUNKENINNER)

/* Border flags */
#define BF_LEFT         0x0001
#define BF_TOP          0x0002
#define BF_RIGHT        0x0004
#define BF_BOTTOM       0x0008

#define BF_TOPLEFT      (BF_TOP | BF_LEFT)
#define BF_TOPRIGHT     (BF_TOP | BF_RIGHT)
#define BF_BOTTOMLEFT   (BF_BOTTOM | BF_LEFT)
#define BF_BOTTOMRIGHT  (BF_BOTTOM | BF_RIGHT)
#define BF_RECT         (BF_LEFT | BF_TOP | BF_RIGHT | BF_BOTTOM)

#define BF_DIAGONAL     0x0010

// For diagonal lines, the BF_RECT flags specify the end point of the
// vector bounded by the rectangle parameter.
#define BF_DIAGONAL_ENDTOPRIGHT     (BF_DIAGONAL | BF_TOP | BF_RIGHT)
#define BF_DIAGONAL_ENDTOPLEFT      (BF_DIAGONAL | BF_TOP | BF_LEFT)
#define BF_DIAGONAL_ENDBOTTOMLEFT   (BF_DIAGONAL | BF_BOTTOM | BF_LEFT)
#define BF_DIAGONAL_ENDBOTTOMRIGHT  (BF_DIAGONAL | BF_BOTTOM | BF_RIGHT)


#define BF_MIDDLE       0x0800  /* Fill in the middle */
#define BF_SOFT         0x1000  /* For softer buttons */
#define BF_ADJUST       0x2000  /* Calculate the space left over */
#define BF_FLAT         0x4000  /* For flat rather than 3D borders */
#define BF_MONO         0x8000  /* For monochrome borders */


WINUSERAPI
BOOL
WINAPI
DrawEdge(
    __in HDC hdc,
    __inout LPRECT qrc,
    __in UINT edge,
    __in UINT grfFlags);

/* flags for DrawFrameControl */

#define DFC_CAPTION             1
#define DFC_MENU                2
#define DFC_SCROLL              3
#define DFC_BUTTON              4
#if(WINVER >= 0x0500)
#define DFC_POPUPMENU           5
#endif /* WINVER >= 0x0500 */

#define DFCS_CAPTIONCLOSE       0x0000
#define DFCS_CAPTIONMIN         0x0001
#define DFCS_CAPTIONMAX         0x0002
#define DFCS_CAPTIONRESTORE     0x0003
#define DFCS_CAPTIONHELP        0x0004

#define DFCS_MENUARROW          0x0000
#define DFCS_MENUCHECK          0x0001
#define DFCS_MENUBULLET         0x0002
#define DFCS_MENUARROWRIGHT     0x0004
#define DFCS_SCROLLUP           0x0000
#define DFCS_SCROLLDOWN         0x0001
#define DFCS_SCROLLLEFT         0x0002
#define DFCS_SCROLLRIGHT        0x0003
#define DFCS_SCROLLCOMBOBOX     0x0005
#define DFCS_SCROLLSIZEGRIP     0x0008
#define DFCS_SCROLLSIZEGRIPRIGHT 0x0010

#define DFCS_BUTTONCHECK        0x0000
#define DFCS_BUTTONRADIOIMAGE   0x0001
#define DFCS_BUTTONRADIOMASK    0x0002
#define DFCS_BUTTONRADIO        0x0004
#define DFCS_BUTTON3STATE       0x0008
#define DFCS_BUTTONPUSH         0x0010

#define DFCS_INACTIVE           0x0100
#define DFCS_PUSHED             0x0200
#define DFCS_CHECKED            0x0400

#if(WINVER >= 0x0500)
#define DFCS_TRANSPARENT        0x0800
#define DFCS_HOT                0x1000
#endif /* WINVER >= 0x0500 */

#define DFCS_ADJUSTRECT         0x2000
#define DFCS_FLAT               0x4000
#define DFCS_MONO               0x8000

WINUSERAPI
BOOL
WINAPI
DrawFrameControl(
    __in HDC,
    __inout LPRECT,
    __in UINT,
    __in UINT);


/* flags for DrawCaption */
#define DC_ACTIVE           0x0001
#define DC_SMALLCAP         0x0002
#define DC_ICON             0x0004
#define DC_TEXT             0x0008
#define DC_INBUTTON         0x0010
#if(WINVER >= 0x0500)
#define DC_GRADIENT         0x0020
#endif /* WINVER >= 0x0500 */
#if(_WIN32_WINNT >= 0x0501)
#define DC_BUTTONS          0x1000
#endif /* _WIN32_WINNT >= 0x0501 */

WINUSERAPI
BOOL
WINAPI
DrawCaption(
    __in HWND hwnd,
    __in HDC hdc,
    __in CONST RECT * lprect,
    __in UINT flags);


#define IDANI_OPEN          1
#define IDANI_CAPTION       3

WINUSERAPI
BOOL
WINAPI
DrawAnimatedRects(
    __in_opt HWND hwnd,
    __in int idAni,
    __in CONST RECT *lprcFrom,
    __in CONST RECT *lprcTo);

#endif /* WINVER >= 0x0400 */

#ifndef NOCLIPBOARD


/*
 * Predefined Clipboard Formats
 */
#define CF_TEXT             1
#define CF_BITMAP           2
#define CF_METAFILEPICT     3
#define CF_SYLK             4
#define CF_DIF              5
#define CF_TIFF             6
#define CF_OEMTEXT          7
#define CF_DIB              8
#define CF_PALETTE          9
#define CF_PENDATA          10
#define CF_RIFF             11
#define CF_WAVE             12
#define CF_UNICODETEXT      13
#define CF_ENHMETAFILE      14
#if(WINVER >= 0x0400)
#define CF_HDROP            15
#define CF_LOCALE           16
#endif /* WINVER >= 0x0400 */
#if(WINVER >= 0x0500)
#define CF_DIBV5            17
#endif /* WINVER >= 0x0500 */

#if(WINVER >= 0x0500)
#define CF_MAX              18
#elif(WINVER >= 0x0400)
#define CF_MAX              17
#else
#define CF_MAX              15
#endif

#define CF_OWNERDISPLAY     0x0080
#define CF_DSPTEXT          0x0081
#define CF_DSPBITMAP        0x0082
#define CF_DSPMETAFILEPICT  0x0083
#define CF_DSPENHMETAFILE   0x008E

/*
 * "Private" formats don't get GlobalFree()'d
 */
#define CF_PRIVATEFIRST     0x0200
#define CF_PRIVATELAST      0x02FF

/*
 * "GDIOBJ" formats do get DeleteObject()'d
 */
#define CF_GDIOBJFIRST      0x0300
#define CF_GDIOBJLAST       0x03FF


#endif /* !NOCLIPBOARD */

/*
 * Defines for the fVirt field of the Accelerator table structure.
 */
#define FVIRTKEY  TRUE          /* Assumed to be == TRUE */
#define FNOINVERT 0x02
#define FSHIFT    0x04
#define FCONTROL  0x08
#define FALT      0x10

typedef struct tagACCEL {
#ifndef _MAC
    BYTE   fVirt;               /* Also called the flags field */
    WORD   key;
    WORD   cmd;
#else
    WORD   fVirt;               /* Also called the flags field */
    WORD   key;
    DWORD  cmd;
#endif
} ACCEL, *LPACCEL;

typedef struct tagPAINTSTRUCT {
    HDC         hdc;
    BOOL        fErase;
    RECT        rcPaint;
    BOOL        fRestore;
    BOOL        fIncUpdate;
    BYTE        rgbReserved[32];
} PAINTSTRUCT, *PPAINTSTRUCT, *NPPAINTSTRUCT, *LPPAINTSTRUCT;

typedef struct tagCREATESTRUCTA {
    LPVOID      lpCreateParams;
    HINSTANCE   hInstance;
    HMENU       hMenu;
    HWND        hwndParent;
    int         cy;
    int         cx;
    int         y;
    int         x;
    LONG        style;
    LPCSTR      lpszName;
    LPCSTR      lpszClass;
    DWORD       dwExStyle;
} CREATESTRUCTA, *LPCREATESTRUCTA;
typedef struct tagCREATESTRUCTW {
    LPVOID      lpCreateParams;
    HINSTANCE   hInstance;
    HMENU       hMenu;
    HWND        hwndParent;
    int         cy;
    int         cx;
    int         y;
    int         x;
    LONG        style;
    LPCWSTR     lpszName;
    LPCWSTR     lpszClass;
    DWORD       dwExStyle;
} CREATESTRUCTW, *LPCREATESTRUCTW;
#ifdef UNICODE
typedef CREATESTRUCTW CREATESTRUCT;
typedef LPCREATESTRUCTW LPCREATESTRUCT;
#else
typedef CREATESTRUCTA CREATESTRUCT;
typedef LPCREATESTRUCTA LPCREATESTRUCT;
#endif // UNICODE

typedef struct tagWINDOWPLACEMENT {
    UINT  length;
    UINT  flags;
    UINT  showCmd;
    POINT ptMinPosition;
    POINT ptMaxPosition;
    RECT  rcNormalPosition;
#ifdef _MAC
    RECT  rcDevice;
#endif
} WINDOWPLACEMENT;
typedef WINDOWPLACEMENT *PWINDOWPLACEMENT, *LPWINDOWPLACEMENT;

#define WPF_SETMINPOSITION          0x0001
#define WPF_RESTORETOMAXIMIZED      0x0002
#if(_WIN32_WINNT >= 0x0500)
#define WPF_ASYNCWINDOWPLACEMENT    0x0004
#endif /* _WIN32_WINNT >= 0x0500 */

#if(WINVER >= 0x0400)
typedef struct tagNMHDR
{
    HWND      hwndFrom;
    UINT_PTR  idFrom;
    UINT      code;         // NM_ code
}   NMHDR;
typedef NMHDR FAR * LPNMHDR;

typedef struct tagSTYLESTRUCT
{
    DWORD   styleOld;
    DWORD   styleNew;
} STYLESTRUCT, * LPSTYLESTRUCT;
#endif /* WINVER >= 0x0400 */


/*
 * Owner draw control types
 */
#define ODT_MENU        1
#define ODT_LISTBOX     2
#define ODT_COMBOBOX    3
#define ODT_BUTTON      4
#if(WINVER >= 0x0400)
#define ODT_STATIC      5
#endif /* WINVER >= 0x0400 */

/*
 * Owner draw actions
 */
#define ODA_DRAWENTIRE  0x0001
#define ODA_SELECT      0x0002
#define ODA_FOCUS       0x0004

/*
 * Owner draw state
 */
#define ODS_SELECTED    0x0001
#define ODS_GRAYED      0x0002
#define ODS_DISABLED    0x0004
#define ODS_CHECKED     0x0008
#define ODS_FOCUS       0x0010
#if(WINVER >= 0x0400)
#define ODS_DEFAULT         0x0020
#define ODS_COMBOBOXEDIT    0x1000
#endif /* WINVER >= 0x0400 */
#if(WINVER >= 0x0500)
#define ODS_HOTLIGHT        0x0040
#define ODS_INACTIVE        0x0080
#if(_WIN32_WINNT >= 0x0500)
#define ODS_NOACCEL         0x0100
#define ODS_NOFOCUSRECT     0x0200
#endif /* _WIN32_WINNT >= 0x0500 */
#endif /* WINVER >= 0x0500 */

/*
 * MEASUREITEMSTRUCT for ownerdraw
 */
typedef struct tagMEASUREITEMSTRUCT {
    UINT       CtlType;
    UINT       CtlID;
    UINT       itemID;
    UINT       itemWidth;
    UINT       itemHeight;
    ULONG_PTR  itemData;
} MEASUREITEMSTRUCT, NEAR *PMEASUREITEMSTRUCT, FAR *LPMEASUREITEMSTRUCT;


/*
 * DRAWITEMSTRUCT for ownerdraw
 */
typedef struct tagDRAWITEMSTRUCT {
    UINT        CtlType;
    UINT        CtlID;
    UINT        itemID;
    UINT        itemAction;
    UINT        itemState;
    HWND        hwndItem;
    HDC         hDC;
    RECT        rcItem;
    ULONG_PTR   itemData;
} DRAWITEMSTRUCT, NEAR *PDRAWITEMSTRUCT, FAR *LPDRAWITEMSTRUCT;

/*
 * DELETEITEMSTRUCT for ownerdraw
 */
typedef struct tagDELETEITEMSTRUCT {
    UINT       CtlType;
    UINT       CtlID;
    UINT       itemID;
    HWND       hwndItem;
    ULONG_PTR  itemData;
} DELETEITEMSTRUCT, NEAR *PDELETEITEMSTRUCT, FAR *LPDELETEITEMSTRUCT;

/*
 * COMPAREITEMSTUCT for ownerdraw sorting
 */
typedef struct tagCOMPAREITEMSTRUCT {
    UINT        CtlType;
    UINT        CtlID;
    HWND        hwndItem;
    UINT        itemID1;
    ULONG_PTR   itemData1;
    UINT        itemID2;
    ULONG_PTR   itemData2;
    DWORD       dwLocaleId;
} COMPAREITEMSTRUCT, NEAR *PCOMPAREITEMSTRUCT, FAR *LPCOMPAREITEMSTRUCT;

#ifndef NOMSG

/*
 * Message Function Templates
 */

WINUSERAPI
BOOL
WINAPI
GetMessageA(
    __out LPMSG lpMsg,
    __in_opt HWND hWnd,
    __in UINT wMsgFilterMin,
    __in UINT wMsgFilterMax);
WINUSERAPI
BOOL
WINAPI
GetMessageW(
    __out LPMSG lpMsg,
    __in_opt HWND hWnd,
    __in UINT wMsgFilterMin,
    __in UINT wMsgFilterMax);
#ifdef UNICODE
#define GetMessage  GetMessageW
#else
#define GetMessage  GetMessageA
#endif // !UNICODE
    
#if defined(_M_CEE)
#undef GetMessage
__inline
BOOL
GetMessage(
    LPMSG lpMsg,
    HWND hWnd,
    UINT wMsgFilterMin,
    UINT wMsgFilterMax
    )
{
#ifdef UNICODE
    return GetMessageW(
#else
    return GetMessageA(
#endif
        lpMsg,
        hWnd,
        wMsgFilterMin,
        wMsgFilterMax
        );
}
#endif	/* _M_CEE */


WINUSERAPI
BOOL
WINAPI
TranslateMessage(
    __in CONST MSG *lpMsg);

WINUSERAPI
LRESULT
WINAPI
DispatchMessageA(
    __in CONST MSG *lpMsg);
WINUSERAPI
LRESULT
WINAPI
DispatchMessageW(
    __in CONST MSG *lpMsg);
#ifdef UNICODE
#define DispatchMessage  DispatchMessageW
#else
#define DispatchMessage  DispatchMessageA
#endif // !UNICODE
    
#if defined(_M_CEE)
#undef DispatchMessage
__inline
LRESULT
DispatchMessage(
    CONST MSG *lpMsg
    )
{
#ifdef UNICODE
    return DispatchMessageW(
#else
    return DispatchMessageA(
#endif
        lpMsg
        );
}
#endif	/* _M_CEE */

WINUSERAPI
BOOL
WINAPI
SetMessageQueue(
    __in int cMessagesMax);

WINUSERAPI
BOOL
WINAPI
PeekMessageA(
    __out LPMSG lpMsg,
    __in_opt HWND hWnd,
    __in UINT wMsgFilterMin,
    __in UINT wMsgFilterMax,
    __in UINT wRemoveMsg);
WINUSERAPI
BOOL
WINAPI
PeekMessageW(
    __out LPMSG lpMsg,
    __in_opt HWND hWnd,
    __in UINT wMsgFilterMin,
    __in UINT wMsgFilterMax,
    __in UINT wRemoveMsg);
#ifdef UNICODE
#define PeekMessage  PeekMessageW
#else
#define PeekMessage  PeekMessageA
#endif // !UNICODE


/*
 * PeekMessage() Options
 */
#define PM_NOREMOVE         0x0000
#define PM_REMOVE           0x0001
#define PM_NOYIELD          0x0002
#if(WINVER >= 0x0500)
#define PM_QS_INPUT         (QS_INPUT << 16)
#define PM_QS_POSTMESSAGE   ((QS_POSTMESSAGE | QS_HOTKEY | QS_TIMER) << 16)
#define PM_QS_PAINT         (QS_PAINT << 16)
#define PM_QS_SENDMESSAGE   (QS_SENDMESSAGE << 16)
#endif /* WINVER >= 0x0500 */


#endif /* !NOMSG */

WINUSERAPI
BOOL
WINAPI
RegisterHotKey(
    __in_opt HWND hWnd,
    __in int id,
    __in UINT fsModifiers,
    __in UINT vk);

WINUSERAPI
BOOL
WINAPI
UnregisterHotKey(
    __in_opt HWND hWnd,
    __in int id);

#define MOD_ALT         0x0001
#define MOD_CONTROL     0x0002
#define MOD_SHIFT       0x0004
#define MOD_WIN         0x0008


#define IDHOT_SNAPWINDOW        (-1)    /* SHIFT-PRINTSCRN  */
#define IDHOT_SNAPDESKTOP       (-2)    /* PRINTSCRN        */

#ifdef WIN_INTERNAL
    #ifndef LSTRING
    #define NOLSTRING
    #endif /* LSTRING */
    #ifndef LFILEIO
    #define NOLFILEIO
    #endif /* LFILEIO */
#endif /* WIN_INTERNAL */

#if(WINVER >= 0x0400)

#define ENDSESSION_LOGOFF    0x80000000

#endif /* WINVER >= 0x0400 */

#define EWX_LOGOFF          0
#define EWX_SHUTDOWN        0x00000001
#define EWX_REBOOT          0x00000002
#define EWX_FORCE           0x00000004
#define EWX_POWEROFF        0x00000008
#if(_WIN32_WINNT >= 0x0500)
#define EWX_FORCEIFHUNG     0x00000010
#endif /* _WIN32_WINNT >= 0x0500 */


#define ExitWindows(dwReserved, Code) ExitWindowsEx(EWX_LOGOFF, 0xFFFFFFFF)

WINUSERAPI
BOOL
WINAPI
ExitWindowsEx(
    __in UINT uFlags,
    __in DWORD dwReason);

WINUSERAPI
BOOL
WINAPI
SwapMouseButton(
    __in BOOL fSwap);

WINUSERAPI
DWORD
WINAPI
GetMessagePos(
    VOID);

WINUSERAPI
LONG
WINAPI
GetMessageTime(
    VOID);

WINUSERAPI
LPARAM
WINAPI
GetMessageExtraInfo(
    VOID);

#if(_WIN32_WINNT >= 0x0501)
WINUSERAPI
BOOL
WINAPI
IsWow64Message(
    VOID);
#endif /* _WIN32_WINNT >= 0x0501 */

#if(WINVER >= 0x0400)
WINUSERAPI
LPARAM
WINAPI
SetMessageExtraInfo(
    __in LPARAM lParam);
#endif /* WINVER >= 0x0400 */

WINUSERAPI
LRESULT
WINAPI
SendMessageA(
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
WINUSERAPI
LRESULT
WINAPI
SendMessageW(
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
#ifdef UNICODE
#define SendMessage  SendMessageW
#else
#define SendMessage  SendMessageA
#endif // !UNICODE
    
#if defined(_M_CEE)
#undef SendMessage
__inline
LRESULT
SendMessage(
    HWND hWnd,
    UINT Msg,
    WPARAM wParam,
    LPARAM lParam
    )
{
#ifdef UNICODE
    return SendMessageW(
#else
    return SendMessageA(
#endif
        hWnd,
        Msg,
        wParam,
        lParam
        );
}
#endif	/* _M_CEE */



WINUSERAPI
LRESULT
WINAPI
SendMessageTimeoutA(
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam,
    __in UINT fuFlags,
    __in UINT uTimeout,
    __out_opt PDWORD_PTR lpdwResult);
WINUSERAPI
LRESULT
WINAPI
SendMessageTimeoutW(
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam,
    __in UINT fuFlags,
    __in UINT uTimeout,
    __out_opt PDWORD_PTR lpdwResult);
#ifdef UNICODE
#define SendMessageTimeout  SendMessageTimeoutW
#else
#define SendMessageTimeout  SendMessageTimeoutA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
SendNotifyMessageA(
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
WINUSERAPI
BOOL
WINAPI
SendNotifyMessageW(
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
#ifdef UNICODE
#define SendNotifyMessage  SendNotifyMessageW
#else
#define SendNotifyMessage  SendNotifyMessageA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
SendMessageCallbackA(
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam,
    __in SENDASYNCPROC lpResultCallBack,
    __in ULONG_PTR dwData);
WINUSERAPI
BOOL
WINAPI
SendMessageCallbackW(
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam,
    __in SENDASYNCPROC lpResultCallBack,
    __in ULONG_PTR dwData);
#ifdef UNICODE
#define SendMessageCallback  SendMessageCallbackW
#else
#define SendMessageCallback  SendMessageCallbackA
#endif // !UNICODE

#if(_WIN32_WINNT >= 0x0501)
typedef struct {
    UINT  cbSize;
    HDESK hdesk;
    HWND  hwnd;
    LUID  luid;
} BSMINFO, *PBSMINFO;

WINUSERAPI
long
WINAPI
BroadcastSystemMessageExA(
    __in DWORD flags,
    __inout_opt LPDWORD lpInfo,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam,
    __out_opt PBSMINFO pbsmInfo);
WINUSERAPI
long
WINAPI
BroadcastSystemMessageExW(
    __in DWORD flags,
    __inout_opt LPDWORD lpInfo,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam,
    __out_opt PBSMINFO pbsmInfo);
#ifdef UNICODE
#define BroadcastSystemMessageEx  BroadcastSystemMessageExW
#else
#define BroadcastSystemMessageEx  BroadcastSystemMessageExA
#endif // !UNICODE
#endif /* _WIN32_WINNT >= 0x0501 */

#if(WINVER >= 0x0400)

#if defined(_WIN32_WINNT)
WINUSERAPI
long
WINAPI
BroadcastSystemMessageA(
    __in DWORD flags,
    __inout_opt LPDWORD lpInfo,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
WINUSERAPI
long
WINAPI
BroadcastSystemMessageW(
    __in DWORD flags,
    __inout_opt LPDWORD lpInfo,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
#ifdef UNICODE
#define BroadcastSystemMessage  BroadcastSystemMessageW
#else
#define BroadcastSystemMessage  BroadcastSystemMessageA
#endif // !UNICODE
#elif defined(_WIN32_WINDOWS)
// The Win95 version isn't A/W decorated
WINUSERAPI
long
WINAPI
BroadcastSystemMessage(
    __in DWORD flags,
    __inout_opt LPDWORD lpInfo,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);

#endif

//Broadcast Special Message Recipient list
#define BSM_ALLCOMPONENTS       0x00000000
#define BSM_VXDS                0x00000001
#define BSM_NETDRIVER           0x00000002
#define BSM_INSTALLABLEDRIVERS  0x00000004
#define BSM_APPLICATIONS        0x00000008
#define BSM_ALLDESKTOPS         0x00000010

//Broadcast Special Message Flags
#define BSF_QUERY               0x00000001
#define BSF_IGNORECURRENTTASK   0x00000002
#define BSF_FLUSHDISK           0x00000004
#define BSF_NOHANG              0x00000008
#define BSF_POSTMESSAGE         0x00000010
#define BSF_FORCEIFHUNG         0x00000020
#define BSF_NOTIMEOUTIFNOTHUNG  0x00000040
#if(_WIN32_WINNT >= 0x0500)
#define BSF_ALLOWSFW            0x00000080
#define BSF_SENDNOTIFYMESSAGE   0x00000100
#endif /* _WIN32_WINNT >= 0x0500 */
#if(_WIN32_WINNT >= 0x0501)
#define BSF_RETURNHDESK         0x00000200
#define BSF_LUID                0x00000400
#endif /* _WIN32_WINNT >= 0x0501 */

#define BROADCAST_QUERY_DENY         0x424D5144  // Return this value to deny a query.
#endif /* WINVER >= 0x0400 */

// RegisterDeviceNotification

#if(WINVER >= 0x0500)
typedef  PVOID           HDEVNOTIFY;
typedef  HDEVNOTIFY     *PHDEVNOTIFY;

#define DEVICE_NOTIFY_WINDOW_HANDLE          0x00000000
#define DEVICE_NOTIFY_SERVICE_HANDLE         0x00000001
#if(_WIN32_WINNT >= 0x0501)
#define DEVICE_NOTIFY_ALL_INTERFACE_CLASSES  0x00000004
#endif /* _WIN32_WINNT >= 0x0501 */

WINUSERAPI
HDEVNOTIFY
WINAPI
RegisterDeviceNotificationA(
    __in HANDLE hRecipient,
    __in LPVOID NotificationFilter,
    __in DWORD Flags);
WINUSERAPI
HDEVNOTIFY
WINAPI
RegisterDeviceNotificationW(
    __in HANDLE hRecipient,
    __in LPVOID NotificationFilter,
    __in DWORD Flags);
#ifdef UNICODE
#define RegisterDeviceNotification  RegisterDeviceNotificationW
#else
#define RegisterDeviceNotification  RegisterDeviceNotificationA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
UnregisterDeviceNotification(
    __in HDEVNOTIFY Handle);

#endif /* WINVER >= 0x0500 */


WINUSERAPI
BOOL
WINAPI
PostMessageA(
    __in_opt HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
WINUSERAPI
BOOL
WINAPI
PostMessageW(
    __in_opt HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
#ifdef UNICODE
#define PostMessage  PostMessageW
#else
#define PostMessage  PostMessageA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
PostThreadMessageA(
    __in DWORD idThread,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
WINUSERAPI
BOOL
WINAPI
PostThreadMessageW(
    __in DWORD idThread,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
#ifdef UNICODE
#define PostThreadMessage  PostThreadMessageW
#else
#define PostThreadMessage  PostThreadMessageA
#endif // !UNICODE

#define PostAppMessageA(idThread, wMsg, wParam, lParam)\
        PostThreadMessageA((DWORD)idThread, wMsg, wParam, lParam)
#define PostAppMessageW(idThread, wMsg, wParam, lParam)\
        PostThreadMessageW((DWORD)idThread, wMsg, wParam, lParam)
#ifdef UNICODE
#define PostAppMessage  PostAppMessageW
#else
#define PostAppMessage  PostAppMessageA
#endif // !UNICODE

/*
 * Special HWND value for use with PostMessage() and SendMessage()
 */
#define HWND_BROADCAST  ((HWND)0xffff)

#if(WINVER >= 0x0500)
#define HWND_MESSAGE     ((HWND)-3)
#endif /* WINVER >= 0x0500 */

WINUSERAPI
BOOL
WINAPI
AttachThreadInput(
    __in DWORD idAttach,
    __in DWORD idAttachTo,
    __in BOOL fAttach);


WINUSERAPI
BOOL
WINAPI
ReplyMessage(
    __in LRESULT lResult);

WINUSERAPI
BOOL
WINAPI
WaitMessage(
    VOID);


WINUSERAPI
DWORD
WINAPI
WaitForInputIdle(
    __in HANDLE hProcess,
    __in DWORD dwMilliseconds);

WINUSERAPI
#ifndef _MAC
LRESULT
WINAPI
#else
LRESULT
CALLBACK
#endif
DefWindowProcA(
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
WINUSERAPI
#ifndef _MAC
LRESULT
WINAPI
#else
LRESULT
CALLBACK
#endif
DefWindowProcW(
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
#ifdef UNICODE
#define DefWindowProc  DefWindowProcW
#else
#define DefWindowProc  DefWindowProcA
#endif // !UNICODE

WINUSERAPI
VOID
WINAPI
PostQuitMessage(
    __in int nExitCode);

#ifdef STRICT

WINUSERAPI
LRESULT
WINAPI
CallWindowProcA(
    __in WNDPROC lpPrevWndFunc,
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
WINUSERAPI
LRESULT
WINAPI
CallWindowProcW(
    __in WNDPROC lpPrevWndFunc,
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
#ifdef UNICODE
#define CallWindowProc  CallWindowProcW
#else
#define CallWindowProc  CallWindowProcA
#endif // !UNICODE

#else /* !STRICT */

WINUSERAPI
LRESULT
WINAPI
CallWindowProcA(
    __in FARPROC lpPrevWndFunc,
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
WINUSERAPI
LRESULT
WINAPI
CallWindowProcW(
    __in FARPROC lpPrevWndFunc,
    __in HWND hWnd,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
#ifdef UNICODE
#define CallWindowProc  CallWindowProcW
#else
#define CallWindowProc  CallWindowProcA
#endif // !UNICODE

#endif /* !STRICT */

WINUSERAPI
BOOL
WINAPI
InSendMessage(
    VOID);

#if(WINVER >= 0x0500)
WINUSERAPI
DWORD
WINAPI
InSendMessageEx(
    __reserved LPVOID lpReserved);

/*
 * InSendMessageEx return value
 */
#define ISMEX_NOSEND      0x00000000
#define ISMEX_SEND        0x00000001
#define ISMEX_NOTIFY      0x00000002
#define ISMEX_CALLBACK    0x00000004
#define ISMEX_REPLIED     0x00000008
#endif /* WINVER >= 0x0500 */

WINUSERAPI
UINT
WINAPI
GetDoubleClickTime(
    VOID);

WINUSERAPI
BOOL
WINAPI
SetDoubleClickTime(
    __in UINT);

WINUSERAPI
ATOM
WINAPI
RegisterClassA(
    __in CONST WNDCLASSA *lpWndClass);
WINUSERAPI
ATOM
WINAPI
RegisterClassW(
    __in CONST WNDCLASSW *lpWndClass);
#ifdef UNICODE
#define RegisterClass  RegisterClassW
#else
#define RegisterClass  RegisterClassA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
UnregisterClassA(
    __in LPCSTR lpClassName,
    __in HINSTANCE hInstance);
WINUSERAPI
BOOL
WINAPI
UnregisterClassW(
    __in LPCWSTR lpClassName,
    __in HINSTANCE hInstance);
#ifdef UNICODE
#define UnregisterClass  UnregisterClassW
#else
#define UnregisterClass  UnregisterClassA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
GetClassInfoA(
    __in_opt HINSTANCE hInstance,
    __in LPCSTR lpClassName,
    __out LPWNDCLASSA lpWndClass);
WINUSERAPI
BOOL
WINAPI
GetClassInfoW(
    __in_opt HINSTANCE hInstance,
    __in LPCWSTR lpClassName,
    __out LPWNDCLASSW lpWndClass);
#ifdef UNICODE
#define GetClassInfo  GetClassInfoW
#else
#define GetClassInfo  GetClassInfoA
#endif // !UNICODE

#if(WINVER >= 0x0400)
WINUSERAPI
ATOM
WINAPI
RegisterClassExA(
    __in CONST WNDCLASSEXA *);
WINUSERAPI
ATOM
WINAPI
RegisterClassExW(
    __in CONST WNDCLASSEXW *);
#ifdef UNICODE
#define RegisterClassEx  RegisterClassExW
#else
#define RegisterClassEx  RegisterClassExA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
GetClassInfoExA(
    __in_opt HINSTANCE hInstance,
    __in LPCSTR lpszClass,
    __out LPWNDCLASSEXA lpwcx);
WINUSERAPI
BOOL
WINAPI
GetClassInfoExW(
    __in_opt HINSTANCE hInstance,
    __in LPCWSTR lpszClass,
    __out LPWNDCLASSEXW lpwcx);
#ifdef UNICODE
#define GetClassInfoEx  GetClassInfoExW
#else
#define GetClassInfoEx  GetClassInfoExA
#endif // !UNICODE

#endif /* WINVER >= 0x0400 */

#define CW_USEDEFAULT       ((int)0x80000000)

/*
 * Special value for CreateWindow, et al.
 */
#define HWND_DESKTOP        ((HWND)0)

#if(_WIN32_WINNT >= 0x0501)
typedef BOOLEAN (WINAPI * PREGISTERCLASSNAMEW)(LPCWSTR);
#endif /* _WIN32_WINNT >= 0x0501 */

WINUSERAPI
HWND
WINAPI
CreateWindowExA(
    __in DWORD dwExStyle,
    __in_opt LPCSTR lpClassName,
    __in_opt LPCSTR lpWindowName,
    __in DWORD dwStyle,
    __in int X,
    __in int Y,
    __in int nWidth,
    __in int nHeight,
    __in_opt HWND hWndParent,
    __in_opt HMENU hMenu,
    __in_opt HINSTANCE hInstance,
    __in_opt LPVOID lpParam);
WINUSERAPI
HWND
WINAPI
CreateWindowExW(
    __in DWORD dwExStyle,
    __in_opt LPCWSTR lpClassName,
    __in_opt LPCWSTR lpWindowName,
    __in DWORD dwStyle,
    __in int X,
    __in int Y,
    __in int nWidth,
    __in int nHeight,
    __in_opt HWND hWndParent,
    __in_opt HMENU hMenu,
    __in_opt HINSTANCE hInstance,
    __in_opt LPVOID lpParam);
#ifdef UNICODE
#define CreateWindowEx  CreateWindowExW
#else
#define CreateWindowEx  CreateWindowExA
#endif // !UNICODE

#define CreateWindowA(lpClassName, lpWindowName, dwStyle, x, y,\
nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)\
CreateWindowExA(0L, lpClassName, lpWindowName, dwStyle, x, y,\
nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)
#define CreateWindowW(lpClassName, lpWindowName, dwStyle, x, y,\
nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)\
CreateWindowExW(0L, lpClassName, lpWindowName, dwStyle, x, y,\
nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)
#ifdef UNICODE
#define CreateWindow  CreateWindowW
#else
#define CreateWindow  CreateWindowA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
IsWindow(
    __in_opt HWND hWnd);

WINUSERAPI
BOOL
WINAPI
IsMenu(
    __in HMENU hMenu);

WINUSERAPI
BOOL
WINAPI
IsChild(
    __in HWND hWndParent,
    __in HWND hWnd);

WINUSERAPI
BOOL
WINAPI
DestroyWindow(
    __in HWND hWnd);

WINUSERAPI
BOOL
WINAPI
ShowWindow(
    __in HWND hWnd,
    __in int nCmdShow);

#if(WINVER >= 0x0500)
WINUSERAPI
BOOL
WINAPI
AnimateWindow(
    __in HWND hWnd,
    __in DWORD dwTime,
    __in DWORD dwFlags);
#endif /* WINVER >= 0x0500 */

#if(_WIN32_WINNT >= 0x0500)
#if defined(_WINGDI_) && !defined(NOGDI)

WINUSERAPI
BOOL
WINAPI
UpdateLayeredWindow(
    __in HWND hWnd,
    __in_opt HDC hdcDst,
    __in_opt POINT *pptDst,
    __in_opt SIZE *psize,
    __in_opt HDC hdcSrc,
    __in_opt POINT *pptSrc,
    __in COLORREF crKey,
    __in_opt BLENDFUNCTION *pblend,
    __in DWORD dwFlags);



/*
 * Layered Window Update information
 */
typedef struct tagUPDATELAYEREDWINDOWINFO
{
    __in                DWORD               cbSize;
    __in_opt            HDC                 hdcDst;
    __in_ecount_opt(1)  POINT CONST         *pptDst;
    __in_ecount_opt(1)  SIZE CONST          *psize;
    __in_opt            HDC                 hdcSrc;
    __in_ecount_opt(1)  POINT CONST         *pptSrc;
    __in_opt            COLORREF            crKey;
    __in_ecount_opt(1)  BLENDFUNCTION CONST *pblend;
    __in                DWORD               dwFlags;
    __in_ecount_opt(1)  RECT CONST          *prcDirty;
} UPDATELAYEREDWINDOWINFO, *PUPDATELAYEREDWINDOWINFO;


#if (_WIN32_WINNT < 0x0502)
typedef
#endif /* _WIN32_WINNT < 0x0502 */
WINUSERAPI
BOOL
WINAPI
UpdateLayeredWindowIndirect(
    __in HWND hWnd,
    __in_ecount(1) UPDATELAYEREDWINDOWINFO CONST *pULWInfo);

#endif

#if(_WIN32_WINNT >= 0x0501)
WINUSERAPI
BOOL
WINAPI
GetLayeredWindowAttributes(
    __in HWND hwnd,
    __out_opt COLORREF *pcrKey,
    __out_opt BYTE *pbAlpha,
    __out_opt DWORD *pdwFlags);

#define PW_CLIENTONLY           0x00000001


WINUSERAPI
BOOL
WINAPI
PrintWindow(
    __in  HWND hwnd,
    __in  HDC hdcBlt,
    __in  UINT nFlags);

#endif /* _WIN32_WINNT >= 0x0501 */

WINUSERAPI
BOOL
WINAPI
SetLayeredWindowAttributes(
    __in HWND hwnd,
    __in COLORREF crKey,
    __in BYTE bAlpha,
    __in DWORD dwFlags);

#define LWA_COLORKEY            0x00000001
#define LWA_ALPHA               0x00000002


#define ULW_COLORKEY            0x00000001
#define ULW_ALPHA               0x00000002
#define ULW_OPAQUE              0x00000004

#define ULW_EX_NORESIZE         0x00000008


#endif /* _WIN32_WINNT >= 0x0500 */

#if(WINVER >= 0x0400)
WINUSERAPI
BOOL
WINAPI
ShowWindowAsync(
    __in  HWND hWnd,
    __in  int nCmdShow);
#endif /* WINVER >= 0x0400 */

WINUSERAPI
BOOL
WINAPI
FlashWindow(
    __in  HWND hWnd,
    __in  BOOL bInvert);

#if(WINVER >= 0x0500)
typedef struct {
    UINT  cbSize;
    HWND  hwnd;
    DWORD dwFlags;
    UINT  uCount;
    DWORD dwTimeout;
} FLASHWINFO, *PFLASHWINFO;

WINUSERAPI
BOOL
WINAPI
FlashWindowEx(
    __in PFLASHWINFO pfwi);

#define FLASHW_STOP         0
#define FLASHW_CAPTION      0x00000001
#define FLASHW_TRAY         0x00000002
#define FLASHW_ALL          (FLASHW_CAPTION | FLASHW_TRAY)
#define FLASHW_TIMER        0x00000004
#define FLASHW_TIMERNOFG    0x0000000C

#endif /* WINVER >= 0x0500 */

WINUSERAPI
BOOL
WINAPI
ShowOwnedPopups(
    __in  HWND hWnd,
    __in  BOOL fShow);

WINUSERAPI
BOOL
WINAPI
OpenIcon(
    __in  HWND hWnd);

WINUSERAPI
BOOL
WINAPI
CloseWindow(
    __in  HWND hWnd);

WINUSERAPI
BOOL
WINAPI
MoveWindow(
    __in HWND hWnd,
    __in int X,
    __in int Y,
    __in int nWidth,
    __in int nHeight,
    __in BOOL bRepaint);

WINUSERAPI
BOOL
WINAPI
SetWindowPos(
    __in HWND hWnd,
    __in_opt HWND hWndInsertAfter,
    __in int X,
    __in int Y,
    __in int cx,
    __in int cy,
    __in UINT uFlags);

WINUSERAPI
BOOL
WINAPI
GetWindowPlacement(
    __in HWND hWnd,
    __inout WINDOWPLACEMENT *lpwndpl);

WINUSERAPI
BOOL
WINAPI
SetWindowPlacement(
    __in HWND hWnd,
    __in CONST WINDOWPLACEMENT *lpwndpl);


#ifndef NODEFERWINDOWPOS

WINUSERAPI
HDWP
WINAPI
BeginDeferWindowPos(
    __in int nNumWindows);

WINUSERAPI
HDWP
WINAPI
DeferWindowPos(
    __in HDWP hWinPosInfo,
    __in HWND hWnd,
    __in_opt HWND hWndInsertAfter,
    __in int x,
    __in int y,
    __in int cx,
    __in int cy,
    __in UINT uFlags);

WINUSERAPI
BOOL
WINAPI
EndDeferWindowPos(
    __in HDWP hWinPosInfo);

#endif /* !NODEFERWINDOWPOS */

WINUSERAPI
BOOL
WINAPI
IsWindowVisible(
    __in HWND hWnd);

WINUSERAPI
BOOL
WINAPI
IsIconic(
    __in HWND hWnd);

WINUSERAPI
BOOL
WINAPI
AnyPopup(
    VOID);

WINUSERAPI
BOOL
WINAPI
BringWindowToTop(
    __in HWND hWnd);

WINUSERAPI
BOOL
WINAPI
IsZoomed(
    __in HWND hWnd);

/*
 * SetWindowPos Flags
 */
#define SWP_NOSIZE          0x0001
#define SWP_NOMOVE          0x0002
#define SWP_NOZORDER        0x0004
#define SWP_NOREDRAW        0x0008
#define SWP_NOACTIVATE      0x0010
#define SWP_FRAMECHANGED    0x0020  /* The frame changed: send WM_NCCALCSIZE */
#define SWP_SHOWWINDOW      0x0040
#define SWP_HIDEWINDOW      0x0080
#define SWP_NOCOPYBITS      0x0100
#define SWP_NOOWNERZORDER   0x0200  /* Don't do owner Z ordering */
#define SWP_NOSENDCHANGING  0x0400  /* Don't send WM_WINDOWPOSCHANGING */

#define SWP_DRAWFRAME       SWP_FRAMECHANGED
#define SWP_NOREPOSITION    SWP_NOOWNERZORDER

#if(WINVER >= 0x0400)
#define SWP_DEFERERASE      0x2000
#define SWP_ASYNCWINDOWPOS  0x4000
#endif /* WINVER >= 0x0400 */


#define HWND_TOP        ((HWND)0)
#define HWND_BOTTOM     ((HWND)1)
#define HWND_TOPMOST    ((HWND)-1)
#define HWND_NOTOPMOST  ((HWND)-2)

#ifndef NOCTLMGR

/*
 * WARNING:
 * The following structures must NOT be DWORD padded because they are
 * followed by strings, etc that do not have to be DWORD aligned.
 */
#include <pshpack2.h>

/*
 * original NT 32 bit dialog template:
 */
typedef struct {
    DWORD style;
    DWORD dwExtendedStyle;
    WORD cdit;
    short x;
    short y;
    short cx;
    short cy;
} DLGTEMPLATE;
typedef DLGTEMPLATE *LPDLGTEMPLATEA;
typedef DLGTEMPLATE *LPDLGTEMPLATEW;
#ifdef UNICODE
typedef LPDLGTEMPLATEW LPDLGTEMPLATE;
#else
typedef LPDLGTEMPLATEA LPDLGTEMPLATE;
#endif // UNICODE
typedef CONST DLGTEMPLATE *LPCDLGTEMPLATEA;
typedef CONST DLGTEMPLATE *LPCDLGTEMPLATEW;
#ifdef UNICODE
typedef LPCDLGTEMPLATEW LPCDLGTEMPLATE;
#else
typedef LPCDLGTEMPLATEA LPCDLGTEMPLATE;
#endif // UNICODE

/*
 * 32 bit Dialog item template.
 */
typedef struct {
    DWORD style;
    DWORD dwExtendedStyle;
    short x;
    short y;
    short cx;
    short cy;
    WORD id;
} DLGITEMTEMPLATE;
typedef DLGITEMTEMPLATE *PDLGITEMTEMPLATEA;
typedef DLGITEMTEMPLATE *PDLGITEMTEMPLATEW;
#ifdef UNICODE
typedef PDLGITEMTEMPLATEW PDLGITEMTEMPLATE;
#else
typedef PDLGITEMTEMPLATEA PDLGITEMTEMPLATE;
#endif // UNICODE
typedef DLGITEMTEMPLATE *LPDLGITEMTEMPLATEA;
typedef DLGITEMTEMPLATE *LPDLGITEMTEMPLATEW;
#ifdef UNICODE
typedef LPDLGITEMTEMPLATEW LPDLGITEMTEMPLATE;
#else
typedef LPDLGITEMTEMPLATEA LPDLGITEMTEMPLATE;
#endif // UNICODE


#include <poppack.h> /* Resume normal packing */

WINUSERAPI
HWND
WINAPI
CreateDialogParamA(
    __in_opt HINSTANCE hInstance,
    __in LPCSTR lpTemplateName,
    __in_opt HWND hWndParent,
    __in_opt DLGPROC lpDialogFunc,
    __in LPARAM dwInitParam);
WINUSERAPI
HWND
WINAPI
CreateDialogParamW(
    __in_opt HINSTANCE hInstance,
    __in LPCWSTR lpTemplateName,
    __in_opt HWND hWndParent,
    __in_opt DLGPROC lpDialogFunc,
    __in LPARAM dwInitParam);
#ifdef UNICODE
#define CreateDialogParam  CreateDialogParamW
#else
#define CreateDialogParam  CreateDialogParamA
#endif // !UNICODE

WINUSERAPI
HWND
WINAPI
CreateDialogIndirectParamA(
    __in_opt HINSTANCE hInstance,
    __in LPCDLGTEMPLATEA lpTemplate,
    __in_opt HWND hWndParent,
    __in_opt DLGPROC lpDialogFunc,
    __in LPARAM dwInitParam);
WINUSERAPI
HWND
WINAPI
CreateDialogIndirectParamW(
    __in_opt HINSTANCE hInstance,
    __in LPCDLGTEMPLATEW lpTemplate,
    __in_opt HWND hWndParent,
    __in_opt DLGPROC lpDialogFunc,
    __in LPARAM dwInitParam);
#ifdef UNICODE
#define CreateDialogIndirectParam  CreateDialogIndirectParamW
#else
#define CreateDialogIndirectParam  CreateDialogIndirectParamA
#endif // !UNICODE

#define CreateDialogA(hInstance, lpName, hWndParent, lpDialogFunc) \
CreateDialogParamA(hInstance, lpName, hWndParent, lpDialogFunc, 0L)
#define CreateDialogW(hInstance, lpName, hWndParent, lpDialogFunc) \
CreateDialogParamW(hInstance, lpName, hWndParent, lpDialogFunc, 0L)
#ifdef UNICODE
#define CreateDialog  CreateDialogW
#else
#define CreateDialog  CreateDialogA
#endif // !UNICODE

#define CreateDialogIndirectA(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
CreateDialogIndirectParamA(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)
#define CreateDialogIndirectW(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
CreateDialogIndirectParamW(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)
#ifdef UNICODE
#define CreateDialogIndirect  CreateDialogIndirectW
#else
#define CreateDialogIndirect  CreateDialogIndirectA
#endif // !UNICODE

WINUSERAPI
INT_PTR
WINAPI
DialogBoxParamA(
    __in_opt HINSTANCE hInstance,
    __in LPCSTR lpTemplateName,
    __in_opt HWND hWndParent,
    __in_opt DLGPROC lpDialogFunc,
    __in LPARAM dwInitParam);
WINUSERAPI
INT_PTR
WINAPI
DialogBoxParamW(
    __in_opt HINSTANCE hInstance,
    __in LPCWSTR lpTemplateName,
    __in_opt HWND hWndParent,
    __in_opt DLGPROC lpDialogFunc,
    __in LPARAM dwInitParam);
#ifdef UNICODE
#define DialogBoxParam  DialogBoxParamW
#else
#define DialogBoxParam  DialogBoxParamA
#endif // !UNICODE

WINUSERAPI
INT_PTR
WINAPI
DialogBoxIndirectParamA(
    __in_opt HINSTANCE hInstance,
    __in LPCDLGTEMPLATEA hDialogTemplate,
    __in_opt HWND hWndParent,
    __in_opt DLGPROC lpDialogFunc,
    __in LPARAM dwInitParam);
WINUSERAPI
INT_PTR
WINAPI
DialogBoxIndirectParamW(
    __in_opt HINSTANCE hInstance,
    __in LPCDLGTEMPLATEW hDialogTemplate,
    __in_opt HWND hWndParent,
    __in_opt DLGPROC lpDialogFunc,
    __in LPARAM dwInitParam);
#ifdef UNICODE
#define DialogBoxIndirectParam  DialogBoxIndirectParamW
#else
#define DialogBoxIndirectParam  DialogBoxIndirectParamA
#endif // !UNICODE

#define DialogBoxA(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
DialogBoxParamA(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)
#define DialogBoxW(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
DialogBoxParamW(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)
#ifdef UNICODE
#define DialogBox  DialogBoxW
#else
#define DialogBox  DialogBoxA
#endif // !UNICODE

#define DialogBoxIndirectA(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
DialogBoxIndirectParamA(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)
#define DialogBoxIndirectW(hInstance, lpTemplate, hWndParent, lpDialogFunc) \
DialogBoxIndirectParamW(hInstance, lpTemplate, hWndParent, lpDialogFunc, 0L)
#ifdef UNICODE
#define DialogBoxIndirect  DialogBoxIndirectW
#else
#define DialogBoxIndirect  DialogBoxIndirectA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
EndDialog(
    __in HWND hDlg,
    __in INT_PTR nResult);

WINUSERAPI
HWND
WINAPI
GetDlgItem(
    __in_opt HWND hDlg,
    __in int nIDDlgItem);

WINUSERAPI
BOOL
WINAPI
SetDlgItemInt(
    __in HWND hDlg,
    __in int nIDDlgItem,
    __in UINT uValue,
    __in BOOL bSigned);

WINUSERAPI
UINT
WINAPI
GetDlgItemInt(
    __in HWND hDlg,
    __in int nIDDlgItem,
    __out_opt BOOL *lpTranslated,
    __in BOOL bSigned);

WINUSERAPI
BOOL
WINAPI
SetDlgItemTextA(
    __in HWND hDlg,
    __in int nIDDlgItem,
    __in LPCSTR lpString);
WINUSERAPI
BOOL
WINAPI
SetDlgItemTextW(
    __in HWND hDlg,
    __in int nIDDlgItem,
    __in LPCWSTR lpString);
#ifdef UNICODE
#define SetDlgItemText  SetDlgItemTextW
#else
#define SetDlgItemText  SetDlgItemTextA
#endif // !UNICODE

WINUSERAPI
UINT
WINAPI
GetDlgItemTextA(
    __in HWND hDlg,
    __in int nIDDlgItem,
    __out_ecount(cchMax) LPSTR lpString,
    __in int cchMax);
WINUSERAPI
UINT
WINAPI
GetDlgItemTextW(
    __in HWND hDlg,
    __in int nIDDlgItem,
    __out_ecount(cchMax) LPWSTR lpString,
    __in int cchMax);
#ifdef UNICODE
#define GetDlgItemText  GetDlgItemTextW
#else
#define GetDlgItemText  GetDlgItemTextA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
CheckDlgButton(
    __in HWND hDlg,
    __in int nIDButton,
    __in UINT uCheck);

WINUSERAPI
BOOL
WINAPI
CheckRadioButton(
    __in HWND hDlg,
    __in int nIDFirstButton,
    __in int nIDLastButton,
    __in int nIDCheckButton);

WINUSERAPI
UINT
WINAPI
IsDlgButtonChecked(
    __in HWND hDlg,
    __in int nIDButton);

WINUSERAPI
LRESULT
WINAPI
SendDlgItemMessageA(
    __in HWND hDlg,
    __in int nIDDlgItem,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
WINUSERAPI
LRESULT
WINAPI
SendDlgItemMessageW(
    __in HWND hDlg,
    __in int nIDDlgItem,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
#ifdef UNICODE
#define SendDlgItemMessage  SendDlgItemMessageW
#else
#define SendDlgItemMessage  SendDlgItemMessageA
#endif // !UNICODE

WINUSERAPI
HWND
WINAPI
GetNextDlgGroupItem(
    __in HWND hDlg,
    __in_opt HWND hCtl,
    __in BOOL bPrevious);

WINUSERAPI
HWND
WINAPI
GetNextDlgTabItem(
    __in HWND hDlg,
    __in_opt HWND hCtl,
    __in BOOL bPrevious);

WINUSERAPI
int
WINAPI
GetDlgCtrlID(
    __in HWND hWnd);

WINUSERAPI
long
WINAPI
GetDialogBaseUnits(VOID);

WINUSERAPI
#ifndef _MAC
LRESULT
WINAPI
#else
LRESULT
CALLBACK
#endif
DefDlgProcA(
    __in HWND hDlg,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
WINUSERAPI
#ifndef _MAC
LRESULT
WINAPI
#else
LRESULT
CALLBACK
#endif
DefDlgProcW(
    __in HWND hDlg,
    __in UINT Msg,
    __in WPARAM wParam,
    __in LPARAM lParam);
#ifdef UNICODE
#define DefDlgProc  DefDlgProcW
#else
#define DefDlgProc  DefDlgProcA
#endif // !UNICODE

/*
 * Window extra byted needed for private dialog classes.
 */
#ifndef _MAC
#define DLGWINDOWEXTRA 30
#else
#define DLGWINDOWEXTRA 48
#endif

#endif /* !NOCTLMGR */

#ifndef NOMSG

WINUSERAPI
BOOL
WINAPI
CallMsgFilterA(
    __in LPMSG lpMsg,
    __in int nCode);
WINUSERAPI
BOOL
WINAPI
CallMsgFilterW(
    __in LPMSG lpMsg,
    __in int nCode);
#ifdef UNICODE
#define CallMsgFilter  CallMsgFilterW
#else
#define CallMsgFilter  CallMsgFilterA
#endif // !UNICODE

#endif /* !NOMSG */

#ifndef NOCLIPBOARD

/*
 * Clipboard Manager Functions
 */

WINUSERAPI
BOOL
WINAPI
OpenClipboard(
    __in_opt HWND hWndNewOwner);

WINUSERAPI
BOOL
WINAPI
CloseClipboard(
    VOID);


#if(WINVER >= 0x0500)

WINUSERAPI
DWORD
WINAPI
GetClipboardSequenceNumber(
    VOID);

#endif /* WINVER >= 0x0500 */

WINUSERAPI
HWND
WINAPI
GetClipboardOwner(
    VOID);

WINUSERAPI
HWND
WINAPI
SetClipboardViewer(
    __in HWND hWndNewViewer);

WINUSERAPI
HWND
WINAPI
GetClipboardViewer(
    VOID);

WINUSERAPI
BOOL
WINAPI
ChangeClipboardChain(
    __in HWND hWndRemove,
    __in HWND hWndNewNext);

WINUSERAPI
HANDLE
WINAPI
SetClipboardData(
    __in UINT uFormat,
    __in_opt HANDLE hMem);

WINUSERAPI
HANDLE
WINAPI
GetClipboardData(
    __in UINT uFormat);

WINUSERAPI
UINT
WINAPI
RegisterClipboardFormatA(
    __in LPCSTR lpszFormat);
WINUSERAPI
UINT
WINAPI
RegisterClipboardFormatW(
    __in LPCWSTR lpszFormat);
#ifdef UNICODE
#define RegisterClipboardFormat  RegisterClipboardFormatW
#else
#define RegisterClipboardFormat  RegisterClipboardFormatA
#endif // !UNICODE

WINUSERAPI
int
WINAPI
CountClipboardFormats(
    VOID);

WINUSERAPI
UINT
WINAPI
EnumClipboardFormats(
    __in UINT format);

WINUSERAPI
int
WINAPI
GetClipboardFormatNameA(
    __in UINT format,
    __out_ecount(cchMaxCount) LPSTR lpszFormatName,
    __in int cchMaxCount);
WINUSERAPI
int
WINAPI
GetClipboardFormatNameW(
    __in UINT format,
    __out_ecount(cchMaxCount) LPWSTR lpszFormatName,
    __in int cchMaxCount);
#ifdef UNICODE
#define GetClipboardFormatName  GetClipboardFormatNameW
#else
#define GetClipboardFormatName  GetClipboardFormatNameA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
EmptyClipboard(
    VOID);

WINUSERAPI
BOOL
WINAPI
IsClipboardFormatAvailable(
    __in UINT format);

WINUSERAPI
int
WINAPI
GetPriorityClipboardFormat(
    __in_ecount(cFormats) UINT *paFormatPriorityList,
    __in int cFormats);

WINUSERAPI
HWND
WINAPI
GetOpenClipboardWindow(
    VOID);

#endif /* !NOCLIPBOARD */

/*
 * Character Translation Routines
 */

WINUSERAPI
BOOL
WINAPI
CharToOemA(
    __in LPCSTR lpszSrc,
    __out LPSTR lpszDst);
WINUSERAPI
BOOL
WINAPI
CharToOemW(
    __in LPCWSTR lpszSrc,
    __out LPSTR lpszDst);
#ifdef UNICODE
#define CharToOem  CharToOemW
#else
#define CharToOem  CharToOemA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
OemToCharA(
    __in LPCSTR lpszSrc,
    __out LPSTR lpszDst);
WINUSERAPI
BOOL
WINAPI
OemToCharW(
    __in LPCSTR lpszSrc,
    __out LPWSTR lpszDst);
#ifdef UNICODE
#define OemToChar  OemToCharW
#else
#define OemToChar  OemToCharA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
CharToOemBuffA(
    __in LPCSTR lpszSrc,
    __out_ecount(cchDstLength) LPSTR lpszDst,
    __in DWORD cchDstLength);
WINUSERAPI
BOOL
WINAPI
CharToOemBuffW(
    __in LPCWSTR lpszSrc,
    __out_ecount(cchDstLength) LPSTR lpszDst,
    __in DWORD cchDstLength);
#ifdef UNICODE
#define CharToOemBuff  CharToOemBuffW
#else
#define CharToOemBuff  CharToOemBuffA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
OemToCharBuffA(
    __in LPCSTR lpszSrc,
    __out_ecount(cchDstLength) LPSTR lpszDst,
    __in DWORD cchDstLength);
WINUSERAPI
BOOL
WINAPI
OemToCharBuffW(
    __in LPCSTR lpszSrc,
    __out_ecount(cchDstLength) LPWSTR lpszDst,
    __in DWORD cchDstLength);
#ifdef UNICODE
#define OemToCharBuff  OemToCharBuffW
#else
#define OemToCharBuff  OemToCharBuffA
#endif // !UNICODE

WINUSERAPI
LPSTR
WINAPI
CharUpperA(
    __inout LPSTR lpsz);
WINUSERAPI
LPWSTR
WINAPI
CharUpperW(
    __inout LPWSTR lpsz);
#ifdef UNICODE
#define CharUpper  CharUpperW
#else
#define CharUpper  CharUpperA
#endif // !UNICODE

WINUSERAPI
DWORD
WINAPI
CharUpperBuffA(
    __inout_ecount(cchLength) LPSTR lpsz,
    __in DWORD cchLength);
WINUSERAPI
DWORD
WINAPI
CharUpperBuffW(
    __inout_ecount(cchLength) LPWSTR lpsz,
    __in DWORD cchLength);
#ifdef UNICODE
#define CharUpperBuff  CharUpperBuffW
#else
#define CharUpperBuff  CharUpperBuffA
#endif // !UNICODE

WINUSERAPI
LPSTR
WINAPI
CharLowerA(
    __inout LPSTR lpsz);
WINUSERAPI
LPWSTR
WINAPI
CharLowerW(
    __inout LPWSTR lpsz);
#ifdef UNICODE
#define CharLower  CharLowerW
#else
#define CharLower  CharLowerA
#endif // !UNICODE

WINUSERAPI
DWORD
WINAPI
CharLowerBuffA(
    __inout_ecount(cchLength) LPSTR lpsz,
    __in DWORD cchLength);
WINUSERAPI
DWORD
WINAPI
CharLowerBuffW(
    __inout_ecount(cchLength) LPWSTR lpsz,
    __in DWORD cchLength);
#ifdef UNICODE
#define CharLowerBuff  CharLowerBuffW
#else
#define CharLowerBuff  CharLowerBuffA
#endif // !UNICODE

WINUSERAPI
LPSTR
WINAPI
CharNextA(
    __in LPCSTR lpsz);
WINUSERAPI
LPWSTR
WINAPI
CharNextW(
    __in LPCWSTR lpsz);
#ifdef UNICODE
#define CharNext  CharNextW
#else
#define CharNext  CharNextA
#endif // !UNICODE

WINUSERAPI
LPSTR
WINAPI
CharPrevA(
    __in LPCSTR lpszStart,
    __in LPCSTR lpszCurrent);
WINUSERAPI
LPWSTR
WINAPI
CharPrevW(
    __in LPCWSTR lpszStart,
    __in LPCWSTR lpszCurrent);
#ifdef UNICODE
#define CharPrev  CharPrevW
#else
#define CharPrev  CharPrevA
#endif // !UNICODE

#if(WINVER >= 0x0400)
WINUSERAPI
LPSTR
WINAPI
CharNextExA(
     __in WORD CodePage,
     __in LPCSTR lpCurrentChar,
     __in DWORD dwFlags);

WINUSERAPI
LPSTR
WINAPI
CharPrevExA(
     __in WORD CodePage,
     __in LPCSTR lpStart,
     __in LPCSTR lpCurrentChar,
     __in DWORD dwFlags);
#endif /* WINVER >= 0x0400 */

/*
 * Compatibility defines for character translation routines
 */
#define AnsiToOem CharToOemA
#define OemToAnsi OemToCharA
#define AnsiToOemBuff CharToOemBuffA
#define OemToAnsiBuff OemToCharBuffA
#define AnsiUpper CharUpperA
#define AnsiUpperBuff CharUpperBuffA
#define AnsiLower CharLowerA
#define AnsiLowerBuff CharLowerBuffA
#define AnsiNext CharNextA
#define AnsiPrev CharPrevA

#ifndef  NOLANGUAGE
/*
 * Language dependent Routines
 */

WINUSERAPI
BOOL
WINAPI
IsCharAlphaA(
    __in CHAR ch);
WINUSERAPI
BOOL
WINAPI
IsCharAlphaW(
    __in WCHAR ch);
#ifdef UNICODE
#define IsCharAlpha  IsCharAlphaW
#else
#define IsCharAlpha  IsCharAlphaA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
IsCharAlphaNumericA(
    __in CHAR ch);
WINUSERAPI
BOOL
WINAPI
IsCharAlphaNumericW(
    __in WCHAR ch);
#ifdef UNICODE
#define IsCharAlphaNumeric  IsCharAlphaNumericW
#else
#define IsCharAlphaNumeric  IsCharAlphaNumericA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
IsCharUpperA(
    __in CHAR ch);
WINUSERAPI
BOOL
WINAPI
IsCharUpperW(
    __in WCHAR ch);
#ifdef UNICODE
#define IsCharUpper  IsCharUpperW
#else
#define IsCharUpper  IsCharUpperA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
IsCharLowerA(
    __in CHAR ch);
WINUSERAPI
BOOL
WINAPI
IsCharLowerW(
    __in WCHAR ch);
#ifdef UNICODE
#define IsCharLower  IsCharLowerW
#else
#define IsCharLower  IsCharLowerA
#endif // !UNICODE

#endif  /* !NOLANGUAGE */

WINUSERAPI
HWND
WINAPI
SetFocus(
    __in_opt HWND hWnd);

WINUSERAPI
HWND
WINAPI
GetActiveWindow(
    VOID);

WINUSERAPI
HWND
WINAPI
GetFocus(
    VOID);

WINUSERAPI
UINT
WINAPI
GetKBCodePage(
    VOID);

WINUSERAPI
SHORT
WINAPI
GetKeyState(
    __in int nVirtKey);

WINUSERAPI
SHORT
WINAPI
GetAsyncKeyState(
    __in int vKey);

WINUSERAPI
BOOL
WINAPI
GetKeyboardState(
    __out_ecount(256) PBYTE lpKeyState);

WINUSERAPI
BOOL
WINAPI
SetKeyboardState(
    __in_ecount(256) LPBYTE lpKeyState);

WINUSERAPI
int
WINAPI
GetKeyNameTextA(
    __in LONG lParam,
    __out_ecount(cchSize) LPSTR lpString,
    __in int cchSize);
WINUSERAPI
int
WINAPI
GetKeyNameTextW(
    __in LONG lParam,
    __out_ecount(cchSize) LPWSTR lpString,
    __in int cchSize);
#ifdef UNICODE
#define GetKeyNameText  GetKeyNameTextW
#else
#define GetKeyNameText  GetKeyNameTextA
#endif // !UNICODE

WINUSERAPI
int
WINAPI
GetKeyboardType(
    __in int nTypeFlag);

WINUSERAPI
int
WINAPI
ToAscii(
    __in UINT uVirtKey,
    __in UINT uScanCode,
    __in_ecount_opt(256) CONST BYTE *lpKeyState,
    __out LPWORD lpChar,
    __in UINT uFlags);

#if(WINVER >= 0x0400)
WINUSERAPI
int
WINAPI
ToAsciiEx(
    __in UINT uVirtKey,
    __in UINT uScanCode,
    __in_ecount_opt(256) CONST BYTE *lpKeyState,
    __out LPWORD lpChar,
    __in UINT uFlags,
    __in_opt HKL dwhkl);
#endif /* WINVER >= 0x0400 */

WINUSERAPI
int
WINAPI
ToUnicode(
    __in UINT wVirtKey,
    __in UINT wScanCode,
    __in_bcount_opt(256) CONST BYTE *lpKeyState,
    __out_ecount(cchBuff) LPWSTR pwszBuff,
    __in int cchBuff,
    __in UINT wFlags);

WINUSERAPI
DWORD
WINAPI
OemKeyScan(
    __in WORD wOemChar);

WINUSERAPI
SHORT
WINAPI
VkKeyScanA(
    __in CHAR ch);
WINUSERAPI
SHORT
WINAPI
VkKeyScanW(
    __in WCHAR ch);
#ifdef UNICODE
#define VkKeyScan  VkKeyScanW
#else
#define VkKeyScan  VkKeyScanA
#endif // !UNICODE

#if(WINVER >= 0x0400)
WINUSERAPI
SHORT
WINAPI
VkKeyScanExA(
    __in CHAR ch,
    __in HKL dwhkl);
WINUSERAPI
SHORT
WINAPI
VkKeyScanExW(
    __in WCHAR ch,
    __in HKL dwhkl);
#ifdef UNICODE
#define VkKeyScanEx  VkKeyScanExW
#else
#define VkKeyScanEx  VkKeyScanExA
#endif // !UNICODE
#endif /* WINVER >= 0x0400 */
#define KEYEVENTF_EXTENDEDKEY 0x0001
#define KEYEVENTF_KEYUP       0x0002
#if(_WIN32_WINNT >= 0x0500)
#define KEYEVENTF_UNICODE     0x0004
#define KEYEVENTF_SCANCODE    0x0008
#endif /* _WIN32_WINNT >= 0x0500 */

WINUSERAPI
VOID
WINAPI
keybd_event(
    __in BYTE bVk,
    __in BYTE bScan,
    __in DWORD dwFlags,
    __in ULONG_PTR dwExtraInfo);

#define MOUSEEVENTF_MOVE        0x0001 /* mouse move */
#define MOUSEEVENTF_LEFTDOWN    0x0002 /* left button down */
#define MOUSEEVENTF_LEFTUP      0x0004 /* left button up */
#define MOUSEEVENTF_RIGHTDOWN   0x0008 /* right button down */
#define MOUSEEVENTF_RIGHTUP     0x0010 /* right button up */
#define MOUSEEVENTF_MIDDLEDOWN  0x0020 /* middle button down */
#define MOUSEEVENTF_MIDDLEUP    0x0040 /* middle button up */
#define MOUSEEVENTF_XDOWN       0x0080 /* x button down */
#define MOUSEEVENTF_XUP         0x0100 /* x button down */
#define MOUSEEVENTF_WHEEL       0x0800 /* wheel button rolled */
#define MOUSEEVENTF_VIRTUALDESK 0x4000 /* map to entire virtual desktop */
#define MOUSEEVENTF_ABSOLUTE    0x8000 /* absolute move */



WINUSERAPI
VOID
WINAPI
mouse_event(
    __in DWORD dwFlags,
    __in DWORD dx,
    __in DWORD dy,
    __in DWORD dwData,
    __in ULONG_PTR dwExtraInfo);

#if (_WIN32_WINNT > 0x0400)

typedef struct tagMOUSEINPUT {
    LONG    dx;
    LONG    dy;
    DWORD   mouseData;
    DWORD   dwFlags;
    DWORD   time;
    ULONG_PTR dwExtraInfo;
} MOUSEINPUT, *PMOUSEINPUT, FAR* LPMOUSEINPUT;

typedef struct tagKEYBDINPUT {
    WORD    wVk;
    WORD    wScan;
    DWORD   dwFlags;
    DWORD   time;
    ULONG_PTR dwExtraInfo;
} KEYBDINPUT, *PKEYBDINPUT, FAR* LPKEYBDINPUT;

typedef struct tagHARDWAREINPUT {
    DWORD   uMsg;
    WORD    wParamL;
    WORD    wParamH;
} HARDWAREINPUT, *PHARDWAREINPUT, FAR* LPHARDWAREINPUT;

#define INPUT_MOUSE     0
#define INPUT_KEYBOARD  1
#define INPUT_HARDWARE  2

typedef struct tagINPUT {
    DWORD   type;

    union
    {
        MOUSEINPUT      mi;
        KEYBDINPUT      ki;
        HARDWAREINPUT   hi;
    };
} INPUT, *PINPUT, FAR* LPINPUT;

WINUSERAPI
UINT
WINAPI
SendInput(
    __in UINT cInputs,                     // number of input in the array
    __in_ecount(cInputs) LPINPUT pInputs,  // array of inputs
    __in int cbSize);                      // sizeof(INPUT)

#endif // (_WIN32_WINNT > 0x0400)

#if(_WIN32_WINNT >= 0x0500)
typedef struct tagLASTINPUTINFO {
    UINT cbSize;
    DWORD dwTime;
} LASTINPUTINFO, * PLASTINPUTINFO;

WINUSERAPI
BOOL
WINAPI
GetLastInputInfo(
    __out PLASTINPUTINFO plii);
#endif /* _WIN32_WINNT >= 0x0500 */

WINUSERAPI
UINT
WINAPI
MapVirtualKeyA(
    __in UINT uCode,
    __in UINT uMapType);
WINUSERAPI
UINT
WINAPI
MapVirtualKeyW(
    __in UINT uCode,
    __in UINT uMapType);
#ifdef UNICODE
#define MapVirtualKey  MapVirtualKeyW
#else
#define MapVirtualKey  MapVirtualKeyA
#endif // !UNICODE

#if(WINVER >= 0x0400)
WINUSERAPI
UINT
WINAPI
MapVirtualKeyExA(
    __in UINT uCode,
    __in UINT uMapType,
    __in_opt HKL dwhkl);
WINUSERAPI
UINT
WINAPI
MapVirtualKeyExW(
    __in UINT uCode,
    __in UINT uMapType,
    __in_opt HKL dwhkl);
#ifdef UNICODE
#define MapVirtualKeyEx  MapVirtualKeyExW
#else
#define MapVirtualKeyEx  MapVirtualKeyExA
#endif // !UNICODE
#endif /* WINVER >= 0x0400 */

WINUSERAPI
BOOL
WINAPI
GetInputState(
    VOID);

WINUSERAPI
DWORD
WINAPI
GetQueueStatus(
    __in UINT flags);


WINUSERAPI
HWND
WINAPI
GetCapture(
    VOID);

WINUSERAPI
HWND
WINAPI
SetCapture(
    __in HWND hWnd);

WINUSERAPI
BOOL
WINAPI
ReleaseCapture(
    VOID);

WINUSERAPI
DWORD
WINAPI
MsgWaitForMultipleObjects(
    __in DWORD nCount,
    __in_ecount_opt(nCount) CONST HANDLE *pHandles,
    __in BOOL fWaitAll,
    __in DWORD dwMilliseconds,
    __in DWORD dwWakeMask);

WINUSERAPI
DWORD
WINAPI
MsgWaitForMultipleObjectsEx(
    __in DWORD nCount,
    __in_ecount_opt(nCount) CONST HANDLE *pHandles,
    __in DWORD dwMilliseconds,
    __in DWORD dwWakeMask,
    __in DWORD dwFlags);


#define MWMO_WAITALL        0x0001
#define MWMO_ALERTABLE      0x0002
#define MWMO_INPUTAVAILABLE 0x0004

/*
 * Queue status flags for GetQueueStatus() and MsgWaitForMultipleObjects()
 */
#define QS_KEY              0x0001
#define QS_MOUSEMOVE        0x0002
#define QS_MOUSEBUTTON      0x0004
#define QS_POSTMESSAGE      0x0008
#define QS_TIMER            0x0010
#define QS_PAINT            0x0020
#define QS_SENDMESSAGE      0x0040
#define QS_HOTKEY           0x0080
#define QS_ALLPOSTMESSAGE   0x0100
#if(_WIN32_WINNT >= 0x0501)
#define QS_RAWINPUT         0x0400
#endif /* _WIN32_WINNT >= 0x0501 */

#define QS_MOUSE           (QS_MOUSEMOVE     | \
                            QS_MOUSEBUTTON)

#if (_WIN32_WINNT >= 0x0501)
#define QS_INPUT           (QS_MOUSE         | \
                            QS_KEY           | \
                            QS_RAWINPUT)
#else
#define QS_INPUT           (QS_MOUSE         | \
                            QS_KEY)
#endif // (_WIN32_WINNT >= 0x0501)

#define QS_ALLEVENTS       (QS_INPUT         | \
                            QS_POSTMESSAGE   | \
                            QS_TIMER         | \
                            QS_PAINT         | \
                            QS_HOTKEY)

#define QS_ALLINPUT        (QS_INPUT         | \
                            QS_POSTMESSAGE   | \
                            QS_TIMER         | \
                            QS_PAINT         | \
                            QS_HOTKEY        | \
                            QS_SENDMESSAGE)


#define USER_TIMER_MAXIMUM  0x7FFFFFFF
#define USER_TIMER_MINIMUM  0x0000000A

/*
 * Windows Functions
 */

WINUSERAPI
UINT_PTR
WINAPI
SetTimer(
    __in_opt HWND hWnd,
    __in UINT_PTR nIDEvent,
    __in UINT uElapse,
    __in_opt TIMERPROC lpTimerFunc);

WINUSERAPI
BOOL
WINAPI
KillTimer(
    __in_opt HWND hWnd,
    __in UINT_PTR uIDEvent);

WINUSERAPI
BOOL
WINAPI
IsWindowUnicode(
    __in HWND hWnd);

WINUSERAPI
BOOL
WINAPI
EnableWindow(
    __in HWND hWnd,
    __in BOOL bEnable);

WINUSERAPI
BOOL
WINAPI
IsWindowEnabled(
    __in HWND hWnd);

WINUSERAPI
HACCEL
WINAPI
LoadAcceleratorsA(
    __in_opt HINSTANCE hInstance,
    __in LPCSTR lpTableName);
WINUSERAPI
HACCEL
WINAPI
LoadAcceleratorsW(
    __in_opt HINSTANCE hInstance,
    __in LPCWSTR lpTableName);
#ifdef UNICODE
#define LoadAccelerators  LoadAcceleratorsW
#else
#define LoadAccelerators  LoadAcceleratorsA
#endif // !UNICODE

WINUSERAPI
HACCEL
WINAPI
CreateAcceleratorTableA(
    __in_ecount(cAccel) LPACCEL paccel, 
    __in int cAccel);
WINUSERAPI
HACCEL
WINAPI
CreateAcceleratorTableW(
    __in_ecount(cAccel) LPACCEL paccel, 
    __in int cAccel);
#ifdef UNICODE
#define CreateAcceleratorTable  CreateAcceleratorTableW
#else
#define CreateAcceleratorTable  CreateAcceleratorTableA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
DestroyAcceleratorTable(
    __in HACCEL hAccel);

WINUSERAPI
int
WINAPI
CopyAcceleratorTableA(
    __in HACCEL hAccelSrc,
    __out_ecount_part_opt(cAccelEntries, return) LPACCEL lpAccelDst,
    __in int cAccelEntries);
WINUSERAPI
int
WINAPI
CopyAcceleratorTableW(
    __in HACCEL hAccelSrc,
    __out_ecount_part_opt(cAccelEntries, return) LPACCEL lpAccelDst,
    __in int cAccelEntries);
#ifdef UNICODE
#define CopyAcceleratorTable  CopyAcceleratorTableW
#else
#define CopyAcceleratorTable  CopyAcceleratorTableA
#endif // !UNICODE

#ifndef NOMSG

WINUSERAPI
int
WINAPI
TranslateAcceleratorA(
    __in HWND hWnd,
    __in HACCEL hAccTable,
    __in LPMSG lpMsg);
WINUSERAPI
int
WINAPI
TranslateAcceleratorW(
    __in HWND hWnd,
    __in HACCEL hAccTable,
    __in LPMSG lpMsg);
#ifdef UNICODE
#define TranslateAccelerator  TranslateAcceleratorW
#else
#define TranslateAccelerator  TranslateAcceleratorA
#endif // !UNICODE

#endif /* !NOMSG */

#ifndef NOSYSMETRICS

/*
 * GetSystemMetrics() codes
 */

#define SM_CXSCREEN             0
#define SM_CYSCREEN             1
#define SM_CXVSCROLL            2
#define SM_CYHSCROLL            3
#define SM_CYCAPTION            4
#define SM_CXBORDER             5
#define SM_CYBORDER             6
#define SM_CXDLGFRAME           7
#define SM_CYDLGFRAME           8
#define SM_CYVTHUMB             9
#define SM_CXHTHUMB             10
#define SM_CXICON               11
#define SM_CYICON               12
#define SM_CXCURSOR             13
#define SM_CYCURSOR             14
#define SM_CYMENU               15
#define SM_CXFULLSCREEN         16
#define SM_CYFULLSCREEN         17
#define SM_CYKANJIWINDOW        18
#define SM_MOUSEPRESENT         19
#define SM_CYVSCROLL            20
#define SM_CXHSCROLL            21
#define SM_DEBUG                22
#define SM_SWAPBUTTON           23
#define SM_RESERVED1            24
#define SM_RESERVED2            25
#define SM_RESERVED3            26
#define SM_RESERVED4            27
#define SM_CXMIN                28
#define SM_CYMIN                29
#define SM_CXSIZE               30
#define SM_CYSIZE               31
#define SM_CXFRAME              32
#define SM_CYFRAME              33
#define SM_CXMINTRACK           34
#define SM_CYMINTRACK           35
#define SM_CXDOUBLECLK          36
#define SM_CYDOUBLECLK          37
#define SM_CXICONSPACING        38
#define SM_CYICONSPACING        39
#define SM_MENUDROPALIGNMENT    40
#define SM_PENWINDOWS           41
#define SM_DBCSENABLED          42
#define SM_CMOUSEBUTTONS        43

#if(WINVER >= 0x0400)
#define SM_CXFIXEDFRAME           SM_CXDLGFRAME  /* ;win40 name change */
#define SM_CYFIXEDFRAME           SM_CYDLGFRAME  /* ;win40 name change */
#define SM_CXSIZEFRAME            SM_CXFRAME     /* ;win40 name change */
#define SM_CYSIZEFRAME            SM_CYFRAME     /* ;win40 name change */

#define SM_SECURE               44
#define SM_CXEDGE               45
#define SM_CYEDGE               46
#define SM_CXMINSPACING         47
#define SM_CYMINSPACING         48
#define SM_CXSMICON             49
#define SM_CYSMICON             50
#define SM_CYSMCAPTION          51
#define SM_CXSMSIZE             52
#define SM_CYSMSIZE             53
#define SM_CXMENUSIZE           54
#define SM_CYMENUSIZE           55
#define SM_ARRANGE              56
#define SM_CXMINIMIZED          57
#define SM_CYMINIMIZED          58
#define SM_CXMAXTRACK           59
#define SM_CYMAXTRACK           60
#define SM_CXMAXIMIZED          61
#define SM_CYMAXIMIZED          62
#define SM_NETWORK              63
#define SM_CLEANBOOT            67
#define SM_CXDRAG               68
#define SM_CYDRAG               69
#endif /* WINVER >= 0x0400 */
#define SM_SHOWSOUNDS           70
#if(WINVER >= 0x0400)
#define SM_CXMENUCHECK          71   /* Use instead of GetMenuCheckMarkDimensions()! */
#define SM_CYMENUCHECK          72
#define SM_SLOWMACHINE          73
#define SM_MIDEASTENABLED       74
#endif /* WINVER >= 0x0400 */

#if (WINVER >= 0x0500) || (_WIN32_WINNT >= 0x0400)
#define SM_MOUSEWHEELPRESENT    75
#endif
#if(WINVER >= 0x0500)
#define SM_XVIRTUALSCREEN       76
#define SM_YVIRTUALSCREEN       77
#define SM_CXVIRTUALSCREEN      78
#define SM_CYVIRTUALSCREEN      79
#define SM_CMONITORS            80
#define SM_SAMEDISPLAYFORMAT    81
#endif /* WINVER >= 0x0500 */
#if(_WIN32_WINNT >= 0x0500)
#define SM_IMMENABLED           82
#endif /* _WIN32_WINNT >= 0x0500 */
#if(_WIN32_WINNT >= 0x0501)
#define SM_CXFOCUSBORDER        83
#define SM_CYFOCUSBORDER        84
#endif /* _WIN32_WINNT >= 0x0501 */

#if(_WIN32_WINNT >= 0x0501)
#define SM_TABLETPC             86
#define SM_MEDIACENTER          87
#define SM_STARTER              88
#define SM_SERVERR2             89
#endif /* _WIN32_WINNT >= 0x0501 */

#if (WINVER < 0x0500) && (!defined(_WIN32_WINNT) || (_WIN32_WINNT < 0x0400))
#define SM_CMETRICS             76
#elif WINVER == 0x500
#define SM_CMETRICS             83
#else
#define SM_CMETRICS             90
#endif

#if(WINVER >= 0x0500)
#define SM_REMOTESESSION        0x1000


#if(_WIN32_WINNT >= 0x0501)
#define SM_SHUTTINGDOWN         0x2000
#endif /* _WIN32_WINNT >= 0x0501 */

#if(WINVER >= 0x0501)
#define SM_REMOTECONTROL        0x2001
#endif /* WINVER >= 0x0501 */

#if(WINVER >= 0x0501)
#define SM_CARETBLINKINGENABLED 0x2002
#endif /* WINVER >= 0x0501 */

#endif /* WINVER >= 0x0500 */


WINUSERAPI
int
WINAPI
GetSystemMetrics(
    __in int nIndex);


#endif /* !NOSYSMETRICS */

#ifndef NOMENUS

WINUSERAPI
HMENU
WINAPI
LoadMenuA(
    __in_opt HINSTANCE hInstance,
    __in LPCSTR lpMenuName);
WINUSERAPI
HMENU
WINAPI
LoadMenuW(
    __in_opt HINSTANCE hInstance,
    __in LPCWSTR lpMenuName);
#ifdef UNICODE
#define LoadMenu  LoadMenuW
#else
#define LoadMenu  LoadMenuA
#endif // !UNICODE

WINUSERAPI
HMENU
WINAPI
LoadMenuIndirectA(
    __in CONST MENUTEMPLATEA *lpMenuTemplate);
WINUSERAPI
HMENU
WINAPI
LoadMenuIndirectW(
    __in CONST MENUTEMPLATEW *lpMenuTemplate);
#ifdef UNICODE
#define LoadMenuIndirect  LoadMenuIndirectW
#else
#define LoadMenuIndirect  LoadMenuIndirectA
#endif // !UNICODE

WINUSERAPI
HMENU
WINAPI
GetMenu(
    __in HWND hWnd);

WINUSERAPI
BOOL
WINAPI
SetMenu(
    __in HWND hWnd,
    __in_opt HMENU hMenu);

WINUSERAPI
BOOL
WINAPI
ChangeMenuA(
    __in HMENU hMenu,
    __in UINT cmd,
    __in_opt LPCSTR lpszNewItem,
    __in UINT cmdInsert,
    __in UINT flags);
WINUSERAPI
BOOL
WINAPI
ChangeMenuW(
    __in HMENU hMenu,
    __in UINT cmd,
    __in_opt LPCWSTR lpszNewItem,
    __in UINT cmdInsert,
    __in UINT flags);
#ifdef UNICODE
#define ChangeMenu  ChangeMenuW
#else
#define ChangeMenu  ChangeMenuA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
HiliteMenuItem(
    __in HWND hWnd,
    __in HMENU hMenu,
    __in UINT uIDHiliteItem,
    __in UINT uHilite);

WINUSERAPI
int
WINAPI
GetMenuStringA(
    __in HMENU hMenu,
    __in UINT uIDItem,
    __out_ecount_opt(cchMax) LPSTR lpString,
    __in int cchMax,
    __in UINT flags);
WINUSERAPI
int
WINAPI
GetMenuStringW(
    __in HMENU hMenu,
    __in UINT uIDItem,
    __out_ecount_opt(cchMax) LPWSTR lpString,
    __in int cchMax,
    __in UINT flags);
#ifdef UNICODE
#define GetMenuString  GetMenuStringW
#else
#define GetMenuString  GetMenuStringA
#endif // !UNICODE

WINUSERAPI
UINT
WINAPI
GetMenuState(
    __in HMENU hMenu,
    __in UINT uId,
    __in UINT uFlags);

WINUSERAPI
BOOL
WINAPI
DrawMenuBar(
    __in HWND hWnd);

#if(_WIN32_WINNT >= 0x0501)
#define PMB_ACTIVE      0x00000001

#endif /* _WIN32_WINNT >= 0x0501 */


WINUSERAPI
HMENU
WINAPI
GetSystemMenu(
    __in HWND hWnd,
    __in BOOL bRevert);


WINUSERAPI
HMENU
WINAPI
CreateMenu(
    VOID);

WINUSERAPI
HMENU
WINAPI
CreatePopupMenu(
    VOID);

WINUSERAPI
BOOL
WINAPI
DestroyMenu(
    __in HMENU hMenu);

WINUSERAPI
DWORD
WINAPI
CheckMenuItem(
    __in HMENU hMenu,
    __in UINT uIDCheckItem,
    __in UINT uCheck);

WINUSERAPI
BOOL
WINAPI
EnableMenuItem(
    __in HMENU hMenu,
    __in UINT uIDEnableItem,
    __in UINT uEnable);

WINUSERAPI
HMENU
WINAPI
GetSubMenu(
    __in HMENU hMenu,
    __in int nPos);

WINUSERAPI
UINT
WINAPI
GetMenuItemID(
    __in HMENU hMenu,
    __in int nPos);

WINUSERAPI
int
WINAPI
GetMenuItemCount(
    __in_opt HMENU hMenu);

WINUSERAPI
BOOL
WINAPI
InsertMenuA(
    __in HMENU hMenu,
    __in UINT uPosition,
    __in UINT uFlags,
    __in UINT_PTR uIDNewItem,
    __in_opt LPCSTR lpNewItem);
WINUSERAPI
BOOL
WINAPI
InsertMenuW(
    __in HMENU hMenu,
    __in UINT uPosition,
    __in UINT uFlags,
    __in UINT_PTR uIDNewItem,
    __in_opt LPCWSTR lpNewItem);
#ifdef UNICODE
#define InsertMenu  InsertMenuW
#else
#define InsertMenu  InsertMenuA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
AppendMenuA(
    __in HMENU hMenu,
    __in UINT uFlags,
    __in UINT_PTR uIDNewItem,
    __in_opt LPCSTR lpNewItem);
WINUSERAPI
BOOL
WINAPI
AppendMenuW(
    __in HMENU hMenu,
    __in UINT uFlags,
    __in UINT_PTR uIDNewItem,
    __in_opt LPCWSTR lpNewItem);
#ifdef UNICODE
#define AppendMenu  AppendMenuW
#else
#define AppendMenu  AppendMenuA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
ModifyMenuA(
    __in HMENU hMnu,
    __in UINT uPosition,
    __in UINT uFlags,
    __in UINT_PTR uIDNewItem,
    __in_opt LPCSTR lpNewItem);
WINUSERAPI
BOOL
WINAPI
ModifyMenuW(
    __in HMENU hMnu,
    __in UINT uPosition,
    __in UINT uFlags,
    __in UINT_PTR uIDNewItem,
    __in_opt LPCWSTR lpNewItem);
#ifdef UNICODE
#define ModifyMenu  ModifyMenuW
#else
#define ModifyMenu  ModifyMenuA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI RemoveMenu(
    __in HMENU hMenu,
    __in UINT uPosition,
    __in UINT uFlags);

WINUSERAPI
BOOL
WINAPI
DeleteMenu(
    __in HMENU hMenu,
    __in UINT uPosition,
    __in UINT uFlags);

WINUSERAPI
BOOL
WINAPI
SetMenuItemBitmaps(
    __in HMENU hMenu,
    __in UINT uPosition,
    __in UINT uFlags,
    __in_opt HBITMAP hBitmapUnchecked,
    __in_opt HBITMAP hBitmapChecked);

WINUSERAPI
LONG
WINAPI
GetMenuCheckMarkDimensions(
    VOID);

WINUSERAPI
BOOL
WINAPI
TrackPopupMenu(
    __in HMENU hMenu,
    __in UINT uFlags,
    __in int x,
    __in int y,
    __in int nReserved,
    __in HWND hWnd,
    __in_opt CONST RECT *prcRect);

#if(WINVER >= 0x0400)
/* return codes for WM_MENUCHAR */
#define MNC_IGNORE  0
#define MNC_CLOSE   1
#define MNC_EXECUTE 2
#define MNC_SELECT  3

typedef struct tagTPMPARAMS
{
    UINT    cbSize;     /* Size of structure */
    RECT    rcExclude;  /* Screen coordinates of rectangle to exclude when positioning */
}   TPMPARAMS;
typedef TPMPARAMS FAR *LPTPMPARAMS;

WINUSERAPI
BOOL
WINAPI
TrackPopupMenuEx(
    __in HMENU,
    __in UINT,
    __in int,
    __in int,
    __in HWND,
    __in_opt LPTPMPARAMS);
#endif /* WINVER >= 0x0400 */

#if(WINVER >= 0x0500)

#define MNS_NOCHECK         0x80000000
#define MNS_MODELESS        0x40000000
#define MNS_DRAGDROP        0x20000000
#define MNS_AUTODISMISS     0x10000000
#define MNS_NOTIFYBYPOS     0x08000000
#define MNS_CHECKORBMP      0x04000000

#define MIM_MAXHEIGHT               0x00000001
#define MIM_BACKGROUND              0x00000002
#define MIM_HELPID                  0x00000004
#define MIM_MENUDATA                0x00000008
#define MIM_STYLE                   0x00000010
#define MIM_APPLYTOSUBMENUS         0x80000000

typedef struct tagMENUINFO
{
    DWORD   cbSize;
    DWORD   fMask;
    DWORD   dwStyle;
    UINT    cyMax;
    HBRUSH  hbrBack;
    DWORD   dwContextHelpID;
    ULONG_PTR dwMenuData;
}   MENUINFO, FAR *LPMENUINFO;
typedef MENUINFO CONST FAR *LPCMENUINFO;

WINUSERAPI
BOOL
WINAPI
GetMenuInfo(
    __in HMENU,
    __inout LPMENUINFO);

WINUSERAPI
BOOL
WINAPI
SetMenuInfo(
    __in HMENU,
    __in LPCMENUINFO);

WINUSERAPI
BOOL
WINAPI
EndMenu(
        VOID);

/*
 * WM_MENUDRAG return values.
 */
#define MND_CONTINUE       0
#define MND_ENDMENU        1

typedef struct tagMENUGETOBJECTINFO
{
    DWORD dwFlags;
    UINT uPos;
    HMENU hmenu;
    PVOID riid;
    PVOID pvObj;
} MENUGETOBJECTINFO, * PMENUGETOBJECTINFO;

/*
 * MENUGETOBJECTINFO dwFlags values
 */
#define MNGOF_TOPGAP         0x00000001
#define MNGOF_BOTTOMGAP      0x00000002

/*
 * WM_MENUGETOBJECT return values
 */
#define MNGO_NOINTERFACE     0x00000000
#define MNGO_NOERROR         0x00000001
#endif /* WINVER >= 0x0500 */

#if(WINVER >= 0x0400)
#define MIIM_STATE       0x00000001
#define MIIM_ID          0x00000002
#define MIIM_SUBMENU     0x00000004
#define MIIM_CHECKMARKS  0x00000008
#define MIIM_TYPE        0x00000010
#define MIIM_DATA        0x00000020
#endif /* WINVER >= 0x0400 */

#if(WINVER >= 0x0500)
#define MIIM_STRING      0x00000040
#define MIIM_BITMAP      0x00000080
#define MIIM_FTYPE       0x00000100

#define HBMMENU_CALLBACK            ((HBITMAP) -1)
#define HBMMENU_SYSTEM              ((HBITMAP)  1)
#define HBMMENU_MBAR_RESTORE        ((HBITMAP)  2)
#define HBMMENU_MBAR_MINIMIZE       ((HBITMAP)  3)
#define HBMMENU_MBAR_CLOSE          ((HBITMAP)  5)
#define HBMMENU_MBAR_CLOSE_D        ((HBITMAP)  6)
#define HBMMENU_MBAR_MINIMIZE_D     ((HBITMAP)  7)
#define HBMMENU_POPUP_CLOSE         ((HBITMAP)  8)
#define HBMMENU_POPUP_RESTORE       ((HBITMAP)  9)
#define HBMMENU_POPUP_MAXIMIZE      ((HBITMAP) 10)
#define HBMMENU_POPUP_MINIMIZE      ((HBITMAP) 11)
#endif /* WINVER >= 0x0500 */

#if(WINVER >= 0x0400)
typedef struct tagMENUITEMINFOA
{
    UINT     cbSize;
    UINT     fMask;
    UINT     fType;         // used if MIIM_TYPE (4.0) or MIIM_FTYPE (>4.0)
    UINT     fState;        // used if MIIM_STATE
    UINT     wID;           // used if MIIM_ID
    HMENU    hSubMenu;      // used if MIIM_SUBMENU
    HBITMAP  hbmpChecked;   // used if MIIM_CHECKMARKS
    HBITMAP  hbmpUnchecked; // used if MIIM_CHECKMARKS
    ULONG_PTR dwItemData;   // used if MIIM_DATA
    LPSTR    dwTypeData;    // used if MIIM_TYPE (4.0) or MIIM_STRING (>4.0)
    UINT     cch;           // used if MIIM_TYPE (4.0) or MIIM_STRING (>4.0)
#if(WINVER >= 0x0500)
    HBITMAP  hbmpItem;      // used if MIIM_BITMAP
#endif /* WINVER >= 0x0500 */
}   MENUITEMINFOA, FAR *LPMENUITEMINFOA;
typedef struct tagMENUITEMINFOW
{
    UINT     cbSize;
    UINT     fMask;
    UINT     fType;         // used if MIIM_TYPE (4.0) or MIIM_FTYPE (>4.0)
    UINT     fState;        // used if MIIM_STATE
    UINT     wID;           // used if MIIM_ID
    HMENU    hSubMenu;      // used if MIIM_SUBMENU
    HBITMAP  hbmpChecked;   // used if MIIM_CHECKMARKS
    HBITMAP  hbmpUnchecked; // used if MIIM_CHECKMARKS
    ULONG_PTR dwItemData;   // used if MIIM_DATA
    LPWSTR   dwTypeData;    // used if MIIM_TYPE (4.0) or MIIM_STRING (>4.0)
    UINT     cch;           // used if MIIM_TYPE (4.0) or MIIM_STRING (>4.0)
#if(WINVER >= 0x0500)
    HBITMAP  hbmpItem;      // used if MIIM_BITMAP
#endif /* WINVER >= 0x0500 */
}   MENUITEMINFOW, FAR *LPMENUITEMINFOW;
#ifdef UNICODE
typedef MENUITEMINFOW MENUITEMINFO;
typedef LPMENUITEMINFOW LPMENUITEMINFO;
#else
typedef MENUITEMINFOA MENUITEMINFO;
typedef LPMENUITEMINFOA LPMENUITEMINFO;
#endif // UNICODE
typedef MENUITEMINFOA CONST FAR *LPCMENUITEMINFOA;
typedef MENUITEMINFOW CONST FAR *LPCMENUITEMINFOW;
#ifdef UNICODE
typedef LPCMENUITEMINFOW LPCMENUITEMINFO;
#else
typedef LPCMENUITEMINFOA LPCMENUITEMINFO;
#endif // UNICODE


WINUSERAPI
BOOL
WINAPI
InsertMenuItemA(
    __in HMENU hmenu,
    __in UINT item,
    __in BOOL fByPosition,
    __in LPCMENUITEMINFOA lpmi);
WINUSERAPI
BOOL
WINAPI
InsertMenuItemW(
    __in HMENU hmenu,
    __in UINT item,
    __in BOOL fByPosition,
    __in LPCMENUITEMINFOW lpmi);
#ifdef UNICODE
#define InsertMenuItem  InsertMenuItemW
#else
#define InsertMenuItem  InsertMenuItemA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
GetMenuItemInfoA(
    __in HMENU hmenu,
    __in UINT item,
    __in BOOL fByPosition,
    __inout LPMENUITEMINFOA lpmii);
WINUSERAPI
BOOL
WINAPI
GetMenuItemInfoW(
    __in HMENU hmenu,
    __in UINT item,
    __in BOOL fByPosition,
    __inout LPMENUITEMINFOW lpmii);
#ifdef UNICODE
#define GetMenuItemInfo  GetMenuItemInfoW
#else
#define GetMenuItemInfo  GetMenuItemInfoA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
SetMenuItemInfoA(
    __in HMENU hmenu,
    __in UINT item,
    __in BOOL fByPositon,
    __in LPCMENUITEMINFOA lpmii);
WINUSERAPI
BOOL
WINAPI
SetMenuItemInfoW(
    __in HMENU hmenu,
    __in UINT item,
    __in BOOL fByPositon,
    __in LPCMENUITEMINFOW lpmii);
#ifdef UNICODE
#define SetMenuItemInfo  SetMenuItemInfoW
#else
#define SetMenuItemInfo  SetMenuItemInfoA
#endif // !UNICODE


#define GMDI_USEDISABLED    0x0001L
#define GMDI_GOINTOPOPUPS   0x0002L

WINUSERAPI
UINT
WINAPI
GetMenuDefaultItem(
    __in HMENU hMenu,
    __in UINT fByPos,
    __in UINT gmdiFlags);

WINUSERAPI
BOOL
WINAPI
SetMenuDefaultItem(
    __in HMENU hMenu,
    __in UINT uItem,
    __in UINT fByPos);

WINUSERAPI
BOOL
WINAPI
GetMenuItemRect(
    __in_opt HWND hWnd,
    __in HMENU hMenu,
    __in UINT uItem,
    __out LPRECT lprcItem);

WINUSERAPI
int
WINAPI
MenuItemFromPoint(
    __in_opt HWND hWnd,
    __in HMENU hMenu,
    __in POINT ptScreen);
#endif /* WINVER >= 0x0400 */

/*
 * Flags for TrackPopupMenu
 */
#define TPM_LEFTBUTTON  0x0000L
#define TPM_RIGHTBUTTON 0x0002L
#define TPM_LEFTALIGN   0x0000L
#define TPM_CENTERALIGN 0x0004L
#define TPM_RIGHTALIGN  0x0008L
#if(WINVER >= 0x0400)
#define TPM_TOPALIGN        0x0000L
#define TPM_VCENTERALIGN    0x0010L
#define TPM_BOTTOMALIGN     0x0020L

#define TPM_HORIZONTAL      0x0000L     /* Horz alignment matters more */
#define TPM_VERTICAL        0x0040L     /* Vert alignment matters more */
#define TPM_NONOTIFY        0x0080L     /* Don't send any notification msgs */
#define TPM_RETURNCMD       0x0100L
#endif /* WINVER >= 0x0400 */
#if(WINVER >= 0x0500)
#define TPM_RECURSE         0x0001L
#define TPM_HORPOSANIMATION 0x0400L
#define TPM_HORNEGANIMATION 0x0800L
#define TPM_VERPOSANIMATION 0x1000L
#define TPM_VERNEGANIMATION 0x2000L
#if(_WIN32_WINNT >= 0x0500)
#define TPM_NOANIMATION     0x4000L
#endif /* _WIN32_WINNT >= 0x0500 */
#if(_WIN32_WINNT >= 0x0501)
#define TPM_LAYOUTRTL       0x8000L
#endif /* _WIN32_WINNT >= 0x0501 */
#endif /* WINVER >= 0x0500 */


#endif /* !NOMENUS */


#if(WINVER >= 0x0400)
//
// Drag-and-drop support
// Obsolete - use OLE instead
//
typedef struct tagDROPSTRUCT
{
    HWND    hwndSource;
    HWND    hwndSink;
    DWORD   wFmt;
    ULONG_PTR dwData;
    POINT   ptDrop;
    DWORD   dwControlData;
} DROPSTRUCT, *PDROPSTRUCT, *LPDROPSTRUCT;

#define DOF_EXECUTABLE      0x8001      // wFmt flags
#define DOF_DOCUMENT        0x8002
#define DOF_DIRECTORY       0x8003
#define DOF_MULTIPLE        0x8004
#define DOF_PROGMAN         0x0001
#define DOF_SHELLDATA       0x0002

#define DO_DROPFILE         0x454C4946L
#define DO_PRINTFILE        0x544E5250L

WINUSERAPI
DWORD
WINAPI
DragObject(
    __in HWND hwndParent,
    __in HWND hwndFrom,
    __in UINT fmt,
    __in ULONG_PTR data,
    __in_opt HCURSOR hcur);

WINUSERAPI
BOOL
WINAPI
DragDetect(
    __in HWND hwnd,
    __in POINT pt);
#endif /* WINVER >= 0x0400 */

WINUSERAPI
BOOL
WINAPI
DrawIcon(
    __in HDC hDC,
    __in int X,
    __in int Y,
    __in HICON hIcon);

#ifndef NODRAWTEXT

/*
 * DrawText() Format Flags
 */
#define DT_TOP                      0x00000000
#define DT_LEFT                     0x00000000
#define DT_CENTER                   0x00000001
#define DT_RIGHT                    0x00000002
#define DT_VCENTER                  0x00000004
#define DT_BOTTOM                   0x00000008
#define DT_WORDBREAK                0x00000010
#define DT_SINGLELINE               0x00000020
#define DT_EXPANDTABS               0x00000040
#define DT_TABSTOP                  0x00000080
#define DT_NOCLIP                   0x00000100
#define DT_EXTERNALLEADING          0x00000200
#define DT_CALCRECT                 0x00000400
#define DT_NOPREFIX                 0x00000800
#define DT_INTERNAL                 0x00001000

#if(WINVER >= 0x0400)
#define DT_EDITCONTROL              0x00002000
#define DT_PATH_ELLIPSIS            0x00004000
#define DT_END_ELLIPSIS             0x00008000
#define DT_MODIFYSTRING             0x00010000
#define DT_RTLREADING               0x00020000
#define DT_WORD_ELLIPSIS            0x00040000
#if(WINVER >= 0x0500)
#define DT_NOFULLWIDTHCHARBREAK     0x00080000
#if(_WIN32_WINNT >= 0x0500)
#define DT_HIDEPREFIX               0x00100000
#define DT_PREFIXONLY               0x00200000
#endif /* _WIN32_WINNT >= 0x0500 */
#endif /* WINVER >= 0x0500 */

typedef struct tagDRAWTEXTPARAMS
{
    UINT    cbSize;
    int     iTabLength;
    int     iLeftMargin;
    int     iRightMargin;
    UINT    uiLengthDrawn;
} DRAWTEXTPARAMS, FAR *LPDRAWTEXTPARAMS;
#endif /* WINVER >= 0x0400 */


WINUSERAPI
int
WINAPI
DrawTextA(
    __in HDC hdc,
    __inout_ecount(cchText) LPCSTR lpchText,
    __in int cchText,
    __inout LPRECT lprc,
    __in UINT format);
WINUSERAPI
int
WINAPI
DrawTextW(
    __in HDC hdc,
    __inout_ecount(cchText) LPCWSTR lpchText,
    __in int cchText,
    __inout LPRECT lprc,
    __in UINT format);
#ifdef UNICODE
#define DrawText  DrawTextW
#else
#define DrawText  DrawTextA
#endif // !UNICODE
    
#if defined(_M_CEE)
#undef DrawText
__inline
int
DrawText(
    HDC hdc,
    LPCTSTR lpchText,
    int cchText,
    LPRECT lprc,
    UINT format
    )
{
#ifdef UNICODE
    return DrawTextW(
#else
    return DrawTextA(
#endif
        hdc,
	lpchText,
	cchText,
	lprc,
	format
        );
}
#endif	/* _M_CEE */


#if(WINVER >= 0x0400)
WINUSERAPI
int
WINAPI
DrawTextExA(
    __in HDC hdc,
    __inout_ecount(cchText) LPSTR lpchText,
    __in int cchText,
    __inout LPRECT lprc,
    __in UINT format,
    __in_opt LPDRAWTEXTPARAMS lpdtp);
WINUSERAPI
int
WINAPI
DrawTextExW(
    __in HDC hdc,
    __inout_ecount(cchText) LPWSTR lpchText,
    __in int cchText,
    __inout LPRECT lprc,
    __in UINT format,
    __in_opt LPDRAWTEXTPARAMS lpdtp);
#ifdef UNICODE
#define DrawTextEx  DrawTextExW
#else
#define DrawTextEx  DrawTextExA
#endif // !UNICODE
#endif /* WINVER >= 0x0400 */

#endif /* !NODRAWTEXT */

WINUSERAPI
BOOL
WINAPI
GrayStringA(
    __in HDC hDC,
    __in_opt HBRUSH hBrush,
    __in_opt GRAYSTRINGPROC lpOutputFunc,
    __in LPARAM lpData,
    __in int nCount,
    __in int X,
    __in int Y,
    __in int nWidth,
    __in int nHeight);
WINUSERAPI
BOOL
WINAPI
GrayStringW(
    __in HDC hDC,
    __in_opt HBRUSH hBrush,
    __in_opt GRAYSTRINGPROC lpOutputFunc,
    __in LPARAM lpData,
    __in int nCount,
    __in int X,
    __in int Y,
    __in int nWidth,
    __in int nHeight);
#ifdef UNICODE
#define GrayString  GrayStringW
#else
#define GrayString  GrayStringA
#endif // !UNICODE

#if(WINVER >= 0x0400)
/* Monolithic state-drawing routine */
/* Image type */
#define DST_COMPLEX     0x0000
#define DST_TEXT        0x0001
#define DST_PREFIXTEXT  0x0002
#define DST_ICON        0x0003
#define DST_BITMAP      0x0004

/* State type */
#define DSS_NORMAL      0x0000
#define DSS_UNION       0x0010  /* Gray string appearance */
#define DSS_DISABLED    0x0020
#define DSS_MONO        0x0080
#if(_WIN32_WINNT >= 0x0500)
#define DSS_HIDEPREFIX  0x0200
#define DSS_PREFIXONLY  0x0400
#endif /* _WIN32_WINNT >= 0x0500 */
#define DSS_RIGHT       0x8000

WINUSERAPI
BOOL
WINAPI
DrawStateA(
    __in HDC hdc,
    __in_opt HBRUSH hbrFore,
    __in_opt DRAWSTATEPROC qfnCallBack,
    __in LPARAM lData,
    __in WPARAM wData,
    __in int x,
    __in int y,
    __in int cx,
    __in int cy,
    __in UINT uFlags);
WINUSERAPI
BOOL
WINAPI
DrawStateW(
    __in HDC hdc,
    __in_opt HBRUSH hbrFore,
    __in_opt DRAWSTATEPROC qfnCallBack,
    __in LPARAM lData,
    __in WPARAM wData,
    __in int x,
    __in int y,
    __in int cx,
    __in int cy,
    __in UINT uFlags);
#ifdef UNICODE
#define DrawState  DrawStateW
#else
#define DrawState  DrawStateA
#endif // !UNICODE
#endif /* WINVER >= 0x0400 */

WINUSERAPI
LONG
WINAPI
TabbedTextOutA(
    __in HDC hdc,
    __in int x,
    __in int y,
    __in_ecount(chCount) LPCSTR lpString,
    __in int chCount,
    __in int nTabPositions,
    __in_ecount_opt(nTabPositions) CONST INT *lpnTabStopPositions,
    __in int nTabOrigin);
WINUSERAPI
LONG
WINAPI
TabbedTextOutW(
    __in HDC hdc,
    __in int x,
    __in int y,
    __in_ecount(chCount) LPCWSTR lpString,
    __in int chCount,
    __in int nTabPositions,
    __in_ecount_opt(nTabPositions) CONST INT *lpnTabStopPositions,
    __in int nTabOrigin);
#ifdef UNICODE
#define TabbedTextOut  TabbedTextOutW
#else
#define TabbedTextOut  TabbedTextOutA
#endif // !UNICODE

WINUSERAPI
DWORD
WINAPI
GetTabbedTextExtentA(
    __in HDC hdc,
    __in_ecount(chCount) LPCSTR lpString,
    __in int chCount,
    __in int nTabPositions,
    __in_ecount_opt(nTabPositions) CONST INT *lpnTabStopPositions);
WINUSERAPI
DWORD
WINAPI
GetTabbedTextExtentW(
    __in HDC hdc,
    __in_ecount(chCount) LPCWSTR lpString,
    __in int chCount,
    __in int nTabPositions,
    __in_ecount_opt(nTabPositions) CONST INT *lpnTabStopPositions);
#ifdef UNICODE
#define GetTabbedTextExtent  GetTabbedTextExtentW
#else
#define GetTabbedTextExtent  GetTabbedTextExtentA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
UpdateWindow(
    __in HWND hWnd);

WINUSERAPI
HWND
WINAPI
SetActiveWindow(
    __in HWND hWnd);

WINUSERAPI
HWND
WINAPI
GetForegroundWindow(
    VOID);

#if(WINVER >= 0x0400)
WINUSERAPI
BOOL
WINAPI
PaintDesktop(
    __in HDC hdc);

WINUSERAPI
VOID
WINAPI
SwitchToThisWindow(
    __in HWND hwnd,
    __in BOOL fUnknown);
#endif /* WINVER >= 0x0400 */

WINUSERAPI
BOOL
WINAPI
SetForegroundWindow(
    __in HWND hWnd);

#if(_WIN32_WINNT >= 0x0500)
WINUSERAPI
BOOL
WINAPI
AllowSetForegroundWindow(
    __in DWORD dwProcessId);

#define ASFW_ANY    ((DWORD)-1)

WINUSERAPI
BOOL
WINAPI
LockSetForegroundWindow(
    __in UINT uLockCode);

#define LSFW_LOCK       1
#define LSFW_UNLOCK     2

#endif /* _WIN32_WINNT >= 0x0500 */

WINUSERAPI
HWND
WINAPI
WindowFromDC(
    __in HDC hDC);

WINUSERAPI
HDC
WINAPI
GetDC(
    __in_opt HWND hWnd);

WINUSERAPI
HDC
WINAPI
GetDCEx(
    __in_opt HWND hWnd,
    __in_opt HRGN hrgnClip,
    __in DWORD flags);

/*
 * GetDCEx() flags
 */
#define DCX_WINDOW           0x00000001L
#define DCX_CACHE            0x00000002L
#define DCX_NORESETATTRS     0x00000004L
#define DCX_CLIPCHILDREN     0x00000008L
#define DCX_CLIPSIBLINGS     0x00000010L
#define DCX_PARENTCLIP       0x00000020L
#define DCX_EXCLUDERGN       0x00000040L
#define DCX_INTERSECTRGN     0x00000080L
#define DCX_EXCLUDEUPDATE    0x00000100L
#define DCX_INTERSECTUPDATE  0x00000200L
#define DCX_LOCKWINDOWUPDATE 0x00000400L

#define DCX_VALIDATE         0x00200000L

WINUSERAPI
HDC
WINAPI
GetWindowDC(
    __in_opt HWND hWnd);

WINUSERAPI
int
WINAPI
ReleaseDC(
    __in_opt HWND hWnd,
    __in HDC hDC);

WINUSERAPI
HDC
WINAPI
BeginPaint(
    __in HWND hWnd,
    __out LPPAINTSTRUCT lpPaint);

WINUSERAPI
BOOL
WINAPI
EndPaint(
    __in HWND hWnd,
    __in CONST PAINTSTRUCT *lpPaint);

WINUSERAPI
BOOL
WINAPI
GetUpdateRect(
    __in HWND hWnd,
    __out_opt LPRECT lpRect,
    __in BOOL bErase);

WINUSERAPI
int
WINAPI
GetUpdateRgn(
    __in HWND hWnd,
    __in HRGN hRgn,
    __in BOOL bErase);

WINUSERAPI
int
WINAPI
SetWindowRgn(
    __in HWND hWnd,
    __in_opt HRGN hRgn,
    __in BOOL bRedraw);


WINUSERAPI
int
WINAPI
GetWindowRgn(
    __in HWND hWnd,
    __in HRGN hRgn);

#if(_WIN32_WINNT >= 0x0501)

WINUSERAPI
int
WINAPI
GetWindowRgnBox(
    __in HWND hWnd,
    __out LPRECT lprc);

#endif /* _WIN32_WINNT >= 0x0501 */

WINUSERAPI
int
WINAPI
ExcludeUpdateRgn(
    __in HDC hDC,
    __in HWND hWnd);

WINUSERAPI
BOOL
WINAPI
InvalidateRect(
    __in_opt HWND hWnd,
    __in_opt CONST RECT *lpRect,
    __in BOOL bErase);

WINUSERAPI
BOOL
WINAPI
ValidateRect(
    __in_opt HWND hWnd,
    __in_opt CONST RECT *lpRect);

WINUSERAPI
BOOL
WINAPI
InvalidateRgn(
    __in HWND hWnd,
    __in_opt HRGN hRgn,
    __in BOOL bErase);

WINUSERAPI
BOOL
WINAPI
ValidateRgn(
    __in HWND hWnd,
    __in_opt HRGN hRgn);


WINUSERAPI
BOOL
WINAPI
RedrawWindow(
    __in_opt HWND hWnd,
    __in_opt CONST RECT *lprcUpdate,
    __in_opt HRGN hrgnUpdate,
    __in UINT flags);

/*
 * RedrawWindow() flags
 */
#define RDW_INVALIDATE          0x0001
#define RDW_INTERNALPAINT       0x0002
#define RDW_ERASE               0x0004

#define RDW_VALIDATE            0x0008
#define RDW_NOINTERNALPAINT     0x0010
#define RDW_NOERASE             0x0020

#define RDW_NOCHILDREN          0x0040
#define RDW_ALLCHILDREN         0x0080

#define RDW_UPDATENOW           0x0100
#define RDW_ERASENOW            0x0200

#define RDW_FRAME               0x0400
#define RDW_NOFRAME             0x0800


/*
 * LockWindowUpdate API
 */

WINUSERAPI
BOOL
WINAPI
LockWindowUpdate(
    __in_opt HWND hWndLock);

WINUSERAPI
BOOL
WINAPI
ScrollWindow(
    __in HWND hWnd,
    __in int XAmount,
    __in int YAmount,
    __in_opt CONST RECT *lpRect,
    __in_opt CONST RECT *lpClipRect);

WINUSERAPI
BOOL
WINAPI
ScrollDC(
    __in HDC hDC,
    __in int dx,
    __in int dy,
    __in_opt CONST RECT *lprcScroll,
    __in_opt CONST RECT *lprcClip,
    __in_opt HRGN hrgnUpdate,
    __out_opt LPRECT lprcUpdate);

WINUSERAPI
int
WINAPI
ScrollWindowEx(
    __in HWND hWnd,
    __in int dx,
    __in int dy,
    __in_opt CONST RECT *prcScroll,
    __in_opt CONST RECT *prcClip,
    __in_opt HRGN hrgnUpdate,
    __out_opt LPRECT prcUpdate,
    __in UINT flags);

#define SW_SCROLLCHILDREN   0x0001  /* Scroll children within *lprcScroll. */
#define SW_INVALIDATE       0x0002  /* Invalidate after scrolling */
#define SW_ERASE            0x0004  /* If SW_INVALIDATE, don't send WM_ERASEBACKGROUND */
#if(WINVER >= 0x0500)
#define SW_SMOOTHSCROLL     0x0010  /* Use smooth scrolling */
#endif /* WINVER >= 0x0500 */

#ifndef NOSCROLL

WINUSERAPI
int
WINAPI
SetScrollPos(
    __in HWND hWnd,
    __in int nBar,
    __in int nPos,
    __in BOOL bRedraw);

WINUSERAPI
int
WINAPI
GetScrollPos(
    __in HWND hWnd,
    __in int nBar);

WINUSERAPI
BOOL
WINAPI
SetScrollRange(
    __in HWND hWnd,
    __in int nBar,
    __in int nMinPos,
    __in int nMaxPos,
    __in BOOL bRedraw);

WINUSERAPI
BOOL
WINAPI
GetScrollRange(
    __in HWND hWnd,
    __in int nBar,
    __out LPINT lpMinPos,
    __out LPINT lpMaxPos);

WINUSERAPI
BOOL
WINAPI
ShowScrollBar(
    __in HWND hWnd,
    __in int wBar,
    __in BOOL bShow);

WINUSERAPI
BOOL
WINAPI
EnableScrollBar(
    __in HWND hWnd,
    __in UINT wSBflags,
    __in UINT wArrows);


/*
 * EnableScrollBar() flags
 */
#define ESB_ENABLE_BOTH     0x0000
#define ESB_DISABLE_BOTH    0x0003

#define ESB_DISABLE_LEFT    0x0001
#define ESB_DISABLE_RIGHT   0x0002

#define ESB_DISABLE_UP      0x0001
#define ESB_DISABLE_DOWN    0x0002

#define ESB_DISABLE_LTUP    ESB_DISABLE_LEFT
#define ESB_DISABLE_RTDN    ESB_DISABLE_RIGHT


#endif  /* !NOSCROLL */

WINUSERAPI
BOOL
WINAPI
SetPropA(
    __in HWND hWnd,
    __in LPCSTR lpString,
    __in_opt HANDLE hData);
WINUSERAPI
BOOL
WINAPI
SetPropW(
    __in HWND hWnd,
    __in LPCWSTR lpString,
    __in_opt HANDLE hData);
#ifdef UNICODE
#define SetProp  SetPropW
#else
#define SetProp  SetPropA
#endif // !UNICODE

WINUSERAPI
HANDLE
WINAPI
GetPropA(
    __in HWND hWnd,
    __in LPCSTR lpString);
WINUSERAPI
HANDLE
WINAPI
GetPropW(
    __in HWND hWnd,
    __in LPCWSTR lpString);
#ifdef UNICODE
#define GetProp  GetPropW
#else
#define GetProp  GetPropA
#endif // !UNICODE

WINUSERAPI
HANDLE
WINAPI
RemovePropA(
    __in HWND hWnd,
    __in LPCSTR lpString);
WINUSERAPI
HANDLE
WINAPI
RemovePropW(
    __in HWND hWnd,
    __in LPCWSTR lpString);
#ifdef UNICODE
#define RemoveProp  RemovePropW
#else
#define RemoveProp  RemovePropA
#endif // !UNICODE

WINUSERAPI
int
WINAPI
EnumPropsExA(
    __in HWND hWnd,
    __in PROPENUMPROCEXA lpEnumFunc,
    __in LPARAM lParam);
WINUSERAPI
int
WINAPI
EnumPropsExW(
    __in HWND hWnd,
    __in PROPENUMPROCEXW lpEnumFunc,
    __in LPARAM lParam);
#ifdef UNICODE
#define EnumPropsEx  EnumPropsExW
#else
#define EnumPropsEx  EnumPropsExA
#endif // !UNICODE

WINUSERAPI
int
WINAPI
EnumPropsA(
    __in HWND hWnd,
    __in PROPENUMPROCA lpEnumFunc);
WINUSERAPI
int
WINAPI
EnumPropsW(
    __in HWND hWnd,
    __in PROPENUMPROCW lpEnumFunc);
#ifdef UNICODE
#define EnumProps  EnumPropsW
#else
#define EnumProps  EnumPropsA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
SetWindowTextA(
    __in HWND hWnd,
    __in_opt LPCSTR lpString);
WINUSERAPI
BOOL
WINAPI
SetWindowTextW(
    __in HWND hWnd,
    __in_opt LPCWSTR lpString);
#ifdef UNICODE
#define SetWindowText  SetWindowTextW
#else
#define SetWindowText  SetWindowTextA
#endif // !UNICODE

WINUSERAPI
int
WINAPI
GetWindowTextA(
    __in HWND hWnd,
    __out_ecount(nMaxCount) LPSTR lpString,
    __in int nMaxCount);
WINUSERAPI
int
WINAPI
GetWindowTextW(
    __in HWND hWnd,
    __out_ecount(nMaxCount) LPWSTR lpString,
    __in int nMaxCount);
#ifdef UNICODE
#define GetWindowText  GetWindowTextW
#else
#define GetWindowText  GetWindowTextA
#endif // !UNICODE

WINUSERAPI
int
WINAPI
GetWindowTextLengthA(
    __in HWND hWnd);
WINUSERAPI
int
WINAPI
GetWindowTextLengthW(
    __in HWND hWnd);
#ifdef UNICODE
#define GetWindowTextLength  GetWindowTextLengthW
#else
#define GetWindowTextLength  GetWindowTextLengthA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
GetClientRect(
    __in HWND hWnd,
    __out LPRECT lpRect);

WINUSERAPI
BOOL
WINAPI
GetWindowRect(
    __in HWND hWnd,
    __out LPRECT lpRect);

WINUSERAPI
BOOL
WINAPI
AdjustWindowRect(
    __inout LPRECT lpRect,
    __in DWORD dwStyle,
    __in BOOL bMenu);

WINUSERAPI
BOOL
WINAPI
AdjustWindowRectEx(
    __inout LPRECT lpRect,
    __in DWORD dwStyle,
    __in BOOL bMenu,
    __in DWORD dwExStyle);


#if(WINVER >= 0x0400)
#define HELPINFO_WINDOW    0x0001
#define HELPINFO_MENUITEM  0x0002
typedef struct tagHELPINFO      /* Structure pointed to by lParam of WM_HELP */
{
    UINT    cbSize;             /* Size in bytes of this struct  */
    int     iContextType;       /* Either HELPINFO_WINDOW or HELPINFO_MENUITEM */
    int     iCtrlId;            /* Control Id or a Menu item Id. */
    HANDLE  hItemHandle;        /* hWnd of control or hMenu.     */
    DWORD_PTR dwContextId;      /* Context Id associated with this item */
    POINT   MousePos;           /* Mouse Position in screen co-ordinates */
}  HELPINFO, FAR *LPHELPINFO;

WINUSERAPI
BOOL
WINAPI
SetWindowContextHelpId(
    __in HWND,
    __in DWORD);

WINUSERAPI
DWORD
WINAPI
GetWindowContextHelpId(
    __in HWND);

WINUSERAPI
BOOL
WINAPI
SetMenuContextHelpId(
    __in HMENU,
    __in DWORD);

WINUSERAPI
DWORD
WINAPI
GetMenuContextHelpId(
    __in HMENU);

#endif /* WINVER >= 0x0400 */


#ifndef NOMB

/*
 * MessageBox() Flags
 */
#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L
#if(WINVER >= 0x0500)
#define MB_CANCELTRYCONTINUE        0x00000006L
#endif /* WINVER >= 0x0500 */


#define MB_ICONHAND                 0x00000010L
#define MB_ICONQUESTION             0x00000020L
#define MB_ICONEXCLAMATION          0x00000030L
#define MB_ICONASTERISK             0x00000040L

#if(WINVER >= 0x0400)
#define MB_USERICON                 0x00000080L
#define MB_ICONWARNING              MB_ICONEXCLAMATION
#define MB_ICONERROR                MB_ICONHAND
#endif /* WINVER >= 0x0400 */

#define MB_ICONINFORMATION          MB_ICONASTERISK
#define MB_ICONSTOP                 MB_ICONHAND

#define MB_DEFBUTTON1               0x00000000L
#define MB_DEFBUTTON2               0x00000100L
#define MB_DEFBUTTON3               0x00000200L
#if(WINVER >= 0x0400)
#define MB_DEFBUTTON4               0x00000300L
#endif /* WINVER >= 0x0400 */

#define MB_APPLMODAL                0x00000000L
#define MB_SYSTEMMODAL              0x00001000L
#define MB_TASKMODAL                0x00002000L
#if(WINVER >= 0x0400)
#define MB_HELP                     0x00004000L // Help Button
#endif /* WINVER >= 0x0400 */

#define MB_NOFOCUS                  0x00008000L
#define MB_SETFOREGROUND            0x00010000L
#define MB_DEFAULT_DESKTOP_ONLY     0x00020000L

#if(WINVER >= 0x0400)
#define MB_TOPMOST                  0x00040000L
#define MB_RIGHT                    0x00080000L
#define MB_RTLREADING               0x00100000L


#endif /* WINVER >= 0x0400 */

#ifdef _WIN32_WINNT
#if (_WIN32_WINNT >= 0x0400)
#define MB_SERVICE_NOTIFICATION          0x00200000L
#else
#define MB_SERVICE_NOTIFICATION          0x00040000L
#endif
#define MB_SERVICE_NOTIFICATION_NT3X     0x00040000L
#endif

#define MB_TYPEMASK                 0x0000000FL
#define MB_ICONMASK                 0x000000F0L
#define MB_DEFMASK                  0x00000F00L
#define MB_MODEMASK                 0x00003000L
#define MB_MISCMASK                 0x0000C000L

WINUSERAPI
int
WINAPI
MessageBoxA(
    __in_opt HWND hWnd,
    __in_opt LPCSTR lpText,
    __in_opt LPCSTR lpCaption,
    __in UINT uType);
WINUSERAPI
int
WINAPI
MessageBoxW(
    __in_opt HWND hWnd,
    __in_opt LPCWSTR lpText,
    __in_opt LPCWSTR lpCaption,
    __in UINT uType);
#ifdef UNICODE
#define MessageBox  MessageBoxW
#else
#define MessageBox  MessageBoxA
#endif // !UNICODE
    
#if defined(_M_CEE)
#undef MessageBox
__inline
int
MessageBox(
    HWND hWnd,
    LPCTSTR lpText,
    LPCTSTR lpCaption,
    UINT uType
    )
{
#ifdef UNICODE
    return MessageBoxW(
#else
    return MessageBoxA(
#endif
        hWnd,
	lpText,
	lpCaption,
	uType
        );
}
#endif	/* _M_CEE */

WINUSERAPI
int
WINAPI
MessageBoxExA(
    __in_opt HWND hWnd,
    __in_opt LPCSTR lpText,
    __in_opt LPCSTR lpCaption,
    __in UINT uType,
    __in WORD wLanguageId);
WINUSERAPI
int
WINAPI
MessageBoxExW(
    __in_opt HWND hWnd,
    __in_opt LPCWSTR lpText,
    __in_opt LPCWSTR lpCaption,
    __in UINT uType,
    __in WORD wLanguageId);
#ifdef UNICODE
#define MessageBoxEx  MessageBoxExW
#else
#define MessageBoxEx  MessageBoxExA
#endif // !UNICODE

#if(WINVER >= 0x0400)

typedef VOID (CALLBACK *MSGBOXCALLBACK)(LPHELPINFO lpHelpInfo);

typedef struct tagMSGBOXPARAMSA
{
    UINT        cbSize;
    HWND        hwndOwner;
    HINSTANCE   hInstance;
    LPCSTR      lpszText;
    LPCSTR      lpszCaption;
    DWORD       dwStyle;
    LPCSTR      lpszIcon;
    DWORD_PTR   dwContextHelpId;
    MSGBOXCALLBACK      lpfnMsgBoxCallback;
    DWORD       dwLanguageId;
} MSGBOXPARAMSA, *PMSGBOXPARAMSA, *LPMSGBOXPARAMSA;
typedef struct tagMSGBOXPARAMSW
{
    UINT        cbSize;
    HWND        hwndOwner;
    HINSTANCE   hInstance;
    LPCWSTR     lpszText;
    LPCWSTR     lpszCaption;
    DWORD       dwStyle;
    LPCWSTR     lpszIcon;
    DWORD_PTR   dwContextHelpId;
    MSGBOXCALLBACK      lpfnMsgBoxCallback;
    DWORD       dwLanguageId;
} MSGBOXPARAMSW, *PMSGBOXPARAMSW, *LPMSGBOXPARAMSW;
#ifdef UNICODE
typedef MSGBOXPARAMSW MSGBOXPARAMS;
typedef PMSGBOXPARAMSW PMSGBOXPARAMS;
typedef LPMSGBOXPARAMSW LPMSGBOXPARAMS;
#else
typedef MSGBOXPARAMSA MSGBOXPARAMS;
typedef PMSGBOXPARAMSA PMSGBOXPARAMS;
typedef LPMSGBOXPARAMSA LPMSGBOXPARAMS;
#endif // UNICODE

WINUSERAPI
int
WINAPI
MessageBoxIndirectA(
    __in CONST MSGBOXPARAMSA * lpmbp);
WINUSERAPI
int
WINAPI
MessageBoxIndirectW(
    __in CONST MSGBOXPARAMSW * lpmbp);
#ifdef UNICODE
#define MessageBoxIndirect  MessageBoxIndirectW
#else
#define MessageBoxIndirect  MessageBoxIndirectA
#endif // !UNICODE
#endif /* WINVER >= 0x0400 */


WINUSERAPI
BOOL
WINAPI
MessageBeep(
    __in UINT uType);

#endif /* !NOMB */

WINUSERAPI
int
WINAPI
ShowCursor(
    __in BOOL bShow);

WINUSERAPI
BOOL
WINAPI
SetCursorPos(
    __in int X,
    __in int Y);

WINUSERAPI
HCURSOR
WINAPI
SetCursor(
    __in_opt HCURSOR hCursor);

WINUSERAPI
BOOL
WINAPI
GetCursorPos(
    __out LPPOINT lpPoint);

WINUSERAPI
BOOL
WINAPI
ClipCursor(
    __in_opt CONST RECT *lpRect);

WINUSERAPI
BOOL
WINAPI
GetClipCursor(
    __out LPRECT lpRect);

WINUSERAPI
HCURSOR
WINAPI
GetCursor(
    VOID);

WINUSERAPI
BOOL
WINAPI
CreateCaret(
    __in HWND hWnd,
    __in_opt HBITMAP hBitmap,
    __in int nWidth,
    __in int nHeight);

WINUSERAPI
UINT
WINAPI
GetCaretBlinkTime(
    VOID);

WINUSERAPI
BOOL
WINAPI
SetCaretBlinkTime(
    __in UINT uMSeconds);

WINUSERAPI
BOOL
WINAPI
DestroyCaret(
    VOID);

WINUSERAPI
BOOL
WINAPI
HideCaret(
    __in_opt HWND hWnd);

WINUSERAPI
BOOL
WINAPI
ShowCaret(
    __in_opt HWND hWnd);

WINUSERAPI
BOOL
WINAPI
SetCaretPos(
    __in int X,
    __in int Y);

WINUSERAPI
BOOL
WINAPI
GetCaretPos(
    __out LPPOINT lpPoint);

WINUSERAPI
BOOL
WINAPI
ClientToScreen(
    __in HWND hWnd,
    __inout LPPOINT lpPoint);

WINUSERAPI
BOOL
WINAPI
ScreenToClient(
    __in HWND hWnd,
    __inout LPPOINT lpPoint);

WINUSERAPI
int
WINAPI
MapWindowPoints(
    __in_opt HWND hWndFrom,
    __in_opt HWND hWndTo,
    __inout_ecount(cPoints) LPPOINT lpPoints,
    __in UINT cPoints);

WINUSERAPI
HWND
WINAPI
WindowFromPoint(
    __in POINT Point);

WINUSERAPI
HWND
WINAPI
ChildWindowFromPoint(
    __in HWND hWndParent,
    __in POINT Point);

#if(WINVER >= 0x0400)
#define CWP_ALL             0x0000
#define CWP_SKIPINVISIBLE   0x0001
#define CWP_SKIPDISABLED    0x0002
#define CWP_SKIPTRANSPARENT 0x0004

WINUSERAPI
HWND
WINAPI
ChildWindowFromPointEx(
    __in HWND hwnd,
    __in POINT pt,
    __in UINT flags);
#endif /* WINVER >= 0x0400 */

#ifndef NOCOLOR

/*
 * Color Types
 */
#define CTLCOLOR_MSGBOX         0
#define CTLCOLOR_EDIT           1
#define CTLCOLOR_LISTBOX        2
#define CTLCOLOR_BTN            3
#define CTLCOLOR_DLG            4
#define CTLCOLOR_SCROLLBAR      5
#define CTLCOLOR_STATIC         6
#define CTLCOLOR_MAX            7

#define COLOR_SCROLLBAR         0
#define COLOR_BACKGROUND        1
#define COLOR_ACTIVECAPTION     2
#define COLOR_INACTIVECAPTION   3
#define COLOR_MENU              4
#define COLOR_WINDOW            5
#define COLOR_WINDOWFRAME       6
#define COLOR_MENUTEXT          7
#define COLOR_WINDOWTEXT        8
#define COLOR_CAPTIONTEXT       9
#define COLOR_ACTIVEBORDER      10
#define COLOR_INACTIVEBORDER    11
#define COLOR_APPWORKSPACE      12
#define COLOR_HIGHLIGHT         13
#define COLOR_HIGHLIGHTTEXT     14
#define COLOR_BTNFACE           15
#define COLOR_BTNSHADOW         16
#define COLOR_GRAYTEXT          17
#define COLOR_BTNTEXT           18
#define COLOR_INACTIVECAPTIONTEXT 19
#define COLOR_BTNHIGHLIGHT      20

#if(WINVER >= 0x0400)
#define COLOR_3DDKSHADOW        21
#define COLOR_3DLIGHT           22
#define COLOR_INFOTEXT          23
#define COLOR_INFOBK            24
#endif /* WINVER >= 0x0400 */

#if(WINVER >= 0x0500)
#define COLOR_HOTLIGHT          26
#define COLOR_GRADIENTACTIVECAPTION 27
#define COLOR_GRADIENTINACTIVECAPTION 28
#if(WINVER >= 0x0501)
#define COLOR_MENUHILIGHT       29
#define COLOR_MENUBAR           30
#endif /* WINVER >= 0x0501 */
#endif /* WINVER >= 0x0500 */

#if(WINVER >= 0x0400)
#define COLOR_DESKTOP           COLOR_BACKGROUND
#define COLOR_3DFACE            COLOR_BTNFACE
#define COLOR_3DSHADOW          COLOR_BTNSHADOW
#define COLOR_3DHIGHLIGHT       COLOR_BTNHIGHLIGHT
#define COLOR_3DHILIGHT         COLOR_BTNHIGHLIGHT
#define COLOR_BTNHILIGHT        COLOR_BTNHIGHLIGHT
#endif /* WINVER >= 0x0400 */


WINUSERAPI
DWORD
WINAPI
GetSysColor(
    __in int nIndex);

#if(WINVER >= 0x0400)
WINUSERAPI
HBRUSH
WINAPI
GetSysColorBrush(
    __in int nIndex);


#endif /* WINVER >= 0x0400 */

WINUSERAPI
BOOL
WINAPI
SetSysColors(
    __in int cElements,
    __in_ecount(cElements) CONST INT * lpaElements,
    __in_ecount(cElements) CONST COLORREF * lpaRgbValues);

#endif /* !NOCOLOR */

WINUSERAPI
BOOL
WINAPI
DrawFocusRect(
    __in HDC hDC,
    __in CONST RECT * lprc);

WINUSERAPI
int
WINAPI
FillRect(
    __in HDC hDC,
    __in CONST RECT *lprc,
    __in HBRUSH hbr);

WINUSERAPI
int
WINAPI
FrameRect(
    __in HDC hDC,
    __in CONST RECT *lprc,
    __in HBRUSH hbr);

WINUSERAPI
BOOL
WINAPI
InvertRect(
    __in HDC hDC,
    __in CONST RECT *lprc);

WINUSERAPI
BOOL
WINAPI
SetRect(
    __out LPRECT lprc,
    __in int xLeft,
    __in int yTop,
    __in int xRight,
    __in int yBottom);

WINUSERAPI
BOOL
WINAPI
SetRectEmpty(
    __out LPRECT lprc);

WINUSERAPI
BOOL
WINAPI
CopyRect(
    __out LPRECT lprcDst,
    __in CONST RECT *lprcSrc);

WINUSERAPI
BOOL
WINAPI
InflateRect(
    __inout LPRECT lprc,
    __in int dx,
    __in int dy);

WINUSERAPI
BOOL
WINAPI
IntersectRect(
    __out LPRECT lprcDst,
    __in CONST RECT *lprcSrc1,
    __in CONST RECT *lprcSrc2);

WINUSERAPI
BOOL
WINAPI
UnionRect(
    __out LPRECT lprcDst,
    __in CONST RECT *lprcSrc1,
    __in CONST RECT *lprcSrc2);

WINUSERAPI
BOOL
WINAPI
SubtractRect(
    __out LPRECT lprcDst,
    __in CONST RECT *lprcSrc1,
    __in CONST RECT *lprcSrc2);

WINUSERAPI
BOOL
WINAPI
OffsetRect(
    __inout LPRECT lprc,
    __in int dx,
    __in int dy);

WINUSERAPI
BOOL
WINAPI
IsRectEmpty(
    __in CONST RECT *lprc);

WINUSERAPI
BOOL
WINAPI
EqualRect(
    __in CONST RECT *lprc1,
    __in CONST RECT *lprc2);

WINUSERAPI
BOOL
WINAPI
PtInRect(
    __in CONST RECT *lprc,
    __in POINT pt);

#ifndef NOWINOFFSETS

WINUSERAPI
WORD
WINAPI
GetWindowWord(
    __in HWND hWnd,
    __in int nIndex);

WINUSERAPI
WORD
WINAPI
SetWindowWord(
    __in HWND hWnd,
    __in int nIndex,
    __in WORD wNewWord);

WINUSERAPI
LONG
WINAPI
GetWindowLongA(
    __in HWND hWnd,
    __in int nIndex);
WINUSERAPI
LONG
WINAPI
GetWindowLongW(
    __in HWND hWnd,
    __in int nIndex);
#ifdef UNICODE
#define GetWindowLong  GetWindowLongW
#else
#define GetWindowLong  GetWindowLongA
#endif // !UNICODE

WINUSERAPI
LONG
WINAPI
SetWindowLongA(
    __in HWND hWnd,
    __in int nIndex,
    __in LONG dwNewLong);
WINUSERAPI
LONG
WINAPI
SetWindowLongW(
    __in HWND hWnd,
    __in int nIndex,
    __in LONG dwNewLong);
#ifdef UNICODE
#define SetWindowLong  SetWindowLongW
#else
#define SetWindowLong  SetWindowLongA
#endif // !UNICODE

#ifdef _WIN64

WINUSERAPI
LONG_PTR
WINAPI
GetWindowLongPtrA(
    __in HWND hWnd,
    __in int nIndex);
WINUSERAPI
LONG_PTR
WINAPI
GetWindowLongPtrW(
    __in HWND hWnd,
    __in int nIndex);
#ifdef UNICODE
#define GetWindowLongPtr  GetWindowLongPtrW
#else
#define GetWindowLongPtr  GetWindowLongPtrA
#endif // !UNICODE

WINUSERAPI
LONG_PTR
WINAPI
SetWindowLongPtrA(
    __in HWND hWnd,
    __in int nIndex,
    __in LONG_PTR dwNewLong);
WINUSERAPI
LONG_PTR
WINAPI
SetWindowLongPtrW(
    __in HWND hWnd,
    __in int nIndex,
    __in LONG_PTR dwNewLong);
#ifdef UNICODE
#define SetWindowLongPtr  SetWindowLongPtrW
#else
#define SetWindowLongPtr  SetWindowLongPtrA
#endif // !UNICODE

#else  /* _WIN64 */

#define GetWindowLongPtrA   GetWindowLongA
#define GetWindowLongPtrW   GetWindowLongW
#ifdef UNICODE
#define GetWindowLongPtr  GetWindowLongPtrW
#else
#define GetWindowLongPtr  GetWindowLongPtrA
#endif // !UNICODE

#define SetWindowLongPtrA   SetWindowLongA
#define SetWindowLongPtrW   SetWindowLongW
#ifdef UNICODE
#define SetWindowLongPtr  SetWindowLongPtrW
#else
#define SetWindowLongPtr  SetWindowLongPtrA
#endif // !UNICODE

#endif /* _WIN64 */

WINUSERAPI
WORD
WINAPI
GetClassWord(
    __in HWND hWnd,
    __in int nIndex);

WINUSERAPI
WORD
WINAPI
SetClassWord(
    __in HWND hWnd,
    __in int nIndex,
    __in WORD wNewWord);

WINUSERAPI
DWORD
WINAPI
GetClassLongA(
    __in HWND hWnd,
    __in int nIndex);
WINUSERAPI
DWORD
WINAPI
GetClassLongW(
    __in HWND hWnd,
    __in int nIndex);
#ifdef UNICODE
#define GetClassLong  GetClassLongW
#else
#define GetClassLong  GetClassLongA
#endif // !UNICODE

WINUSERAPI
DWORD
WINAPI
SetClassLongA(
    __in HWND hWnd,
    __in int nIndex,
    __in LONG dwNewLong);
WINUSERAPI
DWORD
WINAPI
SetClassLongW(
    __in HWND hWnd,
    __in int nIndex,
    __in LONG dwNewLong);
#ifdef UNICODE
#define SetClassLong  SetClassLongW
#else
#define SetClassLong  SetClassLongA
#endif // !UNICODE

#ifdef _WIN64

WINUSERAPI
ULONG_PTR
WINAPI
GetClassLongPtrA(
    __in HWND hWnd,
    __in int nIndex);
WINUSERAPI
ULONG_PTR
WINAPI
GetClassLongPtrW(
    __in HWND hWnd,
    __in int nIndex);
#ifdef UNICODE
#define GetClassLongPtr  GetClassLongPtrW
#else
#define GetClassLongPtr  GetClassLongPtrA
#endif // !UNICODE

WINUSERAPI
ULONG_PTR
WINAPI
SetClassLongPtrA(
    __in HWND hWnd,
    __in int nIndex,
    __in LONG_PTR dwNewLong);
WINUSERAPI
ULONG_PTR
WINAPI
SetClassLongPtrW(
    __in HWND hWnd,
    __in int nIndex,
    __in LONG_PTR dwNewLong);
#ifdef UNICODE
#define SetClassLongPtr  SetClassLongPtrW
#else
#define SetClassLongPtr  SetClassLongPtrA
#endif // !UNICODE

#else  /* _WIN64 */

#define GetClassLongPtrA    GetClassLongA
#define GetClassLongPtrW    GetClassLongW
#ifdef UNICODE
#define GetClassLongPtr  GetClassLongPtrW
#else
#define GetClassLongPtr  GetClassLongPtrA
#endif // !UNICODE

#define SetClassLongPtrA    SetClassLongA
#define SetClassLongPtrW    SetClassLongW
#ifdef UNICODE
#define SetClassLongPtr  SetClassLongPtrW
#else
#define SetClassLongPtr  SetClassLongPtrA
#endif // !UNICODE

#endif /* _WIN64 */

#endif /* !NOWINOFFSETS */

#if(WINVER >= 0x0500)
WINUSERAPI
BOOL
WINAPI
GetProcessDefaultLayout(
    __out DWORD *pdwDefaultLayout);

WINUSERAPI
BOOL
WINAPI
SetProcessDefaultLayout(
    __in DWORD dwDefaultLayout);
#endif /* WINVER >= 0x0500 */

WINUSERAPI
HWND
WINAPI
GetDesktopWindow(
    VOID);


WINUSERAPI
HWND
WINAPI
GetParent(
    __in HWND hWnd);

WINUSERAPI
HWND
WINAPI
SetParent(
    __in HWND hWndChild,
    __in_opt HWND hWndNewParent);

WINUSERAPI
BOOL
WINAPI
EnumChildWindows(
    __in_opt HWND hWndParent,
    __in WNDENUMPROC lpEnumFunc,
    __in LPARAM lParam);

WINUSERAPI
HWND
WINAPI
FindWindowA(
    __in_opt LPCSTR lpClassName,
    __in_opt LPCSTR lpWindowName);
WINUSERAPI
HWND
WINAPI
FindWindowW(
    __in_opt LPCWSTR lpClassName,
    __in_opt LPCWSTR lpWindowName);
#ifdef UNICODE
#define FindWindow  FindWindowW
#else
#define FindWindow  FindWindowA
#endif // !UNICODE

#if(WINVER >= 0x0400)
WINUSERAPI
HWND
WINAPI
FindWindowExA(
    __in_opt HWND hWndParent,
    __in_opt HWND hWndChildAfter,
    __in_opt LPCSTR lpszClass,
    __in_opt LPCSTR lpszWindow);
WINUSERAPI
HWND
WINAPI
FindWindowExW(
    __in_opt HWND hWndParent,
    __in_opt HWND hWndChildAfter,
    __in_opt LPCWSTR lpszClass,
    __in_opt LPCWSTR lpszWindow);
#ifdef UNICODE
#define FindWindowEx  FindWindowExW
#else
#define FindWindowEx  FindWindowExA
#endif // !UNICODE

WINUSERAPI
HWND
WINAPI
GetShellWindow(
    VOID);

#endif /* WINVER >= 0x0400 */


WINUSERAPI
BOOL
WINAPI
RegisterShellHookWindow(
    __in HWND hwnd);

WINUSERAPI
BOOL
WINAPI
DeregisterShellHookWindow(
    __in HWND hwnd);

WINUSERAPI
BOOL
WINAPI
EnumWindows(
    __in WNDENUMPROC lpEnumFunc,
    __in LPARAM lParam);

WINUSERAPI
BOOL
WINAPI
EnumThreadWindows(
    __in DWORD dwThreadId,
    __in WNDENUMPROC lpfn,
    __in LPARAM lParam);

#define EnumTaskWindows(hTask, lpfn, lParam) EnumThreadWindows(HandleToUlong(hTask), lpfn, lParam)

WINUSERAPI
int
WINAPI
GetClassNameA(
    __in HWND hWnd,
    __out_ecount_part(nMaxCount, return) LPSTR lpClassName,
    __in int nMaxCount
    );
WINUSERAPI
int
WINAPI
GetClassNameW(
    __in HWND hWnd,
    __out_ecount_part(nMaxCount, return) LPWSTR lpClassName,
    __in int nMaxCount
    );
#ifdef UNICODE
#define GetClassName  GetClassNameW
#else
#define GetClassName  GetClassNameA
#endif // !UNICODE
    
#if defined(_M_CEE)
#undef GetClassName
__inline
int
GetClassName(
    HWND hWnd,
    LPTSTR lpClassName,
    int nMaxCount
    )
{
#ifdef UNICODE
    return GetClassNameW(
#else
    return GetClassNameA(
#endif
        hWnd,
	lpClassName,
	nMaxCount
        );
}
#endif	/* _M_CEE */



WINUSERAPI
HWND
WINAPI
GetTopWindow(
    __in_opt HWND hWnd);

#define GetNextWindow(hWnd, wCmd) GetWindow(hWnd, wCmd)
#define GetSysModalWindow() (NULL)
#define SetSysModalWindow(hWnd) (NULL)

WINUSERAPI
DWORD
WINAPI
GetWindowThreadProcessId(
    __in HWND hWnd,
    __out_opt LPDWORD lpdwProcessId);

#if(_WIN32_WINNT >= 0x0501)
WINUSERAPI
BOOL
WINAPI
IsGUIThread(
    __in BOOL bConvert);

#endif /* _WIN32_WINNT >= 0x0501 */

#define GetWindowTask(hWnd) \
        ((HANDLE)(DWORD_PTR)GetWindowThreadProcessId(hWnd, NULL))

WINUSERAPI
HWND
WINAPI
GetLastActivePopup(
    __in HWND hWnd);

/*
 * GetWindow() Constants
 */
#define GW_HWNDFIRST        0
#define GW_HWNDLAST         1
#define GW_HWNDNEXT         2
#define GW_HWNDPREV         3
#define GW_OWNER            4
#define GW_CHILD            5
#if(WINVER <= 0x0400)
#define GW_MAX              5
#else
#define GW_ENABLEDPOPUP     6
#define GW_MAX              6
#endif

WINUSERAPI
HWND
WINAPI
GetWindow(
    __in HWND hWnd,
    __in UINT uCmd);


#ifndef NOWH

#ifdef STRICT

WINUSERAPI
HHOOK
WINAPI
SetWindowsHookA(
    __in int nFilterType,
    __in HOOKPROC pfnFilterProc);
WINUSERAPI
HHOOK
WINAPI
SetWindowsHookW(
    __in int nFilterType,
    __in HOOKPROC pfnFilterProc);
#ifdef UNICODE
#define SetWindowsHook  SetWindowsHookW
#else
#define SetWindowsHook  SetWindowsHookA
#endif // !UNICODE

#else /* !STRICT */

WINUSERAPI
HOOKPROC
WINAPI
SetWindowsHookA(
    __in int nFilterType,
    __in HOOKPROC pfnFilterProc);
WINUSERAPI
HOOKPROC
WINAPI
SetWindowsHookW(
    __in int nFilterType,
    __in HOOKPROC pfnFilterProc);
#ifdef UNICODE
#define SetWindowsHook  SetWindowsHookW
#else
#define SetWindowsHook  SetWindowsHookA
#endif // !UNICODE

#endif /* !STRICT */

WINUSERAPI
BOOL
WINAPI
UnhookWindowsHook(
    __in int nCode,
    __in HOOKPROC pfnFilterProc);

WINUSERAPI
HHOOK
WINAPI
SetWindowsHookExA(
    __in int idHook,
    __in HOOKPROC lpfn,
    __in_opt HINSTANCE hmod,
    __in DWORD dwThreadId);
WINUSERAPI
HHOOK
WINAPI
SetWindowsHookExW(
    __in int idHook,
    __in HOOKPROC lpfn,
    __in_opt HINSTANCE hmod,
    __in DWORD dwThreadId);
#ifdef UNICODE
#define SetWindowsHookEx  SetWindowsHookExW
#else
#define SetWindowsHookEx  SetWindowsHookExA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
UnhookWindowsHookEx(
    __in HHOOK hhk);

WINUSERAPI
LRESULT
WINAPI
CallNextHookEx(
    __in_opt HHOOK hhk,
    __in int nCode,
    __in WPARAM wParam,
    __in LPARAM lParam);

/*
 * Macros for source-level compatibility with old functions.
 */
#ifdef STRICT
#define DefHookProc(nCode, wParam, lParam, phhk)\
        CallNextHookEx(*phhk, nCode, wParam, lParam)
#else
#define DefHookProc(nCode, wParam, lParam, phhk)\
        CallNextHookEx((HHOOK)*phhk, nCode, wParam, lParam)
#endif /* STRICT */
#endif /* !NOWH */

#ifndef NOMENUS


/* ;win40  -- A lot of MF_* flags have been renamed as MFT_* and MFS_* flags */
/*
 * Menu flags for Add/Check/EnableMenuItem()
 */
#define MF_INSERT           0x00000000L
#define MF_CHANGE           0x00000080L
#define MF_APPEND           0x00000100L
#define MF_DELETE           0x00000200L
#define MF_REMOVE           0x00001000L

#define MF_BYCOMMAND        0x00000000L
#define MF_BYPOSITION       0x00000400L

#define MF_SEPARATOR        0x00000800L

#define MF_ENABLED          0x00000000L
#define MF_GRAYED           0x00000001L
#define MF_DISABLED         0x00000002L

#define MF_UNCHECKED        0x00000000L
#define MF_CHECKED          0x00000008L
#define MF_USECHECKBITMAPS  0x00000200L

#define MF_STRING           0x00000000L
#define MF_BITMAP           0x00000004L
#define MF_OWNERDRAW        0x00000100L

#define MF_POPUP            0x00000010L
#define MF_MENUBARBREAK     0x00000020L
#define MF_MENUBREAK        0x00000040L

#define MF_UNHILITE         0x00000000L
#define MF_HILITE           0x00000080L

#if(WINVER >= 0x0400)
#define MF_DEFAULT          0x00001000L
#endif /* WINVER >= 0x0400 */
#define MF_SYSMENU          0x00002000L
#define MF_HELP             0x00004000L
#if(WINVER >= 0x0400)
#define MF_RIGHTJUSTIFY     0x00004000L
#endif /* WINVER >= 0x0400 */

#define MF_MOUSESELECT      0x00008000L
#if(WINVER >= 0x0400)
#define MF_END              0x00000080L  /* Obsolete -- only used by old RES files */
#endif /* WINVER >= 0x0400 */


#if(WINVER >= 0x0400)
#define MFT_STRING          MF_STRING
#define MFT_BITMAP          MF_BITMAP
#define MFT_MENUBARBREAK    MF_MENUBARBREAK
#define MFT_MENUBREAK       MF_MENUBREAK
#define MFT_OWNERDRAW       MF_OWNERDRAW
#define MFT_RADIOCHECK      0x00000200L
#define MFT_SEPARATOR       MF_SEPARATOR
#define MFT_RIGHTORDER      0x00002000L
#define MFT_RIGHTJUSTIFY    MF_RIGHTJUSTIFY

/* Menu flags for Add/Check/EnableMenuItem() */
#define MFS_GRAYED          0x00000003L
#define MFS_DISABLED        MFS_GRAYED
#define MFS_CHECKED         MF_CHECKED
#define MFS_HILITE          MF_HILITE
#define MFS_ENABLED         MF_ENABLED
#define MFS_UNCHECKED       MF_UNCHECKED
#define MFS_UNHILITE        MF_UNHILITE
#define MFS_DEFAULT         MF_DEFAULT
#endif /* WINVER >= 0x0400 */


#if(WINVER >= 0x0400)

WINUSERAPI
BOOL
WINAPI
CheckMenuRadioItem(
    __in HMENU hmenu,
    __in UINT first,
    __in UINT last,
    __in UINT check,
    __in UINT flags);
#endif /* WINVER >= 0x0400 */

/*
 * Menu item resource format
 */
typedef struct {
    WORD versionNumber;
    WORD offset;
} MENUITEMTEMPLATEHEADER, *PMENUITEMTEMPLATEHEADER;

typedef struct {        // version 0
    WORD mtOption;
    WORD mtID;
    WCHAR mtString[1];
} MENUITEMTEMPLATE, *PMENUITEMTEMPLATE;
#define MF_END             0x00000080L

#endif /* !NOMENUS */

#ifndef NOSYSCOMMANDS

/*
 * System Menu Command Values
 */
#define SC_SIZE         0xF000
#define SC_MOVE         0xF010
#define SC_MINIMIZE     0xF020
#define SC_MAXIMIZE     0xF030
#define SC_NEXTWINDOW   0xF040
#define SC_PREVWINDOW   0xF050
#define SC_CLOSE        0xF060
#define SC_VSCROLL      0xF070
#define SC_HSCROLL      0xF080
#define SC_MOUSEMENU    0xF090
#define SC_KEYMENU      0xF100
#define SC_ARRANGE      0xF110
#define SC_RESTORE      0xF120
#define SC_TASKLIST     0xF130
#define SC_SCREENSAVE   0xF140
#define SC_HOTKEY       0xF150
#if(WINVER >= 0x0400)
#define SC_DEFAULT      0xF160
#define SC_MONITORPOWER 0xF170
#define SC_CONTEXTHELP  0xF180
#define SC_SEPARATOR    0xF00F
#endif /* WINVER >= 0x0400 */

/*
 * Obsolete names
 */
#define SC_ICON         SC_MINIMIZE
#define SC_ZOOM         SC_MAXIMIZE

#endif /* !NOSYSCOMMANDS */

/*
 * Resource Loading Routines
 */

WINUSERAPI
HBITMAP
WINAPI
LoadBitmapA(
    __in_opt HINSTANCE hInstance,
    __in LPCSTR lpBitmapName);
WINUSERAPI
HBITMAP
WINAPI
LoadBitmapW(
    __in_opt HINSTANCE hInstance,
    __in LPCWSTR lpBitmapName);
#ifdef UNICODE
#define LoadBitmap  LoadBitmapW
#else
#define LoadBitmap  LoadBitmapA
#endif // !UNICODE

WINUSERAPI
HCURSOR
WINAPI
LoadCursorA(
    __in_opt HINSTANCE hInstance,
    __in LPCSTR lpCursorName);
WINUSERAPI
HCURSOR
WINAPI
LoadCursorW(
    __in_opt HINSTANCE hInstance,
    __in LPCWSTR lpCursorName);
#ifdef UNICODE
#define LoadCursor  LoadCursorW
#else
#define LoadCursor  LoadCursorA
#endif // !UNICODE

WINUSERAPI
HCURSOR
WINAPI
LoadCursorFromFileA(
    __in LPCSTR lpFileName);
WINUSERAPI
HCURSOR
WINAPI
LoadCursorFromFileW(
    __in LPCWSTR lpFileName);
#ifdef UNICODE
#define LoadCursorFromFile  LoadCursorFromFileW
#else
#define LoadCursorFromFile  LoadCursorFromFileA
#endif // !UNICODE

WINUSERAPI
HCURSOR
WINAPI
CreateCursor(
    __in_opt HINSTANCE hInst,
    __in int xHotSpot,
    __in int yHotSpot,
    __in int nWidth,
    __in int nHeight,
    __in CONST VOID *pvANDPlane,
    __in CONST VOID *pvXORPlane);

WINUSERAPI
BOOL
WINAPI
DestroyCursor(
    __in HCURSOR hCursor);

#ifndef _MAC
#define CopyCursor(pcur) ((HCURSOR)CopyIcon((HICON)(pcur)))
#else
WINUSERAPI
HCURSOR
WINAPI
CopyCursor(
    __in HCURSOR hCursor);
#endif

/*
 * Standard Cursor IDs
 */
#define IDC_ARROW           MAKEINTRESOURCE(32512)
#define IDC_IBEAM           MAKEINTRESOURCE(32513)
#define IDC_WAIT            MAKEINTRESOURCE(32514)
#define IDC_CROSS           MAKEINTRESOURCE(32515)
#define IDC_UPARROW         MAKEINTRESOURCE(32516)
#define IDC_SIZE            MAKEINTRESOURCE(32640)  /* OBSOLETE: use IDC_SIZEALL */
#define IDC_ICON            MAKEINTRESOURCE(32641)  /* OBSOLETE: use IDC_ARROW */
#define IDC_SIZENWSE        MAKEINTRESOURCE(32642)
#define IDC_SIZENESW        MAKEINTRESOURCE(32643)
#define IDC_SIZEWE          MAKEINTRESOURCE(32644)
#define IDC_SIZENS          MAKEINTRESOURCE(32645)
#define IDC_SIZEALL         MAKEINTRESOURCE(32646)
#define IDC_NO              MAKEINTRESOURCE(32648) /*not in win3.1 */
#if(WINVER >= 0x0500)
#define IDC_HAND            MAKEINTRESOURCE(32649)
#endif /* WINVER >= 0x0500 */
#define IDC_APPSTARTING     MAKEINTRESOURCE(32650) /*not in win3.1 */
#if(WINVER >= 0x0400)
#define IDC_HELP            MAKEINTRESOURCE(32651)
#endif /* WINVER >= 0x0400 */

WINUSERAPI
BOOL
WINAPI
SetSystemCursor(
    __in HCURSOR hcur,
    __in DWORD id);

typedef struct _ICONINFO {
    BOOL    fIcon;
    DWORD   xHotspot;
    DWORD   yHotspot;
    HBITMAP hbmMask;
    HBITMAP hbmColor;
} ICONINFO;
typedef ICONINFO *PICONINFO;

WINUSERAPI
HICON
WINAPI
LoadIconA(
    __in_opt HINSTANCE hInstance,
    __in LPCSTR lpIconName);
WINUSERAPI
HICON
WINAPI
LoadIconW(
    __in_opt HINSTANCE hInstance,
    __in LPCWSTR lpIconName);
#ifdef UNICODE
#define LoadIcon  LoadIconW
#else
#define LoadIcon  LoadIconA
#endif // !UNICODE


WINUSERAPI
UINT
WINAPI
PrivateExtractIconsA(
    __in LPCSTR szFileName,
    __in int nIconIndex,
    __in int cxIcon,
    __in int cyIcon,
    __out_ecount_part_opt(nIcons, return) HICON *phicon,
    __out_ecount_part_opt(nIcons, return) UINT *piconid,
    __in UINT nIcons,
    __in UINT flags);
WINUSERAPI
UINT
WINAPI
PrivateExtractIconsW(
    __in LPCWSTR szFileName,
    __in int nIconIndex,
    __in int cxIcon,
    __in int cyIcon,
    __out_ecount_part_opt(nIcons, return) HICON *phicon,
    __out_ecount_part_opt(nIcons, return) UINT *piconid,
    __in UINT nIcons,
    __in UINT flags);
#ifdef UNICODE
#define PrivateExtractIcons  PrivateExtractIconsW
#else
#define PrivateExtractIcons  PrivateExtractIconsA
#endif // !UNICODE

WINUSERAPI
HICON
WINAPI
CreateIcon(
    __in_opt HINSTANCE hInstance,
    __in int nWidth,
    __in int nHeight,
    __in BYTE cPlanes,
    __in BYTE cBitsPixel,
    __in CONST BYTE *lpbANDbits,
    __in CONST BYTE *lpbXORbits);

WINUSERAPI
BOOL
WINAPI
DestroyIcon(
    __in HICON hIcon);

WINUSERAPI
int
WINAPI
LookupIconIdFromDirectory(
    __in PBYTE presbits,
    __in BOOL fIcon);

#if(WINVER >= 0x0400)
WINUSERAPI
int
WINAPI
LookupIconIdFromDirectoryEx(
    __in PBYTE presbits,
    __in BOOL fIcon,
    __in int cxDesired,
    __in int cyDesired,
    __in UINT Flags);
#endif /* WINVER >= 0x0400 */

WINUSERAPI
HICON
WINAPI
CreateIconFromResource(
    __in PBYTE presbits,
    __in DWORD dwResSize,
    __in BOOL fIcon,
    __in DWORD dwVer);

#if(WINVER >= 0x0400)
WINUSERAPI
HICON
WINAPI
CreateIconFromResourceEx(
    __in PBYTE presbits,
    __in DWORD dwResSize,
    __in BOOL fIcon,
    __in DWORD dwVer,
    __in int cxDesired,
    __in int cyDesired,
    __in UINT Flags);

/* Icon/Cursor header */
typedef struct tagCURSORSHAPE
{
    int     xHotSpot;
    int     yHotSpot;
    int     cx;
    int     cy;
    int     cbWidth;
    BYTE    Planes;
    BYTE    BitsPixel;
} CURSORSHAPE, FAR *LPCURSORSHAPE;
#endif /* WINVER >= 0x0400 */

#define IMAGE_BITMAP        0
#define IMAGE_ICON          1
#define IMAGE_CURSOR        2
#if(WINVER >= 0x0400)
#define IMAGE_ENHMETAFILE   3

#define LR_DEFAULTCOLOR     0x0000
#define LR_MONOCHROME       0x0001
#define LR_COLOR            0x0002
#define LR_COPYRETURNORG    0x0004
#define LR_COPYDELETEORG    0x0008
#define LR_LOADFROMFILE     0x0010
#define LR_LOADTRANSPARENT  0x0020
#define LR_DEFAULTSIZE      0x0040
#define LR_VGACOLOR         0x0080
#define LR_LOADMAP3DCOLORS  0x1000
#define LR_CREATEDIBSECTION 0x2000
#define LR_COPYFROMRESOURCE 0x4000
#define LR_SHARED           0x8000

WINUSERAPI
HANDLE
WINAPI
LoadImageA(
    __in_opt HINSTANCE hInst,
    __in LPCSTR name,
    __in UINT type,
    __in int cx,
    __in int cy,
    __in UINT fuLoad);
WINUSERAPI
HANDLE
WINAPI
LoadImageW(
    __in_opt HINSTANCE hInst,
    __in LPCWSTR name,
    __in UINT type,
    __in int cx,
    __in int cy,
    __in UINT fuLoad);
#ifdef UNICODE
#define LoadImage  LoadImageW
#else
#define LoadImage  LoadImageA
#endif // !UNICODE

WINUSERAPI
HANDLE
WINAPI
CopyImage(
    __in HANDLE h,
    __in UINT type,
    __in int cx,
    __in int cy,
    __in UINT flags);

#define DI_MASK         0x0001
#define DI_IMAGE        0x0002
#define DI_NORMAL       0x0003
#define DI_COMPAT       0x0004
#define DI_DEFAULTSIZE  0x0008
#if(_WIN32_WINNT >= 0x0501)
#define DI_NOMIRROR     0x0010
#endif /* _WIN32_WINNT >= 0x0501 */

WINUSERAPI BOOL WINAPI DrawIconEx(
    __in HDC hdc,
    __in int xLeft,
    __in int yTop,
    __in HICON hIcon,
    __in int cxWidth,
    __in int cyWidth,
    __in UINT istepIfAniCur,
    __in_opt HBRUSH hbrFlickerFreeDraw,
    __in UINT diFlags);

#endif /* WINVER >= 0x0400 */

WINUSERAPI
HICON
WINAPI
CreateIconIndirect(
    __in PICONINFO piconinfo);

WINUSERAPI
HICON
WINAPI
CopyIcon(
    __in HICON hIcon);

WINUSERAPI
BOOL
WINAPI
GetIconInfo(
    __in HICON hIcon,
    __out PICONINFO piconinfo);

#if(WINVER >= 0x0400)
#define RES_ICON    1
#define RES_CURSOR  2
#endif /* WINVER >= 0x0400 */

#ifdef OEMRESOURCE


/*
 * OEM Resource Ordinal Numbers
 */
#define OBM_CLOSE           32754
#define OBM_UPARROW         32753
#define OBM_DNARROW         32752
#define OBM_RGARROW         32751
#define OBM_LFARROW         32750
#define OBM_REDUCE          32749
#define OBM_ZOOM            32748
#define OBM_RESTORE         32747
#define OBM_REDUCED         32746
#define OBM_ZOOMD           32745
#define OBM_RESTORED        32744
#define OBM_UPARROWD        32743
#define OBM_DNARROWD        32742
#define OBM_RGARROWD        32741
#define OBM_LFARROWD        32740
#define OBM_MNARROW         32739
#define OBM_COMBO           32738
#define OBM_UPARROWI        32737
#define OBM_DNARROWI        32736
#define OBM_RGARROWI        32735
#define OBM_LFARROWI        32734

#define OBM_OLD_CLOSE       32767
#define OBM_SIZE            32766
#define OBM_OLD_UPARROW     32765
#define OBM_OLD_DNARROW     32764
#define OBM_OLD_RGARROW     32763
#define OBM_OLD_LFARROW     32762
#define OBM_BTSIZE          32761
#define OBM_CHECK           32760
#define OBM_CHECKBOXES      32759
#define OBM_BTNCORNERS      32758
#define OBM_OLD_REDUCE      32757
#define OBM_OLD_ZOOM        32756
#define OBM_OLD_RESTORE     32755


#define OCR_NORMAL          32512
#define OCR_IBEAM           32513
#define OCR_WAIT            32514
#define OCR_CROSS           32515
#define OCR_UP              32516
#define OCR_SIZE            32640   /* OBSOLETE: use OCR_SIZEALL */
#define OCR_ICON            32641   /* OBSOLETE: use OCR_NORMAL */
#define OCR_SIZENWSE        32642
#define OCR_SIZENESW        32643
#define OCR_SIZEWE          32644
#define OCR_SIZENS          32645
#define OCR_SIZEALL         32646
#define OCR_ICOCUR          32647   /* OBSOLETE: use OIC_WINLOGO */
#define OCR_NO              32648
#if(WINVER >= 0x0500)
#define OCR_HAND            32649
#endif /* WINVER >= 0x0500 */
#if(WINVER >= 0x0400)
#define OCR_APPSTARTING     32650
#endif /* WINVER >= 0x0400 */


#define OIC_SAMPLE          32512
#define OIC_HAND            32513
#define OIC_QUES            32514
#define OIC_BANG            32515
#define OIC_NOTE            32516
#if(WINVER >= 0x0400)
#define OIC_WINLOGO         32517
#define OIC_WARNING         OIC_BANG
#define OIC_ERROR           OIC_HAND
#define OIC_INFORMATION     OIC_NOTE
#endif /* WINVER >= 0x0400 */



#endif /* OEMRESOURCE */

#define ORD_LANGDRIVER    1     /* The ordinal number for the entry point of
                                ** language drivers.
                                */

#ifndef NOICONS

/*
 * Standard Icon IDs
 */
#ifdef RC_INVOKED
#define IDI_APPLICATION     32512
#define IDI_HAND            32513
#define IDI_QUESTION        32514
#define IDI_EXCLAMATION     32515
#define IDI_ASTERISK        32516
#if(WINVER >= 0x0400)
#define IDI_WINLOGO         32517
#endif /* WINVER >= 0x0400 */
#else
#define IDI_APPLICATION     MAKEINTRESOURCE(32512)
#define IDI_HAND            MAKEINTRESOURCE(32513)
#define IDI_QUESTION        MAKEINTRESOURCE(32514)
#define IDI_EXCLAMATION     MAKEINTRESOURCE(32515)
#define IDI_ASTERISK        MAKEINTRESOURCE(32516)
#if(WINVER >= 0x0400)
#define IDI_WINLOGO         MAKEINTRESOURCE(32517)
#endif /* WINVER >= 0x0400 */
#endif /* RC_INVOKED */

#if(WINVER >= 0x0400)
#define IDI_WARNING     IDI_EXCLAMATION
#define IDI_ERROR       IDI_HAND
#define IDI_INFORMATION IDI_ASTERISK
#endif /* WINVER >= 0x0400 */


#endif /* !NOICONS */

WINUSERAPI
int
WINAPI
LoadStringA(
    __in_opt HINSTANCE hInstance,
    __in UINT uID,
    __out_ecount(cchBufferMax) LPSTR lpBuffer,
    __in int cchBufferMax);
WINUSERAPI
int
WINAPI
LoadStringW(
    __in_opt HINSTANCE hInstance,
    __in UINT uID,
    __out_ecount(cchBufferMax) LPWSTR lpBuffer,
    __in int cchBufferMax);
#ifdef UNICODE
#define LoadString  LoadStringW
#else
#define LoadString  LoadStringA
#endif // !UNICODE


/*
 * Dialog Box Command IDs
 */
#define IDOK                1
#define IDCANCEL            2
#define IDABORT             3
#define IDRETRY             4
#define IDIGNORE            5
#define IDYES               6
#define IDNO                7
#if(WINVER >= 0x0400)
#define IDCLOSE         8
#define IDHELP          9
#endif /* WINVER >= 0x0400 */

#if(WINVER >= 0x0500)
#define IDTRYAGAIN      10
#define IDCONTINUE      11
#endif /* WINVER >= 0x0500 */

#if(WINVER >= 0x0501)
#ifndef IDTIMEOUT
#define IDTIMEOUT 32000
#endif
#endif /* WINVER >= 0x0501 */


#ifndef NOCTLMGR

/*
 * Control Manager Structures and Definitions
 */

#ifndef NOWINSTYLES


/*
 * Edit Control Styles
 */
#define ES_LEFT             0x0000L
#define ES_CENTER           0x0001L
#define ES_RIGHT            0x0002L
#define ES_MULTILINE        0x0004L
#define ES_UPPERCASE        0x0008L
#define ES_LOWERCASE        0x0010L
#define ES_PASSWORD         0x0020L
#define ES_AUTOVSCROLL      0x0040L
#define ES_AUTOHSCROLL      0x0080L
#define ES_NOHIDESEL        0x0100L
#define ES_OEMCONVERT       0x0400L
#define ES_READONLY         0x0800L
#define ES_WANTRETURN       0x1000L
#if(WINVER >= 0x0400)
#define ES_NUMBER           0x2000L
#endif /* WINVER >= 0x0400 */


#endif /* !NOWINSTYLES */

/*
 * Edit Control Notification Codes
 */
#define EN_SETFOCUS         0x0100
#define EN_KILLFOCUS        0x0200
#define EN_CHANGE           0x0300
#define EN_UPDATE           0x0400
#define EN_ERRSPACE         0x0500
#define EN_MAXTEXT          0x0501
#define EN_HSCROLL          0x0601
#define EN_VSCROLL          0x0602

#if(_WIN32_WINNT >= 0x0500)
#define EN_ALIGN_LTR_EC     0x0700
#define EN_ALIGN_RTL_EC     0x0701
#endif /* _WIN32_WINNT >= 0x0500 */

#if(WINVER >= 0x0400)
/* Edit control EM_SETMARGIN parameters */
#define EC_LEFTMARGIN       0x0001
#define EC_RIGHTMARGIN      0x0002
#define EC_USEFONTINFO      0xffff
#endif /* WINVER >= 0x0400 */

#if(WINVER >= 0x0500)
/* wParam of EM_GET/SETIMESTATUS  */
#define EMSIS_COMPOSITIONSTRING        0x0001

/* lParam for EMSIS_COMPOSITIONSTRING  */
#define EIMES_GETCOMPSTRATONCE         0x0001
#define EIMES_CANCELCOMPSTRINFOCUS     0x0002
#define EIMES_COMPLETECOMPSTRKILLFOCUS 0x0004
#endif /* WINVER >= 0x0500 */

#ifndef NOWINMESSAGES


/*
 * Edit Control Messages
 */
#define EM_GETSEL               0x00B0
#define EM_SETSEL               0x00B1
#define EM_GETRECT              0x00B2
#define EM_SETRECT              0x00B3
#define EM_SETRECTNP            0x00B4
#define EM_SCROLL               0x00B5
#define EM_LINESCROLL           0x00B6
#define EM_SCROLLCARET          0x00B7
#define EM_GETMODIFY            0x00B8
#define EM_SETMODIFY            0x00B9
#define EM_GETLINECOUNT         0x00BA
#define EM_LINEINDEX            0x00BB
#define EM_SETHANDLE            0x00BC
#define EM_GETHANDLE            0x00BD
#define EM_GETTHUMB             0x00BE
#define EM_LINELENGTH           0x00C1
#define EM_REPLACESEL           0x00C2
#define EM_GETLINE              0x00C4
#define EM_LIMITTEXT            0x00C5
#define EM_CANUNDO              0x00C6
#define EM_UNDO                 0x00C7
#define EM_FMTLINES             0x00C8
#define EM_LINEFROMCHAR         0x00C9
#define EM_SETTABSTOPS          0x00CB
#define EM_SETPASSWORDCHAR      0x00CC
#define EM_EMPTYUNDOBUFFER      0x00CD
#define EM_GETFIRSTVISIBLELINE  0x00CE
#define EM_SETREADONLY          0x00CF
#define EM_SETWORDBREAKPROC     0x00D0
#define EM_GETWORDBREAKPROC     0x00D1
#define EM_GETPASSWORDCHAR      0x00D2
#if(WINVER >= 0x0400)
#define EM_SETMARGINS           0x00D3
#define EM_GETMARGINS           0x00D4
#define EM_SETLIMITTEXT         EM_LIMITTEXT   /* ;win40 Name change */
#define EM_GETLIMITTEXT         0x00D5
#define EM_POSFROMCHAR          0x00D6
#define EM_CHARFROMPOS          0x00D7
#endif /* WINVER >= 0x0400 */

#if(WINVER >= 0x0500)
#define EM_SETIMESTATUS         0x00D8
#define EM_GETIMESTATUS         0x00D9
#endif /* WINVER >= 0x0500 */


#endif /* !NOWINMESSAGES */

/*
 * EDITWORDBREAKPROC code values
 */
#define WB_LEFT            0
#define WB_RIGHT           1
#define WB_ISDELIMITER     2


/*
 * Button Control Styles
 */
#define BS_PUSHBUTTON       0x00000000L
#define BS_DEFPUSHBUTTON    0x00000001L
#define BS_CHECKBOX         0x00000002L
#define BS_AUTOCHECKBOX     0x00000003L
#define BS_RADIOBUTTON      0x00000004L
#define BS_3STATE           0x00000005L
#define BS_AUTO3STATE       0x00000006L
#define BS_GROUPBOX         0x00000007L
#define BS_USERBUTTON       0x00000008L
#define BS_AUTORADIOBUTTON  0x00000009L
#define BS_PUSHBOX          0x0000000AL
#define BS_OWNERDRAW        0x0000000BL
#define BS_TYPEMASK         0x0000000FL
#define BS_LEFTTEXT         0x00000020L
#if(WINVER >= 0x0400)
#define BS_TEXT             0x00000000L
#define BS_ICON             0x00000040L
#define BS_BITMAP           0x00000080L
#define BS_LEFT             0x00000100L
#define BS_RIGHT            0x00000200L
#define BS_CENTER           0x00000300L
#define BS_TOP              0x00000400L
#define BS_BOTTOM           0x00000800L
#define BS_VCENTER          0x00000C00L
#define BS_PUSHLIKE         0x00001000L
#define BS_MULTILINE        0x00002000L
#define BS_NOTIFY           0x00004000L
#define BS_FLAT             0x00008000L
#define BS_RIGHTBUTTON      BS_LEFTTEXT
#endif /* WINVER >= 0x0400 */

/*
 * User Button Notification Codes
 */
#define BN_CLICKED          0
#define BN_PAINT            1
#define BN_HILITE           2
#define BN_UNHILITE         3
#define BN_DISABLE          4
#define BN_DOUBLECLICKED    5
#if(WINVER >= 0x0400)
#define BN_PUSHED           BN_HILITE
#define BN_UNPUSHED         BN_UNHILITE
#define BN_DBLCLK           BN_DOUBLECLICKED
#define BN_SETFOCUS         6
#define BN_KILLFOCUS        7
#endif /* WINVER >= 0x0400 */

/*
 * Button Control Messages
 */
#define BM_GETCHECK        0x00F0
#define BM_SETCHECK        0x00F1
#define BM_GETSTATE        0x00F2
#define BM_SETSTATE        0x00F3
#define BM_SETSTYLE        0x00F4
#if(WINVER >= 0x0400)
#define BM_CLICK           0x00F5
#define BM_GETIMAGE        0x00F6
#define BM_SETIMAGE        0x00F7

#define BST_UNCHECKED      0x0000
#define BST_CHECKED        0x0001
#define BST_INDETERMINATE  0x0002
#define BST_PUSHED         0x0004
#define BST_FOCUS          0x0008
#endif /* WINVER >= 0x0400 */

/*
 * Static Control Constants
 */
#define SS_LEFT             0x00000000L
#define SS_CENTER           0x00000001L
#define SS_RIGHT            0x00000002L
#define SS_ICON             0x00000003L
#define SS_BLACKRECT        0x00000004L
#define SS_GRAYRECT         0x00000005L
#define SS_WHITERECT        0x00000006L
#define SS_BLACKFRAME       0x00000007L
#define SS_GRAYFRAME        0x00000008L
#define SS_WHITEFRAME       0x00000009L
#define SS_USERITEM         0x0000000AL
#define SS_SIMPLE           0x0000000BL
#define SS_LEFTNOWORDWRAP   0x0000000CL
#if(WINVER >= 0x0400)
#define SS_OWNERDRAW        0x0000000DL
#define SS_BITMAP           0x0000000EL
#define SS_ENHMETAFILE      0x0000000FL
#define SS_ETCHEDHORZ       0x00000010L
#define SS_ETCHEDVERT       0x00000011L
#define SS_ETCHEDFRAME      0x00000012L
#define SS_TYPEMASK         0x0000001FL
#endif /* WINVER >= 0x0400 */
#if(WINVER >= 0x0501)
#define SS_REALSIZECONTROL  0x00000040L
#endif /* WINVER >= 0x0501 */
#define SS_NOPREFIX         0x00000080L /* Don't do "&" character translation */
#if(WINVER >= 0x0400)
#define SS_NOTIFY           0x00000100L
#define SS_CENTERIMAGE      0x00000200L
#define SS_RIGHTJUST        0x00000400L
#define SS_REALSIZEIMAGE    0x00000800L
#define SS_SUNKEN           0x00001000L
#define SS_EDITCONTROL      0x00002000L
#define SS_ENDELLIPSIS      0x00004000L
#define SS_PATHELLIPSIS     0x00008000L
#define SS_WORDELLIPSIS     0x0000C000L
#define SS_ELLIPSISMASK     0x0000C000L
#endif /* WINVER >= 0x0400 */



#ifndef NOWINMESSAGES
/*
 * Static Control Mesages
 */
#define STM_SETICON         0x0170
#define STM_GETICON         0x0171
#if(WINVER >= 0x0400)
#define STM_SETIMAGE        0x0172
#define STM_GETIMAGE        0x0173
#define STN_CLICKED         0
#define STN_DBLCLK          1
#define STN_ENABLE          2
#define STN_DISABLE         3
#endif /* WINVER >= 0x0400 */
#define STM_MSGMAX          0x0174
#endif /* !NOWINMESSAGES */

/*
 * Dialog window class
 */
#define WC_DIALOG       (MAKEINTATOM(0x8002))

/*
 * Get/SetWindowWord/Long offsets for use with WC_DIALOG windows
 */
#define DWL_MSGRESULT   0
#define DWL_DLGPROC     4
#define DWL_USER        8

#ifdef _WIN64

#undef DWL_MSGRESULT
#undef DWL_DLGPROC
#undef DWL_USER

#endif /* _WIN64 */

#define DWLP_MSGRESULT  0
#define DWLP_DLGPROC    DWLP_MSGRESULT + sizeof(LRESULT)
#define DWLP_USER       DWLP_DLGPROC + sizeof(DLGPROC)

/*
 * Dialog Manager Routines
 */

#ifndef NOMSG

WINUSERAPI
BOOL
WINAPI
IsDialogMessageA(
    __in HWND hDlg,
    __in LPMSG lpMsg);
WINUSERAPI
BOOL
WINAPI
IsDialogMessageW(
    __in HWND hDlg,
    __in LPMSG lpMsg);
#ifdef UNICODE
#define IsDialogMessage  IsDialogMessageW
#else
#define IsDialogMessage  IsDialogMessageA
#endif // !UNICODE

#endif /* !NOMSG */

WINUSERAPI
BOOL
WINAPI
MapDialogRect(
    __in HWND hDlg,
    __inout LPRECT lpRect);

WINUSERAPI
int
WINAPI
DlgDirListA(
    __in HWND hDlg,
    __inout LPSTR lpPathSpec,
    __in int nIDListBox,
    __in int nIDStaticPath,
    __in UINT uFileType);
WINUSERAPI
int
WINAPI
DlgDirListW(
    __in HWND hDlg,
    __inout LPWSTR lpPathSpec,
    __in int nIDListBox,
    __in int nIDStaticPath,
    __in UINT uFileType);
#ifdef UNICODE
#define DlgDirList  DlgDirListW
#else
#define DlgDirList  DlgDirListA
#endif // !UNICODE

/*
 * DlgDirList, DlgDirListComboBox flags values
 */
#define DDL_READWRITE       0x0000
#define DDL_READONLY        0x0001
#define DDL_HIDDEN          0x0002
#define DDL_SYSTEM          0x0004
#define DDL_DIRECTORY       0x0010
#define DDL_ARCHIVE         0x0020

#define DDL_POSTMSGS        0x2000
#define DDL_DRIVES          0x4000
#define DDL_EXCLUSIVE       0x8000

WINUSERAPI
BOOL
WINAPI
DlgDirSelectExA(
    __in HWND hwndDlg,
    __out_ecount(chCount) LPSTR lpString,
    __in int chCount,
    __in int idListBox);
WINUSERAPI
BOOL
WINAPI
DlgDirSelectExW(
    __in HWND hwndDlg,
    __out_ecount(chCount) LPWSTR lpString,
    __in int chCount,
    __in int idListBox);
#ifdef UNICODE
#define DlgDirSelectEx  DlgDirSelectExW
#else
#define DlgDirSelectEx  DlgDirSelectExA
#endif // !UNICODE

WINUSERAPI
int
WINAPI
DlgDirListComboBoxA(
    __in HWND hDlg,
    __inout LPSTR lpPathSpec,
    __in int nIDComboBox,
    __in int nIDStaticPath,
    __in UINT uFiletype);
WINUSERAPI
int
WINAPI
DlgDirListComboBoxW(
    __in HWND hDlg,
    __inout LPWSTR lpPathSpec,
    __in int nIDComboBox,
    __in int nIDStaticPath,
    __in UINT uFiletype);
#ifdef UNICODE
#define DlgDirListComboBox  DlgDirListComboBoxW
#else
#define DlgDirListComboBox  DlgDirListComboBoxA
#endif // !UNICODE

WINUSERAPI
BOOL
WINAPI
DlgDirSelectComboBoxExA(
    __in HWND hwndDlg,
    __out_ecount(cchOut) LPSTR lpString,
    __in int cchOut,
    __in int idComboBox);
WINUSERAPI
BOOL
WINAPI
DlgDirSelectComboBoxExW(
    __in HWND hwndDlg,
    __out_ecount(cchOut) LPWSTR lpString,
    __in int cchOut,
    __in int idComboBox);
#ifdef UNICODE
#define DlgDirSelectComboBoxEx  DlgDirSelectComboBoxExW
#else
#define DlgDirSelectComboBoxEx  DlgDirSelectComboBoxExA
#endif // !UNICODE



/*
 * Dialog Styles
 */
#define DS_ABSALIGN         0x01L
#define DS_SYSMODAL         0x02L
#define DS_LOCALEDIT        0x20L   /* Edit items get Local storage. */
#define DS_SETFONT          0x40L   /* User specified font for Dlg controls */
#define DS_MODALFRAME       0x80L   /* Can be combined with WS_CAPTION  */
#define DS_NOIDLEMSG        0x100L  /* WM_ENTERIDLE message will not be sent */
#define DS_SETFOREGROUND    0x200L  /* not in win3.1 */


#if(WINVER >= 0x0400)
#define DS_3DLOOK           0x0004L
#define DS_FIXEDSYS         0x0008L
#define DS_NOFAILCREATE     0x0010L
#define DS_CONTROL          0x0400L
#define DS_CENTER           0x0800L
#define DS_CENTERMOUSE      0x1000L
#define DS_CONTEXTHELP      0x2000L

#define DS_SHELLFONT        (DS_SETFONT | DS_FIXEDSYS)
#endif /* WINVER >= 0x0400 */

#if(_WIN32_WCE >= 0x0500)
#define DS_USEPIXELS        0x8000L
#endif


#define DM_GETDEFID         (WM_USER+0)
#define DM_SETDEFID         (WM_USER+1)

#if(WINVER >= 0x0400)
#define DM_REPOSITION       (WM_USER+2)
#endif /* WINVER >= 0x0400 */
/*
 * Returned in HIWORD() of DM_GETDEFID result if msg is supported
 */
#define DC_HASDEFID         0x534B

/*
 * Dialog Codes
 */
#define DLGC_WANTARROWS     0x0001      /* Control wants arrow keys         */
#define DLGC_WANTTAB        0x0002      /* Control wants tab keys           */
#define DLGC_WANTALLKEYS    0x0004      /* Control wants all keys           */
#define DLGC_WANTMESSAGE    0x0004      /* Pass message to control          */
#define DLGC_HASSETSEL      0x0008      /* Understands EM_SETSEL message    */
#define DLGC_DEFPUSHBUTTON  0x0010      /* Default pushbutton               */
#define DLGC_UNDEFPUSHBUTTON 0x0020     /* Non-default pushbutton           */
#define DLGC_RADIOBUTTON    0x0040      /* Radio button                     */
#define DLGC_WANTCHARS      0x0080      /* Want WM_CHAR messages            */
#define DLGC_STATIC         0x0100      /* Static item: don't include       */
#define DLGC_BUTTON         0x2000      /* Button item: can be checked      */

#define LB_CTLCODE          0L

/*
 * Listbox Return Values
 */
#define LB_OKAY             0
#define LB_ERR              (-1)
#define LB_ERRSPACE         (-2)

/*
**  The idStaticPath parameter to DlgDirList can have the following values
**  ORed if the list box should show other details of the files along with
**  the name of the files;
*/
                                  /* all other details also will be returned */


/*
 * Listbox Notification Codes
 */
#define LBN_ERRSPACE        (-2)
#define LBN_SELCHANGE       1
#define LBN_DBLCLK          2
#define LBN_SELCANCEL       3
#define LBN_SETFOCUS        4
#define LBN_KILLFOCUS       5



#ifndef NOWINMESSAGES

/*
 * Listbox messages
 */
#define LB_ADDSTRING            0x0180
#define LB_INSERTSTRING         0x0181
#define LB_DELETESTRING         0x0182
#define LB_SELITEMRANGEEX       0x0183
#define LB_RESETCONTENT         0x0184
#define LB_SETSEL               0x0185
#define LB_SETCURSEL            0x0186
#define LB_GETSEL               0x0187
#define LB_GETCURSEL            0x0188
#define LB_GETTEXT              0x0189
#define LB_GETTEXTLEN           0x018A
#define LB_GETCOUNT             0x018B
#define LB_SELECTSTRING         0x018C
#define LB_DIR                  0x018D
#define LB_GETTOPINDEX          0x018E
#define LB_FINDSTRING           0x018F
#define LB_GETSELCOUNT          0x0190
#define LB_GETSELITEMS          0x0191
#define LB_SETTABSTOPS          0x0192
#define LB_GETHORIZONTALEXTENT  0x0193
#define LB_SETHORIZONTALEXTENT  0x0194
#define LB_SETCOLUMNWIDTH       0x0195
#define LB_ADDFILE              0x0196
#define LB_SETTOPINDEX          0x0197
#define LB_GETITEMRECT          0x0198
#define LB_GETITEMDATA          0x0199
#define LB_SETITEMDATA          0x019A
#define LB_SELITEMRANGE         0x019B
#define LB_SETANCHORINDEX       0x019C
#define LB_GETANCHORINDEX       0x019D
#define LB_SETCARETINDEX        0x019E
#define LB_GETCARETINDEX        0x019F
#define LB_SETITEMHEIGHT        0x01A0
#define LB_GETITEMHEIGHT        0x01A1
#define LB_FINDSTRINGEXACT      0x01A2
#define LB_SETLOCALE            0x01A5
#define LB_GETLOCALE            0x01A6
#define LB_SETCOUNT             0x01A7
#if(WINVER >= 0x0400)
#define LB_INITSTORAGE          0x01A8
#define LB_ITEMFROMPOINT        0x01A9
#endif /* WINVER >= 0x0400 */
#if(_WIN32_WCE >= 0x0400)
#define LB_MULTIPLEADDSTRING    0x01B1
#endif


#if(_WIN32_WINNT >= 0x0501)
#define LB_GETLISTBOXINFO       0x01B2
#endif /* _WIN32_WINNT >= 0x0501 */

#if(_WIN32_WINNT >= 0x0501)
#define LB_MSGMAX               0x01B3
#elif(_WIN32_WCE >= 0x0400)
#define LB_MSGMAX               0x01B1
#elif(WINVER >= 0x0400)
#define LB_MSGMAX               0x01B0
#else
#define LB_MSGMAX               0x01A8
#endif

#endif /* !NOWINMESSAGES */

#ifndef NOWINSTYLES


/*
 * Listbox Styles
 */
#define LBS_NOTIFY            0x0001L
#define LBS_SORT              0x0002L
#define LBS_NOREDRAW          0x0004L
#define LBS_MULTIPLESEL       0x0008L
#define LBS_OWNERDRAWFIXED    0x0010L
#define LBS_OWNERDRAWVARIABLE 0x0020L
#define LBS_HASSTRINGS        0x0040L
#define LBS_USETABSTOPS       0x0080L
#define LBS_NOINTEGRALHEIGHT  0x0100L
#define LBS_MULTICOLUMN       0x0200L
#define LBS_WANTKEYBOARDINPUT 0x0400L
#define LBS_EXTENDEDSEL       0x0800L
#define LBS_DISABLENOSCROLL   0x1000L
#define LBS_NODATA            0x2000L
#if(WINVER >= 0x0400)
#define LBS_NOSEL             0x4000L
#endif /* WINVER >= 0x0400 */
#define LBS_COMBOBOX          0x8000L

#define LBS_STANDARD          (LBS_NOTIFY | LBS_SORT | WS_VSCROLL | WS_BORDER)


#endif /* !NOWINSTYLES */


/*
 * Combo Box return Values
 */
#define CB_OKAY             0
#define CB_ERR              (-1)
#define CB_ERRSPACE         (-2)


/*
 * Combo Box Notification Codes
 */
#define CBN_ERRSPACE        (-1)
#define CBN_SELCHANGE       1
#define CBN_DBLCLK          2
#define CBN_SETFOCUS        3
#define CBN_KILLFOCUS       4
#define CBN_EDITCHANGE      5
#define CBN_EDITUPDATE      6
#define CBN_DROPDOWN        7
#define CBN_CLOSEUP         8
#define CBN_SELENDOK        9
#define CBN_SELENDCANCEL    10

#ifndef NOWINSTYLES

/*
 * Combo Box styles
 */
#define CBS_SIMPLE            0x0001L
#define CBS_DROPDOWN          0x0002L
#define CBS_DROPDOWNLIST      0x0003L
#define CBS_OWNERDRAWFIXED    0x0010L
#define CBS_OWNERDRAWVARIABLE 0x0020L
#define CBS_AUTOHSCROLL       0x0040L
#define CBS_OEMCONVERT        0x0080L
#define CBS_SORT              0x0100L
#define CBS_HASSTRINGS        0x0200L
#define CBS_NOINTEGRALHEIGHT  0x0400L
#define CBS_DISABLENOSCROLL   0x0800L
#if(WINVER >= 0x0400)
#define CBS_UPPERCASE         0x2000L
#define CBS_LOWERCASE         0x4000L
#endif /* WINVER >= 0x0400 */

#endif  /* !NOWINSTYLES */


/*
 * Combo Box messages
 */
#ifndef NOWINMESSAGES
#define CB_GETEDITSEL               0x0140
#define CB_LIMITTEXT                0x0141
#define CB_SETEDITSEL               0x0142
#define CB_ADDSTRING                0x0143
#define CB_DELETESTRING             0x0144
#define CB_DIR                      0x0145
#define CB_GETCOUNT                 0x0146
#define CB_GETCURSEL                0x0147
#define CB_GETLBTEXT                0x0148
#define CB_GETLBTEXTLEN             0x0149
#define CB_INSERTSTRING             0x014A
#define CB_RESETCONTENT             0x014B
#define CB_FINDSTRING               0x014C
#define CB_SELECTSTRING             0x014D
#define CB_SETCURSEL                0x014E
#define CB_SHOWDROPDOWN             0x014F
#define CB_GETITEMDATA              0x0150
#define CB_SETITEMDATA              0x0151
#define CB_GETDROPPEDCONTROLRECT    0x0152
#define CB_SETITEMHEIGHT            0x0153
#define CB_GETITEMHEIGHT            0x0154
#define CB_SETEXTENDEDUI            0x0155
#define CB_GETEXTENDEDUI            0x0156
#define CB_GETDROPPEDSTATE          0x0157
#define CB_FINDSTRINGEXACT          0x0158
#define CB_SETLOCALE                0x0159
#define CB_GETLOCALE                0x015A
#if(WINVER >= 0x0400)
#define CB_GETTOPINDEX              0x015b
#define CB_SETTOPINDEX              0x015c
#define CB_GETHORIZONTALEXTENT      0x015d
#define CB_SETHORIZONTALEXTENT      0x015e
#define CB_GETDROPPEDWIDTH          0x015f
#define CB_SETDROPPEDWIDTH          0x0160
#define CB_INITSTORAGE              0x0161
#if(_WIN32_WCE >= 0x0400)
#define CB_MULTIPLEADDSTRING        0x0163
#endif
#endif /* WINVER >= 0x0400 */

#if(_WIN32_WINNT >= 0x0501)
#define CB_GETCOMBOBOXINFO          0x0164
#endif /* _WIN32_WINNT >= 0x0501 */

#if(_WIN32_WINNT >= 0x0501)
#define CB_MSGMAX                   0x0165
#elif(_WIN32_WCE >= 0x0400)
#define CB_MSGMAX                   0x0163
#elif(WINVER >= 0x0400)
#define CB_MSGMAX                   0x0162
#else
#define CB_MSGMAX                   0x015B
#endif
#endif  /* !NOWINMESSAGES */



#ifndef NOWINSTYLES


/*
 * Scroll Bar Styles
 */
#define SBS_HORZ                    0x0000L
#define SBS_VERT                    0x0001L
#define SBS_TOPALIGN                0x0002L
#define SBS_LEFTALIGN               0x0002L
#define SBS_BOTTOMALIGN             0x0004L
#define SBS_RIGHTALIGN              0x0004L
#define SBS_SIZEBOXTOPLEFTALIGN     0x0002L
#define SBS_SIZEBOXBOTTOMRIGHTALIGN 0x0004L
#define SBS_SIZEBOX                 0x0008L
#if(WINVER >= 0x0400)
#define SBS_SIZEGRIP                0x0010L
#endif /* WINVER >= 0x0400 */


#endif /* !NOWINSTYLES */

/*
 * Scroll bar messages
 */
#ifndef NOWINMESSAGES
#define SBM_SETPOS                  0x00E0 /*not in win3.1 */
#define SBM_GETPOS                  0x00E1 /*not in win3.1 */
#define SBM_SETRANGE                0x00E2 /*not in win3.1 */
#define SBM_SETRANGEREDRAW          0x00E6 /*not in win3.1 */
#define SBM_GETRANGE                0x00E3 /*not in win3.1 */
#define SBM_ENABLE_ARROWS           0x00E4 /*not in win3.1 */
#if(WINVER >= 0x0400)
#define SBM_SETSCROLLINFO           0x00E9
#define SBM_GETSCROLLINFO           0x00EA
#endif /* WINVER >= 0x0400 */

#if(_WIN32_WINNT >= 0x0501)
#define SBM_GETSCROLLBARINFO        0x00EB
#endif /* _WIN32_WINNT >= 0x0501 */

#if(WINVER >= 0x0400)
#define SIF_RANGE           0x0001
#define SIF_PAGE            0x0002
#define SIF_POS             0x0004
#define SIF_DISABLENOSCROLL 0x0008
#define SIF_TRACKPOS        0x0010
#define SIF_ALL             (SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS)

typedef struct tagSCROLLINFO
{
    UINT    cbSize;
    UINT    fMask;
    int     nMin;
    int     nMax;
    UINT    nPage;
    int     nPos;
    int     nTrackPos;
}   SCROLLINFO, FAR *LPSCROLLINFO;
typedef SCROLLINFO CONST FAR *LPCSCROLLINFO;

WINUSERAPI
int
WINAPI
SetScrollInfo(
    __in HWND hwnd,
    __in int nBar,
    __in LPCSCROLLINFO lpsi,
    __in BOOL redraw);

WINUSERAPI
BOOL
WINAPI
GetScrollInfo(
    __in HWND hwnd,
    __in int nBar,
    __inout LPSCROLLINFO lpsi);

#endif /* WINVER >= 0x0400 */
#endif /* !NOWINMESSAGES */
#endif /* !NOCTLMGR */

#ifndef NOMDI

/*
 * MDI client style bits
 */
#define MDIS_ALLCHILDSTYLES    0x0001

/*
 * wParam Flags for WM_MDITILE and WM_MDICASCADE messages.
 */
#define MDITILE_VERTICAL       0x0000 /*not in win3.1 */
#define MDITILE_HORIZONTAL     0x0001 /*not in win3.1 */
#define MDITILE_SKIPDISABLED   0x0002 /*not in win3.1 */
#if(_WIN32_WINNT >= 0x0500)
#define MDITILE_ZORDER         0x0004
#endif /* _WIN32_WINNT >= 0x0500 */

typedef struct tagMDICREATESTRUCTA {
    LPCSTR   szClass;
    LPCSTR   szTitle;
    HANDLE hOwner;
    int x;
    int y;
    int cx;
    int cy;
    DWORD style;
    LPARAM lParam;        /* app-defined stuff */
} MDICREATESTRUCTA, *LPMDICREATESTRUCTA;
typedef struct tagMDICREATESTRUCTW {
    LPCWSTR  szClass;
    LPCWSTR  szTitle;
    HANDLE hOwner;
    int x;
    int y;
    int cx;
    int cy;
    DWORD style;
    LPARAM lParam;        /* app-defined stuff */
} MDICREATESTRUCTW, *LPMDICREATESTRUCTW;
#ifdef UNICODE
typedef MDICREATESTRUCTW MDICREATESTRUCT;
typedef LPMDICREATESTRUCTW LPMDICREATESTRUCT;
#else
typedef MDICREATESTRUCTA MDICREATESTRUCT;
typedef LPMDICREATESTRUCTA LPMDICREATESTRUCT;
#endif // UNICODE

typedef struct tagCLIENTCREATESTRUCT {
    HANDLE hWindowMenu;
    UINT idFirstChild;
} CLIENTCREATESTRUCT, *LPCLIENTCREATESTRUCT;

WINUSERAPI
LRESULT
WINAPI
DefFrameProcA(
    __in HWND hWnd,
    __in_opt HWND hWndMDIClient,
    __in UINT uMsg,
    __in WPARAM wParam,
    __in LPARAM lParam);
WINUSERAPI
LRESULT
WINAPI
DefFrameProcW(
    __in HWND hWnd,
    __in_opt HWND hWndMDIClient,
    __in UINT uMsg,
    __in WPARAM wParam,
    __in LPARAM lParam);
#ifdef UNICODE
#define DefFrameProc  DefFrameProcW
#else
#define DefFrameProc  DefFrameProcA
#endif // !UNICODE

WINUSERAPI
#ifndef _MAC
LRESULT
WINAPI
#else
LRESULT
CALLBACK
#endif
DefMDIChildProcA(
    __in HWND hWnd,
    __in UINT uMsg,
    __in WPARAM wParam,
    __in LPARAM lParam);
WINUSERAPI
#ifndef _MAC
LRESULT
WINAPI
#else
LRESULT
CALLBACK
#endif
DefMDIChildProcW(
    __in HWND hWnd,
    __in UINT uMsg,
    __in WPARAM wParam,
    __in LPARAM lParam);
#ifdef UNICODE
#define DefMDIChildProc  DefMDIChildProcW
#else
#define DefMDIChildProc  DefMDIChildProcA
#endif // !UNICODE

#ifndef NOMSG

WINUSERAPI
BOOL
WINAPI
TranslateMDISysAccel(
    __in HWND hWndClient,
    __in LPMSG lpMsg);

#endif /* !NOMSG */

WINUSERAPI
UINT
WINAPI
ArrangeIconicWindows(
    __in HWND hWnd);

WINUSERAPI
HWND
WINAPI
CreateMDIWindowA(
    __in LPCSTR lpClassName,
    __in LPCSTR lpWindowName,
    __in DWORD dwStyle,
    __in int X,
    __in int Y,
    __in int nWidth,
    __in int nHeight,
    __in_opt HWND hWndParent,
    __in_opt HINSTANCE hInstance,
    __in LPARAM lParam);
WINUSERAPI
HWND
WINAPI
CreateMDIWindowW(
    __in LPCWSTR lpClassName,
    __in LPCWSTR lpWindowName,
    __in DWORD dwStyle,
    __in int X,
    __in int Y,
    __in int nWidth,
    __in int nHeight,
    __in_opt HWND hWndParent,
    __in_opt HINSTANCE hInstance,
    __in LPARAM lParam);
#ifdef UNICODE
#define CreateMDIWindow  CreateMDIWindowW
#else
#define CreateMDIWindow  CreateMDIWindowA
#endif // !UNICODE

#if(WINVER >= 0x0400)
WINUSERAPI 
WORD 
WINAPI 
TileWindows(
    __in_opt HWND hwndParent,
    __in UINT wHow,
    __in_opt CONST RECT * lpRect,
    __in UINT cKids,
    __in_ecount_opt(cKids) const HWND FAR * lpKids);

WINUSERAPI
WORD
WINAPI CascadeWindows(
    __in_opt HWND hwndParent,
    __in UINT wHow,
    __in_opt CONST RECT * lpRect,
    __in UINT cKids,
    __in_ecount_opt(cKids) const HWND FAR * lpKids);

#endif /* WINVER >= 0x0400 */
#endif /* !NOMDI */

#endif /* !NOUSER */

/****** Help support ********************************************************/

#ifndef NOHELP

typedef DWORD HELPPOLY;
typedef struct tagMULTIKEYHELPA {
#ifndef _MAC
    DWORD  mkSize;
#else
    WORD   mkSize;
#endif
    CHAR   mkKeylist;
    CHAR   szKeyphrase[1];
} MULTIKEYHELPA, *PMULTIKEYHELPA, *LPMULTIKEYHELPA;
typedef struct tagMULTIKEYHELPW {
#ifndef _MAC
    DWORD  mkSize;
#else
    WORD   mkSize;
#endif
    WCHAR  mkKeylist;
    WCHAR  szKeyphrase[1];
} MULTIKEYHELPW, *PMULTIKEYHELPW, *LPMULTIKEYHELPW;
#ifdef UNICODE
typedef MULTIKEYHELPW MULTIKEYHELP;
typedef PMULTIKEYHELPW PMULTIKEYHELP;
typedef LPMULTIKEYHELPW LPMULTIKEYHELP;
#else
typedef MULTIKEYHELPA MULTIKEYHELP;
typedef PMULTIKEYHELPA PMULTIKEYHELP;
typedef LPMULTIKEYHELPA LPMULTIKEYHELP;
#endif // UNICODE

typedef struct tagHELPWININFOA {
    int  wStructSize;
    int  x;
    int  y;
    int  dx;
    int  dy;
    int  wMax;
    CHAR   rgchMember[2];
} HELPWININFOA, *PHELPWININFOA, *LPHELPWININFOA;
typedef struct tagHELPWININFOW {
    int  wStructSize;
    int  x;
    int  y;
    int  dx;
    int  dy;
    int  wMax;
    WCHAR  rgchMember[2];
} HELPWININFOW, *PHELPWININFOW, *LPHELPWININFOW;
#ifdef UNICODE
typedef HELPWININFOW HELPWININFO;
typedef PHELPWININFOW PHELPWININFO;
typedef LPHELPWININFOW LPHELPWININFO;
#else
typedef HELPWININFOA HELPWININFO;
typedef PHELPWININFOA PHELPWININFO;
typedef LPHELPWININFOA LPHELPWININFO;
#endif // UNICODE


/*
 * Commands to pass to WinHelp()
 */
#define HELP_CONTEXT      0x0001L  /* Display topic in ulTopic */
#define HELP_QUIT         0x0002L  /* Terminate help */
#define HELP_INDEX        0x0003L  /* Display index */
#define HELP_CONTENTS     0x0003L
#define HELP_HELPONHELP   0x0004L  /* Display help on using help */
#define HELP_SETINDEX     0x0005L  /* Set current Index for multi index help */
#define HELP_SETCONTENTS  0x0005L
#define HELP_CONTEXTPOPUP 0x0008L
#define HELP_FORCEFILE    0x0009L
#define HELP_KEY          0x0101L  /* Display topic for keyword in offabData */
#define HELP_COMMAND      0x0102L
#define HELP_PARTIALKEY   0x0105L
#define HELP_MULTIKEY     0x0201L
#define HELP_SETWINPOS    0x0203L
#if(WINVER >= 0x0400)
#define HELP_CONTEXTMENU  0x000a
#define HELP_FINDER       0x000b
#define HELP_WM_HELP      0x000c
#define HELP_SETPOPUP_POS 0x000d

#define HELP_TCARD              0x8000
#define HELP_TCARD_DATA         0x0010
#define HELP_TCARD_OTHER_CALLER 0x0011

// These are in winhelp.h in Win95.
#define IDH_NO_HELP                     28440
#define IDH_MISSING_CONTEXT             28441 // Control doesn't have matching help context
#define IDH_GENERIC_HELP_BUTTON         28442 // Property sheet help button
#define IDH_OK                          28443
#define IDH_CANCEL                      28444
#define IDH_HELP                        28445

#endif /* WINVER >= 0x0400 */



WINUSERAPI
BOOL
WINAPI
WinHelpA(
    __in_opt HWND hWndMain,
    __in_opt LPCSTR lpszHelp,
    __in UINT uCommand,
    __in ULONG_PTR dwData);
WINUSERAPI
BOOL
WINAPI
WinHelpW(
    __in_opt HWND hWndMain,
    __in_opt LPCWSTR lpszHelp,
    __in UINT uCommand,
    __in ULONG_PTR dwData);
#ifdef UNICODE
#define WinHelp  WinHelpW
#else
#define WinHelp  WinHelpA
#endif // !UNICODE

#endif /* !NOHELP */

#if(WINVER >= 0x0500)

#define GR_GDIOBJECTS     0       /* Count of GDI objects */
#define GR_USEROBJECTS    1       /* Count of USER objects */

WINUSERAPI
DWORD
WINAPI
GetGuiResources(
    __in HANDLE hProcess,
    __in DWORD uiFlags);
#endif /* WINVER >= 0x0500 */


#ifndef NOSYSPARAMSINFO

/*
 * Parameter for SystemParametersInfo()
 */

#define SPI_GETBEEP                 0x0001
#define SPI_SETBEEP                 0x0002
#define SPI_GETMOUSE                0x0003
#define SPI_SETMOUSE                0x0004
#define SPI_GETBORDER               0x0005
#define SPI_SETBORDER               0x0006
#define SPI_GETKEYBOARDSPEED        0x000A
#define SPI_SETKEYBOARDSPEED        0x000B
#define SPI_LANGDRIVER              0x000C
#define SPI_ICONHORIZONTALSPACING   0x000D
#define SPI_GETSCREENSAVETIMEOUT    0x000E
#define SPI_SETSCREENSAVETIMEOUT    0x000F
#define SPI_GETSCREENSAVEACTIVE     0x0010
#define SPI_SETSCREENSAVEACTIVE     0x0011
#define SPI_GETGRIDGRANULARITY      0x0012
#define SPI_SETGRIDGRANULARITY      0x0013
#define SPI_SETDESKWALLPAPER        0x0014
#define SPI_SETDESKPATTERN          0x0015
#define SPI_GETKEYBOARDDELAY        0x0016
#define SPI_SETKEYBOARDDELAY        0x0017
#define SPI_ICONVERTICALSPACING     0x0018
#define SPI_GETICONTITLEWRAP        0x0019
#define SPI_SETICONTITLEWRAP        0x001A
#define SPI_GETMENUDROPALIGNMENT    0x001B
#define SPI_SETMENUDROPALIGNMENT    0x001C
#define SPI_SETDOUBLECLKWIDTH       0x001D
#define SPI_SETDOUBLECLKHEIGHT      0x001E
#define SPI_GETICONTITLELOGFONT     0x001F
#define SPI_SETDOUBLECLICKTIME      0x0020
#define SPI_SETMOUSEBUTTONSWAP      0x0021
#define SPI_SETICONTITLELOGFONT     0x0022
#define SPI_GETFASTTASKSWITCH       0x0023
#define SPI_SETFASTTASKSWITCH       0x0024
#if(WINVER >= 0x0400)
#define SPI_SETDRAGFULLWINDOWS      0x0025
#define SPI_GETDRAGFULLWINDOWS      0x0026
#define SPI_GETNONCLIENTMETRICS     0x0029
#define SPI_SETNONCLIENTMETRICS     0x002A
#define SPI_GETMINIMIZEDMETRICS     0x002B
#define SPI_SETMINIMIZEDMETRICS     0x002C
#define SPI_GETICONMETRICS          0x002D
#define SPI_SETICONMETRICS          0x002E
#define SPI_SETWORKAREA             0x002F
#define SPI_GETWORKAREA             0x0030
#define SPI_SETPENWINDOWS           0x0031

#define SPI_GETHIGHCONTRAST         0x0042
#define SPI_SETHIGHCONTRAST         0x0043
#define SPI_GETKEYBOARDPREF         0x0044
#define SPI_SETKEYBOARDPREF         0x0045
#define SPI_GETSCREENREADER         0x0046
#define SPI_SETSCREENREADER         0x0047
#define SPI_GETANIMATION            0x0048
#define SPI_SETANIMATION            0x0049
#define SPI_GETFONTSMOOTHING        0x004A
#define SPI_SETFONTSMOOTHING        0x004B
#define SPI_SETDRAGWIDTH            0x004C
#define SPI_SETDRAGHEIGHT           0x004D
#define SPI_SETHANDHELD             0x004E
#define SPI_GETLOWPOWERTIMEOUT      0x004F
#define SPI_GETPOWEROFFTIMEOUT      0x0050
#define SPI_SETLOWPOWERTIMEOUT      0x0051
#define SPI_SETPOWEROFFTIMEOUT      0x0052
#define SPI_GETLOWPOWERACTIVE       0x0053
#define SPI_GETPOWEROFFACTIVE       0x0054
#define SPI_SETLOWPOWERACTIVE       0x0055
#define SPI_SETPOWEROFFACTIVE       0x0056
#define SPI_SETCURSORS              0x0057
#define SPI_SETICONS                0x0058
#define SPI_GETDEFAULTINPUTLANG     0x0059
#define SPI_SETDEFAULTINPUTLANG     0x005A
#define SPI_SETLANGTOGGLE           0x005B
#define SPI_GETWINDOWSEXTENSION     0x005C
#define SPI_SETMOUSETRAILS          0x005D
#define SPI_GETMOUSETRAILS          0x005E
#define SPI_SETSCREENSAVERRUNNING   0x0061
#define SPI_SCREENSAVERRUNNING     SPI_SETSCREENSAVERRUNNING
#endif /* WINVER >= 0x0400 */
#define SPI_GETFILTERKEYS          0x0032
#define SPI_SETFILTERKEYS          0x0033
#define SPI_GETTOGGLEKEYS          0x0034
#define SPI_SETTOGGLEKEYS          0x0035
#define SPI_GETMOUSEKEYS           0x0036
#define SPI_SETMOUSEKEYS           0x0037
#define SPI_GETSHOWSOUNDS          0x0038
#define SPI_SETSHOWSOUNDS          0x0039
#define SPI_GETSTICKYKEYS          0x003A
#define SPI_SETSTICKYKEYS          0x003B
#define SPI_GETACCESSTIMEOUT       0x003C
#define SPI_SETACCESSTIMEOUT       0x003D
#if(WINVER >= 0x0400)
#define SPI_GETSERIALKEYS          0x003E
#define SPI_SETSERIALKEYS          0x003F
#endif /* WINVER >= 0x0400 */
#define SPI_GETSOUNDSENTRY         0x0040
#define SPI_SETSOUNDSENTRY         0x0041
#if(_WIN32_WINNT >= 0x0400)
#define SPI_GETSNAPTODEFBUTTON     0x005F
#define SPI_SETSNAPTODEFBUTTON     0x0060
#endif /* _WIN32_WINNT >= 0x0400 */
#if (_WIN32_WINNT >= 0x0400) || (_WIN32_WINDOWS > 0x0400)
#define SPI_GETMOUSEHOVERWIDTH     0x0062
#define SPI_SETMOUSEHOVERWIDTH     0x0063
#define SPI_GETMOUSEHOVERHEIGHT    0x0064
#define SPI_SETMOUSEHOVERHEIGHT    0x0065
#define SPI_GETMOUSEHOVERTIME      0x0066
#define SPI_SETMOUSEHOVERTIME      0x0067
#define SPI_GETWHEELSCROLLLINES    0x0068
#define SPI_SETWHEELSCROLLLINES    0x0069
#define SPI_GETMENUSHOWDELAY       0x006A
#define SPI_SETMENUSHOWDELAY       0x006B


#define SPI_GETSHOWIMEUI          0x006E
#define SPI_SETSHOWIMEUI          0x006F
#endif


#if(WINVER >= 0x0500)
#define SPI_GETMOUSESPEED         0x0070
#define SPI_SETMOUSESPEED         0x0071
#define SPI_GETSCREENSAVERRUNNING 0x0072
#define SPI_GETDESKWALLPAPER      0x0073
#endif /* WINVER >= 0x0500 */


#if(WINVER >= 0x0500)
#define SPI_GETACTIVEWINDOWTRACKING         0x1000
#define SPI_SETACTIVEWINDOWTRACKING         0x1001
#define SPI_GETMENUANIMATION                0x1002
#define SPI_SETMENUANIMATION                0x1003
#define SPI_GETCOMBOBOXANIMATION            0x1004
#define SPI_SETCOMBOBOXANIMATION            0x1005
#define SPI_GETLISTBOXSMOOTHSCROLLING       0x1006
#define SPI_SETLISTBOXSMOOTHSCROLLING       0x1007
#define SPI_GETGRADIENTCAPTIONS             0x1008
#define SPI_SETGRADIENTCAPTIONS             0x1009
#define SPI_GETKEYBOARDCUES                 0x100A
#define SPI_SETKEYBOARDCUES                 0x100B
#define SPI_GETMENUUNDERLINES               SPI_GETKEYBOARDCUES
#define SPI_SETMENUUNDERLINES               SPI_SETKEYBOARDCUES
#define SPI_GETACTIVEWNDTRKZORDER           0x100C
#define SPI_SETACTIVEWNDTRKZORDER           0x100D
#define SPI_GETHOTTRACKING                  0x100E
#define SPI_SETHOTTRACKING                  0x100F
#define SPI_GETMENUFADE                     0x1012
#define SPI_SETMENUFADE                     0x1013
#define SPI_GETSELECTIONFADE                0x1014
#define SPI_SETSELECTIONFADE                0x1015
#define SPI_GETTOOLTIPANIMATION             0x1016
#define SPI_SETTOOLTIPANIMATION             0x1017
#define SPI_GETTOOLTIPFADE                  0x1018
#define SPI_SETTOOLTIPFADE                  0x1019
#define SPI_GETCURSORSHADOW                 0x101A
#define SPI_SETCURSORSHADOW                 0x101B
#if(_WIN32_WINNT >= 0x0501)
#define SPI_GETMOUSESONAR                   0x101C
#define SPI_SETMOUSESONAR                   0x101D
#define SPI_GETMOUSECLICKLOCK               0x101E
#define SPI_SETMOUSECLICKLOCK               0x101F
#define SPI_GETMOUSEVANISH                  0x1020
#define SPI_SETMOUSEVANISH                  0x1021
#define SPI_GETFLATMENU                     0x1022
#define SPI_SETFLATMENU                     0x1023
#define SPI_GETDROPSHADOW                   0x1024
#define SPI_SETDROPSHADOW                   0x1025
#define SPI_GETBLOCKSENDINPUTRESETS         0x1026
#define SPI_SETBLOCKSENDINPUTRESETS         0x1027
#endif /* _WIN32_WINNT >= 0x0501 */
#define SPI_GETUIEFFECTS                    0x103E
#define SPI_SETUIEFFECTS                    0x103F

#define SPI_GETFOREGROUNDLOCKTIMEOUT        0x2000
#define SPI_SETFOREGROUNDLOCKTIMEOUT        0x2001
#define SPI_GETACTIVEWNDTRKTIMEOUT          0x2002
#define SPI_SETACTIVEWNDTRKTIMEOUT          0x2003
#define SPI_GETFOREGROUNDFLASHCOUNT         0x2004
#define SPI_SETFOREGROUNDFLASHCOUNT         0x2005
#define SPI_GETCARETWIDTH                   0x2006
#define SPI_SETCARETWIDTH                   0x2007

#if(_WIN32_WINNT >= 0x0501)
#define SPI_GETMOUSECLICKLOCKTIME           0x2008
#define SPI_SETMOUSECLICKLOCKTIME           0x2009
#define SPI_GETFONTSMOOTHINGTYPE            0x200A
#define SPI_SETFONTSMOOTHINGTYPE            0x200B

/* constants for SPI_GETFONTSMOOTHINGTYPE and SPI_SETFONTSMOOTHINGTYPE: */
#define FE_FONTSMOOTHINGSTANDARD            0x0001
#define FE_FONTSMOOTHINGCLEARTYPE           0x0002
#define FE_FONTSMOOTHINGDOCKING             0x8000

#define SPI_GETFONTSMOOTHINGCONTRAST           0x200C
#define SPI_SETFONTSMOOTHINGCONTRAST           0x200D

#define SPI_GETFOCUSBORDERWIDTH             0x200E
#define SPI_SETFOCUSBORDERWIDTH             0x200F
#define SPI_GETFOCUSBORDERHEIGHT            0x2010
#define SPI_SETFOCUSBORDERHEIGHT            0x2011

#define SPI_GETFONTSMOOTHINGORIENTATION           0x2012
#define SPI_SETFONTSMOOTHINGORIENTATION           0x2013

/* constants for SPI_GETFONTSMOOTHINGORIENTATION and SPI_SETFONTSMOOTHINGORIENTATION: */
#define FE_FONTSMOOTHINGORIENTATIONBGR   0x0000
#define FE_FONTSMOOTHINGORIENTATIONRGB   0x0001
#endif /* _WIN32_WINNT >= 0x0501 */

#endif /* WINVER >= 0x0500 */

/*
 * Flags
 */
#define SPIF_UPDATEINIFILE    0x0001
#define SPIF_SENDWININICHANGE 0x0002
#define SPIF_SENDCHANGE       SPIF_SENDWININICHANGE


#define METRICS_USEDEFAULT -1
#ifdef _WINGDI_
#ifndef NOGDI
typedef struct tagNONCLIENTMETRICSA
{
    UINT    cbSize;
    int     iBorderWidth;
    int     iScrollWidth;
    int     iScrollHeight;
    int     iCaptionWidth;
    int     iCaptionHeight;
    LOGFONTA lfCaptionFont;
    int     iSmCaptionWidth;
    int     iSmCaptionHeight;
    LOGFONTA lfSmCaptionFont;
    int     iMenuWidth;
    int     iMenuHeight;
    LOGFONTA lfMenuFont;
    LOGFONTA lfStatusFont;
    LOGFONTA lfMessageFont;
}   NONCLIENTMETRICSA, *PNONCLIENTMETRICSA, FAR* LPNONCLIENTMETRICSA;
typedef struct tagNONCLIENTMETRICSW
{
    UINT    cbSize;
    int     iBorderWidth;
    int     iScrollWidth;
    int     iScrollHeight;
    int     iCaptionWidth;
    int     iCaptionHeight;
    LOGFONTW lfCaptionFont;
    int     iSmCaptionWidth;
    int     iSmCaptionHeight;
    LOGFONTW lfSmCaptionFont;
    int     iMenuWidth;
    int     iMenuHeight;
    LOGFONTW lfMenuFont;
    LOGFONTW lfStatusFont;
    LOGFONTW lfMessageFont;
}   NONCLIENTMETRICSW, *PNONCLIENTMETRICSW, FAR* LPNONCLIENTMETRICSW;
#ifdef UNICODE
typedef NONCLIENTMETRICSW NONCLIENTMETRICS;
typedef PNONCLIENTMETRICSW PNONCLIENTMETRICS;
typedef LPNONCLIENTMETRICSW LPNONCLIENTMETRICS;
#else
typedef NONCLIENTMETRICSA NONCLIENTMETRICS;
typedef PNONCLIENTMETRICSA PNONCLIENTMETRICS;
typedef LPNONCLIENTMETRICSA LPNONCLIENTMETRICS;
#endif // UNICODE
#endif /* NOGDI */
#endif /* _WINGDI_ */

#define ARW_BOTTOMLEFT              0x0000L
#define ARW_BOTTOMRIGHT             0x0001L
#define ARW_TOPLEFT                 0x0002L
#define ARW_TOPRIGHT                0x0003L
#define ARW_STARTMASK               0x0003L
#define ARW_STARTRIGHT              0x0001L
#define ARW_STARTTOP                0x0002L

#define ARW_LEFT                    0x0000L
#define ARW_RIGHT                   0x0000L
#define ARW_UP                      0x0004L
#define ARW_DOWN                    0x0004L
#define ARW_HIDE                    0x0008L

typedef struct tagMINIMIZEDMETRICS
{
    UINT    cbSize;
    int     iWidth;
    int     iHorzGap;
    int     iVertGap;
    int     iArrange;
}   MINIMIZEDMETRICS, *PMINIMIZEDMETRICS, *LPMINIMIZEDMETRICS;

#ifdef _WINGDI_
#ifndef NOGDI
typedef struct tagICONMETRICSA
{
    UINT    cbSize;
    int     iHorzSpacing;
    int     iVertSpacing;
    int     iTitleWrap;
    LOGFONTA lfFont;
}   ICONMETRICSA, *PICONMETRICSA, *LPICONMETRICSA;
typedef struct tagICONMETRICSW
{
    UINT    cbSize;
    int     iHorzSpacing;
    int     iVertSpacing;
    int     iTitleWrap;
    LOGFONTW lfFont;
}   ICONMETRICSW, *PICONMETRICSW, *LPICONMETRICSW;
#ifdef UNICODE
typedef ICONMETRICSW ICONMETRICS;
typedef PICONMETRICSW PICONMETRICS;
typedef LPICONMETRICSW LPICONMETRICS;
#else
typedef ICONMETRICSA ICONMETRICS;
typedef PICONMETRICSA PICONMETRICS;
typedef LPICONMETRICSA LPICONMETRICS;
#endif // UNICODE
#endif /* NOGDI */
#endif /* _WINGDI_ */

typedef struct tagANIMATIONINFO
{
    UINT    cbSize;
    int     iMinAnimate;
}   ANIMATIONINFO, *LPANIMATIONINFO;

typedef struct tagSERIALKEYSA
{
    UINT    cbSize;
    DWORD   dwFlags;
    LPSTR     lpszActivePort;
    LPSTR     lpszPort;
    UINT    iBaudRate;
    UINT    iPortState;
    UINT    iActive;
}   SERIALKEYSA, *LPSERIALKEYSA;
typedef struct tagSERIALKEYSW
{
    UINT    cbSize;
    DWORD   dwFlags;
    LPWSTR    lpszActivePort;
    LPWSTR    lpszPort;
    UINT    iBaudRate;
    UINT    iPortState;
    UINT    iActive;
}   SERIALKEYSW, *LPSERIALKEYSW;
#ifdef UNICODE
typedef SERIALKEYSW SERIALKEYS;
typedef LPSERIALKEYSW LPSERIALKEYS;
#else
typedef SERIALKEYSA SERIALKEYS;
typedef LPSERIALKEYSA LPSERIALKEYS;
#endif // UNICODE

/* flags for SERIALKEYS dwFlags field */
#define SERKF_SERIALKEYSON  0x00000001
#define SERKF_AVAILABLE     0x00000002
#define SERKF_INDICATOR     0x00000004


typedef struct tagHIGHCONTRASTA
{
    UINT    cbSize;
    DWORD   dwFlags;
    LPSTR   lpszDefaultScheme;
}   HIGHCONTRASTA, *LPHIGHCONTRASTA;
typedef struct tagHIGHCONTRASTW
{
    UINT    cbSize;
    DWORD   dwFlags;
    LPWSTR  lpszDefaultScheme;
}   HIGHCONTRASTW, *LPHIGHCONTRASTW;
#ifdef UNICODE
typedef HIGHCONTRASTW HIGHCONTRAST;
typedef LPHIGHCONTRASTW LPHIGHCONTRAST;
#else
typedef HIGHCONTRASTA HIGHCONTRAST;
typedef LPHIGHCONTRASTA LPHIGHCONTRAST;
#endif // UNICODE

/* flags for HIGHCONTRAST dwFlags field */
#define HCF_HIGHCONTRASTON  0x00000001
#define HCF_AVAILABLE       0x00000002
#define HCF_HOTKEYACTIVE    0x00000004
#define HCF_CONFIRMHOTKEY   0x00000008
#define HCF_HOTKEYSOUND     0x00000010
#define HCF_INDICATOR       0x00000020
#define HCF_HOTKEYAVAILABLE 0x00000040
#define HCF_LOGONDESKTOP    0x00000100
#define HCF_DEFAULTDESKTOP  0x00000200

/* Flags for ChangeDisplaySettings */
#define CDS_UPDATEREGISTRY  0x00000001
#define CDS_TEST            0x00000002
#define CDS_FULLSCREEN      0x00000004
#define CDS_GLOBAL          0x00000008
#define CDS_SET_PRIMARY     0x00000010
#define CDS_VIDEOPARAMETERS 0x00000020
#define CDS_RESET           0x40000000
#define CDS_NORESET         0x10000000

#include <tvout.h>

/* Return values for ChangeDisplaySettings */
#define DISP_CHANGE_SUCCESSFUL       0
#define DISP_CHANGE_RESTART          1
#define DISP_CHANGE_FAILED          -1
#define DISP_CHANGE_BADMODE         -2
#define DISP_CHANGE_NOTUPDATED      -3
#define DISP_CHANGE_BADFLAGS        -4
#define DISP_CHANGE_BADPARAM        -5
#if(_WIN32_WINNT >= 0x0501)
#define DISP_CHANGE_BADDUALVIEW     -6
#endif /* _WIN32_WINNT >= 0x0501 */

#ifdef _WINGDI_
#ifndef NOGDI

WINUSERAPI
LONG
WINAPI
ChangeDisplaySettingsA(
    __in_opt LPDEVMODEA lpDevMode,
    __in DWORD dwFlags);
WINUSERAPI
LONG
WINAPI
ChangeDisplaySettingsW(
    __in_opt LPDEVMODEW lpDevMode,
    __in DWORD dwFlags);
#ifdef UNICODE
#define ChangeDisplaySettings  ChangeDisplaySettingsW
#else
#define ChangeDisplaySettings  ChangeDisplaySettingsA
#endif // !UNICODE

WINUSERAPI
LONG
WINAPI
ChangeDisplaySettingsExA(
    __in_opt LPCSTR lpszDeviceName,
    __in_opt LPDEVMODEA lpDevMode,
    __reserved HWND hwnd,
    __in DWORD dwflags,
    __in_opt LPVOID lParam);
WINUSERAPI
LONG
WINAPI
ChangeDisplaySettingsExW(
    __in_opt LPCWSTR lpszDeviceName,
    __in_opt LPDEVMODEW lpDevMode,
    __reserved HWND hwnd,
    __in DWORD dwflags,
    __in_opt LPVOID lParam);
#ifdef UNICODE
#define ChangeDisplaySettingsEx  ChangeDisplaySettingsExW
#else
#define ChangeDisplaySettingsEx  ChangeDisplaySettingsExA
#endif // !UNICODE

#define ENUM_CURRENT_SETTINGS       ((DWORD)-1)
#define ENUM_REGISTRY_SETTINGS      ((DWORD)-2)

WINUSERAPI
BOOL
WINAPI
EnumDisplaySettingsA(
    __in_opt LPCSTR lpszDeviceName,
    __in DWORD iModeNum,
    __out LPDEVMODEA lpDevMode);
WINUSERAPI
BOOL
WINAPI
EnumDisplaySettingsW(
    __in_opt LPCWSTR lpszDeviceName,
    __in DWORD iModeNum,
    __out LPDEVMODEW lpDevMode);
#ifdef UNICODE
#define EnumDisplaySettings  EnumDisplaySettingsW
#else
#define EnumDisplaySettings  EnumDisplaySettingsA
#endif // !UNICODE

#if(WINVER >= 0x0500)

WINUSERAPI
BOOL
WINAPI
EnumDisplaySettingsExA(
    __in_opt LPCSTR lpszDeviceName,
    __in DWORD iModeNum,
    __out LPDEVMODEA lpDevMode,
    __in DWORD dwFlags);
WINUSERAPI
BOOL
WINAPI
EnumDisplaySettingsExW(
    __in_opt LPCWSTR lpszDeviceName,
    __in DWORD iModeNum,
    __out LPDEVMODEW lpDevMode,
    __in DWORD dwFlags);
#ifdef UNICODE
#define EnumDisplaySettingsEx  EnumDisplaySettingsExW
#else
#define EnumDisplaySettingsEx  EnumDisplaySettingsExA
#endif // !UNICODE

/* Flags for EnumDisplaySettingsEx */
#define EDS_RAWMODE                   0x00000002

WINUSERAPI
BOOL
WINAPI
EnumDisplayDevicesA(
    __in_opt LPCSTR lpDevice,
    __in DWORD iDevNum,
    __inout PDISPLAY_DEVICEA lpDisplayDevice,
    __in DWORD dwFlags);
WINUSERAPI
BOOL
WINAPI
EnumDisplayDevicesW(
    __in_opt LPCWSTR lpDevice,
    __in DWORD iDevNum,
    __inout PDISPLAY_DEVICEW lpDisplayDevice,
    __in DWORD dwFlags);
#ifdef UNICODE
#define EnumDisplayDevices  EnumDisplayDevicesW
#else
#define EnumDisplayDevices  EnumDisplayDevicesA
#endif // !UNICODE
#endif /* WINVER >= 0x0500 */

#endif /* NOGDI */
#endif /* _WINGDI_ */


WINUSERAPI
BOOL
WINAPI
SystemParametersInfoA(
    __in UINT uiAction,
    __in UINT uiParam,
    __inout_opt PVOID pvParam,
    __in UINT fWinIni);
WINUSERAPI
BOOL
WINAPI
SystemParametersInfoW(
    __in UINT uiAction,
    __in UINT uiParam,
    __inout_opt PVOID pvParam,
    __in UINT fWinIni);
#ifdef UNICODE
#define SystemParametersInfo  SystemParametersInfoW
#else
#define SystemParametersInfo  SystemParametersInfoA
#endif // !UNICODE


#endif  /* !NOSYSPARAMSINFO  */

/*
 * Accessibility support
 */
typedef struct tagFILTERKEYS
{
    UINT  cbSize;
    DWORD dwFlags;
    DWORD iWaitMSec;            // Acceptance Delay
    DWORD iDelayMSec;           // Delay Until Repeat
    DWORD iRepeatMSec;          // Repeat Rate
    DWORD iBounceMSec;          // Debounce Time
} FILTERKEYS, *LPFILTERKEYS;

/*
 * FILTERKEYS dwFlags field
 */
#define FKF_FILTERKEYSON    0x00000001
#define FKF_AVAILABLE       0x00000002
#define FKF_HOTKEYACTIVE    0x00000004
#define FKF_CONFIRMHOTKEY   0x00000008
#define FKF_HOTKEYSOUND     0x00000010
#define FKF_INDICATOR       0x00000020
#define FKF_CLICKON         0x00000040

typedef struct tagSTICKYKEYS
{
    UINT  cbSize;
    DWORD dwFlags;
} STICKYKEYS, *LPSTICKYKEYS;

/*
 * STICKYKEYS dwFlags field
 */
#define SKF_STICKYKEYSON    0x00000001
#define SKF_AVAILABLE       0x00000002
#define SKF_HOTKEYACTIVE    0x00000004
#define SKF_CONFIRMHOTKEY   0x00000008
#define SKF_HOTKEYSOUND     0x00000010
#define SKF_INDICATOR       0x00000020
#define SKF_AUDIBLEFEEDBACK 0x00000040
#define SKF_TRISTATE        0x00000080
#define SKF_TWOKEYSOFF      0x00000100
#if(_WIN32_WINNT >= 0x0500)
#define SKF_LALTLATCHED       0x10000000
#define SKF_LCTLLATCHED       0x04000000
#define SKF_LSHIFTLATCHED     0x01000000
#define SKF_RALTLATCHED       0x20000000
#define SKF_RCTLLATCHED       0x08000000
#define SKF_RSHIFTLATCHED     0x02000000
#define SKF_LWINLATCHED       0x40000000
#define SKF_RWINLATCHED       0x80000000
#define SKF_LALTLOCKED        0x00100000
#define SKF_LCTLLOCKED        0x00040000
#define SKF_LSHIFTLOCKED      0x00010000
#define SKF_RALTLOCKED        0x00200000
#define SKF_RCTLLOCKED        0x00080000
#define SKF_RSHIFTLOCKED      0x00020000
#define SKF_LWINLOCKED        0x00400000
#define SKF_RWINLOCKED        0x00800000
#endif /* _WIN32_WINNT >= 0x0500 */

typedef struct tagMOUSEKEYS
{
    UINT cbSize;
    DWORD dwFlags;
    DWORD iMaxSpeed;
    DWORD iTimeToMaxSpeed;
    DWORD iCtrlSpeed;
    DWORD dwReserved1;
    DWORD dwReserved2;
} MOUSEKEYS, *LPMOUSEKEYS;

/*
 * MOUSEKEYS dwFlags field
 */
#define MKF_MOUSEKEYSON     0x00000001
#define MKF_AVAILABLE       0x00000002
#define MKF_HOTKEYACTIVE    0x00000004
#define MKF_CONFIRMHOTKEY   0x00000008
#define MKF_HOTKEYSOUND     0x00000010
#define MKF_INDICATOR       0x00000020
#define MKF_MODIFIERS       0x00000040
#define MKF_REPLACENUMBERS  0x00000080
#if(_WIN32_WINNT >= 0x0500)
#define MKF_LEFTBUTTONSEL   0x10000000
#define MKF_RIGHTBUTTONSEL  0x20000000
#define MKF_LEFTBUTTONDOWN  0x01000000
#define MKF_RIGHTBUTTONDOWN 0x02000000
#define MKF_MOUSEMODE       0x80000000
#endif /* _WIN32_WINNT >= 0x0500 */

typedef struct tagACCESSTIMEOUT
{
    UINT  cbSize;
    DWORD dwFlags;
    DWORD iTimeOutMSec;
} ACCESSTIMEOUT, *LPACCESSTIMEOUT;

/*
 * ACCESSTIMEOUT dwFlags field
 */
#define ATF_TIMEOUTON       0x00000001
#define ATF_ONOFFFEEDBACK   0x00000002

/* values for SOUNDSENTRY iFSGrafEffect field */
#define SSGF_NONE       0
#define SSGF_DISPLAY    3

/* values for SOUNDSENTRY iFSTextEffect field */
#define SSTF_NONE       0
#define SSTF_CHARS      1
#define SSTF_BORDER     2
#define SSTF_DISPLAY    3

/* values for SOUNDSENTRY iWindowsEffect field */
#define SSWF_NONE     0
#define SSWF_TITLE    1
#define SSWF_WINDOW   2
#define SSWF_DISPLAY  3
#define SSWF_CUSTOM   4

typedef struct tagSOUNDSENTRYA
{
    UINT cbSize;
    DWORD dwFlags;
    DWORD iFSTextEffect;
    DWORD iFSTextEffectMSec;
    DWORD iFSTextEffectColorBits;
    DWORD iFSGrafEffect;
    DWORD iFSGrafEffectMSec;
    DWORD iFSGrafEffectColor;
    DWORD iWindowsEffect;
    DWORD iWindowsEffectMSec;
    LPSTR   lpszWindowsEffectDLL;
    DWORD iWindowsEffectOrdinal;
} SOUNDSENTRYA, *LPSOUNDSENTRYA;
typedef struct tagSOUNDSENTRYW
{
    UINT cbSize;
    DWORD dwFlags;
    DWORD iFSTextEffect;
    DWORD iFSTextEffectMSec;
    DWORD iFSTextEffectColorBits;
    DWORD iFSGrafEffect;
    DWORD iFSGrafEffectMSec;
    DWORD iFSGrafEffectColor;
    DWORD iWindowsEffect;
    DWORD iWindowsEffectMSec;
    LPWSTR  lpszWindowsEffectDLL;
    DWORD iWindowsEffectOrdinal;
} SOUNDSENTRYW, *LPSOUNDSENTRYW;
#ifdef UNICODE
typedef SOUNDSENTRYW SOUNDSENTRY;
typedef LPSOUNDSENTRYW LPSOUNDSENTRY;
#else
typedef SOUNDSENTRYA SOUNDSENTRY;
typedef LPSOUNDSENTRYA LPSOUNDSENTRY;
#endif // UNICODE

/*
 * SOUNDSENTRY dwFlags field
 */
#define SSF_SOUNDSENTRYON   0x00000001
#define SSF_AVAILABLE       0x00000002
#define SSF_INDICATOR       0x00000004

typedef struct tagTOGGLEKEYS
{
    UINT cbSize;
    DWORD dwFlags;
} TOGGLEKEYS, *LPTOGGLEKEYS;

/*
 * TOGGLEKEYS dwFlags field
 */
#define TKF_TOGGLEKEYSON    0x00000001
#define TKF_AVAILABLE       0x00000002
#define TKF_HOTKEYACTIVE    0x00000004
#define TKF_CONFIRMHOTKEY   0x00000008
#define TKF_HOTKEYSOUND     0x00000010
#define TKF_INDICATOR       0x00000020

/*
 * Set debug level
 */

WINUSERAPI
VOID
WINAPI
SetDebugErrorLevel(
    __in DWORD dwLevel);

/*
 * SetLastErrorEx() types.
 */

#define SLE_ERROR       0x00000001
#define SLE_MINORERROR  0x00000002
#define SLE_WARNING     0x00000003

WINUSERAPI
VOID
WINAPI
SetLastErrorEx(
    __in DWORD dwErrCode,
    __in DWORD dwType);

WINUSERAPI
int
WINAPI
InternalGetWindowText(
    __in HWND hWnd,
    __out_ecount_part(cchMaxCount, return + 1) LPWSTR pString,
    __in int cchMaxCount);

#if defined(WINNT)
WINUSERAPI
BOOL
WINAPI
EndTask(
    __in HWND hWnd,
    __in BOOL fShutDown,
    __in BOOL fForce);
#endif


#if(WINVER >= 0x0500)

/*
 * Multimonitor API.
 */

#define MONITOR_DEFAULTTONULL       0x00000000
#define MONITOR_DEFAULTTOPRIMARY    0x00000001
#define MONITOR_DEFAULTTONEAREST    0x00000002

WINUSERAPI
HMONITOR
WINAPI
MonitorFromPoint(
    __in POINT pt,
    __in DWORD dwFlags);

WINUSERAPI
HMONITOR
WINAPI
MonitorFromRect(
    __in LPCRECT lprc,
    __in DWORD dwFlags);

WINUSERAPI
HMONITOR
WINAPI
MonitorFromWindow(
    __in HWND hwnd,
    __in DWORD dwFlags);

#define MONITORINFOF_PRIMARY        0x00000001

#ifndef CCHDEVICENAME
#define CCHDEVICENAME 32
#endif

typedef struct tagMONITORINFO
{
    DWORD   cbSize;
    RECT    rcMonitor;
    RECT    rcWork;
    DWORD   dwFlags;
} MONITORINFO, *LPMONITORINFO;

#ifdef __cplusplus
typedef struct tagMONITORINFOEXA : public tagMONITORINFO
{
    CHAR        szDevice[CCHDEVICENAME];
} MONITORINFOEXA, *LPMONITORINFOEXA;
typedef struct tagMONITORINFOEXW : public tagMONITORINFO
{
    WCHAR       szDevice[CCHDEVICENAME];
} MONITORINFOEXW, *LPMONITORINFOEXW;
#ifdef UNICODE
typedef MONITORINFOEXW MONITORINFOEX;
typedef LPMONITORINFOEXW LPMONITORINFOEX;
#else
typedef MONITORINFOEXA MONITORINFOEX;
typedef LPMONITORINFOEXA LPMONITORINFOEX;
#endif // UNICODE
#else // ndef __cplusplus
typedef struct tagMONITORINFOEXA
{
    MONITORINFO;
    CHAR        szDevice[CCHDEVICENAME];
} MONITORINFOEXA, *LPMONITORINFOEXA;
typedef struct tagMONITORINFOEXW
{
    MONITORINFO;
    WCHAR       szDevice[CCHDEVICENAME];
} MONITORINFOEXW, *LPMONITORINFOEXW;
#ifdef UNICODE
typedef MONITORINFOEXW MONITORINFOEX;
typedef LPMONITORINFOEXW LPMONITORINFOEX;
#else
typedef MONITORINFOEXA MONITORINFOEX;
typedef LPMONITORINFOEXA LPMONITORINFOEX;
#endif // UNICODE
#endif

WINUSERAPI
BOOL
WINAPI
GetMonitorInfoA(
    __in HMONITOR hMonitor,
    __inout LPMONITORINFO lpmi);
WINUSERAPI
BOOL
WINAPI
GetMonitorInfoW(
    __in HMONITOR hMonitor,
    __inout LPMONITORINFO lpmi);
#ifdef UNICODE
#define GetMonitorInfo  GetMonitorInfoW
#else
#define GetMonitorInfo  GetMonitorInfoA
#endif // !UNICODE

typedef BOOL (CALLBACK* MONITORENUMPROC)(HMONITOR, HDC, LPRECT, LPARAM);

WINUSERAPI
BOOL
WINAPI
EnumDisplayMonitors(
    __in_opt HDC hdc,
    __in_opt LPCRECT lprcClip,
    __in MONITORENUMPROC lpfnEnum,
    __in LPARAM dwData);


#ifndef NOWINABLE

/*
 * WinEvents - Active Accessibility hooks
 */

WINUSERAPI
VOID
WINAPI
NotifyWinEvent(
    __in DWORD event,
    __in HWND  hwnd,
    __in LONG  idObject,
    __in LONG  idChild);

typedef VOID (CALLBACK* WINEVENTPROC)(
    HWINEVENTHOOK hWinEventHook,
    DWORD         event,
    HWND          hwnd,
    LONG          idObject,
    LONG          idChild,
    DWORD         idEventThread,
    DWORD         dwmsEventTime);

WINUSERAPI
HWINEVENTHOOK
WINAPI
SetWinEventHook(
    __in DWORD eventMin,
    __in DWORD eventMax,
    __in_opt HMODULE hmodWinEventProc,
    __in WINEVENTPROC pfnWinEventProc,
    __in DWORD idProcess,
    __in DWORD idThread,
    __in DWORD dwFlags);

#if(_WIN32_WINNT >= 0x0501)
WINUSERAPI
BOOL
WINAPI
IsWinEventHookInstalled(
    __in DWORD event);
#endif /* _WIN32_WINNT >= 0x0501 */

/*
 * dwFlags for SetWinEventHook
 */
#define WINEVENT_OUTOFCONTEXT   0x0000  // Events are ASYNC
#define WINEVENT_SKIPOWNTHREAD  0x0001  // Don't call back for events on installer's thread
#define WINEVENT_SKIPOWNPROCESS 0x0002  // Don't call back for events on installer's process
#define WINEVENT_INCONTEXT      0x0004  // Events are SYNC, this causes your dll to be injected into every process

WINUSERAPI
BOOL
WINAPI
UnhookWinEvent(
    __in HWINEVENTHOOK hWinEventHook);

/*
 * idObject values for WinEventProc and NotifyWinEvent
 */

/*
 * hwnd + idObject can be used with OLEACC.DLL's OleGetObjectFromWindow()
 * to get an interface pointer to the container.  indexChild is the item
 * within the container in question.  Setup a VARIANT with vt VT_I4 and
 * lVal the indexChild and pass that in to all methods.  Then you
 * are raring to go.
 */


/*
 * Common object IDs (cookies, only for sending WM_GETOBJECT to get at the
 * thing in question).  Positive IDs are reserved for apps (app specific),
 * negative IDs are system things and are global, 0 means "just little old
 * me".
 */
#define     CHILDID_SELF        0
#define     INDEXID_OBJECT      0
#define     INDEXID_CONTAINER   0

/*
 * Reserved IDs for system objects
 */
#define     OBJID_WINDOW        ((LONG)0x00000000)
#define     OBJID_SYSMENU       ((LONG)0xFFFFFFFF)
#define     OBJID_TITLEBAR      ((LONG)0xFFFFFFFE)
#define     OBJID_MENU          ((LONG)0xFFFFFFFD)
#define     OBJID_CLIENT        ((LONG)0xFFFFFFFC)
#define     OBJID_VSCROLL       ((LONG)0xFFFFFFFB)
#define     OBJID_HSCROLL       ((LONG)0xFFFFFFFA)
#define     OBJID_SIZEGRIP      ((LONG)0xFFFFFFF9)
#define     OBJID_CARET         ((LONG)0xFFFFFFF8)
#define     OBJID_CURSOR        ((LONG)0xFFFFFFF7)
#define     OBJID_ALERT         ((LONG)0xFFFFFFF6)
#define     OBJID_SOUND         ((LONG)0xFFFFFFF5)
#define     OBJID_QUERYCLASSNAMEIDX ((LONG)0xFFFFFFF4)
#define     OBJID_NATIVEOM      ((LONG)0xFFFFFFF0)

/*
 * EVENT DEFINITION
 */
#define EVENT_MIN           0x00000001
#define EVENT_MAX           0x7FFFFFFF


/*
 *  EVENT_SYSTEM_SOUND
 *  Sent when a sound is played.  Currently nothing is generating this, we
 *  this event when a system sound (for menus, etc) is played.  Apps
 *  generate this, if accessible, when a private sound is played.  For
 *  example, if Mail plays a "New Mail" sound.
 *
 *  System Sounds:
 *  (Generated by PlaySoundEvent in USER itself)
 *      hwnd            is NULL
 *      idObject        is OBJID_SOUND
 *      idChild         is sound child ID if one
 *  App Sounds:
 *  (PlaySoundEvent won't generate notification; up to app)
 *      hwnd + idObject gets interface pointer to Sound object
 *      idChild identifies the sound in question
 *  are going to be cleaning up the SOUNDSENTRY feature in the control panel
 *  and will use this at that time.  Applications implementing WinEvents
 *  are perfectly welcome to use it.  Clients of IAccessible* will simply
 *  turn around and get back a non-visual object that describes the sound.
 */
#define EVENT_SYSTEM_SOUND              0x0001

/*
 * EVENT_SYSTEM_ALERT
 * System Alerts:
 * (Generated by MessageBox() calls for example)
 *      hwnd            is hwndMessageBox
 *      idObject        is OBJID_ALERT
 * App Alerts:
 * (Generated whenever)
 *      hwnd+idObject gets interface pointer to Alert
 */
#define EVENT_SYSTEM_ALERT              0x0002

/*
 * EVENT_SYSTEM_FOREGROUND
 * Sent when the foreground (active) window changes, even if it is changing
 * to another window in the same thread as the previous one.
 *      hwnd            is hwndNewForeground
 *      idObject        is OBJID_WINDOW
 *      idChild    is INDEXID_OBJECT
 */
#define EVENT_SYSTEM_FOREGROUND         0x0003

/*
 * Menu
 *      hwnd            is window (top level window or popup menu window)
 *      idObject        is ID of control (OBJID_MENU, OBJID_SYSMENU, OBJID_SELF for popup)
 *      idChild         is CHILDID_SELF
 *
 * EVENT_SYSTEM_MENUSTART
 * EVENT_SYSTEM_MENUEND
 * For MENUSTART, hwnd+idObject+idChild refers to the control with the menu bar,
 *  or the control bringing up the context menu.
 *
 * Sent when entering into and leaving from menu mode (system, app bar, and
 * track popups).
 */
#define EVENT_SYSTEM_MENUSTART          0x0004
#define EVENT_SYSTEM_MENUEND            0x0005

/*
 * EVENT_SYSTEM_MENUPOPUPSTART
 * EVENT_SYSTEM_MENUPOPUPEND
 * Sent when a menu popup comes up and just before it is taken down.  Note
 * that for a call to TrackPopupMenu(), a client will see EVENT_SYSTEM_MENUSTART
 * followed almost immediately by EVENT_SYSTEM_MENUPOPUPSTART for the popup
 * being shown.
 *
 * For MENUPOPUP, hwnd+idObject+idChild refers to the NEW popup coming up, not the
 * parent item which is hierarchical.  You can get the parent menu/popup by
 * asking for the accParent object.
 */
#define EVENT_SYSTEM_MENUPOPUPSTART     0x0006
#define EVENT_SYSTEM_MENUPOPUPEND       0x0007


/*
 * EVENT_SYSTEM_CAPTURESTART
 * EVENT_SYSTEM_CAPTUREEND
 * Sent when a window takes the capture and releases the capture.
 */
#define EVENT_SYSTEM_CAPTURESTART       0x0008
#define EVENT_SYSTEM_CAPTUREEND         0x0009

/*
 * Move Size
 * EVENT_SYSTEM_MOVESIZESTART
 * EVENT_SYSTEM_MOVESIZEEND
 * Sent when a window enters and leaves move-size dragging mode.
 */
#define EVENT_SYSTEM_MOVESIZESTART      0x000A
#define EVENT_SYSTEM_MOVESIZEEND        0x000B

/*
 * Context Help
 * EVENT_SYSTEM_CONTEXTHELPSTART
 * EVENT_SYSTEM_CONTEXTHELPEND
 * Sent when a window enters and leaves context sensitive help mode.
 */
#define EVENT_SYSTEM_CONTEXTHELPSTART   0x000C
#define EVENT_SYSTEM_CONTEXTHELPEND     0x000D

/*
 * Drag & Drop
 * EVENT_SYSTEM_DRAGDROPSTART
 * EVENT_SYSTEM_DRAGDROPEND
 * Send the START notification just before going into drag&drop loop.  Send
 * the END notification just after canceling out.
 * Note that it is up to apps and OLE to generate this, since the system
 * doesn't know.  Like EVENT_SYSTEM_SOUND, it will be a while before this
 * is prevalent.
 */
#define EVENT_SYSTEM_DRAGDROPSTART      0x000E
#define EVENT_SYSTEM_DRAGDROPEND        0x000F

/*
 * Dialog
 * Send the START notification right after the dialog is completely
 *  initialized and visible.  Send the END right before the dialog
 *  is hidden and goes away.
 * EVENT_SYSTEM_DIALOGSTART
 * EVENT_SYSTEM_DIALOGEND
 */
#define EVENT_SYSTEM_DIALOGSTART        0x0010
#define EVENT_SYSTEM_DIALOGEND          0x0011

/*
 * EVENT_SYSTEM_SCROLLING
 * EVENT_SYSTEM_SCROLLINGSTART
 * EVENT_SYSTEM_SCROLLINGEND
 * Sent when beginning and ending the tracking of a scrollbar in a window,
 * and also for scrollbar controls.
 */
#define EVENT_SYSTEM_SCROLLINGSTART     0x0012
#define EVENT_SYSTEM_SCROLLINGEND       0x0013

/*
 * Alt-Tab Window
 * Send the START notification right after the switch window is initialized
 * and visible.  Send the END right before it is hidden and goes away.
 * EVENT_SYSTEM_SWITCHSTART
 * EVENT_SYSTEM_SWITCHEND
 */
#define EVENT_SYSTEM_SWITCHSTART        0x0014
#define EVENT_SYSTEM_SWITCHEND          0x0015

/*
 * EVENT_SYSTEM_MINIMIZESTART
 * EVENT_SYSTEM_MINIMIZEEND
 * Sent when a window minimizes and just before it restores.
 */
#define EVENT_SYSTEM_MINIMIZESTART      0x0016
#define EVENT_SYSTEM_MINIMIZEEND        0x0017


#if(_WIN32_WINNT >= 0x0501)
#define EVENT_CONSOLE_CARET             0x4001
#define EVENT_CONSOLE_UPDATE_REGION     0x4002
#define EVENT_CONSOLE_UPDATE_SIMPLE     0x4003
#define EVENT_CONSOLE_UPDATE_SCROLL     0x4004
#define EVENT_CONSOLE_LAYOUT            0x4005
#define EVENT_CONSOLE_START_APPLICATION 0x4006
#define EVENT_CONSOLE_END_APPLICATION   0x4007

/*
 * Flags for EVENT_CONSOLE_START/END_APPLICATION.
 */
#define CONSOLE_APPLICATION_16BIT       0x0001

/*
 * Flags for EVENT_CONSOLE_CARET
 */
#define CONSOLE_CARET_SELECTION         0x0001
#define CONSOLE_CARET_VISIBLE           0x0002
#endif /* _WIN32_WINNT >= 0x0501 */

/*
 * Object events
 *
 * The system AND apps generate these.  The system generates these for
 * real windows.  Apps generate these for objects within their window which
 * act like a separate control, e.g. an item in a list view.
 *
 * When the system generate them, dwParam2 is always WMOBJID_SELF.  When
 * apps generate them, apps put the has-meaning-to-the-app-only ID value
 * in dwParam2.
 * For all events, if you want detailed accessibility information, callers
 * should
 *      * Call AccessibleObjectFromWindow() with the hwnd, idObject parameters
 *          of the event, and IID_IAccessible as the REFIID, to get back an
 *          IAccessible* to talk to
 *      * Initialize and fill in a VARIANT as VT_I4 with lVal the idChild
 *          parameter of the event.
 *      * If idChild isn't zero, call get_accChild() in the container to see
 *          if the child is an object in its own right.  If so, you will get
 *          back an IDispatch* object for the child.  You should release the
 *          parent, and call QueryInterface() on the child object to get its
 *          IAccessible*.  Then you talk directly to the child.  Otherwise,
 *          if get_accChild() returns you nothing, you should continue to
 *          use the child VARIANT.  You will ask the container for the properties
 *          of the child identified by the VARIANT.  In other words, the
 *          child in this case is accessible but not a full-blown object.
 *          Like a button on a titlebar which is 'small' and has no children.
 */

/*
 * For all EVENT_OBJECT events,
 *      hwnd is the dude to Send the WM_GETOBJECT message to (unless NULL,
 *          see above for system things)
 *      idObject is the ID of the object that can resolve any queries a
 *          client might have.  It's a way to deal with windowless controls,
 *          controls that are just drawn on the screen in some larger parent
 *          window (like SDM), or standard frame elements of a window.
 *      idChild is the piece inside of the object that is affected.  This
 *          allows clients to access things that are too small to have full
 *          blown objects in their own right.  Like the thumb of a scrollbar.
 *          The hwnd/idObject pair gets you to the container, the dude you
 *          probably want to talk to most of the time anyway.  The idChild
 *          can then be passed into the acc properties to get the name/value
 *          of it as needed.
 *
 * Example #1:
 *      System propagating a listbox selection change
 *      EVENT_OBJECT_SELECTION
 *          hwnd == listbox hwnd
 *          idObject == OBJID_WINDOW
 *          idChild == new selected item, or CHILDID_SELF if
 *              nothing now selected within container.
 *      Word '97 propagating a listbox selection change
 *          hwnd == SDM window
 *          idObject == SDM ID to get at listbox 'control'
 *          idChild == new selected item, or CHILDID_SELF if
 *              nothing
 *
 * Example #2:
 *      System propagating a menu item selection on the menu bar
 *      EVENT_OBJECT_SELECTION
 *          hwnd == top level window
 *          idObject == OBJID_MENU
 *          idChild == ID of child menu bar item selected
 *
 * Example #3:
 *      System propagating a dropdown coming off of said menu bar item
 *      EVENT_OBJECT_CREATE
 *          hwnd == popup item
 *          idObject == OBJID_WINDOW
 *          idChild == CHILDID_SELF
 *
 * Example #4:
 *
 * For EVENT_OBJECT_REORDER, the object referred to by hwnd/idObject is the
 * PARENT container in which the zorder is occurring.  This is because if
 * one child is zordering, all of them are changing their relative zorder.
 */
#define EVENT_OBJECT_CREATE                 0x8000  // hwnd + ID + idChild is created item
#define EVENT_OBJECT_DESTROY                0x8001  // hwnd + ID + idChild is destroyed item
#define EVENT_OBJECT_SHOW                   0x8002  // hwnd + ID + idChild is shown item
#define EVENT_OBJECT_HIDE                   0x8003  // hwnd + ID + idChild is hidden item
#define EVENT_OBJECT_REORDER                0x8004  // hwnd + ID + idChild is parent of zordering children
/*
 * NOTE:
 * Minimize the number of notifications!
 *
 * When you are hiding a parent object, obviously all child objects are no
 * longer visible on screen.  They still have the same "visible" status,
 * but are not truly visible.  Hence do not send HIDE notifications for the
 * children also.  One implies all.  The same goes for SHOW.
 */


#define EVENT_OBJECT_FOCUS                  0x8005  // hwnd + ID + idChild is focused item
#define EVENT_OBJECT_SELECTION              0x8006  // hwnd + ID + idChild is selected item (if only one), or idChild is OBJID_WINDOW if complex
#define EVENT_OBJECT_SELECTIONADD           0x8007  // hwnd + ID + idChild is item added
#define EVENT_OBJECT_SELECTIONREMOVE        0x8008  // hwnd + ID + idChild is item removed
#define EVENT_OBJECT_SELECTIONWITHIN        0x8009  // hwnd + ID + idChild is parent of changed selected items

/*
 * NOTES:
 * There is only one "focused" child item in a parent.  This is the place
 * keystrokes are going at a given moment.  Hence only send a notification
 * about where the NEW focus is going.  A NEW item getting the focus already
 * implies that the OLD item is losing it.
 *
 * SELECTION however can be multiple.  Hence the different SELECTION
 * notifications.  Here's when to use each:
 *
 * (1) Send a SELECTION notification in the simple single selection
 *     case (like the focus) when the item with the selection is
 *     merely moving to a different item within a container.  hwnd + ID
 *     is the container control, idChildItem is the new child with the
 *     selection.
 *
 * (2) Send a SELECTIONADD notification when a new item has simply been added
 *     to the selection within a container.  This is appropriate when the
 *     number of newly selected items is very small.  hwnd + ID is the
 *     container control, idChildItem is the new child added to the selection.
 *
 * (3) Send a SELECTIONREMOVE notification when a new item has simply been
 *     removed from the selection within a container.  This is appropriate
 *     when the number of newly selected items is very small, just like
 *     SELECTIONADD.  hwnd + ID is the container control, idChildItem is the
 *     new child removed from the selection.
 *
 * (4) Send a SELECTIONWITHIN notification when the selected items within a
 *     control have changed substantially.  Rather than propagate a large
 *     number of changes to reflect removal for some items, addition of
 *     others, just tell somebody who cares that a lot happened.  It will
 *     be faster an easier for somebody watching to just turn around and
 *     query the container control what the new bunch of selected items
 *     are.
 */

#define EVENT_OBJECT_STATECHANGE            0x800A  // hwnd + ID + idChild is item w/ state change
/*
 * Examples of when to send an EVENT_OBJECT_STATECHANGE include
 *      * It is being enabled/disabled (USER does for windows)
 *      * It is being pressed/released (USER does for buttons)
 *      * It is being checked/unchecked (USER does for radio/check buttons)
 */
#define EVENT_OBJECT_LOCATIONCHANGE         0x800B  // hwnd + ID + idChild is moved/sized item

/*
 * Note:
 * A LOCATIONCHANGE is not sent for every child object when the parent
 * changes shape/moves.  Send one notification for the topmost object
 * that is changing.  For example, if the user resizes a top level window,
 * USER will generate a LOCATIONCHANGE for it, but not for the menu bar,
 * title bar, scrollbars, etc.  that are also changing shape/moving.
 *
 * In other words, it only generates LOCATIONCHANGE notifications for
 * real windows that are moving/sizing.  It will not generate a LOCATIONCHANGE
 * for every non-floating child window when the parent moves (the children are
 * logically moving also on screen, but not relative to the parent).
 *
 * Now, if the app itself resizes child windows as a result of being
 * sized, USER will generate LOCATIONCHANGEs for those dudes also because
 * it doesn't know better.
 *
 * Note also that USER will generate LOCATIONCHANGE notifications for two
 * non-window sys objects:
 *      (1) System caret
 *      (2) Cursor
 */

#define EVENT_OBJECT_NAMECHANGE             0x800C  // hwnd + ID + idChild is item w/ name change
#define EVENT_OBJECT_DESCRIPTIONCHANGE      0x800D  // hwnd + ID + idChild is item w/ desc change
#define EVENT_OBJECT_VALUECHANGE            0x800E  // hwnd + ID + idChild is item w/ value change
#define EVENT_OBJECT_PARENTCHANGE           0x800F  // hwnd + ID + idChild is item w/ new parent
#define EVENT_OBJECT_HELPCHANGE             0x8010  // hwnd + ID + idChild is item w/ help change
#define EVENT_OBJECT_DEFACTIONCHANGE        0x8011  // hwnd + ID + idChild is item w/ def action change
#define EVENT_OBJECT_ACCELERATORCHANGE      0x8012  // hwnd + ID + idChild is item w/ keybd accel change

/*
 * Child IDs
 */

/*
 * System Sounds (idChild of system SOUND notification)
 */
#define SOUND_SYSTEM_STARTUP            1
#define SOUND_SYSTEM_SHUTDOWN           2
#define SOUND_SYSTEM_BEEP               3
#define SOUND_SYSTEM_ERROR              4
#define SOUND_SYSTEM_QUESTION           5
#define SOUND_SYSTEM_WARNING            6
#define SOUND_SYSTEM_INFORMATION        7
#define SOUND_SYSTEM_MAXIMIZE           8
#define SOUND_SYSTEM_MINIMIZE           9
#define SOUND_SYSTEM_RESTOREUP          10
#define SOUND_SYSTEM_RESTOREDOWN        11
#define SOUND_SYSTEM_APPSTART           12
#define SOUND_SYSTEM_FAULT              13
#define SOUND_SYSTEM_APPEND             14
#define SOUND_SYSTEM_MENUCOMMAND        15
#define SOUND_SYSTEM_MENUPOPUP          16
#define CSOUND_SYSTEM                   16

/*
 * System Alerts (indexChild of system ALERT notification)
 */
#define ALERT_SYSTEM_INFORMATIONAL      1       // MB_INFORMATION
#define ALERT_SYSTEM_WARNING            2       // MB_WARNING
#define ALERT_SYSTEM_ERROR              3       // MB_ERROR
#define ALERT_SYSTEM_QUERY              4       // MB_QUESTION
#define ALERT_SYSTEM_CRITICAL           5       // HardSysErrBox
#define CALERT_SYSTEM                   6

typedef struct tagGUITHREADINFO
{
    DWORD   cbSize;
    DWORD   flags;
    HWND    hwndActive;
    HWND    hwndFocus;
    HWND    hwndCapture;
    HWND    hwndMenuOwner;
    HWND    hwndMoveSize;
    HWND    hwndCaret;
    RECT    rcCaret;
} GUITHREADINFO, *PGUITHREADINFO, FAR * LPGUITHREADINFO;

#define GUI_CARETBLINKING   0x00000001
#define GUI_INMOVESIZE      0x00000002
#define GUI_INMENUMODE      0x00000004
#define GUI_SYSTEMMENUMODE  0x00000008
#define GUI_POPUPMENUMODE   0x00000010
#if(_WIN32_WINNT >= 0x0501)
#define GUI_16BITTASK       0x00000020
#endif /* _WIN32_WINNT >= 0x0501 */

WINUSERAPI
BOOL
WINAPI
GetGUIThreadInfo(
    __in DWORD idThread,
    __inout PGUITHREADINFO pgui);


WINUSERAPI
UINT
WINAPI
GetWindowModuleFileNameA(
    __in HWND hwnd,
    __out_ecount_part(cchFileNameMax, return) LPSTR pszFileName,
    __in UINT cchFileNameMax);
WINUSERAPI
UINT
WINAPI
GetWindowModuleFileNameW(
    __in HWND hwnd,
    __out_ecount_part(cchFileNameMax, return) LPWSTR pszFileName,
    __in UINT cchFileNameMax);
#ifdef UNICODE
#define GetWindowModuleFileName  GetWindowModuleFileNameW
#else
#define GetWindowModuleFileName  GetWindowModuleFileNameA
#endif // !UNICODE

#ifndef NO_STATE_FLAGS
#define STATE_SYSTEM_UNAVAILABLE        0x00000001  // Disabled
#define STATE_SYSTEM_SELECTED           0x00000002
#define STATE_SYSTEM_FOCUSED            0x00000004
#define STATE_SYSTEM_PRESSED            0x00000008
#define STATE_SYSTEM_CHECKED            0x00000010
#define STATE_SYSTEM_MIXED              0x00000020  // 3-state checkbox or toolbar button
#define STATE_SYSTEM_INDETERMINATE      STATE_SYSTEM_MIXED
#define STATE_SYSTEM_READONLY           0x00000040
#define STATE_SYSTEM_HOTTRACKED         0x00000080
#define STATE_SYSTEM_DEFAULT            0x00000100
#define STATE_SYSTEM_EXPANDED           0x00000200
#define STATE_SYSTEM_COLLAPSED          0x00000400
#define STATE_SYSTEM_BUSY               0x00000800
#define STATE_SYSTEM_FLOATING           0x00001000  // Children "owned" not "contained" by parent
#define STATE_SYSTEM_MARQUEED           0x00002000
#define STATE_SYSTEM_ANIMATED           0x00004000
#define STATE_SYSTEM_INVISIBLE          0x00008000
#define STATE_SYSTEM_OFFSCREEN          0x00010000
#define STATE_SYSTEM_SIZEABLE           0x00020000
#define STATE_SYSTEM_MOVEABLE           0x00040000
#define STATE_SYSTEM_SELFVOICING        0x00080000
#define STATE_SYSTEM_FOCUSABLE          0x00100000
#define STATE_SYSTEM_SELECTABLE         0x00200000
#define STATE_SYSTEM_LINKED             0x00400000
#define STATE_SYSTEM_TRAVERSED          0x00800000
#define STATE_SYSTEM_MULTISELECTABLE    0x01000000  // Supports multiple selection
#define STATE_SYSTEM_EXTSELECTABLE      0x02000000  // Supports extended selection
#define STATE_SYSTEM_ALERT_LOW          0x04000000  // This information is of low priority
#define STATE_SYSTEM_ALERT_MEDIUM       0x08000000  // This information is of medium priority
#define STATE_SYSTEM_ALERT_HIGH         0x10000000  // This information is of high priority
#define STATE_SYSTEM_PROTECTED          0x20000000  // access to this is restricted
#define STATE_SYSTEM_VALID              0x3FFFFFFF
#endif

#define CCHILDREN_TITLEBAR              5
#define CCHILDREN_SCROLLBAR             5

/*
 * Information about the global cursor.
 */
typedef struct tagCURSORINFO
{
    DWORD   cbSize;
    DWORD   flags;
    HCURSOR hCursor;
    POINT   ptScreenPos;
} CURSORINFO, *PCURSORINFO, *LPCURSORINFO;

#define CURSOR_SHOWING     0x00000001

WINUSERAPI
BOOL
WINAPI
GetCursorInfo(
    __inout PCURSORINFO pci);

/*
 * Window information snapshot
 */
typedef struct tagWINDOWINFO
{
    DWORD cbSize;
    RECT rcWindow;
    RECT rcClient;
    DWORD dwStyle;
    DWORD dwExStyle;
    DWORD dwWindowStatus;
    UINT cxWindowBorders;
    UINT cyWindowBorders;
    ATOM atomWindowType;
    WORD wCreatorVersion;
} WINDOWINFO, *PWINDOWINFO, *LPWINDOWINFO;

#define WS_ACTIVECAPTION    0x0001

WINUSERAPI
BOOL
WINAPI
GetWindowInfo(
    __in HWND hwnd,
    __inout PWINDOWINFO pwi);

/*
 * Titlebar information.
 */
typedef struct tagTITLEBARINFO
{
    DWORD cbSize;
    RECT rcTitleBar;
    DWORD rgstate[CCHILDREN_TITLEBAR + 1];
} TITLEBARINFO, *PTITLEBARINFO, *LPTITLEBARINFO;

WINUSERAPI
BOOL
WINAPI
GetTitleBarInfo(
    __in HWND hwnd,
    __inout PTITLEBARINFO pti);

/*
 * Menubar information
 */
typedef struct tagMENUBARINFO
{
    DWORD cbSize;
    RECT rcBar;          // rect of bar, popup, item
    HMENU hMenu;         // real menu handle of bar, popup
    HWND hwndMenu;       // hwnd of item submenu if one
    BOOL fBarFocused:1;  // bar, popup has the focus
    BOOL fFocused:1;     // item has the focus
} MENUBARINFO, *PMENUBARINFO, *LPMENUBARINFO;

WINUSERAPI
BOOL
WINAPI
GetMenuBarInfo(
    __in HWND hwnd,
    __in LONG idObject,
    __in LONG idItem,
    __inout PMENUBARINFO pmbi);

/*
 * Scrollbar information
 */
typedef struct tagSCROLLBARINFO
{
    DWORD cbSize;
    RECT rcScrollBar;
    int dxyLineButton;
    int xyThumbTop;
    int xyThumbBottom;
    int reserved;
    DWORD rgstate[CCHILDREN_SCROLLBAR + 1];
} SCROLLBARINFO, *PSCROLLBARINFO, *LPSCROLLBARINFO;

WINUSERAPI
BOOL
WINAPI
GetScrollBarInfo(
    __in HWND hwnd,
    __in LONG idObject,
    __inout PSCROLLBARINFO psbi);

/*
 * Combobox information
 */
typedef struct tagCOMBOBOXINFO
{
    DWORD cbSize;
    RECT rcItem;
    RECT rcButton;
    DWORD stateButton;
    HWND hwndCombo;
    HWND hwndItem;
    HWND hwndList;
} COMBOBOXINFO, *PCOMBOBOXINFO, *LPCOMBOBOXINFO;

WINUSERAPI
BOOL
WINAPI
GetComboBoxInfo(
    __in HWND hwndCombo,
    __inout PCOMBOBOXINFO pcbi);

/*
 * The "real" ancestor window
 */
#define     GA_PARENT       1
#define     GA_ROOT         2
#define     GA_ROOTOWNER    3

WINUSERAPI
HWND
WINAPI
GetAncestor(
    __in HWND hwnd,
    __in UINT gaFlags);


/*
 * This gets the REAL child window at the point.  If it is in the dead
 * space of a group box, it will try a sibling behind it.  But static
 * fields will get returned.  In other words, it is kind of a cross between
 * ChildWindowFromPointEx and WindowFromPoint.
 */
WINUSERAPI
HWND
WINAPI
RealChildWindowFromPoint(
    __in HWND hwndParent,
    __in POINT ptParentClientCoords);


/*
 * This gets the name of the window TYPE, not class.  This allows us to
 * recognize ThunderButton32 et al.
 */
WINUSERAPI
UINT
WINAPI
RealGetWindowClassA(
    __in HWND hwnd,
    __out_ecount_part(cchClassNameMax, return) LPSTR ptszClassName,
    __in UINT cchClassNameMax);
/*
 * This gets the name of the window TYPE, not class.  This allows us to
 * recognize ThunderButton32 et al.
 */
WINUSERAPI
UINT
WINAPI
RealGetWindowClassW(
    __in HWND hwnd,
    __out_ecount_part(cchClassNameMax, return) LPWSTR ptszClassName,
    __in UINT cchClassNameMax);
#ifdef UNICODE
#define RealGetWindowClass  RealGetWindowClassW
#else
#define RealGetWindowClass  RealGetWindowClassA
#endif // !UNICODE

/*
 * Alt-Tab Switch window information.
 */
typedef struct tagALTTABINFO
{
    DWORD cbSize;
    int cItems;
    int cColumns;
    int cRows;
    int iColFocus;
    int iRowFocus;
    int cxItem;
    int cyItem;
    POINT ptStart;
} ALTTABINFO, *PALTTABINFO, *LPALTTABINFO;

WINUSERAPI
BOOL
WINAPI
GetAltTabInfoA(
    __in_opt HWND hwnd,
    __in int iItem,
    __inout PALTTABINFO pati,
    __out_ecount_opt(cchItemText) LPSTR pszItemText,
    __in UINT cchItemText);
WINUSERAPI
BOOL
WINAPI
GetAltTabInfoW(
    __in_opt HWND hwnd,
    __in int iItem,
    __inout PALTTABINFO pati,
    __out_ecount_opt(cchItemText) LPWSTR pszItemText,
    __in UINT cchItemText);
#ifdef UNICODE
#define GetAltTabInfo  GetAltTabInfoW
#else
#define GetAltTabInfo  GetAltTabInfoA
#endif // !UNICODE

/*
 * Listbox information.
 * Returns the number of items per row.
 */
WINUSERAPI
DWORD
WINAPI
GetListBoxInfo(
    __in HWND hwnd);

#endif /* NOWINABLE */
#endif /* WINVER >= 0x0500 */


#if(_WIN32_WINNT >= 0x0500)
WINUSERAPI
BOOL
WINAPI
LockWorkStation(
    VOID);
#endif /* _WIN32_WINNT >= 0x0500 */

#if(_WIN32_WINNT >= 0x0500)

WINUSERAPI
BOOL
WINAPI
UserHandleGrantAccess(
    __in HANDLE hUserHandle,
    __in HANDLE hJob,
    __in BOOL   bGrant);

#endif /* _WIN32_WINNT >= 0x0500 */

#if(_WIN32_WINNT >= 0x0501)

/*
 * Raw Input Messages.
 */

DECLARE_HANDLE(HRAWINPUT);

/*
 * WM_INPUT wParam
 */

/*
 * Use this macro to get the input code from wParam.
 */
#define GET_RAWINPUT_CODE_WPARAM(wParam)    ((wParam) & 0xff)

/*
 * The input is in the regular message flow,
 * the app is required to call DefWindowProc
 * so that the system can perform clean ups.
 */
#define RIM_INPUT       0

/*
 * The input is sink only. The app is expected
 * to behave nicely.
 */
#define RIM_INPUTSINK   1


/*
 * Raw Input data header
 */
typedef struct tagRAWINPUTHEADER {
    DWORD dwType;
    DWORD dwSize;
    HANDLE hDevice;
    WPARAM wParam;
} RAWINPUTHEADER, *PRAWINPUTHEADER, *LPRAWINPUTHEADER;

/*
 * Type of the raw input
 */
#define RIM_TYPEMOUSE       0
#define RIM_TYPEKEYBOARD    1
#define RIM_TYPEHID         2

/*
 * Raw format of the mouse input
 */
typedef struct tagRAWMOUSE {
    /*
     * Indicator flags.
     */
    USHORT usFlags;

    /*
     * The transition state of the mouse buttons.
     */
    union {
        ULONG ulButtons;
        struct  {
            USHORT  usButtonFlags;
            USHORT  usButtonData;
        };
    };


    /*
     * The raw state of the mouse buttons.
     */
    ULONG ulRawButtons;

    /*
     * The signed relative or absolute motion in the X direction.
     */
    LONG lLastX;

    /*
     * The signed relative or absolute motion in the Y direction.
     */
    LONG lLastY;

    /*
     * Device-specific additional information for the event.
     */
    ULONG ulExtraInformation;

} RAWMOUSE, *PRAWMOUSE, *LPRAWMOUSE;

/*
 * Define the mouse button state indicators.
 */

#define RI_MOUSE_LEFT_BUTTON_DOWN   0x0001  // Left Button changed to down.
#define RI_MOUSE_LEFT_BUTTON_UP     0x0002  // Left Button changed to up.
#define RI_MOUSE_RIGHT_BUTTON_DOWN  0x0004  // Right Button changed to down.
#define RI_MOUSE_RIGHT_BUTTON_UP    0x0008  // Right Button changed to up.
#define RI_MOUSE_MIDDLE_BUTTON_DOWN 0x0010  // Middle Button changed to down.
#define RI_MOUSE_MIDDLE_BUTTON_UP   0x0020  // Middle Button changed to up.

#define RI_MOUSE_BUTTON_1_DOWN      RI_MOUSE_LEFT_BUTTON_DOWN
#define RI_MOUSE_BUTTON_1_UP        RI_MOUSE_LEFT_BUTTON_UP
#define RI_MOUSE_BUTTON_2_DOWN      RI_MOUSE_RIGHT_BUTTON_DOWN
#define RI_MOUSE_BUTTON_2_UP        RI_MOUSE_RIGHT_BUTTON_UP
#define RI_MOUSE_BUTTON_3_DOWN      RI_MOUSE_MIDDLE_BUTTON_DOWN
#define RI_MOUSE_BUTTON_3_UP        RI_MOUSE_MIDDLE_BUTTON_UP

#define RI_MOUSE_BUTTON_4_DOWN      0x0040
#define RI_MOUSE_BUTTON_4_UP        0x0080
#define RI_MOUSE_BUTTON_5_DOWN      0x0100
#define RI_MOUSE_BUTTON_5_UP        0x0200

/*
 * If usButtonFlags has RI_MOUSE_WHEEL, the wheel delta is stored in usButtonData.
 * Take it as a signed value.
 */
#define RI_MOUSE_WHEEL              0x0400

/*
 * Define the mouse indicator flags.
 */
#define MOUSE_MOVE_RELATIVE         0
#define MOUSE_MOVE_ABSOLUTE         1
#define MOUSE_VIRTUAL_DESKTOP    0x02  // the coordinates are mapped to the virtual desktop
#define MOUSE_ATTRIBUTES_CHANGED 0x04  // requery for mouse attributes


/*
 * Raw format of the keyboard input
 */
typedef struct tagRAWKEYBOARD {
    /*
     * The "make" scan code (key depression).
     */
    USHORT MakeCode;

    /*
     * The flags field indicates a "break" (key release) and other
     * miscellaneous scan code information defined in ntddkbd.h.
     */
    USHORT Flags;

    USHORT Reserved;

    /*
     * Windows message compatible information
     */
    USHORT VKey;
    UINT   Message;

    /*
     * Device-specific additional information for the event.
     */
    ULONG ExtraInformation;


} RAWKEYBOARD, *PRAWKEYBOARD, *LPRAWKEYBOARD;


/*
 * Define the keyboard overrun MakeCode.
 */

#define KEYBOARD_OVERRUN_MAKE_CODE    0xFF

/*
 * Define the keyboard input data Flags.
 */
#define RI_KEY_MAKE             0
#define RI_KEY_BREAK            1
#define RI_KEY_E0               2
#define RI_KEY_E1               4
#define RI_KEY_TERMSRV_SET_LED  8
#define RI_KEY_TERMSRV_SHADOW   0x10


/*
 * Raw format of the input from Human Input Devices
 */
typedef struct tagRAWHID {
    DWORD dwSizeHid;    // byte size of each report
    DWORD dwCount;      // number of input packed
    BYTE bRawData[1];
} RAWHID, *PRAWHID, *LPRAWHID;

/*
 * RAWINPUT data structure.
 */
typedef struct tagRAWINPUT {
    RAWINPUTHEADER header;
    union {
        RAWMOUSE    mouse;
        RAWKEYBOARD keyboard;
        RAWHID      hid;
    } data;
} RAWINPUT, *PRAWINPUT, *LPRAWINPUT;

#ifdef _WIN64
#define RAWINPUT_ALIGN(x)   (((x) + sizeof(QWORD) - 1) & ~(sizeof(QWORD) - 1))
#else   // _WIN64
#define RAWINPUT_ALIGN(x)   (((x) + sizeof(DWORD) - 1) & ~(sizeof(DWORD) - 1))
#endif  // _WIN64

#define NEXTRAWINPUTBLOCK(ptr) ((PRAWINPUT)RAWINPUT_ALIGN((ULONG_PTR)((PBYTE)(ptr) + (ptr)->header.dwSize)))

/*
 * Flags for GetRawInputData
 */

#define RID_INPUT               0x10000003
#define RID_HEADER              0x10000005

WINUSERAPI
UINT
WINAPI
GetRawInputData(
    __in HRAWINPUT hRawInput,
    __in UINT uiCommand,
    __out_bcount_part_opt(*pcbSize, return) LPVOID pData,
    __inout PUINT pcbSize,
    __in UINT cbSizeHeader);

/*
 * Raw Input Device Information
 */
#define RIDI_PREPARSEDDATA      0x20000005
#define RIDI_DEVICENAME         0x20000007  // the return valus is the character length, not the byte size
#define RIDI_DEVICEINFO         0x2000000b

typedef struct tagRID_DEVICE_INFO_MOUSE {
    DWORD dwId;
    DWORD dwNumberOfButtons;
    DWORD dwSampleRate;
} RID_DEVICE_INFO_MOUSE, *PRID_DEVICE_INFO_MOUSE;

typedef struct tagRID_DEVICE_INFO_KEYBOARD {
    DWORD dwType;
    DWORD dwSubType;
    DWORD dwKeyboardMode;
    DWORD dwNumberOfFunctionKeys;
    DWORD dwNumberOfIndicators;
    DWORD dwNumberOfKeysTotal;
} RID_DEVICE_INFO_KEYBOARD, *PRID_DEVICE_INFO_KEYBOARD;

typedef struct tagRID_DEVICE_INFO_HID {
    DWORD dwVendorId;
    DWORD dwProductId;
    DWORD dwVersionNumber;

    /*
     * Top level collection UsagePage and Usage
     */
    USHORT usUsagePage;
    USHORT usUsage;
} RID_DEVICE_INFO_HID, *PRID_DEVICE_INFO_HID;

typedef struct tagRID_DEVICE_INFO {
    DWORD cbSize;
    DWORD dwType;
    union {
        RID_DEVICE_INFO_MOUSE mouse;
        RID_DEVICE_INFO_KEYBOARD keyboard;
        RID_DEVICE_INFO_HID hid;
    };
} RID_DEVICE_INFO, *PRID_DEVICE_INFO, *LPRID_DEVICE_INFO;

WINUSERAPI
UINT
WINAPI
GetRawInputDeviceInfoA(
    __in_opt HANDLE hDevice,
    __in UINT uiCommand,
    __inout_bcount_part_opt(*pcbSize, return) LPVOID pData,
    __inout PUINT pcbSize);
WINUSERAPI
UINT
WINAPI
GetRawInputDeviceInfoW(
    __in_opt HANDLE hDevice,
    __in UINT uiCommand,
    __inout_bcount_part_opt(*pcbSize, return) LPVOID pData,
    __inout PUINT pcbSize);
#ifdef UNICODE
#define GetRawInputDeviceInfo  GetRawInputDeviceInfoW
#else
#define GetRawInputDeviceInfo  GetRawInputDeviceInfoA
#endif // !UNICODE


/*
 * Raw Input Bulk Read: GetRawInputBuffer
 */
WINUSERAPI
UINT
WINAPI
GetRawInputBuffer(
    __out_bcount_opt(*pcbSize) PRAWINPUT pData,
    __inout PUINT pcbSize,
    __in UINT cbSizeHeader);

/*
 * Raw Input request APIs
 */
typedef struct tagRAWINPUTDEVICE {
    USHORT usUsagePage; // Toplevel collection UsagePage
    USHORT usUsage;     // Toplevel collection Usage
    DWORD dwFlags;
    HWND hwndTarget;    // Target hwnd. NULL = follows keyboard focus
} RAWINPUTDEVICE, *PRAWINPUTDEVICE, *LPRAWINPUTDEVICE;

typedef CONST RAWINPUTDEVICE* PCRAWINPUTDEVICE;

#define RIDEV_REMOVE            0x00000001
#define RIDEV_EXCLUDE           0x00000010
#define RIDEV_PAGEONLY          0x00000020
#define RIDEV_NOLEGACY          0x00000030
#define RIDEV_INPUTSINK         0x00000100
#define RIDEV_CAPTUREMOUSE      0x00000200  // effective when mouse nolegacy is specified, otherwise it would be an error
#define RIDEV_NOHOTKEYS         0x00000200  // effective for keyboard.
#define RIDEV_APPKEYS           0x00000400  // effective for keyboard.
#define RIDEV_EXMODEMASK        0x000000F0

#define RIDEV_EXMODE(mode)  ((mode) & RIDEV_EXMODEMASK)

WINUSERAPI
BOOL
WINAPI
RegisterRawInputDevices(
    __in_ecount(uiNumDevices) PCRAWINPUTDEVICE pRawInputDevices,
    __in UINT uiNumDevices,
    __in UINT cbSize);

WINUSERAPI
UINT
WINAPI
GetRegisteredRawInputDevices(
    __out_ecount_opt( *puiNumDevices) PRAWINPUTDEVICE pRawInputDevices,
    __inout PUINT puiNumDevices,
    __in UINT cbSize);


typedef struct tagRAWINPUTDEVICELIST {
    HANDLE hDevice;
    DWORD dwType;
} RAWINPUTDEVICELIST, *PRAWINPUTDEVICELIST;

WINUSERAPI
UINT
WINAPI
GetRawInputDeviceList(
    __out_ecount_opt(*puiNumDevices) PRAWINPUTDEVICELIST pRawInputDeviceList,
    __inout PUINT puiNumDevices,
    __in UINT cbSize);

WINUSERAPI
LRESULT
WINAPI
DefRawInputProc(
    __in_ecount(nInput) PRAWINPUT* paRawInput,
    __in INT nInput,
    __in UINT cbSizeHeader);

#endif /* _WIN32_WINNT >= 0x0501 */




#if !defined(RC_INVOKED) /* RC complains about long symbols in #ifs */
#if defined(ISOLATION_AWARE_ENABLED) && (ISOLATION_AWARE_ENABLED != 0)
#include "winuser.inl"
#endif /* ISOLATION_AWARE_ENABLED */
#endif /* RC */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif /* !_WINUSER_ */





