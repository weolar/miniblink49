//------------------------------------------------------------------------------
// File: BDATypes.h
//
// Desc: Typedefs and enums needed by both the WDM drivers and the user mode
//       COM interfaces.
//
// Copyright (c) 1999 - 2004, Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef _BDATYPES_

#define _BDATYPES_      1

// !!!! do not #pragma once, we use this file twice(once for native and once for mgd) in managed interop
#include <exposeenums2managed.h>

/* Utility Macros */

#define MIN_DIMENSION   1


//===========================================================================
//
//  BDA Topology Structures
//
//===========================================================================

#ifndef MANAGED_ENUMS

typedef struct _BDA_TEMPLATE_CONNECTION
{
    ULONG   FromNodeType;
    ULONG   FromNodePinType;
    ULONG   ToNodeType;
    ULONG   ToNodePinType;
}BDA_TEMPLATE_CONNECTION, *PBDA_TEMPLATE_CONNECTION;


typedef struct _BDA_TEMPLATE_PIN_JOINT
{
    ULONG   uliTemplateConnection;
    ULONG   ulcInstancesMax;
}BDA_TEMPLATE_PIN_JOINT, *PBDA_TEMPLATE_PIN_JOINT;
#endif


//===========================================================================
//
//  BDA Events
//
//===========================================================================

//  In-band Event IDs
//
ENUM BDA_EVENT_ID {
    BDA_EVENT_SIGNAL_LOSS = 0,
    BDA_EVENT_SIGNAL_LOCK,
    BDA_EVENT_DATA_START,
    BDA_EVENT_DATA_STOP,
    BDA_EVENT_CHANNEL_ACQUIRED,
    BDA_EVENT_CHANNEL_LOST,
    BDA_EVENT_CHANNEL_SOURCE_CHANGED,
    BDA_EVENT_CHANNEL_ACTIVATED,
    BDA_EVENT_CHANNEL_DEACTIVATED,
    BDA_EVENT_SUBCHANNEL_ACQUIRED,
    BDA_EVENT_SUBCHANNEL_LOST,
    BDA_EVENT_SUBCHANNEL_SOURCE_CHANGED,
    BDA_EVENT_SUBCHANNEL_ACTIVATED,
    BDA_EVENT_SUBCHANNEL_DEACTIVATED,
    BDA_EVENT_ACCESS_GRANTED,
    BDA_EVENT_ACCESS_DENIED,
    BDA_EVENT_OFFER_EXTENDED,
    BDA_EVENT_PURCHASE_COMPLETED,
    BDA_EVENT_SMART_CARD_INSERTED,
    BDA_EVENT_SMART_CARD_REMOVED
} BDA_EVENT_ID, *PBDA_EVENT_ID;



//===========================================================================
//
//  KSSTREAM_HEADER extensions for BDA
//
//===========================================================================

#ifndef MANAGED_ENUMS
typedef struct tagKS_BDA_FRAME_INFO {
    ULONG                   ExtendedHeaderSize; // Size of this extended header
    DWORD                   dwFrameFlags;  //
    ULONG                   ulEvent; //
    ULONG                   ulChannelNumber; //
    ULONG                   ulSubchannelNumber; //
    ULONG                   ulReason; //
} KS_BDA_FRAME_INFO, *PKS_BDA_FRAME_INFO;


//------------------------------------------------------------
//
//  BDA Network Ethernet Filter Property Set
//
// {71985F43-1CA1-11d3-9CC8-00C04F7971E0}
//
typedef struct _BDA_ETHERNET_ADDRESS {
    BYTE    rgbAddress[6];
} BDA_ETHERNET_ADDRESS, *PBDA_ETHERNET_ADDRESS;

typedef struct _BDA_ETHERNET_ADDRESS_LIST {
    ULONG               ulcAddresses;
    BDA_ETHERNET_ADDRESS    rgAddressl[MIN_DIMENSION];
} BDA_ETHERNET_ADDRESS_LIST, * PBDA_ETHERNET_ADDRESS_LIST;

#endif

ENUM BDA_MULTICAST_MODE {
    BDA_PROMISCUOUS_MULTICAST = 0,
    BDA_FILTERED_MULTICAST,
    BDA_NO_MULTICAST
} BDA_MULTICAST_MODE, *PBDA_MULTICAST_MODE;


