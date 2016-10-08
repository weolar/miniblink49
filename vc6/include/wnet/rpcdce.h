/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    rpcdce.h

Abstract:

    This module contains the DCE RPC runtime APIs.

--*/

#ifndef __RPCDCE_H__
#define __RPCDCE_H__

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef OPTIONAL
#define OPTIONAL
#endif

#ifndef DECLSPEC_NORETURN
#if (_MSC_VER >= 1200) && !defined(MIDL_PASS)
#define DECLSPEC_NORETURN   __declspec(noreturn)
#else
#define DECLSPEC_NORETURN
#endif
#endif

/*typedef char small;*/
/*typedef unsigned char byte;*/
/*typedef unsigned char boolean;*/

#include <specstrings.h>

typedef __nullterminated unsigned char __RPC_FAR *RPC_CSTR;
typedef __nullterminated unsigned short __RPC_FAR *RPC_WSTR;

typedef I_RPC_HANDLE RPC_BINDING_HANDLE;
typedef RPC_BINDING_HANDLE handle_t;
#define rpc_binding_handle_t RPC_BINDING_HANDLE

#ifndef GUID_DEFINED
#include <guiddef.h>
#endif /* GUID_DEFINED */

#ifndef UUID_DEFINED
#define UUID_DEFINED
typedef GUID UUID;
#ifndef uuid_t
#define uuid_t UUID
#endif
#endif

typedef struct _RPC_BINDING_VECTOR
{
    unsigned long Count;
    RPC_BINDING_HANDLE BindingH[1];
} RPC_BINDING_VECTOR;
#ifndef rpc_binding_vector_t
#define rpc_binding_vector_t RPC_BINDING_VECTOR
#endif

typedef struct _UUID_VECTOR
{
  unsigned long Count;
  UUID *Uuid[1];
} UUID_VECTOR;
#ifndef uuid_vector_t
#define uuid_vector_t UUID_VECTOR
#endif

typedef void __RPC_FAR * RPC_IF_HANDLE;

#ifndef IFID_DEFINED
#define IFID_DEFINED
typedef struct _RPC_IF_ID
{
    UUID Uuid;
    unsigned short VersMajor;
    unsigned short VersMinor;
} RPC_IF_ID;
#endif

#define RPC_C_BINDING_INFINITE_TIMEOUT 10
#define RPC_C_BINDING_MIN_TIMEOUT 0
#define RPC_C_BINDING_DEFAULT_TIMEOUT 5
#define RPC_C_BINDING_MAX_TIMEOUT 9

#define RPC_C_CANCEL_INFINITE_TIMEOUT -1

#define RPC_C_LISTEN_MAX_CALLS_DEFAULT 1234
#define RPC_C_PROTSEQ_MAX_REQS_DEFAULT 10

// RPC_POLICY EndpointFlags.
#define RPC_C_BIND_TO_ALL_NICS          1
#define RPC_C_USE_INTERNET_PORT         0x1
#define RPC_C_USE_INTRANET_PORT         0x2
#define RPC_C_DONT_FAIL                          0x4

// RPC_POLICY EndpointFlags specific to the Falcon/RPC transport:
#define RPC_C_MQ_TEMPORARY                  0x0000
#define RPC_C_MQ_PERMANENT                  0x0001
#define RPC_C_MQ_CLEAR_ON_OPEN              0x0002
#define RPC_C_MQ_USE_EXISTING_SECURITY      0x0004
#define RPC_C_MQ_AUTHN_LEVEL_NONE           0x0000
#define RPC_C_MQ_AUTHN_LEVEL_PKT_INTEGRITY  0x0008
#define RPC_C_MQ_AUTHN_LEVEL_PKT_PRIVACY    0x0010

// Client: RpcBindingSetOption() values for the Falcon/RPC transport:
#define RPC_C_OPT_MQ_DELIVERY            1
#define RPC_C_OPT_MQ_PRIORITY            2
#define RPC_C_OPT_MQ_JOURNAL             3
#define RPC_C_OPT_MQ_ACKNOWLEDGE         4
#define RPC_C_OPT_MQ_AUTHN_SERVICE       5
#define RPC_C_OPT_MQ_AUTHN_LEVEL         6
#define RPC_C_OPT_MQ_TIME_TO_REACH_QUEUE 7
#define RPC_C_OPT_MQ_TIME_TO_BE_RECEIVED 8
#define RPC_C_OPT_BINDING_NONCAUSAL      9
#define RPC_C_OPT_SECURITY_CALLBACK      10
#define RPC_C_OPT_UNIQUE_BINDING         11
#define RPC_C_OPT_CALL_TIMEOUT           12
#define RPC_C_OPT_DONT_LINGER            13
#define RPC_C_OPT_MAX_OPTIONS            14

#define RPC_C_MQ_EXPRESS                0  // Client: RPC_C_MQ_DELIVERY.
#define RPC_C_MQ_RECOVERABLE            1

#define RPC_C_MQ_JOURNAL_NONE           0  // Client: RPC_C_MQ_JOURNAL.
#define RPC_C_MQ_JOURNAL_DEADLETTER     1
#define RPC_C_MQ_JOURNAL_ALWAYS         2

// flags for RpcServerInqAuthClientEx
//
#define RPC_C_FULL_CERT_CHAIN 0x0001



#ifdef RPC_UNICODE_SUPPORTED
typedef struct _RPC_PROTSEQ_VECTORA
{
    unsigned int Count;
    unsigned char __RPC_FAR * Protseq[1];
} RPC_PROTSEQ_VECTORA;

typedef struct _RPC_PROTSEQ_VECTORW
{
    unsigned int Count;
    unsigned short __RPC_FAR * Protseq[1];
} RPC_PROTSEQ_VECTORW;

#ifdef UNICODE
#define RPC_PROTSEQ_VECTOR RPC_PROTSEQ_VECTORW
#else /* UNICODE */
#define RPC_PROTSEQ_VECTOR RPC_PROTSEQ_VECTORA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

typedef struct _RPC_PROTSEQ_VECTOR
{
    unsigned int Count;
    unsigned char __RPC_FAR * Protseq[1];
} RPC_PROTSEQ_VECTOR;

#endif /* RPC_UNICODE_SUPPORTED */
typedef struct _RPC_POLICY {
    unsigned int Length ;
    unsigned long EndpointFlags ;
    unsigned long NICFlags ;
    } RPC_POLICY,  __RPC_FAR *PRPC_POLICY ;

typedef void __RPC_USER
RPC_OBJECT_INQ_FN (
    IN UUID __RPC_FAR * ObjectUuid,
    OUT UUID __RPC_FAR * TypeUuid,
    OUT RPC_STATUS __RPC_FAR * Status
    );

typedef RPC_STATUS RPC_ENTRY
RPC_IF_CALLBACK_FN (
    IN RPC_IF_HANDLE  InterfaceUuid,
    IN void *Context
    ) ;

typedef void RPC_ENTRY
RPC_SECURITY_CALLBACK_FN (
    IN void *Context
    ) ;

#define RPC_MGR_EPV void

typedef struct
{
    unsigned int Count;
    unsigned long Stats[1];
} RPC_STATS_VECTOR;

#define RPC_C_STATS_CALLS_IN 0
#define RPC_C_STATS_CALLS_OUT 1
#define RPC_C_STATS_PKTS_IN 2
#define RPC_C_STATS_PKTS_OUT 3

typedef struct
{
  unsigned long Count;
  RPC_IF_ID __RPC_FAR * IfId[1];
} RPC_IF_ID_VECTOR;

/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingCopy (
    IN RPC_BINDING_HANDLE SourceBinding,
    OUT RPC_BINDING_HANDLE __RPC_FAR * DestinationBinding
    );

/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingFree (
    IN OUT RPC_BINDING_HANDLE __RPC_FAR * Binding
    );

/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingSetOption( IN RPC_BINDING_HANDLE hBinding,
                     IN unsigned long      option,
                     IN ULONG_PTR          optionValue );

/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqOption( IN  RPC_BINDING_HANDLE hBinding,
                     IN  unsigned long      option,
                     OUT ULONG_PTR         *pOptionValue );

