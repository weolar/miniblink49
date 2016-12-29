/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) 1994-1999  Microsoft Corporation

Module Name:

    psapi.h

Abstract:

    Include file for APIs provided by PSAPI.DLL

Author:

    Richard Shupak   [richards]  06-Jan-1994

Revision History:

--*/

#ifndef _PSAPI_H_
#define _PSAPI_H_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define LIST_MODULES_DEFAULT 0x0  // This is the default one app would get without any flag.
#define LIST_MODULES_32BIT   0x01  // list 32bit modules in the target process.
#define LIST_MODULES_64BIT   0x02  // list all 64bit modules. 32bit exe will be stripped off.

// list all the modules
#define LIST_MODULES_ALL   (LIST_MODULES_32BIT | LIST_MODULES_64BIT)

//
// Give teams a choice of using a downlevel version of psapi.h for an OS versions.
// Teams can set C_DEFINES=$(C_DEFINES) -DPSAPI_VERSION=1 for downlevel psapi
// on windows 7 and higher.  We found that test code needs this capability.
//
#ifndef PSAPI_VERSION
#if (NTDDI_VERSION >= NTDDI_WIN7)
#define PSAPI_VERSION 2
#else
#define PSAPI_VERSION 1
#endif
#endif

#if (PSAPI_VERSION > 1)
#define EnumProcesses               K32EnumProcesses
#define EnumProcessModules          K32EnumProcessModules
#define EnumProcessModulesEx        K32EnumProcessModulesEx
#define GetModuleBaseNameA          K32GetModuleBaseNameA
#define GetModuleBaseNameW          K32GetModuleBaseNameW
#define GetModuleFileNameExA        K32GetModuleFileNameExA
#define GetModuleFileNameExW        K32GetModuleFileNameExW
#define GetModuleInformation        K32GetModuleInformation
#define EmptyWorkingSet             K32EmptyWorkingSet
#define QueryWorkingSet             K32QueryWorkingSet
#define QueryWorkingSetEx           K32QueryWorkingSetEx
#define InitializeProcessForWsWatch K32InitializeProcessForWsWatch
#define GetWsChanges                K32GetWsChanges
#define GetWsChangesEx              K32GetWsChangesEx
#define GetMappedFileNameW          K32GetMappedFileNameW
#define GetMappedFileNameA          K32GetMappedFileNameA
#define EnumDeviceDrivers           K32EnumDeviceDrivers
#define GetDeviceDriverBaseNameA    K32GetDeviceDriverBaseNameA
#define GetDeviceDriverBaseNameW    K32GetDeviceDriverBaseNameW
#define GetDeviceDriverFileNameA    K32GetDeviceDriverFileNameA
#define GetDeviceDriverFileNameW    K32GetDeviceDriverFileNameW
#define GetProcessMemoryInfo        K32GetProcessMemoryInfo
#define GetPerformanceInfo          K32GetPerformanceInfo
#define EnumPageFilesW              K32EnumPageFilesW
#define EnumPageFilesA              K32EnumPageFilesA
#define GetProcessImageFileNameA    K32GetProcessImageFileNameA
#define GetProcessImageFileNameW    K32GetProcessImageFileNameW
#endif

BOOL
WINAPI
EnumProcesses (
    __out_bcount(cb) DWORD * lpidProcess,
    __in DWORD cb,
    __out LPDWORD lpcbNeeded
    );

BOOL
WINAPI
EnumProcessModules(
    __in  HANDLE hProcess,
    __out_bcount(cb) HMODULE *lphModule,
    __in  DWORD cb,
    __out LPDWORD lpcbNeeded
    );

BOOL
WINAPI
EnumProcessModulesEx(
    __in  HANDLE hProcess,
    __out_bcount(cb)  HMODULE *lphModule,
    __in  DWORD cb,
    __out  LPDWORD lpcbNeeded,
    __in  DWORD dwFilterFlag
    );

DWORD
WINAPI
GetModuleBaseNameA(
    __in HANDLE hProcess,
    __in_opt HMODULE hModule,
    __out_ecount(nSize) LPSTR lpBaseName,
    __in DWORD nSize
    );

DWORD
WINAPI
GetModuleBaseNameW(
    __in HANDLE hProcess,
    __in_opt HMODULE hModule,
    __out_ecount(nSize) LPWSTR lpBaseName,
    __in DWORD nSize
    );

#ifdef UNICODE
#define GetModuleBaseName  GetModuleBaseNameW
#else
#define GetModuleBaseName  GetModuleBaseNameA
#endif // !UNICODE


DWORD
WINAPI
GetModuleFileNameExA(
    __in HANDLE hProcess,
    __in_opt HMODULE hModule,
    __out_ecount(nSize) LPSTR lpFilename,
    __in DWORD nSize
    );

DWORD
WINAPI
GetModuleFileNameExW(
    __in HANDLE hProcess,
    __in_opt HMODULE hModule,
    __out_ecount(nSize) LPWSTR lpFilename,
    __in DWORD nSize
    );

#ifdef UNICODE
#define GetModuleFileNameEx  GetModuleFileNameExW
#else
#define GetModuleFileNameEx  GetModuleFileNameExA
#endif // !UNICODE


