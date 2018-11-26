/* test.h */

#ifndef wolfSSL_TEST_H
#define wolfSSL_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/mem_track.h>
#if defined(OPENSSL_EXTRA) && defined(SHOW_CERTS)
    #include <wolfssl/openssl/ssl.h> /* for domain component NID value */
#endif

#ifdef ATOMIC_USER
    #include <wolfssl/wolfcrypt/aes.h>
    #include <wolfssl/wolfcrypt/arc4.h>
    #include <wolfssl/wolfcrypt/hmac.h>
#endif
#ifdef HAVE_PK_CALLBACKS
    #include <wolfssl/wolfcrypt/asn.h>
    #ifndef NO_RSA
        #include <wolfssl/wolfcrypt/rsa.h>
    #endif
    #ifdef HAVE_ECC
        #include <wolfssl/wolfcrypt/ecc.h>
    #endif /* HAVE_ECC */
    #ifndef NO_DH
        #include <wolfssl/wolfcrypt/dh.h>
    #endif /* !NO_DH */
    #ifdef HAVE_ED25519
        #include <wolfssl/wolfcrypt/ed25519.h>
    #endif /* HAVE_ED25519 */
    #ifdef HAVE_CURVE25519
        #include <wolfssl/wolfcrypt/curve25519.h>
    #endif /* HAVE_ECC */
#endif /*HAVE_PK_CALLBACKS */

#ifdef USE_WINDOWS_API
    #include <winsock2.h>
    #include <process.h>
    #ifdef TEST_IPV6            /* don't require newer SDK for IPV4 */
        #include <ws2tcpip.h>
        #include <wspiapi.h>
    #endif
    #define SOCKET_T SOCKET
    #define SNPRINTF _snprintf
#elif defined(WOLFSSL_MDK_ARM) || defined(WOLFSSL_KEIL_TCP_NET)
    #include <string.h>
    #include "rl_net.h"
    #define SOCKET_T int
        typedef int socklen_t ;
        static unsigned long inet_addr(const char *cp)
    {
        unsigned int a[4] ; unsigned long ret ;
        sscanf(cp, "%d.%d.%d.%d", &a[0], &a[1], &a[2], &a[3]) ;
        ret = ((a[3]<<24) + (a[2]<<16) + (a[1]<<8) + a[0]) ;
        return(ret) ;
    }
        #if defined(HAVE_KEIL_RTX)
        #define sleep(t) os_dly_wait(t/1000+1) ;
    #elif defined (WOLFSSL_CMSIS_RTOS)
        #define sleep(t)  osDelay(t/1000+1) ;
    #endif
#elif defined(WOLFSSL_TIRTOS)
    #include <string.h>
    #include <netdb.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <ti/sysbios/knl/Task.h>
    struct hostent {
        char *h_name; /* official name of host */
        char **h_aliases; /* alias list */
        int h_addrtype; /* host address type */
        int h_length; /* length of address */
        char **h_addr_list; /* list of addresses from name server */
    };
    #define SOCKET_T int
#elif defined(WOLFSSL_VXWORKS)
    #include <hostLib.h>
    #include <sockLib.h>
    #include <arpa/inet.h>
    #include <string.h>
    #include <selectLib.h>
    #include <sys/types.h>
    #include <netinet/in.h>
    #include <fcntl.h>
    #include <sys/time.h>
    #include <netdb.h>
    #include <pthread.h>
    #define SOCKET_T int
#else
    #include <string.h>
    #include <sys/types.h>
#ifndef WOLFSSL_LEANPSK
    #include <unistd.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <arpa/inet.h>
    #include <sys/ioctl.h>
    #include <sys/time.h>
    #include <sys/socket.h>
    #include <pthread.h>
    #include <fcntl.h>
    #ifdef TEST_IPV6
        #include <netdb.h>
    #endif
#endif
    #define SOCKET_T int
    #ifndef SO_NOSIGPIPE
        #include <signal.h>  /* ignore SIGPIPE */
    #endif
    #define SNPRINTF snprintf
#endif /* USE_WINDOWS_API */

#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif
#ifdef HAVE_CAVIUM
    #include <wolfssl/wolfcrypt/port/cavium/cavium_nitrox.h>
#endif
#ifdef _MSC_VER
    /* disable conversion warning */
    /* 4996 warning to use MS extensions e.g., strcpy_s instead of strncpy */
    #pragma warning(disable:4244 4996)
#endif

/* Buffer for benchmark tests */
#ifndef TEST_BUFFER_SIZE
#define TEST_BUFFER_SIZE 16384
#endif

#ifndef WOLFSSL_HAVE_MIN
    #define WOLFSSL_HAVE_MIN
    static INLINE word32 min(word32 a, word32 b)
    {
        return a > b ? b : a;
    }
#endif /* WOLFSSL_HAVE_MIN */

/* Socket Handling */
#ifndef WOLFSSL_SOCKET_INVALID
#ifdef USE_WINDOWS_API
    #define WOLFSSL_SOCKET_INVALID  ((SOCKET_T)INVALID_SOCKET)
#elif defined(WOLFSSL_TIRTOS)
    #define WOLFSSL_SOCKET_INVALID  ((SOCKET_T)-1)
#else
    #define WOLFSSL_SOCKET_INVALID  (SOCKET_T)(0)
#endif
#endif /* WOLFSSL_SOCKET_INVALID */

#ifndef WOLFSSL_SOCKET_IS_INVALID
#if defined(USE_WINDOWS_API) || defined(WOLFSSL_TIRTOS)
    #define WOLFSSL_SOCKET_IS_INVALID(s)  ((SOCKET_T)(s) == WOLFSSL_SOCKET_INVALID)
#else
    #define WOLFSSL_SOCKET_IS_INVALID(s)  ((SOCKET_T)(s) < WOLFSSL_SOCKET_INVALID)
#endif
#endif /* WOLFSSL_SOCKET_IS_INVALID */

#if defined(__MACH__) || defined(USE_WINDOWS_API)
    #ifndef _SOCKLEN_T
        typedef int socklen_t;
    #endif
#endif


/* HPUX doesn't use socklent_t for third parameter to accept, unless
   _XOPEN_SOURCE_EXTENDED is defined */
#if !defined(__hpux__) && !defined(WOLFSSL_MDK_ARM) && !defined(WOLFSSL_IAR_ARM)\
 && !defined(WOLFSSL_ROWLEY_ARM)  && !defined(WOLFSSL_KEIL_TCP_NET)
    typedef socklen_t* ACCEPT_THIRD_T;
#else
    #if defined _XOPEN_SOURCE_EXTENDED
        typedef socklen_t* ACCEPT_THIRD_T;
    #else
        typedef int*       ACCEPT_THIRD_T;
    #endif
#endif



#ifdef SINGLE_THREADED
    typedef unsigned int  THREAD_RETURN;
    typedef void*         THREAD_TYPE;
    #define WOLFSSL_THREAD
#else
    #if defined(_POSIX_THREADS) && !defined(__MINGW32__)
        typedef void*         THREAD_RETURN;
        typedef pthread_t     THREAD_TYPE;
        #define WOLFSSL_THREAD
        #define INFINITE -1
        #define WAIT_OBJECT_0 0L
    #elif defined(WOLFSSL_MDK_ARM)|| defined(WOLFSSL_KEIL_TCP_NET)
        typedef unsigned int  THREAD_RETURN;
        typedef int           THREAD_TYPE;
        #define WOLFSSL_THREAD
    #elif defined(WOLFSSL_TIRTOS)
        typedef void          THREAD_RETURN;
        typedef Task_Handle   THREAD_TYPE;
        #define WOLFSSL_THREAD
    #else
        typedef unsigned int  THREAD_RETURN;
        typedef intptr_t      THREAD_TYPE;
        #define WOLFSSL_THREAD __stdcall
    #endif
#endif


#ifdef TEST_IPV6
    typedef struct sockaddr_in6 SOCKADDR_IN_T;
    #define AF_INET_V    AF_INET6
#else
    typedef struct sockaddr_in  SOCKADDR_IN_T;
    #define AF_INET_V    AF_INET
#endif


#ifndef WOLFSSL_NO_TLS12
#define SERVER_DEFAULT_VERSION 3
#else
#define SERVER_DEFAULT_VERSION 4
#endif
#define SERVER_DTLS_DEFAULT_VERSION (-2)
#define SERVER_INVALID_VERSION (-99)
#define SERVER_DOWNGRADE_VERSION (-98)
#ifndef WOLFSSL_NO_TLS12
#define CLIENT_DEFAULT_VERSION 3
#else
#define CLIENT_DEFAULT_VERSION 4
#endif
#define CLIENT_DTLS_DEFAULT_VERSION (-2)
#define CLIENT_INVALID_VERSION (-99)
#define CLIENT_DOWNGRADE_VERSION (-98)
#if !defined(NO_FILESYSTEM) && defined(WOLFSSL_MAX_STRENGTH)
    #define DEFAULT_MIN_DHKEY_BITS 2048
#else
    #define DEFAULT_MIN_DHKEY_BITS 1024
#endif
#if !defined(NO_FILESYSTEM) && defined(WOLFSSL_MAX_STRENGTH)
    #define DEFAULT_MIN_RSAKEY_BITS 2048
#else
    #define DEFAULT_MIN_RSAKEY_BITS 1024
#endif
#if !defined(NO_FILESYSTEM) && defined(WOLFSSL_MAX_STRENGTH)
    #define DEFAULT_MIN_ECCKEY_BITS 256
#else
    #define DEFAULT_MIN_ECCKEY_BITS 224
#endif

/* all certs relative to wolfSSL home directory now */
#if defined(WOLFSSL_NO_CURRDIR) || defined(WOLFSSL_MDK_SHELL)
#define caCertFile     "certs/ca-cert.pem"
#define eccCertFile    "certs/server-ecc.pem"
#define eccKeyFile     "certs/ecc-key.pem"
#define eccRsaCertFile "certs/server-ecc-rsa.pem"
#define svrCertFile    "certs/server-cert.pem"
#define svrKeyFile     "certs/server-key.pem"
#define cliCertFile    "certs/client-cert.pem"
#define cliCertDerFile "certs/client-cert.der"
#define cliKeyFile     "certs/client-key.pem"
#define ntruCertFile   "certs/ntru-cert.pem"
#define ntruKeyFile    "certs/ntru-key.raw"
#define dhParamFile    "certs/dh2048.pem"
#define cliEccKeyFile  "certs/ecc-client-key.pem"
#define cliEccCertFile "certs/client-ecc-cert.pem"
#define caEccCertFile  "certs/ca-ecc-cert/pem"
#define crlPemDir      "certs/crl"
#ifdef HAVE_WNR
    /* Whitewood netRandom default config file */
    #define wnrConfig  "wnr-example.conf"
#endif
#else
#define caCertFile     "./certs/ca-cert.pem"
#define eccCertFile    "./certs/server-ecc.pem"
#define eccKeyFile     "./certs/ecc-key.pem"
#define eccRsaCertFile "./certs/server-ecc-rsa.pem"
#define svrCertFile    "./certs/server-cert.pem"
#define svrKeyFile     "./certs/server-key.pem"
#define cliCertFile    "./certs/client-cert.pem"
#define cliCertDerFile "./certs/client-cert.der"
#define cliKeyFile     "./certs/client-key.pem"
#define ntruCertFile   "./certs/ntru-cert.pem"
#define ntruKeyFile    "./certs/ntru-key.raw"
#define dhParamFile    "./certs/dh2048.pem"
#define cliEccKeyFile  "./certs/ecc-client-key.pem"
#define cliEccCertFile "./certs/client-ecc-cert.pem"
#define caEccCertFile  "./certs/ca-ecc-cert.pem"
#define crlPemDir      "./certs/crl"
#ifdef HAVE_WNR
    /* Whitewood netRandom default config file */
    #define wnrConfig  "./wnr-example.conf"
#endif
#endif

