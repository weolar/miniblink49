/*#!perl
MapHeaderToDll("objbase.h", "ole32.dll");
ActivateAroundFunctionCall("ole32.dll");
#IgnoreFunction("CreateDataAdviseHolder"); # this function occurs in ole2.h and objbase.h
                                           # The wrapped one is in objbase.h
IgnoreFunction("CoBuildVersion"); # deprecated
IgnoreFunction("CoGetCurrentProcess"); # never fails => hard to wrap well
IgnoreFunction("CoAddRefServerProcess"); # never fails => hard to wrap well
IgnoreFunction("CoReleaseServerProcess"); # never fails => hard to wrap well
IgnoreFunction("DebugCoGetRpcFault"); # not documented
IgnoreFunction("DebugCoSetRpcFault"); # not documented
IgnoreFunction("wIsEqualGUID");
DeclareFunctionErrorValue("CoLoadLibrary", "NULL");
DeclareFunctionErrorValue("StringFromGUID2" , "0");
DeclareFunctionErrorValue("CoTaskMemAlloc", "NULL");
DeclareFunctionErrorValue("CoTaskMemRealloc", "NULL");
IgnoreFunction("DllGetClassObject"); # client function prototyped (like WinMain)
IgnoreFunction("DllCanUnloadNow"); # client function prototyped (like WinMain)
*/

//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (c) Microsoft Corporation. All rights reserved.
//
//  File:       objbase.h
//
//  Contents:   Component object model defintions.
//
//----------------------------------------------------------------------------

#include <rpc.h>
#include <rpcndr.h>

#if !defined( _OBJBASE_H_ )
#define _OBJBASE_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include <pshpack8.h>

#ifdef _MAC
#ifndef _WLM_NOFORCE_LIBS

#ifdef _WLMDLL
        #ifdef _DEBUG
                #pragma comment(lib, "oledlgd.lib")
                #pragma comment(lib, "msvcoled.lib")
        #else
                #pragma comment(lib, "oledlg.lib")
                #pragma comment(lib, "msvcole.lib")
        #endif
#else
        #ifdef _DEBUG
                #pragma comment(lib, "wlmoled.lib")
                #pragma comment(lib, "ole2uid.lib")
        #else
                #pragma comment(lib, "wlmole.lib")
                #pragma comment(lib, "ole2ui.lib")
        #endif
        #pragma data_seg(".drectve")
        static char _gszWlmOLEUIResourceDirective[] = "/macres:ole2ui.rsc";
        #pragma data_seg()
#endif

#pragma comment(lib, "uuid.lib")

#ifdef _DEBUG
    #pragma comment(lib, "ole2d.lib")
    #pragma comment(lib, "ole2autd.lib")
#else
    #pragma comment(lib, "ole2.lib")
    #pragma comment(lib, "ole2auto.lib")
#endif

#endif // !_WLM_NOFORCE_LIBS
#endif // _MAC

#ifdef _OLE32_
#define WINOLEAPI        STDAPI
#define WINOLEAPI_(type) STDAPI_(type)
#else

#ifdef _68K_
#ifndef REQUIRESAPPLEPASCAL
#define WINOLEAPI        EXTERN_C DECLSPEC_IMPORT HRESULT PASCAL
#define WINOLEAPI_(type) EXTERN_C DECLSPEC_IMPORT type PASCAL
#else
#define WINOLEAPI        EXTERN_C DECLSPEC_IMPORT PASCAL HRESULT
#define WINOLEAPI_(type) EXTERN_C DECLSPEC_IMPORT PASCAL type
#endif
#else
#define WINOLEAPI        EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define WINOLEAPI_(type) EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
#endif

#endif

/****** Interface Declaration ***********************************************/

