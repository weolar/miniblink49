/*++ BUILD Version: 0004    // Increment this if a change has global effects
*/
//++
//
// Module Name:
//
//  kxia64.h
//
// Abstract:
//
//  This module contains the nongenerated part of the IA64 assembler
//  header file. In general, it contains processor architecture constant
//  information, however some assembler macros are also included.
//
// Author:
//
//  bjl 12-Jun-95 (based on David N. Cutler (davec) 23-Mar-1990)
//
// Revision History:
//
//--

#define SHADOW_IRQL_IMPLEMENTATION 1

//
// N. B. Register aliases have been moved to ksia64.h (because we
// use assembler aliases not #define's)
//

// Register constants

// For setting non-rotating predicates (not used very much)

#define PS0 0x0001
#define PS1 0x0002
#define PS2 0x0004
#define PS3 0x0008
#define PS4 0x0010
#define PS5 0x0020

#define PRP 0x0080

#define PT0 0x0040
#define PT1 0x0100
#define PT2 0x0200
#define PT3 0x0400
#define PT4 0x0800
#define PT5 0x1000
#define PT6 0x2000
#define PT7 0x4000
#define PT8 0x8000

// For setting nomination register

#define NOM_BS0 0x0001
#define NOM_BS1 0x0002
#define NOM_BS2 0x0004
#define NOM_BS3 0x0008
#define NOM_BS4 0x0010
#define NOM_BS5 0x0020

#define NOM_BRP 0x0080

#define NOM_BT0 0x0040
#define NOM_BT1 0x0100
#define NOM_BT2 0x0200
#define NOM_BT3 0x0400
#define NOM_BT4 0x0800
#define NOM_BT5 0x1000
#define NOM_BT6 0x2000
#define NOM_BT7 0x4000
#define NOM_BT8 0x8000
//
//
// Define IA64 system registers.
//
// Define IA64 system register bit field offsets.
//
// Processor Status Register (PSR) Bit positions

// User / System mask
#define PSR_MBZ4    0
#define PSR_BE      1
#define PSR_UP      2
#define PSR_AC      3
#define PSR_MFL     4
#define PSR_MFH     5
// PSR bits 6-12 reserved (must be zero)
#define PSR_MBZ0    6
#define PSR_MBZ0_V  0x7fi64
// System only mask
#define PSR_IC      13
#define PSR_I       14
#define PSR_PK      15
#define PSR_MBZ1    16
#define PSR_MBZ1_V  0x1i64
#define PSR_DT      17
#define PSR_DFL     18
#define PSR_DFH     19
#define PSR_SP      20
#define PSR_PP      21
#define PSR_DI      22
#define PSR_SI      23
#define PSR_DB      24
#define PSR_LP      25
#define PSR_TB      26
#define PSR_RT      27
// PSR bits 28-31 reserved (must be zero)
#define PSR_MBZ2    28
#define PSR_MBZ2_V  0xfi64
// Neither mask
#define PSR_CPL     32
#define PSR_CPL_LEN 2
#define PSR_IS      34
#define PSR_MC      35
#define PSR_IT      36
#define PSR_ID      37
#define PSR_DA      38
#define PSR_DD      39
#define PSR_SS      40
#define PSR_RI      41
#define PSR_RI_LEN  2
#define PSR_ED      43
#define PSR_BN      44
#define PSR_IA      45
// PSR bits 46-63 reserved (must be zero)
#define PSR_MBZ3    46
#define PSR_MBZ3_V  0x3ffffi64

//
// Privilege levels
//

#define PL_KERNEL    0
#define PL_USER      3

//
// Instruction set (IS) bits
//

#define IS_EM        0
#define IS_IA        1

// Floating Point Status Register (FPSR) Bit positions

// Status Field 0 - Flags
#define FPSR_VD        0
#define FPSR_DD        1
#define FPSR_ZD        2
#define FPSR_OD        3
#define FPSR_UD        4
#define FPSR_ID        5
// Status Field 0 - Controls
#define FPSR_FTZ0      6
#define FPSR_WRE0      7
#define FPSR_PC0       8
#define FPSR_RC0       10
#define FPSR_TD0       12
// Status Field 0 - Flags
#define FPSR_V0        13
#define FPSR_D0        14
#define FPSR_Z0        15
#define FPSR_O0        16
#define FPSR_U0        17
#define FPSR_I0        18
// Status Field 1 - Controls
#define FPSR_FTZ1      19
#define FPSR_WRE1      20
#define FPSR_PC1       21
#define FPSR_RC1       23
#define FPSR_TD1       25
// Status Field 1 - Flags
#define FPSR_V1        26
#define FPSR_D1        27
#define FPSR_Z1        28
#define FPSR_O1        29
#define FPSR_U1        30
#define FPSR_I1        31
// Status Field 2 - Controls
#define FPSR_FTZ2      32
#define FPSR_WRE2      33
#define FPSR_PC2       34
#define FPSR_RC2       36
#define FPSR_TD2       38
// Status Field 2 - Flags
#define FPSR_V2        39
#define FPSR_D2        40
#define FPSR_Z2        41
#define FPSR_O2        42
#define FPSR_U2        43
#define FPSR_I2        44
// Status Field 3 - Controls
#define FPSR_FTZ3      45
#define FPSR_WRE3      46
#define FPSR_PC3       47
#define FPSR_RC3       49
#define FPSR_TD3       51
// Status Field 3 - Flags
#define FPSR_V3        52
#define FPSR_D3        53
#define FPSR_Z3        54
#define FPSR_O3        55
#define FPSR_U3        56
#define FPSR_I3        57
// FPSR bits 58-63 Reserved -- Must be zero
#define FPSR_MBZ0      58
#define FPSR_MBZ0_V    0x3fi64

//
// For setting up FPSR on kernel entry
//
// all FP exceptions masked
//
// rounding to nearest, 64-bit precision, wide range enabled for FPSR.fs1
//
// rounding to nearest, 53-bit precision, wide range disabled for FPSR.fs0
//

#define FPSR_FOR_KERNEL 0x9804C0270033F

//
// Define hardware Task Priority Register (TPR)
//
// TPR bit positions

// Bits 0 - 3 ignored
#define TPR_MIC        4
#define TPR_MIC_LEN    4
// Bits 8 - 15 reserved
// TPR.mmi is always 0 in NT
#define TPR_MMI        16
// Bits 17 - 63 ignored

//
// The current IRQL is maintained in the TPR.mic field. The
// shift count is the number of bits to shift right to extract the
// IRQL from the TPR. See the GET/SET_IRQL macros.
//

#define TPR_IRQL_SHIFT TPR_MIC

//
// To go from vector number <-> IRQL we just do a shift
//