typedef struct tcp_ready {
    word16 ready;              /* predicate */
    word16 port;
    char*  srfName;     /* server ready file name */
#if defined(_POSIX_THREADS) && !defined(__MINGW32__)
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
#endif
} tcp_ready;


static INLINE void InitTcpReady(tcp_ready* ready)
{
    ready->ready = 0;
    ready->port = 0;
    ready->srfName = NULL;
#ifdef SINGLE_THREADED
#elif defined(_POSIX_THREADS) && !defined(__MINGW32__)
    pthread_mutex_init(&ready->mutex, 0);
    pthread_cond_init(&ready->cond, 0);
#endif
}


static INLINE void FreeTcpReady(tcp_ready* ready)
{
#ifdef SINGLE_THREADED
    (void)ready;
#elif defined(_POSIX_THREADS) && !defined(__MINGW32__)
    pthread_mutex_destroy(&ready->mutex);
    pthread_cond_destroy(&ready->cond);
#else
    (void)ready;
#endif
}

typedef WOLFSSL_METHOD* (*method_provider)(void);
typedef void (*ctx_callback)(WOLFSSL_CTX* ctx);
typedef void (*ssl_callback)(WOLFSSL* ssl);

typedef struct callback_functions {
    method_provider method;
    ctx_callback ctx_ready;
    ssl_callback ssl_ready;
    ssl_callback on_result;
} callback_functions;

typedef struct func_args {
    int    argc;
    char** argv;
    int    return_code;
    tcp_ready* signal;
    callback_functions *callbacks;
} func_args;




void wait_tcp_ready(func_args*);

typedef THREAD_RETURN WOLFSSL_THREAD THREAD_FUNC(void*);

void start_thread(THREAD_FUNC, func_args*, THREAD_TYPE*);
void join_thread(THREAD_TYPE);

/* wolfSSL */
#ifndef TEST_IPV6
    static const char* const wolfSSLIP   = "127.0.0.1";
#else
    static const char* const wolfSSLIP   = "::1";
#endif
static const word16      wolfSSLPort = 11111;


static INLINE WC_NORETURN void err_sys(const char* msg)
{
    printf("wolfSSL error: %s\n", msg);

#if !defined(__GNUC__)
    /* scan-build (which pretends to be gnuc) can get confused and think the
     * msg pointer can be null even when hardcoded and then it won't exit,
     * making null pointer checks above the err_sys() call useless.
     * We could just always exit() but some compilers will complain about no
     * possible return, with gcc we know the attribute to handle that with
     * WC_NORETURN. */
    if (msg)
#endif
    {
        exit(EXIT_FAILURE);
    }
}


#define MY_EX_USAGE 2

extern int   myoptind;
extern char* myoptarg;

static INLINE int mygetopt(int argc, char** argv, const char* optstring)
{
    static char* next = NULL;

    char  c;
    char* cp;

    if (myoptind == 0)
        next = NULL;   /* we're starting new/over */

    if (next == NULL || *next == '\0') {
        if (myoptind == 0)
            myoptind++;

        if (myoptind >= argc || argv[myoptind][0] != '-' ||
                                argv[myoptind][1] == '\0') {
            myoptarg = NULL;
            if (myoptind < argc)
                myoptarg = argv[myoptind];

            return -1;
        }

        if (strcmp(argv[myoptind], "--") == 0) {
            myoptind++;
            myoptarg = NULL;

            if (myoptind < argc)
                myoptarg = argv[myoptind];

            return -1;
        }

        next = argv[myoptind];
        next++;                  /* skip - */
        myoptind++;
    }

    c  = *next++;
    /* The C++ strchr can return a different value */
    cp = (char*)strchr(optstring, c);

    if (cp == NULL || c == ':')
        return '?';

    cp++;

    if (*cp == ':') {
        if (*next != '\0') {
            myoptarg = next;
            next     = NULL;
        }
        else if (myoptind < argc) {
            myoptarg = argv[myoptind];
            myoptind++;
        }
        else
            return '?';
    }

    return c;
}


#ifdef WOLFSSL_ENCRYPTED_KEYS

static INLINE int PasswordCallBack(char* passwd, int sz, int rw, void* userdata)
{
    (void)rw;
    (void)userdata;
    if (userdata != NULL) {
        strncpy(passwd, (char*)userdata, sz);
        return (int)XSTRLEN((char*)userdata);
    }
    else {
        strncpy(passwd, "yassl123", sz);
        return 8;
    }
}

#endif


#if defined(KEEP_PEER_CERT) || defined(SESSION_CERTS)

static INLINE void ShowX509(WOLFSSL_X509* x509, const char* hdr)
{
    char* altName;
    char* issuer;
    char* subject;
    byte  serial[32];
    int   ret;
    int   sz = sizeof(serial);

    if (x509 == NULL) {
        printf("%s No Cert\n", hdr);
        return;
    }

    issuer  = wolfSSL_X509_NAME_oneline(
                                      wolfSSL_X509_get_issuer_name(x509), 0, 0);
    subject = wolfSSL_X509_NAME_oneline(
                                     wolfSSL_X509_get_subject_name(x509), 0, 0);

    printf("%s\n issuer : %s\n subject: %s\n", hdr, issuer, subject);

    while ( (altName = wolfSSL_X509_get_next_altname(x509)) != NULL)
        printf(" altname = %s\n", altName);

    ret = wolfSSL_X509_get_serial_number(x509, serial, &sz);
    if (ret == WOLFSSL_SUCCESS) {
        int  i;
        int  strLen;
        char serialMsg[80];

        /* testsuite has multiple threads writing to stdout, get output
           message ready to write once */
        strLen = sprintf(serialMsg, " serial number");
        for (i = 0; i < sz; i++)
            sprintf(serialMsg + strLen + (i*3), ":%02x ", serial[i]);
        printf("%s\n", serialMsg);
    }

    XFREE(subject, 0, DYNAMIC_TYPE_OPENSSL);
    XFREE(issuer,  0, DYNAMIC_TYPE_OPENSSL);

#if defined(OPENSSL_EXTRA) && defined(SHOW_CERTS)
    {
        WOLFSSL_BIO* bio;
        char buf[256]; /* should be size of ASN_NAME_MAX */
        int  textSz;


        /* print out domain component if certificate has it */
        textSz = wolfSSL_X509_NAME_get_text_by_NID(
                wolfSSL_X509_get_subject_name(x509), NID_domainComponent,
                buf, sizeof(buf));
        if (textSz > 0) {
            printf("Domain Component = %s\n", buf);
        }

        bio = wolfSSL_BIO_new(wolfSSL_BIO_s_file());
        if (bio != NULL) {
            wolfSSL_BIO_set_fp(bio, stdout, BIO_NOCLOSE);
            wolfSSL_X509_print(bio, x509);
            wolfSSL_BIO_free(bio);
        }
    }
#endif
}

#endif /* KEEP_PEER_CERT || SESSION_CERTS */

#if defined(SESSION_CERTS) && defined(SHOW_CERTS)
static INLINE void ShowX509Chain(WOLFSSL_X509_CHAIN* chain, int count,
    const char* hdr)
{
    int i;
    int length;
    unsigned char buffer[3072];
    WOLFSSL_X509* chainX509;

    for (i = 0; i < count; i++) {
        wolfSSL_get_chain_cert_pem(chain, i, buffer, sizeof(buffer), &length);
        buffer[length] = 0;
        printf("\n%s: %d has length %d data = \n%s\n", hdr, i, length, buffer);

        chainX509 = wolfSSL_get_chain_X509(chain, i);
        if (chainX509)
            ShowX509(chainX509, hdr);
        else
            printf("get_chain_X509 failed\n");
        wolfSSL_FreeX509(chainX509);
    }
}
#endif

static INLINE void showPeer(WOLFSSL* ssl)
{
    WOLFSSL_CIPHER* cipher;
#ifdef HAVE_ECC
    const char *name;
#endif
#ifndef NO_DH
    int bits;
#endif
#ifdef KEEP_PEER_CERT
    WOLFSSL_X509* peer = wolfSSL_get_peer_certificate(ssl);
    if (peer)
        ShowX509(peer, "peer's cert info:");
    else
        printf("peer has no cert!\n");
    wolfSSL_FreeX509(peer);
#endif
#if defined(SHOW_CERTS) && defined(OPENSSL_EXTRA) && defined(KEEP_OUR_CERT)
    ShowX509(wolfSSL_get_certificate(ssl), "our cert info:");
    printf("Peer verify result = %lu\n", wolfSSL_get_verify_result(ssl));
#endif /* SHOW_CERTS */
    printf("SSL version is %s\n", wolfSSL_get_version(ssl));

    cipher = wolfSSL_get_current_cipher(ssl);
#ifdef HAVE_QSH
    printf("SSL cipher suite is %s%s\n", (wolfSSL_isQSH(ssl))? "QSH:": "",
            wolfSSL_CIPHER_get_name(cipher));
#else
    printf("SSL cipher suite is %s\n", wolfSSL_CIPHER_get_name(cipher));
#endif
#ifdef HAVE_ECC
    if ((name = wolfSSL_get_curve_name(ssl)) != NULL)
        printf("SSL curve name is %s\n", name);
#endif
#ifndef NO_DH
    if ((bits = wolfSSL_GetDhKey_Sz(ssl)) > 0)
        printf("SSL DH size is %d bits\n", bits);
#endif
    if (wolfSSL_session_reused(ssl))
        printf("SSL reused session\n");
#ifdef WOLFSSL_ALT_CERT_CHAINS
    if (wolfSSL_is_peer_alt_cert_chain(ssl))
        printf("Alternate cert chain used\n");
#endif

#if defined(SESSION_CERTS) && defined(SHOW_CERTS)
    {
        WOLFSSL_X509_CHAIN* chain;

        chain = wolfSSL_get_peer_chain(ssl);
        ShowX509Chain(chain, wolfSSL_get_chain_count(chain), "session cert");

    #ifdef WOLFSSL_ALT_CERT_CHAINS
        if (wolfSSL_is_peer_alt_cert_chain(ssl)) {
            chain = wolfSSL_get_peer_alt_chain(ssl);
            ShowX509Chain(chain, wolfSSL_get_chain_count(chain), "alt cert");
        }
    #endif
    }
#endif /* SESSION_CERTS && SHOW_CERTS */
  (void)ssl;
}


static INLINE void build_addr(SOCKADDR_IN_T* addr, const char* peer,
                              word16 port, int udp, int sctp)
{
    int useLookup = 0;
    (void)useLookup;
    (void)udp;
    (void)sctp;

    if (addr == NULL)
        err_sys("invalid argument to build_addr, addr is NULL");

    XMEMSET(addr, 0, sizeof(SOCKADDR_IN_T));

#ifndef TEST_IPV6
    /* peer could be in human readable form */
    if ( ((size_t)peer != INADDR_ANY) && isalpha((int)peer[0])) {
        #if defined(WOLFSSL_MDK_ARM) || defined(WOLFSSL_KEIL_TCP_NET)
            int err;
            struct hostent* entry = gethostbyname(peer, &err);
        #elif defined(WOLFSSL_TIRTOS)
            struct hostent* entry = DNSGetHostByName(peer);
        #elif defined(WOLFSSL_VXWORKS)
            struct hostent* entry = (struct hostent*)hostGetByName((char*)peer);
        #else
            struct hostent* entry = gethostbyname(peer);
        #endif

        if (entry) {
            XMEMCPY(&addr->sin_addr.s_addr, entry->h_addr_list[0],
                   entry->h_length);
            useLookup = 1;
        }
        else
            err_sys("no entry for host");
    }
#endif


#ifndef TEST_IPV6
    #if defined(WOLFSSL_MDK_ARM) || defined(WOLFSSL_KEIL_TCP_NET)
        addr->sin_family = PF_INET;
    #else
        addr->sin_family = AF_INET_V;
    #endif
    addr->sin_port = XHTONS(port);
    if ((size_t)peer == INADDR_ANY)
        addr->sin_addr.s_addr = INADDR_ANY;
    else {
        if (!useLookup)
            addr->sin_addr.s_addr = inet_addr(peer);
    }
#else
    addr->sin6_family = AF_INET_V;
    addr->sin6_port = XHTONS(port);
    if ((size_t)peer == INADDR_ANY) {
        addr->sin6_addr = in6addr_any;
    }
    else {
        #ifdef HAVE_GETADDRINFO
            struct addrinfo  hints;
            struct addrinfo* answer = NULL;
            int    ret;
            char   strPort[80];

            XMEMSET(&hints, 0, sizeof(hints));

            hints.ai_family   = AF_INET_V;
            if (udp) {
                hints.ai_socktype = SOCK_DGRAM;
                hints.ai_protocol = IPPROTO_UDP;
            }
        #ifdef WOLFSSL_SCTP
            else if (sctp) {
                hints.ai_socktype = SOCK_STREAM;
                hints.ai_protocol = IPPROTO_SCTP;
            }
        #endif
            else {
                hints.ai_socktype = SOCK_STREAM;
                hints.ai_protocol = IPPROTO_TCP;
            }

            SNPRINTF(strPort, sizeof(strPort), "%d", port);
            strPort[79] = '\0';

            ret = getaddrinfo(peer, strPort, &hints, &answer);
            if (ret < 0 || answer == NULL)
                err_sys("getaddrinfo failed");

            XMEMCPY(addr, answer->ai_addr, answer->ai_addrlen);
            freeaddrinfo(answer);
        #else
            printf("no ipv6 getaddrinfo, loopback only tests/examples\n");
            addr->sin6_addr = in6addr_loopback;
        #endif
    }
#endif
}


