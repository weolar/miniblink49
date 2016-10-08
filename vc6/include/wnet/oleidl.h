

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* Compiler settings for oleidl.idl:
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

#ifndef __oleidl_h__
#define __oleidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IOleAdviseHolder_FWD_DEFINED__
#define __IOleAdviseHolder_FWD_DEFINED__
typedef interface IOleAdviseHolder IOleAdviseHolder;
#endif 	/* __IOleAdviseHolder_FWD_DEFINED__ */


#ifndef __IOleCache_FWD_DEFINED__
#define __IOleCache_FWD_DEFINED__
typedef interface IOleCache IOleCache;
#endif 	/* __IOleCache_FWD_DEFINED__ */


#ifndef __IOleCache2_FWD_DEFINED__
#define __IOleCache2_FWD_DEFINED__
typedef interface IOleCache2 IOleCache2;
#endif 	/* __IOleCache2_FWD_DEFINED__ */


#ifndef __IOleCacheControl_FWD_DEFINED__
#define __IOleCacheControl_FWD_DEFINED__
typedef interface IOleCacheControl IOleCacheControl;
#endif 	/* __IOleCacheControl_FWD_DEFINED__ */


#ifndef __IParseDisplayName_FWD_DEFINED__
#define __IParseDisplayName_FWD_DEFINED__
typedef interface IParseDisplayName IParseDisplayName;
#endif 	/* __IParseDisplayName_FWD_DEFINED__ */


#ifndef __IOleContainer_FWD_DEFINED__
#define __IOleContainer_FWD_DEFINED__
typedef interface IOleContainer IOleContainer;
#endif 	/* __IOleContainer_FWD_DEFINED__ */


#ifndef __IOleClientSite_FWD_DEFINED__
#define __IOleClientSite_FWD_DEFINED__
typedef interface IOleClientSite IOleClientSite;
#endif 	/* __IOleClientSite_FWD_DEFINED__ */


#ifndef __IOleObject_FWD_DEFINED__
#define __IOleObject_FWD_DEFINED__
typedef interface IOleObject IOleObject;
#endif 	/* __IOleObject_FWD_DEFINED__ */


#ifndef __IOleWindow_FWD_DEFINED__
#define __IOleWindow_FWD_DEFINED__
typedef interface IOleWindow IOleWindow;
#endif 	/* __IOleWindow_FWD_DEFINED__ */


#ifndef __IOleLink_FWD_DEFINED__
#define __IOleLink_FWD_DEFINED__
typedef interface IOleLink IOleLink;
#endif 	/* __IOleLink_FWD_DEFINED__ */


#ifndef __IOleItemContainer_FWD_DEFINED__
#define __IOleItemContainer_FWD_DEFINED__
typedef interface IOleItemContainer IOleItemContainer;
#endif 	/* __IOleItemContainer_FWD_DEFINED__ */


#ifndef __IOleInPlaceUIWindow_FWD_DEFINED__
#define __IOleInPlaceUIWindow_FWD_DEFINED__
typedef interface IOleInPlaceUIWindow IOleInPlaceUIWindow;
#endif 	/* __IOleInPlaceUIWindow_FWD_DEFINED__ */


#ifndef __IOleInPlaceActiveObject_FWD_DEFINED__
#define __IOleInPlaceActiveObject_FWD_DEFINED__
typedef interface IOleInPlaceActiveObject IOleInPlaceActiveObject;
#endif 	/* __IOleInPlaceActiveObject_FWD_DEFINED__ */


#ifndef __IOleInPlaceFrame_FWD_DEFINED__
#define __IOleInPlaceFrame_FWD_DEFINED__
typedef interface IOleInPlaceFrame IOleInPlaceFrame;
#endif 	/* __IOleInPlaceFrame_FWD_DEFINED__ */


#ifndef __IOleInPlaceObject_FWD_DEFINED__
#define __IOleInPlaceObject_FWD_DEFINED__
typedef interface IOleInPlaceObject IOleInPlaceObject;
#endif 	/* __IOleInPlaceObject_FWD_DEFINED__ */


#ifndef __IOleInPlaceSite_FWD_DEFINED__
#define __IOleInPlaceSite_FWD_DEFINED__
typedef interface IOleInPlaceSite IOleInPlaceSite;
#endif 	/* __IOleInPlaceSite_FWD_DEFINED__ */


#ifndef __IContinue_FWD_DEFINED__
#define __IContinue_FWD_DEFINED__
typedef interface IContinue IContinue;
#endif 	/* __IContinue_FWD_DEFINED__ */


#ifndef __IViewObject_FWD_DEFINED__
#define __IViewObject_FWD_DEFINED__
typedef interface IViewObject IViewObject;
#endif 	/* __IViewObject_FWD_DEFINED__ */


#ifndef __IViewObject2_FWD_DEFINED__
#define __IViewObject2_FWD_DEFINED__
typedef interface IViewObject2 IViewObject2;
#endif 	/* __IViewObject2_FWD_DEFINED__ */


#ifndef __IDropSource_FWD_DEFINED__
#define __IDropSource_FWD_DEFINED__
typedef interface IDropSource IDropSource;
#endif 	/* __IDropSource_FWD_DEFINED__ */


#ifndef __IDropTarget_FWD_DEFINED__
#define __IDropTarget_FWD_DEFINED__
typedef interface IDropTarget IDropTarget;
#endif 	/* __IDropTarget_FWD_DEFINED__ */


#ifndef __IEnumOLEVERB_FWD_DEFINED__
#define __IEnumOLEVERB_FWD_DEFINED__
typedef interface IEnumOLEVERB IEnumOLEVERB;
#endif 	/* __IEnumOLEVERB_FWD_DEFINED__ */


/* header files for imported files */
#include "objidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_oleidl_0000 */
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




extern RPC_IF_HANDLE __MIDL_itf_oleidl_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_oleidl_0000_v0_0_s_ifspec;

#ifndef __IOleAdviseHolder_INTERFACE_DEFINED__
#define __IOleAdviseHolder_INTERFACE_DEFINED__

/* interface IOleAdviseHolder */
/* [uuid][object][local] */ 

typedef /* [unique] */ IOleAdviseHolder *LPOLEADVISEHOLDER;


