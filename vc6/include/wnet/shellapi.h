 /*****************************************************************************\
*                                                                             *
* shellapi.h -  SHELL.DLL functions, types, and definitions                   *
*                                                                             *
* Copyright (c) Microsoft Corporation. All rights reserved.                   *
*                                                                             *
\*****************************************************************************/

#ifndef _INC_SHELLAPI
#define _INC_SHELLAPI


//
// Define API decoration for direct importing of DLL references.
//
#ifndef WINSHELLAPI
#if !defined(_SHELL32_)
#define WINSHELLAPI       DECLSPEC_IMPORT
#else
#define WINSHELLAPI
#endif
#endif // WINSHELLAPI

#ifndef SHSTDAPI
#if !defined(_SHELL32_)
#define SHSTDAPI          EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define SHSTDAPI_(type)   EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
#else
#define SHSTDAPI          STDAPI
#define SHSTDAPI_(type)   STDAPI_(type)
#endif
#endif // SHSTDAPI

#ifndef SHDOCAPI
#if !defined(_SHDOCVW_)
#define SHDOCAPI          EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define SHDOCAPI_(type)   EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
#else
#define SHDOCAPI          STDAPI
#define SHDOCAPI_(type)   STDAPI_(type)
#endif
#endif // SHDOCAPI


#if !defined(_WIN64)
#include <pshpack1.h>
#endif

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */



DECLARE_HANDLE(HDROP);

SHSTDAPI_(UINT) DragQueryFileA(HDROP,UINT,LPSTR,UINT);
SHSTDAPI_(UINT) DragQueryFileW(HDROP,UINT,LPWSTR,UINT);
#ifdef UNICODE
#define DragQueryFile  DragQueryFileW
#else
#define DragQueryFile  DragQueryFileA
#endif // !UNICODE
SHSTDAPI_(BOOL) DragQueryPoint(HDROP,LPPOINT);
SHSTDAPI_(void) DragFinish(HDROP);
SHSTDAPI_(void) DragAcceptFiles(HWND,BOOL);

SHSTDAPI_(HINSTANCE) ShellExecuteA(HWND hwnd, LPCSTR lpOperation, LPCSTR lpFile, LPCSTR lpParameters, LPCSTR lpDirectory, INT nShowCmd);
SHSTDAPI_(HINSTANCE) ShellExecuteW(HWND hwnd, LPCWSTR lpOperation, LPCWSTR lpFile, LPCWSTR lpParameters, LPCWSTR lpDirectory, INT nShowCmd);
#ifdef UNICODE
#define ShellExecute  ShellExecuteW
#else
#define ShellExecute  ShellExecuteA
#endif // !UNICODE
SHSTDAPI_(HINSTANCE) FindExecutableA(LPCSTR lpFile, LPCSTR lpDirectory, LPSTR lpResult);
SHSTDAPI_(HINSTANCE) FindExecutableW(LPCWSTR lpFile, LPCWSTR lpDirectory, LPWSTR lpResult);
#ifdef UNICODE
#define FindExecutable  FindExecutableW
#else
#define FindExecutable  FindExecutableA
#endif // !UNICODE
SHSTDAPI_(LPWSTR *)  CommandLineToArgvW(LPCWSTR lpCmdLine, int*pNumArgs);

SHSTDAPI_(INT) ShellAboutA(HWND hWnd, LPCSTR szApp, LPCSTR szOtherStuff, HICON hIcon);
SHSTDAPI_(INT) ShellAboutW(HWND hWnd, LPCWSTR szApp, LPCWSTR szOtherStuff, HICON hIcon);
#ifdef UNICODE
#define ShellAbout  ShellAboutW
#else
#define ShellAbout  ShellAboutA
#endif // !UNICODE
SHSTDAPI_(HICON) DuplicateIcon(HINSTANCE hInst, HICON hIcon);
SHSTDAPI_(HICON) ExtractAssociatedIconA(HINSTANCE hInst, LPSTR lpIconPath, LPWORD lpiIcon);
SHSTDAPI_(HICON) ExtractAssociatedIconW(HINSTANCE hInst, LPWSTR lpIconPath, LPWORD lpiIcon);
#ifdef UNICODE
#define ExtractAssociatedIcon  ExtractAssociatedIconW
#else
#define ExtractAssociatedIcon  ExtractAssociatedIconA
#endif // !UNICODE
#if (_WIN32_IE >= 0x0601)
SHSTDAPI_(HICON) ExtractAssociatedIconExA(HINSTANCE hInst,LPSTR lpIconPath,LPWORD lpiIconIndex, LPWORD lpiIconId);  
SHSTDAPI_(HICON) ExtractAssociatedIconExW(HINSTANCE hInst,LPWSTR lpIconPath,LPWORD lpiIconIndex, LPWORD lpiIconId);  
#ifdef UNICODE
#define ExtractAssociatedIconEx  ExtractAssociatedIconExW
#else
#define ExtractAssociatedIconEx  ExtractAssociatedIconExA
#endif // !UNICODE
#endif // (_WIN32_IE >= 0x0601)
SHSTDAPI_(HICON) ExtractIconA(HINSTANCE hInst, LPCSTR lpszExeFileName, UINT nIconIndex);
SHSTDAPI_(HICON) ExtractIconW(HINSTANCE hInst, LPCWSTR lpszExeFileName, UINT nIconIndex);
#ifdef UNICODE
#define ExtractIcon  ExtractIconW
#else
#define ExtractIcon  ExtractIconA
#endif // !UNICODE

