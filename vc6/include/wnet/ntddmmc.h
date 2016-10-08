
#ifndef __NTDDMMC__
#define __NTDDMMC__

#if _MSC_VER >= 1200
#pragma warning(push)
#endif
#pragma warning(disable:4200) // array[0] is not a warning for this file


//
// NOTE: All FEATURE_* structures may be extended.  use of these structures
//       requires verification that the FeatureHeader->AdditionLength field
//       contains AT LEAST enough data to cover the data fields being accessed.
//       This is due to the design, which allows extending the size of the
//       various structures, which will result in these structures sizes
//       being changed over time.
//       *** Programmers beware! ***
//

//
// NOTE: This is based on MMC 3
//       Further revisions will maintain backward compatibility
//       with the non-reserved fields listed here.  If you need
//       to access a new field, please typecast to FEATURE_DATA_RESERVED
//       and access the appropriate bits there.
//

typedef struct _GET_CONFIGURATION_HEADER {
    UCHAR DataLength[4];      // [0] == MSB, [3] == LSB
    UCHAR Reserved[2];
    UCHAR CurrentProfile[2];  // [0] == MSB, [1] == LSB
    UCHAR Data[0];            // extra data, typically FEATURE_HEADER
} GET_CONFIGURATION_HEADER, *PGET_CONFIGURATION_HEADER;

typedef struct _FEATURE_HEADER {
    UCHAR FeatureCode[2];     // [0] == MSB, [1] == LSB
    UCHAR Current    : 1;     // The feature is currently active
    UCHAR Persistent : 1;     // The feature is always current
    UCHAR Version    : 4;
    UCHAR Reserved0  : 2;
    UCHAR AdditionalLength;   // sizeof(Header) + AdditionalLength = size
} FEATURE_HEADER, *PFEATURE_HEADER;

typedef enum _FEATURE_PROFILE_TYPE {
    ProfileInvalid          = 0x0000,
    ProfileNonRemovableDisk = 0x0001,
    ProfileRemovableDisk    = 0x0002,
    ProfileMOErasable       = 0x0003,
    ProfileMOWriteOnce      = 0x0004,
    ProfileAS_MO            = 0x0005,
    // Reserved           0x0006 - 0x0007
    ProfileCdrom            = 0x0008,
    ProfileCdRecordable     = 0x0009,
    ProfileCdRewritable     = 0x000a,
    // Reserved           0x000b - 0x000f
    ProfileDvdRom           = 0x0010,
    ProfileDvdRecordable    = 0x0011,
    ProfileDvdRam           = 0x0012,
    ProfileDvdRewritable    = 0x0013,  // restricted overwrite
    ProfileDvdRWSequential  = 0x0014,
    // Reserved           0x0014 - 0x0019
    ProfileDvdPlusRW        = 0x001A,
    // Reserved           0x001B - 001F
    ProfileDDCdrom          = 0x0020,
    ProfileDDCdRecordable   = 0x0021,
    ProfileDDCdRewritable   = 0x0022,
    // Reserved           0x0023 - 0xfffe
    ProfileNonStandard      = 0xffff
} FEATURE_PROFILE_TYPE, *PFEATURE_PROFILE_TYPE;

