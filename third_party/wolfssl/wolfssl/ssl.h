/* ssl.h
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



/* wolfSSL API */

#ifndef WOLFSSL_SSL_H
#define WOLFSSL_SSL_H


/* for users not using preprocessor flags*/
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/version.h>

#ifdef HAVE_WOLF_EVENT
    #include <wolfssl/wolfcrypt/wolfevent.h>
#endif

#ifndef NO_FILESYSTEM
    #if defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)
        #if MQX_USE_IO_OLD
            #include <fio.h>
        #else
            #include <nio.h>
        #endif
    #endif
#endif

#ifdef WOLFSSL_PREFIX
    #include "prefix_ssl.h"
#endif

#ifdef LIBWOLFSSL_VERSION_STRING
    #define WOLFSSL_VERSION LIBWOLFSSL_VERSION_STRING
#endif

#ifdef _WIN32
    /* wincrypt.h clashes */
    #undef OCSP_REQUEST
    #undef OCSP_RESPONSE
#endif

#ifdef OPENSSL_EXTRA
    #include <wolfssl/openssl/bn.h>
    #include <wolfssl/openssl/hmac.h>
#endif

#ifdef __cplusplus
    extern "C" {
#endif

#ifndef WOLFSSL_WOLFSSL_TYPE_DEFINED
#define WOLFSSL_WOLFSSL_TYPE_DEFINED
typedef struct WOLFSSL          WOLFSSL;
#endif
typedef struct WOLFSSL_SESSION  WOLFSSL_SESSION;
typedef struct WOLFSSL_METHOD   WOLFSSL_METHOD;
#ifndef WOLFSSL_WOLFSSL_CTX_TYPE_DEFINED
#define WOLFSSL_WOLFSSL_CTX_TYPE_DEFINED
typedef struct WOLFSSL_CTX      WOLFSSL_CTX;
#endif

typedef struct WOLFSSL_STACK      WOLFSSL_STACK;
typedef struct WOLFSSL_X509       WOLFSSL_X509;
typedef struct WOLFSSL_X509_NAME  WOLFSSL_X509_NAME;
typedef struct WOLFSSL_X509_NAME_ENTRY  WOLFSSL_X509_NAME_ENTRY;
typedef struct WOLFSSL_X509_CHAIN WOLFSSL_X509_CHAIN;

typedef struct WOLFSSL_CERT_MANAGER WOLFSSL_CERT_MANAGER;
typedef struct WOLFSSL_SOCKADDR     WOLFSSL_SOCKADDR;
typedef struct WOLFSSL_CRL          WOLFSSL_CRL;

/* redeclare guard */
#define WOLFSSL_TYPES_DEFINED

#include <wolfssl/wolfio.h>


#ifndef WOLFSSL_RSA_TYPE_DEFINED /* guard on redeclaration */
typedef struct WOLFSSL_RSA            WOLFSSL_RSA;
#define WOLFSSL_RSA_TYPE_DEFINED
#endif

#ifndef WC_RNG_TYPE_DEFINED /* guard on redeclaration */
    typedef struct WC_RNG WC_RNG;
    #define WC_RNG_TYPE_DEFINED
#endif

#ifndef WOLFSSL_DSA_TYPE_DEFINED /* guard on redeclaration */
typedef struct WOLFSSL_DSA            WOLFSSL_DSA;
#define WOLFSSL_DSA_TYPE_DEFINED
#endif

#ifndef WOLFSSL_EC_TYPE_DEFINED /* guard on redeclaration */
typedef struct WOLFSSL_EC_KEY         WOLFSSL_EC_KEY;
typedef struct WOLFSSL_EC_POINT       WOLFSSL_EC_POINT;
typedef struct WOLFSSL_EC_GROUP       WOLFSSL_EC_GROUP;
#define WOLFSSL_EC_TYPE_DEFINED
#endif

#ifndef WOLFSSL_ECDSA_TYPE_DEFINED /* guard on redeclaration */
typedef struct WOLFSSL_ECDSA_SIG      WOLFSSL_ECDSA_SIG;
#define WOLFSSL_ECDSA_TYPE_DEFINED
#endif

typedef struct WOLFSSL_CIPHER         WOLFSSL_CIPHER;
typedef struct WOLFSSL_X509_LOOKUP    WOLFSSL_X509_LOOKUP;
typedef struct WOLFSSL_X509_LOOKUP_METHOD WOLFSSL_X509_LOOKUP_METHOD;
typedef struct WOLFSSL_X509_CRL       WOLFSSL_X509_CRL;
typedef struct WOLFSSL_X509_STORE     WOLFSSL_X509_STORE;
typedef struct WOLFSSL_BIO            WOLFSSL_BIO;
typedef struct WOLFSSL_BIO_METHOD     WOLFSSL_BIO_METHOD;
typedef struct WOLFSSL_X509_EXTENSION WOLFSSL_X509_EXTENSION;
typedef struct WOLFSSL_ASN1_TIME      WOLFSSL_ASN1_TIME;
typedef struct WOLFSSL_ASN1_INTEGER   WOLFSSL_ASN1_INTEGER;
typedef struct WOLFSSL_ASN1_OBJECT    WOLFSSL_ASN1_OBJECT;

typedef struct WOLFSSL_ASN1_STRING      WOLFSSL_ASN1_STRING;
typedef struct WOLFSSL_dynlock_value    WOLFSSL_dynlock_value;
typedef struct WOLFSSL_DH               WOLFSSL_DH;
typedef struct WOLFSSL_ASN1_BIT_STRING  WOLFSSL_ASN1_BIT_STRING;
typedef unsigned char*                  WOLFSSL_BUF_MEM;

#define WOLFSSL_ASN1_UTCTIME          WOLFSSL_ASN1_TIME
#define WOLFSSL_ASN1_GENERALIZEDTIME  WOLFSSL_ASN1_TIME

struct WOLFSSL_ASN1_INTEGER {
    /* size can be increased set at 20 for tag, length then to hold at least 16
     * byte type */
    unsigned char data[20];
    /* ASN_INTEGER | LENGTH | hex of number */
};

struct WOLFSSL_ASN1_TIME {
    /* MAX_DATA_SIZE is 32 */
    unsigned char data[32 + 2];
    /* ASN_TIME | LENGTH | date bytes */
};

#ifndef WOLFSSL_EVP_PKEY_TYPE_DEFINED /* guard on redeclaration */
typedef struct WOLFSSL_EVP_PKEY     WOLFSSL_EVP_PKEY;
#define WOLFSSL_EVP_PKEY_TYPE_DEFINED
#endif

typedef struct WOLFSSL_MD4_CTX {
    int buffer[32];      /* big enough to hold, check size in Init */
} WOLFSSL_MD4_CTX;


typedef struct WOLFSSL_COMP_METHOD {
    int type;            /* stunnel dereference */
} WOLFSSL_COMP_METHOD;

struct WOLFSSL_X509_LOOKUP_METHOD {
    int type;
};

struct WOLFSSL_X509_LOOKUP {
    WOLFSSL_X509_STORE *store;
};

struct WOLFSSL_X509_STORE {
    int                   cache;          /* stunnel dereference */
    WOLFSSL_CERT_MANAGER* cm;
    WOLFSSL_X509_LOOKUP   lookup;
#ifdef OPENSSL_EXTRA
    int                   isDynamic;
#endif
};

typedef struct WOLFSSL_ALERT {
    int code;
    int level;
} WOLFSSL_ALERT;

typedef struct WOLFSSL_ALERT_HISTORY {
    WOLFSSL_ALERT last_rx;
    WOLFSSL_ALERT last_tx;
} WOLFSSL_ALERT_HISTORY;

typedef struct WOLFSSL_X509_REVOKED {
    WOLFSSL_ASN1_INTEGER* serialNumber;          /* stunnel dereference */
} WOLFSSL_X509_REVOKED;


typedef struct WOLFSSL_X509_OBJECT {
    union {
        char* ptr;
        WOLFSSL_X509 *x509;
        WOLFSSL_X509_CRL* crl;           /* stunnel dereference */
    } data;
} WOLFSSL_X509_OBJECT;

typedef struct WOLFSSL_BUFFER_INFO {
    unsigned char* buffer;
    unsigned int length;
} WOLFSSL_BUFFER_INFO;

typedef struct WOLFSSL_X509_STORE_CTX {
    WOLFSSL_X509_STORE* store;    /* Store full of a CA cert chain */
    WOLFSSL_X509* current_cert;   /* stunnel dereference */
    WOLFSSL_STACK* chain;
    char* domain;                /* subject CN domain name */
    void* ex_data;               /* external data, for fortress build */
    void* userCtx;               /* user ctx */
    int   error;                 /* current error */
    int   error_depth;           /* cert depth for this error */
    int   discardSessionCerts;   /* so verify callback can flag for discard */
    int   totalCerts;            /* number of peer cert buffers */
    WOLFSSL_BUFFER_INFO* certs;  /* peer certs */
} WOLFSSL_X509_STORE_CTX;

typedef char* WOLFSSL_STRING;

/* Valid Alert types from page 16/17 */
enum AlertDescription {
    close_notify                    =   0,
    unexpected_message              =  10,
    bad_record_mac                  =  20,
    record_overflow                 =  22,
    decompression_failure           =  30,
    handshake_failure               =  40,
    no_certificate                  =  41,
    bad_certificate                 =  42,
    unsupported_certificate         =  43,
    certificate_revoked             =  44,
    certificate_expired             =  45,
    certificate_unknown             =  46,
    illegal_parameter               =  47,
    decode_error                    =  50,
    decrypt_error                   =  51,
    #ifdef WOLFSSL_MYSQL_COMPATIBLE
    /* catch name conflict for enum protocol with MYSQL build */
    wc_protocol_version             =  70,
    #else
    protocol_version                =  70,
    #endif
    no_renegotiation                = 100,
    unrecognized_name               = 112, /**< RFC 6066, section 3 */
    bad_certificate_status_response = 113, /**< RFC 6066, section 8 */
    no_application_protocol         = 120
};


enum AlertLevel {
    alert_warning = 1,
    alert_fatal   = 2
};


typedef WOLFSSL_METHOD* (*wolfSSL_method_func)(void* heap);
WOLFSSL_API WOLFSSL_METHOD *wolfSSLv3_server_method_ex(void* heap);
WOLFSSL_API WOLFSSL_METHOD *wolfSSLv3_client_method_ex(void* heap);
WOLFSSL_API WOLFSSL_METHOD *wolfTLSv1_server_method_ex(void* heap);
WOLFSSL_API WOLFSSL_METHOD *wolfTLSv1_client_method_ex(void* heap);
WOLFSSL_API WOLFSSL_METHOD *wolfTLSv1_1_server_method_ex(void* heap);
WOLFSSL_API WOLFSSL_METHOD *wolfTLSv1_1_client_method_ex(void* heap);
WOLFSSL_API WOLFSSL_METHOD *wolfTLSv1_2_server_method_ex(void* heap);
WOLFSSL_API WOLFSSL_METHOD *wolfTLSv1_2_client_method_ex(void* heap);
#ifdef WOLFSSL_TLS13
    WOLFSSL_API WOLFSSL_METHOD *wolfTLSv1_3_server_method_ex(void* heap);
    WOLFSSL_API WOLFSSL_METHOD *wolfTLSv1_3_client_method_ex(void* heap);
#endif
WOLFSSL_API WOLFSSL_METHOD *wolfSSLv23_server_method_ex(void* heap);
WOLFSSL_API WOLFSSL_METHOD *wolfSSLv23_client_method_ex(void* heap);

#ifdef WOLFSSL_DTLS
    WOLFSSL_API WOLFSSL_METHOD *wolfDTLSv1_client_method_ex(void* heap);
    WOLFSSL_API WOLFSSL_METHOD *wolfDTLSv1_server_method_ex(void* heap);
    WOLFSSL_API WOLFSSL_METHOD *wolfDTLSv1_2_client_method_ex(void* heap);
    WOLFSSL_API WOLFSSL_METHOD *wolfDTLSv1_2_server_method_ex(void* heap);
#endif
WOLFSSL_API WOLFSSL_METHOD *wolfSSLv23_method(void);
WOLFSSL_API WOLFSSL_METHOD *wolfSSLv3_server_method(void);
WOLFSSL_API WOLFSSL_METHOD *wolfSSLv3_client_method(void);
WOLFSSL_API WOLFSSL_METHOD *wolfTLSv1_server_method(void);
WOLFSSL_API WOLFSSL_METHOD *wolfTLSv1_client_method(void);
WOLFSSL_API WOLFSSL_METHOD *wolfTLSv1_1_server_method(void);
WOLFSSL_API WOLFSSL_METHOD *wolfTLSv1_1_client_method(void);
WOLFSSL_API WOLFSSL_METHOD *wolfTLSv1_2_server_method(void);
WOLFSSL_API WOLFSSL_METHOD *wolfTLSv1_2_client_method(void);
#ifdef WOLFSSL_TLS13
    WOLFSSL_API WOLFSSL_METHOD *wolfTLSv1_3_server_method(void);
    WOLFSSL_API WOLFSSL_METHOD *wolfTLSv1_3_client_method(void);
#endif

#ifdef WOLFSSL_DTLS
    WOLFSSL_API WOLFSSL_METHOD *wolfDTLSv1_client_method(void);
    WOLFSSL_API WOLFSSL_METHOD *wolfDTLSv1_server_method(void);
    WOLFSSL_API WOLFSSL_METHOD *wolfDTLSv1_2_client_method(void);
    WOLFSSL_API WOLFSSL_METHOD *wolfDTLSv1_2_server_method(void);
#endif

#ifdef HAVE_POLY1305
    WOLFSSL_API int wolfSSL_use_old_poly(WOLFSSL*, int);
#endif

#ifdef WOLFSSL_SESSION_EXPORT
#ifdef WOLFSSL_DTLS
typedef int (*wc_dtls_export)(WOLFSSL* ssl,
                   unsigned char* exportBuffer, unsigned int sz, void* userCtx);
WOLFSSL_API int wolfSSL_dtls_import(WOLFSSL* ssl, unsigned char* buf,
                                                               unsigned int sz);
WOLFSSL_API int wolfSSL_CTX_dtls_set_export(WOLFSSL_CTX* ctx,
                                                           wc_dtls_export func);
WOLFSSL_API int wolfSSL_dtls_set_export(WOLFSSL* ssl, wc_dtls_export func);
WOLFSSL_API int wolfSSL_dtls_export(WOLFSSL* ssl, unsigned char* buf,
                                                              unsigned int* sz);
#endif /* WOLFSSL_DTLS */
#endif /* WOLFSSL_SESSION_EXPORT */

#ifdef WOLFSSL_STATIC_MEMORY
#ifndef WOLFSSL_MEM_GUARD
#define WOLFSSL_MEM_GUARD
    typedef struct WOLFSSL_MEM_STATS      WOLFSSL_MEM_STATS;
    typedef struct WOLFSSL_MEM_CONN_STATS WOLFSSL_MEM_CONN_STATS;
#endif
WOLFSSL_API int wolfSSL_CTX_load_static_memory(WOLFSSL_CTX** ctx,
                                            wolfSSL_method_func method,
                                            unsigned char* buf, unsigned int sz,
                                            int flag, int max);
WOLFSSL_API int wolfSSL_CTX_is_static_memory(WOLFSSL_CTX* ctx,
                                                 WOLFSSL_MEM_STATS* mem_stats);
WOLFSSL_API int wolfSSL_is_static_memory(WOLFSSL* ssl,
                                            WOLFSSL_MEM_CONN_STATS* mem_stats);
#endif

#if !defined(NO_FILESYSTEM) && !defined(NO_CERTS)

WOLFSSL_API int wolfSSL_CTX_use_certificate_file(WOLFSSL_CTX*, const char*, int);
WOLFSSL_API int wolfSSL_CTX_use_PrivateKey_file(WOLFSSL_CTX*, const char*, int);
WOLFSSL_API int wolfSSL_CTX_load_verify_locations(WOLFSSL_CTX*, const char*,
                                                const char*);
#ifdef WOLFSSL_TRUST_PEER_CERT
WOLFSSL_API int wolfSSL_CTX_trust_peer_cert(WOLFSSL_CTX*, const char*, int);
#endif
WOLFSSL_API int wolfSSL_CTX_use_certificate_chain_file(WOLFSSL_CTX *,
                                                     const char *file);
WOLFSSL_API int wolfSSL_CTX_use_RSAPrivateKey_file(WOLFSSL_CTX*, const char*, int);

WOLFSSL_API long wolfSSL_get_verify_depth(WOLFSSL* ssl);
WOLFSSL_API long wolfSSL_CTX_get_verify_depth(WOLFSSL_CTX* ctx);
WOLFSSL_API int wolfSSL_use_certificate_file(WOLFSSL*, const char*, int);
WOLFSSL_API int wolfSSL_use_PrivateKey_file(WOLFSSL*, const char*, int);
WOLFSSL_API int wolfSSL_use_certificate_chain_file(WOLFSSL*, const char *file);
WOLFSSL_API int wolfSSL_use_RSAPrivateKey_file(WOLFSSL*, const char*, int);

#ifdef WOLFSSL_DER_LOAD
    WOLFSSL_API int wolfSSL_CTX_der_load_verify_locations(WOLFSSL_CTX*,
                                                    const char*, int);
#endif

#ifdef HAVE_NTRU
    WOLFSSL_API int wolfSSL_CTX_use_NTRUPrivateKey_file(WOLFSSL_CTX*, const char*);
    /* load NTRU private key blob */
#endif

#ifndef WOLFSSL_PEMCERT_TODER_DEFINED
    WOLFSSL_API int wolfSSL_PemCertToDer(const char*, unsigned char*, int);
    #define WOLFSSL_PEMCERT_TODER_DEFINED
#endif

#endif /* !NO_FILESYSTEM && !NO_CERTS */

WOLFSSL_API WOLFSSL_CTX* wolfSSL_CTX_new(WOLFSSL_METHOD*);
WOLFSSL_API WOLFSSL* wolfSSL_new(WOLFSSL_CTX*);
WOLFSSL_API int  wolfSSL_is_server(WOLFSSL*);
WOLFSSL_API WOLFSSL* wolfSSL_write_dup(WOLFSSL*);
WOLFSSL_API int  wolfSSL_set_fd (WOLFSSL*, int);
WOLFSSL_API int  wolfSSL_set_write_fd (WOLFSSL*, int);
WOLFSSL_API int  wolfSSL_set_read_fd (WOLFSSL*, int);
WOLFSSL_API char* wolfSSL_get_cipher_list(int priority);
WOLFSSL_API char* wolfSSL_get_cipher_list_ex(WOLFSSL* ssl, int priority);
WOLFSSL_API int  wolfSSL_get_ciphers(char*, int);
WOLFSSL_API const char* wolfSSL_get_cipher_name(WOLFSSL* ssl);
WOLFSSL_API const char* wolfSSL_get_shared_ciphers(WOLFSSL* ssl, char* buf,
    int len);
WOLFSSL_API const char* wolfSSL_get_curve_name(WOLFSSL* ssl);
WOLFSSL_API int  wolfSSL_get_fd(const WOLFSSL*);
WOLFSSL_API void wolfSSL_set_using_nonblock(WOLFSSL*, int);
WOLFSSL_API int  wolfSSL_get_using_nonblock(WOLFSSL*);
/* please see note at top of README if you get an error from connect */
WOLFSSL_API int  wolfSSL_connect(WOLFSSL*);
WOLFSSL_API int  wolfSSL_write(WOLFSSL*, const void*, int);
WOLFSSL_API int  wolfSSL_read(WOLFSSL*, void*, int);
WOLFSSL_API int  wolfSSL_peek(WOLFSSL*, void*, int);
WOLFSSL_API int  wolfSSL_accept(WOLFSSL*);
#ifdef WOLFSSL_TLS13
WOLFSSL_API int  wolfSSL_send_hrr_cookie(WOLFSSL* ssl,
    const unsigned char* secret, unsigned int secretSz);
WOLFSSL_API int  wolfSSL_CTX_no_ticket_TLSv13(WOLFSSL_CTX* ctx);
WOLFSSL_API int  wolfSSL_no_ticket_TLSv13(WOLFSSL* ssl);
WOLFSSL_API int  wolfSSL_CTX_no_dhe_psk(WOLFSSL_CTX* ctx);
WOLFSSL_API int  wolfSSL_no_dhe_psk(WOLFSSL* ssl);
WOLFSSL_API int  wolfSSL_update_keys(WOLFSSL* ssl);
WOLFSSL_API int  wolfSSL_CTX_allow_post_handshake_auth(WOLFSSL_CTX* ctx);
WOLFSSL_API int  wolfSSL_allow_post_handshake_auth(WOLFSSL* ssl);
WOLFSSL_API int  wolfSSL_request_certificate(WOLFSSL* ssl);

WOLFSSL_API int  wolfSSL_connect_TLSv13(WOLFSSL*);
WOLFSSL_API int  wolfSSL_accept_TLSv13(WOLFSSL*);

#ifdef WOLFSSL_EARLY_DATA
WOLFSSL_API int  wolfSSL_CTX_set_max_early_data(WOLFSSL_CTX* ctx,
                                                unsigned int sz);
WOLFSSL_API int  wolfSSL_set_max_early_data(WOLFSSL* ssl, unsigned int sz);
WOLFSSL_API int  wolfSSL_write_early_data(WOLFSSL*, const void*, int, int*);
WOLFSSL_API int  wolfSSL_read_early_data(WOLFSSL*, void*, int, int*);
#endif
#endif
WOLFSSL_API void wolfSSL_CTX_free(WOLFSSL_CTX*);
WOLFSSL_API void wolfSSL_free(WOLFSSL*);
WOLFSSL_API int  wolfSSL_shutdown(WOLFSSL*);
WOLFSSL_API int  wolfSSL_send(WOLFSSL*, const void*, int sz, int flags);
WOLFSSL_API int  wolfSSL_recv(WOLFSSL*, void*, int sz, int flags);

WOLFSSL_API void wolfSSL_CTX_set_quiet_shutdown(WOLFSSL_CTX*, int);
WOLFSSL_API void wolfSSL_set_quiet_shutdown(WOLFSSL*, int);

WOLFSSL_API int  wolfSSL_get_error(WOLFSSL*, int);
WOLFSSL_API int  wolfSSL_get_alert_history(WOLFSSL*, WOLFSSL_ALERT_HISTORY *);

WOLFSSL_API int        wolfSSL_set_session(WOLFSSL* ssl,WOLFSSL_SESSION* session);
WOLFSSL_API long       wolfSSL_SSL_SESSION_set_timeout(WOLFSSL_SESSION* session, long t);
WOLFSSL_API WOLFSSL_SESSION* wolfSSL_get_session(WOLFSSL* ssl);
WOLFSSL_API void       wolfSSL_flush_sessions(WOLFSSL_CTX *ctx, long tm);
WOLFSSL_API int        wolfSSL_SetServerID(WOLFSSL* ssl, const unsigned char*,
                                         int, int);

#ifdef SESSION_INDEX
WOLFSSL_API int wolfSSL_GetSessionIndex(WOLFSSL* ssl);
WOLFSSL_API int wolfSSL_GetSessionAtIndex(int index, WOLFSSL_SESSION* session);
#endif /* SESSION_INDEX */

#if defined(SESSION_INDEX) && defined(SESSION_CERTS)
WOLFSSL_API
    WOLFSSL_X509_CHAIN* wolfSSL_SESSION_get_peer_chain(WOLFSSL_SESSION* session);
#endif /* SESSION_INDEX && SESSION_CERTS */

typedef int (*VerifyCallback)(int, WOLFSSL_X509_STORE_CTX*);
typedef int (pem_password_cb)(char*, int, int, void*);

WOLFSSL_API void wolfSSL_CTX_set_verify(WOLFSSL_CTX*, int,
                                      VerifyCallback verify_callback);
WOLFSSL_API void wolfSSL_set_verify(WOLFSSL*, int, VerifyCallback verify_callback);
WOLFSSL_API void wolfSSL_SetCertCbCtx(WOLFSSL*, void*);

WOLFSSL_API int  wolfSSL_pending(WOLFSSL*);

WOLFSSL_API void wolfSSL_load_error_strings(void);
WOLFSSL_API int  wolfSSL_library_init(void);
WOLFSSL_API long wolfSSL_CTX_set_session_cache_mode(WOLFSSL_CTX*, long);

#ifdef HAVE_SECRET_CALLBACK
typedef int (*SessionSecretCb)(WOLFSSL* ssl,
                                        void* secret, int* secretSz, void* ctx);
WOLFSSL_API int  wolfSSL_set_session_secret_cb(WOLFSSL*, SessionSecretCb, void*);
#endif /* HAVE_SECRET_CALLBACK */

/* session cache persistence */
WOLFSSL_API int  wolfSSL_save_session_cache(const char*);
WOLFSSL_API int  wolfSSL_restore_session_cache(const char*);
WOLFSSL_API int  wolfSSL_memsave_session_cache(void*, int);
WOLFSSL_API int  wolfSSL_memrestore_session_cache(const void*, int);
WOLFSSL_API int  wolfSSL_get_session_cache_memsize(void);

/* certificate cache persistence, uses ctx since certs are per ctx */
WOLFSSL_API int  wolfSSL_CTX_save_cert_cache(WOLFSSL_CTX*, const char*);
WOLFSSL_API int  wolfSSL_CTX_restore_cert_cache(WOLFSSL_CTX*, const char*);
WOLFSSL_API int  wolfSSL_CTX_memsave_cert_cache(WOLFSSL_CTX*, void*, int, int*);
WOLFSSL_API int  wolfSSL_CTX_memrestore_cert_cache(WOLFSSL_CTX*, const void*, int);
WOLFSSL_API int  wolfSSL_CTX_get_cert_cache_memsize(WOLFSSL_CTX*);

/* only supports full name from cipher_name[] delimited by : */
WOLFSSL_API int  wolfSSL_CTX_set_cipher_list(WOLFSSL_CTX*, const char*);
WOLFSSL_API int  wolfSSL_set_cipher_list(WOLFSSL*, const char*);

/* Nonblocking DTLS helper functions */
WOLFSSL_API int  wolfSSL_dtls_get_current_timeout(WOLFSSL* ssl);
WOLFSSL_API int  wolfSSL_dtls_set_timeout_init(WOLFSSL* ssl, int);
WOLFSSL_API int  wolfSSL_dtls_set_timeout_max(WOLFSSL* ssl, int);
WOLFSSL_API int  wolfSSL_dtls_got_timeout(WOLFSSL* ssl);
WOLFSSL_API int  wolfSSL_dtls(WOLFSSL* ssl);

WOLFSSL_API int  wolfSSL_dtls_set_peer(WOLFSSL*, void*, unsigned int);
WOLFSSL_API int  wolfSSL_dtls_get_peer(WOLFSSL*, void*, unsigned int*);

WOLFSSL_API int  wolfSSL_CTX_dtls_set_sctp(WOLFSSL_CTX*);
WOLFSSL_API int  wolfSSL_dtls_set_sctp(WOLFSSL*);
WOLFSSL_API int  wolfSSL_CTX_dtls_set_mtu(WOLFSSL_CTX*, unsigned short);
WOLFSSL_API int  wolfSSL_dtls_set_mtu(WOLFSSL*, unsigned short);

WOLFSSL_API int  wolfSSL_dtls_get_drop_stats(WOLFSSL*,
                                             unsigned int*, unsigned int*);
WOLFSSL_API int  wolfSSL_CTX_mcast_set_member_id(WOLFSSL_CTX*, unsigned short);
WOLFSSL_API int  wolfSSL_set_secret(WOLFSSL*, unsigned short,
                     const unsigned char*, unsigned int,
                     const unsigned char*, const unsigned char*,
                     const unsigned char*);
WOLFSSL_API int  wolfSSL_mcast_read(WOLFSSL*, unsigned short*, void*, int);
WOLFSSL_API int  wolfSSL_mcast_peer_add(WOLFSSL*, unsigned short, int);
WOLFSSL_API int  wolfSSL_mcast_peer_known(WOLFSSL*, unsigned short);
WOLFSSL_API int  wolfSSL_mcast_get_max_peers(void);
typedef int (*CallbackMcastHighwater)(unsigned short peerId,
                                      unsigned int maxSeq,
                                      unsigned int curSeq, void* ctx);
WOLFSSL_API int  wolfSSL_CTX_mcast_set_highwater_cb(WOLFSSL_CTX*,
                                                    unsigned int,
                                                    unsigned int,
                                                    unsigned int,
                                                    CallbackMcastHighwater);
WOLFSSL_API int  wolfSSL_mcast_set_highwater_ctx(WOLFSSL*, void*);

WOLFSSL_API int   wolfSSL_ERR_GET_REASON(unsigned long err);
WOLFSSL_API char* wolfSSL_ERR_error_string(unsigned long,char*);
WOLFSSL_API void  wolfSSL_ERR_error_string_n(unsigned long e, char* buf,
                                           unsigned long sz);
WOLFSSL_API const char* wolfSSL_ERR_reason_error_string(unsigned long);

/* extras */

#define STACK_OF(x) WOLFSSL_STACK
WOLFSSL_API int wolfSSL_sk_X509_push(STACK_OF(WOLFSSL_X509_NAME)* sk,
                                                            WOLFSSL_X509* x509);
WOLFSSL_API WOLFSSL_X509* wolfSSL_sk_X509_pop(STACK_OF(WOLFSSL_X509_NAME)* sk);
WOLFSSL_API void wolfSSL_sk_X509_free(STACK_OF(WOLFSSL_X509_NAME)* sk);
WOLFSSL_API WOLFSSL_ASN1_OBJECT* wolfSSL_ASN1_OBJECT_new(void);
WOLFSSL_API void wolfSSL_ASN1_OBJECT_free(WOLFSSL_ASN1_OBJECT* obj);
WOLFSSL_API int wolfSSL_sk_ASN1_OBJECT_push(STACK_OF(WOLFSSL_ASN1_OBJEXT)* sk,
                                                      WOLFSSL_ASN1_OBJECT* obj);
WOLFSSL_API WOLFSSL_ASN1_OBJECT* wolfSSL_sk_ASN1_OBJCET_pop(
                                            STACK_OF(WOLFSSL_ASN1_OBJECT)* sk);
WOLFSSL_API void wolfSSL_sk_ASN1_OBJECT_free(STACK_OF(WOLFSSL_ASN1_OBJECT)* sk);
WOLFSSL_API int wolfSSL_ASN1_STRING_to_UTF8(unsigned char **out, WOLFSSL_ASN1_STRING *in);

WOLFSSL_API int  wolfSSL_set_ex_data(WOLFSSL*, int, void*);
WOLFSSL_API int  wolfSSL_get_shutdown(const WOLFSSL*);
WOLFSSL_API int  wolfSSL_set_rfd(WOLFSSL*, int);
WOLFSSL_API int  wolfSSL_set_wfd(WOLFSSL*, int);
WOLFSSL_API void wolfSSL_set_shutdown(WOLFSSL*, int);
WOLFSSL_API int  wolfSSL_set_session_id_context(WOLFSSL*, const unsigned char*,
                                           unsigned int);
WOLFSSL_API void wolfSSL_set_connect_state(WOLFSSL*);
WOLFSSL_API void wolfSSL_set_accept_state(WOLFSSL*);
WOLFSSL_API int  wolfSSL_session_reused(WOLFSSL*);
WOLFSSL_API void wolfSSL_SESSION_free(WOLFSSL_SESSION* session);
WOLFSSL_API int  wolfSSL_is_init_finished(WOLFSSL*);

WOLFSSL_API const char*  wolfSSL_get_version(WOLFSSL*);
WOLFSSL_API int  wolfSSL_get_current_cipher_suite(WOLFSSL* ssl);
WOLFSSL_API WOLFSSL_CIPHER*  wolfSSL_get_current_cipher(WOLFSSL*);
WOLFSSL_API char* wolfSSL_CIPHER_description(const WOLFSSL_CIPHER*, char*, int);
WOLFSSL_API const char*  wolfSSL_CIPHER_get_name(const WOLFSSL_CIPHER* cipher);
WOLFSSL_API const char*  wolfSSL_SESSION_CIPHER_get_name(WOLFSSL_SESSION* session);
WOLFSSL_API const char*  wolfSSL_get_cipher(WOLFSSL*);
WOLFSSL_API WOLFSSL_SESSION* wolfSSL_get1_session(WOLFSSL* ssl);
                           /* what's ref count */

WOLFSSL_API void wolfSSL_X509_free(WOLFSSL_X509*);
WOLFSSL_API void wolfSSL_OPENSSL_free(void*);

WOLFSSL_API int wolfSSL_OCSP_parse_url(char* url, char** host, char** port,
                                     char** path, int* ssl);

WOLFSSL_API WOLFSSL_METHOD* wolfSSLv23_client_method(void);
WOLFSSL_API WOLFSSL_METHOD* wolfSSLv2_client_method(void);
WOLFSSL_API WOLFSSL_METHOD* wolfSSLv2_server_method(void);

WOLFSSL_API void wolfSSL_MD4_Init(WOLFSSL_MD4_CTX*);
WOLFSSL_API void wolfSSL_MD4_Update(WOLFSSL_MD4_CTX*, const void*, unsigned long);
WOLFSSL_API void wolfSSL_MD4_Final(unsigned char*, WOLFSSL_MD4_CTX*);


WOLFSSL_API WOLFSSL_BIO* wolfSSL_BIO_new(WOLFSSL_BIO_METHOD*);
WOLFSSL_API int  wolfSSL_BIO_free(WOLFSSL_BIO*);
WOLFSSL_API int  wolfSSL_BIO_free_all(WOLFSSL_BIO*);
WOLFSSL_API int  wolfSSL_BIO_read(WOLFSSL_BIO*, void*, int);
WOLFSSL_API int  wolfSSL_BIO_write(WOLFSSL_BIO*, const void*, int);
WOLFSSL_API WOLFSSL_BIO* wolfSSL_BIO_push(WOLFSSL_BIO*, WOLFSSL_BIO* append);
WOLFSSL_API WOLFSSL_BIO* wolfSSL_BIO_pop(WOLFSSL_BIO*);
WOLFSSL_API int  wolfSSL_BIO_flush(WOLFSSL_BIO*);
WOLFSSL_API int  wolfSSL_BIO_pending(WOLFSSL_BIO*);

WOLFSSL_API WOLFSSL_BIO_METHOD* wolfSSL_BIO_f_buffer(void);
WOLFSSL_API long wolfSSL_BIO_set_write_buffer_size(WOLFSSL_BIO*, long size);
WOLFSSL_API WOLFSSL_BIO_METHOD* wolfSSL_BIO_f_ssl(void);
WOLFSSL_API WOLFSSL_BIO*        wolfSSL_BIO_new_socket(int sfd, int flag);
WOLFSSL_API int         wolfSSL_BIO_eof(WOLFSSL_BIO*);

WOLFSSL_API WOLFSSL_BIO_METHOD* wolfSSL_BIO_s_mem(void);
WOLFSSL_API WOLFSSL_BIO_METHOD* wolfSSL_BIO_f_base64(void);
WOLFSSL_API void wolfSSL_BIO_set_flags(WOLFSSL_BIO*, int);

WOLFSSL_API int wolfSSL_BIO_get_mem_data(WOLFSSL_BIO* bio,void* p);
WOLFSSL_API WOLFSSL_BIO* wolfSSL_BIO_new_mem_buf(void* buf, int len);


WOLFSSL_API long wolfSSL_BIO_set_ssl(WOLFSSL_BIO*, WOLFSSL*, int flag);
WOLFSSL_API long wolfSSL_BIO_set_fd(WOLFSSL_BIO* b, int fd, int flag);
WOLFSSL_API void wolfSSL_set_bio(WOLFSSL*, WOLFSSL_BIO* rd, WOLFSSL_BIO* wr);
WOLFSSL_API int  wolfSSL_add_all_algorithms(void);

#ifndef NO_FILESYSTEM
WOLFSSL_API WOLFSSL_BIO_METHOD *wolfSSL_BIO_s_file(void);
#endif

WOLFSSL_API WOLFSSL_BIO_METHOD *wolfSSL_BIO_s_bio(void);
WOLFSSL_API WOLFSSL_BIO_METHOD *wolfSSL_BIO_s_socket(void);

WOLFSSL_API long wolfSSL_BIO_ctrl(WOLFSSL_BIO *bp, int cmd, long larg, void *parg);
WOLFSSL_API long wolfSSL_BIO_int_ctrl(WOLFSSL_BIO *bp, int cmd, long larg, int iarg);

WOLFSSL_API int  wolfSSL_BIO_set_write_buf_size(WOLFSSL_BIO *b, long size);
WOLFSSL_API int  wolfSSL_BIO_make_bio_pair(WOLFSSL_BIO *b1, WOLFSSL_BIO *b2);
WOLFSSL_API int  wolfSSL_BIO_ctrl_reset_read_request(WOLFSSL_BIO *b);
WOLFSSL_API int  wolfSSL_BIO_nread0(WOLFSSL_BIO *bio, char **buf);
WOLFSSL_API int  wolfSSL_BIO_nread(WOLFSSL_BIO *bio, char **buf, int num);
WOLFSSL_API int  wolfSSL_BIO_nwrite(WOLFSSL_BIO *bio, char **buf, int num);
WOLFSSL_API int  wolfSSL_BIO_reset(WOLFSSL_BIO *bio);

WOLFSSL_API int  wolfSSL_BIO_seek(WOLFSSL_BIO *bio, int ofs);
WOLFSSL_API int  wolfSSL_BIO_write_filename(WOLFSSL_BIO *bio, char *name);
WOLFSSL_API long wolfSSL_BIO_set_mem_eof_return(WOLFSSL_BIO *bio, int v);
WOLFSSL_API long wolfSSL_BIO_get_mem_ptr(WOLFSSL_BIO *bio, WOLFSSL_BUF_MEM **m);

WOLFSSL_API void        wolfSSL_RAND_screen(void);
WOLFSSL_API const char* wolfSSL_RAND_file_name(char*, unsigned long);
WOLFSSL_API int         wolfSSL_RAND_write_file(const char*);
WOLFSSL_API int         wolfSSL_RAND_load_file(const char*, long);
WOLFSSL_API int         wolfSSL_RAND_egd(const char*);
WOLFSSL_API int         wolfSSL_RAND_seed(const void*, int);
WOLFSSL_API void        wolfSSL_RAND_add(const void*, int, double);

WOLFSSL_API WOLFSSL_COMP_METHOD* wolfSSL_COMP_zlib(void);
WOLFSSL_API WOLFSSL_COMP_METHOD* wolfSSL_COMP_rle(void);
WOLFSSL_API int wolfSSL_COMP_add_compression_method(int, void*);

WOLFSSL_API int wolfSSL_get_ex_new_index(long, void*, void*, void*, void*);

WOLFSSL_API void wolfSSL_set_id_callback(unsigned long (*f)(void));
WOLFSSL_API void wolfSSL_set_locking_callback(void (*f)(int, int, const char*,
                                                      int));
WOLFSSL_API void wolfSSL_set_dynlock_create_callback(WOLFSSL_dynlock_value* (*f)
                                                   (const char*, int));
WOLFSSL_API void wolfSSL_set_dynlock_lock_callback(void (*f)(int,
                                      WOLFSSL_dynlock_value*, const char*, int));
WOLFSSL_API void wolfSSL_set_dynlock_destroy_callback(void (*f)
                                     (WOLFSSL_dynlock_value*, const char*, int));
WOLFSSL_API int  wolfSSL_num_locks(void);

WOLFSSL_API WOLFSSL_X509* wolfSSL_X509_STORE_CTX_get_current_cert(
                                                        WOLFSSL_X509_STORE_CTX*);
WOLFSSL_API int   wolfSSL_X509_STORE_CTX_get_error(WOLFSSL_X509_STORE_CTX*);
WOLFSSL_API int   wolfSSL_X509_STORE_CTX_get_error_depth(WOLFSSL_X509_STORE_CTX*);

WOLFSSL_API char*       wolfSSL_X509_NAME_oneline(WOLFSSL_X509_NAME*, char*, int);
WOLFSSL_API WOLFSSL_X509_NAME*  wolfSSL_X509_get_issuer_name(WOLFSSL_X509*);
WOLFSSL_API WOLFSSL_X509_NAME*  wolfSSL_X509_get_subject_name(WOLFSSL_X509*);
WOLFSSL_API int  wolfSSL_X509_ext_isSet_by_NID(WOLFSSL_X509*, int);
WOLFSSL_API int  wolfSSL_X509_ext_get_critical_by_NID(WOLFSSL_X509*, int);
WOLFSSL_API int  wolfSSL_X509_get_isCA(WOLFSSL_X509*);
WOLFSSL_API int  wolfSSL_X509_get_isSet_pathLength(WOLFSSL_X509*);
WOLFSSL_API unsigned int wolfSSL_X509_get_pathLength(WOLFSSL_X509*);
WOLFSSL_API unsigned int wolfSSL_X509_get_keyUsage(WOLFSSL_X509*);
WOLFSSL_API unsigned char* wolfSSL_X509_get_authorityKeyID(
                                            WOLFSSL_X509*, unsigned char*, int*);
WOLFSSL_API unsigned char* wolfSSL_X509_get_subjectKeyID(
                                            WOLFSSL_X509*, unsigned char*, int*);
WOLFSSL_API int wolfSSL_X509_NAME_entry_count(WOLFSSL_X509_NAME*);
WOLFSSL_API int wolfSSL_X509_NAME_get_text_by_NID(
                                            WOLFSSL_X509_NAME*, int, char*, int);
WOLFSSL_API int wolfSSL_X509_NAME_get_index_by_NID(
                                           WOLFSSL_X509_NAME*, int, int);
WOLFSSL_API WOLFSSL_ASN1_STRING* wolfSSL_X509_NAME_ENTRY_get_data(WOLFSSL_X509_NAME_ENTRY*);
WOLFSSL_API char* wolfSSL_ASN1_STRING_data(WOLFSSL_ASN1_STRING*);
WOLFSSL_API int wolfSSL_ASN1_STRING_length(WOLFSSL_ASN1_STRING*);
WOLFSSL_API int         wolfSSL_X509_verify_cert(WOLFSSL_X509_STORE_CTX*);
WOLFSSL_API const char* wolfSSL_X509_verify_cert_error_string(long);
WOLFSSL_API int wolfSSL_X509_get_signature_type(WOLFSSL_X509*);
WOLFSSL_API int wolfSSL_X509_get_signature(WOLFSSL_X509*, unsigned char*, int*);

WOLFSSL_API int wolfSSL_X509_LOOKUP_add_dir(WOLFSSL_X509_LOOKUP*,const char*,long);
WOLFSSL_API int wolfSSL_X509_LOOKUP_load_file(WOLFSSL_X509_LOOKUP*, const char*,
                                            long);
WOLFSSL_API WOLFSSL_X509_LOOKUP_METHOD* wolfSSL_X509_LOOKUP_hash_dir(void);
WOLFSSL_API WOLFSSL_X509_LOOKUP_METHOD* wolfSSL_X509_LOOKUP_file(void);

WOLFSSL_API WOLFSSL_X509_LOOKUP* wolfSSL_X509_STORE_add_lookup(WOLFSSL_X509_STORE*,
                                                    WOLFSSL_X509_LOOKUP_METHOD*);
WOLFSSL_API WOLFSSL_X509_STORE*  wolfSSL_X509_STORE_new(void);
WOLFSSL_API void         wolfSSL_X509_STORE_free(WOLFSSL_X509_STORE*);
WOLFSSL_API int          wolfSSL_X509_STORE_add_cert(
                                              WOLFSSL_X509_STORE*, WOLFSSL_X509*);
WOLFSSL_API WOLFSSL_STACK* wolfSSL_X509_STORE_CTX_get_chain(
                                                   WOLFSSL_X509_STORE_CTX* ctx);
WOLFSSL_API int wolfSSL_X509_STORE_set_flags(WOLFSSL_X509_STORE* store,
                                                            unsigned long flag);
WOLFSSL_API int          wolfSSL_X509_STORE_set_default_paths(WOLFSSL_X509_STORE*);
WOLFSSL_API int          wolfSSL_X509_STORE_get_by_subject(WOLFSSL_X509_STORE_CTX*,
                                   int, WOLFSSL_X509_NAME*, WOLFSSL_X509_OBJECT*);
WOLFSSL_API WOLFSSL_X509_STORE_CTX* wolfSSL_X509_STORE_CTX_new(void);
WOLFSSL_API int  wolfSSL_X509_STORE_CTX_init(WOLFSSL_X509_STORE_CTX*,
                      WOLFSSL_X509_STORE*, WOLFSSL_X509*, STACK_OF(WOLFSSL_X509)*);
WOLFSSL_API void wolfSSL_X509_STORE_CTX_free(WOLFSSL_X509_STORE_CTX*);
WOLFSSL_API void wolfSSL_X509_STORE_CTX_cleanup(WOLFSSL_X509_STORE_CTX*);

WOLFSSL_API WOLFSSL_ASN1_TIME* wolfSSL_X509_CRL_get_lastUpdate(WOLFSSL_X509_CRL*);
WOLFSSL_API WOLFSSL_ASN1_TIME* wolfSSL_X509_CRL_get_nextUpdate(WOLFSSL_X509_CRL*);

WOLFSSL_API WOLFSSL_EVP_PKEY* wolfSSL_X509_get_pubkey(WOLFSSL_X509*);
WOLFSSL_API int       wolfSSL_X509_CRL_verify(WOLFSSL_X509_CRL*, WOLFSSL_EVP_PKEY*);
WOLFSSL_API void      wolfSSL_X509_STORE_CTX_set_error(WOLFSSL_X509_STORE_CTX*,
                                                     int);
WOLFSSL_API void      wolfSSL_X509_OBJECT_free_contents(WOLFSSL_X509_OBJECT*);
WOLFSSL_API WOLFSSL_EVP_PKEY* wolfSSL_d2i_PrivateKey(int type,
        WOLFSSL_EVP_PKEY** out, const unsigned char **in, long inSz);
WOLFSSL_API WOLFSSL_EVP_PKEY* wolfSSL_PKEY_new(void);
WOLFSSL_API void      wolfSSL_EVP_PKEY_free(WOLFSSL_EVP_PKEY*);
WOLFSSL_API int       wolfSSL_X509_cmp_current_time(const WOLFSSL_ASN1_TIME*);
WOLFSSL_API int       wolfSSL_sk_X509_REVOKED_num(WOLFSSL_X509_REVOKED*);

WOLFSSL_API WOLFSSL_X509_REVOKED* wolfSSL_X509_CRL_get_REVOKED(WOLFSSL_X509_CRL*);
WOLFSSL_API WOLFSSL_X509_REVOKED* wolfSSL_sk_X509_REVOKED_value(
                                                      WOLFSSL_X509_REVOKED*,int);
WOLFSSL_API WOLFSSL_ASN1_INTEGER* wolfSSL_X509_get_serialNumber(WOLFSSL_X509*);

WOLFSSL_API int wolfSSL_ASN1_TIME_print(WOLFSSL_BIO*, const WOLFSSL_ASN1_TIME*);

WOLFSSL_API int  wolfSSL_ASN1_INTEGER_cmp(const WOLFSSL_ASN1_INTEGER*,
                                       const WOLFSSL_ASN1_INTEGER*);
WOLFSSL_API long wolfSSL_ASN1_INTEGER_get(const WOLFSSL_ASN1_INTEGER*);

#ifdef OPENSSL_EXTRA
WOLFSSL_API WOLFSSL_BIGNUM *wolfSSL_ASN1_INTEGER_to_BN(const WOLFSSL_ASN1_INTEGER *ai,
                                       WOLFSSL_BIGNUM *bn);
WOLFSSL_API STACK_OF(WOLFSSL_X509_NAME)* wolfSSL_load_client_CA_file(const char*);
#endif

WOLFSSL_API STACK_OF(WOLFSSL_X509_NAME)* wolfSSL_SSL_CTX_get_client_CA_list(
        const WOLFSSL_CTX *s);
WOLFSSL_API void  wolfSSL_CTX_set_client_CA_list(WOLFSSL_CTX*,
                                               STACK_OF(WOLFSSL_X509_NAME)*);
WOLFSSL_API void* wolfSSL_X509_STORE_CTX_get_ex_data(WOLFSSL_X509_STORE_CTX*, int);
WOLFSSL_API int   wolfSSL_get_ex_data_X509_STORE_CTX_idx(void);
WOLFSSL_API void* wolfSSL_get_ex_data(const WOLFSSL*, int);

WOLFSSL_API void wolfSSL_CTX_set_default_passwd_cb_userdata(WOLFSSL_CTX*,
                                                          void* userdata);
WOLFSSL_API void wolfSSL_CTX_set_default_passwd_cb(WOLFSSL_CTX*,
                                                   pem_password_cb*);


WOLFSSL_API void wolfSSL_CTX_set_info_callback(WOLFSSL_CTX*,
                          void (*)(const WOLFSSL* ssl, int type, int val));

WOLFSSL_API unsigned long wolfSSL_ERR_peek_error(void);
WOLFSSL_API int           wolfSSL_GET_REASON(int);

WOLFSSL_API char* wolfSSL_alert_type_string_long(int);
WOLFSSL_API char* wolfSSL_alert_desc_string_long(int);
WOLFSSL_API char* wolfSSL_state_string_long(const WOLFSSL*);

WOLFSSL_API WOLFSSL_RSA* wolfSSL_RSA_generate_key(int, unsigned long,
                                               void(*)(int, int, void*), void*);
WOLFSSL_API void wolfSSL_CTX_set_tmp_rsa_callback(WOLFSSL_CTX*,
                                             WOLFSSL_RSA*(*)(WOLFSSL*, int, int));

WOLFSSL_API int wolfSSL_PEM_def_callback(char*, int num, int w, void* key);

WOLFSSL_API long wolfSSL_CTX_sess_accept(WOLFSSL_CTX*);
WOLFSSL_API long wolfSSL_CTX_sess_connect(WOLFSSL_CTX*);
WOLFSSL_API long wolfSSL_CTX_sess_accept_good(WOLFSSL_CTX*);
WOLFSSL_API long wolfSSL_CTX_sess_connect_good(WOLFSSL_CTX*);
WOLFSSL_API long wolfSSL_CTX_sess_accept_renegotiate(WOLFSSL_CTX*);
WOLFSSL_API long wolfSSL_CTX_sess_connect_renegotiate(WOLFSSL_CTX*);
WOLFSSL_API long wolfSSL_CTX_sess_hits(WOLFSSL_CTX*);
WOLFSSL_API long wolfSSL_CTX_sess_cb_hits(WOLFSSL_CTX*);
WOLFSSL_API long wolfSSL_CTX_sess_cache_full(WOLFSSL_CTX*);
WOLFSSL_API long wolfSSL_CTX_sess_misses(WOLFSSL_CTX*);
WOLFSSL_API long wolfSSL_CTX_sess_timeouts(WOLFSSL_CTX*);
WOLFSSL_API long wolfSSL_CTX_sess_number(WOLFSSL_CTX*);

WOLFSSL_API long wolfSSL_CTX_add_extra_chain_cert(WOLFSSL_CTX*, WOLFSSL_X509*);
WOLFSSL_API long wolfSSL_CTX_sess_set_cache_size(WOLFSSL_CTX*, long);
WOLFSSL_API long wolfSSL_CTX_sess_get_cache_size(WOLFSSL_CTX*);

WOLFSSL_API long wolfSSL_CTX_get_session_cache_mode(WOLFSSL_CTX*);
WOLFSSL_API int  wolfSSL_CTX_get_read_ahead(WOLFSSL_CTX*);
WOLFSSL_API int  wolfSSL_CTX_set_read_ahead(WOLFSSL_CTX*, int v);
WOLFSSL_API long wolfSSL_CTX_set_tlsext_status_arg(WOLFSSL_CTX*, void* arg);
WOLFSSL_API long wolfSSL_CTX_set_tlsext_opaque_prf_input_callback_arg(
        WOLFSSL_CTX*, void* arg);

WOLFSSL_API unsigned long wolfSSL_set_options(WOLFSSL *s, unsigned long op);
WOLFSSL_API unsigned long wolfSSL_get_options(const WOLFSSL *s);
WOLFSSL_API long wolfSSL_clear_num_renegotiations(WOLFSSL *s);
WOLFSSL_API long wolfSSL_total_renegotiations(WOLFSSL *s);
WOLFSSL_API long wolfSSL_set_tmp_dh(WOLFSSL *s, WOLFSSL_DH *dh);
WOLFSSL_API long wolfSSL_set_tlsext_debug_arg(WOLFSSL *s, void *arg);
WOLFSSL_API long wolfSSL_set_tlsext_status_type(WOLFSSL *s, int type);
WOLFSSL_API long wolfSSL_set_tlsext_status_exts(WOLFSSL *s, void *arg);
WOLFSSL_API long wolfSSL_get_tlsext_status_ids(WOLFSSL *s, void *arg);
WOLFSSL_API long wolfSSL_set_tlsext_status_ids(WOLFSSL *s, void *arg);
WOLFSSL_API long wolfSSL_get_tlsext_status_ocsp_resp(WOLFSSL *s, unsigned char **resp);
WOLFSSL_API long wolfSSL_set_tlsext_status_ocsp_resp(WOLFSSL *s, unsigned char *resp, int len);

WOLFSSL_API void wolfSSL_CONF_modules_unload(int all);
WOLFSSL_API long wolfSSL_get_tlsext_status_exts(WOLFSSL *s, void *arg);
WOLFSSL_API long wolfSSL_get_verify_result(const WOLFSSL *ssl);

#define WOLFSSL_DEFAULT_CIPHER_LIST ""   /* default all */
#define WOLFSSL_RSA_F4 0x10001L

/* seperated out from other enums because of size */
enum {
    SSL_OP_MICROSOFT_SESS_ID_BUG                  = 0x00000001,
    SSL_OP_NETSCAPE_CHALLENGE_BUG                 = 0x00000002,
    SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG       = 0x00000004,
    SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG            = 0x00000008,
    SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER             = 0x00000010,
    SSL_OP_MSIE_SSLV2_RSA_PADDING                 = 0x00000020,
    SSL_OP_SSLEAY_080_CLIENT_DH_BUG               = 0x00000040,
    SSL_OP_TLS_D5_BUG                             = 0x00000080,
    SSL_OP_TLS_BLOCK_PADDING_BUG                  = 0x00000100,
    SSL_OP_TLS_ROLLBACK_BUG                       = 0x00000200,
    SSL_OP_ALL                                    = 0x00000400,
    SSL_OP_EPHEMERAL_RSA                          = 0x00000800,
    SSL_OP_NO_SSLv3                               = 0x00001000,
    SSL_OP_NO_TLSv1                               = 0x00002000,
    SSL_OP_PKCS1_CHECK_1                          = 0x00004000,
    SSL_OP_PKCS1_CHECK_2                          = 0x00008000,
    SSL_OP_NETSCAPE_CA_DN_BUG                     = 0x00010000,
    SSL_OP_NETSCAPE_DEMO_CIPHER_CHANGE_BUG        = 0x00020000,
    SSL_OP_SINGLE_DH_USE                          = 0x00040000,
    SSL_OP_NO_TICKET                              = 0x00080000,
    SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS            = 0x00100000,
    SSL_OP_NO_QUERY_MTU                           = 0x00200000,
    SSL_OP_COOKIE_EXCHANGE                        = 0x00400000,
    SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION = 0x00800000,
    SSL_OP_SINGLE_ECDH_USE                        = 0x01000000,
    SSL_OP_CIPHER_SERVER_PREFERENCE               = 0x02000000,
    SSL_OP_NO_TLSv1_1                             = 0x04000000,
    SSL_OP_NO_TLSv1_2                             = 0x08000000,
    SSL_OP_NO_COMPRESSION                         = 0x10000000,
    SSL_OP_NO_TLSv1_3                             = 0x20000000,
};


enum {
    OCSP_NOCERTS     = 1,
    OCSP_NOINTERN    = 2,
    OCSP_NOSIGS      = 4,
    OCSP_NOCHAIN     = 8,
    OCSP_NOVERIFY    = 16,
    OCSP_NOEXPLICIT  = 32,
    OCSP_NOCASIGN    = 64,
    OCSP_NODELEGATED = 128,
    OCSP_NOCHECKS    = 256,
    OCSP_TRUSTOTHER  = 512,
    OCSP_RESPID_KEY  = 1024,
    OCSP_NOTIME      = 2048,

    OCSP_CERTID   = 2,
    OCSP_REQUEST  = 4,
    OCSP_RESPONSE = 8,
    OCSP_BASICRESP = 16,

    WOLFSSL_OCSP_URL_OVERRIDE = 1,
    WOLFSSL_OCSP_NO_NONCE     = 2,
    WOLFSSL_OCSP_CHECKALL     = 4,

    WOLFSSL_CRL_CHECKALL = 1,
    WOLFSSL_CRL_CHECK    = 27,

    ASN1_GENERALIZEDTIME = 4,
    SSL_MAX_SSL_SESSION_ID_LENGTH = 32,

    EVP_R_BAD_DECRYPT = 2,

    SSL_ST_CONNECT = 0x1000,
    SSL_ST_ACCEPT  = 0x2000,

    SSL_CB_LOOP = 0x01,
    SSL_CB_EXIT = 0x02,
    SSL_CB_READ = 0x04,
    SSL_CB_WRITE = 0x08,
    SSL_CB_HANDSHAKE_START = 0x10,
    SSL_CB_HANDSHAKE_DONE = 0x20,
    SSL_CB_ALERT = 0x4000,
    SSL_CB_READ_ALERT = (SSL_CB_ALERT | SSL_CB_READ),
    SSL_CB_WRITE_ALERT = (SSL_CB_ALERT | SSL_CB_WRITE),
    SSL_CB_ACCEPT_LOOP = (SSL_ST_ACCEPT | SSL_CB_LOOP),
    SSL_CB_ACCEPT_EXIT = (SSL_ST_ACCEPT | SSL_CB_EXIT),
    SSL_CB_CONNECT_LOOP = (SSL_ST_CONNECT | SSL_CB_LOOP),
    SSL_CB_CONNECT_EXIT = (SSL_ST_CONNECT | SSL_CB_EXIT),

    SSL_MODE_ENABLE_PARTIAL_WRITE = 2,

    BIO_FLAGS_BASE64_NO_NL = 1,
    BIO_CLOSE   = 1,
    BIO_NOCLOSE = 0,

    NID_undef = 0,

    X509_FILETYPE_PEM = 8,
    X509_LU_X509      = 9,
    X509_LU_CRL       = 12,

    X509_V_OK                                    = 0,
    X509_V_ERR_CRL_SIGNATURE_FAILURE             = 13,
    X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD    = 14,
    X509_V_ERR_CRL_HAS_EXPIRED                   = 15,
    X509_V_ERR_CERT_REVOKED                      = 16,
    X509_V_ERR_CERT_CHAIN_TOO_LONG               = 17,
    X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT         = 18,
    X509_V_ERR_CERT_NOT_YET_VALID                = 19,
    X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD    = 20,
    X509_V_ERR_CERT_HAS_EXPIRED                  = 21,
    X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD     = 22,
    X509_V_ERR_CERT_REJECTED                     = 23,
    /* Required for Nginx  */
    X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT       = 24,
    X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN         = 25,
    X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY = 26,
    X509_V_ERR_CERT_UNTRUSTED                    = 27,
    X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE   = 28,
    X509_V_ERR_SUBJECT_ISSUER_MISMATCH           = 29,
    /* additional X509_V_ERR_* enums not used in wolfSSL */
    X509_V_ERR_UNABLE_TO_GET_CRL,
    X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE,
    X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE,
    X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY,
    X509_V_ERR_CERT_SIGNATURE_FAILURE,
    X509_V_ERR_CRL_NOT_YET_VALID,
    X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD,
    X509_V_ERR_OUT_OF_MEM,
    X509_V_ERR_INVALID_CA,
    X509_V_ERR_PATH_LENGTH_EXCEEDED,
    X509_V_ERR_INVALID_PURPOSE,
    X509_V_ERR_AKID_SKID_MISMATCH,
    X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH,
    X509_V_ERR_KEYUSAGE_NO_CERTSIGN,
    X509_V_ERR_UNABLE_TO_GET_CRL_ISSUER,
    X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION,
    X509_V_ERR_KEYUSAGE_NO_CRL_SIGN,
    X509_V_ERR_UNHANDLED_CRITICAL_CRL_EXTENSION,
    X509_V_ERR_INVALID_NON_CA,
    X509_V_ERR_PROXY_PATH_LENGTH_EXCEEDED,
    X509_V_ERR_KEYUSAGE_NO_DIGITAL_SIGNATURE,
    X509_V_ERR_PROXY_CERTIFICATES_NOT_ALLOWED,
    X509_V_ERR_INVALID_EXTENSION,
    X509_V_ERR_INVALID_POLICY_EXTENSION,
    X509_V_ERR_NO_EXPLICIT_POLICY,
    X509_V_ERR_UNNESTED_RESOURCE,

    XN_FLAG_SPC_EQ  = (1 << 23),
    XN_FLAG_ONELINE = 0,
    XN_FLAG_RFC2253 = 1,

    CRYPTO_LOCK = 1,
    CRYPTO_NUM_LOCKS = 10,

    ASN1_STRFLGS_ESC_MSB = 4
};

/* extras end */

#if !defined(NO_FILESYSTEM) && !defined(NO_STDIO_FILESYSTEM)
/* wolfSSL extension, provide last error from SSL_get_error
   since not using thread storage error queue */
#include <stdio.h>
WOLFSSL_API void  wolfSSL_ERR_print_errors_fp(FILE*, int err);
#if defined(OPENSSL_EXTRA) || defined(DEBUG_WOLFSSL_VERBOSE)
WOLFSSL_API void wolfSSL_ERR_dump_errors_fp(FILE* fp);
#endif
#endif

enum { /* ssl Constants */
    SSL_ERROR_NONE      =  0,   /* for most functions */
    SSL_FAILURE         =  0,   /* for some functions */
    SSL_SUCCESS         =  1,
    SSL_SHUTDOWN_NOT_DONE =  2,  /* call wolfSSL_shutdown again to complete */

    SSL_ALPN_NOT_FOUND  = -9,
    SSL_BAD_CERTTYPE    = -8,
    SSL_BAD_STAT        = -7,
    SSL_BAD_PATH        = -6,
    SSL_BAD_FILETYPE    = -5,
    SSL_BAD_FILE        = -4,
    SSL_NOT_IMPLEMENTED = -3,
    SSL_UNKNOWN         = -2,
    SSL_FATAL_ERROR     = -1,

    SSL_FILETYPE_ASN1    = 2,
    SSL_FILETYPE_PEM     = 1,
    SSL_FILETYPE_DEFAULT = 2, /* ASN1 */
    SSL_FILETYPE_RAW     = 3, /* NTRU raw key blob */

    SSL_VERIFY_NONE                 = 0,
    SSL_VERIFY_PEER                 = 1,
    SSL_VERIFY_FAIL_IF_NO_PEER_CERT = 2,
    SSL_VERIFY_CLIENT_ONCE          = 4,
    SSL_VERIFY_FAIL_EXCEPT_PSK      = 8,

    SSL_SESS_CACHE_OFF                = 0x0000,
    SSL_SESS_CACHE_CLIENT             = 0x0001,
    SSL_SESS_CACHE_SERVER             = 0x0002,
    SSL_SESS_CACHE_BOTH               = 0x0003,
    SSL_SESS_CACHE_NO_AUTO_CLEAR      = 0x0008,
    SSL_SESS_CACHE_NO_INTERNAL_LOOKUP = 0x0100,
    SSL_SESS_CACHE_NO_INTERNAL_STORE  = 0x0200,
    SSL_SESS_CACHE_NO_INTERNAL        = 0x0300,

    SSL_ERROR_WANT_READ        =  2,
    SSL_ERROR_WANT_WRITE       =  3,
    SSL_ERROR_WANT_CONNECT     =  7,
    SSL_ERROR_WANT_ACCEPT      =  8,
    SSL_ERROR_SYSCALL          =  5,
    SSL_ERROR_WANT_X509_LOOKUP = 83,
    SSL_ERROR_ZERO_RETURN      =  6,
    SSL_ERROR_SSL              = 85,

    SSL_SENT_SHUTDOWN     = 1,
    SSL_RECEIVED_SHUTDOWN = 2,
    SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER = 4,
    SSL_OP_NO_SSLv2       = 8,

    SSL_R_SSL_HANDSHAKE_FAILURE           = 101,
    SSL_R_TLSV1_ALERT_UNKNOWN_CA          = 102,
    SSL_R_SSLV3_ALERT_CERTIFICATE_UNKNOWN = 103,
    SSL_R_SSLV3_ALERT_BAD_CERTIFICATE     = 104,

    PEM_BUFSIZE = 1024
};


#ifndef NO_PSK
    typedef unsigned int (*wc_psk_client_callback)(WOLFSSL*, const char*, char*,
                                    unsigned int, unsigned char*, unsigned int);
    WOLFSSL_API void wolfSSL_CTX_set_psk_client_callback(WOLFSSL_CTX*,
                                                    wc_psk_client_callback);
    WOLFSSL_API void wolfSSL_set_psk_client_callback(WOLFSSL*,
                                                    wc_psk_client_callback);

    WOLFSSL_API const char* wolfSSL_get_psk_identity_hint(const WOLFSSL*);
    WOLFSSL_API const char* wolfSSL_get_psk_identity(const WOLFSSL*);

    WOLFSSL_API int wolfSSL_CTX_use_psk_identity_hint(WOLFSSL_CTX*, const char*);
    WOLFSSL_API int wolfSSL_use_psk_identity_hint(WOLFSSL*, const char*);

    typedef unsigned int (*wc_psk_server_callback)(WOLFSSL*, const char*,
                          unsigned char*, unsigned int);
    WOLFSSL_API void wolfSSL_CTX_set_psk_server_callback(WOLFSSL_CTX*,
                                                    wc_psk_server_callback);
    WOLFSSL_API void wolfSSL_set_psk_server_callback(WOLFSSL*,
                                                    wc_psk_server_callback);

    #define PSK_TYPES_DEFINED
#endif /* NO_PSK */


#ifdef HAVE_ANON
    WOLFSSL_API int wolfSSL_CTX_allow_anon_cipher(WOLFSSL_CTX*);
#endif /* HAVE_ANON */


/* extra begins */

enum {  /* ERR Constants */
    ERR_TXT_STRING = 1
};

/* bio misc */
enum {
    WOLFSSL_BIO_ERROR = -1,
    WOLFSSL_BIO_UNSET = -2,
    WOLFSSL_BIO_SIZE  = 17000 /* default BIO write size if not set */
};


WOLFSSL_API unsigned long wolfSSL_ERR_get_error_line_data(const char**, int*,
                                                 const char**, int *);

WOLFSSL_API unsigned long wolfSSL_ERR_get_error(void);
WOLFSSL_API void          wolfSSL_ERR_clear_error(void);


WOLFSSL_API int  wolfSSL_RAND_status(void);
WOLFSSL_API int  wolfSSL_RAND_bytes(unsigned char* buf, int num);
WOLFSSL_API WOLFSSL_METHOD *wolfSSLv23_server_method(void);
WOLFSSL_API long wolfSSL_CTX_set_options(WOLFSSL_CTX*, long);
#ifndef NO_CERTS
  WOLFSSL_API int  wolfSSL_CTX_check_private_key(WOLFSSL_CTX*);
#endif /* !NO_CERTS */

WOLFSSL_API void wolfSSL_ERR_free_strings(void);
WOLFSSL_API void wolfSSL_ERR_remove_state(unsigned long);
WOLFSSL_API void wolfSSL_EVP_cleanup(void);
WOLFSSL_API int  wolfSSL_clear(WOLFSSL* ssl);
WOLFSSL_API int  wolfSSL_state(WOLFSSL* ssl);

WOLFSSL_API void wolfSSL_cleanup_all_ex_data(void);
WOLFSSL_API long wolfSSL_CTX_set_mode(WOLFSSL_CTX* ctx, long mode);
WOLFSSL_API long wolfSSL_CTX_get_mode(WOLFSSL_CTX* ctx);
WOLFSSL_API void wolfSSL_CTX_set_default_read_ahead(WOLFSSL_CTX* ctx, int m);
WOLFSSL_API long wolfSSL_SSL_get_mode(WOLFSSL* ssl);


WOLFSSL_API int  wolfSSL_CTX_set_default_verify_paths(WOLFSSL_CTX*);
WOLFSSL_API int  wolfSSL_CTX_set_session_id_context(WOLFSSL_CTX*,
                                            const unsigned char*, unsigned int);
WOLFSSL_API WOLFSSL_X509* wolfSSL_get_peer_certificate(WOLFSSL* ssl);

WOLFSSL_API int wolfSSL_want_read(WOLFSSL*);
WOLFSSL_API int wolfSSL_want_write(WOLFSSL*);

WOLFSSL_API int wolfSSL_BIO_printf(WOLFSSL_BIO*, const char*, ...);
WOLFSSL_API int wolfSSL_ASN1_UTCTIME_print(WOLFSSL_BIO*,
                                         const WOLFSSL_ASN1_UTCTIME*);
WOLFSSL_API int wolfSSL_ASN1_GENERALIZEDTIME_print(WOLFSSL_BIO*,
                                         const WOLFSSL_ASN1_GENERALIZEDTIME*);
WOLFSSL_API int   wolfSSL_sk_num(WOLFSSL_X509_REVOKED*);
WOLFSSL_API void* wolfSSL_sk_value(WOLFSSL_X509_REVOKED*, int);

/* stunnel 4.28 needs */
WOLFSSL_API void* wolfSSL_CTX_get_ex_data(const WOLFSSL_CTX*, int);
WOLFSSL_API int   wolfSSL_CTX_set_ex_data(WOLFSSL_CTX*, int, void*);
WOLFSSL_API void  wolfSSL_CTX_sess_set_get_cb(WOLFSSL_CTX*,
                       WOLFSSL_SESSION*(*f)(WOLFSSL*, unsigned char*, int, int*));
WOLFSSL_API void  wolfSSL_CTX_sess_set_new_cb(WOLFSSL_CTX*,
                                            int (*f)(WOLFSSL*, WOLFSSL_SESSION*));
WOLFSSL_API void  wolfSSL_CTX_sess_set_remove_cb(WOLFSSL_CTX*,
                                       void (*f)(WOLFSSL_CTX*, WOLFSSL_SESSION*));

WOLFSSL_API int          wolfSSL_i2d_SSL_SESSION(WOLFSSL_SESSION*,unsigned char**);
WOLFSSL_API WOLFSSL_SESSION* wolfSSL_d2i_SSL_SESSION(WOLFSSL_SESSION**,
                                                   const unsigned char**, long);

WOLFSSL_API long wolfSSL_SESSION_get_timeout(const WOLFSSL_SESSION*);
WOLFSSL_API long wolfSSL_SESSION_get_time(const WOLFSSL_SESSION*);
WOLFSSL_API int  wolfSSL_CTX_get_ex_new_index(long, void*, void*, void*, void*);

/* extra ends */


/* wolfSSL extensions */

/* call before SSL_connect, if verifying will add name check to
   date check and signature check */
WOLFSSL_API int wolfSSL_check_domain_name(WOLFSSL* ssl, const char* dn);

/* need to call once to load library (session cache) */
WOLFSSL_API int wolfSSL_Init(void);
/* call when done to cleanup/free session cache mutex / resources  */
WOLFSSL_API int wolfSSL_Cleanup(void);

/* which library version do we have */
WOLFSSL_API const char* wolfSSL_lib_version(void);
/* which library version do we have in hex */
WOLFSSL_API unsigned int wolfSSL_lib_version_hex(void);

/* turn logging on, only if compiled in */
WOLFSSL_API int  wolfSSL_Debugging_ON(void);
/* turn logging off */
WOLFSSL_API void wolfSSL_Debugging_OFF(void);

/* do accept or connect depedning on side */
WOLFSSL_API int wolfSSL_negotiate(WOLFSSL* ssl);
/* turn on wolfSSL data compression */
WOLFSSL_API int wolfSSL_set_compression(WOLFSSL* ssl);

WOLFSSL_API int wolfSSL_set_timeout(WOLFSSL*, unsigned int);
WOLFSSL_API int wolfSSL_CTX_set_timeout(WOLFSSL_CTX*, unsigned int);

/* get wolfSSL peer X509_CHAIN */
WOLFSSL_API WOLFSSL_X509_CHAIN* wolfSSL_get_peer_chain(WOLFSSL* ssl);
/* peer chain count */
WOLFSSL_API int  wolfSSL_get_chain_count(WOLFSSL_X509_CHAIN* chain);
/* index cert length */
WOLFSSL_API int  wolfSSL_get_chain_length(WOLFSSL_X509_CHAIN*, int idx);
/* index cert */
WOLFSSL_API unsigned char* wolfSSL_get_chain_cert(WOLFSSL_X509_CHAIN*, int idx);
/* index cert in X509 */
WOLFSSL_API WOLFSSL_X509* wolfSSL_get_chain_X509(WOLFSSL_X509_CHAIN*, int idx);
/* free X509 */
WOLFSSL_API void wolfSSL_FreeX509(WOLFSSL_X509*);
/* get index cert in PEM */
WOLFSSL_API int  wolfSSL_get_chain_cert_pem(WOLFSSL_X509_CHAIN*, int idx,
                                unsigned char* buf, int inLen, int* outLen);
WOLFSSL_API const unsigned char* wolfSSL_get_sessionID(const WOLFSSL_SESSION* s);
WOLFSSL_API int  wolfSSL_X509_get_serial_number(WOLFSSL_X509*,unsigned char*,int*);
WOLFSSL_API char*  wolfSSL_X509_get_subjectCN(WOLFSSL_X509*);
WOLFSSL_API const unsigned char* wolfSSL_X509_get_der(WOLFSSL_X509*, int*);
WOLFSSL_API const unsigned char* wolfSSL_X509_notBefore(WOLFSSL_X509*);
WOLFSSL_API const unsigned char* wolfSSL_X509_notAfter(WOLFSSL_X509*);
WOLFSSL_API int wolfSSL_X509_version(WOLFSSL_X509*);

WOLFSSL_API int wolfSSL_cmp_peer_cert_to_file(WOLFSSL*, const char*);

WOLFSSL_API char* wolfSSL_X509_get_next_altname(WOLFSSL_X509*);

WOLFSSL_API WOLFSSL_X509*
    wolfSSL_X509_d2i(WOLFSSL_X509** x509, const unsigned char* in, int len);
#ifndef NO_FILESYSTEM
    #ifndef NO_STDIO_FILESYSTEM
    WOLFSSL_API WOLFSSL_X509*
        wolfSSL_X509_d2i_fp(WOLFSSL_X509** x509, FILE* file);
    #endif
WOLFSSL_API WOLFSSL_X509*
    wolfSSL_X509_load_certificate_file(const char* fname, int format);
#endif
WOLFSSL_API WOLFSSL_X509* wolfSSL_X509_load_certificate_buffer(
    const unsigned char* buf, int sz, int format);

#ifdef WOLFSSL_SEP
    WOLFSSL_API unsigned char*
           wolfSSL_X509_get_device_type(WOLFSSL_X509*, unsigned char*, int*);
    WOLFSSL_API unsigned char*
           wolfSSL_X509_get_hw_type(WOLFSSL_X509*, unsigned char*, int*);
    WOLFSSL_API unsigned char*
           wolfSSL_X509_get_hw_serial_number(WOLFSSL_X509*, unsigned char*, int*);
#endif

/* connect enough to get peer cert */
WOLFSSL_API int  wolfSSL_connect_cert(WOLFSSL* ssl);



/* PKCS12 compatibility */
typedef struct WC_PKCS12 WC_PKCS12;
WOLFSSL_API WC_PKCS12* wolfSSL_d2i_PKCS12_bio(WOLFSSL_BIO* bio,
                                       WC_PKCS12** pkcs12);
WOLFSSL_API int wolfSSL_PKCS12_parse(WC_PKCS12* pkcs12, const char* psw,
     WOLFSSL_EVP_PKEY** pkey, WOLFSSL_X509** cert, STACK_OF(WOLFSSL_X509)** ca);
WOLFSSL_API void wolfSSL_PKCS12_PBE_add(void);



#ifndef NO_DH
/* server Diffie-Hellman parameters */
WOLFSSL_API int  wolfSSL_SetTmpDH(WOLFSSL*, const unsigned char* p, int pSz,
                                const unsigned char* g, int gSz);
WOLFSSL_API int  wolfSSL_SetTmpDH_buffer(WOLFSSL*, const unsigned char* b, long sz,
                                       int format);
#ifndef NO_FILESYSTEM
    WOLFSSL_API int  wolfSSL_SetTmpDH_file(WOLFSSL*, const char* f, int format);
#endif

/* server ctx Diffie-Hellman parameters */
WOLFSSL_API int  wolfSSL_CTX_SetTmpDH(WOLFSSL_CTX*, const unsigned char* p,
                                    int pSz, const unsigned char* g, int gSz);
WOLFSSL_API int  wolfSSL_CTX_SetTmpDH_buffer(WOLFSSL_CTX*, const unsigned char* b,
                                           long sz, int format);

#ifndef NO_FILESYSTEM
    WOLFSSL_API int  wolfSSL_CTX_SetTmpDH_file(WOLFSSL_CTX*, const char* f,
                                             int format);
#endif

WOLFSSL_API int wolfSSL_CTX_SetMinDhKey_Sz(WOLFSSL_CTX*, unsigned short);
WOLFSSL_API int wolfSSL_SetMinDhKey_Sz(WOLFSSL*, unsigned short);
WOLFSSL_API int wolfSSL_GetDhKey_Sz(WOLFSSL*);
#endif /* NO_DH */

#ifndef NO_RSA
WOLFSSL_API int wolfSSL_CTX_SetMinRsaKey_Sz(WOLFSSL_CTX*, short);
WOLFSSL_API int wolfSSL_SetMinRsaKey_Sz(WOLFSSL*, short);
#endif /* NO_RSA */

#ifdef HAVE_ECC
WOLFSSL_API int wolfSSL_CTX_SetMinEccKey_Sz(WOLFSSL_CTX*, short);
WOLFSSL_API int wolfSSL_SetMinEccKey_Sz(WOLFSSL*, short);
#endif /* NO_RSA */

WOLFSSL_API int  wolfSSL_SetTmpEC_DHE_Sz(WOLFSSL*, unsigned short);
WOLFSSL_API int  wolfSSL_CTX_SetTmpEC_DHE_Sz(WOLFSSL_CTX*, unsigned short);

/* keyblock size in bytes or -1 */
/* need to call wolfSSL_KeepArrays before handshake to save keys */
WOLFSSL_API int wolfSSL_get_keyblock_size(WOLFSSL*);
WOLFSSL_API int wolfSSL_get_keys(WOLFSSL*,unsigned char** ms, unsigned int* msLen,
                                       unsigned char** sr, unsigned int* srLen,
                                       unsigned char** cr, unsigned int* crLen);

/* Computes EAP-TLS and EAP-TTLS keying material from the master_secret. */
WOLFSSL_API int wolfSSL_make_eap_keys(WOLFSSL*, void* key, unsigned int len,
                                                             const char* label);


#ifndef _WIN32
    #ifndef NO_WRITEV
        #ifdef __PPU
            #include <sys/types.h>
            #include <sys/socket.h>
        #elif !defined(WOLFSSL_MDK_ARM) && !defined(WOLFSSL_IAR_ARM) && \
              !defined(WOLFSSL_PICOTCP) && !defined(WOLFSSL_ROWLEY_ARM) && \
              !defined(WOLFSSL_EMBOS) && !defined(WOLFSSL_FROSTED)
            #include <sys/uio.h>
        #endif
        /* allow writev style writing */
        WOLFSSL_API int wolfSSL_writev(WOLFSSL* ssl, const struct iovec* iov,
                                     int iovcnt);
    #endif
#endif


#ifndef NO_CERTS
    /* SSL_CTX versions */
    WOLFSSL_API int wolfSSL_CTX_UnloadCAs(WOLFSSL_CTX*);
#ifdef WOLFSSL_TRUST_PEER_CERT
    WOLFSSL_API int wolfSSL_CTX_Unload_trust_peers(WOLFSSL_CTX*);
    WOLFSSL_API int wolfSSL_CTX_trust_peer_buffer(WOLFSSL_CTX*,
                                               const unsigned char*, long, int);
#endif
    WOLFSSL_API int wolfSSL_CTX_load_verify_buffer(WOLFSSL_CTX*,
                                               const unsigned char*, long, int);
    WOLFSSL_API int wolfSSL_CTX_use_certificate_buffer(WOLFSSL_CTX*,
                                               const unsigned char*, long, int);
    WOLFSSL_API int wolfSSL_CTX_use_PrivateKey_buffer(WOLFSSL_CTX*,
                                               const unsigned char*, long, int);
    WOLFSSL_API int wolfSSL_CTX_use_certificate_chain_buffer_format(WOLFSSL_CTX*,
                                               const unsigned char*, long, int);
    WOLFSSL_API int wolfSSL_CTX_use_certificate_chain_buffer(WOLFSSL_CTX*,
                                                    const unsigned char*, long);

    /* SSL versions */
    WOLFSSL_API int wolfSSL_use_certificate_buffer(WOLFSSL*, const unsigned char*,
                                               long, int);
    WOLFSSL_API int wolfSSL_use_PrivateKey_buffer(WOLFSSL*, const unsigned char*,
                                               long, int);
    WOLFSSL_API int wolfSSL_use_certificate_chain_buffer_format(WOLFSSL*,
                                               const unsigned char*, long, int);
    WOLFSSL_API int wolfSSL_use_certificate_chain_buffer(WOLFSSL*,
                                               const unsigned char*, long);
    WOLFSSL_API int wolfSSL_UnloadCertsKeys(WOLFSSL*);

    #if defined(OPENSSL_EXTRA) && defined(KEEP_OUR_CERT)
        WOLFSSL_API WOLFSSL_X509* wolfSSL_get_certificate(WOLFSSL* ssl);
    #endif
#endif

WOLFSSL_API int wolfSSL_CTX_set_group_messages(WOLFSSL_CTX*);
WOLFSSL_API int wolfSSL_set_group_messages(WOLFSSL*);


#ifdef HAVE_FUZZER
enum fuzzer_type {
    FUZZ_HMAC      = 0,
    FUZZ_ENCRYPT   = 1,
    FUZZ_SIGNATURE = 2,
    FUZZ_HASH      = 3,
    FUZZ_HEAD      = 4
};

typedef int (*CallbackFuzzer)(WOLFSSL* ssl, const unsigned char* buf, int sz,
        int type, void* fuzzCtx);

WOLFSSL_API void wolfSSL_SetFuzzerCb(WOLFSSL* ssl, CallbackFuzzer cbf, void* fCtx);
#endif


WOLFSSL_API int   wolfSSL_DTLS_SetCookieSecret(WOLFSSL*,
                                               const unsigned char*,
                                               unsigned int);


/* I/O Callback default errors */
enum IOerrors {
    WOLFSSL_CBIO_ERR_GENERAL    = -1,     /* general unexpected err */
    WOLFSSL_CBIO_ERR_WANT_READ  = -2,     /* need to call read  again */
    WOLFSSL_CBIO_ERR_WANT_WRITE = -2,     /* need to call write again */
    WOLFSSL_CBIO_ERR_CONN_RST   = -3,     /* connection reset */
    WOLFSSL_CBIO_ERR_ISR        = -4,     /* interrupt */
    WOLFSSL_CBIO_ERR_CONN_CLOSE = -5,     /* connection closed or epipe */
    WOLFSSL_CBIO_ERR_TIMEOUT    = -6      /* socket timeout */
};


/* CA cache callbacks */
enum {
    WOLFSSL_SSLV3    = 0,
    WOLFSSL_TLSV1    = 1,
    WOLFSSL_TLSV1_1  = 2,
    WOLFSSL_TLSV1_2  = 3,
    WOLFSSL_USER_CA  = 1,          /* user added as trusted */
    WOLFSSL_CHAIN_CA = 2           /* added to cache from trusted chain */
};

WOLFSSL_API WC_RNG* wolfSSL_GetRNG(WOLFSSL*);

WOLFSSL_API int wolfSSL_CTX_SetMinVersion(WOLFSSL_CTX* ctx, int version);
WOLFSSL_API int wolfSSL_SetMinVersion(WOLFSSL* ssl, int version);
WOLFSSL_API int wolfSSL_GetObjectSize(void);  /* object size based on build */
WOLFSSL_API int wolfSSL_CTX_GetObjectSize(void); 
WOLFSSL_API int wolfSSL_METHOD_GetObjectSize(void);
WOLFSSL_API int wolfSSL_GetOutputSize(WOLFSSL*, int);
WOLFSSL_API int wolfSSL_GetMaxOutputSize(WOLFSSL*);
WOLFSSL_API int wolfSSL_SetVersion(WOLFSSL* ssl, int version);
WOLFSSL_API int wolfSSL_KeyPemToDer(const unsigned char*, int,
                                    unsigned char*, int, const char*);
WOLFSSL_API int wolfSSL_CertPemToDer(const unsigned char*, int,
                                     unsigned char*, int, int);
#if defined(WOLFSSL_CERT_EXT) || defined(WOLFSSL_PUB_PEM_TO_DER)
    #ifndef WOLFSSL_PEMPUBKEY_TODER_DEFINED
        #ifndef NO_FILESYSTEM
            WOLFSSL_API int wolfSSL_PemPubKeyToDer(const char* fileName,
                                                   unsigned char* derBuf, int derSz);
        #endif
        WOLFSSL_API int wolfSSL_PubKeyPemToDer(const unsigned char*, int,
                                               unsigned char*, int);
        #define WOLFSSL_PEMPUBKEY_TODER_DEFINED
    #endif /* WOLFSSL_PEMPUBKEY_TODER_DEFINED */
#endif /* WOLFSSL_CERT_EXT || WOLFSSL_PUB_PEM_TO_DER*/

typedef void (*CallbackCACache)(unsigned char* der, int sz, int type);
typedef void (*CbMissingCRL)(const char* url);
typedef int  (*CbOCSPIO)(void*, const char*, int,
                                         unsigned char*, int, unsigned char**);
typedef void (*CbOCSPRespFree)(void*,unsigned char*);

#ifdef HAVE_CRL_IO
typedef int  (*CbCrlIO)(WOLFSSL_CRL* crl, const char* url, int urlSz);
#endif

/* User Atomic Record Layer CallBacks */
typedef int (*CallbackMacEncrypt)(WOLFSSL* ssl, unsigned char* macOut,
       const unsigned char* macIn, unsigned int macInSz, int macContent,
       int macVerify, unsigned char* encOut, const unsigned char* encIn,
       unsigned int encSz, void* ctx);
WOLFSSL_API void  wolfSSL_CTX_SetMacEncryptCb(WOLFSSL_CTX*, CallbackMacEncrypt);
WOLFSSL_API void  wolfSSL_SetMacEncryptCtx(WOLFSSL* ssl, void *ctx);
WOLFSSL_API void* wolfSSL_GetMacEncryptCtx(WOLFSSL* ssl);

typedef int (*CallbackDecryptVerify)(WOLFSSL* ssl,
       unsigned char* decOut, const unsigned char* decIn,
       unsigned int decSz, int content, int verify, unsigned int* padSz,
       void* ctx);
WOLFSSL_API void  wolfSSL_CTX_SetDecryptVerifyCb(WOLFSSL_CTX*,
                                               CallbackDecryptVerify);
WOLFSSL_API void  wolfSSL_SetDecryptVerifyCtx(WOLFSSL* ssl, void *ctx);
WOLFSSL_API void* wolfSSL_GetDecryptVerifyCtx(WOLFSSL* ssl);

WOLFSSL_API const unsigned char* wolfSSL_GetMacSecret(WOLFSSL*, int);
WOLFSSL_API const unsigned char* wolfSSL_GetClientWriteKey(WOLFSSL*);
WOLFSSL_API const unsigned char* wolfSSL_GetClientWriteIV(WOLFSSL*);
WOLFSSL_API const unsigned char* wolfSSL_GetServerWriteKey(WOLFSSL*);
WOLFSSL_API const unsigned char* wolfSSL_GetServerWriteIV(WOLFSSL*);
WOLFSSL_API int                  wolfSSL_GetKeySize(WOLFSSL*);
WOLFSSL_API int                  wolfSSL_GetIVSize(WOLFSSL*);
WOLFSSL_API int                  wolfSSL_GetSide(WOLFSSL*);
WOLFSSL_API int                  wolfSSL_IsTLSv1_1(WOLFSSL*);
WOLFSSL_API int                  wolfSSL_GetBulkCipher(WOLFSSL*);
WOLFSSL_API int                  wolfSSL_GetCipherBlockSize(WOLFSSL*);
WOLFSSL_API int                  wolfSSL_GetAeadMacSize(WOLFSSL*);
WOLFSSL_API int                  wolfSSL_GetHmacSize(WOLFSSL*);
WOLFSSL_API int                  wolfSSL_GetHmacType(WOLFSSL*);
WOLFSSL_API int                  wolfSSL_GetCipherType(WOLFSSL*);
WOLFSSL_API int                  wolfSSL_SetTlsHmacInner(WOLFSSL*, unsigned char*,
                                                       unsigned int, int, int);

/* Atomic User Needs */
enum {
    WOLFSSL_SERVER_END = 0,
    WOLFSSL_CLIENT_END = 1,
    WOLFSSL_NEITHER_END = 3,
    WOLFSSL_BLOCK_TYPE = 2,
    WOLFSSL_STREAM_TYPE = 3,
    WOLFSSL_AEAD_TYPE = 4,
    WOLFSSL_TLS_HMAC_INNER_SZ = 13      /* SEQ_SZ + ENUM + VERSION_SZ + LEN_SZ */
};

/* for GetBulkCipher and internal use */
enum BulkCipherAlgorithm {
    wolfssl_cipher_null,
    wolfssl_rc4,
    wolfssl_rc2,
    wolfssl_des,
    wolfssl_triple_des,             /* leading 3 (3des) not valid identifier */
    wolfssl_des40,
    wolfssl_idea,
    wolfssl_aes,
    wolfssl_aes_gcm,
    wolfssl_aes_ccm,
    wolfssl_chacha,
    wolfssl_camellia,
    wolfssl_hc128,                  /* wolfSSL extensions */
    wolfssl_rabbit
};


/* for KDF TLS 1.2 mac types */
enum KDF_MacAlgorithm {
    wolfssl_sha256 = 4,     /* needs to match internal MACAlgorithm */
    wolfssl_sha384,
    wolfssl_sha512
};


/* Public Key Callback support */
typedef int (*CallbackEccSign)(WOLFSSL* ssl,
       const unsigned char* in, unsigned int inSz,
       unsigned char* out, unsigned int* outSz,
       const unsigned char* keyDer, unsigned int keySz,
       void* ctx);
WOLFSSL_API void  wolfSSL_CTX_SetEccSignCb(WOLFSSL_CTX*, CallbackEccSign);
WOLFSSL_API void  wolfSSL_SetEccSignCtx(WOLFSSL* ssl, void *ctx);
WOLFSSL_API void* wolfSSL_GetEccSignCtx(WOLFSSL* ssl);

typedef int (*CallbackEccVerify)(WOLFSSL* ssl,
       const unsigned char* sig, unsigned int sigSz,
       const unsigned char* hash, unsigned int hashSz,
       const unsigned char* keyDer, unsigned int keySz,
       int* result, void* ctx);
WOLFSSL_API void  wolfSSL_CTX_SetEccVerifyCb(WOLFSSL_CTX*, CallbackEccVerify);
WOLFSSL_API void  wolfSSL_SetEccVerifyCtx(WOLFSSL* ssl, void *ctx);
WOLFSSL_API void* wolfSSL_GetEccVerifyCtx(WOLFSSL* ssl);

struct ecc_key;
typedef int (*CallbackEccSharedSecret)(WOLFSSL* ssl, struct ecc_key* otherKey,
        unsigned char* pubKeyDer, unsigned int* pubKeySz,
        unsigned char* out, unsigned int* outlen,
        int side, void* ctx); /* side is WOLFSSL_CLIENT_END or WOLFSSL_SERVER_END */
WOLFSSL_API void  wolfSSL_CTX_SetEccSharedSecretCb(WOLFSSL_CTX*, CallbackEccSharedSecret);
WOLFSSL_API void  wolfSSL_SetEccSharedSecretCtx(WOLFSSL* ssl, void *ctx);
WOLFSSL_API void* wolfSSL_GetEccSharedSecretCtx(WOLFSSL* ssl);

struct ed25519_key;
typedef int (*CallbackEd25519Sign)(WOLFSSL* ssl,
       const unsigned char* in, unsigned int inSz,
       unsigned char* out, unsigned int* outSz,
       const unsigned char* keyDer, unsigned int keySz,
       void* ctx);
WOLFSSL_API void  wolfSSL_CTX_SetEd25519SignCb(WOLFSSL_CTX*,
                                               CallbackEd25519Sign);
WOLFSSL_API void  wolfSSL_SetEd25519SignCtx(WOLFSSL* ssl, void *ctx);
WOLFSSL_API void* wolfSSL_GetEd25519SignCtx(WOLFSSL* ssl);

typedef int (*CallbackEd25519Verify)(WOLFSSL* ssl,
       const unsigned char* sig, unsigned int sigSz,
       const unsigned char* msg, unsigned int msgSz,
       const unsigned char* keyDer, unsigned int keySz,
       int* result, void* ctx);
WOLFSSL_API void  wolfSSL_CTX_SetEd25519VerifyCb(WOLFSSL_CTX*,
                                                 CallbackEd25519Verify);
WOLFSSL_API void  wolfSSL_SetEd25519VerifyCtx(WOLFSSL* ssl, void *ctx);
WOLFSSL_API void* wolfSSL_GetEd25519VerifyCtx(WOLFSSL* ssl);

struct curve25519_key;
typedef int (*CallbackX25519SharedSecret)(WOLFSSL* ssl,
        struct curve25519_key* otherKey,
        unsigned char* pubKeyDer, unsigned int* pubKeySz,
        unsigned char* out, unsigned int* outlen,
        int side, void* ctx);
        /* side is WOLFSSL_CLIENT_END or WOLFSSL_SERVER_END */
WOLFSSL_API void  wolfSSL_CTX_SetX25519SharedSecretCb(WOLFSSL_CTX*,
        CallbackX25519SharedSecret);
WOLFSSL_API void  wolfSSL_SetX25519SharedSecretCtx(WOLFSSL* ssl, void *ctx);
WOLFSSL_API void* wolfSSL_GetX25519SharedSecretCtx(WOLFSSL* ssl);

typedef int (*CallbackRsaSign)(WOLFSSL* ssl,
       const unsigned char* in, unsigned int inSz,
       unsigned char* out, unsigned int* outSz,
       const unsigned char* keyDer, unsigned int keySz,
       void* ctx);
WOLFSSL_API void  wolfSSL_CTX_SetRsaSignCb(WOLFSSL_CTX*, CallbackRsaSign);
WOLFSSL_API void  wolfSSL_SetRsaSignCtx(WOLFSSL* ssl, void *ctx);
WOLFSSL_API void* wolfSSL_GetRsaSignCtx(WOLFSSL* ssl);

typedef int (*CallbackRsaVerify)(WOLFSSL* ssl,
       unsigned char* sig, unsigned int sigSz,
       unsigned char** out,
       const unsigned char* keyDer, unsigned int keySz,
       void* ctx);
WOLFSSL_API void  wolfSSL_CTX_SetRsaVerifyCb(WOLFSSL_CTX*, CallbackRsaVerify);
WOLFSSL_API void  wolfSSL_SetRsaVerifyCtx(WOLFSSL* ssl, void *ctx);
WOLFSSL_API void* wolfSSL_GetRsaVerifyCtx(WOLFSSL* ssl);

#ifdef WC_RSA_PSS
typedef int (*CallbackRsaPssSign)(WOLFSSL* ssl,
       const unsigned char* in, unsigned int inSz,
       unsigned char* out, unsigned int* outSz,
       int hash, int mgf,
       const unsigned char* keyDer, unsigned int keySz,
       void* ctx);
WOLFSSL_API void  wolfSSL_CTX_SetRsaPssSignCb(WOLFSSL_CTX*, CallbackRsaPssSign);
WOLFSSL_API void  wolfSSL_SetRsaPssSignCtx(WOLFSSL* ssl, void *ctx);
WOLFSSL_API void* wolfSSL_GetRsaPssSignCtx(WOLFSSL* ssl);

typedef int (*CallbackRsaPssVerify)(WOLFSSL* ssl,
       unsigned char* sig, unsigned int sigSz,
       unsigned char** out,
       int hash, int mgf,
       const unsigned char* keyDer, unsigned int keySz,
       void* ctx);
WOLFSSL_API void  wolfSSL_CTX_SetRsaPssVerifyCb(WOLFSSL_CTX*,
                                                CallbackRsaPssVerify);
WOLFSSL_API void  wolfSSL_SetRsaPssVerifyCtx(WOLFSSL* ssl, void *ctx);
WOLFSSL_API void* wolfSSL_GetRsaPssVerifyCtx(WOLFSSL* ssl);
#endif

/* RSA Public Encrypt cb */
typedef int (*CallbackRsaEnc)(WOLFSSL* ssl,
       const unsigned char* in, unsigned int inSz,
       unsigned char* out, unsigned int* outSz,
       const unsigned char* keyDer, unsigned int keySz,
       void* ctx);
WOLFSSL_API void  wolfSSL_CTX_SetRsaEncCb(WOLFSSL_CTX*, CallbackRsaEnc);
WOLFSSL_API void  wolfSSL_SetRsaEncCtx(WOLFSSL* ssl, void *ctx);
WOLFSSL_API void* wolfSSL_GetRsaEncCtx(WOLFSSL* ssl);

/* RSA Private Decrypt cb */
typedef int (*CallbackRsaDec)(WOLFSSL* ssl,
       unsigned char* in, unsigned int inSz,
       unsigned char** out,
       const unsigned char* keyDer, unsigned int keySz,
       void* ctx);
WOLFSSL_API void  wolfSSL_CTX_SetRsaDecCb(WOLFSSL_CTX*, CallbackRsaDec);
WOLFSSL_API void  wolfSSL_SetRsaDecCtx(WOLFSSL* ssl, void *ctx);
WOLFSSL_API void* wolfSSL_GetRsaDecCtx(WOLFSSL* ssl);


#ifndef NO_CERTS
    WOLFSSL_API void wolfSSL_CTX_SetCACb(WOLFSSL_CTX*, CallbackCACache);

    WOLFSSL_API WOLFSSL_CERT_MANAGER* wolfSSL_CertManagerNew_ex(void* heap);
    WOLFSSL_API WOLFSSL_CERT_MANAGER* wolfSSL_CertManagerNew(void);
    WOLFSSL_API void wolfSSL_CertManagerFree(WOLFSSL_CERT_MANAGER*);

    WOLFSSL_API int wolfSSL_CertManagerLoadCA(WOLFSSL_CERT_MANAGER*, const char* f,
                                                                 const char* d);
    WOLFSSL_API int wolfSSL_CertManagerLoadCABuffer(WOLFSSL_CERT_MANAGER*,
                                  const unsigned char* in, long sz, int format);
    WOLFSSL_API int wolfSSL_CertManagerUnloadCAs(WOLFSSL_CERT_MANAGER* cm);
#ifdef WOLFSSL_TRUST_PEER_CERT
    WOLFSSL_API int wolfSSL_CertManagerUnload_trust_peers(WOLFSSL_CERT_MANAGER* cm);
#endif
    WOLFSSL_API int wolfSSL_CertManagerVerify(WOLFSSL_CERT_MANAGER*, const char* f,
                                                                    int format);
    WOLFSSL_API int wolfSSL_CertManagerVerifyBuffer(WOLFSSL_CERT_MANAGER* cm,
                                const unsigned char* buff, long sz, int format);
    WOLFSSL_API int wolfSSL_CertManagerCheckCRL(WOLFSSL_CERT_MANAGER*,
                                                        unsigned char*, int sz);
    WOLFSSL_API int wolfSSL_CertManagerEnableCRL(WOLFSSL_CERT_MANAGER*,
                                                                   int options);
    WOLFSSL_API int wolfSSL_CertManagerDisableCRL(WOLFSSL_CERT_MANAGER*);
    WOLFSSL_API int wolfSSL_CertManagerLoadCRL(WOLFSSL_CERT_MANAGER*,
                                                         const char*, int, int);
    WOLFSSL_API int wolfSSL_CertManagerLoadCRLBuffer(WOLFSSL_CERT_MANAGER*,
                                            const unsigned char*, long sz, int);
    WOLFSSL_API int wolfSSL_CertManagerSetCRL_Cb(WOLFSSL_CERT_MANAGER*,
                                                                  CbMissingCRL);
#ifdef HAVE_CRL_IO
    WOLFSSL_API int wolfSSL_CertManagerSetCRL_IOCb(WOLFSSL_CERT_MANAGER*,
                                                                       CbCrlIO);
#endif
    WOLFSSL_API int wolfSSL_CertManagerCheckOCSP(WOLFSSL_CERT_MANAGER*,
                                                        unsigned char*, int sz);
    WOLFSSL_API int wolfSSL_CertManagerEnableOCSP(WOLFSSL_CERT_MANAGER*,
                                                                   int options);
    WOLFSSL_API int wolfSSL_CertManagerDisableOCSP(WOLFSSL_CERT_MANAGER*);
    WOLFSSL_API int wolfSSL_CertManagerSetOCSPOverrideURL(WOLFSSL_CERT_MANAGER*,
                                                                   const char*);
    WOLFSSL_API int wolfSSL_CertManagerSetOCSP_Cb(WOLFSSL_CERT_MANAGER*,
                                               CbOCSPIO, CbOCSPRespFree, void*);

    WOLFSSL_API int wolfSSL_CertManagerEnableOCSPStapling(
                                                      WOLFSSL_CERT_MANAGER* cm);

    WOLFSSL_API int wolfSSL_EnableCRL(WOLFSSL* ssl, int options);
    WOLFSSL_API int wolfSSL_DisableCRL(WOLFSSL* ssl);
    WOLFSSL_API int wolfSSL_LoadCRL(WOLFSSL*, const char*, int, int);
    WOLFSSL_API int wolfSSL_LoadCRLBuffer(WOLFSSL*,
                                          const unsigned char*, long sz, int);
    WOLFSSL_API int wolfSSL_SetCRL_Cb(WOLFSSL*, CbMissingCRL);
#ifdef HAVE_CRL_IO
    WOLFSSL_API int wolfSSL_SetCRL_IOCb(WOLFSSL* ssl, CbCrlIO cb);
#endif
    WOLFSSL_API int wolfSSL_EnableOCSP(WOLFSSL*, int options);
    WOLFSSL_API int wolfSSL_DisableOCSP(WOLFSSL*);
    WOLFSSL_API int wolfSSL_SetOCSP_OverrideURL(WOLFSSL*, const char*);
    WOLFSSL_API int wolfSSL_SetOCSP_Cb(WOLFSSL*, CbOCSPIO, CbOCSPRespFree, void*);

    WOLFSSL_API int wolfSSL_CTX_EnableCRL(WOLFSSL_CTX* ctx, int options);
    WOLFSSL_API int wolfSSL_CTX_DisableCRL(WOLFSSL_CTX* ctx);
    WOLFSSL_API int wolfSSL_CTX_LoadCRL(WOLFSSL_CTX*, const char*, int, int);
    WOLFSSL_API int wolfSSL_CTX_LoadCRLBuffer(WOLFSSL_CTX*,
                                            const unsigned char*, long sz, int);
    WOLFSSL_API int wolfSSL_CTX_SetCRL_Cb(WOLFSSL_CTX*, CbMissingCRL);
#ifdef HAVE_CRL_IO
    WOLFSSL_API int wolfSSL_CTX_SetCRL_IOCb(WOLFSSL_CTX*, CbCrlIO);
#endif
    WOLFSSL_API int wolfSSL_CTX_EnableOCSP(WOLFSSL_CTX*, int options);
    WOLFSSL_API int wolfSSL_CTX_DisableOCSP(WOLFSSL_CTX*);
    WOLFSSL_API int wolfSSL_CTX_SetOCSP_OverrideURL(WOLFSSL_CTX*, const char*);
    WOLFSSL_API int wolfSSL_CTX_SetOCSP_Cb(WOLFSSL_CTX*,
                                               CbOCSPIO, CbOCSPRespFree, void*);

    WOLFSSL_API int wolfSSL_CTX_EnableOCSPStapling(WOLFSSL_CTX*);
#endif /* !NO_CERTS */


#ifdef SINGLE_THREADED
    WOLFSSL_API int wolfSSL_CTX_new_rng(WOLFSSL_CTX*);
#endif

/* end of handshake frees temporary arrays, if user needs for get_keys or
   psk hints, call KeepArrays before handshake and then FreeArrays when done
   if don't want to wait for object free */
WOLFSSL_API void wolfSSL_KeepArrays(WOLFSSL*);
WOLFSSL_API void wolfSSL_FreeArrays(WOLFSSL*);

WOLFSSL_API int wolfSSL_KeepHandshakeResources(WOLFSSL* ssl);
WOLFSSL_API int wolfSSL_FreeHandshakeResources(WOLFSSL* ssl);

WOLFSSL_API int wolfSSL_CTX_UseClientSuites(WOLFSSL_CTX* ctx);
WOLFSSL_API int wolfSSL_UseClientSuites(WOLFSSL* ssl);

/* async additions */
WOLFSSL_API int wolfSSL_UseAsync(WOLFSSL*, int devId);
WOLFSSL_API int wolfSSL_CTX_UseAsync(WOLFSSL_CTX*, int devId);

/* helpers to get device id and heap */
WOLFSSL_API int   wolfSSL_CTX_GetDevId(WOLFSSL_CTX* ctx, WOLFSSL* ssl);
WOLFSSL_API void* wolfSSL_CTX_GetHeap(WOLFSSL_CTX* ctx, WOLFSSL* ssl);

/* TLS Extensions */

/* Server Name Indication */
#ifdef HAVE_SNI

/* SNI types */
enum {
    WOLFSSL_SNI_HOST_NAME = 0
};

WOLFSSL_API int wolfSSL_UseSNI(WOLFSSL* ssl, unsigned char type,
                                         const void* data, unsigned short size);
WOLFSSL_API int wolfSSL_CTX_UseSNI(WOLFSSL_CTX* ctx, unsigned char type,
                                         const void* data, unsigned short size);

#ifndef NO_WOLFSSL_SERVER

/* SNI options */
enum {
    /* Do not abort the handshake if the requested SNI didn't match. */
    WOLFSSL_SNI_CONTINUE_ON_MISMATCH = 0x01,

    /* Behave as if the requested SNI matched in a case of mismatch.  */
    /* In this case, the status will be set to WOLFSSL_SNI_FAKE_MATCH. */
    WOLFSSL_SNI_ANSWER_ON_MISMATCH   = 0x02,

    /* Abort the handshake if the client didn't send a SNI request. */
    WOLFSSL_SNI_ABORT_ON_ABSENCE     = 0x04,
};

WOLFSSL_API void wolfSSL_SNI_SetOptions(WOLFSSL* ssl, unsigned char type,
                                                         unsigned char options);
WOLFSSL_API void wolfSSL_CTX_SNI_SetOptions(WOLFSSL_CTX* ctx,
                                     unsigned char type, unsigned char options);

/* SNI status */
enum {
    WOLFSSL_SNI_NO_MATCH   = 0,
    WOLFSSL_SNI_FAKE_MATCH = 1, /**< @see WOLFSSL_SNI_ANSWER_ON_MISMATCH */
    WOLFSSL_SNI_REAL_MATCH = 2,
    WOLFSSL_SNI_FORCE_KEEP = 3  /** Used with -DWOLFSSL_ALWAYS_KEEP_SNI */
};

WOLFSSL_API unsigned char wolfSSL_SNI_Status(WOLFSSL* ssl, unsigned char type);

WOLFSSL_API unsigned short wolfSSL_SNI_GetRequest(WOLFSSL *ssl,
                                               unsigned char type, void** data);
WOLFSSL_API int wolfSSL_SNI_GetFromBuffer(
                 const unsigned char* clientHello, unsigned int helloSz,
                 unsigned char type, unsigned char* sni, unsigned int* inOutSz);

#endif
#endif

/* Application-Layer Protocol Negotiation */
#ifdef HAVE_ALPN

/* ALPN status code */
enum {
    WOLFSSL_ALPN_NO_MATCH = 0,
    WOLFSSL_ALPN_MATCH    = 1,
    WOLFSSL_ALPN_CONTINUE_ON_MISMATCH = 2,
    WOLFSSL_ALPN_FAILED_ON_MISMATCH = 4,
};

enum {
    WOLFSSL_MAX_ALPN_PROTO_NAME_LEN = 255,
    WOLFSSL_MAX_ALPN_NUMBER = 257
};

#if defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
typedef int (*CallbackALPNSelect)(WOLFSSL* ssl, const unsigned char** out,
    unsigned char* outLen, const unsigned char* in, unsigned int inLen,
    void *arg);
#endif

WOLFSSL_API int wolfSSL_UseALPN(WOLFSSL* ssl, char *protocol_name_list,
                                unsigned int protocol_name_listSz,
                                unsigned char options);

WOLFSSL_API int wolfSSL_ALPN_GetProtocol(WOLFSSL* ssl, char **protocol_name,
                                         unsigned short *size);

WOLFSSL_API int wolfSSL_ALPN_GetPeerProtocol(WOLFSSL* ssl, char **list,
                                             unsigned short *listSz);
WOLFSSL_API int wolfSSL_ALPN_FreePeerProtocol(WOLFSSL* ssl, char **list);
#endif /* HAVE_ALPN */

/* Maximum Fragment Length */
#ifdef HAVE_MAX_FRAGMENT

/* Fragment lengths */
enum {
    WOLFSSL_MFL_2_9  = 1, /*  512 bytes */
    WOLFSSL_MFL_2_10 = 2, /* 1024 bytes */
    WOLFSSL_MFL_2_11 = 3, /* 2048 bytes */
    WOLFSSL_MFL_2_12 = 4, /* 4096 bytes */
    WOLFSSL_MFL_2_13 = 5  /* 8192 bytes *//* wolfSSL ONLY!!! */
};

#ifndef NO_WOLFSSL_CLIENT

WOLFSSL_API int wolfSSL_UseMaxFragment(WOLFSSL* ssl, unsigned char mfl);
WOLFSSL_API int wolfSSL_CTX_UseMaxFragment(WOLFSSL_CTX* ctx, unsigned char mfl);

#endif
#endif

/* Truncated HMAC */
#ifdef HAVE_TRUNCATED_HMAC
#ifndef NO_WOLFSSL_CLIENT

WOLFSSL_API int wolfSSL_UseTruncatedHMAC(WOLFSSL* ssl);
WOLFSSL_API int wolfSSL_CTX_UseTruncatedHMAC(WOLFSSL_CTX* ctx);

#endif
#endif

/* Certificate Status Request */
/* Certificate Status Type */
enum {
    WOLFSSL_CSR_OCSP = 1
};

/* Certificate Status Options (flags) */
enum {
    WOLFSSL_CSR_OCSP_USE_NONCE = 0x01
};

#ifdef HAVE_CERTIFICATE_STATUS_REQUEST
#ifndef NO_WOLFSSL_CLIENT

WOLFSSL_API int wolfSSL_UseOCSPStapling(WOLFSSL* ssl,
                              unsigned char status_type, unsigned char options);

WOLFSSL_API int wolfSSL_CTX_UseOCSPStapling(WOLFSSL_CTX* ctx,
                              unsigned char status_type, unsigned char options);

#endif
#endif

/* Certificate Status Request v2 */
/* Certificate Status Type */
enum {
    WOLFSSL_CSR2_OCSP = 1,
    WOLFSSL_CSR2_OCSP_MULTI = 2
};

/* Certificate Status v2 Options (flags) */
enum {
    WOLFSSL_CSR2_OCSP_USE_NONCE = 0x01
};

#ifdef HAVE_CERTIFICATE_STATUS_REQUEST_V2
#ifndef NO_WOLFSSL_CLIENT

WOLFSSL_API int wolfSSL_UseOCSPStaplingV2(WOLFSSL* ssl,
                              unsigned char status_type, unsigned char options);

WOLFSSL_API int wolfSSL_CTX_UseOCSPStaplingV2(WOLFSSL_CTX* ctx,
                              unsigned char status_type, unsigned char options);

#endif
#endif

/* Named Groups */
enum {
#if 0 /* Not Supported */
    WOLFSSL_ECC_SECT163K1 = 1,
    WOLFSSL_ECC_SECT163R1 = 2,
    WOLFSSL_ECC_SECT163R2 = 3,
    WOLFSSL_ECC_SECT193R1 = 4,
    WOLFSSL_ECC_SECT193R2 = 5,
    WOLFSSL_ECC_SECT233K1 = 6,
    WOLFSSL_ECC_SECT233R1 = 7,
    WOLFSSL_ECC_SECT239K1 = 8,
    WOLFSSL_ECC_SECT283K1 = 9,
    WOLFSSL_ECC_SECT283R1 = 10,
    WOLFSSL_ECC_SECT409K1 = 11,
    WOLFSSL_ECC_SECT409R1 = 12,
    WOLFSSL_ECC_SECT571K1 = 13,
    WOLFSSL_ECC_SECT571R1 = 14,
#endif
    WOLFSSL_ECC_SECP160K1 = 15,
    WOLFSSL_ECC_SECP160R1 = 16,
    WOLFSSL_ECC_SECP160R2 = 17,
    WOLFSSL_ECC_SECP192K1 = 18,
    WOLFSSL_ECC_SECP192R1 = 19,
    WOLFSSL_ECC_SECP224K1 = 20,
    WOLFSSL_ECC_SECP224R1 = 21,
    WOLFSSL_ECC_SECP256K1 = 22,
    WOLFSSL_ECC_SECP256R1 = 23,
    WOLFSSL_ECC_SECP384R1 = 24,
    WOLFSSL_ECC_SECP521R1 = 25,
    WOLFSSL_ECC_BRAINPOOLP256R1 = 26,
    WOLFSSL_ECC_BRAINPOOLP384R1 = 27,
    WOLFSSL_ECC_BRAINPOOLP512R1 = 28,
    WOLFSSL_ECC_X25519    = 29,
#ifdef WOLFSSL_TLS13
    /* Not implemented. */
    WOLFSSL_ECC_X448      = 30,

    WOLFSSL_FFDHE_2048    = 256,
    WOLFSSL_FFDHE_3072    = 257,
    WOLFSSL_FFDHE_4096    = 258,
    WOLFSSL_FFDHE_6144    = 259,
    WOLFSSL_FFDHE_8192    = 260,
#endif
};

#ifdef HAVE_SUPPORTED_CURVES
#ifndef NO_WOLFSSL_CLIENT

WOLFSSL_API int wolfSSL_UseSupportedCurve(WOLFSSL* ssl, unsigned short name);
WOLFSSL_API int wolfSSL_CTX_UseSupportedCurve(WOLFSSL_CTX* ctx,
                                                           unsigned short name);

#endif
#endif

#ifdef WOLFSSL_TLS13
WOLFSSL_API int wolfSSL_UseKeyShare(WOLFSSL* ssl, unsigned short group);
WOLFSSL_API int wolfSSL_NoKeyShares(WOLFSSL* ssl);
#endif


/* Secure Renegotiation */
#ifdef HAVE_SECURE_RENEGOTIATION

WOLFSSL_API int wolfSSL_UseSecureRenegotiation(WOLFSSL* ssl);
WOLFSSL_API int wolfSSL_Rehandshake(WOLFSSL* ssl);

#endif

/* Session Ticket */
#ifdef HAVE_SESSION_TICKET

#ifndef NO_WOLFSSL_CLIENT
WOLFSSL_API int wolfSSL_UseSessionTicket(WOLFSSL* ssl);
WOLFSSL_API int wolfSSL_CTX_UseSessionTicket(WOLFSSL_CTX* ctx);
WOLFSSL_API int wolfSSL_get_SessionTicket(WOLFSSL*, unsigned char*, unsigned int*);
WOLFSSL_API int wolfSSL_set_SessionTicket(WOLFSSL*, const unsigned char*, unsigned int);
typedef int (*CallbackSessionTicket)(WOLFSSL*, const unsigned char*, int, void*);
WOLFSSL_API int wolfSSL_set_SessionTicket_cb(WOLFSSL*,
                                                  CallbackSessionTicket, void*);
#endif /* NO_WOLFSSL_CLIENT */

#ifndef NO_WOLFSSL_SERVER

#define WOLFSSL_TICKET_NAME_SZ 16
#define WOLFSSL_TICKET_IV_SZ   16
#define WOLFSSL_TICKET_MAC_SZ  32

enum TicketEncRet {
    WOLFSSL_TICKET_RET_FATAL  = -1,  /* fatal error, don't use ticket */
    WOLFSSL_TICKET_RET_OK     =  0,  /* ok, use ticket */
    WOLFSSL_TICKET_RET_REJECT,       /* don't use ticket, but not fatal */
    WOLFSSL_TICKET_RET_CREATE        /* existing ticket ok and create new one */
};

typedef int (*SessionTicketEncCb)(WOLFSSL*,
                                 unsigned char key_name[WOLFSSL_TICKET_NAME_SZ],
                                 unsigned char iv[WOLFSSL_TICKET_IV_SZ],
                                 unsigned char mac[WOLFSSL_TICKET_MAC_SZ],
                                 int enc, unsigned char*, int, int*, void*);
WOLFSSL_API int wolfSSL_CTX_set_TicketEncCb(WOLFSSL_CTX* ctx,
                                            SessionTicketEncCb);
WOLFSSL_API int wolfSSL_CTX_set_TicketHint(WOLFSSL_CTX* ctx, int);
WOLFSSL_API int wolfSSL_CTX_set_TicketEncCtx(WOLFSSL_CTX* ctx, void*);

#endif /* NO_WOLFSSL_SERVER */

#endif /* HAVE_SESSION_TICKET */

#ifdef HAVE_QSH
/* Quantum-safe Crypto Schemes */
enum {
    WOLFSSL_NTRU_EESS439 = 0x0101, /* max plaintext length of 65  */
    WOLFSSL_NTRU_EESS593 = 0x0102, /* max plaintext length of 86  */
    WOLFSSL_NTRU_EESS743 = 0x0103, /* max plaintext length of 106 */
    WOLFSSL_LWE_XXX  = 0x0201,     /* Learning With Error encryption scheme */
    WOLFSSL_HFE_XXX  = 0x0301,     /* Hidden Field Equation scheme */
    WOLFSSL_NULL_QSH = 0xFFFF      /* QSHScheme is not used */
};


/* test if the connection is using a QSH secure connection return 1 if so */
WOLFSSL_API int wolfSSL_isQSH(WOLFSSL* ssl);
WOLFSSL_API int wolfSSL_UseSupportedQSH(WOLFSSL* ssl, unsigned short name);
#ifndef NO_WOLFSSL_CLIENT
    /* user control over sending client public key in hello
       when flag = 1 will send keys if flag is 0 or function is not called
       then will not send keys in the hello extension */
    WOLFSSL_API int wolfSSL_UseClientQSHKeys(WOLFSSL* ssl, unsigned char flag);
#endif

#endif /* QSH */

/* TLS Extended Master Secret Extension */
WOLFSSL_API int wolfSSL_DisableExtendedMasterSecret(WOLFSSL* ssl);
WOLFSSL_API int wolfSSL_CTX_DisableExtendedMasterSecret(WOLFSSL_CTX* ctx);


#define WOLFSSL_CRL_MONITOR   0x01   /* monitor this dir flag */
#define WOLFSSL_CRL_START_MON 0x02   /* start monitoring flag */


/* notify user the handshake is done */
typedef int (*HandShakeDoneCb)(WOLFSSL*, void*);
WOLFSSL_API int wolfSSL_SetHsDoneCb(WOLFSSL*, HandShakeDoneCb, void*);


WOLFSSL_API int wolfSSL_PrintSessionStats(void);
WOLFSSL_API int wolfSSL_get_session_stats(unsigned int* active,
                                          unsigned int* total,
                                          unsigned int* peak,
                                          unsigned int* maxSessions);
/* External facing KDF */
WOLFSSL_API
int wolfSSL_MakeTlsMasterSecret(unsigned char* ms, unsigned int msLen,
                               const unsigned char* pms, unsigned int pmsLen,
                               const unsigned char* cr, const unsigned char* sr,
                               int tls1_2, int hash_type);

WOLFSSL_API
int wolfSSL_MakeTlsExtendedMasterSecret(unsigned char* ms, unsigned int msLen,
                              const unsigned char* pms, unsigned int pmsLen,
                              const unsigned char* sHash, unsigned int sHashLen,
                              int tls1_2, int hash_type);

WOLFSSL_API
int wolfSSL_DeriveTlsKeys(unsigned char* key_data, unsigned int keyLen,
                               const unsigned char* ms, unsigned int msLen,
                               const unsigned char* sr, const unsigned char* cr,
                               int tls1_2, int hash_type);

#ifdef WOLFSSL_CALLBACKS

/* used internally by wolfSSL while OpenSSL types aren't */
#include <wolfssl/callbacks.h>

typedef int (*HandShakeCallBack)(HandShakeInfo*);
typedef int (*TimeoutCallBack)(TimeoutInfo*);

/* wolfSSL connect extension allowing HandShakeCallBack and/or TimeoutCallBack
   for diagnostics */
WOLFSSL_API int wolfSSL_connect_ex(WOLFSSL*, HandShakeCallBack, TimeoutCallBack,
                                 Timeval);
WOLFSSL_API int wolfSSL_accept_ex(WOLFSSL*, HandShakeCallBack, TimeoutCallBack,
                                Timeval);

#endif /* WOLFSSL_CALLBACKS */


#ifdef WOLFSSL_HAVE_WOLFSCEP
    WOLFSSL_API void wolfSSL_wolfSCEP(void);
#endif /* WOLFSSL_HAVE_WOLFSCEP */

#ifdef WOLFSSL_HAVE_CERT_SERVICE
    WOLFSSL_API void wolfSSL_cert_service(void);
#endif

#if defined(WOLFSSL_MYSQL_COMPATIBLE) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
WOLFSSL_API char* wolfSSL_ASN1_TIME_to_string(WOLFSSL_ASN1_TIME* time,
                                                            char* buf, int len);
#endif /* WOLFSSL_MYSQL_COMPATIBLE */

#ifdef OPENSSL_EXTRA

#ifndef NO_FILESYSTEM
WOLFSSL_API long wolfSSL_BIO_set_fp(WOLFSSL_BIO *bio, XFILE fp, int c);
WOLFSSL_API long wolfSSL_BIO_get_fp(WOLFSSL_BIO *bio, XFILE* fp);
#endif

WOLFSSL_API unsigned long wolfSSL_ERR_peek_last_error_line(const char **file, int *line);
WOLFSSL_API long wolfSSL_ctrl(WOLFSSL* ssl, int cmd, long opt, void* pt);
WOLFSSL_API long wolfSSL_CTX_ctrl(WOLFSSL_CTX* ctx, int cmd, long opt,void* pt);

#ifndef NO_CERTS
WOLFSSL_API int wolfSSL_check_private_key(const WOLFSSL* ssl);
WOLFSSL_API void* wolfSSL_X509_get_ext_d2i(const WOLFSSL_X509* x509,
                                                     int nid, int* c, int* idx);
WOLFSSL_API int wolfSSL_X509_digest(const WOLFSSL_X509* x509,
        const WOLFSSL_EVP_MD* digest, unsigned char* buf, unsigned int* len);
WOLFSSL_API int wolfSSL_use_certificate(WOLFSSL* ssl, WOLFSSL_X509* x509);
WOLFSSL_API int wolfSSL_use_certificate_ASN1(WOLFSSL* ssl, unsigned char* der,
                                                                     int derSz);
WOLFSSL_API int wolfSSL_use_PrivateKey(WOLFSSL* ssl, WOLFSSL_EVP_PKEY* pkey);
WOLFSSL_API int wolfSSL_use_PrivateKey_ASN1(int pri, WOLFSSL* ssl,
                                            unsigned char* der, long derSz);
WOLFSSL_API WOLFSSL_EVP_PKEY *wolfSSL_get_privatekey(const WOLFSSL *ssl);
#ifndef NO_RSA
WOLFSSL_API int wolfSSL_use_RSAPrivateKey_ASN1(WOLFSSL* ssl, unsigned char* der,
                                                                long derSz);
#endif
#endif /* NO_CERTS */

WOLFSSL_API WOLFSSL_DH *wolfSSL_DSA_dup_DH(const WOLFSSL_DSA *r);

WOLFSSL_API int wolfSSL_SESSION_get_master_key(const WOLFSSL_SESSION* ses,
        unsigned char* out, int outSz);
WOLFSSL_API int wolfSSL_SESSION_get_master_key_length(const WOLFSSL_SESSION* ses);

WOLFSSL_API void wolfSSL_CTX_set_cert_store(WOLFSSL_CTX* ctx,
                                                       WOLFSSL_X509_STORE* str);
WOLFSSL_X509* wolfSSL_d2i_X509_bio(WOLFSSL_BIO* bio, WOLFSSL_X509** x509);
WOLFSSL_API WOLFSSL_X509_STORE* wolfSSL_CTX_get_cert_store(WOLFSSL_CTX* ctx);

WOLFSSL_API size_t wolfSSL_BIO_ctrl_pending(WOLFSSL_BIO *b);
WOLFSSL_API size_t wolfSSL_get_server_random(const WOLFSSL *ssl,
                                             unsigned char *out, size_t outlen);
WOLFSSL_API size_t wolfSSL_get_client_random(const WOLFSSL* ssl,
                                              unsigned char* out, size_t outSz);
WOLFSSL_API pem_password_cb* wolfSSL_CTX_get_default_passwd_cb(WOLFSSL_CTX *ctx);
WOLFSSL_API void *wolfSSL_CTX_get_default_passwd_cb_userdata(WOLFSSL_CTX *ctx);
WOLFSSL_API int wolfSSL_CTX_use_PrivateKey(WOLFSSL_CTX *ctx, WOLFSSL_EVP_PKEY *pkey);
WOLFSSL_API WOLFSSL_X509 *wolfSSL_PEM_read_bio_X509(WOLFSSL_BIO *bp, WOLFSSL_X509 **x, pem_password_cb *cb, void *u);
WOLFSSL_API WOLFSSL_X509 *wolfSSL_PEM_read_bio_X509_AUX
        (WOLFSSL_BIO *bp, WOLFSSL_X509 **x, pem_password_cb *cb, void *u);

/*lighttp compatibility */

#include <wolfssl/openssl/asn1.h>
struct WOLFSSL_X509_NAME_ENTRY {
    WOLFSSL_ASN1_OBJECT* object; /* not defined yet */
    WOLFSSL_ASN1_STRING  data;
    WOLFSSL_ASN1_STRING* value;  /* points to data, for lighttpd port */
    int set;
    int size;
};

#if defined(HAVE_LIGHTY) || defined(WOLFSSL_MYSQL_COMPATIBLE) \
                         || defined(HAVE_STUNNEL) \
                         || defined(WOLFSSL_NGINX) \
                         || defined(WOLFSSL_HAPROXY) \
                         || defined(OPENSSL_EXTRA)
WOLFSSL_API void wolfSSL_X509_NAME_free(WOLFSSL_X509_NAME *name);
WOLFSSL_API char wolfSSL_CTX_use_certificate(WOLFSSL_CTX *ctx, WOLFSSL_X509 *x);
WOLFSSL_API int wolfSSL_BIO_read_filename(WOLFSSL_BIO *b, const char *name);
/* These are to be merged shortly */
WOLFSSL_API const char *  wolfSSL_OBJ_nid2sn(int n);
WOLFSSL_API int wolfSSL_OBJ_obj2nid(const WOLFSSL_ASN1_OBJECT *o);
WOLFSSL_API int wolfSSL_OBJ_sn2nid(const char *sn);
WOLFSSL_API void wolfSSL_CTX_set_verify_depth(WOLFSSL_CTX *ctx,int depth);
WOLFSSL_API void wolfSSL_set_verify_depth(WOLFSSL *ssl,int depth);
WOLFSSL_API void* wolfSSL_get_app_data( const WOLFSSL *ssl);
WOLFSSL_API int wolfSSL_set_app_data(WOLFSSL *ssl, void *arg);
WOLFSSL_API WOLFSSL_ASN1_OBJECT * wolfSSL_X509_NAME_ENTRY_get_object(WOLFSSL_X509_NAME_ENTRY *ne);
WOLFSSL_API WOLFSSL_X509_NAME_ENTRY *wolfSSL_X509_NAME_get_entry(WOLFSSL_X509_NAME *name, int loc);
WOLFSSL_API void wolfSSL_sk_X509_NAME_pop_free(STACK_OF(WOLFSSL_X509_NAME)* sk, void f (WOLFSSL_X509_NAME*));
WOLFSSL_API unsigned char *wolfSSL_SHA1(const unsigned char *d, size_t n, unsigned char *md);
WOLFSSL_API int wolfSSL_X509_check_private_key(WOLFSSL_X509*, WOLFSSL_EVP_PKEY*);
WOLFSSL_API STACK_OF(WOLFSSL_X509_NAME) *wolfSSL_dup_CA_list( STACK_OF(WOLFSSL_X509_NAME) *sk );

/* end lighttpd*/
#endif
#endif

#if defined(HAVE_STUNNEL) || defined(HAVE_LIGHTY) \
                          || defined(WOLFSSL_MYSQL_COMPATIBLE) \
                          || defined(WOLFSSL_HAPROXY) \
                          || defined(OPENSSL_EXTRA)

WOLFSSL_API char* wolfSSL_OBJ_nid2ln(int n);
WOLFSSL_API int wolfSSL_OBJ_txt2nid(const char *sn);
WOLFSSL_API WOLFSSL_BIO* wolfSSL_BIO_new_file(const char *filename, const char *mode);
WOLFSSL_API long wolfSSL_CTX_set_tmp_dh(WOLFSSL_CTX*, WOLFSSL_DH*);
WOLFSSL_API WOLFSSL_DH *wolfSSL_PEM_read_bio_DHparams(WOLFSSL_BIO *bp,
    WOLFSSL_DH **x, pem_password_cb *cb, void *u);
WOLFSSL_API WOLFSSL_DSA *wolfSSL_PEM_read_bio_DSAparams(WOLFSSL_BIO *bp,
    WOLFSSL_DSA **x, pem_password_cb *cb, void *u);
WOLFSSL_API int wolfSSL_PEM_write_bio_X509(WOLFSSL_BIO *bp, WOLFSSL_X509 *x);
WOLFSSL_API long wolfSSL_CTX_get_options(WOLFSSL_CTX* ctx);



#endif /* HAVE_STUNNEL || HAVE_LIGHTY */


#if defined(HAVE_STUNNEL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)

#include <wolfssl/openssl/crypto.h>

/* SNI received callback type */
typedef int (*CallbackSniRecv)(WOLFSSL *ssl, int *ret, void* exArg);

WOLFSSL_API int wolfSSL_CRYPTO_set_mem_ex_functions(void *(*m) (size_t, const char *, int),
    void *(*r) (void *, size_t, const char *, int), void (*f) (void *));

WOLFSSL_API WOLFSSL_DH *wolfSSL_DH_generate_parameters(int prime_len, int generator,
    void (*callback) (int, int, void *), void *cb_arg);

WOLFSSL_API int wolfSSL_DH_generate_parameters_ex(WOLFSSL_DH*, int, int,
                           void (*callback) (int, int, void *));

WOLFSSL_API void wolfSSL_ERR_load_crypto_strings(void);

WOLFSSL_API unsigned long wolfSSL_ERR_peek_last_error(void);

WOLFSSL_API int wolfSSL_FIPS_mode(void);

WOLFSSL_API int wolfSSL_FIPS_mode_set(int r);

WOLFSSL_API int wolfSSL_RAND_set_rand_method(const void *meth);

WOLFSSL_API int wolfSSL_CIPHER_get_bits(const WOLFSSL_CIPHER *c, int *alg_bits);

WOLFSSL_API int wolfSSL_sk_X509_NAME_num(const STACK_OF(WOLFSSL_X509_NAME) *s);

WOLFSSL_API int wolfSSL_sk_X509_num(const STACK_OF(WOLFSSL_X509) *s);

WOLFSSL_API int wolfSSL_X509_NAME_print_ex(WOLFSSL_BIO*,WOLFSSL_X509_NAME*,int,
        unsigned long);

WOLFSSL_API WOLFSSL_ASN1_BIT_STRING* wolfSSL_X509_get0_pubkey_bitstr(
                            const WOLFSSL_X509*);

WOLFSSL_API int        wolfSSL_CTX_add_session(WOLFSSL_CTX*, WOLFSSL_SESSION*);

WOLFSSL_API WOLFSSL_CTX* wolfSSL_get_SSL_CTX(WOLFSSL* ssl);

WOLFSSL_API int  wolfSSL_version(WOLFSSL*);

WOLFSSL_API int wolfSSL_get_state(const WOLFSSL*);

WOLFSSL_API void* wolfSSL_sk_X509_NAME_value(const STACK_OF(WOLFSSL_X509_NAME)*, int);

WOLFSSL_API void* wolfSSL_sk_X509_value(STACK_OF(WOLFSSL_X509)*, int);

WOLFSSL_API STACK_OF(WOLFSSL_X509)* wolfSSL_get_peer_cert_chain(const WOLFSSL*);

WOLFSSL_API void* wolfSSL_SESSION_get_ex_data(const WOLFSSL_SESSION*, int);

WOLFSSL_API int   wolfSSL_SESSION_set_ex_data(WOLFSSL_SESSION*, int, void*);

WOLFSSL_API int wolfSSL_SESSION_get_ex_new_index(long,void*,void*,void*,
        CRYPTO_free_func*);

WOLFSSL_API int wolfSSL_X509_NAME_get_sz(WOLFSSL_X509_NAME*);


WOLFSSL_API const unsigned char* wolfSSL_SESSION_get_id(WOLFSSL_SESSION*,
        unsigned int*);

WOLFSSL_API int wolfSSL_set_tlsext_host_name(WOLFSSL *, const char *);

WOLFSSL_API const char* wolfSSL_get_servername(WOLFSSL *, unsigned char);

WOLFSSL_API WOLFSSL_CTX* wolfSSL_set_SSL_CTX(WOLFSSL*,WOLFSSL_CTX*);

WOLFSSL_API VerifyCallback wolfSSL_CTX_get_verify_callback(WOLFSSL_CTX*);

WOLFSSL_API void wolfSSL_CTX_set_servername_callback(WOLFSSL_CTX *,
        CallbackSniRecv);
WOLFSSL_API int wolfSSL_CTX_set_tlsext_servername_callback(WOLFSSL_CTX *,
        CallbackSniRecv);

WOLFSSL_API void wolfSSL_CTX_set_servername_arg(WOLFSSL_CTX *, void*);

WOLFSSL_API void WOLFSSL_ERR_remove_thread_state(void*);

#ifndef NO_FILESYSTEM
WOLFSSL_API void wolfSSL_print_all_errors_fp(XFILE *fp);
#endif

WOLFSSL_API long wolfSSL_CTX_clear_options(WOLFSSL_CTX*, long);

WOLFSSL_API void wolfSSL_THREADID_set_callback(void (*threadid_func)(void*));

WOLFSSL_API void wolfSSL_THREADID_set_numeric(void* id, unsigned long val);

WOLFSSL_API STACK_OF(WOLFSSL_X509)* wolfSSL_X509_STORE_get1_certs(
                               WOLFSSL_X509_STORE_CTX*, WOLFSSL_X509_NAME*);

WOLFSSL_API void wolfSSL_sk_X509_pop_free(STACK_OF(WOLFSSL_X509)* sk, void f (WOLFSSL_X509*));
#endif /* HAVE_STUNNEL || WOLFSSL_NGINX || WOLFSSL_HAPROXY */

#if defined(HAVE_STUNNEL) || defined(WOLFSSL_MYSQL_COMPATIBLE) \
                          || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)

