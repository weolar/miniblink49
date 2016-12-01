

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* Compiler settings for wtypes.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __wtypes_h__
#define __wtypes_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

/* header files for imported files */
#include "basetsd.h"
#include "guiddef.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_wtypes_0000 */
/* [local] */ 

//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (c) Microsoft Corporation. All rights reserved.
//
//--------------------------------------------------------------------------
#if ( _MSC_VER >= 1020 )
#pragma once
#endif


extern RPC_IF_HANDLE __MIDL_itf_wtypes_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wtypes_0000_v0_0_s_ifspec;

#ifndef __IWinTypes_INTERFACE_DEFINED__
#define __IWinTypes_INTERFACE_DEFINED__

/* interface IWinTypes */
/* [auto_handle][unique][version][uuid] */ 

typedef struct tagRemHGLOBAL
    {
    long fNullHGlobal;
    unsigned long cbData;
    /* [size_is] */ byte data[ 1 ];
    } 	RemHGLOBAL;

typedef struct tagRemHMETAFILEPICT
    {
    long mm;
    long xExt;
    long yExt;
    unsigned long cbData;
    /* [size_is] */ byte data[ 1 ];
    } 	RemHMETAFILEPICT;

typedef struct tagRemHENHMETAFILE
    {
    unsigned long cbData;
    /* [size_is] */ byte data[ 1 ];
    } 	RemHENHMETAFILE;

typedef struct tagRemHBITMAP
    {
    unsigned long cbData;
    /* [size_is] */ byte data[ 1 ];
    } 	RemHBITMAP;

typedef struct tagRemHPALETTE
    {
    unsigned long cbData;
    /* [size_is] */ byte data[ 1 ];
    } 	RemHPALETTE;

typedef struct tagRemBRUSH
    {
    unsigned long cbData;
    /* [size_is] */ byte data[ 1 ];
    } 	RemHBRUSH;

#if !defined(_WIN32) && !defined(_MPPC_)
// The following code is for Win16 only
#ifndef WINAPI          // If not included with 3.1 headers...
#define FAR             _far
#define PASCAL          _pascal
#define CDECL           _cdecl
#define VOID            void
#define WINAPI      FAR PASCAL
#define CALLBACK    FAR PASCAL
#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif // !FALSE
#ifndef _BYTE_DEFINED
#define _BYTE_DEFINED
typedef byte BYTE;

#endif // !_BYTE_DEFINED
#ifndef _WORD_DEFINED
#define _WORD_DEFINED
typedef unsigned short WORD;

#endif // !_WORD_DEFINED
typedef unsigned int UINT;

typedef int INT;

typedef long BOOL;

#ifndef _LONG_DEFINED
#define _LONG_DEFINED
typedef long LONG;

#endif // !_LONG_DEFINED
#ifndef _WPARAM_DEFINED
#define _WPARAM_DEFINED
typedef UINT_PTR WPARAM;

#endif // _WPARAM_DEFINED
#ifndef _DWORD_DEFINED
#define _DWORD_DEFINED
typedef unsigned long DWORD;

#endif // !_DWORD_DEFINED
#ifndef _LPARAM_DEFINED
#define _LPARAM_DEFINED
typedef LONG_PTR LPARAM;

#endif // !_LPARAM_DEFINED
#ifndef _LRESULT_DEFINED
#define _LRESULT_DEFINED
typedef LONG_PTR LRESULT;

#endif // !_LRESULT_DEFINED
typedef void *HANDLE;

typedef void *HMODULE;

typedef void *HINSTANCE;

typedef void *HRGN;

typedef void *HTASK;

typedef void *HKEY;

typedef void *HDESK;

typedef void *HMF;

typedef void *HEMF;

typedef void *HPEN;

typedef void *HRSRC;

typedef void *HSTR;

typedef void *HWINSTA;

typedef void *HKL;

typedef void *HGDIOBJ;

typedef HANDLE HDWP;

#ifndef _HFILE_DEFINED
#define _HFILE_DEFINED
typedef INT HFILE;

#endif // !_HFILE_DEFINED
#ifndef _LPWORD_DEFINED
#define _LPWORD_DEFINED
typedef WORD *LPWORD;

#endif // !_LPWORD_DEFINED
#ifndef _LPDWORD_DEFINED
#define _LPDWORD_DEFINED
typedef DWORD *LPDWORD;

#endif // !_LPDWORD_DEFINED
typedef char CHAR;

typedef /* [string] */ CHAR *LPSTR;

typedef /* [string] */ const CHAR *LPCSTR;

#ifndef _WCHAR_DEFINED
#define _WCHAR_DEFINED
typedef wchar_t WCHAR;

typedef WCHAR TCHAR;

#endif // !_WCHAR_DEFINED
typedef /* [string] */ WCHAR *LPWSTR;

typedef /* [string] */ TCHAR *LPTSTR;

typedef /* [string] */ const WCHAR *LPCWSTR;

typedef /* [string] */ const TCHAR *LPCTSTR;

#ifndef _COLORREF_DEFINED
#define _COLORREF_DEFINED
typedef DWORD COLORREF;