/* client */

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingFromStringBindingA (
    __in RPC_CSTR StringBinding,
    __out RPC_BINDING_HANDLE __RPC_FAR *Binding
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingFromStringBindingW (
    __in RPC_WSTR StringBinding,
    __out RPC_BINDING_HANDLE __RPC_FAR *Binding
    );

#ifdef UNICODE
#define RpcBindingFromStringBinding RpcBindingFromStringBindingW
#else /* UNICODE */
#define RpcBindingFromStringBinding RpcBindingFromStringBindingA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingFromStringBinding (
    __in RPC_CSTR StringBinding,
    __out RPC_BINDING_HANDLE __RPC_FAR *Binding
    );

#endif /* RPC_UNICODE_SUPPORTED */

/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcSsGetContextBinding (
    IN void *ContextHandle,
    OUT RPC_BINDING_HANDLE __RPC_FAR * Binding
    );

/* client/server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqObject (
    IN RPC_BINDING_HANDLE Binding,
    OUT UUID __RPC_FAR * ObjectUuid
    );

/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingReset (
    IN RPC_BINDING_HANDLE Binding
    );

/* RpcBindingServerFromClient : UNSUPPORTED */
/* RpcBindingSetAuthInfo */

/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingSetObject (
    IN RPC_BINDING_HANDLE Binding,
    IN UUID __RPC_FAR * ObjectUuid
    );

/* client/server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtInqDefaultProtectLevel(
    IN  unsigned long AuthnSvc,
    OUT unsigned long __RPC_FAR *AuthnLevel
    );

/* client/server */

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingToStringBindingA (
    __in RPC_BINDING_HANDLE Binding,
    __deref_out RPC_CSTR __RPC_FAR *StringBinding
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingToStringBindingW (
    __in RPC_BINDING_HANDLE Binding,
    __deref_out RPC_WSTR __RPC_FAR *StringBinding
    );

#ifdef UNICODE
#define RpcBindingToStringBinding RpcBindingToStringBindingW
#else /* UNICODE */
#define RpcBindingToStringBinding RpcBindingToStringBindingA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingToStringBinding (
    __in RPC_BINDING_HANDLE Binding,
    __deref_out RPC_CSTR __RPC_FAR *StringBinding
    );

#endif /* RPC_UNICODE_SUPPORTED */

/* client/server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingVectorFree (
    IN OUT RPC_BINDING_VECTOR __RPC_FAR * __RPC_FAR * BindingVector
    );

/* client/server */

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcStringBindingComposeA (
    __in_opt RPC_CSTR ObjUuid,
    __in_opt RPC_CSTR Protseq,
    __in_opt RPC_CSTR NetworkAddr,
    __in_opt RPC_CSTR Endpoint,
    __in_opt RPC_CSTR Options,
    __deref_opt_out RPC_CSTR __RPC_FAR *StringBinding
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcStringBindingComposeW (
    __in_opt RPC_WSTR ObjUuid,
    __in_opt RPC_WSTR Protseq,
    __in_opt RPC_WSTR NetworkAddr,
    __in_opt RPC_WSTR Endpoint,
    __in_opt RPC_WSTR Options,
    __deref_opt_out RPC_WSTR __RPC_FAR *StringBinding
    );

#ifdef UNICODE
#define RpcStringBindingCompose RpcStringBindingComposeW
#else /* UNICODE */
#define RpcStringBindingCompose RpcStringBindingComposeA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcStringBindingCompose (
    __in_opt RPC_CSTR ObjUuid,
    __in_opt RPC_CSTR Protseq,
    __in_opt RPC_CSTR NetworkAddr,
    __in_opt RPC_CSTR Endpoint,
    __in_opt RPC_CSTR Options,
    __deref_opt_out RPC_CSTR __RPC_FAR *StringBinding
    );

#endif /* RPC_UNICODE_SUPPORTED */

/* client/server */

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcStringBindingParseA (
    __in RPC_CSTR StringBinding,
    __deref_opt_out RPC_CSTR __RPC_FAR *ObjUuid,
    __deref_opt_out RPC_CSTR __RPC_FAR *Protseq,
    __deref_opt_out RPC_CSTR __RPC_FAR *NetworkAddr,
    __deref_opt_out RPC_CSTR __RPC_FAR *Endpoint,
    __deref_opt_out RPC_CSTR __RPC_FAR *NetworkOptions
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcStringBindingParseW (
    __in RPC_WSTR StringBinding,
    __deref_opt_out RPC_WSTR __RPC_FAR *ObjUuid,
    __deref_opt_out RPC_WSTR __RPC_FAR *Protseq,
    __deref_opt_out RPC_WSTR __RPC_FAR *NetworkAddr,
    __deref_opt_out RPC_WSTR __RPC_FAR *Endpoint,
    __deref_opt_out RPC_WSTR __RPC_FAR *NetworkOptions
    );

#ifdef UNICODE
#define RpcStringBindingParse RpcStringBindingParseW
#else /* UNICODE */
#define RpcStringBindingParse RpcStringBindingParseA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcStringBindingParse (
    __in RPC_CSTR StringBinding,
    __deref_opt_out RPC_CSTR __RPC_FAR *ObjUuid,
    __deref_opt_out RPC_CSTR __RPC_FAR *Protseq,
    __deref_opt_out RPC_CSTR __RPC_FAR *NetworkAddr,
    __deref_opt_out RPC_CSTR __RPC_FAR *Endpoint,
    __deref_opt_out RPC_CSTR __RPC_FAR *NetworkOptions
    );

#endif /* RPC_UNICODE_SUPPORTED */

/* client/server */

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcStringFreeA (
    __deref RPC_CSTR __RPC_FAR *String
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcStringFreeW (
    __deref RPC_WSTR __RPC_FAR *String
    );

#ifdef UNICODE
#define RpcStringFree RpcStringFreeW
#else /* UNICODE */
#define RpcStringFree RpcStringFreeA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcStringFree (
    __deref RPC_CSTR __RPC_FAR *String
    );

#endif /* RPC_UNICODE_SUPPORTED */

/* client/server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcIfInqId (
    IN RPC_IF_HANDLE RpcIfHandle,
    OUT RPC_IF_ID __RPC_FAR * RpcIfId
    );

/* client/server */

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcNetworkIsProtseqValidA (
    __in RPC_CSTR Protseq
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcNetworkIsProtseqValidW (
    __in RPC_WSTR Protseq
    );

#ifdef UNICODE
#define RpcNetworkIsProtseqValid RpcNetworkIsProtseqValidW
#else /* UNICODE */
#define RpcNetworkIsProtseqValid RpcNetworkIsProtseqValidA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcNetworkIsProtseqValid (
    __in RPC_CSTR Protseq
    );

#endif /* RPC_UNICODE_SUPPORTED */

/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtInqComTimeout (
    IN RPC_BINDING_HANDLE Binding,
    OUT unsigned int __RPC_FAR * Timeout
    );

/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtSetComTimeout (
    IN RPC_BINDING_HANDLE Binding,
    IN unsigned int Timeout
    );

/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtSetCancelTimeout(
    long Timeout
    );


/* server */

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcNetworkInqProtseqsA (
    OUT RPC_PROTSEQ_VECTORA __RPC_FAR * __RPC_FAR * ProtseqVector
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcNetworkInqProtseqsW (
    OUT RPC_PROTSEQ_VECTORW __RPC_FAR * __RPC_FAR * ProtseqVector
    );

#ifdef UNICODE
#define RpcNetworkInqProtseqs RpcNetworkInqProtseqsW
#else /* UNICODE */
#define RpcNetworkInqProtseqs RpcNetworkInqProtseqsA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcNetworkInqProtseqs (
    OUT RPC_PROTSEQ_VECTOR __RPC_FAR * __RPC_FAR * ProtseqVector
    );

#endif /* RPC_UNICODE_SUPPORTED */

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcObjectInqType (
    IN UUID __RPC_FAR * ObjUuid,
    OUT UUID __RPC_FAR * TypeUuid OPTIONAL
    );

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcObjectSetInqFn (
    IN RPC_OBJECT_INQ_FN __RPC_FAR * InquiryFn
    );

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcObjectSetType (
    IN UUID __RPC_FAR * ObjUuid,
    IN UUID __RPC_FAR * TypeUuid OPTIONAL
    );

