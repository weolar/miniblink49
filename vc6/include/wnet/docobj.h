

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* Compiler settings for docobj.idl:
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

#ifndef __docobj_h__
#define __docobj_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IOleDocument_FWD_DEFINED__
#define __IOleDocument_FWD_DEFINED__
typedef interface IOleDocument IOleDocument;
#endif 	/* __IOleDocument_FWD_DEFINED__ */


#ifndef __IOleDocumentSite_FWD_DEFINED__
#define __IOleDocumentSite_FWD_DEFINED__
typedef interface IOleDocumentSite IOleDocumentSite;
#endif 	/* __IOleDocumentSite_FWD_DEFINED__ */


#ifndef __IOleDocumentView_FWD_DEFINED__
#define __IOleDocumentView_FWD_DEFINED__
typedef interface IOleDocumentView IOleDocumentView;
#endif 	/* __IOleDocumentView_FWD_DEFINED__ */


#ifndef __IEnumOleDocumentViews_FWD_DEFINED__
#define __IEnumOleDocumentViews_FWD_DEFINED__
typedef interface IEnumOleDocumentViews IEnumOleDocumentViews;
#endif 	/* __IEnumOleDocumentViews_FWD_DEFINED__ */


#ifndef __IContinueCallback_FWD_DEFINED__
#define __IContinueCallback_FWD_DEFINED__
typedef interface IContinueCallback IContinueCallback;
#endif 	/* __IContinueCallback_FWD_DEFINED__ */


#ifndef __IPrint_FWD_DEFINED__
#define __IPrint_FWD_DEFINED__
typedef interface IPrint IPrint;
#endif 	/* __IPrint_FWD_DEFINED__ */


#ifndef __IOleCommandTarget_FWD_DEFINED__
#define __IOleCommandTarget_FWD_DEFINED__
typedef interface IOleCommandTarget IOleCommandTarget;
#endif 	/* __IOleCommandTarget_FWD_DEFINED__ */


/* header files for imported files */
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_docobj_0000 */
/* [local] */ 

//=--------------------------------------------------------------------------=
// DocObj.h
//=--------------------------------------------------------------------------=
// (C) Copyright 1995-1998 Microsoft Corporation.  All Rights Reserved.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//=--------------------------------------------------------------------------=

#pragma comment(lib,"uuid.lib")

//--------------------------------------------------------------------------
// OLE Document Object Interfaces.








////////////////////////////////////////////////////////////////////////////
//  Interface Definitions
#ifndef _LPOLEDOCUMENT_DEFINED
#define _LPOLEDOCUMENT_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_docobj_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_docobj_0000_v0_0_s_ifspec;

#ifndef __IOleDocument_INTERFACE_DEFINED__
#define __IOleDocument_INTERFACE_DEFINED__

/* interface IOleDocument */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IOleDocument *LPOLEDOCUMENT;

typedef /* [public] */ 
enum __MIDL_IOleDocument_0001
    {	DOCMISC_CANCREATEMULTIPLEVIEWS	= 1,
	DOCMISC_SUPPORTCOMPLEXRECTANGLES	= 2,
	DOCMISC_CANTOPENEDIT	= 4,
	DOCMISC_NOFILESUPPORT	= 8
    } 	DOCMISC;


