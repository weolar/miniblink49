/*++ BUILD Version: 0001        // Increment this if a change has global effects

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    ntddndis.h

Abstract:

    This is the include file that defines all constants and types for
    accessing the Network driver interface device.

Author:

    NDIS/ATM Development Team

Revision History:

    added the correct values for NDIS 3.0.
    added Pnp IoCTLs and structures
    added general co ndis oids.
    added PnP and PM OIDs.

--*/

#ifndef _NTDDNDIS_
#define _NTDDNDIS_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
// Device Name - this string is the name of the device. It is the name
// that should be passed to NtOpenFile when accessing the device.

//
// Note: For devices that support multiple units, it should be suffixed
//       with the Ascii representation of the unit number.
//

#define DD_NDIS_DEVICE_NAME "\\Device\\UNKNOWN"


//
// NtDeviceIoControlFile IoControlCode values for this device.
//
// Warning: Remember that the low two bits of the code specify how the
//          buffers are passed to the driver!
//

#define _NDIS_CONTROL_CODE(request,method) \
            CTL_CODE(FILE_DEVICE_PHYSICAL_NETCARD, request, method, FILE_ANY_ACCESS)

#define IOCTL_NDIS_QUERY_GLOBAL_STATS   _NDIS_CONTROL_CODE(0, METHOD_OUT_DIRECT)
#define IOCTL_NDIS_QUERY_ALL_STATS      _NDIS_CONTROL_CODE(1, METHOD_OUT_DIRECT)
#define IOCTL_NDIS_DO_PNP_OPERATION     _NDIS_CONTROL_CODE(2, METHOD_BUFFERED)
#define IOCTL_NDIS_QUERY_SELECTED_STATS _NDIS_CONTROL_CODE(3, METHOD_OUT_DIRECT)
#define IOCTL_NDIS_ENUMERATE_INTERFACES _NDIS_CONTROL_CODE(4, METHOD_BUFFERED)
#define IOCTL_NDIS_ADD_TDI_DEVICE       _NDIS_CONTROL_CODE(5, METHOD_BUFFERED)
#define IOCTL_NDIS_GET_LOG_DATA         _NDIS_CONTROL_CODE(7, METHOD_OUT_DIRECT)
#define IOCTL_NDIS_GET_VERSION          _NDIS_CONTROL_CODE(8, METHOD_BUFFERED)
#define IOCTL_NDIS_RESERVED7            _NDIS_CONTROL_CODE(0xF, METHOD_OUT_DIRECT)


//
// NtDeviceIoControlFile InputBuffer/OutputBuffer record structures for
// this device.
//

//
// This is the type of an NDIS OID value.
//

typedef ULONG NDIS_OID, *PNDIS_OID;

//
// IOCTL_NDIS_QUERY_ALL_STATS returns a sequence of these, packed
// together.  This structure is unaligned because not all statistics
// have a length that is a ULONG multiple.
//

typedef UNALIGNED struct _NDIS_STATISTICS_VALUE
{
    NDIS_OID    Oid;
    ULONG       DataLength;
    UCHAR       Data[1];            // variable length
} NDIS_STATISTICS_VALUE, *PNDIS_STATISTICS_VALUE;

//
// Structure used to define a self-contained variable data structure
//
typedef struct _NDIS_VAR_DATA_DESC
{
    USHORT      Length;         // # of octects of data
    USHORT      MaximumLength;  // # of octects available
    ULONG_PTR   Offset;         // Offset of data relative to the descriptor
} NDIS_VAR_DATA_DESC, *PNDIS_VAR_DATA_DESC;

#ifndef GUID_DEFINED
#include <guiddef.h>
#endif // !GUID_DEFINED

//
// General Objects
//

//
//  Required OIDs
//
#define OID_GEN_SUPPORTED_LIST                  0x00010101
#define OID_GEN_HARDWARE_STATUS                 0x00010102
#define OID_GEN_MEDIA_SUPPORTED                 0x00010103
#define OID_GEN_MEDIA_IN_USE                    0x00010104
#define OID_GEN_MAXIMUM_LOOKAHEAD               0x00010105
#define OID_GEN_MAXIMUM_FRAME_SIZE              0x00010106
#define OID_GEN_LINK_SPEED                      0x00010107
#define OID_GEN_TRANSMIT_BUFFER_SPACE           0x00010108
#define OID_GEN_RECEIVE_BUFFER_SPACE            0x00010109
#define OID_GEN_TRANSMIT_BLOCK_SIZE             0x0001010A
#define OID_GEN_RECEIVE_BLOCK_SIZE              0x0001010B
#define OID_GEN_VENDOR_ID                       0x0001010C
#define OID_GEN_VENDOR_DESCRIPTION              0x0001010D
#define OID_GEN_CURRENT_PACKET_FILTER           0x0001010E
#define OID_GEN_CURRENT_LOOKAHEAD               0x0001010F
#define OID_GEN_DRIVER_VERSION                  0x00010110
#define OID_GEN_MAXIMUM_TOTAL_SIZE              0x00010111
#define OID_GEN_PROTOCOL_OPTIONS                0x00010112
#define OID_GEN_MAC_OPTIONS                     0x00010113
#define OID_GEN_MEDIA_CONNECT_STATUS            0x00010114
#define OID_GEN_MAXIMUM_SEND_PACKETS            0x00010115


//
//  Optional OIDs
//
#define OID_GEN_VENDOR_DRIVER_VERSION           0x00010116
#define OID_GEN_SUPPORTED_GUIDS                 0x00010117
#define OID_GEN_NETWORK_LAYER_ADDRESSES         0x00010118  // Set only
#define OID_GEN_TRANSPORT_HEADER_OFFSET         0x00010119  // Set only
#define OID_GEN_MACHINE_NAME                    0x0001021A
#define OID_GEN_RNDIS_CONFIG_PARAMETER          0x0001021B  // Set only
#define OID_GEN_VLAN_ID                         0x0001021C
#define OID_GEN_MEDIA_CAPABILITIES              0x00010201
#define OID_GEN_PHYSICAL_MEDIUM                 0x00010202

//
//  Required statistics
//
#define OID_GEN_XMIT_OK                         0x00020101
#define OID_GEN_RCV_OK                          0x00020102
#define OID_GEN_XMIT_ERROR                      0x00020103
#define OID_GEN_RCV_ERROR                       0x00020104
#define OID_GEN_RCV_NO_BUFFER                   0x00020105

//
//  Optional statistics
//
#define OID_GEN_DIRECTED_BYTES_XMIT             0x00020201
#define OID_GEN_DIRECTED_FRAMES_XMIT            0x00020202
#define OID_GEN_MULTICAST_BYTES_XMIT            0x00020203
#define OID_GEN_MULTICAST_FRAMES_XMIT           0x00020204
#define OID_GEN_BROADCAST_BYTES_XMIT            0x00020205
#define OID_GEN_BROADCAST_FRAMES_XMIT           0x00020206
#define OID_GEN_DIRECTED_BYTES_RCV              0x00020207
#define OID_GEN_DIRECTED_FRAMES_RCV             0x00020208
#define OID_GEN_MULTICAST_BYTES_RCV             0x00020209
#define OID_GEN_MULTICAST_FRAMES_RCV            0x0002020A
#define OID_GEN_BROADCAST_BYTES_RCV             0x0002020B
#define OID_GEN_BROADCAST_FRAMES_RCV            0x0002020C

#define OID_GEN_RCV_CRC_ERROR                   0x0002020D
#define OID_GEN_TRANSMIT_QUEUE_LENGTH           0x0002020E

#define OID_GEN_GET_TIME_CAPS                   0x0002020F
#define OID_GEN_GET_NETCARD_TIME                0x00020210
#define OID_GEN_NETCARD_LOAD                    0x00020211
#define OID_GEN_DEVICE_PROFILE                  0x00020212

//
// The following is exported by NDIS itself and is only queryable. It returns
// the time in milliseconds a driver took to initialize.
//
#define OID_GEN_INIT_TIME_MS                    0x00020213
#define OID_GEN_RESET_COUNTS                    0x00020214
#define OID_GEN_MEDIA_SENSE_COUNTS              0x00020215
#define OID_GEN_FRIENDLY_NAME                   0x00020216
#define OID_GEN_MINIPORT_INFO                   0x00020217
#define OID_GEN_RESET_VERIFY_PARAMETERS         0x00020218


//
//  These are connection-oriented general OIDs.
//  These replace the above OIDs for connection-oriented media.
//
#define OID_GEN_CO_SUPPORTED_LIST               OID_GEN_SUPPORTED_LIST
#define OID_GEN_CO_HARDWARE_STATUS              OID_GEN_HARDWARE_STATUS
#define OID_GEN_CO_MEDIA_SUPPORTED              OID_GEN_MEDIA_SUPPORTED
#define OID_GEN_CO_MEDIA_IN_USE                 OID_GEN_MEDIA_IN_USE
#define OID_GEN_CO_LINK_SPEED                   OID_GEN_LINK_SPEED
#define OID_GEN_CO_VENDOR_ID                    OID_GEN_VENDOR_ID
#define OID_GEN_CO_VENDOR_DESCRIPTION           OID_GEN_VENDOR_DESCRIPTION
#define OID_GEN_CO_DRIVER_VERSION               OID_GEN_DRIVER_VERSION
#define OID_GEN_CO_PROTOCOL_OPTIONS             OID_GEN_PROTOCOL_OPTIONS
#define OID_GEN_CO_MAC_OPTIONS                  OID_GEN_MAC_OPTIONS
#define OID_GEN_CO_MEDIA_CONNECT_STATUS         OID_GEN_MEDIA_CONNECT_STATUS
#define OID_GEN_CO_VENDOR_DRIVER_VERSION        OID_GEN_VENDOR_DRIVER_VERSION
#define OID_GEN_CO_SUPPORTED_GUIDS              OID_GEN_SUPPORTED_GUIDS
#define OID_GEN_CO_GET_TIME_CAPS                OID_GEN_GET_TIME_CAPS
#define OID_GEN_CO_GET_NETCARD_TIME             OID_GEN_GET_NETCARD_TIME
#define OID_GEN_CO_MINIMUM_LINK_SPEED           0x00020120

//
//  These are connection-oriented statistics OIDs.
//
#define OID_GEN_CO_XMIT_PDUS_OK                 OID_GEN_XMIT_OK
#define OID_GEN_CO_RCV_PDUS_OK                  OID_GEN_RCV_OK
#define OID_GEN_CO_XMIT_PDUS_ERROR              OID_GEN_XMIT_ERROR
#define OID_GEN_CO_RCV_PDUS_ERROR               OID_GEN_RCV_ERROR
#define OID_GEN_CO_RCV_PDUS_NO_BUFFER           OID_GEN_RCV_NO_BUFFER


#define OID_GEN_CO_RCV_CRC_ERROR                OID_GEN_RCV_CRC_ERROR
#define OID_GEN_CO_TRANSMIT_QUEUE_LENGTH        OID_GEN_TRANSMIT_QUEUE_LENGTH
#define OID_GEN_CO_BYTES_XMIT                   OID_GEN_DIRECTED_BYTES_XMIT
#define OID_GEN_CO_BYTES_RCV                    OID_GEN_DIRECTED_BYTES_RCV
#define OID_GEN_CO_NETCARD_LOAD                 OID_GEN_NETCARD_LOAD
#define OID_GEN_CO_DEVICE_PROFILE               OID_GEN_DEVICE_PROFILE
#define OID_GEN_CO_BYTES_XMIT_OUTSTANDING       0x00020221

//
// 802.3 Objects (Ethernet)
//
#define OID_802_3_PERMANENT_ADDRESS             0x01010101
#define OID_802_3_CURRENT_ADDRESS               0x01010102
#define OID_802_3_MULTICAST_LIST                0x01010103
#define OID_802_3_MAXIMUM_LIST_SIZE             0x01010104
#define OID_802_3_MAC_OPTIONS                   0x01010105

#define NDIS_802_3_MAC_OPTION_PRIORITY          0x00000001

#define OID_802_3_RCV_ERROR_ALIGNMENT           0x01020101
#define OID_802_3_XMIT_ONE_COLLISION            0x01020102
#define OID_802_3_XMIT_MORE_COLLISIONS          0x01020103

#define OID_802_3_XMIT_DEFERRED                 0x01020201
#define OID_802_3_XMIT_MAX_COLLISIONS           0x01020202
#define OID_802_3_RCV_OVERRUN                   0x01020203
#define OID_802_3_XMIT_UNDERRUN                 0x01020204
#define OID_802_3_XMIT_HEARTBEAT_FAILURE        0x01020205
#define OID_802_3_XMIT_TIMES_CRS_LOST           0x01020206
#define OID_802_3_XMIT_LATE_COLLISIONS          0x01020207


//
// 802.5 Objects (Token-Ring)
//
#define OID_802_5_PERMANENT_ADDRESS             0x02010101
#define OID_802_5_CURRENT_ADDRESS               0x02010102
#define OID_802_5_CURRENT_FUNCTIONAL            0x02010103
#define OID_802_5_CURRENT_GROUP                 0x02010104
#define OID_802_5_LAST_OPEN_STATUS              0x02010105
#define OID_802_5_CURRENT_RING_STATUS           0x02010106
#define OID_802_5_CURRENT_RING_STATE            0x02010107

#define OID_802_5_LINE_ERRORS                   0x02020101
#define OID_802_5_LOST_FRAMES                   0x02020102

#define OID_802_5_BURST_ERRORS                  0x02020201
#define OID_802_5_AC_ERRORS                     0x02020202
#define OID_802_5_ABORT_DELIMETERS              0x02020203
#define OID_802_5_FRAME_COPIED_ERRORS           0x02020204
#define OID_802_5_FREQUENCY_ERRORS              0x02020205
#define OID_802_5_TOKEN_ERRORS                  0x02020206
#define OID_802_5_INTERNAL_ERRORS               0x02020207


//
// FDDI Objects
//
#define OID_FDDI_LONG_PERMANENT_ADDR            0x03010101
#define OID_FDDI_LONG_CURRENT_ADDR              0x03010102
#define OID_FDDI_LONG_MULTICAST_LIST            0x03010103
#define OID_FDDI_LONG_MAX_LIST_SIZE             0x03010104
#define OID_FDDI_SHORT_PERMANENT_ADDR           0x03010105
#define OID_FDDI_SHORT_CURRENT_ADDR             0x03010106
#define OID_FDDI_SHORT_MULTICAST_LIST           0x03010107
#define OID_FDDI_SHORT_MAX_LIST_SIZE            0x03010108

#define OID_FDDI_ATTACHMENT_TYPE                0x03020101
#define OID_FDDI_UPSTREAM_NODE_LONG             0x03020102
#define OID_FDDI_DOWNSTREAM_NODE_LONG           0x03020103
#define OID_FDDI_FRAME_ERRORS                   0x03020104
#define OID_FDDI_FRAMES_LOST                    0x03020105
#define OID_FDDI_RING_MGT_STATE                 0x03020106
#define OID_FDDI_LCT_FAILURES                   0x03020107
#define OID_FDDI_LEM_REJECTS                    0x03020108
#define OID_FDDI_LCONNECTION_STATE              0x03020109

