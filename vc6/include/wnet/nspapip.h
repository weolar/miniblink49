/*++

Copyright (c) 1991-1999  Microsoft Corporation

Module Name:

    nspapip.h

Abstract:

    Internel Name Space Provider API prototypes and manifests.  This
    header file should only be included by name space providers.  NSPAPI
    users should include only nspapi.h See the "Windows NT NameSpace
    Provider Specification" document for details.


Author:

    David Treadwell (davidtr)   28-Apr-1994

Environment:

    User Mode -Win32

Notes:

Revision History:

    28-Apr-1994     DavidTr      Created first cut.

--*/

#ifndef _NSPAPIP_INCLUDED
#define _NSPAPIP_INCLUDED

#if _MSC_VER > 1000
#pragma once
#endif

//
// Standard priority values for the dwPriority field of NS_ROUTINE.
//
#define NS_STANDARD_FAST_PRIORITY   (500)
#define NS_MAX_FAST_PRIORITY        (999)
#define NS_STANDARD_PRIORITY        (2000)

//
// Indices for the alpfnFunctions array field of NS_ROUTINE.
//
#define NSPAPI_GET_ADDRESS_BY_NAME  (0x00000000)
#define NSPAPI_GET_SERVICE          (0x00000001)
#define NSPAPI_SET_SERVICE          (0x00000002)

//
// Structures used by the provider interface.
//

typedef
INT
(APIENTRY *LPGET_ADDR_BY_NAME_PROC) (
    IN     LPGUID          lpServiceType,
    IN     LPWSTR          lpServiceName,
    IN     LPDWORD         lpdwProtocols,
    IN     DWORD           dwResolution,
    IN     OUT LPVOID      lpCsaddrBuffer,
    IN     OUT LPDWORD     lpdwBufferLength,
    IN OUT LPWSTR          lpAliasBuffer,
    IN OUT LPDWORD         lpdwAliasBufferLength,
    IN     HANDLE          hCancellationEvent
    );

typedef struct _NS_ROUTINE {
    DWORD        dwFunctionCount;
    LPFN_NSPAPI *alpfnFunctions;
    DWORD        dwNameSpace;
    DWORD        dwPriority;
} NS_ROUTINE, *PNS_ROUTINE, * FAR LPNS_ROUTINE;

typedef
DWORD

(APIENTRY *LPLOAD_NAME_SPACE_PROC) (
    IN OUT LPDWORD         lpdwVersion,
    IN OUT LPNS_ROUTINE    nsrBuffer,
    IN OUT LPDWORD         lpdwBufferLength
    );

typedef
INT
(APIENTRY *LPGET_SERVICE_PROC) (
    IN     LPGUID          lpServiceType,
    IN     LPWSTR          lpServiceName,
    IN     DWORD           dwProperties,
    IN     BOOL            fUnicodeBlob,
    OUT    LPSERVICE_INFO  lpServiceInfo,
    IN OUT LPDWORD         lpdwBufferLen
    );

typedef
DWORD
(APIENTRY *LPSET_SERVICE_PROC) (
    IN     DWORD           dwOperation,
    IN     DWORD           dwFlags,
    IN     BOOL            fUnicodeBlob,
    IN     LPSERVICE_INFO  lpServiceInfo
    );

//
// Internal Functions
//
DWORD
APIENTRY
NPGetService (
    IN     LPGUID          lpServiceType,
    IN     LPWSTR          lpServiceName,
    IN     DWORD           dwProperties,
    IN     BOOL            fUnicodeBlob,
    OUT    LPSERVICE_INFO  lpServiceInfo,
    IN OUT LPDWORD         lpdwBufferLen
    );

DWORD
APIENTRY
NPSetService (
    IN     DWORD           dwOperation,
    IN     DWORD           dwFlags,
    IN     BOOL            fUnicodeBlob,
    IN     LPSERVICE_INFO  lpServiceInfo
    );

INT
APIENTRY
NPGetAddressByName (
    IN     LPGUID          lpServiceType,
    IN     LPWSTR          lpServiceName,
    IN     LPDWORD         lpdwProtocols,
    IN     DWORD           dwResolution,
    IN OUT LPVOID          lpCsaddrBuffer,
    IN OUT LPDWORD         lpdwBufferLength,
    IN OUT LPWSTR          lpAliasBuffer,
    IN OUT LPDWORD         lpdwAliasBufferLength,
    IN     HANDLE          hCancellationEvent
    );

INT
APIENTRY
NPLoadNameSpaces (
    IN OUT LPDWORD         lpdwVersion,
    IN OUT LPNS_ROUTINE    nsrBuffer,
    IN OUT LPDWORD         lpdwBufferLength
    );

#endif  // _NSPAPIP_INCLUDED