#endif // !_COLORREF_DEFINED
#ifndef _LPCOLORREF_DEFINED
#define _LPCOLORREF_DEFINED
typedef DWORD *LPCOLORREF;

#endif // !_LPCOLORREF_DEFINED
typedef HANDLE *LPHANDLE;

typedef struct _RECTL
    {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
    } 	RECTL;

typedef struct _RECTL *PRECTL;

typedef struct _RECTL *LPRECTL;

typedef struct tagPOINT
    {
    LONG x;
    LONG y;
    } 	POINT;

typedef struct tagPOINT *PPOINT;

typedef struct tagPOINT *LPPOINT;

typedef struct _POINTL
    {
    LONG x;
    LONG y;
    } 	POINTL;

typedef struct _POINTL *PPOINTL;

#ifndef WIN16
typedef struct tagSIZE
    {
    LONG cx;
    LONG cy;
    } 	SIZE;

typedef struct tagSIZE *PSIZE;

typedef struct tagSIZE *LPSIZE;

#else // WIN16
typedef struct tagSIZE
{
    INT cx;
    INT cy;
} SIZE, *PSIZE, *LPSIZE;
#endif // WIN16
typedef struct tagSIZEL
    {
    LONG cx;
    LONG cy;
    } 	SIZEL;

typedef struct tagSIZEL *PSIZEL;

typedef struct tagSIZEL *LPSIZEL;

#endif  //WINAPI
#endif  //!WIN32 && !MPPC
#ifndef _PALETTEENTRY_DEFINED
#define _PALETTEENTRY_DEFINED
typedef struct tagPALETTEENTRY
    {
    BYTE peRed;
    BYTE peGreen;
    BYTE peBlue;
    BYTE peFlags;
    } 	PALETTEENTRY;

typedef struct tagPALETTEENTRY *PPALETTEENTRY;

typedef struct tagPALETTEENTRY *LPPALETTEENTRY;

#endif // !_PALETTEENTRY_DEFINED
#ifndef _LOGPALETTE_DEFINED
#define _LOGPALETTE_DEFINED
typedef struct tagLOGPALETTE
    {
    WORD palVersion;
    WORD palNumEntries;
    /* [size_is] */ PALETTEENTRY palPalEntry[ 1 ];
    } 	LOGPALETTE;

typedef struct tagLOGPALETTE *PLOGPALETTE;

typedef struct tagLOGPALETTE *LPLOGPALETTE;

#endif // !_LOGPALETTE_DEFINED
#if defined(_WIN32) && !defined(OLE2ANSI)
typedef WCHAR OLECHAR;

typedef /* [string] */ OLECHAR *LPOLESTR;

typedef /* [string] */ const OLECHAR *LPCOLESTR;

#define OLESTR(str) L##str

#else

typedef char      OLECHAR;
typedef LPSTR     LPOLESTR;
typedef LPCSTR    LPCOLESTR;
#define OLESTR(str) str
#endif
#ifndef _WINDEF_
typedef const RECTL *LPCRECTL;

typedef void *PVOID;

typedef void *LPVOID;

typedef float FLOAT;

typedef struct tagRECT
    {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
    } 	RECT;

typedef struct tagRECT *PRECT;

typedef struct tagRECT *LPRECT;

typedef const RECT *LPCRECT;

#endif  //_WINDEF_
typedef unsigned char UCHAR;

typedef short SHORT;

typedef unsigned short USHORT;

typedef DWORD ULONG;

typedef double DOUBLE;

#ifndef _DWORDLONG_
typedef unsigned __int64 DWORDLONG;

typedef DWORDLONG *PDWORDLONG;

#endif // !_DWORDLONG_
#ifndef _ULONGLONG_
typedef __int64 LONGLONG;

typedef unsigned __int64 ULONGLONG;

typedef LONGLONG *PLONGLONG;

typedef ULONGLONG *PULONGLONG;

#endif // _ULONGLONG_
#if 0
typedef struct _LARGE_INTEGER
    {
    LONGLONG QuadPart;
    } 	LARGE_INTEGER;

typedef LARGE_INTEGER *PLARGE_INTEGER;

typedef struct _ULARGE_INTEGER
    {
    ULONGLONG QuadPart;
    } 	ULARGE_INTEGER;

#endif // 0
#ifndef _WINBASE_
#ifndef _FILETIME_
#define _FILETIME_
typedef struct _FILETIME
    {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
    } 	FILETIME;

typedef struct _FILETIME *PFILETIME;

typedef struct _FILETIME *LPFILETIME;

#endif // !_FILETIME
#ifndef _SYSTEMTIME_
#define _SYSTEMTIME_
typedef struct _SYSTEMTIME
    {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
    } 	SYSTEMTIME;

typedef struct _SYSTEMTIME *PSYSTEMTIME;

typedef struct _SYSTEMTIME *LPSYSTEMTIME;

#endif // !_SYSTEMTIME
#ifndef _SECURITY_ATTRIBUTES_
#define _SECURITY_ATTRIBUTES_
typedef struct _SECURITY_ATTRIBUTES
    {
    DWORD nLength;
    /* [size_is] */ LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
    } 	SECURITY_ATTRIBUTES;