static INLINE void tcp_socket(SOCKET_T* sockfd, int udp, int sctp)
{
    (void)sctp;

    if (udp)
        *sockfd = socket(AF_INET_V, SOCK_DGRAM, IPPROTO_UDP);
#ifdef WOLFSSL_SCTP
    else if (sctp)
        *sockfd = socket(AF_INET_V, SOCK_STREAM, IPPROTO_SCTP);
#endif
    else
        *sockfd = socket(AF_INET_V, SOCK_STREAM, IPPROTO_TCP);

    if(WOLFSSL_SOCKET_IS_INVALID(*sockfd)) {
        err_sys("socket failed\n");
    }

#ifndef USE_WINDOWS_API
#ifdef SO_NOSIGPIPE
    {
        int       on = 1;
        socklen_t len = sizeof(on);
        int       res = setsockopt(*sockfd, SOL_SOCKET, SO_NOSIGPIPE, &on, len);
        if (res < 0)
            err_sys("setsockopt SO_NOSIGPIPE failed\n");
    }
#elif defined(WOLFSSL_MDK_ARM) || defined (WOLFSSL_TIRTOS) ||\
                                          defined(WOLFSSL_KEIL_TCP_NET)
    /* nothing to define */
#else  /* no S_NOSIGPIPE */
    signal(SIGPIPE, SIG_IGN);
#endif /* S_NOSIGPIPE */

#if defined(TCP_NODELAY)
    if (!udp && !sctp)
    {
        int       on = 1;
        socklen_t len = sizeof(on);
        int       res = setsockopt(*sockfd, IPPROTO_TCP, TCP_NODELAY, &on, len);
        if (res < 0)
            err_sys("setsockopt TCP_NODELAY failed\n");
    }
#endif
#endif  /* USE_WINDOWS_API */
}

static INLINE void tcp_connect(SOCKET_T* sockfd, const char* ip, word16 port,
                               int udp, int sctp, WOLFSSL* ssl)
{
    SOCKADDR_IN_T addr;
    build_addr(&addr, ip, port, udp, sctp);
    if (udp) {
        wolfSSL_dtls_set_peer(ssl, &addr, sizeof(addr));
    }
    tcp_socket(sockfd, udp, sctp);

    if (!udp) {
        if (connect(*sockfd, (const struct sockaddr*)&addr, sizeof(addr)) != 0)
            err_sys("tcp connect failed");
    }
}


static INLINE void udp_connect(SOCKET_T* sockfd, void* addr, int addrSz)
{
    if (connect(*sockfd, (const struct sockaddr*)addr, addrSz) != 0)
        err_sys("tcp connect failed");
}


enum {
    TEST_SELECT_FAIL,
    TEST_TIMEOUT,
    TEST_RECV_READY,
    TEST_ERROR_READY
};


#if !defined(WOLFSSL_MDK_ARM) && !defined(WOLFSSL_KEIL_TCP_NET) && \
                                 !defined(WOLFSSL_TIRTOS)
static INLINE int tcp_select(SOCKET_T socketfd, int to_sec)
{
    fd_set recvfds, errfds;
    SOCKET_T nfds = socketfd + 1;
#if !defined(__INTEGRITY)
    struct timeval timeout = {(to_sec > 0) ? to_sec : 0, 0};
#else
    struct timeval timeout;
#endif
    int result;

    FD_ZERO(&recvfds);
    FD_SET(socketfd, &recvfds);
    FD_ZERO(&errfds);
    FD_SET(socketfd, &errfds);

#if defined(__INTEGRITY)
    timeout.tv_sec = (long long)(to_sec > 0) ? to_sec : 0, 0;
#endif
    result = select(nfds, &recvfds, NULL, &errfds, &timeout);

    if (result == 0)
        return TEST_TIMEOUT;
    else if (result > 0) {
        if (FD_ISSET(socketfd, &recvfds))
            return TEST_RECV_READY;
        else if(FD_ISSET(socketfd, &errfds))
            return TEST_ERROR_READY;
    }

    return TEST_SELECT_FAIL;
}
#elif defined(WOLFSSL_TIRTOS) || defined(WOLFSSL_KEIL_TCP_NET)
static INLINE int tcp_select(SOCKET_T socketfd, int to_sec)
{
    return TEST_RECV_READY;
}
#endif /* !WOLFSSL_MDK_ARM */


static INLINE void tcp_listen(SOCKET_T* sockfd, word16* port, int useAnyAddr,
                              int udp, int sctp)
{
    SOCKADDR_IN_T addr;

    /* don't use INADDR_ANY by default, firewall may block, make user switch
       on */
    build_addr(&addr, (useAnyAddr ? INADDR_ANY : wolfSSLIP), *port, udp, sctp);
    tcp_socket(sockfd, udp, sctp);

#if !defined(USE_WINDOWS_API) && !defined(WOLFSSL_MDK_ARM)\
                              && !defined(WOLFSSL_KEIL_TCP_NET)
    {
        int       res, on  = 1;
        socklen_t len = sizeof(on);
        res = setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &on, len);
        if (res < 0)
            err_sys("setsockopt SO_REUSEADDR failed\n");
    }
#endif

    if (bind(*sockfd, (const struct sockaddr*)&addr, sizeof(addr)) != 0)
        err_sys("tcp bind failed");
    if (!udp) {
        #ifdef WOLFSSL_KEIL_TCP_NET
            #define SOCK_LISTEN_MAX_QUEUE 1
        #else
            #define SOCK_LISTEN_MAX_QUEUE 5
        #endif
        if (listen(*sockfd, SOCK_LISTEN_MAX_QUEUE) != 0)
                err_sys("tcp listen failed");
    }
    #if !defined(USE_WINDOWS_API) && !defined(WOLFSSL_TIRTOS)
        if (*port == 0) {
            socklen_t len = sizeof(addr);
            if (getsockname(*sockfd, (struct sockaddr*)&addr, &len) == 0) {
                #ifndef TEST_IPV6
                    *port = XNTOHS(addr.sin_port);
                #else
                    *port = XNTOHS(addr.sin6_port);
                #endif
            }
        }
    #endif
}


#if 0
static INLINE int udp_read_connect(SOCKET_T sockfd)
{
    SOCKADDR_IN_T cliaddr;
    byte          b[1500];
    int           n;
    socklen_t     len = sizeof(cliaddr);

    n = (int)recvfrom(sockfd, (char*)b, sizeof(b), MSG_PEEK,
                      (struct sockaddr*)&cliaddr, &len);
    if (n > 0) {
        if (connect(sockfd, (const struct sockaddr*)&cliaddr,
                    sizeof(cliaddr)) != 0)
            err_sys("udp connect failed");
    }
    else
        err_sys("recvfrom failed");

    return sockfd;
}
#endif

static INLINE void udp_accept(SOCKET_T* sockfd, SOCKET_T* clientfd,
                              int useAnyAddr, word16 port, func_args* args)
{
    SOCKADDR_IN_T addr;

    (void)args;
    build_addr(&addr, (useAnyAddr ? INADDR_ANY : wolfSSLIP), port, 1, 0);
    tcp_socket(sockfd, 1, 0);


#if !defined(USE_WINDOWS_API) && !defined(WOLFSSL_MDK_ARM) \
                              && !defined(WOLFSSL_KEIL_TCP_NET)
    {
        int       res, on  = 1;
        socklen_t len = sizeof(on);
        res = setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &on, len);
        if (res < 0)
            err_sys("setsockopt SO_REUSEADDR failed\n");
    }
#endif

    if (bind(*sockfd, (const struct sockaddr*)&addr, sizeof(addr)) != 0)
        err_sys("tcp bind failed");

    #if (defined(NO_MAIN_DRIVER) && !defined(USE_WINDOWS_API)) && !defined(WOLFSSL_TIRTOS)
        if (port == 0) {
            socklen_t len = sizeof(addr);
            if (getsockname(*sockfd, (struct sockaddr*)&addr, &len) == 0) {
                #ifndef TEST_IPV6
                    port = XNTOHS(addr.sin_port);
                #else
                    port = XNTOHS(addr.sin6_port);
                #endif
            }
        }
    #endif

#if defined(_POSIX_THREADS) && defined(NO_MAIN_DRIVER) && !defined(__MINGW32__)
    /* signal ready to accept data */
    {
    tcp_ready* ready = args->signal;
    pthread_mutex_lock(&ready->mutex);
    ready->ready = 1;
    ready->port = port;
    pthread_cond_signal(&ready->cond);
    pthread_mutex_unlock(&ready->mutex);
    }
#elif defined (WOLFSSL_TIRTOS)
    /* Need mutex? */
    tcp_ready* ready = args->signal;
    ready->ready = 1;
    ready->port = port;
#endif

    *clientfd = *sockfd;
}

static INLINE void tcp_accept(SOCKET_T* sockfd, SOCKET_T* clientfd,
                              func_args* args, word16 port, int useAnyAddr,
                              int udp, int sctp, int ready_file, int do_listen)
{
    SOCKADDR_IN_T client;
    socklen_t client_len = sizeof(client);
    tcp_ready* ready = NULL;

    (void) ready; /* Account for case when "ready" is not used */

    if (udp) {
        udp_accept(sockfd, clientfd, useAnyAddr, port, args);
        return;
    }

    if(do_listen) {
        tcp_listen(sockfd, &port, useAnyAddr, udp, sctp);

    #if defined(_POSIX_THREADS) && defined(NO_MAIN_DRIVER) && !defined(__MINGW32__)
        /* signal ready to tcp_accept */
        if (args)
            ready = args->signal;
        if (ready) {
            pthread_mutex_lock(&ready->mutex);
            ready->ready = 1;
            ready->port = port;
            pthread_cond_signal(&ready->cond);
            pthread_mutex_unlock(&ready->mutex);
        }
    #elif defined (WOLFSSL_TIRTOS)
        /* Need mutex? */
        if (args)
            ready = args->signal;
        if (ready) {
            ready->ready = 1;
            ready->port = port;
        }
    #endif

        if (ready_file) {
        #if !defined(NO_FILESYSTEM) || defined(FORCE_BUFFER_TEST)
            FILE* srf = NULL;
            if (args)
                ready = args->signal;

            if (ready) {
                srf = fopen(ready->srfName, "w");

                if (srf) {
                    /* let's write port sever is listening on to ready file
                       external monitor can then do ephemeral ports by passing
                       -p 0 to server on supported platforms with -R ready_file
                       client can then wait for existence of ready_file and see
                       which port the server is listening on. */
                    fprintf(srf, "%d\n", (int)port);
                    fclose(srf);
                }
            }
        #endif
        }
    }

    *clientfd = accept(*sockfd, (struct sockaddr*)&client,
                      (ACCEPT_THIRD_T)&client_len);
    if(WOLFSSL_SOCKET_IS_INVALID(*clientfd)) {
        err_sys("tcp accept failed");
    }
}