typedef enum _FEATURE_NUMBER {
    FeatureProfileList             = 0x0000,
    FeatureCore                    = 0x0001,
    FeatureMorphing                = 0x0002,
    FeatureRemovableMedium         = 0x0003,
    FeatureWriteProtect            = 0x0004,
    // Reserved                  0x0005 - 0x000f
    FeatureRandomReadable          = 0x0010,
    // Reserved                  0x0011 - 0x001c
    FeatureMultiRead               = 0x001d,
    FeatureCdRead                  = 0x001e,
    FeatureDvdRead                 = 0x001f,
    FeatureRandomWritable          = 0x0020,
    FeatureIncrementalStreamingWritable = 0x0021,
    FeatureSectorErasable          = 0x0022,
    FeatureFormattable             = 0x0023,
    FeatureDefectManagement        = 0x0024,
    FeatureWriteOnce               = 0x0025,
    FeatureRestrictedOverwrite     = 0x0026,
    FeatureCdrwCAVWrite            = 0x0027,
    FeatureMrw                     = 0x0028,
    // Reserved                  0x0029
    FeatureDvdPlusRW               = 0x002A,
    // Reserved                  0x002b
    FeatureRigidRestrictedOverwrite = 0x002c,
    FeatureCdTrackAtOnce           = 0x002d,
    FeatureCdMastering             = 0x002e,
    FeatureDvdRecordableWrite      = 0x002f,   // both -R and -RW
    FeatureDDCDRead                = 0x0030,
    FeatureDDCDRWrite              = 0x0031,
    FeatureDDCDRWWrite             = 0x0032,
    // Reserved                  0x0033 - 0x00ff
    FeaturePowerManagement         = 0x0100,
    FeatureSMART                   = 0x0101,
    FeatureEmbeddedChanger         = 0x0102,
    FeatureCDAudioAnalogPlay       = 0x0103,
    FeatureMicrocodeUpgrade        = 0x0104,
    FeatureTimeout                 = 0x0105,
    FeatureDvdCSS                  = 0x0106,
    FeatureRealTimeStreaming       = 0x0107,
    FeatureLogicalUnitSerialNumber = 0x0108,
    // Reserved                      0x0109
    FeatureDiscControlBlocks       = 0x010a,
    FeatureDvdCPRM                 = 0x010b
    // Reserved                  0x010c - 0xfeff
    // Vendor Unique             0xff00 - 0xffff
} FEATURE_NUMBER, *PFEATURE_NUMBER;

// 0x0000 - FeatureProfileList
// an integral multiple of the _EX structures are returned for page 0000
typedef struct _FEATURE_DATA_PROFILE_LIST_EX {
    UCHAR ProfileNumber[2]; // [0] == MSB, [1] == LSB
    UCHAR Current                   : 1;
    UCHAR Reserved1                 : 7;
    UCHAR Reserved2;
} FEATURE_DATA_PROFILE_LIST_EX, *PFEATURE_DATA_PROFILE_LIST_EX;

typedef struct _FEATURE_DATA_PROFILE_LIST {
    FEATURE_HEADER Header;
    FEATURE_DATA_PROFILE_LIST_EX Profiles[0];
} FEATURE_DATA_PROFILE_LIST, *PFEATURE_DATA_PROFILE_LIST;

// 0x0001 - FeatureCore
typedef struct _FEATURE_DATA_CORE {
    FEATURE_HEADER Header;
    UCHAR PhysicalInterface[4];  // [0] == MSB, [3] == LSB
} FEATURE_DATA_CORE, *PFEATURE_DATA_CORE;

// 0x0002 - FeatureMorphing
typedef struct _FEATURE_DATA_MORPHING {
    FEATURE_HEADER Header;
    UCHAR Asynchronous              : 1;
    UCHAR Reserved1                 : 7;
    UCHAR Reserved2[3];
} FEATURE_DATA_MORPHING, *PFEATURE_DATA_MORPHING;

// 0x0003 - FeatureRemovableMedium
typedef struct _FEATURE_DATA_REMOVABLE_MEDIUM {
    FEATURE_HEADER Header;
    UCHAR Lockable                  : 1;
    UCHAR Reserved1                 : 1;
    UCHAR DefaultToPrevent          : 1;
    UCHAR Eject                     : 1;
    UCHAR Reserved2                 : 1;
    UCHAR LoadingMechanism          : 3;
    UCHAR Reserved3[3];
} FEATURE_DATA_REMOVABLE_MEDIUM, *PFEATURE_DATA_REMOVABLE_MEDIUM;

// 0x0004 - FeatureWriteProtect
typedef struct _FEATURE_DATA_WRITE_PROTECT {
    FEATURE_HEADER Header;
    UCHAR SupportsSWPPBit                : 1;
    UCHAR SupportsPersistentWriteProtect : 1;
    UCHAR Reserved1                      : 6;
    UCHAR Reserved2[3];
} FEATURE_DATA_WRITE_PROTECT, *PFEATURE_DATA_WRITE_PROTECT;

// 0x0005 - 0x000f are Reserved

// 0x0010 - FeatureRandomReadable
typedef struct _FEATURE_DATA_RANDOM_READABLE {
    FEATURE_HEADER Header;
    UCHAR LogicalBlockSize[4];
    UCHAR Blocking[2];
    UCHAR ErrorRecoveryPagePresent : 1;
    UCHAR Reserved1                : 7;
    UCHAR Reserved2;
} FEATURE_DATA_RANDOM_READABLE, *PFEATURE_DATA_RANDOM_READABLE;