#define VECTOR_IRQL_SHIFT TPR_IRQL_SHIFT

//
// Define hardware Interrupt Status Register (ISR)
//
// ISR bit positions

#define ISR_CODE       0
#define ISR_CODE_LEN   16
#define ISR_CODE_MASK  0xFFFF
#define ISR_NA_CODE_MASK  0xF
#define ISR_IA_VECTOR  16
#define ISR_IA_VECTOR_LEN 8
// ISR bits 24-31 reserved
#define ISR_MBZ0       24
#define ISR_MBZ0_V     0xff
#define ISR_X          32
#define ISR_W          33
#define ISR_R          34
#define ISR_NA         35
#define ISR_SP         36
#define ISR_RS         37
#define ISR_IR         38
#define ISR_NI         39
// ISR bit 40 reserved
#define ISR_MBZ1       40
#define ISR_EI         41
#define ISR_ED         43
// ISR bits 44-63 reserved
#define ISR_MBZ2       44
#define ISR_MBZ2_V     0xfffff

//
// ISR codes for Non-Access Instructions: ISR{3:0}
//

#define ISR_TPA            0        // tpa instruction
#define ISR_FC             1        // fc instruction
#define ISR_PROBE          2        // probe instruction
#define ISR_TAK            3        // tak instruction
#define ISR_LFETCH         4        // lfetch, lfetch.fault instruction
#define ISR_PROBE_FAULT    5        // probe.fault instruction

//
// ISR codes for General Exceptions: ISR{7:4}
//

#define ISR_ILLEGAL_OP     0        // Illegal operation fault
#define ISR_PRIV_OP        1        // Privileged operation fault
#define ISR_PRIV_REG       2        // Privileged register fault
#define ISR_RESVD_REG      3        // Reserved register/field fault
#define ISR_ILLEGAL_ISA    4        // Disabled instruction set transition fault
#define ISR_ILLEGAL_HAZARD 8        // Illegal hazard fault

//
// ISR codes for Nat Consumption Faults: ISR{7:4}
//

#define ISR_NAT_REG     1           // Nat Register Consumption fault
#define ISR_NAT_PAGE    2           // Nat Page Consumption fault

//
// For Traps ISR{3:0}
//

// FP trap
#define ISR_FP_TRAP    0
// Lower privilege transfer trap
#define ISR_LP_TRAP    1
// Taken branch trap
#define ISR_TB_TRAP    2
// Single step trap
#define ISR_SS_TRAP    3
// Unimplemented instruction address trap
#define ISR_UI_TRAP    4

//
// Define hardware Default Control Register (DCR)
//
// DCR bit positions

#define DCR_PP         0
#define DCR_BE         1
#define DCR_LC         2
// DCR bits 3-7 reserved
#define DCR_DM         8
#define DCR_DP         9
#define DCR_DK         10
#define DCR_DX         11
#define DCR_DR         12
#define DCR_DA         13
#define DCR_DD         14
#define DCR_DEFER_ALL  0x7f00
// DCR bits 16-63 reserved
#define DCR_MBZ1       2
#define DCR_MBZ1_V     0xffffffffffffi64

// Define hardware RSE Configuration Register
//
// RS Configuration (RSC) bit field positions

#define RSC_MODE       0
#define RSC_PL         2
#define RSC_BE         4
// RSC bits 5-15 reserved
#define RSC_MBZ0       5
#define RSC_MBZ0_V     0x3ff
#define RSC_LOADRS     16
#define RSC_LOADRS_LEN 14
// RSC bits 30-63 reserved
#define RSC_MBZ1       30
#define RSC_MBZ1_LEN   34
#define RSC_MBZ1_V     0x3ffffffffi64

// RSC modes
// Lazy
#define RSC_MODE_LY (0x0)
// Store intensive
#define RSC_MODE_SI (0x1)
// Load intensive
#define RSC_MODE_LI (0x2)
// Eager
#define RSC_MODE_EA (0x3)

// RSC Endian bit values
#define RSC_BE_LITTLE 0
#define RSC_BE_BIG    1

// RSC while in kernel: enabled, little endian, pl = 0, eager mode
#define RSC_KERNEL ((RSC_MODE_EA<<RSC_MODE) | (RSC_BE_LITTLE<<RSC_BE))
// RSE disabled: disabled, pl = 0, little endian, eager mode
#define RSC_KERNEL_DISABLED ((RSC_MODE_LY<<RSC_MODE) | (RSC_BE_LITTLE<<RSC_BE))

//
// Define Interruption Function State (IFS) Register
//
// IFS bit field positions
//

#define IFS_IFM        0
#define IFS_IFM_LEN    38
#define IFS_MBZ0       38
#define IFS_MBZ0_V     0x1ffffffi64
#define IFS_V          63
#define IFS_V_LEN      1

//
// IFS is valid when IFS_V = IFS_VALID
//

#define IFS_VALID      1

//
// define the width of each size field in PFS/IFS
//

#define PFS_PPL                  62         // pfs.ppl bit location
#define PFS_PPL_LEN              PSR_CPL_LEN
#define PFS_EC_SHIFT             52
#define PFS_EC_SIZE              6
#define PFS_EC_MASK              0x3F
#define PFS_SIZE_SHIFT           7
#define PFS_SIZE_MASK            0x7F
#define NAT_BITS_PER_RNAT_REG    63
#define RNAT_ALIGNMENT           (NAT_BITS_PER_RNAT_REG << 3)

//
// Define Region Register (RR)
//
// RR bit field positions
//

#define RR_VE          0
#define RR_MBZ0        1
#define RR_PS          2
#define RR_PS_LEN      6
#define RR_RID         8
#define RR_RID_LEN     24
#define RR_MBZ1        32

//
// indirect mov index for loading RR
//

#define RR_INDEX       61
#define RR_INDEX_LEN   3

//
// Define low order 8 bit's of RR. All RR's have same PS and VE
//

#define RR_PS_VE  ( (PAGE_SHIFT<<RR_PS) | (1<<RR_VE) )

//
// Number of region registers used by NT
//

#define NT_RR_SIZE 4

//
// Total number of region registers
//

#define RR_SIZE 8

//
// Define Protection Key Register (PKR)
//
// PKR bit field positions
//

#define PKR_V          0
#define PKR_WD         1
#define PKR_RD         2
#define PKR_XD         3
#define PKR_MBZ0       4
#define PKR_KEY        8
#define PKR_KEY_LEN    24
#define PKR_MBZ1       32

//
// Define low order 8 bit's of PKR. All valid PKR's have same V, WD, RD, XD
//

#define PKR_VALID  (1<<PKR_V)

//
// Number of protection key registers
//

#define PKRNUM 16