typedef struct _SECURITY_ATTRIBUTES *PSECURITY_ATTRIBUTES;

typedef struct _SECURITY_ATTRIBUTES *LPSECURITY_ATTRIBUTES;

#endif // !_SECURITY_ATTRIBUTES_
#ifndef SECURITY_DESCRIPTOR_REVISION
typedef USHORT SECURITY_DESCRIPTOR_CONTROL;

typedef USHORT *PSECURITY_DESCRIPTOR_CONTROL;

typedef PVOID PSID;

typedef struct _ACL
    {
    UCHAR AclRevision;
    UCHAR Sbz1;
    USHORT AclSize;
    USHORT AceCount;
    USHORT Sbz2;
    } 	ACL;

typedef ACL *PACL;

typedef struct _SECURITY_DESCRIPTOR
    {
    UCHAR Revision;
    UCHAR Sbz1;
    SECURITY_DESCRIPTOR_CONTROL Control;
    PSID Owner;
    PSID Group;
    PACL Sacl;
    PACL Dacl;
    } 	SECURITY_DESCRIPTOR;

typedef struct _SECURITY_DESCRIPTOR *PISECURITY_DESCRIPTOR;

#endif // !SECURITY_DESCRIPTOR_REVISION
#endif //_WINBASE_
typedef struct _COAUTHIDENTITY
    {
    /* [size_is] */ USHORT *User;
    /* [range] */ ULONG UserLength;
    /* [size_is] */ USHORT *Domain;
    /* [range] */ ULONG DomainLength;
    /* [size_is] */ USHORT *Password;
    /* [range] */ ULONG PasswordLength;
    ULONG Flags;
    } 	COAUTHIDENTITY;

typedef struct _COAUTHINFO
    {
    DWORD dwAuthnSvc;
    DWORD dwAuthzSvc;
    LPWSTR pwszServerPrincName;
    DWORD dwAuthnLevel;
    DWORD dwImpersonationLevel;
    COAUTHIDENTITY *pAuthIdentityData;
    DWORD dwCapabilities;
    } 	COAUTHINFO;

typedef LONG SCODE;

#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef LONG HRESULT;

#endif // !_HRESULT_DEFINED
typedef SCODE *PSCODE;

#ifndef __OBJECTID_DEFINED
#define __OBJECTID_DEFINED
#define _OBJECTID_DEFINED
typedef struct _OBJECTID
    {
    GUID Lineage;
    unsigned long Uniquifier;
    } 	OBJECTID;

#endif // !_OBJECTID_DEFINED
#if 0
typedef GUID *REFGUID;

typedef IID *REFIID;

typedef CLSID *REFCLSID;

typedef FMTID *REFFMTID;

#endif // 0
typedef 
enum tagMEMCTX
    {	MEMCTX_TASK	= 1,
	MEMCTX_SHARED	= 2,
	MEMCTX_MACSYSTEM	= 3,
	MEMCTX_UNKNOWN	= -1,
	MEMCTX_SAME	= -2
    } 	MEMCTX;

#ifndef _ROTFLAGS_DEFINED
#define _ROTFLAGS_DEFINED
#define ROTFLAGS_REGISTRATIONKEEPSALIVE 0x1
#define ROTFLAGS_ALLOWANYCLIENT 0x2
#endif // !_ROTFLAGS_DEFINED
#ifndef _ROT_COMPARE_MAX_DEFINED
#define _ROT_COMPARE_MAX_DEFINED
#define ROT_COMPARE_MAX 2048
#endif // !_ROT_COMPARE_MAX_DEFINED
typedef 
enum tagCLSCTX
    {	CLSCTX_INPROC_SERVER	= 0x1,
	CLSCTX_INPROC_HANDLER	= 0x2,
	CLSCTX_LOCAL_SERVER	= 0x4,
	CLSCTX_INPROC_SERVER16	= 0x8,
	CLSCTX_REMOTE_SERVER	= 0x10,
	CLSCTX_INPROC_HANDLER16	= 0x20,
	CLSCTX_RESERVED1	= 0x40,
	CLSCTX_RESERVED2	= 0x80,
	CLSCTX_RESERVED3	= 0x100,
	CLSCTX_RESERVED4	= 0x200,
	CLSCTX_NO_CODE_DOWNLOAD	= 0x400,
	CLSCTX_RESERVED5	= 0x800,
	CLSCTX_NO_CUSTOM_MARSHAL	= 0x1000,
	CLSCTX_ENABLE_CODE_DOWNLOAD	= 0x2000,
	CLSCTX_NO_FAILURE_LOG	= 0x4000,
	CLSCTX_DISABLE_AAA	= 0x8000,
	CLSCTX_ENABLE_AAA	= 0x10000,
	CLSCTX_FROM_DEFAULT_CONTEXT	= 0x20000,
	CLSCTX_ACTIVATE_32_BIT_SERVER	= 0x40000,
	CLSCTX_ACTIVATE_64_BIT_SERVER	= 0x80000
    } 	CLSCTX;

