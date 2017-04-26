
#ifndef _WINSOCK2API_
#define _WINSOCK2API_

#include <winsock.h>
#include <ipexport.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _WSABUF {
    u_long      len;     /* the length of the buffer */
    char FAR *  buf;     /* the pointer to the buffer */
} WSABUF, FAR * LPWSABUF;

typedef struct _OVERLAPPED *    LPWSAOVERLAPPED;
#define WSAOVERLAPPED           OVERLAPPED

typedef
void
(CALLBACK * LPWSAOVERLAPPED_COMPLETION_ROUTINE)(
    IN DWORD dwError,
    IN DWORD cbTransferred,
    IN LPWSAOVERLAPPED lpOverlapped,
    IN DWORD dwFlags
    );

#define _SS_MAXSIZE 128                 // Maximum size
#define _SS_ALIGNSIZE (sizeof(__int64)) // Desired alignment

#define _SS_PAD1SIZE (_SS_ALIGNSIZE - sizeof (short))
#define _SS_PAD2SIZE (_SS_MAXSIZE - (sizeof (short) + _SS_PAD1SIZE + _SS_ALIGNSIZE))

typedef USHORT ADDRESS_FAMILY;

typedef struct sockaddr_storage {
    ADDRESS_FAMILY ss_family;      // address family

    CHAR __ss_pad1[_SS_PAD1SIZE];  // 6 byte pad, this is to make
                                   //   implementation specific pad up to
                                   //   alignment field that follows explicit
                                   //   in the data structure
    __int64 __ss_align;            // Field to force desired structure
    CHAR __ss_pad2[_SS_PAD2SIZE];  // 112 byte pad to achieve desired size;
                                   //   _SS_MAXSIZE value minus size of
                                   //   ss_family, __ss_pad1, and
                                   //   __ss_align fields is 112
} SOCKADDR_STORAGE_LH, *PSOCKADDR_STORAGE_LH, FAR *LPSOCKADDR_STORAGE_LH;

#define WSAAPI                  PASCAL

#define NI_MAXHOST      1025  /* Max size of a fully-qualified domain name */
#define NI_MAXSERV      32    /* Max size of a service name */

typedef struct {
    union {
        struct {
            ULONG Zone : 28;
            ULONG Level : 4;
        };
        ULONG Value;
    };
} SCOPE_ID, *PSCOPE_ID;

typedef struct sockaddr_in6 {
    ADDRESS_FAMILY sin6_family; // AF_INET6.
    USHORT sin6_port;           // Transport level port number.
    ULONG  sin6_flowinfo;       // IPv6 flow information.
    IN6_ADDR sin6_addr;         // IPv6 address.
    union {
        ULONG sin6_scope_id;     // Set of interfaces for a scope.
        SCOPE_ID sin6_scope_struct;
    };
} SOCKADDR_IN6_LH, *PSOCKADDR_IN6_LH, *LPSOCKADDR_IN6_LH;

#define MAX_PROTOCOL_CHAIN 7

#define BASE_PROTOCOL      1
#define LAYERED_PROTOCOL   0

typedef struct _WSAPROTOCOLCHAIN {
    int ChainLen;                                 /* the length of the chain,     */
                                                  /* length = 0 means layered protocol, */
                                                  /* length = 1 means base protocol, */
                                                  /* length > 1 means protocol chain */
    DWORD ChainEntries[MAX_PROTOCOL_CHAIN];       /* a list of dwCatalogEntryIds */
} WSAPROTOCOLCHAIN, FAR * LPWSAPROTOCOLCHAIN;

#define WSAPROTOCOL_LEN 255

typedef struct _WSAPROTOCOL_INFOW {
    DWORD dwServiceFlags1;
    DWORD dwServiceFlags2;
    DWORD dwServiceFlags3;
    DWORD dwServiceFlags4;
    DWORD dwProviderFlags;
    GUID ProviderId;
    DWORD dwCatalogEntryId;
    WSAPROTOCOLCHAIN ProtocolChain;
    int iVersion;
    int iAddressFamily;
    int iMaxSockAddr;
    int iMinSockAddr;
    int iSocketType;
    int iProtocol;
    int iProtocolMaxOffset;
    int iNetworkByteOrder;
    int iSecurityScheme;
    DWORD dwMessageSize;
    DWORD dwProviderReserved;
    WCHAR  szProtocol[WSAPROTOCOL_LEN + 1];
} WSAPROTOCOL_INFOW, FAR * LPWSAPROTOCOL_INFOW;

#define MAPVK_VK_TO_VSC     (0)

#define AF_INET6        23              /* Internetwork Version 6 */

typedef struct _SOCKET_ADDRESS {
    LPSOCKADDR lpSockaddr;
    INT iSockaddrLength;
} SOCKET_ADDRESS, *PSOCKET_ADDRESS, FAR * LPSOCKET_ADDRESS;

