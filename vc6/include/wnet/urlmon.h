

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* Compiler settings for urlmon.idl:
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

#ifndef __urlmon_h__
#define __urlmon_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IPersistMoniker_FWD_DEFINED__
#define __IPersistMoniker_FWD_DEFINED__
typedef interface IPersistMoniker IPersistMoniker;
#endif 	/* __IPersistMoniker_FWD_DEFINED__ */


#ifndef __IMonikerProp_FWD_DEFINED__
#define __IMonikerProp_FWD_DEFINED__
typedef interface IMonikerProp IMonikerProp;
#endif 	/* __IMonikerProp_FWD_DEFINED__ */


#ifndef __IBindProtocol_FWD_DEFINED__
#define __IBindProtocol_FWD_DEFINED__
typedef interface IBindProtocol IBindProtocol;
#endif 	/* __IBindProtocol_FWD_DEFINED__ */


#ifndef __IBinding_FWD_DEFINED__
#define __IBinding_FWD_DEFINED__
typedef interface IBinding IBinding;
#endif 	/* __IBinding_FWD_DEFINED__ */


#ifndef __IBindStatusCallback_FWD_DEFINED__
#define __IBindStatusCallback_FWD_DEFINED__
typedef interface IBindStatusCallback IBindStatusCallback;
#endif 	/* __IBindStatusCallback_FWD_DEFINED__ */


#ifndef __IAuthenticate_FWD_DEFINED__
#define __IAuthenticate_FWD_DEFINED__
typedef interface IAuthenticate IAuthenticate;
#endif 	/* __IAuthenticate_FWD_DEFINED__ */


#ifndef __IHttpNegotiate_FWD_DEFINED__
#define __IHttpNegotiate_FWD_DEFINED__
typedef interface IHttpNegotiate IHttpNegotiate;
#endif 	/* __IHttpNegotiate_FWD_DEFINED__ */


#ifndef __IHttpNegotiate2_FWD_DEFINED__
#define __IHttpNegotiate2_FWD_DEFINED__
typedef interface IHttpNegotiate2 IHttpNegotiate2;
#endif 	/* __IHttpNegotiate2_FWD_DEFINED__ */


#ifndef __IWinInetFileStream_FWD_DEFINED__
#define __IWinInetFileStream_FWD_DEFINED__
typedef interface IWinInetFileStream IWinInetFileStream;
#endif 	/* __IWinInetFileStream_FWD_DEFINED__ */


#ifndef __IWindowForBindingUI_FWD_DEFINED__
#define __IWindowForBindingUI_FWD_DEFINED__
typedef interface IWindowForBindingUI IWindowForBindingUI;
#endif 	/* __IWindowForBindingUI_FWD_DEFINED__ */


#ifndef __ICodeInstall_FWD_DEFINED__
#define __ICodeInstall_FWD_DEFINED__
typedef interface ICodeInstall ICodeInstall;
#endif 	/* __ICodeInstall_FWD_DEFINED__ */


#ifndef __IWinInetInfo_FWD_DEFINED__
#define __IWinInetInfo_FWD_DEFINED__
typedef interface IWinInetInfo IWinInetInfo;
#endif 	/* __IWinInetInfo_FWD_DEFINED__ */


#ifndef __IHttpSecurity_FWD_DEFINED__
#define __IHttpSecurity_FWD_DEFINED__
typedef interface IHttpSecurity IHttpSecurity;
#endif 	/* __IHttpSecurity_FWD_DEFINED__ */


#ifndef __IWinInetHttpInfo_FWD_DEFINED__
#define __IWinInetHttpInfo_FWD_DEFINED__
typedef interface IWinInetHttpInfo IWinInetHttpInfo;
#endif 	/* __IWinInetHttpInfo_FWD_DEFINED__ */


#ifndef __IWinInetCacheHints_FWD_DEFINED__
#define __IWinInetCacheHints_FWD_DEFINED__
typedef interface IWinInetCacheHints IWinInetCacheHints;
#endif 	/* __IWinInetCacheHints_FWD_DEFINED__ */


#ifndef __IBindHost_FWD_DEFINED__
#define __IBindHost_FWD_DEFINED__
typedef interface IBindHost IBindHost;
#endif 	/* __IBindHost_FWD_DEFINED__ */


#ifndef __IInternet_FWD_DEFINED__
#define __IInternet_FWD_DEFINED__
typedef interface IInternet IInternet;
#endif 	/* __IInternet_FWD_DEFINED__ */


#ifndef __IInternetBindInfo_FWD_DEFINED__
#define __IInternetBindInfo_FWD_DEFINED__
typedef interface IInternetBindInfo IInternetBindInfo;
#endif 	/* __IInternetBindInfo_FWD_DEFINED__ */


#ifndef __IInternetProtocolRoot_FWD_DEFINED__
#define __IInternetProtocolRoot_FWD_DEFINED__
typedef interface IInternetProtocolRoot IInternetProtocolRoot;
#endif 	/* __IInternetProtocolRoot_FWD_DEFINED__ */


#ifndef __IInternetProtocol_FWD_DEFINED__
#define __IInternetProtocol_FWD_DEFINED__
typedef interface IInternetProtocol IInternetProtocol;
#endif 	/* __IInternetProtocol_FWD_DEFINED__ */


#ifndef __IInternetProtocolSink_FWD_DEFINED__
#define __IInternetProtocolSink_FWD_DEFINED__
typedef interface IInternetProtocolSink IInternetProtocolSink;
#endif 	/* __IInternetProtocolSink_FWD_DEFINED__ */


#ifndef __IInternetProtocolSinkStackable_FWD_DEFINED__
#define __IInternetProtocolSinkStackable_FWD_DEFINED__
typedef interface IInternetProtocolSinkStackable IInternetProtocolSinkStackable;
#endif 	/* __IInternetProtocolSinkStackable_FWD_DEFINED__ */


#ifndef __IInternetSession_FWD_DEFINED__
#define __IInternetSession_FWD_DEFINED__
typedef interface IInternetSession IInternetSession;
#endif 	/* __IInternetSession_FWD_DEFINED__ */


#ifndef __IInternetThreadSwitch_FWD_DEFINED__
#define __IInternetThreadSwitch_FWD_DEFINED__
typedef interface IInternetThreadSwitch IInternetThreadSwitch;
#endif 	/* __IInternetThreadSwitch_FWD_DEFINED__ */


#ifndef __IInternetPriority_FWD_DEFINED__
#define __IInternetPriority_FWD_DEFINED__
typedef interface IInternetPriority IInternetPriority;
#endif 	/* __IInternetPriority_FWD_DEFINED__ */


#ifndef __IInternetProtocolInfo_FWD_DEFINED__
#define __IInternetProtocolInfo_FWD_DEFINED__
typedef interface IInternetProtocolInfo IInternetProtocolInfo;
#endif 	/* __IInternetProtocolInfo_FWD_DEFINED__ */


#ifndef __IInternetSecurityMgrSite_FWD_DEFINED__
#define __IInternetSecurityMgrSite_FWD_DEFINED__
typedef interface IInternetSecurityMgrSite IInternetSecurityMgrSite;
#endif 	/* __IInternetSecurityMgrSite_FWD_DEFINED__ */


#ifndef __IInternetSecurityManager_FWD_DEFINED__
#define __IInternetSecurityManager_FWD_DEFINED__
typedef interface IInternetSecurityManager IInternetSecurityManager;
#endif 	/* __IInternetSecurityManager_FWD_DEFINED__ */


#ifndef __IInternetSecurityManagerEx_FWD_DEFINED__
#define __IInternetSecurityManagerEx_FWD_DEFINED__
typedef interface IInternetSecurityManagerEx IInternetSecurityManagerEx;
#endif 	/* __IInternetSecurityManagerEx_FWD_DEFINED__ */


#ifndef __IZoneIdentifier_FWD_DEFINED__
#define __IZoneIdentifier_FWD_DEFINED__
typedef interface IZoneIdentifier IZoneIdentifier;
#endif 	/* __IZoneIdentifier_FWD_DEFINED__ */


#ifndef __IInternetHostSecurityManager_FWD_DEFINED__
#define __IInternetHostSecurityManager_FWD_DEFINED__
typedef interface IInternetHostSecurityManager IInternetHostSecurityManager;
#endif 	/* __IInternetHostSecurityManager_FWD_DEFINED__ */


#ifndef __IInternetZoneManager_FWD_DEFINED__
#define __IInternetZoneManager_FWD_DEFINED__
typedef interface IInternetZoneManager IInternetZoneManager;
#endif 	/* __IInternetZoneManager_FWD_DEFINED__ */


#ifndef __IInternetZoneManagerEx_FWD_DEFINED__
#define __IInternetZoneManagerEx_FWD_DEFINED__
typedef interface IInternetZoneManagerEx IInternetZoneManagerEx;
#endif 	/* __IInternetZoneManagerEx_FWD_DEFINED__ */


#ifndef __ISoftDistExt_FWD_DEFINED__
#define __ISoftDistExt_FWD_DEFINED__
typedef interface ISoftDistExt ISoftDistExt;
#endif 	/* __ISoftDistExt_FWD_DEFINED__ */


#ifndef __ICatalogFileInfo_FWD_DEFINED__
#define __ICatalogFileInfo_FWD_DEFINED__
typedef interface ICatalogFileInfo ICatalogFileInfo;
#endif 	/* __ICatalogFileInfo_FWD_DEFINED__ */


#ifndef __IDataFilter_FWD_DEFINED__
#define __IDataFilter_FWD_DEFINED__
typedef interface IDataFilter IDataFilter;
#endif 	/* __IDataFilter_FWD_DEFINED__ */


#ifndef __IEncodingFilterFactory_FWD_DEFINED__
#define __IEncodingFilterFactory_FWD_DEFINED__
typedef interface IEncodingFilterFactory IEncodingFilterFactory;
#endif 	/* __IEncodingFilterFactory_FWD_DEFINED__ */


#ifndef __IWrappedProtocol_FWD_DEFINED__
#define __IWrappedProtocol_FWD_DEFINED__
typedef interface IWrappedProtocol IWrappedProtocol;
#endif 	/* __IWrappedProtocol_FWD_DEFINED__ */


/* header files for imported files */
#include "objidl.h"
#include "oleidl.h"
#include "servprov.h"
#include "msxml.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_urlmon_0000 */
/* [local] */ 

//=--------------------------------------------------------------------------=
// UrlMon.h
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
// URL Moniker Interfaces.












// Side-by-Side clsid
EXTERN_C const IID CLSID_SBS_StdURLMoniker;  
EXTERN_C const IID CLSID_SBS_HttpProtocol;   
EXTERN_C const IID CLSID_SBS_FtpProtocol;    
EXTERN_C const IID CLSID_SBS_GopherProtocol; 
EXTERN_C const IID CLSID_SBS_HttpSProtocol;  
EXTERN_C const IID CLSID_SBS_FileProtocol;   
EXTERN_C const IID CLSID_SBS_MkProtocol;     
EXTERN_C const IID CLSID_SBS_UrlMkBindCtx;   
EXTERN_C const IID CLSID_SBS_SoftDistExt;  
EXTERN_C const IID CLSID_SBS_StdEncodingFilterFac; 
EXTERN_C const IID CLSID_SBS_DeCompMimeFilter;     
EXTERN_C const IID CLSID_SBS_CdlProtocol;          
EXTERN_C const IID CLSID_SBS_ClassInstallFilter;   
EXTERN_C const IID CLSID_SBS_InternetSecurityManager;  
EXTERN_C const IID CLSID_SBS_InternetZoneManager;  
// END Side-by-Side clsid
// These are for backwards compatibility with previous URLMON versions
#define BINDF_DONTUSECACHE BINDF_GETNEWESTVERSION
#define BINDF_DONTPUTINCACHE BINDF_NOWRITECACHE
#define BINDF_NOCOPYDATA BINDF_PULLDATA
#define INVALID_P_ROOT_SECURITY_ID ((BYTE*)-1)
#define PI_DOCFILECLSIDLOOKUP PI_CLSIDLOOKUP
EXTERN_C const IID IID_IAsyncMoniker;    
EXTERN_C const IID CLSID_StdURLMoniker;  
EXTERN_C const IID CLSID_HttpProtocol;   
EXTERN_C const IID CLSID_FtpProtocol;    
EXTERN_C const IID CLSID_GopherProtocol; 
EXTERN_C const IID CLSID_HttpSProtocol;  
EXTERN_C const IID CLSID_FileProtocol;   
EXTERN_C const IID CLSID_MkProtocol;     
EXTERN_C const IID CLSID_StdURLProtocol; 
EXTERN_C const IID CLSID_UrlMkBindCtx;   
EXTERN_C const IID CLSID_StdEncodingFilterFac; 
EXTERN_C const IID CLSID_DeCompMimeFilter;     
EXTERN_C const IID CLSID_CdlProtocol;          
EXTERN_C const IID CLSID_ClassInstallFilter;   
EXTERN_C const IID IID_IAsyncBindCtx;    
 
#define SZ_URLCONTEXT           OLESTR("URL Context")
#define SZ_ASYNC_CALLEE         OLESTR("AsyncCallee")
#define MKSYS_URLMONIKER         6            
#define URL_MK_LEGACY            0            
#define URL_MK_UNIFORM           1            
#define URL_MK_NO_CANONICALIZE   2            
 
STDAPI CreateURLMoniker(LPMONIKER pMkCtx, LPCWSTR szURL, LPMONIKER FAR * ppmk);             
STDAPI CreateURLMonikerEx(LPMONIKER pMkCtx, LPCWSTR szURL, LPMONIKER FAR * ppmk, DWORD dwFlags);             
STDAPI GetClassURL(LPCWSTR szURL, CLSID *pClsID);                                           
STDAPI CreateAsyncBindCtx(DWORD reserved, IBindStatusCallback *pBSCb,                       
                                IEnumFORMATETC *pEFetc, IBindCtx **ppBC);                   
STDAPI CreateAsyncBindCtxEx(IBindCtx *pbc, DWORD dwOptions, IBindStatusCallback *pBSCb, IEnumFORMATETC *pEnum,   
                            IBindCtx **ppBC, DWORD reserved);                                                     
STDAPI MkParseDisplayNameEx(IBindCtx *pbc, LPCWSTR szDisplayName, ULONG *pchEaten,          
                                LPMONIKER *ppmk);                                           
STDAPI RegisterBindStatusCallback(LPBC pBC, IBindStatusCallback *pBSCb,                     
                                IBindStatusCallback**  ppBSCBPrev, DWORD dwReserved);       
STDAPI RevokeBindStatusCallback(LPBC pBC, IBindStatusCallback *pBSCb);                      
STDAPI GetClassFileOrMime(LPBC pBC, LPCWSTR szFilename, LPVOID pBuffer, DWORD cbSize, LPCWSTR szMime, DWORD dwReserved, CLSID *pclsid); 
STDAPI IsValidURL(LPBC pBC, LPCWSTR szURL, DWORD dwReserved);                               
STDAPI CoGetClassObjectFromURL( REFCLSID rCLASSID,
            LPCWSTR szCODE, DWORD dwFileVersionMS, 
            DWORD dwFileVersionLS, LPCWSTR szTYPE,
            LPBINDCTX pBindCtx, DWORD dwClsContext,
            LPVOID pvReserved, REFIID riid, LPVOID * ppv);
STDAPI FaultInIEFeature( HWND hWnd,
            uCLSSPEC *pClassSpec,
            QUERYCONTEXT *pQuery, DWORD dwFlags);                                           
STDAPI GetComponentIDFromCLSSPEC(uCLSSPEC *pClassspec,
             LPSTR * ppszComponentID);                                                      
// flags for FaultInIEFeature
#define FIEF_FLAG_FORCE_JITUI               0x1     // force JIT ui even if
                                                 // previoulsy rejected by 
                                                 // user in this session or
                                                 // marked as Never Ask Again
#define FIEF_FLAG_PEEK                      0x2     // just peek, don't faultin
#define FIEF_FLAG_SKIP_INSTALLED_VERSION_CHECK        0x4     // force JIT without checking local version
 
//helper apis                                                                               
STDAPI IsAsyncMoniker(IMoniker* pmk);                                                       
STDAPI CreateURLBinding(LPCWSTR lpszUrl, IBindCtx *pbc, IBinding **ppBdg);                  
 
STDAPI RegisterMediaTypes(UINT ctypes, const LPCSTR* rgszTypes, CLIPFORMAT* rgcfTypes);            
STDAPI FindMediaType(LPCSTR rgszTypes, CLIPFORMAT* rgcfTypes);                                       
STDAPI CreateFormatEnumerator( UINT cfmtetc, FORMATETC* rgfmtetc, IEnumFORMATETC** ppenumfmtetc); 
STDAPI RegisterFormatEnumerator(LPBC pBC, IEnumFORMATETC *pEFetc, DWORD reserved);          
STDAPI RevokeFormatEnumerator(LPBC pBC, IEnumFORMATETC *pEFetc);                            
STDAPI RegisterMediaTypeClass(LPBC pBC,UINT ctypes, const LPCSTR* rgszTypes, CLSID *rgclsID, DWORD reserved);    
STDAPI FindMediaTypeClass(LPBC pBC, LPCSTR szType, CLSID *pclsID, DWORD reserved);                          
STDAPI UrlMkSetSessionOption(DWORD dwOption, LPVOID pBuffer, DWORD dwBufferLength, DWORD dwReserved);       
STDAPI UrlMkGetSessionOption(DWORD dwOption, LPVOID pBuffer, DWORD dwBufferLength, DWORD *pdwBufferLength, DWORD dwReserved);       
STDAPI FindMimeFromData(                                                                                                                  
                        LPBC pBC,                           // bind context - can be NULL                                                 
                        LPCWSTR pwzUrl,                     // url - can be null                                                          
                        LPVOID pBuffer,                     // buffer with data to sniff - can be null (pwzUrl must be valid)             
                        DWORD cbSize,                       // size of buffer                                                             
                        LPCWSTR pwzMimeProposed,            // proposed mime if - can be null                                             
                        DWORD dwMimeFlags,                  // will be defined                                                            
                        LPWSTR *ppwzMimeOut,                // the suggested mime                                                         
                        DWORD dwReserved);                  // must be 0                                                                  
#define     FMFD_DEFAULT             0x00000000 
#define     FMFD_URLASFILENAME       0x00000001 
#define     FMFD_ENABLEMIMESNIFFING  0x00000002 
#define     FMFD_IGNOREMIMETEXTPLAIN  0x00000004 
STDAPI ObtainUserAgentString(DWORD dwOption, LPSTR pszUAOut, DWORD* cbSize);       
STDAPI CompareSecurityIds(BYTE* pbSecurityId1, DWORD dwLen1, BYTE* pbSecurityId2, DWORD dwLen2, DWORD dwReserved);    
STDAPI CompatFlagsFromClsid(CLSID *pclsid, LPDWORD pdwCompatFlags, LPDWORD pdwMiscStatusFlags);             
 
// URLMON-specific defines for UrlMkSetSessionOption() above
#define URLMON_OPTION_USERAGENT           0x10000001
#define URLMON_OPTION_USERAGENT_REFRESH   0x10000002
#define URLMON_OPTION_URL_ENCODING        0x10000004
#define URLMON_OPTION_USE_BINDSTRINGCREDS 0x10000008
 
#define CF_NULL                 0                                  
#define CFSTR_MIME_NULL         NULL                               
#define CFSTR_MIME_TEXT         (TEXT("text/plain"))             
#define CFSTR_MIME_RICHTEXT     (TEXT("text/richtext"))          
#define CFSTR_MIME_X_BITMAP     (TEXT("image/x-xbitmap"))        
#define CFSTR_MIME_POSTSCRIPT   (TEXT("application/postscript")) 
#define CFSTR_MIME_AIFF         (TEXT("audio/aiff"))             
#define CFSTR_MIME_BASICAUDIO   (TEXT("audio/basic"))            
#define CFSTR_MIME_WAV          (TEXT("audio/wav"))              
#define CFSTR_MIME_X_WAV        (TEXT("audio/x-wav"))            
#define CFSTR_MIME_GIF          (TEXT("image/gif"))              
#define CFSTR_MIME_PJPEG        (TEXT("image/pjpeg"))            
#define CFSTR_MIME_JPEG         (TEXT("image/jpeg"))             
#define CFSTR_MIME_TIFF         (TEXT("image/tiff"))             
#define CFSTR_MIME_X_PNG        (TEXT("image/x-png"))            
#define CFSTR_MIME_BMP          (TEXT("image/bmp"))              
#define CFSTR_MIME_X_ART        (TEXT("image/x-jg"))             
#define CFSTR_MIME_X_EMF        (TEXT("image/x-emf"))            
#define CFSTR_MIME_X_WMF        (TEXT("image/x-wmf"))            
#define CFSTR_MIME_AVI          (TEXT("video/avi"))              
#define CFSTR_MIME_MPEG         (TEXT("video/mpeg"))             
#define CFSTR_MIME_FRACTALS     (TEXT("application/fractals"))   
#define CFSTR_MIME_RAWDATA      (TEXT("application/octet-stream"))
#define CFSTR_MIME_RAWDATASTRM  (TEXT("application/octet-stream"))
#define CFSTR_MIME_PDF          (TEXT("application/pdf"))        
#define CFSTR_MIME_HTA          (TEXT("application/hta"))        
#define CFSTR_MIME_X_AIFF       (TEXT("audio/x-aiff"))           
#define CFSTR_MIME_X_REALAUDIO  (TEXT("audio/x-pn-realaudio"))   
#define CFSTR_MIME_XBM          (TEXT("image/xbm"))              
#define CFSTR_MIME_QUICKTIME    (TEXT("video/quicktime"))        
#define CFSTR_MIME_X_MSVIDEO    (TEXT("video/x-msvideo"))        
#define CFSTR_MIME_X_SGI_MOVIE  (TEXT("video/x-sgi-movie"))      
#define CFSTR_MIME_HTML         (TEXT("text/html"))              
#define CFSTR_MIME_XML          (TEXT("text/xml"))               
 
// MessageId: MK_S_ASYNCHRONOUS                                              
// MessageText: Operation is successful, but will complete asynchronously.   
//                                                                           
#define MK_S_ASYNCHRONOUS    _HRESULT_TYPEDEF_(0x000401E8L)                  
#ifndef S_ASYNCHRONOUS                                                       
#define S_ASYNCHRONOUS       MK_S_ASYNCHRONOUS                               
#endif                                                                       
                                                                             
#ifndef E_PENDING                                                            
#define E_PENDING _HRESULT_TYPEDEF_(0x8000000AL)                             
#endif                                                                       
                                                                             
//                                                                           
//                                                                           
// WinINet and protocol specific errors are mapped to one of the following   
// error which are returned in IBSC::OnStopBinding                           
//                                                                           
//                                                                           
// Note: FACILITY C is split into ranges of 1k                               
// C0000 - C03FF  INET_E_ (URLMON's original hresult)                        
// C0400 - C07FF  INET_E_CLIENT_xxx                                          
// C0800 - C0BFF  INET_E_SERVER_xxx                                          
// C0C00 - C0FFF  INET_E_????                                                
// C1000 - C13FF  INET_E_AGENT_xxx (info delivery agents)                    
#define INET_E_INVALID_URL               _HRESULT_TYPEDEF_(0x800C0002L)      
#define INET_E_NO_SESSION                _HRESULT_TYPEDEF_(0x800C0003L)      
#define INET_E_CANNOT_CONNECT            _HRESULT_TYPEDEF_(0x800C0004L)      
#define INET_E_RESOURCE_NOT_FOUND        _HRESULT_TYPEDEF_(0x800C0005L)      
#define INET_E_OBJECT_NOT_FOUND          _HRESULT_TYPEDEF_(0x800C0006L)      
#define INET_E_DATA_NOT_AVAILABLE        _HRESULT_TYPEDEF_(0x800C0007L)      
#define INET_E_DOWNLOAD_FAILURE          _HRESULT_TYPEDEF_(0x800C0008L)      
#define INET_E_AUTHENTICATION_REQUIRED   _HRESULT_TYPEDEF_(0x800C0009L)      
#define INET_E_NO_VALID_MEDIA            _HRESULT_TYPEDEF_(0x800C000AL)      
#define INET_E_CONNECTION_TIMEOUT        _HRESULT_TYPEDEF_(0x800C000BL)      
#define INET_E_INVALID_REQUEST           _HRESULT_TYPEDEF_(0x800C000CL)      
#define INET_E_UNKNOWN_PROTOCOL          _HRESULT_TYPEDEF_(0x800C000DL)      
#define INET_E_SECURITY_PROBLEM          _HRESULT_TYPEDEF_(0x800C000EL)      
#define INET_E_CANNOT_LOAD_DATA          _HRESULT_TYPEDEF_(0x800C000FL)      
#define INET_E_CANNOT_INSTANTIATE_OBJECT _HRESULT_TYPEDEF_(0x800C0010L)      
#define INET_E_REDIRECT_FAILED           _HRESULT_TYPEDEF_(0x800C0014L)      
#define INET_E_REDIRECT_TO_DIR           _HRESULT_TYPEDEF_(0x800C0015L)      
#define INET_E_CANNOT_LOCK_REQUEST       _HRESULT_TYPEDEF_(0x800C0016L)      
#define INET_E_USE_EXTEND_BINDING        _HRESULT_TYPEDEF_(0x800C0017L)      
#define INET_E_TERMINATED_BIND           _HRESULT_TYPEDEF_(0x800C0018L)      
#define INET_E_ERROR_FIRST               _HRESULT_TYPEDEF_(0x800C0002L)      
#define INET_E_CODE_DOWNLOAD_DECLINED    _HRESULT_TYPEDEF_(0x800C0100L)      
#define INET_E_RESULT_DISPATCHED         _HRESULT_TYPEDEF_(0x800C0200L)      
#define INET_E_CANNOT_REPLACE_SFP_FILE   _HRESULT_TYPEDEF_(0x800C0300L)      
#define INET_E_CODE_INSTALL_SUPPRESSED   _HRESULT_TYPEDEF_(0x800C0400L)      
#define INET_E_ERROR_LAST                INET_E_CANNOT_REPLACE_SFP_FILE
#ifndef _LPPERSISTMONIKER_DEFINED
#define _LPPERSISTMONIKER_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0000_v0_0_s_ifspec;

#ifndef __IPersistMoniker_INTERFACE_DEFINED__
#define __IPersistMoniker_INTERFACE_DEFINED__

/* interface IPersistMoniker */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IPersistMoniker *LPPERSISTMONIKER;