#if(WINVER >= 0x0400)
typedef struct _DRAGINFOA {
    UINT uSize;                 /* init with sizeof(DRAGINFO) */
    POINT pt;
    BOOL fNC;
    LPSTR   lpFileList;
    DWORD grfKeyState;
} DRAGINFOA, *LPDRAGINFOA;
typedef struct _DRAGINFOW {
    UINT uSize;                 /* init with sizeof(DRAGINFO) */
    POINT pt;
    BOOL fNC;
    LPWSTR  lpFileList;
    DWORD grfKeyState;
} DRAGINFOW, *LPDRAGINFOW;
#ifdef UNICODE
typedef DRAGINFOW DRAGINFO;
typedef LPDRAGINFOW LPDRAGINFO;
#else
typedef DRAGINFOA DRAGINFO;
typedef LPDRAGINFOA LPDRAGINFO;
#endif // UNICODE


////
//// AppBar stuff
////
#define ABM_NEW           0x00000000
#define ABM_REMOVE        0x00000001
#define ABM_QUERYPOS      0x00000002
#define ABM_SETPOS        0x00000003
#define ABM_GETSTATE      0x00000004
#define ABM_GETTASKBARPOS 0x00000005
#define ABM_ACTIVATE      0x00000006  // lParam == TRUE/FALSE means activate/deactivate
#define ABM_GETAUTOHIDEBAR 0x00000007
#define ABM_SETAUTOHIDEBAR 0x00000008  // this can fail at any time.  MUST check the result
                                        // lParam = TRUE/FALSE  Set/Unset
                                        // uEdge = what edge
#define ABM_WINDOWPOSCHANGED 0x0000009
#define ABM_SETSTATE      0x0000000a

// these are put in the wparam of callback messages
#define ABN_STATECHANGE    0x0000000
#define ABN_POSCHANGED     0x0000001
#define ABN_FULLSCREENAPP  0x0000002
#define ABN_WINDOWARRANGE  0x0000003 // lParam == TRUE means hide

// flags for get state
#define ABS_AUTOHIDE    0x0000001
#define ABS_ALWAYSONTOP 0x0000002

#define ABE_LEFT        0
#define ABE_TOP         1
#define ABE_RIGHT       2
#define ABE_BOTTOM      3

typedef struct _AppBarData
{
    DWORD cbSize;
    HWND hWnd;
    UINT uCallbackMessage;
    UINT uEdge;
    RECT rc;
    LPARAM lParam; // message specific
} APPBARDATA, *PAPPBARDATA;


SHSTDAPI_(UINT_PTR) SHAppBarMessage(DWORD dwMessage, PAPPBARDATA pData);

////
////  EndAppBar
////

SHSTDAPI_(DWORD)   DoEnvironmentSubstA(LPSTR szString, UINT cchString);
SHSTDAPI_(DWORD)   DoEnvironmentSubstW(LPWSTR szString, UINT cchString);
#ifdef UNICODE
#define DoEnvironmentSubst  DoEnvironmentSubstW
#else
#define DoEnvironmentSubst  DoEnvironmentSubstA
#endif // !UNICODE

#define EIRESID(x) (-1 * (int)(x))
SHSTDAPI_(UINT) ExtractIconExA(LPCSTR lpszFile, int nIconIndex, HICON *phiconLarge, HICON *phiconSmall, UINT nIcons);
SHSTDAPI_(UINT) ExtractIconExW(LPCWSTR lpszFile, int nIconIndex, HICON *phiconLarge, HICON *phiconSmall, UINT nIcons);
#ifdef UNICODE
#define ExtractIconEx  ExtractIconExW
#else
#define ExtractIconEx  ExtractIconExA
#endif // !UNICODE


////
//// Shell File Operations
////
#ifndef FO_MOVE //these need to be kept in sync with the ones in shlobj.h

#define FO_MOVE           0x0001
#define FO_COPY           0x0002
#define FO_DELETE         0x0003
#define FO_RENAME         0x0004

#define FOF_MULTIDESTFILES         0x0001
#define FOF_CONFIRMMOUSE           0x0002
#define FOF_SILENT                 0x0004  // don't create progress/report
#define FOF_RENAMEONCOLLISION      0x0008
#define FOF_NOCONFIRMATION         0x0010  // Don't prompt the user.
#define FOF_WANTMAPPINGHANDLE      0x0020  // Fill in SHFILEOPSTRUCT.hNameMappings
                                      // Must be freed using SHFreeNameMappings
#define FOF_ALLOWUNDO              0x0040
#define FOF_FILESONLY              0x0080  // on *.*, do only files
#define FOF_SIMPLEPROGRESS         0x0100  // means don't show names of files
#define FOF_NOCONFIRMMKDIR         0x0200  // don't confirm making any needed dirs
#define FOF_NOERRORUI              0x0400  // don't put up error UI
#define FOF_NOCOPYSECURITYATTRIBS  0x0800  // dont copy NT file Security Attributes
#define FOF_NORECURSION            0x1000  // don't recurse into directories.
#if (_WIN32_IE >= 0x0500)
#define FOF_NO_CONNECTED_ELEMENTS  0x2000  // don't operate on connected elements.
#define FOF_WANTNUKEWARNING        0x4000  // during delete operation, warn if nuking instead of recycling (partially overrides FOF_NOCONFIRMATION)
#endif // (_WIN32_IE >= 0x500)
#if (_WIN32_WINNT >= 0x0501)
#define FOF_NORECURSEREPARSE       0x8000  // treat reparse points as objects, not containers
#endif // (_WIN32_WINNT >= 0x501)

