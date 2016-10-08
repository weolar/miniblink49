

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* Compiler settings for comcat.idl:
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

#ifndef __comcat_h__
#define __comcat_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IEnumGUID_FWD_DEFINED__
#define __IEnumGUID_FWD_DEFINED__
typedef interface IEnumGUID IEnumGUID;
#endif 	/* __IEnumGUID_FWD_DEFINED__ */


#ifndef __IEnumCATEGORYINFO_FWD_DEFINED__
#define __IEnumCATEGORYINFO_FWD_DEFINED__
typedef interface IEnumCATEGORYINFO IEnumCATEGORYINFO;
#endif 	/* __IEnumCATEGORYINFO_FWD_DEFINED__ */


#ifndef __ICatRegister_FWD_DEFINED__
#define __ICatRegister_FWD_DEFINED__
typedef interface ICatRegister ICatRegister;
#endif 	/* __ICatRegister_FWD_DEFINED__ */


#ifndef __ICatInformation_FWD_DEFINED__
#define __ICatInformation_FWD_DEFINED__
typedef interface ICatInformation ICatInformation;
#endif 	/* __ICatInformation_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_comcat_0000 */
/* [local] */ 

//=--------------------------------------------------------------------------=
// ComCat.h
//=--------------------------------------------------------------------------=
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//=--------------------------------------------------------------------------=

#pragma comment(lib,"uuid.lib")

//=--------------------------------------------------------------------------=
// OLE Componet Categories Interfaces.
//=--------------------------------------------------------------------------=
//




EXTERN_C const CLSID CLSID_StdComponentCategoriesMgr;

////////////////////////////////////////////////////////////////////////////
//  Types
typedef GUID CATID;

typedef REFGUID REFCATID;

#define IID_IEnumCLSID              IID_IEnumGUID
#define IEnumCLSID                  IEnumGUID
#define LPENUMCLSID                 LPENUMGUID
#define CATID_NULL                   GUID_NULL
#define IsEqualCATID(rcatid1, rcatid2)       IsEqualGUID(rcatid1, rcatid2)
#define IID_IEnumCATID       IID_IEnumGUID
#define IEnumCATID           IEnumGUID

////////////////////////////////////////////////////////////////////////////
//  Category IDs (link to uuid3.lib)
EXTERN_C const CATID CATID_Insertable;
EXTERN_C const CATID CATID_Control;
EXTERN_C const CATID CATID_Programmable;
EXTERN_C const CATID CATID_IsShortcut;
EXTERN_C const CATID CATID_NeverShowExt;
EXTERN_C const CATID CATID_DocObject;
EXTERN_C const CATID CATID_Printable;
EXTERN_C const CATID CATID_RequiresDataPathHost;
EXTERN_C const CATID CATID_PersistsToMoniker;
EXTERN_C const CATID CATID_PersistsToStorage;
EXTERN_C const CATID CATID_PersistsToStreamInit;
EXTERN_C const CATID CATID_PersistsToStream;
EXTERN_C const CATID CATID_PersistsToMemory;
EXTERN_C const CATID CATID_PersistsToFile;
EXTERN_C const CATID CATID_PersistsToPropertyBag;
EXTERN_C const CATID CATID_InternetAware;
EXTERN_C const CATID CATID_DesignTimeUIActivatableControl;

////////////////////////////////////////////////////////////////////////////
//  Interface Definitions
#ifndef _LPENUMGUID_DEFINED
#define _LPENUMGUID_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_comcat_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_comcat_0000_v0_0_s_ifspec;

#ifndef __IEnumGUID_INTERFACE_DEFINED__
#define __IEnumGUID_INTERFACE_DEFINED__

/* interface IEnumGUID */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IEnumGUID *LPENUMGUID;


