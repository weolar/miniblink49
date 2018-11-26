//===========================================================================
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// File: shlobj.h
//
//===========================================================================

#ifndef _SHLOBJ_H_
#define _SHLOBJ_H_

#ifndef _WINRESRC_
#ifndef _WIN32_IE
#define _WIN32_IE 0x0501
#else
#if (_WIN32_IE < 0x0400) && defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0500)
#error _WIN32_IE setting conflicts with _WIN32_WINNT setting
#endif
#endif
#endif

#ifndef SNDMSG
#ifdef __cplusplus
#define SNDMSG ::SendMessage
#else
#define SNDMSG SendMessage
#endif
#endif // ifndef SNDMSG

//
// Define API decoration for direct importing of DLL references.
//
#ifndef WINSHELLAPI
#if defined(_SHELL32_)
#define WINSHELLAPI
#else
#define WINSHELLAPI       DECLSPEC_IMPORT
#endif
#endif // WINSHELLAPI

#ifndef SHSTDAPI
#if defined(_SHELL32_)
#define SHSTDAPI          STDAPI
#define SHSTDAPI_(type)   STDAPI_(type)
#else
#define SHSTDAPI          EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define SHSTDAPI_(type)   EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
#endif
#endif // SHSTDAPI

#ifndef SHDOCAPI
#if defined(_SHDOCVW_)
#define SHDOCAPI          STDAPI
#define SHDOCAPI_(type)   STDAPI_(type)
#else
#define SHDOCAPI          EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define SHDOCAPI_(type)   EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
#endif
#endif // SHDOCAPI

// shell32 APIs that are also exported from shdocvw
#ifndef SHSTDDOCAPI
#if defined(_SHDOCVW_) || defined(_SHELL32_)
#define SHSTDDOCAPI          STDAPI
#define SHSTDDOCAPI_(type)   STDAPI_(type)
#else
#define SHSTDDOCAPI          EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define SHSTDDOCAPI_(type)   EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
#endif
#endif // SHSTDDOCAPI

#ifndef BROWSEUIAPI
#if defined(_BROWSEUI_)
#define BROWSEUIAPI           STDAPI
#define BROWSEUIAPI_(type)    STDAPI_(type)
#else
#define BROWSEUIAPI           EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define BROWSEUIAPI_(type)    EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
#endif // defined(_BROWSEUI_)
#endif // BROWSEUIAPI

// shell32 APIs that are also exported from shfolder
#ifndef SHFOLDERAPI
#if defined(_SHFOLDER_) || defined(_SHELL32_)
#define SHFOLDERAPI           STDAPI
#else
#define SHFOLDERAPI           EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#endif
#endif



#include <ole2.h>
#ifndef _PRSHT_H_
#include <prsht.h>
#endif
#ifndef _INC_COMMCTRL
#include <commctrl.h>   // for LPTBBUTTON
#endif

#ifndef INITGUID
#include <shlguid.h>
#endif /* !INITGUID */


#include <pshpack1.h>   /* Assume byte packing throughout */

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif /* __cplusplus */

#include <shtypes.h>


#include <shobjidl.h>

//===========================================================================
//
// Task allocator API
//
//  All the shell extensions MUST use the task allocator (see OLE 2.0
// programming guild for its definition) when they allocate or free
// memory objects (mostly ITEMIDLIST) that are returned across any
// shell interfaces. There are two ways to access the task allocator
// from a shell extension depending on whether or not it is linked with
// OLE32.DLL or not (purely for efficiency).
//
// (1) A shell extension which calls any OLE API (i.e., linked with
//  OLE32.DLL) should call OLE's task allocator (by retrieving
//  the task allocator by calling CoGetMalloc API).
//
// (2) A shell extension which does not call any OLE API (i.e., not linked
//  with OLE32.DLL) should call the shell task allocator API (defined
//  below), so that the shell can quickly loads it when OLE32.DLL is not
//  loaded by any application at that point.
//
// Notes:
//  In next version of Windowso release, SHGetMalloc will be replaced by
// the following macro.
//
// #define SHGetMalloc(ppmem)   CoGetMalloc(MEMCTX_TASK, ppmem)
//
//===========================================================================

SHSTDAPI SHGetMalloc(LPMALLOC * ppMalloc);

#if (_WIN32_IE >= 0x0601)
SHSTDAPI_(void *) SHAlloc(SIZE_T cb);
SHSTDAPI_(void)   SHFree(void * pv);
#endif  // (_WIN32_IE >= 0x0601)


//===========================================================================
//
// IContextMenu interface
//
// [OverView]
//
//  The shell uses the IContextMenu interface in following three cases.
//
// case-1: The shell is loading context menu extensions.
//
//   When the user clicks the right mouse button on an item within the shell's
//  name space (i.g., file, directory, server, work-group, etc.), it creates
//  the default context menu for its type, then loads context menu extensions
//  that are registered for that type (and its base type) so that they can
//  add extra menu items. Those context menu extensions are registered at
//  HKCR\{ProgID}\shellex\ContextMenuHandlers.
//
// case-2: The shell is retrieving a context menu of sub-folders in extended
//   name-space.
//
//   When the explorer's name space is extended by name space extensions,
//  the shell calls their IShellFolder::GetUIObjectOf to get the IContextMenu
//  objects when it creates context menus for folders under those extended
//  name spaces.
//
// case-3: The shell is loading non-default drag and drop handler for directories.
//
//   When the user performed a non-default drag and drop onto one of file
//  system folders (i.e., directories), it loads shell extensions that are
//  registered at HKCR\{ProgID}\DragDropHandlers.
//
//
// [Member functions]
//
//
// IContextMenu::QueryContextMenu
//
//   This member function may insert one or more menuitems to the specified
//  menu (hmenu) at the specified location (indexMenu which is never be -1).
//  The IDs of those menuitem must be in the specified range (idCmdFirst and
//  idCmdLast). It returns the maximum menuitem ID offset (ushort) in the
//  'code' field (low word) of the scode.
//
//   The uFlags specify the context. It may have one or more of following
//  flags.
//
//  CMF_DEFAULTONLY: This flag is passed if the user is invoking the default
//   action (typically by double-clicking, case 1 and 2 only). Context menu
//   extensions (case 1) should not add any menu items, and returns NOERROR.
//
//  CMF_VERBSONLY: The explorer passes this flag if it is constructing
//   a context menu for a short-cut object (case 1 and case 2 only). If this
//   flag is passed, it should not add any menu-items that is not appropriate
//   from a short-cut.
//    A good example is the "Delete" menuitem, which confuses the user
//   because it is not clear whether it deletes the link source item or the
//   link itself.
//
//  CMF_EXPLORER: The explorer passes this flag if it has the left-side pane
//   (case 1 and 2 only). Context menu extensions should ignore this flag.
//
//   High word (16-bit) are reserved for context specific communications
//  and the rest of flags (13-bit) are reserved by the system.
//
//
// IContextMenu::InvokeCommand
//
//   This member is called when the user has selected one of menuitems that
//  are inserted by previous QueryContextMenu member. In this case, the
//  LOWORD(lpici->lpVerb) contains the menuitem ID offset (menuitem ID -
//  idCmdFirst).
//
//   This member function may also be called programmatically. In such a case,
//  lpici->lpVerb specifies the canonical name of the command to be invoked,
//  which is typically retrieved by GetCommandString member previously.
//
//  Parameters in lpci:
//    cbSize -- Specifies the size of this structure (sizeof(*lpci))
//    hwnd   -- Specifies the owner window for any message/dialog box.
//    fMask  -- Specifies whether or not dwHotkey/hIcon paramter is valid.
//    lpVerb -- Specifies the command to be invoked.
//    lpParameters -- Parameters (optional)
//    lpDirectory  -- Working directory (optional)
//    nShow -- Specifies the flag to be passed to ShowWindow (SW_*).
//    dwHotKey -- Hot key to be assigned to the app after invoked (optional).
//    hIcon -- Specifies the icon (optional).
//    hMonitor -- Specifies the default monitor (optional).
//
//
// IContextMenu::GetCommandString
//
//   This member function is called by the explorer either to get the
//  canonical (language independent) command name (uFlags == GCS_VERB) or
//  the help text ((uFlags & GCS_HELPTEXT) != 0) for the specified command.
//  The retrieved canonical string may be passed to its InvokeCommand
//  member function to invoke a command programmatically. The explorer
//  displays the help texts in its status bar; therefore, the length of
//  the help text should be reasonably short (<40 characters).
//
//  Parameters:
//   idCmd -- Specifies menuitem ID offset (from idCmdFirst)
//   uFlags -- Either GCS_VERB or GCS_HELPTEXT
//   pwReserved -- Reserved (must pass NULL when calling, must ignore when called)
//   pszName -- Specifies the string buffer.
//   cchMax -- Specifies the size of the string buffer.
//
//===========================================================================

// QueryContextMenu uFlags
#define CMF_NORMAL              0x00000000
#define CMF_DEFAULTONLY         0x00000001
#define CMF_VERBSONLY           0x00000002
#define CMF_EXPLORE             0x00000004
#define CMF_NOVERBS             0x00000008
#define CMF_CANRENAME           0x00000010
#define CMF_NODEFAULT           0x00000020
#define CMF_INCLUDESTATIC       0x00000040
#define CMF_EXTENDEDVERBS       0x00000100      // rarely used verbs
#define CMF_RESERVED            0xffff0000      // View specific


// GetCommandString uFlags
#define GCS_VERBA        0x00000000     // canonical verb
#define GCS_HELPTEXTA    0x00000001     // help text (for status bar)
#define GCS_VALIDATEA    0x00000002     // validate command exists
#define GCS_VERBW        0x00000004     // canonical verb (unicode)
#define GCS_HELPTEXTW    0x00000005     // help text (unicode version)
#define GCS_VALIDATEW    0x00000006     // validate command exists (unicode)
#define GCS_UNICODE      0x00000004     // for bit testing - Unicode string

#ifdef UNICODE
#define GCS_VERB        GCS_VERBW
#define GCS_HELPTEXT    GCS_HELPTEXTW
#define GCS_VALIDATE    GCS_VALIDATEW
#else
#define GCS_VERB        GCS_VERBA
#define GCS_HELPTEXT    GCS_HELPTEXTA
#define GCS_VALIDATE    GCS_VALIDATEA
#endif

#define CMDSTR_NEWFOLDERA   "NewFolder"
#define CMDSTR_VIEWLISTA    "ViewList"
#define CMDSTR_VIEWDETAILSA "ViewDetails"
#define CMDSTR_NEWFOLDERW   L"NewFolder"
#define CMDSTR_VIEWLISTW    L"ViewList"
#define CMDSTR_VIEWDETAILSW L"ViewDetails"

#ifdef UNICODE
#define CMDSTR_NEWFOLDER    CMDSTR_NEWFOLDERW
#define CMDSTR_VIEWLIST     CMDSTR_VIEWLISTW
#define CMDSTR_VIEWDETAILS  CMDSTR_VIEWDETAILSW
#else
#define CMDSTR_NEWFOLDER    CMDSTR_NEWFOLDERA
#define CMDSTR_VIEWLIST     CMDSTR_VIEWLISTA
#define CMDSTR_VIEWDETAILS  CMDSTR_VIEWDETAILSA
#endif

#define CMIC_MASK_HOTKEY        SEE_MASK_HOTKEY
#define CMIC_MASK_ICON          SEE_MASK_ICON
#define CMIC_MASK_FLAG_NO_UI    SEE_MASK_FLAG_NO_UI
#define CMIC_MASK_UNICODE       SEE_MASK_UNICODE
#define CMIC_MASK_NO_CONSOLE    SEE_MASK_NO_CONSOLE
#define CMIC_MASK_HASLINKNAME   SEE_MASK_HASLINKNAME
#define CMIC_MASK_FLAG_SEP_VDM  SEE_MASK_FLAG_SEPVDM
#define CMIC_MASK_HASTITLE      SEE_MASK_HASTITLE
#define CMIC_MASK_ASYNCOK       SEE_MASK_ASYNCOK
#if (_WIN32_IE >= 0x0603)
#define CMIC_MASK_NOZONECHECKS  SEE_MASK_NOZONECHECKS
#endif // (_WIN32_IE >= 0x0603)

#if (_WIN32_IE >= 0x0501)
#define CMIC_MASK_SHIFT_DOWN    0x10000000
#define CMIC_MASK_CONTROL_DOWN  0x40000000
#endif // (_WIN32_IE >= 0x501)
#if (_WIN32_IE >= 0x0560)
#define CMIC_MASK_FLAG_LOG_USAGE SEE_MASK_FLAG_LOG_USAGE
#define CMIC_MASK_NOZONECHECKS  SEE_MASK_NOZONECHECKS
#endif // (_WIN32_IE >= 0x560)


#if (_WIN32_IE >= 0x0400)
#define CMIC_MASK_PTINVOKE      0x20000000
#endif


#include <pshpack8.h>

//NOTE: When SEE_MASK_HMONITOR is set, hIcon is treated as hMonitor
typedef struct _CMINVOKECOMMANDINFO {
    DWORD cbSize;        // sizeof(CMINVOKECOMMANDINFO)
    DWORD fMask;         // any combination of CMIC_MASK_*
    HWND hwnd;           // might be NULL (indicating no owner window)
    LPCSTR lpVerb;       // either a string or MAKEINTRESOURCE(idOffset)
    LPCSTR lpParameters; // might be NULL (indicating no parameter)
    LPCSTR lpDirectory;  // might be NULL (indicating no specific directory)
    int nShow;           // one of SW_ values for ShowWindow() API

    DWORD dwHotKey;
    HANDLE hIcon;
} CMINVOKECOMMANDINFO,  *LPCMINVOKECOMMANDINFO;

typedef struct _CMInvokeCommandInfoEx {
    DWORD cbSize;        // must be sizeof(CMINVOKECOMMANDINFOEX)
    DWORD fMask;         // any combination of CMIC_MASK_*
    HWND hwnd;           // might be NULL (indicating no owner window)
    LPCSTR lpVerb;       // either a string or MAKEINTRESOURCE(idOffset)
    LPCSTR lpParameters; // might be NULL (indicating no parameter)
    LPCSTR lpDirectory;  // might be NULL (indicating no specific directory)
    int nShow;           // one of SW_ values for ShowWindow() API

    DWORD dwHotKey;

    HANDLE hIcon;
    LPCSTR lpTitle;      // For CreateProcess-StartupInfo.lpTitle
    LPCWSTR lpVerbW;        // Unicode verb (for those who can use it)
    LPCWSTR lpParametersW;  // Unicode parameters (for those who can use it)
    LPCWSTR lpDirectoryW;   // Unicode directory (for those who can use it)
    LPCWSTR lpTitleW;       // Unicode title (for those who can use it)
#if (_WIN32_IE >= 0x0400)
    POINT   ptInvoke;       // Point where it's invoked
#endif
} CMINVOKECOMMANDINFOEX,  *LPCMINVOKECOMMANDINFOEX;

#include <poppack.h>        /* Return to byte packing */


#undef  INTERFACE
#define INTERFACE   IContextMenu

DECLARE_INTERFACE_(IContextMenu, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    STDMETHOD(QueryContextMenu)(THIS_
                                HMENU hmenu,
                                UINT indexMenu,
                                UINT idCmdFirst,
                                UINT idCmdLast,
                                UINT uFlags) = 0;

    STDMETHOD(InvokeCommand)(THIS_
                             LPCMINVOKECOMMANDINFO lpici) = 0;

    STDMETHOD(GetCommandString)(THIS_
                                UINT_PTR    idCmd,
                                UINT        uType,
                                UINT      * pwReserved,
                                LPSTR       pszName,
                                UINT        cchMax) = 0;
};

typedef IContextMenu *  LPCONTEXTMENU;


//
// IContextMenu2 (IContextMenu with one new member)
//
// IContextMenu2::HandleMenuMsg
//
//  This function is called, if the client of IContextMenu is aware of
// IContextMenu2 interface and receives one of following messages while
// it is calling TrackPopupMenu (in the window proc of hwnd):
//      WM_INITPOPUP, WM_DRAWITEM and WM_MEASUREITEM
//  The callee may handle these messages to draw owner draw menuitems.
//

#undef  INTERFACE
#define INTERFACE   IContextMenu2

DECLARE_INTERFACE_(IContextMenu2, IContextMenu)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IContextMenu methods ***

    STDMETHOD(QueryContextMenu)(THIS_
                                HMENU hmenu,
                                UINT indexMenu,
                                UINT idCmdFirst,
                                UINT idCmdLast,
                                UINT uFlags) = 0;

    STDMETHOD(InvokeCommand)(THIS_
                             LPCMINVOKECOMMANDINFO lpici) = 0;

    STDMETHOD(GetCommandString)(THIS_
                                UINT_PTR    idCmd,
                                UINT        uType,
                                UINT      * pwReserved,
                                LPSTR       pszName,
                                UINT        cchMax) = 0;

    // *** IContextMenu2 methods ***

    STDMETHOD(HandleMenuMsg)(THIS_
                             UINT uMsg,
                             WPARAM wParam,
                             LPARAM lParam) = 0;
};

typedef IContextMenu2 * LPCONTEXTMENU2;

//
// IContextMenu3 (IContextMenu with one new member)
//
// IContextMenu3::HandleMenuMsg2
//
//  This function is called, if the client of IContextMenu is aware of
// IContextMenu3 interface and receives a menu message while
// it is calling TrackPopupMenu (in the window proc of hwnd):
//

#undef  INTERFACE
#define INTERFACE   IContextMenu3

DECLARE_INTERFACE_(IContextMenu3, IContextMenu2)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IContextMenu methods ***

    STDMETHOD(QueryContextMenu)(THIS_
                                HMENU hmenu,
                                UINT indexMenu,
                                UINT idCmdFirst,
                                UINT idCmdLast,
                                UINT uFlags) = 0;

    STDMETHOD(InvokeCommand)(THIS_
                             LPCMINVOKECOMMANDINFO lpici) = 0;

    STDMETHOD(GetCommandString)(THIS_
                                UINT_PTR    idCmd,
                                UINT        uType,
                                UINT      * pwReserved,
                                LPSTR       pszName,
                                UINT        cchMax) = 0;

    // *** IContextMenu2 methods ***

    STDMETHOD(HandleMenuMsg)(THIS_
                             UINT uMsg,
                             WPARAM wParam,
                             LPARAM lParam) = 0;

    // *** IContextMenu3 methods ***

    STDMETHOD(HandleMenuMsg2)(THIS_
                             UINT uMsg,
                             WPARAM wParam,
                             LPARAM lParam,
                             LRESULT* plResult) = 0;
};

typedef IContextMenu3 * LPCONTEXTMENU3;


#if (_WIN32_IE >= 0x0500)
#undef  INTERFACE
#define INTERFACE   IPersistFolder3

#define CSIDL_FLAG_PFTI_TRACKTARGET CSIDL_FLAG_DONT_VERIFY

// DESCRIPTION: PERSIST_FOLDER_TARGET_INFO
//    This stucture is used for Folder Shortcuts which allow the shell to
// have a file system folder act like another area in the name space.
// One of pidlTargetFolder, szTargetParsingName, or csidl needs to
// specify the destination name space.
//
// pidlTargetFolder: This is a full pidl to the target folder.  Can be NULL in the IPersistFolder3::InitializeEx()
//                   call but not in the GetFolderTargetInfo() return structure.
// szTargetParsingName: Empty string if not specified. Ortherwise, it is the parsible name
//                       to the target.  This name can be parsed by IShellFolder::
//                       ParsedName() from the desktop.
// szNetworkProvider: Can be an empty string.  If not empty, it specifies the type of network
//                    provider that will be used when binding to the target.  This is used
//                    for performance optimizations for the WNet APIs.
// dwAttributes: -1 if not known.  These are the SFGAO_ flags for IShellFolder::GetAttributesOf()
// csidl: This is -1 if it's not used.  This can be used instead of pidlTargetFolder or
//        szTargetParsingName to indicate the TargetFolder.  See the list of CSIDL_ folders
//        below.  CSIDL_FLAG_PFTI_TRACKTARGET means that the IShellFolder's target folder
//        should change if the user changes the target of the underlying CSIDL value.
//        You can also pass CSIDL_FLAG_CREATE to indicate that the target folder
//        should be created if it does not exist.  No other CSIDL_FLAG_* values are supported.

#include <pshpack8.h>

typedef struct
{
    LPITEMIDLIST  pidlTargetFolder;               // pidl for the folder we want to intiailize
    WCHAR         szTargetParsingName[MAX_PATH];  // optional parsing name for the target
    WCHAR         szNetworkProvider[MAX_PATH];    // optional network provider
    DWORD         dwAttributes;                   // optional FILE_ATTRIBUTES_ flags (-1 if not used)
    int           csidl;                          // optional folder index (SHGetFolderPath()) -1 if not used
} PERSIST_FOLDER_TARGET_INFO;

#include <poppack.h>        /* Return to byte packing */


// DESCRIPTION: IPersistFolder3
//    This interface is implemented by an IShellFolder object that wants non-default
// handling of Folder Shortcuts.  In general, shell name space extensions should use
// pidlRoot (the alias pidl) as their location in the name space and pass it to public
// APIs, such as ShellExecute().  The one exception is that pidlTarget should be used
// when sending ChangeNotifies or registering to listen for change notifies
// (see SFVM_GETNOTIFY).
//
// InitializeEx: This method initializes an IShellFolder and specifies where
//               it is rooted in the name space.
//      pbc: May be NULL.
//      pidlRoot: This is the same parameter as IPersistFolder::Initialize(). Caller allocates
//                and frees this parameter.
//      ppfti: May be NULL, in which case this is the same as a call to IPersistFolder::Initialize().
//             Otherwise this is a Folder Shortcut and this structure specifies the target
//             folder and it's attributes.
// GetFolderTargetInfo: This is used by the caller to find information about
//             the folder shortcut.  This structure may not be initialized by the caller,
//             so the callee needs to initialize every member.  The callee allocates
//             pidlTargetFolder and the caller will free it.  Filling in pidlTargetFolder is
//             ALWAYS required.
DECLARE_INTERFACE_(IPersistFolder3, IPersistFolder2)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef)(THIS)  = 0;
    STDMETHOD_(ULONG,Release)(THIS) = 0;

    // *** IPersist methods ***
    STDMETHOD(GetClassID)(THIS_ LPCLSID lpClassID) = 0;

    // *** IPersistFolder methods ***
    STDMETHOD(Initialize)(THIS_ LPCITEMIDLIST pidl) = 0;

    // *** IPersistFolder2 methods ***
    STDMETHOD(GetCurFolder)(THIS_ LPITEMIDLIST *ppidl) = 0;

    // *** IPersistFolder3 methods ***
    STDMETHOD(InitializeEx)(THIS_ IBindCtx *pbc, LPCITEMIDLIST pidlRoot, const PERSIST_FOLDER_TARGET_INFO *ppfti) = 0;
    STDMETHOD(GetFolderTargetInfo)(THIS_ PERSIST_FOLDER_TARGET_INFO *ppfti) = 0;
};


#endif

//

//===========================================================================
//
// IExtractIcon interface
//
//  This interface is used in two different places in the shell.
//
// Case-1: Icons of sub-folders for the scope-pane of the explorer.
//
//  It is used by the explorer to get the "icon location" of
// sub-folders from each shell folders. When the user expands a folder
// in the scope pane of the explorer, the explorer does following:
//  (1) binds to the folder (gets IShellFolder),
//  (2) enumerates its sub-folders by calling its EnumObjects member,
//  (3) calls its GetUIObjectOf member to get IExtractIcon interface
//     for each sub-folders.
//  In this case, the explorer uses only IExtractIcon::GetIconLocation
// member to get the location of the appropriate icon. An icon location
// always consists of a file name (typically DLL or EXE) and either an icon
// resource or an icon index.
//
//
// Case-2: Extracting an icon image from a file
//
//  It is used by the shell when it extracts an icon image
// from a file. When the shell is extracting an icon from a file,
// it does following:
//  (1) creates the icon extraction handler object (by getting its CLSID
//     under the {ProgID}\shell\ExtractIconHanler key and calling
//     CoCreateInstance requesting for IExtractIcon interface).
//  (2) Calls IExtractIcon::GetIconLocation.
//  (3) Then, calls IExtractIcon::ExtractIcon with the location/index pair.
//  (4) If (3) returns NOERROR, it uses the returned icon.
//  (5) Otherwise, it recursively calls this logic with new location
//     assuming that the location string contains a fully qualified path name.
//
//  From extension programmer's point of view, there are only two cases
// where they provide implementations of IExtractIcon:
//  Case-1) providing explorer extensions (i.e., IShellFolder).
//  Case-2) providing per-instance icons for some types of files.
//
// Because Case-1 is described above, we'll explain only Case-2 here.
//
// When the shell is about display an icon for a file, it does following:
//  (1) Finds its ProgID and ClassID.
//  (2) If the file has a ClassID, it gets the icon location string from the
//    "DefaultIcon" key under it. The string indicates either per-class
//    icon (e.g., "FOOBAR.DLL,2") or per-instance icon (e.g., "%1,1").
//  (3) If a per-instance icon is specified, the shell creates an icon
//    extraction handler object for it, and extracts the icon from it
//    (which is described above).
//
//  It is important to note that the shell calls IExtractIcon::GetIconLocation
// first, then calls IExtractIcon::Extract. Most application programs
// that support per-instance icons will probably store an icon location
// (DLL/EXE name and index/id) rather than an icon image in each file.
// In those cases, a programmer needs to implement only the GetIconLocation
// member and it Extract member simply returns S_FALSE. They need to
// implement Extract member only if they decided to store the icon images
// within files themselved or some other database (which is very rare).
//
//
//
// [Member functions]
//
//
// IExtractIcon::GetIconLocation
//
//  This function returns an icon location.
//
//  Parameters:
//   uFlags     [in]  -- Specifies if it is opened or not (GIL_OPENICON or 0)
//   szIconFile [out] -- Specifies the string buffer buffer for a location name.
//   cchMax     [in]  -- Specifies the size of szIconFile (almost always MAX_PATH)
//   piIndex    [out] -- Sepcifies the address of UINT for the index.
//   pwFlags    [out] -- Returns GIL_* flags
//  Returns:
//   NOERROR, if it returns a valid location; S_FALSE, if the shell use a
//   default icon.
//
//  Notes: The location may or may not be a path to a file. The caller can
//   not assume anything unless the subsequent Extract member call returns
//   S_FALSE.
//
//   if the returned location is not a path to a file, GIL_NOTFILENAME should
//   be set in the returned flags.
//
// IExtractIcon::Extract
//
//  This function extracts an icon image from a specified file.
//
//  Parameters:
//   pszFile [in] -- Specifies the icon location (typically a path to a file).
//   nIconIndex [in] -- Specifies the icon index.
//   phiconLarge [out] -- Specifies the HICON variable for large icon.
//   phiconSmall [out] -- Specifies the HICON variable for small icon.
//   nIconSize [in] -- Specifies the size icon required (size of large icon)
//                     LOWORD is the requested large icon size
//                     HIWORD is the requested small icon size
//  Returns:
//   NOERROR, if it extracted the from the file.
//   S_FALSE, if the caller should extract from the file specified in the
//           location.
//
//===========================================================================

// GetIconLocation() input flags

#define GIL_OPENICON     0x0001      // allows containers to specify an "open" look
#define GIL_FORSHELL     0x0002      // icon is to be displayed in a ShellFolder
#define GIL_ASYNC        0x0020      // this is an async extract, return E_PENDING
#define GIL_DEFAULTICON  0x0040      // get the default icon location if the final one takes too long to get
#define GIL_FORSHORTCUT  0x0080      // the icon is for a shortcut to the object

// GetIconLocation() return flags

#define GIL_SIMULATEDOC  0x0001      // simulate this document icon for this
#define GIL_PERINSTANCE  0x0002      // icons from this class are per instance (each file has its own)
#define GIL_PERCLASS     0x0004      // icons from this class per class (shared for all files of this type)
#define GIL_NOTFILENAME  0x0008      // location is not a filename, must call ::ExtractIcon
#define GIL_DONTCACHE    0x0010      // this icon should not be cached

#undef  INTERFACE
#define INTERFACE   IExtractIconA

DECLARE_INTERFACE_(IExtractIconA, IUnknown)     // exic
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IExtractIcon methods ***
    STDMETHOD(GetIconLocation)(THIS_
                         UINT   uFlags,
                         LPSTR  szIconFile,
                         UINT   cchMax,
                         int   * piIndex,
                         UINT  * pwFlags) = 0;

    STDMETHOD(Extract)(THIS_
                           LPCSTR pszFile,
                           UINT   nIconIndex,
                           HICON   *phiconLarge,
                           HICON   *phiconSmall,
                           UINT    nIconSize) = 0;
};

typedef IExtractIconA * LPEXTRACTICONA;

#undef  INTERFACE
#define INTERFACE   IExtractIconW

DECLARE_INTERFACE_(IExtractIconW, IUnknown)     // exic
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IExtractIcon methods ***
    STDMETHOD(GetIconLocation)(THIS_
                         UINT   uFlags,
                         LPWSTR szIconFile,
                         UINT   cchMax,
                         int   * piIndex,
                         UINT  * pwFlags) = 0;

    STDMETHOD(Extract)(THIS_
                           LPCWSTR pszFile,
                           UINT   nIconIndex,
                           HICON   *phiconLarge,
                           HICON   *phiconSmall,
                           UINT    nIconSize) = 0;
};

typedef IExtractIconW * LPEXTRACTICONW;

#ifdef UNICODE
#define IExtractIcon        IExtractIconW
#define IExtractIconVtbl    IExtractIconWVtbl
#define LPEXTRACTICON       LPEXTRACTICONW
#else
#define IExtractIcon        IExtractIconA
#define IExtractIconVtbl    IExtractIconAVtbl
#define LPEXTRACTICON       LPEXTRACTICONA
#endif