typedef 
enum tagMSHLFLAGS
    {	MSHLFLAGS_NORMAL	= 0,
	MSHLFLAGS_TABLESTRONG	= 1,
	MSHLFLAGS_TABLEWEAK	= 2,
	MSHLFLAGS_NOPING	= 4,
	MSHLFLAGS_RESERVED1	= 8,
	MSHLFLAGS_RESERVED2	= 16,
	MSHLFLAGS_RESERVED3	= 32,
	MSHLFLAGS_RESERVED4	= 64
    } 	MSHLFLAGS;

typedef 
enum tagMSHCTX
    {	MSHCTX_LOCAL	= 0,
	MSHCTX_NOSHAREDMEM	= 1,
	MSHCTX_DIFFERENTMACHINE	= 2,
	MSHCTX_INPROC	= 3,
	MSHCTX_CROSSCTX	= 4
    } 	MSHCTX;

typedef 
enum tagDVASPECT
    {	DVASPECT_CONTENT	= 1,
	DVASPECT_THUMBNAIL	= 2,
	DVASPECT_ICON	= 4,
	DVASPECT_DOCPRINT	= 8
    } 	DVASPECT;

typedef 
enum tagSTGC
    {	STGC_DEFAULT	= 0,
	STGC_OVERWRITE	= 1,
	STGC_ONLYIFCURRENT	= 2,
	STGC_DANGEROUSLYCOMMITMERELYTODISKCACHE	= 4,
	STGC_CONSOLIDATE	= 8
    } 	STGC;

typedef 
enum tagSTGMOVE
    {	STGMOVE_MOVE	= 0,
	STGMOVE_COPY	= 1,
	STGMOVE_SHALLOWCOPY	= 2
    } 	STGMOVE;

typedef 
enum tagSTATFLAG
    {	STATFLAG_DEFAULT	= 0,
	STATFLAG_NONAME	= 1,
	STATFLAG_NOOPEN	= 2
    } 	STATFLAG;

typedef /* [context_handle] */ void *HCONTEXT;

#ifndef _LCID_DEFINED
#define _LCID_DEFINED
typedef DWORD LCID;

#endif // !_LCID_DEFINED
#ifndef _LANGID_DEFINED
#define _LANGID_DEFINED
typedef USHORT LANGID;

#endif // !_LANGID_DEFINED
typedef struct _BYTE_BLOB
    {
    unsigned long clSize;
    /* [size_is] */ byte abData[ 1 ];
    } 	BYTE_BLOB;

typedef /* [unique] */ BYTE_BLOB *UP_BYTE_BLOB;

typedef struct _WORD_BLOB
    {
    unsigned long clSize;
    /* [size_is] */ unsigned short asData[ 1 ];
    } 	WORD_BLOB;

typedef /* [unique] */ WORD_BLOB *UP_WORD_BLOB;

typedef struct _DWORD_BLOB
    {
    unsigned long clSize;
    /* [size_is] */ unsigned long alData[ 1 ];
    } 	DWORD_BLOB;

typedef /* [unique] */ DWORD_BLOB *UP_DWORD_BLOB;

typedef struct _FLAGGED_BYTE_BLOB
    {
    unsigned long fFlags;
    unsigned long clSize;
    /* [size_is] */ byte abData[ 1 ];
    } 	FLAGGED_BYTE_BLOB;

typedef /* [unique] */ FLAGGED_BYTE_BLOB *UP_FLAGGED_BYTE_BLOB;

typedef struct _FLAGGED_WORD_BLOB
    {
    unsigned long fFlags;
    unsigned long clSize;
    /* [size_is] */ unsigned short asData[ 1 ];
    } 	FLAGGED_WORD_BLOB;

typedef /* [unique] */ FLAGGED_WORD_BLOB *UP_FLAGGED_WORD_BLOB;

typedef struct _BYTE_SIZEDARR
    {
    unsigned long clSize;
    /* [size_is] */ byte *pData;
    } 	BYTE_SIZEDARR;

typedef struct _SHORT_SIZEDARR
    {
    unsigned long clSize;
    /* [size_is] */ unsigned short *pData;
    } 	WORD_SIZEDARR;

typedef struct _LONG_SIZEDARR
    {
    unsigned long clSize;
    /* [size_is] */ unsigned long *pData;
    } 	DWORD_SIZEDARR;

typedef struct _HYPER_SIZEDARR
    {
    unsigned long clSize;
    /* [size_is] */ hyper *pData;
    } 	HYPER_SIZEDARR;

#define	WDT_INPROC_CALL	( 0x48746457 )

#define	WDT_REMOTE_CALL	( 0x52746457 )

#define	WDT_INPROC64_CALL	( 0x50746457 )

typedef struct _userCLIPFORMAT
    {
    long fContext;
    /* [switch_is] */ /* [switch_type] */ union __MIDL_IWinTypes_0001
        {
        /* [case()] */ DWORD dwValue;
        /* [case()][string] */ wchar_t *pwszName;
        } 	u;
    } 	userCLIPFORMAT;