//
// Define Interrupt TLB Insertion Register
//
// ITIR bit field positions
//

#define ITIR_RV0       0
#define ITIR_PS        2
#define ITIR_KEY       8
#define ITIR_RV1       32

//
// The following definitions are obsolete but
// there are codes in ke/ia64 reference them.
//
// Define Interruption Translation Register (IDTR/IITR)
//
// IDTR/IITR bit field positions
//

#define IDTR_MBZ0      0
#define IDTR_PS        2
#define IDTR_KEY       8
#define IDTR_MBZ1      32
#define IDTR_IGN0      48
#define IDTR_PPN       56
#define IDTR_MBZ2      63

#define IITR_MBZ0      IDTR_MBZ0
#define IITR_PS        IDTR_PS
#define IITR_KEY       IDTR_KEY
#define IITR_MBZ1      IDTR_MBZ1
#define IITR_IGN0      IDTR_IGN0
#define IITR_PPN       IDTR_PPN
#define IITR_MBZ2      IDTR_MBZ2

//
// ITIR  bit field masks

#define IITR_PPN_MASK              0x7FFF000000000000
#define IITR_ATTRIBUTE_PPN_MASK    0x0003FFFFFFFFF000

//
// Define Translation Insertion Format (TR)
//
// TR bit field positions
//

#define TR_P           0
#define TR_RV0         1
#define TR_MA          2
#define TR_A           5
#define TR_D           6
#define TR_PL          7
#define TR_AR          9
#define TR_PPN         13          // must be same as PAGE_SHIFT
#define TR_RV1         50
#define TR_ED          52
#define TR_IGN0        53

//
// Macros for generating TR value
//
#define TR_VALUE(ed, ppn, ar, pl, d, a, ma, p)        \
                ( ( ed << TR_ED )                  |  \
                  ( ppn & IITR_ATTRIBUTE_PPN_MASK) |  \
                  ( ar << TR_AR )                  |  \
                  ( pl << TR_PL )                  |  \
                  ( d << TR_D )                    |  \
                  ( a << TR_A )                    |  \
                  ( ma << TR_MA )                  |  \
                  ( p << TR_P )                       \
                )

#define ITIR_VALUE(key, ps)                                             \
                ( ( ps << ITIR_PS )                                  |  \
                  ( key << ITIR_KEY )                                   \
                )

//
// Page size definitions
//
#define PS_4K        0xC             // 0xC=12, 2^12=4K
#define PS_8K        0xD             // 0xD=13, 2^13=8K
#define PS_16K       0xE             // 0xE=14, 2^14=16K
#define PS_64K       0x10            // 0x10=16, 2^16=64K
#define PS_256K      0x12            // 0x12=18, 2^18=256K
#define PS_1M        0x14            // 0x14=20, 2^20=1M
#define PS_4M        0x16            // 0x16=22, 2^22=4M
#define PS_16M       0x18            // 0x18=24, 2^24=16M
#define PS_64M       0x1a            // 0x1a=26, 2^26=64M
#define PS_256M      0x1c            // 0x1a=26, 2^26=64M

//
// Debug Registers definitions
//

//
// At least, 4 data and 4 instruction register pairs are implemented
// on all processor models.
//
#define NUMBER_OF_DEBUG_REGISTER_PAIRS    4

//
// For the break conditions (mask):
//

#define DR_MASK      0         // Mask
#define DR_MASK_LEN  56        // Mask length
#define DR_PLM0      56        // Enable privlege level 0
#define DR_PLM1      57        // Enable privlege level 1
#define DR_PLM2      58        // Enable privlege level 2
#define DR_PLM3      59        // Enable privlege level 3 (user)
#define DR_IG        60        // Ignore
#define DR_RW        62        // Read/Write
#define DR_RW_LEN     2        // R/W length
#define DR_X         63        // Execute

//
// Performance Monitor Registers definitions
//

//
// At least, 4 data and 4 instruction register pairs are implemented
// on all processor models.
//
#define NUMBER_OF_PERFMON_REGISTER_PAIRS    4

//
// Macro to generate mask value from bit position
// N.B. If this macro is used in a C expression and the result is a
// 64-bit, the "value" argument shoud be cast as "unsigned long long" to
// produce a 64-bit mask.
//

#define MASK_IA64(bp,value)  (value << bp)

//
// Interrupt Vector Definitions
//

#define APC_VECTOR          APC_LEVEL << VECTOR_IRQL_SHIFT
#define DISPATCH_VECTOR     DISPATCH_LEVEL << VECTOR_IRQL_SHIFT

//
// Define interruption vector offsets
//

#define OFFSET_VECTOR_BREAK         0x2800  // Break instruction vector
#define OFFSET_VECTOR_EXT_INTERRUPT 0x2c00  // External interrupt vector
#define OFFSET_VECTOR_EXC_GENERAL   0x4400  // General exception vector

//
//
// Define IA64 page mask values.
//
#define PAGEMASK_4KB 0x0                // 4kb page
#define PAGEMASK_16KB 0x3               // 16kb page
#define PAGEMASK_64KB 0xf               // 64kb page
#define PAGEMASK_256KB 0x3f             // 256kb page
#define PAGEMASK_1MB 0xff               // 1mb page
#define PAGEMASK_4MB 0x3ff              // 4mb page
#define PAGEMASK_16MB 0xfff             // 16mb page

//
// Define IA64 primary cache states.
//
#define PRIMARY_CACHE_INVALID 0x0       // primary cache invalid
#define PRIMARY_CACHE_SHARED 0x1        // primary cache shared (clean or dirty)
#define PRIMARY_CACHE_CLEAN_EXCLUSIVE 0x2 // primary cache clean exclusive
#define PRIMARY_CACHE_DIRTY_EXCLUSIVE 0x3 // primary cache dirty exclusive

//
// Page table constants
//

#define PS_SHIFT         2
#define PS_LEN           6
#define PTE_VALID_MASK   1
#define PTE_ACCESS_MASK  0x20
#define PTE_NOCACHE      0x10
#define PTE_CACHE_SHIFT  2
#define PTE_CACHE_LEN    3
#define PTE_LARGE_PAGE   54
#define PTE_PFN_SHIFT    8
#define PTE_PFN_LEN      24
#define PTE_ATTR_SHIFT   1
#define PTE_ATTR_LEN     5
#define PTE_PS           55
#define PTE_OFFSET_LEN   10
#define PDE_OFFSET_LEN   10
#define VFN_LEN          19
#define VFN_LEN64        24
#define TB_USER_MASK     0x180
#define PTE_DIRTY_MASK   0x40
#define PTE_WRITE_MASK   0x400
#define PTE_EXECUTE_MASK 0x200
#define PTE_CACHE_MASK   0x0
#define PTE_EXC_DEFER 0x10000000000000

