

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* Compiler settings for exdisp.idl:
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

#ifndef __exdisp_h__
#define __exdisp_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IWebBrowser_FWD_DEFINED__
#define __IWebBrowser_FWD_DEFINED__
typedef interface IWebBrowser IWebBrowser;
#endif 	/* __IWebBrowser_FWD_DEFINED__ */


#ifndef __DWebBrowserEvents_FWD_DEFINED__
#define __DWebBrowserEvents_FWD_DEFINED__
typedef interface DWebBrowserEvents DWebBrowserEvents;
#endif 	/* __DWebBrowserEvents_FWD_DEFINED__ */


#ifndef __IWebBrowserApp_FWD_DEFINED__
#define __IWebBrowserApp_FWD_DEFINED__
typedef interface IWebBrowserApp IWebBrowserApp;
#endif 	/* __IWebBrowserApp_FWD_DEFINED__ */


#ifndef __IWebBrowser2_FWD_DEFINED__
#define __IWebBrowser2_FWD_DEFINED__
typedef interface IWebBrowser2 IWebBrowser2;
#endif 	/* __IWebBrowser2_FWD_DEFINED__ */


#ifndef __DWebBrowserEvents2_FWD_DEFINED__
#define __DWebBrowserEvents2_FWD_DEFINED__
typedef interface DWebBrowserEvents2 DWebBrowserEvents2;
#endif 	/* __DWebBrowserEvents2_FWD_DEFINED__ */


#ifndef __WebBrowser_V1_FWD_DEFINED__
#define __WebBrowser_V1_FWD_DEFINED__

#ifdef __cplusplus
typedef class WebBrowser_V1 WebBrowser_V1;
#else
typedef struct WebBrowser_V1 WebBrowser_V1;
#endif /* __cplusplus */

#endif 	/* __WebBrowser_V1_FWD_DEFINED__ */


#ifndef __WebBrowser_FWD_DEFINED__
#define __WebBrowser_FWD_DEFINED__

#ifdef __cplusplus
typedef class WebBrowser WebBrowser;
#else
typedef struct WebBrowser WebBrowser;
#endif /* __cplusplus */

#endif 	/* __WebBrowser_FWD_DEFINED__ */


#ifndef __InternetExplorer_FWD_DEFINED__
#define __InternetExplorer_FWD_DEFINED__

#ifdef __cplusplus
typedef class InternetExplorer InternetExplorer;
#else
typedef struct InternetExplorer InternetExplorer;
#endif /* __cplusplus */

#endif 	/* __InternetExplorer_FWD_DEFINED__ */


#ifndef __ShellBrowserWindow_FWD_DEFINED__
#define __ShellBrowserWindow_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShellBrowserWindow ShellBrowserWindow;
#else
typedef struct ShellBrowserWindow ShellBrowserWindow;
#endif /* __cplusplus */

#endif 	/* __ShellBrowserWindow_FWD_DEFINED__ */


#ifndef __DShellWindowsEvents_FWD_DEFINED__
#define __DShellWindowsEvents_FWD_DEFINED__
typedef interface DShellWindowsEvents DShellWindowsEvents;
#endif 	/* __DShellWindowsEvents_FWD_DEFINED__ */


#ifndef __IShellWindows_FWD_DEFINED__
#define __IShellWindows_FWD_DEFINED__
typedef interface IShellWindows IShellWindows;
#endif 	/* __IShellWindows_FWD_DEFINED__ */


#ifndef __ShellWindows_FWD_DEFINED__
#define __ShellWindows_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShellWindows ShellWindows;
#else
typedef struct ShellWindows ShellWindows;
#endif /* __cplusplus */

#endif 	/* __ShellWindows_FWD_DEFINED__ */


#ifndef __IShellUIHelper_FWD_DEFINED__
#define __IShellUIHelper_FWD_DEFINED__
typedef interface IShellUIHelper IShellUIHelper;
#endif 	/* __IShellUIHelper_FWD_DEFINED__ */


#ifndef __ShellUIHelper_FWD_DEFINED__
#define __ShellUIHelper_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShellUIHelper ShellUIHelper;
#else
typedef struct ShellUIHelper ShellUIHelper;
#endif /* __cplusplus */

#endif 	/* __ShellUIHelper_FWD_DEFINED__ */


#ifndef __DShellNameSpaceEvents_FWD_DEFINED__
#define __DShellNameSpaceEvents_FWD_DEFINED__
typedef interface DShellNameSpaceEvents DShellNameSpaceEvents;
#endif 	/* __DShellNameSpaceEvents_FWD_DEFINED__ */


#ifndef __IShellFavoritesNameSpace_FWD_DEFINED__
#define __IShellFavoritesNameSpace_FWD_DEFINED__
typedef interface IShellFavoritesNameSpace IShellFavoritesNameSpace;
#endif 	/* __IShellFavoritesNameSpace_FWD_DEFINED__ */


#ifndef __IShellNameSpace_FWD_DEFINED__
#define __IShellNameSpace_FWD_DEFINED__
typedef interface IShellNameSpace IShellNameSpace;
#endif 	/* __IShellNameSpace_FWD_DEFINED__ */


#ifndef __ShellNameSpace_FWD_DEFINED__
#define __ShellNameSpace_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShellNameSpace ShellNameSpace;
#else
typedef struct ShellNameSpace ShellNameSpace;
#endif /* __cplusplus */

#endif 	/* __ShellNameSpace_FWD_DEFINED__ */


#ifndef __IScriptErrorList_FWD_DEFINED__
#define __IScriptErrorList_FWD_DEFINED__
typedef interface IScriptErrorList IScriptErrorList;
#endif 	/* __IScriptErrorList_FWD_DEFINED__ */


#ifndef __CScriptErrorList_FWD_DEFINED__
#define __CScriptErrorList_FWD_DEFINED__

#ifdef __cplusplus
typedef class CScriptErrorList CScriptErrorList;
#else
typedef struct CScriptErrorList CScriptErrorList;
#endif /* __cplusplus */

#endif 	/* __CScriptErrorList_FWD_DEFINED__ */


#ifndef __ISearch_FWD_DEFINED__
#define __ISearch_FWD_DEFINED__
typedef interface ISearch ISearch;
#endif 	/* __ISearch_FWD_DEFINED__ */


#ifndef __ISearches_FWD_DEFINED__
#define __ISearches_FWD_DEFINED__
typedef interface ISearches ISearches;
#endif 	/* __ISearches_FWD_DEFINED__ */


#ifndef __ISearchAssistantOC_FWD_DEFINED__
#define __ISearchAssistantOC_FWD_DEFINED__
typedef interface ISearchAssistantOC ISearchAssistantOC;
#endif 	/* __ISearchAssistantOC_FWD_DEFINED__ */


#ifndef __ISearchAssistantOC2_FWD_DEFINED__
#define __ISearchAssistantOC2_FWD_DEFINED__
typedef interface ISearchAssistantOC2 ISearchAssistantOC2;
#endif 	/* __ISearchAssistantOC2_FWD_DEFINED__ */


#ifndef __ISearchAssistantOC3_FWD_DEFINED__
#define __ISearchAssistantOC3_FWD_DEFINED__
typedef interface ISearchAssistantOC3 ISearchAssistantOC3;
#endif 	/* __ISearchAssistantOC3_FWD_DEFINED__ */


#ifndef ___SearchAssistantEvents_FWD_DEFINED__
#define ___SearchAssistantEvents_FWD_DEFINED__
typedef interface _SearchAssistantEvents _SearchAssistantEvents;
#endif 	/* ___SearchAssistantEvents_FWD_DEFINED__ */


#ifndef __SearchAssistantOC_FWD_DEFINED__
#define __SearchAssistantOC_FWD_DEFINED__

#ifdef __cplusplus
typedef class SearchAssistantOC SearchAssistantOC;
#else
typedef struct SearchAssistantOC SearchAssistantOC;
#endif /* __cplusplus */

#endif 	/* __SearchAssistantOC_FWD_DEFINED__ */


/* header files for imported files */
#include "ocidl.h"
#include "docobj.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 


#ifndef __SHDocVw_LIBRARY_DEFINED__
#define __SHDocVw_LIBRARY_DEFINED__

/* library SHDocVw */
/* [version][lcid][helpstring][uuid] */ 

typedef /* [helpstring][uuid] */  DECLSPEC_UUID("34A226E0-DF30-11CF-89A9-00A0C9054129") 
enum CommandStateChangeConstants
    {	CSC_UPDATECOMMANDS	= 0xffffffff,
	CSC_NAVIGATEFORWARD	= 0x1,
	CSC_NAVIGATEBACK	= 0x2
    } 	CommandStateChangeConstants;

typedef /* [helpstring][uuid] */  DECLSPEC_UUID("65507BE0-91A8-11d3-A845-009027220E6D") 
enum SecureLockIconConstants
    {	secureLockIconUnsecure	= 0,
	secureLockIconMixed	= 0x1,
	secureLockIconSecureUnknownBits	= 0x2,
	secureLockIconSecure40Bit	= 0x3,
	secureLockIconSecure56Bit	= 0x4,
	secureLockIconSecureFortezza	= 0x5,
	secureLockIconSecure128Bit	= 0x6
    } 	SecureLockIconConstants;

typedef /* [helpstring][uuid] */  DECLSPEC_UUID("F41E6981-28E5-11d0-82B4-00A0C90C29C5") 
enum ShellWindowTypeConstants
    {	SWC_EXPLORER	= 0,
	SWC_BROWSER	= 0x1,
	SWC_3RDPARTY	= 0x2,
	SWC_CALLBACK	= 0x4
    } 	ShellWindowTypeConstants;

typedef /* [hidden][helpstring][uuid] */  DECLSPEC_UUID("7716a370-38ca-11d0-a48b-00a0c90a8f39") 
enum ShellWindowFindWindowOptions
    {	SWFO_NEEDDISPATCH	= 0x1,
	SWFO_INCLUDEPENDING	= 0x2,
	SWFO_COOKIEPASSED	= 0x4
    } 	ShellWindowFindWindowOptions;


EXTERN_C const IID LIBID_SHDocVw;

#ifndef __IWebBrowser_INTERFACE_DEFINED__
#define __IWebBrowser_INTERFACE_DEFINED__

/* interface IWebBrowser */
/* [object][oleautomation][dual][hidden][helpcontext][helpstring][uuid] */ 

typedef /* [helpstring][uuid] */  DECLSPEC_UUID("14EE5380-A378-11cf-A731-00A0C9082637") 
enum BrowserNavConstants
    {	navOpenInNewWindow	= 0x1,
	navNoHistory	= 0x2,
	navNoReadFromCache	= 0x4,
	navNoWriteToCache	= 0x8,
	navAllowAutosearch	= 0x10,
	navBrowserBar	= 0x20,
	navHyperlink	= 0x40,
	navEnforceRestricted	= 0x80,
	navNewWindowsManaged	= 0x100,
	navUntrustedForDownload	= 0x200,
	navTrustedForActiveX	= 0x400
    } 	BrowserNavConstants;

typedef /* [helpstring][uuid] */  DECLSPEC_UUID("C317C261-A991-11cf-A731-00A0C9082637") 
enum RefreshConstants
    {	REFRESH_NORMAL	= 0,
	REFRESH_IFEXPIRED	= 1,
	REFRESH_COMPLETELY	= 3
    } 	RefreshConstants;


