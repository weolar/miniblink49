

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* Compiler settings for netcfgn.idl:
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

#ifndef __netcfgn_h__
#define __netcfgn_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __INetCfgPnpReconfigCallback_FWD_DEFINED__
#define __INetCfgPnpReconfigCallback_FWD_DEFINED__
typedef interface INetCfgPnpReconfigCallback INetCfgPnpReconfigCallback;
#endif 	/* __INetCfgPnpReconfigCallback_FWD_DEFINED__ */


#ifndef __INetCfgComponentControl_FWD_DEFINED__
#define __INetCfgComponentControl_FWD_DEFINED__
typedef interface INetCfgComponentControl INetCfgComponentControl;
#endif 	/* __INetCfgComponentControl_FWD_DEFINED__ */


#ifndef __INetCfgComponentSetup_FWD_DEFINED__
#define __INetCfgComponentSetup_FWD_DEFINED__
typedef interface INetCfgComponentSetup INetCfgComponentSetup;
#endif 	/* __INetCfgComponentSetup_FWD_DEFINED__ */


#ifndef __INetCfgComponentPropertyUi_FWD_DEFINED__
#define __INetCfgComponentPropertyUi_FWD_DEFINED__
typedef interface INetCfgComponentPropertyUi INetCfgComponentPropertyUi;
#endif 	/* __INetCfgComponentPropertyUi_FWD_DEFINED__ */


#ifndef __INetCfgComponentNotifyBinding_FWD_DEFINED__
#define __INetCfgComponentNotifyBinding_FWD_DEFINED__
typedef interface INetCfgComponentNotifyBinding INetCfgComponentNotifyBinding;
#endif 	/* __INetCfgComponentNotifyBinding_FWD_DEFINED__ */


#ifndef __INetCfgComponentNotifyGlobal_FWD_DEFINED__
#define __INetCfgComponentNotifyGlobal_FWD_DEFINED__
typedef interface INetCfgComponentNotifyGlobal INetCfgComponentNotifyGlobal;
#endif 	/* __INetCfgComponentNotifyGlobal_FWD_DEFINED__ */


#ifndef __INetCfgComponentUpperEdge_FWD_DEFINED__
#define __INetCfgComponentUpperEdge_FWD_DEFINED__
typedef interface INetCfgComponentUpperEdge INetCfgComponentUpperEdge;
#endif 	/* __INetCfgComponentUpperEdge_FWD_DEFINED__ */


#ifndef __INetLanConnectionUiInfo_FWD_DEFINED__
#define __INetLanConnectionUiInfo_FWD_DEFINED__
typedef interface INetLanConnectionUiInfo INetLanConnectionUiInfo;
#endif 	/* __INetLanConnectionUiInfo_FWD_DEFINED__ */


#ifndef __INetCfgComponentSysPrep_FWD_DEFINED__
#define __INetCfgComponentSysPrep_FWD_DEFINED__
typedef interface INetCfgComponentSysPrep INetCfgComponentSysPrep;
#endif 	/* __INetCfgComponentSysPrep_FWD_DEFINED__ */


/* header files for imported files */
#include "unknwn.h"
#include "netcfgx.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_netcfgn_0000 */
/* [local] */ 

//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (c) Microsoft Corporation. All rights reserved.
//
//--------------------------------------------------------------------------
#if ( _MSC_VER >= 800 )
#pragma warning(disable:4201)
#endif


extern RPC_IF_HANDLE __MIDL_itf_netcfgn_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_netcfgn_0000_v0_0_s_ifspec;

#ifndef __INetCfgPnpReconfigCallback_INTERFACE_DEFINED__
#define __INetCfgPnpReconfigCallback_INTERFACE_DEFINED__

/* interface INetCfgPnpReconfigCallback */
/* [unique][uuid][object][local] */ 

typedef /* [v1_enum] */ 
enum tagNCPNP_RECONFIG_LAYER
    {	NCRL_NDIS	= 1,
	NCRL_TDI	= 2
    } 	NCPNP_RECONFIG_LAYER;