#define VALID_KERNEL_PTE (PTE_VALID_MASK|PTE_ACCESS_MASK|PTE_WRITE_MASK|PTE_CACHE_MASK|PTE_DIRTY_MASK)
#define VALID_KERNEL_EXECUTE_PTE (PTE_VALID_MASK|PTE_ACCESS_MASK|PTE_EXECUTE_MASK|PTE_WRITE_MASK|PTE_CACHE_MASK|PTE_DIRTY_MASK|PTE_EXC_DEFER)
#define PTE_VALID        0
#define PTE_ACCESS       5
#define PTE_OWNER        7
#define PTE_WRITE        10
#define PTE_LP_CACHE_SHIFT    53
#define ATE_INDIRECT     62
#define ATE_MASK         0xFFFFFFFFFFFFF9DE
#define ATE_MASK0        0x621
#define PAGE4K_SHIFT     12
#define ALT4KB_BASE 0x6FC00000000
#define ALT4KB_END  0x6FC00800000

#define VRN_SHIFT        61
#define KSEG3_VRN        4
#define KSEG4_VRN        5
#define MAX_PHYSICAL_SHIFT 44

//
// Translation register usage
//

//
// In NTLDR
//

//
// Boot loader CONFIGFLAG definitions.
//

#define DISABLE_TAR_FIX           0
#define DISABLE_BTB_FIX           1
#define DISABLE_DATA_BP_FIX       2
#define DISABLE_DET_STALL_FIX     3
#define ENABLE_FULL_DISPERSAL     4
#define ENABLE_TB_BROADCAST       5
#define DISABLE_CPL_FIX           6
#define ENABLE_POWER_MANAGEMENT   7
#define DISABLE_IA32BR_FIX        8
#define DISABLE_L1_BYPASS         9
#define DISABLE_VHPT_WALKER      10
#define DISABLE_IA32RSB_FIX      11
#define DISABLE_INTERRUPTION_LOG 13
#define DISABLE_UNSAFE_FILL      14
#define DISABLE_STORE_UPDATE     15
#define ENABLE_HISTORY_BUFFER    16

#define BL_4M                0x00400000
#define BL_16M               0x01000000
#define BL_20M               0x01400000
#define BL_24M               0x01800000
#define BL_28M               0x01C00000
#define BL_32M               0x02000000
#define BL_36M               0x02400000
#define BL_40M               0x02800000
#define BL_48M               0x03000000
#define BL_64M               0x04000000
#define BL_80M               0x05000000
#define BL_128M              0x08000000

#define TR_INFO_TABLE_SIZE   10

#define BL_SAL_INDEX         0
#define BL_KERNEL_INDEX      1
#define BL_DRIVER0_INDEX     2        // freed during the phase 0 initialization
#define BL_DRIVER1_INDEX     3        // freed during the phase 0 initialization
#define BL_DECOMPRESS_INDEX  4        // freed before entering kernel
#define BL_IO_PORT_INDEX     5        // freed before entering kernel
#define BL_PAL_INDEX         6
#define BL_LOADER_INDEX      7        // freed before entering kernel


//
// In NTOSKRNL
//

#define DTR_KIPCR_INDEX      0
#define DTR_KERNEL_INDEX     1

#define DTR_DRIVER0_INDEX    2        // freed during the phase 0 initialization
#define DTR_KTBASE_INDEX     2

#define DTR_DRIVER1_INDEX    3        // freed during the phase 0 initialization
#define DTR_UTBASE_INDEX     3
#define DTR_VIDEO_INDEX      3        // not used

#define DTR_KIPCR2_INDEX     4        // freed in the phase 0 initialization
#define DTR_STBASE_INDEX     4

#define DTR_IO_PORT_INDEX    5

#define DTR_KTBASE_INDEX_TMP 6        // freed during the phase 0 initialization
#define DTR_HAL_INDEX        6
#define DTR_PAL_INDEX        6

#define DTR_UTBASE_INDEX_TMP 7        // freed during the phase 0 initialization
#define DTR_LOADER_INDEX     7        // freed during the phase 0 initialization
#define DTR_UTBASE_INDEX_TMP 7        // freed during the phase 0 initialization

#define ITR_EPC_INDEX        0

#define ITR_KERNEL_INDEX     1

#define ITR_DRIVER0_INDEX    2        // freed during the phase 0 initialization

#define ITR_DRIVER1_INDEX    3        // freed during the phase 0 initialization

#define ITR_HAL_INDEX        4
#define ITR_PAL_INDEX        4

#define ITR_LOADER_INDEX     7        // freed during the phase 0 initialization


#define MEM_4K         0x1000
#define MEM_8K         0x2000
#define MEM_16K        0x4000
#define MEM_64K        0x10000
#define MEM_256K       0x40000
#define MEM_1M         0x100000
#define MEM_4M         0x400000
#define MEM_16M        0x1000000
#define MEM_64M        0x4000000
#define MEM_256M       0x10000000

//
// Macro for translation memory size in bytes to page size in TR format
//
#define MEM_SIZE_TO_PS(MemSize, TrPageSize)             \
                if (MemSize <= MEM_4K) {                \
                    TrPageSize = PS_4K;                 \
                } else if (MemSize <= MEM_8K)       {   \
                    TrPageSize = PS_8K;                 \
                } else if (MemSize <= MEM_16K)      {   \
                    TrPageSize = PS_16K;                \
                } else if (MemSize <= MEM_64K)      {   \
                    TrPageSize = PS_64K;                \
                } else if (MemSize <= MEM_256K)     {   \
                    TrPageSize = PS_256K;               \
                } else if (MemSize <= MEM_1M)       {   \
                    TrPageSize = PS_1M;                 \
                } else if (MemSize <= MEM_4M)       {   \
                    TrPageSize = PS_4M;                 \
                } else if (MemSize <= MEM_16M)      {   \
                    TrPageSize = PS_16M;                \
                } else if (MemSize <= MEM_64M)      {   \
                    TrPageSize = PS_64M;                \
                } else if (MemSize <= MEM_256M)     {   \
                    TrPageSize = PS_256M;               \
                }

//
// TLB forward progress queue
//

#define NUMBER_OF_FWP_ENTRIES 8

//
// Define the kernel base address
//

#define KERNEL_BASE  KADDRESS_BASE+0x80000000
#define KERNEL_BASE2 KADDRESS_BASE+0x81000000

//
// Initial value of data TR's for kernel/user PCR
//            ed  ign  res  ppn     ar   pl  d  a  ma   p
// kernel:    0   000  0000 0x0     010  00  1  1  000  1
// user:      0   000  0000 0x0     000  11  1  1  000  1
//

