
/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    evntprov.h

Abstract:

    This defines the unified provider side user mode API.

Revision History:

--*/

#ifndef _EVNTPROV_H_
#define _EVNTPROV_H_

#pragma once

#ifndef EVNTAPI
#ifndef MIDL_PASS
#ifdef _EVNT_SOURCE_
#define EVNTAPI __stdcall
#else
#define EVNTAPI DECLSPEC_IMPORT __stdcall
#endif // _EVNT_SOURCE_
#endif // MIDL_PASS
#endif // EVNTAPI

#ifdef __cplusplus
extern "C" {
#endif


#define EVENT_MIN_LEVEL                      (0)
#define EVENT_MAX_LEVEL                      (0xff)

#define EVENT_ACTIVITY_CTRL_GET_ID           (1)
#define EVENT_ACTIVITY_CTRL_SET_ID           (2)
#define EVENT_ACTIVITY_CTRL_CREATE_ID        (3)
#define EVENT_ACTIVITY_CTRL_GET_SET_ID       (4)
#define EVENT_ACTIVITY_CTRL_CREATE_SET_ID    (5)

typedef ULONGLONG REGHANDLE, *PREGHANDLE;

#define MAX_EVENT_DATA_DESCRIPTORS           (128)
#define MAX_EVENT_FILTER_DATA_SIZE           (1024)

#define EVENT_FILTER_TYPE_SCHEMATIZED        (0x80000000)

//
// EVENT_DATA_DESCRIPTOR is used to pass in user data items
// in events.
// 
typedef struct _EVENT_DATA_DESCRIPTOR {

    ULONGLONG   Ptr;        // Pointer to data
    ULONG       Size;       // Size of data in bytes
    ULONG       Reserved;

} EVENT_DATA_DESCRIPTOR, *PEVENT_DATA_DESCRIPTOR;

//
// EVENT_DESCRIPTOR describes and categorizes an event.
// 
typedef struct _EVENT_DESCRIPTOR {

    USHORT      Id;
    UCHAR       Version;
    UCHAR       Channel;
    UCHAR       Level;
    UCHAR       Opcode;
    USHORT      Task;
    ULONGLONG   Keyword;

} EVENT_DESCRIPTOR, *PEVENT_DESCRIPTOR;

typedef const EVENT_DESCRIPTOR *PCEVENT_DESCRIPTOR;

//
// EVENT_FILTER_DESCRIPTOR is used to pass in enable filter
// data item to a user callback function.
// 
typedef struct _EVENT_FILTER_DESCRIPTOR {

    ULONGLONG   Ptr;
    ULONG       Size;
    ULONG       Type;

} EVENT_FILTER_DESCRIPTOR, *PEVENT_FILTER_DESCRIPTOR;


typedef struct _EVENT_FILTER_HEADER {

    USHORT     Id;
    UCHAR      Version;
    UCHAR      Reserved[5];
    ULONGLONG  InstanceId;
    ULONG      Size;
    ULONG      NextOffset;

} EVENT_FILTER_HEADER, *PEVENT_FILTER_HEADER;

#ifndef _ETW_KM_ 

//
// Optional callback function that users provide
//
typedef 
VOID
(NTAPI *PENABLECALLBACK) (
    __in LPCGUID SourceId,
    __in ULONG IsEnabled,
    __in UCHAR Level,
    __in ULONGLONG MatchAnyKeyword,
    __in ULONGLONG MatchAllKeyword,
    __in_opt PEVENT_FILTER_DESCRIPTOR FilterData,
    __inout_opt PVOID CallbackContext
    );  

//
// Registration APIs
//

#if (WINVER >= _WIN32_WINNT_VISTA)
ULONG
EVNTAPI
EventRegister(
    __in LPCGUID ProviderId,
    __in_opt PENABLECALLBACK EnableCallback,
    __in_opt PVOID CallbackContext,
    __out PREGHANDLE RegHandle
    );
#endif

#if (WINVER >= _WIN32_WINNT_VISTA)
ULONG
EVNTAPI
EventUnregister(
    __in REGHANDLE RegHandle
    );
#endif

//
// Control (Is Enabled) APIs
//

#if (WINVER >= _WIN32_WINNT_VISTA)
BOOLEAN
EVNTAPI
EventEnabled(
    __in REGHANDLE RegHandle,
    __in PCEVENT_DESCRIPTOR EventDescriptor
    );
#endif

#if (WINVER >= _WIN32_WINNT_VISTA)
BOOLEAN
EVNTAPI
EventProviderEnabled(
    __in REGHANDLE RegHandle,
    __in UCHAR Level,
    __in ULONGLONG Keyword
    );
#endif

//
// Writing (Publishing/Logging) APIs
//

#if (WINVER >= _WIN32_WINNT_VISTA)
ULONG
EVNTAPI
EventWrite(
    __in REGHANDLE RegHandle,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in ULONG UserDataCount,
    __in_ecount_opt(UserDataCount) PEVENT_DATA_DESCRIPTOR UserData
    );
#endif

#if (WINVER >= _WIN32_WINNT_VISTA)
ULONG
EVNTAPI
EventWriteTransfer(
    __in REGHANDLE RegHandle,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in_opt LPCGUID ActivityId,
    __in_opt LPCGUID RelatedActivityId,
    __in ULONG UserDataCount,
    __in_ecount_opt(UserDataCount) PEVENT_DATA_DESCRIPTOR UserData
    );
#endif

#if (WINVER >= _WIN32_WINNT_WIN7)
ULONG
EVNTAPI
EventWriteEx(
    __in REGHANDLE RegHandle,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in ULONG64 Filter,
    __in ULONG Flags,
    __in_opt LPCGUID ActivityId,
    __in_opt LPCGUID RelatedActivityId,
    __in ULONG UserDataCount,
    __in_ecount_opt(UserDataCount) PEVENT_DATA_DESCRIPTOR UserData
    );
#endif

#if (WINVER >= _WIN32_WINNT_VISTA)
ULONG
EVNTAPI
EventWriteString(
    __in REGHANDLE RegHandle,
    __in UCHAR Level,
    __in ULONGLONG Keyword,
    __in PCWSTR String
    );
#endif


//
// ActivityId Control APIs
//

#if (WINVER >= _WIN32_WINNT_VISTA)
ULONG
EVNTAPI
EventActivityIdControl(
    __in ULONG ControlCode,
    __inout LPGUID ActivityId
    );
#endif

#endif // _ETW_KM_ 


//
// Macros to create Event and Event Data Descriptors
//

FORCEINLINE
VOID
EventDataDescCreate(
    __out PEVENT_DATA_DESCRIPTOR EventDataDescriptor,
    __in const VOID* DataPtr,
    __in ULONG DataSize
    )
{
    EventDataDescriptor->Ptr = (ULONGLONG)(ULONG_PTR)DataPtr;
    EventDataDescriptor->Size = DataSize;
    EventDataDescriptor->Reserved = 0;
    return;
}

FORCEINLINE
VOID
EventDescCreate(
    __out PEVENT_DESCRIPTOR EventDescriptor,
    __in USHORT Id,
    __in UCHAR Version,
    __in UCHAR Channel,
    __in UCHAR Level,
    __in USHORT Task,
    __in UCHAR Opcode,
    __in ULONGLONG Keyword
    )
{
    EventDescriptor->Id = Id;
    EventDescriptor->Version = Version;
    EventDescriptor->Channel = Channel;
    EventDescriptor->Level = Level;
    EventDescriptor->Task = Task;
    EventDescriptor->Opcode = Opcode;
    EventDescriptor->Keyword = Keyword;
    return;
}

FORCEINLINE
VOID
EventDescZero(
    __out PEVENT_DESCRIPTOR EventDescriptor
    )
{
    memset(EventDescriptor, 0, sizeof(EVENT_DESCRIPTOR));
    return;
}

//
// Macros to extract info from an Event Descriptor
//

FORCEINLINE
USHORT
EventDescGetId(
    __in PCEVENT_DESCRIPTOR EventDescriptor
    )
{
    return (EventDescriptor->Id);
}

FORCEINLINE
UCHAR
EventDescGetVersion(
    __in PCEVENT_DESCRIPTOR EventDescriptor
    )
{
    return (EventDescriptor->Version);
}

FORCEINLINE
USHORT
EventDescGetTask(
    __in PCEVENT_DESCRIPTOR EventDescriptor
    )
{
    return (EventDescriptor->Task);
}

FORCEINLINE
UCHAR
EventDescGetOpcode(
    __in PCEVENT_DESCRIPTOR EventDescriptor
    )
{
    return (EventDescriptor->Opcode);
}

FORCEINLINE
UCHAR
EventDescGetChannel(
    __in PCEVENT_DESCRIPTOR EventDescriptor
    )
{
    return (EventDescriptor->Channel);
}

FORCEINLINE
UCHAR
EventDescGetLevel(
    __in PCEVENT_DESCRIPTOR EventDescriptor
    )
{
    return (EventDescriptor->Level);
}

FORCEINLINE
ULONGLONG
EventDescGetKeyword(
    __in PCEVENT_DESCRIPTOR EventDescriptor
    )
{
    return (EventDescriptor->Keyword);
}

//
// Macros to set info into an Event Descriptor
//

FORCEINLINE
PEVENT_DESCRIPTOR
EventDescSetId(
    __in PEVENT_DESCRIPTOR EventDescriptor,
    __in USHORT Id
    )
{
    EventDescriptor->Id         = Id;
    return (EventDescriptor);
}

FORCEINLINE
PEVENT_DESCRIPTOR
EventDescSetVersion(
    __in PEVENT_DESCRIPTOR EventDescriptor,
    __in UCHAR Version
    )
{
    EventDescriptor->Version    = Version;
    return (EventDescriptor);
}

FORCEINLINE
PEVENT_DESCRIPTOR
EventDescSetTask(
    __in PEVENT_DESCRIPTOR EventDescriptor,
    __in USHORT Task
    )
{
    EventDescriptor->Task       = Task;
    return (EventDescriptor);
}

FORCEINLINE
PEVENT_DESCRIPTOR
EventDescSetOpcode(
    __in PEVENT_DESCRIPTOR EventDescriptor,
    __in UCHAR Opcode
    )
{
    EventDescriptor->Opcode     = Opcode;
    return (EventDescriptor);
}

FORCEINLINE
PEVENT_DESCRIPTOR
EventDescSetLevel(
    __in PEVENT_DESCRIPTOR EventDescriptor,
    __in UCHAR  Level
    )
{
    EventDescriptor->Level      = Level;
    return (EventDescriptor);
}

FORCEINLINE
PEVENT_DESCRIPTOR
EventDescSetChannel(
    __in PEVENT_DESCRIPTOR EventDescriptor,
    __in UCHAR Channel
    )
{
    EventDescriptor->Channel    = Channel;
    return (EventDescriptor);
}

FORCEINLINE
PEVENT_DESCRIPTOR
EventDescSetKeyword(
    __in PEVENT_DESCRIPTOR EventDescriptor,
    __in ULONGLONG Keyword
    )
{
    EventDescriptor->Keyword    = Keyword;
    return (EventDescriptor);
}


FORCEINLINE
PEVENT_DESCRIPTOR
EventDescOrKeyword(
    __in PEVENT_DESCRIPTOR EventDescriptor,
    __in ULONGLONG Keyword
    )
{
    EventDescriptor->Keyword    |= Keyword;
    return (EventDescriptor);
}


#ifdef __cplusplus
}
#endif
#endif