WOLFSSL_API int wolfSSL_CTX_get_verify_mode(WOLFSSL_CTX* ctx);

#endif

#ifdef WOLFSSL_JNI
WOLFSSL_API int wolfSSL_set_jobject(WOLFSSL* ssl, void* objPtr);
WOLFSSL_API void* wolfSSL_get_jobject(WOLFSSL* ssl);
#endif /* WOLFSSL_JNI */


#ifdef WOLFSSL_ASYNC_CRYPT
WOLFSSL_API int wolfSSL_AsyncPoll(WOLFSSL* ssl, WOLF_EVENT_FLAG flags);
WOLFSSL_API int wolfSSL_CTX_AsyncPoll(WOLFSSL_CTX* ctx, WOLF_EVENT** events, int maxEvents,
    WOLF_EVENT_FLAG flags, int* eventCount);
#endif /* WOLFSSL_ASYNC_CRYPT */

#ifdef OPENSSL_EXTRA
WOLFSSL_API int wolfSSL_CTX_set1_curves_list(WOLFSSL_CTX* ctx, char* names);

typedef void (*SSL_Msg_Cb)(int write_p, int version, int content_type,
    const void *buf, size_t len, WOLFSSL *ssl, void *arg);

WOLFSSL_API int wolfSSL_CTX_set_msg_callback(WOLFSSL_CTX *ctx, SSL_Msg_Cb cb);
WOLFSSL_API int wolfSSL_set_msg_callback(WOLFSSL *ssl, SSL_Msg_Cb cb);
WOLFSSL_API int wolfSSL_CTX_set_msg_callback_arg(WOLFSSL_CTX *ctx, void* arg);
WOLFSSL_API int wolfSSL_set_msg_callback_arg(WOLFSSL *ssl, void* arg);
#endif