#define PDR_TR_INITIAL   TR_VALUE(0, 0, 2, 0, 1, 1, 0, 1)
#define KIPCR_TR_INITIAL TR_VALUE(0, 0, 2, 0, 1, 1, 0, 1)
#define USPCR_TR_INITIAL TR_VALUE(0, 0, 0, 3, 1, 1, 0, 1)

//
// Initial value of PTA (64-bits)
// base (region 0)    res   vf  size      res  ve
// 0x00000000000 0 0 00000  1   000000     0    1
//
#define PTA_INITIAL 0x001

//
// Initial value of DCR (64-bits)
// res              du dd da dr dx dk dp dm res   lc be pp
// 0x000000000000   1  1  1  1  1  1  1  1  00000 1  0  1
//
#define DCR_INITIAL 0x0000000000007e05

//
// Initial value of PSR low (32-bits)
// res  rt tb lp db si di pp sp dfh dfl dt rv pk i ic res       mfh mfl ac up be res
// 0000 1  0  0  0  0  1  1  0  1   0   1  0  0  0 1  0 0000 00 0   0   1  0  0  0
//
#define PSRL_INITIAL 0x086a2008

//
// Initial value of user PSR (64-bits)
// Bits 63-32
// res                     bn ed ri ss dd da id it mc is cpl
// 0000 0000 0000 0000 000 1   0 00 0  0  0  0  1  0  0  11
// Bits 31-0
// res  rt tb lp db si di pp sp dfh dfl dt rv pk i ic res        mfh mfl ac up be res
// 0000 1  0  0  0  0  0  1  0  1   0   1  0  0  1 1  0 0000 00  0   0   1  0  0  0
//
#define USER_PSR_INITIAL 0x00001013082a6008i64

//
// Initial value of user FPSR (64-bits)
//
// all FP exceptions masked
//
// rounding to nearest, 64-bit precisoin, wide range enabled for FPSR.fs1
//
// rounding to nearest, 53-bit precision, wide range disabled for FPSR.fs0
//

#define USER_FPSR_INITIAL 0x9804C0270033F

//
// Initial value of DCR (64-bits)
// res             dd da dr dx dk dp dm res   lc be pp
// 0x000000000000   1  1  1  1  1  1  1  00000 1  0  1
//
#define USER_DCR_INITIAL 0x0000000000007f05i64

//
// Initial value of user RSC (low 32-bits)
// Mode: Lazy. Little endian. User PL.
//
#define USER_RSC_INITIAL ((RSC_MODE_LY<<RSC_MODE) \
                          | (RSC_BE_LITTLE<<RSC_BE) \
                          | (0x3<<RSC_PL))

//
// Definitions that used by CSD and SSD
//

#define USER_CODE_DESCRIPTOR  0xCFBFFFFF00000000   
#define USER_DATA_DESCRIPTOR  0xCF3FFFFF00000000

//
//
// IA64 Software conventions
//

// Bytes in stack scratch area
#define STACK_SCRATCH_AREA  16

//
// Constants for trap
//

// Bits to shift for computing interrupt routine funtion pointer: fp = base + irql<<INT_ROUTINE_SHIFT

#ifdef _WIN64
#define INT_ROUTINES_SHIFT 3
#else
#define INT_ROUTINES_SHIFT 2
#endif

//
//
// Define disable and restore interrupt macros.
// Note: Serialization is implicit for rsm
//

#define DISABLE_INTERRUPTS(reg) \
         mov       reg = psr                    ;\
         rsm       1 << PSR_I

//
// Restore psr.i bit based on value of bit PSR_I in reg
// Enable does not do serialization, so interrupts may not be enabeld for
// a number of cycles after ssm.
//

#define RESTORE_INTERRUPTS(reg)  \
         tbit##.##nz   pt0,pt1 = reg, PSR_I;;   ;\
(pt0)    ssm       1 << PSR_I                   ;\
(pt1)    rsm       1 << PSR_I

//
// The FAST versions can be used when it is not necessary to save/restore
// the previous interrupt enable state.
//

#define FAST_DISABLE_INTERRUPTS \
         rsm       1 << PSR_I

//
// FAST ENABLE does not do serialization -- we don't care if interrupt
// enable is not visible for few instructions.
//

#define FAST_ENABLE_INTERRUPTS \
         ssm       1 << PSR_I

#define YIELD   hint##.##m 0

//
//
// Define TB and cache parameters.
//
#define PCR_ENTRY 0                     // TB entry numbers (2) for the PCR
#define PDR_ENTRY 2                     // TB entry number (1) for the PDR
#define LARGE_ENTRY 3                   // TB entry number (1) for large entry
#define DMA_ENTRY 4                     // TB entry number (1) for DMA/InterruptSource

#define TB_ENTRY_SIZE (3 * 4)           // size of TB entry
#define FIXED_BASE 0                    // base index of fixed TB entries
#define FIXED_ENTRIES (DMA_ENTRY + 1)   // number of fixed TB entries

//
// Define cache parameters
//

#define DCACHE_SIZE 4 * 1024            // size of data cache in bytes
#define ICACHE_SIZE 4 * 1024            // size of instruction cache in bytes
#define MINIMUM_CACHE_SIZE 4 * 1024     // minimum size of cache
#define MAXIMUM_CACHE_SIZE 128 * 1024   // maximum size fo cache

//
// RID and Sequence number limits.
// Start with 1 because 0 means not initialized.
// RID's are 24 bits.
//

#define KSEG3_RID          0x00000
#define START_GLOBAL_RID   0x00001
#define HAL_RID            0x00002
#define START_SESSION_RID  0x00003
#define START_PROCESS_RID  0x00004

//
// making the maximum RID to 18-bit, temp fix for Merced
//

#define MAXIMUM_RID        0x3FFFF

//
// Sequence numbers are 32 bits
// Start with 1 because 0 means not initialized.
//

#define START_SEQUENCE     1
#define MAXIMUM_SEQUENCE   0xFFFFFFFFFFFFFFFF

//
//
// Define subtitle macro
//

#define SBTTL(x)

//
// Define procedure entry macros
//

#define PROLOGUE_BEGIN     .##prologue;
#define PROLOGUE_END       .##body;

#define ALTERNATE_ENTRY(Name)                    \
         .##global Name;                         \
         .##type   Name, @function;              \
Name::

#define CPUBLIC_LEAF_ENTRY(Name,i)               \
         .##text;                                \
         .##proc   Name##@##i;                   \
Name##@##i::

#define LEAF_ENTRY(Name)                         \
         .##text;                                \
         .##global Name;                         \
         .##proc   Name;                         \
Name::