EXTERN_C const IID IID_INetCfgPnpReconfigCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8d84bd35-e227-11d2-b700-00a0c98a6a85")
    INetCfgPnpReconfigCallback : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SendPnpReconfig( 
            /* [in] */ NCPNP_RECONFIG_LAYER Layer,
            /* [in] */ LPCWSTR pszwUpper,
            /* [in] */ LPCWSTR pszwLower,
            /* [in] */ PVOID pvData,
            /* [in] */ DWORD dwSizeOfData) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetCfgPnpReconfigCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetCfgPnpReconfigCallback * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetCfgPnpReconfigCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetCfgPnpReconfigCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *SendPnpReconfig )( 
            INetCfgPnpReconfigCallback * This,
            /* [in] */ NCPNP_RECONFIG_LAYER Layer,
            /* [in] */ LPCWSTR pszwUpper,
            /* [in] */ LPCWSTR pszwLower,
            /* [in] */ PVOID pvData,
            /* [in] */ DWORD dwSizeOfData);
        
        END_INTERFACE
    } INetCfgPnpReconfigCallbackVtbl;

    interface INetCfgPnpReconfigCallback
    {
        CONST_VTBL struct INetCfgPnpReconfigCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetCfgPnpReconfigCallback_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetCfgPnpReconfigCallback_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetCfgPnpReconfigCallback_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetCfgPnpReconfigCallback_SendPnpReconfig(This,Layer,pszwUpper,pszwLower,pvData,dwSizeOfData)	\
    (This)->lpVtbl -> SendPnpReconfig(This,Layer,pszwUpper,pszwLower,pvData,dwSizeOfData)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INetCfgPnpReconfigCallback_SendPnpReconfig_Proxy( 
    INetCfgPnpReconfigCallback * This,
    /* [in] */ NCPNP_RECONFIG_LAYER Layer,
    /* [in] */ LPCWSTR pszwUpper,
    /* [in] */ LPCWSTR pszwLower,
    /* [in] */ PVOID pvData,
    /* [in] */ DWORD dwSizeOfData);


void __RPC_STUB INetCfgPnpReconfigCallback_SendPnpReconfig_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetCfgPnpReconfigCallback_INTERFACE_DEFINED__ */


#ifndef __INetCfgComponentControl_INTERFACE_DEFINED__
#define __INetCfgComponentControl_INTERFACE_DEFINED__

/* interface INetCfgComponentControl */
/* [unique][uuid][object][local] */ 


EXTERN_C const IID IID_INetCfgComponentControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("932238df-bea1-11d0-9298-00c04fc99dcf")
    INetCfgComponentControl : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ INetCfgComponent *pIComp,
            /* [in] */ INetCfg *pINetCfg,
            /* [in] */ BOOL fInstalling) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ApplyRegistryChanges( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ApplyPnpChanges( 
            /* [in] */ INetCfgPnpReconfigCallback *pICallback) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CancelChanges( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetCfgComponentControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetCfgComponentControl * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetCfgComponentControl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetCfgComponentControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            INetCfgComponentControl * This,
            /* [in] */ INetCfgComponent *pIComp,
            /* [in] */ INetCfg *pINetCfg,
            /* [in] */ BOOL fInstalling);
        
        HRESULT ( STDMETHODCALLTYPE *ApplyRegistryChanges )( 
            INetCfgComponentControl * This);
        
        HRESULT ( STDMETHODCALLTYPE *ApplyPnpChanges )( 
            INetCfgComponentControl * This,
            /* [in] */ INetCfgPnpReconfigCallback *pICallback);
        
        HRESULT ( STDMETHODCALLTYPE *CancelChanges )( 
            INetCfgComponentControl * This);
        
        END_INTERFACE
    } INetCfgComponentControlVtbl;

    interface INetCfgComponentControl
    {
        CONST_VTBL struct INetCfgComponentControlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetCfgComponentControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetCfgComponentControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetCfgComponentControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetCfgComponentControl_Initialize(This,pIComp,pINetCfg,fInstalling)	\
    (This)->lpVtbl -> Initialize(This,pIComp,pINetCfg,fInstalling)

#define INetCfgComponentControl_ApplyRegistryChanges(This)	\
    (This)->lpVtbl -> ApplyRegistryChanges(This)

#define INetCfgComponentControl_ApplyPnpChanges(This,pICallback)	\
    (This)->lpVtbl -> ApplyPnpChanges(This,pICallback)

#define INetCfgComponentControl_CancelChanges(This)	\
    (This)->lpVtbl -> CancelChanges(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INetCfgComponentControl_Initialize_Proxy( 
    INetCfgComponentControl * This,
    /* [in] */ INetCfgComponent *pIComp,
    /* [in] */ INetCfg *pINetCfg,
    /* [in] */ BOOL fInstalling);


void __RPC_STUB INetCfgComponentControl_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INetCfgComponentControl_ApplyRegistryChanges_Proxy( 
    INetCfgComponentControl * This);


void __RPC_STUB INetCfgComponentControl_ApplyRegistryChanges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INetCfgComponentControl_ApplyPnpChanges_Proxy( 
    INetCfgComponentControl * This,
    /* [in] */ INetCfgPnpReconfigCallback *pICallback);


void __RPC_STUB INetCfgComponentControl_ApplyPnpChanges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INetCfgComponentControl_CancelChanges_Proxy( 
    INetCfgComponentControl * This);


void __RPC_STUB INetCfgComponentControl_CancelChanges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetCfgComponentControl_INTERFACE_DEFINED__ */


#ifndef __INetCfgComponentSetup_INTERFACE_DEFINED__
#define __INetCfgComponentSetup_INTERFACE_DEFINED__

/* interface INetCfgComponentSetup */
/* [unique][uuid][object][local] */ 

typedef /* [v1_enum] */ 
enum tagNETWORK_INSTALL_TIME
    {	NSF_PRIMARYINSTALL	= 0x1,
	NSF_POSTSYSINSTALL	= 0x2
    } 	NETWORK_INSTALL_TIME;

typedef /* [v1_enum] */ 
enum tagNETWORK_UPGRADE_TYPE
    {	NSF_WIN16_UPGRADE	= 0x10,
	NSF_WIN95_UPGRADE	= 0x20,
	NSF_WINNT_WKS_UPGRADE	= 0x40,
	NSF_WINNT_SVR_UPGRADE	= 0x80,
	NSF_WINNT_SBS_UPGRADE	= 0x100,
	NSF_COMPONENT_UPDATE	= 0x200
    } 	NETWORK_UPGRADE_TYPE;


EXTERN_C const IID IID_INetCfgComponentSetup;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("932238e3-bea1-11d0-9298-00c04fc99dcf")
    INetCfgComponentSetup : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Install( 
            /* [in] */ DWORD dwSetupFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Upgrade( 
            /* [in] */ DWORD dwSetupFlags,
            /* [in] */ DWORD dwUpgradeFomBuildNo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReadAnswerFile( 
            /* [in] */ LPCWSTR pszwAnswerFile,
            /* [in] */ LPCWSTR pszwAnswerSections) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Removing( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetCfgComponentSetupVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetCfgComponentSetup * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetCfgComponentSetup * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetCfgComponentSetup * This);
        
        HRESULT ( STDMETHODCALLTYPE *Install )( 
            INetCfgComponentSetup * This,
            /* [in] */ DWORD dwSetupFlags);
        
        HRESULT ( STDMETHODCALLTYPE *Upgrade )( 
            INetCfgComponentSetup * This,
            /* [in] */ DWORD dwSetupFlags,
            /* [in] */ DWORD dwUpgradeFomBuildNo);
        
        HRESULT ( STDMETHODCALLTYPE *ReadAnswerFile )( 
            INetCfgComponentSetup * This,
            /* [in] */ LPCWSTR pszwAnswerFile,
            /* [in] */ LPCWSTR pszwAnswerSections);
        
        HRESULT ( STDMETHODCALLTYPE *Removing )( 
            INetCfgComponentSetup * This);
        
        END_INTERFACE
    } INetCfgComponentSetupVtbl;

    interface INetCfgComponentSetup
    {
        CONST_VTBL struct INetCfgComponentSetupVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetCfgComponentSetup_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetCfgComponentSetup_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetCfgComponentSetup_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetCfgComponentSetup_Install(This,dwSetupFlags)	\
    (This)->lpVtbl -> Install(This,dwSetupFlags)

#define INetCfgComponentSetup_Upgrade(This,dwSetupFlags,dwUpgradeFomBuildNo)	\
    (This)->lpVtbl -> Upgrade(This,dwSetupFlags,dwUpgradeFomBuildNo)

#define INetCfgComponentSetup_ReadAnswerFile(This,pszwAnswerFile,pszwAnswerSections)	\
    (This)->lpVtbl -> ReadAnswerFile(This,pszwAnswerFile,pszwAnswerSections)

#define INetCfgComponentSetup_Removing(This)	\
    (This)->lpVtbl -> Removing(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INetCfgComponentSetup_Install_Proxy( 
    INetCfgComponentSetup * This,
    /* [in] */ DWORD dwSetupFlags);


void __RPC_STUB INetCfgComponentSetup_Install_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INetCfgComponentSetup_Upgrade_Proxy( 
    INetCfgComponentSetup * This,
    /* [in] */ DWORD dwSetupFlags,
    /* [in] */ DWORD dwUpgradeFomBuildNo);


void __RPC_STUB INetCfgComponentSetup_Upgrade_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INetCfgComponentSetup_ReadAnswerFile_Proxy( 
    INetCfgComponentSetup * This,
    /* [in] */ LPCWSTR pszwAnswerFile,
    /* [in] */ LPCWSTR pszwAnswerSections);


void __RPC_STUB INetCfgComponentSetup_ReadAnswerFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INetCfgComponentSetup_Removing_Proxy( 
    INetCfgComponentSetup * This);


void __RPC_STUB INetCfgComponentSetup_Removing_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetCfgComponentSetup_INTERFACE_DEFINED__ */


#ifndef __INetCfgComponentPropertyUi_INTERFACE_DEFINED__
#define __INetCfgComponentPropertyUi_INTERFACE_DEFINED__

/* interface INetCfgComponentPropertyUi */
/* [unique][uuid][object][local] */ 

typedef /* [v1_enum] */ 
enum tagDEFAULT_PAGES
    {	DPP_ADVANCED	= 1
    } 	DEFAULT_PAGES;


EXTERN_C const IID IID_INetCfgComponentPropertyUi;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("932238e0-bea1-11d0-9298-00c04fc99dcf")
    INetCfgComponentPropertyUi : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE QueryPropertyUi( 
            /* [in] */ IUnknown *pUnkReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetContext( 
            /* [in] */ IUnknown *pUnkReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MergePropPages( 
            /* [out][in] */ DWORD *pdwDefPages,
            /* [out] */ BYTE **pahpspPrivate,
            /* [out] */ UINT *pcPages,
            /* [in] */ HWND hwndParent,
            /* [out] */ LPCWSTR *pszStartPage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ValidateProperties( 
            /* [in] */ HWND hwndSheet) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ApplyProperties( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CancelProperties( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetCfgComponentPropertyUiVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetCfgComponentPropertyUi * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetCfgComponentPropertyUi * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetCfgComponentPropertyUi * This);
        
        HRESULT ( STDMETHODCALLTYPE *QueryPropertyUi )( 
            INetCfgComponentPropertyUi * This,
            /* [in] */ IUnknown *pUnkReserved);
        
        HRESULT ( STDMETHODCALLTYPE *SetContext )( 
            INetCfgComponentPropertyUi * This,
            /* [in] */ IUnknown *pUnkReserved);
        
        HRESULT ( STDMETHODCALLTYPE *MergePropPages )( 
            INetCfgComponentPropertyUi * This,
            /* [out][in] */ DWORD *pdwDefPages,
            /* [out] */ BYTE **pahpspPrivate,
            /* [out] */ UINT *pcPages,
            /* [in] */ HWND hwndParent,
            /* [out] */ LPCWSTR *pszStartPage);
        
        HRESULT ( STDMETHODCALLTYPE *ValidateProperties )( 
            INetCfgComponentPropertyUi * This,
            /* [in] */ HWND hwndSheet);
        
        HRESULT ( STDMETHODCALLTYPE *ApplyProperties )( 
            INetCfgComponentPropertyUi * This);
        
        HRESULT ( STDMETHODCALLTYPE *CancelProperties )( 
            INetCfgComponentPropertyUi * This);
        
        END_INTERFACE
    } INetCfgComponentPropertyUiVtbl;

    interface INetCfgComponentPropertyUi
    {
        CONST_VTBL struct INetCfgComponentPropertyUiVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetCfgComponentPropertyUi_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetCfgComponentPropertyUi_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetCfgComponentPropertyUi_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetCfgComponentPropertyUi_QueryPropertyUi(This,pUnkReserved)	\
    (This)->lpVtbl -> QueryPropertyUi(This,pUnkReserved)

#define INetCfgComponentPropertyUi_SetContext(This,pUnkReserved)	\
    (This)->lpVtbl -> SetContext(This,pUnkReserved)

#define INetCfgComponentPropertyUi_MergePropPages(This,pdwDefPages,pahpspPrivate,pcPages,hwndParent,pszStartPage)	\
    (This)->lpVtbl -> MergePropPages(This,pdwDefPages,pahpspPrivate,pcPages,hwndParent,pszStartPage)

#define INetCfgComponentPropertyUi_ValidateProperties(This,hwndSheet)	\
    (This)->lpVtbl -> ValidateProperties(This,hwndSheet)

#define INetCfgComponentPropertyUi_ApplyProperties(This)	\
    (This)->lpVtbl -> ApplyProperties(This)

#define INetCfgComponentPropertyUi_CancelProperties(This)	\
    (This)->lpVtbl -> CancelProperties(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INetCfgComponentPropertyUi_QueryPropertyUi_Proxy( 
    INetCfgComponentPropertyUi * This,
    /* [in] */ IUnknown *pUnkReserved);


void __RPC_STUB INetCfgComponentPropertyUi_QueryPropertyUi_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INetCfgComponentPropertyUi_SetContext_Proxy( 
    INetCfgComponentPropertyUi * This,
    /* [in] */ IUnknown *pUnkReserved);


void __RPC_STUB INetCfgComponentPropertyUi_SetContext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INetCfgComponentPropertyUi_MergePropPages_Proxy( 
    INetCfgComponentPropertyUi * This,
    /* [out][in] */ DWORD *pdwDefPages,
    /* [out] */ BYTE **pahpspPrivate,
    /* [out] */ UINT *pcPages,
    /* [in] */ HWND hwndParent,
    /* [out] */ LPCWSTR *pszStartPage);


void __RPC_STUB INetCfgComponentPropertyUi_MergePropPages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INetCfgComponentPropertyUi_ValidateProperties_Proxy( 
    INetCfgComponentPropertyUi * This,
    /* [in] */ HWND hwndSheet);


void __RPC_STUB INetCfgComponentPropertyUi_ValidateProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INetCfgComponentPropertyUi_ApplyProperties_Proxy( 
    INetCfgComponentPropertyUi * This);


void __RPC_STUB INetCfgComponentPropertyUi_ApplyProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INetCfgComponentPropertyUi_CancelProperties_Proxy( 
    INetCfgComponentPropertyUi * This);


void __RPC_STUB INetCfgComponentPropertyUi_CancelProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetCfgComponentPropertyUi_INTERFACE_DEFINED__ */


#ifndef __INetCfgComponentNotifyBinding_INTERFACE_DEFINED__
#define __INetCfgComponentNotifyBinding_INTERFACE_DEFINED__

/* interface INetCfgComponentNotifyBinding */
/* [unique][uuid][object][local] */ 

typedef /* [v1_enum] */ 
enum tagBIND_FLAGS1
    {	NCN_ADD	= 0x1,
	NCN_REMOVE	= 0x2,
	NCN_UPDATE	= 0x4,
	NCN_ENABLE	= 0x10,
	NCN_DISABLE	= 0x20,
	NCN_BINDING_PATH	= 0x100,
	NCN_PROPERTYCHANGE	= 0x200,
	NCN_NET	= 0x10000,
	NCN_NETTRANS	= 0x20000,
	NCN_NETCLIENT	= 0x40000,
	NCN_NETSERVICE	= 0x80000
    } 	BIND_FLAGS1;


EXTERN_C const IID IID_INetCfgComponentNotifyBinding;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("932238e1-bea1-11d0-9298-00c04fc99dcf")
    INetCfgComponentNotifyBinding : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE QueryBindingPath( 
            /* [in] */ DWORD dwChangeFlag,
            /* [in] */ INetCfgBindingPath *pIPath) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NotifyBindingPath( 
            /* [in] */ DWORD dwChangeFlag,
            /* [in] */ INetCfgBindingPath *pIPath) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetCfgComponentNotifyBindingVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetCfgComponentNotifyBinding * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetCfgComponentNotifyBinding * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetCfgComponentNotifyBinding * This);
        
        HRESULT ( STDMETHODCALLTYPE *QueryBindingPath )( 
            INetCfgComponentNotifyBinding * This,
            /* [in] */ DWORD dwChangeFlag,
            /* [in] */ INetCfgBindingPath *pIPath);
        
        HRESULT ( STDMETHODCALLTYPE *NotifyBindingPath )( 
            INetCfgComponentNotifyBinding * This,
            /* [in] */ DWORD dwChangeFlag,
            /* [in] */ INetCfgBindingPath *pIPath);
        
        END_INTERFACE
    } INetCfgComponentNotifyBindingVtbl;

    interface INetCfgComponentNotifyBinding
    {
        CONST_VTBL struct INetCfgComponentNotifyBindingVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetCfgComponentNotifyBinding_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetCfgComponentNotifyBinding_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetCfgComponentNotifyBinding_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetCfgComponentNotifyBinding_QueryBindingPath(This,dwChangeFlag,pIPath)	\
    (This)->lpVtbl -> QueryBindingPath(This,dwChangeFlag,pIPath)

#define INetCfgComponentNotifyBinding_NotifyBindingPath(This,dwChangeFlag,pIPath)	\
    (This)->lpVtbl -> NotifyBindingPath(This,dwChangeFlag,pIPath)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INetCfgComponentNotifyBinding_QueryBindingPath_Proxy( 
    INetCfgComponentNotifyBinding * This,
    /* [in] */ DWORD dwChangeFlag,
    /* [in] */ INetCfgBindingPath *pIPath);


void __RPC_STUB INetCfgComponentNotifyBinding_QueryBindingPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INetCfgComponentNotifyBinding_NotifyBindingPath_Proxy( 
    INetCfgComponentNotifyBinding * This,
    /* [in] */ DWORD dwChangeFlag,
    /* [in] */ INetCfgBindingPath *pIPath);


void __RPC_STUB INetCfgComponentNotifyBinding_NotifyBindingPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetCfgComponentNotifyBinding_INTERFACE_DEFINED__ */


#ifndef __INetCfgComponentNotifyGlobal_INTERFACE_DEFINED__
#define __INetCfgComponentNotifyGlobal_INTERFACE_DEFINED__

/* interface INetCfgComponentNotifyGlobal */
/* [unique][uuid][object][local] */ 


EXTERN_C const IID IID_INetCfgComponentNotifyGlobal;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("932238e2-bea1-11d0-9298-00c04fc99dcf")
    INetCfgComponentNotifyGlobal : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetSupportedNotifications( 
            /* [out] */ DWORD *dwNotifications) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SysQueryBindingPath( 
            /* [in] */ DWORD dwChangeFlag,
            /* [in] */ INetCfgBindingPath *pIPath) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SysNotifyBindingPath( 
            /* [in] */ DWORD dwChangeFlag,
            /* [in] */ INetCfgBindingPath *pIPath) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SysNotifyComponent( 
            /* [in] */ DWORD dwChangeFlag,
            /* [in] */ INetCfgComponent *pIComp) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetCfgComponentNotifyGlobalVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetCfgComponentNotifyGlobal * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetCfgComponentNotifyGlobal * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetCfgComponentNotifyGlobal * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSupportedNotifications )( 
            INetCfgComponentNotifyGlobal * This,
            /* [out] */ DWORD *dwNotifications);
        
        HRESULT ( STDMETHODCALLTYPE *SysQueryBindingPath )( 
            INetCfgComponentNotifyGlobal * This,
            /* [in] */ DWORD dwChangeFlag,
            /* [in] */ INetCfgBindingPath *pIPath);
        
        HRESULT ( STDMETHODCALLTYPE *SysNotifyBindingPath )( 
            INetCfgComponentNotifyGlobal * This,
            /* [in] */ DWORD dwChangeFlag,
            /* [in] */ INetCfgBindingPath *pIPath);
        
        HRESULT ( STDMETHODCALLTYPE *SysNotifyComponent )( 
            INetCfgComponentNotifyGlobal * This,
            /* [in] */ DWORD dwChangeFlag,
            /* [in] */ INetCfgComponent *pIComp);
        
        END_INTERFACE
    } INetCfgComponentNotifyGlobalVtbl;

    interface INetCfgComponentNotifyGlobal
    {
        CONST_VTBL struct INetCfgComponentNotifyGlobalVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetCfgComponentNotifyGlobal_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetCfgComponentNotifyGlobal_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetCfgComponentNotifyGlobal_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetCfgComponentNotifyGlobal_GetSupportedNotifications(This,dwNotifications)	\
    (This)->lpVtbl -> GetSupportedNotifications(This,dwNotifications)

#define INetCfgComponentNotifyGlobal_SysQueryBindingPath(This,dwChangeFlag,pIPath)	\
    (This)->lpVtbl -> SysQueryBindingPath(This,dwChangeFlag,pIPath)

#define INetCfgComponentNotifyGlobal_SysNotifyBindingPath(This,dwChangeFlag,pIPath)	\
    (This)->lpVtbl -> SysNotifyBindingPath(This,dwChangeFlag,pIPath)

#define INetCfgComponentNotifyGlobal_SysNotifyComponent(This,dwChangeFlag,pIComp)	\
    (This)->lpVtbl -> SysNotifyComponent(This,dwChangeFlag,pIComp)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INetCfgComponentNotifyGlobal_GetSupportedNotifications_Proxy( 
    INetCfgComponentNotifyGlobal * This,
    /* [out] */ DWORD *dwNotifications);


void __RPC_STUB INetCfgComponentNotifyGlobal_GetSupportedNotifications_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INetCfgComponentNotifyGlobal_SysQueryBindingPath_Proxy( 
    INetCfgComponentNotifyGlobal * This,
    /* [in] */ DWORD dwChangeFlag,
    /* [in] */ INetCfgBindingPath *pIPath);


void __RPC_STUB INetCfgComponentNotifyGlobal_SysQueryBindingPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INetCfgComponentNotifyGlobal_SysNotifyBindingPath_Proxy( 
    INetCfgComponentNotifyGlobal * This,
    /* [in] */ DWORD dwChangeFlag,
    /* [in] */ INetCfgBindingPath *pIPath);


void __RPC_STUB INetCfgComponentNotifyGlobal_SysNotifyBindingPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INetCfgComponentNotifyGlobal_SysNotifyComponent_Proxy( 
    INetCfgComponentNotifyGlobal * This,
    /* [in] */ DWORD dwChangeFlag,
    /* [in] */ INetCfgComponent *pIComp);


void __RPC_STUB INetCfgComponentNotifyGlobal_SysNotifyComponent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetCfgComponentNotifyGlobal_INTERFACE_DEFINED__ */


#ifndef __INetCfgComponentUpperEdge_INTERFACE_DEFINED__
#define __INetCfgComponentUpperEdge_INTERFACE_DEFINED__

/* interface INetCfgComponentUpperEdge */
/* [unique][uuid][object][local] */ 


EXTERN_C const IID IID_INetCfgComponentUpperEdge;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("932238e4-bea1-11d0-9298-00c04fc99dcf")
    INetCfgComponentUpperEdge : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetInterfaceIdsForAdapter( 
            /* [in] */ INetCfgComponent *pAdapter,
            /* [out] */ DWORD *pdwNumInterfaces,
            /* [out] */ GUID **ppguidInterfaceIds) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddInterfacesToAdapter( 
            /* [in] */ INetCfgComponent *pAdapter,
            /* [in] */ DWORD dwNumInterfaces) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveInterfacesFromAdapter( 
            /* [in] */ INetCfgComponent *pAdapter,
            /* [in] */ DWORD dwNumInterfaces,
            /* [in] */ const GUID *pguidInterfaceIds) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetCfgComponentUpperEdgeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetCfgComponentUpperEdge * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetCfgComponentUpperEdge * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetCfgComponentUpperEdge * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetInterfaceIdsForAdapter )( 
            INetCfgComponentUpperEdge * This,
            /* [in] */ INetCfgComponent *pAdapter,
            /* [out] */ DWORD *pdwNumInterfaces,
            /* [out] */ GUID **ppguidInterfaceIds);
        
        HRESULT ( STDMETHODCALLTYPE *AddInterfacesToAdapter )( 
            INetCfgComponentUpperEdge * This,
            /* [in] */ INetCfgComponent *pAdapter,
            /* [in] */ DWORD dwNumInterfaces);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveInterfacesFromAdapter )( 
            INetCfgComponentUpperEdge * This,
            /* [in] */ INetCfgComponent *pAdapter,
            /* [in] */ DWORD dwNumInterfaces,
            /* [in] */ const GUID *pguidInterfaceIds);
        
        END_INTERFACE
    } INetCfgComponentUpperEdgeVtbl;

    interface INetCfgComponentUpperEdge
    {
        CONST_VTBL struct INetCfgComponentUpperEdgeVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetCfgComponentUpperEdge_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetCfgComponentUpperEdge_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetCfgComponentUpperEdge_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetCfgComponentUpperEdge_GetInterfaceIdsForAdapter(This,pAdapter,pdwNumInterfaces,ppguidInterfaceIds)	\
    (This)->lpVtbl -> GetInterfaceIdsForAdapter(This,pAdapter,pdwNumInterfaces,ppguidInterfaceIds)

#define INetCfgComponentUpperEdge_AddInterfacesToAdapter(This,pAdapter,dwNumInterfaces)	\
    (This)->lpVtbl -> AddInterfacesToAdapter(This,pAdapter,dwNumInterfaces)

#define INetCfgComponentUpperEdge_RemoveInterfacesFromAdapter(This,pAdapter,dwNumInterfaces,pguidInterfaceIds)	\
    (This)->lpVtbl -> RemoveInterfacesFromAdapter(This,pAdapter,dwNumInterfaces,pguidInterfaceIds)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INetCfgComponentUpperEdge_GetInterfaceIdsForAdapter_Proxy( 
    INetCfgComponentUpperEdge * This,
    /* [in] */ INetCfgComponent *pAdapter,
    /* [out] */ DWORD *pdwNumInterfaces,
    /* [out] */ GUID **ppguidInterfaceIds);


void __RPC_STUB INetCfgComponentUpperEdge_GetInterfaceIdsForAdapter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INetCfgComponentUpperEdge_AddInterfacesToAdapter_Proxy( 
    INetCfgComponentUpperEdge * This,
    /* [in] */ INetCfgComponent *pAdapter,
    /* [in] */ DWORD dwNumInterfaces);


void __RPC_STUB INetCfgComponentUpperEdge_AddInterfacesToAdapter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INetCfgComponentUpperEdge_RemoveInterfacesFromAdapter_Proxy( 
    INetCfgComponentUpperEdge * This,
    /* [in] */ INetCfgComponent *pAdapter,
    /* [in] */ DWORD dwNumInterfaces,
    /* [in] */ const GUID *pguidInterfaceIds);


void __RPC_STUB INetCfgComponentUpperEdge_RemoveInterfacesFromAdapter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetCfgComponentUpperEdge_INTERFACE_DEFINED__ */


#ifndef __INetLanConnectionUiInfo_INTERFACE_DEFINED__
#define __INetLanConnectionUiInfo_INTERFACE_DEFINED__

/* interface INetLanConnectionUiInfo */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_INetLanConnectionUiInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C08956A6-1CD3-11D1-B1C5-00805FC1270E")
    INetLanConnectionUiInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDeviceGuid( 
            /* [out] */ GUID *pguid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetLanConnectionUiInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetLanConnectionUiInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetLanConnectionUiInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetLanConnectionUiInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetDeviceGuid )( 
            INetLanConnectionUiInfo * This,
            /* [out] */ GUID *pguid);
        
        END_INTERFACE
    } INetLanConnectionUiInfoVtbl;

    interface INetLanConnectionUiInfo
    {
        CONST_VTBL struct INetLanConnectionUiInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetLanConnectionUiInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetLanConnectionUiInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetLanConnectionUiInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetLanConnectionUiInfo_GetDeviceGuid(This,pguid)	\
    (This)->lpVtbl -> GetDeviceGuid(This,pguid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INetLanConnectionUiInfo_GetDeviceGuid_Proxy( 
    INetLanConnectionUiInfo * This,
    /* [out] */ GUID *pguid);


void __RPC_STUB INetLanConnectionUiInfo_GetDeviceGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetLanConnectionUiInfo_INTERFACE_DEFINED__ */


#ifndef __INetCfgComponentSysPrep_INTERFACE_DEFINED__
#define __INetCfgComponentSysPrep_INTERFACE_DEFINED__

/* interface INetCfgComponentSysPrep */
/* [unique][uuid][object][local] */ 


EXTERN_C const IID IID_INetCfgComponentSysPrep;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C0E8AE9A-306E-11D1-AACF-00805FC1270E")
    INetCfgComponentSysPrep : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SaveAdapterParameters( 
            /* [in] */ INetCfgSysPrep *pncsp,
            /* [in] */ LPCWSTR pszwAnswerSections,
            /* [in] */ GUID *pAdapterInstanceGuid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RestoreAdapterParameters( 
            /* [in] */ LPCWSTR pszwAnswerFile,
            /* [in] */ LPCWSTR pszwAnswerSection,
            /* [in] */ GUID *pAdapterInstanceGuid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetCfgComponentSysPrepVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetCfgComponentSysPrep * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetCfgComponentSysPrep * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetCfgComponentSysPrep * This);
        
        HRESULT ( STDMETHODCALLTYPE *SaveAdapterParameters )( 
            INetCfgComponentSysPrep * This,
            /* [in] */ INetCfgSysPrep *pncsp,
            /* [in] */ LPCWSTR pszwAnswerSections,
            /* [in] */ GUID *pAdapterInstanceGuid);
        
        HRESULT ( STDMETHODCALLTYPE *RestoreAdapterParameters )( 
            INetCfgComponentSysPrep * This,
            /* [in] */ LPCWSTR pszwAnswerFile,
            /* [in] */ LPCWSTR pszwAnswerSection,
            /* [in] */ GUID *pAdapterInstanceGuid);
        
        END_INTERFACE
    } INetCfgComponentSysPrepVtbl;

    interface INetCfgComponentSysPrep
    {
        CONST_VTBL struct INetCfgComponentSysPrepVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetCfgComponentSysPrep_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetCfgComponentSysPrep_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetCfgComponentSysPrep_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetCfgComponentSysPrep_SaveAdapterParameters(This,pncsp,pszwAnswerSections,pAdapterInstanceGuid)	\
    (This)->lpVtbl -> SaveAdapterParameters(This,pncsp,pszwAnswerSections,pAdapterInstanceGuid)

#define INetCfgComponentSysPrep_RestoreAdapterParameters(This,pszwAnswerFile,pszwAnswerSection,pAdapterInstanceGuid)	\
    (This)->lpVtbl -> RestoreAdapterParameters(This,pszwAnswerFile,pszwAnswerSection,pAdapterInstanceGuid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INetCfgComponentSysPrep_SaveAdapterParameters_Proxy( 
    INetCfgComponentSysPrep * This,
    /* [in] */ INetCfgSysPrep *pncsp,
    /* [in] */ LPCWSTR pszwAnswerSections,
    /* [in] */ GUID *pAdapterInstanceGuid);


void __RPC_STUB INetCfgComponentSysPrep_SaveAdapterParameters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INetCfgComponentSysPrep_RestoreAdapterParameters_Proxy( 
    INetCfgComponentSysPrep * This,
    /* [in] */ LPCWSTR pszwAnswerFile,
    /* [in] */ LPCWSTR pszwAnswerSection,
    /* [in] */ GUID *pAdapterInstanceGuid);


void __RPC_STUB INetCfgComponentSysPrep_RestoreAdapterParameters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetCfgComponentSysPrep_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