#define OID_FDDI_SMT_STATION_ID                 0x03030201
#define OID_FDDI_SMT_OP_VERSION_ID              0x03030202
#define OID_FDDI_SMT_HI_VERSION_ID              0x03030203
#define OID_FDDI_SMT_LO_VERSION_ID              0x03030204
#define OID_FDDI_SMT_MANUFACTURER_DATA          0x03030205
#define OID_FDDI_SMT_USER_DATA                  0x03030206
#define OID_FDDI_SMT_MIB_VERSION_ID             0x03030207
#define OID_FDDI_SMT_MAC_CT                     0x03030208
#define OID_FDDI_SMT_NON_MASTER_CT              0x03030209
#define OID_FDDI_SMT_MASTER_CT                  0x0303020A
#define OID_FDDI_SMT_AVAILABLE_PATHS            0x0303020B
#define OID_FDDI_SMT_CONFIG_CAPABILITIES        0x0303020C
#define OID_FDDI_SMT_CONFIG_POLICY              0x0303020D
#define OID_FDDI_SMT_CONNECTION_POLICY          0x0303020E
#define OID_FDDI_SMT_T_NOTIFY                   0x0303020F
#define OID_FDDI_SMT_STAT_RPT_POLICY            0x03030210
#define OID_FDDI_SMT_TRACE_MAX_EXPIRATION       0x03030211
#define OID_FDDI_SMT_PORT_INDEXES               0x03030212
#define OID_FDDI_SMT_MAC_INDEXES                0x03030213
#define OID_FDDI_SMT_BYPASS_PRESENT             0x03030214
#define OID_FDDI_SMT_ECM_STATE                  0x03030215
#define OID_FDDI_SMT_CF_STATE                   0x03030216
#define OID_FDDI_SMT_HOLD_STATE                 0x03030217
#define OID_FDDI_SMT_REMOTE_DISCONNECT_FLAG     0x03030218
#define OID_FDDI_SMT_STATION_STATUS             0x03030219
#define OID_FDDI_SMT_PEER_WRAP_FLAG             0x0303021A
#define OID_FDDI_SMT_MSG_TIME_STAMP             0x0303021B
#define OID_FDDI_SMT_TRANSITION_TIME_STAMP      0x0303021C
#define OID_FDDI_SMT_SET_COUNT                  0x0303021D
#define OID_FDDI_SMT_LAST_SET_STATION_ID        0x0303021E
#define OID_FDDI_MAC_FRAME_STATUS_FUNCTIONS     0x0303021F
#define OID_FDDI_MAC_BRIDGE_FUNCTIONS           0x03030220
#define OID_FDDI_MAC_T_MAX_CAPABILITY           0x03030221
#define OID_FDDI_MAC_TVX_CAPABILITY             0x03030222
#define OID_FDDI_MAC_AVAILABLE_PATHS            0x03030223
#define OID_FDDI_MAC_CURRENT_PATH               0x03030224
#define OID_FDDI_MAC_UPSTREAM_NBR               0x03030225
#define OID_FDDI_MAC_DOWNSTREAM_NBR             0x03030226
#define OID_FDDI_MAC_OLD_UPSTREAM_NBR           0x03030227
#define OID_FDDI_MAC_OLD_DOWNSTREAM_NBR         0x03030228
#define OID_FDDI_MAC_DUP_ADDRESS_TEST           0x03030229
#define OID_FDDI_MAC_REQUESTED_PATHS            0x0303022A
#define OID_FDDI_MAC_DOWNSTREAM_PORT_TYPE       0x0303022B
#define OID_FDDI_MAC_INDEX                      0x0303022C
#define OID_FDDI_MAC_SMT_ADDRESS                0x0303022D
#define OID_FDDI_MAC_LONG_GRP_ADDRESS           0x0303022E
#define OID_FDDI_MAC_SHORT_GRP_ADDRESS          0x0303022F
#define OID_FDDI_MAC_T_REQ                      0x03030230
#define OID_FDDI_MAC_T_NEG                      0x03030231
#define OID_FDDI_MAC_T_MAX                      0x03030232
#define OID_FDDI_MAC_TVX_VALUE                  0x03030233
#define OID_FDDI_MAC_T_PRI0                     0x03030234
#define OID_FDDI_MAC_T_PRI1                     0x03030235
#define OID_FDDI_MAC_T_PRI2                     0x03030236
#define OID_FDDI_MAC_T_PRI3                     0x03030237
#define OID_FDDI_MAC_T_PRI4                     0x03030238
#define OID_FDDI_MAC_T_PRI5                     0x03030239
#define OID_FDDI_MAC_T_PRI6                     0x0303023A
#define OID_FDDI_MAC_FRAME_CT                   0x0303023B
#define OID_FDDI_MAC_COPIED_CT                  0x0303023C
#define OID_FDDI_MAC_TRANSMIT_CT                0x0303023D
#define OID_FDDI_MAC_TOKEN_CT                   0x0303023E
#define OID_FDDI_MAC_ERROR_CT                   0x0303023F
#define OID_FDDI_MAC_LOST_CT                    0x03030240
#define OID_FDDI_MAC_TVX_EXPIRED_CT             0x03030241
#define OID_FDDI_MAC_NOT_COPIED_CT              0x03030242
#define OID_FDDI_MAC_LATE_CT                    0x03030243
#define OID_FDDI_MAC_RING_OP_CT                 0x03030244
#define OID_FDDI_MAC_FRAME_ERROR_THRESHOLD      0x03030245
#define OID_FDDI_MAC_FRAME_ERROR_RATIO          0x03030246
#define OID_FDDI_MAC_NOT_COPIED_THRESHOLD       0x03030247
#define OID_FDDI_MAC_NOT_COPIED_RATIO           0x03030248
#define OID_FDDI_MAC_RMT_STATE                  0x03030249
#define OID_FDDI_MAC_DA_FLAG                    0x0303024A
#define OID_FDDI_MAC_UNDA_FLAG                  0x0303024B
#define OID_FDDI_MAC_FRAME_ERROR_FLAG           0x0303024C
#define OID_FDDI_MAC_NOT_COPIED_FLAG            0x0303024D
#define OID_FDDI_MAC_MA_UNITDATA_AVAILABLE      0x0303024E
#define OID_FDDI_MAC_HARDWARE_PRESENT           0x0303024F
#define OID_FDDI_MAC_MA_UNITDATA_ENABLE         0x03030250
#define OID_FDDI_PATH_INDEX                     0x03030251
#define OID_FDDI_PATH_RING_LATENCY              0x03030252
#define OID_FDDI_PATH_TRACE_STATUS              0x03030253
#define OID_FDDI_PATH_SBA_PAYLOAD               0x03030254
#define OID_FDDI_PATH_SBA_OVERHEAD              0x03030255
#define OID_FDDI_PATH_CONFIGURATION             0x03030256
#define OID_FDDI_PATH_T_R_MODE                  0x03030257
#define OID_FDDI_PATH_SBA_AVAILABLE             0x03030258
#define OID_FDDI_PATH_TVX_LOWER_BOUND           0x03030259
#define OID_FDDI_PATH_T_MAX_LOWER_BOUND         0x0303025A
#define OID_FDDI_PATH_MAX_T_REQ                 0x0303025B
#define OID_FDDI_PORT_MY_TYPE                   0x0303025C
#define OID_FDDI_PORT_NEIGHBOR_TYPE             0x0303025D
#define OID_FDDI_PORT_CONNECTION_POLICIES       0x0303025E
#define OID_FDDI_PORT_MAC_INDICATED             0x0303025F
#define OID_FDDI_PORT_CURRENT_PATH              0x03030260
#define OID_FDDI_PORT_REQUESTED_PATHS           0x03030261
#define OID_FDDI_PORT_MAC_PLACEMENT             0x03030262
#define OID_FDDI_PORT_AVAILABLE_PATHS           0x03030263
#define OID_FDDI_PORT_MAC_LOOP_TIME             0x03030264
#define OID_FDDI_PORT_PMD_CLASS                 0x03030265
#define OID_FDDI_PORT_CONNECTION_CAPABILITIES   0x03030266
#define OID_FDDI_PORT_INDEX                     0x03030267
#define OID_FDDI_PORT_MAINT_LS                  0x03030268
#define OID_FDDI_PORT_BS_FLAG                   0x03030269
#define OID_FDDI_PORT_PC_LS                     0x0303026A
#define OID_FDDI_PORT_EB_ERROR_CT               0x0303026B
#define OID_FDDI_PORT_LCT_FAIL_CT               0x0303026C
#define OID_FDDI_PORT_LER_ESTIMATE              0x0303026D
#define OID_FDDI_PORT_LEM_REJECT_CT             0x0303026E
#define OID_FDDI_PORT_LEM_CT                    0x0303026F
#define OID_FDDI_PORT_LER_CUTOFF                0x03030270
#define OID_FDDI_PORT_LER_ALARM                 0x03030271
#define OID_FDDI_PORT_CONNNECT_STATE            0x03030272
#define OID_FDDI_PORT_PCM_STATE                 0x03030273
#define OID_FDDI_PORT_PC_WITHHOLD               0x03030274
#define OID_FDDI_PORT_LER_FLAG                  0x03030275
#define OID_FDDI_PORT_HARDWARE_PRESENT          0x03030276
#define OID_FDDI_SMT_STATION_ACTION             0x03030277
#define OID_FDDI_PORT_ACTION                    0x03030278
#define OID_FDDI_IF_DESCR                       0x03030279
#define OID_FDDI_IF_TYPE                        0x0303027A
#define OID_FDDI_IF_MTU                         0x0303027B
#define OID_FDDI_IF_SPEED                       0x0303027C
#define OID_FDDI_IF_PHYS_ADDRESS                0x0303027D
#define OID_FDDI_IF_ADMIN_STATUS                0x0303027E
#define OID_FDDI_IF_OPER_STATUS                 0x0303027F
#define OID_FDDI_IF_LAST_CHANGE                 0x03030280
#define OID_FDDI_IF_IN_OCTETS                   0x03030281
#define OID_FDDI_IF_IN_UCAST_PKTS               0x03030282
#define OID_FDDI_IF_IN_NUCAST_PKTS              0x03030283
#define OID_FDDI_IF_IN_DISCARDS                 0x03030284
#define OID_FDDI_IF_IN_ERRORS                   0x03030285
#define OID_FDDI_IF_IN_UNKNOWN_PROTOS           0x03030286
#define OID_FDDI_IF_OUT_OCTETS                  0x03030287
#define OID_FDDI_IF_OUT_UCAST_PKTS              0x03030288
#define OID_FDDI_IF_OUT_NUCAST_PKTS             0x03030289
#define OID_FDDI_IF_OUT_DISCARDS                0x0303028A
#define OID_FDDI_IF_OUT_ERRORS                  0x0303028B
#define OID_FDDI_IF_OUT_QLEN                    0x0303028C
#define OID_FDDI_IF_SPECIFIC                    0x0303028D

//
// WAN objects
//
#define OID_WAN_PERMANENT_ADDRESS               0x04010101
#define OID_WAN_CURRENT_ADDRESS                 0x04010102
#define OID_WAN_QUALITY_OF_SERVICE              0x04010103
#define OID_WAN_PROTOCOL_TYPE                   0x04010104
#define OID_WAN_MEDIUM_SUBTYPE                  0x04010105
#define OID_WAN_HEADER_FORMAT                   0x04010106

#define OID_WAN_GET_INFO                        0x04010107
#define OID_WAN_SET_LINK_INFO                   0x04010108
#define OID_WAN_GET_LINK_INFO                   0x04010109

#define OID_WAN_LINE_COUNT                      0x0401010A
#define OID_WAN_PROTOCOL_CAPS                   0x0401010B

#define OID_WAN_GET_BRIDGE_INFO                 0x0401020A
#define OID_WAN_SET_BRIDGE_INFO                 0x0401020B
#define OID_WAN_GET_COMP_INFO                   0x0401020C
#define OID_WAN_SET_COMP_INFO                   0x0401020D
#define OID_WAN_GET_STATS_INFO                  0x0401020E

//
//  These are connection-oriented WAN OIDs.
//  These replace the above OIDs for CoNDIS WAN Miniports
//
#define OID_WAN_CO_GET_INFO                     0x04010180
#define OID_WAN_CO_SET_LINK_INFO                0x04010181
#define OID_WAN_CO_GET_LINK_INFO                0x04010182
#define OID_WAN_CO_GET_COMP_INFO                0x04010280
#define OID_WAN_CO_SET_COMP_INFO                0x04010281
#define OID_WAN_CO_GET_STATS_INFO               0x04010282


//
// LocalTalk objects
//
#define OID_LTALK_CURRENT_NODE_ID               0x05010102

#define OID_LTALK_IN_BROADCASTS                 0x05020101
#define OID_LTALK_IN_LENGTH_ERRORS              0x05020102

#define OID_LTALK_OUT_NO_HANDLERS               0x05020201
#define OID_LTALK_COLLISIONS                    0x05020202
#define OID_LTALK_DEFERS                        0x05020203
#define OID_LTALK_NO_DATA_ERRORS                0x05020204
#define OID_LTALK_RANDOM_CTS_ERRORS             0x05020205
#define OID_LTALK_FCS_ERRORS                    0x05020206

//
// Arcnet objects
//
#define OID_ARCNET_PERMANENT_ADDRESS            0x06010101
#define OID_ARCNET_CURRENT_ADDRESS              0x06010102

#define OID_ARCNET_RECONFIGURATIONS             0x06020201


//
// TAPI objects
//
#define OID_TAPI_ACCEPT                         0x07030101
#define OID_TAPI_ANSWER                         0x07030102
#define OID_TAPI_CLOSE                          0x07030103
#define OID_TAPI_CLOSE_CALL                     0x07030104
#define OID_TAPI_CONDITIONAL_MEDIA_DETECTION    0x07030105
#define OID_TAPI_CONFIG_DIALOG                  0x07030106
#define OID_TAPI_DEV_SPECIFIC                   0x07030107
#define OID_TAPI_DIAL                           0x07030108
#define OID_TAPI_DROP                           0x07030109
#define OID_TAPI_GET_ADDRESS_CAPS               0x0703010A
#define OID_TAPI_GET_ADDRESS_ID                 0x0703010B
#define OID_TAPI_GET_ADDRESS_STATUS             0x0703010C
#define OID_TAPI_GET_CALL_ADDRESS_ID            0x0703010D
#define OID_TAPI_GET_CALL_INFO                  0x0703010E
#define OID_TAPI_GET_CALL_STATUS                0x0703010F
#define OID_TAPI_GET_DEV_CAPS                   0x07030110
#define OID_TAPI_GET_DEV_CONFIG                 0x07030111
#define OID_TAPI_GET_EXTENSION_ID               0x07030112
#define OID_TAPI_GET_ID                         0x07030113
#define OID_TAPI_GET_LINE_DEV_STATUS            0x07030114
#define OID_TAPI_MAKE_CALL                      0x07030115
#define OID_TAPI_NEGOTIATE_EXT_VERSION          0x07030116
#define OID_TAPI_OPEN                           0x07030117
#define OID_TAPI_PROVIDER_INITIALIZE            0x07030118
#define OID_TAPI_PROVIDER_SHUTDOWN              0x07030119
#define OID_TAPI_SECURE_CALL                    0x0703011A
#define OID_TAPI_SELECT_EXT_VERSION             0x0703011B
#define OID_TAPI_SEND_USER_USER_INFO            0x0703011C
#define OID_TAPI_SET_APP_SPECIFIC               0x0703011D
#define OID_TAPI_SET_CALL_PARAMS                0x0703011E
#define OID_TAPI_SET_DEFAULT_MEDIA_DETECTION    0x0703011F
#define OID_TAPI_SET_DEV_CONFIG                 0x07030120
#define OID_TAPI_SET_MEDIA_MODE                 0x07030121
#define OID_TAPI_SET_STATUS_MESSAGES            0x07030122
#define OID_TAPI_GATHER_DIGITS                  0x07030123
#define OID_TAPI_MONITOR_DIGITS                 0x07030124

//
// ATM Connection Oriented OIDs
//
#define OID_ATM_SUPPORTED_VC_RATES              0x08010101
#define OID_ATM_SUPPORTED_SERVICE_CATEGORY      0x08010102
#define OID_ATM_SUPPORTED_AAL_TYPES             0x08010103
#define OID_ATM_HW_CURRENT_ADDRESS              0x08010104
#define OID_ATM_MAX_ACTIVE_VCS                  0x08010105
#define OID_ATM_MAX_ACTIVE_VCI_BITS             0x08010106
#define OID_ATM_MAX_ACTIVE_VPI_BITS             0x08010107
#define OID_ATM_MAX_AAL0_PACKET_SIZE            0x08010108
#define OID_ATM_MAX_AAL1_PACKET_SIZE            0x08010109
#define OID_ATM_MAX_AAL34_PACKET_SIZE           0x0801010A
#define OID_ATM_MAX_AAL5_PACKET_SIZE            0x0801010B

#define OID_ATM_SIGNALING_VPIVCI                0x08010201
#define OID_ATM_ASSIGNED_VPI                    0x08010202
#define OID_ATM_ACQUIRE_ACCESS_NET_RESOURCES    0x08010203
#define OID_ATM_RELEASE_ACCESS_NET_RESOURCES    0x08010204
#define OID_ATM_ILMI_VPIVCI                     0x08010205
#define OID_ATM_DIGITAL_BROADCAST_VPIVCI        0x08010206
#define OID_ATM_GET_NEAREST_FLOW                0x08010207
#define OID_ATM_ALIGNMENT_REQUIRED              0x08010208
#define OID_ATM_LECS_ADDRESS                    0x08010209
#define OID_ATM_SERVICE_ADDRESS                 0x0801020A

#define OID_ATM_CALL_PROCEEDING                 0x0801020B  // UNI 4.0
#define OID_ATM_CALL_ALERTING                   0x0801020C  // UNI 4.0
#define OID_ATM_PARTY_ALERTING                  0x0801020D  // UNI 4.0
#define OID_ATM_CALL_NOTIFY                     0x0801020E  // UNI 4.0

#define OID_ATM_MY_IP_NM_ADDRESS                0x0801020F


//
//  ATM specific statistics OIDs.
//
#define OID_ATM_RCV_CELLS_OK                    0x08020101
#define OID_ATM_XMIT_CELLS_OK                   0x08020102
#define OID_ATM_RCV_CELLS_DROPPED               0x08020103

#define OID_ATM_RCV_INVALID_VPI_VCI             0x08020201
#define OID_ATM_CELLS_HEC_ERROR                 0x08020202
#define OID_ATM_RCV_REASSEMBLY_ERROR            0x08020203

//
// PCCA (Wireless) object
//

//
// All WirelessWAN devices must support the following OIDs
//

#define OID_WW_GEN_NETWORK_TYPES_SUPPORTED      0x09010101
#define OID_WW_GEN_NETWORK_TYPE_IN_USE          0x09010102
#define OID_WW_GEN_HEADER_FORMATS_SUPPORTED     0x09010103
#define OID_WW_GEN_HEADER_FORMAT_IN_USE         0x09010104
#define OID_WW_GEN_INDICATION_REQUEST           0x09010105
#define OID_WW_GEN_DEVICE_INFO                  0x09010106
#define OID_WW_GEN_OPERATION_MODE               0x09010107
#define OID_WW_GEN_LOCK_STATUS                  0x09010108
#define OID_WW_GEN_DISABLE_TRANSMITTER          0x09010109
#define OID_WW_GEN_NETWORK_ID                   0x0901010A
#define OID_WW_GEN_PERMANENT_ADDRESS            0x0901010B
#define OID_WW_GEN_CURRENT_ADDRESS              0x0901010C
#define OID_WW_GEN_SUSPEND_DRIVER               0x0901010D
#define OID_WW_GEN_BASESTATION_ID               0x0901010E
#define OID_WW_GEN_CHANNEL_ID                   0x0901010F
#define OID_WW_GEN_ENCRYPTION_SUPPORTED         0x09010110
#define OID_WW_GEN_ENCRYPTION_IN_USE            0x09010111
#define OID_WW_GEN_ENCRYPTION_STATE             0x09010112
#define OID_WW_GEN_CHANNEL_QUALITY              0x09010113
#define OID_WW_GEN_REGISTRATION_STATUS          0x09010114
#define OID_WW_GEN_RADIO_LINK_SPEED             0x09010115
#define OID_WW_GEN_LATENCY                      0x09010116
#define OID_WW_GEN_BATTERY_LEVEL                0x09010117
#define OID_WW_GEN_EXTERNAL_POWER               0x09010118

