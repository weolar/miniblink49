//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992-1999.
//
//  File:       sspi.h
//
//  Contents:   Security Support Provider Interface
//              Prototypes and structure definitions
//
//  Functions:  Security Support Provider API
//
//
//----------------------------------------------------------------------------

//#include <sdkddkver.h>

// begin_ntifs
#ifndef __SSPI_H__
#define __SSPI_H__
// end_ntifs

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
// Determine environment:
//

#ifdef SECURITY_WIN32
#define ISSP_LEVEL  32
#define ISSP_MODE   1
#endif // SECURITY_WIN32

#ifdef SECURITY_KERNEL
#define ISSP_LEVEL  32          // ntifs

//
// SECURITY_KERNEL trumps SECURITY_WIN32.  Undefine ISSP_MODE so that
// we don't get redefine errors.
//
#ifdef ISSP_MODE
#undef ISSP_MODE
#endif
#define ISSP_MODE   0           // ntifs
#endif // SECURITY_KERNEL

#ifdef SECURITY_MAC
#define ISSP_LEVEL  32
#define ISSP_MODE   1
#endif // SECURITY_MAC


#ifndef ISSP_LEVEL
#error  You must define one of SECURITY_WIN32, SECURITY_KERNEL, or
#error  SECURITY_MAC
#endif // !ISSP_LEVEL


//
// Now, define platform specific mappings:
//


// begin_ntifs

#if defined(_NO_KSECDD_IMPORT_)

#define KSECDDDECLSPEC

#else

#define KSECDDDECLSPEC __declspec(dllimport)

#endif

typedef WCHAR SEC_WCHAR;
typedef CHAR SEC_CHAR;

#ifndef __SECSTATUS_DEFINED__
typedef LONG SECURITY_STATUS;
#define __SECSTATUS_DEFINED__
#endif

#define SEC_TEXT TEXT
#define SEC_FAR
#define SEC_ENTRY __stdcall

// end_ntifs

//
// Decide what a string - 32 bits only since for 16 bits it is clear.
//


#ifdef UNICODE
typedef SEC_WCHAR * SECURITY_PSTR;
typedef CONST SEC_WCHAR * SECURITY_PCSTR;
#else // UNICODE
typedef SEC_CHAR * SECURITY_PSTR;
typedef CONST SEC_CHAR * SECURITY_PCSTR;
#endif // UNICODE



//
// Equivalent string for rpcrt:
//

#define __SEC_FAR SEC_FAR


//
// Okay, security specific types:
//


// begin_ntifs

#ifndef __SECHANDLE_DEFINED__
typedef struct _SecHandle
{
    ULONG_PTR dwLower ;
    ULONG_PTR dwUpper ;
} SecHandle, * PSecHandle ;

#define __SECHANDLE_DEFINED__
#endif // __SECHANDLE_DEFINED__

#define SecInvalidateHandle( x )    \
            ((PSecHandle) (x))->dwLower = ((PSecHandle) (x))->dwUpper = ((ULONG_PTR) ((INT_PTR)-1)) ;

#define SecIsValidHandle( x ) \
            ( ( ((PSecHandle) (x))->dwLower != ((ULONG_PTR) ((INT_PTR) -1 ))) && \
              ( ((PSecHandle) (x))->dwUpper != ((ULONG_PTR) ((INT_PTR) -1 ))) )

//
// pseudo handle value: the handle has already been deleted
//

#define SEC_DELETED_HANDLE  ((ULONG_PTR) (-2))

typedef SecHandle CredHandle;
typedef PSecHandle PCredHandle;

typedef SecHandle CtxtHandle;
typedef PSecHandle PCtxtHandle;

// end_ntifs


#  ifdef WIN32_CHICAGO

typedef unsigned __int64 QWORD;
typedef QWORD SECURITY_INTEGER, *PSECURITY_INTEGER;
#define SEC_SUCCESS(Status) ((Status) >= 0)

#  elif defined(_NTDEF_) || defined(_WINNT_)

typedef LARGE_INTEGER _SECURITY_INTEGER, SECURITY_INTEGER, *PSECURITY_INTEGER; // ntifs

#  else // _NTDEF_ || _WINNT_

typedef struct _SECURITY_INTEGER
{
    unsigned long LowPart;
    long HighPart;
} SECURITY_INTEGER, *PSECURITY_INTEGER;

#  endif // _NTDEF_ || _WINNT_

#  ifndef SECURITY_MAC

typedef SECURITY_INTEGER TimeStamp;                 // ntifs
typedef SECURITY_INTEGER * PTimeStamp;      // ntifs

#  else // SECURITY_MAC
typedef unsigned long TimeStamp;
typedef unsigned long * PTimeStamp;
#  endif // SECUIRT_MAC



//
// If we are in 32 bit mode, define the SECURITY_STRING structure,
// as a clone of the base UNICODE_STRING structure.  This is used
// internally in security components, an as the string interface
// for kernel components (e.g. FSPs)
//

#  ifndef _NTDEF_
typedef struct _SECURITY_STRING {
    unsigned short      Length;
    unsigned short      MaximumLength;
#    ifdef MIDL_PASS
    [size_is(MaximumLength / 2), length_is(Length / 2)]
#    endif // MIDL_PASS
    unsigned short *    Buffer;
} SECURITY_STRING, * PSECURITY_STRING;
#  else // _NTDEF_
typedef UNICODE_STRING SECURITY_STRING, *PSECURITY_STRING;  // ntifs
#  endif // _NTDEF_


// begin_ntifs

//
// SecPkgInfo structure
//
//  Provides general information about a security provider
//

typedef struct _SecPkgInfoW
{
    unsigned long fCapabilities;        // Capability bitmask
    unsigned short wVersion;            // Version of driver
    unsigned short wRPCID;              // ID for RPC Runtime
    unsigned long cbMaxToken;           // Size of authentication token (max)
#ifdef MIDL_PASS
    [string]
#endif
    SEC_WCHAR * Name;           // Text name

#ifdef MIDL_PASS
    [string]
#endif
    SEC_WCHAR * Comment;        // Comment
} SecPkgInfoW, * PSecPkgInfoW;

// end_ntifs

typedef struct _SecPkgInfoA
{
    unsigned long fCapabilities;        // Capability bitmask
    unsigned short wVersion;            // Version of driver
    unsigned short wRPCID;              // ID for RPC Runtime
    unsigned long cbMaxToken;           // Size of authentication token (max)
#ifdef MIDL_PASS
    [string]
#endif
    SEC_CHAR * Name;            // Text name

#ifdef MIDL_PASS
    [string]
#endif
    SEC_CHAR * Comment;         // Comment
} SecPkgInfoA, * PSecPkgInfoA;

#ifdef UNICODE
#  define SecPkgInfo SecPkgInfoW        // ntifs
#  define PSecPkgInfo PSecPkgInfoW      // ntifs
#else
#  define SecPkgInfo SecPkgInfoA
#  define PSecPkgInfo PSecPkgInfoA
#endif // !UNICODE

// begin_ntifs

//
//  Security Package Capabilities
//
#define SECPKG_FLAG_INTEGRITY                   0x00000001  // Supports integrity on messages
#define SECPKG_FLAG_PRIVACY                     0x00000002  // Supports privacy (confidentiality)
#define SECPKG_FLAG_TOKEN_ONLY                  0x00000004  // Only security token needed
#define SECPKG_FLAG_DATAGRAM                    0x00000008  // Datagram RPC support
#define SECPKG_FLAG_CONNECTION                  0x00000010  // Connection oriented RPC support
#define SECPKG_FLAG_MULTI_REQUIRED              0x00000020  // Full 3-leg required for re-auth.
#define SECPKG_FLAG_CLIENT_ONLY                 0x00000040  // Server side functionality not available
#define SECPKG_FLAG_EXTENDED_ERROR              0x00000080  // Supports extended error msgs
#define SECPKG_FLAG_IMPERSONATION               0x00000100  // Supports impersonation
#define SECPKG_FLAG_ACCEPT_WIN32_NAME           0x00000200  // Accepts Win32 names
#define SECPKG_FLAG_STREAM                      0x00000400  // Supports stream semantics
#define SECPKG_FLAG_NEGOTIABLE                  0x00000800  // Can be used by the negotiate package
#define SECPKG_FLAG_GSS_COMPATIBLE              0x00001000  // GSS Compatibility Available
#define SECPKG_FLAG_LOGON                       0x00002000  // Supports common LsaLogonUser
#define SECPKG_FLAG_ASCII_BUFFERS               0x00004000  // Token Buffers are in ASCII
#define SECPKG_FLAG_FRAGMENT                    0x00008000  // Package can fragment to fit
#define SECPKG_FLAG_MUTUAL_AUTH                 0x00010000  // Package can perform mutual authentication
#define SECPKG_FLAG_DELEGATION                  0x00020000  // Package can delegate
#define SECPKG_FLAG_READONLY_WITH_CHECKSUM      0x00040000  // Package can delegate
#define SECPKG_FLAG_RESTRICTED_TOKENS           0x00080000  // Package supports restricted callers
#define SECPKG_FLAG_NEGO_EXTENDER               0x00100000  // this package extends SPNEGO, there is at most one
#define SECPKG_FLAG_NEGOTIABLE2                 0x00200000  // this package is negotiated under the NegoExtender

#define SECPKG_ID_NONE      0xFFFF

#define __field_bcount(x) 
#define __field_ecount(x) 

//
// SecBuffer
//
//  Generic memory descriptors for buffers passed in to the security
//  API
//

typedef struct _SecBuffer {
    unsigned long cbBuffer;             // Size of the buffer, in bytes
    unsigned long BufferType;           // Type of the buffer (below)
#ifdef MIDL_PASS
    [size_is(cbBuffer)] char * pvBuffer;                         // Pointer to the buffer
#else
    __field_bcount(cbBuffer) void SEC_FAR * pvBuffer;            // Pointer to the buffer
#endif
} SecBuffer, * PSecBuffer;

typedef struct _SecBufferDesc {
    unsigned long ulVersion;            // Version number
    unsigned long cBuffers;             // Number of buffers
#ifdef MIDL_PASS
    [size_is(cBuffers)]
#endif
    __field_ecount(cBuffers) PSecBuffer pBuffers;                // Pointer to array of buffers
} SecBufferDesc, SEC_FAR * PSecBufferDesc;

#define SECBUFFER_VERSION           0

#define SECBUFFER_EMPTY             0   // Undefined, replaced by provider
#define SECBUFFER_DATA              1   // Packet data
#define SECBUFFER_TOKEN             2   // Security token
#define SECBUFFER_PKG_PARAMS        3   // Package specific parameters
#define SECBUFFER_MISSING           4   // Missing Data indicator
#define SECBUFFER_EXTRA             5   // Extra data
#define SECBUFFER_STREAM_TRAILER    6   // Security Trailer
#define SECBUFFER_STREAM_HEADER     7   // Security Header
#define SECBUFFER_NEGOTIATION_INFO  8   // Hints from the negotiation pkg
#define SECBUFFER_PADDING           9   // non-data padding
#define SECBUFFER_STREAM            10  // whole encrypted message
#define SECBUFFER_MECHLIST          11
#define SECBUFFER_MECHLIST_SIGNATURE 12
#define SECBUFFER_TARGET            13  // obsolete
#define SECBUFFER_CHANNEL_BINDINGS  14
#define SECBUFFER_CHANGE_PASS_RESPONSE 15
#define SECBUFFER_TARGET_HOST       16
#define SECBUFFER_ALERT             17

#define SECBUFFER_ATTRMASK                      0xF0000000
#define SECBUFFER_READONLY                      0x80000000  // Buffer is read-only, no checksum
#define SECBUFFER_READONLY_WITH_CHECKSUM        0x10000000  // Buffer is read-only, and checksummed
#define SECBUFFER_RESERVED                      0x60000000  // Flags reserved to security system


typedef struct _SEC_NEGOTIATION_INFO {
    unsigned long       Size;           // Size of this structure
    unsigned long       NameLength;     // Length of name hint
    SEC_WCHAR * Name;           // Name hint
    void *      Reserved;       // Reserved
} SEC_NEGOTIATION_INFO, * PSEC_NEGOTIATION_INFO ;

typedef struct _SEC_CHANNEL_BINDINGS {
    unsigned long  dwInitiatorAddrType;
    unsigned long  cbInitiatorLength;
    unsigned long  dwInitiatorOffset;
    unsigned long  dwAcceptorAddrType;
    unsigned long  cbAcceptorLength;
    unsigned long  dwAcceptorOffset;
    unsigned long  cbApplicationDataLength;
    unsigned long  dwApplicationDataOffset;
} SEC_CHANNEL_BINDINGS, * PSEC_CHANNEL_BINDINGS ;


//
//  Data Representation Constant:
//
#define SECURITY_NATIVE_DREP        0x00000010
#define SECURITY_NETWORK_DREP       0x00000000

//
//  Credential Use Flags
//
#define SECPKG_CRED_INBOUND         0x00000001
#define SECPKG_CRED_OUTBOUND        0x00000002
#define SECPKG_CRED_BOTH            0x00000003
#define SECPKG_CRED_DEFAULT         0x00000004
#define SECPKG_CRED_RESERVED        0xF0000000

//
//  SSP SHOULD prompt the user for credentials/consent, independent
//  of whether credentials to be used are the 'logged on' credentials
//  or retrieved from credman.
//
//  An SSP may choose not to prompt, however, in circumstances determined
//  by the SSP.
//

#define SECPKG_CRED_AUTOLOGON_RESTRICTED    0x00000010

//
// auth will always fail, ISC() is called to process policy data only
//

#define SECPKG_CRED_PROCESS_POLICY_ONLY     0x00000020

//
//  InitializeSecurityContext Requirement and return flags:
//

#define ISC_REQ_DELEGATE                0x00000001
#define ISC_REQ_MUTUAL_AUTH             0x00000002
#define ISC_REQ_REPLAY_DETECT           0x00000004
#define ISC_REQ_SEQUENCE_DETECT         0x00000008
#define ISC_REQ_CONFIDENTIALITY         0x00000010
#define ISC_REQ_USE_SESSION_KEY         0x00000020
#define ISC_REQ_PROMPT_FOR_CREDS        0x00000040
#define ISC_REQ_USE_SUPPLIED_CREDS      0x00000080
#define ISC_REQ_ALLOCATE_MEMORY         0x00000100
#define ISC_REQ_USE_DCE_STYLE           0x00000200
#define ISC_REQ_DATAGRAM                0x00000400
#define ISC_REQ_CONNECTION              0x00000800
#define ISC_REQ_CALL_LEVEL              0x00001000
#define ISC_REQ_FRAGMENT_SUPPLIED       0x00002000
#define ISC_REQ_EXTENDED_ERROR          0x00004000
#define ISC_REQ_STREAM                  0x00008000
#define ISC_REQ_INTEGRITY               0x00010000
#define ISC_REQ_IDENTIFY                0x00020000
#define ISC_REQ_NULL_SESSION            0x00040000
#define ISC_REQ_MANUAL_CRED_VALIDATION  0x00080000
#define ISC_REQ_RESERVED1               0x00100000
#define ISC_REQ_FRAGMENT_TO_FIT         0x00200000
// This exists only in Windows Vista and greater
#define ISC_REQ_FORWARD_CREDENTIALS     0x00400000
#define ISC_REQ_NO_INTEGRITY            0x00800000 // honored only by SPNEGO
#define ISC_REQ_USE_HTTP_STYLE          0x01000000