EXTERN_C const IID IID_IWebBrowser;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EAB22AC1-30C1-11CF-A7EB-0000C05BAE0B")
    IWebBrowser : public IDispatch
    {
    public:
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE GoBack( void) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE GoForward( void) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE GoHome( void) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE GoSearch( void) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Navigate( 
            /* [in] */ BSTR URL,
            /* [optional][in] */ VARIANT *Flags,
            /* [optional][in] */ VARIANT *TargetFrameName,
            /* [optional][in] */ VARIANT *PostData,
            /* [optional][in] */ VARIANT *Headers) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh2( 
            /* [optional][in] */ VARIANT *Level) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IDispatch **ppDisp) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Parent( 
            /* [retval][out] */ IDispatch **ppDisp) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Container( 
            /* [retval][out] */ IDispatch **ppDisp) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Document( 
            /* [retval][out] */ IDispatch **ppDisp) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_TopLevelContainer( 
            /* [retval][out] */ VARIANT_BOOL *pBool) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ BSTR *Type) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Left( 
            /* [retval][out] */ long *pl) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Left( 
            /* [in] */ long Left) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Top( 
            /* [retval][out] */ long *pl) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Top( 
            /* [in] */ long Top) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Width( 
            /* [retval][out] */ long *pl) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Width( 
            /* [in] */ long Width) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Height( 
            /* [retval][out] */ long *pl) = 0;
        
        virtual /* [propput][id] */ HRESULT STDMETHODCALLTYPE put_Height( 
            /* [in] */ long Height) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_LocationName( 
            /* [retval][out] */ BSTR *LocationName) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_LocationURL( 
            /* [retval][out] */ BSTR *LocationURL) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Busy( 
            /* [retval][out] */ VARIANT_BOOL *pBool) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWebBrowserVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebBrowser * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebBrowser * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebBrowser * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IWebBrowser * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IWebBrowser * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IWebBrowser * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebBrowser * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GoBack )( 
            IWebBrowser * This);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GoForward )( 
            IWebBrowser * This);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GoHome )( 
            IWebBrowser * This);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GoSearch )( 
            IWebBrowser * This);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Navigate )( 
            IWebBrowser * This,
            /* [in] */ BSTR URL,
            /* [optional][in] */ VARIANT *Flags,
            /* [optional][in] */ VARIANT *TargetFrameName,
            /* [optional][in] */ VARIANT *PostData,
            /* [optional][in] */ VARIANT *Headers);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Refresh )( 
            IWebBrowser * This);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Refresh2 )( 
            IWebBrowser * This,
            /* [optional][in] */ VARIANT *Level);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IWebBrowser * This);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            IWebBrowser * This,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Parent )( 
            IWebBrowser * This,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Container )( 
            IWebBrowser * This,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Document )( 
            IWebBrowser * This,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_TopLevelContainer )( 
            IWebBrowser * This,
            /* [retval][out] */ VARIANT_BOOL *pBool);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Type )( 
            IWebBrowser * This,
            /* [retval][out] */ BSTR *Type);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Left )( 
            IWebBrowser * This,
            /* [retval][out] */ long *pl);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Left )( 
            IWebBrowser * This,
            /* [in] */ long Left);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Top )( 
            IWebBrowser * This,
            /* [retval][out] */ long *pl);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Top )( 
            IWebBrowser * This,
            /* [in] */ long Top);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Width )( 
            IWebBrowser * This,
            /* [retval][out] */ long *pl);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Width )( 
            IWebBrowser * This,
            /* [in] */ long Width);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Height )( 
            IWebBrowser * This,
            /* [retval][out] */ long *pl);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Height )( 
            IWebBrowser * This,
            /* [in] */ long Height);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_LocationName )( 
            IWebBrowser * This,
            /* [retval][out] */ BSTR *LocationName);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_LocationURL )( 
            IWebBrowser * This,
            /* [retval][out] */ BSTR *LocationURL);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Busy )( 
            IWebBrowser * This,
            /* [retval][out] */ VARIANT_BOOL *pBool);
        
        END_INTERFACE
    } IWebBrowserVtbl;

    interface IWebBrowser
    {
        CONST_VTBL struct IWebBrowserVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebBrowser_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWebBrowser_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWebBrowser_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWebBrowser_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWebBrowser_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWebBrowser_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWebBrowser_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWebBrowser_GoBack(This)	\
    (This)->lpVtbl -> GoBack(This)

#define IWebBrowser_GoForward(This)	\
    (This)->lpVtbl -> GoForward(This)

#define IWebBrowser_GoHome(This)	\
    (This)->lpVtbl -> GoHome(This)

#define IWebBrowser_GoSearch(This)	\
    (This)->lpVtbl -> GoSearch(This)

#define IWebBrowser_Navigate(This,URL,Flags,TargetFrameName,PostData,Headers)	\
    (This)->lpVtbl -> Navigate(This,URL,Flags,TargetFrameName,PostData,Headers)

#define IWebBrowser_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IWebBrowser_Refresh2(This,Level)	\
    (This)->lpVtbl -> Refresh2(This,Level)

#define IWebBrowser_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IWebBrowser_get_Application(This,ppDisp)	\
    (This)->lpVtbl -> get_Application(This,ppDisp)

#define IWebBrowser_get_Parent(This,ppDisp)	\
    (This)->lpVtbl -> get_Parent(This,ppDisp)

#define IWebBrowser_get_Container(This,ppDisp)	\
    (This)->lpVtbl -> get_Container(This,ppDisp)

#define IWebBrowser_get_Document(This,ppDisp)	\
    (This)->lpVtbl -> get_Document(This,ppDisp)

#define IWebBrowser_get_TopLevelContainer(This,pBool)	\
    (This)->lpVtbl -> get_TopLevelContainer(This,pBool)

#define IWebBrowser_get_Type(This,Type)	\
    (This)->lpVtbl -> get_Type(This,Type)

#define IWebBrowser_get_Left(This,pl)	\
    (This)->lpVtbl -> get_Left(This,pl)

#define IWebBrowser_put_Left(This,Left)	\
    (This)->lpVtbl -> put_Left(This,Left)

#define IWebBrowser_get_Top(This,pl)	\
    (This)->lpVtbl -> get_Top(This,pl)

#define IWebBrowser_put_Top(This,Top)	\
    (This)->lpVtbl -> put_Top(This,Top)

#define IWebBrowser_get_Width(This,pl)	\
    (This)->lpVtbl -> get_Width(This,pl)

#define IWebBrowser_put_Width(This,Width)	\
    (This)->lpVtbl -> put_Width(This,Width)

#define IWebBrowser_get_Height(This,pl)	\
    (This)->lpVtbl -> get_Height(This,pl)

#define IWebBrowser_put_Height(This,Height)	\
    (This)->lpVtbl -> put_Height(This,Height)

#define IWebBrowser_get_LocationName(This,LocationName)	\
    (This)->lpVtbl -> get_LocationName(This,LocationName)

#define IWebBrowser_get_LocationURL(This,LocationURL)	\
    (This)->lpVtbl -> get_LocationURL(This,LocationURL)

#define IWebBrowser_get_Busy(This,pBool)	\
    (This)->lpVtbl -> get_Busy(This,pBool)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_GoBack_Proxy( 
    IWebBrowser * This);


void __RPC_STUB IWebBrowser_GoBack_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_GoForward_Proxy( 
    IWebBrowser * This);


void __RPC_STUB IWebBrowser_GoForward_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_GoHome_Proxy( 
    IWebBrowser * This);


void __RPC_STUB IWebBrowser_GoHome_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_GoSearch_Proxy( 
    IWebBrowser * This);


void __RPC_STUB IWebBrowser_GoSearch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_Navigate_Proxy( 
    IWebBrowser * This,
    /* [in] */ BSTR URL,
    /* [optional][in] */ VARIANT *Flags,
    /* [optional][in] */ VARIANT *TargetFrameName,
    /* [optional][in] */ VARIANT *PostData,
    /* [optional][in] */ VARIANT *Headers);


void __RPC_STUB IWebBrowser_Navigate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_Refresh_Proxy( 
    IWebBrowser * This);


void __RPC_STUB IWebBrowser_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_Refresh2_Proxy( 
    IWebBrowser * This,
    /* [optional][in] */ VARIANT *Level);


void __RPC_STUB IWebBrowser_Refresh2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_Stop_Proxy( 
    IWebBrowser * This);


void __RPC_STUB IWebBrowser_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_get_Application_Proxy( 
    IWebBrowser * This,
    /* [retval][out] */ IDispatch **ppDisp);


void __RPC_STUB IWebBrowser_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_get_Parent_Proxy( 
    IWebBrowser * This,
    /* [retval][out] */ IDispatch **ppDisp);


void __RPC_STUB IWebBrowser_get_Parent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_get_Container_Proxy( 
    IWebBrowser * This,
    /* [retval][out] */ IDispatch **ppDisp);


void __RPC_STUB IWebBrowser_get_Container_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_get_Document_Proxy( 
    IWebBrowser * This,
    /* [retval][out] */ IDispatch **ppDisp);


void __RPC_STUB IWebBrowser_get_Document_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_get_TopLevelContainer_Proxy( 
    IWebBrowser * This,
    /* [retval][out] */ VARIANT_BOOL *pBool);


void __RPC_STUB IWebBrowser_get_TopLevelContainer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_get_Type_Proxy( 
    IWebBrowser * This,
    /* [retval][out] */ BSTR *Type);


void __RPC_STUB IWebBrowser_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_get_Left_Proxy( 
    IWebBrowser * This,
    /* [retval][out] */ long *pl);


void __RPC_STUB IWebBrowser_get_Left_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_put_Left_Proxy( 
    IWebBrowser * This,
    /* [in] */ long Left);


void __RPC_STUB IWebBrowser_put_Left_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_get_Top_Proxy( 
    IWebBrowser * This,
    /* [retval][out] */ long *pl);


void __RPC_STUB IWebBrowser_get_Top_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_put_Top_Proxy( 
    IWebBrowser * This,
    /* [in] */ long Top);


void __RPC_STUB IWebBrowser_put_Top_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_get_Width_Proxy( 
    IWebBrowser * This,
    /* [retval][out] */ long *pl);


void __RPC_STUB IWebBrowser_get_Width_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_put_Width_Proxy( 
    IWebBrowser * This,
    /* [in] */ long Width);


void __RPC_STUB IWebBrowser_put_Width_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_get_Height_Proxy( 
    IWebBrowser * This,
    /* [retval][out] */ long *pl);


void __RPC_STUB IWebBrowser_get_Height_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_put_Height_Proxy( 
    IWebBrowser * This,
    /* [in] */ long Height);


void __RPC_STUB IWebBrowser_put_Height_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_get_LocationName_Proxy( 
    IWebBrowser * This,
    /* [retval][out] */ BSTR *LocationName);


void __RPC_STUB IWebBrowser_get_LocationName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_get_LocationURL_Proxy( 
    IWebBrowser * This,
    /* [retval][out] */ BSTR *LocationURL);


void __RPC_STUB IWebBrowser_get_LocationURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser_get_Busy_Proxy( 
    IWebBrowser * This,
    /* [retval][out] */ VARIANT_BOOL *pBool);


void __RPC_STUB IWebBrowser_get_Busy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWebBrowser_INTERFACE_DEFINED__ */


#ifndef __DWebBrowserEvents_DISPINTERFACE_DEFINED__
#define __DWebBrowserEvents_DISPINTERFACE_DEFINED__

/* dispinterface DWebBrowserEvents */
/* [hidden][helpstring][uuid] */ 


EXTERN_C const IID DIID_DWebBrowserEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("EAB22AC2-30C1-11CF-A7EB-0000C05BAE0B")
    DWebBrowserEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct DWebBrowserEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            DWebBrowserEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            DWebBrowserEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            DWebBrowserEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            DWebBrowserEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            DWebBrowserEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            DWebBrowserEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            DWebBrowserEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } DWebBrowserEventsVtbl;

    interface DWebBrowserEvents
    {
        CONST_VTBL struct DWebBrowserEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define DWebBrowserEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define DWebBrowserEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define DWebBrowserEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define DWebBrowserEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define DWebBrowserEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define DWebBrowserEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define DWebBrowserEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __DWebBrowserEvents_DISPINTERFACE_DEFINED__ */


#ifndef __IWebBrowserApp_INTERFACE_DEFINED__
#define __IWebBrowserApp_INTERFACE_DEFINED__

/* interface IWebBrowserApp */
/* [object][dual][oleautomation][hidden][helpcontext][helpstring][uuid] */ 


EXTERN_C const IID IID_IWebBrowserApp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0002DF05-0000-0000-C000-000000000046")
    IWebBrowserApp : public IWebBrowser
    {
    public:
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Quit( void) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE ClientToWindow( 
            /* [out][in] */ int *pcx,
            /* [out][in] */ int *pcy) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE PutProperty( 
            /* [in] */ BSTR Property,
            /* [in] */ VARIANT vtValue) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE GetProperty( 
            /* [in] */ BSTR Property,
            /* [retval][out] */ VARIANT *pvtValue) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR *Name) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_HWND( 
            /* [retval][out] */ SHANDLE_PTR *pHWND) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_FullName( 
            /* [retval][out] */ BSTR *FullName) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Path( 
            /* [retval][out] */ BSTR *Path) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Visible( 
            /* [retval][out] */ VARIANT_BOOL *pBool) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Visible( 
            /* [in] */ VARIANT_BOOL Value) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_StatusBar( 
            /* [retval][out] */ VARIANT_BOOL *pBool) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_StatusBar( 
            /* [in] */ VARIANT_BOOL Value) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_StatusText( 
            /* [retval][out] */ BSTR *StatusText) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_StatusText( 
            /* [in] */ BSTR StatusText) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_ToolBar( 
            /* [retval][out] */ int *Value) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_ToolBar( 
            /* [in] */ int Value) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_MenuBar( 
            /* [retval][out] */ VARIANT_BOOL *Value) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_MenuBar( 
            /* [in] */ VARIANT_BOOL Value) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_FullScreen( 
            /* [retval][out] */ VARIANT_BOOL *pbFullScreen) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_FullScreen( 
            /* [in] */ VARIANT_BOOL bFullScreen) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWebBrowserAppVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebBrowserApp * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebBrowserApp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebBrowserApp * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IWebBrowserApp * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IWebBrowserApp * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IWebBrowserApp * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebBrowserApp * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GoBack )( 
            IWebBrowserApp * This);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GoForward )( 
            IWebBrowserApp * This);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GoHome )( 
            IWebBrowserApp * This);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GoSearch )( 
            IWebBrowserApp * This);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Navigate )( 
            IWebBrowserApp * This,
            /* [in] */ BSTR URL,
            /* [optional][in] */ VARIANT *Flags,
            /* [optional][in] */ VARIANT *TargetFrameName,
            /* [optional][in] */ VARIANT *PostData,
            /* [optional][in] */ VARIANT *Headers);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Refresh )( 
            IWebBrowserApp * This);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Refresh2 )( 
            IWebBrowserApp * This,
            /* [optional][in] */ VARIANT *Level);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IWebBrowserApp * This);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            IWebBrowserApp * This,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Parent )( 
            IWebBrowserApp * This,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Container )( 
            IWebBrowserApp * This,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Document )( 
            IWebBrowserApp * This,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_TopLevelContainer )( 
            IWebBrowserApp * This,
            /* [retval][out] */ VARIANT_BOOL *pBool);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Type )( 
            IWebBrowserApp * This,
            /* [retval][out] */ BSTR *Type);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Left )( 
            IWebBrowserApp * This,
            /* [retval][out] */ long *pl);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Left )( 
            IWebBrowserApp * This,
            /* [in] */ long Left);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Top )( 
            IWebBrowserApp * This,
            /* [retval][out] */ long *pl);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Top )( 
            IWebBrowserApp * This,
            /* [in] */ long Top);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Width )( 
            IWebBrowserApp * This,
            /* [retval][out] */ long *pl);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Width )( 
            IWebBrowserApp * This,
            /* [in] */ long Width);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Height )( 
            IWebBrowserApp * This,
            /* [retval][out] */ long *pl);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Height )( 
            IWebBrowserApp * This,
            /* [in] */ long Height);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_LocationName )( 
            IWebBrowserApp * This,
            /* [retval][out] */ BSTR *LocationName);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_LocationURL )( 
            IWebBrowserApp * This,
            /* [retval][out] */ BSTR *LocationURL);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Busy )( 
            IWebBrowserApp * This,
            /* [retval][out] */ VARIANT_BOOL *pBool);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Quit )( 
            IWebBrowserApp * This);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ClientToWindow )( 
            IWebBrowserApp * This,
            /* [out][in] */ int *pcx,
            /* [out][in] */ int *pcy);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PutProperty )( 
            IWebBrowserApp * This,
            /* [in] */ BSTR Property,
            /* [in] */ VARIANT vtValue);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProperty )( 
            IWebBrowserApp * This,
            /* [in] */ BSTR Property,
            /* [retval][out] */ VARIANT *pvtValue);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IWebBrowserApp * This,
            /* [retval][out] */ BSTR *Name);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_HWND )( 
            IWebBrowserApp * This,
            /* [retval][out] */ SHANDLE_PTR *pHWND);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_FullName )( 
            IWebBrowserApp * This,
            /* [retval][out] */ BSTR *FullName);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Path )( 
            IWebBrowserApp * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Visible )( 
            IWebBrowserApp * This,
            /* [retval][out] */ VARIANT_BOOL *pBool);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Visible )( 
            IWebBrowserApp * This,
            /* [in] */ VARIANT_BOOL Value);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_StatusBar )( 
            IWebBrowserApp * This,
            /* [retval][out] */ VARIANT_BOOL *pBool);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_StatusBar )( 
            IWebBrowserApp * This,
            /* [in] */ VARIANT_BOOL Value);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_StatusText )( 
            IWebBrowserApp * This,
            /* [retval][out] */ BSTR *StatusText);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_StatusText )( 
            IWebBrowserApp * This,
            /* [in] */ BSTR StatusText);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ToolBar )( 
            IWebBrowserApp * This,
            /* [retval][out] */ int *Value);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ToolBar )( 
            IWebBrowserApp * This,
            /* [in] */ int Value);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_MenuBar )( 
            IWebBrowserApp * This,
            /* [retval][out] */ VARIANT_BOOL *Value);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_MenuBar )( 
            IWebBrowserApp * This,
            /* [in] */ VARIANT_BOOL Value);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_FullScreen )( 
            IWebBrowserApp * This,
            /* [retval][out] */ VARIANT_BOOL *pbFullScreen);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_FullScreen )( 
            IWebBrowserApp * This,
            /* [in] */ VARIANT_BOOL bFullScreen);
        
        END_INTERFACE
    } IWebBrowserAppVtbl;

    interface IWebBrowserApp
    {
        CONST_VTBL struct IWebBrowserAppVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebBrowserApp_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWebBrowserApp_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWebBrowserApp_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWebBrowserApp_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWebBrowserApp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWebBrowserApp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWebBrowserApp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWebBrowserApp_GoBack(This)	\
    (This)->lpVtbl -> GoBack(This)

#define IWebBrowserApp_GoForward(This)	\
    (This)->lpVtbl -> GoForward(This)

#define IWebBrowserApp_GoHome(This)	\
    (This)->lpVtbl -> GoHome(This)

#define IWebBrowserApp_GoSearch(This)	\
    (This)->lpVtbl -> GoSearch(This)

#define IWebBrowserApp_Navigate(This,URL,Flags,TargetFrameName,PostData,Headers)	\
    (This)->lpVtbl -> Navigate(This,URL,Flags,TargetFrameName,PostData,Headers)

#define IWebBrowserApp_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IWebBrowserApp_Refresh2(This,Level)	\
    (This)->lpVtbl -> Refresh2(This,Level)

#define IWebBrowserApp_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IWebBrowserApp_get_Application(This,ppDisp)	\
    (This)->lpVtbl -> get_Application(This,ppDisp)

#define IWebBrowserApp_get_Parent(This,ppDisp)	\
    (This)->lpVtbl -> get_Parent(This,ppDisp)

#define IWebBrowserApp_get_Container(This,ppDisp)	\
    (This)->lpVtbl -> get_Container(This,ppDisp)

#define IWebBrowserApp_get_Document(This,ppDisp)	\
    (This)->lpVtbl -> get_Document(This,ppDisp)

#define IWebBrowserApp_get_TopLevelContainer(This,pBool)	\
    (This)->lpVtbl -> get_TopLevelContainer(This,pBool)

#define IWebBrowserApp_get_Type(This,Type)	\
    (This)->lpVtbl -> get_Type(This,Type)

#define IWebBrowserApp_get_Left(This,pl)	\
    (This)->lpVtbl -> get_Left(This,pl)

#define IWebBrowserApp_put_Left(This,Left)	\
    (This)->lpVtbl -> put_Left(This,Left)

#define IWebBrowserApp_get_Top(This,pl)	\
    (This)->lpVtbl -> get_Top(This,pl)

#define IWebBrowserApp_put_Top(This,Top)	\
    (This)->lpVtbl -> put_Top(This,Top)

#define IWebBrowserApp_get_Width(This,pl)	\
    (This)->lpVtbl -> get_Width(This,pl)

#define IWebBrowserApp_put_Width(This,Width)	\
    (This)->lpVtbl -> put_Width(This,Width)

#define IWebBrowserApp_get_Height(This,pl)	\
    (This)->lpVtbl -> get_Height(This,pl)

#define IWebBrowserApp_put_Height(This,Height)	\
    (This)->lpVtbl -> put_Height(This,Height)

#define IWebBrowserApp_get_LocationName(This,LocationName)	\
    (This)->lpVtbl -> get_LocationName(This,LocationName)

#define IWebBrowserApp_get_LocationURL(This,LocationURL)	\
    (This)->lpVtbl -> get_LocationURL(This,LocationURL)

#define IWebBrowserApp_get_Busy(This,pBool)	\
    (This)->lpVtbl -> get_Busy(This,pBool)


#define IWebBrowserApp_Quit(This)	\
    (This)->lpVtbl -> Quit(This)

#define IWebBrowserApp_ClientToWindow(This,pcx,pcy)	\
    (This)->lpVtbl -> ClientToWindow(This,pcx,pcy)

#define IWebBrowserApp_PutProperty(This,Property,vtValue)	\
    (This)->lpVtbl -> PutProperty(This,Property,vtValue)

#define IWebBrowserApp_GetProperty(This,Property,pvtValue)	\
    (This)->lpVtbl -> GetProperty(This,Property,pvtValue)

#define IWebBrowserApp_get_Name(This,Name)	\
    (This)->lpVtbl -> get_Name(This,Name)

#define IWebBrowserApp_get_HWND(This,pHWND)	\
    (This)->lpVtbl -> get_HWND(This,pHWND)

#define IWebBrowserApp_get_FullName(This,FullName)	\
    (This)->lpVtbl -> get_FullName(This,FullName)

#define IWebBrowserApp_get_Path(This,Path)	\
    (This)->lpVtbl -> get_Path(This,Path)

#define IWebBrowserApp_get_Visible(This,pBool)	\
    (This)->lpVtbl -> get_Visible(This,pBool)

#define IWebBrowserApp_put_Visible(This,Value)	\
    (This)->lpVtbl -> put_Visible(This,Value)

#define IWebBrowserApp_get_StatusBar(This,pBool)	\
    (This)->lpVtbl -> get_StatusBar(This,pBool)

#define IWebBrowserApp_put_StatusBar(This,Value)	\
    (This)->lpVtbl -> put_StatusBar(This,Value)

#define IWebBrowserApp_get_StatusText(This,StatusText)	\
    (This)->lpVtbl -> get_StatusText(This,StatusText)

#define IWebBrowserApp_put_StatusText(This,StatusText)	\
    (This)->lpVtbl -> put_StatusText(This,StatusText)

#define IWebBrowserApp_get_ToolBar(This,Value)	\
    (This)->lpVtbl -> get_ToolBar(This,Value)

#define IWebBrowserApp_put_ToolBar(This,Value)	\
    (This)->lpVtbl -> put_ToolBar(This,Value)

#define IWebBrowserApp_get_MenuBar(This,Value)	\
    (This)->lpVtbl -> get_MenuBar(This,Value)

#define IWebBrowserApp_put_MenuBar(This,Value)	\
    (This)->lpVtbl -> put_MenuBar(This,Value)

#define IWebBrowserApp_get_FullScreen(This,pbFullScreen)	\
    (This)->lpVtbl -> get_FullScreen(This,pbFullScreen)

#define IWebBrowserApp_put_FullScreen(This,bFullScreen)	\
    (This)->lpVtbl -> put_FullScreen(This,bFullScreen)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_Quit_Proxy( 
    IWebBrowserApp * This);


void __RPC_STUB IWebBrowserApp_Quit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_ClientToWindow_Proxy( 
    IWebBrowserApp * This,
    /* [out][in] */ int *pcx,
    /* [out][in] */ int *pcy);


void __RPC_STUB IWebBrowserApp_ClientToWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_PutProperty_Proxy( 
    IWebBrowserApp * This,
    /* [in] */ BSTR Property,
    /* [in] */ VARIANT vtValue);


void __RPC_STUB IWebBrowserApp_PutProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_GetProperty_Proxy( 
    IWebBrowserApp * This,
    /* [in] */ BSTR Property,
    /* [retval][out] */ VARIANT *pvtValue);


void __RPC_STUB IWebBrowserApp_GetProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_get_Name_Proxy( 
    IWebBrowserApp * This,
    /* [retval][out] */ BSTR *Name);


void __RPC_STUB IWebBrowserApp_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_get_HWND_Proxy( 
    IWebBrowserApp * This,
    /* [retval][out] */ SHANDLE_PTR *pHWND);


void __RPC_STUB IWebBrowserApp_get_HWND_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_get_FullName_Proxy( 
    IWebBrowserApp * This,
    /* [retval][out] */ BSTR *FullName);


void __RPC_STUB IWebBrowserApp_get_FullName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_get_Path_Proxy( 
    IWebBrowserApp * This,
    /* [retval][out] */ BSTR *Path);


void __RPC_STUB IWebBrowserApp_get_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_get_Visible_Proxy( 
    IWebBrowserApp * This,
    /* [retval][out] */ VARIANT_BOOL *pBool);


void __RPC_STUB IWebBrowserApp_get_Visible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_put_Visible_Proxy( 
    IWebBrowserApp * This,
    /* [in] */ VARIANT_BOOL Value);


void __RPC_STUB IWebBrowserApp_put_Visible_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_get_StatusBar_Proxy( 
    IWebBrowserApp * This,
    /* [retval][out] */ VARIANT_BOOL *pBool);


void __RPC_STUB IWebBrowserApp_get_StatusBar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_put_StatusBar_Proxy( 
    IWebBrowserApp * This,
    /* [in] */ VARIANT_BOOL Value);


void __RPC_STUB IWebBrowserApp_put_StatusBar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_get_StatusText_Proxy( 
    IWebBrowserApp * This,
    /* [retval][out] */ BSTR *StatusText);


void __RPC_STUB IWebBrowserApp_get_StatusText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_put_StatusText_Proxy( 
    IWebBrowserApp * This,
    /* [in] */ BSTR StatusText);


void __RPC_STUB IWebBrowserApp_put_StatusText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_get_ToolBar_Proxy( 
    IWebBrowserApp * This,
    /* [retval][out] */ int *Value);


void __RPC_STUB IWebBrowserApp_get_ToolBar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_put_ToolBar_Proxy( 
    IWebBrowserApp * This,
    /* [in] */ int Value);


void __RPC_STUB IWebBrowserApp_put_ToolBar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_get_MenuBar_Proxy( 
    IWebBrowserApp * This,
    /* [retval][out] */ VARIANT_BOOL *Value);


void __RPC_STUB IWebBrowserApp_get_MenuBar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_put_MenuBar_Proxy( 
    IWebBrowserApp * This,
    /* [in] */ VARIANT_BOOL Value);


void __RPC_STUB IWebBrowserApp_put_MenuBar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_get_FullScreen_Proxy( 
    IWebBrowserApp * This,
    /* [retval][out] */ VARIANT_BOOL *pbFullScreen);


void __RPC_STUB IWebBrowserApp_get_FullScreen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IWebBrowserApp_put_FullScreen_Proxy( 
    IWebBrowserApp * This,
    /* [in] */ VARIANT_BOOL bFullScreen);


void __RPC_STUB IWebBrowserApp_put_FullScreen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWebBrowserApp_INTERFACE_DEFINED__ */


#ifndef __IWebBrowser2_INTERFACE_DEFINED__
#define __IWebBrowser2_INTERFACE_DEFINED__

/* interface IWebBrowser2 */
/* [object][dual][oleautomation][hidden][helpcontext][helpstring][uuid] */ 


EXTERN_C const IID IID_IWebBrowser2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D30C1661-CDAF-11d0-8A3E-00C04FC9E26E")
    IWebBrowser2 : public IWebBrowserApp
    {
    public:
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE Navigate2( 
            /* [in] */ VARIANT *URL,
            /* [optional][in] */ VARIANT *Flags,
            /* [optional][in] */ VARIANT *TargetFrameName,
            /* [optional][in] */ VARIANT *PostData,
            /* [optional][in] */ VARIANT *Headers) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE QueryStatusWB( 
            /* [in] */ OLECMDID cmdID,
            /* [retval][out] */ OLECMDF *pcmdf) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE ExecWB( 
            /* [in] */ OLECMDID cmdID,
            /* [in] */ OLECMDEXECOPT cmdexecopt,
            /* [optional][in] */ VARIANT *pvaIn,
            /* [optional][in][out] */ VARIANT *pvaOut) = 0;
        
        virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE ShowBrowserBar( 
            /* [in] */ VARIANT *pvaClsid,
            /* [optional][in] */ VARIANT *pvarShow,
            /* [optional][in] */ VARIANT *pvarSize) = 0;
        
        virtual /* [bindable][propget][id] */ HRESULT STDMETHODCALLTYPE get_ReadyState( 
            /* [out][retval] */ READYSTATE *plReadyState) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Offline( 
            /* [retval][out] */ VARIANT_BOOL *pbOffline) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Offline( 
            /* [in] */ VARIANT_BOOL bOffline) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Silent( 
            /* [retval][out] */ VARIANT_BOOL *pbSilent) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Silent( 
            /* [in] */ VARIANT_BOOL bSilent) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_RegisterAsBrowser( 
            /* [retval][out] */ VARIANT_BOOL *pbRegister) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_RegisterAsBrowser( 
            /* [in] */ VARIANT_BOOL bRegister) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_RegisterAsDropTarget( 
            /* [retval][out] */ VARIANT_BOOL *pbRegister) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_RegisterAsDropTarget( 
            /* [in] */ VARIANT_BOOL bRegister) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_TheaterMode( 
            /* [retval][out] */ VARIANT_BOOL *pbRegister) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_TheaterMode( 
            /* [in] */ VARIANT_BOOL bRegister) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_AddressBar( 
            /* [retval][out] */ VARIANT_BOOL *Value) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_AddressBar( 
            /* [in] */ VARIANT_BOOL Value) = 0;
        
        virtual /* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE get_Resizable( 
            /* [retval][out] */ VARIANT_BOOL *Value) = 0;
        
        virtual /* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE put_Resizable( 
            /* [in] */ VARIANT_BOOL Value) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWebBrowser2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebBrowser2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebBrowser2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebBrowser2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IWebBrowser2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IWebBrowser2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IWebBrowser2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IWebBrowser2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GoBack )( 
            IWebBrowser2 * This);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GoForward )( 
            IWebBrowser2 * This);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GoHome )( 
            IWebBrowser2 * This);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GoSearch )( 
            IWebBrowser2 * This);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Navigate )( 
            IWebBrowser2 * This,
            /* [in] */ BSTR URL,
            /* [optional][in] */ VARIANT *Flags,
            /* [optional][in] */ VARIANT *TargetFrameName,
            /* [optional][in] */ VARIANT *PostData,
            /* [optional][in] */ VARIANT *Headers);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Refresh )( 
            IWebBrowser2 * This);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Refresh2 )( 
            IWebBrowser2 * This,
            /* [optional][in] */ VARIANT *Level);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IWebBrowser2 * This);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Application )( 
            IWebBrowser2 * This,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Parent )( 
            IWebBrowser2 * This,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Container )( 
            IWebBrowser2 * This,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Document )( 
            IWebBrowser2 * This,
            /* [retval][out] */ IDispatch **ppDisp);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_TopLevelContainer )( 
            IWebBrowser2 * This,
            /* [retval][out] */ VARIANT_BOOL *pBool);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Type )( 
            IWebBrowser2 * This,
            /* [retval][out] */ BSTR *Type);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Left )( 
            IWebBrowser2 * This,
            /* [retval][out] */ long *pl);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Left )( 
            IWebBrowser2 * This,
            /* [in] */ long Left);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Top )( 
            IWebBrowser2 * This,
            /* [retval][out] */ long *pl);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Top )( 
            IWebBrowser2 * This,
            /* [in] */ long Top);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Width )( 
            IWebBrowser2 * This,
            /* [retval][out] */ long *pl);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Width )( 
            IWebBrowser2 * This,
            /* [in] */ long Width);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Height )( 
            IWebBrowser2 * This,
            /* [retval][out] */ long *pl);
        
        /* [propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Height )( 
            IWebBrowser2 * This,
            /* [in] */ long Height);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_LocationName )( 
            IWebBrowser2 * This,
            /* [retval][out] */ BSTR *LocationName);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_LocationURL )( 
            IWebBrowser2 * This,
            /* [retval][out] */ BSTR *LocationURL);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Busy )( 
            IWebBrowser2 * This,
            /* [retval][out] */ VARIANT_BOOL *pBool);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Quit )( 
            IWebBrowser2 * This);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ClientToWindow )( 
            IWebBrowser2 * This,
            /* [out][in] */ int *pcx,
            /* [out][in] */ int *pcy);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PutProperty )( 
            IWebBrowser2 * This,
            /* [in] */ BSTR Property,
            /* [in] */ VARIANT vtValue);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProperty )( 
            IWebBrowser2 * This,
            /* [in] */ BSTR Property,
            /* [retval][out] */ VARIANT *pvtValue);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Name )( 
            IWebBrowser2 * This,
            /* [retval][out] */ BSTR *Name);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_HWND )( 
            IWebBrowser2 * This,
            /* [retval][out] */ SHANDLE_PTR *pHWND);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_FullName )( 
            IWebBrowser2 * This,
            /* [retval][out] */ BSTR *FullName);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Path )( 
            IWebBrowser2 * This,
            /* [retval][out] */ BSTR *Path);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Visible )( 
            IWebBrowser2 * This,
            /* [retval][out] */ VARIANT_BOOL *pBool);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Visible )( 
            IWebBrowser2 * This,
            /* [in] */ VARIANT_BOOL Value);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_StatusBar )( 
            IWebBrowser2 * This,
            /* [retval][out] */ VARIANT_BOOL *pBool);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_StatusBar )( 
            IWebBrowser2 * This,
            /* [in] */ VARIANT_BOOL Value);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_StatusText )( 
            IWebBrowser2 * This,
            /* [retval][out] */ BSTR *StatusText);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_StatusText )( 
            IWebBrowser2 * This,
            /* [in] */ BSTR StatusText);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ToolBar )( 
            IWebBrowser2 * This,
            /* [retval][out] */ int *Value);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_ToolBar )( 
            IWebBrowser2 * This,
            /* [in] */ int Value);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_MenuBar )( 
            IWebBrowser2 * This,
            /* [retval][out] */ VARIANT_BOOL *Value);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_MenuBar )( 
            IWebBrowser2 * This,
            /* [in] */ VARIANT_BOOL Value);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_FullScreen )( 
            IWebBrowser2 * This,
            /* [retval][out] */ VARIANT_BOOL *pbFullScreen);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_FullScreen )( 
            IWebBrowser2 * This,
            /* [in] */ VARIANT_BOOL bFullScreen);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Navigate2 )( 
            IWebBrowser2 * This,
            /* [in] */ VARIANT *URL,
            /* [optional][in] */ VARIANT *Flags,
            /* [optional][in] */ VARIANT *TargetFrameName,
            /* [optional][in] */ VARIANT *PostData,
            /* [optional][in] */ VARIANT *Headers);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *QueryStatusWB )( 
            IWebBrowser2 * This,
            /* [in] */ OLECMDID cmdID,
            /* [retval][out] */ OLECMDF *pcmdf);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ExecWB )( 
            IWebBrowser2 * This,
            /* [in] */ OLECMDID cmdID,
            /* [in] */ OLECMDEXECOPT cmdexecopt,
            /* [optional][in] */ VARIANT *pvaIn,
            /* [optional][in][out] */ VARIANT *pvaOut);
        
        /* [helpcontext][helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ShowBrowserBar )( 
            IWebBrowser2 * This,
            /* [in] */ VARIANT *pvaClsid,
            /* [optional][in] */ VARIANT *pvarShow,
            /* [optional][in] */ VARIANT *pvarSize);
        
        /* [bindable][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_ReadyState )( 
            IWebBrowser2 * This,
            /* [out][retval] */ READYSTATE *plReadyState);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Offline )( 
            IWebBrowser2 * This,
            /* [retval][out] */ VARIANT_BOOL *pbOffline);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Offline )( 
            IWebBrowser2 * This,
            /* [in] */ VARIANT_BOOL bOffline);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Silent )( 
            IWebBrowser2 * This,
            /* [retval][out] */ VARIANT_BOOL *pbSilent);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Silent )( 
            IWebBrowser2 * This,
            /* [in] */ VARIANT_BOOL bSilent);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_RegisterAsBrowser )( 
            IWebBrowser2 * This,
            /* [retval][out] */ VARIANT_BOOL *pbRegister);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_RegisterAsBrowser )( 
            IWebBrowser2 * This,
            /* [in] */ VARIANT_BOOL bRegister);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_RegisterAsDropTarget )( 
            IWebBrowser2 * This,
            /* [retval][out] */ VARIANT_BOOL *pbRegister);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_RegisterAsDropTarget )( 
            IWebBrowser2 * This,
            /* [in] */ VARIANT_BOOL bRegister);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_TheaterMode )( 
            IWebBrowser2 * This,
            /* [retval][out] */ VARIANT_BOOL *pbRegister);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_TheaterMode )( 
            IWebBrowser2 * This,
            /* [in] */ VARIANT_BOOL bRegister);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_AddressBar )( 
            IWebBrowser2 * This,
            /* [retval][out] */ VARIANT_BOOL *Value);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_AddressBar )( 
            IWebBrowser2 * This,
            /* [in] */ VARIANT_BOOL Value);
        
        /* [helpcontext][helpstring][propget][id] */ HRESULT ( STDMETHODCALLTYPE *get_Resizable )( 
            IWebBrowser2 * This,
            /* [retval][out] */ VARIANT_BOOL *Value);
        
        /* [helpcontext][helpstring][propput][id] */ HRESULT ( STDMETHODCALLTYPE *put_Resizable )( 
            IWebBrowser2 * This,
            /* [in] */ VARIANT_BOOL Value);
        
        END_INTERFACE
    } IWebBrowser2Vtbl;

    interface IWebBrowser2
    {
        CONST_VTBL struct IWebBrowser2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebBrowser2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWebBrowser2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWebBrowser2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWebBrowser2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWebBrowser2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWebBrowser2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWebBrowser2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWebBrowser2_GoBack(This)	\
    (This)->lpVtbl -> GoBack(This)

#define IWebBrowser2_GoForward(This)	\
    (This)->lpVtbl -> GoForward(This)

#define IWebBrowser2_GoHome(This)	\
    (This)->lpVtbl -> GoHome(This)

#define IWebBrowser2_GoSearch(This)	\
    (This)->lpVtbl -> GoSearch(This)

#define IWebBrowser2_Navigate(This,URL,Flags,TargetFrameName,PostData,Headers)	\
    (This)->lpVtbl -> Navigate(This,URL,Flags,TargetFrameName,PostData,Headers)

#define IWebBrowser2_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IWebBrowser2_Refresh2(This,Level)	\
    (This)->lpVtbl -> Refresh2(This,Level)

#define IWebBrowser2_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define IWebBrowser2_get_Application(This,ppDisp)	\
    (This)->lpVtbl -> get_Application(This,ppDisp)

#define IWebBrowser2_get_Parent(This,ppDisp)	\
    (This)->lpVtbl -> get_Parent(This,ppDisp)

#define IWebBrowser2_get_Container(This,ppDisp)	\
    (This)->lpVtbl -> get_Container(This,ppDisp)

#define IWebBrowser2_get_Document(This,ppDisp)	\
    (This)->lpVtbl -> get_Document(This,ppDisp)

#define IWebBrowser2_get_TopLevelContainer(This,pBool)	\
    (This)->lpVtbl -> get_TopLevelContainer(This,pBool)

#define IWebBrowser2_get_Type(This,Type)	\
    (This)->lpVtbl -> get_Type(This,Type)

#define IWebBrowser2_get_Left(This,pl)	\
    (This)->lpVtbl -> get_Left(This,pl)

#define IWebBrowser2_put_Left(This,Left)	\
    (This)->lpVtbl -> put_Left(This,Left)

#define IWebBrowser2_get_Top(This,pl)	\
    (This)->lpVtbl -> get_Top(This,pl)

#define IWebBrowser2_put_Top(This,Top)	\
    (This)->lpVtbl -> put_Top(This,Top)

#define IWebBrowser2_get_Width(This,pl)	\
    (This)->lpVtbl -> get_Width(This,pl)

#define IWebBrowser2_put_Width(This,Width)	\
    (This)->lpVtbl -> put_Width(This,Width)

#define IWebBrowser2_get_Height(This,pl)	\
    (This)->lpVtbl -> get_Height(This,pl)

#define IWebBrowser2_put_Height(This,Height)	\
    (This)->lpVtbl -> put_Height(This,Height)

#define IWebBrowser2_get_LocationName(This,LocationName)	\
    (This)->lpVtbl -> get_LocationName(This,LocationName)

#define IWebBrowser2_get_LocationURL(This,LocationURL)	\
    (This)->lpVtbl -> get_LocationURL(This,LocationURL)

#define IWebBrowser2_get_Busy(This,pBool)	\
    (This)->lpVtbl -> get_Busy(This,pBool)


#define IWebBrowser2_Quit(This)	\
    (This)->lpVtbl -> Quit(This)

#define IWebBrowser2_ClientToWindow(This,pcx,pcy)	\
    (This)->lpVtbl -> ClientToWindow(This,pcx,pcy)

#define IWebBrowser2_PutProperty(This,Property,vtValue)	\
    (This)->lpVtbl -> PutProperty(This,Property,vtValue)

#define IWebBrowser2_GetProperty(This,Property,pvtValue)	\
    (This)->lpVtbl -> GetProperty(This,Property,pvtValue)

#define IWebBrowser2_get_Name(This,Name)	\
    (This)->lpVtbl -> get_Name(This,Name)

#define IWebBrowser2_get_HWND(This,pHWND)	\
    (This)->lpVtbl -> get_HWND(This,pHWND)

#define IWebBrowser2_get_FullName(This,FullName)	\
    (This)->lpVtbl -> get_FullName(This,FullName)

#define IWebBrowser2_get_Path(This,Path)	\
    (This)->lpVtbl -> get_Path(This,Path)

#define IWebBrowser2_get_Visible(This,pBool)	\
    (This)->lpVtbl -> get_Visible(This,pBool)

#define IWebBrowser2_put_Visible(This,Value)	\
    (This)->lpVtbl -> put_Visible(This,Value)

#define IWebBrowser2_get_StatusBar(This,pBool)	\
    (This)->lpVtbl -> get_StatusBar(This,pBool)

#define IWebBrowser2_put_StatusBar(This,Value)	\
    (This)->lpVtbl -> put_StatusBar(This,Value)

#define IWebBrowser2_get_StatusText(This,StatusText)	\
    (This)->lpVtbl -> get_StatusText(This,StatusText)

#define IWebBrowser2_put_StatusText(This,StatusText)	\
    (This)->lpVtbl -> put_StatusText(This,StatusText)

#define IWebBrowser2_get_ToolBar(This,Value)	\
    (This)->lpVtbl -> get_ToolBar(This,Value)

#define IWebBrowser2_put_ToolBar(This,Value)	\
    (This)->lpVtbl -> put_ToolBar(This,Value)

#define IWebBrowser2_get_MenuBar(This,Value)	\
    (This)->lpVtbl -> get_MenuBar(This,Value)

#define IWebBrowser2_put_MenuBar(This,Value)	\
    (This)->lpVtbl -> put_MenuBar(This,Value)

#define IWebBrowser2_get_FullScreen(This,pbFullScreen)	\
    (This)->lpVtbl -> get_FullScreen(This,pbFullScreen)

#define IWebBrowser2_put_FullScreen(This,bFullScreen)	\
    (This)->lpVtbl -> put_FullScreen(This,bFullScreen)


#define IWebBrowser2_Navigate2(This,URL,Flags,TargetFrameName,PostData,Headers)	\
    (This)->lpVtbl -> Navigate2(This,URL,Flags,TargetFrameName,PostData,Headers)

#define IWebBrowser2_QueryStatusWB(This,cmdID,pcmdf)	\
    (This)->lpVtbl -> QueryStatusWB(This,cmdID,pcmdf)

#define IWebBrowser2_ExecWB(This,cmdID,cmdexecopt,pvaIn,pvaOut)	\
    (This)->lpVtbl -> ExecWB(This,cmdID,cmdexecopt,pvaIn,pvaOut)

#define IWebBrowser2_ShowBrowserBar(This,pvaClsid,pvarShow,pvarSize)	\
    (This)->lpVtbl -> ShowBrowserBar(This,pvaClsid,pvarShow,pvarSize)

#define IWebBrowser2_get_ReadyState(This,plReadyState)	\
    (This)->lpVtbl -> get_ReadyState(This,plReadyState)

#define IWebBrowser2_get_Offline(This,pbOffline)	\
    (This)->lpVtbl -> get_Offline(This,pbOffline)

#define IWebBrowser2_put_Offline(This,bOffline)	\
    (This)->lpVtbl -> put_Offline(This,bOffline)

#define IWebBrowser2_get_Silent(This,pbSilent)	\
    (This)->lpVtbl -> get_Silent(This,pbSilent)

#define IWebBrowser2_put_Silent(This,bSilent)	\
    (This)->lpVtbl -> put_Silent(This,bSilent)

#define IWebBrowser2_get_RegisterAsBrowser(This,pbRegister)	\
    (This)->lpVtbl -> get_RegisterAsBrowser(This,pbRegister)

#define IWebBrowser2_put_RegisterAsBrowser(This,bRegister)	\
    (This)->lpVtbl -> put_RegisterAsBrowser(This,bRegister)

#define IWebBrowser2_get_RegisterAsDropTarget(This,pbRegister)	\
    (This)->lpVtbl -> get_RegisterAsDropTarget(This,pbRegister)

#define IWebBrowser2_put_RegisterAsDropTarget(This,bRegister)	\
    (This)->lpVtbl -> put_RegisterAsDropTarget(This,bRegister)

#define IWebBrowser2_get_TheaterMode(This,pbRegister)	\
    (This)->lpVtbl -> get_TheaterMode(This,pbRegister)

#define IWebBrowser2_put_TheaterMode(This,bRegister)	\
    (This)->lpVtbl -> put_TheaterMode(This,bRegister)

#define IWebBrowser2_get_AddressBar(This,Value)	\
    (This)->lpVtbl -> get_AddressBar(This,Value)

#define IWebBrowser2_put_AddressBar(This,Value)	\
    (This)->lpVtbl -> put_AddressBar(This,Value)

#define IWebBrowser2_get_Resizable(This,Value)	\
    (This)->lpVtbl -> get_Resizable(This,Value)

#define IWebBrowser2_put_Resizable(This,Value)	\
    (This)->lpVtbl -> put_Resizable(This,Value)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser2_Navigate2_Proxy( 
    IWebBrowser2 * This,
    /* [in] */ VARIANT *URL,
    /* [optional][in] */ VARIANT *Flags,
    /* [optional][in] */ VARIANT *TargetFrameName,
    /* [optional][in] */ VARIANT *PostData,
    /* [optional][in] */ VARIANT *Headers);


void __RPC_STUB IWebBrowser2_Navigate2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser2_QueryStatusWB_Proxy( 
    IWebBrowser2 * This,
    /* [in] */ OLECMDID cmdID,
    /* [retval][out] */ OLECMDF *pcmdf);


void __RPC_STUB IWebBrowser2_QueryStatusWB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser2_ExecWB_Proxy( 
    IWebBrowser2 * This,
    /* [in] */ OLECMDID cmdID,
    /* [in] */ OLECMDEXECOPT cmdexecopt,
    /* [optional][in] */ VARIANT *pvaIn,
    /* [optional][in][out] */ VARIANT *pvaOut);


void __RPC_STUB IWebBrowser2_ExecWB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser2_ShowBrowserBar_Proxy( 
    IWebBrowser2 * This,
    /* [in] */ VARIANT *pvaClsid,
    /* [optional][in] */ VARIANT *pvarShow,
    /* [optional][in] */ VARIANT *pvarSize);


void __RPC_STUB IWebBrowser2_ShowBrowserBar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [bindable][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser2_get_ReadyState_Proxy( 
    IWebBrowser2 * This,
    /* [out][retval] */ READYSTATE *plReadyState);


void __RPC_STUB IWebBrowser2_get_ReadyState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser2_get_Offline_Proxy( 
    IWebBrowser2 * This,
    /* [retval][out] */ VARIANT_BOOL *pbOffline);


void __RPC_STUB IWebBrowser2_get_Offline_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser2_put_Offline_Proxy( 
    IWebBrowser2 * This,
    /* [in] */ VARIANT_BOOL bOffline);


void __RPC_STUB IWebBrowser2_put_Offline_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser2_get_Silent_Proxy( 
    IWebBrowser2 * This,
    /* [retval][out] */ VARIANT_BOOL *pbSilent);


void __RPC_STUB IWebBrowser2_get_Silent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser2_put_Silent_Proxy( 
    IWebBrowser2 * This,
    /* [in] */ VARIANT_BOOL bSilent);


void __RPC_STUB IWebBrowser2_put_Silent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser2_get_RegisterAsBrowser_Proxy( 
    IWebBrowser2 * This,
    /* [retval][out] */ VARIANT_BOOL *pbRegister);


void __RPC_STUB IWebBrowser2_get_RegisterAsBrowser_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser2_put_RegisterAsBrowser_Proxy( 
    IWebBrowser2 * This,
    /* [in] */ VARIANT_BOOL bRegister);


void __RPC_STUB IWebBrowser2_put_RegisterAsBrowser_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser2_get_RegisterAsDropTarget_Proxy( 
    IWebBrowser2 * This,
    /* [retval][out] */ VARIANT_BOOL *pbRegister);


void __RPC_STUB IWebBrowser2_get_RegisterAsDropTarget_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser2_put_RegisterAsDropTarget_Proxy( 
    IWebBrowser2 * This,
    /* [in] */ VARIANT_BOOL bRegister);


void __RPC_STUB IWebBrowser2_put_RegisterAsDropTarget_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser2_get_TheaterMode_Proxy( 
    IWebBrowser2 * This,
    /* [retval][out] */ VARIANT_BOOL *pbRegister);


void __RPC_STUB IWebBrowser2_get_TheaterMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser2_put_TheaterMode_Proxy( 
    IWebBrowser2 * This,
    /* [in] */ VARIANT_BOOL bRegister);


void __RPC_STUB IWebBrowser2_put_TheaterMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser2_get_AddressBar_Proxy( 
    IWebBrowser2 * This,
    /* [retval][out] */ VARIANT_BOOL *Value);


void __RPC_STUB IWebBrowser2_get_AddressBar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser2_put_AddressBar_Proxy( 
    IWebBrowser2 * This,
    /* [in] */ VARIANT_BOOL Value);


void __RPC_STUB IWebBrowser2_put_AddressBar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propget][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser2_get_Resizable_Proxy( 
    IWebBrowser2 * This,
    /* [retval][out] */ VARIANT_BOOL *Value);


void __RPC_STUB IWebBrowser2_get_Resizable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpcontext][helpstring][propput][id] */ HRESULT STDMETHODCALLTYPE IWebBrowser2_put_Resizable_Proxy( 
    IWebBrowser2 * This,
    /* [in] */ VARIANT_BOOL Value);


void __RPC_STUB IWebBrowser2_put_Resizable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWebBrowser2_INTERFACE_DEFINED__ */


#ifndef __DWebBrowserEvents2_DISPINTERFACE_DEFINED__
#define __DWebBrowserEvents2_DISPINTERFACE_DEFINED__

/* dispinterface DWebBrowserEvents2 */
/* [hidden][helpstring][uuid] */ 


EXTERN_C const IID DIID_DWebBrowserEvents2;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("34A715A0-6587-11D0-924A-0020AFC7AC4D")
    DWebBrowserEvents2 : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct DWebBrowserEvents2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            DWebBrowserEvents2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            DWebBrowserEvents2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            DWebBrowserEvents2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            DWebBrowserEvents2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            DWebBrowserEvents2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            DWebBrowserEvents2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            DWebBrowserEvents2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } DWebBrowserEvents2Vtbl;

    interface DWebBrowserEvents2
    {
        CONST_VTBL struct DWebBrowserEvents2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define DWebBrowserEvents2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define DWebBrowserEvents2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define DWebBrowserEvents2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define DWebBrowserEvents2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define DWebBrowserEvents2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define DWebBrowserEvents2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define DWebBrowserEvents2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __DWebBrowserEvents2_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_WebBrowser_V1;

#ifdef __cplusplus

class DECLSPEC_UUID("EAB22AC3-30C1-11CF-A7EB-0000C05BAE0B")
WebBrowser_V1;
#endif

EXTERN_C const CLSID CLSID_WebBrowser;

#ifdef __cplusplus

class DECLSPEC_UUID("8856F961-340A-11D0-A96B-00C04FD705A2")
WebBrowser;
#endif

EXTERN_C const CLSID CLSID_InternetExplorer;

#ifdef __cplusplus

class DECLSPEC_UUID("0002DF01-0000-0000-C000-000000000046")
InternetExplorer;
#endif

EXTERN_C const CLSID CLSID_ShellBrowserWindow;

#ifdef __cplusplus

class DECLSPEC_UUID("c08afd90-f2a1-11d1-8455-00a0c91f3880")
ShellBrowserWindow;
#endif

#ifndef __DShellWindowsEvents_DISPINTERFACE_DEFINED__
#define __DShellWindowsEvents_DISPINTERFACE_DEFINED__

/* dispinterface DShellWindowsEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID_DShellWindowsEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("fe4106e0-399a-11d0-a48c-00a0c90a8f39")
    DShellWindowsEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct DShellWindowsEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            DShellWindowsEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            DShellWindowsEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            DShellWindowsEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            DShellWindowsEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            DShellWindowsEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            DShellWindowsEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            DShellWindowsEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } DShellWindowsEventsVtbl;

    interface DShellWindowsEvents
    {
        CONST_VTBL struct DShellWindowsEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define DShellWindowsEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define DShellWindowsEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define DShellWindowsEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define DShellWindowsEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define DShellWindowsEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define DShellWindowsEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define DShellWindowsEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __DShellWindowsEvents_DISPINTERFACE_DEFINED__ */


#ifndef __IShellWindows_INTERFACE_DEFINED__
#define __IShellWindows_INTERFACE_DEFINED__

/* interface IShellWindows */
/* [object][dual][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_IShellWindows;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("85CB6900-4D95-11CF-960C-0080C7F4EE85")
    IShellWindows : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *Count) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [optional][in] */ VARIANT index,
            /* [retval][out] */ IDispatch **Folder) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE _NewEnum( 
            /* [retval][out] */ IUnknown **ppunk) = 0;
        
        virtual /* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE Register( 
            /* [in] */ IDispatch *pid,
            /* [in] */ long hwnd,
            /* [in] */ int swClass,
            /* [out] */ long *plCookie) = 0;
        
        virtual /* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE RegisterPending( 
            /* [in] */ long lThreadId,
            /* [in] */ VARIANT *pvarloc,
            /* [in] */ VARIANT *pvarlocRoot,
            /* [in] */ int swClass,
            /* [out] */ long *plCookie) = 0;
        
        virtual /* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE Revoke( 
            /* [in] */ long lCookie) = 0;
        
        virtual /* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE OnNavigate( 
            /* [in] */ long lCookie,
            /* [in] */ VARIANT *pvarLoc) = 0;
        
        virtual /* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE OnActivated( 
            /* [in] */ long lCookie,
            /* [in] */ VARIANT_BOOL fActive) = 0;
        
        virtual /* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE FindWindowSW( 
            /* [in] */ VARIANT *pvarLoc,
            /* [in] */ VARIANT *pvarLocRoot,
            /* [in] */ int swClass,
            /* [out] */ long *phwnd,
            /* [in] */ int swfwOptions,
            /* [retval][out] */ IDispatch **ppdispOut) = 0;
        
        virtual /* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE OnCreated( 
            /* [in] */ long lCookie,
            /* [in] */ IUnknown *punk) = 0;
        
        virtual /* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE ProcessAttachDetach( 
            /* [in] */ VARIANT_BOOL fAttach) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellWindowsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellWindows * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellWindows * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellWindows * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IShellWindows * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IShellWindows * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IShellWindows * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IShellWindows * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IShellWindows * This,
            /* [retval][out] */ long *Count);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            IShellWindows * This,
            /* [optional][in] */ VARIANT index,
            /* [retval][out] */ IDispatch **Folder);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *_NewEnum )( 
            IShellWindows * This,
            /* [retval][out] */ IUnknown **ppunk);
        
        /* [hidden][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Register )( 
            IShellWindows * This,
            /* [in] */ IDispatch *pid,
            /* [in] */ long hwnd,
            /* [in] */ int swClass,
            /* [out] */ long *plCookie);
        
        /* [hidden][helpstring] */ HRESULT ( STDMETHODCALLTYPE *RegisterPending )( 
            IShellWindows * This,
            /* [in] */ long lThreadId,
            /* [in] */ VARIANT *pvarloc,
            /* [in] */ VARIANT *pvarlocRoot,
            /* [in] */ int swClass,
            /* [out] */ long *plCookie);
        
        /* [hidden][helpstring] */ HRESULT ( STDMETHODCALLTYPE *Revoke )( 
            IShellWindows * This,
            /* [in] */ long lCookie);
        
        /* [hidden][helpstring] */ HRESULT ( STDMETHODCALLTYPE *OnNavigate )( 
            IShellWindows * This,
            /* [in] */ long lCookie,
            /* [in] */ VARIANT *pvarLoc);
        
        /* [hidden][helpstring] */ HRESULT ( STDMETHODCALLTYPE *OnActivated )( 
            IShellWindows * This,
            /* [in] */ long lCookie,
            /* [in] */ VARIANT_BOOL fActive);
        
        /* [hidden][helpstring] */ HRESULT ( STDMETHODCALLTYPE *FindWindowSW )( 
            IShellWindows * This,
            /* [in] */ VARIANT *pvarLoc,
            /* [in] */ VARIANT *pvarLocRoot,
            /* [in] */ int swClass,
            /* [out] */ long *phwnd,
            /* [in] */ int swfwOptions,
            /* [retval][out] */ IDispatch **ppdispOut);
        
        /* [hidden][helpstring] */ HRESULT ( STDMETHODCALLTYPE *OnCreated )( 
            IShellWindows * This,
            /* [in] */ long lCookie,
            /* [in] */ IUnknown *punk);
        
        /* [hidden][helpstring] */ HRESULT ( STDMETHODCALLTYPE *ProcessAttachDetach )( 
            IShellWindows * This,
            /* [in] */ VARIANT_BOOL fAttach);
        
        END_INTERFACE
    } IShellWindowsVtbl;

    interface IShellWindows
    {
        CONST_VTBL struct IShellWindowsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellWindows_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellWindows_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellWindows_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellWindows_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IShellWindows_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IShellWindows_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IShellWindows_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IShellWindows_get_Count(This,Count)	\
    (This)->lpVtbl -> get_Count(This,Count)

#define IShellWindows_Item(This,index,Folder)	\
    (This)->lpVtbl -> Item(This,index,Folder)

#define IShellWindows__NewEnum(This,ppunk)	\
    (This)->lpVtbl -> _NewEnum(This,ppunk)

#define IShellWindows_Register(This,pid,hwnd,swClass,plCookie)	\
    (This)->lpVtbl -> Register(This,pid,hwnd,swClass,plCookie)

#define IShellWindows_RegisterPending(This,lThreadId,pvarloc,pvarlocRoot,swClass,plCookie)	\
    (This)->lpVtbl -> RegisterPending(This,lThreadId,pvarloc,pvarlocRoot,swClass,plCookie)

#define IShellWindows_Revoke(This,lCookie)	\
    (This)->lpVtbl -> Revoke(This,lCookie)

#define IShellWindows_OnNavigate(This,lCookie,pvarLoc)	\
    (This)->lpVtbl -> OnNavigate(This,lCookie,pvarLoc)

#define IShellWindows_OnActivated(This,lCookie,fActive)	\
    (This)->lpVtbl -> OnActivated(This,lCookie,fActive)

#define IShellWindows_FindWindowSW(This,pvarLoc,pvarLocRoot,swClass,phwnd,swfwOptions,ppdispOut)	\
    (This)->lpVtbl -> FindWindowSW(This,pvarLoc,pvarLocRoot,swClass,phwnd,swfwOptions,ppdispOut)

#define IShellWindows_OnCreated(This,lCookie,punk)	\
    (This)->lpVtbl -> OnCreated(This,lCookie,punk)

#define IShellWindows_ProcessAttachDetach(This,fAttach)	\
    (This)->lpVtbl -> ProcessAttachDetach(This,fAttach)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IShellWindows_get_Count_Proxy( 
    IShellWindows * This,
    /* [retval][out] */ long *Count);


void __RPC_STUB IShellWindows_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IShellWindows_Item_Proxy( 
    IShellWindows * This,
    /* [optional][in] */ VARIANT index,
    /* [retval][out] */ IDispatch **Folder);


void __RPC_STUB IShellWindows_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IShellWindows__NewEnum_Proxy( 
    IShellWindows * This,
    /* [retval][out] */ IUnknown **ppunk);


void __RPC_STUB IShellWindows__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE IShellWindows_Register_Proxy( 
    IShellWindows * This,
    /* [in] */ IDispatch *pid,
    /* [in] */ long hwnd,
    /* [in] */ int swClass,
    /* [out] */ long *plCookie);


void __RPC_STUB IShellWindows_Register_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE IShellWindows_RegisterPending_Proxy( 
    IShellWindows * This,
    /* [in] */ long lThreadId,
    /* [in] */ VARIANT *pvarloc,
    /* [in] */ VARIANT *pvarlocRoot,
    /* [in] */ int swClass,
    /* [out] */ long *plCookie);


void __RPC_STUB IShellWindows_RegisterPending_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE IShellWindows_Revoke_Proxy( 
    IShellWindows * This,
    /* [in] */ long lCookie);


void __RPC_STUB IShellWindows_Revoke_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE IShellWindows_OnNavigate_Proxy( 
    IShellWindows * This,
    /* [in] */ long lCookie,
    /* [in] */ VARIANT *pvarLoc);


void __RPC_STUB IShellWindows_OnNavigate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE IShellWindows_OnActivated_Proxy( 
    IShellWindows * This,
    /* [in] */ long lCookie,
    /* [in] */ VARIANT_BOOL fActive);


void __RPC_STUB IShellWindows_OnActivated_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE IShellWindows_FindWindowSW_Proxy( 
    IShellWindows * This,
    /* [in] */ VARIANT *pvarLoc,
    /* [in] */ VARIANT *pvarLocRoot,
    /* [in] */ int swClass,
    /* [out] */ long *phwnd,
    /* [in] */ int swfwOptions,
    /* [retval][out] */ IDispatch **ppdispOut);


void __RPC_STUB IShellWindows_FindWindowSW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE IShellWindows_OnCreated_Proxy( 
    IShellWindows * This,
    /* [in] */ long lCookie,
    /* [in] */ IUnknown *punk);


void __RPC_STUB IShellWindows_OnCreated_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE IShellWindows_ProcessAttachDetach_Proxy( 
    IShellWindows * This,
    /* [in] */ VARIANT_BOOL fAttach);


void __RPC_STUB IShellWindows_ProcessAttachDetach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellWindows_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ShellWindows;

#ifdef __cplusplus

class DECLSPEC_UUID("9BA05972-F6A8-11CF-A442-00A0C90A8F39")
ShellWindows;
#endif

#ifndef __IShellUIHelper_INTERFACE_DEFINED__
#define __IShellUIHelper_INTERFACE_DEFINED__

/* interface IShellUIHelper */
/* [object][dual][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_IShellUIHelper;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("729FE2F8-1EA8-11d1-8F85-00C04FC2FBE1")
    IShellUIHelper : public IDispatch
    {
    public:
        virtual /* [id][hidden] */ HRESULT STDMETHODCALLTYPE ResetFirstBootMode( void) = 0;
        
        virtual /* [id][hidden] */ HRESULT STDMETHODCALLTYPE ResetSafeMode( void) = 0;
        
        virtual /* [id][hidden] */ HRESULT STDMETHODCALLTYPE RefreshOfflineDesktop( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE AddFavorite( 
            /* [in] */ BSTR URL,
            /* [in][optional] */ VARIANT *Title) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE AddChannel( 
            /* [in] */ BSTR URL) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE AddDesktopComponent( 
            /* [in] */ BSTR URL,
            /* [in] */ BSTR Type,
            /* [in][optional] */ VARIANT *Left,
            /* [in][optional] */ VARIANT *Top,
            /* [in][optional] */ VARIANT *Width,
            /* [in][optional] */ VARIANT *Height) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsSubscribed( 
            /* [in] */ BSTR URL,
            /* [retval][out] */ VARIANT_BOOL *pBool) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE NavigateAndFind( 
            /* [in] */ BSTR URL,
            /* [in] */ BSTR strQuery,
            /* [in] */ VARIANT *varTargetFrame) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ImportExportFavorites( 
            /* [in] */ VARIANT_BOOL fImport,
            /* [in] */ BSTR strImpExpPath) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE AutoCompleteSaveForm( 
            /* [in][optional] */ VARIANT *Form) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE AutoScan( 
            /* [in] */ BSTR strSearch,
            /* [in] */ BSTR strFailureUrl,
            /* [in][optional] */ VARIANT *pvarTargetFrame) = 0;
        
        virtual /* [id][hidden] */ HRESULT STDMETHODCALLTYPE AutoCompleteAttach( 
            /* [in][optional] */ VARIANT *Reserved) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ShowBrowserUI( 
            /* [in] */ BSTR bstrName,
            /* [in] */ VARIANT *pvarIn,
            /* [retval][out] */ VARIANT *pvarOut) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellUIHelperVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellUIHelper * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellUIHelper * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellUIHelper * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IShellUIHelper * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IShellUIHelper * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IShellUIHelper * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IShellUIHelper * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][hidden] */ HRESULT ( STDMETHODCALLTYPE *ResetFirstBootMode )( 
            IShellUIHelper * This);
        
        /* [id][hidden] */ HRESULT ( STDMETHODCALLTYPE *ResetSafeMode )( 
            IShellUIHelper * This);
        
        /* [id][hidden] */ HRESULT ( STDMETHODCALLTYPE *RefreshOfflineDesktop )( 
            IShellUIHelper * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *AddFavorite )( 
            IShellUIHelper * This,
            /* [in] */ BSTR URL,
            /* [in][optional] */ VARIANT *Title);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *AddChannel )( 
            IShellUIHelper * This,
            /* [in] */ BSTR URL);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *AddDesktopComponent )( 
            IShellUIHelper * This,
            /* [in] */ BSTR URL,
            /* [in] */ BSTR Type,
            /* [in][optional] */ VARIANT *Left,
            /* [in][optional] */ VARIANT *Top,
            /* [in][optional] */ VARIANT *Width,
            /* [in][optional] */ VARIANT *Height);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *IsSubscribed )( 
            IShellUIHelper * This,
            /* [in] */ BSTR URL,
            /* [retval][out] */ VARIANT_BOOL *pBool);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *NavigateAndFind )( 
            IShellUIHelper * This,
            /* [in] */ BSTR URL,
            /* [in] */ BSTR strQuery,
            /* [in] */ VARIANT *varTargetFrame);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ImportExportFavorites )( 
            IShellUIHelper * This,
            /* [in] */ VARIANT_BOOL fImport,
            /* [in] */ BSTR strImpExpPath);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *AutoCompleteSaveForm )( 
            IShellUIHelper * This,
            /* [in][optional] */ VARIANT *Form);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *AutoScan )( 
            IShellUIHelper * This,
            /* [in] */ BSTR strSearch,
            /* [in] */ BSTR strFailureUrl,
            /* [in][optional] */ VARIANT *pvarTargetFrame);
        
        /* [id][hidden] */ HRESULT ( STDMETHODCALLTYPE *AutoCompleteAttach )( 
            IShellUIHelper * This,
            /* [in][optional] */ VARIANT *Reserved);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ShowBrowserUI )( 
            IShellUIHelper * This,
            /* [in] */ BSTR bstrName,
            /* [in] */ VARIANT *pvarIn,
            /* [retval][out] */ VARIANT *pvarOut);
        
        END_INTERFACE
    } IShellUIHelperVtbl;

    interface IShellUIHelper
    {
        CONST_VTBL struct IShellUIHelperVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellUIHelper_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellUIHelper_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellUIHelper_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellUIHelper_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IShellUIHelper_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IShellUIHelper_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IShellUIHelper_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IShellUIHelper_ResetFirstBootMode(This)	\
    (This)->lpVtbl -> ResetFirstBootMode(This)

#define IShellUIHelper_ResetSafeMode(This)	\
    (This)->lpVtbl -> ResetSafeMode(This)

#define IShellUIHelper_RefreshOfflineDesktop(This)	\
    (This)->lpVtbl -> RefreshOfflineDesktop(This)

#define IShellUIHelper_AddFavorite(This,URL,Title)	\
    (This)->lpVtbl -> AddFavorite(This,URL,Title)

#define IShellUIHelper_AddChannel(This,URL)	\
    (This)->lpVtbl -> AddChannel(This,URL)

#define IShellUIHelper_AddDesktopComponent(This,URL,Type,Left,Top,Width,Height)	\
    (This)->lpVtbl -> AddDesktopComponent(This,URL,Type,Left,Top,Width,Height)

#define IShellUIHelper_IsSubscribed(This,URL,pBool)	\
    (This)->lpVtbl -> IsSubscribed(This,URL,pBool)

#define IShellUIHelper_NavigateAndFind(This,URL,strQuery,varTargetFrame)	\
    (This)->lpVtbl -> NavigateAndFind(This,URL,strQuery,varTargetFrame)

#define IShellUIHelper_ImportExportFavorites(This,fImport,strImpExpPath)	\
    (This)->lpVtbl -> ImportExportFavorites(This,fImport,strImpExpPath)

#define IShellUIHelper_AutoCompleteSaveForm(This,Form)	\
    (This)->lpVtbl -> AutoCompleteSaveForm(This,Form)

#define IShellUIHelper_AutoScan(This,strSearch,strFailureUrl,pvarTargetFrame)	\
    (This)->lpVtbl -> AutoScan(This,strSearch,strFailureUrl,pvarTargetFrame)

#define IShellUIHelper_AutoCompleteAttach(This,Reserved)	\
    (This)->lpVtbl -> AutoCompleteAttach(This,Reserved)

#define IShellUIHelper_ShowBrowserUI(This,bstrName,pvarIn,pvarOut)	\
    (This)->lpVtbl -> ShowBrowserUI(This,bstrName,pvarIn,pvarOut)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][hidden] */ HRESULT STDMETHODCALLTYPE IShellUIHelper_ResetFirstBootMode_Proxy( 
    IShellUIHelper * This);