//
// These are optional
//
#define OID_WW_GEN_PING_ADDRESS                 0x09010201
#define OID_WW_GEN_RSSI                         0x09010202
#define OID_WW_GEN_SIM_STATUS                   0x09010203
#define OID_WW_GEN_ENABLE_SIM_PIN               0x09010204
#define OID_WW_GEN_CHANGE_SIM_PIN               0x09010205
#define OID_WW_GEN_SIM_PUK                      0x09010206
#define OID_WW_GEN_SIM_EXCEPTION                0x09010207

//
// Network Dependent OIDs - Mobitex:
//

#define OID_WW_MBX_SUBADDR                      0x09050101
// OID 0x09050102 is reserved and may not be used
#define OID_WW_MBX_FLEXLIST                     0x09050103
#define OID_WW_MBX_GROUPLIST                    0x09050104
#define OID_WW_MBX_TRAFFIC_AREA                 0x09050105
#define OID_WW_MBX_LIVE_DIE                     0x09050106
#define OID_WW_MBX_TEMP_DEFAULTLIST             0x09050107

//
// Network Dependent OIDs - Pinpoint:
//
//
// The following Pin Point characteristics have been deprecated by the
// PCCA and are considered reserved values. They are include here only for
// historical purposes and should not be used
//
#define OID_WW_PIN_LOC_AUTHORIZE                0x09090101
#define OID_WW_PIN_LAST_LOCATION                0x09090102
#define OID_WW_PIN_LOC_FIX                      0x09090103

//
// Network Dependent - CDPD:
//
#define OID_WW_CDPD_SPNI                        0x090D0101
#define OID_WW_CDPD_WASI                        0x090D0102
#define OID_WW_CDPD_AREA_COLOR                  0x090D0103
#define OID_WW_CDPD_TX_POWER_LEVEL              0x090D0104
#define OID_WW_CDPD_EID                         0x090D0105
#define OID_WW_CDPD_HEADER_COMPRESSION          0x090D0106
#define OID_WW_CDPD_DATA_COMPRESSION            0x090D0107
#define OID_WW_CDPD_CHANNEL_SELECT              0x090D0108
#define OID_WW_CDPD_CHANNEL_STATE               0x090D0109
#define OID_WW_CDPD_NEI                         0x090D010A
#define OID_WW_CDPD_NEI_STATE                   0x090D010B
#define OID_WW_CDPD_SERVICE_PROVIDER_IDENTIFIER 0x090D010C
#define OID_WW_CDPD_SLEEP_MODE                  0x090D010D

//
// At the request of the PCCA STD-201 Annex C working group the following OID
// value has been superceeded by more specific objects. Its value is reserved by
// the PCCA,is included here for historical purposes only, and should not be
// used.
//
#define OID_WW_CDPD_CIRCUIT_SWITCHED            0x090D010E

#define OID_WW_CDPD_TEI                         0x090D010F
#define OID_WW_CDPD_RSSI                        0x090D0110

//
// CDPD Circuit Switched objects
//
#define OID_WW_CDPD_CS_SERVICE_PREFERENCE       0x090D0111
#define OID_WW_CDPD_CS_SERVICE_STATUS           0x090D0112
#define OID_WW_CDPD_CS_INFO                     0x090D0113
#define OID_WW_CDPD_CS_SUSPEND                  0x090D0114
#define OID_WW_CDPD_CS_DEFAULT_DIAL_CODE        0x090D0115
#define OID_WW_CDPD_CS_CALLBACK                 0x090D0116
#define OID_WW_CDPD_CS_SID_LIST                 0x090D0117
#define OID_WW_CDPD_CS_CONFIGURATION            0x090D0118

//
// Network Dependent - Ardis:
//
//
// At the request of Ardis these OID value have been superceeded. Their
// functionality has been merged with the DataTAC objects. Therefore
// these values are reserved by the PCCA, are include here for
// historical purposes only, and should not be used.
//
#define OID_WW_ARD_SNDCP                        0x09110101
#define OID_WW_ARD_TMLY_MSG                     0x09110102
#define OID_WW_ARD_DATAGRAM                     0x09110103

//
// Network Dependent - DataTac:
//

#define OID_WW_TAC_COMPRESSION                  0x09150101

//
// At the request of Motorola, the following two OID values have been
// superceeded. Their functionality has been subsumed by other more specific
// DataTac objects and should not be used. These values are reserved by the
// PCCA and are include here only for historical purposes only.
//
#define OID_WW_TAC_SET_CONFIG                   0x09150102
#define OID_WW_TAC_GET_STATUS                   0x09150103
#define OID_WW_TAC_USER_HEADER                  0x09150104

//
// DataTAC characteristic object values
//
#define OID_WW_TAC_UNIQUE_SDU_TAG               0x09150105
#define OID_WW_TAC_SEND_COMMAND                 0x09150106
#define OID_WW_TAC_GET_RESPONSE                 0x09150107
#define OID_WW_TAC_DISABLE_RECEIVER             0x09150108
#define OID_WW_TAC_ANTENNA_MODE                 0x09150109
#define OID_WW_TAC_FLUSH_DATA                   0x0915010A
#define OID_WW_TAC_SHUTDOWN_DEVICE              0x0915010B
#define OID_WW_TAC_DEVICE_EXCEPTION             0x0915010C
#define OID_WW_TAC_RECEIVE_EXCEPTION            0x0915010D
#define OID_WW_TAC_BUFFER_EXCEPTION             0x0915010E
#define OID_WW_TAC_BATTERY_EXCEPTION            0x0915010F
#define OID_WW_TAC_TRANSMITTER_KEYED            0x09150110
#define OID_WW_TAC_SYSTEM_TABLE                 0x09150111
#define OID_WW_TAC_CHANNEL_TABLE                0x09150112
#define OID_WW_TAC_DCHANNEL_TABLE               0x09150113
#define OID_WW_TAC_RECEIVE_QUEUE_COUNT          0x09150114

//
// DataTac statistic object value
//
#define OID_WW_TAC_STATISTICS                   0x09160101

//
// Network Dependent - Metricom:
//

#define OID_WW_MET_FUNCTION                     0x09190101


//
// IEEE 802.11 OIDs
//
#define OID_802_11_BSSID                        0x0D010101
#define OID_802_11_SSID                         0x0D010102
#define OID_802_11_NETWORK_TYPES_SUPPORTED      0x0D010203
#define OID_802_11_NETWORK_TYPE_IN_USE          0x0D010204
#define OID_802_11_TX_POWER_LEVEL               0x0D010205
#define OID_802_11_RSSI                         0x0D010206
#define OID_802_11_RSSI_TRIGGER                 0x0D010207
#define OID_802_11_INFRASTRUCTURE_MODE          0x0D010108
#define OID_802_11_FRAGMENTATION_THRESHOLD      0x0D010209
#define OID_802_11_RTS_THRESHOLD                0x0D01020A
#define OID_802_11_NUMBER_OF_ANTENNAS           0x0D01020B
#define OID_802_11_RX_ANTENNA_SELECTED          0x0D01020C
#define OID_802_11_TX_ANTENNA_SELECTED          0x0D01020D
#define OID_802_11_SUPPORTED_RATES              0x0D01020E
#define OID_802_11_DESIRED_RATES                0x0D010210
#define OID_802_11_CONFIGURATION                0x0D010211
#define OID_802_11_STATISTICS                   0x0D020212
#define OID_802_11_ADD_WEP                      0x0D010113
#define OID_802_11_REMOVE_WEP                   0x0D010114
#define OID_802_11_DISASSOCIATE                 0x0D010115
#define OID_802_11_POWER_MODE                   0x0D010216
#define OID_802_11_BSSID_LIST                   0x0D010217
#define OID_802_11_AUTHENTICATION_MODE          0x0D010118
#define OID_802_11_PRIVACY_FILTER               0x0D010119
#define OID_802_11_BSSID_LIST_SCAN              0x0D01011A
#define OID_802_11_WEP_STATUS                   0x0D01011B
// Renamed to reflect better the extended set of encryption status
#define OID_802_11_ENCRYPTION_STATUS            OID_802_11_WEP_STATUS
#define OID_802_11_RELOAD_DEFAULTS              0x0D01011C
// Added to allow key mapping and default keys
#define OID_802_11_ADD_KEY                      0x0D01011D
#define OID_802_11_REMOVE_KEY                   0x0D01011E
#define OID_802_11_ASSOCIATION_INFORMATION      0x0D01011F
#define OID_802_11_TEST                         0x0D010120
#define OID_802_11_MEDIA_STREAM_MODE            0x0D010121

//
// IEEE 802.11 Structures and definitions
//
// new types for Media Specific Indications

#define NDIS_802_11_LENGTH_SSID         32
#define NDIS_802_11_LENGTH_RATES        8
#define NDIS_802_11_LENGTH_RATES_EX     16

typedef enum _NDIS_802_11_STATUS_TYPE
{
    Ndis802_11StatusType_Authentication,
    Ndis802_11StatusType_MediaStreamMode,
    Ndis802_11StatusTypeMax    // not a real type, defined as an upper bound
} NDIS_802_11_STATUS_TYPE, *PNDIS_802_11_STATUS_TYPE;

typedef UCHAR   NDIS_802_11_MAC_ADDRESS[6];

typedef struct _NDIS_802_11_STATUS_INDICATION
{
    NDIS_802_11_STATUS_TYPE StatusType;
} NDIS_802_11_STATUS_INDICATION, *PNDIS_802_11_STATUS_INDICATION;

// mask for authentication/integrity fields
#define NDIS_802_11_AUTH_REQUEST_AUTH_FIELDS        0x0f

#define NDIS_802_11_AUTH_REQUEST_REAUTH             0x01
#define NDIS_802_11_AUTH_REQUEST_KEYUPDATE          0x02
#define NDIS_802_11_AUTH_REQUEST_PAIRWISE_ERROR     0x06
#define NDIS_802_11_AUTH_REQUEST_GROUP_ERROR        0x0E

typedef struct _NDIS_802_11_AUTHENTICATION_REQUEST
{
    ULONG Length;            // Length of structure
    NDIS_802_11_MAC_ADDRESS Bssid;
    ULONG Flags;
} NDIS_802_11_AUTHENTICATION_REQUEST, *PNDIS_802_11_AUTHENTICATION_REQUEST;

// Added new types for OFDM 5G and 2.4G
typedef enum _NDIS_802_11_NETWORK_TYPE
{
    Ndis802_11FH,
    Ndis802_11DS,
    Ndis802_11OFDM5,
    Ndis802_11OFDM24,
    Ndis802_11NetworkTypeMax    // not a real type, defined as an upper bound
} NDIS_802_11_NETWORK_TYPE, *PNDIS_802_11_NETWORK_TYPE;

typedef struct _NDIS_802_11_NETWORK_TYPE_LIST
{
    ULONG                       NumberOfItems;  // in list below, at least 1
    NDIS_802_11_NETWORK_TYPE    NetworkType [1];
} NDIS_802_11_NETWORK_TYPE_LIST, *PNDIS_802_11_NETWORK_TYPE_LIST;

typedef enum _NDIS_802_11_POWER_MODE
{
    Ndis802_11PowerModeCAM,
    Ndis802_11PowerModeMAX_PSP,
    Ndis802_11PowerModeFast_PSP,
    Ndis802_11PowerModeMax      // not a real mode, defined as an upper bound
} NDIS_802_11_POWER_MODE, *PNDIS_802_11_POWER_MODE;

typedef ULONG   NDIS_802_11_TX_POWER_LEVEL; // in milliwatts

//
// Received Signal Strength Indication
//
typedef LONG   NDIS_802_11_RSSI;           // in dBm

typedef struct _NDIS_802_11_CONFIGURATION_FH
{
    ULONG           Length;             // Length of structure
    ULONG           HopPattern;         // As defined by 802.11, MSB set
    ULONG           HopSet;             // to one if non-802.11
    ULONG           DwellTime;          // units are Kusec
} NDIS_802_11_CONFIGURATION_FH, *PNDIS_802_11_CONFIGURATION_FH;

typedef struct _NDIS_802_11_CONFIGURATION
{
    ULONG           Length;             // Length of structure
    ULONG           BeaconPeriod;       // units are Kusec
    ULONG           ATIMWindow;         // units are Kusec
    ULONG           DSConfig;           // Frequency, units are kHz
    NDIS_802_11_CONFIGURATION_FH    FHConfig;
} NDIS_802_11_CONFIGURATION, *PNDIS_802_11_CONFIGURATION;

typedef struct _NDIS_802_11_STATISTICS
{
    ULONG           Length;             // Length of structure
    LARGE_INTEGER   TransmittedFragmentCount;
    LARGE_INTEGER   MulticastTransmittedFrameCount;
    LARGE_INTEGER   FailedCount;
    LARGE_INTEGER   RetryCount;
    LARGE_INTEGER   MultipleRetryCount;
    LARGE_INTEGER   RTSSuccessCount;
    LARGE_INTEGER   RTSFailureCount;
    LARGE_INTEGER   ACKFailureCount;
    LARGE_INTEGER   FrameDuplicateCount;
    LARGE_INTEGER   ReceivedFragmentCount;
    LARGE_INTEGER   MulticastReceivedFrameCount;
    LARGE_INTEGER   FCSErrorCount;
} NDIS_802_11_STATISTICS, *PNDIS_802_11_STATISTICS;

typedef  ULONG  NDIS_802_11_KEY_INDEX;
typedef ULONGLONG   NDIS_802_11_KEY_RSC;

// Key mapping keys require a BSSID
typedef struct _NDIS_802_11_KEY
{
    ULONG           Length;             // Length of this structure
    ULONG           KeyIndex;           
    ULONG           KeyLength;          // length of key in bytes
    NDIS_802_11_MAC_ADDRESS BSSID;
    NDIS_802_11_KEY_RSC KeyRSC;
    UCHAR           KeyMaterial[1];     // variable length depending on above field
} NDIS_802_11_KEY, *PNDIS_802_11_KEY;

typedef struct _NDIS_802_11_REMOVE_KEY
{
    ULONG           Length;             // Length of this structure
    ULONG           KeyIndex;           
    NDIS_802_11_MAC_ADDRESS BSSID;      
} NDIS_802_11_REMOVE_KEY, *PNDIS_802_11_REMOVE_KEY;

typedef struct _NDIS_802_11_WEP
{
    ULONG           Length;             // Length of this structure
    ULONG           KeyIndex;           // 0 is the per-client key, 1-N are the
                                        // global keys
    ULONG           KeyLength;          // length of key in bytes
    UCHAR           KeyMaterial[1];     // variable length depending on above field
} NDIS_802_11_WEP, *PNDIS_802_11_WEP;


typedef enum _NDIS_802_11_NETWORK_INFRASTRUCTURE
{
    Ndis802_11IBSS,
    Ndis802_11Infrastructure,
    Ndis802_11AutoUnknown,
    Ndis802_11InfrastructureMax         // Not a real value, defined as upper bound
} NDIS_802_11_NETWORK_INFRASTRUCTURE, *PNDIS_802_11_NETWORK_INFRASTRUCTURE;

// Add new authentication modes
typedef enum _NDIS_802_11_AUTHENTICATION_MODE
{
    Ndis802_11AuthModeOpen,
    Ndis802_11AuthModeShared,
    Ndis802_11AuthModeAutoSwitch,
    Ndis802_11AuthModeWPA,
    Ndis802_11AuthModeWPAPSK,
    Ndis802_11AuthModeWPANone,
    Ndis802_11AuthModeMax               // Not a real mode, defined as upper bound
} NDIS_802_11_AUTHENTICATION_MODE, *PNDIS_802_11_AUTHENTICATION_MODE;

typedef UCHAR   NDIS_802_11_RATES[NDIS_802_11_LENGTH_RATES];        // Set of 8 data rates
typedef UCHAR   NDIS_802_11_RATES_EX[NDIS_802_11_LENGTH_RATES_EX];  // Set of 16 data rates

typedef struct _NDIS_802_11_SSID
{
    ULONG   SsidLength;         // length of SSID field below, in bytes;
                                // this can be zero.
    UCHAR   Ssid[NDIS_802_11_LENGTH_SSID];           // SSID information field
} NDIS_802_11_SSID, *PNDIS_802_11_SSID;


typedef struct _NDIS_WLAN_BSSID
{
    ULONG                               Length;             // Length of this structure
    NDIS_802_11_MAC_ADDRESS             MacAddress;         // BSSID
    UCHAR                               Reserved[2];
    NDIS_802_11_SSID                    Ssid;               // SSID
    ULONG                               Privacy;            // WEP encryption requirement
    NDIS_802_11_RSSI                    Rssi;               // receive signal
                                                            // strength in dBm
    NDIS_802_11_NETWORK_TYPE            NetworkTypeInUse;
    NDIS_802_11_CONFIGURATION           Configuration;
    NDIS_802_11_NETWORK_INFRASTRUCTURE  InfrastructureMode;
    NDIS_802_11_RATES                   SupportedRates;
} NDIS_WLAN_BSSID, *PNDIS_WLAN_BSSID;

typedef struct _NDIS_802_11_BSSID_LIST
{
    ULONG           NumberOfItems;      // in list below, at least 1
    NDIS_WLAN_BSSID Bssid[1];
} NDIS_802_11_BSSID_LIST, *PNDIS_802_11_BSSID_LIST;

