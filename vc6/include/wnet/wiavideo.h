

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* Compiler settings for wiavideo.idl:
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

#ifndef __wiavideo_h__
#define __wiavideo_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IWiaVideo_FWD_DEFINED__
#define __IWiaVideo_FWD_DEFINED__
typedef interface IWiaVideo IWiaVideo;
#endif 	/* __IWiaVideo_FWD_DEFINED__ */


#ifndef __WiaVideo_FWD_DEFINED__
#define __WiaVideo_FWD_DEFINED__

#ifdef __cplusplus
typedef class WiaVideo WiaVideo;
#else
typedef struct WiaVideo WiaVideo;
#endif /* __cplusplus */

#endif 	/* __WiaVideo_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_wiavideo_0000 */
/* [local] */ 

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_wiavideo_0000_0001
    {	WIAVIDEO_NO_VIDEO	= 1,
	WIAVIDEO_CREATING_VIDEO	= 2,
	WIAVIDEO_VIDEO_CREATED	= 3,
	WIAVIDEO_VIDEO_PLAYING	= 4,
	WIAVIDEO_VIDEO_PAUSED	= 5,
	WIAVIDEO_DESTROYING_VIDEO	= 6
    } 	WIAVIDEO_STATE;



extern RPC_IF_HANDLE __MIDL_itf_wiavideo_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wiavideo_0000_v0_0_s_ifspec;

#ifndef __IWiaVideo_INTERFACE_DEFINED__
#define __IWiaVideo_INTERFACE_DEFINED__

