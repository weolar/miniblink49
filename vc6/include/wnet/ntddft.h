/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    ntddft.h

Abstract:

    This file contains NTFT definitions and prototypes.

Authors:

    Bob Rinne
    Mike Glass

Notes:

Revision History:

--*/

#ifndef _NTDDFT_
#define _NTDDFT_

#if _MSC_VER > 1000
#pragma once
#endif

#include <ntdskreg.h>

#ifdef __cplusplus
extern "C" {
#endif

//
// Device Control Codes
//

#define FTTYPE  ((ULONG)'f')

#define FT_INITIALIZE_SET CTL_CODE(FTTYPE, 0, METHOD_BUFFERED,   FILE_ANY_ACCESS)
#define FT_REGENERATE     CTL_CODE(FTTYPE, 1, METHOD_BUFFERED,   FILE_ANY_ACCESS)
#define FT_CONFIGURE      CTL_CODE(FTTYPE, 2, METHOD_NEITHER,    FILE_ANY_ACCESS)
#define FT_VERIFY         CTL_CODE(FTTYPE, 3, METHOD_BUFFERED,   FILE_ANY_ACCESS)
#define FT_SECONDARY_READ CTL_CODE(FTTYPE, 4, METHOD_OUT_DIRECT, FILE_READ_ACCESS)
#define FT_PRIMARY_READ   CTL_CODE(FTTYPE, 5, METHOD_OUT_DIRECT, FILE_READ_ACCESS)
#define FT_BALANCED_READ_MODE    CTL_CODE(FTTYPE, 6, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FT_SYNC_REDUNDANT_COPY   CTL_CODE(FTTYPE, 7, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FT_SEQUENTIAL_WRITE_MODE CTL_CODE(FTTYPE, 8, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FT_PARALLEL_WRITE_MODE   CTL_CODE(FTTYPE, 9, METHOD_NEITHER,  FILE_ANY_ACCESS)
#define FT_QUERY_SET_STATE       CTL_CODE(FTTYPE,10, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FT_CLUSTER_SET_MEMBER_STATE CTL_CODE(FTTYPE,11, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define FT_CLUSTER_GET_MEMBER_STATE CTL_CODE(FTTYPE,12, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Used as input parameters to the SECONDARY and PRIMARY read device controls
//

typedef struct _FT_SPECIAL_READ {
    LARGE_INTEGER ByteOffset;
    ULONG         Length;
} FT_SPECIAL_READ, *PFT_SPECIAL_READ;

//
// Used to find a partition in the system.
//

typedef struct _FT_CONTROL_BLOCK {
    USHORT     Type;
    USHORT     FtGroup;
} FT_CONTROL_BLOCK, *PFT_CONTROL_BLOCK;

//
// FT SYNC primary from secondary IOCTL
//

typedef struct _FT_SYNC_INFORMATION {
    LARGE_INTEGER ByteOffset;
    LARGE_INTEGER ByteCount;
} FT_SYNC_INFORMATION, *PFT_SYNC_INFORMATION;

//
// Define for registry information.
//

#define DISK_REGISTRY_KEY  "\\REGISTRY\\MACHINE\\SYSTEM\\DISK"

//
// Define for value name for registry information.
//

#define DISK_REGISTRY_VALUE "Information"

//
// Structures for config registry.
//

//
// The header for the registry information structure.
//

#include "pshpack4.h"
typedef struct _DISK_CONFIG_HEADER {

    ULONG Version;
    ULONG CheckSum;
    BOOLEAN DirtyShutdown;
    UCHAR Reserved[3];
    ULONG DiskInformationOffset;
    ULONG DiskInformationSize;
    ULONG FtInformationOffset;
    ULONG FtInformationSize;
    ULONG FtStripeWidth;
    ULONG FtPoolSize;
    ULONG NameOffset;  // Reserved for future.
    ULONG NameSize;    // Reserved for future.

} DISK_CONFIG_HEADER, *PDISK_CONFIG_HEADER;

#define DISK_INFORMATION_VERSION    0x03

//
// FT_STATE is an enumerated field that describes the state of the FT
// volume as a whole.
//

typedef enum _FT_STATE {

    FtStateOk,      // set is complete
    FtHasOrphan,    // set is running, but one member is turned off
    FtDisabled,     // set has been turned off - it cannot be used
    FtRegenerating, // member of set is being generated from check data
    FtInitializing, // set is being initialized
    FtCheckParity,  // check data is not in sync
    FtNoCheckData   // parity has not been initialized

} FT_STATE, *PFT_STATE;

//
// State information for an FT set.  The partition state enum is used to
// specify the state for the complete set.
//

typedef struct _FT_SET_INFORMATION {
    ULONG        NumberOfMembers;
    FT_TYPE      Type;
    FT_STATE     SetState;
} FT_SET_INFORMATION, *PFT_SET_INFORMATION;

//
// The description information for an FT component member.
//

typedef struct _FT_MEMBER_DESCRIPTION {

    USHORT        State;
    USHORT        ReservedShort;
    ULONG         Signature;
    ULONG         OffsetToPartitionInfo;
    ULONG         LogicalNumber;

} FT_MEMBER_DESCRIPTION, *PFT_MEMBER_DESCRIPTION;

//
// Definitions for FT volume state.
//

//
// When this state is set, FT assumes at boot time that the volume
// must be initialized and starts a system thread of execution to
// perform the initialization work.
//

#define FT_VOLUME_INITIALIZING 0x0001
#define FT_VOLUME_REGENERATING 0x0002

//
// The description information for an FT component.
//

typedef struct _FT_DESCRIPTION {

    USHORT   NumberOfMembers;
    USHORT   Type;
    ULONG    Reserved;

    //
    // The volume state will indicate which member is being regenerated
    //

    FT_STATE FtVolumeState;

    //
    // Array for NumberOfMembers
    //

    FT_MEMBER_DESCRIPTION FtMemberDescription[1];

} FT_DESCRIPTION, *PFT_DESCRIPTION;

//
// Header for all FT information.
//

typedef struct _FT_REGISTRY {

    USHORT NumberOfComponents;
    USHORT ReservedShort;

    //
    // An array of NumberOfComponents
    //

    FT_DESCRIPTION FtDescription[1];

} FT_REGISTRY, *PFT_REGISTRY;
#include "poppack.h"

#ifdef __cplusplus
}
#endif

#endif  // _NTDDFT_

