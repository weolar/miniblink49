/*++ BUILD Version: 0009    // Increment this if a change has global effects

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    ntregapi.h

Abstract:

    This module contains the registration apis and related structures,
    in the forms for use with the Nt api set (as opposed to the win api set).

Author:

    Bryan M. Willman (bryanwi) 26-Aug-1991

Revision History:

--*/

#ifndef _NTREGAPI_
#define _NTREGAPI_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
// begin_winnt
//

// begin_ntddk begin_wdm begin_nthal
//
// Registry Specific Access Rights.
//

#define KEY_QUERY_VALUE         (0x0001)
#define KEY_SET_VALUE           (0x0002)
#define KEY_CREATE_SUB_KEY      (0x0004)
#define KEY_ENUMERATE_SUB_KEYS  (0x0008)
#define KEY_NOTIFY              (0x0010)
#define KEY_CREATE_LINK         (0x0020)
#define KEY_WOW64_32KEY         (0x0200)
#define KEY_WOW64_64KEY         (0x0100)
#define KEY_WOW64_RES           (0x0300)

#define KEY_READ                ((STANDARD_RIGHTS_READ       |\
                                  KEY_QUERY_VALUE            |\
                                  KEY_ENUMERATE_SUB_KEYS     |\
                                  KEY_NOTIFY)                 \
                                  &                           \
                                 (~SYNCHRONIZE))


#define KEY_WRITE               ((STANDARD_RIGHTS_WRITE      |\
                                  KEY_SET_VALUE              |\
                                  KEY_CREATE_SUB_KEY)         \
                                  &                           \
                                 (~SYNCHRONIZE))

#define KEY_EXECUTE             ((KEY_READ)                   \
                                  &                           \
                                 (~SYNCHRONIZE))

#define KEY_ALL_ACCESS          ((STANDARD_RIGHTS_ALL        |\
                                  KEY_QUERY_VALUE            |\
                                  KEY_SET_VALUE              |\
                                  KEY_CREATE_SUB_KEY         |\
                                  KEY_ENUMERATE_SUB_KEYS     |\
                                  KEY_NOTIFY                 |\
                                  KEY_CREATE_LINK)            \
                                  &                           \
                                 (~SYNCHRONIZE))

//
// Open/Create Options
//

#define REG_OPTION_RESERVED         (0x00000000L)   // Parameter is reserved

#define REG_OPTION_NON_VOLATILE     (0x00000000L)   // Key is preserved
                                                    // when system is rebooted

#define REG_OPTION_VOLATILE         (0x00000001L)   // Key is not preserved
                                                    // when system is rebooted

#define REG_OPTION_CREATE_LINK      (0x00000002L)   // Created key is a
                                                    // symbolic link

#define REG_OPTION_BACKUP_RESTORE   (0x00000004L)   // open for backup or restore
                                                    // special access rules
                                                    // privilege required

#define REG_OPTION_OPEN_LINK        (0x00000008L)   // Open symbolic link

#define REG_LEGAL_OPTION            \
                (REG_OPTION_RESERVED            |\
                 REG_OPTION_NON_VOLATILE        |\
                 REG_OPTION_VOLATILE            |\
                 REG_OPTION_CREATE_LINK         |\
                 REG_OPTION_BACKUP_RESTORE      |\
                 REG_OPTION_OPEN_LINK)

//
// Key creation/open disposition
//

#define REG_CREATED_NEW_KEY         (0x00000001L)   // New Registry Key created
#define REG_OPENED_EXISTING_KEY     (0x00000002L)   // Existing Key opened

//
// hive format to be used by Reg(Nt)SaveKeyEx
//
#define REG_STANDARD_FORMAT     1
#define REG_LATEST_FORMAT       2
#define REG_NO_COMPRESSION      4

//
// Key restore flags
//

#define REG_WHOLE_HIVE_VOLATILE     (0x00000001L)   // Restore whole hive volatile
#define REG_REFRESH_HIVE            (0x00000002L)   // Unwind changes to last flush
#define REG_NO_LAZY_FLUSH           (0x00000004L)   // Never lazy flush this hive
#define REG_FORCE_RESTORE           (0x00000008L)   // Force the restore process even when we have open handles on subkeys