#define ISC_RET_DELEGATE                0x00000001
#define ISC_RET_MUTUAL_AUTH             0x00000002
#define ISC_RET_REPLAY_DETECT           0x00000004
#define ISC_RET_SEQUENCE_DETECT         0x00000008
#define ISC_RET_CONFIDENTIALITY         0x00000010
#define ISC_RET_USE_SESSION_KEY         0x00000020
#define ISC_RET_USED_COLLECTED_CREDS    0x00000040
#define ISC_RET_USED_SUPPLIED_CREDS     0x00000080
#define ISC_RET_ALLOCATED_MEMORY        0x00000100
#define ISC_RET_USED_DCE_STYLE          0x00000200
#define ISC_RET_DATAGRAM                0x00000400
#define ISC_RET_CONNECTION              0x00000800
#define ISC_RET_INTERMEDIATE_RETURN     0x00001000
#define ISC_RET_CALL_LEVEL              0x00002000
#define ISC_RET_EXTENDED_ERROR          0x00004000
#define ISC_RET_STREAM                  0x00008000
#define ISC_RET_INTEGRITY               0x00010000
#define ISC_RET_IDENTIFY                0x00020000
#define ISC_RET_NULL_SESSION            0x00040000
#define ISC_RET_MANUAL_CRED_VALIDATION  0x00080000
#define ISC_RET_RESERVED1               0x00100000
#define ISC_RET_FRAGMENT_ONLY           0x00200000
// This exists only in Windows Vista and greater
#define ISC_RET_FORWARD_CREDENTIALS     0x00400000

#define ISC_RET_USED_HTTP_STYLE         0x01000000
#define ISC_RET_NO_ADDITIONAL_TOKEN     0x02000000  // *INTERNAL*
#define ISC_RET_REAUTHENTICATION        0x08000000  // *INTERNAL*

#define ASC_REQ_DELEGATE                0x00000001
#define ASC_REQ_MUTUAL_AUTH             0x00000002
#define ASC_REQ_REPLAY_DETECT           0x00000004
#define ASC_REQ_SEQUENCE_DETECT         0x00000008
#define ASC_REQ_CONFIDENTIALITY         0x00000010
#define ASC_REQ_USE_SESSION_KEY         0x00000020
#define ASC_REQ_ALLOCATE_MEMORY         0x00000100
#define ASC_REQ_USE_DCE_STYLE           0x00000200
#define ASC_REQ_DATAGRAM                0x00000400
#define ASC_REQ_CONNECTION              0x00000800
#define ASC_REQ_CALL_LEVEL              0x00001000
#define ASC_REQ_EXTENDED_ERROR          0x00008000
#define ASC_REQ_STREAM                  0x00010000
#define ASC_REQ_INTEGRITY               0x00020000
#define ASC_REQ_LICENSING               0x00040000
#define ASC_REQ_IDENTIFY                0x00080000
#define ASC_REQ_ALLOW_NULL_SESSION      0x00100000
#define ASC_REQ_ALLOW_NON_USER_LOGONS   0x00200000
#define ASC_REQ_ALLOW_CONTEXT_REPLAY    0x00400000
#define ASC_REQ_FRAGMENT_TO_FIT         0x00800000
#define ASC_REQ_FRAGMENT_SUPPLIED       0x00002000
#define ASC_REQ_NO_TOKEN                0x01000000
#define ASC_REQ_PROXY_BINDINGS          0x04000000
//      SSP_RET_REAUTHENTICATION        0x08000000  // *INTERNAL*
#define ASC_REQ_ALLOW_MISSING_BINDINGS  0x10000000

#define ASC_RET_DELEGATE                0x00000001
#define ASC_RET_MUTUAL_AUTH             0x00000002
#define ASC_RET_REPLAY_DETECT           0x00000004
#define ASC_RET_SEQUENCE_DETECT         0x00000008
#define ASC_RET_CONFIDENTIALITY         0x00000010
#define ASC_RET_USE_SESSION_KEY         0x00000020
#define ASC_RET_ALLOCATED_MEMORY        0x00000100
#define ASC_RET_USED_DCE_STYLE          0x00000200
#define ASC_RET_DATAGRAM                0x00000400
#define ASC_RET_CONNECTION              0x00000800
#define ASC_RET_CALL_LEVEL              0x00002000 // skipped 1000 to be like ISC_
#define ASC_RET_THIRD_LEG_FAILED        0x00004000
#define ASC_RET_EXTENDED_ERROR          0x00008000
#define ASC_RET_STREAM                  0x00010000
#define ASC_RET_INTEGRITY               0x00020000
#define ASC_RET_LICENSING               0x00040000
#define ASC_RET_IDENTIFY                0x00080000
#define ASC_RET_NULL_SESSION            0x00100000
#define ASC_RET_ALLOW_NON_USER_LOGONS   0x00200000
#define ASC_RET_ALLOW_CONTEXT_REPLAY    0x00400000  // deprecated - don't use this flag!!!
#define ASC_RET_FRAGMENT_ONLY           0x00800000
#define ASC_RET_NO_TOKEN                0x01000000
#define ASC_RET_NO_ADDITIONAL_TOKEN     0x02000000  // *INTERNAL*
#define ASC_RET_NO_PROXY_BINDINGS       0x04000000
//      SSP_RET_REAUTHENTICATION        0x08000000  // *INTERNAL*
#define ASC_RET_MISSING_BINDINGS        0x10000000

//
//  Security Credentials Attributes:
//

#define SECPKG_CRED_ATTR_NAMES        1
#define SECPKG_CRED_ATTR_SSI_PROVIDER 2

typedef struct _SecPkgCredentials_NamesW
{
#ifdef MIDL_PASS
    [string]
#endif
    SEC_WCHAR * sUserName;

} SecPkgCredentials_NamesW, * PSecPkgCredentials_NamesW;

// end_ntifs

typedef struct _SecPkgCredentials_NamesA
{
#ifdef MIDL_PASS
    [string]
#endif
    SEC_CHAR * sUserName;

} SecPkgCredentials_NamesA, * PSecPkgCredentials_NamesA;

#ifdef UNICODE
#  define SecPkgCredentials_Names SecPkgCredentials_NamesW      // ntifs
#  define PSecPkgCredentials_Names PSecPkgCredentials_NamesW    // ntifs
#else
#  define SecPkgCredentials_Names SecPkgCredentials_NamesA
#  define PSecPkgCredentials_Names PSecPkgCredentials_NamesA
#endif // !UNICODE

// begin_ntifs

#if NTDDI_VERSION > NTDDI_WS03
typedef struct _SecPkgCredentials_SSIProviderW
{
    SEC_WCHAR * sProviderName;
    unsigned long       ProviderInfoLength;
    char *      ProviderInfo;
} SecPkgCredentials_SSIProviderW, * PSecPkgCredentials_SSIProviderW;
#endif // End W2k3SP1 and greater
// end_ntifs

typedef struct _SecPkgCredentials_SSIProviderA
{
    SEC_CHAR  * sProviderName;
    unsigned long       ProviderInfoLength;
    char *      ProviderInfo;
} SecPkgCredentials_SSIProviderA, * PSecPkgCredentials_SSIProviderA;

#ifdef UNICODE
#  define SecPkgCredentials_SSIProvider SecPkgCredentials_SSIProviderW      // ntifs
#  define PSecPkgCredentials_SSIProvider PSecPkgCredentials_SSIProviderW    // ntifs
#else
#  define SecPkgCredentials_SSIProvider SecPkgCredentials_SSIProviderA
#  define PSecPkgCredentials_SSIProvider PSecPkgCredentials_SSIProviderA
#endif // !UNICODE

// begin_ntifs

//
//  Security Context Attributes:
//

#define SECPKG_ATTR_SIZES           0
#define SECPKG_ATTR_NAMES           1
#define SECPKG_ATTR_LIFESPAN        2
#define SECPKG_ATTR_DCE_INFO        3
#define SECPKG_ATTR_STREAM_SIZES    4
#define SECPKG_ATTR_KEY_INFO        5
#define SECPKG_ATTR_AUTHORITY       6
#define SECPKG_ATTR_PROTO_INFO      7
#define SECPKG_ATTR_PASSWORD_EXPIRY 8
#define SECPKG_ATTR_SESSION_KEY     9
#define SECPKG_ATTR_PACKAGE_INFO    10
#define SECPKG_ATTR_USER_FLAGS      11
#define SECPKG_ATTR_NEGOTIATION_INFO 12
#define SECPKG_ATTR_NATIVE_NAMES    13
#define SECPKG_ATTR_FLAGS           14
// These attributes exist only in Win XP and greater
#define SECPKG_ATTR_USE_VALIDATED   15
#define SECPKG_ATTR_CREDENTIAL_NAME 16
#define SECPKG_ATTR_TARGET_INFORMATION 17
#define SECPKG_ATTR_ACCESS_TOKEN    18
// These attributes exist only in Win2K3 and greater
#define SECPKG_ATTR_TARGET          19
#define SECPKG_ATTR_AUTHENTICATION_ID  20
// These attributes exist only in Win2K3SP1 and greater
#define SECPKG_ATTR_LOGOFF_TIME     21
//
// win7 or greater
//
#define SECPKG_ATTR_NEGO_KEYS         22
#define SECPKG_ATTR_PROMPTING_NEEDED  24
#define SECPKG_ATTR_UNIQUE_BINDINGS   25
#define SECPKG_ATTR_ENDPOINT_BINDINGS 26
#define SECPKG_ATTR_CLIENT_SPECIFIED_TARGET 27

#define SECPKG_ATTR_LAST_CLIENT_TOKEN_STATUS 30
#define SECPKG_ATTR_NEGO_PKG_INFO        31 // contains nego info of packages
#define SECPKG_ATTR_NEGO_STATUS          32 // contains the last error
#define SECPKG_ATTR_CONTEXT_DELETED      33 // a context has been deleted

#define SECPKG_ATTR_SUBJECT_SECURITY_ATTRIBUTES 128

typedef struct _SecPkgContext_SubjectAttributes { 
    void* AttributeInfo; // contains a PAUTHZ_SECURITY_ATTRIBUTES_INFORMATION structure
} SecPkgContext_SubjectAttributes, *PSecPkgContext_SubjectAttributes;

#define SECPKG_ATTR_NEGO_INFO_FLAG_NO_KERBEROS 0x1
#define SECPKG_ATTR_NEGO_INFO_FLAG_NO_NTLM     0x2

//
// types of credentials, used by SECPKG_ATTR_PROMPTING_NEEDED
//

typedef enum _SECPKG_CRED_CLASS {
    SecPkgCredClass_None = 0,  // no creds
    SecPkgCredClass_Ephemeral = 10,  // logon creds
    SecPkgCredClass_PersistedGeneric = 20, // saved creds, not target specific
    SecPkgCredClass_PersistedSpecific = 30, // saved creds, target specific
    SecPkgCredClass_Explicit = 40, // explicitly supplied creds
} SECPKG_CRED_CLASS, * PSECPKG_CRED_CLASS;

typedef struct _SecPkgContext_CredInfo {
    SECPKG_CRED_CLASS CredClass;
    unsigned long IsPromptingNeeded;
} SecPkgContext_CredInfo, *PSecPkgContext_CredInfo;

typedef struct _SecPkgContext_NegoPackageInfo
{
    unsigned long PackageMask;
} SecPkgContext_NegoPackageInfo, * PSecPkgContext_NegoPackageInfo;

typedef struct _SecPkgContext_NegoStatus
{
    unsigned long LastStatus;
} SecPkgContext_NegoStatus, * PSecPkgContext_NegoStatus;

typedef struct _SecPkgContext_Sizes
{
    unsigned long cbMaxToken;
    unsigned long cbMaxSignature;
    unsigned long cbBlockSize;
    unsigned long cbSecurityTrailer;
} SecPkgContext_Sizes, * PSecPkgContext_Sizes;

typedef struct _SecPkgContext_StreamSizes
{
    unsigned long   cbHeader;
    unsigned long   cbTrailer;
    unsigned long   cbMaximumMessage;
    unsigned long   cBuffers;
    unsigned long   cbBlockSize;
} SecPkgContext_StreamSizes, * PSecPkgContext_StreamSizes;

typedef struct _SecPkgContext_NamesW
{
    SEC_WCHAR * sUserName;
} SecPkgContext_NamesW, * PSecPkgContext_NamesW;

// end_ntifs

typedef enum _SECPKG_ATTR_LCT_STATUS {
    SecPkgAttrLastClientTokenYes,
    SecPkgAttrLastClientTokenNo,
    SecPkgAttrLastClientTokenMaybe
} SECPKG_ATTR_LCT_STATUS, * PSECPKG_ATTR_LCT_STATUS;


typedef struct _SecPkgContext_LastClientTokenStatus {
    SECPKG_ATTR_LCT_STATUS LastClientTokenStatus;
} SecPkgContext_LastClientTokenStatus, * PSecPkgContext_LastClientTokenStatus;

typedef struct _SecPkgContext_NamesA
{
    SEC_CHAR * sUserName;
} SecPkgContext_NamesA, * PSecPkgContext_NamesA;

#ifdef UNICODE
#  define SecPkgContext_Names SecPkgContext_NamesW          // ntifs
#  define PSecPkgContext_Names PSecPkgContext_NamesW        // ntifs
#else
#  define SecPkgContext_Names SecPkgContext_NamesA
#  define PSecPkgContext_Names PSecPkgContext_NamesA
#endif // !UNICODE

// begin_ntifs

typedef struct _SecPkgContext_Lifespan
{
    TimeStamp tsStart;
    TimeStamp tsExpiry;
} SecPkgContext_Lifespan, * PSecPkgContext_Lifespan;

typedef struct _SecPkgContext_DceInfo
{
    unsigned long AuthzSvc;
    void * pPac;
} SecPkgContext_DceInfo, * PSecPkgContext_DceInfo;

