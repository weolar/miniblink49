

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* Compiler settings for wbemprov.idl:
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

#ifndef __wbemprov_h__
#define __wbemprov_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IWbemPropertyProvider_FWD_DEFINED__
#define __IWbemPropertyProvider_FWD_DEFINED__
typedef interface IWbemPropertyProvider IWbemPropertyProvider;
#endif 	/* __IWbemPropertyProvider_FWD_DEFINED__ */


#ifndef __IWbemUnboundObjectSink_FWD_DEFINED__
#define __IWbemUnboundObjectSink_FWD_DEFINED__
typedef interface IWbemUnboundObjectSink IWbemUnboundObjectSink;
#endif 	/* __IWbemUnboundObjectSink_FWD_DEFINED__ */


#ifndef __IWbemEventProvider_FWD_DEFINED__
#define __IWbemEventProvider_FWD_DEFINED__
typedef interface IWbemEventProvider IWbemEventProvider;
#endif 	/* __IWbemEventProvider_FWD_DEFINED__ */


#ifndef __IWbemEventProviderQuerySink_FWD_DEFINED__
#define __IWbemEventProviderQuerySink_FWD_DEFINED__
typedef interface IWbemEventProviderQuerySink IWbemEventProviderQuerySink;
#endif 	/* __IWbemEventProviderQuerySink_FWD_DEFINED__ */


#ifndef __IWbemEventProviderSecurity_FWD_DEFINED__
#define __IWbemEventProviderSecurity_FWD_DEFINED__
typedef interface IWbemEventProviderSecurity IWbemEventProviderSecurity;
#endif 	/* __IWbemEventProviderSecurity_FWD_DEFINED__ */


#ifndef __IWbemEventConsumerProvider_FWD_DEFINED__
#define __IWbemEventConsumerProvider_FWD_DEFINED__
typedef interface IWbemEventConsumerProvider IWbemEventConsumerProvider;
#endif 	/* __IWbemEventConsumerProvider_FWD_DEFINED__ */


#ifndef __IWbemProviderInitSink_FWD_DEFINED__
#define __IWbemProviderInitSink_FWD_DEFINED__
typedef interface IWbemProviderInitSink IWbemProviderInitSink;
#endif 	/* __IWbemProviderInitSink_FWD_DEFINED__ */


#ifndef __IWbemProviderInit_FWD_DEFINED__
#define __IWbemProviderInit_FWD_DEFINED__
typedef interface IWbemProviderInit IWbemProviderInit;
#endif 	/* __IWbemProviderInit_FWD_DEFINED__ */


#ifndef __IWbemHiPerfProvider_FWD_DEFINED__
#define __IWbemHiPerfProvider_FWD_DEFINED__
typedef interface IWbemHiPerfProvider IWbemHiPerfProvider;
#endif 	/* __IWbemHiPerfProvider_FWD_DEFINED__ */


#ifndef __IWbemDecoupledRegistrar_FWD_DEFINED__
#define __IWbemDecoupledRegistrar_FWD_DEFINED__
typedef interface IWbemDecoupledRegistrar IWbemDecoupledRegistrar;
#endif 	/* __IWbemDecoupledRegistrar_FWD_DEFINED__ */


#ifndef __WbemAdministrativeLocator_FWD_DEFINED__
#define __WbemAdministrativeLocator_FWD_DEFINED__

#ifdef __cplusplus
typedef class WbemAdministrativeLocator WbemAdministrativeLocator;
#else
typedef struct WbemAdministrativeLocator WbemAdministrativeLocator;
#endif /* __cplusplus */

#endif 	/* __WbemAdministrativeLocator_FWD_DEFINED__ */


#ifndef __WbemAuthenticatedLocator_FWD_DEFINED__
#define __WbemAuthenticatedLocator_FWD_DEFINED__

#ifdef __cplusplus
typedef class WbemAuthenticatedLocator WbemAuthenticatedLocator;
#else
typedef struct WbemAuthenticatedLocator WbemAuthenticatedLocator;
#endif /* __cplusplus */

#endif 	/* __WbemAuthenticatedLocator_FWD_DEFINED__ */


#ifndef __WbemUnauthenticatedLocator_FWD_DEFINED__
#define __WbemUnauthenticatedLocator_FWD_DEFINED__

#ifdef __cplusplus
typedef class WbemUnauthenticatedLocator WbemUnauthenticatedLocator;
#else
typedef struct WbemUnauthenticatedLocator WbemUnauthenticatedLocator;
#endif /* __cplusplus */

#endif 	/* __WbemUnauthenticatedLocator_FWD_DEFINED__ */


#ifndef __WbemDecoupledRegistrar_FWD_DEFINED__
#define __WbemDecoupledRegistrar_FWD_DEFINED__

#ifdef __cplusplus
typedef class WbemDecoupledRegistrar WbemDecoupledRegistrar;
#else
typedef struct WbemDecoupledRegistrar WbemDecoupledRegistrar;
#endif /* __cplusplus */

#endif 	/* __WbemDecoupledRegistrar_FWD_DEFINED__ */


#ifndef __WbemDecoupledBasicEventProvider_FWD_DEFINED__
#define __WbemDecoupledBasicEventProvider_FWD_DEFINED__

#ifdef __cplusplus
typedef class WbemDecoupledBasicEventProvider WbemDecoupledBasicEventProvider;
#else
typedef struct WbemDecoupledBasicEventProvider WbemDecoupledBasicEventProvider;
#endif /* __cplusplus */

#endif 	/* __WbemDecoupledBasicEventProvider_FWD_DEFINED__ */


#ifndef __IWbemUnboundObjectSink_FWD_DEFINED__
#define __IWbemUnboundObjectSink_FWD_DEFINED__
typedef interface IWbemUnboundObjectSink IWbemUnboundObjectSink;
#endif 	/* __IWbemUnboundObjectSink_FWD_DEFINED__ */


#ifndef __IWbemPropertyProvider_FWD_DEFINED__
#define __IWbemPropertyProvider_FWD_DEFINED__
typedef interface IWbemPropertyProvider IWbemPropertyProvider;
#endif 	/* __IWbemPropertyProvider_FWD_DEFINED__ */


#ifndef __IWbemEventProvider_FWD_DEFINED__
#define __IWbemEventProvider_FWD_DEFINED__
typedef interface IWbemEventProvider IWbemEventProvider;
#endif 	/* __IWbemEventProvider_FWD_DEFINED__ */


#ifndef __IWbemEventProviderQuerySink_FWD_DEFINED__
#define __IWbemEventProviderQuerySink_FWD_DEFINED__
typedef interface IWbemEventProviderQuerySink IWbemEventProviderQuerySink;
#endif 	/* __IWbemEventProviderQuerySink_FWD_DEFINED__ */