#ifdef OPENSSL_EXTRA
WOLFSSL_API unsigned long wolfSSL_ERR_peek_error_line_data(const char **file,
    int *line, const char **data, int *flags);
#endif

#if defined WOLFSSL_NGINX || defined WOLFSSL_HAPROXY
/* Not an OpenSSL API. */
WOLFSSL_LOCAL int wolfSSL_get_ocsp_response(WOLFSSL* ssl, byte** response);
/* Not an OpenSSL API. */
WOLFSSL_LOCAL char* wolfSSL_get_ocsp_url(WOLFSSL* ssl);
/* Not an OpenSSL API. */
WOLFSSL_API int wolfSSL_set_ocsp_url(WOLFSSL* ssl, char* url);

WOLFSSL_API STACK_OF(WOLFSSL_CIPHER) *wolfSSL_get_ciphers_compat(const WOLFSSL *ssl);
WOLFSSL_API void wolfSSL_OPENSSL_config(char *config_name);
WOLFSSL_API int wolfSSL_X509_get_ex_new_index(int idx, void *arg, void *a,
    void *b, void *c);
WOLFSSL_API void *wolfSSL_X509_get_ex_data(WOLFSSL_X509 *x509, int idx);
WOLFSSL_API int wolfSSL_X509_set_ex_data(WOLFSSL_X509 *x509, int idx,
    void *data);