typedef struct _MODULEINFO {
    LPVOID lpBaseOfDll;
    DWORD SizeOfImage;
    LPVOID EntryPoint;
} MODULEINFO, *LPMODULEINFO;


BOOL
WINAPI
GetModuleInformation(
    __in HANDLE hProcess,
    __in HMODULE hModule,
    __out LPMODULEINFO lpmodinfo,
    __in DWORD cb
    );


BOOL
WINAPI
EmptyWorkingSet(
    __in HANDLE hProcess
    );

//
// Working set information structures. All non-specified bits are reserved.
//

#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4201)   // unnamed struct
#pragma warning(disable:4214)   // bit fields other than int

typedef union _PSAPI_WORKING_SET_BLOCK {
    ULONG_PTR Flags;
    struct {
        ULONG_PTR Protection : 5;
        ULONG_PTR ShareCount : 3;
        ULONG_PTR Shared : 1;
        ULONG_PTR Reserved : 3;
#if defined(_WIN64)
        ULONG_PTR VirtualPage : 52;
#else
        ULONG_PTR VirtualPage : 20;
#endif
    };
} PSAPI_WORKING_SET_BLOCK, *PPSAPI_WORKING_SET_BLOCK;

typedef struct _PSAPI_WORKING_SET_INFORMATION {
    ULONG_PTR NumberOfEntries;
    PSAPI_WORKING_SET_BLOCK WorkingSetInfo[1];
} PSAPI_WORKING_SET_INFORMATION, *PPSAPI_WORKING_SET_INFORMATION;

typedef union _PSAPI_WORKING_SET_EX_BLOCK {
    ULONG_PTR Flags;
    struct {
        ULONG_PTR Valid : 1;        // The following fields are valid only if this bit is set
        ULONG_PTR ShareCount : 3;
        ULONG_PTR Win32Protection : 11;
        ULONG_PTR Shared : 1;
        ULONG_PTR Node : 6;
        ULONG_PTR Locked : 1;
        ULONG_PTR LargePage : 1;
    };
} PSAPI_WORKING_SET_EX_BLOCK, *PPSAPI_WORKING_SET_EX_BLOCK;

typedef struct _PSAPI_WORKING_SET_EX_INFORMATION {
    PVOID VirtualAddress;
    PSAPI_WORKING_SET_EX_BLOCK VirtualAttributes;
} PSAPI_WORKING_SET_EX_INFORMATION, *PPSAPI_WORKING_SET_EX_INFORMATION;

#if _MSC_VER >= 1200
#pragma warning(pop)
#else
#pragma warning(default:4214)
#pragma warning(default:4201)
#endif

BOOL
WINAPI
QueryWorkingSet(
    __in HANDLE hProcess,
    __out_bcount(cb) PVOID pv,
    __in DWORD cb
    );

BOOL
WINAPI
QueryWorkingSetEx(
    __in HANDLE hProcess,
    __out_bcount(cb) PVOID pv,
    __in DWORD cb
    );

BOOL
WINAPI
InitializeProcessForWsWatch(
    __in HANDLE hProcess
    );

typedef struct _PSAPI_WS_WATCH_INFORMATION {
    LPVOID FaultingPc;
    LPVOID FaultingVa;
} PSAPI_WS_WATCH_INFORMATION, *PPSAPI_WS_WATCH_INFORMATION;

typedef struct _PSAPI_WS_WATCH_INFORMATION_EX {
    PSAPI_WS_WATCH_INFORMATION BasicInfo;
    ULONG_PTR FaultingThreadId;
    ULONG_PTR Flags;    // Reserved
} PSAPI_WS_WATCH_INFORMATION_EX, *PPSAPI_WS_WATCH_INFORMATION_EX;

BOOL
WINAPI
GetWsChanges(
    __in HANDLE hProcess,
    __out_bcount(cb) PPSAPI_WS_WATCH_INFORMATION lpWatchInfo,
    __in DWORD cb
    );

BOOL
WINAPI
GetWsChangesEx(
    __in HANDLE hProcess,
    __out_bcount_part(*cb, *cb) PPSAPI_WS_WATCH_INFORMATION_EX lpWatchInfoEx,
    __inout PDWORD cb
    );

DWORD
WINAPI
GetMappedFileNameW (
    __in HANDLE hProcess,
    __in LPVOID lpv,
    __out_ecount(nSize) LPWSTR lpFilename,
    __in DWORD nSize
    );

DWORD
WINAPI
GetMappedFileNameA (
    __in HANDLE hProcess,
    __in LPVOID lpv,
    __out_ecount(nSize) LPSTR lpFilename,
    __in DWORD nSize
    );

#ifdef UNICODE
#define GetMappedFileName  GetMappedFileNameW
#else
#define GetMappedFileName  GetMappedFileNameA
#endif // !UNICODE

BOOL
WINAPI
EnumDeviceDrivers (
    __out_bcount(cb) LPVOID *lpImageBase,
    __in DWORD cb,
    __out LPDWORD lpcbNeeded
    );