//===========================================================================
//
// IShellIcon Interface
//
// used to get a icon index for a IShellFolder object.
//
// this interface can be implemented by a IShellFolder, as a quick way to
// return the icon for a object in the folder.
//
// a instance of this interface is only created once for the folder, unlike
// IExtractIcon witch is created once for each object.
//
// if a ShellFolder does not implement this interface, the standard
// GetUIObject(....IExtractIcon) method will be used to get a icon
// for all objects.
//
// the following standard imagelist indexs can be returned:
//
//      0   document (blank page) (not associated)
//      1   document (with stuff on the page)
//      2   application (exe, com, bat)
//      3   folder (plain)
//      4   folder (open)
//
// IShellIcon:GetIconOf(pidl, flags, lpIconIndex)
//
//      pidl            object to get icon for.
//      flags           GIL_* input flags (GIL_OPEN, ...)
//      lpIconIndex     place to return icon index.
//
//  returns:
//      NOERROR, if lpIconIndex contains the correct system imagelist index.
//      S_FALSE, if unable to get icon for this object, go through
//               GetUIObject, IExtractIcon, methods.
//
//===========================================================================

#undef  INTERFACE
#define INTERFACE   IShellIcon

DECLARE_INTERFACE_(IShellIcon, IUnknown)      // shi
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IShellIcon methods ***
    STDMETHOD(GetIconOf)(THIS_ LPCITEMIDLIST pidl, UINT flags,
                    LPINT lpIconIndex) = 0;
};

typedef IShellIcon *LPSHELLICON;

//===========================================================================
//
// IShellIconOverlayIdentifier
//
// Used to identify a file as a member of the group of files that have this specific
// icon overlay
//
// Users can create new IconOverlayIdentifiers and place them in the following registry
// location together with the Icon overlay image and their priority.
// HKEY_LOCAL_MACHINE "Software\\Microsoft\\Windows\\CurrentVersion\\ShellIconOverlayIdentifiers"
//
// The shell will enumerate through all IconOverlayIdentifiers at start, and prioritize
// them according to internal rules, in case the internal rules don't apply, we use their
// input priority
//
// IShellIconOverlayIdentifier:IsMemberOf(LPCWSTR pwszPath, DWORD dwAttrib)
//      pwszPath        full path of the file
//      dwAttrib        attribute of this file
//
//  returns:
//      S_OK,    if the file is a member
//      S_FALSE, if the file is not a member
//      E_FAIL,  if the operation failed due to bad WIN32_FIND_DATA
//
// IShellIconOverlayIdentifier::GetOverlayInfo(LPWSTR pwszIconFile, int * pIndex, DWORD * dwFlags) = 0;
//      pszIconFile    the path of the icon file
//      pIndex         Depend on the flags, this could contain the IconIndex
//      dwFlags        defined below
//
// IShellIconOverlayIdentifier::GetPriority(int * pIPriority) = 0;
//      pIPriority     the priority of this Overlay Identifier
//
//===========================================================================

#undef  INTERFACE
#define INTERFACE   IShellIconOverlayIdentifier

DECLARE_INTERFACE_(IShellIconOverlayIdentifier, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IShellIconOverlayIdentifier methods ***
    STDMETHOD (IsMemberOf)(THIS_ LPCWSTR pwszPath, DWORD dwAttrib) = 0;
    STDMETHOD (GetOverlayInfo)(THIS_ LPWSTR pwszIconFile, int cchMax, int * pIndex, DWORD * pdwFlags) = 0;
    STDMETHOD (GetPriority)(THIS_ int * pIPriority) = 0;
};

#define ISIOI_ICONFILE            0x00000001          // path is returned through pwszIconFile
#define ISIOI_ICONINDEX           0x00000002          // icon index in pwszIconFile is returned through pIndex

//===========================================================================
//
// IShellIconOverlayManager
//
// Used to return the icon overlay information including OverlayIndex, Image Index or Priority for an IShellFolder object.
//
// IShellIconOverlayManager:GetFileOverlayInfo(LPCWSTR pwszPath, DWORD dwAttrib, int * pIndex, DWORD dwflags)
//      pwszPath        full path of the file
//      dwAttrib        attribute of this file
//      pIndex          pointer to the Icon Index in the system image list
//      pOverlayIndex   pointer to the OverlayIndex in the system image list
//      pPriority       pointer to the Priority of this overlay
// IShellIconOverlayManager:GetReservedOverlayInfo(LPCWSTR pwszPath, DWORD dwAttrib, int * pIndex, DWORD dwflags, int iReservedID)
//      iReservedID     reserved icon overlay id
//  returns:
//      S_OK,  if the index of an Overlay is found
//      S_FALSE, if no Overlay exists for this file
//      E_FAIL, if lpfd is bad
// IShellIconOverlayManager:RefreshOverlayImages(DWORD dwFlags)
//      This will refresh the overlay cache, depends on the dwFlags passed in
//      It will reload the icons into the imagelist, when passed SIOM_ICONINDEX
// IShellIconOverlayManager::LoadNonloadedOverlayIdentifiers()
//      This method loads any registered overlay identifiers (handlers) that
//      are not currently loaded.
// IShellIconOverlayManager::OverlayIndexFromImageIndex(int iImage, int *piIndex, BOOL fAdd)
//      iImage          existing shell image list index to look for
//      piIndex         returned overlay index
//      fAdd            Add image if not already present?
//===========================================================================

#if (_WIN32_IE >= 0x0601)
#undef  INTERFACE
#define INTERFACE   IShellIconOverlayManager

DECLARE_INTERFACE_(IShellIconOverlayManager, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IShellIconOverlayManager methods ***
    STDMETHOD(GetFileOverlayInfo)(THIS_ LPCWSTR pwszPath, DWORD dwAttrib, int * pIndex, DWORD dwflags) = 0;
    STDMETHOD(GetReservedOverlayInfo)(THIS_ LPCWSTR pwszPath, DWORD dwAttrib, int * pIndex, DWORD dwflags, int iReservedID) = 0;
    STDMETHOD(RefreshOverlayImages)(THIS_ DWORD dwFlags) = 0;
    STDMETHOD(LoadNonloadedOverlayIdentifiers)(THIS) = 0;
    STDMETHOD(OverlayIndexFromImageIndex)(THIS_ int iImage, int * piIndex, BOOL fAdd) = 0;
};
#define SIOM_OVERLAYINDEX         1
#define SIOM_ICONINDEX            2
// #define SIOM_PRIORITY          3
#define SIOM_RESERVED_SHARED      0
#define SIOM_RESERVED_LINK        1
#define SIOM_RESERVED_SLOWFILE    2
#endif  // (_WIN32_IE >= 0x0601)

//===========================================================================
//
// IShellIconOverlay
//
// Used to return the icon overlay index or its icon index for an IShellFolder object,
// this is always implemented with IShellFolder
//
// IShellIconOverlay:GetOverlayIndex(LPCITEMIDLIST pidl, DWORD * pdwIndex)
//      pidl            object to identify icon overlay for.
//      pdwIndex        the Overlay Index in the system image list
//
// IShellIconOverlay:GetOverlayIconIndex(LPCITEMIDLIST pidl, DWORD * pdwIndex)
//      pdwIconIndex    the Overlay Icon index in the system image list
// This method is only used for those who are interested in seeing the real bits
// of the Overlay Icon
//
//  returns:
//      S_OK,  if the index of an Overlay is found
//      S_FALSE, if no Overlay exists for this file
//      E_FAIL, if pidl is bad
//
//===========================================================================

#undef  INTERFACE
#define INTERFACE   IShellIconOverlay

DECLARE_INTERFACE_(IShellIconOverlay, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IShellIconOverlay methods ***
    STDMETHOD(GetOverlayIndex)(THIS_ LPCITEMIDLIST pidl, int * pIndex) = 0;
    STDMETHOD(GetOverlayIconIndex)(THIS_ LPCITEMIDLIST pidl, int * pIconIndex) = 0;
};

#define OI_ASYNC 0xFFFFEEEE

//-------------------------------------------------------------------------
//
// SHGetIconOverlayIndex
//
// This function takes the path and icon/res id to the icon and convert it into
// an overlay index in the system image list.
// Note: there are totally only 15 slots for system image overlays, some of which
// was reserved by the system, or taken by the overlayidentifiers, so it's possible
// that this function would fail and return -1;
//
// To get the default overlays in the system, such as the share hand, link shortcut
// and slow files, pass NULL as the file name, then the IDO_SHGIOI_* flags as the icon index
//-------------------------------------------------------------------------

#define IDO_SHGIOI_SHARE  0x0FFFFFFF
#define IDO_SHGIOI_LINK   0x0FFFFFFE
#define IDO_SHGIOI_SLOWFILE 0x0FFFFFFFD
SHSTDAPI_(int) SHGetIconOverlayIndexA(LPCSTR pszIconPath, int iIconIndex);
SHSTDAPI_(int) SHGetIconOverlayIndexW(LPCWSTR pszIconPath, int iIconIndex);
#ifdef UNICODE
#define SHGetIconOverlayIndex  SHGetIconOverlayIndexW
#else
#define SHGetIconOverlayIndex  SHGetIconOverlayIndexA
#endif // !UNICODE


#if (_WIN32_IE >= 0x0400)

// IShellLinkDataList::GetFlags()/SetFlags()
typedef enum {
   SLDF_HAS_ID_LIST         = 0x00000001,   // Shell link saved with ID list
   SLDF_HAS_LINK_INFO       = 0x00000002,   // Shell link saved with LinkInfo
   SLDF_HAS_NAME            = 0x00000004,
   SLDF_HAS_RELPATH         = 0x00000008,
   SLDF_HAS_WORKINGDIR      = 0x00000010,
   SLDF_HAS_ARGS            = 0x00000020,
   SLDF_HAS_ICONLOCATION    = 0x00000040,
   SLDF_UNICODE             = 0x00000080,   // the strings are unicode
   SLDF_FORCE_NO_LINKINFO   = 0x00000100,   // don't create a LINKINFO (make a dumb link)
   SLDF_HAS_EXP_SZ          = 0x00000200,   // the link contains expandable env strings
   SLDF_RUN_IN_SEPARATE     = 0x00000400,   // Run the 16-bit target exe in a separate VDM/WOW
   SLDF_HAS_LOGO3ID         = 0x00000800,   // this link is a special Logo3/MSICD link
   SLDF_HAS_DARWINID        = 0x00001000,   // this link is a special Darwin link
   SLDF_RUNAS_USER          = 0x00002000,   // Run this link as a different user
   SLDF_HAS_EXP_ICON_SZ     = 0x00004000,   // contains expandable env string for icon path
   SLDF_NO_PIDL_ALIAS       = 0x00008000,   // don't ever resolve to a logical location
   SLDF_FORCE_UNCNAME       = 0x00010000,   // make GetPath() prefer the UNC name to the local name
   SLDF_RUN_WITH_SHIMLAYER  = 0x00020000,   // Launch the target of this link w/ shim layer active
   SLDF_RESERVED            = 0x80000000,   // Reserved-- so we can use the low word as an index value in the future
} SHELL_LINK_DATA_FLAGS;

typedef struct tagDATABLOCKHEADER
{
    DWORD   cbSize;             // Size of this extra data block
    DWORD   dwSignature;        // signature of this extra data block
} DATABLOCK_HEADER, *LPDATABLOCK_HEADER, *LPDBLIST;

typedef struct {
#ifdef __cplusplus
    DATABLOCK_HEADER dbh;
#else
    DATABLOCK_HEADER;
#endif
    WORD     wFillAttribute;         // fill attribute for console
    WORD     wPopupFillAttribute;    // fill attribute for console popups
    COORD    dwScreenBufferSize;     // screen buffer size for console
    COORD    dwWindowSize;           // window size for console
    COORD    dwWindowOrigin;         // window origin for console
    DWORD    nFont;
    DWORD    nInputBufferSize;
    COORD    dwFontSize;
    UINT     uFontFamily;
    UINT     uFontWeight;
    WCHAR    FaceName[LF_FACESIZE];
    UINT     uCursorSize;
    BOOL     bFullScreen;
    BOOL     bQuickEdit;
    BOOL     bInsertMode;
    BOOL     bAutoPosition;
    UINT     uHistoryBufferSize;
    UINT     uNumberOfHistoryBuffers;
    BOOL     bHistoryNoDup;
    COLORREF ColorTable[ 16 ];
} NT_CONSOLE_PROPS, *LPNT_CONSOLE_PROPS;
#define NT_CONSOLE_PROPS_SIG 0xA0000002

// This is a FE Console property
typedef struct {
#ifdef __cplusplus
    DATABLOCK_HEADER dbh;
#else
    DATABLOCK_HEADER;
#endif
    UINT     uCodePage;
} NT_FE_CONSOLE_PROPS, *LPNT_FE_CONSOLE_PROPS;
#define NT_FE_CONSOLE_PROPS_SIG 0xA0000004

#if (_WIN32_IE >= 0x0500)
typedef struct {
#ifdef __cplusplus
    DATABLOCK_HEADER dbh;
#else
    DATABLOCK_HEADER;
#endif
    CHAR        szDarwinID[MAX_PATH];  // ANSI darwin ID associated with link
    WCHAR       szwDarwinID[MAX_PATH]; // UNICODE darwin ID associated with link
} EXP_DARWIN_LINK, *LPEXP_DARWIN_LINK;
#define EXP_DARWIN_ID_SIG       0xA0000006
#define EXP_LOGO3_ID_SIG        0xA0000007
#endif

#define EXP_SPECIAL_FOLDER_SIG         0xA0000005   // LPEXP_SPECIAL_FOLDER


typedef struct
{
    DWORD       cbSize;             // Size of this extra data block
    DWORD       dwSignature;        // signature of this extra data block
    DWORD       idSpecialFolder;    // special folder id this link points into
    DWORD       cbOffset;           // ofset into pidl from SLDF_HAS_ID_LIST for child
} EXP_SPECIAL_FOLDER, *LPEXP_SPECIAL_FOLDER;



typedef struct
{
    DWORD       cbSize;             // Size of this extra data block
    DWORD       dwSignature;        // signature of this extra data block
    CHAR        szTarget[ MAX_PATH ];   // ANSI target name w/EXP_SZ in it
    WCHAR       swzTarget[ MAX_PATH ];  // UNICODE target name w/EXP_SZ in it
} EXP_SZ_LINK, *LPEXP_SZ_LINK;
#define EXP_SZ_LINK_SIG                0xA0000001   // LPEXP_SZ_LINK (target)
#define EXP_SZ_ICON_SIG                0xA0000007   // LPEXP_SZ_LINK (icon)

#undef  INTERFACE
#define INTERFACE IShellLinkDataList

DECLARE_INTERFACE_(IShellLinkDataList, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG, Release) (THIS) = 0;

    // *** IShellLinkDataList methods ***
    STDMETHOD(AddDataBlock)(THIS_ void * pDataBlock) = 0;
    STDMETHOD(CopyDataBlock)(THIS_ DWORD dwSig, void **ppDataBlock) = 0;
    STDMETHOD(RemoveDataBlock)(THIS_ DWORD dwSig) = 0;
    STDMETHOD(GetFlags)(THIS_ DWORD *pdwFlags) = 0;
    STDMETHOD(SetFlags)(THIS_ DWORD dwFlags) = 0;
};

#endif // (_WIN32_IE >= 0x0400)

#if (_WIN32_IE >= 0x0500)
#undef  INTERFACE
#define INTERFACE IResolveShellLink

DECLARE_INTERFACE_(IResolveShellLink, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG, Release) (THIS) = 0;

    // *** IResolveShellLink methods ***
    STDMETHOD(ResolveShellLink)(THIS_ IUnknown* punk, HWND hwnd, DWORD fFlags) = 0;
};
#endif // (_WIN32_IE >= 0x0500)


#ifdef _INC_SHELLAPI    /* for LPSHELLEXECUTEINFO */
//===========================================================================
//
// IShellExecuteHook Interface
//
//===========================================================================

#undef  INTERFACE
#define INTERFACE   IShellExecuteHookA

DECLARE_INTERFACE_(IShellExecuteHookA, IUnknown) // shexhk
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG, Release) (THIS) = 0;

    // *** IShellExecuteHookA methods ***
    STDMETHOD(Execute)(THIS_ LPSHELLEXECUTEINFOA pei) = 0;
};

#undef  INTERFACE
#define INTERFACE   IShellExecuteHookW

DECLARE_INTERFACE_(IShellExecuteHookW, IUnknown) // shexhk
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG, Release) (THIS) = 0;

    // *** IShellExecuteHookW methods ***
    STDMETHOD(Execute)(THIS_ LPSHELLEXECUTEINFOW pei) = 0;
};

#ifdef UNICODE
#define IShellExecuteHook       IShellExecuteHookW
#define IShellExecuteHookVtbl   IShellExecuteHookWVtbl
#else
#define IShellExecuteHook       IShellExecuteHookA
#define IShellExecuteHookVtbl   IShellExecuteHookAVtbl
#endif
#endif

//===========================================================================
//
// IURLSearchHook Interface
//
//===========================================================================

#undef  INTERFACE
#define INTERFACE   IURLSearchHook

DECLARE_INTERFACE_(IURLSearchHook, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG, Release) (THIS) = 0;

    // *** IURLSearchHook methods ***
    STDMETHOD(Translate)(THIS_ LPWSTR lpwszSearchURL, DWORD cchBufferSize) = 0;
};

#undef  INTERFACE
#define INTERFACE   ISearchContext

DECLARE_INTERFACE_(ISearchContext, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG, Release) (THIS) = 0;

    // *** ISearchContext methods ***
    STDMETHOD(GetSearchUrl)(THIS_ BSTR * pbstrSearchUrl) = 0;
    STDMETHOD(GetSearchText)(THIS_ BSTR * pbstrSearchText) = 0;
    STDMETHOD(GetSearchStyle)(THIS_ DWORD * pdwSearchStyle) = 0;
};

#undef  INTERFACE
#define INTERFACE   IURLSearchHook2

DECLARE_INTERFACE_(IURLSearchHook2, IURLSearchHook)
{
    // *** IURLSearchHook2 methods ***
    STDMETHOD(TranslateWithSearchContext)(THIS_ LPWSTR lpwszSearchURL, DWORD cchBufferSize, ISearchContext * pSearchContext) = 0;
};

//===========================================================================
//
// INewShortcutHook Interface
//
//===========================================================================

#undef  INTERFACE
#define INTERFACE   INewShortcutHookA

DECLARE_INTERFACE_(INewShortcutHookA, IUnknown) // nshhk
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG, Release) (THIS) = 0;

    // *** INewShortcutHook methods ***
    STDMETHOD(SetReferent)(THIS_ LPCSTR pcszReferent, HWND hwnd) = 0;
    STDMETHOD(GetReferent)(THIS_ LPSTR pszReferent, int cchReferent) = 0;
    STDMETHOD(SetFolder)(THIS_ LPCSTR pcszFolder) = 0;
    STDMETHOD(GetFolder)(THIS_ LPSTR pszFolder, int cchFolder) = 0;
    STDMETHOD(GetName)(THIS_ LPSTR pszName, int cchName) = 0;
    STDMETHOD(GetExtension)(THIS_ LPSTR pszExtension, int cchExtension) = 0;
};

#undef  INTERFACE
#define INTERFACE   INewShortcutHookW

DECLARE_INTERFACE_(INewShortcutHookW, IUnknown) // nshhk
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG, Release) (THIS) = 0;

    // *** INewShortcutHook methods ***
    STDMETHOD(SetReferent)(THIS_ LPCWSTR pcszReferent, HWND hwnd) = 0;
    STDMETHOD(GetReferent)(THIS_ LPWSTR pszReferent, int cchReferent) = 0;
    STDMETHOD(SetFolder)(THIS_ LPCWSTR pcszFolder) = 0;
    STDMETHOD(GetFolder)(THIS_ LPWSTR pszFolder, int cchFolder) = 0;
    STDMETHOD(GetName)(THIS_ LPWSTR pszName, int cchName) = 0;
    STDMETHOD(GetExtension)(THIS_ LPWSTR pszExtension, int cchExtension) = 0;
};

#ifdef UNICODE
#define INewShortcutHook        INewShortcutHookW
#define INewShortcutHookVtbl    INewShortcutHookWVtbl
#else
#define INewShortcutHook        INewShortcutHookA
#define INewShortcutHookVtbl    INewShortcutHookAVtbl
#endif

//===========================================================================
//
// ICopyHook Interface
//
//  The copy hook is called whenever file system directories are
//  copy/moved/deleted/renamed via the shell.  It is also called by the shell
//  on changes of status of printers.
//
//  Clients register their id under STRREG_SHEX_COPYHOOK for file system hooks
//  and STRREG_SHEx_PRNCOPYHOOK for printer hooks.
//  the CopyCallback is called prior to the action, so the hook has the chance
//  to allow, deny or cancel the operation by returning the falues:
//     IDYES  -  means allow the operation
//     IDNO   -  means disallow the operation on this file, but continue with
//              any other operations (eg. batch copy)
//     IDCANCEL - means disallow the current operation and cancel any pending
//              operations
//
//   arguments to the CopyCallback
//      hwnd - window to use for any UI
//      wFunc - what operation is being done
//      wFlags - and flags (FOF_*) set in the initial call to the file operation
//      pszSrcFile - name of the source file
//      dwSrcAttribs - file attributes of the source file
//      pszDestFile - name of the destiation file (for move and renames)
//      dwDestAttribs - file attributes of the destination file
//
//
//===========================================================================

#ifndef FO_MOVE //these need to be kept in sync with the ones in shellapi.h

// file operations

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
#if (_WIN32_IE >= 0x500)
#define FOF_NO_CONNECTED_ELEMENTS  0x2000  // don't operate on connected file elements.
#define FOF_WANTNUKEWARNING        0x4000  // during delete operation, warn if nuking instead of recycling (partially overrides FOF_NOCONFIRMATION)
#endif // _WIN32_IE >= 0x500
#if (_WIN32_WINNT >= 0x0501)
#define FOF_NORECURSEREPARSE       0x8000  // treat reparse points as objects, not containers
#endif // (_WIN32_WINNT >= 0x501)

typedef WORD FILEOP_FLAGS;

// printer operations

#define PO_DELETE       0x0013  // printer is being deleted
#define PO_RENAME       0x0014  // printer is being renamed
#define PO_PORTCHANGE   0x0020  // port this printer connected to is being changed
                                // if this id is set, the strings received by
                                // the copyhook are a doubly-null terminated
                                // list of strings.  The first is the printer
                                // name and the second is the printer port.
#define PO_REN_PORT     0x0034  // PO_RENAME and PO_PORTCHANGE at same time.

// no POF_ flags currently defined

typedef UINT PRINTEROP_FLAGS;

#endif // FO_MOVE

#undef  INTERFACE
#define INTERFACE   ICopyHookA

DECLARE_INTERFACE_(ICopyHookA, IUnknown)        // sl
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** ICopyHook methods ***
    STDMETHOD_(UINT,CopyCallback) (THIS_ HWND hwnd, UINT wFunc, UINT wFlags, LPCSTR pszSrcFile, DWORD dwSrcAttribs,
                                   LPCSTR pszDestFile, DWORD dwDestAttribs) = 0;
};

typedef ICopyHookA *    LPCOPYHOOKA;

#undef  INTERFACE
#define INTERFACE   ICopyHookW

DECLARE_INTERFACE_(ICopyHookW, IUnknown)        // sl
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** ICopyHook methods ***
    STDMETHOD_(UINT,CopyCallback) (THIS_ HWND hwnd, UINT wFunc, UINT wFlags, LPCWSTR pszSrcFile, DWORD dwSrcAttribs,
                                   LPCWSTR pszDestFile, DWORD dwDestAttribs) = 0;
};

typedef ICopyHookW *    LPCOPYHOOKW;

#ifdef UNICODE
#define ICopyHook       ICopyHookW
#define ICopyHookVtbl   ICopyHookWVtbl
#define LPCOPYHOOK      LPCOPYHOOKW
#else
#define ICopyHook       ICopyHookA
#define ICopyHookVtbl   ICopyHookAVtbl
#define LPCOPYHOOK      LPCOPYHOOKA
#endif

//===========================================================================
//
// IFileViewerSite Interface
//
//===========================================================================

#undef  INTERFACE
#define INTERFACE   IFileViewerSite

DECLARE_INTERFACE_(IFileViewerSite, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IFileViewerSite methods ***
    STDMETHOD(SetPinnedWindow) (THIS_ HWND hwnd) = 0;
    STDMETHOD(GetPinnedWindow) (THIS_ HWND *phwnd) = 0;
};

typedef IFileViewerSite * LPFILEVIEWERSITE;


//===========================================================================
//
// IFileViewer Interface
//
// Implemented in a FileViewer component object.  Used to tell a
// FileViewer to PrintTo or to view, the latter happening though
// ShowInitialize and Show.  The filename is always given to the
// viewer through IPersistFile.
//
//===========================================================================

#include <pshpack8.h>

typedef struct
{
    // Stuff passed into viewer (in)
    DWORD cbSize;           // Size of structure for future expansion...
    HWND hwndOwner;         // who is the owner window.
    int iShow;              // The show command

    // Passed in and updated  (in/Out)
    DWORD dwFlags;          // flags
    RECT rect;              // Where to create the window may have defaults
    IUnknown *punkRel;      // Relese this interface when window is visible

    // Stuff that might be returned from viewer (out)
    OLECHAR strNewFile[MAX_PATH];   // New File to view.

} FVSHOWINFO, *LPFVSHOWINFO;

#include <poppack.h>        /* Return to byte packing */

    // Define File View Show Info Flags.
#define FVSIF_RECT      0x00000001      // The rect variable has valid data.
#define FVSIF_PINNED    0x00000002      // We should Initialize pinned

#define FVSIF_NEWFAILED 0x08000000      // The new file passed back failed
                                        // to be viewed.

#define FVSIF_NEWFILE   0x80000000      // A new file to view has been returned
#define FVSIF_CANVIEWIT 0x40000000      // The viewer can view it.

#undef  INTERFACE
#define INTERFACE   IFileViewerA

DECLARE_INTERFACE(IFileViewerA)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IFileViewer methods ***
    STDMETHOD(ShowInitialize) (THIS_ LPFILEVIEWERSITE lpfsi) = 0;
    STDMETHOD(Show) (THIS_ LPFVSHOWINFO pvsi) = 0;
    STDMETHOD(PrintTo) (THIS_ LPSTR pszDriver, BOOL fSuppressUI) = 0;
};

typedef IFileViewerA * LPFILEVIEWERA;

#undef  INTERFACE
#define INTERFACE   IFileViewerW

DECLARE_INTERFACE(IFileViewerW)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IFileViewer methods ***
    STDMETHOD(ShowInitialize) (THIS_ LPFILEVIEWERSITE lpfsi) = 0;
    STDMETHOD(Show) (THIS_ LPFVSHOWINFO pvsi) = 0;
    STDMETHOD(PrintTo) (THIS_ LPWSTR pszDriver, BOOL fSuppressUI) = 0;
};

typedef IFileViewerW * LPFILEVIEWERW;

#ifdef UNICODE
#define IFileViewer IFileViewerW
#define LPFILEVIEWER LPFILEVIEWERW
#else
#define IFileViewer IFileViewerA
#define LPFILEVIEWER LPFILEVIEWERA
#endif



//==========================================================================
//
// IShellBrowser/IShellView/IShellFolder interface
//
//  These three interfaces are used when the shell communicates with
// name space extensions. The shell (explorer) provides IShellBrowser
// interface, and extensions implements IShellFolder and IShellView
// interfaces.
//
//==========================================================================


//--------------------------------------------------------------------------
//
// Command/menuitem IDs
//
//  The explorer dispatches WM_COMMAND messages based on the range of
// command/menuitem IDs. All the IDs of menuitems that the view (right
// pane) inserts must be in FCIDM_SHVIEWFIRST/LAST (otherwise, the explorer
// won't dispatch them). The view should not deal with any menuitems
// in FCIDM_BROWSERFIRST/LAST (otherwise, it won't work with the future
// version of the shell).
//
//  FCIDM_SHVIEWFIRST/LAST      for the right pane (IShellView)
//  FCIDM_BROWSERFIRST/LAST     for the explorer frame (IShellBrowser)
//  FCIDM_GLOBAL/LAST           for the explorer's submenu IDs
//
//--------------------------------------------------------------------------

#define FCIDM_SHVIEWFIRST           0x0000
#define FCIDM_SHVIEWLAST            0x7fff
#define FCIDM_BROWSERFIRST          0xa000
#define FCIDM_BROWSERLAST           0xbf00
#define FCIDM_GLOBALFIRST           0x8000
#define FCIDM_GLOBALLAST            0x9fff

//
// Global submenu IDs and separator IDs
//
#define FCIDM_MENU_FILE             (FCIDM_GLOBALFIRST+0x0000)
#define FCIDM_MENU_EDIT             (FCIDM_GLOBALFIRST+0x0040)
#define FCIDM_MENU_VIEW             (FCIDM_GLOBALFIRST+0x0080)
#define FCIDM_MENU_VIEW_SEP_OPTIONS (FCIDM_GLOBALFIRST+0x0081)
#define FCIDM_MENU_TOOLS            (FCIDM_GLOBALFIRST+0x00c0) // for Win9x compat
#define FCIDM_MENU_TOOLS_SEP_GOTO   (FCIDM_GLOBALFIRST+0x00c1) // for Win9x compat
#define FCIDM_MENU_HELP             (FCIDM_GLOBALFIRST+0x0100)
#define FCIDM_MENU_FIND             (FCIDM_GLOBALFIRST+0x0140)
#define FCIDM_MENU_EXPLORE          (FCIDM_GLOBALFIRST+0x0150)
#define FCIDM_MENU_FAVORITES        (FCIDM_GLOBALFIRST+0x0170)