typedef /* [unique] */ userCLIPFORMAT *wireCLIPFORMAT;

typedef /* [wire_marshal] */ WORD CLIPFORMAT;

typedef struct _GDI_NONREMOTE
    {
    long fContext;
    /* [switch_is] */ /* [switch_type] */ union __MIDL_IWinTypes_0002
        {
        /* [case()] */ long hInproc;
        /* [case()] */ DWORD_BLOB *hRemote;
        } 	u;
    } 	GDI_NONREMOTE;

typedef struct _userHGLOBAL
    {
    long fContext;
    /* [switch_is] */ /* [switch_type] */ union __MIDL_IWinTypes_0003
        {
        /* [case()] */ long hInproc;
        /* [case()] */ FLAGGED_BYTE_BLOB *hRemote;
        /* [case()] */ __int64 hInproc64;
        } 	u;
    } 	userHGLOBAL;

typedef /* [unique] */ userHGLOBAL *wireHGLOBAL;

typedef struct _userHMETAFILE
    {
    long fContext;
    /* [switch_is] */ /* [switch_type] */ union __MIDL_IWinTypes_0004
        {
        /* [case()] */ long hInproc;
        /* [case()] */ BYTE_BLOB *hRemote;
        /* [case()] */ __int64 hInproc64;
        } 	u;
    } 	userHMETAFILE;

typedef struct _remoteMETAFILEPICT
    {
    long mm;
    long xExt;
    long yExt;
    userHMETAFILE *hMF;
    } 	remoteMETAFILEPICT;

typedef struct _userHMETAFILEPICT
    {
    long fContext;
    /* [switch_is] */ /* [switch_type] */ union __MIDL_IWinTypes_0005
        {
        /* [case()] */ long hInproc;
        /* [case()] */ remoteMETAFILEPICT *hRemote;
        /* [case()] */ __int64 hInproc64;
        } 	u;
    } 	userHMETAFILEPICT;

typedef struct _userHENHMETAFILE
    {
    long fContext;
    /* [switch_is] */ /* [switch_type] */ union __MIDL_IWinTypes_0006
        {
        /* [case()] */ long hInproc;
        /* [case()] */ BYTE_BLOB *hRemote;
        /* [case()] */ __int64 hInproc64;
        } 	u;
    } 	userHENHMETAFILE;

typedef struct _userBITMAP
    {
    LONG bmType;
    LONG bmWidth;
    LONG bmHeight;
    LONG bmWidthBytes;
    WORD bmPlanes;
    WORD bmBitsPixel;
    ULONG cbSize;
    /* [size_is] */ byte pBuffer[ 1 ];
    } 	userBITMAP;

typedef struct _userHBITMAP
    {
    long fContext;
    /* [switch_is] */ /* [switch_type] */ union __MIDL_IWinTypes_0007
        {
        /* [case()] */ long hInproc;
        /* [case()] */ userBITMAP *hRemote;
        /* [case()] */ __int64 hInproc64;
        } 	u;
    } 	userHBITMAP;

typedef struct _userHPALETTE
    {
    long fContext;
    /* [switch_is] */ /* [switch_type] */ union __MIDL_IWinTypes_0008
        {
        /* [case()] */ long hInproc;
        /* [case()] */ LOGPALETTE *hRemote;
        /* [case()] */ __int64 hInproc64;
        } 	u;
    } 	userHPALETTE;

typedef struct _RemotableHandle
    {
    long fContext;
    /* [switch_is] */ /* [switch_type] */ union __MIDL_IWinTypes_0009
        {
        /* [case()] */ long hInproc;
        /* [case()] */ long hRemote;
        } 	u;
    } 	RemotableHandle;

typedef /* [unique] */ RemotableHandle *wireHWND;

typedef /* [unique] */ RemotableHandle *wireHMENU;

typedef /* [unique] */ RemotableHandle *wireHACCEL;

typedef /* [unique] */ RemotableHandle *wireHBRUSH;

typedef /* [unique] */ RemotableHandle *wireHFONT;

typedef /* [unique] */ RemotableHandle *wireHDC;

typedef /* [unique] */ RemotableHandle *wireHICON;

#if 0
typedef /* [wire_marshal] */ void *HWND;

typedef /* [wire_marshal] */ void *HMENU;

typedef /* [wire_marshal] */ void *HACCEL;

typedef /* [wire_marshal] */ void *HBRUSH;

typedef /* [wire_marshal] */ void *HFONT;

typedef /* [wire_marshal] */ void *HDC;

typedef /* [wire_marshal] */ void *HICON;

#ifndef _HCURSOR_DEFINED
#define _HCURSOR_DEFINED
typedef HICON HCURSOR;

