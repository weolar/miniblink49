/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    EvnTrace.h

Abstract:

    Public headers for event tracing control applications,
    consumers and providers

--*/

#ifndef _EVNTRACE_
#define _EVNTRACE_

#if defined(_WINNT_) || defined(WINNT)
#ifndef WMIAPI
#ifndef MIDL_PASS
#ifdef _WMI_SOURCE_
#define WMIAPI __stdcall
#else
#define WMIAPI DECLSPEC_IMPORT __stdcall
#endif // _WMI_SOURCE
#endif // MIDL_PASS

#endif // WMIAPI

#include <guiddef.h>

//
// EventTraceGuid is used to identify a event tracing session
//
DEFINE_GUID ( /* 68fdd900-4a3e-11d1-84f4-0000f80464e3 */
    EventTraceGuid,
    0x68fdd900,
    0x4a3e,
    0x11d1,
    0x84, 0xf4, 0x00, 0x00, 0xf8, 0x04, 0x64, 0xe3
  );

//
// SystemTraceControlGuid. Used to specify event tracing for kernel
//
DEFINE_GUID ( /* 9e814aad-3204-11d2-9a82-006008a86939 */
    SystemTraceControlGuid,
    0x9e814aad,
    0x3204,
    0x11d2,
    0x9a, 0x82, 0x00, 0x60, 0x08, 0xa8, 0x69, 0x39
  );

//
// EventTraceConfigGuid. Used to report system configuration records
//
DEFINE_GUID ( /* 01853a65-418f-4f36-aefc-dc0f1d2fd235 */
    EventTraceConfigGuid,
    0x01853a65,
    0x418f,
    0x4f36,
    0xae, 0xfc, 0xdc, 0x0f, 0x1d, 0x2f, 0xd2, 0x35
  );

//
// DefaultTraceSecurityGuid. Specifies the default event tracing security
//
DEFINE_GUID ( /* 0811c1af-7a07-4a06-82ed-869455cdf713 */
    DefaultTraceSecurityGuid,
    0x0811c1af,
    0x7a07,
    0x4a06,
    0x82, 0xed, 0x86, 0x94, 0x55, 0xcd, 0xf7, 0x13
  );

#define KERNEL_LOGGER_NAMEW   L"NT Kernel Logger"
#define GLOBAL_LOGGER_NAMEW   L"GlobalLogger"
#define EVENT_LOGGER_NAMEW    L"Event Log"

#define KERNEL_LOGGER_NAMEA   "NT Kernel Logger"
#define GLOBAL_LOGGER_NAMEA   "GlobalLogger"
#define EVENT_LOGGER_NAMEA    "Event Log"

#define MAX_MOF_FIELDS                      16  // Limit of USE_MOF_PTR fields
typedef ULONG64 TRACEHANDLE, *PTRACEHANDLE;

//
// predefined generic event types (0x00 to 0x09 reserved).
//

#define EVENT_TRACE_TYPE_INFO               0x00  // Info or point event
#define EVENT_TRACE_TYPE_START              0x01  // Start event
#define EVENT_TRACE_TYPE_END                0x02  // End event
#define EVENT_TRACE_TYPE_DC_START           0x03  // Collection start marker
#define EVENT_TRACE_TYPE_DC_END             0x04  // Collection end marker
#define EVENT_TRACE_TYPE_EXTENSION          0x05  // Extension/continuation
#define EVENT_TRACE_TYPE_REPLY              0x06  // Reply event
#define EVENT_TRACE_TYPE_DEQUEUE            0x07  // De-queue event
#define EVENT_TRACE_TYPE_CHECKPOINT         0x08  // Generic checkpoint event
#define EVENT_TRACE_TYPE_RESERVED9          0x09

//
// Predefined Event Tracing Levels for Software/Debug Tracing
//
//
// Trace Level is UCHAR and passed in through the EnableLevel parameter
// in EnableTrace API. It is retrieved by the provider using the
// GetTraceEnableLevel macro.It should be interpreted as an integer value
// to mean everything at or below that level will be traced.
//
// Here are the possible Levels.
//

#define TRACE_LEVEL_NONE        0   // Tracing is not on
#define TRACE_LEVEL_FATAL       1   // Abnormal exit or termination
#define TRACE_LEVEL_ERROR       2   // Severe errors that need logging
#define TRACE_LEVEL_WARNING     3   // Warnings such as allocation failure
#define TRACE_LEVEL_INFORMATION 4   // Includes non-error cases(e.g.,Entry-Exit)
#define TRACE_LEVEL_VERBOSE     5   // Detailed traces from intermediate steps
#define TRACE_LEVEL_RESERVED6   6
#define TRACE_LEVEL_RESERVED7   7
#define TRACE_LEVEL_RESERVED8   8
#define TRACE_LEVEL_RESERVED9   9


//
// Event types for Process & Threads
//

#define EVENT_TRACE_TYPE_LOAD                  0x0A      // Load image

//
// Event types for IO subsystem
//

#define EVENT_TRACE_TYPE_IO_READ               0x0A
#define EVENT_TRACE_TYPE_IO_WRITE              0x0B

//
// Event types for Memory subsystem
//