// 0x0011 - 0x001c are Reserved

// 0x001d - FeatureMultiRead
typedef struct _FEATURE_DATA_MULTI_READ {
    FEATURE_HEADER Header;
} FEATURE_DATA_MULTI_READ, *PFEATURE_DATA_MULTI_READ;

// 0x001e - FeatureCdRead
typedef struct _FEATURE_DATA_CD_READ {
    FEATURE_HEADER Header;
    UCHAR CDText                   : 1;
    UCHAR C2ErrorData              : 1;
    UCHAR Reserved1                : 6;
    UCHAR Reserved2[3];
} FEATURE_DATA_CD_READ, *PFEATURE_DATA_CD_READ;

// 0x001f - FeatureDvdRead
typedef struct _FEATURE_DATA_DVD_READ {
    FEATURE_HEADER Header;
} FEATURE_DATA_DVD_READ, *PFEATURE_DATA_DVD_READ;

// 0x0020 - FeatureRandomWritable
typedef struct _FEATURE_DATA_RANDOM_WRITABLE {
    FEATURE_HEADER Header;
    UCHAR LastLBA[4];
    UCHAR LogicalBlockSize[4];
    UCHAR Blocking[2];
    UCHAR ErrorRecoveryPagePresent : 1;
    UCHAR Reserved1                : 7;
    UCHAR Reserved2;
} FEATURE_DATA_RANDOM_WRITABLE, *PFEATURE_DATA_RANDOM_WRITABLE;

// 0x0021 - FeatureIncrementalStreamingWritable
typedef struct _FEATURE_DATA_INCREMENTAL_STREAMING_WRITABLE {
    FEATURE_HEADER Header;
    UCHAR DataTypeSupported[2];   // [0] == MSB, [1] == LSB // see also FeatureCdTrackAtOnce
    UCHAR BufferUnderrunFree : 1;
    UCHAR Reserved1          : 7;
    UCHAR NumberOfLinkSizes;
    UCHAR LinkSize[0];
} FEATURE_DATA_INCREMENTAL_STREAMING_WRITABLE, *PFEATURE_DATA_INCREMENTAL_STREAMING_WRITABLE;

// 0x0022 - FeatureSectorErasable
typedef struct _FEATURE_DATA_SECTOR_ERASABLE {
    FEATURE_HEADER Header;
} FEATURE_DATA_SECTOR_ERASABLE, *PFEATURE_DATA_SECTOR_ERASABLE;

// 0x0023 - FeatureFormattable
typedef struct _FEATURE_DATA_FORMATTABLE {
    FEATURE_HEADER Header;
} FEATURE_DATA_FORMATTABLE, *PFEATURE_DATA_FORMATTABLE;

// 0x0024 - FeatureDefectManagement
typedef struct _FEATURE_DATA_DEFECT_MANAGEMENT {
    FEATURE_HEADER Header;
    UCHAR Reserved1             : 7;
    UCHAR SupplimentalSpareArea : 1;
    UCHAR Reserved2[3];
} FEATURE_DATA_DEFECT_MANAGEMENT, *PFEATURE_DATA_DEFECT_MANAGEMENT;

// 0x0025 - FeatureWriteOnce
typedef struct _FEATURE_DATA_WRITE_ONCE {
    FEATURE_HEADER Header;
    UCHAR LogicalBlockSize[4];
    UCHAR Blocking[2];
    UCHAR ErrorRecoveryPagePresent : 1;
    UCHAR Reserved1                : 7;
    UCHAR Reserved2;
} FEATURE_DATA_WRITE_ONCE, *PFEATURE_DATA_WRITE_ONCE;

// 0x0026 - FeatureRestrictedOverwrite
typedef struct _FEATURE_DATA_RESTRICTED_OVERWRITE {
    FEATURE_HEADER Header;
} FEATURE_DATA_RESTRICTED_OVERWRITE, *PFEATURE_DATA_RESTRICTED_OVERWRITE;

// 0x0027 - FeatureCdrwCAVWrite
typedef struct _FEATURE_DATA_CDRW_CAV_WRITE {
    FEATURE_HEADER Header;
    UCHAR Reserved1[4];
} FEATURE_DATA_CDRW_CAV_WRITE, *PFEATURE_DATA_CDRW_CAV_WRITE;

