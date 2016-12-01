

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* Compiler settings for shldisp.idl:
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

#ifndef __shldisp_h__
#define __shldisp_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IFolderViewOC_FWD_DEFINED__
#define __IFolderViewOC_FWD_DEFINED__
typedef interface IFolderViewOC IFolderViewOC;
#endif 	/* __IFolderViewOC_FWD_DEFINED__ */


#ifndef __DShellFolderViewEvents_FWD_DEFINED__
#define __DShellFolderViewEvents_FWD_DEFINED__
typedef interface DShellFolderViewEvents DShellFolderViewEvents;
#endif 	/* __DShellFolderViewEvents_FWD_DEFINED__ */


#ifndef __ShellFolderViewOC_FWD_DEFINED__
#define __ShellFolderViewOC_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShellFolderViewOC ShellFolderViewOC;
#else
typedef struct ShellFolderViewOC ShellFolderViewOC;
#endif /* __cplusplus */

#endif 	/* __ShellFolderViewOC_FWD_DEFINED__ */


#ifndef __DFConstraint_FWD_DEFINED__
#define __DFConstraint_FWD_DEFINED__
typedef interface DFConstraint DFConstraint;
#endif 	/* __DFConstraint_FWD_DEFINED__ */


#ifndef __ISearchCommandExt_FWD_DEFINED__
#define __ISearchCommandExt_FWD_DEFINED__
typedef interface ISearchCommandExt ISearchCommandExt;
#endif 	/* __ISearchCommandExt_FWD_DEFINED__ */


#ifndef __FolderItem_FWD_DEFINED__
#define __FolderItem_FWD_DEFINED__
typedef interface FolderItem FolderItem;
#endif 	/* __FolderItem_FWD_DEFINED__ */


#ifndef __FolderItems_FWD_DEFINED__
#define __FolderItems_FWD_DEFINED__
typedef interface FolderItems FolderItems;
#endif 	/* __FolderItems_FWD_DEFINED__ */


#ifndef __FolderItemVerb_FWD_DEFINED__
#define __FolderItemVerb_FWD_DEFINED__
typedef interface FolderItemVerb FolderItemVerb;
#endif 	/* __FolderItemVerb_FWD_DEFINED__ */


#ifndef __FolderItemVerbs_FWD_DEFINED__
#define __FolderItemVerbs_FWD_DEFINED__
typedef interface FolderItemVerbs FolderItemVerbs;
#endif 	/* __FolderItemVerbs_FWD_DEFINED__ */


#ifndef __Folder_FWD_DEFINED__
#define __Folder_FWD_DEFINED__
typedef interface Folder Folder;
#endif 	/* __Folder_FWD_DEFINED__ */


#ifndef __Folder2_FWD_DEFINED__
#define __Folder2_FWD_DEFINED__
typedef interface Folder2 Folder2;
#endif 	/* __Folder2_FWD_DEFINED__ */


#ifndef __Folder3_FWD_DEFINED__
#define __Folder3_FWD_DEFINED__
typedef interface Folder3 Folder3;
#endif 	/* __Folder3_FWD_DEFINED__ */


#ifndef __FolderItem2_FWD_DEFINED__
#define __FolderItem2_FWD_DEFINED__
typedef interface FolderItem2 FolderItem2;
#endif 	/* __FolderItem2_FWD_DEFINED__ */


#ifndef __ShellFolderItem_FWD_DEFINED__
#define __ShellFolderItem_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShellFolderItem ShellFolderItem;
#else
typedef struct ShellFolderItem ShellFolderItem;
#endif /* __cplusplus */

#endif 	/* __ShellFolderItem_FWD_DEFINED__ */


#ifndef __FolderItems2_FWD_DEFINED__
#define __FolderItems2_FWD_DEFINED__
typedef interface FolderItems2 FolderItems2;
#endif 	/* __FolderItems2_FWD_DEFINED__ */


#ifndef __FolderItems3_FWD_DEFINED__
#define __FolderItems3_FWD_DEFINED__
typedef interface FolderItems3 FolderItems3;
#endif 	/* __FolderItems3_FWD_DEFINED__ */


#ifndef __IShellLinkDual_FWD_DEFINED__
#define __IShellLinkDual_FWD_DEFINED__
typedef interface IShellLinkDual IShellLinkDual;
#endif 	/* __IShellLinkDual_FWD_DEFINED__ */


#ifndef __IShellLinkDual2_FWD_DEFINED__
#define __IShellLinkDual2_FWD_DEFINED__
typedef interface IShellLinkDual2 IShellLinkDual2;
#endif 	/* __IShellLinkDual2_FWD_DEFINED__ */


#ifndef __ShellLinkObject_FWD_DEFINED__
#define __ShellLinkObject_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShellLinkObject ShellLinkObject;
#else
typedef struct ShellLinkObject ShellLinkObject;
#endif /* __cplusplus */

#endif 	/* __ShellLinkObject_FWD_DEFINED__ */


#ifndef __IShellFolderViewDual_FWD_DEFINED__
#define __IShellFolderViewDual_FWD_DEFINED__
typedef interface IShellFolderViewDual IShellFolderViewDual;
#endif 	/* __IShellFolderViewDual_FWD_DEFINED__ */


#ifndef __IShellFolderViewDual2_FWD_DEFINED__
#define __IShellFolderViewDual2_FWD_DEFINED__
typedef interface IShellFolderViewDual2 IShellFolderViewDual2;
#endif 	/* __IShellFolderViewDual2_FWD_DEFINED__ */


#ifndef __ShellFolderView_FWD_DEFINED__
#define __ShellFolderView_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShellFolderView ShellFolderView;
#else
typedef struct ShellFolderView ShellFolderView;
#endif /* __cplusplus */

#endif 	/* __ShellFolderView_FWD_DEFINED__ */


#ifndef __IShellDispatch_FWD_DEFINED__
#define __IShellDispatch_FWD_DEFINED__
typedef interface IShellDispatch IShellDispatch;
#endif 	/* __IShellDispatch_FWD_DEFINED__ */


#ifndef __IShellDispatch2_FWD_DEFINED__
#define __IShellDispatch2_FWD_DEFINED__
typedef interface IShellDispatch2 IShellDispatch2;
#endif 	/* __IShellDispatch2_FWD_DEFINED__ */


#ifndef __IShellDispatch3_FWD_DEFINED__
#define __IShellDispatch3_FWD_DEFINED__
typedef interface IShellDispatch3 IShellDispatch3;
#endif 	/* __IShellDispatch3_FWD_DEFINED__ */


#ifndef __IShellDispatch4_FWD_DEFINED__
#define __IShellDispatch4_FWD_DEFINED__
typedef interface IShellDispatch4 IShellDispatch4;
#endif 	/* __IShellDispatch4_FWD_DEFINED__ */


#ifndef __Shell_FWD_DEFINED__
#define __Shell_FWD_DEFINED__

#ifdef __cplusplus
typedef class Shell Shell;
#else
typedef struct Shell Shell;
#endif /* __cplusplus */

#endif 	/* __Shell_FWD_DEFINED__ */


#ifndef __ShellDispatchInproc_FWD_DEFINED__
#define __ShellDispatchInproc_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShellDispatchInproc ShellDispatchInproc;
#else
typedef struct ShellDispatchInproc ShellDispatchInproc;
#endif /* __cplusplus */

#endif 	/* __ShellDispatchInproc_FWD_DEFINED__ */


#ifndef __WebViewFolderContents_FWD_DEFINED__
#define __WebViewFolderContents_FWD_DEFINED__

#ifdef __cplusplus
typedef class WebViewFolderContents WebViewFolderContents;
#else
typedef struct WebViewFolderContents WebViewFolderContents;
#endif /* __cplusplus */

#endif 	/* __WebViewFolderContents_FWD_DEFINED__ */


#ifndef __DSearchCommandEvents_FWD_DEFINED__
#define __DSearchCommandEvents_FWD_DEFINED__
typedef interface DSearchCommandEvents DSearchCommandEvents;
#endif 	/* __DSearchCommandEvents_FWD_DEFINED__ */


#ifndef __SearchCommand_FWD_DEFINED__
#define __SearchCommand_FWD_DEFINED__

#ifdef __cplusplus
typedef class SearchCommand SearchCommand;
#else
typedef struct SearchCommand SearchCommand;
#endif /* __cplusplus */

#endif 	/* __SearchCommand_FWD_DEFINED__ */


#ifndef __IFileSearchBand_FWD_DEFINED__
#define __IFileSearchBand_FWD_DEFINED__
typedef interface IFileSearchBand IFileSearchBand;
#endif 	/* __IFileSearchBand_FWD_DEFINED__ */


#ifndef __FileSearchBand_FWD_DEFINED__
#define __FileSearchBand_FWD_DEFINED__

#ifdef __cplusplus
typedef class FileSearchBand FileSearchBand;
#else
typedef struct FileSearchBand FileSearchBand;
#endif /* __cplusplus */

#endif 	/* __FileSearchBand_FWD_DEFINED__ */


#ifndef __IWebWizardHost_FWD_DEFINED__
#define __IWebWizardHost_FWD_DEFINED__
typedef interface IWebWizardHost IWebWizardHost;
#endif 	/* __IWebWizardHost_FWD_DEFINED__ */


#ifndef __INewWDEvents_FWD_DEFINED__
#define __INewWDEvents_FWD_DEFINED__
typedef interface INewWDEvents INewWDEvents;
#endif 	/* __INewWDEvents_FWD_DEFINED__ */


#ifndef __IPassportClientServices_FWD_DEFINED__
#define __IPassportClientServices_FWD_DEFINED__
typedef interface IPassportClientServices IPassportClientServices;
#endif 	/* __IPassportClientServices_FWD_DEFINED__ */


#ifndef __PassportClientServices_FWD_DEFINED__
#define __PassportClientServices_FWD_DEFINED__

#ifdef __cplusplus
typedef class PassportClientServices PassportClientServices;
#else
typedef struct PassportClientServices PassportClientServices;
#endif /* __cplusplus */

#endif 	/* __PassportClientServices_FWD_DEFINED__ */


#ifndef __IAutoComplete_FWD_DEFINED__
#define __IAutoComplete_FWD_DEFINED__
typedef interface IAutoComplete IAutoComplete;
#endif 	/* __IAutoComplete_FWD_DEFINED__ */


#ifndef __IAutoComplete2_FWD_DEFINED__
#define __IAutoComplete2_FWD_DEFINED__
typedef interface IAutoComplete2 IAutoComplete2;
#endif 	/* __IAutoComplete2_FWD_DEFINED__ */


#ifndef __IEnumACString_FWD_DEFINED__
#define __IEnumACString_FWD_DEFINED__
typedef interface IEnumACString IEnumACString;
#endif 	/* __IEnumACString_FWD_DEFINED__ */


#ifndef __IAsyncOperation_FWD_DEFINED__
#define __IAsyncOperation_FWD_DEFINED__
typedef interface IAsyncOperation IAsyncOperation;
#endif 	/* __IAsyncOperation_FWD_DEFINED__ */


/* header files for imported files */
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_shldisp_0000 */
/* [local] */ 




extern RPC_IF_HANDLE __MIDL_itf_shldisp_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shldisp_0000_v0_0_s_ifspec;


#ifndef __Shell32_LIBRARY_DEFINED__
#define __Shell32_LIBRARY_DEFINED__

/* library Shell32 */
/* [version][lcid][helpstring][uuid] */ 

typedef /* [helpstring][uuid] */  DECLSPEC_UUID("418f4e6a-b903-11d1-b0a6-00c04fc33aa5") 
enum SearchCommandExecuteErrors
    {	SCEE_PATHNOTFOUND	= 1,
	SCEE_MAXFILESFOUND	= SCEE_PATHNOTFOUND + 1,
	SCEE_INDEXSEARCH	= SCEE_MAXFILESFOUND + 1,
	SCEE_CONSTRAINT	= SCEE_INDEXSEARCH + 1,
	SCEE_SCOPEMISMATCH	= SCEE_CONSTRAINT + 1,
	SCEE_CASESENINDEX	= SCEE_SCOPEMISMATCH + 1,
	SCEE_INDEXNOTCOMPLETE	= SCEE_CASESENINDEX + 1
    } 	SearchCommandExecuteErrors;





typedef /* [helpstring][uuid] */  DECLSPEC_UUID("35f1a0d0-3e9a-11d2-8499-005345000000") 
enum OfflineFolderStatus
    {	OFS_INACTIVE	= -1,
	OFS_ONLINE	= OFS_INACTIVE + 1,
	OFS_OFFLINE	= OFS_ONLINE + 1,
	OFS_SERVERBACK	= OFS_OFFLINE + 1,
	OFS_DIRTYCACHE	= OFS_SERVERBACK + 1
    } 	OfflineFolderStatus;

typedef /* [helpstring][uuid] */  DECLSPEC_UUID("742A99A0-C77E-11D0-A32C-00A0C91EEDBA") 
enum ShellFolderViewOptions
    {	SFVVO_SHOWALLOBJECTS	= 0x1,
	SFVVO_SHOWEXTENSIONS	= 0x2,
	SFVVO_SHOWCOMPCOLOR	= 0x8,
	SFVVO_SHOWSYSFILES	= 0x20,
	SFVVO_WIN95CLASSIC	= 0x40,
	SFVVO_DOUBLECLICKINWEBVIEW	= 0x80,
	SFVVO_DESKTOPHTML	= 0x200
    } 	ShellFolderViewOptions;

typedef /* [helpstring][uuid] */  DECLSPEC_UUID("CA31EA20-48D0-11CF-8350-444553540000") 
enum ShellSpecialFolderConstants
    {	ssfDESKTOP	= 0,
	ssfPROGRAMS	= 0x2,
	ssfCONTROLS	= 0x3,
	ssfPRINTERS	= 0x4,
	ssfPERSONAL	= 0x5,
	ssfFAVORITES	= 0x6,
	ssfSTARTUP	= 0x7,
	ssfRECENT	= 0x8,
	ssfSENDTO	= 0x9,
	ssfBITBUCKET	= 0xa,
	ssfSTARTMENU	= 0xb,
	ssfDESKTOPDIRECTORY	= 0x10,
	ssfDRIVES	= 0x11,
	ssfNETWORK	= 0x12,
	ssfNETHOOD	= 0x13,
	ssfFONTS	= 0x14,
	ssfTEMPLATES	= 0x15,
	ssfCOMMONSTARTMENU	= 0x16,
	ssfCOMMONPROGRAMS	= 0x17,
	ssfCOMMONSTARTUP	= 0x18,
	ssfCOMMONDESKTOPDIR	= 0x19,
	ssfAPPDATA	= 0x1a,
	ssfPRINTHOOD	= 0x1b,
	ssfLOCALAPPDATA	= 0x1c,
	ssfALTSTARTUP	= 0x1d,
	ssfCOMMONALTSTARTUP	= 0x1e,
	ssfCOMMONFAVORITES	= 0x1f,
	ssfINTERNETCACHE	= 0x20,
	ssfCOOKIES	= 0x21,
	ssfHISTORY	= 0x22,
	ssfCOMMONAPPDATA	= 0x23,
	ssfWINDOWS	= 0x24,
	ssfSYSTEM	= 0x25,
	ssfPROGRAMFILES	= 0x26,
	ssfMYPICTURES	= 0x27,
	ssfPROFILE	= 0x28,
	ssfSYSTEMx86	= 0x29,
	ssfPROGRAMFILESx86	= 0x30
    } 	ShellSpecialFolderConstants;


EXTERN_C const IID LIBID_Shell32;

#ifndef __IFolderViewOC_INTERFACE_DEFINED__
#define __IFolderViewOC_INTERFACE_DEFINED__

/* interface IFolderViewOC */
/* [object][dual][oleautomation][hidden][helpcontext][helpstring][uuid] */ 


EXTERN_C const IID IID_IFolderViewOC;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9BA05970-F6A8-11CF-A442-00A0C90A8F39")
    IFolderViewOC : public IDispatch
    {
    public:
        virtual /* [helpcontext][helpstring] */ HRESULT STDMETHODCALLTYPE SetFolderView( 
            /* [in] */ IDispatch *pdisp) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFolderViewOCVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFolderViewOC * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFolderViewOC * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFolderViewOC * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFolderViewOC * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFolderViewOC * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFolderViewOC * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFolderViewOC * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpcontext][helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetFolderView )( 
            IFolderViewOC * This,
            /* [in] */ IDispatch *pdisp);
        
        END_INTERFACE
    } IFolderViewOCVtbl;

    interface IFolderViewOC
    {
        CONST_VTBL struct IFolderViewOCVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFolderViewOC_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFolderViewOC_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFolderViewOC_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IFolderViewOC_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IFolderViewOC_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IFolderViewOC_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IFolderViewOC_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IFolderViewOC_SetFolderView(This,pdisp)	\
    (This)->lpVtbl -> SetFolderView(This,pdisp)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpcontext][helpstring] */ HRESULT STDMETHODCALLTYPE IFolderViewOC_SetFolderView_Proxy( 
    IFolderViewOC * This,
    /* [in] */ IDispatch *pdisp);


void __RPC_STUB IFolderViewOC_SetFolderView_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFolderViewOC_INTERFACE_DEFINED__ */


#ifndef __DShellFolderViewEvents_DISPINTERFACE_DEFINED__
#define __DShellFolderViewEvents_DISPINTERFACE_DEFINED__