typedef int socklen_t;

#define INET6_ADDRSTRLEN 65

struct addrinfo
{
	int              ai_flags;
	int              ai_family;
	int              ai_socktype;
	int              ai_protocol;
	socklen_t   ai_addrlen;   /* Follow rfc3493 struct addrinfo */
	char            *ai_canonname;
	struct sockaddr *ai_addr;
	struct addrinfo *ai_next;
};
#define NI_NAMEREQD     0x04  /* Error if the host's name not in DNS */
#define AI_ADDRCONFIG               0x00000400  // Resolution only if global address configured
#define AI_V4MAPPED                 0x00000800  // On v6 failure, query v4 and convert to V4MAPPED format
#define WSA_NOT_ENOUGH_MEMORY	8L

#define WSA_IO_PENDING          997L

#define XP1_IFS_HANDLES                     0x00020000
#define SO_PROTOCOL_INFOA 0x2004      /* WSAPROTOCOL_INFOA structure */
#define SO_PROTOCOL_INFOW 0x2005      /* WSAPROTOCOL_INFOW structure */

#define IPPROTO_IPV6 41 // IPv6 header
#define IPV6_MULTICAST_HOPS   10 // IP multicast hop limit.

struct ipv6_mreq {
	IN6_ADDR ipv6mr_multiaddr;  // IPv6 multicast address.
	ULONG ipv6mr_interface;     // Interface index.
};



__checkReturn
SOCKET
WSAAPI
WSASocketW(
	__in int af,
	__in int type,
	__in int protocol,
	__in_opt LPWSAPROTOCOL_INFOW lpProtocolInfo,
	__in unsigned int g,
	__in DWORD dwFlags
);
#define FROM_PROTOCOL_INFO (-1)
#define WSA_FLAG_OVERLAPPED           0x01

#define VALIDATE_MULTICAST_LOOP(value) (1)
#define IPV6_MULTICAST_IF      9 // IP multicast interface.
#define IPV6_DROP_MEMBERSHIP  13 // Drop an IP group membership.
#define IPV6_ADD_MEMBERSHIP   12 // Add an IP group membership.

int
WSAAPI
WSARecvFrom(
	__in SOCKET s,
	__in_ecount(dwBufferCount) LPWSABUF lpBuffers,
	__in DWORD dwBufferCount,
	__out_opt LPDWORD lpNumberOfBytesRecvd,
	__inout LPDWORD lpFlags,
	__out_bcount_part_opt(*lpFromlen, *lpFromlen) struct sockaddr FAR * lpFrom,
	__inout_opt LPINT lpFromlen,
	__inout_opt LPWSAOVERLAPPED lpOverlapped,
	__in_opt LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
);

int
WSAAPI
WSARecv(
	__in SOCKET s,
	__in_ecount(dwBufferCount) LPWSABUF lpBuffers,
	__in DWORD dwBufferCount,
	__out_opt LPDWORD lpNumberOfBytesRecvd,
	__inout LPDWORD lpFlags,
	__inout_opt LPWSAOVERLAPPED lpOverlapped,
	__in_opt LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
);
int WSAAPI WSASendTo(
	SOCKET s,
	LPWSABUF lpBuffers,
	DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesSent,
	int iFlags,
	LPVOID lpTo,
	int iToLen,
	LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
);
int WSAAPI WSASend(
	SOCKET s,
	LPWSABUF lpBuffers,
	DWORD dwBufferCount,
	LPDWORD lpNumberOfBytesSent,
	DWORD dwFlags,
	LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine
);
int WSAAPI WSAIoctl(SOCKET s,
	DWORD dwIoControlCode,
	LPVOID lpvInBuffer,
	DWORD cbInBuffer,
	LPVOID lpvOutBuffer,
	DWORD cbOutBuffer,
	LPDWORD lpcbBytesReturned,
	LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE
	lpCompletionRoutine);
int
WSAAPI
WSADuplicateSocketW(
	__in SOCKET s,
	__in DWORD dwProcessId,
	__out LPWSAPROTOCOL_INFOW lpProtocolInfo
);
#define SD_RECEIVE      0x00
#define SD_SEND         0x01
#define SD_BOTH         0x02

#define IPV6_MULTICAST_LOOP   11 // IP multicast loopback.
#define IOC_WSK                       (IOC_WS2|0x07000000)
#define IOC_WS2                       0x08000000

#define _WSAIO(x,y)                   (IOC_VOID|(x)|(y))
#define _WSAIOR(x,y)                  (IOC_OUT|(x)|(y))
#define _WSAIOW(x,y)                  (IOC_IN|(x)|(y))
#define _WSAIORW(x,y)                 (IOC_INOUT|(x)|(y))


#define SIO_GET_EXTENSION_FUNCTION_POINTER  _WSAIORW(IOC_WS2,6)
#ifdef __cplusplus
}
#endif

#endif // _WINSOCK2API_


