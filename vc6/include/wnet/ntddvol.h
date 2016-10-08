/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    ntddvol.h

Abstract:

    This header file defines the IOCTLs supported by Windows NT volumes.

Author:

    Norbert Kusters 15-April-1998

Notes:

Revision History:

--*/

#ifndef _NTDDVOL_
#define _NTDDVOL_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef IOCTL_VOLUME_BASE
// begin_winioctl

//
// These IOCTLs are handled by hard disk volumes.
//

#define IOCTL_VOLUME_BASE   ((ULONG) 'V')

#define IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS    CTL_CODE(IOCTL_VOLUME_BASE, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VOLUME_IS_CLUSTERED               CTL_CODE(IOCTL_VOLUME_BASE, 12, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Disk extent definition.
//

typedef struct _DISK_EXTENT {
    ULONG           DiskNumber;
    LARGE_INTEGER   StartingOffset;
    LARGE_INTEGER   ExtentLength;
} DISK_EXTENT, *PDISK_EXTENT;

//
// Output structure for IOCTL_VOLUME_GET_VOLUME_DISK_EXTENTS.
//

typedef struct _VOLUME_DISK_EXTENTS {
    ULONG       NumberOfDiskExtents;
    DISK_EXTENT Extents[1];
} VOLUME_DISK_EXTENTS, *PVOLUME_DISK_EXTENTS;

// end_winioctl
#endif

//
// These IOCTLs are to support cluster services.
//

#define IOCTL_VOLUME_SUPPORTS_ONLINE_OFFLINE    CTL_CODE(IOCTL_VOLUME_BASE, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VOLUME_ONLINE                     CTL_CODE(IOCTL_VOLUME_BASE, 2, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_VOLUME_OFFLINE                    CTL_CODE(IOCTL_VOLUME_BASE, 3, METHOD_BUFFERED, FILE_READ_ACCESS | FILE_WRITE_ACCESS)
#define IOCTL_VOLUME_IS_OFFLINE                 CTL_CODE(IOCTL_VOLUME_BASE, 4, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VOLUME_IS_IO_CAPABLE              CTL_CODE(IOCTL_VOLUME_BASE, 5, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VOLUME_QUERY_FAILOVER_SET         CTL_CODE(IOCTL_VOLUME_BASE, 6, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Output structure for IOCTL_VOLUME_QUERY_FAILOVER_SET.
//

typedef struct _VOLUME_FAILOVER_SET {
    ULONG   NumberOfDisks;
    ULONG   DiskNumbers[1];
} VOLUME_FAILOVER_SET, *PVOLUME_FAILOVER_SET;

//
// More IOCTLs for hard disk volumes.
//

#define IOCTL_VOLUME_QUERY_VOLUME_NUMBER        CTL_CODE(IOCTL_VOLUME_BASE, 7, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VOLUME_LOGICAL_TO_PHYSICAL        CTL_CODE(IOCTL_VOLUME_BASE, 8, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VOLUME_PHYSICAL_TO_LOGICAL        CTL_CODE(IOCTL_VOLUME_BASE, 9, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VOLUME_IS_PARTITION               CTL_CODE(IOCTL_VOLUME_BASE, 10, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VOLUME_READ_PLEX                  CTL_CODE(IOCTL_VOLUME_BASE, 11, METHOD_OUT_DIRECT, FILE_READ_ACCESS)

//
// Output structure for IOCTL_VOLUME_QUERY_VOLUME_NUMBER.  If the volume
// manager name is less than 8 characters then it should be padded with
// blanks (0x20).
//

typedef struct _VOLUME_NUMBER {
    ULONG   VolumeNumber;
    WCHAR   VolumeManagerName[8];
} VOLUME_NUMBER, *PVOLUME_NUMBER;

//
// Input structure for IOCTL_VOLUME_LOGICAL_TO_PHYSICAL.
// Output structure for IOCTL_VOLUME_PHYSICAL_TO_LOGICAL.
//

typedef struct _VOLUME_LOGICAL_OFFSET {
    LONGLONG    LogicalOffset;
} VOLUME_LOGICAL_OFFSET, *PVOLUME_LOGICAL_OFFSET;

//
// Input structure for IOCTL_VOLUME_PHYSICAL_TO_LOGICAL.
//

typedef struct _VOLUME_PHYSICAL_OFFSET {
    ULONG       DiskNumber;
    LONGLONG    Offset;
} VOLUME_PHYSICAL_OFFSET, *PVOLUME_PHYSICAL_OFFSET;

//
// Output structure for IOCTL_VOLUME_LOGICAL_TO_PHYSICAL.
//

typedef struct _VOLUME_PHYSICAL_OFFSETS {
    ULONG                   NumberOfPhysicalOffsets;
    VOLUME_PHYSICAL_OFFSET  PhysicalOffset[1];
} VOLUME_PHYSICAL_OFFSETS, *PVOLUME_PHYSICAL_OFFSETS;

//
// Input structure for IOCTL_VOLUME_READ_PLEX.
//

typedef struct _VOLUME_READ_PLEX_INPUT {
    LARGE_INTEGER   ByteOffset;
    ULONG           Length;
    ULONG           PlexNumber;
} VOLUME_READ_PLEX_INPUT, *PVOLUME_READ_PLEX_INPUT;

//
// IOCTLs for setting and getting GPT attributes on volumes.
//

#define IOCTL_VOLUME_SET_GPT_ATTRIBUTES CTL_CODE(IOCTL_VOLUME_BASE, 13, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_VOLUME_GET_GPT_ATTRIBUTES CTL_CODE(IOCTL_VOLUME_BASE, 14, METHOD_BUFFERED, FILE_ANY_ACCESS)

//
// Input structure for IOCTL_VOLUME_SET_GPT_ATTRIBUTES.
//

typedef struct _VOLUME_SET_GPT_ATTRIBUTES_INFORMATION {
    ULONGLONG   GptAttributes;
    BOOLEAN     RevertOnClose;
    BOOLEAN     ApplyToAllConnectedVolumes;
    USHORT      Reserved1;                  // Must be 0.
    ULONG       Reserved2;                  // Must be 0.
} VOLUME_SET_GPT_ATTRIBUTES_INFORMATION, *PVOLUME_SET_GPT_ATTRIBUTES_INFORMATION;

//
// Output structure for IOCTL_VOLUME_GET_GPT_ATTRIBUTES.
//

typedef struct _VOLUME_GET_GPT_ATTRIBUTES_INFORMATION {
    ULONGLONG   GptAttributes;
} VOLUME_GET_GPT_ATTRIBUTES_INFORMATION, *PVOLUME_GET_GPT_ATTRIBUTES_INFORMATION;

#ifdef __cplusplus
}
#endif

#endif  // _NTDDVOL_