/* dispinterface DShellFolderViewEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID_DShellFolderViewEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("62112AA2-EBE4-11cf-A5FB-0020AFE7292D")
    DShellFolderViewEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct DShellFolderViewEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            DShellFolderViewEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            DShellFolderViewEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            DShellFolderViewEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            DShellFolderViewEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            DShellFolderViewEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            DShellFolderViewEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            DShellFolderViewEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } DShellFolderViewEventsVtbl;

    interface DShellFolderViewEvents
    {
        CONST_VTBL struct DShellFolderViewEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define DShellFolderViewEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define DShellFolderViewEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define DShellFolderViewEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define DShellFolderViewEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define DShellFolderViewEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define DShellFolderViewEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define DShellFolderViewEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __DShellFolderViewEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ShellFolderViewOC;

#ifdef __cplusplus

class DECLSPEC_UUID("9BA05971-F6A8-11CF-A442-00A0C90A8F39")
ShellFolderViewOC;
#endif

#ifndef __DFConstraint_INTERFACE_DEFINED__
#define __DFConstraint_INTERFACE_DEFINED__

/* interface DFConstraint */
/* [object][dual][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_DFConstraint;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4a3df050-23bd-11d2-939f-00a0c91eedba")
    DFConstraint : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pbs) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [retval][out] */ VARIANT *pv) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct DFConstraintVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            DFConstraint * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            DFConstraint * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            DFConstraint * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            DFConstraint * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            DFConstraint * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            DFConstraint * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            DFConstraint * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            DFConstraint * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Value )( 
            DFConstraint * This,
            /* [retval][out] */ VARIANT *pv);
        
        END_INTERFACE
    } DFConstraintVtbl;

    interface DFConstraint
    {
        CONST_VTBL struct DFConstraintVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define DFConstraint_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define DFConstraint_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define DFConstraint_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define DFConstraint_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define DFConstraint_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define DFConstraint_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define DFConstraint_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define DFConstraint_get_Name(This,pbs)	\
    (This)->lpVtbl -> get_Name(This,pbs)

#define DFConstraint_get_Value(This,pv)	\
    (This)->lpVtbl -> get_Value(This,pv)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE DFConstraint_get_Name_Proxy( 
    DFConstraint * This,
    /* [retval][out] */ BSTR *pbs);


void __RPC_STUB DFConstraint_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE DFConstraint_get_Value_Proxy( 
    DFConstraint * This,
    /* [retval][out] */ VARIANT *pv);


void __RPC_STUB DFConstraint_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __DFConstraint_INTERFACE_DEFINED__ */


#ifndef __ISearchCommandExt_INTERFACE_DEFINED__
#define __ISearchCommandExt_INTERFACE_DEFINED__

/* interface ISearchCommandExt */
/* [object][oleautomation][dual][helpstring][uuid] */ 

typedef /* [public] */ 
enum __MIDL_ISearchCommandExt_0001
    {	SCE_SEARCHFORFILES	= 0,
	SCE_SEARCHFORCOMPUTERS	= 1
    } 	SEARCH_FOR_TYPE;


EXTERN_C const IID IID_ISearchCommandExt;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1D2EFD50-75CE-11d1-B75A-00A0C90564FE")
    ISearchCommandExt : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ClearResults( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE NavigateToSearchResults( void) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ProgressText( 
            /* [retval][out] */ BSTR *pbs) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveSearch( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetErrorInfo( 
            /* [out] */ BSTR *pbs,
            /* [retval][out] */ int *phr) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SearchFor( 
            /* [in] */ int iFor) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetScopeInfo( 
            /* [in] */ BSTR bsScope,
            /* [out] */ int *pdwScopeInfo) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RestoreSavedSearch( 
            /* [in] */ VARIANT *pvarFile) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Execute( 
            /* [optional][in] */ VARIANT *RecordsAffected,
            /* [optional][in] */ VARIANT *Parameters,
            /* [optional][in] */ long Options) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddConstraint( 
            /* [in] */ BSTR Name,
            /* [in] */ VARIANT Value) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetNextConstraint( 
            /* [in] */ VARIANT_BOOL fReset,
            /* [retval][out] */ DFConstraint **ppdfc) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISearchCommandExtVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISearchCommandExt * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISearchCommandExt * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISearchCommandExt * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISearchCommandExt * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISearchCommandExt * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISearchCommandExt * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISearchCommandExt * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ClearResults )( 
            ISearchCommandExt * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *NavigateToSearchResults )( 
            ISearchCommandExt * This);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ProgressText )( 
            ISearchCommandExt * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SaveSearch )( 
            ISearchCommandExt * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetErrorInfo )( 
            ISearchCommandExt * This,
            /* [out] */ BSTR *pbs,
            /* [retval][out] */ int *phr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SearchFor )( 
            ISearchCommandExt * This,
            /* [in] */ int iFor);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetScopeInfo )( 
            ISearchCommandExt * This,
            /* [in] */ BSTR bsScope,
            /* [out] */ int *pdwScopeInfo);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RestoreSavedSearch )( 
            ISearchCommandExt * This,
            /* [in] */ VARIANT *pvarFile);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Execute )( 
            ISearchCommandExt * This,
            /* [optional][in] */ VARIANT *RecordsAffected,
            /* [optional][in] */ VARIANT *Parameters,
            /* [optional][in] */ long Options);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddConstraint )( 
            ISearchCommandExt * This,
            /* [in] */ BSTR Name,
            /* [in] */ VARIANT Value);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetNextConstraint )( 
            ISearchCommandExt * This,
            /* [in] */ VARIANT_BOOL fReset,
            /* [retval][out] */ DFConstraint **ppdfc);
        
        END_INTERFACE
    } ISearchCommandExtVtbl;

    interface ISearchCommandExt
    {
        CONST_VTBL struct ISearchCommandExtVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISearchCommandExt_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISearchCommandExt_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISearchCommandExt_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISearchCommandExt_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISearchCommandExt_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISearchCommandExt_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISearchCommandExt_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISearchCommandExt_ClearResults(This)	\
    (This)->lpVtbl -> ClearResults(This)

#define ISearchCommandExt_NavigateToSearchResults(This)	\
    (This)->lpVtbl -> NavigateToSearchResults(This)

#define ISearchCommandExt_get_ProgressText(This,pbs)	\
    (This)->lpVtbl -> get_ProgressText(This,pbs)

#define ISearchCommandExt_SaveSearch(This)	\
    (This)->lpVtbl -> SaveSearch(This)

#define ISearchCommandExt_GetErrorInfo(This,pbs,phr)	\
    (This)->lpVtbl -> GetErrorInfo(This,pbs,phr)

#define ISearchCommandExt_SearchFor(This,iFor)	\
    (This)->lpVtbl -> SearchFor(This,iFor)

#define ISearchCommandExt_GetScopeInfo(This,bsScope,pdwScopeInfo)	\
    (This)->lpVtbl -> GetScopeInfo(This,bsScope,pdwScopeInfo)

#define ISearchCommandExt_RestoreSavedSearch(This,pvarFile)	\
    (This)->lpVtbl -> RestoreSavedSearch(This,pvarFile)

#define ISearchCommandExt_Execute(This,RecordsAffected,Parameters,Options)	\
    (This)->lpVtbl -> Execute(This,RecordsAffected,Parameters,Options)

#define ISearchCommandExt_AddConstraint(This,Name,Value)	\
    (This)->lpVtbl -> AddConstraint(This,Name,Value)

#define ISearchCommandExt_GetNextConstraint(This,fReset,ppdfc)	\
    (This)->lpVtbl -> GetNextConstraint(This,fReset,ppdfc)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISearchCommandExt_ClearResults_Proxy( 
    ISearchCommandExt * This);


void __RPC_STUB ISearchCommandExt_ClearResults_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISearchCommandExt_NavigateToSearchResults_Proxy( 
    ISearchCommandExt * This);


void __RPC_STUB ISearchCommandExt_NavigateToSearchResults_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE ISearchCommandExt_get_ProgressText_Proxy( 
    ISearchCommandExt * This,
    /* [retval][out] */ BSTR *pbs);


void __RPC_STUB ISearchCommandExt_get_ProgressText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISearchCommandExt_SaveSearch_Proxy( 
    ISearchCommandExt * This);


void __RPC_STUB ISearchCommandExt_SaveSearch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISearchCommandExt_GetErrorInfo_Proxy( 
    ISearchCommandExt * This,
    /* [out] */ BSTR *pbs,
    /* [retval][out] */ int *phr);


void __RPC_STUB ISearchCommandExt_GetErrorInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISearchCommandExt_SearchFor_Proxy( 
    ISearchCommandExt * This,
    /* [in] */ int iFor);


void __RPC_STUB ISearchCommandExt_SearchFor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISearchCommandExt_GetScopeInfo_Proxy( 
    ISearchCommandExt * This,
    /* [in] */ BSTR bsScope,
    /* [out] */ int *pdwScopeInfo);


void __RPC_STUB ISearchCommandExt_GetScopeInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISearchCommandExt_RestoreSavedSearch_Proxy( 
    ISearchCommandExt * This,
    /* [in] */ VARIANT *pvarFile);


void __RPC_STUB ISearchCommandExt_RestoreSavedSearch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISearchCommandExt_Execute_Proxy( 
    ISearchCommandExt * This,
    /* [optional][in] */ VARIANT *RecordsAffected,
    /* [optional][in] */ VARIANT *Parameters,
    /* [optional][in] */ long Options);


void __RPC_STUB ISearchCommandExt_Execute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISearchCommandExt_AddConstraint_Proxy( 
    ISearchCommandExt * This,
    /* [in] */ BSTR Name,
    /* [in] */ VARIANT Value);


void __RPC_STUB ISearchCommandExt_AddConstraint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISearchCommandExt_GetNextConstraint_Proxy( 
    ISearchCommandExt * This,
    /* [in] */ VARIANT_BOOL fReset,
    /* [retval][out] */ DFConstraint **ppdfc);


void __RPC_STUB ISearchCommandExt_GetNextConstraint_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISearchCommandExt_INTERFACE_DEFINED__ */


#ifndef __FolderItem_INTERFACE_DEFINED__
#define __FolderItem_INTERFACE_DEFINED__

/* interface FolderItem */
/* [object][dual][oleautomation][helpstring][uuid] */ 

typedef /* [unique] */ FolderItem *LPFOLDERITEM;


EXTERN_C const IID IID_FolderItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FAC32C80-CBE4-11CE-8350-444553540000")
    FolderItem : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IDispatch **ppid) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Parent( 
            /* [retval][out] */ IDispatch **ppid) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pbs) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR bs) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Path( 
            /* [retval][out] */ BSTR *pbs) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_GetLink( 
            /* [retval][out] */ IDispatch **ppid) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_GetFolder( 
            /* [retval][out] */ IDispatch **ppid) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsLink( 
            /* [retval][out] */ VARIANT_BOOL *pb) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsFolder( 
            /* [retval][out] */ VARIANT_BOOL *pb) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsFileSystem( 
            /* [retval][out] */ VARIANT_BOOL *pb) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_IsBrowsable( 
            /* [retval][out] */ VARIANT_BOOL *pb) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ModifyDate( 
            /* [retval][out] */ DATE *pdt) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ModifyDate( 
            /* [in] */ DATE dt) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Size( 
            /* [retval][out] */ LONG *pul) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ BSTR *pbs) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Verbs( 
            /* [retval][out] */ FolderItemVerbs **ppfic) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE InvokeVerb( 
            /* [optional][in] */ VARIANT vVerb) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct FolderItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            FolderItem * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            FolderItem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            FolderItem * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            FolderItem * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            FolderItem * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            FolderItem * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            FolderItem * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            FolderItem * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parent )( 
            FolderItem * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            FolderItem * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            FolderItem * This,
            /* [in] */ BSTR bs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Path )( 
            FolderItem * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_GetLink )( 
            FolderItem * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_GetFolder )( 
            FolderItem * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsLink )( 
            FolderItem * This,
            /* [retval][out] */ VARIANT_BOOL *pb);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsFolder )( 
            FolderItem * This,
            /* [retval][out] */ VARIANT_BOOL *pb);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsFileSystem )( 
            FolderItem * This,
            /* [retval][out] */ VARIANT_BOOL *pb);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsBrowsable )( 
            FolderItem * This,
            /* [retval][out] */ VARIANT_BOOL *pb);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ModifyDate )( 
            FolderItem * This,
            /* [retval][out] */ DATE *pdt);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ModifyDate )( 
            FolderItem * This,
            /* [in] */ DATE dt);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            FolderItem * This,
            /* [retval][out] */ LONG *pul);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Type )( 
            FolderItem * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Verbs )( 
            FolderItem * This,
            /* [retval][out] */ FolderItemVerbs **ppfic);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *InvokeVerb )( 
            FolderItem * This,
            /* [optional][in] */ VARIANT vVerb);
        
        END_INTERFACE
    } FolderItemVtbl;

    interface FolderItem
    {
        CONST_VTBL struct FolderItemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define FolderItem_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define FolderItem_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define FolderItem_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define FolderItem_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define FolderItem_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define FolderItem_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define FolderItem_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define FolderItem_get_Application(This,ppid)	\
    (This)->lpVtbl -> get_Application(This,ppid)

#define FolderItem_get_Parent(This,ppid)	\
    (This)->lpVtbl -> get_Parent(This,ppid)

#define FolderItem_get_Name(This,pbs)	\
    (This)->lpVtbl -> get_Name(This,pbs)

#define FolderItem_put_Name(This,bs)	\
    (This)->lpVtbl -> put_Name(This,bs)

#define FolderItem_get_Path(This,pbs)	\
    (This)->lpVtbl -> get_Path(This,pbs)

#define FolderItem_get_GetLink(This,ppid)	\
    (This)->lpVtbl -> get_GetLink(This,ppid)

#define FolderItem_get_GetFolder(This,ppid)	\
    (This)->lpVtbl -> get_GetFolder(This,ppid)

#define FolderItem_get_IsLink(This,pb)	\
    (This)->lpVtbl -> get_IsLink(This,pb)

#define FolderItem_get_IsFolder(This,pb)	\
    (This)->lpVtbl -> get_IsFolder(This,pb)

#define FolderItem_get_IsFileSystem(This,pb)	\
    (This)->lpVtbl -> get_IsFileSystem(This,pb)

#define FolderItem_get_IsBrowsable(This,pb)	\
    (This)->lpVtbl -> get_IsBrowsable(This,pb)

#define FolderItem_get_ModifyDate(This,pdt)	\
    (This)->lpVtbl -> get_ModifyDate(This,pdt)

#define FolderItem_put_ModifyDate(This,dt)	\
    (This)->lpVtbl -> put_ModifyDate(This,dt)

#define FolderItem_get_Size(This,pul)	\
    (This)->lpVtbl -> get_Size(This,pul)

#define FolderItem_get_Type(This,pbs)	\
    (This)->lpVtbl -> get_Type(This,pbs)

#define FolderItem_Verbs(This,ppfic)	\
    (This)->lpVtbl -> Verbs(This,ppfic)

#define FolderItem_InvokeVerb(This,vVerb)	\
    (This)->lpVtbl -> InvokeVerb(This,vVerb)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItem_get_Application_Proxy( 
    FolderItem * This,
    /* [retval][out] */ IDispatch **ppid);


void __RPC_STUB FolderItem_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItem_get_Parent_Proxy( 
    FolderItem * This,
    /* [retval][out] */ IDispatch **ppid);


void __RPC_STUB FolderItem_get_Parent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE FolderItem_get_Name_Proxy( 
    FolderItem * This,
    /* [retval][out] */ BSTR *pbs);


void __RPC_STUB FolderItem_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE FolderItem_put_Name_Proxy( 
    FolderItem * This,
    /* [in] */ BSTR bs);


void __RPC_STUB FolderItem_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItem_get_Path_Proxy( 
    FolderItem * This,
    /* [retval][out] */ BSTR *pbs);


void __RPC_STUB FolderItem_get_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItem_get_GetLink_Proxy( 
    FolderItem * This,
    /* [retval][out] */ IDispatch **ppid);


void __RPC_STUB FolderItem_get_GetLink_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItem_get_GetFolder_Proxy( 
    FolderItem * This,
    /* [retval][out] */ IDispatch **ppid);


void __RPC_STUB FolderItem_get_GetFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItem_get_IsLink_Proxy( 
    FolderItem * This,
    /* [retval][out] */ VARIANT_BOOL *pb);


void __RPC_STUB FolderItem_get_IsLink_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItem_get_IsFolder_Proxy( 
    FolderItem * This,
    /* [retval][out] */ VARIANT_BOOL *pb);


void __RPC_STUB FolderItem_get_IsFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItem_get_IsFileSystem_Proxy( 
    FolderItem * This,
    /* [retval][out] */ VARIANT_BOOL *pb);


void __RPC_STUB FolderItem_get_IsFileSystem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItem_get_IsBrowsable_Proxy( 
    FolderItem * This,
    /* [retval][out] */ VARIANT_BOOL *pb);


void __RPC_STUB FolderItem_get_IsBrowsable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItem_get_ModifyDate_Proxy( 
    FolderItem * This,
    /* [retval][out] */ DATE *pdt);


void __RPC_STUB FolderItem_get_ModifyDate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE FolderItem_put_ModifyDate_Proxy( 
    FolderItem * This,
    /* [in] */ DATE dt);


void __RPC_STUB FolderItem_put_ModifyDate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItem_get_Size_Proxy( 
    FolderItem * This,
    /* [retval][out] */ LONG *pul);


void __RPC_STUB FolderItem_get_Size_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItem_get_Type_Proxy( 
    FolderItem * This,
    /* [retval][out] */ BSTR *pbs);


void __RPC_STUB FolderItem_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE FolderItem_Verbs_Proxy( 
    FolderItem * This,
    /* [retval][out] */ FolderItemVerbs **ppfic);


void __RPC_STUB FolderItem_Verbs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE FolderItem_InvokeVerb_Proxy( 
    FolderItem * This,
    /* [optional][in] */ VARIANT vVerb);


void __RPC_STUB FolderItem_InvokeVerb_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __FolderItem_INTERFACE_DEFINED__ */


#ifndef __FolderItems_INTERFACE_DEFINED__
#define __FolderItems_INTERFACE_DEFINED__

/* interface FolderItems */
/* [object][dual][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_FolderItems;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("744129E0-CBE5-11CE-8350-444553540000")
    FolderItems : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *plCount) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IDispatch **ppid) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Parent( 
            /* [retval][out] */ IDispatch **ppid) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [optional][in] */ VARIANT index,
            /* [retval][out] */ FolderItem **ppid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE _NewEnum( 
            /* [retval][out] */ IUnknown **ppunk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct FolderItemsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            FolderItems * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            FolderItems * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            FolderItems * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            FolderItems * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            FolderItems * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            FolderItems * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            FolderItems * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            FolderItems * This,
            /* [retval][out] */ long *plCount);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            FolderItems * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parent )( 
            FolderItems * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            FolderItems * This,
            /* [optional][in] */ VARIANT index,
            /* [retval][out] */ FolderItem **ppid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *_NewEnum )( 
            FolderItems * This,
            /* [retval][out] */ IUnknown **ppunk);
        
        END_INTERFACE
    } FolderItemsVtbl;

    interface FolderItems
    {
        CONST_VTBL struct FolderItemsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define FolderItems_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define FolderItems_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define FolderItems_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define FolderItems_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define FolderItems_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define FolderItems_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define FolderItems_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define FolderItems_get_Count(This,plCount)	\
    (This)->lpVtbl -> get_Count(This,plCount)

#define FolderItems_get_Application(This,ppid)	\
    (This)->lpVtbl -> get_Application(This,ppid)

#define FolderItems_get_Parent(This,ppid)	\
    (This)->lpVtbl -> get_Parent(This,ppid)

#define FolderItems_Item(This,index,ppid)	\
    (This)->lpVtbl -> Item(This,index,ppid)

#define FolderItems__NewEnum(This,ppunk)	\
    (This)->lpVtbl -> _NewEnum(This,ppunk)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItems_get_Count_Proxy( 
    FolderItems * This,
    /* [retval][out] */ long *plCount);


void __RPC_STUB FolderItems_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItems_get_Application_Proxy( 
    FolderItems * This,
    /* [retval][out] */ IDispatch **ppid);


void __RPC_STUB FolderItems_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItems_get_Parent_Proxy( 
    FolderItems * This,
    /* [retval][out] */ IDispatch **ppid);


void __RPC_STUB FolderItems_get_Parent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE FolderItems_Item_Proxy( 
    FolderItems * This,
    /* [optional][in] */ VARIANT index,
    /* [retval][out] */ FolderItem **ppid);


void __RPC_STUB FolderItems_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FolderItems__NewEnum_Proxy( 
    FolderItems * This,
    /* [retval][out] */ IUnknown **ppunk);


void __RPC_STUB FolderItems__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __FolderItems_INTERFACE_DEFINED__ */


#ifndef __FolderItemVerb_INTERFACE_DEFINED__
#define __FolderItemVerb_INTERFACE_DEFINED__

/* interface FolderItemVerb */
/* [object][dual][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_FolderItemVerb;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("08EC3E00-50B0-11CF-960C-0080C7F4EE85")
    FolderItemVerb : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IDispatch **ppid) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Parent( 
            /* [retval][out] */ IDispatch **ppid) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *pbs) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE DoIt( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct FolderItemVerbVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            FolderItemVerb * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            FolderItemVerb * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            FolderItemVerb * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            FolderItemVerb * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            FolderItemVerb * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            FolderItemVerb * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            FolderItemVerb * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            FolderItemVerb * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parent )( 
            FolderItemVerb * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            FolderItemVerb * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *DoIt )( 
            FolderItemVerb * This);
        
        END_INTERFACE
    } FolderItemVerbVtbl;

    interface FolderItemVerb
    {
        CONST_VTBL struct FolderItemVerbVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define FolderItemVerb_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define FolderItemVerb_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define FolderItemVerb_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define FolderItemVerb_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define FolderItemVerb_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define FolderItemVerb_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define FolderItemVerb_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define FolderItemVerb_get_Application(This,ppid)	\
    (This)->lpVtbl -> get_Application(This,ppid)

#define FolderItemVerb_get_Parent(This,ppid)	\
    (This)->lpVtbl -> get_Parent(This,ppid)

#define FolderItemVerb_get_Name(This,pbs)	\
    (This)->lpVtbl -> get_Name(This,pbs)

#define FolderItemVerb_DoIt(This)	\
    (This)->lpVtbl -> DoIt(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItemVerb_get_Application_Proxy( 
    FolderItemVerb * This,
    /* [retval][out] */ IDispatch **ppid);


void __RPC_STUB FolderItemVerb_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItemVerb_get_Parent_Proxy( 
    FolderItemVerb * This,
    /* [retval][out] */ IDispatch **ppid);


void __RPC_STUB FolderItemVerb_get_Parent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE FolderItemVerb_get_Name_Proxy( 
    FolderItemVerb * This,
    /* [retval][out] */ BSTR *pbs);


void __RPC_STUB FolderItemVerb_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE FolderItemVerb_DoIt_Proxy( 
    FolderItemVerb * This);


void __RPC_STUB FolderItemVerb_DoIt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __FolderItemVerb_INTERFACE_DEFINED__ */


#ifndef __FolderItemVerbs_INTERFACE_DEFINED__
#define __FolderItemVerbs_INTERFACE_DEFINED__

/* interface FolderItemVerbs */
/* [object][dual][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_FolderItemVerbs;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1F8352C0-50B0-11CF-960C-0080C7F4EE85")
    FolderItemVerbs : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *plCount) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IDispatch **ppid) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Parent( 
            /* [retval][out] */ IDispatch **ppid) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [optional][in] */ VARIANT index,
            /* [retval][out] */ FolderItemVerb **ppid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE _NewEnum( 
            /* [retval][out] */ IUnknown **ppunk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct FolderItemVerbsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            FolderItemVerbs * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            FolderItemVerbs * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            FolderItemVerbs * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            FolderItemVerbs * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            FolderItemVerbs * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            FolderItemVerbs * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            FolderItemVerbs * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            FolderItemVerbs * This,
            /* [retval][out] */ long *plCount);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            FolderItemVerbs * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parent )( 
            FolderItemVerbs * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            FolderItemVerbs * This,
            /* [optional][in] */ VARIANT index,
            /* [retval][out] */ FolderItemVerb **ppid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *_NewEnum )( 
            FolderItemVerbs * This,
            /* [retval][out] */ IUnknown **ppunk);
        
        END_INTERFACE
    } FolderItemVerbsVtbl;

    interface FolderItemVerbs
    {
        CONST_VTBL struct FolderItemVerbsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define FolderItemVerbs_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define FolderItemVerbs_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define FolderItemVerbs_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define FolderItemVerbs_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define FolderItemVerbs_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define FolderItemVerbs_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define FolderItemVerbs_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define FolderItemVerbs_get_Count(This,plCount)	\
    (This)->lpVtbl -> get_Count(This,plCount)

#define FolderItemVerbs_get_Application(This,ppid)	\
    (This)->lpVtbl -> get_Application(This,ppid)

#define FolderItemVerbs_get_Parent(This,ppid)	\
    (This)->lpVtbl -> get_Parent(This,ppid)

#define FolderItemVerbs_Item(This,index,ppid)	\
    (This)->lpVtbl -> Item(This,index,ppid)

#define FolderItemVerbs__NewEnum(This,ppunk)	\
    (This)->lpVtbl -> _NewEnum(This,ppunk)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItemVerbs_get_Count_Proxy( 
    FolderItemVerbs * This,
    /* [retval][out] */ long *plCount);


void __RPC_STUB FolderItemVerbs_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItemVerbs_get_Application_Proxy( 
    FolderItemVerbs * This,
    /* [retval][out] */ IDispatch **ppid);


void __RPC_STUB FolderItemVerbs_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE FolderItemVerbs_get_Parent_Proxy( 
    FolderItemVerbs * This,
    /* [retval][out] */ IDispatch **ppid);


void __RPC_STUB FolderItemVerbs_get_Parent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE FolderItemVerbs_Item_Proxy( 
    FolderItemVerbs * This,
    /* [optional][in] */ VARIANT index,
    /* [retval][out] */ FolderItemVerb **ppid);


void __RPC_STUB FolderItemVerbs_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FolderItemVerbs__NewEnum_Proxy( 
    FolderItemVerbs * This,
    /* [retval][out] */ IUnknown **ppunk);


void __RPC_STUB FolderItemVerbs__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __FolderItemVerbs_INTERFACE_DEFINED__ */


#ifndef __Folder_INTERFACE_DEFINED__
#define __Folder_INTERFACE_DEFINED__

/* interface Folder */
/* [object][dual][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_Folder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BBCBDE60-C3FF-11CE-8350-444553540000")
    Folder : public IDispatch
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Title( 
            /* [retval][out] */ BSTR *pbs) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IDispatch **ppid) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Parent( 
            /* [retval][out] */ IDispatch **ppid) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ParentFolder( 
            /* [retval][out] */ Folder **ppsf) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Items( 
            /* [retval][out] */ FolderItems **ppid) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ParseName( 
            /* [in] */ BSTR bName,
            /* [retval][out] */ FolderItem **ppid) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE NewFolder( 
            /* [in] */ BSTR bName,
            /* [optional][in] */ VARIANT vOptions) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE MoveHere( 
            /* [in] */ VARIANT vItem,
            /* [optional][in] */ VARIANT vOptions) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CopyHere( 
            /* [in] */ VARIANT vItem,
            /* [optional][in] */ VARIANT vOptions) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDetailsOf( 
            /* [in] */ VARIANT vItem,
            /* [in] */ int iColumn,
            /* [retval][out] */ BSTR *pbs) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct FolderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            Folder * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            Folder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            Folder * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            Folder * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            Folder * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            Folder * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            Folder * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Title )( 
            Folder * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            Folder * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parent )( 
            Folder * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ParentFolder )( 
            Folder * This,
            /* [retval][out] */ Folder **ppsf);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Items )( 
            Folder * This,
            /* [retval][out] */ FolderItems **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ParseName )( 
            Folder * This,
            /* [in] */ BSTR bName,
            /* [retval][out] */ FolderItem **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *NewFolder )( 
            Folder * This,
            /* [in] */ BSTR bName,
            /* [optional][in] */ VARIANT vOptions);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *MoveHere )( 
            Folder * This,
            /* [in] */ VARIANT vItem,
            /* [optional][in] */ VARIANT vOptions);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CopyHere )( 
            Folder * This,
            /* [in] */ VARIANT vItem,
            /* [optional][in] */ VARIANT vOptions);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetDetailsOf )( 
            Folder * This,
            /* [in] */ VARIANT vItem,
            /* [in] */ int iColumn,
            /* [retval][out] */ BSTR *pbs);
        
        END_INTERFACE
    } FolderVtbl;

    interface Folder
    {
        CONST_VTBL struct FolderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Folder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define Folder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define Folder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define Folder_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define Folder_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define Folder_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define Folder_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define Folder_get_Title(This,pbs)	\
    (This)->lpVtbl -> get_Title(This,pbs)

#define Folder_get_Application(This,ppid)	\
    (This)->lpVtbl -> get_Application(This,ppid)

#define Folder_get_Parent(This,ppid)	\
    (This)->lpVtbl -> get_Parent(This,ppid)

#define Folder_get_ParentFolder(This,ppsf)	\
    (This)->lpVtbl -> get_ParentFolder(This,ppsf)

#define Folder_Items(This,ppid)	\
    (This)->lpVtbl -> Items(This,ppid)

#define Folder_ParseName(This,bName,ppid)	\
    (This)->lpVtbl -> ParseName(This,bName,ppid)

#define Folder_NewFolder(This,bName,vOptions)	\
    (This)->lpVtbl -> NewFolder(This,bName,vOptions)

#define Folder_MoveHere(This,vItem,vOptions)	\
    (This)->lpVtbl -> MoveHere(This,vItem,vOptions)

#define Folder_CopyHere(This,vItem,vOptions)	\
    (This)->lpVtbl -> CopyHere(This,vItem,vOptions)

#define Folder_GetDetailsOf(This,vItem,iColumn,pbs)	\
    (This)->lpVtbl -> GetDetailsOf(This,vItem,iColumn,pbs)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE Folder_get_Title_Proxy( 
    Folder * This,
    /* [retval][out] */ BSTR *pbs);


void __RPC_STUB Folder_get_Title_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Folder_get_Application_Proxy( 
    Folder * This,
    /* [retval][out] */ IDispatch **ppid);


void __RPC_STUB Folder_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Folder_get_Parent_Proxy( 
    Folder * This,
    /* [retval][out] */ IDispatch **ppid);


void __RPC_STUB Folder_get_Parent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Folder_get_ParentFolder_Proxy( 
    Folder * This,
    /* [retval][out] */ Folder **ppsf);


void __RPC_STUB Folder_get_ParentFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Folder_Items_Proxy( 
    Folder * This,
    /* [retval][out] */ FolderItems **ppid);


void __RPC_STUB Folder_Items_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Folder_ParseName_Proxy( 
    Folder * This,
    /* [in] */ BSTR bName,
    /* [retval][out] */ FolderItem **ppid);


void __RPC_STUB Folder_ParseName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Folder_NewFolder_Proxy( 
    Folder * This,
    /* [in] */ BSTR bName,
    /* [optional][in] */ VARIANT vOptions);


void __RPC_STUB Folder_NewFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Folder_MoveHere_Proxy( 
    Folder * This,
    /* [in] */ VARIANT vItem,
    /* [optional][in] */ VARIANT vOptions);


void __RPC_STUB Folder_MoveHere_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Folder_CopyHere_Proxy( 
    Folder * This,
    /* [in] */ VARIANT vItem,
    /* [optional][in] */ VARIANT vOptions);


void __RPC_STUB Folder_CopyHere_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Folder_GetDetailsOf_Proxy( 
    Folder * This,
    /* [in] */ VARIANT vItem,
    /* [in] */ int iColumn,
    /* [retval][out] */ BSTR *pbs);


void __RPC_STUB Folder_GetDetailsOf_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __Folder_INTERFACE_DEFINED__ */


#ifndef __Folder2_INTERFACE_DEFINED__
#define __Folder2_INTERFACE_DEFINED__

/* interface Folder2 */
/* [object][dual][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_Folder2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("f0d2d8ef-3890-11d2-bf8b-00c04fb93661")
    Folder2 : public Folder
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Self( 
            /* [retval][out] */ FolderItem **ppfi) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_OfflineStatus( 
            /* [retval][out] */ LONG *pul) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Synchronize( void) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_HaveToShowWebViewBarricade( 
            /* [retval][out] */ VARIANT_BOOL *pbHaveToShowWebViewBarricade) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE DismissedWebViewBarricade( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct Folder2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            Folder2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            Folder2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            Folder2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            Folder2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            Folder2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            Folder2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            Folder2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Title )( 
            Folder2 * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            Folder2 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parent )( 
            Folder2 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ParentFolder )( 
            Folder2 * This,
            /* [retval][out] */ Folder **ppsf);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Items )( 
            Folder2 * This,
            /* [retval][out] */ FolderItems **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ParseName )( 
            Folder2 * This,
            /* [in] */ BSTR bName,
            /* [retval][out] */ FolderItem **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *NewFolder )( 
            Folder2 * This,
            /* [in] */ BSTR bName,
            /* [optional][in] */ VARIANT vOptions);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *MoveHere )( 
            Folder2 * This,
            /* [in] */ VARIANT vItem,
            /* [optional][in] */ VARIANT vOptions);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CopyHere )( 
            Folder2 * This,
            /* [in] */ VARIANT vItem,
            /* [optional][in] */ VARIANT vOptions);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetDetailsOf )( 
            Folder2 * This,
            /* [in] */ VARIANT vItem,
            /* [in] */ int iColumn,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Self )( 
            Folder2 * This,
            /* [retval][out] */ FolderItem **ppfi);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OfflineStatus )( 
            Folder2 * This,
            /* [retval][out] */ LONG *pul);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Synchronize )( 
            Folder2 * This);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_HaveToShowWebViewBarricade )( 
            Folder2 * This,
            /* [retval][out] */ VARIANT_BOOL *pbHaveToShowWebViewBarricade);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *DismissedWebViewBarricade )( 
            Folder2 * This);
        
        END_INTERFACE
    } Folder2Vtbl;

    interface Folder2
    {
        CONST_VTBL struct Folder2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Folder2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define Folder2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define Folder2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define Folder2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define Folder2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define Folder2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define Folder2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define Folder2_get_Title(This,pbs)	\
    (This)->lpVtbl -> get_Title(This,pbs)

#define Folder2_get_Application(This,ppid)	\
    (This)->lpVtbl -> get_Application(This,ppid)

#define Folder2_get_Parent(This,ppid)	\
    (This)->lpVtbl -> get_Parent(This,ppid)

#define Folder2_get_ParentFolder(This,ppsf)	\
    (This)->lpVtbl -> get_ParentFolder(This,ppsf)

#define Folder2_Items(This,ppid)	\
    (This)->lpVtbl -> Items(This,ppid)

#define Folder2_ParseName(This,bName,ppid)	\
    (This)->lpVtbl -> ParseName(This,bName,ppid)

#define Folder2_NewFolder(This,bName,vOptions)	\
    (This)->lpVtbl -> NewFolder(This,bName,vOptions)

#define Folder2_MoveHere(This,vItem,vOptions)	\
    (This)->lpVtbl -> MoveHere(This,vItem,vOptions)

#define Folder2_CopyHere(This,vItem,vOptions)	\
    (This)->lpVtbl -> CopyHere(This,vItem,vOptions)

#define Folder2_GetDetailsOf(This,vItem,iColumn,pbs)	\
    (This)->lpVtbl -> GetDetailsOf(This,vItem,iColumn,pbs)


#define Folder2_get_Self(This,ppfi)	\
    (This)->lpVtbl -> get_Self(This,ppfi)

#define Folder2_get_OfflineStatus(This,pul)	\
    (This)->lpVtbl -> get_OfflineStatus(This,pul)

#define Folder2_Synchronize(This)	\
    (This)->lpVtbl -> Synchronize(This)

#define Folder2_get_HaveToShowWebViewBarricade(This,pbHaveToShowWebViewBarricade)	\
    (This)->lpVtbl -> get_HaveToShowWebViewBarricade(This,pbHaveToShowWebViewBarricade)

#define Folder2_DismissedWebViewBarricade(This)	\
    (This)->lpVtbl -> DismissedWebViewBarricade(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Folder2_get_Self_Proxy( 
    Folder2 * This,
    /* [retval][out] */ FolderItem **ppfi);


void __RPC_STUB Folder2_get_Self_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE Folder2_get_OfflineStatus_Proxy( 
    Folder2 * This,
    /* [retval][out] */ LONG *pul);


void __RPC_STUB Folder2_get_OfflineStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Folder2_Synchronize_Proxy( 
    Folder2 * This);


void __RPC_STUB Folder2_Synchronize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE Folder2_get_HaveToShowWebViewBarricade_Proxy( 
    Folder2 * This,
    /* [retval][out] */ VARIANT_BOOL *pbHaveToShowWebViewBarricade);


void __RPC_STUB Folder2_get_HaveToShowWebViewBarricade_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE Folder2_DismissedWebViewBarricade_Proxy( 
    Folder2 * This);


void __RPC_STUB Folder2_DismissedWebViewBarricade_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __Folder2_INTERFACE_DEFINED__ */


#ifndef __Folder3_INTERFACE_DEFINED__
#define __Folder3_INTERFACE_DEFINED__

/* interface Folder3 */
/* [object][dual][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_Folder3;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A7AE5F64-C4D7-4d7f-9307-4D24EE54B841")
    Folder3 : public Folder2
    {
    public:
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ShowWebViewBarricade( 
            /* [retval][out] */ VARIANT_BOOL *pbShowWebViewBarricade) = 0;
        
        virtual /* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ShowWebViewBarricade( 
            /* [in] */ VARIANT_BOOL bShowWebViewBarricade) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct Folder3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            Folder3 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            Folder3 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            Folder3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            Folder3 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            Folder3 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            Folder3 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            Folder3 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Title )( 
            Folder3 * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            Folder3 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parent )( 
            Folder3 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ParentFolder )( 
            Folder3 * This,
            /* [retval][out] */ Folder **ppsf);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Items )( 
            Folder3 * This,
            /* [retval][out] */ FolderItems **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ParseName )( 
            Folder3 * This,
            /* [in] */ BSTR bName,
            /* [retval][out] */ FolderItem **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *NewFolder )( 
            Folder3 * This,
            /* [in] */ BSTR bName,
            /* [optional][in] */ VARIANT vOptions);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *MoveHere )( 
            Folder3 * This,
            /* [in] */ VARIANT vItem,
            /* [optional][in] */ VARIANT vOptions);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CopyHere )( 
            Folder3 * This,
            /* [in] */ VARIANT vItem,
            /* [optional][in] */ VARIANT vOptions);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetDetailsOf )( 
            Folder3 * This,
            /* [in] */ VARIANT vItem,
            /* [in] */ int iColumn,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Self )( 
            Folder3 * This,
            /* [retval][out] */ FolderItem **ppfi);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_OfflineStatus )( 
            Folder3 * This,
            /* [retval][out] */ LONG *pul);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Synchronize )( 
            Folder3 * This);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_HaveToShowWebViewBarricade )( 
            Folder3 * This,
            /* [retval][out] */ VARIANT_BOOL *pbHaveToShowWebViewBarricade);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *DismissedWebViewBarricade )( 
            Folder3 * This);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ShowWebViewBarricade )( 
            Folder3 * This,
            /* [retval][out] */ VARIANT_BOOL *pbShowWebViewBarricade);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ShowWebViewBarricade )( 
            Folder3 * This,
            /* [in] */ VARIANT_BOOL bShowWebViewBarricade);
        
        END_INTERFACE
    } Folder3Vtbl;

    interface Folder3
    {
        CONST_VTBL struct Folder3Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Folder3_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define Folder3_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define Folder3_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define Folder3_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define Folder3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define Folder3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define Folder3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define Folder3_get_Title(This,pbs)	\
    (This)->lpVtbl -> get_Title(This,pbs)

#define Folder3_get_Application(This,ppid)	\
    (This)->lpVtbl -> get_Application(This,ppid)

#define Folder3_get_Parent(This,ppid)	\
    (This)->lpVtbl -> get_Parent(This,ppid)

#define Folder3_get_ParentFolder(This,ppsf)	\
    (This)->lpVtbl -> get_ParentFolder(This,ppsf)

#define Folder3_Items(This,ppid)	\
    (This)->lpVtbl -> Items(This,ppid)

#define Folder3_ParseName(This,bName,ppid)	\
    (This)->lpVtbl -> ParseName(This,bName,ppid)

#define Folder3_NewFolder(This,bName,vOptions)	\
    (This)->lpVtbl -> NewFolder(This,bName,vOptions)

#define Folder3_MoveHere(This,vItem,vOptions)	\
    (This)->lpVtbl -> MoveHere(This,vItem,vOptions)

#define Folder3_CopyHere(This,vItem,vOptions)	\
    (This)->lpVtbl -> CopyHere(This,vItem,vOptions)

#define Folder3_GetDetailsOf(This,vItem,iColumn,pbs)	\
    (This)->lpVtbl -> GetDetailsOf(This,vItem,iColumn,pbs)


#define Folder3_get_Self(This,ppfi)	\
    (This)->lpVtbl -> get_Self(This,ppfi)

#define Folder3_get_OfflineStatus(This,pul)	\
    (This)->lpVtbl -> get_OfflineStatus(This,pul)

#define Folder3_Synchronize(This)	\
    (This)->lpVtbl -> Synchronize(This)

#define Folder3_get_HaveToShowWebViewBarricade(This,pbHaveToShowWebViewBarricade)	\
    (This)->lpVtbl -> get_HaveToShowWebViewBarricade(This,pbHaveToShowWebViewBarricade)

#define Folder3_DismissedWebViewBarricade(This)	\
    (This)->lpVtbl -> DismissedWebViewBarricade(This)


#define Folder3_get_ShowWebViewBarricade(This,pbShowWebViewBarricade)	\
    (This)->lpVtbl -> get_ShowWebViewBarricade(This,pbShowWebViewBarricade)

#define Folder3_put_ShowWebViewBarricade(This,bShowWebViewBarricade)	\
    (This)->lpVtbl -> put_ShowWebViewBarricade(This,bShowWebViewBarricade)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE Folder3_get_ShowWebViewBarricade_Proxy( 
    Folder3 * This,
    /* [retval][out] */ VARIANT_BOOL *pbShowWebViewBarricade);


void __RPC_STUB Folder3_get_ShowWebViewBarricade_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE Folder3_put_ShowWebViewBarricade_Proxy( 
    Folder3 * This,
    /* [in] */ VARIANT_BOOL bShowWebViewBarricade);


void __RPC_STUB Folder3_put_ShowWebViewBarricade_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __Folder3_INTERFACE_DEFINED__ */


#ifndef __FolderItem2_INTERFACE_DEFINED__
#define __FolderItem2_INTERFACE_DEFINED__

/* interface FolderItem2 */
/* [object][dual][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_FolderItem2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("edc817aa-92b8-11d1-b075-00c04fc33aa5")
    FolderItem2 : public FolderItem
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE InvokeVerbEx( 
            /* [optional][in] */ VARIANT vVerb,
            /* [optional][in] */ VARIANT vArgs) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ExtendedProperty( 
            /* [in] */ BSTR bstrPropName,
            /* [retval][out] */ VARIANT *pvRet) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct FolderItem2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            FolderItem2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            FolderItem2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            FolderItem2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            FolderItem2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            FolderItem2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            FolderItem2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            FolderItem2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            FolderItem2 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parent )( 
            FolderItem2 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            FolderItem2 * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Name )( 
            FolderItem2 * This,
            /* [in] */ BSTR bs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Path )( 
            FolderItem2 * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_GetLink )( 
            FolderItem2 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_GetFolder )( 
            FolderItem2 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsLink )( 
            FolderItem2 * This,
            /* [retval][out] */ VARIANT_BOOL *pb);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsFolder )( 
            FolderItem2 * This,
            /* [retval][out] */ VARIANT_BOOL *pb);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsFileSystem )( 
            FolderItem2 * This,
            /* [retval][out] */ VARIANT_BOOL *pb);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_IsBrowsable )( 
            FolderItem2 * This,
            /* [retval][out] */ VARIANT_BOOL *pb);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ModifyDate )( 
            FolderItem2 * This,
            /* [retval][out] */ DATE *pdt);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ModifyDate )( 
            FolderItem2 * This,
            /* [in] */ DATE dt);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Size )( 
            FolderItem2 * This,
            /* [retval][out] */ LONG *pul);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Type )( 
            FolderItem2 * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Verbs )( 
            FolderItem2 * This,
            /* [retval][out] */ FolderItemVerbs **ppfic);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *InvokeVerb )( 
            FolderItem2 * This,
            /* [optional][in] */ VARIANT vVerb);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *InvokeVerbEx )( 
            FolderItem2 * This,
            /* [optional][in] */ VARIANT vVerb,
            /* [optional][in] */ VARIANT vArgs);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ExtendedProperty )( 
            FolderItem2 * This,
            /* [in] */ BSTR bstrPropName,
            /* [retval][out] */ VARIANT *pvRet);
        
        END_INTERFACE
    } FolderItem2Vtbl;

    interface FolderItem2
    {
        CONST_VTBL struct FolderItem2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define FolderItem2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define FolderItem2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define FolderItem2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define FolderItem2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define FolderItem2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define FolderItem2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define FolderItem2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define FolderItem2_get_Application(This,ppid)	\
    (This)->lpVtbl -> get_Application(This,ppid)

#define FolderItem2_get_Parent(This,ppid)	\
    (This)->lpVtbl -> get_Parent(This,ppid)

#define FolderItem2_get_Name(This,pbs)	\
    (This)->lpVtbl -> get_Name(This,pbs)

#define FolderItem2_put_Name(This,bs)	\
    (This)->lpVtbl -> put_Name(This,bs)

#define FolderItem2_get_Path(This,pbs)	\
    (This)->lpVtbl -> get_Path(This,pbs)

#define FolderItem2_get_GetLink(This,ppid)	\
    (This)->lpVtbl -> get_GetLink(This,ppid)

#define FolderItem2_get_GetFolder(This,ppid)	\
    (This)->lpVtbl -> get_GetFolder(This,ppid)

#define FolderItem2_get_IsLink(This,pb)	\
    (This)->lpVtbl -> get_IsLink(This,pb)

#define FolderItem2_get_IsFolder(This,pb)	\
    (This)->lpVtbl -> get_IsFolder(This,pb)

#define FolderItem2_get_IsFileSystem(This,pb)	\
    (This)->lpVtbl -> get_IsFileSystem(This,pb)

#define FolderItem2_get_IsBrowsable(This,pb)	\
    (This)->lpVtbl -> get_IsBrowsable(This,pb)

#define FolderItem2_get_ModifyDate(This,pdt)	\
    (This)->lpVtbl -> get_ModifyDate(This,pdt)

#define FolderItem2_put_ModifyDate(This,dt)	\
    (This)->lpVtbl -> put_ModifyDate(This,dt)

#define FolderItem2_get_Size(This,pul)	\
    (This)->lpVtbl -> get_Size(This,pul)

#define FolderItem2_get_Type(This,pbs)	\
    (This)->lpVtbl -> get_Type(This,pbs)

#define FolderItem2_Verbs(This,ppfic)	\
    (This)->lpVtbl -> Verbs(This,ppfic)

#define FolderItem2_InvokeVerb(This,vVerb)	\
    (This)->lpVtbl -> InvokeVerb(This,vVerb)


#define FolderItem2_InvokeVerbEx(This,vVerb,vArgs)	\
    (This)->lpVtbl -> InvokeVerbEx(This,vVerb,vArgs)

#define FolderItem2_ExtendedProperty(This,bstrPropName,pvRet)	\
    (This)->lpVtbl -> ExtendedProperty(This,bstrPropName,pvRet)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE FolderItem2_InvokeVerbEx_Proxy( 
    FolderItem2 * This,
    /* [optional][in] */ VARIANT vVerb,
    /* [optional][in] */ VARIANT vArgs);


void __RPC_STUB FolderItem2_InvokeVerbEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE FolderItem2_ExtendedProperty_Proxy( 
    FolderItem2 * This,
    /* [in] */ BSTR bstrPropName,
    /* [retval][out] */ VARIANT *pvRet);


void __RPC_STUB FolderItem2_ExtendedProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __FolderItem2_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ShellFolderItem;

#ifdef __cplusplus

class DECLSPEC_UUID("2fe352ea-fd1f-11d2-b1f4-00c04f8eeb3e")
ShellFolderItem;
#endif

#ifndef __FolderItems2_INTERFACE_DEFINED__
#define __FolderItems2_INTERFACE_DEFINED__

/* interface FolderItems2 */
/* [object][dual][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_FolderItems2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C94F0AD0-F363-11d2-A327-00C04F8EEC7F")
    FolderItems2 : public FolderItems
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE InvokeVerbEx( 
            /* [optional][in] */ VARIANT vVerb,
            /* [optional][in] */ VARIANT vArgs) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct FolderItems2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            FolderItems2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            FolderItems2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            FolderItems2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            FolderItems2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            FolderItems2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            FolderItems2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            FolderItems2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            FolderItems2 * This,
            /* [retval][out] */ long *plCount);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            FolderItems2 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parent )( 
            FolderItems2 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            FolderItems2 * This,
            /* [optional][in] */ VARIANT index,
            /* [retval][out] */ FolderItem **ppid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *_NewEnum )( 
            FolderItems2 * This,
            /* [retval][out] */ IUnknown **ppunk);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *InvokeVerbEx )( 
            FolderItems2 * This,
            /* [optional][in] */ VARIANT vVerb,
            /* [optional][in] */ VARIANT vArgs);
        
        END_INTERFACE
    } FolderItems2Vtbl;

    interface FolderItems2
    {
        CONST_VTBL struct FolderItems2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define FolderItems2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define FolderItems2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define FolderItems2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define FolderItems2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define FolderItems2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define FolderItems2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define FolderItems2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define FolderItems2_get_Count(This,plCount)	\
    (This)->lpVtbl -> get_Count(This,plCount)

#define FolderItems2_get_Application(This,ppid)	\
    (This)->lpVtbl -> get_Application(This,ppid)

#define FolderItems2_get_Parent(This,ppid)	\
    (This)->lpVtbl -> get_Parent(This,ppid)

#define FolderItems2_Item(This,index,ppid)	\
    (This)->lpVtbl -> Item(This,index,ppid)

#define FolderItems2__NewEnum(This,ppunk)	\
    (This)->lpVtbl -> _NewEnum(This,ppunk)


#define FolderItems2_InvokeVerbEx(This,vVerb,vArgs)	\
    (This)->lpVtbl -> InvokeVerbEx(This,vVerb,vArgs)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE FolderItems2_InvokeVerbEx_Proxy( 
    FolderItems2 * This,
    /* [optional][in] */ VARIANT vVerb,
    /* [optional][in] */ VARIANT vArgs);


void __RPC_STUB FolderItems2_InvokeVerbEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __FolderItems2_INTERFACE_DEFINED__ */


#ifndef __FolderItems3_INTERFACE_DEFINED__
#define __FolderItems3_INTERFACE_DEFINED__

/* interface FolderItems3 */
/* [object][dual][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_FolderItems3;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("eaa7c309-bbec-49d5-821d-64d966cb667f")
    FolderItems3 : public FolderItems2
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Filter( 
            /* [in] */ long grfFlags,
            /* [in] */ BSTR bstrFileSpec) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Verbs( 
            /* [retval][out] */ FolderItemVerbs **ppfic) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct FolderItems3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            FolderItems3 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            FolderItems3 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            FolderItems3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            FolderItems3 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            FolderItems3 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            FolderItems3 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            FolderItems3 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            FolderItems3 * This,
            /* [retval][out] */ long *plCount);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            FolderItems3 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parent )( 
            FolderItems3 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            FolderItems3 * This,
            /* [optional][in] */ VARIANT index,
            /* [retval][out] */ FolderItem **ppid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *_NewEnum )( 
            FolderItems3 * This,
            /* [retval][out] */ IUnknown **ppunk);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *InvokeVerbEx )( 
            FolderItems3 * This,
            /* [optional][in] */ VARIANT vVerb,
            /* [optional][in] */ VARIANT vArgs);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Filter )( 
            FolderItems3 * This,
            /* [in] */ long grfFlags,
            /* [in] */ BSTR bstrFileSpec);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Verbs )( 
            FolderItems3 * This,
            /* [retval][out] */ FolderItemVerbs **ppfic);
        
        END_INTERFACE
    } FolderItems3Vtbl;

    interface FolderItems3
    {
        CONST_VTBL struct FolderItems3Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define FolderItems3_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define FolderItems3_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define FolderItems3_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define FolderItems3_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define FolderItems3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define FolderItems3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define FolderItems3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define FolderItems3_get_Count(This,plCount)	\
    (This)->lpVtbl -> get_Count(This,plCount)

#define FolderItems3_get_Application(This,ppid)	\
    (This)->lpVtbl -> get_Application(This,ppid)

#define FolderItems3_get_Parent(This,ppid)	\
    (This)->lpVtbl -> get_Parent(This,ppid)

#define FolderItems3_Item(This,index,ppid)	\
    (This)->lpVtbl -> Item(This,index,ppid)

#define FolderItems3__NewEnum(This,ppunk)	\
    (This)->lpVtbl -> _NewEnum(This,ppunk)


#define FolderItems3_InvokeVerbEx(This,vVerb,vArgs)	\
    (This)->lpVtbl -> InvokeVerbEx(This,vVerb,vArgs)


#define FolderItems3_Filter(This,grfFlags,bstrFileSpec)	\
    (This)->lpVtbl -> Filter(This,grfFlags,bstrFileSpec)

#define FolderItems3_get_Verbs(This,ppfic)	\
    (This)->lpVtbl -> get_Verbs(This,ppfic)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE FolderItems3_Filter_Proxy( 
    FolderItems3 * This,
    /* [in] */ long grfFlags,
    /* [in] */ BSTR bstrFileSpec);


void __RPC_STUB FolderItems3_Filter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE FolderItems3_get_Verbs_Proxy( 
    FolderItems3 * This,
    /* [retval][out] */ FolderItemVerbs **ppfic);


void __RPC_STUB FolderItems3_get_Verbs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __FolderItems3_INTERFACE_DEFINED__ */


#ifndef __IShellLinkDual_INTERFACE_DEFINED__
#define __IShellLinkDual_INTERFACE_DEFINED__

/* interface IShellLinkDual */
/* [object][hidden][dual][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_IShellLinkDual;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("88A05C00-F000-11CE-8350-444553540000")
    IShellLinkDual : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Path( 
            /* [retval][out] */ BSTR *pbs) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Path( 
            /* [in] */ BSTR bs) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR *pbs) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ BSTR bs) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_WorkingDirectory( 
            /* [retval][out] */ BSTR *pbs) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_WorkingDirectory( 
            /* [in] */ BSTR bs) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Arguments( 
            /* [retval][out] */ BSTR *pbs) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Arguments( 
            /* [in] */ BSTR bs) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Hotkey( 
            /* [retval][out] */ int *piHK) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_Hotkey( 
            /* [in] */ int iHK) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ShowCommand( 
            /* [retval][out] */ int *piShowCommand) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ShowCommand( 
            /* [in] */ int iShowCommand) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Resolve( 
            /* [in] */ int fFlags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetIconLocation( 
            /* [out] */ BSTR *pbs,
            /* [retval][out] */ int *piIcon) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetIconLocation( 
            /* [in] */ BSTR bs,
            /* [in] */ int iIcon) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [optional][in] */ VARIANT vWhere) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellLinkDualVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellLinkDual * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellLinkDual * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellLinkDual * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IShellLinkDual * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IShellLinkDual * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IShellLinkDual * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IShellLinkDual * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Path )( 
            IShellLinkDual * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Path )( 
            IShellLinkDual * This,
            /* [in] */ BSTR bs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Description )( 
            IShellLinkDual * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Description )( 
            IShellLinkDual * This,
            /* [in] */ BSTR bs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_WorkingDirectory )( 
            IShellLinkDual * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_WorkingDirectory )( 
            IShellLinkDual * This,
            /* [in] */ BSTR bs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Arguments )( 
            IShellLinkDual * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Arguments )( 
            IShellLinkDual * This,
            /* [in] */ BSTR bs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Hotkey )( 
            IShellLinkDual * This,
            /* [retval][out] */ int *piHK);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Hotkey )( 
            IShellLinkDual * This,
            /* [in] */ int iHK);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ShowCommand )( 
            IShellLinkDual * This,
            /* [retval][out] */ int *piShowCommand);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ShowCommand )( 
            IShellLinkDual * This,
            /* [in] */ int iShowCommand);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Resolve )( 
            IShellLinkDual * This,
            /* [in] */ int fFlags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetIconLocation )( 
            IShellLinkDual * This,
            /* [out] */ BSTR *pbs,
            /* [retval][out] */ int *piIcon);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetIconLocation )( 
            IShellLinkDual * This,
            /* [in] */ BSTR bs,
            /* [in] */ int iIcon);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Save )( 
            IShellLinkDual * This,
            /* [optional][in] */ VARIANT vWhere);
        
        END_INTERFACE
    } IShellLinkDualVtbl;

    interface IShellLinkDual
    {
        CONST_VTBL struct IShellLinkDualVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellLinkDual_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellLinkDual_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellLinkDual_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellLinkDual_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IShellLinkDual_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IShellLinkDual_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IShellLinkDual_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IShellLinkDual_get_Path(This,pbs)	\
    (This)->lpVtbl -> get_Path(This,pbs)

#define IShellLinkDual_put_Path(This,bs)	\
    (This)->lpVtbl -> put_Path(This,bs)

#define IShellLinkDual_get_Description(This,pbs)	\
    (This)->lpVtbl -> get_Description(This,pbs)

#define IShellLinkDual_put_Description(This,bs)	\
    (This)->lpVtbl -> put_Description(This,bs)

#define IShellLinkDual_get_WorkingDirectory(This,pbs)	\
    (This)->lpVtbl -> get_WorkingDirectory(This,pbs)

#define IShellLinkDual_put_WorkingDirectory(This,bs)	\
    (This)->lpVtbl -> put_WorkingDirectory(This,bs)

#define IShellLinkDual_get_Arguments(This,pbs)	\
    (This)->lpVtbl -> get_Arguments(This,pbs)

#define IShellLinkDual_put_Arguments(This,bs)	\
    (This)->lpVtbl -> put_Arguments(This,bs)

#define IShellLinkDual_get_Hotkey(This,piHK)	\
    (This)->lpVtbl -> get_Hotkey(This,piHK)

#define IShellLinkDual_put_Hotkey(This,iHK)	\
    (This)->lpVtbl -> put_Hotkey(This,iHK)

#define IShellLinkDual_get_ShowCommand(This,piShowCommand)	\
    (This)->lpVtbl -> get_ShowCommand(This,piShowCommand)

#define IShellLinkDual_put_ShowCommand(This,iShowCommand)	\
    (This)->lpVtbl -> put_ShowCommand(This,iShowCommand)

#define IShellLinkDual_Resolve(This,fFlags)	\
    (This)->lpVtbl -> Resolve(This,fFlags)

#define IShellLinkDual_GetIconLocation(This,pbs,piIcon)	\
    (This)->lpVtbl -> GetIconLocation(This,pbs,piIcon)

#define IShellLinkDual_SetIconLocation(This,bs,iIcon)	\
    (This)->lpVtbl -> SetIconLocation(This,bs,iIcon)

#define IShellLinkDual_Save(This,vWhere)	\
    (This)->lpVtbl -> Save(This,vWhere)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IShellLinkDual_get_Path_Proxy( 
    IShellLinkDual * This,
    /* [retval][out] */ BSTR *pbs);


void __RPC_STUB IShellLinkDual_get_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IShellLinkDual_put_Path_Proxy( 
    IShellLinkDual * This,
    /* [in] */ BSTR bs);


void __RPC_STUB IShellLinkDual_put_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IShellLinkDual_get_Description_Proxy( 
    IShellLinkDual * This,
    /* [retval][out] */ BSTR *pbs);


void __RPC_STUB IShellLinkDual_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IShellLinkDual_put_Description_Proxy( 
    IShellLinkDual * This,
    /* [in] */ BSTR bs);


void __RPC_STUB IShellLinkDual_put_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IShellLinkDual_get_WorkingDirectory_Proxy( 
    IShellLinkDual * This,
    /* [retval][out] */ BSTR *pbs);


void __RPC_STUB IShellLinkDual_get_WorkingDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IShellLinkDual_put_WorkingDirectory_Proxy( 
    IShellLinkDual * This,
    /* [in] */ BSTR bs);


void __RPC_STUB IShellLinkDual_put_WorkingDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IShellLinkDual_get_Arguments_Proxy( 
    IShellLinkDual * This,
    /* [retval][out] */ BSTR *pbs);


void __RPC_STUB IShellLinkDual_get_Arguments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IShellLinkDual_put_Arguments_Proxy( 
    IShellLinkDual * This,
    /* [in] */ BSTR bs);


void __RPC_STUB IShellLinkDual_put_Arguments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IShellLinkDual_get_Hotkey_Proxy( 
    IShellLinkDual * This,
    /* [retval][out] */ int *piHK);


void __RPC_STUB IShellLinkDual_get_Hotkey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IShellLinkDual_put_Hotkey_Proxy( 
    IShellLinkDual * This,
    /* [in] */ int iHK);


void __RPC_STUB IShellLinkDual_put_Hotkey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IShellLinkDual_get_ShowCommand_Proxy( 
    IShellLinkDual * This,
    /* [retval][out] */ int *piShowCommand);


void __RPC_STUB IShellLinkDual_get_ShowCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IShellLinkDual_put_ShowCommand_Proxy( 
    IShellLinkDual * This,
    /* [in] */ int iShowCommand);


void __RPC_STUB IShellLinkDual_put_ShowCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellLinkDual_Resolve_Proxy( 
    IShellLinkDual * This,
    /* [in] */ int fFlags);


void __RPC_STUB IShellLinkDual_Resolve_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellLinkDual_GetIconLocation_Proxy( 
    IShellLinkDual * This,
    /* [out] */ BSTR *pbs,
    /* [retval][out] */ int *piIcon);


void __RPC_STUB IShellLinkDual_GetIconLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellLinkDual_SetIconLocation_Proxy( 
    IShellLinkDual * This,
    /* [in] */ BSTR bs,
    /* [in] */ int iIcon);


void __RPC_STUB IShellLinkDual_SetIconLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellLinkDual_Save_Proxy( 
    IShellLinkDual * This,
    /* [optional][in] */ VARIANT vWhere);


void __RPC_STUB IShellLinkDual_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellLinkDual_INTERFACE_DEFINED__ */


#ifndef __IShellLinkDual2_INTERFACE_DEFINED__
#define __IShellLinkDual2_INTERFACE_DEFINED__

/* interface IShellLinkDual2 */
/* [object][hidden][dual][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_IShellLinkDual2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("317EE249-F12E-11d2-B1E4-00C04F8EEB3E")
    IShellLinkDual2 : public IShellLinkDual
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Target( 
            /* [retval][out] */ FolderItem **ppfi) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellLinkDual2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellLinkDual2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellLinkDual2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellLinkDual2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IShellLinkDual2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IShellLinkDual2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IShellLinkDual2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IShellLinkDual2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Path )( 
            IShellLinkDual2 * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Path )( 
            IShellLinkDual2 * This,
            /* [in] */ BSTR bs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Description )( 
            IShellLinkDual2 * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Description )( 
            IShellLinkDual2 * This,
            /* [in] */ BSTR bs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_WorkingDirectory )( 
            IShellLinkDual2 * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_WorkingDirectory )( 
            IShellLinkDual2 * This,
            /* [in] */ BSTR bs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Arguments )( 
            IShellLinkDual2 * This,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Arguments )( 
            IShellLinkDual2 * This,
            /* [in] */ BSTR bs);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Hotkey )( 
            IShellLinkDual2 * This,
            /* [retval][out] */ int *piHK);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Hotkey )( 
            IShellLinkDual2 * This,
            /* [in] */ int iHK);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ShowCommand )( 
            IShellLinkDual2 * This,
            /* [retval][out] */ int *piShowCommand);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ShowCommand )( 
            IShellLinkDual2 * This,
            /* [in] */ int iShowCommand);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Resolve )( 
            IShellLinkDual2 * This,
            /* [in] */ int fFlags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetIconLocation )( 
            IShellLinkDual2 * This,
            /* [out] */ BSTR *pbs,
            /* [retval][out] */ int *piIcon);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetIconLocation )( 
            IShellLinkDual2 * This,
            /* [in] */ BSTR bs,
            /* [in] */ int iIcon);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Save )( 
            IShellLinkDual2 * This,
            /* [optional][in] */ VARIANT vWhere);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Target )( 
            IShellLinkDual2 * This,
            /* [retval][out] */ FolderItem **ppfi);
        
        END_INTERFACE
    } IShellLinkDual2Vtbl;

    interface IShellLinkDual2
    {
        CONST_VTBL struct IShellLinkDual2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellLinkDual2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellLinkDual2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellLinkDual2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellLinkDual2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IShellLinkDual2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IShellLinkDual2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IShellLinkDual2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IShellLinkDual2_get_Path(This,pbs)	\
    (This)->lpVtbl -> get_Path(This,pbs)

#define IShellLinkDual2_put_Path(This,bs)	\
    (This)->lpVtbl -> put_Path(This,bs)

#define IShellLinkDual2_get_Description(This,pbs)	\
    (This)->lpVtbl -> get_Description(This,pbs)

#define IShellLinkDual2_put_Description(This,bs)	\
    (This)->lpVtbl -> put_Description(This,bs)

#define IShellLinkDual2_get_WorkingDirectory(This,pbs)	\
    (This)->lpVtbl -> get_WorkingDirectory(This,pbs)

#define IShellLinkDual2_put_WorkingDirectory(This,bs)	\
    (This)->lpVtbl -> put_WorkingDirectory(This,bs)

#define IShellLinkDual2_get_Arguments(This,pbs)	\
    (This)->lpVtbl -> get_Arguments(This,pbs)

#define IShellLinkDual2_put_Arguments(This,bs)	\
    (This)->lpVtbl -> put_Arguments(This,bs)

#define IShellLinkDual2_get_Hotkey(This,piHK)	\
    (This)->lpVtbl -> get_Hotkey(This,piHK)

#define IShellLinkDual2_put_Hotkey(This,iHK)	\
    (This)->lpVtbl -> put_Hotkey(This,iHK)

#define IShellLinkDual2_get_ShowCommand(This,piShowCommand)	\
    (This)->lpVtbl -> get_ShowCommand(This,piShowCommand)

#define IShellLinkDual2_put_ShowCommand(This,iShowCommand)	\
    (This)->lpVtbl -> put_ShowCommand(This,iShowCommand)

#define IShellLinkDual2_Resolve(This,fFlags)	\
    (This)->lpVtbl -> Resolve(This,fFlags)

#define IShellLinkDual2_GetIconLocation(This,pbs,piIcon)	\
    (This)->lpVtbl -> GetIconLocation(This,pbs,piIcon)

#define IShellLinkDual2_SetIconLocation(This,bs,iIcon)	\
    (This)->lpVtbl -> SetIconLocation(This,bs,iIcon)

#define IShellLinkDual2_Save(This,vWhere)	\
    (This)->lpVtbl -> Save(This,vWhere)


#define IShellLinkDual2_get_Target(This,ppfi)	\
    (This)->lpVtbl -> get_Target(This,ppfi)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IShellLinkDual2_get_Target_Proxy( 
    IShellLinkDual2 * This,
    /* [retval][out] */ FolderItem **ppfi);


void __RPC_STUB IShellLinkDual2_get_Target_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellLinkDual2_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ShellLinkObject;

#ifdef __cplusplus

class DECLSPEC_UUID("11219420-1768-11d1-95BE-00609797EA4F")
ShellLinkObject;
#endif

#ifndef __IShellFolderViewDual_INTERFACE_DEFINED__
#define __IShellFolderViewDual_INTERFACE_DEFINED__

/* interface IShellFolderViewDual */
/* [object][dual][hidden][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_IShellFolderViewDual;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E7A1AF80-4D96-11CF-960C-0080C7F4EE85")
    IShellFolderViewDual : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IDispatch **ppid) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Parent( 
            /* [retval][out] */ IDispatch **ppid) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Folder( 
            /* [retval][out] */ Folder **ppid) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SelectedItems( 
            /* [retval][out] */ FolderItems **ppid) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_FocusedItem( 
            /* [retval][out] */ FolderItem **ppid) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SelectItem( 
            /* [in] */ VARIANT *pvfi,
            /* [in] */ int dwFlags) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE PopupItemMenu( 
            /* [in] */ FolderItem *pfi,
            /* [optional][in] */ VARIANT vx,
            /* [optional][in] */ VARIANT vy,
            /* [retval][out] */ BSTR *pbs) = 0;
        
        virtual /* [helpcontext][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Script( 
            /* [retval][out] */ IDispatch **ppDisp) = 0;
        
        virtual /* [helpcontext][helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ViewOptions( 
            /* [retval][out] */ long *plViewOptions) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellFolderViewDualVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellFolderViewDual * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellFolderViewDual * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellFolderViewDual * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IShellFolderViewDual * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IShellFolderViewDual * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IShellFolderViewDual * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IShellFolderViewDual * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            IShellFolderViewDual * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parent )( 
            IShellFolderViewDual * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Folder )( 
            IShellFolderViewDual * This,
            /* [retval][out] */ Folder **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SelectedItems )( 
            IShellFolderViewDual * This,
            /* [retval][out] */ FolderItems **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FocusedItem )( 
            IShellFolderViewDual * This,
            /* [retval][out] */ FolderItem **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SelectItem )( 
            IShellFolderViewDual * This,
            /* [in] */ VARIANT *pvfi,
            /* [in] */ int dwFlags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *PopupItemMenu )( 
            IShellFolderViewDual * This,
            /* [in] */ FolderItem *pfi,
            /* [optional][in] */ VARIANT vx,
            /* [optional][in] */ VARIANT vy,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpcontext][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Script )( 
            IShellFolderViewDual * This,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [helpcontext][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewOptions )( 
            IShellFolderViewDual * This,
            /* [retval][out] */ long *plViewOptions);
        
        END_INTERFACE
    } IShellFolderViewDualVtbl;

    interface IShellFolderViewDual
    {
        CONST_VTBL struct IShellFolderViewDualVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellFolderViewDual_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellFolderViewDual_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellFolderViewDual_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellFolderViewDual_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IShellFolderViewDual_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IShellFolderViewDual_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IShellFolderViewDual_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IShellFolderViewDual_get_Application(This,ppid)	\
    (This)->lpVtbl -> get_Application(This,ppid)

#define IShellFolderViewDual_get_Parent(This,ppid)	\
    (This)->lpVtbl -> get_Parent(This,ppid)

#define IShellFolderViewDual_get_Folder(This,ppid)	\
    (This)->lpVtbl -> get_Folder(This,ppid)

#define IShellFolderViewDual_SelectedItems(This,ppid)	\
    (This)->lpVtbl -> SelectedItems(This,ppid)

#define IShellFolderViewDual_get_FocusedItem(This,ppid)	\
    (This)->lpVtbl -> get_FocusedItem(This,ppid)

#define IShellFolderViewDual_SelectItem(This,pvfi,dwFlags)	\
    (This)->lpVtbl -> SelectItem(This,pvfi,dwFlags)

#define IShellFolderViewDual_PopupItemMenu(This,pfi,vx,vy,pbs)	\
    (This)->lpVtbl -> PopupItemMenu(This,pfi,vx,vy,pbs)

#define IShellFolderViewDual_get_Script(This,ppDisp)	\
    (This)->lpVtbl -> get_Script(This,ppDisp)

#define IShellFolderViewDual_get_ViewOptions(This,plViewOptions)	\
    (This)->lpVtbl -> get_ViewOptions(This,plViewOptions)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IShellFolderViewDual_get_Application_Proxy( 
    IShellFolderViewDual * This,
    /* [retval][out] */ IDispatch **ppid);


void __RPC_STUB IShellFolderViewDual_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IShellFolderViewDual_get_Parent_Proxy( 
    IShellFolderViewDual * This,
    /* [retval][out] */ IDispatch **ppid);


void __RPC_STUB IShellFolderViewDual_get_Parent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IShellFolderViewDual_get_Folder_Proxy( 
    IShellFolderViewDual * This,
    /* [retval][out] */ Folder **ppid);


void __RPC_STUB IShellFolderViewDual_get_Folder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellFolderViewDual_SelectedItems_Proxy( 
    IShellFolderViewDual * This,
    /* [retval][out] */ FolderItems **ppid);


void __RPC_STUB IShellFolderViewDual_SelectedItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IShellFolderViewDual_get_FocusedItem_Proxy( 
    IShellFolderViewDual * This,
    /* [retval][out] */ FolderItem **ppid);


void __RPC_STUB IShellFolderViewDual_get_FocusedItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellFolderViewDual_SelectItem_Proxy( 
    IShellFolderViewDual * This,
    /* [in] */ VARIANT *pvfi,
    /* [in] */ int dwFlags);


void __RPC_STUB IShellFolderViewDual_SelectItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellFolderViewDual_PopupItemMenu_Proxy( 
    IShellFolderViewDual * This,
    /* [in] */ FolderItem *pfi,
    /* [optional][in] */ VARIANT vx,
    /* [optional][in] */ VARIANT vy,
    /* [retval][out] */ BSTR *pbs);


void __RPC_STUB IShellFolderViewDual_PopupItemMenu_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget] */ HRESULT STDMETHODCALLTYPE IShellFolderViewDual_get_Script_Proxy( 
    IShellFolderViewDual * This,
    /* [retval][out] */ IDispatch **ppDisp);