//--------------------------------------------------------------------------
// control IDs known to the view
//--------------------------------------------------------------------------

#define FCIDM_TOOLBAR      (FCIDM_BROWSERFIRST + 0)
#define FCIDM_STATUS       (FCIDM_BROWSERFIRST + 1)

#if (_WIN32_IE >= 0x0400)
//--------------------------------------------------------------------------
//
// The resource id of the offline cursor
// This cursor is avaialble in shdocvw.dll
#define IDC_OFFLINE_HAND        103
//
//--------------------------------------------------------------------------
#endif


// SBCMDID_GETPANE - not necessarily in order
#define PANE_NONE        -1
#define PANE_ZONE        1
#define PANE_OFFLINE     2
#define PANE_PRINTER     3
#define PANE_SSL         4
#define PANE_NAVIGATION  5
#define PANE_PROGRESS    6
#define PANE_PRIVACY     7


//-------------------------------------------------------------------------
// ICommDlgBrowser interface
//
//  ICommDlgBrowser interface is the interface that is provided by the new
// common dialog window to hook and modify the behavior of IShellView.  When
// a default view is created, it queries its parent IShellBrowser for the
// ICommDlgBrowser interface.  If supported, it calls out to that interface
// in several cases that need to behave differently in a dialog.
//
// Member functions:
//
//  ICommDlgBrowser::OnDefaultCommand()
//    Called when the user double-clicks in the view or presses Enter.  The
//   browser should return S_OK if it processed the action itself, S_FALSE
//   to let the view perform the default action.
//
//  ICommDlgBrowser::OnStateChange(ULONG uChange)
//    Called when some states in the view change.  'uChange' is one of the
//   CDBOSC_* values.  This call is made after the state (selection, focus,
//   etc) has changed.  There is no return value.
//
//  ICommDlgBrowser::IncludeObject(LPCITEMIDLIST pidl)
//    Called when the view is enumerating objects.  'pidl' is a relative
//   IDLIST.  The browser should return S_OK to include the object in the
//   view, S_FALSE to hide it
//
//-------------------------------------------------------------------------

#define CDBOSC_SETFOCUS     0x00000000
#define CDBOSC_KILLFOCUS    0x00000001
#define CDBOSC_SELCHANGE    0x00000002
#define CDBOSC_RENAME       0x00000003
#define CDBOSC_STATECHANGE  0x00000004

#undef  INTERFACE
#define INTERFACE   ICommDlgBrowser

DECLARE_INTERFACE_(ICommDlgBrowser, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** ICommDlgBrowser methods ***
    STDMETHOD(OnDefaultCommand) (THIS_ struct IShellView * ppshv) = 0;
    STDMETHOD(OnStateChange) (THIS_ struct IShellView * ppshv,
                ULONG uChange) = 0;
    STDMETHOD(IncludeObject) (THIS_ struct IShellView * ppshv,
                LPCITEMIDLIST pidl) = 0;
};

typedef ICommDlgBrowser * LPCOMMDLGBROWSER;

//-------------------------------------------------------------------------
// ICommDlgBrowser2 interface
//
// Member functions:
//
//  ICommDlgBrowser2::Notify(IShellView *pshv, DWORD dwNotfyType)
//   Called when the view is wants to notify common dialog when an event
//  occurrs.
//
//  CDB2N_CONTEXTMENU_START indicates the context menu has started.
//  CDB2N_CONTEXTMENU_DONE  indicates the context menu has completed.
//
//  ICommDlgBrowser2::GetDefaultMenuText(IShellView *pshv,
//                                      WCHAR *pszText, INT cchMax)
//   Called when the view wants to get the default context menu text.
//  pszText points to buffer and cchMax specifies the size of the
//  buffer in characters.  The browser on return has filled the buffer
//  with the default context menu text.  The Shell will call this method
//  with at least a buffer size of MAX_PATH.  The browser should return
//  S_OK if it returned a new default menu text, S_FALSE to let the view
//  to use the normal default menu text.
//
//  ICommDlgBrowser2::GetViewFlags(DWORD *pdwFlags)
//     Called when the view wants to determine  if special customization needs to
//    be done for the common dialog browser. For example View calls this function to
//    determin if all files(hidden and system)needs to be shown. If the GetViewFlags returns a DWORD with
//    CDB2GVF_SHOWALLFILES  flag set then it will show all the files.
//-------------------------------------------------------------------------

#define CDB2N_CONTEXTMENU_DONE  0x00000001
#define CDB2N_CONTEXTMENU_START 0x00000002

//GetViewFlags
#define CDB2GVF_SHOWALLFILES        0x00000001

#undef  INTERFACE
#define INTERFACE   ICommDlgBrowser2

DECLARE_INTERFACE_(ICommDlgBrowser2, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** ICommDlgBrowser methods ***
    STDMETHOD(OnDefaultCommand) (THIS_ struct IShellView * ppshv) = 0;
    STDMETHOD(OnStateChange) (THIS_ struct IShellView * ppshv,
                ULONG uChange) = 0;
    STDMETHOD(IncludeObject) (THIS_ struct IShellView * ppshv,
                LPCITEMIDLIST pidl) = 0;

    // *** ICommDlgBrowser2 methods ***
    STDMETHOD(Notify) (THIS_ struct IShellView * ppshv,
                DWORD dwNotifyType) = 0;
    STDMETHOD(GetDefaultMenuText) (THIS_ struct IShellView * ppshv,
                WCHAR *pszText, INT cchMax) = 0;
    STDMETHOD(GetViewFlags)(THIS_ DWORD *pdwFlags) = 0;

};

typedef ICommDlgBrowser2 * LPCOMMDLGBROWSER2;



//
// function assumes the size of the buffer (MAX_PATH). The pidl
// should point to a file system object.

SHSTDAPI_(BOOL) SHGetPathFromIDListA(LPCITEMIDLIST pidl, LPSTR pszPath);
SHSTDAPI_(BOOL) SHGetPathFromIDListW(LPCITEMIDLIST pidl, LPWSTR pszPath);
#ifdef UNICODE
#define SHGetPathFromIDList  SHGetPathFromIDListW
#else
#define SHGetPathFromIDList  SHGetPathFromIDListA
#endif // !UNICODE

#if (_WIN32_IE >= 0x0601)
SHSTDAPI_(int) SHCreateDirectory(HWND hwnd, LPCWSTR pszPath);
#endif  // (_WIN32_IE >= 0x0601)
SHSTDAPI_(int) SHCreateDirectoryExA(HWND hwnd, LPCSTR pszPath, SECURITY_ATTRIBUTES *psa);
SHSTDAPI_(int) SHCreateDirectoryExW(HWND hwnd, LPCWSTR pszPath, SECURITY_ATTRIBUTES *psa);
#ifdef UNICODE
#define SHCreateDirectoryEx  SHCreateDirectoryExW
#else
#define SHCreateDirectoryEx  SHCreateDirectoryExA
#endif // !UNICODE

SHSTDAPI SHOpenFolderAndSelectItems(LPCITEMIDLIST pidlFolder, UINT cidl, LPCITEMIDLIST *apidl, DWORD dwFlags);

#if (_WIN32_IE >= 0x0601)
SHSTDAPI SHCreateShellItem(LPCITEMIDLIST pidlParent, IShellFolder *psfParent, LPCITEMIDLIST pidl, IShellItem **ppsi);
#endif  // (_WIN32_IE >= 0x0601)


//-------------------------------------------------------------------------
//
// SHGetSpecialFolderLocation
//
//  Caller should use SHGetMalloc to obtain an allocator that can free the pidl
//
//
//-------------------------------------------------------------------------
//
// registry entries for special paths are kept in :
#define REGSTR_PATH_SPECIAL_FOLDERS    REGSTR_PATH_EXPLORER TEXT("\\Shell Folders")


#define CSIDL_DESKTOP                   0x0000        // <desktop>
#define CSIDL_INTERNET                  0x0001        // Internet Explorer (icon on desktop)
#define CSIDL_PROGRAMS                  0x0002        // Start Menu\Programs
#define CSIDL_CONTROLS                  0x0003        // My Computer\Control Panel
#define CSIDL_PRINTERS                  0x0004        // My Computer\Printers
#define CSIDL_PERSONAL                  0x0005        // My Documents
#define CSIDL_FAVORITES                 0x0006        // <user name>\Favorites
#define CSIDL_STARTUP                   0x0007        // Start Menu\Programs\Startup
#define CSIDL_RECENT                    0x0008        // <user name>\Recent
#define CSIDL_SENDTO                    0x0009        // <user name>\SendTo
#define CSIDL_BITBUCKET                 0x000a        // <desktop>\Recycle Bin
#define CSIDL_STARTMENU                 0x000b        // <user name>\Start Menu
#define CSIDL_MYDOCUMENTS               0x000c        // logical "My Documents" desktop icon
#define CSIDL_MYMUSIC                   0x000d        // "My Music" folder
#define CSIDL_MYVIDEO                   0x000e        // "My Videos" folder
#define CSIDL_DESKTOPDIRECTORY          0x0010        // <user name>\Desktop
#define CSIDL_DRIVES                    0x0011        // My Computer
#define CSIDL_NETWORK                   0x0012        // Network Neighborhood (My Network Places)
#define CSIDL_NETHOOD                   0x0013        // <user name>\nethood
#define CSIDL_FONTS                     0x0014        // windows\fonts
#define CSIDL_TEMPLATES                 0x0015
#define CSIDL_COMMON_STARTMENU          0x0016        // All Users\Start Menu
#define CSIDL_COMMON_PROGRAMS           0X0017        // All Users\Start Menu\Programs
#define CSIDL_COMMON_STARTUP            0x0018        // All Users\Startup
#define CSIDL_COMMON_DESKTOPDIRECTORY   0x0019        // All Users\Desktop
#define CSIDL_APPDATA                   0x001a        // <user name>\Application Data
#define CSIDL_PRINTHOOD                 0x001b        // <user name>\PrintHood

#ifndef CSIDL_LOCAL_APPDATA
#define CSIDL_LOCAL_APPDATA             0x001c        // <user name>\Local Settings\Applicaiton Data (non roaming)
#endif // CSIDL_LOCAL_APPDATA

#define CSIDL_ALTSTARTUP                0x001d        // non localized startup
#define CSIDL_COMMON_ALTSTARTUP         0x001e        // non localized common startup
#define CSIDL_COMMON_FAVORITES          0x001f

#ifndef _SHFOLDER_H_
#define CSIDL_INTERNET_CACHE            0x0020
#define CSIDL_COOKIES                   0x0021
#define CSIDL_HISTORY                   0x0022
#define CSIDL_COMMON_APPDATA            0x0023        // All Users\Application Data
#define CSIDL_WINDOWS                   0x0024        // GetWindowsDirectory()
#define CSIDL_SYSTEM                    0x0025        // GetSystemDirectory()
#define CSIDL_PROGRAM_FILES             0x0026        // C:\Program Files
#define CSIDL_MYPICTURES                0x0027        // C:\Program Files\My Pictures
#endif // _SHFOLDER_H_

#define CSIDL_PROFILE                   0x0028        // USERPROFILE
#define CSIDL_SYSTEMX86                 0x0029        // x86 system directory on RISC
#define CSIDL_PROGRAM_FILESX86          0x002a        // x86 C:\Program Files on RISC

#ifndef _SHFOLDER_H_
#define CSIDL_PROGRAM_FILES_COMMON      0x002b        // C:\Program Files\Common
#endif // _SHFOLDER_H_

#define CSIDL_PROGRAM_FILES_COMMONX86   0x002c        // x86 Program Files\Common on RISC
#define CSIDL_COMMON_TEMPLATES          0x002d        // All Users\Templates

#ifndef _SHFOLDER_H_
#define CSIDL_COMMON_DOCUMENTS          0x002e        // All Users\Documents
#define CSIDL_COMMON_ADMINTOOLS         0x002f        // All Users\Start Menu\Programs\Administrative Tools
#define CSIDL_ADMINTOOLS                0x0030        // <user name>\Start Menu\Programs\Administrative Tools
#endif // _SHFOLDER_H_

#define CSIDL_CONNECTIONS               0x0031        // Network and Dial-up Connections
#define CSIDL_COMMON_MUSIC              0x0035        // All Users\My Music
#define CSIDL_COMMON_PICTURES           0x0036        // All Users\My Pictures
#define CSIDL_COMMON_VIDEO              0x0037        // All Users\My Video
#define CSIDL_RESOURCES                 0x0038        // Resource Direcotry

#ifndef _SHFOLDER_H_
#define CSIDL_RESOURCES_LOCALIZED       0x0039        // Localized Resource Direcotry
#endif // _SHFOLDER_H_

#define CSIDL_COMMON_OEM_LINKS          0x003a        // Links to All Users OEM specific apps
#define CSIDL_CDBURN_AREA               0x003b        // USERPROFILE\Local Settings\Application Data\Microsoft\CD Burning
// unused                               0x003c
#define CSIDL_COMPUTERSNEARME           0x003d        // Computers Near Me (computered from Workgroup membership)

#ifndef _SHFOLDER_H_
#define CSIDL_FLAG_CREATE               0x8000        // combine with CSIDL_ value to force folder creation in SHGetFolderPath()
#endif // _SHFOLDER_H_

#define CSIDL_FLAG_DONT_VERIFY          0x4000        // combine with CSIDL_ value to return an unverified folder path
#define CSIDL_FLAG_NO_ALIAS             0x1000        // combine with CSIDL_ value to insure non-alias versions of the pidl
#define CSIDL_FLAG_PER_USER_INIT        0x0800        // combine with CSIDL_ value to indicate per-user init (eg. upgrade)
#define CSIDL_FLAG_MASK                 0xFF00        // mask for all possible flag values


SHSTDAPI SHGetSpecialFolderLocation(HWND hwnd, int csidl, LPITEMIDLIST *ppidl);

#if (_WIN32_IE >= 0x0400)
SHSTDAPI_(void) SHFlushSFCache(void);   
SHSTDAPI_(LPITEMIDLIST) SHCloneSpecialIDList(HWND hwnd, int csidl, BOOL fCreate);

SHSTDAPI_(BOOL) SHGetSpecialFolderPathA(HWND hwnd, LPSTR pszPath, int csidl, BOOL fCreate);
SHSTDAPI_(BOOL) SHGetSpecialFolderPathW(HWND hwnd, LPWSTR pszPath, int csidl, BOOL fCreate);
#ifdef UNICODE
#define SHGetSpecialFolderPath  SHGetSpecialFolderPathW
#else
#define SHGetSpecialFolderPath  SHGetSpecialFolderPathA
#endif // !UNICODE

#if (_WIN32_IE >= 0x0500)

typedef enum {
    SHGFP_TYPE_CURRENT  = 0,   // current value for user, verify it exists
    SHGFP_TYPE_DEFAULT  = 1,   // default value, may not exist
} SHGFP_TYPE;

SHFOLDERAPI SHGetFolderPathA(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPSTR pszPath);
SHFOLDERAPI SHGetFolderPathW(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPWSTR pszPath);
#ifdef UNICODE
#define SHGetFolderPath  SHGetFolderPathW
#else
#define SHGetFolderPath  SHGetFolderPathA
#endif // !UNICODE
SHSTDAPI SHGetFolderLocation(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPITEMIDLIST *ppidl);

SHFOLDERAPI SHGetFolderPathAndSubDirA(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPCSTR pszSubDir, LPSTR pszPath);
SHFOLDERAPI SHGetFolderPathAndSubDirW(HWND hwnd, int csidl, HANDLE hToken, DWORD dwFlags, LPCWSTR pszSubDir, LPWSTR pszPath);
#ifdef UNICODE
#define SHGetFolderPathAndSubDir  SHGetFolderPathAndSubDirW
#else
#define SHGetFolderPathAndSubDir  SHGetFolderPathAndSubDirA
#endif // !UNICODE

#endif      // _WIN32_IE >= 0x0500

#endif      // _WIN32_IE >= 0x0400


#if (_WIN32_IE >= 0x0500)

#define FCS_READ                    0x00000001
#define FCS_FORCEWRITE              0x00000002
#define FCS_WRITE                   (FCS_READ | FCS_FORCEWRITE)

#define FCS_FLAG_DRAGDROP           2

// Mask which values have been retreived or being set.
#define FCSM_VIEWID                 0x00000001
#define FCSM_WEBVIEWTEMPLATE        0x00000002
#define FCSM_INFOTIP                0x00000004
#define FCSM_CLSID                  0x00000008
#define FCSM_ICONFILE               0x00000010
#define FCSM_LOGO                   0x00000020
#define FCSM_FLAGS                  0x00000040

#include <pshpack8.h>

// Used by SHGetSetFolderCustomSettingsA
typedef struct
{
    DWORD           dwSize;
    DWORD           dwMask;             // IN/OUT   Which Attributes to Get/Set
    SHELLVIEWID*    pvid;               // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    // The folder's WebView template path
    LPSTR           pszWebViewTemplate;  // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    DWORD           cchWebViewTemplate;  // IN - Specifies the size of the buffer pointed to by pszWebViewTemplate
                                        // Ignored if dwReadWrite is FCS_READ
    LPSTR           pszWebViewTemplateVersion;  // currently IN only
    // Infotip for the folder
    LPSTR           pszInfoTip;         // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    DWORD           cchInfoTip;         // IN - Specifies the size of the buffer pointed to by pszInfoTip
                                        // Ignored if dwReadWrite is FCS_READ
    // CLSID that points to more info in the registry
    CLSID*          pclsid;             // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    // Other flags for the folder. Takes FCS_FLAG_* values
    DWORD           dwFlags;            // OUT - if dwReadWrite is FCS_READ, IN - otherwise

    LPSTR           pszIconFile;        // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    DWORD           cchIconFile;        // IN - Specifies the size of the buffer pointed to by pszIconFile
                                        // Ignored if dwReadWrite is FCS_READ

    int             iIconIndex;         // OUT - if dwReadWrite is FCS_READ, IN - otherwise

    LPSTR           pszLogo;        // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    DWORD           cchLogo;        // IN - Specifies the size of the buffer pointed to by pszIconFile
                                        // Ignored if dwReadWrite is FCS_READ


} SHFOLDERCUSTOMSETTINGSA, *LPSHFOLDERCUSTOMSETTINGSA;

// Used by SHGetSetFolderCustomSettingsW
typedef struct
{
    DWORD           dwSize;
    DWORD           dwMask;              // IN/OUT  Which Attributes to Get/Set
    SHELLVIEWID*    pvid;                // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    // The folder's WebView template path
    LPWSTR          pszWebViewTemplate;   // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    DWORD           cchWebViewTemplate;   // IN - Specifies the size of the buffer pointed to by pszWebViewTemplate
                                         // Ignored if dwReadWrite is FCS_READ
    LPWSTR           pszWebViewTemplateVersion;  // currently IN only
    // Infotip for the folder
    LPWSTR          pszInfoTip;          // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    DWORD           cchInfoTip;          // IN - Specifies the size of the buffer pointed to by pszInfoTip
                                         // Ignored if dwReadWrite is FCS_READ
    // CLSID that points to more info in the registry
    CLSID*          pclsid;              // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    // Other flags for the folder. Takes FCS_FLAG_* values
    DWORD           dwFlags;             // OUT - if dwReadWrite is FCS_READ, IN - otherwise


    LPWSTR           pszIconFile;        // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    DWORD            cchIconFile;        // IN - Specifies the size of the buffer pointed to by pszIconFile
                                         // Ignored if dwReadWrite is FCS_READ

    int              iIconIndex;         // OUT - if dwReadWrite is FCS_READ, IN - otherwise

    LPWSTR           pszLogo;        // OUT - if dwReadWrite is FCS_READ, IN - otherwise
    DWORD            cchLogo;        // IN - Specifies the size of the buffer pointed to by pszIconFile
                                         // Ignored if dwReadWrite is FCS_READ
} SHFOLDERCUSTOMSETTINGSW, *LPSHFOLDERCUSTOMSETTINGSW;

#include <poppack.h>        /* Return to byte packing */

// Gets/Sets the Folder Custom Settings for pszPath based on dwReadWrite. dwReadWrite can be FCS_READ/FCS_WRITE/FCS_FORCEWRITE
SHSTDAPI SHGetSetFolderCustomSettingsA(LPSHFOLDERCUSTOMSETTINGSA pfcs, LPCSTR pszPath, DWORD dwReadWrite);
SHSTDAPI SHGetSetFolderCustomSettingsW(LPSHFOLDERCUSTOMSETTINGSW pfcs, LPCWSTR pszPath, DWORD dwReadWrite);

#ifdef UNICODE
#define SHFOLDERCUSTOMSETTINGS          SHFOLDERCUSTOMSETTINGSW
#define SHGetSetFolderCustomSettings    SHGetSetFolderCustomSettingsW
#define LPSHFOLDERCUSTOMSETTINGS        LPSHFOLDERCUSTOMSETTINGSW
#else
#define SHFOLDERCUSTOMSETTINGS          SHFOLDERCUSTOMSETTINGSA
#define SHGetSetFolderCustomSettings    SHGetSetFolderCustomSettingsA
#define LPSHFOLDERCUSTOMSETTINGS        LPSHFOLDERCUSTOMSETTINGSA
#endif

#endif  // _WIN32_IE >= 0x0500

//-------------------------------------------------------------------------
//
// SHBrowseForFolder API
//
//
//-------------------------------------------------------------------------

typedef int (CALLBACK* BFFCALLBACK)(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);

#include <pshpack8.h>

typedef struct _browseinfoA {
    HWND        hwndOwner;
    LPCITEMIDLIST pidlRoot;
    LPSTR        pszDisplayName;        // Return display name of item selected.
    LPCSTR       lpszTitle;                     // text to go in the banner over the tree.
    UINT         ulFlags;                       // Flags that control the return stuff
    BFFCALLBACK  lpfn;
    LPARAM       lParam;                        // extra info that's passed back in callbacks
    int          iImage;                        // output var: where to return the Image index.
} BROWSEINFOA, *PBROWSEINFOA, *LPBROWSEINFOA;

typedef struct _browseinfoW {
    HWND        hwndOwner;
    LPCITEMIDLIST pidlRoot;
    LPWSTR       pszDisplayName;        // Return display name of item selected.
    LPCWSTR      lpszTitle;                     // text to go in the banner over the tree.
    UINT         ulFlags;                       // Flags that control the return stuff
    BFFCALLBACK  lpfn;
    LPARAM       lParam;                        // extra info that's passed back in callbacks
    int          iImage;                        // output var: where to return the Image index.
} BROWSEINFOW, *PBROWSEINFOW, *LPBROWSEINFOW;

#include <poppack.h>        /* Return to byte packing */

#ifdef UNICODE
#define BROWSEINFO      BROWSEINFOW
#define PBROWSEINFO     PBROWSEINFOW
#define LPBROWSEINFO    LPBROWSEINFOW
#else
#define BROWSEINFO      BROWSEINFOA
#define PBROWSEINFO     PBROWSEINFOA
#define LPBROWSEINFO    LPBROWSEINFOA
#endif

// Browsing for directory.
#define BIF_RETURNONLYFSDIRS   0x0001  // For finding a folder to start document searching
#define BIF_DONTGOBELOWDOMAIN  0x0002  // For starting the Find Computer
#define BIF_STATUSTEXT         0x0004   // Top of the dialog has 2 lines of text for BROWSEINFO.lpszTitle and one line if
                                        // this flag is set.  Passing the message BFFM_SETSTATUSTEXTA to the hwnd can set the
                                        // rest of the text.  This is not used with BIF_USENEWUI and BROWSEINFO.lpszTitle gets
                                        // all three lines of text.
#define BIF_RETURNFSANCESTORS  0x0008
#define BIF_EDITBOX            0x0010   // Add an editbox to the dialog
#define BIF_VALIDATE           0x0020   // insist on valid result (or CANCEL)

#define BIF_NEWDIALOGSTYLE     0x0040   // Use the new dialog layout with the ability to resize
                                        // Caller needs to call OleInitialize() before using this API

#define BIF_USENEWUI           (BIF_NEWDIALOGSTYLE | BIF_EDITBOX)

#define BIF_BROWSEINCLUDEURLS  0x0080   // Allow URLs to be displayed or entered. (Requires BIF_USENEWUI)
#define BIF_UAHINT             0x0100   // Add a UA hint to the dialog, in place of the edit box. May not be combined with BIF_EDITBOX
#define BIF_NONEWFOLDERBUTTON  0x0200   // Do not add the "New Folder" button to the dialog.  Only applicable with BIF_NEWDIALOGSTYLE.
#define BIF_NOTRANSLATETARGETS 0x0400   // don't traverse target as shortcut

#define BIF_BROWSEFORCOMPUTER  0x1000  // Browsing for Computers.
#define BIF_BROWSEFORPRINTER   0x2000  // Browsing for Printers
#define BIF_BROWSEINCLUDEFILES 0x4000  // Browsing for Everything
#define BIF_SHAREABLE          0x8000  // sharable resources displayed (remote shares, requires BIF_USENEWUI)

// message from browser
#define BFFM_INITIALIZED        1
#define BFFM_SELCHANGED         2
#define BFFM_VALIDATEFAILEDA    3   // lParam:szPath ret:1(cont),0(EndDialog)
#define BFFM_VALIDATEFAILEDW    4   // lParam:wzPath ret:1(cont),0(EndDialog)
#define BFFM_IUNKNOWN           5   // provides IUnknown to client. lParam: IUnknown*

// messages to browser
#define BFFM_SETSTATUSTEXTA     (WM_USER + 100)
#define BFFM_ENABLEOK           (WM_USER + 101)
#define BFFM_SETSELECTIONA      (WM_USER + 102)
#define BFFM_SETSELECTIONW      (WM_USER + 103)
#define BFFM_SETSTATUSTEXTW     (WM_USER + 104)
#define BFFM_SETOKTEXT          (WM_USER + 105) // Unicode only
#define BFFM_SETEXPANDED        (WM_USER + 106) // Unicode only

SHSTDAPI_(LPITEMIDLIST) SHBrowseForFolderA(LPBROWSEINFOA lpbi);
SHSTDAPI_(LPITEMIDLIST) SHBrowseForFolderW(LPBROWSEINFOW lpbi);

#ifdef UNICODE
#define SHBrowseForFolder   SHBrowseForFolderW
#define BFFM_SETSTATUSTEXT  BFFM_SETSTATUSTEXTW
#define BFFM_SETSELECTION   BFFM_SETSELECTIONW

#define BFFM_VALIDATEFAILED BFFM_VALIDATEFAILEDW
#else
#define SHBrowseForFolder   SHBrowseForFolderA
#define BFFM_SETSTATUSTEXT  BFFM_SETSTATUSTEXTA
#define BFFM_SETSELECTION   BFFM_SETSELECTIONA

#define BFFM_VALIDATEFAILED BFFM_VALIDATEFAILEDA
#endif

//-------------------------------------------------------------------------
//
// SHLoadInProc
//
//   When this function is called, the shell calls CoCreateInstance
//  (or equivalent) with CLSCTX_INPROC_SERVER and the specified CLSID
//  from within the shell's process and release it immediately.
//
//-------------------------------------------------------------------------

SHSTDAPI SHLoadInProc(REFCLSID rclsid);


#if (_WIN32_IE >= 0x0600)

//-------------------------------------------------------------------------
//
// SHEnableServiceObject
//
//   Like SHLoadInProc, but gives control over the object's lifetime
//  via fEnable parameter.  TRUE tells the shell to create the object
//  and hold onto it, FALSE tells the shell to look for the previously
//  created instance of the object and release it.
//
//-------------------------------------------------------------------------

SHSTDAPI SHEnableServiceObject(REFCLSID rclsid, BOOL fEnable);

#endif


//-------------------------------------------------------------------------
//
// Internet Shortcut Object
//
//-------------------------------------------------------------------------
// Cmds for CGID_ShortCut
enum {
    ISHCUTCMDID_DOWNLOADICON  = 0,
    ISHCUTCMDID_INTSHORTCUTCREATE = 1,
};
#define CMDID_INTSHORTCUTCREATE ISHCUTCMDID_INTSHORTCUTCREATE



//
//  Helper function which returns a IShellFolder interface to the desktop
// folder. This is equivalent to call CoCreateInstance with CLSID_ShellDesktop.
//
//  CoCreateInstance(CLSID_Desktop, NULL,
//                   CLSCTX_INPROC, IID_IShellFolder, &pshf);
//
SHSTDAPI SHGetDesktopFolder(IShellFolder **ppshf);


// IShellFolder IBindCtx* parameters. the IUnknown for these are
// accessed through IBindCtx::RegisterObjectParam/GetObjectParam
// use this to provide the data needed create IDLists through
// IShellFolder::ParseDisplayName(). this data applies to the last element
// of the name that is parsed (c:\foo\bar.txt, data applies to bar.txt)
// this makes creating these IDLists much faster that suppling the name only

#define STR_FILE_SYS_BIND_DATA      L"File System Bind Data"

#undef  INTERFACE
#define INTERFACE   IFileSystemBindData

DECLARE_INTERFACE_(IFileSystemBindData, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IFileSystemBindData methods ***
    STDMETHOD(SetFindData)(THIS_ const WIN32_FIND_DATAW *pfd) = 0;
    STDMETHOD(GetFindData)(THIS_ WIN32_FIND_DATAW *pfd) = 0;
};

#undef  INTERFACE
#define INTERFACE   IShellDetails

DECLARE_INTERFACE_(IShellDetails, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IShellDetails methods ***
    STDMETHOD(GetDetailsOf)(THIS_ LPCITEMIDLIST pidl, UINT iColumn, SHELLDETAILS *pDetails) = 0;
    STDMETHOD(ColumnClick)(THIS_ UINT iColumn) = 0;
};