#define EVENT_TRACE_TYPE_MM_TF                 0x0A      // Transition fault
#define EVENT_TRACE_TYPE_MM_DZF                0x0B      // Demand Zero fault
#define EVENT_TRACE_TYPE_MM_COW                0x0C      // Copy on Write
#define EVENT_TRACE_TYPE_MM_GPF                0x0D      // Guard Page fault
#define EVENT_TRACE_TYPE_MM_HPF                0x0E      // Hard page fault

//
// Event types for Network subsystem, all protocols
//

#define EVENT_TRACE_TYPE_SEND                  0x0A     // Send
#define EVENT_TRACE_TYPE_RECEIVE               0x0B     // Receive
#define EVENT_TRACE_TYPE_CONNECT               0x0C     // Connect
#define EVENT_TRACE_TYPE_DISCONNECT            0x0D     // Disconnect
#define EVENT_TRACE_TYPE_RETRANSMIT            0x0E     // ReTransmit
#define EVENT_TRACE_TYPE_ACCEPT                0x0F     // Accept
#define EVENT_TRACE_TYPE_RECONNECT             0x10     // ReConnect
#define EVENT_TRACE_TYPE_CONNFAIL              0x11     // Fail
#define EVENT_TRACE_TYPE_COPY_TCP              0x12     // Copy in PendData
#define EVENT_TRACE_TYPE_COPY_ARP              0x13     // NDIS_STATUS_RESOURCES Copy
#define EVENT_TRACE_TYPE_ACKFULL               0x14     // A full data ACK
#define EVENT_TRACE_TYPE_ACKPART               0x15     // A Partial data ACK
#define EVENT_TRACE_TYPE_ACKDUP                0x16     // A Duplicate data ACK


//
// Event Types for the Header (to handle internal event headers)
//

#define EVENT_TRACE_TYPE_GUIDMAP                0x0A
#define EVENT_TRACE_TYPE_CONFIG                 0x0B
#define EVENT_TRACE_TYPE_SIDINFO                0x0C
#define EVENT_TRACE_TYPE_SECURITY               0x0D

//
// Event types for Registry subsystem
//

#define EVENT_TRACE_TYPE_REGCREATE              0x0A     // NtCreateKey
#define EVENT_TRACE_TYPE_REGOPEN                0x0B     // NtOpenKey
#define EVENT_TRACE_TYPE_REGDELETE              0x0C     // NtDeleteKey
#define EVENT_TRACE_TYPE_REGQUERY               0x0D     // NtQueryKey
#define EVENT_TRACE_TYPE_REGSETVALUE            0x0E     // NtSetValueKey
#define EVENT_TRACE_TYPE_REGDELETEVALUE         0x0F     // NtDeleteValueKey
#define EVENT_TRACE_TYPE_REGQUERYVALUE          0x10     // NtQueryValueKey
#define EVENT_TRACE_TYPE_REGENUMERATEKEY        0x11     // NtEnumerateKey
#define EVENT_TRACE_TYPE_REGENUMERATEVALUEKEY   0x12     // NtEnumerateValueKey
#define EVENT_TRACE_TYPE_REGQUERYMULTIPLEVALUE  0x13     // NtQueryMultipleValueKey
#define EVENT_TRACE_TYPE_REGSETINFORMATION      0x14     // NtSetInformationKey
#define EVENT_TRACE_TYPE_REGFLUSH               0x15     // NtFlushKey
#define EVENT_TRACE_TYPE_REGKCBDMP              0x16     // KcbDump/create

//
// Event types for system configuration records
//
#define EVENT_TRACE_TYPE_CONFIG_CPU             0x0A     // CPU Configuration
#define EVENT_TRACE_TYPE_CONFIG_PHYSICALDISK    0x0B     // Physical Disk Configuration
#define EVENT_TRACE_TYPE_CONFIG_LOGICALDISK     0x0C     // Logical Disk Configuration
#define EVENT_TRACE_TYPE_CONFIG_NIC             0x0D     // NIC Configuration
#define EVENT_TRACE_TYPE_CONFIG_VIDEO           0x0E     // Video Adapter Configuration
#define EVENT_TRACE_TYPE_CONFIG_SERVICES        0x0F     // Active Services
#define EVENT_TRACE_TYPE_CONFIG_POWER           0x10     // ACPI Configuration
#define EVENT_TRACE_TYPE_CONFIG_NETINFO         0x11     // Networking Configuration

//
// Enable flags for SystemControlGuid only
//
#define EVENT_TRACE_FLAG_PROCESS            0x00000001  // process start & end
#define EVENT_TRACE_FLAG_THREAD             0x00000002  // thread start & end
#define EVENT_TRACE_FLAG_IMAGE_LOAD         0x00000004  // image load

#define EVENT_TRACE_FLAG_DISK_IO            0x00000100  // physical disk IO
#define EVENT_TRACE_FLAG_DISK_FILE_IO       0x00000200  // requires disk IO

#define EVENT_TRACE_FLAG_MEMORY_PAGE_FAULTS 0x00001000  // all page faults
#define EVENT_TRACE_FLAG_MEMORY_HARD_FAULTS 0x00002000  // hard faults only

