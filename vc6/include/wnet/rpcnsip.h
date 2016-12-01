/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    rpcnsip.h

Abstract:

    This file contains the types and function definitions to use the
    to implement the autohandle features of the runtime.

--*/

#ifndef __RPCNSIP_H__
#define __RPCNSIP_H__

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
   RPC_NS_HANDLE        LookupContext;
   RPC_BINDING_HANDLE   ProposedHandle;
   RPC_BINDING_VECTOR * Bindings;

} RPC_IMPORT_CONTEXT_P, * PRPC_IMPORT_CONTEXT_P;


/* Stub Auto Binding routines. */

RPCNSAPI
RPC_STATUS
RPC_ENTRY
I_RpcNsGetBuffer(
    IN PRPC_MESSAGE Message
    );

RPCNSAPI
RPC_STATUS
RPC_ENTRY
I_RpcNsSendReceive(
    IN PRPC_MESSAGE Message,
    OUT RPC_BINDING_HANDLE __RPC_FAR * Handle
    );

RPCNSAPI
void
RPC_ENTRY
I_RpcNsRaiseException(
    IN PRPC_MESSAGE Message,
    IN RPC_STATUS Status
    );

RPCNSAPI
RPC_STATUS
RPC_ENTRY
I_RpcReBindBuffer(
    IN PRPC_MESSAGE Message
    );

RPCNSAPI
RPC_STATUS
RPC_ENTRY
I_NsServerBindSearch(
    );

RPCNSAPI
RPC_STATUS
RPC_ENTRY
I_NsClientBindSearch(
    );

RPCNSAPI
void
RPC_ENTRY
I_NsClientBindDone(
    );

#ifdef __cplusplus
}
#endif

#endif /* __RPCNSIP_H__ */