//-------------------------------------------------------------------------
//
// IObjMgr interface
//
//
// [Member functions]
//
// IObjMgr::Append(punk)
//   This function adds an object to the end of a list of objects.
//
// IObjMgr::Remove(punk)
//   This function removes an object from a list of objects.
//
// This is implemented by CLSID_ACLMulti so each AutoComplete List
// (CLSID_ACLHistory, CLSID_ACListISF, CLSID_ACLMRU) can be added.
// CLSID_ACLMulti's IEnumString will then be the union of the results
// from the COM Objects added.
//-------------------------------------------------------------------------

#undef INTERFACE
#define INTERFACE IObjMgr

DECLARE_INTERFACE_(IObjMgr, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef) (THIS) = 0;
    STDMETHOD_(ULONG, Release) (THIS) = 0;

    // *** IObjMgr specific methods ***
    STDMETHOD(Append) (THIS_ IUnknown *punk) = 0;
    STDMETHOD(Remove) (THIS_ IUnknown *punk) = 0;
};


//-------------------------------------------------------------------------
//
// ICurrentWorkingDirectory interface
//
//
// [Member functions]
//
// ICurrentWorkingDirectory::GetDirectory(LPWSTR pwzPath, DWORD cchSize)
//   This function gets the Current Working Directory from a COM object that
//   stores such state.
//
// ICurrentWorkingDirectory::SetDirectory(LPCWSTR pwzPath)
//   This function sets the Current Working Directory of a COM object that
//   stores such state.
//
// This function can be used generically.  One COM object that implements it
// is CLSID_ACListISF so that the AutoComplete engine can complete relative
// paths.  SetDirectory() will set the "Current Working Directory" and
// AutoComplete with then complete both absolute and relative paths.
// For Example, if ::SetDirectory(L"C:\Program Files") is called, then
// the user can AutoComplete "..\winnt".  In order to set the current
// working directory for non-file system paths, "ftp://ftp.microsoft.com/" or
// "Control Panel" for example, use IPersistFolder.
//-------------------------------------------------------------------------

#undef INTERFACE
#define INTERFACE ICurrentWorkingDirectory

DECLARE_INTERFACE_(ICurrentWorkingDirectory, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef) (THIS) = 0;
    STDMETHOD_(ULONG, Release) (THIS) = 0;

    // *** ICurrentWorkingDirectory specific methods ***
    STDMETHOD(GetDirectory) (THIS_ LPWSTR pwzPath, DWORD cchSize) = 0;
    STDMETHOD(SetDirectory) (THIS_ LPCWSTR pwzPath) = 0;
};


//-------------------------------------------------------------------------
//
// IACList interface
//
//
// [Member functions]
//
// IObjMgr::Expand(LPCOLESTR)
//   This function tells an autocomplete list to expand a specific string.
//
// If the user enters a multi-level path, AutoComplete (CLSID_AutoComplete)
// will use this interface to tell the "AutoComplete Lists" where to expand
// the results.
//
// For Example, if the user enters "C:\Program Files\Micros", AutoComplete
// first completely enumerate the "AutoComplete Lists" via IEnumString.  Then it
// will call the "AutoComplete Lists" with IACList::Expand(L"C:\Program Files").
// It will then enumerate the IEnumString interface again to get results in
// that directory.
//-------------------------------------------------------------------------

#undef INTERFACE
#define INTERFACE IACList

DECLARE_INTERFACE_(IACList, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef) (THIS) = 0;
    STDMETHOD_(ULONG, Release) (THIS) = 0;

    // *** IACList specific methods ***
    STDMETHOD(Expand) (THIS_ LPCOLESTR pszExpand) = 0;
};

//-------------------------------------------------------------------------
//
// IACList2 interface
//
// [Description]
//              This interface exists to allow the caller to set filter criteria
// for an AutoComplete List.  AutoComplete Lists generates the list of
// possible AutoComplete completions.  CLSID_ACListISF is one AutoComplete
// List COM object that implements this interface.
//-------------------------------------------------------------------------

#undef INTERFACE
#define INTERFACE IACList2

typedef enum _tagAUTOCOMPLETELISTOPTIONS
{
    ACLO_NONE            = 0,    // don't enumerate anything
    ACLO_CURRENTDIR      = 1,    // enumerate current directory
    ACLO_MYCOMPUTER      = 2,    // enumerate MyComputer
    ACLO_DESKTOP         = 4,    // enumerate Desktop Folder
    ACLO_FAVORITES       = 8,    // enumerate Favorites Folder
    ACLO_FILESYSONLY     = 16,   // enumerate only the file system
    ACLO_FILESYSDIRS     = 32,   // enumerate only the file system dirs, UNC shares, and UNC servers.
} AUTOCOMPLETELISTOPTIONS;

DECLARE_INTERFACE_(IACList2, IACList)
{
    // *** IACList2 specific methods ***
    STDMETHOD(SetOptions)(THIS_ DWORD dwFlag) = 0;
    STDMETHOD(GetOptions)(THIS_ DWORD* pdwFlag) = 0;
};


/*-------------------------------------------------------------------------*\
    INTERFACE: IProgressDialog

    DESCRIPTION:
        CLSID_ProgressDialog/IProgressDialog exist to allow a caller to create
    a progress dialog, set it's title, animation, text lines, progress, and
    it will do all the work of updating on a background thread, being modless,
    handling the user cancelling the operation, and estimating the time remaining
    until the operation completes.

    USAGE:
        This is how the dialog is used during operations that require progress
    and the ability to cancel:
    {
        DWORD dwComplete, dwTotal;
        IProgressDialog * ppd;
        CoCreateInstance(CLSID_ProgressDialog, NULL, CLSCTX_INPROC_SERVER, IID_IProgressDialog, (void **)&ppd);
        ppd->SetTitle(L"My Slow Operation");                                // Set the title of the dialog.
        ppd->SetAnimation(hInstApp, IDA_OPERATION_ANIMATION);               // Set the animation to play.
        ppd->StartProgressDialog(hwndParent, punk, PROGDLG_AUTOTIME, NULL); // Display and enable automatic estimated time remaining.
        ppd->SetCancelMsg(L"Please wait while the current operation is cleaned up", NULL);   // Will only be displayed if Cancel button is pressed.

        dwComplete = 0;
        dwTotal = CalcTotalUnitsToDo();

        // Reset because CalcTotalUnitsToDo() took a long time and the estimated time
        // is based on the time between ::StartProgressDialog() and the first
        // ::SetProgress() call.
        ppd->Timer(PDTIMER_RESET, NULL);

        for (nIndex = 0; nIndex < nTotal; nIndex++)
        {
            if (TRUE == ppd->HasUserCancelled())
                break;

            ppd->SetLine(2, L"I'm processing item n", FALSE, NULL);
            dwComplete += DoSlowOperation();

            ppd->SetProgress(dwCompleted, dwTotal);
        }

        ppd->StopProgressDialog();
        ppd->Release();
    }
\*-------------------------------------------------------------------------*/

// Flags for IProgressDialog::StartProgressDialog() (dwFlags)
#define PROGDLG_NORMAL          0x00000000      // default normal progress dlg behavior
#define PROGDLG_MODAL           0x00000001      // the dialog is modal to its hwndParent (default is modeless)
#define PROGDLG_AUTOTIME        0x00000002      // automatically updates the "Line3" text with the "time remaining" (you cant call SetLine3 if you passs this!)
#define PROGDLG_NOTIME          0x00000004      // we dont show the "time remaining" if this is set. We need this if dwTotal < dwCompleted for sparse files
#define PROGDLG_NOMINIMIZE      0x00000008      // Do not have a minimize button in the caption bar.
#define PROGDLG_NOPROGRESSBAR   0x00000010      // Don't display the progress bar

// Time Actions (dwTimerAction)
#define PDTIMER_RESET       0x00000001       // Reset the timer so the progress will be calculated from now until the first ::SetProgress() is called so
                                             // those this time will correspond to the values passed to ::SetProgress().  Only do this before ::SetProgress() is called.


#undef  INTERFACE
#define INTERFACE   IProgressDialog

DECLARE_INTERFACE_(IProgressDialog, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IProgressDialog specific methods
    STDMETHOD(StartProgressDialog)(THIS_ HWND hwndParent, IUnknown * punkEnableModless, DWORD dwFlags, LPCVOID pvResevered) = 0;
    STDMETHOD(StopProgressDialog)(THIS) = 0;
    STDMETHOD(SetTitle)(THIS_ LPCWSTR pwzTitle) = 0;
    STDMETHOD(SetAnimation)(THIS_ HINSTANCE hInstAnimation, UINT idAnimation) = 0;
    STDMETHOD_(BOOL,HasUserCancelled) (THIS) = 0;
    STDMETHOD(SetProgress)(THIS_ DWORD dwCompleted, DWORD dwTotal) = 0;
    STDMETHOD(SetProgress64)(THIS_ ULONGLONG ullCompleted, ULONGLONG ullTotal) = 0;
    STDMETHOD(SetLine)(THIS_ DWORD dwLineNum, LPCWSTR pwzString, BOOL fCompactPath, LPCVOID pvResevered) = 0;
    STDMETHOD(SetCancelMsg)(THIS_ LPCWSTR pwzCancelMsg, LPCVOID pvResevered) = 0;
    STDMETHOD(Timer)(THIS_ DWORD dwTimerAction, LPCVOID pvResevered) = 0;
};


//==========================================================================
// IInputObjectSite/IInputObject interfaces
//
//  These interfaces allow us (or ISVs) to install/update external Internet
// Toolbar for IE and the shell. The frame will simply get the CLSID from
// registry (to be defined) and CoCreateInstance it.
//
//==========================================================================

//-------------------------------------------------------------------------
//
// IInputObjectSite interface
//
//   A site implements this interface so the object can communicate
// focus change to it.
//
// [Member functions]
//
// IInputObjectSite::OnFocusChangeIS(punkObj, fSetFocus)
//   Object (punkObj) is getting or losing the focus.
//
//-------------------------------------------------------------------------


#undef  INTERFACE
#define INTERFACE   IInputObjectSite

DECLARE_INTERFACE_(IInputObjectSite, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IInputObjectSite specific methods ***
    STDMETHOD(OnFocusChangeIS)(THIS_ IUnknown* punkObj, BOOL fSetFocus) = 0;
};


//-------------------------------------------------------------------------
//
// IInputObject interface
//
//   An object implements this interface so the site can communicate
// activation and accelerator events to it.
//
// [Member functions]
//
// IInputObject::UIActivateIO(fActivate, lpMsg)
//   Activates or deactivates the object.  lpMsg may be NULL.  Returns
//   S_OK if the activation succeeded.
//
// IInputObject::HasFocusIO()
//   Returns S_OK if the object has the focus, S_FALSE if not.
//
// IInputObject::TranslateAcceleratorIO(lpMsg)
//   Allow the object to process the message.  Returns S_OK if the
//   message was processed (eaten).
//
//-------------------------------------------------------------------------


#undef  INTERFACE
#define INTERFACE   IInputObject

DECLARE_INTERFACE_(IInputObject, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IInputObject specific methods ***
    STDMETHOD(UIActivateIO)(THIS_ BOOL fActivate, LPMSG lpMsg) = 0;
    STDMETHOD(HasFocusIO)(THIS) = 0;
    STDMETHOD(TranslateAcceleratorIO)(THIS_ LPMSG lpMsg) = 0;
};


//==========================================================================
// IDockingWindowSite/IDockingWindow/IDockingWindowFrame interfaces
// IInputObjectSite/IInputObject interfaces
//
//  These interfaces allow us (or ISVs) to install/update external Internet
// Toolbar for IE and the shell. The frame will simply get the CLSID from
// registry (to be defined) and CoCreateInstance it.
//
//==========================================================================


//-------------------------------------------------------------------------
//
// IDockingWindowSite interface
//
//   A site implements this interface so the object can negotiate for
// and inquire about real estate on the site.
//
// [Member functions]
//
// IDockingWindowSite::GetBorderDW(punkObj, prcBorder)
//   Site returns the bounding rectangle of the given source object
//   (punkObj).
//
// IDockingWindowSite::RequestBorderSpaceDW(punkObj, pbw)
//   Object requests that the site makes room for it, as specified in
//   *pbw.
//
// IDockingWindowSite::SetBorderSpaceDW(punkObj, pbw)
//   Object requests that the site set the border spacing to the size
//   specified in *pbw.
//
//-------------------------------------------------------------------------


#undef  INTERFACE
#define INTERFACE   IDockingWindowSite

DECLARE_INTERFACE_(IDockingWindowSite, IOleWindow)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IOleWindow methods ***
    STDMETHOD(GetWindow) (THIS_ HWND * lphwnd) = 0;
    STDMETHOD(ContextSensitiveHelp) (THIS_ BOOL fEnterMode) = 0;

    // *** IDockingWindowSite methods ***
    STDMETHOD(GetBorderDW) (THIS_ IUnknown* punkObj, LPRECT prcBorder) = 0;
    STDMETHOD(RequestBorderSpaceDW) (THIS_ IUnknown* punkObj, LPCBORDERWIDTHS pbw) = 0;
    STDMETHOD(SetBorderSpaceDW) (THIS_ IUnknown* punkObj, LPCBORDERWIDTHS pbw) = 0;
};



//-------------------------------------------------------------------------
//
// IDockingWindowFrame interface
//
// [Member functions]
//
// IDockingWindowFrame::AddToolbar(punkSrc, pwszItem, dwReserved)
//
// IDockingWindowFrame::RemoveToolbar(punkSrc, dwRemoveFlags)
//
// IDockingWindowFrame::FindToolbar(pwszItem, riid, ppv)
//
//-------------------------------------------------------------------------


// flags for RemoveToolbar
#define DWFRF_NORMAL            0x0000
#define DWFRF_DELETECONFIGDATA  0x0001


// flags for AddToolbar
#define DWFAF_HIDDEN  0x0001   // add hidden

#undef  INTERFACE
#define INTERFACE   IDockingWindowFrame

DECLARE_INTERFACE_(IDockingWindowFrame, IOleWindow)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IOleWindow methods ***
    STDMETHOD(GetWindow) (THIS_ HWND * lphwnd) = 0;
    STDMETHOD(ContextSensitiveHelp) (THIS_ BOOL fEnterMode) = 0;

    // *** IDockingWindowFrame methods ***
    STDMETHOD(AddToolbar) (THIS_ IUnknown* punkSrc, LPCWSTR pwszItem, DWORD dwAddFlags) = 0;
    STDMETHOD(RemoveToolbar) (THIS_ IUnknown* punkSrc, DWORD dwRemoveFlags) = 0;
    STDMETHOD(FindToolbar) (THIS_ LPCWSTR pwszItem, REFIID riid, void **ppv) = 0;
};



#if (_WIN32_IE >= 0x0400)

//-------------------------------------------------------------------------
//
// IRunnableTask interface
//
//   This is a free threaded interface used for putting items on a background
// scheduler for execution within the view.  It allows a scheduler to start and
// stop tasks on as many worker threads as it deems necessary.
//
// Run(), Kill() and Suspend() may be called from different threads.
//
// [Member functions]
//
// IRunnableTask::Run(void)
//   Initiate the task to run.  This should return E_PENDING if the task
//   has been suspended.
//
// IRunnableTask::Kill(void)
//
// IRunnableTask::Suspend(void)
//
// IRunnableTask::Resume(void)
//
// IRunnableTask::IsRunning(void)
//
//-------------------------------------------------------------------------

// Convenient state values
#define IRTIR_TASK_NOT_RUNNING  0
#define IRTIR_TASK_RUNNING      1
#define IRTIR_TASK_SUSPENDED    2
#define IRTIR_TASK_PENDING      3
#define IRTIR_TASK_FINISHED     4

#undef  INTERFACE
#define INTERFACE   IRunnableTask

DECLARE_INTERFACE_( IRunnableTask, IUnknown )
{
    // *** IUnknown methods ***
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef)(THIS) = 0;
    STDMETHOD_(ULONG, Release)(THIS) = 0;

    // *** IRunnableTask methods ***
    STDMETHOD (Run)(THIS) = 0;
    STDMETHOD (Kill)(THIS_ BOOL fWait ) = 0;
    STDMETHOD (Suspend)(THIS) = 0;
    STDMETHOD (Resume)(THIS) = 0;
    STDMETHOD_(ULONG, IsRunning)(THIS) = 0;
};

typedef IRunnableTask * LPRUNNABLETASK;
#endif


#if (_WIN32_IE >= 0x0400)

#define TASKOWNERID     GUID
#define REFTASKOWNERID  REFGUID


// ---IShellTaskScheduler
// An interface for interacting with and controlling a task scheduler. This
// interface does not need to be free-threaded unless the items in the queue
// interact with the scheduler as well as the main execution thread on which the
// task scheduler was created.

// IShellTaskScheduler::AddTask()
//      Adds Tasks to the scheduler's background queue. The TASKOWNERID allow particular types
//      of tasks to be grouped so that they can be counted or removed. The lParam allows the task
//      to be associated with a particular item (for example an item in a listview).
// IShellTaskScheduler::RemoveTasks()
//      Removes tasks from the scheduler's queue. These can be sepcified in terms of their TASKOWNERID
//      or their LPARAM, or both, or neither (TOID_NULL && ITSAT_DEFAULT_LPARAM results in all tasks being
//      removed). If a task that matches is currently running and ITaskScheduler::Status() has been
//      passeed ITSSFLAG_KILL_ON_DESTROY then the scheduler will attempt to kill the current task. The
//      fWaitIfRunning parameter is then passed to IRunnableTask::Kill().
// IShellTaskScheduler::CountTasks()
//      Counts the tasks in the queue depending upon the TASKOWNERID and the LPARAM passed. (TOID_NULL and
//      ITSAT_DEFAULT_LPARAM will count all tasks in the queue)
// IShellTaskScheduler::Status()
//      This sets the ReleaseStatus for the current task and the background thread timeout. When
//      ITaskScheduler::RemoveTasks() is called and there is a task currently running that matches
//      ITSSFLAG_COMPLETE_ON_DESTROY will cause TRUE to be passed to the task's IRunnableTask::Kill().
//      The dwThreadTimeout parameter if not set to the default will cause the background thread to
//      die if no new tasks have been added to the queue in the timeout period. The Thread will be
//      recreated when the next new task is added.

////////////////////////
// Status() flags,
// wait for the current task to complete before deleting the scheduler
#define ITSSFLAG_COMPLETE_ON_DESTROY        0x0000

// kill the current task (if there is one) when the task scheduler is deleted
#define ITSSFLAG_KILL_ON_DESTROY            0x0001

#define ITSSFLAG_SUPPORTS_TERMINATE         0x0002

#define ITSSFLAG_FLAGS_MASK                 0x0003

// set the timeout for killing the thread when the object is terminated.
// this timeout can be used to stop the object from blocking the system
// indefinitely.
#define ITSSFLAG_THREAD_TERMINATE_TIMEOUT   0x0010

// set the timeout for threads that are idle in the thread pool
#define ITSSFLAG_THREAD_POOL_TIMEOUT        0x0020

// The default timeout passed to release Status to determine how long the thread
// can be asleep before the thread is expired
#define ITSS_THREAD_DESTROY_DEFAULT_TIMEOUT     (60*1000)

// default, we won't kill it...
#define ITSS_THREAD_TERMINATE_TIMEOUT           (INFINITE)

// there is no change to the thread timeout
#define ITSS_THREAD_TIMEOUT_NO_CHANGE           (INFINITE - 1)

// the LPARAM allows task to be associated with items thus all tasks owned by a
// particular item can be accessed by passing a non default value for this parameter
#define ITSAT_DEFAULT_LPARAM        0xffffffff

// Task priorities
// ---------------
// This depends on the cooperation of tasks currently under execution. New tasks will
// be inserted in the queue in priority order. If a task of a low priority is currently
// under execution when a higher priority task is added, the scheduler will attempt
// to suspend the task currently under execution. It will be resumed when the other tasks
// have been completed.
#define ITSAT_DEFAULT_PRIORITY      0x10000000
#define ITSAT_MAX_PRIORITY          0x7fffffff
#define ITSAT_MIN_PRIORITY          0x00000000

#define TOID_NULL   CLSID_NULL

#undef INTERFACE
#define INTERFACE IShellTaskScheduler

DECLARE_INTERFACE_( IShellTaskScheduler, IUnknown )
{
    // *** IUnknown methods ***
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef)(THIS) = 0;
    STDMETHOD_(ULONG, Release)(THIS) = 0;

    // *** IShellTaskScheduler methods ***
    STDMETHOD (AddTask)(THIS_ IRunnableTask * pTask,
               REFTASKOWNERID rtoid,
               DWORD_PTR lParam,
               DWORD dwPriority ) = 0;

    STDMETHOD (RemoveTasks)(THIS_ REFTASKOWNERID rtoid,
               DWORD_PTR lParam,
               BOOL fWaitIfRunning ) = 0;

    STDMETHOD_(UINT, CountTasks)(THIS_ REFTASKOWNERID rtoid) = 0;

    STDMETHOD (Status)(THIS_ DWORD dwReleaseStatus, DWORD dwThreadTimeout ) = 0;
};

typedef IShellTaskScheduler * LPSHELLTASKSCHEDULER;

#if (_WIN32_IE >= 0x0501)

#define ITSSFLAG_TASK_PLACEINFRONT 0x00000001
#define ITSSFLAG_TASK_PLACEINBACK  0x00000002

#undef INTERFACE
#define INTERFACE IShellTaskScheduler2

DECLARE_INTERFACE_( IShellTaskScheduler2, IShellTaskScheduler )
{
    // *** IUnknown methods ***
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef)(THIS) = 0;
    STDMETHOD_(ULONG, Release)(THIS) = 0;

    // *** IShellTaskScheduler methods ***
    STDMETHOD (AddTask)(THIS_ IRunnableTask * pTask,
               REFTASKOWNERID rtoid,
               DWORD_PTR lParam,
               DWORD dwPriority ) = 0;

    STDMETHOD (RemoveTasks)(THIS_ REFTASKOWNERID rtoid,
               DWORD_PTR lParam,
               BOOL fWaitIfRunning ) = 0;

    STDMETHOD_(UINT, CountTasks)(THIS_ REFTASKOWNERID rtoid) = 0;

    STDMETHOD (Status)(THIS_ DWORD dwReleaseStatus, DWORD dwThreadTimeout ) = 0;

    // *** IShellTaskScheduler2 methods ***
    STDMETHOD (AddTask2)(THIS_ IRunnableTask * pTask,
               REFTASKOWNERID rtoid,
               DWORD_PTR lParam,
               DWORD dwPriority,
               DWORD grfFlags ) = 0;

    STDMETHOD (MoveTask)(THIS_ REFTASKOWNERID rtoid,
               DWORD_PTR lParam,
               DWORD dwPriority,
               DWORD grfFlags ) = 0;
};

#endif // (_WIN32_IE >= 0x0501)

#endif // (_WIN32_IE >= 0x0400)


#if (_WIN32_IE >= 0x0400)

/* ***************** IThumbnailCapture
 * CaptureThumbnail : takes an IHTMLDocument2 and returns a thumbnail of specified
 *                    size as an hbitmap
 */

#undef  INTERFACE
#define INTERFACE   IThumbnailCapture

DECLARE_INTERFACE_ ( IThumbnailCapture, IUnknown )
{
    // *** IThumbnailCapture methods ***
    STDMETHOD (CaptureThumbnail)    ( THIS_ const SIZE * pMaxSize,
                                      IUnknown * pHTMLDoc2,
                                      HBITMAP * phbmThumbnail ) = 0;
};
typedef IThumbnailCapture * LPTHUMBNAILCAPTURE;

#endif


#if (_WIN32_IE >= 0x0500)

#include <pshpack8.h>

typedef struct _EnumImageStoreDATAtag
{
    WCHAR     szPath[MAX_PATH];
    FILETIME  ftTimeStamp;
} ENUMSHELLIMAGESTOREDATA, * PENUMSHELLIMAGESTOREDATA;

#include <poppack.h>        /* Return to byte packing */

#undef  INTERFACE
#define INTERFACE   IEnumShellImageStore

DECLARE_INTERFACE_( IEnumShellImageStore, IUnknown )
{
    STDMETHOD ( QueryInterface ) ( THIS_ REFIID riid, void **ppv ) = 0;
    STDMETHOD_( ULONG, AddRef ) ( THIS ) = 0;
    STDMETHOD_( ULONG, Release ) ( THIS ) = 0;

    STDMETHOD ( Reset ) ( THIS ) = 0;
    STDMETHOD ( Next ) ( THIS_ ULONG celt, PENUMSHELLIMAGESTOREDATA * prgElt, ULONG * pceltFetched ) = 0;
    STDMETHOD ( Skip ) ( THIS_ ULONG celt ) = 0;
    STDMETHOD ( Clone ) ( THIS_ IEnumShellImageStore ** ppEnum ) = 0;
};

typedef IEnumShellImageStore * LPENUMSHELLIMAGESTORE;


// flags used to determine the capabilities of the storage for the images
#define SHIMSTCAPFLAG_LOCKABLE    0x0001       // does the store require/support locking
#define SHIMSTCAPFLAG_PURGEABLE   0x0002       // does the store require dead items purging externally ?

#undef  INTERFACE
#define INTERFACE   IShellImageStore

// this interface is used to manipulate the Image cache. It can potentially be used
// in a free threaded manner in conjunction with the Lock parameter to Open and close
DECLARE_INTERFACE_( IShellImageStore, IUnknown )
{
    STDMETHOD ( QueryInterface )( THIS_ REFIID riid, void **ppv ) = 0;
    STDMETHOD_( ULONG, AddRef ) ( THIS ) = 0;
    STDMETHOD_( ULONG, Release ) ( THIS ) = 0;

    // if the lock parameter is used, then all other calls into
    // open and/or create will block until the lock is released.
    STDMETHOD ( Open ) ( THIS_ DWORD dwMode, DWORD * pdwLock ) = 0;
    STDMETHOD ( Create ) ( THIS_ DWORD dwMode, DWORD * pdwLock ) = 0;

    // if the lock is passed to either of these two methods, it releases the lock
    // once the operation is complete.
    STDMETHOD ( ReleaseLock ) ( THIS_ DWORD const * pdwLock ) = 0;
    STDMETHOD ( Close ) ( THIS_ DWORD const * pdwLock ) = 0;
    STDMETHOD ( Commit ) ( THIS_ DWORD const * pdwLock ) = 0;
    STDMETHOD ( IsLocked ) ( THIS ) = 0;

    STDMETHOD ( GetMode ) ( THIS_ DWORD * pdwMode ) = 0;
    STDMETHOD ( GetCapabilities ) ( THIS_ DWORD * pdwCapMask ) = 0;

    STDMETHOD ( AddEntry ) ( THIS_ LPCWSTR pszName, const FILETIME * pftTimeStamp, DWORD dwMode, HBITMAP hImage ) = 0;
    STDMETHOD ( GetEntry ) ( THIS_ LPCWSTR pszName, DWORD dwMode, HBITMAP * phImage ) = 0;
    STDMETHOD ( DeleteEntry ) ( THIS_ LPCWSTR pszName ) = 0;
    STDMETHOD ( IsEntryInStore ) ( THIS_ LPCWSTR pszName, FILETIME * pftTimeStamp ) = 0;

    STDMETHOD ( Enum ) ( THIS_ LPENUMSHELLIMAGESTORE * ppEnum ) = 0;
};

typedef IShellImageStore * LPSHELLIMAGESTORE;
#endif

#if (_WIN32_IE >= 0x0400)


////  IShellFolderBand

// Field mask
#define ISFB_MASK_STATE          0x00000001 // TRUE if dwStateMask and dwState is valid
#define ISFB_MASK_BKCOLOR        0x00000002 // TRUE if crBkgnd field is valid
#define ISFB_MASK_VIEWMODE       0x00000004 // TRUE if wViewMode field is valid
#define ISFB_MASK_SHELLFOLDER    0x00000008
#define ISFB_MASK_IDLIST         0x00000010
#define ISFB_MASK_COLORS         0x00000020 // TRUE if crXXXX fields are valid (except bkgnd)

#define ISFB_STATE_DEFAULT       0x00000000
#define ISFB_STATE_DEBOSSED      0x00000001
#define ISFB_STATE_ALLOWRENAME   0x00000002
#define ISFB_STATE_NOSHOWTEXT    0x00000004 // TRUE if _fNoShowText
#define ISFB_STATE_CHANNELBAR    0x00000010 // TRUE if we want NavigateTarget support
#define ISFB_STATE_QLINKSMODE    0x00000020 // TRUE if we want to turn off drag & drop onto content items
#define ISFB_STATE_FULLOPEN      0x00000040 // TRUE if band should maximize when opened
#define ISFB_STATE_NONAMESORT    0x00000080 // TRUE if band should _not_ sort icons by name
#define ISFB_STATE_BTNMINSIZE    0x00000100 // TRUE if band should report min thickness of button

#define ISFBVIEWMODE_SMALLICONS   0x0001
#define ISFBVIEWMODE_LARGEICONS   0x0002
#define ISFBVIEWMODE_LOGOS        0x0003

#include <pshpack8.h>

typedef struct {
    DWORD       dwMask;       // [in] ISFB_MASK mask of valid fields from crBkgnd on
    DWORD       dwStateMask;  // [in] ISFB_STATE mask of dwState bits being set/queried
    DWORD       dwState;      // [in/out] ISFB_STATE bits
    COLORREF    crBkgnd;      // [in/out]
    COLORREF    crBtnLt;      // [in/out]
    COLORREF    crBtnDk;      // [in/out]
    WORD        wViewMode;    // [in/out]
    WORD        wAlign;       // not used (yet)
    IShellFolder * psf;       // [out]
    LPITEMIDLIST   pidl;      // [out]
} BANDINFOSFB, *PBANDINFOSFB;