// end_ntddk end_wdm end_nthal

//
// Notify filter values
//
#define REG_NOTIFY_CHANGE_NAME          (0x00000001L) // Create or delete (child)
#define REG_NOTIFY_CHANGE_ATTRIBUTES    (0x00000002L)
#define REG_NOTIFY_CHANGE_LAST_SET      (0x00000004L) // time stamp
#define REG_NOTIFY_CHANGE_SECURITY      (0x00000008L)

#define REG_LEGAL_CHANGE_FILTER                 \
                (REG_NOTIFY_CHANGE_NAME          |\
                 REG_NOTIFY_CHANGE_ATTRIBUTES    |\
                 REG_NOTIFY_CHANGE_LAST_SET      |\
                 REG_NOTIFY_CHANGE_SECURITY)

//
// end_winnt
//

// Boot condition flags (for NtInitializeRegistry)

#define REG_INIT_BOOT_SM         (0x0000)    // Init called from SM after Autocheck, etc.
#define REG_INIT_BOOT_SETUP      (0x0001)    // Init called from text-mode setup

//
// Values to indicate that a standard Boot has been accepted by the Service Controller.
// The Boot information will be saved by NtInitializeRegistry to a registry ControlSet with an
//
//       ID = [Given Boot Condition Value] - REG_INIT_BOOT_ACCEPTED_BASE
//

#define REG_INIT_BOOT_ACCEPTED_BASE   (0x0002)
#define REG_INIT_BOOT_ACCEPTED_MAX    REG_INIT_BOOT_ACCEPTED_BASE + 999

#define REG_INIT_MAX_VALID_CONDITION  REG_INIT_BOOT_ACCEPTED_MAX

//
// registry limits for value name and key name
//
#define REG_MAX_KEY_VALUE_NAME_LENGTH   32767       // 32k - sanity limit for value name
#define REG_MAX_KEY_NAME_LENGTH         512         // allow for 256 unicode, as promise


// begin_ntddk begin_wdm begin_nthal
//
// Key query structures
//

typedef struct _KEY_BASIC_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable length string
} KEY_BASIC_INFORMATION, *PKEY_BASIC_INFORMATION;

typedef struct _KEY_NODE_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   ClassOffset;
    ULONG   ClassLength;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable length string
//          Class[1];           // Variable length string not declared
} KEY_NODE_INFORMATION, *PKEY_NODE_INFORMATION;

typedef struct _KEY_FULL_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   ClassOffset;
    ULONG   ClassLength;
    ULONG   SubKeys;
    ULONG   MaxNameLen;
    ULONG   MaxClassLen;
    ULONG   Values;
    ULONG   MaxValueNameLen;
    ULONG   MaxValueDataLen;
    WCHAR   Class[1];           // Variable length
} KEY_FULL_INFORMATION, *PKEY_FULL_INFORMATION;

// end_wdm
typedef struct _KEY_NAME_INFORMATION {
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable length string
} KEY_NAME_INFORMATION, *PKEY_NAME_INFORMATION;

typedef struct _KEY_CACHED_INFORMATION {
    LARGE_INTEGER LastWriteTime;
    ULONG   TitleIndex;
    ULONG   SubKeys;
    ULONG   MaxNameLen;
    ULONG   Values;
    ULONG   MaxValueNameLen;
    ULONG   MaxValueDataLen;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable length string
} KEY_CACHED_INFORMATION, *PKEY_CACHED_INFORMATION;

typedef struct _KEY_FLAGS_INFORMATION {
    ULONG   UserFlags;
} KEY_FLAGS_INFORMATION, *PKEY_FLAGS_INFORMATION;

// begin_wdm
typedef enum _KEY_INFORMATION_CLASS {
    KeyBasicInformation,
    KeyNodeInformation,
    KeyFullInformation
// end_wdm
    ,
    KeyNameInformation,
    KeyCachedInformation,
    KeyFlagsInformation
// begin_wdm
} KEY_INFORMATION_CLASS;

