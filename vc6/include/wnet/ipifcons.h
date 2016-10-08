/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    ipifcons.h

Abstract:
    Constants needed for the Interface Object

--*/

#ifndef __IPIFCONS_H__
#define __IPIFCONS_H__

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Media types                                                              //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#define MIN_IF_TYPE                     1

#define IF_TYPE_OTHER                   1   // None of the below
#define IF_TYPE_REGULAR_1822            2
#define IF_TYPE_HDH_1822                3
#define IF_TYPE_DDN_X25                 4
#define IF_TYPE_RFC877_X25              5
#define IF_TYPE_ETHERNET_CSMACD         6
#define IF_TYPE_IS088023_CSMACD         7
#define IF_TYPE_ISO88024_TOKENBUS       8
#define IF_TYPE_ISO88025_TOKENRING      9
#define IF_TYPE_ISO88026_MAN            10
#define IF_TYPE_STARLAN                 11
#define IF_TYPE_PROTEON_10MBIT          12
#define IF_TYPE_PROTEON_80MBIT          13
#define IF_TYPE_HYPERCHANNEL            14
#define IF_TYPE_FDDI                    15
#define IF_TYPE_LAP_B                   16
#define IF_TYPE_SDLC                    17
#define IF_TYPE_DS1                     18  // DS1-MIB
#define IF_TYPE_E1                      19  // Obsolete; see DS1-MIB
#define IF_TYPE_BASIC_ISDN              20
#define IF_TYPE_PRIMARY_ISDN            21
#define IF_TYPE_PROP_POINT2POINT_SERIAL 22  // proprietary serial
#define IF_TYPE_PPP                     23
#define IF_TYPE_SOFTWARE_LOOPBACK       24
#define IF_TYPE_EON                     25  // CLNP over IP
#define IF_TYPE_ETHERNET_3MBIT          26
#define IF_TYPE_NSIP                    27  // XNS over IP
#define IF_TYPE_SLIP                    28  // Generic Slip
#define IF_TYPE_ULTRA                   29  // ULTRA Technologies
#define IF_TYPE_DS3                     30  // DS3-MIB
#define IF_TYPE_SIP                     31  // SMDS, coffee
#define IF_TYPE_FRAMERELAY              32  // DTE only
#define IF_TYPE_RS232                   33
#define IF_TYPE_PARA                    34  // Parallel port
#define IF_TYPE_ARCNET                  35
#define IF_TYPE_ARCNET_PLUS             36
#define IF_TYPE_ATM                     37  // ATM cells
#define IF_TYPE_MIO_X25                 38
#define IF_TYPE_SONET                   39  // SONET or SDH
#define IF_TYPE_X25_PLE                 40
#define IF_TYPE_ISO88022_LLC            41
#define IF_TYPE_LOCALTALK               42
#define IF_TYPE_SMDS_DXI                43
#define IF_TYPE_FRAMERELAY_SERVICE      44  // FRNETSERV-MIB
#define IF_TYPE_V35                     45
#define IF_TYPE_HSSI                    46
#define IF_TYPE_HIPPI                   47
#define IF_TYPE_MODEM                   48  // Generic Modem
#define IF_TYPE_AAL5                    49  // AAL5 over ATM
#define IF_TYPE_SONET_PATH              50
#define IF_TYPE_SONET_VT                51
#define IF_TYPE_SMDS_ICIP               52  // SMDS InterCarrier Interface
#define IF_TYPE_PROP_VIRTUAL            53  // Proprietary virtual/internal
#define IF_TYPE_PROP_MULTIPLEXOR        54  // Proprietary multiplexing
#define IF_TYPE_IEEE80212               55  // 100BaseVG
#define IF_TYPE_FIBRECHANNEL            56
#define IF_TYPE_HIPPIINTERFACE          57
#define IF_TYPE_FRAMERELAY_INTERCONNECT 58  // Obsolete, use 32 or 44
#define IF_TYPE_AFLANE_8023             59  // ATM Emulated LAN for 802.3
#define IF_TYPE_AFLANE_8025             60  // ATM Emulated LAN for 802.5
#define IF_TYPE_CCTEMUL                 61  // ATM Emulated circuit
#define IF_TYPE_FASTETHER               62  // Fast Ethernet (100BaseT)
#define IF_TYPE_ISDN                    63  // ISDN and X.25
#define IF_TYPE_V11                     64  // CCITT V.11/X.21
#define IF_TYPE_V36                     65  // CCITT V.36
#define IF_TYPE_G703_64K                66  // CCITT G703 at 64Kbps
#define IF_TYPE_G703_2MB                67  // Obsolete; see DS1-MIB
#define IF_TYPE_QLLC                    68  // SNA QLLC
#define IF_TYPE_FASTETHER_FX            69  // Fast Ethernet (100BaseFX)
#define IF_TYPE_CHANNEL                 70
#define IF_TYPE_IEEE80211               71  // Radio spread spectrum
#define IF_TYPE_IBM370PARCHAN           72  // IBM System 360/370 OEMI Channel
#define IF_TYPE_ESCON                   73  // IBM Enterprise Systems Connection
#define IF_TYPE_DLSW                    74  // Data Link Switching
#define IF_TYPE_ISDN_S                  75  // ISDN S/T interface
#define IF_TYPE_ISDN_U                  76  // ISDN U interface
#define IF_TYPE_LAP_D                   77  // Link Access Protocol D
#define IF_TYPE_IPSWITCH                78  // IP Switching Objects
#define IF_TYPE_RSRB                    79  // Remote Source Route Bridging
#define IF_TYPE_ATM_LOGICAL             80  // ATM Logical Port
#define IF_TYPE_DS0                     81  // Digital Signal Level 0
#define IF_TYPE_DS0_BUNDLE              82  // Group of ds0s on the same ds1
#define IF_TYPE_BSC                     83  // Bisynchronous Protocol
#define IF_TYPE_ASYNC                   84  // Asynchronous Protocol
#define IF_TYPE_CNR                     85  // Combat Net Radio
#define IF_TYPE_ISO88025R_DTR           86  // ISO 802.5r DTR
#define IF_TYPE_EPLRS                   87  // Ext Pos Loc Report Sys
#define IF_TYPE_ARAP                    88  // Appletalk Remote Access Protocol
#define IF_TYPE_PROP_CNLS               89  // Proprietary Connectionless Proto
#define IF_TYPE_HOSTPAD                 90  // CCITT-ITU X.29 PAD Protocol
#define IF_TYPE_TERMPAD                 91  // CCITT-ITU X.3 PAD Facility
#define IF_TYPE_FRAMERELAY_MPI          92  // Multiproto Interconnect over FR
#define IF_TYPE_X213                    93  // CCITT-ITU X213
#define IF_TYPE_ADSL                    94  // Asymmetric Digital Subscrbr Loop
#define IF_TYPE_RADSL                   95  // Rate-Adapt Digital Subscrbr Loop
#define IF_TYPE_SDSL                    96  // Symmetric Digital Subscriber Loop
#define IF_TYPE_VDSL                    97  // Very H-Speed Digital Subscrb Loop
#define IF_TYPE_ISO88025_CRFPRINT       98  // ISO 802.5 CRFP
#define IF_TYPE_MYRINET                 99  // Myricom Myrinet
#define IF_TYPE_VOICE_EM                100 // Voice recEive and transMit
#define IF_TYPE_VOICE_FXO               101 // Voice Foreign Exchange Office
#define IF_TYPE_VOICE_FXS               102 // Voice Foreign Exchange Station
#define IF_TYPE_VOICE_ENCAP             103 // Voice encapsulation
#define IF_TYPE_VOICE_OVERIP            104 // Voice over IP encapsulation
#define IF_TYPE_ATM_DXI                 105 // ATM DXI
#define IF_TYPE_ATM_FUNI                106 // ATM FUNI
#define IF_TYPE_ATM_IMA                 107 // ATM IMA
#define IF_TYPE_PPPMULTILINKBUNDLE      108 // PPP Multilink Bundle
#define IF_TYPE_IPOVER_CDLC             109 // IBM ipOverCdlc
#define IF_TYPE_IPOVER_CLAW             110 // IBM Common Link Access to Workstn
#define IF_TYPE_STACKTOSTACK            111 // IBM stackToStack
#define IF_TYPE_VIRTUALIPADDRESS        112 // IBM VIPA
#define IF_TYPE_MPC                     113 // IBM multi-proto channel support
#define IF_TYPE_IPOVER_ATM              114 // IBM ipOverAtm
#define IF_TYPE_ISO88025_FIBER          115 // ISO 802.5j Fiber Token Ring
#define IF_TYPE_TDLC                    116 // IBM twinaxial data link control
#define IF_TYPE_GIGABITETHERNET         117
#define IF_TYPE_HDLC                    118
#define IF_TYPE_LAP_F                   119
#define IF_TYPE_V37                     120
#define IF_TYPE_X25_MLP                 121 // Multi-Link Protocol
#define IF_TYPE_X25_HUNTGROUP           122 // X.25 Hunt Group
#define IF_TYPE_TRANSPHDLC              123
#define IF_TYPE_INTERLEAVE              124 // Interleave channel
#define IF_TYPE_FAST                    125 // Fast channel
#define IF_TYPE_IP                      126 // IP (for APPN HPR in IP networks)
#define IF_TYPE_DOCSCABLE_MACLAYER      127 // CATV Mac Layer
#define IF_TYPE_DOCSCABLE_DOWNSTREAM    128 // CATV Downstream interface
#define IF_TYPE_DOCSCABLE_UPSTREAM      129 // CATV Upstream interface
#define IF_TYPE_A12MPPSWITCH            130 // Avalon Parallel Processor
#define IF_TYPE_TUNNEL                  131 // Encapsulation interface
#define IF_TYPE_COFFEE                  132 // Coffee pot
#define IF_TYPE_CES                     133 // Circuit Emulation Service
#define IF_TYPE_ATM_SUBINTERFACE        134 // ATM Sub Interface
#define IF_TYPE_L2_VLAN                 135 // Layer 2 Virtual LAN using 802.1Q
#define IF_TYPE_L3_IPVLAN               136 // Layer 3 Virtual LAN using IP
#define IF_TYPE_L3_IPXVLAN              137 // Layer 3 Virtual LAN using IPX
#define IF_TYPE_DIGITALPOWERLINE        138 // IP over Power Lines
#define IF_TYPE_MEDIAMAILOVERIP         139 // Multimedia Mail over IP
#define IF_TYPE_DTM                     140 // Dynamic syncronous Transfer Mode
#define IF_TYPE_DCN                     141 // Data Communications Network
#define IF_TYPE_IPFORWARD               142 // IP Forwarding Interface
#define IF_TYPE_MSDSL                   143 // Multi-rate Symmetric DSL
#define IF_TYPE_IEEE1394                144 // IEEE1394 High Perf Serial Bus
#define IF_TYPE_RECEIVE_ONLY            145 // TV adapter type