//------------------------------------------------------------
//
//  BDA Network IPv4 Filter Property Set
//
// {71985F44-1CA1-11d3-9CC8-00C04F7971E0}
//
#ifndef MANAGED_ENUMS
typedef struct _BDA_IPv4_ADDRESS {
    BYTE    rgbAddress[4];
} BDA_IPv4_ADDRESS, *PBDA_IPv4_ADDRESS;

typedef struct _BDA_IPv4_ADDRESS_LIST {
    ULONG               ulcAddresses;
    BDA_IPv4_ADDRESS    rgAddressl[MIN_DIMENSION];
} BDA_IPv4_ADDRESS_LIST, * PBDA_IPv4_ADDRESS_LIST;

//------------------------------------------------------------
//
//  BDA Network IPv4 Filter Property Set
//
// {E1785A74-2A23-4fb3-9245-A8F88017EF33}
//
typedef struct _BDA_IPv6_ADDRESS {
    BYTE    rgbAddress[6];
} BDA_IPv6_ADDRESS, *PBDA_IPv6_ADDRESS;

typedef struct _BDA_IPv6_ADDRESS_LIST {
    ULONG               ulcAddresses;
    BDA_IPv6_ADDRESS    rgAddressl[MIN_DIMENSION];
} BDA_IPv6_ADDRESS_LIST, * PBDA_IPv6_ADDRESS_LIST;

#endif


//------------------------------------------------------------
//
//
//  BDA Signal Property Set
//
//  {D2F1644B-B409-11d2-BC69-00A0C9EE9E16}
//
ENUM BDA_SIGNAL_STATE {
    BDA_SIGNAL_UNAVAILABLE = 0,
    BDA_SIGNAL_INACTIVE,
    BDA_SIGNAL_ACTIVE
} BDA_SIGNAL_STATE, * PBDA_SIGNAL_STATE;


//------------------------------------------------------------
//
//
//  BDA Change Sync Method Set
//
// {FD0A5AF3-B41D-11d2-9C95-00C04F7971E0}
//
ENUM BDA_CHANGE_STATE {
    BDA_CHANGES_COMPLETE = 0,
    BDA_CHANGES_PENDING
} BDA_CHANGE_STATE, * PBDA_CHANGE_STATE;


//------------------------------------------------------------
//
//
//  BDA Device Configuration Method Set
//
// {71985F45-1CA1-11d3-9CC8-00C04F7971E0}
//


//------------------------------------------------------------
//
//
//  BDA Topology Property Set
//
// {A14EE835-0A23-11d3-9CC7-00C04F7971E0}
//

#ifndef MANAGED_ENUMS
typedef struct _BDANODE_DESCRIPTOR
{
    ULONG               ulBdaNodeType;  // The node type as it is used
                                        // in the BDA template topology

    GUID                guidFunction;   // GUID from BdaMedia.h describing
                                        // the node's function (e.g.
                                        // KSNODE_BDA_RF_TUNER)

    GUID                guidName;       // GUID that can be use to look up
                                        // a displayable name for the node.
} BDANODE_DESCRIPTOR, *PBDANODE_DESCRIPTOR;


//------------------------------------------------------------
//
//
//  BDA Void Transform Property Set
//
// {71985F46-1CA1-11d3-9CC8-00C04F7971E0}
//


//------------------------------------------------------------
//
//
//  BDA Null Transform Property Set
//
// {DDF15B0D-BD25-11d2-9CA0-00C04F7971E0}
//


//------------------------------------------------------------
//
//
//  BDA Frequency Filter Property Set
//
// {71985F47-1CA1-11d3-9CC8-00C04F7971E0}
//


//------------------------------------------------------------
//
//
//  BDA Autodemodulate Property Set
//
// {DDF15B12-BD25-11d2-9CA0-00C04F7971E0}
//


//------------------------------------------------------------
//
//
//  BDA Table Section Property Set
//
// {516B99C5-971C-4aaf-B3F3-D9FDA8A15E16}
//

typedef struct _BDA_TABLE_SECTION
{
    ULONG               ulPrimarySectionId;
    ULONG               ulSecondarySectionId;
    ULONG               ulcbSectionLength;
    ULONG               argbSectionData[MIN_DIMENSION];
} BDA_TABLE_SECTION, *PBDA_TABLE_SECTION;