#include <poppack.h>        /* Return to byte packing */

#undef INTERFACE
#define INTERFACE IShellFolderBand

DECLARE_INTERFACE_(IShellFolderBand, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IShellFolderBand Methods ***
    STDMETHOD(InitializeSFB)(THIS_ IShellFolder *psf, LPCITEMIDLIST pidl) = 0;
    STDMETHOD(SetBandInfoSFB)(THIS_ PBANDINFOSFB pbi) = 0;
    STDMETHOD(GetBandInfoSFB)(THIS_ PBANDINFOSFB pbi) = 0;
};

// Command Target IDs
enum {
    SFBID_PIDLCHANGED,
};


////  IDeskBarClient

#undef  INTERFACE
#define INTERFACE   IDeskBarClient

DECLARE_INTERFACE_(IDeskBarClient, IOleWindow)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IOleWindow methods ***
    STDMETHOD(GetWindow) (THIS_ HWND * lphwnd) = 0;
    STDMETHOD(ContextSensitiveHelp) (THIS_ BOOL fEnterMode) = 0;

    // *** IDeskBarClient methods ***
    STDMETHOD(SetDeskBarSite)   (THIS_ IUnknown* punkSite) = 0;
    STDMETHOD(SetModeDBC)          (THIS_ DWORD dwMode) = 0;
    STDMETHOD(UIActivateDBC)    (THIS_ DWORD dwState) = 0;
    STDMETHOD(GetSize)    (THIS_ DWORD dwWhich, LPRECT prc) = 0;
};

#define DBC_GS_IDEAL          0  // get the ideal size
#define DBC_GS_SIZEDOWN       1  // clip the height of a rect to a multiple of the rebar's integral size


#define DBC_HIDE        0 // Band is hidden (being destroyed)
#define DBC_SHOW        1 // Band is visible
#define DBC_SHOWOBSCURE 2 // Band is completely obscured


enum {
    DBCID_EMPTY = 0,        // bandsite is empty
    DBCID_ONDRAG = 1,       // (down)DragMoveEnter/Leave vaIn:I4:eDrag
    DBCID_CLSIDOFBAR = 2,   // clsid of bar inside  
    DBCID_RESIZE = 3,       // resize from keyboard
    DBCID_GETBAR = 4,       // returns vaOut:VT_UNKNOWN of hosting dockbar (IDeskBar)
};


#endif // (_WIN32_IE > 0x0400)



#if (_WIN32_IE >= 0x400)
//
// We need to make sure that WININET.H is included before this interface is
// used because the COMPONENT structure uses INTERNET_MAX_URL_LENGTH
//
#ifdef _WININET_
//
//  Flags and structures used by IActiveDesktop
//

typedef struct _tagWALLPAPEROPT
{
    DWORD   dwSize;     // size of this Structure.
    DWORD   dwStyle;    // WPSTYLE_* mentioned above
}
WALLPAPEROPT;

typedef WALLPAPEROPT  *LPWALLPAPEROPT;
typedef const WALLPAPEROPT *LPCWALLPAPEROPT;

typedef struct _tagCOMPONENTSOPT
{
    DWORD   dwSize;             //Size of this structure
    BOOL    fEnableComponents;  //Enable components?
    BOOL    fActiveDesktop;     // Active desktop enabled ?
}
COMPONENTSOPT;

typedef COMPONENTSOPT   *LPCOMPONENTSOPT;
typedef const COMPONENTSOPT   *LPCCOMPONENTSOPT;

typedef struct _tagCOMPPOS
{
    DWORD   dwSize;             //Size of this structure
    int     iLeft;              //Left of top-left corner in screen co-ordinates.
    int     iTop;               //Top of top-left corner in screen co-ordinates.
    DWORD   dwWidth;            // Width in pixels.
    DWORD   dwHeight;           // Height in pixels.
    int     izIndex;            // Indicates the Z-order of the component.
    BOOL    fCanResize;         // Is the component resizeable?
    BOOL    fCanResizeX;        // Resizeable in X-direction?
    BOOL    fCanResizeY;        // Resizeable in Y-direction?
    int     iPreferredLeftPercent;    //Left of top-left corner as percent of screen width
    int     iPreferredTopPercent;     //Top of top-left corner as percent of screen height
}
COMPPOS;

typedef COMPPOS *LPCOMPPOS;
typedef const COMPPOS *LPCCOMPPOS;

typedef struct  _tagCOMPSTATEINFO
{
    DWORD   dwSize;             // Size of this structure.
    int     iLeft;              // Left of the top-left corner in screen co-ordinates.
    int     iTop;               // Top of top-left corner in screen co-ordinates.
    DWORD   dwWidth;            // Width in pixels.
    DWORD   dwHeight;           // Height in pixels.
    DWORD   dwItemState;        // State of the component (full-screen mode or split-screen or normal state.
}
COMPSTATEINFO;

typedef COMPSTATEINFO   *LPCOMPSTATEINFO;
typedef const COMPSTATEINFO *LPCCOMPSTATEINFO;



#define COMPONENT_TOP (0x3fffffff)  // izOrder value meaning component is at the top


// iCompType values
#define COMP_TYPE_HTMLDOC       0
#define COMP_TYPE_PICTURE       1
#define COMP_TYPE_WEBSITE       2
#define COMP_TYPE_CONTROL       3
#define COMP_TYPE_CFHTML        4
#define COMP_TYPE_MAX           4

// The following is the COMPONENT structure used in IE4.01, IE4.0 and Memphis. It is kept here for compatibility
// reasons.
typedef struct _tagIE4COMPONENT
{
    DWORD   dwSize;             //Size of this structure
    DWORD   dwID;               //Reserved: Set it always to zero.
    int     iComponentType;     //One of COMP_TYPE_*
    BOOL    fChecked;           // Is this component enabled?
    BOOL    fDirty;             // Had the component been modified and not yet saved to disk?
    BOOL    fNoScroll;          // Is the component scrollable?
    COMPPOS cpPos;              // Width, height etc.,
    WCHAR   wszFriendlyName[MAX_PATH];          // Friendly name of component.
    WCHAR   wszSource[INTERNET_MAX_URL_LENGTH]; //URL of the component.
    WCHAR   wszSubscribedURL[INTERNET_MAX_URL_LENGTH]; //Subscrined URL
}
IE4COMPONENT;

typedef IE4COMPONENT *LPIE4COMPONENT;
typedef const IE4COMPONENT *LPCIE4COMPONENT;

//
// The following is the new NT5 component structure. Note that the initial portion of this component exactly
// matches the IE4COMPONENT structure. All new fields are added at the bottom and the dwSize field is used to
// distinguish between IE4COMPONENT and the new COMPONENT structures.
//
typedef struct _tagCOMPONENT
{
    DWORD   dwSize;             //Size of this structure
    DWORD   dwID;               //Reserved: Set it always to zero.
    int     iComponentType;     //One of COMP_TYPE_*
    BOOL    fChecked;           // Is this component enabled?
    BOOL    fDirty;             // Had the component been modified and not yet saved to disk?
    BOOL    fNoScroll;          // Is the component scrollable?
    COMPPOS cpPos;              // Width, height etc.,
    WCHAR   wszFriendlyName[MAX_PATH];          // Friendly name of component.
    WCHAR   wszSource[INTERNET_MAX_URL_LENGTH]; //URL of the component.
    WCHAR   wszSubscribedURL[INTERNET_MAX_URL_LENGTH]; //Subscrined URL

    //New fields are added below. Everything above here must exactly match the IE4COMPONENT Structure.
    DWORD           dwCurItemState; // Current state of the Component.
    COMPSTATEINFO   csiOriginal;    // Original state of the component when it was first added.
    COMPSTATEINFO   csiRestored;    // Restored state of the component.
}
COMPONENT;

typedef COMPONENT *LPCOMPONENT;
typedef const COMPONENT *LPCCOMPONENT;


// Defines for dwCurItemState
#define IS_NORMAL               0x00000001
#define IS_FULLSCREEN           0x00000002
#define IS_SPLIT                0x00000004
#define IS_VALIDSIZESTATEBITS   (IS_NORMAL | IS_SPLIT | IS_FULLSCREEN)  // The set of IS_* state bits which define the "size" of the component - these bits are mutually exclusive.
#define IS_VALIDSTATEBITS       (IS_NORMAL | IS_SPLIT | IS_FULLSCREEN | 0x80000000 | 0x40000000)  // All of the currently defined IS_* bits.

////////////////////////////////////////////
// Flags for IActiveDesktop::ApplyChanges()
#define AD_APPLY_SAVE             0x00000001
#define AD_APPLY_HTMLGEN          0x00000002
#define AD_APPLY_REFRESH          0x00000004
#define AD_APPLY_ALL              (AD_APPLY_SAVE | AD_APPLY_HTMLGEN | AD_APPLY_REFRESH)
#define AD_APPLY_FORCE            0x00000008
#define AD_APPLY_BUFFERED_REFRESH 0x00000010
#define AD_APPLY_DYNAMICREFRESH   0x00000020

////////////////////////////////////////////
// Flags for IActiveDesktop::GetWallpaperOptions()
//           IActiveDesktop::SetWallpaperOptions()
#define WPSTYLE_CENTER      0
#define WPSTYLE_TILE        1
#define WPSTYLE_STRETCH     2
#define WPSTYLE_MAX         3


////////////////////////////////////////////
// Flags for IActiveDesktop::ModifyComponent()

#define COMP_ELEM_TYPE          0x00000001
#define COMP_ELEM_CHECKED       0x00000002
#define COMP_ELEM_DIRTY         0x00000004
#define COMP_ELEM_NOSCROLL      0x00000008
#define COMP_ELEM_POS_LEFT      0x00000010
#define COMP_ELEM_POS_TOP       0x00000020
#define COMP_ELEM_SIZE_WIDTH    0x00000040
#define COMP_ELEM_SIZE_HEIGHT   0x00000080
#define COMP_ELEM_POS_ZINDEX    0x00000100
#define COMP_ELEM_SOURCE        0x00000200
#define COMP_ELEM_FRIENDLYNAME  0x00000400
#define COMP_ELEM_SUBSCRIBEDURL 0x00000800
#define COMP_ELEM_ORIGINAL_CSI  0x00001000
#define COMP_ELEM_RESTORED_CSI  0x00002000
#define COMP_ELEM_CURITEMSTATE  0x00004000

#define COMP_ELEM_ALL   (COMP_ELEM_TYPE | COMP_ELEM_CHECKED | COMP_ELEM_DIRTY |                     \
                         COMP_ELEM_NOSCROLL | COMP_ELEM_POS_LEFT | COMP_ELEM_SIZE_WIDTH  |          \
                         COMP_ELEM_SIZE_HEIGHT | COMP_ELEM_POS_ZINDEX | COMP_ELEM_SOURCE |          \
                         COMP_ELEM_FRIENDLYNAME | COMP_ELEM_POS_TOP | COMP_ELEM_SUBSCRIBEDURL |     \
                         COMP_ELEM_ORIGINAL_CSI | COMP_ELEM_RESTORED_CSI | COMP_ELEM_CURITEMSTATE)


////////////////////////////////////////////
// Flags for IActiveDesktop::AddDesktopItemWithUI()
typedef enum tagDTI_ADTIWUI
{
    DTI_ADDUI_DEFAULT               = 0x00000000,
    DTI_ADDUI_DISPSUBWIZARD         = 0x00000001,
    DTI_ADDUI_POSITIONITEM          = 0x00000002,
};


////////////////////////////////////////////
// Flags for IActiveDesktop::AddUrl()
#define ADDURL_SILENT           0X0001


////////////////////////////////////////////
// Default positions for ADI
#define COMPONENT_DEFAULT_LEFT    (0xFFFF)
#define COMPONENT_DEFAULT_TOP     (0xFFFF)




//
//  Interface for manipulating the Active Desktop.
//

#undef INTERFACE
#define INTERFACE IActiveDesktop

DECLARE_INTERFACE_( IActiveDesktop, IUnknown )
{
    // IUnknown methods
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef) ( THIS ) = 0;
    STDMETHOD_(ULONG, Release) ( THIS ) = 0;

    // IActiveDesktop methods
    STDMETHOD (ApplyChanges)(THIS_ DWORD dwFlags) = 0;
    STDMETHOD (GetWallpaper)(THIS_ LPWSTR pwszWallpaper, UINT cchWallpaper, DWORD dwReserved) = 0;
    STDMETHOD (SetWallpaper)(THIS_ LPCWSTR pwszWallpaper, DWORD dwReserved) = 0;
    STDMETHOD (GetWallpaperOptions)(THIS_ LPWALLPAPEROPT pwpo, DWORD dwReserved) = 0;
    STDMETHOD (SetWallpaperOptions)(THIS_ LPCWALLPAPEROPT pwpo, DWORD dwReserved) = 0;
    STDMETHOD (GetPattern)(THIS_ LPWSTR pwszPattern, UINT cchPattern, DWORD dwReserved) = 0;
    STDMETHOD (SetPattern)(THIS_ LPCWSTR pwszPattern, DWORD dwReserved) = 0;
    STDMETHOD (GetDesktopItemOptions)(THIS_ LPCOMPONENTSOPT pco, DWORD dwReserved) = 0;
    STDMETHOD (SetDesktopItemOptions)(THIS_ LPCCOMPONENTSOPT pco, DWORD dwReserved) = 0;
    STDMETHOD (AddDesktopItem)(THIS_ LPCCOMPONENT pcomp, DWORD dwReserved) = 0;
    STDMETHOD (AddDesktopItemWithUI)(THIS_ HWND hwnd, LPCOMPONENT pcomp, DWORD dwReserved) = 0;
    STDMETHOD (ModifyDesktopItem)(THIS_ LPCCOMPONENT pcomp, DWORD dwFlags) = 0;
    STDMETHOD (RemoveDesktopItem)(THIS_ LPCCOMPONENT pcomp, DWORD dwReserved) = 0;
    STDMETHOD (GetDesktopItemCount)(THIS_ LPINT lpiCount, DWORD dwReserved) = 0;
    STDMETHOD (GetDesktopItem)(THIS_ int nComponent, LPCOMPONENT pcomp, DWORD dwReserved) = 0;
    STDMETHOD (GetDesktopItemByID)(THIS_ ULONG_PTR dwID, LPCOMPONENT pcomp, DWORD dwReserved) = 0;
    STDMETHOD (GenerateDesktopItemHtml)(THIS_ LPCWSTR pwszFileName, LPCOMPONENT pcomp, DWORD dwReserved) = 0;
    STDMETHOD (AddUrl)(THIS_ HWND hwnd, LPCWSTR pszSource, LPCOMPONENT pcomp, DWORD dwFlags) = 0;
    STDMETHOD (GetDesktopItemBySource)(THIS_ LPCWSTR pwszSource, LPCOMPONENT pcomp, DWORD dwReserved) = 0;
};

typedef IActiveDesktop * LPACTIVEDESKTOP;

// Flags for SetSafeMode
#define SSM_CLEAR   0x0000
#define SSM_SET     0x0001
#define SSM_REFRESH 0x0002
#define SSM_UPDATE  0x0004

// Flags for Set/GetScheme
#define SCHEME_DISPLAY          0x0001
#define SCHEME_EDIT             0x0002
#define SCHEME_LOCAL            0x0004
#define SCHEME_GLOBAL           0x0008
#define SCHEME_REFRESH          0x0010
#define SCHEME_UPDATE           0x0020
#define SCHEME_DONOTUSE 0x0040 // used to be SCHEME_ENUMERATE; no longer supported
#define SCHEME_CREATE           0x0080

#undef INTERFACE
#define INTERFACE IActiveDesktopP

DECLARE_INTERFACE_( IActiveDesktopP, IUnknown )
{
    // IUnknown methods
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef) ( THIS ) = 0;
    STDMETHOD_(ULONG, Release) ( THIS ) = 0;

    // IActiveDesktopP methods
    STDMETHOD (SetSafeMode)(THIS_ DWORD dwFlags) = 0;
    STDMETHOD (EnsureUpdateHTML)(THIS) = 0;
    STDMETHOD (SetScheme)(THIS_ LPCWSTR pwszSchemeName, DWORD dwFlags) = 0;
    STDMETHOD (GetScheme)(THIS_ LPWSTR pwszSchemeName, DWORD *lpdwcchBuffer, DWORD dwFlags) = 0;
    //
};

typedef IActiveDesktopP * LPACTIVEDESKTOPP;

//Flags for GetObjectFlags
#define GADOF_DIRTY    0x00000001

#undef INTERFACE
#define INTERFACE IADesktopP2

DECLARE_INTERFACE_( IADesktopP2, IUnknown )
{
    // IUnknown methods
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef) ( THIS ) = 0;
    STDMETHOD_(ULONG, Release) ( THIS ) = 0;

    // IADesktopP2 methods
    STDMETHOD (ReReadWallpaper)(THIS) = 0;
    STDMETHOD (GetADObjectFlags)(THIS_ DWORD *lpdwFlags, DWORD dwMask) = 0;
    STDMETHOD (UpdateAllDesktopSubscriptions)(THIS) = 0;
    STDMETHOD (MakeDynamicChanges)(THIS_ IOleObject *pOleObj) = 0;
};

typedef IADesktopP2 * LPADESKTOPP2;


#endif // _WININET_

#if (_WIN32_IE >= 0x0500)

#define MAX_COLUMN_NAME_LEN 80
#define MAX_COLUMN_DESC_LEN 128

#include <pshpack1.h>

typedef struct {
    SHCOLUMNID  scid;                           // OUT the unique identifier of this column
    VARTYPE     vt;                             // OUT the native type of the data returned
    DWORD       fmt;                            // OUT this listview format (LVCFMT_LEFT, usually)
    UINT        cChars;                         // OUT the default width of the column, in characters
    DWORD       csFlags;                        // OUT SHCOLSTATE flags
    WCHAR wszTitle[MAX_COLUMN_NAME_LEN];        // OUT the title of the column
    WCHAR wszDescription[MAX_COLUMN_DESC_LEN];  // OUT full description of this column
} SHCOLUMNINFO, *LPSHCOLUMNINFO;
typedef const SHCOLUMNINFO* LPCSHCOLUMNINFO;

#include <poppack.h>        /* Return to default */

#include <pshpack8.h>

typedef struct {
    ULONG   dwFlags;              // initialization flags
    ULONG   dwReserved;           // reserved for future use.
    WCHAR   wszFolder[MAX_PATH];  // fully qualified folder path (or empty if multiple folders)
} SHCOLUMNINIT, *LPSHCOLUMNINIT;
typedef const SHCOLUMNINIT* LPCSHCOLUMNINIT;

#define SHCDF_UPDATEITEM        0x00000001      // this flag is a hint that the file has changed since the last call to GetItemData

typedef struct {
    ULONG   dwFlags;             // combination of SHCDF_ flags.
    DWORD   dwFileAttributes;    // file attributes.
    ULONG   dwReserved;          // reserved for future use.
    WCHAR*  pwszExt;             // address of file name extension
    WCHAR   wszFile[MAX_PATH];   // Absolute path of file.
} SHCOLUMNDATA, *LPSHCOLUMNDATA;
typedef const SHCOLUMNDATA* LPCSHCOLUMNDATA;

#include <poppack.h>        /* Return to byte packing */

#undef INTERFACE
#define INTERFACE IColumnProvider

// Note: these objects must be threadsafe!  GetItemData _will_ be called
// simultaneously from multiple threads.
DECLARE_INTERFACE_(IColumnProvider, IUnknown)
{
    // IUnknown methods
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef)(THIS) = 0;
    STDMETHOD_(ULONG, Release)(THIS) = 0;

    // IColumnProvider methods
    STDMETHOD (Initialize)(THIS_ LPCSHCOLUMNINIT psci) = 0;
    STDMETHOD (GetColumnInfo)(THIS_ DWORD dwIndex, SHCOLUMNINFO *psci) = 0;
    STDMETHOD (GetItemData)(THIS_ LPCSHCOLUMNID pscid, LPCSHCOLUMNDATA pscd, VARIANT *pvarData) = 0;
};


///////////////////////////////////////////////////////
//
// Drag and Drop helper
//
// Purpose: To expose the Shell drag images
//
// This interface is implemented in the shell by CLSID_DragDropHelper.
//
// To use:
//   If you are the source of a drag (i.e. in response to LV_DRAGBEGIN or
//    equivelent begin drag message) call
//    IDragSourceHelper::InitializeFromWindow
//              (<hwnd of window supporting DI_GETDRAGIMAGE>,
//               <pointer to POINT indicating offset to the mouse from
//                  the upper left corner of the image>,
//               <pointer to data object>)
//
//      NOTE: The Data object must support IDataObject::SetData with multiple
//            data types and GetData must implement data type cloning
//            (Including HGLOBAL), not just aliasing.
//
//   If you wish to have an image while over your application add the
//    IDragImages::Dr* calls to your IDropTarget implementation. For Example:
//
//    STDMETHODIMP CUserDropTarget::DragEnter(IDataObject* pDataObject,
//                                            DWORD grfKeyState,
//                                            POINTL pt, DWORD* pdwEffect)
//    {
//          // Process your DragEnter
//          // Call IDragImages::DragEnter last.
//          _pDropTargetHelper->DragEnter(_hwndDragOver, pDataObject,
//                                        (POINT*)&pt, *pdwEffect);
//          return hres;
//    }
//
//
//   If you wish to be able to source a drag image from a custom control,
//     implement a handler for the RegisterWindowMessage(DI_GETDRAGIMAGE).
//     The LPARAM is a pointer to an SHDRAGIMAGE structure.
//
//      sizeDragImage  -   Calculate the length and width required to render
//                          the images.
//      ptOffset       -   Calculate the offset from the upper left corner to
//                          the mouse cursor within the image
//      hbmpDragImage  -   CreateBitmap( sizeDragImage.cx, sizeDragImage.cy,
//                           GetDeviceCaps(hdcScreen, PLANES),
//                           GetDeviceCaps(hdcScreen, BITSPIXEL),
//                           NULL);
//
//   Drag Images will only be displayed on Windows NT 5.0 or later.
//
//
//   Note about IDropTargetHelper::Show - This method is provided for
//     showing/hiding the Drag image in low color depth video modes. When
//     painting to a window that is currently being dragged over (i.e. For
//     indicating a selection) you need to hide the drag image by calling this
//     method passing FALSE. After the window is done painting, Show the image
//     again by passing TRUE.

#include <pshpack8.h>

typedef struct
{
    SIZE        sizeDragImage;      // OUT - The length and Width of the
                                    //        rendered image
    POINT       ptOffset;           // OUT - The Offset from the mouse cursor to
                                    //        the upper left corner of the image
    HBITMAP     hbmpDragImage;      // OUT - The Bitmap containing the rendered
                                    //        drag images
    COLORREF    crColorKey;         // OUT - The COLORREF that has been blitted
                                    //        to the background of the images
} SHDRAGIMAGE, *LPSHDRAGIMAGE;

#include <poppack.h>        /* Return to byte packing */

// This is sent to a window to get the rendered images to a bitmap
// Call RegisterWindowMessage to get the ID
#define DI_GETDRAGIMAGE     TEXT("ShellGetDragImage")

#undef INTERFACE
#define INTERFACE IDropTargetHelper

DECLARE_INTERFACE_( IDropTargetHelper, IUnknown )
{
    // IUnknown methods
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef) ( THIS ) = 0;
    STDMETHOD_(ULONG, Release) ( THIS ) = 0;

    // IDropTargetHelper
    STDMETHOD (DragEnter)(THIS_ HWND hwndTarget, IDataObject* pDataObject,
                          POINT* ppt, DWORD dwEffect) = 0;
    STDMETHOD (DragLeave)(THIS) = 0;
    STDMETHOD (DragOver)(THIS_ POINT* ppt, DWORD dwEffect) = 0;
    STDMETHOD (Drop)(THIS_ IDataObject* pDataObject, POINT* ppt,
                     DWORD dwEffect) = 0;
    STDMETHOD (Show)(THIS_ BOOL fShow) = 0;

};

#undef INTERFACE
#define INTERFACE IDragSourceHelper

DECLARE_INTERFACE_( IDragSourceHelper, IUnknown )
{
    // IUnknown methods
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG, AddRef) ( THIS ) = 0;
    STDMETHOD_(ULONG, Release) ( THIS ) = 0;

    // IDragSourceHelper
    STDMETHOD (InitializeFromBitmap)(THIS_ LPSHDRAGIMAGE pshdi,
                                     IDataObject* pDataObject) = 0;
    STDMETHOD (InitializeFromWindow)(THIS_ HWND hwnd, POINT* ppt,
                                     IDataObject* pDataObject) = 0;
};
#endif // (_WIN32_IE >= 0x0500)
#endif // _WIN32_IE

//==========================================================================
// Clipboard format which may be supported by IDataObject from system
// defined shell folders (such as directories, network, ...).
//==========================================================================

#define CFSTR_SHELLIDLIST                   TEXT("Shell IDList Array")      // CF_IDLIST
#define CFSTR_SHELLIDLISTOFFSET             TEXT("Shell Object Offsets")    // CF_OBJECTPOSITIONS
#define CFSTR_NETRESOURCES                  TEXT("Net Resource")            // CF_NETRESOURCE
#define CFSTR_FILEDESCRIPTORA               TEXT("FileGroupDescriptor")     // CF_FILEGROUPDESCRIPTORA
#define CFSTR_FILEDESCRIPTORW               TEXT("FileGroupDescriptorW")    // CF_FILEGROUPDESCRIPTORW
#define CFSTR_FILECONTENTS                  TEXT("FileContents")            // CF_FILECONTENTS
#define CFSTR_FILENAMEA                     TEXT("FileName")                // CF_FILENAMEA
#define CFSTR_FILENAMEW                     TEXT("FileNameW")               // CF_FILENAMEW
#define CFSTR_PRINTERGROUP                  TEXT("PrinterFriendlyName")     // CF_PRINTERS
#define CFSTR_FILENAMEMAPA                  TEXT("FileNameMap")             // CF_FILENAMEMAPA
#define CFSTR_FILENAMEMAPW                  TEXT("FileNameMapW")            // CF_FILENAMEMAPW
#define CFSTR_SHELLURL                      TEXT("UniformResourceLocator")
#define CFSTR_INETURLA                      CFSTR_SHELLURL
#define CFSTR_INETURLW                      TEXT("UniformResourceLocatorW")
#define CFSTR_PREFERREDDROPEFFECT           TEXT("Preferred DropEffect")
#define CFSTR_PERFORMEDDROPEFFECT           TEXT("Performed DropEffect")
#define CFSTR_PASTESUCCEEDED                TEXT("Paste Succeeded")
#define CFSTR_INDRAGLOOP                    TEXT("InShellDragLoop")
#define CFSTR_DRAGCONTEXT                   TEXT("DragContext")
#define CFSTR_MOUNTEDVOLUME                 TEXT("MountedVolume")
#define CFSTR_PERSISTEDDATAOBJECT           TEXT("PersistedDataObject")
#define CFSTR_TARGETCLSID                           TEXT("TargetCLSID")                         // HGLOBAL with a CLSID of the drop target
#define CFSTR_LOGICALPERFORMEDDROPEFFECT    TEXT("Logical Performed DropEffect")
#define CFSTR_AUTOPLAY_SHELLIDLISTS         TEXT("Autoplay Enumerated IDList Array")  //  (HGLOBAL with LPIDA)
#define CFSTR_UNTRUSTEDDRAGDROP             TEXT("UntrustedDragDrop") //  DWORD 

#ifdef UNICODE
#define CFSTR_FILEDESCRIPTOR    CFSTR_FILEDESCRIPTORW
#define CFSTR_FILENAME          CFSTR_FILENAMEW
#define CFSTR_FILENAMEMAP       CFSTR_FILENAMEMAPW
#define CFSTR_INETURL           CFSTR_INETURLW
#else
#define CFSTR_FILEDESCRIPTOR    CFSTR_FILEDESCRIPTORA
#define CFSTR_FILENAME          CFSTR_FILENAMEA
#define CFSTR_FILENAMEMAP       CFSTR_FILENAMEMAPA
#define CFSTR_INETURL           CFSTR_INETURLA
#endif

#define DVASPECT_SHORTNAME      2 // use for CF_HDROP to get short name version of file paths
#define DVASPECT_COPY           3 // use to indicate format is a "Copy" of the data (FILECONTENTS, FILEDESCRIPTOR, etc)
#define DVASPECT_LINK           4 // use to indicate format is a "Shortcut" to the data (FILECONTENTS, FILEDESCRIPTOR, etc)

#include <pshpack8.h>
//
// format of CF_NETRESOURCE
//
typedef struct _NRESARRAY {     // anr
    UINT cItems;
    NETRESOURCE nr[1];
} NRESARRAY, * LPNRESARRAY;
#include <poppack.h>        /* Return to byte packing */

//
// format of CF_IDLIST
//
typedef struct _IDA {
    UINT cidl;          // number of relative IDList
    UINT aoffset[1];    // [0]: folder IDList, [1]-[cidl]: item IDList
} CIDA, * LPIDA;

//
// FILEDESCRIPTOR.dwFlags field indicate which fields are to be used
//
typedef enum {
    FD_CLSID            = 0x0001,
    FD_SIZEPOINT        = 0x0002,
    FD_ATTRIBUTES       = 0x0004,
    FD_CREATETIME       = 0x0008,
    FD_ACCESSTIME       = 0x0010,
    FD_WRITESTIME       = 0x0020,
    FD_FILESIZE         = 0x0040,
    FD_PROGRESSUI       = 0x4000,       // Show Progress UI w/Drag and Drop
    FD_LINKUI           = 0x8000,       // 'link' UI is prefered
} FD_FLAGS;