#ifndef __IWbemEventProviderSecurity_FWD_DEFINED__
#define __IWbemEventProviderSecurity_FWD_DEFINED__
typedef interface IWbemEventProviderSecurity IWbemEventProviderSecurity;
#endif 	/* __IWbemEventProviderSecurity_FWD_DEFINED__ */


#ifndef __IWbemProviderIdentity_FWD_DEFINED__
#define __IWbemProviderIdentity_FWD_DEFINED__
typedef interface IWbemProviderIdentity IWbemProviderIdentity;
#endif 	/* __IWbemProviderIdentity_FWD_DEFINED__ */


#ifndef __IWbemEventConsumerProvider_FWD_DEFINED__
#define __IWbemEventConsumerProvider_FWD_DEFINED__
typedef interface IWbemEventConsumerProvider IWbemEventConsumerProvider;
#endif 	/* __IWbemEventConsumerProvider_FWD_DEFINED__ */


#ifndef __IWbemProviderInitSink_FWD_DEFINED__
#define __IWbemProviderInitSink_FWD_DEFINED__
typedef interface IWbemProviderInitSink IWbemProviderInitSink;
#endif 	/* __IWbemProviderInitSink_FWD_DEFINED__ */


#ifndef __IWbemProviderInit_FWD_DEFINED__
#define __IWbemProviderInit_FWD_DEFINED__
typedef interface IWbemProviderInit IWbemProviderInit;
#endif 	/* __IWbemProviderInit_FWD_DEFINED__ */


#ifndef __IWbemHiPerfProvider_FWD_DEFINED__
#define __IWbemHiPerfProvider_FWD_DEFINED__
typedef interface IWbemHiPerfProvider IWbemHiPerfProvider;
#endif 	/* __IWbemHiPerfProvider_FWD_DEFINED__ */


#ifndef __IWbemDecoupledRegistrar_FWD_DEFINED__
#define __IWbemDecoupledRegistrar_FWD_DEFINED__
typedef interface IWbemDecoupledRegistrar IWbemDecoupledRegistrar;
#endif 	/* __IWbemDecoupledRegistrar_FWD_DEFINED__ */


#ifndef __IWbemDecoupledBasicEventProvider_FWD_DEFINED__
#define __IWbemDecoupledBasicEventProvider_FWD_DEFINED__
typedef interface IWbemDecoupledBasicEventProvider IWbemDecoupledBasicEventProvider;
#endif 	/* __IWbemDecoupledBasicEventProvider_FWD_DEFINED__ */


#ifndef __IWbemEventSink_FWD_DEFINED__
#define __IWbemEventSink_FWD_DEFINED__
typedef interface IWbemEventSink IWbemEventSink;
#endif 	/* __IWbemEventSink_FWD_DEFINED__ */


/* header files for imported files */
#include "objidl.h"
#include "oleidl.h"
#include "oaidl.h"
#include "wbemcli.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_wbemprov_0000 */
/* [local] */ 

/*******************************************************************************/
/*                                                                             */
/*    Copyright © Microsoft Corporation.  All rights reserved.                 */
/*                                                                             */
/*    This IDL file defines the interfaces that WBEM providers need in         */
/*    addition to the client interfaces defined in WBEMCLI.IDL                 */
/*                                                                             */
/*******************************************************************************/
typedef VARIANT WBEM_VARIANT;

typedef /* [string] */ WCHAR *WBEM_WSTR;

typedef /* [string] */ const WCHAR *WBEM_CWSTR;

typedef /* [v1_enum] */ 
enum tag_WBEM_PROVIDER_REQUIREMENTS_TYPE
    {	WBEM_REQUIREMENTS_START_POSTFILTER	= 0,
	WBEM_REQUIREMENTS_STOP_POSTFILTER	= 1,
	WBEM_REQUIREMENTS_RECHECK_SUBSCRIPTIONS	= 2
    } 	WBEM_PROVIDER_REQUIREMENTS_TYPE;



extern RPC_IF_HANDLE __MIDL_itf_wbemprov_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wbemprov_0000_v0_0_s_ifspec;


#ifndef __WbemProviders_v1_LIBRARY_DEFINED__
#define __WbemProviders_v1_LIBRARY_DEFINED__

/* library WbemProviders_v1 */
/* [uuid] */ 












EXTERN_C const IID LIBID_WbemProviders_v1;

#ifndef __IWbemPropertyProvider_INTERFACE_DEFINED__
#define __IWbemPropertyProvider_INTERFACE_DEFINED__

/* interface IWbemPropertyProvider */
/* [uuid][object][restricted] */ 