#define LEAF_SETUP(i,l,o,r)                      \
         .##regstk i,l,o,r;                      \
         alloc     r31=ar##.##pfs,i,l,o,r

#define CPUBLIC_NESTED_ENTRY(Name,i)             \
         .##text;                                \
         .##proc   Name##@##i;                   \
         .##unwentry;                            \
Name##@##i::

#define NESTED_ENTRY_EX(Name, Handler)           \
         .##text;                                \
         .##global Name;                         \
         .##proc   Name;                         \
         .##personality Handler;                 \
Name::

#define NESTED_ENTRY(Name)                       \
         .##text;                                \
         .##global Name;                         \
         .##proc   Name;                         \
Name::

// Note: use of NESTED_SETUP requires number of locals (l) >= 2

#define NESTED_SETUP(i,l,o,r)                    \
         .##regstk i,l,o,r;                      \
         .##prologue 0xC, loc0;                  \
         alloc     savedpfs=ar##.##pfs,i,l,o,r  ;\
         mov       savedbrp=brp;

//
// Define procedure exit macros
//

#define LEAF_RETURN                              \
         br##.##ret##.##sptk##.##few##.##clr brp

#define NESTED_RETURN                            \
         mov       ar##.##pfs = savedpfs;        \
         mov       brp = savedbrp;               \
         br##.##ret##.##sptk##.##few##.##clr brp

#define LEAF_EXIT(Name)                          \
         .##endp Name;

#define NESTED_EXIT(Name)                        \
         .##endp Name;


//++
// Routine:
//
//       LDPTR(rD, rPtr)
//
// Routine Description:
//
//       load pointer value.
//
// Agruments:
//
//       rD:     destination register
//       rPtr: register containing pointer
//
// Return Value:
//
//       rD = load from address [rPtr]
//
// Notes:
//
//       rPtr is unchanged
//
//--

#ifdef  _WIN64
#define LDPTR(rD, rPtr) \
       ld8        rD = [rPtr]
#else
#define LDPTR(rD, rPtr) \
       ld4        rD = [rPtr] ;\
       ;;                     ;\
       sxt4       rD = rD
#endif

//++
// Routine:
//
//       LDPTRINC(rD, rPtr, imm)
//
// Routine Description:
//
//       load pointer value and update base.
//
// Agruments:
//
//       rD:     destination register
//       rPtr: register containing pointer
//       imm:  number to be incremented to base
//
// Return Value:
//
//       rD = load from address [rPtr] and rPtr += imm
//
// Notes:
//
//--

#ifdef  _WIN64
#define LDPTRINC(rD, rPtr, imm) \
       ld8        rD = [rPtr], imm
#else
#define LDPTRINC(rD, rPtr, imm) \
       ld4        rD = [rPtr], imm ;\
       ;;                     ;\
       sxt4       rD = rD
#endif

//++
// Routine:
//
//       PLDPTRINC(rP, rD, rPtr, imm)
//
// Routine Description:
//
//       predicated load pointer value and update base.
//
// Agruments:
//
//       rP:   predicate register
//       rD:   destination register
//       rPtr: register containing pointer
//       imm:  number to be incremented to base
//
// Return Value:
//
//       if (rP) rD = load from address [rPtr] and rPtr += imm
//
// Notes:
//
//--

#ifdef  _WIN64
#define PLDPTRINC(rP, rD, rPtr, imm) \
(rP)   ld8        rD = [rPtr], imm
#else
#define PLDPTRINC(rP, rD, rPtr, imm) \
(rP)   ld4        rD = [rPtr], imm ;\
       ;;                     ;\
(rP)   sxt4       rD = rD
#endif

//++
// Routine:
//
//       PLDPTR(rP, rD, rPtr)
//
// Routine Description:
//
//       predicated load pointer value.
//
// Agruments:
//
//       rP:   predicate register
//       rD:   destination register
//       rPtr: register containing pointer
//
// Return Value:
//
//       if (rP == 1)  rD = load from address [rPtr], else NO-OP
//
// Notes:
//
//       rPtr is unchanged
//
//--

#ifdef  _WIN64
#define PLDPTR(rP, rD, rPtr) \
(rP)    ld8       rD = [rPtr]
#else
#define PLDPTR(rP, rD, rPtr) \
(rP)    ld4       rD = [rPtr] ;\
       ;;                     ;\
(rP)    sxt4      rD = rD
#endif

//++
// Routine:
//
//       STPTR(rPtr, rS)
//
// Routine Description:
//
//       store pointer value.
//
// Agruments:
//
//       rPtr: register containing pointer
//       rS  : source pointer value
//
// Return Value:
//
//       store [rPtr] = rS
//
// Notes:
//
//       rPtr is unchanged
//
//--

#ifdef  _WIN64
#define STPTR(rPtr, rS) \
       st8        [rPtr] = rS
#else
#define STPTR(rPtr, rS) \
       st4        [rPtr] = rS
#endif

//++
// Routine:
//
//       PSTPTR(rP, rPtr, rS)
//
// Routine Description:
//
//       predicated store pointer value.
//
// Agruments:
//
//       rP:   predicate register
//       rPtr: register containing pointer
//       rS  : source pointer value
//
// Return Value:
//
//       if (rP)  store [rPtr] = rS
//
// Notes:
//
//       rPtr is unchanged
//
//--

#ifdef  _WIN64
#define PSTPTR(rP, rPtr, rS) \
(rP)   st8        [rPtr] = rS
#else
#define PSTPTR(rP, rPtr, rS) \
(rP)   st4        [rPtr] = rS
#endif

//++
// Routine:
//
//       STPTRINC(rPtr, rS, imm)
//
// Routine Description:
//
//       store pointer value.
//
// Agruments:
//
//       rPtr: register containing pointer
//       rS  : source pointer value
//       imm:  number to be incremented to base
//
// Return Value:
//
//       if (rP) store [rPtr] = rS, rPtr += imm
//
// Notes:
//
//--

#ifdef  _WIN64
#define STPTRINC(rPtr, rS, imm) \
       st8        [rPtr] = rS, imm
#else
#define STPTRINC(rPtr, rS, imm) \
       st4        [rPtr] = rS, imm
#endif

//++
// Routine:
//
//       ARGPTR(rPtr)
//
// Routine Description:
//
//       sign extend the pointer argument for WIN32
//
// Agruments:
//
//       rPtr: register containing pointer argument
//
// Return Value:
//
//
// Notes:
//
//
//
//--

#ifdef  _WIN64
#define ARGPTR(rPtr)
#else
#define ARGPTR(rPtr) \
        sxt4      rPtr = rPtr
#endif


//
// Assembler spinlock macros
//