typedef struct _FILEDESCRIPTORA { // fod
    DWORD dwFlags;

    CLSID clsid;
    SIZEL sizel;
    POINTL pointl;

    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    CHAR   cFileName[ MAX_PATH ];
} FILEDESCRIPTORA, *LPFILEDESCRIPTORA;

typedef struct _FILEDESCRIPTORW { // fod
    DWORD dwFlags;

    CLSID clsid;
    SIZEL sizel;
    POINTL pointl;

    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    WCHAR  cFileName[ MAX_PATH ];
} FILEDESCRIPTORW, *LPFILEDESCRIPTORW;

#ifdef UNICODE
#define FILEDESCRIPTOR      FILEDESCRIPTORW
#define LPFILEDESCRIPTOR    LPFILEDESCRIPTORW
#else
#define FILEDESCRIPTOR      FILEDESCRIPTORA
#define LPFILEDESCRIPTOR    LPFILEDESCRIPTORA
#endif

//
// format of CF_FILEGROUPDESCRIPTOR
//
typedef struct _FILEGROUPDESCRIPTORA { // fgd
     UINT cItems;
     FILEDESCRIPTORA fgd[1];
} FILEGROUPDESCRIPTORA, * LPFILEGROUPDESCRIPTORA;

typedef struct _FILEGROUPDESCRIPTORW { // fgd
     UINT cItems;
     FILEDESCRIPTORW fgd[1];
} FILEGROUPDESCRIPTORW, * LPFILEGROUPDESCRIPTORW;

#ifdef UNICODE
#define FILEGROUPDESCRIPTOR     FILEGROUPDESCRIPTORW
#define LPFILEGROUPDESCRIPTOR   LPFILEGROUPDESCRIPTORW
#else
#define FILEGROUPDESCRIPTOR     FILEGROUPDESCRIPTORA
#define LPFILEGROUPDESCRIPTOR   LPFILEGROUPDESCRIPTORA
#endif

//
// format of CF_HDROP and CF_PRINTERS, in the HDROP case the data that follows
// is a double null terinated list of file names, for printers they are printer
// friendly names
//
typedef struct _DROPFILES {
   DWORD pFiles;                       // offset of file list
   POINT pt;                           // drop point (client coords)
   BOOL fNC;                           // is it on NonClient area
                                       // and pt is in screen coords
   BOOL fWide;                         // WIDE character switch
} DROPFILES, *LPDROPFILES;


//====== File System Notification APIs ===============================
//
typedef struct _SHChangeNotifyEntry
{
    LPCITEMIDLIST pidl;
    BOOL   fRecursive;
} SHChangeNotifyEntry;


//
//  File System Notification flags
//

#define SHCNE_RENAMEITEM          0x00000001L
#define SHCNE_CREATE              0x00000002L
#define SHCNE_DELETE              0x00000004L
#define SHCNE_MKDIR               0x00000008L
#define SHCNE_RMDIR               0x00000010L
#define SHCNE_MEDIAINSERTED       0x00000020L
#define SHCNE_MEDIAREMOVED        0x00000040L
#define SHCNE_DRIVEREMOVED        0x00000080L
#define SHCNE_DRIVEADD            0x00000100L
#define SHCNE_NETSHARE            0x00000200L
#define SHCNE_NETUNSHARE          0x00000400L
#define SHCNE_ATTRIBUTES          0x00000800L
#define SHCNE_UPDATEDIR           0x00001000L
#define SHCNE_UPDATEITEM          0x00002000L
#define SHCNE_SERVERDISCONNECT    0x00004000L
#define SHCNE_UPDATEIMAGE         0x00008000L
#define SHCNE_DRIVEADDGUI         0x00010000L
#define SHCNE_RENAMEFOLDER        0x00020000L
#define SHCNE_FREESPACE           0x00040000L

#if (_WIN32_IE >= 0x0400)
// SHCNE_EXTENDED_EVENT: the extended event is identified in dwItem1,
// packed in LPITEMIDLIST format (same as SHCNF_DWORD packing).
// Additional information can be passed in the dwItem2 parameter
// of SHChangeNotify (called "pidl2" below), which if present, must also
// be in LPITEMIDLIST format.
//
// Unlike the standard events, the extended events are ORDINALs, so we
// don't run out of bits.  Extended events follow the SHCNEE_* naming
// convention.
//
// The dwItem2 parameter varies according to the extended event.

#define SHCNE_EXTENDED_EVENT      0x04000000L
#endif      // (_WIN32_IE >= 0x0400)

#define SHCNE_ASSOCCHANGED        0x08000000L

#define SHCNE_DISKEVENTS          0x0002381FL
#define SHCNE_GLOBALEVENTS        0x0C0581E0L // Events that dont match pidls first
#define SHCNE_ALLEVENTS           0x7FFFFFFFL
#define SHCNE_INTERRUPT           0x80000000L // The presence of this flag indicates
                                            // that the event was generated by an
                                            // interrupt.  It is stripped out before
                                            // the clients of SHCNNotify_ see it.

#if (_WIN32_IE >= 0x0400)
// SHCNE_EXTENDED_EVENT extended events.  These events are ordinals.
// This is not a bitfield.

#define SHCNEE_ORDERCHANGED         2L  // pidl2 is the changed folder
#define SHCNEE_MSI_CHANGE           4L  // pidl2 is a SHChangeProductKeyAsIDList
#define SHCNEE_MSI_UNINSTALL        5L  // pidl2 is a SHChangeProductKeyAsIDList
#endif


// Flags
// uFlags & SHCNF_TYPE is an ID which indicates what dwItem1 and dwItem2 mean
#define SHCNF_IDLIST      0x0000        // LPITEMIDLIST
#define SHCNF_PATHA       0x0001        // path name
#define SHCNF_PRINTERA    0x0002        // printer friendly name
#define SHCNF_DWORD       0x0003        // DWORD
#define SHCNF_PATHW       0x0005        // path name
#define SHCNF_PRINTERW    0x0006        // printer friendly name
#define SHCNF_TYPE        0x00FF
#define SHCNF_FLUSH       0x1000
#define SHCNF_FLUSHNOWAIT 0x2000

#ifdef UNICODE
#define SHCNF_PATH      SHCNF_PATHW
#define SHCNF_PRINTER   SHCNF_PRINTERW
#else
#define SHCNF_PATH      SHCNF_PATHA
#define SHCNF_PRINTER   SHCNF_PRINTERA
#endif


//
//  APIs
//
SHSTDAPI_(void) SHChangeNotify(LONG wEventId, UINT uFlags, LPCVOID dwItem1, LPCVOID dwItem2);

//
// IShellChangeNotify
//
#undef  INTERFACE
#define INTERFACE  IShellChangeNotify

DECLARE_INTERFACE_(IShellChangeNotify, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IShellChangeNotify methods ***
    STDMETHOD(OnChange) (THIS_ LONG lEvent, LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2) = 0;
} ;

//
// IQueryInfo
//
//-------------------------------------------------------------------------
//
// IQueryInfo interface
//
// [Methods]
//              ::GetInfoTip()
//-------------------------------------------------------------------------

#undef  INTERFACE
#define INTERFACE  IQueryInfo

DECLARE_INTERFACE_(IQueryInfo, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IQueryInfo methods ***
    STDMETHOD(GetInfoTip)(THIS_ DWORD dwFlags, WCHAR **ppwszTip) = 0;
    STDMETHOD(GetInfoFlags)(THIS_ DWORD *pdwFlags) = 0;
} ;

#define QITIPF_DEFAULT          0x00000000
#define QITIPF_USENAME          0x00000001
#define QITIPF_LINKNOTARGET     0x00000002
#define QITIPF_LINKUSETARGET    0x00000004
#define QITIPF_USESLOWTIP       0x00000008  // Flag says it's OK to take a long time generating tip

#define QIF_CACHED           0x00000001
#define QIF_DONTEXPANDFOLDER 0x00000002


//
// SHAddToRecentDocs
//
#define SHARD_PIDL      0x00000001L
#define SHARD_PATHA     0x00000002L
#define SHARD_PATHW     0x00000003L

#ifdef UNICODE
#define SHARD_PATH  SHARD_PATHW
#else
#define SHARD_PATH  SHARD_PATHA
#endif

SHSTDAPI_(void) SHAddToRecentDocs(UINT uFlags, LPCVOID pv);

typedef struct _SHChangeDWORDAsIDList {
    USHORT   cb;
    DWORD    dwItem1;
    DWORD    dwItem2;
    USHORT   cbZero;
} SHChangeDWORDAsIDList, *LPSHChangeDWORDAsIDList;


#if (_WIN32_IE >= 0x0400)


typedef struct _SHChangeUpdateImageIDList {
    USHORT cb;
    int iIconIndex;
    int iCurIndex;
    UINT uFlags;
    DWORD dwProcessID;
    WCHAR szName[MAX_PATH];
    USHORT cbZero;
} SHChangeUpdateImageIDList, * LPSHChangeUpdateImageIDList;

SHSTDAPI_(int)  SHHandleUpdateImage( LPCITEMIDLIST pidlExtra );

typedef struct _SHChangeProductKeyAsIDList {
    USHORT cb;
    WCHAR wszProductKey[39];
    USHORT cbZero;
} SHChangeProductKeyAsIDList, *LPSHChangeProductKeyAsIDList;


SHSTDAPI_(void) SHUpdateImageA(LPCSTR pszHashItem, int iIndex, UINT uFlags, int iImageIndex);
SHSTDAPI_(void) SHUpdateImageW(LPCWSTR pszHashItem, int iIndex, UINT uFlags, int iImageIndex);
#ifdef UNICODE
#define SHUpdateImage  SHUpdateImageW
#else
#define SHUpdateImage  SHUpdateImageA
#endif // !UNICODE
#endif /* _WIN32_IE */

SHSTDAPI_(ULONG) SHChangeNotifyRegister(HWND hwnd, int fSources, LONG fEvents, UINT wMsg, int cEntries, SHChangeNotifyEntry *pshcne);
SHSTDAPI_(BOOL) SHChangeNotifyDeregister(unsigned long ulID);
SHSTDAPI_(HANDLE) SHChangeNotification_Lock(HANDLE hChangeNotification, DWORD dwProcessId, LPITEMIDLIST **pppidl, LONG *plEvent);
SHSTDAPI_(BOOL) SHChangeNotification_Unlock(HANDLE hLock);
#if (_WIN32_IE >= 0x0400)
// The pidls that are given to the view via the ChangeNotifyEvents are simple Pidls,
// SHGetRealIDL() will convert them to true PIDLs.
SHSTDAPI SHGetRealIDL(IShellFolder *psf, LPCITEMIDLIST pidlSimple, LPITEMIDLIST * ppidlReal);
#endif // (_WIN32_IE >= 0x0400)


SHSTDAPI SHGetInstanceExplorer(IUnknown **ppunk);

//
// SHGetDataFromIDListA/W
//
// SHGetDataFromIDList nFormat values TCHAR
#define SHGDFIL_FINDDATA        1
#define SHGDFIL_NETRESOURCE     2
#define SHGDFIL_DESCRIPTIONID   3

#define SHDID_ROOT_REGITEM          1
#define SHDID_FS_FILE               2
#define SHDID_FS_DIRECTORY          3
#define SHDID_FS_OTHER              4
#define SHDID_COMPUTER_DRIVE35      5
#define SHDID_COMPUTER_DRIVE525     6
#define SHDID_COMPUTER_REMOVABLE    7
#define SHDID_COMPUTER_FIXED        8
#define SHDID_COMPUTER_NETDRIVE     9
#define SHDID_COMPUTER_CDROM        10
#define SHDID_COMPUTER_RAMDISK      11
#define SHDID_COMPUTER_OTHER        12
#define SHDID_NET_DOMAIN            13
#define SHDID_NET_SERVER            14
#define SHDID_NET_SHARE             15
#define SHDID_NET_RESTOFNET         16
#define SHDID_NET_OTHER             17
#define SHDID_COMPUTER_IMAGING      18
#define SHDID_COMPUTER_AUDIO        19
#define SHDID_COMPUTER_SHAREDDOCS   20

#include <pshpack8.h>

typedef struct _SHDESCRIPTIONID {
    DWORD   dwDescriptionId;
    CLSID   clsid;
} SHDESCRIPTIONID, *LPSHDESCRIPTIONID;

#include <poppack.h>        /* Return to byte packing */

// these delegate to IShellFolder2::GetItemData()

SHSTDAPI SHGetDataFromIDListA(IShellFolder *psf, LPCITEMIDLIST pidl, int nFormat, void *pv, int cb);
SHSTDAPI SHGetDataFromIDListW(IShellFolder *psf, LPCITEMIDLIST pidl, int nFormat, void *pv, int cb);
#ifdef UNICODE
#define SHGetDataFromIDList  SHGetDataFromIDListW
#else
#define SHGetDataFromIDList  SHGetDataFromIDListA
#endif // !UNICODE


//===========================================================================

// PathResolve flags
#define PRF_VERIFYEXISTS            0x0001
#define PRF_TRYPROGRAMEXTENSIONS    (0x0002 | PRF_VERIFYEXISTS)
#define PRF_FIRSTDIRDEF             0x0004
#define PRF_DONTFINDLNK             0x0008      // if PRF_TRYPROGRAMEXTENSIONS is specified
SHSTDAPI_(int) RestartDialog(HWND hwnd, LPCWSTR lpPrompt, DWORD dwReturn);
SHSTDAPI_(int) RestartDialogEx(HWND hwnd, LPCWSTR lpPrompt, DWORD dwReturn, DWORD dwReasonCode);

SHSTDAPI SHCoCreateInstance(LPCWSTR pszCLSID, const CLSID *pclsid, IUnknown *pUnkOuter, REFIID riid, void **ppv);
// For CallCPLEntry16
//
DECLARE_HANDLE(FARPROC16);
SHSTDAPI_(LRESULT) CallCPLEntry16(HINSTANCE hinst, FARPROC16 lpfnEntry, HWND hwndCPL, UINT msg, LPARAM lParam1, LPARAM lParam2);
SHSTDAPI SHCreateStdEnumFmtEtc(UINT cfmt, const FORMATETC afmt[], IEnumFORMATETC **ppenumFormatEtc);
SHSTDAPI SHDoDragDrop(HWND hwnd, IDataObject *pdata, IDropSource *pdsrc, DWORD dwEffect, DWORD *pdwEffect);
// stuff for doing auto scrolling
#define NUM_POINTS      3
typedef struct {        // asd
    int iNextSample;
    DWORD dwLastScroll;
    BOOL bFull;
    POINT pts[NUM_POINTS];
    DWORD dwTimes[NUM_POINTS];
} AUTO_SCROLL_DATA;

SHSTDAPI_(BOOL) DAD_SetDragImage(HIMAGELIST him, POINT * pptOffset);
SHSTDAPI_(BOOL) DAD_DragEnterEx(HWND hwndTarget, const POINT ptStart);
SHSTDAPI_(BOOL) DAD_DragEnterEx2(HWND hwndTarget, const POINT ptStart, IDataObject *pdtObject);
SHSTDAPI_(BOOL) DAD_ShowDragImage(BOOL fShow);                                                      
SHSTDAPI_(BOOL) DAD_DragMove(POINT pt);
SHSTDAPI_(BOOL) DAD_DragLeave(void);
SHSTDAPI_(BOOL) DAD_AutoScroll(HWND hwnd, AUTO_SCROLL_DATA *pad, const POINT *pptNow);
typedef struct {
    WORD cLength;
    WORD nVersion;

    BOOL fFullPathTitle            : 1;
    BOOL fSaveLocalView            : 1;
    BOOL fNotShell                 : 1;
    BOOL fSimpleDefault            : 1;
    BOOL fDontShowDescBar          : 1;
    BOOL fNewWindowMode            : 1;
    BOOL fShowCompColor            : 1;  // NT: Show compressed volumes in a different colour
    BOOL fDontPrettyNames          : 1;  // NT: Do 8.3 name conversion, or not!
    BOOL fAdminsCreateCommonGroups : 1;  // NT: Administrators create comon groups
    UINT fUnusedFlags : 7;

    UINT fMenuEnumFilter;

} CABINETSTATE, * LPCABINETSTATE;

#define CABINETSTATE_VERSION 2

// APIs for reading and writing the cabinet state.
SHSTDAPI_(BOOL) ReadCabinetState( LPCABINETSTATE lpState, int iSize );
SHSTDAPI_(BOOL) WriteCabinetState( LPCABINETSTATE lpState );
SHSTDAPI_(BOOL) PathMakeUniqueName(LPWSTR pszUniqueName, UINT cchMax, LPCWSTR pszTemplate, LPCWSTR pszLongPlate, LPCWSTR pszDir);
SHSTDAPI_(void) PathQualify(LPWSTR psz);
SHSTDAPI_(BOOL) PathIsExe(LPCWSTR pszPath);
SHSTDAPI_(BOOL) PathIsSlowA(LPCSTR pszFile, DWORD dwAttr);
SHSTDAPI_(BOOL) PathIsSlowW(LPCWSTR pszFile, DWORD dwAttr);
#ifdef UNICODE
#define PathIsSlow  PathIsSlowW
#else
#define PathIsSlow  PathIsSlowA
#endif // !UNICODE
//
//  Return codes from PathCleanupSpec.  Negative return values are
//  unrecoverable errors
//
#define PCS_FATAL           0x80000000
#define PCS_REPLACEDCHAR    0x00000001
#define PCS_REMOVEDCHAR     0x00000002
#define PCS_TRUNCATED       0x00000004
#define PCS_PATHTOOLONG     0x00000008  // Always combined with FATAL

SHSTDAPI_(int) PathCleanupSpec(LPCWSTR pszDir, LPWSTR pszSpec);
SHSTDAPI_(int) PathResolve(LPWSTR pszPath, LPCWSTR dirs[], UINT fFlags);
SHSTDAPI_(BOOL) GetFileNameFromBrowse(HWND hwnd, LPWSTR pszFilePath, UINT cbFilePath,
                                      LPCWSTR pszWorkingDir, LPCWSTR pszDefExt, LPCWSTR pszFilters, LPCWSTR pszTitle);
SHSTDAPI_(int) DriveType(int iDrive);

SHSTDAPI_(int) RealDriveType(int iDrive, BOOL fOKToHitNet);
SHSTDAPI_(int) IsNetDrive(int iDrive);
// Flags for Shell_MergeMenus
#define MM_ADDSEPARATOR         0x00000001L
#define MM_SUBMENUSHAVEIDS      0x00000002L
#define MM_DONTREMOVESEPS       0x00000004L

SHSTDAPI_(UINT) Shell_MergeMenus(HMENU hmDst, HMENU hmSrc, UINT uInsert, UINT uIDAdjust, UINT uIDAdjustMax, ULONG uFlags);


/*
 * The SHObjectProperties API provides an easy way to invoke
 *   the Properties context menu command on shell objects.
 *
 *   PARAMETERS
 *
 *     hwnd    The window handle of the window which will own the dialog
 *     dwType       A SHOP_ value as defined below
 *     lpObject     Name of the object, see SHOP_ values below
 *     lpPage       The name of the property sheet page to open to or NULL.
 *
 *   RETURN
 *
 *     TRUE if the Properties command was invoked
 */
SHSTDAPI_(BOOL) SHObjectProperties(HWND hwnd, DWORD dwType, LPCWSTR lpObject, LPCWSTR lpPage);

#define SHOP_PRINTERNAME 0x00000001  // lpObject points to a printer friendly name
#define SHOP_FILEPATH    0x00000002  // lpObject points to a fully qualified path+file name
#define SHOP_VOLUMEGUID  0x00000004  // lpObject points to a Volume GUID


/*
 * The SHFormatDrive API provides access to the Shell
 *   format dialog. This allows apps which want to format disks
 *   to bring up the same dialog that the Shell does to do it.
 *
 *   This dialog is not sub-classable. You cannot put custom
 *   controls in it. If you want this ability, you will have
 *   to write your own front end for the DMaint_FormatDrive
 *   engine.
 *
 *   NOTE that the user can format as many diskettes in the specified
 *   drive, or as many times, as he/she wishes to. There is no way to
 *   force any specififc number of disks to format. If you want this
 *   ability, you will have to write your own front end for the
 *   DMaint_FormatDrive engine.
 *
 *   NOTE also that the format will not start till the user pushes the
 *   start button in the dialog. There is no way to do auto start. If
 *   you want this ability, you will have to write your own front end
 *   for the DMaint_FormatDrive engine.
 *
 *   PARAMETERS
 *
 *     hwnd    = The window handle of the window which will own the dialog
 *               NOTE that unlike SHCheckDrive, hwnd == NULL does not cause
 *               this dialog to come up as a "top level application" window.
 *               This parameter should always be non-null, this dialog is
 *               only designed to be the child of another window, not a
 *               stand-alone application.
 *     drive   = The 0 based (A: == 0) drive number of the drive to format
 *     fmtID   = The ID of the physical format to format the disk with
 *               NOTE: The special value SHFMT_ID_DEFAULT means "use the
 *                     default format specified by the DMaint_FormatDrive
 *                     engine". If you want to FORCE a particular format
 *                     ID "up front" you will have to call
 *                     DMaint_GetFormatOptions yourself before calling
 *                     this to obtain the valid list of phys format IDs
 *                     (contents of the PhysFmtIDList array in the
 *                     FMTINFOSTRUCT).
 *     options = There is currently only two option bits defined
 *
 *                SHFMT_OPT_FULL
 *                SHFMT_OPT_SYSONLY
 *
 *               The normal defualt in the Shell format dialog is
 *               "Quick Format", setting this option bit indicates that
 *               the caller wants to start with FULL format selected
 *               (this is useful for folks detecting "unformatted" disks
 *               and wanting to bring up the format dialog).
 *
 *               The SHFMT_OPT_SYSONLY initializes the dialog to
 *               default to just sys the disk.
 *
 *               All other bits are reserved for future expansion and
 *               must be 0.
 *
 *               Please note that this is a bit field and not a value
 *               and treat it accordingly.
 *
 *   RETURN
 *      The return is either one of the SHFMT_* values, or if the
 *      returned DWORD value is not == to one of these values, then
 *      the return is the physical format ID of the last succesful
 *      format. The LOWORD of this value can be passed on subsequent
 *      calls as the fmtID parameter to "format the same type you did
 *      last time".
 *
 */
SHSTDAPI_(DWORD) SHFormatDrive(HWND hwnd, UINT drive, UINT fmtID, UINT options);
//
// Special value of fmtID which means "use the default format"
//
#define SHFMT_ID_DEFAULT    0xFFFF

//
// Option bits for options parameter
//
#define SHFMT_OPT_FULL     0x0001
#define SHFMT_OPT_SYSONLY  0x0002

//
// Special return values. PLEASE NOTE that these are DWORD values.
//
#define SHFMT_ERROR     0xFFFFFFFFL     // Error on last format, drive may be formatable
#define SHFMT_CANCEL    0xFFFFFFFEL     // Last format was canceled
#define SHFMT_NOFORMAT  0xFFFFFFFDL     // Drive is not formatable