void __RPC_STUB IShellFolderViewDual_get_Script_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget] */ HRESULT STDMETHODCALLTYPE IShellFolderViewDual_get_ViewOptions_Proxy( 
    IShellFolderViewDual * This,
    /* [retval][out] */ long *plViewOptions);


void __RPC_STUB IShellFolderViewDual_get_ViewOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellFolderViewDual_INTERFACE_DEFINED__ */


#ifndef __IShellFolderViewDual2_INTERFACE_DEFINED__
#define __IShellFolderViewDual2_INTERFACE_DEFINED__

/* interface IShellFolderViewDual2 */
/* [object][dual][hidden][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_IShellFolderViewDual2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("31C147b6-0ADE-4A3C-B514-DDF932EF6D17")
    IShellFolderViewDual2 : public IShellFolderViewDual
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentViewMode( 
            /* [retval][out] */ UINT *pViewMode) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_CurrentViewMode( 
            /* [in] */ UINT ViewMode) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SelectItemRelative( 
            /* [in] */ int iRelative) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellFolderViewDual2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellFolderViewDual2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellFolderViewDual2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellFolderViewDual2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IShellFolderViewDual2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IShellFolderViewDual2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IShellFolderViewDual2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IShellFolderViewDual2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            IShellFolderViewDual2 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parent )( 
            IShellFolderViewDual2 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Folder )( 
            IShellFolderViewDual2 * This,
            /* [retval][out] */ Folder **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SelectedItems )( 
            IShellFolderViewDual2 * This,
            /* [retval][out] */ FolderItems **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_FocusedItem )( 
            IShellFolderViewDual2 * This,
            /* [retval][out] */ FolderItem **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SelectItem )( 
            IShellFolderViewDual2 * This,
            /* [in] */ VARIANT *pvfi,
            /* [in] */ int dwFlags);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *PopupItemMenu )( 
            IShellFolderViewDual2 * This,
            /* [in] */ FolderItem *pfi,
            /* [optional][in] */ VARIANT vx,
            /* [optional][in] */ VARIANT vy,
            /* [retval][out] */ BSTR *pbs);
        
        /* [helpcontext][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Script )( 
            IShellFolderViewDual2 * This,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [helpcontext][helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ViewOptions )( 
            IShellFolderViewDual2 * This,
            /* [retval][out] */ long *plViewOptions);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CurrentViewMode )( 
            IShellFolderViewDual2 * This,
            /* [retval][out] */ UINT *pViewMode);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE *put_CurrentViewMode )( 
            IShellFolderViewDual2 * This,
            /* [in] */ UINT ViewMode);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SelectItemRelative )( 
            IShellFolderViewDual2 * This,
            /* [in] */ int iRelative);
        
        END_INTERFACE
    } IShellFolderViewDual2Vtbl;

    interface IShellFolderViewDual2
    {
        CONST_VTBL struct IShellFolderViewDual2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellFolderViewDual2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellFolderViewDual2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellFolderViewDual2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellFolderViewDual2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IShellFolderViewDual2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IShellFolderViewDual2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IShellFolderViewDual2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IShellFolderViewDual2_get_Application(This,ppid)	\
    (This)->lpVtbl -> get_Application(This,ppid)

#define IShellFolderViewDual2_get_Parent(This,ppid)	\
    (This)->lpVtbl -> get_Parent(This,ppid)

#define IShellFolderViewDual2_get_Folder(This,ppid)	\
    (This)->lpVtbl -> get_Folder(This,ppid)

#define IShellFolderViewDual2_SelectedItems(This,ppid)	\
    (This)->lpVtbl -> SelectedItems(This,ppid)

#define IShellFolderViewDual2_get_FocusedItem(This,ppid)	\
    (This)->lpVtbl -> get_FocusedItem(This,ppid)

#define IShellFolderViewDual2_SelectItem(This,pvfi,dwFlags)	\
    (This)->lpVtbl -> SelectItem(This,pvfi,dwFlags)

#define IShellFolderViewDual2_PopupItemMenu(This,pfi,vx,vy,pbs)	\
    (This)->lpVtbl -> PopupItemMenu(This,pfi,vx,vy,pbs)

#define IShellFolderViewDual2_get_Script(This,ppDisp)	\
    (This)->lpVtbl -> get_Script(This,ppDisp)

#define IShellFolderViewDual2_get_ViewOptions(This,plViewOptions)	\
    (This)->lpVtbl -> get_ViewOptions(This,plViewOptions)


#define IShellFolderViewDual2_get_CurrentViewMode(This,pViewMode)	\
    (This)->lpVtbl -> get_CurrentViewMode(This,pViewMode)

#define IShellFolderViewDual2_put_CurrentViewMode(This,ViewMode)	\
    (This)->lpVtbl -> put_CurrentViewMode(This,ViewMode)

#define IShellFolderViewDual2_SelectItemRelative(This,iRelative)	\
    (This)->lpVtbl -> SelectItemRelative(This,iRelative)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IShellFolderViewDual2_get_CurrentViewMode_Proxy( 
    IShellFolderViewDual2 * This,
    /* [retval][out] */ UINT *pViewMode);


void __RPC_STUB IShellFolderViewDual2_get_CurrentViewMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IShellFolderViewDual2_put_CurrentViewMode_Proxy( 
    IShellFolderViewDual2 * This,
    /* [in] */ UINT ViewMode);


void __RPC_STUB IShellFolderViewDual2_put_CurrentViewMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellFolderViewDual2_SelectItemRelative_Proxy( 
    IShellFolderViewDual2 * This,
    /* [in] */ int iRelative);


void __RPC_STUB IShellFolderViewDual2_SelectItemRelative_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellFolderViewDual2_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ShellFolderView;

#ifdef __cplusplus

class DECLSPEC_UUID("62112AA1-EBE4-11cf-A5FB-0020AFE7292D")
ShellFolderView;
#endif

#ifndef __IShellDispatch_INTERFACE_DEFINED__
#define __IShellDispatch_INTERFACE_DEFINED__

/* interface IShellDispatch */
/* [object][dual][hidden][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_IShellDispatch;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D8F015C0-C278-11CE-A49E-444553540000")
    IShellDispatch : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IDispatch **ppid) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Parent( 
            /* [retval][out] */ IDispatch **ppid) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE NameSpace( 
            /* [in] */ VARIANT vDir,
            /* [retval][out] */ Folder **ppsdf) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE BrowseForFolder( 
            /* [in] */ long Hwnd,
            /* [in] */ BSTR Title,
            /* [in] */ long Options,
            /* [optional][in] */ VARIANT RootFolder,
            /* [retval][out] */ Folder **ppsdf) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Windows( 
            /* [retval][out] */ IDispatch **ppid) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Open( 
            /* [in] */ VARIANT vDir) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Explore( 
            /* [in] */ VARIANT vDir) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE MinimizeAll( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE UndoMinimizeALL( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE FileRun( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CascadeWindows( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE TileVertically( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE TileHorizontally( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ShutdownWindows( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Suspend( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EjectPC( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE SetTime( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE TrayProperties( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Help( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE FindFiles( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE FindComputer( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RefreshMenu( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ControlPanelItem( 
            /* [in] */ BSTR szDir) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellDispatchVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellDispatch * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellDispatch * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellDispatch * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IShellDispatch * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IShellDispatch * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IShellDispatch * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IShellDispatch * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            IShellDispatch * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parent )( 
            IShellDispatch * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *NameSpace )( 
            IShellDispatch * This,
            /* [in] */ VARIANT vDir,
            /* [retval][out] */ Folder **ppsdf);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *BrowseForFolder )( 
            IShellDispatch * This,
            /* [in] */ long Hwnd,
            /* [in] */ BSTR Title,
            /* [in] */ long Options,
            /* [optional][in] */ VARIANT RootFolder,
            /* [retval][out] */ Folder **ppsdf);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Windows )( 
            IShellDispatch * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Open )( 
            IShellDispatch * This,
            /* [in] */ VARIANT vDir);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Explore )( 
            IShellDispatch * This,
            /* [in] */ VARIANT vDir);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *MinimizeAll )( 
            IShellDispatch * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *UndoMinimizeALL )( 
            IShellDispatch * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *FileRun )( 
            IShellDispatch * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CascadeWindows )( 
            IShellDispatch * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *TileVertically )( 
            IShellDispatch * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *TileHorizontally )( 
            IShellDispatch * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ShutdownWindows )( 
            IShellDispatch * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Suspend )( 
            IShellDispatch * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *EjectPC )( 
            IShellDispatch * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetTime )( 
            IShellDispatch * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *TrayProperties )( 
            IShellDispatch * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Help )( 
            IShellDispatch * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *FindFiles )( 
            IShellDispatch * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *FindComputer )( 
            IShellDispatch * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *RefreshMenu )( 
            IShellDispatch * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ControlPanelItem )( 
            IShellDispatch * This,
            /* [in] */ BSTR szDir);
        
        END_INTERFACE
    } IShellDispatchVtbl;

    interface IShellDispatch
    {
        CONST_VTBL struct IShellDispatchVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellDispatch_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellDispatch_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellDispatch_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellDispatch_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IShellDispatch_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IShellDispatch_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IShellDispatch_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IShellDispatch_get_Application(This,ppid)	\
    (This)->lpVtbl -> get_Application(This,ppid)

#define IShellDispatch_get_Parent(This,ppid)	\
    (This)->lpVtbl -> get_Parent(This,ppid)

#define IShellDispatch_NameSpace(This,vDir,ppsdf)	\
    (This)->lpVtbl -> NameSpace(This,vDir,ppsdf)

#define IShellDispatch_BrowseForFolder(This,Hwnd,Title,Options,RootFolder,ppsdf)	\
    (This)->lpVtbl -> BrowseForFolder(This,Hwnd,Title,Options,RootFolder,ppsdf)

#define IShellDispatch_Windows(This,ppid)	\
    (This)->lpVtbl -> Windows(This,ppid)

#define IShellDispatch_Open(This,vDir)	\
    (This)->lpVtbl -> Open(This,vDir)

#define IShellDispatch_Explore(This,vDir)	\
    (This)->lpVtbl -> Explore(This,vDir)

#define IShellDispatch_MinimizeAll(This)	\
    (This)->lpVtbl -> MinimizeAll(This)

#define IShellDispatch_UndoMinimizeALL(This)	\
    (This)->lpVtbl -> UndoMinimizeALL(This)

#define IShellDispatch_FileRun(This)	\
    (This)->lpVtbl -> FileRun(This)

#define IShellDispatch_CascadeWindows(This)	\
    (This)->lpVtbl -> CascadeWindows(This)

#define IShellDispatch_TileVertically(This)	\
    (This)->lpVtbl -> TileVertically(This)

#define IShellDispatch_TileHorizontally(This)	\
    (This)->lpVtbl -> TileHorizontally(This)

#define IShellDispatch_ShutdownWindows(This)	\
    (This)->lpVtbl -> ShutdownWindows(This)

#define IShellDispatch_Suspend(This)	\
    (This)->lpVtbl -> Suspend(This)

#define IShellDispatch_EjectPC(This)	\
    (This)->lpVtbl -> EjectPC(This)

#define IShellDispatch_SetTime(This)	\
    (This)->lpVtbl -> SetTime(This)

#define IShellDispatch_TrayProperties(This)	\
    (This)->lpVtbl -> TrayProperties(This)

#define IShellDispatch_Help(This)	\
    (This)->lpVtbl -> Help(This)

#define IShellDispatch_FindFiles(This)	\
    (This)->lpVtbl -> FindFiles(This)

#define IShellDispatch_FindComputer(This)	\
    (This)->lpVtbl -> FindComputer(This)

#define IShellDispatch_RefreshMenu(This)	\
    (This)->lpVtbl -> RefreshMenu(This)

#define IShellDispatch_ControlPanelItem(This,szDir)	\
    (This)->lpVtbl -> ControlPanelItem(This,szDir)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IShellDispatch_get_Application_Proxy( 
    IShellDispatch * This,
    /* [retval][out] */ IDispatch **ppid);


void __RPC_STUB IShellDispatch_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IShellDispatch_get_Parent_Proxy( 
    IShellDispatch * This,
    /* [retval][out] */ IDispatch **ppid);


void __RPC_STUB IShellDispatch_get_Parent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_NameSpace_Proxy( 
    IShellDispatch * This,
    /* [in] */ VARIANT vDir,
    /* [retval][out] */ Folder **ppsdf);


void __RPC_STUB IShellDispatch_NameSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_BrowseForFolder_Proxy( 
    IShellDispatch * This,
    /* [in] */ long Hwnd,
    /* [in] */ BSTR Title,
    /* [in] */ long Options,
    /* [optional][in] */ VARIANT RootFolder,
    /* [retval][out] */ Folder **ppsdf);


void __RPC_STUB IShellDispatch_BrowseForFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_Windows_Proxy( 
    IShellDispatch * This,
    /* [retval][out] */ IDispatch **ppid);


void __RPC_STUB IShellDispatch_Windows_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_Open_Proxy( 
    IShellDispatch * This,
    /* [in] */ VARIANT vDir);


void __RPC_STUB IShellDispatch_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_Explore_Proxy( 
    IShellDispatch * This,
    /* [in] */ VARIANT vDir);


void __RPC_STUB IShellDispatch_Explore_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_MinimizeAll_Proxy( 
    IShellDispatch * This);


void __RPC_STUB IShellDispatch_MinimizeAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_UndoMinimizeALL_Proxy( 
    IShellDispatch * This);


void __RPC_STUB IShellDispatch_UndoMinimizeALL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_FileRun_Proxy( 
    IShellDispatch * This);


void __RPC_STUB IShellDispatch_FileRun_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_CascadeWindows_Proxy( 
    IShellDispatch * This);


void __RPC_STUB IShellDispatch_CascadeWindows_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_TileVertically_Proxy( 
    IShellDispatch * This);


void __RPC_STUB IShellDispatch_TileVertically_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_TileHorizontally_Proxy( 
    IShellDispatch * This);


void __RPC_STUB IShellDispatch_TileHorizontally_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_ShutdownWindows_Proxy( 
    IShellDispatch * This);


void __RPC_STUB IShellDispatch_ShutdownWindows_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_Suspend_Proxy( 
    IShellDispatch * This);


void __RPC_STUB IShellDispatch_Suspend_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_EjectPC_Proxy( 
    IShellDispatch * This);


void __RPC_STUB IShellDispatch_EjectPC_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_SetTime_Proxy( 
    IShellDispatch * This);


void __RPC_STUB IShellDispatch_SetTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_TrayProperties_Proxy( 
    IShellDispatch * This);


void __RPC_STUB IShellDispatch_TrayProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_Help_Proxy( 
    IShellDispatch * This);


void __RPC_STUB IShellDispatch_Help_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_FindFiles_Proxy( 
    IShellDispatch * This);


void __RPC_STUB IShellDispatch_FindFiles_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_FindComputer_Proxy( 
    IShellDispatch * This);


void __RPC_STUB IShellDispatch_FindComputer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_RefreshMenu_Proxy( 
    IShellDispatch * This);


void __RPC_STUB IShellDispatch_RefreshMenu_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch_ControlPanelItem_Proxy( 
    IShellDispatch * This,
    /* [in] */ BSTR szDir);


void __RPC_STUB IShellDispatch_ControlPanelItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellDispatch_INTERFACE_DEFINED__ */


#ifndef __IShellDispatch2_INTERFACE_DEFINED__
#define __IShellDispatch2_INTERFACE_DEFINED__

/* interface IShellDispatch2 */
/* [object][dual][hidden][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_IShellDispatch2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A4C6892C-3BA9-11d2-9DEA-00C04FB16162")
    IShellDispatch2 : public IShellDispatch
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE IsRestricted( 
            /* [in] */ BSTR Group,
            /* [in] */ BSTR Restriction,
            /* [retval][out] */ long *plRestrictValue) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ShellExecute( 
            /* [in] */ BSTR File,
            /* [optional][in] */ VARIANT vArgs,
            /* [optional][in] */ VARIANT vDir,
            /* [optional][in] */ VARIANT vOperation,
            /* [optional][in] */ VARIANT vShow) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE FindPrinter( 
            /* [optional][in] */ BSTR name,
            /* [optional][in] */ BSTR location,
            /* [optional][in] */ BSTR model) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetSystemInformation( 
            /* [in] */ BSTR name,
            /* [retval][out] */ VARIANT *pv) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ServiceStart( 
            /* [in] */ BSTR ServiceName,
            /* [in] */ VARIANT Persistent,
            /* [retval][out] */ VARIANT *pSuccess) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ServiceStop( 
            /* [in] */ BSTR ServiceName,
            /* [in] */ VARIANT Persistent,
            /* [retval][out] */ VARIANT *pSuccess) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE IsServiceRunning( 
            /* [in] */ BSTR ServiceName,
            /* [retval][out] */ VARIANT *pRunning) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CanStartStopService( 
            /* [in] */ BSTR ServiceName,
            /* [retval][out] */ VARIANT *pCanStartStop) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ShowBrowserBar( 
            /* [in] */ BSTR bstrClsid,
            /* [in] */ VARIANT bShow,
            /* [retval][out] */ VARIANT *pSuccess) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellDispatch2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellDispatch2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellDispatch2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellDispatch2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IShellDispatch2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IShellDispatch2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IShellDispatch2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IShellDispatch2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            IShellDispatch2 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parent )( 
            IShellDispatch2 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *NameSpace )( 
            IShellDispatch2 * This,
            /* [in] */ VARIANT vDir,
            /* [retval][out] */ Folder **ppsdf);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *BrowseForFolder )( 
            IShellDispatch2 * This,
            /* [in] */ long Hwnd,
            /* [in] */ BSTR Title,
            /* [in] */ long Options,
            /* [optional][in] */ VARIANT RootFolder,
            /* [retval][out] */ Folder **ppsdf);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Windows )( 
            IShellDispatch2 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Open )( 
            IShellDispatch2 * This,
            /* [in] */ VARIANT vDir);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Explore )( 
            IShellDispatch2 * This,
            /* [in] */ VARIANT vDir);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *MinimizeAll )( 
            IShellDispatch2 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *UndoMinimizeALL )( 
            IShellDispatch2 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *FileRun )( 
            IShellDispatch2 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CascadeWindows )( 
            IShellDispatch2 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *TileVertically )( 
            IShellDispatch2 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *TileHorizontally )( 
            IShellDispatch2 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ShutdownWindows )( 
            IShellDispatch2 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Suspend )( 
            IShellDispatch2 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *EjectPC )( 
            IShellDispatch2 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetTime )( 
            IShellDispatch2 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *TrayProperties )( 
            IShellDispatch2 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Help )( 
            IShellDispatch2 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *FindFiles )( 
            IShellDispatch2 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *FindComputer )( 
            IShellDispatch2 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *RefreshMenu )( 
            IShellDispatch2 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ControlPanelItem )( 
            IShellDispatch2 * This,
            /* [in] */ BSTR szDir);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *IsRestricted )( 
            IShellDispatch2 * This,
            /* [in] */ BSTR Group,
            /* [in] */ BSTR Restriction,
            /* [retval][out] */ long *plRestrictValue);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ShellExecute )( 
            IShellDispatch2 * This,
            /* [in] */ BSTR File,
            /* [optional][in] */ VARIANT vArgs,
            /* [optional][in] */ VARIANT vDir,
            /* [optional][in] */ VARIANT vOperation,
            /* [optional][in] */ VARIANT vShow);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *FindPrinter )( 
            IShellDispatch2 * This,
            /* [optional][in] */ BSTR name,
            /* [optional][in] */ BSTR location,
            /* [optional][in] */ BSTR model);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetSystemInformation )( 
            IShellDispatch2 * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ VARIANT *pv);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ServiceStart )( 
            IShellDispatch2 * This,
            /* [in] */ BSTR ServiceName,
            /* [in] */ VARIANT Persistent,
            /* [retval][out] */ VARIANT *pSuccess);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ServiceStop )( 
            IShellDispatch2 * This,
            /* [in] */ BSTR ServiceName,
            /* [in] */ VARIANT Persistent,
            /* [retval][out] */ VARIANT *pSuccess);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *IsServiceRunning )( 
            IShellDispatch2 * This,
            /* [in] */ BSTR ServiceName,
            /* [retval][out] */ VARIANT *pRunning);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CanStartStopService )( 
            IShellDispatch2 * This,
            /* [in] */ BSTR ServiceName,
            /* [retval][out] */ VARIANT *pCanStartStop);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ShowBrowserBar )( 
            IShellDispatch2 * This,
            /* [in] */ BSTR bstrClsid,
            /* [in] */ VARIANT bShow,
            /* [retval][out] */ VARIANT *pSuccess);
        
        END_INTERFACE
    } IShellDispatch2Vtbl;

    interface IShellDispatch2
    {
        CONST_VTBL struct IShellDispatch2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellDispatch2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellDispatch2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellDispatch2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellDispatch2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IShellDispatch2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IShellDispatch2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IShellDispatch2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IShellDispatch2_get_Application(This,ppid)	\
    (This)->lpVtbl -> get_Application(This,ppid)

#define IShellDispatch2_get_Parent(This,ppid)	\
    (This)->lpVtbl -> get_Parent(This,ppid)

#define IShellDispatch2_NameSpace(This,vDir,ppsdf)	\
    (This)->lpVtbl -> NameSpace(This,vDir,ppsdf)

#define IShellDispatch2_BrowseForFolder(This,Hwnd,Title,Options,RootFolder,ppsdf)	\
    (This)->lpVtbl -> BrowseForFolder(This,Hwnd,Title,Options,RootFolder,ppsdf)

#define IShellDispatch2_Windows(This,ppid)	\
    (This)->lpVtbl -> Windows(This,ppid)

#define IShellDispatch2_Open(This,vDir)	\
    (This)->lpVtbl -> Open(This,vDir)

#define IShellDispatch2_Explore(This,vDir)	\
    (This)->lpVtbl -> Explore(This,vDir)

#define IShellDispatch2_MinimizeAll(This)	\
    (This)->lpVtbl -> MinimizeAll(This)

#define IShellDispatch2_UndoMinimizeALL(This)	\
    (This)->lpVtbl -> UndoMinimizeALL(This)

#define IShellDispatch2_FileRun(This)	\
    (This)->lpVtbl -> FileRun(This)

#define IShellDispatch2_CascadeWindows(This)	\
    (This)->lpVtbl -> CascadeWindows(This)

#define IShellDispatch2_TileVertically(This)	\
    (This)->lpVtbl -> TileVertically(This)

#define IShellDispatch2_TileHorizontally(This)	\
    (This)->lpVtbl -> TileHorizontally(This)

#define IShellDispatch2_ShutdownWindows(This)	\
    (This)->lpVtbl -> ShutdownWindows(This)

#define IShellDispatch2_Suspend(This)	\
    (This)->lpVtbl -> Suspend(This)

#define IShellDispatch2_EjectPC(This)	\
    (This)->lpVtbl -> EjectPC(This)

#define IShellDispatch2_SetTime(This)	\
    (This)->lpVtbl -> SetTime(This)

#define IShellDispatch2_TrayProperties(This)	\
    (This)->lpVtbl -> TrayProperties(This)

#define IShellDispatch2_Help(This)	\
    (This)->lpVtbl -> Help(This)

#define IShellDispatch2_FindFiles(This)	\
    (This)->lpVtbl -> FindFiles(This)

#define IShellDispatch2_FindComputer(This)	\
    (This)->lpVtbl -> FindComputer(This)

#define IShellDispatch2_RefreshMenu(This)	\
    (This)->lpVtbl -> RefreshMenu(This)

#define IShellDispatch2_ControlPanelItem(This,szDir)	\
    (This)->lpVtbl -> ControlPanelItem(This,szDir)


#define IShellDispatch2_IsRestricted(This,Group,Restriction,plRestrictValue)	\
    (This)->lpVtbl -> IsRestricted(This,Group,Restriction,plRestrictValue)

#define IShellDispatch2_ShellExecute(This,File,vArgs,vDir,vOperation,vShow)	\
    (This)->lpVtbl -> ShellExecute(This,File,vArgs,vDir,vOperation,vShow)

#define IShellDispatch2_FindPrinter(This,name,location,model)	\
    (This)->lpVtbl -> FindPrinter(This,name,location,model)

#define IShellDispatch2_GetSystemInformation(This,name,pv)	\
    (This)->lpVtbl -> GetSystemInformation(This,name,pv)

#define IShellDispatch2_ServiceStart(This,ServiceName,Persistent,pSuccess)	\
    (This)->lpVtbl -> ServiceStart(This,ServiceName,Persistent,pSuccess)

#define IShellDispatch2_ServiceStop(This,ServiceName,Persistent,pSuccess)	\
    (This)->lpVtbl -> ServiceStop(This,ServiceName,Persistent,pSuccess)

#define IShellDispatch2_IsServiceRunning(This,ServiceName,pRunning)	\
    (This)->lpVtbl -> IsServiceRunning(This,ServiceName,pRunning)

#define IShellDispatch2_CanStartStopService(This,ServiceName,pCanStartStop)	\
    (This)->lpVtbl -> CanStartStopService(This,ServiceName,pCanStartStop)

#define IShellDispatch2_ShowBrowserBar(This,bstrClsid,bShow,pSuccess)	\
    (This)->lpVtbl -> ShowBrowserBar(This,bstrClsid,bShow,pSuccess)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch2_IsRestricted_Proxy( 
    IShellDispatch2 * This,
    /* [in] */ BSTR Group,
    /* [in] */ BSTR Restriction,
    /* [retval][out] */ long *plRestrictValue);


void __RPC_STUB IShellDispatch2_IsRestricted_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch2_ShellExecute_Proxy( 
    IShellDispatch2 * This,
    /* [in] */ BSTR File,
    /* [optional][in] */ VARIANT vArgs,
    /* [optional][in] */ VARIANT vDir,
    /* [optional][in] */ VARIANT vOperation,
    /* [optional][in] */ VARIANT vShow);


void __RPC_STUB IShellDispatch2_ShellExecute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch2_FindPrinter_Proxy( 
    IShellDispatch2 * This,
    /* [optional][in] */ BSTR name,
    /* [optional][in] */ BSTR location,
    /* [optional][in] */ BSTR model);


void __RPC_STUB IShellDispatch2_FindPrinter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch2_GetSystemInformation_Proxy( 
    IShellDispatch2 * This,
    /* [in] */ BSTR name,
    /* [retval][out] */ VARIANT *pv);


void __RPC_STUB IShellDispatch2_GetSystemInformation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch2_ServiceStart_Proxy( 
    IShellDispatch2 * This,
    /* [in] */ BSTR ServiceName,
    /* [in] */ VARIANT Persistent,
    /* [retval][out] */ VARIANT *pSuccess);


void __RPC_STUB IShellDispatch2_ServiceStart_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch2_ServiceStop_Proxy( 
    IShellDispatch2 * This,
    /* [in] */ BSTR ServiceName,
    /* [in] */ VARIANT Persistent,
    /* [retval][out] */ VARIANT *pSuccess);


void __RPC_STUB IShellDispatch2_ServiceStop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch2_IsServiceRunning_Proxy( 
    IShellDispatch2 * This,
    /* [in] */ BSTR ServiceName,
    /* [retval][out] */ VARIANT *pRunning);


void __RPC_STUB IShellDispatch2_IsServiceRunning_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch2_CanStartStopService_Proxy( 
    IShellDispatch2 * This,
    /* [in] */ BSTR ServiceName,
    /* [retval][out] */ VARIANT *pCanStartStop);


void __RPC_STUB IShellDispatch2_CanStartStopService_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch2_ShowBrowserBar_Proxy( 
    IShellDispatch2 * This,
    /* [in] */ BSTR bstrClsid,
    /* [in] */ VARIANT bShow,
    /* [retval][out] */ VARIANT *pSuccess);


void __RPC_STUB IShellDispatch2_ShowBrowserBar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellDispatch2_INTERFACE_DEFINED__ */


#ifndef __IShellDispatch3_INTERFACE_DEFINED__
#define __IShellDispatch3_INTERFACE_DEFINED__

/* interface IShellDispatch3 */
/* [object][dual][hidden][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_IShellDispatch3;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("177160ca-bb5a-411c-841d-bd38facdeaa0")
    IShellDispatch3 : public IShellDispatch2
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE AddToRecent( 
            /* [in] */ VARIANT varFile,
            /* [optional][in] */ BSTR bstrCategory) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellDispatch3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellDispatch3 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellDispatch3 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellDispatch3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IShellDispatch3 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IShellDispatch3 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IShellDispatch3 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IShellDispatch3 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            IShellDispatch3 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parent )( 
            IShellDispatch3 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *NameSpace )( 
            IShellDispatch3 * This,
            /* [in] */ VARIANT vDir,
            /* [retval][out] */ Folder **ppsdf);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *BrowseForFolder )( 
            IShellDispatch3 * This,
            /* [in] */ long Hwnd,
            /* [in] */ BSTR Title,
            /* [in] */ long Options,
            /* [optional][in] */ VARIANT RootFolder,
            /* [retval][out] */ Folder **ppsdf);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Windows )( 
            IShellDispatch3 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Open )( 
            IShellDispatch3 * This,
            /* [in] */ VARIANT vDir);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Explore )( 
            IShellDispatch3 * This,
            /* [in] */ VARIANT vDir);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *MinimizeAll )( 
            IShellDispatch3 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *UndoMinimizeALL )( 
            IShellDispatch3 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *FileRun )( 
            IShellDispatch3 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CascadeWindows )( 
            IShellDispatch3 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *TileVertically )( 
            IShellDispatch3 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *TileHorizontally )( 
            IShellDispatch3 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ShutdownWindows )( 
            IShellDispatch3 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Suspend )( 
            IShellDispatch3 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *EjectPC )( 
            IShellDispatch3 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetTime )( 
            IShellDispatch3 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *TrayProperties )( 
            IShellDispatch3 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Help )( 
            IShellDispatch3 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *FindFiles )( 
            IShellDispatch3 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *FindComputer )( 
            IShellDispatch3 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *RefreshMenu )( 
            IShellDispatch3 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ControlPanelItem )( 
            IShellDispatch3 * This,
            /* [in] */ BSTR szDir);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *IsRestricted )( 
            IShellDispatch3 * This,
            /* [in] */ BSTR Group,
            /* [in] */ BSTR Restriction,
            /* [retval][out] */ long *plRestrictValue);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ShellExecute )( 
            IShellDispatch3 * This,
            /* [in] */ BSTR File,
            /* [optional][in] */ VARIANT vArgs,
            /* [optional][in] */ VARIANT vDir,
            /* [optional][in] */ VARIANT vOperation,
            /* [optional][in] */ VARIANT vShow);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *FindPrinter )( 
            IShellDispatch3 * This,
            /* [optional][in] */ BSTR name,
            /* [optional][in] */ BSTR location,
            /* [optional][in] */ BSTR model);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetSystemInformation )( 
            IShellDispatch3 * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ VARIANT *pv);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ServiceStart )( 
            IShellDispatch3 * This,
            /* [in] */ BSTR ServiceName,
            /* [in] */ VARIANT Persistent,
            /* [retval][out] */ VARIANT *pSuccess);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ServiceStop )( 
            IShellDispatch3 * This,
            /* [in] */ BSTR ServiceName,
            /* [in] */ VARIANT Persistent,
            /* [retval][out] */ VARIANT *pSuccess);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *IsServiceRunning )( 
            IShellDispatch3 * This,
            /* [in] */ BSTR ServiceName,
            /* [retval][out] */ VARIANT *pRunning);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CanStartStopService )( 
            IShellDispatch3 * This,
            /* [in] */ BSTR ServiceName,
            /* [retval][out] */ VARIANT *pCanStartStop);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ShowBrowserBar )( 
            IShellDispatch3 * This,
            /* [in] */ BSTR bstrClsid,
            /* [in] */ VARIANT bShow,
            /* [retval][out] */ VARIANT *pSuccess);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddToRecent )( 
            IShellDispatch3 * This,
            /* [in] */ VARIANT varFile,
            /* [optional][in] */ BSTR bstrCategory);
        
        END_INTERFACE
    } IShellDispatch3Vtbl;

    interface IShellDispatch3
    {
        CONST_VTBL struct IShellDispatch3Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellDispatch3_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellDispatch3_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellDispatch3_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellDispatch3_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IShellDispatch3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IShellDispatch3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IShellDispatch3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IShellDispatch3_get_Application(This,ppid)	\
    (This)->lpVtbl -> get_Application(This,ppid)

#define IShellDispatch3_get_Parent(This,ppid)	\
    (This)->lpVtbl -> get_Parent(This,ppid)

#define IShellDispatch3_NameSpace(This,vDir,ppsdf)	\
    (This)->lpVtbl -> NameSpace(This,vDir,ppsdf)

#define IShellDispatch3_BrowseForFolder(This,Hwnd,Title,Options,RootFolder,ppsdf)	\
    (This)->lpVtbl -> BrowseForFolder(This,Hwnd,Title,Options,RootFolder,ppsdf)

#define IShellDispatch3_Windows(This,ppid)	\
    (This)->lpVtbl -> Windows(This,ppid)

#define IShellDispatch3_Open(This,vDir)	\
    (This)->lpVtbl -> Open(This,vDir)

#define IShellDispatch3_Explore(This,vDir)	\
    (This)->lpVtbl -> Explore(This,vDir)

#define IShellDispatch3_MinimizeAll(This)	\
    (This)->lpVtbl -> MinimizeAll(This)

#define IShellDispatch3_UndoMinimizeALL(This)	\
    (This)->lpVtbl -> UndoMinimizeALL(This)

#define IShellDispatch3_FileRun(This)	\
    (This)->lpVtbl -> FileRun(This)

#define IShellDispatch3_CascadeWindows(This)	\
    (This)->lpVtbl -> CascadeWindows(This)

#define IShellDispatch3_TileVertically(This)	\
    (This)->lpVtbl -> TileVertically(This)

#define IShellDispatch3_TileHorizontally(This)	\
    (This)->lpVtbl -> TileHorizontally(This)

#define IShellDispatch3_ShutdownWindows(This)	\
    (This)->lpVtbl -> ShutdownWindows(This)

#define IShellDispatch3_Suspend(This)	\
    (This)->lpVtbl -> Suspend(This)

#define IShellDispatch3_EjectPC(This)	\
    (This)->lpVtbl -> EjectPC(This)

#define IShellDispatch3_SetTime(This)	\
    (This)->lpVtbl -> SetTime(This)

#define IShellDispatch3_TrayProperties(This)	\
    (This)->lpVtbl -> TrayProperties(This)

#define IShellDispatch3_Help(This)	\
    (This)->lpVtbl -> Help(This)

#define IShellDispatch3_FindFiles(This)	\
    (This)->lpVtbl -> FindFiles(This)

#define IShellDispatch3_FindComputer(This)	\
    (This)->lpVtbl -> FindComputer(This)

#define IShellDispatch3_RefreshMenu(This)	\
    (This)->lpVtbl -> RefreshMenu(This)

#define IShellDispatch3_ControlPanelItem(This,szDir)	\
    (This)->lpVtbl -> ControlPanelItem(This,szDir)


#define IShellDispatch3_IsRestricted(This,Group,Restriction,plRestrictValue)	\
    (This)->lpVtbl -> IsRestricted(This,Group,Restriction,plRestrictValue)

#define IShellDispatch3_ShellExecute(This,File,vArgs,vDir,vOperation,vShow)	\
    (This)->lpVtbl -> ShellExecute(This,File,vArgs,vDir,vOperation,vShow)

#define IShellDispatch3_FindPrinter(This,name,location,model)	\
    (This)->lpVtbl -> FindPrinter(This,name,location,model)

#define IShellDispatch3_GetSystemInformation(This,name,pv)	\
    (This)->lpVtbl -> GetSystemInformation(This,name,pv)

#define IShellDispatch3_ServiceStart(This,ServiceName,Persistent,pSuccess)	\
    (This)->lpVtbl -> ServiceStart(This,ServiceName,Persistent,pSuccess)

#define IShellDispatch3_ServiceStop(This,ServiceName,Persistent,pSuccess)	\
    (This)->lpVtbl -> ServiceStop(This,ServiceName,Persistent,pSuccess)

#define IShellDispatch3_IsServiceRunning(This,ServiceName,pRunning)	\
    (This)->lpVtbl -> IsServiceRunning(This,ServiceName,pRunning)

#define IShellDispatch3_CanStartStopService(This,ServiceName,pCanStartStop)	\
    (This)->lpVtbl -> CanStartStopService(This,ServiceName,pCanStartStop)

#define IShellDispatch3_ShowBrowserBar(This,bstrClsid,bShow,pSuccess)	\
    (This)->lpVtbl -> ShowBrowserBar(This,bstrClsid,bShow,pSuccess)


#define IShellDispatch3_AddToRecent(This,varFile,bstrCategory)	\
    (This)->lpVtbl -> AddToRecent(This,varFile,bstrCategory)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch3_AddToRecent_Proxy( 
    IShellDispatch3 * This,
    /* [in] */ VARIANT varFile,
    /* [optional][in] */ BSTR bstrCategory);


void __RPC_STUB IShellDispatch3_AddToRecent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellDispatch3_INTERFACE_DEFINED__ */


#ifndef __IShellDispatch4_INTERFACE_DEFINED__
#define __IShellDispatch4_INTERFACE_DEFINED__

/* interface IShellDispatch4 */
/* [object][dual][hidden][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_IShellDispatch4;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("efd84b2d-4bcf-4298-be25-eb542a59fbda")
    IShellDispatch4 : public IShellDispatch3
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE WindowsSecurity( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ToggleDesktop( void) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ExplorerPolicy( 
            /* [in] */ BSTR bstrPolicyName,
            /* [retval][out] */ VARIANT *pValue) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetSetting( 
            /* [in] */ long lSetting,
            /* [retval][out] */ VARIANT_BOOL *pResult) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellDispatch4Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellDispatch4 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellDispatch4 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellDispatch4 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IShellDispatch4 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IShellDispatch4 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IShellDispatch4 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IShellDispatch4 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            IShellDispatch4 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Parent )( 
            IShellDispatch4 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *NameSpace )( 
            IShellDispatch4 * This,
            /* [in] */ VARIANT vDir,
            /* [retval][out] */ Folder **ppsdf);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *BrowseForFolder )( 
            IShellDispatch4 * This,
            /* [in] */ long Hwnd,
            /* [in] */ BSTR Title,
            /* [in] */ long Options,
            /* [optional][in] */ VARIANT RootFolder,
            /* [retval][out] */ Folder **ppsdf);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Windows )( 
            IShellDispatch4 * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Open )( 
            IShellDispatch4 * This,
            /* [in] */ VARIANT vDir);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Explore )( 
            IShellDispatch4 * This,
            /* [in] */ VARIANT vDir);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *MinimizeAll )( 
            IShellDispatch4 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *UndoMinimizeALL )( 
            IShellDispatch4 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *FileRun )( 
            IShellDispatch4 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CascadeWindows )( 
            IShellDispatch4 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *TileVertically )( 
            IShellDispatch4 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *TileHorizontally )( 
            IShellDispatch4 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ShutdownWindows )( 
            IShellDispatch4 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Suspend )( 
            IShellDispatch4 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *EjectPC )( 
            IShellDispatch4 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *SetTime )( 
            IShellDispatch4 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *TrayProperties )( 
            IShellDispatch4 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Help )( 
            IShellDispatch4 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *FindFiles )( 
            IShellDispatch4 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *FindComputer )( 
            IShellDispatch4 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *RefreshMenu )( 
            IShellDispatch4 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ControlPanelItem )( 
            IShellDispatch4 * This,
            /* [in] */ BSTR szDir);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *IsRestricted )( 
            IShellDispatch4 * This,
            /* [in] */ BSTR Group,
            /* [in] */ BSTR Restriction,
            /* [retval][out] */ long *plRestrictValue);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ShellExecute )( 
            IShellDispatch4 * This,
            /* [in] */ BSTR File,
            /* [optional][in] */ VARIANT vArgs,
            /* [optional][in] */ VARIANT vDir,
            /* [optional][in] */ VARIANT vOperation,
            /* [optional][in] */ VARIANT vShow);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *FindPrinter )( 
            IShellDispatch4 * This,
            /* [optional][in] */ BSTR name,
            /* [optional][in] */ BSTR location,
            /* [optional][in] */ BSTR model);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetSystemInformation )( 
            IShellDispatch4 * This,
            /* [in] */ BSTR name,
            /* [retval][out] */ VARIANT *pv);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ServiceStart )( 
            IShellDispatch4 * This,
            /* [in] */ BSTR ServiceName,
            /* [in] */ VARIANT Persistent,
            /* [retval][out] */ VARIANT *pSuccess);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ServiceStop )( 
            IShellDispatch4 * This,
            /* [in] */ BSTR ServiceName,
            /* [in] */ VARIANT Persistent,
            /* [retval][out] */ VARIANT *pSuccess);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *IsServiceRunning )( 
            IShellDispatch4 * This,
            /* [in] */ BSTR ServiceName,
            /* [retval][out] */ VARIANT *pRunning);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CanStartStopService )( 
            IShellDispatch4 * This,
            /* [in] */ BSTR ServiceName,
            /* [retval][out] */ VARIANT *pCanStartStop);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ShowBrowserBar )( 
            IShellDispatch4 * This,
            /* [in] */ BSTR bstrClsid,
            /* [in] */ VARIANT bShow,
            /* [retval][out] */ VARIANT *pSuccess);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *AddToRecent )( 
            IShellDispatch4 * This,
            /* [in] */ VARIANT varFile,
            /* [optional][in] */ BSTR bstrCategory);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *WindowsSecurity )( 
            IShellDispatch4 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ToggleDesktop )( 
            IShellDispatch4 * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ExplorerPolicy )( 
            IShellDispatch4 * This,
            /* [in] */ BSTR bstrPolicyName,
            /* [retval][out] */ VARIANT *pValue);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetSetting )( 
            IShellDispatch4 * This,
            /* [in] */ long lSetting,
            /* [retval][out] */ VARIANT_BOOL *pResult);
        
        END_INTERFACE
    } IShellDispatch4Vtbl;

    interface IShellDispatch4
    {
        CONST_VTBL struct IShellDispatch4Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellDispatch4_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellDispatch4_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellDispatch4_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellDispatch4_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IShellDispatch4_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IShellDispatch4_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IShellDispatch4_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IShellDispatch4_get_Application(This,ppid)	\
    (This)->lpVtbl -> get_Application(This,ppid)

#define IShellDispatch4_get_Parent(This,ppid)	\
    (This)->lpVtbl -> get_Parent(This,ppid)

#define IShellDispatch4_NameSpace(This,vDir,ppsdf)	\
    (This)->lpVtbl -> NameSpace(This,vDir,ppsdf)

#define IShellDispatch4_BrowseForFolder(This,Hwnd,Title,Options,RootFolder,ppsdf)	\
    (This)->lpVtbl -> BrowseForFolder(This,Hwnd,Title,Options,RootFolder,ppsdf)

#define IShellDispatch4_Windows(This,ppid)	\
    (This)->lpVtbl -> Windows(This,ppid)

#define IShellDispatch4_Open(This,vDir)	\
    (This)->lpVtbl -> Open(This,vDir)

#define IShellDispatch4_Explore(This,vDir)	\
    (This)->lpVtbl -> Explore(This,vDir)

#define IShellDispatch4_MinimizeAll(This)	\
    (This)->lpVtbl -> MinimizeAll(This)

#define IShellDispatch4_UndoMinimizeALL(This)	\
    (This)->lpVtbl -> UndoMinimizeALL(This)

#define IShellDispatch4_FileRun(This)	\
    (This)->lpVtbl -> FileRun(This)

#define IShellDispatch4_CascadeWindows(This)	\
    (This)->lpVtbl -> CascadeWindows(This)

#define IShellDispatch4_TileVertically(This)	\
    (This)->lpVtbl -> TileVertically(This)

#define IShellDispatch4_TileHorizontally(This)	\
    (This)->lpVtbl -> TileHorizontally(This)

#define IShellDispatch4_ShutdownWindows(This)	\
    (This)->lpVtbl -> ShutdownWindows(This)

#define IShellDispatch4_Suspend(This)	\
    (This)->lpVtbl -> Suspend(This)

#define IShellDispatch4_EjectPC(This)	\
    (This)->lpVtbl -> EjectPC(This)

#define IShellDispatch4_SetTime(This)	\
    (This)->lpVtbl -> SetTime(This)

#define IShellDispatch4_TrayProperties(This)	\
    (This)->lpVtbl -> TrayProperties(This)

#define IShellDispatch4_Help(This)	\
    (This)->lpVtbl -> Help(This)

#define IShellDispatch4_FindFiles(This)	\
    (This)->lpVtbl -> FindFiles(This)

#define IShellDispatch4_FindComputer(This)	\
    (This)->lpVtbl -> FindComputer(This)

#define IShellDispatch4_RefreshMenu(This)	\
    (This)->lpVtbl -> RefreshMenu(This)

#define IShellDispatch4_ControlPanelItem(This,szDir)	\
    (This)->lpVtbl -> ControlPanelItem(This,szDir)


#define IShellDispatch4_IsRestricted(This,Group,Restriction,plRestrictValue)	\
    (This)->lpVtbl -> IsRestricted(This,Group,Restriction,plRestrictValue)

#define IShellDispatch4_ShellExecute(This,File,vArgs,vDir,vOperation,vShow)	\
    (This)->lpVtbl -> ShellExecute(This,File,vArgs,vDir,vOperation,vShow)

#define IShellDispatch4_FindPrinter(This,name,location,model)	\
    (This)->lpVtbl -> FindPrinter(This,name,location,model)

#define IShellDispatch4_GetSystemInformation(This,name,pv)	\
    (This)->lpVtbl -> GetSystemInformation(This,name,pv)

#define IShellDispatch4_ServiceStart(This,ServiceName,Persistent,pSuccess)	\
    (This)->lpVtbl -> ServiceStart(This,ServiceName,Persistent,pSuccess)

#define IShellDispatch4_ServiceStop(This,ServiceName,Persistent,pSuccess)	\
    (This)->lpVtbl -> ServiceStop(This,ServiceName,Persistent,pSuccess)

#define IShellDispatch4_IsServiceRunning(This,ServiceName,pRunning)	\
    (This)->lpVtbl -> IsServiceRunning(This,ServiceName,pRunning)

#define IShellDispatch4_CanStartStopService(This,ServiceName,pCanStartStop)	\
    (This)->lpVtbl -> CanStartStopService(This,ServiceName,pCanStartStop)

#define IShellDispatch4_ShowBrowserBar(This,bstrClsid,bShow,pSuccess)	\
    (This)->lpVtbl -> ShowBrowserBar(This,bstrClsid,bShow,pSuccess)


#define IShellDispatch4_AddToRecent(This,varFile,bstrCategory)	\
    (This)->lpVtbl -> AddToRecent(This,varFile,bstrCategory)


#define IShellDispatch4_WindowsSecurity(This)	\
    (This)->lpVtbl -> WindowsSecurity(This)

#define IShellDispatch4_ToggleDesktop(This)	\
    (This)->lpVtbl -> ToggleDesktop(This)

#define IShellDispatch4_ExplorerPolicy(This,bstrPolicyName,pValue)	\
    (This)->lpVtbl -> ExplorerPolicy(This,bstrPolicyName,pValue)

#define IShellDispatch4_GetSetting(This,lSetting,pResult)	\
    (This)->lpVtbl -> GetSetting(This,lSetting,pResult)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch4_WindowsSecurity_Proxy( 
    IShellDispatch4 * This);


void __RPC_STUB IShellDispatch4_WindowsSecurity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch4_ToggleDesktop_Proxy( 
    IShellDispatch4 * This);


void __RPC_STUB IShellDispatch4_ToggleDesktop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch4_ExplorerPolicy_Proxy( 
    IShellDispatch4 * This,
    /* [in] */ BSTR bstrPolicyName,
    /* [retval][out] */ VARIANT *pValue);


void __RPC_STUB IShellDispatch4_ExplorerPolicy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IShellDispatch4_GetSetting_Proxy( 
    IShellDispatch4 * This,
    /* [in] */ long lSetting,
    /* [retval][out] */ VARIANT_BOOL *pResult);


void __RPC_STUB IShellDispatch4_GetSetting_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellDispatch4_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_Shell;

#ifdef __cplusplus

class DECLSPEC_UUID("13709620-C279-11CE-A49E-444553540000")
Shell;
#endif

EXTERN_C const CLSID CLSID_ShellDispatchInproc;

#ifdef __cplusplus

class DECLSPEC_UUID("0A89A860-D7B1-11CE-8350-444553540000")
ShellDispatchInproc;
#endif

EXTERN_C const CLSID CLSID_WebViewFolderContents;

#ifdef __cplusplus

class DECLSPEC_UUID("1820FED0-473E-11D0-A96C-00C04FD705A2")
WebViewFolderContents;
#endif

#ifndef __DSearchCommandEvents_DISPINTERFACE_DEFINED__
#define __DSearchCommandEvents_DISPINTERFACE_DEFINED__

/* dispinterface DSearchCommandEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID_DSearchCommandEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("60890160-69f0-11d1-b758-00a0c90564fe")
    DSearchCommandEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct DSearchCommandEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            DSearchCommandEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            DSearchCommandEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            DSearchCommandEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            DSearchCommandEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            DSearchCommandEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            DSearchCommandEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            DSearchCommandEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } DSearchCommandEventsVtbl;

    interface DSearchCommandEvents
    {
        CONST_VTBL struct DSearchCommandEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define DSearchCommandEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define DSearchCommandEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define DSearchCommandEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define DSearchCommandEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define DSearchCommandEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define DSearchCommandEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define DSearchCommandEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __DSearchCommandEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_SearchCommand;

#ifdef __cplusplus

class DECLSPEC_UUID("B005E690-678D-11d1-B758-00A0C90564FE")
SearchCommand;
#endif

#ifndef __IFileSearchBand_INTERFACE_DEFINED__
#define __IFileSearchBand_INTERFACE_DEFINED__

/* interface IFileSearchBand */
/* [object][unique][hidden][dual][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_IFileSearchBand;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2D91EEA1-9932-11d2-BE86-00A0C9A83DA1")
    IFileSearchBand : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetFocus( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetSearchParameters( 
            /* [in] */ BSTR *pbstrSearchID,
            /* [in] */ VARIANT_BOOL bNavToResults,
            /* [optional][in] */ VARIANT *pvarScope,
            /* [optional][in] */ VARIANT *pvarQueryFile) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_SearchID( 
            /* [retval][out] */ BSTR *pbstrSearchID) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Scope( 
            /* [retval][out] */ VARIANT *pvarScope) = 0;
        
        virtual /* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_QueryFile( 
            /* [retval][out] */ VARIANT *pvarFile) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFileSearchBandVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFileSearchBand * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFileSearchBand * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFileSearchBand * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IFileSearchBand * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IFileSearchBand * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IFileSearchBand * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IFileSearchBand * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetFocus )( 
            IFileSearchBand * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetSearchParameters )( 
            IFileSearchBand * This,
            /* [in] */ BSTR *pbstrSearchID,
            /* [in] */ VARIANT_BOOL bNavToResults,
            /* [optional][in] */ VARIANT *pvarScope,
            /* [optional][in] */ VARIANT *pvarQueryFile);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_SearchID )( 
            IFileSearchBand * This,
            /* [retval][out] */ BSTR *pbstrSearchID);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Scope )( 
            IFileSearchBand * This,
            /* [retval][out] */ VARIANT *pvarScope);
        
        /* [helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_QueryFile )( 
            IFileSearchBand * This,
            /* [retval][out] */ VARIANT *pvarFile);
        
        END_INTERFACE
    } IFileSearchBandVtbl;

    interface IFileSearchBand
    {
        CONST_VTBL struct IFileSearchBandVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFileSearchBand_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFileSearchBand_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFileSearchBand_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IFileSearchBand_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IFileSearchBand_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IFileSearchBand_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IFileSearchBand_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IFileSearchBand_SetFocus(This)	\
    (This)->lpVtbl -> SetFocus(This)

#define IFileSearchBand_SetSearchParameters(This,pbstrSearchID,bNavToResults,pvarScope,pvarQueryFile)	\
    (This)->lpVtbl -> SetSearchParameters(This,pbstrSearchID,bNavToResults,pvarScope,pvarQueryFile)

#define IFileSearchBand_get_SearchID(This,pbstrSearchID)	\
    (This)->lpVtbl -> get_SearchID(This,pbstrSearchID)

#define IFileSearchBand_get_Scope(This,pvarScope)	\
    (This)->lpVtbl -> get_Scope(This,pvarScope)

#define IFileSearchBand_get_QueryFile(This,pvarFile)	\
    (This)->lpVtbl -> get_QueryFile(This,pvarFile)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSearchBand_SetFocus_Proxy( 
    IFileSearchBand * This);


void __RPC_STUB IFileSearchBand_SetFocus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IFileSearchBand_SetSearchParameters_Proxy( 
    IFileSearchBand * This,
    /* [in] */ BSTR *pbstrSearchID,
    /* [in] */ VARIANT_BOOL bNavToResults,
    /* [optional][in] */ VARIANT *pvarScope,
    /* [optional][in] */ VARIANT *pvarQueryFile);


void __RPC_STUB IFileSearchBand_SetSearchParameters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFileSearchBand_get_SearchID_Proxy( 
    IFileSearchBand * This,
    /* [retval][out] */ BSTR *pbstrSearchID);


void __RPC_STUB IFileSearchBand_get_SearchID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFileSearchBand_get_Scope_Proxy( 
    IFileSearchBand * This,
    /* [retval][out] */ VARIANT *pvarScope);


void __RPC_STUB IFileSearchBand_get_Scope_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IFileSearchBand_get_QueryFile_Proxy( 
    IFileSearchBand * This,
    /* [retval][out] */ VARIANT *pvarFile);


void __RPC_STUB IFileSearchBand_get_QueryFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFileSearchBand_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_FileSearchBand;

#ifdef __cplusplus

class DECLSPEC_UUID("C4EE31F3-4768-11D2-BE5C-00A0C9A83DA1")
FileSearchBand;
#endif

#ifndef __IWebWizardHost_INTERFACE_DEFINED__
#define __IWebWizardHost_INTERFACE_DEFINED__

/* interface IWebWizardHost */
/* [helpstring][dual][object][uuid] */ 


EXTERN_C const IID IID_IWebWizardHost;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("18bcc359-4990-4bfb-b951-3c83702be5f9")
    IWebWizardHost : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE FinalBack( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE FinalNext( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Cancel( void) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Caption( 
            /* [in] */ BSTR bstrCaption) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Caption( 
            /* [retval][out] */ BSTR *pbstrCaption) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Property( 
            /* [in] */ BSTR bstrPropertyName,
            /* [in] */ VARIANT *pvProperty) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Property( 
            /* [in] */ BSTR bstrPropertyName,
            /* [retval][out] */ VARIANT *pvProperty) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetWizardButtons( 
            /* [in] */ VARIANT_BOOL vfEnableBack,
            /* [in] */ VARIANT_BOOL vfEnableNext,
            /* [in] */ VARIANT_BOOL vfLastPage) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetHeaderText( 
            /* [in] */ BSTR bstrHeaderTitle,
            /* [in] */ BSTR bstrHeaderSubtitle) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWebWizardHostVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebWizardHost * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebWizardHost * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebWizardHost * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IWebWizardHost * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IWebWizardHost * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IWebWizardHost * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebWizardHost * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FinalBack )( 
            IWebWizardHost * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FinalNext )( 
            IWebWizardHost * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Cancel )( 
            IWebWizardHost * This);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Caption )( 
            IWebWizardHost * This,
            /* [in] */ BSTR bstrCaption);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Caption )( 
            IWebWizardHost * This,
            /* [retval][out] */ BSTR *pbstrCaption);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Property )( 
            IWebWizardHost * This,
            /* [in] */ BSTR bstrPropertyName,
            /* [in] */ VARIANT *pvProperty);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Property )( 
            IWebWizardHost * This,
            /* [in] */ BSTR bstrPropertyName,
            /* [retval][out] */ VARIANT *pvProperty);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetWizardButtons )( 
            IWebWizardHost * This,
            /* [in] */ VARIANT_BOOL vfEnableBack,
            /* [in] */ VARIANT_BOOL vfEnableNext,
            /* [in] */ VARIANT_BOOL vfLastPage);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetHeaderText )( 
            IWebWizardHost * This,
            /* [in] */ BSTR bstrHeaderTitle,
            /* [in] */ BSTR bstrHeaderSubtitle);
        
        END_INTERFACE
    } IWebWizardHostVtbl;

    interface IWebWizardHost
    {
        CONST_VTBL struct IWebWizardHostVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebWizardHost_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWebWizardHost_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWebWizardHost_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWebWizardHost_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWebWizardHost_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWebWizardHost_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWebWizardHost_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWebWizardHost_FinalBack(This)	\
    (This)->lpVtbl -> FinalBack(This)

#define IWebWizardHost_FinalNext(This)	\
    (This)->lpVtbl -> FinalNext(This)

#define IWebWizardHost_Cancel(This)	\
    (This)->lpVtbl -> Cancel(This)

#define IWebWizardHost_put_Caption(This,bstrCaption)	\
    (This)->lpVtbl -> put_Caption(This,bstrCaption)

#define IWebWizardHost_get_Caption(This,pbstrCaption)	\
    (This)->lpVtbl -> get_Caption(This,pbstrCaption)

#define IWebWizardHost_put_Property(This,bstrPropertyName,pvProperty)	\
    (This)->lpVtbl -> put_Property(This,bstrPropertyName,pvProperty)

#define IWebWizardHost_get_Property(This,bstrPropertyName,pvProperty)	\
    (This)->lpVtbl -> get_Property(This,bstrPropertyName,pvProperty)

#define IWebWizardHost_SetWizardButtons(This,vfEnableBack,vfEnableNext,vfLastPage)	\
    (This)->lpVtbl -> SetWizardButtons(This,vfEnableBack,vfEnableNext,vfLastPage)

#define IWebWizardHost_SetHeaderText(This,bstrHeaderTitle,bstrHeaderSubtitle)	\
    (This)->lpVtbl -> SetHeaderText(This,bstrHeaderTitle,bstrHeaderSubtitle)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE IWebWizardHost_FinalBack_Proxy( 
    IWebWizardHost * This);


void __RPC_STUB IWebWizardHost_FinalBack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IWebWizardHost_FinalNext_Proxy( 
    IWebWizardHost * This);


void __RPC_STUB IWebWizardHost_FinalNext_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IWebWizardHost_Cancel_Proxy( 
    IWebWizardHost * This);


void __RPC_STUB IWebWizardHost_Cancel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IWebWizardHost_put_Caption_Proxy( 
    IWebWizardHost * This,
    /* [in] */ BSTR bstrCaption);


void __RPC_STUB IWebWizardHost_put_Caption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IWebWizardHost_get_Caption_Proxy( 
    IWebWizardHost * This,
    /* [retval][out] */ BSTR *pbstrCaption);


void __RPC_STUB IWebWizardHost_get_Caption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IWebWizardHost_put_Property_Proxy( 
    IWebWizardHost * This,
    /* [in] */ BSTR bstrPropertyName,
    /* [in] */ VARIANT *pvProperty);


void __RPC_STUB IWebWizardHost_put_Property_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IWebWizardHost_get_Property_Proxy( 
    IWebWizardHost * This,
    /* [in] */ BSTR bstrPropertyName,
    /* [retval][out] */ VARIANT *pvProperty);


void __RPC_STUB IWebWizardHost_get_Property_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IWebWizardHost_SetWizardButtons_Proxy( 
    IWebWizardHost * This,
    /* [in] */ VARIANT_BOOL vfEnableBack,
    /* [in] */ VARIANT_BOOL vfEnableNext,
    /* [in] */ VARIANT_BOOL vfLastPage);


void __RPC_STUB IWebWizardHost_SetWizardButtons_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IWebWizardHost_SetHeaderText_Proxy( 
    IWebWizardHost * This,
    /* [in] */ BSTR bstrHeaderTitle,
    /* [in] */ BSTR bstrHeaderSubtitle);


void __RPC_STUB IWebWizardHost_SetHeaderText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWebWizardHost_INTERFACE_DEFINED__ */


#ifndef __INewWDEvents_INTERFACE_DEFINED__
#define __INewWDEvents_INTERFACE_DEFINED__

/* interface INewWDEvents */
/* [helpstring][dual][object][uuid] */ 


EXTERN_C const IID IID_INewWDEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0751c551-7568-41c9-8e5b-e22e38919236")
    INewWDEvents : public IWebWizardHost
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE PassportAuthenticate( 
            /* [in] */ BSTR bstrSignInUrl,
            /* [retval][out] */ VARIANT_BOOL *pvfAuthenitcated) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INewWDEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INewWDEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INewWDEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INewWDEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            INewWDEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            INewWDEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            INewWDEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            INewWDEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FinalBack )( 
            INewWDEvents * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FinalNext )( 
            INewWDEvents * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Cancel )( 
            INewWDEvents * This);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Caption )( 
            INewWDEvents * This,
            /* [in] */ BSTR bstrCaption);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Caption )( 
            INewWDEvents * This,
            /* [retval][out] */ BSTR *pbstrCaption);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Property )( 
            INewWDEvents * This,
            /* [in] */ BSTR bstrPropertyName,
            /* [in] */ VARIANT *pvProperty);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Property )( 
            INewWDEvents * This,
            /* [in] */ BSTR bstrPropertyName,
            /* [retval][out] */ VARIANT *pvProperty);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetWizardButtons )( 
            INewWDEvents * This,
            /* [in] */ VARIANT_BOOL vfEnableBack,
            /* [in] */ VARIANT_BOOL vfEnableNext,
            /* [in] */ VARIANT_BOOL vfLastPage);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetHeaderText )( 
            INewWDEvents * This,
            /* [in] */ BSTR bstrHeaderTitle,
            /* [in] */ BSTR bstrHeaderSubtitle);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PassportAuthenticate )( 
            INewWDEvents * This,
            /* [in] */ BSTR bstrSignInUrl,
            /* [retval][out] */ VARIANT_BOOL *pvfAuthenitcated);
        
        END_INTERFACE
    } INewWDEventsVtbl;

    interface INewWDEvents
    {
        CONST_VTBL struct INewWDEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INewWDEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INewWDEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INewWDEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INewWDEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INewWDEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INewWDEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INewWDEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INewWDEvents_FinalBack(This)	\
    (This)->lpVtbl -> FinalBack(This)

#define INewWDEvents_FinalNext(This)	\
    (This)->lpVtbl -> FinalNext(This)

#define INewWDEvents_Cancel(This)	\
    (This)->lpVtbl -> Cancel(This)

#define INewWDEvents_put_Caption(This,bstrCaption)	\
    (This)->lpVtbl -> put_Caption(This,bstrCaption)

#define INewWDEvents_get_Caption(This,pbstrCaption)	\
    (This)->lpVtbl -> get_Caption(This,pbstrCaption)

#define INewWDEvents_put_Property(This,bstrPropertyName,pvProperty)	\
    (This)->lpVtbl -> put_Property(This,bstrPropertyName,pvProperty)

#define INewWDEvents_get_Property(This,bstrPropertyName,pvProperty)	\
    (This)->lpVtbl -> get_Property(This,bstrPropertyName,pvProperty)

#define INewWDEvents_SetWizardButtons(This,vfEnableBack,vfEnableNext,vfLastPage)	\
    (This)->lpVtbl -> SetWizardButtons(This,vfEnableBack,vfEnableNext,vfLastPage)

#define INewWDEvents_SetHeaderText(This,bstrHeaderTitle,bstrHeaderSubtitle)	\
    (This)->lpVtbl -> SetHeaderText(This,bstrHeaderTitle,bstrHeaderSubtitle)


#define INewWDEvents_PassportAuthenticate(This,bstrSignInUrl,pvfAuthenitcated)	\
    (This)->lpVtbl -> PassportAuthenticate(This,bstrSignInUrl,pvfAuthenitcated)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE INewWDEvents_PassportAuthenticate_Proxy( 
    INewWDEvents * This,
    /* [in] */ BSTR bstrSignInUrl,
    /* [retval][out] */ VARIANT_BOOL *pvfAuthenitcated);


void __RPC_STUB INewWDEvents_PassportAuthenticate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INewWDEvents_INTERFACE_DEFINED__ */


#ifndef __IPassportClientServices_INTERFACE_DEFINED__
#define __IPassportClientServices_INTERFACE_DEFINED__

/* interface IPassportClientServices */
/* [helpstring][dual][object][uuid] */ 


EXTERN_C const IID IID_IPassportClientServices;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("b30f7305-5967-45d1-b7bc-d6eb7163d770")
    IPassportClientServices : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE MemberExists( 
            /* [in] */ BSTR bstrUser,
            /* [in] */ BSTR bstrPassword,
            /* [retval][out] */ VARIANT_BOOL *pvfExists) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPassportClientServicesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPassportClientServices * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPassportClientServices * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPassportClientServices * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IPassportClientServices * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IPassportClientServices * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IPassportClientServices * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IPassportClientServices * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *MemberExists )( 
            IPassportClientServices * This,
            /* [in] */ BSTR bstrUser,
            /* [in] */ BSTR bstrPassword,
            /* [retval][out] */ VARIANT_BOOL *pvfExists);
        
        END_INTERFACE
    } IPassportClientServicesVtbl;

    interface IPassportClientServices
    {
        CONST_VTBL struct IPassportClientServicesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPassportClientServices_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPassportClientServices_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPassportClientServices_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPassportClientServices_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IPassportClientServices_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IPassportClientServices_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IPassportClientServices_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IPassportClientServices_MemberExists(This,bstrUser,bstrPassword,pvfExists)	\
    (This)->lpVtbl -> MemberExists(This,bstrUser,bstrPassword,pvfExists)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE IPassportClientServices_MemberExists_Proxy( 
    IPassportClientServices * This,
    /* [in] */ BSTR bstrUser,
    /* [in] */ BSTR bstrPassword,
    /* [retval][out] */ VARIANT_BOOL *pvfExists);


void __RPC_STUB IPassportClientServices_MemberExists_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPassportClientServices_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_PassportClientServices;

#ifdef __cplusplus

class DECLSPEC_UUID("2d2307c8-7db4-40d6-9100-d52af4f97a5b")
PassportClientServices;
#endif
#endif /* __Shell32_LIBRARY_DEFINED__ */

/* interface __MIDL_itf_shldisp_0287 */
/* [local] */ 


//-------------------------------------------------------------------------
//
// IAutoComplete interface
//
//
// [Member functions]
//
// IAutoComplete::Init(hwndEdit, punkACL, pwszRegKeyPath, pwszQuickComplete)
//   This function initializes an AutoComplete object, telling it
//   what control to subclass, and what list of strings to process.
//
// IAutoComplete::Enable(fEnable)
//   This function enables or disables the AutoComplete functionality.
//
//-------------------------------------------------------------------------


extern RPC_IF_HANDLE __MIDL_itf_shldisp_0287_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shldisp_0287_v0_0_s_ifspec;

#ifndef __IAutoComplete_INTERFACE_DEFINED__
#define __IAutoComplete_INTERFACE_DEFINED__

/* interface IAutoComplete */
/* [unique][uuid][object][local][helpstring] */ 

typedef /* [unique] */ IAutoComplete *LPAUTOCOMPLETE;


EXTERN_C const IID IID_IAutoComplete;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("00bb2762-6a77-11d0-a535-00c04fd7d062")
    IAutoComplete : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ HWND hwndEdit,
            /* [unique][in] */ IUnknown *punkACL,
            /* [unique][in] */ LPCOLESTR pwszRegKeyPath,
            /* [in] */ LPCOLESTR pwszQuickComplete) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Enable( 
            /* [in] */ BOOL fEnable) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAutoCompleteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAutoComplete * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAutoComplete * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAutoComplete * This);
        
        HRESULT ( STDMETHODCALLTYPE *Init )( 
            IAutoComplete * This,
            /* [in] */ HWND hwndEdit,
            /* [unique][in] */ IUnknown *punkACL,
            /* [unique][in] */ LPCOLESTR pwszRegKeyPath,
            /* [in] */ LPCOLESTR pwszQuickComplete);
        
        HRESULT ( STDMETHODCALLTYPE *Enable )( 
            IAutoComplete * This,
            /* [in] */ BOOL fEnable);
        
        END_INTERFACE
    } IAutoCompleteVtbl;

    interface IAutoComplete
    {
        CONST_VTBL struct IAutoCompleteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAutoComplete_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAutoComplete_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAutoComplete_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAutoComplete_Init(This,hwndEdit,punkACL,pwszRegKeyPath,pwszQuickComplete)	\
    (This)->lpVtbl -> Init(This,hwndEdit,punkACL,pwszRegKeyPath,pwszQuickComplete)

#define IAutoComplete_Enable(This,fEnable)	\
    (This)->lpVtbl -> Enable(This,fEnable)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IAutoComplete_Init_Proxy( 
    IAutoComplete * This,
    /* [in] */ HWND hwndEdit,
    /* [unique][in] */ IUnknown *punkACL,
    /* [unique][in] */ LPCOLESTR pwszRegKeyPath,
    /* [in] */ LPCOLESTR pwszQuickComplete);


void __RPC_STUB IAutoComplete_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAutoComplete_Enable_Proxy( 
    IAutoComplete * This,
    /* [in] */ BOOL fEnable);


void __RPC_STUB IAutoComplete_Enable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAutoComplete_INTERFACE_DEFINED__ */


#ifndef __IAutoComplete2_INTERFACE_DEFINED__
#define __IAutoComplete2_INTERFACE_DEFINED__

/* interface IAutoComplete2 */
/* [unique][uuid][object][local][helpstring] */ 

typedef /* [unique] */ IAutoComplete2 *LPAUTOCOMPLETE2;

typedef 
enum _tagAUTOCOMPLETEOPTIONS
    {	ACO_NONE	= 0,
	ACO_AUTOSUGGEST	= 0x1,
	ACO_AUTOAPPEND	= 0x2,
	ACO_SEARCH	= 0x4,
	ACO_FILTERPREFIXES	= 0x8,
	ACO_USETAB	= 0x10,
	ACO_UPDOWNKEYDROPSLIST	= 0x20,
	ACO_RTLREADING	= 0x40
    } 	AUTOCOMPLETEOPTIONS;


EXTERN_C const IID IID_IAutoComplete2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EAC04BC0-3791-11d2-BB95-0060977B464C")
    IAutoComplete2 : public IAutoComplete
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetOptions( 
            /* [in] */ DWORD dwFlag) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetOptions( 
            /* [out] */ DWORD *pdwFlag) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAutoComplete2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAutoComplete2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAutoComplete2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAutoComplete2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *Init )( 
            IAutoComplete2 * This,
            /* [in] */ HWND hwndEdit,
            /* [unique][in] */ IUnknown *punkACL,
            /* [unique][in] */ LPCOLESTR pwszRegKeyPath,
            /* [in] */ LPCOLESTR pwszQuickComplete);
        
        HRESULT ( STDMETHODCALLTYPE *Enable )( 
            IAutoComplete2 * This,
            /* [in] */ BOOL fEnable);
        
        HRESULT ( STDMETHODCALLTYPE *SetOptions )( 
            IAutoComplete2 * This,
            /* [in] */ DWORD dwFlag);
        
        HRESULT ( STDMETHODCALLTYPE *GetOptions )( 
            IAutoComplete2 * This,
            /* [out] */ DWORD *pdwFlag);
        
        END_INTERFACE
    } IAutoComplete2Vtbl;

    interface IAutoComplete2
    {
        CONST_VTBL struct IAutoComplete2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAutoComplete2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAutoComplete2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAutoComplete2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAutoComplete2_Init(This,hwndEdit,punkACL,pwszRegKeyPath,pwszQuickComplete)	\
    (This)->lpVtbl -> Init(This,hwndEdit,punkACL,pwszRegKeyPath,pwszQuickComplete)

#define IAutoComplete2_Enable(This,fEnable)	\
    (This)->lpVtbl -> Enable(This,fEnable)


#define IAutoComplete2_SetOptions(This,dwFlag)	\
    (This)->lpVtbl -> SetOptions(This,dwFlag)

#define IAutoComplete2_GetOptions(This,pdwFlag)	\
    (This)->lpVtbl -> GetOptions(This,pdwFlag)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IAutoComplete2_SetOptions_Proxy( 
    IAutoComplete2 * This,
    /* [in] */ DWORD dwFlag);


void __RPC_STUB IAutoComplete2_SetOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAutoComplete2_GetOptions_Proxy( 
    IAutoComplete2 * This,
    /* [out] */ DWORD *pdwFlag);


void __RPC_STUB IAutoComplete2_GetOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAutoComplete2_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shldisp_0289 */
/* [local] */ 

// INTERFACE: IEnumACString
//
// This interface was implemented to return autocomplete strings
// into the caller's buffer (to reduce the number of memory allocations).
// A sort index is also returned to control the order of items displayed.
// by autocomplete.  The sort index should be set to zero if unused.
//
// The NextItem method increments the current index by one (similar to Next
// when one item is requested).
//


extern RPC_IF_HANDLE __MIDL_itf_shldisp_0289_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shldisp_0289_v0_0_s_ifspec;

#ifndef __IEnumACString_INTERFACE_DEFINED__
#define __IEnumACString_INTERFACE_DEFINED__

/* interface IEnumACString */
/* [unique][uuid][object][local][helpstring] */ 

typedef /* [unique] */ IEnumACString *PENUMACSTRING;

typedef /* [unique] */ IEnumACString *LPENUMACSTRING;

typedef 
enum _tagACENUMOPTION
    {	ACEO_NONE	= 0,
	ACEO_MOSTRECENTFIRST	= 0x1,
	ACEO_FIRSTUNUSED	= 0x10000
    } 	ACENUMOPTION;


EXTERN_C const IID IID_IEnumACString;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8E74C210-CF9D-4eaf-A403-7356428F0A5A")
    IEnumACString : public IEnumString
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE NextItem( 
            /* [size_is][unique][string][out] */ LPOLESTR pszUrl,
            /* [in] */ ULONG cchMax,
            /* [out] */ ULONG *pulSortIndex) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetEnumOptions( 
            /* [in] */ DWORD dwOptions) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEnumOptions( 
            /* [out] */ DWORD *pdwOptions) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumACStringVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumACString * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumACString * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumACString * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumACString * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ LPOLESTR *rgelt,
            /* [out] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumACString * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumACString * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumACString * This,
            /* [out] */ IEnumString **ppenum);
        
        HRESULT ( STDMETHODCALLTYPE *NextItem )( 
            IEnumACString * This,
            /* [size_is][unique][string][out] */ LPOLESTR pszUrl,
            /* [in] */ ULONG cchMax,
            /* [out] */ ULONG *pulSortIndex);
        
        HRESULT ( STDMETHODCALLTYPE *SetEnumOptions )( 
            IEnumACString * This,
            /* [in] */ DWORD dwOptions);
        
        HRESULT ( STDMETHODCALLTYPE *GetEnumOptions )( 
            IEnumACString * This,
            /* [out] */ DWORD *pdwOptions);
        
        END_INTERFACE
    } IEnumACStringVtbl;

    interface IEnumACString
    {
        CONST_VTBL struct IEnumACStringVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumACString_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumACString_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumACString_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumACString_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumACString_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumACString_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumACString_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)