void __RPC_STUB IShellUIHelper_ResetFirstBootMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden] */ HRESULT STDMETHODCALLTYPE IShellUIHelper_ResetSafeMode_Proxy( 
    IShellUIHelper * This);


void __RPC_STUB IShellUIHelper_ResetSafeMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden] */ HRESULT STDMETHODCALLTYPE IShellUIHelper_RefreshOfflineDesktop_Proxy( 
    IShellUIHelper * This);


void __RPC_STUB IShellUIHelper_RefreshOfflineDesktop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IShellUIHelper_AddFavorite_Proxy( 
    IShellUIHelper * This,
    /* [in] */ BSTR URL,
    /* [in][optional] */ VARIANT *Title);


void __RPC_STUB IShellUIHelper_AddFavorite_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IShellUIHelper_AddChannel_Proxy( 
    IShellUIHelper * This,
    /* [in] */ BSTR URL);


void __RPC_STUB IShellUIHelper_AddChannel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IShellUIHelper_AddDesktopComponent_Proxy( 
    IShellUIHelper * This,
    /* [in] */ BSTR URL,
    /* [in] */ BSTR Type,
    /* [in][optional] */ VARIANT *Left,
    /* [in][optional] */ VARIANT *Top,
    /* [in][optional] */ VARIANT *Width,
    /* [in][optional] */ VARIANT *Height);