typedef WORD FILEOP_FLAGS;

#define PO_DELETE       0x0013  // printer is being deleted
#define PO_RENAME       0x0014  // printer is being renamed
#define PO_PORTCHANGE   0x0020  // port this printer connected to is being changed
                                // if this id is set, the strings received by
                                // the copyhook are a doubly-null terminated
                                // list of strings.  The first is the printer
                                // name and the second is the printer port.
#define PO_REN_PORT     0x0034  // PO_RENAME and PO_PORTCHANGE at same time.

// no POF_ flags currently defined

typedef WORD PRINTEROP_FLAGS;

#endif // FO_MOVE

// implicit parameters are:
//      if pFrom or pTo are unqualified names the current directories are
//      taken from the global current drive/directory settings managed
//      by Get/SetCurrentDrive/Directory
//
//      the global confirmation settings

typedef struct _SHFILEOPSTRUCTA
{
        HWND            hwnd;
        UINT            wFunc;
        LPCSTR          pFrom;
        LPCSTR          pTo;
        FILEOP_FLAGS    fFlags;
        BOOL            fAnyOperationsAborted;
        LPVOID          hNameMappings;
        LPCSTR           lpszProgressTitle; // only used if FOF_SIMPLEPROGRESS
} SHFILEOPSTRUCTA, *LPSHFILEOPSTRUCTA;
typedef struct _SHFILEOPSTRUCTW
{
        HWND            hwnd;
        UINT            wFunc;
        LPCWSTR         pFrom;
        LPCWSTR         pTo;
        FILEOP_FLAGS    fFlags;
        BOOL            fAnyOperationsAborted;
        LPVOID          hNameMappings;
        LPCWSTR          lpszProgressTitle; // only used if FOF_SIMPLEPROGRESS
} SHFILEOPSTRUCTW, *LPSHFILEOPSTRUCTW;
#ifdef UNICODE
typedef SHFILEOPSTRUCTW SHFILEOPSTRUCT;
typedef LPSHFILEOPSTRUCTW LPSHFILEOPSTRUCT;
#else
typedef SHFILEOPSTRUCTA SHFILEOPSTRUCT;
typedef LPSHFILEOPSTRUCTA LPSHFILEOPSTRUCT;
#endif // UNICODE

SHSTDAPI_(int) SHFileOperationA(LPSHFILEOPSTRUCTA lpFileOp);
SHSTDAPI_(int) SHFileOperationW(LPSHFILEOPSTRUCTW lpFileOp);
#ifdef UNICODE
#define SHFileOperation  SHFileOperationW
#else
#define SHFileOperation  SHFileOperationA
#endif // !UNICODE
SHSTDAPI_(void) SHFreeNameMappings(HANDLE hNameMappings);

typedef struct _SHNAMEMAPPINGA
{
    LPSTR   pszOldPath;
    LPSTR   pszNewPath;
    int   cchOldPath;
    int   cchNewPath;
} SHNAMEMAPPINGA, *LPSHNAMEMAPPINGA;
typedef struct _SHNAMEMAPPINGW
{
    LPWSTR  pszOldPath;
    LPWSTR  pszNewPath;
    int   cchOldPath;
    int   cchNewPath;
} SHNAMEMAPPINGW, *LPSHNAMEMAPPINGW;
#ifdef UNICODE
typedef SHNAMEMAPPINGW SHNAMEMAPPING;
typedef LPSHNAMEMAPPINGW LPSHNAMEMAPPING;
#else
typedef SHNAMEMAPPINGA SHNAMEMAPPING;
typedef LPSHNAMEMAPPINGA LPSHNAMEMAPPING;
#endif // UNICODE


////
//// End Shell File Operations
////

////
////  Begin ShellExecuteEx and family
////

/* ShellExecute() and ShellExecuteEx() error codes */

/* regular WinExec() codes */
#define SE_ERR_FNF              2       // file not found
#define SE_ERR_PNF              3       // path not found
#define SE_ERR_ACCESSDENIED     5       // access denied
#define SE_ERR_OOM              8       // out of memory
#define SE_ERR_DLLNOTFOUND              32

#endif /* WINVER >= 0x0400 */

/* error values for ShellExecute() beyond the regular WinExec() codes */
#define SE_ERR_SHARE                    26
#define SE_ERR_ASSOCINCOMPLETE          27
#define SE_ERR_DDETIMEOUT               28
#define SE_ERR_DDEFAIL                  29
#define SE_ERR_DDEBUSY                  30
#define SE_ERR_NOASSOC                  31

#if(WINVER >= 0x0400)

