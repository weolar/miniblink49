#ifndef curl_sspivc6_h
#define curl_sspivc6_h

#include "schnlspvc6.h"

#define SCHANNEL_SHUTDOWN 1 

typedef ULONG_PTR HCRYPTPROV;

#define PROV_RSA_FULL 1
#define CRYPT_VERIFYCONTEXT 0xF0000000
#define CRYPT_SILENT 0x00000040

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
#define ISC_REQ_UNVERIFIED_TARGET_NAME  0x20000000
#define ISC_REQ_CONFIDENTIALITY_ONLY    0x40000000 // honored by SPNEGO/Kerberos

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
#define ISC_RET_NO_ADDITIONAL_TOKEN     0x02000000 // *INTERNAL*
#define ISC_RET_REAUTHENTICATION        0x08000000 // *INTERNAL*
#define ISC_RET_CONFIDENTIALITY_ONLY    0x40000000 // honored by SPNEGO/Kerberos

#define ASC_REQ_DELEGATE                0x00000001
#define ASC_REQ_MUTUAL_AUTH             0x00000002
#define ASC_REQ_REPLAY_DETECT           0x00000004
#define ASC_REQ_SEQUENCE_DETECT         0x00000008
#define ASC_REQ_CONFIDENTIALITY         0x00000010
#define ASC_REQ_USE_SESSION_KEY         0x00000020
#define ASC_REQ_SESSION_TICKET          0x00000040
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
#define ASC_RET_SESSION_TICKET          0x00000040
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
//      SSP_RET_REAUTHENTICATION        0x08000000  // *INTERNAL*

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

//
// win8 or greater
//
#define SECPKG_ATTR_DTLS_MTU        34
#define SECPKG_ATTR_DATAGRAM_SIZES  SECPKG_ATTR_STREAM_SIZES

#define SECPKG_ATTR_SUBJECT_SECURITY_ATTRIBUTES 128

//
// win8.1 or greater
//
#define SECPKG_ATTR_APPLICATION_PROTOCOL 35

#define FreeCredentialHandle FreeCredentialsHandle

typedef void* UNKNIW_FN;

#ifndef __SECSTATUS_DEFINED__
typedef LONG SECURITY_STATUS;
#define __SECSTATUS_DEFINED__
#endif

#define SEC_TEXT TEXT
#define SEC_FAR
#define SEC_ENTRY __stdcall

#ifndef __SECSTATUS_DEFINED__
typedef LONG SECURITY_STATUS;
#define __SECSTATUS_DEFINED__
#endif

typedef WCHAR SEC_WCHAR;
typedef CHAR SEC_CHAR;

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
} SecPkgInfoW, *PSecPkgInfoW;

#  define SecPkgInfo SecPkgInfoW        // ntifs
#  define PSecPkgInfo PSecPkgInfoW      // ntifs

typedef struct _SecBuffer {
    unsigned long cbBuffer;             // Size of the buffer, in bytes
    unsigned long BufferType;           // Type of the buffer (below)
#ifdef MIDL_PASS
    [size_is(cbBuffer)] char * pvBuffer;                         // Pointer to the buffer
#else
    void* pvBuffer;            // Pointer to the buffer
#endif
} SecBuffer, *PSecBuffer;

typedef struct _SecBufferDesc {
    unsigned long ulVersion;            // Version number
    unsigned long cBuffers;             // Number of buffers
#ifdef MIDL_PASS
    [size_is(cBuffers)]
#endif
    PSecBuffer pBuffers;                // Pointer to array of buffers
} SecBufferDesc, *PSecBufferDesc;


#define SECPKG_CRED_ATTR_NAMES        1
#define SECPKG_CRED_ATTR_SSI_PROVIDER 2
#define SECPKG_CRED_ATTR_KDC_PROXY_SETTINGS 3
#define SECPKG_CRED_ATTR_CERT         4

typedef struct _SecPkgCredentials_NamesW
{
#ifdef MIDL_PASS
    [string]
#endif
    SEC_WCHAR * sUserName;

} SecPkgCredentials_NamesW, *PSecPkgCredentials_NamesW;

#  define SecPkgCredentials_Names SecPkgCredentials_NamesW      // ntifs
#  define PSecPkgCredentials_Names PSecPkgCredentials_NamesW    // ntifs

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
#define SECBUFFER_APPLICATION_PROTOCOLS 18  // Lists of application protocol IDs, one per negotiation extension