void __RPC_STUB IShellUIHelper_AddDesktopComponent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IShellUIHelper_IsSubscribed_Proxy( 
    IShellUIHelper * This,
    /* [in] */ BSTR URL,
    /* [retval][out] */ VARIANT_BOOL *pBool);


void __RPC_STUB IShellUIHelper_IsSubscribed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IShellUIHelper_NavigateAndFind_Proxy( 
    IShellUIHelper * This,
    /* [in] */ BSTR URL,
    /* [in] */ BSTR strQuery,
    /* [in] */ VARIANT *varTargetFrame);


void __RPC_STUB IShellUIHelper_NavigateAndFind_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IShellUIHelper_ImportExportFavorites_Proxy( 
    IShellUIHelper * This,
    /* [in] */ VARIANT_BOOL fImport,
    /* [in] */ BSTR strImpExpPath);


void __RPC_STUB IShellUIHelper_ImportExportFavorites_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IShellUIHelper_AutoCompleteSaveForm_Proxy( 
    IShellUIHelper * This,
    /* [in][optional] */ VARIANT *Form);


void __RPC_STUB IShellUIHelper_AutoCompleteSaveForm_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IShellUIHelper_AutoScan_Proxy( 
    IShellUIHelper * This,
    /* [in] */ BSTR strSearch,
    /* [in] */ BSTR strFailureUrl,
    /* [in][optional] */ VARIANT *pvarTargetFrame);