#define EVENT_TRACE_FLAG_NETWORK_TCPIP      0x00010000  // tcpip send & receive

#define EVENT_TRACE_FLAG_REGISTRY           0x00020000  // registry calls
#define EVENT_TRACE_FLAG_DBGPRINT           0x00040000  // DbgPrint(ex) Calls

#define EVENT_TRACE_FLAG_VOLMGR             0x00200000  // volume manager traces

//
// Pre-defined Enable flags for everybody else
//
#define EVENT_TRACE_FLAG_EXTENSION          0x80000000  // indicates more flags
#define EVENT_TRACE_FLAG_FORWARD_WMI        0x40000000  // Can forward to WMI
#define EVENT_TRACE_FLAG_ENABLE_RESERVE     0x20000000  // Reserved

//
// Logger Mode flags
//

#define EVENT_TRACE_FILE_MODE_NONE          0x00000000  // logfile is off
#define EVENT_TRACE_FILE_MODE_SEQUENTIAL    0x00000001  // log sequentially
#define EVENT_TRACE_FILE_MODE_CIRCULAR      0x00000002  // log in circular manner
#define EVENT_TRACE_FILE_MODE_APPEND        0x00000004  // append sequential log
#define EVENT_TRACE_FILE_MODE_NEWFILE       0x00000008  // auto-switch log file

#define EVENT_TRACE_FILE_MODE_PREALLOCATE   0x00000020  // pre-allocate mode

#define EVENT_TRACE_REAL_TIME_MODE          0x00000100  // real time mode on
#define EVENT_TRACE_DELAY_OPEN_FILE_MODE    0x00000200  // delay opening file
#define EVENT_TRACE_BUFFERING_MODE          0x00000400  // buffering mode only
#define EVENT_TRACE_PRIVATE_LOGGER_MODE     0x00000800  // Process Private Logger
#define EVENT_TRACE_ADD_HEADER_MODE         0x00001000  // Add a logfile header
#define EVENT_TRACE_USE_GLOBAL_SEQUENCE     0x00004000  // Use global sequence no.
#define EVENT_TRACE_USE_LOCAL_SEQUENCE      0x00008000  // Use local sequence no.

#define EVENT_TRACE_RELOG_MODE              0x00010000  // Relogger

#define EVENT_TRACE_USE_PAGED_MEMORY        0x01000000  // Use pageable buffers   

//
// internal control codes used.
//
#define EVENT_TRACE_CONTROL_QUERY           0
#define EVENT_TRACE_CONTROL_STOP            1
#define EVENT_TRACE_CONTROL_UPDATE          2
#define EVENT_TRACE_CONTROL_FLUSH           3       // Flushes all the buffers

//
// Flags used by WMI Trace Message
// Note that the order or value of these flags should NOT be changed as they are processed
// in this order.
//
#define TRACE_MESSAGE_SEQUENCE		1           // Message should include a sequence number
#define TRACE_MESSAGE_GUID			2           // Message includes a GUID
#define TRACE_MESSAGE_COMPONENTID   4           // Message has no GUID, Component ID instead
#define	TRACE_MESSAGE_TIMESTAMP		8           // Message includes a timestamp
#define TRACE_MESSAGE_PERFORMANCE_TIMESTAMP 16  // *Obsolete* Clock type is controlled by the logger
#define	TRACE_MESSAGE_SYSTEMINFO	32          // Message includes system information TID,PID
#define TRACE_MESSAGE_FLAG_MASK     0xFFFF      // Only the lower 16 bits of flags are placed in the message
                                                // those above 16 bits are reserved for local processing
#define TRACE_MESSAGE_MAXIMUM_SIZE  8*1024      // the maximum size allowed for a single trace message
                                                // longer messages will return ERROR_BUFFER_OVERFLOW
//
// Flags to indicate to consumer which fields
// in the EVENT_TRACE_HEADER are valid
//

#define EVENT_TRACE_USE_PROCTIME   0x0001    // ProcessorTime field is valid
#define EVENT_TRACE_USE_NOCPUTIME  0x0002    // No Kernel/User/Processor Times

#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning (disable:4201)
//
// Trace header for all (except kernel) events. This is used to overlay
// to bottom part of WNODE_HEADER to conserve space.
//

typedef struct _EVENT_TRACE_HEADER {        // overlays WNODE_HEADER
    USHORT          Size;                   // Size of entire record
    union {
        USHORT      FieldTypeFlags;         // Indicates valid fields
        struct {
            UCHAR   HeaderType;             // Header type - internal use only
            UCHAR   MarkerFlags;            // Marker - internal use only
        };
    };
    union {
        ULONG       Version;
        struct {
            UCHAR   Type;                   // event type
            UCHAR   Level;                  // trace instrumentation level
            USHORT  Version;                // version of trace record
        } Class;
    };
    ULONG           ThreadId;               // Thread Id
    ULONG           ProcessId;              // Process Id
    LARGE_INTEGER   TimeStamp;              // time when event happens
    union {
        GUID        Guid;                   // Guid that identifies event
        ULONGLONG   GuidPtr;                // use with WNODE_FLAG_USE_GUID_PTR
    };
    union {
        struct {
            ULONG   ClientContext;          // Reserved
            ULONG   Flags;                  // Flags for header
        };
        struct {
            ULONG   KernelTime;             // Kernel Mode CPU ticks
            ULONG   UserTime;               // User mode CPU ticks
        };
        ULONG64     ProcessorTime;          // Processor Clock
    };
} EVENT_TRACE_HEADER, *PEVENT_TRACE_HEADER;