static INLINE void tcp_set_nonblocking(SOCKET_T* sockfd)
{
    #ifdef USE_WINDOWS_API
        unsigned long blocking = 1;
        int ret = ioctlsocket(*sockfd, FIONBIO, &blocking);
        if (ret == SOCKET_ERROR)
            err_sys("ioctlsocket failed");
    #elif defined(WOLFSSL_MDK_ARM) || defined(WOLFSSL_KEIL_TCP_NET) \
        || defined (WOLFSSL_TIRTOS)|| defined(WOLFSSL_VXWORKS)
         /* non blocking not supported, for now */
    #else
        int flags = fcntl(*sockfd, F_GETFL, 0);
        if (flags < 0)
            err_sys("fcntl get failed");
        flags = fcntl(*sockfd, F_SETFL, flags | O_NONBLOCK);
        if (flags < 0)
            err_sys("fcntl set failed");
    #endif
}


#ifndef NO_PSK

/* identity is OpenSSL testing default for openssl s_client, keep same */
static const char* kIdentityStr = "Client_identity";

static INLINE unsigned int my_psk_client_cb(WOLFSSL* ssl, const char* hint,
        char* identity, unsigned int id_max_len, unsigned char* key,
        unsigned int key_max_len)
{
    (void)ssl;
    (void)hint;
    (void)key_max_len;

    /* see internal.h MAX_PSK_ID_LEN for PSK identity limit */
    strncpy(identity, kIdentityStr, id_max_len);

    if (wolfSSL_GetVersion(ssl) < WOLFSSL_TLSV1_3) {
        /* test key in hex is 0x1a2b3c4d , in decimal 439,041,101 , we're using
           unsigned binary */
        key[0] = 0x1a;
        key[1] = 0x2b;
        key[2] = 0x3c;
        key[3] = 0x4d;


        return 4;   /* length of key in octets or 0 for error */
    }
    else {
        int i;
        int b = 0x01;

        for (i = 0; i < 32; i++, b += 0x22) {
            if (b >= 0x100)
                b = 0x01;
            key[i] = b;
        }

        return 32;   /* length of key in octets or 0 for error */
    }
}


static INLINE unsigned int my_psk_server_cb(WOLFSSL* ssl, const char* identity,
        unsigned char* key, unsigned int key_max_len)
{
    (void)ssl;
    (void)key_max_len;

    /* see internal.h MAX_PSK_ID_LEN for PSK identity limit */
    if (strncmp(identity, kIdentityStr, strlen(kIdentityStr)) != 0)
        return 0;

    if (wolfSSL_GetVersion(ssl) < WOLFSSL_TLSV1_3) {
        /* test key in hex is 0x1a2b3c4d , in decimal 439,041,101 , we're using
           unsigned binary */
        key[0] = 0x1a;
        key[1] = 0x2b;
        key[2] = 0x3c;
        key[3] = 0x4d;


        return 4;   /* length of key in octets or 0 for error */
    }
    else {
        int i;
        int b = 0x01;

        for (i = 0; i < 32; i++, b += 0x22) {
            if (b >= 0x100)
                b = 0x01;
            key[i] = b;
        }

        return 32;   /* length of key in octets or 0 for error */
    }
}

#endif /* NO_PSK */


#if defined(WOLFSSL_USER_CURRTIME)
    extern   double current_time(int reset);

#elif defined(USE_WINDOWS_API)

    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>

    static INLINE double current_time(int reset)
    {
        static int init = 0;
        static LARGE_INTEGER freq;

        LARGE_INTEGER count;

        if (!init) {
            QueryPerformanceFrequency(&freq);
            init = 1;
        }

        QueryPerformanceCounter(&count);

        (void)reset;
        return (double)count.QuadPart / freq.QuadPart;
    }

#elif defined(WOLFSSL_TIRTOS)
    extern double current_time();
#else

#if !defined(WOLFSSL_MDK_ARM) && !defined(WOLFSSL_KEIL_TCP_NET) && !defined(WOLFSSL_CHIBIOS)
    #include <sys/time.h>

    static INLINE double current_time(int reset)
    {
        struct timeval tv;
        gettimeofday(&tv, 0);
        (void)reset;

        return (double)tv.tv_sec + (double)tv.tv_usec / 1000000;
    }
#else
    extern double current_time(int reset);
#endif
#endif /* USE_WINDOWS_API */


#if defined(HAVE_OCSP) && defined(WOLFSSL_NONBLOCK_OCSP)
static INLINE int OCSPIOCb(void* ioCtx, const char* url, int urlSz,
    unsigned char* request, int requestSz, unsigned char** response)
{
#ifdef TEST_NONBLOCK_CERTS
    static int ioCbCnt = 0;
#endif

    (void)ioCtx;
    (void)url;
    (void)urlSz;
    (void)request;
    (void)requestSz;
    (void)response;

#ifdef TEST_NONBLOCK_CERTS
    if (ioCbCnt) {
        ioCbCnt = 0;
        return EmbedOcspLookup(ioCtx, url, urlSz, request, requestSz, response);
    }
    else {
        ioCbCnt = 1;
        return WOLFSSL_CBIO_ERR_WANT_READ;
    }
#else
    return EmbedOcspLookup(ioCtx, url, urlSz, request, requestSz, response);
#endif
}

static INLINE void OCSPRespFreeCb(void* ioCtx, unsigned char* response)
{
    (void)ioCtx;
    (void)response;
}
#endif

#if !defined(NO_CERTS)
    #if !defined(NO_FILESYSTEM) || \
        (defined(NO_FILESYSTEM) && defined(FORCE_BUFFER_TEST))

    /* reads file size, allocates buffer, reads into buffer, returns buffer */
    static INLINE int load_file(const char* fname, byte** buf, size_t* bufLen)
    {
        int ret;
        long int fileSz;
        FILE* file;

        if (fname == NULL || buf == NULL || bufLen == NULL)
            return BAD_FUNC_ARG;

        /* set defaults */
        *buf = NULL;
        *bufLen = 0;

        /* open file (read-only binary) */
        file = fopen(fname, "rb");
        if (!file) {
            printf("Error loading %s\n", fname);
            return BAD_PATH_ERROR;
        }

        fseek(file, 0, SEEK_END);
        fileSz = (int)ftell(file);
        rewind(file);
        if (fileSz  > 0) {
            *bufLen = (size_t)fileSz;
            *buf = (byte*)malloc(*bufLen);
            if (*buf == NULL) {
                ret = MEMORY_E;
                printf("Error allocating %lu bytes\n", (unsigned long)*bufLen);
            }
            else {
                size_t readLen = fread(*buf, *bufLen, 1, file);

                /* check response code */
                ret = (readLen > 0) ? 0 : -1;
            }
        }
        else {
            ret = BUFFER_E;
        }
        fclose(file);

        return ret;
    }

    enum {
        WOLFSSL_CA   = 1,
        WOLFSSL_CERT = 2,
        WOLFSSL_KEY  = 3,
        WOLFSSL_CERT_CHAIN = 4,
    };

    static INLINE void load_buffer(WOLFSSL_CTX* ctx, const char* fname, int type)
    {
        int format = WOLFSSL_FILETYPE_PEM;
        byte* buff = NULL;
        size_t sz = 0;

        if (load_file(fname, &buff, &sz) != 0) {
            err_sys("can't open file for buffer load "
                    "Please run from wolfSSL home directory if not");
        }

        /* determine format */
        if (strstr(fname, ".der"))
            format = WOLFSSL_FILETYPE_ASN1;

        if (type == WOLFSSL_CA) {
            if (wolfSSL_CTX_load_verify_buffer(ctx, buff, (long)sz, format)
                                              != WOLFSSL_SUCCESS)
                err_sys("can't load buffer ca file");
        }
        else if (type == WOLFSSL_CERT) {
            if (wolfSSL_CTX_use_certificate_buffer(ctx, buff, (long)sz,
                        format) != WOLFSSL_SUCCESS)
                err_sys("can't load buffer cert file");
        }
        else if (type == WOLFSSL_KEY) {
            if (wolfSSL_CTX_use_PrivateKey_buffer(ctx, buff, (long)sz,
                        format) != WOLFSSL_SUCCESS)
                err_sys("can't load buffer key file");
        }
        else if (type == WOLFSSL_CERT_CHAIN) {
            if (wolfSSL_CTX_use_certificate_chain_buffer_format(ctx, buff,
                    (long)sz, format) != WOLFSSL_SUCCESS)
                err_sys("can't load cert chain buffer");
        }

        if (buff)
            free(buff);
    }

    #ifdef TEST_PK_PRIVKEY
    static INLINE int load_key_file(const char* fname, byte** derBuf, word32* derLen)
    {
        int ret;
        byte* buf = NULL;
        size_t bufLen;

        ret = load_file(fname, &buf, &bufLen);
        if (ret != 0)
            return ret;

        *derBuf = (byte*)malloc(bufLen);
        if (*derBuf == NULL) {
            free(buf);
            return MEMORY_E;
        }

        ret = wc_KeyPemToDer(buf, (word32)bufLen, *derBuf, (word32)bufLen, NULL);
        if (ret < 0) {
            free(buf);
            free(*derBuf);
            return ret;
        }
        *derLen = ret;
        free(buf);

        return 0;
    }
    #endif /* TEST_PK_PRIVKEY */

    #endif /* !NO_FILESYSTEM || (NO_FILESYSTEM && FORCE_BUFFER_TEST) */
#endif /* !NO_CERTS */

static INLINE int myVerify(int preverify, WOLFSSL_X509_STORE_CTX* store)
{
    char buffer[WOLFSSL_MAX_ERROR_SZ];
#ifdef OPENSSL_EXTRA
    WOLFSSL_X509* peer;
#endif
    (void)preverify;

    printf("In verification callback, error = %d, %s\n", store->error,
                                 wolfSSL_ERR_error_string(store->error, buffer));
#ifdef OPENSSL_EXTRA
    peer = store->current_cert;
    if (peer) {
        char* issuer  = wolfSSL_X509_NAME_oneline(
                                       wolfSSL_X509_get_issuer_name(peer), 0, 0);
        char* subject = wolfSSL_X509_NAME_oneline(
                                      wolfSSL_X509_get_subject_name(peer), 0, 0);
        printf("\tPeer's cert info:\n issuer : %s\n subject: %s\n", issuer,
                                                                  subject);
        XFREE(subject, 0, DYNAMIC_TYPE_OPENSSL);
        XFREE(issuer,  0, DYNAMIC_TYPE_OPENSSL);
    }
    else
        printf("\tPeer has no cert!\n");
#else
    printf("\tPeer certs: %d\n", store->totalCerts);
    #ifdef SHOW_CERTS
    {   int i;
        for (i=0; i<store->totalCerts; i++) {
            WOLFSSL_BUFFER_INFO* cert = &store->certs[i];
            printf("\t\tCert %d: Ptr %p, Len %u\n", i, cert->buffer, cert->length);
        }
    }
    #endif
#endif

    printf("\tSubject's domain name is %s\n", store->domain);

    printf("\tAllowing to continue anyway (shouldn't do this, EVER!!!)\n");
    return 1;
}


static INLINE int myDateCb(int preverify, WOLFSSL_X509_STORE_CTX* store)
{
    char buffer[WOLFSSL_MAX_ERROR_SZ];
    (void)preverify;

    printf("In verification callback, error = %d, %s\n", store->error,
                                 wolfSSL_ERR_error_string(store->error, buffer));
    printf("Subject's domain name is %s\n", store->domain);

    if (store->error == ASN_BEFORE_DATE_E || store->error == ASN_AFTER_DATE_E) {
        printf("Overriding cert date error as example for bad clock testing\n");
        return 1;
    }
    printf("Cert error is not date error, not overriding\n");

    return 0;
}