// 0x0028 - FeatureMrw
typedef struct _FEATURE_DATA_MRW {
    FEATURE_HEADER Header;
    UCHAR Write     : 1;
    UCHAR Reserved1 : 7;
    UCHAR Reserved2[3];
} FEATURE_DATA_MRW, *PFEATURE_DATA_MRW;

// 0x0029 is Reserved

// 0x002A - FeatureDvdPlusRW
typedef struct _FEATURE_DATA_DVD_PLUS_RW {
    FEATURE_HEADER Header;
    UCHAR Write     : 1;
    UCHAR Reserved1 : 7;
    UCHAR Reserved2[3];
} FEATURE_DATA_DVD_PLUS_RW, *PFEATURE_DATA_DVD_PLUS_RW;

// 0x002b is Reserved

// 0x002c - FeatureDvdRwRestrictedOverwrite
typedef struct _FEATURE_DATA_DVD_RW_RESTRICTED_OVERWRITE {
    FEATURE_HEADER Header;
    UCHAR Blank                    : 1;
    UCHAR Intermediate             : 1;
    UCHAR DefectStatusDataRead     : 1;
    UCHAR DefectStatusDataGenerate : 1;
    UCHAR Reserved0                : 4;
    UCHAR Reserved1[3];
} FEATURE_DATA_DVD_RW_RESTRICTED_OVERWRITE, *PFEATURE_DATA_DVD_RW_RESTRICTED_OVERWRITE;

// 0x002d - FeatureCdTrackAtOnce
typedef struct _FEATURE_DATA_CD_TRACK_AT_ONCE {
    FEATURE_HEADER Header;
    UCHAR RWSubchannelsRecordable  : 1;
    UCHAR CdRewritable             : 1;
    UCHAR TestWriteOk              : 1;
    UCHAR RWSubchannelPackedOk     : 1; // MMC 3 +
    UCHAR RWSubchannelRawOk        : 1; // MMC 3 +
    UCHAR Reserved1                : 1;
    UCHAR BufferUnderrunFree       : 1; // MMC 3 +
    UCHAR Reserved3                : 1;
    UCHAR Reserved2;
    UCHAR DataTypeSupported[2];   // [0] == MSB, [1] == LSB // see also FeatureIncrementalStreamingWritable
} FEATURE_DATA_CD_TRACK_AT_ONCE, *PFEATURE_DATA_CD_TRACK_AT_ONCE;

// 0x002e - FeatureCdMastering
typedef struct _FEATURE_DATA_CD_MASTERING {
    FEATURE_HEADER Header;
    UCHAR RWSubchannelsRecordable  : 1;
    UCHAR CdRewritable             : 1;
    UCHAR TestWriteOk              : 1;
    UCHAR RawRecordingOk           : 1;
    UCHAR RawMultiSessionOk        : 1;
    UCHAR SessionAtOnceOk          : 1;
    UCHAR BufferUnderrunFree       : 1;
    UCHAR Reserved1                : 1;
    UCHAR MaximumCueSheetLength[3]; // [0] == MSB, [2] == LSB
} FEATURE_DATA_CD_MASTERING, *PFEATURE_DATA_CD_MASTERING;

// 0x002f - FeatureDvdRecordableWrite
typedef struct _FEATURE_DATA_DVD_RECORDABLE_WRITE {
    FEATURE_HEADER Header;
    UCHAR Reserved1                : 1;
    UCHAR DVD_RW                   : 1;
    UCHAR TestWrite                : 1;
    UCHAR Reserved2                : 3;
    UCHAR BufferUnderrunFree       : 1;
    UCHAR Reserved3                : 1;
    UCHAR Reserved4[3];
} FEATURE_DATA_DVD_RECORDABLE_WRITE, *PFEATURE_DATA_DVD_RECORDABLE_WRITE;




// 0x0030 - FeatureDDCDRead
typedef struct _FEATURE_DATA_DDCD_READ {
    FEATURE_HEADER Header;
} FEATURE_DATA_DDCD_READ, *PFEATURE_DATA_DDCD_READ;