//
// This header is used to trace and track transaction co-relations
//
typedef struct _EVENT_INSTANCE_HEADER {
    USHORT          Size;
    union {
        USHORT      FieldTypeFlags;     // Indicates valid fields
        struct {
            UCHAR   HeaderType;         // Header type - internal use only
            UCHAR   MarkerFlags;        // Marker - internal use only
        };
    };
    union {
        ULONG       Version;
        struct {
            UCHAR   Type;
            UCHAR   Level;
            USHORT  Version;
        } Class;
    };
    ULONG           ThreadId;
    ULONG           ProcessId;
    LARGE_INTEGER   TimeStamp;
    ULONGLONG       RegHandle;
    ULONG           InstanceId;
    ULONG           ParentInstanceId;
    union {
        struct {
            ULONG   ClientContext;          // Reserved
            ULONG   Flags;                  // Flags for header
        };
        struct {
            ULONG   KernelTime;             // Kernel Mode CPU ticks
            ULONG   UserTime;               // User mode CPU ticks
        };
        ULONG64     ProcessorTime;          // Processor Clock
    };
    ULONGLONG       ParentRegHandle;
} EVENT_INSTANCE_HEADER, *PEVENT_INSTANCE_HEADER;

//
// Following are structures and macros for use with USE_MOF_PTR
//

#define DEFINE_TRACE_MOF_FIELD(MOF, ptr, length, type) \
    (MOF)->DataPtr  = (ULONG64) ptr; \
    (MOF)->Length   = (ULONG) length; \
    (MOF)->DataType = (ULONG) type;

typedef struct _MOF_FIELD {
    ULONG64     DataPtr;    // Pointer to the field. Up to 64-bits only
    ULONG       Length;     // Length of the MOF field
    ULONG       DataType;   // Type of data
} MOF_FIELD, *PMOF_FIELD;

#if !(defined(_NTDDK_) || defined(_NTIFS_)) || defined(_WMIKM_)
//
// This is the header for every logfile. The memory for LoggerName
// and LogFileName must be contiguous adjacent to this structure
// Allows both user-mode and kernel-mode to understand the header
//
typedef struct _TRACE_LOGFILE_HEADER {
    ULONG           BufferSize;         // Logger buffer size in Kbytes
    union {
        ULONG       Version;            // Logger version
        struct {
            UCHAR   MajorVersion;
            UCHAR   MinorVersion;
            UCHAR   SubVersion;
            UCHAR   SubMinorVersion;
        } VersionDetail;
    };
    ULONG           ProviderVersion;    // defaults to NT version
    ULONG           NumberOfProcessors; // Number of Processors
    LARGE_INTEGER   EndTime;            // Time when logger stops
    ULONG           TimerResolution;    // assumes timer is constant!!!
    ULONG           MaximumFileSize;    // Maximum in Mbytes
    ULONG           LogFileMode;        // specify logfile mode
    ULONG           BuffersWritten;     // used to file start of Circular File
    union {
        GUID LogInstanceGuid;           // For RealTime Buffer Delivery
        struct {
            ULONG   StartBuffers;       // Count of buffers written at start.
            ULONG   PointerSize;        // Size of pointer type in bits
            ULONG   EventsLost;         // Events losts during log session
            ULONG   CpuSpeedInMHz;      // Cpu Speed in MHz
        };
    };
#if defined(_WMIKM_)
    PWCHAR          LoggerName;
    PWCHAR          LogFileName;
    RTL_TIME_ZONE_INFORMATION TimeZone;
#else
    LPWSTR          LoggerName;
    LPWSTR          LogFileName;
    TIME_ZONE_INFORMATION TimeZone;
#endif
    LARGE_INTEGER   BootTime;
    LARGE_INTEGER   PerfFreq;           // Reserved
    LARGE_INTEGER   StartTime;          // Reserved
    ULONG           ReservedFlags;      // Reserved
    ULONG           BuffersLost;
} TRACE_LOGFILE_HEADER, *PTRACE_LOGFILE_HEADER;

#endif // !_NTDDK_ || _WMIKM_


//
// Instance Information to track parent child relationship of Instances.
//
typedef struct EVENT_INSTANCE_INFO {
    HANDLE      RegHandle;
    ULONG       InstanceId;
} EVENT_INSTANCE_INFO, *PEVENT_INSTANCE_INFO;

#if !defined(_WMIKM_) && !defined(_NTDDK_) && !defined(_NTIFS_)
//
// Structures that have UNICODE and ANSI versions are defined here
//

//
// Logger configuration and running statistics. This structure is used
// by user-mode callers, such as PDH library
//