WOLFSSL_API int wolfSSL_X509_NAME_digest(const WOLFSSL_X509_NAME *data,
    const WOLFSSL_EVP_MD *type, unsigned char *md, unsigned int *len);

WOLFSSL_API long wolfSSL_SSL_CTX_get_timeout(const WOLFSSL_CTX *ctx);
WOLFSSL_API int wolfSSL_SSL_CTX_set_tmp_ecdh(WOLFSSL_CTX *ctx,
    WOLFSSL_EC_KEY *ecdh);
WOLFSSL_API int wolfSSL_SSL_CTX_remove_session(WOLFSSL_CTX *,
    WOLFSSL_SESSION *c);

WOLFSSL_API WOLFSSL_BIO *wolfSSL_SSL_get_rbio(const WOLFSSL *s);
WOLFSSL_API WOLFSSL_BIO *wolfSSL_SSL_get_wbio(const WOLFSSL *s);
WOLFSSL_API int wolfSSL_SSL_do_handshake(WOLFSSL *s);
WOLFSSL_API int wolfSSL_SSL_in_init(WOLFSSL *a); /* #define in OpenSSL */
#ifndef NO_SESSION_CACHE
    WOLFSSL_API WOLFSSL_SESSION *wolfSSL_SSL_get0_session(const WOLFSSL *s);