/*
 *      These are macros for declaring interfaces.  They exist so that
 *      a single definition of the interface is simulataneously a proper
 *      declaration of the interface structures (C++ abstract classes)
 *      for both C and C++.
 *
 *      DECLARE_INTERFACE(iface) is used to declare an interface that does
 *      not derive from a base interface.
 *      DECLARE_INTERFACE_(iface, baseiface) is used to declare an interface
 *      that does derive from a base interface.
 *
 *      By default if the source file has a .c extension the C version of
 *      the interface declaratations will be expanded; if it has a .cpp
 *      extension the C++ version will be expanded. if you want to force
 *      the C version expansion even though the source file has a .cpp
 *      extension, then define the macro "CINTERFACE".
 *      eg.     cl -DCINTERFACE file.cpp
 *
 *      Example Interface declaration:
 *
 *          #undef  INTERFACE
 *          #define INTERFACE   IClassFactory
 *
 *          DECLARE_INTERFACE_(IClassFactory, IUnknown)
 *          {
 *              // *** IUnknown methods ***
 *              STDMETHOD(QueryInterface) (THIS_
 *                                        REFIID riid,
 *                                        LPVOID FAR* ppvObj) PURE;
 *              STDMETHOD_(ULONG,AddRef) (THIS) PURE;
 *              STDMETHOD_(ULONG,Release) (THIS) PURE;
 *
 *              // *** IClassFactory methods ***
 *              STDMETHOD(CreateInstance) (THIS_
 *                                        LPUNKNOWN pUnkOuter,
 *                                        REFIID riid,
 *                                        LPVOID FAR* ppvObject) PURE;
 *          };
 *
 *      Example C++ expansion:
 *
 *          struct FAR IClassFactory : public IUnknown
 *          {
 *              virtual HRESULT STDMETHODCALLTYPE QueryInterface(
 *                                                  IID FAR& riid,
 *                                                  LPVOID FAR* ppvObj) = 0;
 *              virtual HRESULT STDMETHODCALLTYPE AddRef(void) = 0;
 *              virtual HRESULT STDMETHODCALLTYPE Release(void) = 0;
 *              virtual HRESULT STDMETHODCALLTYPE CreateInstance(
 *                                              LPUNKNOWN pUnkOuter,
 *                                              IID FAR& riid,
 *                                              LPVOID FAR* ppvObject) = 0;
 *          };
 *
 *          NOTE: Our documentation says '#define interface class' but we use
 *          'struct' instead of 'class' to keep a lot of 'public:' lines
 *          out of the interfaces.  The 'FAR' forces the 'this' pointers to
 *          be far, which is what we need.
 *
 *      Example C expansion:
 *
 *          typedef struct IClassFactory
 *          {
 *              const struct IClassFactoryVtbl FAR* lpVtbl;
 *          } IClassFactory;
 *
 *          typedef struct IClassFactoryVtbl IClassFactoryVtbl;
 *
 *          struct IClassFactoryVtbl
 *          {
 *              HRESULT (STDMETHODCALLTYPE * QueryInterface) (
 *                                                  IClassFactory FAR* This,
 *                                                  IID FAR* riid,
 *                                                  LPVOID FAR* ppvObj) ;
 *              HRESULT (STDMETHODCALLTYPE * AddRef) (IClassFactory FAR* This) ;
 *              HRESULT (STDMETHODCALLTYPE * Release) (IClassFactory FAR* This) ;
 *              HRESULT (STDMETHODCALLTYPE * CreateInstance) (
 *                                                  IClassFactory FAR* This,
 *                                                  LPUNKNOWN pUnkOuter,
 *                                                  IID FAR* riid,
 *                                                  LPVOID FAR* ppvObject);
 *              HRESULT (STDMETHODCALLTYPE * LockServer) (
 *                                                  IClassFactory FAR* This,
 *                                                  BOOL fLock);
 *          };
 */

#if defined(__cplusplus) && !defined(CINTERFACE)
//#define interface               struct FAR
#define __STRUCT__ struct
#define interface __STRUCT__
#define STDMETHOD(method)       virtual HRESULT STDMETHODCALLTYPE method
#define STDMETHOD_(type,method) virtual type STDMETHODCALLTYPE method
#define STDMETHODV(method)       virtual HRESULT STDMETHODVCALLTYPE method
#define STDMETHODV_(type,method) virtual type STDMETHODVCALLTYPE method
#define PURE                    = 0
#define THIS_
#define THIS                    void
#define DECLARE_INTERFACE(iface)    interface DECLSPEC_NOVTABLE iface
#define DECLARE_INTERFACE_(iface, baseiface)    interface DECLSPEC_NOVTABLE iface : public baseiface


#if !defined(BEGIN_INTERFACE)
#if defined(_MPPC_)  && \
    ( (defined(_MSC_VER) || defined(__SC__) || defined(__MWERKS__)) && \
    !defined(NO_NULL_VTABLE_ENTRY) )
   #define BEGIN_INTERFACE virtual void a() {}
   #define END_INTERFACE
#else
   #define BEGIN_INTERFACE
   #define END_INTERFACE
#endif
#endif

#else

#define interface               struct

#define STDMETHOD(method)       HRESULT (STDMETHODCALLTYPE * method)
#define STDMETHOD_(type,method) type (STDMETHODCALLTYPE * method)
#define STDMETHODV(method)       HRESULT (STDMETHODVCALLTYPE * method)
#define STDMETHODV_(type,method) type (STDMETHODVCALLTYPE * method)

#if !defined(BEGIN_INTERFACE)
#if defined(_MPPC_)
    #define BEGIN_INTERFACE       void    *b;
    #define END_INTERFACE
#else
    #define BEGIN_INTERFACE
    #define END_INTERFACE
#endif
#endif


#define PURE
#define THIS_                   INTERFACE FAR* This,
#define THIS                    INTERFACE FAR* This
#ifdef CONST_VTABLE
#undef CONST_VTBL
#define CONST_VTBL const
#define DECLARE_INTERFACE(iface)    typedef interface iface { \
                                    const struct iface##Vtbl FAR* lpVtbl; \
                                } iface; \
                                typedef const struct iface##Vtbl iface##Vtbl; \
                                const struct iface##Vtbl
#else
#undef CONST_VTBL
#define CONST_VTBL
#define DECLARE_INTERFACE(iface)    typedef interface iface { \
                                    struct iface##Vtbl FAR* lpVtbl; \
                                } iface; \
                                typedef struct iface##Vtbl iface##Vtbl; \
                                struct iface##Vtbl
#endif
#define DECLARE_INTERFACE_(iface, baseiface)    DECLARE_INTERFACE(iface)

#endif




/****** Additional basic types **********************************************/


#ifndef FARSTRUCT
#ifdef __cplusplus
#define FARSTRUCT   FAR
#else
#define FARSTRUCT
#endif  // __cplusplus
#endif  // FARSTRUCT



#ifndef HUGEP
#if defined(_WIN32) || defined(_MPPC_)
#define HUGEP
#else
#define HUGEP __huge
#endif // WIN32
#endif // HUGEP


