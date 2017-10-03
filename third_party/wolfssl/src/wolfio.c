/* wolfio.c
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



#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifndef WOLFCRYPT_ONLY

#ifdef _WIN32_WCE
    /* On WinCE winsock2.h must be included before windows.h for socket stuff */
    #include <winsock2.h>
#endif

#include <wolfssl/internal.h>
#include <wolfssl/error-ssl.h>
#include <wolfssl/wolfio.h>

#if defined(HAVE_HTTP_CLIENT)
    #include <stdlib.h>   /* atoi(), strtol() */
#endif

/*
Possible IO enable options:
 * WOLFSSL_USER_IO:     Disables default Embed* callbacks and     default: off
                        allows user to define their own using
                        wolfSSL_SetIORecv and wolfSSL_SetIOSend
 * USE_WOLFSSL_IO:      Enables the wolfSSL IO functions          default: off
 * HAVE_HTTP_CLIENT:    Enables HTTP client API's                 default: off
                                     (unless HAVE_OCSP or HAVE_CRL_IO defined)
 * HAVE_IO_TIMEOUT:     Enables support for connect timeout       default: off
 */


/* if user writes own I/O callbacks they can define WOLFSSL_USER_IO to remove
   automatic setting of default I/O functions EmbedSend() and EmbedReceive()
   but they'll still need SetCallback xxx() at end of file
*/

#if defined(USE_WOLFSSL_IO) || defined(HAVE_HTTP_CLIENT)

/* Translates return codes returned from
 * send() and recv() if need be.
 */
static INLINE int TranslateReturnCode(int old, int sd)
{
    (void)sd;

#if defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)
    if (old == 0) {
        errno = SOCKET_EWOULDBLOCK;
        return -1;  /* convert to BSD style wouldblock as error */
    }

    if (old < 0) {
        errno = RTCS_geterror(sd);
        if (errno == RTCSERR_TCP_CONN_CLOSING)
            return 0;   /* convert to BSD style closing */
        if (errno == RTCSERR_TCP_CONN_RLSD)
            errno = SOCKET_ECONNRESET;
        if (errno == RTCSERR_TCP_TIMED_OUT)
            errno = SOCKET_EAGAIN;
    }
#endif

    return old;
}

static INLINE int LastError(void)
{
#ifdef USE_WINDOWS_API
    return WSAGetLastError();
#elif defined(EBSNET)
    return xn_getlasterror();
#else
    return errno;
#endif
}

#endif /* USE_WOLFSSL_IO || HAVE_HTTP_CLIENT */


#ifdef USE_WOLFSSL_IO

/* The receive embedded callback
 *  return : nb bytes read, or error
 */
int EmbedReceive(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    int sd = *(int*)ctx;
    int recvd;

#ifdef WOLFSSL_DTLS
    {
        int dtls_timeout = wolfSSL_dtls_get_current_timeout(ssl);
        if (wolfSSL_dtls(ssl)
                     && !wolfSSL_get_using_nonblock(ssl)
                     && dtls_timeout != 0) {
            #ifdef USE_WINDOWS_API
                DWORD timeout = dtls_timeout * 1000;
            #else
                struct timeval timeout;
                XMEMSET(&timeout, 0, sizeof(timeout));
                timeout.tv_sec = dtls_timeout;
            #endif
            if (setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,
                           sizeof(timeout)) != 0) {
                WOLFSSL_MSG("setsockopt rcvtimeo failed");
            }
        }
    }