// end_ntifs

typedef struct _SecPkgContext_KeyInfoA
{
    SEC_CHAR *  sSignatureAlgorithmName;
    SEC_CHAR *  sEncryptAlgorithmName;
    unsigned long       KeySize;
    unsigned long       SignatureAlgorithm;
    unsigned long       EncryptAlgorithm;
} SecPkgContext_KeyInfoA, * PSecPkgContext_KeyInfoA;

// begin_ntifs

typedef struct _SecPkgContext_KeyInfoW
{
    SEC_WCHAR * sSignatureAlgorithmName;
    SEC_WCHAR * sEncryptAlgorithmName;
    unsigned long       KeySize;
    unsigned long       SignatureAlgorithm;
    unsigned long       EncryptAlgorithm;
} SecPkgContext_KeyInfoW, * PSecPkgContext_KeyInfoW;

// end_ntifs

#ifdef UNICODE
#define SecPkgContext_KeyInfo   SecPkgContext_KeyInfoW      // ntifs
#define PSecPkgContext_KeyInfo  PSecPkgContext_KeyInfoW     // ntifs
#else
#define SecPkgContext_KeyInfo   SecPkgContext_KeyInfoA
#define PSecPkgContext_KeyInfo  PSecPkgContext_KeyInfoA
#endif

typedef struct _SecPkgContext_AuthorityA
{
    SEC_CHAR *  sAuthorityName;
} SecPkgContext_AuthorityA, * PSecPkgContext_AuthorityA;

// begin_ntifs

typedef struct _SecPkgContext_AuthorityW
{
    SEC_WCHAR * sAuthorityName;
} SecPkgContext_AuthorityW, * PSecPkgContext_AuthorityW;

// end_ntifs

#ifdef UNICODE
#define SecPkgContext_Authority SecPkgContext_AuthorityW        // ntifs
#define PSecPkgContext_Authority    PSecPkgContext_AuthorityW   // ntifs
#else
#define SecPkgContext_Authority SecPkgContext_AuthorityA
#define PSecPkgContext_Authority    PSecPkgContext_AuthorityA
#endif

typedef struct _SecPkgContext_ProtoInfoA
{
    SEC_CHAR *  sProtocolName;
    unsigned long       majorVersion;
    unsigned long       minorVersion;
} SecPkgContext_ProtoInfoA, * PSecPkgContext_ProtoInfoA;

// begin_ntifs

typedef struct _SecPkgContext_ProtoInfoW
{
    SEC_WCHAR * sProtocolName;
    unsigned long majorVersion;
    unsigned long minorVersion;
} SecPkgContext_ProtoInfoW, * PSecPkgContext_ProtoInfoW;

// end_ntifs

#ifdef UNICODE
#define SecPkgContext_ProtoInfo   SecPkgContext_ProtoInfoW      // ntifs
#define PSecPkgContext_ProtoInfo  PSecPkgContext_ProtoInfoW     // ntifs
#else
#define SecPkgContext_ProtoInfo   SecPkgContext_ProtoInfoA
#define PSecPkgContext_ProtoInfo  PSecPkgContext_ProtoInfoA
#endif

// begin_ntifs

typedef struct _SecPkgContext_PasswordExpiry
{
    TimeStamp tsPasswordExpires;
} SecPkgContext_PasswordExpiry, * PSecPkgContext_PasswordExpiry;

#if NTDDI_VERSION > NTDDI_WS03
typedef struct _SecPkgContext_LogoffTime
{
    TimeStamp tsLogoffTime;
} SecPkgContext_LogoffTime, * PSecPkgContext_LogoffTime;
#endif // Greater than Windows Server 2003 RTM (SP1 and greater contains this)

typedef struct _SecPkgContext_SessionKey
{
    unsigned long SessionKeyLength;
    __field_bcount(SessionKeyLength) unsigned char * SessionKey;
} SecPkgContext_SessionKey, *PSecPkgContext_SessionKey;

// used by nego2
typedef struct _SecPkgContext_NegoKeys
{
  unsigned long KeyType;
  unsigned short KeyLength;
  __field_bcount(KeyLength) unsigned char* KeyValue;
  unsigned long  VerifyKeyType;
  unsigned short VerifyKeyLength;
  __field_bcount(VerifyKeyLength) unsigned char* VerifyKeyValue;
} SecPkgContext_NegoKeys, * PSecPkgContext_NegoKeys;

typedef struct _SecPkgContext_PackageInfoW
{
    PSecPkgInfoW PackageInfo;
} SecPkgContext_PackageInfoW, * PSecPkgContext_PackageInfoW;

// end_ntifs

typedef struct _SecPkgContext_PackageInfoA
{
    PSecPkgInfoA PackageInfo;
} SecPkgContext_PackageInfoA, * PSecPkgContext_PackageInfoA;

// begin_ntifs

typedef struct _SecPkgContext_UserFlags
{
    unsigned long UserFlags;
} SecPkgContext_UserFlags, * PSecPkgContext_UserFlags;

typedef struct _SecPkgContext_Flags
{
    unsigned long Flags;
} SecPkgContext_Flags, * PSecPkgContext_Flags;

// end_ntifs

#ifdef UNICODE
#define SecPkgContext_PackageInfo   SecPkgContext_PackageInfoW      // ntifs
#define PSecPkgContext_PackageInfo  PSecPkgContext_PackageInfoW     // ntifs
#else
#define SecPkgContext_PackageInfo   SecPkgContext_PackageInfoA
#define PSecPkgContext_PackageInfo  PSecPkgContext_PackageInfoA
#endif


typedef struct _SecPkgContext_NegotiationInfoA
{
    PSecPkgInfoA    PackageInfo ;
    unsigned long   NegotiationState ;
} SecPkgContext_NegotiationInfoA, * PSecPkgContext_NegotiationInfoA ;

// begin_ntifs
typedef struct _SecPkgContext_NegotiationInfoW
{
    PSecPkgInfoW    PackageInfo ;
    unsigned long   NegotiationState ;
} SecPkgContext_NegotiationInfoW, * PSecPkgContext_NegotiationInfoW ;

// end_ntifs

#ifdef UNICODE
#define SecPkgContext_NegotiationInfo   SecPkgContext_NegotiationInfoW
#define PSecPkgContext_NegotiationInfo  PSecPkgContext_NegotiationInfoW
#else
#define SecPkgContext_NegotiationInfo   SecPkgContext_NegotiationInfoA
#define PSecPkgContext_NegotiationInfo  PSecPkgContext_NegotiationInfoA
#endif

#define SECPKG_NEGOTIATION_COMPLETE             0
#define SECPKG_NEGOTIATION_OPTIMISTIC           1
#define SECPKG_NEGOTIATION_IN_PROGRESS          2
#define SECPKG_NEGOTIATION_DIRECT               3
#define SECPKG_NEGOTIATION_TRY_MULTICRED        4


typedef struct _SecPkgContext_NativeNamesW
{
    SEC_WCHAR * sClientName;
    SEC_WCHAR * sServerName;
} SecPkgContext_NativeNamesW, * PSecPkgContext_NativeNamesW;

typedef struct _SecPkgContext_NativeNamesA
{
    SEC_CHAR * sClientName;
    SEC_CHAR * sServerName;
} SecPkgContext_NativeNamesA, * PSecPkgContext_NativeNamesA;


#ifdef UNICODE
#  define SecPkgContext_NativeNames SecPkgContext_NativeNamesW          // ntifs
#  define PSecPkgContext_NativeNames PSecPkgContext_NativeNamesW        // ntifs
#else
#  define SecPkgContext_NativeNames SecPkgContext_NativeNamesA
#  define PSecPkgContext_NativeNames PSecPkgContext_NativeNamesA
#endif // !UNICODE

// begin_ntifs

#if 1 // OSVER(NTDDI_VERSION) > NTDDI_WIN2K

typedef struct _SecPkgContext_CredentialNameW
{
    unsigned long CredentialType;
    SEC_WCHAR *sCredentialName;
} SecPkgContext_CredentialNameW, * PSecPkgContext_CredentialNameW;

#endif // Later than win2k
// end_ntifs

typedef struct _SecPkgContext_CredentialNameA
{
    unsigned long CredentialType;
    SEC_CHAR *sCredentialName;
} SecPkgContext_CredentialNameA, * PSecPkgContext_CredentialNameA;

#ifdef UNICODE
#  define SecPkgContext_CredentialName SecPkgContext_CredentialNameW          // ntifs
#  define PSecPkgContext_CredentialName PSecPkgContext_CredentialNameW        // ntifs
#else
#  define SecPkgContext_CredentialName SecPkgContext_CredentialNameA
#  define PSecPkgContext_CredentialName PSecPkgContext_CredentialNameA
#endif // !UNICODE

typedef struct _SecPkgContext_AccessToken
{
    void * AccessToken;
} SecPkgContext_AccessToken, * PSecPkgContext_AccessToken;

typedef struct _SecPkgContext_TargetInformation
{
    unsigned long MarshalledTargetInfoLength;
    unsigned char * MarshalledTargetInfo;

} SecPkgContext_TargetInformation, * PSecPkgContext_TargetInformation;

typedef struct _SecPkgContext_AuthzID
{
    unsigned long AuthzIDLength;
    char * AuthzID;

} SecPkgContext_AuthzID, * PSecPkgContext_AuthzID;

typedef struct _SecPkgContext_Target
{
    unsigned long TargetLength;
    char * Target;

} SecPkgContext_Target, * PSecPkgContext_Target;


typedef struct _SecPkgContext_ClientSpecifiedTarget
{
    SEC_WCHAR * sTargetName;
} SecPkgContext_ClientSpecifiedTarget, * PSecPkgContext_ClientSpecifiedTarget;

typedef struct _SecPkgContext_Bindings
{
    unsigned long BindingsLength;
    __field_bcount(BindingsLength) SEC_CHANNEL_BINDINGS * Bindings;
} SecPkgContext_Bindings, * PSecPkgContext_Bindings;


// begin_ntifs

typedef void
(SEC_ENTRY * SEC_GET_KEY_FN) (
    void * Arg,                 // Argument passed in
    void * Principal,           // Principal ID
    unsigned long KeyVer,               // Key Version
    void * * Key,       // Returned ptr to key
    SECURITY_STATUS * Status    // returned status
    );

//
// Flags for ExportSecurityContext
//

#define SECPKG_CONTEXT_EXPORT_RESET_NEW         0x00000001      // New context is reset to initial state
#define SECPKG_CONTEXT_EXPORT_DELETE_OLD        0x00000002      // Old context is deleted during export
// This is only valid in W2K3SP1 and greater
#define SECPKG_CONTEXT_EXPORT_TO_KERNEL         0x00000004      // Context is to be transferred to the kernel


KSECDDDECLSPEC
SECURITY_STATUS SEC_ENTRY
AcquireCredentialsHandleW(
#if ISSP_MODE == 0     // For Kernel mode
    __in_opt  PSECURITY_STRING pPrincipal,
    __in      PSECURITY_STRING pPackage,
#else
    __in_opt  LPWSTR pszPrincipal,                // Name of principal
    __in      LPWSTR pszPackage,                  // Name of package
#endif
    __in      unsigned long fCredentialUse,       // Flags indicating use
    __in_opt  void * pvLogonId,           // Pointer to logon ID
    __in_opt  void * pAuthData,           // Package specific data
    __in_opt  SEC_GET_KEY_FN pGetKeyFn,           // Pointer to GetKey() func
    __in_opt  void * pvGetKeyArgument,    // Value to pass to GetKey()
    __out     PCredHandle phCredential,           // (out) Cred Handle
    __out_opt PTimeStamp ptsExpiry                // (out) Lifetime (optional)
    );

typedef SECURITY_STATUS
(SEC_ENTRY * ACQUIRE_CREDENTIALS_HANDLE_FN_W)(
#if ISSP_MODE == 0
    PSECURITY_STRING,
    PSECURITY_STRING,
#else
    SEC_WCHAR *,
    SEC_WCHAR *,
#endif
    unsigned long,
    void *,
    void *,
    SEC_GET_KEY_FN,
    void *,
    PCredHandle,
    PTimeStamp);

// end_ntifs

SECURITY_STATUS SEC_ENTRY
AcquireCredentialsHandleA(
    __in_opt  LPSTR pszPrincipal,                 // Name of principal
    __in      LPSTR pszPackage,                   // Name of package
    __in      unsigned long fCredentialUse,       // Flags indicating use
    __in_opt  void * pvLogonId,           // Pointer to logon ID
    __in_opt  void * pAuthData,           // Package specific data
    __in_opt  SEC_GET_KEY_FN pGetKeyFn,           // Pointer to GetKey() func
    __in_opt  void * pvGetKeyArgument,    // Value to pass to GetKey()
    __out     PCredHandle phCredential,           // (out) Cred Handle
    __out_opt PTimeStamp ptsExpiry                // (out) Lifetime (optional)
    );

typedef SECURITY_STATUS
(SEC_ENTRY * ACQUIRE_CREDENTIALS_HANDLE_FN_A)(
    SEC_CHAR *,
    SEC_CHAR *,
    unsigned long,
    void *,
    void *,
    SEC_GET_KEY_FN,
    void *,
    PCredHandle,
    PTimeStamp);

#ifdef UNICODE
#  define AcquireCredentialsHandle AcquireCredentialsHandleW            // ntifs
#  define ACQUIRE_CREDENTIALS_HANDLE_FN ACQUIRE_CREDENTIALS_HANDLE_FN_W // ntifs
#else
#  define AcquireCredentialsHandle AcquireCredentialsHandleA
#  define ACQUIRE_CREDENTIALS_HANDLE_FN ACQUIRE_CREDENTIALS_HANDLE_FN_A
#endif // !UNICODE

// begin_ntifs

KSECDDDECLSPEC
SECURITY_STATUS SEC_ENTRY
FreeCredentialsHandle(
    __in PCredHandle phCredential            // Handle to free
    );

typedef SECURITY_STATUS
(SEC_ENTRY * FREE_CREDENTIALS_HANDLE_FN)(
    PCredHandle );

KSECDDDECLSPEC
SECURITY_STATUS SEC_ENTRY
AddCredentialsW(
    __in      PCredHandle hCredentials,
#if ISSP_MODE == 0      // For Kernel mode
    __in_opt  PSECURITY_STRING pPrincipal,
    __in      PSECURITY_STRING pPackage,
#else
    __in_opt  LPWSTR pszPrincipal,                // Name of principal
    __in      LPWSTR pszPackage,                  // Name of package
#endif
    __in      unsigned long fCredentialUse,       // Flags indicating use
    __in_opt  void * pAuthData,           // Package specific data
    __in_opt  SEC_GET_KEY_FN pGetKeyFn,           // Pointer to GetKey() func
    __in_opt  void * pvGetKeyArgument,    // Value to pass to GetKey()
    __out_opt PTimeStamp ptsExpiry                // (out) Lifetime (optional)
    );

