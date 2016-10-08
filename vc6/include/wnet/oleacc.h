

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* Compiler settings for oleacc.idl:
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

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __oleacc_h__
#define __oleacc_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IAccessible_FWD_DEFINED__
#define __IAccessible_FWD_DEFINED__
typedef interface IAccessible IAccessible;
#endif 	/* __IAccessible_FWD_DEFINED__ */


#ifndef __IAccessibleHandler_FWD_DEFINED__
#define __IAccessibleHandler_FWD_DEFINED__
typedef interface IAccessibleHandler IAccessibleHandler;
#endif 	/* __IAccessibleHandler_FWD_DEFINED__ */


#ifndef __IAccIdentity_FWD_DEFINED__
#define __IAccIdentity_FWD_DEFINED__
typedef interface IAccIdentity IAccIdentity;
#endif 	/* __IAccIdentity_FWD_DEFINED__ */


#ifndef __IAccPropServer_FWD_DEFINED__
#define __IAccPropServer_FWD_DEFINED__
typedef interface IAccPropServer IAccPropServer;
#endif 	/* __IAccPropServer_FWD_DEFINED__ */


#ifndef __IAccPropServices_FWD_DEFINED__
#define __IAccPropServices_FWD_DEFINED__
typedef interface IAccPropServices IAccPropServices;
#endif 	/* __IAccPropServices_FWD_DEFINED__ */


#ifndef __IAccessible_FWD_DEFINED__
#define __IAccessible_FWD_DEFINED__
typedef interface IAccessible IAccessible;
#endif 	/* __IAccessible_FWD_DEFINED__ */


#ifndef __IAccessibleHandler_FWD_DEFINED__
#define __IAccessibleHandler_FWD_DEFINED__
typedef interface IAccessibleHandler IAccessibleHandler;
#endif 	/* __IAccessibleHandler_FWD_DEFINED__ */


#ifndef __IAccIdentity_FWD_DEFINED__
#define __IAccIdentity_FWD_DEFINED__
typedef interface IAccIdentity IAccIdentity;
#endif 	/* __IAccIdentity_FWD_DEFINED__ */


#ifndef __IAccPropServer_FWD_DEFINED__
#define __IAccPropServer_FWD_DEFINED__
typedef interface IAccPropServer IAccPropServer;
#endif 	/* __IAccPropServer_FWD_DEFINED__ */


#ifndef __IAccPropServices_FWD_DEFINED__
#define __IAccPropServices_FWD_DEFINED__
typedef interface IAccPropServices IAccPropServices;
#endif 	/* __IAccPropServices_FWD_DEFINED__ */


#ifndef __CAccPropServices_FWD_DEFINED__
#define __CAccPropServices_FWD_DEFINED__

#ifdef __cplusplus
typedef class CAccPropServices CAccPropServices;
#else
typedef struct CAccPropServices CAccPropServices;
#endif /* __cplusplus */

#endif 	/* __CAccPropServices_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_oleacc_0000 */
/* [local] */ 

//=--------------------------------------------------------------------------=
// OLEACC.H
//=--------------------------------------------------------------------------=
// (C) Copyright (c) Microsoft Corporation. All rights reserved.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//=--------------------------------------------------------------------------=

//=--------------------------------------------------------------------------=
// Typedefs
//=--------------------------------------------------------------------------=

typedef LRESULT (STDAPICALLTYPE *LPFNLRESULTFROMOBJECT)(REFIID riid, WPARAM wParam, LPUNKNOWN punk);
typedef HRESULT (STDAPICALLTYPE *LPFNOBJECTFROMLRESULT)(LRESULT lResult, REFIID riid, WPARAM wParam, void** ppvObject);
typedef HRESULT (STDAPICALLTYPE *LPFNACCESSIBLEOBJECTFROMWINDOW)(HWND hwnd, DWORD dwId, REFIID riid, void** ppvObject);
typedef HRESULT (STDAPICALLTYPE *LPFNACCESSIBLEOBJECTFROMPOINT)(POINT ptScreen, IAccessible** ppacc, VARIANT* pvarChild);
typedef HRESULT (STDAPICALLTYPE *LPFNCREATESTDACCESSIBLEOBJECT)(HWND hwnd, LONG idObject, REFIID riid, void** ppvObject);
typedef HRESULT (STDAPICALLTYPE *LPFNACCESSIBLECHILDREN)(IAccessible* paccContainer, LONG iChildStart,LONG cChildren,VARIANT* rgvarChildren,LONG* pcObtained);

//=--------------------------------------------------------------------------=
// GUIDs
//=--------------------------------------------------------------------------=