//++
// Routine:
//
//       ACQUIRE_SPINLOCK(rpLock, rOwn, Loop)
//
// Routine Description:
//
//       Acquire a spinlock. Waits for lock to become free
//       by spinning on the cached lock value.
//
// Agruments:
//
//       rpLock: pointer to the spinlock (64-bit)
//       rOwn:   value to store in lock to indicate owner
//               Depending on call location, it could be:
//                  - rpLock
//                  - pointer to process
//                  - pointer to thread
//                  - pointer to PRCB
//       Loop:   unique name for loop label
//
// Return Value:
//
//       None
//
// Notes:
//
//       Uses temporaries: predicates pt0, pt1, pt2, and GR t22.
//       The first time through the loop pt0 is zero, so we do
//       not need to initialize pt2.
//--

#define ACQUIRE_SPINLOCK(rpLock, rOwn, Loop)                                   \
         cmp##.##eq    pt0, pt1 = zero, zero                                  ;\
         ;;                                                                   ;\
Loop:                                                                         ;\
.pred.rel "mutex",pt0,pt1                                                     ;\
(pt1)    YIELD                                                                ;\
(pt0)    xchg8         t22 = [rpLock], rOwn                                   ;\
(pt1)    ld8##.##nt1   t22 = [rpLock]                                         ;\
         ;;                                                                   ;\
(pt0)    cmp##.##ne    pt2 = zero, t22                                        ;\
         cmp##.##eq    pt0, pt1 = zero, t22                                   ;\
(pt2)    br##.##dpnt   Loop

//++
// Routine:
//
//       RELEASE_SPINLOCK(rpLock)
//
// Routine Description:
//
//       Release a spinlock by setting lock to zero.
//
// Agruments:
//
//       rpLock: pointer to the spinlock.
//
// Return Value:
//
//       None
//
// Notes:
//
//       Uses an ordered store to ensure previous memory accesses in
//       critical section complete.
//--

#define RELEASE_SPINLOCK(rpLock)                                               \
         st8##.##rel           [rpLock] = zero

//++
// Routine:
//
//       PRELEASE_SPINLOCK(rpLock)
//
// Routine Description:
//
//       Predicated release spinlock.
//
// Agruments:
//
//       rpLock: pointer (swizzled) to the spinlock.
//               spinlock itself is 32-bit.
//
// Return Value:
//
//       None
//
// Notes:
//
//       Uses an ordered store to ensure previous memory accesses in
//       critical section complete.
//--

#define PRELEASE_SPINLOCK(px, rpLock)                                          \
(px)    st8##.##rel   [rpLock] = zero

//
// Interrupt and IRQL macros
//

//++
// Routine:
//
//       END_OF_INTERRUPT
//
// Routine Description:
//
//       Hook to perform end-of-interrupt processing. Currently
//       just writes to the EOI control register.
//
// Agruments:
//
//       None
//
// Return Value:
//
//       None
//
// Note:
//
//       Writing EOI requires explicit data serialize. srlz must be preceded by
//       stop bit.
//--

#define END_OF_INTERRUPT                         \
         mov       cr##.##eoi = zero            ;\
         ;;                                     ;\
         srlz##.##d    /* Requires data srlz */

//++
// Routine:
//
//       GET_IRQL(rOldIrql)
//
// Routine Description:
//
//       Read the current IRQL by reading the TPR control register.
//
// Agruments:
//
//       Register to contain the result.
//
// Return Value:
//
//       rOldIrql: the current value of the IRQL.
//
//--

#ifndef SHADOW_IRQL_IMPLEMENTATION

#define GET_IRQL(rOldIrql)                       \
        mov         rOldIrql = cr##.##tpr      ;;\
        extr##.##u  rOldIrql = rOldIrql, TPR_MIC, TPR_MIC_LEN

#else

#define GET_IRQL(rOldIrql)                              \
        movl        rOldIrql = KiPcr+PcCurrentIrql;;    \
        ld1         rOldIrql = [rOldIrql]

#endif

//++
// Routine:
//
//       SET_IRQL(rNewIrql)
//
// Routine Description:
//
//       Update the IRQL by writing the TPR control register.
//       register t21 & t22 are used as a scratch
//
// Agruments:
//
//       Register with the new IRQL value. Contains the unshifted
//       IRQL value (0-15).
//
// Return Value:
//
//       None.
//
// Notes:
//
//       Writing TPR requires explicit data serialize. srlz must be preceded by
//       stop bit.
//
//--

#ifndef SHADOW_IRQL_IMPLEMENTATION

#define SET_IRQL(rNewIrql)                                    \
        dep##.##z t22 = rNewIrql, TPR_MIC, TPR_MIC_LEN;;     ;\
        mov       cr##.##tpr = t22;;                         ;\
        srlz##.##d
#else

#define SET_IRQL(rNewIrql)                                  \
        dep##.##z t22 = rNewIrql, TPR_MIC, TPR_MIC_LEN;;   ;\
        movl      t21 = KiPcr+PcCurrentIrql;;              ;\
        mov       cr##.##tpr = t22                         ;\
        st1       [t21] = rNewIrql

#endif

//++
// Routine:
//
//       PSET_IRQL(pr, rNewIrql)
//
// Routine Description:
//
//       Update the IRQL by writing the TPR control register, predicated
//       on pr
//       register t21 & t22 are used as a scratch
//
// Agruments:
//
//       pr:       predicate -- set irql if pr true.
//
//       rNewIrql: Register with the new IRQL value. Contains the unshifted
//                 IRQL value (0-15).
//
// Return Value:
//
//       None.
//
// Notes:
//
//       Relies on TPR.mi always 0.
//       Writing TPR requires explicit data serialize. srlz must be preceded by
//       stop bit.
//--

#ifndef SHADOW_IRQL_IMPLEMENTATION

#define PSET_IRQL(pr, rNewIrql)                             \
        dep##.##z t22 = rNewIrql, TPR_MIC, TPR_MIC_LEN;;   ;\
(pr)    mov       cr##.##tpr = t22;;                       ;\
(pr)    srlz##.##d

#else

#define PSET_IRQL(pr, rNewIrql)                             \
        mov       t21 = rNewIrql                           ;\
        dep##.##z t22 = rNewIrql, TPR_MIC, TPR_MIC_LEN;;   ;\
(pr)    mov       cr##.##tpr = t22                         ;\
(pr)    movl      t22 = KiPcr+PcCurrentIrql;;              ;\
(pr)    st1       [t22] = t21

#endif

//++
// Routine:
//
//       SWAP_IRQL(rNewIrql)
//
// Routine Description:
//
//       get the current IRQL value and set the IRQL to the new value
//       register t21 and t22 are used as a scratch
//
// Agruments:
//
//       Register with the new IRQL value. Contains the unshifted
//       IRQL value (0-15).
//
// Return Value:
//
//       v0 - current IRQL
//
//--