EXTERN_C const IID IID_IOleDocument;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("b722bcc5-4e68-101b-a2bc-00aa00404770")
    IOleDocument : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateView( 
            /* [unique][in] */ IOleInPlaceSite *pIPSite,
            /* [unique][in] */ IStream *pstm,
            /* [in] */ DWORD dwReserved,
            /* [out] */ IOleDocumentView **ppView) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDocMiscStatus( 
            /* [out] */ DWORD *pdwStatus) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumViews( 
            /* [out] */ IEnumOleDocumentViews **ppEnum,
            /* [out] */ IOleDocumentView **ppView) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOleDocumentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleDocument * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleDocument * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleDocument * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateView )( 
            IOleDocument * This,
            /* [unique][in] */ IOleInPlaceSite *pIPSite,
            /* [unique][in] */ IStream *pstm,
            /* [in] */ DWORD dwReserved,
            /* [out] */ IOleDocumentView **ppView);
        
        HRESULT ( STDMETHODCALLTYPE *GetDocMiscStatus )( 
            IOleDocument * This,
            /* [out] */ DWORD *pdwStatus);
        
        HRESULT ( STDMETHODCALLTYPE *EnumViews )( 
            IOleDocument * This,
            /* [out] */ IEnumOleDocumentViews **ppEnum,
            /* [out] */ IOleDocumentView **ppView);
        
        END_INTERFACE
    } IOleDocumentVtbl;

    interface IOleDocument
    {
        CONST_VTBL struct IOleDocumentVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleDocument_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOleDocument_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOleDocument_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOleDocument_CreateView(This,pIPSite,pstm,dwReserved,ppView)	\
    (This)->lpVtbl -> CreateView(This,pIPSite,pstm,dwReserved,ppView)

#define IOleDocument_GetDocMiscStatus(This,pdwStatus)	\
    (This)->lpVtbl -> GetDocMiscStatus(This,pdwStatus)

#define IOleDocument_EnumViews(This,ppEnum,ppView)	\
    (This)->lpVtbl -> EnumViews(This,ppEnum,ppView)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOleDocument_CreateView_Proxy( 
    IOleDocument * This,
    /* [unique][in] */ IOleInPlaceSite *pIPSite,
    /* [unique][in] */ IStream *pstm,
    /* [in] */ DWORD dwReserved,
    /* [out] */ IOleDocumentView **ppView);


void __RPC_STUB IOleDocument_CreateView_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleDocument_GetDocMiscStatus_Proxy( 
    IOleDocument * This,
    /* [out] */ DWORD *pdwStatus);


void __RPC_STUB IOleDocument_GetDocMiscStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleDocument_EnumViews_Proxy( 
    IOleDocument * This,
    /* [out] */ IEnumOleDocumentViews **ppEnum,
    /* [out] */ IOleDocumentView **ppView);


void __RPC_STUB IOleDocument_EnumViews_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOleDocument_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_docobj_0265 */
/* [local] */ 

#endif
#ifndef _LPOLEDOCUMENTSITE_DEFINED
#define _LPOLEDOCUMENTSITE_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_docobj_0265_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_docobj_0265_v0_0_s_ifspec;

#ifndef __IOleDocumentSite_INTERFACE_DEFINED__
#define __IOleDocumentSite_INTERFACE_DEFINED__

/* interface IOleDocumentSite */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IOleDocumentSite *LPOLEDOCUMENTSITE;


EXTERN_C const IID IID_IOleDocumentSite;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("b722bcc7-4e68-101b-a2bc-00aa00404770")
    IOleDocumentSite : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ActivateMe( 
            /* [in] */ IOleDocumentView *pViewToActivate) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOleDocumentSiteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleDocumentSite * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleDocumentSite * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleDocumentSite * This);
        
        HRESULT ( STDMETHODCALLTYPE *ActivateMe )( 
            IOleDocumentSite * This,
            /* [in] */ IOleDocumentView *pViewToActivate);
        
        END_INTERFACE
    } IOleDocumentSiteVtbl;

    interface IOleDocumentSite
    {
        CONST_VTBL struct IOleDocumentSiteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleDocumentSite_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOleDocumentSite_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOleDocumentSite_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOleDocumentSite_ActivateMe(This,pViewToActivate)	\
    (This)->lpVtbl -> ActivateMe(This,pViewToActivate)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOleDocumentSite_ActivateMe_Proxy( 
    IOleDocumentSite * This,
    /* [in] */ IOleDocumentView *pViewToActivate);


void __RPC_STUB IOleDocumentSite_ActivateMe_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOleDocumentSite_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_docobj_0266 */
/* [local] */ 

#endif
#ifndef _LPOLEDOCUMENTVIEW_DEFINED
#define _LPOLEDOCUMENTVIEW_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_docobj_0266_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_docobj_0266_v0_0_s_ifspec;

#ifndef __IOleDocumentView_INTERFACE_DEFINED__
#define __IOleDocumentView_INTERFACE_DEFINED__

/* interface IOleDocumentView */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IOleDocumentView *LPOLEDOCUMENTVIEW;


EXTERN_C const IID IID_IOleDocumentView;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("b722bcc6-4e68-101b-a2bc-00aa00404770")
    IOleDocumentView : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetInPlaceSite( 
            /* [unique][in] */ IOleInPlaceSite *pIPSite) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetInPlaceSite( 
            /* [out] */ IOleInPlaceSite **ppIPSite) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDocument( 
            /* [out] */ IUnknown **ppunk) = 0;
        
        virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE SetRect( 
            /* [in] */ LPRECT prcView) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetRect( 
            /* [out] */ LPRECT prcView) = 0;
        
        virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE SetRectComplex( 
            /* [unique][in] */ LPRECT prcView,
            /* [unique][in] */ LPRECT prcHScroll,
            /* [unique][in] */ LPRECT prcVScroll,
            /* [unique][in] */ LPRECT prcSizeBox) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Show( 
            /* [in] */ BOOL fShow) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UIActivate( 
            /* [in] */ BOOL fUIActivate) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Open( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CloseView( 
            DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SaveViewState( 
            /* [in] */ LPSTREAM pstm) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ApplyViewState( 
            /* [in] */ LPSTREAM pstm) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [in] */ IOleInPlaceSite *pIPSiteNew,
            /* [out] */ IOleDocumentView **ppViewNew) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOleDocumentViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleDocumentView * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleDocumentView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleDocumentView * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetInPlaceSite )( 
            IOleDocumentView * This,
            /* [unique][in] */ IOleInPlaceSite *pIPSite);
        
        HRESULT ( STDMETHODCALLTYPE *GetInPlaceSite )( 
            IOleDocumentView * This,
            /* [out] */ IOleInPlaceSite **ppIPSite);
        
        HRESULT ( STDMETHODCALLTYPE *GetDocument )( 
            IOleDocumentView * This,
            /* [out] */ IUnknown **ppunk);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *SetRect )( 
            IOleDocumentView * This,
            /* [in] */ LPRECT prcView);
        
        HRESULT ( STDMETHODCALLTYPE *GetRect )( 
            IOleDocumentView * This,
            /* [out] */ LPRECT prcView);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *SetRectComplex )( 
            IOleDocumentView * This,
            /* [unique][in] */ LPRECT prcView,
            /* [unique][in] */ LPRECT prcHScroll,
            /* [unique][in] */ LPRECT prcVScroll,
            /* [unique][in] */ LPRECT prcSizeBox);
        
        HRESULT ( STDMETHODCALLTYPE *Show )( 
            IOleDocumentView * This,
            /* [in] */ BOOL fShow);
        
        HRESULT ( STDMETHODCALLTYPE *UIActivate )( 
            IOleDocumentView * This,
            /* [in] */ BOOL fUIActivate);
        
        HRESULT ( STDMETHODCALLTYPE *Open )( 
            IOleDocumentView * This);
        
        HRESULT ( STDMETHODCALLTYPE *CloseView )( 
            IOleDocumentView * This,
            DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *SaveViewState )( 
            IOleDocumentView * This,
            /* [in] */ LPSTREAM pstm);
        
        HRESULT ( STDMETHODCALLTYPE *ApplyViewState )( 
            IOleDocumentView * This,
            /* [in] */ LPSTREAM pstm);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IOleDocumentView * This,
            /* [in] */ IOleInPlaceSite *pIPSiteNew,
            /* [out] */ IOleDocumentView **ppViewNew);
        
        END_INTERFACE
    } IOleDocumentViewVtbl;

    interface IOleDocumentView
    {
        CONST_VTBL struct IOleDocumentViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleDocumentView_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOleDocumentView_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOleDocumentView_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOleDocumentView_SetInPlaceSite(This,pIPSite)	\
    (This)->lpVtbl -> SetInPlaceSite(This,pIPSite)

#define IOleDocumentView_GetInPlaceSite(This,ppIPSite)	\
    (This)->lpVtbl -> GetInPlaceSite(This,ppIPSite)

#define IOleDocumentView_GetDocument(This,ppunk)	\
    (This)->lpVtbl -> GetDocument(This,ppunk)

#define IOleDocumentView_SetRect(This,prcView)	\
    (This)->lpVtbl -> SetRect(This,prcView)

#define IOleDocumentView_GetRect(This,prcView)	\
    (This)->lpVtbl -> GetRect(This,prcView)

#define IOleDocumentView_SetRectComplex(This,prcView,prcHScroll,prcVScroll,prcSizeBox)	\
    (This)->lpVtbl -> SetRectComplex(This,prcView,prcHScroll,prcVScroll,prcSizeBox)

#define IOleDocumentView_Show(This,fShow)	\
    (This)->lpVtbl -> Show(This,fShow)

#define IOleDocumentView_UIActivate(This,fUIActivate)	\
    (This)->lpVtbl -> UIActivate(This,fUIActivate)

#define IOleDocumentView_Open(This)	\
    (This)->lpVtbl -> Open(This)

#define IOleDocumentView_CloseView(This,dwReserved)	\
    (This)->lpVtbl -> CloseView(This,dwReserved)

#define IOleDocumentView_SaveViewState(This,pstm)	\
    (This)->lpVtbl -> SaveViewState(This,pstm)

#define IOleDocumentView_ApplyViewState(This,pstm)	\
    (This)->lpVtbl -> ApplyViewState(This,pstm)

#define IOleDocumentView_Clone(This,pIPSiteNew,ppViewNew)	\
    (This)->lpVtbl -> Clone(This,pIPSiteNew,ppViewNew)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOleDocumentView_SetInPlaceSite_Proxy( 
    IOleDocumentView * This,
    /* [unique][in] */ IOleInPlaceSite *pIPSite);


void __RPC_STUB IOleDocumentView_SetInPlaceSite_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleDocumentView_GetInPlaceSite_Proxy( 
    IOleDocumentView * This,
    /* [out] */ IOleInPlaceSite **ppIPSite);


void __RPC_STUB IOleDocumentView_GetInPlaceSite_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleDocumentView_GetDocument_Proxy( 
    IOleDocumentView * This,
    /* [out] */ IUnknown **ppunk);


void __RPC_STUB IOleDocumentView_GetDocument_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [input_sync] */ HRESULT STDMETHODCALLTYPE IOleDocumentView_SetRect_Proxy( 
    IOleDocumentView * This,
    /* [in] */ LPRECT prcView);


void __RPC_STUB IOleDocumentView_SetRect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleDocumentView_GetRect_Proxy( 
    IOleDocumentView * This,
    /* [out] */ LPRECT prcView);


void __RPC_STUB IOleDocumentView_GetRect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [input_sync] */ HRESULT STDMETHODCALLTYPE IOleDocumentView_SetRectComplex_Proxy( 
    IOleDocumentView * This,
    /* [unique][in] */ LPRECT prcView,
    /* [unique][in] */ LPRECT prcHScroll,
    /* [unique][in] */ LPRECT prcVScroll,
    /* [unique][in] */ LPRECT prcSizeBox);


void __RPC_STUB IOleDocumentView_SetRectComplex_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleDocumentView_Show_Proxy( 
    IOleDocumentView * This,
    /* [in] */ BOOL fShow);


void __RPC_STUB IOleDocumentView_Show_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleDocumentView_UIActivate_Proxy( 
    IOleDocumentView * This,
    /* [in] */ BOOL fUIActivate);


void __RPC_STUB IOleDocumentView_UIActivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleDocumentView_Open_Proxy( 
    IOleDocumentView * This);


void __RPC_STUB IOleDocumentView_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleDocumentView_CloseView_Proxy( 
    IOleDocumentView * This,
    DWORD dwReserved);


void __RPC_STUB IOleDocumentView_CloseView_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleDocumentView_SaveViewState_Proxy( 
    IOleDocumentView * This,
    /* [in] */ LPSTREAM pstm);


void __RPC_STUB IOleDocumentView_SaveViewState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleDocumentView_ApplyViewState_Proxy( 
    IOleDocumentView * This,
    /* [in] */ LPSTREAM pstm);


void __RPC_STUB IOleDocumentView_ApplyViewState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleDocumentView_Clone_Proxy( 
    IOleDocumentView * This,
    /* [in] */ IOleInPlaceSite *pIPSiteNew,
    /* [out] */ IOleDocumentView **ppViewNew);


void __RPC_STUB IOleDocumentView_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOleDocumentView_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_docobj_0267 */
/* [local] */ 

#endif
#ifndef _LPENUMOLEDOCUMENTVIEWS_DEFINED
#define _LPENUMOLEDOCUMENTVIEWS_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_docobj_0267_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_docobj_0267_v0_0_s_ifspec;

#ifndef __IEnumOleDocumentViews_INTERFACE_DEFINED__
#define __IEnumOleDocumentViews_INTERFACE_DEFINED__

/* interface IEnumOleDocumentViews */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IEnumOleDocumentViews *LPENUMOLEDOCUMENTVIEWS;


EXTERN_C const IID IID_IEnumOleDocumentViews;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("b722bcc8-4e68-101b-a2bc-00aa00404770")
    IEnumOleDocumentViews : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT __stdcall Next( 
            /* [in] */ ULONG cViews,
            /* [out] */ IOleDocumentView **rgpView,
            /* [out] */ ULONG *pcFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG cViews) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumOleDocumentViews **ppEnum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumOleDocumentViewsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumOleDocumentViews * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumOleDocumentViews * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumOleDocumentViews * This);
        
        /* [local] */ HRESULT ( __stdcall *Next )( 
            IEnumOleDocumentViews * This,
            /* [in] */ ULONG cViews,
            /* [out] */ IOleDocumentView **rgpView,
            /* [out] */ ULONG *pcFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumOleDocumentViews * This,
            /* [in] */ ULONG cViews);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumOleDocumentViews * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumOleDocumentViews * This,
            /* [out] */ IEnumOleDocumentViews **ppEnum);
        
        END_INTERFACE
    } IEnumOleDocumentViewsVtbl;

    interface IEnumOleDocumentViews
    {
        CONST_VTBL struct IEnumOleDocumentViewsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumOleDocumentViews_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumOleDocumentViews_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumOleDocumentViews_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumOleDocumentViews_Next(This,cViews,rgpView,pcFetched)	\
    (This)->lpVtbl -> Next(This,cViews,rgpView,pcFetched)

#define IEnumOleDocumentViews_Skip(This,cViews)	\
    (This)->lpVtbl -> Skip(This,cViews)

#define IEnumOleDocumentViews_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumOleDocumentViews_Clone(This,ppEnum)	\
    (This)->lpVtbl -> Clone(This,ppEnum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT __stdcall IEnumOleDocumentViews_RemoteNext_Proxy( 
    IEnumOleDocumentViews * This,
    /* [in] */ ULONG cViews,
    /* [length_is][size_is][out] */ IOleDocumentView **rgpView,
    /* [out] */ ULONG *pcFetched);


void __RPC_STUB IEnumOleDocumentViews_RemoteNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumOleDocumentViews_Skip_Proxy( 
    IEnumOleDocumentViews * This,
    /* [in] */ ULONG cViews);


void __RPC_STUB IEnumOleDocumentViews_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumOleDocumentViews_Reset_Proxy( 
    IEnumOleDocumentViews * This);


void __RPC_STUB IEnumOleDocumentViews_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumOleDocumentViews_Clone_Proxy( 
    IEnumOleDocumentViews * This,
    /* [out] */ IEnumOleDocumentViews **ppEnum);


void __RPC_STUB IEnumOleDocumentViews_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumOleDocumentViews_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_docobj_0268 */
/* [local] */ 

#endif
#ifndef _LPCONTINUECALLBACK_DEFINED
#define _LPCONTINUECALLBACK_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_docobj_0268_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_docobj_0268_v0_0_s_ifspec;

#ifndef __IContinueCallback_INTERFACE_DEFINED__
#define __IContinueCallback_INTERFACE_DEFINED__

/* interface IContinueCallback */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IContinueCallback *LPCONTINUECALLBACK;


EXTERN_C const IID IID_IContinueCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("b722bcca-4e68-101b-a2bc-00aa00404770")
    IContinueCallback : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE FContinue( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FContinuePrinting( 
            /* [in] */ LONG nCntPrinted,
            /* [in] */ LONG nCurPage,
            /* [unique][in] */ wchar_t *pwszPrintStatus) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IContinueCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IContinueCallback * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IContinueCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IContinueCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *FContinue )( 
            IContinueCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *FContinuePrinting )( 
            IContinueCallback * This,
            /* [in] */ LONG nCntPrinted,
            /* [in] */ LONG nCurPage,
            /* [unique][in] */ wchar_t *pwszPrintStatus);
        
        END_INTERFACE
    } IContinueCallbackVtbl;

    interface IContinueCallback
    {
        CONST_VTBL struct IContinueCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IContinueCallback_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IContinueCallback_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IContinueCallback_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IContinueCallback_FContinue(This)	\
    (This)->lpVtbl -> FContinue(This)

#define IContinueCallback_FContinuePrinting(This,nCntPrinted,nCurPage,pwszPrintStatus)	\
    (This)->lpVtbl -> FContinuePrinting(This,nCntPrinted,nCurPage,pwszPrintStatus)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IContinueCallback_FContinue_Proxy( 
    IContinueCallback * This);


void __RPC_STUB IContinueCallback_FContinue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IContinueCallback_FContinuePrinting_Proxy( 
    IContinueCallback * This,
    /* [in] */ LONG nCntPrinted,
    /* [in] */ LONG nCurPage,
    /* [unique][in] */ wchar_t *pwszPrintStatus);


void __RPC_STUB IContinueCallback_FContinuePrinting_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IContinueCallback_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_docobj_0269 */
/* [local] */ 

#endif
#ifndef _LPPRINT_DEFINED
#define _LPPRINT_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_docobj_0269_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_docobj_0269_v0_0_s_ifspec;

#ifndef __IPrint_INTERFACE_DEFINED__
#define __IPrint_INTERFACE_DEFINED__

/* interface IPrint */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IPrint *LPPRINT;

typedef /* [public] */ 
enum __MIDL_IPrint_0001
    {	PRINTFLAG_MAYBOTHERUSER	= 1,
	PRINTFLAG_PROMPTUSER	= 2,
	PRINTFLAG_USERMAYCHANGEPRINTER	= 4,
	PRINTFLAG_RECOMPOSETODEVICE	= 8,
	PRINTFLAG_DONTACTUALLYPRINT	= 16,
	PRINTFLAG_FORCEPROPERTIES	= 32,
	PRINTFLAG_PRINTTOFILE	= 64
    } 	PRINTFLAG;

typedef struct tagPAGERANGE
    {
    LONG nFromPage;
    LONG nToPage;
    } 	PAGERANGE;

typedef struct tagPAGESET
    {
    ULONG cbStruct;
    BOOL fOddPages;
    BOOL fEvenPages;
    ULONG cPageRange;
    /* [size_is] */ PAGERANGE rgPages[ 1 ];
    } 	PAGESET;

#define PAGESET_TOLASTPAGE   ((WORD)(-1L))

EXTERN_C const IID IID_IPrint;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("b722bcc9-4e68-101b-a2bc-00aa00404770")
    IPrint : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetInitialPageNum( 
            /* [in] */ LONG nFirstPage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPageInfo( 
            /* [out] */ LONG *pnFirstPage,
            /* [out] */ LONG *pcPages) = 0;
        
        virtual /* [local] */ HRESULT __stdcall Print( 
            /* [in] */ DWORD grfFlags,
            /* [out][in] */ DVTARGETDEVICE **pptd,
            /* [out][in] */ PAGESET **ppPageSet,
            /* [unique][out][in] */ STGMEDIUM *pstgmOptions,
            /* [in] */ IContinueCallback *pcallback,
            /* [in] */ LONG nFirstPage,
            /* [out] */ LONG *pcPagesPrinted,
            /* [out] */ LONG *pnLastPage) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPrintVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPrint * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPrint * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPrint * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetInitialPageNum )( 
            IPrint * This,
            /* [in] */ LONG nFirstPage);
        
        HRESULT ( STDMETHODCALLTYPE *GetPageInfo )( 
            IPrint * This,
            /* [out] */ LONG *pnFirstPage,
            /* [out] */ LONG *pcPages);
        
        /* [local] */ HRESULT ( __stdcall *Print )( 
            IPrint * This,
            /* [in] */ DWORD grfFlags,
            /* [out][in] */ DVTARGETDEVICE **pptd,
            /* [out][in] */ PAGESET **ppPageSet,
            /* [unique][out][in] */ STGMEDIUM *pstgmOptions,
            /* [in] */ IContinueCallback *pcallback,
            /* [in] */ LONG nFirstPage,
            /* [out] */ LONG *pcPagesPrinted,
            /* [out] */ LONG *pnLastPage);
        
        END_INTERFACE
    } IPrintVtbl;

    interface IPrint
    {
        CONST_VTBL struct IPrintVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPrint_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPrint_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPrint_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPrint_SetInitialPageNum(This,nFirstPage)	\
    (This)->lpVtbl -> SetInitialPageNum(This,nFirstPage)

#define IPrint_GetPageInfo(This,pnFirstPage,pcPages)	\
    (This)->lpVtbl -> GetPageInfo(This,pnFirstPage,pcPages)

#define IPrint_Print(This,grfFlags,pptd,ppPageSet,pstgmOptions,pcallback,nFirstPage,pcPagesPrinted,pnLastPage)	\
    (This)->lpVtbl -> Print(This,grfFlags,pptd,ppPageSet,pstgmOptions,pcallback,nFirstPage,pcPagesPrinted,pnLastPage)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IPrint_SetInitialPageNum_Proxy( 
    IPrint * This,
    /* [in] */ LONG nFirstPage);


void __RPC_STUB IPrint_SetInitialPageNum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPrint_GetPageInfo_Proxy( 
    IPrint * This,
    /* [out] */ LONG *pnFirstPage,
    /* [out] */ LONG *pcPages);


void __RPC_STUB IPrint_GetPageInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT __stdcall IPrint_RemotePrint_Proxy( 
    IPrint * This,
    /* [in] */ DWORD grfFlags,
    /* [out][in] */ DVTARGETDEVICE **pptd,
    /* [out][in] */ PAGESET **pppageset,
    /* [unique][out][in] */ RemSTGMEDIUM *pstgmOptions,
    /* [in] */ IContinueCallback *pcallback,
    /* [in] */ LONG nFirstPage,
    /* [out] */ LONG *pcPagesPrinted,
    /* [out] */ LONG *pnLastPage);


void __RPC_STUB IPrint_RemotePrint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPrint_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_docobj_0270 */
/* [local] */ 

#endif
#ifndef _LPOLECOMMANDTARGET_DEFINED
#define _LPOLECOMMANDTARGET_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_docobj_0270_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_docobj_0270_v0_0_s_ifspec;

#ifndef __IOleCommandTarget_INTERFACE_DEFINED__
#define __IOleCommandTarget_INTERFACE_DEFINED__

/* interface IOleCommandTarget */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IOleCommandTarget *LPOLECOMMANDTARGET;

typedef 
enum OLECMDF
    {	OLECMDF_SUPPORTED	= 0x1,
	OLECMDF_ENABLED	= 0x2,
	OLECMDF_LATCHED	= 0x4,
	OLECMDF_NINCHED	= 0x8,
	OLECMDF_INVISIBLE	= 0x10,
	OLECMDF_DEFHIDEONCTXTMENU	= 0x20
    } 	OLECMDF;

typedef struct _tagOLECMD
    {
    ULONG cmdID;
    DWORD cmdf;
    } 	OLECMD;

typedef struct _tagOLECMDTEXT
    {
    DWORD cmdtextf;
    ULONG cwActual;
    ULONG cwBuf;
    /* [size_is] */ wchar_t rgwz[ 1 ];
    } 	OLECMDTEXT;

typedef 
enum OLECMDTEXTF
    {	OLECMDTEXTF_NONE	= 0,
	OLECMDTEXTF_NAME	= 1,
	OLECMDTEXTF_STATUS	= 2
    } 	OLECMDTEXTF;

typedef 
enum OLECMDEXECOPT
    {	OLECMDEXECOPT_DODEFAULT	= 0,
	OLECMDEXECOPT_PROMPTUSER	= 1,
	OLECMDEXECOPT_DONTPROMPTUSER	= 2,
	OLECMDEXECOPT_SHOWHELP	= 3
    } 	OLECMDEXECOPT;

/* OLECMDID_STOPDOWNLOAD and OLECMDID_ALLOWUILESSSAVEAS are supported for QueryStatus Only */
typedef 
enum OLECMDID
    {	OLECMDID_OPEN	= 1,
	OLECMDID_NEW	= 2,
	OLECMDID_SAVE	= 3,
	OLECMDID_SAVEAS	= 4,
	OLECMDID_SAVECOPYAS	= 5,
	OLECMDID_PRINT	= 6,
	OLECMDID_PRINTPREVIEW	= 7,
	OLECMDID_PAGESETUP	= 8,
	OLECMDID_SPELL	= 9,
	OLECMDID_PROPERTIES	= 10,
	OLECMDID_CUT	= 11,
	OLECMDID_COPY	= 12,
	OLECMDID_PASTE	= 13,
	OLECMDID_PASTESPECIAL	= 14,
	OLECMDID_UNDO	= 15,
	OLECMDID_REDO	= 16,
	OLECMDID_SELECTALL	= 17,
	OLECMDID_CLEARSELECTION	= 18,
	OLECMDID_ZOOM	= 19,
	OLECMDID_GETZOOMRANGE	= 20,
	OLECMDID_UPDATECOMMANDS	= 21,
	OLECMDID_REFRESH	= 22,
	OLECMDID_STOP	= 23,
	OLECMDID_HIDETOOLBARS	= 24,
	OLECMDID_SETPROGRESSMAX	= 25,
	OLECMDID_SETPROGRESSPOS	= 26,
	OLECMDID_SETPROGRESSTEXT	= 27,
	OLECMDID_SETTITLE	= 28,
	OLECMDID_SETDOWNLOADSTATE	= 29,
	OLECMDID_STOPDOWNLOAD	= 30,
	OLECMDID_ONTOOLBARACTIVATED	= 31,
	OLECMDID_FIND	= 32,
	OLECMDID_DELETE	= 33,
	OLECMDID_HTTPEQUIV	= 34,
	OLECMDID_HTTPEQUIV_DONE	= 35,
	OLECMDID_ENABLE_INTERACTION	= 36,
	OLECMDID_ONUNLOAD	= 37,
	OLECMDID_PROPERTYBAG2	= 38,
	OLECMDID_PREREFRESH	= 39,
	OLECMDID_SHOWSCRIPTERROR	= 40,
	OLECMDID_SHOWMESSAGE	= 41,
	OLECMDID_SHOWFIND	= 42,
	OLECMDID_SHOWPAGESETUP	= 43,
	OLECMDID_SHOWPRINT	= 44,
	OLECMDID_CLOSE	= 45,
	OLECMDID_ALLOWUILESSSAVEAS	= 46,
	OLECMDID_DONTDOWNLOADCSS	= 47,
	OLECMDID_UPDATEPAGESTATUS	= 48,
	OLECMDID_PRINT2	= 49,
	OLECMDID_PRINTPREVIEW2	= 50,
	OLECMDID_SETPRINTTEMPLATE	= 51,
	OLECMDID_GETPRINTTEMPLATE	= 52,
	OLECMDID_PAGEACTIONBLOCKED	= 55,
	OLECMDID_PAGEACTIONUIQUERY	= 56,
	OLECMDID_FOCUSVIEWCONTROLS	= 57,
	OLECMDID_FOCUSVIEWCONTROLSQUERY	= 58,
	OLECMDID_SHOWPAGEACTIONMENU	= 59
    } 	OLECMDID;

#define OLECMDERR_E_FIRST            (OLE_E_LAST+1)
#define OLECMDERR_E_NOTSUPPORTED     (OLECMDERR_E_FIRST)
#define OLECMDERR_E_DISABLED         (OLECMDERR_E_FIRST+1)
#define OLECMDERR_E_NOHELP           (OLECMDERR_E_FIRST+2)
#define OLECMDERR_E_CANCELED         (OLECMDERR_E_FIRST+3)
#define OLECMDERR_E_UNKNOWNGROUP     (OLECMDERR_E_FIRST+4)
#define MSOCMDERR_E_FIRST            OLECMDERR_E_FIRST
#define MSOCMDERR_E_NOTSUPPORTED     OLECMDERR_E_NOTSUPPORTED
#define MSOCMDERR_E_DISABLED         OLECMDERR_E_DISABLED
#define MSOCMDERR_E_NOHELP           OLECMDERR_E_NOHELP
#define MSOCMDERR_E_CANCELED         OLECMDERR_E_CANCELED
#define MSOCMDERR_E_UNKNOWNGROUP     OLECMDERR_E_UNKNOWNGROUP
#define OLECMDARGINDEX_SHOWPAGEACTIONMENU_HWND     0
#define OLECMDARGINDEX_SHOWPAGEACTIONMENU_X        1
#define OLECMDARGINDEX_SHOWPAGEACTIONMENU_Y        2
#define OLECMDARGINDEX_ACTIVEXINSTALL_PUBLISHER    0
#define OLECMDARGINDEX_ACTIVEXINSTALL_DISPLAYNAME  1
typedef 
enum IGNOREMIME
    {	IGNOREMIME_PROMPT	= 0x1,
	IGNOREMIME_TEXT	= 0x2
    } 	IGNOREMIME;


EXTERN_C const IID IID_IOleCommandTarget;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("b722bccb-4e68-101b-a2bc-00aa00404770")
    IOleCommandTarget : public IUnknown
    {
    public:
        virtual /* [input_sync] */ HRESULT STDMETHODCALLTYPE QueryStatus( 
            /* [unique][in] */ const GUID *pguidCmdGroup,
            /* [in] */ ULONG cCmds,
            /* [out][in][size_is] */ OLECMD prgCmds[  ],
            /* [unique][out][in] */ OLECMDTEXT *pCmdText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Exec( 
            /* [unique][in] */ const GUID *pguidCmdGroup,
            /* [in] */ DWORD nCmdID,
            /* [in] */ DWORD nCmdexecopt,
            /* [unique][in] */ VARIANT *pvaIn,
            /* [unique][out][in] */ VARIANT *pvaOut) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOleCommandTargetVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IOleCommandTarget * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IOleCommandTarget * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IOleCommandTarget * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *QueryStatus )( 
            IOleCommandTarget * This,
            /* [unique][in] */ const GUID *pguidCmdGroup,
            /* [in] */ ULONG cCmds,
            /* [out][in][size_is] */ OLECMD prgCmds[  ],
            /* [unique][out][in] */ OLECMDTEXT *pCmdText);
        
        HRESULT ( STDMETHODCALLTYPE *Exec )( 
            IOleCommandTarget * This,
            /* [unique][in] */ const GUID *pguidCmdGroup,
            /* [in] */ DWORD nCmdID,
            /* [in] */ DWORD nCmdexecopt,
            /* [unique][in] */ VARIANT *pvaIn,
            /* [unique][out][in] */ VARIANT *pvaOut);
        
        END_INTERFACE
    } IOleCommandTargetVtbl;

    interface IOleCommandTarget
    {
        CONST_VTBL struct IOleCommandTargetVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOleCommandTarget_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOleCommandTarget_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOleCommandTarget_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOleCommandTarget_QueryStatus(This,pguidCmdGroup,cCmds,prgCmds,pCmdText)	\
    (This)->lpVtbl -> QueryStatus(This,pguidCmdGroup,cCmds,prgCmds,pCmdText)

#define IOleCommandTarget_Exec(This,pguidCmdGroup,nCmdID,nCmdexecopt,pvaIn,pvaOut)	\
    (This)->lpVtbl -> Exec(This,pguidCmdGroup,nCmdID,nCmdexecopt,pvaIn,pvaOut)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [input_sync] */ HRESULT STDMETHODCALLTYPE IOleCommandTarget_QueryStatus_Proxy( 
    IOleCommandTarget * This,
    /* [unique][in] */ const GUID *pguidCmdGroup,
    /* [in] */ ULONG cCmds,
    /* [out][in][size_is] */ OLECMD prgCmds[  ],
    /* [unique][out][in] */ OLECMDTEXT *pCmdText);


void __RPC_STUB IOleCommandTarget_QueryStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOleCommandTarget_Exec_Proxy( 
    IOleCommandTarget * This,
    /* [unique][in] */ const GUID *pguidCmdGroup,
    /* [in] */ DWORD nCmdID,
    /* [in] */ DWORD nCmdexecopt,
    /* [unique][in] */ VARIANT *pvaIn,
    /* [unique][out][in] */ VARIANT *pvaOut);


void __RPC_STUB IOleCommandTarget_Exec_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOleCommandTarget_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_docobj_0271 */
/* [local] */ 

#endif
typedef enum
{
      OLECMDIDF_REFRESH_NORMAL          = 0,
      OLECMDIDF_REFRESH_IFEXPIRED       = 1,
      OLECMDIDF_REFRESH_CONTINUE        = 2,
      OLECMDIDF_REFRESH_COMPLETELY      = 3,
      OLECMDIDF_REFRESH_NO_CACHE        = 4,
      OLECMDIDF_REFRESH_RELOAD          = 5,
      OLECMDIDF_REFRESH_LEVELMASK       = 0x00FF,
      OLECMDIDF_REFRESH_CLEARUSERINPUT  = 0x1000,
      OLECMDIDF_REFRESH_PROMPTIFOFFLINE = 0x2000,
      OLECMDIDF_REFRESH_THROUGHSCRIPT   = 0x4000,
      OLECMDIDF_REFRESH_PAGEACTION_ACTIVEXINSTALL = 0x00010000,
      OLECMDIDF_REFRESH_PAGEACTION_FILEDOWNLOAD   = 0x00020000,
      OLECMDIDF_REFRESH_PAGEACTION_LOCALMACHINE   = 0x00040000,
      OLECMDIDF_REFRESH_PAGEACTION_POPUPWINDOW    = 0x00080000,
      OLECMDIDF_REFRESH_PAGEACTION_PROTLOCKDOWNLOCALMACHINE  = 0x00100000,
      OLECMDIDF_REFRESH_PAGEACTION_PROTLOCKDOWNTRUSTED       = 0x00200000,
      OLECMDIDF_REFRESH_PAGEACTION_PROTLOCKDOWNINTRANET      = 0x00400000,
      OLECMDIDF_REFRESH_PAGEACTION_PROTLOCKDOWNINTERNET      = 0x00800000,
      OLECMDIDF_REFRESH_PAGEACTION_PROTLOCKDOWNRESTRICTED    = 0x01000000,
} OLECMDID_REFRESHFLAG;
typedef enum
{
      OLECMDIDF_PAGEACTION_FILEDOWNLOAD              = 0x00000001,
      OLECMDIDF_PAGEACTION_ACTIVEXINSTALL            = 0x00000002,
      OLECMDIDF_PAGEACTION_ACTIVEXTRUSTFAIL          = 0x00000004,
      OLECMDIDF_PAGEACTION_ACTIVEXUSERDISABLE        = 0x00000008,
      OLECMDIDF_PAGEACTION_ACTIVEXDISALLOW           = 0x00000010,
      OLECMDIDF_PAGEACTION_ACTIVEXUNSAFE             = 0x00000020,
      OLECMDIDF_PAGEACTION_POPUPWINDOW               = 0x00000040,
      OLECMDIDF_PAGEACTION_LOCALMACHINE              = 0x00000080,
      OLECMDIDF_PAGEACTION_MIMETEXTPLAIN             = 0x00000100,
      OLECMDIDF_PAGEACTION_SCRIPTNAVIGATE            = 0x00000200,
      OLECMDIDF_PAGEACTION_PROTLOCKDOWNLOCALMACHINE  = 0x00000400,
      OLECMDIDF_PAGEACTION_PROTLOCKDOWNTRUSTED       = 0x00000800,
      OLECMDIDF_PAGEACTION_PROTLOCKDOWNINTRANET      = 0x00001000,
      OLECMDIDF_PAGEACTION_PROTLOCKDOWNINTERNET      = 0x00002000,
      OLECMDIDF_PAGEACTION_PROTLOCKDOWNRESTRICTED    = 0x00004000,
      OLECMDIDF_PAGEACTION_PROTLOCKDOWNDENY          = 0x00008000,
      OLECMDIDF_PAGEACTION_POPUPALLOWED              = 0x00010000,
      OLECMDIDF_PAGEACTION_RESET                     = 0x80000000,
} OLECMDID_PAGEACTIONFLAG;
typedef enum
{
    PAGEACTION_UI_DEFAULT     = 0,
    PAGEACTION_UI_MODAL       = 1,
    PAGEACTION_UI_MODELESS    = 2,
    PAGEACTION_UI_SILENT      = 3,
} PAGEACTION_UI;

////////////////////////////////////////////////////////////////////////////
//  Aliases to original office-compatible names
#define IMsoDocument             IOleDocument
#define IMsoDocumentSite         IOleDocumentSite
#define IMsoView                 IOleDocumentView
#define IEnumMsoView             IEnumOleDocumentViews
#define IMsoCommandTarget        IOleCommandTarget
#define LPMSODOCUMENT            LPOLEDOCUMENT
#define LPMSODOCUMENTSITE        LPOLEDOCUMENTSITE
#define LPMSOVIEW                LPOLEDOCUMENTVIEW
#define LPENUMMSOVIEW            LPENUMOLEDOCUMENTVIEWS
#define LPMSOCOMMANDTARGET       LPOLECOMMANDTARGET
#define MSOCMD                   OLECMD
#define MSOCMDTEXT               OLECMDTEXT
#define IID_IMsoDocument         IID_IOleDocument
#define IID_IMsoDocumentSite     IID_IOleDocumentSite
#define IID_IMsoView             IID_IOleDocumentView
#define IID_IEnumMsoView         IID_IEnumOleDocumentViews
#define IID_IMsoCommandTarget    IID_IOleCommandTarget
#define MSOCMDF_SUPPORTED OLECMDF_SUPPORTED
#define MSOCMDF_ENABLED OLECMDF_ENABLED
#define MSOCMDF_LATCHED OLECMDF_LATCHED
#define MSOCMDF_NINCHED OLECMDF_NINCHED
#define MSOCMDTEXTF_NONE OLECMDTEXTF_NONE
#define MSOCMDTEXTF_NAME OLECMDTEXTF_NAME
#define MSOCMDTEXTF_STATUS OLECMDTEXTF_STATUS
#define MSOCMDEXECOPT_DODEFAULT OLECMDEXECOPT_DODEFAULT
#define MSOCMDEXECOPT_PROMPTUSER OLECMDEXECOPT_PROMPTUSER
#define MSOCMDEXECOPT_DONTPROMPTUSER OLECMDEXECOPT_DONTPROMPTUSER
#define MSOCMDEXECOPT_SHOWHELP OLECMDEXECOPT_SHOWHELP
#define MSOCMDID_OPEN OLECMDID_OPEN
#define MSOCMDID_NEW OLECMDID_NEW
#define MSOCMDID_SAVE OLECMDID_SAVE
#define MSOCMDID_SAVEAS OLECMDID_SAVEAS
#define MSOCMDID_SAVECOPYAS OLECMDID_SAVECOPYAS
#define MSOCMDID_PRINT OLECMDID_PRINT
#define MSOCMDID_PRINTPREVIEW OLECMDID_PRINTPREVIEW
#define MSOCMDID_PAGESETUP OLECMDID_PAGESETUP
#define MSOCMDID_SPELL OLECMDID_SPELL
#define MSOCMDID_PROPERTIES OLECMDID_PROPERTIES
#define MSOCMDID_CUT OLECMDID_CUT
#define MSOCMDID_COPY OLECMDID_COPY
#define MSOCMDID_PASTE OLECMDID_PASTE
#define MSOCMDID_PASTESPECIAL OLECMDID_PASTESPECIAL
#define MSOCMDID_UNDO OLECMDID_UNDO
#define MSOCMDID_REDO OLECMDID_REDO
#define MSOCMDID_SELECTALL OLECMDID_SELECTALL
#define MSOCMDID_CLEARSELECTION OLECMDID_CLEARSELECTION
#define MSOCMDID_ZOOM OLECMDID_ZOOM
#define MSOCMDID_GETZOOMRANGE OLECMDID_GETZOOMRANGE
EXTERN_C const GUID SID_SContainerDispatch;


extern RPC_IF_HANDLE __MIDL_itf_docobj_0271_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_docobj_0271_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* [local] */ HRESULT __stdcall IEnumOleDocumentViews_Next_Proxy( 
    IEnumOleDocumentViews * This,
    /* [in] */ ULONG cViews,
    /* [out] */ IOleDocumentView **rgpView,
    /* [out] */ ULONG *pcFetched);


/* [call_as] */ HRESULT __stdcall IEnumOleDocumentViews_Next_Stub( 
    IEnumOleDocumentViews * This,
    /* [in] */ ULONG cViews,
    /* [length_is][size_is][out] */ IOleDocumentView **rgpView,
    /* [out] */ ULONG *pcFetched);

/* [local] */ HRESULT __stdcall IPrint_Print_Proxy( 
    IPrint * This,
    /* [in] */ DWORD grfFlags,
    /* [out][in] */ DVTARGETDEVICE **pptd,
    /* [out][in] */ PAGESET **ppPageSet,
    /* [unique][out][in] */ STGMEDIUM *pstgmOptions,
    /* [in] */ IContinueCallback *pcallback,
    /* [in] */ LONG nFirstPage,
    /* [out] */ LONG *pcPagesPrinted,
    /* [out] */ LONG *pnLastPage);


/* [call_as] */ HRESULT __stdcall IPrint_Print_Stub( 
    IPrint * This,
    /* [in] */ DWORD grfFlags,
    /* [out][in] */ DVTARGETDEVICE **pptd,
    /* [out][in] */ PAGESET **pppageset,
    /* [unique][out][in] */ RemSTGMEDIUM *pstgmOptions,
    /* [in] */ IContinueCallback *pcallback,
    /* [in] */ LONG nFirstPage,
    /* [out] */ LONG *pcPagesPrinted,
    /* [out] */ LONG *pnLastPage);



/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