#endif // !_HCURSOR_DEFINED
#endif //0
#ifndef _TEXTMETRIC_DEFINED
#define _TEXTMETRIC_DEFINED
typedef struct tagTEXTMETRICW
    {
    LONG tmHeight;
    LONG tmAscent;
    LONG tmDescent;
    LONG tmInternalLeading;
    LONG tmExternalLeading;
    LONG tmAveCharWidth;
    LONG tmMaxCharWidth;
    LONG tmWeight;
    LONG tmOverhang;
    LONG tmDigitizedAspectX;
    LONG tmDigitizedAspectY;
    WCHAR tmFirstChar;
    WCHAR tmLastChar;
    WCHAR tmDefaultChar;
    WCHAR tmBreakChar;
    BYTE tmItalic;
    BYTE tmUnderlined;
    BYTE tmStruckOut;
    BYTE tmPitchAndFamily;
    BYTE tmCharSet;
    } 	TEXTMETRICW;

typedef struct tagTEXTMETRICW *PTEXTMETRICW;

typedef struct tagTEXTMETRICW *LPTEXTMETRICW;

#endif // !_TEXTMETRIC_DEFINED
#ifndef _WIN32           // The following code is for Win16 only
#ifndef WINAPI          // If not included with 3.1 headers...
typedef struct tagMSG
    {
    HWND hwnd;
    UINT message;
    WPARAM wParam;
    LPARAM lParam;
    DWORD time;
    POINT pt;
    } 	MSG;

typedef struct tagMSG *PMSG;

typedef struct tagMSG *NPMSG;

typedef struct tagMSG *LPMSG;

#endif // _WIN32
#endif // WINAPI
typedef /* [unique] */ userHBITMAP *wireHBITMAP;

typedef /* [unique] */ userHPALETTE *wireHPALETTE;

typedef /* [unique] */ userHENHMETAFILE *wireHENHMETAFILE;

typedef /* [unique] */ userHMETAFILE *wireHMETAFILE;

typedef /* [unique] */ userHMETAFILEPICT *wireHMETAFILEPICT;

#if 0
typedef /* [wire_marshal] */ void *HGLOBAL;

typedef HGLOBAL HLOCAL;

typedef /* [wire_marshal] */ void *HBITMAP;

typedef /* [wire_marshal] */ void *HPALETTE;

typedef /* [wire_marshal] */ void *HENHMETAFILE;

typedef /* [wire_marshal] */ void *HMETAFILE;

#endif //0
typedef /* [wire_marshal] */ void *HMETAFILEPICT;



extern RPC_IF_HANDLE IWinTypes_v0_1_c_ifspec;
extern RPC_IF_HANDLE IWinTypes_v0_1_s_ifspec;
#endif /* __IWinTypes_INTERFACE_DEFINED__ */

/* interface __MIDL_itf_wtypes_0003 */
/* [local] */ 

#if ( _MSC_VER >= 800 )
#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4201)
#endif
typedef double DATE;

#ifndef _tagCY_DEFINED
#define _tagCY_DEFINED
#define _CY_DEFINED
#if 0
/* the following isn't the real definition of CY, but it is */
/* what RPC knows how to remote */
typedef struct tagCY
    {
    LONGLONG int64;
    } 	CY;

#else /* 0 */
/* real definition that makes the C++ compiler happy */
typedef union tagCY {
    struct {
#ifdef _MAC
        long      Hi;
        long Lo;
#else
        unsigned long Lo;
        long      Hi;
#endif
    };
    LONGLONG int64;
} CY;
#endif /* 0 */
#endif /* _tagCY_DEFINED */
typedef CY *LPCY;

#if 0 /* _tagDEC_DEFINED */
/* The following isn't the real definition of Decimal type, */
/* but it is what RPC knows how to remote */
typedef struct tagDEC
    {
    USHORT wReserved;
    BYTE scale;
    BYTE sign;
    ULONG Hi32;
    ULONGLONG Lo64;
    } 	DECIMAL;

#else /* _tagDEC_DEFINED */
/* real definition that makes the C++ compiler happy */
typedef struct tagDEC {
    USHORT wReserved;
    union {
        struct {
            BYTE scale;
            BYTE sign;
        };
        USHORT signscale;
    };
    ULONG Hi32;
    union {
        struct {
#ifdef _MAC
            ULONG Mid32;
            ULONG Lo32;
#else
            ULONG Lo32;
            ULONG Mid32;
#endif
        };
        ULONGLONG Lo64;
    };
} DECIMAL;
#define DECIMAL_NEG ((BYTE)0x80)
#define DECIMAL_SETZERO(dec) \
        {(dec).Lo64 = 0; (dec).Hi32 = 0; (dec).signscale = 0;}
#endif /* _tagDEC_DEFINED */
typedef DECIMAL *LPDECIMAL;

#if ( _MSC_VER >= 800 )
#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4201)
#endif
#endif
typedef /* [unique] */ FLAGGED_WORD_BLOB *wireBSTR;

typedef /* [wire_marshal] */ OLECHAR *BSTR;

typedef BSTR *LPBSTR;

/* 0 == FALSE, -1 == TRUE */
typedef short VARIANT_BOOL;

#if !__STDC__ && (_MSC_VER <= 1000)
/* For backward compatibility */
typedef VARIANT_BOOL _VARIANT_BOOL;