#endif

    recvd = wolfIO_Recv(sd, buf, sz, ssl->rflags);
    if (recvd < 0) {
        int err = LastError();
        WOLFSSL_MSG("Embed Receive error");

        if (err == SOCKET_EWOULDBLOCK || err == SOCKET_EAGAIN) {
            if (!wolfSSL_dtls(ssl) || wolfSSL_get_using_nonblock(ssl)) {
                WOLFSSL_MSG("\tWould block");
                return WOLFSSL_CBIO_ERR_WANT_READ;
            }
            else {
                WOLFSSL_MSG("\tSocket timeout");
                return WOLFSSL_CBIO_ERR_TIMEOUT;
            }
        }
        else if (err == SOCKET_ECONNRESET) {
            WOLFSSL_MSG("\tConnection reset");
            return WOLFSSL_CBIO_ERR_CONN_RST;
        }
        else if (err == SOCKET_EINTR) {
            WOLFSSL_MSG("\tSocket interrupted");
            return WOLFSSL_CBIO_ERR_ISR;
        }
        else if (err == SOCKET_ECONNREFUSED) {
            WOLFSSL_MSG("\tConnection refused");
            return WOLFSSL_CBIO_ERR_WANT_READ;
        }
        else if (err == SOCKET_ECONNABORTED) {
            WOLFSSL_MSG("\tConnection aborted");
            return WOLFSSL_CBIO_ERR_CONN_CLOSE;
        }
        else {
            WOLFSSL_MSG("\tGeneral error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }
    else if (recvd == 0) {
        WOLFSSL_MSG("Embed receive connection closed");
        return WOLFSSL_CBIO_ERR_CONN_CLOSE;
    }

    return recvd;
}

/* The send embedded callback
 *  return : nb bytes sent, or error
 */
int EmbedSend(WOLFSSL* ssl, char *buf, int sz, void *ctx)
{
    int sd = *(int*)ctx;
    int sent;

    sent = wolfIO_Send(sd, buf, sz, ssl->wflags);
    if (sent < 0) {
        int err = LastError();
        WOLFSSL_MSG("Embed Send error");

        if (err == SOCKET_EWOULDBLOCK || err == SOCKET_EAGAIN) {
            WOLFSSL_MSG("\tWould Block");
            return WOLFSSL_CBIO_ERR_WANT_WRITE;
        }
        else if (err == SOCKET_ECONNRESET) {
            WOLFSSL_MSG("\tConnection reset");
            return WOLFSSL_CBIO_ERR_CONN_RST;
        }
        else if (err == SOCKET_EINTR) {
            WOLFSSL_MSG("\tSocket interrupted");
            return WOLFSSL_CBIO_ERR_ISR;
        }
        else if (err == SOCKET_EPIPE) {
            WOLFSSL_MSG("\tSocket EPIPE");
            return WOLFSSL_CBIO_ERR_CONN_CLOSE;
        }
        else {
            WOLFSSL_MSG("\tGeneral error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }

    return sent;
}


#ifdef WOLFSSL_DTLS

#include <wolfssl/wolfcrypt/sha.h>

#define SENDTO_FUNCTION sendto
#define RECVFROM_FUNCTION recvfrom


/* The receive embedded callback
 *  return : nb bytes read, or error
 */
int EmbedReceiveFrom(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    WOLFSSL_DTLS_CTX* dtlsCtx = (WOLFSSL_DTLS_CTX*)ctx;
    int recvd;
    int err;
    int sd = dtlsCtx->rfd;
    int dtls_timeout = wolfSSL_dtls_get_current_timeout(ssl);
    SOCKADDR_S peer;
    XSOCKLENT peerSz = sizeof(peer);

    WOLFSSL_ENTER("EmbedReceiveFrom()");

    if (ssl->options.handShakeDone)
        dtls_timeout = 0;

    if (!wolfSSL_get_using_nonblock(ssl)) {
        #ifdef USE_WINDOWS_API
            DWORD timeout = dtls_timeout * 1000;
        #else
            struct timeval timeout;
            XMEMSET(&timeout, 0, sizeof(timeout));
            timeout.tv_sec = dtls_timeout;
        #endif
        if (setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,
                       sizeof(timeout)) != 0) {
                WOLFSSL_MSG("setsockopt rcvtimeo failed");
        }
    }

    recvd = (int)RECVFROM_FUNCTION(sd, buf, sz, ssl->rflags,
                                  (SOCKADDR*)&peer, &peerSz);

    recvd = TranslateReturnCode(recvd, sd);

    if (recvd < 0) {
        err = LastError();
        WOLFSSL_MSG("Embed Receive From error");

        if (err == SOCKET_EWOULDBLOCK || err == SOCKET_EAGAIN) {
            if (wolfSSL_get_using_nonblock(ssl)) {
                WOLFSSL_MSG("\tWould block");
                return WOLFSSL_CBIO_ERR_WANT_READ;
            }
            else {
                WOLFSSL_MSG("\tSocket timeout");
                return WOLFSSL_CBIO_ERR_TIMEOUT;
            }
        }
        else if (err == SOCKET_ECONNRESET) {
            WOLFSSL_MSG("\tConnection reset");
            return WOLFSSL_CBIO_ERR_CONN_RST;
        }
        else if (err == SOCKET_EINTR) {
            WOLFSSL_MSG("\tSocket interrupted");
            return WOLFSSL_CBIO_ERR_ISR;
        }
        else if (err == SOCKET_ECONNREFUSED) {
            WOLFSSL_MSG("\tConnection refused");
            return WOLFSSL_CBIO_ERR_WANT_READ;
        }
        else {
            WOLFSSL_MSG("\tGeneral error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }
    else {
        if (dtlsCtx->peer.sz > 0
                && peerSz != (XSOCKLENT)dtlsCtx->peer.sz
                && XMEMCMP(&peer, dtlsCtx->peer.sa, peerSz) != 0) {
            WOLFSSL_MSG("    Ignored packet from invalid peer");
            return WOLFSSL_CBIO_ERR_WANT_READ;
        }
    }

    return recvd;
}


/* The send embedded callback
 *  return : nb bytes sent, or error
 */
int EmbedSendTo(WOLFSSL* ssl, char *buf, int sz, void *ctx)
{
    WOLFSSL_DTLS_CTX* dtlsCtx = (WOLFSSL_DTLS_CTX*)ctx;
    int sd = dtlsCtx->wfd;
    int sent;
    int len = sz;
    int err;

    WOLFSSL_ENTER("EmbedSendTo()");

    sent = (int)SENDTO_FUNCTION(sd, &buf[sz - len], len, ssl->wflags,
                                (const SOCKADDR*)dtlsCtx->peer.sa,
                                dtlsCtx->peer.sz);

    sent = TranslateReturnCode(sent, sd);

    if (sent < 0) {
        err = LastError();
        WOLFSSL_MSG("Embed Send To error");

        if (err == SOCKET_EWOULDBLOCK || err == SOCKET_EAGAIN) {
            WOLFSSL_MSG("\tWould Block");
            return WOLFSSL_CBIO_ERR_WANT_WRITE;
        }
        else if (err == SOCKET_ECONNRESET) {
            WOLFSSL_MSG("\tConnection reset");
            return WOLFSSL_CBIO_ERR_CONN_RST;
        }
        else if (err == SOCKET_EINTR) {
            WOLFSSL_MSG("\tSocket interrupted");
            return WOLFSSL_CBIO_ERR_ISR;
        }
        else if (err == SOCKET_EPIPE) {
            WOLFSSL_MSG("\tSocket EPIPE");
            return WOLFSSL_CBIO_ERR_CONN_CLOSE;
        }
        else {
            WOLFSSL_MSG("\tGeneral error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }

    return sent;
}


#ifdef WOLFSSL_MULTICAST

/* The alternate receive embedded callback for Multicast
 *  return : nb bytes read, or error
 */
int EmbedReceiveFromMcast(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    WOLFSSL_DTLS_CTX* dtlsCtx = (WOLFSSL_DTLS_CTX*)ctx;
    int recvd;
    int err;
    int sd = dtlsCtx->rfd;

    WOLFSSL_ENTER("EmbedReceiveFromMcast()");

    recvd = (int)RECVFROM_FUNCTION(sd, buf, sz, ssl->rflags, NULL, NULL);

    recvd = TranslateReturnCode(recvd, sd);

    if (recvd < 0) {
        err = LastError();
        WOLFSSL_MSG("Embed Receive From error");

        if (err == SOCKET_EWOULDBLOCK || err == SOCKET_EAGAIN) {
            if (wolfSSL_get_using_nonblock(ssl)) {
                WOLFSSL_MSG("\tWould block");
                return WOLFSSL_CBIO_ERR_WANT_READ;
            }
            else {
                WOLFSSL_MSG("\tSocket timeout");
                return WOLFSSL_CBIO_ERR_TIMEOUT;
            }
        }
        else if (err == SOCKET_ECONNRESET) {
            WOLFSSL_MSG("\tConnection reset");
            return WOLFSSL_CBIO_ERR_CONN_RST;
        }
        else if (err == SOCKET_EINTR) {
            WOLFSSL_MSG("\tSocket interrupted");
            return WOLFSSL_CBIO_ERR_ISR;
        }
        else if (err == SOCKET_ECONNREFUSED) {
            WOLFSSL_MSG("\tConnection refused");
            return WOLFSSL_CBIO_ERR_WANT_READ;
        }
        else {
            WOLFSSL_MSG("\tGeneral error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }

    return recvd;
}
#endif /* WOLFSSL_MULTICAST */


/* The DTLS Generate Cookie callback
 *  return : number of bytes copied into buf, or error
 */
int EmbedGenerateCookie(WOLFSSL* ssl, byte *buf, int sz, void *ctx)
{
    int sd = ssl->wfd;
    SOCKADDR_S peer;
    XSOCKLENT peerSz = sizeof(peer);
    byte digest[SHA256_DIGEST_SIZE];
    int  ret = 0;

    (void)ctx;

    XMEMSET(&peer, 0, sizeof(peer));
    if (getpeername(sd, (SOCKADDR*)&peer, &peerSz) != 0) {
        WOLFSSL_MSG("getpeername failed in EmbedGenerateCookie");
        return GEN_COOKIE_E;
    }

    ret = wc_Sha256Hash((byte*)&peer, peerSz, digest);
    if (ret != 0)
        return ret;

    if (sz > SHA256_DIGEST_SIZE)
        sz = SHA256_DIGEST_SIZE;
    XMEMCPY(buf, digest, sz);

    return sz;
}

#ifdef WOLFSSL_SESSION_EXPORT

    /* get the peer information in human readable form (ip, port, family)
     * default function assumes BSD sockets
     * can be overriden with wolfSSL_CTX_SetIOGetPeer
     */
    int EmbedGetPeer(WOLFSSL* ssl, char* ip, int* ipSz,
                                                 unsigned short* port, int* fam)
    {
        SOCKADDR_S peer;
        word32     peerSz;
        int        ret;

        if (ssl == NULL || ip == NULL || ipSz == NULL ||
                                                  port == NULL || fam == NULL) {
            return BAD_FUNC_ARG;
        }

        /* get peer information stored in ssl struct */
        peerSz = sizeof(SOCKADDR_S);
        if ((ret = wolfSSL_dtls_get_peer(ssl, (void*)&peer, &peerSz))
                                                               != SSL_SUCCESS) {
            return ret;
        }

        /* extract family, ip, and port */
        *fam = ((SOCKADDR_S*)&peer)->ss_family;
        switch (*fam) {
            case WOLFSSL_IP4:
                if (XINET_NTOP(*fam, &(((SOCKADDR_IN*)&peer)->sin_addr),
                                                           ip, *ipSz) == NULL) {
                    WOLFSSL_MSG("XINET_NTOP error");
                    return SOCKET_ERROR_E;
                }
                *port = XNTOHS(((SOCKADDR_IN*)&peer)->sin_port);
                break;

            case WOLFSSL_IP6:
            #ifdef WOLFSSL_IPV6
                if (XINET_NTOP(*fam, &(((SOCKADDR_IN6*)&peer)->sin6_addr),
                                                           ip, *ipSz) == NULL) {
                    WOLFSSL_MSG("XINET_NTOP error");
                    return SOCKET_ERROR_E;
                }
                *port = XNTOHS(((SOCKADDR_IN6*)&peer)->sin6_port);
            #endif /* WOLFSSL_IPV6 */
                break;

            default:
                WOLFSSL_MSG("Unknown family type");
                return SOCKET_ERROR_E;
        }
        ip[*ipSz - 1] = '\0'; /* make sure has terminator */
        *ipSz = (word16)XSTRLEN(ip);

        return SSL_SUCCESS;
    }

    /* set the peer information in human readable form (ip, port, family)
     * default function assumes BSD sockets
     * can be overriden with wolfSSL_CTX_SetIOSetPeer
     */
    int EmbedSetPeer(WOLFSSL* ssl, char* ip, int ipSz,
                                                   unsigned short port, int fam)
    {
        int    ret;
        SOCKADDR_S addr;

        /* sanity checks on arguments */
        if (ssl == NULL || ip == NULL || ipSz < 0 || ipSz > DTLS_EXPORT_IP) {
            return BAD_FUNC_ARG;
        }

        addr.ss_family = fam;
        switch (addr.ss_family) {
            case WOLFSSL_IP4:
                if (XINET_PTON(addr.ss_family, ip,
                                     &(((SOCKADDR_IN*)&addr)->sin_addr)) <= 0) {
                    WOLFSSL_MSG("XINET_PTON error");
                    return SOCKET_ERROR_E;
                }
                ((SOCKADDR_IN*)&addr)->sin_port = XHTONS(port);

                /* peer sa is free'd in SSL_ResourceFree */
                if ((ret = wolfSSL_dtls_set_peer(ssl, (SOCKADDR_IN*)&addr,
                                          sizeof(SOCKADDR_IN)))!= SSL_SUCCESS) {
                    WOLFSSL_MSG("Import DTLS peer info error");
                    return ret;
                }
                break;

            case WOLFSSL_IP6:
            #ifdef WOLFSSL_IPV6
                if (XINET_PTON(addr.ss_family, ip,
                                   &(((SOCKADDR_IN6*)&addr)->sin6_addr)) <= 0) {
                    WOLFSSL_MSG("XINET_PTON error");
                    return SOCKET_ERROR_E;
                }
                ((SOCKADDR_IN6*)&addr)->sin6_port = XHTONS(port);

                /* peer sa is free'd in SSL_ResourceFree */
                if ((ret = wolfSSL_dtls_set_peer(ssl, (SOCKADDR_IN6*)&addr,
                                         sizeof(SOCKADDR_IN6)))!= SSL_SUCCESS) {
                    WOLFSSL_MSG("Import DTLS peer info error");
                    return ret;
                }
            #endif /* WOLFSSL_IPV6 */
                break;

            default:
                WOLFSSL_MSG("Unknown address family");
                return BUFFER_E;
        }

        return SSL_SUCCESS;
    }
#endif /* WOLFSSL_SESSION_EXPORT */
#endif /* WOLFSSL_DTLS */


int wolfIO_Recv(SOCKET_T sd, char *buf, int sz, int rdFlags)
{
    int recvd;

    recvd = (int)RECV_FUNCTION(sd, buf, sz, rdFlags);
    recvd = TranslateReturnCode(recvd, sd);

    return recvd;
}

int wolfIO_Send(SOCKET_T sd, char *buf, int sz, int wrFlags)
{
    int sent;

    sent = (int)SEND_FUNCTION(sd, buf, sz, wrFlags);
    sent = TranslateReturnCode(sent, sd);

    return sent;
}

#endif /* USE_WOLFSSL_IO */


#ifdef HAVE_HTTP_CLIENT

#ifndef HAVE_IO_TIMEOUT
    #define io_timeout_sec 0
#else

    #ifndef DEFAULT_TIMEOUT_SEC
        #define DEFAULT_TIMEOUT_SEC 0 /* no timeout */
    #endif

    static int io_timeout_sec = DEFAULT_TIMEOUT_SEC;

    void wolfIO_SetTimeout(int to_sec)
    {
        io_timeout_sec = to_sec;
    }

    int wolfIO_SetBlockingMode(SOCKET_T sockfd, int non_blocking)
    {
        int ret = 0;

    #ifdef USE_WINDOWS_API
        unsigned long blocking = non_blocking;
        ret = ioctlsocket(sockfd, FIONBIO, &blocking);
        if (ret == SOCKET_ERROR)
            ret = -1;
    #else
        ret = fcntl(sockfd, F_GETFL, 0);
        if (ret >= 0) {
            if (non_blocking)
                ret |= O_NONBLOCK;
            else
                ret &= ~O_NONBLOCK;
            ret = fcntl(sockfd, F_SETFL, ret);
        }
    #endif
        if (ret < 0) {
            WOLFSSL_MSG("wolfIO_SetBlockingMode failed");
        }

        return ret;
    }

    #ifdef _MSC_VER
        /* 4204: non-constant aggregate initializer (nfds = sockfd + 1) */
        #pragma warning(disable: 4204)
    #endif
    int wolfIO_Select(SOCKET_T sockfd, int to_sec)
    {
        fd_set fds;
        SOCKET_T nfds = sockfd + 1;
        struct timeval timeout = { (to_sec > 0) ? to_sec : 0, 0};
        int ret;

        FD_ZERO(&fds);
        FD_SET(sockfd, &fds);

        ret = select(nfds, &fds, &fds, NULL, &timeout);
        if (ret == 0) {
        #ifdef DEBUG_HTTP
            printf("Timeout: %d\n", ret);
        #endif
            return HTTP_TIMEOUT;
        }
        else if (ret > 0) {
            if (FD_ISSET(sockfd, &fds))
                return 0;
        }
        return SOCKET_ERROR_E;
    }
#endif /* HAVE_IO_TIMEOUT */

static int wolfIO_Word16ToString(char* d, word16 number)
{
    int i = 0;
    word16 order = 10000;
    word16 digit;

    if (d == NULL)
        return i;

    if (number == 0)
        d[i++] = '0';
    else {
        while (order) {
            digit = number / order;
            if (i > 0 || digit != 0)
                d[i++] = (char)digit + '0';
            if (digit != 0)
                number %= digit * order;

            order = (order > 1) ? order / 10 : 0;
        }
    }
    d[i] = 0; /* null terminate */

    return i;
}

int wolfIO_TcpConnect(SOCKET_T* sockfd, const char* ip, word16 port, int to_sec)
{
#ifdef HAVE_SOCKADDR
    int ret = 0;
    SOCKADDR_S addr;
    int sockaddr_len = sizeof(SOCKADDR_IN);
#ifdef HAVE_GETADDRINFO
    ADDRINFO hints;
    ADDRINFO* answer = NULL;
    char strPort[6];
#else
    HOSTENT* entry;
    SOCKADDR_IN *sin;
#endif

    XMEMSET(&addr, 0, sizeof(addr));

#ifdef WOLFIO_DEBUG
    printf("TCP Connect: %s:%d\n", ip, port);
#endif

#ifdef HAVE_GETADDRINFO
    XMEMSET(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (wolfIO_Word16ToString(strPort, port) == 0) {
        WOLFSSL_MSG("invalid port number for responder");
        return -1;
    }

    if (getaddrinfo(ip, strPort, &hints, &answer) < 0 || answer == NULL) {
        WOLFSSL_MSG("no addr info for responder");
        return -1;
    }

    sockaddr_len = answer->ai_addrlen;
    XMEMCPY(&addr, answer->ai_addr, sockaddr_len);
    freeaddrinfo(answer);
#else
    entry = gethostbyname(ip);
    sin = (SOCKADDR_IN *)&addr;

    if (entry) {
        sin->sin_family = AF_INET;
        sin->sin_port = XHTONS(port);
        XMEMCPY(&sin->sin_addr.s_addr, entry->h_addr_list[0], entry->h_length);
    }
    else {
        WOLFSSL_MSG("no addr info for responder");
        return -1;
    }
#endif

    *sockfd = (SOCKET_T)socket(addr.ss_family, SOCK_STREAM, 0);

#ifdef USE_WINDOWS_API
    if (*sockfd == INVALID_SOCKET) {
        WOLFSSL_MSG("bad socket fd, out of fds?");
        return -1;
    }
#else
     if (*sockfd < 0) {
         WOLFSSL_MSG("bad socket fd, out of fds?");
         return -1;
     }
#endif

#ifdef HAVE_IO_TIMEOUT
    /* if timeout value provided then set socket non-blocking */
    if (to_sec > 0) {
        wolfIO_SetBlockingMode(*sockfd, 1);
    }
#else
    (void)to_sec;
#endif

    ret = connect(*sockfd, (SOCKADDR *)&addr, sockaddr_len);
#ifdef HAVE_IO_TIMEOUT
    if (ret != 0) {
        if ((errno == EINPROGRESS) && (to_sec > 0)) {
            /* wait for connect to complete */
            ret = wolfIO_Select(*sockfd, to_sec);

            /* restore blocking mode */
            wolfIO_SetBlockingMode(*sockfd, 0);
        }
    }
#endif
    if (ret != 0) {
        WOLFSSL_MSG("Responder tcp connect failed");
        return -1;
    }
    return ret;
#else
    (void)sockfd;
    (void)ip;
    (void)port;
    (void)to_sec;
    return -1;
#endif /* HAVE_SOCKADDR */
}

#ifndef HTTP_SCRATCH_BUFFER_SIZE
    #define HTTP_SCRATCH_BUFFER_SIZE 512
#endif
#ifndef MAX_URL_ITEM_SIZE
    #define MAX_URL_ITEM_SIZE   80
#endif

int wolfIO_DecodeUrl(const char* url, int urlSz, char* outName, char* outPath,
    word16* outPort)
{
    int result = -1;

    if (url == NULL || urlSz == 0) {
        if (outName)
            *outName = 0;
        if (outPath)
            *outPath = 0;
        if (outPort)
            *outPort = 0;
    }
    else {
        int i, cur;

        /* need to break the url down into scheme, address, and port */
        /*     "http://example.com:8080/" */
        /*     "http://[::1]:443/"        */
        if (XSTRNCMP(url, "http://", 7) == 0) {
            cur = 7;
        } else cur = 0;

        i = 0;
        if (url[cur] == '[') {
            cur++;
            /* copy until ']' */
            while (url[cur] != 0 && url[cur] != ']' && cur < urlSz) {
                if (outName)
                    outName[i] = url[cur];
                i++; cur++;
            }
            cur++; /* skip ']' */
        }
        else {
            while (url[cur] != 0 && url[cur] != ':' &&
                                           url[cur] != '/' && cur < urlSz) {
                if (outName)
                    outName[i] = url[cur];
                i++; cur++;
            }
        }
        if (outName)
            outName[i] = 0;
        /* Need to pick out the path after the domain name */

        if (cur < urlSz && url[cur] == ':') {
            char port[6];
            int j;
            word32 bigPort = 0;
            i = 0;
            cur++;
            while (cur < urlSz && url[cur] != 0 && url[cur] != '/' &&
                    i < 6) {
                port[i++] = url[cur++];
            }

            for (j = 0; j < i; j++) {
                if (port[j] < '0' || port[j] > '9') return -1;
                bigPort = (bigPort * 10) + (port[j] - '0');
            }
            if (outPort)
                *outPort = (word16)bigPort;
        }
        else if (outPort)
            *outPort = 80;


        if (cur < urlSz && url[cur] == '/') {
            i = 0;
            while (cur < urlSz && url[cur] != 0 && i < MAX_URL_ITEM_SIZE) {
                if (outPath)
                    outPath[i] = url[cur];
                i++; cur++;
            }
            if (outPath)
                outPath[i] = 0;
        }
        else if (outPath) {
            outPath[0] = '/';
            outPath[1] = 0;
        }

        result = 0;
    }

    return result;
}

static int wolfIO_HttpProcessResponseBuf(int sfd, byte **recvBuf, int* recvBufSz,
    int chunkSz, char* start, int len, int dynType, void* heap)
{
    byte* newRecvBuf = NULL;
    int newRecvSz = *recvBufSz + chunkSz;
    int pos = 0;

    WOLFSSL_MSG("Processing HTTP response");
#ifdef WOLFIO_DEBUG
    printf("HTTP Chunk %d->%d\n", *recvBufSz, chunkSz);
#endif

    newRecvBuf = (byte*)XMALLOC(newRecvSz, heap, dynType);
    if (newRecvBuf == NULL) {
        WOLFSSL_MSG("wolfIO_HttpProcessResponseBuf malloc failed");
        return MEMORY_E;
    }

    /* if buffer already exists, then we are growing it */
    if (*recvBuf) {
        XMEMCPY(&newRecvBuf[pos], *recvBuf, *recvBufSz);
        XFREE(*recvBuf, heap, dynType);
        pos += *recvBufSz;
        *recvBuf = NULL;
    }

    /* copy the remainder of the httpBuf into the respBuf */
    if (len != 0) {
        XMEMCPY(&newRecvBuf[pos], start, len);
        pos += len;
    }

    /* receive the remainder of chunk */
    while (len < chunkSz) {
        int rxSz = wolfIO_Recv(sfd, (char*)&newRecvBuf[pos], chunkSz-len, 0);
        if (rxSz > 0) {
            len += rxSz;
            pos += rxSz;
        }
        else {
            WOLFSSL_MSG("wolfIO_HttpProcessResponseBuf recv failed");
            XFREE(newRecvBuf, heap, dynType);
            return -1;
        }
    }

    *recvBuf = newRecvBuf;
    *recvBufSz = newRecvSz;

    return 0;
}

int wolfIO_HttpProcessResponse(int sfd, const char* appStr,
    byte** respBuf, byte* httpBuf, int httpBufSz, int dynType, void* heap)
{
    int result = 0;
    int len = 0;
    char *start, *end;
    int respBufSz = 0;
    int isChunked = 0, chunkSz = 0;
    enum phr_state { phr_init, phr_http_start, phr_have_length, phr_have_type,
                     phr_wait_end, phr_get_chunk_len, phr_get_chunk_data,
                     phr_http_end
    } state = phr_init;

    *respBuf = NULL;
    start = end = NULL;
    do {
        if (state == phr_get_chunk_data) {
            /* get chunk of data */
            result = wolfIO_HttpProcessResponseBuf(sfd, respBuf, &respBufSz,
                chunkSz, start, len, dynType, heap);

            state = (result != 0) ? phr_http_end : phr_get_chunk_len;
            end = NULL;
            len = 0;
        }

        /* read data if no \r\n or first time */
        if (end == NULL) {
            result = wolfIO_Recv(sfd, (char*)httpBuf+len, httpBufSz-len-1, 0);
            if (result > 0) {
                len += result;
                start = (char*)httpBuf;
                start[len] = 0;
            }
            else {
                WOLFSSL_MSG("wolfIO_HttpProcessResponse recv http from peer failed");
                return -1;
            }
        }
        end = XSTRSTR(start, "\r\n"); /* locate end */

        /* handle incomplete rx */
        if (end == NULL) {
            if (len != 0)
                XMEMMOVE(httpBuf, start, len);
            start = end = NULL;
        }
        /* when start is "\r\n" */
        else if (end == start) {
            /* if waiting for end or need chunk len */
            if (state == phr_wait_end || state == phr_get_chunk_len) {
                state = (isChunked) ? phr_get_chunk_len : phr_http_end;
                len -= 2; start += 2; /* skip \r\n */
             }
             else {
                WOLFSSL_MSG("wolfIO_HttpProcessResponse header ended early");
                return -1;
             }
        }
        else {
            *end = 0; /* null terminate */
            len -= (int)(end - start) + 2;
                /* adjust len to remove the first line including the /r/n */

        #ifdef WOLFIO_DEBUG
            printf("HTTP Resp: %s\n", start);
        #endif

            switch (state) {
                case phr_init:
                    if (XSTRNCASECMP(start, "HTTP/1", 6) == 0) {
                        start += 9;
                        if (XSTRNCASECMP(start, "200 OK", 6) != 0) {
                            WOLFSSL_MSG("wolfIO_HttpProcessResponse not OK");
                            return -1;
                        }
                        state = phr_http_start;
                    }
                    break;
                case phr_http_start:
                case phr_have_length:
                case phr_have_type:
                    if (XSTRNCASECMP(start, "Content-Type:", 13) == 0) {
                        start += 13;
                        while (*start == ' ' && *start != '\0') start++;
                        if (XSTRNCASECMP(start, appStr, XSTRLEN(appStr)) != 0) {
                            WOLFSSL_MSG("wolfIO_HttpProcessResponse appstr mismatch");
                            return -1;
                        }
                        state = (state == phr_http_start) ? phr_have_type : phr_wait_end;
                    }
                    else if (XSTRNCASECMP(start, "Content-Length:", 15) == 0) {
                        start += 15;
                        while (*start == ' ' && *start != '\0') start++;
                        chunkSz = atoi(start);
                        state = (state == phr_http_start) ? phr_have_length : phr_wait_end;
                    }
                    else if (XSTRNCASECMP(start, "Transfer-Encoding:", 18) == 0) {
                        start += 18;
                        while (*start == ' ' && *start != '\0') start++;
                        if (XSTRNCASECMP(start, "chunked", 7) == 0) {
                            isChunked = 1;
                            state = (state == phr_http_start) ? phr_have_length : phr_wait_end;
                        }
                    }
                    break;
                case phr_get_chunk_len:
                    chunkSz = (int)strtol(start, NULL, 16); /* hex format */
                    state = (chunkSz == 0) ? phr_http_end : phr_get_chunk_data;
                    break;
                case phr_get_chunk_data:
                    /* processing for chunk data done above, since \r\n isn't required */
                case phr_wait_end:
                case phr_http_end:
                    /* do nothing */
                    break;
            } /* switch (state) */

            /* skip to end plus \r\n */
            start = end + 2;
        }
    } while (state != phr_http_end);

    if (!isChunked) {
        result = wolfIO_HttpProcessResponseBuf(sfd, respBuf, &respBufSz, chunkSz,
                                                    start, len, dynType, heap);
    }

    if (result >= 0) {
        result = respBufSz;
    }
    else {
        WOLFSSL_ERROR(result);
    }

    return result;
}

int wolfIO_HttpBuildRequest(const char* reqType, const char* domainName,
    const char* path, int pathLen, int reqSz, const char* contentType,
    byte* buf, int bufSize)
{
    word32 reqTypeLen, domainNameLen, reqSzStrLen, contentTypeLen, maxLen;
    char reqSzStr[6];
    char* req = (char*)buf;
    const char* blankStr = " ";
    const char* http11Str = " HTTP/1.1";
    const char* hostStr = "\r\nHost: ";
    const char* contentLenStr = "\r\nContent-Length: ";
    const char* contentTypeStr = "\r\nContent-Type: ";
    const char* doubleCrLfStr = "\r\n\r\n";
    word32 blankStrLen, http11StrLen, hostStrLen, contentLenStrLen,
        contentTypeStrLen, doubleCrLfStrLen;

    reqTypeLen = (word32)XSTRLEN(reqType);
    domainNameLen = (word32)XSTRLEN(domainName);
    reqSzStrLen = wolfIO_Word16ToString(reqSzStr, (word16)reqSz);
    contentTypeLen = (word32)XSTRLEN(contentType);

    blankStrLen = (word32)XSTRLEN(blankStr);
    http11StrLen = (word32)XSTRLEN(http11Str);
    hostStrLen = (word32)XSTRLEN(hostStr);
    contentLenStrLen = (word32)XSTRLEN(contentLenStr);
    contentTypeStrLen = (word32)XSTRLEN(contentTypeStr);
    doubleCrLfStrLen = (word32)XSTRLEN(doubleCrLfStr);

    /* determine max length and check it */
    maxLen =
        reqTypeLen +
        blankStrLen +
        pathLen +
        http11StrLen +
        hostStrLen +
        domainNameLen +
        contentLenStrLen +
        reqSzStrLen +
        contentTypeStrLen +
        contentTypeLen +
        doubleCrLfStrLen +
        1 /* null term */;
    if (maxLen > (word32)bufSize)
        return 0;

    XSTRNCPY((char*)buf, reqType, reqTypeLen);
    buf += reqTypeLen;
    XSTRNCPY((char*)buf, blankStr, blankStrLen+1);
    buf += blankStrLen;
    XSTRNCPY((char*)buf, path, pathLen);
    buf += pathLen;
    XSTRNCPY((char*)buf, http11Str, http11StrLen+1);
    buf += http11StrLen;
    if (domainNameLen > 0) {
        XSTRNCPY((char*)buf, hostStr, hostStrLen+1);
        buf += hostStrLen;
        XSTRNCPY((char*)buf, domainName, domainNameLen);
        buf += domainNameLen;
    }
    if (reqSz > 0 && reqSzStrLen > 0) {
        XSTRNCPY((char*)buf, contentLenStr, contentLenStrLen+1);
        buf += contentLenStrLen;
        XSTRNCPY((char*)buf, reqSzStr, reqSzStrLen);
        buf += reqSzStrLen;
    }
    if (contentTypeLen > 0) {
        XSTRNCPY((char*)buf, contentTypeStr, contentTypeStrLen+1);
        buf += contentTypeStrLen;
        XSTRNCPY((char*)buf, contentType, contentTypeLen);
        buf += contentTypeLen;
    }
    XSTRNCPY((char*)buf, doubleCrLfStr, doubleCrLfStrLen+1);
    buf += doubleCrLfStrLen;

#ifdef WOLFIO_DEBUG
    printf("HTTP %s: %s", reqType, req);
#endif

    /* calculate actual length based on original and new pointer */
    return (int)((char*)buf - req);
}


#ifdef HAVE_OCSP

int wolfIO_HttpBuildRequestOcsp(const char* domainName, const char* path,
                                    int ocspReqSz, byte* buf, int bufSize)
{
    return wolfIO_HttpBuildRequest("POST", domainName, path, (int)XSTRLEN(path),
        ocspReqSz, "application/ocsp-request", buf, bufSize);
}

/* return: >0 OCSP Response Size
 *         -1 error */
int wolfIO_HttpProcessResponseOcsp(int sfd, byte** respBuf,
                                       byte* httpBuf, int httpBufSz, void* heap)
{
    return wolfIO_HttpProcessResponse(sfd, "application/ocsp-response",
        respBuf, httpBuf, httpBufSz, DYNAMIC_TYPE_OCSP, heap);
}

/* in default wolfSSL callback ctx is the heap pointer */
int EmbedOcspLookup(void* ctx, const char* url, int urlSz,
                        byte* ocspReqBuf, int ocspReqSz, byte** ocspRespBuf)
{
    SOCKET_T sfd = 0;
    word16   port;
    int      ret = -1;
#ifdef WOLFSSL_SMALL_STACK
    char*    path;
    char*    domainName;
#else
    char     path[MAX_URL_ITEM_SIZE];
    char     domainName[MAX_URL_ITEM_SIZE];
#endif

#ifdef WOLFSSL_SMALL_STACK
    path = (char*)XMALLOC(MAX_URL_ITEM_SIZE, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (path == NULL)
        return MEMORY_E;

    domainName = (char*)XMALLOC(MAX_URL_ITEM_SIZE, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (domainName == NULL) {
        XFREE(path, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return MEMORY_E;
    }
#endif

    if (ocspReqBuf == NULL || ocspReqSz == 0) {
        WOLFSSL_MSG("OCSP request is required for lookup");
    }
    else if (ocspRespBuf == NULL) {
        WOLFSSL_MSG("Cannot save OCSP response");
    }
    else if (wolfIO_DecodeUrl(url, urlSz, domainName, path, &port) < 0) {
        WOLFSSL_MSG("Unable to decode OCSP URL");
    }
    else {
        /* Note, the library uses the EmbedOcspRespFree() callback to
         * free this buffer. */
        int   httpBufSz = HTTP_SCRATCH_BUFFER_SIZE;
        byte* httpBuf   = (byte*)XMALLOC(httpBufSz, ctx, DYNAMIC_TYPE_OCSP);

        if (httpBuf == NULL) {
            WOLFSSL_MSG("Unable to create OCSP response buffer");
        }
        else {
            httpBufSz = wolfIO_HttpBuildRequestOcsp(domainName, path, ocspReqSz,
                                                            httpBuf, httpBufSz);

            ret = wolfIO_TcpConnect(&sfd, domainName, port, io_timeout_sec);
            if ((ret != 0) || (sfd < 0)) {
                WOLFSSL_MSG("OCSP Responder connection failed");
            }
            else if (wolfIO_Send(sfd, (char*)httpBuf, httpBufSz, 0) !=
                                                                    httpBufSz) {
                WOLFSSL_MSG("OCSP http request failed");
            }
            else if (wolfIO_Send(sfd, (char*)ocspReqBuf, ocspReqSz, 0) !=
                                                                    ocspReqSz) {
                WOLFSSL_MSG("OCSP ocsp request failed");
            }
            else {
                ret = wolfIO_HttpProcessResponseOcsp(sfd, ocspRespBuf, httpBuf,
                                                 HTTP_SCRATCH_BUFFER_SIZE, ctx);
            }

            close(sfd);
            XFREE(httpBuf, ctx, DYNAMIC_TYPE_OCSP);
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(path,       NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(domainName, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

/* in default callback ctx is heap hint */
void EmbedOcspRespFree(void* ctx, byte *resp)
{
    if (resp)
        XFREE(resp, ctx, DYNAMIC_TYPE_OCSP);

    (void)ctx;
}
#endif /* HAVE_OCSP */


#if defined(HAVE_CRL) && defined(HAVE_CRL_IO)

int wolfIO_HttpBuildRequestCrl(const char* url, int urlSz,
    const char* domainName, byte* buf, int bufSize)
{
    return wolfIO_HttpBuildRequest("GET", domainName, url, urlSz, 0, "",
        buf, bufSize);
}

int wolfIO_HttpProcessResponseCrl(WOLFSSL_CRL* crl, int sfd, byte* httpBuf,
    int httpBufSz)
{
    int result;
    byte *respBuf = NULL;

    result = wolfIO_HttpProcessResponse(sfd, "application/pkix-crl",
        &respBuf, httpBuf, httpBufSz, DYNAMIC_TYPE_CRL, crl->heap);
    if (result >= 0) {
        result = BufferLoadCRL(crl, respBuf, result, SSL_FILETYPE_ASN1, 0);
    }
    XFREE(respBuf, crl->heap, DYNAMIC_TYPE_CRL);

    return result;
}

int EmbedCrlLookup(WOLFSSL_CRL* crl, const char* url, int urlSz)
{
    SOCKET_T sfd = 0;
    word16   port;
    int      ret = -1;
#ifdef WOLFSSL_SMALL_STACK
    char*    domainName;
#else
    char     domainName[MAX_URL_ITEM_SIZE];
#endif

#ifdef WOLFSSL_SMALL_STACK
    domainName = (char*)XMALLOC(MAX_URL_ITEM_SIZE, crl->heap,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
    if (domainName == NULL) {
        return MEMORY_E;
    }
#endif

    if (wolfIO_DecodeUrl(url, urlSz, domainName, NULL, &port) < 0) {
        WOLFSSL_MSG("Unable to decode CRL URL");
    }
    else {
        int   httpBufSz = HTTP_SCRATCH_BUFFER_SIZE;
        byte* httpBuf   = (byte*)XMALLOC(httpBufSz, crl->heap,
                                                              DYNAMIC_TYPE_CRL);
        if (httpBuf == NULL) {
            WOLFSSL_MSG("Unable to create CRL response buffer");
        }
        else {
            httpBufSz = wolfIO_HttpBuildRequestCrl(url, urlSz, domainName,
                httpBuf, httpBufSz);

            ret = wolfIO_TcpConnect(&sfd, domainName, port, io_timeout_sec);
            if ((ret != 0) || (sfd < 0)) {
                WOLFSSL_MSG("CRL connection failed");
            }
            else if (wolfIO_Send(sfd, (char*)httpBuf, httpBufSz, 0)
                                                                 != httpBufSz) {
                WOLFSSL_MSG("CRL http get failed");
            }
            else {
                ret = wolfIO_HttpProcessResponseCrl(crl, sfd, httpBuf,
                                                      HTTP_SCRATCH_BUFFER_SIZE);
            }

            close(sfd);
            XFREE(httpBuf, crl->heap, DYNAMIC_TYPE_CRL);
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(domainName, crl->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}
#endif /* HAVE_CRL && HAVE_CRL_IO */

#endif /* HAVE_HTTP_CLIENT */



WOLFSSL_API void wolfSSL_SetIORecv(WOLFSSL_CTX *ctx, CallbackIORecv CBIORecv)
{
    ctx->CBIORecv = CBIORecv;
}


WOLFSSL_API void wolfSSL_SetIOSend(WOLFSSL_CTX *ctx, CallbackIOSend CBIOSend)
{
    ctx->CBIOSend = CBIOSend;
}


WOLFSSL_API void wolfSSL_SetIOReadCtx(WOLFSSL* ssl, void *rctx)
{
    ssl->IOCB_ReadCtx = rctx;
}


WOLFSSL_API void wolfSSL_SetIOWriteCtx(WOLFSSL* ssl, void *wctx)
{
    ssl->IOCB_WriteCtx = wctx;
}


WOLFSSL_API void* wolfSSL_GetIOReadCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->IOCB_ReadCtx;

    return NULL;
}


WOLFSSL_API void* wolfSSL_GetIOWriteCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->IOCB_WriteCtx;

    return NULL;
}


WOLFSSL_API void wolfSSL_SetIOReadFlags(WOLFSSL* ssl, int flags)
{
    ssl->rflags = flags;
}


WOLFSSL_API void wolfSSL_SetIOWriteFlags(WOLFSSL* ssl, int flags)
{
    ssl->wflags = flags;
}


#ifdef WOLFSSL_DTLS

WOLFSSL_API void wolfSSL_CTX_SetGenCookie(WOLFSSL_CTX* ctx, CallbackGenCookie cb)
{
    ctx->CBIOCookie = cb;
}


WOLFSSL_API void wolfSSL_SetCookieCtx(WOLFSSL* ssl, void *ctx)
{
    ssl->IOCB_CookieCtx = ctx;
}


WOLFSSL_API void* wolfSSL_GetCookieCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->IOCB_CookieCtx;

    return NULL;
}

#ifdef WOLFSSL_SESSION_EXPORT

WOLFSSL_API void wolfSSL_CTX_SetIOGetPeer(WOLFSSL_CTX* ctx, CallbackGetPeer cb)
{
    ctx->CBGetPeer = cb;
}


WOLFSSL_API void wolfSSL_CTX_SetIOSetPeer(WOLFSSL_CTX* ctx, CallbackSetPeer cb)
{
    ctx->CBSetPeer = cb;
}

#endif /* WOLFSSL_SESSION_EXPORT */
#endif /* WOLFSSL_DTLS */


#ifdef HAVE_NETX

/* The NetX receive callback
 *  return :  bytes read, or error
 */
int NetX_Receive(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    NetX_Ctx* nxCtx = (NetX_Ctx*)ctx;
    ULONG left;
    ULONG total;
    ULONG copied = 0;
    UINT  status;

    (void)ssl;

    if (nxCtx == NULL || nxCtx->nxSocket == NULL) {
        WOLFSSL_MSG("NetX Recv NULL parameters");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    if (nxCtx->nxPacket == NULL) {
        status = nx_tcp_socket_receive(nxCtx->nxSocket, &nxCtx->nxPacket,
                                       nxCtx->nxWait);
        if (status != NX_SUCCESS) {
            WOLFSSL_MSG("NetX Recv receive error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }

    if (nxCtx->nxPacket) {
        status = nx_packet_length_get(nxCtx->nxPacket, &total);
        if (status != NX_SUCCESS) {
            WOLFSSL_MSG("NetX Recv length get error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }

        left = total - nxCtx->nxOffset;
        status = nx_packet_data_extract_offset(nxCtx->nxPacket, nxCtx->nxOffset,
                                               buf, sz, &copied);
        if (status != NX_SUCCESS) {
            WOLFSSL_MSG("NetX Recv data extract offset error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }

        nxCtx->nxOffset += copied;

        if (copied == left) {
            WOLFSSL_MSG("NetX Recv Drained packet");
            nx_packet_release(nxCtx->nxPacket);
            nxCtx->nxPacket = NULL;
            nxCtx->nxOffset = 0;
        }
    }

    return copied;
}


/* The NetX send callback
 *  return : bytes sent, or error
 */
int NetX_Send(WOLFSSL* ssl, char *buf, int sz, void *ctx)
{
    NetX_Ctx*       nxCtx = (NetX_Ctx*)ctx;
    NX_PACKET*      packet;
    NX_PACKET_POOL* pool;   /* shorthand */
    UINT            status;

    (void)ssl;

    if (nxCtx == NULL || nxCtx->nxSocket == NULL) {
        WOLFSSL_MSG("NetX Send NULL parameters");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    pool = nxCtx->nxSocket->nx_tcp_socket_ip_ptr->nx_ip_default_packet_pool;
    status = nx_packet_allocate(pool, &packet, NX_TCP_PACKET,
                                nxCtx->nxWait);
    if (status != NX_SUCCESS) {
        WOLFSSL_MSG("NetX Send packet alloc error");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    status = nx_packet_data_append(packet, buf, sz, pool, nxCtx->nxWait);
    if (status != NX_SUCCESS) {
        nx_packet_release(packet);
        WOLFSSL_MSG("NetX Send data append error");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    status = nx_tcp_socket_send(nxCtx->nxSocket, packet, nxCtx->nxWait);
    if (status != NX_SUCCESS) {
        nx_packet_release(packet);
        WOLFSSL_MSG("NetX Send socket send error");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    return sz;
}


/* like set_fd, but for default NetX context */
void wolfSSL_SetIO_NetX(WOLFSSL* ssl, NX_TCP_SOCKET* nxSocket, ULONG waitOption)
{
    if (ssl) {
        ssl->nxCtx.nxSocket = nxSocket;
        ssl->nxCtx.nxWait   = waitOption;
    }
}

#endif /* HAVE_NETX */


#ifdef MICRIUM

/* Micrium uTCP/IP port, using the NetSock API
 * TCP and UDP are currently supported with the callbacks below.
 *
 * WOLFSSL_SESSION_EXPORT is not yet supported, would need EmbedGetPeer()
 * and EmbedSetPeer() callbacks implemented.
 *
 * HAVE_CRL is not yet supported, would need an EmbedCrlLookup()
 * callback implemented.
 *
 * HAVE_OCSP is not yet supported, would need an EmbedOCSPLookup()
 * callback implemented.
 */

/* The Micrium uTCP/IP send callback
 * return : bytes sent, or error
 */
int MicriumSend(WOLFSSL* ssl, char* buf, int sz, void* ctx)
{
    NET_SOCK_ID sd = *(int*)ctx;
    NET_SOCK_RTN_CODE ret;
    NET_ERR err;

    ret = NetSock_TxData(sd, buf, sz, ssl->wflags, &err);
    if (ret < 0) {
        WOLFSSL_MSG("Embed Send error");

        if (err == NET_ERR_TX) {
            WOLFSSL_MSG("\tWould block");
            return WOLFSSL_CBIO_ERR_WANT_WRITE;

        } else {
            WOLFSSL_MSG("\tGeneral error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }

    return ret;
}

/* The Micrium uTCP/IP receive callback
 *  return : nb bytes read, or error
 */
int MicriumReceive(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    NET_SOCK_ID sd = *(int*)ctx;
    NET_SOCK_RTN_CODE ret;
    NET_ERR err;

#ifdef WOLFSSL_DTLS
    {
        int dtls_timeout = wolfSSL_dtls_get_current_timeout(ssl);
        if (wolfSSL_dtls(ssl)
                     && !wolfSSL_get_using_nonblock(ssl)
                     && dtls_timeout != 0) {
            /* needs timeout in milliseconds */
            NetSock_CfgTimeoutRxQ_Set(sd, dtls_timeout * 1000, &err);
            if (err != NET_SOCK_ERR_NONE) {
                WOLFSSL_MSG("NetSock_CfgTimeoutRxQ_Set failed");
            }
        }
    }
#endif

    ret = NetSock_RxData(sd, buf, sz, ssl->rflags, &err);
    if (ret < 0) {
        WOLFSSL_MSG("Embed Receive error");

        if (err == NET_ERR_RX || err == NET_SOCK_ERR_RX_Q_EMPTY ||
            err == NET_ERR_FAULT_LOCK_ACQUIRE) {
            if (!wolfSSL_dtls(ssl) || wolfSSL_get_using_nonblock(ssl)) {
                WOLFSSL_MSG("\tWould block");
                return WOLFSSL_CBIO_ERR_WANT_READ;
            }
            else {
                WOLFSSL_MSG("\tSocket timeout");
                return WOLFSSL_CBIO_ERR_TIMEOUT;
            }

        } else if (err == NET_SOCK_ERR_CLOSED) {
            WOLFSSL_MSG("Embed receive connection closed");
            return WOLFSSL_CBIO_ERR_CONN_CLOSE;

        } else {
            WOLFSSL_MSG("\tGeneral error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }

    return ret;
}

/* The Micrium uTCP/IP receivefrom callback
 *  return : nb bytes read, or error
 */
int MicriumReceiveFrom(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    WOLFSSL_DTLS_CTX* dtlsCtx = (WOLFSSL_DTLS_CTX*)ctx;
    NET_SOCK_ID       sd = dtlsCtx->rfd;
    NET_SOCK_ADDR     peer;
    NET_SOCK_ADDR_LEN peerSz = sizeof(peer);
    NET_SOCK_RTN_CODE ret;
    NET_ERR err;
    int dtls_timeout = wolfSSL_dtls_get_current_timeout(ssl);

    WOLFSSL_ENTER("MicriumReceiveFrom()");

    if (ssl->options.handShakeDone)
        dtls_timeout = 0;

    if (!wolfSSL_get_using_nonblock(ssl)) {
        /* needs timeout in milliseconds */
        NetSock_CfgTimeoutRxQ_Set(sd, dtls_timeout * 1000, &err);
        if (err != NET_SOCK_ERR_NONE) {
            WOLFSSL_MSG("NetSock_CfgTimeoutRxQ_Set failed");
        }
    }

    ret = NetSock_RxDataFrom(sd, buf, sz, ssl->rflags, &peer, &peerSz,
                             0, 0, 0, &err);
    if (ret < 0) {
        WOLFSSL_MSG("Embed Receive From error");

        if (err == NET_ERR_RX || err == NET_SOCK_ERR_RX_Q_EMPTY ||
            err == NET_ERR_FAULT_LOCK_ACQUIRE) {
            if (wolfSSL_get_using_nonblock(ssl)) {
                WOLFSSL_MSG("\tWould block");
                return WOLFSSL_CBIO_ERR_WANT_READ;
            }
            else {
                WOLFSSL_MSG("\tSocket timeout");
                return WOLFSSL_CBIO_ERR_TIMEOUT;
            }
        } else {
            WOLFSSL_MSG("\tGeneral error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }
    else {
        if (dtlsCtx->peer.sz > 0
                && peerSz != (NET_SOCK_ADDR_LEN)dtlsCtx->peer.sz
                && XMEMCMP(&peer, dtlsCtx->peer.sa, peerSz) != 0) {
            WOLFSSL_MSG("\tIgnored packet from invalid peer");
            return WOLFSSL_CBIO_ERR_WANT_READ;
        }
    }

    return ret;
}

/* The Micrium uTCP/IP sendto callback
 *  return : nb bytes sent, or error
 */
int MicriumSendTo(WOLFSSL* ssl, char *buf, int sz, void *ctx)
{
    WOLFSSL_DTLS_CTX* dtlsCtx = (WOLFSSL_DTLS_CTX*)ctx;
    NET_SOCK_ID sd = dtlsCtx->wfd;
    NET_SOCK_RTN_CODE ret;
    int len = sz;
    NET_ERR err;

    WOLFSSL_ENTER("MicriumSendTo()");

    ret = NetSock_TxDataTo(sd, &buf[sz - len], len, ssl->wflags,
                           (NET_SOCK_ADDR*)dtlsCtx->peer.sa,
                           (NET_SOCK_ADDR_LEN)dtlsCtx->peer.sz,
                           &err);
    if (err < 0) {
        WOLFSSL_MSG("Embed Send To error");

        if (err == NET_ERR_TX) {
            WOLFSSL_MSG("\tWould block");
            return WOLFSSL_CBIO_ERR_WANT_WRITE;

        } else {
            WOLFSSL_MSG("\tGeneral error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }

    return ret;
}

/* Micrium DTLS Generate Cookie callback
 *  return : number of bytes copied into buf, or error
 */
int MicriumGenerateCookie(WOLFSSL* ssl, byte *buf, int sz, void *ctx)
{
    NET_SOCK_ADDR peer;
    NET_SOCK_ADDR_LEN peerSz = sizeof(peer);
    byte digest[SHA_DIGEST_SIZE];
    int  ret = 0;

    (void)ctx;

    XMEMSET(&peer, 0, sizeof(peer));
    if (wolfSSL_dtls_get_peer(ssl, (void*)&peer,
                              (unsigned int*)&peerSz) != SSL_SUCCESS) {
        WOLFSSL_MSG("getpeername failed in MicriumGenerateCookie");
        return GEN_COOKIE_E;
    }

    ret = wc_ShaHash((byte*)&peer, peerSz, digest);
    if (ret != 0)
        return ret;

    if (sz > SHA_DIGEST_SIZE)
        sz = SHA_DIGEST_SIZE;
    XMEMCPY(buf, digest, sz);

    return sz;
}

#endif /* MICRIUM */

#endif /* WOLFCRYPT_ONLY */
