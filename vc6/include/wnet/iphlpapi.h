/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    iphlpapi.h

Abstract:
    Header file for functions to interact with the IP Stack for MIB-II and
    related functionality

--*/

#ifndef __IPHLPAPI_H__
#define __IPHLPAPI_H__

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// IPRTRMIB.H has the definitions of the strcutures used to set and get     //
// information                                                              //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

#include <iprtrmib.h>
#include <ipexport.h>
#include <iptypes.h>

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// The GetXXXTable APIs take a buffer and a size of buffer.  If the buffer  //
// is not large enough, the APIs return ERROR_INSUFFICIENT_BUFFER  and      //
// *pdwSize is the required buffer size                                     //
// The bOrder is a BOOLEAN, which if TRUE sorts the table according to      //
// MIB-II (RFC XXXX)                                                        //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Retrieves the number of interfaces in the system. These include LAN and  //
// WAN interfaces                                                           //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


DWORD
WINAPI
GetNumberOfInterfaces(
    OUT PDWORD  pdwNumIf
    );

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Gets the MIB-II ifEntry                                                  //
// The dwIndex field of the MIB_IFROW should be set to the index of the     //
// interface being queried                                                  //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

DWORD
WINAPI
GetIfEntry(
    IN OUT PMIB_IFROW   pIfRow
    );

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Gets the MIB-II IfTable                                                  //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

DWORD
WINAPI
GetIfTable(
    OUT    PMIB_IFTABLE pIfTable,
    IN OUT PULONG       pdwSize,
    IN     BOOL         bOrder
    );

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Gets the Interface to IP Address mapping                                 //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

DWORD
WINAPI
GetIpAddrTable(
    OUT    PMIB_IPADDRTABLE pIpAddrTable,
    IN OUT PULONG           pdwSize,
    IN     BOOL             bOrder
    );

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Gets the current IP Address to Physical Address (ARP) mapping            //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

DWORD
WINAPI
GetIpNetTable(
    OUT    PMIB_IPNETTABLE pIpNetTable,
    IN OUT PULONG          pdwSize,
    IN     BOOL            bOrder
    );

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Gets the IP Routing Table  (RFX XXXX)                                    //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

DWORD
WINAPI
GetIpForwardTable(
    OUT    PMIB_IPFORWARDTABLE pIpForwardTable,
    IN OUT PULONG              pdwSize,
    IN     BOOL                bOrder
    );

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Gets TCP Connection/UDP Listener Table                                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


typedef enum _TCP_TABLE_CLASS {
    TCP_TABLE_BASIC_LISTENER,
    TCP_TABLE_BASIC_CONNECTIONS,
    TCP_TABLE_BASIC_ALL,
    TCP_TABLE_OWNER_PID_LISTENER,
    TCP_TABLE_OWNER_PID_CONNECTIONS,
    TCP_TABLE_OWNER_PID_ALL,
    TCP_TABLE_OWNER_MODULE_LISTENER,
    TCP_TABLE_OWNER_MODULE_CONNECTIONS,
    TCP_TABLE_OWNER_MODULE_ALL
} TCP_TABLE_CLASS, *PTCP_TABLE_CLASS;

typedef enum _UDP_TABLE_CLASS {
    UDP_TABLE_BASIC,
    UDP_TABLE_OWNER_PID,
    UDP_TABLE_OWNER_MODULE
} UDP_TABLE_CLASS, *PUDP_TABLE_CLASS;

typedef enum _TCPIP_OWNER_MODULE_INFO_CLASS {
    TCPIP_OWNER_MODULE_INFO_BASIC
} TCPIP_OWNER_MODULE_INFO_CLASS, *PTCPIP_OWNER_MODULE_INFO_CLASS;
    
typedef struct _TCPIP_OWNER_MODULE_BASIC_INFO {
    PWCHAR pModuleName;
    PWCHAR pModulePath;
} TCPIP_OWNER_MODULE_BASIC_INFO, *PTCPIP_OWNER_MODULE_BASIC_INFO;
    
        
DWORD
WINAPI
GetTcpTable(
    OUT    PMIB_TCPTABLE pTcpTable,
    IN OUT PDWORD        pdwSize,
    IN     BOOL          bOrder
    );