#ifdef _MAC
#if !defined(OLE2ANSI)
#define OLE2ANSI
#endif
#endif

#include <stdlib.h>

#define LISet32(li, v) ((li).HighPart = ((LONG) (v)) < 0 ? -1 : 0, (li).LowPart = (v))

#define ULISet32(li, v) ((li).HighPart = 0, (li).LowPart = (v))






#define CLSCTX_INPROC           (CLSCTX_INPROC_SERVER|CLSCTX_INPROC_HANDLER)

// With DCOM, CLSCTX_REMOTE_SERVER should be included
#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
#define CLSCTX_ALL              (CLSCTX_INPROC_SERVER| \
                                 CLSCTX_INPROC_HANDLER| \
                                 CLSCTX_LOCAL_SERVER| \
                                 CLSCTX_REMOTE_SERVER)

#define CLSCTX_SERVER           (CLSCTX_INPROC_SERVER|CLSCTX_LOCAL_SERVER|CLSCTX_REMOTE_SERVER)
#else
#define CLSCTX_ALL              (CLSCTX_INPROC_SERVER| \
                                 CLSCTX_INPROC_HANDLER| \
                                 CLSCTX_LOCAL_SERVER )

#define CLSCTX_SERVER           (CLSCTX_INPROC_SERVER|CLSCTX_LOCAL_SERVER)
#endif


// class registration flags; passed to CoRegisterClassObject
typedef enum tagREGCLS
{
    REGCLS_SINGLEUSE = 0,       // class object only generates one instance
    REGCLS_MULTIPLEUSE = 1,     // same class object genereates multiple inst.
                                // and local automatically goes into inproc tbl.
    REGCLS_MULTI_SEPARATE = 2,  // multiple use, but separate control over each
                                // context.
    REGCLS_SUSPENDED      = 4,  // register is as suspended, will be activated
                                // when app calls CoResumeClassObjects
    REGCLS_SURROGATE      = 8   // must be used when a surrogate process
                                // is registering a class object that will be
                                // loaded in the surrogate
} REGCLS;

// interface marshaling definitions
#define MARSHALINTERFACE_MIN 500 // minimum number of bytes for interface marshl


//
// Common typedefs for paramaters used in Storage API's, gleamed from storage.h
// Also contains Storage error codes, which should be moved into the storage
// idl files.
//


#define CWCSTORAGENAME 32

/* Storage instantiation modes */
#define STGM_DIRECT             0x00000000L
#define STGM_TRANSACTED         0x00010000L
#define STGM_SIMPLE             0x08000000L

#define STGM_READ               0x00000000L
#define STGM_WRITE              0x00000001L
#define STGM_READWRITE          0x00000002L

#define STGM_SHARE_DENY_NONE    0x00000040L
#define STGM_SHARE_DENY_READ    0x00000030L
#define STGM_SHARE_DENY_WRITE   0x00000020L
#define STGM_SHARE_EXCLUSIVE    0x00000010L

#define STGM_PRIORITY           0x00040000L
#define STGM_DELETEONRELEASE    0x04000000L
#if (WINVER >= 400)
#define STGM_NOSCRATCH          0x00100000L
#endif /* WINVER */

#define STGM_CREATE             0x00001000L
#define STGM_CONVERT            0x00020000L
#define STGM_FAILIFTHERE        0x00000000L

#define STGM_NOSNAPSHOT         0x00200000L
#if (_WIN32_WINNT >= 0x0500)
#define STGM_DIRECT_SWMR        0x00400000L
#endif

/*  flags for internet asyncronous and layout docfile */
#define ASYNC_MODE_COMPATIBILITY    0x00000001L
#define ASYNC_MODE_DEFAULT          0x00000000L

#define STGTY_REPEAT                0x00000100L
#define STG_TOEND                   0xFFFFFFFFL

#define STG_LAYOUT_SEQUENTIAL       0x00000000L
#define STG_LAYOUT_INTERLEAVED      0x00000001L

#define STGFMT_STORAGE          0
#define STGFMT_NATIVE           1
#define STGFMT_FILE             3
#define STGFMT_ANY              4
#define STGFMT_DOCFILE          5

// This is a legacy define to allow old component to builds
#define STGFMT_DOCUMENT         0

/* here is where we pull in the MIDL generated headers for the interfaces */
typedef interface    IRpcStubBuffer     IRpcStubBuffer;
typedef interface    IRpcChannelBuffer  IRpcChannelBuffer;

#include <wtypes.h>
#include <unknwn.h>
#include <objidl.h>

#ifdef _OLE32_
#ifdef _OLE32PRIV_
BOOL _fastcall wIsEqualGUID(REFGUID rguid1, REFGUID rguid2);
#define IsEqualGUID(rguid1, rguid2) wIsEqualGUID(rguid1, rguid2)
#else
#define __INLINE_ISEQUAL_GUID
#endif  // _OLE32PRIV_
#endif  // _OLE32_

#include <guiddef.h>

#ifndef INITGUID
#include <cguid.h>
#endif

// COM initialization flags; passed to CoInitialize.
typedef enum tagCOINIT
{
  COINIT_APARTMENTTHREADED  = 0x2,      // Apartment model

#if  (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
  // These constants are only valid on Windows NT 4.0
  COINIT_MULTITHREADED      = 0x0,      // OLE calls objects on any thread.
  COINIT_DISABLE_OLE1DDE    = 0x4,      // Don't use DDE for Ole1 support.
  COINIT_SPEED_OVER_MEMORY  = 0x8,      // Trade memory for speed.
#endif // DCOM
} COINIT;