DWORD
WINAPI
GetDeviceDriverBaseNameA (
    __in LPVOID ImageBase,
    __out_ecount(nSize) LPSTR lpFilename,
    __in DWORD nSize
    );

DWORD
WINAPI
GetDeviceDriverBaseNameW (
    __in LPVOID ImageBase,
    __out_ecount(nSize) LPWSTR lpBaseName,
    __in DWORD nSize
    );

#ifdef UNICODE
#define GetDeviceDriverBaseName  GetDeviceDriverBaseNameW
#else
#define GetDeviceDriverBaseName  GetDeviceDriverBaseNameA
#endif // !UNICODE


DWORD
WINAPI
GetDeviceDriverFileNameA (
    __in LPVOID ImageBase,
    __out_ecount(nSize) LPSTR lpFilename,
    __in DWORD nSize
    );

DWORD
WINAPI
GetDeviceDriverFileNameW (
    __in LPVOID ImageBase,
    __out_ecount(nSize) LPWSTR lpFilename,
    __in DWORD nSize
    );

#ifdef UNICODE
#define GetDeviceDriverFileName  GetDeviceDriverFileNameW
#else
#define GetDeviceDriverFileName  GetDeviceDriverFileNameA
#endif // !UNICODE

// Structure for GetProcessMemoryInfo()

typedef struct _PROCESS_MEMORY_COUNTERS {
    DWORD cb;
    DWORD PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
} PROCESS_MEMORY_COUNTERS;
typedef PROCESS_MEMORY_COUNTERS *PPROCESS_MEMORY_COUNTERS;

#if (_WIN32_WINNT >= 0x0501)

typedef struct _PROCESS_MEMORY_COUNTERS_EX {
    DWORD cb;
    DWORD PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivateUsage;
} PROCESS_MEMORY_COUNTERS_EX;
typedef PROCESS_MEMORY_COUNTERS_EX *PPROCESS_MEMORY_COUNTERS_EX;

#endif

BOOL
WINAPI
GetProcessMemoryInfo(
    HANDLE Process,
    PPROCESS_MEMORY_COUNTERS ppsmemCounters,
    DWORD cb
    );

typedef struct _PERFORMANCE_INFORMATION {
    DWORD cb;
    SIZE_T CommitTotal;
    SIZE_T CommitLimit;
    SIZE_T CommitPeak;
    SIZE_T PhysicalTotal;
    SIZE_T PhysicalAvailable;
    SIZE_T SystemCache;
    SIZE_T KernelTotal;
    SIZE_T KernelPaged;
    SIZE_T KernelNonpaged;
    SIZE_T PageSize;
    DWORD HandleCount;
    DWORD ProcessCount;
    DWORD ThreadCount;
} PERFORMANCE_INFORMATION, *PPERFORMANCE_INFORMATION, PERFORMACE_INFORMATION, *PPERFORMACE_INFORMATION;

BOOL
WINAPI
GetPerformanceInfo (
    PPERFORMANCE_INFORMATION pPerformanceInformation,
    DWORD cb
    );

typedef struct _ENUM_PAGE_FILE_INFORMATION {
    DWORD cb;
    DWORD Reserved;
    SIZE_T TotalSize;
    SIZE_T TotalInUse;
    SIZE_T PeakUsage;
} ENUM_PAGE_FILE_INFORMATION, *PENUM_PAGE_FILE_INFORMATION;

typedef BOOL (*PENUM_PAGE_FILE_CALLBACKW) (LPVOID pContext, PENUM_PAGE_FILE_INFORMATION pPageFileInfo, LPCWSTR lpFilename);

typedef BOOL (*PENUM_PAGE_FILE_CALLBACKA) (LPVOID pContext, PENUM_PAGE_FILE_INFORMATION pPageFileInfo, LPCSTR lpFilename);

BOOL
WINAPI
EnumPageFilesW (
    PENUM_PAGE_FILE_CALLBACKW pCallBackRoutine,
    LPVOID pContext
    );

BOOL
WINAPI
EnumPageFilesA (
    PENUM_PAGE_FILE_CALLBACKA pCallBackRoutine,
    LPVOID pContext
    );

#ifdef UNICODE
#define PENUM_PAGE_FILE_CALLBACK PENUM_PAGE_FILE_CALLBACKW
#define EnumPageFiles EnumPageFilesW
#else
#define PENUM_PAGE_FILE_CALLBACK PENUM_PAGE_FILE_CALLBACKA
#define EnumPageFiles EnumPageFilesA
#endif // !UNICODE

DWORD
WINAPI
GetProcessImageFileNameA (
    __in HANDLE hProcess,
    __out_ecount(nSize) LPSTR lpImageFileName,
    __in DWORD nSize
    );

DWORD
WINAPI
GetProcessImageFileNameW (
    __in HANDLE hProcess,
    __out_ecount(nSize) LPWSTR lpImageFileName,
    __in DWORD nSize
    );

#ifdef UNICODE
#define GetProcessImageFileName  GetProcessImageFileNameW
#else
#define GetProcessImageFileName  GetProcessImageFileNameA
#endif // !UNICODE

#ifdef __cplusplus
}
#endif

#endif