DWORD
WINAPI
GetExtendedTcpTable(
    OUT    PVOID           pTcpTable,
    IN OUT PDWORD          pdwSize,
    IN     BOOL            bOrder,
    IN     ULONG           ulAf,
    IN     TCP_TABLE_CLASS TableClass,
    IN     ULONG           Reserved
    );

DWORD
WINAPI
GetOwnerModuleFromTcpEntry(
    IN     PMIB_TCPROW_OWNER_MODULE      pTcpEntry,
    IN     TCPIP_OWNER_MODULE_INFO_CLASS Class,
    OUT    PVOID                         pBuffer,
    IN OUT PDWORD                        pdwSize
    );

DWORD
WINAPI
GetOwnerModuleFromTcp6Entry(
    IN     PMIB_TCP6ROW_OWNER_MODULE     pTcpEntry,
    IN     TCPIP_OWNER_MODULE_INFO_CLASS Class,
    OUT    PVOID                         pBuffer,
    IN OUT PDWORD                        pdwSize
    );
    
DWORD
WINAPI
GetUdpTable(
    OUT    PMIB_UDPTABLE pUdpTable,
    IN OUT PDWORD        pdwSize,
    IN     BOOL          bOrder
    );

DWORD
WINAPI
GetExtendedUdpTable(
    OUT    PVOID           pUdpTable,
    IN OUT PDWORD          pdwSize,
    IN     BOOL            bOrder,
    IN     ULONG           ulAf,
    IN     UDP_TABLE_CLASS TableClass,
    IN     ULONG           Reserved
    );

DWORD
WINAPI
GetOwnerModuleFromUdpEntry(
    IN     PMIB_UDPROW_OWNER_MODULE      pUdpEntry,
    IN     TCPIP_OWNER_MODULE_INFO_CLASS Class,
    OUT    PVOID                         pBuffer,
    IN OUT PDWORD                        pdwSize
    );

DWORD
WINAPI
GetOwnerModuleFromUdp6Entry(
    IN     PMIB_UDP6ROW_OWNER_MODULE     pUdpEntry,
    IN     TCPIP_OWNER_MODULE_INFO_CLASS Class,
    OUT    PVOID                         pBuffer,
    IN OUT PDWORD                        pdwSize
    );

    
//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Gets IP/ICMP/TCP/UDP Statistics                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

DWORD
WINAPI
GetIpStatistics(
    OUT  PMIB_IPSTATS   pStats
    );

DWORD
WINAPI
GetIpStatisticsEx(
    OUT  PMIB_IPSTATS   pStats,
    IN   DWORD          dwFamily
    );

DWORD
WINAPI
GetIcmpStatistics(
    OUT PMIB_ICMP   pStats
    );

DWORD
WINAPI
GetIcmpStatisticsEx(
    OUT PMIB_ICMP_EX    pStats,
    IN  DWORD           dwFamily
    );

DWORD
WINAPI
GetTcpStatistics(
    OUT PMIB_TCPSTATS   pStats
    );

DWORD
WINAPI
GetTcpStatisticsEx(
    OUT PMIB_TCPSTATS   pStats,
    IN  DWORD           dwFamily
    );

DWORD
WINAPI
GetUdpStatistics(
    OUT PMIB_UDPSTATS   pStats
    );

DWORD
WINAPI
GetUdpStatisticsEx(
    OUT PMIB_UDPSTATS   pStats,
    IN  DWORD           dwFamily
    );

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Used to set the ifAdminStatus on an interface.  The only fields of the   //
// MIB_IFROW that are relevant are the dwIndex (index of the interface      //
// whose status needs to be set) and the dwAdminStatus which can be either  //
// MIB_IF_ADMIN_STATUS_UP or MIB_IF_ADMIN_STATUS_DOWN                       //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

DWORD
WINAPI
SetIfEntry(
    IN PMIB_IFROW pIfRow
    );

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Used to create, modify or delete a route.  In all cases the              //
// dwForwardIfIndex, dwForwardDest, dwForwardMask, dwForwardNextHop and     //
// dwForwardPolicy MUST BE SPECIFIED. Currently dwForwardPolicy is unused   //
// and MUST BE 0.                                                           //
// For a set, the complete MIB_IPFORWARDROW structure must be specified     //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

