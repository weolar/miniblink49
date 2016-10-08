

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* Compiler settings for objsafe.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
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

#ifndef __objsafe_h__
#define __objsafe_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IObjectSafety_FWD_DEFINED__
#define __IObjectSafety_FWD_DEFINED__
typedef interface IObjectSafety IObjectSafety;
#endif 	/* __IObjectSafety_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_objsafe_0000 */
/* [local] */ 

//=--------------------------------------------------------------------------=
// ObjSafe.h
//=--------------------------------------------------------------------------=
// (C) Copyright 1995-1998 Microsoft Corporation.  All Rights Reserved.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//=--------------------------------------------------------------------------=

#pragma comment(lib,"uuid.lib")

//---------------------------------------------------------------------------=
// Object Safety Interfaces.

//+--------------------------------------------------------------------------=
//
//  Contents:   IObjectSafety definition
//
//
//  IObjectSafety should be implemented by objects that have interfaces which
//      support "untrusted" clients (for example, scripts). It allows the owner of
//      the object to specify which interfaces need to be protected from untrusted
//      use. Examples of interfaces that might be protected in this way are:
//
//      IID_IDispatch           - "Safe for automating with untrusted automation client or script"
//      IID_IPersist*           - "Safe for initializing with untrusted data"
//      IID_IActiveScript       - "Safe for running untrusted scripts"
//
//---------------------------------------------------------------------------=
#ifndef _LPSAFEOBJECT_DEFINED
#define _LPSAFEOBJECT_DEFINED

// Option bit definitions for IObjectSafety:
#define	INTERFACESAFE_FOR_UNTRUSTED_CALLER	0x00000001	// Caller of interface may be untrusted
#define	INTERFACESAFE_FOR_UNTRUSTED_DATA	0x00000002	// Data passed into interface may be untrusted
#define	INTERFACE_USES_DISPEX	            0x00000004	// Object knows to use IDispatchEx
#define	INTERFACE_USES_SECURITY_MANAGER	    0x00000008	// Object knows to use IInternetHostSecurityManager

// {CB5BDC81-93C1-11cf-8F20-00805F2CD064}
DEFINE_GUID(IID_IObjectSafety, 0xcb5bdc81, 0x93c1, 0x11cf, 0x8f, 0x20, 0x0, 0x80, 0x5f, 0x2c, 0xd0, 0x64);
EXTERN_C GUID CATID_SafeForScripting;
EXTERN_C GUID CATID_SafeForInitializing;



extern RPC_IF_HANDLE __MIDL_itf_objsafe_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objsafe_0000_v0_0_s_ifspec;

#ifndef __IObjectSafety_INTERFACE_DEFINED__
#define __IObjectSafety_INTERFACE_DEFINED__

/* interface IObjectSafety */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IObjectSafety;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CB5BDC81-93C1-11cf-8F20-00805F2CD064")
    IObjectSafety : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetInterfaceSafetyOptions( 
            /* [in] */ REFIID riid,
            /* [out] */ DWORD *pdwSupportedOptions,
            /* [out] */ DWORD *pdwEnabledOptions) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetInterfaceSafetyOptions( 
            /* [in] */ REFIID riid,
            /* [in] */ DWORD dwOptionSetMask,
            /* [in] */ DWORD dwEnabledOptions) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IObjectSafetyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IObjectSafety * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IObjectSafety * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IObjectSafety * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInterfaceSafetyOptions )( 
            IObjectSafety * This,
            /* [in] */ REFIID riid,
            /* [out] */ DWORD *pdwSupportedOptions,
            /* [out] */ DWORD *pdwEnabledOptions);
        
        HRESULT ( STDMETHODCALLTYPE *SetInterfaceSafetyOptions )( 
            IObjectSafety * This,
            /* [in] */ REFIID riid,
            /* [in] */ DWORD dwOptionSetMask,
            /* [in] */ DWORD dwEnabledOptions);
        
        END_INTERFACE
    } IObjectSafetyVtbl;

    interface IObjectSafety
    {
        CONST_VTBL struct IObjectSafetyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjectSafety_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IObjectSafety_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IObjectSafety_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IObjectSafety_GetInterfaceSafetyOptions(This,riid,pdwSupportedOptions,pdwEnabledOptions)	\
    (This)->lpVtbl -> GetInterfaceSafetyOptions(This,riid,pdwSupportedOptions,pdwEnabledOptions)

#define IObjectSafety_SetInterfaceSafetyOptions(This,riid,dwOptionSetMask,dwEnabledOptions)	\
    (This)->lpVtbl -> SetInterfaceSafetyOptions(This,riid,dwOptionSetMask,dwEnabledOptions)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IObjectSafety_GetInterfaceSafetyOptions_Proxy( 
    IObjectSafety * This,
    /* [in] */ REFIID riid,
    /* [out] */ DWORD *pdwSupportedOptions,
    /* [out] */ DWORD *pdwEnabledOptions);


void __RPC_STUB IObjectSafety_GetInterfaceSafetyOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IObjectSafety_SetInterfaceSafetyOptions_Proxy( 
    IObjectSafety * This,
    /* [in] */ REFIID riid,
    /* [in] */ DWORD dwOptionSetMask,
    /* [in] */ DWORD dwEnabledOptions);


void __RPC_STUB IObjectSafety_SetInterfaceSafetyOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IObjectSafety_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_objsafe_0009 */
/* [local] */ 

typedef /* [unique] */ IObjectSafety *LPOBJECTSAFETY;

#endif


extern RPC_IF_HANDLE __MIDL_itf_objsafe_0009_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_objsafe_0009_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