#define IEnumACString_NextItem(This,pszUrl,cchMax,pulSortIndex)	\
    (This)->lpVtbl -> NextItem(This,pszUrl,cchMax,pulSortIndex)

#define IEnumACString_SetEnumOptions(This,dwOptions)	\
    (This)->lpVtbl -> SetEnumOptions(This,dwOptions)

#define IEnumACString_GetEnumOptions(This,pdwOptions)	\
    (This)->lpVtbl -> GetEnumOptions(This,pdwOptions)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumACString_NextItem_Proxy( 
    IEnumACString * This,
    /* [size_is][unique][string][out] */ LPOLESTR pszUrl,
    /* [in] */ ULONG cchMax,
    /* [out] */ ULONG *pulSortIndex);


void __RPC_STUB IEnumACString_NextItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumACString_SetEnumOptions_Proxy( 
    IEnumACString * This,
    /* [in] */ DWORD dwOptions);


void __RPC_STUB IEnumACString_SetEnumOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumACString_GetEnumOptions_Proxy( 
    IEnumACString * This,
    /* [out] */ DWORD *pdwOptions);


void __RPC_STUB IEnumACString_GetEnumOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumACString_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shldisp_0290 */
/* [local] */ 

// INTERFACE: IAsyncOperation
//
// This interface was implemented to turn some previously synchronous
// interfaces into async.  The following example is for
// doing the IDataObject::Drop() operation asynchronously.
//
// Sometimes the rendering of the IDataObject data (IDataObject::GetData() or
// STGMEDIUM.pStream->Read()) can be time intensive.  The IDropTarget
// may want to do this on another thread.
//
// Implimentation Check list:
// DoDragDrop Caller:
//    If this code can support asynch operations, then it needs to
//    QueryInterface() the IDataObject for IAsyncOperation.
//    IAsyncOperation::SetAsyncMode(VARIANT_TRUE).
//    After calling DoDragDrop(), call InOperation().  If any call fails
//    or InOperation() return FALSE, use the pdwEffect returned by DoDragDrop()
//    and the operation completed synchrously.
//
// OleSetClipboard Caller:
//    If this code can support asynch operations, then it needs to
//    QueryInterface() the IDataObject for IAsyncOperation.  Then call
//    IAsyncOperation::SetAsyncMode(VARIANT_TRUE).
//    If any of that fails, the final dwEffect should be passed to the IDataObject via
//    CFSTR_PERFORMEDDROPEFFECT.
//
// IDataObect Object:
//    IAsyncOperation::GetAsyncMode() should return whatever was last passed in
//          fDoOpAsync to ::SetAsyncMode() or VARIANT_FALSE if ::SetAsyncMode()
//          was never called.
//    IAsyncOperation::SetAsyncMode() should AddRef and store paocb.
//    IAsyncOperation::StartOperation() should store the fact that this was called and
//          cause InOperation() to return VARIANT_TRUE.  pbcReserved is not used and needs
//          to be NULL.
//    IAsyncOperation::InOperation() should return VARIANT_TRUE only if ::StartOperation()
//          was called.
//    IAsyncOperation::EndOperation() needs to call paocbpaocb->EndOperation() with the same
//          parameters.  Then release paocb.
//    IDataObject::SetData(CFSTR_PERFORMEDDROPEFFECT) When this happens, call
//          EndOperation(<into VAR>S_OK, NULL, <into VAR>dwEffect) and pass the dwEffect from the hglobal.
//
// IDropTarget Object:
//    IDropTarget::Drop() If asynch operations aren't supported, nothing is required.
//          The asynch operation can only happen if GetAsyncMode() returns VARIANT_TRUE.
//          Before starting the asynch operation, StartOperation(NULL) needs to be called before
//          returning from IDropTarget::Drop().