// 0x0031 - FeatureDDCDRWrite
typedef struct _FEATURE_DATA_DDCD_R_WRITE {
    FEATURE_HEADER Header;
    UCHAR Reserved1               : 2;
    UCHAR TestWrite               : 1;
    UCHAR Reserved2               : 5;
    UCHAR Reserved3[3];
} FEATURE_DATA_DDCD_R_WRITE, *PFEATURE_DATA_DDCD_R_WRITE;

// 0x0032 - FeatureDDCDRWWrite
typedef struct _FEATURE_DATA_DDCD_RW_WRITE {
    FEATURE_HEADER Header;
    UCHAR Blank                   : 1;
    UCHAR Intermediate            : 1;
    UCHAR Reserved1               : 6;
    UCHAR Reserved2[3];
} FEATURE_DATA_DDCD_RW_WRITE, *PFEATURE_DATA_DDCD_RW_WRITE;

// 0x0033 - 0x00ff are Reserved

// 0x0100 - FeaturePowerManagement
typedef struct _FEATURE_DATA_POWER_MANAGEMENT {
    FEATURE_HEADER Header;
} FEATURE_DATA_POWER_MANAGEMENT, *PFEATURE_DATA_POWER_MANAGEMENT;

// 0x0101 - FeatureSMART (not in MMC 2)
typedef struct _FEATURE_DATA_SMART {
    FEATURE_HEADER Header;
    UCHAR FaultFailureReportingPagePresent : 1;
    UCHAR Reserved1                        : 7;
    UCHAR Reserved2;
} FEATURE_DATA_SMART, *PFEATURE_DATA_SMART;

// 0x0102 - FeatureEmbeddedChanger
typedef struct _FEATURE_DATA_EMBEDDED_CHANGER {
    FEATURE_HEADER Header;
    UCHAR Reserved1                : 2;
    UCHAR SupportsDiscPresent      : 1;
    UCHAR Reserved2                : 1;
    UCHAR SideChangeCapable        : 1;
    UCHAR Reserved3                : 3;
    UCHAR Reserved4[2];
    UCHAR HighestSlotNumber        : 5;
    UCHAR Reserved                 : 3;
} FEATURE_DATA_EMBEDDED_CHANGER, *PFEATURE_DATA_EMBEDDED_CHANGER;

// 0x0103 - FeatureCDAudioAnalogPlay
typedef struct _FEATURE_DATA_CD_AUDIO_ANALOG_PLAY {
    FEATURE_HEADER Header;
    UCHAR SeperateVolume           : 1;
    UCHAR SeperateChannelMute      : 1;
    UCHAR ScanSupported            : 1;
    UCHAR Reserved1                : 5;
    UCHAR Reserved2;
    UCHAR NumerOfVolumeLevels[2];  // [0] == MSB, [1] == LSB
} FEATURE_DATA_CD_AUDIO_ANALOG_PLAY, *PFEATURE_DATA_CD_AUDIO_ANALOG_PLAY;

// 0x0104 - FeatureMicrocodeUpgrade
typedef struct _FEATURE_DATA_MICROCODE_UPDATE {
    FEATURE_HEADER Header;
} FEATURE_DATA_MICROCODE_UPDATE, *PFEATURE_DATA_MICROCODE_UPDATE;

// 0x0105 - FeatureTimeout
typedef struct _FEATURE_DATA_TIMEOUT {
    FEATURE_HEADER Header;
} FEATURE_DATA_TIMEOUT, *PFEATURE_DATA_TIMEOUT;

// 0x0106 - FeatureDvdCSS
typedef struct _FEATURE_DATA_DVD_CSS {
    FEATURE_HEADER Header;
    UCHAR Reserved1[3];
    UCHAR CssVersion;
} FEATURE_DATA_DVD_CSS, *PFEATURE_DATA_DVD_CSS;

// 0x0107 - FeatureRealTimeStreaming
typedef struct _FEATURE_DATA_REAL_TIME_STREAMING {
    FEATURE_HEADER Header;
    UCHAR StreamRecording         : 1;
    UCHAR WriteSpeedInGetPerf     : 1;
    UCHAR WriteSpeedInMP2A        : 1;
    UCHAR SetCDSpeed              : 1;
    UCHAR ReadBufferCapacityBlock : 1;
    UCHAR Reserved1               : 3;
    UCHAR Reserved2[3];
} FEATURE_DATA_REAL_TIME_STREAMING, *PFEATURE_DATA_REAL_TIME_STREAMING;