// Added Capabilities, IELength and IEs for each BSSID
typedef struct _NDIS_WLAN_BSSID_EX
{
    ULONG                               Length;             // Length of this structure
    NDIS_802_11_MAC_ADDRESS             MacAddress;         // BSSID
    UCHAR                               Reserved[2];
    NDIS_802_11_SSID                    Ssid;               // SSID
    ULONG                               Privacy;            // WEP encryption requirement
    NDIS_802_11_RSSI                    Rssi;               // receive signal
                                                            // strength in dBm
    NDIS_802_11_NETWORK_TYPE            NetworkTypeInUse;
    NDIS_802_11_CONFIGURATION           Configuration;
    NDIS_802_11_NETWORK_INFRASTRUCTURE  InfrastructureMode;
    NDIS_802_11_RATES_EX                SupportedRates;
    ULONG                               IELength;
    UCHAR                               IEs[1];
} NDIS_WLAN_BSSID_EX, *PNDIS_WLAN_BSSID_EX;

typedef struct _NDIS_802_11_BSSID_LIST_EX
{
    ULONG                   NumberOfItems;      // in list below, at least 1
    NDIS_WLAN_BSSID_EX      Bssid[1];
} NDIS_802_11_BSSID_LIST_EX, *PNDIS_802_11_BSSID_LIST_EX;

typedef struct _NDIS_802_11_FIXED_IEs 
{
    UCHAR Timestamp[8];
    USHORT BeaconInterval;
    USHORT Capabilities;
} NDIS_802_11_FIXED_IEs, *PNDIS_802_11_FIXED_IEs;

typedef struct _NDIS_802_11_VARIABLE_IEs 
{
    UCHAR ElementID;
    UCHAR Length;    // Number of bytes in data field
    UCHAR data[1];
} NDIS_802_11_VARIABLE_IEs, *PNDIS_802_11_VARIABLE_IEs;

typedef  ULONG   NDIS_802_11_FRAGMENTATION_THRESHOLD;

typedef  ULONG   NDIS_802_11_RTS_THRESHOLD;

typedef  ULONG   NDIS_802_11_ANTENNA;

typedef enum _NDIS_802_11_PRIVACY_FILTER
{
    Ndis802_11PrivFilterAcceptAll,
    Ndis802_11PrivFilter8021xWEP
} NDIS_802_11_PRIVACY_FILTER, *PNDIS_802_11_PRIVACY_FILTER;

// Added new encryption types
// Also aliased typedef to new name
typedef enum _NDIS_802_11_WEP_STATUS
{
    Ndis802_11WEPEnabled,
    Ndis802_11Encryption1Enabled = Ndis802_11WEPEnabled,
    Ndis802_11WEPDisabled,
    Ndis802_11EncryptionDisabled = Ndis802_11WEPDisabled,
    Ndis802_11WEPKeyAbsent,
    Ndis802_11Encryption1KeyAbsent = Ndis802_11WEPKeyAbsent,
    Ndis802_11WEPNotSupported,
    Ndis802_11EncryptionNotSupported = Ndis802_11WEPNotSupported,
    Ndis802_11Encryption2Enabled,
    Ndis802_11Encryption2KeyAbsent,
    Ndis802_11Encryption3Enabled,
    Ndis802_11Encryption3KeyAbsent
} NDIS_802_11_WEP_STATUS, *PNDIS_802_11_WEP_STATUS,
  NDIS_802_11_ENCRYPTION_STATUS, *PNDIS_802_11_ENCRYPTION_STATUS;
    
typedef enum _NDIS_802_11_RELOAD_DEFAULTS
{
    Ndis802_11ReloadWEPKeys
} NDIS_802_11_RELOAD_DEFAULTS, *PNDIS_802_11_RELOAD_DEFAULTS;

#define NDIS_802_11_AI_REQFI_CAPABILITIES      1
#define NDIS_802_11_AI_REQFI_LISTENINTERVAL    2
#define NDIS_802_11_AI_REQFI_CURRENTAPADDRESS  4

#define NDIS_802_11_AI_RESFI_CAPABILITIES      1
#define NDIS_802_11_AI_RESFI_STATUSCODE        2
#define NDIS_802_11_AI_RESFI_ASSOCIATIONID     4

typedef struct _NDIS_802_11_AI_REQFI
{
    USHORT Capabilities;
    USHORT ListenInterval;
    NDIS_802_11_MAC_ADDRESS  CurrentAPAddress;
} NDIS_802_11_AI_REQFI, *PNDIS_802_11_AI_REQFI;

typedef struct _NDIS_802_11_AI_RESFI
{
    USHORT Capabilities;
    USHORT StatusCode;
    USHORT AssociationId;
} NDIS_802_11_AI_RESFI, *PNDIS_802_11_AI_RESFI;

typedef struct _NDIS_802_11_ASSOCIATION_INFORMATION
{
    ULONG                   Length;
    USHORT                  AvailableRequestFixedIEs;
    NDIS_802_11_AI_REQFI    RequestFixedIEs;
    ULONG                   RequestIELength;
    ULONG                   OffsetRequestIEs;
    USHORT                  AvailableResponseFixedIEs;
    NDIS_802_11_AI_RESFI    ResponseFixedIEs;
    ULONG                   ResponseIELength;
    ULONG                   OffsetResponseIEs;
} NDIS_802_11_ASSOCIATION_INFORMATION, *PNDIS_802_11_ASSOCIATION_INFORMATION;

typedef struct _NDIS_802_11_AUTHENTICATION_EVENT
{
    NDIS_802_11_STATUS_INDICATION       Status;
    NDIS_802_11_AUTHENTICATION_REQUEST  Request[1];
} NDIS_802_11_AUTHENTICATION_EVENT, *PNDIS_802_11_AUTHENTICATION_EVENT;
        
typedef struct _NDIS_802_11_TEST
{
    ULONG Length;
    ULONG Type;
    union
    {
        NDIS_802_11_AUTHENTICATION_EVENT AuthenticationEvent;
        NDIS_802_11_RSSI RssiTrigger;
    };
} NDIS_802_11_TEST, *PNDIS_802_11_TEST;

// 802.11 Media stream constraints, associated with OID_802_11_MEDIA_STREAM_MODE
typedef enum _NDIS_802_11_MEDIA_STREAM_MODE
{
    Ndis802_11MediaStreamOff,
    Ndis802_11MediaStreamOn,
} NDIS_802_11_MEDIA_STREAM_MODE, *PNDIS_802_11_MEDIA_STREAM_MODE;

//
// IRDA objects
//
#define OID_IRDA_RECEIVING                      0x0A010100
#define OID_IRDA_TURNAROUND_TIME                0x0A010101
#define OID_IRDA_SUPPORTED_SPEEDS               0x0A010102
#define OID_IRDA_LINK_SPEED                     0x0A010103
#define OID_IRDA_MEDIA_BUSY                     0x0A010104

#define OID_IRDA_EXTRA_RCV_BOFS                 0x0A010200
#define OID_IRDA_RATE_SNIFF                     0x0A010201
#define OID_IRDA_UNICAST_LIST                   0x0A010202
#define OID_IRDA_MAX_UNICAST_LIST_SIZE          0x0A010203
#define OID_IRDA_MAX_RECEIVE_WINDOW_SIZE        0x0A010204
#define OID_IRDA_MAX_SEND_WINDOW_SIZE           0x0A010205
#define OID_IRDA_RESERVED1                      0x0A01020A  // The range between OID_IRDA_RESERVED1
#define OID_IRDA_RESERVED2                      0x0A01020F  // and OID_IRDA_RESERVED2 is reserved

//
// BPC OIDs
//
#define OID_BPC_ADAPTER_CAPS                    0x0B010100
#define OID_BPC_DEVICES                         0x0B010101
#define OID_BPC_DEVICE_CAPS                     0x0B010102
#define OID_BPC_DEVICE_SETTINGS                 0x0B010103
#define OID_BPC_CONNECTION_STATUS               0x0B010104
#define OID_BPC_ADDRESS_COMPARE                 0x0B010105
#define OID_BPC_PROGRAM_GUIDE                   0x0B010106
#define OID_BPC_LAST_ERROR                      0x0B020107
#define OID_BPC_POOL                            0x0B010108

#define OID_BPC_PROVIDER_SPECIFIC               0x0B020109
#define OID_BPC_ADAPTER_SPECIFIC                0x0B02010A

#define OID_BPC_CONNECT                         0x0B01010B
#define OID_BPC_COMMIT                          0x0B01010C
#define OID_BPC_DISCONNECT                      0x0B01010D
#define OID_BPC_CONNECTION_ENABLE               0x0B01010E
#define OID_BPC_POOL_RESERVE                    0x0B01010F
#define OID_BPC_POOL_RETURN                     0x0B010110
#define OID_BPC_FORCE_RECEIVE                   0x0B010111

#define OID_BPC_LAST                            0x0B020112

//
// IEEE1394 mandatory general OIDs.
//
#define OID_1394_LOCAL_NODE_INFO                0x0C010101
#define OID_1394_VC_INFO                        0x0C010102

//
// The following OIDs are not specific to a media.
//

//
// These are objects for Connection-oriented media call-managers.
//
#define OID_CO_ADD_PVC                          0xFE000001
#define OID_CO_DELETE_PVC                       0xFE000002
#define OID_CO_GET_CALL_INFORMATION             0xFE000003
#define OID_CO_ADD_ADDRESS                      0xFE000004
#define OID_CO_DELETE_ADDRESS                   0xFE000005
#define OID_CO_GET_ADDRESSES                    0xFE000006
#define OID_CO_ADDRESS_CHANGE                   0xFE000007
#define OID_CO_SIGNALING_ENABLED                0xFE000008
#define OID_CO_SIGNALING_DISABLED               0xFE000009
#define OID_CO_AF_CLOSE                         0xFE00000A

//
// Objects for call-managers and MCMs that support TAPI access.
//
#define OID_CO_TAPI_CM_CAPS                     0xFE001001
#define OID_CO_TAPI_LINE_CAPS                   0xFE001002
#define OID_CO_TAPI_ADDRESS_CAPS                0xFE001003
#define OID_CO_TAPI_TRANSLATE_TAPI_CALLPARAMS   0xFE001004
#define OID_CO_TAPI_TRANSLATE_NDIS_CALLPARAMS   0xFE001005
#define OID_CO_TAPI_TRANSLATE_TAPI_SAP          0xFE001006
#define OID_CO_TAPI_GET_CALL_DIAGNOSTICS        0xFE001007
#define OID_CO_TAPI_REPORT_DIGITS               0xFE001008 
#define OID_CO_TAPI_DONT_REPORT_DIGITS          0xFE001009

//
//  PnP and PM OIDs
//
#define OID_PNP_CAPABILITIES                    0xFD010100
#define OID_PNP_SET_POWER                       0xFD010101
#define OID_PNP_QUERY_POWER                     0xFD010102
#define OID_PNP_ADD_WAKE_UP_PATTERN             0xFD010103
#define OID_PNP_REMOVE_WAKE_UP_PATTERN          0xFD010104
#define OID_PNP_WAKE_UP_PATTERN_LIST            0xFD010105
#define OID_PNP_ENABLE_WAKE_UP                  0xFD010106

//
//  PnP/PM Statistics (Optional).
//
#define OID_PNP_WAKE_UP_OK                      0xFD020200
#define OID_PNP_WAKE_UP_ERROR                   0xFD020201

//
//  The following bits are defined for OID_PNP_ENABLE_WAKE_UP
//
#define NDIS_PNP_WAKE_UP_MAGIC_PACKET           0x00000001
#define NDIS_PNP_WAKE_UP_PATTERN_MATCH          0x00000002
#define NDIS_PNP_WAKE_UP_LINK_CHANGE            0x00000004

//
//  TCP/IP OIDs
//
#define OID_TCP_TASK_OFFLOAD                    0xFC010201
#define OID_TCP_TASK_IPSEC_ADD_SA               0xFC010202
#define OID_TCP_TASK_IPSEC_DELETE_SA            0xFC010203
#define OID_TCP_SAN_SUPPORT                     0xFC010204
#define OID_TCP_TASK_IPSEC_ADD_UDPESP_SA        0xFC010205
#define OID_TCP_TASK_IPSEC_DELETE_UDPESP_SA     0xFC010206


//
//  Defines for FFP
//
#define OID_FFP_SUPPORT                         0xFC010210
#define OID_FFP_FLUSH                           0xFC010211
#define OID_FFP_CONTROL                         0xFC010212
#define OID_FFP_PARAMS                          0xFC010213
#define OID_FFP_DATA                            0xFC010214

#define OID_FFP_DRIVER_STATS                    0xFC020210
#define OID_FFP_ADAPTER_STATS                   0xFC020211

//
//  Defines for QOS
//
#define OID_QOS_TC_SUPPORTED                    0xFB010100
#define OID_QOS_REMAINING_BANDWIDTH             0xFB010101
#define OID_QOS_ISSLOW_FLOW                     0xFB010102
#define OID_QOS_BESTEFFORT_BANDWIDTH            0xFB010103
#define OID_QOS_LATENCY                         0xFB010104
#define OID_QOS_FLOW_IP_CONFORMING              0xFB010105
#define OID_QOS_FLOW_COUNT                      0xFB010106
#define OID_QOS_NON_BESTEFFORT_LIMIT            0xFB010107
#define OID_QOS_SCHEDULING_PROFILES_SUPPORTED   0xFB010108
#define OID_QOS_CURRENT_SCHEDULING_PROFILE      0xFB010109
#define OID_QOS_TIMER_RESOLUTION                0xFB01010A
#define OID_QOS_STATISTICS_BUFFER               0xFB01010B
#define OID_QOS_HIERARCHY_CLASS                 0xFB01010C
#define OID_QOS_FLOW_MODE                       0xFB01010D
#define OID_QOS_ISSLOW_FRAGMENT_SIZE            0xFB01010E
#define OID_QOS_FLOW_IP_NONCONFORMING           0xFB01010F
#define OID_QOS_FLOW_8021P_CONFORMING           0xFB010110
#define OID_QOS_FLOW_8021P_NONCONFORMING        0xFB010111
#define OID_QOS_ENABLE_AVG_STATS                0xFB010112
#define OID_QOS_ENABLE_WINDOW_ADJUSTMENT        0xFB010113

//
// NDIS Proxy OID_GEN_CO_DEVICE_PROFILE structure. The optional OID and
// this structure is a generic means of describing a CO device's
// capabilites, and is used by the NDIS Proxy to construct a TAPI device
// capabilities structure.
//
typedef struct NDIS_CO_DEVICE_PROFILE
{
    NDIS_VAR_DATA_DESC  DeviceDescription;  // e.g. 'GigabitATMNet'
    NDIS_VAR_DATA_DESC  DevSpecificInfo;    // special features

    ULONG   ulTAPISupplementaryPassThru;// reserved in NT5
    ULONG   ulAddressModes;
    ULONG   ulNumAddresses;
    ULONG   ulBearerModes;
    ULONG   ulMaxTxRate; // bytes per second
    ULONG   ulMinTxRate; // bytes per second
    ULONG   ulMaxRxRate; // bytes per second
    ULONG   ulMinRxRate; // bytes per second
    ULONG   ulMediaModes;   

    //
    // Tone/digit generation and recognition capabilities
    //
    ULONG   ulGenerateToneModes;
    ULONG   ulGenerateToneMaxNumFreq;
    ULONG   ulGenerateDigitModes;
    ULONG   ulMonitorToneMaxNumFreq;
    ULONG   ulMonitorToneMaxNumEntries;
    ULONG   ulMonitorDigitModes;
    ULONG   ulGatherDigitsMinTimeout;// milliseconds
    ULONG   ulGatherDigitsMaxTimeout;// milliseconds

    ULONG   ulDevCapFlags;          // Misc. capabilities
    ULONG   ulMaxNumActiveCalls;    // (This * ulMinRate) = total bandwidth (which may equal ulMaxRate)
    ULONG   ulAnswerMode;           // Effect of answering a new call when an
                                    // existing call is non-idle
    //
    // User-User info sizes allowed to accompany each operation
    //
    ULONG   ulUUIAcceptSize;    // bytes
    ULONG   ulUUIAnswerSize;    // bytes
    ULONG   ulUUIMakeCallSize;  // bytes
    ULONG   ulUUIDropSize;      // bytes
    ULONG   ulUUISendUserUserInfoSize; // bytes
    ULONG   ulUUICallInfoSize;  // bytes

} NDIS_CO_DEVICE_PROFILE, *PNDIS_CO_DEVICE_PROFILE;

//
//  Structures for TCP IPSec.
//
typedef ULONG   IPAddr, IPMask;
typedef ULONG   SPI_TYPE;

typedef enum    _OFFLOAD_OPERATION_E
{
    AUTHENTICATE = 1,
    ENCRYPT
}
    OFFLOAD_OPERATION_E;

typedef struct _OFFLOAD_ALGO_INFO
{
    ULONG   algoIdentifier;
    ULONG   algoKeylen;
    ULONG   algoRounds;
}
    OFFLOAD_ALGO_INFO,
    *POFFLOAD_ALGO_INFO;

typedef enum _OFFLOAD_CONF_ALGO
{
    OFFLOAD_IPSEC_CONF_NONE,
    OFFLOAD_IPSEC_CONF_DES,
    OFFLOAD_IPSEC_CONF_RESERVED,
    OFFLOAD_IPSEC_CONF_3_DES,
    OFFLOAD_IPSEC_CONF_MAX
}
    OFFLOAD_CONF_ALGO;

typedef enum _OFFLOAD_INTEGRITY_ALGO
{
    OFFLOAD_IPSEC_INTEGRITY_NONE,
    OFFLOAD_IPSEC_INTEGRITY_MD5,
    OFFLOAD_IPSEC_INTEGRITY_SHA,
    OFFLOAD_IPSEC_INTEGRITY_MAX
}
    OFFLOAD_INTEGRITY_ALGO;