DEFINE_GUID(LIBID_Accessibility,     0x1ea4dbf0, 0x3c3b, 0x11cf, 0x81, 0x0c, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(IID_IAccessible,         0x618736e0, 0x3c3d, 0x11cf, 0x81, 0x0c, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71);
DEFINE_GUID(IID_IAccessibleHandler,  0x03022430, 0xABC4, 0x11d0, 0xBD, 0xE2, 0x00, 0xAA, 0x00, 0x1A, 0x19, 0x53);
DEFINE_GUID(IID_IAccIdentity,        0x7852b78d, 0x1cfd, 0x41c1, 0xa6, 0x15, 0x9c, 0x0c, 0x85, 0x96, 0x0b, 0x5f);
DEFINE_GUID(IID_IAccPropServer,      0x76c0dbbb, 0x15e0, 0x4e7b, 0xb6, 0x1b, 0x20, 0xee, 0xea, 0x20, 0x01, 0xe0);
DEFINE_GUID(IID_IAccPropServices,    0x6e26e776, 0x04f0, 0x495d, 0x80, 0xe4, 0x33, 0x30, 0x35, 0x2e, 0x31, 0x69);
DEFINE_GUID(IID_IAccPropMgrInternal, 0x2bd370a9, 0x3e7f, 0x4edd, 0x8a, 0x85, 0xf8, 0xfe, 0xd1, 0xf8, 0xe5, 0x1f);
DEFINE_GUID(CLSID_AccPropServices,   0xb5f8350b, 0x0548, 0x48b1, 0xa6, 0xee, 0x88, 0xbd, 0x00, 0xb4, 0xa5, 0xe7);
DEFINE_GUID(IIS_IsOleaccProxy,       0x902697fa, 0x80e4, 0x4560, 0x80, 0x2a, 0xa1, 0x3f, 0x22, 0xa6, 0x47, 0x09);

//=--------------------------------------------------------------------------=
// MSAA API Prototypes
//=--------------------------------------------------------------------------=

STDAPI_(LRESULT) LresultFromObject(REFIID riid, WPARAM wParam, LPUNKNOWN punk);
STDAPI          ObjectFromLresult(LRESULT lResult, REFIID riid, WPARAM wParam, void** ppvObject);
STDAPI          WindowFromAccessibleObject(IAccessible*, HWND* phwnd);
STDAPI          AccessibleObjectFromWindow(HWND hwnd, DWORD dwId, REFIID riid, void **ppvObject);
STDAPI          AccessibleObjectFromEvent(HWND hwnd, DWORD dwId, DWORD dwChildId, IAccessible** ppacc, VARIANT* pvarChild);
STDAPI          AccessibleObjectFromPoint(POINT ptScreen, IAccessible ** ppacc, VARIANT* pvarChild);
STDAPI          AccessibleChildren (IAccessible* paccContainer, LONG iChildStart,LONG cChildren, VARIANT* rgvarChildren,LONG* pcObtained);

STDAPI_(UINT)   GetRoleTextA(DWORD lRole, LPSTR lpszRole, UINT cchRoleMax);
STDAPI_(UINT)   GetRoleTextW(DWORD lRole, LPWSTR lpszRole, UINT cchRoleMax);

#ifdef UNICODE
#define GetRoleText     GetRoleTextW
#else
#define GetRoleText     GetRoleTextA
#endif // UNICODE

STDAPI_(UINT)   GetStateTextA(DWORD lStateBit, LPSTR lpszState, UINT cchState);
STDAPI_(UINT)   GetStateTextW(DWORD lStateBit, LPWSTR lpszState, UINT cchState);
#ifdef UNICODE
#define GetStateText    GetStateTextW
#else
#define GetStateText    GetStateTextA
#endif // UNICODE

STDAPI_(VOID)   GetOleaccVersionInfo(DWORD* pVer, DWORD* pBuild);

STDAPI          CreateStdAccessibleObject(HWND hwnd, LONG idObject, REFIID riid, void** ppvObject);

STDAPI          CreateStdAccessibleProxyA(HWND hwnd, LPCSTR pClassName, LONG idObject, REFIID riid, void** ppvObject);
STDAPI          CreateStdAccessibleProxyW(HWND hwnd, LPCWSTR pClassName, LONG idObject, REFIID riid, void** ppvObject);

#ifdef UNICODE
#define CreateStdAccessibleProxy     CreateStdAccessibleProxyW
#else
#define CreateStdAccessibleProxy     CreateStdAccessibleProxyA
#endif // UNICODE


// Simple Owner-Drawn Menu support...

#define MSAA_MENU_SIG 0xAA0DF00DL

// Menu's dwItemData should point to one of these structs:
// (or can point to an app-defined struct containing this as the first member)
typedef struct tagMSAAMENUINFO {
    DWORD   dwMSAASignature; // Must be MSAA_MENU_SIG
    DWORD   cchWText;        // Length of text, in Unicode characters, excluding terminating NUL
    LPWSTR  pszWText;        // NUL-terminated text, in Unicode
} MSAAMENUINFO, *LPMSAAMENUINFO;


//=--------------------------------------------------------------------------=
// Property GUIDs (used by annotation interfaces)
//=--------------------------------------------------------------------------=

DEFINE_GUID( PROPID_ACC_NAME             , 0x608d3df8, 0x8128, 0x4aa7, 0xa4, 0x28, 0xf5, 0x5e, 0x49, 0x26, 0x72, 0x91);
DEFINE_GUID( PROPID_ACC_VALUE            , 0x123fe443, 0x211a, 0x4615, 0x95, 0x27, 0xc4, 0x5a, 0x7e, 0x93, 0x71, 0x7a);
DEFINE_GUID( PROPID_ACC_DESCRIPTION      , 0x4d48dfe4, 0xbd3f, 0x491f, 0xa6, 0x48, 0x49, 0x2d, 0x6f, 0x20, 0xc5, 0x88);
DEFINE_GUID( PROPID_ACC_ROLE             , 0xcb905ff2, 0x7bd1, 0x4c05, 0xb3, 0xc8, 0xe6, 0xc2, 0x41, 0x36, 0x4d, 0x70);
DEFINE_GUID( PROPID_ACC_STATE            , 0xa8d4d5b0, 0x0a21, 0x42d0, 0xa5, 0xc0, 0x51, 0x4e, 0x98, 0x4f, 0x45, 0x7b);
DEFINE_GUID( PROPID_ACC_HELP             , 0xc831e11f, 0x44db, 0x4a99, 0x97, 0x68, 0xcb, 0x8f, 0x97, 0x8b, 0x72, 0x31);
DEFINE_GUID( PROPID_ACC_KEYBOARDSHORTCUT , 0x7d9bceee, 0x7d1e, 0x4979, 0x93, 0x82, 0x51, 0x80, 0xf4, 0x17, 0x2c, 0x34);
DEFINE_GUID( PROPID_ACC_DEFAULTACTION    , 0x180c072b, 0xc27f, 0x43c7, 0x99, 0x22, 0xf6, 0x35, 0x62, 0xa4, 0x63, 0x2b);

DEFINE_GUID( PROPID_ACC_HELPTOPIC        , 0x787d1379, 0x8ede, 0x440b, 0x8a, 0xec, 0x11, 0xf7, 0xbf, 0x90, 0x30, 0xb3);
DEFINE_GUID( PROPID_ACC_FOCUS            , 0x6eb335df, 0x1c29, 0x4127, 0xb1, 0x2c, 0xde, 0xe9, 0xfd, 0x15, 0x7f, 0x2b);
DEFINE_GUID( PROPID_ACC_SELECTION        , 0xb99d073c, 0xd731, 0x405b, 0x90, 0x61, 0xd9, 0x5e, 0x8f, 0x84, 0x29, 0x84);
DEFINE_GUID( PROPID_ACC_PARENT           , 0x474c22b6, 0xffc2, 0x467a, 0xb1, 0xb5, 0xe9, 0x58, 0xb4, 0x65, 0x73, 0x30);

DEFINE_GUID( PROPID_ACC_NAV_UP           , 0x016e1a2b, 0x1a4e, 0x4767, 0x86, 0x12, 0x33, 0x86, 0xf6, 0x69, 0x35, 0xec);
DEFINE_GUID( PROPID_ACC_NAV_DOWN         , 0x031670ed, 0x3cdf, 0x48d2, 0x96, 0x13, 0x13, 0x8f, 0x2d, 0xd8, 0xa6, 0x68);
DEFINE_GUID( PROPID_ACC_NAV_LEFT         , 0x228086cb, 0x82f1, 0x4a39, 0x87, 0x05, 0xdc, 0xdc, 0x0f, 0xff, 0x92, 0xf5);
DEFINE_GUID( PROPID_ACC_NAV_RIGHT        , 0xcd211d9f, 0xe1cb, 0x4fe5, 0xa7, 0x7c, 0x92, 0x0b, 0x88, 0x4d, 0x09, 0x5b);
DEFINE_GUID( PROPID_ACC_NAV_PREV         , 0x776d3891, 0xc73b, 0x4480, 0xb3, 0xf6, 0x07, 0x6a, 0x16, 0xa1, 0x5a, 0xf6);
DEFINE_GUID( PROPID_ACC_NAV_NEXT         , 0x1cdc5455, 0x8cd9, 0x4c92, 0xa3, 0x71, 0x39, 0x39, 0xa2, 0xfe, 0x3e, 0xee);
DEFINE_GUID( PROPID_ACC_NAV_FIRSTCHILD   , 0xcfd02558, 0x557b, 0x4c67, 0x84, 0xf9, 0x2a, 0x09, 0xfc, 0xe4, 0x07, 0x49);
DEFINE_GUID( PROPID_ACC_NAV_LASTCHILD    , 0x302ecaa5, 0x48d5, 0x4f8d, 0xb6, 0x71, 0x1a, 0x8d, 0x20, 0xa7, 0x78, 0x32);

// Value map, used by sliders and other controls...
DEFINE_GUID( PROPID_ACC_ROLEMAP          , 0xf79acda2, 0x140d, 0x4fe6, 0x89, 0x14, 0x20, 0x84, 0x76, 0x32, 0x82, 0x69);
DEFINE_GUID( PROPID_ACC_VALUEMAP         , 0xda1c3d79, 0xfc5c, 0x420e, 0xb3, 0x99, 0x9d, 0x15, 0x33, 0x54, 0x9e, 0x75);
DEFINE_GUID( PROPID_ACC_STATEMAP         , 0x43946c5e, 0x0ac0, 0x4042, 0xb5, 0x25, 0x07, 0xbb, 0xdb, 0xe1, 0x7f, 0xa7);
DEFINE_GUID( PROPID_ACC_DESCRIPTIONMAP   , 0x1ff1435f, 0x8a14, 0x477b, 0xb2, 0x26, 0xa0, 0xab, 0xe2, 0x79, 0x97, 0x5d);

DEFINE_GUID( PROPID_ACC_DODEFAULTACTION  , 0x1ba09523, 0x2e3b, 0x49a6, 0xa0, 0x59, 0x59, 0x68, 0x2a, 0x3c, 0x48, 0xfd);

//=--------------------------------------------------------------------------=
// Interface Definitions
//=--------------------------------------------------------------------------=



extern RPC_IF_HANDLE __MIDL_itf_oleacc_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_oleacc_0000_v0_0_s_ifspec;

#ifndef __IAccessible_INTERFACE_DEFINED__
#define __IAccessible_INTERFACE_DEFINED__

/* interface IAccessible */
/* [unique][dual][hidden][uuid][object] */ 

#define	DISPID_ACC_PARENT	( -5000 )

#define	DISPID_ACC_CHILDCOUNT	( -5001 )

#define	DISPID_ACC_CHILD	( -5002 )

#define	DISPID_ACC_NAME	( -5003 )

#define	DISPID_ACC_VALUE	( -5004 )

#define	DISPID_ACC_DESCRIPTION	( -5005 )

#define	DISPID_ACC_ROLE	( -5006 )

#define	DISPID_ACC_STATE	( -5007 )

#define	DISPID_ACC_HELP	( -5008 )

#define	DISPID_ACC_HELPTOPIC	( -5009 )

#define	DISPID_ACC_KEYBOARDSHORTCUT	( -5010 )

#define	DISPID_ACC_FOCUS	( -5011 )

#define	DISPID_ACC_SELECTION	( -5012 )

#define	DISPID_ACC_DEFAULTACTION	( -5013 )

#define	DISPID_ACC_SELECT	( -5014 )

#define	DISPID_ACC_LOCATION	( -5015 )

#define	DISPID_ACC_NAVIGATE	( -5016 )

#define	DISPID_ACC_HITTEST	( -5017 )

#define	DISPID_ACC_DODEFAULTACTION	( -5018 )

typedef /* [unique] */ IAccessible *LPACCESSIBLE;

#define	NAVDIR_MIN	( 0 )

#define	NAVDIR_UP	( 0x1 )

#define	NAVDIR_DOWN	( 0x2 )

#define	NAVDIR_LEFT	( 0x3 )

#define	NAVDIR_RIGHT	( 0x4 )

#define	NAVDIR_NEXT	( 0x5 )

#define	NAVDIR_PREVIOUS	( 0x6 )

#define	NAVDIR_FIRSTCHILD	( 0x7 )

#define	NAVDIR_LASTCHILD	( 0x8 )

#define	NAVDIR_MAX	( 0x9 )

#define	SELFLAG_NONE	( 0 )

#define	SELFLAG_TAKEFOCUS	( 0x1 )

#define	SELFLAG_TAKESELECTION	( 0x2 )

#define	SELFLAG_EXTENDSELECTION	( 0x4 )

#define	SELFLAG_ADDSELECTION	( 0x8 )

#define	SELFLAG_REMOVESELECTION	( 0x10 )

#define	SELFLAG_VALID	( 0x1f )

#ifndef STATE_SYSTEM_UNAVAILABLE
#define	STATE_SYSTEM_NORMAL	( 0 )

#define	STATE_SYSTEM_UNAVAILABLE	( 0x1 )

#define	STATE_SYSTEM_SELECTED	( 0x2 )

#define	STATE_SYSTEM_FOCUSED	( 0x4 )

#define	STATE_SYSTEM_PRESSED	( 0x8 )

#define	STATE_SYSTEM_CHECKED	( 0x10 )

#define	STATE_SYSTEM_MIXED	( 0x20 )

#define	STATE_SYSTEM_INDETERMINATE	( STATE_SYSTEM_MIXED )

#define	STATE_SYSTEM_READONLY	( 0x40 )

#define	STATE_SYSTEM_HOTTRACKED	( 0x80 )

#define	STATE_SYSTEM_DEFAULT	( 0x100 )

#define	STATE_SYSTEM_EXPANDED	( 0x200 )

#define	STATE_SYSTEM_COLLAPSED	( 0x400 )

#define	STATE_SYSTEM_BUSY	( 0x800 )

#define	STATE_SYSTEM_FLOATING	( 0x1000 )

#define	STATE_SYSTEM_MARQUEED	( 0x2000 )

#define	STATE_SYSTEM_ANIMATED	( 0x4000 )

#define	STATE_SYSTEM_INVISIBLE	( 0x8000 )

#define	STATE_SYSTEM_OFFSCREEN	( 0x10000 )

#define	STATE_SYSTEM_SIZEABLE	( 0x20000 )

#define	STATE_SYSTEM_MOVEABLE	( 0x40000 )

#define	STATE_SYSTEM_SELFVOICING	( 0x80000 )

#define	STATE_SYSTEM_FOCUSABLE	( 0x100000 )

#define	STATE_SYSTEM_SELECTABLE	( 0x200000 )

#define	STATE_SYSTEM_LINKED	( 0x400000 )

#define	STATE_SYSTEM_TRAVERSED	( 0x800000 )

#define	STATE_SYSTEM_MULTISELECTABLE	( 0x1000000 )

#define	STATE_SYSTEM_EXTSELECTABLE	( 0x2000000 )

#define	STATE_SYSTEM_ALERT_LOW	( 0x4000000 )

#define	STATE_SYSTEM_ALERT_MEDIUM	( 0x8000000 )

#define	STATE_SYSTEM_ALERT_HIGH	( 0x10000000 )

#define	STATE_SYSTEM_PROTECTED	( 0x20000000 )

#define	STATE_SYSTEM_VALID	( 0x7fffffff )

#endif //STATE_SYSTEM_UNAVAILABLE
#ifndef STATE_SYSTEM_HASPOPUP
#define	STATE_SYSTEM_HASPOPUP	( 0x40000000 )

#endif //STATE_SYSTEM_HASPOPUP
#define	ROLE_SYSTEM_TITLEBAR	( 0x1 )

#define	ROLE_SYSTEM_MENUBAR	( 0x2 )

#define	ROLE_SYSTEM_SCROLLBAR	( 0x3 )

#define	ROLE_SYSTEM_GRIP	( 0x4 )

#define	ROLE_SYSTEM_SOUND	( 0x5 )

#define	ROLE_SYSTEM_CURSOR	( 0x6 )

#define	ROLE_SYSTEM_CARET	( 0x7 )

#define	ROLE_SYSTEM_ALERT	( 0x8 )

#define	ROLE_SYSTEM_WINDOW	( 0x9 )

#define	ROLE_SYSTEM_CLIENT	( 0xa )

#define	ROLE_SYSTEM_MENUPOPUP	( 0xb )

#define	ROLE_SYSTEM_MENUITEM	( 0xc )

#define	ROLE_SYSTEM_TOOLTIP	( 0xd )

#define	ROLE_SYSTEM_APPLICATION	( 0xe )

#define	ROLE_SYSTEM_DOCUMENT	( 0xf )

#define	ROLE_SYSTEM_PANE	( 0x10 )

#define	ROLE_SYSTEM_CHART	( 0x11 )

#define	ROLE_SYSTEM_DIALOG	( 0x12 )

#define	ROLE_SYSTEM_BORDER	( 0x13 )

#define	ROLE_SYSTEM_GROUPING	( 0x14 )

#define	ROLE_SYSTEM_SEPARATOR	( 0x15 )

#define	ROLE_SYSTEM_TOOLBAR	( 0x16 )

#define	ROLE_SYSTEM_STATUSBAR	( 0x17 )

#define	ROLE_SYSTEM_TABLE	( 0x18 )

#define	ROLE_SYSTEM_COLUMNHEADER	( 0x19 )

#define	ROLE_SYSTEM_ROWHEADER	( 0x1a )

#define	ROLE_SYSTEM_COLUMN	( 0x1b )

#define	ROLE_SYSTEM_ROW	( 0x1c )

#define	ROLE_SYSTEM_CELL	( 0x1d )

#define	ROLE_SYSTEM_LINK	( 0x1e )

#define	ROLE_SYSTEM_HELPBALLOON	( 0x1f )

#define	ROLE_SYSTEM_CHARACTER	( 0x20 )

#define	ROLE_SYSTEM_LIST	( 0x21 )

#define	ROLE_SYSTEM_LISTITEM	( 0x22 )

#define	ROLE_SYSTEM_OUTLINE	( 0x23 )

#define	ROLE_SYSTEM_OUTLINEITEM	( 0x24 )

#define	ROLE_SYSTEM_PAGETAB	( 0x25 )

#define	ROLE_SYSTEM_PROPERTYPAGE	( 0x26 )

#define	ROLE_SYSTEM_INDICATOR	( 0x27 )

#define	ROLE_SYSTEM_GRAPHIC	( 0x28 )

#define	ROLE_SYSTEM_STATICTEXT	( 0x29 )

#define	ROLE_SYSTEM_TEXT	( 0x2a )

#define	ROLE_SYSTEM_PUSHBUTTON	( 0x2b )

#define	ROLE_SYSTEM_CHECKBUTTON	( 0x2c )

#define	ROLE_SYSTEM_RADIOBUTTON	( 0x2d )

#define	ROLE_SYSTEM_COMBOBOX	( 0x2e )

#define	ROLE_SYSTEM_DROPLIST	( 0x2f )

#define	ROLE_SYSTEM_PROGRESSBAR	( 0x30 )

#define	ROLE_SYSTEM_DIAL	( 0x31 )

#define	ROLE_SYSTEM_HOTKEYFIELD	( 0x32 )

#define	ROLE_SYSTEM_SLIDER	( 0x33 )

#define	ROLE_SYSTEM_SPINBUTTON	( 0x34 )

#define	ROLE_SYSTEM_DIAGRAM	( 0x35 )

#define	ROLE_SYSTEM_ANIMATION	( 0x36 )

#define	ROLE_SYSTEM_EQUATION	( 0x37 )

#define	ROLE_SYSTEM_BUTTONDROPDOWN	( 0x38 )

#define	ROLE_SYSTEM_BUTTONMENU	( 0x39 )

#define	ROLE_SYSTEM_BUTTONDROPDOWNGRID	( 0x3a )

#define	ROLE_SYSTEM_WHITESPACE	( 0x3b )

#define	ROLE_SYSTEM_PAGETABLIST	( 0x3c )

#define	ROLE_SYSTEM_CLOCK	( 0x3d )

#define	ROLE_SYSTEM_SPLITBUTTON	( 0x3e )

#define	ROLE_SYSTEM_IPADDRESS	( 0x3f )

#define	ROLE_SYSTEM_OUTLINEBUTTON	( 0x40 )


EXTERN_C const IID IID_IAccessible;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("618736e0-3c3d-11cf-810c-00aa00389b71")
    IAccessible : public IDispatch
    {
    public:
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accParent( 
            /* [retval][out] */ IDispatch **ppdispParent) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accChildCount( 
            /* [retval][out] */ long *pcountChildren) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accChild( 
            /* [in] */ VARIANT varChild,
            /* [retval][out] */ IDispatch **ppdispChild) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accName( 
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszName) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accValue( 
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszValue) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accDescription( 
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszDescription) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accRole( 
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ VARIANT *pvarRole) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accState( 
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ VARIANT *pvarState) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accHelp( 
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszHelp) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accHelpTopic( 
            /* [out] */ BSTR *pszHelpFile,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ long *pidTopic) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accKeyboardShortcut( 
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszKeyboardShortcut) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accFocus( 
            /* [retval][out] */ VARIANT *pvarChild) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accSelection( 
            /* [retval][out] */ VARIANT *pvarChildren) = 0;
        
        virtual /* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE get_accDefaultAction( 
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszDefaultAction) = 0;
        
        virtual /* [id][hidden] */ HRESULT STDMETHODCALLTYPE accSelect( 
            /* [in] */ long flagsSelect,
            /* [optional][in] */ VARIANT varChild) = 0;
        
        virtual /* [id][hidden] */ HRESULT STDMETHODCALLTYPE accLocation( 
            /* [out] */ long *pxLeft,
            /* [out] */ long *pyTop,
            /* [out] */ long *pcxWidth,
            /* [out] */ long *pcyHeight,
            /* [optional][in] */ VARIANT varChild) = 0;
        
        virtual /* [id][hidden] */ HRESULT STDMETHODCALLTYPE accNavigate( 
            /* [in] */ long navDir,
            /* [optional][in] */ VARIANT varStart,
            /* [retval][out] */ VARIANT *pvarEndUpAt) = 0;
        
        virtual /* [id][hidden] */ HRESULT STDMETHODCALLTYPE accHitTest( 
            /* [in] */ long xLeft,
            /* [in] */ long yTop,
            /* [retval][out] */ VARIANT *pvarChild) = 0;
        
        virtual /* [id][hidden] */ HRESULT STDMETHODCALLTYPE accDoDefaultAction( 
            /* [optional][in] */ VARIANT varChild) = 0;
        
        virtual /* [id][propput][hidden] */ HRESULT STDMETHODCALLTYPE put_accName( 
            /* [optional][in] */ VARIANT varChild,
            /* [in] */ BSTR szName) = 0;
        
        virtual /* [id][propput][hidden] */ HRESULT STDMETHODCALLTYPE put_accValue( 
            /* [optional][in] */ VARIANT varChild,
            /* [in] */ BSTR szValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAccessibleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAccessible * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAccessible * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAccessible * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAccessible * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAccessible * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAccessible * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAccessible * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE *get_accParent )( 
            IAccessible * This,
            /* [retval][out] */ IDispatch **ppdispParent);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE *get_accChildCount )( 
            IAccessible * This,
            /* [retval][out] */ long *pcountChildren);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE *get_accChild )( 
            IAccessible * This,
            /* [in] */ VARIANT varChild,
            /* [retval][out] */ IDispatch **ppdispChild);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE *get_accName )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszName);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE *get_accValue )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszValue);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE *get_accDescription )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszDescription);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE *get_accRole )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ VARIANT *pvarRole);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE *get_accState )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ VARIANT *pvarState);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE *get_accHelp )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszHelp);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE *get_accHelpTopic )( 
            IAccessible * This,
            /* [out] */ BSTR *pszHelpFile,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ long *pidTopic);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE *get_accKeyboardShortcut )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszKeyboardShortcut);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE *get_accFocus )( 
            IAccessible * This,
            /* [retval][out] */ VARIANT *pvarChild);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE *get_accSelection )( 
            IAccessible * This,
            /* [retval][out] */ VARIANT *pvarChildren);
        
        /* [id][propget][hidden] */ HRESULT ( STDMETHODCALLTYPE *get_accDefaultAction )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [retval][out] */ BSTR *pszDefaultAction);
        
        /* [id][hidden] */ HRESULT ( STDMETHODCALLTYPE *accSelect )( 
            IAccessible * This,
            /* [in] */ long flagsSelect,
            /* [optional][in] */ VARIANT varChild);
        
        /* [id][hidden] */ HRESULT ( STDMETHODCALLTYPE *accLocation )( 
            IAccessible * This,
            /* [out] */ long *pxLeft,
            /* [out] */ long *pyTop,
            /* [out] */ long *pcxWidth,
            /* [out] */ long *pcyHeight,
            /* [optional][in] */ VARIANT varChild);
        
        /* [id][hidden] */ HRESULT ( STDMETHODCALLTYPE *accNavigate )( 
            IAccessible * This,
            /* [in] */ long navDir,
            /* [optional][in] */ VARIANT varStart,
            /* [retval][out] */ VARIANT *pvarEndUpAt);
        
        /* [id][hidden] */ HRESULT ( STDMETHODCALLTYPE *accHitTest )( 
            IAccessible * This,
            /* [in] */ long xLeft,
            /* [in] */ long yTop,
            /* [retval][out] */ VARIANT *pvarChild);
        
        /* [id][hidden] */ HRESULT ( STDMETHODCALLTYPE *accDoDefaultAction )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild);
        
        /* [id][propput][hidden] */ HRESULT ( STDMETHODCALLTYPE *put_accName )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [in] */ BSTR szName);
        
        /* [id][propput][hidden] */ HRESULT ( STDMETHODCALLTYPE *put_accValue )( 
            IAccessible * This,
            /* [optional][in] */ VARIANT varChild,
            /* [in] */ BSTR szValue);
        
        END_INTERFACE
    } IAccessibleVtbl;

    interface IAccessible
    {
        CONST_VTBL struct IAccessibleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAccessible_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAccessible_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAccessible_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAccessible_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IAccessible_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IAccessible_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IAccessible_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IAccessible_get_accParent(This,ppdispParent)	\
    (This)->lpVtbl -> get_accParent(This,ppdispParent)

#define IAccessible_get_accChildCount(This,pcountChildren)	\
    (This)->lpVtbl -> get_accChildCount(This,pcountChildren)

#define IAccessible_get_accChild(This,varChild,ppdispChild)	\
    (This)->lpVtbl -> get_accChild(This,varChild,ppdispChild)

#define IAccessible_get_accName(This,varChild,pszName)	\
    (This)->lpVtbl -> get_accName(This,varChild,pszName)

#define IAccessible_get_accValue(This,varChild,pszValue)	\
    (This)->lpVtbl -> get_accValue(This,varChild,pszValue)

#define IAccessible_get_accDescription(This,varChild,pszDescription)	\
    (This)->lpVtbl -> get_accDescription(This,varChild,pszDescription)

#define IAccessible_get_accRole(This,varChild,pvarRole)	\
    (This)->lpVtbl -> get_accRole(This,varChild,pvarRole)

#define IAccessible_get_accState(This,varChild,pvarState)	\
    (This)->lpVtbl -> get_accState(This,varChild,pvarState)

#define IAccessible_get_accHelp(This,varChild,pszHelp)	\
    (This)->lpVtbl -> get_accHelp(This,varChild,pszHelp)

#define IAccessible_get_accHelpTopic(This,pszHelpFile,varChild,pidTopic)	\
    (This)->lpVtbl -> get_accHelpTopic(This,pszHelpFile,varChild,pidTopic)

#define IAccessible_get_accKeyboardShortcut(This,varChild,pszKeyboardShortcut)	\
    (This)->lpVtbl -> get_accKeyboardShortcut(This,varChild,pszKeyboardShortcut)

#define IAccessible_get_accFocus(This,pvarChild)	\
    (This)->lpVtbl -> get_accFocus(This,pvarChild)

#define IAccessible_get_accSelection(This,pvarChildren)	\
    (This)->lpVtbl -> get_accSelection(This,pvarChildren)

#define IAccessible_get_accDefaultAction(This,varChild,pszDefaultAction)	\
    (This)->lpVtbl -> get_accDefaultAction(This,varChild,pszDefaultAction)

#define IAccessible_accSelect(This,flagsSelect,varChild)	\
    (This)->lpVtbl -> accSelect(This,flagsSelect,varChild)

#define IAccessible_accLocation(This,pxLeft,pyTop,pcxWidth,pcyHeight,varChild)	\
    (This)->lpVtbl -> accLocation(This,pxLeft,pyTop,pcxWidth,pcyHeight,varChild)

#define IAccessible_accNavigate(This,navDir,varStart,pvarEndUpAt)	\
    (This)->lpVtbl -> accNavigate(This,navDir,varStart,pvarEndUpAt)

#define IAccessible_accHitTest(This,xLeft,yTop,pvarChild)	\
    (This)->lpVtbl -> accHitTest(This,xLeft,yTop,pvarChild)

#define IAccessible_accDoDefaultAction(This,varChild)	\
    (This)->lpVtbl -> accDoDefaultAction(This,varChild)

#define IAccessible_put_accName(This,varChild,szName)	\
    (This)->lpVtbl -> put_accName(This,varChild,szName)

#define IAccessible_put_accValue(This,varChild,szValue)	\
    (This)->lpVtbl -> put_accValue(This,varChild,szValue)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_get_accParent_Proxy( 
    IAccessible * This,
    /* [retval][out] */ IDispatch **ppdispParent);


void __RPC_STUB IAccessible_get_accParent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_get_accChildCount_Proxy( 
    IAccessible * This,
    /* [retval][out] */ long *pcountChildren);


void __RPC_STUB IAccessible_get_accChildCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_get_accChild_Proxy( 
    IAccessible * This,
    /* [in] */ VARIANT varChild,
    /* [retval][out] */ IDispatch **ppdispChild);


void __RPC_STUB IAccessible_get_accChild_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_get_accName_Proxy( 
    IAccessible * This,
    /* [optional][in] */ VARIANT varChild,
    /* [retval][out] */ BSTR *pszName);


void __RPC_STUB IAccessible_get_accName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_get_accValue_Proxy( 
    IAccessible * This,
    /* [optional][in] */ VARIANT varChild,
    /* [retval][out] */ BSTR *pszValue);


void __RPC_STUB IAccessible_get_accValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_get_accDescription_Proxy( 
    IAccessible * This,
    /* [optional][in] */ VARIANT varChild,
    /* [retval][out] */ BSTR *pszDescription);


void __RPC_STUB IAccessible_get_accDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_get_accRole_Proxy( 
    IAccessible * This,
    /* [optional][in] */ VARIANT varChild,
    /* [retval][out] */ VARIANT *pvarRole);


void __RPC_STUB IAccessible_get_accRole_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_get_accState_Proxy( 
    IAccessible * This,
    /* [optional][in] */ VARIANT varChild,
    /* [retval][out] */ VARIANT *pvarState);


void __RPC_STUB IAccessible_get_accState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_get_accHelp_Proxy( 
    IAccessible * This,
    /* [optional][in] */ VARIANT varChild,
    /* [retval][out] */ BSTR *pszHelp);


void __RPC_STUB IAccessible_get_accHelp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_get_accHelpTopic_Proxy( 
    IAccessible * This,
    /* [out] */ BSTR *pszHelpFile,
    /* [optional][in] */ VARIANT varChild,
    /* [retval][out] */ long *pidTopic);


void __RPC_STUB IAccessible_get_accHelpTopic_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_get_accKeyboardShortcut_Proxy( 
    IAccessible * This,
    /* [optional][in] */ VARIANT varChild,
    /* [retval][out] */ BSTR *pszKeyboardShortcut);


void __RPC_STUB IAccessible_get_accKeyboardShortcut_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_get_accFocus_Proxy( 
    IAccessible * This,
    /* [retval][out] */ VARIANT *pvarChild);


void __RPC_STUB IAccessible_get_accFocus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_get_accSelection_Proxy( 
    IAccessible * This,
    /* [retval][out] */ VARIANT *pvarChildren);


void __RPC_STUB IAccessible_get_accSelection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_get_accDefaultAction_Proxy( 
    IAccessible * This,
    /* [optional][in] */ VARIANT varChild,
    /* [retval][out] */ BSTR *pszDefaultAction);


void __RPC_STUB IAccessible_get_accDefaultAction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_accSelect_Proxy( 
    IAccessible * This,
    /* [in] */ long flagsSelect,
    /* [optional][in] */ VARIANT varChild);


void __RPC_STUB IAccessible_accSelect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_accLocation_Proxy( 
    IAccessible * This,
    /* [out] */ long *pxLeft,
    /* [out] */ long *pyTop,
    /* [out] */ long *pcxWidth,
    /* [out] */ long *pcyHeight,
    /* [optional][in] */ VARIANT varChild);


void __RPC_STUB IAccessible_accLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_accNavigate_Proxy( 
    IAccessible * This,
    /* [in] */ long navDir,
    /* [optional][in] */ VARIANT varStart,
    /* [retval][out] */ VARIANT *pvarEndUpAt);


void __RPC_STUB IAccessible_accNavigate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_accHitTest_Proxy( 
    IAccessible * This,
    /* [in] */ long xLeft,
    /* [in] */ long yTop,
    /* [retval][out] */ VARIANT *pvarChild);


void __RPC_STUB IAccessible_accHitTest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_accDoDefaultAction_Proxy( 
    IAccessible * This,
    /* [optional][in] */ VARIANT varChild);


void __RPC_STUB IAccessible_accDoDefaultAction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_put_accName_Proxy( 
    IAccessible * This,
    /* [optional][in] */ VARIANT varChild,
    /* [in] */ BSTR szName);


void __RPC_STUB IAccessible_put_accName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput][hidden] */ HRESULT STDMETHODCALLTYPE IAccessible_put_accValue_Proxy( 
    IAccessible * This,
    /* [optional][in] */ VARIANT varChild,
    /* [in] */ BSTR szValue);