/****** STD Object API Prototypes *****************************************/

WINOLEAPI_(DWORD) CoBuildVersion( VOID );

/* init/uninit */

WINOLEAPI  CoInitialize(IN LPVOID pvReserved);
WINOLEAPI_(void)  CoUninitialize(void);
WINOLEAPI  CoGetMalloc(IN DWORD dwMemContext, OUT LPMALLOC FAR* ppMalloc);
WINOLEAPI_(DWORD) CoGetCurrentProcess(void);
WINOLEAPI  CoRegisterMallocSpy(IN LPMALLOCSPY pMallocSpy);
WINOLEAPI  CoRevokeMallocSpy(void);
WINOLEAPI  CoCreateStandardMalloc(IN DWORD memctx, OUT IMalloc FAR* FAR* ppMalloc);

#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
/* #!perl PoundIf("CoInitializeEx", "(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI  CoInitializeEx(IN LPVOID pvReserved, IN DWORD dwCoInit);

/* #!perl PoundIf("CoGetCallerTID", "(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI  CoGetCallerTID( LPDWORD lpdwTID );
#endif // DCOM

#if (_WIN32_WINNT >= 0x0501)
/* #!perl
    PoundIf("CoRegisterInitializeSpy", "_WIN32_WINNT >= 0x0501");
    PoundIf("CoRevokeInitializeSpy", "_WIN32_WINNT >= 0x0501");
*/
WINOLEAPI  CoRegisterInitializeSpy(IN LPINITIALIZESPY pSpy, OUT ULARGE_INTEGER *puliCookie);
WINOLEAPI  CoRevokeInitializeSpy(IN ULARGE_INTEGER uliCookie);

WINOLEAPI  CoGetContextToken(ULONG_PTR* pToken);

// COM System Security Descriptors (used when the corresponding registry 
// entries are absent)
typedef enum tagCOMSD
{
    SD_LAUNCHPERMISSIONS = 0,   	// Machine wide launch permissions
    SD_ACCESSPERMISSIONS = 1,   	// Machine wide acesss permissions
    SD_LAUNCHRESTRICTIONS = 2,   	// Machine wide launch limits
    SD_ACCESSRESTRICTIONS = 3   	// Machine wide access limits
    
} COMSD;
WINOLEAPI  CoGetSystemSecurityPermissions(COMSD comSDType, PSECURITY_DESCRIPTOR *ppSD);

#endif

#if DBG == 1
WINOLEAPI_(ULONG) DebugCoGetRpcFault( void );
WINOLEAPI_(void) DebugCoSetRpcFault( ULONG );
#endif

#if (_WIN32_WINNT >= 0x0500)

typedef struct tagSOleTlsDataPublic
{
    void *pvReserved0[2];
    DWORD dwReserved0[3];
    void *pvReserved1[1];
    DWORD dwReserved1[3];
    void *pvReserved2[4];
    DWORD dwReserved2[1];
    void *pCurrentCtx;
} SOleTlsDataPublic;

#endif

/* COM+ APIs */

WINOLEAPI     CoGetObjectContext(IN REFIID riid, OUT LPVOID FAR* ppv);

/* register/revoke/get class objects */

WINOLEAPI  CoGetClassObject(IN REFCLSID rclsid, IN DWORD dwClsContext, IN LPVOID pvReserved,
                    IN REFIID riid, OUT LPVOID FAR* ppv);
WINOLEAPI  CoRegisterClassObject(IN REFCLSID rclsid, IN LPUNKNOWN pUnk,
                    IN DWORD dwClsContext, IN DWORD flags, OUT LPDWORD lpdwRegister);
WINOLEAPI  CoRevokeClassObject(IN DWORD dwRegister);
WINOLEAPI  CoResumeClassObjects(void);
WINOLEAPI  CoSuspendClassObjects(void);
WINOLEAPI_(ULONG) CoAddRefServerProcess(void);
WINOLEAPI_(ULONG) CoReleaseServerProcess(void);
WINOLEAPI  CoGetPSClsid(IN REFIID riid, OUT CLSID *pClsid);
WINOLEAPI  CoRegisterPSClsid(IN REFIID riid, IN REFCLSID rclsid);

// Registering surrogate processes
WINOLEAPI  CoRegisterSurrogate(IN LPSURROGATE pSurrogate);

/* marshaling interface pointers */

WINOLEAPI CoGetMarshalSizeMax(OUT ULONG *pulSize, IN REFIID riid, IN LPUNKNOWN pUnk,
                    IN DWORD dwDestContext, IN LPVOID pvDestContext, IN DWORD mshlflags);
WINOLEAPI CoMarshalInterface(IN LPSTREAM pStm, IN REFIID riid, IN LPUNKNOWN pUnk,
                    IN DWORD dwDestContext, IN LPVOID pvDestContext, IN DWORD mshlflags);