#endif
WOLFSSL_API int wolfSSL_X509_check_host(WOLFSSL_X509 *x, const char *chk,
    size_t chklen, unsigned int flags, char **peername);

WOLFSSL_API int wolfSSL_i2a_ASN1_INTEGER(WOLFSSL_BIO *bp,
    const WOLFSSL_ASN1_INTEGER *a);

#ifdef HAVE_SESSION_TICKET
WOLFSSL_API int wolfSSL_CTX_set_tlsext_ticket_key_cb(WOLFSSL_CTX *, int (*)(
    WOLFSSL *ssl, unsigned char *name, unsigned char *iv,
    WOLFSSL_EVP_CIPHER_CTX *ectx, WOLFSSL_HMAC_CTX *hctx, int enc));
#endif

#ifdef HAVE_OCSP
WOLFSSL_API int wolfSSL_CTX_get_extra_chain_certs(WOLFSSL_CTX* ctx,
    STACK_OF(X509)** chain);
WOLFSSL_API int wolfSSL_CTX_set_tlsext_status_cb(WOLFSSL_CTX* ctx,
    int(*)(WOLFSSL*, void*));

WOLFSSL_API int wolfSSL_X509_STORE_CTX_get1_issuer(WOLFSSL_X509 **issuer,
    WOLFSSL_X509_STORE_CTX *ctx, WOLFSSL_X509 *x);