void __RPC_STUB IShellUIHelper_AutoScan_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][hidden] */ HRESULT STDMETHODCALLTYPE IShellUIHelper_AutoCompleteAttach_Proxy( 
    IShellUIHelper * This,
    /* [in][optional] */ VARIANT *Reserved);


void __RPC_STUB IShellUIHelper_AutoCompleteAttach_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IShellUIHelper_ShowBrowserUI_Proxy( 
    IShellUIHelper * This,
    /* [in] */ BSTR bstrName,
    /* [in] */ VARIANT *pvarIn,
    /* [retval][out] */ VARIANT *pvarOut);


void __RPC_STUB IShellUIHelper_ShowBrowserUI_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellUIHelper_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ShellUIHelper;

#ifdef __cplusplus

class DECLSPEC_UUID("64AB4BB7-111E-11d1-8F79-00C04FC2FBE1")
ShellUIHelper;
#endif

#ifndef __DShellNameSpaceEvents_DISPINTERFACE_DEFINED__
#define __DShellNameSpaceEvents_DISPINTERFACE_DEFINED__

/* dispinterface DShellNameSpaceEvents */
/* [uuid] */ 


EXTERN_C const IID DIID_DShellNameSpaceEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("55136806-B2DE-11D1-B9F2-00A0C98BC547")
    DShellNameSpaceEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct DShellNameSpaceEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            DShellNameSpaceEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            DShellNameSpaceEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            DShellNameSpaceEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            DShellNameSpaceEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            DShellNameSpaceEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            DShellNameSpaceEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            DShellNameSpaceEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } DShellNameSpaceEventsVtbl;

    interface DShellNameSpaceEvents
    {
        CONST_VTBL struct DShellNameSpaceEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define DShellNameSpaceEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define DShellNameSpaceEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define DShellNameSpaceEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define DShellNameSpaceEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define DShellNameSpaceEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define DShellNameSpaceEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define DShellNameSpaceEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __DShellNameSpaceEvents_DISPINTERFACE_DEFINED__ */


#ifndef __IShellFavoritesNameSpace_INTERFACE_DEFINED__
#define __IShellFavoritesNameSpace_INTERFACE_DEFINED__

/* interface IShellFavoritesNameSpace */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IShellFavoritesNameSpace;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("55136804-B2DE-11D1-B9F2-00A0C98BC547")
    IShellFavoritesNameSpace : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MoveSelectionUp( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MoveSelectionDown( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ResetSort( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE NewFolder( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Synchronize( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Import( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Export( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InvokeContextMenuCommand( 
            /* [in] */ BSTR strCommand) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE MoveSelectionTo( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SubscriptionsEnabled( 
            /* [retval][out] */ VARIANT_BOOL *pBool) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateSubscriptionForSelection( 
            /* [retval][out] */ VARIANT_BOOL *pBool) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteSubscriptionForSelection( 
            /* [retval][out] */ VARIANT_BOOL *pBool) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetRoot( 
            /* [in] */ BSTR bstrFullPath) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellFavoritesNameSpaceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellFavoritesNameSpace * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellFavoritesNameSpace * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellFavoritesNameSpace * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IShellFavoritesNameSpace * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IShellFavoritesNameSpace * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IShellFavoritesNameSpace * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IShellFavoritesNameSpace * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MoveSelectionUp )( 
            IShellFavoritesNameSpace * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MoveSelectionDown )( 
            IShellFavoritesNameSpace * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ResetSort )( 
            IShellFavoritesNameSpace * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *NewFolder )( 
            IShellFavoritesNameSpace * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Synchronize )( 
            IShellFavoritesNameSpace * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Import )( 
            IShellFavoritesNameSpace * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Export )( 
            IShellFavoritesNameSpace * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InvokeContextMenuCommand )( 
            IShellFavoritesNameSpace * This,
            /* [in] */ BSTR strCommand);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MoveSelectionTo )( 
            IShellFavoritesNameSpace * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SubscriptionsEnabled )( 
            IShellFavoritesNameSpace * This,
            /* [retval][out] */ VARIANT_BOOL *pBool);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateSubscriptionForSelection )( 
            IShellFavoritesNameSpace * This,
            /* [retval][out] */ VARIANT_BOOL *pBool);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteSubscriptionForSelection )( 
            IShellFavoritesNameSpace * This,
            /* [retval][out] */ VARIANT_BOOL *pBool);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetRoot )( 
            IShellFavoritesNameSpace * This,
            /* [in] */ BSTR bstrFullPath);
        
        END_INTERFACE
    } IShellFavoritesNameSpaceVtbl;

    interface IShellFavoritesNameSpace
    {
        CONST_VTBL struct IShellFavoritesNameSpaceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellFavoritesNameSpace_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellFavoritesNameSpace_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellFavoritesNameSpace_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellFavoritesNameSpace_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IShellFavoritesNameSpace_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IShellFavoritesNameSpace_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IShellFavoritesNameSpace_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IShellFavoritesNameSpace_MoveSelectionUp(This)	\
    (This)->lpVtbl -> MoveSelectionUp(This)

#define IShellFavoritesNameSpace_MoveSelectionDown(This)	\
    (This)->lpVtbl -> MoveSelectionDown(This)

#define IShellFavoritesNameSpace_ResetSort(This)	\
    (This)->lpVtbl -> ResetSort(This)

#define IShellFavoritesNameSpace_NewFolder(This)	\
    (This)->lpVtbl -> NewFolder(This)

#define IShellFavoritesNameSpace_Synchronize(This)	\
    (This)->lpVtbl -> Synchronize(This)

#define IShellFavoritesNameSpace_Import(This)	\
    (This)->lpVtbl -> Import(This)

#define IShellFavoritesNameSpace_Export(This)	\
    (This)->lpVtbl -> Export(This)

#define IShellFavoritesNameSpace_InvokeContextMenuCommand(This,strCommand)	\
    (This)->lpVtbl -> InvokeContextMenuCommand(This,strCommand)

#define IShellFavoritesNameSpace_MoveSelectionTo(This)	\
    (This)->lpVtbl -> MoveSelectionTo(This)

#define IShellFavoritesNameSpace_get_SubscriptionsEnabled(This,pBool)	\
    (This)->lpVtbl -> get_SubscriptionsEnabled(This,pBool)

#define IShellFavoritesNameSpace_CreateSubscriptionForSelection(This,pBool)	\
    (This)->lpVtbl -> CreateSubscriptionForSelection(This,pBool)

#define IShellFavoritesNameSpace_DeleteSubscriptionForSelection(This,pBool)	\
    (This)->lpVtbl -> DeleteSubscriptionForSelection(This,pBool)

#define IShellFavoritesNameSpace_SetRoot(This,bstrFullPath)	\
    (This)->lpVtbl -> SetRoot(This,bstrFullPath)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IShellFavoritesNameSpace_MoveSelectionUp_Proxy( 
    IShellFavoritesNameSpace * This);


void __RPC_STUB IShellFavoritesNameSpace_MoveSelectionUp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IShellFavoritesNameSpace_MoveSelectionDown_Proxy( 
    IShellFavoritesNameSpace * This);


void __RPC_STUB IShellFavoritesNameSpace_MoveSelectionDown_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IShellFavoritesNameSpace_ResetSort_Proxy( 
    IShellFavoritesNameSpace * This);


void __RPC_STUB IShellFavoritesNameSpace_ResetSort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IShellFavoritesNameSpace_NewFolder_Proxy( 
    IShellFavoritesNameSpace * This);


void __RPC_STUB IShellFavoritesNameSpace_NewFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IShellFavoritesNameSpace_Synchronize_Proxy( 
    IShellFavoritesNameSpace * This);


void __RPC_STUB IShellFavoritesNameSpace_Synchronize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IShellFavoritesNameSpace_Import_Proxy( 
    IShellFavoritesNameSpace * This);


void __RPC_STUB IShellFavoritesNameSpace_Import_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IShellFavoritesNameSpace_Export_Proxy( 
    IShellFavoritesNameSpace * This);


void __RPC_STUB IShellFavoritesNameSpace_Export_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IShellFavoritesNameSpace_InvokeContextMenuCommand_Proxy( 
    IShellFavoritesNameSpace * This,
    /* [in] */ BSTR strCommand);


void __RPC_STUB IShellFavoritesNameSpace_InvokeContextMenuCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IShellFavoritesNameSpace_MoveSelectionTo_Proxy( 
    IShellFavoritesNameSpace * This);


void __RPC_STUB IShellFavoritesNameSpace_MoveSelectionTo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IShellFavoritesNameSpace_get_SubscriptionsEnabled_Proxy( 
    IShellFavoritesNameSpace * This,
    /* [retval][out] */ VARIANT_BOOL *pBool);


void __RPC_STUB IShellFavoritesNameSpace_get_SubscriptionsEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IShellFavoritesNameSpace_CreateSubscriptionForSelection_Proxy( 
    IShellFavoritesNameSpace * This,
    /* [retval][out] */ VARIANT_BOOL *pBool);


void __RPC_STUB IShellFavoritesNameSpace_CreateSubscriptionForSelection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IShellFavoritesNameSpace_DeleteSubscriptionForSelection_Proxy( 
    IShellFavoritesNameSpace * This,
    /* [retval][out] */ VARIANT_BOOL *pBool);


void __RPC_STUB IShellFavoritesNameSpace_DeleteSubscriptionForSelection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IShellFavoritesNameSpace_SetRoot_Proxy( 
    IShellFavoritesNameSpace * This,
    /* [in] */ BSTR bstrFullPath);


void __RPC_STUB IShellFavoritesNameSpace_SetRoot_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellFavoritesNameSpace_INTERFACE_DEFINED__ */


#ifndef __IShellNameSpace_INTERFACE_DEFINED__
#define __IShellNameSpace_INTERFACE_DEFINED__

/* interface IShellNameSpace */
/* [hidden][unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IShellNameSpace;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("e572d3c9-37be-4ae2-825d-d521763e3108")
    IShellNameSpace : public IShellFavoritesNameSpace
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EnumOptions( 
            /* [retval][out] */ LONG *pgrfEnumFlags) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EnumOptions( 
            /* [in] */ LONG lVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SelectedItem( 
            /* [retval][out] */ IDispatch **pItem) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SelectedItem( 
            /* [in] */ IDispatch *pItem) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Root( 
            /* [retval][out] */ VARIANT *pvar) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Root( 
            /* [in] */ VARIANT var) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Depth( 
            /* [retval][out] */ int *piDepth) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Depth( 
            /* [in] */ int iDepth) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Mode( 
            /* [retval][out] */ UINT *puMode) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Mode( 
            /* [in] */ UINT uMode) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Flags( 
            /* [retval][out] */ DWORD *pdwFlags) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Flags( 
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TVFlags( 
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TVFlags( 
            /* [retval][out] */ DWORD *dwFlags) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Columns( 
            /* [retval][out] */ BSTR *bstrColumns) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Columns( 
            /* [in] */ BSTR bstrColumns) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CountViewTypes( 
            /* [retval][out] */ int *piTypes) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetViewType( 
            /* [in] */ int iType) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SelectedItems( 
            /* [retval][out] */ IDispatch **ppid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Expand( 
            /* [in] */ VARIANT var,
            int iDepth) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnselectAll( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellNameSpaceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellNameSpace * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellNameSpace * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellNameSpace * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IShellNameSpace * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IShellNameSpace * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IShellNameSpace * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IShellNameSpace * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MoveSelectionUp )( 
            IShellNameSpace * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MoveSelectionDown )( 
            IShellNameSpace * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ResetSort )( 
            IShellNameSpace * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *NewFolder )( 
            IShellNameSpace * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Synchronize )( 
            IShellNameSpace * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Import )( 
            IShellNameSpace * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Export )( 
            IShellNameSpace * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InvokeContextMenuCommand )( 
            IShellNameSpace * This,
            /* [in] */ BSTR strCommand);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *MoveSelectionTo )( 
            IShellNameSpace * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SubscriptionsEnabled )( 
            IShellNameSpace * This,
            /* [retval][out] */ VARIANT_BOOL *pBool);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateSubscriptionForSelection )( 
            IShellNameSpace * This,
            /* [retval][out] */ VARIANT_BOOL *pBool);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteSubscriptionForSelection )( 
            IShellNameSpace * This,
            /* [retval][out] */ VARIANT_BOOL *pBool);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetRoot )( 
            IShellNameSpace * This,
            /* [in] */ BSTR bstrFullPath);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_EnumOptions )( 
            IShellNameSpace * This,
            /* [retval][out] */ LONG *pgrfEnumFlags);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_EnumOptions )( 
            IShellNameSpace * This,
            /* [in] */ LONG lVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SelectedItem )( 
            IShellNameSpace * This,
            /* [retval][out] */ IDispatch **pItem);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_SelectedItem )( 
            IShellNameSpace * This,
            /* [in] */ IDispatch *pItem);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Root )( 
            IShellNameSpace * This,
            /* [retval][out] */ VARIANT *pvar);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Root )( 
            IShellNameSpace * This,
            /* [in] */ VARIANT var);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Depth )( 
            IShellNameSpace * This,
            /* [retval][out] */ int *piDepth);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Depth )( 
            IShellNameSpace * This,
            /* [in] */ int iDepth);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Mode )( 
            IShellNameSpace * This,
            /* [retval][out] */ UINT *puMode);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Mode )( 
            IShellNameSpace * This,
            /* [in] */ UINT uMode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Flags )( 
            IShellNameSpace * This,
            /* [retval][out] */ DWORD *pdwFlags);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Flags )( 
            IShellNameSpace * This,
            /* [in] */ DWORD dwFlags);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_TVFlags )( 
            IShellNameSpace * This,
            /* [in] */ DWORD dwFlags);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_TVFlags )( 
            IShellNameSpace * This,
            /* [retval][out] */ DWORD *dwFlags);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Columns )( 
            IShellNameSpace * This,
            /* [retval][out] */ BSTR *bstrColumns);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Columns )( 
            IShellNameSpace * This,
            /* [in] */ BSTR bstrColumns);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_CountViewTypes )( 
            IShellNameSpace * This,
            /* [retval][out] */ int *piTypes);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetViewType )( 
            IShellNameSpace * This,
            /* [in] */ int iType);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SelectedItems )( 
            IShellNameSpace * This,
            /* [retval][out] */ IDispatch **ppid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Expand )( 
            IShellNameSpace * This,
            /* [in] */ VARIANT var,
            int iDepth);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnselectAll )( 
            IShellNameSpace * This);
        
        END_INTERFACE
    } IShellNameSpaceVtbl;

    interface IShellNameSpace
    {
        CONST_VTBL struct IShellNameSpaceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellNameSpace_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellNameSpace_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellNameSpace_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellNameSpace_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IShellNameSpace_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IShellNameSpace_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IShellNameSpace_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IShellNameSpace_MoveSelectionUp(This)	\
    (This)->lpVtbl -> MoveSelectionUp(This)

#define IShellNameSpace_MoveSelectionDown(This)	\
    (This)->lpVtbl -> MoveSelectionDown(This)

#define IShellNameSpace_ResetSort(This)	\
    (This)->lpVtbl -> ResetSort(This)

#define IShellNameSpace_NewFolder(This)	\
    (This)->lpVtbl -> NewFolder(This)

#define IShellNameSpace_Synchronize(This)	\
    (This)->lpVtbl -> Synchronize(This)

#define IShellNameSpace_Import(This)	\
    (This)->lpVtbl -> Import(This)

#define IShellNameSpace_Export(This)	\
    (This)->lpVtbl -> Export(This)

#define IShellNameSpace_InvokeContextMenuCommand(This,strCommand)	\
    (This)->lpVtbl -> InvokeContextMenuCommand(This,strCommand)

#define IShellNameSpace_MoveSelectionTo(This)	\
    (This)->lpVtbl -> MoveSelectionTo(This)

#define IShellNameSpace_get_SubscriptionsEnabled(This,pBool)	\
    (This)->lpVtbl -> get_SubscriptionsEnabled(This,pBool)

#define IShellNameSpace_CreateSubscriptionForSelection(This,pBool)	\
    (This)->lpVtbl -> CreateSubscriptionForSelection(This,pBool)

#define IShellNameSpace_DeleteSubscriptionForSelection(This,pBool)	\
    (This)->lpVtbl -> DeleteSubscriptionForSelection(This,pBool)

#define IShellNameSpace_SetRoot(This,bstrFullPath)	\
    (This)->lpVtbl -> SetRoot(This,bstrFullPath)


#define IShellNameSpace_get_EnumOptions(This,pgrfEnumFlags)	\
    (This)->lpVtbl -> get_EnumOptions(This,pgrfEnumFlags)

#define IShellNameSpace_put_EnumOptions(This,lVal)	\
    (This)->lpVtbl -> put_EnumOptions(This,lVal)

#define IShellNameSpace_get_SelectedItem(This,pItem)	\
    (This)->lpVtbl -> get_SelectedItem(This,pItem)

#define IShellNameSpace_put_SelectedItem(This,pItem)	\
    (This)->lpVtbl -> put_SelectedItem(This,pItem)

#define IShellNameSpace_get_Root(This,pvar)	\
    (This)->lpVtbl -> get_Root(This,pvar)

#define IShellNameSpace_put_Root(This,var)	\
    (This)->lpVtbl -> put_Root(This,var)

#define IShellNameSpace_get_Depth(This,piDepth)	\
    (This)->lpVtbl -> get_Depth(This,piDepth)

#define IShellNameSpace_put_Depth(This,iDepth)	\
    (This)->lpVtbl -> put_Depth(This,iDepth)

#define IShellNameSpace_get_Mode(This,puMode)	\
    (This)->lpVtbl -> get_Mode(This,puMode)

#define IShellNameSpace_put_Mode(This,uMode)	\
    (This)->lpVtbl -> put_Mode(This,uMode)

#define IShellNameSpace_get_Flags(This,pdwFlags)	\
    (This)->lpVtbl -> get_Flags(This,pdwFlags)

#define IShellNameSpace_put_Flags(This,dwFlags)	\
    (This)->lpVtbl -> put_Flags(This,dwFlags)

#define IShellNameSpace_put_TVFlags(This,dwFlags)	\
    (This)->lpVtbl -> put_TVFlags(This,dwFlags)

#define IShellNameSpace_get_TVFlags(This,dwFlags)	\
    (This)->lpVtbl -> get_TVFlags(This,dwFlags)

#define IShellNameSpace_get_Columns(This,bstrColumns)	\
    (This)->lpVtbl -> get_Columns(This,bstrColumns)

#define IShellNameSpace_put_Columns(This,bstrColumns)	\
    (This)->lpVtbl -> put_Columns(This,bstrColumns)

#define IShellNameSpace_get_CountViewTypes(This,piTypes)	\
    (This)->lpVtbl -> get_CountViewTypes(This,piTypes)

#define IShellNameSpace_SetViewType(This,iType)	\
    (This)->lpVtbl -> SetViewType(This,iType)

#define IShellNameSpace_SelectedItems(This,ppid)	\
    (This)->lpVtbl -> SelectedItems(This,ppid)

#define IShellNameSpace_Expand(This,var,iDepth)	\
    (This)->lpVtbl -> Expand(This,var,iDepth)

#define IShellNameSpace_UnselectAll(This)	\
    (This)->lpVtbl -> UnselectAll(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_get_EnumOptions_Proxy( 
    IShellNameSpace * This,
    /* [retval][out] */ LONG *pgrfEnumFlags);


void __RPC_STUB IShellNameSpace_get_EnumOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_put_EnumOptions_Proxy( 
    IShellNameSpace * This,
    /* [in] */ LONG lVal);


void __RPC_STUB IShellNameSpace_put_EnumOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_get_SelectedItem_Proxy( 
    IShellNameSpace * This,
    /* [retval][out] */ IDispatch **pItem);


void __RPC_STUB IShellNameSpace_get_SelectedItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_put_SelectedItem_Proxy( 
    IShellNameSpace * This,
    /* [in] */ IDispatch *pItem);


void __RPC_STUB IShellNameSpace_put_SelectedItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_get_Root_Proxy( 
    IShellNameSpace * This,
    /* [retval][out] */ VARIANT *pvar);


void __RPC_STUB IShellNameSpace_get_Root_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_put_Root_Proxy( 
    IShellNameSpace * This,
    /* [in] */ VARIANT var);


void __RPC_STUB IShellNameSpace_put_Root_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_get_Depth_Proxy( 
    IShellNameSpace * This,
    /* [retval][out] */ int *piDepth);


void __RPC_STUB IShellNameSpace_get_Depth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_put_Depth_Proxy( 
    IShellNameSpace * This,
    /* [in] */ int iDepth);


void __RPC_STUB IShellNameSpace_put_Depth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_get_Mode_Proxy( 
    IShellNameSpace * This,
    /* [retval][out] */ UINT *puMode);


void __RPC_STUB IShellNameSpace_get_Mode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_put_Mode_Proxy( 
    IShellNameSpace * This,
    /* [in] */ UINT uMode);


void __RPC_STUB IShellNameSpace_put_Mode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_get_Flags_Proxy( 
    IShellNameSpace * This,
    /* [retval][out] */ DWORD *pdwFlags);


void __RPC_STUB IShellNameSpace_get_Flags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_put_Flags_Proxy( 
    IShellNameSpace * This,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IShellNameSpace_put_Flags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_put_TVFlags_Proxy( 
    IShellNameSpace * This,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IShellNameSpace_put_TVFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_get_TVFlags_Proxy( 
    IShellNameSpace * This,
    /* [retval][out] */ DWORD *dwFlags);


void __RPC_STUB IShellNameSpace_get_TVFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_get_Columns_Proxy( 
    IShellNameSpace * This,
    /* [retval][out] */ BSTR *bstrColumns);


void __RPC_STUB IShellNameSpace_get_Columns_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_put_Columns_Proxy( 
    IShellNameSpace * This,
    /* [in] */ BSTR bstrColumns);


void __RPC_STUB IShellNameSpace_put_Columns_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_get_CountViewTypes_Proxy( 
    IShellNameSpace * This,
    /* [retval][out] */ int *piTypes);


void __RPC_STUB IShellNameSpace_get_CountViewTypes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_SetViewType_Proxy( 
    IShellNameSpace * This,
    /* [in] */ int iType);


void __RPC_STUB IShellNameSpace_SetViewType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_SelectedItems_Proxy( 
    IShellNameSpace * This,
    /* [retval][out] */ IDispatch **ppid);


void __RPC_STUB IShellNameSpace_SelectedItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_Expand_Proxy( 
    IShellNameSpace * This,
    /* [in] */ VARIANT var,
    int iDepth);


void __RPC_STUB IShellNameSpace_Expand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IShellNameSpace_UnselectAll_Proxy( 
    IShellNameSpace * This);


void __RPC_STUB IShellNameSpace_UnselectAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellNameSpace_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_ShellNameSpace;

#ifdef __cplusplus

class DECLSPEC_UUID("55136805-B2DE-11D1-B9F2-00A0C98BC547")
ShellNameSpace;
#endif

#ifndef __IScriptErrorList_INTERFACE_DEFINED__
#define __IScriptErrorList_INTERFACE_DEFINED__

/* interface IScriptErrorList */
/* [object][dual][hidden][oleautomation][helpstring][uuid] */ 


EXTERN_C const IID IID_IScriptErrorList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F3470F24-15FD-11d2-BB2E-00805FF7EFCA")
    IScriptErrorList : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE advanceError( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE retreatError( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE canAdvanceError( 
            /* [retval][out] */ BOOL *pfCanAdvance) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE canRetreatError( 
            /* [retval][out] */ BOOL *pfCanRetreat) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE getErrorLine( 
            /* [retval][out] */ LONG *plLine) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE getErrorChar( 
            /* [retval][out] */ LONG *plChar) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE getErrorCode( 
            /* [retval][out] */ LONG *plCode) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE getErrorMsg( 
            /* [retval][out] */ BSTR *pstr) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE getErrorUrl( 
            /* [retval][out] */ BSTR *pstr) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE getAlwaysShowLockState( 
            /* [retval][out] */ BOOL *pfAlwaysShowLocked) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE getDetailsPaneOpen( 
            /* [retval][out] */ BOOL *pfDetailsPaneOpen) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE setDetailsPaneOpen( 
            BOOL fDetailsPaneOpen) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE getPerErrorDisplay( 
            /* [retval][out] */ BOOL *pfPerErrorDisplay) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE setPerErrorDisplay( 
            BOOL fPerErrorDisplay) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IScriptErrorListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IScriptErrorList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IScriptErrorList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IScriptErrorList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IScriptErrorList * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IScriptErrorList * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IScriptErrorList * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IScriptErrorList * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *advanceError )( 
            IScriptErrorList * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *retreatError )( 
            IScriptErrorList * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *canAdvanceError )( 
            IScriptErrorList * This,
            /* [retval][out] */ BOOL *pfCanAdvance);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *canRetreatError )( 
            IScriptErrorList * This,
            /* [retval][out] */ BOOL *pfCanRetreat);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *getErrorLine )( 
            IScriptErrorList * This,
            /* [retval][out] */ LONG *plLine);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *getErrorChar )( 
            IScriptErrorList * This,
            /* [retval][out] */ LONG *plChar);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *getErrorCode )( 
            IScriptErrorList * This,
            /* [retval][out] */ LONG *plCode);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *getErrorMsg )( 
            IScriptErrorList * This,
            /* [retval][out] */ BSTR *pstr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *getErrorUrl )( 
            IScriptErrorList * This,
            /* [retval][out] */ BSTR *pstr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *getAlwaysShowLockState )( 
            IScriptErrorList * This,
            /* [retval][out] */ BOOL *pfAlwaysShowLocked);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *getDetailsPaneOpen )( 
            IScriptErrorList * This,
            /* [retval][out] */ BOOL *pfDetailsPaneOpen);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *setDetailsPaneOpen )( 
            IScriptErrorList * This,
            BOOL fDetailsPaneOpen);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *getPerErrorDisplay )( 
            IScriptErrorList * This,
            /* [retval][out] */ BOOL *pfPerErrorDisplay);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *setPerErrorDisplay )( 
            IScriptErrorList * This,
            BOOL fPerErrorDisplay);
        
        END_INTERFACE
    } IScriptErrorListVtbl;

    interface IScriptErrorList
    {
        CONST_VTBL struct IScriptErrorListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IScriptErrorList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IScriptErrorList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IScriptErrorList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IScriptErrorList_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IScriptErrorList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IScriptErrorList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IScriptErrorList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IScriptErrorList_advanceError(This)	\
    (This)->lpVtbl -> advanceError(This)

#define IScriptErrorList_retreatError(This)	\
    (This)->lpVtbl -> retreatError(This)

#define IScriptErrorList_canAdvanceError(This,pfCanAdvance)	\
    (This)->lpVtbl -> canAdvanceError(This,pfCanAdvance)

#define IScriptErrorList_canRetreatError(This,pfCanRetreat)	\
    (This)->lpVtbl -> canRetreatError(This,pfCanRetreat)

#define IScriptErrorList_getErrorLine(This,plLine)	\
    (This)->lpVtbl -> getErrorLine(This,plLine)

#define IScriptErrorList_getErrorChar(This,plChar)	\
    (This)->lpVtbl -> getErrorChar(This,plChar)

#define IScriptErrorList_getErrorCode(This,plCode)	\
    (This)->lpVtbl -> getErrorCode(This,plCode)

#define IScriptErrorList_getErrorMsg(This,pstr)	\
    (This)->lpVtbl -> getErrorMsg(This,pstr)

#define IScriptErrorList_getErrorUrl(This,pstr)	\
    (This)->lpVtbl -> getErrorUrl(This,pstr)

#define IScriptErrorList_getAlwaysShowLockState(This,pfAlwaysShowLocked)	\
    (This)->lpVtbl -> getAlwaysShowLockState(This,pfAlwaysShowLocked)

#define IScriptErrorList_getDetailsPaneOpen(This,pfDetailsPaneOpen)	\
    (This)->lpVtbl -> getDetailsPaneOpen(This,pfDetailsPaneOpen)

#define IScriptErrorList_setDetailsPaneOpen(This,fDetailsPaneOpen)	\
    (This)->lpVtbl -> setDetailsPaneOpen(This,fDetailsPaneOpen)

#define IScriptErrorList_getPerErrorDisplay(This,pfPerErrorDisplay)	\
    (This)->lpVtbl -> getPerErrorDisplay(This,pfPerErrorDisplay)

#define IScriptErrorList_setPerErrorDisplay(This,fPerErrorDisplay)	\
    (This)->lpVtbl -> setPerErrorDisplay(This,fPerErrorDisplay)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE IScriptErrorList_advanceError_Proxy( 
    IScriptErrorList * This);


void __RPC_STUB IScriptErrorList_advanceError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IScriptErrorList_retreatError_Proxy( 
    IScriptErrorList * This);


void __RPC_STUB IScriptErrorList_retreatError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IScriptErrorList_canAdvanceError_Proxy( 
    IScriptErrorList * This,
    /* [retval][out] */ BOOL *pfCanAdvance);


void __RPC_STUB IScriptErrorList_canAdvanceError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IScriptErrorList_canRetreatError_Proxy( 
    IScriptErrorList * This,
    /* [retval][out] */ BOOL *pfCanRetreat);


void __RPC_STUB IScriptErrorList_canRetreatError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IScriptErrorList_getErrorLine_Proxy( 
    IScriptErrorList * This,
    /* [retval][out] */ LONG *plLine);


void __RPC_STUB IScriptErrorList_getErrorLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IScriptErrorList_getErrorChar_Proxy( 
    IScriptErrorList * This,
    /* [retval][out] */ LONG *plChar);


void __RPC_STUB IScriptErrorList_getErrorChar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IScriptErrorList_getErrorCode_Proxy( 
    IScriptErrorList * This,
    /* [retval][out] */ LONG *plCode);


void __RPC_STUB IScriptErrorList_getErrorCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IScriptErrorList_getErrorMsg_Proxy( 
    IScriptErrorList * This,
    /* [retval][out] */ BSTR *pstr);


void __RPC_STUB IScriptErrorList_getErrorMsg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IScriptErrorList_getErrorUrl_Proxy( 
    IScriptErrorList * This,
    /* [retval][out] */ BSTR *pstr);


void __RPC_STUB IScriptErrorList_getErrorUrl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IScriptErrorList_getAlwaysShowLockState_Proxy( 
    IScriptErrorList * This,
    /* [retval][out] */ BOOL *pfAlwaysShowLocked);


void __RPC_STUB IScriptErrorList_getAlwaysShowLockState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IScriptErrorList_getDetailsPaneOpen_Proxy( 
    IScriptErrorList * This,
    /* [retval][out] */ BOOL *pfDetailsPaneOpen);


void __RPC_STUB IScriptErrorList_getDetailsPaneOpen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IScriptErrorList_setDetailsPaneOpen_Proxy( 
    IScriptErrorList * This,
    BOOL fDetailsPaneOpen);


void __RPC_STUB IScriptErrorList_setDetailsPaneOpen_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IScriptErrorList_getPerErrorDisplay_Proxy( 
    IScriptErrorList * This,
    /* [retval][out] */ BOOL *pfPerErrorDisplay);


void __RPC_STUB IScriptErrorList_getPerErrorDisplay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IScriptErrorList_setPerErrorDisplay_Proxy( 
    IScriptErrorList * This,
    BOOL fPerErrorDisplay);


void __RPC_STUB IScriptErrorList_setPerErrorDisplay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IScriptErrorList_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_CScriptErrorList;

#ifdef __cplusplus

class DECLSPEC_UUID("EFD01300-160F-11d2-BB2E-00805FF7EFCA")
CScriptErrorList;
#endif

#ifndef __ISearch_INTERFACE_DEFINED__
#define __ISearch_INTERFACE_DEFINED__

/* interface ISearch */
/* [object][dual][oleautomation][hidden][helpstring][uuid] */ 


EXTERN_C const IID IID_ISearch;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ba9239a4-3dd5-11d2-bf8b-00c04fb93661")
    ISearch : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Title( 
            /* [retval][out] */ BSTR *pbstrTitle) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Id( 
            /* [retval][out] */ BSTR *pbstrId) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Url( 
            /* [retval][out] */ BSTR *pbstrUrl) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISearchVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISearch * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISearch * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISearch * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISearch * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISearch * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISearch * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISearch * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Title )( 
            ISearch * This,
            /* [retval][out] */ BSTR *pbstrTitle);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Id )( 
            ISearch * This,
            /* [retval][out] */ BSTR *pbstrId);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Url )( 
            ISearch * This,
            /* [retval][out] */ BSTR *pbstrUrl);
        
        END_INTERFACE
    } ISearchVtbl;

    interface ISearch
    {
        CONST_VTBL struct ISearchVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISearch_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISearch_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISearch_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISearch_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISearch_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISearch_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISearch_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISearch_get_Title(This,pbstrTitle)	\
    (This)->lpVtbl -> get_Title(This,pbstrTitle)

#define ISearch_get_Id(This,pbstrId)	\
    (This)->lpVtbl -> get_Id(This,pbstrId)

#define ISearch_get_Url(This,pbstrUrl)	\
    (This)->lpVtbl -> get_Url(This,pbstrUrl)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearch_get_Title_Proxy( 
    ISearch * This,
    /* [retval][out] */ BSTR *pbstrTitle);


void __RPC_STUB ISearch_get_Title_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearch_get_Id_Proxy( 
    ISearch * This,
    /* [retval][out] */ BSTR *pbstrId);


void __RPC_STUB ISearch_get_Id_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearch_get_Url_Proxy( 
    ISearch * This,
    /* [retval][out] */ BSTR *pbstrUrl);


void __RPC_STUB ISearch_get_Url_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISearch_INTERFACE_DEFINED__ */


#ifndef __ISearches_INTERFACE_DEFINED__
#define __ISearches_INTERFACE_DEFINED__

/* interface ISearches */
/* [object][dual][oleautomation][hidden][helpstring][uuid] */ 


EXTERN_C const IID IID_ISearches;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("47c922a2-3dd5-11d2-bf8b-00c04fb93661")
    ISearches : public IDispatch
    {
    public:
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *plCount) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Default( 
            /* [retval][out] */ BSTR *pbstrDefault) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [optional][in] */ VARIANT index,
            /* [retval][out] */ ISearch **ppid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE _NewEnum( 
            /* [retval][out] */ IUnknown **ppunk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISearchesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISearches * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISearches * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISearches * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISearches * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISearches * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISearches * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISearches * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            ISearches * This,
            /* [retval][out] */ long *plCount);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Default )( 
            ISearches * This,
            /* [retval][out] */ BSTR *pbstrDefault);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *Item )( 
            ISearches * This,
            /* [optional][in] */ VARIANT index,
            /* [retval][out] */ ISearch **ppid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *_NewEnum )( 
            ISearches * This,
            /* [retval][out] */ IUnknown **ppunk);
        
        END_INTERFACE
    } ISearchesVtbl;

    interface ISearches
    {
        CONST_VTBL struct ISearchesVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISearches_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISearches_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISearches_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISearches_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISearches_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISearches_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISearches_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISearches_get_Count(This,plCount)	\
    (This)->lpVtbl -> get_Count(This,plCount)

#define ISearches_get_Default(This,pbstrDefault)	\
    (This)->lpVtbl -> get_Default(This,pbstrDefault)

#define ISearches_Item(This,index,ppid)	\
    (This)->lpVtbl -> Item(This,index,ppid)

#define ISearches__NewEnum(This,ppunk)	\
    (This)->lpVtbl -> _NewEnum(This,ppunk)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearches_get_Count_Proxy( 
    ISearches * This,
    /* [retval][out] */ long *plCount);


void __RPC_STUB ISearches_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE ISearches_get_Default_Proxy( 
    ISearches * This,
    /* [retval][out] */ BSTR *pbstrDefault);


void __RPC_STUB ISearches_get_Default_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ISearches_Item_Proxy( 
    ISearches * This,
    /* [optional][in] */ VARIANT index,
    /* [retval][out] */ ISearch **ppid);


void __RPC_STUB ISearches_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ISearches__NewEnum_Proxy( 
    ISearches * This,
    /* [retval][out] */ IUnknown **ppunk);


void __RPC_STUB ISearches__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISearches_INTERFACE_DEFINED__ */


#ifndef __ISearchAssistantOC_INTERFACE_DEFINED__
#define __ISearchAssistantOC_INTERFACE_DEFINED__

/* interface ISearchAssistantOC */
/* [unique][helpstring][dual][hidden][uuid][object] */ 


EXTERN_C const IID IID_ISearchAssistantOC;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("72423E8F-8011-11d2-BE79-00A0C9A83DA1")
    ISearchAssistantOC : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE AddNextMenuItem( 
            /* [in] */ BSTR bstrText,
            /* [in] */ long idItem) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE SetDefaultSearchUrl( 
            /* [in] */ BSTR bstrUrl) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE NavigateToDefaultSearch( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE IsRestricted( 
            /* [in] */ BSTR bstrGuid,
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ShellFeaturesEnabled( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SearchAssistantDefault( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Searches( 
            /* [retval][out] */ ISearches **ppid) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_InWebFolder( 
            /* [retval][out] */ VARIANT_BOOL *pVal) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE PutProperty( 
            /* [in] */ VARIANT_BOOL bPerLocale,
            /* [in] */ BSTR bstrName,
            /* [in] */ BSTR bstrValue) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetProperty( 
            /* [in] */ VARIANT_BOOL bPerLocale,
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ BSTR *pbstrValue) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_EventHandled( 
            /* [in] */ VARIANT_BOOL bHandled) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE ResetNextMenu( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE FindOnWeb( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE FindFilesOrFolders( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE FindComputer( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE FindPrinter( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE FindPeople( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE GetSearchAssistantURL( 
            /* [in] */ VARIANT_BOOL bSubstitute,
            /* [in] */ VARIANT_BOOL bCustomize,
            /* [retval][out] */ BSTR *pbstrValue) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE NotifySearchSettingsChanged( void) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ASProvider( 
            /* [in] */ BSTR Provider) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ASProvider( 
            /* [retval][out] */ BSTR *pProvider) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ASSetting( 
            /* [in] */ int Setting) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ASSetting( 
            /* [retval][out] */ int *pSetting) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE NETDetectNextNavigate( void) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE PutFindText( 
            /* [in] */ BSTR FindText) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Version( 
            /* [retval][out] */ int *pVersion) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE EncodeString( 
            /* [in] */ BSTR bstrValue,
            /* [in] */ BSTR bstrCharSet,
            /* [in] */ VARIANT_BOOL bUseUTF8,
            /* [retval][out] */ BSTR *pbstrResult) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISearchAssistantOCVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISearchAssistantOC * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISearchAssistantOC * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISearchAssistantOC * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISearchAssistantOC * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISearchAssistantOC * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISearchAssistantOC * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISearchAssistantOC * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *AddNextMenuItem )( 
            ISearchAssistantOC * This,
            /* [in] */ BSTR bstrText,
            /* [in] */ long idItem);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetDefaultSearchUrl )( 
            ISearchAssistantOC * This,
            /* [in] */ BSTR bstrUrl);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *NavigateToDefaultSearch )( 
            ISearchAssistantOC * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *IsRestricted )( 
            ISearchAssistantOC * This,
            /* [in] */ BSTR bstrGuid,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ShellFeaturesEnabled )( 
            ISearchAssistantOC * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SearchAssistantDefault )( 
            ISearchAssistantOC * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Searches )( 
            ISearchAssistantOC * This,
            /* [retval][out] */ ISearches **ppid);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InWebFolder )( 
            ISearchAssistantOC * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PutProperty )( 
            ISearchAssistantOC * This,
            /* [in] */ VARIANT_BOOL bPerLocale,
            /* [in] */ BSTR bstrName,
            /* [in] */ BSTR bstrValue);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetProperty )( 
            ISearchAssistantOC * This,
            /* [in] */ VARIANT_BOOL bPerLocale,
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ BSTR *pbstrValue);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_EventHandled )( 
            ISearchAssistantOC * This,
            /* [in] */ VARIANT_BOOL bHandled);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ResetNextMenu )( 
            ISearchAssistantOC * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FindOnWeb )( 
            ISearchAssistantOC * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FindFilesOrFolders )( 
            ISearchAssistantOC * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FindComputer )( 
            ISearchAssistantOC * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FindPrinter )( 
            ISearchAssistantOC * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FindPeople )( 
            ISearchAssistantOC * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetSearchAssistantURL )( 
            ISearchAssistantOC * This,
            /* [in] */ VARIANT_BOOL bSubstitute,
            /* [in] */ VARIANT_BOOL bCustomize,
            /* [retval][out] */ BSTR *pbstrValue);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *NotifySearchSettingsChanged )( 
            ISearchAssistantOC * This);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ASProvider )( 
            ISearchAssistantOC * This,
            /* [in] */ BSTR Provider);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ASProvider )( 
            ISearchAssistantOC * This,
            /* [retval][out] */ BSTR *pProvider);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ASSetting )( 
            ISearchAssistantOC * This,
            /* [in] */ int Setting);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ASSetting )( 
            ISearchAssistantOC * This,
            /* [retval][out] */ int *pSetting);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *NETDetectNextNavigate )( 
            ISearchAssistantOC * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PutFindText )( 
            ISearchAssistantOC * This,
            /* [in] */ BSTR FindText);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Version )( 
            ISearchAssistantOC * This,
            /* [retval][out] */ int *pVersion);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *EncodeString )( 
            ISearchAssistantOC * This,
            /* [in] */ BSTR bstrValue,
            /* [in] */ BSTR bstrCharSet,
            /* [in] */ VARIANT_BOOL bUseUTF8,
            /* [retval][out] */ BSTR *pbstrResult);
        
        END_INTERFACE
    } ISearchAssistantOCVtbl;

    interface ISearchAssistantOC
    {
        CONST_VTBL struct ISearchAssistantOCVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISearchAssistantOC_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISearchAssistantOC_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISearchAssistantOC_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISearchAssistantOC_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISearchAssistantOC_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISearchAssistantOC_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISearchAssistantOC_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISearchAssistantOC_AddNextMenuItem(This,bstrText,idItem)	\
    (This)->lpVtbl -> AddNextMenuItem(This,bstrText,idItem)

#define ISearchAssistantOC_SetDefaultSearchUrl(This,bstrUrl)	\
    (This)->lpVtbl -> SetDefaultSearchUrl(This,bstrUrl)

#define ISearchAssistantOC_NavigateToDefaultSearch(This)	\
    (This)->lpVtbl -> NavigateToDefaultSearch(This)

#define ISearchAssistantOC_IsRestricted(This,bstrGuid,pVal)	\
    (This)->lpVtbl -> IsRestricted(This,bstrGuid,pVal)

#define ISearchAssistantOC_get_ShellFeaturesEnabled(This,pVal)	\
    (This)->lpVtbl -> get_ShellFeaturesEnabled(This,pVal)

#define ISearchAssistantOC_get_SearchAssistantDefault(This,pVal)	\
    (This)->lpVtbl -> get_SearchAssistantDefault(This,pVal)

#define ISearchAssistantOC_get_Searches(This,ppid)	\
    (This)->lpVtbl -> get_Searches(This,ppid)

#define ISearchAssistantOC_get_InWebFolder(This,pVal)	\
    (This)->lpVtbl -> get_InWebFolder(This,pVal)

#define ISearchAssistantOC_PutProperty(This,bPerLocale,bstrName,bstrValue)	\
    (This)->lpVtbl -> PutProperty(This,bPerLocale,bstrName,bstrValue)

#define ISearchAssistantOC_GetProperty(This,bPerLocale,bstrName,pbstrValue)	\
    (This)->lpVtbl -> GetProperty(This,bPerLocale,bstrName,pbstrValue)

#define ISearchAssistantOC_put_EventHandled(This,bHandled)	\
    (This)->lpVtbl -> put_EventHandled(This,bHandled)

#define ISearchAssistantOC_ResetNextMenu(This)	\
    (This)->lpVtbl -> ResetNextMenu(This)

#define ISearchAssistantOC_FindOnWeb(This)	\
    (This)->lpVtbl -> FindOnWeb(This)

#define ISearchAssistantOC_FindFilesOrFolders(This)	\
    (This)->lpVtbl -> FindFilesOrFolders(This)

#define ISearchAssistantOC_FindComputer(This)	\
    (This)->lpVtbl -> FindComputer(This)

#define ISearchAssistantOC_FindPrinter(This)	\
    (This)->lpVtbl -> FindPrinter(This)

#define ISearchAssistantOC_FindPeople(This)	\
    (This)->lpVtbl -> FindPeople(This)

#define ISearchAssistantOC_GetSearchAssistantURL(This,bSubstitute,bCustomize,pbstrValue)	\
    (This)->lpVtbl -> GetSearchAssistantURL(This,bSubstitute,bCustomize,pbstrValue)

#define ISearchAssistantOC_NotifySearchSettingsChanged(This)	\
    (This)->lpVtbl -> NotifySearchSettingsChanged(This)

#define ISearchAssistantOC_put_ASProvider(This,Provider)	\
    (This)->lpVtbl -> put_ASProvider(This,Provider)

#define ISearchAssistantOC_get_ASProvider(This,pProvider)	\
    (This)->lpVtbl -> get_ASProvider(This,pProvider)

#define ISearchAssistantOC_put_ASSetting(This,Setting)	\
    (This)->lpVtbl -> put_ASSetting(This,Setting)

#define ISearchAssistantOC_get_ASSetting(This,pSetting)	\
    (This)->lpVtbl -> get_ASSetting(This,pSetting)

#define ISearchAssistantOC_NETDetectNextNavigate(This)	\
    (This)->lpVtbl -> NETDetectNextNavigate(This)

#define ISearchAssistantOC_PutFindText(This,FindText)	\
    (This)->lpVtbl -> PutFindText(This,FindText)

#define ISearchAssistantOC_get_Version(This,pVersion)	\
    (This)->lpVtbl -> get_Version(This,pVersion)

#define ISearchAssistantOC_EncodeString(This,bstrValue,bstrCharSet,bUseUTF8,pbstrResult)	\
    (This)->lpVtbl -> EncodeString(This,bstrValue,bstrCharSet,bUseUTF8,pbstrResult)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_AddNextMenuItem_Proxy( 
    ISearchAssistantOC * This,
    /* [in] */ BSTR bstrText,
    /* [in] */ long idItem);


void __RPC_STUB ISearchAssistantOC_AddNextMenuItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_SetDefaultSearchUrl_Proxy( 
    ISearchAssistantOC * This,
    /* [in] */ BSTR bstrUrl);


void __RPC_STUB ISearchAssistantOC_SetDefaultSearchUrl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_NavigateToDefaultSearch_Proxy( 
    ISearchAssistantOC * This);


void __RPC_STUB ISearchAssistantOC_NavigateToDefaultSearch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_IsRestricted_Proxy( 
    ISearchAssistantOC * This,
    /* [in] */ BSTR bstrGuid,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISearchAssistantOC_IsRestricted_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_get_ShellFeaturesEnabled_Proxy( 
    ISearchAssistantOC * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISearchAssistantOC_get_ShellFeaturesEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_get_SearchAssistantDefault_Proxy( 
    ISearchAssistantOC * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISearchAssistantOC_get_SearchAssistantDefault_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_get_Searches_Proxy( 
    ISearchAssistantOC * This,
    /* [retval][out] */ ISearches **ppid);


void __RPC_STUB ISearchAssistantOC_get_Searches_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_get_InWebFolder_Proxy( 
    ISearchAssistantOC * This,
    /* [retval][out] */ VARIANT_BOOL *pVal);


void __RPC_STUB ISearchAssistantOC_get_InWebFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_PutProperty_Proxy( 
    ISearchAssistantOC * This,
    /* [in] */ VARIANT_BOOL bPerLocale,
    /* [in] */ BSTR bstrName,
    /* [in] */ BSTR bstrValue);


void __RPC_STUB ISearchAssistantOC_PutProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_GetProperty_Proxy( 
    ISearchAssistantOC * This,
    /* [in] */ VARIANT_BOOL bPerLocale,
    /* [in] */ BSTR bstrName,
    /* [retval][out] */ BSTR *pbstrValue);


void __RPC_STUB ISearchAssistantOC_GetProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_put_EventHandled_Proxy( 
    ISearchAssistantOC * This,
    /* [in] */ VARIANT_BOOL bHandled);


void __RPC_STUB ISearchAssistantOC_put_EventHandled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_ResetNextMenu_Proxy( 
    ISearchAssistantOC * This);


void __RPC_STUB ISearchAssistantOC_ResetNextMenu_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_FindOnWeb_Proxy( 
    ISearchAssistantOC * This);


void __RPC_STUB ISearchAssistantOC_FindOnWeb_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_FindFilesOrFolders_Proxy( 
    ISearchAssistantOC * This);


void __RPC_STUB ISearchAssistantOC_FindFilesOrFolders_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_FindComputer_Proxy( 
    ISearchAssistantOC * This);


void __RPC_STUB ISearchAssistantOC_FindComputer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_FindPrinter_Proxy( 
    ISearchAssistantOC * This);


void __RPC_STUB ISearchAssistantOC_FindPrinter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_FindPeople_Proxy( 
    ISearchAssistantOC * This);


void __RPC_STUB ISearchAssistantOC_FindPeople_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_GetSearchAssistantURL_Proxy( 
    ISearchAssistantOC * This,
    /* [in] */ VARIANT_BOOL bSubstitute,
    /* [in] */ VARIANT_BOOL bCustomize,
    /* [retval][out] */ BSTR *pbstrValue);


void __RPC_STUB ISearchAssistantOC_GetSearchAssistantURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_NotifySearchSettingsChanged_Proxy( 
    ISearchAssistantOC * This);


void __RPC_STUB ISearchAssistantOC_NotifySearchSettingsChanged_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_put_ASProvider_Proxy( 
    ISearchAssistantOC * This,
    /* [in] */ BSTR Provider);


void __RPC_STUB ISearchAssistantOC_put_ASProvider_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_get_ASProvider_Proxy( 
    ISearchAssistantOC * This,
    /* [retval][out] */ BSTR *pProvider);


void __RPC_STUB ISearchAssistantOC_get_ASProvider_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_put_ASSetting_Proxy( 
    ISearchAssistantOC * This,
    /* [in] */ int Setting);


void __RPC_STUB ISearchAssistantOC_put_ASSetting_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_get_ASSetting_Proxy( 
    ISearchAssistantOC * This,
    /* [retval][out] */ int *pSetting);


void __RPC_STUB ISearchAssistantOC_get_ASSetting_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_NETDetectNextNavigate_Proxy( 
    ISearchAssistantOC * This);


void __RPC_STUB ISearchAssistantOC_NETDetectNextNavigate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_PutFindText_Proxy( 
    ISearchAssistantOC * This,
    /* [in] */ BSTR FindText);


void __RPC_STUB ISearchAssistantOC_PutFindText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_get_Version_Proxy( 
    ISearchAssistantOC * This,
    /* [retval][out] */ int *pVersion);


void __RPC_STUB ISearchAssistantOC_get_Version_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC_EncodeString_Proxy( 
    ISearchAssistantOC * This,
    /* [in] */ BSTR bstrValue,
    /* [in] */ BSTR bstrCharSet,
    /* [in] */ VARIANT_BOOL bUseUTF8,
    /* [retval][out] */ BSTR *pbstrResult);


void __RPC_STUB ISearchAssistantOC_EncodeString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISearchAssistantOC_INTERFACE_DEFINED__ */


#ifndef __ISearchAssistantOC2_INTERFACE_DEFINED__
#define __ISearchAssistantOC2_INTERFACE_DEFINED__

/* interface ISearchAssistantOC2 */
/* [unique][helpstring][dual][hidden][uuid][object] */ 


EXTERN_C const IID IID_ISearchAssistantOC2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("72423E8F-8011-11d2-BE79-00A0C9A83DA2")
    ISearchAssistantOC2 : public ISearchAssistantOC
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ShowFindPrinter( 
            /* [retval][out] */ VARIANT_BOOL *pbShowFindPrinter) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISearchAssistantOC2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISearchAssistantOC2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISearchAssistantOC2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISearchAssistantOC2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISearchAssistantOC2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISearchAssistantOC2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISearchAssistantOC2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISearchAssistantOC2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *AddNextMenuItem )( 
            ISearchAssistantOC2 * This,
            /* [in] */ BSTR bstrText,
            /* [in] */ long idItem);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetDefaultSearchUrl )( 
            ISearchAssistantOC2 * This,
            /* [in] */ BSTR bstrUrl);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *NavigateToDefaultSearch )( 
            ISearchAssistantOC2 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *IsRestricted )( 
            ISearchAssistantOC2 * This,
            /* [in] */ BSTR bstrGuid,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ShellFeaturesEnabled )( 
            ISearchAssistantOC2 * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SearchAssistantDefault )( 
            ISearchAssistantOC2 * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Searches )( 
            ISearchAssistantOC2 * This,
            /* [retval][out] */ ISearches **ppid);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InWebFolder )( 
            ISearchAssistantOC2 * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PutProperty )( 
            ISearchAssistantOC2 * This,
            /* [in] */ VARIANT_BOOL bPerLocale,
            /* [in] */ BSTR bstrName,
            /* [in] */ BSTR bstrValue);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetProperty )( 
            ISearchAssistantOC2 * This,
            /* [in] */ VARIANT_BOOL bPerLocale,
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ BSTR *pbstrValue);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_EventHandled )( 
            ISearchAssistantOC2 * This,
            /* [in] */ VARIANT_BOOL bHandled);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ResetNextMenu )( 
            ISearchAssistantOC2 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FindOnWeb )( 
            ISearchAssistantOC2 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FindFilesOrFolders )( 
            ISearchAssistantOC2 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FindComputer )( 
            ISearchAssistantOC2 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FindPrinter )( 
            ISearchAssistantOC2 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FindPeople )( 
            ISearchAssistantOC2 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetSearchAssistantURL )( 
            ISearchAssistantOC2 * This,
            /* [in] */ VARIANT_BOOL bSubstitute,
            /* [in] */ VARIANT_BOOL bCustomize,
            /* [retval][out] */ BSTR *pbstrValue);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *NotifySearchSettingsChanged )( 
            ISearchAssistantOC2 * This);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ASProvider )( 
            ISearchAssistantOC2 * This,
            /* [in] */ BSTR Provider);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ASProvider )( 
            ISearchAssistantOC2 * This,
            /* [retval][out] */ BSTR *pProvider);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ASSetting )( 
            ISearchAssistantOC2 * This,
            /* [in] */ int Setting);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ASSetting )( 
            ISearchAssistantOC2 * This,
            /* [retval][out] */ int *pSetting);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *NETDetectNextNavigate )( 
            ISearchAssistantOC2 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PutFindText )( 
            ISearchAssistantOC2 * This,
            /* [in] */ BSTR FindText);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Version )( 
            ISearchAssistantOC2 * This,
            /* [retval][out] */ int *pVersion);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *EncodeString )( 
            ISearchAssistantOC2 * This,
            /* [in] */ BSTR bstrValue,
            /* [in] */ BSTR bstrCharSet,
            /* [in] */ VARIANT_BOOL bUseUTF8,
            /* [retval][out] */ BSTR *pbstrResult);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ShowFindPrinter )( 
            ISearchAssistantOC2 * This,
            /* [retval][out] */ VARIANT_BOOL *pbShowFindPrinter);
        
        END_INTERFACE
    } ISearchAssistantOC2Vtbl;

    interface ISearchAssistantOC2
    {
        CONST_VTBL struct ISearchAssistantOC2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISearchAssistantOC2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISearchAssistantOC2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISearchAssistantOC2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISearchAssistantOC2_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISearchAssistantOC2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISearchAssistantOC2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISearchAssistantOC2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISearchAssistantOC2_AddNextMenuItem(This,bstrText,idItem)	\
    (This)->lpVtbl -> AddNextMenuItem(This,bstrText,idItem)

#define ISearchAssistantOC2_SetDefaultSearchUrl(This,bstrUrl)	\
    (This)->lpVtbl -> SetDefaultSearchUrl(This,bstrUrl)

#define ISearchAssistantOC2_NavigateToDefaultSearch(This)	\
    (This)->lpVtbl -> NavigateToDefaultSearch(This)

#define ISearchAssistantOC2_IsRestricted(This,bstrGuid,pVal)	\
    (This)->lpVtbl -> IsRestricted(This,bstrGuid,pVal)

#define ISearchAssistantOC2_get_ShellFeaturesEnabled(This,pVal)	\
    (This)->lpVtbl -> get_ShellFeaturesEnabled(This,pVal)

#define ISearchAssistantOC2_get_SearchAssistantDefault(This,pVal)	\
    (This)->lpVtbl -> get_SearchAssistantDefault(This,pVal)

#define ISearchAssistantOC2_get_Searches(This,ppid)	\
    (This)->lpVtbl -> get_Searches(This,ppid)

#define ISearchAssistantOC2_get_InWebFolder(This,pVal)	\
    (This)->lpVtbl -> get_InWebFolder(This,pVal)

#define ISearchAssistantOC2_PutProperty(This,bPerLocale,bstrName,bstrValue)	\
    (This)->lpVtbl -> PutProperty(This,bPerLocale,bstrName,bstrValue)

#define ISearchAssistantOC2_GetProperty(This,bPerLocale,bstrName,pbstrValue)	\
    (This)->lpVtbl -> GetProperty(This,bPerLocale,bstrName,pbstrValue)

#define ISearchAssistantOC2_put_EventHandled(This,bHandled)	\
    (This)->lpVtbl -> put_EventHandled(This,bHandled)

#define ISearchAssistantOC2_ResetNextMenu(This)	\
    (This)->lpVtbl -> ResetNextMenu(This)

#define ISearchAssistantOC2_FindOnWeb(This)	\
    (This)->lpVtbl -> FindOnWeb(This)

#define ISearchAssistantOC2_FindFilesOrFolders(This)	\
    (This)->lpVtbl -> FindFilesOrFolders(This)

#define ISearchAssistantOC2_FindComputer(This)	\
    (This)->lpVtbl -> FindComputer(This)

#define ISearchAssistantOC2_FindPrinter(This)	\
    (This)->lpVtbl -> FindPrinter(This)

#define ISearchAssistantOC2_FindPeople(This)	\
    (This)->lpVtbl -> FindPeople(This)

#define ISearchAssistantOC2_GetSearchAssistantURL(This,bSubstitute,bCustomize,pbstrValue)	\
    (This)->lpVtbl -> GetSearchAssistantURL(This,bSubstitute,bCustomize,pbstrValue)

#define ISearchAssistantOC2_NotifySearchSettingsChanged(This)	\
    (This)->lpVtbl -> NotifySearchSettingsChanged(This)

#define ISearchAssistantOC2_put_ASProvider(This,Provider)	\
    (This)->lpVtbl -> put_ASProvider(This,Provider)

#define ISearchAssistantOC2_get_ASProvider(This,pProvider)	\
    (This)->lpVtbl -> get_ASProvider(This,pProvider)

#define ISearchAssistantOC2_put_ASSetting(This,Setting)	\
    (This)->lpVtbl -> put_ASSetting(This,Setting)

#define ISearchAssistantOC2_get_ASSetting(This,pSetting)	\
    (This)->lpVtbl -> get_ASSetting(This,pSetting)

#define ISearchAssistantOC2_NETDetectNextNavigate(This)	\
    (This)->lpVtbl -> NETDetectNextNavigate(This)

#define ISearchAssistantOC2_PutFindText(This,FindText)	\
    (This)->lpVtbl -> PutFindText(This,FindText)

#define ISearchAssistantOC2_get_Version(This,pVersion)	\
    (This)->lpVtbl -> get_Version(This,pVersion)

#define ISearchAssistantOC2_EncodeString(This,bstrValue,bstrCharSet,bUseUTF8,pbstrResult)	\
    (This)->lpVtbl -> EncodeString(This,bstrValue,bstrCharSet,bUseUTF8,pbstrResult)


#define ISearchAssistantOC2_get_ShowFindPrinter(This,pbShowFindPrinter)	\
    (This)->lpVtbl -> get_ShowFindPrinter(This,pbShowFindPrinter)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC2_get_ShowFindPrinter_Proxy( 
    ISearchAssistantOC2 * This,
    /* [retval][out] */ VARIANT_BOOL *pbShowFindPrinter);


void __RPC_STUB ISearchAssistantOC2_get_ShowFindPrinter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISearchAssistantOC2_INTERFACE_DEFINED__ */


#ifndef __ISearchAssistantOC3_INTERFACE_DEFINED__
#define __ISearchAssistantOC3_INTERFACE_DEFINED__

/* interface ISearchAssistantOC3 */
/* [unique][helpstring][dual][hidden][uuid][object] */ 


EXTERN_C const IID IID_ISearchAssistantOC3;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("72423E8F-8011-11d2-BE79-00A0C9A83DA3")
    ISearchAssistantOC3 : public ISearchAssistantOC2
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_SearchCompanionAvailable( 
            /* [retval][out] */ VARIANT_BOOL *pbAvailable) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_UseSearchCompanion( 
            /* [in] */ VARIANT_BOOL bUseSC) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_UseSearchCompanion( 
            /* [retval][out] */ VARIANT_BOOL *pbUseSC) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISearchAssistantOC3Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISearchAssistantOC3 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISearchAssistantOC3 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISearchAssistantOC3 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISearchAssistantOC3 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISearchAssistantOC3 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISearchAssistantOC3 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISearchAssistantOC3 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *AddNextMenuItem )( 
            ISearchAssistantOC3 * This,
            /* [in] */ BSTR bstrText,
            /* [in] */ long idItem);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *SetDefaultSearchUrl )( 
            ISearchAssistantOC3 * This,
            /* [in] */ BSTR bstrUrl);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *NavigateToDefaultSearch )( 
            ISearchAssistantOC3 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *IsRestricted )( 
            ISearchAssistantOC3 * This,
            /* [in] */ BSTR bstrGuid,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ShellFeaturesEnabled )( 
            ISearchAssistantOC3 * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SearchAssistantDefault )( 
            ISearchAssistantOC3 * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Searches )( 
            ISearchAssistantOC3 * This,
            /* [retval][out] */ ISearches **ppid);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_InWebFolder )( 
            ISearchAssistantOC3 * This,
            /* [retval][out] */ VARIANT_BOOL *pVal);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PutProperty )( 
            ISearchAssistantOC3 * This,
            /* [in] */ VARIANT_BOOL bPerLocale,
            /* [in] */ BSTR bstrName,
            /* [in] */ BSTR bstrValue);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetProperty )( 
            ISearchAssistantOC3 * This,
            /* [in] */ VARIANT_BOOL bPerLocale,
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ BSTR *pbstrValue);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_EventHandled )( 
            ISearchAssistantOC3 * This,
            /* [in] */ VARIANT_BOOL bHandled);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *ResetNextMenu )( 
            ISearchAssistantOC3 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FindOnWeb )( 
            ISearchAssistantOC3 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FindFilesOrFolders )( 
            ISearchAssistantOC3 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FindComputer )( 
            ISearchAssistantOC3 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FindPrinter )( 
            ISearchAssistantOC3 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *FindPeople )( 
            ISearchAssistantOC3 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *GetSearchAssistantURL )( 
            ISearchAssistantOC3 * This,
            /* [in] */ VARIANT_BOOL bSubstitute,
            /* [in] */ VARIANT_BOOL bCustomize,
            /* [retval][out] */ BSTR *pbstrValue);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *NotifySearchSettingsChanged )( 
            ISearchAssistantOC3 * This);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ASProvider )( 
            ISearchAssistantOC3 * This,
            /* [in] */ BSTR Provider);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ASProvider )( 
            ISearchAssistantOC3 * This,
            /* [retval][out] */ BSTR *pProvider);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ASSetting )( 
            ISearchAssistantOC3 * This,
            /* [in] */ int Setting);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ASSetting )( 
            ISearchAssistantOC3 * This,
            /* [retval][out] */ int *pSetting);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *NETDetectNextNavigate )( 
            ISearchAssistantOC3 * This);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *PutFindText )( 
            ISearchAssistantOC3 * This,
            /* [in] */ BSTR FindText);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Version )( 
            ISearchAssistantOC3 * This,
            /* [retval][out] */ int *pVersion);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *EncodeString )( 
            ISearchAssistantOC3 * This,
            /* [in] */ BSTR bstrValue,
            /* [in] */ BSTR bstrCharSet,
            /* [in] */ VARIANT_BOOL bUseUTF8,
            /* [retval][out] */ BSTR *pbstrResult);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_ShowFindPrinter )( 
            ISearchAssistantOC3 * This,
            /* [retval][out] */ VARIANT_BOOL *pbShowFindPrinter);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_SearchCompanionAvailable )( 
            ISearchAssistantOC3 * This,
            /* [retval][out] */ VARIANT_BOOL *pbAvailable);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_UseSearchCompanion )( 
            ISearchAssistantOC3 * This,
            /* [in] */ VARIANT_BOOL bUseSC);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_UseSearchCompanion )( 
            ISearchAssistantOC3 * This,
            /* [retval][out] */ VARIANT_BOOL *pbUseSC);
        
        END_INTERFACE
    } ISearchAssistantOC3Vtbl;

    interface ISearchAssistantOC3
    {
        CONST_VTBL struct ISearchAssistantOC3Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISearchAssistantOC3_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISearchAssistantOC3_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISearchAssistantOC3_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISearchAssistantOC3_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISearchAssistantOC3_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISearchAssistantOC3_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISearchAssistantOC3_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISearchAssistantOC3_AddNextMenuItem(This,bstrText,idItem)	\
    (This)->lpVtbl -> AddNextMenuItem(This,bstrText,idItem)

#define ISearchAssistantOC3_SetDefaultSearchUrl(This,bstrUrl)	\
    (This)->lpVtbl -> SetDefaultSearchUrl(This,bstrUrl)

#define ISearchAssistantOC3_NavigateToDefaultSearch(This)	\
    (This)->lpVtbl -> NavigateToDefaultSearch(This)

#define ISearchAssistantOC3_IsRestricted(This,bstrGuid,pVal)	\
    (This)->lpVtbl -> IsRestricted(This,bstrGuid,pVal)

#define ISearchAssistantOC3_get_ShellFeaturesEnabled(This,pVal)	\
    (This)->lpVtbl -> get_ShellFeaturesEnabled(This,pVal)

#define ISearchAssistantOC3_get_SearchAssistantDefault(This,pVal)	\
    (This)->lpVtbl -> get_SearchAssistantDefault(This,pVal)

#define ISearchAssistantOC3_get_Searches(This,ppid)	\
    (This)->lpVtbl -> get_Searches(This,ppid)

#define ISearchAssistantOC3_get_InWebFolder(This,pVal)	\
    (This)->lpVtbl -> get_InWebFolder(This,pVal)

#define ISearchAssistantOC3_PutProperty(This,bPerLocale,bstrName,bstrValue)	\
    (This)->lpVtbl -> PutProperty(This,bPerLocale,bstrName,bstrValue)

#define ISearchAssistantOC3_GetProperty(This,bPerLocale,bstrName,pbstrValue)	\
    (This)->lpVtbl -> GetProperty(This,bPerLocale,bstrName,pbstrValue)

#define ISearchAssistantOC3_put_EventHandled(This,bHandled)	\
    (This)->lpVtbl -> put_EventHandled(This,bHandled)

#define ISearchAssistantOC3_ResetNextMenu(This)	\
    (This)->lpVtbl -> ResetNextMenu(This)

#define ISearchAssistantOC3_FindOnWeb(This)	\
    (This)->lpVtbl -> FindOnWeb(This)

#define ISearchAssistantOC3_FindFilesOrFolders(This)	\
    (This)->lpVtbl -> FindFilesOrFolders(This)

#define ISearchAssistantOC3_FindComputer(This)	\
    (This)->lpVtbl -> FindComputer(This)

#define ISearchAssistantOC3_FindPrinter(This)	\
    (This)->lpVtbl -> FindPrinter(This)

#define ISearchAssistantOC3_FindPeople(This)	\
    (This)->lpVtbl -> FindPeople(This)

#define ISearchAssistantOC3_GetSearchAssistantURL(This,bSubstitute,bCustomize,pbstrValue)	\
    (This)->lpVtbl -> GetSearchAssistantURL(This,bSubstitute,bCustomize,pbstrValue)

#define ISearchAssistantOC3_NotifySearchSettingsChanged(This)	\
    (This)->lpVtbl -> NotifySearchSettingsChanged(This)

#define ISearchAssistantOC3_put_ASProvider(This,Provider)	\
    (This)->lpVtbl -> put_ASProvider(This,Provider)

#define ISearchAssistantOC3_get_ASProvider(This,pProvider)	\
    (This)->lpVtbl -> get_ASProvider(This,pProvider)

#define ISearchAssistantOC3_put_ASSetting(This,Setting)	\
    (This)->lpVtbl -> put_ASSetting(This,Setting)

#define ISearchAssistantOC3_get_ASSetting(This,pSetting)	\
    (This)->lpVtbl -> get_ASSetting(This,pSetting)

#define ISearchAssistantOC3_NETDetectNextNavigate(This)	\
    (This)->lpVtbl -> NETDetectNextNavigate(This)

#define ISearchAssistantOC3_PutFindText(This,FindText)	\
    (This)->lpVtbl -> PutFindText(This,FindText)

#define ISearchAssistantOC3_get_Version(This,pVersion)	\
    (This)->lpVtbl -> get_Version(This,pVersion)

#define ISearchAssistantOC3_EncodeString(This,bstrValue,bstrCharSet,bUseUTF8,pbstrResult)	\
    (This)->lpVtbl -> EncodeString(This,bstrValue,bstrCharSet,bUseUTF8,pbstrResult)


#define ISearchAssistantOC3_get_ShowFindPrinter(This,pbShowFindPrinter)	\
    (This)->lpVtbl -> get_ShowFindPrinter(This,pbShowFindPrinter)


#define ISearchAssistantOC3_get_SearchCompanionAvailable(This,pbAvailable)	\
    (This)->lpVtbl -> get_SearchCompanionAvailable(This,pbAvailable)

#define ISearchAssistantOC3_put_UseSearchCompanion(This,bUseSC)	\
    (This)->lpVtbl -> put_UseSearchCompanion(This,bUseSC)

#define ISearchAssistantOC3_get_UseSearchCompanion(This,pbUseSC)	\
    (This)->lpVtbl -> get_UseSearchCompanion(This,pbUseSC)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC3_get_SearchCompanionAvailable_Proxy( 
    ISearchAssistantOC3 * This,
    /* [retval][out] */ VARIANT_BOOL *pbAvailable);


void __RPC_STUB ISearchAssistantOC3_get_SearchCompanionAvailable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC3_put_UseSearchCompanion_Proxy( 
    ISearchAssistantOC3 * This,
    /* [in] */ VARIANT_BOOL bUseSC);


void __RPC_STUB ISearchAssistantOC3_put_UseSearchCompanion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE ISearchAssistantOC3_get_UseSearchCompanion_Proxy( 
    ISearchAssistantOC3 * This,
    /* [retval][out] */ VARIANT_BOOL *pbUseSC);


void __RPC_STUB ISearchAssistantOC3_get_UseSearchCompanion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISearchAssistantOC3_INTERFACE_DEFINED__ */


#ifndef ___SearchAssistantEvents_DISPINTERFACE_DEFINED__
#define ___SearchAssistantEvents_DISPINTERFACE_DEFINED__

/* dispinterface _SearchAssistantEvents */
/* [hidden][uuid] */ 


EXTERN_C const IID DIID__SearchAssistantEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("1611FDDA-445B-11d2-85DE-00C04FA35C89")
    _SearchAssistantEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _SearchAssistantEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _SearchAssistantEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _SearchAssistantEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _SearchAssistantEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _SearchAssistantEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _SearchAssistantEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _SearchAssistantEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _SearchAssistantEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _SearchAssistantEventsVtbl;

    interface _SearchAssistantEvents
    {
        CONST_VTBL struct _SearchAssistantEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _SearchAssistantEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define _SearchAssistantEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define _SearchAssistantEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define _SearchAssistantEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define _SearchAssistantEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define _SearchAssistantEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define _SearchAssistantEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___SearchAssistantEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_SearchAssistantOC;

#ifdef __cplusplus

class DECLSPEC_UUID("B45FF030-4447-11D2-85DE-00C04FA35C89")
SearchAssistantOC;
#endif
#endif /* __SHDocVw_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