DWORD
WINAPI
CreateIpForwardEntry(
    IN PMIB_IPFORWARDROW pRoute
    );

DWORD
WINAPI
SetIpForwardEntry(
    IN PMIB_IPFORWARDROW pRoute
    );

DWORD
WINAPI
DeleteIpForwardEntry(
    IN PMIB_IPFORWARDROW pRoute
    );

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Used to set the ipForwarding to ON or OFF (currently only ON->OFF is     //
// allowed) and to set the defaultTTL.  If only one of the fields needs to  //
// be modified and the other needs to be the same as before the other field //
// needs to be set to MIB_USE_CURRENT_TTL or MIB_USE_CURRENT_FORWARDING as  //
// the case may be                                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////


DWORD
WINAPI
SetIpStatistics(
    IN PMIB_IPSTATS pIpStats
    );

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Used to set the defaultTTL.                                              //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

DWORD
WINAPI
SetIpTTL(
    UINT nTTL
    );

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Used to create, modify or delete an ARP entry.  In all cases the dwIndex //
// dwAddr field MUST BE SPECIFIED.                                          //
// For a set, the complete MIB_IPNETROW structure must be specified         //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

DWORD
WINAPI
CreateIpNetEntry(
    IN PMIB_IPNETROW    pArpEntry
    );

DWORD
WINAPI
SetIpNetEntry(
    IN PMIB_IPNETROW    pArpEntry
    );

DWORD
WINAPI
DeleteIpNetEntry(
    IN PMIB_IPNETROW    pArpEntry
    );

DWORD
WINAPI
FlushIpNetTable(
    IN DWORD   dwIfIndex
    );


//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Used to create or delete a Proxy ARP entry. The dwIndex is the index of  //
// the interface on which to PARP for the dwAddress.  If the interface is   //
// of a type that doesnt support ARP, e.g. PPP, then the call will fail     //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

DWORD
WINAPI
CreateProxyArpEntry(
    IN  DWORD   dwAddress,
    IN  DWORD   dwMask,
    IN  DWORD   dwIfIndex
    );

DWORD
WINAPI
DeleteProxyArpEntry(
    IN  DWORD   dwAddress,
    IN  DWORD   dwMask,
    IN  DWORD   dwIfIndex
    );

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Used to set the state of a TCP Connection. The only state that it can be //
// set to is MIB_TCP_STATE_DELETE_TCB.  The complete MIB_TCPROW structure   //
// MUST BE SPECIFIED                                                        //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

DWORD
WINAPI
SetTcpEntry(
    IN PMIB_TCPROW pTcpRow
    );


DWORD
WINAPI
GetInterfaceInfo(
    IN PIP_INTERFACE_INFO pIfTable,
    OUT PULONG            dwOutBufLen
    );

DWORD
WINAPI
GetUniDirectionalAdapterInfo(OUT PIP_UNIDIRECTIONAL_ADAPTER_ADDRESS pIPIfInfo,
                 OUT PULONG dwOutBufLen
                 );

#ifndef NHPALLOCATEANDGETINTERFACEINFOFROMSTACK_DEFINED
#define NHPALLOCATEANDGETINTERFACEINFOFROMSTACK_DEFINED

DWORD
WINAPI
NhpAllocateAndGetInterfaceInfoFromStack(
    OUT IP_INTERFACE_NAME_INFO **ppTable,
    OUT PDWORD                 pdwCount,
    IN BOOL                    bOrder,
    IN HANDLE                  hHeap,
    IN DWORD                   dwFlags
    );

#endif

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Gets the "best" outgoing interface for the specified destination address //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

DWORD
WINAPI
GetBestInterface(
    IN  IPAddr  dwDestAddr,
    OUT PDWORD  pdwBestIfIndex
    );

#pragma warning(push)
#pragma warning(disable:4115)
DWORD
WINAPI
GetBestInterfaceEx(
    IN  struct sockaddr *pDestAddr,
    OUT PDWORD           pdwBestIfIndex
    );
#pragma warning(pop)

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// Gets the best (longest matching prefix) route for the given destination  //
// If the source address is also specified (i.e. is not 0x00000000), and    //
// there are multiple "best" routes to the given destination, the returned  //
// route will be one that goes out over the interface which has an address  //
// that matches the source address                                          //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

DWORD
WINAPI
GetBestRoute(
    IN  DWORD               dwDestAddr,
    IN  DWORD               dwSourceAddr, OPTIONAL
    OUT PMIB_IPFORWARDROW   pBestRoute
    );

DWORD
WINAPI
NotifyAddrChange(
    OUT PHANDLE      Handle,
    IN  LPOVERLAPPED overlapped
    );


DWORD
WINAPI
NotifyRouteChange(
    OUT PHANDLE      Handle,
    IN  LPOVERLAPPED overlapped
    );

BOOL
WINAPI
CancelIPChangeNotify(
    IN  LPOVERLAPPED notifyOverlapped
    );

DWORD
NotifySecurityHealthChange(
    PHANDLE         pHandle,
    LPOVERLAPPED    pOverLapped,
    PULONG          SecurityHealthFlags
    );

BOOL 
CancelSecurityHealthChangeNotify(
    LPOVERLAPPED notifyOverlapped
    );

DWORD
WINAPI
GetAdapterIndex(
    IN LPWSTR  AdapterName,
    OUT PULONG IfIndex
    );

DWORD
WINAPI
AddIPAddress(
    IPAddr  Address,
    IPMask  IpMask,
    DWORD   IfIndex,
    PULONG  NTEContext,
    PULONG  NTEInstance
    );

DWORD
WINAPI
DeleteIPAddress(
    ULONG NTEContext
    );

DWORD
WINAPI
GetNetworkParams(
    PFIXED_INFO pFixedInfo, PULONG pOutBufLen
    );

DWORD
WINAPI
GetAdaptersInfo(
    PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen
    );

PIP_ADAPTER_ORDER_MAP 
WINAPI
GetAdapterOrderMap(
    VOID
    );

#ifdef _WINSOCK2API_

//
// The following functions require Winsock2.
//

DWORD
WINAPI
GetAdaptersAddresses(
    IN     ULONG                 Family,
    IN     DWORD                 Flags,
    IN     PVOID                 Reserved,
    OUT    PIP_ADAPTER_ADDRESSES pAdapterAddresses, 
    IN OUT PULONG                pOutBufLen
    );

#endif

DWORD
WINAPI
GetPerAdapterInfo(
    ULONG IfIndex, PIP_PER_ADAPTER_INFO pPerAdapterInfo, PULONG pOutBufLen
    );

DWORD
WINAPI
IpReleaseAddress(
    PIP_ADAPTER_INDEX_MAP  AdapterInfo
    );


DWORD
WINAPI
IpRenewAddress(
    PIP_ADAPTER_INDEX_MAP  AdapterInfo
    );

DWORD
WINAPI
SendARP(
    IPAddr DestIP,
    IPAddr SrcIP,
    PULONG pMacAddr,
    PULONG  PhyAddrLen
    );

BOOL
WINAPI
GetRTTAndHopCount(
    IPAddr DestIpAddress,
    PULONG HopCount,
    ULONG  MaxHops,
    PULONG RTT
    );

DWORD
WINAPI
GetFriendlyIfIndex(
    DWORD IfIndex
    );

DWORD
WINAPI
EnableRouter(
    HANDLE* pHandle,
    OVERLAPPED* pOverlapped
    );

DWORD
WINAPI
UnenableRouter(
    OVERLAPPED* pOverlapped,
    LPDWORD lpdwEnableCount OPTIONAL
    );
DWORD
WINAPI
DisableMediaSense(
    HANDLE *pHandle,
    OVERLAPPED *pOverLapped
    );

DWORD
WINAPI
RestoreMediaSense(
    OVERLAPPED* pOverlapped,
    LPDWORD lpdwEnableCount OPTIONAL
    );

DWORD
WINAPI
GetIpErrorString(
    IN IP_STATUS ErrorCode,
    OUT PWCHAR Buffer,
    IN OUT PDWORD Size
    );

#ifdef __cplusplus
}
#endif

#endif //__IPHLPAPI_H__

