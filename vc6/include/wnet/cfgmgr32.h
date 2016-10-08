/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    cfgmgr32.h

Abstract:

    This module contains the user APIs for the Configuration Manager,
    along with any public data structures needed to call these APIs.

--*/

#ifndef _CFGMGR32_H_
#define _CFGMGR32_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include <cfg.h>

#ifndef GUID_DEFINED
#include <guiddef.h>
#endif /* GUID_DEFINED */

#ifdef __cplusplus
extern "C" {
#endif

#if !defined (_CFGMGR32_)
#define CMAPI     DECLSPEC_IMPORT
#else
#define CMAPI
#endif

typedef  CONST VOID *PCVOID;



//--------------------------------------------------------------
// General size definitions
//--------------------------------------------------------------

#define MAX_DEVICE_ID_LEN     200
#define MAX_DEVNODE_ID_LEN    MAX_DEVICE_ID_LEN

#define MAX_GUID_STRING_LEN   39          // 38 chars + terminator null
#define MAX_CLASS_NAME_LEN    32
#define MAX_PROFILE_LEN       80

#define MAX_CONFIG_VALUE      9999
#define MAX_INSTANCE_VALUE    9999

#define MAX_MEM_REGISTERS     9     // Win95 compatibility--not applicable to 32-bit ConfigMgr
#define MAX_IO_PORTS          20    // Win95 compatibility--not applicable to 32-bit ConfigMgr
#define MAX_IRQS              7     // Win95 compatibility--not applicable to 32-bit ConfigMgr
#define MAX_DMA_CHANNELS      7     // Win95 compatibility--not applicable to 32-bit ConfigMgr

#define DWORD_MAX             0xFFFFFFFF
#define DWORDLONG_MAX         0xFFFFFFFFFFFFFFFF

#define CONFIGMG_VERSION      0x0400


//--------------------------------------------------------------
// Data types
//--------------------------------------------------------------


//
// Work around weirdness with Win32 typedef...
//
#ifdef NT_INCLUDED

//
// __int64 is only supported by 2.0 and later midl.
// __midl is set by the 2.0 midl and not by 1.0 midl.
//
#if (!defined(MIDL_PASS) || defined(__midl)) && (!defined(_M_IX86) || (defined(_INTEGRAL_MAX_BITS) && _INTEGRAL_MAX_BITS >= 64))
typedef unsigned __int64 DWORDLONG;
#else
typedef double DWORDLONG;
#endif
typedef DWORDLONG *PDWORDLONG;

#endif /* NT_INCLUDED */


//
// Standardized Return Value data type
//
typedef DWORD        RETURN_TYPE;
typedef RETURN_TYPE  CONFIGRET;

//
// Device Instance Handle data type
//
typedef DWORD       DEVNODE, DEVINST;
typedef DEVNODE    *PDEVNODE, *PDEVINST;

//
// Device Instance Identifier data type
// The device instance ID specifies the registry path, relative to the
// Enum key , for a device instance.  For example:  \Root\*PNP0500\0000.
//
typedef __nullterminated CHAR *DEVNODEID_A, *DEVINSTID_A; // Device ID ANSI name.
typedef __nullterminated WCHAR *DEVNODEID_W, *DEVINSTID_W; // Device ID Unicode name.
#ifdef UNICODE
typedef DEVNODEID_W DEVNODEID;
typedef DEVINSTID_W DEVINSTID;
#else
typedef DEVNODEID_A DEVNODEID;
typedef DEVINSTID_A DEVINSTID;
#endif

//
// Logical Configuration Handle data type
//
typedef DWORD_PTR    LOG_CONF;
typedef LOG_CONF    *PLOG_CONF;

//
// Resource Descriptor Handle data type
//
typedef DWORD_PTR    RES_DES;
typedef RES_DES     *PRES_DES;

//
// Resource ID data type (may take any of the ResType_* values)
//
typedef ULONG        RESOURCEID;
typedef RESOURCEID  *PRESOURCEID;

//
// Priority data type (may take any of the LCPRI_* values)
//
typedef ULONG        PRIORITY;
typedef PRIORITY     *PPRIORITY;

//
// Range List Handle data type
//
typedef DWORD_PTR          RANGE_LIST;
typedef RANGE_LIST        *PRANGE_LIST;

//
// Range Element Handle data type
//
typedef DWORD_PTR          RANGE_ELEMENT;
typedef RANGE_ELEMENT     *PRANGE_ELEMENT;

//
// Machine Handle data type
//
typedef  HANDLE             HMACHINE;
typedef  HMACHINE          *PHMACHINE;

//
// Conflict List data types
//
typedef ULONG_PTR           CONFLICT_LIST;
typedef CONFLICT_LIST      *PCONFLICT_LIST;

typedef struct _CONFLICT_DETAILS_A {
    ULONG       CD_ulSize;                   // size of structure, ie: sizeof(CONFLICT_DETAILS)
    ULONG       CD_ulMask;                   // indicates what information is required/valid
    DEVINST     CD_dnDevInst;                // filled with DevInst of conflicting device if CM_CDMASK_DEVINST set
    RES_DES     CD_rdResDes;                 // filled with a ResDes of conflict if CM_CDMASK_RESDES set
    ULONG       CD_ulFlags;                  // various flags regarding conflict
    CHAR        CD_szDescription[MAX_PATH];  // description of conflicting device
} CONFLICT_DETAILS_A , *PCONFLICT_DETAILS_A;

typedef struct _CONFLICT_DETAILS_W {
    ULONG       CD_ulSize;                   // size of structure, ie: sizeof(CONFLICT_DETAILS)
    ULONG       CD_ulMask;                   // indicates what information is required/valid
    DEVINST     CD_dnDevInst;                // filled with DevInst of conflicting device if CM_CDMASK_DEVINST set
    RES_DES     CD_rdResDes;                 // filled with a ResDes of conflict if CM_CDMASK_RESDES set
    ULONG       CD_ulFlags;                  // various flags regarding conflict
    WCHAR       CD_szDescription[MAX_PATH];  // description of conflicting device
} CONFLICT_DETAILS_W , *PCONFLICT_DETAILS_W;

#ifdef UNICODE
typedef CONFLICT_DETAILS_W CONFLICT_DETAILS;
typedef PCONFLICT_DETAILS_W PCONFLICT_DETAILS;
#else
typedef CONFLICT_DETAILS_A CONFLICT_DETAILS;
typedef PCONFLICT_DETAILS_A PCONFLICT_DETAILS;
#endif

#define CM_CDMASK_DEVINST      (0x00000001)   // mask to retrieve CD_dnDevInst attribute for conflict
#define CM_CDMASK_RESDES       (0x00000002)   // mask to retrieve CD_rdResDes attribute for conflict
#define CM_CDMASK_FLAGS        (0x00000004)   // mask to retrieve CD_ulFlags attribute for conflict
#define CM_CDMASK_DESCRIPTION  (0x00000008)   // mask to retrieve CD_szDescription attribute for conflict
#define CM_CDMASK_VALID        (0x0000000F)   // valid bits

#define CM_CDFLAGS_DRIVER      (0x00000001)     // CD_ulFlags: CD_szDescription reports back legacy driver name
#define CM_CDFLAGS_ROOT_OWNED  (0x00000002)     // CD_ulFlags: Root owned device
#define CM_CDFLAGS_RESERVED    (0x00000004)     // CD_ulFlags: Specified range is not available for use

typedef  ULONG             REGDISPOSITION;



//
// use 1 byte packing for the data structures
//
#include "pshpack1.h"



//--------------------------------------------------------------
// Memory resource
//--------------------------------------------------------------

//
// Define the attribute flags for memory ranges.  Each bit flag is
// identified by a constant bitmask.  Following the bitmask definition,
// are the two possible values.
//
#define mMD_MemoryType              (0x1) // Bitmask, whether memory is writable
#define fMD_MemoryType              mMD_MemoryType // compatibility
#define fMD_ROM                     (0x0) // Memory range is read-only
#define fMD_RAM                     (0x1) // Memory range may be written to

#define mMD_32_24                   (0x2) // Bitmask, memory is 24 or 32-bit
#define fMD_32_24                   mMD_32_24 // compatibility
#define fMD_24                      (0x0) // Memory range is 24-bit
#define fMD_32                      (0x2) // Memory range is 32-bit

#define mMD_Prefetchable            (0x4) // Bitmask,whether memory prefetchable
#define fMD_Prefetchable            mMD_Prefetchable // compatibility
#define fMD_Pref                    mMD_Prefetchable // compatibility
#define fMD_PrefetchDisallowed      (0x0) // Memory range is not prefetchable
#define fMD_PrefetchAllowed         (0x4) // Memory range is prefetchable

#define mMD_Readable                (0x8) // Bitmask,whether memory is readable
#define fMD_Readable                mMD_Readable // compatibility
#define fMD_ReadAllowed             (0x0) // Memory range is readable
#define fMD_ReadDisallowed          (0x8) // Memory range is write-only

#define mMD_CombinedWrite           (0x10) // Bitmask,supports write-behind
#define fMD_CombinedWrite           mMD_CombinedWrite // compatibility
#define fMD_CombinedWriteDisallowed (0x0)  // no combined-write caching
#define fMD_CombinedWriteAllowed    (0x10) // supports combined-write caching

#define mMD_Cacheable               (0x20) // Bitmask,whether memory is cacheable
#define fMD_NonCacheable            (0x0)  // Memory range is non-cacheable
#define fMD_Cacheable               (0x20) // Memory range is cacheable

//
// MEM_RANGE Structure
//
typedef struct Mem_Range_s {
   DWORDLONG MR_Align;     // specifies mask for base alignment
   ULONG     MR_nBytes;    // specifies number of bytes required
   DWORDLONG MR_Min;       // specifies minimum address of the range
   DWORDLONG MR_Max;       // specifies maximum address of the range
   DWORD     MR_Flags;     // specifies flags describing range (fMD flags)
   DWORD     MR_Reserved;
} MEM_RANGE, *PMEM_RANGE;

//
// MEM_DES structure
//
typedef struct Mem_Des_s {
   DWORD     MD_Count;        // number of MEM_RANGE structs in MEM_RESOURCE
   DWORD     MD_Type;         // size (in bytes) of MEM_RANGE (MType_Range)
   DWORDLONG MD_Alloc_Base;   // base memory address of range allocated
   DWORDLONG MD_Alloc_End;    // end of allocated range
   DWORD     MD_Flags;        // flags describing allocated range (fMD flags)
   DWORD     MD_Reserved;
} MEM_DES, *PMEM_DES;

//
// MEM_RESOURCE structure
//
typedef struct Mem_Resource_s {
   MEM_DES   MEM_Header;               // info about memory range list
   MEM_RANGE MEM_Data[ANYSIZE_ARRAY];  // list of memory ranges
} MEM_RESOURCE, *PMEM_RESOURCE;

//
// Define the size of each range structure
//
#define MType_Range     sizeof(struct Mem_Range_s)



//--------------------------------------------------------------
// I/O Port Resource
//--------------------------------------------------------------

//
// Define the attribute flags for port resources.  Each bit flag is
// identified by a constant bitmask.  Following the bitmask definition,
// are the two possible values.
//
#define fIOD_PortType   (0x1) // Bitmask,whether port is IO or memory
#define fIOD_Memory     (0x0) // Port resource really uses memory
#define fIOD_IO         (0x1) // Port resource uses IO ports
#define fIOD_DECODE     (0x00fc) // decode flags
#define fIOD_10_BIT_DECODE    (0x0004)
#define fIOD_12_BIT_DECODE    (0x0008)
#define fIOD_16_BIT_DECODE    (0x0010)
#define fIOD_POSITIVE_DECODE  (0x0020)
#define fIOD_PASSIVE_DECODE   (0x0040)
#define fIOD_WINDOW_DECODE    (0x0080)

//
// these are for compatiblity
//
#define IO_ALIAS_10_BIT_DECODE      (0x00000004)
#define IO_ALIAS_12_BIT_DECODE      (0x00000010)
#define IO_ALIAS_16_BIT_DECODE      (0x00000000)
#define IO_ALIAS_POSITIVE_DECODE    (0x000000FF)

//
// IO_RANGE structure
//
typedef struct IO_Range_s {
   DWORDLONG IOR_Align;      // mask for base alignment
   DWORD     IOR_nPorts;     // number of ports
   DWORDLONG IOR_Min;        // minimum port address
   DWORDLONG IOR_Max;        // maximum port address
   DWORD     IOR_RangeFlags; // flags for this port range
   DWORDLONG IOR_Alias;      // multiplier that generates aliases for port(s)
} IO_RANGE, *PIO_RANGE;

//
// IO_DES structure
//
typedef struct IO_Des_s {
   DWORD     IOD_Count;          // number of IO_RANGE structs in IO_RESOURCE
   DWORD     IOD_Type;           // size (in bytes) of IO_RANGE (IOType_Range)
   DWORDLONG IOD_Alloc_Base;     // base of allocated port range
   DWORDLONG IOD_Alloc_End;      // end of allocated port range
   DWORD     IOD_DesFlags;       // flags relating to allocated port range
} IO_DES, *PIO_DES;

//
// IO_RESOURCE
//
typedef struct IO_Resource_s {
   IO_DES   IO_Header;                 // info about I/O port range list
   IO_RANGE IO_Data[ANYSIZE_ARRAY];    // list of I/O port ranges
} IO_RESOURCE, *PIO_RESOURCE;

#define IOA_Local       0xff

//
// Define the size of each range structure
//
#define IOType_Range    sizeof(struct IO_Range_s)



//--------------------------------------------------------------
// DMA Resource
//--------------------------------------------------------------

//
// Define the attribute flags for a DMA resource range.  Each bit flag is
// identified with a constant bitmask.  Following the bitmask definition
// are the possible values.
//
#define mDD_Width         (0x3)    // Bitmask, width of the DMA channel:
#define fDD_BYTE          (0x0)    //   8-bit DMA channel
#define fDD_WORD          (0x1)    //   16-bit DMA channel
#define fDD_DWORD         (0x2)    //   32-bit DMA channel
#define fDD_BYTE_AND_WORD (0x3)    //   8-bit and 16-bit DMA channel

#define mDD_BusMaster     (0x4)    // Bitmask, whether bus mastering is supported
#define fDD_NoBusMaster   (0x0)    //   no bus mastering
#define fDD_BusMaster     (0x4)    //   bus mastering

#define mDD_Type         (0x18)    // Bitmask, specifies type of DMA
#define fDD_TypeStandard (0x00)    //   standard DMA
#define fDD_TypeA        (0x08)    //   Type-A DMA
#define fDD_TypeB        (0x10)    //   Type-B DMA
#define fDD_TypeF        (0x18)    //   Type-F DMA


//
// DMA_RANGE structure
//
typedef struct DMA_Range_s {
   ULONG DR_Min;     // minimum DMA port in the range
   ULONG DR_Max;     // maximum DMA port in the range
   ULONG DR_Flags;   // flags describing the range (fDD flags)
} DMA_RANGE, *PDMA_RANGE;

//
// DMA_DES structure
//
typedef struct DMA_Des_s {
   DWORD  DD_Count;       // number of DMA_RANGE structs in DMA_RESOURCE
   DWORD  DD_Type;        // size (in bytes) of DMA_RANGE struct (DType_Range)
   DWORD  DD_Flags;       // Flags describing DMA channel (fDD flags)
   ULONG  DD_Alloc_Chan;  // Specifies the DMA channel that was allocated
} DMA_DES, *PDMA_DES;

//
// DMA_RESOURCE
//
typedef struct DMA_Resource_s {
   DMA_DES   DMA_Header;               // info about DMA channel range list
   DMA_RANGE DMA_Data[ANYSIZE_ARRAY];  // list of DMA ranges
} DMA_RESOURCE, *PDMA_RESOURCE;

//
// Define the size of each range structure
//
#define DType_Range     sizeof(struct DMA_Range_s)



//--------------------------------------------------------------
// Interrupt Resource
//--------------------------------------------------------------

//
// Define the attribute flags for an interrupt resource range.  Each bit flag
// is identified with a constant bitmask.  Following the bitmask definition
// are the possible values.
//
#define mIRQD_Share        (0x1) // Bitmask,whether the IRQ may be shared:
#define fIRQD_Exclusive    (0x0) //   The IRQ may not be shared
#define fIRQD_Share        (0x1) //   The IRQ may be shared

#define fIRQD_Share_Bit    0     // compatibility
#define fIRQD_Level_Bit    1     // compatibility

//
// ** NOTE: 16-bit ConfigMgr uses fIRQD_Level_Bit being set to indicate that the
// ** interrupt is _level-sensitive_.  For 32-bit ConfigMgr, if this bit is set,
// ** then the interrupt is _edge-sensitive_.
//
#define mIRQD_Edge_Level   (0x2) // Bitmask,whether edge or level triggered:
#define fIRQD_Level        (0x0) //   The IRQ is level-sensitive
#define fIRQD_Edge         (0x2) //   The IRQ is edge-sensitive

//
// IRQ_RANGE
//
typedef struct IRQ_Range_s {
   ULONG IRQR_Min;      // minimum IRQ in the range
   ULONG IRQR_Max;      // maximum IRQ in the range
   ULONG IRQR_Flags;    // flags describing the range (fIRQD flags)
} IRQ_RANGE, *PIRQ_RANGE;

//
// IRQ_DES structure
//
typedef struct IRQ_Des_32_s {
   DWORD   IRQD_Count;       // number of IRQ_RANGE structs in IRQ_RESOURCE
   DWORD   IRQD_Type;        // size (in bytes) of IRQ_RANGE (IRQType_Range)
   DWORD   IRQD_Flags;       // flags describing the IRQ (fIRQD flags)
   ULONG   IRQD_Alloc_Num;   // specifies the IRQ that was allocated
   ULONG32 IRQD_Affinity;
} IRQ_DES_32, *PIRQ_DES_32;

typedef struct IRQ_Des_64_s {
   DWORD   IRQD_Count;       // number of IRQ_RANGE structs in IRQ_RESOURCE
   DWORD   IRQD_Type;        // size (in bytes) of IRQ_RANGE (IRQType_Range)
   DWORD   IRQD_Flags;       // flags describing the IRQ (fIRQD flags)
   ULONG   IRQD_Alloc_Num;   // specifies the IRQ that was allocated
   ULONG64 IRQD_Affinity;
} IRQ_DES_64, *PIRQ_DES_64;

#ifdef _WIN64
typedef IRQ_DES_64   IRQ_DES;
typedef PIRQ_DES_64  PIRQ_DES;
#else
typedef IRQ_DES_32   IRQ_DES;
typedef PIRQ_DES_32  PIRQ_DES;
#endif

//
// IRQ_RESOURCE structure
//
typedef struct IRQ_Resource_32_s {
   IRQ_DES_32   IRQ_Header;               // info about IRQ range list
   IRQ_RANGE    IRQ_Data[ANYSIZE_ARRAY];  // list of IRQ ranges
} IRQ_RESOURCE_32, *PIRQ_RESOURCE_32;

typedef struct IRQ_Resource_64_s {
   IRQ_DES_64   IRQ_Header;               // info about IRQ range list
   IRQ_RANGE    IRQ_Data[ANYSIZE_ARRAY];  // list of IRQ ranges
} IRQ_RESOURCE_64, *PIRQ_RESOURCE_64;

#ifdef _WIN64
typedef IRQ_RESOURCE_64  IRQ_RESOURCE;
typedef PIRQ_RESOURCE_64 PIRQ_RESOURCE;
#else
typedef IRQ_RESOURCE_32  IRQ_RESOURCE;
typedef PIRQ_RESOURCE_32 PIRQ_RESOURCE;
#endif

//
// Define the size of each range structure
//
#define IRQType_Range   sizeof(struct IRQ_Range_s)

//
// Flags for resource descriptor APIs indicating the width of certain
// variable-size resource descriptor structure fields, where applicable.
//
#define CM_RESDES_WIDTH_DEFAULT (0x00000000)  // 32 or 64-bit IRQ_RESOURCE / IRQ_DES, based on client
#define CM_RESDES_WIDTH_32      (0x00000001)  // 32-bit IRQ_RESOURCE / IRQ_DES
#define CM_RESDES_WIDTH_64      (0x00000002)  // 64-bit IRQ_RESOURCE / IRQ_DES
#define CM_RESDES_WIDTH_BITS    (0x00000003)

//--------------------------------------------------------------
// Device Private Resource
//--------------------------------------------------------------

//
// DEVICEPRIVATE_RANGE structure
//

typedef struct DevPrivate_Range_s {
   DWORD    PR_Data1;     // mask for base alignment
   DWORD    PR_Data2;     // number of bytes
   DWORD    PR_Data3;     // minimum address
} DEVPRIVATE_RANGE, *PDEVPRIVATE_RANGE;

//
// DEVPRIVATE_DES structure
//
typedef struct DevPrivate_Des_s {
   DWORD     PD_Count;
   DWORD     PD_Type;
   DWORD     PD_Data1;
   DWORD     PD_Data2;
   DWORD     PD_Data3;
   DWORD     PD_Flags;
} DEVPRIVATE_DES, *PDEVPRIVATE_DES;

//
// DEVPRIVATE_RESOURCE
//
typedef struct DevPrivate_Resource_s {
   DEVPRIVATE_DES   PRV_Header;
   DEVPRIVATE_RANGE PRV_Data[ANYSIZE_ARRAY];
} DEVPRIVATE_RESOURCE, *PDEVPRIVATE_RESOURCE;

//
// Define the size of each range structure
//
#define PType_Range    sizeof(struct DevPrivate_Range_s)



//--------------------------------------------------------------
// Class-Specific Resource
//--------------------------------------------------------------

typedef struct CS_Des_s {
   DWORD    CSD_SignatureLength;
   DWORD    CSD_LegacyDataOffset;
   DWORD    CSD_LegacyDataSize;
   DWORD    CSD_Flags;
   GUID     CSD_ClassGuid;
   BYTE     CSD_Signature[ANYSIZE_ARRAY];
} CS_DES, *PCS_DES;

typedef struct CS_Resource_s {
   CS_DES   CS_Header;
} CS_RESOURCE, *PCS_RESOURCE;



//--------------------------------------------------------------
// PC Card Configuration Resource
//--------------------------------------------------------------

//
// Define the attribute flags for a PC Card configuration resource descriptor.
// Each bit flag is identified with a constant bitmask.  Following the bitmask
// definition are the possible values.
//
#define mPCD_IO_8_16        (0x1)   // Bitmask, whether I/O is 8 or 16 bits
#define fPCD_IO_8           (0x0)   // I/O is 8-bit
#define fPCD_IO_16          (0x1)   // I/O is 16-bit
#define mPCD_MEM_8_16       (0x2)   // Bitmask, whether MEM is 8 or 16 bits
#define fPCD_MEM_8          (0x0)   // MEM is 8-bit
#define fPCD_MEM_16         (0x2)   // MEM is 16-bit
#define mPCD_MEM_A_C        (0xC)   // Bitmask, whether MEMx is Attribute or Common
#define fPCD_MEM1_A         (0x4)   // MEM1 is Attribute
#define fPCD_MEM2_A         (0x8)   // MEM2 is Attribute
#define fPCD_IO_ZW_8        (0x10)  // zero wait on 8 bit I/O
#define fPCD_IO_SRC_16      (0x20)  // iosrc 16
#define fPCD_IO_WS_16       (0x40)  // wait states on 16 bit io
#define mPCD_MEM_WS         (0x300) // Bitmask, for additional wait states on memory windows
#define fPCD_MEM_WS_ONE     (0x100) // 1 wait state
#define fPCD_MEM_WS_TWO     (0x200) // 2 wait states
#define fPCD_MEM_WS_THREE   (0x300) // 3 wait states

#define fPCD_MEM_A          (0x4)   // MEM is Attribute

#define fPCD_ATTRIBUTES_PER_WINDOW (0x8000)

#define fPCD_IO1_16         (0x00010000)  // I/O window 1 is 16-bit
#define fPCD_IO1_ZW_8       (0x00020000)  // I/O window 1 zero wait on 8 bit I/O
#define fPCD_IO1_SRC_16     (0x00040000)  // I/O window 1 iosrc 16
#define fPCD_IO1_WS_16      (0x00080000)  // I/O window 1 wait states on 16 bit io

#define fPCD_IO2_16         (0x00100000)  // I/O window 2 is 16-bit
#define fPCD_IO2_ZW_8       (0x00200000)  // I/O window 2 zero wait on 8 bit I/O
#define fPCD_IO2_SRC_16     (0x00400000)  // I/O window 2 iosrc 16
#define fPCD_IO2_WS_16      (0x00800000)  // I/O window 2 wait states on 16 bit io

#define mPCD_MEM1_WS        (0x03000000)  // MEM window 1 Bitmask, for additional wait states on memory windows
#define fPCD_MEM1_WS_ONE    (0x01000000)  // MEM window 1, 1 wait state
#define fPCD_MEM1_WS_TWO    (0x02000000)  // MEM window 1, 2 wait states
#define fPCD_MEM1_WS_THREE  (0x03000000)  // MEM window 1, 3 wait states
#define fPCD_MEM1_16        (0x04000000)  // MEM window 1 is 16-bit

#define mPCD_MEM2_WS        (0x30000000)  // MEM window 2 Bitmask, for additional wait states on memory windows
#define fPCD_MEM2_WS_ONE    (0x10000000)  // MEM window 2, 1 wait state
#define fPCD_MEM2_WS_TWO    (0x20000000)  // MEM window 2, 2 wait states
#define fPCD_MEM2_WS_THREE  (0x30000000)  // MEM window 2, 3 wait states
#define fPCD_MEM2_16        (0x40000000)  // MEM window 2 is 16-bit

#define PCD_MAX_MEMORY   2
#define PCD_MAX_IO       2


typedef struct PcCard_Des_s {
    DWORD    PCD_Count;
    DWORD    PCD_Type;
    DWORD    PCD_Flags;
    BYTE     PCD_ConfigIndex;
    BYTE     PCD_Reserved[3];
    DWORD    PCD_MemoryCardBase1;
    DWORD    PCD_MemoryCardBase2;
    DWORD    PCD_MemoryCardBase[PCD_MAX_MEMORY];            // will soon be removed
    WORD     PCD_MemoryFlags[PCD_MAX_MEMORY];               // will soon be removed
    BYTE     PCD_IoFlags[PCD_MAX_IO];                       // will soon be removed
} PCCARD_DES, *PPCCARD_DES;

typedef struct PcCard_Resource_s {
   PCCARD_DES   PcCard_Header;
} PCCARD_RESOURCE, *PPCCARD_RESOURCE;

//--------------------------------------------------------------
// MF (multifunction) PCCard Configuration Resource
//--------------------------------------------------------------

#define mPMF_AUDIO_ENABLE   (0x8)   // Bitmask, whether audio is enabled or not
#define fPMF_AUDIO_ENABLE   (0x8)   // Audio is enabled

typedef struct MfCard_Des_s {
    DWORD    PMF_Count;
    DWORD    PMF_Type;
    DWORD    PMF_Flags;
    BYTE     PMF_ConfigOptions;
    BYTE     PMF_IoResourceIndex;
    BYTE     PMF_Reserved[2];
    DWORD    PMF_ConfigRegisterBase;
} MFCARD_DES, *PMFCARD_DES;

typedef struct MfCard_Resource_s {
   MFCARD_DES   MfCard_Header;
} MFCARD_RESOURCE, *PMFCARD_RESOURCE;


//--------------------------------------------------------------
// Bus Number Resource
//--------------------------------------------------------------

//
// Define the attribute flags for a Bus Number resource descriptor.
// Each bit flag is identified with a constant bitmask.  Following the bitmask
// definition are the possible values.
//
// Currently unused.
//

//
// BUSNUMBER_RANGE
//
typedef struct BusNumber_Range_s {
   ULONG BUSR_Min;          // minimum Bus Number in the range
   ULONG BUSR_Max;          // maximum Bus Number in the range
   ULONG BUSR_nBusNumbers;  // specifies number of buses required
   ULONG BUSR_Flags;        // flags describing the range (currently unused)
} BUSNUMBER_RANGE, *PBUSNUMBER_RANGE;

//
// BUSNUMBER_DES structure
//
typedef struct BusNumber_Des_s {
   DWORD BUSD_Count;       // number of BUSNUMBER_RANGE structs in BUSNUMBER_RESOURCE
   DWORD BUSD_Type;        // size (in bytes) of BUSNUMBER_RANGE (BusNumberType_Range)
   DWORD BUSD_Flags;       // flags describing the range (currently unused)
   ULONG BUSD_Alloc_Base;  // specifies the first Bus that was allocated
   ULONG BUSD_Alloc_End;   // specifies the last Bus number that was allocated
} BUSNUMBER_DES, *PBUSNUMBER_DES;

//
// BUSNUMBER_RESOURCE structure
//
typedef struct BusNumber_Resource_s {
   BUSNUMBER_DES   BusNumber_Header;               // info about Bus Number range list
   BUSNUMBER_RANGE BusNumber_Data[ANYSIZE_ARRAY];  // list of Bus Number ranges
} BUSNUMBER_RESOURCE, *PBUSNUMBER_RESOURCE;

//
// Define the size of each range structure
//
#define BusNumberType_Range   sizeof(struct BusNumber_Range_s)



//--------------------------------------------------------------
// Hardware Profile Information
//--------------------------------------------------------------

//
// Define flags relating to hardware profiles
//
#define CM_HWPI_NOT_DOCKABLE  (0x00000000)   // machine is not dockable
#define CM_HWPI_UNDOCKED      (0x00000001)   // hw profile for docked config
#define CM_HWPI_DOCKED        (0x00000002)   // hw profile for undocked config

//
// HWPROFILEINFO structure
//
typedef struct HWProfileInfo_sA {
   ULONG  HWPI_ulHWProfile;                      // handle of hw profile
   CHAR   HWPI_szFriendlyName[MAX_PROFILE_LEN];  // friendly name of hw profile
   DWORD  HWPI_dwFlags;                          // profile flags (CM_HWPI_*)
} HWPROFILEINFO_A, *PHWPROFILEINFO_A;

typedef struct HWProfileInfo_sW {
   ULONG  HWPI_ulHWProfile;                      // handle of hw profile
   WCHAR  HWPI_szFriendlyName[MAX_PROFILE_LEN];  // friendly name of hw profile
   DWORD  HWPI_dwFlags;                          // profile flags (CM_HWPI_*)
} HWPROFILEINFO_W, *PHWPROFILEINFO_W;

#ifdef UNICODE
typedef HWPROFILEINFO_W   HWPROFILEINFO;
typedef PHWPROFILEINFO_W  PHWPROFILEINFO;
#else
typedef HWPROFILEINFO_A   HWPROFILEINFO;
typedef PHWPROFILEINFO_A  PHWPROFILEINFO;
#endif


//
// revert back to normal default packing
//
#include "poppack.h"



//--------------------------------------------------------------
// Miscellaneous
//--------------------------------------------------------------


//
// Resource types
//
#define ResType_All           (0x00000000)   // Return all resource types
#define ResType_None          (0x00000000)   // Arbitration always succeeded
#define ResType_Mem           (0x00000001)   // Physical address resource
#define ResType_IO            (0x00000002)   // Physical I/O address resource
#define ResType_DMA           (0x00000003)   // DMA channels resource
#define ResType_IRQ           (0x00000004)   // IRQ resource
#define ResType_DoNotUse      (0x00000005)   // Used as spacer to sync subsequent ResTypes w/NT
#define ResType_BusNumber     (0x00000006)   // bus number resource
#define ResType_MAX           (0x00000006)   // Maximum known (arbitrated) ResType
#define ResType_Ignored_Bit   (0x00008000)   // Ignore this resource
#define ResType_ClassSpecific (0x0000FFFF)   // class-specific resource
#define ResType_Reserved      (0x00008000)   // reserved for internal use
#define ResType_DevicePrivate (0x00008001)   // device private data
#define ResType_PcCardConfig  (0x00008002)   // PC Card configuration data
#define ResType_MfCardConfig  (0x00008003)   // MF Card configuration data


//
// Flags specifying options for ranges that conflict with ranges already in
// the range list (CM_Add_Range)
//
#define CM_ADD_RANGE_ADDIFCONFLICT        (0x00000000) // merg with conflicting range
#define CM_ADD_RANGE_DONOTADDIFCONFLICT   (0x00000001) // error if range conflicts
#define CM_ADD_RANGE_BITS                 (0x00000001)


//
// Logical Config Flags (specified in call to CM_Get_First_Log_Conf
//
#define BASIC_LOG_CONF    0x00000000  // Specifies the req list.
#define FILTERED_LOG_CONF 0x00000001  // Specifies the filtered req list.
#define ALLOC_LOG_CONF    0x00000002  // Specifies the Alloc Element.
#define BOOT_LOG_CONF     0x00000003  // Specifies the RM Alloc Element.
#define FORCED_LOG_CONF   0x00000004  // Specifies the Forced Log Conf
#define OVERRIDE_LOG_CONF 0x00000005  // Specifies the Override req list.
#define NUM_LOG_CONF      0x00000006  // Number of Log Conf type
#define LOG_CONF_BITS     0x00000007  // The bits of the log conf type.

#define PRIORITY_EQUAL_FIRST  (0x00000008) // Same priority, new one first
#define PRIORITY_EQUAL_LAST   (0x00000000) // Same priority, new one last
#define PRIORITY_BIT          (0x00000008)

//
// Registry disposition values
// (specified in call to CM_Open_DevNode_Key and CM_Open_Class_Key)
//
#define RegDisposition_OpenAlways   (0x00000000)   // open if exists else create
#define RegDisposition_OpenExisting (0x00000001)   // open key only if exists
#define RegDisposition_Bits         (0x00000001)

//
// ulFlags values for CM API routines
//

//
// Flags for CM_Add_ID
//
#define CM_ADD_ID_HARDWARE                (0x00000000)
#define CM_ADD_ID_COMPATIBLE              (0x00000001)
#define CM_ADD_ID_BITS                    (0x00000001)


//
// Device Node creation flags
//
#define CM_CREATE_DEVNODE_NORMAL          (0x00000000)   // install later
#define CM_CREATE_DEVNODE_NO_WAIT_INSTALL (0x00000001)   // NOT SUPPORTED ON NT
#define CM_CREATE_DEVNODE_PHANTOM         (0x00000002)
#define CM_CREATE_DEVNODE_GENERATE_ID     (0x00000004)
#define CM_CREATE_DEVNODE_DO_NOT_INSTALL  (0x00000008)
#define CM_CREATE_DEVNODE_BITS            (0x0000000F)

#define CM_CREATE_DEVINST_NORMAL          CM_CREATE_DEVNODE_NORMAL
#define CM_CREATE_DEVINST_NO_WAIT_INSTALL CM_CREATE_DEVNODE_NO_WAIT_INSTALL
#define CM_CREATE_DEVINST_PHANTOM         CM_CREATE_DEVNODE_PHANTOM
#define CM_CREATE_DEVINST_GENERATE_ID     CM_CREATE_DEVNODE_GENERATE_ID
#define CM_CREATE_DEVINST_DO_NOT_INSTALL  CM_CREATE_DEVNODE_DO_NOT_INSTALL
#define CM_CREATE_DEVINST_BITS            CM_CREATE_DEVNODE_BITS


//
// Flags for CM_Delete_Class_Key
//
#define CM_DELETE_CLASS_ONLY        (0x00000000)
#define CM_DELETE_CLASS_SUBKEYS     (0x00000001)
#define CM_DELETE_CLASS_BITS        (0x00000001)


//
// Detection reason flags (specified in call to CM_Run_Detection)
//
#define CM_DETECT_NEW_PROFILE       (0x00000001) // detection for new hw profile
#define CM_DETECT_CRASHED           (0x00000002) // Previous detection crashed
#define CM_DETECT_HWPROF_FIRST_BOOT (0x00000004)
#define CM_DETECT_RUN               (0x80000000)
#define CM_DETECT_BITS              (0x80000007)

#define CM_DISABLE_POLITE           (0x00000000)    // Ask the driver
#define CM_DISABLE_ABSOLUTE         (0x00000001)    // Don't ask the driver
#define CM_DISABLE_HARDWARE         (0x00000002)    // Don't ask the driver, and won't be restarteable
#define CM_DISABLE_UI_NOT_OK        (0x00000004)    // Don't popup any veto API
#define CM_DISABLE_BITS             (0x00000007)    // The bits for the disable function


//
// Flags for CM_Get_Device_ID_List, CM_Get_Device_ID_List_Size
//
#define CM_GETIDLIST_FILTER_NONE                (0x00000000)
#define CM_GETIDLIST_FILTER_ENUMERATOR          (0x00000001)
#define CM_GETIDLIST_FILTER_SERVICE             (0x00000002)
#define CM_GETIDLIST_FILTER_EJECTRELATIONS      (0x00000004)
#define CM_GETIDLIST_FILTER_REMOVALRELATIONS    (0x00000008)
#define CM_GETIDLIST_FILTER_POWERRELATIONS      (0x00000010)
#define CM_GETIDLIST_FILTER_BUSRELATIONS        (0x00000020)
#define CM_GETIDLIST_DONOTGENERATE              (0x10000040)
#define CM_GETIDLIST_FILTER_BITS                (0x1000007F)

//
// Flags for CM_Get_Device_Interface_List, CM_Get_Device_Interface_List_Size
//
#define CM_GET_DEVICE_INTERFACE_LIST_PRESENT     (0x00000000)  // only currently 'live' device interfaces
#define CM_GET_DEVICE_INTERFACE_LIST_ALL_DEVICES (0x00000001)  // all registered device interfaces, live or not
#define CM_GET_DEVICE_INTERFACE_LIST_BITS        (0x00000001)

//
// Registry properties (specified in call to CM_Get_DevInst_Registry_Property or CM_Get_Class_Registry_Property,
// some are allowed in calls to CM_Set_DevInst_Registry_Property and CM_Set_Class_Registry_Property)
// CM_DRP_xxxx values should be used for CM_Get_DevInst_Registry_Property / CM_Set_DevInst_Registry_Property
// CM_CRP_xxxx values should be used for CM_Get_Class_Registry_Property / CM_Set_Class_Registry_Property
// DRP/CRP values that overlap must have a 1:1 correspondence with each other
//
#define CM_DRP_DEVICEDESC                  (0x00000001) // DeviceDesc REG_SZ property (RW)
#define CM_DRP_HARDWAREID                  (0x00000002) // HardwareID REG_MULTI_SZ property (RW)
#define CM_DRP_COMPATIBLEIDS               (0x00000003) // CompatibleIDs REG_MULTI_SZ property (RW)
#define CM_DRP_UNUSED0                     (0x00000004) // unused
#define CM_DRP_SERVICE                     (0x00000005) // Service REG_SZ property (RW)
#define CM_DRP_UNUSED1                     (0x00000006) // unused
#define CM_DRP_UNUSED2                     (0x00000007) // unused
#define CM_DRP_CLASS                       (0x00000008) // Class REG_SZ property (RW)
#define CM_DRP_CLASSGUID                   (0x00000009) // ClassGUID REG_SZ property (RW)
#define CM_DRP_DRIVER                      (0x0000000A) // Driver REG_SZ property (RW)
#define CM_DRP_CONFIGFLAGS                 (0x0000000B) // ConfigFlags REG_DWORD property (RW)
#define CM_DRP_MFG                         (0x0000000C) // Mfg REG_SZ property (RW)
#define CM_DRP_FRIENDLYNAME                (0x0000000D) // FriendlyName REG_SZ property (RW)
#define CM_DRP_LOCATION_INFORMATION        (0x0000000E) // LocationInformation REG_SZ property (RW)
#define CM_DRP_PHYSICAL_DEVICE_OBJECT_NAME (0x0000000F) // PhysicalDeviceObjectName REG_SZ property (R)
#define CM_DRP_CAPABILITIES                (0x00000010) // Capabilities REG_DWORD property (R)
#define CM_DRP_UI_NUMBER                   (0x00000011) // UiNumber REG_DWORD property (R)
#define CM_DRP_UPPERFILTERS                (0x00000012) // UpperFilters REG_MULTI_SZ property (RW)
#define CM_DRP_LOWERFILTERS                (0x00000013) // LowerFilters REG_MULTI_SZ property (RW)
#define CM_DRP_BUSTYPEGUID                 (0x00000014) // Bus Type Guid, GUID, (R)
#define CM_DRP_LEGACYBUSTYPE               (0x00000015) // Legacy bus type, INTERFACE_TYPE, (R)
#define CM_DRP_BUSNUMBER                   (0x00000016) // Bus Number, DWORD, (R)
#define CM_DRP_ENUMERATOR_NAME             (0x00000017) // Enumerator Name REG_SZ property (R)
#define CM_DRP_SECURITY                    (0x00000018) // Security - Device override (RW)
#define CM_CRP_SECURITY                    CM_DRP_SECURITY   // Class default security (RW)
#define CM_DRP_SECURITY_SDS                (0x00000019) // Security - Device override (RW)
#define CM_CRP_SECURITY_SDS                CM_DRP_SECURITY_SDS // Class default security (RW)
#define CM_DRP_DEVTYPE                     (0x0000001A) // Device Type - Device override (RW)
#define CM_CRP_DEVTYPE                     CM_DRP_DEVTYPE    // Class default Device-type (RW)
#define CM_DRP_EXCLUSIVE                   (0x0000001B) // Exclusivity - Device override (RW)
#define CM_CRP_EXCLUSIVE                   CM_DRP_EXCLUSIVE  // Class default (RW)
#define CM_DRP_CHARACTERISTICS             (0x0000001C) // Characteristics - Device Override (RW)
#define CM_CRP_CHARACTERISTICS             CM_DRP_CHARACTERISTICS  // Class default (RW)
#define CM_DRP_ADDRESS                     (0x0000001D) // Device Address (R)
#define CM_DRP_UI_NUMBER_DESC_FORMAT       (0x0000001E) // UINumberDescFormat REG_SZ property (RW)
#define CM_DRP_DEVICE_POWER_DATA           (0x0000001F) // CM_POWER_DATA REG_BINARY property (R)
#define CM_DRP_REMOVAL_POLICY              (0x00000020) // CM_DEVICE_REMOVAL_POLICY REG_DWORD (R)
#define CM_DRP_REMOVAL_POLICY_HW_DEFAULT   (0x00000021) // CM_DRP_REMOVAL_POLICY_HW_DEFAULT REG_DWORD (R)
#define CM_DRP_REMOVAL_POLICY_OVERRIDE     (0x00000022) // CM_DRP_REMOVAL_POLICY_OVERRIDE REG_DWORD (RW)
#define CM_DRP_INSTALL_STATE               (0x00000023) // CM_DRP_INSTALL_STATE REG_DWORD (R)
#define CM_DRP_LOCATION_PATHS              (0x00000024) // CM_DRP_LOCATION_PATHS REG_MULTI_SZ (R)

#define CM_DRP_MIN                         (0x00000001) // First device register
#define CM_CRP_MIN                         CM_DRP_MIN   // First class register
#define CM_DRP_MAX                         (0x00000024) // Last device register
#define CM_CRP_MAX                         CM_DRP_MAX   // Last class register

//
// Capabilities bits (the capability value is returned from calling
// CM_Get_DevInst_Registry_Property with CM_DRP_CAPABILITIES property)
//
#define CM_DEVCAP_LOCKSUPPORTED     (0x00000001)
#define CM_DEVCAP_EJECTSUPPORTED    (0x00000002)
#define CM_DEVCAP_REMOVABLE         (0x00000004)
#define CM_DEVCAP_DOCKDEVICE        (0x00000008)
#define CM_DEVCAP_UNIQUEID          (0x00000010)
#define CM_DEVCAP_SILENTINSTALL     (0x00000020)
#define CM_DEVCAP_RAWDEVICEOK       (0x00000040)
#define CM_DEVCAP_SURPRISEREMOVALOK (0x00000080)
#define CM_DEVCAP_HARDWAREDISABLED  (0x00000100)
#define CM_DEVCAP_NONDYNAMIC        (0x00000200)

//
// Removal policies (retrievable via CM_Get_DevInst_Registry_Property with
// the CM_DRP_REMOVAL_POLICY, CM_DRP_REMOVAL_POLICY_OVERRIDE, or
// CM_DRP_REMOVAL_POLICY_HW_DEFAULT properties)
//
#define CM_REMOVAL_POLICY_EXPECT_NO_REMOVAL             1
#define CM_REMOVAL_POLICY_EXPECT_ORDERLY_REMOVAL        2
#define CM_REMOVAL_POLICY_EXPECT_SURPRISE_REMOVAL       3

//
// Device install states (retrievable via CM_Get_DevInst_Registry_Property with
// the CM_DRP_INSTALL_STATE properties)
//
#define CM_INSTALL_STATE_INSTALLED                      0
#define CM_INSTALL_STATE_NEEDS_REINSTALL                1
#define CM_INSTALL_STATE_FAILED_INSTALL                 2
#define CM_INSTALL_STATE_FINISH_INSTALL                 3

//
// Flags for CM_Locate_DevNode
//
#define CM_LOCATE_DEVNODE_NORMAL       0x00000000
#define CM_LOCATE_DEVNODE_PHANTOM      0x00000001
#define CM_LOCATE_DEVNODE_CANCELREMOVE 0x00000002
#define CM_LOCATE_DEVNODE_NOVALIDATION 0x00000004
#define CM_LOCATE_DEVNODE_BITS         0x00000007

#define CM_LOCATE_DEVINST_NORMAL       CM_LOCATE_DEVNODE_NORMAL
#define CM_LOCATE_DEVINST_PHANTOM      CM_LOCATE_DEVNODE_PHANTOM
#define CM_LOCATE_DEVINST_CANCELREMOVE CM_LOCATE_DEVNODE_CANCELREMOVE
#define CM_LOCATE_DEVINST_NOVALIDATION CM_LOCATE_DEVNODE_NOVALIDATION
#define CM_LOCATE_DEVINST_BITS         CM_LOCATE_DEVNODE_BITS

//
// Flags for CM_Open_Class_Key
//
#define CM_OPEN_CLASS_KEY_INSTALLER        (0x00000000)
#define CM_OPEN_CLASS_KEY_INTERFACE        (0x00000001)
#define CM_OPEN_CLASS_KEY_BITS             (0x00000001)

//
// Flags for CM_Query_And_Remove_SubTree
//
#define CM_REMOVE_UI_OK             0x00000000
#define CM_REMOVE_UI_NOT_OK         0x00000001
#define CM_REMOVE_NO_RESTART        0x00000002
#define CM_REMOVE_BITS              0x00000003

//
// Backward compatibility--do not use
// (use above CM_REMOVE_* flags instead)
//
#define CM_QUERY_REMOVE_UI_OK       (CM_REMOVE_UI_OK)
#define CM_QUERY_REMOVE_UI_NOT_OK   (CM_REMOVE_UI_NOT_OK)
#define CM_QUERY_REMOVE_BITS        (CM_QUERY_REMOVE_UI_OK|CM_QUERY_REMOVE_UI_NOT_OK)

//
// Flags for CM_Reenumerate_DevNode
//
#define CM_REENUMERATE_NORMAL                   0x00000000
#define CM_REENUMERATE_SYNCHRONOUS              0x00000001
#define CM_REENUMERATE_RETRY_INSTALLATION       0x00000002
#define CM_REENUMERATE_ASYNCHRONOUS             0x00000004
#define CM_REENUMERATE_BITS                     0x00000007

//
// Flags for CM_Register_Device_Driver
//
#define CM_REGISTER_DEVICE_DRIVER_STATIC        (0x00000000)
#define CM_REGISTER_DEVICE_DRIVER_DISABLEABLE   (0x00000001)
#define CM_REGISTER_DEVICE_DRIVER_REMOVABLE     (0x00000002)
#define CM_REGISTER_DEVICE_DRIVER_BITS          (0x00000003)

//
// Registry Branch Locations (for CM_Open_DevNode_Key)
//
#define CM_REGISTRY_HARDWARE        (0x00000000)
#define CM_REGISTRY_SOFTWARE        (0x00000001)
#define CM_REGISTRY_USER            (0x00000100)
#define CM_REGISTRY_CONFIG          (0x00000200)
#define CM_REGISTRY_BITS            (0x00000301)

//
// Flags for CM_Set_DevNode_Problem
//
#define CM_SET_DEVNODE_PROBLEM_NORMAL    (0x00000000)  // only set problem if currently no problem
#define CM_SET_DEVNODE_PROBLEM_OVERRIDE  (0x00000001)  // override current problem with new problem
#define CM_SET_DEVNODE_PROBLEM_BITS      (0x00000001)

#define CM_SET_DEVINST_PROBLEM_NORMAL    CM_SET_DEVNODE_PROBLEM_NORMAL
#define CM_SET_DEVINST_PROBLEM_OVERRIDE  CM_SET_DEVNODE_PROBLEM_OVERRIDE
#define CM_SET_DEVINST_PROBLEM_BITS      CM_SET_DEVNODE_PROBLEM_BITS

//
// Flags for CM_Set_HW_Prof_Flags
//
#define CM_SET_HW_PROF_FLAGS_UI_NOT_OK  (0x00000001)    // Don't popup any veto UI
#define CM_SET_HW_PROF_FLAGS_BITS       (0x00000001)

//
// Re-enable and configuration actions (specified in call to CM_Setup_DevInst)
//
#define CM_SETUP_DEVNODE_READY   (0x00000000) // Reenable problem devinst
#define CM_SETUP_DEVINST_READY   CM_SETUP_DEVNODE_READY
#define CM_SETUP_DOWNLOAD        (0x00000001) // Get info about devinst
#define CM_SETUP_WRITE_LOG_CONFS (0x00000002)
#define CM_SETUP_PROP_CHANGE     (0x00000003)
#define CM_SETUP_DEVNODE_RESET   (0x00000004) // Reset problem devinst without starting
#define CM_SETUP_DEVINST_RESET   CM_SETUP_DEVNODE_RESET
#define CM_SETUP_BITS            (0x00000007)

//
// Flags for CM_Query_Arbitrator_Free_Data and
// CM_Query_Arbitrator_Free_Data_Size.
//
#define CM_QUERY_ARBITRATOR_RAW         (0x00000000)
#define CM_QUERY_ARBITRATOR_TRANSLATED  (0x00000001)
#define CM_QUERY_ARBITRATOR_BITS        (0x00000001)

//
// Flags for CM_Get_DevNode_Custom_Property
//
#define CM_CUSTOMDEVPROP_MERGE_MULTISZ  (0x00000001)
#define CM_CUSTOMDEVPROP_BITS           (0x00000001)


//--------------------------------------------------------------
// Function prototypes
//--------------------------------------------------------------



CMAPI
CONFIGRET
WINAPI
CM_Add_Empty_Log_Conf(
    __out PLOG_CONF plcLogConf,
    __in DEVINST dnDevInst,
    __in PRIORITY Priority,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Add_Empty_Log_Conf_Ex(
    __out PLOG_CONF plcLogConf,
    __in DEVINST dnDevInst,
    __in PRIORITY Priority,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Add_IDA(
    __in DEVINST dnDevInst,
    __in PSTR pszID,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Add_IDW(
    __in DEVINST dnDevInst,
    __in PWSTR pszID,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Add_ID_ExA(
    __in DEVINST dnDevInst,
    __in PSTR pszID,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Add_ID_ExW(
    __in DEVINST dnDevInst,
    __in PWSTR pszID,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#ifdef UNICODE
#define CM_Add_ID             CM_Add_IDW
#define CM_Add_ID_Ex          CM_Add_ID_ExW
#else
#define CM_Add_ID             CM_Add_IDA
#define CM_Add_ID_Ex          CM_Add_ID_ExA
#endif // UNICODE


CMAPI
CONFIGRET
WINAPI
CM_Add_Range(
    __in DWORDLONG ullStartValue,
    __in DWORDLONG ullEndValue,
    __in RANGE_LIST rlh,
    __in ULONG ulFlags
    );


CMAPI
CONFIGRET
WINAPI
CM_Add_Res_Des(
    __out_opt PRES_DES prdResDes,
    __in LOG_CONF lcLogConf,
    __in RESOURCEID ResourceID,
    __in_bcount(ResourceLen) PCVOID ResourceData,
    __in ULONG ResourceLen,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Add_Res_Des_Ex(
    __out_opt PRES_DES prdResDes,
    __in LOG_CONF lcLogConf,
    __in RESOURCEID ResourceID,
    __in_bcount(ResourceLen) PCVOID ResourceData,
    __in ULONG ResourceLen,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Connect_MachineA(
    __in_opt PCSTR UNCServerName,
    __out PHMACHINE phMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Connect_MachineW(
    __in_opt PCWSTR UNCServerName,
    __out PHMACHINE phMachine
    );
#ifdef UNICODE
#define CM_Connect_Machine       CM_Connect_MachineW
#else
#define CM_Connect_Machine       CM_Connect_MachineA
#endif // UNICODE



CMAPI
CONFIGRET
WINAPI
CM_Create_DevNodeA(
    __out PDEVINST pdnDevInst,
    __in DEVINSTID_A pDeviceID,
    __in DEVINST dnParent,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Create_DevNodeW(
    __out PDEVINST pdnDevInst,
    __in DEVINSTID_W pDeviceID,
    __in DEVINST dnParent,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Create_DevNode_ExA(
    __out PDEVINST pdnDevInst,
    __in DEVINSTID_A pDeviceID,
    __in DEVINST dnParent,
    __in ULONG ulFlags,
    __in_opt HANDLE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Create_DevNode_ExW(
    __out PDEVINST pdnDevInst,
    __in DEVINSTID_W pDeviceID,
    __in DEVINST dnParent,
    __in ULONG ulFlags,
    __in_opt HANDLE hMachine
    );
#define CM_Create_DevInstW       CM_Create_DevNodeW
#define CM_Create_DevInstA       CM_Create_DevNodeA
#define CM_Create_DevInst_ExW    CM_Create_DevNode_ExW
#define CM_Create_DevInst_ExA    CM_Create_DevNode_ExA
#ifdef UNICODE
#define CM_Create_DevNode        CM_Create_DevNodeW
#define CM_Create_DevInst        CM_Create_DevNodeW
#define CM_Create_DevNode_Ex     CM_Create_DevNode_ExW
#define CM_Create_DevInst_Ex     CM_Create_DevInst_ExW
#else
#define CM_Create_DevNode        CM_Create_DevNodeA
#define CM_Create_DevInst        CM_Create_DevNodeA
#define CM_Create_DevNode_Ex     CM_Create_DevNode_ExA
#define CM_Create_DevInst_Ex     CM_Create_DevNode_ExA
#endif // UNICODE


CMAPI
CONFIGRET
WINAPI
CM_Create_Range_List(
    __out PRANGE_LIST prlh,
    __in ULONG ulFlags
    );


CMAPI
CONFIGRET
WINAPI
CM_Delete_Class_Key(
    __in LPGUID ClassGuid,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Delete_Class_Key_Ex(
    __in LPGUID ClassGuid,
    __in ULONG ulFlags,
    __in_opt HANDLE hMachine
    );

CMAPI
CONFIGRET
WINAPI
CM_Delete_DevNode_Key(
    __in DEVNODE dnDevNode,
    __in ULONG ulHardwareProfile,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Delete_DevNode_Key_Ex(
    __in DEVNODE dnDevNode,
    __in ULONG ulHardwareProfile,
    __in ULONG ulFlags,
    __in_opt HANDLE hMachine
    );
#define CM_Delete_DevInst_Key       CM_Delete_DevNode_Key
#define CM_Delete_DevInst_Key_Ex    CM_Delete_DevNode_Key_Ex


CMAPI
CONFIGRET
WINAPI
CM_Delete_Range(
    __in DWORDLONG ullStartValue,
    __in DWORDLONG ullEndValue,
    __in RANGE_LIST rlh,
    __in ULONG ulFlags
    );


CMAPI
CONFIGRET
WINAPI
CM_Detect_Resource_Conflict(
    __in DEVINST dnDevInst,
    __in RESOURCEID ResourceID,
    __in_bcount(ResourceLen) PCVOID ResourceData,
    __in ULONG ResourceLen,
    __out PBOOL pbConflictDetected,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Detect_Resource_Conflict_Ex(
    __in DEVINST dnDevInst,
    __in RESOURCEID ResourceID,
    __in_bcount(ResourceLen) PCVOID ResourceData,
    __in ULONG ResourceLen,
    __out PBOOL pbConflictDetected,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Disable_DevNode(
    __in DEVINST dnDevInst,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Disable_DevNode_Ex(
    __in DEVINST dnDevInst,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#define CM_Disable_DevInst       CM_Disable_DevNode
#define CM_Disable_DevInst_Ex    CM_Disable_DevNode_Ex



CMAPI
CONFIGRET
WINAPI
CM_Disconnect_Machine(
    __in HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Dup_Range_List(
    __in RANGE_LIST rlhOld,
    __in RANGE_LIST rlhNew,
    __in ULONG ulFlags
    );


CMAPI
CONFIGRET
WINAPI
CM_Enable_DevNode(
    __in DEVINST dnDevInst,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Enable_DevNode_Ex(
    __in DEVINST dnDevInst,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#define CM_Enable_DevInst        CM_Enable_DevNode
#define CM_Enable_DevInst_Ex     CM_Enable_DevNode_Ex



CMAPI
CONFIGRET
WINAPI
CM_Enumerate_Classes(
    __in ULONG ulClassIndex,
    __out LPGUID ClassGuid,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Enumerate_Classes_Ex(
    __in ULONG ulClassIndex,
    __out LPGUID ClassGuid,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Enumerate_EnumeratorsA(
    __in ULONG ulEnumIndex,
    __out_ecount_part(*pulLength, *pulLength) PSTR Buffer,
    __inout PULONG pulLength,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Enumerate_EnumeratorsW(
    __in ULONG ulEnumIndex,
    __out_ecount_part(*pulLength, *pulLength) PWSTR Buffer,
    __inout PULONG pulLength,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Enumerate_Enumerators_ExA(
    __in ULONG ulEnumIndex,
    __out_ecount_part(*pulLength, *pulLength) PSTR Buffer,
    __inout PULONG pulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Enumerate_Enumerators_ExW(
    __in ULONG ulEnumIndex,
    __out_ecount_part(*pulLength, *pulLength) PWSTR Buffer,
    __inout PULONG pulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#ifdef UNICODE
#define CM_Enumerate_Enumerators       CM_Enumerate_EnumeratorsW
#define CM_Enumerate_Enumerators_Ex    CM_Enumerate_Enumerators_ExW
#else
#define CM_Enumerate_Enumerators       CM_Enumerate_EnumeratorsA
#define CM_Enumerate_Enumerators_Ex    CM_Enumerate_Enumerators_ExA
#endif // UNICODE


CMAPI
CONFIGRET
WINAPI
CM_Find_Range(
    __out PDWORDLONG pullStart,
    __in DWORDLONG ullStart,
    __in ULONG ulLength,
    __in DWORDLONG ullAlignment,
    __in DWORDLONG ullEnd,
    __in RANGE_LIST rlh,
    __in ULONG ulFlags
    );


CMAPI
CONFIGRET
WINAPI
CM_First_Range(
    __in RANGE_LIST rlh,
    __out PDWORDLONG pullStart,
    __out PDWORDLONG pullEnd,
    __out PRANGE_ELEMENT preElement,
    __in ULONG ulFlags
    );


CMAPI
CONFIGRET
WINAPI
CM_Free_Log_Conf(
    __in LOG_CONF lcLogConfToBeFreed,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Free_Log_Conf_Ex(
    __in LOG_CONF lcLogConfToBeFreed,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Free_Log_Conf_Handle(
    __in LOG_CONF lcLogConf
    );


CMAPI
CONFIGRET
WINAPI
CM_Free_Range_List(
    __in RANGE_LIST rlh,
    __in ULONG ulFlags
    );


CMAPI
CONFIGRET
WINAPI
CM_Free_Res_Des(
    __out PRES_DES prdResDes,
    __in RES_DES rdResDes,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Free_Res_Des_Ex(
    __out PRES_DES prdResDes,
    __in RES_DES rdResDes,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Free_Res_Des_Handle(
    __in RES_DES rdResDes
    );


CMAPI
CONFIGRET
WINAPI
CM_Get_Child(
    __out PDEVINST pdnDevInst,
    __in DEVINST dnDevInst,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Child_Ex(
    __out PDEVINST pdnDevInst,
    __in DEVINST dnDevInst,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Get_Class_NameA(
    __in LPGUID ClassGuid,
    __out_ecount_part(*pulLength, *pulLength) PSTR Buffer,
    __inout PULONG pulLength,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Class_NameW(
    __in LPGUID ClassGuid,
    __out_ecount_part(*pulLength, *pulLength) PWSTR Buffer,
    __inout PULONG pulLength,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Class_Name_ExA(
    __in LPGUID ClassGuid,
    __out_ecount_part(*pulLength, *pulLength) PSTR Buffer,
    __inout PULONG pulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Class_Name_ExW(
    __in LPGUID ClassGuid,
    __out_ecount_part(*pulLength, *pulLength) PWSTR Buffer,
    __inout PULONG pulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#ifdef UNICODE
#define CM_Get_Class_Name        CM_Get_Class_NameW
#define CM_Get_Class_Name_Ex     CM_Get_Class_Name_ExW
#else
#define CM_Get_Class_Name        CM_Get_Class_NameA
#define CM_Get_Class_Name_Ex     CM_Get_Class_Name_ExA
#endif // UNICODE


CMAPI
CONFIGRET
WINAPI
CM_Get_Class_Key_NameA(
    __in LPGUID ClassGuid,
    __out_ecount_part(*pulLength, *pulLength) LPSTR pszKeyName,
    __inout PULONG pulLength,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Class_Key_NameW(
    __in LPGUID ClassGuid,
    __out_ecount_part(*pulLength, *pulLength) LPWSTR pszKeyName,
    __inout PULONG pulLength,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Class_Key_Name_ExA(
    __in LPGUID ClassGuid,
    __out_ecount_part(*pulLength, *pulLength) LPSTR pszKeyName,
    __inout PULONG pulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Class_Key_Name_ExW(
    __in LPGUID ClassGuid,
    __out_ecount_part(*pulLength, *pulLength) LPWSTR pszKeyName,
    __inout PULONG pulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#ifdef UNICODE
#define CM_Get_Class_Key_Name        CM_Get_Class_Key_NameW
#define CM_Get_Class_Key_Name_Ex     CM_Get_Class_Key_Name_ExW
#else
#define CM_Get_Class_Key_Name        CM_Get_Class_Key_NameA
#define CM_Get_Class_Key_Name_Ex     CM_Get_Class_Key_Name_ExA
#endif // UNICODE


CMAPI
CONFIGRET
WINAPI
CM_Get_Depth(
    __out PULONG pulDepth,
    __in DEVINST dnDevInst,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Depth_Ex(
    __out PULONG pulDepth,
    __in DEVINST dnDevInst,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Get_Device_IDA(
    __in DEVINST dnDevInst,
    __out_ecount(BufferLen) PSTR Buffer,
    __in ULONG BufferLen,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Device_IDW(
    __in DEVINST dnDevInst,
    __out_ecount(BufferLen) PWSTR Buffer,
    __in ULONG BufferLen,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Device_ID_ExA(
    __in DEVINST dnDevInst,
    __out_ecount(BufferLen) PSTR Buffer,
    __in ULONG BufferLen,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Device_ID_ExW(
    __in DEVINST dnDevInst,
    __out_ecount(BufferLen) PWSTR Buffer,
    __in ULONG BufferLen,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#ifdef UNICODE
#define CM_Get_Device_ID         CM_Get_Device_IDW
#define CM_Get_Device_ID_Ex      CM_Get_Device_ID_ExW
#else
#define CM_Get_Device_ID         CM_Get_Device_IDA
#define CM_Get_Device_ID_Ex      CM_Get_Device_ID_ExA
#endif // UNICODE



CMAPI
CONFIGRET
WINAPI
CM_Get_Device_ID_ListA(
    __in_opt PCSTR pszFilter,
    __out_ecount(BufferLen) PCHAR Buffer,
    __in ULONG BufferLen,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Device_ID_ListW(
    __in_opt PCWSTR pszFilter,
    __out_ecount(BufferLen) PWCHAR Buffer,
    __in ULONG BufferLen,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Device_ID_List_ExA(
    __in_opt PCSTR pszFilter,
    __out_ecount(BufferLen) PCHAR Buffer,
    __in ULONG BufferLen,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Device_ID_List_ExW(
    __in_opt PCWSTR pszFilter,
    __out_ecount(BufferLen) PWCHAR Buffer,
    __in ULONG BufferLen,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#ifdef UNICODE
#define CM_Get_Device_ID_List       CM_Get_Device_ID_ListW
#define CM_Get_Device_ID_List_Ex    CM_Get_Device_ID_List_ExW
#else
#define CM_Get_Device_ID_List       CM_Get_Device_ID_ListA
#define CM_Get_Device_ID_List_Ex    CM_Get_Device_ID_List_ExA
#endif // UNICODE



CMAPI
CONFIGRET
WINAPI
CM_Get_Device_ID_List_SizeA(
    __out PULONG pulLen,
    __in_opt PCSTR pszFilter,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Device_ID_List_SizeW(
    __out PULONG pulLen,
    __in_opt PCWSTR pszFilter,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Device_ID_List_Size_ExA(
    __out PULONG pulLen,
    __in_opt PCSTR pszFilter,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Device_ID_List_Size_ExW(
    __out PULONG pulLen,
    __in_opt PCWSTR pszFilter,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#ifdef UNICODE
#define CM_Get_Device_ID_List_Size    CM_Get_Device_ID_List_SizeW
#define CM_Get_Device_ID_List_Size_Ex CM_Get_Device_ID_List_Size_ExW
#else
#define CM_Get_Device_ID_List_Size    CM_Get_Device_ID_List_SizeA
#define CM_Get_Device_ID_List_Size_Ex CM_Get_Device_ID_List_Size_ExA
#endif // UNICODE



CMAPI
CONFIGRET
WINAPI
CM_Get_Device_ID_Size(
    __out PULONG pulLen,
    __in DEVINST dnDevInst,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Device_ID_Size_Ex(
    __out PULONG pulLen,
    __in DEVINST dnDevInst,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );



CMAPI
CONFIGRET
WINAPI
CM_Get_DevNode_Registry_PropertyA(
    __in DEVINST dnDevInst,
    __in ULONG ulProperty,
    __out_opt PULONG pulRegDataType,
    __out_bcount_part_opt(*pulLength, *pulLength) PVOID Buffer,
    __inout PULONG pulLength,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_DevNode_Registry_PropertyW(
    __in DEVINST dnDevInst,
    __in ULONG ulProperty,
    __out_opt PULONG pulRegDataType,
    __out_bcount_part_opt(*pulLength, *pulLength) PVOID Buffer,
    __inout PULONG pulLength,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_DevNode_Registry_Property_ExA(
    __in DEVINST dnDevInst,
    __in ULONG ulProperty,
    __out_opt PULONG pulRegDataType,
    __out_bcount_part_opt(*pulLength, *pulLength) PVOID Buffer,
    __inout PULONG pulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_DevNode_Registry_Property_ExW(
    __in DEVINST dnDevInst,
    __in ULONG ulProperty,
    __out_opt PULONG pulRegDataType,
    __out_bcount_part_opt(*pulLength, *pulLength) PVOID Buffer,
    __inout PULONG pulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#define CM_Get_DevInst_Registry_PropertyW     CM_Get_DevNode_Registry_PropertyW
#define CM_Get_DevInst_Registry_PropertyA     CM_Get_DevNode_Registry_PropertyA
#define CM_Get_DevInst_Registry_Property_ExW  CM_Get_DevNode_Registry_Property_ExW
#define CM_Get_DevInst_Registry_Property_ExA  CM_Get_DevNode_Registry_Property_ExA
#ifdef UNICODE
#define CM_Get_DevInst_Registry_Property      CM_Get_DevNode_Registry_PropertyW
#define CM_Get_DevInst_Registry_Property_Ex   CM_Get_DevNode_Registry_Property_ExW
#define CM_Get_DevNode_Registry_Property      CM_Get_DevNode_Registry_PropertyW
#define CM_Get_DevNode_Registry_Property_Ex   CM_Get_DevNode_Registry_Property_ExW
#else
#define CM_Get_DevInst_Registry_Property      CM_Get_DevNode_Registry_PropertyA
#define CM_Get_DevInst_Registry_Property_Ex   CM_Get_DevNode_Registry_Property_ExA
#define CM_Get_DevNode_Registry_Property      CM_Get_DevNode_Registry_PropertyA
#define CM_Get_DevNode_Registry_Property_Ex   CM_Get_DevNode_Registry_Property_ExA
#endif // UNICODE


CMAPI
CONFIGRET
WINAPI
CM_Get_DevNode_Custom_PropertyA(
    __in DEVINST dnDevInst,
    __in PCSTR pszCustomPropertyName,
    __out_opt PULONG pulRegDataType,
    __out_bcount_part_opt(*pulLength, *pulLength) PVOID Buffer,
    __inout PULONG pulLength,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_DevNode_Custom_PropertyW(
    __in DEVINST dnDevInst,
    __in PCWSTR pszCustomPropertyName,
    __out_opt PULONG pulRegDataType,
    __out_bcount_part_opt(*pulLength, *pulLength) PVOID Buffer,
    __inout PULONG pulLength,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_DevNode_Custom_Property_ExA(
    __in DEVINST dnDevInst,
    __in PCSTR pszCustomPropertyName,
    __out_opt PULONG pulRegDataType,
    __out_bcount_part_opt(*pulLength, *pulLength) PVOID Buffer,
    __inout PULONG pulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_DevNode_Custom_Property_ExW(
    __in DEVINST dnDevInst,
    __in PCWSTR pszCustomPropertyName,
    __out_opt PULONG pulRegDataType,
    __out_bcount_part_opt(*pulLength, *pulLength) PVOID Buffer,
    __inout PULONG pulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#define CM_Get_DevInst_Custom_PropertyW     CM_Get_DevNode_Custom_PropertyW
#define CM_Get_DevInst_Custom_PropertyA     CM_Get_DevNode_Custom_PropertyA
#define CM_Get_DevInst_Custom_Property_ExW  CM_Get_DevNode_Custom_Property_ExW
#define CM_Get_DevInst_Custom_Property_ExA  CM_Get_DevNode_Custom_Property_ExA
#ifdef UNICODE
#define CM_Get_DevInst_Custom_Property      CM_Get_DevNode_Custom_PropertyW
#define CM_Get_DevInst_Custom_Property_Ex   CM_Get_DevNode_Custom_Property_ExW
#define CM_Get_DevNode_Custom_Property      CM_Get_DevNode_Custom_PropertyW
#define CM_Get_DevNode_Custom_Property_Ex   CM_Get_DevNode_Custom_Property_ExW
#else
#define CM_Get_DevInst_Custom_Property      CM_Get_DevNode_Custom_PropertyA
#define CM_Get_DevInst_Custom_Property_Ex   CM_Get_DevNode_Custom_Property_ExA
#define CM_Get_DevNode_Custom_Property      CM_Get_DevNode_Custom_PropertyA
#define CM_Get_DevNode_Custom_Property_Ex   CM_Get_DevNode_Custom_Property_ExA
#endif // UNICODE


CMAPI
CONFIGRET
WINAPI
CM_Get_DevNode_Status(
    __out PULONG pulStatus,
    __out PULONG pulProblemNumber,
    __in DEVINST dnDevInst,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_DevNode_Status_Ex(
    __out PULONG pulStatus,
    __out PULONG pulProblemNumber,
    __in DEVINST dnDevInst,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#define CM_Get_DevInst_Status    CM_Get_DevNode_Status
#define CM_Get_DevInst_Status_Ex CM_Get_DevNode_Status_Ex


CMAPI
CONFIGRET
WINAPI
CM_Get_First_Log_Conf(
    __out_opt PLOG_CONF plcLogConf,
    __in DEVINST dnDevInst,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_First_Log_Conf_Ex(
    __out_opt PLOG_CONF plcLogConf,
    __in DEVINST dnDevInst,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Get_Global_State(
    __out PULONG pulState,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Global_State_Ex(
    __out PULONG pulState,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Get_Hardware_Profile_InfoA(
    __in ULONG ulIndex,
    __out PHWPROFILEINFO_A pHWProfileInfo,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Hardware_Profile_Info_ExA(
    __in ULONG ulIndex,
    __out PHWPROFILEINFO_A pHWProfileInfo,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Hardware_Profile_InfoW(
    __in ULONG ulIndex,
    __out PHWPROFILEINFO_W pHWProfileInfo,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Hardware_Profile_Info_ExW(
    __in ULONG ulIndex,
    __out PHWPROFILEINFO_W pHWProfileInfo,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#ifdef UNICODE
#define CM_Get_Hardware_Profile_Info      CM_Get_Hardware_Profile_InfoW
#define CM_Get_Hardware_Profile_Info_Ex   CM_Get_Hardware_Profile_Info_ExW
#else
#define CM_Get_Hardware_Profile_Info      CM_Get_Hardware_Profile_InfoA
#define CM_Get_Hardware_Profile_Info_Ex   CM_Get_Hardware_Profile_Info_ExA
#endif // UNICODE



CMAPI
CONFIGRET
WINAPI
CM_Get_HW_Prof_FlagsA(
    __in DEVINSTID_A szDevInstName,
    __in ULONG ulHardwareProfile,
    __out PULONG pulValue,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_HW_Prof_FlagsW(
    __in DEVINSTID_W szDevInstName,
    __in ULONG ulHardwareProfile,
    __out PULONG pulValue,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_HW_Prof_Flags_ExA(
    __in DEVINSTID_A szDevInstName,
    __in ULONG ulHardwareProfile,
    __out PULONG pulValue,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_HW_Prof_Flags_ExW(
    __in DEVINSTID_W szDevInstName,
    __in ULONG ulHardwareProfile,
    __out PULONG pulValue,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#ifdef UNICODE
#define CM_Get_HW_Prof_Flags     CM_Get_HW_Prof_FlagsW
#define CM_Get_HW_Prof_Flags_Ex  CM_Get_HW_Prof_Flags_ExW
#else
#define CM_Get_HW_Prof_Flags     CM_Get_HW_Prof_FlagsA
#define CM_Get_HW_Prof_Flags_Ex  CM_Get_HW_Prof_Flags_ExA
#endif // UNICODE


CMAPI
CONFIGRET
WINAPI
CM_Get_Device_Interface_AliasA(
    __in LPCSTR pszDeviceInterface,
    __in LPGUID AliasInterfaceGuid,
    __out_ecount_part(*pulLength, *pulLength) LPSTR pszAliasDeviceInterface,
    __inout PULONG pulLength,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Device_Interface_AliasW(
    __in LPCWSTR pszDeviceInterface,
    __in LPGUID AliasInterfaceGuid,
    __out_ecount_part(*pulLength, *pulLength) LPWSTR pszAliasDeviceInterface,
    __inout PULONG pulLength,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Device_Interface_Alias_ExA(
    __in LPCSTR pszDeviceInterface,
    __in LPGUID AliasInterfaceGuid,
    __out_ecount_part(*pulLength, *pulLength) LPSTR pszAliasDeviceInterface,
    __inout PULONG pulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Device_Interface_Alias_ExW(
    __in LPCWSTR pszDeviceInterface,
    __in LPGUID AliasInterfaceGuid,
    __out_ecount_part(*pulLength, *pulLength) LPWSTR pszAliasDeviceInterface,
    __inout PULONG pulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#ifdef UNICODE
#define CM_Get_Device_Interface_Alias     CM_Get_Device_Interface_AliasW
#define CM_Get_Device_Interface_Alias_Ex  CM_Get_Device_Interface_Alias_ExW
#else
#define CM_Get_Device_Interface_Alias     CM_Get_Device_Interface_AliasA
#define CM_Get_Device_Interface_Alias_Ex  CM_Get_Device_Interface_Alias_ExA
#endif // UNICODE



CMAPI
CONFIGRET
WINAPI
CM_Get_Device_Interface_ListA(
    __in LPGUID InterfaceClassGuid,
    __in_opt DEVINSTID_A pDeviceID,
    __out_ecount(BufferLen) PCHAR Buffer,
    __in ULONG BufferLen,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Device_Interface_ListW(
    __in LPGUID InterfaceClassGuid,
    __in_opt DEVINSTID_W pDeviceID,
    __out_ecount(BufferLen) PWCHAR Buffer,
    __in ULONG BufferLen,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Device_Interface_List_ExA(
    __in LPGUID InterfaceClassGuid,
    __in_opt DEVINSTID_A pDeviceID,
    __out_ecount(BufferLen) PCHAR Buffer,
    __in ULONG BufferLen,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Device_Interface_List_ExW(
    __in LPGUID InterfaceClassGuid,
    __in_opt DEVINSTID_W pDeviceID,
    __out_ecount(BufferLen) PWCHAR Buffer,
    __in ULONG BufferLen,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#ifdef UNICODE
#define CM_Get_Device_Interface_List     CM_Get_Device_Interface_ListW
#define CM_Get_Device_Interface_List_Ex  CM_Get_Device_Interface_List_ExW
#else
#define CM_Get_Device_Interface_List     CM_Get_Device_Interface_ListA
#define CM_Get_Device_Interface_List_Ex  CM_Get_Device_Interface_List_ExA
#endif // UNICODE


CMAPI
CONFIGRET
WINAPI
CM_Get_Device_Interface_List_SizeA(
    __in PULONG pulLen,
    __in LPGUID InterfaceClassGuid,
    __in_opt DEVINSTID_A pDeviceID,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Device_Interface_List_SizeW(
    __in PULONG pulLen,
    __in LPGUID InterfaceClassGuid,
    __in_opt DEVINSTID_W pDeviceID,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Device_Interface_List_Size_ExA(
    __in PULONG pulLen,
    __in LPGUID InterfaceClassGuid,
    __in_opt DEVINSTID_A pDeviceID,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Device_Interface_List_Size_ExW(
    __in PULONG pulLen,
    __in LPGUID InterfaceClassGuid,
    __in_opt DEVINSTID_W pDeviceID,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#ifdef UNICODE
#define CM_Get_Device_Interface_List_Size     CM_Get_Device_Interface_List_SizeW
#define CM_Get_Device_Interface_List_Size_Ex  CM_Get_Device_Interface_List_Size_ExW
#else
#define CM_Get_Device_Interface_List_Size     CM_Get_Device_Interface_List_SizeA
#define CM_Get_Device_Interface_List_Size_Ex  CM_Get_Device_Interface_List_Size_ExA
#endif // UNICODE


CMAPI
CONFIGRET
WINAPI
CM_Get_Log_Conf_Priority(
    __in LOG_CONF lcLogConf,
    __out PPRIORITY pPriority,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Log_Conf_Priority_Ex(
    __in LOG_CONF lcLogConf,
    __out PPRIORITY pPriority,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Get_Next_Log_Conf(
    __out_opt PLOG_CONF plcLogConf,
    __in LOG_CONF lcLogConf,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Next_Log_Conf_Ex(
    __out_opt PLOG_CONF plcLogConf,
    __in LOG_CONF lcLogConf,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Get_Parent(
    __out PDEVINST pdnDevInst,
    __in DEVINST dnDevInst,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Parent_Ex(
    __out PDEVINST pdnDevInst,
    __in DEVINST dnDevInst,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );

CMAPI
CONFIGRET
WINAPI
CM_Get_Res_Des_Data(
    __in RES_DES rdResDes,
    __out_bcount(BufferLen) PVOID Buffer,
    __in ULONG BufferLen,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Res_Des_Data_Ex(
    __in RES_DES rdResDes,
    __out_bcount(BufferLen) PVOID Buffer,
    __in ULONG BufferLen,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Get_Res_Des_Data_Size(
    __out PULONG pulSize,
    __in RES_DES rdResDes,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Res_Des_Data_Size_Ex(
    __out PULONG pulSize,
    __in RES_DES rdResDes,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Get_Sibling(
    __out PDEVINST pdnDevInst,
    __in DEVINST DevInst,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Sibling_Ex(
    __out PDEVINST pdnDevInst,
    __in DEVINST DevInst,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );



CMAPI
WORD
WINAPI
CM_Get_Version(
    VOID
    );
CMAPI
WORD
WINAPI
CM_Get_Version_Ex(
    __in_opt HMACHINE hMachine
    );



CMAPI
BOOL
WINAPI
CM_Is_Version_Available(
    __in WORD wVersion
    );
CMAPI
BOOL
WINAPI
CM_Is_Version_Available_Ex(
    __in WORD wVersion,
    __in_opt HMACHINE hMachine
    );



CMAPI
CONFIGRET
WINAPI
CM_Intersect_Range_List(
    __in RANGE_LIST rlhOld1,
    __in RANGE_LIST rlhOld2,
    __in RANGE_LIST rlhNew,
    __in ULONG ulFlags
    );


CMAPI
CONFIGRET
WINAPI
CM_Invert_Range_List(
    __in RANGE_LIST rlhOld,
    __in RANGE_LIST rlhNew,
    __in DWORDLONG ullMaxValue,
    __in ULONG ulFlags
    );


CMAPI
CONFIGRET
WINAPI
CM_Locate_DevNodeA(
    __out PDEVINST pdnDevInst,
    __in_opt DEVINSTID_A pDeviceID,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Locate_DevNodeW(
    __out PDEVINST pdnDevInst,
    __in_opt DEVINSTID_W pDeviceID,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Locate_DevNode_ExA(
    __out PDEVINST pdnDevInst,
    __in_opt DEVINSTID_A pDeviceID,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Locate_DevNode_ExW(
    __out PDEVINST pdnDevInst,
    __in_opt DEVINSTID_W pDeviceID,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#define CM_Locate_DevInstA       CM_Locate_DevNodeA
#define CM_Locate_DevInstW       CM_Locate_DevNodeW
#define CM_Locate_DevInst_ExA    CM_Locate_DevNode_ExA
#define CM_Locate_DevInst_ExW    CM_Locate_DevNode_ExW
#ifdef UNICODE
#define CM_Locate_DevNode        CM_Locate_DevNodeW
#define CM_Locate_DevInst        CM_Locate_DevNodeW
#define CM_Locate_DevNode_Ex     CM_Locate_DevNode_ExW
#define CM_Locate_DevInst_Ex     CM_Locate_DevNode_ExW
#else
#define CM_Locate_DevNode        CM_Locate_DevNodeA
#define CM_Locate_DevInst        CM_Locate_DevNodeA
#define CM_Locate_DevNode_Ex     CM_Locate_DevNode_ExA
#define CM_Locate_DevInst_Ex     CM_Locate_DevNode_ExA
#endif // UNICODE


CMAPI
CONFIGRET
WINAPI
CM_Merge_Range_List(
    __in RANGE_LIST rlhOld1,
    __in RANGE_LIST rlhOld2,
    __in RANGE_LIST rlhNew,
    __in ULONG ulFlags
    );


CMAPI
CONFIGRET
WINAPI
CM_Modify_Res_Des(
    __out PRES_DES prdResDes,
    __in RES_DES rdResDes,
    __in RESOURCEID ResourceID,
    __in_bcount(ResourceLen) PCVOID ResourceData,
    __in ULONG ResourceLen,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Modify_Res_Des_Ex(
    __out PRES_DES prdResDes,
    __in RES_DES rdResDes,
    __in RESOURCEID ResourceID,
    __in_bcount(ResourceLen) PCVOID ResourceData,
    __in ULONG ResourceLen,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Move_DevNode(
    __in DEVINST dnFromDevInst,
    __in DEVINST dnToDevInst,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Move_DevNode_Ex(
    __in DEVINST dnFromDevInst,
    __in DEVINST dnToDevInst,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#define CM_Move_DevInst          CM_Move_DevNode
#define CM_Move_DevInst_Ex       CM_Move_DevNode_Ex


CMAPI
CONFIGRET
WINAPI
CM_Next_Range(
    __inout PRANGE_ELEMENT preElement,
    __out PDWORDLONG pullStart,
    __out PDWORDLONG pullEnd,
    __in ULONG ulFlags
    );


CMAPI
CONFIGRET
WINAPI
CM_Get_Next_Res_Des(
    __out PRES_DES prdResDes,
    __in RES_DES rdResDes,
    __in RESOURCEID ForResource,
    __out_opt PRESOURCEID pResourceID,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Get_Next_Res_Des_Ex(
    __out PRES_DES prdResDes,
    __in RES_DES rdResDes,
    __in RESOURCEID ForResource,
    __out_opt PRESOURCEID pResourceID,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Open_Class_KeyA(
    __in_opt LPGUID ClassGuid,
    __in_opt LPCSTR pszClassName,
    __in REGSAM samDesired,
    __in REGDISPOSITION Disposition,
    __out PHKEY phkClass,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Open_Class_KeyW(
    __in_opt LPGUID ClassGuid,
    __in_opt LPCWSTR pszClassName,
    __in REGSAM samDesired,
    __in REGDISPOSITION Disposition,
    __out PHKEY phkClass,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Open_Class_Key_ExA(
    __in_opt LPGUID pszClassGuid,
    __in_opt LPCSTR pszClassName,
    __in REGSAM samDesired,
    __in REGDISPOSITION Disposition,
    __out PHKEY phkClass,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Open_Class_Key_ExW(
    __in_opt LPGUID pszClassGuid,
    __in_opt LPCWSTR pszClassName,
    __in REGSAM samDesired,
    __in REGDISPOSITION Disposition,
    __out PHKEY phkClass,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );

#ifdef UNICODE
#define CM_Open_Class_Key        CM_Open_Class_KeyW
#define CM_Open_Class_Key_Ex     CM_Open_Class_Key_ExW
#else
#define CM_Open_Class_Key        CM_Open_Class_KeyA
#define CM_Open_Class_Key_Ex     CM_Open_Class_Key_ExA
#endif // UNICODE



CMAPI
CONFIGRET
WINAPI
CM_Open_DevNode_Key(
    __in DEVINST dnDevNode,
    __in REGSAM samDesired,
    __in ULONG ulHardwareProfile,
    __in REGDISPOSITION Disposition,
    __out PHKEY phkDevice,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Open_DevNode_Key_Ex(
    __in DEVINST dnDevNode,
    __in REGSAM samDesired,
    __in ULONG ulHardwareProfile,
    __in REGDISPOSITION Disposition,
    __out PHKEY phkDevice,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#define CM_Open_DevInst_Key      CM_Open_DevNode_Key
#define CM_Open_DevInst_Key_Ex   CM_Open_DevNode_Key_Ex


CMAPI
CONFIGRET
WINAPI
CM_Query_Arbitrator_Free_Data(
    __out_bcount(DataLen) PVOID pData,
    __in ULONG DataLen,
    __in DEVINST dnDevInst,
    __in RESOURCEID ResourceID,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Query_Arbitrator_Free_Data_Ex(
    __out_bcount(DataLen) PVOID pData,
    __in ULONG DataLen,
    __in DEVINST dnDevInst,
    __in RESOURCEID ResourceID,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Query_Arbitrator_Free_Size(
    __out PULONG pulSize,
    __in DEVINST dnDevInst,
    __in RESOURCEID ResourceID,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Query_Arbitrator_Free_Size_Ex(
    __out PULONG pulSize,
    __in DEVINST dnDevInst,
    __in RESOURCEID ResourceID,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Query_Remove_SubTree(
    __in DEVINST dnAncestor,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Query_Remove_SubTree_Ex(
    __in DEVINST dnAncestor,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Query_And_Remove_SubTreeA(
    __in DEVINST dnAncestor,
    __out_opt PPNP_VETO_TYPE pVetoType,
    __out_ecount_opt(ulNameLength) LPSTR pszVetoName,
    __in ULONG ulNameLength,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Query_And_Remove_SubTree_ExA(
    __in DEVINST dnAncestor,
    __out_opt PPNP_VETO_TYPE pVetoType,
    __out_ecount_opt(ulNameLength) LPSTR pszVetoName,
    __in ULONG ulNameLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );

CMAPI
CONFIGRET
WINAPI
CM_Query_And_Remove_SubTreeW(
    __in DEVINST dnAncestor,
    __out_opt PPNP_VETO_TYPE pVetoType,
    __out_ecount_opt(ulNameLength) LPWSTR pszVetoName,
    __in ULONG ulNameLength,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Query_And_Remove_SubTree_ExW(
    __in DEVINST dnAncestor,
    __out_opt PPNP_VETO_TYPE pVetoType,
    __out_ecount_opt(ulNameLength) LPWSTR pszVetoName,
    __in ULONG ulNameLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );

#ifdef UNICODE
#define CM_Query_And_Remove_SubTree     CM_Query_And_Remove_SubTreeW
#define CM_Query_And_Remove_SubTree_Ex  CM_Query_And_Remove_SubTree_ExW
#else
#define CM_Query_And_Remove_SubTree     CM_Query_And_Remove_SubTreeA
#define CM_Query_And_Remove_SubTree_Ex  CM_Query_And_Remove_SubTree_ExA
#endif // UNICODE

CMAPI
CONFIGRET
WINAPI
CM_Request_Device_EjectA(
    __in DEVINST dnDevInst,
    __out_opt PPNP_VETO_TYPE pVetoType,
    __out_ecount_opt(ulNameLength) LPSTR pszVetoName,
    __in ULONG ulNameLength,
    __in ULONG ulFlags
    );

CMAPI
CONFIGRET
WINAPI
CM_Request_Device_Eject_ExA(
    __in DEVINST dnDevInst,
    __out_opt PPNP_VETO_TYPE pVetoType,
    __out_ecount_opt(ulNameLength) LPSTR pszVetoName,
    __in ULONG ulNameLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );

CMAPI
CONFIGRET
WINAPI
CM_Request_Device_EjectW(
    __in DEVINST dnDevInst,
    __out_opt PPNP_VETO_TYPE pVetoType,
    __out_ecount_opt(ulNameLength) LPWSTR pszVetoName,
    __in ULONG ulNameLength,
    __in ULONG ulFlags
    );

CMAPI
CONFIGRET
WINAPI
CM_Request_Device_Eject_ExW(
    __in DEVINST dnDevInst,
    __out_opt PPNP_VETO_TYPE pVetoType,
    __out_ecount_opt(ulNameLength) LPWSTR pszVetoName,
    __in ULONG ulNameLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );

#ifdef UNICODE
#define CM_Request_Device_Eject         CM_Request_Device_EjectW
#define CM_Request_Device_Eject_Ex      CM_Request_Device_Eject_ExW
#else
#define CM_Request_Device_Eject         CM_Request_Device_EjectA
#define CM_Request_Device_Eject_Ex      CM_Request_Device_Eject_ExA
#endif // UNICODE

CMAPI
CONFIGRET
WINAPI
CM_Reenumerate_DevNode(
    __in DEVINST dnDevInst,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Reenumerate_DevNode_Ex(
    __in DEVINST dnDevInst,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#define CM_Reenumerate_DevInst      CM_Reenumerate_DevNode
#define CM_Reenumerate_DevInst_Ex   CM_Reenumerate_DevNode_Ex


CMAPI
CONFIGRET
WINAPI
CM_Register_Device_InterfaceA(
    __in DEVINST dnDevInst,
    __in LPGUID InterfaceClassGuid,
    __in_opt LPCSTR pszReference,
    __out_ecount_part(*pulLength, *pulLength) LPSTR pszDeviceInterface,
    __inout PULONG pulLength,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Register_Device_InterfaceW(
    __in DEVINST dnDevInst,
    __in LPGUID InterfaceClassGuid,
    __in_opt LPCWSTR pszReference,
    __out_ecount_part(*pulLength, *pulLength) LPWSTR pszDeviceInterface,
    __inout PULONG pulLength,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Register_Device_Interface_ExA(
    __in DEVINST dnDevInst,
    __in LPGUID InterfaceClassGuid,
    __in_opt LPCSTR pszReference,
    __out_ecount_part(*pulLength, *pulLength) LPSTR pszDeviceInterface,
    __inout PULONG pulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Register_Device_Interface_ExW(
    __in DEVINST dnDevInst,
    __in LPGUID InterfaceClassGuid,
    __in_opt LPCWSTR pszReference,
    __out_ecount_part(*pulLength, *pulLength) LPWSTR pszDeviceInterface,
    __inout PULONG pulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#ifdef UNICODE
#define CM_Register_Device_Interface    CM_Register_Device_InterfaceW
#define CM_Register_Device_Interface_Ex CM_Register_Device_Interface_ExW
#else
#define CM_Register_Device_Interface    CM_Register_Device_InterfaceA
#define CM_Register_Device_Interface_Ex CM_Register_Device_Interface_ExA
#endif // UNICODE


CMAPI
CONFIGRET
WINAPI
CM_Set_DevNode_Problem_Ex(
    __in DEVINST dnDevInst,
    __in ULONG ulProblem,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Set_DevNode_Problem(
    __in DEVINST dnDevInst,
    __in ULONG ulProblem,
    __in ULONG ulFlags
    );
#define CM_Set_DevInst_Problem      CM_Set_DevNode_Problem
#define CM_Set_DevInst_Problem_Ex   CM_Set_DevNode_Problem_Ex


CMAPI
CONFIGRET
WINAPI
CM_Unregister_Device_InterfaceA(
    __in LPCSTR pszDeviceInterface,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Unregister_Device_InterfaceW(
    __in LPCWSTR pszDeviceInterface,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Unregister_Device_Interface_ExA(
    __in LPCSTR pszDeviceInterface,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Unregister_Device_Interface_ExW(
    __in LPCWSTR pszDeviceInterface,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#ifdef UNICODE
#define CM_Unregister_Device_Interface    CM_Unregister_Device_InterfaceW
#define CM_Unregister_Device_Interface_Ex CM_Unregister_Device_Interface_ExW
#else
#define CM_Unregister_Device_Interface    CM_Unregister_Device_InterfaceA
#define CM_Unregister_Device_Interface_Ex CM_Unregister_Device_Interface_ExA
#endif // UNICODE


CMAPI
CONFIGRET
WINAPI
CM_Register_Device_Driver(
    __in DEVINST dnDevInst,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Register_Device_Driver_Ex(
    __in DEVINST dnDevInst,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );

CMAPI
CONFIGRET
WINAPI
CM_Remove_SubTree(
    __in DEVINST dnAncestor,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Remove_SubTree_Ex(
    __in DEVINST dnAncestor,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );



CMAPI
CONFIGRET
WINAPI
CM_Set_DevNode_Registry_PropertyA(
    __in DEVINST dnDevInst,
    __in ULONG ulProperty,
    __in_bcount_opt(ulLength) PCVOID Buffer,
    __in ULONG ulLength,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Set_DevNode_Registry_PropertyW(
    __in DEVINST dnDevInst,
    __in ULONG ulProperty,
    __in_bcount_opt(ulLength) PCVOID Buffer,
    __in ULONG ulLength,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Set_DevNode_Registry_Property_ExA(
    __in DEVINST dnDevInst,
    __in ULONG ulProperty,
    __in_bcount_opt(ulLength) PCVOID Buffer,
    __in ULONG ulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Set_DevNode_Registry_Property_ExW(
    __in DEVINST dnDevInst,
    __in ULONG ulProperty,
    __in_bcount_opt(ulLength) PCVOID Buffer,
    __in ULONG ulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#define CM_Set_DevInst_Registry_PropertyW     CM_Set_DevNode_Registry_PropertyW
#define CM_Set_DevInst_Registry_PropertyA     CM_Set_DevNode_Registry_PropertyA
#define CM_Set_DevInst_Registry_Property_ExW  CM_Set_DevNode_Registry_Property_ExW
#define CM_Set_DevInst_Registry_Property_ExA  CM_Set_DevNode_Registry_Property_ExA
#ifdef UNICODE
#define CM_Set_DevInst_Registry_Property      CM_Set_DevNode_Registry_PropertyW
#define CM_Set_DevInst_Registry_Property_Ex   CM_Set_DevNode_Registry_Property_ExW
#define CM_Set_DevNode_Registry_Property      CM_Set_DevNode_Registry_PropertyW
#define CM_Set_DevNode_Registry_Property_Ex   CM_Set_DevNode_Registry_Property_ExW
#else
#define CM_Set_DevInst_Registry_Property      CM_Set_DevNode_Registry_PropertyA
#define CM_Set_DevInst_Registry_Property_Ex   CM_Set_DevNode_Registry_Property_ExA
#define CM_Set_DevNode_Registry_Property      CM_Set_DevNode_Registry_PropertyA
#define CM_Set_DevNode_Registry_Property_Ex   CM_Set_DevNode_Registry_Property_ExA
#endif // UNICODE


CMAPI
CONFIGRET
WINAPI
CM_Is_Dock_Station_Present(
    __out PBOOL pbPresent
    );

CMAPI
CONFIGRET
WINAPI
CM_Is_Dock_Station_Present_Ex(
    __out PBOOL pbPresent,
    __in_opt HMACHINE hMachine
    );

CMAPI
CONFIGRET
WINAPI
CM_Request_Eject_PC(
    VOID
    );

CMAPI
CONFIGRET
WINAPI
CM_Request_Eject_PC_Ex(
    __in_opt HMACHINE hMachine
    );

CMAPI
CONFIGRET
WINAPI
CM_Set_HW_Prof_FlagsA(
    __in DEVINSTID_A szDevInstName,
    __in ULONG ulConfig,
    __in ULONG ulValue,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Set_HW_Prof_FlagsW(
    __in DEVINSTID_W szDevInstName,
    __in ULONG ulConfig,
    __in ULONG ulValue,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Set_HW_Prof_Flags_ExA(
    __in DEVINSTID_A szDevInstName,
    __in ULONG ulConfig,
    __in ULONG ulValue,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
CMAPI
CONFIGRET
WINAPI
CM_Set_HW_Prof_Flags_ExW(
    __in DEVINSTID_W szDevInstName,
    __in ULONG ulConfig,
    __in ULONG ulValue,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#ifdef UNICODE
#define CM_Set_HW_Prof_Flags     CM_Set_HW_Prof_FlagsW
#define CM_Set_HW_Prof_Flags_Ex  CM_Set_HW_Prof_Flags_ExW
#else
#define CM_Set_HW_Prof_Flags     CM_Set_HW_Prof_FlagsA
#define CM_Set_HW_Prof_Flags_Ex  CM_Set_HW_Prof_Flags_ExA
#endif // UNICODE



CMAPI
CONFIGRET
WINAPI
CM_Setup_DevNode(
    __in DEVINST dnDevInst,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Setup_DevNode_Ex(
    __in DEVINST dnDevInst,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );
#define CM_Setup_DevInst         CM_Setup_DevNode
#define CM_Setup_DevInst_Ex      CM_Setup_DevNode_Ex


CMAPI
CONFIGRET
WINAPI
CM_Test_Range_Available(
    __in DWORDLONG ullStartValue,
    __in DWORDLONG ullEndValue,
    __in RANGE_LIST rlh,
    __in ULONG ulFlags
    );


CMAPI
CONFIGRET
WINAPI
CM_Uninstall_DevNode(
    __in DEVNODE dnPhantom,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Uninstall_DevNode_Ex(
    __in DEVNODE dnPhantom,
    __in ULONG ulFlags,
    __in_opt HANDLE hMachine
    );
#define CM_Uninstall_DevInst     CM_Uninstall_DevNode
#define CM_Uninstall_DevInst_Ex  CM_Uninstall_DevNode_Ex


CMAPI
CONFIGRET
WINAPI
CM_Run_Detection(
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Run_Detection_Ex(
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );


CMAPI
CONFIGRET
WINAPI
CM_Set_HW_Prof(
    __in ULONG ulHardwareProfile,
    __in ULONG ulFlags
    );
CMAPI
CONFIGRET
WINAPI
CM_Set_HW_Prof_Ex(
    __in ULONG ulHardwareProfile,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );

CMAPI
CONFIGRET
WINAPI
CM_Query_Resource_Conflict_List(
    __out PCONFLICT_LIST pclConflictList,
    __in DEVINST dnDevInst,
    __in RESOURCEID ResourceID,
    __in_bcount(ResourceLen) PCVOID ResourceData,
    __in ULONG ResourceLen,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );

CMAPI
CONFIGRET
WINAPI
CM_Free_Resource_Conflict_Handle(
    __in CONFLICT_LIST clConflictList
    );

CMAPI
CONFIGRET
WINAPI
CM_Get_Resource_Conflict_Count(
    __in CONFLICT_LIST clConflictList,
    __out PULONG pulCount
    );

CMAPI
CONFIGRET
WINAPI
CM_Get_Resource_Conflict_DetailsA(
    __in CONFLICT_LIST clConflictList,
    __in ULONG ulIndex,
    __inout PCONFLICT_DETAILS_A pConflictDetails
    );

CMAPI
CONFIGRET
WINAPI
CM_Get_Resource_Conflict_DetailsW(
    __in CONFLICT_LIST clConflictList,
    __in ULONG ulIndex,
    __inout PCONFLICT_DETAILS_W pConflictDetails
    );

#ifdef UNICODE
#define CM_Get_Resource_Conflict_Details CM_Get_Resource_Conflict_DetailsW
#else
#define CM_Get_Resource_Conflict_Details CM_Get_Resource_Conflict_DetailsA
#endif // UNICODE

CMAPI
CONFIGRET
WINAPI
CM_Get_Class_Registry_PropertyW(
    __in LPGUID ClassGUID,
    __in ULONG ulProperty,
    __out_opt PULONG pulRegDataType,
    __out_bcount_part_opt(*pulLength, *pulLength) PVOID Buffer,
    __inout PULONG pulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );

CMAPI
CONFIGRET
WINAPI
CM_Set_Class_Registry_PropertyW(
    __in LPGUID ClassGUID,
    __in ULONG ulProperty,
    __in_bcount_opt(ulLength) PCVOID Buffer,
    __in ULONG ulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );

CMAPI
CONFIGRET
WINAPI
CM_Get_Class_Registry_PropertyA(
    __in LPGUID ClassGUID,
    __in ULONG ulProperty,
    __out_opt PULONG pulRegDataType,
    __out_bcount_part_opt(*pulLength, *pulLength) PVOID Buffer,
    __inout PULONG pulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );

CMAPI
CONFIGRET
WINAPI
CM_Set_Class_Registry_PropertyA(
    __in LPGUID ClassGUID,
    __in ULONG ulProperty,
    __in_bcount_opt(ulLength) PCVOID Buffer,
    __in ULONG ulLength,
    __in ULONG ulFlags,
    __in_opt HMACHINE hMachine
    );

#ifdef UNICODE
#define CM_Get_Class_Registry_Property CM_Get_Class_Registry_PropertyW
#define CM_Set_Class_Registry_Property CM_Set_Class_Registry_PropertyW
#else
#define CM_Get_Class_Registry_Property CM_Get_Class_Registry_PropertyA
#define CM_Set_Class_Registry_Property CM_Set_Class_Registry_PropertyA
#endif // UNICODE

#define CM_WaitNoPendingInstallEvents CMP_WaitNoPendingInstallEvents

DWORD
WINAPI
CM_WaitNoPendingInstallEvents(
    __in DWORD dwTimeout
    );

//--------------------------------------------------------------
// Configuration Manager return status codes
//--------------------------------------------------------------

#define CR_SUCCESS                  (0x00000000)
#define CR_DEFAULT                  (0x00000001)
#define CR_OUT_OF_MEMORY            (0x00000002)
#define CR_INVALID_POINTER          (0x00000003)
#define CR_INVALID_FLAG             (0x00000004)
#define CR_INVALID_DEVNODE          (0x00000005)
#define CR_INVALID_DEVINST          CR_INVALID_DEVNODE
#define CR_INVALID_RES_DES          (0x00000006)
#define CR_INVALID_LOG_CONF         (0x00000007)
#define CR_INVALID_ARBITRATOR       (0x00000008)
#define CR_INVALID_NODELIST         (0x00000009)
#define CR_DEVNODE_HAS_REQS         (0x0000000A)
#define CR_DEVINST_HAS_REQS         CR_DEVNODE_HAS_REQS
#define CR_INVALID_RESOURCEID       (0x0000000B)
#define CR_DLVXD_NOT_FOUND          (0x0000000C)   // WIN 95 ONLY
#define CR_NO_SUCH_DEVNODE          (0x0000000D)
#define CR_NO_SUCH_DEVINST          CR_NO_SUCH_DEVNODE
#define CR_NO_MORE_LOG_CONF         (0x0000000E)
#define CR_NO_MORE_RES_DES          (0x0000000F)
#define CR_ALREADY_SUCH_DEVNODE     (0x00000010)
#define CR_ALREADY_SUCH_DEVINST     CR_ALREADY_SUCH_DEVNODE
#define CR_INVALID_RANGE_LIST       (0x00000011)
#define CR_INVALID_RANGE            (0x00000012)
#define CR_FAILURE                  (0x00000013)
#define CR_NO_SUCH_LOGICAL_DEV      (0x00000014)
#define CR_CREATE_BLOCKED           (0x00000015)
#define CR_NOT_SYSTEM_VM            (0x00000016)   // WIN 95 ONLY
#define CR_REMOVE_VETOED            (0x00000017)
#define CR_APM_VETOED               (0x00000018)
#define CR_INVALID_LOAD_TYPE        (0x00000019)
#define CR_BUFFER_SMALL             (0x0000001A)
#define CR_NO_ARBITRATOR            (0x0000001B)
#define CR_NO_REGISTRY_HANDLE       (0x0000001C)
#define CR_REGISTRY_ERROR           (0x0000001D)
#define CR_INVALID_DEVICE_ID        (0x0000001E)
#define CR_INVALID_DATA             (0x0000001F)
#define CR_INVALID_API              (0x00000020)
#define CR_DEVLOADER_NOT_READY      (0x00000021)
#define CR_NEED_RESTART             (0x00000022)
#define CR_NO_MORE_HW_PROFILES      (0x00000023)
#define CR_DEVICE_NOT_THERE         (0x00000024)
#define CR_NO_SUCH_VALUE            (0x00000025)
#define CR_WRONG_TYPE               (0x00000026)
#define CR_INVALID_PRIORITY         (0x00000027)
#define CR_NOT_DISABLEABLE          (0x00000028)
#define CR_FREE_RESOURCES           (0x00000029)
#define CR_QUERY_VETOED             (0x0000002A)
#define CR_CANT_SHARE_IRQ           (0x0000002B)
#define CR_NO_DEPENDENT             (0x0000002C)
#define CR_SAME_RESOURCES           (0x0000002D)
#define CR_NO_SUCH_REGISTRY_KEY     (0x0000002E)
#define CR_INVALID_MACHINENAME      (0x0000002F)   // NT ONLY
#define CR_REMOTE_COMM_FAILURE      (0x00000030)   // NT ONLY
#define CR_MACHINE_UNAVAILABLE      (0x00000031)   // NT ONLY
#define CR_NO_CM_SERVICES           (0x00000032)   // NT ONLY
#define CR_ACCESS_DENIED            (0x00000033)   // NT ONLY
#define CR_CALL_NOT_IMPLEMENTED     (0x00000034)
#define CR_INVALID_PROPERTY         (0x00000035)
#define CR_DEVICE_INTERFACE_ACTIVE  (0x00000036)
#define CR_NO_SUCH_DEVICE_INTERFACE (0x00000037)
#define CR_INVALID_REFERENCE_STRING (0x00000038)
#define CR_INVALID_CONFLICT_LIST    (0x00000039)
#define CR_INVALID_INDEX            (0x0000003A)
#define CR_INVALID_STRUCTURE_SIZE   (0x0000003B)
#define NUM_CR_RESULTS              (0x0000003C)

#ifdef __cplusplus
}
#endif


#endif // _CFGMGR32_