#ifdef HAVE_EXT_CACHE

static INLINE WOLFSSL_SESSION* mySessGetCb(WOLFSSL* ssl, unsigned char* id,
    int id_len, int* copy)
{
    (void)ssl;
    (void)id;
    (void)id_len;
    (void)copy;

    /* using internal cache, this is for testing only */
    return NULL;
}

static INLINE int mySessNewCb(WOLFSSL* ssl, WOLFSSL_SESSION* session)
{
    (void)ssl;
    (void)session;

    /* using internal cache, this is for testing only */
    return 0;
}

static INLINE void mySessRemCb(WOLFSSL_CTX* ctx, WOLFSSL_SESSION* session)
{
    (void)ctx;
    (void)session;

    /* using internal cache, this is for testing only */
}

#endif /* HAVE_EXT_CACHE */


#ifdef HAVE_CRL

static INLINE void CRL_CallBack(const char* url)
{
    printf("CRL callback url = %s\n", url);
}

#endif

#ifndef NO_DH
static INLINE void SetDH(WOLFSSL* ssl)
{
    /* dh1024 p */
    static const unsigned char p[] =
    {
        0xE6, 0x96, 0x9D, 0x3D, 0x49, 0x5B, 0xE3, 0x2C, 0x7C, 0xF1, 0x80, 0xC3,
        0xBD, 0xD4, 0x79, 0x8E, 0x91, 0xB7, 0x81, 0x82, 0x51, 0xBB, 0x05, 0x5E,
        0x2A, 0x20, 0x64, 0x90, 0x4A, 0x79, 0xA7, 0x70, 0xFA, 0x15, 0xA2, 0x59,
        0xCB, 0xD5, 0x23, 0xA6, 0xA6, 0xEF, 0x09, 0xC4, 0x30, 0x48, 0xD5, 0xA2,
        0x2F, 0x97, 0x1F, 0x3C, 0x20, 0x12, 0x9B, 0x48, 0x00, 0x0E, 0x6E, 0xDD,
        0x06, 0x1C, 0xBC, 0x05, 0x3E, 0x37, 0x1D, 0x79, 0x4E, 0x53, 0x27, 0xDF,
        0x61, 0x1E, 0xBB, 0xBE, 0x1B, 0xAC, 0x9B, 0x5C, 0x60, 0x44, 0xCF, 0x02,
        0x3D, 0x76, 0xE0, 0x5E, 0xEA, 0x9B, 0xAD, 0x99, 0x1B, 0x13, 0xA6, 0x3C,
        0x97, 0x4E, 0x9E, 0xF1, 0x83, 0x9E, 0xB5, 0xDB, 0x12, 0x51, 0x36, 0xF7,
        0x26, 0x2E, 0x56, 0xA8, 0x87, 0x15, 0x38, 0xDF, 0xD8, 0x23, 0xC6, 0x50,
        0x50, 0x85, 0xE2, 0x1F, 0x0D, 0xD5, 0xC8, 0x6B,
    };

    /* dh1024 g */
    static const unsigned char g[] =
    {
      0x02,
    };

    wolfSSL_SetTmpDH(ssl, p, sizeof(p), g, sizeof(g));
}

static INLINE void SetDHCtx(WOLFSSL_CTX* ctx)
{
    /* dh1024 p */
    static const unsigned char p[] =
    {
        0xE6, 0x96, 0x9D, 0x3D, 0x49, 0x5B, 0xE3, 0x2C, 0x7C, 0xF1, 0x80, 0xC3,
        0xBD, 0xD4, 0x79, 0x8E, 0x91, 0xB7, 0x81, 0x82, 0x51, 0xBB, 0x05, 0x5E,
        0x2A, 0x20, 0x64, 0x90, 0x4A, 0x79, 0xA7, 0x70, 0xFA, 0x15, 0xA2, 0x59,
        0xCB, 0xD5, 0x23, 0xA6, 0xA6, 0xEF, 0x09, 0xC4, 0x30, 0x48, 0xD5, 0xA2,
        0x2F, 0x97, 0x1F, 0x3C, 0x20, 0x12, 0x9B, 0x48, 0x00, 0x0E, 0x6E, 0xDD,
        0x06, 0x1C, 0xBC, 0x05, 0x3E, 0x37, 0x1D, 0x79, 0x4E, 0x53, 0x27, 0xDF,
        0x61, 0x1E, 0xBB, 0xBE, 0x1B, 0xAC, 0x9B, 0x5C, 0x60, 0x44, 0xCF, 0x02,
        0x3D, 0x76, 0xE0, 0x5E, 0xEA, 0x9B, 0xAD, 0x99, 0x1B, 0x13, 0xA6, 0x3C,
        0x97, 0x4E, 0x9E, 0xF1, 0x83, 0x9E, 0xB5, 0xDB, 0x12, 0x51, 0x36, 0xF7,
        0x26, 0x2E, 0x56, 0xA8, 0x87, 0x15, 0x38, 0xDF, 0xD8, 0x23, 0xC6, 0x50,
        0x50, 0x85, 0xE2, 0x1F, 0x0D, 0xD5, 0xC8, 0x6B,
    };

    /* dh1024 g */
    static const unsigned char g[] =
    {
      0x02,
    };

    wolfSSL_CTX_SetTmpDH(ctx, p, sizeof(p), g, sizeof(g));
}
#endif /* NO_DH */

#ifndef NO_CERTS

static INLINE void CaCb(unsigned char* der, int sz, int type)
{
    (void)der;
    printf("Got CA cache add callback, derSz = %d, type = %d\n", sz, type);
}

#endif /* !NO_CERTS */


/* Wolf Root Directory Helper */
/* KEIL-RL File System does not support relative directory */
#if !defined(WOLFSSL_MDK_ARM) && !defined(WOLFSSL_KEIL_FS) && !defined(WOLFSSL_TIRTOS)
    /* Maximum depth to search for WolfSSL root */
    #define MAX_WOLF_ROOT_DEPTH 5

    static INLINE int ChangeToWolfRoot(void)
    {
        #if !defined(NO_FILESYSTEM) || defined(FORCE_BUFFER_TEST)
            int depth, res;
            FILE* file;
            for(depth = 0; depth <= MAX_WOLF_ROOT_DEPTH; depth++) {
                file = fopen(ntruKeyFile, "rb");
                if (file != NULL) {
                    fclose(file);
                    return depth;
                }
            #ifdef USE_WINDOWS_API
                res = SetCurrentDirectoryA("..\\");
            #else
                res = chdir("../");
            #endif
                if (res < 0) {
                    printf("chdir to ../ failed!\n");
                    break;
                }
            }

            err_sys("wolf root not found");
            return -1;
        #else
            return 0;
        #endif
    }
#endif /* !defined(WOLFSSL_MDK_ARM) && !defined(WOLFSSL_KEIL_FS) && !defined(WOLFSSL_TIRTOS) */

#ifdef HAVE_STACK_SIZE

typedef THREAD_RETURN WOLFSSL_THREAD (*thread_func)(void* args);
#define STACK_CHECK_VAL 0x01

static INLINE int StackSizeCheck(func_args* args, thread_func tf)
{
    int            ret, i, used;
    void*          status;
    unsigned char* myStack = NULL;
    int            stackSize = 1024*128;
    pthread_attr_t myAttr;
    pthread_t      threadId;

#ifdef PTHREAD_STACK_MIN
    if (stackSize < PTHREAD_STACK_MIN)
        stackSize = PTHREAD_STACK_MIN;
#endif

    ret = posix_memalign((void**)&myStack, sysconf(_SC_PAGESIZE), stackSize);
    if (ret != 0 || myStack == NULL)
        err_sys("posix_memalign failed\n");

    XMEMSET(myStack, STACK_CHECK_VAL, stackSize);

    ret = pthread_attr_init(&myAttr);
    if (ret != 0)
        err_sys("attr_init failed");

    ret = pthread_attr_setstack(&myAttr, myStack, stackSize);
    if (ret != 0)
        err_sys("attr_setstackaddr failed");

    ret = pthread_create(&threadId, &myAttr, tf, args);
    if (ret != 0) {
        perror("pthread_create failed");
        exit(EXIT_FAILURE);
    }

    ret = pthread_join(threadId, &status);
    if (ret != 0)
        err_sys("pthread_join failed");

    for (i = 0; i < stackSize; i++) {
        if (myStack[i] != STACK_CHECK_VAL) {
            break;
        }
    }

    free(myStack);

    used = stackSize - i;
    printf("stack used = %d\n", used);

    return (int)((size_t)status);
}


#endif /* HAVE_STACK_SIZE */


#ifdef STACK_TRAP

/* good settings
   --enable-debug --disable-shared C_EXTRA_FLAGS="-DUSER_TIME -DTFM_TIMING_RESISTANT -DPOSITIVE_EXP_ONLY -DSTACK_TRAP"

*/

#ifdef HAVE_STACK_SIZE
    /* client only for now, setrlimit will fail if pthread_create() called */
    /* STACK_SIZE does pthread_create() on client */
    #error "can't use STACK_TRAP with STACK_SIZE, setrlimit will fail"
#endif /* HAVE_STACK_SIZE */

static INLINE void StackTrap(void)
{
    struct rlimit  rl;
    if (getrlimit(RLIMIT_STACK, &rl) != 0)
        err_sys("getrlimit failed");
    printf("rlim_cur = %llu\n", rl.rlim_cur);
    rl.rlim_cur = 1024*21;  /* adjust trap size here */
    if (setrlimit(RLIMIT_STACK, &rl) != 0) {
        perror("setrlimit");
        err_sys("setrlimit failed");
    }
}

#else /* STACK_TRAP */

static INLINE void StackTrap(void)
{
}

#endif /* STACK_TRAP */


#ifdef ATOMIC_USER

/* Atomic Encrypt Context example */
typedef struct AtomicEncCtx {
    int  keySetup;           /* have we done key setup yet */
    Aes  aes;                /* for aes example */
} AtomicEncCtx;


/* Atomic Decrypt Context example */
typedef struct AtomicDecCtx {
    int  keySetup;           /* have we done key setup yet */
    Aes  aes;                /* for aes example */
} AtomicDecCtx;


static INLINE int myMacEncryptCb(WOLFSSL* ssl, unsigned char* macOut,
       const unsigned char* macIn, unsigned int macInSz, int macContent,
       int macVerify, unsigned char* encOut, const unsigned char* encIn,
       unsigned int encSz, void* ctx)
{
    int  ret;
    Hmac hmac;
    byte myInner[WOLFSSL_TLS_HMAC_INNER_SZ];
    AtomicEncCtx* encCtx = (AtomicEncCtx*)ctx;
    const char* tlsStr = "TLS";

    /* example supports (d)tls aes */
    if (wolfSSL_GetBulkCipher(ssl) != wolfssl_aes) {
        printf("myMacEncryptCb not using AES\n");
        return -1;
    }

    if (strstr(wolfSSL_get_version(ssl), tlsStr) == NULL) {
        printf("myMacEncryptCb not using (D)TLS\n");
        return -1;
    }

    /* hmac, not needed if aead mode */
    wolfSSL_SetTlsHmacInner(ssl, myInner, macInSz, macContent, macVerify);

    ret = wc_HmacSetKey(&hmac, wolfSSL_GetHmacType(ssl),
               wolfSSL_GetMacSecret(ssl, macVerify), wolfSSL_GetHmacSize(ssl));
    if (ret != 0)
        return ret;
    ret = wc_HmacUpdate(&hmac, myInner, sizeof(myInner));
    if (ret != 0)
        return ret;
    ret = wc_HmacUpdate(&hmac, macIn, macInSz);
    if (ret != 0)
        return ret;
    ret = wc_HmacFinal(&hmac, macOut);
    if (ret != 0)
        return ret;


    /* encrypt setup on first time */
    if (encCtx->keySetup == 0) {
        int   keyLen = wolfSSL_GetKeySize(ssl);
        const byte* key;
        const byte* iv;

        if (wolfSSL_GetSide(ssl) == WOLFSSL_CLIENT_END) {
            key = wolfSSL_GetClientWriteKey(ssl);
            iv  = wolfSSL_GetClientWriteIV(ssl);
        }
        else {
            key = wolfSSL_GetServerWriteKey(ssl);
            iv  = wolfSSL_GetServerWriteIV(ssl);
        }

        ret = wc_AesSetKey(&encCtx->aes, key, keyLen, iv, AES_ENCRYPTION);
        if (ret != 0) {
            printf("AesSetKey failed in myMacEncryptCb\n");
            return ret;
        }
        encCtx->keySetup = 1;
    }

    /* encrypt */
    return wc_AesCbcEncrypt(&encCtx->aes, encOut, encIn, encSz);
}


