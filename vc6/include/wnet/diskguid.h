/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    diskguid.h

Abstract:

    GPT disk GUIDs.

Revision History:

--*/

//
// GPT Partition Type GUIDs
//
// need these GUIDs outside conditional includes so that user can
//   #include <ntdddisk.h> in precompiled header
//   #include <initguid.h> in a single source file
//   #include <ntdddisk.h> in that source file a second time to instantiate the GUIDs
//

#ifdef DEFINE_GUID

//
// Make sure FAR is defined...
//
#ifndef FAR
#ifdef _WIN32
#define FAR
#else
#define FAR _far
#endif
#endif


//
// Define the GPT partition guids known by disk drivers and volume managers.
//

DEFINE_GUID(PARTITION_ENTRY_UNUSED_GUID,   0x00000000L, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);    // Entry unused
DEFINE_GUID(PARTITION_SYSTEM_GUID,         0xC12A7328L, 0xF81F, 0x11D2, 0xBA, 0x4B, 0x00, 0xA0, 0xC9, 0x3E, 0xC9, 0x3B);    // EFI system partition
DEFINE_GUID(PARTITION_MSFT_RESERVED_GUID,  0xE3C9E316L, 0x0B5C, 0x4DB8, 0x81, 0x7D, 0xF9, 0x2D, 0xF0, 0x02, 0x15, 0xAE);    // Microsoft reserved space                                        
DEFINE_GUID(PARTITION_BASIC_DATA_GUID,     0xEBD0A0A2L, 0xB9E5, 0x4433, 0x87, 0xC0, 0x68, 0xB6, 0xB7, 0x26, 0x99, 0xC7);    // Basic data partition
DEFINE_GUID(PARTITION_LDM_METADATA_GUID,   0x5808C8AAL, 0x7E8F, 0x42E0, 0x85, 0xD2, 0xE1, 0xE9, 0x04, 0x34, 0xCF, 0xB3);    // Logical Disk Manager metadata partition
DEFINE_GUID(PARTITION_LDM_DATA_GUID,       0xAF9B60A0L, 0x1431, 0x4F62, 0xBC, 0x68, 0x33, 0x11, 0x71, 0x4A, 0x69, 0xAD);    // Logical Disk Manager data partition
#endif