#define SWAP_IRQL(rNewIrql)                                 \
        movl      t22 = KiPcr+PcCurrentIrql;;              ;\
        ld1       v0 = [t22]                               ;\
        dep##.##z t21 = rNewIrql, TPR_MIC, TPR_MIC_LEN;;   ;\
        mov       cr##.##tpr = t21                         ;\
        st1       [t22] = rNewIrql

//++
// Routine:
//
//       GET_IRQL_FOR_VECTOR(pGet,rIrql,rVector)
//
// Routine Description:
//
//       Hook to get the IRQL associated with an interrupt vector.
//       Currently just returns bit {7:4} of the 8-bit vector number.
//
// Agruments:
//
//       pGet:    Predicate: if true then get, else skip.
//       rIrql:   Register to contain the associated IRQL.
//       rVector: Register containing the vector number.
//
// Return Value:
//
//       rIrql: The IRQL value. A 4-bit value in bits {3:0}. All
//              other bits are zero.
//
//--

#define GET_IRQL_FOR_VECTOR(pGet,rIrql,rVector)                      \
(pGet)  shr         rIrql = rVector, VECTOR_IRQL_SHIFT



//++
// Routine:
//
//       GET_VECTOR_FOR_IRQL(pGet,rVector,rIrql)
//
// Routine Description:
//
//       Hook to get the interrupt vector associated with an IRQL.
//       Currently just returns IRQL << 4.
//
// Agruments:
//
//       pGet:    Predicate: if true then get, else skip.
//       rVector: Register containing the associated vector number.
//       rIrql:   Register to containing the IRQL.
//
// Return Value:
//
//       rVector: The vector value. An 8-bit value in bits {7:0}. All
//              other bits are zero.
//
//--

#define GET_VECTOR_FOR_IRQL(pGet, rVector, rIrql)                     \
(pGet)  shl         rVector = rIrql, VECTOR_IRQL_SHIFT

// Routine:
//
//       REQUEST_APC_INT(pReq)
//       REQUEST_DISPATCH_INT(pReq)
//
// Routine Description:
//
//       Request a software interrupt. Used to request
//       APC and DPC interrupts.
//
// Agruments:
//
//       pReq: Predicate: if true then do request, else skip
//
// Return Value:
//
//       None
//
// Notes:
//
//       Uses temporary registers t20, t21
//--

#define REQUEST_APC_INT(pReq)                                                  \
        mov         t20 = 1                                                   ;\
        movl        t21 = KiPcr+PcApcInterrupt                                ;\
        ;;                                                                    ;\
(pReq)  st1         [t21] = t20

#define REQUEST_DISPATCH_INT(pReq)                                             \
        mov         t20 = 1                                                   ;\
        movl        t21 = KiPcr+PcDispatchInterrupt                           ;\
        ;;                                                                    ;\
(pReq)  st1         [t21] = t20


#ifdef __assembler
//++
// Routine:
//
//       LOWER_IRQL(rNewIrql, trapFrame)
//
// Routine Description:
//
//       Check for pending s/w interrupts and lower Irql
//
// Agruments:
//
//       rNewIrql: interrupt request level
//
//       trapFrame: trap frame at the time of the interruption
//
// Return Value:
//
//       None
//
// Notes:
//
//       Pending s/w interrupts are dispatched if new IRQL is low enough,
//       even though interrupts are disabled
//--

#define LOWER_IRQL(rNewIrql, trapFrame)                                        \
        cmp##.##gtu pt0, pt1 = DISPATCH_LEVEL, rNewIrql                       ;\
        movl        t22 = KiPcr+PcSoftwareInterruptPending;;                  ;\
        ld2         t22 = [t22]                                               ;\
        mov         out0 = rNewIrql                                           ;\
        mov         out1 = trapFrame;;                                        ;\
(pt0)   cmp##.##ne  pt0, pt1 = r0, t22                                        ;\
        PSET_IRQL(pt1, rNewIrql)                                              ;\
(pt0)   br##.##call##.##spnt brp = KiCheckForSoftwareInterrupt

//++
//
// Routine:
//
//       LEAF_LOWER_IRQL_AND_RETURN(rNewIrql)
//
// Routine Description:
//
//       Check for pending s/w interrupts and lower Irql
//
//       If a software interupt is in fact pending and would
//       logically fire if IRQL is lowered to the new level,
//       branch to code that will promote to a nested function
//       and handle the interrupt, otherwise, lower IRQL and
//       return from this leaf function.
//
// Agruments:
//
//       rNewIrql: interrupt request level
//
// Return Value:
//
//       None
//
// Notes:
//
//       Pending s/w interrupts are dispatched if new IRQL is low enough,
//       even though interrupts are disabled
//--

#define LEAF_LOWER_IRQL_AND_RETURN(rNewIrql)                                   \
        cmp##.##gtu pt0 = DISPATCH_LEVEL, rNewIrql                            ;\
        movl        t21 = KiPcr+PcSoftwareInterruptPending;;                  ;\
(pt0)   rsm         1 << PSR_I                                                ;\
(pt0)   ld2         t21 = [t21]                                               ;\
        mov         t22 = rNewIrql;;                                          ;\
(pt0)   cmp##.##ltu##.##unc pt1 = rNewIrql, t21                               ;\
(pt1)   br##.##spnt KiLowerIrqlSoftwareInterruptPending                       ;\
        SET_IRQL(rNewIrql)                                                    ;\
        ssm         1 << PSR_I                                                ;\
        br##.##ret##.##spnt##.##few##.##clr brp

#endif // __assembler

//*******
//*
//* The following macros are used in C runtime asm code
//*
//* beginSection - a macro for declaring and beginning a section
//*         .sdata is used to create short data section, if it does not exist
//*
//* endSection - a macro for ending a previously declared section
//*
//*******

#define beginSection(SectName)  .##section   .CRT$##SectName, "a", "progbits"

#define endSection(SectName)

#define PublicFunction(Name) .##global Name; .##type Name,@function

//  XIA  Begin C Initializer Sections
//  XIC   Microsoft Reserved
//  XIU   User
//  XIZ  End C Initializer Sections
//
//  XCA  Begin C++ Constructor Sections
//  XCC   Compiler (MS)
//  XCL   Library
//  XCU   User
//  XCZ  End C++ Constructor Sections
//
//  XPA  Begin C Pre-Terminator Sections
//  XPU   User
//  XPX   Microsoft Reserved
//  XPZ  End C Pre-Terminator Sections
//
//  XTA  Begin C Pre-Terminator Sections
//  XTU   User
//  XTX   Microsoft Reserved
//  XTZ  End C Pre-Terminator Sections
//