WINOLEAPI CoUnmarshalInterface(IN LPSTREAM pStm, IN REFIID riid, OUT LPVOID FAR* ppv);
WINOLEAPI CoMarshalHresult(IN LPSTREAM pstm, IN HRESULT hresult);
WINOLEAPI CoUnmarshalHresult(IN LPSTREAM pstm, OUT HRESULT FAR * phresult);
WINOLEAPI CoReleaseMarshalData(IN LPSTREAM pStm);
WINOLEAPI CoDisconnectObject(IN LPUNKNOWN pUnk, IN DWORD dwReserved);
WINOLEAPI CoLockObjectExternal(IN LPUNKNOWN pUnk, IN BOOL fLock, IN BOOL fLastUnlockReleases);
WINOLEAPI CoGetStandardMarshal(IN REFIID riid, IN LPUNKNOWN pUnk,
                    IN DWORD dwDestContext, IN LPVOID pvDestContext, IN DWORD mshlflags,
                    OUT LPMARSHAL FAR* ppMarshal);


WINOLEAPI CoGetStdMarshalEx(IN LPUNKNOWN pUnkOuter, IN DWORD smexflags,
                            OUT LPUNKNOWN FAR* ppUnkInner);

/* flags for CoGetStdMarshalEx */
typedef enum tagSTDMSHLFLAGS
{
    SMEXF_SERVER     = 0x01,       // server side aggregated std marshaler
    SMEXF_HANDLER    = 0x02        // client side (handler) agg std marshaler
} STDMSHLFLAGS;


WINOLEAPI_(BOOL) CoIsHandlerConnected(IN LPUNKNOWN pUnk);

// Apartment model inter-thread interface passing helpers
WINOLEAPI CoMarshalInterThreadInterfaceInStream(IN REFIID riid, IN LPUNKNOWN pUnk,
                    OUT LPSTREAM *ppStm);

WINOLEAPI CoGetInterfaceAndReleaseStream(IN LPSTREAM pStm, IN REFIID iid,
                    OUT LPVOID FAR* ppv);

WINOLEAPI CoCreateFreeThreadedMarshaler(IN LPUNKNOWN  punkOuter,
                    OUT LPUNKNOWN *ppunkMarshal);

/* dll loading helpers; keeps track of ref counts and unloads all on exit */

WINOLEAPI_(HINSTANCE) CoLoadLibrary(IN LPOLESTR lpszLibName, IN BOOL bAutoFree);
WINOLEAPI_(void) CoFreeLibrary(IN HINSTANCE hInst);
WINOLEAPI_(void) CoFreeAllLibraries(void);
WINOLEAPI_(void) CoFreeUnusedLibraries(void);
#if  (_WIN32_WINNT >= 0x0501)
/* #!perl PoundIf("CoFreeUnusedLibrariesEx", "(_WIN32_WINNT >= 0x0501)");
*/
WINOLEAPI_(void) CoFreeUnusedLibrariesEx(IN DWORD dwUnloadDelay, IN DWORD dwReserved);
#endif

#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM

/* Call Security. */

/* #!perl PoundIf("CoInitializeSecurity", "(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoInitializeSecurity(
                    IN PSECURITY_DESCRIPTOR         pSecDesc,
                    IN LONG                         cAuthSvc,
                    IN SOLE_AUTHENTICATION_SERVICE *asAuthSvc,
                    IN void                        *pReserved1,
                    IN DWORD                        dwAuthnLevel,
                    IN DWORD                        dwImpLevel,
                    IN void                        *pAuthList,
                    IN DWORD                        dwCapabilities,
                    IN void                        *pReserved3 );

/* #!perl PoundIf("CoGetCallContext", "(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoGetCallContext( IN REFIID riid, OUT void **ppInterface );

/* #!perl PoundIf("CoQueryProxyBlanket", "(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoQueryProxyBlanket(
    IN  IUnknown                  *pProxy,
    OUT DWORD                     *pwAuthnSvc,
    OUT DWORD                     *pAuthzSvc,
    OUT OLECHAR                  **pServerPrincName,
    OUT DWORD                     *pAuthnLevel,
    OUT DWORD                     *pImpLevel,
    OUT RPC_AUTH_IDENTITY_HANDLE  *pAuthInfo,
    OUT DWORD                     *pCapabilites );

/* #!perl PoundIf("CoSetProxyBlanket", "(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoSetProxyBlanket(
    IN IUnknown                 *pProxy,
    IN DWORD                     dwAuthnSvc,
    IN DWORD                     dwAuthzSvc,
    IN OLECHAR                  *pServerPrincName,
    IN DWORD                     dwAuthnLevel,
    IN DWORD                     dwImpLevel,
    IN RPC_AUTH_IDENTITY_HANDLE  pAuthInfo,
    IN DWORD                     dwCapabilities );

/* #!perl PoundIf("CoCopyProxy", "(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoCopyProxy(
    IN  IUnknown    *pProxy,
    OUT IUnknown   **ppCopy );

/* #!perl PoundIf("CoQueryClientBlanket", "(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoQueryClientBlanket(
    OUT DWORD             *pAuthnSvc,
    OUT DWORD             *pAuthzSvc,
    OUT OLECHAR          **pServerPrincName,
    OUT DWORD             *pAuthnLevel,
    OUT DWORD             *pImpLevel,
    OUT RPC_AUTHZ_HANDLE  *pPrivs,
    OUT DWORD             *pCapabilities );

/* #!perl PoundIf("CoImpersonateClient", "(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoImpersonateClient();

/* #!perl PoundIf("CoRevertToSelf", "(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoRevertToSelf();

/* #!perl PoundIf("CoQueryAuthenticationServices", "(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoQueryAuthenticationServices(
    OUT DWORD *pcAuthSvc,
    OUT SOLE_AUTHENTICATION_SERVICE **asAuthSvc );

/* #!perl PoundIf("CoSwitchCallContext", "(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoSwitchCallContext( IN IUnknown *pNewObject, OUT IUnknown **ppOldObject );

#define COM_RIGHTS_EXECUTE 1
#define COM_RIGHTS_EXECUTE_LOCAL 2
#define COM_RIGHTS_EXECUTE_REMOTE 4
#define COM_RIGHTS_ACTIVATE_LOCAL 8
#define COM_RIGHTS_ACTIVATE_REMOTE 16



#endif // DCOM

/* helper for creating instances */