typedef struct _KEY_WRITE_TIME_INFORMATION {
    LARGE_INTEGER LastWriteTime;
} KEY_WRITE_TIME_INFORMATION, *PKEY_WRITE_TIME_INFORMATION;

typedef struct _KEY_USER_FLAGS_INFORMATION {
    ULONG   UserFlags;
} KEY_USER_FLAGS_INFORMATION, *PKEY_USER_FLAGS_INFORMATION;

typedef enum _KEY_SET_INFORMATION_CLASS {
    KeyWriteTimeInformation,
    KeyUserFlagsInformation
} KEY_SET_INFORMATION_CLASS;

//
// Value entry query structures
//

typedef struct _KEY_VALUE_BASIC_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable size
} KEY_VALUE_BASIC_INFORMATION, *PKEY_VALUE_BASIC_INFORMATION;

typedef struct _KEY_VALUE_FULL_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   DataOffset;
    ULONG   DataLength;
    ULONG   NameLength;
    WCHAR   Name[1];            // Variable size
//          Data[1];            // Variable size data not declared
} KEY_VALUE_FULL_INFORMATION, *PKEY_VALUE_FULL_INFORMATION;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION {
    ULONG   TitleIndex;
    ULONG   Type;
    ULONG   DataLength;
    UCHAR   Data[1];            // Variable size
} KEY_VALUE_PARTIAL_INFORMATION, *PKEY_VALUE_PARTIAL_INFORMATION;

typedef struct _KEY_VALUE_PARTIAL_INFORMATION_ALIGN64 {
    ULONG   Type;
    ULONG   DataLength;
    UCHAR   Data[1];            // Variable size
} KEY_VALUE_PARTIAL_INFORMATION_ALIGN64, *PKEY_VALUE_PARTIAL_INFORMATION_ALIGN64;

typedef struct _KEY_VALUE_ENTRY {
    PUNICODE_STRING ValueName;
    ULONG           DataLength;
    ULONG           DataOffset;
    ULONG           Type;
} KEY_VALUE_ENTRY, *PKEY_VALUE_ENTRY;

typedef enum _KEY_VALUE_INFORMATION_CLASS {
    KeyValueBasicInformation,
    KeyValueFullInformation,
    KeyValuePartialInformation,
    KeyValueFullInformationAlign64,
    KeyValuePartialInformationAlign64
} KEY_VALUE_INFORMATION_CLASS;


// end_ntddk end_wdm end_nthal
//
// Notify return structures
//

typedef enum _REG_ACTION {
    KeyAdded,
    KeyRemoved,
    KeyModified
} REG_ACTION;

typedef struct _REG_NOTIFY_INFORMATION {
    ULONG           NextEntryOffset;
    REG_ACTION      Action;
    ULONG           KeyLength;
    WCHAR           Key[1];     // Variable size
} REG_NOTIFY_INFORMATION, *PREG_NOTIFY_INFORMATION;