#endif
//------------------------------------------------------------
//
//
//  BDA PID Filter Property Set
//
// {D0A67D65-08DF-4fec-8533-E5B550410B85}
//

//---------------------------------------------------------------------
// From IEnumPIDMap interface
//---------------------------------------------------------------------

ENUM MEDIA_SAMPLE_CONTENT {
    MEDIA_TRANSPORT_PACKET,         //  complete TS packet e.g. pass-through mode
    MEDIA_ELEMENTARY_STREAM,        //  PES payloads; audio/video only
    MEDIA_MPEG2_PSI,                //  PAT, PMT, CAT, Private
    MEDIA_TRANSPORT_PAYLOAD         //  gathered TS packet payloads (PES packets, etc...)
} MEDIA_SAMPLE_CONTENT ;

#ifndef MANAGED_ENUMS
typedef struct {
    ULONG                   ulPID ;
    MEDIA_SAMPLE_CONTENT    MediaSampleContent ;
} PID_MAP ;

typedef struct _BDA_PID_MAP
{
    MEDIA_SAMPLE_CONTENT    MediaSampleContent;
    ULONG                   ulcPIDs;
    ULONG                   aulPIDs[MIN_DIMENSION];
} BDA_PID_MAP, *PBDA_PID_MAP;

typedef struct _BDA_PID_UNMAP
{
    ULONG               ulcPIDs;
    ULONG               aulPIDs[MIN_DIMENSION];
} BDA_PID_UNMAP, *PBDA_PID_UNMAP;


//------------------------------------------------------------
//
//
//  BDA CA Property Set
//
// {B0693766-5278-4ec6-B9E1-3CE40560EF5A}
//
typedef struct _BDA_CA_MODULE_UI
{
    ULONG   ulFormat;
    ULONG   ulbcDesc;
    ULONG   ulDesc[MIN_DIMENSION];
} BDA_CA_MODULE_UI, *PBDA_CA_MODULE_UI;

typedef struct _BDA_PROGRAM_PID_LIST
{
    ULONG   ulProgramNumber;
    ULONG   ulcPIDs;
    ULONG   ulPID[MIN_DIMENSION];
} BDA_PROGRAM_PID_LIST, *PBDA_PROGRAM_PID_LIST;

#endif

//------------------------------------------------------------
//
//
//  BDA CA Event Set
//
// {488C4CCC-B768-4129-8EB1-B00A071F9068}
//



//=============================================================
//
//
//  BDA Tuning Model enumerations
//
//
//=============================================================

// system type for particular DVB Tuning Space instance
ENUM DVBSystemType {
    DVB_Cable,
    DVB_Terrestrial,
    DVB_Satellite,
} DVBSystemType;

//------------------------------------------------------------
//
//  BDA Channel Tune Request

ENUM  BDA_Channel {
    BDA_UNDEFINED_CHANNEL = -1,
} BDA_Channel ;


//------------------------------------------------------------
//
//  BDA Component(substream)
//

ENUM ComponentCategory {
    CategoryNotSet = -1,
    CategoryOther = 0,
    CategoryVideo,
    CategoryAudio,
    CategoryText,
    CategoryData,
} ComponentCategory;

// Component Status
ENUM ComponentStatus {
    StatusActive,
    StatusInactive,
    StatusUnavailable,
} ComponentStatus;


//------------------------------------------------------------
//
//  BDA MPEG2 Component Type
//
// from the MPEG2 specification
ENUM MPEG2StreamType {
    BDA_UNITIALIZED_MPEG2STREAMTYPE = -1,
    Reserved1 = 0x0,
    ISO_IEC_11172_2_VIDEO   = Reserved1 + 1,
    ISO_IEC_13818_2_VIDEO   = ISO_IEC_11172_2_VIDEO + 1,
    ISO_IEC_11172_3_AUDIO   = ISO_IEC_13818_2_VIDEO + 1,
    ISO_IEC_13818_3_AUDIO   = ISO_IEC_11172_3_AUDIO + 1,
    ISO_IEC_13818_1_PRIVATE_SECTION = ISO_IEC_13818_3_AUDIO + 1,
    ISO_IEC_13818_1_PES     = ISO_IEC_13818_1_PRIVATE_SECTION + 1,
    ISO_IEC_13522_MHEG      = ISO_IEC_13818_1_PES + 1,
    ANNEX_A_DSM_CC          = ISO_IEC_13522_MHEG + 1,
    ITU_T_REC_H_222_1       = ANNEX_A_DSM_CC + 1,
    ISO_IEC_13818_6_TYPE_A  = ITU_T_REC_H_222_1 + 1,
    ISO_IEC_13818_6_TYPE_B  = ISO_IEC_13818_6_TYPE_A + 1,
    ISO_IEC_13818_6_TYPE_C  = ISO_IEC_13818_6_TYPE_B + 1,
    ISO_IEC_13818_6_TYPE_D  = ISO_IEC_13818_6_TYPE_C + 1,
    ISO_IEC_13818_1_AUXILIARY = ISO_IEC_13818_6_TYPE_D + 1,
    ISO_IEC_13818_1_RESERVED = ISO_IEC_13818_1_AUXILIARY + 1,
    USER_PRIVATE            = ISO_IEC_13818_1_RESERVED + 1
} MPEG2StreamType;