#ifndef HPSXA_DEFINED
#define HPSXA_DEFINED
DECLARE_HANDLE( HPSXA );
#endif
WINSHELLAPI HPSXA WINAPI SHCreatePropSheetExtArray( HKEY hKey, LPCWSTR pszSubKey, UINT max_iface );
WINSHELLAPI void WINAPI SHDestroyPropSheetExtArray( HPSXA hpsxa );
WINSHELLAPI UINT WINAPI SHAddFromPropSheetExtArray( HPSXA hpsxa, LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam );
WINSHELLAPI UINT WINAPI SHReplaceFromPropSheetExtArray( HPSXA hpsxa, UINT uPageID, LPFNADDPROPSHEETPAGE lpfnReplaceWith, LPARAM lParam );
SHSTDAPI_(LPITEMIDLIST) ILClone(LPCITEMIDLIST pidl);
SHSTDAPI_(LPITEMIDLIST) ILGetNext(LPCITEMIDLIST pidl);
SHSTDAPI_(UINT)         ILGetSize(LPCITEMIDLIST pidl);
SHSTDAPI_(LPITEMIDLIST) ILFindLastID(LPCITEMIDLIST pidl);
SHSTDAPI_(BOOL)         ILRemoveLastID(LPITEMIDLIST pidl);
SHSTDAPI_(LPITEMIDLIST) ILAppendID(LPITEMIDLIST pidl, LPCSHITEMID pmkid, BOOL fAppend);
SHSTDAPI_(void)         ILFree(LPITEMIDLIST pidl);
SHSTDAPI_(LPITEMIDLIST) ILCloneFirst(LPCITEMIDLIST pidl);
SHSTDAPI_(BOOL)         ILIsEqual(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
SHSTDAPI_(BOOL)         ILIsParent(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2, BOOL fImmediate);
SHSTDAPI_(LPITEMIDLIST) ILFindChild(LPCITEMIDLIST pidlParent, LPCITEMIDLIST pidlChild);
SHSTDAPI_(LPITEMIDLIST) ILCombine(LPCITEMIDLIST pidl1, LPCITEMIDLIST pidl2);
SHSTDAPI                ILLoadFromStream(IStream *pstm, LPITEMIDLIST *pidl);
SHSTDAPI                ILSaveToStream(IStream *pstm, LPCITEMIDLIST pidl);
#if (_WIN32_IE >= 0x0400)

SHSTDAPI_(LPITEMIDLIST) ILCreateFromPathA(LPCSTR pszPath);
SHSTDAPI_(LPITEMIDLIST) ILCreateFromPathW(LPCWSTR pszPath);

#ifdef NO_WRAPPERS_FOR_ILCREATEFROMPATH
SHSTDAPI_(LPITEMIDLIST) ILCreateFromPath(LPCTSTR pszPath);
#else
#ifdef UNICODE
#define ILCreateFromPath  ILCreateFromPathW
#else
#define ILCreateFromPath  ILCreateFromPathA
#endif // !UNICODE
#endif

#endif
SHSTDAPI SHILCreateFromPath(LPCWSTR szPath, LPITEMIDLIST *ppidl, DWORD *rgfInOut);
#undef  INTERFACE
#define INTERFACE   IDefViewFrame
DECLARE_INTERFACE_(IDefViewFrame, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IDefViewFrame methods ***
    STDMETHOD(GetWindowLV) (THIS_ HWND * phwnd) = 0;
    STDMETHOD(ReleaseWindowLV) (THIS) = 0;
    STDMETHOD(GetShellFolder)(THIS_ IShellFolder **ppsf) = 0;
};

//===========================================================================
// Shell restrictions. (Parameter for SHRestricted)
typedef enum RESTRICTIONS
{
    REST_NONE                       = 0x00000000,
    REST_NORUN                      = 0x00000001,
    REST_NOCLOSE                    = 0x00000002,
    REST_NOSAVESET                  = 0x00000004,
    REST_NOFILEMENU                 = 0x00000008,
    REST_NOSETFOLDERS               = 0x00000010,
    REST_NOSETTASKBAR               = 0x00000020,
    REST_NODESKTOP                  = 0x00000040,
    REST_NOFIND                     = 0x00000080,
    REST_NODRIVES                   = 0x00000100,
    REST_NODRIVEAUTORUN             = 0x00000200,
    REST_NODRIVETYPEAUTORUN         = 0x00000400,
    REST_NONETHOOD                  = 0x00000800,
    REST_STARTBANNER                = 0x00001000,
    REST_RESTRICTRUN                = 0x00002000,
    REST_NOPRINTERTABS              = 0x00004000,
    REST_NOPRINTERDELETE            = 0x00008000,
    REST_NOPRINTERADD               = 0x00010000,
    REST_NOSTARTMENUSUBFOLDERS      = 0x00020000,
    REST_MYDOCSONNET                = 0x00040000,
    REST_NOEXITTODOS                = 0x00080000,
    REST_ENFORCESHELLEXTSECURITY    = 0x00100000,
    REST_LINKRESOLVEIGNORELINKINFO  = 0x00200000,
    REST_NOCOMMONGROUPS             = 0x00400000,
    REST_SEPARATEDESKTOPPROCESS     = 0x00800000,
    REST_NOWEB                      = 0x01000000,
    REST_NOTRAYCONTEXTMENU          = 0x02000000,
    REST_NOVIEWCONTEXTMENU          = 0x04000000,
    REST_NONETCONNECTDISCONNECT     = 0x08000000,
    REST_STARTMENULOGOFF            = 0x10000000,
    REST_NOSETTINGSASSIST           = 0x20000000,
    REST_NOINTERNETICON             = 0x40000001,
    REST_NORECENTDOCSHISTORY        = 0x40000002,
    REST_NORECENTDOCSMENU           = 0x40000003,
    REST_NOACTIVEDESKTOP            = 0x40000004,
    REST_NOACTIVEDESKTOPCHANGES     = 0x40000005,
    REST_NOFAVORITESMENU            = 0x40000006,
    REST_CLEARRECENTDOCSONEXIT      = 0x40000007,
    REST_CLASSICSHELL               = 0x40000008,
    REST_NOCUSTOMIZEWEBVIEW         = 0x40000009,
    REST_NOHTMLWALLPAPER            = 0x40000010,
    REST_NOCHANGINGWALLPAPER        = 0x40000011,
    REST_NODESKCOMP                 = 0x40000012,
    REST_NOADDDESKCOMP              = 0x40000013,
    REST_NODELDESKCOMP              = 0x40000014,
    REST_NOCLOSEDESKCOMP            = 0x40000015,
    REST_NOCLOSE_DRAGDROPBAND       = 0x40000016,   // Disable Close and Drag & Drop on ALL Bands
    REST_NOMOVINGBAND               = 0x40000017,   // Disable Moving ALL Bands
    REST_NOEDITDESKCOMP             = 0x40000018,
    REST_NORESOLVESEARCH            = 0x40000019,
    REST_NORESOLVETRACK             = 0x4000001A,
    REST_FORCECOPYACLWITHFILE       = 0X4000001B,
    REST_NOLOGO3CHANNELNOTIFY       = 0x4000001C,
    REST_NOFORGETSOFTWAREUPDATE     = 0x4000001D,
    REST_NOSETACTIVEDESKTOP         = 0x4000001E,   // No Active desktop on Settings Menu
    REST_NOUPDATEWINDOWS            = 0x4000001F,   // No Windows Update on Settings Menu
    REST_NOCHANGESTARMENU           = 0x40000020,   // No Context menu or Drag and Drop on Start menu
    REST_NOFOLDEROPTIONS            = 0x40000021,   // No Folder Options on Settings Menu
    REST_HASFINDCOMPUTERS           = 0x40000022,   // Show Start/Search/Computers
    REST_INTELLIMENUS               = 0x40000023,
    REST_RUNDLGMEMCHECKBOX          = 0x40000024,
    REST_ARP_ShowPostSetup          = 0x40000025,   // ARP: Show Post-Setup page
    REST_NOCSC                      = 0x40000026,   // Disable the ClientSide caching on SM
    REST_NOCONTROLPANEL             = 0x40000027,   // Remove the Control Panel only from SM|Settings
    REST_ENUMWORKGROUP              = 0x40000028,   // Enumerate workgroup in root of nethood
    REST_ARP_NOARP                  = 0x40000029,   // ARP: Don't Allow ARP to come up at all
    REST_ARP_NOREMOVEPAGE           = 0x4000002A,   // ARP: Don't allow Remove page
    REST_ARP_NOADDPAGE              = 0x4000002B,   // ARP: Don't allow Add page
    REST_ARP_NOWINSETUPPAGE         = 0x4000002C,   // ARP: Don't allow opt components page
    REST_GREYMSIADS                 = 0x4000002D,    // SM: Allow the greying of Darwin Ads in SM
    REST_NOCHANGEMAPPEDDRIVELABEL   = 0x4000002E,   // Don't enable the UI which allows users to rename mapped drive labels
    REST_NOCHANGEMAPPEDDRIVECOMMENT = 0x4000002F,   // Don't enable the UI which allows users to change mapped drive comments
    REST_MaxRecentDocs              = 0x40000030,
    REST_NONETWORKCONNECTIONS       = 0x40000031,   // No Start Menu | Settings |Network Connections
    REST_FORCESTARTMENULOGOFF       = 0x40000032,   // Force logoff on the Start Menu
    REST_NOWEBVIEW                  = 0x40000033,   // Disable Web View
    REST_NOCUSTOMIZETHISFOLDER      = 0x40000034,   // Disable Customize This Folder
    REST_NOENCRYPTION               = 0x40000035,   // Don't allow file encryption
//  Do NOT use me                     0x40000036,
    REST_DONTSHOWSUPERHIDDEN        = 0x40000037,   // don't show super hidden files
    REST_NOSHELLSEARCHBUTTON        = 0x40000038,
    REST_NOHARDWARETAB              = 0x40000039,   // No Hardware tab on Drives or in control panel
    REST_NORUNASINSTALLPROMPT       = 0x4000003A,   // Don't bring up "Run As" prompt for install programs
    REST_PROMPTRUNASINSTALLNETPATH  = 0x4000003B,   // Force the  "Run As" prompt for install programs on unc/network shares
    REST_NOMANAGEMYCOMPUTERVERB     = 0x4000003C,   // No Manage verb on My Computer
    REST_NORECENTDOCSNETHOOD        = 0x4000003D,   // dont add the recent docs shares to nethood
    REST_DISALLOWRUN                = 0x4000003E,   // don't allow certain apps to be run
    REST_NOWELCOMESCREEN            = 0x4000003F,   // don't allow the welcome screen to be displayed.
    REST_RESTRICTCPL                = 0x40000040,   // only allow certain cpls to be run
    REST_DISALLOWCPL                = 0x40000041,   // don't allow certain cpls to be run
    REST_NOSMBALLOONTIP             = 0x40000042,   // No Start Menu Balloon Tip
    REST_NOSMHELP                   = 0x40000043,   // No Help on the Start Menu
    REST_NOWINKEYS                  = 0x40000044,   // No Windows-X Hot keys
    REST_NOENCRYPTONMOVE            = 0x40000045,   // Don't automatically try to encrypt files that are moved to encryped directories
    REST_NOLOCALMACHINERUN          = 0x40000046,   // ignore HKLM\sw\ms\win\cv\Run and all of it's sub keys
    REST_NOCURRENTUSERRUN           = 0x40000047,   // ignore HKCU\sw\ms\win\cv\Run and all of it's sub keys
    REST_NOLOCALMACHINERUNONCE      = 0x40000048,   // ignore HKLM\sw\ms\win\cv\RunOnce and all of it's sub keys
    REST_NOCURRENTUSERRUNONCE       = 0x40000049,   // ignore HKCU\sw\ms\win\cv\RunOnce and all of it's sub keys
    REST_FORCEACTIVEDESKTOPON       = 0x4000004A,   // Force ActiveDesktop to be turned ON all the time.
    REST_NOCOMPUTERSNEARME          = 0x4000004B,   // removes the "Computers near me" link
    REST_NOVIEWONDRIVE              = 0x4000004C,   // disallows CreateViewObject() on specified drives (CFSFolder only)
    REST_NONETCRAWL                 = 0x4000004D,   // disables the crawling of the WNet namespace.
    REST_NOSHAREDDOCUMENTS          = 0x4000004E,   // don't auto share the Shared Documents/create link
    REST_NOSMMYDOCS                 = 0x4000004F,   // Don't show the My Documents item on the Start Menu.
    REST_NOSMMYPICS                 = 0x40000050,   // Don't show the My Pictures item on the Start Menu
    REST_ALLOWBITBUCKDRIVES         = 0x40000051,   // Bit mask indicating which which drives have bit bucket support
    REST_NONLEGACYSHELLMODE         = 0x40000052,   // new consumer shell modes
    REST_NOCONTROLPANELBARRICADE    = 0x40000053,   // The webview barricade in Control Panel
    REST_NOSTARTPAGE                = 0x40000054,   // Whistler Start Page on desktop.
    REST_NOAUTOTRAYNOTIFY           = 0x40000055,   // Whistler auto-tray notify feature
    REST_NOTASKGROUPING             = 0x40000056,   // Whistler taskbar button grouping feature
    REST_NOCDBURNING                = 0x40000057,   // whistler cd burning feature
    REST_MYCOMPNOPROP               = 0x40000058,   // disables Properties on My Computer's context menu
    REST_MYDOCSNOPROP               = 0x40000059,   // disables Properties on My Documents' context menu
    REST_NOSTARTPANEL               = 0x4000005A,   // Windows start panel (New start menu) for Whistler.
    REST_NODISPLAYAPPEARANCEPAGE    = 0x4000005B,   // disable Themes and Appearance tabs in the Display Control Panel.
    REST_NOTHEMESTAB                = 0x4000005C,   // disable the Themes tab in the Display Control Panel.
    REST_NOVISUALSTYLECHOICE        = 0x4000005D,   // disable the visual style drop down in the Appearance tab of the Display Control Panel.
    REST_NOSIZECHOICE               = 0x4000005E,   // disable the size drop down in the Appearance tab of the Display Control Panel.
    REST_NOCOLORCHOICE              = 0x4000005F,   // disable the color drop down in the Appearance tab of the Display Control Panel.
    REST_SETVISUALSTYLE             = 0x40000060,   // Load the specified file as the visual style.
    REST_STARTRUNNOHOMEPATH         = 0x40000061,   // dont use the %HOMEPATH% env var for the Start-Run dialog
    REST_NOUSERNAMEINSTARTPANEL     = 0x40000062,   // don't show the username is the startpanel.
    REST_NOMYCOMPUTERICON           = 0x40000063,   // don't show my computer anywhere, hide its contents
    REST_NOSMNETWORKPLACES          = 0x40000064,   // don't show network places in startpanel.
    REST_NOSMPINNEDLIST             = 0x40000065,   // don't show the pinned list in startpanel.
    REST_NOSMMYMUSIC                = 0x40000066,   // don't show MyMusic folder in startpanel
    REST_NOSMEJECTPC                = 0x40000067,   // don't show "Undoc PC" command in startmenu
    REST_NOSMMOREPROGRAMS           = 0x40000068,   // don't show "More Programs" button in StartPanel.
    REST_NOSMMFUPROGRAMS            = 0x40000069,   // don't show the MFU programs list in StartPanel.
    REST_NOTRAYITEMSDISPLAY         = 0x4000006A,   // disables the display of the system tray
    REST_NOTOOLBARSONTASKBAR        = 0x4000006B,   // disables toolbar display on the taskbar
    REST_NOSMCONFIGUREPROGRAMS      = 0x4000006F,   // No Configure Programs on Settings Menu
    REST_HIDECLOCK                  = 0x40000070,   // don't show the clock
    REST_NOLOWDISKSPACECHECKS       = 0x40000071,   // disable the low disk space checking
    REST_NOENTIRENETWORK            = 0x40000072,   // removes the "Entire Network" link (i.e. from "My Network Places")
    REST_NODESKTOPCLEANUP           = 0x40000073,   // disable the desktop cleanup wizard
    REST_BITBUCKNUKEONDELETE        = 0x40000074,   // disables recycling of files
    REST_BITBUCKCONFIRMDELETE       = 0x40000075,   // always show the delete confirmation dialog when deleting files
    REST_BITBUCKNOPROP              = 0x40000076,   // disables Properties on Recycle Bin's context menu
    REST_NODISPBACKGROUND           = 0x40000077,   // disables the Desktop tab in the Display CPL
    REST_NODISPSCREENSAVEPG         = 0x40000078,   // disables the Screen Saver tab in the Display CPL
    REST_NODISPSETTINGSPG           = 0x40000079,   // disables the Settings tab in the Display CPL
    REST_NODISPSCREENSAVEPREVIEW    = 0x4000007A,   // disables the screen saver on the Screen Saver tab in the Display CPL
    REST_NODISPLAYCPL               = 0x4000007B,   // disables the Display CPL
    REST_HIDERUNASVERB              = 0x4000007C,   // hides the "Run As..." context menu item
    REST_NOTHUMBNAILCACHE           = 0x4000007D,   // disables use of the thumbnail cache
    REST_NOSTRCMPLOGICAL            = 0x4000007E,   // dont use StrCmpLogical() instead use default CompareString()
    REST_NOPUBLISHWIZARD            = 0x4000007F,   // disables publishing wizard (WPW)
    REST_NOONLINEPRINTSWIZARD       = 0x40000080,   // disables online prints wizard (OPW)
    REST_NOWEBSERVICES              = 0x40000081,   // disables the web specified services for both OPW and WPW
    REST_ALLOWUNHASHEDWEBVIEW       = 0x40000082,   // allow the user to be promted to accept web view templates that don't already have an md5 hash in the registry
    REST_ALLOWLEGACYWEBVIEW         = 0x40000083,   // allow legacy webview template to be shown.
    REST_REVERTWEBVIEWSECURITY      = 0x40000084,   // disable added webview security measures (revert to w2k functionality).
    REST_INHERITCONSOLEHANDLES      = 0x40000086,   // ShellExec() will check for the current process and target process being console processes to inherit handles
    REST_SORTMAXITEMCOUNT           = 0x40000087,   // Do not sort views with more items than this key. Useful for viewing big amount of files in one folder.
    REST_NOREMOTERECURSIVEEVENTS    = 0x40000089,   // Dont register network change events recursively to avoid network traffic
    REST_NOREMOTECHANGENOTIFY       = 0x40000091,   // Do not register for remote change notifies
    REST_NOSIMPLENETIDLIST          = 0x40000092,   // No simple network IDLists
    REST_NOENUMENTIRENETWORK        = 0x40000093,   // Don't enumerate entire network if we happen to get to it (in conjunction with REST_NOENTIRENETWORK)
    REST_NODETAILSTHUMBNAILONNETWORK= 0x40000094,   // Disable Thumbnail for Network files in DUI Details pane
    REST_NOINTERNETOPENWITH         = 0x40000095,   // dont allow looking on the internet for file associations
    REST_ALLOWLEGACYLMZBEHAVIOR     = 0x4000009A,   // allowable LMZ behavior for ActiveX objects changed in XPSP2, this policy gets the pre-XPSP2 behavior
    REST_DONTRETRYBADNETNAME        = 0x4000009B,   // In Network Places: if provider returns ERROR_BAD_NET_NAME, give up
    REST_ALLOWFILECLSIDJUNCTIONS    = 0x4000009C,   // re-enable legacy support for file.{guid} junctions in FileSystem Folder 
    REST_NOUPNPINSTALL              = 0x4000009D,   // disable "install UPnP" task in My Net Places
    
    REST_NODISCONNECT               = 0x41000001,   // No Disconnect option in Start menu
    REST_NOSECURITY                 = 0x41000002,   // No Security option in start menu
    REST_NOFILEASSOCIATE            = 0x41000003,   // Do not allow user to change file association
    REST_ALLOWCOMMENTTOGGLE         = 0x41000004,   // Allow the user to toggle the positions of the Comment and the Computer Name
    REST_USEDESKTOPINICACHE         = 0x41000005,   // Cache desktop.ini entries from network folders
} RESTRICTIONS;
SHSTDAPI_(IStream *) OpenRegStream(HKEY hkey, LPCWSTR pszSubkey, LPCWSTR pszValue, DWORD grfMode);
SHSTDAPI_(BOOL) SHFindFiles(LPCITEMIDLIST pidlFolder, LPCITEMIDLIST pidlSaveFile);
SHSTDAPI_(void) PathGetShortPath(LPWSTR pszLongPath);
SHSTDAPI_(BOOL) PathYetAnotherMakeUniqueName(LPWSTR  pszUniqueName, LPCWSTR pszPath, LPCWSTR pszShort, LPCWSTR pszFileSpec);
SHSTDAPI_(BOOL) Win32DeleteFile(LPCWSTR pszPath);
//
// Path processing function
//
#define PPCF_ADDQUOTES               0x00000001        // return a quoted name if required
#define PPCF_ADDARGUMENTS            0x00000003        // appends arguments (and wraps in quotes if required)
#define PPCF_NODIRECTORIES           0x00000010        // don't match to directories
#define PPCF_FORCEQUALIFY            0x00000040        // qualify even non-relative names
#define PPCF_LONGESTPOSSIBLE         0x00000080        // always find the longest possible name

SHSTDAPI_(LONG) PathProcessCommand(LPCWSTR lpSrc, LPWSTR lpDest, int iMax, DWORD dwFlags);
SHSTDAPI_(DWORD) SHRestricted(RESTRICTIONS rest);
SHSTDAPI_(BOOL) SignalFileOpen(LPCITEMIDLIST pidl);
SHSTDAPI_(LPITEMIDLIST) SHSimpleIDListFromPath(LPCWSTR pszPath);
SHSTDAPI SHLoadOLE(LPARAM lParam);
SHSTDAPI SHStartNetConnectionDialogA(HWND hwnd, LPCSTR pszRemoteName, DWORD dwType);
SHSTDAPI SHStartNetConnectionDialogW(HWND hwnd, LPCWSTR pszRemoteName, DWORD dwType);
#ifdef UNICODE
#define SHStartNetConnectionDialog  SHStartNetConnectionDialogW
#else
#define SHStartNetConnectionDialog  SHStartNetConnectionDialogA
#endif // !UNICODE
SHSTDAPI SHDefExtractIconA(LPCSTR pszIconFile, int iIndex, UINT uFlags,
                           HICON *phiconLarge, HICON *phiconSmall, UINT nIconSize);
SHSTDAPI SHDefExtractIconW(LPCWSTR pszIconFile, int iIndex, UINT uFlags,
                           HICON *phiconLarge, HICON *phiconSmall, UINT nIconSize);
#ifdef UNICODE
#define SHDefExtractIcon  SHDefExtractIconW
#else
#define SHDefExtractIcon  SHDefExtractIconA
#endif // !UNICODE
SHSTDAPI_(BOOL) Shell_GetImageLists(HIMAGELIST *phiml, HIMAGELIST *phimlSmall);
SHSTDAPI_(int)  Shell_GetCachedImageIndex(LPCWSTR pszIconPath, int iIconIndex, UINT uIconFlags);

//
// IDocViewSite
//
#undef  INTERFACE
#define INTERFACE  IDocViewSite
DECLARE_INTERFACE_(IDocViewSite, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)(THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef)(THIS) = 0;
    STDMETHOD_(ULONG,Release)(THIS) = 0;

    // *** IDocViewSite methods ***
    STDMETHOD(OnSetTitle) (THIS_ VARIANTARG *pvTitle) = 0;

} ;
#define VALIDATEUNC_NOUI        0x0002          // don't bring up UI
#define VALIDATEUNC_CONNECT     0x0001          // connect a drive letter
#define VALIDATEUNC_PRINT       0x0004          // validate as print share instead of disk share
#define VALIDATEUNC_VALID       0x0007          // valid flags

SHSTDAPI_(BOOL) SHValidateUNC(HWND hwndOwner, LPWSTR pszFile, UINT fConnect);
#define OPENPROPS_NONE          0x0000
#define OPENPROPS_INHIBITPIF    0x8000
#define GETPROPS_NONE           0x0000
#define SETPROPS_NONE           0x0000
#define CLOSEPROPS_NONE         0x0000		
#define CLOSEPROPS_DISCARD      0x0001		

#define PIFNAMESIZE     30
#define PIFSTARTLOCSIZE 63
#define PIFDEFPATHSIZE  64
#define PIFPARAMSSIZE   64
#define PIFSHPROGSIZE   64
#define PIFSHDATASIZE   64
#define PIFDEFFILESIZE  80
#define PIFMAXFILEPATH  260

typedef struct PROPPRG {                    /* prg */
    WORD    flPrg;                          // see PRG_ flags
    WORD    flPrgInit;                      // see PRGINIT_ flags
    CHAR    achTitle[PIFNAMESIZE];          // name[30]
    CHAR    achCmdLine[PIFSTARTLOCSIZE+PIFPARAMSSIZE+1];// startfile[63] + params[64]
    CHAR    achWorkDir[PIFDEFPATHSIZE];     // defpath[64]
    WORD    wHotKey;                        // PfHotKeyScan thru PfHotKeyVal
    CHAR    achIconFile[PIFDEFFILESIZE];    // name of file containing icon
    WORD    wIconIndex;                     // index of icon within file
    DWORD   dwEnhModeFlags;                 // reserved enh-mode flags
    DWORD   dwRealModeFlags;                // real-mode flags (see RMOPT_*)
    CHAR    achOtherFile[PIFDEFFILESIZE];   // name of "other" file in directory
    CHAR    achPIFFile[PIFMAXFILEPATH];     // name of PIF file
} PROPPRG;
typedef UNALIGNED PROPPRG *PPROPPRG;
typedef UNALIGNED PROPPRG FAR *LPPROPPRG;
typedef const UNALIGNED PROPPRG FAR *LPCPROPPRG;

SHSTDAPI_(HANDLE) PifMgr_OpenProperties(LPCWSTR pszApp, LPCWSTR pszPIF, UINT hInf, UINT flOpt);
SHSTDAPI_(int)    PifMgr_GetProperties(HANDLE hProps, LPCSTR pszGroup, void *lpProps, int cbProps, UINT flOpt);
SHSTDAPI_(int)    PifMgr_SetProperties(HANDLE hProps, LPCSTR pszGroup, const VOID *lpProps, int cbProps, UINT flOpt);
SHSTDAPI_(HANDLE) PifMgr_CloseProperties(HANDLE hProps, UINT flOpt);

SHSTDAPI_(void) SHSetInstanceExplorer(IUnknown *punk);
SHSTDAPI_(BOOL) IsUserAnAdmin(void);

#undef  INTERFACE
#define INTERFACE   IInitializeObject

DECLARE_INTERFACE_(IInitializeObject, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IInitializeObject methods
    STDMETHOD(Initialize)(THIS) = 0;
};


enum
{
    BMICON_LARGE = 0,
    BMICON_SMALL
};

#undef  INTERFACE
#define INTERFACE   IBanneredBar

DECLARE_INTERFACE_(IBanneredBar, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IBanneredBar methods ***
    STDMETHOD(SetIconSize)(THIS_ DWORD iIcon) = 0;
    STDMETHOD(GetIconSize)(THIS_ DWORD* piIcon) = 0;
    STDMETHOD(SetBitmap)(THIS_ HBITMAP hBitmap) = 0;
    STDMETHOD(GetBitmap)(THIS_ HBITMAP* phBitmap) = 0;

};

SHSTDAPI_(LRESULT) SHShellFolderView_Message(HWND hwndMain, UINT uMsg, LPARAM lParam);

//
// Callback interface for the IShellFolderView
//
#undef  INTERFACE
#define INTERFACE   IShellFolderViewCB

DECLARE_INTERFACE_(IShellFolderViewCB, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef) (THIS)  = 0;
    STDMETHOD_(ULONG,Release) (THIS) = 0;

    // *** IShellFolderViewCB methods ***
    STDMETHOD(MessageSFVCB)(THIS_ UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
};


#include <pshpack8.h>

#undef UNSIZED_ARRAY2
#undef EMPTY_SIZE2
#define UNSIZED_ARRAY2
#define EMPTY_SIZE2 UNSIZED_ARRAY2

#define QCMINFO_PLACE_BEFORE    0
#define QCMINFO_PLACE_AFTER     1
typedef struct _QCMINFO_IDMAP_PLACEMENT
{
    UINT id;
    UINT fFlags;
} QCMINFO_IDMAP_PLACEMENT;

typedef struct _QCMINFO_IDMAP
{
    UINT                    nMaxIds;
    QCMINFO_IDMAP_PLACEMENT pIdList[1];
} QCMINFO_IDMAP;

typedef struct _QCMINFO
{
    HMENU       hmenu;          // in
    UINT        indexMenu;      // in
    UINT        idCmdFirst;     // in/out
    UINT        idCmdLast;      // in
    QCMINFO_IDMAP const*  pIdMap; // in / unused
} QCMINFO;
typedef QCMINFO * LPQCMINFO;

// TBINFO flags
#define TBIF_APPEND     0
#define TBIF_PREPEND    1
#define TBIF_REPLACE    2
#define TBIF_DEFAULT      0x00000000
#define TBIF_INTERNETBAR  0x00010000
#define TBIF_STANDARDTOOLBAR   0x00020000
#define TBIF_NOTOOLBAR  0x00030000

typedef struct _TBINFO
{
    UINT        cbuttons;       // out
    UINT        uFlags;         // out (one of TBIF_ flags)
} TBINFO;


typedef struct _DETAILSINFO
{
    LPCITEMIDLIST pidl;
    int fmt;
    int cxChar;
    STRRET str;
    int iImage;
} DETAILSINFO;

typedef struct _SFVM_PROPPAGE_DATA
{
    DWORD                dwReserved;
    LPFNADDPROPSHEETPAGE pfn;
    LPARAM               lParam;
} SFVM_PROPPAGE_DATA;

typedef struct _SFVM_HELPTOPIC_DATA
{
    WCHAR wszHelpFile[MAX_PATH];
    WCHAR wszHelpTopic[MAX_PATH];
} SFVM_HELPTOPIC_DATA;

//                                 uMsg    wParam             lParam
#define SFVM_MERGEMENU             1    // -                  LPQCMINFO
#define SFVM_INVOKECOMMAND         2    // idCmd              -
#define SFVM_GETHELPTEXT           3    // idCmd,cchMax       pszText
#define SFVM_GETTOOLTIPTEXT        4    // idCmd,cchMax       pszText
#define SFVM_GETBUTTONINFO         5    // -                  LPTBINFO
#define SFVM_GETBUTTONS            6    // idCmdFirst,cbtnMax LPTBBUTTON
#define SFVM_INITMENUPOPUP         7    // idCmdFirst,nIndex  hmenu
#define SFVM_FSNOTIFY             14    // LPCITEMIDLIST*     lEvent
#define SFVM_WINDOWCREATED        15    // hwnd               -
#define SFVM_GETDETAILSOF         23    // iColumn            DETAILSINFO*
#define SFVM_COLUMNCLICK          24    // iColumn            -
#define SFVM_QUERYFSNOTIFY        25    // -                  SHChangeNotifyEntry *
#define SFVM_DEFITEMCOUNT         26    // -                  UINT*
#define SFVM_DEFVIEWMODE          27    // -                  FOLDERVIEWMODE*
#define SFVM_UNMERGEMENU          28    // -                  hmenu
#define SFVM_UPDATESTATUSBAR      31    // fInitialize        -
#define SFVM_BACKGROUNDENUM       32    // -                  -
#define SFVM_DIDDRAGDROP          36    // dwEffect           IDataObject *
#define SFVM_SETISFV              39    // -                  IShellFolderView*
#define SFVM_THISIDLIST           41    // -                  LPITMIDLIST*
#define SFVM_ADDPROPERTYPAGES     47    // -                  SFVM_PROPPAGE_DATA *
#define SFVM_BACKGROUNDENUMDONE   48    // -                  -
#define SFVM_GETNOTIFY            49    // LPITEMIDLIST*      LONG*
#define SFVM_GETSORTDEFAULTS      53    // iDirection         iParamSort
#define SFVM_SIZE                 57    // -                  -
#define SFVM_GETZONE              58    // -                  DWORD*
#define SFVM_GETPANE              59    // Pane ID            DWORD*
#define SFVM_GETHELPTOPIC         63    // -                  SFVM_HELPTOPIC_DATA *
#define SFVM_GETANIMATION         68    // HINSTANCE *        WCHAR *
// SHCreateShellFolderView struct
typedef struct _SFV_CREATE
{
    UINT            cbSize;
    IShellFolder*   pshf;
    IShellView*     psvOuter;
    IShellFolderViewCB* psfvcb; // No callback if NULL
} SFV_CREATE;

SHSTDAPI SHCreateShellFolderView(const SFV_CREATE* pcsfv, IShellView **ppsv);
typedef HRESULT (CALLBACK * LPFNDFMCALLBACK)(IShellFolder *psf, HWND hwnd,
                                             IDataObject *pdtobj, UINT uMsg, WPARAM wParam, LPARAM lParam);

SHSTDAPI CDefFolderMenu_Create2(LPCITEMIDLIST pidlFolder, HWND hwnd,
                                UINT cidl, LPCITEMIDLIST *apidl,
                                IShellFolder *psf, LPFNDFMCALLBACK lpfn,
                                UINT nKeys, const HKEY *ahkeyClsKeys,
                                IContextMenu **ppcm);

SHSTDAPI_(BOOL) SHOpenPropSheetA(LPCSTR pszCaption, HKEY ahkeys[], UINT cikeys,
                                 const CLSID * pclsidDefault, IDataObject *pdtobj,
                                 IShellBrowser *psb, LPCSTR pStartPage);
SHSTDAPI_(BOOL) SHOpenPropSheetW(LPCWSTR pszCaption, HKEY ahkeys[], UINT cikeys,
                                 const CLSID * pclsidDefault, IDataObject *pdtobj,
                                 IShellBrowser *psb, LPCWSTR pStartPage);
#ifdef UNICODE
#define SHOpenPropSheet  SHOpenPropSheetW
#else
#define SHOpenPropSheet  SHOpenPropSheetA
#endif // !UNICODE
//                                  uMsg       wParam       lParam
#define DFM_MERGECONTEXTMENU         1      // uFlags       LPQCMINFO
#define DFM_INVOKECOMMAND            2      // idCmd        pszArgs
#define DFM_GETDEFSTATICID           14     // idCmd *      0
// Commands from DFM_INVOKECOMMAND when strings are passed in
#define DFM_CMD_PROPERTIES      ((UINT)-5)
typedef TBINFO * LPTBINFO;

typedef DETAILSINFO *PDETAILSINFO;

typedef HRESULT (CALLBACK * LPFNVIEWCALLBACK)(IShellView *psvOuter,
                                                IShellFolder *psf,
                                                HWND hwndMain,
                                                UINT uMsg,
                                                WPARAM wParam,
                                                LPARAM lParam);

// SHCreateShellFolderViewEx struct
typedef struct _CSFV
{
    UINT            cbSize;
    IShellFolder *  pshf;
    IShellView *    psvOuter;
    LPCITEMIDLIST   pidl;
    LONG            lEvents;
    LPFNVIEWCALLBACK pfnCallback;       // No callback if NULL
    FOLDERVIEWMODE  fvm;
} CSFV, * LPCSFV;


// Tell the FolderView to rearrange.  The lParam will be passed to
// IShellFolder::CompareIDs
#define SFVM_REARRANGE          0x00000001
#define ShellFolderView_ReArrange(_hwnd, _lparam) \
        (BOOL)SHShellFolderView_Message(_hwnd, SFVM_REARRANGE, _lparam)
// Add an OBJECT into the view
#define SFVM_ADDOBJECT         0x00000003
#define ShellFolderView_AddObject(_hwnd, _pidl) \
        (LPARAM)SHShellFolderView_Message(_hwnd, SFVM_ADDOBJECT, (LPARAM)_pidl)
// Remove an OBJECT into the view
#define SFVM_REMOVEOBJECT         0x00000006
#define ShellFolderView_RemoveObject(_hwnd, _pidl) \
        (LPARAM)SHShellFolderView_Message(_hwnd, SFVM_REMOVEOBJECT, (LPARAM)_pidl)