// Note CLASSKEY overrides CLASSNAME
#define SEE_MASK_CLASSNAME         0x00000001
#define SEE_MASK_CLASSKEY          0x00000003
// Note INVOKEIDLIST overrides IDLIST
#define SEE_MASK_IDLIST            0x00000004
#define SEE_MASK_INVOKEIDLIST      0x0000000c
#define SEE_MASK_ICON              0x00000010
#define SEE_MASK_HOTKEY            0x00000020
#define SEE_MASK_NOCLOSEPROCESS    0x00000040
#define SEE_MASK_CONNECTNETDRV     0x00000080
#define SEE_MASK_FLAG_DDEWAIT      0x00000100
#define SEE_MASK_DOENVSUBST        0x00000200
#define SEE_MASK_FLAG_NO_UI        0x00000400
#define SEE_MASK_UNICODE           0x00004000
#define SEE_MASK_NO_CONSOLE        0x00008000
#define SEE_MASK_ASYNCOK           0x00100000
#define SEE_MASK_HMONITOR          0x00200000
#if (_WIN32_IE >= 0x0560)
#define SEE_MASK_NOZONECHECKS      0x00800000
#endif // (_WIN32_IE >= 0x560)
#if (_WIN32_IE >= 0x0500)
#define SEE_MASK_NOQUERYCLASSSTORE 0x01000000
#define SEE_MASK_WAITFORINPUTIDLE  0x02000000
#endif // (_WIN32_IE >= 0x500)
#if (_WIN32_IE >= 0x0560)
#define SEE_MASK_FLAG_LOG_USAGE    0x04000000
#endif // (_WIN32_IE >= 0x560)


//
// For compilers that don't support nameless unions
//
#ifndef DUMMYUNIONNAME
#ifdef NONAMELESSUNION
#define DUMMYUNIONNAME   u
#define DUMMYUNIONNAME2  u2
#define DUMMYUNIONNAME3  u3
#define DUMMYUNIONNAME4  u4
#define DUMMYUNIONNAME5  u5
#else
#define DUMMYUNIONNAME
#define DUMMYUNIONNAME2
#define DUMMYUNIONNAME3
#define DUMMYUNIONNAME4
#define DUMMYUNIONNAME5
#endif
#endif // DUMMYUNIONNAME

typedef struct _SHELLEXECUTEINFOA
{
        DWORD cbSize;
        ULONG fMask;
        HWND hwnd;
        LPCSTR   lpVerb;
        LPCSTR   lpFile;
        LPCSTR   lpParameters;
        LPCSTR   lpDirectory;
        int nShow;
        HINSTANCE hInstApp;
        // Optional fields
        LPVOID lpIDList;
        LPCSTR   lpClass;
        HKEY hkeyClass;
        DWORD dwHotKey;
        union {
        HANDLE hIcon;
        HANDLE hMonitor;
        } DUMMYUNIONNAME;
        HANDLE hProcess;
} SHELLEXECUTEINFOA, *LPSHELLEXECUTEINFOA;
typedef struct _SHELLEXECUTEINFOW
{
        DWORD cbSize;
        ULONG fMask;
        HWND hwnd;
        LPCWSTR  lpVerb;
        LPCWSTR  lpFile;
        LPCWSTR  lpParameters;
        LPCWSTR  lpDirectory;
        int nShow;
        HINSTANCE hInstApp;
        // Optional fields
        LPVOID lpIDList;
        LPCWSTR  lpClass;
        HKEY hkeyClass;
        DWORD dwHotKey;
        union {
        HANDLE hIcon;
        HANDLE hMonitor;
        } DUMMYUNIONNAME;
        HANDLE hProcess;
} SHELLEXECUTEINFOW, *LPSHELLEXECUTEINFOW;
#ifdef UNICODE
typedef SHELLEXECUTEINFOW SHELLEXECUTEINFO;
typedef LPSHELLEXECUTEINFOW LPSHELLEXECUTEINFO;
#else
typedef SHELLEXECUTEINFOA SHELLEXECUTEINFO;
typedef LPSHELLEXECUTEINFOA LPSHELLEXECUTEINFO;
#endif // UNICODE

SHSTDAPI_(BOOL) ShellExecuteExA(LPSHELLEXECUTEINFOA lpExecInfo);
SHSTDAPI_(BOOL) ShellExecuteExW(LPSHELLEXECUTEINFOW lpExecInfo);
#ifdef UNICODE
#define ShellExecuteEx  ShellExecuteExW
#else
#define ShellExecuteEx  ShellExecuteExA
#endif // !UNICODE
SHSTDAPI_(void) WinExecErrorA(HWND hwnd, int error, LPCSTR lpstrFileName, LPCSTR lpstrTitle);
SHSTDAPI_(void) WinExecErrorW(HWND hwnd, int error, LPCWSTR lpstrFileName, LPCWSTR lpstrTitle);
#ifdef UNICODE
#define WinExecError  WinExecErrorW
#else
#define WinExecError  WinExecErrorA
#endif // !UNICODE

//
//  SHCreateProcessAsUser()
typedef struct _SHCREATEPROCESSINFOW
{
        DWORD cbSize;
        ULONG fMask;
        HWND hwnd;
        LPCWSTR  pszFile;
        LPCWSTR  pszParameters;
        LPCWSTR  pszCurrentDirectory;
        IN HANDLE hUserToken;
        IN LPSECURITY_ATTRIBUTES lpProcessAttributes;
        IN LPSECURITY_ATTRIBUTES lpThreadAttributes;
        IN BOOL bInheritHandles;
        IN DWORD dwCreationFlags;
        IN LPSTARTUPINFOW lpStartupInfo;
        OUT LPPROCESS_INFORMATION lpProcessInformation;
} SHCREATEPROCESSINFOW, *PSHCREATEPROCESSINFOW;

SHSTDAPI_(BOOL) SHCreateProcessAsUserW(PSHCREATEPROCESSINFOW pscpi);