extern RPC_IF_HANDLE __MIDL_itf_shldisp_0290_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shldisp_0290_v0_0_s_ifspec;

#ifndef __IAsyncOperation_INTERFACE_DEFINED__
#define __IAsyncOperation_INTERFACE_DEFINED__

/* interface IAsyncOperation */
/* [object][uuid][helpstring] */ 

typedef /* [unique] */ IAsyncOperation *LPASYNCOPERATION;


EXTERN_C const IID IID_IAsyncOperation;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3D8B0590-F691-11d2-8EA9-006097DF5BD4")
    IAsyncOperation : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetAsyncMode( 
            /* [in] */ BOOL fDoOpAsync) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAsyncMode( 
            /* [out] */ BOOL *pfIsOpAsync) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StartOperation( 
            /* [optional][unique][in] */ IBindCtx *pbcReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InOperation( 
            /* [out] */ BOOL *pfInAsyncOp) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EndOperation( 
            /* [in] */ HRESULT hResult,
            /* [unique][in] */ IBindCtx *pbcReserved,
            /* [in] */ DWORD dwEffects) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAsyncOperationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAsyncOperation * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAsyncOperation * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAsyncOperation * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetAsyncMode )( 
            IAsyncOperation * This,
            /* [in] */ BOOL fDoOpAsync);
        
        HRESULT ( STDMETHODCALLTYPE *GetAsyncMode )( 
            IAsyncOperation * This,
            /* [out] */ BOOL *pfIsOpAsync);
        
        HRESULT ( STDMETHODCALLTYPE *StartOperation )( 
            IAsyncOperation * This,
            /* [optional][unique][in] */ IBindCtx *pbcReserved);
        
        HRESULT ( STDMETHODCALLTYPE *InOperation )( 
            IAsyncOperation * This,
            /* [out] */ BOOL *pfInAsyncOp);
        
        HRESULT ( STDMETHODCALLTYPE *EndOperation )( 
            IAsyncOperation * This,
            /* [in] */ HRESULT hResult,
            /* [unique][in] */ IBindCtx *pbcReserved,
            /* [in] */ DWORD dwEffects);
        
        END_INTERFACE
    } IAsyncOperationVtbl;

    interface IAsyncOperation
    {
        CONST_VTBL struct IAsyncOperationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAsyncOperation_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAsyncOperation_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAsyncOperation_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAsyncOperation_SetAsyncMode(This,fDoOpAsync)	\
    (This)->lpVtbl -> SetAsyncMode(This,fDoOpAsync)

#define IAsyncOperation_GetAsyncMode(This,pfIsOpAsync)	\
    (This)->lpVtbl -> GetAsyncMode(This,pfIsOpAsync)

#define IAsyncOperation_StartOperation(This,pbcReserved)	\
    (This)->lpVtbl -> StartOperation(This,pbcReserved)

#define IAsyncOperation_InOperation(This,pfInAsyncOp)	\
    (This)->lpVtbl -> InOperation(This,pfInAsyncOp)

#define IAsyncOperation_EndOperation(This,hResult,pbcReserved,dwEffects)	\
    (This)->lpVtbl -> EndOperation(This,hResult,pbcReserved,dwEffects)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IAsyncOperation_SetAsyncMode_Proxy( 
    IAsyncOperation * This,
    /* [in] */ BOOL fDoOpAsync);


void __RPC_STUB IAsyncOperation_SetAsyncMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAsyncOperation_GetAsyncMode_Proxy( 
    IAsyncOperation * This,
    /* [out] */ BOOL *pfIsOpAsync);


void __RPC_STUB IAsyncOperation_GetAsyncMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAsyncOperation_StartOperation_Proxy( 
    IAsyncOperation * This,
    /* [optional][unique][in] */ IBindCtx *pbcReserved);


void __RPC_STUB IAsyncOperation_StartOperation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAsyncOperation_InOperation_Proxy( 
    IAsyncOperation * This,
    /* [out] */ BOOL *pfInAsyncOp);


void __RPC_STUB IAsyncOperation_InOperation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAsyncOperation_EndOperation_Proxy( 
    IAsyncOperation * This,
    /* [in] */ HRESULT hResult,
    /* [unique][in] */ IBindCtx *pbcReserved,
    /* [in] */ DWORD dwEffects);


void __RPC_STUB IAsyncOperation_EndOperation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAsyncOperation_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