#define SECBUFFER_ATTRMASK                      0xF0000000
#define SECBUFFER_READONLY                      0x80000000  // Buffer is read-only, no checksum
#define SECBUFFER_READONLY_WITH_CHECKSUM        0x10000000  // Buffer is read-only, and checksummed
#define SECBUFFER_RESERVED                      0x60000000  // Flags reserved to security system

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

typedef enum _SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT
{
    SecApplicationProtocolNegotiationExt_None,
    SecApplicationProtocolNegotiationExt_NPN,
    SecApplicationProtocolNegotiationExt_ALPN
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT, *PSEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT;

typedef enum _SEC_APPLICATION_PROTOCOL_NEGOTIATION_STATUS
{
    SecApplicationProtocolNegotiationStatus_None,
    SecApplicationProtocolNegotiationStatus_Success,
    SecApplicationProtocolNegotiationStatus_SelectedClientOnly
} SEC_APPLICATION_PROTOCOL_NEGOTIATION_STATUS, *PSEC_APPLICATION_PROTOCOL_NEGOTIATION_STATUS;

#define MAX_PROTOCOL_ID_SIZE 0xff

typedef struct _SecPkgContext_ApplicationProtocol
{
    SEC_APPLICATION_PROTOCOL_NEGOTIATION_STATUS ProtoNegoStatus; // Application  protocol negotiation status
    SEC_APPLICATION_PROTOCOL_NEGOTIATION_EXT ProtoNegoExt;       // Protocol negotiation extension type corresponding to this protocol ID
    unsigned char ProtocolIdSize;                                // Size in bytes of the application protocol ID
    unsigned char ProtocolId[MAX_PROTOCOL_ID_SIZE];              // Byte string representing the negotiated application protocol ID
} SecPkgContext_ApplicationProtocol, *PSecPkgContext_ApplicationProtocol;

#define SECPKG_ATTR_ISSUER_LIST          0x50   // (OBSOLETE) returns SecPkgContext_IssuerListInfo
#define SECPKG_ATTR_REMOTE_CRED          0x51   // (OBSOLETE) returns SecPkgContext_RemoteCredentialInfo
#define SECPKG_ATTR_LOCAL_CRED           0x52   // (OBSOLETE) returns SecPkgContext_LocalCredentialInfo
#define SECPKG_ATTR_REMOTE_CERT_CONTEXT  0x53   // returns PCCERT_CONTEXT
#define SECPKG_ATTR_LOCAL_CERT_CONTEXT   0x54   // returns PCCERT_CONTEXT
#define SECPKG_ATTR_ROOT_STORE           0x55   // returns HCERTCONTEXT to the root store
#define SECPKG_ATTR_SUPPORTED_ALGS       0x56   // returns SecPkgCred_SupportedAlgs
#define SECPKG_ATTR_CIPHER_STRENGTHS     0x57   // returns SecPkgCred_CipherStrengths
#define SECPKG_ATTR_SUPPORTED_PROTOCOLS  0x58   // returns SecPkgCred_SupportedProtocols
#define SECPKG_ATTR_ISSUER_LIST_EX       0x59   // returns SecPkgContext_IssuerListInfoEx
#define SECPKG_ATTR_CONNECTION_INFO      0x5a   // returns SecPkgContext_ConnectionInfo
#define SECPKG_ATTR_EAP_KEY_BLOCK        0x5b   // returns SecPkgContext_EapKeyBlock
#define SECPKG_ATTR_MAPPED_CRED_ATTR     0x5c   // returns SecPkgContext_MappedCredAttr
#define SECPKG_ATTR_SESSION_INFO         0x5d   // returns SecPkgContext_SessionInfo
#define SECPKG_ATTR_APP_DATA             0x5e   // sets/returns SecPkgContext_SessionAppData
#define SECPKG_ATTR_REMOTE_CERTIFICATES  0x5F   // returns SecPkgContext_Certificates
#define SECPKG_ATTR_CLIENT_CERT_POLICY   0x60   // sets    SecPkgCred_ClientCertCtlPolicy
#define SECPKG_ATTR_CC_POLICY_RESULT     0x61   // returns SecPkgContext_ClientCertPolicyResult
#define SECPKG_ATTR_USE_NCRYPT           0x62   // Sets the CRED_FLAG_USE_NCRYPT_PROVIDER FLAG on cred group
#define SECPKG_ATTR_LOCAL_CERT_INFO      0x63   // returns SecPkgContext_CertInfo
#define SECPKG_ATTR_CIPHER_INFO          0x64   // returns new CNG SecPkgContext_CipherInfo
#define SECPKG_ATTR_EAP_PRF_INFO         0x65   // sets    SecPkgContext_EapPrfInfo
#define SECPKG_ATTR_SUPPORTED_SIGNATURES 0x66   // returns SecPkgContext_SupportedSignatures
#define SECPKG_ATTR_REMOTE_CERT_CHAIN    0x67   // returns PCCERT_CONTEXT
#define SECPKG_ATTR_UI_INFO              0x68   // sets SEcPkgContext_UiInfo
#define SECPKG_ATTR_EARLY_START          0x69   // sets SecPkgContext_EarlyStart

#define X509_ASN_ENCODING 0x00000001

#ifndef __WINCRYPT_H__
typedef void *HCERTSTORE;
struct _CERT_INFO;
typedef struct _CERT_INFO CERT_INFO, *PCERT_INFO;

typedef struct _CERT_CONTEXT {
    DWORD                   dwCertEncodingType;
    BYTE                    *pbCertEncoded;
    DWORD                   cbCertEncoded;
    PCERT_INFO              pCertInfo;
    HCERTSTORE              hCertStore;
} CERT_CONTEXT, *PCERT_CONTEXT;
#endif // __WINCRYPT_H__

typedef void
(SEC_ENTRY * SEC_GET_KEY_FN) (
    void * Arg,                 // Argument passed in
    void * Principal,           // Principal ID
    unsigned long KeyVer,               // Key Version
    void * * Key,       // Returned ptr to key
    SECURITY_STATUS * Status    // returned status
    );

#  ifndef _NTDEF_
typedef struct _SECURITY_STRING {
    unsigned short      Length;
    unsigned short      MaximumLength;
#    ifdef MIDL_PASS
    [size_is(MaximumLength / 2), length_is(Length / 2)]
#    endif // MIDL_PASS
    unsigned short *    Buffer;
} SECURITY_STRING, *PSECURITY_STRING;
#  else // _NTDEF_
typedef UNICODE_STRING SECURITY_STRING, *PSECURITY_STRING;  // ntifs
#  endif // _NTDEF_

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

SECURITY_STATUS SEC_ENTRY
AcquireCredentialsHandleW(
#if ISSP_MODE == 0     // For Kernel mode
    /*_In_opt_*/  PSECURITY_STRING pPrincipal,
    /*_In_*/      PSECURITY_STRING pPackage,
#else
    /*_In_opt_*/  LPWSTR pszPrincipal,                // Name of principal
    /*_In_*/      LPWSTR pszPackage,                  // Name of package
#endif
    /*_In_*/      unsigned long fCredentialUse,       // Flags indicating use
    /*_In_opt_*/  void * pvLogonId,           // Pointer to logon ID
    /*_In_opt_*/  void * pAuthData,           // Package specific data
    /*_In_opt_*/  SEC_GET_KEY_FN pGetKeyFn,           // Pointer to GetKey() func
    /*_In_opt_*/  void * pvGetKeyArgument,    // Value to pass to GetKey()
    /*_Out_*/     PCredHandle phCredential,           // (out) Cred Handle
    /*_Out_opt_*/ PTimeStamp ptsExpiry                // (out) Lifetime (optional)
    );

#  define AcquireCredentialsHandle AcquireCredentialsHandleW            // ntifs
#  define ACQUIRE_CREDENTIALS_HANDLE_FN ACQUIRE_CREDENTIALS_HANDLE_FN_W // ntifs

SECURITY_STATUS SEC_ENTRY
FreeCredentialsHandle(
    /*_In_*/ PCredHandle phCredential            // Handle to free
    );

typedef SECURITY_STATUS
(SEC_ENTRY * FREE_CREDENTIALS_HANDLE_FN)(
    PCredHandle);

SECURITY_STATUS SEC_ENTRY
FreeContextBuffer(
    /*_Inout_*/ PVOID pvContextBuffer      // buffer to free
    );

typedef SECURITY_STATUS
(SEC_ENTRY * FREE_CONTEXT_BUFFER_FN)(
    /*_Inout_*/ PVOID
    );

SECURITY_STATUS SEC_ENTRY
InitializeSecurityContextW(
    /*_In_opt_*/    PCredHandle phCredential,               // Cred to base context
    /*_In_opt_*/    PCtxtHandle phContext,                  // Existing context (OPT)
#if ISSP_MODE == 0
    /*_In_opt_*/ PSECURITY_STRING pTargetName,
#else
    /*_In_opt_*/ SEC_WCHAR * pszTargetName,         // Name of target
#endif
    /*_In_*/        unsigned long fContextReq,              // Context Requirements
    /*_In_*/        unsigned long Reserved1,                // Reserved, MBZ
    /*_In_*/        unsigned long TargetDataRep,            // Data rep of target
    /*_In_opt_*/    PSecBufferDesc pInput,                  // Input Buffers
    /*_In_*/        unsigned long Reserved2,                // Reserved, MBZ
    /*_Inout_opt_*/ PCtxtHandle phNewContext,               // (out) New Context handle
    /*_Inout_opt_*/ PSecBufferDesc pOutput,                 // (inout) Output Buffers
    /*_Out_*/       unsigned long * pfContextAttr,  // (out) Context attrs
    /*_Out_opt_*/   PTimeStamp ptsExpiry                    // (out) Life span (OPT)
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

#  define InitializeSecurityContext InitializeSecurityContextW              // ntifs
#  define INITIALIZE_SECURITY_CONTEXT_FN INITIALIZE_SECURITY_CONTEXT_FN_W   // ntifs

SECURITY_STATUS SEC_ENTRY
QueryCredentialsAttributesW(
    /*_In_*/    PCredHandle phCredential,           // Credential to query
    /*_In_*/    unsigned long ulAttribute,          // Attribute to query
    /*_Inout_*/ void * pBuffer              // Buffer for attributes
    );

typedef SECURITY_STATUS
(SEC_ENTRY * QUERY_CREDENTIALS_ATTRIBUTES_FN_W)(
    PCredHandle,
    unsigned long,
    void *);

#  define QueryCredentialsAttributes QueryCredentialsAttributesW            // ntifs
#  define QUERY_CREDENTIALS_ATTRIBUTES_FN QUERY_CREDENTIALS_ATTRIBUTES_FN_W // ntifs

SECURITY_STATUS SEC_ENTRY
DeleteSecurityContext(
    /*_In_*/ PCtxtHandle phContext               // Context to delete
    );

typedef SECURITY_STATUS
(SEC_ENTRY * DELETE_SECURITY_CONTEXT_FN)(
    PCtxtHandle);

SECURITY_STATUS SEC_ENTRY
QueryContextAttributesW(
    /*_In_*/  PCtxtHandle phContext,              // Context to query
    /*_In_*/  unsigned long ulAttribute,          // Attribute to query
    /*_Out_*/ void * pBuffer              // Buffer for attributes
    );

typedef SECURITY_STATUS
(SEC_ENTRY * QUERY_CONTEXT_ATTRIBUTES_FN_W)(
    PCtxtHandle,
    unsigned long,
    void *);

#  define QueryContextAttributes QueryContextAttributesW            // ntifs
#  define QUERY_CONTEXT_ATTRIBUTES_FN QUERY_CONTEXT_ATTRIBUTES_FN_W // ntifs

SECURITY_STATUS SEC_ENTRY
EncryptMessage(/*_In_*/    PCtxtHandle         phContext,
    /*_In_*/    unsigned long       fQOP,
    /*_In_*/    PSecBufferDesc      pMessage,
    /*_In_*/    unsigned long       MessageSeqNo);

typedef SECURITY_STATUS
(SEC_ENTRY * ENCRYPT_MESSAGE_FN)(
    PCtxtHandle, unsigned long, PSecBufferDesc, unsigned long);

SECURITY_STATUS SEC_ENTRY
DecryptMessage(/*_In_*/      PCtxtHandle         phContext,
    /*_In_*/      PSecBufferDesc      pMessage,
    /*_In_*/      unsigned long       MessageSeqNo,
    /*_Out_opt_*/ unsigned long *     pfQOP);


typedef SECURITY_STATUS
(SEC_ENTRY * DECRYPT_MESSAGE_FN)(
    PCtxtHandle, PSecBufferDesc, unsigned long,
    unsigned long *);

SECURITY_STATUS SEC_ENTRY
QuerySecurityPackageInfoW(
#if ISSP_MODE == 0
    /*_In_*/        PSECURITY_STRING pPackageName,
#else
    /*_In_*/        LPWSTR pszPackageName,          // Name of package
#endif
    /*_Outptr_*/ PSecPkgInfoW *ppPackageInfo     // Receives package info
    );

typedef SECURITY_STATUS
(SEC_ENTRY * QUERY_SECURITY_PACKAGE_INFO_FN_W)(
#if ISSP_MODE == 0
    PSECURITY_STRING,
#else
    SEC_WCHAR *,
#endif
    PSecPkgInfoW *);

#  define QuerySecurityPackageInfo QuerySecurityPackageInfoW                // ntifs
#  define QUERY_SECURITY_PACKAGE_INFO_FN QUERY_SECURITY_PACKAGE_INFO_FN_W   // ntifs

SECURITY_STATUS SEC_ENTRY
CompleteAuthToken(
    /*_In_*/ PCtxtHandle phContext,              // Context to complete
    /*_In_*/ PSecBufferDesc pToken               // Token to complete
    );

typedef SECURITY_STATUS
(SEC_ENTRY * COMPLETE_AUTH_TOKEN_FN)(
    PCtxtHandle,
    PSecBufferDesc);

SECURITY_STATUS SEC_ENTRY
ApplyControlToken(
    /*_In_*/ PCtxtHandle phContext,              // Context to modify
    /*_In_*/ PSecBufferDesc pInput               // Input token to apply
    );

typedef SECURITY_STATUS
(SEC_ENTRY * APPLY_CONTROL_TOKEN_FN)(
    PCtxtHandle, PSecBufferDesc);

typedef struct _SECURITY_FUNCTION_TABLE_W {
    unsigned long                       dwVersion;
    /*ENUMERATE_SECURITY_PACKAGES_FN_W*/UNKNIW_FN    EnumerateSecurityPackagesW;
    QUERY_CREDENTIALS_ATTRIBUTES_FN_W   QueryCredentialsAttributesW;
    ACQUIRE_CREDENTIALS_HANDLE_FN_W     AcquireCredentialsHandleW;
    FREE_CREDENTIALS_HANDLE_FN          FreeCredentialsHandle;
    void *                      Reserved2;
    INITIALIZE_SECURITY_CONTEXT_FN_W    InitializeSecurityContextW;
    /*ACCEPT_SECURITY_CONTEXT_FN*/UNKNIW_FN          AcceptSecurityContext;
    COMPLETE_AUTH_TOKEN_FN              CompleteAuthToken;
    DELETE_SECURITY_CONTEXT_FN          DeleteSecurityContext;
    APPLY_CONTROL_TOKEN_FN              ApplyControlToken;
    QUERY_CONTEXT_ATTRIBUTES_FN_W       QueryContextAttributesW;
    /*IMPERSONATE_SECURITY_CONTEXT_FN*/UNKNIW_FN     ImpersonateSecurityContext;
    /*REVERT_SECURITY_CONTEXT_FN*/UNKNIW_FN          RevertSecurityContext;
    /*MAKE_SIGNATURE_FN*/UNKNIW_FN                   MakeSignature;
    /*VERIFY_SIGNATURE_FN*/UNKNIW_FN                 VerifySignature;
    FREE_CONTEXT_BUFFER_FN              FreeContextBuffer;
    QUERY_SECURITY_PACKAGE_INFO_FN_W    QuerySecurityPackageInfoW;
    void *                      Reserved3;
    void *                      Reserved4;
    /*EXPORT_SECURITY_CONTEXT_FN*/UNKNIW_FN          ExportSecurityContext;
    /*IMPORT_SECURITY_CONTEXT_FN_W*/UNKNIW_FN        ImportSecurityContextW;
    /*ADD_CREDENTIALS_FN_W*/UNKNIW_FN                AddCredentialsW;
    void *                      Reserved8;
    /*QUERY_SECURITY_CONTEXT_TOKEN_FN*/UNKNIW_FN     QuerySecurityContextToken;
    ENCRYPT_MESSAGE_FN                  EncryptMessage;
    DECRYPT_MESSAGE_FN                  DecryptMessage;
//#if OSVER(NTDDI_VERSION) > NTDDI_WIN2K
    // Fields below this are available in OSes after w2k
    /*SET_CONTEXT_ATTRIBUTES_FN_W*/UNKNIW_FN         SetContextAttributesW;
//#endif // greater thean 2K

//#if NTDDI_VERSION > NTDDI_WS03SP1
    // Fields below this are available in OSes after W2k3SP1
    /*SET_CREDENTIALS_ATTRIBUTES_FN_W*/UNKNIW_FN     SetCredentialsAttributesW;
//#endif

#if ISSP_MODE != 0
    /*CHANGE_PASSWORD_FN_W*/UNKNIW_FN                ChangeAccountPasswordW;
#else
    void *                      Reserved9;
#endif
} SecurityFunctionTableW, *PSecurityFunctionTableW;


#  define SecurityFunctionTable SecurityFunctionTableW      // ntifs
#  define PSecurityFunctionTable PSecurityFunctionTableW    // ntifs

#endif