////
////  End ShellExecuteEx and family
////

//
// RecycleBin
//

// struct for query recycle bin info
typedef struct _SHQUERYRBINFO {
    DWORD   cbSize;
#if !defined(_MAC) || defined(_MAC_INT_64)
    __int64 i64Size;
    __int64 i64NumItems;
#else
    DWORDLONG i64Size;
    DWORDLONG i64NumItems;
#endif
} SHQUERYRBINFO, *LPSHQUERYRBINFO;


// flags for SHEmptyRecycleBin
//
#define SHERB_NOCONFIRMATION    0x00000001
#define SHERB_NOPROGRESSUI      0x00000002
#define SHERB_NOSOUND           0x00000004


SHSTDAPI SHQueryRecycleBinA(LPCSTR pszRootPath, LPSHQUERYRBINFO pSHQueryRBInfo);
SHSTDAPI SHQueryRecycleBinW(LPCWSTR pszRootPath, LPSHQUERYRBINFO pSHQueryRBInfo);
#ifdef UNICODE
#define SHQueryRecycleBin  SHQueryRecycleBinW
#else
#define SHQueryRecycleBin  SHQueryRecycleBinA
#endif // !UNICODE
SHSTDAPI SHEmptyRecycleBinA(HWND hwnd, LPCSTR pszRootPath, DWORD dwFlags);
SHSTDAPI SHEmptyRecycleBinW(HWND hwnd, LPCWSTR pszRootPath, DWORD dwFlags);
#ifdef UNICODE
#define SHEmptyRecycleBin  SHEmptyRecycleBinW
#else
#define SHEmptyRecycleBin  SHEmptyRecycleBinA
#endif // !UNICODE

////
//// end of RecycleBin


////
//// Tray notification definitions
////

typedef struct _NOTIFYICONDATAA {
        DWORD cbSize;
        HWND hWnd;
        UINT uID;
        UINT uFlags;
        UINT uCallbackMessage;
        HICON hIcon;
#if (_WIN32_IE < 0x0500)
        CHAR   szTip[64];
#else
        CHAR   szTip[128];
#endif
#if (_WIN32_IE >= 0x0500)
        DWORD dwState;
        DWORD dwStateMask;
        CHAR   szInfo[256];
        union {
            UINT  uTimeout;
            UINT  uVersion;
        } DUMMYUNIONNAME;
        CHAR   szInfoTitle[64];
        DWORD dwInfoFlags;
#endif
#if (_WIN32_IE >= 0x600)
        GUID guidItem;
#endif
} NOTIFYICONDATAA, *PNOTIFYICONDATAA;
typedef struct _NOTIFYICONDATAW {
        DWORD cbSize;
        HWND hWnd;
        UINT uID;
        UINT uFlags;
        UINT uCallbackMessage;
        HICON hIcon;
#if (_WIN32_IE < 0x0500)
        WCHAR  szTip[64];
#else
        WCHAR  szTip[128];
#endif
#if (_WIN32_IE >= 0x0500)
        DWORD dwState;
        DWORD dwStateMask;
        WCHAR  szInfo[256];
        union {
            UINT  uTimeout;
            UINT  uVersion;
        } DUMMYUNIONNAME;
        WCHAR  szInfoTitle[64];
        DWORD dwInfoFlags;
#endif
#if (_WIN32_IE >= 0x600)
        GUID guidItem;
#endif
} NOTIFYICONDATAW, *PNOTIFYICONDATAW;
#ifdef UNICODE
typedef NOTIFYICONDATAW NOTIFYICONDATA;
typedef PNOTIFYICONDATAW PNOTIFYICONDATA;
#else
typedef NOTIFYICONDATAA NOTIFYICONDATA;
typedef PNOTIFYICONDATAA PNOTIFYICONDATA;
#endif // UNICODE


#define NOTIFYICONDATAA_V1_SIZE     FIELD_OFFSET(NOTIFYICONDATAA, szTip[64])
#define NOTIFYICONDATAW_V1_SIZE     FIELD_OFFSET(NOTIFYICONDATAW, szTip[64])
#ifdef UNICODE
#define NOTIFYICONDATA_V1_SIZE      NOTIFYICONDATAW_V1_SIZE
#else
#define NOTIFYICONDATA_V1_SIZE      NOTIFYICONDATAA_V1_SIZE
#endif

#define NOTIFYICONDATAA_V2_SIZE     FIELD_OFFSET(NOTIFYICONDATAA, guidItem)
#define NOTIFYICONDATAW_V2_SIZE     FIELD_OFFSET(NOTIFYICONDATAW, guidItem)
#ifdef UNICODE
#define NOTIFYICONDATA_V2_SIZE      NOTIFYICONDATAW_V2_SIZE
#else
#define NOTIFYICONDATA_V2_SIZE      NOTIFYICONDATAA_V2_SIZE
#endif


#if (_WIN32_IE >= 0x0500)
#define NIN_SELECT          (WM_USER + 0)
#define NINF_KEY            0x1
#define NIN_KEYSELECT       (NIN_SELECT | NINF_KEY)
#endif

#if (_WIN32_IE >= 0x0501)
#define NIN_BALLOONSHOW     (WM_USER + 2)
#define NIN_BALLOONHIDE     (WM_USER + 3)
#define NIN_BALLOONTIMEOUT  (WM_USER + 4)
#define NIN_BALLOONUSERCLICK (WM_USER + 5)
#endif