#else
/* ANSI C/C++ reserve bool as keyword */
#define _VARIANT_BOOL    /##/
#endif
typedef boolean BOOLEAN;

/* The BSTRBLOB structure is used by some implementations */
/* of the IPropertyStorage interface when marshaling BSTRs */
/* on systems which don't support BSTR marshaling. */
#ifndef _tagBSTRBLOB_DEFINED
#define _tagBSTRBLOB_DEFINED
typedef struct tagBSTRBLOB
    {
    ULONG cbSize;
    /* [size_is] */ BYTE *pData;
    } 	BSTRBLOB;

typedef struct tagBSTRBLOB *LPBSTRBLOB;

#endif
#define VARIANT_TRUE ((VARIANT_BOOL)-1)
#define VARIANT_FALSE ((VARIANT_BOOL)0)
#ifndef _tagBLOB_DEFINED
#define _tagBLOB_DEFINED
#define _BLOB_DEFINED
#define _LPBLOB_DEFINED
typedef struct tagBLOB
    {
    ULONG cbSize;
    /* [size_is] */ BYTE *pBlobData;
    } 	BLOB;

typedef struct tagBLOB *LPBLOB;

#endif
typedef struct tagCLIPDATA
    {
    ULONG cbSize;
    long ulClipFmt;
    /* [size_is] */ BYTE *pClipData;
    } 	CLIPDATA;

// Macro to calculate the size of the above pClipData
#define CBPCLIPDATA(clipdata)    ( (clipdata).cbSize - sizeof((clipdata).ulClipFmt) )
typedef unsigned short VARTYPE;

/*
 * VARENUM usage key,
 *
 * * [V] - may appear in a VARIANT
 * * [T] - may appear in a TYPEDESC
 * * [P] - may appear in an OLE property set
 * * [S] - may appear in a Safe Array
 *
 *
 *  VT_EMPTY            [V]   [P]     nothing
 *  VT_NULL             [V]   [P]     SQL style Null
 *  VT_I2               [V][T][P][S]  2 byte signed int
 *  VT_I4               [V][T][P][S]  4 byte signed int
 *  VT_R4               [V][T][P][S]  4 byte real
 *  VT_R8               [V][T][P][S]  8 byte real
 *  VT_CY               [V][T][P][S]  currency
 *  VT_DATE             [V][T][P][S]  date
 *  VT_BSTR             [V][T][P][S]  OLE Automation string
 *  VT_DISPATCH         [V][T]   [S]  IDispatch *
 *  VT_ERROR            [V][T][P][S]  SCODE
 *  VT_BOOL             [V][T][P][S]  True=-1, False=0
 *  VT_VARIANT          [V][T][P][S]  VARIANT *
 *  VT_UNKNOWN          [V][T]   [S]  IUnknown *
 *  VT_DECIMAL          [V][T]   [S]  16 byte fixed point
 *  VT_RECORD           [V]   [P][S]  user defined type
 *  VT_I1               [V][T][P][s]  signed char
 *  VT_UI1              [V][T][P][S]  unsigned char
 *  VT_UI2              [V][T][P][S]  unsigned short
 *  VT_UI4              [V][T][P][S]  unsigned long
 *  VT_I8                  [T][P]     signed 64-bit int
 *  VT_UI8                 [T][P]     unsigned 64-bit int
 *  VT_INT              [V][T][P][S]  signed machine int
 *  VT_UINT             [V][T]   [S]  unsigned machine int
 *  VT_INT_PTR             [T]        signed machine register size width
 *  VT_UINT_PTR            [T]        unsigned machine register size width
 *  VT_VOID                [T]        C style void
 *  VT_HRESULT             [T]        Standard return type
 *  VT_PTR                 [T]        pointer type
 *  VT_SAFEARRAY           [T]        (use VT_ARRAY in VARIANT)
 *  VT_CARRAY              [T]        C style array
 *  VT_USERDEFINED         [T]        user defined type
 *  VT_LPSTR               [T][P]     null terminated string
 *  VT_LPWSTR              [T][P]     wide null terminated string
 *  VT_FILETIME               [P]     FILETIME
 *  VT_BLOB                   [P]     Length prefixed bytes
 *  VT_STREAM                 [P]     Name of the stream follows
 *  VT_STORAGE                [P]     Name of the storage follows
 *  VT_STREAMED_OBJECT        [P]     Stream contains an object
 *  VT_STORED_OBJECT          [P]     Storage contains an object
 *  VT_VERSIONED_STREAM       [P]     Stream with a GUID version
 *  VT_BLOB_OBJECT            [P]     Blob contains an object 
 *  VT_CF                     [P]     Clipboard format
 *  VT_CLSID                  [P]     A Class ID
 *  VT_VECTOR                 [P]     simple counted array
 *  VT_ARRAY            [V]           SAFEARRAY*
 *  VT_BYREF            [V]           void* for local use
 *  VT_BSTR_BLOB                      Reserved for system use
 */