//------------------------------------------------------------
//
//  mpeg-2 transport stride format block; associated with media
//   types MEDIATYPE_Stream/MEDIASUBTYPE_MPEG2_TRANSPORT_STRIDE;
//   *all* format blocks associated with above media type *must*
//   start with the MPEG2_TRANSPORT_STRIDE structure
//

#ifndef MANAGED_ENUMS
typedef struct _MPEG2_TRANSPORT_STRIDE {
    DWORD   dwOffset ;
    DWORD   dwPacketLength ;
    DWORD   dwStride ;
} MPEG2_TRANSPORT_STRIDE, *PMPEG2_TRANSPORT_STRIDE ;
#endif

//------------------------------------------------------------
//
//  BDA ATSC Component Type
//
//
// ATSC made AC3 Audio a descriptor instead of
// defining a user private stream type.

FLAGS ATSCComponentTypeFlags {
    // bit flags for various component type properties
    ATSCCT_AC3 = 0x00000001,
} ATSCComponentTypeFlags;


//------------------------------------------------------------
//
//  BDA Locators
//


ENUM BinaryConvolutionCodeRate {
    BDA_BCC_RATE_NOT_SET = -1,
    BDA_BCC_RATE_NOT_DEFINED = 0,
    BDA_BCC_RATE_1_2 = 1,   // 1/2
    BDA_BCC_RATE_2_3,   // 2/3
    BDA_BCC_RATE_3_4,   // 3/4
    BDA_BCC_RATE_3_5,
    BDA_BCC_RATE_4_5,
    BDA_BCC_RATE_5_6,   // 5/6
    BDA_BCC_RATE_5_11,
    BDA_BCC_RATE_7_8,   // 7/8
    BDA_BCC_RATE_MAX,
} BinaryConvolutionCodeRate;

ENUM FECMethod {
    BDA_FEC_METHOD_NOT_SET = -1,
    BDA_FEC_METHOD_NOT_DEFINED = 0,
    BDA_FEC_VITERBI = 1,          // FEC is a Viterbi Binary Convolution.
    BDA_FEC_RS_204_188,       // The FEC is Reed-Solomon 204/188 (outer FEC)
    BDA_FEC_MAX,
} FECMethod;

ENUM ModulationType {
    BDA_MOD_NOT_SET = -1,
    BDA_MOD_NOT_DEFINED = 0,
    BDA_MOD_16QAM = 1,
    BDA_MOD_32QAM,
    BDA_MOD_64QAM,
    BDA_MOD_80QAM,
    BDA_MOD_96QAM,
    BDA_MOD_112QAM,
    BDA_MOD_128QAM,
    BDA_MOD_160QAM,
    BDA_MOD_192QAM,
    BDA_MOD_224QAM,
    BDA_MOD_256QAM,
    BDA_MOD_320QAM,
    BDA_MOD_384QAM,
    BDA_MOD_448QAM,
    BDA_MOD_512QAM,
    BDA_MOD_640QAM,
    BDA_MOD_768QAM,
    BDA_MOD_896QAM,
    BDA_MOD_1024QAM,
    BDA_MOD_QPSK,
    BDA_MOD_BPSK,
    BDA_MOD_OQPSK,
    BDA_MOD_8VSB,
    BDA_MOD_16VSB,
    BDA_MOD_ANALOG_AMPLITUDE,  // std am
    BDA_MOD_ANALOG_FREQUENCY,  // std fm
    BDA_MOD_8PSK,
    BDA_MOD_MAX,
} ModulationType;