EXTERN_C const IID IID_IPersistMoniker;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9c9-baf9-11ce-8c82-00aa004ba90b")
    IPersistMoniker : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetClassID( 
            /* [out] */ CLSID *pClassID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsDirty( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Load( 
            /* [in] */ BOOL fFullyAvailable,
            /* [in] */ IMoniker *pimkName,
            /* [in] */ LPBC pibc,
            /* [in] */ DWORD grfMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Save( 
            /* [in] */ IMoniker *pimkName,
            /* [in] */ LPBC pbc,
            /* [in] */ BOOL fRemember) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SaveCompleted( 
            /* [in] */ IMoniker *pimkName,
            /* [in] */ LPBC pibc) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurMoniker( 
            /* [out] */ IMoniker **ppimkName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPersistMonikerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPersistMoniker * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPersistMoniker * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPersistMoniker * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassID )( 
            IPersistMoniker * This,
            /* [out] */ CLSID *pClassID);
        
        HRESULT ( STDMETHODCALLTYPE *IsDirty )( 
            IPersistMoniker * This);
        
        HRESULT ( STDMETHODCALLTYPE *Load )( 
            IPersistMoniker * This,
            /* [in] */ BOOL fFullyAvailable,
            /* [in] */ IMoniker *pimkName,
            /* [in] */ LPBC pibc,
            /* [in] */ DWORD grfMode);
        
        HRESULT ( STDMETHODCALLTYPE *Save )( 
            IPersistMoniker * This,
            /* [in] */ IMoniker *pimkName,
            /* [in] */ LPBC pbc,
            /* [in] */ BOOL fRemember);
        
        HRESULT ( STDMETHODCALLTYPE *SaveCompleted )( 
            IPersistMoniker * This,
            /* [in] */ IMoniker *pimkName,
            /* [in] */ LPBC pibc);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurMoniker )( 
            IPersistMoniker * This,
            /* [out] */ IMoniker **ppimkName);
        
        END_INTERFACE
    } IPersistMonikerVtbl;

    interface IPersistMoniker
    {
        CONST_VTBL struct IPersistMonikerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPersistMoniker_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPersistMoniker_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPersistMoniker_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPersistMoniker_GetClassID(This,pClassID)	\
    (This)->lpVtbl -> GetClassID(This,pClassID)

#define IPersistMoniker_IsDirty(This)	\
    (This)->lpVtbl -> IsDirty(This)

#define IPersistMoniker_Load(This,fFullyAvailable,pimkName,pibc,grfMode)	\
    (This)->lpVtbl -> Load(This,fFullyAvailable,pimkName,pibc,grfMode)

#define IPersistMoniker_Save(This,pimkName,pbc,fRemember)	\
    (This)->lpVtbl -> Save(This,pimkName,pbc,fRemember)

#define IPersistMoniker_SaveCompleted(This,pimkName,pibc)	\
    (This)->lpVtbl -> SaveCompleted(This,pimkName,pibc)

#define IPersistMoniker_GetCurMoniker(This,ppimkName)	\
    (This)->lpVtbl -> GetCurMoniker(This,ppimkName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IPersistMoniker_GetClassID_Proxy( 
    IPersistMoniker * This,
    /* [out] */ CLSID *pClassID);


void __RPC_STUB IPersistMoniker_GetClassID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPersistMoniker_IsDirty_Proxy( 
    IPersistMoniker * This);


void __RPC_STUB IPersistMoniker_IsDirty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPersistMoniker_Load_Proxy( 
    IPersistMoniker * This,
    /* [in] */ BOOL fFullyAvailable,
    /* [in] */ IMoniker *pimkName,
    /* [in] */ LPBC pibc,
    /* [in] */ DWORD grfMode);


void __RPC_STUB IPersistMoniker_Load_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPersistMoniker_Save_Proxy( 
    IPersistMoniker * This,
    /* [in] */ IMoniker *pimkName,
    /* [in] */ LPBC pbc,
    /* [in] */ BOOL fRemember);


void __RPC_STUB IPersistMoniker_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPersistMoniker_SaveCompleted_Proxy( 
    IPersistMoniker * This,
    /* [in] */ IMoniker *pimkName,
    /* [in] */ LPBC pibc);


void __RPC_STUB IPersistMoniker_SaveCompleted_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPersistMoniker_GetCurMoniker_Proxy( 
    IPersistMoniker * This,
    /* [out] */ IMoniker **ppimkName);


void __RPC_STUB IPersistMoniker_GetCurMoniker_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPersistMoniker_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0178 */
/* [local] */ 

#endif
#ifndef _LPMONIKERPROP_DEFINED
#define _LPMONIKERPROP_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0178_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0178_v0_0_s_ifspec;

#ifndef __IMonikerProp_INTERFACE_DEFINED__
#define __IMonikerProp_INTERFACE_DEFINED__

/* interface IMonikerProp */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IMonikerProp *LPMONIKERPROP;

typedef /* [public][public] */ 
enum __MIDL_IMonikerProp_0001
    {	MIMETYPEPROP	= 0,
	USE_SRC_URL	= 0x1,
	CLASSIDPROP	= 0x2,
	TRUSTEDDOWNLOADPROP	= 0x3,
	POPUPLEVELPROP	= 0x4
    } 	MONIKERPROPERTY;


EXTERN_C const IID IID_IMonikerProp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("a5ca5f7f-1847-4d87-9c5b-918509f7511d")
    IMonikerProp : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE PutProperty( 
            /* [in] */ MONIKERPROPERTY mkp,
            /* [in] */ LPCWSTR val) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMonikerPropVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMonikerProp * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMonikerProp * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMonikerProp * This);
        
        HRESULT ( STDMETHODCALLTYPE *PutProperty )( 
            IMonikerProp * This,
            /* [in] */ MONIKERPROPERTY mkp,
            /* [in] */ LPCWSTR val);
        
        END_INTERFACE
    } IMonikerPropVtbl;

    interface IMonikerProp
    {
        CONST_VTBL struct IMonikerPropVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMonikerProp_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMonikerProp_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMonikerProp_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMonikerProp_PutProperty(This,mkp,val)	\
    (This)->lpVtbl -> PutProperty(This,mkp,val)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IMonikerProp_PutProperty_Proxy( 
    IMonikerProp * This,
    /* [in] */ MONIKERPROPERTY mkp,
    /* [in] */ LPCWSTR val);


void __RPC_STUB IMonikerProp_PutProperty_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMonikerProp_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0179 */
/* [local] */ 

#endif
#ifndef _LPBINDPROTOCOL_DEFINED
#define _LPBINDPROTOCOL_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0179_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0179_v0_0_s_ifspec;

#ifndef __IBindProtocol_INTERFACE_DEFINED__
#define __IBindProtocol_INTERFACE_DEFINED__

/* interface IBindProtocol */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IBindProtocol *LPBINDPROTOCOL;


EXTERN_C const IID IID_IBindProtocol;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9cd-baf9-11ce-8c82-00aa004ba90b")
    IBindProtocol : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateBinding( 
            /* [in] */ LPCWSTR szUrl,
            /* [in] */ IBindCtx *pbc,
            /* [out] */ IBinding **ppb) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBindProtocolVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBindProtocol * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBindProtocol * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBindProtocol * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateBinding )( 
            IBindProtocol * This,
            /* [in] */ LPCWSTR szUrl,
            /* [in] */ IBindCtx *pbc,
            /* [out] */ IBinding **ppb);
        
        END_INTERFACE
    } IBindProtocolVtbl;

    interface IBindProtocol
    {
        CONST_VTBL struct IBindProtocolVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBindProtocol_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBindProtocol_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBindProtocol_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBindProtocol_CreateBinding(This,szUrl,pbc,ppb)	\
    (This)->lpVtbl -> CreateBinding(This,szUrl,pbc,ppb)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBindProtocol_CreateBinding_Proxy( 
    IBindProtocol * This,
    /* [in] */ LPCWSTR szUrl,
    /* [in] */ IBindCtx *pbc,
    /* [out] */ IBinding **ppb);


void __RPC_STUB IBindProtocol_CreateBinding_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBindProtocol_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0180 */
/* [local] */ 

#endif
#ifndef _LPBINDING_DEFINED
#define _LPBINDING_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0180_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0180_v0_0_s_ifspec;

#ifndef __IBinding_INTERFACE_DEFINED__
#define __IBinding_INTERFACE_DEFINED__

/* interface IBinding */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IBinding *LPBINDING;


EXTERN_C const IID IID_IBinding;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9c0-baf9-11ce-8c82-00aa004ba90b")
    IBinding : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Abort( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Suspend( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Resume( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPriority( 
            /* [in] */ LONG nPriority) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPriority( 
            /* [out] */ LONG *pnPriority) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetBindResult( 
            /* [out] */ CLSID *pclsidProtocol,
            /* [out] */ DWORD *pdwResult,
            /* [out] */ LPOLESTR *pszResult,
            /* [out][in] */ DWORD *pdwReserved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBindingVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBinding * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBinding * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBinding * This);
        
        HRESULT ( STDMETHODCALLTYPE *Abort )( 
            IBinding * This);
        
        HRESULT ( STDMETHODCALLTYPE *Suspend )( 
            IBinding * This);
        
        HRESULT ( STDMETHODCALLTYPE *Resume )( 
            IBinding * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetPriority )( 
            IBinding * This,
            /* [in] */ LONG nPriority);
        
        HRESULT ( STDMETHODCALLTYPE *GetPriority )( 
            IBinding * This,
            /* [out] */ LONG *pnPriority);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *GetBindResult )( 
            IBinding * This,
            /* [out] */ CLSID *pclsidProtocol,
            /* [out] */ DWORD *pdwResult,
            /* [out] */ LPOLESTR *pszResult,
            /* [out][in] */ DWORD *pdwReserved);
        
        END_INTERFACE
    } IBindingVtbl;

    interface IBinding
    {
        CONST_VTBL struct IBindingVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBinding_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBinding_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBinding_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBinding_Abort(This)	\
    (This)->lpVtbl -> Abort(This)

#define IBinding_Suspend(This)	\
    (This)->lpVtbl -> Suspend(This)

#define IBinding_Resume(This)	\
    (This)->lpVtbl -> Resume(This)

#define IBinding_SetPriority(This,nPriority)	\
    (This)->lpVtbl -> SetPriority(This,nPriority)

#define IBinding_GetPriority(This,pnPriority)	\
    (This)->lpVtbl -> GetPriority(This,pnPriority)

#define IBinding_GetBindResult(This,pclsidProtocol,pdwResult,pszResult,pdwReserved)	\
    (This)->lpVtbl -> GetBindResult(This,pclsidProtocol,pdwResult,pszResult,pdwReserved)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBinding_Abort_Proxy( 
    IBinding * This);


void __RPC_STUB IBinding_Abort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBinding_Suspend_Proxy( 
    IBinding * This);


void __RPC_STUB IBinding_Suspend_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBinding_Resume_Proxy( 
    IBinding * This);


void __RPC_STUB IBinding_Resume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBinding_SetPriority_Proxy( 
    IBinding * This,
    /* [in] */ LONG nPriority);


void __RPC_STUB IBinding_SetPriority_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBinding_GetPriority_Proxy( 
    IBinding * This,
    /* [out] */ LONG *pnPriority);


void __RPC_STUB IBinding_GetPriority_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IBinding_RemoteGetBindResult_Proxy( 
    IBinding * This,
    /* [out] */ CLSID *pclsidProtocol,
    /* [out] */ DWORD *pdwResult,
    /* [out] */ LPOLESTR *pszResult,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IBinding_RemoteGetBindResult_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBinding_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0181 */
/* [local] */ 

#endif
#ifndef _LPBINDSTATUSCALLBACK_DEFINED
#define _LPBINDSTATUSCALLBACK_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0181_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0181_v0_0_s_ifspec;

#ifndef __IBindStatusCallback_INTERFACE_DEFINED__
#define __IBindStatusCallback_INTERFACE_DEFINED__

/* interface IBindStatusCallback */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IBindStatusCallback *LPBINDSTATUSCALLBACK;

typedef /* [public] */ 
enum __MIDL_IBindStatusCallback_0001
    {	BINDVERB_GET	= 0,
	BINDVERB_POST	= 0x1,
	BINDVERB_PUT	= 0x2,
	BINDVERB_CUSTOM	= 0x3
    } 	BINDVERB;

typedef /* [public] */ 
enum __MIDL_IBindStatusCallback_0002
    {	BINDINFOF_URLENCODESTGMEDDATA	= 0x1,
	BINDINFOF_URLENCODEDEXTRAINFO	= 0x2
    } 	BINDINFOF;

typedef /* [public] */ 
enum __MIDL_IBindStatusCallback_0003
    {	BINDF_ASYNCHRONOUS	= 0x1,
	BINDF_ASYNCSTORAGE	= 0x2,
	BINDF_NOPROGRESSIVERENDERING	= 0x4,
	BINDF_OFFLINEOPERATION	= 0x8,
	BINDF_GETNEWESTVERSION	= 0x10,
	BINDF_NOWRITECACHE	= 0x20,
	BINDF_NEEDFILE	= 0x40,
	BINDF_PULLDATA	= 0x80,
	BINDF_IGNORESECURITYPROBLEM	= 0x100,
	BINDF_RESYNCHRONIZE	= 0x200,
	BINDF_HYPERLINK	= 0x400,
	BINDF_NO_UI	= 0x800,
	BINDF_SILENTOPERATION	= 0x1000,
	BINDF_PRAGMA_NO_CACHE	= 0x2000,
	BINDF_GETCLASSOBJECT	= 0x4000,
	BINDF_RESERVED_1	= 0x8000,
	BINDF_FREE_THREADED	= 0x10000,
	BINDF_DIRECT_READ	= 0x20000,
	BINDF_FORMS_SUBMIT	= 0x40000,
	BINDF_GETFROMCACHE_IF_NET_FAIL	= 0x80000,
	BINDF_FROMURLMON	= 0x100000,
	BINDF_FWD_BACK	= 0x200000,
	BINDF_PREFERDEFAULTHANDLER	= 0x400000,
	BINDF_ENFORCERESTRICTED	= 0x800000
    } 	BINDF;

typedef /* [public] */ 
enum __MIDL_IBindStatusCallback_0004
    {	URL_ENCODING_NONE	= 0,
	URL_ENCODING_ENABLE_UTF8	= 0x10000000,
	URL_ENCODING_DISABLE_UTF8	= 0x20000000
    } 	URL_ENCODING;

typedef struct _tagBINDINFO
    {
    ULONG cbSize;
    LPWSTR szExtraInfo;
    STGMEDIUM stgmedData;
    DWORD grfBindInfoF;
    DWORD dwBindVerb;
    LPWSTR szCustomVerb;
    DWORD cbstgmedData;
    DWORD dwOptions;
    DWORD dwOptionsFlags;
    DWORD dwCodePage;
    SECURITY_ATTRIBUTES securityAttributes;
    IID iid;
    IUnknown *pUnk;
    DWORD dwReserved;
    } 	BINDINFO;

typedef struct _REMSECURITY_ATTRIBUTES
    {
    DWORD nLength;
    DWORD lpSecurityDescriptor;
    BOOL bInheritHandle;
    } 	REMSECURITY_ATTRIBUTES;

typedef struct _REMSECURITY_ATTRIBUTES *PREMSECURITY_ATTRIBUTES;

typedef struct _REMSECURITY_ATTRIBUTES *LPREMSECURITY_ATTRIBUTES;

typedef struct _tagRemBINDINFO
    {
    ULONG cbSize;
    LPWSTR szExtraInfo;
    DWORD grfBindInfoF;
    DWORD dwBindVerb;
    LPWSTR szCustomVerb;
    DWORD cbstgmedData;
    DWORD dwOptions;
    DWORD dwOptionsFlags;
    DWORD dwCodePage;
    REMSECURITY_ATTRIBUTES securityAttributes;
    IID iid;
    IUnknown *pUnk;
    DWORD dwReserved;
    } 	RemBINDINFO;

typedef struct tagRemFORMATETC
    {
    DWORD cfFormat;
    DWORD ptd;
    DWORD dwAspect;
    LONG lindex;
    DWORD tymed;
    } 	RemFORMATETC;

typedef struct tagRemFORMATETC *LPREMFORMATETC;

typedef /* [public] */ 
enum __MIDL_IBindStatusCallback_0005
    {	BINDINFO_OPTIONS_WININETFLAG	= 0x10000,
	BINDINFO_OPTIONS_ENABLE_UTF8	= 0x20000,
	BINDINFO_OPTIONS_DISABLE_UTF8	= 0x40000,
	BINDINFO_OPTIONS_USE_IE_ENCODING	= 0x80000,
	BINDINFO_OPTIONS_BINDTOOBJECT	= 0x100000,
	BINDINFO_OPTIONS_SECURITYOPTOUT	= 0x200000,
	BINDINFO_OPTIONS_IGNOREMIMETEXTPLAIN	= 0x400000,
	BINDINFO_OPTIONS_USEBINDSTRINGCREDS	= 0x800000,
	BINDINFO_OPTIONS_IGNOREHTTPHTTPSREDIRECTS	= 0x1000000,
	BINDINFO_OPTIONS_SHDOCVW_NAVIGATE	= 0x80000000
    } 	BINDINFO_OPTIONS;

typedef /* [public] */ 
enum __MIDL_IBindStatusCallback_0006
    {	BSCF_FIRSTDATANOTIFICATION	= 0x1,
	BSCF_INTERMEDIATEDATANOTIFICATION	= 0x2,
	BSCF_LASTDATANOTIFICATION	= 0x4,
	BSCF_DATAFULLYAVAILABLE	= 0x8,
	BSCF_AVAILABLEDATASIZEUNKNOWN	= 0x10
    } 	BSCF;

typedef 
enum tagBINDSTATUS
    {	BINDSTATUS_FINDINGRESOURCE	= 1,
	BINDSTATUS_CONNECTING	= BINDSTATUS_FINDINGRESOURCE + 1,
	BINDSTATUS_REDIRECTING	= BINDSTATUS_CONNECTING + 1,
	BINDSTATUS_BEGINDOWNLOADDATA	= BINDSTATUS_REDIRECTING + 1,
	BINDSTATUS_DOWNLOADINGDATA	= BINDSTATUS_BEGINDOWNLOADDATA + 1,
	BINDSTATUS_ENDDOWNLOADDATA	= BINDSTATUS_DOWNLOADINGDATA + 1,
	BINDSTATUS_BEGINDOWNLOADCOMPONENTS	= BINDSTATUS_ENDDOWNLOADDATA + 1,
	BINDSTATUS_INSTALLINGCOMPONENTS	= BINDSTATUS_BEGINDOWNLOADCOMPONENTS + 1,
	BINDSTATUS_ENDDOWNLOADCOMPONENTS	= BINDSTATUS_INSTALLINGCOMPONENTS + 1,
	BINDSTATUS_USINGCACHEDCOPY	= BINDSTATUS_ENDDOWNLOADCOMPONENTS + 1,
	BINDSTATUS_SENDINGREQUEST	= BINDSTATUS_USINGCACHEDCOPY + 1,
	BINDSTATUS_CLASSIDAVAILABLE	= BINDSTATUS_SENDINGREQUEST + 1,
	BINDSTATUS_MIMETYPEAVAILABLE	= BINDSTATUS_CLASSIDAVAILABLE + 1,
	BINDSTATUS_CACHEFILENAMEAVAILABLE	= BINDSTATUS_MIMETYPEAVAILABLE + 1,
	BINDSTATUS_BEGINSYNCOPERATION	= BINDSTATUS_CACHEFILENAMEAVAILABLE + 1,
	BINDSTATUS_ENDSYNCOPERATION	= BINDSTATUS_BEGINSYNCOPERATION + 1,
	BINDSTATUS_BEGINUPLOADDATA	= BINDSTATUS_ENDSYNCOPERATION + 1,
	BINDSTATUS_UPLOADINGDATA	= BINDSTATUS_BEGINUPLOADDATA + 1,
	BINDSTATUS_ENDUPLOADDATA	= BINDSTATUS_UPLOADINGDATA + 1,
	BINDSTATUS_PROTOCOLCLASSID	= BINDSTATUS_ENDUPLOADDATA + 1,
	BINDSTATUS_ENCODING	= BINDSTATUS_PROTOCOLCLASSID + 1,
	BINDSTATUS_VERIFIEDMIMETYPEAVAILABLE	= BINDSTATUS_ENCODING + 1,
	BINDSTATUS_CLASSINSTALLLOCATION	= BINDSTATUS_VERIFIEDMIMETYPEAVAILABLE + 1,
	BINDSTATUS_DECODING	= BINDSTATUS_CLASSINSTALLLOCATION + 1,
	BINDSTATUS_LOADINGMIMEHANDLER	= BINDSTATUS_DECODING + 1,
	BINDSTATUS_CONTENTDISPOSITIONATTACH	= BINDSTATUS_LOADINGMIMEHANDLER + 1,
	BINDSTATUS_FILTERREPORTMIMETYPE	= BINDSTATUS_CONTENTDISPOSITIONATTACH + 1,
	BINDSTATUS_CLSIDCANINSTANTIATE	= BINDSTATUS_FILTERREPORTMIMETYPE + 1,
	BINDSTATUS_IUNKNOWNAVAILABLE	= BINDSTATUS_CLSIDCANINSTANTIATE + 1,
	BINDSTATUS_DIRECTBIND	= BINDSTATUS_IUNKNOWNAVAILABLE + 1,
	BINDSTATUS_RAWMIMETYPE	= BINDSTATUS_DIRECTBIND + 1,
	BINDSTATUS_PROXYDETECTING	= BINDSTATUS_RAWMIMETYPE + 1,
	BINDSTATUS_ACCEPTRANGES	= BINDSTATUS_PROXYDETECTING + 1,
	BINDSTATUS_COOKIE_SENT	= BINDSTATUS_ACCEPTRANGES + 1,
	BINDSTATUS_COMPACT_POLICY_RECEIVED	= BINDSTATUS_COOKIE_SENT + 1,
	BINDSTATUS_COOKIE_SUPPRESSED	= BINDSTATUS_COMPACT_POLICY_RECEIVED + 1,
	BINDSTATUS_COOKIE_STATE_UNKNOWN	= BINDSTATUS_COOKIE_SUPPRESSED + 1,
	BINDSTATUS_COOKIE_STATE_ACCEPT	= BINDSTATUS_COOKIE_STATE_UNKNOWN + 1,
	BINDSTATUS_COOKIE_STATE_REJECT	= BINDSTATUS_COOKIE_STATE_ACCEPT + 1,
	BINDSTATUS_COOKIE_STATE_PROMPT	= BINDSTATUS_COOKIE_STATE_REJECT + 1,
	BINDSTATUS_COOKIE_STATE_LEASH	= BINDSTATUS_COOKIE_STATE_PROMPT + 1,
	BINDSTATUS_COOKIE_STATE_DOWNGRADE	= BINDSTATUS_COOKIE_STATE_LEASH + 1,
	BINDSTATUS_POLICY_HREF	= BINDSTATUS_COOKIE_STATE_DOWNGRADE + 1,
	BINDSTATUS_P3P_HEADER	= BINDSTATUS_POLICY_HREF + 1,
	BINDSTATUS_SESSION_COOKIE_RECEIVED	= BINDSTATUS_P3P_HEADER + 1,
	BINDSTATUS_PERSISTENT_COOKIE_RECEIVED	= BINDSTATUS_SESSION_COOKIE_RECEIVED + 1,
	BINDSTATUS_SESSION_COOKIES_ALLOWED	= BINDSTATUS_PERSISTENT_COOKIE_RECEIVED + 1,
	BINDSTATUS_CACHECONTROL	= BINDSTATUS_SESSION_COOKIES_ALLOWED + 1,
	BINDSTATUS_CONTENTDISPOSITIONFILENAME	= BINDSTATUS_CACHECONTROL + 1,
	BINDSTATUS_MIMETEXTPLAINMISMATCH	= BINDSTATUS_CONTENTDISPOSITIONFILENAME + 1,
	BINDSTATUS_PUBLISHERAVAILABLE	= BINDSTATUS_MIMETEXTPLAINMISMATCH + 1,
	BINDSTATUS_DISPLAYNAMEAVAILABLE	= BINDSTATUS_PUBLISHERAVAILABLE + 1
    } 	BINDSTATUS;


EXTERN_C const IID IID_IBindStatusCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9c1-baf9-11ce-8c82-00aa004ba90b")
    IBindStatusCallback : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnStartBinding( 
            /* [in] */ DWORD dwReserved,
            /* [in] */ IBinding *pib) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPriority( 
            /* [out] */ LONG *pnPriority) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnLowResource( 
            /* [in] */ DWORD reserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnProgress( 
            /* [in] */ ULONG ulProgress,
            /* [in] */ ULONG ulProgressMax,
            /* [in] */ ULONG ulStatusCode,
            /* [in] */ LPCWSTR szStatusText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnStopBinding( 
            /* [in] */ HRESULT hresult,
            /* [unique][in] */ LPCWSTR szError) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetBindInfo( 
            /* [out] */ DWORD *grfBINDF,
            /* [unique][out][in] */ BINDINFO *pbindinfo) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE OnDataAvailable( 
            /* [in] */ DWORD grfBSCF,
            /* [in] */ DWORD dwSize,
            /* [in] */ FORMATETC *pformatetc,
            /* [in] */ STGMEDIUM *pstgmed) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnObjectAvailable( 
            /* [in] */ REFIID riid,
            /* [iid_is][in] */ IUnknown *punk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBindStatusCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBindStatusCallback * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBindStatusCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBindStatusCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *OnStartBinding )( 
            IBindStatusCallback * This,
            /* [in] */ DWORD dwReserved,
            /* [in] */ IBinding *pib);
        
        HRESULT ( STDMETHODCALLTYPE *GetPriority )( 
            IBindStatusCallback * This,
            /* [out] */ LONG *pnPriority);
        
        HRESULT ( STDMETHODCALLTYPE *OnLowResource )( 
            IBindStatusCallback * This,
            /* [in] */ DWORD reserved);
        
        HRESULT ( STDMETHODCALLTYPE *OnProgress )( 
            IBindStatusCallback * This,
            /* [in] */ ULONG ulProgress,
            /* [in] */ ULONG ulProgressMax,
            /* [in] */ ULONG ulStatusCode,
            /* [in] */ LPCWSTR szStatusText);
        
        HRESULT ( STDMETHODCALLTYPE *OnStopBinding )( 
            IBindStatusCallback * This,
            /* [in] */ HRESULT hresult,
            /* [unique][in] */ LPCWSTR szError);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *GetBindInfo )( 
            IBindStatusCallback * This,
            /* [out] */ DWORD *grfBINDF,
            /* [unique][out][in] */ BINDINFO *pbindinfo);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *OnDataAvailable )( 
            IBindStatusCallback * This,
            /* [in] */ DWORD grfBSCF,
            /* [in] */ DWORD dwSize,
            /* [in] */ FORMATETC *pformatetc,
            /* [in] */ STGMEDIUM *pstgmed);
        
        HRESULT ( STDMETHODCALLTYPE *OnObjectAvailable )( 
            IBindStatusCallback * This,
            /* [in] */ REFIID riid,
            /* [iid_is][in] */ IUnknown *punk);
        
        END_INTERFACE
    } IBindStatusCallbackVtbl;

    interface IBindStatusCallback
    {
        CONST_VTBL struct IBindStatusCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBindStatusCallback_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBindStatusCallback_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBindStatusCallback_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBindStatusCallback_OnStartBinding(This,dwReserved,pib)	\
    (This)->lpVtbl -> OnStartBinding(This,dwReserved,pib)

#define IBindStatusCallback_GetPriority(This,pnPriority)	\
    (This)->lpVtbl -> GetPriority(This,pnPriority)

#define IBindStatusCallback_OnLowResource(This,reserved)	\
    (This)->lpVtbl -> OnLowResource(This,reserved)

#define IBindStatusCallback_OnProgress(This,ulProgress,ulProgressMax,ulStatusCode,szStatusText)	\
    (This)->lpVtbl -> OnProgress(This,ulProgress,ulProgressMax,ulStatusCode,szStatusText)

#define IBindStatusCallback_OnStopBinding(This,hresult,szError)	\
    (This)->lpVtbl -> OnStopBinding(This,hresult,szError)

#define IBindStatusCallback_GetBindInfo(This,grfBINDF,pbindinfo)	\
    (This)->lpVtbl -> GetBindInfo(This,grfBINDF,pbindinfo)

#define IBindStatusCallback_OnDataAvailable(This,grfBSCF,dwSize,pformatetc,pstgmed)	\
    (This)->lpVtbl -> OnDataAvailable(This,grfBSCF,dwSize,pformatetc,pstgmed)

#define IBindStatusCallback_OnObjectAvailable(This,riid,punk)	\
    (This)->lpVtbl -> OnObjectAvailable(This,riid,punk)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBindStatusCallback_OnStartBinding_Proxy( 
    IBindStatusCallback * This,
    /* [in] */ DWORD dwReserved,
    /* [in] */ IBinding *pib);


void __RPC_STUB IBindStatusCallback_OnStartBinding_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBindStatusCallback_GetPriority_Proxy( 
    IBindStatusCallback * This,
    /* [out] */ LONG *pnPriority);


void __RPC_STUB IBindStatusCallback_GetPriority_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBindStatusCallback_OnLowResource_Proxy( 
    IBindStatusCallback * This,
    /* [in] */ DWORD reserved);


void __RPC_STUB IBindStatusCallback_OnLowResource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBindStatusCallback_OnProgress_Proxy( 
    IBindStatusCallback * This,
    /* [in] */ ULONG ulProgress,
    /* [in] */ ULONG ulProgressMax,
    /* [in] */ ULONG ulStatusCode,
    /* [in] */ LPCWSTR szStatusText);


void __RPC_STUB IBindStatusCallback_OnProgress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBindStatusCallback_OnStopBinding_Proxy( 
    IBindStatusCallback * This,
    /* [in] */ HRESULT hresult,
    /* [unique][in] */ LPCWSTR szError);


void __RPC_STUB IBindStatusCallback_OnStopBinding_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IBindStatusCallback_RemoteGetBindInfo_Proxy( 
    IBindStatusCallback * This,
    /* [out] */ DWORD *grfBINDF,
    /* [unique][out][in] */ RemBINDINFO *pbindinfo,
    /* [unique][out][in] */ RemSTGMEDIUM *pstgmed);


void __RPC_STUB IBindStatusCallback_RemoteGetBindInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IBindStatusCallback_RemoteOnDataAvailable_Proxy( 
    IBindStatusCallback * This,
    /* [in] */ DWORD grfBSCF,
    /* [in] */ DWORD dwSize,
    /* [in] */ RemFORMATETC *pformatetc,
    /* [in] */ RemSTGMEDIUM *pstgmed);


void __RPC_STUB IBindStatusCallback_RemoteOnDataAvailable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBindStatusCallback_OnObjectAvailable_Proxy( 
    IBindStatusCallback * This,
    /* [in] */ REFIID riid,
    /* [iid_is][in] */ IUnknown *punk);


void __RPC_STUB IBindStatusCallback_OnObjectAvailable_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBindStatusCallback_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0182 */
/* [local] */ 

#endif
#ifndef _LPAUTHENTICATION_DEFINED
#define _LPAUTHENTICATION_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0182_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0182_v0_0_s_ifspec;

#ifndef __IAuthenticate_INTERFACE_DEFINED__
#define __IAuthenticate_INTERFACE_DEFINED__

/* interface IAuthenticate */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IAuthenticate *LPAUTHENTICATION;


EXTERN_C const IID IID_IAuthenticate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9d0-baf9-11ce-8c82-00aa004ba90b")
    IAuthenticate : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Authenticate( 
            /* [out] */ HWND *phwnd,
            /* [out] */ LPWSTR *pszUsername,
            /* [out] */ LPWSTR *pszPassword) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAuthenticateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAuthenticate * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAuthenticate * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAuthenticate * This);
        
        HRESULT ( STDMETHODCALLTYPE *Authenticate )( 
            IAuthenticate * This,
            /* [out] */ HWND *phwnd,
            /* [out] */ LPWSTR *pszUsername,
            /* [out] */ LPWSTR *pszPassword);
        
        END_INTERFACE
    } IAuthenticateVtbl;

    interface IAuthenticate
    {
        CONST_VTBL struct IAuthenticateVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAuthenticate_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAuthenticate_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAuthenticate_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAuthenticate_Authenticate(This,phwnd,pszUsername,pszPassword)	\
    (This)->lpVtbl -> Authenticate(This,phwnd,pszUsername,pszPassword)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IAuthenticate_Authenticate_Proxy( 
    IAuthenticate * This,
    /* [out] */ HWND *phwnd,
    /* [out] */ LPWSTR *pszUsername,
    /* [out] */ LPWSTR *pszPassword);


void __RPC_STUB IAuthenticate_Authenticate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAuthenticate_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0183 */
/* [local] */ 

#endif
#ifndef _LPHTTPNEGOTIATE_DEFINED
#define _LPHTTPNEGOTIATE_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0183_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0183_v0_0_s_ifspec;

#ifndef __IHttpNegotiate_INTERFACE_DEFINED__
#define __IHttpNegotiate_INTERFACE_DEFINED__

/* interface IHttpNegotiate */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IHttpNegotiate *LPHTTPNEGOTIATE;


EXTERN_C const IID IID_IHttpNegotiate;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9d2-baf9-11ce-8c82-00aa004ba90b")
    IHttpNegotiate : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE BeginningTransaction( 
            /* [in] */ LPCWSTR szURL,
            /* [unique][in] */ LPCWSTR szHeaders,
            /* [in] */ DWORD dwReserved,
            /* [out] */ LPWSTR *pszAdditionalHeaders) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnResponse( 
            /* [in] */ DWORD dwResponseCode,
            /* [unique][in] */ LPCWSTR szResponseHeaders,
            /* [unique][in] */ LPCWSTR szRequestHeaders,
            /* [out] */ LPWSTR *pszAdditionalRequestHeaders) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHttpNegotiateVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHttpNegotiate * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHttpNegotiate * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHttpNegotiate * This);
        
        HRESULT ( STDMETHODCALLTYPE *BeginningTransaction )( 
            IHttpNegotiate * This,
            /* [in] */ LPCWSTR szURL,
            /* [unique][in] */ LPCWSTR szHeaders,
            /* [in] */ DWORD dwReserved,
            /* [out] */ LPWSTR *pszAdditionalHeaders);
        
        HRESULT ( STDMETHODCALLTYPE *OnResponse )( 
            IHttpNegotiate * This,
            /* [in] */ DWORD dwResponseCode,
            /* [unique][in] */ LPCWSTR szResponseHeaders,
            /* [unique][in] */ LPCWSTR szRequestHeaders,
            /* [out] */ LPWSTR *pszAdditionalRequestHeaders);
        
        END_INTERFACE
    } IHttpNegotiateVtbl;

    interface IHttpNegotiate
    {
        CONST_VTBL struct IHttpNegotiateVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHttpNegotiate_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IHttpNegotiate_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IHttpNegotiate_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IHttpNegotiate_BeginningTransaction(This,szURL,szHeaders,dwReserved,pszAdditionalHeaders)	\
    (This)->lpVtbl -> BeginningTransaction(This,szURL,szHeaders,dwReserved,pszAdditionalHeaders)

#define IHttpNegotiate_OnResponse(This,dwResponseCode,szResponseHeaders,szRequestHeaders,pszAdditionalRequestHeaders)	\
    (This)->lpVtbl -> OnResponse(This,dwResponseCode,szResponseHeaders,szRequestHeaders,pszAdditionalRequestHeaders)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IHttpNegotiate_BeginningTransaction_Proxy( 
    IHttpNegotiate * This,
    /* [in] */ LPCWSTR szURL,
    /* [unique][in] */ LPCWSTR szHeaders,
    /* [in] */ DWORD dwReserved,
    /* [out] */ LPWSTR *pszAdditionalHeaders);


void __RPC_STUB IHttpNegotiate_BeginningTransaction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHttpNegotiate_OnResponse_Proxy( 
    IHttpNegotiate * This,
    /* [in] */ DWORD dwResponseCode,
    /* [unique][in] */ LPCWSTR szResponseHeaders,
    /* [unique][in] */ LPCWSTR szRequestHeaders,
    /* [out] */ LPWSTR *pszAdditionalRequestHeaders);


void __RPC_STUB IHttpNegotiate_OnResponse_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IHttpNegotiate_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0184 */
/* [local] */ 

#endif
#ifndef _LPHTTPNEGOTIATE2_DEFINED
#define _LPHTTPNEGOTIATE2_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0184_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0184_v0_0_s_ifspec;

#ifndef __IHttpNegotiate2_INTERFACE_DEFINED__
#define __IHttpNegotiate2_INTERFACE_DEFINED__

/* interface IHttpNegotiate2 */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IHttpNegotiate2 *LPHTTPNEGOTIATE2;


EXTERN_C const IID IID_IHttpNegotiate2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4F9F9FCB-E0F4-48eb-B7AB-FA2EA9365CB4")
    IHttpNegotiate2 : public IHttpNegotiate
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetRootSecurityId( 
            /* [size_is][out] */ BYTE *pbSecurityId,
            /* [out][in] */ DWORD *pcbSecurityId,
            /* [in] */ DWORD_PTR dwReserved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHttpNegotiate2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHttpNegotiate2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHttpNegotiate2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHttpNegotiate2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *BeginningTransaction )( 
            IHttpNegotiate2 * This,
            /* [in] */ LPCWSTR szURL,
            /* [unique][in] */ LPCWSTR szHeaders,
            /* [in] */ DWORD dwReserved,
            /* [out] */ LPWSTR *pszAdditionalHeaders);
        
        HRESULT ( STDMETHODCALLTYPE *OnResponse )( 
            IHttpNegotiate2 * This,
            /* [in] */ DWORD dwResponseCode,
            /* [unique][in] */ LPCWSTR szResponseHeaders,
            /* [unique][in] */ LPCWSTR szRequestHeaders,
            /* [out] */ LPWSTR *pszAdditionalRequestHeaders);
        
        HRESULT ( STDMETHODCALLTYPE *GetRootSecurityId )( 
            IHttpNegotiate2 * This,
            /* [size_is][out] */ BYTE *pbSecurityId,
            /* [out][in] */ DWORD *pcbSecurityId,
            /* [in] */ DWORD_PTR dwReserved);
        
        END_INTERFACE
    } IHttpNegotiate2Vtbl;

    interface IHttpNegotiate2
    {
        CONST_VTBL struct IHttpNegotiate2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHttpNegotiate2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IHttpNegotiate2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IHttpNegotiate2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IHttpNegotiate2_BeginningTransaction(This,szURL,szHeaders,dwReserved,pszAdditionalHeaders)	\
    (This)->lpVtbl -> BeginningTransaction(This,szURL,szHeaders,dwReserved,pszAdditionalHeaders)

#define IHttpNegotiate2_OnResponse(This,dwResponseCode,szResponseHeaders,szRequestHeaders,pszAdditionalRequestHeaders)	\
    (This)->lpVtbl -> OnResponse(This,dwResponseCode,szResponseHeaders,szRequestHeaders,pszAdditionalRequestHeaders)


#define IHttpNegotiate2_GetRootSecurityId(This,pbSecurityId,pcbSecurityId,dwReserved)	\
    (This)->lpVtbl -> GetRootSecurityId(This,pbSecurityId,pcbSecurityId,dwReserved)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IHttpNegotiate2_GetRootSecurityId_Proxy( 
    IHttpNegotiate2 * This,
    /* [size_is][out] */ BYTE *pbSecurityId,
    /* [out][in] */ DWORD *pcbSecurityId,
    /* [in] */ DWORD_PTR dwReserved);


void __RPC_STUB IHttpNegotiate2_GetRootSecurityId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IHttpNegotiate2_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0185 */
/* [local] */ 

#endif
#ifndef _LPWININETFILESTREAM_DEFINED
#define _LPWININETFILESTREAM_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0185_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0185_v0_0_s_ifspec;

#ifndef __IWinInetFileStream_INTERFACE_DEFINED__
#define __IWinInetFileStream_INTERFACE_DEFINED__

/* interface IWinInetFileStream */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IWinInetFileStream *LPWININETFILESTREAM;


EXTERN_C const IID IID_IWinInetFileStream;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F134C4B7-B1F8-4e75-B886-74B90943BECB")
    IWinInetFileStream : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetHandleForUnlock( 
            /* [in] */ DWORD_PTR hWinInetLockHandle,
            /* [in] */ DWORD_PTR dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDeleteFile( 
            /* [in] */ DWORD_PTR dwReserved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWinInetFileStreamVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWinInetFileStream * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWinInetFileStream * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWinInetFileStream * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetHandleForUnlock )( 
            IWinInetFileStream * This,
            /* [in] */ DWORD_PTR hWinInetLockHandle,
            /* [in] */ DWORD_PTR dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *SetDeleteFile )( 
            IWinInetFileStream * This,
            /* [in] */ DWORD_PTR dwReserved);
        
        END_INTERFACE
    } IWinInetFileStreamVtbl;

    interface IWinInetFileStream
    {
        CONST_VTBL struct IWinInetFileStreamVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWinInetFileStream_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWinInetFileStream_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWinInetFileStream_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWinInetFileStream_SetHandleForUnlock(This,hWinInetLockHandle,dwReserved)	\
    (This)->lpVtbl -> SetHandleForUnlock(This,hWinInetLockHandle,dwReserved)

#define IWinInetFileStream_SetDeleteFile(This,dwReserved)	\
    (This)->lpVtbl -> SetDeleteFile(This,dwReserved)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWinInetFileStream_SetHandleForUnlock_Proxy( 
    IWinInetFileStream * This,
    /* [in] */ DWORD_PTR hWinInetLockHandle,
    /* [in] */ DWORD_PTR dwReserved);


void __RPC_STUB IWinInetFileStream_SetHandleForUnlock_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWinInetFileStream_SetDeleteFile_Proxy( 
    IWinInetFileStream * This,
    /* [in] */ DWORD_PTR dwReserved);


void __RPC_STUB IWinInetFileStream_SetDeleteFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWinInetFileStream_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0186 */
/* [local] */ 

#endif
#ifndef _LPWINDOWFORBINDINGUI_DEFINED
#define _LPWINDOWFORBINDINGUI_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0186_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0186_v0_0_s_ifspec;

#ifndef __IWindowForBindingUI_INTERFACE_DEFINED__
#define __IWindowForBindingUI_INTERFACE_DEFINED__

/* interface IWindowForBindingUI */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IWindowForBindingUI *LPWINDOWFORBINDINGUI;


EXTERN_C const IID IID_IWindowForBindingUI;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9d5-bafa-11ce-8c82-00aa004ba90b")
    IWindowForBindingUI : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetWindow( 
            /* [in] */ REFGUID rguidReason,
            /* [out] */ HWND *phwnd) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWindowForBindingUIVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWindowForBindingUI * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWindowForBindingUI * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWindowForBindingUI * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IWindowForBindingUI * This,
            /* [in] */ REFGUID rguidReason,
            /* [out] */ HWND *phwnd);
        
        END_INTERFACE
    } IWindowForBindingUIVtbl;

    interface IWindowForBindingUI
    {
        CONST_VTBL struct IWindowForBindingUIVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWindowForBindingUI_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWindowForBindingUI_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWindowForBindingUI_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWindowForBindingUI_GetWindow(This,rguidReason,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,rguidReason,phwnd)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWindowForBindingUI_GetWindow_Proxy( 
    IWindowForBindingUI * This,
    /* [in] */ REFGUID rguidReason,
    /* [out] */ HWND *phwnd);


void __RPC_STUB IWindowForBindingUI_GetWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWindowForBindingUI_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0187 */
/* [local] */ 

#endif
#ifndef _LPCODEINSTALL_DEFINED
#define _LPCODEINSTALL_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0187_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0187_v0_0_s_ifspec;

#ifndef __ICodeInstall_INTERFACE_DEFINED__
#define __ICodeInstall_INTERFACE_DEFINED__

/* interface ICodeInstall */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ ICodeInstall *LPCODEINSTALL;

typedef /* [public] */ 
enum __MIDL_ICodeInstall_0001
    {	CIP_DISK_FULL	= 0,
	CIP_ACCESS_DENIED	= CIP_DISK_FULL + 1,
	CIP_NEWER_VERSION_EXISTS	= CIP_ACCESS_DENIED + 1,
	CIP_OLDER_VERSION_EXISTS	= CIP_NEWER_VERSION_EXISTS + 1,
	CIP_NAME_CONFLICT	= CIP_OLDER_VERSION_EXISTS + 1,
	CIP_TRUST_VERIFICATION_COMPONENT_MISSING	= CIP_NAME_CONFLICT + 1,
	CIP_EXE_SELF_REGISTERATION_TIMEOUT	= CIP_TRUST_VERIFICATION_COMPONENT_MISSING + 1,
	CIP_UNSAFE_TO_ABORT	= CIP_EXE_SELF_REGISTERATION_TIMEOUT + 1,
	CIP_NEED_REBOOT	= CIP_UNSAFE_TO_ABORT + 1,
	CIP_NEED_REBOOT_UI_PERMISSION	= CIP_NEED_REBOOT + 1
    } 	CIP_STATUS;


EXTERN_C const IID IID_ICodeInstall;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9d1-baf9-11ce-8c82-00aa004ba90b")
    ICodeInstall : public IWindowForBindingUI
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnCodeInstallProblem( 
            /* [in] */ ULONG ulStatusCode,
            /* [unique][in] */ LPCWSTR szDestination,
            /* [unique][in] */ LPCWSTR szSource,
            /* [in] */ DWORD dwReserved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICodeInstallVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICodeInstall * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICodeInstall * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICodeInstall * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            ICodeInstall * This,
            /* [in] */ REFGUID rguidReason,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *OnCodeInstallProblem )( 
            ICodeInstall * This,
            /* [in] */ ULONG ulStatusCode,
            /* [unique][in] */ LPCWSTR szDestination,
            /* [unique][in] */ LPCWSTR szSource,
            /* [in] */ DWORD dwReserved);
        
        END_INTERFACE
    } ICodeInstallVtbl;

    interface ICodeInstall
    {
        CONST_VTBL struct ICodeInstallVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICodeInstall_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICodeInstall_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICodeInstall_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICodeInstall_GetWindow(This,rguidReason,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,rguidReason,phwnd)


#define ICodeInstall_OnCodeInstallProblem(This,ulStatusCode,szDestination,szSource,dwReserved)	\
    (This)->lpVtbl -> OnCodeInstallProblem(This,ulStatusCode,szDestination,szSource,dwReserved)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICodeInstall_OnCodeInstallProblem_Proxy( 
    ICodeInstall * This,
    /* [in] */ ULONG ulStatusCode,
    /* [unique][in] */ LPCWSTR szDestination,
    /* [unique][in] */ LPCWSTR szSource,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB ICodeInstall_OnCodeInstallProblem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICodeInstall_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0188 */
/* [local] */ 

#endif
#ifndef _LPWININETINFO_DEFINED
#define _LPWININETINFO_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0188_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0188_v0_0_s_ifspec;

#ifndef __IWinInetInfo_INTERFACE_DEFINED__
#define __IWinInetInfo_INTERFACE_DEFINED__

/* interface IWinInetInfo */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IWinInetInfo *LPWININETINFO;


EXTERN_C const IID IID_IWinInetInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9d6-bafa-11ce-8c82-00aa004ba90b")
    IWinInetInfo : public IUnknown
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE QueryOption( 
            /* [in] */ DWORD dwOption,
            /* [size_is][out][in] */ LPVOID pBuffer,
            /* [out][in] */ DWORD *pcbBuf) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWinInetInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWinInetInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWinInetInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWinInetInfo * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *QueryOption )( 
            IWinInetInfo * This,
            /* [in] */ DWORD dwOption,
            /* [size_is][out][in] */ LPVOID pBuffer,
            /* [out][in] */ DWORD *pcbBuf);
        
        END_INTERFACE
    } IWinInetInfoVtbl;

    interface IWinInetInfo
    {
        CONST_VTBL struct IWinInetInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWinInetInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWinInetInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWinInetInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWinInetInfo_QueryOption(This,dwOption,pBuffer,pcbBuf)	\
    (This)->lpVtbl -> QueryOption(This,dwOption,pBuffer,pcbBuf)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IWinInetInfo_RemoteQueryOption_Proxy( 
    IWinInetInfo * This,
    /* [in] */ DWORD dwOption,
    /* [size_is][out][in] */ BYTE *pBuffer,
    /* [out][in] */ DWORD *pcbBuf);


void __RPC_STUB IWinInetInfo_RemoteQueryOption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWinInetInfo_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0189 */
/* [local] */ 

#endif
#define WININETINFO_OPTION_LOCK_HANDLE 65534
#ifndef _LPHTTPSECURITY_DEFINED
#define _LPHTTPSECURITY_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0189_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0189_v0_0_s_ifspec;

#ifndef __IHttpSecurity_INTERFACE_DEFINED__
#define __IHttpSecurity_INTERFACE_DEFINED__

/* interface IHttpSecurity */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IHttpSecurity *LPHTTPSECURITY;


EXTERN_C const IID IID_IHttpSecurity;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9d7-bafa-11ce-8c82-00aa004ba90b")
    IHttpSecurity : public IWindowForBindingUI
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnSecurityProblem( 
            /* [in] */ DWORD dwProblem) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHttpSecurityVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHttpSecurity * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHttpSecurity * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHttpSecurity * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IHttpSecurity * This,
            /* [in] */ REFGUID rguidReason,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *OnSecurityProblem )( 
            IHttpSecurity * This,
            /* [in] */ DWORD dwProblem);
        
        END_INTERFACE
    } IHttpSecurityVtbl;

    interface IHttpSecurity
    {
        CONST_VTBL struct IHttpSecurityVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHttpSecurity_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IHttpSecurity_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IHttpSecurity_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IHttpSecurity_GetWindow(This,rguidReason,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,rguidReason,phwnd)


#define IHttpSecurity_OnSecurityProblem(This,dwProblem)	\
    (This)->lpVtbl -> OnSecurityProblem(This,dwProblem)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IHttpSecurity_OnSecurityProblem_Proxy( 
    IHttpSecurity * This,
    /* [in] */ DWORD dwProblem);


void __RPC_STUB IHttpSecurity_OnSecurityProblem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IHttpSecurity_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0190 */
/* [local] */ 

#endif
#ifndef _LPWININETHTTPINFO_DEFINED
#define _LPWININETHTTPINFO_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0190_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0190_v0_0_s_ifspec;

#ifndef __IWinInetHttpInfo_INTERFACE_DEFINED__
#define __IWinInetHttpInfo_INTERFACE_DEFINED__

/* interface IWinInetHttpInfo */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IWinInetHttpInfo *LPWININETHTTPINFO;


EXTERN_C const IID IID_IWinInetHttpInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9d8-bafa-11ce-8c82-00aa004ba90b")
    IWinInetHttpInfo : public IWinInetInfo
    {
    public:
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE QueryInfo( 
            /* [in] */ DWORD dwOption,
            /* [size_is][out][in] */ LPVOID pBuffer,
            /* [out][in] */ DWORD *pcbBuf,
            /* [out][in] */ DWORD *pdwFlags,
            /* [out][in] */ DWORD *pdwReserved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWinInetHttpInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWinInetHttpInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWinInetHttpInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWinInetHttpInfo * This);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *QueryOption )( 
            IWinInetHttpInfo * This,
            /* [in] */ DWORD dwOption,
            /* [size_is][out][in] */ LPVOID pBuffer,
            /* [out][in] */ DWORD *pcbBuf);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *QueryInfo )( 
            IWinInetHttpInfo * This,
            /* [in] */ DWORD dwOption,
            /* [size_is][out][in] */ LPVOID pBuffer,
            /* [out][in] */ DWORD *pcbBuf,
            /* [out][in] */ DWORD *pdwFlags,
            /* [out][in] */ DWORD *pdwReserved);
        
        END_INTERFACE
    } IWinInetHttpInfoVtbl;

    interface IWinInetHttpInfo
    {
        CONST_VTBL struct IWinInetHttpInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWinInetHttpInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWinInetHttpInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWinInetHttpInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWinInetHttpInfo_QueryOption(This,dwOption,pBuffer,pcbBuf)	\
    (This)->lpVtbl -> QueryOption(This,dwOption,pBuffer,pcbBuf)


#define IWinInetHttpInfo_QueryInfo(This,dwOption,pBuffer,pcbBuf,pdwFlags,pdwReserved)	\
    (This)->lpVtbl -> QueryInfo(This,dwOption,pBuffer,pcbBuf,pdwFlags,pdwReserved)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [call_as] */ HRESULT STDMETHODCALLTYPE IWinInetHttpInfo_RemoteQueryInfo_Proxy( 
    IWinInetHttpInfo * This,
    /* [in] */ DWORD dwOption,
    /* [size_is][out][in] */ BYTE *pBuffer,
    /* [out][in] */ DWORD *pcbBuf,
    /* [out][in] */ DWORD *pdwFlags,
    /* [out][in] */ DWORD *pdwReserved);


void __RPC_STUB IWinInetHttpInfo_RemoteQueryInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWinInetHttpInfo_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0191 */
/* [local] */ 

#endif
#ifndef _LPWININETCACHEHINTS_DEFINED
#define _LPWININETCACHEHINTS_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0191_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0191_v0_0_s_ifspec;

#ifndef __IWinInetCacheHints_INTERFACE_DEFINED__
#define __IWinInetCacheHints_INTERFACE_DEFINED__

/* interface IWinInetCacheHints */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IWinInetCacheHints *LPWININETCACHEHINTS;


EXTERN_C const IID IID_IWinInetCacheHints;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DD1EC3B3-8391-4fdb-A9E6-347C3CAAA7DD")
    IWinInetCacheHints : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetCacheExtension( 
            /* [in] */ LPCWSTR pwzExt,
            /* [size_is][out][in] */ LPVOID pszCacheFile,
            /* [out][in] */ DWORD *pcbCacheFile,
            /* [out][in] */ DWORD *pdwWinInetError,
            /* [out][in] */ DWORD *pdwReserved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWinInetCacheHintsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWinInetCacheHints * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWinInetCacheHints * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWinInetCacheHints * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetCacheExtension )( 
            IWinInetCacheHints * This,
            /* [in] */ LPCWSTR pwzExt,
            /* [size_is][out][in] */ LPVOID pszCacheFile,
            /* [out][in] */ DWORD *pcbCacheFile,
            /* [out][in] */ DWORD *pdwWinInetError,
            /* [out][in] */ DWORD *pdwReserved);
        
        END_INTERFACE
    } IWinInetCacheHintsVtbl;

    interface IWinInetCacheHints
    {
        CONST_VTBL struct IWinInetCacheHintsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWinInetCacheHints_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWinInetCacheHints_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWinInetCacheHints_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWinInetCacheHints_SetCacheExtension(This,pwzExt,pszCacheFile,pcbCacheFile,pdwWinInetError,pdwReserved)	\
    (This)->lpVtbl -> SetCacheExtension(This,pwzExt,pszCacheFile,pcbCacheFile,pdwWinInetError,pdwReserved)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWinInetCacheHints_SetCacheExtension_Proxy( 
    IWinInetCacheHints * This,
    /* [in] */ LPCWSTR pwzExt,
    /* [size_is][out][in] */ LPVOID pszCacheFile,
    /* [out][in] */ DWORD *pcbCacheFile,
    /* [out][in] */ DWORD *pdwWinInetError,
    /* [out][in] */ DWORD *pdwReserved);


void __RPC_STUB IWinInetCacheHints_SetCacheExtension_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWinInetCacheHints_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0192 */
/* [local] */ 

#endif
#define SID_IBindHost IID_IBindHost
#define SID_SBindHost IID_IBindHost
#ifndef _LPBINDHOST_DEFINED
#define _LPBINDHOST_DEFINED
EXTERN_C const GUID SID_BindHost;


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0192_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0192_v0_0_s_ifspec;

#ifndef __IBindHost_INTERFACE_DEFINED__
#define __IBindHost_INTERFACE_DEFINED__

/* interface IBindHost */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IBindHost *LPBINDHOST;


EXTERN_C const IID IID_IBindHost;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("fc4801a1-2ba9-11cf-a229-00aa003d7352")
    IBindHost : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CreateMoniker( 
            /* [in] */ LPOLESTR szName,
            /* [in] */ IBindCtx *pBC,
            /* [out] */ IMoniker **ppmk,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE MonikerBindToStorage( 
            /* [in] */ IMoniker *pMk,
            /* [in] */ IBindCtx *pBC,
            /* [in] */ IBindStatusCallback *pBSC,
            /* [in] */ REFIID riid,
            /* [out] */ void **ppvObj) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE MonikerBindToObject( 
            /* [in] */ IMoniker *pMk,
            /* [in] */ IBindCtx *pBC,
            /* [in] */ IBindStatusCallback *pBSC,
            /* [in] */ REFIID riid,
            /* [out] */ void **ppvObj) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBindHostVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBindHost * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBindHost * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBindHost * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateMoniker )( 
            IBindHost * This,
            /* [in] */ LPOLESTR szName,
            /* [in] */ IBindCtx *pBC,
            /* [out] */ IMoniker **ppmk,
            /* [in] */ DWORD dwReserved);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *MonikerBindToStorage )( 
            IBindHost * This,
            /* [in] */ IMoniker *pMk,
            /* [in] */ IBindCtx *pBC,
            /* [in] */ IBindStatusCallback *pBSC,
            /* [in] */ REFIID riid,
            /* [out] */ void **ppvObj);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *MonikerBindToObject )( 
            IBindHost * This,
            /* [in] */ IMoniker *pMk,
            /* [in] */ IBindCtx *pBC,
            /* [in] */ IBindStatusCallback *pBSC,
            /* [in] */ REFIID riid,
            /* [out] */ void **ppvObj);
        
        END_INTERFACE
    } IBindHostVtbl;

    interface IBindHost
    {
        CONST_VTBL struct IBindHostVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBindHost_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBindHost_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBindHost_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBindHost_CreateMoniker(This,szName,pBC,ppmk,dwReserved)	\
    (This)->lpVtbl -> CreateMoniker(This,szName,pBC,ppmk,dwReserved)

#define IBindHost_MonikerBindToStorage(This,pMk,pBC,pBSC,riid,ppvObj)	\
    (This)->lpVtbl -> MonikerBindToStorage(This,pMk,pBC,pBSC,riid,ppvObj)

#define IBindHost_MonikerBindToObject(This,pMk,pBC,pBSC,riid,ppvObj)	\
    (This)->lpVtbl -> MonikerBindToObject(This,pMk,pBC,pBSC,riid,ppvObj)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBindHost_CreateMoniker_Proxy( 
    IBindHost * This,
    /* [in] */ LPOLESTR szName,
    /* [in] */ IBindCtx *pBC,
    /* [out] */ IMoniker **ppmk,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IBindHost_CreateMoniker_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IBindHost_RemoteMonikerBindToStorage_Proxy( 
    IBindHost * This,
    /* [unique][in] */ IMoniker *pMk,
    /* [unique][in] */ IBindCtx *pBC,
    /* [unique][in] */ IBindStatusCallback *pBSC,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ IUnknown **ppvObj);


void __RPC_STUB IBindHost_RemoteMonikerBindToStorage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IBindHost_RemoteMonikerBindToObject_Proxy( 
    IBindHost * This,
    /* [unique][in] */ IMoniker *pMk,
    /* [unique][in] */ IBindCtx *pBC,
    /* [unique][in] */ IBindStatusCallback *pBSC,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ IUnknown **ppvObj);


void __RPC_STUB IBindHost_RemoteMonikerBindToObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBindHost_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0193 */
/* [local] */ 

#endif
                                                                                                           
// These are for backwards compatibility with previous URLMON versions
// Flags for the UrlDownloadToCacheFile                                                                    
#define URLOSTRM_USECACHEDCOPY_ONLY             0x1      // Only get from cache                            
#define URLOSTRM_USECACHEDCOPY                  0x2      // Get from cache if available else download      
#define URLOSTRM_GETNEWESTVERSION               0x3      // Get new version only. But put it in cache too  
                                                                                                           
                                                                                                           
struct IBindStatusCallback;                                                                                
STDAPI HlinkSimpleNavigateToString(                                                                        
    /* [in] */ LPCWSTR szTarget,         // required - target document - null if local jump w/in doc       
    /* [in] */ LPCWSTR szLocation,       // optional, for navigation into middle of a doc                  
    /* [in] */ LPCWSTR szTargetFrameName,// optional, for targeting frame-sets                             
    /* [in] */ IUnknown *pUnk,           // required - we'll search this for other necessary interfaces    
    /* [in] */ IBindCtx *pbc,            // optional. caller may register an IBSC in this                  
    /* [in] */ IBindStatusCallback *,                                                                      
    /* [in] */ DWORD grfHLNF,            // flags                                                          
    /* [in] */ DWORD dwReserved          // for future use, must be NULL                                   
);                                                                                                         
                                                                                                           
STDAPI HlinkSimpleNavigateToMoniker(                                                                       
    /* [in] */ IMoniker *pmkTarget,      // required - target document - (may be null                      
    /* [in] */ LPCWSTR szLocation,       // optional, for navigation into middle of a doc                  
    /* [in] */ LPCWSTR szTargetFrameName,// optional, for targeting frame-sets                             
    /* [in] */ IUnknown *pUnk,           // required - we'll search this for other necessary interfaces    
    /* [in] */ IBindCtx *pbc,            // optional. caller may register an IBSC in this                  
    /* [in] */ IBindStatusCallback *,                                                                      
    /* [in] */ DWORD grfHLNF,            // flags                                                          
    /* [in] */ DWORD dwReserved          // for future use, must be NULL                                   
);                                                                                                         
                                                                                                           
STDAPI URLOpenStreamA(LPUNKNOWN,LPCSTR,DWORD,LPBINDSTATUSCALLBACK);                                        
STDAPI URLOpenStreamW(LPUNKNOWN,LPCWSTR,DWORD,LPBINDSTATUSCALLBACK);                                       
STDAPI URLOpenPullStreamA(LPUNKNOWN,LPCSTR,DWORD,LPBINDSTATUSCALLBACK);                                    
STDAPI URLOpenPullStreamW(LPUNKNOWN,LPCWSTR,DWORD,LPBINDSTATUSCALLBACK);                                   
STDAPI URLDownloadToFileA(LPUNKNOWN,LPCSTR,LPCSTR,DWORD,LPBINDSTATUSCALLBACK);                             
STDAPI URLDownloadToFileW(LPUNKNOWN,LPCWSTR,LPCWSTR,DWORD,LPBINDSTATUSCALLBACK);                           
STDAPI URLDownloadToCacheFileA(LPUNKNOWN,LPCSTR,LPTSTR,DWORD,DWORD,LPBINDSTATUSCALLBACK);                  
STDAPI URLDownloadToCacheFileW(LPUNKNOWN,LPCWSTR,LPWSTR,DWORD,DWORD,LPBINDSTATUSCALLBACK);                 
STDAPI URLOpenBlockingStreamA(LPUNKNOWN,LPCSTR,LPSTREAM*,DWORD,LPBINDSTATUSCALLBACK);                      
STDAPI URLOpenBlockingStreamW(LPUNKNOWN,LPCWSTR,LPSTREAM*,DWORD,LPBINDSTATUSCALLBACK);                     
                                                                                                           
#ifdef UNICODE                                                                                             
#define URLOpenStream            URLOpenStreamW                                                            
#define URLOpenPullStream        URLOpenPullStreamW                                                        
#define URLDownloadToFile        URLDownloadToFileW                                                        
#define URLDownloadToCacheFile   URLDownloadToCacheFileW                                                   
#define URLOpenBlockingStream    URLOpenBlockingStreamW                                                    
#else                                                                                                      
#define URLOpenStream            URLOpenStreamA                                                            
#define URLOpenPullStream        URLOpenPullStreamA                                                        
#define URLDownloadToFile        URLDownloadToFileA                                                        
#define URLDownloadToCacheFile   URLDownloadToCacheFileA                                                   
#define URLOpenBlockingStream    URLOpenBlockingStreamA                                                    
#endif // !UNICODE                                                                                         
                                                                                                           
                                                                                                           
STDAPI HlinkGoBack(IUnknown *pUnk);                                                                        
STDAPI HlinkGoForward(IUnknown *pUnk);                                                                     
STDAPI HlinkNavigateString(IUnknown *pUnk, LPCWSTR szTarget);                                              
STDAPI HlinkNavigateMoniker(IUnknown *pUnk, IMoniker *pmkTarget);                                          
                                                                                                           
#ifndef  _URLMON_NO_ASYNC_PLUGABLE_PROTOCOLS_   








#ifndef _LPIINTERNET
#define _LPIINTERNET


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0193_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0193_v0_0_s_ifspec;

#ifndef __IInternet_INTERFACE_DEFINED__
#define __IInternet_INTERFACE_DEFINED__

/* interface IInternet */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IInternet *LPIINTERNET;


EXTERN_C const IID IID_IInternet;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9e0-baf9-11ce-8c82-00aa004ba90b")
    IInternet : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IInternetVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInternet * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInternet * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInternet * This);
        
        END_INTERFACE
    } IInternetVtbl;

    interface IInternet
    {
        CONST_VTBL struct IInternetVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInternet_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInternet_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInternet_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IInternet_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0194 */
/* [local] */ 

#endif
#ifndef _LPIINTERNETBINDINFO
#define _LPIINTERNETBINDINFO


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0194_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0194_v0_0_s_ifspec;

#ifndef __IInternetBindInfo_INTERFACE_DEFINED__
#define __IInternetBindInfo_INTERFACE_DEFINED__

/* interface IInternetBindInfo */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IInternetBindInfo *LPIINTERNETBINDINFO;

typedef 
enum tagBINDSTRING
    {	BINDSTRING_HEADERS	= 1,
	BINDSTRING_ACCEPT_MIMES	= BINDSTRING_HEADERS + 1,
	BINDSTRING_EXTRA_URL	= BINDSTRING_ACCEPT_MIMES + 1,
	BINDSTRING_LANGUAGE	= BINDSTRING_EXTRA_URL + 1,
	BINDSTRING_USERNAME	= BINDSTRING_LANGUAGE + 1,
	BINDSTRING_PASSWORD	= BINDSTRING_USERNAME + 1,
	BINDSTRING_UA_PIXELS	= BINDSTRING_PASSWORD + 1,
	BINDSTRING_UA_COLOR	= BINDSTRING_UA_PIXELS + 1,
	BINDSTRING_OS	= BINDSTRING_UA_COLOR + 1,
	BINDSTRING_USER_AGENT	= BINDSTRING_OS + 1,
	BINDSTRING_ACCEPT_ENCODINGS	= BINDSTRING_USER_AGENT + 1,
	BINDSTRING_POST_COOKIE	= BINDSTRING_ACCEPT_ENCODINGS + 1,
	BINDSTRING_POST_DATA_MIME	= BINDSTRING_POST_COOKIE + 1,
	BINDSTRING_URL	= BINDSTRING_POST_DATA_MIME + 1,
	BINDSTRING_IID	= BINDSTRING_URL + 1,
	BINDSTRING_FLAG_BIND_TO_OBJECT	= BINDSTRING_IID + 1,
	BINDSTRING_PTR_BIND_CONTEXT	= BINDSTRING_FLAG_BIND_TO_OBJECT + 1
    } 	BINDSTRING;


EXTERN_C const IID IID_IInternetBindInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9e1-baf9-11ce-8c82-00aa004ba90b")
    IInternetBindInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetBindInfo( 
            /* [out] */ DWORD *grfBINDF,
            /* [unique][out][in] */ BINDINFO *pbindinfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetBindString( 
            /* [in] */ ULONG ulStringType,
            /* [out][in] */ LPOLESTR *ppwzStr,
            /* [in] */ ULONG cEl,
            /* [out][in] */ ULONG *pcElFetched) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInternetBindInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInternetBindInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInternetBindInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInternetBindInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetBindInfo )( 
            IInternetBindInfo * This,
            /* [out] */ DWORD *grfBINDF,
            /* [unique][out][in] */ BINDINFO *pbindinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetBindString )( 
            IInternetBindInfo * This,
            /* [in] */ ULONG ulStringType,
            /* [out][in] */ LPOLESTR *ppwzStr,
            /* [in] */ ULONG cEl,
            /* [out][in] */ ULONG *pcElFetched);
        
        END_INTERFACE
    } IInternetBindInfoVtbl;

    interface IInternetBindInfo
    {
        CONST_VTBL struct IInternetBindInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInternetBindInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInternetBindInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInternetBindInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IInternetBindInfo_GetBindInfo(This,grfBINDF,pbindinfo)	\
    (This)->lpVtbl -> GetBindInfo(This,grfBINDF,pbindinfo)

#define IInternetBindInfo_GetBindString(This,ulStringType,ppwzStr,cEl,pcElFetched)	\
    (This)->lpVtbl -> GetBindString(This,ulStringType,ppwzStr,cEl,pcElFetched)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IInternetBindInfo_GetBindInfo_Proxy( 
    IInternetBindInfo * This,
    /* [out] */ DWORD *grfBINDF,
    /* [unique][out][in] */ BINDINFO *pbindinfo);


void __RPC_STUB IInternetBindInfo_GetBindInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetBindInfo_GetBindString_Proxy( 
    IInternetBindInfo * This,
    /* [in] */ ULONG ulStringType,
    /* [out][in] */ LPOLESTR *ppwzStr,
    /* [in] */ ULONG cEl,
    /* [out][in] */ ULONG *pcElFetched);


void __RPC_STUB IInternetBindInfo_GetBindString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IInternetBindInfo_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0195 */
/* [local] */ 

#endif
#ifndef _LPIINTERNETPROTOCOLROOT_DEFINED
#define _LPIINTERNETPROTOCOLROOT_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0195_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0195_v0_0_s_ifspec;

#ifndef __IInternetProtocolRoot_INTERFACE_DEFINED__
#define __IInternetProtocolRoot_INTERFACE_DEFINED__

/* interface IInternetProtocolRoot */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IInternetProtocolRoot *LPIINTERNETPROTOCOLROOT;

typedef 
enum _tagPI_FLAGS
    {	PI_PARSE_URL	= 0x1,
	PI_FILTER_MODE	= 0x2,
	PI_FORCE_ASYNC	= 0x4,
	PI_USE_WORKERTHREAD	= 0x8,
	PI_MIMEVERIFICATION	= 0x10,
	PI_CLSIDLOOKUP	= 0x20,
	PI_DATAPROGRESS	= 0x40,
	PI_SYNCHRONOUS	= 0x80,
	PI_APARTMENTTHREADED	= 0x100,
	PI_CLASSINSTALL	= 0x200,
	PI_PASSONBINDCTX	= 0x2000,
	PI_NOMIMEHANDLER	= 0x8000,
	PI_LOADAPPDIRECT	= 0x4000,
	PD_FORCE_SWITCH	= 0x10000,
	PI_PREFERDEFAULTHANDLER	= 0x20000
    } 	PI_FLAGS;

typedef struct _tagPROTOCOLDATA
    {
    DWORD grfFlags;
    DWORD dwState;
    LPVOID pData;
    ULONG cbData;
    } 	PROTOCOLDATA;

typedef struct _tagStartParam
    {
    IID iid;
    IBindCtx *pIBindCtx;
    IUnknown *pItf;
    } 	StartParam;


EXTERN_C const IID IID_IInternetProtocolRoot;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9e3-baf9-11ce-8c82-00aa004ba90b")
    IInternetProtocolRoot : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Start( 
            /* [in] */ LPCWSTR szUrl,
            /* [in] */ IInternetProtocolSink *pOIProtSink,
            /* [in] */ IInternetBindInfo *pOIBindInfo,
            /* [in] */ DWORD grfPI,
            /* [in] */ HANDLE_PTR dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Continue( 
            /* [in] */ PROTOCOLDATA *pProtocolData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Abort( 
            /* [in] */ HRESULT hrReason,
            /* [in] */ DWORD dwOptions) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Terminate( 
            /* [in] */ DWORD dwOptions) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Suspend( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Resume( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInternetProtocolRootVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInternetProtocolRoot * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInternetProtocolRoot * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInternetProtocolRoot * This);
        
        HRESULT ( STDMETHODCALLTYPE *Start )( 
            IInternetProtocolRoot * This,
            /* [in] */ LPCWSTR szUrl,
            /* [in] */ IInternetProtocolSink *pOIProtSink,
            /* [in] */ IInternetBindInfo *pOIBindInfo,
            /* [in] */ DWORD grfPI,
            /* [in] */ HANDLE_PTR dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *Continue )( 
            IInternetProtocolRoot * This,
            /* [in] */ PROTOCOLDATA *pProtocolData);
        
        HRESULT ( STDMETHODCALLTYPE *Abort )( 
            IInternetProtocolRoot * This,
            /* [in] */ HRESULT hrReason,
            /* [in] */ DWORD dwOptions);
        
        HRESULT ( STDMETHODCALLTYPE *Terminate )( 
            IInternetProtocolRoot * This,
            /* [in] */ DWORD dwOptions);
        
        HRESULT ( STDMETHODCALLTYPE *Suspend )( 
            IInternetProtocolRoot * This);
        
        HRESULT ( STDMETHODCALLTYPE *Resume )( 
            IInternetProtocolRoot * This);
        
        END_INTERFACE
    } IInternetProtocolRootVtbl;

    interface IInternetProtocolRoot
    {
        CONST_VTBL struct IInternetProtocolRootVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInternetProtocolRoot_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInternetProtocolRoot_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInternetProtocolRoot_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IInternetProtocolRoot_Start(This,szUrl,pOIProtSink,pOIBindInfo,grfPI,dwReserved)	\
    (This)->lpVtbl -> Start(This,szUrl,pOIProtSink,pOIBindInfo,grfPI,dwReserved)

#define IInternetProtocolRoot_Continue(This,pProtocolData)	\
    (This)->lpVtbl -> Continue(This,pProtocolData)

#define IInternetProtocolRoot_Abort(This,hrReason,dwOptions)	\
    (This)->lpVtbl -> Abort(This,hrReason,dwOptions)

#define IInternetProtocolRoot_Terminate(This,dwOptions)	\
    (This)->lpVtbl -> Terminate(This,dwOptions)

#define IInternetProtocolRoot_Suspend(This)	\
    (This)->lpVtbl -> Suspend(This)

#define IInternetProtocolRoot_Resume(This)	\
    (This)->lpVtbl -> Resume(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IInternetProtocolRoot_Start_Proxy( 
    IInternetProtocolRoot * This,
    /* [in] */ LPCWSTR szUrl,
    /* [in] */ IInternetProtocolSink *pOIProtSink,
    /* [in] */ IInternetBindInfo *pOIBindInfo,
    /* [in] */ DWORD grfPI,
    /* [in] */ HANDLE_PTR dwReserved);


void __RPC_STUB IInternetProtocolRoot_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetProtocolRoot_Continue_Proxy( 
    IInternetProtocolRoot * This,
    /* [in] */ PROTOCOLDATA *pProtocolData);


void __RPC_STUB IInternetProtocolRoot_Continue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetProtocolRoot_Abort_Proxy( 
    IInternetProtocolRoot * This,
    /* [in] */ HRESULT hrReason,
    /* [in] */ DWORD dwOptions);


void __RPC_STUB IInternetProtocolRoot_Abort_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetProtocolRoot_Terminate_Proxy( 
    IInternetProtocolRoot * This,
    /* [in] */ DWORD dwOptions);


void __RPC_STUB IInternetProtocolRoot_Terminate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetProtocolRoot_Suspend_Proxy( 
    IInternetProtocolRoot * This);


void __RPC_STUB IInternetProtocolRoot_Suspend_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetProtocolRoot_Resume_Proxy( 
    IInternetProtocolRoot * This);


void __RPC_STUB IInternetProtocolRoot_Resume_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IInternetProtocolRoot_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0196 */
/* [local] */ 

#endif
#ifndef _LPIINTERNETPROTOCOL_DEFINED
#define _LPIINTERNETPROTOCOL_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0196_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0196_v0_0_s_ifspec;

#ifndef __IInternetProtocol_INTERFACE_DEFINED__
#define __IInternetProtocol_INTERFACE_DEFINED__

/* interface IInternetProtocol */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IInternetProtocol *LPIINTERNETPROTOCOL;


EXTERN_C const IID IID_IInternetProtocol;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9e4-baf9-11ce-8c82-00aa004ba90b")
    IInternetProtocol : public IInternetProtocolRoot
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Read( 
            /* [length_is][size_is][out][in] */ void *pv,
            /* [in] */ ULONG cb,
            /* [out] */ ULONG *pcbRead) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Seek( 
            /* [in] */ LARGE_INTEGER dlibMove,
            /* [in] */ DWORD dwOrigin,
            /* [out] */ ULARGE_INTEGER *plibNewPosition) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LockRequest( 
            /* [in] */ DWORD dwOptions) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnlockRequest( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInternetProtocolVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInternetProtocol * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInternetProtocol * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInternetProtocol * This);
        
        HRESULT ( STDMETHODCALLTYPE *Start )( 
            IInternetProtocol * This,
            /* [in] */ LPCWSTR szUrl,
            /* [in] */ IInternetProtocolSink *pOIProtSink,
            /* [in] */ IInternetBindInfo *pOIBindInfo,
            /* [in] */ DWORD grfPI,
            /* [in] */ HANDLE_PTR dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *Continue )( 
            IInternetProtocol * This,
            /* [in] */ PROTOCOLDATA *pProtocolData);
        
        HRESULT ( STDMETHODCALLTYPE *Abort )( 
            IInternetProtocol * This,
            /* [in] */ HRESULT hrReason,
            /* [in] */ DWORD dwOptions);
        
        HRESULT ( STDMETHODCALLTYPE *Terminate )( 
            IInternetProtocol * This,
            /* [in] */ DWORD dwOptions);
        
        HRESULT ( STDMETHODCALLTYPE *Suspend )( 
            IInternetProtocol * This);
        
        HRESULT ( STDMETHODCALLTYPE *Resume )( 
            IInternetProtocol * This);
        
        HRESULT ( STDMETHODCALLTYPE *Read )( 
            IInternetProtocol * This,
            /* [length_is][size_is][out][in] */ void *pv,
            /* [in] */ ULONG cb,
            /* [out] */ ULONG *pcbRead);
        
        HRESULT ( STDMETHODCALLTYPE *Seek )( 
            IInternetProtocol * This,
            /* [in] */ LARGE_INTEGER dlibMove,
            /* [in] */ DWORD dwOrigin,
            /* [out] */ ULARGE_INTEGER *plibNewPosition);
        
        HRESULT ( STDMETHODCALLTYPE *LockRequest )( 
            IInternetProtocol * This,
            /* [in] */ DWORD dwOptions);
        
        HRESULT ( STDMETHODCALLTYPE *UnlockRequest )( 
            IInternetProtocol * This);
        
        END_INTERFACE
    } IInternetProtocolVtbl;

    interface IInternetProtocol
    {
        CONST_VTBL struct IInternetProtocolVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInternetProtocol_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInternetProtocol_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInternetProtocol_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IInternetProtocol_Start(This,szUrl,pOIProtSink,pOIBindInfo,grfPI,dwReserved)	\
    (This)->lpVtbl -> Start(This,szUrl,pOIProtSink,pOIBindInfo,grfPI,dwReserved)

#define IInternetProtocol_Continue(This,pProtocolData)	\
    (This)->lpVtbl -> Continue(This,pProtocolData)

#define IInternetProtocol_Abort(This,hrReason,dwOptions)	\
    (This)->lpVtbl -> Abort(This,hrReason,dwOptions)

#define IInternetProtocol_Terminate(This,dwOptions)	\
    (This)->lpVtbl -> Terminate(This,dwOptions)

#define IInternetProtocol_Suspend(This)	\
    (This)->lpVtbl -> Suspend(This)

#define IInternetProtocol_Resume(This)	\
    (This)->lpVtbl -> Resume(This)


#define IInternetProtocol_Read(This,pv,cb,pcbRead)	\
    (This)->lpVtbl -> Read(This,pv,cb,pcbRead)

#define IInternetProtocol_Seek(This,dlibMove,dwOrigin,plibNewPosition)	\
    (This)->lpVtbl -> Seek(This,dlibMove,dwOrigin,plibNewPosition)

#define IInternetProtocol_LockRequest(This,dwOptions)	\
    (This)->lpVtbl -> LockRequest(This,dwOptions)

#define IInternetProtocol_UnlockRequest(This)	\
    (This)->lpVtbl -> UnlockRequest(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IInternetProtocol_Read_Proxy( 
    IInternetProtocol * This,
    /* [length_is][size_is][out][in] */ void *pv,
    /* [in] */ ULONG cb,
    /* [out] */ ULONG *pcbRead);


void __RPC_STUB IInternetProtocol_Read_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetProtocol_Seek_Proxy( 
    IInternetProtocol * This,
    /* [in] */ LARGE_INTEGER dlibMove,
    /* [in] */ DWORD dwOrigin,
    /* [out] */ ULARGE_INTEGER *plibNewPosition);


void __RPC_STUB IInternetProtocol_Seek_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetProtocol_LockRequest_Proxy( 
    IInternetProtocol * This,
    /* [in] */ DWORD dwOptions);


void __RPC_STUB IInternetProtocol_LockRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetProtocol_UnlockRequest_Proxy( 
    IInternetProtocol * This);


void __RPC_STUB IInternetProtocol_UnlockRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IInternetProtocol_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0197 */
/* [local] */ 

#endif
#ifndef _LPIINTERNETPROTOCOLSINK_DEFINED
#define _LPIINTERNETPROTOCOLSINK_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0197_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0197_v0_0_s_ifspec;

#ifndef __IInternetProtocolSink_INTERFACE_DEFINED__
#define __IInternetProtocolSink_INTERFACE_DEFINED__

/* interface IInternetProtocolSink */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IInternetProtocolSink *LPIINTERNETPROTOCOLSINK;


EXTERN_C const IID IID_IInternetProtocolSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9e5-baf9-11ce-8c82-00aa004ba90b")
    IInternetProtocolSink : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Switch( 
            /* [in] */ PROTOCOLDATA *pProtocolData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReportProgress( 
            /* [in] */ ULONG ulStatusCode,
            /* [in] */ LPCWSTR szStatusText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReportData( 
            /* [in] */ DWORD grfBSCF,
            /* [in] */ ULONG ulProgress,
            /* [in] */ ULONG ulProgressMax) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReportResult( 
            /* [in] */ HRESULT hrResult,
            /* [in] */ DWORD dwError,
            /* [in] */ LPCWSTR szResult) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInternetProtocolSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInternetProtocolSink * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInternetProtocolSink * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInternetProtocolSink * This);
        
        HRESULT ( STDMETHODCALLTYPE *Switch )( 
            IInternetProtocolSink * This,
            /* [in] */ PROTOCOLDATA *pProtocolData);
        
        HRESULT ( STDMETHODCALLTYPE *ReportProgress )( 
            IInternetProtocolSink * This,
            /* [in] */ ULONG ulStatusCode,
            /* [in] */ LPCWSTR szStatusText);
        
        HRESULT ( STDMETHODCALLTYPE *ReportData )( 
            IInternetProtocolSink * This,
            /* [in] */ DWORD grfBSCF,
            /* [in] */ ULONG ulProgress,
            /* [in] */ ULONG ulProgressMax);
        
        HRESULT ( STDMETHODCALLTYPE *ReportResult )( 
            IInternetProtocolSink * This,
            /* [in] */ HRESULT hrResult,
            /* [in] */ DWORD dwError,
            /* [in] */ LPCWSTR szResult);
        
        END_INTERFACE
    } IInternetProtocolSinkVtbl;

    interface IInternetProtocolSink
    {
        CONST_VTBL struct IInternetProtocolSinkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInternetProtocolSink_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInternetProtocolSink_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInternetProtocolSink_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IInternetProtocolSink_Switch(This,pProtocolData)	\
    (This)->lpVtbl -> Switch(This,pProtocolData)

#define IInternetProtocolSink_ReportProgress(This,ulStatusCode,szStatusText)	\
    (This)->lpVtbl -> ReportProgress(This,ulStatusCode,szStatusText)

#define IInternetProtocolSink_ReportData(This,grfBSCF,ulProgress,ulProgressMax)	\
    (This)->lpVtbl -> ReportData(This,grfBSCF,ulProgress,ulProgressMax)

#define IInternetProtocolSink_ReportResult(This,hrResult,dwError,szResult)	\
    (This)->lpVtbl -> ReportResult(This,hrResult,dwError,szResult)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IInternetProtocolSink_Switch_Proxy( 
    IInternetProtocolSink * This,
    /* [in] */ PROTOCOLDATA *pProtocolData);


void __RPC_STUB IInternetProtocolSink_Switch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetProtocolSink_ReportProgress_Proxy( 
    IInternetProtocolSink * This,
    /* [in] */ ULONG ulStatusCode,
    /* [in] */ LPCWSTR szStatusText);


void __RPC_STUB IInternetProtocolSink_ReportProgress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetProtocolSink_ReportData_Proxy( 
    IInternetProtocolSink * This,
    /* [in] */ DWORD grfBSCF,
    /* [in] */ ULONG ulProgress,
    /* [in] */ ULONG ulProgressMax);


void __RPC_STUB IInternetProtocolSink_ReportData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetProtocolSink_ReportResult_Proxy( 
    IInternetProtocolSink * This,
    /* [in] */ HRESULT hrResult,
    /* [in] */ DWORD dwError,
    /* [in] */ LPCWSTR szResult);


void __RPC_STUB IInternetProtocolSink_ReportResult_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IInternetProtocolSink_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0198 */
/* [local] */ 

#endif
#ifndef _LPIINTERNETPROTOCOLSINKSTACKABLE_DEFINED
#define _LPIINTERNETPROTOCOLSINKSTACKABLE_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0198_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0198_v0_0_s_ifspec;

#ifndef __IInternetProtocolSinkStackable_INTERFACE_DEFINED__
#define __IInternetProtocolSinkStackable_INTERFACE_DEFINED__

/* interface IInternetProtocolSinkStackable */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IInternetProtocolSinkStackable *LPIINTERNETPROTOCOLSINKStackable;


EXTERN_C const IID IID_IInternetProtocolSinkStackable;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9f0-baf9-11ce-8c82-00aa004ba90b")
    IInternetProtocolSinkStackable : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SwitchSink( 
            /* [in] */ IInternetProtocolSink *pOIProtSink) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CommitSwitch( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RollbackSwitch( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInternetProtocolSinkStackableVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInternetProtocolSinkStackable * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInternetProtocolSinkStackable * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInternetProtocolSinkStackable * This);
        
        HRESULT ( STDMETHODCALLTYPE *SwitchSink )( 
            IInternetProtocolSinkStackable * This,
            /* [in] */ IInternetProtocolSink *pOIProtSink);
        
        HRESULT ( STDMETHODCALLTYPE *CommitSwitch )( 
            IInternetProtocolSinkStackable * This);
        
        HRESULT ( STDMETHODCALLTYPE *RollbackSwitch )( 
            IInternetProtocolSinkStackable * This);
        
        END_INTERFACE
    } IInternetProtocolSinkStackableVtbl;

    interface IInternetProtocolSinkStackable
    {
        CONST_VTBL struct IInternetProtocolSinkStackableVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInternetProtocolSinkStackable_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInternetProtocolSinkStackable_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInternetProtocolSinkStackable_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IInternetProtocolSinkStackable_SwitchSink(This,pOIProtSink)	\
    (This)->lpVtbl -> SwitchSink(This,pOIProtSink)

#define IInternetProtocolSinkStackable_CommitSwitch(This)	\
    (This)->lpVtbl -> CommitSwitch(This)

#define IInternetProtocolSinkStackable_RollbackSwitch(This)	\
    (This)->lpVtbl -> RollbackSwitch(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IInternetProtocolSinkStackable_SwitchSink_Proxy( 
    IInternetProtocolSinkStackable * This,
    /* [in] */ IInternetProtocolSink *pOIProtSink);


void __RPC_STUB IInternetProtocolSinkStackable_SwitchSink_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetProtocolSinkStackable_CommitSwitch_Proxy( 
    IInternetProtocolSinkStackable * This);


void __RPC_STUB IInternetProtocolSinkStackable_CommitSwitch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetProtocolSinkStackable_RollbackSwitch_Proxy( 
    IInternetProtocolSinkStackable * This);


void __RPC_STUB IInternetProtocolSinkStackable_RollbackSwitch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IInternetProtocolSinkStackable_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0199 */
/* [local] */ 

#endif
#ifndef _LPIINTERNETSESSION_DEFINED
#define _LPIINTERNETSESSION_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0199_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0199_v0_0_s_ifspec;

#ifndef __IInternetSession_INTERFACE_DEFINED__
#define __IInternetSession_INTERFACE_DEFINED__

/* interface IInternetSession */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IInternetSession *LPIINTERNETSESSION;

typedef 
enum _tagOIBDG_FLAGS
    {	OIBDG_APARTMENTTHREADED	= 0x100,
	OIBDG_DATAONLY	= 0x1000
    } 	OIBDG_FLAGS;


EXTERN_C const IID IID_IInternetSession;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9e7-baf9-11ce-8c82-00aa004ba90b")
    IInternetSession : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE RegisterNameSpace( 
            /* [in] */ IClassFactory *pCF,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ LPCWSTR pwzProtocol,
            /* [in] */ ULONG cPatterns,
            /* [in] */ const LPCWSTR *ppwzPatterns,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnregisterNameSpace( 
            /* [in] */ IClassFactory *pCF,
            /* [in] */ LPCWSTR pszProtocol) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RegisterMimeFilter( 
            /* [in] */ IClassFactory *pCF,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ LPCWSTR pwzType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnregisterMimeFilter( 
            /* [in] */ IClassFactory *pCF,
            /* [in] */ LPCWSTR pwzType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateBinding( 
            /* [in] */ LPBC pBC,
            /* [in] */ LPCWSTR szUrl,
            /* [in] */ IUnknown *pUnkOuter,
            /* [unique][out] */ IUnknown **ppUnk,
            /* [unique][out] */ IInternetProtocol **ppOInetProt,
            /* [in] */ DWORD dwOption) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSessionOption( 
            /* [in] */ DWORD dwOption,
            /* [in] */ LPVOID pBuffer,
            /* [in] */ DWORD dwBufferLength,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSessionOption( 
            /* [in] */ DWORD dwOption,
            /* [out][in] */ LPVOID pBuffer,
            /* [out][in] */ DWORD *pdwBufferLength,
            /* [in] */ DWORD dwReserved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInternetSessionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInternetSession * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInternetSession * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInternetSession * This);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterNameSpace )( 
            IInternetSession * This,
            /* [in] */ IClassFactory *pCF,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ LPCWSTR pwzProtocol,
            /* [in] */ ULONG cPatterns,
            /* [in] */ const LPCWSTR *ppwzPatterns,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *UnregisterNameSpace )( 
            IInternetSession * This,
            /* [in] */ IClassFactory *pCF,
            /* [in] */ LPCWSTR pszProtocol);
        
        HRESULT ( STDMETHODCALLTYPE *RegisterMimeFilter )( 
            IInternetSession * This,
            /* [in] */ IClassFactory *pCF,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ LPCWSTR pwzType);
        
        HRESULT ( STDMETHODCALLTYPE *UnregisterMimeFilter )( 
            IInternetSession * This,
            /* [in] */ IClassFactory *pCF,
            /* [in] */ LPCWSTR pwzType);
        
        HRESULT ( STDMETHODCALLTYPE *CreateBinding )( 
            IInternetSession * This,
            /* [in] */ LPBC pBC,
            /* [in] */ LPCWSTR szUrl,
            /* [in] */ IUnknown *pUnkOuter,
            /* [unique][out] */ IUnknown **ppUnk,
            /* [unique][out] */ IInternetProtocol **ppOInetProt,
            /* [in] */ DWORD dwOption);
        
        HRESULT ( STDMETHODCALLTYPE *SetSessionOption )( 
            IInternetSession * This,
            /* [in] */ DWORD dwOption,
            /* [in] */ LPVOID pBuffer,
            /* [in] */ DWORD dwBufferLength,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *GetSessionOption )( 
            IInternetSession * This,
            /* [in] */ DWORD dwOption,
            /* [out][in] */ LPVOID pBuffer,
            /* [out][in] */ DWORD *pdwBufferLength,
            /* [in] */ DWORD dwReserved);
        
        END_INTERFACE
    } IInternetSessionVtbl;

    interface IInternetSession
    {
        CONST_VTBL struct IInternetSessionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInternetSession_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInternetSession_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInternetSession_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IInternetSession_RegisterNameSpace(This,pCF,rclsid,pwzProtocol,cPatterns,ppwzPatterns,dwReserved)	\
    (This)->lpVtbl -> RegisterNameSpace(This,pCF,rclsid,pwzProtocol,cPatterns,ppwzPatterns,dwReserved)

#define IInternetSession_UnregisterNameSpace(This,pCF,pszProtocol)	\
    (This)->lpVtbl -> UnregisterNameSpace(This,pCF,pszProtocol)

#define IInternetSession_RegisterMimeFilter(This,pCF,rclsid,pwzType)	\
    (This)->lpVtbl -> RegisterMimeFilter(This,pCF,rclsid,pwzType)

#define IInternetSession_UnregisterMimeFilter(This,pCF,pwzType)	\
    (This)->lpVtbl -> UnregisterMimeFilter(This,pCF,pwzType)

#define IInternetSession_CreateBinding(This,pBC,szUrl,pUnkOuter,ppUnk,ppOInetProt,dwOption)	\
    (This)->lpVtbl -> CreateBinding(This,pBC,szUrl,pUnkOuter,ppUnk,ppOInetProt,dwOption)

#define IInternetSession_SetSessionOption(This,dwOption,pBuffer,dwBufferLength,dwReserved)	\
    (This)->lpVtbl -> SetSessionOption(This,dwOption,pBuffer,dwBufferLength,dwReserved)

#define IInternetSession_GetSessionOption(This,dwOption,pBuffer,pdwBufferLength,dwReserved)	\
    (This)->lpVtbl -> GetSessionOption(This,dwOption,pBuffer,pdwBufferLength,dwReserved)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IInternetSession_RegisterNameSpace_Proxy( 
    IInternetSession * This,
    /* [in] */ IClassFactory *pCF,
    /* [in] */ REFCLSID rclsid,
    /* [in] */ LPCWSTR pwzProtocol,
    /* [in] */ ULONG cPatterns,
    /* [in] */ const LPCWSTR *ppwzPatterns,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IInternetSession_RegisterNameSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetSession_UnregisterNameSpace_Proxy( 
    IInternetSession * This,
    /* [in] */ IClassFactory *pCF,
    /* [in] */ LPCWSTR pszProtocol);


void __RPC_STUB IInternetSession_UnregisterNameSpace_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetSession_RegisterMimeFilter_Proxy( 
    IInternetSession * This,
    /* [in] */ IClassFactory *pCF,
    /* [in] */ REFCLSID rclsid,
    /* [in] */ LPCWSTR pwzType);


void __RPC_STUB IInternetSession_RegisterMimeFilter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetSession_UnregisterMimeFilter_Proxy( 
    IInternetSession * This,
    /* [in] */ IClassFactory *pCF,
    /* [in] */ LPCWSTR pwzType);


void __RPC_STUB IInternetSession_UnregisterMimeFilter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetSession_CreateBinding_Proxy( 
    IInternetSession * This,
    /* [in] */ LPBC pBC,
    /* [in] */ LPCWSTR szUrl,
    /* [in] */ IUnknown *pUnkOuter,
    /* [unique][out] */ IUnknown **ppUnk,
    /* [unique][out] */ IInternetProtocol **ppOInetProt,
    /* [in] */ DWORD dwOption);


void __RPC_STUB IInternetSession_CreateBinding_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetSession_SetSessionOption_Proxy( 
    IInternetSession * This,
    /* [in] */ DWORD dwOption,
    /* [in] */ LPVOID pBuffer,
    /* [in] */ DWORD dwBufferLength,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IInternetSession_SetSessionOption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetSession_GetSessionOption_Proxy( 
    IInternetSession * This,
    /* [in] */ DWORD dwOption,
    /* [out][in] */ LPVOID pBuffer,
    /* [out][in] */ DWORD *pdwBufferLength,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IInternetSession_GetSessionOption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IInternetSession_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0200 */
/* [local] */ 

#endif
#ifndef _LPIINTERNETTHREADSWITCH_DEFINED
#define _LPIINTERNETTHREADSWITCH_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0200_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0200_v0_0_s_ifspec;

#ifndef __IInternetThreadSwitch_INTERFACE_DEFINED__
#define __IInternetThreadSwitch_INTERFACE_DEFINED__

/* interface IInternetThreadSwitch */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IInternetThreadSwitch *LPIINTERNETTHREADSWITCH;


EXTERN_C const IID IID_IInternetThreadSwitch;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9e8-baf9-11ce-8c82-00aa004ba90b")
    IInternetThreadSwitch : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Prepare( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Continue( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInternetThreadSwitchVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInternetThreadSwitch * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInternetThreadSwitch * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInternetThreadSwitch * This);
        
        HRESULT ( STDMETHODCALLTYPE *Prepare )( 
            IInternetThreadSwitch * This);
        
        HRESULT ( STDMETHODCALLTYPE *Continue )( 
            IInternetThreadSwitch * This);
        
        END_INTERFACE
    } IInternetThreadSwitchVtbl;

    interface IInternetThreadSwitch
    {
        CONST_VTBL struct IInternetThreadSwitchVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInternetThreadSwitch_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInternetThreadSwitch_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInternetThreadSwitch_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IInternetThreadSwitch_Prepare(This)	\
    (This)->lpVtbl -> Prepare(This)

#define IInternetThreadSwitch_Continue(This)	\
    (This)->lpVtbl -> Continue(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IInternetThreadSwitch_Prepare_Proxy( 
    IInternetThreadSwitch * This);


void __RPC_STUB IInternetThreadSwitch_Prepare_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetThreadSwitch_Continue_Proxy( 
    IInternetThreadSwitch * This);


void __RPC_STUB IInternetThreadSwitch_Continue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IInternetThreadSwitch_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0201 */
/* [local] */ 

#endif
#ifndef _LPIINTERNETPRIORITY_DEFINED
#define _LPIINTERNETPRIORITY_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0201_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0201_v0_0_s_ifspec;

#ifndef __IInternetPriority_INTERFACE_DEFINED__
#define __IInternetPriority_INTERFACE_DEFINED__

/* interface IInternetPriority */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IInternetPriority *LPIINTERNETPRIORITY;


EXTERN_C const IID IID_IInternetPriority;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9eb-baf9-11ce-8c82-00aa004ba90b")
    IInternetPriority : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetPriority( 
            /* [in] */ LONG nPriority) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPriority( 
            /* [out] */ LONG *pnPriority) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInternetPriorityVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInternetPriority * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInternetPriority * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInternetPriority * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetPriority )( 
            IInternetPriority * This,
            /* [in] */ LONG nPriority);
        
        HRESULT ( STDMETHODCALLTYPE *GetPriority )( 
            IInternetPriority * This,
            /* [out] */ LONG *pnPriority);
        
        END_INTERFACE
    } IInternetPriorityVtbl;

    interface IInternetPriority
    {
        CONST_VTBL struct IInternetPriorityVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInternetPriority_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInternetPriority_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInternetPriority_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IInternetPriority_SetPriority(This,nPriority)	\
    (This)->lpVtbl -> SetPriority(This,nPriority)

#define IInternetPriority_GetPriority(This,pnPriority)	\
    (This)->lpVtbl -> GetPriority(This,pnPriority)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IInternetPriority_SetPriority_Proxy( 
    IInternetPriority * This,
    /* [in] */ LONG nPriority);


void __RPC_STUB IInternetPriority_SetPriority_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetPriority_GetPriority_Proxy( 
    IInternetPriority * This,
    /* [out] */ LONG *pnPriority);


void __RPC_STUB IInternetPriority_GetPriority_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IInternetPriority_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0202 */
/* [local] */ 

#endif
#ifndef _LPIINTERNETPROTOCOLINFO_DEFINED
#define _LPIINTERNETPROTOCOLINFO_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0202_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0202_v0_0_s_ifspec;

#ifndef __IInternetProtocolInfo_INTERFACE_DEFINED__
#define __IInternetProtocolInfo_INTERFACE_DEFINED__

/* interface IInternetProtocolInfo */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IInternetProtocolInfo *LPIINTERNETPROTOCOLINFO;

typedef 
enum _tagPARSEACTION
    {	PARSE_CANONICALIZE	= 1,
	PARSE_FRIENDLY	= PARSE_CANONICALIZE + 1,
	PARSE_SECURITY_URL	= PARSE_FRIENDLY + 1,
	PARSE_ROOTDOCUMENT	= PARSE_SECURITY_URL + 1,
	PARSE_DOCUMENT	= PARSE_ROOTDOCUMENT + 1,
	PARSE_ANCHOR	= PARSE_DOCUMENT + 1,
	PARSE_ENCODE	= PARSE_ANCHOR + 1,
	PARSE_DECODE	= PARSE_ENCODE + 1,
	PARSE_PATH_FROM_URL	= PARSE_DECODE + 1,
	PARSE_URL_FROM_PATH	= PARSE_PATH_FROM_URL + 1,
	PARSE_MIME	= PARSE_URL_FROM_PATH + 1,
	PARSE_SERVER	= PARSE_MIME + 1,
	PARSE_SCHEMA	= PARSE_SERVER + 1,
	PARSE_SITE	= PARSE_SCHEMA + 1,
	PARSE_DOMAIN	= PARSE_SITE + 1,
	PARSE_LOCATION	= PARSE_DOMAIN + 1,
	PARSE_SECURITY_DOMAIN	= PARSE_LOCATION + 1,
	PARSE_ESCAPE	= PARSE_SECURITY_DOMAIN + 1,
	PARSE_UNESCAPE	= PARSE_ESCAPE + 1
    } 	PARSEACTION;

typedef 
enum _tagPSUACTION
    {	PSU_DEFAULT	= 1,
	PSU_SECURITY_URL_ONLY	= PSU_DEFAULT + 1
    } 	PSUACTION;

typedef 
enum _tagQUERYOPTION
    {	QUERY_EXPIRATION_DATE	= 1,
	QUERY_TIME_OF_LAST_CHANGE	= QUERY_EXPIRATION_DATE + 1,
	QUERY_CONTENT_ENCODING	= QUERY_TIME_OF_LAST_CHANGE + 1,
	QUERY_CONTENT_TYPE	= QUERY_CONTENT_ENCODING + 1,
	QUERY_REFRESH	= QUERY_CONTENT_TYPE + 1,
	QUERY_RECOMBINE	= QUERY_REFRESH + 1,
	QUERY_CAN_NAVIGATE	= QUERY_RECOMBINE + 1,
	QUERY_USES_NETWORK	= QUERY_CAN_NAVIGATE + 1,
	QUERY_IS_CACHED	= QUERY_USES_NETWORK + 1,
	QUERY_IS_INSTALLEDENTRY	= QUERY_IS_CACHED + 1,
	QUERY_IS_CACHED_OR_MAPPED	= QUERY_IS_INSTALLEDENTRY + 1,
	QUERY_USES_CACHE	= QUERY_IS_CACHED_OR_MAPPED + 1,
	QUERY_IS_SECURE	= QUERY_USES_CACHE + 1,
	QUERY_IS_SAFE	= QUERY_IS_SECURE + 1
    } 	QUERYOPTION;


EXTERN_C const IID IID_IInternetProtocolInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9ec-baf9-11ce-8c82-00aa004ba90b")
    IInternetProtocolInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ParseUrl( 
            /* [in] */ LPCWSTR pwzUrl,
            /* [in] */ PARSEACTION ParseAction,
            /* [in] */ DWORD dwParseFlags,
            /* [out] */ LPWSTR pwzResult,
            /* [in] */ DWORD cchResult,
            /* [out] */ DWORD *pcchResult,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CombineUrl( 
            /* [in] */ LPCWSTR pwzBaseUrl,
            /* [in] */ LPCWSTR pwzRelativeUrl,
            /* [in] */ DWORD dwCombineFlags,
            /* [out] */ LPWSTR pwzResult,
            /* [in] */ DWORD cchResult,
            /* [out] */ DWORD *pcchResult,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CompareUrl( 
            /* [in] */ LPCWSTR pwzUrl1,
            /* [in] */ LPCWSTR pwzUrl2,
            /* [in] */ DWORD dwCompareFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryInfo( 
            /* [in] */ LPCWSTR pwzUrl,
            /* [in] */ QUERYOPTION OueryOption,
            /* [in] */ DWORD dwQueryFlags,
            /* [size_is][out][in] */ LPVOID pBuffer,
            /* [in] */ DWORD cbBuffer,
            /* [out][in] */ DWORD *pcbBuf,
            /* [in] */ DWORD dwReserved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInternetProtocolInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInternetProtocolInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInternetProtocolInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInternetProtocolInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *ParseUrl )( 
            IInternetProtocolInfo * This,
            /* [in] */ LPCWSTR pwzUrl,
            /* [in] */ PARSEACTION ParseAction,
            /* [in] */ DWORD dwParseFlags,
            /* [out] */ LPWSTR pwzResult,
            /* [in] */ DWORD cchResult,
            /* [out] */ DWORD *pcchResult,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *CombineUrl )( 
            IInternetProtocolInfo * This,
            /* [in] */ LPCWSTR pwzBaseUrl,
            /* [in] */ LPCWSTR pwzRelativeUrl,
            /* [in] */ DWORD dwCombineFlags,
            /* [out] */ LPWSTR pwzResult,
            /* [in] */ DWORD cchResult,
            /* [out] */ DWORD *pcchResult,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *CompareUrl )( 
            IInternetProtocolInfo * This,
            /* [in] */ LPCWSTR pwzUrl1,
            /* [in] */ LPCWSTR pwzUrl2,
            /* [in] */ DWORD dwCompareFlags);
        
        HRESULT ( STDMETHODCALLTYPE *QueryInfo )( 
            IInternetProtocolInfo * This,
            /* [in] */ LPCWSTR pwzUrl,
            /* [in] */ QUERYOPTION OueryOption,
            /* [in] */ DWORD dwQueryFlags,
            /* [size_is][out][in] */ LPVOID pBuffer,
            /* [in] */ DWORD cbBuffer,
            /* [out][in] */ DWORD *pcbBuf,
            /* [in] */ DWORD dwReserved);
        
        END_INTERFACE
    } IInternetProtocolInfoVtbl;

    interface IInternetProtocolInfo
    {
        CONST_VTBL struct IInternetProtocolInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInternetProtocolInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInternetProtocolInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInternetProtocolInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IInternetProtocolInfo_ParseUrl(This,pwzUrl,ParseAction,dwParseFlags,pwzResult,cchResult,pcchResult,dwReserved)	\
    (This)->lpVtbl -> ParseUrl(This,pwzUrl,ParseAction,dwParseFlags,pwzResult,cchResult,pcchResult,dwReserved)

#define IInternetProtocolInfo_CombineUrl(This,pwzBaseUrl,pwzRelativeUrl,dwCombineFlags,pwzResult,cchResult,pcchResult,dwReserved)	\
    (This)->lpVtbl -> CombineUrl(This,pwzBaseUrl,pwzRelativeUrl,dwCombineFlags,pwzResult,cchResult,pcchResult,dwReserved)

#define IInternetProtocolInfo_CompareUrl(This,pwzUrl1,pwzUrl2,dwCompareFlags)	\
    (This)->lpVtbl -> CompareUrl(This,pwzUrl1,pwzUrl2,dwCompareFlags)

#define IInternetProtocolInfo_QueryInfo(This,pwzUrl,OueryOption,dwQueryFlags,pBuffer,cbBuffer,pcbBuf,dwReserved)	\
    (This)->lpVtbl -> QueryInfo(This,pwzUrl,OueryOption,dwQueryFlags,pBuffer,cbBuffer,pcbBuf,dwReserved)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IInternetProtocolInfo_ParseUrl_Proxy( 
    IInternetProtocolInfo * This,
    /* [in] */ LPCWSTR pwzUrl,
    /* [in] */ PARSEACTION ParseAction,
    /* [in] */ DWORD dwParseFlags,
    /* [out] */ LPWSTR pwzResult,
    /* [in] */ DWORD cchResult,
    /* [out] */ DWORD *pcchResult,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IInternetProtocolInfo_ParseUrl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetProtocolInfo_CombineUrl_Proxy( 
    IInternetProtocolInfo * This,
    /* [in] */ LPCWSTR pwzBaseUrl,
    /* [in] */ LPCWSTR pwzRelativeUrl,
    /* [in] */ DWORD dwCombineFlags,
    /* [out] */ LPWSTR pwzResult,
    /* [in] */ DWORD cchResult,
    /* [out] */ DWORD *pcchResult,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IInternetProtocolInfo_CombineUrl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetProtocolInfo_CompareUrl_Proxy( 
    IInternetProtocolInfo * This,
    /* [in] */ LPCWSTR pwzUrl1,
    /* [in] */ LPCWSTR pwzUrl2,
    /* [in] */ DWORD dwCompareFlags);


void __RPC_STUB IInternetProtocolInfo_CompareUrl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetProtocolInfo_QueryInfo_Proxy( 
    IInternetProtocolInfo * This,
    /* [in] */ LPCWSTR pwzUrl,
    /* [in] */ QUERYOPTION OueryOption,
    /* [in] */ DWORD dwQueryFlags,
    /* [size_is][out][in] */ LPVOID pBuffer,
    /* [in] */ DWORD cbBuffer,
    /* [out][in] */ DWORD *pcbBuf,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IInternetProtocolInfo_QueryInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IInternetProtocolInfo_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0203 */
/* [local] */ 

#endif
#define IOInet               IInternet            
#define IOInetBindInfo       IInternetBindInfo    
#define IOInetProtocolRoot   IInternetProtocolRoot
#define IOInetProtocol       IInternetProtocol    
#define IOInetProtocolSink   IInternetProtocolSink
#define IOInetProtocolInfo   IInternetProtocolInfo
#define IOInetSession        IInternetSession     
#define IOInetPriority       IInternetPriority    
#define IOInetThreadSwitch   IInternetThreadSwitch
#define IOInetProtocolSinkStackable   IInternetProtocolSinkStackable
#define LPOINET              LPIINTERNET             
#define LPOINETPROTOCOLINFO  LPIINTERNETPROTOCOLINFO 
#define LPOINETBINDINFO      LPIINTERNETBINDINFO     
#define LPOINETPROTOCOLROOT  LPIINTERNETPROTOCOLROOT 
#define LPOINETPROTOCOL      LPIINTERNETPROTOCOL     
#define LPOINETPROTOCOLSINK  LPIINTERNETPROTOCOLSINK 
#define LPOINETSESSION       LPIINTERNETSESSION      
#define LPOINETTHREADSWITCH  LPIINTERNETTHREADSWITCH 
#define LPOINETPRIORITY      LPIINTERNETPRIORITY     
#define LPOINETPROTOCOLINFO  LPIINTERNETPROTOCOLINFO 
#define LPOINETPROTOCOLSINKSTACKABLE  LPIINTERNETPROTOCOLSINKSTACKABLE 
#define IID_IOInet               IID_IInternet            
#define IID_IOInetBindInfo       IID_IInternetBindInfo    
#define IID_IOInetProtocolRoot   IID_IInternetProtocolRoot
#define IID_IOInetProtocol       IID_IInternetProtocol    
#define IID_IOInetProtocolSink   IID_IInternetProtocolSink
#define IID_IOInetProtocolInfo   IID_IInternetProtocolInfo
#define IID_IOInetSession        IID_IInternetSession     
#define IID_IOInetPriority       IID_IInternetPriority    
#define IID_IOInetThreadSwitch   IID_IInternetThreadSwitch
#define IID_IOInetProtocolSinkStackable   IID_IInternetProtocolSinkStackable
STDAPI CoInternetParseUrl(               
    LPCWSTR     pwzUrl,                  
    PARSEACTION ParseAction,             
    DWORD       dwFlags,                 
    LPWSTR      pszResult,               
    DWORD       cchResult,               
    DWORD      *pcchResult,              
    DWORD       dwReserved               
    );                                   
STDAPI CoInternetCombineUrl(             
    LPCWSTR     pwzBaseUrl,              
    LPCWSTR     pwzRelativeUrl,          
    DWORD       dwCombineFlags,          
    LPWSTR      pszResult,               
    DWORD       cchResult,               
    DWORD      *pcchResult,              
    DWORD       dwReserved               
    );                                   
STDAPI CoInternetCompareUrl(             
    LPCWSTR pwzUrl1,                     
    LPCWSTR pwzUrl2,                     
    DWORD dwFlags                        
    );                                   
STDAPI CoInternetGetProtocolFlags(       
    LPCWSTR     pwzUrl,                  
    DWORD      *pdwFlags,                
    DWORD       dwReserved               
    );                                   
STDAPI CoInternetQueryInfo(              
    LPCWSTR     pwzUrl,                  
    QUERYOPTION QueryOptions,            
    DWORD       dwQueryFlags,            
    LPVOID      pvBuffer,                
    DWORD       cbBuffer,                
    DWORD      *pcbBuffer,               
    DWORD       dwReserved               
    );                                   
STDAPI CoInternetGetSession(             
    DWORD       dwSessionMode,           
    IInternetSession **ppIInternetSession,
    DWORD       dwReserved               
    );                                   
STDAPI CoInternetGetSecurityUrl(         
    LPCWSTR pwzUrl,                      
    LPWSTR  *ppwzSecUrl,                 
    PSUACTION  psuAction,                
    DWORD   dwReserved                   
    );                                   
STDAPI AsyncInstallDistributionUnit(     
    LPCWSTR szDistUnit,                  
    LPCWSTR szTYPE,                      
    LPCWSTR szExt,                       
    DWORD dwFileVersionMS,               
    DWORD dwFileVersionLS,               
    LPCWSTR szURL,                       
    IBindCtx *pbc,                       
    LPVOID   pvReserved,                 
    DWORD   flags                        
    );                                   
#ifndef _INTERNETFEATURELIST_DEFINED
#define _INTERNETFEATURELIST_DEFINED
typedef 
enum _tagINTERNETFEATURELIST
    {	FEATURE_OBJECT_CACHING	= 0,
	FEATURE_ZONE_ELEVATION	= FEATURE_OBJECT_CACHING + 1,
	FEATURE_MIME_HANDLING	= FEATURE_ZONE_ELEVATION + 1,
	FEATURE_MIME_SNIFFING	= FEATURE_MIME_HANDLING + 1,
	FEATURE_WINDOW_RESTRICTIONS	= FEATURE_MIME_SNIFFING + 1,
	FEATURE_WEBOC_POPUPMANAGEMENT	= FEATURE_WINDOW_RESTRICTIONS + 1,
	FEATURE_BEHAVIORS	= FEATURE_WEBOC_POPUPMANAGEMENT + 1,
	FEATURE_DISABLE_MK_PROTOCOL	= FEATURE_BEHAVIORS + 1,
	FEATURE_LOCALMACHINE_LOCKDOWN	= FEATURE_DISABLE_MK_PROTOCOL + 1,
	FEATURE_SECURITYBAND	= FEATURE_LOCALMACHINE_LOCKDOWN + 1,
	FEATURE_RESTRICT_ACTIVEXINSTALL	= FEATURE_SECURITYBAND + 1,
	FEATURE_VALIDATE_NAVIGATE_URL	= FEATURE_RESTRICT_ACTIVEXINSTALL + 1,
	FEATURE_RESTRICT_FILEDOWNLOAD	= FEATURE_VALIDATE_NAVIGATE_URL + 1,
	FEATURE_ADDON_MANAGEMENT	= FEATURE_RESTRICT_FILEDOWNLOAD + 1,
	FEATURE_PROTOCOL_LOCKDOWN	= FEATURE_ADDON_MANAGEMENT + 1,
	FEATURE_HTTP_USERNAME_PASSWORD_DISABLE	= FEATURE_PROTOCOL_LOCKDOWN + 1,
	FEATURE_SAFE_BINDTOOBJECT	= FEATURE_HTTP_USERNAME_PASSWORD_DISABLE + 1,
	FEATURE_UNC_SAVEDFILECHECK	= FEATURE_SAFE_BINDTOOBJECT + 1,
	FEATURE_GET_URL_DOM_FILEPATH_UNENCODED	= FEATURE_UNC_SAVEDFILECHECK + 1,
	FEATURE_ENTRY_COUNT	= FEATURE_GET_URL_DOM_FILEPATH_UNENCODED + 1
    } 	INTERNETFEATURELIST;


// CoInternetSetFeatureEnabled can be used to set/reset features. 
// The following flags control where the feature is set

#define SET_FEATURE_ON_THREAD                       0x00000001
#define SET_FEATURE_ON_PROCESS                      0x00000002
#define SET_FEATURE_IN_REGISTRY                     0x00000004
#define SET_FEATURE_ON_THREAD_LOCALMACHINE          0x00000008
#define SET_FEATURE_ON_THREAD_INTRANET              0x00000010
#define SET_FEATURE_ON_THREAD_TRUSTED               0x00000020
#define SET_FEATURE_ON_THREAD_INTERNET              0x00000040
#define SET_FEATURE_ON_THREAD_RESTRICTED            0x00000080

// CoInternetIsFeatureEnabled can be used to get features. 
// The following flags control where the feature is obtained from
// default is from process

#define GET_FEATURE_FROM_THREAD                      0x00000001
#define GET_FEATURE_FROM_PROCESS                     0x00000002
#define GET_FEATURE_FROM_REGISTRY                    0x00000004
#define GET_FEATURE_FROM_THREAD_LOCALMACHINE         0x00000008
#define GET_FEATURE_FROM_THREAD_INTRANET             0x00000010
#define GET_FEATURE_FROM_THREAD_TRUSTED              0x00000020
#define GET_FEATURE_FROM_THREAD_INTERNET             0x00000040
#define GET_FEATURE_FROM_THREAD_RESTRICTED           0x00000080
#endif
STDAPI CoInternetSetFeatureEnabled(      
    INTERNETFEATURELIST FeatureEntry,    
    DWORD dwFlags,                       
    BOOL fEnable                         
    );                                   
STDAPI CoInternetIsFeatureEnabled(       
    INTERNETFEATURELIST FeatureEntry,    
    DWORD dwFlags                        
    );                                   
STDAPI CoInternetIsFeatureEnabledForUrl( 
    INTERNETFEATURELIST FeatureEntry,    
    DWORD dwFlags,                       
    LPCWSTR szURL,                       
    IInternetSecurityManager *pSecMgr    
    );                                   
STDAPI CoInternetIsFeatureZoneElevationEnabled( 
    LPCWSTR szFromURL,                          
    LPCWSTR szToURL,                            
    IInternetSecurityManager *pSecMgr,          
    DWORD dwFlags                               
    );                                          
 
STDAPI CopyStgMedium(const STGMEDIUM * pcstgmedSrc,  
                           STGMEDIUM * pstgmedDest); 
STDAPI CopyBindInfo( const BINDINFO * pcbiSrc,   
                           BINDINFO * pbiDest ); 
STDAPI_(void) ReleaseBindInfo( BINDINFO * pbindinfo );  
 
#define INET_E_USE_DEFAULT_PROTOCOLHANDLER _HRESULT_TYPEDEF_(0x800C0011L)      
#define INET_E_USE_DEFAULT_SETTING         _HRESULT_TYPEDEF_(0x800C0012L)      
#define INET_E_DEFAULT_ACTION              INET_E_USE_DEFAULT_PROTOCOLHANDLER  
#define INET_E_QUERYOPTION_UNKNOWN         _HRESULT_TYPEDEF_(0x800C0013L)      
#define INET_E_REDIRECTING                 _HRESULT_TYPEDEF_(0x800C0014L)      
#define OInetParseUrl               CoInternetParseUrl               
#define OInetCombineUrl             CoInternetCombineUrl             
#define OInetCompareUrl             CoInternetCompareUrl             
#define OInetQueryInfo              CoInternetQueryInfo              
#define OInetGetSession             CoInternetGetSession             
#endif // !_URLMON_NO_ASYNC_PLUGABLE_PROTOCOLS_ 
//
// Static Protocol flags
//
#define PROTOCOLFLAG_NO_PICS_CHECK     0x00000001

// Creates the security manager object. The first argument is the Service provider
// to allow for delegation
STDAPI CoInternetCreateSecurityManager(IServiceProvider *pSP, IInternetSecurityManager **ppSM, DWORD dwReserved);

STDAPI CoInternetCreateZoneManager(IServiceProvider *pSP, IInternetZoneManager **ppZM, DWORD dwReserved);


// Security manager CLSID's
EXTERN_C const IID CLSID_InternetSecurityManager;  
EXTERN_C const IID CLSID_InternetZoneManager;  
EXTERN_C const IID CLSID_PersistentZoneIdentifier;  
// This service is used for delegation support on the Security Manager interface
#define SID_SInternetSecurityManager         IID_IInternetSecurityManager

#define SID_SInternetSecurityManagerEx         IID_IInternetSecurityManagerEx

#define SID_SInternetHostSecurityManager     IID_IInternetHostSecurityManager

#ifndef _LPINTERNETSECURITYMGRSITE_DEFINED
#define _LPINTERNETSECURITYMGRSITE_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0203_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0203_v0_0_s_ifspec;

#ifndef __IInternetSecurityMgrSite_INTERFACE_DEFINED__
#define __IInternetSecurityMgrSite_INTERFACE_DEFINED__

/* interface IInternetSecurityMgrSite */
/* [unique][helpstring][uuid][object][local] */ 


EXTERN_C const IID IID_IInternetSecurityMgrSite;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9ed-baf9-11ce-8c82-00aa004ba90b")
    IInternetSecurityMgrSite : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetWindow( 
            /* [out] */ HWND *phwnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnableModeless( 
            /* [in] */ BOOL fEnable) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInternetSecurityMgrSiteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInternetSecurityMgrSite * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInternetSecurityMgrSite * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInternetSecurityMgrSite * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IInternetSecurityMgrSite * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *EnableModeless )( 
            IInternetSecurityMgrSite * This,
            /* [in] */ BOOL fEnable);
        
        END_INTERFACE
    } IInternetSecurityMgrSiteVtbl;

    interface IInternetSecurityMgrSite
    {
        CONST_VTBL struct IInternetSecurityMgrSiteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInternetSecurityMgrSite_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInternetSecurityMgrSite_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInternetSecurityMgrSite_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IInternetSecurityMgrSite_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IInternetSecurityMgrSite_EnableModeless(This,fEnable)	\
    (This)->lpVtbl -> EnableModeless(This,fEnable)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IInternetSecurityMgrSite_GetWindow_Proxy( 
    IInternetSecurityMgrSite * This,
    /* [out] */ HWND *phwnd);


void __RPC_STUB IInternetSecurityMgrSite_GetWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetSecurityMgrSite_EnableModeless_Proxy( 
    IInternetSecurityMgrSite * This,
    /* [in] */ BOOL fEnable);


void __RPC_STUB IInternetSecurityMgrSite_EnableModeless_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IInternetSecurityMgrSite_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0204 */
/* [local] */ 

#endif
#ifndef _LPINTERNETSECURITYMANANGEREX_DEFINED
#define _LPINTERNETSECURITYMANANGEREX_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0204_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0204_v0_0_s_ifspec;

#ifndef __IInternetSecurityManager_INTERFACE_DEFINED__
#define __IInternetSecurityManager_INTERFACE_DEFINED__

/* interface IInternetSecurityManager */
/* [object][unique][helpstring][uuid] */ 

#define MUTZ_NOSAVEDFILECHECK        0x00000001 // don't check file: for saved file comment
#define MUTZ_ISFILE                  0x00000002 // Assume URL if File, url does not need file://
#define MUTZ_ACCEPT_WILDCARD_SCHEME  0x00000080 // Accept a wildcard scheme
#define MUTZ_ENFORCERESTRICTED       0x00000100 // enforce restricted zone independent of URL
#define MUTZ_REQUIRESAVEDFILECHECK   0x00000400 // always check the file for MOTW (overriding FEATURE_UNC_SAVEDFILECHECK)
#define MUTZ_DONT_UNESCAPE           0x00000800 // Do not unescape the url
// MapUrlToZone returns the zone index given a URL
#define MAX_SIZE_SECURITY_ID 512 // bytes
typedef /* [public] */ 
enum __MIDL_IInternetSecurityManager_0001
    {	PUAF_DEFAULT	= 0,
	PUAF_NOUI	= 0x1,
	PUAF_ISFILE	= 0x2,
	PUAF_WARN_IF_DENIED	= 0x4,
	PUAF_FORCEUI_FOREGROUND	= 0x8,
	PUAF_CHECK_TIFS	= 0x10,
	PUAF_DONTCHECKBOXINDIALOG	= 0x20,
	PUAF_TRUSTED	= 0x40,
	PUAF_ACCEPT_WILDCARD_SCHEME	= 0x80,
	PUAF_ENFORCERESTRICTED	= 0x100,
	PUAF_NOSAVEDFILECHECK	= 0x200,
	PUAF_REQUIRESAVEDFILECHECK	= 0x400,
	PUAF_LMZ_UNLOCKED	= 0x10000,
	PUAF_LMZ_LOCKED	= 0x20000,
	PUAF_DEFAULTZONEPOL	= 0x40000,
	PUAF_NPL_USE_LOCKED_IF_RESTRICTED	= 0x80000,
	PUAF_NOUIIFLOCKED	= 0x100000,
	PUAF_DRAGPROTOCOLCHECK	= 0x200000
    } 	PUAF;

typedef /* [public] */ 
enum __MIDL_IInternetSecurityManager_0002
    {	PUAFOUT_DEFAULT	= 0,
	PUAFOUT_ISLOCKZONEPOLICY	= 0x1
    } 	PUAFOUT;

// This is the wrapper function that most clients will use.
// It figures out the current Policy for the passed in Action,
// and puts up UI if the current Policy indicates that the user
// should be queried. It returns back the Policy which the caller
// will use to determine if the action should be allowed
// This is the wrapper function to conveniently read a custom policy.
typedef /* [public] */ 
enum __MIDL_IInternetSecurityManager_0003
    {	SZM_CREATE	= 0,
	SZM_DELETE	= 0x1
    } 	SZM_FLAGS;

// SetZoneMapping
//    lpszPattern: string denoting a URL pattern
//        Examples of valid patterns:   
//            *://*.msn.com             
//            http://*.sony.co.jp       
//            *://et.msn.com            
//            ftp://157.54.23.41/       
//            https://localsvr          
//            file:\localsvr\share     
//            *://157.54.100-200.*      
//        Examples of invalid patterns: 
//            http://*.lcs.mit.edu      
//            ftp://*                   
//    dwFlags: SZM_FLAGS values         

EXTERN_C const IID IID_IInternetSecurityManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9ee-baf9-11ce-8c82-00aa004ba90b")
    IInternetSecurityManager : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetSecuritySite( 
            /* [unique][in] */ IInternetSecurityMgrSite *pSite) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSecuritySite( 
            /* [out] */ IInternetSecurityMgrSite **ppSite) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MapUrlToZone( 
            /* [in] */ LPCWSTR pwszUrl,
            /* [out] */ DWORD *pdwZone,
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSecurityId( 
            /* [in] */ LPCWSTR pwszUrl,
            /* [size_is][out] */ BYTE *pbSecurityId,
            /* [out][in] */ DWORD *pcbSecurityId,
            /* [in] */ DWORD_PTR dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ProcessUrlAction( 
            /* [in] */ LPCWSTR pwszUrl,
            /* [in] */ DWORD dwAction,
            /* [size_is][out] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ BYTE *pContext,
            /* [in] */ DWORD cbContext,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryCustomPolicy( 
            /* [in] */ LPCWSTR pwszUrl,
            /* [in] */ REFGUID guidKey,
            /* [size_is][size_is][out] */ BYTE **ppPolicy,
            /* [out] */ DWORD *pcbPolicy,
            /* [in] */ BYTE *pContext,
            /* [in] */ DWORD cbContext,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetZoneMapping( 
            /* [in] */ DWORD dwZone,
            /* [in] */ LPCWSTR lpszPattern,
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetZoneMappings( 
            /* [in] */ DWORD dwZone,
            /* [out] */ IEnumString **ppenumString,
            /* [in] */ DWORD dwFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInternetSecurityManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInternetSecurityManager * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInternetSecurityManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInternetSecurityManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetSecuritySite )( 
            IInternetSecurityManager * This,
            /* [unique][in] */ IInternetSecurityMgrSite *pSite);
        
        HRESULT ( STDMETHODCALLTYPE *GetSecuritySite )( 
            IInternetSecurityManager * This,
            /* [out] */ IInternetSecurityMgrSite **ppSite);
        
        HRESULT ( STDMETHODCALLTYPE *MapUrlToZone )( 
            IInternetSecurityManager * This,
            /* [in] */ LPCWSTR pwszUrl,
            /* [out] */ DWORD *pdwZone,
            /* [in] */ DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetSecurityId )( 
            IInternetSecurityManager * This,
            /* [in] */ LPCWSTR pwszUrl,
            /* [size_is][out] */ BYTE *pbSecurityId,
            /* [out][in] */ DWORD *pcbSecurityId,
            /* [in] */ DWORD_PTR dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *ProcessUrlAction )( 
            IInternetSecurityManager * This,
            /* [in] */ LPCWSTR pwszUrl,
            /* [in] */ DWORD dwAction,
            /* [size_is][out] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ BYTE *pContext,
            /* [in] */ DWORD cbContext,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *QueryCustomPolicy )( 
            IInternetSecurityManager * This,
            /* [in] */ LPCWSTR pwszUrl,
            /* [in] */ REFGUID guidKey,
            /* [size_is][size_is][out] */ BYTE **ppPolicy,
            /* [out] */ DWORD *pcbPolicy,
            /* [in] */ BYTE *pContext,
            /* [in] */ DWORD cbContext,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *SetZoneMapping )( 
            IInternetSecurityManager * This,
            /* [in] */ DWORD dwZone,
            /* [in] */ LPCWSTR lpszPattern,
            /* [in] */ DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetZoneMappings )( 
            IInternetSecurityManager * This,
            /* [in] */ DWORD dwZone,
            /* [out] */ IEnumString **ppenumString,
            /* [in] */ DWORD dwFlags);
        
        END_INTERFACE
    } IInternetSecurityManagerVtbl;

    interface IInternetSecurityManager
    {
        CONST_VTBL struct IInternetSecurityManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInternetSecurityManager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInternetSecurityManager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInternetSecurityManager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IInternetSecurityManager_SetSecuritySite(This,pSite)	\
    (This)->lpVtbl -> SetSecuritySite(This,pSite)

#define IInternetSecurityManager_GetSecuritySite(This,ppSite)	\
    (This)->lpVtbl -> GetSecuritySite(This,ppSite)

#define IInternetSecurityManager_MapUrlToZone(This,pwszUrl,pdwZone,dwFlags)	\
    (This)->lpVtbl -> MapUrlToZone(This,pwszUrl,pdwZone,dwFlags)

#define IInternetSecurityManager_GetSecurityId(This,pwszUrl,pbSecurityId,pcbSecurityId,dwReserved)	\
    (This)->lpVtbl -> GetSecurityId(This,pwszUrl,pbSecurityId,pcbSecurityId,dwReserved)

#define IInternetSecurityManager_ProcessUrlAction(This,pwszUrl,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved)	\
    (This)->lpVtbl -> ProcessUrlAction(This,pwszUrl,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved)

#define IInternetSecurityManager_QueryCustomPolicy(This,pwszUrl,guidKey,ppPolicy,pcbPolicy,pContext,cbContext,dwReserved)	\
    (This)->lpVtbl -> QueryCustomPolicy(This,pwszUrl,guidKey,ppPolicy,pcbPolicy,pContext,cbContext,dwReserved)

#define IInternetSecurityManager_SetZoneMapping(This,dwZone,lpszPattern,dwFlags)	\
    (This)->lpVtbl -> SetZoneMapping(This,dwZone,lpszPattern,dwFlags)

#define IInternetSecurityManager_GetZoneMappings(This,dwZone,ppenumString,dwFlags)	\
    (This)->lpVtbl -> GetZoneMappings(This,dwZone,ppenumString,dwFlags)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IInternetSecurityManager_SetSecuritySite_Proxy( 
    IInternetSecurityManager * This,
    /* [unique][in] */ IInternetSecurityMgrSite *pSite);


void __RPC_STUB IInternetSecurityManager_SetSecuritySite_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetSecurityManager_GetSecuritySite_Proxy( 
    IInternetSecurityManager * This,
    /* [out] */ IInternetSecurityMgrSite **ppSite);


void __RPC_STUB IInternetSecurityManager_GetSecuritySite_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetSecurityManager_MapUrlToZone_Proxy( 
    IInternetSecurityManager * This,
    /* [in] */ LPCWSTR pwszUrl,
    /* [out] */ DWORD *pdwZone,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IInternetSecurityManager_MapUrlToZone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetSecurityManager_GetSecurityId_Proxy( 
    IInternetSecurityManager * This,
    /* [in] */ LPCWSTR pwszUrl,
    /* [size_is][out] */ BYTE *pbSecurityId,
    /* [out][in] */ DWORD *pcbSecurityId,
    /* [in] */ DWORD_PTR dwReserved);


void __RPC_STUB IInternetSecurityManager_GetSecurityId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetSecurityManager_ProcessUrlAction_Proxy( 
    IInternetSecurityManager * This,
    /* [in] */ LPCWSTR pwszUrl,
    /* [in] */ DWORD dwAction,
    /* [size_is][out] */ BYTE *pPolicy,
    /* [in] */ DWORD cbPolicy,
    /* [in] */ BYTE *pContext,
    /* [in] */ DWORD cbContext,
    /* [in] */ DWORD dwFlags,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IInternetSecurityManager_ProcessUrlAction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetSecurityManager_QueryCustomPolicy_Proxy( 
    IInternetSecurityManager * This,
    /* [in] */ LPCWSTR pwszUrl,
    /* [in] */ REFGUID guidKey,
    /* [size_is][size_is][out] */ BYTE **ppPolicy,
    /* [out] */ DWORD *pcbPolicy,
    /* [in] */ BYTE *pContext,
    /* [in] */ DWORD cbContext,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IInternetSecurityManager_QueryCustomPolicy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetSecurityManager_SetZoneMapping_Proxy( 
    IInternetSecurityManager * This,
    /* [in] */ DWORD dwZone,
    /* [in] */ LPCWSTR lpszPattern,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IInternetSecurityManager_SetZoneMapping_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetSecurityManager_GetZoneMappings_Proxy( 
    IInternetSecurityManager * This,
    /* [in] */ DWORD dwZone,
    /* [out] */ IEnumString **ppenumString,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IInternetSecurityManager_GetZoneMappings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IInternetSecurityManager_INTERFACE_DEFINED__ */


#ifndef __IInternetSecurityManagerEx_INTERFACE_DEFINED__
#define __IInternetSecurityManagerEx_INTERFACE_DEFINED__

/* interface IInternetSecurityManagerEx */
/* [object][unique][helpstring][uuid] */ 

// This is the wrapper function that most clients will use.
// It figures out the current Policy for the passed in Action,
// and puts up UI if the current Policy indicates that the user
// should be queried. It returns back the Policy which the caller
// will use to determine if the action should be allowed

EXTERN_C const IID IID_IInternetSecurityManagerEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F164EDF1-CC7C-4f0d-9A94-34222625C393")
    IInternetSecurityManagerEx : public IInternetSecurityManager
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ProcessUrlActionEx( 
            /* [in] */ LPCWSTR pwszUrl,
            /* [in] */ DWORD dwAction,
            /* [size_is][out] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ BYTE *pContext,
            /* [in] */ DWORD cbContext,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwReserved,
            /* [out] */ DWORD *pdwOutFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInternetSecurityManagerExVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInternetSecurityManagerEx * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInternetSecurityManagerEx * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInternetSecurityManagerEx * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetSecuritySite )( 
            IInternetSecurityManagerEx * This,
            /* [unique][in] */ IInternetSecurityMgrSite *pSite);
        
        HRESULT ( STDMETHODCALLTYPE *GetSecuritySite )( 
            IInternetSecurityManagerEx * This,
            /* [out] */ IInternetSecurityMgrSite **ppSite);
        
        HRESULT ( STDMETHODCALLTYPE *MapUrlToZone )( 
            IInternetSecurityManagerEx * This,
            /* [in] */ LPCWSTR pwszUrl,
            /* [out] */ DWORD *pdwZone,
            /* [in] */ DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetSecurityId )( 
            IInternetSecurityManagerEx * This,
            /* [in] */ LPCWSTR pwszUrl,
            /* [size_is][out] */ BYTE *pbSecurityId,
            /* [out][in] */ DWORD *pcbSecurityId,
            /* [in] */ DWORD_PTR dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *ProcessUrlAction )( 
            IInternetSecurityManagerEx * This,
            /* [in] */ LPCWSTR pwszUrl,
            /* [in] */ DWORD dwAction,
            /* [size_is][out] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ BYTE *pContext,
            /* [in] */ DWORD cbContext,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *QueryCustomPolicy )( 
            IInternetSecurityManagerEx * This,
            /* [in] */ LPCWSTR pwszUrl,
            /* [in] */ REFGUID guidKey,
            /* [size_is][size_is][out] */ BYTE **ppPolicy,
            /* [out] */ DWORD *pcbPolicy,
            /* [in] */ BYTE *pContext,
            /* [in] */ DWORD cbContext,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *SetZoneMapping )( 
            IInternetSecurityManagerEx * This,
            /* [in] */ DWORD dwZone,
            /* [in] */ LPCWSTR lpszPattern,
            /* [in] */ DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetZoneMappings )( 
            IInternetSecurityManagerEx * This,
            /* [in] */ DWORD dwZone,
            /* [out] */ IEnumString **ppenumString,
            /* [in] */ DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *ProcessUrlActionEx )( 
            IInternetSecurityManagerEx * This,
            /* [in] */ LPCWSTR pwszUrl,
            /* [in] */ DWORD dwAction,
            /* [size_is][out] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ BYTE *pContext,
            /* [in] */ DWORD cbContext,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwReserved,
            /* [out] */ DWORD *pdwOutFlags);
        
        END_INTERFACE
    } IInternetSecurityManagerExVtbl;

    interface IInternetSecurityManagerEx
    {
        CONST_VTBL struct IInternetSecurityManagerExVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInternetSecurityManagerEx_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInternetSecurityManagerEx_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInternetSecurityManagerEx_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IInternetSecurityManagerEx_SetSecuritySite(This,pSite)	\
    (This)->lpVtbl -> SetSecuritySite(This,pSite)

#define IInternetSecurityManagerEx_GetSecuritySite(This,ppSite)	\
    (This)->lpVtbl -> GetSecuritySite(This,ppSite)

#define IInternetSecurityManagerEx_MapUrlToZone(This,pwszUrl,pdwZone,dwFlags)	\
    (This)->lpVtbl -> MapUrlToZone(This,pwszUrl,pdwZone,dwFlags)

#define IInternetSecurityManagerEx_GetSecurityId(This,pwszUrl,pbSecurityId,pcbSecurityId,dwReserved)	\
    (This)->lpVtbl -> GetSecurityId(This,pwszUrl,pbSecurityId,pcbSecurityId,dwReserved)

#define IInternetSecurityManagerEx_ProcessUrlAction(This,pwszUrl,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved)	\
    (This)->lpVtbl -> ProcessUrlAction(This,pwszUrl,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved)

#define IInternetSecurityManagerEx_QueryCustomPolicy(This,pwszUrl,guidKey,ppPolicy,pcbPolicy,pContext,cbContext,dwReserved)	\
    (This)->lpVtbl -> QueryCustomPolicy(This,pwszUrl,guidKey,ppPolicy,pcbPolicy,pContext,cbContext,dwReserved)

#define IInternetSecurityManagerEx_SetZoneMapping(This,dwZone,lpszPattern,dwFlags)	\
    (This)->lpVtbl -> SetZoneMapping(This,dwZone,lpszPattern,dwFlags)

#define IInternetSecurityManagerEx_GetZoneMappings(This,dwZone,ppenumString,dwFlags)	\
    (This)->lpVtbl -> GetZoneMappings(This,dwZone,ppenumString,dwFlags)


#define IInternetSecurityManagerEx_ProcessUrlActionEx(This,pwszUrl,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved,pdwOutFlags)	\
    (This)->lpVtbl -> ProcessUrlActionEx(This,pwszUrl,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved,pdwOutFlags)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IInternetSecurityManagerEx_ProcessUrlActionEx_Proxy( 
    IInternetSecurityManagerEx * This,
    /* [in] */ LPCWSTR pwszUrl,
    /* [in] */ DWORD dwAction,
    /* [size_is][out] */ BYTE *pPolicy,
    /* [in] */ DWORD cbPolicy,
    /* [in] */ BYTE *pContext,
    /* [in] */ DWORD cbContext,
    /* [in] */ DWORD dwFlags,
    /* [in] */ DWORD dwReserved,
    /* [out] */ DWORD *pdwOutFlags);


void __RPC_STUB IInternetSecurityManagerEx_ProcessUrlActionEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IInternetSecurityManagerEx_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0205 */
/* [local] */ 

#endif
#ifndef _LPINTERNETSECURITYMANANGER_DEFINED
#define _LPINTERNETSECURITYMANANGER_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0205_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0205_v0_0_s_ifspec;

#ifndef __IZoneIdentifier_INTERFACE_DEFINED__
#define __IZoneIdentifier_INTERFACE_DEFINED__

/* interface IZoneIdentifier */
/* [unique][uuid][object][local] */ 


EXTERN_C const IID IID_IZoneIdentifier;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("cd45f185-1b21-48e2-967b-ead743a8914e")
    IZoneIdentifier : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetId( 
            /* [out] */ DWORD *pdwZone) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetId( 
            /* [in] */ DWORD dwZone) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Remove( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IZoneIdentifierVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IZoneIdentifier * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IZoneIdentifier * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IZoneIdentifier * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetId )( 
            IZoneIdentifier * This,
            /* [out] */ DWORD *pdwZone);
        
        HRESULT ( STDMETHODCALLTYPE *SetId )( 
            IZoneIdentifier * This,
            /* [in] */ DWORD dwZone);
        
        HRESULT ( STDMETHODCALLTYPE *Remove )( 
            IZoneIdentifier * This);
        
        END_INTERFACE
    } IZoneIdentifierVtbl;

    interface IZoneIdentifier
    {
        CONST_VTBL struct IZoneIdentifierVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IZoneIdentifier_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IZoneIdentifier_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IZoneIdentifier_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IZoneIdentifier_GetId(This,pdwZone)	\
    (This)->lpVtbl -> GetId(This,pdwZone)

#define IZoneIdentifier_SetId(This,dwZone)	\
    (This)->lpVtbl -> SetId(This,dwZone)

#define IZoneIdentifier_Remove(This)	\
    (This)->lpVtbl -> Remove(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IZoneIdentifier_GetId_Proxy( 
    IZoneIdentifier * This,
    /* [out] */ DWORD *pdwZone);


void __RPC_STUB IZoneIdentifier_GetId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IZoneIdentifier_SetId_Proxy( 
    IZoneIdentifier * This,
    /* [in] */ DWORD dwZone);


void __RPC_STUB IZoneIdentifier_SetId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IZoneIdentifier_Remove_Proxy( 
    IZoneIdentifier * This);


void __RPC_STUB IZoneIdentifier_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IZoneIdentifier_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0207 */
/* [local] */ 

#endif
#ifndef _LPINTERNETHOSTSECURITYMANANGER_DEFINED
#define _LPINTERNETHOSTSECURITYMANANGER_DEFINED
//This is the interface MSHTML exposes to its clients
//The clients need not pass in a URL to these functions
//since MSHTML maintains the notion of the current URL


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0207_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0207_v0_0_s_ifspec;

#ifndef __IInternetHostSecurityManager_INTERFACE_DEFINED__
#define __IInternetHostSecurityManager_INTERFACE_DEFINED__

/* interface IInternetHostSecurityManager */
/* [unique][helpstring][uuid][object][local] */ 


EXTERN_C const IID IID_IInternetHostSecurityManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3af280b6-cb3f-11d0-891e-00c04fb6bfc4")
    IInternetHostSecurityManager : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetSecurityId( 
            /* [size_is][out] */ BYTE *pbSecurityId,
            /* [out][in] */ DWORD *pcbSecurityId,
            /* [in] */ DWORD_PTR dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ProcessUrlAction( 
            /* [in] */ DWORD dwAction,
            /* [size_is][out] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ BYTE *pContext,
            /* [in] */ DWORD cbContext,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryCustomPolicy( 
            /* [in] */ REFGUID guidKey,
            /* [size_is][size_is][out] */ BYTE **ppPolicy,
            /* [out] */ DWORD *pcbPolicy,
            /* [in] */ BYTE *pContext,
            /* [in] */ DWORD cbContext,
            /* [in] */ DWORD dwReserved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInternetHostSecurityManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInternetHostSecurityManager * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInternetHostSecurityManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInternetHostSecurityManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSecurityId )( 
            IInternetHostSecurityManager * This,
            /* [size_is][out] */ BYTE *pbSecurityId,
            /* [out][in] */ DWORD *pcbSecurityId,
            /* [in] */ DWORD_PTR dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *ProcessUrlAction )( 
            IInternetHostSecurityManager * This,
            /* [in] */ DWORD dwAction,
            /* [size_is][out] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ BYTE *pContext,
            /* [in] */ DWORD cbContext,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *QueryCustomPolicy )( 
            IInternetHostSecurityManager * This,
            /* [in] */ REFGUID guidKey,
            /* [size_is][size_is][out] */ BYTE **ppPolicy,
            /* [out] */ DWORD *pcbPolicy,
            /* [in] */ BYTE *pContext,
            /* [in] */ DWORD cbContext,
            /* [in] */ DWORD dwReserved);
        
        END_INTERFACE
    } IInternetHostSecurityManagerVtbl;

    interface IInternetHostSecurityManager
    {
        CONST_VTBL struct IInternetHostSecurityManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInternetHostSecurityManager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInternetHostSecurityManager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInternetHostSecurityManager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IInternetHostSecurityManager_GetSecurityId(This,pbSecurityId,pcbSecurityId,dwReserved)	\
    (This)->lpVtbl -> GetSecurityId(This,pbSecurityId,pcbSecurityId,dwReserved)

#define IInternetHostSecurityManager_ProcessUrlAction(This,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved)	\
    (This)->lpVtbl -> ProcessUrlAction(This,dwAction,pPolicy,cbPolicy,pContext,cbContext,dwFlags,dwReserved)

#define IInternetHostSecurityManager_QueryCustomPolicy(This,guidKey,ppPolicy,pcbPolicy,pContext,cbContext,dwReserved)	\
    (This)->lpVtbl -> QueryCustomPolicy(This,guidKey,ppPolicy,pcbPolicy,pContext,cbContext,dwReserved)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IInternetHostSecurityManager_GetSecurityId_Proxy( 
    IInternetHostSecurityManager * This,
    /* [size_is][out] */ BYTE *pbSecurityId,
    /* [out][in] */ DWORD *pcbSecurityId,
    /* [in] */ DWORD_PTR dwReserved);


void __RPC_STUB IInternetHostSecurityManager_GetSecurityId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetHostSecurityManager_ProcessUrlAction_Proxy( 
    IInternetHostSecurityManager * This,
    /* [in] */ DWORD dwAction,
    /* [size_is][out] */ BYTE *pPolicy,
    /* [in] */ DWORD cbPolicy,
    /* [in] */ BYTE *pContext,
    /* [in] */ DWORD cbContext,
    /* [in] */ DWORD dwFlags,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IInternetHostSecurityManager_ProcessUrlAction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetHostSecurityManager_QueryCustomPolicy_Proxy( 
    IInternetHostSecurityManager * This,
    /* [in] */ REFGUID guidKey,
    /* [size_is][size_is][out] */ BYTE **ppPolicy,
    /* [out] */ DWORD *pcbPolicy,
    /* [in] */ BYTE *pContext,
    /* [in] */ DWORD cbContext,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IInternetHostSecurityManager_QueryCustomPolicy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IInternetHostSecurityManager_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0208 */
/* [local] */ 

#endif

// The zone manager maintains policies for a set of standard actions. 
// These actions are identified by integral values (called action indexes)
// specified below.

// Minimum legal value for an action    
#define URLACTION_MIN                                          0x00001000

#define URLACTION_DOWNLOAD_MIN                                 0x00001000
#define URLACTION_DOWNLOAD_SIGNED_ACTIVEX                      0x00001001
#define URLACTION_DOWNLOAD_UNSIGNED_ACTIVEX                    0x00001004
#define URLACTION_DOWNLOAD_CURR_MAX                            0x00001004
#define URLACTION_DOWNLOAD_MAX                                 0x000011FF

#define URLACTION_ACTIVEX_MIN                                  0x00001200
#define URLACTION_ACTIVEX_RUN                                  0x00001200
#define URLPOLICY_ACTIVEX_CHECK_LIST                           0x00010000
#define URLACTION_ACTIVEX_OVERRIDE_OBJECT_SAFETY               0x00001201 // aggregate next four
#define URLACTION_ACTIVEX_OVERRIDE_DATA_SAFETY                 0x00001202 //
#define URLACTION_ACTIVEX_OVERRIDE_SCRIPT_SAFETY               0x00001203 //
#define URLACTION_SCRIPT_OVERRIDE_SAFETY                       0x00001401 //
#define URLACTION_ACTIVEX_CONFIRM_NOOBJECTSAFETY               0x00001204 //
#define URLACTION_ACTIVEX_TREATASUNTRUSTED                     0x00001205
#define URLACTION_ACTIVEX_NO_WEBOC_SCRIPT                      0x00001206
#define URLACTION_ACTIVEX_CURR_MAX                             0x00001206
#define URLACTION_ACTIVEX_MAX                                  0x000013ff

#define URLACTION_SCRIPT_MIN                                   0x00001400
#define URLACTION_SCRIPT_RUN                                   0x00001400
#define URLACTION_SCRIPT_JAVA_USE                              0x00001402
#define URLACTION_SCRIPT_SAFE_ACTIVEX                          0x00001405
#define URLACTION_CROSS_DOMAIN_DATA                            0x00001406
#define URLACTION_SCRIPT_PASTE                                 0x00001407
#define URLACTION_SCRIPT_CURR_MAX                              0x00001407
#define URLACTION_SCRIPT_MAX                                   0x000015ff

#define URLACTION_HTML_MIN                                     0x00001600
#define URLACTION_HTML_SUBMIT_FORMS                            0x00001601 // aggregate next two
#define URLACTION_HTML_SUBMIT_FORMS_FROM                       0x00001602 //
#define URLACTION_HTML_SUBMIT_FORMS_TO                         0x00001603 //
#define URLACTION_HTML_FONT_DOWNLOAD                           0x00001604
#define URLACTION_HTML_JAVA_RUN                                0x00001605 // derive from Java custom policy
#define URLACTION_HTML_USERDATA_SAVE                           0x00001606
#define URLACTION_HTML_SUBFRAME_NAVIGATE                       0x00001607
#define URLACTION_HTML_META_REFRESH                            0x00001608
#define URLACTION_HTML_MIXED_CONTENT                           0x00001609
#define URLACTION_HTML_MAX                                     0x000017ff

#define URLACTION_SHELL_MIN                                    0x00001800
#define URLACTION_SHELL_INSTALL_DTITEMS                        0x00001800
#define URLACTION_SHELL_MOVE_OR_COPY                           0x00001802
#define URLACTION_SHELL_FILE_DOWNLOAD                          0x00001803
#define URLACTION_SHELL_VERB                                   0x00001804
#define URLACTION_SHELL_WEBVIEW_VERB                           0x00001805
#define URLACTION_SHELL_SHELLEXECUTE                           0x00001806
#define URLACTION_SHELL_EXECUTE_HIGHRISK                       0x00001806
#define URLACTION_SHELL_EXECUTE_MODRISK                        0x00001807
#define URLACTION_SHELL_EXECUTE_LOWRISK                        0x00001808
#define URLACTION_SHELL_POPUPMGR                               0x00001809
#define URLACTION_SHELL_RTF_OBJECTS_LOAD                       0x0000180A
#define URLACTION_SHELL_ENHANCED_DRAGDROP_SECURITY             0x0000180B
#define URLACTION_SHELL_CURR_MAX                               0x0000180B
#define URLACTION_SHELL_MAX                                    0x000019ff

#define URLACTION_NETWORK_MIN                                  0x00001A00

#define URLACTION_CREDENTIALS_USE                              0x00001A00
#define URLPOLICY_CREDENTIALS_SILENT_LOGON_OK        0x00000000
#define URLPOLICY_CREDENTIALS_MUST_PROMPT_USER       0x00010000
#define URLPOLICY_CREDENTIALS_CONDITIONAL_PROMPT     0x00020000
#define URLPOLICY_CREDENTIALS_ANONYMOUS_ONLY         0x00030000

#define URLACTION_AUTHENTICATE_CLIENT                          0x00001A01
#define URLPOLICY_AUTHENTICATE_CLEARTEXT_OK          0x00000000
#define URLPOLICY_AUTHENTICATE_CHALLENGE_RESPONSE    0x00010000
#define URLPOLICY_AUTHENTICATE_MUTUAL_ONLY           0x00030000


#define URLACTION_COOKIES                                      0x00001A02
#define URLACTION_COOKIES_SESSION                              0x00001A03

#define URLACTION_CLIENT_CERT_PROMPT                           0x00001A04

#define URLACTION_COOKIES_THIRD_PARTY                          0x00001A05
#define URLACTION_COOKIES_SESSION_THIRD_PARTY                  0x00001A06

#define URLACTION_COOKIES_ENABLED                              0x00001A10

#define URLACTION_NETWORK_CURR_MAX                             0x00001A10
#define URLACTION_NETWORK_MAX                                  0x00001Bff


#define URLACTION_JAVA_MIN                                     0x00001C00
#define URLACTION_JAVA_PERMISSIONS                             0x00001C00
#define URLPOLICY_JAVA_PROHIBIT                      0x00000000
#define URLPOLICY_JAVA_HIGH                          0x00010000
#define URLPOLICY_JAVA_MEDIUM                        0x00020000
#define URLPOLICY_JAVA_LOW                           0x00030000
#define URLPOLICY_JAVA_CUSTOM                        0x00800000
#define URLACTION_JAVA_CURR_MAX                                0x00001C00
#define URLACTION_JAVA_MAX                                     0x00001Cff


// The following Infodelivery actions should have no default policies
// in the registry.  They assume that no default policy means fall
// back to the global restriction.  If an admin sets a policy per
// zone, then it overrides the global restriction.

#define URLACTION_INFODELIVERY_MIN                           0x00001D00
#define URLACTION_INFODELIVERY_NO_ADDING_CHANNELS            0x00001D00
#define URLACTION_INFODELIVERY_NO_EDITING_CHANNELS           0x00001D01
#define URLACTION_INFODELIVERY_NO_REMOVING_CHANNELS          0x00001D02
#define URLACTION_INFODELIVERY_NO_ADDING_SUBSCRIPTIONS       0x00001D03
#define URLACTION_INFODELIVERY_NO_EDITING_SUBSCRIPTIONS      0x00001D04
#define URLACTION_INFODELIVERY_NO_REMOVING_SUBSCRIPTIONS     0x00001D05
#define URLACTION_INFODELIVERY_NO_CHANNEL_LOGGING            0x00001D06
#define URLACTION_INFODELIVERY_CURR_MAX                      0x00001D06
#define URLACTION_INFODELIVERY_MAX                           0x00001Dff
#define URLACTION_CHANNEL_SOFTDIST_MIN                       0x00001E00
#define URLACTION_CHANNEL_SOFTDIST_PERMISSIONS               0x00001E05
#define URLPOLICY_CHANNEL_SOFTDIST_PROHIBIT          0x00010000
#define URLPOLICY_CHANNEL_SOFTDIST_PRECACHE          0x00020000
#define URLPOLICY_CHANNEL_SOFTDIST_AUTOINSTALL       0x00030000
#define URLACTION_CHANNEL_SOFTDIST_MAX                       0x00001Eff
#define URLACTION_BEHAVIOR_MIN                               0x00002000
#define URLACTION_BEHAVIOR_RUN                               0x00002000
#define URLPOLICY_BEHAVIOR_CHECK_LIST                        0x00010000

// The following actions correspond to the Feature options above.
// However, they are NOT in the same order.
#define URLACTION_FEATURE_MIN                                0x00002100
#define URLACTION_FEATURE_MIME_SNIFFING                      0x00002100
#define URLACTION_FEATURE_ZONE_ELEVATION                     0x00002101
#define URLACTION_FEATURE_WINDOW_RESTRICTIONS                0x00002102

#define URLACTION_AUTOMATIC_DOWNLOAD_UI_MIN                  0x00002200
#define URLACTION_AUTOMATIC_DOWNLOAD_UI                      0x00002200
#define URLACTION_AUTOMATIC_ACTIVEX_UI                       0x00002201

#define URLACTION_ALLOW_RESTRICTEDPROTOCOLS                0x00002300

// For each action specified above the system maintains
// a set of policies for the action. 
// The only policies supported currently are permissions (i.e. is something allowed)
// and logging status. 
// IMPORTANT: If you are defining your own policies don't overload the meaning of the
// loword of the policy. You can use the hiword to store any policy bits which are only
// meaningful to your action.
// For an example of how to do this look at the URLPOLICY_JAVA above

// Permissions 
#define URLPOLICY_ALLOW                0x00
#define URLPOLICY_QUERY                0x01
#define URLPOLICY_DISALLOW             0x03

// Notifications are not done when user already queried.
#define URLPOLICY_NOTIFY_ON_ALLOW      0x10
#define URLPOLICY_NOTIFY_ON_DISALLOW   0x20

// Logging is done regardless of whether user was queried.
#define URLPOLICY_LOG_ON_ALLOW         0x40
#define URLPOLICY_LOG_ON_DISALLOW      0x80

#define URLPOLICY_MASK_PERMISSIONS     0x0f
#define GetUrlPolicyPermissions(dw)        (dw & URLPOLICY_MASK_PERMISSIONS)
#define SetUrlPolicyPermissions(dw,dw2)    ((dw) = ((dw) & ~(URLPOLICY_MASK_PERMISSIONS)) | (dw2))


#define URLPOLICY_DONTCHECKDLGBOX     0x100
// The ordinal #'s that define the predefined zones internet explorer knows about. 
// When we support user-defined zones their zone numbers should be between 
// URLZONE_USER_MIN and URLZONE_USER_MAX
// Custom policy to query whether the local machine zone
// has been unlocked for current document.
EXTERN_C const GUID GUID_CUSTOM_LOCALMACHINEZONEUNLOCKED; 
#ifndef _LPINTERNETZONEMANAGER_DEFINED
#define _LPINTERNETZONEMANAGER_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0208_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0208_v0_0_s_ifspec;

#ifndef __IInternetZoneManager_INTERFACE_DEFINED__
#define __IInternetZoneManager_INTERFACE_DEFINED__

/* interface IInternetZoneManager */
/* [unique][helpstring][uuid][object][local] */ 

typedef /* [unique] */ IInternetZoneManager *LPURLZONEMANAGER;

typedef 
enum tagURLZONE
    {	URLZONE_PREDEFINED_MIN	= 0,
	URLZONE_LOCAL_MACHINE	= 0,
	URLZONE_INTRANET	= URLZONE_LOCAL_MACHINE + 1,
	URLZONE_TRUSTED	= URLZONE_INTRANET + 1,
	URLZONE_INTERNET	= URLZONE_TRUSTED + 1,
	URLZONE_UNTRUSTED	= URLZONE_INTERNET + 1,
	URLZONE_PREDEFINED_MAX	= 999,
	URLZONE_USER_MIN	= 1000,
	URLZONE_USER_MAX	= 10000
    } 	URLZONE;

// Enhanced Security Configuration zone mapping flag for IInternetSecurityManager::SetZoneMapping
#define URLZONE_ESC_FLAG     0x100
typedef 
enum tagURLTEMPLATE
    {	URLTEMPLATE_CUSTOM	= 0,
	URLTEMPLATE_PREDEFINED_MIN	= 0x10000,
	URLTEMPLATE_LOW	= 0x10000,
	URLTEMPLATE_MEDLOW	= 0x10500,
	URLTEMPLATE_MEDIUM	= 0x11000,
	URLTEMPLATE_HIGH	= 0x12000,
	URLTEMPLATE_PREDEFINED_MAX	= 0x20000
    } 	URLTEMPLATE;


enum __MIDL_IInternetZoneManager_0001
    {	MAX_ZONE_PATH	= 260,
	MAX_ZONE_DESCRIPTION	= 200
    } ;
typedef /* [public] */ 
enum __MIDL_IInternetZoneManager_0002
    {	ZAFLAGS_CUSTOM_EDIT	= 0x1,
	ZAFLAGS_ADD_SITES	= 0x2,
	ZAFLAGS_REQUIRE_VERIFICATION	= 0x4,
	ZAFLAGS_INCLUDE_PROXY_OVERRIDE	= 0x8,
	ZAFLAGS_INCLUDE_INTRANET_SITES	= 0x10,
	ZAFLAGS_NO_UI	= 0x20,
	ZAFLAGS_SUPPORTS_VERIFICATION	= 0x40,
	ZAFLAGS_UNC_AS_INTRANET	= 0x80,
	ZAFLAGS_USE_LOCKED_ZONES	= 0x10000
    } 	ZAFLAGS;

typedef struct _ZONEATTRIBUTES
    {
    ULONG cbSize;
    WCHAR szDisplayName[ 260 ];
    WCHAR szDescription[ 200 ];
    WCHAR szIconPath[ 260 ];
    DWORD dwTemplateMinLevel;
    DWORD dwTemplateRecommended;
    DWORD dwTemplateCurrentLevel;
    DWORD dwFlags;
    } 	ZONEATTRIBUTES;

typedef struct _ZONEATTRIBUTES *LPZONEATTRIBUTES;

// Gets the zone attributes (information in registry other than actual security
// policies associated with the zone).  Zone attributes are fixed as:
// Sets the zone attributes (information in registry other than actual security
// policies associated with the zone).  Zone attributes as above.
// Returns S_OK or ??? if failed to write the zone attributes.
/* Registry Flags

    When reading, default behavior is:
        If HKLM allows override and HKCU value exists
            Then use HKCU value
            Else use HKLM value
    When writing, default behavior is same as HKCU
        If HKLM allows override
           Then Write to HKCU
           Else Fail
*/
typedef 
enum _URLZONEREG
    {	URLZONEREG_DEFAULT	= 0,
	URLZONEREG_HKLM	= URLZONEREG_DEFAULT + 1,
	URLZONEREG_HKCU	= URLZONEREG_HKLM + 1
    } 	URLZONEREG;

// Gets a named custom policy associated with a zone;
// e.g. the Java VM settings can be defined with a unique key such as 'Java'.
// Custom policy support is intended to allow extensibility from the predefined
// set of policies that IE4 has built in.
// 
// pwszKey is the string name designating the custom policy.  Components are
//   responsible for having unique names.
// ppPolicy is the callee allocated buffer for the policy byte blob; caller is
//   responsible for freeing this buffer eventually.
// pcbPolicy is the size of the byte blob returned.
// dwRegFlags determines how registry is accessed (see above).
// Returns S_OK if key is found and buffer allocated; ??? if key is not found (no buffer alloced).
// Sets a named custom policy associated with a zone;
// e.g. the Java VM settings can be defined with a unique key such as 'Java'.
// Custom policy support is intended to allow extensibility from the predefined
// set of policies that IE4 has built in.  
// 
// pwszKey is the string name designating the custom policy.  Components are
//   responsible for having unique names.
// ppPolicy is the caller allocated buffer for the policy byte blob.
// pcbPolicy is the size of the byte blob to be set.
// dwRegFlags determines if HTCU or HKLM is set.
// Returns S_OK or ??? if failed to write the zone custom policy.
// Gets action policy associated with a zone, the builtin, fixed-length policies info.

// dwAction is the action code for the action as defined above.
// pPolicy is the caller allocated buffer for the policy data.
// cbPolicy is the size of the caller allocated buffer.
// dwRegFlags determines how registry is accessed (see above).
// Returns S_OK if action is valid; ??? if action is not valid.

EXTERN_C const IID IID_IInternetZoneManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79eac9ef-baf9-11ce-8c82-00aa004ba90b")
    IInternetZoneManager : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetZoneAttributes( 
            /* [in] */ DWORD dwZone,
            /* [unique][out][in] */ ZONEATTRIBUTES *pZoneAttributes) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetZoneAttributes( 
            /* [in] */ DWORD dwZone,
            /* [in] */ ZONEATTRIBUTES *pZoneAttributes) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetZoneCustomPolicy( 
            /* [in] */ DWORD dwZone,
            /* [in] */ REFGUID guidKey,
            /* [size_is][size_is][out] */ BYTE **ppPolicy,
            /* [out] */ DWORD *pcbPolicy,
            /* [in] */ URLZONEREG urlZoneReg) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetZoneCustomPolicy( 
            /* [in] */ DWORD dwZone,
            /* [in] */ REFGUID guidKey,
            /* [size_is][in] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ URLZONEREG urlZoneReg) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetZoneActionPolicy( 
            /* [in] */ DWORD dwZone,
            /* [in] */ DWORD dwAction,
            /* [size_is][out] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ URLZONEREG urlZoneReg) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetZoneActionPolicy( 
            /* [in] */ DWORD dwZone,
            /* [in] */ DWORD dwAction,
            /* [size_is][in] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ URLZONEREG urlZoneReg) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PromptAction( 
            /* [in] */ DWORD dwAction,
            /* [in] */ HWND hwndParent,
            /* [in] */ LPCWSTR pwszUrl,
            /* [in] */ LPCWSTR pwszText,
            /* [in] */ DWORD dwPromptFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LogAction( 
            /* [in] */ DWORD dwAction,
            /* [in] */ LPCWSTR pwszUrl,
            /* [in] */ LPCWSTR pwszText,
            /* [in] */ DWORD dwLogFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateZoneEnumerator( 
            /* [out] */ DWORD *pdwEnum,
            /* [out] */ DWORD *pdwCount,
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetZoneAt( 
            /* [in] */ DWORD dwEnum,
            /* [in] */ DWORD dwIndex,
            /* [out] */ DWORD *pdwZone) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DestroyZoneEnumerator( 
            /* [in] */ DWORD dwEnum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CopyTemplatePoliciesToZone( 
            /* [in] */ DWORD dwTemplate,
            /* [in] */ DWORD dwZone,
            /* [in] */ DWORD dwReserved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInternetZoneManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInternetZoneManager * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInternetZoneManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInternetZoneManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetZoneAttributes )( 
            IInternetZoneManager * This,
            /* [in] */ DWORD dwZone,
            /* [unique][out][in] */ ZONEATTRIBUTES *pZoneAttributes);
        
        HRESULT ( STDMETHODCALLTYPE *SetZoneAttributes )( 
            IInternetZoneManager * This,
            /* [in] */ DWORD dwZone,
            /* [in] */ ZONEATTRIBUTES *pZoneAttributes);
        
        HRESULT ( STDMETHODCALLTYPE *GetZoneCustomPolicy )( 
            IInternetZoneManager * This,
            /* [in] */ DWORD dwZone,
            /* [in] */ REFGUID guidKey,
            /* [size_is][size_is][out] */ BYTE **ppPolicy,
            /* [out] */ DWORD *pcbPolicy,
            /* [in] */ URLZONEREG urlZoneReg);
        
        HRESULT ( STDMETHODCALLTYPE *SetZoneCustomPolicy )( 
            IInternetZoneManager * This,
            /* [in] */ DWORD dwZone,
            /* [in] */ REFGUID guidKey,
            /* [size_is][in] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ URLZONEREG urlZoneReg);
        
        HRESULT ( STDMETHODCALLTYPE *GetZoneActionPolicy )( 
            IInternetZoneManager * This,
            /* [in] */ DWORD dwZone,
            /* [in] */ DWORD dwAction,
            /* [size_is][out] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ URLZONEREG urlZoneReg);
        
        HRESULT ( STDMETHODCALLTYPE *SetZoneActionPolicy )( 
            IInternetZoneManager * This,
            /* [in] */ DWORD dwZone,
            /* [in] */ DWORD dwAction,
            /* [size_is][in] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ URLZONEREG urlZoneReg);
        
        HRESULT ( STDMETHODCALLTYPE *PromptAction )( 
            IInternetZoneManager * This,
            /* [in] */ DWORD dwAction,
            /* [in] */ HWND hwndParent,
            /* [in] */ LPCWSTR pwszUrl,
            /* [in] */ LPCWSTR pwszText,
            /* [in] */ DWORD dwPromptFlags);
        
        HRESULT ( STDMETHODCALLTYPE *LogAction )( 
            IInternetZoneManager * This,
            /* [in] */ DWORD dwAction,
            /* [in] */ LPCWSTR pwszUrl,
            /* [in] */ LPCWSTR pwszText,
            /* [in] */ DWORD dwLogFlags);
        
        HRESULT ( STDMETHODCALLTYPE *CreateZoneEnumerator )( 
            IInternetZoneManager * This,
            /* [out] */ DWORD *pdwEnum,
            /* [out] */ DWORD *pdwCount,
            /* [in] */ DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetZoneAt )( 
            IInternetZoneManager * This,
            /* [in] */ DWORD dwEnum,
            /* [in] */ DWORD dwIndex,
            /* [out] */ DWORD *pdwZone);
        
        HRESULT ( STDMETHODCALLTYPE *DestroyZoneEnumerator )( 
            IInternetZoneManager * This,
            /* [in] */ DWORD dwEnum);
        
        HRESULT ( STDMETHODCALLTYPE *CopyTemplatePoliciesToZone )( 
            IInternetZoneManager * This,
            /* [in] */ DWORD dwTemplate,
            /* [in] */ DWORD dwZone,
            /* [in] */ DWORD dwReserved);
        
        END_INTERFACE
    } IInternetZoneManagerVtbl;

    interface IInternetZoneManager
    {
        CONST_VTBL struct IInternetZoneManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInternetZoneManager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInternetZoneManager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInternetZoneManager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IInternetZoneManager_GetZoneAttributes(This,dwZone,pZoneAttributes)	\
    (This)->lpVtbl -> GetZoneAttributes(This,dwZone,pZoneAttributes)

#define IInternetZoneManager_SetZoneAttributes(This,dwZone,pZoneAttributes)	\
    (This)->lpVtbl -> SetZoneAttributes(This,dwZone,pZoneAttributes)

#define IInternetZoneManager_GetZoneCustomPolicy(This,dwZone,guidKey,ppPolicy,pcbPolicy,urlZoneReg)	\
    (This)->lpVtbl -> GetZoneCustomPolicy(This,dwZone,guidKey,ppPolicy,pcbPolicy,urlZoneReg)

#define IInternetZoneManager_SetZoneCustomPolicy(This,dwZone,guidKey,pPolicy,cbPolicy,urlZoneReg)	\
    (This)->lpVtbl -> SetZoneCustomPolicy(This,dwZone,guidKey,pPolicy,cbPolicy,urlZoneReg)

#define IInternetZoneManager_GetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg)	\
    (This)->lpVtbl -> GetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg)

#define IInternetZoneManager_SetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg)	\
    (This)->lpVtbl -> SetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg)

#define IInternetZoneManager_PromptAction(This,dwAction,hwndParent,pwszUrl,pwszText,dwPromptFlags)	\
    (This)->lpVtbl -> PromptAction(This,dwAction,hwndParent,pwszUrl,pwszText,dwPromptFlags)

#define IInternetZoneManager_LogAction(This,dwAction,pwszUrl,pwszText,dwLogFlags)	\
    (This)->lpVtbl -> LogAction(This,dwAction,pwszUrl,pwszText,dwLogFlags)

#define IInternetZoneManager_CreateZoneEnumerator(This,pdwEnum,pdwCount,dwFlags)	\
    (This)->lpVtbl -> CreateZoneEnumerator(This,pdwEnum,pdwCount,dwFlags)

#define IInternetZoneManager_GetZoneAt(This,dwEnum,dwIndex,pdwZone)	\
    (This)->lpVtbl -> GetZoneAt(This,dwEnum,dwIndex,pdwZone)

#define IInternetZoneManager_DestroyZoneEnumerator(This,dwEnum)	\
    (This)->lpVtbl -> DestroyZoneEnumerator(This,dwEnum)

#define IInternetZoneManager_CopyTemplatePoliciesToZone(This,dwTemplate,dwZone,dwReserved)	\
    (This)->lpVtbl -> CopyTemplatePoliciesToZone(This,dwTemplate,dwZone,dwReserved)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IInternetZoneManager_GetZoneAttributes_Proxy( 
    IInternetZoneManager * This,
    /* [in] */ DWORD dwZone,
    /* [unique][out][in] */ ZONEATTRIBUTES *pZoneAttributes);


void __RPC_STUB IInternetZoneManager_GetZoneAttributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetZoneManager_SetZoneAttributes_Proxy( 
    IInternetZoneManager * This,
    /* [in] */ DWORD dwZone,
    /* [in] */ ZONEATTRIBUTES *pZoneAttributes);


void __RPC_STUB IInternetZoneManager_SetZoneAttributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetZoneManager_GetZoneCustomPolicy_Proxy( 
    IInternetZoneManager * This,
    /* [in] */ DWORD dwZone,
    /* [in] */ REFGUID guidKey,
    /* [size_is][size_is][out] */ BYTE **ppPolicy,
    /* [out] */ DWORD *pcbPolicy,
    /* [in] */ URLZONEREG urlZoneReg);


void __RPC_STUB IInternetZoneManager_GetZoneCustomPolicy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetZoneManager_SetZoneCustomPolicy_Proxy( 
    IInternetZoneManager * This,
    /* [in] */ DWORD dwZone,
    /* [in] */ REFGUID guidKey,
    /* [size_is][in] */ BYTE *pPolicy,
    /* [in] */ DWORD cbPolicy,
    /* [in] */ URLZONEREG urlZoneReg);


void __RPC_STUB IInternetZoneManager_SetZoneCustomPolicy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetZoneManager_GetZoneActionPolicy_Proxy( 
    IInternetZoneManager * This,
    /* [in] */ DWORD dwZone,
    /* [in] */ DWORD dwAction,
    /* [size_is][out] */ BYTE *pPolicy,
    /* [in] */ DWORD cbPolicy,
    /* [in] */ URLZONEREG urlZoneReg);


void __RPC_STUB IInternetZoneManager_GetZoneActionPolicy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetZoneManager_SetZoneActionPolicy_Proxy( 
    IInternetZoneManager * This,
    /* [in] */ DWORD dwZone,
    /* [in] */ DWORD dwAction,
    /* [size_is][in] */ BYTE *pPolicy,
    /* [in] */ DWORD cbPolicy,
    /* [in] */ URLZONEREG urlZoneReg);


void __RPC_STUB IInternetZoneManager_SetZoneActionPolicy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetZoneManager_PromptAction_Proxy( 
    IInternetZoneManager * This,
    /* [in] */ DWORD dwAction,
    /* [in] */ HWND hwndParent,
    /* [in] */ LPCWSTR pwszUrl,
    /* [in] */ LPCWSTR pwszText,
    /* [in] */ DWORD dwPromptFlags);


void __RPC_STUB IInternetZoneManager_PromptAction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetZoneManager_LogAction_Proxy( 
    IInternetZoneManager * This,
    /* [in] */ DWORD dwAction,
    /* [in] */ LPCWSTR pwszUrl,
    /* [in] */ LPCWSTR pwszText,
    /* [in] */ DWORD dwLogFlags);


void __RPC_STUB IInternetZoneManager_LogAction_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetZoneManager_CreateZoneEnumerator_Proxy( 
    IInternetZoneManager * This,
    /* [out] */ DWORD *pdwEnum,
    /* [out] */ DWORD *pdwCount,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IInternetZoneManager_CreateZoneEnumerator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetZoneManager_GetZoneAt_Proxy( 
    IInternetZoneManager * This,
    /* [in] */ DWORD dwEnum,
    /* [in] */ DWORD dwIndex,
    /* [out] */ DWORD *pdwZone);


void __RPC_STUB IInternetZoneManager_GetZoneAt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetZoneManager_DestroyZoneEnumerator_Proxy( 
    IInternetZoneManager * This,
    /* [in] */ DWORD dwEnum);


void __RPC_STUB IInternetZoneManager_DestroyZoneEnumerator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetZoneManager_CopyTemplatePoliciesToZone_Proxy( 
    IInternetZoneManager * This,
    /* [in] */ DWORD dwTemplate,
    /* [in] */ DWORD dwZone,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IInternetZoneManager_CopyTemplatePoliciesToZone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IInternetZoneManager_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0209 */
/* [local] */ 

#endif
#ifndef _LPINTERNETZONEMANAGEREX_DEFINED
#define _LPINTERNETZONEMANAGEREX_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0209_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0209_v0_0_s_ifspec;

#ifndef __IInternetZoneManagerEx_INTERFACE_DEFINED__
#define __IInternetZoneManagerEx_INTERFACE_DEFINED__

/* interface IInternetZoneManagerEx */
/* [unique][helpstring][uuid][object][local] */ 

// Gets action policy associated with a zone, the builtin, fixed-length policies info.

// dwAction is the action code for the action as defined above.
// pPolicy is the caller allocated buffer for the policy data.
// cbPolicy is the size of the caller allocated buffer.
// dwRegFlags determines how registry is accessed (see above).
// dwFlags determine which registry policies are accessed (see above).
// Returns S_OK if action is valid; ??? if action is not valid.

EXTERN_C const IID IID_IInternetZoneManagerEx;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A4C23339-8E06-431e-9BF4-7E711C085648")
    IInternetZoneManagerEx : public IInternetZoneManager
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetZoneActionPolicyEx( 
            /* [in] */ DWORD dwZone,
            /* [in] */ DWORD dwAction,
            /* [size_is][out] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ URLZONEREG urlZoneReg,
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetZoneActionPolicyEx( 
            /* [in] */ DWORD dwZone,
            /* [in] */ DWORD dwAction,
            /* [size_is][in] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ URLZONEREG urlZoneReg,
            /* [in] */ DWORD dwFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInternetZoneManagerExVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInternetZoneManagerEx * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInternetZoneManagerEx * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInternetZoneManagerEx * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetZoneAttributes )( 
            IInternetZoneManagerEx * This,
            /* [in] */ DWORD dwZone,
            /* [unique][out][in] */ ZONEATTRIBUTES *pZoneAttributes);
        
        HRESULT ( STDMETHODCALLTYPE *SetZoneAttributes )( 
            IInternetZoneManagerEx * This,
            /* [in] */ DWORD dwZone,
            /* [in] */ ZONEATTRIBUTES *pZoneAttributes);
        
        HRESULT ( STDMETHODCALLTYPE *GetZoneCustomPolicy )( 
            IInternetZoneManagerEx * This,
            /* [in] */ DWORD dwZone,
            /* [in] */ REFGUID guidKey,
            /* [size_is][size_is][out] */ BYTE **ppPolicy,
            /* [out] */ DWORD *pcbPolicy,
            /* [in] */ URLZONEREG urlZoneReg);
        
        HRESULT ( STDMETHODCALLTYPE *SetZoneCustomPolicy )( 
            IInternetZoneManagerEx * This,
            /* [in] */ DWORD dwZone,
            /* [in] */ REFGUID guidKey,
            /* [size_is][in] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ URLZONEREG urlZoneReg);
        
        HRESULT ( STDMETHODCALLTYPE *GetZoneActionPolicy )( 
            IInternetZoneManagerEx * This,
            /* [in] */ DWORD dwZone,
            /* [in] */ DWORD dwAction,
            /* [size_is][out] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ URLZONEREG urlZoneReg);
        
        HRESULT ( STDMETHODCALLTYPE *SetZoneActionPolicy )( 
            IInternetZoneManagerEx * This,
            /* [in] */ DWORD dwZone,
            /* [in] */ DWORD dwAction,
            /* [size_is][in] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ URLZONEREG urlZoneReg);
        
        HRESULT ( STDMETHODCALLTYPE *PromptAction )( 
            IInternetZoneManagerEx * This,
            /* [in] */ DWORD dwAction,
            /* [in] */ HWND hwndParent,
            /* [in] */ LPCWSTR pwszUrl,
            /* [in] */ LPCWSTR pwszText,
            /* [in] */ DWORD dwPromptFlags);
        
        HRESULT ( STDMETHODCALLTYPE *LogAction )( 
            IInternetZoneManagerEx * This,
            /* [in] */ DWORD dwAction,
            /* [in] */ LPCWSTR pwszUrl,
            /* [in] */ LPCWSTR pwszText,
            /* [in] */ DWORD dwLogFlags);
        
        HRESULT ( STDMETHODCALLTYPE *CreateZoneEnumerator )( 
            IInternetZoneManagerEx * This,
            /* [out] */ DWORD *pdwEnum,
            /* [out] */ DWORD *pdwCount,
            /* [in] */ DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetZoneAt )( 
            IInternetZoneManagerEx * This,
            /* [in] */ DWORD dwEnum,
            /* [in] */ DWORD dwIndex,
            /* [out] */ DWORD *pdwZone);
        
        HRESULT ( STDMETHODCALLTYPE *DestroyZoneEnumerator )( 
            IInternetZoneManagerEx * This,
            /* [in] */ DWORD dwEnum);
        
        HRESULT ( STDMETHODCALLTYPE *CopyTemplatePoliciesToZone )( 
            IInternetZoneManagerEx * This,
            /* [in] */ DWORD dwTemplate,
            /* [in] */ DWORD dwZone,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *GetZoneActionPolicyEx )( 
            IInternetZoneManagerEx * This,
            /* [in] */ DWORD dwZone,
            /* [in] */ DWORD dwAction,
            /* [size_is][out] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ URLZONEREG urlZoneReg,
            /* [in] */ DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *SetZoneActionPolicyEx )( 
            IInternetZoneManagerEx * This,
            /* [in] */ DWORD dwZone,
            /* [in] */ DWORD dwAction,
            /* [size_is][in] */ BYTE *pPolicy,
            /* [in] */ DWORD cbPolicy,
            /* [in] */ URLZONEREG urlZoneReg,
            /* [in] */ DWORD dwFlags);
        
        END_INTERFACE
    } IInternetZoneManagerExVtbl;

    interface IInternetZoneManagerEx
    {
        CONST_VTBL struct IInternetZoneManagerExVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInternetZoneManagerEx_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInternetZoneManagerEx_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInternetZoneManagerEx_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IInternetZoneManagerEx_GetZoneAttributes(This,dwZone,pZoneAttributes)	\
    (This)->lpVtbl -> GetZoneAttributes(This,dwZone,pZoneAttributes)

#define IInternetZoneManagerEx_SetZoneAttributes(This,dwZone,pZoneAttributes)	\
    (This)->lpVtbl -> SetZoneAttributes(This,dwZone,pZoneAttributes)

#define IInternetZoneManagerEx_GetZoneCustomPolicy(This,dwZone,guidKey,ppPolicy,pcbPolicy,urlZoneReg)	\
    (This)->lpVtbl -> GetZoneCustomPolicy(This,dwZone,guidKey,ppPolicy,pcbPolicy,urlZoneReg)

#define IInternetZoneManagerEx_SetZoneCustomPolicy(This,dwZone,guidKey,pPolicy,cbPolicy,urlZoneReg)	\
    (This)->lpVtbl -> SetZoneCustomPolicy(This,dwZone,guidKey,pPolicy,cbPolicy,urlZoneReg)

#define IInternetZoneManagerEx_GetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg)	\
    (This)->lpVtbl -> GetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg)

#define IInternetZoneManagerEx_SetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg)	\
    (This)->lpVtbl -> SetZoneActionPolicy(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg)

#define IInternetZoneManagerEx_PromptAction(This,dwAction,hwndParent,pwszUrl,pwszText,dwPromptFlags)	\
    (This)->lpVtbl -> PromptAction(This,dwAction,hwndParent,pwszUrl,pwszText,dwPromptFlags)

#define IInternetZoneManagerEx_LogAction(This,dwAction,pwszUrl,pwszText,dwLogFlags)	\
    (This)->lpVtbl -> LogAction(This,dwAction,pwszUrl,pwszText,dwLogFlags)

#define IInternetZoneManagerEx_CreateZoneEnumerator(This,pdwEnum,pdwCount,dwFlags)	\
    (This)->lpVtbl -> CreateZoneEnumerator(This,pdwEnum,pdwCount,dwFlags)

#define IInternetZoneManagerEx_GetZoneAt(This,dwEnum,dwIndex,pdwZone)	\
    (This)->lpVtbl -> GetZoneAt(This,dwEnum,dwIndex,pdwZone)

#define IInternetZoneManagerEx_DestroyZoneEnumerator(This,dwEnum)	\
    (This)->lpVtbl -> DestroyZoneEnumerator(This,dwEnum)

#define IInternetZoneManagerEx_CopyTemplatePoliciesToZone(This,dwTemplate,dwZone,dwReserved)	\
    (This)->lpVtbl -> CopyTemplatePoliciesToZone(This,dwTemplate,dwZone,dwReserved)


#define IInternetZoneManagerEx_GetZoneActionPolicyEx(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg,dwFlags)	\
    (This)->lpVtbl -> GetZoneActionPolicyEx(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg,dwFlags)

#define IInternetZoneManagerEx_SetZoneActionPolicyEx(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg,dwFlags)	\
    (This)->lpVtbl -> SetZoneActionPolicyEx(This,dwZone,dwAction,pPolicy,cbPolicy,urlZoneReg,dwFlags)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IInternetZoneManagerEx_GetZoneActionPolicyEx_Proxy( 
    IInternetZoneManagerEx * This,
    /* [in] */ DWORD dwZone,
    /* [in] */ DWORD dwAction,
    /* [size_is][out] */ BYTE *pPolicy,
    /* [in] */ DWORD cbPolicy,
    /* [in] */ URLZONEREG urlZoneReg,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IInternetZoneManagerEx_GetZoneActionPolicyEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IInternetZoneManagerEx_SetZoneActionPolicyEx_Proxy( 
    IInternetZoneManagerEx * This,
    /* [in] */ DWORD dwZone,
    /* [in] */ DWORD dwAction,
    /* [size_is][in] */ BYTE *pPolicy,
    /* [in] */ DWORD cbPolicy,
    /* [in] */ URLZONEREG urlZoneReg,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IInternetZoneManagerEx_SetZoneActionPolicyEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IInternetZoneManagerEx_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0210 */
/* [local] */ 

#endif
EXTERN_C const IID CLSID_SoftDistExt;  
#ifndef _LPSOFTDISTEXT_DEFINED
#define _LPSOFTDISTEXT_DEFINED

#define SOFTDIST_FLAG_USAGE_EMAIL        0x00000001
#define SOFTDIST_FLAG_USAGE_PRECACHE     0x00000002
#define SOFTDIST_FLAG_USAGE_AUTOINSTALL  0x00000004
#define SOFTDIST_FLAG_DELETE_SUBSCRIPTION 0x00000008


#define SOFTDIST_ADSTATE_NONE                0x00000000
#define SOFTDIST_ADSTATE_AVAILABLE       0x00000001
#define SOFTDIST_ADSTATE_DOWNLOADED      0x00000002
#define SOFTDIST_ADSTATE_INSTALLED           0x00000003

typedef struct _tagCODEBASEHOLD
    {
    ULONG cbSize;
    LPWSTR szDistUnit;
    LPWSTR szCodeBase;
    DWORD dwVersionMS;
    DWORD dwVersionLS;
    DWORD dwStyle;
    } 	CODEBASEHOLD;

typedef struct _tagCODEBASEHOLD *LPCODEBASEHOLD;

typedef struct _tagSOFTDISTINFO
    {
    ULONG cbSize;
    DWORD dwFlags;
    DWORD dwAdState;
    LPWSTR szTitle;
    LPWSTR szAbstract;
    LPWSTR szHREF;
    DWORD dwInstalledVersionMS;
    DWORD dwInstalledVersionLS;
    DWORD dwUpdateVersionMS;
    DWORD dwUpdateVersionLS;
    DWORD dwAdvertisedVersionMS;
    DWORD dwAdvertisedVersionLS;
    DWORD dwReserved;
    } 	SOFTDISTINFO;

typedef struct _tagSOFTDISTINFO *LPSOFTDISTINFO;



extern RPC_IF_HANDLE __MIDL_itf_urlmon_0210_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0210_v0_0_s_ifspec;

#ifndef __ISoftDistExt_INTERFACE_DEFINED__
#define __ISoftDistExt_INTERFACE_DEFINED__

/* interface ISoftDistExt */
/* [unique][helpstring][uuid][object][local] */ 


EXTERN_C const IID IID_ISoftDistExt;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B15B8DC1-C7E1-11d0-8680-00AA00BDCB71")
    ISoftDistExt : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ProcessSoftDist( 
            /* [in] */ LPCWSTR szCDFURL,
            /* [in] */ IXMLElement *pSoftDistElement,
            /* [out][in] */ LPSOFTDISTINFO lpsdi) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFirstCodeBase( 
            /* [in] */ LPWSTR *szCodeBase,
            /* [in] */ LPDWORD dwMaxSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNextCodeBase( 
            /* [in] */ LPWSTR *szCodeBase,
            /* [in] */ LPDWORD dwMaxSize) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AsyncInstallDistributionUnit( 
            /* [in] */ IBindCtx *pbc,
            /* [in] */ LPVOID pvReserved,
            /* [in] */ DWORD flags,
            /* [in] */ LPCODEBASEHOLD lpcbh) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISoftDistExtVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISoftDistExt * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISoftDistExt * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISoftDistExt * This);
        
        HRESULT ( STDMETHODCALLTYPE *ProcessSoftDist )( 
            ISoftDistExt * This,
            /* [in] */ LPCWSTR szCDFURL,
            /* [in] */ IXMLElement *pSoftDistElement,
            /* [out][in] */ LPSOFTDISTINFO lpsdi);
        
        HRESULT ( STDMETHODCALLTYPE *GetFirstCodeBase )( 
            ISoftDistExt * This,
            /* [in] */ LPWSTR *szCodeBase,
            /* [in] */ LPDWORD dwMaxSize);
        
        HRESULT ( STDMETHODCALLTYPE *GetNextCodeBase )( 
            ISoftDistExt * This,
            /* [in] */ LPWSTR *szCodeBase,
            /* [in] */ LPDWORD dwMaxSize);
        
        HRESULT ( STDMETHODCALLTYPE *AsyncInstallDistributionUnit )( 
            ISoftDistExt * This,
            /* [in] */ IBindCtx *pbc,
            /* [in] */ LPVOID pvReserved,
            /* [in] */ DWORD flags,
            /* [in] */ LPCODEBASEHOLD lpcbh);
        
        END_INTERFACE
    } ISoftDistExtVtbl;

    interface ISoftDistExt
    {
        CONST_VTBL struct ISoftDistExtVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISoftDistExt_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISoftDistExt_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISoftDistExt_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISoftDistExt_ProcessSoftDist(This,szCDFURL,pSoftDistElement,lpsdi)	\
    (This)->lpVtbl -> ProcessSoftDist(This,szCDFURL,pSoftDistElement,lpsdi)

#define ISoftDistExt_GetFirstCodeBase(This,szCodeBase,dwMaxSize)	\
    (This)->lpVtbl -> GetFirstCodeBase(This,szCodeBase,dwMaxSize)

#define ISoftDistExt_GetNextCodeBase(This,szCodeBase,dwMaxSize)	\
    (This)->lpVtbl -> GetNextCodeBase(This,szCodeBase,dwMaxSize)

#define ISoftDistExt_AsyncInstallDistributionUnit(This,pbc,pvReserved,flags,lpcbh)	\
    (This)->lpVtbl -> AsyncInstallDistributionUnit(This,pbc,pvReserved,flags,lpcbh)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ISoftDistExt_ProcessSoftDist_Proxy( 
    ISoftDistExt * This,
    /* [in] */ LPCWSTR szCDFURL,
    /* [in] */ IXMLElement *pSoftDistElement,
    /* [out][in] */ LPSOFTDISTINFO lpsdi);


void __RPC_STUB ISoftDistExt_ProcessSoftDist_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISoftDistExt_GetFirstCodeBase_Proxy( 
    ISoftDistExt * This,
    /* [in] */ LPWSTR *szCodeBase,
    /* [in] */ LPDWORD dwMaxSize);


void __RPC_STUB ISoftDistExt_GetFirstCodeBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISoftDistExt_GetNextCodeBase_Proxy( 
    ISoftDistExt * This,
    /* [in] */ LPWSTR *szCodeBase,
    /* [in] */ LPDWORD dwMaxSize);


void __RPC_STUB ISoftDistExt_GetNextCodeBase_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISoftDistExt_AsyncInstallDistributionUnit_Proxy( 
    ISoftDistExt * This,
    /* [in] */ IBindCtx *pbc,
    /* [in] */ LPVOID pvReserved,
    /* [in] */ DWORD flags,
    /* [in] */ LPCODEBASEHOLD lpcbh);


void __RPC_STUB ISoftDistExt_AsyncInstallDistributionUnit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISoftDistExt_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0211 */
/* [local] */ 

STDAPI GetSoftwareUpdateInfo( LPCWSTR szDistUnit, LPSOFTDISTINFO psdi );
STDAPI SetSoftwareUpdateAdvertisementState( LPCWSTR szDistUnit, DWORD dwAdState, DWORD dwAdvertisedVersionMS, DWORD dwAdvertisedVersionLS );
#endif
#ifndef _LPCATALOGFILEINFO_DEFINED
#define _LPCATALOGFILEINFO_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0211_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0211_v0_0_s_ifspec;

#ifndef __ICatalogFileInfo_INTERFACE_DEFINED__
#define __ICatalogFileInfo_INTERFACE_DEFINED__

/* interface ICatalogFileInfo */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ ICatalogFileInfo *LPCATALOGFILEINFO;


EXTERN_C const IID IID_ICatalogFileInfo;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("711C7600-6B48-11d1-B403-00AA00B92AF1")
    ICatalogFileInfo : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCatalogFile( 
            /* [out] */ LPSTR *ppszCatalogFile) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetJavaTrust( 
            /* [out] */ void **ppJavaTrust) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICatalogFileInfoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICatalogFileInfo * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICatalogFileInfo * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICatalogFileInfo * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCatalogFile )( 
            ICatalogFileInfo * This,
            /* [out] */ LPSTR *ppszCatalogFile);
        
        HRESULT ( STDMETHODCALLTYPE *GetJavaTrust )( 
            ICatalogFileInfo * This,
            /* [out] */ void **ppJavaTrust);
        
        END_INTERFACE
    } ICatalogFileInfoVtbl;

    interface ICatalogFileInfo
    {
        CONST_VTBL struct ICatalogFileInfoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICatalogFileInfo_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICatalogFileInfo_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICatalogFileInfo_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICatalogFileInfo_GetCatalogFile(This,ppszCatalogFile)	\
    (This)->lpVtbl -> GetCatalogFile(This,ppszCatalogFile)

#define ICatalogFileInfo_GetJavaTrust(This,ppJavaTrust)	\
    (This)->lpVtbl -> GetJavaTrust(This,ppJavaTrust)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICatalogFileInfo_GetCatalogFile_Proxy( 
    ICatalogFileInfo * This,
    /* [out] */ LPSTR *ppszCatalogFile);


void __RPC_STUB ICatalogFileInfo_GetCatalogFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICatalogFileInfo_GetJavaTrust_Proxy( 
    ICatalogFileInfo * This,
    /* [out] */ void **ppJavaTrust);


void __RPC_STUB ICatalogFileInfo_GetJavaTrust_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICatalogFileInfo_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0212 */
/* [local] */ 

#endif
#ifndef _LPDATAFILTER_DEFINED
#define _LPDATAFILTER_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0212_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0212_v0_0_s_ifspec;

#ifndef __IDataFilter_INTERFACE_DEFINED__
#define __IDataFilter_INTERFACE_DEFINED__

/* interface IDataFilter */
/* [unique][uuid][object] */ 

typedef /* [unique] */ IDataFilter *LPDATAFILTER;


EXTERN_C const IID IID_IDataFilter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("69d14c80-c18e-11d0-a9ce-006097942311")
    IDataFilter : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE DoEncode( 
            /* [in] */ DWORD dwFlags,
            /* [in] */ LONG lInBufferSize,
            /* [size_is][in] */ BYTE *pbInBuffer,
            /* [in] */ LONG lOutBufferSize,
            /* [size_is][out] */ BYTE *pbOutBuffer,
            /* [in] */ LONG lInBytesAvailable,
            /* [out] */ LONG *plInBytesRead,
            /* [out] */ LONG *plOutBytesWritten,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DoDecode( 
            /* [in] */ DWORD dwFlags,
            /* [in] */ LONG lInBufferSize,
            /* [size_is][in] */ BYTE *pbInBuffer,
            /* [in] */ LONG lOutBufferSize,
            /* [size_is][out] */ BYTE *pbOutBuffer,
            /* [in] */ LONG lInBytesAvailable,
            /* [out] */ LONG *plInBytesRead,
            /* [out] */ LONG *plOutBytesWritten,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetEncodingLevel( 
            /* [in] */ DWORD dwEncLevel) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDataFilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDataFilter * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDataFilter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDataFilter * This);
        
        HRESULT ( STDMETHODCALLTYPE *DoEncode )( 
            IDataFilter * This,
            /* [in] */ DWORD dwFlags,
            /* [in] */ LONG lInBufferSize,
            /* [size_is][in] */ BYTE *pbInBuffer,
            /* [in] */ LONG lOutBufferSize,
            /* [size_is][out] */ BYTE *pbOutBuffer,
            /* [in] */ LONG lInBytesAvailable,
            /* [out] */ LONG *plInBytesRead,
            /* [out] */ LONG *plOutBytesWritten,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *DoDecode )( 
            IDataFilter * This,
            /* [in] */ DWORD dwFlags,
            /* [in] */ LONG lInBufferSize,
            /* [size_is][in] */ BYTE *pbInBuffer,
            /* [in] */ LONG lOutBufferSize,
            /* [size_is][out] */ BYTE *pbOutBuffer,
            /* [in] */ LONG lInBytesAvailable,
            /* [out] */ LONG *plInBytesRead,
            /* [out] */ LONG *plOutBytesWritten,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *SetEncodingLevel )( 
            IDataFilter * This,
            /* [in] */ DWORD dwEncLevel);
        
        END_INTERFACE
    } IDataFilterVtbl;

    interface IDataFilter
    {
        CONST_VTBL struct IDataFilterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDataFilter_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDataFilter_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDataFilter_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDataFilter_DoEncode(This,dwFlags,lInBufferSize,pbInBuffer,lOutBufferSize,pbOutBuffer,lInBytesAvailable,plInBytesRead,plOutBytesWritten,dwReserved)	\
    (This)->lpVtbl -> DoEncode(This,dwFlags,lInBufferSize,pbInBuffer,lOutBufferSize,pbOutBuffer,lInBytesAvailable,plInBytesRead,plOutBytesWritten,dwReserved)

#define IDataFilter_DoDecode(This,dwFlags,lInBufferSize,pbInBuffer,lOutBufferSize,pbOutBuffer,lInBytesAvailable,plInBytesRead,plOutBytesWritten,dwReserved)	\
    (This)->lpVtbl -> DoDecode(This,dwFlags,lInBufferSize,pbInBuffer,lOutBufferSize,pbOutBuffer,lInBytesAvailable,plInBytesRead,plOutBytesWritten,dwReserved)

#define IDataFilter_SetEncodingLevel(This,dwEncLevel)	\
    (This)->lpVtbl -> SetEncodingLevel(This,dwEncLevel)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDataFilter_DoEncode_Proxy( 
    IDataFilter * This,
    /* [in] */ DWORD dwFlags,
    /* [in] */ LONG lInBufferSize,
    /* [size_is][in] */ BYTE *pbInBuffer,
    /* [in] */ LONG lOutBufferSize,
    /* [size_is][out] */ BYTE *pbOutBuffer,
    /* [in] */ LONG lInBytesAvailable,
    /* [out] */ LONG *plInBytesRead,
    /* [out] */ LONG *plOutBytesWritten,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IDataFilter_DoEncode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDataFilter_DoDecode_Proxy( 
    IDataFilter * This,
    /* [in] */ DWORD dwFlags,
    /* [in] */ LONG lInBufferSize,
    /* [size_is][in] */ BYTE *pbInBuffer,
    /* [in] */ LONG lOutBufferSize,
    /* [size_is][out] */ BYTE *pbOutBuffer,
    /* [in] */ LONG lInBytesAvailable,
    /* [out] */ LONG *plInBytesRead,
    /* [out] */ LONG *plOutBytesWritten,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IDataFilter_DoDecode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDataFilter_SetEncodingLevel_Proxy( 
    IDataFilter * This,
    /* [in] */ DWORD dwEncLevel);


void __RPC_STUB IDataFilter_SetEncodingLevel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDataFilter_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0213 */
/* [local] */ 

#endif
#ifndef _LPENCODINGFILTERFACTORY_DEFINED
#define _LPENCODINGFILTERFACTORY_DEFINED
typedef struct _tagPROTOCOLFILTERDATA
    {
    DWORD cbSize;
    IInternetProtocolSink *pProtocolSink;
    IInternetProtocol *pProtocol;
    IUnknown *pUnk;
    DWORD dwFilterFlags;
    } 	PROTOCOLFILTERDATA;



extern RPC_IF_HANDLE __MIDL_itf_urlmon_0213_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0213_v0_0_s_ifspec;

#ifndef __IEncodingFilterFactory_INTERFACE_DEFINED__
#define __IEncodingFilterFactory_INTERFACE_DEFINED__

/* interface IEncodingFilterFactory */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IEncodingFilterFactory *LPENCODINGFILTERFACTORY;

typedef struct _tagDATAINFO
    {
    ULONG ulTotalSize;
    ULONG ulavrPacketSize;
    ULONG ulConnectSpeed;
    ULONG ulProcessorSpeed;
    } 	DATAINFO;


EXTERN_C const IID IID_IEncodingFilterFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("70bdde00-c18e-11d0-a9ce-006097942311")
    IEncodingFilterFactory : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE FindBestFilter( 
            /* [in] */ LPCWSTR pwzCodeIn,
            /* [in] */ LPCWSTR pwzCodeOut,
            /* [in] */ DATAINFO info,
            /* [out] */ IDataFilter **ppDF) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDefaultFilter( 
            /* [in] */ LPCWSTR pwzCodeIn,
            /* [in] */ LPCWSTR pwzCodeOut,
            /* [out] */ IDataFilter **ppDF) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEncodingFilterFactoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEncodingFilterFactory * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEncodingFilterFactory * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEncodingFilterFactory * This);
        
        HRESULT ( STDMETHODCALLTYPE *FindBestFilter )( 
            IEncodingFilterFactory * This,
            /* [in] */ LPCWSTR pwzCodeIn,
            /* [in] */ LPCWSTR pwzCodeOut,
            /* [in] */ DATAINFO info,
            /* [out] */ IDataFilter **ppDF);
        
        HRESULT ( STDMETHODCALLTYPE *GetDefaultFilter )( 
            IEncodingFilterFactory * This,
            /* [in] */ LPCWSTR pwzCodeIn,
            /* [in] */ LPCWSTR pwzCodeOut,
            /* [out] */ IDataFilter **ppDF);
        
        END_INTERFACE
    } IEncodingFilterFactoryVtbl;

    interface IEncodingFilterFactory
    {
        CONST_VTBL struct IEncodingFilterFactoryVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEncodingFilterFactory_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEncodingFilterFactory_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEncodingFilterFactory_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEncodingFilterFactory_FindBestFilter(This,pwzCodeIn,pwzCodeOut,info,ppDF)	\
    (This)->lpVtbl -> FindBestFilter(This,pwzCodeIn,pwzCodeOut,info,ppDF)

#define IEncodingFilterFactory_GetDefaultFilter(This,pwzCodeIn,pwzCodeOut,ppDF)	\
    (This)->lpVtbl -> GetDefaultFilter(This,pwzCodeIn,pwzCodeOut,ppDF)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEncodingFilterFactory_FindBestFilter_Proxy( 
    IEncodingFilterFactory * This,
    /* [in] */ LPCWSTR pwzCodeIn,
    /* [in] */ LPCWSTR pwzCodeOut,
    /* [in] */ DATAINFO info,
    /* [out] */ IDataFilter **ppDF);


void __RPC_STUB IEncodingFilterFactory_FindBestFilter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEncodingFilterFactory_GetDefaultFilter_Proxy( 
    IEncodingFilterFactory * This,
    /* [in] */ LPCWSTR pwzCodeIn,
    /* [in] */ LPCWSTR pwzCodeOut,
    /* [out] */ IDataFilter **ppDF);


void __RPC_STUB IEncodingFilterFactory_GetDefaultFilter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEncodingFilterFactory_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0214 */
/* [local] */ 

#endif
#ifndef _HITLOGGING_DEFINED
#define _HITLOGGING_DEFINED
// Logging-specific apis
BOOL WINAPI IsLoggingEnabledA(IN LPCSTR  pszUrl);                    
BOOL WINAPI IsLoggingEnabledW(IN LPCWSTR  pwszUrl);                  
#ifdef UNICODE                                                       
#define IsLoggingEnabled         IsLoggingEnabledW                   
#else                                                                
#define IsLoggingEnabled         IsLoggingEnabledA                   
#endif // !UNICODE                                                   
typedef struct _tagHIT_LOGGING_INFO
    {
    DWORD dwStructSize;
    LPSTR lpszLoggedUrlName;
    SYSTEMTIME StartTime;
    SYSTEMTIME EndTime;
    LPSTR lpszExtendedInfo;
    } 	HIT_LOGGING_INFO;

typedef struct _tagHIT_LOGGING_INFO *LPHIT_LOGGING_INFO;

BOOL WINAPI WriteHitLogging(IN LPHIT_LOGGING_INFO lpLogginginfo);    
#define CONFIRMSAFETYACTION_LOADOBJECT  0x00000001
struct CONFIRMSAFETY
    {
    CLSID clsid;
    IUnknown *pUnk;
    DWORD dwFlags;
    } ;
EXTERN_C const GUID GUID_CUSTOM_CONFIRMOBJECTSAFETY; 
#endif
#ifndef _LPIWRAPPEDPROTOCOL_DEFINED
#define _LPIWRAPPEDPROTOCOL_DEFINED


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0214_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0214_v0_0_s_ifspec;

#ifndef __IWrappedProtocol_INTERFACE_DEFINED__
#define __IWrappedProtocol_INTERFACE_DEFINED__

/* interface IWrappedProtocol */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ IWrappedProtocol *LPIWRAPPEDPROTOCOL;


EXTERN_C const IID IID_IWrappedProtocol;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("53c84785-8425-4dc5-971b-e58d9c19f9b6")
    IWrappedProtocol : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetWrapperCode( 
            /* [out] */ LONG *pnCode,
            /* [in] */ DWORD_PTR dwReserved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWrappedProtocolVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWrappedProtocol * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWrappedProtocol * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWrappedProtocol * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetWrapperCode )( 
            IWrappedProtocol * This,
            /* [out] */ LONG *pnCode,
            /* [in] */ DWORD_PTR dwReserved);
        
        END_INTERFACE
    } IWrappedProtocolVtbl;

    interface IWrappedProtocol
    {
        CONST_VTBL struct IWrappedProtocolVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWrappedProtocol_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWrappedProtocol_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWrappedProtocol_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWrappedProtocol_GetWrapperCode(This,pnCode,dwReserved)	\
    (This)->lpVtbl -> GetWrapperCode(This,pnCode,dwReserved)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWrappedProtocol_GetWrapperCode_Proxy( 
    IWrappedProtocol * This,
    /* [out] */ LONG *pnCode,
    /* [in] */ DWORD_PTR dwReserved);


void __RPC_STUB IWrappedProtocol_GetWrapperCode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWrappedProtocol_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_urlmon_0215 */
/* [local] */ 

#endif


extern RPC_IF_HANDLE __MIDL_itf_urlmon_0215_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_urlmon_0215_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  HWND_UserSize(     unsigned long *, unsigned long            , HWND * ); 
unsigned char * __RPC_USER  HWND_UserMarshal(  unsigned long *, unsigned char *, HWND * ); 
unsigned char * __RPC_USER  HWND_UserUnmarshal(unsigned long *, unsigned char *, HWND * ); 
void                      __RPC_USER  HWND_UserFree(     unsigned long *, HWND * ); 

/* [local] */ HRESULT STDMETHODCALLTYPE IBinding_GetBindResult_Proxy( 
    IBinding * This,
    /* [out] */ CLSID *pclsidProtocol,
    /* [out] */ DWORD *pdwResult,
    /* [out] */ LPOLESTR *pszResult,
    /* [out][in] */ DWORD *pdwReserved);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IBinding_GetBindResult_Stub( 
    IBinding * This,
    /* [out] */ CLSID *pclsidProtocol,
    /* [out] */ DWORD *pdwResult,
    /* [out] */ LPOLESTR *pszResult,
    /* [in] */ DWORD dwReserved);

/* [local] */ HRESULT STDMETHODCALLTYPE IBindStatusCallback_GetBindInfo_Proxy( 
    IBindStatusCallback * This,
    /* [out] */ DWORD *grfBINDF,
    /* [unique][out][in] */ BINDINFO *pbindinfo);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IBindStatusCallback_GetBindInfo_Stub( 
    IBindStatusCallback * This,
    /* [out] */ DWORD *grfBINDF,
    /* [unique][out][in] */ RemBINDINFO *pbindinfo,
    /* [unique][out][in] */ RemSTGMEDIUM *pstgmed);

/* [local] */ HRESULT STDMETHODCALLTYPE IBindStatusCallback_OnDataAvailable_Proxy( 
    IBindStatusCallback * This,
    /* [in] */ DWORD grfBSCF,
    /* [in] */ DWORD dwSize,
    /* [in] */ FORMATETC *pformatetc,
    /* [in] */ STGMEDIUM *pstgmed);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IBindStatusCallback_OnDataAvailable_Stub( 
    IBindStatusCallback * This,
    /* [in] */ DWORD grfBSCF,
    /* [in] */ DWORD dwSize,
    /* [in] */ RemFORMATETC *pformatetc,
    /* [in] */ RemSTGMEDIUM *pstgmed);

/* [local] */ HRESULT STDMETHODCALLTYPE IWinInetInfo_QueryOption_Proxy( 
    IWinInetInfo * This,
    /* [in] */ DWORD dwOption,
    /* [size_is][out][in] */ LPVOID pBuffer,
    /* [out][in] */ DWORD *pcbBuf);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IWinInetInfo_QueryOption_Stub( 
    IWinInetInfo * This,
    /* [in] */ DWORD dwOption,
    /* [size_is][out][in] */ BYTE *pBuffer,
    /* [out][in] */ DWORD *pcbBuf);

/* [local] */ HRESULT STDMETHODCALLTYPE IWinInetHttpInfo_QueryInfo_Proxy( 
    IWinInetHttpInfo * This,
    /* [in] */ DWORD dwOption,
    /* [size_is][out][in] */ LPVOID pBuffer,
    /* [out][in] */ DWORD *pcbBuf,
    /* [out][in] */ DWORD *pdwFlags,
    /* [out][in] */ DWORD *pdwReserved);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IWinInetHttpInfo_QueryInfo_Stub( 
    IWinInetHttpInfo * This,
    /* [in] */ DWORD dwOption,
    /* [size_is][out][in] */ BYTE *pBuffer,
    /* [out][in] */ DWORD *pcbBuf,
    /* [out][in] */ DWORD *pdwFlags,
    /* [out][in] */ DWORD *pdwReserved);

/* [local] */ HRESULT STDMETHODCALLTYPE IBindHost_MonikerBindToStorage_Proxy( 
    IBindHost * This,
    /* [in] */ IMoniker *pMk,
    /* [in] */ IBindCtx *pBC,
    /* [in] */ IBindStatusCallback *pBSC,
    /* [in] */ REFIID riid,
    /* [out] */ void **ppvObj);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IBindHost_MonikerBindToStorage_Stub( 
    IBindHost * This,
    /* [unique][in] */ IMoniker *pMk,
    /* [unique][in] */ IBindCtx *pBC,
    /* [unique][in] */ IBindStatusCallback *pBSC,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ IUnknown **ppvObj);

/* [local] */ HRESULT STDMETHODCALLTYPE IBindHost_MonikerBindToObject_Proxy( 
    IBindHost * This,
    /* [in] */ IMoniker *pMk,
    /* [in] */ IBindCtx *pBC,
    /* [in] */ IBindStatusCallback *pBSC,
    /* [in] */ REFIID riid,
    /* [out] */ void **ppvObj);


/* [call_as] */ HRESULT STDMETHODCALLTYPE IBindHost_MonikerBindToObject_Stub( 
    IBindHost * This,
    /* [unique][in] */ IMoniker *pMk,
    /* [unique][in] */ IBindCtx *pBC,
    /* [unique][in] */ IBindStatusCallback *pBSC,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ IUnknown **ppvObj);



/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