enum VARENUM
    {	VT_EMPTY	= 0,
	VT_NULL	= 1,
	VT_I2	= 2,
	VT_I4	= 3,
	VT_R4	= 4,
	VT_R8	= 5,
	VT_CY	= 6,
	VT_DATE	= 7,
	VT_BSTR	= 8,
	VT_DISPATCH	= 9,
	VT_ERROR	= 10,
	VT_BOOL	= 11,
	VT_VARIANT	= 12,
	VT_UNKNOWN	= 13,
	VT_DECIMAL	= 14,
	VT_I1	= 16,
	VT_UI1	= 17,
	VT_UI2	= 18,
	VT_UI4	= 19,
	VT_I8	= 20,
	VT_UI8	= 21,
	VT_INT	= 22,
	VT_UINT	= 23,
	VT_VOID	= 24,
	VT_HRESULT	= 25,
	VT_PTR	= 26,
	VT_SAFEARRAY	= 27,
	VT_CARRAY	= 28,
	VT_USERDEFINED	= 29,
	VT_LPSTR	= 30,
	VT_LPWSTR	= 31,
	VT_RECORD	= 36,
	VT_INT_PTR	= 37,
	VT_UINT_PTR	= 38,
	VT_FILETIME	= 64,
	VT_BLOB	= 65,
	VT_STREAM	= 66,
	VT_STORAGE	= 67,
	VT_STREAMED_OBJECT	= 68,
	VT_STORED_OBJECT	= 69,
	VT_BLOB_OBJECT	= 70,
	VT_CF	= 71,
	VT_CLSID	= 72,
	VT_VERSIONED_STREAM	= 73,
	VT_BSTR_BLOB	= 0xfff,
	VT_VECTOR	= 0x1000,
	VT_ARRAY	= 0x2000,
	VT_BYREF	= 0x4000,
	VT_RESERVED	= 0x8000,
	VT_ILLEGAL	= 0xffff,
	VT_ILLEGALMASKED	= 0xfff,
	VT_TYPEMASK	= 0xfff
    } ;
typedef ULONG PROPID;

#ifndef SID_IDENTIFIER_AUTHORITY_DEFINED
#define SID_IDENTIFIER_AUTHORITY_DEFINED
typedef struct _SID_IDENTIFIER_AUTHORITY
    {
    UCHAR Value[ 6 ];
    } 	SID_IDENTIFIER_AUTHORITY;

typedef struct _SID_IDENTIFIER_AUTHORITY *PSID_IDENTIFIER_AUTHORITY;

#endif
#ifndef SID_DEFINED
#define SID_DEFINED
typedef struct _SID
    {
    BYTE Revision;
    BYTE SubAuthorityCount;
    SID_IDENTIFIER_AUTHORITY IdentifierAuthority;
    /* [size_is] */ ULONG SubAuthority[ 1 ];
    } 	SID;

typedef struct _SID *PISID;

typedef struct _SID_AND_ATTRIBUTES
    {
    SID *Sid;
    DWORD Attributes;
    } 	SID_AND_ATTRIBUTES;

typedef struct _SID_AND_ATTRIBUTES *PSID_AND_ATTRIBUTES;

#endif
typedef struct tagCSPLATFORM
    {
    DWORD dwPlatformId;
    DWORD dwVersionHi;
    DWORD dwVersionLo;
    DWORD dwProcessorArch;
    } 	CSPLATFORM;

typedef struct tagQUERYCONTEXT
    {
    DWORD dwContext;
    CSPLATFORM Platform;
    LCID Locale;
    DWORD dwVersionHi;
    DWORD dwVersionLo;
    } 	QUERYCONTEXT;

typedef /* [v1_enum] */ 
enum tagTYSPEC
    {	TYSPEC_CLSID	= 0,
	TYSPEC_FILEEXT	= TYSPEC_CLSID + 1,
	TYSPEC_MIMETYPE	= TYSPEC_FILEEXT + 1,
	TYSPEC_FILENAME	= TYSPEC_MIMETYPE + 1,
	TYSPEC_PROGID	= TYSPEC_FILENAME + 1,
	TYSPEC_PACKAGENAME	= TYSPEC_PROGID + 1,
	TYSPEC_OBJECTID	= TYSPEC_PACKAGENAME + 1
    } 	TYSPEC;

typedef /* [public] */ struct __MIDL___MIDL_itf_wtypes_0003_0001
    {
    DWORD tyspec;
    /* [switch_is] */ /* [switch_type] */ union __MIDL___MIDL_itf_wtypes_0003_0005
        {
        /* [case()] */ CLSID clsid;
        /* [case()] */ LPOLESTR pFileExt;
        /* [case()] */ LPOLESTR pMimeType;
        /* [case()] */ LPOLESTR pProgId;
        /* [case()] */ LPOLESTR pFileName;
        /* [case()] */ struct 
            {
            LPOLESTR pPackageName;
            GUID PolicyId;
            } 	ByName;
        /* [case()] */ struct 
            {
            GUID ObjectId;
            GUID PolicyId;
            } 	ByObjectId;
        } 	tagged_union;
    } 	uCLSSPEC;



extern RPC_IF_HANDLE __MIDL_itf_wtypes_0003_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wtypes_0003_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



