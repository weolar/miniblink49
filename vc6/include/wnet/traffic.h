/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    traffic.h

Abstract:

    This module contains API definitions for the traffic control interface.

--*/

#ifndef __TRAFFIC_H
#define __TRAFFIC_H

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern  "C" {
#endif

//---------------------------------------------------------------------------
//
// Define's
//
#define CURRENT_TCI_VERSION       	0x0002


//
// Definitions of notification events. These may be passed
// to the client's notification handler, to identify the
// notification type
//

//
// A TC interface has come up
//
#define TC_NOTIFY_IFC_UP		1
//
// A TC interface has come down
//
#define TC_NOTIFY_IFC_CLOSE		2
//
// A change on a TC interface, typically a change in the
// list of supported network addresses
//
#define TC_NOTIFY_IFC_CHANGE	3
//
// A TC parameter has changed
//
#define TC_NOTIFY_PARAM_CHANGED	4
//
// A flow has been closed by the TC interface
// for example: after a remote call close, or the whole interface
// is going down
//
#define TC_NOTIFY_FLOW_CLOSE		5

#define TC_INVALID_HANDLE	((HANDLE)0)

#define MAX_STRING_LENGTH	256


//---------------------------------------------------------------------------
//
// Typedef's and structures
//

#ifndef CALLBACK
#define CALLBACK __stdcall
#endif

#ifndef APIENTRY
#define APIENTRY FAR __stdcall
#endif

//
// Handlers registered by the TCI client
//

typedef
VOID (CALLBACK * TCI_NOTIFY_HANDLER)(
    IN	HANDLE		ClRegCtx,
    IN	HANDLE		ClIfcCtx,
	IN	ULONG		Event, 		// See list below
	IN	HANDLE	    SubCode,
	IN	ULONG		BufSize,
	IN	PVOID		Buffer
	);

typedef
VOID (CALLBACK * TCI_ADD_FLOW_COMPLETE_HANDLER)(
	IN HANDLE 	ClFlowCtx,
	IN ULONG	Status
    );

typedef
VOID (CALLBACK * TCI_MOD_FLOW_COMPLETE_HANDLER)(
	IN HANDLE 	ClFlowCtx,
	IN ULONG	Status
    );

typedef
VOID (CALLBACK * TCI_DEL_FLOW_COMPLETE_HANDLER)(
	IN HANDLE 	ClFlowCtx,
	IN ULONG	Status
    );


typedef struct _TCI_CLIENT_FUNC_LIST {

    TCI_NOTIFY_HANDLER				ClNotifyHandler;
    TCI_ADD_FLOW_COMPLETE_HANDLER	ClAddFlowCompleteHandler;
    TCI_MOD_FLOW_COMPLETE_HANDLER	ClModifyFlowCompleteHandler;
    TCI_DEL_FLOW_COMPLETE_HANDLER	ClDeleteFlowCompleteHandler;

} TCI_CLIENT_FUNC_LIST, *PTCI_CLIENT_FUNC_LIST;


//
// Network address descriptor
//
typedef struct _ADDRESS_LIST_DESCRIPTOR {

    ULONG					MediaType;
    NETWORK_ADDRESS_LIST	AddressList;

} ADDRESS_LIST_DESCRIPTOR, *PADDRESS_LIST_DESCRIPTOR;


//
// An interface ID that is returned by the enumerator
//
typedef struct _TC_IFC_DESCRIPTOR {

    ULONG					Length;
    LPWSTR					pInterfaceName;
    LPWSTR					pInterfaceID;
	ADDRESS_LIST_DESCRIPTOR	AddressListDesc;

} TC_IFC_DESCRIPTOR, *PTC_IFC_DESCRIPTOR;


//
// This structure is returned by a QoS data provider in reply to
// GUID_QOS_SUPPORTED query or with an interface UP notification
//
typedef struct _TC_SUPPORTED_INFO_BUFFER {

    USHORT						InstanceIDLength;
    // device or interface ID
    WCHAR						InstanceID[MAX_STRING_LENGTH];
    // address list
    ADDRESS_LIST_DESCRIPTOR		AddrListDesc;

} TC_SUPPORTED_INFO_BUFFER, *PTC_SUPPORTED_INFO_BUFFER;


//
// Filters are used to match packets. The Pattern field
// indicates the values to which bits in corresponding
// positions in candidate packets should be compared. The
// Mask field indicates which bits are to be compared and
// which bits are don't cares.
//
// Different filters can be submitted on the TCI interface.
// The generic filter structure is defined to include an
// AddressType, which indicates the specific type of filter to
// follow.
//

typedef struct _TC_GEN_FILTER {

    USHORT	AddressType;	// IP, IPX, etc.
    ULONG	PatternSize;	// byte count of the pattern
	PVOID  	Pattern;		// specific format, e.g. IP_PATTERN
	PVOID  	Mask;			// same type as Pattern

} TC_GEN_FILTER, *PTC_GEN_FILTER;


//
// A generic flow includes two flowspecs and a freeform
// buffer which contains flow specific TC objects.
//
typedef struct _TC_GEN_FLOW {
	
    FLOWSPEC		SendingFlowspec;
	FLOWSPEC		ReceivingFlowspec;
	ULONG			TcObjectsLength;		// number of optional bytes
	QOS_OBJECT_HDR	TcObjects[1];

} TC_GEN_FLOW, *PTC_GEN_FLOW;


//
// Format of specific pattern or mask used by GPC for the IP protocol
//
typedef struct _IP_PATTERN {

    ULONG		Reserved1;
    ULONG		Reserved2;

    ULONG		SrcAddr;
    ULONG		DstAddr;

    union {
        struct { USHORT s_srcport,s_dstport; } S_un_ports;
        struct { UCHAR s_type,s_code; USHORT filler; } S_un_icmp;
        ULONG	S_Spi;
    } S_un;

    UCHAR		ProtocolId;
    UCHAR		Reserved3[3];

#define tcSrcPort S_un.S_un_ports.s_srcport
#define tcDstPort S_un.S_un_ports.s_dstport
#define tcIcmpType        S_un.S_un_icmp.s_type
#define tcIcmpCode        S_un.S_un_icmp.s_code
#define tcSpi             S_un.S_Spi

} IP_PATTERN, *PIP_PATTERN;

//
// Format of specific pattern or mask used by GPC for the IPX protocol
//
typedef struct _IPX_PATTERN {

    struct {
        ULONG 	NetworkAddress;
        UCHAR 	NodeAddress[6];
        USHORT 	Socket;
    } Src, Dest;

} IPX_PATTERN, *PIPX_PATTERN;


//
// The enumeration buffer is the flow parameters + a list of filters
//
typedef struct _ENUMERATION_BUFFER {

	ULONG			Length;
	ULONG			OwnerProcessId;
    USHORT			FlowNameLength;
    WCHAR			FlowName[MAX_STRING_LENGTH];
	PTC_GEN_FLOW	pFlow;
	ULONG			NumberOfFilters;
	TC_GEN_FILTER	GenericFilter[1]; 	// one for each filter

} ENUMERATION_BUFFER, *PENUMERATION_BUFFER;

//
// QoS objects supported by traffic
//
#define QOS_TRAFFIC_GENERAL_ID_BASE 4000

#define QOS_OBJECT_DS_CLASS                    (0x00000001 + QOS_TRAFFIC_GENERAL_ID_BASE)
        /* QOS_DS_CLASS structure passed */
#define QOS_OBJECT_TRAFFIC_CLASS               (0x00000002 + QOS_TRAFFIC_GENERAL_ID_BASE) 
          /* QOS_Traffic class structure passed */
#define   QOS_OBJECT_DIFFSERV                  (0x00000003 + QOS_TRAFFIC_GENERAL_ID_BASE)
          /* QOS_DIFFSERV Structure */
#define QOS_OBJECT_TCP_TRAFFIC                 (0x00000004 + QOS_TRAFFIC_GENERAL_ID_BASE)
        /* QOS_TCP_TRAFFIC structure */
#define QOS_OBJECT_FRIENDLY_NAME               (0x00000005 + QOS_TRAFFIC_GENERAL_ID_BASE)
        /* QOS_FRIENDLY_NAME structure */


//
// This structure is used to associate a friendly name with the flow
// 

typedef struct _QOS_FRIENDLY_NAME {
    QOS_OBJECT_HDR ObjectHdr;
    WCHAR          FriendlyName[MAX_STRING_LENGTH];
} QOS_FRIENDLY_NAME, *LPQOS_FRIENDLY_NAME;

//
// This structure may carry an 802.1 TrafficClass parameter which 
// has been provided to the host by a layer 2 network, for example, 
// in an 802.1 extended RSVP RESV message. If this object is obtained
// from the network, hosts will stamp the MAC headers of corresponding
// transmitted packets, with the value in the object. Otherwise, hosts
// may select a value based on the standard Intserv mapping of 
// ServiceType to 802.1 TrafficClass.
//
//

typedef struct _QOS_TRAFFIC_CLASS {

    QOS_OBJECT_HDR   ObjectHdr;
    ULONG            TrafficClass;

} QOS_TRAFFIC_CLASS, *LPQOS_TRAFFIC_CLASS;

//
// This structure may carry an DSField parameter which  has been provided to 
// the host by a layer 3 network, for example, in an extended RSVP RESV message. 
// If this object is obtained from the network, hosts will stamp the DS Field on the
// IP header of transmitted packets, with the value in the object. Otherwise, hosts
// may select a value based on the standard Intserv mapping of ServiceType to DS Field 
//

typedef struct _QOS_DS_CLASS {

    QOS_OBJECT_HDR ObjectHdr;
    ULONG          DSField;

} QOS_DS_CLASS, *LPQOS_DS_CLASS;


//
// This structure is used to create DiffServ Flows. This creates flows in the packet scheduler
// and allows it to classify to packets based on a particular DS field. This structure takes
// a variable length array of QOS_DIFFSERV_RULE, where each DS field is specified by a 
// QOS_DIFFSERV_RULE
//
typedef struct _QOS_DIFFSERV {

    QOS_OBJECT_HDR ObjectHdr;
    ULONG          DSFieldCount;
    UCHAR          DiffservRule[1];
} QOS_DIFFSERV, *LPQOS_DIFFSERV;

//
// The rule for a Diffserv DS codepoint. 
//
typedef struct _QOS_DIFFSERV_RULE {
    UCHAR InboundDSField;
    UCHAR ConformingOutboundDSField;
    UCHAR NonConformingOutboundDSField;
    UCHAR ConformingUserPriority;
    UCHAR NonConformingUserPriority;
} QOS_DIFFSERV_RULE, *LPQOS_DIFFSERV_RULE;

// 
// This structure is passed to indicate that the IP Precedence and UserPriority mappings for the flow
// have to be set to the system defaults for TCP traffic. If this object is passed, 
// the ServiceType ==> DSField mapping, ServiceType ==> UserPriorityMapping, QOS_OBJECT_DS_CLASS
// and QOS_OBJECT_TRAFFIC_CLASS will be ignored.
//

typedef struct _QOS_TCP_TRAFFIC {
    QOS_OBJECT_HDR ObjectHdr;
} QOS_TCP_TRAFFIC, *LPQOS_TCP_TRAFFIC;



	

//---------------------------------------------------------------------------
//
// Interface Function Definitions
//

ULONG
APIENTRY
TcRegisterClient(
	IN		ULONG					TciVersion,
    IN		HANDLE					ClRegCtx,
	IN		PTCI_CLIENT_FUNC_LIST	ClientHandlerList,
	OUT		PHANDLE					pClientHandle
    );

ULONG
APIENTRY
TcEnumerateInterfaces(
	IN 		HANDLE				ClientHandle,
	IN OUT	PULONG				pBufferSize,
	OUT		PTC_IFC_DESCRIPTOR 	InterfaceBuffer
    );

ULONG
APIENTRY
TcOpenInterfaceA(
	IN		LPSTR			pInterfaceName,
	IN 		HANDLE			ClientHandle,
	IN		HANDLE			ClIfcCtx,
	OUT		PHANDLE			pIfcHandle
    );

ULONG
APIENTRY
TcOpenInterfaceW(
	IN		LPWSTR			pInterfaceName,
	IN 		HANDLE			ClientHandle,
	IN		HANDLE			ClIfcCtx,
	OUT		PHANDLE			pIfcHandle
    );

ULONG
APIENTRY
TcCloseInterface(
    IN 		HANDLE       IfcHandle
    );

ULONG
APIENTRY
TcQueryInterface(
	IN 		HANDLE 		IfcHandle,
    IN		LPGUID		pGuidParam,
    IN		BOOLEAN		NotifyChange,
    IN OUT	PULONG		pBufferSize,
    OUT		PVOID		Buffer
    );

ULONG
APIENTRY
TcSetInterface(
	IN 		HANDLE 		IfcHandle,
    IN		LPGUID		pGuidParam,
    IN 		ULONG		BufferSize,
    IN		PVOID		Buffer
    );

ULONG
APIENTRY
TcQueryFlowA(
	IN 		LPSTR		pFlowName,
    IN		LPGUID		pGuidParam,
    IN OUT	PULONG		pBufferSize,
    OUT		PVOID		Buffer
    );

ULONG
APIENTRY
TcQueryFlowW(
	IN 		LPWSTR		pFlowName,
    IN		LPGUID		pGuidParam,
    IN OUT	PULONG		pBufferSize,
    OUT		PVOID		Buffer
    );

ULONG
APIENTRY
TcSetFlowA(
	IN 		LPSTR		pFlowName,
    IN		LPGUID		pGuidParam,
    IN 		ULONG		BufferSize,
    IN		PVOID		Buffer
    );

ULONG
APIENTRY
TcSetFlowW(
	IN 		LPWSTR		pFlowName,
    IN		LPGUID		pGuidParam,
    IN 		ULONG		BufferSize,
    IN		PVOID		Buffer
    );

ULONG
APIENTRY
TcAddFlow(
	IN  	HANDLE	     	IfcHandle,
    IN		HANDLE			ClFlowCtx,
    IN		ULONG			Flags,
    IN		PTC_GEN_FLOW	pGenericFlow,
    OUT		PHANDLE			pFlowHandle
);

ULONG
APIENTRY
TcGetFlowNameA(
	IN		HANDLE			FlowHandle,
    IN		ULONG			StrSize,
    OUT		LPSTR			pFlowName
);

ULONG
APIENTRY
TcGetFlowNameW(
	IN		HANDLE			FlowHandle,
    IN		ULONG			StrSize,
    OUT		LPWSTR			pFlowName
);

ULONG
APIENTRY
TcModifyFlow(	
	IN		HANDLE			FlowHandle,
    IN		PTC_GEN_FLOW	pGenericFlow
    );

ULONG
APIENTRY
TcAddFilter(
	IN		HANDLE			FlowHandle,
    IN		PTC_GEN_FILTER	pGenericFilter,
    OUT		PHANDLE			pFilterHandle
    );


ULONG
APIENTRY
TcDeregisterClient(
	IN 		HANDLE 			ClientHandle
    );


ULONG
APIENTRY
TcDeleteFlow(
    IN 		HANDLE  		FlowHandle
    );

ULONG
APIENTRY
TcDeleteFilter(
    IN 		HANDLE  		FilterHandle
    );

ULONG
APIENTRY
TcEnumerateFlows(	
    IN		HANDLE					IfcHandle,
	IN OUT 	PHANDLE					pEnumHandle,
    IN OUT	PULONG					pFlowCount,
    IN OUT	PULONG					pBufSize,
    OUT		PENUMERATION_BUFFER		Buffer
    );

#ifdef UNICODE

#define TcOpenInterface					TcOpenInterfaceW
#define TcQueryFlow						TcQueryFlowW
#define TcSetFlow						TcSetFlowW
#define TcGetFlowName					TcGetFlowNameW

#else	// UNICODE

#define TcOpenInterface		TcOpenInterfaceA
#define TcQueryFlow			TcQueryFlowA
#define TcSetFlow			TcSetFlowA
#define TcGetFlowName		TcGetFlowNameA

#endif	// UNICODE


#ifdef __cplusplus
}
#endif


#endif