static INLINE int myDecryptVerifyCb(WOLFSSL* ssl,
       unsigned char* decOut, const unsigned char* decIn,
       unsigned int decSz, int macContent, int macVerify,
       unsigned int* padSz, void* ctx)
{
    AtomicDecCtx* decCtx = (AtomicDecCtx*)ctx;
    int ret      = 0;
    int macInSz  = 0;
    int ivExtra  = 0;
    int digestSz = wolfSSL_GetHmacSize(ssl);
    unsigned int pad     = 0;
    unsigned int padByte = 0;
    Hmac hmac;
    byte myInner[WOLFSSL_TLS_HMAC_INNER_SZ];
    byte verify[WC_MAX_DIGEST_SIZE];
    const char* tlsStr = "TLS";

    /* example supports (d)tls aes */
    if (wolfSSL_GetBulkCipher(ssl) != wolfssl_aes) {
        printf("myMacEncryptCb not using AES\n");
        return -1;
    }

    if (strstr(wolfSSL_get_version(ssl), tlsStr) == NULL) {
        printf("myMacEncryptCb not using (D)TLS\n");
        return -1;
    }

    /*decrypt */
    if (decCtx->keySetup == 0) {
        int   keyLen = wolfSSL_GetKeySize(ssl);
        const byte* key;
        const byte* iv;

        /* decrypt is from other side (peer) */
        if (wolfSSL_GetSide(ssl) == WOLFSSL_SERVER_END) {
            key = wolfSSL_GetClientWriteKey(ssl);
            iv  = wolfSSL_GetClientWriteIV(ssl);
        }
        else {
            key = wolfSSL_GetServerWriteKey(ssl);
            iv  = wolfSSL_GetServerWriteIV(ssl);
        }

        ret = wc_AesSetKey(&decCtx->aes, key, keyLen, iv, AES_DECRYPTION);
        if (ret != 0) {
            printf("AesSetKey failed in myDecryptVerifyCb\n");
            return ret;
        }
        decCtx->keySetup = 1;
    }

    /* decrypt */
    ret = wc_AesCbcDecrypt(&decCtx->aes, decOut, decIn, decSz);
    if (ret != 0)
        return ret;

    if (wolfSSL_GetCipherType(ssl) == WOLFSSL_AEAD_TYPE) {
        *padSz = wolfSSL_GetAeadMacSize(ssl);
        return 0; /* hmac, not needed if aead mode */
    }

    if (wolfSSL_GetCipherType(ssl) == WOLFSSL_BLOCK_TYPE) {
        pad     = *(decOut + decSz - 1);
        padByte = 1;
        if (wolfSSL_IsTLSv1_1(ssl))
            ivExtra = wolfSSL_GetCipherBlockSize(ssl);
    }

    *padSz  = wolfSSL_GetHmacSize(ssl) + pad + padByte;
    macInSz = decSz - ivExtra - digestSz - pad - padByte;

    wolfSSL_SetTlsHmacInner(ssl, myInner, macInSz, macContent, macVerify);

    ret = wc_HmacSetKey(&hmac, wolfSSL_GetHmacType(ssl),
               wolfSSL_GetMacSecret(ssl, macVerify), digestSz);
    if (ret != 0)
        return ret;
    ret = wc_HmacUpdate(&hmac, myInner, sizeof(myInner));
    if (ret != 0)
        return ret;
    ret = wc_HmacUpdate(&hmac, decOut + ivExtra, macInSz);
    if (ret != 0)
        return ret;
    ret = wc_HmacFinal(&hmac, verify);
    if (ret != 0)
        return ret;

    if (XMEMCMP(verify, decOut + decSz - digestSz - pad - padByte,
               digestSz) != 0) {
        printf("myDecryptVerify verify failed\n");
        return -1;
    }

    return ret;
}


static INLINE void SetupAtomicUser(WOLFSSL_CTX* ctx, WOLFSSL* ssl)
{
    AtomicEncCtx* encCtx;
    AtomicDecCtx* decCtx;

    encCtx = (AtomicEncCtx*)malloc(sizeof(AtomicEncCtx));
    if (encCtx == NULL)
        err_sys("AtomicEncCtx malloc failed");
    XMEMSET(encCtx, 0, sizeof(AtomicEncCtx));

    decCtx = (AtomicDecCtx*)malloc(sizeof(AtomicDecCtx));
    if (decCtx == NULL) {
        free(encCtx);
        err_sys("AtomicDecCtx malloc failed");
    }
    XMEMSET(decCtx, 0, sizeof(AtomicDecCtx));

    wolfSSL_CTX_SetMacEncryptCb(ctx, myMacEncryptCb);
    wolfSSL_SetMacEncryptCtx(ssl, encCtx);

    wolfSSL_CTX_SetDecryptVerifyCb(ctx, myDecryptVerifyCb);
    wolfSSL_SetDecryptVerifyCtx(ssl, decCtx);
}


static INLINE void FreeAtomicUser(WOLFSSL* ssl)
{
    AtomicEncCtx* encCtx = (AtomicEncCtx*)wolfSSL_GetMacEncryptCtx(ssl);
    AtomicDecCtx* decCtx = (AtomicDecCtx*)wolfSSL_GetDecryptVerifyCtx(ssl);

    free(decCtx);
    free(encCtx);
}

#endif /* ATOMIC_USER */

#ifdef WOLFSSL_STATIC_MEMORY
static INLINE int wolfSSL_PrintStats(WOLFSSL_MEM_STATS* stats)
{
    word16 i;

    if (stats == NULL) {
        return 0;
    }

    /* print to stderr so is on the same pipe as WOLFSSL_DEBUG */
    fprintf(stderr, "Total mallocs   = %d\n", stats->totalAlloc);
    fprintf(stderr, "Total frees     = %d\n", stats->totalFr);
    fprintf(stderr, "Current mallocs = %d\n", stats->curAlloc);
    fprintf(stderr, "Available IO    = %d\n", stats->avaIO);
    fprintf(stderr, "Max con. handshakes  = %d\n", stats->maxHa);
    fprintf(stderr, "Max con. IO          = %d\n", stats->maxIO);
    fprintf(stderr, "State of memory blocks: size   : available \n");
    for (i = 0; i < WOLFMEM_MAX_BUCKETS; i++) {
       fprintf(stderr, "                      : %d\t : %d\n", stats->blockSz[i],
                                                            stats->avaBlock[i]);
    }

    return 1;
}
#endif /* WOLFSSL_STATIC_MEMORY */

#ifdef HAVE_PK_CALLBACKS

typedef struct PkCbInfo {
    const char* ourKey;
} PkCbInfo;

#ifdef HAVE_ECC

static INLINE int myEccSign(WOLFSSL* ssl, const byte* in, word32 inSz,
        byte* out, word32* outSz, const byte* key, word32 keySz, void* ctx)
{
    int       ret;
    WC_RNG    rng;
    word32    idx = 0;
    ecc_key   myKey;
    byte*     keyBuf = (byte*)key;
    PkCbInfo* cbInfo = (PkCbInfo*)ctx;

    (void)ssl;
    (void)cbInfo;

#ifdef TEST_PK_PRIVKEY
    ret = load_key_file(cbInfo->ourKey, &keyBuf, &keySz);
    if (ret != 0)
        return ret;
#endif

    ret = wc_InitRng(&rng);
    if (ret != 0)
        return ret;

    ret = wc_ecc_init(&myKey);
    if (ret == 0) {
        ret = wc_EccPrivateKeyDecode(keyBuf, &idx, &myKey, keySz);
        if (ret == 0)
            ret = wc_ecc_sign_hash(in, inSz, out, outSz, &rng, &myKey);
        wc_ecc_free(&myKey);
    }
    wc_FreeRng(&rng);

#ifdef TEST_PK_PRIVKEY
    free(keyBuf);
#endif

    return ret;
}


static INLINE int myEccVerify(WOLFSSL* ssl, const byte* sig, word32 sigSz,
        const byte* hash, word32 hashSz, const byte* key, word32 keySz,
        int* result, void* ctx)
{
    int       ret;
    word32    idx = 0;
    ecc_key   myKey;
    PkCbInfo* cbInfo = (PkCbInfo*)ctx;

    (void)ssl;
    (void)cbInfo;

    ret = wc_ecc_init(&myKey);
    if (ret == 0) {
        ret = wc_EccPublicKeyDecode(key, &idx, &myKey, keySz);
        if (ret == 0)
            ret = wc_ecc_verify_hash(sig, sigSz, hash, hashSz, result, &myKey);
        wc_ecc_free(&myKey);
    }

    return ret;
}

static INLINE int myEccSharedSecret(WOLFSSL* ssl, ecc_key* otherKey,
        unsigned char* pubKeyDer, unsigned int* pubKeySz,
        unsigned char* out, unsigned int* outlen,
        int side, void* ctx)
{
    int       ret;
    ecc_key*  privKey = NULL;
    ecc_key*  pubKey = NULL;
    ecc_key   tmpKey;
    PkCbInfo* cbInfo = (PkCbInfo*)ctx;

    (void)ssl;
    (void)cbInfo;

    ret = wc_ecc_init(&tmpKey);
    if (ret != 0) {
        return ret;
    }

    /* for client: create and export public key */
    if (side == WOLFSSL_CLIENT_END) {
        WC_RNG rng;

        privKey = &tmpKey;
        pubKey = otherKey;

        ret = wc_InitRng(&rng);
        if (ret == 0) {
            ret = wc_ecc_make_key_ex(&rng, 0, privKey, otherKey->dp->id);
        #ifdef WOLFSSL_ASYNC_CRYPT
            if (ret == WC_PENDING_E) {
                ret = wc_AsyncWait(ret, &privKey->asyncDev, WC_ASYNC_FLAG_NONE);
            }
        #endif
            if (ret == 0)
                ret = wc_ecc_export_x963(privKey, pubKeyDer, pubKeySz);
            wc_FreeRng(&rng);
        }
    }

    /* for server: import public key */
    else if (side == WOLFSSL_SERVER_END) {
        privKey = otherKey;
        pubKey = &tmpKey;

        ret = wc_ecc_import_x963_ex(pubKeyDer, *pubKeySz, pubKey,
            otherKey->dp->id);
    }
    else {
        ret = BAD_FUNC_ARG;
    }

    /* generate shared secret and return it */
    if (ret == 0) {
        ret = wc_ecc_shared_secret(privKey, pubKey, out, outlen);

    #ifdef WOLFSSL_ASYNC_CRYPT
        if (ret == WC_PENDING_E) {
            ret = wc_AsyncWait(ret, &privKey->asyncDev, WC_ASYNC_FLAG_CALL_AGAIN);
        }
    #endif
    }

    wc_ecc_free(&tmpKey);

    return ret;
}