typedef struct _EVENT_TRACE_PROPERTIES {
    WNODE_HEADER Wnode;
//
// data provided by caller
    ULONG BufferSize;                   // buffer size for logging (kbytes)
    ULONG MinimumBuffers;               // minimum to preallocate
    ULONG MaximumBuffers;               // maximum buffers allowed
    ULONG MaximumFileSize;              // maximum logfile size (in MBytes)
    ULONG LogFileMode;                  // sequential, circular
    ULONG FlushTimer;                   // buffer flush timer, in seconds
    ULONG EnableFlags;                  // trace enable flags
    LONG  AgeLimit;                     // age decay time, in minutes

// data returned to caller
    ULONG NumberOfBuffers;              // no of buffers in use
    ULONG FreeBuffers;                  // no of buffers free
    ULONG EventsLost;                   // event records lost
    ULONG BuffersWritten;               // no of buffers written to file
    ULONG LogBuffersLost;               // no of logfile write failures
    ULONG RealTimeBuffersLost;          // no of rt delivery failures
    HANDLE LoggerThreadId;              // thread id of Logger
    ULONG LogFileNameOffset;            // Offset to LogFileName
    ULONG LoggerNameOffset;             // Offset to LoggerName
} EVENT_TRACE_PROPERTIES, *PEVENT_TRACE_PROPERTIES;

// NOTE:
// If AgeLimit is 0, default is used
// If AgeLimit is < 0, buffer aging is turned off

typedef struct _TRACE_GUID_PROPERTIES {
    GUID    Guid;
    ULONG   GuidType;
    ULONG   LoggerId;
    ULONG   EnableLevel;
    ULONG   EnableFlags;
    BOOLEAN     IsEnable;
} TRACE_GUID_PROPERTIES, *PTRACE_GUID_PROPERTIES;


//
// Data Provider structures
//
// Used by RegisterTraceGuids()

typedef struct  _TRACE_GUID_REGISTRATION {
    LPCGUID Guid;           // Guid of data block being registered or updated.
    HANDLE RegHandle;      // Guid Registration Handle is returned.
} TRACE_GUID_REGISTRATION, *PTRACE_GUID_REGISTRATION;

//
// Data consumer structures
//

// An EVENT_TRACE consists of a fixed header (EVENT_TRACE_HEADER) and
// optionally a variable portion pointed to by MofData. The datablock
// layout of the variable portion is unknown to the Logger and must
// be obtained from WBEM CIMOM database.
//
typedef struct _EVENT_TRACE {
    EVENT_TRACE_HEADER      Header;             // Event trace header
    ULONG                   InstanceId;         // Instance Id of this event
    ULONG                   ParentInstanceId;   // Parent Instance Id.
    GUID                    ParentGuid;         // Parent Guid;
    PVOID                   MofData;            // Pointer to Variable Data
    ULONG                   MofLength;          // Variable Datablock Length
    ULONG                   ClientContext;      // Reserved
} EVENT_TRACE, *PEVENT_TRACE;


typedef struct _EVENT_TRACE_LOGFILEW
                EVENT_TRACE_LOGFILEW, *PEVENT_TRACE_LOGFILEW;

typedef struct _EVENT_TRACE_LOGFILEA
                EVENT_TRACE_LOGFILEA, *PEVENT_TRACE_LOGFILEA;

typedef ULONG (WINAPI * PEVENT_TRACE_BUFFER_CALLBACKW)
                (PEVENT_TRACE_LOGFILEW Logfile);

typedef ULONG (WINAPI * PEVENT_TRACE_BUFFER_CALLBACKA)
                (PEVENT_TRACE_LOGFILEA Logfile);

typedef VOID (WINAPI *PEVENT_CALLBACK)( PEVENT_TRACE pEvent );

//
// Prototype for service request callback. Data providers register with WMI
// by passing a service request callback function that is called for all
// wmi requests.

typedef ULONG (
#ifndef MIDL_PASS
WINAPI
#endif
*WMIDPREQUEST)(
    IN WMIDPREQUESTCODE RequestCode,
    IN PVOID RequestContext,
    IN OUT ULONG *BufferSize,
    IN OUT PVOID Buffer
    );


struct _EVENT_TRACE_LOGFILEW {
    LPWSTR                  LogFileName;    // Logfile Name
    LPWSTR                  LoggerName;     // LoggerName
    LONGLONG                CurrentTime;    // timestamp of last event
    ULONG                   BuffersRead;    // buffers read to date
    ULONG                   LogFileMode;    // Mode of the logfile

    EVENT_TRACE             CurrentEvent;   // Current Event from this stream.
    TRACE_LOGFILE_HEADER    LogfileHeader;  // logfile header structure
    PEVENT_TRACE_BUFFER_CALLBACKW           // callback before each buffer
                            BufferCallback; // is read
    //
    // following variables are filled for BufferCallback.
    //
    ULONG                   BufferSize;
    ULONG                   Filled;
    ULONG                   EventsLost;
    //
    // following needs to be propaged to each buffer
    //

    PEVENT_CALLBACK         EventCallback;  // callback for every event
    ULONG                   IsKernelTrace;  // TRUE for kernel logfile

    PVOID                   Context;        // reserved for internal use
};