/* server */

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcProtseqVectorFreeA (
    IN OUT RPC_PROTSEQ_VECTORA __RPC_FAR * __RPC_FAR * ProtseqVector
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcProtseqVectorFreeW (
    IN OUT RPC_PROTSEQ_VECTORW __RPC_FAR * __RPC_FAR * ProtseqVector
    );

#ifdef UNICODE
#define RpcProtseqVectorFree RpcProtseqVectorFreeW
#else /* UNICODE */
#define RpcProtseqVectorFree RpcProtseqVectorFreeA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcProtseqVectorFree (
    IN OUT RPC_PROTSEQ_VECTOR __RPC_FAR * __RPC_FAR * ProtseqVector
    );

#endif /* RPC_UNICODE_SUPPORTED */

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerInqBindings (
    OUT RPC_BINDING_VECTOR __RPC_FAR * __RPC_FAR * BindingVector
    );

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerInqIf (
    IN RPC_IF_HANDLE IfSpec,
    IN UUID __RPC_FAR * MgrTypeUuid, OPTIONAL
    OUT RPC_MGR_EPV __RPC_FAR * __RPC_FAR * MgrEpv
    );

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerListen (
    IN unsigned int MinimumCallThreads,
    IN unsigned int MaxCalls,
    IN unsigned int DontWait
    );

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerRegisterIf (
    IN RPC_IF_HANDLE IfSpec,
    IN UUID __RPC_FAR * MgrTypeUuid OPTIONAL,
    IN RPC_MGR_EPV __RPC_FAR * MgrEpv OPTIONAL
    );

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerRegisterIfEx (
    IN RPC_IF_HANDLE IfSpec,
    IN UUID __RPC_FAR * MgrTypeUuid,
    IN RPC_MGR_EPV __RPC_FAR * MgrEpv,
    IN unsigned int Flags,
    IN unsigned int MaxCalls,
    IN RPC_IF_CALLBACK_FN __RPC_FAR *IfCallback
    );

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerRegisterIf2 (
    IN RPC_IF_HANDLE IfSpec,
    IN UUID * MgrTypeUuid,
    IN RPC_MGR_EPV * MgrEpv,
    IN unsigned int Flags,
    IN unsigned int MaxCalls,
    IN unsigned int MaxRpcSize,
    IN RPC_IF_CALLBACK_FN *IfCallbackFn
    );


/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUnregisterIf (
    IN RPC_IF_HANDLE IfSpec,
    IN UUID __RPC_FAR * MgrTypeUuid, OPTIONAL
    IN unsigned int WaitForCallsToComplete
    );

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUnregisterIfEx (
    IN RPC_IF_HANDLE IfSpec,
    IN UUID __RPC_FAR * MgrTypeUuid, OPTIONAL
    IN int RundownContextHandles
    );

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseAllProtseqs (
    IN unsigned int MaxCalls,
    IN void __RPC_FAR * SecurityDescriptor OPTIONAL
    );

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseAllProtseqsEx (
    IN unsigned int MaxCalls,
    IN void __RPC_FAR * SecurityDescriptor,
    IN PRPC_POLICY Policy
    );

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseAllProtseqsIf (
    IN unsigned int MaxCalls,
    IN RPC_IF_HANDLE IfSpec,
    IN void __RPC_FAR * SecurityDescriptor OPTIONAL
    );

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseAllProtseqsIfEx (
    IN unsigned int MaxCalls,
    IN RPC_IF_HANDLE IfSpec,
    IN void __RPC_FAR * SecurityDescriptor,
    IN PRPC_POLICY Policy
    );