WOLFSSL_API void wolfSSL_X509_email_free(STACK_OF(WOLFSSL_STRING) *sk);
WOLFSSL_API STACK_OF(WOLFSSL_STRING) *wolfSSL_X509_get1_ocsp(WOLFSSL_X509 *x);

WOLFSSL_API int wolfSSL_X509_check_issued(WOLFSSL_X509 *issuer,
    WOLFSSL_X509 *subject);

WOLFSSL_API WOLFSSL_X509* wolfSSL_X509_dup(WOLFSSL_X509 *x);

WOLFSSL_API char* wolfSSL_sk_WOLFSSL_STRING_value(
    STACK_OF(WOLFSSL_STRING)* strings, int idx);
#endif /* HAVE_OCSP */

WOLFSSL_API int PEM_write_bio_WOLFSSL_X509(WOLFSSL_BIO *bio,
    WOLFSSL_X509 *cert);

#endif /* WOLFSSL_NGINX */

WOLFSSL_API void wolfSSL_get0_alpn_selected(const WOLFSSL *ssl,
        const unsigned char **data, unsigned int *len);
WOLFSSL_API int wolfSSL_select_next_proto(unsigned char **out,
        unsigned char *outlen,
        const unsigned char *in, unsigned int inlen,
        const unsigned char *client,
        unsigned int client_len);