typedef SECURITY_STATUS
(SEC_ENTRY * ADD_CREDENTIALS_FN_W)(
    PCredHandle,
#if ISSP_MODE == 0
    PSECURITY_STRING,
    PSECURITY_STRING,
#else
    SEC_WCHAR *,
    SEC_WCHAR *,
#endif
    unsigned long,
    void *,
    SEC_GET_KEY_FN,
    void *,
    PTimeStamp);

SECURITY_STATUS SEC_ENTRY
AddCredentialsA(
    __in PCredHandle hCredentials,
    __in_opt LPSTR pszPrincipal,             // Name of principal
    __in LPSTR pszPackage,                   // Name of package
    __in unsigned long fCredentialUse,       // Flags indicating use
    __in_opt void * pAuthData,           // Package specific data
    __in_opt SEC_GET_KEY_FN pGetKeyFn,           // Pointer to GetKey() func
    __in_opt void * pvGetKeyArgument,    // Value to pass to GetKey()
    __out_opt PTimeStamp ptsExpiry                // (out) Lifetime (optional)
    );

typedef SECURITY_STATUS
(SEC_ENTRY * ADD_CREDENTIALS_FN_A)(
    PCredHandle,
    SEC_CHAR *,
    SEC_CHAR *,
    unsigned long,
    void *,
    SEC_GET_KEY_FN,
    void *,
    PTimeStamp);

#ifdef UNICODE
#define AddCredentials  AddCredentialsW
#define ADD_CREDENTIALS_FN  ADD_CREDENTIALS_FN_W
#else
#define AddCredentials  AddCredentialsA
#define ADD_CREDENTIALS_FN ADD_CREDENTIALS_FN_A
#endif

////////////////////////////////////////////////////////////////////////
///
/// Password Change Functions
///
////////////////////////////////////////////////////////////////////////

#if ISSP_MODE != 0

SECURITY_STATUS SEC_ENTRY
ChangeAccountPasswordW(
    __in    SEC_WCHAR *  pszPackageName,
    __in    SEC_WCHAR *  pszDomainName,
    __in    SEC_WCHAR *  pszAccountName,
    __in    SEC_WCHAR *  pszOldPassword,
    __in    SEC_WCHAR *  pszNewPassword,
    __in    BOOLEAN              bImpersonating,
    __in    unsigned long        dwReserved,
    __inout PSecBufferDesc       pOutput
    );

typedef SECURITY_STATUS
(SEC_ENTRY * CHANGE_PASSWORD_FN_W)(
    SEC_WCHAR *,
    SEC_WCHAR *,
    SEC_WCHAR *,
    SEC_WCHAR *,
    SEC_WCHAR *,
    BOOLEAN,
    unsigned long,
    PSecBufferDesc
    );



SECURITY_STATUS SEC_ENTRY
ChangeAccountPasswordA(
    __in    SEC_CHAR *  pszPackageName,
    __in    SEC_CHAR *  pszDomainName,
    __in    SEC_CHAR *  pszAccountName,
    __in    SEC_CHAR *  pszOldPassword,
    __in    SEC_CHAR *  pszNewPassword,
    __in    BOOLEAN             bImpersonating,
    __in    unsigned long       dwReserved,
    __inout PSecBufferDesc      pOutput
    );

typedef SECURITY_STATUS
(SEC_ENTRY * CHANGE_PASSWORD_FN_A)(
    SEC_CHAR *,
    SEC_CHAR *,
    SEC_CHAR *,
    SEC_CHAR *,
    SEC_CHAR *,
    BOOLEAN,
    unsigned long,
    PSecBufferDesc
    );

#ifdef UNICODE
#  define ChangeAccountPassword ChangeAccountPasswordW
#  define CHANGE_PASSWORD_FN CHANGE_PASSWORD_FN_W
#else
#  define ChangeAccountPassword ChangeAccountPasswordA
#  define CHANGE_PASSWORD_FN CHANGE_PASSWORD_FN_A
#endif // !UNICODE

#endif // ISSP_MODE


////////////////////////////////////////////////////////////////////////
///
/// Context Management Functions
///
////////////////////////////////////////////////////////////////////////

KSECDDDECLSPEC
SECURITY_STATUS SEC_ENTRY
InitializeSecurityContextW(
    __in_opt    PCredHandle phCredential,               // Cred to base context
    __in_opt    PCtxtHandle phContext,                  // Existing context (OPT)
#if ISSP_MODE == 0
    __in_opt PSECURITY_STRING pTargetName,
#else
    __in_opt SEC_WCHAR * pszTargetName,         // Name of target
#endif
    __in        unsigned long fContextReq,              // Context Requirements
    __in        unsigned long Reserved1,                // Reserved, MBZ
    __in        unsigned long TargetDataRep,            // Data rep of target
    __in_opt    PSecBufferDesc pInput,                  // Input Buffers
    __in        unsigned long Reserved2,                // Reserved, MBZ
    __inout_opt PCtxtHandle phNewContext,               // (out) New Context handle
    __inout_opt PSecBufferDesc pOutput,                 // (inout) Output Buffers
    __out       unsigned long * pfContextAttr,  // (out) Context attrs
    __out_opt   PTimeStamp ptsExpiry                    // (out) Life span (OPT)
    );

typedef SECURITY_STATUS
(SEC_ENTRY * INITIALIZE_SECURITY_CONTEXT_FN_W)(
    PCredHandle,
    PCtxtHandle,
#if ISSP_MODE == 0
    PSECURITY_STRING,
#else
    SEC_WCHAR *,
#endif
    unsigned long,
    unsigned long,
    unsigned long,
    PSecBufferDesc,
    unsigned long,
    PCtxtHandle,
    PSecBufferDesc,
    unsigned long *,
    PTimeStamp);

// end_ntifs

SECURITY_STATUS SEC_ENTRY
InitializeSecurityContextA(
    __in_opt    PCredHandle phCredential,               // Cred to base context
    __in_opt    PCtxtHandle phContext,                  // Existing context (OPT)
    __in_opt    SEC_CHAR * pszTargetName,       // Name of target
    __in        unsigned long fContextReq,              // Context Requirements
    __in        unsigned long Reserved1,                // Reserved, MBZ
    __in        unsigned long TargetDataRep,            // Data rep of target
    __in_opt    PSecBufferDesc pInput,                  // Input Buffers
    __in        unsigned long Reserved2,                // Reserved, MBZ
    __inout_opt PCtxtHandle phNewContext,               // (out) New Context handle
    __inout_opt PSecBufferDesc pOutput,                 // (inout) Output Buffers
    __out       unsigned long * pfContextAttr,  // (out) Context attrs
    __out_opt   PTimeStamp ptsExpiry                    // (out) Life span (OPT)
    );

typedef SECURITY_STATUS
(SEC_ENTRY * INITIALIZE_SECURITY_CONTEXT_FN_A)(
    PCredHandle,
    PCtxtHandle,
    SEC_CHAR *,
    unsigned long,
    unsigned long,
    unsigned long,
    PSecBufferDesc,
    unsigned long,
    PCtxtHandle,
    PSecBufferDesc,
    unsigned long *,
    PTimeStamp);

#ifdef UNICODE
#  define InitializeSecurityContext InitializeSecurityContextW              // ntifs
#  define INITIALIZE_SECURITY_CONTEXT_FN INITIALIZE_SECURITY_CONTEXT_FN_W   // ntifs
#else
#  define InitializeSecurityContext InitializeSecurityContextA
#  define INITIALIZE_SECURITY_CONTEXT_FN INITIALIZE_SECURITY_CONTEXT_FN_A
#endif // !UNICODE

// begin_ntifs

KSECDDDECLSPEC
SECURITY_STATUS SEC_ENTRY
AcceptSecurityContext(
    __in_opt  PCredHandle phCredential,               // Cred to base context
    __in_opt  PCtxtHandle phContext,                  // Existing context (OPT)
    __in_opt  PSecBufferDesc pInput,                  // Input buffer
    __in      unsigned long fContextReq,              // Context Requirements
    __in      unsigned long TargetDataRep,            // Target Data Rep
    __in_opt  PCtxtHandle phNewContext,               // (out) New context handle
    __in_opt  PSecBufferDesc pOutput,                 // (inout) Output buffers
    __out     unsigned long * pfContextAttr,  // (out) Context attributes
    __out_opt PTimeStamp ptsExpiry                    // (out) Life span (OPT)
    );

typedef SECURITY_STATUS
(SEC_ENTRY * ACCEPT_SECURITY_CONTEXT_FN)(
    PCredHandle,
    PCtxtHandle,
    PSecBufferDesc,
    unsigned long,
    unsigned long,
    PCtxtHandle,
    PSecBufferDesc,
    unsigned long *,
    PTimeStamp);



SECURITY_STATUS SEC_ENTRY
CompleteAuthToken(
    __in PCtxtHandle phContext,              // Context to complete
    __in PSecBufferDesc pToken               // Token to complete
    );

typedef SECURITY_STATUS
(SEC_ENTRY * COMPLETE_AUTH_TOKEN_FN)(
    PCtxtHandle,
    PSecBufferDesc);

KSECDDDECLSPEC
SECURITY_STATUS SEC_ENTRY
ImpersonateSecurityContext(
    __in PCtxtHandle phContext               // Context to impersonate
    );

typedef SECURITY_STATUS
(SEC_ENTRY * IMPERSONATE_SECURITY_CONTEXT_FN)(
    PCtxtHandle);


KSECDDDECLSPEC
SECURITY_STATUS SEC_ENTRY
RevertSecurityContext(
    __in PCtxtHandle phContext               // Context from which to re
    );

typedef SECURITY_STATUS
(SEC_ENTRY * REVERT_SECURITY_CONTEXT_FN)(
    PCtxtHandle);


KSECDDDECLSPEC
SECURITY_STATUS SEC_ENTRY
QuerySecurityContextToken(
    __in  PCtxtHandle phContext,
    __out void * * Token
    );

typedef SECURITY_STATUS
(SEC_ENTRY * QUERY_SECURITY_CONTEXT_TOKEN_FN)(
    PCtxtHandle, void * *);


KSECDDDECLSPEC
SECURITY_STATUS SEC_ENTRY
DeleteSecurityContext(
    __in PCtxtHandle phContext               // Context to delete
    );

typedef SECURITY_STATUS
(SEC_ENTRY * DELETE_SECURITY_CONTEXT_FN)(
    PCtxtHandle);


KSECDDDECLSPEC
SECURITY_STATUS SEC_ENTRY
ApplyControlToken(
    __in PCtxtHandle phContext,              // Context to modify
    __in PSecBufferDesc pInput               // Input token to apply
    );

typedef SECURITY_STATUS
(SEC_ENTRY * APPLY_CONTROL_TOKEN_FN)(
    PCtxtHandle, PSecBufferDesc);


KSECDDDECLSPEC
SECURITY_STATUS SEC_ENTRY
QueryContextAttributesW(
    __in  PCtxtHandle phContext,              // Context to query
    __in  unsigned long ulAttribute,          // Attribute to query
    __out void * pBuffer              // Buffer for attributes
    );

typedef SECURITY_STATUS
(SEC_ENTRY * QUERY_CONTEXT_ATTRIBUTES_FN_W)(
    PCtxtHandle,
    unsigned long,
    void *);

// end_ntifs

SECURITY_STATUS SEC_ENTRY
QueryContextAttributesA(
    __in  PCtxtHandle phContext,              // Context to query
    __in  unsigned long ulAttribute,          // Attribute to query
    __out void * pBuffer              // Buffer for attributes
    );

typedef SECURITY_STATUS
(SEC_ENTRY * QUERY_CONTEXT_ATTRIBUTES_FN_A)(
    PCtxtHandle,
    unsigned long,
    void *);

#ifdef UNICODE
#  define QueryContextAttributes QueryContextAttributesW            // ntifs
#  define QUERY_CONTEXT_ATTRIBUTES_FN QUERY_CONTEXT_ATTRIBUTES_FN_W // ntifs
#else
#  define QueryContextAttributes QueryContextAttributesA
#  define QUERY_CONTEXT_ATTRIBUTES_FN QUERY_CONTEXT_ATTRIBUTES_FN_A
#endif // !UNICODE

// begin_ntifs

#if 1 //(OSVER(NTDDI_VERSION) > NTDDI_WIN2K)

SECURITY_STATUS SEC_ENTRY
SetContextAttributesW(
    __in PCtxtHandle phContext,                   // Context to Set
    __in unsigned long ulAttribute,               // Attribute to Set
    __in_bcount(cbBuffer) void * pBuffer, // Buffer for attributes
    __in unsigned long cbBuffer                   // Size (in bytes) of Buffer
    );

typedef SECURITY_STATUS
(SEC_ENTRY * SET_CONTEXT_ATTRIBUTES_FN_W)(
    PCtxtHandle,
    unsigned long,
    void *,
    unsigned long );

#endif // Greater than w2k

// end_ntifs

SECURITY_STATUS SEC_ENTRY
SetContextAttributesA(
    __in PCtxtHandle phContext,                   // Context to Set
    __in unsigned long ulAttribute,               // Attribute to Set
    __in_bcount(cbBuffer) void * pBuffer, // Buffer for attributes
    __in unsigned long cbBuffer                   // Size (in bytes) of Buffer
    );

typedef SECURITY_STATUS
(SEC_ENTRY * SET_CONTEXT_ATTRIBUTES_FN_A)(
    PCtxtHandle,
    unsigned long,
    void *,
    unsigned long );

#ifdef UNICODE
#  define SetContextAttributes SetContextAttributesW            // ntifs
#  define SET_CONTEXT_ATTRIBUTES_FN SET_CONTEXT_ATTRIBUTES_FN_W // ntifs
#else
#  define SetContextAttributes SetContextAttributesA
#  define SET_CONTEXT_ATTRIBUTES_FN SET_CONTEXT_ATTRIBUTES_FN_A
#endif // !UNICODE

// begin_ntifs

KSECDDDECLSPEC
SECURITY_STATUS SEC_ENTRY
QueryCredentialsAttributesW(
    __in    PCredHandle phCredential,           // Credential to query
    __in    unsigned long ulAttribute,          // Attribute to query
    __inout void * pBuffer              // Buffer for attributes
    );

typedef SECURITY_STATUS
(SEC_ENTRY * QUERY_CREDENTIALS_ATTRIBUTES_FN_W)(
    PCredHandle,
    unsigned long,
    void *);

// end_ntifs

