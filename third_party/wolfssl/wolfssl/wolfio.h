/* io.h
 *
 * Copyright (C) 2006-2016 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */


#ifndef WOLFSSL_IO_H
#define WOLFSSL_IO_H

#ifdef __cplusplus
    extern "C" {
#endif

/* OCSP and CRL_IO require HTTP client */
#if defined(HAVE_OCSP) || defined(HAVE_CRL_IO)
    #ifndef HAVE_HTTP_CLIENT
        #define HAVE_HTTP_CLIENT
    #endif
#endif

#if !defined(WOLFSSL_USER_IO)
    /* Micrium uses NetSock I/O callbacks in wolfio.c */
    #if !defined(USE_WOLFSSL_IO) && !defined(MICRIUM)
        #define USE_WOLFSSL_IO
    #endif
#endif


#if defined(USE_WOLFSSL_IO) || defined(HAVE_HTTP_CLIENT)

#ifdef HAVE_LIBZ
    #include "zlib.h"
#endif

#ifndef USE_WINDOWS_API
    #ifdef WOLFSSL_LWIP
        /* lwIP needs to be configured to use sockets API in this mode */
        /* LWIP_SOCKET 1 in lwip/opt.h or in build */
        #include "lwip/sockets.h"
        #include <errno.h>
        #ifndef LWIP_PROVIDE_ERRNO
            #define LWIP_PROVIDE_ERRNO 1
        #endif
    #elif defined(FREESCALE_MQX)
        #include <posix.h>
        #include <rtcs.h>
    #elif defined(FREESCALE_KSDK_MQX)
        #include <rtcs.h>
    #elif defined(WOLFSSL_MDK_ARM) || defined(WOLFSSL_KEIL_TCP_NET)
        #if !defined(WOLFSSL_MDK_ARM)
            #include "cmsis_os.h"
            #include "rl_net.h"
        #else
            #include <rtl.h>
        #endif
        #include "errno.h"
        #define SOCKET_T int
    #elif defined(WOLFSSL_TIRTOS)
        #include <sys/socket.h>
    #elif defined(FREERTOS_TCP)
        #include "FreeRTOS_Sockets.h"
    #elif defined(WOLFSSL_IAR_ARM)
        /* nothing */
    #elif defined(WOLFSSL_VXWORKS)
        #include <sockLib.h>
        #include <errno.h>
    #elif defined(WOLFSSL_ATMEL)
        #include "socket/include/socket.h"
    #elif defined(INTIME_RTOS)
        #undef MIN
        #undef MAX
        #include <rt.h>
        #include <sys/types.h>
        #include <sys/socket.h>
        #include <netdb.h>
        #include <netinet/in.h>
        #include <io.h>
        /* <sys/socket.h> defines these, to avoid conflict, do undef */
        #undef SOCKADDR
        #undef SOCKADDR_IN
    #elif defined(WOLFSSL_PRCONNECT_PRO)
        #include <prconnect_pro/prconnect_pro.h>
        #include <sys/types.h>
        #include <errno.h>
        #include <unistd.h>
        #include <fcntl.h>
        #include <netdb.h>
        #include <sys/ioctl.h>
    #elif defined(WOLFSSL_SGX)
        #include <errno.h>
    #elif !defined(WOLFSSL_NO_SOCK)
        #include <sys/types.h>
        #include <errno.h>
        #ifndef EBSNET
            #include <unistd.h>
        #endif
        #include <fcntl.h>

        #if defined(HAVE_RTP_SYS)
            #include <socket.h>
        #elif defined(EBSNET)
            #include "rtipapi.h"  /* errno */
            #include "socket.h"
        #elif !defined(DEVKITPRO) && !defined(WOLFSSL_PICOTCP)
            #include <sys/socket.h>
            #include <arpa/inet.h>
            #include <netinet/in.h>
            #include <netdb.h>
            #ifdef __PPU
                #include <netex/errno.h>
            #else
                #include <sys/ioctl.h>
            #endif
        #endif
    #endif
#endif /* USE_WINDOWS_API */

#ifdef __sun
    #include <sys/filio.h>
#endif

#ifdef USE_WINDOWS_API
    /* no epipe yet */
    #ifndef WSAEPIPE
        #define WSAEPIPE       -12345
    #endif
    #define SOCKET_EWOULDBLOCK WSAEWOULDBLOCK
    #define SOCKET_EAGAIN      WSAETIMEDOUT
    #define SOCKET_ECONNRESET  WSAECONNRESET
    #define SOCKET_EINTR       WSAEINTR
    #define SOCKET_EPIPE       WSAEPIPE
    #define SOCKET_ECONNREFUSED WSAENOTCONN
    #define SOCKET_ECONNABORTED WSAECONNABORTED
    #define close(s) closesocket(s)
#elif defined(__PPU)
    #define SOCKET_EWOULDBLOCK SYS_NET_EWOULDBLOCK
    #define SOCKET_EAGAIN      SYS_NET_EAGAIN
    #define SOCKET_ECONNRESET  SYS_NET_ECONNRESET
    #define SOCKET_EINTR       SYS_NET_EINTR
    #define SOCKET_EPIPE       SYS_NET_EPIPE
    #define SOCKET_ECONNREFUSED SYS_NET_ECONNREFUSED
    #define SOCKET_ECONNABORTED SYS_NET_ECONNABORTED
#elif defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)
    #if MQX_USE_IO_OLD
        /* RTCS old I/O doesn't have an EWOULDBLOCK */
        #define SOCKET_EWOULDBLOCK  EAGAIN
        #define SOCKET_EAGAIN       EAGAIN
        #define SOCKET_ECONNRESET   RTCSERR_TCP_CONN_RESET
        #define SOCKET_EINTR        EINTR
        #define SOCKET_EPIPE        EPIPE
        #define SOCKET_ECONNREFUSED RTCSERR_TCP_CONN_REFUSED
        #define SOCKET_ECONNABORTED RTCSERR_TCP_CONN_ABORTED
    #else
        #define SOCKET_EWOULDBLOCK  NIO_EWOULDBLOCK
        #define SOCKET_EAGAIN       NIO_EAGAIN
        #define SOCKET_ECONNRESET   NIO_ECONNRESET
        #define SOCKET_EINTR        NIO_EINTR
        #define SOCKET_EPIPE        NIO_EPIPE
        #define SOCKET_ECONNREFUSED NIO_ECONNREFUSED
        #define SOCKET_ECONNABORTED NIO_ECONNABORTED
    #endif
#elif defined(WOLFSSL_MDK_ARM)|| defined(WOLFSSL_KEIL_TCP_NET)
    #if !defined(WOLFSSL_MDK_ARM)
        #define SOCKET_EWOULDBLOCK BSD_ERROR_WOULDBLOCK
        #define SOCKET_EAGAIN      BSD_ERROR_LOCKED
        #define SOCKET_ECONNRESET  BSD_ERROR_CLOSED
        #define SOCKET_EINTR       BSD_ERROR
        #define SOCKET_EPIPE       BSD_ERROR
        #define SOCKET_ECONNREFUSED BSD_ERROR
        #define SOCKET_ECONNABORTED BSD_ERROR
    #else
        #define SOCKET_EWOULDBLOCK SCK_EWOULDBLOCK
        #define SOCKET_EAGAIN      SCK_ELOCKED
        #define SOCKET_ECONNRESET  SCK_ECLOSED
        #define SOCKET_EINTR       SCK_ERROR
        #define SOCKET_EPIPE       SCK_ERROR
        #define SOCKET_ECONNREFUSED SCK_ERROR
        #define SOCKET_ECONNABORTED SCK_ERROR
    #endif
#elif defined(WOLFSSL_PICOTCP)
    #define SOCKET_EWOULDBLOCK  PICO_ERR_EAGAIN
    #define SOCKET_EAGAIN       PICO_ERR_EAGAIN
    #define SOCKET_ECONNRESET   PICO_ERR_ECONNRESET
    #define SOCKET_EINTR        PICO_ERR_EINTR
    #define SOCKET_EPIPE        PICO_ERR_EIO
    #define SOCKET_ECONNREFUSED PICO_ERR_ECONNREFUSED
    #define SOCKET_ECONNABORTED PICO_ERR_ESHUTDOWN
#elif defined(FREERTOS_TCP)
    #define SOCKET_EWOULDBLOCK FREERTOS_EWOULDBLOCK
    #define SOCKET_EAGAIN       FREERTOS_EWOULDBLOCK
    #define SOCKET_ECONNRESET   FREERTOS_SOCKET_ERROR
    #define SOCKET_EINTR        FREERTOS_SOCKET_ERROR
    #define SOCKET_EPIPE        FREERTOS_SOCKET_ERROR
    #define SOCKET_ECONNREFUSED FREERTOS_SOCKET_ERROR
    #define SOCKET_ECONNABORTED FREERTOS_SOCKET_ERROR
#else
    #define SOCKET_EWOULDBLOCK EWOULDBLOCK
    #define SOCKET_EAGAIN      EAGAIN
    #define SOCKET_ECONNRESET  ECONNRESET
    #define SOCKET_EINTR       EINTR
    #define SOCKET_EPIPE       EPIPE
    #define SOCKET_ECONNREFUSED ECONNREFUSED
    #define SOCKET_ECONNABORTED ECONNABORTED
#endif /* USE_WINDOWS_API */


#ifdef DEVKITPRO
    /* from network.h */
    int net_send(int, const void*, int, unsigned int);
    int net_recv(int, void*, int, unsigned int);
    #define SEND_FUNCTION net_send
    #define RECV_FUNCTION net_recv
#elif defined(WOLFSSL_LWIP)
    #define SEND_FUNCTION lwip_send
    #define RECV_FUNCTION lwip_recv
#elif defined(WOLFSSL_PICOTCP)
    #define SEND_FUNCTION pico_send
    #define RECV_FUNCTION pico_recv
#elif defined(FREERTOS_TCP)
    #define RECV_FUNCTION(a,b,c,d)  FreeRTOS_recv((Socket_t)(a),(void*)(b), (size_t)(c), (BaseType_t)(d))
    #define SEND_FUNCTION(a,b,c,d)  FreeRTOS_send((Socket_t)(a),(void*)(b), (size_t)(c), (BaseType_t)(d))
#elif defined(WOLFSSL_VXWORKS)
    #define SEND_FUNCTION send
    #define RECV_FUNCTION recv
#else
    #define SEND_FUNCTION send
    #define RECV_FUNCTION recv
    #if !defined(HAVE_SOCKADDR) && !defined(WOLFSSL_NO_SOCK)
        #define HAVE_SOCKADDR
    #endif
#endif

#ifdef USE_WINDOWS_API
    typedef unsigned int SOCKET_T;
#else
    typedef int SOCKET_T;
#endif

#ifndef WOLFSSL_NO_SOCK
    #ifndef XSOCKLENT
        #ifdef USE_WINDOWS_API
            #define XSOCKLENT int
        #else
            #define XSOCKLENT socklen_t
        #endif
    #endif

    /* Socket Addr Support */
    #ifdef HAVE_SOCKADDR
        typedef struct sockaddr         SOCKADDR;
        typedef struct sockaddr_storage SOCKADDR_S;
        typedef struct sockaddr_in      SOCKADDR_IN;
        #ifdef WOLFSSL_IPV6
            typedef struct sockaddr_in6 SOCKADDR_IN6;
        #endif
        typedef struct hostent          HOSTENT;
    #endif /* HAVE_SOCKADDR */

    #ifdef HAVE_GETADDRINFO
        typedef struct addrinfo         ADDRINFO;
    #endif
#endif /* WOLFSSL_NO_SOCK */


/* IO API's */
#ifdef HAVE_IO_TIMEOUT
    WOLFSSL_API  int wolfIO_SetBlockingMode(SOCKET_T sockfd, int non_blocking);
    WOLFSSL_API void wolfIO_SetTimeout(int to_sec);;
    WOLFSSL_API  int wolfIO_Select(SOCKET_T sockfd, int to_sec);
#endif
WOLFSSL_API  int wolfIO_TcpConnect(SOCKET_T* sockfd, const char* ip,
    unsigned short port, int to_sec);
WOLFSSL_API  int wolfIO_Send(SOCKET_T sd, char *buf, int sz, int wrFlags);
WOLFSSL_API  int wolfIO_Recv(SOCKET_T sd, char *buf, int sz, int rdFlags);

#endif /* USE_WOLFSSL_IO || HAVE_HTTP_CLIENT */


#if defined(USE_WOLFSSL_IO)
    /* default IO callbacks */
    WOLFSSL_API int EmbedReceive(WOLFSSL* ssl, char* buf, int sz, void* ctx);
    WOLFSSL_API int EmbedSend(WOLFSSL* ssl, char* buf, int sz, void* ctx);

    #ifdef WOLFSSL_DTLS
        WOLFSSL_API int EmbedReceiveFrom(WOLFSSL* ssl, char* buf, int sz, void*);
        WOLFSSL_API int EmbedSendTo(WOLFSSL* ssl, char* buf, int sz, void* ctx);
        WOLFSSL_API int EmbedGenerateCookie(WOLFSSL* ssl, unsigned char* buf,
                                           int sz, void*);
        #ifdef WOLFSSL_MULTICAST
            WOLFSSL_API int EmbedReceiveFromMcast(WOLFSSL* ssl,
                                                  char* buf, int sz, void*);
        #endif /* WOLFSSL_MULTICAST */
        #ifdef WOLFSSL_SESSION_EXPORT
            WOLFSSL_API int EmbedGetPeer(WOLFSSL* ssl, char* ip, int* ipSz,
                                                unsigned short* port, int* fam);
            WOLFSSL_API int EmbedSetPeer(WOLFSSL* ssl, char* ip, int ipSz,
                                                  unsigned short port, int fam);
        #endif /* WOLFSSL_SESSION_EXPORT */
    #endif /* WOLFSSL_DTLS */
#endif /* USE_WOLFSSL_IO */

#ifdef HAVE_OCSP
    WOLFSSL_API int wolfIO_HttpBuildRequestOcsp(const char* domainName,
        const char* path, int ocspReqSz, unsigned char* buf, int bufSize);
    WOLFSSL_API int wolfIO_HttpProcessResponseOcsp(int sfd,
        unsigned char** respBuf, unsigned char* httpBuf, int httpBufSz,
        void* heap);

    WOLFSSL_API int EmbedOcspLookup(void*, const char*, int, unsigned char*,
                                   int, unsigned char**);
    WOLFSSL_API void EmbedOcspRespFree(void*, unsigned char*);
#endif

#ifdef HAVE_CRL_IO
    WOLFSSL_API int wolfIO_HttpBuildRequestCrl(const char* url, int urlSz,
        const char* domainName, unsigned char* buf, int bufSize);
    WOLFSSL_API int wolfIO_HttpProcessResponseCrl(WOLFSSL_CRL* crl, int sfd,
        unsigned char* httpBuf, int httpBufSz);

    WOLFSSL_API int EmbedCrlLookup(WOLFSSL_CRL* crl, const char* url,
        int urlSz);
#endif


#if defined(HAVE_HTTP_CLIENT)
    WOLFSSL_API  int wolfIO_DecodeUrl(const char* url, int urlSz, char* outName,
        char* outPath, unsigned short* outPort);

    WOLFSSL_API  int wolfIO_HttpBuildRequest(const char* reqType,
        const char* domainName, const char* path, int pathLen, int reqSz,
        const char* contentType, unsigned char* buf, int bufSize);
    WOLFSSL_API  int wolfIO_HttpProcessResponse(int sfd, const char* appStr,
        unsigned char** respBuf, unsigned char* httpBuf, int httpBufSz,
        int dynType, void* heap);
#endif /* HAVE_HTTP_CLIENT */


/* I/O callbacks */
typedef int (*CallbackIORecv)(WOLFSSL *ssl, char *buf, int sz, void *ctx);
typedef int (*CallbackIOSend)(WOLFSSL *ssl, char *buf, int sz, void *ctx);
WOLFSSL_API void wolfSSL_SetIORecv(WOLFSSL_CTX*, CallbackIORecv);
WOLFSSL_API void wolfSSL_SetIOSend(WOLFSSL_CTX*, CallbackIOSend);

WOLFSSL_API void wolfSSL_SetIOReadCtx( WOLFSSL* ssl, void *ctx);
WOLFSSL_API void wolfSSL_SetIOWriteCtx(WOLFSSL* ssl, void *ctx);

WOLFSSL_API void* wolfSSL_GetIOReadCtx( WOLFSSL* ssl);
WOLFSSL_API void* wolfSSL_GetIOWriteCtx(WOLFSSL* ssl);

WOLFSSL_API void wolfSSL_SetIOReadFlags( WOLFSSL* ssl, int flags);
WOLFSSL_API void wolfSSL_SetIOWriteFlags(WOLFSSL* ssl, int flags);


#ifdef HAVE_NETX
    WOLFSSL_LOCAL int NetX_Receive(WOLFSSL *ssl, char *buf, int sz, void *ctx);
    WOLFSSL_LOCAL int NetX_Send(WOLFSSL *ssl, char *buf, int sz, void *ctx);

    WOLFSSL_API void wolfSSL_SetIO_NetX(WOLFSSL* ssl, NX_TCP_SOCKET* nxsocket,
                                      ULONG waitoption);
#endif /* HAVE_NETX */

#ifdef MICRIUM
    WOLFSSL_LOCAL int MicriumSend(WOLFSSL* ssl, char* buf, int sz, void* ctx);
    WOLFSSL_LOCAL int MicriumReceive(WOLFSSL* ssl, char* buf, int sz,
                                     void* ctx);
    WOLFSSL_LOCAL int MicriumReceiveFrom(WOLFSSL* ssl, char* buf, int sz,
                                         void* ctx);
    WOLFSSL_LOCAL int MicriumSendTo(WOLFSSL* ssl, char* buf, int sz, void* ctx);
#endif /* MICRIUM */

#ifdef WOLFSSL_DTLS
    typedef int (*CallbackGenCookie)(WOLFSSL* ssl, unsigned char* buf, int sz,
                                     void* ctx);
    WOLFSSL_API void  wolfSSL_CTX_SetGenCookie(WOLFSSL_CTX*, CallbackGenCookie);
    WOLFSSL_API void  wolfSSL_SetCookieCtx(WOLFSSL* ssl, void *ctx);
    WOLFSSL_API void* wolfSSL_GetCookieCtx(WOLFSSL* ssl);

    #ifdef WOLFSSL_SESSION_EXPORT
        typedef int (*CallbackGetPeer)(WOLFSSL* ssl, char* ip, int* ipSz,
                                            unsigned short* port, int* fam);
        typedef int (*CallbackSetPeer)(WOLFSSL* ssl, char* ip, int ipSz,
                                              unsigned short port, int fam);

        WOLFSSL_API void wolfSSL_CTX_SetIOGetPeer(WOLFSSL_CTX*, CallbackGetPeer);
        WOLFSSL_API void wolfSSL_CTX_SetIOSetPeer(WOLFSSL_CTX*, CallbackSetPeer);
    #endif /* WOLFSSL_SESSION_EXPORT */
#endif



#ifndef XINET_NTOP
    #define XINET_NTOP(a,b,c,d) inet_ntop((a),(b),(c),(d))
#endif
#ifndef XINET_PTON
    #define XINET_PTON(a,b,c)   inet_pton((a),(b),(c))
#endif
#ifndef XHTONS
    #define XHTONS(a) htons((a))
#endif
#ifndef XNTOHS
    #define XNTOHS(a) ntohs((a))
#endif

#ifndef WOLFSSL_IP4
    #define WOLFSSL_IP4 AF_INET
#endif
#ifndef WOLFSSL_IP6
    #define WOLFSSL_IP6 AF_INET6
#endif


#ifdef __cplusplus
    }  /* extern "C" */
#endif

#endif /* WOLFSSL_IO_H */