EXTERN_C const IID IID_IOleAdviseHolder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000111-0000-0000-C000-000000000046")
    IOleAdviseHolder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Advise( 
            /* [unique][in] */ IAdviseSink *pAdvise,
            /* [out] */ DWORD *pdwConnection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Unadvise( 
            /* [in] */ DWORD dwConnection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumAdvise( 
            /* [out] */ IEnumSTATDATA **ppenumAdvise) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SendOnRename( 
            /* [unique][in] */ IMoniker *pmk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SendOnSave( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SendOnClose( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOleAdviseHolderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleAdviseHolder * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleAdviseHolder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleAdviseHolder * This);
        
        HRESULT ( STDMETHODCALLTYPE *Advise )( 
            IOleAdviseHolder * This,
            /* [unique][in] */ IAdviseSink *pAdvise,
            /* [out] */ DWORD *pdwConnection);
        
        HRESULT ( STDMETHODCALLTYPE *Unadvise )( 
            IOleAdviseHolder * This,
            /* [in] */ DWORD dwConnection);
        
        HRESULT ( STDMETHODCALLTYPE *EnumAdvise )( 
            IOleAdviseHolder * This,
            /* [out] */ IEnumSTATDATA **ppenumAdvise);
        
        HRESULT ( STDMETHODCALLTYPE *SendOnRename )( 
            IOleAdviseHolder * This,
            /* [unique][in] */ IMoniker *pmk);
        
        HRESULT ( STDMETHODCALLTYPE *SendOnSave )( 
            IOleAdviseHolder * This);
        
        HRESULT ( STDMETHODCALLTYPE *SendOnClose )( 
            IOleAdviseHolder * This);
        
        END_INTERFACE
    } IOleAdviseHolderVtbl;

    interface IOleAdviseHolder
    {
        CONST_VTBL struct IOleAdviseHolderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleAdviseHolder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOleAdviseHolder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOleAdviseHolder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOleAdviseHolder_Advise(This,pAdvise,pdwConnection)	\
    (This)->lpVtbl -> Advise(This,pAdvise,pdwConnection)

#define IOleAdviseHolder_Unadvise(This,dwConnection)	\
    (This)->lpVtbl -> Unadvise(This,dwConnection)

#define IOleAdviseHolder_EnumAdvise(This,ppenumAdvise)	\
    (This)->lpVtbl -> EnumAdvise(This,ppenumAdvise)

#define IOleAdviseHolder_SendOnRename(This,pmk)	\
    (This)->lpVtbl -> SendOnRename(This,pmk)

#define IOleAdviseHolder_SendOnSave(This)	\
    (This)->lpVtbl -> SendOnSave(This)

#define IOleAdviseHolder_SendOnClose(This)	\
    (This)->lpVtbl -> SendOnClose(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOleAdviseHolder_Advise_Proxy( 
    IOleAdviseHolder * This,
    /* [unique][in] */ IAdviseSink *pAdvise,
    /* [out] */ DWORD *pdwConnection);


void __RPC_STUB IOleAdviseHolder_Advise_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleAdviseHolder_Unadvise_Proxy( 
    IOleAdviseHolder * This,
    /* [in] */ DWORD dwConnection);


void __RPC_STUB IOleAdviseHolder_Unadvise_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleAdviseHolder_EnumAdvise_Proxy( 
    IOleAdviseHolder * This,
    /* [out] */ IEnumSTATDATA **ppenumAdvise);


void __RPC_STUB IOleAdviseHolder_EnumAdvise_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleAdviseHolder_SendOnRename_Proxy( 
    IOleAdviseHolder * This,
    /* [unique][in] */ IMoniker *pmk);


void __RPC_STUB IOleAdviseHolder_SendOnRename_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleAdviseHolder_SendOnSave_Proxy( 
    IOleAdviseHolder * This);


void __RPC_STUB IOleAdviseHolder_SendOnSave_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleAdviseHolder_SendOnClose_Proxy( 
    IOleAdviseHolder * This);


void __RPC_STUB IOleAdviseHolder_SendOnClose_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOleAdviseHolder_INTERFACE_DEFINED__ */


#ifndef __IOleCache_INTERFACE_DEFINED__
#define __IOleCache_INTERFACE_DEFINED__

/* interface IOleCache */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IOleCache *LPOLECACHE;


EXTERN_C const IID IID_IOleCache;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000011e-0000-0000-C000-000000000046")
    IOleCache : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Cache( 
            /* [unique][in] */ FORMATETC *pformatetc,
            /* [in] */ DWORD advf,
            /* [out] */ DWORD *pdwConnection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Uncache( 
            /* [in] */ DWORD dwConnection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumCache( 
            /* [out] */ IEnumSTATDATA **ppenumSTATDATA) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InitCache( 
            /* [unique][in] */ IDataObject *pDataObject) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetData( 
            /* [unique][in] */ FORMATETC *pformatetc,
            /* [unique][in] */ STGMEDIUM *pmedium,
            /* [in] */ BOOL fRelease) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOleCacheVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleCache * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleCache * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleCache * This);
        
        HRESULT ( STDMETHODCALLTYPE *Cache )( 
            IOleCache * This,
            /* [unique][in] */ FORMATETC *pformatetc,
            /* [in] */ DWORD advf,
            /* [out] */ DWORD *pdwConnection);
        
        HRESULT ( STDMETHODCALLTYPE *Uncache )( 
            IOleCache * This,
            /* [in] */ DWORD dwConnection);
        
        HRESULT ( STDMETHODCALLTYPE *EnumCache )( 
            IOleCache * This,
            /* [out] */ IEnumSTATDATA **ppenumSTATDATA);
        
        HRESULT ( STDMETHODCALLTYPE *InitCache )( 
            IOleCache * This,
            /* [unique][in] */ IDataObject *pDataObject);
        
        HRESULT ( STDMETHODCALLTYPE *SetData )( 
            IOleCache * This,
            /* [unique][in] */ FORMATETC *pformatetc,
            /* [unique][in] */ STGMEDIUM *pmedium,
            /* [in] */ BOOL fRelease);
        
        END_INTERFACE
    } IOleCacheVtbl;

    interface IOleCache
    {
        CONST_VTBL struct IOleCacheVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleCache_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOleCache_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOleCache_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOleCache_Cache(This,pformatetc,advf,pdwConnection)	\
    (This)->lpVtbl -> Cache(This,pformatetc,advf,pdwConnection)

#define IOleCache_Uncache(This,dwConnection)	\
    (This)->lpVtbl -> Uncache(This,dwConnection)

#define IOleCache_EnumCache(This,ppenumSTATDATA)	\
    (This)->lpVtbl -> EnumCache(This,ppenumSTATDATA)

#define IOleCache_InitCache(This,pDataObject)	\
    (This)->lpVtbl -> InitCache(This,pDataObject)

#define IOleCache_SetData(This,pformatetc,pmedium,fRelease)	\
    (This)->lpVtbl -> SetData(This,pformatetc,pmedium,fRelease)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOleCache_Cache_Proxy( 
    IOleCache * This,
    /* [unique][in] */ FORMATETC *pformatetc,
    /* [in] */ DWORD advf,
    /* [out] */ DWORD *pdwConnection);


void __RPC_STUB IOleCache_Cache_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleCache_Uncache_Proxy( 
    IOleCache * This,
    /* [in] */ DWORD dwConnection);


void __RPC_STUB IOleCache_Uncache_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleCache_EnumCache_Proxy( 
    IOleCache * This,
    /* [out] */ IEnumSTATDATA **ppenumSTATDATA);


void __RPC_STUB IOleCache_EnumCache_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleCache_InitCache_Proxy( 
    IOleCache * This,
    /* [unique][in] */ IDataObject *pDataObject);


void __RPC_STUB IOleCache_InitCache_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleCache_SetData_Proxy( 
    IOleCache * This,
    /* [unique][in] */ FORMATETC *pformatetc,
    /* [unique][in] */ STGMEDIUM *pmedium,
    /* [in] */ BOOL fRelease);


void __RPC_STUB IOleCache_SetData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOleCache_INTERFACE_DEFINED__ */


#ifndef __IOleCache2_INTERFACE_DEFINED__
#define __IOleCache2_INTERFACE_DEFINED__

/* interface IOleCache2 */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IOleCache2 *LPOLECACHE2;

#define	UPDFCACHE_NODATACACHE	( 0x1 )

#define	UPDFCACHE_ONSAVECACHE	( 0x2 )

#define	UPDFCACHE_ONSTOPCACHE	( 0x4 )

#define	UPDFCACHE_NORMALCACHE	( 0x8 )

#define	UPDFCACHE_IFBLANK	( 0x10 )

#define	UPDFCACHE_ONLYIFBLANK	( 0x80000000 )

#define	UPDFCACHE_IFBLANKORONSAVECACHE	( UPDFCACHE_IFBLANK | UPDFCACHE_ONSAVECACHE )

#define	UPDFCACHE_ALL	( ( DWORD  )~UPDFCACHE_ONLYIFBLANK )

#define	UPDFCACHE_ALLBUTNODATACACHE	( UPDFCACHE_ALL & ( DWORD  )~UPDFCACHE_NODATACACHE )

typedef /* [v1_enum] */ 
enum tagDISCARDCACHE
    {	DISCARDCACHE_SAVEIFDIRTY	= 0,
	DISCARDCACHE_NOSAVE	= 1
    } 	DISCARDCACHE;


EXTERN_C const IID IID_IOleCache2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000128-0000-0000-C000-000000000046")
    IOleCache2 : public IOleCache
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE UpdateCache( 
            /* [in] */ LPDATAOBJECT pDataObject,
            /* [in] */ DWORD grfUpdf,
            /* [in] */ LPVOID pReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DiscardCache( 
            /* [in] */ DWORD dwDiscardOptions) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOleCache2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleCache2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleCache2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleCache2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Cache )( 
            IOleCache2 * This,
            /* [unique][in] */ FORMATETC *pformatetc,
            /* [in] */ DWORD advf,
            /* [out] */ DWORD *pdwConnection);
        
        HRESULT ( STDMETHODCALLTYPE *Uncache )( 
            IOleCache2 * This,
            /* [in] */ DWORD dwConnection);
        
        HRESULT ( STDMETHODCALLTYPE *EnumCache )( 
            IOleCache2 * This,
            /* [out] */ IEnumSTATDATA **ppenumSTATDATA);
        
        HRESULT ( STDMETHODCALLTYPE *InitCache )( 
            IOleCache2 * This,
            /* [unique][in] */ IDataObject *pDataObject);
        
        HRESULT ( STDMETHODCALLTYPE *SetData )( 
            IOleCache2 * This,
            /* [unique][in] */ FORMATETC *pformatetc,
            /* [unique][in] */ STGMEDIUM *pmedium,
            /* [in] */ BOOL fRelease);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *UpdateCache )( 
            IOleCache2 * This,
            /* [in] */ LPDATAOBJECT pDataObject,
            /* [in] */ DWORD grfUpdf,
            /* [in] */ LPVOID pReserved);
        
        HRESULT ( STDMETHODCALLTYPE *DiscardCache )( 
            IOleCache2 * This,
            /* [in] */ DWORD dwDiscardOptions);
        
        END_INTERFACE
    } IOleCache2Vtbl;

    interface IOleCache2
    {
        CONST_VTBL struct IOleCache2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleCache2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOleCache2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOleCache2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOleCache2_Cache(This,pformatetc,advf,pdwConnection)	\
    (This)->lpVtbl -> Cache(This,pformatetc,advf,pdwConnection)

#define IOleCache2_Uncache(This,dwConnection)	\
    (This)->lpVtbl -> Uncache(This,dwConnection)

#define IOleCache2_EnumCache(This,ppenumSTATDATA)	\
    (This)->lpVtbl -> EnumCache(This,ppenumSTATDATA)

#define IOleCache2_InitCache(This,pDataObject)	\
    (This)->lpVtbl -> InitCache(This,pDataObject)

#define IOleCache2_SetData(This,pformatetc,pmedium,fRelease)	\
    (This)->lpVtbl -> SetData(This,pformatetc,pmedium,fRelease)


#define IOleCache2_UpdateCache(This,pDataObject,grfUpdf,pReserved)	\
    (This)->lpVtbl -> UpdateCache(This,pDataObject,grfUpdf,pReserved)

#define IOleCache2_DiscardCache(This,dwDiscardOptions)	\
    (This)->lpVtbl -> DiscardCache(This,dwDiscardOptions)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IOleCache2_RemoteUpdateCache_Proxy( 
    IOleCache2 * This,
    /* [in] */ LPDATAOBJECT pDataObject,
    /* [in] */ DWORD grfUpdf,
    /* [in] */ LONG_PTR pReserved);


void __RPC_STUB IOleCache2_RemoteUpdateCache_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleCache2_DiscardCache_Proxy( 
    IOleCache2 * This,
    /* [in] */ DWORD dwDiscardOptions);


void __RPC_STUB IOleCache2_DiscardCache_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOleCache2_INTERFACE_DEFINED__ */


#ifndef __IOleCacheControl_INTERFACE_DEFINED__
#define __IOleCacheControl_INTERFACE_DEFINED__

/* interface IOleCacheControl */
/* [uuid][object] */ 

typedef /* [unique] */ IOleCacheControl *LPOLECACHECONTROL;


EXTERN_C const IID IID_IOleCacheControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000129-0000-0000-C000-000000000046")
    IOleCacheControl : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnRun( 
            LPDATAOBJECT pDataObject) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnStop( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOleCacheControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleCacheControl * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleCacheControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleCacheControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnRun )( 
            IOleCacheControl * This,
            LPDATAOBJECT pDataObject);
        
        HRESULT ( STDMETHODCALLTYPE *OnStop )( 
            IOleCacheControl * This);
        
        END_INTERFACE
    } IOleCacheControlVtbl;

    interface IOleCacheControl
    {
        CONST_VTBL struct IOleCacheControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleCacheControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOleCacheControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOleCacheControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOleCacheControl_OnRun(This,pDataObject)	\
    (This)->lpVtbl -> OnRun(This,pDataObject)

#define IOleCacheControl_OnStop(This)	\
    (This)->lpVtbl -> OnStop(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOleCacheControl_OnRun_Proxy( 
    IOleCacheControl * This,
    LPDATAOBJECT pDataObject);


void __RPC_STUB IOleCacheControl_OnRun_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleCacheControl_OnStop_Proxy( 
    IOleCacheControl * This);


void __RPC_STUB IOleCacheControl_OnStop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOleCacheControl_INTERFACE_DEFINED__ */


#ifndef __IParseDisplayName_INTERFACE_DEFINED__
#define __IParseDisplayName_INTERFACE_DEFINED__

/* interface IParseDisplayName */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IParseDisplayName *LPPARSEDISPLAYNAME;


EXTERN_C const IID IID_IParseDisplayName;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000011a-0000-0000-C000-000000000046")
    IParseDisplayName : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ParseDisplayName( 
            /* [unique][in] */ IBindCtx *pbc,
            /* [in] */ LPOLESTR pszDisplayName,
            /* [out] */ ULONG *pchEaten,
            /* [out] */ IMoniker **ppmkOut) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IParseDisplayNameVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IParseDisplayName * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IParseDisplayName * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IParseDisplayName * This);
        
        HRESULT ( STDMETHODCALLTYPE *ParseDisplayName )( 
            IParseDisplayName * This,
            /* [unique][in] */ IBindCtx *pbc,
            /* [in] */ LPOLESTR pszDisplayName,
            /* [out] */ ULONG *pchEaten,
            /* [out] */ IMoniker **ppmkOut);
        
        END_INTERFACE
    } IParseDisplayNameVtbl;

    interface IParseDisplayName
    {
        CONST_VTBL struct IParseDisplayNameVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IParseDisplayName_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IParseDisplayName_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IParseDisplayName_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IParseDisplayName_ParseDisplayName(This,pbc,pszDisplayName,pchEaten,ppmkOut)	\
    (This)->lpVtbl -> ParseDisplayName(This,pbc,pszDisplayName,pchEaten,ppmkOut)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IParseDisplayName_ParseDisplayName_Proxy( 
    IParseDisplayName * This,
    /* [unique][in] */ IBindCtx *pbc,
    /* [in] */ LPOLESTR pszDisplayName,
    /* [out] */ ULONG *pchEaten,
    /* [out] */ IMoniker **ppmkOut);


void __RPC_STUB IParseDisplayName_ParseDisplayName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IParseDisplayName_INTERFACE_DEFINED__ */


#ifndef __IOleContainer_INTERFACE_DEFINED__
#define __IOleContainer_INTERFACE_DEFINED__

/* interface IOleContainer */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IOleContainer *LPOLECONTAINER;


EXTERN_C const IID IID_IOleContainer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000011b-0000-0000-C000-000000000046")
    IOleContainer : public IParseDisplayName
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EnumObjects( 
            /* [in] */ DWORD grfFlags,
            /* [out] */ IEnumUnknown **ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LockContainer( 
            /* [in] */ BOOL fLock) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOleContainerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleContainer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleContainer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleContainer * This);
        
        HRESULT ( STDMETHODCALLTYPE *ParseDisplayName )( 
            IOleContainer * This,
            /* [unique][in] */ IBindCtx *pbc,
            /* [in] */ LPOLESTR pszDisplayName,
            /* [out] */ ULONG *pchEaten,
            /* [out] */ IMoniker **ppmkOut);
        
        HRESULT ( STDMETHODCALLTYPE *EnumObjects )( 
            IOleContainer * This,
            /* [in] */ DWORD grfFlags,
            /* [out] */ IEnumUnknown **ppenum);
        
        HRESULT ( STDMETHODCALLTYPE *LockContainer )( 
            IOleContainer * This,
            /* [in] */ BOOL fLock);
        
        END_INTERFACE
    } IOleContainerVtbl;

    interface IOleContainer
    {
        CONST_VTBL struct IOleContainerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleContainer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOleContainer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOleContainer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOleContainer_ParseDisplayName(This,pbc,pszDisplayName,pchEaten,ppmkOut)	\
    (This)->lpVtbl -> ParseDisplayName(This,pbc,pszDisplayName,pchEaten,ppmkOut)


#define IOleContainer_EnumObjects(This,grfFlags,ppenum)	\
    (This)->lpVtbl -> EnumObjects(This,grfFlags,ppenum)

#define IOleContainer_LockContainer(This,fLock)	\
    (This)->lpVtbl -> LockContainer(This,fLock)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOleContainer_EnumObjects_Proxy( 
    IOleContainer * This,
    /* [in] */ DWORD grfFlags,
    /* [out] */ IEnumUnknown **ppenum);


void __RPC_STUB IOleContainer_EnumObjects_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleContainer_LockContainer_Proxy( 
    IOleContainer * This,
    /* [in] */ BOOL fLock);


void __RPC_STUB IOleContainer_LockContainer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOleContainer_INTERFACE_DEFINED__ */


#ifndef __IOleClientSite_INTERFACE_DEFINED__
#define __IOleClientSite_INTERFACE_DEFINED__

/* interface IOleClientSite */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IOleClientSite *LPOLECLIENTSITE;


EXTERN_C const IID IID_IOleClientSite;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000118-0000-0000-C000-000000000046")
    IOleClientSite : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SaveObject( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMoniker( 
            /* [in] */ DWORD dwAssign,
            /* [in] */ DWORD dwWhichMoniker,
            /* [out] */ IMoniker **ppmk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetContainer( 
            /* [out] */ IOleContainer **ppContainer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ShowObject( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnShowWindow( 
            /* [in] */ BOOL fShow) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RequestNewObjectLayout( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOleClientSiteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleClientSite * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleClientSite * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleClientSite * This);
        
        HRESULT ( STDMETHODCALLTYPE *SaveObject )( 
            IOleClientSite * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetMoniker )( 
            IOleClientSite * This,
            /* [in] */ DWORD dwAssign,
            /* [in] */ DWORD dwWhichMoniker,
            /* [out] */ IMoniker **ppmk);
        
        HRESULT ( STDMETHODCALLTYPE *GetContainer )( 
            IOleClientSite * This,
            /* [out] */ IOleContainer **ppContainer);
        
        HRESULT ( STDMETHODCALLTYPE *ShowObject )( 
            IOleClientSite * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnShowWindow )( 
            IOleClientSite * This,
            /* [in] */ BOOL fShow);
        
        HRESULT ( STDMETHODCALLTYPE *RequestNewObjectLayout )( 
            IOleClientSite * This);
        
        END_INTERFACE
    } IOleClientSiteVtbl;

    interface IOleClientSite
    {
        CONST_VTBL struct IOleClientSiteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleClientSite_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOleClientSite_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOleClientSite_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOleClientSite_SaveObject(This)	\
    (This)->lpVtbl -> SaveObject(This)

#define IOleClientSite_GetMoniker(This,dwAssign,dwWhichMoniker,ppmk)	\
    (This)->lpVtbl -> GetMoniker(This,dwAssign,dwWhichMoniker,ppmk)

#define IOleClientSite_GetContainer(This,ppContainer)	\
    (This)->lpVtbl -> GetContainer(This,ppContainer)

#define IOleClientSite_ShowObject(This)	\
    (This)->lpVtbl -> ShowObject(This)

#define IOleClientSite_OnShowWindow(This,fShow)	\
    (This)->lpVtbl -> OnShowWindow(This,fShow)

#define IOleClientSite_RequestNewObjectLayout(This)	\
    (This)->lpVtbl -> RequestNewObjectLayout(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOleClientSite_SaveObject_Proxy( 
    IOleClientSite * This);


void __RPC_STUB IOleClientSite_SaveObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleClientSite_GetMoniker_Proxy( 
    IOleClientSite * This,
    /* [in] */ DWORD dwAssign,
    /* [in] */ DWORD dwWhichMoniker,
    /* [out] */ IMoniker **ppmk);


void __RPC_STUB IOleClientSite_GetMoniker_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleClientSite_GetContainer_Proxy( 
    IOleClientSite * This,
    /* [out] */ IOleContainer **ppContainer);


void __RPC_STUB IOleClientSite_GetContainer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleClientSite_ShowObject_Proxy( 
    IOleClientSite * This);


void __RPC_STUB IOleClientSite_ShowObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleClientSite_OnShowWindow_Proxy( 
    IOleClientSite * This,
    /* [in] */ BOOL fShow);


void __RPC_STUB IOleClientSite_OnShowWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleClientSite_RequestNewObjectLayout_Proxy( 
    IOleClientSite * This);


void __RPC_STUB IOleClientSite_RequestNewObjectLayout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOleClientSite_INTERFACE_DEFINED__ */


#ifndef __IOleObject_INTERFACE_DEFINED__
#define __IOleObject_INTERFACE_DEFINED__

/* interface IOleObject */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IOleObject *LPOLEOBJECT;

typedef 
enum tagOLEGETMONIKER
    {	OLEGETMONIKER_ONLYIFTHERE	= 1,
	OLEGETMONIKER_FORCEASSIGN	= 2,
	OLEGETMONIKER_UNASSIGN	= 3,
	OLEGETMONIKER_TEMPFORUSER	= 4
    } 	OLEGETMONIKER;

typedef 
enum tagOLEWHICHMK
    {	OLEWHICHMK_CONTAINER	= 1,
	OLEWHICHMK_OBJREL	= 2,
	OLEWHICHMK_OBJFULL	= 3
    } 	OLEWHICHMK;

typedef 
enum tagUSERCLASSTYPE
    {	USERCLASSTYPE_FULL	= 1,
	USERCLASSTYPE_SHORT	= 2,
	USERCLASSTYPE_APPNAME	= 3
    } 	USERCLASSTYPE;

typedef 
enum tagOLEMISC
    {	OLEMISC_RECOMPOSEONRESIZE	= 0x1,
	OLEMISC_ONLYICONIC	= 0x2,
	OLEMISC_INSERTNOTREPLACE	= 0x4,
	OLEMISC_STATIC	= 0x8,
	OLEMISC_CANTLINKINSIDE	= 0x10,
	OLEMISC_CANLINKBYOLE1	= 0x20,
	OLEMISC_ISLINKOBJECT	= 0x40,
	OLEMISC_INSIDEOUT	= 0x80,
	OLEMISC_ACTIVATEWHENVISIBLE	= 0x100,
	OLEMISC_RENDERINGISDEVICEINDEPENDENT	= 0x200,
	OLEMISC_INVISIBLEATRUNTIME	= 0x400,
	OLEMISC_ALWAYSRUN	= 0x800,
	OLEMISC_ACTSLIKEBUTTON	= 0x1000,
	OLEMISC_ACTSLIKELABEL	= 0x2000,
	OLEMISC_NOUIACTIVATE	= 0x4000,
	OLEMISC_ALIGNABLE	= 0x8000,
	OLEMISC_SIMPLEFRAME	= 0x10000,
	OLEMISC_SETCLIENTSITEFIRST	= 0x20000,
	OLEMISC_IMEMODE	= 0x40000,
	OLEMISC_IGNOREACTIVATEWHENVISIBLE	= 0x80000,
	OLEMISC_WANTSTOMENUMERGE	= 0x100000,
	OLEMISC_SUPPORTSMULTILEVELUNDO	= 0x200000
    } 	OLEMISC;

typedef 
enum tagOLECLOSE
    {	OLECLOSE_SAVEIFDIRTY	= 0,
	OLECLOSE_NOSAVE	= 1,
	OLECLOSE_PROMPTSAVE	= 2
    } 	OLECLOSE;


EXTERN_C const IID IID_IOleObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000112-0000-0000-C000-000000000046")
    IOleObject : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetClientSite( 
            /* [unique][in] */ IOleClientSite *pClientSite) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetClientSite( 
            /* [out] */ IOleClientSite **ppClientSite) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHostNames( 
            /* [in] */ LPCOLESTR szContainerApp,
            /* [unique][in] */ LPCOLESTR szContainerObj) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Close( 
            /* [in] */ DWORD dwSaveOption) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMoniker( 
            /* [in] */ DWORD dwWhichMoniker,
            /* [unique][in] */ IMoniker *pmk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMoniker( 
            /* [in] */ DWORD dwAssign,
            /* [in] */ DWORD dwWhichMoniker,
            /* [out] */ IMoniker **ppmk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InitFromData( 
            /* [unique][in] */ IDataObject *pDataObject,
            /* [in] */ BOOL fCreation,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetClipboardData( 
            /* [in] */ DWORD dwReserved,
            /* [out] */ IDataObject **ppDataObject) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DoVerb( 
            /* [in] */ LONG iVerb,
            /* [unique][in] */ LPMSG lpmsg,
            /* [unique][in] */ IOleClientSite *pActiveSite,
            /* [in] */ LONG lindex,
            /* [in] */ HWND hwndParent,
            /* [unique][in] */ LPCRECT lprcPosRect) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumVerbs( 
            /* [out] */ IEnumOLEVERB **ppEnumOleVerb) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Update( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsUpToDate( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetUserClassID( 
            /* [out] */ CLSID *pClsid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetUserType( 
            /* [in] */ DWORD dwFormOfType,
            /* [out] */ LPOLESTR *pszUserType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetExtent( 
            /* [in] */ DWORD dwDrawAspect,
            /* [in] */ SIZEL *psizel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetExtent( 
            /* [in] */ DWORD dwDrawAspect,
            /* [out] */ SIZEL *psizel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Advise( 
            /* [unique][in] */ IAdviseSink *pAdvSink,
            /* [out] */ DWORD *pdwConnection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Unadvise( 
            /* [in] */ DWORD dwConnection) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumAdvise( 
            /* [out] */ IEnumSTATDATA **ppenumAdvise) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMiscStatus( 
            /* [in] */ DWORD dwAspect,
            /* [out] */ DWORD *pdwStatus) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetColorScheme( 
            /* [in] */ LOGPALETTE *pLogpal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOleObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleObject * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetClientSite )( 
            IOleObject * This,
            /* [unique][in] */ IOleClientSite *pClientSite);
        
        HRESULT ( STDMETHODCALLTYPE *GetClientSite )( 
            IOleObject * This,
            /* [out] */ IOleClientSite **ppClientSite);
        
        HRESULT ( STDMETHODCALLTYPE *SetHostNames )( 
            IOleObject * This,
            /* [in] */ LPCOLESTR szContainerApp,
            /* [unique][in] */ LPCOLESTR szContainerObj);
        
        HRESULT ( STDMETHODCALLTYPE *Close )( 
            IOleObject * This,
            /* [in] */ DWORD dwSaveOption);
        
        HRESULT ( STDMETHODCALLTYPE *SetMoniker )( 
            IOleObject * This,
            /* [in] */ DWORD dwWhichMoniker,
            /* [unique][in] */ IMoniker *pmk);
        
        HRESULT ( STDMETHODCALLTYPE *GetMoniker )( 
            IOleObject * This,
            /* [in] */ DWORD dwAssign,
            /* [in] */ DWORD dwWhichMoniker,
            /* [out] */ IMoniker **ppmk);
        
        HRESULT ( STDMETHODCALLTYPE *InitFromData )( 
            IOleObject * This,
            /* [unique][in] */ IDataObject *pDataObject,
            /* [in] */ BOOL fCreation,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *GetClipboardData )( 
            IOleObject * This,
            /* [in] */ DWORD dwReserved,
            /* [out] */ IDataObject **ppDataObject);
        
        HRESULT ( STDMETHODCALLTYPE *DoVerb )( 
            IOleObject * This,
            /* [in] */ LONG iVerb,
            /* [unique][in] */ LPMSG lpmsg,
            /* [unique][in] */ IOleClientSite *pActiveSite,
            /* [in] */ LONG lindex,
            /* [in] */ HWND hwndParent,
            /* [unique][in] */ LPCRECT lprcPosRect);
        
        HRESULT ( STDMETHODCALLTYPE *EnumVerbs )( 
            IOleObject * This,
            /* [out] */ IEnumOLEVERB **ppEnumOleVerb);
        
        HRESULT ( STDMETHODCALLTYPE *Update )( 
            IOleObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *IsUpToDate )( 
            IOleObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetUserClassID )( 
            IOleObject * This,
            /* [out] */ CLSID *pClsid);
        
        HRESULT ( STDMETHODCALLTYPE *GetUserType )( 
            IOleObject * This,
            /* [in] */ DWORD dwFormOfType,
            /* [out] */ LPOLESTR *pszUserType);
        
        HRESULT ( STDMETHODCALLTYPE *SetExtent )( 
            IOleObject * This,
            /* [in] */ DWORD dwDrawAspect,
            /* [in] */ SIZEL *psizel);
        
        HRESULT ( STDMETHODCALLTYPE *GetExtent )( 
            IOleObject * This,
            /* [in] */ DWORD dwDrawAspect,
            /* [out] */ SIZEL *psizel);
        
        HRESULT ( STDMETHODCALLTYPE *Advise )( 
            IOleObject * This,
            /* [unique][in] */ IAdviseSink *pAdvSink,
            /* [out] */ DWORD *pdwConnection);
        
        HRESULT ( STDMETHODCALLTYPE *Unadvise )( 
            IOleObject * This,
            /* [in] */ DWORD dwConnection);
        
        HRESULT ( STDMETHODCALLTYPE *EnumAdvise )( 
            IOleObject * This,
            /* [out] */ IEnumSTATDATA **ppenumAdvise);
        
        HRESULT ( STDMETHODCALLTYPE *GetMiscStatus )( 
            IOleObject * This,
            /* [in] */ DWORD dwAspect,
            /* [out] */ DWORD *pdwStatus);
        
        HRESULT ( STDMETHODCALLTYPE *SetColorScheme )( 
            IOleObject * This,
            /* [in] */ LOGPALETTE *pLogpal);
        
        END_INTERFACE
    } IOleObjectVtbl;

    interface IOleObject
    {
        CONST_VTBL struct IOleObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleObject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOleObject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOleObject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOleObject_SetClientSite(This,pClientSite)	\
    (This)->lpVtbl -> SetClientSite(This,pClientSite)

#define IOleObject_GetClientSite(This,ppClientSite)	\
    (This)->lpVtbl -> GetClientSite(This,ppClientSite)

#define IOleObject_SetHostNames(This,szContainerApp,szContainerObj)	\
    (This)->lpVtbl -> SetHostNames(This,szContainerApp,szContainerObj)

#define IOleObject_Close(This,dwSaveOption)	\
    (This)->lpVtbl -> Close(This,dwSaveOption)

#define IOleObject_SetMoniker(This,dwWhichMoniker,pmk)	\
    (This)->lpVtbl -> SetMoniker(This,dwWhichMoniker,pmk)

#define IOleObject_GetMoniker(This,dwAssign,dwWhichMoniker,ppmk)	\
    (This)->lpVtbl -> GetMoniker(This,dwAssign,dwWhichMoniker,ppmk)

#define IOleObject_InitFromData(This,pDataObject,fCreation,dwReserved)	\
    (This)->lpVtbl -> InitFromData(This,pDataObject,fCreation,dwReserved)

#define IOleObject_GetClipboardData(This,dwReserved,ppDataObject)	\
    (This)->lpVtbl -> GetClipboardData(This,dwReserved,ppDataObject)

#define IOleObject_DoVerb(This,iVerb,lpmsg,pActiveSite,lindex,hwndParent,lprcPosRect)	\
    (This)->lpVtbl -> DoVerb(This,iVerb,lpmsg,pActiveSite,lindex,hwndParent,lprcPosRect)

#define IOleObject_EnumVerbs(This,ppEnumOleVerb)	\
    (This)->lpVtbl -> EnumVerbs(This,ppEnumOleVerb)

#define IOleObject_Update(This)	\
    (This)->lpVtbl -> Update(This)

#define IOleObject_IsUpToDate(This)	\
    (This)->lpVtbl -> IsUpToDate(This)

#define IOleObject_GetUserClassID(This,pClsid)	\
    (This)->lpVtbl -> GetUserClassID(This,pClsid)

#define IOleObject_GetUserType(This,dwFormOfType,pszUserType)	\
    (This)->lpVtbl -> GetUserType(This,dwFormOfType,pszUserType)

#define IOleObject_SetExtent(This,dwDrawAspect,psizel)	\
    (This)->lpVtbl -> SetExtent(This,dwDrawAspect,psizel)

#define IOleObject_GetExtent(This,dwDrawAspect,psizel)	\
    (This)->lpVtbl -> GetExtent(This,dwDrawAspect,psizel)

#define IOleObject_Advise(This,pAdvSink,pdwConnection)	\
    (This)->lpVtbl -> Advise(This,pAdvSink,pdwConnection)

#define IOleObject_Unadvise(This,dwConnection)	\
    (This)->lpVtbl -> Unadvise(This,dwConnection)

#define IOleObject_EnumAdvise(This,ppenumAdvise)	\
    (This)->lpVtbl -> EnumAdvise(This,ppenumAdvise)

#define IOleObject_GetMiscStatus(This,dwAspect,pdwStatus)	\
    (This)->lpVtbl -> GetMiscStatus(This,dwAspect,pdwStatus)

#define IOleObject_SetColorScheme(This,pLogpal)	\
    (This)->lpVtbl -> SetColorScheme(This,pLogpal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOleObject_SetClientSite_Proxy( 
    IOleObject * This,
    /* [unique][in] */ IOleClientSite *pClientSite);


void __RPC_STUB IOleObject_SetClientSite_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_GetClientSite_Proxy( 
    IOleObject * This,
    /* [out] */ IOleClientSite **ppClientSite);


void __RPC_STUB IOleObject_GetClientSite_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_SetHostNames_Proxy( 
    IOleObject * This,
    /* [in] */ LPCOLESTR szContainerApp,
    /* [unique][in] */ LPCOLESTR szContainerObj);


void __RPC_STUB IOleObject_SetHostNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_Close_Proxy( 
    IOleObject * This,
    /* [in] */ DWORD dwSaveOption);


void __RPC_STUB IOleObject_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_SetMoniker_Proxy( 
    IOleObject * This,
    /* [in] */ DWORD dwWhichMoniker,
    /* [unique][in] */ IMoniker *pmk);


void __RPC_STUB IOleObject_SetMoniker_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_GetMoniker_Proxy( 
    IOleObject * This,
    /* [in] */ DWORD dwAssign,
    /* [in] */ DWORD dwWhichMoniker,
    /* [out] */ IMoniker **ppmk);


void __RPC_STUB IOleObject_GetMoniker_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_InitFromData_Proxy( 
    IOleObject * This,
    /* [unique][in] */ IDataObject *pDataObject,
    /* [in] */ BOOL fCreation,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IOleObject_InitFromData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_GetClipboardData_Proxy( 
    IOleObject * This,
    /* [in] */ DWORD dwReserved,
    /* [out] */ IDataObject **ppDataObject);


void __RPC_STUB IOleObject_GetClipboardData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_DoVerb_Proxy( 
    IOleObject * This,
    /* [in] */ LONG iVerb,
    /* [unique][in] */ LPMSG lpmsg,
    /* [unique][in] */ IOleClientSite *pActiveSite,
    /* [in] */ LONG lindex,
    /* [in] */ HWND hwndParent,
    /* [unique][in] */ LPCRECT lprcPosRect);


void __RPC_STUB IOleObject_DoVerb_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_EnumVerbs_Proxy( 
    IOleObject * This,
    /* [out] */ IEnumOLEVERB **ppEnumOleVerb);


void __RPC_STUB IOleObject_EnumVerbs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_Update_Proxy( 
    IOleObject * This);


void __RPC_STUB IOleObject_Update_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_IsUpToDate_Proxy( 
    IOleObject * This);


void __RPC_STUB IOleObject_IsUpToDate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_GetUserClassID_Proxy( 
    IOleObject * This,
    /* [out] */ CLSID *pClsid);


void __RPC_STUB IOleObject_GetUserClassID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_GetUserType_Proxy( 
    IOleObject * This,
    /* [in] */ DWORD dwFormOfType,
    /* [out] */ LPOLESTR *pszUserType);


void __RPC_STUB IOleObject_GetUserType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_SetExtent_Proxy( 
    IOleObject * This,
    /* [in] */ DWORD dwDrawAspect,
    /* [in] */ SIZEL *psizel);


void __RPC_STUB IOleObject_SetExtent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_GetExtent_Proxy( 
    IOleObject * This,
    /* [in] */ DWORD dwDrawAspect,
    /* [out] */ SIZEL *psizel);


void __RPC_STUB IOleObject_GetExtent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_Advise_Proxy( 
    IOleObject * This,
    /* [unique][in] */ IAdviseSink *pAdvSink,
    /* [out] */ DWORD *pdwConnection);


void __RPC_STUB IOleObject_Advise_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_Unadvise_Proxy( 
    IOleObject * This,
    /* [in] */ DWORD dwConnection);


void __RPC_STUB IOleObject_Unadvise_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_EnumAdvise_Proxy( 
    IOleObject * This,
    /* [out] */ IEnumSTATDATA **ppenumAdvise);


void __RPC_STUB IOleObject_EnumAdvise_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_GetMiscStatus_Proxy( 
    IOleObject * This,
    /* [in] */ DWORD dwAspect,
    /* [out] */ DWORD *pdwStatus);


void __RPC_STUB IOleObject_GetMiscStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleObject_SetColorScheme_Proxy( 
    IOleObject * This,
    /* [in] */ LOGPALETTE *pLogpal);


void __RPC_STUB IOleObject_SetColorScheme_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOleObject_INTERFACE_DEFINED__ */


#ifndef __IOLETypes_INTERFACE_DEFINED__
#define __IOLETypes_INTERFACE_DEFINED__

/* interface IOLETypes */
/* [auto_handle][uuid] */ 

typedef 
enum tagOLERENDER
    {	OLERENDER_NONE	= 0,
	OLERENDER_DRAW	= 1,
	OLERENDER_FORMAT	= 2,
	OLERENDER_ASIS	= 3
    } 	OLERENDER;

typedef OLERENDER *LPOLERENDER;

typedef struct tagOBJECTDESCRIPTOR
    {
    ULONG cbSize;
    CLSID clsid;
    DWORD dwDrawAspect;
    SIZEL sizel;
    POINTL pointl;
    DWORD dwStatus;
    DWORD dwFullUserTypeName;
    DWORD dwSrcOfCopy;
    } 	OBJECTDESCRIPTOR;

typedef struct tagOBJECTDESCRIPTOR *POBJECTDESCRIPTOR;

typedef struct tagOBJECTDESCRIPTOR *LPOBJECTDESCRIPTOR;

typedef struct tagOBJECTDESCRIPTOR LINKSRCDESCRIPTOR;

typedef struct tagOBJECTDESCRIPTOR *PLINKSRCDESCRIPTOR;

typedef struct tagOBJECTDESCRIPTOR *LPLINKSRCDESCRIPTOR;



extern RPC_IF_HANDLE IOLETypes_v0_0_c_ifspec;
extern RPC_IF_HANDLE IOLETypes_v0_0_s_ifspec;
#endif /* __IOLETypes_INTERFACE_DEFINED__ */

#ifndef __IOleWindow_INTERFACE_DEFINED__
#define __IOleWindow_INTERFACE_DEFINED__

/* interface IOleWindow */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IOleWindow *LPOLEWINDOW;


EXTERN_C const IID IID_IOleWindow;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000114-0000-0000-C000-000000000046")
    IOleWindow : public IUnknown
    {
    public:
        virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE GetWindow( 
            /* [out] */ HWND *phwnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ContextSensitiveHelp( 
            /* [in] */ BOOL fEnterMode) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOleWindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleWindow * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleWindow * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleWindow * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IOleWindow * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ContextSensitiveHelp )( 
            IOleWindow * This,
            /* [in] */ BOOL fEnterMode);
        
        END_INTERFACE
    } IOleWindowVtbl;

    interface IOleWindow
    {
        CONST_VTBL struct IOleWindowVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleWindow_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOleWindow_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOleWindow_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOleWindow_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IOleWindow_ContextSensitiveHelp(This,fEnterMode)	\
    (This)->lpVtbl -> ContextSensitiveHelp(This,fEnterMode)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [input_sync] */ HRESULT STDMETHODCALLTYPE IOleWindow_GetWindow_Proxy( 
    IOleWindow * This,
    /* [out] */ HWND *phwnd);


void __RPC_STUB IOleWindow_GetWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleWindow_ContextSensitiveHelp_Proxy( 
    IOleWindow * This,
    /* [in] */ BOOL fEnterMode);


void __RPC_STUB IOleWindow_ContextSensitiveHelp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOleWindow_INTERFACE_DEFINED__ */


#ifndef __IOleLink_INTERFACE_DEFINED__
#define __IOleLink_INTERFACE_DEFINED__

/* interface IOleLink */
/* [uuid][object] */ 

typedef /* [unique] */ IOleLink *LPOLELINK;

typedef 
enum tagOLEUPDATE
    {	OLEUPDATE_ALWAYS	= 1,
	OLEUPDATE_ONCALL	= 3
    } 	OLEUPDATE;

typedef OLEUPDATE *LPOLEUPDATE;

typedef OLEUPDATE *POLEUPDATE;

typedef 
enum tagOLELINKBIND
    {	OLELINKBIND_EVENIFCLASSDIFF	= 1
    } 	OLELINKBIND;


EXTERN_C const IID IID_IOleLink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000011d-0000-0000-C000-000000000046")
    IOleLink : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetUpdateOptions( 
            /* [in] */ DWORD dwUpdateOpt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetUpdateOptions( 
            /* [out] */ DWORD *pdwUpdateOpt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSourceMoniker( 
            /* [unique][in] */ IMoniker *pmk,
            /* [in] */ REFCLSID rclsid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSourceMoniker( 
            /* [out] */ IMoniker **ppmk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSourceDisplayName( 
            /* [in] */ LPCOLESTR pszStatusText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSourceDisplayName( 
            /* [out] */ LPOLESTR *ppszDisplayName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BindToSource( 
            /* [in] */ DWORD bindflags,
            /* [unique][in] */ IBindCtx *pbc) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BindIfRunning( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetBoundSource( 
            /* [out] */ IUnknown **ppunk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnbindSource( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Update( 
            /* [unique][in] */ IBindCtx *pbc) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOleLinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleLink * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleLink * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleLink * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetUpdateOptions )( 
            IOleLink * This,
            /* [in] */ DWORD dwUpdateOpt);
        
        HRESULT ( STDMETHODCALLTYPE *GetUpdateOptions )( 
            IOleLink * This,
            /* [out] */ DWORD *pdwUpdateOpt);
        
        HRESULT ( STDMETHODCALLTYPE *SetSourceMoniker )( 
            IOleLink * This,
            /* [unique][in] */ IMoniker *pmk,
            /* [in] */ REFCLSID rclsid);
        
        HRESULT ( STDMETHODCALLTYPE *GetSourceMoniker )( 
            IOleLink * This,
            /* [out] */ IMoniker **ppmk);
        
        HRESULT ( STDMETHODCALLTYPE *SetSourceDisplayName )( 
            IOleLink * This,
            /* [in] */ LPCOLESTR pszStatusText);
        
        HRESULT ( STDMETHODCALLTYPE *GetSourceDisplayName )( 
            IOleLink * This,
            /* [out] */ LPOLESTR *ppszDisplayName);
        
        HRESULT ( STDMETHODCALLTYPE *BindToSource )( 
            IOleLink * This,
            /* [in] */ DWORD bindflags,
            /* [unique][in] */ IBindCtx *pbc);
        
        HRESULT ( STDMETHODCALLTYPE *BindIfRunning )( 
            IOleLink * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetBoundSource )( 
            IOleLink * This,
            /* [out] */ IUnknown **ppunk);
        
        HRESULT ( STDMETHODCALLTYPE *UnbindSource )( 
            IOleLink * This);
        
        HRESULT ( STDMETHODCALLTYPE *Update )( 
            IOleLink * This,
            /* [unique][in] */ IBindCtx *pbc);
        
        END_INTERFACE
    } IOleLinkVtbl;

    interface IOleLink
    {
        CONST_VTBL struct IOleLinkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleLink_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOleLink_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOleLink_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOleLink_SetUpdateOptions(This,dwUpdateOpt)	\
    (This)->lpVtbl -> SetUpdateOptions(This,dwUpdateOpt)

#define IOleLink_GetUpdateOptions(This,pdwUpdateOpt)	\
    (This)->lpVtbl -> GetUpdateOptions(This,pdwUpdateOpt)

#define IOleLink_SetSourceMoniker(This,pmk,rclsid)	\
    (This)->lpVtbl -> SetSourceMoniker(This,pmk,rclsid)

#define IOleLink_GetSourceMoniker(This,ppmk)	\
    (This)->lpVtbl -> GetSourceMoniker(This,ppmk)

#define IOleLink_SetSourceDisplayName(This,pszStatusText)	\
    (This)->lpVtbl -> SetSourceDisplayName(This,pszStatusText)

#define IOleLink_GetSourceDisplayName(This,ppszDisplayName)	\
    (This)->lpVtbl -> GetSourceDisplayName(This,ppszDisplayName)

#define IOleLink_BindToSource(This,bindflags,pbc)	\
    (This)->lpVtbl -> BindToSource(This,bindflags,pbc)

#define IOleLink_BindIfRunning(This)	\
    (This)->lpVtbl -> BindIfRunning(This)

#define IOleLink_GetBoundSource(This,ppunk)	\
    (This)->lpVtbl -> GetBoundSource(This,ppunk)

#define IOleLink_UnbindSource(This)	\
    (This)->lpVtbl -> UnbindSource(This)

#define IOleLink_Update(This,pbc)	\
    (This)->lpVtbl -> Update(This,pbc)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOleLink_SetUpdateOptions_Proxy( 
    IOleLink * This,
    /* [in] */ DWORD dwUpdateOpt);


void __RPC_STUB IOleLink_SetUpdateOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleLink_GetUpdateOptions_Proxy( 
    IOleLink * This,
    /* [out] */ DWORD *pdwUpdateOpt);


void __RPC_STUB IOleLink_GetUpdateOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleLink_SetSourceMoniker_Proxy( 
    IOleLink * This,
    /* [unique][in] */ IMoniker *pmk,
    /* [in] */ REFCLSID rclsid);


void __RPC_STUB IOleLink_SetSourceMoniker_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleLink_GetSourceMoniker_Proxy( 
    IOleLink * This,
    /* [out] */ IMoniker **ppmk);


void __RPC_STUB IOleLink_GetSourceMoniker_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleLink_SetSourceDisplayName_Proxy( 
    IOleLink * This,
    /* [in] */ LPCOLESTR pszStatusText);


void __RPC_STUB IOleLink_SetSourceDisplayName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleLink_GetSourceDisplayName_Proxy( 
    IOleLink * This,
    /* [out] */ LPOLESTR *ppszDisplayName);


void __RPC_STUB IOleLink_GetSourceDisplayName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleLink_BindToSource_Proxy( 
    IOleLink * This,
    /* [in] */ DWORD bindflags,
    /* [unique][in] */ IBindCtx *pbc);


void __RPC_STUB IOleLink_BindToSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleLink_BindIfRunning_Proxy( 
    IOleLink * This);


void __RPC_STUB IOleLink_BindIfRunning_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleLink_GetBoundSource_Proxy( 
    IOleLink * This,
    /* [out] */ IUnknown **ppunk);


void __RPC_STUB IOleLink_GetBoundSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleLink_UnbindSource_Proxy( 
    IOleLink * This);


void __RPC_STUB IOleLink_UnbindSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleLink_Update_Proxy( 
    IOleLink * This,
    /* [unique][in] */ IBindCtx *pbc);


void __RPC_STUB IOleLink_Update_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOleLink_INTERFACE_DEFINED__ */


#ifndef __IOleItemContainer_INTERFACE_DEFINED__
#define __IOleItemContainer_INTERFACE_DEFINED__

/* interface IOleItemContainer */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IOleItemContainer *LPOLEITEMCONTAINER;

typedef 
enum tagBINDSPEED
    {	BINDSPEED_INDEFINITE	= 1,
	BINDSPEED_MODERATE	= 2,
	BINDSPEED_IMMEDIATE	= 3
    } 	BINDSPEED;

typedef /* [v1_enum] */ 
enum tagOLECONTF
    {	OLECONTF_EMBEDDINGS	= 1,
	OLECONTF_LINKS	= 2,
	OLECONTF_OTHERS	= 4,
	OLECONTF_ONLYUSER	= 8,
	OLECONTF_ONLYIFRUNNING	= 16
    } 	OLECONTF;


EXTERN_C const IID IID_IOleItemContainer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000011c-0000-0000-C000-000000000046")
    IOleItemContainer : public IOleContainer
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetObject( 
            /* [in] */ LPOLESTR pszItem,
            /* [in] */ DWORD dwSpeedNeeded,
            /* [unique][in] */ IBindCtx *pbc,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetObjectStorage( 
            /* [in] */ LPOLESTR pszItem,
            /* [unique][in] */ IBindCtx *pbc,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvStorage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsRunning( 
            /* [in] */ LPOLESTR pszItem) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOleItemContainerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleItemContainer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleItemContainer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleItemContainer * This);
        
        HRESULT ( STDMETHODCALLTYPE *ParseDisplayName )( 
            IOleItemContainer * This,
            /* [unique][in] */ IBindCtx *pbc,
            /* [in] */ LPOLESTR pszDisplayName,
            /* [out] */ ULONG *pchEaten,
            /* [out] */ IMoniker **ppmkOut);
        
        HRESULT ( STDMETHODCALLTYPE *EnumObjects )( 
            IOleItemContainer * This,
            /* [in] */ DWORD grfFlags,
            /* [out] */ IEnumUnknown **ppenum);
        
        HRESULT ( STDMETHODCALLTYPE *LockContainer )( 
            IOleItemContainer * This,
            /* [in] */ BOOL fLock);
        
        HRESULT ( STDMETHODCALLTYPE *GetObject )( 
            IOleItemContainer * This,
            /* [in] */ LPOLESTR pszItem,
            /* [in] */ DWORD dwSpeedNeeded,
            /* [unique][in] */ IBindCtx *pbc,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        HRESULT ( STDMETHODCALLTYPE *GetObjectStorage )( 
            IOleItemContainer * This,
            /* [in] */ LPOLESTR pszItem,
            /* [unique][in] */ IBindCtx *pbc,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvStorage);
        
        HRESULT ( STDMETHODCALLTYPE *IsRunning )( 
            IOleItemContainer * This,
            /* [in] */ LPOLESTR pszItem);
        
        END_INTERFACE
    } IOleItemContainerVtbl;

    interface IOleItemContainer
    {
        CONST_VTBL struct IOleItemContainerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleItemContainer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOleItemContainer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOleItemContainer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOleItemContainer_ParseDisplayName(This,pbc,pszDisplayName,pchEaten,ppmkOut)	\
    (This)->lpVtbl -> ParseDisplayName(This,pbc,pszDisplayName,pchEaten,ppmkOut)


#define IOleItemContainer_EnumObjects(This,grfFlags,ppenum)	\
    (This)->lpVtbl -> EnumObjects(This,grfFlags,ppenum)

#define IOleItemContainer_LockContainer(This,fLock)	\
    (This)->lpVtbl -> LockContainer(This,fLock)


#define IOleItemContainer_GetObject(This,pszItem,dwSpeedNeeded,pbc,riid,ppvObject)	\
    (This)->lpVtbl -> GetObject(This,pszItem,dwSpeedNeeded,pbc,riid,ppvObject)

#define IOleItemContainer_GetObjectStorage(This,pszItem,pbc,riid,ppvStorage)	\
    (This)->lpVtbl -> GetObjectStorage(This,pszItem,pbc,riid,ppvStorage)

#define IOleItemContainer_IsRunning(This,pszItem)	\
    (This)->lpVtbl -> IsRunning(This,pszItem)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOleItemContainer_GetObject_Proxy( 
    IOleItemContainer * This,
    /* [in] */ LPOLESTR pszItem,
    /* [in] */ DWORD dwSpeedNeeded,
    /* [unique][in] */ IBindCtx *pbc,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppvObject);


void __RPC_STUB IOleItemContainer_GetObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleItemContainer_GetObjectStorage_Proxy( 
    IOleItemContainer * This,
    /* [in] */ LPOLESTR pszItem,
    /* [unique][in] */ IBindCtx *pbc,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppvStorage);


void __RPC_STUB IOleItemContainer_GetObjectStorage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleItemContainer_IsRunning_Proxy( 
    IOleItemContainer * This,
    /* [in] */ LPOLESTR pszItem);


void __RPC_STUB IOleItemContainer_IsRunning_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOleItemContainer_INTERFACE_DEFINED__ */


#ifndef __IOleInPlaceUIWindow_INTERFACE_DEFINED__
#define __IOleInPlaceUIWindow_INTERFACE_DEFINED__

/* interface IOleInPlaceUIWindow */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IOleInPlaceUIWindow *LPOLEINPLACEUIWINDOW;

typedef RECT BORDERWIDTHS;

typedef LPRECT LPBORDERWIDTHS;

typedef LPCRECT LPCBORDERWIDTHS;


EXTERN_C const IID IID_IOleInPlaceUIWindow;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000115-0000-0000-C000-000000000046")
    IOleInPlaceUIWindow : public IOleWindow
    {
    public:
        virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE GetBorder( 
            /* [out] */ LPRECT lprectBorder) = 0;
        
        virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE RequestBorderSpace( 
            /* [unique][in] */ LPCBORDERWIDTHS pborderwidths) = 0;
        
        virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE SetBorderSpace( 
            /* [unique][in] */ LPCBORDERWIDTHS pborderwidths) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetActiveObject( 
            /* [unique][in] */ IOleInPlaceActiveObject *pActiveObject,
            /* [unique][string][in] */ LPCOLESTR pszObjName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOleInPlaceUIWindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleInPlaceUIWindow * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleInPlaceUIWindow * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleInPlaceUIWindow * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IOleInPlaceUIWindow * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ContextSensitiveHelp )( 
            IOleInPlaceUIWindow * This,
            /* [in] */ BOOL fEnterMode);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetBorder )( 
            IOleInPlaceUIWindow * This,
            /* [out] */ LPRECT lprectBorder);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *RequestBorderSpace )( 
            IOleInPlaceUIWindow * This,
            /* [unique][in] */ LPCBORDERWIDTHS pborderwidths);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *SetBorderSpace )( 
            IOleInPlaceUIWindow * This,
            /* [unique][in] */ LPCBORDERWIDTHS pborderwidths);
        
        HRESULT ( STDMETHODCALLTYPE *SetActiveObject )( 
            IOleInPlaceUIWindow * This,
            /* [unique][in] */ IOleInPlaceActiveObject *pActiveObject,
            /* [unique][string][in] */ LPCOLESTR pszObjName);
        
        END_INTERFACE
    } IOleInPlaceUIWindowVtbl;

    interface IOleInPlaceUIWindow
    {
        CONST_VTBL struct IOleInPlaceUIWindowVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleInPlaceUIWindow_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOleInPlaceUIWindow_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOleInPlaceUIWindow_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOleInPlaceUIWindow_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IOleInPlaceUIWindow_ContextSensitiveHelp(This,fEnterMode)	\
    (This)->lpVtbl -> ContextSensitiveHelp(This,fEnterMode)


#define IOleInPlaceUIWindow_GetBorder(This,lprectBorder)	\
    (This)->lpVtbl -> GetBorder(This,lprectBorder)

#define IOleInPlaceUIWindow_RequestBorderSpace(This,pborderwidths)	\
    (This)->lpVtbl -> RequestBorderSpace(This,pborderwidths)

#define IOleInPlaceUIWindow_SetBorderSpace(This,pborderwidths)	\
    (This)->lpVtbl -> SetBorderSpace(This,pborderwidths)

#define IOleInPlaceUIWindow_SetActiveObject(This,pActiveObject,pszObjName)	\
    (This)->lpVtbl -> SetActiveObject(This,pActiveObject,pszObjName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [input_sync] */ HRESULT STDMETHODCALLTYPE IOleInPlaceUIWindow_GetBorder_Proxy( 
    IOleInPlaceUIWindow * This,
    /* [out] */ LPRECT lprectBorder);


void __RPC_STUB IOleInPlaceUIWindow_GetBorder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [input_sync] */ HRESULT STDMETHODCALLTYPE IOleInPlaceUIWindow_RequestBorderSpace_Proxy( 
    IOleInPlaceUIWindow * This,
    /* [unique][in] */ LPCBORDERWIDTHS pborderwidths);


void __RPC_STUB IOleInPlaceUIWindow_RequestBorderSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [input_sync] */ HRESULT STDMETHODCALLTYPE IOleInPlaceUIWindow_SetBorderSpace_Proxy( 
    IOleInPlaceUIWindow * This,
    /* [unique][in] */ LPCBORDERWIDTHS pborderwidths);


void __RPC_STUB IOleInPlaceUIWindow_SetBorderSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleInPlaceUIWindow_SetActiveObject_Proxy( 
    IOleInPlaceUIWindow * This,
    /* [unique][in] */ IOleInPlaceActiveObject *pActiveObject,
    /* [unique][string][in] */ LPCOLESTR pszObjName);


void __RPC_STUB IOleInPlaceUIWindow_SetActiveObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOleInPlaceUIWindow_INTERFACE_DEFINED__ */


#ifndef __IOleInPlaceActiveObject_INTERFACE_DEFINED__
#define __IOleInPlaceActiveObject_INTERFACE_DEFINED__

/* interface IOleInPlaceActiveObject */
/* [uuid][object] */ 

typedef /* [unique] */ IOleInPlaceActiveObject *LPOLEINPLACEACTIVEOBJECT;


EXTERN_C const IID IID_IOleInPlaceActiveObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000117-0000-0000-C000-000000000046")
    IOleInPlaceActiveObject : public IOleWindow
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE TranslateAccelerator( 
            /* [in] */ LPMSG lpmsg) = 0;
        
        virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE OnFrameWindowActivate( 
            /* [in] */ BOOL fActivate) = 0;
        
        virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE OnDocWindowActivate( 
            /* [in] */ BOOL fActivate) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE ResizeBorder( 
            /* [in] */ LPCRECT prcBorder,
            /* [unique][in] */ IOleInPlaceUIWindow *pUIWindow,
            /* [in] */ BOOL fFrameWindow) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnableModeless( 
            /* [in] */ BOOL fEnable) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOleInPlaceActiveObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleInPlaceActiveObject * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleInPlaceActiveObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleInPlaceActiveObject * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IOleInPlaceActiveObject * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ContextSensitiveHelp )( 
            IOleInPlaceActiveObject * This,
            /* [in] */ BOOL fEnterMode);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *TranslateAccelerator )( 
            IOleInPlaceActiveObject * This,
            /* [in] */ LPMSG lpmsg);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *OnFrameWindowActivate )( 
            IOleInPlaceActiveObject * This,
            /* [in] */ BOOL fActivate);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *OnDocWindowActivate )( 
            IOleInPlaceActiveObject * This,
            /* [in] */ BOOL fActivate);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *ResizeBorder )( 
            IOleInPlaceActiveObject * This,
            /* [in] */ LPCRECT prcBorder,
            /* [unique][in] */ IOleInPlaceUIWindow *pUIWindow,
            /* [in] */ BOOL fFrameWindow);
        
        HRESULT ( STDMETHODCALLTYPE *EnableModeless )( 
            IOleInPlaceActiveObject * This,
            /* [in] */ BOOL fEnable);
        
        END_INTERFACE
    } IOleInPlaceActiveObjectVtbl;

    interface IOleInPlaceActiveObject
    {
        CONST_VTBL struct IOleInPlaceActiveObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleInPlaceActiveObject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOleInPlaceActiveObject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOleInPlaceActiveObject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOleInPlaceActiveObject_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IOleInPlaceActiveObject_ContextSensitiveHelp(This,fEnterMode)	\
    (This)->lpVtbl -> ContextSensitiveHelp(This,fEnterMode)


#define IOleInPlaceActiveObject_TranslateAccelerator(This,lpmsg)	\
    (This)->lpVtbl -> TranslateAccelerator(This,lpmsg)

#define IOleInPlaceActiveObject_OnFrameWindowActivate(This,fActivate)	\
    (This)->lpVtbl -> OnFrameWindowActivate(This,fActivate)

#define IOleInPlaceActiveObject_OnDocWindowActivate(This,fActivate)	\
    (This)->lpVtbl -> OnDocWindowActivate(This,fActivate)

#define IOleInPlaceActiveObject_ResizeBorder(This,prcBorder,pUIWindow,fFrameWindow)	\
    (This)->lpVtbl -> ResizeBorder(This,prcBorder,pUIWindow,fFrameWindow)

#define IOleInPlaceActiveObject_EnableModeless(This,fEnable)	\
    (This)->lpVtbl -> EnableModeless(This,fEnable)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_RemoteTranslateAccelerator_Proxy( 
    IOleInPlaceActiveObject * This);


void __RPC_STUB IOleInPlaceActiveObject_RemoteTranslateAccelerator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [input_sync] */ HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_OnFrameWindowActivate_Proxy( 
    IOleInPlaceActiveObject * This,
    /* [in] */ BOOL fActivate);


void __RPC_STUB IOleInPlaceActiveObject_OnFrameWindowActivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [input_sync] */ HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_OnDocWindowActivate_Proxy( 
    IOleInPlaceActiveObject * This,
    /* [in] */ BOOL fActivate);


void __RPC_STUB IOleInPlaceActiveObject_OnDocWindowActivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [input_sync][call_as] */ HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_RemoteResizeBorder_Proxy( 
    IOleInPlaceActiveObject * This,
    /* [in] */ LPCRECT prcBorder,
    /* [in] */ REFIID riid,
    /* [iid_is][unique][in] */ IOleInPlaceUIWindow *pUIWindow,
    /* [in] */ BOOL fFrameWindow);


void __RPC_STUB IOleInPlaceActiveObject_RemoteResizeBorder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_EnableModeless_Proxy( 
    IOleInPlaceActiveObject * This,
    /* [in] */ BOOL fEnable);


void __RPC_STUB IOleInPlaceActiveObject_EnableModeless_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOleInPlaceActiveObject_INTERFACE_DEFINED__ */


#ifndef __IOleInPlaceFrame_INTERFACE_DEFINED__
#define __IOleInPlaceFrame_INTERFACE_DEFINED__

/* interface IOleInPlaceFrame */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IOleInPlaceFrame *LPOLEINPLACEFRAME;

typedef struct tagOIFI
    {
    UINT cb;
    BOOL fMDIApp;
    HWND hwndFrame;
    HACCEL haccel;
    UINT cAccelEntries;
    } 	OLEINPLACEFRAMEINFO;

typedef struct tagOIFI *LPOLEINPLACEFRAMEINFO;

typedef struct tagOleMenuGroupWidths
    {
    LONG width[ 6 ];
    } 	OLEMENUGROUPWIDTHS;

typedef struct tagOleMenuGroupWidths *LPOLEMENUGROUPWIDTHS;

typedef HGLOBAL HOLEMENU;


EXTERN_C const IID IID_IOleInPlaceFrame;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000116-0000-0000-C000-000000000046")
    IOleInPlaceFrame : public IOleInPlaceUIWindow
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InsertMenus( 
            /* [in] */ HMENU hmenuShared,
            /* [out][in] */ LPOLEMENUGROUPWIDTHS lpMenuWidths) = 0;
        
        virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE SetMenu( 
            /* [in] */ HMENU hmenuShared,
            /* [in] */ HOLEMENU holemenu,
            /* [in] */ HWND hwndActiveObject) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveMenus( 
            /* [in] */ HMENU hmenuShared) = 0;
        
        virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE SetStatusText( 
            /* [unique][in] */ LPCOLESTR pszStatusText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnableModeless( 
            /* [in] */ BOOL fEnable) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator( 
            /* [in] */ LPMSG lpmsg,
            /* [in] */ WORD wID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOleInPlaceFrameVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleInPlaceFrame * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleInPlaceFrame * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleInPlaceFrame * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IOleInPlaceFrame * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ContextSensitiveHelp )( 
            IOleInPlaceFrame * This,
            /* [in] */ BOOL fEnterMode);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetBorder )( 
            IOleInPlaceFrame * This,
            /* [out] */ LPRECT lprectBorder);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *RequestBorderSpace )( 
            IOleInPlaceFrame * This,
            /* [unique][in] */ LPCBORDERWIDTHS pborderwidths);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *SetBorderSpace )( 
            IOleInPlaceFrame * This,
            /* [unique][in] */ LPCBORDERWIDTHS pborderwidths);
        
        HRESULT ( STDMETHODCALLTYPE *SetActiveObject )( 
            IOleInPlaceFrame * This,
            /* [unique][in] */ IOleInPlaceActiveObject *pActiveObject,
            /* [unique][string][in] */ LPCOLESTR pszObjName);
        
        HRESULT ( STDMETHODCALLTYPE *InsertMenus )( 
            IOleInPlaceFrame * This,
            /* [in] */ HMENU hmenuShared,
            /* [out][in] */ LPOLEMENUGROUPWIDTHS lpMenuWidths);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *SetMenu )( 
            IOleInPlaceFrame * This,
            /* [in] */ HMENU hmenuShared,
            /* [in] */ HOLEMENU holemenu,
            /* [in] */ HWND hwndActiveObject);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveMenus )( 
            IOleInPlaceFrame * This,
            /* [in] */ HMENU hmenuShared);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *SetStatusText )( 
            IOleInPlaceFrame * This,
            /* [unique][in] */ LPCOLESTR pszStatusText);
        
        HRESULT ( STDMETHODCALLTYPE *EnableModeless )( 
            IOleInPlaceFrame * This,
            /* [in] */ BOOL fEnable);
        
        HRESULT ( STDMETHODCALLTYPE *TranslateAccelerator )( 
            IOleInPlaceFrame * This,
            /* [in] */ LPMSG lpmsg,
            /* [in] */ WORD wID);
        
        END_INTERFACE
    } IOleInPlaceFrameVtbl;

    interface IOleInPlaceFrame
    {
        CONST_VTBL struct IOleInPlaceFrameVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleInPlaceFrame_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOleInPlaceFrame_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOleInPlaceFrame_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOleInPlaceFrame_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IOleInPlaceFrame_ContextSensitiveHelp(This,fEnterMode)	\
    (This)->lpVtbl -> ContextSensitiveHelp(This,fEnterMode)


#define IOleInPlaceFrame_GetBorder(This,lprectBorder)	\
    (This)->lpVtbl -> GetBorder(This,lprectBorder)

#define IOleInPlaceFrame_RequestBorderSpace(This,pborderwidths)	\
    (This)->lpVtbl -> RequestBorderSpace(This,pborderwidths)

#define IOleInPlaceFrame_SetBorderSpace(This,pborderwidths)	\
    (This)->lpVtbl -> SetBorderSpace(This,pborderwidths)

#define IOleInPlaceFrame_SetActiveObject(This,pActiveObject,pszObjName)	\
    (This)->lpVtbl -> SetActiveObject(This,pActiveObject,pszObjName)


#define IOleInPlaceFrame_InsertMenus(This,hmenuShared,lpMenuWidths)	\
    (This)->lpVtbl -> InsertMenus(This,hmenuShared,lpMenuWidths)

#define IOleInPlaceFrame_SetMenu(This,hmenuShared,holemenu,hwndActiveObject)	\
    (This)->lpVtbl -> SetMenu(This,hmenuShared,holemenu,hwndActiveObject)

#define IOleInPlaceFrame_RemoveMenus(This,hmenuShared)	\
    (This)->lpVtbl -> RemoveMenus(This,hmenuShared)

#define IOleInPlaceFrame_SetStatusText(This,pszStatusText)	\
    (This)->lpVtbl -> SetStatusText(This,pszStatusText)

#define IOleInPlaceFrame_EnableModeless(This,fEnable)	\
    (This)->lpVtbl -> EnableModeless(This,fEnable)

#define IOleInPlaceFrame_TranslateAccelerator(This,lpmsg,wID)	\
    (This)->lpVtbl -> TranslateAccelerator(This,lpmsg,wID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOleInPlaceFrame_InsertMenus_Proxy( 
    IOleInPlaceFrame * This,
    /* [in] */ HMENU hmenuShared,
    /* [out][in] */ LPOLEMENUGROUPWIDTHS lpMenuWidths);


void __RPC_STUB IOleInPlaceFrame_InsertMenus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [input_sync] */ HRESULT STDMETHODCALLTYPE IOleInPlaceFrame_SetMenu_Proxy( 
    IOleInPlaceFrame * This,
    /* [in] */ HMENU hmenuShared,
    /* [in] */ HOLEMENU holemenu,
    /* [in] */ HWND hwndActiveObject);


void __RPC_STUB IOleInPlaceFrame_SetMenu_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleInPlaceFrame_RemoveMenus_Proxy( 
    IOleInPlaceFrame * This,
    /* [in] */ HMENU hmenuShared);


void __RPC_STUB IOleInPlaceFrame_RemoveMenus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [input_sync] */ HRESULT STDMETHODCALLTYPE IOleInPlaceFrame_SetStatusText_Proxy( 
    IOleInPlaceFrame * This,
    /* [unique][in] */ LPCOLESTR pszStatusText);


void __RPC_STUB IOleInPlaceFrame_SetStatusText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleInPlaceFrame_EnableModeless_Proxy( 
    IOleInPlaceFrame * This,
    /* [in] */ BOOL fEnable);


void __RPC_STUB IOleInPlaceFrame_EnableModeless_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleInPlaceFrame_TranslateAccelerator_Proxy( 
    IOleInPlaceFrame * This,
    /* [in] */ LPMSG lpmsg,
    /* [in] */ WORD wID);


void __RPC_STUB IOleInPlaceFrame_TranslateAccelerator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOleInPlaceFrame_INTERFACE_DEFINED__ */


#ifndef __IOleInPlaceObject_INTERFACE_DEFINED__
#define __IOleInPlaceObject_INTERFACE_DEFINED__

/* interface IOleInPlaceObject */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IOleInPlaceObject *LPOLEINPLACEOBJECT;


EXTERN_C const IID IID_IOleInPlaceObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000113-0000-0000-C000-000000000046")
    IOleInPlaceObject : public IOleWindow
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InPlaceDeactivate( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UIDeactivate( void) = 0;
        
        virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE SetObjectRects( 
            /* [in] */ LPCRECT lprcPosRect,
            /* [in] */ LPCRECT lprcClipRect) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReactivateAndUndo( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOleInPlaceObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleInPlaceObject * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleInPlaceObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleInPlaceObject * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IOleInPlaceObject * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ContextSensitiveHelp )( 
            IOleInPlaceObject * This,
            /* [in] */ BOOL fEnterMode);
        
        HRESULT ( STDMETHODCALLTYPE *InPlaceDeactivate )( 
            IOleInPlaceObject * This);
        
        HRESULT ( STDMETHODCALLTYPE *UIDeactivate )( 
            IOleInPlaceObject * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *SetObjectRects )( 
            IOleInPlaceObject * This,
            /* [in] */ LPCRECT lprcPosRect,
            /* [in] */ LPCRECT lprcClipRect);
        
        HRESULT ( STDMETHODCALLTYPE *ReactivateAndUndo )( 
            IOleInPlaceObject * This);
        
        END_INTERFACE
    } IOleInPlaceObjectVtbl;

    interface IOleInPlaceObject
    {
        CONST_VTBL struct IOleInPlaceObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleInPlaceObject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOleInPlaceObject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOleInPlaceObject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOleInPlaceObject_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IOleInPlaceObject_ContextSensitiveHelp(This,fEnterMode)	\
    (This)->lpVtbl -> ContextSensitiveHelp(This,fEnterMode)


#define IOleInPlaceObject_InPlaceDeactivate(This)	\
    (This)->lpVtbl -> InPlaceDeactivate(This)

#define IOleInPlaceObject_UIDeactivate(This)	\
    (This)->lpVtbl -> UIDeactivate(This)

#define IOleInPlaceObject_SetObjectRects(This,lprcPosRect,lprcClipRect)	\
    (This)->lpVtbl -> SetObjectRects(This,lprcPosRect,lprcClipRect)

#define IOleInPlaceObject_ReactivateAndUndo(This)	\
    (This)->lpVtbl -> ReactivateAndUndo(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOleInPlaceObject_InPlaceDeactivate_Proxy( 
    IOleInPlaceObject * This);


void __RPC_STUB IOleInPlaceObject_InPlaceDeactivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleInPlaceObject_UIDeactivate_Proxy( 
    IOleInPlaceObject * This);


void __RPC_STUB IOleInPlaceObject_UIDeactivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [input_sync] */ HRESULT STDMETHODCALLTYPE IOleInPlaceObject_SetObjectRects_Proxy( 
    IOleInPlaceObject * This,
    /* [in] */ LPCRECT lprcPosRect,
    /* [in] */ LPCRECT lprcClipRect);


void __RPC_STUB IOleInPlaceObject_SetObjectRects_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleInPlaceObject_ReactivateAndUndo_Proxy( 
    IOleInPlaceObject * This);


void __RPC_STUB IOleInPlaceObject_ReactivateAndUndo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOleInPlaceObject_INTERFACE_DEFINED__ */


#ifndef __IOleInPlaceSite_INTERFACE_DEFINED__
#define __IOleInPlaceSite_INTERFACE_DEFINED__

/* interface IOleInPlaceSite */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IOleInPlaceSite *LPOLEINPLACESITE;


EXTERN_C const IID IID_IOleInPlaceSite;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000119-0000-0000-C000-000000000046")
    IOleInPlaceSite : public IOleWindow
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CanInPlaceActivate( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnInPlaceActivate( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnUIActivate( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetWindowContext( 
            /* [out] */ IOleInPlaceFrame **ppFrame,
            /* [out] */ IOleInPlaceUIWindow **ppDoc,
            /* [out] */ LPRECT lprcPosRect,
            /* [out] */ LPRECT lprcClipRect,
            /* [out][in] */ LPOLEINPLACEFRAMEINFO lpFrameInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Scroll( 
            /* [in] */ SIZE scrollExtant) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnUIDeactivate( 
            /* [in] */ BOOL fUndoable) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnInPlaceDeactivate( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DiscardUndoState( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DeactivateAndUndo( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnPosRectChange( 
            /* [in] */ LPCRECT lprcPosRect) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOleInPlaceSiteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleInPlaceSite * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleInPlaceSite * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleInPlaceSite * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IOleInPlaceSite * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ContextSensitiveHelp )( 
            IOleInPlaceSite * This,
            /* [in] */ BOOL fEnterMode);
        
        HRESULT ( STDMETHODCALLTYPE *CanInPlaceActivate )( 
            IOleInPlaceSite * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnInPlaceActivate )( 
            IOleInPlaceSite * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnUIActivate )( 
            IOleInPlaceSite * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetWindowContext )( 
            IOleInPlaceSite * This,
            /* [out] */ IOleInPlaceFrame **ppFrame,
            /* [out] */ IOleInPlaceUIWindow **ppDoc,
            /* [out] */ LPRECT lprcPosRect,
            /* [out] */ LPRECT lprcClipRect,
            /* [out][in] */ LPOLEINPLACEFRAMEINFO lpFrameInfo);
        
        HRESULT ( STDMETHODCALLTYPE *Scroll )( 
            IOleInPlaceSite * This,
            /* [in] */ SIZE scrollExtant);
        
        HRESULT ( STDMETHODCALLTYPE *OnUIDeactivate )( 
            IOleInPlaceSite * This,
            /* [in] */ BOOL fUndoable);
        
        HRESULT ( STDMETHODCALLTYPE *OnInPlaceDeactivate )( 
            IOleInPlaceSite * This);
        
        HRESULT ( STDMETHODCALLTYPE *DiscardUndoState )( 
            IOleInPlaceSite * This);
        
        HRESULT ( STDMETHODCALLTYPE *DeactivateAndUndo )( 
            IOleInPlaceSite * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnPosRectChange )( 
            IOleInPlaceSite * This,
            /* [in] */ LPCRECT lprcPosRect);
        
        END_INTERFACE
    } IOleInPlaceSiteVtbl;

    interface IOleInPlaceSite
    {
        CONST_VTBL struct IOleInPlaceSiteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleInPlaceSite_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOleInPlaceSite_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOleInPlaceSite_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOleInPlaceSite_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IOleInPlaceSite_ContextSensitiveHelp(This,fEnterMode)	\
    (This)->lpVtbl -> ContextSensitiveHelp(This,fEnterMode)


#define IOleInPlaceSite_CanInPlaceActivate(This)	\
    (This)->lpVtbl -> CanInPlaceActivate(This)

#define IOleInPlaceSite_OnInPlaceActivate(This)	\
    (This)->lpVtbl -> OnInPlaceActivate(This)

#define IOleInPlaceSite_OnUIActivate(This)	\
    (This)->lpVtbl -> OnUIActivate(This)

#define IOleInPlaceSite_GetWindowContext(This,ppFrame,ppDoc,lprcPosRect,lprcClipRect,lpFrameInfo)	\
    (This)->lpVtbl -> GetWindowContext(This,ppFrame,ppDoc,lprcPosRect,lprcClipRect,lpFrameInfo)

#define IOleInPlaceSite_Scroll(This,scrollExtant)	\
    (This)->lpVtbl -> Scroll(This,scrollExtant)

#define IOleInPlaceSite_OnUIDeactivate(This,fUndoable)	\
    (This)->lpVtbl -> OnUIDeactivate(This,fUndoable)

#define IOleInPlaceSite_OnInPlaceDeactivate(This)	\
    (This)->lpVtbl -> OnInPlaceDeactivate(This)

#define IOleInPlaceSite_DiscardUndoState(This)	\
    (This)->lpVtbl -> DiscardUndoState(This)

#define IOleInPlaceSite_DeactivateAndUndo(This)	\
    (This)->lpVtbl -> DeactivateAndUndo(This)

#define IOleInPlaceSite_OnPosRectChange(This,lprcPosRect)	\
    (This)->lpVtbl -> OnPosRectChange(This,lprcPosRect)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOleInPlaceSite_CanInPlaceActivate_Proxy( 
    IOleInPlaceSite * This);


void __RPC_STUB IOleInPlaceSite_CanInPlaceActivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleInPlaceSite_OnInPlaceActivate_Proxy( 
    IOleInPlaceSite * This);


void __RPC_STUB IOleInPlaceSite_OnInPlaceActivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleInPlaceSite_OnUIActivate_Proxy( 
    IOleInPlaceSite * This);


void __RPC_STUB IOleInPlaceSite_OnUIActivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleInPlaceSite_GetWindowContext_Proxy( 
    IOleInPlaceSite * This,
    /* [out] */ IOleInPlaceFrame **ppFrame,
    /* [out] */ IOleInPlaceUIWindow **ppDoc,
    /* [out] */ LPRECT lprcPosRect,
    /* [out] */ LPRECT lprcClipRect,
    /* [out][in] */ LPOLEINPLACEFRAMEINFO lpFrameInfo);


void __RPC_STUB IOleInPlaceSite_GetWindowContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleInPlaceSite_Scroll_Proxy( 
    IOleInPlaceSite * This,
    /* [in] */ SIZE scrollExtant);


void __RPC_STUB IOleInPlaceSite_Scroll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleInPlaceSite_OnUIDeactivate_Proxy( 
    IOleInPlaceSite * This,
    /* [in] */ BOOL fUndoable);


void __RPC_STUB IOleInPlaceSite_OnUIDeactivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleInPlaceSite_OnInPlaceDeactivate_Proxy( 
    IOleInPlaceSite * This);


void __RPC_STUB IOleInPlaceSite_OnInPlaceDeactivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleInPlaceSite_DiscardUndoState_Proxy( 
    IOleInPlaceSite * This);


void __RPC_STUB IOleInPlaceSite_DiscardUndoState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleInPlaceSite_DeactivateAndUndo_Proxy( 
    IOleInPlaceSite * This);


void __RPC_STUB IOleInPlaceSite_DeactivateAndUndo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleInPlaceSite_OnPosRectChange_Proxy( 
    IOleInPlaceSite * This,
    /* [in] */ LPCRECT lprcPosRect);


void __RPC_STUB IOleInPlaceSite_OnPosRectChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOleInPlaceSite_INTERFACE_DEFINED__ */


#ifndef __IContinue_INTERFACE_DEFINED__
#define __IContinue_INTERFACE_DEFINED__

/* interface IContinue */
/* [uuid][object] */ 


EXTERN_C const IID IID_IContinue;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000012a-0000-0000-C000-000000000046")
    IContinue : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE FContinue( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IContinueVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IContinue * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IContinue * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IContinue * This);
        
        HRESULT ( STDMETHODCALLTYPE *FContinue )( 
            IContinue * This);
        
        END_INTERFACE
    } IContinueVtbl;

    interface IContinue
    {
        CONST_VTBL struct IContinueVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IContinue_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IContinue_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IContinue_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IContinue_FContinue(This)	\
    (This)->lpVtbl -> FContinue(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IContinue_FContinue_Proxy( 
    IContinue * This);


void __RPC_STUB IContinue_FContinue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IContinue_INTERFACE_DEFINED__ */


#ifndef __IViewObject_INTERFACE_DEFINED__
#define __IViewObject_INTERFACE_DEFINED__

/* interface IViewObject */
/* [uuid][object] */ 

typedef /* [unique] */ IViewObject *LPVIEWOBJECT;


EXTERN_C const IID IID_IViewObject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0000010d-0000-0000-C000-000000000046")
    IViewObject : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE Draw( 
            /* [in] */ DWORD dwDrawAspect,
            /* [in] */ LONG lindex,
            /* [unique][in] */ void *pvAspect,
            /* [unique][in] */ DVTARGETDEVICE *ptd,
            /* [in] */ HDC hdcTargetDev,
            /* [in] */ HDC hdcDraw,
            /* [in] */ LPCRECTL lprcBounds,
            /* [unique][in] */ LPCRECTL lprcWBounds,
            /* [in] */ BOOL ( STDMETHODCALLTYPE *pfnContinue )( 
                ULONG_PTR dwContinue),
            /* [in] */ ULONG_PTR dwContinue) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetColorSet( 
            /* [in] */ DWORD dwDrawAspect,
            /* [in] */ LONG lindex,
            /* [unique][in] */ void *pvAspect,
            /* [unique][in] */ DVTARGETDEVICE *ptd,
            /* [in] */ HDC hicTargetDev,
            /* [out] */ LOGPALETTE **ppColorSet) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE Freeze( 
            /* [in] */ DWORD dwDrawAspect,
            /* [in] */ LONG lindex,
            /* [unique][in] */ void *pvAspect,
            /* [out] */ DWORD *pdwFreeze) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Unfreeze( 
            /* [in] */ DWORD dwFreeze) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetAdvise( 
            /* [in] */ DWORD aspects,
            /* [in] */ DWORD advf,
            /* [unique][in] */ IAdviseSink *pAdvSink) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetAdvise( 
            /* [unique][out] */ DWORD *pAspects,
            /* [unique][out] */ DWORD *pAdvf,
            /* [out] */ IAdviseSink **ppAdvSink) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IViewObjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IViewObject * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IViewObject * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IViewObject * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Draw )( 
            IViewObject * This,
            /* [in] */ DWORD dwDrawAspect,
            /* [in] */ LONG lindex,
            /* [unique][in] */ void *pvAspect,
            /* [unique][in] */ DVTARGETDEVICE *ptd,
            /* [in] */ HDC hdcTargetDev,
            /* [in] */ HDC hdcDraw,
            /* [in] */ LPCRECTL lprcBounds,
            /* [unique][in] */ LPCRECTL lprcWBounds,
            /* [in] */ BOOL ( STDMETHODCALLTYPE *pfnContinue )( 
                ULONG_PTR dwContinue),
            /* [in] */ ULONG_PTR dwContinue);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *GetColorSet )( 
            IViewObject * This,
            /* [in] */ DWORD dwDrawAspect,
            /* [in] */ LONG lindex,
            /* [unique][in] */ void *pvAspect,
            /* [unique][in] */ DVTARGETDEVICE *ptd,
            /* [in] */ HDC hicTargetDev,
            /* [out] */ LOGPALETTE **ppColorSet);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Freeze )( 
            IViewObject * This,
            /* [in] */ DWORD dwDrawAspect,
            /* [in] */ LONG lindex,
            /* [unique][in] */ void *pvAspect,
            /* [out] */ DWORD *pdwFreeze);
        
        HRESULT ( STDMETHODCALLTYPE *Unfreeze )( 
            IViewObject * This,
            /* [in] */ DWORD dwFreeze);
        
        HRESULT ( STDMETHODCALLTYPE *SetAdvise )( 
            IViewObject * This,
            /* [in] */ DWORD aspects,
            /* [in] */ DWORD advf,
            /* [unique][in] */ IAdviseSink *pAdvSink);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *GetAdvise )( 
            IViewObject * This,
            /* [unique][out] */ DWORD *pAspects,
            /* [unique][out] */ DWORD *pAdvf,
            /* [out] */ IAdviseSink **ppAdvSink);
        
        END_INTERFACE
    } IViewObjectVtbl;

    interface IViewObject
    {
        CONST_VTBL struct IViewObjectVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IViewObject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IViewObject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IViewObject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IViewObject_Draw(This,dwDrawAspect,lindex,pvAspect,ptd,hdcTargetDev,hdcDraw,lprcBounds,lprcWBounds,pfnContinue,dwContinue)	\
    (This)->lpVtbl -> Draw(This,dwDrawAspect,lindex,pvAspect,ptd,hdcTargetDev,hdcDraw,lprcBounds,lprcWBounds,pfnContinue,dwContinue)

#define IViewObject_GetColorSet(This,dwDrawAspect,lindex,pvAspect,ptd,hicTargetDev,ppColorSet)	\
    (This)->lpVtbl -> GetColorSet(This,dwDrawAspect,lindex,pvAspect,ptd,hicTargetDev,ppColorSet)

#define IViewObject_Freeze(This,dwDrawAspect,lindex,pvAspect,pdwFreeze)	\
    (This)->lpVtbl -> Freeze(This,dwDrawAspect,lindex,pvAspect,pdwFreeze)

#define IViewObject_Unfreeze(This,dwFreeze)	\
    (This)->lpVtbl -> Unfreeze(This,dwFreeze)

#define IViewObject_SetAdvise(This,aspects,advf,pAdvSink)	\
    (This)->lpVtbl -> SetAdvise(This,aspects,advf,pAdvSink)

#define IViewObject_GetAdvise(This,pAspects,pAdvf,ppAdvSink)	\
    (This)->lpVtbl -> GetAdvise(This,pAspects,pAdvf,ppAdvSink)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IViewObject_RemoteDraw_Proxy( 
    IViewObject * This,
    /* [in] */ DWORD dwDrawAspect,
    /* [in] */ LONG lindex,
    /* [in] */ ULONG_PTR pvAspect,
    /* [unique][in] */ DVTARGETDEVICE *ptd,
    /* [in] */ ULONG_PTR hdcTargetDev,
    /* [in] */ ULONG_PTR hdcDraw,
    /* [in] */ LPCRECTL lprcBounds,
    /* [unique][in] */ LPCRECTL lprcWBounds,
    /* [in] */ IContinue *pContinue);


void __RPC_STUB IViewObject_RemoteDraw_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IViewObject_RemoteGetColorSet_Proxy( 
    IViewObject * This,
    /* [in] */ DWORD dwDrawAspect,
    /* [in] */ LONG lindex,
    /* [in] */ ULONG_PTR pvAspect,
    /* [unique][in] */ DVTARGETDEVICE *ptd,
    /* [in] */ ULONG_PTR hicTargetDev,
    /* [out] */ LOGPALETTE **ppColorSet);


void __RPC_STUB IViewObject_RemoteGetColorSet_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IViewObject_RemoteFreeze_Proxy( 
    IViewObject * This,
    /* [in] */ DWORD dwDrawAspect,
    /* [in] */ LONG lindex,
    /* [in] */ ULONG_PTR pvAspect,
    /* [out] */ DWORD *pdwFreeze);


void __RPC_STUB IViewObject_RemoteFreeze_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IViewObject_Unfreeze_Proxy( 
    IViewObject * This,
    /* [in] */ DWORD dwFreeze);


void __RPC_STUB IViewObject_Unfreeze_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IViewObject_SetAdvise_Proxy( 
    IViewObject * This,
    /* [in] */ DWORD aspects,
    /* [in] */ DWORD advf,
    /* [unique][in] */ IAdviseSink *pAdvSink);


void __RPC_STUB IViewObject_SetAdvise_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IViewObject_RemoteGetAdvise_Proxy( 
    IViewObject * This,
    /* [out] */ DWORD *pAspects,
    /* [out] */ DWORD *pAdvf,
    /* [out] */ IAdviseSink **ppAdvSink);


void __RPC_STUB IViewObject_RemoteGetAdvise_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IViewObject_INTERFACE_DEFINED__ */


#ifndef __IViewObject2_INTERFACE_DEFINED__
#define __IViewObject2_INTERFACE_DEFINED__

/* interface IViewObject2 */
/* [uuid][object] */ 

typedef /* [unique] */ IViewObject2 *LPVIEWOBJECT2;


EXTERN_C const IID IID_IViewObject2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000127-0000-0000-C000-000000000046")
    IViewObject2 : public IViewObject
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetExtent( 
            /* [in] */ DWORD dwDrawAspect,
            /* [in] */ LONG lindex,
            /* [unique][in] */ DVTARGETDEVICE *ptd,
            /* [out] */ LPSIZEL lpsizel) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IViewObject2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IViewObject2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IViewObject2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IViewObject2 * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Draw )( 
            IViewObject2 * This,
            /* [in] */ DWORD dwDrawAspect,
            /* [in] */ LONG lindex,
            /* [unique][in] */ void *pvAspect,
            /* [unique][in] */ DVTARGETDEVICE *ptd,
            /* [in] */ HDC hdcTargetDev,
            /* [in] */ HDC hdcDraw,
            /* [in] */ LPCRECTL lprcBounds,
            /* [unique][in] */ LPCRECTL lprcWBounds,
            /* [in] */ BOOL ( STDMETHODCALLTYPE *pfnContinue )( 
                ULONG_PTR dwContinue),
            /* [in] */ ULONG_PTR dwContinue);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *GetColorSet )( 
            IViewObject2 * This,
            /* [in] */ DWORD dwDrawAspect,
            /* [in] */ LONG lindex,
            /* [unique][in] */ void *pvAspect,
            /* [unique][in] */ DVTARGETDEVICE *ptd,
            /* [in] */ HDC hicTargetDev,
            /* [out] */ LOGPALETTE **ppColorSet);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Freeze )( 
            IViewObject2 * This,
            /* [in] */ DWORD dwDrawAspect,
            /* [in] */ LONG lindex,
            /* [unique][in] */ void *pvAspect,
            /* [out] */ DWORD *pdwFreeze);
        
        HRESULT ( STDMETHODCALLTYPE *Unfreeze )( 
            IViewObject2 * This,
            /* [in] */ DWORD dwFreeze);
        
        HRESULT ( STDMETHODCALLTYPE *SetAdvise )( 
            IViewObject2 * This,
            /* [in] */ DWORD aspects,
            /* [in] */ DWORD advf,
            /* [unique][in] */ IAdviseSink *pAdvSink);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *GetAdvise )( 
            IViewObject2 * This,
            /* [unique][out] */ DWORD *pAspects,
            /* [unique][out] */ DWORD *pAdvf,
            /* [out] */ IAdviseSink **ppAdvSink);
        
        HRESULT ( STDMETHODCALLTYPE *GetExtent )( 
            IViewObject2 * This,
            /* [in] */ DWORD dwDrawAspect,
            /* [in] */ LONG lindex,
            /* [unique][in] */ DVTARGETDEVICE *ptd,
            /* [out] */ LPSIZEL lpsizel);
        
        END_INTERFACE
    } IViewObject2Vtbl;

    interface IViewObject2
    {
        CONST_VTBL struct IViewObject2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IViewObject2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IViewObject2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IViewObject2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IViewObject2_Draw(This,dwDrawAspect,lindex,pvAspect,ptd,hdcTargetDev,hdcDraw,lprcBounds,lprcWBounds,pfnContinue,dwContinue)	\
    (This)->lpVtbl -> Draw(This,dwDrawAspect,lindex,pvAspect,ptd,hdcTargetDev,hdcDraw,lprcBounds,lprcWBounds,pfnContinue,dwContinue)

#define IViewObject2_GetColorSet(This,dwDrawAspect,lindex,pvAspect,ptd,hicTargetDev,ppColorSet)	\
    (This)->lpVtbl -> GetColorSet(This,dwDrawAspect,lindex,pvAspect,ptd,hicTargetDev,ppColorSet)

#define IViewObject2_Freeze(This,dwDrawAspect,lindex,pvAspect,pdwFreeze)	\
    (This)->lpVtbl -> Freeze(This,dwDrawAspect,lindex,pvAspect,pdwFreeze)

#define IViewObject2_Unfreeze(This,dwFreeze)	\
    (This)->lpVtbl -> Unfreeze(This,dwFreeze)

#define IViewObject2_SetAdvise(This,aspects,advf,pAdvSink)	\
    (This)->lpVtbl -> SetAdvise(This,aspects,advf,pAdvSink)

#define IViewObject2_GetAdvise(This,pAspects,pAdvf,ppAdvSink)	\
    (This)->lpVtbl -> GetAdvise(This,pAspects,pAdvf,ppAdvSink)


#define IViewObject2_GetExtent(This,dwDrawAspect,lindex,ptd,lpsizel)	\
    (This)->lpVtbl -> GetExtent(This,dwDrawAspect,lindex,ptd,lpsizel)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IViewObject2_GetExtent_Proxy( 
    IViewObject2 * This,
    /* [in] */ DWORD dwDrawAspect,
    /* [in] */ LONG lindex,
    /* [unique][in] */ DVTARGETDEVICE *ptd,
    /* [out] */ LPSIZEL lpsizel);


void __RPC_STUB IViewObject2_GetExtent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IViewObject2_INTERFACE_DEFINED__ */


#ifndef __IDropSource_INTERFACE_DEFINED__
#define __IDropSource_INTERFACE_DEFINED__

/* interface IDropSource */
/* [uuid][object][local] */ 

typedef /* [unique] */ IDropSource *LPDROPSOURCE;


EXTERN_C const IID IID_IDropSource;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000121-0000-0000-C000-000000000046")
    IDropSource : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE QueryContinueDrag( 
            /* [in] */ BOOL fEscapePressed,
            /* [in] */ DWORD grfKeyState) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GiveFeedback( 
            /* [in] */ DWORD dwEffect) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDropSourceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDropSource * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDropSource * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDropSource * This);
        
        HRESULT ( STDMETHODCALLTYPE *QueryContinueDrag )( 
            IDropSource * This,
            /* [in] */ BOOL fEscapePressed,
            /* [in] */ DWORD grfKeyState);
        
        HRESULT ( STDMETHODCALLTYPE *GiveFeedback )( 
            IDropSource * This,
            /* [in] */ DWORD dwEffect);
        
        END_INTERFACE
    } IDropSourceVtbl;

    interface IDropSource
    {
        CONST_VTBL struct IDropSourceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDropSource_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDropSource_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDropSource_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDropSource_QueryContinueDrag(This,fEscapePressed,grfKeyState)	\
    (This)->lpVtbl -> QueryContinueDrag(This,fEscapePressed,grfKeyState)

#define IDropSource_GiveFeedback(This,dwEffect)	\
    (This)->lpVtbl -> GiveFeedback(This,dwEffect)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDropSource_QueryContinueDrag_Proxy( 
    IDropSource * This,
    /* [in] */ BOOL fEscapePressed,
    /* [in] */ DWORD grfKeyState);


void __RPC_STUB IDropSource_QueryContinueDrag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDropSource_GiveFeedback_Proxy( 
    IDropSource * This,
    /* [in] */ DWORD dwEffect);


void __RPC_STUB IDropSource_GiveFeedback_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDropSource_INTERFACE_DEFINED__ */


#ifndef __IDropTarget_INTERFACE_DEFINED__
#define __IDropTarget_INTERFACE_DEFINED__

/* interface IDropTarget */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IDropTarget *LPDROPTARGET;

#define	MK_ALT	( 0x20 )

#define	DROPEFFECT_NONE	( 0 )

#define	DROPEFFECT_COPY	( 1 )

#define	DROPEFFECT_MOVE	( 2 )

#define	DROPEFFECT_LINK	( 4 )

#define	DROPEFFECT_SCROLL	( 0x80000000 )

// default inset-width of the hot zone, in pixels
//   typical use: GetProfileInt("windows","DragScrollInset",DD_DEFSCROLLINSET)
#define	DD_DEFSCROLLINSET	( 11 )

// default delay before scrolling, in milliseconds
//   typical use: GetProfileInt("windows","DragScrollDelay",DD_DEFSCROLLDELAY)
#define	DD_DEFSCROLLDELAY	( 50 )

// default scroll interval, in milliseconds
//   typical use: GetProfileInt("windows","DragScrollInterval", DD_DEFSCROLLINTERVAL)
#define	DD_DEFSCROLLINTERVAL	( 50 )

// default delay before dragging should start, in milliseconds
//   typical use: GetProfileInt("windows", "DragDelay", DD_DEFDRAGDELAY)
#define	DD_DEFDRAGDELAY	( 200 )

// default minimum distance (radius) before dragging should start, in pixels
//   typical use: GetProfileInt("windows", "DragMinDist", DD_DEFDRAGMINDIST)
#define	DD_DEFDRAGMINDIST	( 2 )


EXTERN_C const IID IID_IDropTarget;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000122-0000-0000-C000-000000000046")
    IDropTarget : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE DragEnter( 
            /* [unique][in] */ IDataObject *pDataObj,
            /* [in] */ DWORD grfKeyState,
            /* [in] */ POINTL pt,
            /* [out][in] */ DWORD *pdwEffect) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DragOver( 
            /* [in] */ DWORD grfKeyState,
            /* [in] */ POINTL pt,
            /* [out][in] */ DWORD *pdwEffect) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DragLeave( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Drop( 
            /* [unique][in] */ IDataObject *pDataObj,
            /* [in] */ DWORD grfKeyState,
            /* [in] */ POINTL pt,
            /* [out][in] */ DWORD *pdwEffect) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDropTargetVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDropTarget * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDropTarget * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDropTarget * This);
        
        HRESULT ( STDMETHODCALLTYPE *DragEnter )( 
            IDropTarget * This,
            /* [unique][in] */ IDataObject *pDataObj,
            /* [in] */ DWORD grfKeyState,
            /* [in] */ POINTL pt,
            /* [out][in] */ DWORD *pdwEffect);
        
        HRESULT ( STDMETHODCALLTYPE *DragOver )( 
            IDropTarget * This,
            /* [in] */ DWORD grfKeyState,
            /* [in] */ POINTL pt,
            /* [out][in] */ DWORD *pdwEffect);
        
        HRESULT ( STDMETHODCALLTYPE *DragLeave )( 
            IDropTarget * This);
        
        HRESULT ( STDMETHODCALLTYPE *Drop )( 
            IDropTarget * This,
            /* [unique][in] */ IDataObject *pDataObj,
            /* [in] */ DWORD grfKeyState,
            /* [in] */ POINTL pt,
            /* [out][in] */ DWORD *pdwEffect);
        
        END_INTERFACE
    } IDropTargetVtbl;

    interface IDropTarget
    {
        CONST_VTBL struct IDropTargetVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDropTarget_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDropTarget_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDropTarget_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDropTarget_DragEnter(This,pDataObj,grfKeyState,pt,pdwEffect)	\
    (This)->lpVtbl -> DragEnter(This,pDataObj,grfKeyState,pt,pdwEffect)

#define IDropTarget_DragOver(This,grfKeyState,pt,pdwEffect)	\
    (This)->lpVtbl -> DragOver(This,grfKeyState,pt,pdwEffect)

#define IDropTarget_DragLeave(This)	\
    (This)->lpVtbl -> DragLeave(This)

#define IDropTarget_Drop(This,pDataObj,grfKeyState,pt,pdwEffect)	\
    (This)->lpVtbl -> Drop(This,pDataObj,grfKeyState,pt,pdwEffect)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDropTarget_DragEnter_Proxy( 
    IDropTarget * This,
    /* [unique][in] */ IDataObject *pDataObj,
    /* [in] */ DWORD grfKeyState,
    /* [in] */ POINTL pt,
    /* [out][in] */ DWORD *pdwEffect);


void __RPC_STUB IDropTarget_DragEnter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDropTarget_DragOver_Proxy( 
    IDropTarget * This,
    /* [in] */ DWORD grfKeyState,
    /* [in] */ POINTL pt,
    /* [out][in] */ DWORD *pdwEffect);


void __RPC_STUB IDropTarget_DragOver_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDropTarget_DragLeave_Proxy( 
    IDropTarget * This);


void __RPC_STUB IDropTarget_DragLeave_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDropTarget_Drop_Proxy( 
    IDropTarget * This,
    /* [unique][in] */ IDataObject *pDataObj,
    /* [in] */ DWORD grfKeyState,
    /* [in] */ POINTL pt,
    /* [out][in] */ DWORD *pdwEffect);


void __RPC_STUB IDropTarget_Drop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDropTarget_INTERFACE_DEFINED__ */


#ifndef __IEnumOLEVERB_INTERFACE_DEFINED__
#define __IEnumOLEVERB_INTERFACE_DEFINED__

/* interface IEnumOLEVERB */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IEnumOLEVERB *LPENUMOLEVERB;

typedef struct tagOLEVERB
    {
    LONG lVerb;
    LPOLESTR lpszVerbName;
    DWORD fuFlags;
    DWORD grfAttribs;
    } 	OLEVERB;

typedef struct tagOLEVERB *LPOLEVERB;

typedef /* [v1_enum] */ 
enum tagOLEVERBATTRIB
    {	OLEVERBATTRIB_NEVERDIRTIES	= 1,
	OLEVERBATTRIB_ONCONTAINERMENU	= 2
    } 	OLEVERBATTRIB;


EXTERN_C const IID IID_IEnumOLEVERB;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00000104-0000-0000-C000-000000000046")
    IEnumOLEVERB : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ LPOLEVERB rgelt,
            /* [out] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumOLEVERB **ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumOLEVERBVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumOLEVERB * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumOLEVERB * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumOLEVERB * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumOLEVERB * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ LPOLEVERB rgelt,
            /* [out] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumOLEVERB * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumOLEVERB * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumOLEVERB * This,
            /* [out] */ IEnumOLEVERB **ppenum);
        
        END_INTERFACE
    } IEnumOLEVERBVtbl;

    interface IEnumOLEVERB
    {
        CONST_VTBL struct IEnumOLEVERBVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumOLEVERB_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumOLEVERB_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumOLEVERB_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumOLEVERB_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumOLEVERB_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumOLEVERB_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumOLEVERB_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumOLEVERB_RemoteNext_Proxy( 
    IEnumOLEVERB * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ LPOLEVERB rgelt,
    /* [out] */ ULONG *pceltFetched);


void __RPC_STUB IEnumOLEVERB_RemoteNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumOLEVERB_Skip_Proxy( 
    IEnumOLEVERB * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumOLEVERB_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumOLEVERB_Reset_Proxy( 
    IEnumOLEVERB * This);


void __RPC_STUB IEnumOLEVERB_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumOLEVERB_Clone_Proxy( 
    IEnumOLEVERB * This,
    /* [out] */ IEnumOLEVERB **ppenum);


void __RPC_STUB IEnumOLEVERB_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumOLEVERB_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  CLIPFORMAT_UserSize(     unsigned long *, unsigned long            , CLIPFORMAT * ); 
unsigned char * __RPC_USER  CLIPFORMAT_UserMarshal(  unsigned long *, unsigned char *, CLIPFORMAT * ); 
unsigned char * __RPC_USER  CLIPFORMAT_UserUnmarshal(unsigned long *, unsigned char *, CLIPFORMAT * ); 
void                      __RPC_USER  CLIPFORMAT_UserFree(     unsigned long *, CLIPFORMAT * ); 

unsigned long             __RPC_USER  HACCEL_UserSize(     unsigned long *, unsigned long            , HACCEL * ); 
unsigned char * __RPC_USER  HACCEL_UserMarshal(  unsigned long *, unsigned char *, HACCEL * ); 
unsigned char * __RPC_USER  HACCEL_UserUnmarshal(unsigned long *, unsigned char *, HACCEL * ); 
void                      __RPC_USER  HACCEL_UserFree(     unsigned long *, HACCEL * ); 

unsigned long             __RPC_USER  HGLOBAL_UserSize(     unsigned long *, unsigned long            , HGLOBAL * ); 
unsigned char * __RPC_USER  HGLOBAL_UserMarshal(  unsigned long *, unsigned char *, HGLOBAL * ); 
unsigned char * __RPC_USER  HGLOBAL_UserUnmarshal(unsigned long *, unsigned char *, HGLOBAL * ); 
void                      __RPC_USER  HGLOBAL_UserFree(     unsigned long *, HGLOBAL * ); 

unsigned long             __RPC_USER  HMENU_UserSize(     unsigned long *, unsigned long            , HMENU * ); 
unsigned char * __RPC_USER  HMENU_UserMarshal(  unsigned long *, unsigned char *, HMENU * ); 
unsigned char * __RPC_USER  HMENU_UserUnmarshal(unsigned long *, unsigned char *, HMENU * ); 
void                      __RPC_USER  HMENU_UserFree(     unsigned long *, HMENU * ); 

unsigned long             __RPC_USER  HWND_UserSize(     unsigned long *, unsigned long            , HWND * ); 
unsigned char * __RPC_USER  HWND_UserMarshal(  unsigned long *, unsigned char *, HWND * ); 
unsigned char * __RPC_USER  HWND_UserUnmarshal(unsigned long *, unsigned char *, HWND * ); 
void                      __RPC_USER  HWND_UserFree(     unsigned long *, HWND * ); 

unsigned long             __RPC_USER  STGMEDIUM_UserSize(     unsigned long *, unsigned long            , STGMEDIUM * ); 
unsigned char * __RPC_USER  STGMEDIUM_UserMarshal(  unsigned long *, unsigned char *, STGMEDIUM * ); 
unsigned char * __RPC_USER  STGMEDIUM_UserUnmarshal(unsigned long *, unsigned char *, STGMEDIUM * ); 
void                      __RPC_USER  STGMEDIUM_UserFree(     unsigned long *, STGMEDIUM * ); 

/* [local] */ HRESULT STDMETHODCALLTYPE IOleCache2_UpdateCache_Proxy( 
    IOleCache2 * This,
    /* [in] */ LPDATAOBJECT pDataObject,
    /* [in] */ DWORD grfUpdf,
    /* [in] */ LPVOID pReserved);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IOleCache2_UpdateCache_Stub( 
    IOleCache2 * This,
    /* [in] */ LPDATAOBJECT pDataObject,
    /* [in] */ DWORD grfUpdf,
    /* [in] */ LONG_PTR pReserved);

/* [local] */ HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_TranslateAccelerator_Proxy( 
    IOleInPlaceActiveObject * This,
    /* [in] */ LPMSG lpmsg);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_TranslateAccelerator_Stub( 
    IOleInPlaceActiveObject * This);

/* [local] */ HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_ResizeBorder_Proxy( 
    IOleInPlaceActiveObject * This,
    /* [in] */ LPCRECT prcBorder,
    /* [unique][in] */ IOleInPlaceUIWindow *pUIWindow,
    /* [in] */ BOOL fFrameWindow);


/* [input_sync][call_as] */ HRESULT STDMETHODCALLTYPE IOleInPlaceActiveObject_ResizeBorder_Stub( 
    IOleInPlaceActiveObject * This,
    /* [in] */ LPCRECT prcBorder,
    /* [in] */ REFIID riid,
    /* [iid_is][unique][in] */ IOleInPlaceUIWindow *pUIWindow,
    /* [in] */ BOOL fFrameWindow);

/* [local] */ HRESULT STDMETHODCALLTYPE IViewObject_Draw_Proxy( 
    IViewObject * This,
    /* [in] */ DWORD dwDrawAspect,
    /* [in] */ LONG lindex,
    /* [unique][in] */ void *pvAspect,
    /* [unique][in] */ DVTARGETDEVICE *ptd,
    /* [in] */ HDC hdcTargetDev,
    /* [in] */ HDC hdcDraw,
    /* [in] */ LPCRECTL lprcBounds,
    /* [unique][in] */ LPCRECTL lprcWBounds,
    /* [in] */ BOOL ( STDMETHODCALLTYPE *pfnContinue )( 
        ULONG_PTR dwContinue),
    /* [in] */ ULONG_PTR dwContinue);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IViewObject_Draw_Stub( 
    IViewObject * This,
    /* [in] */ DWORD dwDrawAspect,
    /* [in] */ LONG lindex,
    /* [in] */ ULONG_PTR pvAspect,
    /* [unique][in] */ DVTARGETDEVICE *ptd,
    /* [in] */ ULONG_PTR hdcTargetDev,
    /* [in] */ ULONG_PTR hdcDraw,
    /* [in] */ LPCRECTL lprcBounds,
    /* [unique][in] */ LPCRECTL lprcWBounds,
    /* [in] */ IContinue *pContinue);

/* [local] */ HRESULT STDMETHODCALLTYPE IViewObject_GetColorSet_Proxy( 
    IViewObject * This,
    /* [in] */ DWORD dwDrawAspect,
    /* [in] */ LONG lindex,
    /* [unique][in] */ void *pvAspect,
    /* [unique][in] */ DVTARGETDEVICE *ptd,
    /* [in] */ HDC hicTargetDev,
    /* [out] */ LOGPALETTE **ppColorSet);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IViewObject_GetColorSet_Stub( 
    IViewObject * This,
    /* [in] */ DWORD dwDrawAspect,
    /* [in] */ LONG lindex,
    /* [in] */ ULONG_PTR pvAspect,
    /* [unique][in] */ DVTARGETDEVICE *ptd,
    /* [in] */ ULONG_PTR hicTargetDev,
    /* [out] */ LOGPALETTE **ppColorSet);

/* [local] */ HRESULT STDMETHODCALLTYPE IViewObject_Freeze_Proxy( 
    IViewObject * This,
    /* [in] */ DWORD dwDrawAspect,
    /* [in] */ LONG lindex,
    /* [unique][in] */ void *pvAspect,
    /* [out] */ DWORD *pdwFreeze);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IViewObject_Freeze_Stub( 
    IViewObject * This,
    /* [in] */ DWORD dwDrawAspect,
    /* [in] */ LONG lindex,
    /* [in] */ ULONG_PTR pvAspect,
    /* [out] */ DWORD *pdwFreeze);

/* [local] */ HRESULT STDMETHODCALLTYPE IViewObject_GetAdvise_Proxy( 
    IViewObject * This,
    /* [unique][out] */ DWORD *pAspects,
    /* [unique][out] */ DWORD *pAdvf,
    /* [out] */ IAdviseSink **ppAdvSink);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IViewObject_GetAdvise_Stub( 
    IViewObject * This,
    /* [out] */ DWORD *pAspects,
    /* [out] */ DWORD *pAdvf,
    /* [out] */ IAdviseSink **ppAdvSink);

/* [local] */ HRESULT STDMETHODCALLTYPE IEnumOLEVERB_Next_Proxy( 
    IEnumOLEVERB * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ LPOLEVERB rgelt,
    /* [out] */ ULONG *pceltFetched);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IEnumOLEVERB_Next_Stub( 
    IEnumOLEVERB * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ LPOLEVERB rgelt,
    /* [out] */ ULONG *pceltFetched);



/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