/* server */

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseqA (
    __in RPC_CSTR Protseq,
    __in unsigned int MaxCalls,
    __in_opt void __RPC_FAR *SecurityDescriptor
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseqExA (
    __in RPC_CSTR Protseq,
    __in unsigned int MaxCalls,
    __in_opt void __RPC_FAR *SecurityDescriptor,
    __in PRPC_POLICY Policy
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseqW (
    __in RPC_WSTR Protseq,
    __in unsigned int MaxCalls,
    __in_opt void __RPC_FAR *SecurityDescriptor
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseqExW (
    __in RPC_WSTR Protseq,
    __in unsigned int MaxCalls,
    __in_opt void __RPC_FAR *SecurityDescriptor,
    __in PRPC_POLICY Policy
    );

#ifdef UNICODE
#define RpcServerUseProtseq RpcServerUseProtseqW
#define RpcServerUseProtseqEx RpcServerUseProtseqExW
#else /* UNICODE */
#define RpcServerUseProtseq RpcServerUseProtseqA
#define RpcServerUseProtseqEx RpcServerUseProtseqExA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseq (
    __in RPC_CSTR Protseq,
    __in unsigned int MaxCalls,
    __in_opt void __RPC_FAR *SecurityDescriptor
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseqEx (
    __in RPC_CSTR Protseq,
    __in unsigned int MaxCalls,
    __in_opt void __RPC_FAR *SecurityDescriptor,
    __in PRPC_POLICY Policy
    );

#endif /* RPC_UNICODE_SUPPORTED */

/* server */

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseqEpA (
    __in RPC_CSTR Protseq,
    __in unsigned int MaxCalls,
    __in RPC_CSTR Endpoint,
    __in_opt void __RPC_FAR *SecurityDescriptor
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseqEpExA (
    __in RPC_CSTR Protseq,
    __in unsigned int MaxCalls,
    __in RPC_CSTR Endpoint,
    __in_opt void __RPC_FAR *SecurityDescriptor,
    __in PRPC_POLICY Policy
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseqEpW (
    __in RPC_WSTR Protseq,
    __in unsigned int MaxCalls,
    __in RPC_WSTR Endpoint,
    __in_opt void __RPC_FAR *SecurityDescriptor
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseqEpExW (
    __in RPC_WSTR Protseq,
    __in unsigned int MaxCalls,
    __in RPC_WSTR Endpoint,
    __in_opt void __RPC_FAR *SecurityDescriptor,
    __in PRPC_POLICY Policy
    );

#ifdef UNICODE
#define RpcServerUseProtseqEp RpcServerUseProtseqEpW
#define RpcServerUseProtseqEpEx RpcServerUseProtseqEpExW
#else /* UNICODE */
#define RpcServerUseProtseqEp RpcServerUseProtseqEpA
#define RpcServerUseProtseqEpEx RpcServerUseProtseqEpExA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseqEp (
    __in RPC_CSTR Protseq,
    __in unsigned int MaxCalls,
    __in RPC_CSTR Endpoint,
    __in_opt void __RPC_FAR *SecurityDescriptor
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseqEpEx (
    __in RPC_CSTR Protseq,
    __in unsigned int MaxCalls,
    __in RPC_CSTR Endpoint,
    __in_opt void __RPC_FAR *SecurityDescriptor,
    __in PRPC_POLICY Policy
    );

#endif /* RPC_UNICODE_SUPPORTED */

/* server */

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseqIfA (
    __in RPC_CSTR Protseq,
    __in unsigned int MaxCalls,
    __in RPC_IF_HANDLE IfSpec,
    __in_opt void __RPC_FAR *SecurityDescriptor
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseqIfExA (
    __in RPC_CSTR Protseq,
    __in unsigned int MaxCalls,
    __in RPC_IF_HANDLE IfSpec,
    __in_opt void __RPC_FAR *SecurityDescriptor,
    __in PRPC_POLICY Policy
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseqIfW (
    __in RPC_WSTR Protseq,
    __in unsigned int MaxCalls,
    __in RPC_IF_HANDLE IfSpec,
    __in_opt void __RPC_FAR *SecurityDescriptor
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseqIfExW (
    __in RPC_WSTR Protseq,
    __in unsigned int MaxCalls,
    __in RPC_IF_HANDLE IfSpec,
    __in_opt void __RPC_FAR *SecurityDescriptor,
    __in PRPC_POLICY Policy
    );

#ifdef UNICODE
#define RpcServerUseProtseqIf RpcServerUseProtseqIfW
#define RpcServerUseProtseqIfEx RpcServerUseProtseqIfExW
#else /* UNICODE */
#define RpcServerUseProtseqIf RpcServerUseProtseqIfA
#define RpcServerUseProtseqIfEx RpcServerUseProtseqIfExA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseqIf (
    __in RPC_CSTR Protseq,
    __in unsigned int MaxCalls,
    __in RPC_IF_HANDLE IfSpec,
    __in_opt void __RPC_FAR *SecurityDescriptor
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerUseProtseqIfEx (
    __in RPC_CSTR Protseq,
    __in unsigned int MaxCalls,
    __in RPC_IF_HANDLE IfSpec,
    __in_opt void __RPC_FAR *SecurityDescriptor,
    __in PRPC_POLICY Policy
    );

#endif /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
void
RPC_ENTRY
RpcServerYield (
    );

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtStatsVectorFree (
    IN RPC_STATS_VECTOR ** StatsVector
    );

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtInqStats (
    IN RPC_BINDING_HANDLE Binding,
    OUT RPC_STATS_VECTOR ** Statistics
    );

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtIsServerListening (
    IN RPC_BINDING_HANDLE Binding
    );

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtStopServerListening (
    IN RPC_BINDING_HANDLE Binding
    );

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtWaitServerListen (
    void
    );

/* server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtSetServerStackSize (
    IN unsigned long ThreadStackSize
    );

/* server */
RPCRTAPI
void
RPC_ENTRY
RpcSsDontSerializeContext (
    void
    );

/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtEnableIdleCleanup (
    void
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtInqIfIds (
    IN RPC_BINDING_HANDLE Binding,
    OUT RPC_IF_ID_VECTOR __RPC_FAR * __RPC_FAR * IfIdVector
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcIfIdVectorFree (
    IN OUT RPC_IF_ID_VECTOR __RPC_FAR * __RPC_FAR * IfIdVector
    );

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtInqServerPrincNameA (
    __in RPC_BINDING_HANDLE Binding,
    __in unsigned long AuthnSvc,
    __deref_out RPC_CSTR __RPC_FAR *ServerPrincName
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtInqServerPrincNameW (
    __in RPC_BINDING_HANDLE Binding,
    __in unsigned long AuthnSvc,
    __deref_out RPC_WSTR __RPC_FAR *ServerPrincName
    );

#ifdef UNICODE
#define RpcMgmtInqServerPrincName RpcMgmtInqServerPrincNameW
#else /* UNICODE */
#define RpcMgmtInqServerPrincName RpcMgmtInqServerPrincNameA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtInqServerPrincName (
    __in RPC_BINDING_HANDLE Binding,
    __in unsigned long AuthnSvc,
    __deref_out RPC_CSTR __RPC_FAR *ServerPrincName
    );

#endif /* RPC_UNICODE_SUPPORTED */

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerInqDefaultPrincNameA (
    __in unsigned long AuthnSvc,
    __deref_out RPC_CSTR __RPC_FAR *PrincName
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerInqDefaultPrincNameW (
    __in unsigned long AuthnSvc,
    __deref_out RPC_WSTR __RPC_FAR *PrincName
    );

#ifdef UNICODE
#define RpcServerInqDefaultPrincName RpcServerInqDefaultPrincNameW
#else /* UNICODE */
#define RpcServerInqDefaultPrincName RpcServerInqDefaultPrincNameA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerInqDefaultPrincName (
    __in unsigned long AuthnSvc,
    __deref_out RPC_CSTR __RPC_FAR *PrincName
    );

#endif /* RPC_UNICODE_SUPPORTED */

/* client */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcEpResolveBinding (
    IN RPC_BINDING_HANDLE Binding,
    IN RPC_IF_HANDLE IfSpec
    );

/* client */

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcNsBindingInqEntryNameA (
    __in RPC_BINDING_HANDLE Binding,
    __in unsigned long EntryNameSyntax,
    __deref_out RPC_CSTR __RPC_FAR *EntryName
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcNsBindingInqEntryNameW (
    __in RPC_BINDING_HANDLE Binding,
    __in unsigned long EntryNameSyntax,
    __deref_out RPC_WSTR __RPC_FAR *EntryName
    );

#ifdef UNICODE
#define RpcNsBindingInqEntryName RpcNsBindingInqEntryNameW
#else /* UNICODE */
#define RpcNsBindingInqEntryName RpcNsBindingInqEntryNameA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcNsBindingInqEntryName (
    __in RPC_BINDING_HANDLE Binding,
    __in unsigned long EntryNameSyntax,
    __deref_out RPC_CSTR __RPC_FAR *EntryName
    );

#endif /* RPC_UNICODE_SUPPORTED */

typedef void __RPC_FAR * RPC_AUTH_IDENTITY_HANDLE;
typedef void __RPC_FAR * RPC_AUTHZ_HANDLE;

#define RPC_C_AUTHN_LEVEL_DEFAULT       0
#define RPC_C_AUTHN_LEVEL_NONE          1
#define RPC_C_AUTHN_LEVEL_CONNECT       2
#define RPC_C_AUTHN_LEVEL_CALL          3
#define RPC_C_AUTHN_LEVEL_PKT           4
#define RPC_C_AUTHN_LEVEL_PKT_INTEGRITY 5
#define RPC_C_AUTHN_LEVEL_PKT_PRIVACY   6

#define RPC_C_IMP_LEVEL_DEFAULT      0
#define RPC_C_IMP_LEVEL_ANONYMOUS    1
#define RPC_C_IMP_LEVEL_IDENTIFY     2
#define RPC_C_IMP_LEVEL_IMPERSONATE  3
#define RPC_C_IMP_LEVEL_DELEGATE     4

#define RPC_C_QOS_IDENTITY_STATIC    0
#define RPC_C_QOS_IDENTITY_DYNAMIC   1

#define RPC_C_QOS_CAPABILITIES_DEFAULT                        0x0
#define RPC_C_QOS_CAPABILITIES_MUTUAL_AUTH                    0x1
#define RPC_C_QOS_CAPABILITIES_MAKE_FULLSIC                   0x2
#define RPC_C_QOS_CAPABILITIES_ANY_AUTHORITY                  0x4
#define RPC_C_QOS_CAPABILITIES_IGNORE_DELEGATE_FAILURE        0x8
#define RPC_C_QOS_CAPABILITIES_LOCAL_MA_HINT                 0x10

#define RPC_C_PROTECT_LEVEL_DEFAULT       (RPC_C_AUTHN_LEVEL_DEFAULT)
#define RPC_C_PROTECT_LEVEL_NONE          (RPC_C_AUTHN_LEVEL_NONE)
#define RPC_C_PROTECT_LEVEL_CONNECT       (RPC_C_AUTHN_LEVEL_CONNECT)
#define RPC_C_PROTECT_LEVEL_CALL          (RPC_C_AUTHN_LEVEL_CALL)
#define RPC_C_PROTECT_LEVEL_PKT           (RPC_C_AUTHN_LEVEL_PKT)
#define RPC_C_PROTECT_LEVEL_PKT_INTEGRITY (RPC_C_AUTHN_LEVEL_PKT_INTEGRITY)
#define RPC_C_PROTECT_LEVEL_PKT_PRIVACY   (RPC_C_AUTHN_LEVEL_PKT_PRIVACY)

#define RPC_C_AUTHN_NONE          0
#define RPC_C_AUTHN_DCE_PRIVATE   1
#define RPC_C_AUTHN_DCE_PUBLIC    2
#define RPC_C_AUTHN_DEC_PUBLIC    4
#define RPC_C_AUTHN_GSS_NEGOTIATE 9
#define RPC_C_AUTHN_WINNT        10
#define RPC_C_AUTHN_GSS_SCHANNEL 14
#define RPC_C_AUTHN_GSS_KERBEROS 16
#define RPC_C_AUTHN_DPA          17
#define RPC_C_AUTHN_MSN          18
#define RPC_C_AUTHN_DIGEST       21
#define RPC_C_AUTHN_MQ          100
#define RPC_C_AUTHN_DEFAULT     0xFFFFFFFFL

#define RPC_C_NO_CREDENTIALS ((RPC_AUTH_IDENTITY_HANDLE) MAXUINT_PTR)

#define RPC_C_SECURITY_QOS_VERSION  1L
#define RPC_C_SECURITY_QOS_VERSION_1  1L

typedef struct _RPC_SECURITY_QOS {
  unsigned long Version;
  unsigned long Capabilities;
  unsigned long IdentityTracking;
  unsigned long ImpersonationType;
} RPC_SECURITY_QOS, *PRPC_SECURITY_QOS;

#ifndef _AUTH_IDENTITY_DEFINED
#define _AUTH_IDENTITY_DEFINED

#define SEC_WINNT_AUTH_IDENTITY_ANSI    0x1
#define SEC_WINNT_AUTH_IDENTITY_UNICODE 0x2

typedef struct _SEC_WINNT_AUTH_IDENTITY_W {
  unsigned short __RPC_FAR *User;
  unsigned long UserLength;
  unsigned short __RPC_FAR *Domain;
  unsigned long DomainLength;
  unsigned short __RPC_FAR *Password;
  unsigned long PasswordLength;
  unsigned long Flags;
} SEC_WINNT_AUTH_IDENTITY_W, *PSEC_WINNT_AUTH_IDENTITY_W;

typedef struct _SEC_WINNT_AUTH_IDENTITY_A {
  unsigned char __RPC_FAR *User;
  unsigned long UserLength;
  unsigned char __RPC_FAR *Domain;
  unsigned long DomainLength;
  unsigned char __RPC_FAR *Password;
  unsigned long PasswordLength;
  unsigned long Flags;
} SEC_WINNT_AUTH_IDENTITY_A, *PSEC_WINNT_AUTH_IDENTITY_A;

#ifdef UNICODE
#define SEC_WINNT_AUTH_IDENTITY SEC_WINNT_AUTH_IDENTITY_W
#define PSEC_WINNT_AUTH_IDENTITY PSEC_WINNT_AUTH_IDENTITY_W
#define _SEC_WINNT_AUTH_IDENTITY _SEC_WINNT_AUTH_IDENTITY_W
#else // UNICODE
#define SEC_WINNT_AUTH_IDENTITY SEC_WINNT_AUTH_IDENTITY_A
#define PSEC_WINNT_AUTH_IDENTITY PSEC_WINNT_AUTH_IDENTITY_A
#define _SEC_WINNT_AUTH_IDENTITY _SEC_WINNT_AUTH_IDENTITY_A
#endif // UNICODE

#define RPC_C_SECURITY_QOS_VERSION_2 2L

#define RPC_C_AUTHN_INFO_TYPE_HTTP                  1

#define RPC_C_HTTP_AUTHN_TARGET_SERVER              1
#define RPC_C_HTTP_AUTHN_TARGET_PROXY               2

#define RPC_C_HTTP_AUTHN_SCHEME_BASIC      0x00000001
#define RPC_C_HTTP_AUTHN_SCHEME_NTLM       0x00000002
#define RPC_C_HTTP_AUTHN_SCHEME_PASSPORT   0x00000004
#define RPC_C_HTTP_AUTHN_SCHEME_DIGEST     0x00000008
#define RPC_C_HTTP_AUTHN_SCHEME_NEGOTIATE  0x00000010
#define RPC_C_HTTP_AUTHN_SCHEME_CERT       0x00010000
// 0x00020000 & 0x00040000 are reserved

#define RPC_C_HTTP_FLAG_USE_SSL                     1
#define RPC_C_HTTP_FLAG_USE_FIRST_AUTH_SCHEME       2
#define RPC_C_HTTP_FLAG_IGNORE_CERT_CN_INVALID      8

typedef struct _RPC_HTTP_TRANSPORT_CREDENTIALS_W
{
    SEC_WINNT_AUTH_IDENTITY_W *TransportCredentials;
    unsigned long Flags;
    unsigned long AuthenticationTarget;
    unsigned long NumberOfAuthnSchemes;
    unsigned long *AuthnSchemes;
    unsigned short __RPC_FAR *ServerCertificateSubject;
} RPC_HTTP_TRANSPORT_CREDENTIALS_W, *PRPC_HTTP_TRANSPORT_CREDENTIALS_W;

typedef struct _RPC_HTTP_TRANSPORT_CREDENTIALS_A
{
    SEC_WINNT_AUTH_IDENTITY_A *TransportCredentials;
    unsigned long Flags;
    unsigned long AuthenticationTarget;
    unsigned long NumberOfAuthnSchemes;
    unsigned long *AuthnSchemes;
    unsigned char __RPC_FAR *ServerCertificateSubject;
} RPC_HTTP_TRANSPORT_CREDENTIALS_A, *PRPC_HTTP_TRANSPORT_CREDENTIALS_A;

typedef struct _RPC_SECURITY_QOS_V2_W {
  unsigned long Version;
  unsigned long Capabilities;
  unsigned long IdentityTracking;
  unsigned long ImpersonationType;
  unsigned long AdditionalSecurityInfoType;
  union 
      {
      RPC_HTTP_TRANSPORT_CREDENTIALS_W *HttpCredentials;
      } u;
} RPC_SECURITY_QOS_V2_W, *PRPC_SECURITY_QOS_V2_W;

typedef struct _RPC_SECURITY_QOS_V2_A {
  unsigned long Version;
  unsigned long Capabilities;
  unsigned long IdentityTracking;
  unsigned long ImpersonationType;
  unsigned long AdditionalSecurityInfoType;
  union 
      {
      RPC_HTTP_TRANSPORT_CREDENTIALS_A *HttpCredentials;
      } u;
} RPC_SECURITY_QOS_V2_A, *PRPC_SECURITY_QOS_V2_A;

#define RPC_C_SECURITY_QOS_VERSION_3 3L

typedef struct _RPC_SECURITY_QOS_V3_W {
  unsigned long Version;
  unsigned long Capabilities;
  unsigned long IdentityTracking;
  unsigned long ImpersonationType;
  unsigned long AdditionalSecurityInfoType;
  union 
      {
      RPC_HTTP_TRANSPORT_CREDENTIALS_W *HttpCredentials;
      } u;
  void *Sid;
} RPC_SECURITY_QOS_V3_W, *PRPC_SECURITY_QOS_V3_W;

typedef struct _RPC_SECURITY_QOS_V3_A {
  unsigned long Version;
  unsigned long Capabilities;
  unsigned long IdentityTracking;
  unsigned long ImpersonationType;
  unsigned long AdditionalSecurityInfoType;
  union 
      {
      RPC_HTTP_TRANSPORT_CREDENTIALS_A *HttpCredentials;
      } u;
  void *Sid;
} RPC_SECURITY_QOS_V3_A, *PRPC_SECURITY_QOS_V3_A;

#endif //_AUTH_IDENTITY_DEFINED

#ifdef UNICODE

#define RPC_SECURITY_QOS_V2 RPC_SECURITY_QOS_V2_W
#define PRPC_SECURITY_QOS_V2 PRPC_SECURITY_QOS_V2_W
#define _RPC_SECURITY_QOS_V2 _RPC_SECURITY_QOS_V2_W

#define RPC_HTTP_TRANSPORT_CREDENTIALS RPC_HTTP_TRANSPORT_CREDENTIALS_W
#define PRPC_HTTP_TRANSPORT_CREDENTIALS PRPC_HTTP_TRANSPORT_CREDENTIALS_W
#define _RPC_HTTP_TRANSPORT_CREDENTIALS _RPC_HTTP_TRANSPORT_CREDENTIALS_W

#define RPC_SECURITY_QOS_V3 RPC_SECURITY_QOS_V3_W
#define PRPC_SECURITY_QOS_V3 PRPC_SECURITY_QOS_V3_W
#define _RPC_SECURITY_QOS_V3 _RPC_SECURITY_QOS_V3_W

#else // UNICODE

#define RPC_SECURITY_QOS_V2 RPC_SECURITY_QOS_V2_A
#define PRPC_SECURITY_QOS_V2 PRPC_SECURITY_QOS_V2_A
#define _RPC_SECURITY_QOS_V2 _RPC_SECURITY_QOS_V2_A

#define RPC_HTTP_TRANSPORT_CREDENTIALS RPC_HTTP_TRANSPORT_CREDENTIALS_A
#define PRPC_HTTP_TRANSPORT_CREDENTIALS PRPC_HTTP_TRANSPORT_CREDENTIALS_A
#define _RPC_HTTP_TRANSPORT_CREDENTIALS _RPC_HTTP_TRANSPORT_CREDENTIALS_A

#define RPC_SECURITY_QOS_V3 RPC_SECURITY_QOS_V3_A
#define PRPC_SECURITY_QOS_V3 PRPC_SECURITY_QOS_V3_A
#define _RPC_SECURITY_QOS_V3 _RPC_SECURITY_QOS_V3_A

#endif // UNICODE

typedef enum _RPC_HTTP_REDIRECTOR_STAGE
{
    RPCHTTP_RS_REDIRECT = 1,
    RPCHTTP_RS_ACCESS_1,
    RPCHTTP_RS_SESSION,
    RPCHTTP_RS_ACCESS_2,
    RPCHTTP_RS_INTERFACE
} RPC_HTTP_REDIRECTOR_STAGE;

typedef RPC_STATUS
(__RPC_USER * RPC_NEW_HTTP_PROXY_CHANNEL) (
    IN RPC_HTTP_REDIRECTOR_STAGE RedirectorStage,
    IN unsigned short __RPC_FAR *ServerName,
    IN unsigned short __RPC_FAR *ServerPort,
    IN OPTIONAL unsigned short __RPC_FAR *RemoteUser,
    IN OPTIONAL unsigned short __RPC_FAR *AuthType,
    IN OPTIONAL void __RPC_FAR *ResourceUuid,
    IN OPTIONAL void __RPC_FAR *Metadata,
    IN OPTIONAL void __RPC_FAR *SessionId,
    IN OPTIONAL void __RPC_FAR *Interface,
    IN OPTIONAL void __RPC_FAR *Reserved,
    IN unsigned long Flags,
    OUT OPTIONAL unsigned short __RPC_FAR **NewServerName,
    OUT OPTIONAL unsigned short __RPC_FAR **NewServerPort
    );

typedef void
(__RPC_USER * RPC_HTTP_PROXY_FREE_STRING) (
    IN unsigned short __RPC_FAR *String
    );

#define RPC_C_AUTHZ_NONE    0
#define RPC_C_AUTHZ_NAME    1
#define RPC_C_AUTHZ_DCE     2
#define RPC_C_AUTHZ_DEFAULT 0xffffffff

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcImpersonateClient (
    IN RPC_BINDING_HANDLE BindingHandle OPTIONAL
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcRevertToSelfEx (
    IN RPC_BINDING_HANDLE BindingHandle OPTIONAL
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcRevertToSelf (
    );

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqAuthClientA (
    __in_opt RPC_BINDING_HANDLE ClientBinding,
    __out RPC_AUTHZ_HANDLE __RPC_FAR *Privs,
    __deref_opt_out RPC_CSTR __RPC_FAR *ServerPrincName,
    __out_opt unsigned long __RPC_FAR *AuthnLevel, 
    __out_opt unsigned long __RPC_FAR *AuthnSvc,
    __out_opt unsigned long __RPC_FAR *AuthzSvc
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqAuthClientW (
    __in_opt RPC_BINDING_HANDLE ClientBinding,
    __out RPC_AUTHZ_HANDLE __RPC_FAR *Privs,
    __deref_opt_out RPC_WSTR __RPC_FAR *ServerPrincName,
    __out_opt unsigned long __RPC_FAR *AuthnLevel,
    __out_opt unsigned long __RPC_FAR *AuthnSvc,
    __out_opt unsigned long __RPC_FAR *AuthzSvc
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqAuthClientExA (
    __in_opt RPC_BINDING_HANDLE ClientBinding,
    __out RPC_AUTHZ_HANDLE __RPC_FAR *Privs,
    __deref_opt_out RPC_CSTR __RPC_FAR *ServerPrincName,
    __out_opt unsigned long __RPC_FAR *AuthnLevel,
    __out_opt unsigned long __RPC_FAR *AuthnSvc,
    __out_opt unsigned long __RPC_FAR *AuthzSvc,
    IN  unsigned long             Flags
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqAuthClientExW (
    __in_opt RPC_BINDING_HANDLE ClientBinding,
    __out RPC_AUTHZ_HANDLE __RPC_FAR *Privs,
    __deref_opt_out RPC_WSTR __RPC_FAR *ServerPrincName,
    __out_opt unsigned long __RPC_FAR *AuthnLevel,
    __out_opt unsigned long __RPC_FAR *AuthnSvc,
    __out_opt unsigned long __RPC_FAR *AuthzSvc,
    IN  unsigned long             Flags
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqAuthInfoA (
    __in RPC_BINDING_HANDLE Binding,
    __deref_opt_out RPC_CSTR __RPC_FAR *ServerPrincName,
    __out_opt unsigned long __RPC_FAR *AuthnLevel,
    __out_opt unsigned long __RPC_FAR *AuthnSvc,
    __out_opt RPC_AUTH_IDENTITY_HANDLE __RPC_FAR *AuthIdentity,
    __out_opt unsigned long __RPC_FAR *AuthzSvc
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqAuthInfoW (
    __in RPC_BINDING_HANDLE Binding,
    __deref_opt_out RPC_WSTR __RPC_FAR *ServerPrincName,
    __out_opt unsigned long __RPC_FAR *AuthnLevel,
    __out_opt unsigned long __RPC_FAR *AuthnSvc,
    __out_opt RPC_AUTH_IDENTITY_HANDLE __RPC_FAR *AuthIdentity,
    __out_opt unsigned long __RPC_FAR *AuthzSvc
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingSetAuthInfoA (
    __in RPC_BINDING_HANDLE Binding,
    __in RPC_CSTR ServerPrincName,
    __in unsigned long AuthnLevel,
    __in unsigned long AuthnSvc,
    __in_opt RPC_AUTH_IDENTITY_HANDLE AuthIdentity,
    __in unsigned long AuthzSvc
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingSetAuthInfoExA (
    __in RPC_BINDING_HANDLE Binding,
    __in RPC_CSTR ServerPrincName,
    __in unsigned long AuthnLevel,
    __in unsigned long AuthnSvc,
    __in_opt RPC_AUTH_IDENTITY_HANDLE AuthIdentity,
    __in unsigned long AuthzSvc,
    __in_opt RPC_SECURITY_QOS *SecurityQos
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingSetAuthInfoW (
    __in RPC_BINDING_HANDLE Binding,
    __in RPC_WSTR ServerPrincName,
    __in unsigned long AuthnLevel,
    __in unsigned long AuthnSvc,
    __in_opt RPC_AUTH_IDENTITY_HANDLE AuthIdentity,
    __in unsigned long AuthzSvc
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingSetAuthInfoExW (
    __in RPC_BINDING_HANDLE Binding,
    __in RPC_WSTR ServerPrincName,
    __in unsigned long AuthnLevel,
    __in unsigned long AuthnSvc,
    __in_opt RPC_AUTH_IDENTITY_HANDLE AuthIdentity,
    __in unsigned long AuthzSvc,
    __in_opt RPC_SECURITY_QOS *SecurityQOS
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqAuthInfoExA (
    __in RPC_BINDING_HANDLE Binding,
    __deref_opt_out RPC_CSTR __RPC_FAR *ServerPrincName,
    __out_opt unsigned long __RPC_FAR *AuthnLevel,
    __out_opt unsigned long __RPC_FAR *AuthnSvc,
    __out_opt RPC_AUTH_IDENTITY_HANDLE __RPC_FAR *AuthIdentity,
    __out_opt unsigned long __RPC_FAR *AuthzSvc,
    __in unsigned long RpcQosVersion,
    __out_opt RPC_SECURITY_QOS *SecurityQOS
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqAuthInfoExW (
    __in RPC_BINDING_HANDLE Binding,
    __deref_opt_out RPC_WSTR __RPC_FAR *ServerPrincName, 
    __out_opt unsigned long __RPC_FAR *AuthnLevel,
    __out_opt unsigned long __RPC_FAR *AuthnSvc,
    __out_opt RPC_AUTH_IDENTITY_HANDLE __RPC_FAR *AuthIdentity,
    __out_opt unsigned long __RPC_FAR *AuthzSvc,
    __in unsigned long RpcQosVersion,
    __out_opt RPC_SECURITY_QOS *SecurityQOS
    );

typedef void
(__RPC_USER * RPC_AUTH_KEY_RETRIEVAL_FN) (
    IN void __RPC_FAR * Arg,
    IN unsigned short __RPC_FAR * ServerPrincName,
    IN unsigned long KeyVer,
    OUT void __RPC_FAR * __RPC_FAR * Key,
    OUT RPC_STATUS __RPC_FAR * Status
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerRegisterAuthInfoA (
    __in RPC_CSTR ServerPrincName,
    __in unsigned long AuthnSvc,
    __in_opt RPC_AUTH_KEY_RETRIEVAL_FN GetKeyFn,
    __in_opt void __RPC_FAR *Arg
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerRegisterAuthInfoW (
    __in RPC_WSTR ServerPrincName,
    __in unsigned long AuthnSvc,
    __in_opt RPC_AUTH_KEY_RETRIEVAL_FN GetKeyFn,
    __in_opt void __RPC_FAR *Arg
    );

#ifdef UNICODE
#define RpcBindingInqAuthClient RpcBindingInqAuthClientW
#define RpcBindingInqAuthClientEx RpcBindingInqAuthClientExW
#define RpcBindingInqAuthInfo RpcBindingInqAuthInfoW
#define RpcBindingSetAuthInfo RpcBindingSetAuthInfoW
#define RpcServerRegisterAuthInfo RpcServerRegisterAuthInfoW
#define RpcBindingInqAuthInfoEx RpcBindingInqAuthInfoExW
#define RpcBindingSetAuthInfoEx RpcBindingSetAuthInfoExW
#else /* UNICODE */
#define RpcBindingInqAuthClient RpcBindingInqAuthClientA
#define RpcBindingInqAuthClientEx RpcBindingInqAuthClientExA
#define RpcBindingInqAuthInfo RpcBindingInqAuthInfoA
#define RpcBindingSetAuthInfo RpcBindingSetAuthInfoA
#define RpcServerRegisterAuthInfo RpcServerRegisterAuthInfoA
#define RpcBindingInqAuthInfoEx RpcBindingInqAuthInfoExA
#define RpcBindingSetAuthInfoEx RpcBindingSetAuthInfoExA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqAuthClient (
    __in_opt RPC_BINDING_HANDLE ClientBinding,
    __out RPC_AUTHZ_HANDLE __RPC_FAR *Privs,
    __deref_opt_out RPC_CSTR __RPC_FAR *ServerPrincName,
    __out_opt unsigned long __RPC_FAR *AuthnLevel,
    __out_opt unsigned long __RPC_FAR *AuthnSvc,
    __out_opt unsigned long __RPC_FAR *AuthzSvc
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqAuthClientEx (
    __in_opt RPC_BINDING_HANDLE ClientBinding,
    __out RPC_AUTHZ_HANDLE __RPC_FAR *Privs,
    __deref_opt_out RPC_CSTR __RPC_FAR *ServerPrincName,
    __out_opt unsigned long __RPC_FAR *AuthnLevel,
    __out_opt unsigned long __RPC_FAR *AuthnSvc,
    __out_opt unsigned long __RPC_FAR *AuthzSvc,
    __in unsigned long Flags
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingInqAuthInfo (
    __in RPC_BINDING_HANDLE Binding,
    __deref_opt_out RPC_CSTR __RPC_FAR *ServerPrincName,
    __out_opt unsigned long __RPC_FAR *AuthnLevel,
    __out_opt unsigned long __RPC_FAR *AuthnSvc,
    __out_opt RPC_AUTH_IDENTITY_HANDLE __RPC_FAR *AuthIdentity,
    __out_opt unsigned long __RPC_FAR *AuthzSvc
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingSetAuthInfo (
    __in RPC_BINDING_HANDLE Binding,
    __in RPC_CSTR ServerPrincName,
    __in unsigned long AuthnLevel,
    __in unsigned long AuthnSvc,
    __in_opt RPC_AUTH_IDENTITY_HANDLE AuthIdentity,
    __in unsigned long AuthzSvc
    );

typedef void
(__RPC_USER * RPC_AUTH_KEY_RETRIEVAL_FN) (
    IN void __RPC_FAR * Arg,
    IN unsigned char __RPC_FAR * ServerPrincName,
    IN unsigned long KeyVer,
    OUT void __RPC_FAR * __RPC_FAR * Key,
    OUT RPC_STATUS __RPC_FAR * Status
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerRegisterAuthInfo (
    __in RPC_CSTR ServerPrincName,
    __in unsigned long AuthnSvc,
    __in_opt RPC_AUTH_KEY_RETRIEVAL_FN GetKeyFn,
    __in_opt void __RPC_FAR *Arg
    );

#endif /* RPC_UNICODE_SUPPORTED */

#if !defined(_M_IA64)
typedef struct {
    unsigned char __RPC_FAR * UserName;
    unsigned char __RPC_FAR * ComputerName;
    unsigned short Privilege;
    unsigned long AuthFlags;
} RPC_CLIENT_INFORMATION1, __RPC_FAR * PRPC_CLIENT_INFORMATION1;
#endif

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcBindingServerFromClient (
    IN RPC_BINDING_HANDLE ClientBinding,
    OUT RPC_BINDING_HANDLE __RPC_FAR * ServerBinding
    );

RPCRTAPI
DECLSPEC_NORETURN
void
RPC_ENTRY
RpcRaiseException (
    IN RPC_STATUS exception
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcTestCancel(
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcServerTestCancel (
    IN RPC_BINDING_HANDLE BindingHandle OPTIONAL
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcCancelThread(
    IN void * Thread
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcCancelThreadEx(
    IN void * Thread,
    IN long Timeout
    );

/* client/server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
UuidCreate (
    OUT UUID __RPC_FAR * Uuid
    );

/* client/server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
UuidCreateSequential (
    OUT UUID __RPC_FAR * Uuid
    );

#ifdef RPC_UNICODE_SUPPORTED

/* client/server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
UuidToStringA (
    __in UUID __RPC_FAR *Uuid,
    __deref_out RPC_CSTR __RPC_FAR *StringUuid
    );

/* client/server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
UuidFromStringA (
    __in RPC_CSTR StringUuid,
    __out UUID __RPC_FAR *Uuid
    );

/* client/server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
UuidToStringW (
    __in UUID __RPC_FAR *Uuid,
    __deref_out RPC_WSTR __RPC_FAR *StringUuid
    );

/* client/server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
UuidFromStringW (
    __in_opt RPC_WSTR StringUuid,
    __out UUID __RPC_FAR *Uuid
    );

#ifdef UNICODE
#define UuidFromString UuidFromStringW
#define UuidToString UuidToStringW
#else /* UNICODE */
#define UuidFromString UuidFromStringA
#define UuidToString UuidToStringA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

/* client/server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
UuidToString (
    __in UUID __RPC_FAR *Uuid,
    __deref_out RPC_CSTR __RPC_FAR *StringUuid
    );

/* client/server */
RPCRTAPI
RPC_STATUS
RPC_ENTRY
UuidFromString (
    __in_opt RPC_CSTR StringUuid,
    __out UUID __RPC_FAR *Uuid
    );

#endif /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
signed int
RPC_ENTRY
UuidCompare (
    IN UUID __RPC_FAR * Uuid1,
    IN UUID __RPC_FAR * Uuid2,
    OUT RPC_STATUS __RPC_FAR * Status
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
UuidCreateNil (
    OUT UUID __RPC_FAR * NilUuid
    );

RPCRTAPI
int
RPC_ENTRY
UuidEqual (
    IN UUID __RPC_FAR * Uuid1,
    IN UUID __RPC_FAR * Uuid2,
    OUT RPC_STATUS __RPC_FAR * Status
    );

RPCRTAPI
unsigned short
RPC_ENTRY
UuidHash (
    IN UUID __RPC_FAR * Uuid,
    OUT RPC_STATUS __RPC_FAR * Status
    );

RPCRTAPI
int
RPC_ENTRY
UuidIsNil (
    IN UUID __RPC_FAR * Uuid,
    OUT RPC_STATUS __RPC_FAR * Status
    );

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcEpRegisterNoReplaceA (
    __in RPC_IF_HANDLE IfSpec,
    __in RPC_BINDING_VECTOR *BindingVector,
    __in_opt UUID_VECTOR *UuidVector,
    __in_opt RPC_CSTR Annotation
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcEpRegisterNoReplaceW (
    __in RPC_IF_HANDLE IfSpec,
    __in RPC_BINDING_VECTOR *BindingVector,
    __in_opt UUID_VECTOR *UuidVector,
    __in_opt RPC_WSTR Annotation
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcEpRegisterA (
    __in RPC_IF_HANDLE IfSpec,
    __in RPC_BINDING_VECTOR *BindingVector,
    __in_opt UUID_VECTOR *UuidVector,
    __in_opt RPC_CSTR Annotation
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcEpRegisterW (
    __in RPC_IF_HANDLE IfSpec,
    __in RPC_BINDING_VECTOR *BindingVector,
    __in_opt UUID_VECTOR *UuidVector,
    __in_opt RPC_WSTR Annotation
    );

#ifdef UNICODE
#define RpcEpRegisterNoReplace RpcEpRegisterNoReplaceW
#define RpcEpRegister RpcEpRegisterW
#else /* UNICODE */
#define RpcEpRegisterNoReplace RpcEpRegisterNoReplaceA
#define RpcEpRegister RpcEpRegisterA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcEpRegisterNoReplace (
    __in RPC_IF_HANDLE IfSpec,
    __in RPC_BINDING_VECTOR *BindingVector,
    __in_opt UUID_VECTOR *UuidVector,
    __in_opt RPC_CSTR Annotation
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcEpRegister (
    __in RPC_IF_HANDLE IfSpec,
    __in RPC_BINDING_VECTOR *BindingVector,
    __in_opt UUID_VECTOR *UuidVector,
    __in_opt RPC_CSTR Annotation
    );

#endif /* RPC_UNICODE_SUPPORTED */


RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcEpUnregister(
    IN RPC_IF_HANDLE IfSpec,
    IN RPC_BINDING_VECTOR *BindingVector,
    IN UUID_VECTOR *UuidVector
    );

#define DCE_C_ERROR_STRING_LEN 256

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
DceErrorInqTextA (
    __in RPC_STATUS RpcStatus,
    __out_ecount(DCE_C_ERROR_STRING_LEN) RPC_CSTR ErrorText
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
DceErrorInqTextW (
    __in RPC_STATUS RpcStatus,
    __out_ecount(DCE_C_ERROR_STRING_LEN) RPC_WSTR ErrorText
    );

#ifdef UNICODE
#define DceErrorInqText DceErrorInqTextW
#else /* UNICODE */
#define DceErrorInqText DceErrorInqTextA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
DceErrorInqText (
    __in RPC_STATUS RpcStatus,
    __out_ecount(DCE_C_ERROR_STRING_LEN) RPC_CSTR ErrorText
    );

#endif /* RPC_UNICODE_SUPPORTED */

typedef I_RPC_HANDLE * RPC_EP_INQ_HANDLE;

#define  RPC_C_EP_ALL_ELTS        0
#define  RPC_C_EP_MATCH_BY_IF     1
#define  RPC_C_EP_MATCH_BY_OBJ    2
#define  RPC_C_EP_MATCH_BY_BOTH   3

#define  RPC_C_VERS_ALL           1
#define  RPC_C_VERS_COMPATIBLE    2
#define  RPC_C_VERS_EXACT         3
#define  RPC_C_VERS_MAJOR_ONLY    4
#define  RPC_C_VERS_UPTO          5

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtEpEltInqBegin (
    IN RPC_BINDING_HANDLE EpBinding OPTIONAL,
    IN unsigned long InquiryType,
    IN RPC_IF_ID __RPC_FAR * IfId OPTIONAL,
    IN unsigned long VersOption OPTIONAL,
    IN UUID __RPC_FAR * ObjectUuid OPTIONAL,
    OUT RPC_EP_INQ_HANDLE __RPC_FAR * InquiryContext
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtEpEltInqDone (
    IN OUT RPC_EP_INQ_HANDLE __RPC_FAR * InquiryContext
    );

#ifdef RPC_UNICODE_SUPPORTED

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtEpEltInqNextA (
    __in RPC_EP_INQ_HANDLE InquiryContext,
    __out RPC_IF_ID __RPC_FAR *IfId,
    __out_opt RPC_BINDING_HANDLE __RPC_FAR *Binding,
    __out_opt UUID __RPC_FAR *ObjectUuid,
    __deref_opt_out RPC_CSTR __RPC_FAR *Annotation
    );

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtEpEltInqNextW (
    __in RPC_EP_INQ_HANDLE InquiryContext,
    __out RPC_IF_ID __RPC_FAR *IfId,
    __out_opt RPC_BINDING_HANDLE __RPC_FAR *Binding,
    __out_opt UUID __RPC_FAR *ObjectUuid,
    __deref_opt_out RPC_WSTR __RPC_FAR *Annotation
    );

#ifdef UNICODE
#define RpcMgmtEpEltInqNext RpcMgmtEpEltInqNextW
#else /* UNICODE */
#define RpcMgmtEpEltInqNext RpcMgmtEpEltInqNextA
#endif /* UNICODE */

#else /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtEpEltInqNext (
    __in RPC_EP_INQ_HANDLE InquiryContext,
    __out RPC_IF_ID __RPC_FAR *IfId,
    __out_opt RPC_BINDING_HANDLE __RPC_FAR *Binding,
    __deref_opt_out RPC_CSTR __RPC_FAR *Annotation
    );

#endif /* RPC_UNICODE_SUPPORTED */

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtEpUnregister (
    IN RPC_BINDING_HANDLE EpBinding OPTIONAL,
    IN RPC_IF_ID __RPC_FAR * IfId,
    IN RPC_BINDING_HANDLE Binding,
    IN UUID __RPC_FAR * ObjectUuid OPTIONAL
    );

typedef int
(__RPC_API * RPC_MGMT_AUTHORIZATION_FN) (
    IN RPC_BINDING_HANDLE ClientBinding,
    IN unsigned long RequestedMgmtOperation,
    OUT RPC_STATUS __RPC_FAR * Status
    );

#define RPC_C_MGMT_INQ_IF_IDS         0
#define RPC_C_MGMT_INQ_PRINC_NAME     1
#define RPC_C_MGMT_INQ_STATS          2
#define RPC_C_MGMT_IS_SERVER_LISTEN   3
#define RPC_C_MGMT_STOP_SERVER_LISTEN 4

RPCRTAPI
RPC_STATUS
RPC_ENTRY
RpcMgmtSetAuthorizationFn (
    IN RPC_MGMT_AUTHORIZATION_FN AuthorizationFn
    );


#define RPC_C_PARM_MAX_PACKET_LENGTH    1
#define RPC_C_PARM_BUFFER_LENGTH        2

#define RPC_IF_AUTOLISTEN                   0x0001
#define RPC_IF_OLE                          0x0002
#define RPC_IF_ALLOW_UNKNOWN_AUTHORITY      0x0004
#define RPC_IF_ALLOW_SECURE_ONLY            0x0008
#define RPC_IF_ALLOW_CALLBACKS_WITH_NO_AUTH 0x0010
#define RPC_IF_ALLOW_LOCAL_ONLY             0x0020
#define RPC_IF_SEC_NO_CACHE                 0x0040

#include <rpcdcep.h>

#ifdef __cplusplus
}
#endif

#endif /* __RPCDCE_H__ */