SECURITY_STATUS SEC_ENTRY
QueryCredentialsAttributesA(
    __in    PCredHandle phCredential,           // Credential to query
    __in    unsigned long ulAttribute,          // Attribute to query
    __inout void * pBuffer              // Buffer for attributes
    );

typedef SECURITY_STATUS
(SEC_ENTRY * QUERY_CREDENTIALS_ATTRIBUTES_FN_A)(
    PCredHandle,
    unsigned long,
    void *);

#ifdef UNICODE
#  define QueryCredentialsAttributes QueryCredentialsAttributesW            // ntifs
#  define QUERY_CREDENTIALS_ATTRIBUTES_FN QUERY_CREDENTIALS_ATTRIBUTES_FN_W // ntifs
#else
#  define QueryCredentialsAttributes QueryCredentialsAttributesA
#  define QUERY_CREDENTIALS_ATTRIBUTES_FN QUERY_CREDENTIALS_ATTRIBUTES_FN_A
#endif // !UNICODE

// begin_ntifs

#if NTDDI_VERSION > NTDDI_WS03

KSECDDDECLSPEC
SECURITY_STATUS SEC_ENTRY
SetCredentialsAttributesW(
    __in PCredHandle phCredential,                // Credential to Set
    __in unsigned long ulAttribute,               // Attribute to Set
    __in_bcount(cbBuffer) void * pBuffer, // Buffer for attributes
    __in unsigned long cbBuffer                   // Size (in bytes) of Buffer
    );

typedef SECURITY_STATUS
(SEC_ENTRY * SET_CREDENTIALS_ATTRIBUTES_FN_W)(
    PCredHandle,
    unsigned long,
    void *,
    unsigned long );

#endif // For W2k3SP1 and greater

// end_ntifs

SECURITY_STATUS SEC_ENTRY
SetCredentialsAttributesA(
    __in PCredHandle phCredential,                // Credential to Set
    __in unsigned long ulAttribute,               // Attribute to Set
    __in_bcount(cbBuffer) void * pBuffer, // Buffer for attributes
    __in unsigned long cbBuffer                   // Size (in bytes) of Buffer
    );

typedef SECURITY_STATUS
(SEC_ENTRY * SET_CREDENTIALS_ATTRIBUTES_FN_A)(
    PCredHandle,
    unsigned long,
    void *,
    unsigned long );

#ifdef UNICODE
#  define SetCredentialsAttributes SetCredentialsAttributesW            // ntifs
#  define SET_CREDENTIALS_ATTRIBUTES_FN SET_CREDENTIALS_ATTRIBUTES_FN_W // ntifs
#else
#  define SetCredentialsAttributes SetCredentialsAttributesA
#  define SET_CREDENTIALS_ATTRIBUTES_FN SET_CREDENTIALS_ATTRIBUTES_FN_A
#endif // !UNICODE

// begin_ntifs

SECURITY_STATUS SEC_ENTRY
FreeContextBuffer(
    __inout PVOID pvContextBuffer      // buffer to free
    );

typedef SECURITY_STATUS
(SEC_ENTRY * FREE_CONTEXT_BUFFER_FN)(
    __inout PVOID
    );

// end_ntifs

// begin_ntifs
///////////////////////////////////////////////////////////////////
////
////    Message Support API
////
//////////////////////////////////////////////////////////////////

KSECDDDECLSPEC
SECURITY_STATUS SEC_ENTRY
MakeSignature(
    __in PCtxtHandle phContext,              // Context to use
    __in unsigned long fQOP,                 // Quality of Protection
    __in PSecBufferDesc pMessage,            // Message to sign
    __in unsigned long MessageSeqNo          // Message Sequence Num.
    );

typedef SECURITY_STATUS
(SEC_ENTRY * MAKE_SIGNATURE_FN)(
    PCtxtHandle,
    unsigned long,
    PSecBufferDesc,
    unsigned long);


KSECDDDECLSPEC
SECURITY_STATUS SEC_ENTRY
VerifySignature(
    __in  PCtxtHandle phContext,              // Context to use
    __in  PSecBufferDesc pMessage,            // Message to verify
    __in  unsigned long MessageSeqNo,         // Sequence Num.
    __out unsigned long * pfQOP       // QOP used
    );

typedef SECURITY_STATUS
(SEC_ENTRY * VERIFY_SIGNATURE_FN)(
    PCtxtHandle,
    PSecBufferDesc,
    unsigned long,
    unsigned long *);

// This only exists win Win2k3 and Greater
#define SECQOP_WRAP_NO_ENCRYPT      0x80000001
#define SECQOP_WRAP_OOB_DATA        0x40000000

SECURITY_STATUS SEC_ENTRY
EncryptMessage( __in    PCtxtHandle         phContext,
                __in    unsigned long       fQOP,
                __inout PSecBufferDesc      pMessage,
                __in    unsigned long       MessageSeqNo);

typedef SECURITY_STATUS
(SEC_ENTRY * ENCRYPT_MESSAGE_FN)(
    PCtxtHandle, unsigned long, PSecBufferDesc, unsigned long);


SECURITY_STATUS SEC_ENTRY
DecryptMessage( __in      PCtxtHandle         phContext,
                __inout   PSecBufferDesc      pMessage,
                __in      unsigned long       MessageSeqNo,
                __out_opt unsigned long *     pfQOP);


typedef SECURITY_STATUS
(SEC_ENTRY * DECRYPT_MESSAGE_FN)(
    PCtxtHandle, PSecBufferDesc, unsigned long,
    unsigned long *);


// end_ntifs

// begin_ntifs
///////////////////////////////////////////////////////////////////////////
////
////    Misc.
////
///////////////////////////////////////////////////////////////////////////

KSECDDDECLSPEC
SECURITY_STATUS SEC_ENTRY
EnumerateSecurityPackagesW(
    __out       unsigned long * pcPackages,     // Receives num. packages
    __deref_out PSecPkgInfoW  * ppPackageInfo    // Receives array of info
    );

typedef SECURITY_STATUS
(SEC_ENTRY * ENUMERATE_SECURITY_PACKAGES_FN_W)(
    unsigned long *,
    PSecPkgInfoW *);

// end_ntifs

SECURITY_STATUS SEC_ENTRY
EnumerateSecurityPackagesA(
    __out       unsigned long * pcPackages,     // Receives num. packages
    __deref_out PSecPkgInfoA  * ppPackageInfo    // Receives array of info
    );

typedef SECURITY_STATUS
(SEC_ENTRY * ENUMERATE_SECURITY_PACKAGES_FN_A)(
    unsigned long *,
    PSecPkgInfoA *);

#ifdef UNICODE
#  define EnumerateSecurityPackages EnumerateSecurityPackagesW              // ntifs
#  define ENUMERATE_SECURITY_PACKAGES_FN ENUMERATE_SECURITY_PACKAGES_FN_W   // ntifs
#else
#  define EnumerateSecurityPackages EnumerateSecurityPackagesA
#  define ENUMERATE_SECURITY_PACKAGES_FN ENUMERATE_SECURITY_PACKAGES_FN_A
#endif // !UNICODE

// begin_ntifs

KSECDDDECLSPEC
SECURITY_STATUS SEC_ENTRY
QuerySecurityPackageInfoW(
#if ISSP_MODE == 0
    __in        PSECURITY_STRING pPackageName,
#else
    __in        LPWSTR pszPackageName,          // Name of package
#endif
    __deref_out PSecPkgInfoW *ppPackageInfo     // Receives package info
    );

typedef SECURITY_STATUS
(SEC_ENTRY * QUERY_SECURITY_PACKAGE_INFO_FN_W)(
#if ISSP_MODE == 0
    PSECURITY_STRING,
#else
    SEC_WCHAR *,
#endif
    PSecPkgInfoW *);

// end_ntifs

SECURITY_STATUS SEC_ENTRY
QuerySecurityPackageInfoA(
    __in        LPSTR pszPackageName,           // Name of package
    __deref_out PSecPkgInfoA *ppPackageInfo     // Receives package info
    );

typedef SECURITY_STATUS
(SEC_ENTRY * QUERY_SECURITY_PACKAGE_INFO_FN_A)(
    SEC_CHAR *,
    PSecPkgInfoA *);

#ifdef UNICODE
#  define QuerySecurityPackageInfo QuerySecurityPackageInfoW                // ntifs
#  define QUERY_SECURITY_PACKAGE_INFO_FN QUERY_SECURITY_PACKAGE_INFO_FN_W   // ntifs
#else
#  define QuerySecurityPackageInfo QuerySecurityPackageInfoA
#  define QUERY_SECURITY_PACKAGE_INFO_FN QUERY_SECURITY_PACKAGE_INFO_FN_A
#endif // !UNICODE


typedef enum _SecDelegationType {
    SecFull,
    SecService,
    SecTree,
    SecDirectory,
    SecObject
} SecDelegationType, * PSecDelegationType;

SECURITY_STATUS SEC_ENTRY
DelegateSecurityContext(
    PCtxtHandle         phContext,          // IN Active context to delegate
#if ISSP_MODE == 0
    PSECURITY_STRING    pTarget,            // IN Target path
#else
    __in LPSTR          pszTarget,
#endif
    SecDelegationType   DelegationType,     // IN Type of delegation
    PTimeStamp          pExpiry,            // IN OPTIONAL time limit
    PSecBuffer          pPackageParameters, // IN OPTIONAL package specific
    PSecBufferDesc      pOutput);           // OUT Token for applycontroltoken.


///////////////////////////////////////////////////////////////////////////
////
////    Proxies
////
///////////////////////////////////////////////////////////////////////////


//
// Proxies are only available on NT platforms
//

// begin_ntifs

///////////////////////////////////////////////////////////////////////////
////
////    Context export/import
////
///////////////////////////////////////////////////////////////////////////


KSECDDDECLSPEC
SECURITY_STATUS SEC_ENTRY
ExportSecurityContext(
    __in  PCtxtHandle          phContext,             // (in) context to export
    __in  ULONG                fFlags,                // (in) option flags
    __out PSecBuffer           pPackedContext,        // (out) marshalled context
    __out void * * pToken             // (out, optional) token handle for impersonation
    );

typedef SECURITY_STATUS
(SEC_ENTRY * EXPORT_SECURITY_CONTEXT_FN)(
    PCtxtHandle,
    ULONG,
    PSecBuffer,
    void * *
    );

KSECDDDECLSPEC
SECURITY_STATUS SEC_ENTRY
ImportSecurityContextW(
#if ISSP_MODE == 0
    __in  PSECURITY_STRING     pszPackage,
#else
    __in  LPWSTR               pszPackage,
#endif
    __in  PSecBuffer           pPackedContext,        // (in) marshalled context
    __in  void *               Token,                 // (in, optional) handle to token for context
    __out PCtxtHandle          phContext              // (out) new context handle
    );

typedef SECURITY_STATUS
(SEC_ENTRY * IMPORT_SECURITY_CONTEXT_FN_W)(
#if ISSP_MODE == 0
    PSECURITY_STRING,
#else
    SEC_WCHAR *,
#endif
    PSecBuffer,
    VOID *,
    PCtxtHandle
    );

// end_ntifs
SECURITY_STATUS SEC_ENTRY
ImportSecurityContextA(
    __in  LPSTR                pszPackage,
    __in  PSecBuffer           pPackedContext,        // (in) marshalled context
    __in  VOID *               Token,                 // (in, optional) handle to token for context
    __out PCtxtHandle          phContext              // (out) new context handle
    );

typedef SECURITY_STATUS
(SEC_ENTRY * IMPORT_SECURITY_CONTEXT_FN_A)(
    SEC_CHAR *,
    PSecBuffer,
    void *,
    PCtxtHandle
    );

#ifdef UNICODE
#  define ImportSecurityContext ImportSecurityContextW              // ntifs
#  define IMPORT_SECURITY_CONTEXT_FN IMPORT_SECURITY_CONTEXT_FN_W   // ntifs
#else
#  define ImportSecurityContext ImportSecurityContextA
#  define IMPORT_SECURITY_CONTEXT_FN IMPORT_SECURITY_CONTEXT_FN_A
#endif // !UNICODE

// begin_ntifs

#if ISSP_MODE == 0
KSECDDDECLSPEC
NTSTATUS
NTAPI
SecMakeSPN(
    IN PUNICODE_STRING ServiceClass,
    IN PUNICODE_STRING ServiceName,
    IN PUNICODE_STRING InstanceName OPTIONAL,
    IN USHORT InstancePort OPTIONAL,
    IN PUNICODE_STRING Referrer OPTIONAL,
    IN OUT PUNICODE_STRING Spn,
    OUT PULONG Length OPTIONAL,
    IN BOOLEAN Allocate
    );

#if OSVER(NTDDI_VERSION) > NTDD_WIN2K

KSECDDDECLSPEC
NTSTATUS
NTAPI
SecMakeSPNEx(
    IN PUNICODE_STRING ServiceClass,
    IN PUNICODE_STRING ServiceName,
    IN PUNICODE_STRING InstanceName OPTIONAL,
    IN USHORT InstancePort OPTIONAL,
    IN PUNICODE_STRING Referrer OPTIONAL,
    IN PUNICODE_STRING TargetInfo OPTIONAL,
    IN OUT PUNICODE_STRING Spn,
    OUT PULONG Length OPTIONAL,
    IN BOOLEAN Allocate
    );

#if OSVER(NTDDI_VERSION) > NTDDI_WS03

KSECDDDECLSPEC
NTSTATUS
NTAPI
SecMakeSPNEx2(
    IN PUNICODE_STRING ServiceClass,
    IN PUNICODE_STRING ServiceName,
    IN PUNICODE_STRING InstanceName OPTIONAL,
    IN USHORT InstancePort OPTIONAL,
    IN PUNICODE_STRING Referrer OPTIONAL,
    IN PUNICODE_STRING InTargetInfo OPTIONAL,
    IN OUT PUNICODE_STRING Spn,
    OUT PULONG TotalSize OPTIONAL,
    IN BOOLEAN Allocate,
    IN BOOLEAN IsTargetInfoMarshaled
    );

#endif // Windows Vista and greater

KSECDDDECLSPEC
NTSTATUS
SEC_ENTRY
SecLookupAccountSid(
    __in      PSID Sid,
    __out     PULONG NameSize,
    __inout   PUNICODE_STRING NameBuffer,
    __out     PULONG DomainSize OPTIONAL,
    __out_opt PUNICODE_STRING DomainBuffer OPTIONAL,
    __out     PSID_NAME_USE NameUse
    );