ENUM SpectralInversion {
    BDA_SPECTRAL_INVERSION_NOT_SET = -1,
    BDA_SPECTRAL_INVERSION_NOT_DEFINED = 0,
    BDA_SPECTRAL_INVERSION_AUTOMATIC = 1,
    BDA_SPECTRAL_INVERSION_NORMAL,
    BDA_SPECTRAL_INVERSION_INVERTED,
    BDA_SPECTRAL_INVERSION_MAX
} SpectralInversion;

ENUM Polarisation {
    BDA_POLARISATION_NOT_SET = -1,
    BDA_POLARISATION_NOT_DEFINED = 0,
    BDA_POLARISATION_LINEAR_H = 1, // Linear horizontal polarisation
    BDA_POLARISATION_LINEAR_V, // Linear vertical polarisation
    BDA_POLARISATION_CIRCULAR_L, // Circular left polarisation
    BDA_POLARISATION_CIRCULAR_R, // Circular right polarisation
    BDA_POLARISATION_MAX,
} Polarisation;

ENUM GuardInterval {
    BDA_GUARD_NOT_SET = -1,
    BDA_GUARD_NOT_DEFINED = 0,
    BDA_GUARD_1_32 = 1, // Guard interval is 1/32
    BDA_GUARD_1_16, // Guard interval is 1/16
    BDA_GUARD_1_8, // Guard interval is 1/8
    BDA_GUARD_1_4, // Guard interval is 1/4
    BDA_GUARD_MAX,
} GuardInterval;

ENUM HierarchyAlpha {
    BDA_HALPHA_NOT_SET = -1,
    BDA_HALPHA_NOT_DEFINED = 0,
    BDA_HALPHA_1 = 1, // Hierarchy alpha is 1.
    BDA_HALPHA_2, // Hierarchy alpha is 2.
    BDA_HALPHA_4, // Hierarchy alpha is 4.
    BDA_HALPHA_MAX,
} HierarchyAlpha;

ENUM TransmissionMode {
    BDA_XMIT_MODE_NOT_SET = -1,
    BDA_XMIT_MODE_NOT_DEFINED = 0,
    BDA_XMIT_MODE_2K = 1, // Transmission uses 1705 carriers (use a 2K FFT)
    BDA_XMIT_MODE_8K, // Transmission uses 6817 carriers (use an 8K FFT)
    BDA_XMIT_MODE_MAX,
} TransmissionMode;

//  Settings for Tuner Frequency
//
ENUM BDA_Frequency {
    BDA_FREQUENCY_NOT_SET = -1,
    BDA_FREQUENCY_NOT_DEFINED = 0
} BDA_Frequency;

//  Settings for Tuner Range
//
//  Tuner range refers to the setting of LNB High/Low as well as the
//  selection of a satellite on a multiple satellite switch.
//
ENUM BDA_Range {
    BDA_RANGE_NOT_SET = -1,
    BDA_RANGE_NOT_DEFINED = 0
} BDA_Range;

//  Settings for Tuner Channel Bandwidth
//
ENUM BDA_Channel_Bandwidth {
    BDA_CHAN_BANDWITH_NOT_SET      = -1,
    BDA_CHAN_BANDWITH_NOT_DEFINED  = 0
} BDA_Channel_Bandwidth;

//  Settings for Tuner Frequency Multiplier
//
ENUM BDA_Frequency_Multiplier {
    BDA_FREQUENCY_MULTIPLIER_NOT_SET       = -1,
    BDA_FREQUENCY_MULTIPLIER_NOT_DEFINED   = 0 
} BDA_Frequency_Multiplier;

FLAGS BDA_Comp_Flags {
    // equiv comparison rule overrides, default behavior is type specific
    BDACOMP_NOT_DEFINED              = 0x00000000,  
    BDACOMP_EXCLUDE_TS_FROM_TR       = 0x00000001,  // never put TS in TR equiv comparison
    BDACOMP_INCLUDE_LOCATOR_IN_TR    = 0x00000002,  // always include loc in TR equiv comparison
} BDA_Comp_Flags;

#include <unexposeenums2managed.h>

#endif // not defined _BDATYPES_

// end of file -- bdatypes.h

