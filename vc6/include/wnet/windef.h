/****************************************************************************
*                                                                           *
* windef.h -- Basic Windows Type Definitions                                *
*                                                                           *
* Copyright (c) Microsoft Corporation. All rights reserved.                 *
*                                                                           *
****************************************************************************/


#ifndef _WINDEF_
#define _WINDEF_

#ifndef NO_STRICT
#ifndef STRICT
#define STRICT 1
#endif
#endif /* NO_STRICT */

// Win32 defines _WIN32 automatically,
// but Macintosh doesn't, so if we are using
// Win32 Functions, we must do it here

#ifdef _MAC
#ifndef _WIN32
#define _WIN32
#endif
#endif //_MAC

#ifndef WIN32
#define WIN32
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WINVER
#define WINVER 0x0500
#endif /* WINVER */

/*
 * BASETYPES is defined in ntdef.h if these types are already defined
 */

#ifndef BASETYPES
#define BASETYPES
typedef unsigned long ULONG;
typedef ULONG *PULONG;
typedef unsigned short USHORT;
typedef USHORT *PUSHORT;
typedef unsigned char UCHAR;
typedef UCHAR *PUCHAR;
typedef char *PSZ;
#endif  /* !BASETYPES */

#define MAX_PATH          260

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#ifndef FALSE
#define FALSE               0
#endif

#ifndef TRUE
#define TRUE                1
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef OPTIONAL
#define OPTIONAL
#endif

#undef far
#undef near
#undef pascal

#define far
#define near
#if (!defined(_MAC)) && ((_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED))
#define pascal __stdcall
#else
#define pascal
#endif

#if defined(DOSWIN32) || defined(_MAC)
#define cdecl _cdecl
#ifndef CDECL
#define CDECL _cdecl
#endif
#else
#define cdecl
#ifndef CDECL
#define CDECL
#endif
#endif

#ifdef _MAC
#define CALLBACK    PASCAL
#define WINAPI      CDECL
#define WINAPIV     CDECL
#define APIENTRY    WINAPI
#define APIPRIVATE  CDECL
#ifdef _68K_
#define PASCAL      __pascal
#else
#define PASCAL
#endif
#elif (_MSC_VER >= 800) || defined(_STDCALL_SUPPORTED)
#define CALLBACK    __stdcall
#define WINAPI      __stdcall
#define WINAPIV     __cdecl
#define APIENTRY    WINAPI
#define APIPRIVATE  __stdcall
#define PASCAL      __stdcall
#else
#define CALLBACK
#define WINAPI
#define WINAPIV
#define APIENTRY    WINAPI
#define APIPRIVATE
#define PASCAL      pascal
#endif

#ifdef _M_CEE_PURE
#define WINAPI_INLINE  __clrcall
#else
#define WINAPI_INLINE  WINAPI
#endif

#undef FAR
#undef  NEAR
#define FAR                 far
#define NEAR                near
#ifndef CONST
#define CONST               const
#endif

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL near           *PBOOL;
typedef BOOL far            *LPBOOL;
typedef BYTE near           *PBYTE;
typedef BYTE far            *LPBYTE;
typedef int near            *PINT;
typedef int far             *LPINT;
typedef WORD near           *PWORD;
typedef WORD far            *LPWORD;
typedef long far            *LPLONG;
typedef DWORD near          *PDWORD;
typedef DWORD far           *LPDWORD;
typedef void far            *LPVOID;
typedef CONST void far      *LPCVOID;

typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;

#ifndef NT_INCLUDED
#include <winnt.h>
#endif /* NT_INCLUDED */

#include <specstrings.h>

/* Types use for passing & returning polymorphic values */
typedef UINT_PTR            WPARAM;
typedef LONG_PTR            LPARAM;
typedef LONG_PTR            LRESULT;

#ifndef NOMINMAX

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif  /* NOMINMAX */

#define MAKEWORD(a, b)      ((WORD)(((BYTE)((DWORD_PTR)(a) & 0xff)) | ((WORD)((BYTE)((DWORD_PTR)(b) & 0xff))) << 8))
#define MAKELONG(a, b)      ((LONG)(((WORD)((DWORD_PTR)(a) & 0xffff)) | ((DWORD)((WORD)((DWORD_PTR)(b) & 0xffff))) << 16))
#define LOWORD(l)           ((WORD)((DWORD_PTR)(l) & 0xffff))
#define HIWORD(l)           ((WORD)((DWORD_PTR)(l) >> 16))
#define LOBYTE(w)           ((BYTE)((DWORD_PTR)(w) & 0xff))
#define HIBYTE(w)           ((BYTE)((DWORD_PTR)(w) >> 8))


#ifndef WIN_INTERNAL
DECLARE_HANDLE            (HWND);
DECLARE_HANDLE            (HHOOK);
#ifdef WINABLE
DECLARE_HANDLE            (HEVENT);
#endif
#endif

typedef WORD                ATOM;

typedef HANDLE NEAR         *SPHANDLE;
typedef HANDLE FAR          *LPHANDLE;
typedef HANDLE              HGLOBAL;
typedef HANDLE              HLOCAL;
typedef HANDLE              GLOBALHANDLE;
typedef HANDLE              LOCALHANDLE;
#ifndef _MAC
#ifdef _WIN64
typedef INT_PTR (FAR WINAPI *FARPROC)();
typedef INT_PTR (NEAR WINAPI *NEARPROC)();
typedef INT_PTR (WINAPI *PROC)();
#else
typedef int (FAR WINAPI *FARPROC)();
typedef int (NEAR WINAPI *NEARPROC)();
typedef int (WINAPI *PROC)();
#endif  // _WIN64
#else
typedef int (CALLBACK *FARPROC)();
typedef int (CALLBACK *NEARPROC)();
typedef int (CALLBACK *PROC)();
#endif