#define NIM_ADD         0x00000000
#define NIM_MODIFY      0x00000001
#define NIM_DELETE      0x00000002
#if (_WIN32_IE >= 0x0500)
#define NIM_SETFOCUS    0x00000003
#define NIM_SETVERSION  0x00000004
#define     NOTIFYICON_VERSION 3
#endif

#define NIF_MESSAGE     0x00000001
#define NIF_ICON        0x00000002
#define NIF_TIP         0x00000004
#if (_WIN32_IE >= 0x0500)
#define NIF_STATE       0x00000008
#define NIF_INFO        0x00000010
#endif
#if (_WIN32_IE >= 0x600)
#define NIF_GUID        0x00000020
#endif

#if (_WIN32_IE >= 0x0500)
#define NIS_HIDDEN              0x00000001
#define NIS_SHAREDICON          0x00000002

// says this is the source of a shared icon

// Notify Icon Infotip flags
#define NIIF_NONE       0x00000000
// icon flags are mutually exclusive
// and take only the lowest 2 bits
#define NIIF_INFO       0x00000001
#define NIIF_WARNING    0x00000002
#define NIIF_ERROR      0x00000003
#if (_WIN32_IE >= 0x0600)
#define NIIF_USER       0x00000004
#endif
#define NIIF_ICON_MASK  0x0000000F
#if (_WIN32_IE >= 0x0501)
#define NIIF_NOSOUND    0x00000010
#endif
#endif

SHSTDAPI_(BOOL) Shell_NotifyIconA(DWORD dwMessage, PNOTIFYICONDATAA lpData);
SHSTDAPI_(BOOL) Shell_NotifyIconW(DWORD dwMessage, PNOTIFYICONDATAW lpData);
#ifdef UNICODE
#define Shell_NotifyIcon  Shell_NotifyIconW
#else
#define Shell_NotifyIcon  Shell_NotifyIconA
#endif // !UNICODE

////
//// End Tray Notification Icons
////


#ifndef SHFILEINFO_DEFINED
#define SHFILEINFO_DEFINED
////
//// Begin SHGetFileInfo
////

/*
 * The SHGetFileInfo API provides an easy way to get attributes
 * for a file given a pathname.
 *
 *   PARAMETERS
 *
 *     pszPath              file name to get info about
 *     dwFileAttributes     file attribs, only used with SHGFI_USEFILEATTRIBUTES
 *     psfi                 place to return file info
 *     cbFileInfo           size of structure
 *     uFlags               flags
 *
 *   RETURN
 *     TRUE if things worked
 */

typedef struct _SHFILEINFOA
{
        HICON       hIcon;                      // out: icon
        int         iIcon;                      // out: icon index
        DWORD       dwAttributes;               // out: SFGAO_ flags
        CHAR        szDisplayName[MAX_PATH];    // out: display name (or path)
        CHAR        szTypeName[80];             // out: type name
} SHFILEINFOA;
typedef struct _SHFILEINFOW
{
        HICON       hIcon;                      // out: icon
        int         iIcon;                      // out: icon index
        DWORD       dwAttributes;               // out: SFGAO_ flags
        WCHAR       szDisplayName[MAX_PATH];    // out: display name (or path)
        WCHAR       szTypeName[80];             // out: type name
} SHFILEINFOW;
#ifdef UNICODE
typedef SHFILEINFOW SHFILEINFO;
#else
typedef SHFILEINFOA SHFILEINFO;
#endif // UNICODE


// NOTE: This is also in shlwapi.h.  Please keep in synch.
#endif // !SHFILEINFO_DEFINED

#define SHGFI_ICON              0x000000100     // get icon
#define SHGFI_DISPLAYNAME       0x000000200     // get display name
#define SHGFI_TYPENAME          0x000000400     // get type name
#define SHGFI_ATTRIBUTES        0x000000800     // get attributes
#define SHGFI_ICONLOCATION      0x000001000     // get icon location
#define SHGFI_EXETYPE           0x000002000     // return exe type
#define SHGFI_SYSICONINDEX      0x000004000     // get system icon index
#define SHGFI_LINKOVERLAY       0x000008000     // put a link overlay on icon
#define SHGFI_SELECTED          0x000010000     // show icon in selected state
#define SHGFI_ATTR_SPECIFIED    0x000020000     // get only specified attributes
#define SHGFI_LARGEICON         0x000000000     // get large icon
#define SHGFI_SMALLICON         0x000000001     // get small icon
#define SHGFI_OPENICON          0x000000002     // get open icon
#define SHGFI_SHELLICONSIZE     0x000000004     // get shell size icon
#define SHGFI_PIDL              0x000000008     // pszPath is a pidl
#define SHGFI_USEFILEATTRIBUTES 0x000000010     // use passed dwFileAttribute

#if (_WIN32_IE >= 0x0500)
#define SHGFI_ADDOVERLAYS       0x000000020     // apply the appropriate overlays
#define SHGFI_OVERLAYINDEX      0x000000040     // Get the index of the overlay
                                                // in the upper 8 bits of the iIcon 
#endif

SHSTDAPI_(DWORD_PTR) SHGetFileInfoA(LPCSTR pszPath, DWORD dwFileAttributes, SHFILEINFOA *psfi, UINT cbFileInfo, UINT uFlags);
SHSTDAPI_(DWORD_PTR) SHGetFileInfoW(LPCWSTR pszPath, DWORD dwFileAttributes, SHFILEINFOW *psfi, UINT cbFileInfo, UINT uFlags);
#ifdef UNICODE
#define SHGetFileInfo  SHGetFileInfoW
#else
#define SHGetFileInfo  SHGetFileInfoA
#endif // !UNICODE