WOLFSSL_API void wolfSSL_CTX_set_alpn_select_cb(WOLFSSL_CTX *ctx,
        int (*cb) (WOLFSSL *ssl,
            const unsigned char **out,
            unsigned char *outlen,
            const unsigned char *in,
            unsigned int inlen,
            void *arg), void *arg);
WOLFSSL_API void wolfSSL_CTX_set_next_protos_advertised_cb(WOLFSSL_CTX *s,
        int (*cb) (WOLFSSL *ssl,
            const unsigned char **out,
            unsigned int *outlen,
            void *arg), void *arg);
WOLFSSL_API void wolfSSL_CTX_set_next_proto_select_cb(WOLFSSL_CTX *s,
        int (*cb) (WOLFSSL *ssl,
            unsigned char **out,
            unsigned char *outlen,
            const unsigned char *in,
            unsigned int inlen,
            void *arg), void *arg);
WOLFSSL_API void wolfSSL_get0_next_proto_negotiated(const WOLFSSL *s, const unsigned char **data,
        unsigned *len);


#if defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
WOLFSSL_API const unsigned char *SSL_SESSION_get0_id_context(
        const WOLFSSL_SESSION *sess, unsigned int *sid_ctx_length);
WOLFSSL_API size_t SSL_get_finished(const WOLFSSL *s, void *buf, size_t count);
WOLFSSL_API size_t SSL_get_peer_finished(const WOLFSSL *s, void *buf, size_t count);
#endif