EXTERN_C const IID IID_IEnumGUID;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0002E000-0000-0000-C000-000000000046")
    IEnumGUID : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ GUID *rgelt,
            /* [out] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumGUID **ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumGUIDVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumGUID * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumGUID * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumGUID * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumGUID * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ GUID *rgelt,
            /* [out] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumGUID * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumGUID * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumGUID * This,
            /* [out] */ IEnumGUID **ppenum);
        
        END_INTERFACE
    } IEnumGUIDVtbl;

    interface IEnumGUID
    {
        CONST_VTBL struct IEnumGUIDVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumGUID_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumGUID_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumGUID_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumGUID_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumGUID_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumGUID_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumGUID_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumGUID_Next_Proxy( 
    IEnumGUID * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ GUID *rgelt,
    /* [out] */ ULONG *pceltFetched);


void __RPC_STUB IEnumGUID_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumGUID_Skip_Proxy( 
    IEnumGUID * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumGUID_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumGUID_Reset_Proxy( 
    IEnumGUID * This);


void __RPC_STUB IEnumGUID_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumGUID_Clone_Proxy( 
    IEnumGUID * This,
    /* [out] */ IEnumGUID **ppenum);


void __RPC_STUB IEnumGUID_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumGUID_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_comcat_0009 */
/* [local] */ 

#endif
#ifndef _LPENUMCATEGORYINFO_DEFINED
#define _LPENUMCATEGORYINFO_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_comcat_0009_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_comcat_0009_v0_0_s_ifspec;

#ifndef __IEnumCATEGORYINFO_INTERFACE_DEFINED__
#define __IEnumCATEGORYINFO_INTERFACE_DEFINED__

/* interface IEnumCATEGORYINFO */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IEnumCATEGORYINFO *LPENUMCATEGORYINFO;

typedef struct tagCATEGORYINFO
    {
    CATID catid;
    LCID lcid;
    OLECHAR szDescription[ 128 ];
    } 	CATEGORYINFO;

typedef struct tagCATEGORYINFO *LPCATEGORYINFO;


EXTERN_C const IID IID_IEnumCATEGORYINFO;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0002E011-0000-0000-C000-000000000046")
    IEnumCATEGORYINFO : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ CATEGORYINFO *rgelt,
            /* [out] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumCATEGORYINFO **ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumCATEGORYINFOVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumCATEGORYINFO * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumCATEGORYINFO * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumCATEGORYINFO * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumCATEGORYINFO * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ CATEGORYINFO *rgelt,
            /* [out] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumCATEGORYINFO * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumCATEGORYINFO * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumCATEGORYINFO * This,
            /* [out] */ IEnumCATEGORYINFO **ppenum);
        
        END_INTERFACE
    } IEnumCATEGORYINFOVtbl;

    interface IEnumCATEGORYINFO
    {
        CONST_VTBL struct IEnumCATEGORYINFOVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumCATEGORYINFO_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumCATEGORYINFO_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumCATEGORYINFO_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumCATEGORYINFO_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumCATEGORYINFO_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumCATEGORYINFO_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumCATEGORYINFO_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumCATEGORYINFO_Next_Proxy( 
    IEnumCATEGORYINFO * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ CATEGORYINFO *rgelt,
    /* [out] */ ULONG *pceltFetched);


void __RPC_STUB IEnumCATEGORYINFO_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCATEGORYINFO_Skip_Proxy( 
    IEnumCATEGORYINFO * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumCATEGORYINFO_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCATEGORYINFO_Reset_Proxy( 
    IEnumCATEGORYINFO * This);


void __RPC_STUB IEnumCATEGORYINFO_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumCATEGORYINFO_Clone_Proxy( 
    IEnumCATEGORYINFO * This,
    /* [out] */ IEnumCATEGORYINFO **ppenum);


void __RPC_STUB IEnumCATEGORYINFO_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumCATEGORYINFO_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_comcat_0010 */
/* [local] */ 

#endif
#ifndef _LPCATREGISTER_DEFINED
#define _LPCATREGISTER_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_comcat_0010_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_comcat_0010_v0_0_s_ifspec;

#ifndef __ICatRegister_INTERFACE_DEFINED__
#define __ICatRegister_INTERFACE_DEFINED__

/* interface ICatRegister */
/* [unique][uuid][object] */ 

typedef /* [unique] */ ICatRegister *LPCATREGISTER;


EXTERN_C const IID IID_ICatRegister;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0002E012-0000-0000-C000-000000000046")
    ICatRegister : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE RegisterCategories( 
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATEGORYINFO rgCategoryInfo[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnRegisterCategories( 
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID rgcatid[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RegisterClassImplCategories( 
            /* [in] */ REFCLSID rclsid,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID rgcatid[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnRegisterClassImplCategories( 
            /* [in] */ REFCLSID rclsid,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID rgcatid[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RegisterClassReqCategories( 
            /* [in] */ REFCLSID rclsid,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID rgcatid[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnRegisterClassReqCategories( 
            /* [in] */ REFCLSID rclsid,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID rgcatid[  ]) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICatRegisterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICatRegister * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICatRegister * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICatRegister * This);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterCategories )( 
            ICatRegister * This,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATEGORYINFO rgCategoryInfo[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *UnRegisterCategories )( 
            ICatRegister * This,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID rgcatid[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterClassImplCategories )( 
            ICatRegister * This,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID rgcatid[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *UnRegisterClassImplCategories )( 
            ICatRegister * This,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID rgcatid[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterClassReqCategories )( 
            ICatRegister * This,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID rgcatid[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *UnRegisterClassReqCategories )( 
            ICatRegister * This,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ ULONG cCategories,
            /* [size_is][in] */ CATID rgcatid[  ]);
        
        END_INTERFACE
    } ICatRegisterVtbl;

    interface ICatRegister
    {
        CONST_VTBL struct ICatRegisterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICatRegister_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICatRegister_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICatRegister_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICatRegister_RegisterCategories(This,cCategories,rgCategoryInfo)	\
    (This)->lpVtbl -> RegisterCategories(This,cCategories,rgCategoryInfo)

#define ICatRegister_UnRegisterCategories(This,cCategories,rgcatid)	\
    (This)->lpVtbl -> UnRegisterCategories(This,cCategories,rgcatid)

#define ICatRegister_RegisterClassImplCategories(This,rclsid,cCategories,rgcatid)	\
    (This)->lpVtbl -> RegisterClassImplCategories(This,rclsid,cCategories,rgcatid)

#define ICatRegister_UnRegisterClassImplCategories(This,rclsid,cCategories,rgcatid)	\
    (This)->lpVtbl -> UnRegisterClassImplCategories(This,rclsid,cCategories,rgcatid)

#define ICatRegister_RegisterClassReqCategories(This,rclsid,cCategories,rgcatid)	\
    (This)->lpVtbl -> RegisterClassReqCategories(This,rclsid,cCategories,rgcatid)

#define ICatRegister_UnRegisterClassReqCategories(This,rclsid,cCategories,rgcatid)	\
    (This)->lpVtbl -> UnRegisterClassReqCategories(This,rclsid,cCategories,rgcatid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICatRegister_RegisterCategories_Proxy( 
    ICatRegister * This,
    /* [in] */ ULONG cCategories,
    /* [size_is][in] */ CATEGORYINFO rgCategoryInfo[  ]);


void __RPC_STUB ICatRegister_RegisterCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICatRegister_UnRegisterCategories_Proxy( 
    ICatRegister * This,
    /* [in] */ ULONG cCategories,
    /* [size_is][in] */ CATID rgcatid[  ]);


void __RPC_STUB ICatRegister_UnRegisterCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICatRegister_RegisterClassImplCategories_Proxy( 
    ICatRegister * This,
    /* [in] */ REFCLSID rclsid,
    /* [in] */ ULONG cCategories,
    /* [size_is][in] */ CATID rgcatid[  ]);


void __RPC_STUB ICatRegister_RegisterClassImplCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICatRegister_UnRegisterClassImplCategories_Proxy( 
    ICatRegister * This,
    /* [in] */ REFCLSID rclsid,
    /* [in] */ ULONG cCategories,
    /* [size_is][in] */ CATID rgcatid[  ]);


void __RPC_STUB ICatRegister_UnRegisterClassImplCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICatRegister_RegisterClassReqCategories_Proxy( 
    ICatRegister * This,
    /* [in] */ REFCLSID rclsid,
    /* [in] */ ULONG cCategories,
    /* [size_is][in] */ CATID rgcatid[  ]);


void __RPC_STUB ICatRegister_RegisterClassReqCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICatRegister_UnRegisterClassReqCategories_Proxy( 
    ICatRegister * This,
    /* [in] */ REFCLSID rclsid,
    /* [in] */ ULONG cCategories,
    /* [size_is][in] */ CATID rgcatid[  ]);


void __RPC_STUB ICatRegister_UnRegisterClassReqCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICatRegister_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_comcat_0011 */
/* [local] */ 

#endif
#ifndef _LPCATINFORMATION_DEFINED
#define _LPCATINFORMATION_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_comcat_0011_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_comcat_0011_v0_0_s_ifspec;

#ifndef __ICatInformation_INTERFACE_DEFINED__
#define __ICatInformation_INTERFACE_DEFINED__

/* interface ICatInformation */
/* [unique][uuid][object] */ 

typedef /* [unique] */ ICatInformation *LPCATINFORMATION;


EXTERN_C const IID IID_ICatInformation;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0002E013-0000-0000-C000-000000000046")
    ICatInformation : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnumCategories( 
            /* [in] */ LCID lcid,
            /* [out] */ IEnumCATEGORYINFO **ppenumCategoryInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCategoryDesc( 
            /* [in] */ REFCATID rcatid,
            /* [in] */ LCID lcid,
            /* [out] */ LPWSTR *pszDesc) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE EnumClassesOfCategories( 
            /* [in] */ ULONG cImplemented,
            /* [size_is][in] */ CATID rgcatidImpl[  ],
            /* [in] */ ULONG cRequired,
            /* [size_is][in] */ CATID rgcatidReq[  ],
            /* [out] */ IEnumGUID **ppenumClsid) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE IsClassOfCategories( 
            /* [in] */ REFCLSID rclsid,
            /* [in] */ ULONG cImplemented,
            /* [size_is][in] */ CATID rgcatidImpl[  ],
            /* [in] */ ULONG cRequired,
            /* [size_is][in] */ CATID rgcatidReq[  ]) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumImplCategoriesOfClass( 
            /* [in] */ REFCLSID rclsid,
            /* [out] */ IEnumGUID **ppenumCatid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumReqCategoriesOfClass( 
            /* [in] */ REFCLSID rclsid,
            /* [out] */ IEnumGUID **ppenumCatid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICatInformationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICatInformation * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICatInformation * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICatInformation * This);
        
        HRESULT ( STDMETHODCALLTYPE *EnumCategories )( 
            ICatInformation * This,
            /* [in] */ LCID lcid,
            /* [out] */ IEnumCATEGORYINFO **ppenumCategoryInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetCategoryDesc )( 
            ICatInformation * This,
            /* [in] */ REFCATID rcatid,
            /* [in] */ LCID lcid,
            /* [out] */ LPWSTR *pszDesc);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *EnumClassesOfCategories )( 
            ICatInformation * This,
            /* [in] */ ULONG cImplemented,
            /* [size_is][in] */ CATID rgcatidImpl[  ],
            /* [in] */ ULONG cRequired,
            /* [size_is][in] */ CATID rgcatidReq[  ],
            /* [out] */ IEnumGUID **ppenumClsid);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *IsClassOfCategories )( 
            ICatInformation * This,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ ULONG cImplemented,
            /* [size_is][in] */ CATID rgcatidImpl[  ],
            /* [in] */ ULONG cRequired,
            /* [size_is][in] */ CATID rgcatidReq[  ]);
        
        HRESULT ( STDMETHODCALLTYPE *EnumImplCategoriesOfClass )( 
            ICatInformation * This,
            /* [in] */ REFCLSID rclsid,
            /* [out] */ IEnumGUID **ppenumCatid);
        
        HRESULT ( STDMETHODCALLTYPE *EnumReqCategoriesOfClass )( 
            ICatInformation * This,
            /* [in] */ REFCLSID rclsid,
            /* [out] */ IEnumGUID **ppenumCatid);
        
        END_INTERFACE
    } ICatInformationVtbl;

    interface ICatInformation
    {
        CONST_VTBL struct ICatInformationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICatInformation_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICatInformation_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICatInformation_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICatInformation_EnumCategories(This,lcid,ppenumCategoryInfo)	\
    (This)->lpVtbl -> EnumCategories(This,lcid,ppenumCategoryInfo)

#define ICatInformation_GetCategoryDesc(This,rcatid,lcid,pszDesc)	\
    (This)->lpVtbl -> GetCategoryDesc(This,rcatid,lcid,pszDesc)

#define ICatInformation_EnumClassesOfCategories(This,cImplemented,rgcatidImpl,cRequired,rgcatidReq,ppenumClsid)	\
    (This)->lpVtbl -> EnumClassesOfCategories(This,cImplemented,rgcatidImpl,cRequired,rgcatidReq,ppenumClsid)

#define ICatInformation_IsClassOfCategories(This,rclsid,cImplemented,rgcatidImpl,cRequired,rgcatidReq)	\
    (This)->lpVtbl -> IsClassOfCategories(This,rclsid,cImplemented,rgcatidImpl,cRequired,rgcatidReq)

#define ICatInformation_EnumImplCategoriesOfClass(This,rclsid,ppenumCatid)	\
    (This)->lpVtbl -> EnumImplCategoriesOfClass(This,rclsid,ppenumCatid)

#define ICatInformation_EnumReqCategoriesOfClass(This,rclsid,ppenumCatid)	\
    (This)->lpVtbl -> EnumReqCategoriesOfClass(This,rclsid,ppenumCatid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICatInformation_EnumCategories_Proxy( 
    ICatInformation * This,
    /* [in] */ LCID lcid,
    /* [out] */ IEnumCATEGORYINFO **ppenumCategoryInfo);


void __RPC_STUB ICatInformation_EnumCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICatInformation_GetCategoryDesc_Proxy( 
    ICatInformation * This,
    /* [in] */ REFCATID rcatid,
    /* [in] */ LCID lcid,
    /* [out] */ LPWSTR *pszDesc);


void __RPC_STUB ICatInformation_GetCategoryDesc_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE ICatInformation_RemoteEnumClassesOfCategories_Proxy( 
    ICatInformation * This,
    /* [in] */ ULONG cImplemented,
    /* [size_is][unique][in] */ CATID rgcatidImpl[  ],
    /* [in] */ ULONG cRequired,
    /* [size_is][unique][in] */ CATID rgcatidReq[  ],
    /* [out] */ IEnumGUID **ppenumClsid);


void __RPC_STUB ICatInformation_RemoteEnumClassesOfCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE ICatInformation_RemoteIsClassOfCategories_Proxy( 
    ICatInformation * This,
    /* [in] */ REFCLSID rclsid,
    /* [in] */ ULONG cImplemented,
    /* [size_is][unique][in] */ CATID rgcatidImpl[  ],
    /* [in] */ ULONG cRequired,
    /* [size_is][unique][in] */ CATID rgcatidReq[  ]);


void __RPC_STUB ICatInformation_RemoteIsClassOfCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICatInformation_EnumImplCategoriesOfClass_Proxy( 
    ICatInformation * This,
    /* [in] */ REFCLSID rclsid,
    /* [out] */ IEnumGUID **ppenumCatid);


void __RPC_STUB ICatInformation_EnumImplCategoriesOfClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICatInformation_EnumReqCategoriesOfClass_Proxy( 
    ICatInformation * This,
    /* [in] */ REFCLSID rclsid,
    /* [out] */ IEnumGUID **ppenumCatid);


void __RPC_STUB ICatInformation_EnumReqCategoriesOfClass_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICatInformation_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_comcat_0012 */
/* [local] */ 

#endif


extern RPC_IF_HANDLE __MIDL_itf_comcat_0012_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_comcat_0012_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* [local] */ HRESULT STDMETHODCALLTYPE ICatInformation_EnumClassesOfCategories_Proxy( 
    ICatInformation * This,
    /* [in] */ ULONG cImplemented,
    /* [size_is][in] */ CATID rgcatidImpl[  ],
    /* [in] */ ULONG cRequired,
    /* [size_is][in] */ CATID rgcatidReq[  ],
    /* [out] */ IEnumGUID **ppenumClsid);


/* [call_as] */ HRESULT STDMETHODCALLTYPE ICatInformation_EnumClassesOfCategories_Stub( 
    ICatInformation * This,
    /* [in] */ ULONG cImplemented,
    /* [size_is][unique][in] */ CATID rgcatidImpl[  ],
    /* [in] */ ULONG cRequired,
    /* [size_is][unique][in] */ CATID rgcatidReq[  ],
    /* [out] */ IEnumGUID **ppenumClsid);

/* [local] */ HRESULT STDMETHODCALLTYPE ICatInformation_IsClassOfCategories_Proxy( 
    ICatInformation * This,
    /* [in] */ REFCLSID rclsid,
    /* [in] */ ULONG cImplemented,
    /* [size_is][in] */ CATID rgcatidImpl[  ],
    /* [in] */ ULONG cRequired,
    /* [size_is][in] */ CATID rgcatidReq[  ]);


/* [call_as] */ HRESULT STDMETHODCALLTYPE ICatInformation_IsClassOfCategories_Stub( 
    ICatInformation * This,
    /* [in] */ REFCLSID rclsid,
    /* [in] */ ULONG cImplemented,
    /* [size_is][unique][in] */ CATID rgcatidImpl[  ],
    /* [in] */ ULONG cRequired,
    /* [size_is][unique][in] */ CATID rgcatidReq[  ]);



/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