WINOLEAPI CoCreateInstance(IN REFCLSID rclsid, IN LPUNKNOWN pUnkOuter,
                    IN DWORD dwClsContext, IN REFIID riid, OUT LPVOID FAR* ppv);


#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM

/* #!perl PoundIf("CoGetInstanceFromFile", "(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoGetInstanceFromFile(
    IN COSERVERINFO *              pServerInfo,
    IN CLSID       *               pClsid,
    IN IUnknown    *               punkOuter, // only relevant locally
    IN DWORD                       dwClsCtx,
    IN DWORD                       grfMode,
    IN OLECHAR *                   pwszName,
    IN DWORD                       dwCount,
    IN OUT MULTI_QI    *           pResults );

/* #!perl PoundIf("CoGetInstanceFromIStorage", "(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoGetInstanceFromIStorage(
    IN COSERVERINFO *              pServerInfo,
    IN CLSID       *               pClsid,
    IN IUnknown    *               punkOuter, // only relevant locally
    IN DWORD                       dwClsCtx,
    IN struct IStorage *           pstg,
    IN DWORD                       dwCount,
    IN OUT MULTI_QI    *           pResults );

/* #!perl PoundIf("CoCreateInstanceEx", "(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoCreateInstanceEx(
    IN REFCLSID                    Clsid,
    IN IUnknown    *               punkOuter, // only relevant locally
    IN DWORD                       dwClsCtx,
    IN COSERVERINFO *              pServerInfo,
    IN DWORD                       dwCount,
    IN OUT MULTI_QI    *           pResults );

#endif // DCOM

/* Call related APIs */
#if (_WIN32_WINNT >= 0x0500 ) || defined(_WIN32_DCOM) // DCOM

/* #!perl PoundIf("CoGetCancelObject", "(_WIN32_WINNT >= 0x0500 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoGetCancelObject(IN DWORD dwThreadId, IN REFIID iid, OUT void **ppUnk);

/* #!perl PoundIf("CoSetCancelObject", "(_WIN32_WINNT >= 0x0500 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoSetCancelObject(IN IUnknown *pUnk);

/* #!perl PoundIf("CoCancelCall", "(_WIN32_WINNT >= 0x0500 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoCancelCall(IN DWORD dwThreadId, IN ULONG ulTimeout);

/* #!perl PoundIf("CoTestCancel", "(_WIN32_WINNT >= 0x0500 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoTestCancel();

/* #!perl PoundIf("CoEnableCallCancellation", "(_WIN32_WINNT >= 0x0500 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoEnableCallCancellation(IN LPVOID pReserved);

/* #!perl PoundIf("CoDisableCallCancellation", "(_WIN32_WINNT >= 0x0500 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoDisableCallCancellation(IN LPVOID pReserved);

/* #!perl PoundIf("CoAllowSetForegroundWindow", "(_WIN32_WINNT >= 0x0500 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoAllowSetForegroundWindow(IN IUnknown *pUnk, IN LPVOID lpvReserved);

/* #!perl PoundIf("DcomChannelSetHResult", "(_WIN32_WINNT >= 0x0500 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI DcomChannelSetHResult(IN LPVOID pvReserved, IN ULONG* pulReserved, IN HRESULT appsHR);

#endif

/* other helpers */

WINOLEAPI StringFromCLSID(IN REFCLSID rclsid, OUT LPOLESTR FAR* lplpsz);
WINOLEAPI CLSIDFromString(IN LPOLESTR lpsz, OUT LPCLSID pclsid);
WINOLEAPI StringFromIID(IN REFIID rclsid, OUT LPOLESTR FAR* lplpsz);
WINOLEAPI IIDFromString(IN LPOLESTR lpsz, OUT LPIID lpiid);
WINOLEAPI_(BOOL) CoIsOle1Class(IN REFCLSID rclsid);
WINOLEAPI ProgIDFromCLSID (IN REFCLSID clsid, OUT LPOLESTR FAR* lplpszProgID);
WINOLEAPI CLSIDFromProgID (IN LPCOLESTR lpszProgID, OUT LPCLSID lpclsid);
WINOLEAPI CLSIDFromProgIDEx (IN LPCOLESTR lpszProgID, OUT LPCLSID lpclsid);
WINOLEAPI_(int) StringFromGUID2(IN REFGUID rguid, OUT LPOLESTR lpsz, IN int cchMax);

WINOLEAPI CoCreateGuid(OUT GUID FAR *pguid);

WINOLEAPI_(BOOL) CoFileTimeToDosDateTime(
                 IN FILETIME FAR* lpFileTime, OUT LPWORD lpDosDate, OUT LPWORD lpDosTime);
WINOLEAPI_(BOOL) CoDosDateTimeToFileTime(
                       IN WORD nDosDate, IN WORD nDosTime, OUT FILETIME FAR* lpFileTime);