#define SHGetDiskFreeSpace SHGetDiskFreeSpaceEx

SHSTDAPI_(BOOL) SHGetDiskFreeSpaceExA(LPCSTR pszDirectoryName, ULARGE_INTEGER* pulFreeBytesAvailableToCaller, ULARGE_INTEGER* pulTotalNumberOfBytes, ULARGE_INTEGER* pulTotalNumberOfFreeBytes);
SHSTDAPI_(BOOL) SHGetDiskFreeSpaceExW(LPCWSTR pszDirectoryName, ULARGE_INTEGER* pulFreeBytesAvailableToCaller, ULARGE_INTEGER* pulTotalNumberOfBytes, ULARGE_INTEGER* pulTotalNumberOfFreeBytes);
#ifdef UNICODE
#define SHGetDiskFreeSpaceEx  SHGetDiskFreeSpaceExW
#else
#define SHGetDiskFreeSpaceEx  SHGetDiskFreeSpaceExA
#endif // !UNICODE
SHSTDAPI_(BOOL) SHGetNewLinkInfoA(LPCSTR pszLinkTo, LPCSTR pszDir, LPSTR pszName, BOOL *pfMustCopy, UINT uFlags);
SHSTDAPI_(BOOL) SHGetNewLinkInfoW(LPCWSTR pszLinkTo, LPCWSTR pszDir, LPWSTR pszName, BOOL *pfMustCopy, UINT uFlags);
#ifdef UNICODE
#define SHGetNewLinkInfo  SHGetNewLinkInfoW
#else
#define SHGetNewLinkInfo  SHGetNewLinkInfoA
#endif // !UNICODE

#define SHGNLI_PIDL             0x000000001     // pszLinkTo is a pidl
#define SHGNLI_PREFIXNAME       0x000000002     // Make name "Shortcut to xxx"
#define SHGNLI_NOUNIQUE         0x000000004     // don't do the unique name generation
#if (_WIN32_IE >= 0x0501)
#define SHGNLI_NOLNK            0x000000008     // don't add ".lnk" extension
#endif // _WIN2_IE >= 0x0501


////
//// End SHGetFileInfo
////

// Printer stuff
#define PRINTACTION_OPEN           0
#define PRINTACTION_PROPERTIES     1
#define PRINTACTION_NETINSTALL     2
#define PRINTACTION_NETINSTALLLINK 3
#define PRINTACTION_TESTPAGE       4
#define PRINTACTION_OPENNETPRN     5
#ifdef WINNT
#define PRINTACTION_DOCUMENTDEFAULTS 6
#define PRINTACTION_SERVERPROPERTIES 7
#endif

SHSTDAPI_(BOOL) SHInvokePrinterCommandA(HWND hwnd, UINT uAction, LPCSTR lpBuf1, LPCSTR lpBuf2, BOOL fModal);
SHSTDAPI_(BOOL) SHInvokePrinterCommandW(HWND hwnd, UINT uAction, LPCWSTR lpBuf1, LPCWSTR lpBuf2, BOOL fModal);
#ifdef UNICODE
#define SHInvokePrinterCommand  SHInvokePrinterCommandW
#else
#define SHInvokePrinterCommand  SHInvokePrinterCommandA
#endif // !UNICODE


#endif /* WINVER >= 0x0400 */

#if (_WIN32_WINNT >= 0x0500) || (_WIN32_WINDOWS >= 0x0500)  

//
// The SHLoadNonloadedIconOverlayIdentifiers API causes the shell's
// icon overlay manager to load any registered icon overlay
// identifers that are not currently loaded.  This is useful if an
// overlay identifier did not load at shell startup but is needed
// and can be loaded at a later time.  Identifiers already loaded
// are not affected.  Overlay identifiers implement the 
// IShellIconOverlayIdentifier interface.
//
// Returns:
//      S_OK
// 
SHSTDAPI SHLoadNonloadedIconOverlayIdentifiers(void);

//
// The SHIsFileAvailableOffline API determines whether a file
// or folder is available for offline use.
//
// Parameters:
//     pwszPath             file name to get info about
//     pdwStatus            (optional) OFFLINE_STATUS_* flags returned here
//
// Returns:
//     S_OK                 File/directory is available offline, unless
//                            OFFLINE_STATUS_INCOMPLETE is returned.
//     E_INVALIDARG         Path is invalid, or not a net path
//     E_FAIL               File/directory is not available offline
// 
// Notes:
//     OFFLINE_STATUS_INCOMPLETE is never returned for directories.
//     Both OFFLINE_STATUS_LOCAL and OFFLINE_STATUS_REMOTE may be returned,
//     indicating "open in both places." This is common when the server is online.
//
SHSTDAPI SHIsFileAvailableOffline(LPCWSTR pwszPath, LPDWORD pdwStatus);

#define OFFLINE_STATUS_LOCAL        0x0001  // If open, it's open locally
#define OFFLINE_STATUS_REMOTE       0x0002  // If open, it's open remotely
#define OFFLINE_STATUS_INCOMPLETE   0x0004  // The local copy is currently imcomplete.
                                            // The file will not be available offline
                                            // until it has been synchronized.