struct _EVENT_TRACE_LOGFILEA {
    LPSTR                   LogFileName;    // Logfile Name
    LPSTR                   LoggerName;     // LoggerName
    LONGLONG                CurrentTime;    // timestamp of last event
    ULONG                   BuffersRead;    // buffers read to date
    ULONG                   LogFileMode;    // LogFile Mode.

    EVENT_TRACE             CurrentEvent;   // Current Event from this stream
    TRACE_LOGFILE_HEADER    LogfileHeader;  // logfile header structure
    PEVENT_TRACE_BUFFER_CALLBACKA           // callback before each buffer
                            BufferCallback; // is read

    //
    // following variables are filled for BufferCallback.
    //
    ULONG                   BufferSize;
    ULONG                   Filled;
    ULONG                   EventsLost;
    //
    // following needs to be propaged to each buffer
    //

    PEVENT_CALLBACK         EventCallback;  // callback for every event
    ULONG                   IsKernelTrace;  // TRUE for kernel logfile

    PVOID                   Context;        // reserved for internal use
};

//
// Define generic structures
//

#if defined(_UNICODE) || defined(UNICODE)
#define PEVENT_TRACE_BUFFER_CALLBACK    PEVENT_TRACE_BUFFER_CALLBACKW
#define EVENT_TRACE_LOGFILE             EVENT_TRACE_LOGFILEW
#define PEVENT_TRACE_LOGFILE            PEVENT_TRACE_LOGFILEW
#define KERNEL_LOGGER_NAME              KERNEL_LOGGER_NAMEW
#define GLOBAL_LOGGER_NAME              GLOBAL_LOGGER_NAMEW
#define EVENT_LOGGER_NAME               EVENT_LOGGER_NAMEW

#else

#define PEVENT_TRACE_BUFFER_CALLBACK    PEVENT_TRACE_BUFFER_CALLBACKA
#define EVENT_TRACE_LOGFILE             EVENT_TRACE_LOGFILEA
#define PEVENT_TRACE_LOGFILE            PEVENT_TRACE_LOGFILEA
#define KERNEL_LOGGER_NAME              KERNEL_LOGGER_NAMEA
#define GLOBAL_LOGGER_NAME              GLOBAL_LOGGER_NAMEA
#define EVENT_LOGGER_NAME               EVENT_LOGGER_NAMEA

#endif

#if _MSC_VER >= 1200
#pragma warning(pop)
#endif