WINOLEAPI  CoFileTimeNow( OUT FILETIME FAR* lpFileTime );


WINOLEAPI CoRegisterMessageFilter( IN LPMESSAGEFILTER lpMessageFilter,
                                OUT LPMESSAGEFILTER FAR* lplpMessageFilter );

#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
/* #!perl PoundIf("CoRegisterChannelHook", "(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoRegisterChannelHook( IN REFGUID ExtensionUuid, IN IChannelHook *pChannelHook );
#endif // DCOM

#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
/* Synchronization API */

/* #!perl PoundIf("CoWaitForMultipleHandles", "(_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM)");
*/
WINOLEAPI CoWaitForMultipleHandles (IN DWORD dwFlags,
                                    IN DWORD dwTimeout,
                                    IN ULONG cHandles,
                                    IN LPHANDLE pHandles,
                                    OUT LPDWORD  lpdwindex);

/* Flags for Synchronization API and Classes */

typedef enum tagCOWAIT_FLAGS
{
  COWAIT_WAITALL = 1,
  COWAIT_ALERTABLE = 2,
  COWAIT_INPUTAVAILABLE = 4
}COWAIT_FLAGS;

#endif // DCOM

/* for flushing OLESCM remote binding handles */

#if  (_WIN32_WINNT >= 0x0501)
/* #!perl
    PoundIf("CoInvalidateRemoteMachineBindings", "_WIN32_WINNT >= 0x0501");
*/
WINOLEAPI CoInvalidateRemoteMachineBindings(LPOLESTR pszMachineName);
#endif

/* TreatAs APIS */

WINOLEAPI CoGetTreatAsClass(IN REFCLSID clsidOld, OUT LPCLSID pClsidNew);
WINOLEAPI CoTreatAsClass(IN REFCLSID clsidOld, IN REFCLSID clsidNew);


/* the server dlls must define their DllGetClassObject and DllCanUnloadNow
 * to match these; the typedefs are located here to ensure all are changed at
 * the same time.
 */

//#ifdef _MAC
//typedef STDAPICALLTYPE HRESULT (* LPFNGETCLASSOBJECT) (REFCLSID, REFIID, LPVOID *);
//#else
typedef HRESULT (STDAPICALLTYPE * LPFNGETCLASSOBJECT) (REFCLSID, REFIID, LPVOID *);
//#endif

//#ifdef _MAC
//typedef STDAPICALLTYPE HRESULT (* LPFNCANUNLOADNOW)(void);
//#else
typedef HRESULT (STDAPICALLTYPE * LPFNCANUNLOADNOW)(void);
//#endif

STDAPI  DllGetClassObject(IN REFCLSID rclsid, IN REFIID riid, OUT LPVOID FAR* ppv);

STDAPI  DllCanUnloadNow(void);


/****** Default Memory Allocation ******************************************/
WINOLEAPI_(LPVOID) CoTaskMemAlloc(IN SIZE_T cb);
WINOLEAPI_(LPVOID) CoTaskMemRealloc(IN LPVOID pv, IN SIZE_T cb);
WINOLEAPI_(void)   CoTaskMemFree(IN LPVOID pv);

/****** DV APIs ***********************************************************/

/* This function is declared in objbase.h and ole2.h */
WINOLEAPI CreateDataAdviseHolder(OUT LPDATAADVISEHOLDER FAR* ppDAHolder);

WINOLEAPI CreateDataCache(IN LPUNKNOWN pUnkOuter, IN REFCLSID rclsid,
                                        IN REFIID iid, OUT LPVOID FAR* ppv);


/****** Storage API Prototypes ********************************************/


WINOLEAPI StgCreateDocfile(IN const OLECHAR FAR* pwcsName,
            IN DWORD grfMode,
            IN DWORD reserved,
            OUT IStorage FAR * FAR *ppstgOpen);

WINOLEAPI StgCreateDocfileOnILockBytes(IN ILockBytes FAR *plkbyt,
                    IN DWORD grfMode,
                    IN DWORD reserved,
                    OUT IStorage FAR * FAR *ppstgOpen);

WINOLEAPI StgOpenStorage(IN const OLECHAR FAR* pwcsName,
              IN  IStorage FAR *pstgPriority,
              IN  DWORD grfMode,
              IN  SNB snbExclude,
              IN  DWORD reserved,
              OUT IStorage FAR * FAR *ppstgOpen);
WINOLEAPI StgOpenStorageOnILockBytes(IN ILockBytes FAR *plkbyt,
                  IN  IStorage FAR *pstgPriority,
                  IN  DWORD grfMode,
                  IN  SNB snbExclude,
                  IN  DWORD reserved,
                  OUT IStorage FAR * FAR *ppstgOpen);

WINOLEAPI StgIsStorageFile(IN const OLECHAR FAR* pwcsName);
WINOLEAPI StgIsStorageILockBytes(IN ILockBytes FAR* plkbyt);

WINOLEAPI StgSetTimes(IN OLECHAR const FAR* lpszName,
                   IN FILETIME const FAR* pctime,
                   IN FILETIME const FAR* patime,
                   IN FILETIME const FAR* pmtime);

WINOLEAPI StgOpenAsyncDocfileOnIFillLockBytes( IN IFillLockBytes *pflb,
             IN  DWORD grfMode,
             IN  DWORD asyncFlags,
             OUT IStorage **ppstgOpen);