KSECDDDECLSPEC
NTSTATUS
SEC_ENTRY
SecLookupAccountName(
    __in        PUNICODE_STRING Name,
    __inout     PULONG SidSize,
    __out       PSID Sid,
    __out       PSID_NAME_USE NameUse,
    __out       PULONG DomainSize OPTIONAL,
    __inout_opt PUNICODE_STRING ReferencedDomain OPTIONAL
    );

#endif // Greater than W2k

#if OSVER(NTDDI_VERSION) > NTDDI_WINXP

KSECDDDECLSPEC
NTSTATUS
SEC_ENTRY
SecLookupWellKnownSid(
    __in        WELL_KNOWN_SID_TYPE SidType,
    __out       PSID Sid,
    __in        ULONG SidBufferSize,
    __inout_opt PULONG SidSize OPTIONAL
    );

#endif // Greater than XP


#endif

// end_ntifs

///////////////////////////////////////////////////////////////////////////////
////
////  Fast access for RPC:
////
///////////////////////////////////////////////////////////////////////////////

#define SECURITY_ENTRYPOINT_ANSIW "InitSecurityInterfaceW"
#define SECURITY_ENTRYPOINT_ANSIA "InitSecurityInterfaceA"
#define SECURITY_ENTRYPOINTW SEC_TEXT("InitSecurityInterfaceW")     // ntifs
#define SECURITY_ENTRYPOINTA SEC_TEXT("InitSecurityInterfaceA")
#define SECURITY_ENTRYPOINT16 "INITSECURITYINTERFACEA"

#ifdef SECURITY_WIN32
#  ifdef UNICODE
#    define SECURITY_ENTRYPOINT SECURITY_ENTRYPOINTW                // ntifs
#    define SECURITY_ENTRYPOINT_ANSI SECURITY_ENTRYPOINT_ANSIW
#  else // UNICODE
#    define SECURITY_ENTRYPOINT SECURITY_ENTRYPOINTA
#    define SECURITY_ENTRYPOINT_ANSI SECURITY_ENTRYPOINT_ANSIA
#  endif // UNICODE
#else // SECURITY_WIN32
#  define SECURITY_ENTRYPOINT SECURITY_ENTRYPOINT16
#  define SECURITY_ENTRYPOINT_ANSI SECURITY_ENTRYPOINT16
#endif // SECURITY_WIN32

// begin_ntifs

#define FreeCredentialHandle FreeCredentialsHandle

typedef struct _SECURITY_FUNCTION_TABLE_W {
    unsigned long                       dwVersion;
    ENUMERATE_SECURITY_PACKAGES_FN_W    EnumerateSecurityPackagesW;
    QUERY_CREDENTIALS_ATTRIBUTES_FN_W   QueryCredentialsAttributesW;
    ACQUIRE_CREDENTIALS_HANDLE_FN_W     AcquireCredentialsHandleW;
    FREE_CREDENTIALS_HANDLE_FN          FreeCredentialsHandle;
    void *                      Reserved2;
    INITIALIZE_SECURITY_CONTEXT_FN_W    InitializeSecurityContextW;
    ACCEPT_SECURITY_CONTEXT_FN          AcceptSecurityContext;
    COMPLETE_AUTH_TOKEN_FN              CompleteAuthToken;
    DELETE_SECURITY_CONTEXT_FN          DeleteSecurityContext;
    APPLY_CONTROL_TOKEN_FN              ApplyControlToken;
    QUERY_CONTEXT_ATTRIBUTES_FN_W       QueryContextAttributesW;
    IMPERSONATE_SECURITY_CONTEXT_FN     ImpersonateSecurityContext;
    REVERT_SECURITY_CONTEXT_FN          RevertSecurityContext;
    MAKE_SIGNATURE_FN                   MakeSignature;
    VERIFY_SIGNATURE_FN                 VerifySignature;
    FREE_CONTEXT_BUFFER_FN              FreeContextBuffer;
    QUERY_SECURITY_PACKAGE_INFO_FN_W    QuerySecurityPackageInfoW;
    void *                      Reserved3;
    void *                      Reserved4;
    EXPORT_SECURITY_CONTEXT_FN          ExportSecurityContext;
    IMPORT_SECURITY_CONTEXT_FN_W        ImportSecurityContextW;
    ADD_CREDENTIALS_FN_W                AddCredentialsW ;
    void *                      Reserved8;
    QUERY_SECURITY_CONTEXT_TOKEN_FN     QuerySecurityContextToken;
    ENCRYPT_MESSAGE_FN                  EncryptMessage;
    DECRYPT_MESSAGE_FN                  DecryptMessage;
#if 1 // OSVER(NTDDI_VERSION) > NTDDI_WIN2K
    // Fields below this are available in OSes after w2k
    SET_CONTEXT_ATTRIBUTES_FN_W         SetContextAttributesW;
#endif // greater thean 2K

#if NTDDI_VERSION > NTDDI_WS03SP1
    // Fields below this are available in OSes after W2k3SP1
    SET_CREDENTIALS_ATTRIBUTES_FN_W     SetCredentialsAttributesW;
#endif
#if ISSP_MODE != 0
    CHANGE_PASSWORD_FN_W                ChangeAccountPasswordW;
#else
    void *                      Reserved9;
#endif
} SecurityFunctionTableW, * PSecurityFunctionTableW;

// end_ntifs

typedef struct _SECURITY_FUNCTION_TABLE_A {
    unsigned long                       dwVersion;
    ENUMERATE_SECURITY_PACKAGES_FN_A    EnumerateSecurityPackagesA;
    QUERY_CREDENTIALS_ATTRIBUTES_FN_A   QueryCredentialsAttributesA;
    ACQUIRE_CREDENTIALS_HANDLE_FN_A     AcquireCredentialsHandleA;
    FREE_CREDENTIALS_HANDLE_FN          FreeCredentialHandle;
    void *                      Reserved2;
    INITIALIZE_SECURITY_CONTEXT_FN_A    InitializeSecurityContextA;
    ACCEPT_SECURITY_CONTEXT_FN          AcceptSecurityContext;
    COMPLETE_AUTH_TOKEN_FN              CompleteAuthToken;
    DELETE_SECURITY_CONTEXT_FN          DeleteSecurityContext;
    APPLY_CONTROL_TOKEN_FN              ApplyControlToken;
    QUERY_CONTEXT_ATTRIBUTES_FN_A       QueryContextAttributesA;
    IMPERSONATE_SECURITY_CONTEXT_FN     ImpersonateSecurityContext;
    REVERT_SECURITY_CONTEXT_FN          RevertSecurityContext;
    MAKE_SIGNATURE_FN                   MakeSignature;
    VERIFY_SIGNATURE_FN                 VerifySignature;
    FREE_CONTEXT_BUFFER_FN              FreeContextBuffer;
    QUERY_SECURITY_PACKAGE_INFO_FN_A    QuerySecurityPackageInfoA;
    void *                      Reserved3;
    void *                      Reserved4;
    EXPORT_SECURITY_CONTEXT_FN          ExportSecurityContext;
    IMPORT_SECURITY_CONTEXT_FN_A        ImportSecurityContextA;
    ADD_CREDENTIALS_FN_A                AddCredentialsA ;
    void *                      Reserved8;
    QUERY_SECURITY_CONTEXT_TOKEN_FN     QuerySecurityContextToken;
    ENCRYPT_MESSAGE_FN                  EncryptMessage;
    DECRYPT_MESSAGE_FN                  DecryptMessage;
    SET_CONTEXT_ATTRIBUTES_FN_A         SetContextAttributesA;
    SET_CREDENTIALS_ATTRIBUTES_FN_A     SetCredentialsAttributesA;
#if ISSP_MODE != 0
    CHANGE_PASSWORD_FN_A                ChangeAccountPasswordA;
#else
    void *                      Reserved9;
#endif
} SecurityFunctionTableA, * PSecurityFunctionTableA;

#ifdef UNICODE
#  define SecurityFunctionTable SecurityFunctionTableW      // ntifs
#  define PSecurityFunctionTable PSecurityFunctionTableW    // ntifs
#else
#  define SecurityFunctionTable SecurityFunctionTableA
#  define PSecurityFunctionTable PSecurityFunctionTableA
#endif // !UNICODE

#define SECURITY_

// Function table has all routines through DecryptMessage
#define SECURITY_SUPPORT_PROVIDER_INTERFACE_VERSION     1   // ntifs

// Function table has all routines through SetContextAttributes
#define SECURITY_SUPPORT_PROVIDER_INTERFACE_VERSION_2   2   // ntifs

// Function table has all routines through SetCredentialsAttributes
#define SECURITY_SUPPORT_PROVIDER_INTERFACE_VERSION_3   3   // ntifs

// Function table has all routines through ChangeAccountPassword
#define SECURITY_SUPPORT_PROVIDER_INTERFACE_VERSION_4   4   // ntifs

PSecurityFunctionTableA SEC_ENTRY
InitSecurityInterfaceA(
    void
    );

typedef PSecurityFunctionTableA
(SEC_ENTRY * INIT_SECURITY_INTERFACE_A)(void);

// begin_ntifs

KSECDDDECLSPEC
PSecurityFunctionTableW SEC_ENTRY
InitSecurityInterfaceW(
    void
    );

typedef PSecurityFunctionTableW
(SEC_ENTRY * INIT_SECURITY_INTERFACE_W)(void);

// end_ntifs

#ifdef UNICODE
#  define InitSecurityInterface InitSecurityInterfaceW          // ntifs
#  define INIT_SECURITY_INTERFACE INIT_SECURITY_INTERFACE_W     // ntifs
#else
#  define InitSecurityInterface InitSecurityInterfaceA
#  define INIT_SECURITY_INTERFACE INIT_SECURITY_INTERFACE_A
#endif // !UNICODE


#ifdef SECURITY_WIN32

//
// SASL Profile Support
//


SECURITY_STATUS
SEC_ENTRY
SaslEnumerateProfilesA(
    __deref_out LPSTR * ProfileList,
    __out       ULONG * ProfileCount
    );

SECURITY_STATUS
SEC_ENTRY
SaslEnumerateProfilesW(
    __deref_out LPWSTR * ProfileList,
    __out       ULONG * ProfileCount
    );

#ifdef UNICODE
#define SaslEnumerateProfiles   SaslEnumerateProfilesW
#else
#define SaslEnumerateProfiles   SaslEnumerateProfilesA
#endif


SECURITY_STATUS
SEC_ENTRY
SaslGetProfilePackageA(
    __in        LPSTR ProfileName,
    __deref_out PSecPkgInfoA * PackageInfo
    );


SECURITY_STATUS
SEC_ENTRY
SaslGetProfilePackageW(
    __in        LPWSTR ProfileName,
    __deref_out PSecPkgInfoW * PackageInfo
    );

#ifdef UNICODE
#define SaslGetProfilePackage   SaslGetProfilePackageW
#else
#define SaslGetProfilePackage   SaslGetProfilePackageA
#endif

SECURITY_STATUS
SEC_ENTRY
SaslIdentifyPackageA(
    __in        PSecBufferDesc pInput,
    __deref_out PSecPkgInfoA * PackageInfo
    );

SECURITY_STATUS
SEC_ENTRY
SaslIdentifyPackageW(
    __in        PSecBufferDesc pInput,
    __deref_out PSecPkgInfoW * PackageInfo
    );

#ifdef UNICODE
#define SaslIdentifyPackage SaslIdentifyPackageW
#else
#define SaslIdentifyPackage SaslIdentifyPackageA
#endif

SECURITY_STATUS
SEC_ENTRY
SaslInitializeSecurityContextW(
    __in_opt    PCredHandle                 phCredential,       // Cred to base context
    __in_opt    PCtxtHandle                 phContext,          // Existing context (OPT)
    __in_opt    LPWSTR                      pszTargetName,      // Name of target
    __in        unsigned long               fContextReq,        // Context Requirements
    __in        unsigned long               Reserved1,          // Reserved, MBZ
    __in        unsigned long               TargetDataRep,      // Data rep of target
    __in_opt    PSecBufferDesc              pInput,             // Input Buffers
    __in        unsigned long               Reserved2,          // Reserved, MBZ
    __inout_opt PCtxtHandle                 phNewContext,       // (out) New Context handle
    __inout_opt PSecBufferDesc              pOutput,            // (inout) Output Buffers
    __out       unsigned long *     pfContextAttr,      // (out) Context attrs
    __out_opt   PTimeStamp                  ptsExpiry           // (out) Life span (OPT)
    );

SECURITY_STATUS
SEC_ENTRY
SaslInitializeSecurityContextA(
    __in_opt    PCredHandle                 phCredential,       // Cred to base context
    __in_opt    PCtxtHandle                 phContext,          // Existing context (OPT)
    __in_opt    LPSTR                       pszTargetName,      // Name of target
    __in        unsigned long               fContextReq,        // Context Requirements
    __in        unsigned long               Reserved1,          // Reserved, MBZ
    __in        unsigned long               TargetDataRep,      // Data rep of target
    __in_opt    PSecBufferDesc              pInput,             // Input Buffers
    __in        unsigned long               Reserved2,          // Reserved, MBZ
    __inout_opt PCtxtHandle                 phNewContext,       // (out) New Context handle
    __inout_opt PSecBufferDesc              pOutput,            // (inout) Output Buffers
    __out       unsigned long *     pfContextAttr,      // (out) Context attrs
    __out_opt   PTimeStamp                  ptsExpiry           // (out) Life span (OPT)
    );

#ifdef UNICODE
#define SaslInitializeSecurityContext   SaslInitializeSecurityContextW
#else
#define SaslInitializeSecurityContext   SaslInitializeSecurityContextA
#endif


SECURITY_STATUS
SEC_ENTRY
SaslAcceptSecurityContext(
    __in_opt    PCredHandle                 phCredential,       // Cred to base context
    __in_opt    PCtxtHandle                 phContext,          // Existing context (OPT)
    __in_opt    PSecBufferDesc              pInput,             // Input buffer
    __in        unsigned long               fContextReq,        // Context Requirements
    __in        unsigned long               TargetDataRep,      // Target Data Rep
    __inout_opt PCtxtHandle                 phNewContext,       // (out) New context handle
    __inout_opt PSecBufferDesc              pOutput,            // (inout) Output buffers
    __out       unsigned long *     pfContextAttr,      // (out) Context attributes
    __out_opt   PTimeStamp                  ptsExpiry           // (out) Life span (OPT)
    );


#define SASL_OPTION_SEND_SIZE       1       // Maximum size to send to peer
#define SASL_OPTION_RECV_SIZE       2       // Maximum size willing to receive
#define SASL_OPTION_AUTHZ_STRING    3       // Authorization string
#define SASL_OPTION_AUTHZ_PROCESSING    4       // Authorization string processing