typedef struct _OFFLOAD_SECURITY_ASSOCIATION
{
    OFFLOAD_OPERATION_E     Operation;
    SPI_TYPE                SPI;
    OFFLOAD_ALGO_INFO       IntegrityAlgo;
    OFFLOAD_ALGO_INFO       ConfAlgo;
    OFFLOAD_ALGO_INFO       Reserved;
}
    OFFLOAD_SECURITY_ASSOCIATION,
    *POFFLOAD_SECURITY_ASSOCIATION;

#define OFFLOAD_MAX_SAS             3

#define OFFLOAD_INBOUND_SA          0x0001
#define OFFLOAD_OUTBOUND_SA         0x0002

typedef struct _OFFLOAD_IPSEC_ADD_SA
{
    IPAddr                          SrcAddr;
    IPMask                          SrcMask;
    IPAddr                          DestAddr;
    IPMask                          DestMask;
    ULONG                           Protocol;
    USHORT                          SrcPort;
    USHORT                          DestPort;
    IPAddr                          SrcTunnelAddr;
    IPAddr                          DestTunnelAddr;
    USHORT                          Flags;
    SHORT                           NumSAs;
    OFFLOAD_SECURITY_ASSOCIATION    SecAssoc[OFFLOAD_MAX_SAS];
    HANDLE                          OffloadHandle;
    ULONG                           KeyLen;
    UCHAR                           KeyMat[1];
} OFFLOAD_IPSEC_ADD_SA, *POFFLOAD_IPSEC_ADD_SA;

typedef struct _OFFLOAD_IPSEC_DELETE_SA
{
    HANDLE                          OffloadHandle;
} OFFLOAD_IPSEC_DELETE_SA, *POFFLOAD_IPSEC_DELETE_SA;


typedef enum _UDP_ENCAP_TYPE
{
    OFFLOAD_IPSEC_UDPESP_ENCAPTYPE_IKE,
    OFFLOAD_IPSEC_UDPESP_ENCAPTYPE_OTHER
} UDP_ENCAP_TYPE, * PUDP_ENCAP_TYPE;


typedef struct _OFFLOAD_IPSEC_UDPESP_ENCAPTYPE_ENTRY
{
    UDP_ENCAP_TYPE                  UdpEncapType;
    USHORT                          DstEncapPort;
} OFFLOAD_IPSEC_UDPESP_ENCAPTYPE_ENTRY, * POFFLOAD_IPSEC_UDPESP_ENCAPTYPE_ENTRY;


typedef struct _OFFLOAD_IPSEC_ADD_UDPESP_SA
{
    IPAddr                                  SrcAddr;
    IPMask                                  SrcMask;
    IPAddr                                  DstAddr;
    IPMask                                  DstMask;
    ULONG                                   Protocol;
    USHORT                                  SrcPort;
    USHORT                                  DstPort;
    IPAddr                                  SrcTunnelAddr;
    IPAddr                                  DstTunnelAddr;
    USHORT                                  Flags;
    SHORT                                   NumSAs;
    OFFLOAD_SECURITY_ASSOCIATION            SecAssoc[OFFLOAD_MAX_SAS];
    HANDLE                                  OffloadHandle;
    OFFLOAD_IPSEC_UDPESP_ENCAPTYPE_ENTRY    EncapTypeEntry;
    HANDLE                                  EncapTypeEntryOffldHandle;
    ULONG                                   KeyLen;
    UCHAR                                   KeyMat[1];
} OFFLOAD_IPSEC_ADD_UDPESP_SA, * POFFLOAD_IPSEC_ADD_UDPESP_SA;


typedef struct _OFFLOAD_IPSEC_DELETE_UDPESP_SA
{
    HANDLE                                  OffloadHandle;
    HANDLE                                  EncapTypeEntryOffldHandle;
} OFFLOAD_IPSEC_DELETE_UDPESP_SA, * POFFLOAD_IPSEC_DELETE_UDPESP_SA;


//
// Type to go with OID_GEN_VLAN_ID: the least significant 12 bits are
// used as the VLAN ID (VID) per IEEE 802.1Q. Higher order bits are
// reserved and must be set to 0.
//
typedef ULONG NDIS_VLAN_ID;

//
// Medium the Ndis Driver is running on (OID_GEN_MEDIA_SUPPORTED/ OID_GEN_MEDIA_IN_USE).
//
typedef enum _NDIS_MEDIUM
{
    NdisMedium802_3,
    NdisMedium802_5,
    NdisMediumFddi,
    NdisMediumWan,
    NdisMediumLocalTalk,
    NdisMediumDix,              // defined for convenience, not a real medium
    NdisMediumArcnetRaw,
    NdisMediumArcnet878_2,
    NdisMediumAtm,
    NdisMediumWirelessWan,
    NdisMediumIrda,
    NdisMediumBpc,
    NdisMediumCoWan,
    NdisMedium1394,
    NdisMediumInfiniBand,
    NdisMediumMax               // Not a real medium, defined as an upper-bound
} NDIS_MEDIUM, *PNDIS_MEDIUM;


//
// Physical Medium Type definitions. Used with OID_GEN_PHYSICAL_MEDIUM.
//
typedef enum _NDIS_PHYSICAL_MEDIUM
{
    NdisPhysicalMediumUnspecified,
    NdisPhysicalMediumWirelessLan,
    NdisPhysicalMediumCableModem,
    NdisPhysicalMediumPhoneLine,
    NdisPhysicalMediumPowerLine,
    NdisPhysicalMediumDSL,      // includes ADSL and UADSL (G.Lite)
    NdisPhysicalMediumFibreChannel,
    NdisPhysicalMedium1394,
    NdisPhysicalMediumWirelessWan,
    NdisPhysicalMediumNative802_11,
    NdisPhysicalMediumBluetooth,
    NdisPhysicalMediumMax       // Not a real physical type, defined as an upper-bound
} NDIS_PHYSICAL_MEDIUM, *PNDIS_PHYSICAL_MEDIUM;


//
//  Protocol types supported by ndis. These values need to be consistent with ADDRESS_TYPE_XXX defined in TDI.H
//
#define NDIS_PROTOCOL_ID_DEFAULT        0x00
#define NDIS_PROTOCOL_ID_TCP_IP         0x02
#define NDIS_PROTOCOL_ID_IPX            0x06
#define NDIS_PROTOCOL_ID_NBF            0x07
#define NDIS_PROTOCOL_ID_MAX            0x0F
#define NDIS_PROTOCOL_ID_MASK           0x0F

//
// The following is used with OID_GEN_TRANSPORT_HEADER_OFFSET to indicate the length of the layer-2 header
// for packets sent by a particular protocol.
//
typedef struct _TRANSPORT_HEADER_OFFSET
{
    USHORT      ProtocolType;       // The protocol that is sending this OID (NDIS_PROTOCOL_ID_XXX above)
    USHORT      HeaderOffset;       // The header offset
} TRANSPORT_HEADER_OFFSET, *PTRANSPORT_HEADER_OFFSET;


//
// The structures below need to be consistent with TRANSPORT_ADDRESS structures in TDI.H
//
typedef struct _NETWORK_ADDRESS
{
    USHORT      AddressLength;      // length in bytes of Address[] in this
    USHORT      AddressType;        // type of this address (NDIS_PROTOCOL_ID_XXX above)
    UCHAR       Address[1];         // actually AddressLength bytes long
} NETWORK_ADDRESS, *PNETWORK_ADDRESS;

//
// The following is used with OID_GEN_NETWORK_LAYER_ADDRESSES to set network layer addresses on an interface
//
typedef struct _NETWORK_ADDRESS_LIST
{
    LONG        AddressCount;       // number of addresses following
    USHORT      AddressType;        // type of this address (NDIS_PROTOCOL_ID_XXX above)
    NETWORK_ADDRESS Address[1];     // actually AddressCount elements long
} NETWORK_ADDRESS_LIST, *PNETWORK_ADDRESS_LIST;

//
// IP address - This must remain consistent with TDI_ADDRESS_IP in tdi.h
//
typedef struct _NETWORK_ADDRESS_IP
{
    USHORT      sin_port;
    ULONG       in_addr;
    UCHAR       sin_zero[8];
} NETWORK_ADDRESS_IP, *PNETWORK_ADDRESS_IP;

#define NETWORK_ADDRESS_LENGTH_IP sizeof (NETWORK_ADDRESS_IP)

//
// IPX address - This must remain consistent with TDI_ADDRESS_IPX in tdi.h.
//
typedef struct _NETWORK_ADDRESS_IPX
{
    ULONG       NetworkAddress;
    UCHAR       NodeAddress[6];
    USHORT      Socket;
} NETWORK_ADDRESS_IPX, *PNETWORK_ADDRESS_IPX;

#define NETWORK_ADDRESS_LENGTH_IPX sizeof (NETWORK_ADDRESS_IPX)

//
// Hardware status codes (OID_GEN_HARDWARE_STATUS).
//

typedef enum _NDIS_HARDWARE_STATUS
{
    NdisHardwareStatusReady,
    NdisHardwareStatusInitializing,
    NdisHardwareStatusReset,
    NdisHardwareStatusClosing,
    NdisHardwareStatusNotReady
} NDIS_HARDWARE_STATUS, *PNDIS_HARDWARE_STATUS;


//
// this is the type passed in the OID_GEN_GET_TIME_CAPS request
//
typedef struct _GEN_GET_TIME_CAPS
{
    ULONG                       Flags;  // Bits defined below
    ULONG                       ClockPrecision;
} GEN_GET_TIME_CAPS, *PGEN_GET_TIME_CAPS;

#define READABLE_LOCAL_CLOCK                    0x00000001
#define CLOCK_NETWORK_DERIVED                   0x00000002
#define CLOCK_PRECISION                         0x00000004
#define RECEIVE_TIME_INDICATION_CAPABLE         0x00000008
#define TIMED_SEND_CAPABLE                      0x00000010
#define TIME_STAMP_CAPABLE                      0x00000020

//
// this is the type passed in the OID_GEN_GET_NETCARD_TIME request
//
typedef struct _GEN_GET_NETCARD_TIME
{
    ULONGLONG                   ReadTime;
} GEN_GET_NETCARD_TIME, *PGEN_GET_NETCARD_TIME;

//
//  NDIS PnP routines and definitions.
//
typedef struct _NDIS_PM_PACKET_PATTERN
{
    ULONG   Priority;                   // Importance of the given pattern.
    ULONG   Reserved;                   // Context information for transports.
    ULONG   MaskSize;                   // Size in bytes of the pattern mask.
    ULONG   PatternOffset;              // Offset from beginning of this
                                        // structure to the pattern bytes.
    ULONG   PatternSize;                // Size in bytes of the pattern.
    ULONG   PatternFlags;               // Flags (TBD).
} NDIS_PM_PACKET_PATTERN, *PNDIS_PM_PACKET_PATTERN;


//
//  The following structure defines the device power states.
//
typedef enum _NDIS_DEVICE_POWER_STATE
{
    NdisDeviceStateUnspecified = 0,
    NdisDeviceStateD0,
    NdisDeviceStateD1,
    NdisDeviceStateD2,
    NdisDeviceStateD3,
    NdisDeviceStateMaximum
} NDIS_DEVICE_POWER_STATE, *PNDIS_DEVICE_POWER_STATE;

//
//  The following structure defines the wake-up capabilities of the device.
//
typedef struct _NDIS_PM_WAKE_UP_CAPABILITIES
{
    NDIS_DEVICE_POWER_STATE MinMagicPacketWakeUp;
    NDIS_DEVICE_POWER_STATE MinPatternWakeUp;
    NDIS_DEVICE_POWER_STATE MinLinkChangeWakeUp;
} NDIS_PM_WAKE_UP_CAPABILITIES, *PNDIS_PM_WAKE_UP_CAPABILITIES;

//
// the following flags define the -enabled- wake-up capabilities of the device
// passed in the Flags field of NDIS_PNP_CAPABILITIES structure
//
#define NDIS_DEVICE_WAKE_UP_ENABLE                          0x00000001
#define NDIS_DEVICE_WAKE_ON_PATTERN_MATCH_ENABLE            0x00000002
#define NDIS_DEVICE_WAKE_ON_MAGIC_PACKET_ENABLE             0x00000004

//
//  This structure defines general PnP capabilities of the miniport driver.
//
typedef struct _NDIS_PNP_CAPABILITIES
{
    ULONG                           Flags;
    NDIS_PM_WAKE_UP_CAPABILITIES    WakeUpCapabilities;
} NDIS_PNP_CAPABILITIES, *PNDIS_PNP_CAPABILITIES;

//
// Defines the attachment types for FDDI (OID_FDDI_ATTACHMENT_TYPE).
//
typedef enum _NDIS_FDDI_ATTACHMENT_TYPE
{
    NdisFddiTypeIsolated = 1,
    NdisFddiTypeLocalA,
    NdisFddiTypeLocalB,
    NdisFddiTypeLocalAB,
    NdisFddiTypeLocalS,
    NdisFddiTypeWrapA,
    NdisFddiTypeWrapB,
    NdisFddiTypeWrapAB,
    NdisFddiTypeWrapS,
    NdisFddiTypeCWrapA,
    NdisFddiTypeCWrapB,
    NdisFddiTypeCWrapS,
    NdisFddiTypeThrough
} NDIS_FDDI_ATTACHMENT_TYPE, *PNDIS_FDDI_ATTACHMENT_TYPE;


//
// Defines the ring management states for FDDI (OID_FDDI_RING_MGT_STATE).
//
typedef enum _NDIS_FDDI_RING_MGT_STATE
{
    NdisFddiRingIsolated = 1,
    NdisFddiRingNonOperational,
    NdisFddiRingOperational,
    NdisFddiRingDetect,
    NdisFddiRingNonOperationalDup,
    NdisFddiRingOperationalDup,
    NdisFddiRingDirected,
    NdisFddiRingTrace
} NDIS_FDDI_RING_MGT_STATE, *PNDIS_FDDI_RING_MGT_STATE;


//
// Defines the Lconnection state for FDDI (OID_FDDI_LCONNECTION_STATE).
//
typedef enum _NDIS_FDDI_LCONNECTION_STATE
{
    NdisFddiStateOff = 1,
    NdisFddiStateBreak,
    NdisFddiStateTrace,
    NdisFddiStateConnect,
    NdisFddiStateNext,
    NdisFddiStateSignal,
    NdisFddiStateJoin,
    NdisFddiStateVerify,
    NdisFddiStateActive,
    NdisFddiStateMaintenance
} NDIS_FDDI_LCONNECTION_STATE, *PNDIS_FDDI_LCONNECTION_STATE;


//
// Defines the medium subtypes for WAN medium (OID_WAN_MEDIUM_SUBTYPE).
// Sub-medium used only by connection-oriented WAN devices
// i.e. NdisMediumWan, NdisMediumCoWan.
//
typedef enum _NDIS_WAN_MEDIUM_SUBTYPE
{
    NdisWanMediumHub,
    NdisWanMediumX_25,
    NdisWanMediumIsdn,
    NdisWanMediumSerial,
    NdisWanMediumFrameRelay,
    NdisWanMediumAtm,
    NdisWanMediumSonet,
    NdisWanMediumSW56K,
    NdisWanMediumPPTP,
    NdisWanMediumL2TP,
    NdisWanMediumIrda,
    NdisWanMediumParallel,
    NdisWanMediumPppoe
} NDIS_WAN_MEDIUM_SUBTYPE, *PNDIS_WAN_MEDIUM_SUBTYPE;


//
// Defines the header format for WAN medium (OID_WAN_HEADER_FORMAT).
//
typedef enum _NDIS_WAN_HEADER_FORMAT
{
    NdisWanHeaderNative,        // src/dest based on subtype, followed by NLPID
    NdisWanHeaderEthernet       // emulation of ethernet header
} NDIS_WAN_HEADER_FORMAT, *PNDIS_WAN_HEADER_FORMAT;


//
// Defines the line quality on a WAN line (OID_WAN_QUALITY_OF_SERVICE).
//
typedef enum _NDIS_WAN_QUALITY
{
    NdisWanRaw,
    NdisWanErrorControl,
    NdisWanReliable
} NDIS_WAN_QUALITY, *PNDIS_WAN_QUALITY;


//
// Defines a protocol's WAN specific capabilities (OID_WAN_PROTOCOL_CAPS).
//
typedef struct _NDIS_WAN_PROTOCOL_CAPS
{
    IN  ULONG   Flags;
    IN  ULONG   Reserved;
} NDIS_WAN_PROTOCOL_CAPS, *PNDIS_WAN_PROTOCOL_CAPS;


//
// Flags used in NDIS_WAN_PROTOCOL_CAPS
//
#define WAN_PROTOCOL_KEEPS_STATS    0x00000001


//
// Defines the state of a token-ring adapter (OID_802_5_CURRENT_RING_STATE).
//
typedef enum _NDIS_802_5_RING_STATE
{
    NdisRingStateOpened = 1,
    NdisRingStateClosed,
    NdisRingStateOpening,
    NdisRingStateClosing,
    NdisRingStateOpenFailure,
    NdisRingStateRingFailure
} NDIS_802_5_RING_STATE, *PNDIS_802_5_RING_STATE;

//
// Defines the state of the LAN media
//
typedef enum _NDIS_MEDIA_STATE
{
    NdisMediaStateConnected,
    NdisMediaStateDisconnected
} NDIS_MEDIA_STATE, *PNDIS_MEDIA_STATE;

//
// The following is set on a per-packet basis as OOB data with NdisClass802_3Priority
//
typedef ULONG   Priority_802_3;         // 0-7 priority levels