#endif

//  sets the specified path to use the string resource
//  as the UI instead of the file system name
SHSTDAPI SHSetLocalizedName(LPWSTR pszPath, LPCWSTR pszResModule, int idsRes);


//====== ShellMessageBox ================================================

// If lpcTitle is NULL, the title is taken from hWnd
// If lpcText is NULL, this is assumed to be an Out Of Memory message
// If the selector of lpcTitle or lpcText is NULL, the offset should be a
//     string resource ID
// The variable arguments must all be 32-bit values (even if fewer bits
//     are actually used)
// lpcText (or whatever string resource it causes to be loaded) should
//     be a formatting string similar to wsprintf except that only the
//     following formats are available:
//         %%              formats to a single '%'
//         %nn%s           the nn-th arg is a string which is inserted
//         %nn%ld          the nn-th arg is a DWORD, and formatted decimal
//         %nn%lx          the nn-th arg is a DWORD, and formatted hex
//     note that lengths are allowed on the %s, %ld, and %lx, just
//                         like wsprintf
//
#if (_WIN32_IE >= 0x0601)

int _cdecl ShellMessageBoxA(
    HINSTANCE hAppInst,
    HWND hWnd,
    LPCSTR lpcText,
    LPCSTR lpcTitle,
    UINT fuStyle,
    ...);
int _cdecl ShellMessageBoxW(
    HINSTANCE hAppInst,
    HWND hWnd,
    LPCWSTR lpcText,
    LPCWSTR lpcTitle,
    UINT fuStyle,
    ...);
#ifdef UNICODE
#define ShellMessageBox  ShellMessageBoxW
#else
#define ShellMessageBox  ShellMessageBoxA
#endif // !UNICODE

#endif // (_WIN32_IE >= 0x0601)

#if (_WIN32_IE >= 0x0601)
SHSTDAPI_(BOOL) IsLFNDriveA(LPCSTR pszPath);
SHSTDAPI_(BOOL) IsLFNDriveW(LPCWSTR pszPath);
#ifdef UNICODE
#define IsLFNDrive  IsLFNDriveW
#else
#define IsLFNDrive  IsLFNDriveA
#endif // !UNICODE
#endif // (_WIN32_IE >= 0x0601)


#if         _WIN32_IE >= 0x0600

STDAPI          SHEnumerateUnreadMailAccountsA(HKEY hKeyUser, DWORD dwIndex, LPSTR pszMailAddress, int cchMailAddress);
STDAPI          SHEnumerateUnreadMailAccountsW(HKEY hKeyUser, DWORD dwIndex, LPWSTR pszMailAddress, int cchMailAddress);
#ifdef UNICODE
#define SHEnumerateUnreadMailAccounts  SHEnumerateUnreadMailAccountsW
#else
#define SHEnumerateUnreadMailAccounts  SHEnumerateUnreadMailAccountsA
#endif // !UNICODE
STDAPI          SHGetUnreadMailCountA(HKEY hKeyUser, LPCSTR pszMailAddress, DWORD *pdwCount, FILETIME *pFileTime, LPSTR pszShellExecuteCommand, int cchShellExecuteCommand);
STDAPI          SHGetUnreadMailCountW(HKEY hKeyUser, LPCWSTR pszMailAddress, DWORD *pdwCount, FILETIME *pFileTime, LPWSTR pszShellExecuteCommand, int cchShellExecuteCommand);
#ifdef UNICODE
#define SHGetUnreadMailCount  SHGetUnreadMailCountW
#else
#define SHGetUnreadMailCount  SHGetUnreadMailCountA
#endif // !UNICODE
STDAPI          SHSetUnreadMailCountA(LPCSTR pszMailAddress, DWORD dwCount, LPCSTR pszShellExecuteCommand);
STDAPI          SHSetUnreadMailCountW(LPCWSTR pszMailAddress, DWORD dwCount, LPCWSTR pszShellExecuteCommand);
#ifdef UNICODE
#define SHSetUnreadMailCount  SHSetUnreadMailCountW
#else
#define SHSetUnreadMailCount  SHSetUnreadMailCountA
#endif // !UNICODE

#endif  /*  _WIN32_IE >= 0x0600     */

#if (_WIN32_IE >= 0x0601)
STDAPI_(BOOL)   SHTestTokenMembership(HANDLE hToken, ULONG ulRID);
#endif // (_WIN32_IE >= 0x0601)

#if         _WIN32_IE >= 0x0600

STDAPI SHGetImageList(int iImageList, REFIID riid, void **ppvObj);

#define SHIL_LARGE          0   // normally 32x32
#define SHIL_SMALL          1   // normally 16x16
#define SHIL_EXTRALARGE     2
#define SHIL_SYSSMALL       3   // like SHIL_SMALL, but tracks system small icon metric correctly

#define SHIL_LAST           SHIL_SYSSMALL


// Function call types for ntshrui folder sharing helpers
typedef HRESULT (STDMETHODCALLTYPE *PFNCANSHAREFOLDERW)(IN LPCWSTR pszPath);
typedef HRESULT (STDMETHODCALLTYPE *PFNSHOWSHAREFOLDERUIW)(IN HWND hwndParent, IN LPCWSTR pszPath);

#endif  /*  _WIN32_IE >= 0x0600     */

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#if !defined(_WIN64)
#include <poppack.h>
#endif

#endif  /* _INC_SHELLAPI */