WINOLEAPI StgGetIFillLockBytesOnILockBytes( IN ILockBytes *pilb,
             OUT IFillLockBytes **ppflb);

WINOLEAPI StgGetIFillLockBytesOnFile(IN OLECHAR const *pwcsName,
             OUT IFillLockBytes **ppflb);


WINOLEAPI StgOpenLayoutDocfile(IN OLECHAR const *pwcsDfName,
             IN  DWORD grfMode,
             IN  DWORD reserved,
             OUT IStorage **ppstgOpen);

// STG initialization options for StgCreateStorageEx and StgOpenStorageEx
#if _WIN32_WINNT == 0x500
#define STGOPTIONS_VERSION 1
#elif _WIN32_WINNT > 0x500
#define STGOPTIONS_VERSION 2
#else
#define STGOPTIONS_VERSION 0
#endif

typedef struct tagSTGOPTIONS
{
    USHORT usVersion;            // Versions 1 and 2 supported
    USHORT reserved;             // must be 0 for padding
    ULONG ulSectorSize;          // docfile header sector size (512)
#if STGOPTIONS_VERSION >= 2
    const WCHAR *pwcsTemplateFile;  // version 2 or above 
#endif    
} STGOPTIONS;

WINOLEAPI StgCreateStorageEx (IN const WCHAR* pwcsName,
            IN  DWORD grfMode,
            IN  DWORD stgfmt,              // enum
            IN  DWORD grfAttrs,             // reserved
            IN  STGOPTIONS * pStgOptions,
            IN  void * reserved,
            IN  REFIID riid,
            OUT void ** ppObjectOpen);

WINOLEAPI StgOpenStorageEx (IN const WCHAR* pwcsName,
            IN  DWORD grfMode,
            IN  DWORD stgfmt,              // enum
            IN  DWORD grfAttrs,             // reserved
            IN  STGOPTIONS * pStgOptions,
            IN  void * reserved,
            IN  REFIID riid,
            OUT void ** ppObjectOpen);


//
//  Moniker APIs
//

WINOLEAPI  BindMoniker(IN LPMONIKER pmk, IN DWORD grfOpt, IN REFIID iidResult, OUT LPVOID FAR* ppvResult);

WINOLEAPI  CoInstall(
    IN  IBindCtx     * pbc,
    IN  DWORD          dwFlags,
    IN  uCLSSPEC     * pClassSpec,
    IN  QUERYCONTEXT * pQuery,
    IN  LPWSTR         pszCodeBase);

WINOLEAPI  CoGetObject(IN LPCWSTR pszName, IN BIND_OPTS *pBindOptions, IN REFIID riid, OUT void **ppv);
WINOLEAPI  MkParseDisplayName(IN LPBC pbc, IN LPCOLESTR szUserName,
                OUT ULONG FAR * pchEaten, OUT LPMONIKER FAR * ppmk);
WINOLEAPI  MonikerRelativePathTo(IN LPMONIKER pmkSrc, IN LPMONIKER pmkDest, OUT LPMONIKER
                FAR* ppmkRelPath, IN BOOL dwReserved);
WINOLEAPI  MonikerCommonPrefixWith(IN LPMONIKER pmkThis, IN LPMONIKER pmkOther,
                OUT LPMONIKER FAR* ppmkCommon);
WINOLEAPI  CreateBindCtx(IN DWORD reserved, OUT LPBC FAR* ppbc);
WINOLEAPI  CreateGenericComposite(IN LPMONIKER pmkFirst, IN LPMONIKER pmkRest,
    OUT LPMONIKER FAR* ppmkComposite);
WINOLEAPI  GetClassFile (IN LPCOLESTR szFilename, OUT CLSID FAR* pclsid);

WINOLEAPI  CreateClassMoniker(IN REFCLSID rclsid, OUT LPMONIKER FAR* ppmk);

WINOLEAPI  CreateFileMoniker(IN LPCOLESTR lpszPathName, OUT LPMONIKER FAR* ppmk);

WINOLEAPI  CreateItemMoniker(IN LPCOLESTR lpszDelim, IN LPCOLESTR lpszItem,
    OUT LPMONIKER FAR* ppmk);
WINOLEAPI  CreateAntiMoniker(OUT LPMONIKER FAR* ppmk);
WINOLEAPI  CreatePointerMoniker(IN LPUNKNOWN punk, OUT LPMONIKER FAR* ppmk);
WINOLEAPI  CreateObjrefMoniker(IN LPUNKNOWN punk, OUT LPMONIKER FAR * ppmk);

WINOLEAPI  GetRunningObjectTable( IN DWORD reserved, OUT LPRUNNINGOBJECTTABLE FAR* pprot);

#include <urlmon.h>
#include <propidl.h>

//
// Standard Progress Indicator impolementation
//
WINOLEAPI CreateStdProgressIndicator(IN HWND hwndParent,
                                   IN  LPCOLESTR pszTitle,
                                   IN  IBindStatusCallback * pIbscCaller,
                                   OUT IBindStatusCallback ** ppIbsc);

//12ea2135-0f75-4d97-821a-c78c710d42b8
/*#!perl
SetInsertionPoint("objbase.h", "12ea2135-0f75-4d97-821a-c78c710d42b8");
*/

#ifndef RC_INVOKED
#include <poppack.h>
#endif // RC_INVOKED

#endif     // __OBJBASE_H__

