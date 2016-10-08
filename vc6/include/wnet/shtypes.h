

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* Compiler settings for shtypes.idl:
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


#ifndef __shtypes_h__
#define __shtypes_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

/* header files for imported files */
#include "wtypes.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_shtypes_0000 */
/* [local] */ 

//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (c) Microsoft Corporation. All rights reserved.
//
//--------------------------------------------------------------------------
//===========================================================================
//
// Object identifiers in the explorer's name space (ItemID and IDList)
//
//  All the items that the user can browse with the explorer (such as files,
// directories, servers, work-groups, etc.) has an identifier which is unique
// among items within the parent folder. Those identifiers are called item
// IDs (SHITEMID). Since all its parent folders have their own item IDs,
// any items can be uniquely identified by a list of item IDs, which is called
// an ID list (ITEMIDLIST).
//
//  ID lists are almost always allocated by the task allocator (see some
// description below as well as OLE 2.0 SDK) and may be passed across
// some of shell interfaces (such as IShellFolder). Each item ID in an ID list
// is only meaningful to its parent folder (which has generated it), and all
// the clients must treat it as an opaque binary data except the first two
// bytes, which indicates the size of the item ID.
//
//  When a shell extension -- which implements the IShellFolder interace --
// generates an item ID, it may put any information in it, not only the data
// with that it needs to identifies the item, but also some additional
// information, which would help implementing some other functions efficiently.
// For example, the shell's IShellFolder implementation of file system items
// stores the primary (long) name of a file or a directory as the item
// identifier, but it also stores its alternative (short) name, size and date
// etc.
//
//  When an ID list is passed to one of shell APIs (such as SHGetPathFromIDList),
// it is always an absolute path -- relative from the root of the name space,
// which is the desktop folder. When an ID list is passed to one of IShellFolder
// member function, it is always a relative path from the folder (unless it
// is explicitly specified).
//
//===========================================================================
//
// SHITEMID -- Item ID  (mkid)
//     USHORT      cb;             // Size of the ID (including cb itself)
//     BYTE        abID[];         // The item ID (variable length)
//
#include <pshpack1.h>
typedef struct _SHITEMID
    {
    USHORT cb;
    BYTE abID[ 1 ];
    } 	SHITEMID;

#include <poppack.h>
#if defined(_M_IX86)
#define __unaligned
#endif // __unaligned
typedef SHITEMID __unaligned *LPSHITEMID;

typedef const SHITEMID __unaligned *LPCSHITEMID;

//
// ITEMIDLIST -- List if item IDs (combined with 0-terminator)
//
#include <pshpack1.h>
typedef struct _ITEMIDLIST
    {
    SHITEMID mkid;
    } 	ITEMIDLIST;

#include <poppack.h>
typedef /* [unique] */ BYTE_BLOB *wirePIDL;

typedef /* [wire_marshal] */ ITEMIDLIST __unaligned *LPITEMIDLIST;

typedef /* [wire_marshal] */ const ITEMIDLIST __unaligned *LPCITEMIDLIST;

#ifdef WINBASE_DEFINED_MIDL
typedef struct _WIN32_FIND_DATAA
    {
    DWORD bData[ 80 ];
    } 	WIN32_FIND_DATAA;

typedef struct _WIN32_FIND_DATAW
    {
    DWORD bData[ 148 ];
    } 	WIN32_FIND_DATAW;

#endif // WINBASE_DEFINED_MIDL
//-------------------------------------------------------------------------
//
// struct STRRET
//
// structure for returning strings from IShellFolder member functions
//
//-------------------------------------------------------------------------
//
//  uType indicate which union member to use 
//    STRRET_WSTR    Use STRRET.pOleStr     must be freed by caller of GetDisplayNameOf
//    STRRET_OFFSET  Use STRRET.uOffset     Offset into SHITEMID for ANSI string 
//    STRRET_CSTR    Use STRRET.cStr        ANSI Buffer
//
typedef /* [v1_enum] */ 
enum tagSTRRET_TYPE
    {	STRRET_WSTR	= 0,
	STRRET_OFFSET	= 0x1,
	STRRET_CSTR	= 0x2
    } 	STRRET_TYPE;

#include <pshpack8.h>
typedef struct _STRRET
    {
    UINT uType;
    /* [switch_is][switch_type] */ union 
        {
        /* [case()][string] */ LPWSTR pOleStr;
        /* [case()] */ UINT uOffset;
        /* [case()] */ char cStr[ 260 ];
        } 	DUMMYUNIONNAME;
    } 	STRRET;

#include <poppack.h>
typedef STRRET *LPSTRRET;

//-------------------------------------------------------------------------
//
// struct SHELLDETAILS
//
// structure for returning strings from IShellDetails
//
//-------------------------------------------------------------------------
//
//  fmt;            // LVCFMT_* value (header only)
//  cxChar;         // Number of 'average' characters (header only)
//  str;            // String information
//
#include <pshpack1.h>
typedef struct _SHELLDETAILS
    {
    int fmt;
    int cxChar;
    STRRET str;
    } 	SHELLDETAILS;

typedef struct _SHELLDETAILS *LPSHELLDETAILS;

#include <poppack.h>


extern RPC_IF_HANDLE __MIDL_itf_shtypes_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shtypes_0000_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