// 0x0108 - FeatureLogicalUnitSerialNumber
typedef struct _FEATURE_DATA_LOGICAL_UNIT_SERIAL_NUMBER {
    FEATURE_HEADER Header;
    UCHAR SerialNumber[0];
} FEATURE_DATA_LOGICAL_UNIT_SERIAL_NUMBER, *PFEATURE_DATA_LOGICAL_UNIT_SERIAL_NUMBER;

// 0x0109 is Reserved

// 0x010a
// an integral multiple of the _EX structures are returned for page 010A
typedef struct _FEATURE_DATA_DISC_CONTROL_BLOCKS_EX {
    UCHAR ContentDescriptor[4];
} FEATURE_DATA_DISC_CONTROL_BLOCKS_EX, *PFEATURE_DATA_DISC_CONTROL_BLOCKS_EX;
// use a zero-sized array for this....
typedef struct _FEATURE_DATA_DISC_CONTROL_BLOCKS {
    FEATURE_HEADER Header;
    FEATURE_DATA_DISC_CONTROL_BLOCKS_EX Data[0];
} FEATURE_DATA_DISC_CONTROL_BLOCKS, *PFEATURE_DATA_DISC_CONTROL_BLOCKS;

// 0x010b
typedef struct _FEATURE_DATA_DVD_CPRM {
    FEATURE_HEADER Header;
    UCHAR Reserved0[3];
    UCHAR CPRMVersion;
} FEATURE_DATA_DVD_CPRM, *PFEATURE_DATA_DVD_CPRM;

// 0x010c - 0xfeff are Reserved
typedef struct _FEATURE_DATA_RESERVED {
    FEATURE_HEADER Header;
    UCHAR Data[0];
} FEATURE_DATA_RESERVED, *PFEATURE_DATA_RESERVED;

// 0xff00 - 0xffff are Vendor Specific
typedef struct _FEATURE_DATA_VENDOR_SPECIFIC {
    FEATURE_HEADER Header;
    UCHAR VendorSpecificData[0];
} FEATURE_DATA_VENDOR_SPECIFIC, *PFEATURE_DATA_VENDOR_SPECIFIC;


//
// NOTE: All FEATURE_* structures may be extended.  use of these structures
//       requires verification that the FeatureHeader->AdditionLength field
//       contains AT LEAST enough data to cover the data fields being accessed.
//       This is due to the design, which allows extending the size of the
//       various structures, which will result in these structures sizes
//       being changed over time.
//       *** Programmers beware! ***
//

//
// NOTE: This is based on MMC 3
//       Further revisions will maintain backward compatibility
//       with the non-reserved fields listed here.  If you need
//       to access a new field, please typecast to FEATURE_DATA_RESERVED
//       and access the appropriate bits there.
//

//
// IOCTL_CDROM_GET_CONFIGURATION returns a FEATURE_* struct, which always
//       starts with a FEATURE_HEADER structure.
//

//
// these are to be used for the request type
//

#define SCSI_GET_CONFIGURATION_REQUEST_TYPE_ALL     0x0
#define SCSI_GET_CONFIGURATION_REQUEST_TYPE_CURRENT 0x1
#define SCSI_GET_CONFIGURATION_REQUEST_TYPE_ONE     0x2


typedef struct _GET_CONFIGURATION_IOCTL_INPUT {
    FEATURE_NUMBER Feature;
    ULONG          RequestType; // SCSI_GET_CONFIGURATION_REQUEST_TYPE_*
    PVOID          Reserved[2];
} GET_CONFIGURATION_IOCTL_INPUT, *PGET_CONFIGURATION_IOCTL_INPUT;

#if defined(_WIN64)
typedef struct _GET_CONFIGURATION_IOCTL_INPUT32 {
    FEATURE_NUMBER   Feature;
    ULONG            RequestType; // SCSI_GET_CONFIGURATION_REQUEST_TYPE_*
    VOID* POINTER_32 Reserved[2];
} GET_CONFIGURATION_IOCTL_INPUT32, *PGET_CONFIGURATION_IOCTL_INPUT32;
#endif


#if _MSC_VER >= 1200
#pragma warning(pop)          // un-sets any local warning changes
#endif
#pragma warning(default:4200) // array[0] is not a warning for this file


#endif // __NTDDMMC__