//
// Nt level registry API calls
//

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCreateKey(
    OUT PHANDLE KeyHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes,
    IN ULONG TitleIndex,
    IN PUNICODE_STRING Class OPTIONAL,
    IN ULONG CreateOptions,
    OUT PULONG Disposition OPTIONAL
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeleteKey(
    IN HANDLE KeyHandle
    );


NTSYSCALLAPI
NTSTATUS
NTAPI
NtDeleteValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtEnumerateKey(
    IN HANDLE KeyHandle,
    IN ULONG Index,
    IN KEY_INFORMATION_CLASS KeyInformationClass,
    OUT PVOID KeyInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );


NTSYSCALLAPI
NTSTATUS
NTAPI
NtEnumerateValueKey(
    IN HANDLE KeyHandle,
    IN ULONG Index,
    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    OUT PVOID KeyValueInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtFlushKey(
    IN HANDLE KeyHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtInitializeRegistry(
    IN USHORT BootCondition
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtNotifyChangeKey(
    IN HANDLE KeyHandle,
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG CompletionFilter,
    IN BOOLEAN WatchTree,
    OUT PVOID Buffer,
    IN ULONG BufferSize,
    IN BOOLEAN Asynchronous
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtNotifyChangeMultipleKeys(
    IN HANDLE MasterKeyHandle,  		
    IN ULONG Count,
    IN OBJECT_ATTRIBUTES SlaveObjects[],
    IN HANDLE Event OPTIONAL,
    IN PIO_APC_ROUTINE ApcRoutine OPTIONAL,
    IN PVOID ApcContext OPTIONAL,
    OUT PIO_STATUS_BLOCK IoStatusBlock,
    IN ULONG CompletionFilter,
    IN BOOLEAN WatchTree,
    OUT PVOID Buffer,
    IN ULONG BufferSize,
    IN BOOLEAN Asynchronous
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtLoadKey(
    IN POBJECT_ATTRIBUTES TargetKey,
    IN POBJECT_ATTRIBUTES SourceFile
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtLoadKey2(
    IN POBJECT_ATTRIBUTES TargetKey,
    IN POBJECT_ATTRIBUTES SourceFile,
    IN ULONG Flags
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtOpenKey(
    OUT PHANDLE KeyHandle,
    IN ACCESS_MASK DesiredAccess,
    IN POBJECT_ATTRIBUTES ObjectAttributes
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryKey(
    IN HANDLE KeyHandle,
    IN KEY_INFORMATION_CLASS KeyInformationClass,
    OUT PVOID KeyInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName,
    IN KEY_VALUE_INFORMATION_CLASS KeyValueInformationClass,
    OUT PVOID KeyValueInformation,
    IN ULONG Length,
    OUT PULONG ResultLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryMultipleValueKey(
    IN HANDLE KeyHandle,
    IN OUT PKEY_VALUE_ENTRY ValueEntries,
    IN ULONG EntryCount,
    OUT PVOID ValueBuffer,
    IN OUT PULONG BufferLength,
    OUT OPTIONAL PULONG RequiredBufferLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtReplaceKey(
    IN POBJECT_ATTRIBUTES NewFile,
    IN HANDLE             TargetHandle,
    IN POBJECT_ATTRIBUTES OldFile
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtRenameKey(
    IN HANDLE           KeyHandle,
    IN PUNICODE_STRING  NewName
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCompactKeys(
    IN ULONG Count,
    IN HANDLE KeyArray[]
            );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtCompressKey(
    IN HANDLE Key
            );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtRestoreKey(
    IN HANDLE KeyHandle,
    IN HANDLE FileHandle,
    IN ULONG  Flags
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSaveKey(
    IN HANDLE KeyHandle,
    IN HANDLE FileHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSaveKeyEx(
    IN HANDLE KeyHandle,
    IN HANDLE FileHandle,
    IN ULONG  Format
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSaveMergedKeys(
    IN HANDLE HighPrecedenceKeyHandle,
    IN HANDLE LowPrecedenceKeyHandle,
    IN HANDLE FileHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetValueKey(
    IN HANDLE KeyHandle,
    IN PUNICODE_STRING ValueName,
    IN ULONG TitleIndex OPTIONAL,
    IN ULONG Type,
    IN PVOID Data,
    IN ULONG DataSize
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtUnloadKey(
    IN POBJECT_ATTRIBUTES TargetKey
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtUnloadKeyEx(
    IN POBJECT_ATTRIBUTES TargetKey,
    IN HANDLE Event OPTIONAL
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtSetInformationKey(
    IN HANDLE KeyHandle,
    IN KEY_SET_INFORMATION_CLASS KeySetInformationClass,
    IN PVOID KeySetInformation,
    IN ULONG KeySetInformationLength
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtQueryOpenSubKeys(
    IN POBJECT_ATTRIBUTES TargetKey,
    OUT PULONG  HandleCount
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtLockRegistryKey(
    IN HANDLE           KeyHandle
    );

NTSYSCALLAPI
NTSTATUS
NTAPI
NtLockProductActivationKeys(
    ULONG   *pPrivateVer,
    ULONG   *pIsSafeMode
    );

#ifdef __cplusplus
}
#endif

#endif // _NTREGAPI_