//
//  The following structure is used to query OID_GEN_CO_LINK_SPEED and
//  OID_GEN_CO_MINIMUM_LINK_SPEED. The first OID will return the current
//  link speed of the adapter. The second will return the minimum link speed
//  the adapter is capable of.
//
typedef struct _NDIS_CO_LINK_SPEED
{
    ULONG   Outbound;
    ULONG   Inbound;
} NDIS_CO_LINK_SPEED, *PNDIS_CO_LINK_SPEED;

#ifndef _NDIS_
typedef int NDIS_STATUS, *PNDIS_STATUS;
#endif

//
//  Structure to be used for OID_GEN_SUPPORTED_GUIDS.
//  This structure describes an OID to GUID mapping.
//  Or a Status to GUID mapping.
//  When ndis receives a request for a give GUID it will
//  query the miniport with the supplied OID.
//
typedef struct _NDIS_GUID
{
    GUID            Guid;
    union
    {
        NDIS_OID    Oid;
        NDIS_STATUS Status;
    };
    ULONG       Size;               //  Size of the data element. If the GUID
                                    //  represents an array then this is the
                                    //  size of an element in the array.
                                    //  This is -1 for strings.
    ULONG       Flags;
} NDIS_GUID, *PNDIS_GUID;

#define fNDIS_GUID_TO_OID           0x00000001
#define fNDIS_GUID_TO_STATUS        0x00000002
#define fNDIS_GUID_ANSI_STRING      0x00000004
#define fNDIS_GUID_UNICODE_STRING   0x00000008
#define fNDIS_GUID_ARRAY            0x00000010
#define fNDIS_GUID_ALLOW_READ       0x00000020
#define fNDIS_GUID_ALLOW_WRITE      0x00000040

//
// Ndis Packet Filter Bits (OID_GEN_CURRENT_PACKET_FILTER).
//
#define NDIS_PACKET_TYPE_DIRECTED               0x00000001
#define NDIS_PACKET_TYPE_MULTICAST              0x00000002
#define NDIS_PACKET_TYPE_ALL_MULTICAST          0x00000004
#define NDIS_PACKET_TYPE_BROADCAST              0x00000008
#define NDIS_PACKET_TYPE_SOURCE_ROUTING         0x00000010
#define NDIS_PACKET_TYPE_PROMISCUOUS            0x00000020
#define NDIS_PACKET_TYPE_SMT                    0x00000040
#define NDIS_PACKET_TYPE_ALL_LOCAL              0x00000080
#define NDIS_PACKET_TYPE_GROUP                  0x00001000
#define NDIS_PACKET_TYPE_ALL_FUNCTIONAL         0x00002000
#define NDIS_PACKET_TYPE_FUNCTIONAL             0x00004000
#define NDIS_PACKET_TYPE_MAC_FRAME              0x00008000


//
// Ndis Token-Ring Ring Status Codes (OID_802_5_CURRENT_RING_STATUS).
//
#define NDIS_RING_SIGNAL_LOSS                   0x00008000
#define NDIS_RING_HARD_ERROR                    0x00004000
#define NDIS_RING_SOFT_ERROR                    0x00002000
#define NDIS_RING_TRANSMIT_BEACON               0x00001000
#define NDIS_RING_LOBE_WIRE_FAULT               0x00000800
#define NDIS_RING_AUTO_REMOVAL_ERROR            0x00000400
#define NDIS_RING_REMOVE_RECEIVED               0x00000200
#define NDIS_RING_COUNTER_OVERFLOW              0x00000100
#define NDIS_RING_SINGLE_STATION                0x00000080
#define NDIS_RING_RING_RECOVERY                 0x00000040


//
// Ndis protocol option bits (OID_GEN_PROTOCOL_OPTIONS).
//
#define NDIS_PROT_OPTION_ESTIMATED_LENGTH               0x00000001
#define NDIS_PROT_OPTION_NO_LOOPBACK                    0x00000002
#define NDIS_PROT_OPTION_NO_RSVD_ON_RCVPKT              0x00000004
#define NDIS_PROT_OPTION_SEND_RESTRICTED                0x00000008

//
// Ndis MAC option bits (OID_GEN_MAC_OPTIONS).
//
#define NDIS_MAC_OPTION_COPY_LOOKAHEAD_DATA             0x00000001
#define NDIS_MAC_OPTION_RECEIVE_SERIALIZED              0x00000002
#define NDIS_MAC_OPTION_TRANSFERS_NOT_PEND              0x00000004
#define NDIS_MAC_OPTION_NO_LOOPBACK                     0x00000008
#define NDIS_MAC_OPTION_FULL_DUPLEX                     0x00000010
#define NDIS_MAC_OPTION_EOTX_INDICATION                 0x00000020
#define NDIS_MAC_OPTION_8021P_PRIORITY                  0x00000040
#define NDIS_MAC_OPTION_SUPPORTS_MAC_ADDRESS_OVERWRITE  0x00000080
#define NDIS_MAC_OPTION_RECEIVE_AT_DPC                  0x00000100
#define NDIS_MAC_OPTION_8021Q_VLAN                      0x00000200
#define NDIS_MAC_OPTION_RESERVED                        0x80000000

//
//  NDIS media capabilities bits (OID_GEN_MEDIA_CAPABILITIES).
//
#define NDIS_MEDIA_CAP_TRANSMIT                 0x00000001  // Supports sending data
#define NDIS_MEDIA_CAP_RECEIVE                  0x00000002  // Supports receiving data

//
//  NDIS MAC option bits for OID_GEN_CO_MAC_OPTIONS.
//
#define NDIS_CO_MAC_OPTION_DYNAMIC_LINK_SPEED   0x00000001

//
// The following is set on a per-packet basis as OOB data with NdisClassIrdaPacketInfo
// This is the per-packet info specified on a per-packet basis
//
typedef struct _NDIS_IRDA_PACKET_INFO
{
    ULONG                       ExtraBOFs;
    ULONG                       MinTurnAroundTime;
} NDIS_IRDA_PACKET_INFO, *PNDIS_IRDA_PACKET_INFO;


#ifdef WIRELESS_WAN

//
// Wireless WAN structure definitions
//
//
// currently defined Wireless network subtypes
//
typedef enum _NDIS_WW_NETWORK_TYPE
{
    NdisWWGeneric,
    NdisWWMobitex,
    NdisWWPinpoint,
    NdisWWCDPD,
    NdisWWArdis,
    NdisWWDataTAC,
    NdisWWMetricom,
    NdisWWGSM,
    NdisWWCDMA,
    NdisWWTDMA,
    NdisWWAMPS,
    NdisWWInmarsat,
    NdisWWpACT,
    NdisWWFlex,
    NdisWWIDEN
} NDIS_WW_NETWORK_TYPE;

//
// currently defined header formats
//
typedef enum _NDIS_WW_HEADER_FORMAT
{
    NdisWWDIXEthernetFrames,
    NdisWWMPAKFrames,
    NdisWWRDLAPFrames,
    NdisWWMDC4800Frames,
    NdisWWNCLFrames
} NDIS_WW_HEADER_FORMAT;

//
// currently defined encryption types
//
typedef enum _NDIS_WW_ENCRYPTION_TYPE
{
    NdisWWUnknownEncryption = -1,
    NdisWWNoEncryption,
    NdisWWDefaultEncryption,
    NdisWWDESEncryption,
    NdisWWRC2Encryption,
    NdisWWRC4Encryption,
    NdisWWRC5Encryption
} NDIS_WW_ENCRYPTION_TYPE, *PNDIS_WW_ENCRYPTION_TYPE;

typedef enum _WW_ADDRESS_FORMAT
{
    WW_IEEE_ADDRESS = 0,
    WW_MOBITEX_MAN_ADDRESS,
    WW_DATATAC_RDLAP_ADDRESS,
    WW_DATATAC_MDC4800_ADDRESS,
    WW_DATATAC_RESERVED,
    WW_IPv4_ADDRESS,
    WW_IPv6_ADDRESS,
    WW_PROPRIETARY_ADDRESS,
} WW_ADDRESS_FORMAT;

typedef enum _WW_GEN_SUM_EXCEPTION
{
    SIM_STATUS_OK = 0,
    SIM_STATUS_ERROR,
    SIM_STATUS_MISSING,
    SIM_STATUS_NO_RESPONSE,
    SIM_STATUS_REMOVED,
    SIM_STATUS_CRYPT_ERROR,
    SIM_STATUS_AUTH_ERROR,
    SIM_STATUS_NEED_PIN,
    SIM_STATUS_NEED_PUK,
    SIM_STATUS_WRONG,
} WW_GEN_SIM_EXCEPTION;


//
// OID_WW_GEN_INDICATION_REQUEST
//
typedef struct _NDIS_WW_INDICATION_REQUEST
{
    NDIS_OID            Oid;                    // IN
    ULONG               uIndicationFlag;        // IN
    ULONG               uApplicationToken;      // IN OUT
    HANDLE              hIndicationHandle;      // IN OUT
    INT                 iPollingInterval;       // IN OUT
    NDIS_VAR_DATA_DESC  InitialValue;           // IN OUT
    NDIS_VAR_DATA_DESC  OIDIndicationValue;     // OUT - only valid after indication
    NDIS_VAR_DATA_DESC  TriggerValue;           // IN
} NDIS_WW_INDICATION_REQUEST, *PNDIS_WW_INDICATION_REQUEST;

#define OID_INDICATION_REQUEST_ENABLE           0x0000
#define OID_INDICATION_REQUEST_CANCEL           0x0001

//
// OID_WW_GEN_DEVICE_INFO
//
typedef struct _WW_DEVICE_INFO
{
    NDIS_VAR_DATA_DESC  Manufacturer;
    NDIS_VAR_DATA_DESC  ModelNum;
    NDIS_VAR_DATA_DESC  SWVersionNum;
    NDIS_VAR_DATA_DESC  SerialNum;
} WW_DEVICE_INFO, *PWW_DEVICE_INFO;

//
// OID_WW_GEN_OPERATION_MODE
//
typedef INT WW_OPERATION_MODE;                  //  0 = Normal mode
                                                //  1 = Power saving mode
                                                // -1 = mode unknown

//
// OID_WW_GEN_LOCK_STATUS
//
typedef INT WW_LOCK_STATUS;                     //  0 = unlocked
                                                //  1 = locked
                                                // -1 = unknown lock status

//
// OID_WW_GEN_DISABLE_TRANSMITTER
//
typedef INT WW_DISABLE_TRANSMITTER;             //  0 = transmitter enabled
                                                //  1 = transmitter disabled
                                                // -1 = unknown value

//
// OID_WW_GEN_NETWORK_ID
//
typedef NDIS_VAR_DATA_DESC  WW_NETWORK_ID;

//
// OID_WW_GEN_PERMANENT_ADDRESS 
//
typedef NDIS_VAR_DATA_DESC  WW_PERMANENT_ADDRESS;

//
// OID_WW_GEN_CURRENT_ADDRESS   
//
typedef struct _WW_CURRENT_ADDRESS
{
    NDIS_WW_HEADER_FORMAT   Format;
    NDIS_VAR_DATA_DESC      Address;
} WW_CURRENT_ADDRESS, *PWW_CURRENT_ADDRESS;

//
// OID_WW_GEN_SUSPEND_DRIVER
//
typedef BOOLEAN WW_SUSPEND_DRIVER;              // 0 = driver operational
                                                // 1 = driver suspended
//
// OID_WW_GEN_BASESTATION_ID
//
typedef NDIS_VAR_DATA_DESC  WW_BASESTATION_ID;

//
// OID_WW_GEN_CHANNEL_ID
//
typedef NDIS_VAR_DATA_DESC  WW_CHANNEL_ID;

//
// OID_WW_GEN_ENCRYPTION_STATE
//
typedef BOOLEAN WW_ENCRYPTION_STATE;            // 0 = if encryption is disabled
                                                // 1 = if encryption is enabled

//
// OID_WW_GEN_CHANNEL_QUALITY
//
typedef INT     WW_CHANNEL_QUALITY;             //  0 = Not in network contact,
                                                // 1-100 = Quality of Channel (100 is highest quality).
                                                // -1 = channel quality is unknown

//
// OID_WW_GEN_REGISTRATION_STATUS
//
typedef INT     WW_REGISTRATION_STATUS;         //  0 = Registration denied
                                                //  1 = Registration pending
                                                //  2 = Registered
                                                // -1 = unknown registration status

//
// OID_WW_GEN_RADIO_LINK_SPEED
//
typedef ULONG   WW_RADIO_LINK_SPEED;            // Bits per second.

//
// OID_WW_GEN_LATENCY
//
typedef ULONG   WW_LATENCY;                     //  milliseconds

//
// OID_WW_GEN_BATTERY_LEVEL
//
typedef INT     WW_BATTERY_LEVEL;               //  0-100 = battery level in percentage
                                                //  (100=fully charged)
                                                // -1 = unknown battery level.

//
// OID_WW_GEN_EXTERNAL_POWER
//
typedef INT     WW_EXTERNAL_POWER;              //  0 = no external power connected
                                                //  1 = external power connected
                                                //  -1 = unknown

//
// Ping Address structure
//
typedef struct _WW_PING_ADDRESS
{
    WW_ADDRESS_FORMAT   Format;                 // IN
    NDIS_VAR_DATA_DESC  TargetAddress;          // IN
    UINT                uTime;                  // OUT in milleseconds
} WW_PING_ADDRESS;


//
// RSSI structure
//
typedef struct _WW_RECEIVE_SIGNAL_STRENGTH_INDICATOR
{
    INT                 iDecibels;              // value in DB
    INT                 iFactor;                // power of 10
} WW_RECEIVE_SIGNAL_STRENGTH_INDICATOR;


//
// SIM status structure
//
typedef struct _WW_SIM_STATUS
{
    BOOLEAN             bHasSIM;                // TRUE = SIM required
    BOOLEAN             bBlocked;               // TRUE = SIM PIN access blocked
    BOOLEAN             bLocked;                // TRUE = PIN need to access device
    BOOLEAN             bInitialized;           // TRUE = SIM initialized
    UINT                uCountdown;             // = remaining number of attempt to
                                                // enter correct PIN
} WW_SIM_STATUS;

//
// enable SIM PIN structure
//
typedef struct _WW_ENABLE_SIM_PIN
{
    BOOLEAN             bEnabled;               // TRUE = security feature of SIM enabled
    NDIS_VAR_DATA_DESC  CurrentPIN;             // describes buffer containing PIN value
} WW_ENABLE_SIM_PIN;

//
// SIM PIN structure
//
typedef struct _WW_CHANGE_SIM_PIN
{
    NDIS_VAR_DATA_DESC  OldPIN;                 // describes buffer containing OLD PIN
    NDIS_VAR_DATA_DESC  NewPIN;                 // describes buffer containing new PIN
} WW_CHANGE_SIM_PIN;


//
// new change SIM PUK structure
//
typedef NDIS_VAR_DATA_DESC      WW_ENABLE_SIM_PUK;


//
// OID_WW_MET_FUNCTION
//
typedef NDIS_VAR_DATA_DESC      WW_MET_FUNCTION;

//
// OID_WW_TAC_COMPRESSION
//
typedef BOOLEAN WW_TAC_COMPRESSION;             // Determines whether or not network level compression
                                                // is being used.

//
// OID_WW_TAC_SET_CONFIG
//
// The DataTAC OID that referenced this object has been superceeded. The
// definition is still included for historical purposes only and should not
// be used
//
typedef struct _WW_TAC_SETCONFIG
{
    NDIS_VAR_DATA_DESC  RCV_MODE;               // Select confirmed/unconfirmed
                                                // receive mode
    NDIS_VAR_DATA_DESC  TX_CONTROL;             // Enable or Disable transmitter
    NDIS_VAR_DATA_DESC  RX_CONTROL;             // Enable or disable radio in
                                                // the modem
    NDIS_VAR_DATA_DESC  FLOW_CONTROL;           // Set flow control between DTE
                                                // and DCE
    NDIS_VAR_DATA_DESC  RESET_CNF;              // Reset configuration to
                                                // default
    NDIS_VAR_DATA_DESC  READ_CNF;               // Read the current
                                                // configuration
} WW_TAC_SETCONFIG, *PWW_TAC_SETCONFIG;

//
// OID_WW_TAC_GET_STATUS
//
// The DataTAC OID that referenced this object has been superceeded. The
// definition is still included for historical purposes only and should not
// be used
//
typedef struct _WW_TAC_GETSTATUS
{
    BOOLEAN                 Action;             // Set = Execute command.
    NDIS_VAR_DATA_DESC      Command;
    NDIS_VAR_DATA_DESC      Option;
    NDIS_VAR_DATA_DESC      Response;           // The response to the requested command
                                                // - max. length of string is 256 octets.
} WW_TAC_GETSTATUS, *PWW_TAC_GETSTATUS;

//
// OID_WW_TAC_USER_HEADER
//
typedef NDIS_VAR_DATA_DESC  WW_TAC_USERHEADER;  // This will hold the user header - Max. 64 octets.

// August 25, 1998 @14:16 EDT by Emil Sturniolo - WRQ
// added new DataTAC get response structure
typedef  struct _WW_TAC_GET_RESPONSE
{
    UINT                SDUTag;                 // previousl assigned token
    NDIS_VAR_DATA_DESC  Response;               // response - max 2048 octets
} WW_TAC_GET_RESPONSE;

//
// DataTAC disable receiver structure
//
typedef INT WW_TAC_DISABLE_RECEIVER;            // 0 = receiver enabled
                                                // 1 = receiver disabled
                                                // -1 = state of recevier unknown

//
// DataTAC antenna mode structure
//
typedef INT WW_TAC_ANTENNA_MODE;                // 0 = Automatic Antenna selection
                                                // 1 = Always use primary antenna
                                                // 2 = Always use secondary antenna
                                                // -1 = Antenna algorithm unknown