#if !defined(_MAC) || !defined(GDI_INTERNAL)
#ifdef STRICT
typedef void NEAR* HGDIOBJ;
#else
DECLARE_HANDLE(HGDIOBJ);
#endif
#endif

DECLARE_HANDLE(HKEY);
typedef HKEY *PHKEY;

#if !defined(_MAC) || !defined(WIN_INTERNAL)
DECLARE_HANDLE(HACCEL);
#endif
#if !defined(_MAC) || !defined(GDI_INTERNAL)
DECLARE_HANDLE(HBITMAP);
DECLARE_HANDLE(HBRUSH);
#endif
#if(WINVER >= 0x0400)
DECLARE_HANDLE(HCOLORSPACE);
#endif /* WINVER >= 0x0400 */
#if !defined(_MAC) || !defined(GDI_INTERNAL)
DECLARE_HANDLE(HDC);
#endif
DECLARE_HANDLE(HGLRC);          // OpenGL
DECLARE_HANDLE(HDESK);
DECLARE_HANDLE(HENHMETAFILE);
#if !defined(_MAC) || !defined(GDI_INTERNAL)
DECLARE_HANDLE(HFONT);
#endif
DECLARE_HANDLE(HICON);
#if !defined(_MAC) || !defined(WIN_INTERNAL)
DECLARE_HANDLE(HMENU);
#endif
DECLARE_HANDLE(HMETAFILE);
DECLARE_HANDLE(HINSTANCE);
typedef HINSTANCE HMODULE;      /* HMODULEs can be used in place of HINSTANCEs */
#if !defined(_MAC) || !defined(GDI_INTERNAL)
DECLARE_HANDLE(HPALETTE);
DECLARE_HANDLE(HPEN);
#endif
DECLARE_HANDLE(HRGN);
DECLARE_HANDLE(HRSRC);
DECLARE_HANDLE(HSTR);
DECLARE_HANDLE(HTASK);
DECLARE_HANDLE(HWINSTA);
DECLARE_HANDLE(HKL);

#if(WINVER >= 0x0500)
#ifndef _MAC
DECLARE_HANDLE(HMONITOR);
DECLARE_HANDLE(HWINEVENTHOOK);
#endif
DECLARE_HANDLE(HUMPD);
#endif /* WINVER >= 0x0500 */

#ifndef _MAC
typedef int HFILE;
typedef HICON HCURSOR;      /* HICONs & HCURSORs are polymorphic */
#else
typedef short HFILE;
DECLARE_HANDLE(HCURSOR);    /* HICONs & HCURSORs are not polymorphic */
#endif

typedef DWORD   COLORREF;
typedef DWORD   *LPCOLORREF;

#define HFILE_ERROR ((HFILE)-1)

typedef struct tagRECT
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECT, *PRECT, NEAR *NPRECT, FAR *LPRECT;

typedef const RECT FAR* LPCRECT;

typedef struct _RECTL       /* rcl */
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECTL, *PRECTL, *LPRECTL;

typedef const RECTL FAR* LPCRECTL;

typedef struct tagPOINT
{
    LONG  x;
    LONG  y;
} POINT, *PPOINT, NEAR *NPPOINT, FAR *LPPOINT;

typedef struct _POINTL      /* ptl  */
{
    LONG  x;
    LONG  y;
} POINTL, *PPOINTL;

typedef struct tagSIZE
{
    LONG        cx;
    LONG        cy;
} SIZE, *PSIZE, *LPSIZE;

typedef SIZE               SIZEL;
typedef SIZE               *PSIZEL, *LPSIZEL;

typedef struct tagPOINTS
{
#ifndef _MAC
    SHORT   x;
    SHORT   y;
#else
    SHORT   y;
    SHORT   x;
#endif
} POINTS, *PPOINTS, *LPPOINTS;

//
//  File System time stamps are represented with the following structure:
//

typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;
#define _FILETIME_


/* mode selections for the device mode function */
#define DM_UPDATE           1
#define DM_COPY             2
#define DM_PROMPT           4
#define DM_MODIFY           8

#define DM_IN_BUFFER        DM_MODIFY
#define DM_IN_PROMPT        DM_PROMPT
#define DM_OUT_BUFFER       DM_COPY
#define DM_OUT_DEFAULT      DM_UPDATE

/* device capabilities indices */
#define DC_FIELDS           1
#define DC_PAPERS           2
#define DC_PAPERSIZE        3
#define DC_MINEXTENT        4
#define DC_MAXEXTENT        5
#define DC_BINS             6
#define DC_DUPLEX           7
#define DC_SIZE             8
#define DC_EXTRA            9
#define DC_VERSION          10
#define DC_DRIVER           11
#define DC_BINNAMES         12
#define DC_ENUMRESOLUTIONS  13
#define DC_FILEDEPENDENCIES 14
#define DC_TRUETYPE         15
#define DC_PAPERNAMES       16
#define DC_ORIENTATION      17
#define DC_COPIES           18

#ifdef __cplusplus
}
#endif

#endif /* _WINDEF_ */


