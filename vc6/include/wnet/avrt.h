/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    avrt.h

Abstract:

    This module contains the multimedia class scheduler APIs and any public data
    structures needed to call these APIs.


Author:

    Samer Arafeh (samera) 13-Jan-2005

--*/


#ifndef _AVRT_
#define _AVRT_

#include <windows.h>

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
// AvRt Priorities
//

typedef enum _AVRT_PRIORITY
{
    AVRT_PRIORITY_LOW = -1,
    AVRT_PRIORITY_NORMAL,
    AVRT_PRIORITY_HIGH,
    AVRT_PRIORITY_CRITICAL
} AVRT_PRIORITY, *PAVRT_PRIORITY;


//
//  Infinite timeout for a thread order group.
//

#define THREAD_ORDER_GROUP_INFINITE_TIMEOUT     (-1I64)

//
// Define API decoration for direct importing of DLL references.
//

#if !defined(_AVRT_)
#define AVRTAPI DECLSPEC_IMPORT
#else
#define AVRTAPI
#endif


AVRTAPI
HANDLE
WINAPI
AvSetMmThreadCharacteristicsA (
    __in LPCSTR TaskName,
    __inout LPDWORD TaskIndex
    );
AVRTAPI
HANDLE
WINAPI
AvSetMmThreadCharacteristicsW (
    __in LPCWSTR TaskName,
    __inout LPDWORD TaskIndex
    );
#ifdef UNICODE
#define AvSetMmThreadCharacteristics  AvSetMmThreadCharacteristicsW
#else
#define AvSetMmThreadCharacteristics  AvSetMmThreadCharacteristicsA
#endif // !UNICODE
    
AVRTAPI
HANDLE
WINAPI
AvSetMmMaxThreadCharacteristicsA (
    __in LPCSTR FirstTask,
    __in LPCSTR SecondTask,
    __inout LPDWORD TaskIndex
    );
AVRTAPI
HANDLE
WINAPI
AvSetMmMaxThreadCharacteristicsW (
    __in LPCWSTR FirstTask,
    __in LPCWSTR SecondTask,
    __inout LPDWORD TaskIndex
    );
#ifdef UNICODE
#define AvSetMmMaxThreadCharacteristics  AvSetMmMaxThreadCharacteristicsW
#else
#define AvSetMmMaxThreadCharacteristics  AvSetMmMaxThreadCharacteristicsA
#endif // !UNICODE

AVRTAPI    
BOOL
WINAPI
AvRevertMmThreadCharacteristics (
    __in HANDLE AvrtHandle
    );
    
AVRTAPI
BOOL
WINAPI
AvSetMmThreadPriority (
    __in HANDLE AvrtHandle,
    __in AVRT_PRIORITY Priority
    );

AVRTAPI
BOOL
WINAPI
AvRtCreateThreadOrderingGroup (
    __out PHANDLE Context,
    __in PLARGE_INTEGER Period,
    __inout GUID *ThreadOrderingGuid,
    __in_opt PLARGE_INTEGER Timeout
    );
    
AVRTAPI
BOOL
WINAPI
AvRtCreateThreadOrderingGroupExA (
    __out PHANDLE Context,
    __in PLARGE_INTEGER Period,
    __inout GUID *ThreadOrderingGuid,
    __in_opt PLARGE_INTEGER Timeout,
    __in LPCSTR TaskName
    );
AVRTAPI
BOOL
WINAPI
AvRtCreateThreadOrderingGroupExW (
    __out PHANDLE Context,
    __in PLARGE_INTEGER Period,
    __inout GUID *ThreadOrderingGuid,
    __in_opt PLARGE_INTEGER Timeout,
    __in LPCWSTR TaskName
    );
#ifdef UNICODE
#define AvRtCreateThreadOrderingGroupEx  AvRtCreateThreadOrderingGroupExW
#else
#define AvRtCreateThreadOrderingGroupEx  AvRtCreateThreadOrderingGroupExA
#endif // !UNICODE

AVRTAPI
BOOL
WINAPI
AvRtJoinThreadOrderingGroup (
    __out PHANDLE Context,
    __in GUID *ThreadOrderingGuid,
    __in BOOL Before
    );
    
AVRTAPI
BOOL
WINAPI
AvRtWaitOnThreadOrderingGroup (
    __in HANDLE Context
    );

AVRTAPI
BOOL
WINAPI
AvRtLeaveThreadOrderingGroup (
    __in HANDLE Context
    );

AVRTAPI
BOOL
WINAPI
AvRtDeleteThreadOrderingGroup (
    __in HANDLE Context
    );

AVRTAPI
BOOL
WINAPI
AvQuerySystemResponsiveness (
    __in HANDLE AvrtHandle,
    __out PULONG SystemResponsivenessValue
    );

    
#ifdef __cplusplus
}
#endif

#endif // _AVRT_