//
// DataTAC get response structure
//
typedef INT WW_TAC_FLUSH_DATA;                  // 1 = flush buffered data destine to net
                                                // 2 = flush buffered data received from net
                                                // 3 = flush all buffered data

//
// DataTAC shutdown device structure
//
typedef INT WW_TAC_SHUTDOWN_DEVICE;             // 0 = device enabled
                                                // 1 = device disabled
                                                // -1 = state of device unknown

//
// DataTAC transmitter keyed structure
//
typedef BOOLEAN WW_TAC_TRANSMITTER_KEYED;

//
// added new DataTAC system table structure
//
typedef struct _WW_TAC_SYSTEM_TABLE
{
    UINT        SystemCount;
    UCHAR       ContryTable[32];
    UCHAR       PrefixTable[32];
    UCHAR       IDTable[32];
} WW_TAC_SYSTEM_TABLE;

//
// added new DataTAC channel table structure
//
typedef struct _WW_TAC_CHANNEL_TABLE
{
    UINT        ChannelCount;
    UCHAR       ChannelTable[64];
    UCHAR       AttrTable[64];
} WW_TAC_CHANNEL_TABLE;


//
// added new DataTAC statistics structure
//
typedef NDIS_VAR_DATA_DESC WW_TAC_STATISTICS;



//
// OID_WW_ARD_SNDCP
//
// The ARDIS OIDs that referenced these object have been deprecated and merged
// with the new DataTAC objects. Their definition are still included for
// historical purposes only and should not be used.
//
typedef struct _WW_ARD_SNDCP
{
    NDIS_VAR_DATA_DESC  Version;                // The version of SNDCP protocol supported.
    INT                 BlockSize;              // The block size used for SNDCP
    INT                 Window;                 // The window size used in SNDCP
} WW_ARD_SNDCP, *PWW_ARD_SNDCP;

//
// OID_WW_ARD_TMLY_MSG
//
typedef BOOLEAN WW_ARD_CHANNEL_STATUS;          // The current status of the inbound RF Channel.

//
// OID_WW_ARD_DATAGRAM
//
typedef struct _WW_ARD_DATAGRAM
{
    BOOLEAN             LoadLevel;              // Byte that contains the load level info.
    INT                 SessionTime;            // Datagram session time remaining.
    NDIS_VAR_DATA_DESC  HostAddr;               // Host address.
    NDIS_VAR_DATA_DESC  THostAddr;              // Test host address.
} WW_ARD_DATAGRAM, *PWW_ARD_DATAGRAM;

//
// OID_WW_CDPD_SPNI
//
typedef struct _WW_CDPD_SPNI
{
    ULONG   SPNI[10];                           //10 16-bit service provider network IDs
    INT     OperatingMode;                      // 0 = ignore SPNI,
                                                // 1 = require SPNI from list,
                                                // 2 = prefer SPNI from list.
                                                // 3 = exclude SPNI from list.
} WW_CDPD_SPNI, *PWW_CDPD_SPNI;

//
// OID_WW_CDPD_WASI
//
typedef struct _WW_CDPD_WIDE_AREA_SERVICE_ID
{
    ULONG   WASI[10];                           //10 16-bit wide area service IDs
    INT     OperatingMode;                      // 0 = ignore WASI,
                                                // 1 = Require WASI from list,
                                                // 2 = prefer WASI from list
                                                // 3 = exclude WASI from list.
} WW_CDPD_WIDE_AREA_SERVICE_ID, *PWW_CDPD_WIDE_AREA_SERVICE_ID;

//
// OID_WW_CDPD_AREA_COLOR
//
typedef INT     WW_CDPD_AREA_COLOR;

//
// OID_WW_CDPD_TX_POWER_LEVEL
//
typedef ULONG   WW_CDPD_TX_POWER_LEVEL;

//
// OID_WW_CDPD_EID
//
typedef NDIS_VAR_DATA_DESC  WW_CDPD_EID;
//
// OID_WW_CDPD_HEADER_COMPRESSION
//
typedef INT WW_CDPD_HEADER_COMPRESSION;         //  0 = no header compression,
                                                //  1 = always compress headers,
                                                //  2 = compress headers if MD-IS does
                                                // -1 = unknown

//
// OID_WW_CDPD_DATA_COMPRESSION
//
typedef INT WW_CDPD_DATA_COMPRESSION;           // 0  = no data compression,
                                                // 1  = data compression enabled
                                                // -1 = unknown

//
// OID_WW_CDPD_CHANNEL_SELECT
//
typedef struct _WW_CDPD_CHANNEL_SELECT
{
    ULONG   ChannelID;                          // channel number
    ULONG   fixedDuration;                      // duration in seconds
} WW_CDPD_CHANNEL_SELECT, *PWW_CDPD_CHANNEL_SELECT;

//
// OID_WW_CDPD_CHANNEL_STATE
//
typedef enum _WW_CDPD_CHANNEL_STATE
{
    CDPDChannelNotAvail,
    CDPDChannelScanning,
    CDPDChannelInitAcquired,
    CDPDChannelAcquired,
    CDPDChannelSleeping,
    CDPDChannelWaking,
    CDPDChannelCSDialing,
    CDPDChannelCSRedial,
    CDPDChannelCSAnswering,
    CDPDChannelCSConnected,
    CDPDChannelCSSuspended
} WW_CDPD_CHANNEL_STATE, *PWW_CDPD_CHANNEL_STATE;

//
// OID_WW_CDPD_NEI
//
typedef enum _WW_CDPD_NEI_FORMAT
{
    CDPDNeiIPv4,
    CDPDNeiCLNP,
    CDPDNeiIPv6
} WW_CDPD_NEI_FORMAT, *PWW_CDPD_NEI_FORMAT;

typedef enum _WW_CDPD_NEI_TYPE
{
    CDPDNeiIndividual,
    CDPDNeiMulticast,
    CDPDNeiBroadcast
} WW_CDPD_NEI_TYPE;


typedef struct _WW_CDPD_NEI
{
    ULONG               uNeiIndex;
    WW_CDPD_NEI_FORMAT  NeiFormat;
    WW_CDPD_NEI_TYPE    NeiType;
    WORD                NeiGmid;                // group member identifier, only
                                                // meaningful if NeiType ==
                                                // CDPDNeiMulticast
    NDIS_VAR_DATA_DESC  NeiAddress;
} WW_CDPD_NEI;

//
// OID_WW_CDPD_NEI_STATE
//

typedef enum _WW_CDPD_NEI_STATE
{
    CDPDUnknown,
    CDPDRegistered,
    CDPDDeregistered
} WW_CDPD_NEI_STATE, *PWW_CDPD_NEI_STATE;

typedef enum _WW_CDPD_NEI_SUB_STATE
{
    CDPDPending,                                // Registration pending
    CDPDNoReason,                               // Registration denied - no reason given
    CDPDMDISNotCapable,                         // Registration denied - MD-IS not capable of
                                                //  handling M-ES at this time
    CDPDNEINotAuthorized,                       // Registration denied - NEI is not authorized to
                                                //  use this subnetwork
    CDPDInsufficientAuth,                       // Registration denied - M-ES gave insufficient
                                                //  authentication credentials
    CDPDUnsupportedAuth,                        // Registration denied - M-ES gave unsupported
                                                //  authentication credentials
    CDPDUsageExceeded,                          // Registration denied - NEI has exceeded usage
                                                //  limitations
    CDPDDeniedThisNetwork                       // Registration denied on this network, service
                                                //  may be obtained on alternate Service Provider
                                                //  network
} WW_CDPD_NEI_SUB_STATE;

typedef struct _WW_CDPD_NEI_REG_STATE
{
    ULONG               uNeiIndex;
    WW_CDPD_NEI_STATE   NeiState;
    WW_CDPD_NEI_SUB_STATE NeiSubState;
} WW_CDPD_NEI_REG_STATE, *PWW_CDPD_NEI_REG_STATE;

//
// OID_WW_CDPD_SERVICE_PROVIDER_IDENTIFIER
//
typedef struct _WW_CDPD_SERVICE_PROVIDER_ID
{
    ULONG   SPI[10];                            //10 16-bit service provider IDs
    INT     OperatingMode;                      // 0 = ignore SPI,
                                                // 1 = require SPI from list,
                                                // 2 = prefer SPI from list.
                                                // 3 = SPI from list is excluded
} WW_CDPD_SERVICE_PROVIDER_ID, *PWW_CDPD_SERVICE_PROVIDER_ID;

//
// OID_WW_CDPD_SLEEP_MODE
//
typedef INT WW_CDPD_SLEEP_MODE;

//
// OID_WW_CDPD_TEI
//
typedef ULONG   WW_CDPD_TEI;

//
// OID_WW_CDPD_CIRCUIT_SWITCHED
//
// The CDPD OID that referenced this object has been deprecated and superceeded
// by new discrete CDPD objects. The definition is still included for
// historical purposes only and should not be used.
//
typedef struct _WW_CDPD_CIRCUIT_SWITCHED
{
    INT                 service_preference;  // -1 = unknown,
                                                //  0 = always use packet switched CDPD,
                                                //  1 = always use CS CDPD via AMPS,
                                                //  2 = always use CS CDPD via PSTN,
                                                //  3 = use circuit switched via AMPS only
                                                //  when packet switched is not available.
                                                //  4 = use packet switched only when circuit
                                                //  switched via AMPS is not available.
                                                //  5 = device manuf. defined service
                                                //  preference.
                                                //  6 = device manuf. defined service
                                                //  preference.
    
    INT                 service_status;         // -1 = unknown,
                                                //  0 = packet switched CDPD,
                                                //  1 = circuit switched CDPD via AMPS,
                                                //  2 = circuit switched CDPD via PSTN.
    
    INT                 connect_rate;           //  CS connection bit rate (bits per second).
                                                //  0 = no active connection,
                                                // -1 = unknown

                                                //  Dial code last used to dial.
    NDIS_VAR_DATA_DESC  dial_code[20];
    
    ULONG               sid;                    //  Current AMPS system ID
    
    INT                 a_b_side_selection;     // -1 = unknown,
                                                //  0 = no AMPS service
                                                //  1 = AMPS "A" side channels selected
                                                //  2 = AMPS "B" side channels selected
    
    INT                 AMPS_channel;           // -1= unknown
                                                //  0 = no AMPS service.
                                                //  1-1023 = AMPS channel number in use
    
    ULONG               action;                 //  0 = no action
                                                //  1 = suspend (hangup)
                                                //  2 = dial
    
                                                //  Default dial code for CS CDPD service
                                                //  encoded as specified in the CS CDPD
                                                //  implementor guidelines.
    NDIS_VAR_DATA_DESC  default_dial[20];
    
                                                //  Number for the CS CDPD network to call
                                                //  back the mobile, encoded as specified in
                                                //  the CS CDPD implementor guidelines.
    NDIS_VAR_DATA_DESC  call_back[20];
    
    ULONG               sid_list[10];           //  List of 10 16-bit preferred AMPS
                                                //  system IDs for CS CDPD.
    
    ULONG               inactivity_timer;       //  Wait time after last data before dropping
                                                //  call.
                                                //  0-65535 = inactivity time limit (seconds).
    
    ULONG               receive_timer;          //  secs. per CS-CDPD Implementor Guidelines.
    
    ULONG               conn_resp_timer;        //  secs. per CS-CDPD Implementor Guidelines.
    
    ULONG               reconn_resp_timer;      //  secs. per CS-CDPD Implementor Guidelines.
    
    ULONG               disconn_timer;          //  secs. per CS-CDPD Implementor Guidelines.
    
    ULONG               NEI_reg_timer;          //  secs. per CS-CDPD Implementor Guidelines.
    
    ULONG               reconn_retry_timer;     //  secs. per CS-CDPD Implementor Guidelines.
    
    ULONG               link_reset_timer;       //  secs. per CS-CDPD Implementor Guidelines.
    
    ULONG               link_reset_ack_timer;   //  secs. per CS-CDPD Implementor Guidelines.
    
    ULONG               n401_retry_limit;       //  per CS-CDPD Implementor Guidelines.
    
    ULONG               n402_retry_limit;       //  per CS-CDPD Implementor Guidelines.
    
    ULONG               n404_retry_limit;       //  per CS-CDPD Implementor Guidelines.
    
    ULONG               n405_retry_limit;       //  per CS-CDPD Implementor Guidelines.
} WW_CDPD_CIRCUIT_SWITCHED, *WW_PCDPD_CIRCUIT_SWITCHED;

typedef ULONG   WW_CDPD_RSSI;

//
// cs-cdpd service preference structure
//
typedef INT WW_CDPD_CS_SERVICE_PREFERENCE;      // 0 = use packet switched CDPD only
                                                // 1 = use CS-CDPD via AMPS only
                                                // 2 = use CS-CDPD via PSTN only
                                                // 3 = use CS-CDPD via AMPS only
                                                //     when packet switched is N/A
                                                // 4 = use packet switched CDPD only
                                                //     when  CS-CDPD via AMPS is N/A
                                                // 5 = Device manufacture defined
                                                //     service preference
                                                // 6 = device manufacture defined
                                                //     service preference
                                                // -1 = unknown

//
// cs-cdpd service status structure
//
typedef INT WW_CDPD_CS_SERVICE_STATUS;          // 0 = Packet switched CDPD
                                                // 1 = CS-CDPD via AMPS
                                                // 2 = CS-CDPD via PSTN
                                                // -1 = unknown



//
// cs-cdpd info structure
//
typedef struct _WW_CDPD_CS_INFO {
    INT                 ConnectRage;            // 0 = no active connection
                                                // -1 = unknown
                                                // all other values represent BPS
    NDIS_VAR_DATA_DESC  DialCode;               // describes buffer of last dial code
    UINT                SID;                    // Current AMPS System ID
    INT                 ABSideSelection;        // 0 = no AMPS service
                                                // 1 = AMPS "A" side channel selected
                                                // 2 = AMPS "B" side channel selected
    INT                 AMPSChannel;            // 0 = no AMPS service
                                                // 1-1023 = current AMPS channel
                                                // -1 = Unknown
                                                // all other values reserved
} WW_CDPD_CS_INFO;



//
// cs-cdpd suspend structure
//
typedef UINT WW_CDPD_CS_SUSPEND;                // 0 = nop; 1 = hang up


//
// cs-cdpd default dial code structure
//
typedef NDIS_VAR_DATA_DESC WW_CDPD_DEFAULT_DIAL_CODE;   // max 20 octets

//
// cs-cdpd callback structure
//
typedef struct _WW_CDPD_CS_CALLBACK
{
    UINT                Enabled;                // 0 = disable; 1 = enable; -1 = unknown
    NDIS_VAR_DATA_DESC  Number;                 // descibes buffer contianing dial code
                                                // max 20 octets
} WW_CDPD_CS_CALLBACK;


//
// cs-cdpd system id list structure
//
typedef struct _WW_CDPD_CS_SID_LIST
{
    UINT    AMPSystemId[10];
} WW_CDPD_CS_SID_LIST;

//
// cs-cdpd configuration structure
//
typedef struct _WW_CDPD_CS_CONFIGURATION
{
    UINT    InactivityTimer;                    // in seconds
    UINT    ReceiveTimer;                       // in seconds
    UINT    ConnResTimer;                       // in seconds
    UINT    ReconnRespTimer;                    // in seconds
    UINT    DisconnTimer;                       // in seconds
    UINT    NEIRegTimer;                        // in seconds
    UINT    ReconnRetryTimer;                   // in seconds
    UINT    LinkResetTimer;                     // in seconds
    UINT    LinkResetAckTimer;                  // in seconds
    UINT    n401RetryLimit;                     // per CS-CDPD Implementers guidelines
    UINT    n402RetryLimit;                     // per CS-CDPD Implementers guidelines
    UINT    n404RetryLimit;                     // per CS-CDPD Implementers guidelines
    UINT    n405RetryLimit;                     // per CS-CDPD Implementers guidelines
} WW_CDPD_CS_CONFIGURATION;


//
// OID_WW_PIN_LOC_AUTHORIZE
//
// The Pin Point OIDs that referenced the structures below have been
// deprecated from the PCCA STD-201 standard. Their definitions are still
// included for historical purposes only and should not be used.
//
typedef INT WW_PIN_AUTHORIZED;                  // 0  = unauthorized
                                                // 1  = authorized
                                                // -1 = unknown

//
// OID_WW_PIN_LAST_LOCATION
// OID_WW_PIN_LOC_FIX
//
typedef struct _WW_PIN_LOCATION
{
    INT     Latitude;                           // Latitude in hundredths of a second
    INT     Longitude;                          // Longitude in hundredths of a second
    INT     Altitude;                           // Altitude in feet
    INT     FixTime;                            // Time of the location fix, since midnight,  local time (of the
                                                // current day), in tenths of a second
    INT     NetTime;                            // Current local network time of the current day, since midnight,
                                                // in tenths of a second
    INT     LocQuality;                         // 0-100 = location quality
    INT     LatReg;                             // Latitude registration offset, in hundredths of a second
    INT     LongReg;                            // Longitude registration offset, in hundredths of a second
    INT     GMTOffset;                          // Offset in minutes of the local time zone from GMT
} WW_PIN_LOCATION, *PWW_PIN_LOCATION;


//
// The following is set on a per-packet basis as OOB data with NdisClassWirelessWanMbxMailbox
//
typedef ULONG   WW_MBX_MAILBOX_FLAG;            // 1 = set mailbox flag, 0 = do not set mailbox flag

//
// OID_WW_MBX_SUBADDR
//
typedef struct _WW_MBX_PMAN
{
    BOOLEAN             ACTION;                 // 0 = Login PMAN,  1 = Logout PMAN
    ULONG               MAN;
    UCHAR               PASSWORD[8];            // Password should be null for Logout and indications.
                                                // Maximum length of password is 8 chars.
} WW_MBX_PMAN, *PWW_MBX_PMAN;