/* interface IWiaVideo */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IWiaVideo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D52920AA-DB88-41F0-946C-E00DC0A19CFA")
    IWiaVideo : public IUnknown
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PreviewVisible( 
            /* [retval][out] */ BOOL *pbPreviewVisible) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PreviewVisible( 
            /* [in] */ BOOL bPreviewVisible) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ImagesDirectory( 
            /* [retval][out] */ BSTR *pbstrImageDirectory) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ImagesDirectory( 
            /* [in] */ BSTR bstrImageDirectory) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateVideoByWiaDevID( 
            /* [in] */ BSTR bstrWiaDeviceID,
            /* [in] */ HWND hwndParent,
            /* [in] */ BOOL bStretchToFitParent,
            /* [in] */ BOOL bAutoBeginPlayback) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateVideoByDevNum( 
            /* [in] */ UINT uiDeviceNumber,
            /* [in] */ HWND hwndParent,
            /* [in] */ BOOL bStretchToFitParent,
            /* [in] */ BOOL bAutoBeginPlayback) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateVideoByName( 
            /* [in] */ BSTR bstrFriendlyName,
            /* [in] */ HWND hwndParent,
            /* [in] */ BOOL bStretchToFitParent,
            /* [in] */ BOOL bAutoBeginPlayback) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DestroyVideo( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Play( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Pause( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE TakePicture( 
            /* [out] */ BSTR *pbstrNewImageFilename) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ResizeVideo( 
            /* [in] */ BOOL bStretchToFitParent) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCurrentState( 
            /* [retval][out] */ WIAVIDEO_STATE *pState) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWiaVideoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWiaVideo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWiaVideo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWiaVideo * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_PreviewVisible )( 
            IWiaVideo * This,
            /* [retval][out] */ BOOL *pbPreviewVisible);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_PreviewVisible )( 
            IWiaVideo * This,
            /* [in] */ BOOL bPreviewVisible);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ImagesDirectory )( 
            IWiaVideo * This,
            /* [retval][out] */ BSTR *pbstrImageDirectory);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ImagesDirectory )( 
            IWiaVideo * This,
            /* [in] */ BSTR bstrImageDirectory);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateVideoByWiaDevID )( 
            IWiaVideo * This,
            /* [in] */ BSTR bstrWiaDeviceID,
            /* [in] */ HWND hwndParent,
            /* [in] */ BOOL bStretchToFitParent,
            /* [in] */ BOOL bAutoBeginPlayback);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateVideoByDevNum )( 
            IWiaVideo * This,
            /* [in] */ UINT uiDeviceNumber,
            /* [in] */ HWND hwndParent,
            /* [in] */ BOOL bStretchToFitParent,
            /* [in] */ BOOL bAutoBeginPlayback);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateVideoByName )( 
            IWiaVideo * This,
            /* [in] */ BSTR bstrFriendlyName,
            /* [in] */ HWND hwndParent,
            /* [in] */ BOOL bStretchToFitParent,
            /* [in] */ BOOL bAutoBeginPlayback);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DestroyVideo )( 
            IWiaVideo * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Play )( 
            IWiaVideo * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Pause )( 
            IWiaVideo * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *TakePicture )( 
            IWiaVideo * This,
            /* [out] */ BSTR *pbstrNewImageFilename);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ResizeVideo )( 
            IWiaVideo * This,
            /* [in] */ BOOL bStretchToFitParent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCurrentState )( 
            IWiaVideo * This,
            /* [retval][out] */ WIAVIDEO_STATE *pState);
        
        END_INTERFACE
    } IWiaVideoVtbl;

    interface IWiaVideo
    {
        CONST_VTBL struct IWiaVideoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWiaVideo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWiaVideo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWiaVideo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWiaVideo_get_PreviewVisible(This,pbPreviewVisible)	\
    (This)->lpVtbl -> get_PreviewVisible(This,pbPreviewVisible)

#define IWiaVideo_put_PreviewVisible(This,bPreviewVisible)	\
    (This)->lpVtbl -> put_PreviewVisible(This,bPreviewVisible)

#define IWiaVideo_get_ImagesDirectory(This,pbstrImageDirectory)	\
    (This)->lpVtbl -> get_ImagesDirectory(This,pbstrImageDirectory)

#define IWiaVideo_put_ImagesDirectory(This,bstrImageDirectory)	\
    (This)->lpVtbl -> put_ImagesDirectory(This,bstrImageDirectory)

#define IWiaVideo_CreateVideoByWiaDevID(This,bstrWiaDeviceID,hwndParent,bStretchToFitParent,bAutoBeginPlayback)	\
    (This)->lpVtbl -> CreateVideoByWiaDevID(This,bstrWiaDeviceID,hwndParent,bStretchToFitParent,bAutoBeginPlayback)

#define IWiaVideo_CreateVideoByDevNum(This,uiDeviceNumber,hwndParent,bStretchToFitParent,bAutoBeginPlayback)	\
    (This)->lpVtbl -> CreateVideoByDevNum(This,uiDeviceNumber,hwndParent,bStretchToFitParent,bAutoBeginPlayback)

#define IWiaVideo_CreateVideoByName(This,bstrFriendlyName,hwndParent,bStretchToFitParent,bAutoBeginPlayback)	\
    (This)->lpVtbl -> CreateVideoByName(This,bstrFriendlyName,hwndParent,bStretchToFitParent,bAutoBeginPlayback)

#define IWiaVideo_DestroyVideo(This)	\
    (This)->lpVtbl -> DestroyVideo(This)

#define IWiaVideo_Play(This)	\
    (This)->lpVtbl -> Play(This)

#define IWiaVideo_Pause(This)	\
    (This)->lpVtbl -> Pause(This)

#define IWiaVideo_TakePicture(This,pbstrNewImageFilename)	\
    (This)->lpVtbl -> TakePicture(This,pbstrNewImageFilename)

#define IWiaVideo_ResizeVideo(This,bStretchToFitParent)	\
    (This)->lpVtbl -> ResizeVideo(This,bStretchToFitParent)

#define IWiaVideo_GetCurrentState(This,pState)	\
    (This)->lpVtbl -> GetCurrentState(This,pState)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaVideo_get_PreviewVisible_Proxy( 
    IWiaVideo * This,
    /* [retval][out] */ BOOL *pbPreviewVisible);


void __RPC_STUB IWiaVideo_get_PreviewVisible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWiaVideo_put_PreviewVisible_Proxy( 
    IWiaVideo * This,
    /* [in] */ BOOL bPreviewVisible);


void __RPC_STUB IWiaVideo_put_PreviewVisible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWiaVideo_get_ImagesDirectory_Proxy( 
    IWiaVideo * This,
    /* [retval][out] */ BSTR *pbstrImageDirectory);


void __RPC_STUB IWiaVideo_get_ImagesDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWiaVideo_put_ImagesDirectory_Proxy( 
    IWiaVideo * This,
    /* [in] */ BSTR bstrImageDirectory);


void __RPC_STUB IWiaVideo_put_ImagesDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaVideo_CreateVideoByWiaDevID_Proxy( 
    IWiaVideo * This,
    /* [in] */ BSTR bstrWiaDeviceID,
    /* [in] */ HWND hwndParent,
    /* [in] */ BOOL bStretchToFitParent,
    /* [in] */ BOOL bAutoBeginPlayback);


void __RPC_STUB IWiaVideo_CreateVideoByWiaDevID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaVideo_CreateVideoByDevNum_Proxy( 
    IWiaVideo * This,
    /* [in] */ UINT uiDeviceNumber,
    /* [in] */ HWND hwndParent,
    /* [in] */ BOOL bStretchToFitParent,
    /* [in] */ BOOL bAutoBeginPlayback);


void __RPC_STUB IWiaVideo_CreateVideoByDevNum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaVideo_CreateVideoByName_Proxy( 
    IWiaVideo * This,
    /* [in] */ BSTR bstrFriendlyName,
    /* [in] */ HWND hwndParent,
    /* [in] */ BOOL bStretchToFitParent,
    /* [in] */ BOOL bAutoBeginPlayback);


void __RPC_STUB IWiaVideo_CreateVideoByName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaVideo_DestroyVideo_Proxy( 
    IWiaVideo * This);


void __RPC_STUB IWiaVideo_DestroyVideo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaVideo_Play_Proxy( 
    IWiaVideo * This);


void __RPC_STUB IWiaVideo_Play_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaVideo_Pause_Proxy( 
    IWiaVideo * This);


void __RPC_STUB IWiaVideo_Pause_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaVideo_TakePicture_Proxy( 
    IWiaVideo * This,
    /* [out] */ BSTR *pbstrNewImageFilename);


void __RPC_STUB IWiaVideo_TakePicture_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaVideo_ResizeVideo_Proxy( 
    IWiaVideo * This,
    /* [in] */ BOOL bStretchToFitParent);


void __RPC_STUB IWiaVideo_ResizeVideo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWiaVideo_GetCurrentState_Proxy( 
    IWiaVideo * This,
    /* [retval][out] */ WIAVIDEO_STATE *pState);


void __RPC_STUB IWiaVideo_GetCurrentState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWiaVideo_INTERFACE_DEFINED__ */



#ifndef __WIAVIDEOLib_LIBRARY_DEFINED__
#define __WIAVIDEOLib_LIBRARY_DEFINED__

/* library WIAVIDEOLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_WIAVIDEOLib;

EXTERN_C const CLSID CLSID_WiaVideo;

#ifdef __cplusplus

class DECLSPEC_UUID("3908C3CD-4478-4536-AF2F-10C25D4EF89A")
WiaVideo;
#endif
#endif /* __WIAVIDEOLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  HWND_UserSize(     unsigned long *, unsigned long            , HWND * ); 
unsigned char * __RPC_USER  HWND_UserMarshal(  unsigned long *, unsigned char *, HWND * ); 
unsigned char * __RPC_USER  HWND_UserUnmarshal(unsigned long *, unsigned char *, HWND * ); 
void                      __RPC_USER  HWND_UserFree(     unsigned long *, HWND * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