#ifdef __cplusplus
extern "C" {
#endif


//
// Logger control APIs
//

//
// Use the routine below to start an event trace session
//

// ULONG
// StartTrace(
//      OUT PTRACEHANDLE TraceHandle,
//      IN LPTSTR InstanceName,
//      IN OUT PEVENT_TRACE_PROPERTIES Properties
//      );


EXTERN_C
ULONG
WMIAPI
StartTraceW(
    __out PTRACEHANDLE TraceHandle,
    __in LPCWSTR InstanceName,
    __inout OUT PEVENT_TRACE_PROPERTIES Properties
    );

EXTERN_C
ULONG
WMIAPI
StartTraceA(
    __out PTRACEHANDLE TraceHandle,
    __in LPCSTR InstanceName,
    __inout PEVENT_TRACE_PROPERTIES Properties
    );

//
// Use the routine below to stop an event trace session
//

//
// ULONG
// StopTrace(
//      IN TRACEHANDLE TraceHandle,
//      IN LPTSTR InstanceName,
//      IN OUT PEVENT_TRACE_PROPERTIES Properties
//      );

EXTERN_C
ULONG
WMIAPI
StopTraceW(
    __in TRACEHANDLE TraceHandle,
    __in_opt LPCWSTR InstanceName,
    __inout PEVENT_TRACE_PROPERTIES Properties
    );

EXTERN_C
ULONG
WMIAPI
StopTraceA(
    __in TRACEHANDLE TraceHandle,
    __in_opt LPCSTR InstanceName,
    __inout PEVENT_TRACE_PROPERTIES Properties
    );


//
// Use the routine below to query the properties of an event trace session
//

// ULONG
// QueryTrace(
//      IN TRACEHANDLE TraceHandle,
//      IN LPTSTR InstanceName,
//      IN OUT PEVENT_TRACE_PROPERTIES Properties
//      );

EXTERN_C
ULONG
WMIAPI
QueryTraceW(
    __in TRACEHANDLE TraceHandle,
    __in_opt LPCWSTR InstanceName,
    __inout PEVENT_TRACE_PROPERTIES Properties
    );

EXTERN_C
ULONG
WMIAPI
QueryTraceA(
    __in TRACEHANDLE TraceHandle,
    __in_opt LPCSTR InstanceName,
    __inout PEVENT_TRACE_PROPERTIES Properties
    );

//
// Use the routine below to update certain properties of an event trace session
//

// ULONG
// UpdateTrace(
//      IN (PTRACEHANDLE TraceHandle,
//      IN LPTSTR InstanceName,
//      IN OUT PEVENT_TRACE_PROPERTIES Properties
//      );

EXTERN_C
ULONG
WMIAPI
UpdateTraceW(
    __in TRACEHANDLE TraceHandle,
    __in_opt LPCWSTR InstanceName,
    __inout PEVENT_TRACE_PROPERTIES Properties
    );

EXTERN_C
ULONG
WMIAPI
UpdateTraceA(
    __in TRACEHANDLE TraceHandle,
    __in_opt LPCSTR InstanceName,
    __inout PEVENT_TRACE_PROPERTIES Properties
    );

//
// Use the routine below to request that all active buffers an event trace
// session be "flushed", or written out.
//

#if (WINVER >= 0x0501)
// ULONG
// FlushTrace(
//      IN TRACEHANDLE TraceHandle,
//      IN LPTSTR InstanceName,
//      IN OUT PEVENT_TRACE_PROPERTIES Properties
//      );

EXTERN_C
ULONG
WMIAPI
FlushTraceW(
    __in TRACEHANDLE TraceHandle,
    __in_opt LPCWSTR InstanceName,
    __inout PEVENT_TRACE_PROPERTIES Properties
    );

EXTERN_C
ULONG
WMIAPI
FlushTraceA(
    __in TRACEHANDLE TraceHandle,
    __in_opt LPCSTR InstanceName,
    __inout PEVENT_TRACE_PROPERTIES Properties
    );

#endif

//
// Generic trace control routine
//
EXTERN_C
ULONG
WMIAPI
ControlTraceW(
    __in TRACEHANDLE TraceHandle,
    __in_opt LPCWSTR InstanceName,
    __inout PEVENT_TRACE_PROPERTIES Properties,
    __in ULONG ControlCode
    );

EXTERN_C
ULONG
WMIAPI
ControlTraceA(
    __in TRACEHANDLE TraceHandle,
    __in_opt LPCSTR InstanceName,
    __inout PEVENT_TRACE_PROPERTIES Properties,
    __in ULONG ControlCode
    );

//
// ULONG
// QueryAllTraces(
//  OUT PEVENT_TRACE_PROPERTIES *PropertyArray,
//  IN ULONG PropertyArrayCount,
//  OUT PULONG LoggerCount
//  );
//

EXTERN_C
ULONG
WMIAPI
QueryAllTracesW(
    __out_ecount(PropertyArrayCount) PEVENT_TRACE_PROPERTIES *PropertyArray,
    __in  ULONG PropertyArrayCount,
    __out PULONG LoggerCount
    );

EXTERN_C
ULONG
WMIAPI
QueryAllTracesA(
    __out_ecount(PropertyArrayCount) PEVENT_TRACE_PROPERTIES *PropertyArray,
    __in  ULONG PropertyArrayCount,
    __out PULONG LoggerCount
    );


//
// Data Provider APIs
//

EXTERN_C
ULONG
WMIAPI
CreateTraceInstanceId(
    __in HANDLE RegHandle,
    __inout PEVENT_INSTANCE_INFO pInstInfo
    );

EXTERN_C
ULONG
WMIAPI
EnableTrace(
    __in ULONG Enable,
    __in ULONG EnableFlag,
    __in ULONG EnableLevel,
    __in LPCGUID ControlGuid,
    __in TRACEHANDLE TraceHandle
    );

//
// Use the routine below to generate and record an event trace
//


EXTERN_C
ULONG
WMIAPI
TraceEvent(
    __in TRACEHANDLE  TraceHandle,
    __inout PEVENT_TRACE_HEADER EventTrace
    );

EXTERN_C
ULONG
WMIAPI
TraceEventInstance(
    __in TRACEHANDLE TraceHandle,
    __in PEVENT_INSTANCE_HEADER EventTrace,
    __in PEVENT_INSTANCE_INFO pInstInfo,
    __in_opt PEVENT_INSTANCE_INFO pParentInstInfo
    );

//
// Use the routine below to register a guid for tracing
//

//
// ULONG
// RegisterTraceGuids(
//  IN WMIDPREQUEST  RequestAddress,
//  IN PVOID         RequestContext,
//  IN LPCGUID       ControlGuid,
//  IN ULONG         GuidCount,
//  IN PTRACE_GUID_REGISTRATION TraceGuidReg,
//  IN LPCTSTR       MofImagePath,
//  IN LPCTSTR       MofResourceName,
//  OUT PTRACEHANDLE RegistrationHandle
//  );
//

EXTERN_C
ULONG
WMIAPI
RegisterTraceGuidsW(
    __in WMIDPREQUEST  RequestAddress,
    __in_opt PVOID     RequestContext,
    __in LPCGUID       ControlGuid,
    __in ULONG         GuidCount,
    __in_ecount_opt(GuidCount) PTRACE_GUID_REGISTRATION TraceGuidReg,
    __in_opt LPCWSTR       MofImagePath,
    __in_opt LPCWSTR       MofResourceName,
    __out PTRACEHANDLE RegistrationHandle
    );

EXTERN_C
ULONG
WMIAPI
RegisterTraceGuidsA(
    __in WMIDPREQUEST  RequestAddress,
    __in_opt PVOID     RequestContext,
    __in LPCGUID       ControlGuid,
    __in ULONG         GuidCount,
    __in_ecount_opt(GuidCount) PTRACE_GUID_REGISTRATION TraceGuidReg,
    __in_opt LPCSTR        MofImagePath,
    __in_opt LPCSTR        MofResourceName,
    __out PTRACEHANDLE RegistrationHandle
    );

#if (WINVER >= 0x0501)
EXTERN_C
ULONG
WMIAPI
EnumerateTraceGuids(
    __inout_ecount(PropertyArrayCount) PTRACE_GUID_PROPERTIES *GuidPropertiesArray,
    __in ULONG PropertyArrayCount,
    __out PULONG GuidCount
    );
#endif

EXTERN_C
ULONG
WMIAPI
UnregisterTraceGuids(
    __in TRACEHANDLE RegistrationHandle
    );

EXTERN_C
TRACEHANDLE
WMIAPI
GetTraceLoggerHandle(
    __in PVOID Buffer
    );

EXTERN_C
UCHAR
WMIAPI
GetTraceEnableLevel(
    __in TRACEHANDLE TraceHandle
    );

EXTERN_C
ULONG
WMIAPI
GetTraceEnableFlags(
    __in TRACEHANDLE TraceHandle
    );

//
// Data Consumer APIs and structures start here
//

//
// TRACEHANDLE
// OpenTrace(
//  IN OUT PEVENT_TRACE_LOGFILE Logfile
//  );
//

EXTERN_C
TRACEHANDLE
WMIAPI
OpenTraceA(
    __inout PEVENT_TRACE_LOGFILEA Logfile
    );

EXTERN_C
TRACEHANDLE
WMIAPI
OpenTraceW(
    __inout PEVENT_TRACE_LOGFILEW Logfile
    );

EXTERN_C
ULONG
WMIAPI
ProcessTrace(
    __in_ecount(HandleCount) PTRACEHANDLE HandleArray,
    __in ULONG HandleCount,
    __in_opt LPFILETIME StartTime,
    __in_opt LPFILETIME EndTime
    );

EXTERN_C
ULONG
WMIAPI
CloseTrace(
    __in TRACEHANDLE TraceHandle
    );

EXTERN_C
ULONG
WMIAPI
SetTraceCallback(
    __in LPCGUID pGuid,
    __in PEVENT_CALLBACK EventCallback
    );

EXTERN_C
ULONG
WMIAPI
RemoveTraceCallback (
    __in LPCGUID pGuid
    );

//
// The routines for tracing Messages follow
//
EXTERN_C
ULONG 
__cdecl
TraceMessage(
    __in TRACEHANDLE  LoggerHandle,
    __in ULONG        MessageFlags,
    __in LPGUID       MessageGuid,
    __in USHORT       MessageNumber,
    ...
);

EXTERN_C
ULONG 
TraceMessageVa(
    __in TRACEHANDLE  LoggerHandle,
    __in ULONG        MessageFlags,
    __in LPGUID       MessageGuid,
    __in USHORT       MessageNumber,
    __in va_list      MessageArgList
);

#ifdef __cplusplus
}       // extern "C"
#endif