EXTERN_C const IID IID_IWbemPropertyProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("CE61E841-65BC-11d0-B6BD-00AA003240C7")
    IWbemPropertyProvider : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetProperty( 
            /* [in] */ long lFlags,
            /* [in] */ const BSTR strLocale,
            /* [in] */ const BSTR strClassMapping,
            /* [in] */ const BSTR strInstMapping,
            /* [in] */ const BSTR strPropMapping,
            /* [out] */ VARIANT *pvValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PutProperty( 
            /* [in] */ long lFlags,
            /* [in] */ const BSTR strLocale,
            /* [in] */ const BSTR strClassMapping,
            /* [in] */ const BSTR strInstMapping,
            /* [in] */ const BSTR strPropMapping,
            /* [in] */ const VARIANT *pvValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWbemPropertyProviderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWbemPropertyProvider * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWbemPropertyProvider * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWbemPropertyProvider * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetProperty )( 
            IWbemPropertyProvider * This,
            /* [in] */ long lFlags,
            /* [in] */ const BSTR strLocale,
            /* [in] */ const BSTR strClassMapping,
            /* [in] */ const BSTR strInstMapping,
            /* [in] */ const BSTR strPropMapping,
            /* [out] */ VARIANT *pvValue);
        
        HRESULT ( STDMETHODCALLTYPE *PutProperty )( 
            IWbemPropertyProvider * This,
            /* [in] */ long lFlags,
            /* [in] */ const BSTR strLocale,
            /* [in] */ const BSTR strClassMapping,
            /* [in] */ const BSTR strInstMapping,
            /* [in] */ const BSTR strPropMapping,
            /* [in] */ const VARIANT *pvValue);
        
        END_INTERFACE
    } IWbemPropertyProviderVtbl;

    interface IWbemPropertyProvider
    {
        CONST_VTBL struct IWbemPropertyProviderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWbemPropertyProvider_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWbemPropertyProvider_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWbemPropertyProvider_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWbemPropertyProvider_GetProperty(This,lFlags,strLocale,strClassMapping,strInstMapping,strPropMapping,pvValue)	\
    (This)->lpVtbl -> GetProperty(This,lFlags,strLocale,strClassMapping,strInstMapping,strPropMapping,pvValue)

#define IWbemPropertyProvider_PutProperty(This,lFlags,strLocale,strClassMapping,strInstMapping,strPropMapping,pvValue)	\
    (This)->lpVtbl -> PutProperty(This,lFlags,strLocale,strClassMapping,strInstMapping,strPropMapping,pvValue)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWbemPropertyProvider_GetProperty_Proxy( 
    IWbemPropertyProvider * This,
    /* [in] */ long lFlags,
    /* [in] */ const BSTR strLocale,
    /* [in] */ const BSTR strClassMapping,
    /* [in] */ const BSTR strInstMapping,
    /* [in] */ const BSTR strPropMapping,
    /* [out] */ VARIANT *pvValue);


void __RPC_STUB IWbemPropertyProvider_GetProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWbemPropertyProvider_PutProperty_Proxy( 
    IWbemPropertyProvider * This,
    /* [in] */ long lFlags,
    /* [in] */ const BSTR strLocale,
    /* [in] */ const BSTR strClassMapping,
    /* [in] */ const BSTR strInstMapping,
    /* [in] */ const BSTR strPropMapping,
    /* [in] */ const VARIANT *pvValue);


void __RPC_STUB IWbemPropertyProvider_PutProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWbemPropertyProvider_INTERFACE_DEFINED__ */


#ifndef __IWbemUnboundObjectSink_INTERFACE_DEFINED__
#define __IWbemUnboundObjectSink_INTERFACE_DEFINED__

/* interface IWbemUnboundObjectSink */
/* [uuid][object][restricted] */ 


EXTERN_C const IID IID_IWbemUnboundObjectSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("e246107b-b06e-11d0-ad61-00c04fd8fdff")
    IWbemUnboundObjectSink : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE IndicateToConsumer( 
            /* [in] */ IWbemClassObject *pLogicalConsumer,
            /* [in] */ long lNumObjects,
            /* [size_is][in] */ IWbemClassObject **apObjects) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWbemUnboundObjectSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWbemUnboundObjectSink * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWbemUnboundObjectSink * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWbemUnboundObjectSink * This);
        
        HRESULT ( STDMETHODCALLTYPE *IndicateToConsumer )( 
            IWbemUnboundObjectSink * This,
            /* [in] */ IWbemClassObject *pLogicalConsumer,
            /* [in] */ long lNumObjects,
            /* [size_is][in] */ IWbemClassObject **apObjects);
        
        END_INTERFACE
    } IWbemUnboundObjectSinkVtbl;

    interface IWbemUnboundObjectSink
    {
        CONST_VTBL struct IWbemUnboundObjectSinkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWbemUnboundObjectSink_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWbemUnboundObjectSink_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWbemUnboundObjectSink_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWbemUnboundObjectSink_IndicateToConsumer(This,pLogicalConsumer,lNumObjects,apObjects)	\
    (This)->lpVtbl -> IndicateToConsumer(This,pLogicalConsumer,lNumObjects,apObjects)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWbemUnboundObjectSink_IndicateToConsumer_Proxy( 
    IWbemUnboundObjectSink * This,
    /* [in] */ IWbemClassObject *pLogicalConsumer,
    /* [in] */ long lNumObjects,
    /* [size_is][in] */ IWbemClassObject **apObjects);


void __RPC_STUB IWbemUnboundObjectSink_IndicateToConsumer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWbemUnboundObjectSink_INTERFACE_DEFINED__ */


#ifndef __IWbemEventProvider_INTERFACE_DEFINED__
#define __IWbemEventProvider_INTERFACE_DEFINED__

/* interface IWbemEventProvider */
/* [uuid][object][restricted] */ 


EXTERN_C const IID IID_IWbemEventProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("e245105b-b06e-11d0-ad61-00c04fd8fdff")
    IWbemEventProvider : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ProvideEvents( 
            /* [in] */ IWbemObjectSink *pSink,
            /* [in] */ long lFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWbemEventProviderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWbemEventProvider * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWbemEventProvider * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWbemEventProvider * This);
        
        HRESULT ( STDMETHODCALLTYPE *ProvideEvents )( 
            IWbemEventProvider * This,
            /* [in] */ IWbemObjectSink *pSink,
            /* [in] */ long lFlags);
        
        END_INTERFACE
    } IWbemEventProviderVtbl;

    interface IWbemEventProvider
    {
        CONST_VTBL struct IWbemEventProviderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWbemEventProvider_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWbemEventProvider_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWbemEventProvider_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWbemEventProvider_ProvideEvents(This,pSink,lFlags)	\
    (This)->lpVtbl -> ProvideEvents(This,pSink,lFlags)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWbemEventProvider_ProvideEvents_Proxy( 
    IWbemEventProvider * This,
    /* [in] */ IWbemObjectSink *pSink,
    /* [in] */ long lFlags);


void __RPC_STUB IWbemEventProvider_ProvideEvents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWbemEventProvider_INTERFACE_DEFINED__ */


#ifndef __IWbemEventProviderQuerySink_INTERFACE_DEFINED__
#define __IWbemEventProviderQuerySink_INTERFACE_DEFINED__

/* interface IWbemEventProviderQuerySink */
/* [uuid][object][restricted] */ 


EXTERN_C const IID IID_IWbemEventProviderQuerySink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("580acaf8-fa1c-11d0-ad72-00c04fd8fdff")
    IWbemEventProviderQuerySink : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE NewQuery( 
            /* [in] */ unsigned long dwId,
            /* [in] */ WBEM_WSTR wszQueryLanguage,
            /* [in] */ WBEM_WSTR wszQuery) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CancelQuery( 
            /* [in] */ unsigned long dwId) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWbemEventProviderQuerySinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWbemEventProviderQuerySink * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWbemEventProviderQuerySink * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWbemEventProviderQuerySink * This);
        
        HRESULT ( STDMETHODCALLTYPE *NewQuery )( 
            IWbemEventProviderQuerySink * This,
            /* [in] */ unsigned long dwId,
            /* [in] */ WBEM_WSTR wszQueryLanguage,
            /* [in] */ WBEM_WSTR wszQuery);
        
        HRESULT ( STDMETHODCALLTYPE *CancelQuery )( 
            IWbemEventProviderQuerySink * This,
            /* [in] */ unsigned long dwId);
        
        END_INTERFACE
    } IWbemEventProviderQuerySinkVtbl;

    interface IWbemEventProviderQuerySink
    {
        CONST_VTBL struct IWbemEventProviderQuerySinkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWbemEventProviderQuerySink_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWbemEventProviderQuerySink_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWbemEventProviderQuerySink_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWbemEventProviderQuerySink_NewQuery(This,dwId,wszQueryLanguage,wszQuery)	\
    (This)->lpVtbl -> NewQuery(This,dwId,wszQueryLanguage,wszQuery)

#define IWbemEventProviderQuerySink_CancelQuery(This,dwId)	\
    (This)->lpVtbl -> CancelQuery(This,dwId)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWbemEventProviderQuerySink_NewQuery_Proxy( 
    IWbemEventProviderQuerySink * This,
    /* [in] */ unsigned long dwId,
    /* [in] */ WBEM_WSTR wszQueryLanguage,
    /* [in] */ WBEM_WSTR wszQuery);


void __RPC_STUB IWbemEventProviderQuerySink_NewQuery_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWbemEventProviderQuerySink_CancelQuery_Proxy( 
    IWbemEventProviderQuerySink * This,
    /* [in] */ unsigned long dwId);


void __RPC_STUB IWbemEventProviderQuerySink_CancelQuery_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWbemEventProviderQuerySink_INTERFACE_DEFINED__ */


#ifndef __IWbemEventProviderSecurity_INTERFACE_DEFINED__
#define __IWbemEventProviderSecurity_INTERFACE_DEFINED__

/* interface IWbemEventProviderSecurity */
/* [uuid][object][restricted] */ 


EXTERN_C const IID IID_IWbemEventProviderSecurity;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("631f7d96-d993-11d2-b339-00105a1f4aaf")
    IWbemEventProviderSecurity : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AccessCheck( 
            /* [in] */ WBEM_CWSTR wszQueryLanguage,
            /* [in] */ WBEM_CWSTR wszQuery,
            /* [in] */ long lSidLength,
            /* [unique][size_is][in] */ const BYTE *pSid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWbemEventProviderSecurityVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWbemEventProviderSecurity * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWbemEventProviderSecurity * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWbemEventProviderSecurity * This);
        
        HRESULT ( STDMETHODCALLTYPE *AccessCheck )( 
            IWbemEventProviderSecurity * This,
            /* [in] */ WBEM_CWSTR wszQueryLanguage,
            /* [in] */ WBEM_CWSTR wszQuery,
            /* [in] */ long lSidLength,
            /* [unique][size_is][in] */ const BYTE *pSid);
        
        END_INTERFACE
    } IWbemEventProviderSecurityVtbl;

    interface IWbemEventProviderSecurity
    {
        CONST_VTBL struct IWbemEventProviderSecurityVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWbemEventProviderSecurity_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWbemEventProviderSecurity_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWbemEventProviderSecurity_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWbemEventProviderSecurity_AccessCheck(This,wszQueryLanguage,wszQuery,lSidLength,pSid)	\
    (This)->lpVtbl -> AccessCheck(This,wszQueryLanguage,wszQuery,lSidLength,pSid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWbemEventProviderSecurity_AccessCheck_Proxy( 
    IWbemEventProviderSecurity * This,
    /* [in] */ WBEM_CWSTR wszQueryLanguage,
    /* [in] */ WBEM_CWSTR wszQuery,
    /* [in] */ long lSidLength,
    /* [unique][size_is][in] */ const BYTE *pSid);


void __RPC_STUB IWbemEventProviderSecurity_AccessCheck_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWbemEventProviderSecurity_INTERFACE_DEFINED__ */


#ifndef __IWbemEventConsumerProvider_INTERFACE_DEFINED__
#define __IWbemEventConsumerProvider_INTERFACE_DEFINED__

/* interface IWbemEventConsumerProvider */
/* [uuid][object][restricted] */ 


EXTERN_C const IID IID_IWbemEventConsumerProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("e246107a-b06e-11d0-ad61-00c04fd8fdff")
    IWbemEventConsumerProvider : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE FindConsumer( 
            /* [in] */ IWbemClassObject *pLogicalConsumer,
            /* [out] */ IWbemUnboundObjectSink **ppConsumer) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWbemEventConsumerProviderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWbemEventConsumerProvider * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWbemEventConsumerProvider * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWbemEventConsumerProvider * This);
        
        HRESULT ( STDMETHODCALLTYPE *FindConsumer )( 
            IWbemEventConsumerProvider * This,
            /* [in] */ IWbemClassObject *pLogicalConsumer,
            /* [out] */ IWbemUnboundObjectSink **ppConsumer);
        
        END_INTERFACE
    } IWbemEventConsumerProviderVtbl;

    interface IWbemEventConsumerProvider
    {
        CONST_VTBL struct IWbemEventConsumerProviderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWbemEventConsumerProvider_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWbemEventConsumerProvider_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWbemEventConsumerProvider_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWbemEventConsumerProvider_FindConsumer(This,pLogicalConsumer,ppConsumer)	\
    (This)->lpVtbl -> FindConsumer(This,pLogicalConsumer,ppConsumer)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWbemEventConsumerProvider_FindConsumer_Proxy( 
    IWbemEventConsumerProvider * This,
    /* [in] */ IWbemClassObject *pLogicalConsumer,
    /* [out] */ IWbemUnboundObjectSink **ppConsumer);


void __RPC_STUB IWbemEventConsumerProvider_FindConsumer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWbemEventConsumerProvider_INTERFACE_DEFINED__ */


#ifndef __IWbemProviderInitSink_INTERFACE_DEFINED__
#define __IWbemProviderInitSink_INTERFACE_DEFINED__

/* interface IWbemProviderInitSink */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWbemProviderInitSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1be41571-91dd-11d1-aeb2-00c04fb68820")
    IWbemProviderInitSink : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetStatus( 
            /* [in] */ LONG lStatus,
            /* [in] */ LONG lFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWbemProviderInitSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWbemProviderInitSink * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWbemProviderInitSink * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWbemProviderInitSink * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetStatus )( 
            IWbemProviderInitSink * This,
            /* [in] */ LONG lStatus,
            /* [in] */ LONG lFlags);
        
        END_INTERFACE
    } IWbemProviderInitSinkVtbl;

    interface IWbemProviderInitSink
    {
        CONST_VTBL struct IWbemProviderInitSinkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWbemProviderInitSink_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWbemProviderInitSink_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWbemProviderInitSink_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWbemProviderInitSink_SetStatus(This,lStatus,lFlags)	\
    (This)->lpVtbl -> SetStatus(This,lStatus,lFlags)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWbemProviderInitSink_SetStatus_Proxy( 
    IWbemProviderInitSink * This,
    /* [in] */ LONG lStatus,
    /* [in] */ LONG lFlags);


void __RPC_STUB IWbemProviderInitSink_SetStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWbemProviderInitSink_INTERFACE_DEFINED__ */


#ifndef __IWbemProviderInit_INTERFACE_DEFINED__
#define __IWbemProviderInit_INTERFACE_DEFINED__

/* interface IWbemProviderInit */
/* [uuid][object] */ 


EXTERN_C const IID IID_IWbemProviderInit;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1be41572-91dd-11d1-aeb2-00c04fb68820")
    IWbemProviderInit : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [string][unique][in] */ LPWSTR wszUser,
            /* [in] */ LONG lFlags,
            /* [string][in] */ LPWSTR wszNamespace,
            /* [string][unique][in] */ LPWSTR wszLocale,
            /* [in] */ IWbemServices *pNamespace,
            /* [in] */ IWbemContext *pCtx,
            /* [in] */ IWbemProviderInitSink *pInitSink) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWbemProviderInitVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWbemProviderInit * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWbemProviderInit * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWbemProviderInit * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IWbemProviderInit * This,
            /* [string][unique][in] */ LPWSTR wszUser,
            /* [in] */ LONG lFlags,
            /* [string][in] */ LPWSTR wszNamespace,
            /* [string][unique][in] */ LPWSTR wszLocale,
            /* [in] */ IWbemServices *pNamespace,
            /* [in] */ IWbemContext *pCtx,
            /* [in] */ IWbemProviderInitSink *pInitSink);
        
        END_INTERFACE
    } IWbemProviderInitVtbl;

    interface IWbemProviderInit
    {
        CONST_VTBL struct IWbemProviderInitVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWbemProviderInit_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWbemProviderInit_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWbemProviderInit_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWbemProviderInit_Initialize(This,wszUser,lFlags,wszNamespace,wszLocale,pNamespace,pCtx,pInitSink)	\
    (This)->lpVtbl -> Initialize(This,wszUser,lFlags,wszNamespace,wszLocale,pNamespace,pCtx,pInitSink)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWbemProviderInit_Initialize_Proxy( 
    IWbemProviderInit * This,
    /* [string][unique][in] */ LPWSTR wszUser,
    /* [in] */ LONG lFlags,
    /* [string][in] */ LPWSTR wszNamespace,
    /* [string][unique][in] */ LPWSTR wszLocale,
    /* [in] */ IWbemServices *pNamespace,
    /* [in] */ IWbemContext *pCtx,
    /* [in] */ IWbemProviderInitSink *pInitSink);


void __RPC_STUB IWbemProviderInit_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWbemProviderInit_INTERFACE_DEFINED__ */


#ifndef __IWbemHiPerfProvider_INTERFACE_DEFINED__
#define __IWbemHiPerfProvider_INTERFACE_DEFINED__

/* interface IWbemHiPerfProvider */
/* [uuid][object][restricted][local] */ 


EXTERN_C const IID IID_IWbemHiPerfProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("49353c93-516b-11d1-aea6-00c04fb68820")
    IWbemHiPerfProvider : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE QueryInstances( 
            /* [in] */ IWbemServices *pNamespace,
            /* [string][in] */ WCHAR *wszClass,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext *pCtx,
            /* [in] */ IWbemObjectSink *pSink) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateRefresher( 
            /* [in] */ IWbemServices *pNamespace,
            /* [in] */ long lFlags,
            /* [out] */ IWbemRefresher **ppRefresher) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateRefreshableObject( 
            /* [in] */ IWbemServices *pNamespace,
            /* [in] */ IWbemObjectAccess *pTemplate,
            /* [in] */ IWbemRefresher *pRefresher,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext *pContext,
            /* [out] */ IWbemObjectAccess **ppRefreshable,
            /* [out] */ long *plId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StopRefreshing( 
            /* [in] */ IWbemRefresher *pRefresher,
            /* [in] */ long lId,
            /* [in] */ long lFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateRefreshableEnum( 
            /* [in] */ IWbemServices *pNamespace,
            /* [string][in] */ LPCWSTR wszClass,
            /* [in] */ IWbemRefresher *pRefresher,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext *pContext,
            /* [in] */ IWbemHiPerfEnum *pHiPerfEnum,
            /* [out] */ long *plId) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetObjects( 
            /* [in] */ IWbemServices *pNamespace,
            /* [in] */ long lNumObjects,
            /* [size_is][out][in] */ IWbemObjectAccess **apObj,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext *pContext) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWbemHiPerfProviderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWbemHiPerfProvider * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWbemHiPerfProvider * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWbemHiPerfProvider * This);
        
        HRESULT ( STDMETHODCALLTYPE *QueryInstances )( 
            IWbemHiPerfProvider * This,
            /* [in] */ IWbemServices *pNamespace,
            /* [string][in] */ WCHAR *wszClass,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext *pCtx,
            /* [in] */ IWbemObjectSink *pSink);
        
        HRESULT ( STDMETHODCALLTYPE *CreateRefresher )( 
            IWbemHiPerfProvider * This,
            /* [in] */ IWbemServices *pNamespace,
            /* [in] */ long lFlags,
            /* [out] */ IWbemRefresher **ppRefresher);
        
        HRESULT ( STDMETHODCALLTYPE *CreateRefreshableObject )( 
            IWbemHiPerfProvider * This,
            /* [in] */ IWbemServices *pNamespace,
            /* [in] */ IWbemObjectAccess *pTemplate,
            /* [in] */ IWbemRefresher *pRefresher,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext *pContext,
            /* [out] */ IWbemObjectAccess **ppRefreshable,
            /* [out] */ long *plId);
        
        HRESULT ( STDMETHODCALLTYPE *StopRefreshing )( 
            IWbemHiPerfProvider * This,
            /* [in] */ IWbemRefresher *pRefresher,
            /* [in] */ long lId,
            /* [in] */ long lFlags);
        
        HRESULT ( STDMETHODCALLTYPE *CreateRefreshableEnum )( 
            IWbemHiPerfProvider * This,
            /* [in] */ IWbemServices *pNamespace,
            /* [string][in] */ LPCWSTR wszClass,
            /* [in] */ IWbemRefresher *pRefresher,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext *pContext,
            /* [in] */ IWbemHiPerfEnum *pHiPerfEnum,
            /* [out] */ long *plId);
        
        HRESULT ( STDMETHODCALLTYPE *GetObjects )( 
            IWbemHiPerfProvider * This,
            /* [in] */ IWbemServices *pNamespace,
            /* [in] */ long lNumObjects,
            /* [size_is][out][in] */ IWbemObjectAccess **apObj,
            /* [in] */ long lFlags,
            /* [in] */ IWbemContext *pContext);
        
        END_INTERFACE
    } IWbemHiPerfProviderVtbl;

    interface IWbemHiPerfProvider
    {
        CONST_VTBL struct IWbemHiPerfProviderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWbemHiPerfProvider_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWbemHiPerfProvider_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWbemHiPerfProvider_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWbemHiPerfProvider_QueryInstances(This,pNamespace,wszClass,lFlags,pCtx,pSink)	\
    (This)->lpVtbl -> QueryInstances(This,pNamespace,wszClass,lFlags,pCtx,pSink)

#define IWbemHiPerfProvider_CreateRefresher(This,pNamespace,lFlags,ppRefresher)	\
    (This)->lpVtbl -> CreateRefresher(This,pNamespace,lFlags,ppRefresher)

#define IWbemHiPerfProvider_CreateRefreshableObject(This,pNamespace,pTemplate,pRefresher,lFlags,pContext,ppRefreshable,plId)	\
    (This)->lpVtbl -> CreateRefreshableObject(This,pNamespace,pTemplate,pRefresher,lFlags,pContext,ppRefreshable,plId)

#define IWbemHiPerfProvider_StopRefreshing(This,pRefresher,lId,lFlags)	\
    (This)->lpVtbl -> StopRefreshing(This,pRefresher,lId,lFlags)

#define IWbemHiPerfProvider_CreateRefreshableEnum(This,pNamespace,wszClass,pRefresher,lFlags,pContext,pHiPerfEnum,plId)	\
    (This)->lpVtbl -> CreateRefreshableEnum(This,pNamespace,wszClass,pRefresher,lFlags,pContext,pHiPerfEnum,plId)

#define IWbemHiPerfProvider_GetObjects(This,pNamespace,lNumObjects,apObj,lFlags,pContext)	\
    (This)->lpVtbl -> GetObjects(This,pNamespace,lNumObjects,apObj,lFlags,pContext)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWbemHiPerfProvider_QueryInstances_Proxy( 
    IWbemHiPerfProvider * This,
    /* [in] */ IWbemServices *pNamespace,
    /* [string][in] */ WCHAR *wszClass,
    /* [in] */ long lFlags,
    /* [in] */ IWbemContext *pCtx,
    /* [in] */ IWbemObjectSink *pSink);


void __RPC_STUB IWbemHiPerfProvider_QueryInstances_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWbemHiPerfProvider_CreateRefresher_Proxy( 
    IWbemHiPerfProvider * This,
    /* [in] */ IWbemServices *pNamespace,
    /* [in] */ long lFlags,
    /* [out] */ IWbemRefresher **ppRefresher);


void __RPC_STUB IWbemHiPerfProvider_CreateRefresher_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWbemHiPerfProvider_CreateRefreshableObject_Proxy( 
    IWbemHiPerfProvider * This,
    /* [in] */ IWbemServices *pNamespace,
    /* [in] */ IWbemObjectAccess *pTemplate,
    /* [in] */ IWbemRefresher *pRefresher,
    /* [in] */ long lFlags,
    /* [in] */ IWbemContext *pContext,
    /* [out] */ IWbemObjectAccess **ppRefreshable,
    /* [out] */ long *plId);


void __RPC_STUB IWbemHiPerfProvider_CreateRefreshableObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWbemHiPerfProvider_StopRefreshing_Proxy( 
    IWbemHiPerfProvider * This,
    /* [in] */ IWbemRefresher *pRefresher,
    /* [in] */ long lId,
    /* [in] */ long lFlags);


void __RPC_STUB IWbemHiPerfProvider_StopRefreshing_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWbemHiPerfProvider_CreateRefreshableEnum_Proxy( 
    IWbemHiPerfProvider * This,
    /* [in] */ IWbemServices *pNamespace,
    /* [string][in] */ LPCWSTR wszClass,
    /* [in] */ IWbemRefresher *pRefresher,
    /* [in] */ long lFlags,
    /* [in] */ IWbemContext *pContext,
    /* [in] */ IWbemHiPerfEnum *pHiPerfEnum,
    /* [out] */ long *plId);


void __RPC_STUB IWbemHiPerfProvider_CreateRefreshableEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWbemHiPerfProvider_GetObjects_Proxy( 
    IWbemHiPerfProvider * This,
    /* [in] */ IWbemServices *pNamespace,
    /* [in] */ long lNumObjects,
    /* [size_is][out][in] */ IWbemObjectAccess **apObj,
    /* [in] */ long lFlags,
    /* [in] */ IWbemContext *pContext);


void __RPC_STUB IWbemHiPerfProvider_GetObjects_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWbemHiPerfProvider_INTERFACE_DEFINED__ */


#ifndef __IWbemDecoupledRegistrar_INTERFACE_DEFINED__
#define __IWbemDecoupledRegistrar_INTERFACE_DEFINED__

/* interface IWbemDecoupledRegistrar */
/* [uuid][local][object] */ 


EXTERN_C const IID IID_IWbemDecoupledRegistrar;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1005cbcf-e64f-4646-bcd3-3a089d8a84b4")
    IWbemDecoupledRegistrar : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Register( 
            /* [in] */ long a_Flags,
            /* [in] */ IWbemContext *a_Context,
            /* [in] */ LPCWSTR a_User,
            /* [in] */ LPCWSTR a_Locale,
            /* [in] */ LPCWSTR a_Scope,
            /* [in] */ LPCWSTR a_Registration,
            /* [in] */ IUnknown *a_Unknown) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnRegister( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWbemDecoupledRegistrarVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWbemDecoupledRegistrar * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWbemDecoupledRegistrar * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWbemDecoupledRegistrar * This);
        
        HRESULT ( STDMETHODCALLTYPE *Register )( 
            IWbemDecoupledRegistrar * This,
            /* [in] */ long a_Flags,
            /* [in] */ IWbemContext *a_Context,
            /* [in] */ LPCWSTR a_User,
            /* [in] */ LPCWSTR a_Locale,
            /* [in] */ LPCWSTR a_Scope,
            /* [in] */ LPCWSTR a_Registration,
            /* [in] */ IUnknown *a_Unknown);
        
        HRESULT ( STDMETHODCALLTYPE *UnRegister )( 
            IWbemDecoupledRegistrar * This);
        
        END_INTERFACE
    } IWbemDecoupledRegistrarVtbl;

    interface IWbemDecoupledRegistrar
    {
        CONST_VTBL struct IWbemDecoupledRegistrarVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWbemDecoupledRegistrar_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWbemDecoupledRegistrar_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWbemDecoupledRegistrar_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWbemDecoupledRegistrar_Register(This,a_Flags,a_Context,a_User,a_Locale,a_Scope,a_Registration,a_Unknown)	\
    (This)->lpVtbl -> Register(This,a_Flags,a_Context,a_User,a_Locale,a_Scope,a_Registration,a_Unknown)

#define IWbemDecoupledRegistrar_UnRegister(This)	\
    (This)->lpVtbl -> UnRegister(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWbemDecoupledRegistrar_Register_Proxy( 
    IWbemDecoupledRegistrar * This,
    /* [in] */ long a_Flags,
    /* [in] */ IWbemContext *a_Context,
    /* [in] */ LPCWSTR a_User,
    /* [in] */ LPCWSTR a_Locale,
    /* [in] */ LPCWSTR a_Scope,
    /* [in] */ LPCWSTR a_Registration,
    /* [in] */ IUnknown *a_Unknown);


void __RPC_STUB IWbemDecoupledRegistrar_Register_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWbemDecoupledRegistrar_UnRegister_Proxy( 
    IWbemDecoupledRegistrar * This);


void __RPC_STUB IWbemDecoupledRegistrar_UnRegister_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWbemDecoupledRegistrar_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_WbemAdministrativeLocator;

#ifdef __cplusplus

class DECLSPEC_UUID("cb8555cc-9128-11d1-ad9b-00c04fd8fdff")
WbemAdministrativeLocator;
#endif

EXTERN_C const CLSID CLSID_WbemAuthenticatedLocator;

#ifdef __cplusplus

class DECLSPEC_UUID("cd184336-9128-11d1-ad9b-00c04fd8fdff")
WbemAuthenticatedLocator;
#endif

EXTERN_C const CLSID CLSID_WbemUnauthenticatedLocator;

#ifdef __cplusplus

class DECLSPEC_UUID("443E7B79-DE31-11d2-B340-00104BCC4B4A")
WbemUnauthenticatedLocator;
#endif

EXTERN_C const CLSID CLSID_WbemDecoupledRegistrar;

#ifdef __cplusplus

class DECLSPEC_UUID("4cfc7932-0f9d-4bef-9c32-8ea2a6b56fcb")
WbemDecoupledRegistrar;
#endif

EXTERN_C const CLSID CLSID_WbemDecoupledBasicEventProvider;

#ifdef __cplusplus

class DECLSPEC_UUID("f5f75737-2843-4f22-933d-c76a97cda62f")
WbemDecoupledBasicEventProvider;
#endif
#endif /* __WbemProviders_v1_LIBRARY_DEFINED__ */

#ifndef __IWbemProviderIdentity_INTERFACE_DEFINED__
#define __IWbemProviderIdentity_INTERFACE_DEFINED__

/* interface IWbemProviderIdentity */
/* [uuid][object][restricted] */ 


EXTERN_C const IID IID_IWbemProviderIdentity;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("631f7d97-d993-11d2-b339-00105a1f4aaf")
    IWbemProviderIdentity : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetRegistrationObject( 
            /* [in] */ long lFlags,
            /* [in] */ IWbemClassObject *pProvReg) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWbemProviderIdentityVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWbemProviderIdentity * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWbemProviderIdentity * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWbemProviderIdentity * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetRegistrationObject )( 
            IWbemProviderIdentity * This,
            /* [in] */ long lFlags,
            /* [in] */ IWbemClassObject *pProvReg);
        
        END_INTERFACE
    } IWbemProviderIdentityVtbl;

    interface IWbemProviderIdentity
    {
        CONST_VTBL struct IWbemProviderIdentityVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWbemProviderIdentity_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWbemProviderIdentity_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWbemProviderIdentity_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWbemProviderIdentity_SetRegistrationObject(This,lFlags,pProvReg)	\
    (This)->lpVtbl -> SetRegistrationObject(This,lFlags,pProvReg)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWbemProviderIdentity_SetRegistrationObject_Proxy( 
    IWbemProviderIdentity * This,
    /* [in] */ long lFlags,
    /* [in] */ IWbemClassObject *pProvReg);


void __RPC_STUB IWbemProviderIdentity_SetRegistrationObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWbemProviderIdentity_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_wbemprov_0169 */
/* [local] */ 

typedef 
enum tag_WBEM_EXTRA_RETURN_CODES
    {	WBEM_S_INITIALIZED	= 0,
	WBEM_S_LIMITED_SERVICE	= 0x43001,
	WBEM_S_INDIRECTLY_UPDATED	= WBEM_S_LIMITED_SERVICE + 1,
	WBEM_S_SUBJECT_TO_SDS	= WBEM_S_INDIRECTLY_UPDATED + 1,
	WBEM_E_RETRY_LATER	= 0x80043001,
	WBEM_E_RESOURCE_CONTENTION	= WBEM_E_RETRY_LATER + 1
    } 	WBEM_EXTRA_RETURN_CODES;

typedef 
enum tag_WBEM_PROVIDER_FLAGS
    {	WBEM_FLAG_OWNER_UPDATE	= 0x10000
    } 	WBEM_PROVIDER_FLAGS;



extern RPC_IF_HANDLE __MIDL_itf_wbemprov_0169_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wbemprov_0169_v0_0_s_ifspec;

#ifndef __IWbemDecoupledBasicEventProvider_INTERFACE_DEFINED__
#define __IWbemDecoupledBasicEventProvider_INTERFACE_DEFINED__

/* interface IWbemDecoupledBasicEventProvider */
/* [uuid][local][object] */ 


EXTERN_C const IID IID_IWbemDecoupledBasicEventProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("86336d20-ca11-4786-9ef1-bc8a946b42fc")
    IWbemDecoupledBasicEventProvider : public IWbemDecoupledRegistrar
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetSink( 
            /* [in] */ long a_Flags,
            /* [in] */ IWbemContext *a_Context,
            /* [out] */ IWbemObjectSink **a_Sink) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetService( 
            /* [in] */ long a_Flags,
            /* [in] */ IWbemContext *a_Context,
            /* [out] */ IWbemServices **a_Service) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWbemDecoupledBasicEventProviderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWbemDecoupledBasicEventProvider * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWbemDecoupledBasicEventProvider * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWbemDecoupledBasicEventProvider * This);
        
        HRESULT ( STDMETHODCALLTYPE *Register )( 
            IWbemDecoupledBasicEventProvider * This,
            /* [in] */ long a_Flags,
            /* [in] */ IWbemContext *a_Context,
            /* [in] */ LPCWSTR a_User,
            /* [in] */ LPCWSTR a_Locale,
            /* [in] */ LPCWSTR a_Scope,
            /* [in] */ LPCWSTR a_Registration,
            /* [in] */ IUnknown *a_Unknown);
        
        HRESULT ( STDMETHODCALLTYPE *UnRegister )( 
            IWbemDecoupledBasicEventProvider * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSink )( 
            IWbemDecoupledBasicEventProvider * This,
            /* [in] */ long a_Flags,
            /* [in] */ IWbemContext *a_Context,
            /* [out] */ IWbemObjectSink **a_Sink);
        
        HRESULT ( STDMETHODCALLTYPE *GetService )( 
            IWbemDecoupledBasicEventProvider * This,
            /* [in] */ long a_Flags,
            /* [in] */ IWbemContext *a_Context,
            /* [out] */ IWbemServices **a_Service);
        
        END_INTERFACE
    } IWbemDecoupledBasicEventProviderVtbl;

    interface IWbemDecoupledBasicEventProvider
    {
        CONST_VTBL struct IWbemDecoupledBasicEventProviderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWbemDecoupledBasicEventProvider_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWbemDecoupledBasicEventProvider_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWbemDecoupledBasicEventProvider_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWbemDecoupledBasicEventProvider_Register(This,a_Flags,a_Context,a_User,a_Locale,a_Scope,a_Registration,a_Unknown)	\
    (This)->lpVtbl -> Register(This,a_Flags,a_Context,a_User,a_Locale,a_Scope,a_Registration,a_Unknown)

#define IWbemDecoupledBasicEventProvider_UnRegister(This)	\
    (This)->lpVtbl -> UnRegister(This)


#define IWbemDecoupledBasicEventProvider_GetSink(This,a_Flags,a_Context,a_Sink)	\
    (This)->lpVtbl -> GetSink(This,a_Flags,a_Context,a_Sink)

#define IWbemDecoupledBasicEventProvider_GetService(This,a_Flags,a_Context,a_Service)	\
    (This)->lpVtbl -> GetService(This,a_Flags,a_Context,a_Service)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWbemDecoupledBasicEventProvider_GetSink_Proxy( 
    IWbemDecoupledBasicEventProvider * This,
    /* [in] */ long a_Flags,
    /* [in] */ IWbemContext *a_Context,
    /* [out] */ IWbemObjectSink **a_Sink);


void __RPC_STUB IWbemDecoupledBasicEventProvider_GetSink_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWbemDecoupledBasicEventProvider_GetService_Proxy( 
    IWbemDecoupledBasicEventProvider * This,
    /* [in] */ long a_Flags,
    /* [in] */ IWbemContext *a_Context,
    /* [out] */ IWbemServices **a_Service);


void __RPC_STUB IWbemDecoupledBasicEventProvider_GetService_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWbemDecoupledBasicEventProvider_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_wbemprov_0174 */
/* [local] */ 

typedef /* [v1_enum] */ 
enum tag_WBEM_BATCH_TYPE
    {	WBEM_FLAG_BATCH_IF_NEEDED	= 0,
	WBEM_FLAG_MUST_BATCH	= 0x1,
	WBEM_FLAG_MUST_NOT_BATCH	= 0x2
    } 	WBEM_BATCH_TYPE;



extern RPC_IF_HANDLE __MIDL_itf_wbemprov_0174_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wbemprov_0174_v0_0_s_ifspec;

#ifndef __IWbemEventSink_INTERFACE_DEFINED__
#define __IWbemEventSink_INTERFACE_DEFINED__

/* interface IWbemEventSink */
/* [uuid][restricted][object] */ 


EXTERN_C const IID IID_IWbemEventSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3ae0080a-7e3a-4366-bf89-0feedc931659")
    IWbemEventSink : public IWbemObjectSink
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetSinkSecurity( 
            /* [in] */ long lSDLength,
            /* [size_is][in] */ BYTE *pSD) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsActive( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRestrictedSink( 
            /* [in] */ long lNumQueries,
            /* [string][size_is][in] */ const LPCWSTR *awszQueries,
            /* [in] */ IUnknown *pCallback,
            /* [out] */ IWbemEventSink **ppSink) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetBatchingParameters( 
            /* [in] */ LONG lFlags,
            /* [in] */ DWORD dwMaxBufferSize,
            /* [in] */ DWORD dwMaxSendLatency) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWbemEventSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWbemEventSink * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWbemEventSink * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWbemEventSink * This);
        
        HRESULT ( STDMETHODCALLTYPE *Indicate )( 
            IWbemEventSink * This,
            /* [in] */ long lObjectCount,
            /* [size_is][in] */ IWbemClassObject **apObjArray);
        
        HRESULT ( STDMETHODCALLTYPE *SetStatus )( 
            IWbemEventSink * This,
            /* [in] */ long lFlags,
            /* [in] */ HRESULT hResult,
            /* [in] */ BSTR strParam,
            /* [in] */ IWbemClassObject *pObjParam);
        
        HRESULT ( STDMETHODCALLTYPE *SetSinkSecurity )( 
            IWbemEventSink * This,
            /* [in] */ long lSDLength,
            /* [size_is][in] */ BYTE *pSD);
        
        HRESULT ( STDMETHODCALLTYPE *IsActive )( 
            IWbemEventSink * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetRestrictedSink )( 
            IWbemEventSink * This,
            /* [in] */ long lNumQueries,
            /* [string][size_is][in] */ const LPCWSTR *awszQueries,
            /* [in] */ IUnknown *pCallback,
            /* [out] */ IWbemEventSink **ppSink);
        
        HRESULT ( STDMETHODCALLTYPE *SetBatchingParameters )( 
            IWbemEventSink * This,
            /* [in] */ LONG lFlags,
            /* [in] */ DWORD dwMaxBufferSize,
            /* [in] */ DWORD dwMaxSendLatency);
        
        END_INTERFACE
    } IWbemEventSinkVtbl;

    interface IWbemEventSink
    {
        CONST_VTBL struct IWbemEventSinkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWbemEventSink_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWbemEventSink_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWbemEventSink_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWbemEventSink_Indicate(This,lObjectCount,apObjArray)	\
    (This)->lpVtbl -> Indicate(This,lObjectCount,apObjArray)

#define IWbemEventSink_SetStatus(This,lFlags,hResult,strParam,pObjParam)	\
    (This)->lpVtbl -> SetStatus(This,lFlags,hResult,strParam,pObjParam)


#define IWbemEventSink_SetSinkSecurity(This,lSDLength,pSD)	\
    (This)->lpVtbl -> SetSinkSecurity(This,lSDLength,pSD)

#define IWbemEventSink_IsActive(This)	\
    (This)->lpVtbl -> IsActive(This)

#define IWbemEventSink_GetRestrictedSink(This,lNumQueries,awszQueries,pCallback,ppSink)	\
    (This)->lpVtbl -> GetRestrictedSink(This,lNumQueries,awszQueries,pCallback,ppSink)

#define IWbemEventSink_SetBatchingParameters(This,lFlags,dwMaxBufferSize,dwMaxSendLatency)	\
    (This)->lpVtbl -> SetBatchingParameters(This,lFlags,dwMaxBufferSize,dwMaxSendLatency)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWbemEventSink_SetSinkSecurity_Proxy( 
    IWbemEventSink * This,
    /* [in] */ long lSDLength,
    /* [size_is][in] */ BYTE *pSD);


void __RPC_STUB IWbemEventSink_SetSinkSecurity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWbemEventSink_IsActive_Proxy( 
    IWbemEventSink * This);


void __RPC_STUB IWbemEventSink_IsActive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWbemEventSink_GetRestrictedSink_Proxy( 
    IWbemEventSink * This,
    /* [in] */ long lNumQueries,
    /* [string][size_is][in] */ const LPCWSTR *awszQueries,
    /* [in] */ IUnknown *pCallback,
    /* [out] */ IWbemEventSink **ppSink);


void __RPC_STUB IWbemEventSink_GetRestrictedSink_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWbemEventSink_SetBatchingParameters_Proxy( 
    IWbemEventSink * This,
    /* [in] */ LONG lFlags,
    /* [in] */ DWORD dwMaxBufferSize,
    /* [in] */ DWORD dwMaxSendLatency);


void __RPC_STUB IWbemEventSink_SetBatchingParameters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWbemEventSink_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