void __RPC_STUB IAccessible_put_accValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAccessible_INTERFACE_DEFINED__ */


#ifndef __IAccessibleHandler_INTERFACE_DEFINED__
#define __IAccessibleHandler_INTERFACE_DEFINED__

/* interface IAccessibleHandler */
/* [unique][oleautomation][hidden][uuid][object] */ 

typedef /* [unique] */ IAccessibleHandler *LPACCESSIBLEHANDLER;


EXTERN_C const IID IID_IAccessibleHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("03022430-ABC4-11d0-BDE2-00AA001A1953")
    IAccessibleHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AccessibleObjectFromID( 
            /* [in] */ long hwnd,
            /* [in] */ long lObjectID,
            /* [out] */ LPACCESSIBLE *pIAccessible) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAccessibleHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAccessibleHandler * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAccessibleHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAccessibleHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *AccessibleObjectFromID )( 
            IAccessibleHandler * This,
            /* [in] */ long hwnd,
            /* [in] */ long lObjectID,
            /* [out] */ LPACCESSIBLE *pIAccessible);
        
        END_INTERFACE
    } IAccessibleHandlerVtbl;

    interface IAccessibleHandler
    {
        CONST_VTBL struct IAccessibleHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAccessibleHandler_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAccessibleHandler_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAccessibleHandler_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAccessibleHandler_AccessibleObjectFromID(This,hwnd,lObjectID,pIAccessible)	\
    (This)->lpVtbl -> AccessibleObjectFromID(This,hwnd,lObjectID,pIAccessible)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IAccessibleHandler_AccessibleObjectFromID_Proxy( 
    IAccessibleHandler * This,
    /* [in] */ long hwnd,
    /* [in] */ long lObjectID,
    /* [out] */ LPACCESSIBLE *pIAccessible);


void __RPC_STUB IAccessibleHandler_AccessibleObjectFromID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAccessibleHandler_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_oleacc_0116 */
/* [local] */ 

typedef 
enum AnnoScope
    {	ANNO_THIS	= 0,
	ANNO_CONTAINER	= 1
    } 	AnnoScope;

typedef GUID MSAAPROPID;



extern RPC_IF_HANDLE __MIDL_itf_oleacc_0116_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_oleacc_0116_v0_0_s_ifspec;

#ifndef __IAccIdentity_INTERFACE_DEFINED__
#define __IAccIdentity_INTERFACE_DEFINED__

/* interface IAccIdentity */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IAccIdentity;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7852b78d-1cfd-41c1-a615-9c0c85960b5f")
    IAccIdentity : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetIdentityString( 
            /* [in] */ DWORD dwIDChild,
            /* [size_is][size_is][out] */ BYTE **ppIDString,
            /* [out] */ DWORD *pdwIDStringLen) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAccIdentityVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAccIdentity * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAccIdentity * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAccIdentity * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetIdentityString )( 
            IAccIdentity * This,
            /* [in] */ DWORD dwIDChild,
            /* [size_is][size_is][out] */ BYTE **ppIDString,
            /* [out] */ DWORD *pdwIDStringLen);
        
        END_INTERFACE
    } IAccIdentityVtbl;

    interface IAccIdentity
    {
        CONST_VTBL struct IAccIdentityVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAccIdentity_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAccIdentity_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAccIdentity_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAccIdentity_GetIdentityString(This,dwIDChild,ppIDString,pdwIDStringLen)	\
    (This)->lpVtbl -> GetIdentityString(This,dwIDChild,ppIDString,pdwIDStringLen)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IAccIdentity_GetIdentityString_Proxy( 
    IAccIdentity * This,
    /* [in] */ DWORD dwIDChild,
    /* [size_is][size_is][out] */ BYTE **ppIDString,
    /* [out] */ DWORD *pdwIDStringLen);


void __RPC_STUB IAccIdentity_GetIdentityString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAccIdentity_INTERFACE_DEFINED__ */


#ifndef __IAccPropServer_INTERFACE_DEFINED__
#define __IAccPropServer_INTERFACE_DEFINED__

/* interface IAccPropServer */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IAccPropServer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("76c0dbbb-15e0-4e7b-b61b-20eeea2001e0")
    IAccPropServer : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPropValue( 
            /* [size_is][in] */ const BYTE *pIDString,
            /* [in] */ DWORD dwIDStringLen,
            /* [in] */ MSAAPROPID idProp,
            /* [out] */ VARIANT *pvarValue,
            /* [out] */ BOOL *pfHasProp) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAccPropServerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAccPropServer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAccPropServer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAccPropServer * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPropValue )( 
            IAccPropServer * This,
            /* [size_is][in] */ const BYTE *pIDString,
            /* [in] */ DWORD dwIDStringLen,
            /* [in] */ MSAAPROPID idProp,
            /* [out] */ VARIANT *pvarValue,
            /* [out] */ BOOL *pfHasProp);
        
        END_INTERFACE
    } IAccPropServerVtbl;

    interface IAccPropServer
    {
        CONST_VTBL struct IAccPropServerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAccPropServer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAccPropServer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAccPropServer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAccPropServer_GetPropValue(This,pIDString,dwIDStringLen,idProp,pvarValue,pfHasProp)	\
    (This)->lpVtbl -> GetPropValue(This,pIDString,dwIDStringLen,idProp,pvarValue,pfHasProp)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IAccPropServer_GetPropValue_Proxy( 
    IAccPropServer * This,
    /* [size_is][in] */ const BYTE *pIDString,
    /* [in] */ DWORD dwIDStringLen,
    /* [in] */ MSAAPROPID idProp,
    /* [out] */ VARIANT *pvarValue,
    /* [out] */ BOOL *pfHasProp);


void __RPC_STUB IAccPropServer_GetPropValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAccPropServer_INTERFACE_DEFINED__ */


#ifndef __IAccPropServices_INTERFACE_DEFINED__
#define __IAccPropServices_INTERFACE_DEFINED__

/* interface IAccPropServices */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IAccPropServices;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6e26e776-04f0-495d-80e4-3330352e3169")
    IAccPropServices : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetPropValue( 
            /* [size_is][in] */ const BYTE *pIDString,
            /* [in] */ DWORD dwIDStringLen,
            /* [in] */ MSAAPROPID idProp,
            /* [in] */ VARIANT var) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPropServer( 
            /* [size_is][in] */ const BYTE *pIDString,
            /* [in] */ DWORD dwIDStringLen,
            /* [size_is][in] */ const MSAAPROPID *paProps,
            /* [in] */ int cProps,
            /* [in] */ IAccPropServer *pServer,
            /* [in] */ AnnoScope annoScope) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ClearProps( 
            /* [size_is][in] */ const BYTE *pIDString,
            /* [in] */ DWORD dwIDStringLen,
            /* [size_is][in] */ const MSAAPROPID *paProps,
            /* [in] */ int cProps) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHwndProp( 
            /* [in] */ HWND hwnd,
            /* [in] */ DWORD idObject,
            /* [in] */ DWORD idChild,
            /* [in] */ MSAAPROPID idProp,
            /* [in] */ VARIANT var) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHwndPropStr( 
            /* [in] */ HWND hwnd,
            /* [in] */ DWORD idObject,
            /* [in] */ DWORD idChild,
            /* [in] */ MSAAPROPID idProp,
            /* [string][in] */ LPCWSTR str) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHwndPropServer( 
            /* [in] */ HWND hwnd,
            /* [in] */ DWORD idObject,
            /* [in] */ DWORD idChild,
            /* [size_is][in] */ const MSAAPROPID *paProps,
            /* [in] */ int cProps,
            /* [in] */ IAccPropServer *pServer,
            /* [in] */ AnnoScope annoScope) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ClearHwndProps( 
            /* [in] */ HWND hwnd,
            /* [in] */ DWORD idObject,
            /* [in] */ DWORD idChild,
            /* [size_is][in] */ const MSAAPROPID *paProps,
            /* [in] */ int cProps) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ComposeHwndIdentityString( 
            /* [in] */ HWND hwnd,
            /* [in] */ DWORD idObject,
            /* [in] */ DWORD idChild,
            /* [size_is][size_is][out] */ BYTE **ppIDString,
            /* [out] */ DWORD *pdwIDStringLen) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DecomposeHwndIdentityString( 
            /* [size_is][in] */ const BYTE *pIDString,
            /* [in] */ DWORD dwIDStringLen,
            /* [out] */ HWND *phwnd,
            /* [out] */ DWORD *pidObject,
            /* [out] */ DWORD *pidChild) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHmenuProp( 
            /* [in] */ HMENU hmenu,
            /* [in] */ DWORD idChild,
            /* [in] */ MSAAPROPID idProp,
            /* [in] */ VARIANT var) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHmenuPropStr( 
            /* [in] */ HMENU hmenu,
            /* [in] */ DWORD idChild,
            /* [in] */ MSAAPROPID idProp,
            /* [string][in] */ LPCWSTR str) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHmenuPropServer( 
            /* [in] */ HMENU hmenu,
            /* [in] */ DWORD idChild,
            /* [size_is][in] */ const MSAAPROPID *paProps,
            /* [in] */ int cProps,
            /* [in] */ IAccPropServer *pServer,
            /* [in] */ AnnoScope annoScope) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ClearHmenuProps( 
            /* [in] */ HMENU hmenu,
            /* [in] */ DWORD idChild,
            /* [size_is][in] */ const MSAAPROPID *paProps,
            /* [in] */ int cProps) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ComposeHmenuIdentityString( 
            /* [in] */ HMENU hmenu,
            /* [in] */ DWORD idChild,
            /* [size_is][size_is][out] */ BYTE **ppIDString,
            /* [out] */ DWORD *pdwIDStringLen) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DecomposeHmenuIdentityString( 
            /* [size_is][in] */ const BYTE *pIDString,
            /* [in] */ DWORD dwIDStringLen,
            /* [out] */ HMENU *phmenu,
            /* [out] */ DWORD *pidChild) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAccPropServicesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAccPropServices * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAccPropServices * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAccPropServices * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetPropValue )( 
            IAccPropServices * This,
            /* [size_is][in] */ const BYTE *pIDString,
            /* [in] */ DWORD dwIDStringLen,
            /* [in] */ MSAAPROPID idProp,
            /* [in] */ VARIANT var);
        
        HRESULT ( STDMETHODCALLTYPE *SetPropServer )( 
            IAccPropServices * This,
            /* [size_is][in] */ const BYTE *pIDString,
            /* [in] */ DWORD dwIDStringLen,
            /* [size_is][in] */ const MSAAPROPID *paProps,
            /* [in] */ int cProps,
            /* [in] */ IAccPropServer *pServer,
            /* [in] */ AnnoScope annoScope);
        
        HRESULT ( STDMETHODCALLTYPE *ClearProps )( 
            IAccPropServices * This,
            /* [size_is][in] */ const BYTE *pIDString,
            /* [in] */ DWORD dwIDStringLen,
            /* [size_is][in] */ const MSAAPROPID *paProps,
            /* [in] */ int cProps);
        
        HRESULT ( STDMETHODCALLTYPE *SetHwndProp )( 
            IAccPropServices * This,
            /* [in] */ HWND hwnd,
            /* [in] */ DWORD idObject,
            /* [in] */ DWORD idChild,
            /* [in] */ MSAAPROPID idProp,
            /* [in] */ VARIANT var);
        
        HRESULT ( STDMETHODCALLTYPE *SetHwndPropStr )( 
            IAccPropServices * This,
            /* [in] */ HWND hwnd,
            /* [in] */ DWORD idObject,
            /* [in] */ DWORD idChild,
            /* [in] */ MSAAPROPID idProp,
            /* [string][in] */ LPCWSTR str);
        
        HRESULT ( STDMETHODCALLTYPE *SetHwndPropServer )( 
            IAccPropServices * This,
            /* [in] */ HWND hwnd,
            /* [in] */ DWORD idObject,
            /* [in] */ DWORD idChild,
            /* [size_is][in] */ const MSAAPROPID *paProps,
            /* [in] */ int cProps,
            /* [in] */ IAccPropServer *pServer,
            /* [in] */ AnnoScope annoScope);
        
        HRESULT ( STDMETHODCALLTYPE *ClearHwndProps )( 
            IAccPropServices * This,
            /* [in] */ HWND hwnd,
            /* [in] */ DWORD idObject,
            /* [in] */ DWORD idChild,
            /* [size_is][in] */ const MSAAPROPID *paProps,
            /* [in] */ int cProps);
        
        HRESULT ( STDMETHODCALLTYPE *ComposeHwndIdentityString )( 
            IAccPropServices * This,
            /* [in] */ HWND hwnd,
            /* [in] */ DWORD idObject,
            /* [in] */ DWORD idChild,
            /* [size_is][size_is][out] */ BYTE **ppIDString,
            /* [out] */ DWORD *pdwIDStringLen);
        
        HRESULT ( STDMETHODCALLTYPE *DecomposeHwndIdentityString )( 
            IAccPropServices * This,
            /* [size_is][in] */ const BYTE *pIDString,
            /* [in] */ DWORD dwIDStringLen,
            /* [out] */ HWND *phwnd,
            /* [out] */ DWORD *pidObject,
            /* [out] */ DWORD *pidChild);
        
        HRESULT ( STDMETHODCALLTYPE *SetHmenuProp )( 
            IAccPropServices * This,
            /* [in] */ HMENU hmenu,
            /* [in] */ DWORD idChild,
            /* [in] */ MSAAPROPID idProp,
            /* [in] */ VARIANT var);
        
        HRESULT ( STDMETHODCALLTYPE *SetHmenuPropStr )( 
            IAccPropServices * This,
            /* [in] */ HMENU hmenu,
            /* [in] */ DWORD idChild,
            /* [in] */ MSAAPROPID idProp,
            /* [string][in] */ LPCWSTR str);
        
        HRESULT ( STDMETHODCALLTYPE *SetHmenuPropServer )( 
            IAccPropServices * This,
            /* [in] */ HMENU hmenu,
            /* [in] */ DWORD idChild,
            /* [size_is][in] */ const MSAAPROPID *paProps,
            /* [in] */ int cProps,
            /* [in] */ IAccPropServer *pServer,
            /* [in] */ AnnoScope annoScope);
        
        HRESULT ( STDMETHODCALLTYPE *ClearHmenuProps )( 
            IAccPropServices * This,
            /* [in] */ HMENU hmenu,
            /* [in] */ DWORD idChild,
            /* [size_is][in] */ const MSAAPROPID *paProps,
            /* [in] */ int cProps);
        
        HRESULT ( STDMETHODCALLTYPE *ComposeHmenuIdentityString )( 
            IAccPropServices * This,
            /* [in] */ HMENU hmenu,
            /* [in] */ DWORD idChild,
            /* [size_is][size_is][out] */ BYTE **ppIDString,
            /* [out] */ DWORD *pdwIDStringLen);
        
        HRESULT ( STDMETHODCALLTYPE *DecomposeHmenuIdentityString )( 
            IAccPropServices * This,
            /* [size_is][in] */ const BYTE *pIDString,
            /* [in] */ DWORD dwIDStringLen,
            /* [out] */ HMENU *phmenu,
            /* [out] */ DWORD *pidChild);
        
        END_INTERFACE
    } IAccPropServicesVtbl;

    interface IAccPropServices
    {
        CONST_VTBL struct IAccPropServicesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAccPropServices_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAccPropServices_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAccPropServices_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAccPropServices_SetPropValue(This,pIDString,dwIDStringLen,idProp,var)	\
    (This)->lpVtbl -> SetPropValue(This,pIDString,dwIDStringLen,idProp,var)

#define IAccPropServices_SetPropServer(This,pIDString,dwIDStringLen,paProps,cProps,pServer,annoScope)	\
    (This)->lpVtbl -> SetPropServer(This,pIDString,dwIDStringLen,paProps,cProps,pServer,annoScope)

#define IAccPropServices_ClearProps(This,pIDString,dwIDStringLen,paProps,cProps)	\
    (This)->lpVtbl -> ClearProps(This,pIDString,dwIDStringLen,paProps,cProps)

#define IAccPropServices_SetHwndProp(This,hwnd,idObject,idChild,idProp,var)	\
    (This)->lpVtbl -> SetHwndProp(This,hwnd,idObject,idChild,idProp,var)

#define IAccPropServices_SetHwndPropStr(This,hwnd,idObject,idChild,idProp,str)	\
    (This)->lpVtbl -> SetHwndPropStr(This,hwnd,idObject,idChild,idProp,str)

#define IAccPropServices_SetHwndPropServer(This,hwnd,idObject,idChild,paProps,cProps,pServer,annoScope)	\
    (This)->lpVtbl -> SetHwndPropServer(This,hwnd,idObject,idChild,paProps,cProps,pServer,annoScope)

#define IAccPropServices_ClearHwndProps(This,hwnd,idObject,idChild,paProps,cProps)	\
    (This)->lpVtbl -> ClearHwndProps(This,hwnd,idObject,idChild,paProps,cProps)

#define IAccPropServices_ComposeHwndIdentityString(This,hwnd,idObject,idChild,ppIDString,pdwIDStringLen)	\
    (This)->lpVtbl -> ComposeHwndIdentityString(This,hwnd,idObject,idChild,ppIDString,pdwIDStringLen)

#define IAccPropServices_DecomposeHwndIdentityString(This,pIDString,dwIDStringLen,phwnd,pidObject,pidChild)	\
    (This)->lpVtbl -> DecomposeHwndIdentityString(This,pIDString,dwIDStringLen,phwnd,pidObject,pidChild)

#define IAccPropServices_SetHmenuProp(This,hmenu,idChild,idProp,var)	\
    (This)->lpVtbl -> SetHmenuProp(This,hmenu,idChild,idProp,var)

#define IAccPropServices_SetHmenuPropStr(This,hmenu,idChild,idProp,str)	\
    (This)->lpVtbl -> SetHmenuPropStr(This,hmenu,idChild,idProp,str)

#define IAccPropServices_SetHmenuPropServer(This,hmenu,idChild,paProps,cProps,pServer,annoScope)	\
    (This)->lpVtbl -> SetHmenuPropServer(This,hmenu,idChild,paProps,cProps,pServer,annoScope)

#define IAccPropServices_ClearHmenuProps(This,hmenu,idChild,paProps,cProps)	\
    (This)->lpVtbl -> ClearHmenuProps(This,hmenu,idChild,paProps,cProps)

#define IAccPropServices_ComposeHmenuIdentityString(This,hmenu,idChild,ppIDString,pdwIDStringLen)	\
    (This)->lpVtbl -> ComposeHmenuIdentityString(This,hmenu,idChild,ppIDString,pdwIDStringLen)

#define IAccPropServices_DecomposeHmenuIdentityString(This,pIDString,dwIDStringLen,phmenu,pidChild)	\
    (This)->lpVtbl -> DecomposeHmenuIdentityString(This,pIDString,dwIDStringLen,phmenu,pidChild)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IAccPropServices_SetPropValue_Proxy( 
    IAccPropServices * This,
    /* [size_is][in] */ const BYTE *pIDString,
    /* [in] */ DWORD dwIDStringLen,
    /* [in] */ MSAAPROPID idProp,
    /* [in] */ VARIANT var);


void __RPC_STUB IAccPropServices_SetPropValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAccPropServices_SetPropServer_Proxy( 
    IAccPropServices * This,
    /* [size_is][in] */ const BYTE *pIDString,
    /* [in] */ DWORD dwIDStringLen,
    /* [size_is][in] */ const MSAAPROPID *paProps,
    /* [in] */ int cProps,
    /* [in] */ IAccPropServer *pServer,
    /* [in] */ AnnoScope annoScope);


void __RPC_STUB IAccPropServices_SetPropServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAccPropServices_ClearProps_Proxy( 
    IAccPropServices * This,
    /* [size_is][in] */ const BYTE *pIDString,
    /* [in] */ DWORD dwIDStringLen,
    /* [size_is][in] */ const MSAAPROPID *paProps,
    /* [in] */ int cProps);


void __RPC_STUB IAccPropServices_ClearProps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAccPropServices_SetHwndProp_Proxy( 
    IAccPropServices * This,
    /* [in] */ HWND hwnd,
    /* [in] */ DWORD idObject,
    /* [in] */ DWORD idChild,
    /* [in] */ MSAAPROPID idProp,
    /* [in] */ VARIANT var);


void __RPC_STUB IAccPropServices_SetHwndProp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAccPropServices_SetHwndPropStr_Proxy( 
    IAccPropServices * This,
    /* [in] */ HWND hwnd,
    /* [in] */ DWORD idObject,
    /* [in] */ DWORD idChild,
    /* [in] */ MSAAPROPID idProp,
    /* [string][in] */ LPCWSTR str);


void __RPC_STUB IAccPropServices_SetHwndPropStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAccPropServices_SetHwndPropServer_Proxy( 
    IAccPropServices * This,
    /* [in] */ HWND hwnd,
    /* [in] */ DWORD idObject,
    /* [in] */ DWORD idChild,
    /* [size_is][in] */ const MSAAPROPID *paProps,
    /* [in] */ int cProps,
    /* [in] */ IAccPropServer *pServer,
    /* [in] */ AnnoScope annoScope);


void __RPC_STUB IAccPropServices_SetHwndPropServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAccPropServices_ClearHwndProps_Proxy( 
    IAccPropServices * This,
    /* [in] */ HWND hwnd,
    /* [in] */ DWORD idObject,
    /* [in] */ DWORD idChild,
    /* [size_is][in] */ const MSAAPROPID *paProps,
    /* [in] */ int cProps);


void __RPC_STUB IAccPropServices_ClearHwndProps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAccPropServices_ComposeHwndIdentityString_Proxy( 
    IAccPropServices * This,
    /* [in] */ HWND hwnd,
    /* [in] */ DWORD idObject,
    /* [in] */ DWORD idChild,
    /* [size_is][size_is][out] */ BYTE **ppIDString,
    /* [out] */ DWORD *pdwIDStringLen);


void __RPC_STUB IAccPropServices_ComposeHwndIdentityString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAccPropServices_DecomposeHwndIdentityString_Proxy( 
    IAccPropServices * This,
    /* [size_is][in] */ const BYTE *pIDString,
    /* [in] */ DWORD dwIDStringLen,
    /* [out] */ HWND *phwnd,
    /* [out] */ DWORD *pidObject,
    /* [out] */ DWORD *pidChild);


void __RPC_STUB IAccPropServices_DecomposeHwndIdentityString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAccPropServices_SetHmenuProp_Proxy( 
    IAccPropServices * This,
    /* [in] */ HMENU hmenu,
    /* [in] */ DWORD idChild,
    /* [in] */ MSAAPROPID idProp,
    /* [in] */ VARIANT var);


void __RPC_STUB IAccPropServices_SetHmenuProp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAccPropServices_SetHmenuPropStr_Proxy( 
    IAccPropServices * This,
    /* [in] */ HMENU hmenu,
    /* [in] */ DWORD idChild,
    /* [in] */ MSAAPROPID idProp,
    /* [string][in] */ LPCWSTR str);


void __RPC_STUB IAccPropServices_SetHmenuPropStr_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAccPropServices_SetHmenuPropServer_Proxy( 
    IAccPropServices * This,
    /* [in] */ HMENU hmenu,
    /* [in] */ DWORD idChild,
    /* [size_is][in] */ const MSAAPROPID *paProps,
    /* [in] */ int cProps,
    /* [in] */ IAccPropServer *pServer,
    /* [in] */ AnnoScope annoScope);


void __RPC_STUB IAccPropServices_SetHmenuPropServer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAccPropServices_ClearHmenuProps_Proxy( 
    IAccPropServices * This,
    /* [in] */ HMENU hmenu,
    /* [in] */ DWORD idChild,
    /* [size_is][in] */ const MSAAPROPID *paProps,
    /* [in] */ int cProps);


void __RPC_STUB IAccPropServices_ClearHmenuProps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAccPropServices_ComposeHmenuIdentityString_Proxy( 
    IAccPropServices * This,
    /* [in] */ HMENU hmenu,
    /* [in] */ DWORD idChild,
    /* [size_is][size_is][out] */ BYTE **ppIDString,
    /* [out] */ DWORD *pdwIDStringLen);


void __RPC_STUB IAccPropServices_ComposeHmenuIdentityString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAccPropServices_DecomposeHmenuIdentityString_Proxy( 
    IAccPropServices * This,
    /* [size_is][in] */ const BYTE *pIDString,
    /* [in] */ DWORD dwIDStringLen,
    /* [out] */ HMENU *phmenu,
    /* [out] */ DWORD *pidChild);


void __RPC_STUB IAccPropServices_DecomposeHmenuIdentityString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAccPropServices_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_oleacc_0119 */
/* [local] */ 


//=--------------------------------------------------------------------------=
// Type Library Definitions
//=--------------------------------------------------------------------------=



extern RPC_IF_HANDLE __MIDL_itf_oleacc_0119_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_oleacc_0119_v0_0_s_ifspec;


#ifndef __Accessibility_LIBRARY_DEFINED__
#define __Accessibility_LIBRARY_DEFINED__

/* library Accessibility */
/* [hidden][version][lcid][uuid] */ 







EXTERN_C const IID LIBID_Accessibility;

EXTERN_C const CLSID CLSID_CAccPropServices;

#ifdef __cplusplus

class DECLSPEC_UUID("b5f8350b-0548-48b1-a6ee-88bd00b4a5e7")
CAccPropServices;
#endif
#endif /* __Accessibility_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  HMENU_UserSize(     unsigned long *, unsigned long            , HMENU * ); 
unsigned char * __RPC_USER  HMENU_UserMarshal(  unsigned long *, unsigned char *, HMENU * ); 
unsigned char * __RPC_USER  HMENU_UserUnmarshal(unsigned long *, unsigned char *, HMENU * ); 
void                      __RPC_USER  HMENU_UserFree(     unsigned long *, HMENU * ); 

unsigned long             __RPC_USER  HWND_UserSize(     unsigned long *, unsigned long            , HWND * ); 
unsigned char * __RPC_USER  HWND_UserMarshal(  unsigned long *, unsigned char *, HWND * ); 
unsigned char * __RPC_USER  HWND_UserUnmarshal(unsigned long *, unsigned char *, HWND * ); 
void                      __RPC_USER  HWND_UserFree(     unsigned long *, HWND * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