// updates an object by passing in pointer to two PIDLS, the first
// is the old pidl, the second one is the one with update information.
//
// _ppidl[1] must be a *copy* of a pidl, as control over the lifetime
// of the pidl belongs to the view after successful completion of
// this call.  (Unsuccessful completion (a -1 return) implies failure
// and the caller must free the memory.)  Win95 waits a while before
// freeing the pidl, IE4 frees the pidl immediately.
// IShellFolderView::UpdateObject does not suffer from this problem.
//
#define SFVM_UPDATEOBJECT         0x00000007
#define ShellFolderView_UpdateObject(_hwnd, _ppidl) \
        (LPARAM)SHShellFolderView_Message(_hwnd, SFVM_UPDATEOBJECT, (LPARAM)_ppidl)


// Returns an array of the selected IDS to the caller.
//     lparam is a pointer to receive the idlists into
//     return value is the count of items in the array.
#define SFVM_GETSELECTEDOBJECTS 0x00000009
#define ShellFolderView_GetSelectedObjects(_hwnd, ppidl) \
        (LPARAM)SHShellFolderView_Message(_hwnd, SFVM_GETSELECTEDOBJECTS, (LPARAM)ppidl)
typedef struct _SFV_SETITEMPOS
{
        LPCITEMIDLIST pidl;
        POINT pt;
} SFV_SETITEMPOS, *LPSFV_SETITEMPOS;

// Sets the position of an item in the viewer
//     lparam is a pointer to a SVF_SETITEMPOS
//     return value is unused
#define SFVM_SETITEMPOS         0x0000000e
#define ShellFolderView_SetItemPos(_hwnd, _pidl, _x, _y) \
{       SFV_SETITEMPOS _sip = {_pidl, {_x, _y}}; \
        SHShellFolderView_Message(_hwnd, SFVM_SETITEMPOS, (LPARAM)(LPSFV_SETITEMPOS)&_sip);}
//  Notifies a ShellView when one of its objects get put on the clipboard
//  as a result of a menu command.
//
//
//     lparam is the dwEffect (DROPEFFECT_MOVE, DROPEFFECT_COPY)
//     return value is void.
#define SFVM_SETCLIPBOARD       0x00000010
#define ShellFolderView_SetClipboard(_hwnd, _dwEffect) \
        (void)SHShellFolderView_Message(_hwnd, SFVM_SETCLIPBOARD, (LPARAM)(DWORD)(_dwEffect))
#define SFVM_SETPOINTS           0x00000017
#define ShellFolderView_SetPoints(_hwnd, _pdtobj) \
        (void)SHShellFolderView_Message(_hwnd, SFVM_SETPOINTS, (LPARAM)_pdtobj)
#include <poppack.h>        /* Return to byte packing */
SHSTDAPI_(IContextMenu *) SHFind_InitMenuPopup(HMENU hmenu, HWND hwndOwner, UINT idCmdFirst, UINT idCmdLast);
SHSTDAPI SHCreateShellFolderViewEx(LPCSFV pcsfv, IShellView ** ppsv);

//
// PROPIDs for Internet Shortcuts (FMTID_Intshcut) to be used with
// IPropertySetStorage/IPropertyStorage
//
// The known property ids and their variant types are:
//      PID_IS_URL          [VT_LPWSTR]   URL
//      PID_IS_NAME         [VT_LPWSTR]   Name of the internet shortcut
//      PID_IS_WORKINGDIR   [VT_LPWSTR]   Working directory for the shortcut
//      PID_IS_HOTKEY       [VT_UI2]      Hotkey for the shortcut
//      PID_IS_SHOWCMD      [VT_I4]       Show command for shortcut
//      PID_IS_ICONINDEX    [VT_I4]       Index into file that has icon
//      PID_IS_ICONFILE     [VT_LPWSTR]   File that has the icon
//      PID_IS_WHATSNEW     [VT_LPWSTR]   What's New text
//      PID_IS_AUTHOR       [VT_LPWSTR]   Author
//      PID_IS_DESCRIPTION  [VT_LPWSTR]   Description text of site
//      PID_IS_COMMENT      [VT_LPWSTR]   User annotated comment
//

#define PID_IS_URL           2
#define PID_IS_NAME          4
#define PID_IS_WORKINGDIR    5
#define PID_IS_HOTKEY        6
#define PID_IS_SHOWCMD       7
#define PID_IS_ICONINDEX     8
#define PID_IS_ICONFILE      9
#define PID_IS_WHATSNEW      10
#define PID_IS_AUTHOR        11
#define PID_IS_DESCRIPTION   12
#define PID_IS_COMMENT       13

//
// PROPIDs for Internet Sites (FMTID_InternetSite) to be used with
// IPropertySetStorage/IPropertyStorage
//
// The known property ids and their variant types are:
//      PID_INTSITE_WHATSNEW     [VT_LPWSTR]   What's New text
//      PID_INTSITE_AUTHOR       [VT_LPWSTR]   Author
//      PID_INTSITE_LASTVISIT    [VT_FILETIME] Time site was last visited
//      PID_INTSITE_LASTMOD      [VT_FILETIME] Time site was last modified
//      PID_INTSITE_VISITCOUNT   [VT_UI4]      Number of times user has visited
//      PID_INTSITE_DESCRIPTION  [VT_LPWSTR]   Description text of site
//      PID_INTSITE_COMMENT      [VT_LPWSTR]   User annotated comment
//      PID_INTSITE_RECURSE      [VT_UI4]      Levels to recurse (0-3)
//      PID_INTSITE_WATCH        [VT_UI4]      PIDISM_ flags
//      PID_INTSITE_SUBSCRIPTION [VT_UI8]      Subscription cookie
//      PID_INTSITE_URL          [VT_LPWSTR]   URL
//      PID_INTSITE_TITLE        [VT_LPWSTR]   Title
//      PID_INTSITE_CODEPAGE     [VT_UI4]      Codepage of the document
//      PID_INTSITE_TRACKING     [VT_UI4]      Tracking
//      PID_INTSITE_ICONINDEX    [VT_I4]       Retrieve the index to the icon
//      PID_INTSITE_ICONFILE     [VT_LPWSTR]   Retrieve the file containing the icon index.


#define PID_INTSITE_WHATSNEW      2
#define PID_INTSITE_AUTHOR        3
#define PID_INTSITE_LASTVISIT     4
#define PID_INTSITE_LASTMOD       5
#define PID_INTSITE_VISITCOUNT    6
#define PID_INTSITE_DESCRIPTION   7
#define PID_INTSITE_COMMENT       8
#define PID_INTSITE_FLAGS         9
#define PID_INTSITE_CONTENTLEN    10
#define PID_INTSITE_CONTENTCODE   11
#define PID_INTSITE_RECURSE       12
#define PID_INTSITE_WATCH         13
#define PID_INTSITE_SUBSCRIPTION  14
#define PID_INTSITE_URL           15
#define PID_INTSITE_TITLE         16
#define PID_INTSITE_CODEPAGE      18
#define PID_INTSITE_TRACKING      19
#define PID_INTSITE_ICONINDEX     20
#define PID_INTSITE_ICONFILE      21


// Flags for PID_IS_FLAGS
#define PIDISF_RECENTLYCHANGED  0x00000001
#define PIDISF_CACHEDSTICKY     0x00000002
#define PIDISF_CACHEIMAGES      0x00000010
#define PIDISF_FOLLOWALLLINKS   0x00000020

// Values for PID_INTSITE_WATCH
#define PIDISM_GLOBAL           0       // Monitor based on global setting
#define PIDISM_WATCH            1       // User says watch
#define PIDISM_DONTWATCH        2       // User says don't watch


////////////////////////////////////////////////////////////////////
//
// The shell keeps track of some per-user state to handle display
// options that is of major interest to ISVs.
// The key one requested right now is "DoubleClickInWebView".

typedef struct {
    BOOL fShowAllObjects : 1;
    BOOL fShowExtensions : 1;
    BOOL fNoConfirmRecycle : 1;

    BOOL fShowSysFiles : 1;
    BOOL fShowCompColor : 1;
    BOOL fDoubleClickInWebView : 1;
    BOOL fDesktopHTML : 1;
    BOOL fWin95Classic : 1;
    BOOL fDontPrettyPath : 1;
    BOOL fShowAttribCol : 1; // No longer used, dead bit
    BOOL fMapNetDrvBtn : 1;
    BOOL fShowInfoTip : 1;
    BOOL fHideIcons : 1;
    BOOL fWebView : 1;
    BOOL fFilter : 1;
    BOOL fShowSuperHidden : 1;
    BOOL fNoNetCrawling : 1;

    DWORD dwWin95Unused; // Win95 only - no longer supported pszHiddenFileExts
    UINT  uWin95Unused; // Win95 only - no longer supported cbHiddenFileExts

    // Note: Not a typo!  This is a persisted structure so we cannot use LPARAM
    LONG   lParamSort;
    int    iSortDirection;

    UINT   version;

    // new for win2k. need notUsed var to calc the right size of ie4 struct
    // FIELD_OFFSET does not work on bit fields
    UINT uNotUsed; // feel free to rename and use
    BOOL fSepProcess: 1;

    // new for Whistler.
    BOOL fStartPanelOn: 1;       //Indicates if the Whistler StartPanel mode is ON or OFF.

    BOOL fShowStartPage: 1;      //Indicates if the Whistler StartPage on desktop is ON or OFF.

    UINT fSpareFlags : 13;

} SHELLSTATEA, *LPSHELLSTATEA;

typedef struct {
    BOOL fShowAllObjects : 1;
    BOOL fShowExtensions : 1;
    BOOL fNoConfirmRecycle : 1;
    BOOL fShowSysFiles : 1;
    BOOL fShowCompColor : 1;
    BOOL fDoubleClickInWebView : 1;
    BOOL fDesktopHTML : 1;
    BOOL fWin95Classic : 1;
    BOOL fDontPrettyPath : 1;
    BOOL fShowAttribCol : 1;
    BOOL fMapNetDrvBtn : 1;
    BOOL fShowInfoTip : 1;
    BOOL fHideIcons : 1;
    BOOL fWebView : 1;
    BOOL fFilter : 1;
    BOOL fShowSuperHidden : 1;
    BOOL fNoNetCrawling : 1;

    DWORD dwWin95Unused; // Win95 only - no longer supported pszHiddenFileExts
    UINT  uWin95Unused; // Win95 only - no longer supported cbHiddenFileExts

    // Note: Not a typo!  This is a persisted structure so we cannot use LPARAM
    LONG   lParamSort;
    int    iSortDirection;
    UINT   version;

    // new for win2k. need notUsed var to calc the right size of ie4 struct
    // FIELD_OFFSET does not work on bit fields
    UINT uNotUsed; // feel free to rename and use
    BOOL fSepProcess: 1;

    // new for Whistler.
    BOOL fStartPanelOn: 1;       //Indicates if the Whistler StartPage mode is ON or OFF.

    BOOL fShowStartPage: 1;      //Indicates if the Whistler StartPage on desktop is ON or OFF.

    // If you need a new flag, steal a bit from from fSpareFlags.
    UINT fSpareFlags : 13;


} SHELLSTATEW, *LPSHELLSTATEW;

#define SHELLSTATEVERSION_IE4   9
#define SHELLSTATEVERSION_WIN2K 10

#ifdef UNICODE
#define SHELLSTATE   SHELLSTATEW
#define LPSHELLSTATE LPSHELLSTATEW
#else
#define SHELLSTATE   SHELLSTATEA
#define LPSHELLSTATE LPSHELLSTATEA
#endif

#define SHELLSTATE_SIZE_WIN95 FIELD_OFFSET(SHELLSTATE,lParamSort)
#define SHELLSTATE_SIZE_NT4   FIELD_OFFSET(SHELLSTATE,version)
#define SHELLSTATE_SIZE_IE4   FIELD_OFFSET(SHELLSTATE,uNotUsed)
#define SHELLSTATE_SIZE_WIN2K sizeof(SHELLSTATE)

SHSTDAPI_(void) SHGetSetSettings(LPSHELLSTATE lpss, DWORD dwMask, BOOL bSet);

//
//  SysFiles are these windows special files:
//      "dll sys vxd 386 drv"
//
//  hidden files are files with the FILE_ATTRIBUTE_HIDDEN attribute
//
//  system files are files with the FILE_ATTRIBUTE_SYSTEM attribute
//
//      fShowAllObjects fShowSysFiles   Result
//      --------------- -------------   ------
//      0               0               hide hidden + SysFiles + system files
//      0               1               hide hidden files.
//      1               0               show all files.
//      1               1               show all files.
//
typedef struct {
    BOOL fShowAllObjects : 1;
    BOOL fShowExtensions : 1;
    BOOL fNoConfirmRecycle : 1;
    BOOL fShowSysFiles : 1;
    BOOL fShowCompColor : 1;
    BOOL fDoubleClickInWebView : 1;
    BOOL fDesktopHTML : 1;
    BOOL fWin95Classic : 1;
    BOOL fDontPrettyPath : 1;
    BOOL fShowAttribCol : 1;
    BOOL fMapNetDrvBtn : 1;
    BOOL fShowInfoTip : 1;
    BOOL fHideIcons : 1;
    UINT fRestFlags : 3;
} SHELLFLAGSTATE, *LPSHELLFLAGSTATE;

#define SSF_SHOWALLOBJECTS          0x00000001
#define SSF_SHOWEXTENSIONS          0x00000002
#define SSF_HIDDENFILEEXTS          0x00000004
#define SSF_SERVERADMINUI           0x00000004
#define SSF_SHOWCOMPCOLOR           0x00000008
#define SSF_SORTCOLUMNS             0x00000010
#define SSF_SHOWSYSFILES            0x00000020
#define SSF_DOUBLECLICKINWEBVIEW    0x00000080
#define SSF_SHOWATTRIBCOL           0x00000100
#define SSF_DESKTOPHTML             0x00000200
#define SSF_WIN95CLASSIC            0x00000400
#define SSF_DONTPRETTYPATH          0x00000800
#define SSF_SHOWINFOTIP             0x00002000
#define SSF_MAPNETDRVBUTTON         0x00001000
#define SSF_NOCONFIRMRECYCLE        0x00008000
#define SSF_HIDEICONS               0x00004000
#define SSF_FILTER                  0x00010000
#define SSF_WEBVIEW                 0x00020000
#define SSF_SHOWSUPERHIDDEN         0x00040000
#define SSF_SEPPROCESS              0x00080000
#define SSF_NONETCRAWLING           0x00100000
#define SSF_STARTPANELON            0x00200000
#define SSF_SHOWSTARTPAGE           0x00400000

// SHGetSettings(LPSHELLFLAGSTATE lpss, DWORD dwMask)
//
// Specify the bits you are interested in in dwMask and they will be
// filled out in the lpss structure.
//
// When these settings change, a WM_SETTINGCHANGE message is sent
// with the string lParam value of "ShellState".
//
SHSTDAPI_(void) SHGetSettings(LPSHELLFLAGSTATE lpsfs, DWORD dwMask);

// SHBindToParent(LPCITEMIDLIST pidl, REFIID riid, void **ppv, LPCITEMIDLIST *ppidlLast)
//
// Given a pidl, you can get an interface pointer (as specified by riid) of the pidl's parent folder (in ppv)
// If ppidlLast is non-NULL, you can also get the pidl of the last item.
//
SHSTDAPI SHBindToParent(LPCITEMIDLIST pidl, REFIID riid, void **ppv, LPCITEMIDLIST *ppidlLast);

// SHSTDAPI SHParseDisplayName(PCWSTR pszName, IBindCtx *pbc, LPITEMIDLIST *ppidl, SFGAOF sfgaoIn, SFGAOF *psfgaoOut)
//
//  given a string it will call psfDesktop->ParseDisplayName() to try and create a pidl
//  if no pbc specified, it uses the preferred options for parsing.
//  this includes mapping file system paths to their appropriate aliased location (RegisterObjectParam(STR_PARSE_TRANSLATE_ALIASES))
//  psfgaoOut is optional for SFGAO attributes
//
SHSTDAPI SHParseDisplayName(PCWSTR pszName, IBindCtx *pbc, LPITEMIDLIST *ppidl, SFGAOF sfgaoIn, SFGAOF *psfgaoOut);


// SHPathPrepareForWrite(HWND hwnd, IUnknown *punkEnableModless, LPCTSTR pszPath, DWORD dwFlags)
//
// DESCRIPTION:
//     This API will prepare the path for the caller.  This includes:
// 1. Prompting for the ejectable media to be re-inserted. (Floppy, CD-ROM, ZIP drive, etc.)
// 2. Prompting for the media to be formatted. (Floppy, hard drive, etc.)
// 3. Remount mapped drives if the connection was lost. (\\unc\share mapped to N: becomes disconnected)
// 4. If the path doesn't exist, create it.  (SHPPFW_DIRCREATE and SHPPFW_ASKDIRCREATE)
// 5. Display an error if the media is read only. (SHPPFW_NOWRITECHECK not set)
//
// PARAMETERS:
//      hwnd: Parernt window for UI.  NULL means don't display UI. OPTIONAL
//      punkEnableModless: Parent that will be set to modal during UI using IOleInPlaceActiveObject::EnableModeless(). OPTIONAL
//      pszPath: Path to verify is valid for writting.  This can be a UNC or file drive path.  The path
//               should only contain directories.  Pass SHPPFW_IGNOREFILENAME if the last path segment
//               is always filename to ignore.
//      dwFlags: SHPPFW_* Flags to modify behavior
//
//-------------------------------------------------------------------------
#define SHPPFW_NONE             0x00000000
#define SHPPFW_DEFAULT          SHPPFW_DIRCREATE        // May change
#define SHPPFW_DIRCREATE        0x00000001              // Create the directory if it doesn't exist without asking the user.
#define SHPPFW_ASKDIRCREATE     0x00000002              // Create the directory if it doesn't exist after asking the user.
#define SHPPFW_IGNOREFILENAME   0x00000004              // Ignore the last item in pszPath because it's a file.  Example: pszPath="C:\DirA\DirB", only use "C:\DirA".
#define SHPPFW_NOWRITECHECK     0x00000008              // Caller only needs to read from the drive, so don't check if it's READ ONLY.
#define SHPPFW_MEDIACHECKONLY   0x00000010              // do the retrys on the media (or net path), return errors if the file can't be found

SHSTDAPI SHPathPrepareForWriteA(HWND hwnd, IUnknown *punkEnableModless, LPCSTR pszPath, DWORD dwFlags);
SHSTDAPI SHPathPrepareForWriteW(HWND hwnd, IUnknown *punkEnableModless, LPCWSTR pszPath, DWORD dwFlags);
#ifdef UNICODE
#define SHPathPrepareForWrite  SHPathPrepareForWriteW
#else
#define SHPathPrepareForWrite  SHPathPrepareForWriteA
#endif // !UNICODE





//--------------------------------------------------------------------------
//
// Interface used for exposing the INI file methods on a shortcut file
//
//
//--------------------------------------------------------------------------
#undef  INTERFACE
#define INTERFACE  INamedPropertyBag
DECLARE_INTERFACE_(INamedPropertyBag, IUnknown)
{
    // *** IUnknown methods ***
    STDMETHOD(QueryInterface)   (THIS_ REFIID riid, void **ppv) = 0;
    STDMETHOD_(ULONG,AddRef)    (THIS) = 0;
    STDMETHOD_(ULONG,Release)   (THIS) = 0;
    // *** INamedPropertyBag methods ***
    STDMETHOD(ReadPropertyNPB) (THIS_  /* [in] */ LPCOLESTR pszBagname,
                            /* [in] */ LPCOLESTR pszPropName,
                            /* [out][in] */ PROPVARIANT *pVar) = 0;

    STDMETHOD(WritePropertyNPB)(THIS_ /* [in] */ LPCOLESTR pszBagname,
                           /* [in] */ LPCOLESTR pszPropName,
                           /* [in] */ PROPVARIANT  *pVar) = 0;

    STDMETHOD(RemovePropertyNPB)(THIS_ /* [in] */ LPCOLESTR pszBagname,
                          /* [in] */ LPCOLESTR pszPropName) = 0;
};

#ifdef __urlmon_h__
//    NOTE: urlmon.h must be included before shlobj.h to access this function.
//
//    SoftwareUpdateMessageBox
//
//    Provides a standard message box for the alerting the user that a software
//    update is available or installed. No UI will be displayed if there is no
//    update available or if the available update version is less than or equal
//    to the Advertised update version.
//
//    hWnd                - [in] Handle of owner window
//    szDistUnit          - [in] Unique identifier string for a code distribution unit. For
//                               ActiveX controls and Active Setup installed components, this
//                               is typically a GUID string.
//    dwFlags             - [in] Must be 0.
//    psdi                - [in,out] Pointer to SOFTDISTINFO ( see URLMon.h ). May be NULL.
//                                cbSize should be initialized
//                                by the caller to sizeof(SOFTDISTINFO), dwReserved should be set to 0.
//
//    RETURNS:
//
//    IDNO     - The user chose cancel. If *pbRemind is FALSE, the caller should save the
//               update version from the SOFTDISTINFO and pass it in as the Advertised
//               version in future calls.
//
//    IDYES    - The user has selected Update Now/About Update. The caller should navigate to
//               the SOFTDISTINFO's pszHREF to initiate the install or learn about it.
//               The caller should save the update version from the SOFTDISTINFO and pass
//               it in as the Advertised version in future calls.
//
//    IDIGNORE - There is no pending software update. Note: There is
//               no Ignore button in the standard UI. This occurs if the available
//               version is less than the installed version or is not present or if the
//               Advertised version is greater than or equal to the update version.
//
//    IDABORT  - An error occured. Call GetSoftwareUpdateInfo() for a more specific HRESULT.
//               Note: There is no Abort button in the standard UI.


SHDOCAPI_(DWORD) SoftwareUpdateMessageBox( HWND hWnd,
                                           LPCWSTR szDistUnit,
                                           DWORD dwFlags,
                                           LPSOFTDISTINFO psdi );
#endif // if __urlmon_h__


//  SHPropStgCreate()
//  Wrap of IPropertySetStorage::Open/Create
//
//  This function ensures proper handling of code page retrieval/assignment
//  for the requested property set operation.
//
//  psstg,          //  Address of IPropertySetStorage vtable
//  fmtid,          //  property set ID
//  pclsid,         //  class ID associated with the set. This can be NULL
//  grfFlags,       //  PROPSETFLAG_xxx.  All sets containing ansi bytes should be created with
                    //  PROPSETFLAG_ANSI, otherwise PROPSETFLAG_DEFAULT.
//  grfMode,        //  STGM_ flags.  Must contain STGM_DIRECT|STGM_EXCLUSIVE.
//  dwDisposition,  //  OPEN_EXISTING. OPEN_ALWAYS, CREATE_NEW, or CREATE_ALWAYS
//  IPropertyStorage** ppstg,  // Address to receive requested vtable
//  puCodePage      //  Optional address to receive the code page ID for the set.
//
SHSTDAPI SHPropStgCreate( IPropertySetStorage* psstg, REFFMTID fmtid, CLSID* pclsid, DWORD grfFlags, DWORD grfMode, DWORD dwDisposition, OUT IPropertyStorage** ppstg, OUT OPTIONAL UINT* puCodePage );


//  SHPropStgReadMultiple()
//  IPropertyStorage::ReadMultiple wrap
//
//  The wrap ensures ANSI/UNICODE translations are handled properly for
//  legacy property sets.
//
//  pps,       // address of IPropertyStorage vtable.
//  uCodePage, //Code page value retrieved from SHCreatePropertySet
//  cpspec,    //Count of properties being read
//  rgpspec,   //Array of the properties to be read
//  rgvar      //Array of PROPVARIANTs containing the property values on return
//
SHSTDAPI SHPropStgReadMultiple( IPropertyStorage* pps, UINT uCodePage, ULONG cpspec, PROPSPEC const rgpspec[], PROPVARIANT rgvar[] );


//  SHPropStgWriteMultiple()
//  IPropertyStorage::WriteMultiple wrap
//
//  The wrap ensures ANSI/UNICODE translations are handled properly for
//  legacy property sets.
//
//  pps,       // address of IPropertyStorage vtable.
//  uCodePage, // code page retrieved from SHCreatePropertySet.
//  cpspec,    // The number of properties being set
//  rgpspec,   // Property specifiers
//  rgvar,     // Array of PROPVARIANT values
//  propidNameFirst // Minimum value for property identifiers. This value should be >= PID_FIRST_USABLE
//
SHSTDAPI SHPropStgWriteMultiple( IPropertyStorage* pps, UINT* puCodePage, ULONG cpspec, PROPSPEC const rgpspec[], PROPVARIANT rgvar[], PROPID propidNameFirst );

SHSTDAPI SHCreateFileExtractIconA(LPCSTR pszFile, DWORD dwFileAttributes, REFIID riid, void **ppv);
SHSTDAPI SHCreateFileExtractIconW(LPCWSTR pszFile, DWORD dwFileAttributes, REFIID riid, void **ppv);
#ifdef UNICODE
#define SHCreateFileExtractIcon  SHCreateFileExtractIconW
#else
#define SHCreateFileExtractIcon  SHCreateFileExtractIconA
#endif // !UNICODE
#include <pshpack8.h>
SHSTDAPI SHLimitInputEdit(HWND hwndEdit, IShellFolder *psf);

#if (_WIN32_WINNT >= 0x0500) || (_WIN32_WINDOWS >= 0x0500)  

//
// The SHMultiFileProperties API displays a property sheet for a 
// set of files specified in an IDList Array.
//
// Parameters:
//      pdtobj  - Data object containing list of files.  The data
//                object must provide the "Shell IDList Array" 
//                clipboard format.  The parent folder's implementation of
//                IShellFolder::GetDisplayNameOf must return a fully-qualified
//                filesystem path for each item in response to the 
//                SHGDN_FORPARSING flag.
//
//      dwFlags - Reserved for future use.  Should be set to 0.
//
// Returns:
//      S_OK
// 
SHSTDAPI SHMultiFileProperties(IDataObject *pdtobj, DWORD dwFlags);

#endif 

#if (_WIN32_IE >= 0x0600)
//
//
typedef void (CALLBACK *PFNASYNCICONTASKBALLBACK)(LPCITEMIDLIST pidl, LPVOID pvData, LPVOID pvHint, INT iIconIndex, INT iOpenIconIndex);

// HRESULT SHMapIDListToImageListIndexAsync(IShellTaskScheduler* pts, IShellFolder *psf, LPCITEMIDLIST pidl, UINT flags,
//                                            PFNASYNCICONTASKBALLBACK pfn, LPVOID pvData, LPVOID pvHint, int *piIndex, int *piIndexSel);
// A usefull function for asynchronously mapping idlist into index into system
// image list.  Optionally it can also look up the index of the selected icon.
// pts          Task scheduler interface to use to create the background task
// psf          Shell folder relating to the pidl
// pidl         Item whose icon is requested
// flags        GIL_ flags
// pfn          Function called back when the background task is done
// pvData       User data passed back in the (*pfn) callback
// pvHint       User data passed back in the (*pfn) callback
// piIndex      Icon index returned. This is the temporary index if the function returns E_PENDING. The final index will be provided thru the callback
// piIndexSel   Optional icon index for the open icon case (GIL_OPENICON).
//
// Returns S_OK if all the requested info was available. E_PENDING means that you get temporary icons, and will be called back
//              asynchronously with the final icons. Other failure code means the function failed.
SHSTDAPI SHMapIDListToImageListIndexAsync(IShellTaskScheduler* pts, IShellFolder *psf, LPCITEMIDLIST pidl, UINT flags,
                                            PFNASYNCICONTASKBALLBACK pfn, LPVOID pvData, LPVOID pvHint, int *piIndex, int *piIndexSel);
#endif // (_WIN32_IE >= 0x0600)

// A usefull function in Defview for mapping idlist into index into system
// image list.  Optionally it can also look up the index of the selected
// icon.
SHSTDAPI_(int) SHMapPIDLToSystemImageListIndex(IShellFolder *pshf, LPCITEMIDLIST pidl, int *piIndexSel);


STDAPI SHCLSIDFromString(PCWSTR lpsz, CLSID *pclsid);
STDAPI SHFlushClipboard(void);
STDAPI SHCreateQueryCancelAutoPlayMoniker(IMoniker** ppmoniker);
HINSTANCE WINAPI SHGetShellStyleHInstance(void);
STDAPI_(void) PerUserInit(void);
WINSHELLAPI BOOL    WINAPI SHRunControlPanel(LPCWSTR lpcszCmdLine, HWND hwndMsgParent);

WINSHELLAPI int   WINAPI PickIconDlg(HWND hwnd, LPWSTR pszIconPath, UINT cbIconPath, int *piIconIndex);


typedef struct tagAAMENUFILENAME
{
  SHORT  cbTotal;
  BYTE   rgbReserved[12];
  WCHAR  szFileName[1]; 	// variable length string
} AASHELLMENUFILENAME, *LPAASHELLMENUFILENAME;

typedef struct tagAASHELLMENUITEM
{
  void*  lpReserved1;
  int    iReserved;
  UINT   uiReserved;
  LPAASHELLMENUFILENAME lpName; // name of file
  LPWSTR psz; 			// text to use if no file
} AASHELLMENUITEM, *LPAASHELLMENUITEM;


STDAPI SHGetAttributesFromDataObject(IDataObject *pdo, DWORD dwAttributeMask, DWORD *pdwAttributes, UINT *pcItems);

#include <poppack.h>        /* Return to byte packing */

#ifdef __cplusplus
}

#endif  /* __cplusplus */

#include <poppack.h>


SHDOCAPI_(BOOL) ImportPrivacySettings( LPCWSTR szFilename,
                            IN OUT BOOL* pfParsePrivacyPreferences, IN OUT BOOL* pfParsePerSiteRules);
#ifndef IEnumPrivacyRecords
typedef interface IEnumPrivacyRecords IEnumPrivacyRecords;
#endif
SHDOCAPI DoPrivacyDlg( HWND hwndParent, LPOLESTR pszUrl, IEnumPrivacyRecords *pPrivacyEnum, BOOL fReportAllSites);


#endif // _SHLOBJ_H_