#ifdef HAVE_ED25519
static INLINE int myEd25519Sign(WOLFSSL* ssl, const byte* in, word32 inSz,
        byte* out, word32* outSz, const byte* key, word32 keySz, void* ctx)
{
    int         ret;
    word32      idx = 0;
    ed25519_key myKey;
    byte*       keyBuf = (byte*)key;
    PkCbInfo*   cbInfo = (PkCbInfo*)ctx;

    (void)ssl;
    (void)cbInfo;

#ifdef TEST_PK_PRIVKEY
    ret = load_key_file(cbInfo->ourKey, &keyBuf, &keySz);
    if (ret != 0)
        return ret;
#endif

    ret = wc_ed25519_init(&myKey);
    if (ret == 0) {
        ret = wc_Ed25519PrivateKeyDecode(keyBuf, &idx, &myKey, keySz);
        if (ret == 0)
            ret = wc_ed25519_sign_msg(in, inSz, out, outSz, &myKey);
        wc_ed25519_free(&myKey);
    }

#ifdef TEST_PK_PRIVKEY
    free(keyBuf);
#endif

    return ret;
}


static INLINE int myEd25519Verify(WOLFSSL* ssl, const byte* sig, word32 sigSz,
        const byte* msg, word32 msgSz, const byte* key, word32 keySz,
        int* result, void* ctx)
{
    int         ret;
    ed25519_key myKey;
    PkCbInfo* cbInfo = (PkCbInfo*)ctx;

    (void)ssl;
    (void)cbInfo;

    ret = wc_ed25519_init(&myKey);
    if (ret == 0) {
        ret = wc_ed25519_import_public(key, keySz, &myKey);
        if (ret == 0) {
            ret = wc_ed25519_verify_msg(sig, sigSz, msg, msgSz, result, &myKey);
        }
        wc_ed25519_free(&myKey);
    }

    return ret;
}
#endif /* HAVE_ED25519 */

#ifdef HAVE_CURVE25519
static INLINE int myX25519SharedSecret(WOLFSSL* ssl, curve25519_key* otherKey,
        unsigned char* pubKeyDer, unsigned int* pubKeySz,
        unsigned char* out, unsigned int* outlen,
        int side, void* ctx)
{
    int      ret;
    curve25519_key* privKey = NULL;
    curve25519_key* pubKey = NULL;
    curve25519_key  tmpKey;
    PkCbInfo* cbInfo = (PkCbInfo*)ctx;

    (void)ssl;
    (void)cbInfo;

    ret = wc_curve25519_init(&tmpKey);
    if (ret != 0) {
        return ret;
    }

    /* for client: create and export public key */
    if (side == WOLFSSL_CLIENT_END) {
        WC_RNG rng;

        privKey = &tmpKey;
        pubKey = otherKey;

        ret = wc_InitRng(&rng);
        if (ret == 0) {
            ret = wc_curve25519_make_key(&rng, CURVE25519_KEYSIZE, privKey);
            if (ret == 0) {
                ret = wc_curve25519_export_public_ex(privKey, pubKeyDer,
                    pubKeySz, EC25519_LITTLE_ENDIAN);
            }
            wc_FreeRng(&rng);
        }
    }

    /* for server: import public key */
    else if (side == WOLFSSL_SERVER_END) {
        privKey = otherKey;
        pubKey = &tmpKey;

        ret = wc_curve25519_import_public_ex(pubKeyDer, *pubKeySz, pubKey,
            EC25519_LITTLE_ENDIAN);
    }
    else {
        ret = BAD_FUNC_ARG;
    }

    /* generate shared secret and return it */
    if (ret == 0) {
        ret = wc_curve25519_shared_secret_ex(privKey, pubKey, out, outlen,
            EC25519_LITTLE_ENDIAN);
    }

    wc_curve25519_free(&tmpKey);

    return ret;
}
#endif /* HAVE_CURVE25519 */

#endif /* HAVE_ECC */

#ifndef NO_DH
static INLINE int myDhCallback(WOLFSSL* ssl, struct DhKey* key,
        const unsigned char* priv, unsigned int privSz,
        const unsigned char* pubKeyDer, unsigned int pubKeySz,
        unsigned char* out, unsigned int* outlen,
        void* ctx)
{
    PkCbInfo* cbInfo = (PkCbInfo*)ctx;

    (void)ssl;
    (void)cbInfo;

    /* return 0 on success */
    return wc_DhAgree(key, out, outlen, priv, privSz, pubKeyDer, pubKeySz);
};

#endif /* !NO_DH */

#ifndef NO_RSA

static INLINE int myRsaSign(WOLFSSL* ssl, const byte* in, word32 inSz,
        byte* out, word32* outSz, const byte* key, word32 keySz, void* ctx)
{
    WC_RNG  rng;
    int     ret;
    word32  idx = 0;
    RsaKey  myKey;
    byte*   keyBuf = (byte*)key;
    PkCbInfo* cbInfo = (PkCbInfo*)ctx;

    (void)ssl;
    (void)cbInfo;

#ifdef TEST_PK_PRIVKEY
    ret = load_key_file(cbInfo->ourKey, &keyBuf, &keySz);
    if (ret != 0)
        return ret;
#endif

    ret = wc_InitRng(&rng);
    if (ret != 0)
        return ret;

    ret = wc_InitRsaKey(&myKey, NULL);
    if (ret == 0) {
        ret = wc_RsaPrivateKeyDecode(keyBuf, &idx, &myKey, keySz);
        if (ret == 0)
            ret = wc_RsaSSL_Sign(in, inSz, out, *outSz, &myKey, &rng);
        if (ret > 0) {  /* save and convert to 0 success */
            *outSz = ret;
            ret = 0;
        }
        wc_FreeRsaKey(&myKey);
    }
    wc_FreeRng(&rng);

#ifdef TEST_PK_PRIVKEY
    free(keyBuf);
#endif

    return ret;
}


static INLINE int myRsaVerify(WOLFSSL* ssl, byte* sig, word32 sigSz,
        byte** out, const byte* key, word32 keySz, void* ctx)
{
    int     ret;
    word32  idx = 0;
    RsaKey  myKey;
    PkCbInfo* cbInfo = (PkCbInfo*)ctx;

    (void)ssl;
    (void)cbInfo;

    ret = wc_InitRsaKey(&myKey, NULL);
    if (ret == 0) {
        ret = wc_RsaPublicKeyDecode(key, &idx, &myKey, keySz);
        if (ret == 0)
            ret = wc_RsaSSL_VerifyInline(sig, sigSz, out, &myKey);
        wc_FreeRsaKey(&myKey);
    }

    return ret;
}

static INLINE int myRsaSignCheck(WOLFSSL* ssl, byte* sig, word32 sigSz,
        byte** out, const byte* key, word32 keySz, void* ctx)
{
    int     ret;
    word32  idx = 0;
    RsaKey  myKey;
    byte*   keyBuf = (byte*)key;
    PkCbInfo* cbInfo = (PkCbInfo*)ctx;

    (void)ssl;
    (void)cbInfo;

#ifdef TEST_PK_PRIVKEY
    ret = load_key_file(cbInfo->ourKey, &keyBuf, &keySz);
    if (ret != 0)
        return ret;
#endif

    ret = wc_InitRsaKey(&myKey, NULL);
    if (ret == 0) {
        ret = wc_RsaPrivateKeyDecode(keyBuf, &idx, &myKey, keySz);
        if (ret == 0)
            ret = wc_RsaSSL_VerifyInline(sig, sigSz, out, &myKey);
        wc_FreeRsaKey(&myKey);
    }
#ifdef TEST_PK_PRIVKEY
    free(keyBuf);
#endif

    return ret;
}

#ifdef WC_RSA_PSS
static INLINE int myRsaPssSign(WOLFSSL* ssl, const byte* in, word32 inSz,
        byte* out, word32* outSz, int hash, int mgf, const byte* key,
        word32 keySz, void* ctx)
{
    enum wc_HashType hashType = WC_HASH_TYPE_NONE;
    WC_RNG           rng;
    int              ret;
    word32           idx = 0;
    RsaKey           myKey;
    byte*            keyBuf = (byte*)key;
    PkCbInfo* cbInfo = (PkCbInfo*)ctx;

    (void)ssl;
    (void)cbInfo;

#ifdef TEST_PK_PRIVKEY
    ret = load_key_file(cbInfo->ourKey, &keyBuf, &keySz);
    if (ret != 0)
        return ret;
#endif

    switch (hash) {
#ifndef NO_SHA256
        case SHA256h:
            hashType = WC_HASH_TYPE_SHA256;
            break;
#endif
#ifdef WOLFSSL_SHA384
        case SHA384h:
            hashType = WC_HASH_TYPE_SHA384;
            break;
#endif
#ifdef WOLFSSL_SHA512
        case SHA512h:
            hashType = WC_HASH_TYPE_SHA512;
            break;
#endif
    }

    ret = wc_InitRng(&rng);
    if (ret != 0)
        return ret;

    ret = wc_InitRsaKey(&myKey, NULL);
    if (ret == 0) {
        ret = wc_RsaPrivateKeyDecode(keyBuf, &idx, &myKey, keySz);
        if (ret == 0) {
            ret = wc_RsaPSS_Sign(in, inSz, out, *outSz, hashType, mgf, &myKey,
                                 &rng);
        }
        if (ret > 0) {  /* save and convert to 0 success */
            *outSz = ret;
            ret = 0;
        }
        wc_FreeRsaKey(&myKey);
    }
    wc_FreeRng(&rng);

#ifdef TEST_PK_PRIVKEY
    free(keyBuf);
#endif

    return ret;
}


static INLINE int myRsaPssVerify(WOLFSSL* ssl, byte* sig, word32 sigSz,
        byte** out, int hash, int mgf, const byte* key, word32 keySz, void* ctx)
{
    int       ret;
    word32    idx = 0;
    RsaKey    myKey;
    PkCbInfo* cbInfo = (PkCbInfo*)ctx;
    enum wc_HashType hashType = WC_HASH_TYPE_NONE;

    (void)ssl;
    (void)cbInfo;

    switch (hash) {
#ifndef NO_SHA256
        case SHA256h:
            hashType = WC_HASH_TYPE_SHA256;
            break;
#endif
#ifdef WOLFSSL_SHA384
        case SHA384h:
            hashType = WC_HASH_TYPE_SHA384;
            break;
#endif
#ifdef WOLFSSL_SHA512
        case SHA512h:
            hashType = WC_HASH_TYPE_SHA512;
            break;
#endif
    }

    ret = wc_InitRsaKey(&myKey, NULL);
    if (ret == 0) {
        ret = wc_RsaPublicKeyDecode(key, &idx, &myKey, keySz);
        if (ret == 0) {
            ret = wc_RsaPSS_VerifyInline(sig, sigSz, out, hashType, mgf,
                                         &myKey);
            }
        wc_FreeRsaKey(&myKey);
    }

    return ret;
}

static INLINE int myRsaPssSignCheck(WOLFSSL* ssl, byte* sig, word32 sigSz,
        byte** out, int hash, int mgf, const byte* key, word32 keySz, void* ctx)
{
    int       ret;
    word32    idx = 0;
    RsaKey    myKey;
    byte*     keyBuf = (byte*)key;
    PkCbInfo* cbInfo = (PkCbInfo*)ctx;
    enum wc_HashType hashType = WC_HASH_TYPE_NONE;

    (void)ssl;
    (void)cbInfo;

#ifdef TEST_PK_PRIVKEY
    ret = load_key_file(cbInfo->ourKey, &keyBuf, &keySz);
    if (ret != 0)
        return ret;
#endif

    switch (hash) {
#ifndef NO_SHA256
        case SHA256h:
            hashType = WC_HASH_TYPE_SHA256;
            break;
#endif
#ifdef WOLFSSL_SHA384
        case SHA384h:
            hashType = WC_HASH_TYPE_SHA384;
            break;
#endif
#ifdef WOLFSSL_SHA512
        case SHA512h:
            hashType = WC_HASH_TYPE_SHA512;
            break;
#endif
    }

    ret = wc_InitRsaKey(&myKey, NULL);
    if (ret == 0) {
        ret = wc_RsaPrivateKeyDecode(keyBuf, &idx, &myKey, keySz);
        if (ret == 0) {
            ret = wc_RsaPSS_VerifyInline(sig, sigSz, out, hashType, mgf,
                                         &myKey);
            }
        wc_FreeRsaKey(&myKey);
    }

#ifdef TEST_PK_PRIVKEY
    free(keyBuf);
#endif

    return ret;
}
#endif