//
//
// Define the encoding independent routines
//

#if defined(UNICODE) || defined(_UNICODE)
#define RegisterTraceGuids      RegisterTraceGuidsW
#define StartTrace              StartTraceW
#define ControlTrace            ControlTraceW
#if defined(__TRACE_W2K_COMPATIBLE)
#define StopTrace(a,b,c)        ControlTraceW((a),(b),(c), \
                                        EVENT_TRACE_CONTROL_STOP)
#define QueryTrace(a,b,c)       ControlTraceW((a),(b),(c), \
                                        EVENT_TRACE_CONTROL_QUERY)
#define UpdateTrace(a,b,c)      ControlTraceW((a),(b),(c), \
                                        EVENT_TRACE_CONTROL_UPDATE)
#else
#define StopTrace               StopTraceW
#define QueryTrace              QueryTraceW
#define UpdateTrace             UpdateTraceW
#endif
#if (WINVER >= 0x0501)
#define FlushTrace              FlushTraceW
#endif
#define QueryAllTraces          QueryAllTracesW
#define OpenTrace               OpenTraceW
#else
#define RegisterTraceGuids      RegisterTraceGuidsA
#define StartTrace              StartTraceA
#define ControlTrace            ControlTraceA
#if defined(__TRACE_W2K_COMPATIBLE)
#define StopTrace(a,b,c)        ControlTraceA((a),(b),(c), \
                                        EVENT_TRACE_CONTROL_STOP)
#define QueryTrace(a,b,c)       ControlTraceA((a),(b),(c), \
                                        EVENT_TRACE_CONTROL_QUERY)
#define UpdateTrace(a,b,c)      ControlTraceA((a),(b),(c), \
                                        EVENT_TRACE_CONTROL_UPDATE)
#else
#define StopTrace               StopTraceA
#define QueryTrace              QueryTraceA
#define UpdateTrace             UpdateTraceA
#endif
#if (WINVER >= 0x0501)
#define FlushTrace              FlushTraceA
#endif
#define QueryAllTraces          QueryAllTracesA
#define OpenTrace               OpenTraceA
#endif  // UNICODE

#endif /* _WMIKM_  && _NTDDK_ */

#endif // WINNT
#endif /* _EVNTRACE_ */