typedef enum _SASL_AUTHZID_STATE {
    Sasl_AuthZIDForbidden,             // allow no AuthZID strings to be specified - error out (default)
    Sasl_AuthZIDProcessed             // AuthZID Strings processed by Application or SSP
} SASL_AUTHZID_STATE ;

SECURITY_STATUS
SEC_ENTRY
SaslSetContextOption(
    __in PCtxtHandle ContextHandle,
    __in ULONG Option,
    __in PVOID Value,
    __in ULONG Size
    );


SECURITY_STATUS
SEC_ENTRY
SaslGetContextOption(
    __in      PCtxtHandle ContextHandle,
    __in      ULONG Option,
    __out     PVOID Value,
    __in      ULONG Size,
    __out_opt PULONG Needed OPTIONAL
    );

#endif

#ifdef SECURITY_DOS
#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4147)
#endif
#endif

//
// This is the legacy credentials structure.
// The EX version below is preferred.

// begin_ntifs

#ifndef _AUTH_IDENTITY_EX2_DEFINED
#define _AUTH_IDENTITY_EX2_DEFINED

#define SEC_WINNT_AUTH_IDENTITY_VERSION_2 0x201

typedef struct _SEC_WINNT_AUTH_IDENTITY_EX2 {
   unsigned long Version; // contains SEC_WINNT_AUTH_IDENTITY_VERSION_2
   unsigned short cbHeaderLength;
   unsigned long cbStructureLength;
   unsigned long UserOffset;                // Non-NULL terminated string, unicode only
   unsigned short UserLength;               // # of bytes (NOT WCHARs), not including NULL.
   unsigned long DomainOffset;              // Non-NULL terminated string, unicode only
   unsigned short DomainLength;             // # of bytes (NOT WCHARs), not including NULL.
   unsigned long PackedCredentialsOffset;   // Non-NULL terminated string, unicode only
   unsigned short PackedCredentialsLength;  // # of bytes (NOT WCHARs), not including NULL.
   unsigned long Flags;
   unsigned long PackageListOffset;         // Non-NULL terminated string, unicode only
   unsigned short PackageListLength;
} SEC_WINNT_AUTH_IDENTITY_EX2, *PSEC_WINNT_AUTH_IDENTITY_EX2;

#endif // _AUTH_IDENTITY_EX2_DEFINED

#ifndef _AUTH_IDENTITY_DEFINED
#define _AUTH_IDENTITY_DEFINED

//
// This was not defined in NTIFS.h for windows 2000 however
// this struct has always been there and are safe to use
// in windows 2000 and above.
//

#define SEC_WINNT_AUTH_IDENTITY_ANSI    0x1
#define SEC_WINNT_AUTH_IDENTITY_UNICODE 0x2

typedef struct _SEC_WINNT_AUTH_IDENTITY_W {
  unsigned short *User;         //  Non-NULL terminated string.
  unsigned long UserLength;     //  # of characters (NOT bytes), not including NULL.
  unsigned short *Domain;       //  Non-NULL terminated string.
  unsigned long DomainLength;   //  # of characters (NOT bytes), not including NULL.
  unsigned short *Password;     //  Non-NULL terminated string.
  unsigned long PasswordLength; //  # of characters (NOT bytes), not including NULL.
  unsigned long Flags;
} SEC_WINNT_AUTH_IDENTITY_W, *PSEC_WINNT_AUTH_IDENTITY_W;

// end_ntifs

#define _AUTH_IDENTITY_A_DEFINED

typedef struct _SEC_WINNT_AUTH_IDENTITY_A {
  unsigned char *User;          //  Non-NULL terminated string.
  unsigned long UserLength;     //  # of characters (NOT bytes), not including NULL.
  unsigned char *Domain;        //  Non-NULL terminated string.
  unsigned long DomainLength;   //  # of characters (NOT bytes), not including NULL.
  unsigned char *Password;      //  Non-NULL terminated string.
  unsigned long PasswordLength; //  # of characters (NOT bytes), not including NULL.
  unsigned long Flags;
} SEC_WINNT_AUTH_IDENTITY_A, *PSEC_WINNT_AUTH_IDENTITY_A;


#ifdef UNICODE
#define SEC_WINNT_AUTH_IDENTITY SEC_WINNT_AUTH_IDENTITY_W       // ntifs
#define PSEC_WINNT_AUTH_IDENTITY PSEC_WINNT_AUTH_IDENTITY_W     // ntifs
#define _SEC_WINNT_AUTH_IDENTITY _SEC_WINNT_AUTH_IDENTITY_W     // ntifs
#else // UNICODE
#define SEC_WINNT_AUTH_IDENTITY SEC_WINNT_AUTH_IDENTITY_A
#define PSEC_WINNT_AUTH_IDENTITY PSEC_WINNT_AUTH_IDENTITY_A
#define _SEC_WINNT_AUTH_IDENTITY _SEC_WINNT_AUTH_IDENTITY_A
#endif // UNICODE

// begin_ntifs

#endif //_AUTH_IDENTITY_DEFINED                                 // ntifs

//
// This is the combined authentication identity structure that may be
// used with the negotiate package, NTLM, Kerberos, or SCHANNEL
//

#ifndef SEC_WINNT_AUTH_IDENTITY_VERSION
#define SEC_WINNT_AUTH_IDENTITY_VERSION 0x200

typedef struct _SEC_WINNT_AUTH_IDENTITY_EXW {
    unsigned long Version;
    unsigned long Length;
    unsigned short *User;           //  Non-NULL terminated string.
    unsigned long UserLength;       //  # of characters (NOT bytes), not including NULL.
    unsigned short *Domain;         //  Non-NULL terminated string.
    unsigned long DomainLength;     //  # of characters (NOT bytes), not including NULL.
    unsigned short *Password;       //  Non-NULL terminated string.
    unsigned long PasswordLength;   //  # of characters (NOT bytes), not including NULL.
    unsigned long Flags;
    unsigned short *PackageList;
    unsigned long PackageListLength;
} SEC_WINNT_AUTH_IDENTITY_EXW, *PSEC_WINNT_AUTH_IDENTITY_EXW;

// end_ntifs

typedef struct _SEC_WINNT_AUTH_IDENTITY_EXA {
    unsigned long Version;
    unsigned long Length;
    unsigned char *User;            //  Non-NULL terminated string.
    unsigned long UserLength;       //  # of characters (NOT bytes), not including NULL.
    unsigned char *Domain;          //  Non-NULL terminated string.
    unsigned long DomainLength;     //  # of characters (NOT bytes), not including NULL.
    unsigned char *Password;        //  Non-NULL terminated string.
    unsigned long PasswordLength;   //  # of characters (NOT bytes), not including NULL.
    unsigned long Flags;
    unsigned char * PackageList;
    unsigned long PackageListLength;
} SEC_WINNT_AUTH_IDENTITY_EXA, *PSEC_WINNT_AUTH_IDENTITY_EXA;

#ifdef UNICODE
#define SEC_WINNT_AUTH_IDENTITY_EX  SEC_WINNT_AUTH_IDENTITY_EXW    // ntifs
#define PSEC_WINNT_AUTH_IDENTITY_EX PSEC_WINNT_AUTH_IDENTITY_EXW   // ntifs
#else
#define SEC_WINNT_AUTH_IDENTITY_EX  SEC_WINNT_AUTH_IDENTITY_EXA
#endif

// begin_ntifs

#endif // SEC_WINNT_AUTH_IDENTITY_VERSION

// end_ntifs

#ifndef _AUTH_IDENTITY_INFO_DEFINED
#define _AUTH_IDENTITY_INFO_DEFINED

//
// the procedure for how to parse a SEC_WINNT_AUTH_IDENTITY_INFO structure:
//
// 1) First check the first DWORD of SEC_WINNT_AUTH_IDENTITY_INFO, if the first
//   DWORD is 0x200, it is either an AuthIdExw or AuthIdExA, otherwise if the first
//   DWORD is 0x201, the structure is an AuthIdEx2 structure. Otherwise the structure
//   is either an AuthId_a or an AuthId_w.
//
// 2) Secondly check the flags for SEC_WINNT_AUTH_IDENTITY_ANSI or
//   SEC_WINNT_AUTH_IDENTITY_UNICODE, the presence of the former means the structure
//   is an ANSI structure. Otherwise, the structure is the wide version.  Note that
//   AuthIdEx2 does not have an ANSI version so this check does not apply to it.
//

typedef union _SEC_WINNT_AUTH_IDENTITY_INFO {
    SEC_WINNT_AUTH_IDENTITY_EXW AuthIdExw;
    SEC_WINNT_AUTH_IDENTITY_EXA AuthIdExa;
    SEC_WINNT_AUTH_IDENTITY_A AuthId_a;
    SEC_WINNT_AUTH_IDENTITY_W AuthId_w;
    SEC_WINNT_AUTH_IDENTITY_EX2 AuthIdEx2;
} SEC_WINNT_AUTH_IDENTITY_INFO, *PSEC_WINNT_AUTH_IDENTITY_INFO;

// the credential structure is encrypted via
// RtlEncryptMemory(OptionFlags = 0)
#define SEC_WINNT_AUTH_IDENTITY_FLAGS_PROCESS_ENCRYPTED 0x10

// the credential structure is protected by local system via
// RtlEncryptMemory(OptionFlags =
// IOCTL_KSEC_ENCRYPT_MEMORY_SAME_LOGON)
#define SEC_WINNT_AUTH_IDENTITY_FLAGS_SYSTEM_PROTECTED  0x20

#define SEC_WINNT_AUTH_IDENTITY_FLAGS_RESERVED       0x10000
#define SEC_WINNT_AUTH_IDENTITY_FLAGS_NULL_USER      0x20000
#define SEC_WINNT_AUTH_IDENTITY_FLAGS_NULL_DOMAIN    0x40000

//
//  These bits are for communication between SspiPromptForCredentials()
//  and the credential providers. Do not use these bits for any other
//  purpose.
//

#define SEC_WINNT_AUTH_IDENTITY_FLAGS_SSPIPFC_USE_MASK  0xFF000000

//
//  Instructs the credential provider to not save credentials itself
//  when caller selects the "Remember my credential" checkbox.
//

#define SEC_WINNT_AUTH_IDENTITY_FLAGS_SSPIPFC_SAVE_CRED_BY_CALLER   0x80000000

//
//  State of the "Remember my credentials" checkbox.
//  When set, indicates checked; when cleared, indicates unchecked.
//

#define SEC_WINNT_AUTH_IDENTITY_FLAGS_SSPIPFC_SAVE_CRED_CHECKED     0x40000000

#define SEC_WINNT_AUTH_IDENTITY_FLAGS_VALID_SSPIPFC_FLAGS   \
                (SEC_WINNT_AUTH_IDENTITY_FLAGS_SSPIPFC_SAVE_CRED_BY_CALLER | \
                 SEC_WINNT_AUTH_IDENTITY_FLAGS_SSPIPFC_SAVE_CRED_CHECKED)


#endif // _AUTH_IDENTITY_INFO_DEFINED

#ifndef _SSPIPFC_NONE_ // the public view

// begin_ntifs

typedef PVOID PSEC_WINNT_AUTH_IDENTITY_OPAQUE; // the credential structure is opaque

// end_ntifs

#else  // the internal view

typedef PSEC_WINNT_AUTH_IDENTITY_INFO PSEC_WINNT_AUTH_IDENTITY_OPAQUE;

#endif // _SSPIPFC_NONE_

//
//  dwFlags parameter of SspiPromptForCredentials():
//

//
//  Indicates that the credentials should not be saved if
//  the user selects the 'save' (or 'remember my password')
//  checkbox in the credential dialog box. The location pointed
//  to by the pfSave parameter indicates whether or not the user
//  selected the checkbox.
//
//  Note that some credential providers won't honour this flag and
//  may save the credentials in a persistent manner anyway if the
//  user selects the 'save' checbox.
//

#define SSPIPFC_SAVE_CRED_BY_CALLER     0x00000001

#define SSPIPFC_VALID_FLAGS (SSPIPFC_SAVE_CRED_BY_CALLER)

#ifndef _SSPIPFC_NONE_ // the public view

// Use SspiFreeAuthIdentity() to free the buffer returned
// in ppAuthIdentity.

unsigned long
SEC_ENTRY
SspiPromptForCredentialsW(
    __in PCWSTR pszTargetName,
#ifdef _CREDUI_INFO_DEFINED
    __in_opt PCREDUI_INFOW pUiInfo,
#else
    __in_opt PVOID pUiInfo,
#endif // _CREDUI_INFO_DEFINED
    __in unsigned long dwAuthError,
    __in PCWSTR pszPackage,
    __in_opt PSEC_WINNT_AUTH_IDENTITY_OPAQUE pInputAuthIdentity,
    __deref_out PSEC_WINNT_AUTH_IDENTITY_OPAQUE* ppAuthIdentity,
    __inout_opt int* pfSave,
    __in unsigned long dwFlags
    );

// Use SspiFreeAuthIdentity() to free the buffer returned
// in ppAuthIdentity.

unsigned long
SEC_ENTRY
SspiPromptForCredentialsA(
    __in PCSTR pszTargetName,
#ifdef _CREDUI_INFO_DEFINED
    __in_opt PCREDUI_INFOA pUiInfo,
#else
    __in_opt PVOID pUiInfo,
#endif // _CREDUI_INFO_DEFINED
    __in unsigned long dwAuthError,
    __in PCSTR pszPackage,
    __in_opt PSEC_WINNT_AUTH_IDENTITY_OPAQUE pInputAuthIdentity,
    __deref_out PSEC_WINNT_AUTH_IDENTITY_OPAQUE* ppAuthIdentity,
    __inout_opt int* pfSave,
    __in unsigned long dwFlags
    );
#endif // _SSPIPFC_NONE_

#ifdef UNICODE
#define SspiPromptForCredentials   SspiPromptForCredentialsW
#else
#define SspiPromptForCredentials   SspiPromptForCredentialsA
#endif

#ifdef _SEC_WINNT_AUTH_TYPES

typedef struct _SEC_WINNT_AUTH_BYTE_VECTOR {
    unsigned long ByteArrayOffset; // each element is a byte
    unsigned short ByteArrayLength; //
} SEC_WINNT_AUTH_BYTE_VECTOR, *PSEC_WINNT_AUTH_BYTE_VECTOR;

typedef struct _SEC_WINNT_AUTH_DATA {
   GUID CredType;
   SEC_WINNT_AUTH_BYTE_VECTOR CredData;
} SEC_WINNT_AUTH_DATA, *PSEC_WINNT_AUTH_DATA;

