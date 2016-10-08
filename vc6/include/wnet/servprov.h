

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* Compiler settings for servprov.idl:
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

#ifndef __servprov_h__
#define __servprov_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IServiceProvider_FWD_DEFINED__
#define __IServiceProvider_FWD_DEFINED__
typedef interface IServiceProvider IServiceProvider;
#endif 	/* __IServiceProvider_FWD_DEFINED__ */


/* header files for imported files */
#include "objidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_servprov_0000 */
/* [local] */ 

//=--------------------------------------------------------------------------=
// ServProv.h
//=--------------------------------------------------------------------------=
// (C) Copyright 1995-1999 Microsoft Corporation.  All Rights Reserved.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//=--------------------------------------------------------------------------=

#pragma comment(lib,"uuid.lib")

//---------------------------------------------------------------------------=
// IServiceProvider Interfaces.




extern RPC_IF_HANDLE __MIDL_itf_servprov_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_servprov_0000_v0_0_s_ifspec;

#ifndef __IServiceProvider_INTERFACE_DEFINED__
#define __IServiceProvider_INTERFACE_DEFINED__

/* interface IServiceProvider */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IServiceProvider *LPSERVICEPROVIDER;

#if (_MSC_VER >= 1100) && defined(__cplusplus) && !defined(CINTERFACE)
    EXTERN_C const IID IID_IServiceProvider;
    extern "C++"
    {
        MIDL_INTERFACE("6d5140c1-7436-11ce-8034-00aa006009fa")
        IServiceProvider : public IUnknown
        {
        public:
            virtual /* [local] */ HRESULT STDMETHODCALLTYPE QueryService( 
                /* [in] */ REFGUID guidService,
                /* [in] */ REFIID riid,
                /* [out] */ void __RPC_FAR *__RPC_FAR *ppvObject) = 0;
            
            template <class Q>
            HRESULT STDMETHODCALLTYPE QueryService(REFGUID guidService, Q** pp)
            {
                return QueryService(guidService, __uuidof(Q), (void **)pp);
            }
        };
    }

    /* [call_as] */ HRESULT STDMETHODCALLTYPE IServiceProvider_RemoteQueryService_Proxy( 
        IServiceProvider __RPC_FAR * This,
        /* [in] */ REFGUID guidService,
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ IUnknown __RPC_FAR *__RPC_FAR *ppvObject);

    void __RPC_STUB IServiceProvider_RemoteQueryService_Stub(
        IRpcStubBuffer *This,
        IRpcChannelBuffer *_pRpcChannelBuffer,
        PRPC_MESSAGE _pRpcMessage,
        DWORD *_pdwStubPhase);

#else // VC6 Hack

EXTERN_C const IID IID_IServiceProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6d5140c1-7436-11ce-8034-00aa006009fa")
    IServiceProvider : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE QueryService( 
            /* [in] */ REFGUID guidService,
            /* [in] */ REFIID riid,
            /* [out] */ void **ppvObject) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IServiceProviderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IServiceProvider * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IServiceProvider * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IServiceProvider * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *QueryService )( 
            IServiceProvider * This,
            /* [in] */ REFGUID guidService,
            /* [in] */ REFIID riid,
            /* [out] */ void **ppvObject);
        
        END_INTERFACE
    } IServiceProviderVtbl;

    interface IServiceProvider
    {
        CONST_VTBL struct IServiceProviderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IServiceProvider_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IServiceProvider_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IServiceProvider_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IServiceProvider_QueryService(This,guidService,riid,ppvObject)	\
    (This)->lpVtbl -> QueryService(This,guidService,riid,ppvObject)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IServiceProvider_RemoteQueryService_Proxy( 
    IServiceProvider * This,
    /* [in] */ REFGUID guidService,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ IUnknown **ppvObject);


void __RPC_STUB IServiceProvider_RemoteQueryService_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IServiceProvider_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_servprov_0093 */
/* [local] */ 

#endif // VC6 Hack


extern RPC_IF_HANDLE __MIDL_itf_servprov_0093_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_servprov_0093_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* [local] */ HRESULT STDMETHODCALLTYPE IServiceProvider_QueryService_Proxy( 
    IServiceProvider * This,
    /* [in] */ REFGUID guidService,
    /* [in] */ REFIID riid,
    /* [out] */ void **ppvObject);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IServiceProvider_QueryService_Stub( 
    IServiceProvider * This,
    /* [in] */ REFGUID guidService,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ IUnknown **ppvObject);



/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