#define MAX_IF_TYPE                     145

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Access types                                                             //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#define IF_ACCESS_LOOPBACK              1
#define IF_ACCESS_BROADCAST             2
#define IF_ACCESS_POINTTOPOINT          3
#define IF_ACCESS_POINTTOMULTIPOINT     4

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Interface Capabilities (bit flags)                                       //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#define IF_CHECK_NONE                   0x00
#define IF_CHECK_MCAST                  0x01
#define IF_CHECK_SEND                   0x02


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Connection Types                                                         //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#define IF_CONNECTION_DEDICATED         1
#define IF_CONNECTION_PASSIVE           2
#define IF_CONNECTION_DEMAND            3


#define IF_ADMIN_STATUS_UP              1
#define IF_ADMIN_STATUS_DOWN            2
#define IF_ADMIN_STATUS_TESTING         3

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// The following are the the operational states for WAN and LAN interfaces. //
// The order of the states seems weird, but is done for a purpose. All      //
// states >= CONNECTED can transmit data right away. States >= DISCONNECTED //
// can tx data but some set up might be needed. States < DISCONNECTED can   //
// not transmit data.                                                       //
// A card is marked UNREACHABLE if DIM calls InterfaceUnreachable for       //
// reasons other than failure to connect.                                   //
//                                                                          //
// NON_OPERATIONAL -- Valid for LAN Interfaces. Means the card is not       //
//                      working or not plugged in or has no address.        //
// UNREACHABLE     -- Valid for WAN Interfaces. Means the remote site is    //
//                      not reachable at this time.                         //
// DISCONNECTED    -- Valid for WAN Interfaces. Means the remote site is    //
//                      not connected at this time.                         //
// CONNECTING      -- Valid for WAN Interfaces. Means a connection attempt  //
//                      has been initiated to the remote site.              //
// CONNECTED       -- Valid for WAN Interfaces. Means the remote site is    //
//                      connected.                                          //
// OPERATIONAL     -- Valid for LAN Interfaces. Means the card is plugged   //
//                      in and working.                                     //
//                                                                          //
// It is the users duty to convert these values to MIB-II values if they    //
// are to be used by a subagent                                             //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#define IF_OPER_STATUS_NON_OPERATIONAL  0
#define IF_OPER_STATUS_UNREACHABLE      1
#define IF_OPER_STATUS_DISCONNECTED     2
#define IF_OPER_STATUS_CONNECTING       3
#define IF_OPER_STATUS_CONNECTED        4
#define IF_OPER_STATUS_OPERATIONAL      5

#define MIB_IF_TYPE_OTHER               1
#define MIB_IF_TYPE_ETHERNET            6
#define MIB_IF_TYPE_TOKENRING           9
#define MIB_IF_TYPE_FDDI                15
#define MIB_IF_TYPE_PPP                 23
#define MIB_IF_TYPE_LOOPBACK            24
#define MIB_IF_TYPE_SLIP                28

#define MIB_IF_ADMIN_STATUS_UP          1
#define MIB_IF_ADMIN_STATUS_DOWN        2
#define MIB_IF_ADMIN_STATUS_TESTING     3

#define MIB_IF_OPER_STATUS_NON_OPERATIONAL      0
#define MIB_IF_OPER_STATUS_UNREACHABLE          1
#define MIB_IF_OPER_STATUS_DISCONNECTED         2
#define MIB_IF_OPER_STATUS_CONNECTING           3
#define MIB_IF_OPER_STATUS_CONNECTED            4
#define MIB_IF_OPER_STATUS_OPERATIONAL          5

#ifdef __cplusplus
}
#endif

#endif //__ROUTING_IPIFCONS_H__