//
// OID_WW_MBX_FLEXLIST
//
typedef struct  _WW_MBX_FLEXLIST
{
    INT     count;                              //  Number of MAN entries used.
                                                // -1=unknown.
    ULONG   MAN[7];                             //  List of MANs.
} WW_MBX_FLEXLIST;

//
// OID_WW_MBX_GROUPLIST
//
typedef struct  _WW_MBX_GROUPLIST
{
    INT  count;                                 //  Number of MAN entries used.
                                                // -1=unknown.
    ULONG   MAN[15];                            //  List of MANs.
} WW_MBX_GROUPLIST;

//
// OID_WW_MBX_TRAFFIC_AREA
//
typedef enum    _WW_MBX_TRAFFIC_AREA
{
    unknown_traffic_area,                       // The driver has no information about the current traffic area.
    in_traffic_area,                            // Mobile unit has entered a subscribed traffic area.
    in_auth_traffic_area,                       // Mobile unit is outside traffic area but is authorized.
    unauth_traffic_area                         // Mobile unit is outside traffic area but is un-authorized.
} WW_MBX_TRAFFIC_AREA;

//
// OID_WW_MBX_LIVE_DIE
//
typedef INT WW_MBX_LIVE_DIE;                    //  0 = DIE last received   
                                                //  1 = LIVE last received
                                                // -1 = unknown

//
// OID_WW_MBX_TEMP_DEFAULTLIST
//
typedef struct _WW_MBX_CHANNEL_PAIR
{
    ULONG               Mobile_Tx;
    ULONG               Mobile_Rx;
} WW_MBX_CHANNEL_PAIR, *PWW_MBX_CHANNEL_PAIR;

typedef struct _WW_MBX_TEMPDEFAULTLIST
{
    ULONG               Length;
    WW_MBX_CHANNEL_PAIR ChannelPair[1];
} WW_MBX_TEMPDEFAULTLIST, *WW_PMBX_TEMPDEFAULTLIST;

#endif // WIRELESS_WAN

//
//
// Base types that were redefined for BPC
//
// BPC_FILETIME is used exactly like FILETIME in Win32
//
// BPC_HANDLE is opaque to everything except the Miniport
//              
typedef struct _BPC_FILETIME
{
    ULONG   dwLowDateTime;
    ULONG   dwHighDateTime;
} BPC_FILETIME, *PBPC_FILETIME;

typedef PVOID   BPC_HANDLE;

//
// BPC Extension Globals
//
//
#define BPC_MIN_DIMENSION       1
#define BPC_MAX_BUFFER_SIZE     64
#define BPC_MIN_BUFFER_SIZE     4
#define BPC_DEVICE_ANY          ((BPC_HANDLE) 0xFFFFFFFF)

//
// Buffer indicate reason codes
//
//
typedef enum _NDIS_BPC_INDICATE_REASON
{
    bpcBufferFull = 0,
    bpcBufferForced,
    bpcBufferTimeOut,
    bpcBufferDiscontinuity,
    bpcBufferOverflow,
    bpcBufferStatusEvent

} NDIS_BPC_INDICATE_REASON, *PNDIS_BPC_INDICATE_REASON;


//
// BPC Stream Types
//
#define BPC_STREAM_TYPE_GENERIC_MIN     0x01000000
#define BPC_STREAM_TYPE_RAW             0x01000000
#define BPC_STREAM_TYPE_MPT_128         0x01000001
#define BPC_STREAM_TYPE_MPT_128_CRC     0x01000002
#define BPC_STREAM_TYPE_IP              0x01000003
#define BPC_STREAM_TYPE_PROVIDER_MIN    0x02000000
#define BPC_STREAM_TYPE_PROVIDER_MAX    0x02ffffff
#define BPC_STREAM_TYPE_ADAPTER_MIN     0x03000000
#define BPC_STREAM_TYPE_ADAPTER_MAX     0x03ffffff


//
// BPC Adapter Capabilities
//
typedef struct _NDIS_BPC_ADAPTER_CAPS
{
    ULONG   ulBPCMajorRev;
    ULONG   ulBPCMinorRev;
    ULONG   ulcHSDataDevices;
    ULONG   ulbpsHSDeviceMax;
    ULONG   ulcLSDataDevices;
    ULONG   ulbpsLSDeviceMax;
    ULONG   ulcTuningDevices;
    ULONG   ulcbLargestStatus;
    ULONG   ulVendorId;
    ULONG   ulAdapterId;
    GUID    guidProvider;

} NDIS_BPC_ADAPTER_CAPS, *PNDIS_BPC_ADAPTER_CAPS;


//
//  BPC Device Enumeration
//
typedef struct _NDIS_BPC_DEVICES
{
    ULONG       ulcDevices;
    BPC_HANDLE  rgnhDevices[BPC_MIN_DIMENSION];

} NDIS_BPC_DEVICES, *PNDIS_BPC_DEVICES;

#define CbDevices(cnt) (FIELD_OFFSET(NDIS_BPC_DEVICES, rgnhDevices) + (cnt) * sizeof(BPC_HANDLE))


//
// BPC Device Capabilities Structure
//
typedef struct NDIS_BPC_DEVICE_CAPS
{
    BPC_HANDLE  nhDevice;
    ULONG       ulBPCCaps;
    ULONG       ulbpsMax;
    ULONG       ulcStreamTypes;
    ULONG       rgulStreamTypes[BPC_MIN_DIMENSION];

} NDIS_BPC_DEVICE_CAPS, *PNDIS_BPC_DEVICE_CAPS;

#define CbDeviceCaps(cnt) (FIELD_OFFSET(NDIS_BPC_DEVICE_CAPS, rgulStreamTypes) + (cnt) * sizeof(ULONG))


//
// BPC Device Capability Definitions
// (ie Flags that can be set in ulBPCCaps
//
#define BPCCapBusMasteredData   0x01
#define BPCCapIndependentTuner  0x02
#define BPCCapExternalDataBus   0x04
#define BPCCapLowSpeedData      0x10
#define BPCCapHighSpeedData     0x20


//
// BPC Device Settings Structure
//
typedef struct NDIS_BPC_DEVICE_SETTINGS
{
    BPC_HANDLE  nhDevice;
    ULONG       ulBPCCaps;
    ULONG       ulcConnections;
    BOOLEAN     fEnabled;
    ULONG       ulStreamType;
    ULONG       ulcbAddressConnection;
    ULONG       rgulAddressConnection[BPC_MIN_DIMENSION];

} NDIS_BPC_DEVICE_SETTINGS, *PNDIS_BPC_DEVICE_SETTINGS;

#define CbDeviceSettings(cnt) (FIELD_OFFSET(NDIS_BPC_DEVICE_SETTINGS, rgulAddressConnection) + (cnt) * sizeof(ULONG))


//
// BPC Connection State Definitions
// (ie Acceptable values for ulState)
//
#define BPC_CONNECT_STATE_UNCOMMITTED   0
#define BPC_CONNECT_STATE_QUEUED        1
#define BPC_CONNECT_STATE_ACTIVE        2
#define BPC_CONNECT_STATE_DISCONNECTING 3


//
// BPC Connections Status Structure
//
typedef struct NDIS_BPC_CONNECTION_STATUS
{
    BPC_HANDLE  nhConnection;
    BPC_HANDLE  nhDevice;
    ULONG       ulConnectPriority;
    ULONG       ulDisconnectPriority;
    ULONG       ulbpsAverage;
    ULONG       ulbpsBurst;
    ULONG       ulState;
    BOOLEAN     fEnabled;

} NDIS_BPC_CONNECTION_STATUS, *PNDIS_BPC_CONNECTION_STATUS;


//
// BPC Address Comparison Structure
//
typedef struct NDIS_BPC_ADDRESS_COMPARE
{
    BOOLEAN fEqual;
    ULONG   ulcbFirstOffset;
    ULONG   ulcbFirstLength;
    ULONG   ulcbSecondOffset;
    ULONG   ulcbSecondLength;

} NDIS_BPC_ADDRESS_COMPARE, *PNDIS_BPC_ADDRESS_COMPARE;


//
// BPC Program Guide Types
//
// Currently there are no Generic BPC Program Guide types.
//
#define BPC_GUIDE_GENERIC_MIN   0x01000000
#define BPC_GUIDE_GENERIC_MAX   0x01ffffff
#define BPC_GUIDE_PROVIDER_MIN  0x02000000
#define BPC_GUIDE_PROVIDER_MAX  0x02ffffff
#define BPC_GUIDE_ADAPTER_MIN   0x03000000
#define BPC_GUIDE_ADAPTER_MAX   0x03ffffff


//
// BPC Program Guide Structure
//
typedef struct NDIS_BPC_PROGRAM_GUIDE
{
    ULONG       ulGuideType;
    BPC_FILETIME ftLastUpdate;
    ULONG       ulChangeNumber;
    ULONG       ulcbGuideSize;
    ULONG       rgulGuideData[BPC_MIN_DIMENSION];

} NDIS_BPC_PROGRAM_GUIDE, *PNDIS_BPC_PROGRAM_GUIDE;


//
// BPC Extension Errors
// (ie Acceptable values for ulBPCError)
//
typedef enum _NDIS_BPC_ERROR
{
    bpcErrorUnknownFailure = 0xc0ff0000,
    bpcErrorHardwareFailure,
    bpcErrorProviderFailure,
    bpcErrorNoDataDevice,
    bpcErrorNoTuningDevice,
    bpcErrorDeviceNotCapable,
    bpcErrorConflictingDevice,
    bpcErrorConflictingCapability,
    bpcErrorNoBufferMemory,
    bpcErrorNoResources,
    bpcErrorAdapterClosing,
    bpcErrorConnectionClosing,
    bpcErrorTooComplex,
    bpcErrorProviderNotSupported,
    bpcErrorUnknownProviderStructure,
    bpcErrorAddressNotSupported,
    bpcErrorInvalidAddress,
    bpcErrorUnknownAdapterStructure
} NDIS_BPC_ERROR, *PNDIS_BPC_ERROR;


//
//  BPC Last Error Structure
//
typedef struct NDIS_BPC_LAST_ERROR
{
    ULONG       ulErrorContext;
    ULONG       ulBPCError;
    ULONG       ulAdapterError;
    ULONG       ulAdapterContext;

} NDIS_BPC_LAST_ERROR, *PNDIS_BPC_LAST_ERROR;


//
//  BPC Buffer Pool Request/Report Structure
//
typedef struct NDIS_BPC_POOL
{
    BPC_HANDLE  nhConnection;
    ULONG       ulcbPoolSize;
    ULONG       ulcbMaxBufferSize;
    ULONG       ulcbBufferReserved;

} NDIS_BPC_POOL, *PNDIS_BPC_POOL;


//
// BPC Provider and Adapter Specific Structures are defined in the
// BpcXXXX.H file which the Provider/Adapter Manufacturer supplies.
//

//
// BPC Connect Structure
//
typedef struct NDIS_BPC_CONNECT
{
    BPC_HANDLE  nhConnection;
    BPC_HANDLE  nhDevice;
    ULONG       ulConnectPriority;
    ULONG       ulDisconnectPriority;
    BOOLEAN     fImmediate;
    ULONG       ulcbAddress;
    GUID        guidProvider;
    ULONG       rgulAddress[BPC_MIN_DIMENSION];

} NDIS_BPC_CONNECT, *PNDIS_BPC_CONNECT;

#define CbConnect(cnt)  (FIELD_OFFSET(NDIS_BPC_CONNECT, rgulAddress) + (cnt) * sizeof(ULONG))


//
//  BPC Commit Connections Structure
//
typedef struct NDIS_BPC_COMMIT
{
    ULONG       ulcConnections;
    BPC_HANDLE  rgnhConnections[BPC_MIN_DIMENSION];

} NDIS_BPC_COMMIT, *PNDIS_BPC_COMMIT;


//
//  BPC Disconnect Structure
//
typedef struct NDIS_BPC_DISCONNECT
{
    BPC_HANDLE  nhConnection;

} NDIS_BPC_DISCONNECT, *PNDIS_BPC_DISCONNECT;


//
//  BPC Enable Connection Structure
//
typedef struct NDIS_BPC_CONNECTION_ENABLE
{
    BPC_HANDLE  nhConnection;
    BOOLEAN     fEnabled;

} NDIS_BPC_CONNECTION_ENABLE, *PNDIS_BPC_CONNECTION_ENABLE;


//
// BPC Pool Return Structure
//
typedef struct NDIS_BPC_POOL_RETURN
{
    BPC_HANDLE  nhConnection;

} NDIS_BPC_POOL_RETURN, *PNDIS_BPC_POOL_RETURN;


typedef struct NDIS_BPC_FORCE_RECEIVE
{
    BPC_HANDLE  nhConnection;
    ULONG       ulReasonCode;

} NDIS_BPC_FORCE_RECEIVE, *PNDIS_BPC_FORCE_RECEIVE;


//
//  BPC Media Specific Information Structure
//
typedef struct NDIS_BPC_MEDIA_SPECIFIC_INFORMATION
{
    BPC_HANDLE nhConnection;                    // The handle to the data device.
    ULONG       ulBPCStreamType;                // The stream type of the data in packet
    ULONG       ulReasonCode;                   // The reason the buffer was indicated
    PVOID       pvMiniportReserved1;
    ULONG       ulMiniportReserved2;

} NDIS_BPC_MEDIA_SPECIFIC_INFORMATION, *PNDIS_BPC_MEDIA_SPECIFIC_INFORMATION;


//
// BPC Status Categories
//
#define BPC_CATEGORY_BPC            0x01000000
#define BPC_CATEGORY_PROVIDER       0x02000000
#define BPC_CATEGORY_ADAPTER        0x03000000


//
// BPC Status Types for Category BPC_CATEGORY_BPC
//
#define BPC_STATUS_CONNECTED        0x00000001
#define BPC_STATUS_QUEUED           0x00000002
#define BPC_STATUS_ACTIVE           0x00000003
#define BPC_STATUS_DISCONNECTED     0x00000004
#define BPC_STATUS_OVERFLOW         0x00000005
#define BPC_STATUS_DATA_STOP        0x00000006
#define BPC_STATUS_DATA_START       0x00000007
#define BPC_STATUS_DATA_ERROR       0x00000008


//
// BPC Status Indication Structure
//
typedef struct NDIS_BPC_STATUS
{
    ULONG ulStatusCategory;
    ULONG ulStatusType;
    ULONG ulcbStatus;
    ULONG rgulStatus;
} NDIS_BPC_STATUS, *PNDIS_BPC_STATUS;


//
// BPC Connection Status Structure
//
// All BPC Generic Connection Status package this structure
// in rgulStatus to indicate to which connection and device
// the status pertains.
//
typedef struct NDIS_BPC_STATUS_CONNECTION
{
    BPC_HANDLE  nhConnection;
    BPC_HANDLE nhDevice;

} NDIS_BPC_STATUS_CONNECTED, *PNDIS_BPC_STATUS_CONNECTED;

#ifdef __cplusplus
}
#endif

//
// flags used for OID_GEN_MINIPORT_INFO
//

#define NDIS_MINIPORT_BUS_MASTER                        0x00000001
#define NDIS_MINIPORT_WDM_DRIVER                        0x00000002
#define NDIS_MINIPORT_SG_LIST                           0x00000004
#define NDIS_MINIPORT_SUPPORTS_MEDIA_QUERY              0x00000008
#define NDIS_MINIPORT_INDICATES_PACKETS                 0x00000010
#define NDIS_MINIPORT_IGNORE_PACKET_QUEUE               0x00000020
#define NDIS_MINIPORT_IGNORE_REQUEST_QUEUE              0x00000040
#define NDIS_MINIPORT_IGNORE_TOKEN_RING_ERRORS          0x00000080
#define NDIS_MINIPORT_INTERMEDIATE_DRIVER               0x00000100
#define NDIS_MINIPORT_IS_NDIS_5                         0x00000200
#define NDIS_MINIPORT_IS_CO                             0x00000400
#define NDIS_MINIPORT_DESERIALIZE                       0x00000800
#define NDIS_MINIPORT_REQUIRES_MEDIA_POLLING            0x00001000
#define NDIS_MINIPORT_SUPPORTS_MEDIA_SENSE              0x00002000
#define NDIS_MINIPORT_NETBOOT_CARD                      0x00004000
#define NDIS_MINIPORT_PM_SUPPORTED                      0x00008000
#define NDIS_MINIPORT_SUPPORTS_MAC_ADDRESS_OVERWRITE    0x00010000
#define NDIS_MINIPORT_USES_SAFE_BUFFER_APIS             0x00020000
#define NDIS_MINIPORT_HIDDEN                            0x00040000
#define NDIS_MINIPORT_SWENUM                            0x00080000
#define NDIS_MINIPORT_SURPRISE_REMOVE_OK                0x00100000
#define NDIS_MINIPORT_NO_HALT_ON_SUSPEND                0x00200000
#define NDIS_MINIPORT_HARDWARE_DEVICE                   0x00400000
#define NDIS_MINIPORT_SUPPORTS_CANCEL_SEND_PACKETS      0x00800000
#define NDIS_MINIPORT_64BITS_DMA                        0x01000000
#define NDIS_MINIPORT_USE_NEW_BITS                      0x02000000
#define NDIS_MINIPORT_EXCLUSIVE_INTERRUPT               0x04000000
#define NDIS_MINIPORT_SENDS_PACKET_ARRAY                0x08000000
#define NDIS_MINIPORT_FILTER_IM                         0x10000000
#define NDIS_MINIPORT_SHORT_PACKETS_ARE_PADDED          0x20000000

#endif // _NTDDNDIS_