WOLFSSL_API int SSL_SESSION_set1_id(WOLFSSL_SESSION *s, const unsigned char *sid, unsigned int sid_len);
WOLFSSL_API int SSL_SESSION_set1_id_context(WOLFSSL_SESSION *s, const unsigned char *sid_ctx, unsigned int sid_ctx_len);
WOLFSSL_API void *X509_get0_tbs_sigalg(const WOLFSSL_X509 *x);
WOLFSSL_API void X509_ALGOR_get0(WOLFSSL_ASN1_OBJECT **paobj, int *pptype, const void **ppval, const void *algor);
WOLFSSL_API void *X509_get_X509_PUBKEY(void * x);
WOLFSSL_API int X509_PUBKEY_get0_param(WOLFSSL_ASN1_OBJECT **ppkalg, const unsigned char **pk, int *ppklen, void **pa, WOLFSSL_EVP_PKEY *pub);
WOLFSSL_API int EVP_PKEY_bits(WOLFSSL_EVP_PKEY *pkey);
WOLFSSL_API int i2d_X509(WOLFSSL_X509 *x, unsigned char **out);
WOLFSSL_API int i2t_ASN1_OBJECT(char *buf, int buf_len, WOLFSSL_ASN1_OBJECT *a);
WOLFSSL_API void SSL_CTX_set_tmp_dh_callback(WOLFSSL_CTX *ctx, WOLFSSL_DH *(*dh) (WOLFSSL *ssl, int is_export, int keylength));
WOLFSSL_API STACK_OF(SSL_COMP) *SSL_COMP_get_compression_methods(void);
WOLFSSL_API int X509_STORE_load_locations(WOLFSSL_X509_STORE *ctx, const char *file, const char *dir);
WOLFSSL_API int wolfSSL_sk_SSL_CIPHER_num(const void * p);
WOLFSSL_API int wolfSSL_sk_SSL_COMP_zero(WOLFSSL_STACK* st);
WOLFSSL_API WOLFSSL_CIPHER* wolfSSL_sk_SSL_CIPHER_value(void *ciphers, int idx);
WOLFSSL_API void ERR_load_SSL_strings(void);

#ifdef __cplusplus
    }  /* extern "C" */
#endif


#endif /* WOLFSSL_SSL_H */