static INLINE int myRsaEnc(WOLFSSL* ssl, const byte* in, word32 inSz,
                           byte* out, word32* outSz, const byte* key,
                           word32 keySz, void* ctx)
{
    int       ret;
    word32    idx = 0;
    RsaKey    myKey;
    WC_RNG    rng;
    PkCbInfo* cbInfo = (PkCbInfo*)ctx;

    (void)ssl;
    (void)cbInfo;

    ret = wc_InitRng(&rng);
    if (ret != 0)
        return ret;

    ret = wc_InitRsaKey(&myKey, NULL);
    if (ret == 0) {
        ret = wc_RsaPublicKeyDecode(key, &idx, &myKey, keySz);
        if (ret == 0) {
            ret = wc_RsaPublicEncrypt(in, inSz, out, *outSz, &myKey, &rng);
            if (ret > 0) {
                *outSz = ret;
                ret = 0;  /* reset to success */
            }
        }
        wc_FreeRsaKey(&myKey);
    }
    wc_FreeRng(&rng);

    return ret;
}

static INLINE int myRsaDec(WOLFSSL* ssl, byte* in, word32 inSz,
                           byte** out,
                           const byte* key, word32 keySz, void* ctx)
{
    int       ret;
    word32    idx = 0;
    RsaKey    myKey;
    byte*     keyBuf = (byte*)key;
    PkCbInfo* cbInfo = (PkCbInfo*)ctx;

    (void)ssl;
    (void)cbInfo;

#ifdef TEST_PK_PRIVKEY
    ret = load_key_file(cbInfo->ourKey, &keyBuf, &keySz);
    if (ret != 0)
        return ret;
#endif

    ret = wc_InitRsaKey(&myKey, NULL);
    if (ret == 0) {
        ret = wc_RsaPrivateKeyDecode(keyBuf, &idx, &myKey, keySz);
        if (ret == 0) {
            #ifdef WC_RSA_BLINDING
                ret = wc_RsaSetRNG(&myKey, wolfSSL_GetRNG(ssl));
                if (ret != 0) {
                    wc_FreeRsaKey(&myKey);
                    return ret;
                }
            #endif
            ret = wc_RsaPrivateDecryptInline(in, inSz, out, &myKey);
        }
        wc_FreeRsaKey(&myKey);
    }

#ifdef TEST_PK_PRIVKEY
    free(keyBuf);
#endif

    return ret;
}

#endif /* NO_RSA */

static INLINE void SetupPkCallbacks(WOLFSSL_CTX* ctx)
{
    (void)ctx;

    #ifdef HAVE_ECC
        wolfSSL_CTX_SetEccSignCb(ctx, myEccSign);
        wolfSSL_CTX_SetEccVerifyCb(ctx, myEccVerify);
        wolfSSL_CTX_SetEccSharedSecretCb(ctx, myEccSharedSecret);
    #endif /* HAVE_ECC */
    #ifndef NO_DH
        wolfSSL_CTX_SetDhAgreeCb(ctx, myDhCallback);
    #endif
    #ifdef HAVE_ED25519
        wolfSSL_CTX_SetEd25519SignCb(ctx, myEd25519Sign);
        wolfSSL_CTX_SetEd25519VerifyCb(ctx, myEd25519Verify);
    #endif
    #ifdef HAVE_CURVE25519
        wolfSSL_CTX_SetX25519SharedSecretCb(ctx, myX25519SharedSecret);
    #endif
    #ifndef NO_RSA
        wolfSSL_CTX_SetRsaSignCb(ctx, myRsaSign);
        wolfSSL_CTX_SetRsaVerifyCb(ctx, myRsaVerify);
        wolfSSL_CTX_SetRsaSignCheckCb(ctx, myRsaSignCheck);
        #ifdef WC_RSA_PSS
            wolfSSL_CTX_SetRsaPssSignCb(ctx, myRsaPssSign);
            wolfSSL_CTX_SetRsaPssVerifyCb(ctx, myRsaPssVerify);
            wolfSSL_CTX_SetRsaPssSignCheckCb(ctx, myRsaPssSignCheck);
        #endif
        wolfSSL_CTX_SetRsaEncCb(ctx, myRsaEnc);
        wolfSSL_CTX_SetRsaDecCb(ctx, myRsaDec);
    #endif /* NO_RSA */
}

static INLINE void SetupPkCallbackContexts(WOLFSSL* ssl, void* myCtx)
{
    #ifdef HAVE_ECC
        wolfSSL_SetEccSignCtx(ssl, myCtx);
        wolfSSL_SetEccVerifyCtx(ssl, myCtx);
        wolfSSL_SetEccSharedSecretCtx(ssl, myCtx);
    #endif /* HAVE_ECC */
    #ifndef NO_DH
        wolfSSL_SetDhAgreeCtx(ssl, myCtx);
    #endif
    #ifdef HAVE_ED25519
        wolfSSL_SetEd25519SignCtx(ssl, myCtx);
        wolfSSL_SetEd25519VerifyCtx(ssl, myCtx);
    #endif
    #ifdef HAVE_CURVE25519
        wolfSSL_SetX25519SharedSecretCtx(ssl, myCtx);
    #endif
    #ifndef NO_RSA
        wolfSSL_SetRsaSignCtx(ssl, myCtx);
        wolfSSL_SetRsaVerifyCtx(ssl, myCtx);
        #ifdef WC_RSA_PSS
            wolfSSL_SetRsaPssSignCtx(ssl, myCtx);
            wolfSSL_SetRsaPssVerifyCtx(ssl, myCtx);
        #endif
        wolfSSL_SetRsaEncCtx(ssl, myCtx);
        wolfSSL_SetRsaDecCtx(ssl, myCtx);
    #endif /* NO_RSA */
}

#endif /* HAVE_PK_CALLBACKS */




#if defined(__hpux__) || defined(__MINGW32__) || defined (WOLFSSL_TIRTOS) \
                      || defined(_MSC_VER)

/* HP/UX doesn't have strsep, needed by test/suites.c */
static INLINE char* strsep(char **stringp, const char *delim)
{
    char* start;
    char* end;

    start = *stringp;
    if (start == NULL)
        return NULL;

    if ((end = strpbrk(start, delim))) {
        *end++ = '\0';
        *stringp = end;
    } else {
        *stringp = NULL;
    }

    return start;
}

#endif /* __hpux__ and others */

/* Create unique filename, len is length of tempfn name, assuming
   len does not include null terminating character,
   num is number of characters in tempfn name to randomize */
static INLINE const char* mymktemp(char *tempfn, int len, int num)
{
    int x, size;
    static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "abcdefghijklmnopqrstuvwxyz";
    WC_RNG rng;
    byte   out;

    if (tempfn == NULL || len < 1 || num < 1 || len <= num) {
        printf("Bad input\n");
        return NULL;
    }

    size = len - 1;

    if (wc_InitRng(&rng) != 0) {
        printf("InitRng failed\n");
        return NULL;
    }

    for (x = size; x > size - num; x--) {
        if (wc_RNG_GenerateBlock(&rng,(byte*)&out, sizeof(out)) != 0) {
            printf("RNG_GenerateBlock failed\n");
            return NULL;
        }
        tempfn[x] = alphanum[out % (sizeof(alphanum) - 1)];
    }
    tempfn[len] = '\0';

    wc_FreeRng(&rng);

    return tempfn;
}



#if defined(HAVE_SESSION_TICKET) && defined(HAVE_CHACHA) && \
                                    defined(HAVE_POLY1305)

    #include <wolfssl/wolfcrypt/chacha20_poly1305.h>

    typedef struct key_ctx {
        byte name[WOLFSSL_TICKET_NAME_SZ];        /* name for this context */
        byte key[CHACHA20_POLY1305_AEAD_KEYSIZE]; /* cipher key */
    } key_ctx;

    static key_ctx myKey_ctx;
    static WC_RNG myKey_rng;

    static INLINE int TicketInit(void)
    {
        int ret = wc_InitRng(&myKey_rng);
        if (ret != 0) return ret;

        ret = wc_RNG_GenerateBlock(&myKey_rng, myKey_ctx.key, sizeof(myKey_ctx.key));
        if (ret != 0) return ret;

        ret = wc_RNG_GenerateBlock(&myKey_rng, myKey_ctx.name,sizeof(myKey_ctx.name));
        if (ret != 0) return ret;

        return 0;
    }

    static INLINE void TicketCleanup(void)
    {
        wc_FreeRng(&myKey_rng);
    }

    static INLINE int myTicketEncCb(WOLFSSL* ssl,
                             byte key_name[WOLFSSL_TICKET_NAME_SZ],
                             byte iv[WOLFSSL_TICKET_IV_SZ],
                             byte mac[WOLFSSL_TICKET_MAC_SZ],
                             int enc, byte* ticket, int inLen, int* outLen,
                             void* userCtx)
    {
        (void)ssl;
        (void)userCtx;

        int ret;
        word16 sLen = XHTONS(inLen);
        byte aad[WOLFSSL_TICKET_NAME_SZ + WOLFSSL_TICKET_IV_SZ + 2];
        int  aadSz = WOLFSSL_TICKET_NAME_SZ + WOLFSSL_TICKET_IV_SZ + 2;
        byte* tmp = aad;

        if (enc) {
            XMEMCPY(key_name, myKey_ctx.name, WOLFSSL_TICKET_NAME_SZ);

            ret = wc_RNG_GenerateBlock(&myKey_rng, iv, WOLFSSL_TICKET_IV_SZ);
            if (ret != 0) return WOLFSSL_TICKET_RET_REJECT;

            /* build aad from key name, iv, and length */
            XMEMCPY(tmp, key_name, WOLFSSL_TICKET_NAME_SZ);
            tmp += WOLFSSL_TICKET_NAME_SZ;
            XMEMCPY(tmp, iv, WOLFSSL_TICKET_IV_SZ);
            tmp += WOLFSSL_TICKET_IV_SZ;
            XMEMCPY(tmp, &sLen, 2);

            ret = wc_ChaCha20Poly1305_Encrypt(myKey_ctx.key, iv,
                                              aad, aadSz,
                                              ticket, inLen,
                                              ticket,
                                              mac);
            if (ret != 0) return WOLFSSL_TICKET_RET_REJECT;
            *outLen = inLen;  /* no padding in this mode */
        } else {
            /* decrypt */

            /* see if we know this key */
            if (XMEMCMP(key_name, myKey_ctx.name, WOLFSSL_TICKET_NAME_SZ) != 0){
                printf("client presented unknown ticket key name ");
                return WOLFSSL_TICKET_RET_FATAL;
            }

            /* build aad from key name, iv, and length */
            XMEMCPY(tmp, key_name, WOLFSSL_TICKET_NAME_SZ);
            tmp += WOLFSSL_TICKET_NAME_SZ;
            XMEMCPY(tmp, iv, WOLFSSL_TICKET_IV_SZ);
            tmp += WOLFSSL_TICKET_IV_SZ;
            XMEMCPY(tmp, &sLen, 2);

            ret = wc_ChaCha20Poly1305_Decrypt(myKey_ctx.key, iv,
                                              aad, aadSz,
                                              ticket, inLen,
                                              mac,
                                              ticket);
            if (ret != 0) return WOLFSSL_TICKET_RET_REJECT;
            *outLen = inLen;  /* no padding in this mode */
        }

        return WOLFSSL_TICKET_RET_OK;
    }

#endif  /* HAVE_SESSION_TICKET && CHACHA20 && POLY1305 */

static INLINE word16 GetRandomPort(void)
{
    word16 port = 0;

    /* Generate random port for testing */
    WC_RNG rng;
    if (wc_InitRng(&rng) == 0) {
        wc_RNG_GenerateBlock(&rng, (byte*)&port, sizeof(port));
        port |= 0xC000; /* Make sure its in the 49152 - 65535 range */
        wc_FreeRng(&rng);
    }
    return port;
}

#endif /* wolfSSL_TEST_H */