typedef struct _SEC_WINNT_AUTH_PACKED_CREDENTIALS {
   unsigned short cbHeaderLength;    // the length of the header
   unsigned short cbStructureLength; // pay load length including the header
   SEC_WINNT_AUTH_DATA AuthData;
} SEC_WINNT_AUTH_PACKED_CREDENTIALS, *PSEC_WINNT_AUTH_PACKED_CREDENTIALS;

// {28BFC32F-10F6-4738-98D1-1AC061DF716A}
static const GUID SEC_WINNT_AUTH_DATA_TYPE_PASSWORD =
   { 0x28bfc32f, 0x10f6, 0x4738, { 0x98, 0xd1, 0x1a, 0xc0, 0x61, 0xdf, 0x71, 0x6a } };

// {235F69AD-73FB-4dbc-8203-0629E739339B}
static const GUID SEC_WINNT_AUTH_DATA_TYPE_CERT =
   { 0x235f69ad, 0x73fb, 0x4dbc, { 0x82, 0x3, 0x6, 0x29, 0xe7, 0x39, 0x33, 0x9b } };

typedef struct _SEC_WINNT_AUTH_DATA_PASSWORD {
   SEC_WINNT_AUTH_BYTE_VECTOR UnicodePassword;
} SEC_WINNT_AUTH_DATA_PASSWORD, PSEC_WINNT_AUTH_DATA_PASSWORD;

//
// smartcard cred data
//
// {68FD9879-079C-4dfe-8281-578AADC1C100}

static const GUID SEC_WINNT_AUTH_DATA_TYPE_CSP_DATA =
   { 0x68fd9879, 0x79c, 0x4dfe, { 0x82, 0x81, 0x57, 0x8a, 0xad, 0xc1, 0xc1, 0x0 } };

typedef struct _SEC_WINNT_AUTH_CERTIFICATE_DATA {
   unsigned short cbHeaderLength;
   unsigned short cbStructureLength;
   SEC_WINNT_AUTH_BYTE_VECTOR Certificate;
} SEC_WINNT_AUTH_CERTIFICATE_DATA, *PSEC_WINNT_AUTH_CERTIFICATE_DATA;

typedef struct _SEC_WINNT_CREDUI_CONTEXT_VECTOR
{
   ULONG CredUIContextArrayOffset; // offset starts at the beginning of
   // this structure, and each element is a SEC_WINNT_AUTH_BYTE_VECTOR that
   // describes the flat CredUI context returned by SpGetCredUIContext()
   USHORT CredUIContextCount;
} SEC_WINNT_CREDUI_CONTEXT_VECTOR, *PSEC_WINNT_CREDUI_CONTEXT_VECTOR;

typedef struct _SEC_WINNT_AUTH_SHORT_VECTOR
{
    ULONG ShortArrayOffset; // each element is a short
    USHORT ShortArrayCount; // number of characters
} SEC_WINNT_AUTH_SHORT_VECTOR, *PSEC_WINNT_AUTH_SHORT_VECTOR;

// free the returned memory using SspiLocalFree

SECURITY_STATUS
SEC_ENTRY
SspiGetCredUIContext(
   __in HANDLE ContextHandle,
   __in GUID* CredType,
   __in_opt LUID* LogonId, // use this LogonId, the caller must be localsystem to supply a logon id
   __deref_out PSEC_WINNT_CREDUI_CONTEXT_VECTOR* CredUIContexts,
   __out_opt HANDLE* TokenHandle
   );

SECURITY_STATUS
SEC_ENTRY
SspiUpdateCredentials(
   __in HANDLE ContextHandle,
   __in GUID* CredType,
   __in ULONG FlatCredUIContextLength,
   __in_bcount(FlatCredUIContextLength) PUCHAR FlatCredUIContext
   );

typedef struct _CREDUIWIN_MARSHALED_CONTEXT
{
    GUID StructureType;
    USHORT cbHeaderLength;
    LUID LogonId; // user's logon id
    GUID MarshaledDataType;
    ULONG MarshaledDataOffset;
    USHORT MarshaledDataLength;
} CREDUIWIN_MARSHALED_CONTEXT, *PCREDUIWIN_MARSHALED_CONTEXT;

typedef struct _SEC_WINNT_CREDUI_CONTEXT
{
    USHORT cbHeaderLength;
    HANDLE CredUIContextHandle; // the handle to call SspiGetCredUIContext()
#ifdef _CREDUI_INFO_DEFINED
    PCREDUI_INFOW UIInfo; // input from SspiPromptForCredentials()
#else
    PVOID UIInfo;
#endif // _CREDUI_INFO_DEFINED
    ULONG dwAuthError; // the authentication error
    PSEC_WINNT_AUTH_IDENTITY_OPAQUE pInputAuthIdentity;
    PUNICODE_STRING TargetName;
} SEC_WINNT_CREDUI_CONTEXT, *PSEC_WINNT_CREDUI_CONTEXT;

// {3C3E93D9-D96B-49b5-94A7-458592088337}
static const GUID CREDUIWIN_STRUCTURE_TYPE_SSPIPFC  =
{ 0x3c3e93d9, 0xd96b, 0x49b5, { 0x94, 0xa7, 0x45, 0x85, 0x92, 0x8, 0x83, 0x37 } };

// {C2FFFE6F-503D-4c3d-A95E-BCE821213D44}
static const GUID SSPIPFC_STRUCTURE_TYPE_CREDUI_CONTEXT =
{ 0xc2fffe6f, 0x503d, 0x4c3d, { 0xa9, 0x5e, 0xbc, 0xe8, 0x21, 0x21, 0x3d, 0x44 } };

typedef struct _SEC_WINNT_AUTH_PACKED_CREDENTIALS_EX {
   unsigned short cbHeaderLength;
   unsigned long Flags; // contains the Flags field in
                        // SEC_WINNT_AUTH_IDENTITY_EX
   SEC_WINNT_AUTH_BYTE_VECTOR PackedCredentials;
   SEC_WINNT_AUTH_SHORT_VECTOR PackageList;
} SEC_WINNT_AUTH_PACKED_CREDENTIALS_EX, *PSEC_WINNT_AUTH_PACKED_CREDENTIALS_EX;

//
// free the returned memory using SspiLocalFree
//

SECURITY_STATUS
SEC_ENTRY
SspiUnmarshalCredUIContext(
    __in_bcount(MarshaledCredUIContextLength) PUCHAR MarshaledCredUIContext,
    __in ULONG MarshaledCredUIContextLength,
    __deref_out PSEC_WINNT_CREDUI_CONTEXT* CredUIContext
    );

#endif // _SEC_WINNT_AUTH_TYPES

SECURITY_STATUS
SEC_ENTRY
SspiPrepareForCredRead(
    __in PSEC_WINNT_AUTH_IDENTITY_OPAQUE AuthIdentity,
    __in PCWSTR pszTargetName,
    __out PULONG pCredmanCredentialType,
    __deref_out PCWSTR* ppszCredmanTargetName
    );

SECURITY_STATUS
SEC_ENTRY
SspiPrepareForCredWrite(
    __in PSEC_WINNT_AUTH_IDENTITY_OPAQUE AuthIdentity,
    __in_opt PCWSTR pszTargetName, // supply NULL for username-target credentials
    __out PULONG pCredmanCredentialType,
    __deref_out PCWSTR* ppszCredmanTargetName,
    __deref_out PCWSTR* ppszCredmanUserName,
    __deref_out_bcount(*pCredentialBlobSize) PUCHAR *ppCredentialBlob,
    __out PULONG pCredentialBlobSize
    );

SECURITY_STATUS
SEC_ENTRY
SspiEncryptAuthIdentity(
    __inout PSEC_WINNT_AUTH_IDENTITY_OPAQUE AuthData
    );

SECURITY_STATUS
SEC_ENTRY
SspiDecryptAuthIdentity(
    __inout PSEC_WINNT_AUTH_IDENTITY_OPAQUE EncryptedAuthData
    );

BOOLEAN
SEC_ENTRY
SspiIsAuthIdentityEncrypted(
    __in PSEC_WINNT_AUTH_IDENTITY_OPAQUE EncryptedAuthData
    );

// begin_ntifs

#if (NTDDI_VERSION >= NTDDI_WIN7)                                 
//
//  Convert the _OPAQUE structure passed in to the
//  3 tuple <username, domainname, 'password'>.
//
//  Note: The 'strings' returned need not necessarily be
//  in user recognisable form. The purpose of this API
//  is to 'flatten' the _OPAQUE structure into the 3 tuple.
//  User recognisable <username, domainname> can always be
//  obtained by passing NULL to the pszPackedCredentialsString
//  parameter.
//
// zero out the pszPackedCredentialsString then
// free the returned memory using SspiLocalFree()
//

SECURITY_STATUS
SEC_ENTRY
SspiEncodeAuthIdentityAsStrings(
    __in PSEC_WINNT_AUTH_IDENTITY_OPAQUE pAuthIdentity,
    __deref_out_opt PCWSTR* ppszUserName,
    __deref_out_opt PCWSTR* ppszDomainName,
    __deref_opt_out_opt PCWSTR* ppszPackedCredentialsString
    );

SECURITY_STATUS
SEC_ENTRY
SspiValidateAuthIdentity(
    __in PSEC_WINNT_AUTH_IDENTITY_OPAQUE AuthData
    );

//
// free the returned memory using SspiFreeAuthIdentity()
//

SECURITY_STATUS
SEC_ENTRY
SspiCopyAuthIdentity(
    __in PSEC_WINNT_AUTH_IDENTITY_OPAQUE AuthData,
    __deref_out PSEC_WINNT_AUTH_IDENTITY_OPAQUE* AuthDataCopy
    );

//
// use only for the memory returned by SspiCopyAuthIdentity().
// Internally calls SspiZeroAuthIdentity().
//

VOID
SEC_ENTRY
SspiFreeAuthIdentity(
    __in_opt PSEC_WINNT_AUTH_IDENTITY_OPAQUE AuthData
    );

VOID
SEC_ENTRY
SspiZeroAuthIdentity(
    __in_opt PSEC_WINNT_AUTH_IDENTITY_OPAQUE AuthData
    );

VOID
SEC_ENTRY
SspiLocalFree(
    __in_opt PVOID DataBuffer
    );

//
// call SspiFreeAuthIdentity to free the returned AuthIdentity 
// which zeroes out the credentials blob before freeing it
//

SECURITY_STATUS
SEC_ENTRY
SspiEncodeStringsAsAuthIdentity(
    __in_opt PCWSTR pszUserName,
    __in_opt PCWSTR pszDomainName,
    __in_opt PCWSTR pszPackedCredentialsString,
    __deref_out PSEC_WINNT_AUTH_IDENTITY_OPAQUE* ppAuthIdentity
    );

SECURITY_STATUS
SEC_ENTRY
SspiCompareAuthIdentities(
    __in_opt PSEC_WINNT_AUTH_IDENTITY_OPAQUE AuthIdentity1,
    __in_opt PSEC_WINNT_AUTH_IDENTITY_OPAQUE AuthIdentity2,
    __out_opt PBOOLEAN SameSuppliedUser,
    __out_opt PBOOLEAN SameSuppliedIdentity
    );

//
// zero out the returned AuthIdentityByteArray then
// free the returned memory using SspiLocalFree()
//

SECURITY_STATUS
SEC_ENTRY
SspiMarshalAuthIdentity(
    __in PSEC_WINNT_AUTH_IDENTITY_OPAQUE AuthIdentity,
    __out unsigned long* AuthIdentityLength,
    __deref_out_bcount(*AuthIdentityLength) char** AuthIdentityByteArray
    );

//
// free the returned auth identity using SspiFreeAuthIdentity()
//

SECURITY_STATUS
SEC_ENTRY
SspiUnmarshalAuthIdentity(
    __in unsigned long AuthIdentityLength,
    __in_bcount(AuthIdentityLength) char* AuthIdentityByteArray,
    __deref_out PSEC_WINNT_AUTH_IDENTITY_OPAQUE* ppAuthIdentity
    );

BOOLEAN
SEC_ENTRY
SspiIsPromptingNeeded(
    __in unsigned long ErrorOrNtStatus
    );

SECURITY_STATUS
SEC_ENTRY
SspiGetTargetHostName(
    __in PCWSTR pszTargetName,
    __deref_out PWSTR* pszHostName
    );

SECURITY_STATUS
SEC_ENTRY
SspiExcludePackage(
    __in_opt PSEC_WINNT_AUTH_IDENTITY_OPAQUE AuthIdentity,
    __in PCWSTR pszPackageName,
    __deref_out PSEC_WINNT_AUTH_IDENTITY_OPAQUE* ppNewAuthIdentity
    );

//
// Common types used by negotiable security packages
//
// These are defined after W2K
//

#define SEC_WINNT_AUTH_IDENTITY_MARSHALLED      0x4     // all data is in one buffer
#define SEC_WINNT_AUTH_IDENTITY_ONLY            0x8     // these credentials are for identity only - no PAC needed

#endif // NTDDI_VERSION
// end_ntifs

//
// Routines for manipulating packages
//

typedef struct _SECURITY_PACKAGE_OPTIONS {
    unsigned long   Size;
    unsigned long   Type;
    unsigned long   Flags;
    unsigned long   SignatureSize;
    void *  Signature;
} SECURITY_PACKAGE_OPTIONS, * PSECURITY_PACKAGE_OPTIONS;

#define SECPKG_OPTIONS_TYPE_UNKNOWN 0
#define SECPKG_OPTIONS_TYPE_LSA     1
#define SECPKG_OPTIONS_TYPE_SSPI    2

#define SECPKG_OPTIONS_PERMANENT    0x00000001

SECURITY_STATUS
SEC_ENTRY
AddSecurityPackageA(
    __in     LPSTR                     pszPackageName,
    __in_opt PSECURITY_PACKAGE_OPTIONS pOptions
    );

SECURITY_STATUS
SEC_ENTRY
AddSecurityPackageW(
    __in     LPWSTR                    pszPackageName,
    __in_opt PSECURITY_PACKAGE_OPTIONS pOptions
    );

#ifdef UNICODE
#define AddSecurityPackage  AddSecurityPackageW
#else
#define AddSecurityPackage  AddSecurityPackageA
#endif

SECURITY_STATUS
SEC_ENTRY
DeleteSecurityPackageA(
    __in LPSTR pszPackageName
    );

SECURITY_STATUS
SEC_ENTRY
DeleteSecurityPackageW(
    __in LPWSTR pszPackageName
    );

#ifdef UNICODE
#define DeleteSecurityPackage   DeleteSecurityPackageW
#else
#define DeleteSecurityPackage   DeleteSecurityPackageA
#endif

#ifdef __cplusplus
}  // extern "C"
#endif

// begin_ntifs
#endif // __SSPI_H__
// end_ntifs

