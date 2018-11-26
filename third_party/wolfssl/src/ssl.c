/* ssl.c
 *
 * Copyright (C) 2006-2017 wolfSSL Inc.
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

#ifdef HAVE_ERRNO_H
    #include <errno.h>
#endif

#include <wolfssl/internal.h>
#include <wolfssl/error-ssl.h>
#include <wolfssl/wolfcrypt/coding.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif


#ifndef WOLFSSL_ALLOW_NO_SUITES
    #if defined(NO_DH) && !defined(HAVE_ECC) && !defined(WOLFSSL_STATIC_RSA) \
                  && !defined(WOLFSSL_STATIC_DH) && !defined(WOLFSSL_STATIC_PSK)
        #error "No cipher suites defined because DH disabled, ECC disabled, and no static suites defined. Please see top of README"
    #endif
#endif

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL) || \
        defined(HAVE_WEBSERVER) || defined(WOLFSSL_KEY_GEN)
    #include <wolfssl/openssl/evp.h>
    /* openssl headers end, wolfssl internal headers next */
#endif

#include <wolfssl/wolfcrypt/wc_encrypt.h>

#ifdef OPENSSL_EXTRA
    /* openssl headers begin */
    #include <wolfssl/openssl/aes.h>
    #include <wolfssl/openssl/hmac.h>
    #include <wolfssl/openssl/crypto.h>
    #include <wolfssl/openssl/des.h>
    #include <wolfssl/openssl/bn.h>
    #include <wolfssl/openssl/buffer.h>
    #include <wolfssl/openssl/dh.h>
    #include <wolfssl/openssl/rsa.h>
    #include <wolfssl/openssl/pem.h>
    #include <wolfssl/openssl/ec.h>
    #include <wolfssl/openssl/ec25519.h>
    #include <wolfssl/openssl/ed25519.h>
    #include <wolfssl/openssl/ecdsa.h>
    #include <wolfssl/openssl/ecdh.h>
    #include <wolfssl/openssl/rc4.h>
    /* openssl headers end, wolfssl internal headers next */
    #include <wolfssl/wolfcrypt/hmac.h>
    #include <wolfssl/wolfcrypt/random.h>
    #include <wolfssl/wolfcrypt/des3.h>
    #include <wolfssl/wolfcrypt/md4.h>
    #include <wolfssl/wolfcrypt/md5.h>
    #include <wolfssl/wolfcrypt/arc4.h>
    #include <wolfssl/wolfcrypt/idea.h>
    #include <wolfssl/wolfcrypt/curve25519.h>
    #include <wolfssl/wolfcrypt/ed25519.h>
    #if defined(OPENSSL_ALL) || defined(HAVE_STUNNEL)
        #include <wolfssl/openssl/ocsp.h>
    #endif /* WITH_STUNNEL */
    #ifdef WOLFSSL_SHA512
        #include <wolfssl/wolfcrypt/sha512.h>
    #endif
    #if defined(WOLFCRYPT_HAVE_SRP) && !defined(NO_SHA256) \
        && !defined(WC_NO_RNG)
        #include <wolfssl/wolfcrypt/srp.h>
        #include <wolfssl/wolfcrypt/random.h>
    #endif
#endif

#ifdef NO_ASN
    #include <wolfssl/wolfcrypt/dh.h>
#endif


#ifdef WOLFSSL_SESSION_EXPORT
#ifdef WOLFSSL_DTLS
int wolfSSL_dtls_import(WOLFSSL* ssl, unsigned char* buf, unsigned int sz)
{
    WOLFSSL_ENTER("wolfSSL_session_import");

    if (ssl == NULL || buf == NULL) {
        return BAD_FUNC_ARG;
    }

    /* sanity checks on buffer and protocol are done in internal function */
    return wolfSSL_dtls_import_internal(ssl, buf, sz);
}


/* Sets the function to call for serializing the session. This function is
 * called right after the handshake is completed. */
int wolfSSL_CTX_dtls_set_export(WOLFSSL_CTX* ctx, wc_dtls_export func)
{

    WOLFSSL_ENTER("wolfSSL_CTX_dtls_set_export");

    /* purposefully allow func to be NULL */
    if (ctx == NULL) {
        return BAD_FUNC_ARG;
    }

    ctx->dtls_export = func;

    return WOLFSSL_SUCCESS;
}


/* Sets the function in WOLFSSL struct to call for serializing the session. This
 * function is called right after the handshake is completed. */
int wolfSSL_dtls_set_export(WOLFSSL* ssl, wc_dtls_export func)
{

    WOLFSSL_ENTER("wolfSSL_dtls_set_export");

    /* purposefully allow func to be NULL */
    if (ssl == NULL) {
        return BAD_FUNC_ARG;
    }

    ssl->dtls_export = func;

    return WOLFSSL_SUCCESS;
}


/* This function allows for directly serializing a session rather than using
 * callbacks. It has less overhead by removing a temporary buffer and gives
 * control over when the session gets serialized. When using callbacks the
 * session is always serialized immediatly after the handshake is finished.
 *
 * buf is the argument to contain the serialized session
 * sz  is the size of the buffer passed in
 * ssl is the WOLFSSL struct to serialize
 * returns the size of serialized session on success, 0 on no action, and
 *         negative value on error */
int wolfSSL_dtls_export(WOLFSSL* ssl, unsigned char* buf, unsigned int* sz)
{
    WOLFSSL_ENTER("wolfSSL_dtls_export");

    if (ssl == NULL || sz == NULL) {
        return BAD_FUNC_ARG;
    }

    if (buf == NULL) {
        *sz = MAX_EXPORT_BUFFER;
        return 0;
    }

    /* if not DTLS do nothing */
    if (!ssl->options.dtls) {
        WOLFSSL_MSG("Currently only DTLS export is supported");
        return 0;
    }

    /* copy over keys, options, and dtls state struct */
    return wolfSSL_dtls_export_internal(ssl, buf, *sz);
}


/* returns 0 on success */
int wolfSSL_send_session(WOLFSSL* ssl)
{
    int ret;
    byte* buf;
    word16 bufSz = MAX_EXPORT_BUFFER;

    WOLFSSL_ENTER("wolfSSL_send_session");

    if (ssl == NULL) {
        return BAD_FUNC_ARG;
    }

    buf = (byte*)XMALLOC(bufSz, ssl->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (buf == NULL) {
        return MEMORY_E;
    }

    /* if not DTLS do nothing */
    if (!ssl->options.dtls) {
        XFREE(buf, ssl->heap, DYNAMIC_TYPE_TMP_BUFFER);
        WOLFSSL_MSG("Currently only DTLS export is supported");
        return 0;
    }

    /* copy over keys, options, and dtls state struct */
    ret = wolfSSL_dtls_export_internal(ssl, buf, bufSz);
    if (ret < 0) {
        XFREE(buf, ssl->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return ret;
    }

    /* if no error ret has size of buffer */
    ret = ssl->dtls_export(ssl, buf, ret, NULL);
    if (ret != WOLFSSL_SUCCESS) {
        XFREE(buf, ssl->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return ret;
    }

    XFREE(buf, ssl->heap, DYNAMIC_TYPE_TMP_BUFFER);
    return 0;
}
#endif /* WOLFSSL_DTLS */
#endif /* WOLFSSL_SESSION_EXPORT */


/* prevent multiple mutex initializations */
static volatile int initRefCount = 0;
static wolfSSL_Mutex count_mutex;   /* init ref count mutex */

/* Create a new WOLFSSL_CTX struct and return the pointer to created struct.
   WOLFSSL_METHOD pointer passed in is given to ctx to manage.
   This function frees the passed in WOLFSSL_METHOD struct on failure and on
   success is freed when ctx is freed.
 */
WOLFSSL_CTX* wolfSSL_CTX_new_ex(WOLFSSL_METHOD* method, void* heap)
{
    WOLFSSL_CTX* ctx = NULL;

    WOLFSSL_ENTER("WOLFSSL_CTX_new_ex");

    if (initRefCount == 0) {
        /* user no longer forced to call Init themselves */
        int ret = wolfSSL_Init();
        if (ret != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("wolfSSL_Init failed");
            WOLFSSL_LEAVE("WOLFSSL_CTX_new", 0);
            if (method != NULL) {
                XFREE(method, heap, DYNAMIC_TYPE_METHOD);
            }
            return NULL;
        }
    }

    if (method == NULL)
        return ctx;

    ctx = (WOLFSSL_CTX*) XMALLOC(sizeof(WOLFSSL_CTX), heap, DYNAMIC_TYPE_CTX);
    if (ctx) {
        if (InitSSL_Ctx(ctx, method, heap) < 0) {
            WOLFSSL_MSG("Init CTX failed");
            wolfSSL_CTX_free(ctx);
            ctx = NULL;
        }
#if defined(OPENSSL_EXTRA) && defined(WOLFCRYPT_HAVE_SRP) \
                           && !defined(NO_SHA256) && !defined(WC_NO_RNG)
        else {
            ctx->srp = (Srp*) XMALLOC(sizeof(Srp), heap, DYNAMIC_TYPE_SRP);
            if (ctx->srp == NULL){
                WOLFSSL_MSG("Init CTX failed");
                wolfSSL_CTX_free(ctx);
                return NULL;
            }
            XMEMSET(ctx->srp, 0, sizeof(Srp));
        }
#endif
    }
    else {
        WOLFSSL_MSG("Alloc CTX failed, method freed");
        XFREE(method, heap, DYNAMIC_TYPE_METHOD);
    }


    WOLFSSL_LEAVE("WOLFSSL_CTX_new", 0);
    return ctx;
}


WOLFSSL_CTX* wolfSSL_CTX_new(WOLFSSL_METHOD* method)
{
#ifdef WOLFSSL_HEAP_TEST
    /* if testing the heap hint then set top level CTX to have test value */
    return wolfSSL_CTX_new_ex(method, (void*)WOLFSSL_HEAP_TEST);
#else
    return wolfSSL_CTX_new_ex(method, NULL);
#endif
}


void wolfSSL_CTX_free(WOLFSSL_CTX* ctx)
{
    WOLFSSL_ENTER("SSL_CTX_free");
    if (ctx) {
#if defined(OPENSSL_EXTRA) && defined(WOLFCRYPT_HAVE_SRP) \
&& !defined(NO_SHA256) && !defined(WC_NO_RNG)
        if (ctx->srp != NULL){
            if (ctx->srp_password != NULL){
                XFREE(ctx->srp_password, ctx->heap, DYNAMIC_TYPE_SRP);
            }
            wc_SrpTerm(ctx->srp);
            XFREE(ctx->srp, ctx->heap, DYNAMIC_TYPE_SRP);
        }
#endif
        FreeSSL_Ctx(ctx);
    }

    WOLFSSL_LEAVE("SSL_CTX_free", 0);
}


#ifdef SINGLE_THREADED
/* no locking in single threaded mode, allow a CTX level rng to be shared with
 * WOLFSSL objects, WOLFSSL_SUCCESS on ok */
int wolfSSL_CTX_new_rng(WOLFSSL_CTX* ctx)
{
    WC_RNG* rng;
    int     ret;

    if (ctx == NULL) {
        return BAD_FUNC_ARG;
    }

    rng = XMALLOC(sizeof(WC_RNG), ctx->heap, DYNAMIC_TYPE_RNG);
    if (rng == NULL) {
        return MEMORY_E;
    }

#ifndef HAVE_FIPS
    ret = wc_InitRng_ex(rng, ctx->heap, ctx->devId);
#else
    ret = wc_InitRng(rng);
#endif
    if (ret != 0) {
        XFREE(rng, ctx->heap, DYNAMIC_TYPE_RNG);
        return ret;
    }

    ctx->rng = rng;
    return WOLFSSL_SUCCESS;
}
#endif


WOLFSSL* wolfSSL_new(WOLFSSL_CTX* ctx)
{
    WOLFSSL* ssl = NULL;
    int ret = 0;

    (void)ret;
    WOLFSSL_ENTER("SSL_new");

    if (ctx == NULL)
        return ssl;

    ssl = (WOLFSSL*) XMALLOC(sizeof(WOLFSSL), ctx->heap, DYNAMIC_TYPE_SSL);
    if (ssl)
        if ( (ret = InitSSL(ssl, ctx, 0)) < 0) {
            FreeSSL(ssl, ctx->heap);
            ssl = 0;
        }

    WOLFSSL_LEAVE("SSL_new", ret);
    return ssl;
}


void wolfSSL_free(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("SSL_free");
    if (ssl)
        FreeSSL(ssl, ssl->ctx->heap);
    WOLFSSL_LEAVE("SSL_free", 0);
}


int wolfSSL_is_server(WOLFSSL* ssl)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;
    return ssl->options.side == WOLFSSL_SERVER_END;
}

#ifdef HAVE_WRITE_DUP

/*
 * Release resources around WriteDup object
 *
 * ssl WOLFSSL object
 *
 * no return, destruction so make best attempt
*/
void FreeWriteDup(WOLFSSL* ssl)
{
    int doFree = 0;

    WOLFSSL_ENTER("FreeWriteDup");

    if (ssl->dupWrite) {
        if (wc_LockMutex(&ssl->dupWrite->dupMutex) == 0) {
            ssl->dupWrite->dupCount--;
            if (ssl->dupWrite->dupCount == 0) {
                doFree = 1;
            } else {
                WOLFSSL_MSG("WriteDup count not zero, no full free");
            }
            wc_UnLockMutex(&ssl->dupWrite->dupMutex);
        }
    }

    if (doFree) {
        WOLFSSL_MSG("Doing WriteDup full free, count to zero");
        wc_FreeMutex(&ssl->dupWrite->dupMutex);
        XFREE(ssl->dupWrite, ssl->heap, DYNAMIC_TYPE_WRITEDUP);
    }
}


/*
 * duplicate existing ssl members into dup needed for writing
 *
 * dup write only WOLFSSL
 * ssl exisiting WOLFSSL
 *
 * 0 on success
*/
static int DupSSL(WOLFSSL* dup, WOLFSSL* ssl)
{
    /* shared dupWrite setup */
    ssl->dupWrite = (WriteDup*)XMALLOC(sizeof(WriteDup), ssl->heap,
                                       DYNAMIC_TYPE_WRITEDUP);
    if (ssl->dupWrite == NULL) {
        return MEMORY_E;
    }
    XMEMSET(ssl->dupWrite, 0, sizeof(WriteDup));

    if (wc_InitMutex(&ssl->dupWrite->dupMutex) != 0) {
        XFREE(ssl->dupWrite, ssl->heap, DYNAMIC_TYPE_WRITEDUP);
        ssl->dupWrite = NULL;
        return BAD_MUTEX_E;
    }
    ssl->dupWrite->dupCount = 2;    /* both sides have a count to start */
    dup->dupWrite = ssl->dupWrite; /* each side uses */

    /* copy write parts over to dup writer */
    XMEMCPY(&dup->specs,   &ssl->specs,   sizeof(CipherSpecs));
    XMEMCPY(&dup->options, &ssl->options, sizeof(Options));
    XMEMCPY(&dup->keys,    &ssl->keys,    sizeof(Keys));
    XMEMCPY(&dup->encrypt, &ssl->encrypt, sizeof(Ciphers));
    /* dup side now owns encrypt/write ciphers */
    XMEMSET(&ssl->encrypt, 0, sizeof(Ciphers));

    dup->IOCB_WriteCtx = ssl->IOCB_WriteCtx;
    dup->wfd    = ssl->wfd;
    dup->wflags = ssl->wflags;
    dup->hmac   = ssl->hmac;
#ifdef HAVE_TRUNCATED_HMAC
    dup->truncated_hmac = ssl->truncated_hmac;
#endif

    /* unique side dup setup */
    dup->dupSide = WRITE_DUP_SIDE;
    ssl->dupSide = READ_DUP_SIDE;

    return 0;
}


/*
 * duplicate a WOLFSSL object post handshake for writing only
 * turn exisitng object into read only.  Allows concurrent access from two
 * different threads.
 *
 * ssl exisiting WOLFSSL object
 *
 * return dup'd WOLFSSL object on success
*/
WOLFSSL* wolfSSL_write_dup(WOLFSSL* ssl)
{
    WOLFSSL* dup = NULL;
    int ret = 0;

    (void)ret;
    WOLFSSL_ENTER("wolfSSL_write_dup");

    if (ssl == NULL) {
        return ssl;
    }

    if (ssl->options.handShakeDone == 0) {
        WOLFSSL_MSG("wolfSSL_write_dup called before handshake complete");
        return NULL;
    }

    if (ssl->dupWrite) {
        WOLFSSL_MSG("wolfSSL_write_dup already called once");
        return NULL;
    }

    dup = (WOLFSSL*) XMALLOC(sizeof(WOLFSSL), ssl->ctx->heap, DYNAMIC_TYPE_SSL);
    if (dup) {
        if ( (ret = InitSSL(dup, ssl->ctx, 1)) < 0) {
            FreeSSL(dup, ssl->ctx->heap);
            dup = NULL;
        } else if ( (ret = DupSSL(dup, ssl) < 0)) {
            FreeSSL(dup, ssl->ctx->heap);
            dup = NULL;
        }
    }

    WOLFSSL_LEAVE("wolfSSL_write_dup", ret);

    return dup;
}


/*
 * Notify write dup side of fatal error or close notify
 *
 * ssl WOLFSSL object
 * err Notify err
 *
 * 0 on success
*/
int NotifyWriteSide(WOLFSSL* ssl, int err)
{
    int ret;

    WOLFSSL_ENTER("NotifyWriteSide");

    ret = wc_LockMutex(&ssl->dupWrite->dupMutex);
    if (ret == 0) {
        ssl->dupWrite->dupErr = err;
        ret = wc_UnLockMutex(&ssl->dupWrite->dupMutex);
    }

    return ret;
}


#endif /* HAVE_WRITE_DUP */


#ifdef HAVE_POLY1305
/* set if to use old poly 1 for yes 0 to use new poly */
int wolfSSL_use_old_poly(WOLFSSL* ssl, int value)
{
    (void)ssl;
    (void)value;

#ifndef WOLFSSL_NO_TLS12
    WOLFSSL_ENTER("SSL_use_old_poly");
    WOLFSSL_MSG("Warning SSL connection auto detects old/new and this function"
            "is depriciated");
    ssl->options.oldPoly = (word16)value;
    WOLFSSL_LEAVE("SSL_use_old_poly", 0);
#endif
    return 0;
}
#endif


int wolfSSL_set_fd(WOLFSSL* ssl, int fd)
{
    int ret;

    WOLFSSL_ENTER("SSL_set_fd");

    if (ssl == NULL) {
        return BAD_FUNC_ARG;
    }

    ret = wolfSSL_set_read_fd(ssl, fd);
    if (ret == WOLFSSL_SUCCESS) {
        ret = wolfSSL_set_write_fd(ssl, fd);
    }

    return ret;
}


int wolfSSL_set_read_fd(WOLFSSL* ssl, int fd)
{
    WOLFSSL_ENTER("SSL_set_read_fd");

    if (ssl == NULL) {
        return BAD_FUNC_ARG;
    }

    ssl->rfd = fd;      /* not used directly to allow IO callbacks */
    ssl->IOCB_ReadCtx  = &ssl->rfd;

    #ifdef WOLFSSL_DTLS
        if (ssl->options.dtls) {
            ssl->IOCB_ReadCtx = &ssl->buffers.dtlsCtx;
            ssl->buffers.dtlsCtx.rfd = fd;
        }
    #endif

    WOLFSSL_LEAVE("SSL_set_read_fd", WOLFSSL_SUCCESS);
    return WOLFSSL_SUCCESS;
}


int wolfSSL_set_write_fd(WOLFSSL* ssl, int fd)
{
    WOLFSSL_ENTER("SSL_set_write_fd");

    if (ssl == NULL) {
        return BAD_FUNC_ARG;
    }

    ssl->wfd = fd;      /* not used directly to allow IO callbacks */
    ssl->IOCB_WriteCtx  = &ssl->wfd;

    #ifdef WOLFSSL_DTLS
        if (ssl->options.dtls) {
            ssl->IOCB_WriteCtx = &ssl->buffers.dtlsCtx;
            ssl->buffers.dtlsCtx.wfd = fd;
        }
    #endif

    WOLFSSL_LEAVE("SSL_set_write_fd", WOLFSSL_SUCCESS);
    return WOLFSSL_SUCCESS;
}


/**
  * Get the name of cipher at priority level passed in.
  */
char* wolfSSL_get_cipher_list(int priority)
{
    const CipherSuiteInfo* ciphers = GetCipherNames();

    if (priority >= GetCipherNamesSize() || priority < 0) {
        return 0;
    }

    return (char*)ciphers[priority].name;
}


/**
  * Get the name of cipher at priority level passed in.
  */
char* wolfSSL_get_cipher_list_ex(WOLFSSL* ssl, int priority)
{

    if (ssl == NULL) {
        return NULL;
    }
    else {
        const char* cipher;

        if ((cipher = wolfSSL_get_cipher_name_internal(ssl)) != NULL) {
            if (priority == 0) {
                return (char*)cipher;
            }
            else {
                return NULL;
            }
        }
        else {
            return wolfSSL_get_cipher_list(priority);
        }
    }
}


int wolfSSL_get_ciphers(char* buf, int len)
{
    const CipherSuiteInfo* ciphers = GetCipherNames();
    int  totalInc = 0;
    int  step     = 0;
    char delim    = ':';
    int  size     = GetCipherNamesSize();
    int  i;

    if (buf == NULL || len <= 0)
        return BAD_FUNC_ARG;

    /* Add each member to the buffer delimited by a : */
    for (i = 0; i < size; i++) {
        step = (int)(XSTRLEN(ciphers[i].name) + 1);  /* delimiter */
        totalInc += step;

        /* Check to make sure buf is large enough and will not overflow */
        if (totalInc < len) {
            size_t cipherLen = XSTRLEN(ciphers[i].name);
            XSTRNCPY(buf, ciphers[i].name, cipherLen);
            buf += cipherLen;

            if (i < size - 1)
                *buf++ = delim;
            else
                *buf++ = '\0';
        }
        else
            return BUFFER_E;
    }
    return WOLFSSL_SUCCESS;
}

const char* wolfSSL_get_shared_ciphers(WOLFSSL* ssl, char* buf, int len)
{
    const char* cipher;

    if (ssl == NULL)
        return NULL;

    cipher = wolfSSL_get_cipher_name_iana(ssl);
    len = min(len, (int)(XSTRLEN(cipher) + 1));
    XMEMCPY(buf, cipher, len);
    return buf;
}

int wolfSSL_get_fd(const WOLFSSL* ssl)
{
    WOLFSSL_ENTER("SSL_get_fd");
    WOLFSSL_LEAVE("SSL_get_fd", ssl->rfd);
    return ssl->rfd;
}


int wolfSSL_dtls(WOLFSSL* ssl)
{
    return ssl->options.dtls;
}


#ifndef WOLFSSL_LEANPSK
int wolfSSL_dtls_set_peer(WOLFSSL* ssl, void* peer, unsigned int peerSz)
{
#ifdef WOLFSSL_DTLS
    void* sa = (void*)XMALLOC(peerSz, ssl->heap, DYNAMIC_TYPE_SOCKADDR);
    if (sa != NULL) {
        if (ssl->buffers.dtlsCtx.peer.sa != NULL)
            XFREE(ssl->buffers.dtlsCtx.peer.sa,ssl->heap,DYNAMIC_TYPE_SOCKADDR);
        XMEMCPY(sa, peer, peerSz);
        ssl->buffers.dtlsCtx.peer.sa = sa;
        ssl->buffers.dtlsCtx.peer.sz = peerSz;
        return WOLFSSL_SUCCESS;
    }
    return WOLFSSL_FAILURE;
#else
    (void)ssl;
    (void)peer;
    (void)peerSz;
    return WOLFSSL_NOT_IMPLEMENTED;
#endif
}

int wolfSSL_dtls_get_peer(WOLFSSL* ssl, void* peer, unsigned int* peerSz)
{
#ifdef WOLFSSL_DTLS
    if (ssl == NULL) {
        return WOLFSSL_FAILURE;
    }

    if (peer != NULL && peerSz != NULL
            && *peerSz >= ssl->buffers.dtlsCtx.peer.sz
            && ssl->buffers.dtlsCtx.peer.sa != NULL) {
        *peerSz = ssl->buffers.dtlsCtx.peer.sz;
        XMEMCPY(peer, ssl->buffers.dtlsCtx.peer.sa, *peerSz);
        return WOLFSSL_SUCCESS;
    }
    return WOLFSSL_FAILURE;
#else
    (void)ssl;
    (void)peer;
    (void)peerSz;
    return WOLFSSL_NOT_IMPLEMENTED;
#endif
}


#if defined(WOLFSSL_SCTP) && defined(WOLFSSL_DTLS)

int wolfSSL_CTX_dtls_set_sctp(WOLFSSL_CTX* ctx)
{
    WOLFSSL_ENTER("wolfSSL_CTX_dtls_set_sctp()");

    if (ctx == NULL)
        return BAD_FUNC_ARG;

    ctx->dtlsSctp = 1;
    return WOLFSSL_SUCCESS;
}


int wolfSSL_dtls_set_sctp(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("wolfSSL_dtls_set_sctp()");

    if (ssl == NULL)
        return BAD_FUNC_ARG;

    ssl->options.dtlsSctp = 1;
    return WOLFSSL_SUCCESS;
}


int wolfSSL_CTX_dtls_set_mtu(WOLFSSL_CTX* ctx, word16 newMtu)
{
    if (ctx == NULL || newMtu > MAX_RECORD_SIZE)
        return BAD_FUNC_ARG;

    ctx->dtlsMtuSz = newMtu;
    return WOLFSSL_SUCCESS;
}


int wolfSSL_dtls_set_mtu(WOLFSSL* ssl, word16 newMtu)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    if (newMtu > MAX_RECORD_SIZE) {
        ssl->error = BAD_FUNC_ARG;
        return WOLFSSL_FAILURE;
    }

    ssl->dtlsMtuSz = newMtu;
    return WOLFSSL_SUCCESS;
}


#endif /* WOLFSSL_DTLS && WOLFSSL_SCTP */


#ifdef WOLFSSL_DTLS_DROP_STATS

int wolfSSL_dtls_get_drop_stats(WOLFSSL* ssl,
                                word32* macDropCount, word32* replayDropCount)
{
    int ret;

    WOLFSSL_ENTER("wolfSSL_dtls_get_drop_stats()");

    if (ssl == NULL)
        ret = BAD_FUNC_ARG;
    else {
        ret = WOLFSSL_SUCCESS;
        if (macDropCount != NULL)
            *macDropCount = ssl->macDropCount;
        if (replayDropCount != NULL)
            *replayDropCount = ssl->replayDropCount;
    }

    WOLFSSL_LEAVE("wolfSSL_dtls_get_drop_stats()", ret);
    return ret;
}

#endif /* WOLFSSL_DTLS_DROP_STATS */


#if defined(WOLFSSL_MULTICAST)

int wolfSSL_CTX_mcast_set_member_id(WOLFSSL_CTX* ctx, word16 id)
{
    int ret = 0;

    WOLFSSL_ENTER("wolfSSL_CTX_mcast_set_member_id()");

    if (ctx == NULL || id > 255)
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        ctx->haveEMS = 0;
        ctx->haveMcast = 1;
        ctx->mcastID = id;
#ifndef WOLFSSL_USER_IO
        ctx->CBIORecv = EmbedReceiveFromMcast;
#endif /* WOLFSSL_USER_IO */
    }

    if (ret == 0)
        ret = WOLFSSL_SUCCESS;
    WOLFSSL_LEAVE("wolfSSL_CTX_mcast_set_member_id()", ret);
    return ret;
}

int wolfSSL_mcast_get_max_peers(void)
{
    return WOLFSSL_MULTICAST_PEERS;
}

#ifdef WOLFSSL_DTLS
static INLINE word32 UpdateHighwaterMark(word32 cur, word32 first,
                                         word32 second, word32 max)
{
    word32 newCur = 0;

    if (cur < first)
        newCur = first;
    else if (cur < second)
        newCur = second;
    else if (cur < max)
        newCur = max;

    return newCur;
}
#endif /* WOLFSSL_DTLS */


int wolfSSL_set_secret(WOLFSSL* ssl, word16 epoch,
                       const byte* preMasterSecret, word32 preMasterSz,
                       const byte* clientRandom, const byte* serverRandom,
                       const byte* suite)
{
    int ret = 0;

    WOLFSSL_ENTER("wolfSSL_set_secret()");

    if (ssl == NULL || preMasterSecret == NULL ||
        preMasterSz == 0 || preMasterSz > ENCRYPT_LEN ||
        clientRandom == NULL || serverRandom == NULL || suite == NULL) {

        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        XMEMCPY(ssl->arrays->preMasterSecret, preMasterSecret, preMasterSz);
        ssl->arrays->preMasterSz = preMasterSz;
        XMEMCPY(ssl->arrays->clientRandom, clientRandom, RAN_LEN);
        XMEMCPY(ssl->arrays->serverRandom, serverRandom, RAN_LEN);
        ssl->options.cipherSuite0 = suite[0];
        ssl->options.cipherSuite = suite[1];

        ret = SetCipherSpecs(ssl);
    }

    if (ret == 0)
        ret = MakeTlsMasterSecret(ssl);

    if (ret == 0) {
        ssl->keys.encryptionOn = 1;
        ret = SetKeysSide(ssl, ENCRYPT_AND_DECRYPT_SIDE);
    }

    if (ret == 0) {
        if (ssl->options.dtls) {
        #ifdef WOLFSSL_DTLS
            WOLFSSL_DTLS_PEERSEQ* peerSeq;
            int i;

            ssl->keys.dtls_epoch = epoch;
            for (i = 0, peerSeq = ssl->keys.peerSeq;
                 i < WOLFSSL_DTLS_PEERSEQ_SZ;
                 i++, peerSeq++) {

                peerSeq->nextEpoch = epoch;
                peerSeq->prevSeq_lo = peerSeq->nextSeq_lo;
                peerSeq->prevSeq_hi = peerSeq->nextSeq_hi;
                peerSeq->nextSeq_lo = 0;
                peerSeq->nextSeq_hi = 0;
                XMEMCPY(peerSeq->prevWindow, peerSeq->window, DTLS_SEQ_SZ);
                XMEMSET(peerSeq->window, 0, DTLS_SEQ_SZ);
                peerSeq->highwaterMark = UpdateHighwaterMark(0,
                        ssl->ctx->mcastFirstSeq,
                        ssl->ctx->mcastSecondSeq,
                        ssl->ctx->mcastMaxSeq);
            }
        #else
            (void)epoch;
        #endif
        }
        FreeHandshakeResources(ssl);
        ret = WOLFSSL_SUCCESS;
    }
    else {
        if (ssl)
            ssl->error = ret;
        ret = WOLFSSL_FATAL_ERROR;
    }
    WOLFSSL_LEAVE("wolfSSL_set_secret()", ret);
    return ret;
}


#ifdef WOLFSSL_DTLS

int wolfSSL_mcast_peer_add(WOLFSSL* ssl, word16 peerId, int remove)
{
    WOLFSSL_DTLS_PEERSEQ* p = NULL;
    int ret = WOLFSSL_SUCCESS;
    int i;

    WOLFSSL_ENTER("wolfSSL_mcast_peer_add()");
    if (ssl == NULL || peerId > 255)
        return BAD_FUNC_ARG;

    if (!remove) {
        /* Make sure it isn't already present, while keeping the first
         * open spot. */
        for (i = 0; i < WOLFSSL_DTLS_PEERSEQ_SZ; i++) {
            if (ssl->keys.peerSeq[i].peerId == INVALID_PEER_ID)
                p = &ssl->keys.peerSeq[i];
            if (ssl->keys.peerSeq[i].peerId == peerId) {
                WOLFSSL_MSG("Peer ID already in multicast peer list.");
                p = NULL;
            }
        }

        if (p != NULL) {
            XMEMSET(p, 0, sizeof(WOLFSSL_DTLS_PEERSEQ));
            p->peerId = peerId;
            p->highwaterMark = UpdateHighwaterMark(0,
                ssl->ctx->mcastFirstSeq,
                ssl->ctx->mcastSecondSeq,
                ssl->ctx->mcastMaxSeq);
        }
        else {
            WOLFSSL_MSG("No room in peer list.");
            ret = -1;
        }
    }
    else {
        for (i = 0; i < WOLFSSL_DTLS_PEERSEQ_SZ; i++) {
            if (ssl->keys.peerSeq[i].peerId == peerId)
                p = &ssl->keys.peerSeq[i];
        }

        if (p != NULL) {
            p->peerId = INVALID_PEER_ID;
        }
        else {
            WOLFSSL_MSG("Peer not found in list.");
        }
    }

    WOLFSSL_LEAVE("wolfSSL_mcast_peer_add()", ret);
    return ret;
}


/* If peerId is in the list of peers and its last sequence number is non-zero,
 * return 1, otherwise return 0. */
int wolfSSL_mcast_peer_known(WOLFSSL* ssl, unsigned short peerId)
{
    int known = 0;
    int i;

    WOLFSSL_ENTER("wolfSSL_mcast_peer_known()");

    if (ssl == NULL || peerId > 255) {
        return BAD_FUNC_ARG;
    }

    for (i = 0; i < WOLFSSL_DTLS_PEERSEQ_SZ; i++) {
        if (ssl->keys.peerSeq[i].peerId == peerId) {
            if (ssl->keys.peerSeq[i].nextSeq_hi ||
                ssl->keys.peerSeq[i].nextSeq_lo) {

                known = 1;
            }
            break;
        }
    }

    WOLFSSL_LEAVE("wolfSSL_mcast_peer_known()", known);
    return known;
}


int wolfSSL_CTX_mcast_set_highwater_cb(WOLFSSL_CTX* ctx, word32 maxSeq,
                                       word32 first, word32 second,
                                       CallbackMcastHighwater cb)
{
    if (ctx == NULL || (second && first > second) ||
        first > maxSeq || second > maxSeq || cb == NULL) {

        return BAD_FUNC_ARG;
    }

    ctx->mcastHwCb = cb;
    ctx->mcastFirstSeq = first;
    ctx->mcastSecondSeq = second;
    ctx->mcastMaxSeq = maxSeq;

    return WOLFSSL_SUCCESS;
}


int wolfSSL_mcast_set_highwater_ctx(WOLFSSL* ssl, void* ctx)
{
    if (ssl == NULL || ctx == NULL)
        return BAD_FUNC_ARG;

    ssl->mcastHwCbCtx = ctx;

    return WOLFSSL_SUCCESS;
}

#endif /* WOLFSSL_DTLS */

#endif /* WOLFSSL_MULTICAST */


#endif /* WOLFSSL_LEANPSK */


/* return underlying connect or accept, WOLFSSL_SUCCESS on ok */
int wolfSSL_negotiate(WOLFSSL* ssl)
{
    int err = WOLFSSL_FATAL_ERROR;

    WOLFSSL_ENTER("wolfSSL_negotiate");
#ifndef NO_WOLFSSL_SERVER
    if (ssl->options.side == WOLFSSL_SERVER_END) {
#ifdef WOLFSSL_TLS13
        if (IsAtLeastTLSv1_3(ssl->version))
            err = wolfSSL_accept_TLSv13(ssl);
        else
#endif
            err = wolfSSL_accept(ssl);
    }
#endif

#ifndef NO_WOLFSSL_CLIENT
    if (ssl->options.side == WOLFSSL_CLIENT_END) {
#ifdef WOLFSSL_TLS13
        if (IsAtLeastTLSv1_3(ssl->version))
            err = wolfSSL_connect_TLSv13(ssl);
        else
#endif
            err = wolfSSL_connect(ssl);
    }
#endif

    (void)ssl;

    WOLFSSL_LEAVE("wolfSSL_negotiate", err);

    return err;
}


WC_RNG* wolfSSL_GetRNG(WOLFSSL* ssl)
{
    if (ssl) {
        return ssl->rng;
    }

    return NULL;
}


#ifndef WOLFSSL_LEANPSK
/* object size based on build */
int wolfSSL_GetObjectSize(void)
{
#ifdef SHOW_SIZES
    printf("sizeof suites           = %lu\n", sizeof(Suites));
    printf("sizeof ciphers(2)       = %lu\n", sizeof(Ciphers));
#ifndef NO_RC4
    printf("\tsizeof arc4         = %lu\n", sizeof(Arc4));
#endif
    printf("\tsizeof aes          = %lu\n", sizeof(Aes));
#ifndef NO_DES3
    printf("\tsizeof des3         = %lu\n", sizeof(Des3));
#endif
#ifndef NO_RABBIT
    printf("\tsizeof rabbit       = %lu\n", sizeof(Rabbit));
#endif
#ifdef HAVE_CHACHA
    printf("\tsizeof chacha       = %lu\n", sizeof(ChaCha));
#endif
    printf("sizeof cipher specs     = %lu\n", sizeof(CipherSpecs));
    printf("sizeof keys             = %lu\n", sizeof(Keys));
    printf("sizeof Hashes(2)        = %lu\n", sizeof(Hashes));
#ifndef NO_MD5
    printf("\tsizeof MD5          = %lu\n", sizeof(wc_Md5));
#endif
#ifndef NO_SHA
    printf("\tsizeof SHA          = %lu\n", sizeof(wc_Sha));
#endif
#ifdef WOLFSSL_SHA224
    printf("\tsizeof SHA224       = %lu\n", sizeof(wc_Sha224));
#endif
#ifndef NO_SHA256
    printf("\tsizeof SHA256       = %lu\n", sizeof(wc_Sha256));
#endif
#ifdef WOLFSSL_SHA384
    printf("\tsizeof SHA384       = %lu\n", sizeof(wc_Sha384));
#endif
#ifdef WOLFSSL_SHA384
    printf("\tsizeof SHA512       = %lu\n", sizeof(wc_Sha512));
#endif
    printf("sizeof Buffers          = %lu\n", sizeof(Buffers));
    printf("sizeof Options          = %lu\n", sizeof(Options));
    printf("sizeof Arrays           = %lu\n", sizeof(Arrays));
#ifndef NO_RSA
    printf("sizeof RsaKey           = %lu\n", sizeof(RsaKey));
#endif
#ifdef HAVE_ECC
    printf("sizeof ecc_key          = %lu\n", sizeof(ecc_key));
#endif
    printf("sizeof WOLFSSL_CIPHER    = %lu\n", sizeof(WOLFSSL_CIPHER));
    printf("sizeof WOLFSSL_SESSION   = %lu\n", sizeof(WOLFSSL_SESSION));
    printf("sizeof WOLFSSL           = %lu\n", sizeof(WOLFSSL));
    printf("sizeof WOLFSSL_CTX       = %lu\n", sizeof(WOLFSSL_CTX));
#endif

    return sizeof(WOLFSSL);
}

int wolfSSL_CTX_GetObjectSize(void)
{
    return sizeof(WOLFSSL_CTX);
}

int wolfSSL_METHOD_GetObjectSize(void)
{
    return sizeof(WOLFSSL_METHOD);
}
#endif


#ifdef WOLFSSL_STATIC_MEMORY

int wolfSSL_CTX_load_static_memory(WOLFSSL_CTX** ctx, wolfSSL_method_func method,
                                   unsigned char* buf, unsigned int sz,
                                   int flag, int max)
{
    WOLFSSL_HEAP*      heap;
    WOLFSSL_HEAP_HINT* hint;
    word32 idx = 0;

    if (ctx == NULL || buf == NULL) {
        return BAD_FUNC_ARG;
    }

    if (*ctx == NULL && method == NULL) {
        return BAD_FUNC_ARG;
    }

    if (*ctx == NULL || (*ctx)->heap == NULL) {
        if (sizeof(WOLFSSL_HEAP) + sizeof(WOLFSSL_HEAP_HINT) > sz - idx) {
            return BUFFER_E; /* not enough memory for structures */
        }
        heap = (WOLFSSL_HEAP*)buf;
        idx += sizeof(WOLFSSL_HEAP);
        if (wolfSSL_init_memory_heap(heap) != 0) {
            return WOLFSSL_FAILURE;
        }
        hint = (WOLFSSL_HEAP_HINT*)(buf + idx);
        idx += sizeof(WOLFSSL_HEAP_HINT);
        XMEMSET(hint, 0, sizeof(WOLFSSL_HEAP_HINT));
        hint->memory = heap;

        if (*ctx && (*ctx)->heap == NULL) {
            (*ctx)->heap = (void*)hint;
        }
    }
    else {
#ifdef WOLFSSL_HEAP_TEST
        /* do not load in memory if test has been set */
        if ((*ctx)->heap == (void*)WOLFSSL_HEAP_TEST) {
            return WOLFSSL_SUCCESS;
        }
#endif
        hint = (WOLFSSL_HEAP_HINT*)((*ctx)->heap);
        heap = hint->memory;
    }

    if (wolfSSL_load_static_memory(buf + idx, sz - idx, flag, heap) != 1) {
        WOLFSSL_MSG("Error partitioning memory");
        return WOLFSSL_FAILURE;
    }

    /* create ctx if needed */
    if (*ctx == NULL) {
        *ctx = wolfSSL_CTX_new_ex(method(hint), hint);
        if (*ctx == NULL) {
            WOLFSSL_MSG("Error creating ctx");
            return WOLFSSL_FAILURE;
        }
    }

    /* determine what max applies too */
    if (flag & WOLFMEM_IO_POOL || flag & WOLFMEM_IO_POOL_FIXED) {
        heap->maxIO = max;
    }
    else { /* general memory used in handshakes */
        heap->maxHa = max;
    }

    heap->flag |= flag;

    (void)max;
    (void)method;

    return WOLFSSL_SUCCESS;
}


int wolfSSL_is_static_memory(WOLFSSL* ssl, WOLFSSL_MEM_CONN_STATS* mem_stats)
{
    if (ssl == NULL) {
        return BAD_FUNC_ARG;
    }
    WOLFSSL_ENTER("wolfSSL_is_static_memory");

    /* fill out statistics if wanted and WOLFMEM_TRACK_STATS flag */
    if (mem_stats != NULL && ssl->heap != NULL) {
        WOLFSSL_HEAP_HINT* hint = ((WOLFSSL_HEAP_HINT*)(ssl->heap));
        WOLFSSL_HEAP* heap      = hint->memory;
        if (heap->flag & WOLFMEM_TRACK_STATS && hint->stats != NULL) {
            XMEMCPY(mem_stats, hint->stats, sizeof(WOLFSSL_MEM_CONN_STATS));
        }
    }

    return (ssl->heap) ? 1 : 0;
}


int wolfSSL_CTX_is_static_memory(WOLFSSL_CTX* ctx, WOLFSSL_MEM_STATS* mem_stats)
{
    if (ctx == NULL) {
        return BAD_FUNC_ARG;
    }
    WOLFSSL_ENTER("wolfSSL_CTX_is_static_memory");

    /* fill out statistics if wanted */
    if (mem_stats != NULL && ctx->heap != NULL) {
        WOLFSSL_HEAP* heap = ((WOLFSSL_HEAP_HINT*)(ctx->heap))->memory;
        if (wolfSSL_GetMemStats(heap, mem_stats) != 1) {
            return MEMORY_E;
        }
    }

    return (ctx->heap) ? 1 : 0;
}

#endif /* WOLFSSL_STATIC_MEMORY */


/* return max record layer size plaintext input size */
int wolfSSL_GetMaxOutputSize(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("wolfSSL_GetMaxOutputSize");

    if (ssl == NULL)
        return BAD_FUNC_ARG;

    if (ssl->options.handShakeState != HANDSHAKE_DONE) {
        WOLFSSL_MSG("Handshake not complete yet");
        return BAD_FUNC_ARG;
    }

    return wolfSSL_GetMaxRecordSize(ssl, OUTPUT_RECORD_SIZE);
}


/* return record layer size of plaintext input size */
int wolfSSL_GetOutputSize(WOLFSSL* ssl, int inSz)
{
    int maxSize;

    WOLFSSL_ENTER("wolfSSL_GetOutputSize");

    if (inSz < 0)
        return BAD_FUNC_ARG;

    maxSize = wolfSSL_GetMaxOutputSize(ssl);
    if (maxSize < 0)
        return maxSize;   /* error */
    if (inSz > maxSize)
        return INPUT_SIZE_E;

    return BuildMessage(ssl, NULL, 0, NULL, inSz, application_data, 0, 1, 0);
}


#ifdef HAVE_ECC
int wolfSSL_CTX_SetMinEccKey_Sz(WOLFSSL_CTX* ctx, short keySz)
{
    if (ctx == NULL || keySz < 0 || keySz % 8 != 0) {
        WOLFSSL_MSG("Key size must be divisable by 8 or ctx was null");
        return BAD_FUNC_ARG;
    }

    ctx->minEccKeySz     = keySz / 8;
#ifndef NO_CERTS
    ctx->cm->minEccKeySz = keySz / 8;
#endif
    return WOLFSSL_SUCCESS;
}


int wolfSSL_SetMinEccKey_Sz(WOLFSSL* ssl, short keySz)
{
    if (ssl == NULL || keySz < 0 || keySz % 8 != 0) {
        WOLFSSL_MSG("Key size must be divisable by 8 or ssl was null");
        return BAD_FUNC_ARG;
    }

    ssl->options.minEccKeySz = keySz / 8;
    return WOLFSSL_SUCCESS;
}

#endif /* !NO_RSA */

#ifndef NO_RSA
int wolfSSL_CTX_SetMinRsaKey_Sz(WOLFSSL_CTX* ctx, short keySz)
{
    if (ctx == NULL || keySz < 0 || keySz % 8 != 0) {
        WOLFSSL_MSG("Key size must be divisable by 8 or ctx was null");
        return BAD_FUNC_ARG;
    }

    ctx->minRsaKeySz     = keySz / 8;
    ctx->cm->minRsaKeySz = keySz / 8;
    return WOLFSSL_SUCCESS;
}


int wolfSSL_SetMinRsaKey_Sz(WOLFSSL* ssl, short keySz)
{
    if (ssl == NULL || keySz < 0 || keySz % 8 != 0) {
        WOLFSSL_MSG("Key size must be divisable by 8 or ssl was null");
        return BAD_FUNC_ARG;
    }

    ssl->options.minRsaKeySz = keySz / 8;
    return WOLFSSL_SUCCESS;
}
#endif /* !NO_RSA */

#ifndef NO_DH
/* server Diffie-Hellman parameters, WOLFSSL_SUCCESS on ok */
int wolfSSL_SetTmpDH(WOLFSSL* ssl, const unsigned char* p, int pSz,
                    const unsigned char* g, int gSz)
{
    word16 havePSK = 0;
    word16 haveRSA = 1;
    int    keySz   = 0;

    WOLFSSL_ENTER("wolfSSL_SetTmpDH");
    if (ssl == NULL || p == NULL || g == NULL) return BAD_FUNC_ARG;

    if (pSz < ssl->options.minDhKeySz)
        return DH_KEY_SIZE_E;

    if (ssl->options.side != WOLFSSL_SERVER_END)
        return SIDE_ERROR;

    if (ssl->buffers.serverDH_P.buffer && ssl->buffers.weOwnDH) {
        XFREE(ssl->buffers.serverDH_P.buffer, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
        ssl->buffers.serverDH_P.buffer = NULL;
    }
    if (ssl->buffers.serverDH_G.buffer && ssl->buffers.weOwnDH) {
        XFREE(ssl->buffers.serverDH_G.buffer, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
        ssl->buffers.serverDH_G.buffer = NULL;
    }

    ssl->buffers.weOwnDH = 1;  /* SSL owns now */
    ssl->buffers.serverDH_P.buffer = (byte*)XMALLOC(pSz, ssl->heap,
                                                    DYNAMIC_TYPE_PUBLIC_KEY);
    if (ssl->buffers.serverDH_P.buffer == NULL)
            return MEMORY_E;

    ssl->buffers.serverDH_G.buffer = (byte*)XMALLOC(gSz, ssl->heap,
                                                    DYNAMIC_TYPE_PUBLIC_KEY);
    if (ssl->buffers.serverDH_G.buffer == NULL) {
        XFREE(ssl->buffers.serverDH_P.buffer, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
        ssl->buffers.serverDH_P.buffer = NULL;
        return MEMORY_E;
    }

    ssl->buffers.serverDH_P.length = pSz;
    ssl->buffers.serverDH_G.length = gSz;

    XMEMCPY(ssl->buffers.serverDH_P.buffer, p, pSz);
    XMEMCPY(ssl->buffers.serverDH_G.buffer, g, gSz);

    ssl->options.haveDH = 1;
    #ifndef NO_PSK
        havePSK = ssl->options.havePSK;
    #endif
    #ifdef NO_RSA
        haveRSA = 0;
    #endif
    #ifndef NO_CERTS
        keySz = ssl->buffers.keySz;
    #endif
    InitSuites(ssl->suites, ssl->version, keySz, haveRSA, havePSK,
               ssl->options.haveDH, ssl->options.haveNTRU,
               ssl->options.haveECDSAsig, ssl->options.haveECC,
               ssl->options.haveStaticECC, ssl->options.side);

    WOLFSSL_LEAVE("wolfSSL_SetTmpDH", 0);
    return WOLFSSL_SUCCESS;
}

/* server ctx Diffie-Hellman parameters, WOLFSSL_SUCCESS on ok */
int wolfSSL_CTX_SetTmpDH(WOLFSSL_CTX* ctx, const unsigned char* p, int pSz,
                         const unsigned char* g, int gSz)
{
    WOLFSSL_ENTER("wolfSSL_CTX_SetTmpDH");
    if (ctx == NULL || p == NULL || g == NULL) return BAD_FUNC_ARG;

    if (pSz < ctx->minDhKeySz)
        return DH_KEY_SIZE_E;

    XFREE(ctx->serverDH_P.buffer, ctx->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    XFREE(ctx->serverDH_G.buffer, ctx->heap, DYNAMIC_TYPE_PUBLIC_KEY);

    ctx->serverDH_P.buffer = (byte*)XMALLOC(pSz, ctx->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    if (ctx->serverDH_P.buffer == NULL)
       return MEMORY_E;

    ctx->serverDH_G.buffer = (byte*)XMALLOC(gSz, ctx->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    if (ctx->serverDH_G.buffer == NULL) {
        XFREE(ctx->serverDH_P.buffer, ctx->heap, DYNAMIC_TYPE_PUBLIC_KEY);
        return MEMORY_E;
    }

    ctx->serverDH_P.length = pSz;
    ctx->serverDH_G.length = gSz;

    XMEMCPY(ctx->serverDH_P.buffer, p, pSz);
    XMEMCPY(ctx->serverDH_G.buffer, g, gSz);

    ctx->haveDH = 1;

    WOLFSSL_LEAVE("wolfSSL_CTX_SetTmpDH", 0);
    return WOLFSSL_SUCCESS;
}


int wolfSSL_CTX_SetMinDhKey_Sz(WOLFSSL_CTX* ctx, word16 keySz)
{
    if (ctx == NULL || keySz > 16000 || keySz % 8 != 0)
        return BAD_FUNC_ARG;

    ctx->minDhKeySz = keySz / 8;
    return WOLFSSL_SUCCESS;
}


int wolfSSL_SetMinDhKey_Sz(WOLFSSL* ssl, word16 keySz)
{
    if (ssl == NULL || keySz > 16000 || keySz % 8 != 0)
        return BAD_FUNC_ARG;

    ssl->options.minDhKeySz = keySz / 8;
    return WOLFSSL_SUCCESS;
}


int wolfSSL_GetDhKey_Sz(WOLFSSL* ssl)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    return (ssl->options.dhKeySz * 8);
}

#endif /* !NO_DH */


int wolfSSL_write(WOLFSSL* ssl, const void* data, int sz)
{
    int ret;

    WOLFSSL_ENTER("SSL_write()");

    if (ssl == NULL || data == NULL || sz < 0)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_EARLY_DATA
    if (ssl->earlyData != no_early_data && (ret = wolfSSL_negotiate(ssl)) < 0) {
        ssl->error = ret;
        return WOLFSSL_FATAL_ERROR;
    }
    ssl->earlyData = no_early_data;
#endif

#ifdef HAVE_WRITE_DUP
    { /* local variable scope */
        int dupErr = 0;   /* local copy */

        ret = 0;

        if (ssl->dupWrite && ssl->dupSide == READ_DUP_SIDE) {
            WOLFSSL_MSG("Read dup side cannot write");
            return WRITE_DUP_WRITE_E;
        }
        if (ssl->dupWrite) {
            if (wc_LockMutex(&ssl->dupWrite->dupMutex) != 0) {
                return BAD_MUTEX_E;
            }
            dupErr = ssl->dupWrite->dupErr;
            ret = wc_UnLockMutex(&ssl->dupWrite->dupMutex);
        }

        if (ret != 0) {
            ssl->error = ret;  /* high priority fatal error */
            return WOLFSSL_FATAL_ERROR;
        }
        if (dupErr != 0) {
            WOLFSSL_MSG("Write dup error from other side");
            ssl->error = dupErr;
            return WOLFSSL_FATAL_ERROR;
        }
    }
#endif

#ifdef HAVE_ERRNO_H
    errno = 0;
#endif

    #ifdef OPENSSL_EXTRA
    if (ssl->CBIS != NULL) {
        ssl->CBIS(ssl, SSL_CB_WRITE, SSL_SUCCESS);
        ssl->cbmode = SSL_CB_WRITE;
    }
    #endif
    ret = SendData(ssl, data, sz);

    WOLFSSL_LEAVE("SSL_write()", ret);

    if (ret < 0)
        return WOLFSSL_FATAL_ERROR;
    else
        return ret;
}

static int wolfSSL_read_internal(WOLFSSL* ssl, void* data, int sz, int peek)
{
    int ret;

    WOLFSSL_ENTER("wolfSSL_read_internal()");

    if (ssl == NULL || data == NULL || sz < 0)
        return BAD_FUNC_ARG;

#ifdef HAVE_WRITE_DUP
    if (ssl->dupWrite && ssl->dupSide == WRITE_DUP_SIDE) {
        WOLFSSL_MSG("Write dup side cannot read");
        return WRITE_DUP_READ_E;
    }
#endif

#ifdef HAVE_ERRNO_H
        errno = 0;
#endif

#ifdef WOLFSSL_DTLS
    if (ssl->options.dtls) {
        ssl->dtls_expected_rx = max(sz + 100, MAX_MTU);
#ifdef WOLFSSL_SCTP
        if (ssl->options.dtlsSctp)
            ssl->dtls_expected_rx = max(ssl->dtls_expected_rx, ssl->dtlsMtuSz);
#endif
    }
#endif

    sz = wolfSSL_GetMaxRecordSize(ssl, sz);

    ret = ReceiveData(ssl, (byte*)data, sz, peek);

#ifdef HAVE_WRITE_DUP
    if (ssl->dupWrite) {
        if (ssl->error != 0 && ssl->error != WANT_READ
        #ifdef WOLFSSL_ASYNC_CRYPT
            && ssl->error != WC_PENDING_E
        #endif
        ) {
            int notifyErr;

            WOLFSSL_MSG("Notifying write side of fatal read error");
            notifyErr  = NotifyWriteSide(ssl, ssl->error);
            if (notifyErr < 0) {
                ret = ssl->error = notifyErr;
            }
        }
    }
#endif

    WOLFSSL_LEAVE("wolfSSL_read_internal()", ret);

    if (ret < 0)
        return WOLFSSL_FATAL_ERROR;
    else
        return ret;
}


int wolfSSL_peek(WOLFSSL* ssl, void* data, int sz)
{
    WOLFSSL_ENTER("wolfSSL_peek()");

    return wolfSSL_read_internal(ssl, data, sz, TRUE);
}


int wolfSSL_read(WOLFSSL* ssl, void* data, int sz)
{
    WOLFSSL_ENTER("wolfSSL_read()");

    #ifdef OPENSSL_EXTRA
    if (ssl->CBIS != NULL) {
        ssl->CBIS(ssl, SSL_CB_READ, SSL_SUCCESS);
        ssl->cbmode = SSL_CB_READ;
    }
    #endif
    return wolfSSL_read_internal(ssl, data, sz, FALSE);
}


#ifdef WOLFSSL_MULTICAST

int wolfSSL_mcast_read(WOLFSSL* ssl, word16* id, void* data, int sz)
{
    int ret = 0;

    WOLFSSL_ENTER("wolfSSL_mcast_read()");

    if (ssl == NULL)
        return BAD_FUNC_ARG;

    ret = wolfSSL_read_internal(ssl, data, sz, FALSE);
    if (ssl->options.dtls && ssl->options.haveMcast && id != NULL)
        *id = ssl->keys.curPeerId;
    return ret;
}

#endif /* WOLFSSL_MULTICAST */


/* helpers to set the device id, WOLFSSL_SUCCESS on ok */
int wolfSSL_SetDevId(WOLFSSL* ssl, int devId)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    ssl->devId = devId;

    return WOLFSSL_SUCCESS;
}
int wolfSSL_CTX_SetDevId(WOLFSSL_CTX* ctx, int devId)
{
    if (ctx == NULL)
        return BAD_FUNC_ARG;

    ctx->devId = devId;

    return WOLFSSL_SUCCESS;
}

/* helpers to get device id and heap */
int wolfSSL_CTX_GetDevId(WOLFSSL_CTX* ctx, WOLFSSL* ssl)
{
    int devId = INVALID_DEVID;
    if (ctx != NULL)
        devId = ctx->devId;
    else if (ssl != NULL)
        devId = ssl->devId;
    return devId;
}
void* wolfSSL_CTX_GetHeap(WOLFSSL_CTX* ctx, WOLFSSL* ssl)
{
    void* heap = NULL;
    if (ctx != NULL)
        heap = ctx->heap;
    else if (ssl != NULL)
        heap = ssl->heap;
    return heap;
}


#ifdef HAVE_SNI

int wolfSSL_UseSNI(WOLFSSL* ssl, byte type, const void* data, word16 size)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    return TLSX_UseSNI(&ssl->extensions, type, data, size, ssl->heap);
}


int wolfSSL_CTX_UseSNI(WOLFSSL_CTX* ctx, byte type, const void* data,
                                                                    word16 size)
{
    if (ctx == NULL)
        return BAD_FUNC_ARG;

    return TLSX_UseSNI(&ctx->extensions, type, data, size, ctx->heap);
}

#ifndef NO_WOLFSSL_SERVER

void wolfSSL_SNI_SetOptions(WOLFSSL* ssl, byte type, byte options)
{
    if (ssl && ssl->extensions)
        TLSX_SNI_SetOptions(ssl->extensions, type, options);
}


void wolfSSL_CTX_SNI_SetOptions(WOLFSSL_CTX* ctx, byte type, byte options)
{
    if (ctx && ctx->extensions)
        TLSX_SNI_SetOptions(ctx->extensions, type, options);
}


byte wolfSSL_SNI_Status(WOLFSSL* ssl, byte type)
{
    return TLSX_SNI_Status(ssl ? ssl->extensions : NULL, type);
}


word16 wolfSSL_SNI_GetRequest(WOLFSSL* ssl, byte type, void** data)
{
    if (data)
        *data = NULL;

    if (ssl && ssl->extensions)
        return TLSX_SNI_GetRequest(ssl->extensions, type, data);

    return 0;
}


int wolfSSL_SNI_GetFromBuffer(const byte* clientHello, word32 helloSz,
                              byte type, byte* sni, word32* inOutSz)
{
    if (clientHello && helloSz > 0 && sni && inOutSz && *inOutSz > 0)
        return TLSX_SNI_GetFromBuffer(clientHello, helloSz, type, sni, inOutSz);

    return BAD_FUNC_ARG;
}

#endif /* NO_WOLFSSL_SERVER */

#endif /* HAVE_SNI */


#ifdef HAVE_MAX_FRAGMENT
#ifndef NO_WOLFSSL_CLIENT

int wolfSSL_UseMaxFragment(WOLFSSL* ssl, byte mfl)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    return TLSX_UseMaxFragment(&ssl->extensions, mfl, ssl->heap);
}


int wolfSSL_CTX_UseMaxFragment(WOLFSSL_CTX* ctx, byte mfl)
{
    if (ctx == NULL)
        return BAD_FUNC_ARG;

    return TLSX_UseMaxFragment(&ctx->extensions, mfl, ctx->heap);
}

#endif /* NO_WOLFSSL_CLIENT */
#endif /* HAVE_MAX_FRAGMENT */

#ifdef HAVE_TRUNCATED_HMAC
#ifndef NO_WOLFSSL_CLIENT

int wolfSSL_UseTruncatedHMAC(WOLFSSL* ssl)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    return TLSX_UseTruncatedHMAC(&ssl->extensions, ssl->heap);
}


int wolfSSL_CTX_UseTruncatedHMAC(WOLFSSL_CTX* ctx)
{
    if (ctx == NULL)
        return BAD_FUNC_ARG;

    return TLSX_UseTruncatedHMAC(&ctx->extensions, ctx->heap);
}

#endif /* NO_WOLFSSL_CLIENT */
#endif /* HAVE_TRUNCATED_HMAC */

#ifdef HAVE_CERTIFICATE_STATUS_REQUEST

int wolfSSL_UseOCSPStapling(WOLFSSL* ssl, byte status_type, byte options)
{
    if (ssl == NULL || ssl->options.side != WOLFSSL_CLIENT_END)
        return BAD_FUNC_ARG;

    return TLSX_UseCertificateStatusRequest(&ssl->extensions, status_type,
                                                options, ssl->heap, ssl->devId);
}


int wolfSSL_CTX_UseOCSPStapling(WOLFSSL_CTX* ctx, byte status_type,
                                                                   byte options)
{
    if (ctx == NULL || ctx->method->side != WOLFSSL_CLIENT_END)
        return BAD_FUNC_ARG;

    return TLSX_UseCertificateStatusRequest(&ctx->extensions, status_type,
                                                options, ctx->heap, ctx->devId);
}

#endif /* HAVE_CERTIFICATE_STATUS_REQUEST */

#ifdef HAVE_CERTIFICATE_STATUS_REQUEST_V2

int wolfSSL_UseOCSPStaplingV2(WOLFSSL* ssl, byte status_type, byte options)
{
    if (ssl == NULL || ssl->options.side != WOLFSSL_CLIENT_END)
        return BAD_FUNC_ARG;

    return TLSX_UseCertificateStatusRequestV2(&ssl->extensions, status_type,
                                                options, ssl->heap, ssl->devId);
}


int wolfSSL_CTX_UseOCSPStaplingV2(WOLFSSL_CTX* ctx,
                                                 byte status_type, byte options)
{
    if (ctx == NULL || ctx->method->side != WOLFSSL_CLIENT_END)
        return BAD_FUNC_ARG;

    return TLSX_UseCertificateStatusRequestV2(&ctx->extensions, status_type,
                                                options, ctx->heap, ctx->devId);
}

#endif /* HAVE_CERTIFICATE_STATUS_REQUEST_V2 */

/* Elliptic Curves */
#ifdef HAVE_SUPPORTED_CURVES
#ifndef NO_WOLFSSL_CLIENT

int wolfSSL_UseSupportedCurve(WOLFSSL* ssl, word16 name)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    switch (name) {
        case WOLFSSL_ECC_SECP160K1:
        case WOLFSSL_ECC_SECP160R1:
        case WOLFSSL_ECC_SECP160R2:
        case WOLFSSL_ECC_SECP192K1:
        case WOLFSSL_ECC_SECP192R1:
        case WOLFSSL_ECC_SECP224K1:
        case WOLFSSL_ECC_SECP224R1:
        case WOLFSSL_ECC_SECP256K1:
        case WOLFSSL_ECC_SECP256R1:
        case WOLFSSL_ECC_SECP384R1:
        case WOLFSSL_ECC_SECP521R1:
        case WOLFSSL_ECC_BRAINPOOLP256R1:
        case WOLFSSL_ECC_BRAINPOOLP384R1:
        case WOLFSSL_ECC_BRAINPOOLP512R1:
        case WOLFSSL_ECC_X25519:
            break;

#ifdef WOLFSSL_TLS13
        case WOLFSSL_FFDHE_2048:
        case WOLFSSL_FFDHE_3072:
        case WOLFSSL_FFDHE_4096:
        case WOLFSSL_FFDHE_6144:
        case WOLFSSL_FFDHE_8192:
            if (!IsAtLeastTLSv1_3(ssl->version))
                return WOLFSSL_SUCCESS;
            break;
#endif

        default:
            return BAD_FUNC_ARG;
    }

    ssl->options.userCurves = 1;

    return TLSX_UseSupportedCurve(&ssl->extensions, name, ssl->heap);
}


int wolfSSL_CTX_UseSupportedCurve(WOLFSSL_CTX* ctx, word16 name)
{
    if (ctx == NULL)
        return BAD_FUNC_ARG;

    switch (name) {
        case WOLFSSL_ECC_SECP160K1:
        case WOLFSSL_ECC_SECP160R1:
        case WOLFSSL_ECC_SECP160R2:
        case WOLFSSL_ECC_SECP192K1:
        case WOLFSSL_ECC_SECP192R1:
        case WOLFSSL_ECC_SECP224K1:
        case WOLFSSL_ECC_SECP224R1:
        case WOLFSSL_ECC_SECP256K1:
        case WOLFSSL_ECC_SECP256R1:
        case WOLFSSL_ECC_SECP384R1:
        case WOLFSSL_ECC_SECP521R1:
        case WOLFSSL_ECC_BRAINPOOLP256R1:
        case WOLFSSL_ECC_BRAINPOOLP384R1:
        case WOLFSSL_ECC_BRAINPOOLP512R1:
        case WOLFSSL_ECC_X25519:
            break;

#ifdef WOLFSSL_TLS13
        case WOLFSSL_FFDHE_2048:
        case WOLFSSL_FFDHE_3072:
        case WOLFSSL_FFDHE_4096:
        case WOLFSSL_FFDHE_6144:
        case WOLFSSL_FFDHE_8192:
            break;
#endif

        default:
            return BAD_FUNC_ARG;
    }

    ctx->userCurves = 1;

    return TLSX_UseSupportedCurve(&ctx->extensions, name, ctx->heap);
}

#endif /* NO_WOLFSSL_CLIENT */
#endif /* HAVE_SUPPORTED_CURVES */

/* QSH quantum safe handshake */
#ifdef HAVE_QSH
/* returns 1 if QSH has been used 0 otherwise */
int wolfSSL_isQSH(WOLFSSL* ssl)
{
    /* if no ssl struct than QSH was not used */
    if (ssl == NULL)
        return 0;

    return ssl->isQSH;
}


int wolfSSL_UseSupportedQSH(WOLFSSL* ssl, word16 name)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    switch (name) {
    #ifdef HAVE_NTRU
        case WOLFSSL_NTRU_EESS439:
        case WOLFSSL_NTRU_EESS593:
        case WOLFSSL_NTRU_EESS743:
            break;
    #endif
        default:
            return BAD_FUNC_ARG;
    }

    ssl->user_set_QSHSchemes = 1;

    return TLSX_UseQSHScheme(&ssl->extensions, name, NULL, 0, ssl->heap);
}

#ifndef NO_WOLFSSL_CLIENT
    /* user control over sending client public key in hello
       when flag = 1 will send keys if flag is 0 or function is not called
       then will not send keys in the hello extension
       return 0 on success
     */
    int wolfSSL_UseClientQSHKeys(WOLFSSL* ssl, unsigned char flag)
    {
        if (ssl == NULL)
            return BAD_FUNC_ARG;

        ssl->sendQSHKeys = flag;

        return 0;
    }
#endif /* NO_WOLFSSL_CLIENT */
#endif /* HAVE_QSH */

/* Application-Layer Protocol Negotiation */
#ifdef HAVE_ALPN

int wolfSSL_UseALPN(WOLFSSL* ssl, char *protocol_name_list,
                    word32 protocol_name_listSz, byte options)
{
    char    *list, *ptr, *token[10];
    word16  len;
    int     idx = 0;
    int     ret = WOLFSSL_FAILURE;

    WOLFSSL_ENTER("wolfSSL_UseALPN");

    if (ssl == NULL || protocol_name_list == NULL)
        return BAD_FUNC_ARG;

    if (protocol_name_listSz > (WOLFSSL_MAX_ALPN_NUMBER *
                                WOLFSSL_MAX_ALPN_PROTO_NAME_LEN +
                                WOLFSSL_MAX_ALPN_NUMBER)) {
        WOLFSSL_MSG("Invalid arguments, protocol name list too long");
        return BAD_FUNC_ARG;
    }

    if (!(options & WOLFSSL_ALPN_CONTINUE_ON_MISMATCH) &&
        !(options & WOLFSSL_ALPN_FAILED_ON_MISMATCH)) {
            WOLFSSL_MSG("Invalid arguments, options not supported");
            return BAD_FUNC_ARG;
        }


    list = (char *)XMALLOC(protocol_name_listSz+1, ssl->heap,
                           DYNAMIC_TYPE_ALPN);
    if (list == NULL) {
        WOLFSSL_MSG("Memory failure");
        return MEMORY_ERROR;
    }

    XSTRNCPY(list, protocol_name_list, protocol_name_listSz);
    list[protocol_name_listSz] = '\0';

    /* read all protocol name from the list */
    token[idx] = XSTRTOK(list, ",", &ptr);
    while (token[idx] != NULL)
        token[++idx] = XSTRTOK(NULL, ",", &ptr);

    /* add protocol name list in the TLS extension in reverse order */
    while ((idx--) > 0) {
        len = (word16)XSTRLEN(token[idx]);

        ret = TLSX_UseALPN(&ssl->extensions, token[idx], len, options,
                                                                     ssl->heap);
        if (ret != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("TLSX_UseALPN failure");
            break;
        }
    }

    XFREE(list, ssl->heap, DYNAMIC_TYPE_ALPN);

    return ret;
}

int wolfSSL_ALPN_GetProtocol(WOLFSSL* ssl, char **protocol_name, word16 *size)
{
    return TLSX_ALPN_GetRequest(ssl ? ssl->extensions : NULL,
                               (void **)protocol_name, size);
}

int wolfSSL_ALPN_GetPeerProtocol(WOLFSSL* ssl, char **list, word16 *listSz)
{
    if (list == NULL || listSz == NULL)
        return BAD_FUNC_ARG;

    if (ssl->alpn_client_list == NULL)
        return BUFFER_ERROR;

    *listSz = (word16)XSTRLEN(ssl->alpn_client_list);
    if (*listSz == 0)
        return BUFFER_ERROR;

    *list = (char *)XMALLOC((*listSz)+1, ssl->heap, DYNAMIC_TYPE_TLSX);
    if (*list == NULL)
        return MEMORY_ERROR;

    XSTRNCPY(*list, ssl->alpn_client_list, (*listSz)+1);
    (*list)[*listSz] = 0;

    return WOLFSSL_SUCCESS;
}


/* used to free memory allocated by wolfSSL_ALPN_GetPeerProtocol */
int wolfSSL_ALPN_FreePeerProtocol(WOLFSSL* ssl, char **list)
{
    if (ssl == NULL) {
        return BAD_FUNC_ARG;
    }

    XFREE(*list, ssl->heap, DYNAMIC_TYPE_TLSX);
    *list = NULL;

    return WOLFSSL_SUCCESS;
}

#endif /* HAVE_ALPN */

/* Secure Renegotiation */
#ifdef HAVE_SECURE_RENEGOTIATION

/* user is forcing ability to use secure renegotiation, we discourage it */
int wolfSSL_UseSecureRenegotiation(WOLFSSL* ssl)
{
    int ret = BAD_FUNC_ARG;

    if (ssl)
        ret = TLSX_UseSecureRenegotiation(&ssl->extensions, ssl->heap);

    if (ret == WOLFSSL_SUCCESS) {
        TLSX* extension = TLSX_Find(ssl->extensions, TLSX_RENEGOTIATION_INFO);

        if (extension)
            ssl->secure_renegotiation = (SecureRenegotiation*)extension->data;
    }

    return ret;
}


/* do a secure renegotiation handshake, user forced, we discourage */
int wolfSSL_Rehandshake(WOLFSSL* ssl)
{
    int ret;

    if (ssl == NULL)
        return BAD_FUNC_ARG;

    if (ssl->secure_renegotiation == NULL) {
        WOLFSSL_MSG("Secure Renegotiation not forced on by user");
        return SECURE_RENEGOTIATION_E;
    }

    if (ssl->secure_renegotiation->enabled == 0) {
        WOLFSSL_MSG("Secure Renegotiation not enabled at extension level");
        return SECURE_RENEGOTIATION_E;
    }

    if (ssl->options.handShakeState != HANDSHAKE_DONE) {
        WOLFSSL_MSG("Can't renegotiate until previous handshake complete");
        return SECURE_RENEGOTIATION_E;
    }

#ifndef NO_FORCE_SCR_SAME_SUITE
    /* force same suite */
    if (ssl->suites) {
        ssl->suites->suiteSz = SUITE_LEN;
        ssl->suites->suites[0] = ssl->options.cipherSuite0;
        ssl->suites->suites[1] = ssl->options.cipherSuite;
    }
#endif

    /* reset handshake states */
    ssl->options.serverState = NULL_STATE;
    ssl->options.clientState = NULL_STATE;
    ssl->options.connectState  = CONNECT_BEGIN;
    ssl->options.acceptState   = ACCEPT_BEGIN;
    ssl->options.handShakeState = NULL_STATE;
    ssl->options.processReply  = 0;  /* TODO, move states in internal.h */

    XMEMSET(&ssl->msgsReceived, 0, sizeof(ssl->msgsReceived));

    ssl->secure_renegotiation->cache_status = SCR_CACHE_NEEDED;

    ret = InitHandshakeHashes(ssl);
    if (ret !=0)
        return ret;

    ret = wolfSSL_negotiate(ssl);
    return ret;
}

#endif /* HAVE_SECURE_RENEGOTIATION */

/* Session Ticket */
#if !defined(NO_WOLFSSL_SERVER) && defined(HAVE_SESSION_TICKET)
/* WOLFSSL_SUCCESS on ok */
int wolfSSL_CTX_set_TicketEncCb(WOLFSSL_CTX* ctx, SessionTicketEncCb cb)
{
    if (ctx == NULL)
        return BAD_FUNC_ARG;

    ctx->ticketEncCb = cb;

    return WOLFSSL_SUCCESS;
}

/* set hint interval, WOLFSSL_SUCCESS on ok */
int wolfSSL_CTX_set_TicketHint(WOLFSSL_CTX* ctx, int hint)
{
    if (ctx == NULL)
        return BAD_FUNC_ARG;

    ctx->ticketHint = hint;

    return WOLFSSL_SUCCESS;
}

/* set user context, WOLFSSL_SUCCESS on ok */
int wolfSSL_CTX_set_TicketEncCtx(WOLFSSL_CTX* ctx, void* userCtx)
{
    if (ctx == NULL)
        return BAD_FUNC_ARG;

    ctx->ticketEncCtx = userCtx;

    return WOLFSSL_SUCCESS;
}

#endif /* !defined(NO_WOLFSSL_CLIENT) && defined(HAVE_SESSION_TICKET) */

/* Session Ticket */
#if !defined(NO_WOLFSSL_CLIENT) && defined(HAVE_SESSION_TICKET)
int wolfSSL_UseSessionTicket(WOLFSSL* ssl)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    return TLSX_UseSessionTicket(&ssl->extensions, NULL, ssl->heap);
}

int wolfSSL_CTX_UseSessionTicket(WOLFSSL_CTX* ctx)
{
    if (ctx == NULL)
        return BAD_FUNC_ARG;

    return TLSX_UseSessionTicket(&ctx->extensions, NULL, ctx->heap);
}

WOLFSSL_API int wolfSSL_get_SessionTicket(WOLFSSL* ssl,
                                          byte* buf, word32* bufSz)
{
    if (ssl == NULL || buf == NULL || bufSz == NULL || *bufSz == 0)
        return BAD_FUNC_ARG;

    if (ssl->session.ticketLen <= *bufSz) {
        XMEMCPY(buf, ssl->session.ticket, ssl->session.ticketLen);
        *bufSz = ssl->session.ticketLen;
    }
    else
        *bufSz = 0;

    return WOLFSSL_SUCCESS;
}

WOLFSSL_API int wolfSSL_set_SessionTicket(WOLFSSL* ssl, const byte* buf,
                                          word32 bufSz)
{
    if (ssl == NULL || (buf == NULL && bufSz > 0))
        return BAD_FUNC_ARG;

    if (bufSz > 0) {
        /* Ticket will fit into static ticket */
        if(bufSz <= SESSION_TICKET_LEN) {
            if (ssl->session.isDynamic) {
                XFREE(ssl->session.ticket, ssl->heap, DYNAMIC_TYPE_SESSION_TICK);
                ssl->session.isDynamic = 0;
                ssl->session.ticket = ssl->session.staticTicket;
            }
        } else { /* Ticket requires dynamic ticket storage */
            if (ssl->session.ticketLen < bufSz) { /* is dyn buffer big enough */
                if(ssl->session.isDynamic)
                    XFREE(ssl->session.ticket, ssl->heap,
                            DYNAMIC_TYPE_SESSION_TICK);
                ssl->session.ticket = (byte*)XMALLOC(bufSz, ssl->heap,
                        DYNAMIC_TYPE_SESSION_TICK);
                if(!ssl->session.ticket) {
                    ssl->session.ticket = ssl->session.staticTicket;
                    ssl->session.isDynamic = 0;
                    return MEMORY_ERROR;
                }
                ssl->session.isDynamic = 1;
            }
        }
        XMEMCPY(ssl->session.ticket, buf, bufSz);
    }
    ssl->session.ticketLen = (word16)bufSz;

    return WOLFSSL_SUCCESS;
}


WOLFSSL_API int wolfSSL_set_SessionTicket_cb(WOLFSSL* ssl,
                                            CallbackSessionTicket cb, void* ctx)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    ssl->session_ticket_cb = cb;
    ssl->session_ticket_ctx = ctx;

    return WOLFSSL_SUCCESS;
}
#endif


#ifdef HAVE_EXTENDED_MASTER
#ifndef NO_WOLFSSL_CLIENT

int wolfSSL_CTX_DisableExtendedMasterSecret(WOLFSSL_CTX* ctx)
{
    if (ctx == NULL)
        return BAD_FUNC_ARG;

    ctx->haveEMS = 0;

    return WOLFSSL_SUCCESS;
}


int wolfSSL_DisableExtendedMasterSecret(WOLFSSL* ssl)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    ssl->options.haveEMS = 0;

    return WOLFSSL_SUCCESS;
}

#endif
#endif


#ifndef WOLFSSL_LEANPSK

int wolfSSL_send(WOLFSSL* ssl, const void* data, int sz, int flags)
{
    int ret;
    int oldFlags;

    WOLFSSL_ENTER("wolfSSL_send()");

    if (ssl == NULL || data == NULL || sz < 0)
        return BAD_FUNC_ARG;

    oldFlags = ssl->wflags;

    ssl->wflags = flags;
    ret = wolfSSL_write(ssl, data, sz);
    ssl->wflags = oldFlags;

    WOLFSSL_LEAVE("wolfSSL_send()", ret);

    return ret;
}


int wolfSSL_recv(WOLFSSL* ssl, void* data, int sz, int flags)
{
    int ret;
    int oldFlags;

    WOLFSSL_ENTER("wolfSSL_recv()");

    if (ssl == NULL || data == NULL || sz < 0)
        return BAD_FUNC_ARG;

    oldFlags = ssl->rflags;

    ssl->rflags = flags;
    ret = wolfSSL_read(ssl, data, sz);
    ssl->rflags = oldFlags;

    WOLFSSL_LEAVE("wolfSSL_recv()", ret);

    return ret;
}
#endif


/* WOLFSSL_SUCCESS on ok */
int wolfSSL_shutdown(WOLFSSL* ssl)
{
    int  ret = WOLFSSL_FATAL_ERROR;
    byte tmp;
    WOLFSSL_ENTER("SSL_shutdown()");

    if (ssl == NULL)
        return WOLFSSL_FATAL_ERROR;

    if (ssl->options.quietShutdown) {
        WOLFSSL_MSG("quiet shutdown, no close notify sent");
        ret = WOLFSSL_SUCCESS;
    }
    else {
        /* try to send close notify, not an error if can't */
        if (!ssl->options.isClosed && !ssl->options.connReset &&
                                      !ssl->options.sentNotify) {
            ssl->error = SendAlert(ssl, alert_warning, close_notify);
            if (ssl->error < 0) {
                WOLFSSL_ERROR(ssl->error);
                return WOLFSSL_FATAL_ERROR;
            }
            ssl->options.sentNotify = 1;  /* don't send close_notify twice */
            if (ssl->options.closeNotify)
                ret = WOLFSSL_SUCCESS;
            else {
                ret = WOLFSSL_SHUTDOWN_NOT_DONE;
                WOLFSSL_LEAVE("SSL_shutdown()", ret);
                return ret;
            }
        }

        /* call wolfSSL_shutdown again for bidirectional shutdown */
        if (ssl->options.sentNotify && !ssl->options.closeNotify) {
            ret = wolfSSL_read(ssl, &tmp, 0);
            if (ret < 0) {
                WOLFSSL_ERROR(ssl->error);
                ret = WOLFSSL_FATAL_ERROR;
            } else if (ssl->options.closeNotify) {
                ssl->error = WOLFSSL_ERROR_SYSCALL;   /* simulate OpenSSL behavior */
                ret = WOLFSSL_SUCCESS;
            }
        }
    }

#ifdef OPENSSL_EXTRA
    /* reset WOLFSSL structure state for possible re-use */
    if (ret == WOLFSSL_SUCCESS) {
        if (wolfSSL_clear(ssl) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("could not clear WOLFSSL");
            ret = WOLFSSL_FATAL_ERROR;
        }
    }
#endif

    WOLFSSL_LEAVE("SSL_shutdown()", ret);

    return ret;
}


/* get current error state value */
int wolfSSL_state(WOLFSSL* ssl)
{
    if (ssl == NULL) {
        return BAD_FUNC_ARG;
    }

    return ssl->error;
}


int wolfSSL_get_error(WOLFSSL* ssl, int ret)
{
    WOLFSSL_ENTER("SSL_get_error");

    if (ret > 0)
        return WOLFSSL_ERROR_NONE;
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    WOLFSSL_LEAVE("SSL_get_error", ssl->error);

    /* make sure converted types are handled in SetErrorString() too */
    if (ssl->error == WANT_READ)
        return WOLFSSL_ERROR_WANT_READ;         /* convert to OpenSSL type */
    else if (ssl->error == WANT_WRITE)
        return WOLFSSL_ERROR_WANT_WRITE;        /* convert to OpenSSL type */
    else if (ssl->error == ZERO_RETURN)
        return WOLFSSL_ERROR_ZERO_RETURN;       /* convert to OpenSSL type */
    return ssl->error;
}


/* retrive alert history, WOLFSSL_SUCCESS on ok */
int wolfSSL_get_alert_history(WOLFSSL* ssl, WOLFSSL_ALERT_HISTORY *h)
{
    if (ssl && h) {
        *h = ssl->alert_history;
    }
    return WOLFSSL_SUCCESS;
}


/* return TRUE if current error is want read */
int wolfSSL_want_read(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("SSL_want_read");
    if (ssl->error == WANT_READ)
        return 1;

    return 0;
}


/* return TRUE if current error is want write */
int wolfSSL_want_write(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("SSL_want_write");
    if (ssl->error == WANT_WRITE)
        return 1;

    return 0;
}


char* wolfSSL_ERR_error_string(unsigned long errNumber, char* data)
{
    static const char* const msg = "Please supply a buffer for error string";

    WOLFSSL_ENTER("ERR_error_string");
    if (data) {
        SetErrorString((int)errNumber, data);
        return data;
    }

    return (char*)msg;
}


void wolfSSL_ERR_error_string_n(unsigned long e, char* buf, unsigned long len)
{
    WOLFSSL_ENTER("wolfSSL_ERR_error_string_n");
    if (len >= WOLFSSL_MAX_ERROR_SZ)
        wolfSSL_ERR_error_string(e, buf);
    else {
        char tmp[WOLFSSL_MAX_ERROR_SZ];

        WOLFSSL_MSG("Error buffer too short, truncating");
        if (len) {
            wolfSSL_ERR_error_string(e, tmp);
            XMEMCPY(buf, tmp, len-1);
            buf[len-1] = '\0';
        }
    }
}


/* don't free temporary arrays at end of handshake */
void wolfSSL_KeepArrays(WOLFSSL* ssl)
{
    if (ssl)
        ssl->options.saveArrays = 1;
}


/* user doesn't need temporary arrays anymore, Free */
void wolfSSL_FreeArrays(WOLFSSL* ssl)
{
    if (ssl && ssl->options.handShakeState == HANDSHAKE_DONE) {
        ssl->options.saveArrays = 0;
        FreeArrays(ssl, 1);
    }
}

/* Set option to indicate that the resources are not to be freed after
 * handshake.
 *
 * ssl  The SSL/TLS object.
 * returns BAD_FUNC_ARG when ssl is NULL and 0 on success.
 */
int wolfSSL_KeepHandshakeResources(WOLFSSL* ssl)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    ssl->options.keepResources = 1;

    return 0;
}

/* Free the handshake resources after handshake.
 *
 * ssl  The SSL/TLS object.
 * returns BAD_FUNC_ARG when ssl is NULL and 0 on success.
 */
int wolfSSL_FreeHandshakeResources(WOLFSSL* ssl)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    FreeHandshakeResources(ssl);

    return 0;
}

/* Use the client's order of preference when matching cipher suites.
 *
 * ssl  The SSL/TLS context object.
 * returns BAD_FUNC_ARG when ssl is NULL and 0 on success.
 */
int wolfSSL_CTX_UseClientSuites(WOLFSSL_CTX* ctx)
{
    if (ctx == NULL)
        return BAD_FUNC_ARG;

    ctx->useClientOrder = 1;

    return 0;
}

/* Use the client's order of preference when matching cipher suites.
 *
 * ssl  The SSL/TLS object.
 * returns BAD_FUNC_ARG when ssl is NULL and 0 on success.
 */
int wolfSSL_UseClientSuites(WOLFSSL* ssl)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    ssl->options.useClientOrder = 1;

    return 0;
}

const byte* wolfSSL_GetMacSecret(WOLFSSL* ssl, int verify)
{
    if (ssl == NULL)
        return NULL;

    if ( (ssl->options.side == WOLFSSL_CLIENT_END && !verify) ||
         (ssl->options.side == WOLFSSL_SERVER_END &&  verify) )
        return ssl->keys.client_write_MAC_secret;
    else
        return ssl->keys.server_write_MAC_secret;
}


#ifdef ATOMIC_USER

void  wolfSSL_CTX_SetMacEncryptCb(WOLFSSL_CTX* ctx, CallbackMacEncrypt cb)
{
    if (ctx)
        ctx->MacEncryptCb = cb;
}


void  wolfSSL_SetMacEncryptCtx(WOLFSSL* ssl, void *ctx)
{
    if (ssl)
        ssl->MacEncryptCtx = ctx;
}


void* wolfSSL_GetMacEncryptCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->MacEncryptCtx;

    return NULL;
}


void  wolfSSL_CTX_SetDecryptVerifyCb(WOLFSSL_CTX* ctx, CallbackDecryptVerify cb)
{
    if (ctx)
        ctx->DecryptVerifyCb = cb;
}


void  wolfSSL_SetDecryptVerifyCtx(WOLFSSL* ssl, void *ctx)
{
    if (ssl)
        ssl->DecryptVerifyCtx = ctx;
}


void* wolfSSL_GetDecryptVerifyCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->DecryptVerifyCtx;

    return NULL;
}


const byte* wolfSSL_GetClientWriteKey(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->keys.client_write_key;

    return NULL;
}


const byte* wolfSSL_GetClientWriteIV(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->keys.client_write_IV;

    return NULL;
}


const byte* wolfSSL_GetServerWriteKey(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->keys.server_write_key;

    return NULL;
}


const byte* wolfSSL_GetServerWriteIV(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->keys.server_write_IV;

    return NULL;
}

int wolfSSL_GetKeySize(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->specs.key_size;

    return BAD_FUNC_ARG;
}


int wolfSSL_GetIVSize(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->specs.iv_size;

    return BAD_FUNC_ARG;
}


int wolfSSL_GetBulkCipher(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->specs.bulk_cipher_algorithm;

    return BAD_FUNC_ARG;
}


int wolfSSL_GetCipherType(WOLFSSL* ssl)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    if (ssl->specs.cipher_type == block)
        return WOLFSSL_BLOCK_TYPE;
    if (ssl->specs.cipher_type == stream)
        return WOLFSSL_STREAM_TYPE;
    if (ssl->specs.cipher_type == aead)
        return WOLFSSL_AEAD_TYPE;

    return -1;
}


int wolfSSL_GetCipherBlockSize(WOLFSSL* ssl)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    return ssl->specs.block_size;
}


int wolfSSL_GetAeadMacSize(WOLFSSL* ssl)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    return ssl->specs.aead_mac_size;
}


int wolfSSL_IsTLSv1_1(WOLFSSL* ssl)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    if (ssl->options.tls1_1)
        return 1;

    return 0;
}


int wolfSSL_GetSide(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->options.side;

    return BAD_FUNC_ARG;
}


int wolfSSL_GetHmacSize(WOLFSSL* ssl)
{
    /* AEAD ciphers don't have HMAC keys */
    if (ssl)
        return (ssl->specs.cipher_type != aead) ? ssl->specs.hash_size : 0;

    return BAD_FUNC_ARG;
}

#endif /* ATOMIC_USER */

#ifndef NO_CERTS

WOLFSSL_CERT_MANAGER* wolfSSL_CTX_GetCertManager(WOLFSSL_CTX* ctx)
{
    WOLFSSL_CERT_MANAGER* cm = NULL;
    if (ctx)
        cm = ctx->cm;
    return cm;
}

WOLFSSL_CERT_MANAGER* wolfSSL_CertManagerNew_ex(void* heap)
{
    WOLFSSL_CERT_MANAGER* cm = NULL;

    WOLFSSL_ENTER("wolfSSL_CertManagerNew");

    cm = (WOLFSSL_CERT_MANAGER*) XMALLOC(sizeof(WOLFSSL_CERT_MANAGER), heap,
                                         DYNAMIC_TYPE_CERT_MANAGER);
    if (cm) {
        XMEMSET(cm, 0, sizeof(WOLFSSL_CERT_MANAGER));

        if (wc_InitMutex(&cm->caLock) != 0) {
            WOLFSSL_MSG("Bad mutex init");
            wolfSSL_CertManagerFree(cm);
            return NULL;
        }

        #ifdef WOLFSSL_TRUST_PEER_CERT
        if (wc_InitMutex(&cm->tpLock) != 0) {
            WOLFSSL_MSG("Bad mutex init");
            wolfSSL_CertManagerFree(cm);
            return NULL;
        }
        #endif

        /* set default minimum key size allowed */
        #ifndef NO_RSA
            cm->minRsaKeySz = MIN_RSAKEY_SZ;
        #endif
        #ifdef HAVE_ECC
            cm->minEccKeySz = MIN_ECCKEY_SZ;
        #endif
            cm->heap = heap;
    }

    return cm;
}


WOLFSSL_CERT_MANAGER* wolfSSL_CertManagerNew(void)
{
    return wolfSSL_CertManagerNew_ex(NULL);
}


void wolfSSL_CertManagerFree(WOLFSSL_CERT_MANAGER* cm)
{
    WOLFSSL_ENTER("wolfSSL_CertManagerFree");

    if (cm) {
        #ifdef HAVE_CRL
            if (cm->crl)
                FreeCRL(cm->crl, 1);
        #endif
        #ifdef HAVE_OCSP
            if (cm->ocsp)
                FreeOCSP(cm->ocsp, 1);
            XFREE(cm->ocspOverrideURL, cm->heap, DYNAMIC_TYPE_URL);
        #if defined(HAVE_CERTIFICATE_STATUS_REQUEST) \
         || defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
            if (cm->ocsp_stapling)
                FreeOCSP(cm->ocsp_stapling, 1);
        #endif
        #endif
        FreeSignerTable(cm->caTable, CA_TABLE_SIZE, cm->heap);
        wc_FreeMutex(&cm->caLock);

        #ifdef WOLFSSL_TRUST_PEER_CERT
        FreeTrustedPeerTable(cm->tpTable, TP_TABLE_SIZE, cm->heap);
        wc_FreeMutex(&cm->tpLock);
        #endif

        XFREE(cm, cm->heap, DYNAMIC_TYPE_CERT_MANAGER);
    }

}


/* Unload the CA signer list */
int wolfSSL_CertManagerUnloadCAs(WOLFSSL_CERT_MANAGER* cm)
{
    WOLFSSL_ENTER("wolfSSL_CertManagerUnloadCAs");

    if (cm == NULL)
        return BAD_FUNC_ARG;

    if (wc_LockMutex(&cm->caLock) != 0)
        return BAD_MUTEX_E;

    FreeSignerTable(cm->caTable, CA_TABLE_SIZE, NULL);

    wc_UnLockMutex(&cm->caLock);


    return WOLFSSL_SUCCESS;
}


#ifdef WOLFSSL_TRUST_PEER_CERT
int wolfSSL_CertManagerUnload_trust_peers(WOLFSSL_CERT_MANAGER* cm)
{
    WOLFSSL_ENTER("wolfSSL_CertManagerUnload_trust_peers");

    if (cm == NULL)
        return BAD_FUNC_ARG;

    if (wc_LockMutex(&cm->tpLock) != 0)
        return BAD_MUTEX_E;

    FreeTrustedPeerTable(cm->tpTable, TP_TABLE_SIZE, NULL);

    wc_UnLockMutex(&cm->tpLock);


    return WOLFSSL_SUCCESS;
}
#endif /* WOLFSSL_TRUST_PEER_CERT */

#endif /* NO_CERTS */

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL) || \
    defined(HAVE_WEBSERVER)

static const struct cipher{
        unsigned char type;
        const char *name;
} cipher_tbl[] = {

#ifndef NO_AES
    #ifdef WOLFSSL_AES_128
    {AES_128_CBC_TYPE, "AES-128-CBC"},
    #endif
    #ifdef WOLFSSL_AES_192
    {AES_192_CBC_TYPE, "AES-192-CBC"},
    #endif
    #ifdef WOLFSSL_AES_256
    {AES_256_CBC_TYPE, "AES-256-CBC"},
    #endif
#if defined(OPENSSL_EXTRA)
    #ifdef WOLFSSL_AES_128
        {AES_128_CTR_TYPE, "AES-128-CTR"},
    #endif
    #ifdef WOLFSSL_AES_192
        {AES_192_CTR_TYPE, "AES-192-CTR"},
    #endif
    #ifdef WOLFSSL_AES_256
        {AES_256_CTR_TYPE, "AES-256-CTR"},
    #endif

    #ifdef WOLFSSL_AES_128
        {AES_128_ECB_TYPE, "AES-128-ECB"},
    #endif
    #ifdef WOLFSSL_AES_192
        {AES_192_ECB_TYPE, "AES-192-ECB"},
    #endif
    #ifdef WOLFSSL_AES_256
        {AES_256_ECB_TYPE, "AES-256-ECB"},
    #endif
#endif

#endif

#ifndef NO_DES3
    {DES_CBC_TYPE, "DES-CBC"},
    {DES_ECB_TYPE, "DES-ECB"},

    {DES_EDE3_CBC_TYPE, "DES-EDE3-CBC"},
    {DES_EDE3_ECB_TYPE, "DES-EDE3-ECB"},
#endif

#ifndef NO_RC4
    {ARC4_TYPE, "ARC4"},
#endif

#ifdef HAVE_IDEA
    {IDEA_CBC_TYPE, "IDEA-CBC"},
#endif
    { 0, NULL}
};

const WOLFSSL_EVP_CIPHER *wolfSSL_EVP_get_cipherbyname(const char *name)
{

    static const struct alias {
        const char *name;
        const char *alias;
    } alias_tbl[] =
    {
#ifndef NO_DES3
        {"DES-CBC", "DES"},
        {"DES-CBC", "des"},
        {"DES-ECB", "DES-ECB"},
        {"DES-ECB", "des-ecb"},
        {"DES-EDE3-CBC", "DES3"},
        {"DES-EDE3-CBC", "des3"},
        {"DES-EDE3-ECB", "DES-EDE3"},
        {"DES-EDE3-ECB", "des-ede3"},
        {"DES-EDE3-ECB", "des-ede3-ecb"},
#endif
#ifdef HAVE_IDEA
        {"IDEA-CBC", "IDEA"},
        {"IDEA-CBC", "idea"},
#endif
#ifndef NO_AES
    #ifdef HAVE_AES_CBC
        #ifdef WOLFSSL_AES_128
        {"AES-128-CBC", "AES128-CBC"},
        {"AES-128-CBC", "aes128-cbc"},
        #endif
        #ifdef WOLFSSL_AES_192
        {"AES-192-CBC", "AES192-CBC"},
        {"AES-192-CBC", "aes192-cbc"},
        #endif
        #ifdef WOLFSSL_AES_256
        {"AES-256-CBC", "AES256-CBC"},
        {"AES-256-CBC", "aes256-cbc"},
        #endif
    #endif
    #ifdef WOLFSSL_AES_128
        {"AES-128-ECB", "AES128-ECB"},
        {"AES-128-ECB", "aes128-ecb"},
    #endif
    #ifdef WOLFSSL_AES_192
        {"AES-192-ECB", "AES192-ECB"},
        {"AES-192-ECB", "aes192-ecb"},
    #endif
    #ifdef WOLFSSL_AES_256
        {"AES-256-ECB", "AES256-ECB"},
        {"AES-256-EBC", "aes256-ecb"},
    #endif
#endif
#ifndef NO_RC4
        {"ARC4", "RC4"},
#endif
        { NULL, NULL}
    };

    const struct cipher *ent;
    const struct alias  *al;

    WOLFSSL_ENTER("EVP_get_cipherbyname");

    for( al = alias_tbl; al->name != NULL; al++)
        if(XSTRNCMP(name, al->alias, XSTRLEN(al->alias)+1) == 0) {
            name = al->name;
            break;
        }

    for( ent = cipher_tbl; ent->name != NULL; ent++)
        if(XSTRNCMP(name, ent->name, XSTRLEN(ent->name)+1) == 0) {
            return (WOLFSSL_EVP_CIPHER *)ent->name;
        }

    return NULL;
}

/*
 * return an EVP_CIPHER structure when cipher NID is passed.
 *
 * id  cipher NID
 *
 * retrun WOLFSSL_EVP_CIPHER
*/
const WOLFSSL_EVP_CIPHER *wolfSSL_EVP_get_cipherbynid(int id)
{
    WOLFSSL_ENTER("EVP_get_cipherbynid");

    switch(id) {

#if defined(OPENSSL_EXTRA)
#ifndef NO_AES
    #ifdef HAVE_AES_CBC
        #ifdef WOLFSSL_AES_128
        case NID_aes_128_cbc:
            return wolfSSL_EVP_aes_128_cbc();
        #endif
        #ifdef WOLFSSL_AES_192
        case NID_aes_192_cbc:
            return wolfSSL_EVP_aes_192_cbc();
        #endif
        #ifdef WOLFSSL_AES_256
        case NID_aes_256_cbc:
            return wolfSSL_EVP_aes_256_cbc();
        #endif
    #endif
    #ifdef WOLFSSL_AES_COUNTER
        #ifdef WOLFSSL_AES_128
        case NID_aes_128_ctr:
            return wolfSSL_EVP_aes_128_ctr();
        #endif
        #ifdef WOLFSSL_AES_192
        case NID_aes_192_ctr:
            return wolfSSL_EVP_aes_192_ctr();
        #endif
        #ifdef WOLFSSL_AES_256
        case NID_aes_256_ctr:
            return wolfSSL_EVP_aes_256_ctr();
        #endif
    #endif /* WOLFSSL_AES_COUNTER */
    #ifdef HAVE_AES_ECB
        #ifdef WOLFSSL_AES_128
        case NID_aes_128_ecb:
            return wolfSSL_EVP_aes_128_ecb();
        #endif
        #ifdef WOLFSSL_AES_192
        case NID_aes_192_ecb:
            return wolfSSL_EVP_aes_192_ecb();
        #endif
        #ifdef WOLFSSL_AES_256
        case NID_aes_256_ecb:
            return wolfSSL_EVP_aes_256_ecb();
        #endif
    #endif /* HAVE_AES_ECB */
#endif

#ifndef NO_DES3
        case NID_des_cbc:
            return wolfSSL_EVP_des_cbc();
#ifdef WOLFSSL_DES_ECB
        case NID_des_ecb:
            return wolfSSL_EVP_des_ecb();
#endif
        case NID_des_ede3_cbc:
            return wolfSSL_EVP_des_ede3_cbc();
#ifdef WOLFSSL_DES_ECB
        case NID_des_ede3_ecb:
            return wolfSSL_EVP_des_ede3_ecb();
#endif
#endif /*NO_DES3*/

#ifdef HAVE_IDEA
        case NID_idea_cbc:
            return wolfSSL_EVP_idea_cbc();
#endif
#endif /*OPENSSL_EXTRA*/

        default:
            WOLFSSL_MSG("Bad cipher id value");
    }

    return NULL;
}

#ifndef NO_AES
    #ifdef HAVE_AES_CBC
    #ifdef WOLFSSL_AES_128
        static char *EVP_AES_128_CBC;
    #endif
    #ifdef WOLFSSL_AES_192
        static char *EVP_AES_192_CBC;
    #endif
    #ifdef WOLFSSL_AES_256
        static char *EVP_AES_256_CBC;
    #endif
    #endif /* HAVE_AES_CBC */
#if defined(OPENSSL_EXTRA)
    #ifdef WOLFSSL_AES_128
    static char *EVP_AES_128_CTR;
    #endif
    #ifdef WOLFSSL_AES_192
    static char *EVP_AES_192_CTR;
    #endif
    #ifdef WOLFSSL_AES_256
    static char *EVP_AES_256_CTR;
    #endif

    #ifdef WOLFSSL_AES_128
    static char *EVP_AES_128_ECB;
    #endif
    #ifdef WOLFSSL_AES_192
    static char *EVP_AES_192_ECB;
    #endif
    #ifdef WOLFSSL_AES_256
    static char *EVP_AES_256_ECB;
    #endif
    static const int  EVP_AES_SIZE = 11;
#endif
#endif

#ifndef NO_DES3
static char *EVP_DES_CBC;
static char *EVP_DES_ECB;

static char *EVP_DES_EDE3_CBC;
static char *EVP_DES_EDE3_ECB;

#ifdef OPENSSL_EXTRA
static const int  EVP_DES_SIZE = 7;
static const int  EVP_DES_EDE3_SIZE = 12;
#endif

#endif

#ifdef HAVE_IDEA
static char *EVP_IDEA_CBC;
#if defined(OPENSSL_EXTRA)
static const int  EVP_IDEA_SIZE = 8;
#endif
#endif

void wolfSSL_EVP_init(void)
{
#ifndef NO_AES
    #ifdef HAVE_AES_CBC
        #ifdef WOLFSSL_AES_128
        EVP_AES_128_CBC = (char *)EVP_get_cipherbyname("AES-128-CBC");
        #endif
        #ifdef WOLFSSL_AES_192
        EVP_AES_192_CBC = (char *)EVP_get_cipherbyname("AES-192-CBC");
        #endif
        #ifdef WOLFSSL_AES_256
        EVP_AES_256_CBC = (char *)EVP_get_cipherbyname("AES-256-CBC");
        #endif
    #endif /* HAVE_AES_CBC */

#if defined(OPENSSL_EXTRA)
        #ifdef WOLFSSL_AES_128
        EVP_AES_128_CTR = (char *)EVP_get_cipherbyname("AES-128-CTR");
        #endif
        #ifdef WOLFSSL_AES_192
        EVP_AES_192_CTR = (char *)EVP_get_cipherbyname("AES-192-CTR");
        #endif
        #ifdef WOLFSSL_AES_256
        EVP_AES_256_CTR = (char *)EVP_get_cipherbyname("AES-256-CTR");
        #endif

        #ifdef WOLFSSL_AES_128
        EVP_AES_128_ECB = (char *)EVP_get_cipherbyname("AES-128-ECB");
        #endif
        #ifdef WOLFSSL_AES_192
        EVP_AES_192_ECB = (char *)EVP_get_cipherbyname("AES-192-ECB");
        #endif
        #ifdef WOLFSSL_AES_256
        EVP_AES_256_ECB = (char *)EVP_get_cipherbyname("AES-256-ECB");
        #endif
#endif
#endif

#ifndef NO_DES3
    EVP_DES_CBC = (char *)EVP_get_cipherbyname("DES-CBC");
    EVP_DES_ECB = (char *)EVP_get_cipherbyname("DES-ECB");

    EVP_DES_EDE3_CBC = (char *)EVP_get_cipherbyname("DES-EDE3-CBC");
    EVP_DES_EDE3_ECB = (char *)EVP_get_cipherbyname("DES-EDE3-ECB");
#endif

#ifdef HAVE_IDEA
    EVP_IDEA_CBC = (char *)EVP_get_cipherbyname("IDEA-CBC");
#endif
}

#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL || HAVE_WEBSERVER */


#if !defined(NO_FILESYSTEM) && !defined(NO_STDIO_FILESYSTEM)

void wolfSSL_ERR_print_errors_fp(XFILE fp, int err)
{
    char data[WOLFSSL_MAX_ERROR_SZ + 1];

    WOLFSSL_ENTER("wolfSSL_ERR_print_errors_fp");
    SetErrorString(err, data);
    fprintf(fp, "%s", data);
}

#if defined(OPENSSL_EXTRA) || defined(DEBUG_WOLFSSL_VERBOSE)
void wolfSSL_ERR_dump_errors_fp(XFILE fp)
{
    wc_ERR_print_errors_fp(fp);
}
#endif
#endif


int wolfSSL_pending(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("SSL_pending");
    return ssl->buffers.clearOutputBuffer.length;
}


#ifndef WOLFSSL_LEANPSK
/* turn on handshake group messages for context */
int wolfSSL_CTX_set_group_messages(WOLFSSL_CTX* ctx)
{
    if (ctx == NULL)
       return BAD_FUNC_ARG;

    ctx->groupMessages = 1;

    return WOLFSSL_SUCCESS;
}
#endif


#ifndef NO_WOLFSSL_CLIENT
/* connect enough to get peer cert chain */
int wolfSSL_connect_cert(WOLFSSL* ssl)
{
    int  ret;

    if (ssl == NULL)
        return WOLFSSL_FAILURE;

    ssl->options.certOnly = 1;
    ret = wolfSSL_connect(ssl);
    ssl->options.certOnly   = 0;

    return ret;
}
#endif


#ifndef WOLFSSL_LEANPSK
/* turn on handshake group messages for ssl object */
int wolfSSL_set_group_messages(WOLFSSL* ssl)
{
    if (ssl == NULL)
       return BAD_FUNC_ARG;

    ssl->options.groupMessages = 1;

    return WOLFSSL_SUCCESS;
}


/* make minVersion the internal equivalent SSL version */
static int SetMinVersionHelper(byte* minVersion, int version)
{
#ifdef NO_TLS
    (void)minVersion;
#endif

    switch (version) {
#if defined(WOLFSSL_ALLOW_SSLV3) && !defined(NO_OLD_TLS)
        case WOLFSSL_SSLV3:
            *minVersion = SSLv3_MINOR;
            break;
#endif

#ifndef NO_TLS
    #ifndef NO_OLD_TLS
        case WOLFSSL_TLSV1:
            *minVersion = TLSv1_MINOR;
            break;

        case WOLFSSL_TLSV1_1:
            *minVersion = TLSv1_1_MINOR;
            break;
    #endif
    #ifndef WOLFSSL_NO_TLS12
        case WOLFSSL_TLSV1_2:
            *minVersion = TLSv1_2_MINOR;
            break;
    #endif
#endif
    #ifdef WOLFSSL_TLS13
        case WOLFSSL_TLSV1_3:
            *minVersion = TLSv1_3_MINOR;
            break;
    #endif

        default:
            WOLFSSL_MSG("Bad function argument");
            return BAD_FUNC_ARG;
    }

    return WOLFSSL_SUCCESS;
}


/* Set minimum downgrade version allowed, WOLFSSL_SUCCESS on ok */
int wolfSSL_CTX_SetMinVersion(WOLFSSL_CTX* ctx, int version)
{
    WOLFSSL_ENTER("wolfSSL_CTX_SetMinVersion");

    if (ctx == NULL) {
        WOLFSSL_MSG("Bad function argument");
        return BAD_FUNC_ARG;
    }

    return SetMinVersionHelper(&ctx->minDowngrade, version);
}


/* Set minimum downgrade version allowed, WOLFSSL_SUCCESS on ok */
int wolfSSL_SetMinVersion(WOLFSSL* ssl, int version)
{
    WOLFSSL_ENTER("wolfSSL_SetMinVersion");

    if (ssl == NULL) {
        WOLFSSL_MSG("Bad function argument");
        return BAD_FUNC_ARG;
    }

    return SetMinVersionHelper(&ssl->options.minDowngrade, version);
}


/* Function to get version as WOLFSSL_ enum value for wolfSSL_SetVersion */
int wolfSSL_GetVersion(WOLFSSL* ssl)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    if (ssl->version.major == SSLv3_MAJOR) {
        switch (ssl->version.minor) {
            case SSLv3_MINOR :
                return WOLFSSL_SSLV3;
            case TLSv1_MINOR :
                return WOLFSSL_TLSV1;
            case TLSv1_1_MINOR :
                return WOLFSSL_TLSV1_1;
            case TLSv1_2_MINOR :
                return WOLFSSL_TLSV1_2;
            case TLSv1_3_MINOR :
                return WOLFSSL_TLSV1_3;
            default:
                break;
        }
    }

    return VERSION_ERROR;
}

int wolfSSL_SetVersion(WOLFSSL* ssl, int version)
{
    word16 haveRSA = 1;
    word16 havePSK = 0;
    int    keySz   = 0;

    WOLFSSL_ENTER("wolfSSL_SetVersion");

    if (ssl == NULL) {
        WOLFSSL_MSG("Bad function argument");
        return BAD_FUNC_ARG;
    }

    switch (version) {
#if defined(WOLFSSL_ALLOW_SSLV3) && !defined(NO_OLD_TLS)
        case WOLFSSL_SSLV3:
            ssl->version = MakeSSLv3();
            break;
#endif

#ifndef NO_TLS
    #ifndef NO_OLD_TLS
        #ifdef WOLFSSL_ALLOW_TLSV10
        case WOLFSSL_TLSV1:
            ssl->version = MakeTLSv1();
            break;
        #endif

        case WOLFSSL_TLSV1_1:
            ssl->version = MakeTLSv1_1();
            break;
    #endif
    #ifndef WOLFSSL_NO_TLS12
        case WOLFSSL_TLSV1_2:
            ssl->version = MakeTLSv1_2();
            break;
    #endif
#endif
#ifdef WOLFSSL_TLS13
        case WOLFSSL_TLSV1_3:
            ssl->version = MakeTLSv1_3();
            break;

#endif

        default:
            WOLFSSL_MSG("Bad function argument");
            return BAD_FUNC_ARG;
    }

    #ifdef NO_RSA
        haveRSA = 0;
    #endif
    #ifndef NO_PSK
        havePSK = ssl->options.havePSK;
    #endif
    #ifndef NO_CERTS
        keySz = ssl->buffers.keySz;
    #endif

    InitSuites(ssl->suites, ssl->version, keySz, haveRSA, havePSK,
               ssl->options.haveDH, ssl->options.haveNTRU,
               ssl->options.haveECDSAsig, ssl->options.haveECC,
               ssl->options.haveStaticECC, ssl->options.side);

    return WOLFSSL_SUCCESS;
}
#endif /* !leanpsk */


#if !defined(NO_CERTS) || !defined(NO_SESSION_CACHE)

/* Make a work from the front of random hash */
static INLINE word32 MakeWordFromHash(const byte* hashID)
{
    return ((word32)hashID[0] << 24) | (hashID[1] << 16) |
        (hashID[2] <<  8) | hashID[3];
}

#endif /* !NO_CERTS || !NO_SESSION_CACHE */


#ifndef NO_CERTS

/* hash is the SHA digest of name, just use first 32 bits as hash */
static INLINE word32 HashSigner(const byte* hash)
{
    return MakeWordFromHash(hash) % CA_TABLE_SIZE;
}


/* does CA already exist on signer list */
int AlreadySigner(WOLFSSL_CERT_MANAGER* cm, byte* hash)
{
    Signer* signers;
    int     ret = 0;
    word32  row;

    if (cm == NULL || hash == NULL) {
        return ret;
    }

    row = HashSigner(hash);

    if (wc_LockMutex(&cm->caLock) != 0) {
        return ret;
    }
    signers = cm->caTable[row];
    while (signers) {
        byte* subjectHash;

    #ifndef NO_SKID
        subjectHash = signers->subjectKeyIdHash;
    #else
        subjectHash = signers->subjectNameHash;
    #endif

        if (XMEMCMP(hash, subjectHash, SIGNER_DIGEST_SIZE) == 0) {
            ret = 1; /* success */
            break;
        }
        signers = signers->next;
    }
    wc_UnLockMutex(&cm->caLock);

    return ret;
}


#ifdef WOLFSSL_TRUST_PEER_CERT
/* hash is the SHA digest of name, just use first 32 bits as hash */
static INLINE word32 TrustedPeerHashSigner(const byte* hash)
{
    return MakeWordFromHash(hash) % TP_TABLE_SIZE;
}

/* does trusted peer already exist on signer list */
int AlreadyTrustedPeer(WOLFSSL_CERT_MANAGER* cm, byte* hash)
{
    TrustedPeerCert* tp;
    int     ret = 0;
    word32  row = TrustedPeerHashSigner(hash);

    if (wc_LockMutex(&cm->tpLock) != 0)
        return  ret;
    tp = cm->tpTable[row];
    while (tp) {
        byte* subjectHash;
        #ifndef NO_SKID
            subjectHash = tp->subjectKeyIdHash;
        #else
            subjectHash = tp->subjectNameHash;
        #endif
        if (XMEMCMP(hash, subjectHash, SIGNER_DIGEST_SIZE) == 0) {
            ret = 1;
            break;
        }
        tp = tp->next;
    }
    wc_UnLockMutex(&cm->tpLock);

    return ret;
}


/* return Trusted Peer if found, otherwise NULL
    type is what to match on
 */
TrustedPeerCert* GetTrustedPeer(void* vp, byte* hash, int type)
{
    WOLFSSL_CERT_MANAGER* cm = (WOLFSSL_CERT_MANAGER*)vp;
    TrustedPeerCert* ret = NULL;
    TrustedPeerCert* tp  = NULL;
    word32  row;

    if (cm == NULL || hash == NULL)
        return NULL;

    row = TrustedPeerHashSigner(hash);

    if (wc_LockMutex(&cm->tpLock) != 0)
        return ret;

    tp = cm->tpTable[row];
    while (tp) {
        byte* subjectHash;
        switch (type) {
            #ifndef NO_SKID
            case WC_MATCH_SKID:
                subjectHash = tp->subjectKeyIdHash;
                break;
            #endif
            case WC_MATCH_NAME:
                subjectHash = tp->subjectNameHash;
                break;
            default:
                WOLFSSL_MSG("Unknown search type");
                wc_UnLockMutex(&cm->tpLock);
                return NULL;
        }
        if (XMEMCMP(hash, subjectHash, SIGNER_DIGEST_SIZE) == 0) {
            ret = tp;
            break;
        }
        tp = tp->next;
    }
    wc_UnLockMutex(&cm->tpLock);

    return ret;
}


int MatchTrustedPeer(TrustedPeerCert* tp, DecodedCert* cert)
{
    if (tp == NULL || cert == NULL)
        return BAD_FUNC_ARG;

    /* subject key id or subject hash has been compared when searching
       tpTable for the cert from function GetTrustedPeer */

    /* compare signatures */
    if (tp->sigLen == cert->sigLength) {
        if (XMEMCMP(tp->sig, cert->signature, cert->sigLength)) {
            return WOLFSSL_FAILURE;
        }
    }
    else {
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}
#endif /* WOLFSSL_TRUST_PEER_CERT */


/* return CA if found, otherwise NULL */
Signer* GetCA(void* vp, byte* hash)
{
    WOLFSSL_CERT_MANAGER* cm = (WOLFSSL_CERT_MANAGER*)vp;
    Signer* ret = NULL;
    Signer* signers;
    word32  row = HashSigner(hash);

    if (cm == NULL)
        return NULL;

    if (wc_LockMutex(&cm->caLock) != 0)
        return ret;

    signers = cm->caTable[row];
    while (signers) {
        byte* subjectHash;
        #ifndef NO_SKID
            subjectHash = signers->subjectKeyIdHash;
        #else
            subjectHash = signers->subjectNameHash;
        #endif
        if (XMEMCMP(hash, subjectHash, SIGNER_DIGEST_SIZE) == 0) {
            ret = signers;
            break;
        }
        signers = signers->next;
    }
    wc_UnLockMutex(&cm->caLock);

    return ret;
}


#ifndef NO_SKID
/* return CA if found, otherwise NULL. Walk through hash table. */
Signer* GetCAByName(void* vp, byte* hash)
{
    WOLFSSL_CERT_MANAGER* cm = (WOLFSSL_CERT_MANAGER*)vp;
    Signer* ret = NULL;
    Signer* signers;
    word32  row;

    if (cm == NULL)
        return NULL;

    if (wc_LockMutex(&cm->caLock) != 0)
        return ret;

    for (row = 0; row < CA_TABLE_SIZE && ret == NULL; row++) {
        signers = cm->caTable[row];
        while (signers && ret == NULL) {
            if (XMEMCMP(hash, signers->subjectNameHash,
                        SIGNER_DIGEST_SIZE) == 0) {
                ret = signers;
            }
            signers = signers->next;
        }
    }
    wc_UnLockMutex(&cm->caLock);

    return ret;
}
#endif


#ifdef WOLFSSL_TRUST_PEER_CERT
/* add a trusted peer cert to linked list */
int AddTrustedPeer(WOLFSSL_CERT_MANAGER* cm, DerBuffer** pDer, int verify)
{
    int ret, row;
    TrustedPeerCert* peerCert;
    DecodedCert* cert = NULL;
    DerBuffer*   der = *pDer;
    byte* subjectHash = NULL;

    WOLFSSL_MSG("Adding a Trusted Peer Cert");

    cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), cm->heap,
                                 DYNAMIC_TYPE_DCERT);
    if (cert == NULL)
        return MEMORY_E;

    InitDecodedCert(cert, der->buffer, der->length, cm->heap);
    if ((ret = ParseCert(cert, TRUSTED_PEER_TYPE, verify, cm)) != 0) {
        XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
        return ret;
    }
    WOLFSSL_MSG("\tParsed new trusted peer cert");

    peerCert = (TrustedPeerCert*)XMALLOC(sizeof(TrustedPeerCert), cm->heap,
                                                             DYNAMIC_TYPE_CERT);
    if (peerCert == NULL) {
        FreeDecodedCert(cert);
        XFREE(cert, cm->heap, DYNAMIC_TYPE_DCERT);
        return MEMORY_E;
    }
    XMEMSET(peerCert, 0, sizeof(TrustedPeerCert));

#ifndef NO_SKID
    if (cert->extAuthKeyIdSet) {
        subjectHash = cert->extSubjKeyId;
    }
    else {
        subjectHash = cert->subjectHash;
    }
#else
    subjectHash = cert->subjectHash;
#endif

    #ifndef IGNORE_NAME_CONSTRAINTS
        if (peerCert->permittedNames)
            FreeNameSubtrees(peerCert->permittedNames, cm->heap);
        if (peerCert->excludedNames)
            FreeNameSubtrees(peerCert->excludedNames, cm->heap);
    #endif

    if (AlreadyTrustedPeer(cm, subjectHash)) {
        WOLFSSL_MSG("\tAlready have this CA, not adding again");
        (void)ret;
    }
    else {
        /* add trusted peer signature */
        peerCert->sigLen = cert->sigLength;
        peerCert->sig = XMALLOC(cert->sigLength, cm->heap,
                                                        DYNAMIC_TYPE_SIGNATURE);
        if (peerCert->sig == NULL) {
            FreeDecodedCert(cert);
            XFREE(cert, cm->heap, DYNAMIC_TYPE_DCERT);
            FreeTrustedPeer(peerCert, cm->heap);
            return MEMORY_E;
        }
        XMEMCPY(peerCert->sig, cert->signature, cert->sigLength);

        /* add trusted peer name */
        peerCert->nameLen = cert->subjectCNLen;
        peerCert->name    = cert->subjectCN;
        #ifndef IGNORE_NAME_CONSTRAINTS
            peerCert->permittedNames = cert->permittedNames;
            peerCert->excludedNames  = cert->excludedNames;
        #endif

        /* add SKID when available and hash of name */
        #ifndef NO_SKID
            XMEMCPY(peerCert->subjectKeyIdHash, cert->extSubjKeyId,
                    SIGNER_DIGEST_SIZE);
        #endif
            XMEMCPY(peerCert->subjectNameHash, cert->subjectHash,
                    SIGNER_DIGEST_SIZE);
            peerCert->next    = NULL; /* If Key Usage not set, all uses valid. */
            cert->subjectCN = 0;
        #ifndef IGNORE_NAME_CONSTRAINTS
            cert->permittedNames = NULL;
            cert->excludedNames = NULL;
        #endif

        #ifndef NO_SKID
            if (cert->extAuthKeyIdSet) {
                row = TrustedPeerHashSigner(peerCert->subjectKeyIdHash);
            }
            else {
                row = TrustedPeerHashSigner(peerCert->subjectNameHash);
            }
        #else
            row = TrustedPeerHashSigner(peerCert->subjectNameHash);
        #endif

            if (wc_LockMutex(&cm->tpLock) == 0) {
                peerCert->next = cm->tpTable[row];
                cm->tpTable[row] = peerCert;   /* takes ownership */
                wc_UnLockMutex(&cm->tpLock);
            }
            else {
                WOLFSSL_MSG("\tTrusted Peer Cert Mutex Lock failed");
                FreeDecodedCert(cert);
                XFREE(cert, cm->heap, DYNAMIC_TYPE_DCERT);
                FreeTrustedPeer(peerCert, cm->heap);
                return BAD_MUTEX_E;
            }
        }

    WOLFSSL_MSG("\tFreeing parsed trusted peer cert");
    FreeDecodedCert(cert);
    XFREE(cert, cm->heap, DYNAMIC_TYPE_DCERT);
    WOLFSSL_MSG("\tFreeing der trusted peer cert");
    FreeDer(&der);
    WOLFSSL_MSG("\t\tOK Freeing der trusted peer cert");
    WOLFSSL_LEAVE("AddTrustedPeer", ret);

    return WOLFSSL_SUCCESS;
}
#endif /* WOLFSSL_TRUST_PEER_CERT */


/* owns der, internal now uses too */
/* type flag ids from user or from chain received during verify
   don't allow chain ones to be added w/o isCA extension */
int AddCA(WOLFSSL_CERT_MANAGER* cm, DerBuffer** pDer, int type, int verify)
{
    int         ret;
    Signer*     signer = 0;
    word32      row;
    byte*       subjectHash;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert* cert = NULL;
#else
    DecodedCert  cert[1];
#endif
    DerBuffer*   der = *pDer;

    WOLFSSL_MSG("Adding a CA");

#ifdef WOLFSSL_SMALL_STACK
    cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), NULL,
                                 DYNAMIC_TYPE_DCERT);
    if (cert == NULL)
        return MEMORY_E;
#endif

    InitDecodedCert(cert, der->buffer, der->length, cm->heap);
    ret = ParseCert(cert, CA_TYPE, verify, cm);
    WOLFSSL_MSG("\tParsed new CA");

#ifndef NO_SKID
    subjectHash = cert->extSubjKeyId;
#else
    subjectHash = cert->subjectHash;
#endif

    /* check CA key size */
    if (verify) {
        switch (cert->keyOID) {
            #ifndef NO_RSA
            case RSAk:
                if (cm->minRsaKeySz < 0 ||
                                   cert->pubKeySize < (word16)cm->minRsaKeySz) {
                    ret = RSA_KEY_SIZE_E;
                    WOLFSSL_MSG("\tCA RSA key size error");
                }
                break;
            #endif /* !NO_RSA */
            #ifdef HAVE_ECC
            case ECDSAk:
                if (cm->minEccKeySz < 0 ||
                                   cert->pubKeySize < (word16)cm->minEccKeySz) {
                    ret = ECC_KEY_SIZE_E;
                    WOLFSSL_MSG("\tCA ECC key size error");
                }
                break;
            #endif /* HAVE_ECC */
            #ifdef HAVE_ED25519
            case ED25519k:
                if (cm->minEccKeySz < 0 ||
                                   ED25519_KEY_SIZE < (word16)cm->minEccKeySz) {
                    ret = ECC_KEY_SIZE_E;
                    WOLFSSL_MSG("\tCA ECC key size error");
                }
                break;
            #endif /* HAVE_ED25519 */

            default:
                WOLFSSL_MSG("\tNo key size check done on CA");
                break; /* no size check if key type is not in switch */
        }
    }

    if (ret == 0 && cert->isCA == 0 && type != WOLFSSL_USER_CA) {
        WOLFSSL_MSG("\tCan't add as CA if not actually one");
        ret = NOT_CA_ERROR;
    }
#ifndef ALLOW_INVALID_CERTSIGN
    else if (ret == 0 && cert->isCA == 1 && type != WOLFSSL_USER_CA &&
             (cert->extKeyUsage & KEYUSE_KEY_CERT_SIGN) == 0) {
        /* Intermediate CA certs are required to have the keyCertSign
        * extension set. User loaded root certs are not. */
        WOLFSSL_MSG("\tDoesn't have key usage certificate signing");
        ret = NOT_CA_ERROR;
    }
#endif
    else if (ret == 0 && AlreadySigner(cm, subjectHash)) {
        WOLFSSL_MSG("\tAlready have this CA, not adding again");
        (void)ret;
    }
    else if (ret == 0) {
        /* take over signer parts */
        signer = MakeSigner(cm->heap);
        if (!signer)
            ret = MEMORY_ERROR;
        else {
            signer->keyOID         = cert->keyOID;
            if (cert->pubKeyStored) {
                signer->publicKey      = cert->publicKey;
                signer->pubKeySize     = cert->pubKeySize;
            }
            if (cert->subjectCNStored) {
                signer->nameLen        = cert->subjectCNLen;
                signer->name           = cert->subjectCN;
            }
            signer->pathLength     = cert->pathLength;
            signer->pathLengthSet  = cert->pathLengthSet;
        #ifndef IGNORE_NAME_CONSTRAINTS
            signer->permittedNames = cert->permittedNames;
            signer->excludedNames  = cert->excludedNames;
        #endif
        #ifndef NO_SKID
            XMEMCPY(signer->subjectKeyIdHash, cert->extSubjKeyId,
                    SIGNER_DIGEST_SIZE);
        #endif
            XMEMCPY(signer->subjectNameHash, cert->subjectHash,
                    SIGNER_DIGEST_SIZE);
            signer->keyUsage = cert->extKeyUsageSet ? cert->extKeyUsage
                                                    : 0xFFFF;
            signer->next    = NULL; /* If Key Usage not set, all uses valid. */
            cert->publicKey = 0;    /* in case lock fails don't free here.   */
            cert->subjectCN = 0;
        #ifndef IGNORE_NAME_CONSTRAINTS
            cert->permittedNames = NULL;
            cert->excludedNames = NULL;
        #endif

        #ifndef NO_SKID
            row = HashSigner(signer->subjectKeyIdHash);
        #else
            row = HashSigner(signer->subjectNameHash);
        #endif

            if (wc_LockMutex(&cm->caLock) == 0) {
                signer->next = cm->caTable[row];
                cm->caTable[row] = signer;   /* takes ownership */
                wc_UnLockMutex(&cm->caLock);
                if (cm->caCacheCallback)
                    cm->caCacheCallback(der->buffer, (int)der->length, type);
            }
            else {
                WOLFSSL_MSG("\tCA Mutex Lock failed");
                ret = BAD_MUTEX_E;
                FreeSigner(signer, cm->heap);
            }
        }
    }

    WOLFSSL_MSG("\tFreeing Parsed CA");
    FreeDecodedCert(cert);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
#endif
    WOLFSSL_MSG("\tFreeing der CA");
    FreeDer(pDer);
    WOLFSSL_MSG("\t\tOK Freeing der CA");

    WOLFSSL_LEAVE("AddCA", ret);

    return ret == 0 ? WOLFSSL_SUCCESS : ret;
}

#endif /* !NO_CERTS */


#ifndef NO_SESSION_CACHE

    /* basic config gives a cache with 33 sessions, adequate for clients and
       embedded servers

       MEDIUM_SESSION_CACHE allows 1055 sessions, adequate for servers that
       aren't under heavy load, basically allows 200 new sessions per minute

       BIG_SESSION_CACHE yields 20,027 sessions

       HUGE_SESSION_CACHE yields 65,791 sessions, for servers under heavy load,
       allows over 13,000 new sessions per minute or over 200 new sessions per
       second

       SMALL_SESSION_CACHE only stores 6 sessions, good for embedded clients
       or systems where the default of nearly 3kB is too much RAM, this define
       uses less than 500 bytes RAM

       default SESSION_CACHE stores 33 sessions (no XXX_SESSION_CACHE defined)
    */
    #ifdef HUGE_SESSION_CACHE
        #define SESSIONS_PER_ROW 11
        #define SESSION_ROWS 5981
    #elif defined(BIG_SESSION_CACHE)
        #define SESSIONS_PER_ROW 7
        #define SESSION_ROWS 2861
    #elif defined(MEDIUM_SESSION_CACHE)
        #define SESSIONS_PER_ROW 5
        #define SESSION_ROWS 211
    #elif defined(SMALL_SESSION_CACHE)
        #define SESSIONS_PER_ROW 2
        #define SESSION_ROWS 3
    #else
        #define SESSIONS_PER_ROW 3
        #define SESSION_ROWS 11
    #endif

    typedef struct SessionRow {
        int nextIdx;                           /* where to place next one   */
        int totalCount;                        /* sessions ever on this row */
        WOLFSSL_SESSION Sessions[SESSIONS_PER_ROW];
    } SessionRow;

    static SessionRow SessionCache[SESSION_ROWS];

    #if defined(WOLFSSL_SESSION_STATS) && defined(WOLFSSL_PEAK_SESSIONS)
        static word32 PeakSessions;
    #endif

    static wolfSSL_Mutex session_mutex;   /* SessionCache mutex */

    #ifndef NO_CLIENT_CACHE

        typedef struct ClientSession {
            word16 serverRow;            /* SessionCache Row id */
            word16 serverIdx;            /* SessionCache Idx (column) */
        } ClientSession;

        typedef struct ClientRow {
            int nextIdx;                /* where to place next one   */
            int totalCount;             /* sessions ever on this row */
            ClientSession Clients[SESSIONS_PER_ROW];
        } ClientRow;

        static ClientRow ClientCache[SESSION_ROWS];  /* Client Cache */
                                                     /* uses session mutex */
    #endif  /* NO_CLIENT_CACHE */

#endif /* NO_SESSION_CACHE */

int wolfSSL_Init(void)
{
    WOLFSSL_ENTER("wolfSSL_Init");

    if (initRefCount == 0) {
        /* Initialize crypto for use with TLS connection */
        if (wolfCrypt_Init() != 0) {
            WOLFSSL_MSG("Bad wolfCrypt Init");
            return WC_INIT_E;
        }
#ifndef NO_SESSION_CACHE
        if (wc_InitMutex(&session_mutex) != 0) {
            WOLFSSL_MSG("Bad Init Mutex session");
            return BAD_MUTEX_E;
        }
#endif
        if (wc_InitMutex(&count_mutex) != 0) {
            WOLFSSL_MSG("Bad Init Mutex count");
            return BAD_MUTEX_E;
        }
    }

    if (wc_LockMutex(&count_mutex) != 0) {
        WOLFSSL_MSG("Bad Lock Mutex count");
        return BAD_MUTEX_E;
    }

    initRefCount++;
    wc_UnLockMutex(&count_mutex);

    return WOLFSSL_SUCCESS;
}



#ifndef NO_CERTS

/* process user cert chain to pass during the handshake */
static int ProcessUserChain(WOLFSSL_CTX* ctx, const unsigned char* buff,
                         long sz, int format, int type, WOLFSSL* ssl,
                         long* used, EncryptedInfo* info)
{
    int ret = 0;
    void* heap = wolfSSL_CTX_GetHeap(ctx, ssl);
#ifdef WOLFSSL_TLS13
    int cnt = 0;
#endif

    /* we may have a user cert chain, try to consume */
    if (type == CERT_TYPE && info->consumed < sz) {
    #ifdef WOLFSSL_SMALL_STACK
        byte   staticBuffer[1];                 /* force heap usage */
    #else
        byte   staticBuffer[FILE_BUFFER_SIZE];  /* tmp chain buffer */
    #endif
        byte*  chainBuffer = staticBuffer;
        int    dynamicBuffer = 0;
        word32 bufferSz = sizeof(staticBuffer);
        long   consumed = info->consumed;
        word32 idx = 0;
        int    gotOne = 0;

        if ( (sz - consumed) > (int)bufferSz) {
            WOLFSSL_MSG("Growing Tmp Chain Buffer");
            bufferSz = (word32)(sz - consumed);
                       /* will shrink to actual size */
            chainBuffer = (byte*)XMALLOC(bufferSz, heap, DYNAMIC_TYPE_FILE);
            if (chainBuffer == NULL) {
                return MEMORY_E;
            }
            dynamicBuffer = 1;
        }

        WOLFSSL_MSG("Processing Cert Chain");
        while (consumed < sz) {
            DerBuffer* part = NULL;
            word32 remain = (word32)(sz - consumed);
            info->consumed = 0;

            if (format == WOLFSSL_FILETYPE_PEM) {
            #ifdef WOLFSSL_PEM_TO_DER
                ret = PemToDer(buff + consumed, remain, type, &part,
                               heap, info, NULL);
            #else
                ret = NOT_COMPILED_IN;
            #endif
            }
            else {
                int length = remain;
                if (format == WOLFSSL_FILETYPE_ASN1) {
                    /* get length of der (read sequence) */
                    word32 inOutIdx = 0;
                    if (GetSequence(buff + consumed, &inOutIdx, &length, remain) < 0) {
                        ret = ASN_NO_PEM_HEADER;
                    }
                    length += inOutIdx; /* include leading squence */
                }
                info->consumed = length;
                if (ret == 0) {
                    ret = AllocDer(&part, length, type, heap);
                    if (ret == 0) {
                        XMEMCPY(part->buffer, buff + consumed, length);
                    }
                }
            }
            if (ret == 0) {
                gotOne = 1;
#ifdef WOLFSSL_TLS13
                cnt++;
#endif
                if ((idx + part->length) > bufferSz) {
                    WOLFSSL_MSG("   Cert Chain bigger than buffer");
                    ret = BUFFER_E;
                }
                else {
                    c32to24(part->length, &chainBuffer[idx]);
                    idx += CERT_HEADER_SZ;
                    XMEMCPY(&chainBuffer[idx], part->buffer, part->length);
                    idx += part->length;
                    consumed  += info->consumed;
                    if (used)
                        *used += info->consumed;
                }
            }
            FreeDer(&part);

            if (ret == ASN_NO_PEM_HEADER && gotOne) {
                WOLFSSL_MSG("We got one good cert, so stuff at end ok");
                break;
            }

            if (ret < 0) {
                WOLFSSL_MSG("   Error in Cert in Chain");
                if (dynamicBuffer)
                    XFREE(chainBuffer, heap, DYNAMIC_TYPE_FILE);
                return ret;
            }
            WOLFSSL_MSG("   Consumed another Cert in Chain");
        }
        WOLFSSL_MSG("Finished Processing Cert Chain");

        /* only retain actual size used */
        ret = 0;
        if (idx > 0) {
            if (ssl) {
                if (ssl->buffers.weOwnCertChain) {
                    FreeDer(&ssl->buffers.certChain);
                }
                ret = AllocDer(&ssl->buffers.certChain, idx, type, heap);
                if (ret == 0) {
                    XMEMCPY(ssl->buffers.certChain->buffer, chainBuffer, idx);
                    ssl->buffers.weOwnCertChain = 1;
                }
#ifdef WOLFSSL_TLS13
                ssl->buffers.certChainCnt = cnt;
#endif
            } else if (ctx) {
                FreeDer(&ctx->certChain);
                ret = AllocDer(&ctx->certChain, idx, type, heap);
                if (ret == 0) {
                    XMEMCPY(ctx->certChain->buffer, chainBuffer, idx);
                }
#ifdef WOLFSSL_TLS13
                ctx->certChainCnt = cnt;
#endif
            }
        }

        if (dynamicBuffer)
            XFREE(chainBuffer, heap, DYNAMIC_TYPE_FILE);
    }

    return ret;
}
/* process the buffer buff, length sz, into ctx of format and type
   used tracks bytes consumed, userChain specifies a user cert chain
   to pass during the handshake */
int ProcessBuffer(WOLFSSL_CTX* ctx, const unsigned char* buff,
                         long sz, int format, int type, WOLFSSL* ssl,
                         long* used, int userChain)
{
    DerBuffer*    der = NULL;        /* holds DER or RAW (for NTRU) */
    int           ret = 0;
    int           eccKey = 0;
    int           ed25519Key = 0;
    int           rsaKey = 0;
    int           resetSuites = 0;
    void*         heap = wolfSSL_CTX_GetHeap(ctx, ssl);
    int           devId = wolfSSL_CTX_GetDevId(ctx, ssl);
#ifdef WOLFSSL_SMALL_STACK
    EncryptedInfo* info = NULL;
#else
    EncryptedInfo  info[1];
#endif

    (void)rsaKey;
    (void)devId;

    if (used)
        *used = sz;     /* used bytes default to sz, PEM chain may shorten*/

    /* check args */
    if (format != WOLFSSL_FILETYPE_ASN1 && format != WOLFSSL_FILETYPE_PEM
                                    && format != WOLFSSL_FILETYPE_RAW)
        return WOLFSSL_BAD_FILETYPE;

    if (ctx == NULL && ssl == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_SMALL_STACK
    info = (EncryptedInfo*)XMALLOC(sizeof(EncryptedInfo), heap,
                                   DYNAMIC_TYPE_ENCRYPTEDINFO);
    if (info == NULL)
        return MEMORY_E;
#endif

    XMEMSET(info, 0, sizeof(EncryptedInfo));
#ifdef WOLFSSL_ENCRYPTED_KEYS
    if (ctx) {
        info->passwd_cb       = ctx->passwd_cb;
        info->passwd_userdata = ctx->passwd_userdata;
    }
#endif

    if (format == WOLFSSL_FILETYPE_PEM) {
    #ifdef WOLFSSL_PEM_TO_DER
        ret = PemToDer(buff, sz, type, &der, heap, info, &eccKey);
    #else
        ret = NOT_COMPILED_IN;
    #endif
    }
    else {
        /* ASN1 (DER) or RAW (NTRU) */
        int length = (int)sz;
        if (format == WOLFSSL_FILETYPE_ASN1) {
            /* get length of der (read sequence) */
            word32 inOutIdx = 0;
            if (GetSequence(buff, &inOutIdx, &length, (word32)sz) < 0) {
                ret = ASN_PARSE_E;
            }
            length += inOutIdx; /* include leading squence */
        }

        info->consumed = length;

        if (ret == 0) {
            ret = AllocDer(&der, (word32)length, type, heap);
            if (ret == 0) {
                XMEMCPY(der->buffer, buff, length);
            }
        }
    }

    if (used) {
        *used = info->consumed;
    }

    /* process user chain */
    if (ret >= 0) {
        if (userChain) {
            ret = ProcessUserChain(ctx, buff, sz, format, type, ssl, used, info);
        }
    }

#ifdef WOLFSSL_ENCRYPTED_KEYS
    /* for WOLFSSL_FILETYPE_PEM, PemToDer manage the decryption if required */
    if (ret >= 0 && info->set && format != WOLFSSL_FILETYPE_PEM) {
        /* decrypt */
        int   passwordSz = NAME_SZ;
#ifdef WOLFSSL_SMALL_STACK
        char* password = NULL;
#else
        char  password[NAME_SZ];
#endif

    #ifdef WOLFSSL_SMALL_STACK
        password = (char*)XMALLOC(passwordSz, heap, DYNAMIC_TYPE_STRING);
        if (password == NULL)
            ret = MEMORY_E;
        else
    #endif
        if (info->passwd_cb == NULL) {
            WOLFSSL_MSG("No password callback set");
            ret = NO_PASSWORD;
        }
        else {
            ret = info->passwd_cb(password, passwordSz, PEM_PASS_READ,
                info->passwd_userdata);
            if (ret >= 0) {
                passwordSz = ret;

                /* decrypt the key */
                ret = wc_BufferKeyDecrypt(info, der->buffer, der->length,
                    (byte*)password, passwordSz, WC_MD5);

                ForceZero(password, passwordSz);
            }
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(password, heap, DYNAMIC_TYPE_STRING);
    #endif
    }
#endif /* WOLFSSL_ENCRYPTED_KEYS */

#ifdef WOLFSSL_SMALL_STACK
    XFREE(info, heap, DYNAMIC_TYPE_ENCRYPTEDINFO);
#endif

    /* check for error */
    if (ret < 0) {
        FreeDer(&der);
        return ret;
    }

    /* Handle DER owner */
    if (type == CA_TYPE) {
        if (ctx == NULL) {
            WOLFSSL_MSG("Need context for CA load");
            FreeDer(&der);
            return BAD_FUNC_ARG;
        }
        /* verify CA unless user set to no verify */
        return AddCA(ctx->cm, &der, WOLFSSL_USER_CA, !ctx->verifyNone);
    }
#ifdef WOLFSSL_TRUST_PEER_CERT
    else if (type == TRUSTED_PEER_TYPE) {
        if (ctx == NULL) {
            WOLFSSL_MSG("Need context for trusted peer cert load");
            FreeDer(&der);
            return BAD_FUNC_ARG;
        }
        /* add trusted peer cert */
        return AddTrustedPeer(ctx->cm, &der, !ctx->verifyNone);
    }
#endif /* WOLFSSL_TRUST_PEER_CERT */
    else if (type == CERT_TYPE) {
        if (ssl) {
             /* Make sure previous is free'd */
            if (ssl->buffers.weOwnCert) {
                FreeDer(&ssl->buffers.certificate);
            #ifdef KEEP_OUR_CERT
                FreeX509(ssl->ourCert);
                if (ssl->ourCert) {
                    XFREE(ssl->ourCert, ssl->heap, DYNAMIC_TYPE_X509);
                    ssl->ourCert = NULL;
                }
            #endif
            }
            ssl->buffers.certificate = der;
        #ifdef KEEP_OUR_CERT
            ssl->keepCert = 1; /* hold cert for ssl lifetime */
        #endif
            ssl->buffers.weOwnCert = 1;
        }
        else if (ctx) {
            FreeDer(&ctx->certificate); /* Make sure previous is free'd */
        #ifdef KEEP_OUR_CERT
            if (ctx->ourCert) {
                if (ctx->ownOurCert) {
                    FreeX509(ctx->ourCert);
                    XFREE(ctx->ourCert, ctx->heap, DYNAMIC_TYPE_X509);
                }
                ctx->ourCert = NULL;
            }
        #endif
            ctx->certificate = der;
        }
    }
    else if (type == PRIVATEKEY_TYPE) {
        if (ssl) {
             /* Make sure previous is free'd */
            if (ssl->buffers.weOwnKey) {
                FreeDer(&ssl->buffers.key);
            }
            ssl->buffers.key = der;
            ssl->buffers.weOwnKey = 1;
        }
        else if (ctx) {
            FreeDer(&ctx->privateKey);
            ctx->privateKey = der;
        }
    }
    else {
        FreeDer(&der);
        return WOLFSSL_BAD_CERTTYPE;
    }

    if (type == PRIVATEKEY_TYPE && format != WOLFSSL_FILETYPE_RAW) {
    #ifndef NO_RSA
        if (!eccKey && !ed25519Key) {
            /* make sure RSA key can be used */
            word32 idx = 0;
        #ifdef WOLFSSL_SMALL_STACK
            RsaKey* key = NULL;
        #else
            RsaKey  key[1];
        #endif

        #ifdef WOLFSSL_SMALL_STACK
            key = (RsaKey*)XMALLOC(sizeof(RsaKey), heap, DYNAMIC_TYPE_RSA);
            if (key == NULL)
                return MEMORY_E;
        #endif

            ret = wc_InitRsaKey_ex(key, heap, devId);
            if (ret == 0) {
                if (wc_RsaPrivateKeyDecode(der->buffer, &idx, key, der->length)
                    != 0) {
                #ifdef HAVE_ECC
                    /* could have DER ECC (or pkcs8 ecc), no easy way to tell */
                    eccKey = 1;  /* try it next */
                #elif defined(HAVE_ED25519)
                    ed25519Key = 1; /* try it next */
                #else
                    WOLFSSL_MSG("RSA decode failed and ECC not enabled to try");
                    ret = WOLFSSL_BAD_FILE;
                #endif
                }
                else {
                    /* check that the size of the RSA key is enough */
                    int rsaSz = wc_RsaEncryptSize((RsaKey*)key);
                    int minRsaSz;

                    minRsaSz = ssl ? ssl->options.minRsaKeySz : ctx->minRsaKeySz;
                    if (rsaSz < minRsaSz) {
                        ret = RSA_KEY_SIZE_E;
                        WOLFSSL_MSG("Private Key size too small");
                    }

                    if (ssl) {
                        ssl->buffers.keyType = rsa_sa_algo;
                        ssl->buffers.keySz = rsaSz;
                    }
                    else if(ctx) {
                        ctx->privateKeyType = rsa_sa_algo;
                        ctx->privateKeySz = rsaSz;
                    }

                    rsaKey = 1;
                    (void)rsaKey;  /* for no ecc builds */

                    if (ssl && ssl->options.side == WOLFSSL_SERVER_END) {
                        ssl->options.haveStaticECC = 0;
                        resetSuites = 1;
                    }
                }

                wc_FreeRsaKey(key);
            }

        #ifdef WOLFSSL_SMALL_STACK
            XFREE(key, heap, DYNAMIC_TYPE_RSA);
        #endif

            if (ret != 0)
                return ret;
        }
    #endif
    #ifdef HAVE_ECC
        if (!rsaKey && !ed25519Key) {
            /* make sure ECC key can be used */
            word32  idx = 0;
            ecc_key key;

            if (wc_ecc_init_ex(&key, heap, devId) == 0) {
                if (wc_EccPrivateKeyDecode(der->buffer, &idx, &key,
                                                            der->length) == 0) {
                    int keySz = wc_ecc_size(&key);
                    int minKeySz;

                    /* check for minimum ECC key size and then free */
                    minKeySz = ssl ? ssl->options.minEccKeySz : ctx->minEccKeySz;
                    if (keySz < minKeySz) {
                        wc_ecc_free(&key);
                        WOLFSSL_MSG("ECC private key too small");
                        return ECC_KEY_SIZE_E;
                    }

                    eccKey = 1;
                    if (ssl) {
                        ssl->options.haveStaticECC = 1;
                        ssl->buffers.keyType = ecc_dsa_sa_algo;
                        ssl->buffers.keySz = keySz;
                    }
                    else if (ctx) {
                        ctx->haveStaticECC = 1;
                        ctx->privateKeyType = ecc_dsa_sa_algo;
                        ctx->privateKeySz = keySz;
                    }

                    if (ssl && ssl->options.side == WOLFSSL_SERVER_END) {
                        resetSuites = 1;
                    }
                }
                else
                    eccKey = 0;

                wc_ecc_free(&key);
            }
        }
    #endif /* HAVE_ECC */
    #ifdef HAVE_ED25519
        if (!rsaKey && !eccKey) {
            /* make sure Ed25519 key can be used */
            word32      idx = 0;
            ed25519_key key;
            const int keySz = ED25519_KEY_SIZE;
            int minKeySz;

            ret = wc_ed25519_init(&key);
            if (ret != 0) {
                return ret;
            }

            if (wc_Ed25519PrivateKeyDecode(der->buffer, &idx, &key,
                                                            der->length) != 0) {
                wc_ed25519_free(&key);
                return WOLFSSL_BAD_FILE;
            }

            /* check for minimum key size and then free */
            minKeySz = ssl ? ssl->options.minEccKeySz : ctx->minEccKeySz;
            if (keySz < minKeySz) {
                wc_ed25519_free(&key);
                WOLFSSL_MSG("ED25519 private key too small");
                return ECC_KEY_SIZE_E;
            }

            if (ssl) {
                ssl->buffers.keyType = ed25519_sa_algo;
                ssl->buffers.keySz = keySz;
            }
            else if (ctx) {
                ctx->privateKeyType = ed25519_sa_algo;
                ctx->privateKeySz = keySz;
            }

            wc_ed25519_free(&key);
            ed25519Key = 1;

            if (ssl && ssl->options.side == WOLFSSL_SERVER_END) {
                resetSuites = 1;
            }
        }
    #else
        if (!rsaKey && !eccKey && !ed25519Key)
            return WOLFSSL_BAD_FILE;
    #endif
        (void)ed25519Key;
        (void)devId;
    }
    else if (type == CERT_TYPE) {
    #ifdef WOLFSSL_SMALL_STACK
        DecodedCert* cert = NULL;
    #else
        DecodedCert  cert[1];
    #endif
    #ifdef HAVE_PK_CALLBACKS
        int keyType = 0, keySz = 0;
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), heap,
                                     DYNAMIC_TYPE_DCERT);
        if (cert == NULL)
            return MEMORY_E;
    #endif

        WOLFSSL_MSG("Checking cert signature type");
        InitDecodedCert(cert, der->buffer, der->length, heap);

        if (DecodeToKey(cert, 0) < 0) {
            WOLFSSL_MSG("Decode to key failed");
            FreeDecodedCert(cert);
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(cert, heap, DYNAMIC_TYPE_DCERT);
        #endif
            return WOLFSSL_BAD_FILE;
        }

        if (ssl && ssl->options.side == WOLFSSL_SERVER_END) {
            resetSuites = 1;
        }
        if (ssl && ssl->ctx->haveECDSAsig) {
            WOLFSSL_MSG("SSL layer setting cert, CTX had ECDSA, turning off");
            ssl->options.haveECDSAsig = 0;   /* may turn back on next */
        }

        switch (cert->signatureOID) {
            case CTC_SHAwECDSA:
            case CTC_SHA256wECDSA:
            case CTC_SHA384wECDSA:
            case CTC_SHA512wECDSA:
                WOLFSSL_MSG("ECDSA cert signature");
                if (ssl)
                    ssl->options.haveECDSAsig = 1;
                else if (ctx)
                    ctx->haveECDSAsig = 1;
                break;
            case CTC_ED25519:
                WOLFSSL_MSG("ED25519 cert signature");
                if (ssl)
                    ssl->options.haveECDSAsig = 1;
                else if (ctx)
                    ctx->haveECDSAsig = 1;
                break;
            default:
                WOLFSSL_MSG("Not ECDSA cert signature");
                break;
        }

    #ifdef HAVE_ECC
        if (ssl) {
            ssl->pkCurveOID = cert->pkCurveOID;
        #ifndef WC_STRICT_SIG
            if (cert->keyOID == ECDSAk) {
                ssl->options.haveECC = 1;
            }
            #ifdef HAVE_ED25519
                else if (cert->keyOID == ED25519k) {
                    ssl->options.haveECC = 1;
                }
            #endif
        #else
            ssl->options.haveECC = ssl->options.haveECDSAsig;
        #endif
        }
        else if (ctx) {
            ctx->pkCurveOID = cert->pkCurveOID;
        #ifndef WC_STRICT_SIG
            if (cert->keyOID == ECDSAk) {
                ctx->haveECC = 1;
            }
            #ifdef HAVE_ED25519
                else if (cert->keyOID == ED25519k) {
                    ctx->haveECC = 1;
                }
            #endif
        #else
            ctx->haveECC = ctx->haveECDSAsig;
        #endif
        }
    #endif

        /* check key size of cert unless specified not to */
        switch (cert->keyOID) {
        #ifndef NO_RSA
            case RSAk:
                if (ssl && !ssl->options.verifyNone) {
                    if (ssl->options.minRsaKeySz < 0 ||
                          cert->pubKeySize < (word16)ssl->options.minRsaKeySz) {
                        ret = RSA_KEY_SIZE_E;
                        WOLFSSL_MSG("Certificate RSA key size too small");
                    }
                }
                else if (ctx && !ctx->verifyNone) {
                    if (ctx->minRsaKeySz < 0 ||
                                  cert->pubKeySize < (word16)ctx->minRsaKeySz) {
                        ret = RSA_KEY_SIZE_E;
                        WOLFSSL_MSG("Certificate RSA key size too small");
                    }
                }
            #ifdef HAVE_PK_CALLBACKS
                keyType = rsa_sa_algo;
                /* pubKeySize is the encoded public key */
                /* mask lsb 5-bits to round by 16 to get actual key size */
                keySz = cert->pubKeySize & ~0x1FL;
            #endif
                break;
        #endif /* !NO_RSA */
        #ifdef HAVE_ECC
            case ECDSAk:
                if (ssl && !ssl->options.verifyNone) {
                    if (ssl->options.minEccKeySz < 0 ||
                          cert->pubKeySize < (word16)ssl->options.minEccKeySz) {
                        ret = ECC_KEY_SIZE_E;
                        WOLFSSL_MSG("Certificate ECC key size error");
                    }
                }
                else if (ctx && !ctx->verifyNone) {
                    if (ctx->minEccKeySz < 0 ||
                                  cert->pubKeySize < (word16)ctx->minEccKeySz) {
                        ret = ECC_KEY_SIZE_E;
                        WOLFSSL_MSG("Certificate ECC key size error");
                    }
                }
            #ifdef HAVE_PK_CALLBACKS
                keyType = ecc_dsa_sa_algo;
                /* pubKeySize is encByte + x + y */
                keySz = (cert->pubKeySize - 1) / 2;
            #endif
                break;
        #endif /* HAVE_ECC */
        #ifdef HAVE_ED25519
            case ED25519k:
                if (ssl && !ssl->options.verifyNone) {
                    if (ssl->options.minEccKeySz < 0 ||
                          ED25519_KEY_SIZE < (word16)ssl->options.minEccKeySz) {
                        ret = ECC_KEY_SIZE_E;
                        WOLFSSL_MSG("Certificate Ed key size error");
                    }
                }
                else if (ctx && !ctx->verifyNone) {
                    if (ctx->minEccKeySz < 0 ||
                                  ED25519_KEY_SIZE < (word16)ctx->minEccKeySz) {
                        ret = ECC_KEY_SIZE_E;
                        WOLFSSL_MSG("Certificate ECC key size error");
                    }
                }
            #ifdef HAVE_PK_CALLBACKS
                keyType = ed25519_sa_algo;
                keySz = ED25519_KEY_SIZE;
            #endif
                break;
        #endif /* HAVE_ED25519 */

            default:
                WOLFSSL_MSG("No key size check done on certificate");
                break; /* do no check if not a case for the key */
        }

    #ifdef HAVE_PK_CALLBACKS
        if (ssl && ssl->buffers.keyType == 0) {
            ssl->buffers.keyType = keyType;
            ssl->buffers.keySz = keySz;
        }
        else if (ctx && ctx->privateKeyType == 0) {
            ctx->privateKeyType = keyType;
            ctx->privateKeySz = keySz;
        }
    #endif

        FreeDecodedCert(cert);
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(cert, heap, DYNAMIC_TYPE_DCERT);
    #endif

        if (ret != 0) {
            return ret;
        }
    }

    if (ssl && resetSuites) {
        word16 havePSK = 0;
        word16 haveRSA = 0;
        int    keySz   = 0;

        #ifndef NO_PSK
        if (ssl->options.havePSK) {
            havePSK = 1;
        }
        #endif
        #ifndef NO_RSA
            haveRSA = 1;
        #endif
        #ifndef NO_CERTS
            keySz = ssl->buffers.keySz;
        #endif

        /* let's reset suites */
        InitSuites(ssl->suites, ssl->version, keySz, haveRSA,
                   havePSK, ssl->options.haveDH, ssl->options.haveNTRU,
                   ssl->options.haveECDSAsig, ssl->options.haveECC,
                   ssl->options.haveStaticECC, ssl->options.side);
    }

    return WOLFSSL_SUCCESS;
}


/* CA PEM file for verification, may have multiple/chain certs to process */
static int ProcessChainBuffer(WOLFSSL_CTX* ctx, const unsigned char* buff,
                            long sz, int format, int type, WOLFSSL* ssl)
{
    long used   = 0;
    int  ret    = 0;
    int  gotOne = 0;

    WOLFSSL_MSG("Processing CA PEM file");
    while (used < sz) {
        long consumed = 0;

        ret = ProcessBuffer(ctx, buff + used, sz - used, format, type, ssl,
                            &consumed, 0);

#ifdef WOLFSSL_WPAS
#ifdef HAVE_CRL
        if (ret < 0) {
            DerBuffer*    der = NULL;
            EncryptedInfo info;

            WOLFSSL_MSG("Trying a CRL");
            if (PemToDer(buff + used, sz - used, CRL_TYPE, &der, NULL, &info,
                                                                   NULL) == 0) {
                WOLFSSL_MSG("   Proccessed a CRL");
                wolfSSL_CertManagerLoadCRLBuffer(ctx->cm, der->buffer,
                                                der->length, WOLFSSL_FILETYPE_ASN1);
                FreeDer(&der);
                used += info.consumed;
                continue;
            }
        }
#endif
#endif
        if (ret < 0)
        {
            if(consumed > 0) { /* Made progress in file */
                WOLFSSL_ERROR(ret);
                WOLFSSL_MSG("CA Parse failed, with progress in file.");
                WOLFSSL_MSG("Search for other certs in file");
            } else {
                WOLFSSL_MSG("CA Parse failed, no progress in file.");
                WOLFSSL_MSG("Do not continue search for other certs in file");
                break;
            }
        } else {
            WOLFSSL_MSG("   Processed a CA");
            gotOne = 1;
        }
        used += consumed;
    }

    if(gotOne)
    {
        WOLFSSL_MSG("Processed at least one valid CA. Other stuff OK");
        return WOLFSSL_SUCCESS;
    }
    return ret;
}


static INLINE WOLFSSL_METHOD* cm_pick_method(void)
{
    #ifndef NO_WOLFSSL_CLIENT
        #if defined(WOLFSSL_ALLOW_SSLV3) && !defined(NO_OLD_TLS)
            return wolfSSLv3_client_method();
        #elif !defined(WOLFSSL_NO_TLS12)
            return wolfTLSv1_2_client_method();
        #elif defined(WOLFSSL_TLS13)
            return wolfTLSv1_3_client_method();
        #endif
    #elif !defined(NO_WOLFSSL_SERVER)
        #if defined(WOLFSSL_ALLOW_SSLV3) && !defined(NO_OLD_TLS)
            return wolfSSLv3_server_method();
        #elif !defined(WOLFSSL_NO_TLS12)
            return wolfTLSv1_2_server_method();
        #elif defined(WOLFSSL_TLS13)
            return wolfTLSv1_3_server_method();
        #endif
    #else
        return NULL;
    #endif
}


/* like load verify locations, 1 for success, < 0 for error */
int wolfSSL_CertManagerLoadCABuffer(WOLFSSL_CERT_MANAGER* cm,
                                   const unsigned char* in, long sz, int format)
{
    int ret = WOLFSSL_FATAL_ERROR;
    WOLFSSL_CTX* tmp;

    WOLFSSL_ENTER("wolfSSL_CertManagerLoadCABuffer");

    if (cm == NULL) {
        WOLFSSL_MSG("No CertManager error");
        return ret;
    }
    tmp = wolfSSL_CTX_new(cm_pick_method());

    if (tmp == NULL) {
        WOLFSSL_MSG("CTX new failed");
        return ret;
    }

    /* for tmp use */
    wolfSSL_CertManagerFree(tmp->cm);
    tmp->cm = cm;

    ret = wolfSSL_CTX_load_verify_buffer(tmp, in, sz, format);

    /* don't loose our good one */
    tmp->cm = NULL;
    wolfSSL_CTX_free(tmp);

    return ret;
}

#ifdef HAVE_CRL

int wolfSSL_CertManagerLoadCRLBuffer(WOLFSSL_CERT_MANAGER* cm,
                                   const unsigned char* buff, long sz, int type)
{
    WOLFSSL_ENTER("wolfSSL_CertManagerLoadCRLBuffer");
    if (cm == NULL)
        return BAD_FUNC_ARG;

    if (cm->crl == NULL) {
        if (wolfSSL_CertManagerEnableCRL(cm, 0) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("Enable CRL failed");
            return WOLFSSL_FATAL_ERROR;
        }
    }

    return BufferLoadCRL(cm->crl, buff, sz, type, 0);
}


int wolfSSL_CTX_LoadCRLBuffer(WOLFSSL_CTX* ctx, const unsigned char* buff,
                              long sz, int type)
{
    WOLFSSL_ENTER("wolfSSL_CTX_LoadCRLBuffer");

    if (ctx == NULL)
        return BAD_FUNC_ARG;

    return wolfSSL_CertManagerLoadCRLBuffer(ctx->cm, buff, sz, type);
}


int wolfSSL_LoadCRLBuffer(WOLFSSL* ssl, const unsigned char* buff,
                          long sz, int type)
{
    WOLFSSL_ENTER("wolfSSL_LoadCRLBuffer");

    if (ssl == NULL || ssl->ctx == NULL)
        return BAD_FUNC_ARG;

    return wolfSSL_CertManagerLoadCRLBuffer(ssl->ctx->cm, buff, sz, type);
}


#endif /* HAVE_CRL */

/* turn on CRL if off and compiled in, set options */
int wolfSSL_CertManagerEnableCRL(WOLFSSL_CERT_MANAGER* cm, int options)
{
    int ret = WOLFSSL_SUCCESS;

    (void)options;

    WOLFSSL_ENTER("wolfSSL_CertManagerEnableCRL");
    if (cm == NULL)
        return BAD_FUNC_ARG;

    #ifdef HAVE_CRL
        if (cm->crl == NULL) {
            cm->crl = (WOLFSSL_CRL*)XMALLOC(sizeof(WOLFSSL_CRL), cm->heap,
                                            DYNAMIC_TYPE_CRL);
            if (cm->crl == NULL)
                return MEMORY_E;

            if (InitCRL(cm->crl, cm) != 0) {
                WOLFSSL_MSG("Init CRL failed");
                FreeCRL(cm->crl, 1);
                cm->crl = NULL;
                return WOLFSSL_FAILURE;
            }

        #ifdef HAVE_CRL_IO
            cm->crl->crlIOCb = EmbedCrlLookup;
        #endif
        }

        cm->crlEnabled = 1;
        if (options & WOLFSSL_CRL_CHECKALL)
            cm->crlCheckAll = 1;
    #else
        ret = NOT_COMPILED_IN;
    #endif

    return ret;
}


int wolfSSL_CertManagerDisableCRL(WOLFSSL_CERT_MANAGER* cm)
{
    WOLFSSL_ENTER("wolfSSL_CertManagerDisableCRL");
    if (cm == NULL)
        return BAD_FUNC_ARG;

    cm->crlEnabled = 0;

    return WOLFSSL_SUCCESS;
}
/* Verify the certificate, WOLFSSL_SUCCESS for ok, < 0 for error */
int wolfSSL_CertManagerVerifyBuffer(WOLFSSL_CERT_MANAGER* cm, const byte* buff,
                                    long sz, int format)
{
    int ret = 0;
    DerBuffer* der = NULL;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert* cert = NULL;
#else
    DecodedCert  cert[1];
#endif

    WOLFSSL_ENTER("wolfSSL_CertManagerVerifyBuffer");

#ifdef WOLFSSL_SMALL_STACK
    cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), cm->heap,
                                 DYNAMIC_TYPE_DCERT);
    if (cert == NULL)
        return MEMORY_E;
#endif

    if (format == WOLFSSL_FILETYPE_PEM) {
#ifdef WOLFSSL_PEM_TO_DER
        ret = PemToDer(buff, sz, CERT_TYPE, &der, cm->heap, NULL, NULL);
        if (ret != 0) {
            FreeDer(&der);
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(cert, cm->heap, DYNAMIC_TYPE_DCERT);
        #endif
            return ret;
        }
        InitDecodedCert(cert, der->buffer, der->length, cm->heap);
#else
        ret = NOT_COMPILED_IN;
#endif
    }
    else {
        InitDecodedCert(cert, (byte*)buff, (word32)sz, cm->heap);
    }

    if (ret == 0)
        ret = ParseCertRelative(cert, CERT_TYPE, 1, cm);

#ifdef HAVE_CRL
    if (ret == 0 && cm->crlEnabled)
        ret = CheckCertCRL(cm->crl, cert);
#endif

    FreeDecodedCert(cert);
    FreeDer(&der);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(cert, cm->heap, DYNAMIC_TYPE_DCERT);
#endif

    return ret == 0 ? WOLFSSL_SUCCESS : ret;
}


/* turn on OCSP if off and compiled in, set options */
int wolfSSL_CertManagerEnableOCSP(WOLFSSL_CERT_MANAGER* cm, int options)
{
    int ret = WOLFSSL_SUCCESS;

    (void)options;

    WOLFSSL_ENTER("wolfSSL_CertManagerEnableOCSP");
    if (cm == NULL)
        return BAD_FUNC_ARG;

    #ifdef HAVE_OCSP
        if (cm->ocsp == NULL) {
            cm->ocsp = (WOLFSSL_OCSP*)XMALLOC(sizeof(WOLFSSL_OCSP), cm->heap,
                                              DYNAMIC_TYPE_OCSP);
            if (cm->ocsp == NULL)
                return MEMORY_E;

            if (InitOCSP(cm->ocsp, cm) != 0) {
                WOLFSSL_MSG("Init OCSP failed");
                FreeOCSP(cm->ocsp, 1);
                cm->ocsp = NULL;
                return WOLFSSL_FAILURE;
            }
        }
        cm->ocspEnabled = 1;
        if (options & WOLFSSL_OCSP_URL_OVERRIDE)
            cm->ocspUseOverrideURL = 1;
        if (options & WOLFSSL_OCSP_NO_NONCE)
            cm->ocspSendNonce = 0;
        else
            cm->ocspSendNonce = 1;
        if (options & WOLFSSL_OCSP_CHECKALL)
            cm->ocspCheckAll = 1;
        #ifndef WOLFSSL_USER_IO
            cm->ocspIOCb = EmbedOcspLookup;
            cm->ocspRespFreeCb = EmbedOcspRespFree;
            cm->ocspIOCtx = cm->heap;
        #endif /* WOLFSSL_USER_IO */
    #else
        ret = NOT_COMPILED_IN;
    #endif

    return ret;
}


int wolfSSL_CertManagerDisableOCSP(WOLFSSL_CERT_MANAGER* cm)
{
    WOLFSSL_ENTER("wolfSSL_CertManagerDisableOCSP");
    if (cm == NULL)
        return BAD_FUNC_ARG;

    cm->ocspEnabled = 0;

    return WOLFSSL_SUCCESS;
}

/* turn on OCSP Stapling if off and compiled in, set options */
int wolfSSL_CertManagerEnableOCSPStapling(WOLFSSL_CERT_MANAGER* cm)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_CertManagerEnableOCSPStapling");

    if (cm == NULL)
        return BAD_FUNC_ARG;

#if defined(HAVE_CERTIFICATE_STATUS_REQUEST) \
 || defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
    if (cm->ocsp_stapling == NULL) {
        cm->ocsp_stapling = (WOLFSSL_OCSP*)XMALLOC(sizeof(WOLFSSL_OCSP),
                                               cm->heap, DYNAMIC_TYPE_OCSP);
        if (cm->ocsp_stapling == NULL)
            return MEMORY_E;

        if (InitOCSP(cm->ocsp_stapling, cm) != 0) {
            WOLFSSL_MSG("Init OCSP failed");
            FreeOCSP(cm->ocsp_stapling, 1);
            cm->ocsp_stapling = NULL;
            return WOLFSSL_FAILURE;
        }
    }
    cm->ocspStaplingEnabled = 1;

    #ifndef WOLFSSL_USER_IO
        cm->ocspIOCb = EmbedOcspLookup;
        cm->ocspRespFreeCb = EmbedOcspRespFree;
        cm->ocspIOCtx = cm->heap;
    #endif /* WOLFSSL_USER_IO */
#else
    ret = NOT_COMPILED_IN;
#endif

    return ret;
}

int wolfSSL_CertManagerDisableOCSPStapling(WOLFSSL_CERT_MANAGER* cm)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_CertManagerDisableOCSPStapling");

    if (cm == NULL)
        return BAD_FUNC_ARG;

#if defined(HAVE_CERTIFICATE_STATUS_REQUEST) \
 || defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
    cm->ocspStaplingEnabled = 0;
#else
    ret = NOT_COMPILED_IN;
#endif
    return ret;
}
#if defined(SESSION_CERTS)
WOLF_STACK_OF(WOLFSSL_X509)* wolfSSL_get_peer_cert_chain(const WOLFSSL* ssl)
{
    WOLFSSL_ENTER("wolfSSL_get_peer_cert_chain");
    if ((ssl == NULL) || (ssl->session.chain.count == 0))
        return NULL;
    else
        return (WOLF_STACK_OF(WOLFSSL_X509)* )&ssl->session.chain;
}
#endif
#ifdef HAVE_OCSP

/* check CRL if enabled, WOLFSSL_SUCCESS  */
int wolfSSL_CertManagerCheckOCSP(WOLFSSL_CERT_MANAGER* cm, byte* der, int sz)
{
    int ret;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert* cert = NULL;
#else
    DecodedCert  cert[1];
#endif

    WOLFSSL_ENTER("wolfSSL_CertManagerCheckOCSP");

    if (cm == NULL)
        return BAD_FUNC_ARG;

    if (cm->ocspEnabled == 0)
        return WOLFSSL_SUCCESS;

#ifdef WOLFSSL_SMALL_STACK
    cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), NULL, DYNAMIC_TYPE_DCERT);
    if (cert == NULL)
        return MEMORY_E;
#endif

    InitDecodedCert(cert, der, sz, NULL);

    if ((ret = ParseCertRelative(cert, CERT_TYPE, VERIFY_OCSP, cm)) != 0) {
        WOLFSSL_MSG("ParseCert failed");
    }
    else if ((ret = CheckCertOCSP(cm->ocsp, cert, NULL)) != 0) {
        WOLFSSL_MSG("CheckCertOCSP failed");
    }

    FreeDecodedCert(cert);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
#endif

    return ret == 0 ? WOLFSSL_SUCCESS : ret;
}


int wolfSSL_CertManagerSetOCSPOverrideURL(WOLFSSL_CERT_MANAGER* cm,
                                          const char* url)
{
    WOLFSSL_ENTER("wolfSSL_CertManagerSetOCSPOverrideURL");
    if (cm == NULL)
        return BAD_FUNC_ARG;

    XFREE(cm->ocspOverrideURL, cm->heap, DYNAMIC_TYPE_URL);
    if (url != NULL) {
        int urlSz = (int)XSTRLEN(url) + 1;
        cm->ocspOverrideURL = (char*)XMALLOC(urlSz, cm->heap, DYNAMIC_TYPE_URL);
        if (cm->ocspOverrideURL != NULL) {
            XMEMCPY(cm->ocspOverrideURL, url, urlSz);
        }
        else
            return MEMORY_E;
    }
    else
        cm->ocspOverrideURL = NULL;

    return WOLFSSL_SUCCESS;
}


int wolfSSL_CertManagerSetOCSP_Cb(WOLFSSL_CERT_MANAGER* cm,
                        CbOCSPIO ioCb, CbOCSPRespFree respFreeCb, void* ioCbCtx)
{
    WOLFSSL_ENTER("wolfSSL_CertManagerSetOCSP_Cb");
    if (cm == NULL)
        return BAD_FUNC_ARG;

    cm->ocspIOCb = ioCb;
    cm->ocspRespFreeCb = respFreeCb;
    cm->ocspIOCtx = ioCbCtx;

    return WOLFSSL_SUCCESS;
}


int wolfSSL_EnableOCSP(WOLFSSL* ssl, int options)
{
    WOLFSSL_ENTER("wolfSSL_EnableOCSP");
    if (ssl)
        return wolfSSL_CertManagerEnableOCSP(ssl->ctx->cm, options);
    else
        return BAD_FUNC_ARG;
}

int wolfSSL_DisableOCSP(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("wolfSSL_DisableOCSP");
    if (ssl)
        return wolfSSL_CertManagerDisableOCSP(ssl->ctx->cm);
    else
        return BAD_FUNC_ARG;
}


int wolfSSL_EnableOCSPStapling(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("wolfSSL_EnableOCSPStapling");
    if (ssl)
        return wolfSSL_CertManagerEnableOCSPStapling(ssl->ctx->cm);
    else
        return BAD_FUNC_ARG;
}

int wolfSSL_DisableOCSPStapling(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("wolfSSL_DisableOCSPStapling");
    if (ssl)
        return wolfSSL_CertManagerDisableOCSPStapling(ssl->ctx->cm);
    else
        return BAD_FUNC_ARG;
}

int wolfSSL_SetOCSP_OverrideURL(WOLFSSL* ssl, const char* url)
{
    WOLFSSL_ENTER("wolfSSL_SetOCSP_OverrideURL");
    if (ssl)
        return wolfSSL_CertManagerSetOCSPOverrideURL(ssl->ctx->cm, url);
    else
        return BAD_FUNC_ARG;
}


int wolfSSL_SetOCSP_Cb(WOLFSSL* ssl,
                        CbOCSPIO ioCb, CbOCSPRespFree respFreeCb, void* ioCbCtx)
{
    WOLFSSL_ENTER("wolfSSL_SetOCSP_Cb");
    if (ssl) {
        ssl->ocspIOCtx = ioCbCtx; /* use SSL specific ioCbCtx */
        return wolfSSL_CertManagerSetOCSP_Cb(ssl->ctx->cm,
                                             ioCb, respFreeCb, NULL);
    }
    else
        return BAD_FUNC_ARG;
}


int wolfSSL_CTX_EnableOCSP(WOLFSSL_CTX* ctx, int options)
{
    WOLFSSL_ENTER("wolfSSL_CTX_EnableOCSP");
    if (ctx)
        return wolfSSL_CertManagerEnableOCSP(ctx->cm, options);
    else
        return BAD_FUNC_ARG;
}


int wolfSSL_CTX_DisableOCSP(WOLFSSL_CTX* ctx)
{
    WOLFSSL_ENTER("wolfSSL_CTX_DisableOCSP");
    if (ctx)
        return wolfSSL_CertManagerDisableOCSP(ctx->cm);
    else
        return BAD_FUNC_ARG;
}


int wolfSSL_CTX_SetOCSP_OverrideURL(WOLFSSL_CTX* ctx, const char* url)
{
    WOLFSSL_ENTER("wolfSSL_SetOCSP_OverrideURL");
    if (ctx)
        return wolfSSL_CertManagerSetOCSPOverrideURL(ctx->cm, url);
    else
        return BAD_FUNC_ARG;
}


int wolfSSL_CTX_SetOCSP_Cb(WOLFSSL_CTX* ctx, CbOCSPIO ioCb,
                           CbOCSPRespFree respFreeCb, void* ioCbCtx)
{
    WOLFSSL_ENTER("wolfSSL_CTX_SetOCSP_Cb");
    if (ctx)
        return wolfSSL_CertManagerSetOCSP_Cb(ctx->cm, ioCb,
                                             respFreeCb, ioCbCtx);
    else
        return BAD_FUNC_ARG;
}

#if defined(HAVE_CERTIFICATE_STATUS_REQUEST) \
 || defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
int wolfSSL_CTX_EnableOCSPStapling(WOLFSSL_CTX* ctx)
{
    WOLFSSL_ENTER("wolfSSL_CTX_EnableOCSPStapling");
    if (ctx)
        return wolfSSL_CertManagerEnableOCSPStapling(ctx->cm);
    else
        return BAD_FUNC_ARG;
}

int wolfSSL_CTX_DisableOCSPStapling(WOLFSSL_CTX* ctx)
{
    WOLFSSL_ENTER("wolfSSL_CTX_DisableOCSPStapling");
    if (ctx)
        return wolfSSL_CertManagerDisableOCSPStapling(ctx->cm);
    else
        return BAD_FUNC_ARG;
}
#endif /* HAVE_CERTIFICATE_STATUS_REQUEST || HAVE_CERTIFICATE_STATUS_REQUEST_V2 */

#endif /* HAVE_OCSP */


#ifndef NO_FILESYSTEM

/* process a file with name fname into ctx of format and type
   userChain specifies a user certificate chain to pass during handshake */
int ProcessFile(WOLFSSL_CTX* ctx, const char* fname, int format, int type,
                WOLFSSL* ssl, int userChain, WOLFSSL_CRL* crl)
{
#ifdef WOLFSSL_SMALL_STACK
    byte   staticBuffer[1]; /* force heap usage */
#else
    byte   staticBuffer[FILE_BUFFER_SIZE];
#endif
    byte*  myBuffer = staticBuffer;
    int    dynamic = 0;
    int    ret;
    long   sz = 0;
    XFILE  file;
    void*  heapHint = wolfSSL_CTX_GetHeap(ctx, ssl);

    (void)crl;
    (void)heapHint;

    if (fname == NULL) return WOLFSSL_BAD_FILE;

    file = XFOPEN(fname, "rb");
    if (file == XBADFILE) return WOLFSSL_BAD_FILE;
    XFSEEK(file, 0, XSEEK_END);
    sz = XFTELL(file);
    XREWIND(file);

    if (sz > (long)sizeof(staticBuffer)) {
        WOLFSSL_MSG("Getting dynamic buffer");
        myBuffer = (byte*)XMALLOC(sz, heapHint, DYNAMIC_TYPE_FILE);
        if (myBuffer == NULL) {
            XFCLOSE(file);
            return WOLFSSL_BAD_FILE;
        }
        dynamic = 1;
    }
    else if (sz <= 0) {
        XFCLOSE(file);
        return WOLFSSL_BAD_FILE;
    }

    if ( (ret = (int)XFREAD(myBuffer, 1, sz, file)) != sz)
        ret = WOLFSSL_BAD_FILE;
    else {
        if ((type == CA_TYPE || type == TRUSTED_PEER_TYPE)
                                                  && format == WOLFSSL_FILETYPE_PEM)
            ret = ProcessChainBuffer(ctx, myBuffer, sz, format, type, ssl);
#ifdef HAVE_CRL
        else if (type == CRL_TYPE)
            ret = BufferLoadCRL(crl, myBuffer, sz, format, 0);
#endif
        else
            ret = ProcessBuffer(ctx, myBuffer, sz, format, type, ssl, NULL,
                                userChain);
    }

    XFCLOSE(file);
    if (dynamic)
        XFREE(myBuffer, heapHint, DYNAMIC_TYPE_FILE);

    return ret;
}


/* loads file then loads each file in path, no c_rehash */
int wolfSSL_CTX_load_verify_locations(WOLFSSL_CTX* ctx, const char* file,
                                     const char* path)
{
    int ret = WOLFSSL_SUCCESS;
#ifndef NO_WOLFSSL_DIR
    int fileRet;
#endif

    WOLFSSL_ENTER("wolfSSL_CTX_load_verify_locations");

    if (ctx == NULL || (file == NULL && path == NULL) )
        return WOLFSSL_FAILURE;

    if (file)
        ret = ProcessFile(ctx, file, WOLFSSL_FILETYPE_PEM, CA_TYPE, NULL, 0, NULL);

    if (ret == WOLFSSL_SUCCESS && path) {
#ifndef NO_WOLFSSL_DIR
        char* name = NULL;
    #ifdef WOLFSSL_SMALL_STACK
        ReadDirCtx* readCtx = NULL;
        readCtx = (ReadDirCtx*)XMALLOC(sizeof(ReadDirCtx), ctx->heap,
                                                       DYNAMIC_TYPE_DIRCTX);
        if (readCtx == NULL)
            return MEMORY_E;
    #else
        ReadDirCtx readCtx[1];
    #endif

        /* try to load each regular file in path */
        fileRet = wc_ReadDirFirst(readCtx, path, &name);
        while (fileRet == 0 && name) {
            ret = ProcessFile(ctx, name, WOLFSSL_FILETYPE_PEM, CA_TYPE,
                                                          NULL, 0, NULL);
            if (ret != WOLFSSL_SUCCESS)
                break;
            fileRet = wc_ReadDirNext(readCtx, path, &name);
        }
        wc_ReadDirClose(readCtx);

        /* pass directory read failure to response code */
        if (ret == WOLFSSL_SUCCESS && fileRet != -1) {
            ret = fileRet;
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(readCtx, ctx->heap, DYNAMIC_TYPE_DIRCTX);
    #endif
#else
        ret = NOT_COMPILED_IN;
#endif
    }

    return ret;
}


#ifdef WOLFSSL_TRUST_PEER_CERT
/* Used to specify a peer cert to match when connecting
    ctx : the ctx structure to load in peer cert
    file: the string name of cert file
    type: type of format such as PEM/DER
 */
int wolfSSL_CTX_trust_peer_cert(WOLFSSL_CTX* ctx, const char* file, int type)
{
    WOLFSSL_ENTER("wolfSSL_CTX_trust_peer_cert");

    if (ctx == NULL || file == NULL) {
        return WOLFSSL_FAILURE;
    }

    return ProcessFile(ctx, file, type, TRUSTED_PEER_TYPE, NULL, 0, NULL);
}
#endif /* WOLFSSL_TRUST_PEER_CERT */


/* Verify the certificate, WOLFSSL_SUCCESS for ok, < 0 for error */
int wolfSSL_CertManagerVerify(WOLFSSL_CERT_MANAGER* cm, const char* fname,
                             int format)
{
    int    ret = WOLFSSL_FATAL_ERROR;
#ifdef WOLFSSL_SMALL_STACK
    byte   staticBuffer[1]; /* force heap usage */
#else
    byte   staticBuffer[FILE_BUFFER_SIZE];
#endif
    byte*  myBuffer = staticBuffer;
    int    dynamic = 0;
    long   sz = 0;
    XFILE  file = XFOPEN(fname, "rb");

    WOLFSSL_ENTER("wolfSSL_CertManagerVerify");

    if (file == XBADFILE) return WOLFSSL_BAD_FILE;
    XFSEEK(file, 0, XSEEK_END);
    sz = XFTELL(file);
    XREWIND(file);

    if (sz > MAX_WOLFSSL_FILE_SIZE || sz <= 0) {
        WOLFSSL_MSG("CertManagerVerify file bad size");
        XFCLOSE(file);
        return WOLFSSL_BAD_FILE;
    }

    if (sz > (long)sizeof(staticBuffer)) {
        WOLFSSL_MSG("Getting dynamic buffer");
        myBuffer = (byte*) XMALLOC(sz, cm->heap, DYNAMIC_TYPE_FILE);
        if (myBuffer == NULL) {
            XFCLOSE(file);
            return WOLFSSL_BAD_FILE;
        }
        dynamic = 1;
    }

    if ( (ret = (int)XFREAD(myBuffer, 1, sz, file)) != sz)
        ret = WOLFSSL_BAD_FILE;
    else
        ret = wolfSSL_CertManagerVerifyBuffer(cm, myBuffer, sz, format);

    XFCLOSE(file);
    if (dynamic)
        XFREE(myBuffer, cm->heap, DYNAMIC_TYPE_FILE);

    return ret;
}


/* like load verify locations, 1 for success, < 0 for error */
int wolfSSL_CertManagerLoadCA(WOLFSSL_CERT_MANAGER* cm, const char* file,
                             const char* path)
{
    int ret = WOLFSSL_FATAL_ERROR;
    WOLFSSL_CTX* tmp;

    WOLFSSL_ENTER("wolfSSL_CertManagerLoadCA");

    if (cm == NULL) {
        WOLFSSL_MSG("No CertManager error");
        return ret;
    }
    tmp = wolfSSL_CTX_new(cm_pick_method());

    if (tmp == NULL) {
        WOLFSSL_MSG("CTX new failed");
        return ret;
    }

    /* for tmp use */
    wolfSSL_CertManagerFree(tmp->cm);
    tmp->cm = cm;

    ret = wolfSSL_CTX_load_verify_locations(tmp, file, path);

    /* don't loose our good one */
    tmp->cm = NULL;
    wolfSSL_CTX_free(tmp);

    return ret;
}


/* Check private against public in certificate for match
 *
 * ctx  WOLFSSL_CTX structure to check private key in
 *
 * Returns SSL_SUCCESS on good private key and SSL_FAILURE if miss matched. */
int wolfSSL_CTX_check_private_key(const WOLFSSL_CTX* ctx)
{
    DecodedCert der;
    word32 size;
    byte*  buff;
    int    ret;

    WOLFSSL_ENTER("wolfSSL_CTX_check_private_key");

    if (ctx == NULL) {
        return WOLFSSL_FAILURE;
    }

#ifndef NO_CERTS
    size = ctx->certificate->length;
    buff = ctx->certificate->buffer;
    InitDecodedCert(&der, buff, size, ctx->heap);
    if (ParseCertRelative(&der, CERT_TYPE, NO_VERIFY, NULL) != 0) {
        FreeDecodedCert(&der);
        return WOLFSSL_FAILURE;
    }

    size = ctx->privateKey->length;
    buff = ctx->privateKey->buffer;
    ret  = wc_CheckPrivateKey(buff, size, &der);
    FreeDecodedCert(&der);

    if (ret == 1) {
        return WOLFSSL_SUCCESS;
    }
    else {
        return WOLFSSL_FAILURE;
    }
#else
    WOLFSSL_MSG("NO_CERTS is defined, can not check private key");
    return WOLFSSL_FAILURE;
#endif
}

#ifdef HAVE_CRL


/* check CRL if enabled, WOLFSSL_SUCCESS  */
int wolfSSL_CertManagerCheckCRL(WOLFSSL_CERT_MANAGER* cm, byte* der, int sz)
{
    int ret = 0;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert* cert = NULL;
#else
    DecodedCert  cert[1];
#endif

    WOLFSSL_ENTER("wolfSSL_CertManagerCheckCRL");

    if (cm == NULL)
        return BAD_FUNC_ARG;

    if (cm->crlEnabled == 0)
        return WOLFSSL_SUCCESS;

#ifdef WOLFSSL_SMALL_STACK
    cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), NULL, DYNAMIC_TYPE_DCERT);
    if (cert == NULL)
        return MEMORY_E;
#endif

    InitDecodedCert(cert, der, sz, NULL);

    if ((ret = ParseCertRelative(cert, CERT_TYPE, VERIFY_CRL, cm)) != 0) {
        WOLFSSL_MSG("ParseCert failed");
    }
    else if ((ret = CheckCertCRL(cm->crl, cert)) != 0) {
        WOLFSSL_MSG("CheckCertCRL failed");
    }

    FreeDecodedCert(cert);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
#endif

    return ret == 0 ? WOLFSSL_SUCCESS : ret;
}


int wolfSSL_CertManagerSetCRL_Cb(WOLFSSL_CERT_MANAGER* cm, CbMissingCRL cb)
{
    WOLFSSL_ENTER("wolfSSL_CertManagerSetCRL_Cb");
    if (cm == NULL)
        return BAD_FUNC_ARG;

    cm->cbMissingCRL = cb;

    return WOLFSSL_SUCCESS;
}

#ifdef HAVE_CRL_IO
int wolfSSL_CertManagerSetCRL_IOCb(WOLFSSL_CERT_MANAGER* cm, CbCrlIO cb)
{
    if (cm == NULL)
        return BAD_FUNC_ARG;

    cm->crl->crlIOCb = cb;

    return WOLFSSL_SUCCESS;
}
#endif

int wolfSSL_CertManagerLoadCRL(WOLFSSL_CERT_MANAGER* cm, const char* path,
                              int type, int monitor)
{
    WOLFSSL_ENTER("wolfSSL_CertManagerLoadCRL");
    if (cm == NULL)
        return BAD_FUNC_ARG;

    if (cm->crl == NULL) {
        if (wolfSSL_CertManagerEnableCRL(cm, 0) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("Enable CRL failed");
            return WOLFSSL_FATAL_ERROR;
        }
    }

    return LoadCRL(cm->crl, path, type, monitor);
}


int wolfSSL_EnableCRL(WOLFSSL* ssl, int options)
{
    WOLFSSL_ENTER("wolfSSL_EnableCRL");
    if (ssl)
        return wolfSSL_CertManagerEnableCRL(ssl->ctx->cm, options);
    else
        return BAD_FUNC_ARG;
}


int wolfSSL_DisableCRL(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("wolfSSL_DisableCRL");
    if (ssl)
        return wolfSSL_CertManagerDisableCRL(ssl->ctx->cm);
    else
        return BAD_FUNC_ARG;
}


int wolfSSL_LoadCRL(WOLFSSL* ssl, const char* path, int type, int monitor)
{
    WOLFSSL_ENTER("wolfSSL_LoadCRL");
    if (ssl)
        return wolfSSL_CertManagerLoadCRL(ssl->ctx->cm, path, type, monitor);
    else
        return BAD_FUNC_ARG;
}


int wolfSSL_SetCRL_Cb(WOLFSSL* ssl, CbMissingCRL cb)
{
    WOLFSSL_ENTER("wolfSSL_SetCRL_Cb");
    if (ssl)
        return wolfSSL_CertManagerSetCRL_Cb(ssl->ctx->cm, cb);
    else
        return BAD_FUNC_ARG;
}

#ifdef HAVE_CRL_IO
int wolfSSL_SetCRL_IOCb(WOLFSSL* ssl, CbCrlIO cb)
{
    WOLFSSL_ENTER("wolfSSL_SetCRL_Cb");
    if (ssl)
        return wolfSSL_CertManagerSetCRL_IOCb(ssl->ctx->cm, cb);
    else
        return BAD_FUNC_ARG;
}
#endif

int wolfSSL_CTX_EnableCRL(WOLFSSL_CTX* ctx, int options)
{
    WOLFSSL_ENTER("wolfSSL_CTX_EnableCRL");
    if (ctx)
        return wolfSSL_CertManagerEnableCRL(ctx->cm, options);
    else
        return BAD_FUNC_ARG;
}


int wolfSSL_CTX_DisableCRL(WOLFSSL_CTX* ctx)
{
    WOLFSSL_ENTER("wolfSSL_CTX_DisableCRL");
    if (ctx)
        return wolfSSL_CertManagerDisableCRL(ctx->cm);
    else
        return BAD_FUNC_ARG;
}


int wolfSSL_CTX_LoadCRL(WOLFSSL_CTX* ctx, const char* path,
                        int type, int monitor)
{
    WOLFSSL_ENTER("wolfSSL_CTX_LoadCRL");
    if (ctx)
        return wolfSSL_CertManagerLoadCRL(ctx->cm, path, type, monitor);
    else
        return BAD_FUNC_ARG;
}


int wolfSSL_CTX_SetCRL_Cb(WOLFSSL_CTX* ctx, CbMissingCRL cb)
{
    WOLFSSL_ENTER("wolfSSL_CTX_SetCRL_Cb");
    if (ctx)
        return wolfSSL_CertManagerSetCRL_Cb(ctx->cm, cb);
    else
        return BAD_FUNC_ARG;
}

#ifdef HAVE_CRL_IO
int wolfSSL_CTX_SetCRL_IOCb(WOLFSSL_CTX* ctx, CbCrlIO cb)
{
    WOLFSSL_ENTER("wolfSSL_CTX_SetCRL_IOCb");
    if (ctx)
        return wolfSSL_CertManagerSetCRL_IOCb(ctx->cm, cb);
    else
        return BAD_FUNC_ARG;
}
#endif


#endif /* HAVE_CRL */


#ifdef WOLFSSL_DER_LOAD

/* Add format parameter to allow DER load of CA files */
int wolfSSL_CTX_der_load_verify_locations(WOLFSSL_CTX* ctx, const char* file,
                                          int format)
{
    WOLFSSL_ENTER("wolfSSL_CTX_der_load_verify_locations");
    if (ctx == NULL || file == NULL)
        return WOLFSSL_FAILURE;

    if (ProcessFile(ctx, file, format, CA_TYPE, NULL, 0, NULL) == WOLFSSL_SUCCESS)
        return WOLFSSL_SUCCESS;

    return WOLFSSL_FAILURE;
}

#endif /* WOLFSSL_DER_LOAD */



int wolfSSL_CTX_use_certificate_file(WOLFSSL_CTX* ctx, const char* file,
                                     int format)
{
    WOLFSSL_ENTER("wolfSSL_CTX_use_certificate_file");
    if (ProcessFile(ctx, file, format, CERT_TYPE, NULL, 0, NULL) == WOLFSSL_SUCCESS)
        return WOLFSSL_SUCCESS;

    return WOLFSSL_FAILURE;
}


int wolfSSL_CTX_use_PrivateKey_file(WOLFSSL_CTX* ctx, const char* file,
                                    int format)
{
    WOLFSSL_ENTER("wolfSSL_CTX_use_PrivateKey_file");
    if (ProcessFile(ctx, file, format, PRIVATEKEY_TYPE, NULL, 0, NULL)
                    == WOLFSSL_SUCCESS)
        return WOLFSSL_SUCCESS;

    return WOLFSSL_FAILURE;
}


/* Sets the max chain depth when verifying a certificate chain. Default depth
 * is set to MAX_CHAIN_DEPTH.
 *
 * ctx   WOLFSSL_CTX structure to set depth in
 * depth max depth
 */
void wolfSSL_CTX_set_verify_depth(WOLFSSL_CTX *ctx, int depth) {
    WOLFSSL_ENTER("wolfSSL_CTX_set_verify_depth");

    if (ctx == NULL || depth < 0 || depth > MAX_CHAIN_DEPTH) {
        WOLFSSL_MSG("Bad depth argument, too large or less than 0");
        return;
    }

    ctx->verifyDepth = (byte)depth;
}


/* get cert chaining depth using ssl struct */
long wolfSSL_get_verify_depth(WOLFSSL* ssl)
{
    if(ssl == NULL) {
        return BAD_FUNC_ARG;
    }
#ifndef OPENSSL_EXTRA
    return MAX_CHAIN_DEPTH;
#else
    return ssl->options.verifyDepth;
#endif
}


/* get cert chaining depth using ctx struct */
long wolfSSL_CTX_get_verify_depth(WOLFSSL_CTX* ctx)
{
    if(ctx == NULL) {
        return BAD_FUNC_ARG;
    }
#ifndef OPENSSL_EXTRA
    return MAX_CHAIN_DEPTH;
#else
    return ctx->verifyDepth;
#endif
}


int wolfSSL_CTX_use_certificate_chain_file(WOLFSSL_CTX* ctx, const char* file)
{
   /* process up to MAX_CHAIN_DEPTH plus subject cert */
   WOLFSSL_ENTER("wolfSSL_CTX_use_certificate_chain_file");
   if (ProcessFile(ctx, file, WOLFSSL_FILETYPE_PEM,CERT_TYPE,NULL,1, NULL)
                   == WOLFSSL_SUCCESS)
       return WOLFSSL_SUCCESS;

   return WOLFSSL_FAILURE;
}


int wolfSSL_CTX_use_certificate_chain_file_format(WOLFSSL_CTX* ctx,
                                                  const char* file, int format)
{
   /* process up to MAX_CHAIN_DEPTH plus subject cert */
   WOLFSSL_ENTER("wolfSSL_CTX_use_certificate_chain_file_format");
   if (ProcessFile(ctx, file, format, CERT_TYPE, NULL, 1, NULL)
                   == WOLFSSL_SUCCESS)
       return WOLFSSL_SUCCESS;

   return WOLFSSL_FAILURE;
}


#ifndef NO_DH

/* server Diffie-Hellman parameters */
static int wolfSSL_SetTmpDH_file_wrapper(WOLFSSL_CTX* ctx, WOLFSSL* ssl,
                                        const char* fname, int format)
{
#ifdef WOLFSSL_SMALL_STACK
    byte   staticBuffer[1]; /* force heap usage */
#else
    byte   staticBuffer[FILE_BUFFER_SIZE];
#endif
    byte*  myBuffer = staticBuffer;
    int    dynamic = 0;
    int    ret;
    long   sz = 0;
    XFILE  file;

    if (ctx == NULL || fname == NULL)
        return BAD_FUNC_ARG;

    file = XFOPEN(fname, "rb");
    if (file == XBADFILE) return WOLFSSL_BAD_FILE;
    XFSEEK(file, 0, XSEEK_END);
    sz = XFTELL(file);
    XREWIND(file);

    if (sz > (long)sizeof(staticBuffer)) {
        WOLFSSL_MSG("Getting dynamic buffer");
        myBuffer = (byte*) XMALLOC(sz, ctx->heap, DYNAMIC_TYPE_FILE);
        if (myBuffer == NULL) {
            XFCLOSE(file);
            return WOLFSSL_BAD_FILE;
        }
        dynamic = 1;
    }
    else if (sz <= 0) {
        XFCLOSE(file);
        return WOLFSSL_BAD_FILE;
    }

    if ( (ret = (int)XFREAD(myBuffer, 1, sz, file)) != sz)
        ret = WOLFSSL_BAD_FILE;
    else {
        if (ssl)
            ret = wolfSSL_SetTmpDH_buffer(ssl, myBuffer, sz, format);
        else
            ret = wolfSSL_CTX_SetTmpDH_buffer(ctx, myBuffer, sz, format);
    }

    XFCLOSE(file);
    if (dynamic)
        XFREE(myBuffer, ctx->heap, DYNAMIC_TYPE_FILE);

    return ret;
}

/* server Diffie-Hellman parameters */
int wolfSSL_SetTmpDH_file(WOLFSSL* ssl, const char* fname, int format)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    return wolfSSL_SetTmpDH_file_wrapper(ssl->ctx, ssl, fname, format);
}


/* server Diffie-Hellman parameters */
int wolfSSL_CTX_SetTmpDH_file(WOLFSSL_CTX* ctx, const char* fname, int format)
{
    return wolfSSL_SetTmpDH_file_wrapper(ctx, NULL, fname, format);
}

#endif /* NO_DH */

#endif /* NO_FILESYSTEM */


#if defined(OPENSSL_EXTRA) || !defined(NO_PWDBASED) && \
    (defined(OPENSSL_EXTRA_X509_SMALL) || defined(HAVE_WEBSERVER))

static int wolfSSL_EVP_get_hashinfo(const WOLFSSL_EVP_MD* evp,
    int* pHash, int* pHashSz)
{
    enum wc_HashType hash = WC_HASH_TYPE_NONE;
    int hashSz;

    if (XSTRLEN(evp) < 3) {
        /* do not try comparing strings if size is too small */
        return WOLFSSL_FAILURE;
    }

    if (XSTRNCMP("SHA", evp, 3) == 0) {
        if (XSTRLEN(evp) > 3) {
        #ifndef NO_SHA256
            if (XSTRNCMP("SHA256", evp, 6) == 0) {
                hash = WC_HASH_TYPE_SHA256;
            }
            else
        #endif
        #ifdef WOLFSSL_SHA384
            if (XSTRNCMP("SHA384", evp, 6) == 0) {
                hash = WC_HASH_TYPE_SHA384;
            }
            else
        #endif
        #ifdef WOLFSSL_SHA512
            if (XSTRNCMP("SHA512", evp, 6) == 0) {
                hash = WC_HASH_TYPE_SHA512;
            }
            else
        #endif
            {
                WOLFSSL_MSG("Unknown SHA hash");
            }
        }
        else {
            hash = WC_HASH_TYPE_SHA;
        }
    }
#ifdef WOLFSSL_MD2
    else if (XSTRNCMP("MD2", evp, 3) == 0) {
        hash = WC_HASH_TYPE_MD2;
    }
#endif
#ifndef NO_MD4
    else if (XSTRNCMP("MD4", evp, 3) == 0) {
        hash = WC_HASH_TYPE_MD4;
    }
#endif
#ifndef NO_MD5
    else if (XSTRNCMP("MD5", evp, 3) == 0) {
        hash = WC_HASH_TYPE_MD5;
    }
#endif

    if (pHash)
        *pHash = hash;

    hashSz = wc_HashGetDigestSize(hash);
    if (pHashSz)
        *pHashSz = hashSz;

    if (hashSz < 0) {
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}

#endif


#ifdef OPENSSL_EXTRA
/* put SSL type in extra for now, not very common */

/* Converts a DER format key read from "bio" to a PKCS8 structure.
 *
 * bio  input bio to read DER from
 * pkey If not NULL then this pointer will be overwritten with a new PKCS8
 *      structure.
 *
 * returns a WOLFSSL_PKCS8_PRIV_KEY_INFO pointer on success and NULL in fail
 *         case.
 */
WOLFSSL_PKCS8_PRIV_KEY_INFO* wolfSSL_d2i_PKCS8_PKEY_bio(WOLFSSL_BIO* bio,
        WOLFSSL_PKCS8_PRIV_KEY_INFO** pkey)
{
    WOLFSSL_PKCS8_PRIV_KEY_INFO* pkcs8 = NULL;
#ifdef WOLFSSL_PEM_TO_DER
    unsigned char* mem;
    int memSz;
    int keySz;

    WOLFSSL_MSG("wolfSSL_d2i_PKCS8_PKEY_bio()");

    if (bio == NULL) {
        return NULL;
    }

    if ((memSz = wolfSSL_BIO_get_mem_data(bio, &mem)) < 0) {
        return NULL;
    }

    if ((keySz = wc_KeyPemToDer(mem, memSz, mem, memSz, NULL)) < 0) {
        WOLFSSL_MSG("Not PEM format");
        keySz = memSz;
        if ((keySz = ToTraditional((byte*)mem, (word32)keySz)) < 0) {
            return NULL;
        }
    }

    pkcs8 = wolfSSL_PKEY_new();
    pkcs8->pkey.ptr = (char*)XMALLOC(keySz, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
    if (pkcs8->pkey.ptr == NULL) {
        wolfSSL_EVP_PKEY_free(pkcs8);
        return NULL;
    }
    XMEMCPY(pkcs8->pkey.ptr, mem, keySz);
    pkcs8->pkey_sz = keySz;

    if (pkey != NULL) {
        *pkey = pkcs8;
    }
#else
    (void)bio;
    (void)pkey;
#endif /* WOLFSSL_PEM_TO_DER */

    return pkcs8;
}


/* expecting DER format public key
 *
 * bio  input bio to read DER from
 * out  If not NULL then this pointer will be overwritten with a new
 * WOLFSSL_EVP_PKEY pointer
 *
 * returns a WOLFSSL_EVP_PKEY pointer on success and NULL in fail case.
 */
WOLFSSL_EVP_PKEY* wolfSSL_d2i_PUBKEY_bio(WOLFSSL_BIO* bio,
                                         WOLFSSL_EVP_PKEY** out)
{
    unsigned char* mem;
    long memSz;
    WOLFSSL_EVP_PKEY* pkey = NULL;

    WOLFSSL_ENTER("wolfSSL_d2i_PUBKEY_bio()");

    if (bio == NULL) {
        return NULL;
    }
    (void)out;

    memSz = wolfSSL_BIO_pending(bio);
    if (memSz <= 0) {
        return NULL;
    }

    mem = (unsigned char*)XMALLOC(memSz, bio->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (mem == NULL) {
        return NULL;
    }

    if (wolfSSL_BIO_read(bio, mem, (int)memSz) == memSz) {
        pkey = wolfSSL_d2i_PUBKEY(NULL, &mem, memSz);
        if (out != NULL && pkey != NULL) {
            *out = pkey;
        }
    }

    XFREE(mem, bio->heap, DYNAMIC_TYPE_TMP_BUFFER);
    return pkey;
}



/* Converts a DER encoded public key to a WOLFSSL_EVP_PKEY structure.
 *
 * out  pointer to new WOLFSSL_EVP_PKEY structure. Can be NULL
 * in   DER buffer to convert
 * inSz size of in buffer
 *
 * returns a pointer to a new WOLFSSL_EVP_PKEY structure on success and NULL
 *         on fail
 */
WOLFSSL_EVP_PKEY* wolfSSL_d2i_PUBKEY(WOLFSSL_EVP_PKEY** out, unsigned char** in,
        long inSz)
{
    WOLFSSL_EVP_PKEY* pkey = NULL;
    const unsigned char* mem;
    long memSz = inSz;

    WOLFSSL_ENTER("wolfSSL_d2i_PUBKEY");

    if (in == NULL || inSz < 0) {
        WOLFSSL_MSG("Bad argument");
        return NULL;
    }
    mem = *in;

    #if !defined(NO_RSA)
    {
        RsaKey rsa;
        word32 keyIdx = 0;

        /* test if RSA key */
        if (wc_InitRsaKey(&rsa, NULL) == 0 &&
            wc_RsaPublicKeyDecode(mem, &keyIdx, &rsa, (word32)memSz) == 0) {
            wc_FreeRsaKey(&rsa);
            pkey = wolfSSL_PKEY_new();
            if (pkey != NULL) {
                pkey->pkey_sz = keyIdx;
                pkey->pkey.ptr = (char*)XMALLOC(memSz, NULL,
                        DYNAMIC_TYPE_PUBLIC_KEY);
                if (pkey->pkey.ptr == NULL) {
                    wolfSSL_EVP_PKEY_free(pkey);
                    return NULL;
                }
                XMEMCPY(pkey->pkey.ptr, mem, keyIdx);
                pkey->type = EVP_PKEY_RSA;
                if (out != NULL) {
                    *out = pkey;
                }

                pkey->ownRsa = 1;
                pkey->rsa = wolfSSL_RSA_new();
                if (pkey->rsa == NULL) {
                    wolfSSL_EVP_PKEY_free(pkey);
                    return NULL;
                }

                if (wolfSSL_RSA_LoadDer_ex(pkey->rsa,
                            (const unsigned char*)pkey->pkey.ptr,
                            pkey->pkey_sz, WOLFSSL_RSA_LOAD_PUBLIC) != 1) {
                    wolfSSL_EVP_PKEY_free(pkey);
                    return NULL;
                }

                return pkey;
            }
        }
        wc_FreeRsaKey(&rsa);
    }
    #endif /* NO_RSA */

    #ifdef HAVE_ECC
    {
        word32  keyIdx = 0;
        ecc_key ecc;

        if (wc_ecc_init(&ecc) == 0 &&
            wc_EccPublicKeyDecode(mem, &keyIdx, &ecc, (word32)memSz) == 0) {
            wc_ecc_free(&ecc);
            pkey = wolfSSL_PKEY_new();
            if (pkey != NULL) {
                pkey->pkey_sz = keyIdx;
                pkey->pkey.ptr = (char*)XMALLOC(keyIdx, NULL,
                        DYNAMIC_TYPE_PUBLIC_KEY);
                if (pkey->pkey.ptr == NULL) {
                    wolfSSL_EVP_PKEY_free(pkey);
                    return NULL;
                }
                XMEMCPY(pkey->pkey.ptr, mem, keyIdx);
                pkey->type = EVP_PKEY_EC;
                if (out != NULL) {
                    *out = pkey;
                }
                return pkey;
            }
        }
        wc_ecc_free(&ecc);
    }
    #endif /* HAVE_ECC */

    return pkey;

}


/* Reads in a DER format key. If PKCS8 headers are found they are stripped off.
 *
 * type  type of key
 * out   newly created WOLFSSL_EVP_PKEY structure
 * in    pointer to input key DER
 * inSz  size of in buffer
 *
 * On success a non null pointer is returned and the pointer in is advanced the
 * same number of bytes read.
 */
WOLFSSL_EVP_PKEY* wolfSSL_d2i_PrivateKey(int type, WOLFSSL_EVP_PKEY** out,
        const unsigned char **in, long inSz)
{
    WOLFSSL_EVP_PKEY* local;
    word32 idx = 0;
    int    ret;

    WOLFSSL_ENTER("wolfSSL_d2i_PrivateKey");

    if (in == NULL || inSz < 0) {
        WOLFSSL_MSG("Bad argument");
        return NULL;
    }

    /* Check if input buffer has PKCS8 header. In the case that it does not
     * have a PKCS8 header then do not error out. */
    if ((ret = ToTraditionalInline((const byte*)(*in), &idx, (word32)inSz))
            > 0) {
        WOLFSSL_MSG("Found and removed PKCS8 header");
    }
    else {
        if (ret != ASN_PARSE_E) {
            WOLFSSL_MSG("Unexpected error with trying to remove PKCS8 header");
            return NULL;
        }
    }

    if (out != NULL && *out != NULL) {
        wolfSSL_EVP_PKEY_free(*out);
    }
    local = wolfSSL_PKEY_new();
    if (local == NULL) {
        return NULL;
    }

    /* sanity check on idx before use */
    if ((int)idx > inSz) {
        WOLFSSL_MSG("Issue with index pointer");
        wolfSSL_EVP_PKEY_free(local);
        local = NULL;
        return NULL;
    }

    local->type     = type;
    local->pkey_sz  = (int)inSz - idx;
    local->pkey.ptr = (char*)XMALLOC(inSz - idx, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
    if (local->pkey.ptr == NULL) {
        wolfSSL_EVP_PKEY_free(local);
        local = NULL;
        return NULL;
    }
    else {
        XMEMCPY(local->pkey.ptr, *in + idx, inSz - idx);
    }

    switch (type) {
#ifndef NO_RSA
        case EVP_PKEY_RSA:
            local->ownRsa = 1;
            local->rsa = wolfSSL_RSA_new();
            if (local->rsa == NULL) {
                wolfSSL_EVP_PKEY_free(local);
                return NULL;
            }
            if (wolfSSL_RSA_LoadDer_ex(local->rsa,
                      (const unsigned char*)local->pkey.ptr, local->pkey_sz,
                      WOLFSSL_RSA_LOAD_PRIVATE) != SSL_SUCCESS) {
                wolfSSL_EVP_PKEY_free(local);
                return NULL;
            }
            break;
#endif /* NO_RSA */
#ifdef HAVE_ECC
        case EVP_PKEY_EC:
            local->ownEcc = 1;
            local->ecc = wolfSSL_EC_KEY_new();
            if (local->ecc == NULL) {
                wolfSSL_EVP_PKEY_free(local);
                return NULL;
            }
            if (wolfSSL_EC_KEY_LoadDer(local->ecc,
                      (const unsigned char*)local->pkey.ptr, local->pkey_sz)
                      != SSL_SUCCESS) {
                wolfSSL_EVP_PKEY_free(local);
                return NULL;
            }
            break;
#endif /* HAVE_ECC */

        default:
            WOLFSSL_MSG("Unsupported key type");
            wolfSSL_EVP_PKEY_free(local);
            return NULL;
    }

    /* advance pointer with success */
    if (local != NULL) {
        if ((idx + local->pkey_sz) <= (word32)inSz) {
            *in = *in + idx + local->pkey_sz;
        }

        if (out != NULL) {
            *out = local;
        }
    }

    return local;
}

#ifndef NO_WOLFSSL_STUB
long wolfSSL_ctrl(WOLFSSL* ssl, int cmd, long opt, void* pt)
{
    WOLFSSL_STUB("SSL_ctrl");
    (void)ssl;
    (void)cmd;
    (void)opt;
    (void)pt;
    return WOLFSSL_FAILURE;
}
#endif

#ifndef NO_WOLFSSL_STUB
long wolfSSL_CTX_ctrl(WOLFSSL_CTX* ctx, int cmd, long opt, void* pt)
{
    WOLFSSL_STUB("SSL_CTX_ctrl");
    (void)ctx;
    (void)cmd;
    (void)opt;
    (void)pt;
    return WOLFSSL_FAILURE;
}
#endif

#ifndef NO_CERTS

int wolfSSL_check_private_key(const WOLFSSL* ssl)
{
    DecodedCert der;
    word32 size;
    byte*  buff;
    int    ret;

    if (ssl == NULL) {
        return WOLFSSL_FAILURE;
    }

    size = ssl->buffers.certificate->length;
    buff = ssl->buffers.certificate->buffer;
    InitDecodedCert(&der, buff, size, ssl->heap);
    if (ParseCertRelative(&der, CERT_TYPE, NO_VERIFY, NULL) != 0) {
        FreeDecodedCert(&der);
        return WOLFSSL_FAILURE;
    }

    size = ssl->buffers.key->length;
    buff = ssl->buffers.key->buffer;
    ret  = wc_CheckPrivateKey(buff, size, &der);
    FreeDecodedCert(&der);
    return ret;
}


/* Looks for the extension matching the passed in nid
 *
 * c   : if not null then is set to status value -2 if multiple occurances
 *       of the extension are found, -1 if not found, 0 if found and not
 *       critical, and 1 if found and critical.
 * nid : Extension OID to be found.
 * idx : if NULL return first extension found match, otherwise start search at
 *       idx location and set idx to the location of extension returned.
 * returns NULL or a pointer to an WOLFSSL_STACK holding extension structure
 *
 * NOTE code for decoding extensions is in asn.c DecodeCertExtensions --
 * use already decoded extension in this function to avoid decoding twice.
 * Currently we do not make use of idx since getting pre decoded extensions.
 */
void* wolfSSL_X509_get_ext_d2i(const WOLFSSL_X509* x509,
                                                     int nid, int* c, int* idx)
{
    WOLFSSL_STACK* sk = NULL;
    WOLFSSL_ASN1_OBJECT* obj = NULL;

    WOLFSSL_ENTER("wolfSSL_X509_get_ext_d2i");

    if (x509 == NULL) {
        return NULL;
    }

    if (c != NULL) {
        *c = -1; /* default to not found */
    }

    sk = (WOLF_STACK_OF(WOLFSSL_ASN1_OBJECT)*)XMALLOC(
                sizeof(WOLF_STACK_OF(WOLFSSL_ASN1_OBJECT)), NULL, DYNAMIC_TYPE_ASN1);
    if (sk == NULL) {
        return NULL;
    }
    XMEMSET(sk, 0, sizeof(WOLF_STACK_OF(WOLFSSL_ASN1_OBJECT)));

    switch (nid) {
        case BASIC_CA_OID:
            if (x509->basicConstSet) {
                obj = wolfSSL_ASN1_OBJECT_new();
                if (c != NULL) {
                    *c = x509->basicConstCrit;
                }
                obj->type = BASIC_CA_OID;
                obj->grp  = oidCertExtType;
            }
            else {
                WOLFSSL_MSG("No Basic Constraint set");
            }
            break;

        case ALT_NAMES_OID:
            {
                DNS_entry* dns = NULL;

                if (x509->subjAltNameSet && x509->altNames != NULL) {
                    /* alt names are DNS_entry structs */
                    if (c != NULL) {
                        if (x509->altNames->next != NULL) {
                            *c = -2; /* more then one found */
                        }
                        else {
                            *c = x509->subjAltNameCrit;
                        }
                    }

                    dns = x509->altNames;
                    while (dns != NULL) {
                        obj = wolfSSL_ASN1_OBJECT_new();
                        obj->type = dns->type;
                        obj->grp  = oidCertExtType;
                        obj->obj  = (byte*)dns->name;

                        /* set app derefrenced pointers */
                        obj->d.ia5_internal.data   = dns->name;
                        obj->d.ia5_internal.length = (int)XSTRLEN(dns->name);
                        dns = dns->next;
                        /* last dns in list add at end of function */
                        if (dns != NULL) {
                            if (wolfSSL_sk_ASN1_OBJECT_push(sk, obj) !=
                                                                  WOLFSSL_SUCCESS) {
                            WOLFSSL_MSG("Error pushing ASN1 object onto stack");
                            wolfSSL_ASN1_OBJECT_free(obj);
                            wolfSSL_sk_ASN1_OBJECT_free(sk);
                            sk = NULL;
                            }
                        }
                    }
                }
                else {
                    WOLFSSL_MSG("No Alt Names set");
                }
            }
            break;

        case CRL_DIST_OID:
            if (x509->CRLdistSet && x509->CRLInfo != NULL) {
                if (c != NULL) {
                    *c = x509->CRLdistCrit;
                }
                obj = wolfSSL_ASN1_OBJECT_new();
                obj->type  = CRL_DIST_OID;
                obj->grp   = oidCertExtType;
                obj->obj   = x509->CRLInfo;
                obj->objSz = x509->CRLInfoSz;
            }
            else {
                WOLFSSL_MSG("No CRL dist set");
            }
            break;

        case AUTH_INFO_OID:
            if (x509->authInfoSet && x509->authInfo != NULL) {
                if (c != NULL) {
                    *c = x509->authInfoCrit;
                }
                obj = wolfSSL_ASN1_OBJECT_new();
                obj->type  = AUTH_INFO_OID;
                obj->grp   = oidCertExtType;
                obj->obj   = x509->authInfo;
                obj->objSz = x509->authInfoSz;
            }
            else {
                WOLFSSL_MSG("No Auth Info set");
            }
            break;

        case AUTH_KEY_OID:
            if (x509->authKeyIdSet) {
                if (c != NULL) {
                    *c = x509->authKeyIdCrit;
                }
                obj = wolfSSL_ASN1_OBJECT_new();
                obj->type  = AUTH_KEY_OID;
                obj->grp   = oidCertExtType;
                obj->obj   = x509->authKeyId;
                obj->objSz = x509->authKeyIdSz;
            }
            else {
                WOLFSSL_MSG("No Auth Key set");
            }
            break;

        case SUBJ_KEY_OID:
            if (x509->subjKeyIdSet) {
                if (c != NULL) {
                    *c = x509->subjKeyIdCrit;
                }
                obj = wolfSSL_ASN1_OBJECT_new();
                obj->type  = SUBJ_KEY_OID;
                obj->grp   = oidCertExtType;
                obj->obj   = x509->subjKeyId;
                obj->objSz = x509->subjKeyIdSz;
            }
            else {
                WOLFSSL_MSG("No Subject Key set");
            }
            break;

        case CERT_POLICY_OID:
            #ifdef WOLFSSL_CERT_EXT
            {
                int i;

                if (x509->certPoliciesNb > 0) {
                    if (c != NULL) {
                        if (x509->certPoliciesNb > 1) {
                            *c = -2;
                        }
                        else {
                            *c = 0;
                        }
                    }

                    for (i = 0; i < x509->certPoliciesNb - 1; i++) {
                        obj = wolfSSL_ASN1_OBJECT_new();
                        obj->type  = CERT_POLICY_OID;
                        obj->grp   = oidCertExtType;
                        obj->obj   = (byte*)(x509->certPolicies[i]);
                        obj->objSz = MAX_CERTPOL_SZ;
                        if (wolfSSL_sk_ASN1_OBJECT_push(sk, obj)
                                                               != WOLFSSL_SUCCESS) {
                            WOLFSSL_MSG("Error pushing ASN1 object onto stack");
                            wolfSSL_ASN1_OBJECT_free(obj);
                            wolfSSL_sk_ASN1_OBJECT_free(sk);
                            sk = NULL;
                        }
                    }
                    obj = wolfSSL_ASN1_OBJECT_new();
                    obj->type  = CERT_POLICY_OID;
                    obj->grp   = oidCertExtType;
                    obj->obj   = (byte*)(x509->certPolicies[i]);
                    obj->objSz = MAX_CERTPOL_SZ;
                }
                else {
                    WOLFSSL_MSG("No Cert Policy set");
                }
            }
            #else
                #ifdef WOLFSSL_SEP
                if (x509->certPolicySet) {
                    if (c != NULL) {
                        *c = x509->certPolicyCrit;
                    }
                    obj = wolfSSL_ASN1_OBJECT_new();
                    obj->type  = CERT_POLICY_OID;
                    obj->grp   = oidCertExtType;
                }
                else {
                    WOLFSSL_MSG("No Cert Policy set");
                }
                #else
                WOLFSSL_MSG("wolfSSL not built with WOLFSSL_SEP or WOLFSSL_CERT_EXT");
                #endif /* WOLFSSL_SEP */
            #endif /* WOLFSSL_CERT_EXT */
            break;

        case KEY_USAGE_OID:
            if (x509->keyUsageSet) {
                if (c != NULL) {
                    *c = x509->keyUsageCrit;
                }
                obj = wolfSSL_ASN1_OBJECT_new();
                obj->type  = KEY_USAGE_OID;
                obj->grp   = oidCertExtType;
                obj->obj   = (byte*)&(x509->keyUsage);
                obj->objSz = sizeof(word16);
            }
            else {
                WOLFSSL_MSG("No Key Usage set");
            }
            break;

        case INHIBIT_ANY_OID:
            WOLFSSL_MSG("INHIBIT ANY extension not supported");
            break;

        case EXT_KEY_USAGE_OID:
            if (x509->extKeyUsageSrc != NULL) {
                if (c != NULL) {
                    if (x509->extKeyUsageCount > 1) {
                        *c = -2;
                    }
                    else {
                        *c = x509->extKeyUsageCrit;
                    }
                }
                obj = wolfSSL_ASN1_OBJECT_new();
                obj->type  = EXT_KEY_USAGE_OID;
                obj->grp   = oidCertExtType;
                obj->obj   = x509->extKeyUsageSrc;
                obj->objSz = x509->extKeyUsageSz;
            }
            else {
                WOLFSSL_MSG("No Extended Key Usage set");
            }
            break;

        case NAME_CONS_OID:
            WOLFSSL_MSG("Name Constraint OID extension not supported");
            break;

        case PRIV_KEY_USAGE_PERIOD_OID:
            WOLFSSL_MSG("Private Key Usage Period extension not supported");
            break;

        case SUBJECT_INFO_ACCESS:
            WOLFSSL_MSG("Subject Info Access extension not supported");
            break;

        case POLICY_MAP_OID:
            WOLFSSL_MSG("Policy Map extension not supported");
            break;

        case POLICY_CONST_OID:
            WOLFSSL_MSG("Policy Constraint extension not supported");
            break;

        case ISSUE_ALT_NAMES_OID:
            WOLFSSL_MSG("Issue Alt Names extension not supported");
            break;

        case TLS_FEATURE_OID:
            WOLFSSL_MSG("TLS Feature extension not supported");
            break;

        default:
            WOLFSSL_MSG("Unsupported/Unknown extension OID");
    }

    if (obj != NULL) {
        if (wolfSSL_sk_ASN1_OBJECT_push(sk, obj) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("Error pushing ASN1 object onto stack");
            wolfSSL_ASN1_OBJECT_free(obj);
            wolfSSL_sk_ASN1_OBJECT_free(sk);
            sk = NULL;
        }
    }
    else { /* no ASN1 object found for extension, free stack */
        wolfSSL_sk_ASN1_OBJECT_free(sk);
        sk = NULL;
    }

    (void)idx;

    return sk;
}


/* this function makes the assumption that out buffer is big enough for digest*/
static int wolfSSL_EVP_Digest(unsigned char* in, int inSz, unsigned char* out,
                              unsigned int* outSz, const WOLFSSL_EVP_MD* evp,
                              WOLFSSL_ENGINE* eng)
{
    int err;
    int hashType = WC_HASH_TYPE_NONE;
    int hashSz;

    (void)eng;

    err = wolfSSL_EVP_get_hashinfo(evp, &hashType, &hashSz);
    if (err != WOLFSSL_SUCCESS)
        return err;

    *outSz = hashSz;

    if (wc_Hash((enum wc_HashType)hashType, in, inSz, out, *outSz) != 0) {
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}


int wolfSSL_X509_digest(const WOLFSSL_X509* x509, const WOLFSSL_EVP_MD* digest,
        unsigned char* buf, unsigned int* len)
{
    WOLFSSL_ENTER("wolfSSL_X509_digest");

    if (x509 == NULL || digest == NULL) {
        return WOLFSSL_FAILURE;
    }

    return wolfSSL_EVP_Digest(x509->derCert->buffer, x509->derCert->length, buf,
                              len, digest, NULL);
}


int wolfSSL_use_PrivateKey(WOLFSSL* ssl, WOLFSSL_EVP_PKEY* pkey)
{
    WOLFSSL_ENTER("wolfSSL_use_PrivateKey");
    if (ssl == NULL || pkey == NULL ) {
        return WOLFSSL_FAILURE;
    }

    return wolfSSL_use_PrivateKey_buffer(ssl, (unsigned char*)pkey->pkey.ptr,
                                         pkey->pkey_sz, WOLFSSL_FILETYPE_ASN1);
}


int wolfSSL_use_PrivateKey_ASN1(int pri, WOLFSSL* ssl, unsigned char* der,
                                long derSz)
{
    WOLFSSL_ENTER("wolfSSL_use_PrivateKey_ASN1");
    if (ssl == NULL || der == NULL ) {
        return WOLFSSL_FAILURE;
    }

    (void)pri; /* type of private key */
    return wolfSSL_use_PrivateKey_buffer(ssl, der, derSz, WOLFSSL_FILETYPE_ASN1);
}


#ifndef NO_RSA
int wolfSSL_use_RSAPrivateKey_ASN1(WOLFSSL* ssl, unsigned char* der, long derSz)
{
    WOLFSSL_ENTER("wolfSSL_use_RSAPrivateKey_ASN1");
    if (ssl == NULL || der == NULL ) {
        return WOLFSSL_FAILURE;
    }

    return wolfSSL_use_PrivateKey_buffer(ssl, der, derSz, WOLFSSL_FILETYPE_ASN1);
}
#endif

int wolfSSL_use_certificate_ASN1(WOLFSSL* ssl, unsigned char* der, int derSz)
{
    long idx;

    WOLFSSL_ENTER("wolfSSL_use_certificate_ASN1");
    if (der != NULL && ssl != NULL) {
        if (ProcessBuffer(NULL, der, derSz, WOLFSSL_FILETYPE_ASN1, CERT_TYPE, ssl,
                                                        &idx, 0) == WOLFSSL_SUCCESS)
            return WOLFSSL_SUCCESS;
    }

    (void)idx;
    return WOLFSSL_FAILURE;
}


int wolfSSL_use_certificate(WOLFSSL* ssl, WOLFSSL_X509* x509)
{
    long idx;

    WOLFSSL_ENTER("wolfSSL_use_certificate");
    if (x509 != NULL && ssl != NULL && x509->derCert != NULL) {
        if (ProcessBuffer(NULL, x509->derCert->buffer, x509->derCert->length,
                     WOLFSSL_FILETYPE_ASN1, CERT_TYPE, ssl, &idx, 0) == WOLFSSL_SUCCESS)
            return WOLFSSL_SUCCESS;
    }

    (void)idx;
    return WOLFSSL_FAILURE;
}
#endif /* NO_CERTS */

#ifndef NO_FILESYSTEM

int wolfSSL_use_certificate_file(WOLFSSL* ssl, const char* file, int format)
{
    WOLFSSL_ENTER("wolfSSL_use_certificate_file");
    if (ProcessFile(ssl->ctx, file, format, CERT_TYPE,
                    ssl, 0, NULL) == WOLFSSL_SUCCESS)
        return WOLFSSL_SUCCESS;

    return WOLFSSL_FAILURE;
}


int wolfSSL_use_PrivateKey_file(WOLFSSL* ssl, const char* file, int format)
{
    WOLFSSL_ENTER("wolfSSL_use_PrivateKey_file");
    if (ProcessFile(ssl->ctx, file, format, PRIVATEKEY_TYPE,
                    ssl, 0, NULL) == WOLFSSL_SUCCESS)
        return WOLFSSL_SUCCESS;

    return WOLFSSL_FAILURE;
}


int wolfSSL_use_certificate_chain_file(WOLFSSL* ssl, const char* file)
{
   /* process up to MAX_CHAIN_DEPTH plus subject cert */
   WOLFSSL_ENTER("wolfSSL_use_certificate_chain_file");
   if (ProcessFile(ssl->ctx, file, WOLFSSL_FILETYPE_PEM, CERT_TYPE,
                   ssl, 1, NULL) == WOLFSSL_SUCCESS)
       return WOLFSSL_SUCCESS;

   return WOLFSSL_FAILURE;
}

int wolfSSL_use_certificate_chain_file_format(WOLFSSL* ssl, const char* file,
                                              int format)
{
   /* process up to MAX_CHAIN_DEPTH plus subject cert */
   WOLFSSL_ENTER("wolfSSL_use_certificate_chain_file_format");
   if (ProcessFile(ssl->ctx, file, format, CERT_TYPE, ssl, 1,
                   NULL) == WOLFSSL_SUCCESS)
       return WOLFSSL_SUCCESS;

   return WOLFSSL_FAILURE;
}


#ifdef HAVE_ECC

/* Set Temp CTX EC-DHE size in octets, should be 20 - 66 for 160 - 521 bit */
int wolfSSL_CTX_SetTmpEC_DHE_Sz(WOLFSSL_CTX* ctx, word16 sz)
{
    if (ctx == NULL || sz < ECC_MINSIZE || sz > ECC_MAXSIZE)
        return BAD_FUNC_ARG;

    ctx->eccTempKeySz = sz;

    return WOLFSSL_SUCCESS;
}


/* Set Temp SSL EC-DHE size in octets, should be 20 - 66 for 160 - 521 bit */
int wolfSSL_SetTmpEC_DHE_Sz(WOLFSSL* ssl, word16 sz)
{
    if (ssl == NULL || sz < ECC_MINSIZE || sz > ECC_MAXSIZE)
        return BAD_FUNC_ARG;

    ssl->eccTempKeySz = sz;

    return WOLFSSL_SUCCESS;
}

#endif /* HAVE_ECC */




int wolfSSL_CTX_use_RSAPrivateKey_file(WOLFSSL_CTX* ctx,const char* file,
                                   int format)
{
    WOLFSSL_ENTER("SSL_CTX_use_RSAPrivateKey_file");

    return wolfSSL_CTX_use_PrivateKey_file(ctx, file, format);
}


int wolfSSL_use_RSAPrivateKey_file(WOLFSSL* ssl, const char* file, int format)
{
    WOLFSSL_ENTER("wolfSSL_use_RSAPrivateKey_file");

    return wolfSSL_use_PrivateKey_file(ssl, file, format);
}

#endif /* NO_FILESYSTEM */

/* Copies the master secret over to out buffer. If outSz is 0 returns the size
 * of master secret.
 *
 * ses : a session from completed TLS/SSL handshake
 * out : buffer to hold copy of master secret
 * outSz : size of out buffer
 * returns : number of bytes copied into out buffer on success
 *           less then or equal to 0 is considered a failure case
 */
int wolfSSL_SESSION_get_master_key(const WOLFSSL_SESSION* ses,
        unsigned char* out, int outSz)
{
    int size;

    if (outSz == 0) {
        return SECRET_LEN;
    }

    if (ses == NULL || out == NULL || outSz < 0) {
        return 0;
    }

    if (outSz > SECRET_LEN) {
        size = SECRET_LEN;
    }
    else {
        size = outSz;
    }

    XMEMCPY(out, ses->masterSecret, size);
    return size;
}


int wolfSSL_SESSION_get_master_key_length(const WOLFSSL_SESSION* ses)
{
    (void)ses;
    return SECRET_LEN;
}

#endif /* OPENSSL_EXTRA */

#ifndef NO_FILESYSTEM
#ifdef HAVE_NTRU

int wolfSSL_CTX_use_NTRUPrivateKey_file(WOLFSSL_CTX* ctx, const char* file)
{
    WOLFSSL_ENTER("wolfSSL_CTX_use_NTRUPrivateKey_file");
    if (ctx == NULL)
        return WOLFSSL_FAILURE;

    if (ProcessFile(ctx, file, WOLFSSL_FILETYPE_RAW, PRIVATEKEY_TYPE, NULL, 0, NULL)
                         == WOLFSSL_SUCCESS) {
        ctx->haveNTRU = 1;
        return WOLFSSL_SUCCESS;
    }

    return WOLFSSL_FAILURE;
}

#endif /* HAVE_NTRU */


#endif /* NO_FILESYSTEM */


void wolfSSL_CTX_set_verify(WOLFSSL_CTX* ctx, int mode, VerifyCallback vc)
{
    WOLFSSL_ENTER("wolfSSL_CTX_set_verify");
    if (mode & WOLFSSL_VERIFY_PEER) {
        ctx->verifyPeer = 1;
        ctx->verifyNone = 0;  /* in case previously set */
    }

    if (mode == WOLFSSL_VERIFY_NONE) {
        ctx->verifyNone = 1;
        ctx->verifyPeer = 0;  /* in case previously set */
    }

    if (mode & WOLFSSL_VERIFY_FAIL_IF_NO_PEER_CERT)
        ctx->failNoCert = 1;

    if (mode & WOLFSSL_VERIFY_FAIL_EXCEPT_PSK) {
        ctx->failNoCert    = 0; /* fail on all is set to fail on PSK */
        ctx->failNoCertxPSK = 1;
    }

    ctx->verifyCallback = vc;
}


void wolfSSL_set_verify(WOLFSSL* ssl, int mode, VerifyCallback vc)
{
    WOLFSSL_ENTER("wolfSSL_set_verify");
    if (mode & WOLFSSL_VERIFY_PEER) {
        ssl->options.verifyPeer = 1;
        ssl->options.verifyNone = 0;  /* in case previously set */
    }

    if (mode == WOLFSSL_VERIFY_NONE) {
        ssl->options.verifyNone = 1;
        ssl->options.verifyPeer = 0;  /* in case previously set */
    }

    if (mode & WOLFSSL_VERIFY_FAIL_IF_NO_PEER_CERT)
        ssl->options.failNoCert = 1;

    if (mode & WOLFSSL_VERIFY_FAIL_EXCEPT_PSK) {
        ssl->options.failNoCert    = 0; /* fail on all is set to fail on PSK */
        ssl->options.failNoCertxPSK = 1;
    }

    ssl->verifyCallback = vc;
}


/* store user ctx for verify callback */
void wolfSSL_SetCertCbCtx(WOLFSSL* ssl, void* ctx)
{
    WOLFSSL_ENTER("wolfSSL_SetCertCbCtx");
    if (ssl)
        ssl->verifyCbCtx = ctx;
}


/* store context CA Cache addition callback */
void wolfSSL_CTX_SetCACb(WOLFSSL_CTX* ctx, CallbackCACache cb)
{
    if (ctx && ctx->cm)
        ctx->cm->caCacheCallback = cb;
}


#if defined(PERSIST_CERT_CACHE)

#if !defined(NO_FILESYSTEM)

/* Persist cert cache to file */
int wolfSSL_CTX_save_cert_cache(WOLFSSL_CTX* ctx, const char* fname)
{
    WOLFSSL_ENTER("wolfSSL_CTX_save_cert_cache");

    if (ctx == NULL || fname == NULL)
        return BAD_FUNC_ARG;

    return CM_SaveCertCache(ctx->cm, fname);
}


/* Persist cert cache from file */
int wolfSSL_CTX_restore_cert_cache(WOLFSSL_CTX* ctx, const char* fname)
{
    WOLFSSL_ENTER("wolfSSL_CTX_restore_cert_cache");

    if (ctx == NULL || fname == NULL)
        return BAD_FUNC_ARG;

    return CM_RestoreCertCache(ctx->cm, fname);
}

#endif /* NO_FILESYSTEM */

/* Persist cert cache to memory */
int wolfSSL_CTX_memsave_cert_cache(WOLFSSL_CTX* ctx, void* mem,
                                   int sz, int* used)
{
    WOLFSSL_ENTER("wolfSSL_CTX_memsave_cert_cache");

    if (ctx == NULL || mem == NULL || used == NULL || sz <= 0)
        return BAD_FUNC_ARG;

    return CM_MemSaveCertCache(ctx->cm, mem, sz, used);
}


/* Restore cert cache from memory */
int wolfSSL_CTX_memrestore_cert_cache(WOLFSSL_CTX* ctx, const void* mem, int sz)
{
    WOLFSSL_ENTER("wolfSSL_CTX_memrestore_cert_cache");

    if (ctx == NULL || mem == NULL || sz <= 0)
        return BAD_FUNC_ARG;

    return CM_MemRestoreCertCache(ctx->cm, mem, sz);
}


/* get how big the the cert cache save buffer needs to be */
int wolfSSL_CTX_get_cert_cache_memsize(WOLFSSL_CTX* ctx)
{
    WOLFSSL_ENTER("wolfSSL_CTX_get_cert_cache_memsize");

    if (ctx == NULL)
        return BAD_FUNC_ARG;

    return CM_GetCertCacheMemSize(ctx->cm);
}

#endif /* PERSIST_CERT_CACHE */
#endif /* !NO_CERTS */


#ifndef NO_SESSION_CACHE

WOLFSSL_SESSION* wolfSSL_get_session(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("SSL_get_session");
    if (ssl)
        return GetSession(ssl, 0, 0);

    return NULL;
}


int wolfSSL_set_session(WOLFSSL* ssl, WOLFSSL_SESSION* session)
{
    WOLFSSL_ENTER("SSL_set_session");
    if (session)
        return SetSession(ssl, session);

    return WOLFSSL_FAILURE;
}


#ifndef NO_CLIENT_CACHE

/* Associate client session with serverID, find existing or store for saving
   if newSession flag on, don't reuse existing session
   WOLFSSL_SUCCESS on ok */
int wolfSSL_SetServerID(WOLFSSL* ssl, const byte* id, int len, int newSession)
{
    WOLFSSL_SESSION* session = NULL;

    WOLFSSL_ENTER("wolfSSL_SetServerID");

    if (ssl == NULL || id == NULL || len <= 0)
        return BAD_FUNC_ARG;

    if (newSession == 0) {
        session = GetSessionClient(ssl, id, len);
        if (session) {
            if (SetSession(ssl, session) != WOLFSSL_SUCCESS) {
    #ifdef HAVE_EXT_CACHE
                wolfSSL_SESSION_free(session);
    #endif
                WOLFSSL_MSG("SetSession failed");
                session = NULL;
            }
        }
    }

    if (session == NULL) {
        WOLFSSL_MSG("Valid ServerID not cached already");

        ssl->session.idLen = (word16)min(SERVER_ID_LEN, (word32)len);
        XMEMCPY(ssl->session.serverID, id, ssl->session.idLen);
    }
    #ifdef HAVE_EXT_CACHE
    else
        wolfSSL_SESSION_free(session);
    #endif

    return WOLFSSL_SUCCESS;
}

#endif /* NO_CLIENT_CACHE */

#if defined(PERSIST_SESSION_CACHE)

/* for persistence, if changes to layout need to increment and modify
   save_session_cache() and restore_session_cache and memory versions too */
#define WOLFSSL_CACHE_VERSION 2

/* Session Cache Header information */
typedef struct {
    int version;     /* cache layout version id */
    int rows;        /* session rows */
    int columns;     /* session columns */
    int sessionSz;   /* sizeof WOLFSSL_SESSION */
} cache_header_t;

/* current persistence layout is:

   1) cache_header_t
   2) SessionCache
   3) ClientCache

   update WOLFSSL_CACHE_VERSION if change layout for the following
   PERSISTENT_SESSION_CACHE functions
*/


/* get how big the the session cache save buffer needs to be */
int wolfSSL_get_session_cache_memsize(void)
{
    int sz  = (int)(sizeof(SessionCache) + sizeof(cache_header_t));

    #ifndef NO_CLIENT_CACHE
        sz += (int)(sizeof(ClientCache));
    #endif

    return sz;
}


/* Persist session cache to memory */
int wolfSSL_memsave_session_cache(void* mem, int sz)
{
    int i;
    cache_header_t cache_header;
    SessionRow*    row  = (SessionRow*)((byte*)mem + sizeof(cache_header));
#ifndef NO_CLIENT_CACHE
    ClientRow*     clRow;
#endif

    WOLFSSL_ENTER("wolfSSL_memsave_session_cache");

    if (sz < wolfSSL_get_session_cache_memsize()) {
        WOLFSSL_MSG("Memory buffer too small");
        return BUFFER_E;
    }

    cache_header.version   = WOLFSSL_CACHE_VERSION;
    cache_header.rows      = SESSION_ROWS;
    cache_header.columns   = SESSIONS_PER_ROW;
    cache_header.sessionSz = (int)sizeof(WOLFSSL_SESSION);
    XMEMCPY(mem, &cache_header, sizeof(cache_header));

    if (wc_LockMutex(&session_mutex) != 0) {
        WOLFSSL_MSG("Session cache mutex lock failed");
        return BAD_MUTEX_E;
    }

    for (i = 0; i < cache_header.rows; ++i)
        XMEMCPY(row++, SessionCache + i, sizeof(SessionRow));

#ifndef NO_CLIENT_CACHE
    clRow = (ClientRow*)row;
    for (i = 0; i < cache_header.rows; ++i)
        XMEMCPY(clRow++, ClientCache + i, sizeof(ClientRow));
#endif

    wc_UnLockMutex(&session_mutex);

    WOLFSSL_LEAVE("wolfSSL_memsave_session_cache", WOLFSSL_SUCCESS);

    return WOLFSSL_SUCCESS;
}


/* Restore the persistent session cache from memory */
int wolfSSL_memrestore_session_cache(const void* mem, int sz)
{
    int    i;
    cache_header_t cache_header;
    SessionRow*    row  = (SessionRow*)((byte*)mem + sizeof(cache_header));
#ifndef NO_CLIENT_CACHE
    ClientRow*     clRow;
#endif

    WOLFSSL_ENTER("wolfSSL_memrestore_session_cache");

    if (sz < wolfSSL_get_session_cache_memsize()) {
        WOLFSSL_MSG("Memory buffer too small");
        return BUFFER_E;
    }

    XMEMCPY(&cache_header, mem, sizeof(cache_header));
    if (cache_header.version   != WOLFSSL_CACHE_VERSION ||
        cache_header.rows      != SESSION_ROWS ||
        cache_header.columns   != SESSIONS_PER_ROW ||
        cache_header.sessionSz != (int)sizeof(WOLFSSL_SESSION)) {

        WOLFSSL_MSG("Session cache header match failed");
        return CACHE_MATCH_ERROR;
    }

    if (wc_LockMutex(&session_mutex) != 0) {
        WOLFSSL_MSG("Session cache mutex lock failed");
        return BAD_MUTEX_E;
    }

    for (i = 0; i < cache_header.rows; ++i)
        XMEMCPY(SessionCache + i, row++, sizeof(SessionRow));

#ifndef NO_CLIENT_CACHE
    clRow = (ClientRow*)row;
    for (i = 0; i < cache_header.rows; ++i)
        XMEMCPY(ClientCache + i, clRow++, sizeof(ClientRow));
#endif

    wc_UnLockMutex(&session_mutex);

    WOLFSSL_LEAVE("wolfSSL_memrestore_session_cache", WOLFSSL_SUCCESS);

    return WOLFSSL_SUCCESS;
}

#if !defined(NO_FILESYSTEM)

/* Persist session cache to file */
/* doesn't use memsave because of additional memory use */
int wolfSSL_save_session_cache(const char *fname)
{
    XFILE  file;
    int    ret;
    int    rc = WOLFSSL_SUCCESS;
    int    i;
    cache_header_t cache_header;

    WOLFSSL_ENTER("wolfSSL_save_session_cache");

    file = XFOPEN(fname, "w+b");
    if (file == XBADFILE) {
        WOLFSSL_MSG("Couldn't open session cache save file");
        return WOLFSSL_BAD_FILE;
    }
    cache_header.version   = WOLFSSL_CACHE_VERSION;
    cache_header.rows      = SESSION_ROWS;
    cache_header.columns   = SESSIONS_PER_ROW;
    cache_header.sessionSz = (int)sizeof(WOLFSSL_SESSION);

    /* cache header */
    ret = (int)XFWRITE(&cache_header, sizeof cache_header, 1, file);
    if (ret != 1) {
        WOLFSSL_MSG("Session cache header file write failed");
        XFCLOSE(file);
        return FWRITE_ERROR;
    }

    if (wc_LockMutex(&session_mutex) != 0) {
        WOLFSSL_MSG("Session cache mutex lock failed");
        XFCLOSE(file);
        return BAD_MUTEX_E;
    }

    /* session cache */
    for (i = 0; i < cache_header.rows; ++i) {
        ret = (int)XFWRITE(SessionCache + i, sizeof(SessionRow), 1, file);
        if (ret != 1) {
            WOLFSSL_MSG("Session cache member file write failed");
            rc = FWRITE_ERROR;
            break;
        }
    }

#ifndef NO_CLIENT_CACHE
    /* client cache */
    for (i = 0; i < cache_header.rows; ++i) {
        ret = (int)XFWRITE(ClientCache + i, sizeof(ClientRow), 1, file);
        if (ret != 1) {
            WOLFSSL_MSG("Client cache member file write failed");
            rc = FWRITE_ERROR;
            break;
        }
    }
#endif /* NO_CLIENT_CACHE */

    wc_UnLockMutex(&session_mutex);

    XFCLOSE(file);
    WOLFSSL_LEAVE("wolfSSL_save_session_cache", rc);

    return rc;
}


/* Restore the persistent session cache from file */
/* doesn't use memstore because of additional memory use */
int wolfSSL_restore_session_cache(const char *fname)
{
    XFILE  file;
    int    rc = WOLFSSL_SUCCESS;
    int    ret;
    int    i;
    cache_header_t cache_header;

    WOLFSSL_ENTER("wolfSSL_restore_session_cache");

    file = XFOPEN(fname, "rb");
    if (file == XBADFILE) {
        WOLFSSL_MSG("Couldn't open session cache save file");
        return WOLFSSL_BAD_FILE;
    }
    /* cache header */
    ret = (int)XFREAD(&cache_header, sizeof cache_header, 1, file);
    if (ret != 1) {
        WOLFSSL_MSG("Session cache header file read failed");
        XFCLOSE(file);
        return FREAD_ERROR;
    }
    if (cache_header.version   != WOLFSSL_CACHE_VERSION ||
        cache_header.rows      != SESSION_ROWS ||
        cache_header.columns   != SESSIONS_PER_ROW ||
        cache_header.sessionSz != (int)sizeof(WOLFSSL_SESSION)) {

        WOLFSSL_MSG("Session cache header match failed");
        XFCLOSE(file);
        return CACHE_MATCH_ERROR;
    }

    if (wc_LockMutex(&session_mutex) != 0) {
        WOLFSSL_MSG("Session cache mutex lock failed");
        XFCLOSE(file);
        return BAD_MUTEX_E;
    }

    /* session cache */
    for (i = 0; i < cache_header.rows; ++i) {
        ret = (int)XFREAD(SessionCache + i, sizeof(SessionRow), 1, file);
        if (ret != 1) {
            WOLFSSL_MSG("Session cache member file read failed");
            XMEMSET(SessionCache, 0, sizeof SessionCache);
            rc = FREAD_ERROR;
            break;
        }
    }

#ifndef NO_CLIENT_CACHE
    /* client cache */
    for (i = 0; i < cache_header.rows; ++i) {
        ret = (int)XFREAD(ClientCache + i, sizeof(ClientRow), 1, file);
        if (ret != 1) {
            WOLFSSL_MSG("Client cache member file read failed");
            XMEMSET(ClientCache, 0, sizeof ClientCache);
            rc = FREAD_ERROR;
            break;
        }
    }

#endif /* NO_CLIENT_CACHE */

    wc_UnLockMutex(&session_mutex);

    XFCLOSE(file);
    WOLFSSL_LEAVE("wolfSSL_restore_session_cache", rc);

    return rc;
}

#endif /* !NO_FILESYSTEM */
#endif /* PERSIST_SESSION_CACHE */
#endif /* NO_SESSION_CACHE */


void wolfSSL_load_error_strings(void)   /* compatibility only */
{}


int wolfSSL_library_init(void)
{
    WOLFSSL_ENTER("SSL_library_init");
    if (wolfSSL_Init() == WOLFSSL_SUCCESS)
        return WOLFSSL_SUCCESS;
    else
        return WOLFSSL_FATAL_ERROR;
}


#ifdef HAVE_SECRET_CALLBACK

int wolfSSL_set_session_secret_cb(WOLFSSL* ssl, SessionSecretCb cb, void* ctx)
{
    WOLFSSL_ENTER("wolfSSL_set_session_secret_cb");
    if (ssl == NULL)
        return WOLFSSL_FATAL_ERROR;

    ssl->sessionSecretCb = cb;
    ssl->sessionSecretCtx = ctx;
    /* If using a pre-set key, assume session resumption. */
    ssl->session.sessionIDSz = 0;
    ssl->options.resuming = 1;

    return WOLFSSL_SUCCESS;
}

#endif


#ifndef NO_SESSION_CACHE

/* on by default if built in but allow user to turn off */
long wolfSSL_CTX_set_session_cache_mode(WOLFSSL_CTX* ctx, long mode)
{
    WOLFSSL_ENTER("SSL_CTX_set_session_cache_mode");
    if (mode == WOLFSSL_SESS_CACHE_OFF)
        ctx->sessionCacheOff = 1;

    if ((mode & WOLFSSL_SESS_CACHE_NO_AUTO_CLEAR) != 0)
        ctx->sessionCacheFlushOff = 1;

#ifdef HAVE_EXT_CACHE
    if ((mode & WOLFSSL_SESS_CACHE_NO_INTERNAL_STORE) != 0)
        ctx->internalCacheOff = 1;
#endif

    return WOLFSSL_SUCCESS;
}

#endif /* NO_SESSION_CACHE */


#if !defined(NO_CERTS)
#if defined(PERSIST_CERT_CACHE)


#define WOLFSSL_CACHE_CERT_VERSION 1

typedef struct {
    int version;                 /* cache cert layout version id */
    int rows;                    /* hash table rows, CA_TABLE_SIZE */
    int columns[CA_TABLE_SIZE];  /* columns per row on list */
    int signerSz;                /* sizeof Signer object */
} CertCacheHeader;

/* current cert persistence layout is:

   1) CertCacheHeader
   2) caTable

   update WOLFSSL_CERT_CACHE_VERSION if change layout for the following
   PERSIST_CERT_CACHE functions
*/


/* Return memory needed to persist this signer, have lock */
static INLINE int GetSignerMemory(Signer* signer)
{
    int sz = sizeof(signer->pubKeySize) + sizeof(signer->keyOID)
           + sizeof(signer->nameLen)    + sizeof(signer->subjectNameHash);

#if !defined(NO_SKID)
        sz += (int)sizeof(signer->subjectKeyIdHash);
#endif

    /* add dynamic bytes needed */
    sz += signer->pubKeySize;
    sz += signer->nameLen;

    return sz;
}


/* Return memory needed to persist this row, have lock */
static INLINE int GetCertCacheRowMemory(Signer* row)
{
    int sz = 0;

    while (row) {
        sz += GetSignerMemory(row);
        row = row->next;
    }

    return sz;
}


/* get the size of persist cert cache, have lock */
static INLINE int GetCertCacheMemSize(WOLFSSL_CERT_MANAGER* cm)
{
    int sz;
    int i;

    sz = sizeof(CertCacheHeader);

    for (i = 0; i < CA_TABLE_SIZE; i++)
        sz += GetCertCacheRowMemory(cm->caTable[i]);

    return sz;
}


/* Store cert cache header columns with number of items per list, have lock */
static INLINE void SetCertHeaderColumns(WOLFSSL_CERT_MANAGER* cm, int* columns)
{
    int     i;
    Signer* row;

    for (i = 0; i < CA_TABLE_SIZE; i++) {
        int count = 0;
        row = cm->caTable[i];

        while (row) {
            ++count;
            row = row->next;
        }
        columns[i] = count;
    }
}


/* Restore whole cert row from memory, have lock, return bytes consumed,
   < 0 on error, have lock */
static INLINE int RestoreCertRow(WOLFSSL_CERT_MANAGER* cm, byte* current,
                                 int row, int listSz, const byte* end)
{
    int idx = 0;

    if (listSz < 0) {
        WOLFSSL_MSG("Row header corrupted, negative value");
        return PARSE_ERROR;
    }

    while (listSz) {
        Signer* signer;
        byte*   start = current + idx;  /* for end checks on this signer */
        int     minSz = sizeof(signer->pubKeySize) + sizeof(signer->keyOID) +
                      sizeof(signer->nameLen) + sizeof(signer->subjectNameHash);
        #ifndef NO_SKID
                minSz += (int)sizeof(signer->subjectKeyIdHash);
        #endif

        if (start + minSz > end) {
            WOLFSSL_MSG("Would overread restore buffer");
            return BUFFER_E;
        }
        signer = MakeSigner(cm->heap);
        if (signer == NULL)
            return MEMORY_E;

        /* pubKeySize */
        XMEMCPY(&signer->pubKeySize, current + idx, sizeof(signer->pubKeySize));
        idx += (int)sizeof(signer->pubKeySize);

        /* keyOID */
        XMEMCPY(&signer->keyOID, current + idx, sizeof(signer->keyOID));
        idx += (int)sizeof(signer->keyOID);

        /* pulicKey */
        if (start + minSz + signer->pubKeySize > end) {
            WOLFSSL_MSG("Would overread restore buffer");
            FreeSigner(signer, cm->heap);
            return BUFFER_E;
        }
        signer->publicKey = (byte*)XMALLOC(signer->pubKeySize, cm->heap,
                                           DYNAMIC_TYPE_KEY);
        if (signer->publicKey == NULL) {
            FreeSigner(signer, cm->heap);
            return MEMORY_E;
        }

        XMEMCPY(signer->publicKey, current + idx, signer->pubKeySize);
        idx += signer->pubKeySize;

        /* nameLen */
        XMEMCPY(&signer->nameLen, current + idx, sizeof(signer->nameLen));
        idx += (int)sizeof(signer->nameLen);

        /* name */
        if (start + minSz + signer->pubKeySize + signer->nameLen > end) {
            WOLFSSL_MSG("Would overread restore buffer");
            FreeSigner(signer, cm->heap);
            return BUFFER_E;
        }
        signer->name = (char*)XMALLOC(signer->nameLen, cm->heap,
                                      DYNAMIC_TYPE_SUBJECT_CN);
        if (signer->name == NULL) {
            FreeSigner(signer, cm->heap);
            return MEMORY_E;
        }

        XMEMCPY(signer->name, current + idx, signer->nameLen);
        idx += signer->nameLen;

        /* subjectNameHash */
        XMEMCPY(signer->subjectNameHash, current + idx, SIGNER_DIGEST_SIZE);
        idx += SIGNER_DIGEST_SIZE;

        #ifndef NO_SKID
            /* subjectKeyIdHash */
            XMEMCPY(signer->subjectKeyIdHash, current + idx,SIGNER_DIGEST_SIZE);
            idx += SIGNER_DIGEST_SIZE;
        #endif

        signer->next = cm->caTable[row];
        cm->caTable[row] = signer;

        --listSz;
    }

    return idx;
}


/* Store whole cert row into memory, have lock, return bytes added */
static INLINE int StoreCertRow(WOLFSSL_CERT_MANAGER* cm, byte* current, int row)
{
    int     added  = 0;
    Signer* list   = cm->caTable[row];

    while (list) {
        XMEMCPY(current + added, &list->pubKeySize, sizeof(list->pubKeySize));
        added += (int)sizeof(list->pubKeySize);

        XMEMCPY(current + added, &list->keyOID,     sizeof(list->keyOID));
        added += (int)sizeof(list->keyOID);

        XMEMCPY(current + added, list->publicKey, list->pubKeySize);
        added += list->pubKeySize;

        XMEMCPY(current + added, &list->nameLen, sizeof(list->nameLen));
        added += (int)sizeof(list->nameLen);

        XMEMCPY(current + added, list->name, list->nameLen);
        added += list->nameLen;

        XMEMCPY(current + added, list->subjectNameHash, SIGNER_DIGEST_SIZE);
        added += SIGNER_DIGEST_SIZE;

        #ifndef NO_SKID
            XMEMCPY(current + added, list->subjectKeyIdHash,SIGNER_DIGEST_SIZE);
            added += SIGNER_DIGEST_SIZE;
        #endif

        list = list->next;
    }

    return added;
}


/* Persist cert cache to memory, have lock */
static INLINE int DoMemSaveCertCache(WOLFSSL_CERT_MANAGER* cm,
                                     void* mem, int sz)
{
    int realSz;
    int ret = WOLFSSL_SUCCESS;
    int i;

    WOLFSSL_ENTER("DoMemSaveCertCache");

    realSz = GetCertCacheMemSize(cm);
    if (realSz > sz) {
        WOLFSSL_MSG("Mem output buffer too small");
        ret = BUFFER_E;
    }
    else {
        byte*           current;
        CertCacheHeader hdr;

        hdr.version  = WOLFSSL_CACHE_CERT_VERSION;
        hdr.rows     = CA_TABLE_SIZE;
        SetCertHeaderColumns(cm, hdr.columns);
        hdr.signerSz = (int)sizeof(Signer);

        XMEMCPY(mem, &hdr, sizeof(CertCacheHeader));
        current = (byte*)mem + sizeof(CertCacheHeader);

        for (i = 0; i < CA_TABLE_SIZE; ++i)
            current += StoreCertRow(cm, current, i);
    }

    return ret;
}


#if !defined(NO_FILESYSTEM)

/* Persist cert cache to file */
int CM_SaveCertCache(WOLFSSL_CERT_MANAGER* cm, const char* fname)
{
    XFILE file;
    int   rc = WOLFSSL_SUCCESS;
    int   memSz;
    byte* mem;

    WOLFSSL_ENTER("CM_SaveCertCache");

    file = XFOPEN(fname, "w+b");
    if (file == XBADFILE) {
       WOLFSSL_MSG("Couldn't open cert cache save file");
       return WOLFSSL_BAD_FILE;
    }

    if (wc_LockMutex(&cm->caLock) != 0) {
        WOLFSSL_MSG("wc_LockMutex on caLock failed");
        XFCLOSE(file);
        return BAD_MUTEX_E;
    }

    memSz = GetCertCacheMemSize(cm);
    mem   = (byte*)XMALLOC(memSz, cm->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (mem == NULL) {
        WOLFSSL_MSG("Alloc for tmp buffer failed");
        rc = MEMORY_E;
    } else {
        rc = DoMemSaveCertCache(cm, mem, memSz);
        if (rc == WOLFSSL_SUCCESS) {
            int ret = (int)XFWRITE(mem, memSz, 1, file);
            if (ret != 1) {
                WOLFSSL_MSG("Cert cache file write failed");
                rc = FWRITE_ERROR;
            }
        }
        XFREE(mem, cm->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }

    wc_UnLockMutex(&cm->caLock);
    XFCLOSE(file);

    return rc;
}


/* Restore cert cache from file */
int CM_RestoreCertCache(WOLFSSL_CERT_MANAGER* cm, const char* fname)
{
    XFILE file;
    int   rc = WOLFSSL_SUCCESS;
    int   ret;
    int   memSz;
    byte* mem;

    WOLFSSL_ENTER("CM_RestoreCertCache");

    file = XFOPEN(fname, "rb");
    if (file == XBADFILE) {
       WOLFSSL_MSG("Couldn't open cert cache save file");
       return WOLFSSL_BAD_FILE;
    }

    XFSEEK(file, 0, XSEEK_END);
    memSz = (int)XFTELL(file);
    XREWIND(file);

    if (memSz <= 0) {
        WOLFSSL_MSG("Bad file size");
        XFCLOSE(file);
        return WOLFSSL_BAD_FILE;
    }

    mem = (byte*)XMALLOC(memSz, cm->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (mem == NULL) {
        WOLFSSL_MSG("Alloc for tmp buffer failed");
        XFCLOSE(file);
        return MEMORY_E;
    }

    ret = (int)XFREAD(mem, memSz, 1, file);
    if (ret != 1) {
        WOLFSSL_MSG("Cert file read error");
        rc = FREAD_ERROR;
    } else {
        rc = CM_MemRestoreCertCache(cm, mem, memSz);
        if (rc != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("Mem restore cert cache failed");
        }
    }

    XFREE(mem, cm->heap, DYNAMIC_TYPE_TMP_BUFFER);
    XFCLOSE(file);

    return rc;
}

#endif /* NO_FILESYSTEM */


/* Persist cert cache to memory */
int CM_MemSaveCertCache(WOLFSSL_CERT_MANAGER* cm, void* mem, int sz, int* used)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("CM_MemSaveCertCache");

    if (wc_LockMutex(&cm->caLock) != 0) {
        WOLFSSL_MSG("wc_LockMutex on caLock failed");
        return BAD_MUTEX_E;
    }

    ret = DoMemSaveCertCache(cm, mem, sz);
    if (ret == WOLFSSL_SUCCESS)
        *used  = GetCertCacheMemSize(cm);

    wc_UnLockMutex(&cm->caLock);

    return ret;
}


/* Restore cert cache from memory */
int CM_MemRestoreCertCache(WOLFSSL_CERT_MANAGER* cm, const void* mem, int sz)
{
    int ret = WOLFSSL_SUCCESS;
    int i;
    CertCacheHeader* hdr = (CertCacheHeader*)mem;
    byte*            current = (byte*)mem + sizeof(CertCacheHeader);
    byte*            end     = (byte*)mem + sz;  /* don't go over */

    WOLFSSL_ENTER("CM_MemRestoreCertCache");

    if (current > end) {
        WOLFSSL_MSG("Cert Cache Memory buffer too small");
        return BUFFER_E;
    }

    if (hdr->version  != WOLFSSL_CACHE_CERT_VERSION ||
        hdr->rows     != CA_TABLE_SIZE ||
        hdr->signerSz != (int)sizeof(Signer)) {

        WOLFSSL_MSG("Cert Cache Memory header mismatch");
        return CACHE_MATCH_ERROR;
    }

    if (wc_LockMutex(&cm->caLock) != 0) {
        WOLFSSL_MSG("wc_LockMutex on caLock failed");
        return BAD_MUTEX_E;
    }

    FreeSignerTable(cm->caTable, CA_TABLE_SIZE, cm->heap);

    for (i = 0; i < CA_TABLE_SIZE; ++i) {
        int added = RestoreCertRow(cm, current, i, hdr->columns[i], end);
        if (added < 0) {
            WOLFSSL_MSG("RestoreCertRow error");
            ret = added;
            break;
        }
        current += added;
    }

    wc_UnLockMutex(&cm->caLock);

    return ret;
}


/* get how big the the cert cache save buffer needs to be */
int CM_GetCertCacheMemSize(WOLFSSL_CERT_MANAGER* cm)
{
    int sz;

    WOLFSSL_ENTER("CM_GetCertCacheMemSize");

    if (wc_LockMutex(&cm->caLock) != 0) {
        WOLFSSL_MSG("wc_LockMutex on caLock failed");
        return BAD_MUTEX_E;
    }

    sz = GetCertCacheMemSize(cm);

    wc_UnLockMutex(&cm->caLock);

    return sz;
}

#endif /* PERSIST_CERT_CACHE */
#endif /* NO_CERTS */


int wolfSSL_CTX_set_cipher_list(WOLFSSL_CTX* ctx, const char* list)
{
    WOLFSSL_ENTER("wolfSSL_CTX_set_cipher_list");

    /* alloc/init on demand only */
    if (ctx->suites == NULL) {
        ctx->suites = (Suites*)XMALLOC(sizeof(Suites), ctx->heap,
                                       DYNAMIC_TYPE_SUITES);
        if (ctx->suites == NULL) {
            WOLFSSL_MSG("Memory alloc for Suites failed");
            return WOLFSSL_FAILURE;
        }
        XMEMSET(ctx->suites, 0, sizeof(Suites));
    }

    return (SetCipherList(ctx, ctx->suites, list)) ? WOLFSSL_SUCCESS : WOLFSSL_FAILURE;
}


int wolfSSL_set_cipher_list(WOLFSSL* ssl, const char* list)
{
    WOLFSSL_ENTER("wolfSSL_set_cipher_list");
    return (SetCipherList(ssl->ctx, ssl->suites, list)) ? WOLFSSL_SUCCESS : WOLFSSL_FAILURE;
}


int wolfSSL_dtls_get_using_nonblock(WOLFSSL* ssl)
{
    int useNb = 0;

    WOLFSSL_ENTER("wolfSSL_dtls_get_using_nonblock");
    if (ssl->options.dtls) {
#ifdef WOLFSSL_DTLS
        useNb = ssl->options.dtlsUseNonblock;
#endif
    }
    else {
        WOLFSSL_MSG("wolfSSL_dtls_get_using_nonblock() is "
                    "DEPRECATED for non-DTLS use.");
    }
    return useNb;
}


#ifndef WOLFSSL_LEANPSK

void wolfSSL_dtls_set_using_nonblock(WOLFSSL* ssl, int nonblock)
{
    (void)nonblock;

    WOLFSSL_ENTER("wolfSSL_dtls_set_using_nonblock");
    if (ssl->options.dtls) {
#ifdef WOLFSSL_DTLS
        ssl->options.dtlsUseNonblock = (nonblock != 0);
#endif
    }
    else {
        WOLFSSL_MSG("wolfSSL_dtls_set_using_nonblock() is "
                    "DEPRECATED for non-DTLS use.");
    }
}


#ifdef WOLFSSL_DTLS

int wolfSSL_dtls_get_current_timeout(WOLFSSL* ssl)
{
    return ssl->dtls_timeout;
}


/* user may need to alter init dtls recv timeout, WOLFSSL_SUCCESS on ok */
int wolfSSL_dtls_set_timeout_init(WOLFSSL* ssl, int timeout)
{
    if (ssl == NULL || timeout < 0)
        return BAD_FUNC_ARG;

    if (timeout > ssl->dtls_timeout_max) {
        WOLFSSL_MSG("Can't set dtls timeout init greater than dtls timeout max");
        return BAD_FUNC_ARG;
    }

    ssl->dtls_timeout_init = timeout;
    ssl->dtls_timeout = timeout;

    return WOLFSSL_SUCCESS;
}


/* user may need to alter max dtls recv timeout, WOLFSSL_SUCCESS on ok */
int wolfSSL_dtls_set_timeout_max(WOLFSSL* ssl, int timeout)
{
    if (ssl == NULL || timeout < 0)
        return BAD_FUNC_ARG;

    if (timeout < ssl->dtls_timeout_init) {
        WOLFSSL_MSG("Can't set dtls timeout max less than dtls timeout init");
        return BAD_FUNC_ARG;
    }

    ssl->dtls_timeout_max = timeout;

    return WOLFSSL_SUCCESS;
}


int wolfSSL_dtls_got_timeout(WOLFSSL* ssl)
{
    int result = WOLFSSL_SUCCESS;

    if (!ssl->options.handShakeDone &&
        (DtlsMsgPoolTimeout(ssl) < 0 || DtlsMsgPoolSend(ssl, 0) < 0)) {

        result = WOLFSSL_FATAL_ERROR;
    }
    return result;
}

#endif /* DTLS */
#endif /* LEANPSK */


#if defined(WOLFSSL_DTLS) && !defined(NO_WOLFSSL_SERVER)

/* Not an SSL function, return 0 for success, error code otherwise */
/* Prereq: ssl's RNG needs to be initialized. */
int wolfSSL_DTLS_SetCookieSecret(WOLFSSL* ssl,
                                 const byte* secret, word32 secretSz)
{
    int ret = 0;

    WOLFSSL_ENTER("wolfSSL_DTLS_SetCookieSecret");

    if (ssl == NULL) {
        WOLFSSL_MSG("need a SSL object");
        return BAD_FUNC_ARG;
    }

    if (secret != NULL && secretSz == 0) {
        WOLFSSL_MSG("can't have a new secret without a size");
        return BAD_FUNC_ARG;
    }

    /* If secretSz is 0, use the default size. */
    if (secretSz == 0)
        secretSz = COOKIE_SECRET_SZ;

    if (secretSz != ssl->buffers.dtlsCookieSecret.length) {
        byte* newSecret;

        if (ssl->buffers.dtlsCookieSecret.buffer != NULL) {
            ForceZero(ssl->buffers.dtlsCookieSecret.buffer,
                      ssl->buffers.dtlsCookieSecret.length);
            XFREE(ssl->buffers.dtlsCookieSecret.buffer,
                  ssl->heap, DYNAMIC_TYPE_NONE);
        }

        newSecret = (byte*)XMALLOC(secretSz, ssl->heap,DYNAMIC_TYPE_COOKIE_PWD);
        if (newSecret == NULL) {
            ssl->buffers.dtlsCookieSecret.buffer = NULL;
            ssl->buffers.dtlsCookieSecret.length = 0;
            WOLFSSL_MSG("couldn't allocate new cookie secret");
            return MEMORY_ERROR;
        }
        ssl->buffers.dtlsCookieSecret.buffer = newSecret;
        ssl->buffers.dtlsCookieSecret.length = secretSz;
    }

    /* If the supplied secret is NULL, randomly generate a new secret. */
    if (secret == NULL) {
        ret = wc_RNG_GenerateBlock(ssl->rng,
                             ssl->buffers.dtlsCookieSecret.buffer, secretSz);
    }
    else
        XMEMCPY(ssl->buffers.dtlsCookieSecret.buffer, secret, secretSz);

    WOLFSSL_LEAVE("wolfSSL_DTLS_SetCookieSecret", 0);
    return ret;
}

#endif /* WOLFSSL_DTLS && !NO_WOLFSSL_SERVER */

#ifdef OPENSSL_EXTRA
    WOLFSSL_METHOD* wolfSSLv23_method(void) {
        WOLFSSL_METHOD* m = NULL;
        WOLFSSL_ENTER("wolfSSLv23_method");
#if !defined(NO_WOLFSSL_CLIENT)
        m = wolfSSLv23_client_method();
#elif !defined(NO_WOLFSSL_SERVER)
        m = wolfSSLv23_server_method();
#endif
        if (m != NULL) {
            m->side = WOLFSSL_NEITHER_END;
        }

        return m;
    }
#endif /* OPENSSL_EXTRA */

/* client only parts */
#ifndef NO_WOLFSSL_CLIENT

    #if defined(WOLFSSL_ALLOW_SSLV3) && !defined(NO_OLD_TLS)
    WOLFSSL_METHOD* wolfSSLv3_client_method(void)
    {
        WOLFSSL_ENTER("SSLv3_client_method");
        return wolfSSLv3_client_method_ex(NULL);
    }
    #endif

    #ifdef WOLFSSL_DTLS

        #ifndef NO_OLD_TLS
        WOLFSSL_METHOD* wolfDTLSv1_client_method(void)
        {
            WOLFSSL_ENTER("DTLSv1_client_method");
            return wolfDTLSv1_client_method_ex(NULL);
        }
        #endif  /* NO_OLD_TLS */

        WOLFSSL_METHOD* wolfDTLSv1_2_client_method(void)
        {
            WOLFSSL_ENTER("DTLSv1_2_client_method");
            return wolfDTLSv1_2_client_method_ex(NULL);
        }
    #endif

    #if defined(WOLFSSL_ALLOW_SSLV3) && !defined(NO_OLD_TLS)
    WOLFSSL_METHOD* wolfSSLv3_client_method_ex(void* heap)
    {
        WOLFSSL_METHOD* method =
                              (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                     heap, DYNAMIC_TYPE_METHOD);
        WOLFSSL_ENTER("SSLv3_client_method_ex");
        if (method)
            InitSSL_Method(method, MakeSSLv3());
        return method;
    }
    #endif

    #ifdef WOLFSSL_DTLS

        #ifndef NO_OLD_TLS
        WOLFSSL_METHOD* wolfDTLSv1_client_method_ex(void* heap)
        {
            WOLFSSL_METHOD* method =
                              (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                     heap, DYNAMIC_TYPE_METHOD);
            WOLFSSL_ENTER("DTLSv1_client_method_ex");
            if (method)
                InitSSL_Method(method, MakeDTLSv1());
            return method;
        }
        #endif  /* NO_OLD_TLS */

        WOLFSSL_METHOD* wolfDTLSv1_2_client_method_ex(void* heap)
        {
            WOLFSSL_METHOD* method =
                              (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                     heap, DYNAMIC_TYPE_METHOD);
            WOLFSSL_ENTER("DTLSv1_2_client_method_ex");
            if (method)
                InitSSL_Method(method, MakeDTLSv1_2());
            (void)heap;
            return method;
        }
    #endif

    /* If SCTP is not enabled returns the state of the dtls option.
     * If SCTP is enabled returns dtls && !sctp. */
    static INLINE int IsDtlsNotSctpMode(WOLFSSL* ssl)
    {
        int result = ssl->options.dtls;

        if (result) {
        #ifdef WOLFSSL_SCTP
            result = !ssl->options.dtlsSctp;
        #endif
        }

        return result;
    }


    /* please see note at top of README if you get an error from connect */
    int wolfSSL_connect(WOLFSSL* ssl)
    {
    #ifndef WOLFSSL_NO_TLS12
        int neededState;
    #endif

        WOLFSSL_ENTER("SSL_connect()");

        #ifdef HAVE_ERRNO_H
            errno = 0;
        #endif

        if (ssl == NULL)
            return BAD_FUNC_ARG;

        #ifdef OPENSSL_EXTRA
            if (ssl->CBIS != NULL) {
                ssl->CBIS(ssl, SSL_ST_CONNECT, SSL_SUCCESS);
                ssl->cbmode = SSL_CB_WRITE;
            }
        #endif
        if (ssl->options.side != WOLFSSL_CLIENT_END) {
            WOLFSSL_ERROR(ssl->error = SIDE_ERROR);
            return WOLFSSL_FATAL_ERROR;
        }

    #ifdef WOLFSSL_NO_TLS12
        return wolfSSL_connect_TLSv13(ssl);
    #else
        #ifdef WOLFSSL_TLS13
            if (ssl->options.tls1_3)
                return wolfSSL_connect_TLSv13(ssl);
        #endif

        #ifdef WOLFSSL_DTLS
            if (ssl->version.major == DTLS_MAJOR) {
                ssl->options.dtls   = 1;
                ssl->options.tls    = 1;
                ssl->options.tls1_1 = 1;
            }
        #endif

        if (ssl->buffers.outputBuffer.length > 0) {
            if ( (ssl->error = SendBuffered(ssl)) == 0) {
                /* fragOffset is non-zero when sending fragments. On the last
                 * fragment, fragOffset is zero again, and the state can be
                 * advanced. */
                if (ssl->fragOffset == 0) {
                    ssl->options.connectState++;
                    WOLFSSL_MSG("connect state: "
                                "Advanced from last buffered fragment send");
                }
                else {
                    WOLFSSL_MSG("connect state: "
                                "Not advanced, more fragments to send");
                }
            }
            else {
                WOLFSSL_ERROR(ssl->error);
                return WOLFSSL_FATAL_ERROR;
            }
        }

#ifdef WOLFSSL_TLS13
        if (ssl->options.tls1_3)
            return wolfSSL_connect_TLSv13(ssl);
#endif

        switch (ssl->options.connectState) {

        case CONNECT_BEGIN :
            /* always send client hello first */
            if ( (ssl->error = SendClientHello(ssl)) != 0) {
                WOLFSSL_ERROR(ssl->error);
                return WOLFSSL_FATAL_ERROR;
            }
            ssl->options.connectState = CLIENT_HELLO_SENT;
            WOLFSSL_MSG("connect state: CLIENT_HELLO_SENT");
            FALL_THROUGH;

        case CLIENT_HELLO_SENT :
            neededState = ssl->options.resuming ? SERVER_FINISHED_COMPLETE :
                                          SERVER_HELLODONE_COMPLETE;
            #ifdef WOLFSSL_DTLS
                /* In DTLS, when resuming, we can go straight to FINISHED,
                 * or do a cookie exchange and then skip to FINISHED, assume
                 * we need the cookie exchange first. */
                if (IsDtlsNotSctpMode(ssl))
                    neededState = SERVER_HELLOVERIFYREQUEST_COMPLETE;
            #endif
            /* get response */
            while (ssl->options.serverState < neededState) {
                #ifdef WOLFSSL_TLS13
                    if (ssl->options.tls1_3)
                        return wolfSSL_connect_TLSv13(ssl);
                #endif
                if ( (ssl->error = ProcessReply(ssl)) < 0) {
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
                /* if resumption failed, reset needed state */
                else if (neededState == SERVER_FINISHED_COMPLETE)
                    if (!ssl->options.resuming) {
                        if (!IsDtlsNotSctpMode(ssl))
                            neededState = SERVER_HELLODONE_COMPLETE;
                        else
                            neededState = SERVER_HELLOVERIFYREQUEST_COMPLETE;
                    }
            }

            ssl->options.connectState = HELLO_AGAIN;
            WOLFSSL_MSG("connect state: HELLO_AGAIN");
            FALL_THROUGH;

        case HELLO_AGAIN :
            if (ssl->options.certOnly)
                return WOLFSSL_SUCCESS;

        #ifdef WOLFSSL_TLS13
            if (ssl->options.tls1_3)
                return wolfSSL_connect_TLSv13(ssl);
        #endif

            #ifdef WOLFSSL_DTLS
                if (IsDtlsNotSctpMode(ssl)) {
                    /* re-init hashes, exclude first hello and verify request */
                    if ((ssl->error = InitHandshakeHashes(ssl)) != 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }
                    if ( (ssl->error = SendClientHello(ssl)) != 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }
                }
            #endif

            ssl->options.connectState = HELLO_AGAIN_REPLY;
            WOLFSSL_MSG("connect state: HELLO_AGAIN_REPLY");
            FALL_THROUGH;

        case HELLO_AGAIN_REPLY :
            #ifdef WOLFSSL_DTLS
                if (IsDtlsNotSctpMode(ssl)) {
                    neededState = ssl->options.resuming ?
                           SERVER_FINISHED_COMPLETE : SERVER_HELLODONE_COMPLETE;

                    /* get response */
                    while (ssl->options.serverState < neededState) {
                        if ( (ssl->error = ProcessReply(ssl)) < 0) {
                            WOLFSSL_ERROR(ssl->error);
                            return WOLFSSL_FATAL_ERROR;
                        }
                        /* if resumption failed, reset needed state */
                        if (neededState == SERVER_FINISHED_COMPLETE) {
                            if (!ssl->options.resuming)
                                neededState = SERVER_HELLODONE_COMPLETE;
                        }
                    }
                }
            #endif

            ssl->options.connectState = FIRST_REPLY_DONE;
            WOLFSSL_MSG("connect state: FIRST_REPLY_DONE");
            FALL_THROUGH;

        case FIRST_REPLY_DONE :
            #if !defined(NO_CERTS) && !defined(WOLFSSL_NO_CLIENT_AUTH)
                #ifdef WOLFSSL_TLS13
                    if (ssl->options.tls1_3)
                        return wolfSSL_connect_TLSv13(ssl);
                #endif
                if (ssl->options.sendVerify) {
                    if ( (ssl->error = SendCertificate(ssl)) != 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }
                    WOLFSSL_MSG("sent: certificate");
                }

            #endif
            ssl->options.connectState = FIRST_REPLY_FIRST;
            WOLFSSL_MSG("connect state: FIRST_REPLY_FIRST");
            FALL_THROUGH;

        case FIRST_REPLY_FIRST :
        #ifdef WOLFSSL_TLS13
            if (ssl->options.tls1_3)
                return wolfSSL_connect_TLSv13(ssl);
        #endif
            if (!ssl->options.resuming) {
                if ( (ssl->error = SendClientKeyExchange(ssl)) != 0) {
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
                WOLFSSL_MSG("sent: client key exchange");
            }

            ssl->options.connectState = FIRST_REPLY_SECOND;
            WOLFSSL_MSG("connect state: FIRST_REPLY_SECOND");
            FALL_THROUGH;

        case FIRST_REPLY_SECOND :
            #if !defined(NO_CERTS) && !defined(WOLFSSL_NO_CLIENT_AUTH)
                if (ssl->options.sendVerify) {
                    if ( (ssl->error = SendCertificateVerify(ssl)) != 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }
                    WOLFSSL_MSG("sent: certificate verify");
                }
            #endif /* !NO_CERTS && !WOLFSSL_NO_CLIENT_AUTH */
            ssl->options.connectState = FIRST_REPLY_THIRD;
            WOLFSSL_MSG("connect state: FIRST_REPLY_THIRD");
            FALL_THROUGH;

        case FIRST_REPLY_THIRD :
            if ( (ssl->error = SendChangeCipher(ssl)) != 0) {
                WOLFSSL_ERROR(ssl->error);
                return WOLFSSL_FATAL_ERROR;
            }
            WOLFSSL_MSG("sent: change cipher spec");
            ssl->options.connectState = FIRST_REPLY_FOURTH;
            WOLFSSL_MSG("connect state: FIRST_REPLY_FOURTH");
            FALL_THROUGH;

        case FIRST_REPLY_FOURTH :
            if ( (ssl->error = SendFinished(ssl)) != 0) {
                WOLFSSL_ERROR(ssl->error);
                return WOLFSSL_FATAL_ERROR;
            }
            WOLFSSL_MSG("sent: finished");
            ssl->options.connectState = FINISHED_DONE;
            WOLFSSL_MSG("connect state: FINISHED_DONE");
            FALL_THROUGH;

        case FINISHED_DONE :
            /* get response */
            while (ssl->options.serverState < SERVER_FINISHED_COMPLETE)
                if ( (ssl->error = ProcessReply(ssl)) < 0) {
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }

            ssl->options.connectState = SECOND_REPLY_DONE;
            WOLFSSL_MSG("connect state: SECOND_REPLY_DONE");
            FALL_THROUGH;

        case SECOND_REPLY_DONE:
#ifndef NO_HANDSHAKE_DONE_CB
            if (ssl->hsDoneCb) {
                int cbret = ssl->hsDoneCb(ssl, ssl->hsDoneCtx);
                if (cbret < 0) {
                    ssl->error = cbret;
                    WOLFSSL_MSG("HandShake Done Cb don't continue error");
                    return WOLFSSL_FATAL_ERROR;
                }
            }
#endif /* NO_HANDSHAKE_DONE_CB */

            if (!ssl->options.dtls) {
                if (!ssl->options.keepResources) {
                    FreeHandshakeResources(ssl);
                }
            }
#ifdef WOLFSSL_DTLS
            else {
                ssl->options.dtlsHsRetain = 1;
            }
#endif /* WOLFSSL_DTLS */

            WOLFSSL_LEAVE("SSL_connect()", WOLFSSL_SUCCESS);
            return WOLFSSL_SUCCESS;

        default:
            WOLFSSL_MSG("Unknown connect state ERROR");
            return WOLFSSL_FATAL_ERROR; /* unknown connect state */
        }
    #endif /* !WOLFSSL_NO_TLS12 */
    }

#endif /* NO_WOLFSSL_CLIENT */


/* server only parts */
#ifndef NO_WOLFSSL_SERVER

    #if defined(WOLFSSL_ALLOW_SSLV3) && !defined(NO_OLD_TLS)
    WOLFSSL_METHOD* wolfSSLv3_server_method(void)
    {
        WOLFSSL_ENTER("SSLv3_server_method");
        return wolfSSLv3_server_method_ex(NULL);
    }
    #endif


    #ifdef WOLFSSL_DTLS

        #ifndef NO_OLD_TLS
        WOLFSSL_METHOD* wolfDTLSv1_server_method(void)
        {
            WOLFSSL_ENTER("DTLSv1_server_method");
            return wolfDTLSv1_server_method_ex(NULL);
        }
        #endif /* NO_OLD_TLS */

        WOLFSSL_METHOD* wolfDTLSv1_2_server_method(void)
        {
            WOLFSSL_ENTER("DTLSv1_2_server_method");
            return wolfDTLSv1_2_server_method_ex(NULL);
        }
    #endif

    #if defined(WOLFSSL_ALLOW_SSLV3) && !defined(NO_OLD_TLS)
    WOLFSSL_METHOD* wolfSSLv3_server_method_ex(void* heap)
    {
        WOLFSSL_METHOD* method =
                              (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                     heap, DYNAMIC_TYPE_METHOD);
        WOLFSSL_ENTER("SSLv3_server_method_ex");
        if (method) {
            InitSSL_Method(method, MakeSSLv3());
            method->side = WOLFSSL_SERVER_END;
        }
        return method;
    }
    #endif


    #ifdef WOLFSSL_DTLS

        #ifndef NO_OLD_TLS
        WOLFSSL_METHOD* wolfDTLSv1_server_method_ex(void* heap)
        {
            WOLFSSL_METHOD* method =
                              (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                     heap, DYNAMIC_TYPE_METHOD);
            WOLFSSL_ENTER("DTLSv1_server_method_ex");
            if (method) {
                InitSSL_Method(method, MakeDTLSv1());
                method->side = WOLFSSL_SERVER_END;
            }
            return method;
        }
        #endif /* NO_OLD_TLS */

        WOLFSSL_METHOD* wolfDTLSv1_2_server_method_ex(void* heap)
        {
            WOLFSSL_METHOD* method =
                              (WOLFSSL_METHOD*) XMALLOC(sizeof(WOLFSSL_METHOD),
                                                     heap, DYNAMIC_TYPE_METHOD);
            WOLFSSL_ENTER("DTLSv1_2_server_method_ex");
            if (method) {
                InitSSL_Method(method, MakeDTLSv1_2());
                method->side = WOLFSSL_SERVER_END;
            }
            (void)heap;
            return method;
        }
    #endif


    int wolfSSL_accept(WOLFSSL* ssl)
    {
#ifndef WOLFSSL_NO_TLS12
        word16 havePSK = 0;
        word16 haveAnon = 0;
        word16 haveMcast = 0;
#endif

#ifdef WOLFSSL_NO_TLS12
        return wolfSSL_accept_TLSv13(ssl);
#else
    #ifdef WOLFSSL_TLS13
        if (ssl->options.tls1_3)
            return wolfSSL_accept_TLSv13(ssl);
    #endif
        WOLFSSL_ENTER("SSL_accept()");

        #ifdef HAVE_ERRNO_H
            errno = 0;
        #endif

        #ifndef NO_PSK
            havePSK = ssl->options.havePSK;
        #endif
        (void)havePSK;

        #ifdef HAVE_ANON
            haveAnon = ssl->options.haveAnon;
        #endif
        (void)haveAnon;

        #ifdef WOLFSSL_MULTICAST
            haveMcast = ssl->options.haveMcast;
        #endif
        (void)haveMcast;

        if (ssl->options.side != WOLFSSL_SERVER_END) {
            WOLFSSL_ERROR(ssl->error = SIDE_ERROR);
            return WOLFSSL_FATAL_ERROR;
        }

    #ifndef NO_CERTS
        /* in case used set_accept_state after init */
        /* allow no private key if using PK callbacks and CB is set */
        if (!havePSK && !haveAnon && !haveMcast) {
            if (!ssl->buffers.certificate ||
                !ssl->buffers.certificate->buffer) {

                WOLFSSL_MSG("accept error: server cert required");
                WOLFSSL_ERROR(ssl->error = NO_PRIVATE_KEY);
                return WOLFSSL_FATAL_ERROR;
            }

        #ifdef HAVE_PK_CALLBACKS
            if (wolfSSL_CTX_IsPrivatePkSet(ssl->ctx)) {
                WOLFSSL_MSG("Using PK for server private key");
            }
            else
        #endif
            if (!ssl->buffers.key || !ssl->buffers.key->buffer) {
                WOLFSSL_MSG("accept error: server key required");
                WOLFSSL_ERROR(ssl->error = NO_PRIVATE_KEY);
                return WOLFSSL_FATAL_ERROR;
            }
        }
    #endif

    #ifdef WOLFSSL_DTLS
        if (ssl->version.major == DTLS_MAJOR) {
            ssl->options.dtls   = 1;
            ssl->options.tls    = 1;
            ssl->options.tls1_1 = 1;
        }
    #endif

        if (ssl->buffers.outputBuffer.length > 0) {
            if ( (ssl->error = SendBuffered(ssl)) == 0) {
                /* fragOffset is non-zero when sending fragments. On the last
                 * fragment, fragOffset is zero again, and the state can be
                 * advanced. */
                if (ssl->fragOffset == 0) {
                    ssl->options.acceptState++;
                    WOLFSSL_MSG("accept state: "
                                "Advanced from last buffered fragment send");
                }
                else {
                    WOLFSSL_MSG("accept state: "
                                "Not advanced, more fragments to send");
                }
            }
            else {
                WOLFSSL_ERROR(ssl->error);
                return WOLFSSL_FATAL_ERROR;
            }
        }

        switch (ssl->options.acceptState) {

        case ACCEPT_BEGIN :
            /* get response */
            while (ssl->options.clientState < CLIENT_HELLO_COMPLETE)
                if ( (ssl->error = ProcessReply(ssl)) < 0) {
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
#ifdef WOLFSSL_TLS13
            ssl->options.acceptState = ACCEPT_CLIENT_HELLO_DONE;
            WOLFSSL_MSG("accept state ACCEPT_CLIENT_HELLO_DONE");
            FALL_THROUGH;

        case ACCEPT_CLIENT_HELLO_DONE :
            if (ssl->options.tls1_3) {
                return wolfSSL_accept_TLSv13(ssl);
            }
#endif
            ssl->options.acceptState = ACCEPT_FIRST_REPLY_DONE;
            WOLFSSL_MSG("accept state ACCEPT_FIRST_REPLY_DONE");
            FALL_THROUGH;

        case ACCEPT_FIRST_REPLY_DONE :
            if ( (ssl->error = SendServerHello(ssl)) != 0) {
                WOLFSSL_ERROR(ssl->error);
                return WOLFSSL_FATAL_ERROR;
            }
            ssl->options.acceptState = SERVER_HELLO_SENT;
            WOLFSSL_MSG("accept state SERVER_HELLO_SENT");
            FALL_THROUGH;

        case SERVER_HELLO_SENT :
        #ifdef WOLFSSL_TLS13
            if (ssl->options.tls1_3) {
                return wolfSSL_accept_TLSv13(ssl);
            }
        #endif
            #ifndef NO_CERTS
                if (!ssl->options.resuming)
                    if ( (ssl->error = SendCertificate(ssl)) != 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }
            #endif
            ssl->options.acceptState = CERT_SENT;
            WOLFSSL_MSG("accept state CERT_SENT");
            FALL_THROUGH;

        case CERT_SENT :
            #ifndef NO_CERTS
            if (!ssl->options.resuming)
                if ( (ssl->error = SendCertificateStatus(ssl)) != 0) {
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
            #endif
            ssl->options.acceptState = CERT_STATUS_SENT;
            WOLFSSL_MSG("accept state CERT_STATUS_SENT");
            FALL_THROUGH;

        case CERT_STATUS_SENT :
        #ifdef WOLFSSL_TLS13
            if (ssl->options.tls1_3) {
                return wolfSSL_accept_TLSv13(ssl);
            }
        #endif
            if (!ssl->options.resuming)
                if ( (ssl->error = SendServerKeyExchange(ssl)) != 0) {
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
            ssl->options.acceptState = KEY_EXCHANGE_SENT;
            WOLFSSL_MSG("accept state KEY_EXCHANGE_SENT");
            FALL_THROUGH;

        case KEY_EXCHANGE_SENT :
            #ifndef NO_CERTS
                if (!ssl->options.resuming) {
                    if (ssl->options.verifyPeer) {
                        if ( (ssl->error = SendCertificateRequest(ssl)) != 0) {
                            WOLFSSL_ERROR(ssl->error);
                            return WOLFSSL_FATAL_ERROR;
                        }
                    }
                }
            #endif
            ssl->options.acceptState = CERT_REQ_SENT;
            WOLFSSL_MSG("accept state CERT_REQ_SENT");
            FALL_THROUGH;

        case CERT_REQ_SENT :
            if (!ssl->options.resuming)
                if ( (ssl->error = SendServerHelloDone(ssl)) != 0) {
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
            ssl->options.acceptState = SERVER_HELLO_DONE;
            WOLFSSL_MSG("accept state SERVER_HELLO_DONE");
            FALL_THROUGH;

        case SERVER_HELLO_DONE :
            if (!ssl->options.resuming) {
                while (ssl->options.clientState < CLIENT_FINISHED_COMPLETE)
                    if ( (ssl->error = ProcessReply(ssl)) < 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }
            }
            ssl->options.acceptState = ACCEPT_SECOND_REPLY_DONE;
            WOLFSSL_MSG("accept state  ACCEPT_SECOND_REPLY_DONE");
            FALL_THROUGH;

        case ACCEPT_SECOND_REPLY_DONE :
#ifdef HAVE_SESSION_TICKET
            if (ssl->options.createTicket) {
                if ( (ssl->error = SendTicket(ssl)) != 0) {
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
            }
#endif /* HAVE_SESSION_TICKET */
            ssl->options.acceptState = TICKET_SENT;
            WOLFSSL_MSG("accept state  TICKET_SENT");
            FALL_THROUGH;

        case TICKET_SENT:
            if ( (ssl->error = SendChangeCipher(ssl)) != 0) {
                WOLFSSL_ERROR(ssl->error);
                return WOLFSSL_FATAL_ERROR;
            }
            ssl->options.acceptState = CHANGE_CIPHER_SENT;
            WOLFSSL_MSG("accept state  CHANGE_CIPHER_SENT");
            FALL_THROUGH;

        case CHANGE_CIPHER_SENT :
            if ( (ssl->error = SendFinished(ssl)) != 0) {
                WOLFSSL_ERROR(ssl->error);
                return WOLFSSL_FATAL_ERROR;
            }

            ssl->options.acceptState = ACCEPT_FINISHED_DONE;
            WOLFSSL_MSG("accept state ACCEPT_FINISHED_DONE");
            FALL_THROUGH;

        case ACCEPT_FINISHED_DONE :
            if (ssl->options.resuming)
                while (ssl->options.clientState < CLIENT_FINISHED_COMPLETE)
                    if ( (ssl->error = ProcessReply(ssl)) < 0) {
                        WOLFSSL_ERROR(ssl->error);
                        return WOLFSSL_FATAL_ERROR;
                    }

            ssl->options.acceptState = ACCEPT_THIRD_REPLY_DONE;
            WOLFSSL_MSG("accept state ACCEPT_THIRD_REPLY_DONE");
            FALL_THROUGH;

        case ACCEPT_THIRD_REPLY_DONE :
#ifndef NO_HANDSHAKE_DONE_CB
            if (ssl->hsDoneCb) {
                int cbret = ssl->hsDoneCb(ssl, ssl->hsDoneCtx);
                if (cbret < 0) {
                    ssl->error = cbret;
                    WOLFSSL_MSG("HandShake Done Cb don't continue error");
                    return WOLFSSL_FATAL_ERROR;
                }
            }
#endif /* NO_HANDSHAKE_DONE_CB */

            if (!ssl->options.dtls) {
                if (!ssl->options.keepResources) {
                    FreeHandshakeResources(ssl);
                }
            }
#ifdef WOLFSSL_DTLS
            else {
                ssl->options.dtlsHsRetain = 1;
            }
#endif /* WOLFSSL_DTLS */

#ifdef WOLFSSL_SESSION_EXPORT
            if (ssl->dtls_export) {
                if ((ssl->error = wolfSSL_send_session(ssl)) != 0) {
                    WOLFSSL_MSG("Export DTLS session error");
                    WOLFSSL_ERROR(ssl->error);
                    return WOLFSSL_FATAL_ERROR;
                }
            }
#endif

            WOLFSSL_LEAVE("SSL_accept()", WOLFSSL_SUCCESS);
            return WOLFSSL_SUCCESS;

        default :
            WOLFSSL_MSG("Unknown accept state ERROR");
            return WOLFSSL_FATAL_ERROR;
        }
#endif /* !WOLFSSL_NO_TLS12 */
    }

#endif /* NO_WOLFSSL_SERVER */


#ifndef NO_HANDSHAKE_DONE_CB

int wolfSSL_SetHsDoneCb(WOLFSSL* ssl, HandShakeDoneCb cb, void* user_ctx)
{
    WOLFSSL_ENTER("wolfSSL_SetHsDoneCb");

    if (ssl == NULL)
        return BAD_FUNC_ARG;

    ssl->hsDoneCb  = cb;
    ssl->hsDoneCtx = user_ctx;


    return WOLFSSL_SUCCESS;
}

#endif /* NO_HANDSHAKE_DONE_CB */

int wolfSSL_Cleanup(void)
{
    int ret = WOLFSSL_SUCCESS;
    int release = 0;

    WOLFSSL_ENTER("wolfSSL_Cleanup");

    if (initRefCount == 0)
        return ret;  /* possibly no init yet, but not failure either way */

    if (wc_LockMutex(&count_mutex) != 0) {
        WOLFSSL_MSG("Bad Lock Mutex count");
        return BAD_MUTEX_E;
    }

    release = initRefCount-- == 1;
    if (initRefCount < 0)
        initRefCount = 0;

    wc_UnLockMutex(&count_mutex);

    if (!release)
        return ret;

#ifndef NO_SESSION_CACHE
    if (wc_FreeMutex(&session_mutex) != 0)
        ret = BAD_MUTEX_E;
#endif
    if (wc_FreeMutex(&count_mutex) != 0)
        ret = BAD_MUTEX_E;

    if (wolfCrypt_Cleanup() != 0) {
        WOLFSSL_MSG("Error with wolfCrypt_Cleanup call");
        ret = WC_CLEANUP_E;
    }

    return ret;
}


#ifndef NO_SESSION_CACHE


/* some session IDs aren't random after all, let's make them random */
static INLINE word32 HashSession(const byte* sessionID, word32 len, int* error)
{
    byte digest[WC_MAX_DIGEST_SIZE];

#ifndef NO_MD5
    *error =  wc_Md5Hash(sessionID, len, digest);
#elif !defined(NO_SHA)
    *error =  wc_ShaHash(sessionID, len, digest);
#elif !defined(NO_SHA256)
    *error =  wc_Sha256Hash(sessionID, len, digest);
#else
    #error "We need a digest to hash the session IDs"
#endif

    return *error == 0 ? MakeWordFromHash(digest) : 0; /* 0 on failure */
}


void wolfSSL_flush_sessions(WOLFSSL_CTX* ctx, long tm)
{
    /* static table now, no flushing needed */
    (void)ctx;
    (void)tm;
}


/* set ssl session timeout in seconds */
int wolfSSL_set_timeout(WOLFSSL* ssl, unsigned int to)
{
    if (ssl == NULL)
        return BAD_FUNC_ARG;

    if (to == 0)
        to = WOLFSSL_SESSION_TIMEOUT;
    ssl->timeout = to;

    return WOLFSSL_SUCCESS;
}


/* set ctx session timeout in seconds */
int wolfSSL_CTX_set_timeout(WOLFSSL_CTX* ctx, unsigned int to)
{
    if (ctx == NULL)
        return BAD_FUNC_ARG;

    if (to == 0)
        to = WOLFSSL_SESSION_TIMEOUT;
    ctx->timeout = to;

    return WOLFSSL_SUCCESS;
}


#ifndef NO_CLIENT_CACHE

/* Get Session from Client cache based on id/len, return NULL on failure */
WOLFSSL_SESSION* GetSessionClient(WOLFSSL* ssl, const byte* id, int len)
{
    WOLFSSL_SESSION* ret = NULL;
    word32          row;
    int             idx;
    int             count;
    int             error = 0;

    WOLFSSL_ENTER("GetSessionClient");

    if (ssl->ctx->sessionCacheOff)
        return NULL;

    if (ssl->options.side == WOLFSSL_SERVER_END)
        return NULL;

    len = min(SERVER_ID_LEN, (word32)len);

#ifdef HAVE_EXT_CACHE
    if (ssl->ctx->get_sess_cb != NULL) {
        int copy = 0;
        ret = ssl->ctx->get_sess_cb(ssl, (byte*)id, len, &copy);
        if (ret != NULL)
            return ret;
    }

    if (ssl->ctx->internalCacheOff)
        return NULL;
#endif

    row = HashSession(id, len, &error) % SESSION_ROWS;
    if (error != 0) {
        WOLFSSL_MSG("Hash session failed");
        return NULL;
    }

    if (wc_LockMutex(&session_mutex) != 0) {
        WOLFSSL_MSG("Lock session mutex failed");
        return NULL;
    }

    /* start from most recently used */
    count = min((word32)ClientCache[row].totalCount, SESSIONS_PER_ROW);
    idx = ClientCache[row].nextIdx - 1;
    if (idx < 0)
        idx = SESSIONS_PER_ROW - 1; /* if back to front, the previous was end */

    for (; count > 0; --count, idx = idx ? idx - 1 : SESSIONS_PER_ROW - 1) {
        WOLFSSL_SESSION* current;
        ClientSession   clSess;

        if (idx >= SESSIONS_PER_ROW || idx < 0) { /* sanity check */
            WOLFSSL_MSG("Bad idx");
            break;
        }

        clSess = ClientCache[row].Clients[idx];

        current = &SessionCache[clSess.serverRow].Sessions[clSess.serverIdx];
        if (XMEMCMP(current->serverID, id, len) == 0) {
            WOLFSSL_MSG("Found a serverid match for client");
            if (LowResTimer() < (current->bornOn + current->timeout)) {
                WOLFSSL_MSG("Session valid");
                ret = current;
                break;
            } else {
                WOLFSSL_MSG("Session timed out");  /* could have more for id */
            }
        } else {
            WOLFSSL_MSG("ServerID not a match from client table");
        }
    }

    wc_UnLockMutex(&session_mutex);

    return ret;
}

#endif /* NO_CLIENT_CACHE */

/* Restore the master secret and session information for certificates.
 *
 * ssl                  The SSL/TLS object.
 * session              The cached session to restore.
 * masterSecret         The master secret from the cached session.
 * restoreSessionCerts  Restoring session certificates is required.
 */
static INLINE void RestoreSession(WOLFSSL* ssl, WOLFSSL_SESSION* session,
        byte* masterSecret, byte restoreSessionCerts)
{
    (void)ssl;
    (void)restoreSessionCerts;

    if (masterSecret)
        XMEMCPY(masterSecret, session->masterSecret, SECRET_LEN);
#ifdef SESSION_CERTS
    /* If set, we should copy the session certs into the ssl object
     * from the session we are returning so we can resume */
    if (restoreSessionCerts) {
        ssl->session.chain        = session->chain;
        ssl->session.version      = session->version;
        ssl->session.cipherSuite0 = session->cipherSuite0;
        ssl->session.cipherSuite  = session->cipherSuite;
    }
#endif /* SESSION_CERTS */
}

WOLFSSL_SESSION* GetSession(WOLFSSL* ssl, byte* masterSecret,
        byte restoreSessionCerts)
{
    WOLFSSL_SESSION* ret = 0;
    const byte*  id = NULL;
    word32       row;
    int          idx;
    int          count;
    int          error = 0;

    (void)       restoreSessionCerts;

    if (ssl->options.sessionCacheOff)
        return NULL;

    if (ssl->options.haveSessionId == 0)
        return NULL;

#ifdef HAVE_SESSION_TICKET
    if (ssl->options.side == WOLFSSL_SERVER_END && ssl->options.useTicket == 1)
        return NULL;
#endif

    if (ssl->arrays)
        id = ssl->arrays->sessionID;
    else
        id = ssl->session.sessionID;

#ifdef HAVE_EXT_CACHE
    if (ssl->ctx->get_sess_cb != NULL) {
        int copy = 0;
        /* Attempt to retrieve the session from the external cache. */
        ret = ssl->ctx->get_sess_cb(ssl, (byte*)id, ID_LEN, &copy);
        if (ret != NULL) {
            RestoreSession(ssl, ret, masterSecret, restoreSessionCerts);
            return ret;
        }
    }

    if (ssl->ctx->internalCacheOff)
        return NULL;
#endif

    row = HashSession(id, ID_LEN, &error) % SESSION_ROWS;
    if (error != 0) {
        WOLFSSL_MSG("Hash session failed");
        return NULL;
    }

    if (wc_LockMutex(&session_mutex) != 0)
        return 0;

    /* start from most recently used */
    count = min((word32)SessionCache[row].totalCount, SESSIONS_PER_ROW);
    idx = SessionCache[row].nextIdx - 1;
    if (idx < 0)
        idx = SESSIONS_PER_ROW - 1; /* if back to front, the previous was end */

    for (; count > 0; --count, idx = idx ? idx - 1 : SESSIONS_PER_ROW - 1) {
        WOLFSSL_SESSION* current;

        if (idx >= SESSIONS_PER_ROW || idx < 0) { /* sanity check */
            WOLFSSL_MSG("Bad idx");
            break;
        }

        current = &SessionCache[row].Sessions[idx];
        if (XMEMCMP(current->sessionID, id, ID_LEN) == 0) {
            WOLFSSL_MSG("Found a session match");
            if (LowResTimer() < (current->bornOn + current->timeout)) {
                WOLFSSL_MSG("Session valid");
                ret = current;
                RestoreSession(ssl, ret, masterSecret, restoreSessionCerts);
            } else {
                WOLFSSL_MSG("Session timed out");
            }
            break;  /* no more sessionIDs whether valid or not that match */
        } else {
            WOLFSSL_MSG("SessionID not a match at this idx");
        }
    }

    wc_UnLockMutex(&session_mutex);

    return ret;
}


static int GetDeepCopySession(WOLFSSL* ssl, WOLFSSL_SESSION* copyFrom)
{
    WOLFSSL_SESSION* copyInto = &ssl->session;
    void* tmpBuff             = NULL;
    int ticketLen             = 0;
    int doDynamicCopy         = 0;
    int ret                   = WOLFSSL_SUCCESS;

    (void)ticketLen;
    (void)doDynamicCopy;
    (void)tmpBuff;

    if (!ssl || !copyFrom)
        return BAD_FUNC_ARG;

#ifdef HAVE_SESSION_TICKET
    /* Free old dynamic ticket if we had one to avoid leak */
    if (copyInto->isDynamic) {
        XFREE(copyInto->ticket, ssl->heap, DYNAMIC_TYPE_SESSION_TICK);
        copyInto->ticket = copyInto->staticTicket;
        copyInto->isDynamic = 0;
    }
#endif

    if (wc_LockMutex(&session_mutex) != 0)
        return BAD_MUTEX_E;

#ifdef HAVE_SESSION_TICKET
    /* Size of ticket to alloc if needed; Use later for alloc outside lock */
    doDynamicCopy = copyFrom->isDynamic;
    ticketLen = copyFrom->ticketLen;
#endif

    *copyInto = *copyFrom;

    /* Default ticket to non dynamic. This will avoid crash if we fail below */
#ifdef HAVE_SESSION_TICKET
    copyInto->ticket = copyInto->staticTicket;
    copyInto->isDynamic = 0;
#endif

    if (wc_UnLockMutex(&session_mutex) != 0) {
        return BAD_MUTEX_E;
    }

#ifdef HAVE_SESSION_TICKET
#ifdef WOLFSSL_TLS13
    if (wc_LockMutex(&session_mutex) != 0) {
        XFREE(tmpBuff, ssl->heap, DYNAMIC_TYPE_SESSION_TICK);
        return BAD_MUTEX_E;
    }

    copyInto->cipherSuite0   = copyFrom->cipherSuite0;
    copyInto->cipherSuite    = copyFrom->cipherSuite;
    copyInto->namedGroup     = copyFrom->namedGroup;
    copyInto->ticketSeen     = copyFrom->ticketSeen;
    copyInto->ticketAdd      = copyFrom->ticketAdd;
#ifndef WOLFSSL_TLS13_DRAFT_18
    XMEMCPY(&copyInto->ticketNonce, &copyFrom->ticketNonce,
                                                           sizeof(TicketNonce));
#endif
#ifdef WOLFSSL_EARLY_DATA
    copyInto->maxEarlyDataSz = copyFrom->maxEarlyDataSz;
#endif
    XMEMCPY(copyInto->masterSecret, copyFrom->masterSecret, SECRET_LEN);

    if (wc_UnLockMutex(&session_mutex) != 0) {
        if (ret == WOLFSSL_SUCCESS)
            ret = BAD_MUTEX_E;
    }
#endif
    /* If doing dynamic copy, need to alloc outside lock, then inside a lock
     * confirm the size still matches and memcpy */
    if (doDynamicCopy) {
        tmpBuff = (byte*)XMALLOC(ticketLen, ssl->heap,
                                                     DYNAMIC_TYPE_SESSION_TICK);
        if (!tmpBuff)
            return MEMORY_ERROR;

        if (wc_LockMutex(&session_mutex) != 0) {
            XFREE(tmpBuff, ssl->heap, DYNAMIC_TYPE_SESSION_TICK);
            return BAD_MUTEX_E;
        }

        if (ticketLen != copyFrom->ticketLen) {
            /* Another thread modified the ssl-> session ticket during alloc.
             * Treat as error, since ticket different than when copy requested */
            ret = VAR_STATE_CHANGE_E;
        }

        if (ret == WOLFSSL_SUCCESS) {
            copyInto->ticket = (byte*)tmpBuff;
            copyInto->isDynamic = 1;
            XMEMCPY(copyInto->ticket, copyFrom->ticket, ticketLen);
        }
    } else {
        /* Need to ensure ticket pointer gets updated to own buffer
         * and is not pointing to buff of session copied from */
        copyInto->ticket = copyInto->staticTicket;
    }

    if (doDynamicCopy) {
        if (wc_UnLockMutex(&session_mutex) != 0) {
            if (ret == WOLFSSL_SUCCESS)
                ret = BAD_MUTEX_E;
        }
    }

    if (ret != WOLFSSL_SUCCESS) {
        /* cleanup */
        if (tmpBuff)
            XFREE(tmpBuff, ssl->heap, DYNAMIC_TYPE_SESSION_TICK);
        copyInto->ticket = copyInto->staticTicket;
        copyInto->isDynamic = 0;
    }
#endif /* HAVE_SESSION_TICKET */
    return ret;
}


int SetSession(WOLFSSL* ssl, WOLFSSL_SESSION* session)
{
    if (ssl->options.sessionCacheOff)
        return WOLFSSL_FAILURE;

#ifdef OPENSSL_EXTRA
    /* check for application context id */
    if (ssl->sessionCtxSz > 0) {
        if (XMEMCMP(ssl->sessionCtx, session->sessionCtx, ssl->sessionCtxSz)) {
            /* context id did not match! */
            WOLFSSL_MSG("Session context did not match");
            return SSL_FAILURE;
        }
    }
#endif /* OPENSSL_EXTRA */

    if (LowResTimer() < (session->bornOn + session->timeout)) {
        int ret = GetDeepCopySession(ssl, session);
        if (ret == WOLFSSL_SUCCESS) {
            ssl->options.resuming = 1;

#if defined(SESSION_CERTS) || (defined(WOLFSSL_TLS13) && \
                               defined(HAVE_SESSION_TICKET))
            ssl->version              = session->version;
            ssl->options.cipherSuite0 = session->cipherSuite0;
            ssl->options.cipherSuite  = session->cipherSuite;
#endif
        }

        return ret;
    }
    return WOLFSSL_FAILURE;  /* session timed out */
}


#ifdef WOLFSSL_SESSION_STATS
static int get_locked_session_stats(word32* active, word32* total,
                                    word32* peak);
#endif

int AddSession(WOLFSSL* ssl)
{
    word32 row = 0;
    word32 idx = 0;
    int    error = 0;
#ifdef HAVE_SESSION_TICKET
    byte*  tmpBuff = NULL;
    int    ticLen  = 0;
#endif
    WOLFSSL_SESSION* session;

    if (ssl->options.sessionCacheOff)
        return 0;

    if (ssl->options.haveSessionId == 0)
        return 0;

#ifdef HAVE_SESSION_TICKET
    if (ssl->options.side == WOLFSSL_SERVER_END && ssl->options.useTicket == 1)
        return 0;
#endif

#ifdef HAVE_SESSION_TICKET
    ticLen = ssl->session.ticketLen;
    /* Alloc Memory here so if Malloc fails can exit outside of lock */
    if(ticLen > SESSION_TICKET_LEN) {
        tmpBuff = (byte*)XMALLOC(ticLen, ssl->heap,
                DYNAMIC_TYPE_SESSION_TICK);
        if(!tmpBuff)
            return MEMORY_E;
    }
#endif

#ifdef HAVE_EXT_CACHE
    if (ssl->options.internalCacheOff) {
        /* Create a new session object to be stored. */
        session = (WOLFSSL_SESSION*)XMALLOC(sizeof(WOLFSSL_SESSION), NULL,
                                            DYNAMIC_TYPE_OPENSSL);
        if (session == NULL) {
#ifdef HAVE_SESSION_TICKET
            XFREE(tmpBuff, ssl->heap, DYNAMIC_TYPE_SESSION_TICK);
#endif
            return MEMORY_E;
        }
        XMEMSET(session, 0, sizeof(WOLFSSL_SESSION));
        session->isAlloced = 1;
    }
    else
#endif
    {
        /* Use the session object in the cache for external cache if required.
         */
        row = HashSession(ssl->arrays->sessionID, ID_LEN, &error) %
                SESSION_ROWS;
        if (error != 0) {
            WOLFSSL_MSG("Hash session failed");
#ifdef HAVE_SESSION_TICKET
            XFREE(tmpBuff, ssl->heap, DYNAMIC_TYPE_SESSION_TICK);
#endif
            return error;
        }

        if (wc_LockMutex(&session_mutex) != 0) {
#ifdef HAVE_SESSION_TICKET
            XFREE(tmpBuff, ssl->heap, DYNAMIC_TYPE_SESSION_TICK);
#endif
            return BAD_MUTEX_E;
        }

        idx = SessionCache[row].nextIdx++;
#ifdef SESSION_INDEX
        ssl->sessionIndex = (row << SESSIDX_ROW_SHIFT) | idx;
#endif
        session = &SessionCache[row].Sessions[idx];
    }

    if (!ssl->options.tls1_3)
        XMEMCPY(session->masterSecret, ssl->arrays->masterSecret, SECRET_LEN);
    else
        XMEMCPY(session->masterSecret, ssl->session.masterSecret, SECRET_LEN);
    session->haveEMS = ssl->options.haveEMS;
    XMEMCPY(session->sessionID, ssl->arrays->sessionID, ID_LEN);
    session->sessionIDSz = ssl->arrays->sessionIDSz;

#ifdef OPENSSL_EXTRA
    /* If using compatibilty layer then check for and copy over session context
     * id. */
    if (ssl->sessionCtxSz > 0 && ssl->sessionCtxSz < ID_LEN) {
        XMEMCPY(session->sessionCtx, ssl->sessionCtx, ssl->sessionCtxSz);
    }
#endif

    session->timeout = ssl->timeout;
    session->bornOn  = LowResTimer();

#ifdef HAVE_SESSION_TICKET
    /* Check if another thread modified ticket since alloc */
    if (ticLen != ssl->session.ticketLen) {
        error = VAR_STATE_CHANGE_E;
    }

    if (error == 0) {
        /* Cleanup cache row's old Dynamic buff if exists */
        if(session->isDynamic) {
            XFREE(session->ticket, ssl->heap, DYNAMIC_TYPE_SESSION_TICK);
            session->ticket = NULL;
        }

        /* If too large to store in static buffer, use dyn buffer */
        if (ticLen > SESSION_TICKET_LEN) {
            session->ticket = tmpBuff;
            session->isDynamic = 1;
        } else {
            session->ticket = session->staticTicket;
            session->isDynamic = 0;
        }
    }

    if (error == 0) {
        session->ticketLen = (word16)ticLen;
        XMEMCPY(session->ticket, ssl->session.ticket, ticLen);
    } else { /* cleanup, reset state */
        session->ticket    = session->staticTicket;
        session->isDynamic = 0;
        session->ticketLen = 0;
        if (tmpBuff) {
            XFREE(tmpBuff, ssl->heap, DYNAMIC_TYPE_SESSION_TICK);
            tmpBuff = NULL;
        }
    }
#endif

#ifdef SESSION_CERTS
    if (error == 0) {
        session->chain.count = ssl->session.chain.count;
        XMEMCPY(session->chain.certs, ssl->session.chain.certs,
                sizeof(x509_buffer) * MAX_CHAIN_DEPTH);
    }
#endif /* SESSION_CERTS */
#if defined(SESSION_CERTS) || (defined(WOLFSSL_TLS13) && \
                               defined(HAVE_SESSION_TICKET))
    if (error == 0) {
        session->version      = ssl->version;
        session->cipherSuite0 = ssl->options.cipherSuite0;
        session->cipherSuite  = ssl->options.cipherSuite;
    }
#endif /* SESSION_CERTS || (WOLFSSL_TLS13 & HAVE_SESSION_TICKET) */
#if defined(WOLFSSL_TLS13)
    if (error == 0) {
        session->namedGroup     = ssl->session.namedGroup;
    }
#endif
#if defined(WOLFSSL_TLS13) && defined(HAVE_SESSION_TICKET)
    if (error == 0) {
        session->ticketSeen     = ssl->session.ticketSeen;
        session->ticketAdd      = ssl->session.ticketAdd;
#ifndef WOLFSSL_TLS13_DRAFT_18
        XMEMCPY(&session->ticketNonce, &ssl->session.ticketNonce,
                                                           sizeof(TicketNonce));
#endif
    #ifdef WOLFSSL_EARLY_DATA
        session->maxEarlyDataSz = ssl->session.maxEarlyDataSz;
    #endif
    }
#endif /* WOLFSSL_TLS13 && HAVE_SESSION_TICKET */
#ifdef HAVE_EXT_CACHE
    if (!ssl->options.internalCacheOff)
#endif
    {
        if (error == 0) {
            SessionCache[row].totalCount++;
            if (SessionCache[row].nextIdx == SESSIONS_PER_ROW)
                SessionCache[row].nextIdx = 0;
        }
    }
#ifndef NO_CLIENT_CACHE
    if (error == 0) {
        if (ssl->options.side == WOLFSSL_CLIENT_END && ssl->session.idLen) {
            word32 clientRow, clientIdx;

            WOLFSSL_MSG("Adding client cache entry");

            session->idLen = ssl->session.idLen;
            XMEMCPY(session->serverID, ssl->session.serverID,
                    ssl->session.idLen);

#ifdef HAVE_EXT_CACHE
            if (!ssl->options.internalCacheOff)
#endif
            {
                clientRow = HashSession(ssl->session.serverID,
                        ssl->session.idLen, &error) % SESSION_ROWS;
                if (error != 0) {
                    WOLFSSL_MSG("Hash session failed");
                } else {
                    clientIdx = ClientCache[clientRow].nextIdx++;

                    ClientCache[clientRow].Clients[clientIdx].serverRow =
                                                                   (word16)row;
                    ClientCache[clientRow].Clients[clientIdx].serverIdx =
                                                                   (word16)idx;

                    ClientCache[clientRow].totalCount++;
                    if (ClientCache[clientRow].nextIdx == SESSIONS_PER_ROW)
                        ClientCache[clientRow].nextIdx = 0;
                }
            }
        }
        else
            session->idLen = 0;
    }
#endif /* NO_CLIENT_CACHE */

#if defined(WOLFSSL_SESSION_STATS) && defined(WOLFSSL_PEAK_SESSIONS)
#ifdef HAVE_EXT_CACHE
    if (!ssl->options.internalCacheOff)
#endif
    {
        if (error == 0) {
            word32 active = 0;

            error = get_locked_session_stats(&active, NULL, NULL);
            if (error == WOLFSSL_SUCCESS) {
                error = 0;  /* back to this function ok */

                if (active > PeakSessions)
                    PeakSessions = active;
            }
        }
    }
#endif /* defined(WOLFSSL_SESSION_STATS) && defined(WOLFSSL_PEAK_SESSIONS) */

#ifdef HAVE_EXT_CACHE
    if (!ssl->options.internalCacheOff)
#endif
    {
        if (wc_UnLockMutex(&session_mutex) != 0)
            return BAD_MUTEX_E;
    }

#ifdef HAVE_EXT_CACHE
    if (error == 0 && ssl->ctx->new_sess_cb != NULL)
        ssl->ctx->new_sess_cb(ssl, session);
    if (ssl->options.internalCacheOff)
        wolfSSL_SESSION_free(session);
#endif

    return error;
}


#ifdef SESSION_INDEX

int wolfSSL_GetSessionIndex(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("wolfSSL_GetSessionIndex");
    WOLFSSL_LEAVE("wolfSSL_GetSessionIndex", ssl->sessionIndex);
    return ssl->sessionIndex;
}


int wolfSSL_GetSessionAtIndex(int idx, WOLFSSL_SESSION* session)
{
    int row, col, result = WOLFSSL_FAILURE;

    WOLFSSL_ENTER("wolfSSL_GetSessionAtIndex");

    row = idx >> SESSIDX_ROW_SHIFT;
    col = idx & SESSIDX_IDX_MASK;

    if (wc_LockMutex(&session_mutex) != 0) {
        return BAD_MUTEX_E;
    }

    if (row < SESSION_ROWS &&
        col < (int)min(SessionCache[row].totalCount, SESSIONS_PER_ROW)) {
        XMEMCPY(session,
                 &SessionCache[row].Sessions[col], sizeof(WOLFSSL_SESSION));
        result = WOLFSSL_SUCCESS;
    }

    if (wc_UnLockMutex(&session_mutex) != 0)
        result = BAD_MUTEX_E;

    WOLFSSL_LEAVE("wolfSSL_GetSessionAtIndex", result);
    return result;
}

#endif /* SESSION_INDEX */

#if defined(SESSION_INDEX) && defined(SESSION_CERTS)

WOLFSSL_X509_CHAIN* wolfSSL_SESSION_get_peer_chain(WOLFSSL_SESSION* session)
{
    WOLFSSL_X509_CHAIN* chain = NULL;

    WOLFSSL_ENTER("wolfSSL_SESSION_get_peer_chain");
    if (session)
        chain = &session->chain;

    WOLFSSL_LEAVE("wolfSSL_SESSION_get_peer_chain", chain ? 1 : 0);
    return chain;
}

#endif /* SESSION_INDEX && SESSION_CERTS */


#ifdef WOLFSSL_SESSION_STATS

/* requires session_mutex lock held, WOLFSSL_SUCCESS on ok */
static int get_locked_session_stats(word32* active, word32* total, word32* peak)
{
    int result = WOLFSSL_SUCCESS;
    int i;
    int count;
    int idx;
    word32 now   = 0;
    word32 seen  = 0;
    word32 ticks = LowResTimer();

    (void)peak;

    WOLFSSL_ENTER("get_locked_session_stats");

    for (i = 0; i < SESSION_ROWS; i++) {
        seen += SessionCache[i].totalCount;

        if (active == NULL)
            continue;  /* no need to calculate what we can't set */

        count = min((word32)SessionCache[i].totalCount, SESSIONS_PER_ROW);
        idx   = SessionCache[i].nextIdx - 1;
        if (idx < 0)
            idx = SESSIONS_PER_ROW - 1; /* if back to front previous was end */

        for (; count > 0; --count, idx = idx ? idx - 1 : SESSIONS_PER_ROW - 1) {
            if (idx >= SESSIONS_PER_ROW || idx < 0) {  /* sanity check */
                WOLFSSL_MSG("Bad idx");
                break;
            }

            /* if not expried then good */
            if (ticks < (SessionCache[i].Sessions[idx].bornOn +
                         SessionCache[i].Sessions[idx].timeout) ) {
                now++;
            }
        }
    }

    if (active)
        *active = now;

    if (total)
        *total = seen;

#ifdef WOLFSSL_PEAK_SESSIONS
    if (peak)
        *peak = PeakSessions;
#endif

    WOLFSSL_LEAVE("get_locked_session_stats", result);

    return result;
}


/* return WOLFSSL_SUCCESS on ok */
int wolfSSL_get_session_stats(word32* active, word32* total, word32* peak,
                              word32* maxSessions)
{
    int result = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_get_session_stats");

    if (maxSessions) {
        *maxSessions = SESSIONS_PER_ROW * SESSION_ROWS;

        if (active == NULL && total == NULL && peak == NULL)
            return result;  /* we're done */
    }

    /* user must provide at least one query value */
    if (active == NULL && total == NULL && peak == NULL)
        return BAD_FUNC_ARG;

    if (wc_LockMutex(&session_mutex) != 0) {
        return BAD_MUTEX_E;
    }

    result = get_locked_session_stats(active, total, peak);

    if (wc_UnLockMutex(&session_mutex) != 0)
        result = BAD_MUTEX_E;

    WOLFSSL_LEAVE("wolfSSL_get_session_stats", result);

    return result;
}

#endif /* WOLFSSL_SESSION_STATS */


    #ifdef PRINT_SESSION_STATS

    /* WOLFSSL_SUCCESS on ok */
    int wolfSSL_PrintSessionStats(void)
    {
        word32 totalSessionsSeen = 0;
        word32 totalSessionsNow = 0;
        word32 peak = 0;
        word32 maxSessions = 0;
        int    i;
        int    ret;
        double E;               /* expected freq */
        double chiSquare = 0;

        ret = wolfSSL_get_session_stats(&totalSessionsNow, &totalSessionsSeen,
                                        &peak, &maxSessions);
        if (ret != WOLFSSL_SUCCESS)
            return ret;
        printf("Total Sessions Seen = %d\n", totalSessionsSeen);
        printf("Total Sessions Now  = %d\n", totalSessionsNow);
#ifdef WOLFSSL_PEAK_SESSIONS
        printf("Peak  Sessions      = %d\n", peak);
#endif
        printf("Max   Sessions      = %d\n", maxSessions);

        E = (double)totalSessionsSeen / SESSION_ROWS;

        for (i = 0; i < SESSION_ROWS; i++) {
            double diff = SessionCache[i].totalCount - E;
            diff *= diff;                /* square    */
            diff /= E;                   /* normalize */

            chiSquare += diff;
        }
        printf("  chi-square = %5.1f, d.f. = %d\n", chiSquare,
                                                     SESSION_ROWS - 1);
        #if (SESSION_ROWS == 11)
            printf(" .05 p value =  18.3, chi-square should be less\n");
        #elif (SESSION_ROWS == 211)
            printf(".05 p value  = 244.8, chi-square should be less\n");
        #elif (SESSION_ROWS == 5981)
            printf(".05 p value  = 6161.0, chi-square should be less\n");
        #elif (SESSION_ROWS == 3)
            printf(".05 p value  =   6.0, chi-square should be less\n");
        #elif (SESSION_ROWS == 2861)
            printf(".05 p value  = 2985.5, chi-square should be less\n");
        #endif
        printf("\n");

        return ret;
    }

    #endif /* SESSION_STATS */

#else  /* NO_SESSION_CACHE */

/* No session cache version */
WOLFSSL_SESSION* GetSession(WOLFSSL* ssl, byte* masterSecret,
        byte restoreSessionCerts)
{
    (void)ssl;
    (void)masterSecret;
    (void)restoreSessionCerts;

    return NULL;
}

#endif /* NO_SESSION_CACHE */


/* call before SSL_connect, if verifying will add name check to
   date check and signature check */
int wolfSSL_check_domain_name(WOLFSSL* ssl, const char* dn)
{
    WOLFSSL_ENTER("wolfSSL_check_domain_name");

    if (ssl == NULL || dn == NULL) {
        WOLFSSL_MSG("Bad function argument: NULL");
        return WOLFSSL_FAILURE;
    }

    if (ssl->buffers.domainName.buffer)
        XFREE(ssl->buffers.domainName.buffer, ssl->heap, DYNAMIC_TYPE_DOMAIN);

    ssl->buffers.domainName.length = (word32)XSTRLEN(dn);
    ssl->buffers.domainName.buffer = (byte*)XMALLOC(
            ssl->buffers.domainName.length + 1, ssl->heap, DYNAMIC_TYPE_DOMAIN);

    if (ssl->buffers.domainName.buffer) {
        char* domainName = (char*)ssl->buffers.domainName.buffer;
        XSTRNCPY(domainName, dn, ssl->buffers.domainName.length);
        domainName[ssl->buffers.domainName.length] = '\0';
        return WOLFSSL_SUCCESS;
    }
    else {
        ssl->error = MEMORY_ERROR;
        return WOLFSSL_FAILURE;
    }
}


/* turn on wolfSSL zlib compression
   returns WOLFSSL_SUCCESS for success, else error (not built in)
*/
int wolfSSL_set_compression(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("wolfSSL_set_compression");
    (void)ssl;
#ifdef HAVE_LIBZ
    ssl->options.usingCompression = 1;
    return WOLFSSL_SUCCESS;
#else
    return NOT_COMPILED_IN;
#endif
}


#ifndef USE_WINDOWS_API
    #ifndef NO_WRITEV

        /* simulate writev semantics, doesn't actually do block at a time though
           because of SSL_write behavior and because front adds may be small */
        int wolfSSL_writev(WOLFSSL* ssl, const struct iovec* iov, int iovcnt)
        {
        #ifdef WOLFSSL_SMALL_STACK
            byte   staticBuffer[1]; /* force heap usage */
        #else
            byte   staticBuffer[FILE_BUFFER_SIZE];
        #endif
            byte* myBuffer  = staticBuffer;
            int   dynamic   = 0;
            int   sending   = 0;
            int   idx       = 0;
            int   i;
            int   ret;

            WOLFSSL_ENTER("wolfSSL_writev");

            for (i = 0; i < iovcnt; i++)
                sending += (int)iov[i].iov_len;

            if (sending > (int)sizeof(staticBuffer)) {
                myBuffer = (byte*)XMALLOC(sending, ssl->heap,
                                                           DYNAMIC_TYPE_WRITEV);
                if (!myBuffer)
                    return MEMORY_ERROR;

                dynamic = 1;
            }

            for (i = 0; i < iovcnt; i++) {
                XMEMCPY(&myBuffer[idx], iov[i].iov_base, iov[i].iov_len);
                idx += (int)iov[i].iov_len;
            }

            ret = wolfSSL_write(ssl, myBuffer, sending);

            if (dynamic)
                XFREE(myBuffer, ssl->heap, DYNAMIC_TYPE_WRITEV);

            return ret;
        }
    #endif
#endif


#ifdef WOLFSSL_CALLBACKS

    typedef struct itimerval Itimerval;

    /* don't keep calling simple functions while setting up timer and signals
       if no inlining these are the next best */

    #define AddTimes(a, b, c)                       \
        do {                                        \
            c.tv_sec  = a.tv_sec  + b.tv_sec;       \
            c.tv_usec = a.tv_usec + b.tv_usec;      \
            if (c.tv_usec >=  1000000) {            \
                c.tv_sec++;                         \
                c.tv_usec -= 1000000;               \
            }                                       \
        } while (0)


    #define SubtractTimes(a, b, c)                  \
        do {                                        \
            c.tv_sec  = a.tv_sec  - b.tv_sec;       \
            c.tv_usec = a.tv_usec - b.tv_usec;      \
            if (c.tv_usec < 0) {                    \
                c.tv_sec--;                         \
                c.tv_usec += 1000000;               \
            }                                       \
        } while (0)

    #define CmpTimes(a, b, cmp)                     \
        ((a.tv_sec  ==  b.tv_sec) ?                 \
            (a.tv_usec cmp b.tv_usec) :             \
            (a.tv_sec  cmp b.tv_sec))               \


    /* do nothing handler */
    static void myHandler(int signo)
    {
        (void)signo;
        return;
    }


    static int wolfSSL_ex_wrapper(WOLFSSL* ssl, HandShakeCallBack hsCb,
                                 TimeoutCallBack toCb, Timeval timeout)
    {
        int       ret        = WOLFSSL_FATAL_ERROR;
        int       oldTimerOn = 0;   /* was timer already on */
        Timeval   startTime;
        Timeval   endTime;
        Timeval   totalTime;
        Itimerval myTimeout;
        Itimerval oldTimeout; /* if old timer adjust from total time to reset */
        struct sigaction act, oact;

        #define ERR_OUT(x) { ssl->hsInfoOn = 0; ssl->toInfoOn = 0; return x; }

        if (hsCb) {
            ssl->hsInfoOn = 1;
            InitHandShakeInfo(&ssl->handShakeInfo, ssl);
        }
        if (toCb) {
            ssl->toInfoOn = 1;
            InitTimeoutInfo(&ssl->timeoutInfo);

            if (gettimeofday(&startTime, 0) < 0)
                ERR_OUT(GETTIME_ERROR);

            /* use setitimer to simulate getitimer, init 0 myTimeout */
            myTimeout.it_interval.tv_sec  = 0;
            myTimeout.it_interval.tv_usec = 0;
            myTimeout.it_value.tv_sec     = 0;
            myTimeout.it_value.tv_usec    = 0;
            if (setitimer(ITIMER_REAL, &myTimeout, &oldTimeout) < 0)
                ERR_OUT(SETITIMER_ERROR);

            if (oldTimeout.it_value.tv_sec || oldTimeout.it_value.tv_usec) {
                oldTimerOn = 1;

                /* is old timer going to expire before ours */
                if (CmpTimes(oldTimeout.it_value, timeout, <)) {
                    timeout.tv_sec  = oldTimeout.it_value.tv_sec;
                    timeout.tv_usec = oldTimeout.it_value.tv_usec;
                }
            }
            myTimeout.it_value.tv_sec  = timeout.tv_sec;
            myTimeout.it_value.tv_usec = timeout.tv_usec;

            /* set up signal handler, don't restart socket send/recv */
            act.sa_handler = myHandler;
            sigemptyset(&act.sa_mask);
            act.sa_flags = 0;
#ifdef SA_INTERRUPT
            act.sa_flags |= SA_INTERRUPT;
#endif
            if (sigaction(SIGALRM, &act, &oact) < 0)
                ERR_OUT(SIGACT_ERROR);

            if (setitimer(ITIMER_REAL, &myTimeout, 0) < 0)
                ERR_OUT(SETITIMER_ERROR);
        }

        /* do main work */
#ifndef NO_WOLFSSL_CLIENT
        if (ssl->options.side == WOLFSSL_CLIENT_END)
            ret = wolfSSL_connect(ssl);
#endif
#ifndef NO_WOLFSSL_SERVER
        if (ssl->options.side == WOLFSSL_SERVER_END)
            ret = wolfSSL_accept(ssl);
#endif

        /* do callbacks */
        if (toCb) {
            if (oldTimerOn) {
                gettimeofday(&endTime, 0);
                SubtractTimes(endTime, startTime, totalTime);
                /* adjust old timer for elapsed time */
                if (CmpTimes(totalTime, oldTimeout.it_value, <))
                    SubtractTimes(oldTimeout.it_value, totalTime,
                                  oldTimeout.it_value);
                else {
                    /* reset value to interval, may be off */
                    oldTimeout.it_value.tv_sec = oldTimeout.it_interval.tv_sec;
                    oldTimeout.it_value.tv_usec =oldTimeout.it_interval.tv_usec;
                }
                /* keep iter the same whether there or not */
            }
            /* restore old handler */
            if (sigaction(SIGALRM, &oact, 0) < 0)
                ret = SIGACT_ERROR;    /* more pressing error, stomp */
            else
                /* use old settings which may turn off (expired or not there) */
                if (setitimer(ITIMER_REAL, &oldTimeout, 0) < 0)
                    ret = SETITIMER_ERROR;

            /* if we had a timeout call callback */
            if (ssl->timeoutInfo.timeoutName[0]) {
                ssl->timeoutInfo.timeoutValue.tv_sec  = timeout.tv_sec;
                ssl->timeoutInfo.timeoutValue.tv_usec = timeout.tv_usec;
                (toCb)(&ssl->timeoutInfo);
            }
            /* clean up */
            FreeTimeoutInfo(&ssl->timeoutInfo, ssl->heap);
            ssl->toInfoOn = 0;
        }
        if (hsCb) {
            FinishHandShakeInfo(&ssl->handShakeInfo);
            (hsCb)(&ssl->handShakeInfo);
            ssl->hsInfoOn = 0;
        }
        return ret;
    }


#ifndef NO_WOLFSSL_CLIENT

    int wolfSSL_connect_ex(WOLFSSL* ssl, HandShakeCallBack hsCb,
                          TimeoutCallBack toCb, Timeval timeout)
    {
        WOLFSSL_ENTER("wolfSSL_connect_ex");
        return wolfSSL_ex_wrapper(ssl, hsCb, toCb, timeout);
    }

#endif


#ifndef NO_WOLFSSL_SERVER

    int wolfSSL_accept_ex(WOLFSSL* ssl, HandShakeCallBack hsCb,
                         TimeoutCallBack toCb,Timeval timeout)
    {
        WOLFSSL_ENTER("wolfSSL_accept_ex");
        return wolfSSL_ex_wrapper(ssl, hsCb, toCb, timeout);
    }

#endif

#endif /* WOLFSSL_CALLBACKS */


#ifndef NO_PSK

    void wolfSSL_CTX_set_psk_client_callback(WOLFSSL_CTX* ctx,
                                         wc_psk_client_callback cb)
    {
        WOLFSSL_ENTER("SSL_CTX_set_psk_client_callback");
        ctx->havePSK = 1;
        ctx->client_psk_cb = cb;
    }


    void wolfSSL_set_psk_client_callback(WOLFSSL* ssl,wc_psk_client_callback cb)
    {
        byte haveRSA = 1;
        int  keySz   = 0;

        WOLFSSL_ENTER("SSL_set_psk_client_callback");
        ssl->options.havePSK = 1;
        ssl->options.client_psk_cb = cb;

        #ifdef NO_RSA
            haveRSA = 0;
        #endif
        #ifndef NO_CERTS
            keySz = ssl->buffers.keySz;
        #endif
        InitSuites(ssl->suites, ssl->version, keySz, haveRSA, TRUE,
                   ssl->options.haveDH, ssl->options.haveNTRU,
                   ssl->options.haveECDSAsig, ssl->options.haveECC,
                   ssl->options.haveStaticECC, ssl->options.side);
    }


    void wolfSSL_CTX_set_psk_server_callback(WOLFSSL_CTX* ctx,
                                         wc_psk_server_callback cb)
    {
        WOLFSSL_ENTER("SSL_CTX_set_psk_server_callback");
        ctx->havePSK = 1;
        ctx->server_psk_cb = cb;
    }


    void wolfSSL_set_psk_server_callback(WOLFSSL* ssl,wc_psk_server_callback cb)
    {
        byte haveRSA = 1;
        int  keySz   = 0;

        WOLFSSL_ENTER("SSL_set_psk_server_callback");
        ssl->options.havePSK = 1;
        ssl->options.server_psk_cb = cb;

        #ifdef NO_RSA
            haveRSA = 0;
        #endif
        #ifndef NO_CERTS
            keySz = ssl->buffers.keySz;
        #endif
        InitSuites(ssl->suites, ssl->version, keySz, haveRSA, TRUE,
                   ssl->options.haveDH, ssl->options.haveNTRU,
                   ssl->options.haveECDSAsig, ssl->options.haveECC,
                   ssl->options.haveStaticECC, ssl->options.side);
    }


    const char* wolfSSL_get_psk_identity_hint(const WOLFSSL* ssl)
    {
        WOLFSSL_ENTER("SSL_get_psk_identity_hint");

        if (ssl == NULL || ssl->arrays == NULL)
            return NULL;

        return ssl->arrays->server_hint;
    }


    const char* wolfSSL_get_psk_identity(const WOLFSSL* ssl)
    {
        WOLFSSL_ENTER("SSL_get_psk_identity");

        if (ssl == NULL || ssl->arrays == NULL)
            return NULL;

        return ssl->arrays->client_identity;
    }


    int wolfSSL_CTX_use_psk_identity_hint(WOLFSSL_CTX* ctx, const char* hint)
    {
        WOLFSSL_ENTER("SSL_CTX_use_psk_identity_hint");
        if (hint == 0)
            ctx->server_hint[0] = '\0';
        else {
            XSTRNCPY(ctx->server_hint, hint, sizeof(ctx->server_hint));
            ctx->server_hint[MAX_PSK_ID_LEN] = '\0'; /* null term */
        }
        return WOLFSSL_SUCCESS;
    }


    int wolfSSL_use_psk_identity_hint(WOLFSSL* ssl, const char* hint)
    {
        WOLFSSL_ENTER("SSL_use_psk_identity_hint");

        if (ssl == NULL || ssl->arrays == NULL)
            return WOLFSSL_FAILURE;

        if (hint == 0)
            ssl->arrays->server_hint[0] = 0;
        else {
            XSTRNCPY(ssl->arrays->server_hint, hint,
                                            sizeof(ssl->arrays->server_hint));
            ssl->arrays->server_hint[MAX_PSK_ID_LEN] = '\0'; /* null term */
        }
        return WOLFSSL_SUCCESS;
    }

#endif /* NO_PSK */


#ifdef HAVE_ANON

    int wolfSSL_CTX_allow_anon_cipher(WOLFSSL_CTX* ctx)
    {
        WOLFSSL_ENTER("wolfSSL_CTX_allow_anon_cipher");

        if (ctx == NULL)
            return WOLFSSL_FAILURE;

        ctx->haveAnon = 1;

        return WOLFSSL_SUCCESS;
    }

#endif /* HAVE_ANON */


#ifndef NO_CERTS
/* used to be defined on NO_FILESYSTEM only, but are generally useful */

    /* wolfSSL extension allows DER files to be loaded from buffers as well */
    int wolfSSL_CTX_load_verify_buffer(WOLFSSL_CTX* ctx,
                                       const unsigned char* in,
                                       long sz, int format)
    {
        WOLFSSL_ENTER("wolfSSL_CTX_load_verify_buffer");
        if (format == WOLFSSL_FILETYPE_PEM)
            return ProcessChainBuffer(ctx, in, sz, format, CA_TYPE, NULL);
        else
            return ProcessBuffer(ctx, in, sz, format, CA_TYPE, NULL,NULL,0);
    }


#ifdef WOLFSSL_TRUST_PEER_CERT
    int wolfSSL_CTX_trust_peer_buffer(WOLFSSL_CTX* ctx,
                                       const unsigned char* in,
                                       long sz, int format)
    {
        WOLFSSL_ENTER("wolfSSL_CTX_trust_peer_buffer");

        /* sanity check on arguments */
        if (sz < 0 || in == NULL || ctx == NULL) {
            return BAD_FUNC_ARG;
        }

        if (format == WOLFSSL_FILETYPE_PEM)
            return ProcessChainBuffer(ctx, in, sz, format,
                                                       TRUSTED_PEER_TYPE, NULL);
        else
            return ProcessBuffer(ctx, in, sz, format, TRUSTED_PEER_TYPE,
                                                                   NULL,NULL,0);
    }
#endif /* WOLFSSL_TRUST_PEER_CERT */


    int wolfSSL_CTX_use_certificate_buffer(WOLFSSL_CTX* ctx,
                                 const unsigned char* in, long sz, int format)
    {
        WOLFSSL_ENTER("wolfSSL_CTX_use_certificate_buffer");
        return ProcessBuffer(ctx, in, sz, format, CERT_TYPE, NULL, NULL, 0);
    }


    int wolfSSL_CTX_use_PrivateKey_buffer(WOLFSSL_CTX* ctx,
                                 const unsigned char* in, long sz, int format)
    {
        WOLFSSL_ENTER("wolfSSL_CTX_use_PrivateKey_buffer");
        return ProcessBuffer(ctx, in, sz, format, PRIVATEKEY_TYPE, NULL,NULL,0);
    }


    int wolfSSL_CTX_use_certificate_chain_buffer_format(WOLFSSL_CTX* ctx,
                                 const unsigned char* in, long sz, int format)
    {
        WOLFSSL_ENTER("wolfSSL_CTX_use_certificate_chain_buffer_format");
        return ProcessBuffer(ctx, in, sz, format, CERT_TYPE, NULL, NULL, 1);
    }

    int wolfSSL_CTX_use_certificate_chain_buffer(WOLFSSL_CTX* ctx,
                                 const unsigned char* in, long sz)
    {
        return wolfSSL_CTX_use_certificate_chain_buffer_format(ctx, in, sz,
                                                            WOLFSSL_FILETYPE_PEM);
    }


#ifndef NO_DH

    /* server wrapper for ctx or ssl Diffie-Hellman parameters */
    static int wolfSSL_SetTmpDH_buffer_wrapper(WOLFSSL_CTX* ctx, WOLFSSL* ssl,
                                               const unsigned char* buf,
                                               long sz, int format)
    {
        DerBuffer* der = NULL;
        int    ret      = 0;
        word32 pSz = MAX_DH_SIZE;
        word32 gSz = MAX_DH_SIZE;
    #ifdef WOLFSSL_SMALL_STACK
        byte*  p = NULL;
        byte*  g = NULL;
    #else
        byte   p[MAX_DH_SIZE];
        byte   g[MAX_DH_SIZE];
    #endif

        if (ctx == NULL || buf == NULL)
            return BAD_FUNC_ARG;

        ret = AllocDer(&der, 0, DH_PARAM_TYPE, ctx->heap);
        if (ret != 0) {
            return ret;
        }
        der->buffer = (byte*)buf;
        der->length = (word32)sz;

    #ifdef WOLFSSL_SMALL_STACK
        p = (byte*)XMALLOC(pSz, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
        g = (byte*)XMALLOC(gSz, NULL, DYNAMIC_TYPE_PUBLIC_KEY);

        if (p == NULL || g == NULL) {
            XFREE(p, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
            XFREE(g, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
            return MEMORY_E;
        }
    #endif

        if (format != WOLFSSL_FILETYPE_ASN1 && format != WOLFSSL_FILETYPE_PEM)
            ret = WOLFSSL_BAD_FILETYPE;
        else {
            if (format == WOLFSSL_FILETYPE_PEM) {
#ifdef WOLFSSL_PEM_TO_DER
                FreeDer(&der);
                ret = PemToDer(buf, sz, DH_PARAM_TYPE, &der, ctx->heap,
                               NULL, NULL);
    #ifdef WOLFSSL_WPAS
        #ifndef NO_DSA
                if (ret < 0) {
                    ret = PemToDer(buf, sz, DSA_PARAM_TYPE, &der, ctx->heap,
                               NULL, NULL);
                }
        #endif
    #endif /* WOLFSSL_WPAS */
#else
                ret = NOT_COMPILED_IN;
#endif /* WOLFSSL_PEM_TO_DER */
            }

            if (ret == 0) {
                if (wc_DhParamsLoad(der->buffer, der->length, p, &pSz, g, &gSz) < 0)
                    ret = WOLFSSL_BAD_FILETYPE;
                else if (ssl)
                    ret = wolfSSL_SetTmpDH(ssl, p, pSz, g, gSz);
                else
                    ret = wolfSSL_CTX_SetTmpDH(ctx, p, pSz, g, gSz);
            }
        }

        FreeDer(&der);

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(p, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
        XFREE(g, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
    #endif

        return ret;
    }


    /* server Diffie-Hellman parameters, WOLFSSL_SUCCESS on ok */
    int wolfSSL_SetTmpDH_buffer(WOLFSSL* ssl, const unsigned char* buf, long sz,
                               int format)
    {
        if (ssl == NULL)
            return BAD_FUNC_ARG;

        return wolfSSL_SetTmpDH_buffer_wrapper(ssl->ctx, ssl, buf, sz, format);
    }


    /* server ctx Diffie-Hellman parameters, WOLFSSL_SUCCESS on ok */
    int wolfSSL_CTX_SetTmpDH_buffer(WOLFSSL_CTX* ctx, const unsigned char* buf,
                                   long sz, int format)
    {
        return wolfSSL_SetTmpDH_buffer_wrapper(ctx, NULL, buf, sz, format);
    }

#endif /* NO_DH */


    int wolfSSL_use_certificate_buffer(WOLFSSL* ssl,
                                 const unsigned char* in, long sz, int format)
    {
        WOLFSSL_ENTER("wolfSSL_use_certificate_buffer");
        return ProcessBuffer(ssl->ctx, in, sz, format,CERT_TYPE,ssl,NULL,0);
    }


    int wolfSSL_use_PrivateKey_buffer(WOLFSSL* ssl,
                                 const unsigned char* in, long sz, int format)
    {
        WOLFSSL_ENTER("wolfSSL_use_PrivateKey_buffer");
        return ProcessBuffer(ssl->ctx, in, sz, format, PRIVATEKEY_TYPE,
                             ssl, NULL, 0);
    }

    int wolfSSL_use_certificate_chain_buffer_format(WOLFSSL* ssl,
                                 const unsigned char* in, long sz, int format)
    {
        WOLFSSL_ENTER("wolfSSL_use_certificate_chain_buffer_format");
        return ProcessBuffer(ssl->ctx, in, sz, format, CERT_TYPE,
                             ssl, NULL, 1);
    }

    int wolfSSL_use_certificate_chain_buffer(WOLFSSL* ssl,
                                 const unsigned char* in, long sz)
    {
        return wolfSSL_use_certificate_chain_buffer_format(ssl, in, sz,
                                                            WOLFSSL_FILETYPE_PEM);
    }


    /* unload any certs or keys that SSL owns, leave CTX as is
       WOLFSSL_SUCCESS on ok */
    int wolfSSL_UnloadCertsKeys(WOLFSSL* ssl)
    {
        if (ssl == NULL) {
            WOLFSSL_MSG("Null function arg");
            return BAD_FUNC_ARG;
        }

        if (ssl->buffers.weOwnCert && !ssl->keepCert) {
            WOLFSSL_MSG("Unloading cert");
            FreeDer(&ssl->buffers.certificate);
            #ifdef KEEP_OUR_CERT
                FreeX509(ssl->ourCert);
                if (ssl->ourCert) {
                    XFREE(ssl->ourCert, ssl->heap, DYNAMIC_TYPE_X509);
                    ssl->ourCert = NULL;
                }
            #endif
            ssl->buffers.weOwnCert = 0;
        }

        if (ssl->buffers.weOwnCertChain) {
            WOLFSSL_MSG("Unloading cert chain");
            FreeDer(&ssl->buffers.certChain);
            ssl->buffers.weOwnCertChain = 0;
        }

        if (ssl->buffers.weOwnKey) {
            WOLFSSL_MSG("Unloading key");
            FreeDer(&ssl->buffers.key);
            ssl->buffers.weOwnKey = 0;
        }

        return WOLFSSL_SUCCESS;
    }


    int wolfSSL_CTX_UnloadCAs(WOLFSSL_CTX* ctx)
    {
        WOLFSSL_ENTER("wolfSSL_CTX_UnloadCAs");

        if (ctx == NULL)
            return BAD_FUNC_ARG;

        return wolfSSL_CertManagerUnloadCAs(ctx->cm);
    }


#ifdef WOLFSSL_TRUST_PEER_CERT
    int wolfSSL_CTX_Unload_trust_peers(WOLFSSL_CTX* ctx)
    {
        WOLFSSL_ENTER("wolfSSL_CTX_Unload_trust_peers");

        if (ctx == NULL)
            return BAD_FUNC_ARG;

        return wolfSSL_CertManagerUnload_trust_peers(ctx->cm);
    }
#endif /* WOLFSSL_TRUST_PEER_CERT */
/* old NO_FILESYSTEM end */
#endif /* !NO_CERTS */


#ifdef OPENSSL_EXTRA

    int wolfSSL_add_all_algorithms(void)
    {
        WOLFSSL_ENTER("wolfSSL_add_all_algorithms");
        if (wolfSSL_Init() == WOLFSSL_SUCCESS)
            return WOLFSSL_SUCCESS;
        else
            return WOLFSSL_FATAL_ERROR;
    }

    int wolfSSL_OPENSSL_add_all_algorithms_noconf(void)
    {
        WOLFSSL_ENTER("wolfSSL_OPENSSL_add_all_algorithms_noconf");

        if  (wolfSSL_add_all_algorithms() == WOLFSSL_FATAL_ERROR)
            return WOLFSSL_FATAL_ERROR;
        
        return  WOLFSSL_SUCCESS;
    }

   /* returns previous set cache size which stays constant */
    long wolfSSL_CTX_sess_set_cache_size(WOLFSSL_CTX* ctx, long sz)
    {
        /* cache size fixed at compile time in wolfSSL */
        (void)ctx;
        (void)sz;
        WOLFSSL_MSG("session cache is set at compile time");
        #ifndef NO_SESSION_CACHE
            return SESSIONS_PER_ROW * SESSION_ROWS;
        #else
            return 0;
        #endif
    }

#endif

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_EXTRA)
    void wolfSSL_CTX_set_quiet_shutdown(WOLFSSL_CTX* ctx, int mode)
    {
        WOLFSSL_ENTER("wolfSSL_CTX_set_quiet_shutdown");
        if (mode)
            ctx->quietShutdown = 1;
    }


    void wolfSSL_set_quiet_shutdown(WOLFSSL* ssl, int mode)
    {
        WOLFSSL_ENTER("wolfSSL_CTX_set_quiet_shutdown");
        if (mode)
            ssl->options.quietShutdown = 1;
    }
#endif

#ifdef OPENSSL_EXTRA
    void wolfSSL_set_bio(WOLFSSL* ssl, WOLFSSL_BIO* rd, WOLFSSL_BIO* wr)
    {
        WOLFSSL_ENTER("wolfSSL_set_bio");

        if (ssl == NULL) {
            WOLFSSL_MSG("Bad argument, ssl was NULL");
            return;
        }

        /* if WOLFSSL_BIO is socket type then set WOLFSSL socket to use */
        if (rd != NULL && rd->type == WOLFSSL_BIO_SOCKET) {
            wolfSSL_set_rfd(ssl, rd->fd);
        }
        if (wr != NULL && wr->type == WOLFSSL_BIO_SOCKET) {
            wolfSSL_set_wfd(ssl, wr->fd);
        }

        /* free any existing WOLFSSL_BIOs in use */
        if (ssl->biord != NULL) {
            if (ssl->biord != ssl->biowr) {
                if (ssl->biowr != NULL) {
                    wolfSSL_BIO_free(ssl->biowr);
                    ssl->biowr = NULL;
                }
            }
            wolfSSL_BIO_free(ssl->biord);
            ssl->biord = NULL;
        }


        ssl->biord = rd;
        ssl->biowr = wr;

        /* set SSL to use BIO callbacks instead */
        if (rd != NULL && rd->type != WOLFSSL_BIO_SOCKET) {
            ssl->CBIORecv = BioReceive;
        }
        if (wr != NULL && wr->type != WOLFSSL_BIO_SOCKET) {
            ssl->CBIOSend = BioSend;
        }
    }
#endif

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_EXTRA)
    void wolfSSL_CTX_set_client_CA_list(WOLFSSL_CTX* ctx,
                                       WOLF_STACK_OF(WOLFSSL_X509_NAME)* names)
    {
        WOLFSSL_ENTER("wolfSSL_SSL_CTX_set_client_CA_list");

        if (ctx != NULL)
            ctx->ca_names = names;
    }

    WOLF_STACK_OF(WOLFSSL_X509_NAME)* wolfSSL_SSL_CTX_get_client_CA_list(
            const WOLFSSL_CTX *s)
    {
        WOLFSSL_ENTER("wolfSSL_SSL_CTX_get_client_CA_list");

        if (s == NULL)
            return NULL;

        return s->ca_names;
    }
#endif

#ifdef OPENSSL_EXTRA
    #if !defined(NO_RSA) && !defined(NO_CERTS)
    WOLF_STACK_OF(WOLFSSL_X509_NAME)* wolfSSL_load_client_CA_file(const char* fname)
    {
        WOLFSSL_STACK *list = NULL;
        WOLFSSL_STACK *node;
        WOLFSSL_BIO* bio;
        WOLFSSL_X509 *cert = NULL;
        WOLFSSL_X509_NAME *subjectName = NULL;

        WOLFSSL_ENTER("wolfSSL_load_client_CA_file");

        bio = wolfSSL_BIO_new_file(fname, "r");
        if (bio == NULL)
            return NULL;

        /* Read each certificate in the chain out of the file. */
        while (wolfSSL_PEM_read_bio_X509(bio, &cert, NULL, NULL) != NULL) {
            subjectName = wolfSSL_X509_get_subject_name(cert);
            if (subjectName == NULL)
                break;

            node = (WOLFSSL_STACK*)XMALLOC(sizeof(WOLFSSL_STACK), NULL,
                                           DYNAMIC_TYPE_OPENSSL);
            if (node == NULL)
                break;

            /* Need a persistent copy of the subject name. */
            node->data.name = (WOLFSSL_X509_NAME*)XMALLOC(
                    sizeof(WOLFSSL_X509_NAME), NULL, DYNAMIC_TYPE_OPENSSL);
            if (node->data.name == NULL) {
                XFREE(node, NULL, DYNAMIC_TYPE_OPENSSL);
                break;
            }
            XMEMCPY(node->data.name, subjectName, sizeof(WOLFSSL_X509_NAME));
            /* Clear pointers so freeing certificate doesn't free memory. */
            XMEMSET(subjectName, 0, sizeof(WOLFSSL_X509_NAME));

            /* Put node on the front of the list. */
            node->num  = (list == NULL) ? 1 : list->num + 1;
            node->next = list;
            list = node;

            wolfSSL_X509_free(cert);
            cert = NULL;
        }

        wolfSSL_X509_free(cert);
        wolfSSL_BIO_free(bio);
        return list;
    }

    int wolfSSL_CTX_add_client_CA(WOLFSSL_CTX* ctx, WOLFSSL_X509* x509)
    {
        WOLFSSL_STACK *node = NULL;
        WOLFSSL_X509_NAME *subjectName = NULL;

        WOLFSSL_ENTER("wolfSSL_CTX_add_client_CA");

        if (ctx == NULL || x509 == NULL){
            WOLFSSL_MSG("Bad argument");
            return SSL_FAILURE;
        }

        subjectName = wolfSSL_X509_get_subject_name(x509);
        if (subjectName == NULL){
            WOLFSSL_MSG("invalid x509 data");
            return SSL_FAILURE;
        }

        /* Alloc stack struct */
        node = (WOLF_STACK_OF(WOLFSSL_X509_NAME)*)XMALLOC(
                                           sizeof(WOLF_STACK_OF(WOLFSSL_X509_NAME)),
                                           NULL, DYNAMIC_TYPE_OPENSSL);
        if (node == NULL){
            WOLFSSL_MSG("memory allocation error");
            return SSL_FAILURE;
        }
        XMEMSET(node, 0, sizeof(WOLF_STACK_OF(WOLFSSL_X509_NAME)));

        /* Alloc and copy WOLFSSL_X509_NAME */
        node->data.name = (WOLFSSL_X509_NAME*)XMALLOC(
                                              sizeof(WOLFSSL_X509_NAME),
                                              NULL, DYNAMIC_TYPE_OPENSSL);
        if (node->data.name == NULL) {
            XFREE(node, NULL, DYNAMIC_TYPE_OPENSSL);
            WOLFSSL_MSG("memory allocation error");
            return SSL_FAILURE;
        }
        XMEMCPY(node->data.name, subjectName, sizeof(WOLFSSL_X509_NAME));
        XMEMSET(subjectName, 0, sizeof(WOLFSSL_X509_NAME));

        /* push new node onto head of stack */
        node->num = (ctx->ca_names == NULL) ? 1 : ctx->ca_names->num + 1;
        node->next = ctx->ca_names;
        ctx->ca_names = node;
        return SSL_SUCCESS;
    }
    #endif

    #ifndef NO_WOLFSSL_STUB
    int wolfSSL_CTX_set_default_verify_paths(WOLFSSL_CTX* ctx)
    {
        /* TODO:, not needed in goahead */
        (void)ctx;
        WOLFSSL_STUB("SSL_CTX_set_default_verify_paths");
        return SSL_NOT_IMPLEMENTED;
    }
    #endif

    #if defined(WOLFCRYPT_HAVE_SRP) && !defined(NO_SHA256) \
        && !defined(WC_NO_RNG)
    static const byte srp_N[] = {
        0xEE, 0xAF, 0x0A, 0xB9, 0xAD, 0xB3, 0x8D, 0xD6, 0x9C, 0x33, 0xF8,
        0x0A, 0xFA, 0x8F, 0xC5, 0xE8, 0x60, 0x72, 0x61, 0x87, 0x75, 0xFF,
        0x3C, 0x0B, 0x9E, 0xA2, 0x31, 0x4C, 0x9C, 0x25, 0x65, 0x76, 0xD6,
        0x74, 0xDF, 0x74, 0x96, 0xEA, 0x81, 0xD3, 0x38, 0x3B, 0x48, 0x13,
        0xD6, 0x92, 0xC6, 0xE0, 0xE0, 0xD5, 0xD8, 0xE2, 0x50, 0xB9, 0x8B,
        0xE4, 0x8E, 0x49, 0x5C, 0x1D, 0x60, 0x89, 0xDA, 0xD1, 0x5D, 0xC7,
        0xD7, 0xB4, 0x61, 0x54, 0xD6, 0xB6, 0xCE, 0x8E, 0xF4, 0xAD, 0x69,
        0xB1, 0x5D, 0x49, 0x82, 0x55, 0x9B, 0x29, 0x7B, 0xCF, 0x18, 0x85,
        0xC5, 0x29, 0xF5, 0x66, 0x66, 0x0E, 0x57, 0xEC, 0x68, 0xED, 0xBC,
        0x3C, 0x05, 0x72, 0x6C, 0xC0, 0x2F, 0xD4, 0xCB, 0xF4, 0x97, 0x6E,
        0xAA, 0x9A, 0xFD, 0x51, 0x38, 0xFE, 0x83, 0x76, 0x43, 0x5B, 0x9F,
        0xC6, 0x1D, 0x2F, 0xC0, 0xEB, 0x06, 0xE3
    };
    static const byte srp_g[] = {
        0x02
    };

    int wolfSSL_CTX_set_srp_username(WOLFSSL_CTX* ctx, char* username)
    {
        int r = 0;
        SrpSide srp_side = SRP_CLIENT_SIDE;
        WC_RNG rng;
        byte salt[SRP_SALT_SIZE];

        WOLFSSL_ENTER("wolfSSL_CTX_set_srp_username");
        if (ctx == NULL || ctx->srp == NULL || username==NULL)
            return SSL_FAILURE;

        if (ctx->method->side == WOLFSSL_SERVER_END){
            srp_side = SRP_SERVER_SIDE;
        } else if (ctx->method->side == WOLFSSL_CLIENT_END){
            srp_side = SRP_CLIENT_SIDE;
        } else {
            WOLFSSL_MSG("Init CTX failed");
            return SSL_FAILURE;
        }

        if (wc_SrpInit(ctx->srp, SRP_TYPE_SHA256, srp_side) < 0){
            WOLFSSL_MSG("Init CTX failed");
            XFREE(ctx->srp, ctx->heap, DYNAMIC_TYPE_SRP);
            wolfSSL_CTX_free(ctx);
            return SSL_FAILURE;
        }
        r = wc_SrpSetUsername(ctx->srp, (const byte*)username,
                              (word32)XSTRLEN(username));
        if (r < 0) {
            WOLFSSL_MSG("fail to set srp username.");
            return SSL_FAILURE;
        }

        /* if wolfSSL_CTX_set_srp_password has already been called, */
        /* execute wc_SrpSetPassword here */
        if (ctx->srp_password != NULL){
            if (wc_InitRng(&rng) < 0){
                WOLFSSL_MSG("wc_InitRng failed");
                return SSL_FAILURE;
            }
            XMEMSET(salt, 0, sizeof(salt)/sizeof(salt[0]));
            if (wc_RNG_GenerateBlock(&rng, salt,
                                     sizeof(salt)/sizeof(salt[0])) <  0){
                WOLFSSL_MSG("wc_RNG_GenerateBlock failed");
                wc_FreeRng(&rng);
                return SSL_FAILURE;
            }
            if (wc_SrpSetParams(ctx->srp, srp_N, sizeof(srp_N)/sizeof(srp_N[0]),
                                srp_g, sizeof(srp_g)/sizeof(srp_g[0]),
                                salt, sizeof(salt)/sizeof(salt[0])) < 0){
                WOLFSSL_MSG("wc_SrpSetParam failed");
                wc_FreeRng(&rng);
                return SSL_FAILURE;
            }
            r = wc_SrpSetPassword(ctx->srp,
                     (const byte*)ctx->srp_password,
                     (word32)XSTRLEN((char *)ctx->srp_password));
            if (r < 0) {
                WOLFSSL_MSG("fail to set srp password.");
                return SSL_FAILURE;
            }
            wc_FreeRng(&rng);
            XFREE(ctx->srp_password, ctx->heap, DYNAMIC_TYPE_SRP);
            ctx->srp_password = NULL;
        }

        return SSL_SUCCESS;
    }

    int wolfSSL_CTX_set_srp_password(WOLFSSL_CTX* ctx, char* password)
    {
        int r;
        WC_RNG rng;
        byte salt[SRP_SALT_SIZE];

        WOLFSSL_ENTER("wolfSSL_CTX_set_srp_password");
        if (ctx == NULL || ctx->srp == NULL || password == NULL)
            return SSL_FAILURE;

        if (ctx->srp->user != NULL){
            if (wc_InitRng(&rng) < 0){
                WOLFSSL_MSG("wc_InitRng failed");
                return SSL_FAILURE;
            }
            XMEMSET(salt, 0, sizeof(salt)/sizeof(salt[0]));
            if (wc_RNG_GenerateBlock(&rng, salt,
                                     sizeof(salt)/sizeof(salt[0])) <  0){
                WOLFSSL_MSG("wc_RNG_GenerateBlock failed");
                wc_FreeRng(&rng);
                return SSL_FAILURE;
            }
            if (wc_SrpSetParams(ctx->srp, srp_N, sizeof(srp_N)/sizeof(srp_N[0]),
                                srp_g, sizeof(srp_g)/sizeof(srp_g[0]),
                                salt, sizeof(salt)/sizeof(salt[0])) < 0){
                WOLFSSL_MSG("wc_SrpSetParam failed");
                wc_FreeRng(&rng);
                return SSL_FAILURE;
            }
            r = wc_SrpSetPassword(ctx->srp, (const byte*)password,
                                  (word32)XSTRLEN(password));
            if (r < 0) {
                WOLFSSL_MSG("wc_SrpSetPassword failed.");
                wc_FreeRng(&rng);
                return SSL_FAILURE;
            }
            if (ctx->srp_password != NULL){
                XFREE(ctx->srp_password,NULL,
                      DYNAMIC_TYPE_SRP);
                ctx->srp_password = NULL;
            }
            wc_FreeRng(&rng);
        } else {
            /* save password for wolfSSL_set_srp_username */
            if (ctx->srp_password != NULL)
                XFREE(ctx->srp_password,ctx->heap, DYNAMIC_TYPE_SRP);

            ctx->srp_password = (byte*)XMALLOC(XSTRLEN(password) + 1, ctx->heap,
                                               DYNAMIC_TYPE_SRP);
            if (ctx->srp_password == NULL){
                WOLFSSL_MSG("memory allocation error");
                return SSL_FAILURE;
            }
            XMEMCPY(ctx->srp_password, password, XSTRLEN(password) + 1);
        }
        return SSL_SUCCESS;
    }
    #endif /* WOLFCRYPT_HAVE_SRP && !NO_SHA256 && !WC_NO_RNG */

    /* keyblock size in bytes or -1 */
    int wolfSSL_get_keyblock_size(WOLFSSL* ssl)
    {
        if (ssl == NULL)
            return WOLFSSL_FATAL_ERROR;

        return 2 * (ssl->specs.key_size + ssl->specs.iv_size +
                    ssl->specs.hash_size);
    }


    /* store keys returns WOLFSSL_SUCCESS or -1 on error */
    int wolfSSL_get_keys(WOLFSSL* ssl, unsigned char** ms, unsigned int* msLen,
                                     unsigned char** sr, unsigned int* srLen,
                                     unsigned char** cr, unsigned int* crLen)
    {
        if (ssl == NULL || ssl->arrays == NULL)
            return WOLFSSL_FATAL_ERROR;

        *ms = ssl->arrays->masterSecret;
        *sr = ssl->arrays->serverRandom;
        *cr = ssl->arrays->clientRandom;

        *msLen = SECRET_LEN;
        *srLen = RAN_LEN;
        *crLen = RAN_LEN;

        return WOLFSSL_SUCCESS;
    }

#endif /* OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_EXTRA)
    void wolfSSL_set_accept_state(WOLFSSL* ssl)
    {
        word16 haveRSA = 1;
        word16 havePSK = 0;

        WOLFSSL_ENTER("SSL_set_accept_state");
        if (ssl->options.side == WOLFSSL_CLIENT_END) {
    #ifdef HAVE_ECC
            ecc_key key;
            word32 idx = 0;

            if (ssl->options.haveStaticECC && ssl->buffers.key != NULL) {
                wc_ecc_init(&key);
                if (wc_EccPrivateKeyDecode(ssl->buffers.key->buffer, &idx, &key,
                                               ssl->buffers.key->length) != 0) {
                    ssl->options.haveECDSAsig = 0;
                    ssl->options.haveECC = 0;
                    ssl->options.haveStaticECC = 0;
                }
                wc_ecc_free(&key);
            }
    #endif

    #ifndef NO_DH
            if (!ssl->options.haveDH && ssl->ctx->haveDH) {
                ssl->buffers.serverDH_P = ssl->ctx->serverDH_P;
                ssl->buffers.serverDH_G = ssl->ctx->serverDH_G;
                ssl->options.haveDH = 1;
            }
    #endif
        }
        ssl->options.side = WOLFSSL_SERVER_END;
        /* reset suites in case user switched */

        #ifdef NO_RSA
            haveRSA = 0;
        #endif
        #ifndef NO_PSK
            havePSK = ssl->options.havePSK;
        #endif
        InitSuites(ssl->suites, ssl->version, ssl->buffers.keySz, haveRSA,
                   havePSK, ssl->options.haveDH, ssl->options.haveNTRU,
                   ssl->options.haveECDSAsig, ssl->options.haveECC,
                   ssl->options.haveStaticECC, ssl->options.side);
    }

#endif /* OPENSSL_EXTRA || WOLFSSL_EXTRA */

    /* return true if connection established */
    int wolfSSL_is_init_finished(WOLFSSL* ssl)
    {
        if (ssl == NULL)
            return 0;

        if (ssl->options.handShakeState == HANDSHAKE_DONE)
            return 1;

        return 0;
    }

#ifdef OPENSSL_EXTRA

    void wolfSSL_CTX_set_tmp_rsa_callback(WOLFSSL_CTX* ctx,
                                      WOLFSSL_RSA*(*f)(WOLFSSL*, int, int))
    {
        /* wolfSSL verifies all these internally */
        (void)ctx;
        (void)f;
    }


    void wolfSSL_set_shutdown(WOLFSSL* ssl, int opt)
    {
        WOLFSSL_ENTER("wolfSSL_set_shutdown");
        if(ssl==NULL) {
            WOLFSSL_MSG("Shutdown not set. ssl is null");
            return;
        }

        ssl->options.sentNotify =  (opt&WOLFSSL_SENT_SHUTDOWN) > 0;
        ssl->options.closeNotify = (opt&WOLFSSL_RECEIVED_SHUTDOWN) > 0;
    }


    long wolfSSL_CTX_get_options(WOLFSSL_CTX* ctx)
    {
        WOLFSSL_ENTER("wolfSSL_CTX_get_options");
        WOLFSSL_MSG("wolfSSL options are set through API calls and macros");
        if(ctx == NULL)
            return BAD_FUNC_ARG;
        return ctx->mask;
    }


    long wolfSSL_CTX_set_options(WOLFSSL_CTX* ctx, long opt)
    {
        WOLFSSL *ssl;
        WOLFSSL_ENTER("SSL_CTX_set_options");
        if(ctx == NULL)
            return BAD_FUNC_ARG;
        ssl = wolfSSL_new(ctx);
        if(ssl == NULL)
            return SSL_FAILURE;
        ctx->mask = wolfSSL_set_options(ssl, opt);
        wolfSSL_free(ssl);
        return ctx->mask;
    }

    long wolfSSL_CTX_clear_options(WOLFSSL_CTX* ctx, long opt)
    {
        WOLFSSL_ENTER("SSL_CTX_clear_options");
        if(ctx == NULL)
            return BAD_FUNC_ARG;
        ctx->mask &= ~opt;
        return ctx->mask;
    }

    int wolfSSL_set_rfd(WOLFSSL* ssl, int rfd)
    {
        WOLFSSL_ENTER("SSL_set_rfd");
        ssl->rfd = rfd;      /* not used directly to allow IO callbacks */

        ssl->IOCB_ReadCtx  = &ssl->rfd;

        return WOLFSSL_SUCCESS;
    }


    int wolfSSL_set_wfd(WOLFSSL* ssl, int wfd)
    {
        WOLFSSL_ENTER("SSL_set_wfd");
        ssl->wfd = wfd;      /* not used directly to allow IO callbacks */

        ssl->IOCB_WriteCtx  = &ssl->wfd;

        return WOLFSSL_SUCCESS;
    }




#ifndef NO_CERTS
    WOLFSSL_X509_STORE* wolfSSL_CTX_get_cert_store(WOLFSSL_CTX* ctx)
    {
        if (ctx == NULL) {
            return NULL;
        }

        return &ctx->x509_store;
    }


    void wolfSSL_CTX_set_cert_store(WOLFSSL_CTX* ctx, WOLFSSL_X509_STORE* str)
    {
        if (ctx == NULL || str == NULL) {
            return;
        }

        /* free cert manager if have one */
        if (ctx->cm != NULL) {
            wolfSSL_CertManagerFree(ctx->cm);
        }
        ctx->cm               = str->cm;
        ctx->x509_store.cache = str->cache;
        ctx->x509_store_pt    = str; /* take ownership of store and free it
                                        with CTX free */
    }


    WOLFSSL_X509* wolfSSL_X509_STORE_CTX_get_current_cert(
                                                    WOLFSSL_X509_STORE_CTX* ctx)
    {
        WOLFSSL_ENTER("wolfSSL_X509_STORE_CTX_get_current_cert");
        if (ctx)
            return ctx->current_cert;
        return NULL;
    }


    int wolfSSL_X509_STORE_CTX_get_error(WOLFSSL_X509_STORE_CTX* ctx)
    {
        WOLFSSL_ENTER("wolfSSL_X509_STORE_CTX_get_error");
        if (ctx != NULL)
            return ctx->error;
        return 0;
    }


    int wolfSSL_X509_STORE_CTX_get_error_depth(WOLFSSL_X509_STORE_CTX* ctx)
    {
        WOLFSSL_ENTER("wolfSSL_X509_STORE_CTX_get_error_depth");
        if(ctx)
            return ctx->error_depth;
        return WOLFSSL_FATAL_ERROR;
    }

    void wolfSSL_X509_STORE_CTX_set_verify_cb(WOLFSSL_X509_STORE_CTX *ctx,
                                  WOLFSSL_X509_STORE_CTX_verify_cb verify_cb)
    {
        WOLFSSL_ENTER("wolfSSL_X509_STORE_CTX_set_verify_cb");
        if(ctx == NULL)
            return;
        ctx->verify_cb = verify_cb;
    }
#endif /* !NO_CERTS */

    WOLFSSL_BIO_METHOD* wolfSSL_BIO_f_buffer(void)
    {
        static WOLFSSL_BIO_METHOD meth;

        WOLFSSL_ENTER("BIO_f_buffer");
        meth.type = WOLFSSL_BIO_BUFFER;

        return &meth;
    }

    #ifndef NO_WOLFSSL_STUB
    long wolfSSL_BIO_set_write_buffer_size(WOLFSSL_BIO* bio, long size)
    {
        /* wolfSSL has internal buffer, compatibility only */
        WOLFSSL_ENTER("BIO_set_write_buffer_size");
        WOLFSSL_STUB("BIO_set_write_buffer_size");
        (void)bio;
        return size;
    }
    #endif

    WOLFSSL_BIO_METHOD* wolfSSL_BIO_s_bio(void)
    {
        static WOLFSSL_BIO_METHOD bio_meth;

        WOLFSSL_ENTER("wolfSSL_BIO_f_bio");
        bio_meth.type = WOLFSSL_BIO_BIO;

        return &bio_meth;
    }


#ifndef NO_FILESYSTEM
    WOLFSSL_BIO_METHOD* wolfSSL_BIO_s_file(void)
    {
        static WOLFSSL_BIO_METHOD file_meth;

        WOLFSSL_ENTER("wolfSSL_BIO_f_file");
        file_meth.type = WOLFSSL_BIO_FILE;

        return &file_meth;
    }
#endif


    WOLFSSL_BIO_METHOD* wolfSSL_BIO_f_ssl(void)
    {
        static WOLFSSL_BIO_METHOD meth;

        WOLFSSL_ENTER("BIO_f_ssl");
        meth.type = WOLFSSL_BIO_SSL;

        return &meth;
    }


    WOLFSSL_BIO_METHOD *wolfSSL_BIO_s_socket(void)
    {
        static WOLFSSL_BIO_METHOD meth;

        WOLFSSL_ENTER("BIO_s_socket");
        meth.type = WOLFSSL_BIO_SOCKET;

        return &meth;
    }


    WOLFSSL_BIO* wolfSSL_BIO_new_socket(int sfd, int closeF)
    {
        WOLFSSL_BIO* bio = wolfSSL_BIO_new(wolfSSL_BIO_s_socket());

        WOLFSSL_ENTER("BIO_new_socket");
        if (bio) {
            bio->type  = WOLFSSL_BIO_SOCKET;
            bio->close = (byte)closeF;
            bio->fd    = sfd;
        }
        return bio;
    }


    int wolfSSL_BIO_eof(WOLFSSL_BIO* b)
    {
        WOLFSSL_ENTER("BIO_eof");
        if (b->eof)
            return 1;

        return 0;
    }


    long wolfSSL_BIO_set_ssl(WOLFSSL_BIO* b, WOLFSSL* ssl, int closeF)
    {
        WOLFSSL_ENTER("wolfSSL_BIO_set_ssl");

        if (b != NULL) {
            b->ssl   = ssl;
            b->close = (byte)closeF;
    /* add to ssl for bio free if SSL_free called before/instead of free_all? */
        }

        return 0;
    }


    long wolfSSL_BIO_set_fd(WOLFSSL_BIO* b, int fd, int closeF)
    {
        WOLFSSL_ENTER("wolfSSL_BIO_set_fd");

        if (b != NULL) {
            b->fd    = fd;
            b->close = (byte)closeF;
        }

        return WOLFSSL_SUCCESS;
    }


    WOLFSSL_BIO* wolfSSL_BIO_new(WOLFSSL_BIO_METHOD* method)
    {
        WOLFSSL_BIO* bio = (WOLFSSL_BIO*) XMALLOC(sizeof(WOLFSSL_BIO), 0,
                                                DYNAMIC_TYPE_OPENSSL);
        WOLFSSL_ENTER("wolfSSL_BIO_new");
        if (bio) {
            XMEMSET(bio, 0, sizeof(WOLFSSL_BIO));
            bio->type   = method->type;
            bio->close  = BIO_CLOSE; /* default to close things */
            if (method->type != WOLFSSL_BIO_FILE &&
                    method->type != WOLFSSL_BIO_SOCKET) {
                bio->mem_buf =(WOLFSSL_BUF_MEM*)XMALLOC(sizeof(WOLFSSL_BUF_MEM),
                                                       0, DYNAMIC_TYPE_OPENSSL);
                if (bio->mem_buf == NULL) {
                    WOLFSSL_MSG("Memory error");
                    wolfSSL_BIO_free(bio);
                    return NULL;
                }
                bio->mem_buf->data = (char*)bio->mem;
            }
        }
        return bio;
    }


    int wolfSSL_BIO_get_mem_data(WOLFSSL_BIO* bio, void* p)
    {
        WOLFSSL_ENTER("wolfSSL_BIO_get_mem_data");

        if (bio == NULL || p == NULL)
            return WOLFSSL_FATAL_ERROR;

        *(byte **)p = bio->mem;

        return bio->memLen;
    }


    WOLFSSL_BIO* wolfSSL_BIO_new_mem_buf(void* buf, int len)
    {
        WOLFSSL_BIO* bio = NULL;

        if (buf == NULL || len < 0) {
            return bio;
        }

        bio = wolfSSL_BIO_new(wolfSSL_BIO_s_mem());
        if (bio == NULL) {
            return bio;
        }

        bio->memLen = bio->wrSz = len;
        bio->mem    = (byte*)XMALLOC(len, 0, DYNAMIC_TYPE_OPENSSL);
        if (bio->mem == NULL) {
            wolfSSL_BIO_free(bio);
            return NULL;
        }
        if (bio->mem_buf != NULL) {
            bio->mem_buf->data = (char*)bio->mem;
            bio->mem_buf->length = bio->memLen;
        }

        XMEMCPY(bio->mem, buf, len);

        return bio;
    }

    /*
     * Note : If the flag BIO_NOCLOSE is set then freeing memory buffers is up
     *        to the application.
     */
    int wolfSSL_BIO_free(WOLFSSL_BIO* bio)
    {
        /* unchain?, doesn't matter in goahead since from free all */
        WOLFSSL_ENTER("wolfSSL_BIO_free");
        if (bio) {
            /* remove from pair by setting the paired bios pair to NULL */
            if (bio->pair != NULL) {
                bio->pair->pair = NULL;
            }

            if (bio->close) {
                if (bio->ssl)
                    wolfSSL_free(bio->ssl);
                if (bio->fd)
                    CloseSocket(bio->fd);
            }

        #ifndef NO_FILESYSTEM
            if (bio->type == WOLFSSL_BIO_FILE && bio->close == BIO_CLOSE) {
                if (bio->file) {
                    XFCLOSE(bio->file);
                }
            }
        #endif

            if (bio->close != BIO_NOCLOSE) {
                if (bio->mem != NULL) {
                    if (bio->mem_buf != NULL) {
                        if (bio->mem_buf->data != (char*)bio->mem) {
                            XFREE(bio->mem, bio->heap, DYNAMIC_TYPE_OPENSSL);
                            bio->mem = NULL;
                        }
                    }
                    else {
                        XFREE(bio->mem, bio->heap, DYNAMIC_TYPE_OPENSSL);
                        bio->mem = NULL;
                    }
                }
                if (bio->mem_buf != NULL) {
                    wolfSSL_BUF_MEM_free(bio->mem_buf);
                    bio->mem_buf = NULL;
                }
            }

            XFREE(bio, 0, DYNAMIC_TYPE_OPENSSL);
        }
        return 0;
    }


    int wolfSSL_BIO_free_all(WOLFSSL_BIO* bio)
    {
        WOLFSSL_ENTER("BIO_free_all");
        while (bio) {
            WOLFSSL_BIO* next = bio->next;
            wolfSSL_BIO_free(bio);
            bio = next;
        }
        return 0;
    }


    WOLFSSL_BIO* wolfSSL_BIO_push(WOLFSSL_BIO* top, WOLFSSL_BIO* append)
    {
        WOLFSSL_ENTER("BIO_push");
        top->next    = append;
        append->prev = top;

        return top;
    }


    int wolfSSL_BIO_flush(WOLFSSL_BIO* bio)
    {
        /* for wolfSSL no flushing needed */
        WOLFSSL_ENTER("BIO_flush");
        (void)bio;
        return 1;
    }
#endif /* OPENSSL_EXTRA */

#ifdef WOLFSSL_ENCRYPTED_KEYS

    void wolfSSL_CTX_set_default_passwd_cb_userdata(WOLFSSL_CTX* ctx,
                                                   void* userdata)
    {
        WOLFSSL_ENTER("SSL_CTX_set_default_passwd_cb_userdata");
        ctx->passwd_userdata = userdata;
    }


    void wolfSSL_CTX_set_default_passwd_cb(WOLFSSL_CTX* ctx,pem_password_cb* cb)
    {
        WOLFSSL_ENTER("SSL_CTX_set_default_passwd_cb");
        if (ctx != NULL) {
            ctx->passwd_cb = cb;
        }
    }

    pem_password_cb* wolfSSL_CTX_get_default_passwd_cb(WOLFSSL_CTX *ctx)
    {
        if (ctx == NULL || ctx->passwd_cb == NULL) {
            return NULL;
        }

        return ctx->passwd_cb;
    }


    void* wolfSSL_CTX_get_default_passwd_cb_userdata(WOLFSSL_CTX *ctx)
    {
        if (ctx == NULL) {
            return NULL;
        }

        return ctx->passwd_userdata;
    }

#if !defined(NO_PWDBASED) && (defined(OPENSSL_EXTRA) || \
        defined(OPENSSL_EXTRA_X509_SMALL) || defined(HAVE_WEBSERVER))

    int wolfSSL_EVP_BytesToKey(const WOLFSSL_EVP_CIPHER* type,
                       const WOLFSSL_EVP_MD* md, const byte* salt,
                       const byte* data, int sz, int count, byte* key, byte* iv)
    {
        int ret;
        int hashType = WC_HASH_TYPE_NONE;
    #ifdef WOLFSSL_SMALL_STACK
        EncryptedInfo* info = NULL;
    #else
        EncryptedInfo  info[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        info = (EncryptedInfo*)XMALLOC(sizeof(EncryptedInfo), NULL,
                                       DYNAMIC_TYPE_ENCRYPTEDINFO);
        if (info == NULL) {
            WOLFSSL_MSG("malloc failed");
            return WOLFSSL_FAILURE;
        }
    #endif

        XMEMSET(info, 0, sizeof(EncryptedInfo));
        info->ivSz = EVP_SALT_SIZE;

        ret = wolfSSL_EVP_get_hashinfo(md, &hashType, NULL);
        if (ret == 0)
            ret = wc_EncryptedInfoGet(info, type);
        if (ret == 0)
            ret = wc_PBKDF1_ex(key, info->keySz, iv, info->ivSz, data, sz, salt,
                               EVP_SALT_SIZE, count, hashType, NULL);

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(info, NULL, DYNAMIC_TYPE_ENCRYPTEDINFO);
    #endif

        if (ret <= 0)
            return 0; /* failure - for compatibility */

        return ret;
    }

#endif /* !NO_PWDBASED && (OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL || HAVE_WEBSERVER) */
#endif /* WOLFSSL_ENCRYPTED_KEYS */


#if defined(OPENSSL_EXTRA) || defined(HAVE_WEBSERVER)
    int wolfSSL_num_locks(void)
    {
        return 0;
    }

    void wolfSSL_set_locking_callback(void (*f)(int, int, const char*, int))
    {
        WOLFSSL_ENTER("wolfSSL_set_locking_callback");

        if (wc_SetMutexCb(f) != 0) {
            WOLFSSL_MSG("Error when setting mutex call back");
        }
    }


    typedef unsigned long (idCb)(void);
    static idCb* inner_idCb = NULL;

    unsigned long wolfSSL_thread_id(void)
    {
        if (inner_idCb != NULL) {
            return inner_idCb();
        }
        else {
            return 0;
        }
    }


    void wolfSSL_set_id_callback(unsigned long (*f)(void))
    {
        inner_idCb = f;
    }

    unsigned long wolfSSL_ERR_get_error(void)
    {
        WOLFSSL_ENTER("wolfSSL_ERR_get_error");

#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
        {
            unsigned long ret = wolfSSL_ERR_peek_error_line_data(NULL, NULL,
                                                                 NULL, NULL);
            wc_RemoveErrorNode(-1);
            return ret;
        }
#elif (defined(OPENSSL_EXTRA) || defined(DEBUG_WOLFSSL_VERBOSE))
        {
            int ret = wc_PullErrorNode(NULL, NULL, NULL);

            if (ret < 0) {
                if (ret == BAD_STATE_E) return 0; /* no errors in queue */
                WOLFSSL_MSG("Error with pulling error node!");
                WOLFSSL_LEAVE("wolfSSL_ERR_get_error", ret);
                ret = 0 - ret; /* return absolute value of error */

                /* panic and try to clear out nodes */
                wc_ClearErrorNodes();
            }

            return (unsigned long)ret;
        }
#else
        return (unsigned long)(0 - NOT_COMPILED_IN);
#endif
    }

#endif /* OPENSSL_EXTRA || HAVE_WEBSERVER */


#ifdef OPENSSL_EXTRA

#if !defined(NO_WOLFSSL_SERVER)
size_t wolfSSL_get_server_random(const WOLFSSL *ssl, unsigned char *out,
                                                                   size_t outSz)
{
    size_t size;

    /* return max size of buffer */
    if (outSz == 0) {
        return RAN_LEN;
    }

    if (ssl == NULL || out == NULL) {
        return 0;
    }

    if (ssl->options.saveArrays == 0 || ssl->arrays == NULL) {
        WOLFSSL_MSG("Arrays struct not saved after handshake");
        return 0;
    }

    if (outSz > RAN_LEN) {
        size = RAN_LEN;
    }
    else {
        size = outSz;
    }

    XMEMCPY(out, ssl->arrays->serverRandom, size);
    return size;
}
#endif /* !defined(NO_WOLFSSL_SERVER) */


#if !defined(NO_WOLFSSL_CLIENT)
/* Return the amount of random bytes copied over or error case.
 * ssl : ssl struct after handshake
 * out : buffer to hold random bytes
 * outSz : either 0 (return max buffer sz) or size of out buffer
 *
 * NOTE: wolfSSL_KeepArrays(ssl) must be called to retain handshake information.
 */
size_t wolfSSL_get_client_random(const WOLFSSL* ssl, unsigned char* out,
                                                                   size_t outSz)
{
    size_t size;

    /* return max size of buffer */
    if (outSz == 0) {
        return RAN_LEN;
    }

    if (ssl == NULL || out == NULL) {
        return 0;
    }

    if (ssl->options.saveArrays == 0 || ssl->arrays == NULL) {
        WOLFSSL_MSG("Arrays struct not saved after handshake");
        return 0;
    }

    if (outSz > RAN_LEN) {
        size = RAN_LEN;
    }
    else {
        size = outSz;
    }

    XMEMCPY(out, ssl->arrays->clientRandom, size);
    return size;
}
#endif /* !NO_WOLFSSL_CLIENT */


    unsigned long wolfSSLeay(void)
    {
        return SSLEAY_VERSION_NUMBER;
    }


    const char* wolfSSLeay_version(int type)
    {
        static const char* version = "SSLeay wolfSSL compatibility";
        (void)type;
        return version;
    }


#ifndef NO_MD5
    int wolfSSL_MD5_Init(WOLFSSL_MD5_CTX* md5)
    {
        int ret;
        typedef char md5_test[sizeof(MD5_CTX) >= sizeof(wc_Md5) ? 1 : -1];
        (void)sizeof(md5_test);

        WOLFSSL_ENTER("MD5_Init");
        ret = wc_InitMd5((wc_Md5*)md5);

        /* return 1 on success, 0 otherwise */
        if (ret == 0)
            return 1;

        return 0;
    }


    int wolfSSL_MD5_Update(WOLFSSL_MD5_CTX* md5, const void* input,
                           unsigned long sz)
    {
        int ret;

        WOLFSSL_ENTER("wolfSSL_MD5_Update");
        ret = wc_Md5Update((wc_Md5*)md5, (const byte*)input, (word32)sz);

        /* return 1 on success, 0 otherwise */
        if (ret == 0)
            return 1;

        return 0;
    }


    int wolfSSL_MD5_Final(byte* input, WOLFSSL_MD5_CTX* md5)
    {
        int ret;

        WOLFSSL_ENTER("MD5_Final");
        ret = wc_Md5Final((wc_Md5*)md5, input);

        /* return 1 on success, 0 otherwise */
        if (ret == 0)
            return 1;

        return 0;
    }
#endif /* !NO_MD5 */


#ifndef NO_SHA
    int wolfSSL_SHA_Init(WOLFSSL_SHA_CTX* sha)
    {
        int ret;

        typedef char sha_test[sizeof(SHA_CTX) >= sizeof(wc_Sha) ? 1 : -1];
        (void)sizeof(sha_test);

        WOLFSSL_ENTER("SHA_Init");
        ret = wc_InitSha((wc_Sha*)sha);

        /* return 1 on success, 0 otherwise */
        if (ret == 0)
            return 1;

        return 0;
    }


    int wolfSSL_SHA_Update(WOLFSSL_SHA_CTX* sha, const void* input,
                           unsigned long sz)
    {
        int ret;

        WOLFSSL_ENTER("SHA_Update");
        ret = wc_ShaUpdate((wc_Sha*)sha, (const byte*)input, (word32)sz);

        /* return 1 on success, 0 otherwise */
        if (ret == 0)
            return 1;

        return 0;
    }


    int wolfSSL_SHA_Final(byte* input, WOLFSSL_SHA_CTX* sha)
    {
        int ret;

        WOLFSSL_ENTER("SHA_Final");
        ret = wc_ShaFinal((wc_Sha*)sha, input);

        /* return 1 on success, 0 otherwise */
        if (ret == 0)
            return 1;

        return 0;
    }


    int wolfSSL_SHA1_Init(WOLFSSL_SHA_CTX* sha)
    {
        WOLFSSL_ENTER("SHA1_Init");
        return SHA_Init(sha);
    }


    int wolfSSL_SHA1_Update(WOLFSSL_SHA_CTX* sha, const void* input,
                            unsigned long sz)
    {
        WOLFSSL_ENTER("SHA1_Update");
        return SHA_Update(sha, input, sz);
    }


    int wolfSSL_SHA1_Final(byte* input, WOLFSSL_SHA_CTX* sha)
    {
        WOLFSSL_ENTER("SHA1_Final");
        return SHA_Final(input, sha);
    }
#endif /* !NO_SHA */

#ifdef WOLFSSL_SHA224

    int wolfSSL_SHA224_Init(WOLFSSL_SHA224_CTX* sha)
    {
        int ret;

        typedef char sha_test[sizeof(SHA224_CTX) >= sizeof(wc_Sha224) ? 1 : -1];
        (void)sizeof(sha_test);

        WOLFSSL_ENTER("SHA224_Init");
        ret = wc_InitSha224((wc_Sha224*)sha);

        /* return 1 on success, 0 otherwise */
        if (ret == 0)
            return 1;

        return 0;
    }


    int wolfSSL_SHA224_Update(WOLFSSL_SHA224_CTX* sha, const void* input,
                           unsigned long sz)
    {
        int ret;

        WOLFSSL_ENTER("SHA224_Update");
        ret = wc_Sha224Update((wc_Sha224*)sha, (const byte*)input, (word32)sz);

        /* return 1 on success, 0 otherwise */
        if (ret == 0)
            return 1;

        return 0;
    }


    int wolfSSL_SHA224_Final(byte* input, WOLFSSL_SHA224_CTX* sha)
    {
        int ret;

        WOLFSSL_ENTER("SHA224_Final");
        ret = wc_Sha224Final((wc_Sha224*)sha, input);

        /* return 1 on success, 0 otherwise */
        if (ret == 0)
            return 1;

        return 0;
    }

#endif /* WOLFSSL_SHA224 */


    int wolfSSL_SHA256_Init(WOLFSSL_SHA256_CTX* sha256)
    {
        int ret;

        typedef char sha_test[sizeof(SHA256_CTX) >= sizeof(wc_Sha256) ? 1 : -1];
        (void)sizeof(sha_test);

        WOLFSSL_ENTER("SHA256_Init");
        ret = wc_InitSha256((wc_Sha256*)sha256);

        /* return 1 on success, 0 otherwise */
        if (ret == 0)
            return 1;

        return 0;
    }


    int wolfSSL_SHA256_Update(WOLFSSL_SHA256_CTX* sha, const void* input,
                              unsigned long sz)
    {
        int ret;

        WOLFSSL_ENTER("SHA256_Update");
        ret = wc_Sha256Update((wc_Sha256*)sha, (const byte*)input, (word32)sz);

        /* return 1 on success, 0 otherwise */
        if (ret == 0)
            return 1;

        return 0;
    }


    int wolfSSL_SHA256_Final(byte* input, WOLFSSL_SHA256_CTX* sha)
    {
        int ret;

        WOLFSSL_ENTER("SHA256_Final");
        ret = wc_Sha256Final((wc_Sha256*)sha, input);

        /* return 1 on success, 0 otherwise */
        if (ret == 0)
            return 1;

        return 0;
    }


#ifdef WOLFSSL_SHA384

    int wolfSSL_SHA384_Init(WOLFSSL_SHA384_CTX* sha)
    {
        int ret;

        typedef char sha_test[sizeof(SHA384_CTX) >= sizeof(wc_Sha384) ? 1 : -1];
        (void)sizeof(sha_test);

        WOLFSSL_ENTER("SHA384_Init");
        ret = wc_InitSha384((wc_Sha384*)sha);

        /* return 1 on success, 0 otherwise */
        if (ret == 0)
            return 1;

        return 0;
    }


    int wolfSSL_SHA384_Update(WOLFSSL_SHA384_CTX* sha, const void* input,
                           unsigned long sz)
    {
        int ret;

        WOLFSSL_ENTER("SHA384_Update");
        ret = wc_Sha384Update((wc_Sha384*)sha, (const byte*)input, (word32)sz);

        /* return 1 on success, 0 otherwise */
        if (ret == 0)
            return 1;

        return 0;
    }


    int wolfSSL_SHA384_Final(byte* input, WOLFSSL_SHA384_CTX* sha)
    {
        int ret;

        WOLFSSL_ENTER("SHA384_Final");
        ret = wc_Sha384Final((wc_Sha384*)sha, input);

        /* return 1 on success, 0 otherwise */
        if (ret == 0)
            return 1;

        return 0;
    }

#endif /* WOLFSSL_SHA384 */


#ifdef WOLFSSL_SHA512

    int wolfSSL_SHA512_Init(WOLFSSL_SHA512_CTX* sha)
    {
        int ret;

        typedef char sha_test[sizeof(SHA512_CTX) >= sizeof(wc_Sha512) ? 1 : -1];
        (void)sizeof(sha_test);

        WOLFSSL_ENTER("SHA512_Init");
        ret = wc_InitSha512((wc_Sha512*)sha);

        /* return 1 on success, 0 otherwise */
        if (ret == 0)
            return 1;

        return 0;
    }


    int wolfSSL_SHA512_Update(WOLFSSL_SHA512_CTX* sha, const void* input,
                           unsigned long sz)
    {
        int ret;

        WOLFSSL_ENTER("SHA512_Update");
        ret = wc_Sha512Update((wc_Sha512*)sha, (const byte*)input, (word32)sz);

        /* return 1 on success, 0 otherwise */
        if (ret == 0)
            return 1;

        return 0;
    }


    int wolfSSL_SHA512_Final(byte* input, WOLFSSL_SHA512_CTX* sha)
    {
        int ret;

        WOLFSSL_ENTER("SHA512_Final");
        ret = wc_Sha512Final((wc_Sha512*)sha, input);

        /* return 1 on success, 0 otherwise */
        if (ret == 0)
            return 1;

        return 0;
    }

#endif /* WOLFSSL_SHA512 */

    static const struct s_ent {
        const unsigned char macType;
        const char *name;
    } md_tbl[] = {
    #ifndef NO_MD4
         {MD4, "MD4"},
    #endif /* NO_MD4 */

    #ifndef NO_MD5
        {WC_MD5, "MD5"},
    #endif /* NO_MD5 */

    #ifndef NO_SHA
        {WC_SHA, "SHA"},
    #endif /* NO_SHA */

    #ifdef WOLFSSL_SHA224
        {WC_SHA224, "SHA224"},
    #endif /* WOLFSSL_SHA224 */
    #ifndef NO_SHA256
        {WC_SHA256, "SHA256"},
    #endif

    #ifdef WOLFSSL_SHA384
        {WC_SHA384, "SHA384"},
    #endif /* WOLFSSL_SHA384 */
    #ifdef WOLFSSL_SHA512
        {WC_SHA512, "SHA512"},
    #endif /* WOLFSSL_SHA512 */
        {0, NULL}
    };

const WOLFSSL_EVP_MD *wolfSSL_EVP_get_digestbyname(const char *name)
{
    static const struct alias {
        const char *name;
        const char *alias;
    } alias_tbl[] =
    {
        {"MD4", "ssl3-md4"},
        {"MD5", "ssl3-md5"},
        {"SHA", "ssl3-sha1"},
        {"SHA", "SHA1"},
        { NULL, NULL}
    };

    const struct alias  *al;
    const struct s_ent *ent;

    for (al = alias_tbl; al->name != NULL; al++)
        if(XSTRNCMP(name, al->alias, XSTRLEN(al->alias)+1) == 0) {
            name = al->name;
            break;
        }

    for (ent = md_tbl; ent->name != NULL; ent++)
        if(XSTRNCMP(name, ent->name, XSTRLEN(ent->name)+1) == 0) {
            return (EVP_MD *)ent->name;
        }
    return NULL;
}

static WOLFSSL_EVP_MD *wolfSSL_EVP_get_md(const unsigned char type)
{
    const struct s_ent *ent ;
    WOLFSSL_ENTER("EVP_get_md");
    for( ent = md_tbl; ent->name != NULL; ent++){
        if(type == ent->macType) {
            return (WOLFSSL_EVP_MD *)ent->name;
        }
    }
    return (WOLFSSL_EVP_MD *)"";
}

int wolfSSL_EVP_MD_type(const WOLFSSL_EVP_MD *md)
{
    const struct s_ent *ent ;
    WOLFSSL_ENTER("EVP_MD_type");
    for( ent = md_tbl; ent->name != NULL; ent++){
        if(XSTRNCMP((const char *)md, ent->name, XSTRLEN(ent->name)+1) == 0) {
            return ent->macType;
        }
    }
    return 0;
}


#ifndef NO_MD4

    /* return a pointer to MD4 EVP type */
    const WOLFSSL_EVP_MD* wolfSSL_EVP_md4(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_md4");
        return EVP_get_digestbyname("MD4");
    }

#endif /* !NO_MD4 */


#ifndef NO_MD5

    const WOLFSSL_EVP_MD* wolfSSL_EVP_md5(void)
    {
        WOLFSSL_ENTER("EVP_md5");
        return EVP_get_digestbyname("MD5");
    }

#endif /* !NO_MD5 */


#ifndef NO_SHA
    const WOLFSSL_EVP_MD* wolfSSL_EVP_sha1(void)
    {
        WOLFSSL_ENTER("EVP_sha1");
        return EVP_get_digestbyname("SHA");
    }
#endif /* NO_SHA */

#ifdef WOLFSSL_SHA224

    const WOLFSSL_EVP_MD* wolfSSL_EVP_sha224(void)
    {
        WOLFSSL_ENTER("EVP_sha224");
        return EVP_get_digestbyname("SHA224");
    }

#endif /* WOLFSSL_SHA224 */


    const WOLFSSL_EVP_MD* wolfSSL_EVP_sha256(void)
    {
        WOLFSSL_ENTER("EVP_sha256");
        return EVP_get_digestbyname("SHA256");
    }

#ifdef WOLFSSL_SHA384

    const WOLFSSL_EVP_MD* wolfSSL_EVP_sha384(void)
    {
        WOLFSSL_ENTER("EVP_sha384");
        return EVP_get_digestbyname("SHA384");
    }

#endif /* WOLFSSL_SHA384 */

#ifdef WOLFSSL_SHA512

    const WOLFSSL_EVP_MD* wolfSSL_EVP_sha512(void)
    {
        WOLFSSL_ENTER("EVP_sha512");
        return EVP_get_digestbyname("SHA512");
    }

#endif /* WOLFSSL_SHA512 */


    WOLFSSL_EVP_MD_CTX *wolfSSL_EVP_MD_CTX_new(void)
    {
        WOLFSSL_EVP_MD_CTX* ctx;
        WOLFSSL_ENTER("EVP_MD_CTX_new");
        ctx = (WOLFSSL_EVP_MD_CTX*)XMALLOC(sizeof *ctx, NULL,
                                                       DYNAMIC_TYPE_OPENSSL);
        if (ctx){
            wolfSSL_EVP_MD_CTX_init(ctx);
        }
        return ctx;
    }

    WOLFSSL_API void wolfSSL_EVP_MD_CTX_free(WOLFSSL_EVP_MD_CTX *ctx)
    {
        if (ctx) {
            WOLFSSL_ENTER("EVP_MD_CTX_free");
                wolfSSL_EVP_MD_CTX_cleanup(ctx);
                XFREE(ctx, NULL, DYNAMIC_TYPE_OPENSSL);
            }
    }


    /* returns the type of message digest used by the ctx */
    int wolfSSL_EVP_MD_CTX_type(const WOLFSSL_EVP_MD_CTX *ctx) {
        WOLFSSL_ENTER("EVP_MD_CTX_type");
        return ctx->macType;
    }


    /* returns WOLFSSL_SUCCESS on success */
    int wolfSSL_EVP_MD_CTX_copy(WOLFSSL_EVP_MD_CTX *out, const WOLFSSL_EVP_MD_CTX *in)
    {
        return wolfSSL_EVP_MD_CTX_copy_ex(out, in);
    }


    /* copies structure in to the structure out
     *
     * returns WOLFSSL_SUCCESS on success */
    int wolfSSL_EVP_MD_CTX_copy_ex(WOLFSSL_EVP_MD_CTX *out, const WOLFSSL_EVP_MD_CTX *in)
    {
        if ((out == NULL) || (in == NULL)) return WOLFSSL_FAILURE;
        WOLFSSL_ENTER("EVP_CIPHER_MD_CTX_copy_ex");
        XMEMCPY(out, in, sizeof(WOLFSSL_EVP_MD_CTX));
        return WOLFSSL_SUCCESS;
    }

    void wolfSSL_EVP_MD_CTX_init(WOLFSSL_EVP_MD_CTX* ctx)
    {
        WOLFSSL_ENTER("EVP_CIPHER_MD_CTX_init");
        XMEMSET(ctx, 0, sizeof(WOLFSSL_EVP_MD_CTX));
    }

    const WOLFSSL_EVP_MD *wolfSSL_EVP_MD_CTX_md(const WOLFSSL_EVP_MD_CTX *ctx)
    {
        if (ctx == NULL)
            return NULL;
        WOLFSSL_ENTER("EVP_MD_CTX_md");
        return (const WOLFSSL_EVP_MD *)wolfSSL_EVP_get_md(ctx->macType);
    }

    #ifndef NO_AES

    #ifdef HAVE_AES_CBC
    #ifdef WOLFSSL_AES_128
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_cbc(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_128_cbc");
        if (EVP_AES_128_CBC == NULL)
            wolfSSL_EVP_init();
        return EVP_AES_128_CBC;
    }
    #endif /* WOLFSSL_AES_128 */


    #ifdef WOLFSSL_AES_192
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_cbc(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_192_cbc");
        if (EVP_AES_192_CBC == NULL)
            wolfSSL_EVP_init();
        return EVP_AES_192_CBC;
    }
    #endif /* WOLFSSL_AES_192 */


    #ifdef WOLFSSL_AES_256
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_cbc(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_256_cbc");
        if (EVP_AES_256_CBC == NULL)
            wolfSSL_EVP_init();
        return EVP_AES_256_CBC;
    }
    #endif /* WOLFSSL_AES_256 */
    #endif /* HAVE_AES_CBC */


    #ifdef WOLFSSL_AES_128
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_ctr(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_128_ctr");
        if (EVP_AES_128_CTR == NULL)
            wolfSSL_EVP_init();
        return EVP_AES_128_CTR;
    }
    #endif /* WOLFSSL_AES_2128 */


    #ifdef WOLFSSL_AES_192
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_ctr(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_192_ctr");
        if (EVP_AES_192_CTR == NULL)
            wolfSSL_EVP_init();
        return EVP_AES_192_CTR;
    }
    #endif /* WOLFSSL_AES_192 */


    #ifdef WOLFSSL_AES_256
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_ctr(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_256_ctr");
        if (EVP_AES_256_CTR == NULL)
            wolfSSL_EVP_init();
        return EVP_AES_256_CTR;
    }
    #endif /* WOLFSSL_AES_256 */

    #ifdef WOLFSSL_AES_128
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_128_ecb(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_128_ecb");
        if (EVP_AES_128_ECB == NULL)
            wolfSSL_EVP_init();
        return EVP_AES_128_ECB;
    }
    #endif /* WOLFSSL_AES_128 */


    #ifdef WOLFSSL_AES_192
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_192_ecb(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_192_ecb");
        if (EVP_AES_192_ECB == NULL)
            wolfSSL_EVP_init();
        return EVP_AES_192_ECB;
    }
    #endif /* WOLFSSL_AES_192*/


    #ifdef WOLFSSL_AES_256
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_aes_256_ecb(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_aes_256_ecb");
        if (EVP_AES_256_ECB == NULL)
            wolfSSL_EVP_init();
        return EVP_AES_256_ECB;
    }
    #endif /* WOLFSSL_AES_256 */
    #endif /* NO_AES */

#ifndef NO_DES3
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_des_cbc(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_des_cbc");
        if (EVP_DES_CBC == NULL)
            wolfSSL_EVP_init();
        return EVP_DES_CBC;
    }
#ifdef WOLFSSL_DES_ECB
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_des_ecb(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_des_ecb");
        if (EVP_DES_ECB == NULL)
            wolfSSL_EVP_init();
        return EVP_DES_ECB;
    }
#endif
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_des_ede3_cbc(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_des_ede3_cbc");
        if (EVP_DES_EDE3_CBC == NULL)
            wolfSSL_EVP_init();
        return EVP_DES_EDE3_CBC;
    }
#ifdef WOLFSSL_DES_ECB
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_des_ede3_ecb(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_des_ede3_ecb");
        if (EVP_DES_EDE3_ECB == NULL)
            wolfSSL_EVP_init();
        return EVP_DES_EDE3_ECB;
    }
#endif
#endif /* NO_DES3 */

#ifndef NO_RC4
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_rc4(void)
    {
        static const char* type = "ARC4";
        WOLFSSL_ENTER("wolfSSL_EVP_rc4");
        return type;
    }
#endif

#ifdef HAVE_IDEA
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_idea_cbc(void)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_idea_cbc");
        if (EVP_IDEA_CBC == NULL)
            wolfSSL_EVP_init();
        return EVP_IDEA_CBC;
    }
#endif
    const WOLFSSL_EVP_CIPHER* wolfSSL_EVP_enc_null(void)
    {
        static const char* type = "NULL";
        WOLFSSL_ENTER("wolfSSL_EVP_enc_null");
        return type;
    }


    int wolfSSL_EVP_MD_CTX_cleanup(WOLFSSL_EVP_MD_CTX* ctx)
    {
        WOLFSSL_ENTER("EVP_MD_CTX_cleanup");
        ForceZero(ctx, sizeof(*ctx));
        ctx->macType = 0xFF;
        return 1;
    }



    void wolfSSL_EVP_CIPHER_CTX_init(WOLFSSL_EVP_CIPHER_CTX* ctx)
    {
        WOLFSSL_ENTER("EVP_CIPHER_CTX_init");
        if (ctx) {
            ctx->cipherType = 0xff;   /* no init */
            ctx->keyLen     = 0;
            ctx->enc        = 1;      /* start in encrypt mode */
        }
    }


    /* WOLFSSL_SUCCESS on ok */
    int wolfSSL_EVP_CIPHER_CTX_cleanup(WOLFSSL_EVP_CIPHER_CTX* ctx)
    {
        WOLFSSL_ENTER("EVP_CIPHER_CTX_cleanup");
        if (ctx) {
            ctx->cipherType = 0xff;  /* no more init */
            ctx->keyLen     = 0;
        }

        return WOLFSSL_SUCCESS;
    }


    /* return WOLFSSL_SUCCESS on ok, 0 on failure to match API compatibility */
    int  wolfSSL_EVP_CipherInit(WOLFSSL_EVP_CIPHER_CTX* ctx,
                               const WOLFSSL_EVP_CIPHER* type, const byte* key,
                               const byte* iv, int enc)
    {
        int ret = 0;
        (void)key;
        (void)iv;
        (void)enc;

        WOLFSSL_ENTER("wolfSSL_EVP_CipherInit");
        if (ctx == NULL) {
            WOLFSSL_MSG("no ctx");
            return 0;   /* failure */
        }

        if (type == NULL && ctx->cipherType == WOLFSSL_EVP_CIPH_TYPE_INIT) {
            WOLFSSL_MSG("no type set");
            return 0;   /* failure */
        }
        if (ctx->cipherType == WOLFSSL_EVP_CIPH_TYPE_INIT){
            ctx->bufUsed = 0;
            ctx->lastUsed = 0;
            ctx->flags   = 0;
        }
#ifndef NO_AES
    #ifdef HAVE_AES_CBC
        #ifdef WOLFSSL_AES_128
        if (ctx->cipherType == AES_128_CBC_TYPE ||
            (type && XSTRNCMP(type, EVP_AES_128_CBC, EVP_AES_SIZE) == 0)) {
            WOLFSSL_MSG("EVP_AES_128_CBC");
            ctx->cipherType = AES_128_CBC_TYPE;
            ctx->flags     &= ~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CBC_MODE;
            ctx->keyLen     = 16;
            ctx->block_size = AES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = wc_AesSetKey(&ctx->cipher.aes, key, ctx->keyLen, iv,
                                ctx->enc ? AES_ENCRYPTION : AES_DECRYPTION);
                if (ret != 0)
                    return ret;
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0)
                    return ret;
            }
        }
        #endif /* WOLFSSL_AES_128 */
        #ifdef WOLFSSL_AES_192
        if (ctx->cipherType == AES_192_CBC_TYPE ||
                 (type && XSTRNCMP(type, EVP_AES_192_CBC, EVP_AES_SIZE) == 0)) {
            WOLFSSL_MSG("EVP_AES_192_CBC");
            ctx->cipherType = AES_192_CBC_TYPE;
            ctx->flags     &= ~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CBC_MODE;
            ctx->keyLen     = 24;
            ctx->block_size = AES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = wc_AesSetKey(&ctx->cipher.aes, key, ctx->keyLen, iv,
                                ctx->enc ? AES_ENCRYPTION : AES_DECRYPTION);
                if (ret != 0)
                    return ret;
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0)
                    return ret;
            }
        }
        #endif /* WOLFSSL_AES_192 */
        #ifdef WOLFSSL_AES_256
        if (ctx->cipherType == AES_256_CBC_TYPE ||
                 (type && XSTRNCMP(type, EVP_AES_256_CBC, EVP_AES_SIZE) == 0)) {
            WOLFSSL_MSG("EVP_AES_256_CBC");
            ctx->cipherType = AES_256_CBC_TYPE;
            ctx->flags     &= ~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CBC_MODE;
            ctx->keyLen     = 32;
            ctx->block_size = AES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = wc_AesSetKey(&ctx->cipher.aes, key, ctx->keyLen, iv,
                                ctx->enc ? AES_ENCRYPTION : AES_DECRYPTION);
                if (ret != 0){
                    WOLFSSL_MSG("wc_AesSetKey() failed");
                    return ret;
                }
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0){
                    WOLFSSL_MSG("wc_AesSetIV() failed");
                    return ret;
                }
            }
        }
        #endif /* WOLFSSL_AES_256 */
    #endif /* HAVE_AES_CBC */
#ifdef WOLFSSL_AES_COUNTER
        #ifdef WOLFSSL_AES_128
        if (ctx->cipherType == AES_128_CTR_TYPE ||
                 (type && XSTRNCMP(type, EVP_AES_128_CTR, EVP_AES_SIZE) == 0)) {
            WOLFSSL_MSG("EVP_AES_128_CTR");
            ctx->flags     &= ~WOLFSSL_EVP_CIPH_MODE;
            ctx->cipherType = AES_128_CTR_TYPE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CTR_MODE;
            ctx->keyLen     = 16;
            ctx->block_size = AES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
              ret = wc_AesSetKey(&ctx->cipher.aes, key, ctx->keyLen, iv,
                    AES_ENCRYPTION);
                if (ret != 0)
                    return ret;
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0)
                    return ret;
            }
        }
        #endif /* WOLFSSL_AES_128 */
        #ifdef WOLFSSL_AES_192
        if (ctx->cipherType == AES_192_CTR_TYPE ||
                 (type && XSTRNCMP(type, EVP_AES_192_CTR, EVP_AES_SIZE) == 0)) {
            WOLFSSL_MSG("EVP_AES_192_CTR");
            ctx->cipherType = AES_192_CTR_TYPE;
            ctx->flags     &= ~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CTR_MODE;
            ctx->keyLen     = 24;
            ctx->block_size = AES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = wc_AesSetKey(&ctx->cipher.aes, key, ctx->keyLen, iv,
                      AES_ENCRYPTION);
                if (ret != 0)
                    return ret;
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0)
                    return ret;
            }
        }
        #endif /* WOLFSSL_AES_192 */
        #ifdef WOLFSSL_AES_256
        if (ctx->cipherType == AES_256_CTR_TYPE ||
                 (type && XSTRNCMP(type, EVP_AES_256_CTR, EVP_AES_SIZE) == 0)) {
            WOLFSSL_MSG("EVP_AES_256_CTR");
            ctx->cipherType = AES_256_CTR_TYPE;
            ctx->flags     &= ~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CTR_MODE;
            ctx->keyLen     = 32;
            ctx->block_size = AES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = wc_AesSetKey(&ctx->cipher.aes, key, ctx->keyLen, iv,
                      AES_ENCRYPTION);
                if (ret != 0)
                    return ret;
            }
            if (iv && key == NULL) {
                ret = wc_AesSetIV(&ctx->cipher.aes, iv);
                if (ret != 0)
                    return ret;
            }
        }
        #endif /* WOLFSSL_AES_256 */
#endif /* WOLFSSL_AES_COUNTER */
        #ifdef WOLFSSL_AES_128
        if (ctx->cipherType == AES_128_ECB_TYPE ||
            (type && XSTRNCMP(type, EVP_AES_128_ECB, EVP_AES_SIZE) == 0)) {
            WOLFSSL_MSG("EVP_AES_128_ECB");
            ctx->cipherType = AES_128_ECB_TYPE;
            ctx->flags     &= ~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_ECB_MODE;
            ctx->keyLen     = 16;
            ctx->block_size = AES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = wc_AesSetKey(&ctx->cipher.aes, key, ctx->keyLen, NULL,
                      ctx->enc ? AES_ENCRYPTION : AES_DECRYPTION);
            }
            if (ret != 0)
                return ret;
        }
        #endif /* WOLFSSL_AES_128 */
        #ifdef WOLFSSL_AES_192
        if (ctx->cipherType == AES_192_ECB_TYPE ||
                 (type && XSTRNCMP(type, EVP_AES_192_ECB, EVP_AES_SIZE) == 0)) {
            WOLFSSL_MSG("EVP_AES_192_ECB");
            ctx->cipherType = AES_192_ECB_TYPE;
            ctx->flags     &= ~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_ECB_MODE;
            ctx->keyLen     = 24;
            ctx->block_size = AES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = wc_AesSetKey(&ctx->cipher.aes, key, ctx->keyLen, NULL,
                      ctx->enc ? AES_ENCRYPTION : AES_DECRYPTION);
            }
            if (ret != 0)
                return ret;
        }
        #endif /* WOLFSSL_AES_192 */
        #ifdef WOLFSSL_AES_256
        if (ctx->cipherType == AES_256_ECB_TYPE ||
                 (type && XSTRNCMP(type, EVP_AES_256_ECB, EVP_AES_SIZE) == 0)) {
            WOLFSSL_MSG("EVP_AES_256_ECB");
            ctx->cipherType = AES_256_ECB_TYPE;
            ctx->flags     &= ~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_ECB_MODE;
            ctx->keyLen     = 32;
            ctx->block_size = AES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
              ret = wc_AesSetKey(&ctx->cipher.aes, key, ctx->keyLen, NULL,
                    ctx->enc ? AES_ENCRYPTION : AES_DECRYPTION);
            }
            if (ret != 0)
                return ret;
        }
        #endif /* WOLFSSL_AES_256 */
#endif /* NO_AES */

#ifndef NO_DES3
        if (ctx->cipherType == DES_CBC_TYPE ||
                 (type && XSTRNCMP(type, EVP_DES_CBC, EVP_DES_SIZE) == 0)) {
            WOLFSSL_MSG("EVP_DES_CBC");
            ctx->cipherType = DES_CBC_TYPE;
            ctx->flags     &= ~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CBC_MODE;
            ctx->keyLen     = 8;
            ctx->block_size = DES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = wc_Des_SetKey(&ctx->cipher.des, key, iv,
                          ctx->enc ? DES_ENCRYPTION : DES_DECRYPTION);
                if (ret != 0)
                    return ret;
            }

            if (iv && key == NULL)
                wc_Des_SetIV(&ctx->cipher.des, iv);
        }
#ifdef WOLFSSL_DES_ECB
        else if (ctx->cipherType == DES_ECB_TYPE ||
                 (type && XSTRNCMP(type, EVP_DES_ECB, EVP_DES_SIZE) == 0)) {
            WOLFSSL_MSG("EVP_DES_ECB");
            ctx->cipherType = DES_ECB_TYPE;
            ctx->flags     &= ~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_ECB_MODE;
            ctx->keyLen     = 8;
            ctx->block_size = DES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                WOLFSSL_MSG("Des_SetKey");
                ret = wc_Des_SetKey(&ctx->cipher.des, key, NULL,
                          ctx->enc ? DES_ENCRYPTION : DES_DECRYPTION);
                if (ret != 0)
                    return ret;
            }
        }
#endif
        else if (ctx->cipherType == DES_EDE3_CBC_TYPE ||
                 (type &&
                  XSTRNCMP(type, EVP_DES_EDE3_CBC, EVP_DES_EDE3_SIZE) == 0)) {
            WOLFSSL_MSG("EVP_DES_EDE3_CBC");
            ctx->cipherType = DES_EDE3_CBC_TYPE;
            ctx->flags     &= ~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CBC_MODE;
            ctx->keyLen     = 24;
            ctx->block_size = DES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = wc_Des3_SetKey(&ctx->cipher.des3, key, iv,
                          ctx->enc ? DES_ENCRYPTION : DES_DECRYPTION);
                if (ret != 0)
                    return ret;
            }

            if (iv && key == NULL) {
                ret = wc_Des3_SetIV(&ctx->cipher.des3, iv);
                if (ret != 0)
                    return ret;
            }
        }
        else if (ctx->cipherType == DES_EDE3_ECB_TYPE ||
                 (type &&
                  XSTRNCMP(type, EVP_DES_EDE3_ECB, EVP_DES_EDE3_SIZE) == 0)) {
            WOLFSSL_MSG("EVP_DES_EDE3_ECB");
            ctx->cipherType = DES_EDE3_ECB_TYPE;
            ctx->flags     &= ~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_ECB_MODE;
            ctx->keyLen     = 24;
            ctx->block_size = DES_BLOCK_SIZE;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = wc_Des3_SetKey(&ctx->cipher.des3, key, NULL,
                          ctx->enc ? DES_ENCRYPTION : DES_DECRYPTION);
                if (ret != 0)
                    return ret;
            }
        }
#endif /* NO_DES3 */
#ifndef NO_RC4
        if (ctx->cipherType == ARC4_TYPE || (type &&
                                     XSTRNCMP(type, "ARC4", 4) == 0)) {
            WOLFSSL_MSG("ARC4");
            ctx->cipherType = ARC4_TYPE;
            ctx->flags     &= ~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_STREAM_CIPHER;
            ctx->block_size = 1;
            if (ctx->keyLen == 0)  /* user may have already set */
                ctx->keyLen = 16;  /* default to 128 */
            if (key)
                wc_Arc4SetKey(&ctx->cipher.arc4, key, ctx->keyLen);
        }
#endif /* NO_RC4 */
#ifdef HAVE_IDEA
        if (ctx->cipherType == IDEA_CBC_TYPE ||
                 (type && XSTRNCMP(type, EVP_IDEA_CBC, EVP_IDEA_SIZE) == 0)) {
            WOLFSSL_MSG("EVP_IDEA_CBC");
            ctx->cipherType = IDEA_CBC_TYPE;
            ctx->flags     &= ~WOLFSSL_EVP_CIPH_MODE;
            ctx->flags     |= WOLFSSL_EVP_CIPH_CBC_MODE;
            ctx->keyLen     = IDEA_KEY_SIZE;
            ctx->block_size = 8;
            if (enc == 0 || enc == 1)
                ctx->enc = enc ? 1 : 0;
            if (key) {
                ret = wc_IdeaSetKey(&ctx->cipher.idea, key, (word16)ctx->keyLen,
                                    iv, ctx->enc ? IDEA_ENCRYPTION :
                                                   IDEA_DECRYPTION);
                if (ret != 0)
                    return ret;
            }

            if (iv && key == NULL)
                wc_IdeaSetIV(&ctx->cipher.idea, iv);
        }
#endif /* HAVE_IDEA */
        if (ctx->cipherType == NULL_CIPHER_TYPE || (type &&
                                     XSTRNCMP(type, "NULL", 4) == 0)) {
            WOLFSSL_MSG("NULL cipher");
            ctx->cipherType = NULL_CIPHER_TYPE;
            ctx->keyLen = 0;
            ctx->block_size = 16;
        }
        (void)ret; /* remove warning. If execution reaches this point, ret=0 */
        return WOLFSSL_SUCCESS;
    }


    /* WOLFSSL_SUCCESS on ok */
    int wolfSSL_EVP_CIPHER_CTX_key_length(WOLFSSL_EVP_CIPHER_CTX* ctx)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_CIPHER_CTX_key_length");
        if (ctx)
            return ctx->keyLen;

        return 0;   /* failure */
    }


    /* WOLFSSL_SUCCESS on ok */
    int wolfSSL_EVP_CIPHER_CTX_set_key_length(WOLFSSL_EVP_CIPHER_CTX* ctx,
                                             int keylen)
    {
        WOLFSSL_ENTER("wolfSSL_EVP_CIPHER_CTX_set_key_length");
        if (ctx)
            ctx->keyLen = keylen;
        else
            return 0;  /* failure */

        return WOLFSSL_SUCCESS;
    }


    /* WOLFSSL_SUCCESS on ok */
    int wolfSSL_EVP_Cipher(WOLFSSL_EVP_CIPHER_CTX* ctx, byte* dst, byte* src,
                          word32 len)
    {
        int ret = 0;
        WOLFSSL_ENTER("wolfSSL_EVP_Cipher");

        if (ctx == NULL || dst == NULL || src == NULL) {
            WOLFSSL_MSG("Bad function argument");
            return 0;  /* failure */
        }

        if (ctx->cipherType == 0xff) {
            WOLFSSL_MSG("no init");
            return 0;  /* failure */
        }

        switch (ctx->cipherType) {

#ifndef NO_AES
#ifdef HAVE_AES_CBC
            case AES_128_CBC_TYPE :
            case AES_192_CBC_TYPE :
            case AES_256_CBC_TYPE :
                WOLFSSL_MSG("AES CBC");
                if (ctx->enc)
                    ret = wc_AesCbcEncrypt(&ctx->cipher.aes, dst, src, len);
                else
                    ret = wc_AesCbcDecrypt(&ctx->cipher.aes, dst, src, len);
                break;
#endif /* HAVE_AES_CBC */
#ifdef HAVE_AES_ECB
            case AES_128_ECB_TYPE :
            case AES_192_ECB_TYPE :
            case AES_256_ECB_TYPE :
                WOLFSSL_MSG("AES ECB");
                if (ctx->enc)
                    ret = wc_AesEcbEncrypt(&ctx->cipher.aes, dst, src, len);
                else
                    ret = wc_AesEcbDecrypt(&ctx->cipher.aes, dst, src, len);
                break;
#endif
#ifdef WOLFSSL_AES_COUNTER
            case AES_128_CTR_TYPE :
            case AES_192_CTR_TYPE :
            case AES_256_CTR_TYPE :
                    WOLFSSL_MSG("AES CTR");
                    ret = wc_AesCtrEncrypt(&ctx->cipher.aes, dst, src, len);
                break;
#endif /* WOLFSSL_AES_COUNTER */
#endif /* NO_AES */

#ifndef NO_DES3
            case DES_CBC_TYPE :
                if (ctx->enc)
                    wc_Des_CbcEncrypt(&ctx->cipher.des, dst, src, len);
                else
                    wc_Des_CbcDecrypt(&ctx->cipher.des, dst, src, len);
                break;
            case DES_EDE3_CBC_TYPE :
                if (ctx->enc)
                    ret = wc_Des3_CbcEncrypt(&ctx->cipher.des3, dst, src, len);
                else
                    ret = wc_Des3_CbcDecrypt(&ctx->cipher.des3, dst, src, len);
                break;
#ifdef WOLFSSL_DES_ECB
            case DES_ECB_TYPE :
                ret = wc_Des_EcbEncrypt(&ctx->cipher.des, dst, src, len);
                break;
            case DES_EDE3_ECB_TYPE :
                ret = wc_Des3_EcbEncrypt(&ctx->cipher.des3, dst, src, len);
                break;
#endif
#endif /* !NO_DES3 */

#ifndef NO_RC4
            case ARC4_TYPE :
                wc_Arc4Process(&ctx->cipher.arc4, dst, src, len);
                break;
#endif

#ifdef HAVE_IDEA
            case IDEA_CBC_TYPE :
                if (ctx->enc)
                    wc_IdeaCbcEncrypt(&ctx->cipher.idea, dst, src, len);
                else
                    wc_IdeaCbcDecrypt(&ctx->cipher.idea, dst, src, len);
                break;
#endif
            case NULL_CIPHER_TYPE :
                XMEMCPY(dst, src, len);
                break;

            default: {
                WOLFSSL_MSG("bad type");
                return 0;  /* failure */
            }
        }

        if (ret != 0) {
            WOLFSSL_MSG("wolfSSL_EVP_Cipher failure");
            return 0;  /* failure */
        }

        WOLFSSL_MSG("wolfSSL_EVP_Cipher success");
        return WOLFSSL_SUCCESS;  /* success */
    }

#define WOLFSSL_EVP_INCLUDED
#include "wolfcrypt/src/evp.c"


    /* store for external read of iv, WOLFSSL_SUCCESS on success */
    int  wolfSSL_StoreExternalIV(WOLFSSL_EVP_CIPHER_CTX* ctx)
    {
        WOLFSSL_ENTER("wolfSSL_StoreExternalIV");

        if (ctx == NULL) {
            WOLFSSL_MSG("Bad function argument");
            return WOLFSSL_FATAL_ERROR;
        }

        switch (ctx->cipherType) {

#ifndef NO_AES
            case AES_128_CBC_TYPE :
            case AES_192_CBC_TYPE :
            case AES_256_CBC_TYPE :
                WOLFSSL_MSG("AES CBC");
                XMEMCPY(ctx->iv, &ctx->cipher.aes.reg, AES_BLOCK_SIZE);
                break;

#ifdef WOLFSSL_AES_COUNTER
            case AES_128_CTR_TYPE :
            case AES_192_CTR_TYPE :
            case AES_256_CTR_TYPE :
                WOLFSSL_MSG("AES CTR");
                XMEMCPY(ctx->iv, &ctx->cipher.aes.reg, AES_BLOCK_SIZE);
                break;
#endif /* WOLFSSL_AES_COUNTER */

#endif /* NO_AES */

#ifndef NO_DES3
            case DES_CBC_TYPE :
                WOLFSSL_MSG("DES CBC");
                XMEMCPY(ctx->iv, &ctx->cipher.des.reg, DES_BLOCK_SIZE);
                break;

            case DES_EDE3_CBC_TYPE :
                WOLFSSL_MSG("DES EDE3 CBC");
                XMEMCPY(ctx->iv, &ctx->cipher.des3.reg, DES_BLOCK_SIZE);
                break;
#endif

#ifdef HAVE_IDEA
            case IDEA_CBC_TYPE :
                WOLFSSL_MSG("IDEA CBC");
                XMEMCPY(ctx->iv, &ctx->cipher.idea.reg, IDEA_BLOCK_SIZE);
                break;
#endif
            case ARC4_TYPE :
                WOLFSSL_MSG("ARC4");
                break;

            case NULL_CIPHER_TYPE :
                WOLFSSL_MSG("NULL");
                break;

            default: {
                WOLFSSL_MSG("bad type");
                return WOLFSSL_FATAL_ERROR;
            }
        }
        return WOLFSSL_SUCCESS;
    }


    /* set internal IV from external, WOLFSSL_SUCCESS on success */
    int  wolfSSL_SetInternalIV(WOLFSSL_EVP_CIPHER_CTX* ctx)
    {

        WOLFSSL_ENTER("wolfSSL_SetInternalIV");

        if (ctx == NULL) {
            WOLFSSL_MSG("Bad function argument");
            return WOLFSSL_FATAL_ERROR;
        }

        switch (ctx->cipherType) {

#ifndef NO_AES
            case AES_128_CBC_TYPE :
            case AES_192_CBC_TYPE :
            case AES_256_CBC_TYPE :
                WOLFSSL_MSG("AES CBC");
                XMEMCPY(&ctx->cipher.aes.reg, ctx->iv, AES_BLOCK_SIZE);
                break;

#ifdef WOLFSSL_AES_COUNTER
            case AES_128_CTR_TYPE :
            case AES_192_CTR_TYPE :
            case AES_256_CTR_TYPE :
                WOLFSSL_MSG("AES CTR");
                XMEMCPY(&ctx->cipher.aes.reg, ctx->iv, AES_BLOCK_SIZE);
                break;
#endif

#endif /* NO_AES */

#ifndef NO_DES3
            case DES_CBC_TYPE :
                WOLFSSL_MSG("DES CBC");
                XMEMCPY(&ctx->cipher.des.reg, ctx->iv, DES_BLOCK_SIZE);
                break;

            case DES_EDE3_CBC_TYPE :
                WOLFSSL_MSG("DES EDE3 CBC");
                XMEMCPY(&ctx->cipher.des3.reg, ctx->iv, DES_BLOCK_SIZE);
                break;
#endif

#ifdef HAVE_IDEA
            case IDEA_CBC_TYPE :
                WOLFSSL_MSG("IDEA CBC");
                XMEMCPY(&ctx->cipher.idea.reg, ctx->iv, IDEA_BLOCK_SIZE);
                break;
#endif
            case ARC4_TYPE :
                WOLFSSL_MSG("ARC4");
                break;

            case NULL_CIPHER_TYPE :
                WOLFSSL_MSG("NULL");
                break;

            default: {
                WOLFSSL_MSG("bad type");
                return WOLFSSL_FATAL_ERROR;
            }
        }
        return WOLFSSL_SUCCESS;
    }


    /* WOLFSSL_SUCCESS on ok */
    int wolfSSL_EVP_DigestInit(WOLFSSL_EVP_MD_CTX* ctx,
                               const WOLFSSL_EVP_MD* type)
    {
        int ret = WOLFSSL_SUCCESS;

        WOLFSSL_ENTER("EVP_DigestInit");

        if (ctx == NULL || type == NULL) {
            return BAD_FUNC_ARG;
        }


    #ifdef WOLFSSL_ASYNC_CRYPT
        /* compile-time validation of ASYNC_CTX_SIZE */
        typedef char async_test[WC_ASYNC_DEV_SIZE >= sizeof(WC_ASYNC_DEV) ?
                                                                        1 : -1];
        (void)sizeof(async_test);
    #endif

        if (XSTRNCMP(type, "SHA256", 6) == 0) {
             ctx->macType = WC_SHA256;
             ret = wolfSSL_SHA256_Init(&(ctx->hash.digest.sha256));
        }
    #ifdef WOLFSSL_SHA224
        else if (XSTRNCMP(type, "SHA224", 6) == 0) {
             ctx->macType = WC_SHA224;
             ret = wolfSSL_SHA224_Init(&(ctx->hash.digest.sha224));
        }
    #endif
    #ifdef WOLFSSL_SHA384
        else if (XSTRNCMP(type, "SHA384", 6) == 0) {
             ctx->macType = WC_SHA384;
             ret = wolfSSL_SHA384_Init(&(ctx->hash.digest.sha384));
        }
    #endif
    #ifdef WOLFSSL_SHA512
        else if (XSTRNCMP(type, "SHA512", 6) == 0) {
             ctx->macType = WC_SHA512;
             ret = wolfSSL_SHA512_Init(&(ctx->hash.digest.sha512));
        }
    #endif
    #ifndef NO_MD4
        else if (XSTRNCMP(type, "MD4", 3) == 0) {
            ctx->macType = MD4;
            wolfSSL_MD4_Init(&(ctx->hash.digest.md4));
        }
    #endif
    #ifndef NO_MD5
        else if (XSTRNCMP(type, "MD5", 3) == 0) {
            ctx->macType = WC_MD5;
            ret = wolfSSL_MD5_Init(&(ctx->hash.digest.md5));
        }
    #endif
    #ifndef NO_SHA
        /* has to be last since would pick or 224, 256, 384, or 512 too */
        else if (XSTRNCMP(type, "SHA", 3) == 0) {
             ctx->macType = WC_SHA;
             ret = wolfSSL_SHA_Init(&(ctx->hash.digest.sha));
        }
    #endif /* NO_SHA */
        else
             return BAD_FUNC_ARG;

        return ret;
    }


    /* WOLFSSL_SUCCESS on ok, WOLFSSL_FAILURE on failure */
    int wolfSSL_EVP_DigestUpdate(WOLFSSL_EVP_MD_CTX* ctx, const void* data,
                                size_t sz)
    {
        WOLFSSL_ENTER("EVP_DigestUpdate");

        switch (ctx->macType) {
#ifndef NO_MD4
            case MD4:
                wolfSSL_MD4_Update((MD4_CTX*)&ctx->hash, data,
                                  (unsigned long)sz);
                break;
#endif
#ifndef NO_MD5
            case WC_MD5:
                wolfSSL_MD5_Update((MD5_CTX*)&ctx->hash, data,
                                  (unsigned long)sz);
                break;
#endif
#ifndef NO_SHA
            case WC_SHA:
                wolfSSL_SHA_Update((SHA_CTX*)&ctx->hash, data,
                                  (unsigned long)sz);
                break;
#endif
#ifdef WOLFSSL_SHA224
            case WC_SHA224:
                wolfSSL_SHA224_Update((SHA224_CTX*)&ctx->hash, data,
                                     (unsigned long)sz);
                break;
#endif
#ifndef NO_SHA256
            case WC_SHA256:
                wolfSSL_SHA256_Update((SHA256_CTX*)&ctx->hash, data,
                                     (unsigned long)sz);
                break;
#endif /* !NO_SHA256 */
#ifdef WOLFSSL_SHA384
            case WC_SHA384:
                wolfSSL_SHA384_Update((SHA384_CTX*)&ctx->hash, data,
                                     (unsigned long)sz);
                break;
#endif
#ifdef WOLFSSL_SHA512
            case WC_SHA512:
                wolfSSL_SHA512_Update((SHA512_CTX*)&ctx->hash, data,
                                     (unsigned long)sz);
                break;
#endif /* WOLFSSL_SHA512 */
            default:
                return WOLFSSL_FAILURE;
        }

        return WOLFSSL_SUCCESS;
    }


    /* WOLFSSL_SUCCESS on ok */
    int wolfSSL_EVP_DigestFinal(WOLFSSL_EVP_MD_CTX* ctx, unsigned char* md,
                               unsigned int* s)
    {
        WOLFSSL_ENTER("EVP_DigestFinal");
        switch (ctx->macType) {
#ifndef NO_MD4
            case MD4:
                wolfSSL_MD4_Final(md, (MD4_CTX*)&ctx->hash);
                if (s) *s = MD4_DIGEST_SIZE;
                break;
#endif
#ifndef NO_MD5
            case WC_MD5:
                wolfSSL_MD5_Final(md, (MD5_CTX*)&ctx->hash);
                if (s) *s = WC_MD5_DIGEST_SIZE;
                break;
#endif
#ifndef NO_SHA
            case WC_SHA:
                wolfSSL_SHA_Final(md, (SHA_CTX*)&ctx->hash);
                if (s) *s = WC_SHA_DIGEST_SIZE;
                break;
#endif
#ifdef WOLFSSL_SHA224
            case WC_SHA224:
                wolfSSL_SHA224_Final(md, (SHA224_CTX*)&ctx->hash);
                if (s) *s = WC_SHA224_DIGEST_SIZE;
                break;
#endif
#ifndef NO_SHA256
            case WC_SHA256:
                wolfSSL_SHA256_Final(md, (SHA256_CTX*)&ctx->hash);
                if (s) *s = WC_SHA256_DIGEST_SIZE;
                break;
#endif /* !NO_SHA256 */
#ifdef WOLFSSL_SHA384
            case WC_SHA384:
                wolfSSL_SHA384_Final(md, (SHA384_CTX*)&ctx->hash);
                if (s) *s = WC_SHA384_DIGEST_SIZE;
                break;
#endif
#ifdef WOLFSSL_SHA512
            case WC_SHA512:
                wolfSSL_SHA512_Final(md, (SHA512_CTX*)&ctx->hash);
                if (s) *s = WC_SHA512_DIGEST_SIZE;
                break;
#endif /* WOLFSSL_SHA512 */
            default:
                return WOLFSSL_FAILURE;
        }

        return WOLFSSL_SUCCESS;
    }


    /* WOLFSSL_SUCCESS on ok */
    int wolfSSL_EVP_DigestFinal_ex(WOLFSSL_EVP_MD_CTX* ctx, unsigned char* md,
                                   unsigned int* s)
    {
        WOLFSSL_ENTER("EVP_DigestFinal_ex");
        return EVP_DigestFinal(ctx, md, s);
    }


    unsigned char* wolfSSL_HMAC(const WOLFSSL_EVP_MD* evp_md, const void* key,
                                int key_len, const unsigned char* d, int n,
                                unsigned char* md, unsigned int* md_len)
    {
        int type;
        int mdlen;
        unsigned char* ret = NULL;
#ifdef WOLFSSL_SMALL_STACK
        Hmac* hmac = NULL;
#else
        Hmac  hmac[1];
#endif
        void* heap = NULL;

        WOLFSSL_ENTER("wolfSSL_HMAC");
        if (!md) {
            WOLFSSL_MSG("Static buffer not supported, pass in md buffer");
            return NULL;  /* no static buffer support */
        }

#ifndef NO_MD5
        if (XSTRNCMP(evp_md, "MD5", 3) == 0) {
            type = WC_MD5;
            mdlen = WC_MD5_DIGEST_SIZE;
        } else
#endif
#ifdef WOLFSSL_SHA224
        if (XSTRNCMP(evp_md, "SHA224", 6) == 0) {
            type = WC_SHA224;
            mdlen = WC_SHA224_DIGEST_SIZE;
        } else
#endif
#ifndef NO_SHA256
        if (XSTRNCMP(evp_md, "SHA256", 6) == 0) {
            type = WC_SHA256;
            mdlen = WC_SHA256_DIGEST_SIZE;
        } else
#endif
#ifdef WOLFSSL_SHA512
#ifdef WOLFSSL_SHA384
        if (XSTRNCMP(evp_md, "SHA384", 6) == 0) {
            type = WC_SHA384;
            mdlen = WC_SHA384_DIGEST_SIZE;
        } else
#endif
        if (XSTRNCMP(evp_md, "SHA512", 6) == 0) {
            type = WC_SHA512;
            mdlen = WC_SHA512_DIGEST_SIZE;
        } else
#endif
#ifndef NO_SHA
        if (XSTRNCMP(evp_md, "SHA", 3) == 0) {
            type = WC_SHA;
            mdlen = WC_SHA_DIGEST_SIZE;
        } else
#endif
        {
            return NULL;
        }

    #ifdef WOLFSSL_SMALL_STACK
        hmac = (Hmac*)XMALLOC(sizeof(Hmac), heap, DYNAMIC_TYPE_HMAC);
        if (hmac == NULL)
            return NULL;
    #endif

        if (wc_HmacInit(hmac, heap, INVALID_DEVID) == 0) {
            if (wc_HmacSetKey(hmac, type, (const byte*)key, key_len) == 0) {
                if (wc_HmacUpdate(hmac, d, n) == 0) {
                    if (wc_HmacFinal(hmac, md) == 0) {
                        if (md_len)
                            *md_len = mdlen;
                        ret = md;
                    }
                }
            }
            wc_HmacFree(hmac);
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(hmac, heap, DYNAMIC_TYPE_HMAC);
    #endif

        (void)evp_md;
        return ret;
    }

    void wolfSSL_ERR_clear_error(void)
    {
        WOLFSSL_ENTER("wolfSSL_ERR_clear_error");

#if defined(DEBUG_WOLFSSL) || defined(WOLFSSL_NGINX)
        wc_ClearErrorNodes();
#endif
    }


    /* frees all nodes in the current threads error queue
     *
     * id  thread id. ERR_remove_state is depriciated and id is ignored. The
     *     current threads queue will be free'd.
     */
    void wolfSSL_ERR_remove_state(unsigned long id)
    {
        WOLFSSL_ENTER("wolfSSL_ERR_remove_state");
        (void)id;
        if (wc_ERR_remove_state() != 0) {
            WOLFSSL_MSG("Error with removing the state");
        }
    }


    int wolfSSL_RAND_status(void)
    {
        return WOLFSSL_SUCCESS;  /* wolfCrypt provides enough seed internally */
    }


    #ifndef NO_WOLFSSL_STUB
    void wolfSSL_RAND_add(const void* add, int len, double entropy)
    {
        (void)add;
        (void)len;
        (void)entropy;
        WOLFSSL_STUB("RAND_add");
        /* wolfSSL seeds/adds internally, use explicit RNG if you want
           to take control */
    }
    #endif

#ifndef NO_DES3
    /* 0 on ok */
    int wolfSSL_DES_key_sched(WOLFSSL_const_DES_cblock* key,
                              WOLFSSL_DES_key_schedule* schedule)
    {
        WOLFSSL_ENTER("wolfSSL_DES_key_sched");

        if (key == NULL || schedule == NULL) {
            WOLFSSL_MSG("Null argument passed in");
        }
        else {
            XMEMCPY(schedule, key, sizeof(WOLFSSL_const_DES_cblock));
        }

        return 0;
    }


    /* intended to behave similar to Kerberos mit_des_cbc_cksum
     * return the last 4 bytes of cipher text */
    WOLFSSL_DES_LONG wolfSSL_DES_cbc_cksum(const unsigned char* in,
            WOLFSSL_DES_cblock* out, long length, WOLFSSL_DES_key_schedule* sc,
            WOLFSSL_const_DES_cblock* iv)
    {
        WOLFSSL_DES_LONG ret;
        unsigned char* tmp;
        unsigned char* data   = (unsigned char*)in;
        long           dataSz = length;
        byte dynamicFlag = 0; /* when padding the buffer created needs free'd */

        WOLFSSL_ENTER("wolfSSL_DES_cbc_cksum");

        if (in == NULL || out == NULL || sc == NULL || iv == NULL) {
            WOLFSSL_MSG("Bad argument passed in");
            return 0;
        }

        /* if input length is not a multiple of DES_BLOCK_SIZE pad with 0s */
        if (dataSz % DES_BLOCK_SIZE) {
            dataSz += DES_BLOCK_SIZE - (dataSz % DES_BLOCK_SIZE);
            data = (unsigned char*)XMALLOC(dataSz, NULL,
                                           DYNAMIC_TYPE_TMP_BUFFER);
            if (data == NULL) {
                WOLFSSL_MSG("Issue creating temporary buffer");
                return 0;
            }
            dynamicFlag = 1; /* set to free buffer at end */
            XMEMCPY(data, in, length);
            XMEMSET(data + length, 0, dataSz - length); /* padding */
        }

        tmp = (unsigned char*)XMALLOC(dataSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (tmp == NULL) {
            WOLFSSL_MSG("Issue creating temporary buffer");
            if (dynamicFlag == 1) {
                XFREE(data, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            }
            return 0;
        }

        wolfSSL_DES_cbc_encrypt(data, tmp, dataSz, sc,
                (WOLFSSL_DES_cblock*)iv, 1);
        XMEMCPY((unsigned char*)out, tmp + (dataSz - DES_BLOCK_SIZE),
                DES_BLOCK_SIZE);

        ret = (((*((unsigned char*)out + 4) & 0xFF) << 24)|
               ((*((unsigned char*)out + 5) & 0xFF) << 16)|
               ((*((unsigned char*)out + 6) & 0xFF) << 8) |
               (*((unsigned char*)out + 7) & 0xFF));

        XFREE(tmp, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (dynamicFlag == 1) {
            XFREE(data, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        }

        return ret;
    }


    void wolfSSL_DES_cbc_encrypt(const unsigned char* input,
                                 unsigned char* output, long length,
                                 WOLFSSL_DES_key_schedule* schedule,
                                 WOLFSSL_DES_cblock* ivec, int enc)
    {
        Des myDes;
        byte lastblock[DES_BLOCK_SIZE];
        int  lb_sz;
        long  blk;

        WOLFSSL_ENTER("DES_cbc_encrypt");

        /* OpenSSL compat, no ret */
        wc_Des_SetKey(&myDes, (const byte*)schedule, (const byte*)ivec, !enc);
        lb_sz = length%DES_BLOCK_SIZE;
        blk   = length/DES_BLOCK_SIZE;

        if (enc){
            wc_Des_CbcEncrypt(&myDes, output, input, (word32)blk*DES_BLOCK_SIZE);
            if(lb_sz){
                XMEMSET(lastblock, 0, DES_BLOCK_SIZE);
                XMEMCPY(lastblock, input+length-lb_sz, lb_sz);
                wc_Des_CbcEncrypt(&myDes, output+blk*DES_BLOCK_SIZE,
                    lastblock, (word32)DES_BLOCK_SIZE);
            }
        }
        else {
            wc_Des_CbcDecrypt(&myDes, output, input, (word32)blk*DES_BLOCK_SIZE);
            if(lb_sz){
                wc_Des_CbcDecrypt(&myDes, lastblock, input+length-lb_sz, (word32)DES_BLOCK_SIZE);
                XMEMCPY(output+length-lb_sz, lastblock, lb_sz);
            }
        }
    }


    /* WOLFSSL_DES_key_schedule is a unsigned char array of size 8 */
    void wolfSSL_DES_ede3_cbc_encrypt(const unsigned char* input,
                                      unsigned char* output, long sz,
                                      WOLFSSL_DES_key_schedule* ks1,
                                      WOLFSSL_DES_key_schedule* ks2,
                                      WOLFSSL_DES_key_schedule* ks3,
                                      WOLFSSL_DES_cblock* ivec, int enc)
    {
        Des3 des;
        byte key[24];/* EDE uses 24 size key */
        byte lastblock[DES_BLOCK_SIZE];
        int  lb_sz;
        long  blk;

        WOLFSSL_ENTER("wolfSSL_DES_ede3_cbc_encrypt");

        XMEMSET(key, 0, sizeof(key));
        XMEMCPY(key, *ks1, DES_BLOCK_SIZE);
        XMEMCPY(&key[DES_BLOCK_SIZE], *ks2, DES_BLOCK_SIZE);
        XMEMCPY(&key[DES_BLOCK_SIZE * 2], *ks3, DES_BLOCK_SIZE);
        lb_sz = sz%DES_BLOCK_SIZE;
        blk   = sz/DES_BLOCK_SIZE;
        if (enc) {
            wc_Des3_SetKey(&des, key, (const byte*)ivec, DES_ENCRYPTION);
            wc_Des3_CbcEncrypt(&des, output, input, (word32)blk*DES_BLOCK_SIZE);
            if(lb_sz){
                XMEMSET(lastblock, 0, DES_BLOCK_SIZE);
                XMEMCPY(lastblock, input+sz-lb_sz, lb_sz);
                wc_Des3_CbcEncrypt(&des, output+blk*DES_BLOCK_SIZE,
                    lastblock, (word32)DES_BLOCK_SIZE);
            }
        }
        else {
            wc_Des3_SetKey(&des, key, (const byte*)ivec, DES_DECRYPTION);
            wc_Des3_CbcDecrypt(&des, output, input, (word32)blk*DES_BLOCK_SIZE);
            if(lb_sz){
                wc_Des3_CbcDecrypt(&des, lastblock, input+sz-lb_sz, (word32)DES_BLOCK_SIZE);
                XMEMCPY(output+sz-lb_sz, lastblock, lb_sz);
            }
        }
    }


    /* correctly sets ivec for next call */
    void wolfSSL_DES_ncbc_encrypt(const unsigned char* input,
                     unsigned char* output, long length,
                     WOLFSSL_DES_key_schedule* schedule, WOLFSSL_DES_cblock* ivec,
                     int enc)
    {
        Des myDes;
        byte lastblock[DES_BLOCK_SIZE];
        int  lb_sz;
        long  blk;

        WOLFSSL_ENTER("DES_ncbc_encrypt");

        /* OpenSSL compat, no ret */
        wc_Des_SetKey(&myDes, (const byte*)schedule, (const byte*)ivec, !enc);
        lb_sz = length%DES_BLOCK_SIZE;
        blk   = length/DES_BLOCK_SIZE;
        if (enc){
            wc_Des_CbcEncrypt(&myDes, output, input, (word32)blk*DES_BLOCK_SIZE);
            if(lb_sz){
                XMEMSET(lastblock, 0, DES_BLOCK_SIZE);
                XMEMCPY(lastblock, input+length-lb_sz, lb_sz);
                wc_Des_CbcEncrypt(&myDes, output+blk*DES_BLOCK_SIZE,
                    lastblock, (word32)DES_BLOCK_SIZE);
            }
        } else {
            wc_Des_CbcDecrypt(&myDes, output, input, (word32)blk*DES_BLOCK_SIZE);
            if(lb_sz){
                wc_Des_CbcDecrypt(&myDes, lastblock, input+length-lb_sz, (word32)DES_BLOCK_SIZE);
                XMEMCPY(output+length-lb_sz, lastblock, lb_sz);
            }
        }

        XMEMCPY(ivec, output + length - sizeof(DES_cblock), sizeof(DES_cblock));
    }

#endif /* NO_DES3 */


    void wolfSSL_ERR_free_strings(void)
    {
        /* handled internally */
    }


    void wolfSSL_EVP_cleanup(void)
    {
        /* nothing to do here */
    }


    void wolfSSL_cleanup_all_ex_data(void)
    {
        /* nothing to do here */
    }

    int wolfSSL_clear(WOLFSSL* ssl)
    {
        if (ssl == NULL) {
            return WOLFSSL_FAILURE;
        }

        ssl->options.isClosed = 0;
        ssl->options.connReset = 0;
        ssl->options.sentNotify = 0;

        ssl->options.serverState = NULL_STATE;
        ssl->options.clientState = NULL_STATE;
        ssl->options.connectState = CONNECT_BEGIN;
        ssl->options.acceptState  = ACCEPT_BEGIN;
        ssl->options.handShakeState  = NULL_STATE;
        ssl->options.handShakeDone = 0;
        /* ssl->options.processReply = doProcessInit; */

        ssl->keys.encryptionOn = 0;
        XMEMSET(&ssl->msgsReceived, 0, sizeof(ssl->msgsReceived));

        if (ssl->hsHashes != NULL) {
#ifndef NO_OLD_TLS
#ifndef NO_MD5
            wc_InitMd5(&ssl->hsHashes->hashMd5);
#endif
#ifndef NO_SHA
            if (wc_InitSha(&ssl->hsHashes->hashSha) != 0)
                return WOLFSSL_FAILURE;
#endif
#endif
#ifndef NO_SHA256
            if (wc_InitSha256(&ssl->hsHashes->hashSha256) != 0)
                return WOLFSSL_FAILURE;
#endif
#ifdef WOLFSSL_SHA384
            if (wc_InitSha384(&ssl->hsHashes->hashSha384) != 0)
                return WOLFSSL_FAILURE;
#endif
#ifdef WOLFSSL_SHA512
            if (wc_InitSha512(&ssl->hsHashes->hashSha512) != 0)
                return WOLFSSL_FAILURE;
#endif
        }
#ifdef SESSION_CERTS
        ssl->session.chain.count = 0;
#endif
#ifdef KEEP_PEER_CERT
        FreeX509(&ssl->peerCert);
        InitX509(&ssl->peerCert, 0, ssl->heap);
#endif

        return WOLFSSL_SUCCESS;
    }

    long wolfSSL_SSL_SESSION_set_timeout(WOLFSSL_SESSION* ses, long t)
    {
        word32 tmptime;
        if (!ses || t < 0)
            return BAD_FUNC_ARG;

        tmptime = t & 0xFFFFFFFF;

        ses->timeout = tmptime;

        return WOLFSSL_SUCCESS;
    }


    long wolfSSL_CTX_set_mode(WOLFSSL_CTX* ctx, long mode)
    {
        /* WOLFSSL_MODE_ACCEPT_MOVING_WRITE_BUFFER is wolfSSL default mode */

        WOLFSSL_ENTER("SSL_CTX_set_mode");
        if (mode == SSL_MODE_ENABLE_PARTIAL_WRITE)
            ctx->partialWrite = 1;

        return mode;
    }

    #ifndef NO_WOLFSSL_STUB
    long wolfSSL_SSL_get_mode(WOLFSSL* ssl)
    {
        /* TODO: */
        (void)ssl;
        WOLFSSL_STUB("SSL_get_mode");
        return 0;
    }
    #endif

    #ifndef NO_WOLFSSL_STUB
    long wolfSSL_CTX_get_mode(WOLFSSL_CTX* ctx)
    {
        /* TODO: */
        (void)ctx;
        WOLFSSL_STUB("SSL_CTX_get_mode");
        return 0;
    }
    #endif

    #ifndef NO_WOLFSSL_STUB
    void wolfSSL_CTX_set_default_read_ahead(WOLFSSL_CTX* ctx, int m)
    {
        /* TODO: maybe? */
        (void)ctx;
        (void)m;
        WOLFSSL_STUB("SSL_CTX_set_default_read_ahead");
    }
    #endif


    /* Storing app session context id, this value is inherited by WOLFSSL
     * objects created from WOLFSSL_CTX. Any session that is imported with a
     * different session context id will be rejected.
     *
     * ctx         structure to set context in
     * sid_ctx     value of context to set
     * sid_ctx_len length of sid_ctx buffer
     *
     * Returns SSL_SUCCESS in success case and SSL_FAILURE when failing
     */
    int wolfSSL_CTX_set_session_id_context(WOLFSSL_CTX* ctx,
                                           const unsigned char* sid_ctx,
                                           unsigned int sid_ctx_len)
    {
        WOLFSSL_ENTER("SSL_CTX_set_session_id_context");

        /* No application specific context needed for wolfSSL */
        if (sid_ctx_len > ID_LEN || ctx == NULL || sid_ctx == NULL) {
            return SSL_FAILURE;
        }
        XMEMCPY(ctx->sessionCtx, sid_ctx, sid_ctx_len);
        ctx->sessionCtxSz = (byte)sid_ctx_len;

        return SSL_SUCCESS;
    }



    /* Storing app session context id. Any session that is imported with a
     * different session context id will be rejected.
     *
     * ssl  structure to set context in
     * id   value of context to set
     * len  length of sid_ctx buffer
     *
     * Returns SSL_SUCCESS in success case and SSL_FAILURE when failing
     */
    int wolfSSL_set_session_id_context(WOLFSSL* ssl, const unsigned char* id,
                                   unsigned int len)
    {
        WOLFSSL_STUB("wolfSSL_set_session_id_context");

        if (len > ID_LEN || ssl == NULL || id == NULL) {
            return SSL_FAILURE;
        }
        XMEMCPY(ssl->sessionCtx, id, len);
        ssl->sessionCtxSz = (byte)len;

        return SSL_SUCCESS;
    }


    long wolfSSL_CTX_sess_get_cache_size(WOLFSSL_CTX* ctx)
    {
        (void)ctx;
        #ifndef NO_SESSION_CACHE
            return SESSIONS_PER_ROW * SESSION_ROWS;
        #else
            return 0;
        #endif
    }


    /* returns the unsigned error value and increments the pointer into the
     * error queue.
     *
     * file  pointer to file name
     * line  gets set to line number of error when not NULL
     */
    unsigned long wolfSSL_ERR_get_error_line(const char** file, int* line)
    {
    #ifdef DEBUG_WOLFSSL
        int ret = wc_PullErrorNode(file, NULL, line);
        if (ret < 0) {
            if (ret == BAD_STATE_E) return 0; /* no errors in queue */
            WOLFSSL_MSG("Issue getting error node");
            WOLFSSL_LEAVE("wolfSSL_ERR_get_error_line", ret);
            ret = 0 - ret; /* return absolute value of error */

            /* panic and try to clear out nodes */
            wc_ClearErrorNodes();
        }
        return (unsigned long)ret;
    #else
        (void)file;
        (void)line;

        return 0;
    #endif
    }


#ifdef DEBUG_WOLFSSL
    static const char WOLFSSL_SYS_ACCEPT_T[]  = "accept";
    static const char WOLFSSL_SYS_BIND_T[]    = "bind";
    static const char WOLFSSL_SYS_CONNECT_T[] = "connect";
    static const char WOLFSSL_SYS_FOPEN_T[]   = "fopen";
    static const char WOLFSSL_SYS_FREAD_T[]   = "fread";
    static const char WOLFSSL_SYS_GETADDRINFO_T[] = "getaddrinfo";
    static const char WOLFSSL_SYS_GETSOCKOPT_T[]  = "getsockopt";
    static const char WOLFSSL_SYS_GETSOCKNAME_T[] = "getsockname";
    static const char WOLFSSL_SYS_GETHOSTBYNAME_T[] = "gethostbyname";
    static const char WOLFSSL_SYS_GETNAMEINFO_T[]   = "getnameinfo";
    static const char WOLFSSL_SYS_GETSERVBYNAME_T[] = "getservbyname";
    static const char WOLFSSL_SYS_IOCTLSOCKET_T[]   = "ioctlsocket";
    static const char WOLFSSL_SYS_LISTEN_T[]        = "listen";
    static const char WOLFSSL_SYS_OPENDIR_T[]       = "opendir";
    static const char WOLFSSL_SYS_SETSOCKOPT_T[]    = "setsockopt";
    static const char WOLFSSL_SYS_SOCKET_T[]        = "socket";

    /* switch with int mapped to function name for compatibility */
    static const char* wolfSSL_ERR_sys_func(int fun)
    {
        switch (fun) {
            case WOLFSSL_SYS_ACCEPT:      return WOLFSSL_SYS_ACCEPT_T;
            case WOLFSSL_SYS_BIND:        return WOLFSSL_SYS_BIND_T;
            case WOLFSSL_SYS_CONNECT:     return WOLFSSL_SYS_CONNECT_T;
            case WOLFSSL_SYS_FOPEN:       return WOLFSSL_SYS_FOPEN_T;
            case WOLFSSL_SYS_FREAD:       return WOLFSSL_SYS_FREAD_T;
            case WOLFSSL_SYS_GETADDRINFO: return WOLFSSL_SYS_GETADDRINFO_T;
            case WOLFSSL_SYS_GETSOCKOPT:  return WOLFSSL_SYS_GETSOCKOPT_T;
            case WOLFSSL_SYS_GETSOCKNAME: return WOLFSSL_SYS_GETSOCKNAME_T;
            case WOLFSSL_SYS_GETHOSTBYNAME: return WOLFSSL_SYS_GETHOSTBYNAME_T;
            case WOLFSSL_SYS_GETNAMEINFO: return WOLFSSL_SYS_GETNAMEINFO_T;
            case WOLFSSL_SYS_GETSERVBYNAME: return WOLFSSL_SYS_GETSERVBYNAME_T;
            case WOLFSSL_SYS_IOCTLSOCKET: return WOLFSSL_SYS_IOCTLSOCKET_T;
            case WOLFSSL_SYS_LISTEN:      return WOLFSSL_SYS_LISTEN_T;
            case WOLFSSL_SYS_OPENDIR:     return WOLFSSL_SYS_OPENDIR_T;
            case WOLFSSL_SYS_SETSOCKOPT:  return WOLFSSL_SYS_SETSOCKOPT_T;
            case WOLFSSL_SYS_SOCKET:      return WOLFSSL_SYS_SOCKET_T;
            default:
                return "NULL";
        }
    }
#endif /* DEBUG_WOLFSSL */


    /* @TODO when having an error queue this needs to push to the queue */
    void wolfSSL_ERR_put_error(int lib, int fun, int err, const char* file,
            int line)
    {
        WOLFSSL_ENTER("wolfSSL_ERR_put_error");

        #ifndef DEBUG_WOLFSSL
        (void)fun;
        (void)err;
        (void)file;
        (void)line;
        WOLFSSL_MSG("Not compiled in debug mode");
        #else
        WOLFSSL_ERROR_LINE(err, wolfSSL_ERR_sys_func(fun), (unsigned int)line,
            file, NULL);
        #endif
        (void)lib;
    }


    /* Similar to wolfSSL_ERR_get_error_line but takes in a flags argument for
     * more flexability.
     *
     * file  output pointer to file where error happened
     * line  output to line number of error
     * data  output data. Is a string if ERR_TXT_STRING flag is used
     * flags bit flag to adjust data output
     *
     * Returns the error value or 0 if no errors are in the queue
     */
    unsigned long wolfSSL_ERR_get_error_line_data(const char** file, int* line,
                                                  const char** data, int *flags)
    {
        int ret;

        WOLFSSL_STUB("wolfSSL_ERR_get_error_line_data");

        if (flags != NULL) {
            if ((*flags & ERR_TXT_STRING) == ERR_TXT_STRING) {
                ret = wc_PullErrorNode(file, data, line);
                if (ret < 0) {
                    if (ret == BAD_STATE_E) return 0; /* no errors in queue */
                    WOLFSSL_MSG("Error with pulling error node!");
                    WOLFSSL_LEAVE("wolfSSL_ERR_get_error_line_data", ret);
                    ret = 0 - ret; /* return absolute value of error */

                    /* panic and try to clear out nodes */
                    wc_ClearErrorNodes();
                }

                return (unsigned long)ret;
            }
        }

        ret = wc_PullErrorNode(file, NULL, line);
        if (ret < 0) {
            if (ret == BAD_STATE_E) return 0; /* no errors in queue */
            WOLFSSL_MSG("Error with pulling error node!");
            WOLFSSL_LEAVE("wolfSSL_ERR_get_error_line_data", ret);
            ret = 0 - ret; /* return absolute value of error */

            /* panic and try to clear out nodes */
            wc_ClearErrorNodes();
        }

        return (unsigned long)ret;
    }

#endif /* OPENSSL_EXTRA */


#ifdef KEEP_PEER_CERT
    #ifdef SESSION_CERTS
    /* Decode the X509 DER encoded certificate into a WOLFSSL_X509 object.
     *
     * x509  WOLFSSL_X509 object to decode into.
     * in    X509 DER data.
     * len   Length of the X509 DER data.
     * returns the new certificate on success, otherwise NULL.
     */
    static int DecodeToX509(WOLFSSL_X509* x509, const byte* in, int len)
    {
        int          ret;
    #ifdef WOLFSSL_SMALL_STACK
        DecodedCert* cert = NULL;
    #else
        DecodedCert  cert[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), NULL,
                                     DYNAMIC_TYPE_DCERT);
        if (cert == NULL)
            return MEMORY_E;
    #endif

        /* Create a DecodedCert object and copy fields into WOLFSSL_X509 object.
         */
        InitDecodedCert(cert, (byte*)in, len, NULL);
        if ((ret = ParseCertRelative(cert, CERT_TYPE, 0, NULL)) == 0) {
            InitX509(x509, 0, NULL);
            ret = CopyDecodedToX509(x509, cert);
            FreeDecodedCert(cert);
        }
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
    #endif

        return ret;
    }
    #endif /* SESSION_CERTS */


    WOLFSSL_X509* wolfSSL_get_peer_certificate(WOLFSSL* ssl)
    {
        WOLFSSL_ENTER("SSL_get_peer_certificate");
        if (ssl->peerCert.issuer.sz)
            return &ssl->peerCert;
#ifdef SESSION_CERTS
        else if (ssl->session.chain.count > 0) {
            if (DecodeToX509(&ssl->peerCert, ssl->session.chain.certs[0].buffer,
                    ssl->session.chain.certs[0].length) == 0) {
                return &ssl->peerCert;
            }
        }
#endif
        return 0;
    }

#endif /* KEEP_PEER_CERT */


#ifndef NO_CERTS
#if defined(KEEP_PEER_CERT) || defined(SESSION_CERTS) || \
    defined(OPENSSL_EXTRA)  || defined(OPENSSL_EXTRA_X509_SMALL)

/* user externally called free X509, if dynamic go ahead with free, otherwise
 * don't */
static void ExternalFreeX509(WOLFSSL_X509* x509)
{
    WOLFSSL_ENTER("ExternalFreeX509");
    if (x509) {
        if (x509->dynamicMemory) {
            FreeX509(x509);
            XFREE(x509, x509->heap, DYNAMIC_TYPE_X509);
        } else {
            WOLFSSL_MSG("free called on non dynamic object, not freeing");
        }
    }
}

/* Frees an external WOLFSSL_X509 structure */
void wolfSSL_X509_free(WOLFSSL_X509* x509)
{
    WOLFSSL_ENTER("wolfSSL_FreeX509");
    ExternalFreeX509(x509);
}


/* copy name into in buffer, at most sz bytes, if buffer is null will
   malloc buffer, call responsible for freeing                     */
char* wolfSSL_X509_NAME_oneline(WOLFSSL_X509_NAME* name, char* in, int sz)
{
    int copySz;

    if (name == NULL) {
        WOLFSSL_MSG("WOLFSSL_X509_NAME pointer was NULL");
        return NULL;
    }

    copySz = min(sz, name->sz);

    WOLFSSL_ENTER("wolfSSL_X509_NAME_oneline");
    if (!name->sz) return in;

    if (!in) {
    #ifdef WOLFSSL_STATIC_MEMORY
        WOLFSSL_MSG("Using static memory -- please pass in a buffer");
        return NULL;
    #else
        in = (char*)XMALLOC(name->sz, NULL, DYNAMIC_TYPE_OPENSSL);
        if (!in ) return in;
        copySz = name->sz;
    #endif
    }

    if (copySz <= 0)
        return in;

    XMEMCPY(in, name->name, copySz - 1);
    in[copySz - 1] = 0;

    return in;
}


/* Wraps wolfSSL_X509_d2i
 *
 * returns a WOLFSSL_X509 structure pointer on success and NULL on fail
 */
WOLFSSL_X509* wolfSSL_d2i_X509(WOLFSSL_X509** x509, const unsigned char** in,
        int len)
{
    return wolfSSL_X509_d2i(x509, *in, len);
}


WOLFSSL_X509* wolfSSL_X509_d2i(WOLFSSL_X509** x509, const byte* in, int len)
{
    WOLFSSL_X509 *newX509 = NULL;

    WOLFSSL_ENTER("wolfSSL_X509_d2i");

    if (in != NULL && len != 0) {
    #ifdef WOLFSSL_SMALL_STACK
        DecodedCert* cert = NULL;
    #else
        DecodedCert  cert[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), NULL,
                                     DYNAMIC_TYPE_DCERT);
        if (cert == NULL)
            return NULL;
    #endif

        InitDecodedCert(cert, (byte*)in, len, NULL);
        if (ParseCertRelative(cert, CERT_TYPE, 0, NULL) == 0) {
            newX509 = (WOLFSSL_X509*)XMALLOC(sizeof(WOLFSSL_X509), NULL,
                                             DYNAMIC_TYPE_X509);
            if (newX509 != NULL) {
                InitX509(newX509, 1, NULL);
                if (CopyDecodedToX509(newX509, cert) != 0) {
                    XFREE(newX509, NULL, DYNAMIC_TYPE_X509);
                    newX509 = NULL;
                }
            }
        }
        FreeDecodedCert(cert);
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
    #endif
    }

    if (x509 != NULL)
        *x509 = newX509;

    return newX509;
}
#endif /* KEEP_PEER_CERT || SESSION_CERTS || OPENSSL_EXTRA ||
          OPENSSL_EXTRA_X509_SMALL */

#if defined(OPENSSL_ALL) || defined(KEEP_PEER_CERT) || defined(SESSION_CERTS)
    /* return the next, if any, altname from the peer cert */
    char* wolfSSL_X509_get_next_altname(WOLFSSL_X509* cert)
    {
        char* ret = NULL;
        WOLFSSL_ENTER("wolfSSL_X509_get_next_altname");

        /* don't have any to work with */
        if (cert == NULL || cert->altNames == NULL)
            return NULL;

        /* already went through them */
        if (cert->altNamesNext == NULL)
            return NULL;

        ret = cert->altNamesNext->name;
        cert->altNamesNext = cert->altNamesNext->next;

        return ret;
    }


    int wolfSSL_X509_get_isCA(WOLFSSL_X509* x509)
    {
        int isCA = 0;

        WOLFSSL_ENTER("wolfSSL_X509_get_isCA");

        if (x509 != NULL)
            isCA = x509->isCa;

        WOLFSSL_LEAVE("wolfSSL_X509_get_isCA", isCA);

        return isCA;
    }

    int wolfSSL_X509_get_signature(WOLFSSL_X509* x509,
                                                 unsigned char* buf, int* bufSz)
    {
        WOLFSSL_ENTER("wolfSSL_X509_get_signature");
        if (x509 == NULL || bufSz == NULL || *bufSz < (int)x509->sig.length)
            return WOLFSSL_FATAL_ERROR;

        if (buf != NULL)
            XMEMCPY(buf, x509->sig.buffer, x509->sig.length);
        *bufSz = x509->sig.length;

        return WOLFSSL_SUCCESS;
    }


    /* write X509 serial number in unsigned binary to buffer
       buffer needs to be at least EXTERNAL_SERIAL_SIZE (32) for all cases
       return WOLFSSL_SUCCESS on success */
    int wolfSSL_X509_get_serial_number(WOLFSSL_X509* x509,
                                       byte* in, int* inOutSz)
    {
        WOLFSSL_ENTER("wolfSSL_X509_get_serial_number");
        if (x509 == NULL || in == NULL ||
                                   inOutSz == NULL || *inOutSz < x509->serialSz)
            return BAD_FUNC_ARG;

        XMEMCPY(in, x509->serial, x509->serialSz);
        *inOutSz = x509->serialSz;

        return WOLFSSL_SUCCESS;
    }


    const byte* wolfSSL_X509_get_der(WOLFSSL_X509* x509, int* outSz)
    {
        WOLFSSL_ENTER("wolfSSL_X509_get_der");

        if (x509 == NULL || x509->derCert == NULL || outSz == NULL)
            return NULL;

        *outSz = (int)x509->derCert->length;
        return x509->derCert->buffer;
    }


    int wolfSSL_X509_version(WOLFSSL_X509* x509)
    {
        WOLFSSL_ENTER("wolfSSL_X509_version");

        if (x509 == NULL)
            return 0;

        return x509->version;
    }


    const byte* wolfSSL_X509_notBefore(WOLFSSL_X509* x509)
    {
        WOLFSSL_ENTER("wolfSSL_X509_notBefore");

        if (x509 == NULL)
            return NULL;

        return x509->notBefore;
    }


    const byte* wolfSSL_X509_notAfter(WOLFSSL_X509* x509)
    {
        WOLFSSL_ENTER("wolfSSL_X509_notAfter");

        if (x509 == NULL)
            return NULL;

        return x509->notAfter;
    }


#ifdef WOLFSSL_SEP

/* copy oid into in buffer, at most *inOutSz bytes, if buffer is null will
   malloc buffer, call responsible for freeing. Actual size returned in
   *inOutSz. Requires inOutSz be non-null */
byte* wolfSSL_X509_get_device_type(WOLFSSL_X509* x509, byte* in, int *inOutSz)
{
    int copySz;

    WOLFSSL_ENTER("wolfSSL_X509_get_dev_type");
    if (inOutSz == NULL) return NULL;
    if (!x509->deviceTypeSz) return in;

    copySz = min(*inOutSz, x509->deviceTypeSz);

    if (!in) {
    #ifdef WOLFSSL_STATIC_MEMORY
        WOLFSSL_MSG("Using static memory -- please pass in a buffer");
        return NULL;
    #else
        in = (byte*)XMALLOC(x509->deviceTypeSz, 0, DYNAMIC_TYPE_OPENSSL);
        if (!in) return in;
        copySz = x509->deviceTypeSz;
    #endif
    }

    XMEMCPY(in, x509->deviceType, copySz);
    *inOutSz = copySz;

    return in;
}


byte* wolfSSL_X509_get_hw_type(WOLFSSL_X509* x509, byte* in, int* inOutSz)
{
    int copySz;

    WOLFSSL_ENTER("wolfSSL_X509_get_hw_type");
    if (inOutSz == NULL) return NULL;
    if (!x509->hwTypeSz) return in;

    copySz = min(*inOutSz, x509->hwTypeSz);

    if (!in) {
    #ifdef WOLFSSL_STATIC_MEMORY
        WOLFSSL_MSG("Using static memory -- please pass in a buffer");
        return NULL;
    #else
        in = (byte*)XMALLOC(x509->hwTypeSz, 0, DYNAMIC_TYPE_OPENSSL);
        if (!in) return in;
        copySz = x509->hwTypeSz;
    #endif
    }

    XMEMCPY(in, x509->hwType, copySz);
    *inOutSz = copySz;

    return in;
}


byte* wolfSSL_X509_get_hw_serial_number(WOLFSSL_X509* x509,byte* in,
                                        int* inOutSz)
{
    int copySz;

    WOLFSSL_ENTER("wolfSSL_X509_get_hw_serial_number");
    if (inOutSz == NULL) return NULL;
    if (!x509->hwTypeSz) return in;

    copySz = min(*inOutSz, x509->hwSerialNumSz);

    if (!in) {
    #ifdef WOLFSSL_STATIC_MEMORY
        WOLFSSL_MSG("Using static memory -- please pass in a buffer");
        return NULL;
    #else
        in = (byte*)XMALLOC(x509->hwSerialNumSz, 0, DYNAMIC_TYPE_OPENSSL);
        if (!in) return in;
        copySz = x509->hwSerialNumSz;
    #endif
    }

    XMEMCPY(in, x509->hwSerialNum, copySz);
    *inOutSz = copySz;

    return in;
}

#endif /* WOLFSSL_SEP */

/* require OPENSSL_EXTRA since wolfSSL_X509_free is wrapped by OPENSSL_EXTRA */
#if !defined(NO_CERTS) && defined(OPENSSL_EXTRA)
/* return 1 on success 0 on fail */
int wolfSSL_sk_X509_push(WOLF_STACK_OF(WOLFSSL_X509_NAME)* sk, WOLFSSL_X509* x509)
{
    WOLFSSL_STACK* node;

    if (sk == NULL || x509 == NULL) {
        return WOLFSSL_FAILURE;
    }

    /* no previous values in stack */
    if (sk->data.x509 == NULL) {
        sk->data.x509 = x509;
        sk->num += 1;
        return WOLFSSL_SUCCESS;
    }

    /* stack already has value(s) create a new node and add more */
    node = (WOLFSSL_STACK*)XMALLOC(sizeof(WOLFSSL_STACK), NULL,
                                                             DYNAMIC_TYPE_X509);
    if (node == NULL) {
        WOLFSSL_MSG("Memory error");
        return WOLFSSL_FAILURE;
    }
    XMEMSET(node, 0, sizeof(WOLFSSL_STACK));

    /* push new x509 onto head of stack */
    node->data.x509 = sk->data.x509;
    node->next      = sk->next;
    sk->next        = node;
    sk->data.x509   = x509;
    sk->num        += 1;

    return WOLFSSL_SUCCESS;
}


WOLFSSL_X509* wolfSSL_sk_X509_pop(WOLF_STACK_OF(WOLFSSL_X509_NAME)* sk) {
    WOLFSSL_STACK* node;
    WOLFSSL_X509*  x509;

    if (sk == NULL) {
        return NULL;
    }

    node = sk->next;
    x509 = sk->data.x509;

    if (node != NULL) { /* update sk and remove node from stack */
        sk->data.x509 = node->data.x509;
        sk->next = node->next;
        XFREE(node, NULL, DYNAMIC_TYPE_X509);
    }
    else { /* last x509 in stack */
        sk->data.x509 = NULL;
    }

    if (sk->num > 0) {
        sk->num -= 1;
    }

    return x509;
}


/* Getter function for WOLFSSL_X509_NAME pointer
 *
 * sk is the stack to retrieve pointer from
 * i  is the index value in stack
 *
 * returns a pointer to a WOLFSSL_X509_NAME structure on success and NULL on
 *         fail
 */
void* wolfSSL_sk_X509_NAME_value(const STACK_OF(WOLFSSL_X509_NAME)* sk, int i)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_NAME_value");

    for (; sk != NULL && i > 0; i--)
        sk = sk->next;

    if (i != 0 || sk == NULL)
        return NULL;
    return sk->data.name;
}


/* Getter function for WOLFSSL_X509 pointer
 *
 * sk is the stack to retrieve pointer from
 * i  is the index value in stack
 *
 * returns a pointer to a WOLFSSL_X509 structure on success and NULL on
 *         fail
 */
void* wolfSSL_sk_X509_value(STACK_OF(WOLFSSL_X509)* sk, int i)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_value");

    for (; sk != NULL && i > 0; i--)
        sk = sk->next;

    if (i != 0 || sk == NULL)
        return NULL;
    return sk->data.x509;
}


/* Free's all nodes in X509 stack. This is different then wolfSSL_sk_X509_free
 * in that it allows for choosing the function to use when freeing an X509s.
 *
 * sk  stack to free nodes in
 * f   X509 free function
 */
void wolfSSL_sk_X509_pop_free(STACK_OF(WOLFSSL_X509)* sk, void f (WOLFSSL_X509*)){
    WOLFSSL_STACK* node;

    WOLFSSL_ENTER("wolfSSL_sk_X509_pop_free");

    if (sk == NULL) {
        return;
    }

    /* parse through stack freeing each node */
    node = sk->next;
    while (sk->num > 1) {
        WOLFSSL_STACK* tmp = node;
        node = node->next;

        f(tmp->data.x509);
        XFREE(tmp, NULL, DYNAMIC_TYPE_X509);
        sk->num -= 1;
    }

    /* free head of stack */
    if (sk->num == 1) {
	    f(sk->data.x509);
    }
    XFREE(sk, NULL, DYNAMIC_TYPE_X509);
}


/* free structure for x509 stack */
void wolfSSL_sk_X509_free(WOLF_STACK_OF(WOLFSSL_X509_NAME)* sk) {
    WOLFSSL_STACK* node;

    if (sk == NULL) {
        return;
    }

    /* parse through stack freeing each node */
    node = sk->next;
    while (sk->num > 1) {
        WOLFSSL_STACK* tmp = node;
        node = node->next;

        wolfSSL_X509_free(tmp->data.x509);
        XFREE(tmp, NULL, DYNAMIC_TYPE_X509);
        sk->num -= 1;
    }

    /* free head of stack */
    if (sk->num == 1) {
    wolfSSL_X509_free(sk->data.x509);
    }
    XFREE(sk, NULL, DYNAMIC_TYPE_X509);
}

#endif /* NO_CERTS && OPENSSL_EXTRA */

#ifdef OPENSSL_EXTRA

/* Returns the general name at index i from the stack
 *
 * sk stack to get general name from
 * i  index to get
 *
 * return a pointer to the internal node of the stack
 */
WOLFSSL_ASN1_OBJECT* wolfSSL_sk_GENERAL_NAME_value(WOLFSSL_STACK* sk, int i)
{
    WOLFSSL_STACK* cur;
    int j;

    WOLFSSL_ENTER("wolfSSL_sk_GENERAL_NAME_value");

    if (i < 0 || sk == NULL) {
        return NULL;
    }

    cur = sk;
    for (j = 0; j < i && cur != NULL; j++) {
        cur = cur->next;
    }

    if (cur == NULL) {
        return NULL;
    }

    return cur->data.obj;
}


/* Gets the number of nodes in the stack
 *
 * sk  stack to get the number of nodes from
 *
 * returns the number of nodes, -1 if no nodes
 */
int wolfSSL_sk_GENERAL_NAME_num(WOLFSSL_STACK* sk)
{
    WOLFSSL_ENTER("wolfSSL_sk_GENERAL_NAME_num");

    if (sk == NULL) {
        return -1;
    }

    return (int)sk->num;
}

/* Frees all nodes in a GENERAL NAME stack
 *
 * sk stack of nodes to free
 * f  free function to use, not called with wolfSSL
 */
void wolfSSL_sk_GENERAL_NAME_pop_free(WOLFSSL_STACK* sk,
        void f (WOLFSSL_ASN1_OBJECT*))
{
    WOLFSSL_STACK* node;

    WOLFSSL_ENTER("wolfSSL_sk_GENERAL_NAME_pop_free");

    (void)f;
    if (sk == NULL) {
        return;
    }

    /* parse through stack freeing each node */
    node = sk->next;
    while (sk->num > 1) {
        WOLFSSL_STACK* tmp = node;
        node = node->next;

        wolfSSL_ASN1_OBJECT_free(tmp->data.obj);
        XFREE(tmp, NULL, DYNAMIC_TYPE_ASN1);
        sk->num -= 1;
    }

    /* free head of stack */
    if (sk->num == 1) {
	    wolfSSL_ASN1_OBJECT_free(sk->data.obj);
    }
    XFREE(sk, NULL, DYNAMIC_TYPE_ASN1);


}
#endif /* OPENSSL_EXTRA */

#ifndef NO_FILESYSTEM

#ifndef NO_STDIO_FILESYSTEM

WOLFSSL_X509* wolfSSL_X509_d2i_fp(WOLFSSL_X509** x509, XFILE file)
{
    WOLFSSL_X509* newX509 = NULL;

    WOLFSSL_ENTER("wolfSSL_X509_d2i_fp");

    if (file != XBADFILE) {
        byte* fileBuffer = NULL;
        long sz = 0;

        XFSEEK(file, 0, XSEEK_END);
        sz = XFTELL(file);
        XREWIND(file);

        if (sz < 0) {
            WOLFSSL_MSG("Bad tell on FILE");
            return NULL;
        }

        fileBuffer = (byte*)XMALLOC(sz, NULL, DYNAMIC_TYPE_FILE);
        if (fileBuffer != NULL) {
            int ret = (int)XFREAD(fileBuffer, 1, sz, file);
            if (ret == sz) {
                newX509 = wolfSSL_X509_d2i(NULL, fileBuffer, (int)sz);
            }
            XFREE(fileBuffer, NULL, DYNAMIC_TYPE_FILE);
        }
    }

    if (x509 != NULL)
        *x509 = newX509;

    return newX509;
}

#endif /* NO_STDIO_FILESYSTEM */

WOLFSSL_X509* wolfSSL_X509_load_certificate_file(const char* fname, int format)
{
#ifdef WOLFSSL_SMALL_STACK
    byte  staticBuffer[1]; /* force heap usage */
#else
    byte  staticBuffer[FILE_BUFFER_SIZE];
#endif
    byte* fileBuffer = staticBuffer;
    int   dynamic = 0;
    int   ret;
    long  sz = 0;
    XFILE file;

    WOLFSSL_X509* x509 = NULL;

    /* Check the inputs */
    if ((fname == NULL) ||
        (format != WOLFSSL_FILETYPE_ASN1 && format != WOLFSSL_FILETYPE_PEM))
        return NULL;

    file = XFOPEN(fname, "rb");
    if (file == XBADFILE)
        return NULL;

    XFSEEK(file, 0, XSEEK_END);
    sz = XFTELL(file);
    XREWIND(file);

    if (sz > (long)sizeof(staticBuffer)) {
        fileBuffer = (byte*)XMALLOC(sz, NULL, DYNAMIC_TYPE_FILE);
        if (fileBuffer == NULL) {
            XFCLOSE(file);
            return NULL;
        }
        dynamic = 1;
    }
    else if (sz < 0) {
        XFCLOSE(file);
        return NULL;
    }

    ret = (int)XFREAD(fileBuffer, 1, sz, file);
    if (ret != sz) {
        XFCLOSE(file);
        if (dynamic)
            XFREE(fileBuffer, NULL, DYNAMIC_TYPE_FILE);
        return NULL;
    }

    XFCLOSE(file);

    x509 = wolfSSL_X509_load_certificate_buffer(fileBuffer, (int)sz, format);

    if (dynamic)
        XFREE(fileBuffer, NULL, DYNAMIC_TYPE_FILE);

    return x509;
}

#endif /* NO_FILESYSTEM */


WOLFSSL_X509* wolfSSL_X509_load_certificate_buffer(
    const unsigned char* buf, int sz, int format)
{
    int ret;
    WOLFSSL_X509* x509 = NULL;
    DerBuffer* der = NULL;

    WOLFSSL_ENTER("wolfSSL_X509_load_certificate_ex");

    if (format == WOLFSSL_FILETYPE_PEM) {
    #ifdef WOLFSSL_PEM_TO_DER
        if (PemToDer(buf, sz, CERT_TYPE, &der, NULL, NULL, NULL) != 0) {
            FreeDer(&der);
        }
    #else
        ret = NOT_COMPILED_IN;
    #endif
    }
    else {
        ret = AllocDer(&der, (word32)sz, CERT_TYPE, NULL);
        if (ret == 0) {
            XMEMCPY(der->buffer, buf, sz);
        }
    }

    /* At this point we want `der` to have the certificate in DER format */
    /* ready to be decoded. */
    if (der != NULL && der->buffer != NULL) {
    #ifdef WOLFSSL_SMALL_STACK
        DecodedCert* cert = NULL;
    #else
        DecodedCert  cert[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), NULL,
                                     DYNAMIC_TYPE_DCERT);
        if (cert != NULL)
    #endif
        {
            InitDecodedCert(cert, der->buffer, der->length, NULL);
            if (ParseCertRelative(cert, CERT_TYPE, 0, NULL) == 0) {
                x509 = (WOLFSSL_X509*)XMALLOC(sizeof(WOLFSSL_X509), NULL,
                                                             DYNAMIC_TYPE_X509);
                if (x509 != NULL) {
                    InitX509(x509, 1, NULL);
                    if (CopyDecodedToX509(x509, cert) != 0) {
                        XFREE(x509, NULL, DYNAMIC_TYPE_X509);
                        x509 = NULL;
                    }
                }
            }

            FreeDecodedCert(cert);
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
        #endif
        }

        FreeDer(&der);
    }

    return x509;
}

#endif /* KEEP_PEER_CERT || SESSION_CERTS */

/* OPENSSL_EXTRA is needed for wolfSSL_X509_d21 function
   KEEP_OUR_CERT is to insure ability for returning ssl certificate */
#if defined(OPENSSL_EXTRA) && defined(KEEP_OUR_CERT)
WOLFSSL_X509* wolfSSL_get_certificate(WOLFSSL* ssl)
{
    if (ssl == NULL) {
        return NULL;
    }

    if (ssl->buffers.weOwnCert) {
        if (ssl->ourCert == NULL) {
            if (ssl->buffers.certificate == NULL) {
                WOLFSSL_MSG("Certificate buffer not set!");
                return NULL;
            }
            ssl->ourCert = wolfSSL_X509_d2i(NULL,
                                              ssl->buffers.certificate->buffer,
                                              ssl->buffers.certificate->length);
        }
        return ssl->ourCert;
    }
    else { /* if cert not owned get parent ctx cert or return null */
        if (ssl->ctx) {
            if (ssl->ctx->ourCert == NULL) {
                if (ssl->ctx->certificate == NULL) {
                    WOLFSSL_MSG("Ctx Certificate buffer not set!");
                    return NULL;
                }
                ssl->ctx->ourCert = wolfSSL_X509_d2i(NULL,
                                               ssl->ctx->certificate->buffer,
                                               ssl->ctx->certificate->length);
                ssl->ctx->ownOurCert = 1;
            }
            return ssl->ctx->ourCert;
        }
    }

    return NULL;
}
#endif /* OPENSSL_EXTRA && KEEP_OUR_CERT */
#endif /* NO_CERTS */


#ifdef OPENSSL_EXTRA
/* return 1 on success 0 on fail */
int wolfSSL_sk_ASN1_OBJECT_push(WOLF_STACK_OF(WOLFSSL_ASN1_OBJEXT)* sk,
                                                      WOLFSSL_ASN1_OBJECT* obj)
{
    WOLFSSL_STACK* node;

    if (sk == NULL || obj == NULL) {
        return WOLFSSL_FAILURE;
    }

    /* no previous values in stack */
    if (sk->data.obj == NULL) {
        sk->data.obj = obj;
        sk->num += 1;
        return WOLFSSL_SUCCESS;
    }

    /* stack already has value(s) create a new node and add more */
    node = (WOLFSSL_STACK*)XMALLOC(sizeof(WOLFSSL_STACK), NULL,
                                                             DYNAMIC_TYPE_ASN1);
    if (node == NULL) {
        WOLFSSL_MSG("Memory error");
        return WOLFSSL_FAILURE;
    }
    XMEMSET(node, 0, sizeof(WOLFSSL_STACK));

    /* push new obj onto head of stack */
    node->data.obj = sk->data.obj;
    node->next      = sk->next;
    sk->next        = node;
    sk->data.obj   = obj;
    sk->num        += 1;

    return WOLFSSL_SUCCESS;
}


WOLFSSL_ASN1_OBJECT* wolfSSL_sk_ASN1_OBJCET_pop(
                                            WOLF_STACK_OF(WOLFSSL_ASN1_OBJECT)* sk)
{
    WOLFSSL_STACK* node;
    WOLFSSL_ASN1_OBJECT*  obj;

    if (sk == NULL) {
        return NULL;
    }

    node = sk->next;
    obj = sk->data.obj;

    if (node != NULL) { /* update sk and remove node from stack */
        sk->data.obj = node->data.obj;
        sk->next = node->next;
        XFREE(node, NULL, DYNAMIC_TYPE_ASN1);
    }
    else { /* last obj in stack */
        sk->data.obj = NULL;
    }

    if (sk->num > 0) {
        sk->num -= 1;
    }

    return obj;
}


#ifndef NO_ASN
WOLFSSL_ASN1_OBJECT* wolfSSL_ASN1_OBJECT_new(void)
{
    WOLFSSL_ASN1_OBJECT* obj;

    obj = (WOLFSSL_ASN1_OBJECT*)XMALLOC(sizeof(WOLFSSL_ASN1_OBJECT), NULL,
                                        DYNAMIC_TYPE_ASN1);
    if (obj == NULL) {
        return NULL;
    }

    XMEMSET(obj, 0, sizeof(WOLFSSL_ASN1_OBJECT));
    obj->d.ia5 = &(obj->d.ia5_internal);
    return obj;
}


void wolfSSL_ASN1_OBJECT_free(WOLFSSL_ASN1_OBJECT* obj)
{
    if (obj == NULL) {
        return;
    }

    if (obj->dynamic == 1) {
        if (obj->obj != NULL) {
            WOLFSSL_MSG("Freeing ASN1 OBJECT data");
            XFREE(obj->obj, obj->heap, DYNAMIC_TYPE_ASN1);
        }
    }

    XFREE(obj, NULL, DYNAMIC_TYPE_ASN1);
}


/* free structure for x509 stack */
void wolfSSL_sk_ASN1_OBJECT_free(WOLF_STACK_OF(WOLFSSL_ASN1_OBJECT)* sk)
{
    WOLFSSL_STACK* node;

    if (sk == NULL) {
        return;
    }

    /* parse through stack freeing each node */
    node = sk->next;
    while (sk->num > 1) {
        WOLFSSL_STACK* tmp = node;
        node = node->next;

        wolfSSL_ASN1_OBJECT_free(tmp->data.obj);
        XFREE(tmp, NULL, DYNAMIC_TYPE_ASN1);
        sk->num -= 1;
    }

    /* free head of stack */
    if (sk->num == 1) {
        wolfSSL_ASN1_OBJECT_free(sk->data.obj);
    }
    XFREE(sk, NULL, DYNAMIC_TYPE_ASN1);
}

int wolfSSL_ASN1_STRING_to_UTF8(unsigned char **out, WOLFSSL_ASN1_STRING *in)
{
    /*
       ASN1_STRING_to_UTF8() converts the string in to UTF8 format,
       the converted data is allocated in a buffer in *out.
       The length of out is returned or a negative error code.
       The buffer *out should be free using OPENSSL_free().
       */
    (void)out;
    (void)in;
    WOLFSSL_STUB("ASN1_STRING_to_UTF8");
    return -1;
}
#endif /* NO_ASN */

void wolfSSL_set_connect_state(WOLFSSL* ssl)
{
    word16 haveRSA = 1;
    word16 havePSK = 0;

    if (ssl == NULL) {
        WOLFSSL_MSG("WOLFSSL struct pointer passed in was null");
        return;
    }

    #ifndef NO_DH
    /* client creates its own DH parameters on handshake */
    if (ssl->buffers.serverDH_P.buffer && ssl->buffers.weOwnDH) {
        XFREE(ssl->buffers.serverDH_P.buffer, ssl->heap,
            DYNAMIC_TYPE_PUBLIC_KEY);
    }
    ssl->buffers.serverDH_P.buffer = NULL;
    if (ssl->buffers.serverDH_G.buffer && ssl->buffers.weOwnDH) {
        XFREE(ssl->buffers.serverDH_G.buffer, ssl->heap,
            DYNAMIC_TYPE_PUBLIC_KEY);
    }
    ssl->buffers.serverDH_G.buffer = NULL;
    #endif

    if (ssl->options.side == WOLFSSL_SERVER_END) {
        #ifdef NO_RSA
            haveRSA = 0;
        #endif
        #ifndef NO_PSK
            havePSK = ssl->options.havePSK;
        #endif
        InitSuites(ssl->suites, ssl->version, ssl->buffers.keySz, haveRSA,
                   havePSK, ssl->options.haveDH, ssl->options.haveNTRU,
                   ssl->options.haveECDSAsig, ssl->options.haveECC,
                   ssl->options.haveStaticECC, WOLFSSL_CLIENT_END);
    }
    ssl->options.side = WOLFSSL_CLIENT_END;
}
#endif /* OPENSSL_EXTRA || WOLFSSL_EXTRA */


int wolfSSL_get_shutdown(const WOLFSSL* ssl)
{
    WOLFSSL_ENTER("wolfSSL_get_shutdown");
    /* in OpenSSL, WOLFSSL_SENT_SHUTDOWN = 1, when closeNotifySent   *
     * WOLFSSL_RECEIVED_SHUTDOWN = 2, from close notify or fatal err */
    return ((ssl->options.closeNotify||ssl->options.connReset) << 1)
            | (ssl->options.sentNotify);
}


int wolfSSL_session_reused(WOLFSSL* ssl)
{
    return ssl->options.resuming;
}

#if defined(OPENSSL_EXTRA) || defined(HAVE_EXT_CACHE)
void wolfSSL_SESSION_free(WOLFSSL_SESSION* session)
{
    if (session == NULL)
        return;

#ifdef HAVE_EXT_CACHE
    if (session->isAlloced) {
    #ifdef HAVE_SESSION_TICKET
        if (session->isDynamic)
            XFREE(session->ticket, NULL, DYNAMIC_TYPE_SESSION_TICK);
    #endif
        XFREE(session, NULL, DYNAMIC_TYPE_OPENSSL);
    }
#else
    /* No need to free since cache is static */
    (void)session;
#endif
}
#endif

const char* wolfSSL_get_version(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("SSL_get_version");
    if (ssl->version.major == SSLv3_MAJOR) {
        switch (ssl->version.minor) {
        #ifndef NO_OLD_TLS
            #ifdef WOLFSSL_ALLOW_SSLV3
            case SSLv3_MINOR :
                return "SSLv3";
            #endif
            #ifdef WOLFSSL_ALLOW_TLSV10
            case TLSv1_MINOR :
                return "TLSv1";
            #endif
            case TLSv1_1_MINOR :
                return "TLSv1.1";
        #endif
            case TLSv1_2_MINOR :
                return "TLSv1.2";
        #ifdef WOLFSSL_TLS13
            case TLSv1_3_MINOR :
            /* TODO: [TLS13] Remove draft versions. */
            #ifndef WOLFSSL_TLS13_FINAL
                #ifdef WOLFSSL_TLS13_DRAFT_18
                    return "TLSv1.3 (Draft 18)";
                #elif defined(WOLFSSL_TLS13_DRAFT_22)
                    return "TLSv1.3 (Draft 22)";
                #elif defined(WOLFSSL_TLS13_DRAFT_23)
                    return "TLSv1.3 (Draft 23)";
                #elif defined(WOLFSSL_TLS13_DRAFT_26)
                    return "TLSv1.3 (Draft 26)";
                #else
                    return "TLSv1.3 (Draft 28)";
                #endif
            #else
                return "TLSv1.3";
            #endif
        #endif
            default:
                return "unknown";
        }
    }
#ifdef WOLFSSL_DTLS
    else if (ssl->version.major == DTLS_MAJOR) {
        switch (ssl->version.minor) {
            case DTLS_MINOR :
                return "DTLS";
            case DTLSv1_2_MINOR :
                return "DTLSv1.2";
            default:
                return "unknown";
        }
    }
#endif /* WOLFSSL_DTLS */
    return "unknown";
}


/* current library version */
const char* wolfSSL_lib_version(void)
{
    return LIBWOLFSSL_VERSION_STRING;
}


/* current library version in hex */
word32 wolfSSL_lib_version_hex(void)
{
    return LIBWOLFSSL_VERSION_HEX;
}


int wolfSSL_get_current_cipher_suite(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("SSL_get_current_cipher_suite");
    if (ssl)
        return (ssl->options.cipherSuite0 << 8) | ssl->options.cipherSuite;
    return 0;
}

WOLFSSL_CIPHER* wolfSSL_get_current_cipher(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("SSL_get_current_cipher");
    if (ssl)
        return &ssl->cipher;
    else
        return NULL;
}


const char* wolfSSL_CIPHER_get_name(const WOLFSSL_CIPHER* cipher)
{
    WOLFSSL_ENTER("SSL_CIPHER_get_name");

    if (cipher == NULL || cipher->ssl == NULL) {
        return NULL;
    }

    return wolfSSL_get_cipher_name_iana(cipher->ssl);
}

const char* wolfSSL_SESSION_CIPHER_get_name(WOLFSSL_SESSION* session)
{
    if (session == NULL) {
        return NULL;
    }

#ifdef SESSION_CERTS
    return GetCipherNameIana(session->cipherSuite0, session->cipherSuite);
#else
    return NULL;
#endif
}

const char* wolfSSL_get_cipher(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("wolfSSL_get_cipher");
    return wolfSSL_CIPHER_get_name(wolfSSL_get_current_cipher(ssl));
}

/* gets cipher name in the format DHE-RSA-... rather then TLS_DHE... */
const char* wolfSSL_get_cipher_name(WOLFSSL* ssl)
{
    /* get access to cipher_name_idx in internal.c */
    return wolfSSL_get_cipher_name_internal(ssl);
}

const char* wolfSSL_get_cipher_name_from_suite(const byte cipherSuite0,
    const byte cipherSuite)
{
    return GetCipherNameInternal(cipherSuite0, cipherSuite);
}


#ifdef HAVE_ECC
/* Return the name of the curve used for key exchange as a printable string.
 *
 * ssl  The SSL/TLS object.
 * returns NULL if ECDH was not used, otherwise the name as a string.
 */
const char* wolfSSL_get_curve_name(WOLFSSL* ssl)
{
    if (ssl == NULL)
        return NULL;
    if (!IsAtLeastTLSv1_3(ssl->version) && ssl->specs.kea != ecdhe_psk_kea &&
            ssl->specs.kea != ecc_diffie_hellman_kea)
        return NULL;
    if (ssl->ecdhCurveOID == 0)
        return NULL;
    if (ssl->ecdhCurveOID == ECC_X25519_OID)
        return "X25519";
    return wc_ecc_get_name(wc_ecc_get_oid(ssl->ecdhCurveOID, NULL, NULL));
}
#endif


#if defined(OPENSSL_EXTRA_X509_SMALL) || defined(KEEP_PEER_CERT) || \
    defined(SESSION_CERTS)
/* Smaller subset of X509 compatibility functions. Avoid increasing the size of
 * this subset and its memory usage */

#if !defined(NO_CERTS)
/* returns a pointer to a new WOLFSSL_X509 structure on success and NULL on
 * fail
 */
WOLFSSL_X509* wolfSSL_X509_new()
{
    WOLFSSL_X509* x509;

    x509 = (WOLFSSL_X509*)XMALLOC(sizeof(WOLFSSL_X509), NULL,
            DYNAMIC_TYPE_X509);
    if (x509 != NULL) {
        InitX509(x509, 1, NULL);
    }

    return x509;
}

WOLFSSL_X509_NAME* wolfSSL_X509_get_subject_name(WOLFSSL_X509* cert)
{
    WOLFSSL_ENTER("wolfSSL_X509_get_subject_name");
    if (cert && cert->subject.sz != 0)
        return &cert->subject;
    return NULL;
}



WOLFSSL_X509_NAME* wolfSSL_X509_get_issuer_name(WOLFSSL_X509* cert)
{
    WOLFSSL_ENTER("X509_get_issuer_name");
    if (cert && cert->issuer.sz != 0)
        return &cert->issuer;
    return NULL;
}


int wolfSSL_X509_get_signature_type(WOLFSSL_X509* x509)
{
    int type = 0;

    WOLFSSL_ENTER("wolfSSL_X509_get_signature_type");

    if (x509 != NULL)
        type = x509->sigOID;

    return type;
}

#if defined(OPENSSL_EXTRA_X509_SMALL)
#ifdef HAVE_ECC
    static int SetECKeyExternal(WOLFSSL_EC_KEY* eckey);
#endif

/* Used to get a string from the WOLFSSL_X509_NAME structure that
 * corresponds with the NID value passed in.
 *
 * name structure to get string from
 * nid  NID value to search for
 * buf  [out] buffer to hold results. If NULL then the buffer size minus the
 *      null char is returned.
 * len  size of "buf" passed in
 *
 * returns the length of string found, not including the NULL terminator.
 *         It's possible the function could return a negative value in the
 *         case that len is less than or equal to 0. A negative value is
 *         considered an error case.
 */
int wolfSSL_X509_NAME_get_text_by_NID(WOLFSSL_X509_NAME* name,
                                      int nid, char* buf, int len)
{
    char *text = NULL;
    int textSz = 0;

    WOLFSSL_ENTER("wolfSSL_X509_NAME_get_text_by_NID");

    switch (nid) {
        case ASN_COMMON_NAME:
            text = name->fullName.fullName + name->fullName.cnIdx;
            textSz = name->fullName.cnLen;
            break;
        case ASN_SUR_NAME:
            text = name->fullName.fullName + name->fullName.snIdx;
            textSz = name->fullName.snLen;
            break;
        case ASN_SERIAL_NUMBER:
            text = name->fullName.fullName + name->fullName.serialIdx;
            textSz = name->fullName.serialLen;
            break;
        case ASN_COUNTRY_NAME:
            text = name->fullName.fullName + name->fullName.cIdx;
            textSz = name->fullName.cLen;
            break;
        case ASN_LOCALITY_NAME:
            text = name->fullName.fullName + name->fullName.lIdx;
            textSz = name->fullName.lLen;
            break;
        case ASN_STATE_NAME:
            text = name->fullName.fullName + name->fullName.stIdx;
            textSz = name->fullName.stLen;
            break;
        case ASN_ORG_NAME:
            text = name->fullName.fullName + name->fullName.oIdx;
            textSz = name->fullName.oLen;
            break;
        case ASN_ORGUNIT_NAME:
            text = name->fullName.fullName + name->fullName.ouIdx;
            textSz = name->fullName.ouLen;
            break;
        case ASN_DOMAIN_COMPONENT:
            text = name->fullName.fullName + name->fullName.dcIdx[0];
            textSz = name->fullName.dcLen[0];
            break;
        default:
            WOLFSSL_MSG("Entry type not found");
            return SSL_FATAL_ERROR;
    }

    /* if buf is NULL return size of buffer needed (minus null char) */
    if (buf == NULL) {
        return textSz;
    }

    if (buf != NULL && text != NULL) {
        textSz = min(textSz + 1, len); /* + 1 to account for null char */
        if (textSz > 0) {
            XMEMCPY(buf, text, textSz - 1);
            buf[textSz - 1] = '\0';
        }
    }

    WOLFSSL_LEAVE("wolfSSL_X509_NAME_get_text_by_NID", textSz);
    return (textSz - 1); /* do not include null character in size */
}


/* Creates a new WOLFSSL_EVP_PKEY structure that has the public key from x509
 *
 * returns a pointer to the created WOLFSSL_EVP_PKEY on success and NULL on fail
 */
WOLFSSL_EVP_PKEY* wolfSSL_X509_get_pubkey(WOLFSSL_X509* x509)
{
    WOLFSSL_EVP_PKEY* key = NULL;
    WOLFSSL_ENTER("X509_get_pubkey");
    if (x509 != NULL) {
        key = (WOLFSSL_EVP_PKEY*)XMALLOC(
                    sizeof(WOLFSSL_EVP_PKEY), x509->heap,
                                                       DYNAMIC_TYPE_PUBLIC_KEY);
        if (key != NULL) {
            XMEMSET(key, 0, sizeof(WOLFSSL_EVP_PKEY));
            if (x509->pubKeyOID == RSAk) {
                key->type = EVP_PKEY_RSA;
            }
            else {
                key->type = EVP_PKEY_EC;
            }
            key->save_type = 0;
            key->pkey.ptr = (char*)XMALLOC(
                        x509->pubKey.length, x509->heap,
                                                       DYNAMIC_TYPE_PUBLIC_KEY);
            if (key->pkey.ptr == NULL) {
                XFREE(key, x509->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                return NULL;
            }
            XMEMCPY(key->pkey.ptr, x509->pubKey.buffer, x509->pubKey.length);
            key->pkey_sz = x509->pubKey.length;

            #ifdef HAVE_ECC
                key->pkey_curve = (int)x509->pkCurveOID;
            #endif /* HAVE_ECC */

            /* decode RSA key */
            #ifndef NO_RSA
            if (key->type == EVP_PKEY_RSA) {
                key->ownRsa = 1;
                key->rsa = wolfSSL_RSA_new();
                if (key->rsa == NULL) {
                    XFREE(key, x509->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                    return NULL;
                }

                if (wolfSSL_RSA_LoadDer_ex(key->rsa,
                            (const unsigned char*)key->pkey.ptr, key->pkey_sz,
                            WOLFSSL_RSA_LOAD_PUBLIC) != SSL_SUCCESS) {
                    wolfSSL_RSA_free(key->rsa);
                    XFREE(key, x509->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                    return NULL;
                }
            }
            #endif /* NO_RSA */

            /* decode ECC key */
            #ifdef HAVE_ECC
            if (key->type == EVP_PKEY_EC) {
                word32 idx = 0;

                key->ownEcc = 1;
                key->ecc = wolfSSL_EC_KEY_new();
                if (key->ecc == NULL || key->ecc->internal == NULL) {
                    XFREE(key, x509->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                    return NULL;
                }

                /* not using wolfSSL_EC_KEY_LoadDer because public key in x509
                 * is in the format of x963 (no sequence at start of buffer) */
                if (wc_EccPublicKeyDecode((const unsigned char*)key->pkey.ptr,
                        &idx, (ecc_key*)key->ecc->internal, key->pkey_sz) < 0) {
                    WOLFSSL_MSG("wc_EccPublicKeyDecode failed");
                    XFREE(key, x509->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                    wolfSSL_EC_KEY_free(key->ecc);
                    return NULL;
                }

                if (SetECKeyExternal(key->ecc) != SSL_SUCCESS) {
                    WOLFSSL_MSG("SetECKeyExternal failed");
                    XFREE(key, x509->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                    wolfSSL_EC_KEY_free(key->ecc);
                    return NULL;
                }

                key->ecc->inSet = 1;
            }
            #endif /* HAVE_ECC */
        }
    }
    return key;
}
#endif /* OPENSSL_EXTRA_X509_SMALL */
#endif /* !NO_CERTS */

/* End of smaller subset of X509 compatibility functions. Avoid increasing the
 * size of this subset and its memory usage */
#endif /* OPENSSL_EXTRA_X509_SMALL */

#if defined(OPENSSL_EXTRA)
#if !defined(NO_CERTS)
    int wolfSSL_X509_ext_isSet_by_NID(WOLFSSL_X509* x509, int nid)
    {
        int isSet = 0;

        WOLFSSL_ENTER("wolfSSL_X509_ext_isSet_by_NID");

        if (x509 != NULL) {
            switch (nid) {
                case BASIC_CA_OID: isSet = x509->basicConstSet; break;
                case ALT_NAMES_OID: isSet = x509->subjAltNameSet; break;
                case AUTH_KEY_OID: isSet = x509->authKeyIdSet; break;
                case SUBJ_KEY_OID: isSet = x509->subjKeyIdSet; break;
                case KEY_USAGE_OID: isSet = x509->keyUsageSet; break;
                #ifdef WOLFSSL_SEP
                    case CERT_POLICY_OID: isSet = x509->certPolicySet; break;
                #endif /* WOLFSSL_SEP */
            }
        }

        WOLFSSL_LEAVE("wolfSSL_X509_ext_isSet_by_NID", isSet);

        return isSet;
    }


    int wolfSSL_X509_ext_get_critical_by_NID(WOLFSSL_X509* x509, int nid)
    {
        int crit = 0;

        WOLFSSL_ENTER("wolfSSL_X509_ext_get_critical_by_NID");

        if (x509 != NULL) {
            switch (nid) {
                case BASIC_CA_OID: crit = x509->basicConstCrit; break;
                case ALT_NAMES_OID: crit = x509->subjAltNameCrit; break;
                case AUTH_KEY_OID: crit = x509->authKeyIdCrit; break;
                case SUBJ_KEY_OID: crit = x509->subjKeyIdCrit; break;
                case KEY_USAGE_OID: crit = x509->keyUsageCrit; break;
                #ifdef WOLFSSL_SEP
                    case CERT_POLICY_OID: crit = x509->certPolicyCrit; break;
                #endif /* WOLFSSL_SEP */
            }
        }

        WOLFSSL_LEAVE("wolfSSL_X509_ext_get_critical_by_NID", crit);

        return crit;
    }


    int wolfSSL_X509_get_isSet_pathLength(WOLFSSL_X509* x509)
    {
        int isSet = 0;

        WOLFSSL_ENTER("wolfSSL_X509_get_isSet_pathLength");

        if (x509 != NULL)
            isSet = x509->basicConstPlSet;

        WOLFSSL_LEAVE("wolfSSL_X509_get_isSet_pathLength", isSet);

        return isSet;
    }


    word32 wolfSSL_X509_get_pathLength(WOLFSSL_X509* x509)
    {
        word32 pathLength = 0;

        WOLFSSL_ENTER("wolfSSL_X509_get_pathLength");

        if (x509 != NULL)
            pathLength = x509->pathLength;

        WOLFSSL_LEAVE("wolfSSL_X509_get_pathLength", pathLength);

        return pathLength;
    }


    unsigned int wolfSSL_X509_get_keyUsage(WOLFSSL_X509* x509)
    {
        word16 usage = 0;

        WOLFSSL_ENTER("wolfSSL_X509_get_keyUsage");

        if (x509 != NULL)
            usage = x509->keyUsage;

        WOLFSSL_LEAVE("wolfSSL_X509_get_keyUsage", usage);

        return usage;
    }


    byte* wolfSSL_X509_get_authorityKeyID(WOLFSSL_X509* x509,
                                          byte* dst, int* dstLen)
    {
        byte *id = NULL;
        int copySz = 0;

        WOLFSSL_ENTER("wolfSSL_X509_get_authorityKeyID");

        if (x509 != NULL) {
            if (x509->authKeyIdSet) {
                copySz = min(dstLen != NULL ? *dstLen : 0,
                             (int)x509->authKeyIdSz);
                id = x509->authKeyId;
            }

            if (dst != NULL && dstLen != NULL && id != NULL && copySz > 0) {
                XMEMCPY(dst, id, copySz);
                id = dst;
                *dstLen = copySz;
            }
        }

        WOLFSSL_LEAVE("wolfSSL_X509_get_authorityKeyID", copySz);

        return id;
    }


    byte* wolfSSL_X509_get_subjectKeyID(WOLFSSL_X509* x509,
                                        byte* dst, int* dstLen)
    {
        byte *id = NULL;
        int copySz = 0;

        WOLFSSL_ENTER("wolfSSL_X509_get_subjectKeyID");

        if (x509 != NULL) {
            if (x509->subjKeyIdSet) {
                copySz = min(dstLen != NULL ? *dstLen : 0,
                                                        (int)x509->subjKeyIdSz);
                id = x509->subjKeyId;
            }

            if (dst != NULL && dstLen != NULL && id != NULL && copySz > 0) {
                XMEMCPY(dst, id, copySz);
                id = dst;
                *dstLen = copySz;
            }
        }

        WOLFSSL_LEAVE("wolfSSL_X509_get_subjectKeyID", copySz);

        return id;
    }


    int wolfSSL_X509_NAME_entry_count(WOLFSSL_X509_NAME* name)
    {
        int count = 0;

        WOLFSSL_ENTER("wolfSSL_X509_NAME_entry_count");

        if (name != NULL)
            count = name->fullName.entryCount;

        WOLFSSL_LEAVE("wolfSSL_X509_NAME_entry_count", count);
        return count;
    }



    int wolfSSL_X509_NAME_get_index_by_NID(WOLFSSL_X509_NAME* name,
                                          int nid, int pos)
    {
        int ret    = -1;

        WOLFSSL_ENTER("wolfSSL_X509_NAME_get_index_by_NID");

        if (name == NULL) {
            return BAD_FUNC_ARG;
        }

        /* these index values are already stored in DecodedName
           use those when available */
        if (name->fullName.fullName && name->fullName.fullNameLen > 0) {
            name->fullName.dcMode = 0;
            switch (nid) {
                case ASN_COMMON_NAME:
                    if (pos != name->fullName.cnIdx)
                        ret = name->fullName.cnIdx;
                    break;
                case ASN_DOMAIN_COMPONENT:
                    name->fullName.dcMode = 1;
                    if (pos < name->fullName.dcNum - 1){
                        ret = pos + 1;
                    } else {
                        ret = -1;
                    }
                    break;
                default:
                    WOLFSSL_MSG("NID not yet implemented");
                    break;
            }
        }

        WOLFSSL_LEAVE("wolfSSL_X509_NAME_get_index_by_NID", ret);

        (void)pos;
        (void)nid;

        return ret;
    }


    WOLFSSL_ASN1_STRING*  wolfSSL_X509_NAME_ENTRY_get_data(
                                                    WOLFSSL_X509_NAME_ENTRY* in)
    {
        WOLFSSL_ENTER("wolfSSL_X509_NAME_ENTRY_get_data");
        return in->value;
    }


    /* Creates a new WOLFSSL_ASN1_STRING structure.
     *
     * returns a pointer to the new structure created on success or NULL if fail
     */
    WOLFSSL_ASN1_STRING* wolfSSL_ASN1_STRING_new()
    {
        WOLFSSL_ASN1_STRING* asn1;

        WOLFSSL_ENTER("wolfSSL_ASN1_STRING_new");

        asn1 = (WOLFSSL_ASN1_STRING*)XMALLOC(sizeof(WOLFSSL_ASN1_STRING), NULL,
                DYNAMIC_TYPE_OPENSSL);
        if (asn1 != NULL) {
            XMEMSET(asn1, 0, sizeof(WOLFSSL_ASN1_STRING));
        }

        return asn1; /* no check for null because error case is returning null*/
    }


    /* used to free a WOLFSSL_ASN1_STRING structure */
    void wolfSSL_ASN1_STRING_free(WOLFSSL_ASN1_STRING* asn1)
    {
        WOLFSSL_ENTER("wolfSSL_ASN1_STRING_free");

        if (asn1 != NULL) {
            if (asn1->length > 0 && asn1->data != NULL) {
                XFREE(asn1->data, NULL, DYNAMIC_TYPE_OPENSSL);
            }
            XFREE(asn1, NULL, DYNAMIC_TYPE_OPENSSL);
        }
    }


    /* Creates a new WOLFSSL_ASN1_STRING structure given the input type.
     *
     * type is the type of set when WOLFSSL_ASN1_STRING is created
     *
     * returns a pointer to the new structure created on success or NULL if fail
     */
    WOLFSSL_ASN1_STRING* wolfSSL_ASN1_STRING_type_new(int type)
    {
        WOLFSSL_ASN1_STRING* asn1;

        WOLFSSL_ENTER("wolfSSL_ASN1_STRING_type_new");

        asn1 = wolfSSL_ASN1_STRING_new();
        if (asn1 == NULL) {
            return NULL;
        }
        asn1->type = type;

        return asn1;
    }


    /* if dataSz is negative then use XSTRLEN to find length of data
     * return WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on failure */
    int wolfSSL_ASN1_STRING_set(WOLFSSL_ASN1_STRING* asn1, const void* data,
            int dataSz)
    {
        int sz;

        WOLFSSL_ENTER("wolfSSL_ASN1_STRING_set");

        if (data == NULL || asn1 == NULL) {
            return WOLFSSL_FAILURE;
        }

        if (dataSz < 0) {
            sz = (int)XSTRLEN((const char*)data) + 1; /* +1 for null */
        }
        else {
            sz = dataSz;
        }

        if (sz < 0) {
            return WOLFSSL_FAILURE;
        }

        /* free any existing data before copying */
        if (asn1->data != NULL) {
            XFREE(asn1->data, NULL, DYNAMIC_TYPE_OPENSSL);
        }

        /* create new data buffer and copy over */
        asn1->data = (char*)XMALLOC(sz, NULL, DYNAMIC_TYPE_OPENSSL);
        if (asn1->data == NULL) {
            return WOLFSSL_FAILURE;
        }
        XMEMCPY(asn1->data, data, sz);
        asn1->length = sz;

        return WOLFSSL_SUCCESS;
    }


    unsigned char* wolfSSL_ASN1_STRING_data(WOLFSSL_ASN1_STRING* asn)
    {
        WOLFSSL_ENTER("wolfSSL_ASN1_STRING_data");

        if (asn) {
            return (unsigned char*)asn->data;
        }
        else {
            return NULL;
        }
    }


    int wolfSSL_ASN1_STRING_length(WOLFSSL_ASN1_STRING* asn)
    {
        WOLFSSL_ENTER("wolfSSL_ASN1_STRING_length");

        if (asn) {
            return asn->length;
        }
        else {
            return 0;
        }
    }


#ifdef XSNPRINTF /* a snprintf function needs to be available */
    /* Writes the human readable form of x509 to bio.
     *
     * bio  WOLFSSL_BIO to write to.
     * x509 Certificate to write.
     *
     * returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on failure
     */
    int wolfSSL_X509_print(WOLFSSL_BIO* bio, WOLFSSL_X509* x509)
    {
        WOLFSSL_ENTER("wolfSSL_X509_print");

        if (bio == NULL || x509 == NULL) {
            return WOLFSSL_FAILURE;
        }

        if (wolfSSL_BIO_write(bio, "Certificate:\n", sizeof("Certificate:\n"))
            <= 0) {
                return WOLFSSL_FAILURE;
        }

        if (wolfSSL_BIO_write(bio, "    Data:\n", sizeof("    Data:\n"))
            <= 0) {
                return WOLFSSL_FAILURE;
        }

        /* print version of cert */
        {
            int version;
            char tmp[17];

            if ((version = wolfSSL_X509_version(x509)) <= 0) {
                WOLFSSL_MSG("Error getting X509 version");
                return WOLFSSL_FAILURE;
            }
            if (wolfSSL_BIO_write(bio, "        Version: ",
                                sizeof("        Version: ")) <= 0) {
                return WOLFSSL_FAILURE;
            }
	        XSNPRINTF(tmp, sizeof(tmp), "%d\n", version);
            if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
                return WOLFSSL_FAILURE;
            }
        }

        /* print serial number out */
        {
            unsigned char serial[32];
            int  sz = sizeof(serial);

            XMEMSET(serial, 0, sz);
            if (wolfSSL_X509_get_serial_number(x509, serial, &sz)
                    != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("Error getting x509 serial number");
                return WOLFSSL_FAILURE;
            }
            if (wolfSSL_BIO_write(bio, "        Serial Number: ",
                                sizeof("        Serial Number: ")) <= 0) {
                return WOLFSSL_FAILURE;
            }

            /* if serial can fit into byte than print on the same line */
            if (sz <= (int)sizeof(byte)) {
                char tmp[17];
                XSNPRINTF(tmp, sizeof(tmp), "%d (0x%x)\n", serial[0],serial[0]);
                if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
                    return WOLFSSL_FAILURE;
                }
            }
            else {
                int i;
                char tmp[100];
                int  tmpSz = 100;
                char val[5];
                int  valSz = 5;

                /* serial is larger than int size so print off hex values */
                if (wolfSSL_BIO_write(bio, "\n            ",
                                sizeof("\n            ")) <= 0) {
                    return WOLFSSL_FAILURE;
                }
                tmp[0] = '\0';
                for (i = 0; i < sz - 1 && (3 * i) < tmpSz - valSz; i++) {
                    XSNPRINTF(val, sizeof(val) - 1, "%02x:", serial[i]);
                    val[3] = '\0'; /* make sure is null terminated */
                    XSTRNCAT(tmp, val, valSz);
                }
                XSNPRINTF(val, sizeof(val) - 1, "%02x\n", serial[i]);
                val[3] = '\0'; /* make sure is null terminated */
                XSTRNCAT(tmp, val, valSz);
                if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
                    return WOLFSSL_FAILURE;
                }
            }
        }

        /* print signature algo */
        {
            int   oid;
            char* sig;

            if ((oid = wolfSSL_X509_get_signature_type(x509)) <= 0) {
                WOLFSSL_MSG("Error getting x509 signature type");
                return WOLFSSL_FAILURE;
            }
            if (wolfSSL_BIO_write(bio, "    Signature Algorithm: ",
                                sizeof("    Signature Algorithm: ")) <= 0) {
                return WOLFSSL_FAILURE;
            }
            sig = GetSigName(oid);
            if (wolfSSL_BIO_write(bio, sig, (int)XSTRLEN(sig)) <= 0) {
                return WOLFSSL_FAILURE;
            }
            if (wolfSSL_BIO_write(bio, "\n", sizeof("\n")) <= 0) {
                return WOLFSSL_FAILURE;
            }
        }

        /* print issuer */
        {
            char* issuer;
        #ifdef WOLFSSL_SMALL_STACK
            char* buff  = NULL;
            int   issSz = 0;
        #else
            char buff[256];
            int  issSz = 256;
        #endif

            issuer  = wolfSSL_X509_NAME_oneline(
                             wolfSSL_X509_get_issuer_name(x509), buff, issSz);

            if (wolfSSL_BIO_write(bio, "        Issuer: ",
                                sizeof("        Issuer: ")) <= 0) {
                #ifdef WOLFSSL_SMALL_STACK
                XFREE(issuer, NULL, DYNAMIC_TYPE_OPENSSL);
                #endif
                return WOLFSSL_FAILURE;
            }
            if (issuer != NULL) {
                if (wolfSSL_BIO_write(bio, issuer, (int)XSTRLEN(issuer)) <= 0) {
                    #ifdef WOLFSSL_SMALL_STACK
                    XFREE(issuer, NULL, DYNAMIC_TYPE_OPENSSL);
                    #endif
                    return WOLFSSL_FAILURE;
                }
            }
            #ifdef WOLFSSL_SMALL_STACK
            XFREE(issuer, NULL, DYNAMIC_TYPE_OPENSSL);
            #endif
            if (wolfSSL_BIO_write(bio, "\n", sizeof("\n")) <= 0) {
                return WOLFSSL_FAILURE;
            }
        }

        /* print validity */
        {
            char tmp[80];

            if (wolfSSL_BIO_write(bio, "        Validity\n",
                                sizeof("        Validity\n")) <= 0) {
                return WOLFSSL_FAILURE;
            }
            if (wolfSSL_BIO_write(bio, "            Not Before: ",
                                sizeof("            Not Before: ")) <= 0) {
                return WOLFSSL_FAILURE;
            }
            if (GetTimeString(x509->notBefore + 2, ASN_UTC_TIME,
                tmp, sizeof(tmp)) != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("Error getting not before date");
                return WOLFSSL_FAILURE;
            }
            tmp[sizeof(tmp) - 1] = '\0'; /* make sure null terminated */
            if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
                return WOLFSSL_FAILURE;
            }
            if (wolfSSL_BIO_write(bio, "\n            Not After : ",
                                sizeof("\n            Not After : ")) <= 0) {
                return WOLFSSL_FAILURE;
            }
            if (GetTimeString(x509->notAfter + 2,ASN_UTC_TIME,
                tmp, sizeof(tmp)) != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("Error getting not before date");
                return WOLFSSL_FAILURE;
            }
            tmp[sizeof(tmp) - 1] = '\0'; /* make sure null terminated */
            if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
                return WOLFSSL_FAILURE;
            }
        }

        /* print subject */
        {
            char* subject;
        #ifdef WOLFSSL_SMALL_STACK
            char* buff  = NULL;
            int   subSz = 0;
        #else
            char buff[256];
            int  subSz = 256;
        #endif

            subject  = wolfSSL_X509_NAME_oneline(
                             wolfSSL_X509_get_subject_name(x509), buff, subSz);

            if (wolfSSL_BIO_write(bio, "\n        Subject: ",
                                sizeof("\n        Subject: ")) <= 0) {
                #ifdef WOLFSSL_SMALL_STACK
                XFREE(subject, NULL, DYNAMIC_TYPE_OPENSSL);
                #endif
                return WOLFSSL_FAILURE;
            }
            if (subject != NULL) {
                if (wolfSSL_BIO_write(bio, subject, (int)XSTRLEN(subject)) <= 0) {
                    #ifdef WOLFSSL_SMALL_STACK
                    XFREE(subject, NULL, DYNAMIC_TYPE_OPENSSL);
                    #endif
                    return WOLFSSL_FAILURE;
                }
            }
            #ifdef WOLFSSL_SMALL_STACK
            XFREE(subject, NULL, DYNAMIC_TYPE_OPENSSL);
            #endif
        }

        /* get and print public key */
        if (wolfSSL_BIO_write(bio, "\n        Subject Public Key Info:\n",
                          sizeof("\n        Subject Public Key Info:\n")) <= 0) {
            return WOLFSSL_FAILURE;
        }
        {
            char tmp[100];

            switch (x509->pubKeyOID) {
                #ifndef NO_RSA
                case RSAk:
                    if (wolfSSL_BIO_write(bio,
                                "            Public Key Algorithm: RSA\n",
                      sizeof("            Public Key Algorithm: RSA\n")) <= 0) {
                        return WOLFSSL_FAILURE;
                    }
                #ifdef HAVE_USER_RSA
                    if (wolfSSL_BIO_write(bio,
                        "                Build without user RSA to print key\n",
                sizeof("                Build without user RSA to print key\n"))
                        <= 0) {
                        return WOLFSSL_FAILURE;
                    }
                #else
                    {
                        RsaKey rsa;
                        word32 idx = 0;
                        int  sz;
                        byte lbit = 0;
                        int  rawLen;
                        unsigned char* rawKey;

                        if (wc_InitRsaKey(&rsa, NULL) != 0) {
                            WOLFSSL_MSG("wc_InitRsaKey failure");
                            return WOLFSSL_FAILURE;
                        }
                        if (wc_RsaPublicKeyDecode(x509->pubKey.buffer,
                                &idx, &rsa, x509->pubKey.length) != 0) {
                            WOLFSSL_MSG("Error decoding RSA key");
                            return WOLFSSL_FAILURE;
                        }
                        if ((sz = wc_RsaEncryptSize(&rsa)) < 0) {
                            WOLFSSL_MSG("Error getting RSA key size");
                            return WOLFSSL_FAILURE;
                        }
                        XSNPRINTF(tmp, sizeof(tmp) - 1, "%s%s: (%d bit)\n%s\n",
                                "                 ", "Public-Key", 8 * sz,
                                "                 Modulus:");
                        tmp[sizeof(tmp) - 1] = '\0';
                        if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
                            return WOLFSSL_FAILURE;
                        }

                        /* print out modulus */
                        XSNPRINTF(tmp, sizeof(tmp) - 1,"                     ");
                        tmp[sizeof(tmp) - 1] = '\0';
                        if (mp_leading_bit(&rsa.n)) {
                            lbit = 1;
                            XSTRNCAT(tmp, "00", sizeof("00"));
                        }

                        rawLen = mp_unsigned_bin_size(&rsa.n);
                        rawKey = (unsigned char*)XMALLOC(rawLen, NULL,
                                DYNAMIC_TYPE_TMP_BUFFER);
                        if (rawKey == NULL) {
                            WOLFSSL_MSG("Memory error");
                            return WOLFSSL_FAILURE;
                        }
                        mp_to_unsigned_bin(&rsa.n, rawKey);
                        for (idx = 0; idx < (word32)rawLen; idx++) {
                            char val[5];
                            int valSz = 5;

                            if ((idx == 0) && !lbit) {
                                XSNPRINTF(val, valSz - 1, "%02x", rawKey[idx]);
                            }
                            else if ((idx != 0) && (((idx + lbit) % 15) == 0)) {
                                tmp[sizeof(tmp) - 1] = '\0';
                                if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp))
                                        <= 0) {
                                    XFREE(rawKey, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                                    return WOLFSSL_FAILURE;
                                }
                                XSNPRINTF(tmp, sizeof(tmp) - 1,
                                        ":\n                     ");
                                XSNPRINTF(val, valSz - 1, "%02x", rawKey[idx]);
                            }
                            else {
                                XSNPRINTF(val, valSz - 1, ":%02x", rawKey[idx]);
                            }
                            XSTRNCAT(tmp, val, valSz);
                        }
                        XFREE(rawKey, NULL, DYNAMIC_TYPE_TMP_BUFFER);

                        /* print out remaning modulus values */
                        if ((idx > 0) && (((idx - 1 + lbit) % 15) != 0)) {
                                tmp[sizeof(tmp) - 1] = '\0';
                                if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp))
                                        <= 0) {
                                    return WOLFSSL_FAILURE;
                                }
                        }

                        /* print out exponent values */
                        rawLen = mp_unsigned_bin_size(&rsa.e);
                        if (rawLen < 0) {
                            WOLFSSL_MSG("Error getting exponent size");
                            return WOLFSSL_FAILURE;
                        }

                        if ((word32)rawLen < sizeof(word32)) {
                            rawLen = sizeof(word32);
                        }
                        rawKey = (unsigned char*)XMALLOC(rawLen, NULL,
                                DYNAMIC_TYPE_TMP_BUFFER);
                        if (rawKey == NULL) {
                            WOLFSSL_MSG("Memory error");
                            return WOLFSSL_FAILURE;
                        }
                        XMEMSET(rawKey, 0, rawLen);
                        mp_to_unsigned_bin(&rsa.e, rawKey);
                        if ((word32)rawLen <= sizeof(word32)) {
                            idx = *(word32*)rawKey;
                        }
                        XSNPRINTF(tmp, sizeof(tmp) - 1,
                        "\n                 Exponent: %d\n", idx);
                        if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
                            XFREE(rawKey, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                            return WOLFSSL_FAILURE;
                        }
                        XFREE(rawKey, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                    }
                #endif /* HAVE_USER_RSA */
                    break;
                #endif /* NO_RSA */

                #ifdef HAVE_ECC
                case ECDSAk:
                    {
                        word32 i;
                        ecc_key ecc;

                        if (wolfSSL_BIO_write(bio,
                                "            Public Key Algorithm: EC\n",
                      sizeof("            Public Key Algorithm: EC\n")) <= 0) {
                        return WOLFSSL_FAILURE;
                        }
                        if (wc_ecc_init_ex(&ecc, x509->heap, INVALID_DEVID)
                                != 0) {
                            return WOLFSSL_FAILURE;
                        }

                        i = 0;
                        if (wc_EccPublicKeyDecode(x509->pubKey.buffer, &i,
                                              &ecc, x509->pubKey.length) != 0) {
                            wc_ecc_free(&ecc);
                            return WOLFSSL_FAILURE;
                        }
                        XSNPRINTF(tmp, sizeof(tmp) - 1, "%s%s: (%d bit)\n%s\n",
                                "                 ", "Public-Key",
                                8 * wc_ecc_size(&ecc),
                                "                 pub:");
                        tmp[sizeof(tmp) - 1] = '\0';
                        if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
                            wc_ecc_free(&ecc);
                            return WOLFSSL_FAILURE;
                        }
                        XSNPRINTF(tmp, sizeof(tmp) - 1,"                     ");
                        {
                            word32 derSz;
                            byte*  der;

                            derSz = wc_ecc_size(&ecc) * WOLFSSL_BIT_SIZE;
                            der = (byte*)XMALLOC(derSz, x509->heap,
                                    DYNAMIC_TYPE_TMP_BUFFER);
                            if (der == NULL) {
                                wc_ecc_free(&ecc);
                                return WOLFSSL_FAILURE;
                            }

                            if (wc_ecc_export_x963(&ecc, der, &derSz) != 0) {
                                wc_ecc_free(&ecc);
                                XFREE(der, x509->heap, DYNAMIC_TYPE_TMP_BUFFER);
                                return WOLFSSL_FAILURE;
                            }
                            for (i = 0; i < derSz; i++) {
                                char val[5];
                                int valSz = 5;

                                if (i == 0) {
                                    XSNPRINTF(val, valSz - 1, "%02x", der[i]);
                                }
                                else if ((i % 15) == 0) {
                                    tmp[sizeof(tmp) - 1] = '\0';
                                    if (wolfSSL_BIO_write(bio, tmp,
                                                (int)XSTRLEN(tmp)) <= 0) {
                                        wc_ecc_free(&ecc);
                                        XFREE(der, x509->heap,
                                                DYNAMIC_TYPE_TMP_BUFFER);
                                        return WOLFSSL_FAILURE;
                                    }
                                    XSNPRINTF(tmp, sizeof(tmp) - 1,
                                        ":\n                     ");
                                    XSNPRINTF(val, valSz - 1, "%02x", der[i]);
                                }
                                else {
                                    XSNPRINTF(val, valSz - 1, ":%02x", der[i]);
                                }
                                XSTRNCAT(tmp, val, valSz);
                            }

                            /* print out remaning modulus values */
                            if ((i > 0) && (((i - 1) % 15) != 0)) {
                                tmp[sizeof(tmp) - 1] = '\0';
                                if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp))
                                        <= 0) {
                                    wc_ecc_free(&ecc);
                                    XFREE(der, x509->heap,
                                                DYNAMIC_TYPE_TMP_BUFFER);
                                    return WOLFSSL_FAILURE;
                                }
                            }

                            XFREE(der, x509->heap, DYNAMIC_TYPE_TMP_BUFFER);
                        }
                        XSNPRINTF(tmp, sizeof(tmp) - 1, "\n%s%s: %s\n",
                                "                ", "ASN1 OID",
                                ecc.dp->name);
                        if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
                            wc_ecc_free(&ecc);
                            return WOLFSSL_FAILURE;
                        }
                        wc_ecc_free(&ecc);
                    }
                    break;
                #endif /* HAVE_ECC */
                default:
                    WOLFSSL_MSG("Unknown key type");
                    return WOLFSSL_FAILURE;
            }
        }

        /* print out extensions */
        if (wolfSSL_BIO_write(bio, "        X509v3 extensions:\n",
                            sizeof("        X509v3 extensions:\n")) <= 0) {
            return WOLFSSL_FAILURE;
        }

        /* print subject key id */
        if (x509->subjKeyIdSet && x509->subjKeyId != NULL &&
                x509->subjKeyIdSz > 0) {
            char tmp[100];
            word32 i;
            char val[5];
            int valSz = 5;


            if (wolfSSL_BIO_write(bio,
                        "            X509v3 Subject Key Identifier:\n",
                 sizeof("            X509v3 Subject Key Identifier:\n"))
                 <= 0) {
                return WOLFSSL_FAILURE;
            }

            XSNPRINTF(tmp, sizeof(tmp) - 1, "                 ");
            for (i = 0; i < sizeof(tmp) && i < (x509->subjKeyIdSz - 1); i++) {
                XSNPRINTF(val, valSz - 1, "%02X:", x509->subjKeyId[i]);
                XSTRNCAT(tmp, val, valSz);
            }
            XSNPRINTF(val, valSz - 1, "%02X\n", x509->subjKeyId[i]);
            XSTRNCAT(tmp, val, valSz);
            if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
                return WOLFSSL_FAILURE;
            }
        }

        /* printf out authority key id */
        if (x509->authKeyIdSet && x509->authKeyId != NULL &&
                x509->authKeyIdSz > 0) {
            char tmp[100];
            word32 i;
            char val[5];
            int valSz = 5;

            if (wolfSSL_BIO_write(bio,
                        "            X509v3 Authority Key Identifier:\n",
                 sizeof("            X509v3 Authority Key Identifier:\n"))
                 <= 0) {
                return WOLFSSL_FAILURE;
            }

            XSNPRINTF(tmp, sizeof(tmp) - 1, "                 keyid");
            for (i = 0; i < x509->authKeyIdSz; i++) {
                /* check if buffer is almost full */
                if (XSTRLEN(tmp) >= sizeof(tmp) - valSz) {
                    if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
                        return WOLFSSL_FAILURE;
                    }
                    tmp[0] = '\0';
                }
                XSNPRINTF(val, valSz - 1, ":%02X", x509->authKeyId[i]);
                XSTRNCAT(tmp, val, valSz);
            }
            if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
                return WOLFSSL_FAILURE;
            }

            /* print issuer */
            {
                char* issuer;
            #ifdef WOLFSSL_SMALL_STACK
                char* buff  = NULL;
                int   issSz = 0;
            #else
                char buff[256];
                int  issSz = 256;
            #endif

                issuer  = wolfSSL_X509_NAME_oneline(
                               wolfSSL_X509_get_issuer_name(x509), buff, issSz);

                if (wolfSSL_BIO_write(bio, "\n                 DirName:",
                                  sizeof("\n                 DirName:")) <= 0) {
                    #ifdef WOLFSSL_SMALL_STACK
                    XFREE(issuer, NULL, DYNAMIC_TYPE_OPENSSL);
                    #endif
                    return WOLFSSL_FAILURE;
                }
                if (issuer != NULL) {
                    if (wolfSSL_BIO_write(bio, issuer, (int)XSTRLEN(issuer)) <= 0) {
                        #ifdef WOLFSSL_SMALL_STACK
                        XFREE(issuer, NULL, DYNAMIC_TYPE_OPENSSL);
                        #endif
                        return WOLFSSL_FAILURE;
                    }
                }
                #ifdef WOLFSSL_SMALL_STACK
                XFREE(issuer, NULL, DYNAMIC_TYPE_OPENSSL);
                #endif
                if (wolfSSL_BIO_write(bio, "\n", sizeof("\n")) <= 0) {
                    return WOLFSSL_FAILURE;
                }
            }
        }

        /* print basic constraint */
        if (x509->basicConstSet) {
            char tmp[100];

            if (wolfSSL_BIO_write(bio,
                        "\n            X509v3 Basic Constraints:\n",
                 sizeof("\n            X509v3 Basic Constraints:\n"))
                 <= 0) {
                return WOLFSSL_FAILURE;
            }
            XSNPRINTF(tmp, sizeof(tmp),
                    "                    CA:%s\n",
                    (x509->isCa)? "TRUE": "FALSE");
            if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
                return WOLFSSL_FAILURE;
            }
        }

        /* print out signature */
        {
            unsigned char* sig;
            int sigSz;
            int i;
            char tmp[100];
            int sigOid = wolfSSL_X509_get_signature_type(x509);

            if (wolfSSL_BIO_write(bio,
                                "    Signature Algorithm: ",
                      sizeof("    Signature Algorithm: ")) <= 0) {
                return WOLFSSL_FAILURE;
            }
            XSNPRINTF(tmp, sizeof(tmp) - 1,"%s\n", GetSigName(sigOid));
            tmp[sizeof(tmp) - 1] = '\0';
            if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
                return WOLFSSL_FAILURE;
            }

            sigSz = (int)x509->sig.length;
            sig = (unsigned char*)XMALLOC(sigSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            if (sig == NULL || sigSz <= 0) {
                return WOLFSSL_FAILURE;
            }
            if (wolfSSL_X509_get_signature(x509, sig, &sigSz) <= 0) {
                XFREE(sig, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                return WOLFSSL_FAILURE;
            }
            XSNPRINTF(tmp, sizeof(tmp) - 1,"         ");
            tmp[sizeof(tmp) - 1] = '\0';
            for (i = 0; i < sigSz; i++) {
                char val[5];
                int valSz = 5;

                if (i == 0) {
                    XSNPRINTF(val, valSz - 1, "%02x", sig[i]);
                }
                else if (((i % 18) == 0)) {
                    tmp[sizeof(tmp) - 1] = '\0';
                    if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp))
                            <= 0) {
                        XFREE(sig, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                        return WOLFSSL_FAILURE;
                    }
                    XSNPRINTF(tmp, sizeof(tmp) - 1,
                            ":\n         ");
                    XSNPRINTF(val, valSz - 1, "%02x", sig[i]);
                }
                else {
                    XSNPRINTF(val, valSz - 1, ":%02x", sig[i]);
                }
                XSTRNCAT(tmp, val, valSz);
            }
            XFREE(sig, NULL, DYNAMIC_TYPE_TMP_BUFFER);

            /* print out remaning sig values */
            if ((i > 0) && (((i - 1) % 18) != 0)) {
                    tmp[sizeof(tmp) - 1] = '\0';
                    if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp))
                            <= 0) {
                        return WOLFSSL_FAILURE;
                    }
            }
        }

        /* done with print out */
        if (wolfSSL_BIO_write(bio, "\n", sizeof("\n")) <= 0) {
            return WOLFSSL_FAILURE;
        }

        return WOLFSSL_SUCCESS;
    }
#endif /* XSNPRINTF */

#endif /* NO_CERTS */

char* wolfSSL_CIPHER_description(const WOLFSSL_CIPHER* cipher, char* in,
                                 int len)
{
    char *ret = in;
    const char *keaStr, *authStr, *encStr, *macStr;
    size_t strLen;

    if (cipher == NULL || in == NULL)
        return NULL;

    switch (cipher->ssl->specs.kea) {
        case no_kea:
            keaStr = "None";
            break;
#ifndef NO_RSA
        case rsa_kea:
            keaStr = "RSA";
            break;
#endif
#ifndef NO_DH
        case diffie_hellman_kea:
            keaStr = "DHE";
            break;
#endif
        case fortezza_kea:
            keaStr = "FZ";
            break;
#ifndef NO_PSK
        case psk_kea:
            keaStr = "PSK";
            break;
    #ifndef NO_DH
        case dhe_psk_kea:
            keaStr = "DHEPSK";
            break;
    #endif
    #ifdef HAVE_ECC
        case ecdhe_psk_kea:
            keaStr = "ECDHEPSK";
            break;
    #endif
#endif
#ifdef HAVE_NTRU
        case ntru_kea:
            keaStr = "NTRU";
            break;
#endif
#ifdef HAVE_ECC
        case ecc_diffie_hellman_kea:
            keaStr = "ECDHE";
            break;
        case ecc_static_diffie_hellman_kea:
            keaStr = "ECDH";
            break;
#endif
        default:
            keaStr = "unknown";
            break;
    }

    switch (cipher->ssl->specs.sig_algo) {
        case anonymous_sa_algo:
            authStr = "None";
            break;
#ifndef NO_RSA
        case rsa_sa_algo:
            authStr = "RSA";
            break;
#endif
#ifndef NO_DSA
        case dsa_sa_algo:
            authStr = "DSA";
            break;
#endif
#ifdef HAVE_ECC
        case ecc_dsa_sa_algo:
            authStr = "ECDSA";
            break;
#endif
        default:
            authStr = "unknown";
            break;
    }

    switch (cipher->ssl->specs.bulk_cipher_algorithm) {
        case wolfssl_cipher_null:
            encStr = "None";
            break;
#ifndef NO_RC4
        case wolfssl_rc4:
            encStr = "RC4(128)";
            break;
#endif
#ifndef NO_DES3
        case wolfssl_triple_des:
            encStr = "3DES(168)";
            break;
#endif
#ifdef HAVE_IDEA
        case wolfssl_idea:
            encStr = "IDEA(128)";
            break;
#endif
#ifndef NO_AES
        case wolfssl_aes:
            if (cipher->ssl->specs.key_size == 128)
                encStr = "AES(128)";
            else if (cipher->ssl->specs.key_size == 256)
                encStr = "AES(256)";
            else
                encStr = "AES(?)";
            break;
    #ifdef HAVE_AESGCM
        case wolfssl_aes_gcm:
            if (cipher->ssl->specs.key_size == 128)
                encStr = "AESGCM(128)";
            else if (cipher->ssl->specs.key_size == 256)
                encStr = "AESGCM(256)";
            else
                encStr = "AESGCM(?)";
            break;
    #endif
    #ifdef HAVE_AESCCM
        case wolfssl_aes_ccm:
            if (cipher->ssl->specs.key_size == 128)
                encStr = "AESCCM(128)";
            else if (cipher->ssl->specs.key_size == 256)
                encStr = "AESCCM(256)";
            else
                encStr = "AESCCM(?)";
            break;
    #endif
#endif
#ifdef HAVE_CHACHA
        case wolfssl_chacha:
            encStr = "CHACHA20/POLY1305(256)";
            break;
#endif
#ifdef HAVE_CAMELLIA
        case wolfssl_camellia:
            if (cipher->ssl->specs.key_size == 128)
                encStr = "Camellia(128)";
            else if (cipher->ssl->specs.key_size == 256)
                encStr = "Camellia(256)";
            else
                encStr = "Camellia(?)";
            break;
#endif
#if defined(HAVE_HC128) && !defined(NO_HC128)
        case wolfssl_hc128:
            encStr = "HC128(128)";
            break;
#endif
#if defined(HAVE_RABBIT) && !defined(NO_RABBIT)
        case wolfssl_rabbit:
            encStr = "RABBIT(128)";
            break;
#endif
        default:
            encStr = "unknown";
            break;
    }

    switch (cipher->ssl->specs.mac_algorithm) {
        case no_mac:
            macStr = "None";
            break;
#ifndef NO_MD5
        case md5_mac:
            macStr = "MD5";
            break;
#endif
#ifndef NO_SHA
        case sha_mac:
            macStr = "SHA1";
            break;
#endif
#ifdef HAVE_SHA224
        case sha224_mac:
            macStr = "SHA224";
            break;
#endif
#ifndef NO_SHA256
        case sha256_mac:
            macStr = "SHA256";
            break;
#endif
#ifdef HAVE_SHA384
        case sha384_mac:
            macStr = "SHA384";
            break;
#endif
#ifdef HAVE_SHA512
        case sha512_mac:
            macStr = "SHA512";
            break;
#endif
#ifdef HAVE_BLAKE2
        case blake2b_mac:
            macStr = "BLAKE2b";
            break;
#endif
        default:
            macStr = "unknown";
            break;
    }

    /* Build up the string by copying onto the end. */
    XSTRNCPY(in, wolfSSL_CIPHER_get_name(cipher), len);
    in[len-1] = '\0'; strLen = XSTRLEN(in); len -= (int)strLen; in += strLen;

    XSTRNCPY(in, " ", len);
    in[len-1] = '\0'; strLen = XSTRLEN(in); len -= (int)strLen; in += strLen;
    XSTRNCPY(in, wolfSSL_get_version(cipher->ssl), len);
    in[len-1] = '\0'; strLen = XSTRLEN(in); len -= (int)strLen; in += strLen;

    XSTRNCPY(in, " Kx=", len);
    in[len-1] = '\0'; strLen = XSTRLEN(in); len -= (int)strLen; in += strLen;
    XSTRNCPY(in, keaStr, len);
    in[len-1] = '\0'; strLen = XSTRLEN(in); len -= (int)strLen; in += strLen;

    XSTRNCPY(in, " Au=", len);
    in[len-1] = '\0'; strLen = XSTRLEN(in); len -= (int)strLen; in += strLen;
    XSTRNCPY(in, authStr, len);
    in[len-1] = '\0'; strLen = XSTRLEN(in); len -= (int)strLen; in += strLen;

    XSTRNCPY(in, " Enc=", len);
    in[len-1] = '\0'; strLen = XSTRLEN(in); len -= (int)strLen; in += strLen;
    XSTRNCPY(in, encStr, len);
    in[len-1] = '\0'; strLen = XSTRLEN(in); len -= (int)strLen; in += strLen;

    XSTRNCPY(in, " Mac=", len);
    in[len-1] = '\0'; strLen = XSTRLEN(in); len -= (int)strLen; in += strLen;
    XSTRNCPY(in, macStr, len);
    in[len-1] = '\0';

    return ret;
}


#ifndef NO_SESSION_CACHE

WOLFSSL_SESSION* wolfSSL_get1_session(WOLFSSL* ssl)
{
    if (ssl == NULL) {
        return NULL;
    }

    /* sessions are stored statically, no need for reference count */
    return wolfSSL_get_session(ssl);
}

#endif /* NO_SESSION_CACHE */



/* was do nothing */
/*
void OPENSSL_free(void* buf)
{
    (void)buf;
}
*/

#ifndef NO_WOLFSSL_STUB
int wolfSSL_OCSP_parse_url(char* url, char** host, char** port, char** path,
                   int* ssl)
{
    (void)url;
    (void)host;
    (void)port;
    (void)path;
    (void)ssl;
    WOLFSSL_STUB("OCSP_parse_url");
    return 0;
}
#endif

WOLFSSL_METHOD* wolfSSLv2_client_method(void)
{
    return 0;
}


WOLFSSL_METHOD* wolfSSLv2_server_method(void)
{
    return 0;
}


#ifndef NO_MD4

void wolfSSL_MD4_Init(WOLFSSL_MD4_CTX* md4)
{
    /* make sure we have a big enough buffer */
    typedef char ok[sizeof(md4->buffer) >= sizeof(Md4) ? 1 : -1];
    (void) sizeof(ok);

    WOLFSSL_ENTER("MD4_Init");
    wc_InitMd4((Md4*)md4);
}


void wolfSSL_MD4_Update(WOLFSSL_MD4_CTX* md4, const void* data,
                       unsigned long len)
{
    WOLFSSL_ENTER("MD4_Update");
    wc_Md4Update((Md4*)md4, (const byte*)data, (word32)len);
}


void wolfSSL_MD4_Final(unsigned char* digest, WOLFSSL_MD4_CTX* md4)
{
    WOLFSSL_ENTER("MD4_Final");
    wc_Md4Final((Md4*)md4, digest);
}

#endif /* NO_MD4 */


/* Removes a WOLFSSL_BIO struct from the WOLFSSL_BIO linked list.
 *
 * bio is the WOLFSSL_BIO struct in the list and removed.
 *
 * The return WOLFSSL_BIO struct is the next WOLFSSL_BIO in the list or NULL if
 * there is none.
 */
WOLFSSL_BIO* wolfSSL_BIO_pop(WOLFSSL_BIO* bio)
{
    if (bio == NULL) {
        WOLFSSL_MSG("Bad argument passed in");
        return NULL;
    }

    if (bio->prev != NULL) {
        bio->prev->next = bio->next;
    }

    if (bio->next != NULL) {
        bio->next->prev = bio->prev;
    }

    return bio->next;
}


int wolfSSL_BIO_pending(WOLFSSL_BIO* bio)
{
    return (int)wolfSSL_BIO_ctrl_pending(bio);
}



WOLFSSL_BIO_METHOD* wolfSSL_BIO_s_mem(void)
{
    static WOLFSSL_BIO_METHOD meth;

    WOLFSSL_ENTER("BIO_s_mem");
    meth.type = WOLFSSL_BIO_MEMORY;

    return &meth;
}


WOLFSSL_BIO_METHOD* wolfSSL_BIO_f_base64(void)
{
    static WOLFSSL_BIO_METHOD meth;

    WOLFSSL_ENTER("wolfSSL_BIO_f_base64");
    meth.type = WOLFSSL_BIO_BASE64;

    return &meth;
}


/* Set the flag for the bio.
 *
 * bio   the structre to set the flag in
 * flags the flag to use
 */
void wolfSSL_BIO_set_flags(WOLFSSL_BIO* bio, int flags)
{
    WOLFSSL_ENTER("wolfSSL_BIO_set_flags");

    if (bio != NULL) {
        bio->flags |= flags;
    }
}


#ifndef NO_WOLFSSL_STUB
void wolfSSL_RAND_screen(void)
{
    WOLFSSL_STUB("RAND_screen");
}
#endif



int wolfSSL_RAND_load_file(const char* fname, long len)
{
    (void)fname;
    /* wolfCrypt provides enough entropy internally or will report error */
    if (len == -1)
        return 1024;
    else
        return (int)len;
}


#ifndef NO_WOLFSSL_STUB
WOLFSSL_COMP_METHOD* wolfSSL_COMP_zlib(void)
{
    WOLFSSL_STUB("COMP_zlib");
    return 0;
}
#endif

#ifndef NO_WOLFSSL_STUB
WOLFSSL_COMP_METHOD* wolfSSL_COMP_rle(void)
{
    WOLFSSL_STUB("COMP_rle");
    return 0;
}
#endif

#ifndef NO_WOLFSSL_STUB
int wolfSSL_COMP_add_compression_method(int method, void* data)
{
    (void)method;
    (void)data;
    WOLFSSL_STUB("COMP_add_compression_method");
    return 0;
}
#endif

#ifndef NO_WOLFSSL_STUB
void wolfSSL_set_dynlock_create_callback(WOLFSSL_dynlock_value* (*f)(
                                                          const char*, int))
{
    WOLFSSL_STUB("CRYPTO_set_dynlock_create_callback");
    (void)f;
}
#endif

#ifndef NO_WOLFSSL_STUB
void wolfSSL_set_dynlock_lock_callback(
             void (*f)(int, WOLFSSL_dynlock_value*, const char*, int))
{
    WOLFSSL_STUB("CRYPTO_set_set_dynlock_lock_callback");
    (void)f;
}
#endif

#ifndef NO_WOLFSSL_STUB
void wolfSSL_set_dynlock_destroy_callback(
                  void (*f)(WOLFSSL_dynlock_value*, const char*, int))
{
    WOLFSSL_STUB("CRYPTO_set_set_dynlock_destroy_callback");
    (void)f;
}
#endif


const char* wolfSSL_X509_verify_cert_error_string(long err)
{
    return wolfSSL_ERR_reason_error_string(err);
}


#ifndef NO_WOLFSSL_STUB
int wolfSSL_X509_LOOKUP_add_dir(WOLFSSL_X509_LOOKUP* lookup, const char* dir,
                               long len)
{
    (void)lookup;
    (void)dir;
    (void)len;
    WOLFSSL_STUB("X509_LOOKUP_add_dir");
    return 0;
}
#endif

int wolfSSL_X509_LOOKUP_load_file(WOLFSSL_X509_LOOKUP* lookup,
                                 const char* file, long type)
{
#if !defined(NO_FILESYSTEM) && \
    (defined(WOLFSSL_PEM_TO_DER) || defined(WOLFSSL_DER_TO_PEM))
    int           ret = WOLFSSL_FAILURE;
    XFILE         fp;
    long          sz;
    byte*         pem = NULL;
    byte*         curr = NULL;
    byte*         prev = NULL;
    WOLFSSL_X509* x509;
    const char* header = NULL;
    const char* footer = NULL;

    if (type != X509_FILETYPE_PEM)
        return BAD_FUNC_ARG;

    fp = XFOPEN(file, "r");
    if (fp == NULL)
        return BAD_FUNC_ARG;

    XFSEEK(fp, 0, XSEEK_END);
    sz = XFTELL(fp);
    XREWIND(fp);

    if (sz <= 0)
        goto end;

    pem = (byte*)XMALLOC(sz, 0, DYNAMIC_TYPE_PEM);
    if (pem == NULL) {
        ret = MEMORY_ERROR;
        goto end;
    }

    /* Read in file which may be CRLs or certificates. */
    if (XFREAD(pem, (size_t)sz, 1, fp) != 1)
        goto end;

    prev = curr = pem;
    do {
        /* get PEM header and footer based on type */
        if (wc_PemGetHeaderFooter(CRL_TYPE, &header, &footer) == 0 &&
                XSTRNSTR((char*)curr, header, (unsigned int)sz) != NULL) {
#ifdef HAVE_CRL
            WOLFSSL_CERT_MANAGER* cm = lookup->store->cm;

            if (cm->crl == NULL) {
                if (wolfSSL_CertManagerEnableCRL(cm, 0) != WOLFSSL_SUCCESS) {
                    WOLFSSL_MSG("Enable CRL failed");
                    goto end;
                }
            }

            ret = BufferLoadCRL(cm->crl, curr, sz, WOLFSSL_FILETYPE_PEM, 1);
            if (ret != WOLFSSL_SUCCESS)
                goto end;
#endif
            curr = (byte*)XSTRNSTR((char*)curr, footer, (unsigned int)sz);
        }
        else if (wc_PemGetHeaderFooter(CERT_TYPE, &header, &footer) == 0 &&
                XSTRNSTR((char*)curr, header, (unsigned int)sz) != NULL) {
            x509 = wolfSSL_X509_load_certificate_buffer(curr, (int)sz,
                                                        WOLFSSL_FILETYPE_PEM);
            if (x509 == NULL)
                 goto end;
            ret = wolfSSL_X509_STORE_add_cert(lookup->store, x509);
            wolfSSL_X509_free(x509);
            if (ret != WOLFSSL_SUCCESS)
                goto end;
            curr = (byte*)XSTRNSTR((char*)curr, footer, (unsigned int)sz);
        }
        else
            goto end;

        if (curr == NULL)
            goto end;

        curr++;
        sz -= (long)(curr - prev);
        prev = curr;
    }
    while (ret == WOLFSSL_SUCCESS);

end:
    if (pem != NULL)
        XFREE(pem, 0, DYNAMIC_TYPE_PEM);
    XFCLOSE(fp);
    return ret;
#else
    (void)lookup;
    (void)file;
    (void)type;
    return WOLFSSL_FAILURE;
#endif
}

WOLFSSL_X509_LOOKUP_METHOD* wolfSSL_X509_LOOKUP_hash_dir(void)
{
    /* Method implementation in functions. */
    static WOLFSSL_X509_LOOKUP_METHOD meth = { 1 };
    return &meth;
}

WOLFSSL_X509_LOOKUP_METHOD* wolfSSL_X509_LOOKUP_file(void)
{
    /* Method implementation in functions. */
    static WOLFSSL_X509_LOOKUP_METHOD meth = { 0 };
    return &meth;
}


WOLFSSL_X509_LOOKUP* wolfSSL_X509_STORE_add_lookup(WOLFSSL_X509_STORE* store,
                                               WOLFSSL_X509_LOOKUP_METHOD* m)
{
    /* Method is a dummy value and is not needed. */
    (void)m;
    /* Make sure the lookup has a back reference to the store. */
    store->lookup.store = store;
    return &store->lookup;
}


#ifndef NO_CERTS
/* Converts the X509 to DER format and outputs it into bio.
 *
 * bio  is the structure to hold output DER
 * x509 certificate to create DER from
 *
 * returns WOLFSSL_SUCCESS on success
 */
int wolfSSL_i2d_X509_bio(WOLFSSL_BIO* bio, WOLFSSL_X509* x509)
{
    WOLFSSL_ENTER("wolfSSL_i2d_X509_bio");

    if (bio == NULL || x509 == NULL) {
        return WOLFSSL_FAILURE;
    }

    if (x509->derCert != NULL) {
        word32 len = x509->derCert->length;
        byte*  der = x509->derCert->buffer;

        if (wolfSSL_BIO_write(bio, der, len) == (int)len) {
            return SSL_SUCCESS;
        }
    }

    return WOLFSSL_FAILURE;
}


/* Converts an internal structure to a DER buffer
 *
 * x509 structure to get DER buffer from
 * out  buffer to hold result. If NULL then *out is NULL then a new buffer is
 *      created.
 *
 * returns the size of the DER result on success
 */
int wolfSSL_i2d_X509(WOLFSSL_X509* x509, unsigned char** out)
{
    const unsigned char* der;
    int derSz = 0;

    if (x509 == NULL || out == NULL) {
        return BAD_FUNC_ARG;
    }

    der = wolfSSL_X509_get_der(x509, &derSz);
    if (der == NULL) {
        return MEMORY_E;
    }

    if (*out == NULL) {
        *out = (unsigned char*)XMALLOC(derSz, NULL, DYNAMIC_TYPE_OPENSSL);
        if (*out == NULL) {
            return MEMORY_E;
        }
    }

    XMEMCPY(*out, der, derSz);

    return derSz;
}


/* Converts the DER from bio and creates a WOLFSSL_X509 structure from it.
 *
 * bio  is the structure holding DER
 * x509 certificate to create from DER. Can be NULL
 *
 * returns pointer to WOLFSSL_X509 structure on success and NULL on fail
 */
WOLFSSL_X509* wolfSSL_d2i_X509_bio(WOLFSSL_BIO* bio, WOLFSSL_X509** x509)
{
    WOLFSSL_X509* localX509 = NULL;
    unsigned char* mem  = NULL;
    int    ret;
    word32 size;

    WOLFSSL_ENTER("wolfSSL_d2i_X509_bio");

    if (bio == NULL) {
        WOLFSSL_MSG("Bad Function Argument bio is NULL");
        return NULL;
    }

    ret = wolfSSL_BIO_get_mem_data(bio, &mem);
    if (mem == NULL || ret <= 0) {
        WOLFSSL_MSG("Failed to get data from bio struct");
        return NULL;
    }
    size = ret;

    localX509 = wolfSSL_X509_d2i(NULL, mem, size);
    if (localX509 == NULL) {
        return NULL;
    }

    if (x509 != NULL) {
        *x509 = localX509;
    }

    return localX509;
}


#if !defined(NO_ASN) && !defined(NO_PWDBASED)
WC_PKCS12* wolfSSL_d2i_PKCS12_bio(WOLFSSL_BIO* bio, WC_PKCS12** pkcs12)
{
    WC_PKCS12* localPkcs12    = NULL;
    unsigned char* mem  = NULL;
    int ret;
    word32 size;

    WOLFSSL_ENTER("wolfSSL_d2i_PKCS12_bio");

    if (bio == NULL) {
        WOLFSSL_MSG("Bad Function Argument bio is NULL");
        return NULL;
    }

    localPkcs12 = wc_PKCS12_new();
    if (localPkcs12 == NULL) {
        WOLFSSL_MSG("Memory error");
        return NULL;
    }

    if (pkcs12 != NULL) {
        *pkcs12 = localPkcs12;
    }

    ret = wolfSSL_BIO_get_mem_data(bio, &mem);
    if (mem == NULL || ret <= 0) {
        WOLFSSL_MSG("Failed to get data from bio struct");
        wc_PKCS12_free(localPkcs12);
        if (pkcs12 != NULL) {
            *pkcs12 = NULL;
        }
        return NULL;
    }
    size = ret;

    ret = wc_d2i_PKCS12(mem, size, localPkcs12);
    if (ret < 0) {
        WOLFSSL_MSG("Failed to get PKCS12 sequence");
        wc_PKCS12_free(localPkcs12);
        if (pkcs12 != NULL) {
            *pkcs12 = NULL;
        }
        return NULL;
    }

    return localPkcs12;
}


/* helper function to get DER buffer from WOLFSSL_EVP_PKEY */
static int wolfSSL_i2d_PrivateKey(WOLFSSL_EVP_PKEY* key, unsigned char** der)
{
    *der = (unsigned char*)key->pkey.ptr;

    return key->pkey_sz;
}



/* Creates a new WC_PKCS12 structure
 *
 * pass  password to use
 * name  friendlyName to use
 * pkey  private key to go into PKCS12 bundle
 * cert  certificate to go into PKCS12 bundle
 * ca    extra certificates that can be added to bundle. Can be NULL
 * keyNID  type of encryption to use on the key (-1 means no encryption)
 * certNID type of ecnryption to use on the certificate
 * itt     number of iterations with encryption
 * macItt  number of iterations with mac creation
 * keyType flag for signature and/or encryption key
 *
 * returns a pointer to a new WC_PKCS12 structure on success and NULL on fail
 */
WC_PKCS12* wolfSSL_PKCS12_create(char* pass, char* name,
        WOLFSSL_EVP_PKEY* pkey, WOLFSSL_X509* cert,
        WOLF_STACK_OF(WOLFSSL_X509)* ca,
        int keyNID, int certNID, int itt, int macItt, int keyType)
{
    WC_PKCS12*      pkcs12;
    WC_DerCertList* list = NULL;
    word32 passSz;
    byte* keyDer;
    word32 keyDerSz;
    byte* certDer;
    int certDerSz;

    int ret;

    WOLFSSL_ENTER("wolfSSL_PKCS12_create()");

    if (pass == NULL || pkey == NULL || cert == NULL) {
        WOLFSSL_LEAVE("wolfSSL_PKCS12_create()", BAD_FUNC_ARG);
        return NULL;
    }
    passSz = (word32)XSTRLEN(pass);

    if ((ret = wolfSSL_i2d_PrivateKey(pkey, &keyDer)) < 0) {
        WOLFSSL_LEAVE("wolfSSL_PKCS12_create", ret);
        return NULL;
    }
    keyDerSz = ret;

    certDer = (byte*)wolfSSL_X509_get_der(cert, &certDerSz);
    if (certDer == NULL) {
        return NULL;
    }

    if (ca != NULL) {
        WC_DerCertList* cur;
        unsigned long numCerts = ca->num;
        byte* curDer;
        int   curDerSz = 0;
        WOLFSSL_STACK* sk = ca;

        while (numCerts > 0 && sk != NULL) {
            cur = (WC_DerCertList*)XMALLOC(sizeof(WC_DerCertList), NULL,
                    DYNAMIC_TYPE_PKCS);
            if (cur == NULL) {
                wc_FreeCertList(list, NULL);
                return NULL;
            }

            curDer = (byte*)wolfSSL_X509_get_der(sk->data.x509, &curDerSz);
            if (curDer == NULL || curDerSz < 0) {
                XFREE(cur, NULL, DYNAMIC_TYPE_PKCS);
                wc_FreeCertList(list, NULL);
                return NULL;
            }

            cur->buffer = (byte*)XMALLOC(curDerSz, NULL, DYNAMIC_TYPE_PKCS);
            if (cur->buffer == NULL) {
                XFREE(cur, NULL, DYNAMIC_TYPE_PKCS);
                wc_FreeCertList(list, NULL);
                return NULL;
            }
            XMEMCPY(cur->buffer, curDer, curDerSz);
            cur->bufferSz = curDerSz;
            cur->next = list;
            list = cur;

            sk = sk->next;
            numCerts--;
        }
    }

    pkcs12 = wc_PKCS12_create(pass, passSz, name, keyDer, keyDerSz,
            certDer, certDerSz, list, keyNID, certNID, itt, macItt,
            keyType, NULL);

    if (ca != NULL) {
        wc_FreeCertList(list, NULL);
    }

    return pkcs12;
}


/* return WOLFSSL_SUCCESS on success, WOLFSSL_FAILURE on failure */
int wolfSSL_PKCS12_parse(WC_PKCS12* pkcs12, const char* psw,
      WOLFSSL_EVP_PKEY** pkey, WOLFSSL_X509** cert, WOLF_STACK_OF(WOLFSSL_X509)** ca)
{
    DecodedCert DeCert;
    void* heap = NULL;
    int ret;
    byte* certData = NULL;
    word32 certDataSz;
    byte* pk = NULL;
    word32 pkSz;
    WC_DerCertList* certList = NULL;

    WOLFSSL_ENTER("wolfSSL_PKCS12_parse");

    if (pkcs12 == NULL || psw == NULL || pkey == NULL || cert == NULL) {
        WOLFSSL_MSG("Bad argument value");
        return WOLFSSL_FAILURE;
    }

    heap  = wc_PKCS12_GetHeap(pkcs12);
    *pkey = NULL;
    *cert = NULL;

    if (ca == NULL) {
        ret = wc_PKCS12_parse(pkcs12, psw, &pk, &pkSz, &certData, &certDataSz,
            NULL);
    }
    else {
        *ca = NULL;
        ret = wc_PKCS12_parse(pkcs12, psw, &pk, &pkSz, &certData, &certDataSz,
            &certList);
    }
    if (ret < 0) {
        WOLFSSL_LEAVE("wolfSSL_PKCS12_parse", ret);
        return WOLFSSL_FAILURE;
    }

    /* Decode cert and place in X509 stack struct */
    if (certList != NULL) {
        WC_DerCertList* current = certList;

        *ca = (WOLF_STACK_OF(WOLFSSL_X509)*)XMALLOC(sizeof(WOLF_STACK_OF(WOLFSSL_X509)),
                                               heap, DYNAMIC_TYPE_X509);
        if (*ca == NULL) {
            if (pk != NULL) {
                XFREE(pk, heap, DYNAMIC_TYPE_PUBLIC_KEY);
            }
            if (certData != NULL) {
                XFREE(*cert, heap, DYNAMIC_TYPE_PKCS); *cert = NULL;
            }
            /* Free up WC_DerCertList and move on */
            while (current != NULL) {
                WC_DerCertList* next = current->next;

                XFREE(current->buffer, heap, DYNAMIC_TYPE_PKCS);
                XFREE(current, heap, DYNAMIC_TYPE_PKCS);
                current = next;
            }
            return WOLFSSL_FAILURE;
        }
        XMEMSET(*ca, 0, sizeof(WOLF_STACK_OF(WOLFSSL_X509)));

        /* add list of DER certs as X509's to stack */
        while (current != NULL) {
            WC_DerCertList*  toFree = current;
            WOLFSSL_X509* x509;

            x509 = (WOLFSSL_X509*)XMALLOC(sizeof(WOLFSSL_X509), heap,
                                                             DYNAMIC_TYPE_X509);
            InitX509(x509, 1, heap);
            InitDecodedCert(&DeCert, current->buffer, current->bufferSz, heap);
            if (ParseCertRelative(&DeCert, CERT_TYPE, NO_VERIFY, NULL) != 0) {
                WOLFSSL_MSG("Issue with parsing certificate");
                FreeDecodedCert(&DeCert);
                wolfSSL_X509_free(x509);
            }
            else {
                if ((ret = CopyDecodedToX509(x509, &DeCert)) != 0) {
                    WOLFSSL_MSG("Failed to copy decoded cert");
                    FreeDecodedCert(&DeCert);
                    wolfSSL_X509_free(x509);
                    wolfSSL_sk_X509_free(*ca); *ca = NULL;
                    if (pk != NULL) {
                        XFREE(pk, heap, DYNAMIC_TYPE_PUBLIC_KEY);
                    }
                    if (certData != NULL) {
                        XFREE(certData, heap, DYNAMIC_TYPE_PKCS);
                    }
                    /* Free up WC_DerCertList */
                    while (current != NULL) {
                        WC_DerCertList* next = current->next;

                        XFREE(current->buffer, heap, DYNAMIC_TYPE_PKCS);
                        XFREE(current, heap, DYNAMIC_TYPE_PKCS);
                        current = next;
                    }
                    return WOLFSSL_FAILURE;
                }
                FreeDecodedCert(&DeCert);

                if (wolfSSL_sk_X509_push(*ca, x509) != 1) {
                    WOLFSSL_MSG("Failed to push x509 onto stack");
                    wolfSSL_X509_free(x509);
                    wolfSSL_sk_X509_free(*ca); *ca = NULL;
                    if (pk != NULL) {
                        XFREE(pk, heap, DYNAMIC_TYPE_PUBLIC_KEY);
                    }
                    if (certData != NULL) {
                        XFREE(certData, heap, DYNAMIC_TYPE_PKCS);
                    }

                    /* Free up WC_DerCertList */
                    while (current != NULL) {
                        WC_DerCertList* next = current->next;

                        XFREE(current->buffer, heap, DYNAMIC_TYPE_PKCS);
                        XFREE(current, heap, DYNAMIC_TYPE_PKCS);
                        current = next;
                    }
                    return WOLFSSL_FAILURE;
                }
            }
            current = current->next;
            XFREE(toFree->buffer, heap, DYNAMIC_TYPE_PKCS);
            XFREE(toFree, heap, DYNAMIC_TYPE_PKCS);
        }
    }


    /* Decode cert and place in X509 struct */
    if (certData != NULL) {
        *cert = (WOLFSSL_X509*)XMALLOC(sizeof(WOLFSSL_X509), heap,
                                                             DYNAMIC_TYPE_X509);
        if (*cert == NULL) {
            if (pk != NULL) {
                XFREE(pk, heap, DYNAMIC_TYPE_PUBLIC_KEY);
            }
            if (ca != NULL) {
                wolfSSL_sk_X509_free(*ca); *ca = NULL;
            }
            XFREE(certData, heap, DYNAMIC_TYPE_PKCS);
            return WOLFSSL_FAILURE;
        }
        InitX509(*cert, 1, heap);
        InitDecodedCert(&DeCert, certData, certDataSz, heap);
        if (ParseCertRelative(&DeCert, CERT_TYPE, NO_VERIFY, NULL) != 0) {
            WOLFSSL_MSG("Issue with parsing certificate");
        }
        if ((ret = CopyDecodedToX509(*cert, &DeCert)) != 0) {
            WOLFSSL_MSG("Failed to copy decoded cert");
            FreeDecodedCert(&DeCert);
            if (pk != NULL) {
                XFREE(pk, heap, DYNAMIC_TYPE_PUBLIC_KEY);
            }
            if (ca != NULL) {
                wolfSSL_sk_X509_free(*ca); *ca = NULL;
            }
            wolfSSL_X509_free(*cert); *cert = NULL;
            return WOLFSSL_FAILURE;
        }
        FreeDecodedCert(&DeCert);
        XFREE(certData, heap, DYNAMIC_TYPE_PKCS);
    }


    /* get key type */
    ret = BAD_STATE_E;
    if (pk != NULL) { /* decode key if present */
        *pkey = wolfSSL_PKEY_new_ex(heap);
        if (*pkey == NULL) {
            wolfSSL_X509_free(*cert); *cert = NULL;
            if (ca != NULL) {
                wolfSSL_sk_X509_free(*ca); *ca = NULL;
            }
            XFREE(pk, heap, DYNAMIC_TYPE_PUBLIC_KEY);
            return WOLFSSL_FAILURE;
        }
        #ifndef NO_RSA
        {
            word32 keyIdx = 0;
            RsaKey key;

            if (wc_InitRsaKey(&key, heap) != 0) {
                ret = BAD_STATE_E;
            }
            else {
                if ((ret = wc_RsaPrivateKeyDecode(pk, &keyIdx, &key, pkSz))
                                                                         == 0) {
                    (*pkey)->type = EVP_PKEY_RSA;
                    (*pkey)->rsa  = wolfSSL_RSA_new();
                    (*pkey)->ownRsa = 1; /* we own RSA */
                    if ((*pkey)->rsa == NULL) {
                        WOLFSSL_MSG("issue creating EVP RSA key");
                        wolfSSL_X509_free(*cert); *cert = NULL;
                        if (ca != NULL) {
                            wolfSSL_sk_X509_free(*ca); *ca = NULL;
                        }
                        wolfSSL_EVP_PKEY_free(*pkey); *pkey = NULL;
                        XFREE(pk, heap, DYNAMIC_TYPE_PKCS);
                        return WOLFSSL_FAILURE;
                    }
                    if ((ret = wolfSSL_RSA_LoadDer_ex((*pkey)->rsa, pk, pkSz,
                                    WOLFSSL_RSA_LOAD_PRIVATE)) != SSL_SUCCESS) {
                        WOLFSSL_MSG("issue loading RSA key");
                        wolfSSL_X509_free(*cert); *cert = NULL;
                        if (ca != NULL) {
                            wolfSSL_sk_X509_free(*ca); *ca = NULL;
                        }
                        wolfSSL_EVP_PKEY_free(*pkey); *pkey = NULL;
                        XFREE(pk, heap, DYNAMIC_TYPE_PKCS);
                        return WOLFSSL_FAILURE;
                    }

                    WOLFSSL_MSG("Found PKCS12 RSA key");
                    ret = 0; /* set in success state for upcoming ECC check */
                }
                wc_FreeRsaKey(&key);
            }
        }
        #endif /* NO_RSA */

        #ifdef HAVE_ECC
        {
            word32  keyIdx = 0;
            ecc_key key;

            if (ret != 0) { /* if is in fail state check if ECC key */
                if (wc_ecc_init(&key) != 0) {
                    wolfSSL_X509_free(*cert); *cert = NULL;
                    if (ca != NULL) {
                        wolfSSL_sk_X509_free(*ca); *ca = NULL;
                    }
                    wolfSSL_EVP_PKEY_free(*pkey); *pkey = NULL;
                    XFREE(pk, heap, DYNAMIC_TYPE_PKCS);
                    return WOLFSSL_FAILURE;
                }

                if ((ret = wc_EccPrivateKeyDecode(pk, &keyIdx, &key, pkSz))
                                                                         != 0) {
                    wolfSSL_X509_free(*cert); *cert = NULL;
                    if (ca != NULL) {
                        wolfSSL_sk_X509_free(*ca); *ca = NULL;
                    }
                    wolfSSL_EVP_PKEY_free(*pkey); *pkey = NULL;
                    XFREE(pk, heap, DYNAMIC_TYPE_PKCS);
                    WOLFSSL_MSG("Bad PKCS12 key format");
                    return WOLFSSL_FAILURE;
                }
                (*pkey)->type = EVP_PKEY_EC;
                (*pkey)->pkey_curve = key.dp->oidSum;
                wc_ecc_free(&key);
                WOLFSSL_MSG("Found PKCS12 ECC key");
            }
        }
        #else
        if (ret != 0) { /* if is in fail state and no ECC then fail */
            wolfSSL_X509_free(*cert); *cert = NULL;
            if (ca != NULL) {
                wolfSSL_sk_X509_free(*ca); *ca = NULL;
            }
            wolfSSL_EVP_PKEY_free(*pkey); *pkey = NULL;
            XFREE(pk, heap, DYNAMIC_TYPE_PKCS);
            WOLFSSL_MSG("Bad PKCS12 key format");
            return WOLFSSL_FAILURE;
        }
        #endif /* HAVE_ECC */

        (*pkey)->save_type = 0;
        (*pkey)->pkey_sz   = pkSz;
        (*pkey)->pkey.ptr  = (char*)pk;
    }

    (void)ret;
    (void)ca;

    return WOLFSSL_SUCCESS;
}
#endif /* !defined(NO_ASN) && !defined(NO_PWDBASED) */


/* no-op function. Was initially used for adding encryption algorithms available
 * for PKCS12 */
void wolfSSL_PKCS12_PBE_add(void)
{
    WOLFSSL_ENTER("wolfSSL_PKCS12_PBE_add");
}



WOLFSSL_STACK* wolfSSL_X509_STORE_CTX_get_chain(WOLFSSL_X509_STORE_CTX* ctx)
{
    if (ctx == NULL) {
        return NULL;
    }

#ifdef SESSION_CERTS
    /* if chain is null but sesChain is available then populate stack */
    if (ctx->chain == NULL && ctx->sesChain != NULL) {
        int i;
        WOLFSSL_X509_CHAIN* c = ctx->sesChain;
        WOLFSSL_STACK*     sk = (WOLFSSL_STACK*)XMALLOC(sizeof(WOLFSSL_STACK),
                                    NULL, DYNAMIC_TYPE_X509);

        if (sk == NULL) {
            return NULL;
        }

        XMEMSET(sk, 0, sizeof(WOLFSSL_STACK));
        ctx->chain = sk;
        for (i = 0; i < c->count && i < MAX_CHAIN_DEPTH; i++) {
            WOLFSSL_X509* x509 = wolfSSL_get_chain_X509(c, i);

            if (x509 == NULL) {
                WOLFSSL_MSG("Unable to get x509 from chain");
                wolfSSL_sk_X509_free(sk);
                return NULL;
            }

            if (wolfSSL_sk_X509_push(sk, x509) != SSL_SUCCESS) {
                WOLFSSL_MSG("Unable to load x509 into stack");
                wolfSSL_sk_X509_free(sk);
                return NULL;
            }
        }
    }
#endif /* SESSION_CERTS */

    return ctx->chain;
}


int wolfSSL_X509_STORE_add_cert(WOLFSSL_X509_STORE* store, WOLFSSL_X509* x509)
{
    int result = WOLFSSL_FATAL_ERROR;

    WOLFSSL_ENTER("wolfSSL_X509_STORE_add_cert");
    if (store != NULL && store->cm != NULL && x509 != NULL
                                                && x509->derCert != NULL) {
        DerBuffer* derCert = NULL;

        result = AllocDer(&derCert, x509->derCert->length,
            x509->derCert->type, NULL);
        if (result == 0) {
            /* AddCA() frees the buffer. */
            XMEMCPY(derCert->buffer,
                            x509->derCert->buffer, x509->derCert->length);
            result = AddCA(store->cm, &derCert, WOLFSSL_USER_CA, 1);
        }
    }

    WOLFSSL_LEAVE("wolfSSL_X509_STORE_add_cert", result);

    if (result != WOLFSSL_SUCCESS) {
        result = WOLFSSL_FATAL_ERROR;
    }

    return result;
}

WOLFSSL_X509_STORE* wolfSSL_X509_STORE_new(void)
{
    WOLFSSL_X509_STORE* store = NULL;

    if((store = (WOLFSSL_X509_STORE*)XMALLOC(sizeof(WOLFSSL_X509_STORE), NULL,
                            DYNAMIC_TYPE_X509_STORE)) == NULL)
        goto err_exit;
    
    if((store->cm = wolfSSL_CertManagerNew()) == NULL)
        goto err_exit;

    store->isDynamic = 1;

#ifdef HAVE_CRL
    store->crl = NULL;
    if((store->crl = (WOLFSSL_X509_CRL *)XMALLOC(sizeof(WOLFSSL_X509_CRL), 
                                NULL, DYNAMIC_TYPE_TMP_BUFFER)) == NULL)
        goto err_exit;
    if(InitCRL(store->crl, NULL) < 0)
        goto err_exit;       
#endif

    return store;

err_exit:
    if(store == NULL)
        return NULL;
    if(store->cm != NULL)
        wolfSSL_CertManagerFree(store->cm);
#ifdef HAVE_CRL
    if(store->crl != NULL)
        wolfSSL_X509_CRL_free(store->crl);
#endif
    wolfSSL_X509_STORE_free(store);

    return NULL;
}


void wolfSSL_X509_STORE_free(WOLFSSL_X509_STORE* store)
{
    if (store != NULL && store->isDynamic) {
        if (store->cm != NULL)
            wolfSSL_CertManagerFree(store->cm);
#ifdef HAVE_CRL
        if (store->crl != NULL)
            wolfSSL_X509_CRL_free(store->crl);
#endif
        XFREE(store, NULL, DYNAMIC_TYPE_X509_STORE);
    }
}


int wolfSSL_X509_STORE_set_flags(WOLFSSL_X509_STORE* store, unsigned long flag)
{
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_X509_STORE_set_flags");

    if ((flag & WOLFSSL_CRL_CHECKALL) || (flag & WOLFSSL_CRL_CHECK)) {
        ret = wolfSSL_CertManagerEnableCRL(store->cm, (int)flag);
    }

    (void)store;
    (void)flag;

    return ret;
}


int wolfSSL_X509_STORE_set_default_paths(WOLFSSL_X509_STORE* store)
{
    (void)store;
    return WOLFSSL_SUCCESS;
}

#ifndef NO_WOLFSSL_STUB
int wolfSSL_X509_STORE_get_by_subject(WOLFSSL_X509_STORE_CTX* ctx, int idx,
                            WOLFSSL_X509_NAME* name, WOLFSSL_X509_OBJECT* obj)
{
    (void)ctx;
    (void)idx;
    (void)name;
    (void)obj;
    WOLFSSL_STUB("X509_STORE_get_by_subject");
    return 0;
}
#endif

WOLFSSL_X509_STORE_CTX* wolfSSL_X509_STORE_CTX_new(void)
{
    WOLFSSL_X509_STORE_CTX* ctx = (WOLFSSL_X509_STORE_CTX*)XMALLOC(
                                    sizeof(WOLFSSL_X509_STORE_CTX), NULL,
                                    DYNAMIC_TYPE_X509_CTX);
    if (ctx != NULL) {
        ctx->param = NULL;
        wolfSSL_X509_STORE_CTX_init(ctx, NULL, NULL, NULL);
    }

    return ctx;
}


int wolfSSL_X509_STORE_CTX_init(WOLFSSL_X509_STORE_CTX* ctx,
     WOLFSSL_X509_STORE* store, WOLFSSL_X509* x509, WOLF_STACK_OF(WOLFSSL_X509)* sk)
{
    (void)sk;
    WOLFSSL_ENTER("wolfSSL_X509_STORE_CTX_init");
    if (ctx != NULL) {
        ctx->store = store;
        ctx->current_cert = x509;
        ctx->chain  = sk;
        ctx->domain = NULL;
#ifdef HAVE_EX_DATA
        ctx->ex_data = NULL;
#endif
        ctx->userCtx = NULL;
        ctx->error = 0;
        ctx->error_depth = 0;
        ctx->discardSessionCerts = 0;
#ifdef OPENSSL_EXTRA
        if (ctx->param == NULL) {
            ctx->param = (WOLFSSL_X509_VERIFY_PARAM*)XMALLOC(
                           sizeof(WOLFSSL_X509_VERIFY_PARAM),
                           NULL,DYNAMIC_TYPE_OPENSSL);
            if (ctx->param == NULL){
                WOLFSSL_MSG("wolfSSL_X509_STORE_CTX_init failed");
                return SSL_FATAL_ERROR;
            }
        }
#endif
        return SSL_SUCCESS;
    }
    return WOLFSSL_FATAL_ERROR;
}


void wolfSSL_X509_STORE_CTX_free(WOLFSSL_X509_STORE_CTX* ctx)
{
    if (ctx != NULL) {
        if (ctx->store != NULL)
            wolfSSL_X509_STORE_free(ctx->store);
        if (ctx->current_cert != NULL)
            wolfSSL_FreeX509(ctx->current_cert);
        if (ctx->chain != NULL)
            wolfSSL_sk_X509_free(ctx->chain);
#ifdef OPENSSL_EXTRA
        if (ctx->param != NULL){
            XFREE(ctx->param,NULL,DYNAMIC_TYPE_OPENSSL);
        }
#endif
        XFREE(ctx, NULL, DYNAMIC_TYPE_X509_CTX);
    }
}


void wolfSSL_X509_STORE_CTX_cleanup(WOLFSSL_X509_STORE_CTX* ctx)
{
    (void)ctx;
    /* Do nothing */
}


int wolfSSL_X509_verify_cert(WOLFSSL_X509_STORE_CTX* ctx)
{
    if (ctx != NULL && ctx->store != NULL && ctx->store->cm != NULL
         && ctx->current_cert != NULL && ctx->current_cert->derCert != NULL) {
        return wolfSSL_CertManagerVerifyBuffer(ctx->store->cm,
                    ctx->current_cert->derCert->buffer,
                    ctx->current_cert->derCert->length,
                    WOLFSSL_FILETYPE_ASN1);
    }
    return WOLFSSL_FATAL_ERROR;
}
#endif /* NO_CERTS */

#if !defined(NO_FILESYSTEM) 
static void *wolfSSL_d2i_X509_fp_ex(XFILE file, void **x509, int type)
{
    void *newx509 = NULL;
    DerBuffer*   der = NULL;
    byte *fileBuffer = NULL;

    if (file != XBADFILE)
    {
        long sz = 0;

        XFSEEK(file, 0, XSEEK_END);
        sz = XFTELL(file);
        XREWIND(file);

        if (sz < 0)
        {
            WOLFSSL_MSG("Bad tell on FILE");
            return NULL;
        }

        fileBuffer = (byte *)XMALLOC(sz, NULL, DYNAMIC_TYPE_FILE);
        if (fileBuffer != NULL)
        {
            if((long)XFREAD(fileBuffer, 1, sz, file) != sz)
            {
                WOLFSSL_MSG("File read failed");
                goto err_exit;
            }          
            if(type == CERT_TYPE)
                newx509 = (void *)wolfSSL_X509_d2i(NULL, fileBuffer, (int)sz);
            #ifdef HAVE_CRL
            else if(type == CRL_TYPE)
                newx509 = (void *)wolfSSL_d2i_X509_CRL(NULL, fileBuffer, (int)sz);
            #endif
            #if !defined(NO_ASN) && !defined(NO_PWDBASED)
            else if(type == PKCS12_TYPE){
                if((newx509 = wc_PKCS12_new()) == NULL)
                    goto err_exit;
                if(wc_d2i_PKCS12(fileBuffer, (int)sz, (WC_PKCS12*)newx509) < 0)
                    goto err_exit;
            }
            #endif
            else goto err_exit;
            if(newx509 == NULL)
            {
                WOLFSSL_MSG("X509 failed");
                goto err_exit;
            }
        }
    }
    if (x509 != NULL)
        *x509 = newx509;

    goto _exit;

err_exit:
    if(newx509 != NULL){
        if(type == CERT_TYPE)
            wolfSSL_X509_free((WOLFSSL_X509*)newx509);
        #ifdef HAVE_CRL
        else {
           if(type == CRL_TYPE)
                wolfSSL_X509_CRL_free((WOLFSSL_X509_CRL*)newx509);
        }
        #endif
    }
_exit:
    if(der != NULL)
        FreeDer(&der);
    if(fileBuffer != NULL)
        XFREE(fileBuffer, NULL, DYNAMIC_TYPE_FILE);
    return newx509;
}

WOLFSSL_X509_PKCS12 *wolfSSL_d2i_PKCS12_fp(XFILE fp, WOLFSSL_X509_PKCS12 **pkcs12)
{
    WOLFSSL_ENTER("wolfSSL_d2i_PKCS12_fp");
    return (WOLFSSL_X509_PKCS12 *)wolfSSL_d2i_X509_fp_ex(fp, (void **)pkcs12, PKCS12_TYPE);
}

WOLFSSL_X509 *wolfSSL_d2i_X509_fp(XFILE fp, WOLFSSL_X509 **x509)
{
    WOLFSSL_ENTER("wolfSSL_d2i_X509_fp");
    return (WOLFSSL_X509 *)wolfSSL_d2i_X509_fp_ex(fp, (void **)x509, CERT_TYPE);
}
#endif /* !NO_FILESYSTEM */


#ifdef HAVE_CRL
#ifndef NO_FILESYSTEM
WOLFSSL_X509_CRL *wolfSSL_d2i_X509_CRL_fp(XFILE fp, WOLFSSL_X509_CRL **crl)
{
    WOLFSSL_ENTER("wolfSSL_d2i_X509_CRL_fp");
    return (WOLFSSL_X509_CRL *)wolfSSL_d2i_X509_fp_ex(fp, (void **)crl, CRL_TYPE);
}
#endif /* !NO_FILESYSTEM */


WOLFSSL_X509_CRL* wolfSSL_d2i_X509_CRL(WOLFSSL_X509_CRL** crl, const unsigned char* in, int len)
{
    WOLFSSL_X509_CRL *newcrl = NULL;
    int ret ;

    WOLFSSL_ENTER("wolfSSL_d2i_X509_CRL");

    if(in == NULL){
        WOLFSSL_MSG("Bad argument value");
        return NULL;
    }

    newcrl = (WOLFSSL_X509_CRL*)XMALLOC(sizeof(WOLFSSL_X509_CRL), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (newcrl == NULL){
        WOLFSSL_MSG("New CRL allocation failed");
        return NULL;
    }
    if (InitCRL(newcrl, NULL) < 0) {
        WOLFSSL_MSG("Init tmp CRL failed");
        goto err_exit;
    }
    ret = BufferLoadCRL(newcrl, in, len, WOLFSSL_FILETYPE_ASN1, 1); 
    if (ret != WOLFSSL_SUCCESS){
        WOLFSSL_MSG("Buffer Load CRL failed");
        goto err_exit;
    }
    if(crl){
        *crl = newcrl;
    }
    goto _exit;

err_exit:
    if(newcrl != NULL)
        wolfSSL_X509_CRL_free(newcrl); 
    newcrl = NULL;
_exit:
    return newcrl;
}

void wolfSSL_X509_CRL_free(WOLFSSL_X509_CRL *crl)
{
    WOLFSSL_ENTER("wolfSSL_X509_CRL_free");

    FreeCRL(crl, 1);
    return;
}
#endif /* HAVE_CRL */

#ifndef NO_WOLFSSL_STUB
WOLFSSL_ASN1_TIME* wolfSSL_X509_CRL_get_lastUpdate(WOLFSSL_X509_CRL* crl)
{
    (void)crl;
    WOLFSSL_STUB("X509_CRL_get_lastUpdate");
    return 0;
}
#endif
#ifndef NO_WOLFSSL_STUB
WOLFSSL_ASN1_TIME* wolfSSL_X509_CRL_get_nextUpdate(WOLFSSL_X509_CRL* crl)
{
    (void)crl;
    WOLFSSL_STUB("X509_CRL_get_nextUpdate");
    return 0;
}
#endif


#ifndef NO_WOLFSSL_STUB
int wolfSSL_X509_CRL_verify(WOLFSSL_X509_CRL* crl, WOLFSSL_EVP_PKEY* key)
{
    (void)crl;
    (void)key;
    WOLFSSL_STUB("X509_CRL_verify");
    return 0;
}
#endif
#endif /* OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA_X509_SMALL)
/* Subset of OPENSSL_EXTRA for PKEY operations PKEY free is needed by the
 * subset of X509 API */

WOLFSSL_EVP_PKEY* wolfSSL_PKEY_new(){
    return wolfSSL_PKEY_new_ex(NULL);
}


WOLFSSL_EVP_PKEY* wolfSSL_PKEY_new_ex(void* heap)
{
    WOLFSSL_EVP_PKEY* pkey;
    int ret;
    WOLFSSL_ENTER("wolfSSL_PKEY_new");
    pkey = (WOLFSSL_EVP_PKEY*)XMALLOC(sizeof(WOLFSSL_EVP_PKEY), heap,
            DYNAMIC_TYPE_PUBLIC_KEY);
    if (pkey != NULL) {
        XMEMSET(pkey, 0, sizeof(WOLFSSL_EVP_PKEY));
        pkey->heap = heap;
        pkey->type = WOLFSSL_EVP_PKEY_DEFAULT;
#ifndef HAVE_FIPS
        ret = wc_InitRng_ex(&(pkey->rng), heap, INVALID_DEVID);
#else
        ret = wc_InitRng(&(pkey->rng));
#endif
        if (ret != 0){
            wolfSSL_EVP_PKEY_free(pkey);
            WOLFSSL_MSG("memory falure");
            return NULL;
        }
    }
    else {
        WOLFSSL_MSG("memory failure");
    }

    return pkey;
}


void wolfSSL_EVP_PKEY_free(WOLFSSL_EVP_PKEY* key)
{
    WOLFSSL_ENTER("wolfSSL_PKEY_free");
    if (key != NULL) {
        wc_FreeRng(&(key->rng));
        if (key->pkey.ptr != NULL)
        {
            XFREE(key->pkey.ptr, key->heap, DYNAMIC_TYPE_PUBLIC_KEY);
        }
        switch(key->type)
        {
            #ifndef NO_RSA
            case EVP_PKEY_RSA:
                if (key->rsa != NULL && key->ownRsa == 1) {
                    wolfSSL_RSA_free(key->rsa);
                }
                break;
            #endif /* NO_RSA */

            #ifdef HAVE_ECC
            case EVP_PKEY_EC:
                if (key->ecc != NULL && key->ownEcc == 1) {
                    wolfSSL_EC_KEY_free(key->ecc);
                }
                break;
            #endif /* HAVE_ECC */

            default:
            break;
        }
        XFREE(key, key->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    }
}
#endif /* OPENSSL_EXTRA_X509_SMALL */


#ifdef OPENSSL_EXTRA

void wolfSSL_X509_STORE_CTX_set_time(WOLFSSL_X509_STORE_CTX* ctx,
                                    unsigned long flags,
                                    time_t t)
{
    (void)flags;

    if (ctx == NULL || ctx->param == NULL)
        return;

    ctx->param->check_time = t;
    ctx->param->flags |= WOLFSSL_USE_CHECK_TIME;
}

#ifndef NO_WOLFSSL_STUB
void wolfSSL_X509_OBJECT_free_contents(WOLFSSL_X509_OBJECT* obj)
{
    (void)obj;
    WOLFSSL_STUB("X509_OBJECT_free_contents");
}
#endif

#ifndef NO_WOLFSSL_STUB
int wolfSSL_X509_cmp_current_time(const WOLFSSL_ASN1_TIME* asnTime)
{
    (void)asnTime;
    WOLFSSL_STUB("X509_cmp_current_time");
    return 0;
}
#endif

#ifndef NO_WOLFSSL_STUB
int wolfSSL_sk_X509_REVOKED_num(WOLFSSL_X509_REVOKED* revoked)
{
    (void)revoked;
    WOLFSSL_STUB("sk_X509_REVOKED_num");
    return 0;
}
#endif

#ifndef NO_WOLFSSL_STUB
WOLFSSL_X509_REVOKED* wolfSSL_X509_CRL_get_REVOKED(WOLFSSL_X509_CRL* crl)
{
    (void)crl;
    WOLFSSL_STUB("X509_CRL_get_REVOKED");
    return 0;
}
#endif

#ifndef NO_WOLFSSL_STUB
WOLFSSL_X509_REVOKED* wolfSSL_sk_X509_REVOKED_value(
                                    WOLFSSL_X509_REVOKED* revoked, int value)
{
    (void)revoked;
    (void)value;
    WOLFSSL_STUB("sk_X509_REVOKED_value");
    return 0;
}
#endif

/* Used to create a new WOLFSSL_ASN1_INTEGER structure.
 * returns a pointer to new structure on success and NULL on failure
 */
WOLFSSL_ASN1_INTEGER* wolfSSL_ASN1_INTEGER_new(void)
{
    WOLFSSL_ASN1_INTEGER* a;

    a = (WOLFSSL_ASN1_INTEGER*)XMALLOC(sizeof(WOLFSSL_ASN1_INTEGER), NULL,
                                       DYNAMIC_TYPE_OPENSSL);
    if (a == NULL) {
        return NULL;
    }

    XMEMSET(a, 0, sizeof(WOLFSSL_ASN1_INTEGER));
    a->data    = a->intData;
    a->dataMax = WOLFSSL_ASN1_INTEGER_MAX;
    return a;
}


/* free's internal elements of WOLFSSL_ASN1_INTEGER and free's "in" itself */
void wolfSSL_ASN1_INTEGER_free(WOLFSSL_ASN1_INTEGER* in)
{
    if (in != NULL) {
        if (in->isDynamic) {
            XFREE(in->data, NULL, DYNAMIC_TYPE_OPENSSL);
        }
        XFREE(in, NULL, DYNAMIC_TYPE_OPENSSL);
    }
}


WOLFSSL_ASN1_INTEGER* wolfSSL_X509_get_serialNumber(WOLFSSL_X509* x509)
{
    WOLFSSL_ASN1_INTEGER* a;
    int i = 0;

    WOLFSSL_ENTER("wolfSSL_X509_get_serialNumber");

    a = wolfSSL_ASN1_INTEGER_new();
    if (a == NULL)
        return NULL;

    /* Make sure there is space for the data, ASN.1 type and length. */
    if (x509->serialSz > (WOLFSSL_ASN1_INTEGER_MAX - 2)) {
        /* dynamicly create data buffer, +2 for type and length */
        a->data = (unsigned char*)XMALLOC(x509->serialSz + 2, NULL,
                DYNAMIC_TYPE_OPENSSL);
        if (a->data == NULL) {
            wolfSSL_ASN1_INTEGER_free(a);
            return NULL;
        }
        a->dataMax   = x509->serialSz + 2;
        a->isDynamic = 1;
    }

    a->data[i++] = ASN_INTEGER;
    i += SetLength(x509->serialSz, a->data + i);
    XMEMCPY(&a->data[i], x509->serial, x509->serialSz);

    return a;
}

#endif /* OPENSSL_EXTRA */

#if defined(WOLFSSL_MYSQL_COMPATIBLE) || defined(WOLFSSL_NGINX) || \
    defined(WOLFSSL_HAPROXY) || defined(OPENSSL_EXTRA) || defined(OPENSSL_ALL)
int wolfSSL_ASN1_TIME_print(WOLFSSL_BIO* bio, const WOLFSSL_ASN1_TIME* asnTime)
{
    char buf[MAX_TIME_STRING_SZ];
    int  ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_ASN1_TIME_print");

    if (bio == NULL || asnTime == NULL) {
        WOLFSSL_MSG("NULL function argument");
        return WOLFSSL_FAILURE;
    }

    if (wolfSSL_ASN1_TIME_to_string((WOLFSSL_ASN1_TIME*)asnTime, buf,
                sizeof(buf)) == NULL) {
        XMEMSET(buf, 0, MAX_TIME_STRING_SZ);
        XMEMCPY(buf, "Bad time value", 14);
        ret = WOLFSSL_FAILURE;
    }

    if (wolfSSL_BIO_write(bio, buf, (int)XSTRLEN(buf)) <= 0) {
        WOLFSSL_MSG("Unable to write to bio");
        return WOLFSSL_FAILURE;
    }

    return ret;
}


char* wolfSSL_ASN1_TIME_to_string(WOLFSSL_ASN1_TIME* t, char* buf, int len)
{
    int format;
    int dateLen;
    byte* date = (byte*)t;

    WOLFSSL_ENTER("wolfSSL_ASN1_TIME_to_string");

    if (t == NULL || buf == NULL || len < 5) {
        WOLFSSL_MSG("Bad argument");
        return NULL;
    }

    format  = *date; date++;
    dateLen = *date; date++;
    if (dateLen > len) {
        WOLFSSL_MSG("Length of date is longer then buffer");
        return NULL;
    }

    if (!GetTimeString(date, format, buf, len)) {
        return NULL;
    }

    return buf;
}
#endif /* WOLFSSL_MYSQL_COMPATIBLE || WOLFSSL_NGINX || WOLFSSL_HAPROXY ||
    OPENSSL_EXTRA*/


#ifdef OPENSSL_EXTRA

#if !defined(NO_ASN_TIME) && !defined(USER_TIME) && \
    !defined(TIME_OVERRIDES) && !defined(NO_FILESYSTEM)

WOLFSSL_ASN1_TIME* wolfSSL_ASN1_TIME_adj(WOLFSSL_ASN1_TIME *s, time_t t,
                                    int offset_day, long offset_sec)
{
    const int sec_per_day = 24*60*60;
    struct tm* ts = NULL;
    struct tm* tmpTime = NULL;
    time_t t_adj = 0;
    time_t offset_day_sec = 0;

#if defined(NEED_TMP_TIME)
    struct tm tmpTimeStorage;
    tmpTime = &tmpTimeStorage;
#else
    (void)tmpTime;
#endif

    WOLFSSL_ENTER("wolfSSL_ASN1_TIME_adj");

    if (s == NULL){
        s = (WOLFSSL_ASN1_TIME*)XMALLOC(sizeof(WOLFSSL_ASN1_TIME), NULL,
                                        DYNAMIC_TYPE_OPENSSL);
        if (s == NULL){
            return NULL;
        }
    }

    /* compute GMT time with offset */
    offset_day_sec = offset_day * sec_per_day;
    t_adj          = t + offset_day_sec + offset_sec;
    ts             = (struct tm *)XGMTIME(&t_adj, tmpTime);
    if (ts == NULL){
        WOLFSSL_MSG("failed to get time data.");
        XFREE(s, NULL, DYNAMIC_TYPE_OPENSSL);
        return NULL;
    }

    /* create ASN1 time notation */
    /* UTC Time */
    if (ts->tm_year >= 50 && ts->tm_year < 150){
        char utc_str[ASN_UTC_TIME_SIZE];
        int utc_year = 0,utc_mon,utc_day,utc_hour,utc_min,utc_sec;
        byte *data_ptr = NULL;

        if (ts->tm_year >= 50 && ts->tm_year < 100){
            utc_year = ts->tm_year;
        } else if (ts->tm_year >= 100 && ts->tm_year < 150){
            utc_year = ts->tm_year - 100;
        }
        utc_mon  = ts->tm_mon + 1;
        utc_day  = ts->tm_mday;
        utc_hour = ts->tm_hour;
        utc_min  = ts->tm_min;
        utc_sec  = ts->tm_sec;
        XSNPRINTF((char *)utc_str, ASN_UTC_TIME_SIZE,
                  "%02d%02d%02d%02d%02d%02dZ",
                  utc_year, utc_mon, utc_day, utc_hour, utc_min, utc_sec);
        data_ptr  = s->data;
        *data_ptr = (byte) ASN_UTC_TIME; data_ptr++;
        *data_ptr = (byte) ASN_UTC_TIME_SIZE; data_ptr++;
        XMEMCPY(data_ptr,(byte *)utc_str, ASN_UTC_TIME_SIZE);
    /* GeneralizedTime */
    } else {
        char gt_str[ASN_GENERALIZED_TIME_SIZE];
        int gt_year,gt_mon,gt_day,gt_hour,gt_min,gt_sec;
        byte *data_ptr = NULL;

        gt_year = ts->tm_year + 1900;
        gt_mon  = ts->tm_mon + 1;
        gt_day  = ts->tm_mday;
        gt_hour = ts->tm_hour;
        gt_min  = ts->tm_min;
        gt_sec  = ts->tm_sec;
        XSNPRINTF((char *)gt_str, ASN_GENERALIZED_TIME_SIZE,
                  "%4d%02d%02d%02d%02d%02dZ",
                  gt_year, gt_mon, gt_day, gt_hour, gt_min,gt_sec);
        data_ptr  = s->data;
        *data_ptr = (byte) ASN_GENERALIZED_TIME; data_ptr++;
        *data_ptr = (byte) ASN_GENERALIZED_TIME_SIZE; data_ptr++;
        XMEMCPY(data_ptr,(byte *)gt_str, ASN_GENERALIZED_TIME_SIZE);
    }

    return s;
}
#endif /* !NO_ASN_TIME && !USER_TIME && !TIME_OVERRIDES && !NO_FILESYSTEM */

#ifndef NO_WOLFSSL_STUB
int wolfSSL_ASN1_INTEGER_cmp(const WOLFSSL_ASN1_INTEGER* a,
                            const WOLFSSL_ASN1_INTEGER* b)
{
    (void)a;
    (void)b;
    WOLFSSL_STUB("ASN1_INTEGER_cmp");
    return 0;
}
#endif

#ifndef NO_WOLFSSL_STUB
long wolfSSL_ASN1_INTEGER_get(const WOLFSSL_ASN1_INTEGER* i)
{
    (void)i;
    WOLFSSL_STUB("ASN1_INTEGER_get");
    return 0;
}
#endif


void* wolfSSL_X509_STORE_CTX_get_ex_data(WOLFSSL_X509_STORE_CTX* ctx, int idx)
{
    WOLFSSL_ENTER("wolfSSL_X509_STORE_CTX_get_ex_data");
#if defined(HAVE_EX_DATA) || defined(FORTRESS)
    if (ctx != NULL && idx == 0)
        return ctx->ex_data;
#else
    (void)ctx;
    (void)idx;
#endif
    return 0;
}


/* Gets an index to store SSL structure at.
 *
 * Returns positive index on success and negative values on failure
 */
int wolfSSL_get_ex_data_X509_STORE_CTX_idx(void)
{
    WOLFSSL_ENTER("wolfSSL_get_ex_data_X509_STORE_CTX_idx");

    /* store SSL at index 0 */
    return 0;
}


/* Set an error stat in the X509 STORE CTX
 *
 */
void wolfSSL_X509_STORE_CTX_set_error(WOLFSSL_X509_STORE_CTX* ctx, int er)
{
    WOLFSSL_ENTER("wolfSSL_X509_STORE_CTX_set_error");

    if (ctx != NULL) {
        ctx->error = er;
    }
}


/* Sets a function callback that will send information about the state of all
 * WOLFSSL objects that have been created by the WOLFSSL_CTX structure passed
 * in.
 *
 * ctx WOLFSSL_CTX structre to set callback function in
 * f   callback function to use
 */
void wolfSSL_CTX_set_info_callback(WOLFSSL_CTX* ctx,
       void (*f)(const WOLFSSL* ssl, int type, int val))
{
    WOLFSSL_ENTER("wolfSSL_CTX_set_info_callback");
    if (ctx == NULL) {
        WOLFSSL_MSG("Bad function argument");
    }
    else {
        ctx->CBIS = f;
    }
}


unsigned long wolfSSL_ERR_peek_error(void)
{
    WOLFSSL_ENTER("wolfSSL_ERR_peek_error");

    return wolfSSL_ERR_peek_error_line_data(NULL, NULL, NULL, NULL);
}


/* This function is to find global error values that are the same through out
 * all library version. With wolfSSL having only one set of error codes the
 * return value is pretty straight forward. The only thing needed is all wolfSSL
 * error values are typically negative.
 *
 * Returns the error reason
 */
int wolfSSL_ERR_GET_REASON(unsigned long err)
{
    int ret = (int)err;

    WOLFSSL_ENTER("wolfSSL_ERR_GET_REASON");

#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
    /* Nginx looks for this error to know to stop parsing certificates. */
    if (err == ((ERR_LIB_PEM << 24) | PEM_R_NO_START_LINE))
        return PEM_R_NO_START_LINE;
#endif

    /* check if error value is in range of wolfSSL errors */
    ret = 0 - ret; /* setting as negative value */
    /* wolfCrypt range is less than MAX (-100)
       wolfSSL range is MIN (-300) and lower */
    if (ret < MAX_CODE_E) {
        return ret;
    }
    else {
        WOLFSSL_MSG("Not in range of typical error values");
        ret = (int)err;
    }

    return ret;
}


/* returns a string that describes the alert
 *
 * alertID the alert value to look up
 */
const char* wolfSSL_alert_type_string_long(int alertID)
{
    WOLFSSL_ENTER("wolfSSL_aalert_type_string_long");

    switch (alertID) {
        case close_notify:
            {
                static const char close_notify_str[] =
                    "close_notify";
                return close_notify_str;
            }

        case unexpected_message:
            {
                static const char unexpected_message_str[] =
                    "unexpected_message";
                return unexpected_message_str;
            }

        case bad_record_mac:
            {
                static const char bad_record_mac_str[] =
                    "bad_record_mac";
                return bad_record_mac_str;
            }

        case record_overflow:
            {
                static const char record_overflow_str[] =
                    "record_overflow";
                return record_overflow_str;
            }

        case decompression_failure:
            {
                static const char decompression_failure_str[] =
                    "decompression_failure";
                return decompression_failure_str;
            }

        case handshake_failure:
            {
                static const char handshake_failure_str[] =
                    "handshake_failure";
                return handshake_failure_str;
            }

        case no_certificate:
            {
                static const char no_certificate_str[] =
                    "no_certificate";
                return no_certificate_str;
            }

        case bad_certificate:
            {
                static const char bad_certificate_str[] =
                    "bad_certificate";
                return bad_certificate_str;
            }

        case unsupported_certificate:
            {
                static const char unsupported_certificate_str[] =
                    "unsupported_certificate";
                return unsupported_certificate_str;
            }

        case certificate_revoked:
            {
                static const char certificate_revoked_str[] =
                    "certificate_revoked";
                return certificate_revoked_str;
            }

        case certificate_expired:
            {
                static const char certificate_expired_str[] =
                    "certificate_expired";
                return certificate_expired_str;
            }

        case certificate_unknown:
            {
                static const char certificate_unknown_str[] =
                    "certificate_unknown";
                return certificate_unknown_str;
            }

        case illegal_parameter:
            {
                static const char illegal_parameter_str[] =
                    "illegal_parameter";
                return illegal_parameter_str;
            }

        case decode_error:
            {
                static const char decode_error_str[] =
                    "decode_error";
                return decode_error_str;
            }

        case decrypt_error:
            {
                static const char decrypt_error_str[] =
                    "decrypt_error";
                return decrypt_error_str;
            }

    #ifdef WOLFSSL_MYSQL_COMPATIBLE
    /* catch name conflict for enum protocol with MYSQL build */
        case wc_protocol_version:
            {
                static const char wc_protocol_version_str[] =
                    "wc_protocol_version";
                return wc_protocol_version_str;
            }

    #else
        case protocol_version:
            {
                static const char protocol_version_str[] =
                    "protocol_version";
                return protocol_version_str;
            }

    #endif
        case no_renegotiation:
            {
                static const char no_renegotiation_str[] =
                    "no_renegotiation";
                return no_renegotiation_str;
            }

        case unrecognized_name:
            {
                static const char unrecognized_name_str[] =
                    "unrecognized_name";
                return unrecognized_name_str;
            }

        case bad_certificate_status_response:
            {
                static const char bad_certificate_status_response_str[] =
                    "bad_certificate_status_response";
                return bad_certificate_status_response_str;
            }

        case no_application_protocol:
            {
                static const char no_application_protocol_str[] =
                    "no_application_protocol";
                return no_application_protocol_str;
            }

        default:
            WOLFSSL_MSG("Unknown Alert");
            return NULL;
    }
}


const char* wolfSSL_alert_desc_string_long(int alertID)
{
    WOLFSSL_ENTER("wolfSSL_alert_desc_string_long");
    return wolfSSL_alert_type_string_long(alertID);
}


/* Gets the current state of the WOLFSSL structure
 *
 * ssl WOLFSSL structure to get state of
 *
 * Returns a human readable string of the WOLFSSL structure state
 */
const char* wolfSSL_state_string_long(const WOLFSSL* ssl)
{

    static const char* OUTPUT_STR[14][6][3] = {
        {
            {"SSLv3 Initialization","SSLv3 Initialization","SSLv3 Initialization"},
            {"TLSv1 Initialization","TLSv2 Initialization","TLSv2 Initialization"},
            {"TLSv1_1 Initialization","TLSv1_1 Initialization","TLSv1_1 Initialization"},
            {"TLSv1_2 Initialization","TLSv1_2 Initialization","TLSv1_2 Initialization"},
            {"DTLSv1 Initialization","DTLSv1 Initialization","DTLSv1 Initialization"},
            {"DTLSv1_2 Initialization","DTLSv1_2 Initialization","DTLSv1_2 Initialization"},
        },
        {
            {"SSLv3 read Server Hello Verify Request",
             "SSLv3 write Server Hello Verify Request",
             "SSLv3 Server Hello Verify Request"},
            {"TLSv1 read Server Hello Verify Request",
             "TLSv1 write Server Hello Verify Request",
             "TLSv1 Server Hello Verify Request"},
            {"TLSv1_1 read Server Hello Verify Request",
            "TLSv1_1 write Server Hello Verify Request",
             "TLSv1_1 Server Hello Verify Request"},
            {"TLSv1_2 read Server Hello Verify Request",
            "TLSv1_2 write Server Hello Verify Request",
             "TLSv1_2 Server Hello Verify Request"},
            {"DTLSv1 read Server Hello Verify Request",
             "DTLSv1 write Server Hello Verify Request",
             "DTLSv1 Server Hello Verify Request"},
            {"DTLSv1_2 read Server Hello Verify Request",
             "DTLSv1_2 write Server Hello Verify Request",
             "DTLSv1_2 Server Hello Verify Request"},
        },
        {
            {"SSLv3 read Server Hello",
             "SSLv3 write Server Hello",
             "SSLv3 Server Hello"},
            {"TLSv1 read Server Hello",
             "TLSv1 write Server Hello",
             "TLSv1 Server Hello"},
            {"TLSv1_1 read Server Hello",
            "TLSv1_1 write Server Hello",
             "TLSv1_1 Server Hello"},
            {"TLSv1_2 read Server Hello",
            "TLSv1_2 write Server Hello",
             "TLSv1_2 Server Hello"},
            {"DTLSv1 read Server Hello",
            "DTLSv1 write Server Hello",
             "DTLSv1 Server Hello"},
            {"DTLSv1_2 read Server Hello"
             "DTLSv1_2 write Server Hello",
             "DTLSv1_2 Server Hello",
            },
        },
        {
            {"SSLv3 read Server Session Ticket",
             "SSLv3 write Server Session Ticket",
             "SSLv3 Server Session Ticket"},
            {"TLSv1 read Server Session Ticket",
             "TLSv1 write Server Session Ticket",
             "TLSv1 Server Session Ticket"},
            {"TLSv1_1 read Server Session Ticket",
             "TLSv1_1 write Server Session Ticket",
             "TLSv1_1 Server Session Ticket"},
            {"TLSv1_2 read Server Session Ticket",
             "TLSv1_2 write Server Session Ticket",
             "TLSv1_2 Server Session Ticket"},
            {"DTLSv1 read Server Session Ticket",
             "DTLSv1 write Server Session Ticket",
             "DTLSv1 Server Session Ticket"},
            {"DTLSv1_2 read Server Session Ticket",
             "DTLSv1_2 write Server Session Ticket",
             "DTLSv1_2 Server Session Ticket"},
        },
        {
            {"SSLv3 read Server Cert",
             "SSLv3 write Server Cert",
             "SSLv3 Server Cert"},
            {"TLSv1 read Server Cert",
             "TLSv1 write Server Cert",
             "TLSv1 Server Cert"},
            {"TLSv1_1 read Server Cert",
             "TLSv1_1 write Server Cert",
             "TLSv1_1 Server Cert"},
            {"TLSv1_2 read Server Cert",
             "TLSv1_2 write Server Cert",
             "TLSv1_2 Server Cert"},
            {"DTLSv1 read Server Cert",
             "DTLSv1 write Server Cert",
             "DTLSv1 Server Cert"},
            {"DTLSv1_2 read Server Cert",
             "DTLSv1_2 write Server Cert",
             "DTLSv1_2 Server Cert"},
        },
        {
            {"SSLv3 read Server Key Exchange",
             "SSLv3 write Server Key Exchange",
             "SSLv3 Server Key Exchange"},
            {"TLSv1 read Server Key Exchange",
             "TLSv1 write Server Key Exchange",
             "TLSv1 Server Key Exchange"},
            {"TLSv1_1 read Server Key Exchange",
             "TLSv1_1 write Server Key Exchange",
             "TLSv1_1 Server Key Exchange"},
            {"TLSv1_2 read Server Key Exchange",
             "TLSv1_2 write Server Key Exchange",
             "TLSv1_2 Server Key Exchange"},
            {"DTLSv1 read Server Key Exchange",
             "DTLSv1 write Server Key Exchange",
             "DTLSv1 Server Key Exchange"},
            {"DTLSv1_2 read Server Key Exchange",
             "DTLSv1_2 write Server Key Exchange",
             "DTLSv1_2 Server Key Exchange"},
        },
        {
            {"SSLv3 read Server Hello Done",
             "SSLv3 write Server Hello Done",
             "SSLv3 Server Hello Done"},
            {"TLSv1 read Server Hello Done",
             "TLSv1 write Server Hello Done",
             "TLSv1 Server Hello Done"},
            {"TLSv1_1 read Server Hello Done",
             "TLSv1_1 write Server Hello Done",
             "TLSv1_1 Server Hello Done"},
            {"TLSv1_2 read Server Hello Done",
             "TLSv1_2 write Server Hello Done",
             "TLSv1_2 Server Hello Done"},
            {"DTLSv1 read Server Hello Done",
             "DTLSv1 write Server Hello Done",
             "DTLSv1 Server Hello Done"},
            {"DTLSv1_2 read Server Hello Done",
             "DTLSv1_2 write Server Hello Done",
             "DTLSv1_2 Server Hello Done"},
        },
        {
            {"SSLv3 read Server Change CipherSpec",
             "SSLv3 write Server Change CipherSpec",
             "SSLv3 Server Change CipherSpec"},
            {"TLSv1 read Server Change CipherSpec",
             "TLSv1 write Server Change CipherSpec",
             "TLSv1 Server Change CipherSpec"},
            {"TLSv1_1 read Server Change CipherSpec",
             "TLSv1_1 write Server Change CipherSpec",
             "TLSv1_1 Server Change CipherSpec"},
            {"TLSv1_2 read Server Change CipherSpec",
             "TLSv1_2 write Server Change CipherSpec",
             "TLSv1_2 Server Change CipherSpec"},
            {"DTLSv1 read Server Change CipherSpec",
             "DTLSv1 write Server Change CipherSpec",
             "DTLSv1 Server Change CipherSpec"},
            {"DTLSv1_2 read Server Change CipherSpec",
             "DTLSv1_2 write Server Change CipherSpec",
             "DTLSv1_2 Server Change CipherSpec"},
        },
        {
            {"SSLv3 read Server Finished",
             "SSLv3 write Server Finished",
             "SSLv3 Server Finished"},
            {"TLSv1 read Server Finished",
             "TLSv1 write Server Finished",
             "TLSv1 Server Finished"},
            {"TLSv1_1 read Server Finished",
             "TLSv1_1 write Server Finished",
             "TLSv1_1 Server Finished"},
            {"TLSv1_2 read Server Finished",
             "TLSv1_2 write Server Finished",
             "TLSv1_2 Server Finished"},
            {"DTLSv1 read Server Finished",
             "DTLSv1 write Server Finished",
             "DTLSv1 Server Finished"},
            {"DTLSv1_2 read Server Finished",
             "DTLSv1_2 write Server Finished",
             "DTLSv1_2 Server Finished"},
        },
        {
            {"SSLv3 read Client Hello",
             "SSLv3 write Client Hello",
             "SSLv3 Client Hello"},
            {"TLSv1 read Client Hello",
             "TLSv1 write Client Hello",
             "TLSv1 Client Hello"},
            {"TLSv1_1 read Client Hello",
             "TLSv1_1 write Client Hello",
             "TLSv1_1 Client Hello"},
            {"TLSv1_2 read Client Hello",
             "TLSv1_2 write Client Hello",
             "TLSv1_2 Client Hello"},
            {"DTLSv1 read Client Hello",
             "DTLSv1 write Client Hello",
             "DTLSv1 Client Hello"},
            {"DTLSv1_2 read Client Hello",
             "DTLSv1_2 write Client Hello",
             "DTLSv1_2 Client Hello"},
        },
        {
            {"SSLv3 read Client Key Exchange",
             "SSLv3 write Client Key Exchange",
             "SSLv3 Client Key Exchange"},
            {"TLSv1 read Client Key Exchange",
             "TLSv1 write Client Key Exchange",
             "TLSv1 Client Key Exchange"},
            {"TLSv1_1 read Client Key Exchange",
             "TLSv1_1 write Client Key Exchange",
             "TLSv1_1 Client Key Exchange"},
            {"TLSv1_2 read Client Key Exchange",
             "TLSv1_2 write Client Key Exchange",
             "TLSv1_2 Client Key Exchange"},
            {"DTLSv1 read Client Key Exchange",
             "DTLSv1 write Client Key Exchange",
             "DTLSv1 Client Key Exchange"},
            {"DTLSv1_2 read Client Key Exchange",
             "DTLSv1_2 write Client Key Exchange",
             "DTLSv1_2 Client Key Exchange"},
        },
        {
            {"SSLv3 read Client Change CipherSpec",
             "SSLv3 write Client Change CipherSpec",
             "SSLv3 Client Change CipherSpec"},
            {"TLSv1 read Client Change CipherSpec",
             "TLSv1 write Client Change CipherSpec",
             "TLSv1 Client Change CipherSpec"},
            {"TLSv1_1 read Client Change CipherSpec",
             "TLSv1_1 write Client Change CipherSpec",
             "TLSv1_1 Client Change CipherSpec"},
            {"TLSv1_2 read Client Change CipherSpec",
             "TLSv1_2 write Client Change CipherSpec",
             "TLSv1_2 Client Change CipherSpec"},
            {"DTLSv1 read Client Change CipherSpec",
             "DTLSv1 write Client Change CipherSpec",
             "DTLSv1 Client Change CipherSpec"},
            {"DTLSv1_2 read Client Change CipherSpec",
             "DTLSv1_2 write Client Change CipherSpec",
             "DTLSv1_2 Client Change CipherSpec"},
        },
        {
            {"SSLv3 read Client Finished",
             "SSLv3 write Client Finished",
             "SSLv3 Client Finished"},
            {"TLSv1 read Client Finished",
             "TLSv1 write Client Finished",
             "TLSv1 Client Finished"},
            {"TLSv1_1 read Client Finished",
             "TLSv1_1 write Client Finished",
             "TLSv1_1 Client Finished"},
            {"TLSv1_2 read Client Finished",
             "TLSv1_2 write Client Finished",
             "TLSv1_2 Client Finished"},
            {"DTLSv1 read Client Finished",
             "DTLSv1 write Client Finished",
             "DTLSv1 Client Finished"},
            {"DTLSv1_2 read Client Finished",
             "DTLSv1_2 write Client Finished",
             "DTLSv1_2 Client Finished"},
        },
        {
            {"SSLv3 Handshake Done",
             "SSLv3 Handshake Done",
             "SSLv3 Handshake Done"},
            {"TLSv1 Handshake Done",
             "TLSv1 Handshake Done",
             "TLSv1 Handshake Done"},
            {"TLSv1_1 Handshake Done",
             "TLSv1_1 Handshake Done",
             "TLSv1_1 Handshake Done"},
            {"TLSv1_2 Handshake Done",
             "TLSv1_2 Handshake Done",
             "TLSv1_2 Handshake Done"},
            {"DTLSv1 Handshake Done",
             "DTLSv1 Handshake Done",
             "DTLSv1 Handshake Done"},
            {"DTLSv1_2 Handshake Done"
             "DTLSv1_2 Handshake Done"
             "DTLSv1_2 Handshake Done"}
        }
    };
    enum ProtocolVer {
        SSL_V3 = 0,
        TLS_V1,
        TLS_V1_1,
        TLS_V1_2,
        DTLS_V1,
        DTLS_V1_2,
        UNKNOWN = 100
    };

    enum IOMode {
        SS_READ = 0,
        SS_WRITE,
        SS_NEITHER
    };

    enum SslState {
        ss_null_state = 0,
        ss_server_helloverify,
        ss_server_hello,
        ss_sessionticket,
        ss_server_cert,
        ss_server_keyexchange,
        ss_server_hellodone,
        ss_server_changecipherspec,
        ss_server_finished,
        ss_client_hello,
        ss_client_keyexchange,
        ss_client_changecipherspec,
        ss_client_finished,
        ss_handshake_done
    };

    int protocol = 0;
    int cbmode = 0;
    int state = 0;

    WOLFSSL_ENTER("wolfSSL_state_string_long");
    if (ssl == NULL) {
        WOLFSSL_MSG("Null argument passed in");
        return NULL;
    }

    /* Get state of callback */
    if (ssl->cbmode == SSL_CB_MODE_WRITE){
        cbmode =  SS_WRITE;
    } else if (ssl->cbmode == SSL_CB_MODE_READ){
        cbmode =  SS_READ;
    } else {
        cbmode =  SS_NEITHER;
    }

    /* Get protocol version */
    switch (ssl->version.major){
        case SSLv3_MAJOR:
            switch (ssl->version.minor){
                case TLSv1_MINOR:
                    protocol = TLS_V1;
                    break;
                case TLSv1_1_MINOR:
                    protocol = TLS_V1_1;
                    break;
                case TLSv1_2_MINOR:
                    protocol = TLS_V1_2;
                    break;
                case SSLv3_MINOR:
                    protocol = SSL_V3;
                    break;
                default:
                    protocol = UNKNOWN;
            }
            break;
        case DTLS_MAJOR:
            switch (ssl->version.minor){
        case DTLS_MINOR:
            protocol = DTLS_V1;
            break;
        case DTLSv1_2_MINOR:
            protocol = DTLS_V1_2;
            break;
        default:
            protocol = UNKNOWN;
    }
    break;
    default:
        protocol = UNKNOWN;
    }

    /* accept process */
    if (ssl->cbmode == SSL_CB_MODE_READ){
        state = ssl->cbtype;
        switch (state) {
            case hello_verify_request:
                state = ss_server_helloverify;
                break;
            case session_ticket:
                state = ss_sessionticket;
                break;
            case server_hello:
                state = ss_server_hello;
                break;
            case server_hello_done:
                state = ss_server_hellodone;
                break;
            case certificate:
                state = ss_server_cert;
                break;
            case server_key_exchange:
                state = ss_server_keyexchange;
                break;
            case client_hello:
                state = ss_client_hello;
                break;
            case client_key_exchange:
                state = ss_client_keyexchange;
                break;
            case finished:
                if (ssl->options.side == WOLFSSL_SERVER_END)
                    state = ss_client_finished;
                else if (ssl->options.side == WOLFSSL_CLIENT_END)
                    state = ss_server_finished;
                break;
            default:
                WOLFSSL_MSG("Unknown State");
                state = ss_null_state;
        }
    } else {
        /* Send process */
        if (ssl->options.side == WOLFSSL_SERVER_END)
            state = ssl->options.serverState;
        else
            state = ssl->options.clientState;

        switch(state){
            case SERVER_HELLOVERIFYREQUEST_COMPLETE:
                state = ss_server_helloverify;
                break;
            case SERVER_HELLO_COMPLETE:
                state = ss_server_hello;
                break;
            case SERVER_CERT_COMPLETE:
                state = ss_server_cert;
                break;
            case SERVER_KEYEXCHANGE_COMPLETE:
                state = ss_server_keyexchange;
                break;
            case SERVER_HELLODONE_COMPLETE:
                state = ss_server_hellodone;
                break;
            case SERVER_CHANGECIPHERSPEC_COMPLETE:
                state = ss_server_changecipherspec;
                break;
            case SERVER_FINISHED_COMPLETE:
                state = ss_server_finished;
                break;
            case CLIENT_HELLO_COMPLETE:
                state = ss_client_hello;
                break;
            case CLIENT_KEYEXCHANGE_COMPLETE:
                state = ss_client_keyexchange;
                break;
            case CLIENT_CHANGECIPHERSPEC_COMPLETE:
                state = ss_client_changecipherspec;
                break;
            case CLIENT_FINISHED_COMPLETE:
                state = ss_client_finished;
                break;
            case HANDSHAKE_DONE:
                state = ss_handshake_done;
                break;
            default:
                WOLFSSL_MSG("Unknown State");
                state = ss_null_state;
        }
    }

    if (protocol == UNKNOWN)
        return NULL;
    else
        return OUTPUT_STR[state][protocol][cbmode];
}

#ifndef NO_WOLFSSL_STUB
int wolfSSL_PEM_def_callback(char* name, int num, int w, void* key)
{
    (void)name;
    (void)num;
    (void)w;
    (void)key;
    WOLFSSL_STUB("PEM_def_callback");
    return 0;
}
#endif

long wolfSSL_set_options(WOLFSSL* ssl, long op)
{
    word16 haveRSA = 1;
    word16 havePSK = 0;
    int    keySz   = 0;

    WOLFSSL_ENTER("wolfSSL_set_options");

    if (ssl == NULL) {
        return 0;
    }

    /* if SSL_OP_ALL then turn all bug workarounds one */
    if ((op & SSL_OP_ALL) == SSL_OP_ALL) {
        WOLFSSL_MSG("\tSSL_OP_ALL");

        op |= SSL_OP_MICROSOFT_SESS_ID_BUG;
        op |= SSL_OP_NETSCAPE_CHALLENGE_BUG;
        op |= SSL_OP_NETSCAPE_REUSE_CIPHER_CHANGE_BUG;
        op |= SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG;
        op |= SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER;
        op |= SSL_OP_MSIE_SSLV2_RSA_PADDING;
        op |= SSL_OP_SSLEAY_080_CLIENT_DH_BUG;
        op |= SSL_OP_TLS_D5_BUG;
        op |= SSL_OP_TLS_BLOCK_PADDING_BUG;
        op |= SSL_OP_TLS_ROLLBACK_BUG;
        op |= SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS;
    }

    ssl->options.mask |= op;

    /* by default cookie exchange is on with DTLS */
    if ((ssl->options.mask & SSL_OP_COOKIE_EXCHANGE) == SSL_OP_COOKIE_EXCHANGE) {
        WOLFSSL_MSG("\tSSL_OP_COOKIE_EXCHANGE : on by default");
    }

    if ((ssl->options.mask & WOLFSSL_OP_NO_SSLv2) == WOLFSSL_OP_NO_SSLv2) {
        WOLFSSL_MSG("\tWOLFSSL_OP_NO_SSLv2 : wolfSSL does not support SSLv2");
    }

    if ((ssl->options.mask & SSL_OP_NO_TLSv1_3) == SSL_OP_NO_TLSv1_3) {
        WOLFSSL_MSG("\tSSL_OP_NO_TLSv1_3");
        if (ssl->version.minor == TLSv1_3_MINOR)
            ssl->version.minor = TLSv1_2_MINOR;
    }

    if ((ssl->options.mask & SSL_OP_NO_TLSv1_2) == SSL_OP_NO_TLSv1_2) {
        WOLFSSL_MSG("\tSSL_OP_NO_TLSv1_2");
        if (ssl->version.minor == TLSv1_2_MINOR)
            ssl->version.minor = TLSv1_1_MINOR;
    }

    if ((ssl->options.mask & SSL_OP_NO_TLSv1_1) == SSL_OP_NO_TLSv1_1) {
        WOLFSSL_MSG("\tSSL_OP_NO_TLSv1_1");
        if (ssl->version.minor == TLSv1_1_MINOR)
            ssl->version.minor = TLSv1_MINOR;
    }

    if ((ssl->options.mask & SSL_OP_NO_TLSv1) == SSL_OP_NO_TLSv1) {
        WOLFSSL_MSG("\tSSL_OP_NO_TLSv1");
        if (ssl->version.minor == TLSv1_MINOR)
            ssl->version.minor = SSLv3_MINOR;
    }

    if ((ssl->options.mask & SSL_OP_NO_SSLv3) == SSL_OP_NO_SSLv3) {
        WOLFSSL_MSG("\tSSL_OP_NO_SSLv3");
    }

    if ((ssl->options.mask & SSL_OP_NO_COMPRESSION) == SSL_OP_NO_COMPRESSION) {
    #ifdef HAVE_LIBZ
        WOLFSSL_MSG("SSL_OP_NO_COMPRESSION");
        ssl->options.usingCompression = 0;
    #else
        WOLFSSL_MSG("SSL_OP_NO_COMPRESSION: compression not compiled in");
    #endif
    }

    /* in the case of a version change the cipher suites should be reset */
    #ifndef NO_PSK
        havePSK = ssl->options.havePSK;
    #endif
    #ifdef NO_RSA
        haveRSA = 0;
    #endif
    #ifndef NO_CERTS
        keySz = ssl->buffers.keySz;
    #endif
    InitSuites(ssl->suites, ssl->version, keySz, haveRSA, havePSK,
                       ssl->options.haveDH, ssl->options.haveNTRU,
                       ssl->options.haveECDSAsig, ssl->options.haveECC,
                       ssl->options.haveStaticECC, ssl->options.side);

    return ssl->options.mask;
}


long wolfSSL_get_options(const WOLFSSL* ssl)
{
    WOLFSSL_ENTER("wolfSSL_get_options");
    if(ssl == NULL)
        return WOLFSSL_FAILURE;
    return ssl->options.mask;
}

long wolfSSL_clear_options(WOLFSSL* ssl, long opt)
{
    WOLFSSL_ENTER("SSL_clear_options");
    if(ssl == NULL)
        return WOLFSSL_FAILURE;
    ssl->options.mask &= ~opt;
    return ssl->options.mask;
}

/*** TBD ***/
#ifndef NO_WOLFSSL_STUB
WOLFSSL_API long wolfSSL_clear_num_renegotiations(WOLFSSL *s)
{
    (void)s;
    WOLFSSL_STUB("SSL_clear_num_renegotiations");
    return 0;
}
#endif

/*** TBD ***/
#ifndef NO_WOLFSSL_STUB
WOLFSSL_API long wolfSSL_total_renegotiations(WOLFSSL *s)
{
    (void)s;
    WOLFSSL_STUB("SSL_total_renegotiations");
    return 0;
}
#endif

#ifndef NO_DH
long wolfSSL_set_tmp_dh(WOLFSSL *ssl, WOLFSSL_DH *dh)
{
    int pSz, gSz;
    byte *p, *g;
    int ret = 0;

    WOLFSSL_ENTER("wolfSSL_set_tmp_dh");

    if (!ssl || !dh)
        return BAD_FUNC_ARG;

    /* Get needed size for p and g */
    pSz = wolfSSL_BN_bn2bin(dh->p, NULL);
    gSz = wolfSSL_BN_bn2bin(dh->g, NULL);

    if (pSz <= 0 || gSz <= 0)
        return WOLFSSL_FATAL_ERROR;

    p = (byte*)XMALLOC(pSz, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    if (!p)
        return MEMORY_E;

    g = (byte*)XMALLOC(gSz, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    if (!g) {
        XFREE(p, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
        return MEMORY_E;
    }

    pSz = wolfSSL_BN_bn2bin(dh->p, p);
    gSz = wolfSSL_BN_bn2bin(dh->g, g);

    if (pSz >= 0 && gSz >= 0) /* Conversion successful */
        ret = wolfSSL_SetTmpDH(ssl, p, pSz, g, gSz);

    XFREE(p, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    XFREE(g, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);

    return pSz > 0 && gSz > 0 ? ret : WOLFSSL_FATAL_ERROR;
}
#endif /* !NO_DH */


#ifdef HAVE_PK_CALLBACKS
long wolfSSL_set_tlsext_debug_arg(WOLFSSL* ssl, void *arg)
{
    if (ssl == NULL) {
        return WOLFSSL_FAILURE;
    }

    ssl->loggingCtx = arg;
    return WOLFSSL_SUCCESS;
}
#endif /* HAVE_PK_CALLBACKS */

#if defined(OPENSSL_ALL) || defined(WOLFSSL_HAPROXY)
const unsigned char *SSL_SESSION_get0_id_context(const SSL_SESSION *sess, unsigned int *sid_ctx_length)
{
    const byte *c = wolfSSL_SESSION_get_id((SSL_SESSION *)sess, sid_ctx_length);
    return c;
}
#endif

/*** TBD ***/
#ifndef NO_WOLFSSL_STUB
WOLFSSL_API int wolfSSL_sk_SSL_COMP_zero(WOLFSSL_STACK* st)
{
    (void)st;
    WOLFSSL_STUB("wolfSSL_sk_SSL_COMP_zero");
    //wolfSSL_set_options(ssl, SSL_OP_NO_COMPRESSION);
    return WOLFSSL_FAILURE;
}
#endif

#ifdef HAVE_CERTIFICATE_STATUS_REQUEST
long wolfSSL_set_tlsext_status_type(WOLFSSL *s, int type)
{
    WOLFSSL_ENTER("wolfSSL_set_tlsext_status_type");

    if (s == NULL){
        return BAD_FUNC_ARG;
    }

    if (type == TLSEXT_STATUSTYPE_ocsp){
        int r = 0;
        r = TLSX_UseCertificateStatusRequest(&s->extensions, type,
                                                     0, s->heap, s->devId);
        return (long)r;
    } else {
        WOLFSSL_MSG(
       "SSL_set_tlsext_status_type only supports TLSEXT_STATUSTYPE_ocsp type.");
        return SSL_FAILURE;
    }

}
#endif /* HAVE_CERTIFICATE_STATUS_REQUEST */

#ifndef NO_WOLFSSL_STUB
WOLFSSL_API long wolfSSL_get_tlsext_status_exts(WOLFSSL *s, void *arg)
{
    (void)s;
    (void)arg;
    WOLFSSL_STUB("wolfSSL_get_tlsext_status_exts");
    return WOLFSSL_FAILURE;
}
#endif

/*** TBD ***/
#ifndef NO_WOLFSSL_STUB
WOLFSSL_API long wolfSSL_set_tlsext_status_exts(WOLFSSL *s, void *arg)
{
    (void)s;
    (void)arg;
    WOLFSSL_STUB("wolfSSL_set_tlsext_status_exts");
    return WOLFSSL_FAILURE;
}
#endif

/*** TBD ***/
#ifndef NO_WOLFSSL_STUB
WOLFSSL_API long wolfSSL_get_tlsext_status_ids(WOLFSSL *s, void *arg)
{
    (void)s;
    (void)arg;
    WOLFSSL_STUB("wolfSSL_get_tlsext_status_ids");
    return WOLFSSL_FAILURE;
}
#endif

/*** TBD ***/
#ifndef NO_WOLFSSL_STUB
WOLFSSL_API long wolfSSL_set_tlsext_status_ids(WOLFSSL *s, void *arg)
{
    (void)s;
    (void)arg;
    WOLFSSL_STUB("wolfSSL_set_tlsext_status_ids");
    return WOLFSSL_FAILURE;
}
#endif

/*** TBD ***/
#ifndef NO_WOLFSSL_STUB
WOLFSSL_API int SSL_SESSION_set1_id(WOLFSSL_SESSION *s, const unsigned char *sid, unsigned int sid_len)
{
    (void)s;
    (void)sid;
    (void)sid_len;
    WOLFSSL_STUB("SSL_SESSION_set1_id");
    return WOLFSSL_FAILURE;
}
#endif

#ifndef NO_WOLFSSL_STUB
/*** TBD ***/
WOLFSSL_API int SSL_SESSION_set1_id_context(WOLFSSL_SESSION *s, const unsigned char *sid_ctx, unsigned int sid_ctx_len)
{
    (void)s;
    (void)sid_ctx;
    (void)sid_ctx_len;
    WOLFSSL_STUB("SSL_SESSION_set1_id_context");
    return WOLFSSL_FAILURE;
}
#endif

#ifndef NO_WOLFSSL_STUB
/*** TBD ***/
WOLFSSL_API void *X509_get0_tbs_sigalg(const WOLFSSL_X509 *x)
{
    (void)x;
    WOLFSSL_STUB("X509_get0_tbs_sigalg");
    return NULL;
}
#endif

#ifndef NO_WOLFSSL_STUB
/*** TBD ***/
WOLFSSL_API void X509_ALGOR_get0(WOLFSSL_ASN1_OBJECT **paobj, int *pptype, const void **ppval, const void *algor)
{
    (void)paobj;
    (void)pptype;
    (void)ppval;
    (void)algor;
    WOLFSSL_STUB("X509_ALGOR_get0");
}
#endif

#ifndef NO_WOLFSSL_STUB
/*** TBD ***/
WOLFSSL_API void *X509_get_X509_PUBKEY(void * x)
{
    (void)x;
    WOLFSSL_STUB("X509_get_X509_PUBKEY");
    return NULL;
}
#endif

#ifndef NO_WOLFSSL_STUB
/*** TBD ***/
WOLFSSL_API int X509_PUBKEY_get0_param(WOLFSSL_ASN1_OBJECT **ppkalg, const unsigned char **pk, int *ppklen, void **pa, WOLFSSL_EVP_PKEY *pub)
{
    (void)ppkalg;
    (void)pk;
    (void)ppklen;
    (void)pa;
    (void)pub;
    WOLFSSL_STUB("X509_PUBKEY_get0_param");
    return WOLFSSL_FAILURE;
}
#endif

#ifndef NO_WOLFSSL_STUB
/*** TBD ***/
WOLFSSL_API WOLFSSL_EVP_PKEY *wolfSSL_get_privatekey(const WOLFSSL *ssl)
{
    (void)ssl;
    WOLFSSL_STUB("SSL_get_privatekey");
    return NULL;
}
#endif

#ifndef NO_WOLFSSL_STUB
/*** TBD ***/
WOLFSSL_API int i2t_ASN1_OBJECT(char *buf, int buf_len, WOLFSSL_ASN1_OBJECT *a)
{
    (void)buf;
    (void)buf_len;
    (void)a;
    WOLFSSL_STUB("i2t_ASN1_OBJECT");
    return -1;
}
#endif

#if defined(OPENSSL_ALL) || defined(WOLFSSL_HAPROXY)
#ifndef NO_WOLFSSL_STUB
/*** TBD ***/
WOLFSSL_API size_t SSL_get_finished(const WOLFSSL *s, void *buf, size_t count)
{
    (void)s;
    (void)buf;
    (void)count;
    WOLFSSL_STUB("SSL_get_finished");
    return WOLFSSL_FAILURE;
}
#endif

#ifndef NO_WOLFSSL_STUB
/*** TBD ***/
WOLFSSL_API size_t SSL_get_peer_finished(const WOLFSSL *s, void *buf, size_t count)
{
    (void)s;
    (void)buf;
    (void)count;
    WOLFSSL_STUB("SSL_get_peer_finished");
    return WOLFSSL_FAILURE;
}
#endif
#endif /* WOLFSSL_HAPROXY */

#ifndef NO_WOLFSSL_STUB
/*** TBD ***/
WOLFSSL_API void SSL_CTX_set_tmp_dh_callback(WOLFSSL_CTX *ctx, WOLFSSL_DH *(*dh) (WOLFSSL *ssl, int is_export, int keylength))
{
    (void)ctx;
    (void)dh;
    WOLFSSL_STUB("SSL_CTX_set_tmp_dh_callback");
}
#endif

#ifndef NO_WOLFSSL_STUB
/*** TBD ***/
WOLFSSL_API WOLF_STACK_OF(SSL_COMP) *SSL_COMP_get_compression_methods(void)
{
    WOLFSSL_STUB("SSL_COMP_get_compression_methods");
    return NULL;
}
#endif

#ifndef NO_WOLFSSL_STUB
/*** TBD ***/
WOLFSSL_API int wolfSSL_sk_SSL_CIPHER_num(const void * p)
{
    (void)p;
    WOLFSSL_STUB("wolfSSL_sk_SSL_CIPHER_num");
    return -1;
}
#endif

#if !defined(NO_FILESYSTEM)
#ifndef NO_WOLFSSL_STUB
/*** TBD ***/
WOLFSSL_API WOLFSSL_X509 *wolfSSL_PEM_read_X509(FILE *fp, WOLFSSL_X509 **x, pem_password_cb *cb, void *u)
{
    (void)fp;
    (void)x;
    (void)cb;
    (void)u;
    WOLFSSL_STUB("PEM_read_X509");
    return NULL;
}
#endif

#ifndef NO_WOLFSSL_STUB
/*** TBD ***/
WOLFSSL_API WOLFSSL_EVP_PKEY *wolfSSL_PEM_read_PrivateKey(FILE *fp, WOLFSSL_EVP_PKEY **x, pem_password_cb *cb, void *u)
{
    (void)fp;
    (void)x;
    (void)cb;
    (void)u;
    WOLFSSL_STUB("PEM_read_PrivateKey");
    return NULL;
}
#endif
#endif

#ifndef NO_WOLFSSL_STUB
/*** TBD ***/
WOLFSSL_API int X509_STORE_load_locations(WOLFSSL_X509_STORE *ctx, const char *file, const char *dir)
{
    (void)ctx;
    (void)file;
    (void)dir;
    WOLFSSL_STUB("X509_STORE_load_locations");
    return WOLFSSL_FAILURE;
}
#endif

#ifndef NO_WOLFSSL_STUB
/*** TBD ***/
WOLFSSL_API WOLFSSL_CIPHER* wolfSSL_sk_SSL_CIPHER_value(void *ciphers, int idx)
{
    (void)ciphers;
    (void)idx;
    WOLFSSL_STUB("wolfSSL_sk_SSL_CIPHER_value");
    return NULL;
}
#endif

WOLFSSL_API void ERR_load_SSL_strings(void)
{

}

#ifdef HAVE_OCSP
WOLFSSL_API long wolfSSL_get_tlsext_status_ocsp_resp(WOLFSSL *s, unsigned char **resp)
{
    if (s == NULL || resp == NULL)
        return 0;

    *resp = s->ocspResp;
    return s->ocspRespSz;
}

WOLFSSL_API long wolfSSL_set_tlsext_status_ocsp_resp(WOLFSSL *s, unsigned char *resp, int len)
{
    if (s == NULL)
        return WOLFSSL_FAILURE;

    s->ocspResp   = resp;
    s->ocspRespSz = len;

    return WOLFSSL_SUCCESS;
}
#endif /* HAVE_OCSP */

long wolfSSL_get_verify_result(const WOLFSSL *ssl)
{
    if (ssl == NULL) {
        return WOLFSSL_FAILURE;
    }

    return ssl->peerVerifyRet;
}


#ifndef NO_WOLFSSL_STUB
/* shows the number of accepts attempted by CTX in it's lifetime */
long wolfSSL_CTX_sess_accept(WOLFSSL_CTX* ctx)
{
    WOLFSSL_STUB("wolfSSL_CTX_sess_accept");
    (void)ctx;
    return 0;
}
#endif

#ifndef NO_WOLFSSL_STUB
/* shows the number of connects attempted CTX in it's lifetime */
long wolfSSL_CTX_sess_connect(WOLFSSL_CTX* ctx)
{
    WOLFSSL_STUB("wolfSSL_CTX_sess_connect");
    (void)ctx;
    return 0;
}
#endif


#ifndef NO_WOLFSSL_STUB
/* shows the number of accepts completed by CTX in it's lifetime */
long wolfSSL_CTX_sess_accept_good(WOLFSSL_CTX* ctx)
{
    WOLFSSL_STUB("wolfSSL_CTX_sess_accept_good");
    (void)ctx;
    return 0;
}
#endif


#ifndef NO_WOLFSSL_STUB
/* shows the number of connects completed by CTX in it's lifetime */
long wolfSSL_CTX_sess_connect_good(WOLFSSL_CTX* ctx)
{
    WOLFSSL_STUB("wolfSSL_CTX_sess_connect_good");
    (void)ctx;
    return 0;
}
#endif


#ifndef NO_WOLFSSL_STUB
/* shows the number of renegotiation accepts attempted by CTX */
long wolfSSL_CTX_sess_accept_renegotiate(WOLFSSL_CTX* ctx)
{
    WOLFSSL_STUB("wolfSSL_CTX_sess_accept_renegotiate");
    (void)ctx;
    return 0;
}
#endif


#ifndef NO_WOLFSSL_STUB
/* shows the number of renegotiation accepts attempted by CTX */
long wolfSSL_CTX_sess_connect_renegotiate(WOLFSSL_CTX* ctx)
{
    WOLFSSL_STUB("wolfSSL_CTX_sess_connect_renegotiate");
    (void)ctx;
    return 0;
}
#endif


#ifndef NO_WOLFSSL_STUB
long wolfSSL_CTX_sess_hits(WOLFSSL_CTX* ctx)
{
    WOLFSSL_STUB("wolfSSL_CTX_sess_hits");
    (void)ctx;
    return 0;
}
#endif


#ifndef NO_WOLFSSL_STUB
long wolfSSL_CTX_sess_cb_hits(WOLFSSL_CTX* ctx)
{
    WOLFSSL_STUB("wolfSSL_CTX_sess_cb_hits");
    (void)ctx;
    return 0;
}
#endif


#ifndef NO_WOLFSSL_STUB
long wolfSSL_CTX_sess_cache_full(WOLFSSL_CTX* ctx)
{
    WOLFSSL_STUB("wolfSSL_CTX_sess_cache_full");
    (void)ctx;
    return 0;
}
#endif


#ifndef NO_WOLFSSL_STUB
long wolfSSL_CTX_sess_misses(WOLFSSL_CTX* ctx)
{
    WOLFSSL_STUB("wolfSSL_CTX_sess_misses");
    (void)ctx;
    return 0;
}
#endif


#ifndef NO_WOLFSSL_STUB
long wolfSSL_CTX_sess_timeouts(WOLFSSL_CTX* ctx)
{
    WOLFSSL_STUB("wolfSSL_CTX_sess_timeouts");
    (void)ctx;
    return 0;
}
#endif


/* Return the total number of sessions */
long wolfSSL_CTX_sess_number(WOLFSSL_CTX* ctx)
{
    word32 total = 0;

    WOLFSSL_ENTER("wolfSSL_CTX_sess_number");
    (void)ctx;

#ifdef WOLFSSL_SESSION_STATS
    if (wolfSSL_get_session_stats(NULL, &total, NULL, NULL) != SSL_SUCCESS) {
        WOLFSSL_MSG("Error getting session stats");
    }
#else
    WOLFSSL_MSG("Please use macro WOLFSSL_SESSION_STATS for session stats");
#endif

    return (long)total;
}


#ifndef NO_CERTS
long wolfSSL_CTX_add_extra_chain_cert(WOLFSSL_CTX* ctx, WOLFSSL_X509* x509)
{
    byte* chain = NULL;
    long  chainSz = 0;
    int   derSz;
    const byte* der;
    int   ret;
    int   idx = 0;
    DerBuffer *derBuffer = NULL;

    WOLFSSL_ENTER("wolfSSL_CTX_add_extra_chain_cert");

    if (ctx == NULL || x509 == NULL) {
        WOLFSSL_MSG("Bad Argument");
        return WOLFSSL_FAILURE;
    }

    der = wolfSSL_X509_get_der(x509, &derSz);
    if (der == NULL || derSz <= 0) {
        WOLFSSL_MSG("Error getting X509 DER");
        return WOLFSSL_FAILURE;
    }

    if (ctx->certificate == NULL) {
        /* Process buffer makes first certificate the leaf. */
        ret = ProcessBuffer(ctx, der, derSz, WOLFSSL_FILETYPE_ASN1, CERT_TYPE,
                            NULL, NULL, 1);
        if (ret != WOLFSSL_SUCCESS) {
            WOLFSSL_LEAVE("wolfSSL_CTX_add_extra_chain_cert", ret);
            return WOLFSSL_FAILURE;
        }
    }
    else {
        /* TODO: Do this elsewhere. */
        ret = AllocDer(&derBuffer, derSz, CERT_TYPE, ctx->heap);
        if (ret != 0) {
            WOLFSSL_MSG("Memory Error");
            return WOLFSSL_FAILURE;
        }
        XMEMCPY(derBuffer->buffer, der, derSz);
        ret = AddCA(ctx->cm, &derBuffer, WOLFSSL_USER_CA, !ctx->verifyNone);
        if (ret != WOLFSSL_SUCCESS) {
            WOLFSSL_LEAVE("wolfSSL_CTX_add_extra_chain_cert", ret);
            return WOLFSSL_FAILURE;
        }

        /* adding cert to existing chain */
        if (ctx->certChain != NULL && ctx->certChain->length > 0) {
            chainSz += ctx->certChain->length;
        }
        chainSz += OPAQUE24_LEN + derSz;

        chain = (byte*)XMALLOC(chainSz, ctx->heap, DYNAMIC_TYPE_DER);
        if (chain == NULL) {
            WOLFSSL_MSG("Memory Error");
            return WOLFSSL_FAILURE;
        }

        if (ctx->certChain != NULL && ctx->certChain->length > 0) {
            XMEMCPY(chain, ctx->certChain->buffer, ctx->certChain->length);
            idx = ctx->certChain->length;
        }
        c32to24(derSz, chain + idx);
        idx += OPAQUE24_LEN,
        XMEMCPY(chain + idx, der, derSz);
        idx += derSz;
#ifdef WOLFSSL_TLS13
        ctx->certChainCnt++;
#endif

        FreeDer(&ctx->certChain);
        ret = AllocDer(&ctx->certChain, idx, CERT_TYPE, ctx->heap);
        if (ret == 0) {
            XMEMCPY(ctx->certChain->buffer, chain, idx);
        }
    }

    /* on success WOLFSSL_X509 memory is responsibility of ctx */
    wolfSSL_X509_free(x509);
    if (chain != NULL)
        XFREE(chain, ctx->heap, DYNAMIC_TYPE_DER);

    return WOLFSSL_SUCCESS;
}


long wolfSSL_CTX_set_tlsext_status_arg(WOLFSSL_CTX* ctx, void* arg)
{
    if (ctx == NULL || ctx->cm == NULL) {
        return WOLFSSL_FAILURE;
    }

    ctx->cm->ocspIOCtx = arg;
    return WOLFSSL_SUCCESS;
}

#endif /* NO_CERTS */


/* Get the session cache mode for CTX
 *
 * ctx  WOLFSSL_CTX struct to get cache mode from
 *
 * Returns a bit mask that has the session cache mode */
WOLFSSL_API long wolfSSL_CTX_get_session_cache_mode(WOLFSSL_CTX* ctx)
{
    long m = 0;

    WOLFSSL_ENTER("SSL_CTX_set_session_cache_mode");

    if (ctx == NULL) {
        return m;
    }

    if (ctx->sessionCacheOff != 1) {
        m |= SSL_SESS_CACHE_SERVER;
    }

    if (ctx->sessionCacheFlushOff == 1) {
        m |= SSL_SESS_CACHE_NO_AUTO_CLEAR;
    }

#ifdef HAVE_EXT_CACHE
    if (ctx->internalCacheOff == 1) {
        m |= SSL_SESS_CACHE_NO_INTERNAL_STORE;
    }
#endif

    return m;
}


int wolfSSL_CTX_get_read_ahead(WOLFSSL_CTX* ctx)
{
    if (ctx == NULL) {
        return WOLFSSL_FAILURE;
    }

    return ctx->readAhead;
}


int wolfSSL_CTX_set_read_ahead(WOLFSSL_CTX* ctx, int v)
{
    if (ctx == NULL) {
        return WOLFSSL_FAILURE;
    }

    ctx->readAhead = (byte)v;

    return WOLFSSL_SUCCESS;
}


long wolfSSL_CTX_set_tlsext_opaque_prf_input_callback_arg(WOLFSSL_CTX* ctx,
        void* arg)
{
    if (ctx == NULL) {
        return WOLFSSL_FAILURE;
    }

    ctx->userPRFArg = arg;
    return WOLFSSL_SUCCESS;
}


#ifndef NO_DES3
/* 0 on success */
int wolfSSL_DES_set_key(WOLFSSL_const_DES_cblock* myDes,
                                               WOLFSSL_DES_key_schedule* key)
{
#ifdef WOLFSSL_CHECK_DESKEY
    return wolfSSL_DES_set_key_checked(myDes, key);
#else
    wolfSSL_DES_set_key_unchecked(myDes, key);
    return 0;
#endif
}



/* return true in fail case (1) */
static int DES_check(word32 mask, word32 mask2, unsigned char* key)
{
    word32 value[2];

    /* sanity check on length made in wolfSSL_DES_set_key_checked */
    value[0] = mask;
    value[1] = mask2;
    return (XMEMCMP(value, key, sizeof(value)) == 0)? 1: 0;
}


/* check that the key is odd parity and is not a weak key
 * returns -1 if parity is wrong, -2 if weak/null key and 0 on success */
int wolfSSL_DES_set_key_checked(WOLFSSL_const_DES_cblock* myDes,
                                               WOLFSSL_DES_key_schedule* key)
{
    if (myDes == NULL || key == NULL) {
        WOLFSSL_MSG("Bad argument passed to wolfSSL_DES_set_key_checked");
        return -2;
    }
    else {
        word32 i;
        word32 sz = sizeof(WOLFSSL_DES_key_schedule);

        /* sanity check before call to DES_check */
        if (sz != (sizeof(word32) * 2)) {
            WOLFSSL_MSG("Unexpected WOLFSSL_DES_key_schedule size");
            return -2;
        }

        /* check odd parity */
        for (i = 0; i < sz; i++) {
            unsigned char c = *((unsigned char*)myDes + i);
            if (((c & 0x01) ^
                ((c >> 1) & 0x01) ^
                ((c >> 2) & 0x01) ^
                ((c >> 3) & 0x01) ^
                ((c >> 4) & 0x01) ^
                ((c >> 5) & 0x01) ^
                ((c >> 6) & 0x01) ^
                ((c >> 7) & 0x01)) != 1) {
                WOLFSSL_MSG("Odd parity test fail");
                return -1;
            }
        }

        if (wolfSSL_DES_is_weak_key(myDes) == 1) {
            WOLFSSL_MSG("Weak key found");
            return -2;
        }

        /* passed tests, now copy over key */
        XMEMCPY(key, myDes, sizeof(WOLFSSL_const_DES_cblock));

        return 0;
    }
}


/* check is not weak. Weak key list from Nist "Recommendation for the Triple
 * Data Encryption Algorithm (TDEA) Block Cipher"
 *
 * returns 1 if is weak 0 if not
 */
int wolfSSL_DES_is_weak_key(WOLFSSL_const_DES_cblock* key)
{
    word32 mask, mask2;

    WOLFSSL_ENTER("wolfSSL_DES_is_weak_key");

    if (key == NULL) {
        WOLFSSL_MSG("NULL key passed in");
        return 1;
    }

    mask = 0x01010101; mask2 = 0x01010101;
    if (DES_check(mask, mask2, *key)) {
        WOLFSSL_MSG("Weak key found");
        return 1;
    }

    mask = 0xFEFEFEFE; mask2 = 0xFEFEFEFE;
    if (DES_check(mask, mask2, *key)) {
        WOLFSSL_MSG("Weak key found");
        return 1;
    }

    mask = 0xE0E0E0E0; mask2 = 0xF1F1F1F1;
    if (DES_check(mask, mask2, *key)) {
        WOLFSSL_MSG("Weak key found");
        return 1;
    }

    mask = 0x1F1F1F1F; mask2 = 0x0E0E0E0E;
    if (DES_check(mask, mask2, *key)) {
        WOLFSSL_MSG("Weak key found");
        return 1;
    }

    /* semi-weak *key check (list from same Nist paper) */
    mask  = 0x011F011F; mask2 = 0x010E010E;
    if (DES_check(mask, mask2, *key) ||
       DES_check(ByteReverseWord32(mask), ByteReverseWord32(mask2), *key)) {
        WOLFSSL_MSG("Weak key found");
        return 1;
    }

    mask  = 0x01E001E0; mask2 = 0x01F101F1;
    if (DES_check(mask, mask2, *key) ||
       DES_check(ByteReverseWord32(mask), ByteReverseWord32(mask2), *key)) {
        WOLFSSL_MSG("Weak key found");
        return 1;
    }

    mask  = 0x01FE01FE; mask2 = 0x01FE01FE;
    if (DES_check(mask, mask2, *key) ||
       DES_check(ByteReverseWord32(mask), ByteReverseWord32(mask2), *key)) {
        WOLFSSL_MSG("Weak key found");
        return 1;
    }

    mask  = 0x1FE01FE0; mask2 = 0x0EF10EF1;
    if (DES_check(mask, mask2, *key) ||
       DES_check(ByteReverseWord32(mask), ByteReverseWord32(mask2), *key)) {
        WOLFSSL_MSG("Weak key found");
        return 1;
    }

    mask  = 0x1FFE1FFE; mask2 = 0x0EFE0EFE;
    if (DES_check(mask, mask2, *key) ||
       DES_check(ByteReverseWord32(mask), ByteReverseWord32(mask2), *key)) {
        WOLFSSL_MSG("Weak key found");
        return 1;
    }

    return 0;
}


void wolfSSL_DES_set_key_unchecked(WOLFSSL_const_DES_cblock* myDes,
                                               WOLFSSL_DES_key_schedule* key)
{
    if (myDes != NULL && key != NULL) {
        XMEMCPY(key, myDes, sizeof(WOLFSSL_const_DES_cblock));
    }
}


/* Sets the parity of the DES key for use */
void wolfSSL_DES_set_odd_parity(WOLFSSL_DES_cblock* myDes)
{
    word32 i;
    word32 sz = sizeof(WOLFSSL_DES_cblock);

    WOLFSSL_ENTER("wolfSSL_DES_set_odd_parity");

    for (i = 0; i < sz; i++) {
        unsigned char c = *((unsigned char*)myDes + i);
        if ((
            ((c >> 1) & 0x01) ^
            ((c >> 2) & 0x01) ^
            ((c >> 3) & 0x01) ^
            ((c >> 4) & 0x01) ^
            ((c >> 5) & 0x01) ^
            ((c >> 6) & 0x01) ^
            ((c >> 7) & 0x01)) != 1) {
            WOLFSSL_MSG("Setting odd parity bit");
            *((unsigned char*)myDes + i) = *((unsigned char*)myDes + i) | 0x01;
        }
    }
}


#ifdef WOLFSSL_DES_ECB
/* Encrpyt or decrypt input message desa with key and get output in desb.
 * if enc is DES_ENCRYPT,input message is encrypted or
 * if enc is DES_DECRYPT,input message is decrypted.
 * */
void wolfSSL_DES_ecb_encrypt(WOLFSSL_DES_cblock* desa,
             WOLFSSL_DES_cblock* desb, WOLFSSL_DES_key_schedule* key, int enc)
{
    Des myDes;

    WOLFSSL_ENTER("wolfSSL_DES_ecb_encrypt");

    if (desa == NULL || key == NULL || desb == NULL ||
        (enc != DES_ENCRYPT && enc != DES_DECRYPT)) {
        WOLFSSL_MSG("Bad argument passed to wolfSSL_DES_ecb_encrypt");
    } else {
        if (wc_Des_SetKey(&myDes, (const byte*) key,
                           (const byte*) NULL, !enc) != 0) {
            WOLFSSL_MSG("wc_Des_SetKey return error.");
            return;
        }
        if (enc){
            if (wc_Des_EcbEncrypt(&myDes, (byte*) desb, (const byte*) desa,
                        sizeof(WOLFSSL_DES_cblock)) != 0){
                WOLFSSL_MSG("wc_Des_EcbEncrpyt return error.");
            }
        } else {
            if (wc_Des_EcbDecrypt(&myDes, (byte*) desb, (const byte*) desa,
                        sizeof(WOLFSSL_DES_cblock)) != 0){
                WOLFSSL_MSG("wc_Des_EcbDecrpyt return error.");
            }
        }
    }
}
#endif

#endif /* NO_DES3 */

#ifndef NO_RC4
/* Set the key state for Arc4 structure.
 *
 * key  Arc4 structure to use
 * len  length of data buffer
 * data initial state to set Arc4 structure
 */
void wolfSSL_RC4_set_key(WOLFSSL_RC4_KEY* key, int len,
        const unsigned char* data)
{
    typedef char rc4_test[sizeof(WOLFSSL_RC4_KEY) >= sizeof(Arc4) ? 1 : -1];
    (void)sizeof(rc4_test);

    WOLFSSL_ENTER("wolfSSL_RC4_set_key");

    if (key == NULL || len < 0) {
        WOLFSSL_MSG("bad argument passed in");
        return;
    }

    XMEMSET(key, 0, sizeof(WOLFSSL_RC4_KEY));
    wc_Arc4SetKey((Arc4*)key, data, (word32)len);
}


/* Encrypt/decrypt with Arc4 structure.
 *
 * len length of buffer to encrypt/decrypt (in/out)
 * in  buffer to encrypt/decrypt
 * out results of encryption/decryption
 */
void wolfSSL_RC4(WOLFSSL_RC4_KEY* key, size_t len,
        const unsigned char* in, unsigned char* out)
{
    WOLFSSL_ENTER("wolfSSL_RC4");

    if (key == NULL || in == NULL || out == NULL) {
        WOLFSSL_MSG("Bad argument passed in");
        return;
    }

    wc_Arc4Process((Arc4*)key, out, in, (word32)len);
}
#endif /* NO_RC4 */

#ifndef NO_AES

#ifdef WOLFSSL_AES_DIRECT
/* AES encrypt direct, it is expected to be blocks of AES_BLOCK_SIZE for input.
 *
 * input  Data to encrypt
 * output Encrypted data after done
 * key    AES key to use for encryption
 */
void wolfSSL_AES_encrypt(const unsigned char* input, unsigned char* output,
        AES_KEY *key)
{
    WOLFSSL_ENTER("wolfSSL_AES_encrypt");

    if (input == NULL || output == NULL || key == NULL) {
        WOLFSSL_MSG("Null argument passed in");
        return;
    }

    wc_AesEncryptDirect((Aes*)key, output, input);
}


/* AES decrypt direct, it is expected to be blocks of AES_BLOCK_SIZE for input.
 *
 * input  Data to decrypt
 * output Decrypted data after done
 * key    AES key to use for encryption
 */
void wolfSSL_AES_decrypt(const unsigned char* input, unsigned char* output,
        AES_KEY *key)
{
    WOLFSSL_ENTER("wolfSSL_AES_decrypt");

    if (input == NULL || output == NULL || key == NULL) {
        WOLFSSL_MSG("Null argument passed in");
        return;
    }

    wc_AesDecryptDirect((Aes*)key, output, input);
}
#endif /* WOLFSSL_AES_DIRECT */

/* Setup of an AES key to use for encryption.
 *
 * key  key in bytes to use for encryption
 * bits size of key in bits
 * aes  AES structure to initialize
 */
int wolfSSL_AES_set_encrypt_key(const unsigned char *key, const int bits,
        AES_KEY *aes)
{
    typedef char aes_test[sizeof(AES_KEY) >= sizeof(Aes) ? 1 : -1];
    (void)sizeof(aes_test);

    WOLFSSL_ENTER("wolfSSL_AES_set_encrypt_key");

    if (key == NULL || aes == NULL) {
        WOLFSSL_MSG("Null argument passed in");
        return -1;
    }

    XMEMSET(aes, 0, sizeof(AES_KEY));
    if (wc_AesSetKey((Aes*)aes, key, ((bits)/8), NULL, AES_ENCRYPTION) != 0) {
        WOLFSSL_MSG("Error in setting AES key");
        return -1;
    }
    return 0;
}


/* Setup of an AES key to use for decryption.
 *
 * key  key in bytes to use for decryption
 * bits size of key in bits
 * aes  AES structure to initialize
 */
int wolfSSL_AES_set_decrypt_key(const unsigned char *key, const int bits,
        AES_KEY *aes)
{
    typedef char aes_test[sizeof(AES_KEY) >= sizeof(Aes) ? 1 : -1];
    (void)sizeof(aes_test);

    WOLFSSL_ENTER("wolfSSL_AES_set_decrypt_key");

    if (key == NULL || aes == NULL) {
        WOLFSSL_MSG("Null argument passed in");
        return -1;
    }

    XMEMSET(aes, 0, sizeof(AES_KEY));
    if (wc_AesSetKey((Aes*)aes, key, ((bits)/8), NULL, AES_DECRYPTION) != 0) {
        WOLFSSL_MSG("Error in setting AES key");
        return -1;
    }
    return 0;
}


#ifdef HAVE_AES_ECB
/* Encrypt/decrypt a 16 byte block of data using the key passed in.
 *
 * in  buffer to encrypt/decyrpt
 * out buffer to hold result of encryption/decryption
 * key AES structure to use with encryption/decryption
 * enc AES_ENCRPT for encryption and AES_DECRYPT for decryption
 */
void wolfSSL_AES_ecb_encrypt(const unsigned char *in, unsigned char* out,
                             AES_KEY *key, const int enc)
{
    Aes* aes;

    WOLFSSL_ENTER("wolfSSL_AES_ecb_encrypt");

    if (key == NULL || in == NULL || out == NULL) {
        WOLFSSL_MSG("Error, Null argument passed in");
        return;
    }

    aes = (Aes*)key;
    if (enc == AES_ENCRYPT) {
        if (wc_AesEcbEncrypt(aes, out, in, AES_BLOCK_SIZE) != 0) {
            WOLFSSL_MSG("Error with AES CBC encrypt");
        }
    }
    else {
    #ifdef HAVE_AES_DECRYPT
        if (wc_AesEcbDecrypt(aes, out, in, AES_BLOCK_SIZE) != 0) {
            WOLFSSL_MSG("Error with AES CBC decrypt");
        }
    #else
        WOLFSSL_MSG("AES decryption not compiled in");
    #endif
    }
}
#endif /* HAVE_AES_ECB */


/* Encrypt data using key and iv passed in. iv gets updated to most recent iv
 * state after encryptiond/decryption.
 *
 * in  buffer to encrypt/decyrpt
 * out buffer to hold result of encryption/decryption
 * len length of input buffer
 * key AES structure to use with encryption/decryption
 * iv  iv to use with operation
 * enc AES_ENCRPT for encryption and AES_DECRYPT for decryption
 */
void wolfSSL_AES_cbc_encrypt(const unsigned char *in, unsigned char* out,
        size_t len, AES_KEY *key, unsigned char* iv, const int enc)
{
    Aes* aes;

    WOLFSSL_ENTER("wolfSSL_AES_cbc_encrypt");

    if (key == NULL || in == NULL || out == NULL || iv == NULL) {
        WOLFSSL_MSG("Error, Null argument passed in");
        return;
    }

    aes = (Aes*)key;
    if (wc_AesSetIV(aes, (const byte*)iv) != 0) {
        WOLFSSL_MSG("Error with setting iv");
        return;
    }

    if (enc == AES_ENCRYPT) {
        if (wc_AesCbcEncrypt(aes, out, in, (word32)len) != 0) {
            WOLFSSL_MSG("Error with AES CBC encrypt");
        }
    }
    else {
        if (wc_AesCbcDecrypt(aes, out, in, (word32)len) != 0) {
            WOLFSSL_MSG("Error with AES CBC decrypt");
        }
    }

    /* to be compatible copy iv to iv buffer after completing operation */
    XMEMCPY(iv, (byte*)(aes->reg), AES_BLOCK_SIZE);
}


/* Encrypt data using CFB mode with key and iv passed in. iv gets updated to
 * most recent iv state after encryptiond/decryption.
 *
 * in  buffer to encrypt/decyrpt
 * out buffer to hold result of encryption/decryption
 * len length of input buffer
 * key AES structure to use with encryption/decryption
 * iv  iv to use with operation
 * num contains the amount of block used
 * enc AES_ENCRPT for encryption and AES_DECRYPT for decryption
 */
void wolfSSL_AES_cfb128_encrypt(const unsigned char *in, unsigned char* out,
        size_t len, AES_KEY *key, unsigned char* iv, int* num,
        const int enc)
{
#ifndef WOLFSSL_AES_CFB
    WOLFSSL_MSG("CFB mode not enabled please use macro WOLFSSL_AES_CFB");
    (void)in;
    (void)out;
    (void)len;
    (void)key;
    (void)iv;
    (void)num;
    (void)enc;

    return;
#else
    Aes* aes;

    WOLFSSL_ENTER("wolfSSL_AES_cbc_encrypt");
    if (key == NULL || in == NULL || out == NULL || iv == NULL) {
        WOLFSSL_MSG("Error, Null argument passed in");
        return;
    }

    aes = (Aes*)key;
    if (wc_AesSetIV(aes, (const byte*)iv) != 0) {
        WOLFSSL_MSG("Error with setting iv");
        return;
    }

    if (enc == AES_ENCRYPT) {
        if (wc_AesCfbEncrypt(aes, out, in, (word32)len) != 0) {
            WOLFSSL_MSG("Error with AES CBC encrypt");
        }
    }
    else {
        if (wc_AesCfbDecrypt(aes, out, in, (word32)len) != 0) {
            WOLFSSL_MSG("Error with AES CBC decrypt");
        }
    }

    /* to be compatible copy iv to iv buffer after completing operation */
    XMEMCPY(iv, (byte*)(aes->reg), AES_BLOCK_SIZE);

    /* store number of left over bytes to num */
    *num = (aes->left)? AES_BLOCK_SIZE - aes->left : 0;
#endif /* WOLFSSL_AES_CFB */
}
#endif /* NO_AES */

#ifndef NO_WOLFSSL_STUB
int wolfSSL_BIO_printf(WOLFSSL_BIO* bio, const char* format, ...)
{
    (void)bio;
    (void)format;
    WOLFSSL_STUB("BIO_printf");
    return 0;
}
#endif

#ifndef NO_WOLFSSL_STUB
int wolfSSL_ASN1_UTCTIME_print(WOLFSSL_BIO* bio, const WOLFSSL_ASN1_UTCTIME* a)
{
    (void)bio;
    (void)a;
    WOLFSSL_STUB("ASN1_UTCTIME_print");
    return 0;
}
#endif

/* Return the month as a string.
 *
 * n  The number of the month as a two characters (1 based).
 * returns the month as a string.
 */
static INLINE const char* MonthStr(const char* n)
{
    static const char monthStr[12][4] = {
            "Jan", "Feb", "Mar", "Apr", "May", "Jun",
            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    return monthStr[(n[0] - '0') * 10 + (n[1] - '0') - 1];
}

int wolfSSL_ASN1_GENERALIZEDTIME_print(WOLFSSL_BIO* bio,
    const WOLFSSL_ASN1_GENERALIZEDTIME* asnTime)
{
    const char* p = (const char *)(asnTime->data + 2);
    WOLFSSL_ENTER("wolfSSL_ASN1_GENERALIZEDTIME_print");

    if (bio == NULL || asnTime == NULL)
        return BAD_FUNC_ARG;

    /* GetTimeString not always available. */
    wolfSSL_BIO_write(bio, MonthStr(p + 4), 3);
    wolfSSL_BIO_write(bio, " ", 1);
    /* Day */
    wolfSSL_BIO_write(bio, p + 6, 2);
    wolfSSL_BIO_write(bio, " ", 1);
    /* Hour */
    wolfSSL_BIO_write(bio, p + 8, 2);
    wolfSSL_BIO_write(bio, ":", 1);
    /* Min */
    wolfSSL_BIO_write(bio, p + 10, 2);
    wolfSSL_BIO_write(bio, ":", 1);
    /* Secs */
    wolfSSL_BIO_write(bio, p + 12, 2);
    wolfSSL_BIO_write(bio, " ", 1);
    wolfSSL_BIO_write(bio, p, 4);

    return 0;
}

void wolfSSL_ASN1_GENERALIZEDTIME_free(WOLFSSL_ASN1_TIME* asn1Time)
{
    WOLFSSL_ENTER("wolfSSL_ASN1_GENERALIZEDTIME_free");
    if (asn1Time == NULL)
        return;
    XMEMSET(asn1Time->data, 0, sizeof(asn1Time->data));
}

int  wolfSSL_sk_num(WOLF_STACK_OF(WOLFSSL_ASN1_OBJECT)* sk)
{
    if (sk == NULL)
        return 0;
    return (int)sk->num;
}

void* wolfSSL_sk_value(WOLF_STACK_OF(WOLFSSL_ASN1_OBJECT)* sk, int i)
{
    for (; sk != NULL && i > 0; i--)
        sk = sk->next;
    if (sk == NULL)
        return NULL;
    return (void*)sk->data.obj;
}

#endif /* OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA) || defined(HAVE_EXT_CACHE)
/* stunnel 4.28 needs */
void wolfSSL_CTX_sess_set_get_cb(WOLFSSL_CTX* ctx,
                    WOLFSSL_SESSION*(*f)(WOLFSSL*, unsigned char*, int, int*))
{
#ifdef HAVE_EXT_CACHE
    ctx->get_sess_cb = f;
#else
    (void)ctx;
    (void)f;
#endif
}

void wolfSSL_CTX_sess_set_new_cb(WOLFSSL_CTX* ctx,
                             int (*f)(WOLFSSL*, WOLFSSL_SESSION*))
{
#ifdef HAVE_EXT_CACHE
    ctx->new_sess_cb = f;
#else
    (void)ctx;
    (void)f;
#endif
}

void wolfSSL_CTX_sess_set_remove_cb(WOLFSSL_CTX* ctx, void (*f)(WOLFSSL_CTX*,
                                                        WOLFSSL_SESSION*))
{
#ifdef HAVE_EXT_CACHE
    ctx->rem_sess_cb = f;
#else
    (void)ctx;
    (void)f;
#endif
}
#endif /* OPENSSL_EXTRA || HAVE_EXT_CACHE */

#ifdef OPENSSL_EXTRA

/*
 *
 * Note: It is expected that the importing and exporting function have been
 *       built with the same settings. For example if session tickets was
 *       enabled with the wolfSSL library exporting a session then it is
 *       expected to be turned on with the wolfSSL library importing the session.
 */
int wolfSSL_i2d_SSL_SESSION(WOLFSSL_SESSION* sess, unsigned char** p)
{
    int size = 0;
#ifdef HAVE_EXT_CACHE
    int idx = 0;
#ifdef SESSION_CERTS
    int i;
#endif
    unsigned char *data;

    if (sess == NULL) {
        return BAD_FUNC_ARG;
    }

    /* bornOn | timeout | sessionID len | sessionID | masterSecret | haveEMS */
    size += OPAQUE32_LEN + OPAQUE32_LEN + OPAQUE8_LEN + sess->sessionIDSz +
            SECRET_LEN + OPAQUE8_LEN;
#ifdef SESSION_CERTS
    /* Peer chain */
    size += OPAQUE8_LEN;
    for (i = 0; i < sess->chain.count; i++)
        size += OPAQUE16_LEN + sess->chain.certs[i].length;
    /* Protocol version + cipher suite */
    size += OPAQUE16_LEN + OPAQUE16_LEN;
#endif
#ifndef NO_CLIENT_CACHE
    /* ServerID len | ServerID */
    size += OPAQUE16_LEN + sess->idLen;
#endif
#ifdef HAVE_SESSION_TICKET
    /* ticket len | ticket */
    size += OPAQUE16_LEN + sess->ticketLen;
#endif
#ifdef OPENSSL_EXTRA
    /* session context ID len | session context ID */
    size += OPAQUE8_LEN + sess->sessionCtxSz;
#endif

    if (p != NULL) {
        if (*p == NULL)
            *p = (unsigned char*)XMALLOC(size, NULL, DYNAMIC_TYPE_OPENSSL);
        if (*p == NULL)
            return 0;
        data = *p;

        c32toa(sess->bornOn, data + idx); idx += OPAQUE32_LEN;
        c32toa(sess->timeout, data + idx); idx += OPAQUE32_LEN;
        data[idx++] = sess->sessionIDSz;
        XMEMCPY(data + idx, sess->sessionID, sess->sessionIDSz);
        idx += sess->sessionIDSz;
        XMEMCPY(data + idx, sess->masterSecret, SECRET_LEN); idx += SECRET_LEN;
        data[idx++] = (byte)sess->haveEMS;
#ifdef SESSION_CERTS
        data[idx++] = (byte)sess->chain.count;
        for (i = 0; i < sess->chain.count; i++) {
            c16toa((word16)sess->chain.certs[i].length, data + idx);
            idx += OPAQUE16_LEN;
            XMEMCPY(data + idx, sess->chain.certs[i].buffer,
                    sess->chain.certs[i].length);
            idx += sess->chain.certs[i].length;
        }
        data[idx++] = sess->version.major;
        data[idx++] = sess->version.minor;
        data[idx++] = sess->cipherSuite0;
        data[idx++] = sess->cipherSuite;
#endif
#ifndef NO_CLIENT_CACHE
        c16toa(sess->idLen, data + idx); idx += OPAQUE16_LEN;
        XMEMCPY(data + idx, sess->serverID, sess->idLen);
        idx += sess->idLen;
#endif
#ifdef HAVE_SESSION_TICKET
        c16toa(sess->ticketLen, data + idx); idx += OPAQUE16_LEN;
        XMEMCPY(data + idx, sess->ticket, sess->ticketLen);
        idx += sess->ticketLen;
#endif
#ifdef OPENSSL_EXTRA
        data[idx++] = sess->sessionCtxSz;
        XMEMCPY(data + idx, sess->sessionCtx, sess->sessionCtxSz);
        idx += sess->sessionCtxSz;
#endif
    }
#endif

    (void)sess;
    (void)p;
#ifdef HAVE_EXT_CACHE
    (void)idx;
#endif

    return size;
}


/* TODO: no function to free new session.
 *
 * Note: It is expected that the importing and exporting function have been
 *       built with the same settings. For example if session tickets was
 *       enabled with the wolfSSL library exporting a session then it is
 *       expected to be turned on with the wolfSSL library importing the session.
 */
WOLFSSL_SESSION* wolfSSL_d2i_SSL_SESSION(WOLFSSL_SESSION** sess,
                                const unsigned char** p, long i)
{
    WOLFSSL_SESSION* s = NULL;
    int ret = 0;
#if defined(HAVE_EXT_CACHE)
    int idx;
    byte* data;
#ifdef SESSION_CERTS
    int j;
    word16 length;
#endif
#endif

    (void)p;
    (void)i;
    (void)ret;

    if (sess != NULL)
        s = *sess;

#ifdef HAVE_EXT_CACHE
    if (p == NULL || *p == NULL)
        return NULL;

    if (s == NULL) {
        s = (WOLFSSL_SESSION*)XMALLOC(sizeof(WOLFSSL_SESSION), NULL,
                                      DYNAMIC_TYPE_OPENSSL);
        if (s == NULL)
            return NULL;
        XMEMSET(s, 0, sizeof(WOLFSSL_SESSION));
        s->isAlloced = 1;
#ifdef HAVE_SESSION_TICKET
        s->isDynamic = 0;
#endif
    }

    idx = 0;
    data = (byte*)*p;

    /* bornOn | timeout | sessionID len */
    if (i < OPAQUE32_LEN + OPAQUE32_LEN + OPAQUE8_LEN) {
        ret = BUFFER_ERROR;
        goto end;
    }
    ato32(data + idx, &s->bornOn); idx += OPAQUE32_LEN;
    ato32(data + idx, &s->timeout); idx += OPAQUE32_LEN;
    s->sessionIDSz = data[idx++];

    /* sessionID | secret | haveEMS */
    if (i - idx < s->sessionIDSz + SECRET_LEN + OPAQUE8_LEN) {
        ret = BUFFER_ERROR;
        goto end;
    }
    XMEMCPY(s->sessionID, data + idx, s->sessionIDSz);
    idx  += s->sessionIDSz;
    XMEMCPY(s->masterSecret, data + idx, SECRET_LEN); idx += SECRET_LEN;
    s->haveEMS = data[idx++];

#ifdef SESSION_CERTS
    /* Certificate chain */
    if (i - idx == 0) {
        ret = BUFFER_ERROR;
        goto end;
    }
    s->chain.count = data[idx++];
    for (j = 0; j < s->chain.count; j++) {
        if (i - idx < OPAQUE16_LEN) {
            ret = BUFFER_ERROR;
            goto end;
        }
        ato16(data + idx, &length); idx += OPAQUE16_LEN;
        s->chain.certs[j].length = length;
        if (i - idx < length) {
            ret = BUFFER_ERROR;
            goto end;
        }
        XMEMCPY(s->chain.certs[j].buffer, data + idx, length);
        idx += length;
    }

    /* Protocol Version | Cipher suite */
    if (i - idx < OPAQUE16_LEN + OPAQUE16_LEN) {
        ret = BUFFER_ERROR;
        goto end;
    }
    s->version.major = data[idx++];
    s->version.minor = data[idx++];
    s->cipherSuite0 = data[idx++];
    s->cipherSuite = data[idx++];
#endif
#ifndef NO_CLIENT_CACHE
    /* ServerID len */
    if (i - idx < OPAQUE16_LEN) {
        ret = BUFFER_ERROR;
        goto end;
    }
    ato16(data + idx, &s->idLen); idx += OPAQUE16_LEN;

    /* ServerID */
    if (i - idx < s->idLen) {
        ret = BUFFER_ERROR;
        goto end;
    }
    XMEMCPY(s->serverID, data + idx, s->idLen); idx += s->idLen;
#endif
#ifdef HAVE_SESSION_TICKET
    /* ticket len */
    if (i - idx < OPAQUE16_LEN) {
        ret = BUFFER_ERROR;
        goto end;
    }
    ato16(data + idx, &s->ticketLen); idx += OPAQUE16_LEN;

    /* Dispose of ol dynamic ticket and ensure space for new ticket. */
    if (s->isDynamic)
        XFREE(s->ticket, NULL, DYNAMIC_TYPE_SESSION_TICK);
    if (s->ticketLen <= SESSION_TICKET_LEN)
        s->ticket = s->staticTicket;
    else {
        s->ticket = (byte*)XMALLOC(s->ticketLen, NULL,
                                   DYNAMIC_TYPE_SESSION_TICK);
        if (s->ticket == NULL) {
            ret = MEMORY_ERROR;
            goto end;
        }
        s->isDynamic = 1;
    }

    /* ticket */
    if (i - idx < s->ticketLen) {
        ret = BUFFER_ERROR;
        goto end;
    }
    XMEMCPY(s->ticket, data + idx, s->ticketLen); idx += s->ticketLen;
#endif
#ifdef OPENSSL_EXTRA
    /* byte for length of session context ID */
    if (i - idx < OPAQUE8_LEN) {
        ret = BUFFER_ERROR;
        goto end;
    }
    s->sessionCtxSz = data[idx++];

    /* app session context ID */
    if (i - idx < s->sessionCtxSz) {
        ret = BUFFER_ERROR;
        goto end;
    }
    XMEMCPY(s->sessionCtx, data + idx, s->sessionCtxSz); idx += s->sessionCtxSz;
#endif
    (void)idx;

    if (sess != NULL)
        *sess = s;

    *p += idx;

end:
    if (ret != 0 && (sess == NULL || *sess != s))
        wolfSSL_SESSION_free(s);
#endif
    return s;
}


long wolfSSL_SESSION_get_timeout(const WOLFSSL_SESSION* sess)
{
    WOLFSSL_ENTER("wolfSSL_SESSION_get_timeout");
    return sess->timeout;
}


long wolfSSL_SESSION_get_time(const WOLFSSL_SESSION* sess)
{
    WOLFSSL_ENTER("wolfSSL_SESSION_get_time");
    return sess->bornOn;
}


#endif /* OPENSSL_EXTRA */


#ifdef KEEP_PEER_CERT
char*  wolfSSL_X509_get_subjectCN(WOLFSSL_X509* x509)
{
    if (x509 == NULL)
        return NULL;

    return x509->subjectCN;
}
#endif /* KEEP_PEER_CERT */

#ifdef OPENSSL_EXTRA

#if defined(FORTRESS) && !defined(NO_FILESYSTEM)
int wolfSSL_cmp_peer_cert_to_file(WOLFSSL* ssl, const char *fname)
{
    int ret = WOLFSSL_FATAL_ERROR;

    WOLFSSL_ENTER("wolfSSL_cmp_peer_cert_to_file");
    if (ssl != NULL && fname != NULL)
    {
    #ifdef WOLFSSL_SMALL_STACK
        byte           staticBuffer[1]; /* force heap usage */
    #else
        byte           staticBuffer[FILE_BUFFER_SIZE];
    #endif
        byte*          myBuffer  = staticBuffer;
        int            dynamic   = 0;
        XFILE          file      = XBADFILE;
        size_t         sz        = 0;
        WOLFSSL_CTX*   ctx       = ssl->ctx;
        WOLFSSL_X509*  peer_cert = &ssl->peerCert;
        DerBuffer*     fileDer = NULL;

        file = XFOPEN(fname, "rb");
        if (file == XBADFILE)
            return WOLFSSL_BAD_FILE;

        XFSEEK(file, 0, XSEEK_END);
        sz = XFTELL(file);
        XREWIND(file);

        if (sz > (long)sizeof(staticBuffer)) {
            WOLFSSL_MSG("Getting dynamic buffer");
            myBuffer = (byte*)XMALLOC(sz, ctx->heap, DYNAMIC_TYPE_FILE);
            dynamic = 1;
        }


        if ((myBuffer != NULL) &&
            (sz > 0) &&
            (XFREAD(myBuffer, 1, sz, file) == sz) &&
            (PemToDer(myBuffer, (long)sz, CERT_TYPE,
                      &fileDer, ctx->heap, NULL, NULL) == 0) &&
            (fileDer->length != 0) &&
            (fileDer->length == peer_cert->derCert->length) &&
            (XMEMCMP(peer_cert->derCert->buffer, fileDer->buffer,
                                                fileDer->length) == 0))
        {
            ret = 0;
        }

        FreeDer(&fileDer);

        if (dynamic)
            XFREE(myBuffer, ctx->heap, DYNAMIC_TYPE_FILE);

        XFCLOSE(file);
    }

    return ret;
}
#endif
#endif /* OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA) || \
    (defined(OPENSSL_EXTRA_X509_SMALL) && !defined(NO_RSA))
static WC_RNG globalRNG;
static int initGlobalRNG = 0;
#endif

#ifdef OPENSSL_EXTRA

/* Not thread safe! Can be called multiple times.
 * Checks if the global RNG has been created. If not then one is created.
 *
 * Returns SSL_SUCCESS when no error is encountered.
 */
static int wolfSSL_RAND_Init(void)
{
    if (initGlobalRNG == 0) {
        if (wc_InitRng(&globalRNG) < 0) {
            WOLFSSL_MSG("wolfSSL Init Global RNG failed");
            return 0;
        }
        initGlobalRNG = 1;
    }

    return SSL_SUCCESS;
}


/* SSL_SUCCESS on ok */
int wolfSSL_RAND_seed(const void* seed, int len)
{

    WOLFSSL_MSG("wolfSSL_RAND_seed");

    (void)seed;
    (void)len;

    return wolfSSL_RAND_Init();
}


/* Returns the path for reading seed data from.
 * Uses the env variable $RANDFILE first if set, if not then used $HOME/.rnd
 *
 * Note uses stdlib by default unless XGETENV macro is overwritten
 *
 * fname buffer to hold path
 * len   length of fname buffer
 *
 * Returns a pointer to fname on success and NULL on failure
 */
const char* wolfSSL_RAND_file_name(char* fname, unsigned long len)
{
#ifndef NO_FILESYSTEM
    char* rt;
    char ap[] = "/.rnd";

    WOLFSSL_ENTER("wolfSSL_RAND_file_name");

    if (fname == NULL) {
        return NULL;
    }

    XMEMSET(fname, 0, len);
    /* if access to stdlib.h */
    if ((rt = XGETENV("RANDFILE")) != NULL) {
        if (len > XSTRLEN(rt)) {
            XMEMCPY(fname, rt, XSTRLEN(rt));
        }
        else {
            WOLFSSL_MSG("RANDFILE too large for buffer");
            rt = NULL;
        }
    }

    /* $RANDFILE was not set or is too large, check $HOME */
    if (rt == NULL) {
        WOLFSSL_MSG("Environment variable RANDFILE not set");
        if ((rt = XGETENV("HOME")) == NULL) {
            WOLFSSL_MSG("Environment variable HOME not set");
            return NULL;
        }

        if (len > XSTRLEN(rt) +  XSTRLEN(ap)) {
            fname[0] = '\0';
            XSTRNCAT(fname, rt, len);
            XSTRNCAT(fname, ap, len - XSTRLEN(rt));
            return fname;
        }
        else {
            WOLFSSL_MSG("HOME too large for buffer");
            return NULL;
        }
    }

    return fname;
#else
    /* no filesystem defined */
    WOLFSSL_ENTER("wolfSSL_RAND_file_name");
    WOLFSSL_MSG("No filesystem feature enabled, not compiled in");
    (void)fname;
    (void)len;
    return NULL;
#endif
}


/* Writes 1024 bytes from the RNG to the given file name.
 *
 * fname name of file to write to
 *
 * Returns the number of bytes writen
 */
int wolfSSL_RAND_write_file(const char* fname)
{
    int bytes = 0;

    WOLFSSL_ENTER("RAND_write_file");

    if (fname == NULL) {
        return SSL_FAILURE;
    }

#ifndef NO_FILESYSTEM
    {
    #ifndef WOLFSSL_SMALL_STACK
        unsigned char buf[1024];
    #else
        unsigned char* buf = (unsigned char *)XMALLOC(1024, NULL,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
        if (buf == NULL) {
            WOLFSSL_MSG("malloc failed");
            return SSL_FAILURE;
        }
    #endif
        bytes = 1024; /* default size of buf */

        if (initGlobalRNG == 0 && wolfSSL_RAND_Init() != SSL_SUCCESS) {
            WOLFSSL_MSG("No RNG to use");
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(buf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        #endif
            return 0;
        }

        if (wc_RNG_GenerateBlock(&globalRNG, buf, bytes) != 0) {
            WOLFSSL_MSG("Error generating random buffer");
            bytes = 0;
        }
        else {
            XFILE f;

            f = XFOPEN(fname, "wb");
            if (f == NULL) {
                WOLFSSL_MSG("Error opening the file");
                bytes = 0;
            }
            else {
                XFWRITE(buf, 1, bytes, f);
                XFCLOSE(f);
            }
        }
        ForceZero(buf, bytes);
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(buf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif
    }
#endif

    return bytes;
}

#ifndef FREERTOS_TCP

/* These constant values are protocol values made by egd */
#if defined(USE_WOLFSSL_IO) && !defined(USE_WINDOWS_API)
    #define WOLFSSL_EGD_NBLOCK 0x01
    #include <sys/un.h>
#endif

/* at compile time check for HASH DRBG and throw warning if not found */
#ifndef HAVE_HASHDRBG
    #warning HAVE_HASHDRBG is needed for wolfSSL_RAND_egd to seed
#endif

/* This collects entropy from the path nm and seeds the global PRNG with it.
 * Makes a call to wolfSSL_RAND_Init which is not thread safe.
 *
 * nm is the file path to the egd server
 *
 * Returns the number of bytes read.
 */
int wolfSSL_RAND_egd(const char* nm)
{
#if defined(USE_WOLFSSL_IO) && !defined(USE_WINDOWS_API) && !defined(HAVE_FIPS)
    struct sockaddr_un rem;
    int fd;
    int ret = WOLFSSL_SUCCESS;
    word32 bytes = 0;
    word32 idx   = 0;
#ifndef WOLFSSL_SMALL_STACK
    unsigned char buf[256];
#else
    unsigned char* buf;
    buf = (unsigned char*)XMALLOC(256, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (buf == NULL) {
        WOLFSSL_MSG("Not enough memory");
        return WOLFSSL_FATAL_ERROR;
    }
#endif

    if (nm == NULL) {
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(buf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif
        return WOLFSSL_FATAL_ERROR;
    }

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        WOLFSSL_MSG("Error creating socket");
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(buf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif
        return WOLFSSL_FATAL_ERROR;
    }
    if (ret == WOLFSSL_SUCCESS) {
        rem.sun_family = AF_UNIX;
        XSTRNCPY(rem.sun_path, nm, sizeof(rem.sun_path));
        rem.sun_path[sizeof(rem.sun_path)-1] = '\0';
    }

    /* connect to egd server */
    if (ret == WOLFSSL_SUCCESS) {
        if (connect(fd, (struct sockaddr*)&rem, sizeof(struct sockaddr_un))
                == -1) {
            WOLFSSL_MSG("error connecting to egd server");
            ret = WOLFSSL_FATAL_ERROR;
        }
    }

    while (ret == WOLFSSL_SUCCESS && bytes < 255 && idx + 2 < 256) {
        if (ret == WOLFSSL_SUCCESS) {
            buf[idx]     = WOLFSSL_EGD_NBLOCK;
            buf[idx + 1] = 255 - bytes; /* request 255 bytes from server */
            ret = (int)write(fd, buf + idx, 2);
            if (ret <= 0 || ret != 2) {
                if (errno == EAGAIN) {
                    ret = WOLFSSL_SUCCESS;
                    continue;
                }
                WOLFSSL_MSG("error requesting entropy from egd server");
                ret = WOLFSSL_FATAL_ERROR;
                break;
            }
        }

        /* attempting to read */
        buf[idx] = 0;
        ret = (int)read(fd, buf + idx, 256 - bytes);
        if (ret == 0) {
            WOLFSSL_MSG("error reading entropy from egd server");
            ret = WOLFSSL_FATAL_ERROR;
            break;
        }
        if (ret > 0 && buf[idx] > 0) {
            bytes += buf[idx]; /* egd stores amount sent in first byte */
            if (bytes + idx > 255 || buf[idx] > ret) {
                WOLFSSL_MSG("Buffer error");
                ret = WOLFSSL_FATAL_ERROR;
                break;
            }
            XMEMMOVE(buf + idx, buf + idx + 1, buf[idx]);
            idx = bytes;
            ret = WOLFSSL_SUCCESS;
            if (bytes >= 255) {
                break;
            }
        }
        else {
            if (errno == EAGAIN || errno == EINTR) {
                WOLFSSL_MSG("EGD would read");
                ret = WOLFSSL_SUCCESS; /* try again */
            }
            else if (buf[idx] == 0) {
                /* if egd returned 0 then there is no more entropy to be had.
                   Do not try more reads. */
                ret = WOLFSSL_SUCCESS;
                break;
            }
            else {
                WOLFSSL_MSG("Error with read");
                ret = WOLFSSL_FATAL_ERROR;
            }
        }
    }

    if (bytes > 0 && ret == WOLFSSL_SUCCESS) {
        wolfSSL_RAND_Init(); /* call to check global RNG is created */
        if (wc_RNG_DRBG_Reseed(&globalRNG, (const byte*) buf, bytes)
                != 0) {
            WOLFSSL_MSG("Error with reseeding DRBG structure");
            ret = WOLFSSL_FATAL_ERROR;
        }
        #ifdef SHOW_SECRETS
        { /* print out entropy found */
            word32 i;
            printf("EGD Entropy = ");
            for (i = 0; i < bytes; i++) {
                printf("%02X", buf[i]);
            }
            printf("\n");
        }
        #endif
    }

    ForceZero(buf, bytes);
    #ifdef WOLFSSL_SMALL_STACK
    XFREE(buf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif
    close(fd);

    if (ret == WOLFSSL_SUCCESS) {
        return bytes;
    }
    else {
        return ret;
    }
#else /* defined(USE_WOLFSSL_IO) && !defined(USE_WINDOWS_API) && !HAVE_FIPS */
    WOLFSSL_MSG("Type of socket needed is not available");
    WOLFSSL_MSG("\tor using FIPS mode where RNG API is not available");
    (void)nm;

    return WOLFSSL_FATAL_ERROR;
#endif /* defined(USE_WOLFSSL_IO) && !defined(USE_WINDOWS_API) */
}

#endif /* !FREERTOS_TCP */

void wolfSSL_RAND_Cleanup(void)
{
    WOLFSSL_ENTER("wolfSSL_RAND_Cleanup()");

    if (initGlobalRNG != 0) {
        wc_FreeRng(&globalRNG);
        initGlobalRNG = 0;
    }
}


int wolfSSL_RAND_pseudo_bytes(unsigned char* buf, int num)
{
    return wolfSSL_RAND_bytes(buf, num);
}


/* SSL_SUCCESS on ok */
int wolfSSL_RAND_bytes(unsigned char* buf, int num)
{
    int     ret = 0;
    int     initTmpRng = 0;
    WC_RNG* rng = NULL;
#ifdef WOLFSSL_SMALL_STACK
    WC_RNG* tmpRNG = NULL;
#else
    WC_RNG  tmpRNG[1];
#endif

    WOLFSSL_ENTER("wolfSSL_RAND_bytes");

#ifdef WOLFSSL_SMALL_STACK
    tmpRNG = (WC_RNG*)XMALLOC(sizeof(WC_RNG), NULL, DYNAMIC_TYPE_RNG);
    if (tmpRNG == NULL)
        return ret;
#endif

    if (wc_InitRng(tmpRNG) == 0) {
        rng = tmpRNG;
        initTmpRng = 1;
    }
    else if (initGlobalRNG)
        rng = &globalRNG;

    if (rng) {
        if (wc_RNG_GenerateBlock(rng, buf, num) != 0)
            WOLFSSL_MSG("Bad wc_RNG_GenerateBlock");
        else
            ret = WOLFSSL_SUCCESS;
    }

    if (initTmpRng)
        wc_FreeRng(tmpRNG);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(tmpRNG, NULL, DYNAMIC_TYPE_RNG);
#endif

    return ret;
}


int wolfSSL_RAND_poll()
{
    byte  entropy[16];
    int  ret = 0;
    word32 entropy_sz = 16;

    WOLFSSL_ENTER("wolfSSL_RAND_poll");
    if (initGlobalRNG == 0){
        WOLFSSL_MSG("Global RNG no Init");
        return  WOLFSSL_FAILURE;
    }
    ret = wc_GenerateSeed(&globalRNG.seed, entropy, entropy_sz);
    if (ret != 0){
        WOLFSSL_MSG("Bad wc_RNG_GenerateBlock");
        ret = WOLFSSL_FAILURE;
    }else
        ret = WOLFSSL_SUCCESS;

    return ret;
}

WOLFSSL_BN_CTX* wolfSSL_BN_CTX_new(void)
{
    static int ctx;  /* wolfcrypt doesn't now need ctx */

    WOLFSSL_MSG("wolfSSL_BN_CTX_new");
    return (WOLFSSL_BN_CTX*)&ctx;

}

void wolfSSL_BN_CTX_init(WOLFSSL_BN_CTX* ctx)
{
    (void)ctx;
    WOLFSSL_MSG("wolfSSL_BN_CTX_init");
}


void wolfSSL_BN_CTX_free(WOLFSSL_BN_CTX* ctx)
{
    (void)ctx;
    WOLFSSL_MSG("wolfSSL_BN_CTX_free");
    /* do free since static ctx that does nothing */
}
#endif /* OPENSSL_EXTRA */


#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
static void InitwolfSSL_BigNum(WOLFSSL_BIGNUM* bn)
{
    if (bn) {
        XMEMSET(bn, 0, sizeof(WOLFSSL_BIGNUM));
        bn->neg      = 0;
        bn->internal = NULL;
    }
}

WOLFSSL_BIGNUM* wolfSSL_BN_new(void)
{
    WOLFSSL_BIGNUM* external;
    mp_int*        mpi;

    WOLFSSL_MSG("wolfSSL_BN_new");

    mpi = (mp_int*) XMALLOC(sizeof(mp_int), NULL, DYNAMIC_TYPE_BIGINT);
    if (mpi == NULL) {
        WOLFSSL_MSG("wolfSSL_BN_new malloc mpi failure");
        return NULL;
    }

    external = (WOLFSSL_BIGNUM*) XMALLOC(sizeof(WOLFSSL_BIGNUM), NULL,
                                        DYNAMIC_TYPE_BIGINT);
    if (external == NULL) {
        WOLFSSL_MSG("wolfSSL_BN_new malloc WOLFSSL_BIGNUM failure");
        XFREE(mpi, NULL, DYNAMIC_TYPE_BIGINT);
        return NULL;
    }

    InitwolfSSL_BigNum(external);
    external->internal = mpi;
    if (mp_init(mpi) != MP_OKAY) {
        wolfSSL_BN_free(external);
        return NULL;
    }

    return external;
}


void wolfSSL_BN_free(WOLFSSL_BIGNUM* bn)
{
    WOLFSSL_MSG("wolfSSL_BN_free");
    if (bn) {
        if (bn->internal) {
            mp_forcezero((mp_int*)bn->internal);
            XFREE(bn->internal, NULL, DYNAMIC_TYPE_BIGINT);
            bn->internal = NULL;
        }
        XFREE(bn, NULL, DYNAMIC_TYPE_BIGINT);
        bn = NULL;
    }
}
#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */

#ifdef OPENSSL_EXTRA

void wolfSSL_BN_clear_free(WOLFSSL_BIGNUM* bn)
{
    WOLFSSL_MSG("wolfSSL_BN_clear_free");

    wolfSSL_BN_free(bn);
}


/* WOLFSSL_SUCCESS on ok */
int wolfSSL_BN_sub(WOLFSSL_BIGNUM* r, const WOLFSSL_BIGNUM* a,
                  const WOLFSSL_BIGNUM* b)
{
    WOLFSSL_MSG("wolfSSL_BN_sub");

    if (r == NULL || a == NULL || b == NULL)
        return 0;

    if (mp_sub((mp_int*)a->internal,(mp_int*)b->internal,
               (mp_int*)r->internal) == MP_OKAY)
        return WOLFSSL_SUCCESS;

    WOLFSSL_MSG("wolfSSL_BN_sub mp_sub failed");
    return 0;
}

/* WOLFSSL_SUCCESS on ok */
int wolfSSL_BN_mod(WOLFSSL_BIGNUM* r, const WOLFSSL_BIGNUM* a,
                  const WOLFSSL_BIGNUM* b, const WOLFSSL_BN_CTX* c)
{
    (void)c;
    WOLFSSL_MSG("wolfSSL_BN_mod");

    if (r == NULL || a == NULL || b == NULL)
        return 0;

    if (mp_mod((mp_int*)a->internal,(mp_int*)b->internal,
               (mp_int*)r->internal) == MP_OKAY)
        return WOLFSSL_SUCCESS;

    WOLFSSL_MSG("wolfSSL_BN_mod mp_mod failed");
    return 0;
}


/* r = (a^p) % m */
int wolfSSL_BN_mod_exp(WOLFSSL_BIGNUM *r, const WOLFSSL_BIGNUM *a,
      const WOLFSSL_BIGNUM *p, const WOLFSSL_BIGNUM *m, WOLFSSL_BN_CTX *ctx)
{
    int ret;

    WOLFSSL_ENTER("wolfSSL_BN_mod_exp");

    (void) ctx;
    if (r == NULL || a == NULL || p == NULL || m == NULL) {
        WOLFSSL_MSG("Bad Argument");
        return WOLFSSL_FAILURE;
    }

    if ((ret = mp_exptmod((mp_int*)a->internal,(mp_int*)p->internal,
               (mp_int*)m->internal, (mp_int*)r->internal)) == MP_OKAY) {
        return WOLFSSL_SUCCESS;
    }

    WOLFSSL_LEAVE("wolfSSL_BN_mod_exp", ret);
    (void)ret;

    return WOLFSSL_FAILURE;
}

/* r = (a * p) % m */
int wolfSSL_BN_mod_mul(WOLFSSL_BIGNUM *r, const WOLFSSL_BIGNUM *a,
        const WOLFSSL_BIGNUM *p, const WOLFSSL_BIGNUM *m, WOLFSSL_BN_CTX *ctx)
{
    int ret;

    WOLFSSL_ENTER("wolfSSL_BN_mod_mul");

    (void) ctx;
    if (r == NULL || a == NULL || p == NULL || m == NULL) {
        WOLFSSL_MSG("Bad Argument");
        return SSL_FAILURE;
    }

    if ((ret = mp_mulmod((mp_int*)a->internal,(mp_int*)p->internal,
               (mp_int*)m->internal, (mp_int*)r->internal)) == MP_OKAY) {
        return SSL_SUCCESS;
    }

    WOLFSSL_LEAVE("wolfSSL_BN_mod_mul", ret);
    (void)ret;

    return SSL_FAILURE;
}

const WOLFSSL_BIGNUM* wolfSSL_BN_value_one(void)
{
    static WOLFSSL_BIGNUM* bn_one = NULL;

    WOLFSSL_MSG("wolfSSL_BN_value_one");

    if (bn_one == NULL) {
        bn_one = wolfSSL_BN_new();
        if (bn_one) {
            if (mp_set_int((mp_int*)bn_one->internal, 1) != MP_OKAY) {
                /* handle error by freeing BN and returning NULL */
                wolfSSL_BN_free(bn_one);
                bn_one = NULL;
            }
        }
    }

    return bn_one;
}

/* return compliant with OpenSSL
 *   size of BIGNUM in bytes, 0 if error */
int wolfSSL_BN_num_bytes(const WOLFSSL_BIGNUM* bn)
{
    WOLFSSL_ENTER("wolfSSL_BN_num_bytes");

    if (bn == NULL || bn->internal == NULL)
        return WOLFSSL_FAILURE;

    return mp_unsigned_bin_size((mp_int*)bn->internal);
}

/* return compliant with OpenSSL
 *   size of BIGNUM in bits, 0 if error */
int wolfSSL_BN_num_bits(const WOLFSSL_BIGNUM* bn)
{
    WOLFSSL_ENTER("wolfSSL_BN_num_bits");

    if (bn == NULL || bn->internal == NULL)
        return WOLFSSL_FAILURE;

    return mp_count_bits((mp_int*)bn->internal);
}

/* return compliant with OpenSSL
 *   1 if BIGNUM is zero, 0 else */
int wolfSSL_BN_is_zero(const WOLFSSL_BIGNUM* bn)
{
    WOLFSSL_MSG("wolfSSL_BN_is_zero");

    if (bn == NULL || bn->internal == NULL)
        return WOLFSSL_FAILURE;

    if (mp_iszero((mp_int*)bn->internal) == MP_YES)
        return WOLFSSL_SUCCESS;

    return WOLFSSL_FAILURE;
}

/* return compliant with OpenSSL
 *   1 if BIGNUM is one, 0 else */
int wolfSSL_BN_is_one(const WOLFSSL_BIGNUM* bn)
{
    WOLFSSL_MSG("wolfSSL_BN_is_one");

    if (bn == NULL || bn->internal == NULL)
        return WOLFSSL_FAILURE;

    if (mp_cmp_d((mp_int*)bn->internal, 1) == MP_EQ)
        return WOLFSSL_SUCCESS;

    return WOLFSSL_FAILURE;
}

/* return compliant with OpenSSL
 *   1 if BIGNUM is odd, 0 else */
int wolfSSL_BN_is_odd(const WOLFSSL_BIGNUM* bn)
{
    WOLFSSL_MSG("wolfSSL_BN_is_odd");

    if (bn == NULL || bn->internal == NULL)
        return WOLFSSL_FAILURE;

    if (mp_isodd((mp_int*)bn->internal) == MP_YES)
        return WOLFSSL_SUCCESS;

    return WOLFSSL_FAILURE;
}

/* return compliant with OpenSSL
 *   -1 if a < b, 0 if a == b and 1 if a > b
 */
int wolfSSL_BN_cmp(const WOLFSSL_BIGNUM* a, const WOLFSSL_BIGNUM* b)
{
    int ret;

    WOLFSSL_MSG("wolfSSL_BN_cmp");

    if (a == NULL || a->internal == NULL || b == NULL || b->internal == NULL)
        return WOLFSSL_FATAL_ERROR;

    ret = mp_cmp((mp_int*)a->internal, (mp_int*)b->internal);

    return (ret == MP_EQ ? 0 : (ret == MP_GT ? 1 : -1));
}

/* return compliant with OpenSSL
 *   length of BIGNUM in bytes, -1 if error */
int wolfSSL_BN_bn2bin(const WOLFSSL_BIGNUM* bn, unsigned char* r)
{
    WOLFSSL_MSG("wolfSSL_BN_bn2bin");

    if (bn == NULL || bn->internal == NULL) {
        WOLFSSL_MSG("NULL bn error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (r == NULL)
        return mp_unsigned_bin_size((mp_int*)bn->internal);

    if (mp_to_unsigned_bin((mp_int*)bn->internal, r) != MP_OKAY) {
        WOLFSSL_MSG("mp_to_unsigned_bin error");
        return WOLFSSL_FATAL_ERROR;
    }

    return mp_unsigned_bin_size((mp_int*)bn->internal);
}


WOLFSSL_BIGNUM* wolfSSL_BN_bin2bn(const unsigned char* str, int len,
                            WOLFSSL_BIGNUM* ret)
{
    int weOwn = 0;

    WOLFSSL_MSG("wolfSSL_BN_bin2bn");

    /* if ret is null create a BN */
    if (ret == NULL) {
        ret = wolfSSL_BN_new();
        weOwn = 1;
        if (ret == NULL)
            return NULL;
    }

    /* check ret and ret->internal then read in value */
    if (ret && ret->internal) {
        if (mp_read_unsigned_bin((mp_int*)ret->internal, str, len) != 0) {
            WOLFSSL_MSG("mp_read_unsigned_bin failure");
            if (weOwn)
                wolfSSL_BN_free(ret);
            return NULL;
        }
    }

    return ret;
}

/* return compliant with OpenSSL
 *   1 if success, 0 if error */
#ifndef NO_WOLFSSL_STUB
int wolfSSL_mask_bits(WOLFSSL_BIGNUM* bn, int n)
{
    (void)bn;
    (void)n;
    WOLFSSL_ENTER("wolfSSL_BN_mask_bits");
    WOLFSSL_STUB("BN_mask_bits");
    return SSL_FAILURE;
}
#endif


/* WOLFSSL_SUCCESS on ok */
int wolfSSL_BN_rand(WOLFSSL_BIGNUM* bn, int bits, int top, int bottom)
{
    int           ret    = 0;
    int           len    = bits / 8;
    int           initTmpRng = 0;
    WC_RNG*       rng    = NULL;
#ifdef WOLFSSL_SMALL_STACK
    WC_RNG*       tmpRNG = NULL;
    byte*         buff   = NULL;
#else
    WC_RNG        tmpRNG[1];
    byte          buff[1024];
#endif

    (void)top;
    (void)bottom;
    WOLFSSL_MSG("wolfSSL_BN_rand");

    if (bits % 8)
        len++;

#ifdef WOLFSSL_SMALL_STACK
    buff   = (byte*)XMALLOC(1024,        NULL, DYNAMIC_TYPE_TMP_BUFFER);
    tmpRNG = (WC_RNG*) XMALLOC(sizeof(WC_RNG), NULL, DYNAMIC_TYPE_RNG);
    if (buff == NULL || tmpRNG == NULL) {
        XFREE(buff,   NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(tmpRNG, NULL, DYNAMIC_TYPE_RNG);
        return ret;
    }
#endif

    if (bn == NULL || bn->internal == NULL)
        WOLFSSL_MSG("Bad function arguments");
    else if (wc_InitRng(tmpRNG) == 0) {
        rng = tmpRNG;
        initTmpRng = 1;
    }
    else if (initGlobalRNG)
        rng = &globalRNG;

    if (rng) {
        if (wc_RNG_GenerateBlock(rng, buff, len) != 0)
            WOLFSSL_MSG("Bad wc_RNG_GenerateBlock");
        else {
            buff[0]     |= 0x80 | 0x40;
            buff[len-1] |= 0x01;

            if (mp_read_unsigned_bin((mp_int*)bn->internal,buff,len) != MP_OKAY)
                WOLFSSL_MSG("mp read bin failed");
            else
                ret = WOLFSSL_SUCCESS;
        }
    }

    if (initTmpRng)
        wc_FreeRng(tmpRNG);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(buff,   NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(tmpRNG, NULL, DYNAMIC_TYPE_RNG);
#endif

    return ret;
}


/* WOLFSSL_SUCCESS on ok
 * code is same as wolfSSL_BN_rand except for how top and bottom is handled.
 * top -1 then leave most sig bit alone
 * top 0 then most sig is set to 1
 * top is 1 then first two most sig bits are 1
 *
 * bottom is hot then odd number */
int wolfSSL_BN_pseudo_rand(WOLFSSL_BIGNUM* bn, int bits, int top, int bottom)
{
    int           ret    = 0;
    int           len    = bits / 8;
    int           initTmpRng = 0;
    WC_RNG*       rng    = NULL;
#ifdef WOLFSSL_SMALL_STACK
    WC_RNG*       tmpRNG = NULL;
    byte*         buff   = NULL;
#else
    WC_RNG        tmpRNG[1];
    byte          buff[1024];
#endif

    WOLFSSL_MSG("wolfSSL_BN_rand");

    if (bits % 8)
        len++;

#ifdef WOLFSSL_SMALL_STACK
    buff   = (byte*)XMALLOC(1024,        NULL, DYNAMIC_TYPE_TMP_BUFFER);
    tmpRNG = (WC_RNG*) XMALLOC(sizeof(WC_RNG), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (buff == NULL || tmpRNG == NULL) {
        XFREE(buff,   NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(tmpRNG, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return ret;
    }
#endif

    if (bn == NULL || bn->internal == NULL)
        WOLFSSL_MSG("Bad function arguments");
    else if (wc_InitRng(tmpRNG) == 0) {
        rng = tmpRNG;
        initTmpRng = 1;
    }
    else if (initGlobalRNG)
        rng = &globalRNG;

    if (rng) {
        if (wc_RNG_GenerateBlock(rng, buff, len) != 0)
            WOLFSSL_MSG("Bad wc_RNG_GenerateBlock");
        else {
            switch (top) {
                case -1:
                    break;

                case 0:
                    buff[0] |= 0x80;
                    break;

                case 1:
                    buff[0] |= 0x80 | 0x40;
                    break;
            }

            if (bottom == 1) {
                buff[len-1] |= 0x01;
            }

            if (mp_read_unsigned_bin((mp_int*)bn->internal,buff,len) != MP_OKAY)
                WOLFSSL_MSG("mp read bin failed");
            else
                ret = WOLFSSL_SUCCESS;
        }
    }

    if (initTmpRng)
        wc_FreeRng(tmpRNG);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(buff,   NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(tmpRNG, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

/* return code compliant with OpenSSL :
 *   1 if bit set, 0 else
 */
int wolfSSL_BN_is_bit_set(const WOLFSSL_BIGNUM* bn, int n)
{
    if (bn == NULL || bn->internal == NULL) {
        WOLFSSL_MSG("bn NULL error");
        return WOLFSSL_FAILURE;
    }

    if (n > DIGIT_BIT) {
        WOLFSSL_MSG("input bit count too large");
        return WOLFSSL_FAILURE;
    }

    return mp_is_bit_set((mp_int*)bn->internal, (mp_digit)n);
}

/* return code compliant with OpenSSL :
 *   1 if success, 0 else
 */
int wolfSSL_BN_set_bit(WOLFSSL_BIGNUM* bn, int n)
{
    if (bn == NULL || bn->internal == NULL) {
        WOLFSSL_MSG("bn NULL error");
        return WOLFSSL_FAILURE;
    }

    if (mp_set_bit((mp_int*)bn->internal, n) != MP_OKAY) {
        WOLFSSL_MSG("mp_set_int error");
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}


/* WOLFSSL_SUCCESS on ok */
/* Note on use: this function expects str to be an even length. It is
 * converting pairs of bytes into 8-bit values. As an example, the RSA
 * public exponent is commonly 0x010001. To get it to convert, you need
 * to pass in the string "010001", it will fail if you use "10001". This
 * is an affect of how Base16_Decode() works.
 */
int wolfSSL_BN_hex2bn(WOLFSSL_BIGNUM** bn, const char* str)
{
    int     ret     = 0;
    word32  decSz   = 1024;
#ifdef WOLFSSL_SMALL_STACK
    byte*   decoded = NULL;
#else
    byte    decoded[1024];
#endif

    WOLFSSL_MSG("wolfSSL_BN_hex2bn");

#ifdef WOLFSSL_SMALL_STACK
    decoded = (byte*)XMALLOC(decSz, NULL, DYNAMIC_TYPE_DER);
    if (decoded == NULL)
        return ret;
#endif

    if (str == NULL || str[0] == '\0')
        WOLFSSL_MSG("Bad function argument");
    else if (Base16_Decode((byte*)str, (int)XSTRLEN(str), decoded, &decSz) < 0)
        WOLFSSL_MSG("Bad Base16_Decode error");
    else if (bn == NULL)
        ret = decSz;
    else {
        if (*bn == NULL)
            *bn = wolfSSL_BN_new();

        if (*bn == NULL)
            WOLFSSL_MSG("BN new failed");
        else if (wolfSSL_BN_bin2bn(decoded, decSz, *bn) == NULL)
            WOLFSSL_MSG("Bad bin2bn error");
        else
            ret = WOLFSSL_SUCCESS;
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(decoded, NULL, DYNAMIC_TYPE_DER);
#endif

    return ret;
}


WOLFSSL_BIGNUM* wolfSSL_BN_dup(const WOLFSSL_BIGNUM* bn)
{
    WOLFSSL_BIGNUM* ret;

    WOLFSSL_MSG("wolfSSL_BN_dup");

    if (bn == NULL || bn->internal == NULL) {
        WOLFSSL_MSG("bn NULL error");
        return NULL;
    }

    ret = wolfSSL_BN_new();
    if (ret == NULL) {
        WOLFSSL_MSG("bn new error");
        return NULL;
    }

    if (mp_copy((mp_int*)bn->internal, (mp_int*)ret->internal) != MP_OKAY) {
        WOLFSSL_MSG("mp_copy error");
        wolfSSL_BN_free(ret);
        return NULL;
    }

    ret->neg = bn->neg;

    return ret;
}


WOLFSSL_BIGNUM* wolfSSL_BN_copy(WOLFSSL_BIGNUM* r, const WOLFSSL_BIGNUM* bn)
{
    WOLFSSL_MSG("wolfSSL_BN_copy");

    if (r == NULL || bn == NULL) {
        WOLFSSL_MSG("r or bn NULL error");
        return NULL;
    }

    if (mp_copy((mp_int*)bn->internal, (mp_int*)r->internal) != MP_OKAY) {
        WOLFSSL_MSG("mp_copy error");
        return NULL;
    }

    r->neg = bn->neg;

    return r;
}

/* return code compliant with OpenSSL :
 *   1 if success, 0 else
 */
int wolfSSL_BN_set_word(WOLFSSL_BIGNUM* bn, WOLFSSL_BN_ULONG w)
{
    WOLFSSL_MSG("wolfSSL_BN_set_word");

    if (bn == NULL) {
        WOLFSSL_MSG("bn NULL error");
        return WOLFSSL_FAILURE;
    }

    if (mp_set_int((mp_int*)bn->internal, w) != MP_OKAY) {
        WOLFSSL_MSG("mp_init_set_int error");
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}


/* Returns the big number as an unsigned long if possible.
 *
 * bn  big number structure to get value from
 *
 * Returns value or 0xFFFFFFFFL if bigger than unsigned long.
 */
unsigned long wolfSSL_BN_get_word(const WOLFSSL_BIGNUM* bn)
{
    mp_int* mp;

    WOLFSSL_MSG("wolfSSL_BN_get_word");

    if (bn == NULL) {
        WOLFSSL_MSG("Invalid argument");
        return 0;
    }

    if (wolfSSL_BN_num_bytes(bn) > (int)sizeof(unsigned long)) {
        WOLFSSL_MSG("bignum is larger than unsigned long");
        return 0xFFFFFFFFL;
    }
    mp = (mp_int*)bn->internal;

    return (unsigned long)(mp->dp[0]);
}

/* return code compliant with OpenSSL :
 *   number length in decimal if success, 0 if error
 */
#ifndef NO_WOLFSSL_STUB
int wolfSSL_BN_dec2bn(WOLFSSL_BIGNUM** bn, const char* str)
{
    (void)bn;
    (void)str;

    WOLFSSL_MSG("wolfSSL_BN_dec2bn");
    WOLFSSL_STUB("BN_dec2bn");
    return SSL_FAILURE;
}
#endif

#if defined(WOLFSSL_KEY_GEN) || defined(HAVE_COMP_KEY)
char *wolfSSL_BN_bn2dec(const WOLFSSL_BIGNUM *bn)
{
    int len = 0;
    char *buf;

    WOLFSSL_MSG("wolfSSL_BN_bn2dec");

    if (bn == NULL || bn->internal == NULL) {
        WOLFSSL_MSG("bn NULL error");
        return NULL;
    }

    if (mp_radix_size((mp_int*)bn->internal, MP_RADIX_DEC, &len) != MP_OKAY) {
        WOLFSSL_MSG("mp_radix_size failure");
        return NULL;
    }

    buf = (char*) XMALLOC(len, NULL, DYNAMIC_TYPE_OPENSSL);
    if (buf == NULL) {
        WOLFSSL_MSG("BN_bn2dec malloc buffer failure");
        return NULL;
    }

    if (mp_todecimal((mp_int*)bn->internal, buf) != MP_OKAY) {
        XFREE(buf, NULL, DYNAMIC_TYPE_ECC);
        return NULL;
    }

    return buf;
}
#else
char* wolfSSL_BN_bn2dec(const WOLFSSL_BIGNUM* bn)
{
    (void)bn;

    WOLFSSL_MSG("wolfSSL_BN_bn2dec");

    return NULL;
}
#endif /* defined(WOLFSSL_KEY_GEN) || defined(HAVE_COMP_KEY) */

/* return code compliant with OpenSSL :
 *   1 if success, 0 else
 */
int wolfSSL_BN_lshift(WOLFSSL_BIGNUM *r, const WOLFSSL_BIGNUM *bn, int n)
{
    WOLFSSL_MSG("wolfSSL_BN_lshift");

    if (r == NULL || r->internal == NULL || bn == NULL || bn->internal == NULL){
        WOLFSSL_MSG("bn NULL error");
        return WOLFSSL_FAILURE;
    }

    if (mp_mul_2d((mp_int*)bn->internal, n, (mp_int*)r->internal) != MP_OKAY) {
        WOLFSSL_MSG("mp_mul_2d error");
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}

/* return code compliant with OpenSSL :
 *   1 if success, 0 else
 */
int wolfSSL_BN_rshift(WOLFSSL_BIGNUM *r, const WOLFSSL_BIGNUM *bn, int n)
{
    WOLFSSL_MSG("wolfSSL_BN_rshift");

    if (r == NULL || r->internal == NULL || bn == NULL || bn->internal == NULL){
        WOLFSSL_MSG("bn NULL error");
        return WOLFSSL_FAILURE;
    }

    if (mp_div_2d((mp_int*)bn->internal, n,
                  (mp_int*)r->internal, NULL) != MP_OKAY) {
        WOLFSSL_MSG("mp_mul_2d error");
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}

/* return code compliant with OpenSSL :
 *   1 if success, 0 else
 */
int wolfSSL_BN_add_word(WOLFSSL_BIGNUM *bn, WOLFSSL_BN_ULONG w)
{
    WOLFSSL_MSG("wolfSSL_BN_add_word");

    if (bn == NULL || bn->internal == NULL) {
        WOLFSSL_MSG("bn NULL error");
        return WOLFSSL_FAILURE;
    }

    if (mp_add_d((mp_int*)bn->internal, w, (mp_int*)bn->internal) != MP_OKAY) {
        WOLFSSL_MSG("mp_add_d error");
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}

/* return code compliant with OpenSSL :
 *   1 if success, 0 else
 */
int wolfSSL_BN_add(WOLFSSL_BIGNUM *r, WOLFSSL_BIGNUM *a, WOLFSSL_BIGNUM *b)
{
    WOLFSSL_MSG("wolfSSL_BN_add");

    if (r == NULL || r->internal == NULL || a == NULL || a->internal == NULL ||
        b == NULL || b->internal == NULL) {
        WOLFSSL_MSG("bn NULL error");
        return WOLFSSL_FAILURE;
    }

    if (mp_add((mp_int*)a->internal, (mp_int*)b->internal,
               (mp_int*)r->internal) != MP_OKAY) {
        WOLFSSL_MSG("mp_add_d error");
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}

#ifdef WOLFSSL_KEY_GEN

/* return code compliant with OpenSSL :
 *   1 if prime, 0 if not, -1 if error
 */
int wolfSSL_BN_is_prime_ex(const WOLFSSL_BIGNUM *bn, int nbchecks,
                           WOLFSSL_BN_CTX *ctx, WOLFSSL_BN_GENCB *cb)
{
    int res;

    (void)ctx;
    (void)cb;

    WOLFSSL_MSG("wolfSSL_BN_is_prime_ex");

    if (bn == NULL || bn->internal == NULL) {
        WOLFSSL_MSG("bn NULL error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (mp_prime_is_prime((mp_int*)bn->internal, nbchecks, &res) != MP_OKAY) {
        WOLFSSL_MSG("mp_prime_is_prime error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (res != MP_YES) {
        WOLFSSL_MSG("mp_prime_is_prime not prime");
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}

/* return code compliant with OpenSSL :
 *   (bn mod w) if success, -1 if error
 */
WOLFSSL_BN_ULONG wolfSSL_BN_mod_word(const WOLFSSL_BIGNUM *bn,
                                     WOLFSSL_BN_ULONG w)
{
    WOLFSSL_BN_ULONG ret = 0;

    WOLFSSL_MSG("wolfSSL_BN_mod_word");

    if (bn == NULL || bn->internal == NULL) {
        WOLFSSL_MSG("bn NULL error");
        return (WOLFSSL_BN_ULONG)WOLFSSL_FATAL_ERROR;
    }

    if (mp_mod_d((mp_int*)bn->internal, w, &ret) != MP_OKAY) {
        WOLFSSL_MSG("mp_add_d error");
        return (WOLFSSL_BN_ULONG)WOLFSSL_FATAL_ERROR;
    }

    return ret;
}
#endif /* #ifdef WOLFSSL_KEY_GEN */

char *wolfSSL_BN_bn2hex(const WOLFSSL_BIGNUM *bn)
{
#if defined(WOLFSSL_KEY_GEN) || defined(HAVE_COMP_KEY) || defined(DEBUG_WOLFSSL)
    int len = 0;
    char *buf;

    WOLFSSL_ENTER("wolfSSL_BN_bn2hex");

    if (bn == NULL || bn->internal == NULL) {
        WOLFSSL_MSG("bn NULL error");
        return NULL;
    }

    if (mp_radix_size((mp_int*)bn->internal, MP_RADIX_HEX, &len) != MP_OKAY) {
        WOLFSSL_MSG("mp_radix_size failure");
        return NULL;
    }

    buf = (char*) XMALLOC(len, NULL, DYNAMIC_TYPE_ECC);
    if (buf == NULL) {
        WOLFSSL_MSG("BN_bn2hex malloc buffer failure");
        return NULL;
    }

    if (mp_tohex((mp_int*)bn->internal, buf) != MP_OKAY) {
        XFREE(buf, NULL, DYNAMIC_TYPE_ECC);
        return NULL;
    }

    return buf;
#else
    (void)bn;
    WOLFSSL_MSG("wolfSSL_BN_bn2hex not compiled in");
    return (char*)"";
#endif
}

#ifndef NO_FILESYSTEM
/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
int wolfSSL_BN_print_fp(XFILE fp, const WOLFSSL_BIGNUM *bn)
{
#if defined(WOLFSSL_KEY_GEN) || defined(HAVE_COMP_KEY) || defined(DEBUG_WOLFSSL)
    char *buf;

    WOLFSSL_ENTER("wolfSSL_BN_print_fp");

    if (fp == NULL || bn == NULL || bn->internal == NULL) {
        WOLFSSL_MSG("bn NULL error");
        return WOLFSSL_FAILURE;
    }

    buf = wolfSSL_BN_bn2hex(bn);
    if (buf == NULL) {
        WOLFSSL_MSG("wolfSSL_BN_bn2hex failure");
        return WOLFSSL_FAILURE;
    }

    fprintf(fp, "%s", buf);
    XFREE(buf, NULL, DYNAMIC_TYPE_ECC);

    return WOLFSSL_SUCCESS;
#else
    (void)fp;
    (void)bn;

    WOLFSSL_MSG("wolfSSL_BN_print_fp not compiled in");

    return WOLFSSL_SUCCESS;
#endif
}
#endif /* !NO_FILESYSTEM */


WOLFSSL_BIGNUM *wolfSSL_BN_CTX_get(WOLFSSL_BN_CTX *ctx)
{
    /* ctx is not used, return new Bignum */
    (void)ctx;

    WOLFSSL_ENTER("wolfSSL_BN_CTX_get");

    return wolfSSL_BN_new();
}

#ifndef NO_WOLFSSL_STUB
void wolfSSL_BN_CTX_start(WOLFSSL_BN_CTX *ctx)
{
    (void)ctx;

    WOLFSSL_ENTER("wolfSSL_BN_CTX_start");
    WOLFSSL_STUB("BN_CTX_start");
    WOLFSSL_MSG("wolfSSL_BN_CTX_start TBD");
}
#endif


WOLFSSL_BIGNUM *wolfSSL_BN_mod_inverse(WOLFSSL_BIGNUM *r,
                                       WOLFSSL_BIGNUM *a,
                                       const WOLFSSL_BIGNUM *n,
                                       WOLFSSL_BN_CTX *ctx)
{
    int dynamic = 0;

    /* ctx is not used */
    (void)ctx;

    WOLFSSL_ENTER("wolfSSL_BN_mod_inverse");

    /* check parameter */
    if (r == NULL) {
        r = wolfSSL_BN_new();
        if (r == NULL){
            WOLFSSL_MSG("WolfSSL_BN_new() failed");
            return NULL;
        }
        dynamic = 1;
    }

    if (a == NULL) {
        WOLFSSL_MSG("a NULL error");
        if (dynamic == 1) {
            wolfSSL_BN_free(r);
        }
        return NULL;
    }

    if (n == NULL) {
        WOLFSSL_MSG("n NULL error");
        if (dynamic == 1) {
            wolfSSL_BN_free(r);
        }
        return NULL;
    }

    /* Compute inverse of a modulo n and return r */
    if (mp_invmod((mp_int *)a->internal,(mp_int *)n->internal,
                  (mp_int*)r->internal) == MP_VAL){
        WOLFSSL_MSG("mp_invmod() error");
        if (dynamic == 1) {
            wolfSSL_BN_free(r);
        }
        return NULL;
    }

    return  r;
}

#ifndef NO_DH

static void InitwolfSSL_DH(WOLFSSL_DH* dh)
{
    if (dh) {
        dh->p        = NULL;
        dh->g        = NULL;
        dh->q        = NULL;
        dh->pub_key  = NULL;
        dh->priv_key = NULL;
        dh->internal = NULL;
        dh->inSet    = 0;
        dh->exSet    = 0;
    }
}


WOLFSSL_DH* wolfSSL_DH_new(void)
{
    WOLFSSL_DH* external;
    DhKey*     key;

    WOLFSSL_MSG("wolfSSL_DH_new");

    key = (DhKey*) XMALLOC(sizeof(DhKey), NULL, DYNAMIC_TYPE_DH);
    if (key == NULL) {
        WOLFSSL_MSG("wolfSSL_DH_new malloc DhKey failure");
        return NULL;
    }

    external = (WOLFSSL_DH*) XMALLOC(sizeof(WOLFSSL_DH), NULL,
                                    DYNAMIC_TYPE_DH);
    if (external == NULL) {
        WOLFSSL_MSG("wolfSSL_DH_new malloc WOLFSSL_DH failure");
        XFREE(key, NULL, DYNAMIC_TYPE_DH);
        return NULL;
    }

    InitwolfSSL_DH(external);
    if (wc_InitDhKey(key) != 0) {
        WOLFSSL_MSG("wolfSSL_DH_new InitDhKey failure");
        XFREE(key, NULL, DYNAMIC_TYPE_DH);
        XFREE(external, NULL, DYNAMIC_TYPE_DH);
        return NULL;
    }
    external->internal = key;

    return external;
}


void wolfSSL_DH_free(WOLFSSL_DH* dh)
{
    WOLFSSL_MSG("wolfSSL_DH_free");

    if (dh) {
        if (dh->internal) {
            wc_FreeDhKey((DhKey*)dh->internal);
            XFREE(dh->internal, NULL, DYNAMIC_TYPE_DH);
            dh->internal = NULL;
        }
        wolfSSL_BN_free(dh->priv_key);
        wolfSSL_BN_free(dh->pub_key);
        wolfSSL_BN_free(dh->g);
        wolfSSL_BN_free(dh->p);
        wolfSSL_BN_free(dh->q);
        InitwolfSSL_DH(dh);  /* set back to NULLs for safety */

        XFREE(dh, NULL, DYNAMIC_TYPE_DH);
    }
}


static int SetDhInternal(WOLFSSL_DH* dh)
{
    int            ret = WOLFSSL_FATAL_ERROR;
    int            pSz = 1024;
    int            gSz = 1024;
#ifdef WOLFSSL_SMALL_STACK
    unsigned char* p   = NULL;
    unsigned char* g   = NULL;
#else
    unsigned char  p[1024];
    unsigned char  g[1024];
#endif

    WOLFSSL_ENTER("SetDhInternal");

    if (dh == NULL || dh->p == NULL || dh->g == NULL)
        WOLFSSL_MSG("Bad function arguments");
    else if (wolfSSL_BN_bn2bin(dh->p, NULL) > pSz)
        WOLFSSL_MSG("Bad p internal size");
    else if (wolfSSL_BN_bn2bin(dh->g, NULL) > gSz)
        WOLFSSL_MSG("Bad g internal size");
    else {
    #ifdef WOLFSSL_SMALL_STACK
        p = (unsigned char*)XMALLOC(pSz, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
        g = (unsigned char*)XMALLOC(gSz, NULL, DYNAMIC_TYPE_PUBLIC_KEY);

        if (p == NULL || g == NULL) {
            XFREE(p, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
            XFREE(g, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
            return ret;
        }
    #endif

        pSz = wolfSSL_BN_bn2bin(dh->p, p);
        gSz = wolfSSL_BN_bn2bin(dh->g, g);

        if (pSz <= 0 || gSz <= 0)
            WOLFSSL_MSG("Bad BN2bin set");
        else if (wc_DhSetKey((DhKey*)dh->internal, p, pSz, g, gSz) < 0)
            WOLFSSL_MSG("Bad DH SetKey");
        else {
            dh->inSet = 1;
            ret = WOLFSSL_SUCCESS;
        }

    #ifdef WOLFSSL_SMALL_STACK
        XFREE(p, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
        XFREE(g, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
    #endif
    }


    return ret;
}

/* return code compliant with OpenSSL :
 *   DH prime size in bytes if success, 0 if error
 */
int wolfSSL_DH_size(WOLFSSL_DH* dh)
{
    WOLFSSL_MSG("wolfSSL_DH_size");

    if (dh == NULL)
        return WOLFSSL_FATAL_ERROR;

    return wolfSSL_BN_num_bytes(dh->p);
}


/* This sets a big number with the 1536-bit prime from RFC 3526.
 *
 * bn  if not NULL then the big number structure is used. If NULL then a new
 *     big number structure is created.
 *
 * Returns a WOLFSSL_BIGNUM structure on success and NULL with failure.
 */
WOLFSSL_BIGNUM* wolfSSL_DH_1536_prime(WOLFSSL_BIGNUM* bn)
{
    const char prm[] = {
        "FFFFFFFFFFFFFFFFC90FDAA22168C234"
        "C4C6628B80DC1CD129024E088A67CC74"
        "020BBEA63B139B22514A08798E3404DD"
        "EF9519B3CD3A431B302B0A6DF25F1437"
        "4FE1356D6D51C245E485B576625E7EC6"
        "F44C42E9A637ED6B0BFF5CB6F406B7ED"
        "EE386BFB5A899FA5AE9F24117C4B1FE6"
        "49286651ECE45B3DC2007CB8A163BF05"
        "98DA48361C55D39A69163FA8FD24CF5F"
        "83655D23DCA3AD961C62F356208552BB"
        "9ED529077096966D670C354E4ABC9804"
        "F1746C08CA237327FFFFFFFFFFFFFFFF"
    };

    WOLFSSL_ENTER("wolfSSL_DH_1536_prime");

    if (wolfSSL_BN_hex2bn(&bn, prm) != SSL_SUCCESS) {
        WOLFSSL_MSG("Error converting DH 1536 prime to big number");
        return NULL;
    }

    return bn;
}


/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
int wolfSSL_DH_generate_key(WOLFSSL_DH* dh)
{
    int            ret    = WOLFSSL_FAILURE;
    word32         pubSz  = 768;
    word32         privSz = 768;
    int            initTmpRng = 0;
    WC_RNG*        rng    = NULL;
#ifdef WOLFSSL_SMALL_STACK
    unsigned char* pub    = NULL;
    unsigned char* priv   = NULL;
    WC_RNG*        tmpRNG = NULL;
#else
    unsigned char  pub [768];
    unsigned char  priv[768];
    WC_RNG         tmpRNG[1];
#endif

    WOLFSSL_MSG("wolfSSL_DH_generate_key");

#ifdef WOLFSSL_SMALL_STACK
    tmpRNG = (WC_RNG*)XMALLOC(sizeof(WC_RNG), NULL, DYNAMIC_TYPE_RNG);
    pub    = (unsigned char*)XMALLOC(pubSz,   NULL, DYNAMIC_TYPE_PUBLIC_KEY);
    priv   = (unsigned char*)XMALLOC(privSz,  NULL, DYNAMIC_TYPE_PRIVATE_KEY);

    if (tmpRNG == NULL || pub == NULL || priv == NULL) {
        XFREE(tmpRNG, NULL, DYNAMIC_TYPE_RNG);
        XFREE(pub,    NULL, DYNAMIC_TYPE_PUBLIC_KEY);
        XFREE(priv,   NULL, DYNAMIC_TYPE_PRIVATE_KEY);
        return ret;
    }
#endif

    if (dh == NULL || dh->p == NULL || dh->g == NULL)
        WOLFSSL_MSG("Bad function arguments");
    else if (dh->inSet == 0 && SetDhInternal(dh) != WOLFSSL_SUCCESS)
            WOLFSSL_MSG("Bad DH set internal");
    else if (wc_InitRng(tmpRNG) == 0) {
        rng = tmpRNG;
        initTmpRng = 1;
    }
    else {
        WOLFSSL_MSG("Bad RNG Init, trying global");
        if (initGlobalRNG == 0)
            WOLFSSL_MSG("Global RNG no Init");
        else
            rng = &globalRNG;
    }

    if (rng) {
       if (wc_DhGenerateKeyPair((DhKey*)dh->internal, rng, priv, &privSz,
                                                               pub, &pubSz) < 0)
            WOLFSSL_MSG("Bad wc_DhGenerateKeyPair");
       else {
            if (dh->pub_key)
                wolfSSL_BN_free(dh->pub_key);

            dh->pub_key = wolfSSL_BN_new();
            if (dh->pub_key == NULL) {
                WOLFSSL_MSG("Bad DH new pub");
            }
            if (dh->priv_key)
                wolfSSL_BN_free(dh->priv_key);

            dh->priv_key = wolfSSL_BN_new();

            if (dh->priv_key == NULL) {
                WOLFSSL_MSG("Bad DH new priv");
            }

            if (dh->pub_key && dh->priv_key) {
               if (wolfSSL_BN_bin2bn(pub, pubSz, dh->pub_key) == NULL)
                   WOLFSSL_MSG("Bad DH bn2bin error pub");
               else if (wolfSSL_BN_bin2bn(priv, privSz, dh->priv_key) == NULL)
                   WOLFSSL_MSG("Bad DH bn2bin error priv");
               else
                   ret = WOLFSSL_SUCCESS;
            }
        }
    }

    if (initTmpRng)
        wc_FreeRng(tmpRNG);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(tmpRNG, NULL, DYNAMIC_TYPE_RNG);
    XFREE(pub,    NULL, DYNAMIC_TYPE_PUBLIC_KEY);
    XFREE(priv,   NULL, DYNAMIC_TYPE_PRIVATE_KEY);
#endif

    return ret;
}


/* return code compliant with OpenSSL :
 *   size of shared secret if success, -1 if error
 */
int wolfSSL_DH_compute_key(unsigned char* key, WOLFSSL_BIGNUM* otherPub,
                          WOLFSSL_DH* dh)
{
    int            ret    = WOLFSSL_FATAL_ERROR;
    word32         keySz  = 0;
    word32         pubSz  = 1024;
    word32         privSz = 1024;
#ifdef WOLFSSL_SMALL_STACK
    unsigned char* pub    = NULL;
    unsigned char* priv   = NULL;
#else
    unsigned char  pub [1024];
    unsigned char  priv[1024];
#endif

    WOLFSSL_MSG("wolfSSL_DH_compute_key");

#ifdef WOLFSSL_SMALL_STACK
    pub = (unsigned char*)XMALLOC(pubSz, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
    if (pub == NULL)
        return ret;

    priv = (unsigned char*)XMALLOC(privSz, NULL, DYNAMIC_TYPE_PRIVATE_KEY);
    if (priv == NULL) {
        XFREE(pub, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
        return ret;
    }
#endif

    if (dh == NULL || dh->priv_key == NULL || otherPub == NULL)
        WOLFSSL_MSG("Bad function arguments");
    else if ((keySz = (word32)DH_size(dh)) == 0)
        WOLFSSL_MSG("Bad DH_size");
    else if (wolfSSL_BN_bn2bin(dh->priv_key, NULL) > (int)privSz)
        WOLFSSL_MSG("Bad priv internal size");
    else if (wolfSSL_BN_bn2bin(otherPub, NULL) > (int)pubSz)
        WOLFSSL_MSG("Bad otherPub size");
    else {
        privSz = wolfSSL_BN_bn2bin(dh->priv_key, priv);
        pubSz  = wolfSSL_BN_bn2bin(otherPub, pub);
        if (dh->inSet == 0 && SetDhInternal(dh) != SSL_SUCCESS){
                WOLFSSL_MSG("Bad DH set internal");
        }
        if (privSz <= 0 || pubSz <= 0)
            WOLFSSL_MSG("Bad BN2bin set");
        else if (wc_DhAgree((DhKey*)dh->internal, key, &keySz,
                            priv, privSz, pub, pubSz) < 0)
            WOLFSSL_MSG("wc_DhAgree failed");
        else
            ret = (int)keySz;
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(pub,  NULL, DYNAMIC_TYPE_PUBLIC_KEY);
    XFREE(priv, NULL, DYNAMIC_TYPE_PRIVATE_KEY);
#endif

    return ret;
}
#endif /* NO_DH */


#ifndef NO_DSA
static void InitwolfSSL_DSA(WOLFSSL_DSA* dsa)
{
    if (dsa) {
        dsa->p        = NULL;
        dsa->q        = NULL;
        dsa->g        = NULL;
        dsa->pub_key  = NULL;
        dsa->priv_key = NULL;
        dsa->internal = NULL;
        dsa->inSet    = 0;
        dsa->exSet    = 0;
    }
}


WOLFSSL_DSA* wolfSSL_DSA_new(void)
{
    WOLFSSL_DSA* external;
    DsaKey*     key;

    WOLFSSL_MSG("wolfSSL_DSA_new");

    key = (DsaKey*) XMALLOC(sizeof(DsaKey), NULL, DYNAMIC_TYPE_DSA);
    if (key == NULL) {
        WOLFSSL_MSG("wolfSSL_DSA_new malloc DsaKey failure");
        return NULL;
    }

    external = (WOLFSSL_DSA*) XMALLOC(sizeof(WOLFSSL_DSA), NULL,
                                    DYNAMIC_TYPE_DSA);
    if (external == NULL) {
        WOLFSSL_MSG("wolfSSL_DSA_new malloc WOLFSSL_DSA failure");
        XFREE(key, NULL, DYNAMIC_TYPE_DSA);
        return NULL;
    }

    InitwolfSSL_DSA(external);
    if (wc_InitDsaKey(key) != 0) {
        WOLFSSL_MSG("wolfSSL_DSA_new InitDsaKey failure");
        XFREE(key, NULL, DYNAMIC_TYPE_DSA);
        wolfSSL_DSA_free(external);
        return NULL;
    }
    external->internal = key;

    return external;
}


void wolfSSL_DSA_free(WOLFSSL_DSA* dsa)
{
    WOLFSSL_MSG("wolfSSL_DSA_free");

    if (dsa) {
        if (dsa->internal) {
            FreeDsaKey((DsaKey*)dsa->internal);
            XFREE(dsa->internal, NULL, DYNAMIC_TYPE_DSA);
            dsa->internal = NULL;
        }
        wolfSSL_BN_free(dsa->priv_key);
        wolfSSL_BN_free(dsa->pub_key);
        wolfSSL_BN_free(dsa->g);
        wolfSSL_BN_free(dsa->q);
        wolfSSL_BN_free(dsa->p);
        InitwolfSSL_DSA(dsa);  /* set back to NULLs for safety */

        XFREE(dsa, NULL, DYNAMIC_TYPE_DSA);
        dsa = NULL;
    }
}

#endif /* NO_DSA */

#endif /* OPENSSL_EXTRA */
#if !defined(NO_RSA) && defined(OPENSSL_EXTRA_X509_SMALL)
static void InitwolfSSL_Rsa(WOLFSSL_RSA* rsa)
{
    if (rsa) {
        XMEMSET(rsa, 0, sizeof(WOLFSSL_RSA));
    }
}

void wolfSSL_RSA_free(WOLFSSL_RSA* rsa)
{
    WOLFSSL_ENTER("wolfSSL_RSA_free");

    if (rsa) {
        if (rsa->internal) {
#if !defined(HAVE_FIPS) && !defined(HAVE_USER_RSA) && \
    !defined(HAVE_FAST_RSA) && defined(WC_RSA_BLINDING)
            WC_RNG* rng;

            /* check if RNG is owned before freeing it */
            if (rsa->ownRng) {
                rng = ((RsaKey*)rsa->internal)->rng;
                if (rng != NULL && rng != &globalRNG) {
                    wc_FreeRng(rng);
                    XFREE(rng, NULL, DYNAMIC_TYPE_RNG);
                }
            }
#endif /* WC_RSA_BLINDING */
            wc_FreeRsaKey((RsaKey*)rsa->internal);
            XFREE(rsa->internal, NULL, DYNAMIC_TYPE_RSA);
            rsa->internal = NULL;
        }
        wolfSSL_BN_free(rsa->iqmp);
        wolfSSL_BN_free(rsa->dmq1);
        wolfSSL_BN_free(rsa->dmp1);
        wolfSSL_BN_free(rsa->q);
        wolfSSL_BN_free(rsa->p);
        wolfSSL_BN_free(rsa->d);
        wolfSSL_BN_free(rsa->e);
        wolfSSL_BN_free(rsa->n);

    #ifdef WC_RSA_BLINDING
        if (wc_FreeRng(rsa->rng) != 0) {
            WOLFSSL_MSG("Issue freeing rng");
        }
        XFREE(rsa->rng, NULL, DYNAMIC_TYPE_RNG);
    #endif

        InitwolfSSL_Rsa(rsa);  /* set back to NULLs for safety */

        XFREE(rsa, NULL, DYNAMIC_TYPE_RSA);
        rsa = NULL;
    }
}

WOLFSSL_RSA* wolfSSL_RSA_new(void)
{
    WOLFSSL_RSA* external;
    RsaKey*     key;

    WOLFSSL_ENTER("wolfSSL_RSA_new");

    key = (RsaKey*) XMALLOC(sizeof(RsaKey), NULL, DYNAMIC_TYPE_RSA);
    if (key == NULL) {
        WOLFSSL_MSG("wolfSSL_RSA_new malloc RsaKey failure");
        return NULL;
    }

    external = (WOLFSSL_RSA*) XMALLOC(sizeof(WOLFSSL_RSA), NULL,
                                     DYNAMIC_TYPE_RSA);
    if (external == NULL) {
        WOLFSSL_MSG("wolfSSL_RSA_new malloc WOLFSSL_RSA failure");
        XFREE(key, NULL, DYNAMIC_TYPE_RSA);
        return NULL;
    }

    InitwolfSSL_Rsa(external);
    if (wc_InitRsaKey(key, NULL) != 0) {
        WOLFSSL_MSG("InitRsaKey WOLFSSL_RSA failure");
        XFREE(external, NULL, DYNAMIC_TYPE_RSA);
        XFREE(key, NULL, DYNAMIC_TYPE_RSA);
        return NULL;
    }

#if !defined(HAVE_FIPS) && !defined(HAVE_USER_RSA) && \
    !defined(HAVE_FAST_RSA) && defined(WC_RSA_BLINDING)
    {
        WC_RNG* rng = NULL;

        rng = (WC_RNG*) XMALLOC(sizeof(WC_RNG), NULL, DYNAMIC_TYPE_RNG);
        if (rng != NULL && wc_InitRng(rng) != 0) {
            WOLFSSL_MSG("InitRng failure, attempting to use global RNG");
            XFREE(rng, NULL, DYNAMIC_TYPE_RNG);
            rng = NULL;
        }

        external->ownRng = 1;
        if (rng == NULL && initGlobalRNG) {
            external->ownRng = 0;
            rng = &globalRNG;
        }

        if (rng == NULL) {
            WOLFSSL_MSG("wolfSSL_RSA_new no WC_RNG for blinding");
            XFREE(external, NULL, DYNAMIC_TYPE_RSA);
            XFREE(key, NULL, DYNAMIC_TYPE_RSA);
            return NULL;
        }

        wc_RsaSetRNG(key, rng);
    }
#endif /* WC_RSA_BLINDING */

    external->internal = key;
    external->inSet = 0;
    return external;
}
#endif /* !NO_RSA && OPENSSL_EXTRA_X509_SMALL */

/* these defines are to make sure the functions SetIndividualExternal is not
 * declared and then not used. */
#if !defined(NO_ASN) || !defined(NO_DSA) || defined(HAVE_ECC) || \
    (!defined(NO_RSA) && !defined(HAVE_USER_RSA) && !defined(HAVE_FAST_RSA))

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
/* when calling SetIndividualExternal, mpi should be cleared by caller if no
 * longer used. ie mp_clear(mpi). This is to free data when fastmath is
 * disabled since a copy of mpi is made by this function and placed into bn.
 */
static int SetIndividualExternal(WOLFSSL_BIGNUM** bn, mp_int* mpi)
{
    byte dynamic = 0;

    WOLFSSL_MSG("Entering SetIndividualExternal");

    if (mpi == NULL || bn == NULL) {
        WOLFSSL_MSG("mpi NULL error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (*bn == NULL) {
        *bn = wolfSSL_BN_new();
        if (*bn == NULL) {
            WOLFSSL_MSG("SetIndividualExternal alloc failed");
            return WOLFSSL_FATAL_ERROR;
        }
        dynamic = 1;
    }

    if (mp_copy(mpi, (mp_int*)((*bn)->internal)) != MP_OKAY) {
        WOLFSSL_MSG("mp_copy error");
        if (dynamic == 1) {
            wolfSSL_BN_free(*bn);
        }
        return WOLFSSL_FATAL_ERROR;
    }

    return WOLFSSL_SUCCESS;
}
#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */

#ifdef OPENSSL_EXTRA /* only without X509_SMALL */
static int SetIndividualInternal(WOLFSSL_BIGNUM* bn, mp_int* mpi)
{
    WOLFSSL_MSG("Entering SetIndividualInternal");

    if (bn == NULL || bn->internal == NULL) {
        WOLFSSL_MSG("bn NULL error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (mpi == NULL || (mp_init(mpi) != MP_OKAY)) {
        WOLFSSL_MSG("mpi NULL error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (mp_copy((mp_int*)bn->internal, mpi) != MP_OKAY) {
        WOLFSSL_MSG("mp_copy error");
        return WOLFSSL_FATAL_ERROR;
    }

    return WOLFSSL_SUCCESS;
}

#ifndef NO_ASN
WOLFSSL_BIGNUM *wolfSSL_ASN1_INTEGER_to_BN(const WOLFSSL_ASN1_INTEGER *ai,
                                       WOLFSSL_BIGNUM *bn)
{
    mp_int mpi;
    word32 idx = 0;
    int ret;

    WOLFSSL_ENTER("wolfSSL_ASN1_INTEGER_to_BN");

    if (ai == NULL) {
        return NULL;
    }

    if ((ret = GetInt(&mpi, ai->data, &idx, ai->dataMax)) != 0) {
        /* expecting ASN1 format for INTEGER */
        WOLFSSL_LEAVE("wolfSSL_ASN1_INTEGER_to_BN", ret);
        return NULL;
    }

    /* mp_clear needs called because mpi is copied and causes memory leak with
     * --disable-fastmath */
    ret = SetIndividualExternal(&bn, &mpi);
    mp_clear(&mpi);

    if (ret != WOLFSSL_SUCCESS) {
        return NULL;
    }
    return bn;
}
#endif /* !NO_ASN */

#if !defined(NO_DSA) && !defined(NO_DH)
WOLFSSL_DH *wolfSSL_DSA_dup_DH(const WOLFSSL_DSA *dsa)
{
    WOLFSSL_DH* dh;
    DhKey*      key;

    WOLFSSL_ENTER("wolfSSL_DSA_dup_DH");

    if (dsa == NULL) {
        return NULL;
    }

    dh = wolfSSL_DH_new();
    if (dh == NULL) {
        return NULL;
    }
    key = (DhKey*)dh->internal;

    if (dsa->p != NULL &&
        SetIndividualInternal(((WOLFSSL_DSA*)dsa)->p, &key->p) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("rsa p key error");
        wolfSSL_DH_free(dh);
        return NULL;
    }
    if (dsa->g != NULL &&
        SetIndividualInternal(((WOLFSSL_DSA*)dsa)->g, &key->g) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("rsa g key error");
        wolfSSL_DH_free(dh);
        return NULL;
    }

    if (SetIndividualExternal(&dh->p, &key->p) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("dsa p key error");
        wolfSSL_DH_free(dh);
        return NULL;
    }
    if (SetIndividualExternal(&dh->g, &key->g) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("dsa g key error");
        wolfSSL_DH_free(dh);
        return NULL;
    }

    return dh;
}
#endif /* !defined(NO_DSA) && !defined(NO_DH) */

#endif /* OPENSSL_EXTRA */
#endif /* !NO_RSA && !NO_DSA */

#ifdef OPENSSL_EXTRA

#ifndef NO_DSA
/* wolfSSL -> OpenSSL */
static int SetDsaExternal(WOLFSSL_DSA* dsa)
{
    DsaKey* key;
    WOLFSSL_MSG("Entering SetDsaExternal");

    if (dsa == NULL || dsa->internal == NULL) {
        WOLFSSL_MSG("dsa key NULL error");
        return WOLFSSL_FATAL_ERROR;
    }

    key = (DsaKey*)dsa->internal;

    if (SetIndividualExternal(&dsa->p, &key->p) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("dsa p key error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (SetIndividualExternal(&dsa->q, &key->q) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("dsa q key error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (SetIndividualExternal(&dsa->g, &key->g) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("dsa g key error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (SetIndividualExternal(&dsa->pub_key, &key->y) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("dsa y key error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (SetIndividualExternal(&dsa->priv_key, &key->x) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("dsa x key error");
        return WOLFSSL_FATAL_ERROR;
    }

    dsa->exSet = 1;

    return WOLFSSL_SUCCESS;
}

/* Openssl -> WolfSSL */
static int SetDsaInternal(WOLFSSL_DSA* dsa)
{
    DsaKey* key;
    WOLFSSL_MSG("Entering SetDsaInternal");

    if (dsa == NULL || dsa->internal == NULL) {
        WOLFSSL_MSG("dsa key NULL error");
        return WOLFSSL_FATAL_ERROR;
    }

    key = (DsaKey*)dsa->internal;

    if (dsa->p != NULL &&
        SetIndividualInternal(dsa->p, &key->p) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("rsa p key error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (dsa->q != NULL &&
        SetIndividualInternal(dsa->q, &key->q) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("rsa q key error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (dsa->g != NULL &&
        SetIndividualInternal(dsa->g, &key->g) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("rsa g key error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (dsa->pub_key != NULL) {
        if (SetIndividualInternal(dsa->pub_key, &key->y) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("rsa pub_key error");
            return WOLFSSL_FATAL_ERROR;
        }

        /* public key */
        key->type = DSA_PUBLIC;
    }

    if (dsa->priv_key != NULL) {
        if (SetIndividualInternal(dsa->priv_key, &key->x) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("rsa priv_key error");
            return WOLFSSL_FATAL_ERROR;
        }

        /* private key */
        key->type = DSA_PRIVATE;
    }

    dsa->inSet = 1;

    return WOLFSSL_SUCCESS;
}
#endif /* NO_DSA */
#endif /* OPENSSL_EXTRA */

#if !defined(HAVE_USER_RSA) && !defined(HAVE_FAST_RSA) && \
    !defined(NO_RSA) && (defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL))
/* WolfSSL -> OpenSSL */
static int SetRsaExternal(WOLFSSL_RSA* rsa)
{
    RsaKey* key;
    WOLFSSL_MSG("Entering SetRsaExternal");

    if (rsa == NULL || rsa->internal == NULL) {
        WOLFSSL_MSG("rsa key NULL error");
        return WOLFSSL_FATAL_ERROR;
    }

    key = (RsaKey*)rsa->internal;

    if (SetIndividualExternal(&rsa->n, &key->n) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("rsa n key error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (SetIndividualExternal(&rsa->e, &key->e) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("rsa e key error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (key->type == RSA_PRIVATE) {
        if (SetIndividualExternal(&rsa->d, &key->d) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("rsa d key error");
            return WOLFSSL_FATAL_ERROR;
        }

        if (SetIndividualExternal(&rsa->p, &key->p) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("rsa p key error");
            return WOLFSSL_FATAL_ERROR;
        }

        if (SetIndividualExternal(&rsa->q, &key->q) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("rsa q key error");
            return WOLFSSL_FATAL_ERROR;
        }

        if (SetIndividualExternal(&rsa->dmp1, &key->dP) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("rsa dP key error");
            return WOLFSSL_FATAL_ERROR;
        }

        if (SetIndividualExternal(&rsa->dmq1, &key->dQ) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("rsa dQ key error");
            return WOLFSSL_FATAL_ERROR;
        }

        if (SetIndividualExternal(&rsa->iqmp, &key->u) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("rsa u key error");
            return WOLFSSL_FATAL_ERROR;
        }
    }
    rsa->exSet = 1;

    return WOLFSSL_SUCCESS;
}
#endif

#ifdef OPENSSL_EXTRA
#if !defined(NO_RSA)
#if !defined(HAVE_USER_RSA) && !defined(HAVE_FAST_RSA)
/* Openssl -> WolfSSL */
static int SetRsaInternal(WOLFSSL_RSA* rsa)
{
    RsaKey* key;
    WOLFSSL_MSG("Entering SetRsaInternal");

    if (rsa == NULL || rsa->internal == NULL) {
        WOLFSSL_MSG("rsa key NULL error");
        return WOLFSSL_FATAL_ERROR;
    }

    key = (RsaKey*)rsa->internal;

    if (SetIndividualInternal(rsa->n, &key->n) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("rsa n key error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (SetIndividualInternal(rsa->e, &key->e) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("rsa e key error");
        return WOLFSSL_FATAL_ERROR;
    }

    /* public key */
    key->type = RSA_PUBLIC;

    if (rsa->d != NULL) {
        if (SetIndividualInternal(rsa->d, &key->d) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("rsa d key error");
            return WOLFSSL_FATAL_ERROR;
        }

        /* private key */
        key->type = RSA_PRIVATE;
    }

    if (rsa->p != NULL &&
        SetIndividualInternal(rsa->p, &key->p) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("rsa p key error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (rsa->q != NULL &&
        SetIndividualInternal(rsa->q, &key->q) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("rsa q key error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (rsa->dmp1 != NULL &&
        SetIndividualInternal(rsa->dmp1, &key->dP) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("rsa dP key error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (rsa->dmq1 != NULL &&
        SetIndividualInternal(rsa->dmq1, &key->dQ) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("rsa dQ key error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (rsa->iqmp != NULL &&
        SetIndividualInternal(rsa->iqmp, &key->u) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("rsa u key error");
        return WOLFSSL_FATAL_ERROR;
    }

    rsa->inSet = 1;

    return WOLFSSL_SUCCESS;
}


/* SSL_SUCCESS on ok */
#ifndef NO_WOLFSSL_STUB
int wolfSSL_RSA_blinding_on(WOLFSSL_RSA* rsa, WOLFSSL_BN_CTX* bn)
{
    (void)rsa;
    (void)bn;
    WOLFSSL_STUB("RSA_blinding_on");
    WOLFSSL_MSG("wolfSSL_RSA_blinding_on");

    return WOLFSSL_SUCCESS;  /* on by default */
}
#endif

/* return compliant with OpenSSL
 *   size of encrypted data if success , -1 if error
 */
int wolfSSL_RSA_public_encrypt(int len, const unsigned char* fr,
                            unsigned char* to, WOLFSSL_RSA* rsa, int padding)
{
    int initTmpRng = 0;
    WC_RNG *rng = NULL;
    int outLen;
    int ret = 0;
#ifdef WOLFSSL_SMALL_STACK
    WC_RNG* tmpRNG = NULL;
#else
    WC_RNG  tmpRNG[1];
#endif
#if !defined(HAVE_FIPS) && !defined(HAVE_USER_RSA) && !defined(HAVE_FAST_RSA)
    int  mgf = WC_MGF1NONE;
    enum wc_HashType hash = WC_HASH_TYPE_NONE;
#endif

    WOLFSSL_MSG("wolfSSL_RSA_public_encrypt");

    /* Check and remap the padding to internal values, if needed. */
#if !defined(HAVE_FIPS) && !defined(HAVE_USER_RSA) && !defined(HAVE_FAST_RSA)
    if (padding == RSA_PKCS1_PADDING)
        padding = WC_RSA_PKCSV15_PAD;
    else if (padding == RSA_PKCS1_OAEP_PADDING) {
        padding = WC_RSA_OAEP_PAD;
        hash = WC_HASH_TYPE_SHA;
        mgf = WC_MGF1SHA1;
    }
#else
    if (padding == RSA_PKCS1_PADDING)
      ;
#endif
    else {
        WOLFSSL_MSG("wolfSSL_RSA_public_encrypt unsupported padding");
        return 0;
    }

    if (rsa->inSet == 0)
    {
        if (SetRsaInternal(rsa) != SSL_SUCCESS) {
            WOLFSSL_MSG("SetRsaInternal failed");
            return 0;
        }
    }

    outLen = wolfSSL_RSA_size(rsa);

#if !defined(HAVE_FIPS) && !defined(HAVE_USER_RSA) && \
    !defined(HAVE_FAST_RSA) && defined(WC_RSA_BLINDING)
    rng = ((RsaKey*)rsa->internal)->rng;
#endif
    if (rng == NULL) {
#ifdef WOLFSSL_SMALL_STACK
        tmpRNG = (WC_RNG*)XMALLOC(sizeof(WC_RNG), NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (tmpRNG == NULL)
            return 0;
#endif

        if (wc_InitRng(tmpRNG) == 0) {
            rng = tmpRNG;
            initTmpRng = 1;
        }
        else {
            WOLFSSL_MSG("Bad RNG Init, trying global");
            if (initGlobalRNG == 0)
                WOLFSSL_MSG("Global RNG no Init");
            else
                rng = &globalRNG;
        }
    }

    if (outLen == 0) {
        WOLFSSL_MSG("Bad RSA size");
    }

    if (rng) {
#if !defined(HAVE_FIPS) && !defined(HAVE_USER_RSA) && !defined(HAVE_FAST_RSA)
        ret = wc_RsaPublicEncrypt_ex(fr, len, to, outLen,
                             (RsaKey*)rsa->internal, rng, padding,
                             hash, mgf, NULL, 0);
#else
        ret = wc_RsaPublicEncrypt(fr, len, to, outLen,
                             (RsaKey*)rsa->internal, rng);
#endif
        if (ret <= 0) {
            WOLFSSL_MSG("Bad Rsa Encrypt");
        }
        if (len <= 0) {
            WOLFSSL_MSG("Bad Rsa Encrypt");
        }
    }

    if (initTmpRng)
        wc_FreeRng(tmpRNG);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(tmpRNG,     NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    if (ret >= 0)
        WOLFSSL_MSG("wolfSSL_RSA_public_encrypt success");
    else {
        WOLFSSL_MSG("wolfSSL_RSA_public_encrypt failed");
        ret = WOLFSSL_FATAL_ERROR; /* return -1 on error case */
    }
    return ret;
}

/* return compliant with OpenSSL
 *   size of plain recovered data if success , -1 if error
 */
int wolfSSL_RSA_private_decrypt(int len, const unsigned char* fr,
                            unsigned char* to, WOLFSSL_RSA* rsa, int padding)
{
    int outLen;
    int ret = 0;
  #if !defined(HAVE_FIPS) && !defined(HAVE_USER_RSA) && !defined(HAVE_FAST_RSA)
    int mgf = WC_MGF1NONE;
    enum wc_HashType hash = WC_HASH_TYPE_NONE;
  #endif

    WOLFSSL_MSG("wolfSSL_RSA_private_decrypt");

#if !defined(HAVE_FIPS) && !defined(HAVE_USER_RSA) && !defined(HAVE_FAST_RSA)
    if (padding == RSA_PKCS1_PADDING)
        padding = WC_RSA_PKCSV15_PAD;
    else if (padding == RSA_PKCS1_OAEP_PADDING) {
        padding = WC_RSA_OAEP_PAD;
        hash = WC_HASH_TYPE_SHA;
        mgf = WC_MGF1SHA1;
    }
#else
    if (padding == RSA_PKCS1_PADDING)
        ;
#endif
    else {
        WOLFSSL_MSG("wolfSSL_RSA_private_decrypt unsupported padding");
        return 0;
    }

    if (rsa->inSet == 0)
    {
        if (SetRsaInternal(rsa) != SSL_SUCCESS) {
            WOLFSSL_MSG("SetRsaInternal failed");
            return 0;
        }
    }

    outLen = wolfSSL_RSA_size(rsa);
    if (outLen == 0) {
        WOLFSSL_MSG("Bad RSA size");
    }

    /* size of 'to' buffer must be size of RSA key */
#if !defined(HAVE_FIPS) && !defined(HAVE_USER_RSA) && !defined(HAVE_FAST_RSA)
    ret = wc_RsaPrivateDecrypt_ex(fr, len, to, outLen,
                            (RsaKey*)rsa->internal, padding,
                            hash, mgf, NULL, 0);
#else
    ret = wc_RsaPrivateDecrypt(fr, len, to, outLen,
                            (RsaKey*)rsa->internal);
#endif

    if (len <= 0) {
        WOLFSSL_MSG("Bad Rsa Decrypt");
    }

    if (ret > 0)
        WOLFSSL_MSG("wolfSSL_RSA_private_decrypt success");
    else {
        WOLFSSL_MSG("wolfSSL_RSA_private_decrypt failed");
        ret = WOLFSSL_FATAL_ERROR;
    }
    return ret;
}


/* RSA private encrypt calls wc_RsaSSL_Sign. Similar function set up as RSA
 * public decrypt.
 *
 * len  Length of input buffer
 * in   Input buffer to sign
 * out  Output buffer (expected to be greater than or equal to RSA key size)
 * rsa     Key to use for encryption
 * padding Type of RSA padding to use.
 */
int wolfSSL_RSA_private_encrypt(int len, unsigned char* in,
                            unsigned char* out, WOLFSSL_RSA* rsa, int padding)
{
    int sz = 0;
    WC_RNG* rng = NULL;
    RsaKey* key;

    WOLFSSL_MSG("wolfSSL_RSA_private_encrypt");

    if (len < 0 || rsa == NULL || rsa->internal == NULL || in == NULL) {
        WOLFSSL_MSG("Bad function arguments");
        return 0;
    }

    if (padding != RSA_PKCS1_PADDING) {
        WOLFSSL_MSG("wolfSSL_RSA_private_encrypt unsupported padding");
        return 0;
    }

    if (rsa->inSet == 0)
    {
        WOLFSSL_MSG("Setting internal RSA structure");

        if (SetRsaInternal(rsa) != SSL_SUCCESS) {
            WOLFSSL_MSG("SetRsaInternal failed");
            return 0;
        }
    }

    key = (RsaKey*)rsa->internal;
    #if defined(WC_RSA_BLINDING) && !defined(HAVE_USER_RSA)
    rng = key->rng;
    #else
#ifndef HAVE_FIPS
    if (wc_InitRng_ex(rng, key->heap, INVALID_DEVID) != 0) {
#else
    if (wc_InitRng(rng) != 0) {
#endif
        WOLFSSL_MSG("Error with random number");
        return SSL_FATAL_ERROR;
    }
    #endif

    /* size of output buffer must be size of RSA key */
    sz = wc_RsaSSL_Sign(in, (word32)len, out, wolfSSL_RSA_size(rsa), key, rng);
    #if !defined(WC_RSA_BLINDING) || defined(HAVE_USER_RSA)
    if (wc_FreeRng(rng) != 0) {
        WOLFSSL_MSG("Error freeing random number generator");
        return SSL_FATAL_ERROR;
    }
    #endif
    if (sz <= 0) {
        WOLFSSL_LEAVE("wolfSSL_RSA_private_encrypt", sz);
        return 0;
    }

    return sz;
}
#endif /* HAVE_USER_RSA */

/* return compliant with OpenSSL
 *   RSA modulus size in bytes, -1 if error
 */
int wolfSSL_RSA_size(const WOLFSSL_RSA* rsa)
{
    WOLFSSL_ENTER("wolfSSL_RSA_size");

    if (rsa == NULL)
        return WOLFSSL_FATAL_ERROR;
    if (rsa->inSet == 0)
    {
        if (SetRsaInternal((WOLFSSL_RSA*)rsa) != SSL_SUCCESS) {
            WOLFSSL_MSG("SetRsaInternal failed");
            return 0;
        }
    }
    return wc_RsaEncryptSize((RsaKey*)rsa->internal);
}


/* Generates a RSA key of length len
 *
 * len  length of RSA key i.e. 2048
 * e    e to use when generating RSA key
 * f    callback function for generation details
 * data user callback argument
 *
 * Note: Because of wc_MakeRsaKey an RSA key size generated can be slightly
 *       rounded down. For example generating a key of size 2999 with e =
 *       65537 will make a key of size 374 instead of 375.
 * Returns a new RSA key on success and NULL on failure
 */
WOLFSSL_RSA* wolfSSL_RSA_generate_key(int len, unsigned long e,
                                      void(*f)(int, int, void*), void* data)
{
    WOLFSSL_RSA*    rsa = NULL;
    WOLFSSL_BIGNUM* bn  = NULL;

    WOLFSSL_ENTER("wolfSSL_RSA_generate_key");

    (void)f;
    (void)data;

    if (len < 0) {
        WOLFSSL_MSG("Bad argument: length was less than 0");
        return NULL;
    }

    bn = wolfSSL_BN_new();
    if (bn == NULL) {
        WOLFSSL_MSG("Error creating big number");
        return NULL;
    }

    if (wolfSSL_BN_set_word(bn, (WOLFSSL_BN_ULONG)e) != SSL_SUCCESS) {
        WOLFSSL_MSG("Error using e value");
        wolfSSL_BN_free(bn);
        return NULL;
    }

    rsa = wolfSSL_RSA_new();
    if (rsa == NULL) {
        WOLFSSL_MSG("memory error");
    }
    else {
        if (wolfSSL_RSA_generate_key_ex(rsa, len, bn, NULL) != SSL_SUCCESS){
            wolfSSL_RSA_free(rsa);
            rsa = NULL;
        }
    }
    wolfSSL_BN_free(bn);

    return rsa;
}


/* return compliant with OpenSSL
 *   1 if success, 0 if error
 */
int wolfSSL_RSA_generate_key_ex(WOLFSSL_RSA* rsa, int bits, WOLFSSL_BIGNUM* bn,
                                void* cb)
{
    int ret = WOLFSSL_FAILURE;

    (void)cb;
    (void)bn;
    (void)bits;

    WOLFSSL_ENTER("wolfSSL_RSA_generate_key_ex");

    if (rsa == NULL || rsa->internal == NULL) {
        /* bit size checked during make key call */
        WOLFSSL_MSG("bad arguments");
        return WOLFSSL_FAILURE;
    }

#ifdef WOLFSSL_KEY_GEN
    {
    #ifdef WOLFSSL_SMALL_STACK
        WC_RNG* rng = NULL;
    #else
        WC_RNG  rng[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        rng = (WC_RNG*)XMALLOC(sizeof(WC_RNG), NULL, DYNAMIC_TYPE_RNG);
        if (rng == NULL)
            return WOLFSSL_FAILURE;
    #endif

        if (wc_InitRng(rng) < 0)
            WOLFSSL_MSG("RNG init failed");
        else if (wc_MakeRsaKey((RsaKey*)rsa->internal, bits,
                    wolfSSL_BN_get_word(bn), rng) != MP_OKAY)
            WOLFSSL_MSG("wc_MakeRsaKey failed");
        else if (SetRsaExternal(rsa) != WOLFSSL_SUCCESS)
            WOLFSSL_MSG("SetRsaExternal failed");
        else {
            rsa->inSet = 1;
            ret = WOLFSSL_SUCCESS;
        }

        wc_FreeRng(rng);
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(rng, NULL, DYNAMIC_TYPE_RNG);
    #endif
    }
#else
    WOLFSSL_MSG("No Key Gen built in");
#endif
    return ret;
}
#endif /* NO_RSA */

#ifndef NO_DSA
/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
int wolfSSL_DSA_generate_key(WOLFSSL_DSA* dsa)
{
    int ret = WOLFSSL_FAILURE;

    WOLFSSL_ENTER("wolfSSL_DSA_generate_key");

    if (dsa == NULL || dsa->internal == NULL) {
        WOLFSSL_MSG("Bad arguments");
        return WOLFSSL_FAILURE;
    }

    if (dsa->inSet == 0) {
        WOLFSSL_MSG("No DSA internal set, do it");

        if (SetDsaInternal(dsa) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetDsaInternal failed");
            return ret;
        }
    }

#ifdef WOLFSSL_KEY_GEN
    {
        int initTmpRng = 0;
        WC_RNG *rng = NULL;
#ifdef WOLFSSL_SMALL_STACK
        WC_RNG *tmpRNG = NULL;
#else
        WC_RNG tmpRNG[1];
#endif

#ifdef WOLFSSL_SMALL_STACK
        tmpRNG = (WC_RNG*)XMALLOC(sizeof(WC_RNG), NULL, DYNAMIC_TYPE_RNG);
        if (tmpRNG == NULL)
            return WOLFSSL_FATAL_ERROR;
#endif
        if (wc_InitRng(tmpRNG) == 0) {
            rng = tmpRNG;
            initTmpRng = 1;
        }
        else {
            WOLFSSL_MSG("Bad RNG Init, trying global");
            if (initGlobalRNG == 0)
                WOLFSSL_MSG("Global RNG no Init");
            else
                rng = &globalRNG;
        }

        if (rng) {
            if (wc_MakeDsaKey(rng, (DsaKey*)dsa->internal) != MP_OKAY)
                WOLFSSL_MSG("wc_MakeDsaKey failed");
            else if (SetDsaExternal(dsa) != WOLFSSL_SUCCESS)
                WOLFSSL_MSG("SetDsaExternal failed");
            else
                ret = WOLFSSL_SUCCESS;
        }

        if (initTmpRng)
            wc_FreeRng(tmpRNG);

#ifdef WOLFSSL_SMALL_STACK
        XFREE(tmpRNG, NULL, DYNAMIC_TYPE_RNG);
#endif
    }
#else /* WOLFSSL_KEY_GEN */
    WOLFSSL_MSG("No Key Gen built in");
#endif
    return ret;
}


/* Returns a pointer to a new WOLFSSL_DSA structure on success and NULL on fail
 */
WOLFSSL_DSA* wolfSSL_DSA_generate_parameters(int bits, unsigned char* seed,
        int seedLen, int* counterRet, unsigned long* hRet,
        WOLFSSL_BN_CB cb, void* CBArg)
{
    WOLFSSL_DSA* dsa;

    WOLFSSL_ENTER("wolfSSL_DSA_generate_parameters()");

    (void)cb;
    (void)CBArg;
    dsa = wolfSSL_DSA_new();
    if (dsa == NULL) {
        return NULL;
    }

    if (wolfSSL_DSA_generate_parameters_ex(dsa, bits, seed, seedLen,
                                  counterRet, hRet, NULL) != SSL_SUCCESS) {
        wolfSSL_DSA_free(dsa);
        return NULL;
    }

    return dsa;
}


/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
int wolfSSL_DSA_generate_parameters_ex(WOLFSSL_DSA* dsa, int bits,
                                       unsigned char* seed, int seedLen,
                                       int* counterRet,
                                       unsigned long* hRet, void* cb)
{
    int ret = WOLFSSL_FAILURE;

    (void)bits;
    (void)seed;
    (void)seedLen;
    (void)counterRet;
    (void)hRet;
    (void)cb;

    WOLFSSL_ENTER("wolfSSL_DSA_generate_parameters_ex");

    if (dsa == NULL || dsa->internal == NULL) {
        WOLFSSL_MSG("Bad arguments");
        return WOLFSSL_FAILURE;
    }

#ifdef WOLFSSL_KEY_GEN
    {
        int initTmpRng = 0;
        WC_RNG *rng = NULL;
#ifdef WOLFSSL_SMALL_STACK
        WC_RNG *tmpRNG = NULL;
#else
        WC_RNG tmpRNG[1];
#endif

#ifdef WOLFSSL_SMALL_STACK
        tmpRNG = (WC_RNG*)XMALLOC(sizeof(WC_RNG), NULL, DYNAMIC_TYPE_RNG);
        if (tmpRNG == NULL)
            return WOLFSSL_FATAL_ERROR;
#endif
        if (wc_InitRng(tmpRNG) == 0) {
            rng = tmpRNG;
            initTmpRng = 1;
        }
        else {
            WOLFSSL_MSG("Bad RNG Init, trying global");
            if (initGlobalRNG == 0)
                WOLFSSL_MSG("Global RNG no Init");
            else
                rng = &globalRNG;
        }

        if (rng) {
            if (wc_MakeDsaParameters(rng, bits,
                                     (DsaKey*)dsa->internal) != MP_OKAY)
                WOLFSSL_MSG("wc_MakeDsaParameters failed");
            else if (SetDsaExternal(dsa) != WOLFSSL_SUCCESS)
                WOLFSSL_MSG("SetDsaExternal failed");
            else
                ret = WOLFSSL_SUCCESS;
        }

        if (initTmpRng)
            wc_FreeRng(tmpRNG);

#ifdef WOLFSSL_SMALL_STACK
        XFREE(tmpRNG, NULL, DYNAMIC_TYPE_RNG);
#endif
    }
#else /* WOLFSSL_KEY_GEN */
    WOLFSSL_MSG("No Key Gen built in");
#endif

    return ret;
}

/* return WOLFSSL_SUCCESS on success, < 0 otherwise */
int wolfSSL_DSA_do_sign(const unsigned char* d, unsigned char* sigRet,
                       WOLFSSL_DSA* dsa)
{
    int     ret = WOLFSSL_FATAL_ERROR;
    int     initTmpRng = 0;
    WC_RNG* rng = NULL;
#ifdef WOLFSSL_SMALL_STACK
    WC_RNG* tmpRNG = NULL;
#else
    WC_RNG  tmpRNG[1];
#endif

    WOLFSSL_ENTER("wolfSSL_DSA_do_sign");

    if (d == NULL || sigRet == NULL || dsa == NULL) {
        WOLFSSL_MSG("Bad function arguments");
        return ret;
    }

    if (dsa->inSet == 0)
    {
        WOLFSSL_MSG("No DSA internal set, do it");

        if (SetDsaInternal(dsa) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetDsaInternal failed");
            return ret;
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    tmpRNG = (WC_RNG*)XMALLOC(sizeof(WC_RNG), NULL, DYNAMIC_TYPE_RNG);
    if (tmpRNG == NULL)
        return WOLFSSL_FATAL_ERROR;
#endif

    if (wc_InitRng(tmpRNG) == 0) {
        rng = tmpRNG;
        initTmpRng = 1;
    }
    else {
        WOLFSSL_MSG("Bad RNG Init, trying global");
        if (initGlobalRNG == 0)
            WOLFSSL_MSG("Global RNG no Init");
        else
            rng = &globalRNG;
    }

    if (rng) {
        if (DsaSign(d, sigRet, (DsaKey*)dsa->internal, rng) < 0)
            WOLFSSL_MSG("DsaSign failed");
        else
            ret = WOLFSSL_SUCCESS;
    }

    if (initTmpRng)
        wc_FreeRng(tmpRNG);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(tmpRNG, NULL, DYNAMIC_TYPE_RNG);
#endif

    return ret;
}


int wolfSSL_DSA_do_verify(const unsigned char* d, unsigned char* sig,
                        WOLFSSL_DSA* dsa, int *dsacheck)
{
    int    ret = WOLFSSL_FATAL_ERROR;

    WOLFSSL_ENTER("wolfSSL_DSA_do_verify");

    if (d == NULL || sig == NULL || dsa == NULL) {
        WOLFSSL_MSG("Bad function arguments");
        return WOLFSSL_FATAL_ERROR;
    }
    if (dsa->inSet == 0)
    {
        WOLFSSL_MSG("No DSA internal set, do it");

        if (SetDsaInternal(dsa) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetDsaInternal failed");
            return WOLFSSL_FATAL_ERROR;
        }
    }

    ret = DsaVerify(d, sig, (DsaKey*)dsa->internal, dsacheck);
    if (ret != 0 || *dsacheck != 1) {
        WOLFSSL_MSG("DsaVerify failed");
        return ret;
    }

    return WOLFSSL_SUCCESS;
}
#endif /* NO_DSA */


#if !defined(NO_RSA) && !defined(HAVE_USER_RSA)

#ifdef DEBUG_SIGN
static void show(const char *title, const unsigned char *out, unsigned int outlen)
{
    const unsigned char *pt;
    printf("%s[%d] = \n", title, (int)outlen);
    outlen = outlen>100?100:outlen;
    for (pt = out; pt < out + outlen;
            printf("%c", ((*pt)&0x6f)>='A'?((*pt)&0x6f):'.'), pt++);
    printf("\n");
}
#else
#define show(a,b,c)
#endif

/* return SSL_SUCCES on ok, 0 otherwise */
int wolfSSL_RSA_sign(int type, const unsigned char* m,
                           unsigned int mLen, unsigned char* sigRet,
                           unsigned int* sigLen, WOLFSSL_RSA* rsa)
{
    return wolfSSL_RSA_sign_ex(type, m, mLen, sigRet, sigLen, rsa, 1);
}

int wolfSSL_RSA_sign_ex(int type, const unsigned char* m,
                           unsigned int mLen, unsigned char* sigRet,
                           unsigned int* sigLen, WOLFSSL_RSA* rsa, int flag)
{
    word32  outLen;
    word32  signSz;
    int     initTmpRng = 0;
    WC_RNG* rng        = NULL;
    int     ret        = 0;
#ifdef WOLFSSL_SMALL_STACK
    WC_RNG* tmpRNG     = NULL;
    byte*   encodedSig = NULL;
#else
    WC_RNG  tmpRNG[1];
    byte    encodedSig[MAX_ENCODED_SIG_SZ];
#endif

    WOLFSSL_ENTER("wolfSSL_RSA_sign");

    if (m == NULL || sigRet == NULL || sigLen == NULL || rsa == NULL) {
        WOLFSSL_MSG("Bad function arguments");
        return 0;
    }
    show("Message to Sign", m, mLen);

    switch (type) {
    #ifdef WOLFSSL_MD2
        case NID_md2:       type = MD2h;    break;
    #endif
    #ifndef NO_MD5
        case NID_md5:       type = MD5h;    break;
    #endif
    #ifndef NO_SHA
        case NID_sha1:      type = SHAh;    break;
    #endif
    #ifndef NO_SHA256
        case NID_sha256:    type = SHA256h; break;
    #endif
    #ifdef WOLFSSL_SHA384
        case NID_sha384:    type = SHA384h; break;
    #endif
    #ifdef WOLFSSL_SHA512
        case NID_sha512:    type = SHA512h; break;
    #endif
        default:
            WOLFSSL_MSG("This NID (md type) not configured or not implemented");
            return 0;
    }

    if (rsa->inSet == 0)
    {
        WOLFSSL_MSG("No RSA internal set, do it");

        if (SetRsaInternal(rsa) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetRsaInternal failed");
            return 0;
        }
    }

    outLen = (word32)wolfSSL_BN_num_bytes(rsa->n);

#ifdef WOLFSSL_SMALL_STACK
    tmpRNG = (WC_RNG*)XMALLOC(sizeof(WC_RNG), NULL, DYNAMIC_TYPE_RNG);
    if (tmpRNG == NULL)
        return 0;

    encodedSig = (byte*)XMALLOC(MAX_ENCODED_SIG_SZ, NULL,
                                                   DYNAMIC_TYPE_SIGNATURE);
    if (encodedSig == NULL) {
        XFREE(tmpRNG, NULL, DYNAMIC_TYPE_RNG);
        return 0;
    }
#endif

    if (outLen == 0)
        WOLFSSL_MSG("Bad RSA size");
    else if (wc_InitRng(tmpRNG) == 0) {
        rng = tmpRNG;
        initTmpRng = 1;
    }
    else {
        WOLFSSL_MSG("Bad RNG Init, trying global");

        if (initGlobalRNG == 0)
            WOLFSSL_MSG("Global RNG no Init");
        else
            rng = &globalRNG;
    }

    if (rng) {

        signSz = wc_EncodeSignature(encodedSig, m, mLen, type);
        if (signSz == 0) {
            WOLFSSL_MSG("Bad Encode Signature");
        }
        else {
            show("Encoded Message", encodedSig, signSz);
            if (flag != 0) {
                ret = wc_RsaSSL_Sign(encodedSig, signSz, sigRet, outLen,
                                (RsaKey*)rsa->internal, rng);
                if (ret <= 0) {
                    WOLFSSL_MSG("Bad Rsa Sign");
                    ret = 0;
                }
                else {
                    *sigLen = (unsigned int)ret;
                    ret = SSL_SUCCESS;
                    show("Signature", sigRet, *sigLen);
                }
            } else {
                ret = SSL_SUCCESS;
                XMEMCPY(sigRet, encodedSig, signSz);
                *sigLen = signSz;
            }
        }

    }

    if (initTmpRng)
        wc_FreeRng(tmpRNG);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(tmpRNG,     NULL, DYNAMIC_TYPE_RNG);
    XFREE(encodedSig, NULL, DYNAMIC_TYPE_SIGNATURE);
#endif

    if (ret == WOLFSSL_SUCCESS)
        WOLFSSL_MSG("wolfSSL_RSA_sign success");
    else {
        WOLFSSL_MSG("wolfSSL_RSA_sign failed");
    }
    return ret;
}


/* returns WOLFSSL_SUCCESS on successful verify and WOLFSSL_FAILURE on fail */
int wolfSSL_RSA_verify(int type, const unsigned char* m,
                               unsigned int mLen, const unsigned char* sig,
                               unsigned int sigLen, WOLFSSL_RSA* rsa)
{
    int     ret;
    unsigned char *sigRet ;
    unsigned char *sigDec ;
    unsigned int   len;

    WOLFSSL_ENTER("wolfSSL_RSA_verify");
    if ((m == NULL) || (sig == NULL)) {
        WOLFSSL_MSG("Bad function arguments");
        return WOLFSSL_FAILURE;
    }

    sigRet = (unsigned char *)XMALLOC(sigLen, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (sigRet == NULL) {
        WOLFSSL_MSG("Memory failure");
        return WOLFSSL_FAILURE;
    }
    sigDec = (unsigned char *)XMALLOC(sigLen, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (sigDec == NULL) {
        WOLFSSL_MSG("Memory failure");
        XFREE(sigRet, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return WOLFSSL_FAILURE;
    }
    /* get non-encrypted signature to be compared with decrypted sugnature*/
    ret = wolfSSL_RSA_sign_ex(type, m, mLen, sigRet, &len, rsa, 0);
    if (ret <= 0) {
        WOLFSSL_MSG("Message Digest Error");
        XFREE(sigRet, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(sigDec, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return WOLFSSL_FAILURE;
    }
    show("Encoded Message", sigRet, len);
    /* decrypt signature */
    ret = wc_RsaSSL_Verify(sig, sigLen, (unsigned char *)sigDec, sigLen,
        (RsaKey*)rsa->internal);
    if (ret <= 0) {
        WOLFSSL_MSG("RSA Decrypt error");
        XFREE(sigRet, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(sigDec, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return WOLFSSL_FAILURE;
    }
    show("Decrypted Signature", sigDec, ret);

    if ((int)len == ret && XMEMCMP(sigRet, sigDec, ret) == 0) {
        WOLFSSL_MSG("wolfSSL_RSA_verify success");
        XFREE(sigRet, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(sigDec, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return WOLFSSL_SUCCESS;
    }
    else {
        WOLFSSL_MSG("wolfSSL_RSA_verify failed");
        XFREE(sigRet, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(sigDec, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return WOLFSSL_FAILURE;
    }
}

int wolfSSL_RSA_public_decrypt(int flen, const unsigned char* from,
                          unsigned char* to, WOLFSSL_RSA* rsa, int padding)
{
    int tlen = 0;

    WOLFSSL_ENTER("wolfSSL_RSA_public_decrypt");

    if (rsa == NULL || rsa->internal == NULL || from == NULL) {
        WOLFSSL_MSG("Bad function arguments");
        return 0;
    }

    if (padding != RSA_PKCS1_PADDING) {
        WOLFSSL_MSG("wolfSSL_RSA_public_decrypt unsupported padding");
        return 0;
    }

    if (rsa->inSet == 0)
    {
        WOLFSSL_MSG("No RSA internal set, do it");

        if (SetRsaInternal(rsa) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetRsaInternal failed");
            return 0;
        }
    }

    /* size of 'to' buffer must be size of RSA key */
    tlen = wc_RsaSSL_Verify(from, flen, to, wolfSSL_RSA_size(rsa),
                            (RsaKey*)rsa->internal);
    if (tlen <= 0)
        WOLFSSL_MSG("wolfSSL_RSA_public_decrypt failed");
    else {
        WOLFSSL_MSG("wolfSSL_RSA_public_decrypt success");
    }
    return tlen;
}


/* generate p-1 and q-1, WOLFSSL_SUCCESS on ok */
int wolfSSL_RSA_GenAdd(WOLFSSL_RSA* rsa)
{
    int    err;
    mp_int tmp;

    WOLFSSL_MSG("wolfSSL_RsaGenAdd");

    if (rsa == NULL || rsa->p == NULL || rsa->q == NULL || rsa->d == NULL ||
                       rsa->dmp1 == NULL || rsa->dmq1 == NULL) {
        WOLFSSL_MSG("rsa no init error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (mp_init(&tmp) != MP_OKAY) {
        WOLFSSL_MSG("mp_init error");
        return WOLFSSL_FATAL_ERROR;
    }

    err = mp_sub_d((mp_int*)rsa->p->internal, 1, &tmp);
    if (err != MP_OKAY) {
        WOLFSSL_MSG("mp_sub_d error");
    }
    else
        err = mp_mod((mp_int*)rsa->d->internal, &tmp,
                     (mp_int*)rsa->dmp1->internal);

    if (err != MP_OKAY) {
        WOLFSSL_MSG("mp_mod error");
    }
    else
        err = mp_sub_d((mp_int*)rsa->q->internal, 1, &tmp);
    if (err != MP_OKAY) {
        WOLFSSL_MSG("mp_sub_d error");
    }
    else
        err = mp_mod((mp_int*)rsa->d->internal, &tmp,
                     (mp_int*)rsa->dmq1->internal);

    mp_clear(&tmp);

    if (err == MP_OKAY)
        return WOLFSSL_SUCCESS;
    else
        return WOLFSSL_FATAL_ERROR;
}
#endif /* NO_RSA */

int wolfSSL_HMAC_CTX_Init(WOLFSSL_HMAC_CTX* ctx)
{
    WOLFSSL_MSG("wolfSSL_HMAC_CTX_Init");

    if (ctx != NULL) {
        /* wc_HmacSetKey sets up ctx->hmac */
        XMEMSET(ctx, 0, sizeof(WOLFSSL_HMAC_CTX));
    }

    return WOLFSSL_SUCCESS;
}


int wolfSSL_HMAC_Init_ex(WOLFSSL_HMAC_CTX* ctx, const void* key,
                             int keylen, const EVP_MD* type, WOLFSSL_ENGINE* e)
{
    WOLFSSL_ENTER("wolfSSL_HMAC_Init_ex");

    /* WOLFSSL_ENGINE not used, call wolfSSL_HMAC_Init */
    (void)e;
    return wolfSSL_HMAC_Init(ctx, key, keylen, type);
}


/* Deep copy of information from src to des structure
 *
 * des destination to copy information to
 * src structure to get infromation from
 *
 * Returns SSL_SUCCESS on success and SSL_FAILURE on error
 */
int wolfSSL_HMAC_CTX_copy(WOLFSSL_HMAC_CTX* des, WOLFSSL_HMAC_CTX* src)
{
    void* heap = NULL;

    WOLFSSL_ENTER("wolfSSL_HMAC_CTX_copy");

    if (des == NULL || src == NULL) {
        return SSL_FAILURE;
    }

#ifndef HAVE_FIPS
    heap = src->hmac.heap;
#endif

    if (wc_HmacInit(&des->hmac, heap, 0) != 0) {
        WOLFSSL_MSG("Error initializing HMAC");
        return SSL_FAILURE;
    }

    des->type = src->type;

    /* requires that hash structures have no dynamic parts to them */
    switch (src->hmac.macType) {
    #ifndef NO_MD5
        case WC_MD5:
            XMEMCPY(&des->hmac.hash.md5, &src->hmac.hash.md5, sizeof(wc_Md5));
            break;
    #endif /* !NO_MD5 */

    #ifndef NO_SHA
        case WC_SHA:
            XMEMCPY(&des->hmac.hash.sha, &src->hmac.hash.sha, sizeof(wc_Sha));
            break;
    #endif /* !NO_SHA */

    #ifdef WOLFSSL_SHA224
        case WC_SHA224:
            XMEMCPY(&des->hmac.hash.sha224, &src->hmac.hash.sha224,
                    sizeof(wc_Sha224));
            break;
    #endif /* WOLFSSL_SHA224 */

    #ifndef NO_SHA256
        case WC_SHA256:
            XMEMCPY(&des->hmac.hash.sha256, &src->hmac.hash.sha256,
                    sizeof(wc_Sha256));
            break;
    #endif /* !NO_SHA256 */

    #ifdef WOLFSSL_SHA512
    #ifdef WOLFSSL_SHA384
        case WC_SHA384:
            XMEMCPY(&des->hmac.hash.sha384, &src->hmac.hash.sha384,
                    sizeof(wc_Sha384));
            break;
    #endif /* WOLFSSL_SHA384 */
        case WC_SHA512:
            XMEMCPY(&des->hmac.hash.sha512, &src->hmac.hash.sha512,
                    sizeof(wc_Sha512));
            break;
    #endif /* WOLFSSL_SHA512 */

        default:
            WOLFSSL_MSG("Unknown or unsupported hash type");
            return WOLFSSL_FAILURE;
    }

    XMEMCPY((byte*)des->hmac.ipad, (byte*)src->hmac.ipad, WC_HMAC_BLOCK_SIZE);
    XMEMCPY((byte*)des->hmac.opad, (byte*)src->hmac.opad, WC_HMAC_BLOCK_SIZE);
    XMEMCPY((byte*)des->hmac.innerHash, (byte*)src->hmac.innerHash,
                                                            WC_MAX_DIGEST_SIZE);
#ifndef HAVE_FIPS
    des->hmac.heap    = heap;
#endif
    des->hmac.macType = src->hmac.macType;
    des->hmac.innerHashKeyed = src->hmac.innerHashKeyed;
    XMEMCPY((byte *)&des->save_ipad, (byte *)&src->hmac.ipad,
                                        WC_HMAC_BLOCK_SIZE);
    XMEMCPY((byte *)&des->save_opad, (byte *)&src->hmac.opad,
                                        WC_HMAC_BLOCK_SIZE);

#ifdef WOLFSSL_ASYNC_CRYPT
    XMEMCPY(&des->hmac.asyncDev, &src->hmac.asyncDev, sizeof(WC_ASYNC_DEV));
    des->hmac.keyLen = src->hmac.keyLen;
    #ifdef HAVE_CAVIUM
        des->hmac.data = (byte*)XMALLOC(src->hmac.dataLen, des->hmac.heap,
                DYNAMIC_TYPE_HMAC);
        if (des->hmac.data == NULL) {
            return BUFFER_E;
        }
        XMEMCPY(des->hmac.data, src->hmac.data, src->hmac.dataLen);
        des->hmac.dataLen = src->hmac.dataLen;
    #endif /* HAVE_CAVIUM */
#endif /* WOLFSSL_ASYNC_CRYPT */
        return WOLFSSL_SUCCESS;
}

#ifdef HAVE_FIPS
int _InitHmac(Hmac* hmac, int type, void* heap)
{
    int ret = 0;

    switch (type) {
    #ifndef NO_MD5
        case WC_MD5:
            ret = wc_InitMd5(&hmac->hash.md5);
            break;
    #endif /* !NO_MD5 */

    #ifndef NO_SHA
        case WC_SHA:
            ret = wc_InitSha(&hmac->hash.sha);
            break;
    #endif /* !NO_SHA */

    #ifdef WOLFSSL_SHA224
        case WC_SHA224:
            ret = wc_InitSha224(&hmac->hash.sha224);
            break;
    #endif /* WOLFSSL_SHA224 */

    #ifndef NO_SHA256
        case WC_SHA256:
            ret = wc_InitSha256(&hmac->hash.sha256);
            break;
    #endif /* !NO_SHA256 */

    #ifdef WOLFSSL_SHA512
    #ifdef WOLFSSL_SHA384
        case WC_SHA384:
            ret = wc_InitSha384(&hmac->hash.sha384);
            break;
    #endif /* WOLFSSL_SHA384 */
        case WC_SHA512:
            ret = wc_InitSha512(&hmac->hash.sha512);
            break;
    #endif /* WOLFSSL_SHA512 */

    #ifdef HAVE_BLAKE2
        case BLAKE2B_ID:
            ret = wc_InitBlake2b(&hmac->hash.blake2b, BLAKE2B_256);
            break;
    #endif /* HAVE_BLAKE2 */

        default:
            ret = BAD_FUNC_ARG;
            break;
    }

    (void)heap;

    return ret;
}
#endif /* HAVE_FIPS */


int wolfSSL_HMAC_Init(WOLFSSL_HMAC_CTX* ctx, const void* key, int keylen,
                  const EVP_MD* type)
{
    int hmac_error = 0;
    void* heap = NULL;

    WOLFSSL_MSG("wolfSSL_HMAC_Init");

    if (ctx == NULL) {
        WOLFSSL_MSG("no ctx on init");
        return WOLFSSL_FAILURE;
    }

#ifndef HAVE_FIPS
    heap = ctx->hmac.heap;
#endif

    if (type) {
        WOLFSSL_MSG("init has type");

#ifndef NO_MD5
        if (XSTRNCMP(type, "MD5", 3) == 0) {
            WOLFSSL_MSG("md5 hmac");
            ctx->type = WC_MD5;
        }
        else
#endif
#ifdef WOLFSSL_SHA224
        if (XSTRNCMP(type, "SHA224", 6) == 0) {
            WOLFSSL_MSG("sha224 hmac");
            ctx->type = WC_SHA224;
        }
        else
#endif
#ifndef NO_SHA256
        if (XSTRNCMP(type, "SHA256", 6) == 0) {
            WOLFSSL_MSG("sha256 hmac");
            ctx->type = WC_SHA256;
        }
        else
#endif
#ifdef WOLFSSL_SHA384
        if (XSTRNCMP(type, "SHA384", 6) == 0) {
            WOLFSSL_MSG("sha384 hmac");
            ctx->type = WC_SHA384;
        }
        else
#endif
#ifdef WOLFSSL_SHA512
        if (XSTRNCMP(type, "SHA512", 6) == 0) {
            WOLFSSL_MSG("sha512 hmac");
            ctx->type = WC_SHA512;
        }
        else
#endif

#ifndef NO_SHA
        /* has to be last since would pick or 256, 384, or 512 too */
        if (XSTRNCMP(type, "SHA", 3) == 0) {
            WOLFSSL_MSG("sha hmac");
            ctx->type = WC_SHA;
        }
        else
#endif
        {
            WOLFSSL_MSG("bad init type");
            return WOLFSSL_FAILURE;
        }
    }

    if (key && keylen) {
        WOLFSSL_MSG("keying hmac");

        if (wc_HmacInit(&ctx->hmac, NULL, INVALID_DEVID) == 0) {
            hmac_error = wc_HmacSetKey(&ctx->hmac, ctx->type, (const byte*)key,
                                       (word32)keylen);
            if (hmac_error < 0){
                wc_HmacFree(&ctx->hmac);
                return WOLFSSL_FAILURE;
            }
            XMEMCPY((byte *)&ctx->save_ipad, (byte *)&ctx->hmac.ipad,
                                        WC_HMAC_BLOCK_SIZE);
            XMEMCPY((byte *)&ctx->save_opad, (byte *)&ctx->hmac.opad,
                                        WC_HMAC_BLOCK_SIZE);
        }
        /* OpenSSL compat, no error */
    } else if(ctx->type >= 0) { /* MD5 == 0 */
        WOLFSSL_MSG("recover hmac");
        if (wc_HmacInit(&ctx->hmac, NULL, INVALID_DEVID) == 0) {
            ctx->hmac.macType = (byte)ctx->type;
            ctx->hmac.innerHashKeyed = 0;
            XMEMCPY((byte *)&ctx->hmac.ipad, (byte *)&ctx->save_ipad,
                                       WC_HMAC_BLOCK_SIZE);
            XMEMCPY((byte *)&ctx->hmac.opad, (byte *)&ctx->save_opad,
                                       WC_HMAC_BLOCK_SIZE);
            if ((hmac_error = _InitHmac(&ctx->hmac, ctx->hmac.macType, heap))
                    !=0) {
               return hmac_error;
            }
        }
    }

    (void)hmac_error;

    return WOLFSSL_SUCCESS;
}


int wolfSSL_HMAC_Update(WOLFSSL_HMAC_CTX* ctx, const unsigned char* data,
                    int len)
{
    int hmac_error = 0;

    WOLFSSL_MSG("wolfSSL_HMAC_Update");

    if (ctx == NULL) {
        WOLFSSL_MSG("no ctx");
        return WOLFSSL_FAILURE;
    }

    if (data) {
        WOLFSSL_MSG("updating hmac");
        hmac_error = wc_HmacUpdate(&ctx->hmac, data, (word32)len);
        if (hmac_error < 0){
            WOLFSSL_MSG("hmac update error");
            return WOLFSSL_FAILURE;
        }
    }

    return WOLFSSL_SUCCESS;
}


int wolfSSL_HMAC_Final(WOLFSSL_HMAC_CTX* ctx, unsigned char* hash,
                   unsigned int* len)
{
    int hmac_error;

    WOLFSSL_MSG("wolfSSL_HMAC_Final");

	/* "len" parameter is optional. */
    if (ctx == NULL || hash == NULL) {
        WOLFSSL_MSG("invalid parameter");
        return WOLFSSL_FAILURE;
    }

    WOLFSSL_MSG("final hmac");
    hmac_error = wc_HmacFinal(&ctx->hmac, hash);
    if (hmac_error < 0){
        WOLFSSL_MSG("final hmac error");
        return WOLFSSL_FAILURE;
    }

    if (len) {
        WOLFSSL_MSG("setting output len");
        switch (ctx->type) {
            #ifndef NO_MD5
            case WC_MD5:
                *len = WC_MD5_DIGEST_SIZE;
                break;
            #endif

            #ifndef NO_SHA
            case WC_SHA:
                *len = WC_SHA_DIGEST_SIZE;
                break;
            #endif

            #ifdef WOLFSSL_SHA224
            case WC_SHA224:
                *len = WC_SHA224_DIGEST_SIZE;
                break;
            #endif

            #ifndef NO_SHA256
            case WC_SHA256:
                *len = WC_SHA256_DIGEST_SIZE;
                break;
            #endif

            #ifdef WOLFSSL_SHA384
            case WC_SHA384:
                *len = WC_SHA384_DIGEST_SIZE;
                break;
            #endif

            #ifdef WOLFSSL_SHA512
            case WC_SHA512:
                *len = WC_SHA512_DIGEST_SIZE;
                break;
            #endif

            default:
                WOLFSSL_MSG("bad hmac type");
                return WOLFSSL_FAILURE;
        }
    }

    return WOLFSSL_SUCCESS;
}


int wolfSSL_HMAC_cleanup(WOLFSSL_HMAC_CTX* ctx)
{
    WOLFSSL_MSG("wolfSSL_HMAC_cleanup");

    if (ctx)
        wc_HmacFree(&ctx->hmac);

    return SSL_SUCCESS;
}


const WOLFSSL_EVP_MD* wolfSSL_EVP_get_digestbynid(int id)
{
    WOLFSSL_MSG("wolfSSL_get_digestbynid");

    switch(id) {
#ifndef NO_MD5
        case NID_md5:
            return wolfSSL_EVP_md5();
#endif
#ifndef NO_SHA
        case NID_sha1:
            return wolfSSL_EVP_sha1();
#endif
        default:
            WOLFSSL_MSG("Bad digest id value");
    }

    return NULL;
}


#ifndef NO_RSA
WOLFSSL_RSA* wolfSSL_EVP_PKEY_get1_RSA(WOLFSSL_EVP_PKEY* key)
{
    WOLFSSL_RSA* local;

    WOLFSSL_MSG("wolfSSL_EVP_PKEY_get1_RSA");

    if (key == NULL) {
        return NULL;
    }

    local = wolfSSL_RSA_new();
    if (local == NULL) {
        WOLFSSL_MSG("Error creating a new WOLFSSL_RSA structure");
        return NULL;
    }

    if (key->type == EVP_PKEY_RSA) {
        if (wolfSSL_RSA_LoadDer(local, (const unsigned char*)key->pkey.ptr,
                    key->pkey_sz) != SSL_SUCCESS) {
            /* now try public key */
            if (wolfSSL_RSA_LoadDer_ex(local,
                        (const unsigned char*)key->pkey.ptr, key->pkey_sz,
                        WOLFSSL_RSA_LOAD_PUBLIC) != SSL_SUCCESS) {
                wolfSSL_RSA_free(local);
                local = NULL;
            }
        }
    }
    else {
        WOLFSSL_MSG("WOLFSSL_EVP_PKEY does not hold an RSA key");
        wolfSSL_RSA_free(local);
        local = NULL;
    }
    return local;
}


/* with set1 functions the pkey struct does not own the RSA structure
 *
 * returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on failure
 */
int wolfSSL_EVP_PKEY_set1_RSA(WOLFSSL_EVP_PKEY *pkey, WOLFSSL_RSA *key)
{
    if((pkey == NULL) || (key ==NULL))return WOLFSSL_FAILURE;
    WOLFSSL_ENTER("wolfSSL_EVP_PKEY_set1_RSA");
    if (pkey->rsa != NULL && pkey->ownRsa == 1) {
        wolfSSL_RSA_free(pkey->rsa);
    }
    pkey->rsa    = key;
    pkey->ownRsa = 0; /* pkey does not own RSA */
    pkey->type = EVP_PKEY_RSA;
#ifdef WC_RSA_BLINDING
    if (key->ownRng == 0) {
        if (wc_RsaSetRNG((RsaKey*)(pkey->rsa->internal), &(pkey->rng)) != 0) {
            WOLFSSL_MSG("Error setting RSA rng");
            return SSL_FAILURE;
        }
    }
#endif
    return WOLFSSL_SUCCESS;
}
#endif /* NO_RSA */

#ifndef NO_WOLFSSL_STUB
WOLFSSL_DSA* wolfSSL_EVP_PKEY_get1_DSA(WOLFSSL_EVP_PKEY* key)
{
    (void)key;
    WOLFSSL_MSG("wolfSSL_EVP_PKEY_get1_DSA not implemented");
    WOLFSSL_STUB("EVP_PKEY_get1_DSA");
    return NULL;
}
#endif

#ifndef NO_WOLFSSL_STUB
WOLFSSL_EC_KEY* wolfSSL_EVP_PKEY_get1_EC_KEY(WOLFSSL_EVP_PKEY* key)
{
    (void)key;
    WOLFSSL_STUB("EVP_PKEY_get1_EC_KEY");
    WOLFSSL_MSG("wolfSSL_EVP_PKEY_get1_EC_KEY not implemented");

    return NULL;
}
#endif

void* wolfSSL_EVP_X_STATE(const WOLFSSL_EVP_CIPHER_CTX* ctx)
{
    WOLFSSL_MSG("wolfSSL_EVP_X_STATE");

    if (ctx) {
        switch (ctx->cipherType) {
            case ARC4_TYPE:
                WOLFSSL_MSG("returning arc4 state");
                return (void*)&ctx->cipher.arc4.x;

            default:
                WOLFSSL_MSG("bad x state type");
                return 0;
        }
    }

    return NULL;
}


int wolfSSL_EVP_X_STATE_LEN(const WOLFSSL_EVP_CIPHER_CTX* ctx)
{
    WOLFSSL_MSG("wolfSSL_EVP_X_STATE_LEN");

    if (ctx) {
        switch (ctx->cipherType) {
            case ARC4_TYPE:
                WOLFSSL_MSG("returning arc4 state size");
                return sizeof(Arc4);

            default:
                WOLFSSL_MSG("bad x state type");
                return 0;
        }
    }

    return 0;
}


#ifndef NO_DES3

void wolfSSL_3des_iv(WOLFSSL_EVP_CIPHER_CTX* ctx, int doset,
                            unsigned char* iv, int len)
{
    (void)len;

    WOLFSSL_MSG("wolfSSL_3des_iv");

    if (ctx == NULL || iv == NULL) {
        WOLFSSL_MSG("Bad function argument");
        return;
    }

    if (doset)
        wc_Des3_SetIV(&ctx->cipher.des3, iv);  /* OpenSSL compat, no ret */
    else
        XMEMCPY(iv, &ctx->cipher.des3.reg, DES_BLOCK_SIZE);
}

#endif /* NO_DES3 */


#ifndef NO_AES

void wolfSSL_aes_ctr_iv(WOLFSSL_EVP_CIPHER_CTX* ctx, int doset,
                      unsigned char* iv, int len)
{
    (void)len;

    WOLFSSL_MSG("wolfSSL_aes_ctr_iv");

    if (ctx == NULL || iv == NULL) {
        WOLFSSL_MSG("Bad function argument");
        return;
    }

    if (doset)
       (void)wc_AesSetIV(&ctx->cipher.aes, iv);  /* OpenSSL compat, no ret */
    else
        XMEMCPY(iv, &ctx->cipher.aes.reg, AES_BLOCK_SIZE);
}

#endif /* NO_AES */

#ifndef NO_WOLFSSL_STUB
const WOLFSSL_EVP_MD* wolfSSL_EVP_ripemd160(void)
{
    WOLFSSL_MSG("wolfSSL_ripemd160");
    WOLFSSL_STUB("EVP_ripemd160");
    return NULL;
}
#endif

int wolfSSL_EVP_MD_size(const WOLFSSL_EVP_MD* type)
{
    WOLFSSL_MSG("wolfSSL_EVP_MD_size");

    if (type == NULL) {
        WOLFSSL_MSG("No md type arg");
        return BAD_FUNC_ARG;
    }

    if (XSTRNCMP(type, "SHA256", 6) == 0) {
        return WC_SHA256_DIGEST_SIZE;
    }
#ifndef NO_MD5
    else if (XSTRNCMP(type, "MD5", 3) == 0) {
        return WC_MD5_DIGEST_SIZE;
    }
#endif
#ifdef WOLFSSL_SHA224
    else if (XSTRNCMP(type, "SHA224", 6) == 0) {
        return WC_SHA224_DIGEST_SIZE;
    }
#endif
#ifdef WOLFSSL_SHA384
    else if (XSTRNCMP(type, "SHA384", 6) == 0) {
        return WC_SHA384_DIGEST_SIZE;
    }
#endif
#ifdef WOLFSSL_SHA512
    else if (XSTRNCMP(type, "SHA512", 6) == 0) {
        return WC_SHA512_DIGEST_SIZE;
    }
#endif
#ifndef NO_SHA
    /* has to be last since would pick or 256, 384, or 512 too */
    else if (XSTRNCMP(type, "SHA", 3) == 0) {
        return WC_SHA_DIGEST_SIZE;
    }
#endif

    return BAD_FUNC_ARG;
}


int wolfSSL_EVP_CIPHER_CTX_iv_length(const WOLFSSL_EVP_CIPHER_CTX* ctx)
{
    WOLFSSL_MSG("wolfSSL_EVP_CIPHER_CTX_iv_length");

    switch (ctx->cipherType) {

#ifdef HAVE_AES_CBC
        case AES_128_CBC_TYPE :
        case AES_192_CBC_TYPE :
        case AES_256_CBC_TYPE :
            WOLFSSL_MSG("AES CBC");
            return AES_BLOCK_SIZE;
#endif
#ifdef WOLFSSL_AES_COUNTER
        case AES_128_CTR_TYPE :
        case AES_192_CTR_TYPE :
        case AES_256_CTR_TYPE :
            WOLFSSL_MSG("AES CTR");
            return AES_BLOCK_SIZE;
#endif
#ifndef NO_DES3
        case DES_CBC_TYPE :
            WOLFSSL_MSG("DES CBC");
            return DES_BLOCK_SIZE;

        case DES_EDE3_CBC_TYPE :
            WOLFSSL_MSG("DES EDE3 CBC");
            return DES_BLOCK_SIZE;
#endif
#ifdef HAVE_IDEA
        case IDEA_CBC_TYPE :
            WOLFSSL_MSG("IDEA CBC");
            return IDEA_BLOCK_SIZE;
#endif
#ifndef NO_RC4
        case ARC4_TYPE :
            WOLFSSL_MSG("ARC4");
            return 0;
#endif

        case NULL_CIPHER_TYPE :
            WOLFSSL_MSG("NULL");
            return 0;

        default: {
            WOLFSSL_MSG("bad type");
        }
    }
    return 0;
}

int wolfSSL_EVP_CIPHER_iv_length(const WOLFSSL_EVP_CIPHER* cipher)
{
    const char *name = (const char *)cipher;
    WOLFSSL_MSG("wolfSSL_EVP_CIPHER_iv_length");

#ifndef NO_AES
    #ifdef WOLFSSL_AES_128
    if (XSTRNCMP(name, EVP_AES_128_CBC, XSTRLEN(EVP_AES_128_CBC)) == 0)
        return AES_BLOCK_SIZE;
    #endif
    #ifdef WOLFSSL_AES_192
    if (XSTRNCMP(name, EVP_AES_192_CBC, XSTRLEN(EVP_AES_192_CBC)) == 0)
        return AES_BLOCK_SIZE;
    #endif
    #ifdef WOLFSSL_AES_256
    if (XSTRNCMP(name, EVP_AES_256_CBC, XSTRLEN(EVP_AES_256_CBC)) == 0)
        return AES_BLOCK_SIZE;
    #endif
#ifdef WOLFSSL_AES_COUNTER
    #ifdef WOLFSSL_AES_128
    if (XSTRNCMP(name, EVP_AES_128_CTR, XSTRLEN(EVP_AES_128_CTR)) == 0)
        return AES_BLOCK_SIZE;
    #endif
    #ifdef WOLFSSL_AES_192
    if (XSTRNCMP(name, EVP_AES_192_CTR, XSTRLEN(EVP_AES_192_CTR)) == 0)
        return AES_BLOCK_SIZE;
    #endif
    #ifdef WOLFSSL_AES_256
    if (XSTRNCMP(name, EVP_AES_256_CTR, XSTRLEN(EVP_AES_256_CTR)) == 0)
        return AES_BLOCK_SIZE;
    #endif
#endif
#endif

#ifndef NO_DES3
    if ((XSTRNCMP(name, EVP_DES_CBC, XSTRLEN(EVP_DES_CBC)) == 0) ||
           (XSTRNCMP(name, EVP_DES_EDE3_CBC, XSTRLEN(EVP_DES_EDE3_CBC)) == 0)) {
        return DES_BLOCK_SIZE;
    }
#endif

#ifdef HAVE_IDEA
    if (XSTRNCMP(name, EVP_IDEA_CBC, XSTRLEN(EVP_IDEA_CBC)) == 0)
        return IDEA_BLOCK_SIZE;
#endif

    (void)name;

    return 0;
}

/* Free the dynamically allocated data.
 *
 * p  Pointer to dynamically allocated memory.
 */
void wolfSSL_OPENSSL_free(void* p)
{
    WOLFSSL_MSG("wolfSSL_OPENSSL_free");

    XFREE(p, NULL, DYNAMIC_TYPE_OPENSSL);
}

void *wolfSSL_OPENSSL_malloc(size_t a)
{
  return XMALLOC(a, NULL, DYNAMIC_TYPE_OPENSSL);
}

#if defined(WOLFSSL_KEY_GEN) && defined(WOLFSSL_PEM_TO_DER)

static int EncryptDerKey(byte *der, int *derSz, const EVP_CIPHER* cipher,
                         unsigned char* passwd, int passwdSz, byte **cipherInfo)
{
    int ret, paddingSz;
    word32 idx, cipherInfoSz;
#ifdef WOLFSSL_SMALL_STACK
    EncryptedInfo* info = NULL;
#else
    EncryptedInfo  info[1];
#endif

    WOLFSSL_ENTER("EncryptDerKey");

    if (der == NULL || derSz == NULL || cipher == NULL ||
        passwd == NULL || cipherInfo == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_SMALL_STACK
    info = (EncryptedInfo*)XMALLOC(sizeof(EncryptedInfo), NULL,
                                   DYNAMIC_TYPE_ENCRYPTEDINFO);
    if (info == NULL) {
        WOLFSSL_MSG("malloc failed");
        return WOLFSSL_FAILURE;
    }
#endif

    XMEMSET(info, 0, sizeof(EncryptedInfo));

    /* set the cipher name on info */
    XSTRNCPY(info->name, cipher, NAME_SZ-1);
    info->name[NAME_SZ-1] = '\0'; /* null term */

    ret = wc_EncryptedInfoGet(info, info->name);
    if (ret != 0) {
        WOLFSSL_MSG("unsupported cipher");
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(info, NULL, DYNAMIC_TYPE_ENCRYPTEDINFO);
    #endif
        return WOLFSSL_FAILURE;
    }

    /* Generate a random salt */
    if (wolfSSL_RAND_bytes(info->iv, info->ivSz) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("generate iv failed");
#ifdef WOLFSSL_SMALL_STACK
        XFREE(info, NULL, DYNAMIC_TYPE_ENCRYPTEDINFO);
#endif
        return WOLFSSL_FAILURE;
    }

    /* add the padding before encryption */
    paddingSz = ((*derSz)/info->ivSz + 1) * info->ivSz - (*derSz);
    if (paddingSz == 0)
        paddingSz = info->ivSz;
    XMEMSET(der+(*derSz), (byte)paddingSz, paddingSz);
    (*derSz) += paddingSz;

    /* encrypt buffer */
    if (wc_BufferKeyEncrypt(info, der, *derSz, passwd, passwdSz, WC_MD5) != 0) {
        WOLFSSL_MSG("encrypt key failed");
#ifdef WOLFSSL_SMALL_STACK
        XFREE(info, NULL, DYNAMIC_TYPE_ENCRYPTEDINFO);
#endif
        return WOLFSSL_FAILURE;
    }

    /* create cipher info : 'cipher_name,Salt(hex)' */
    cipherInfoSz = (word32)(2*info->ivSz + XSTRLEN(info->name) + 2);
    *cipherInfo = (byte*)XMALLOC(cipherInfoSz, NULL,
                                DYNAMIC_TYPE_STRING);
    if (*cipherInfo == NULL) {
        WOLFSSL_MSG("malloc failed");
#ifdef WOLFSSL_SMALL_STACK
        XFREE(info, NULL, DYNAMIC_TYPE_ENCRYPTEDINFO);
#endif
        return WOLFSSL_FAILURE;
    }
    XSTRNCPY((char*)*cipherInfo, info->name, cipherInfoSz);
    XSTRNCAT((char*)*cipherInfo, ",", 1);

    idx = (word32)XSTRLEN((char*)*cipherInfo);
    cipherInfoSz -= idx;
    ret = Base16_Encode(info->iv, info->ivSz, *cipherInfo+idx, &cipherInfoSz);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(info, NULL, DYNAMIC_TYPE_ENCRYPTEDINFO);
#endif
    if (ret != 0) {
        WOLFSSL_MSG("Base16_Encode failed");
        XFREE(*cipherInfo, NULL, DYNAMIC_TYPE_STRING);
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}
#endif /* WOLFSSL_KEY_GEN || WOLFSSL_PEM_TO_DER */

#if defined(WOLFSSL_KEY_GEN) || defined(WOLFSSL_CERT_GEN)
/* Takes a WOLFSSL_RSA key and writes it out to a WOLFSSL_BIO
 *
 * bio    the WOLFSSL_BIO to write to
 * key    the WOLFSSL_RSA key to write out
 * cipher cipher used
 * passwd password string if used
 * len    length of password string
 * cb     password callback to use
 * arg    null terminated string for passphrase
 */
int wolfSSL_PEM_write_bio_RSAPrivateKey(WOLFSSL_BIO* bio, WOLFSSL_RSA* key,
                                        const WOLFSSL_EVP_CIPHER* cipher,
                                        unsigned char* passwd, int len,
                                        pem_password_cb* cb, void* arg)
{
    int ret;
    WOLFSSL_EVP_PKEY* pkey;

    WOLFSSL_ENTER("wolfSSL_PEM_write_bio_RSAPrivateKey");


    pkey = wolfSSL_PKEY_new_ex(bio->heap);
    pkey->type   = EVP_PKEY_RSA;
    pkey->rsa    = key;
    pkey->ownRsa = 0;
#ifdef WOLFSSL_KEY_GEN
    /* similar to how wolfSSL_PEM_write_mem_RSAPrivateKey finds DER of key */
    {
        int derMax;
        int derSz;
        byte* derBuf;

        /* 5 > size of n, d, p, q, d%(p-1), d(q-1), 1/q%p, e + ASN.1 additional
         *  informations
         */
        derMax = 5 * wolfSSL_RSA_size(key) + AES_BLOCK_SIZE;

        derBuf = (byte*)XMALLOC(derMax, bio->heap, DYNAMIC_TYPE_TMP_BUFFER);
        if (derBuf == NULL) {
            WOLFSSL_MSG("malloc failed");
            return SSL_FAILURE;
        }

        /* Key to DER */
        derSz = wc_RsaKeyToDer((RsaKey*)key->internal, derBuf, derMax);
        if (derSz < 0) {
            WOLFSSL_MSG("wc_RsaKeyToDer failed");
            XFREE(derBuf, bio->heap, DYNAMIC_TYPE_TMP_BUFFER);
            return SSL_FAILURE;
        }

        pkey->pkey.ptr = (char*)XMALLOC(derSz, bio->heap,
                DYNAMIC_TYPE_TMP_BUFFER);
        if (pkey->pkey.ptr == NULL) {
            WOLFSSL_MSG("key malloc failed");
            XFREE(derBuf, bio->heap, DYNAMIC_TYPE_TMP_BUFFER);
            return SSL_FAILURE;
        }
        pkey->pkey_sz = derSz;
        XMEMCPY(pkey->pkey.ptr, derBuf, derSz);
        XFREE(derBuf, bio->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
#endif

    ret = wolfSSL_PEM_write_bio_PrivateKey(bio, pkey, cipher, passwd, len,
                                        cb, arg);

    wolfSSL_EVP_PKEY_free(pkey);

    return ret;
}


int wolfSSL_PEM_write_bio_PrivateKey(WOLFSSL_BIO* bio, WOLFSSL_EVP_PKEY* key,
                                        const WOLFSSL_EVP_CIPHER* cipher,
                                        unsigned char* passwd, int len,
                                        pem_password_cb* cb, void* arg)
{
    byte* keyDer;
    int pemSz;
    int type;
    int ret;
    byte* tmp;

    (void)cipher;
    (void)passwd;
    (void)len;
    (void)cb;
    (void)arg;

    WOLFSSL_ENTER("wolfSSL_PEM_write_bio_PrivateKey");

    if (bio == NULL || key == NULL) {
        return WOLFSSL_FAILURE;
    }

    keyDer = (byte*)key->pkey.ptr;

    switch (key->type) {
        case EVP_PKEY_RSA:
            type = PRIVATEKEY_TYPE;
            break;

#ifndef NO_DSA
        case EVP_PKEY_DSA:
            type = DSA_PRIVATEKEY_TYPE;
            break;
#endif

        case EVP_PKEY_EC:
            type = ECC_PRIVATEKEY_TYPE;
            break;

        default:
            WOLFSSL_MSG("Unknown Key type!");
            type = PRIVATEKEY_TYPE;
    }

    pemSz = wc_DerToPem(keyDer, key->pkey_sz, NULL, 0, type);
    if (pemSz < 0) {
        WOLFSSL_LEAVE("wolfSSL_PEM_write_bio_PrivateKey", pemSz);
        return WOLFSSL_FAILURE;
    }
    tmp = (byte*)XMALLOC(pemSz, bio->heap, DYNAMIC_TYPE_OPENSSL);
    if (tmp == NULL) {
        return MEMORY_E;
    }

    ret = wc_DerToPemEx(keyDer, key->pkey_sz, tmp, pemSz,
                                NULL, type);
    if (ret < 0) {
        WOLFSSL_LEAVE("wolfSSL_PEM_write_bio_PrivateKey", ret);
        XFREE(tmp, bio->heap, DYNAMIC_TYPE_OPENSSL);
        return SSL_FAILURE;
    }

    ret = wolfSSL_BIO_write(bio, tmp, pemSz);
    XFREE(tmp, bio->heap, DYNAMIC_TYPE_OPENSSL);
    if (ret != pemSz) {
        WOLFSSL_MSG("Unable to write full PEM to BIO");
        return SSL_FAILURE;
    }

    return SSL_SUCCESS;
}
#endif /* defined(WOLFSSL_KEY_GEN) || defined(WOLFSSL_CERT_GEN) */

#if defined(WOLFSSL_KEY_GEN) && !defined(NO_RSA) && !defined(HAVE_USER_RSA) && \
    (defined(WOLFSSL_PEM_TO_DER) || defined(WOLFSSL_DER_TO_PEM))

/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
int wolfSSL_PEM_write_mem_RSAPrivateKey(RSA* rsa, const EVP_CIPHER* cipher,
                                        unsigned char* passwd, int passwdSz,
                                        unsigned char **pem, int *plen)
{
    byte *derBuf, *tmp, *cipherInfo = NULL;
    int  der_max_len = 0, derSz = 0;
    const int type = PRIVATEKEY_TYPE;
    const char* header = NULL;
    const char* footer = NULL;

    WOLFSSL_ENTER("wolfSSL_PEM_write_mem_RSAPrivateKey");

    if (pem == NULL || plen == NULL || rsa == NULL || rsa->internal == NULL) {
        WOLFSSL_MSG("Bad function arguments");
        return WOLFSSL_FAILURE;
    }

    if (wc_PemGetHeaderFooter(type, &header, &footer) != 0)
        return WOLFSSL_FAILURE;

    if (rsa->inSet == 0) {
        WOLFSSL_MSG("No RSA internal set, do it");

        if (SetRsaInternal(rsa) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetRsaInternal failed");
            return WOLFSSL_FAILURE;
        }
    }

    /* 5 > size of n, d, p, q, d%(p-1), d(q-1), 1/q%p, e + ASN.1 additional
     *  informations
     */
    der_max_len = 5 * wolfSSL_RSA_size(rsa) + AES_BLOCK_SIZE;

    derBuf = (byte*)XMALLOC(der_max_len, NULL, DYNAMIC_TYPE_DER);
    if (derBuf == NULL) {
        WOLFSSL_MSG("malloc failed");
        return WOLFSSL_FAILURE;
    }

    /* Key to DER */
    derSz = wc_RsaKeyToDer((RsaKey*)rsa->internal, derBuf, der_max_len);
    if (derSz < 0) {
        WOLFSSL_MSG("wc_RsaKeyToDer failed");
        XFREE(derBuf, NULL, DYNAMIC_TYPE_DER);
        return WOLFSSL_FAILURE;
    }

    /* encrypt DER buffer if required */
    if (passwd != NULL && passwdSz > 0 && cipher != NULL) {
        int ret;

        ret = EncryptDerKey(derBuf, &derSz, cipher,
                            passwd, passwdSz, &cipherInfo);
        if (ret != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("EncryptDerKey failed");
            XFREE(derBuf, NULL, DYNAMIC_TYPE_DER);
            return ret;
        }

        /* tmp buffer with a max size */
        *plen = (derSz * 2) + (int)XSTRLEN(header) + 1 +
            (int)XSTRLEN(footer) + 1 + HEADER_ENCRYPTED_KEY_SIZE;
    }
    else {
        /* tmp buffer with a max size */
        *plen = (derSz * 2) + (int)XSTRLEN(header) + 1 +
            (int)XSTRLEN(footer) + 1;
    }

    tmp = (byte*)XMALLOC(*plen, NULL, DYNAMIC_TYPE_PEM);
    if (tmp == NULL) {
        WOLFSSL_MSG("malloc failed");
        XFREE(derBuf, NULL, DYNAMIC_TYPE_DER);
        if (cipherInfo != NULL)
            XFREE(cipherInfo, NULL, DYNAMIC_TYPE_STRING);
        return WOLFSSL_FAILURE;
    }

    /* DER to PEM */
    *plen = wc_DerToPemEx(derBuf, derSz, tmp, *plen, cipherInfo, type);
    if (*plen <= 0) {
        WOLFSSL_MSG("wc_DerToPemEx failed");
        XFREE(derBuf, NULL, DYNAMIC_TYPE_DER);
        XFREE(tmp, NULL, DYNAMIC_TYPE_PEM);
        if (cipherInfo != NULL)
            XFREE(cipherInfo, NULL, DYNAMIC_TYPE_STRING);
        return WOLFSSL_FAILURE;
    }
    XFREE(derBuf, NULL, DYNAMIC_TYPE_DER);
    if (cipherInfo != NULL)
        XFREE(cipherInfo, NULL, DYNAMIC_TYPE_STRING);

    *pem = (byte*)XMALLOC((*plen)+1, NULL, DYNAMIC_TYPE_KEY);
    if (*pem == NULL) {
        WOLFSSL_MSG("malloc failed");
        XFREE(tmp, NULL, DYNAMIC_TYPE_PEM);
        return WOLFSSL_FAILURE;
    }
    XMEMSET(*pem, 0, (*plen)+1);

    if (XMEMCPY(*pem, tmp, *plen) == NULL) {
        WOLFSSL_MSG("XMEMCPY failed");
        XFREE(pem, NULL, DYNAMIC_TYPE_KEY);
        XFREE(tmp, NULL, DYNAMIC_TYPE_PEM);
        return WOLFSSL_FAILURE;
    }
    XFREE(tmp, NULL, DYNAMIC_TYPE_PEM);

    return WOLFSSL_SUCCESS;
}


#ifndef NO_FILESYSTEM
/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
int wolfSSL_PEM_write_RSAPrivateKey(FILE *fp, WOLFSSL_RSA *rsa,
                                    const EVP_CIPHER *enc,
                                    unsigned char *kstr, int klen,
                                    pem_password_cb *cb, void *u)
{
    byte *pem;
    int  plen, ret;

    (void)cb;
    (void)u;

    WOLFSSL_MSG("wolfSSL_PEM_write_RSAPrivateKey");

    if (fp == NULL || rsa == NULL || rsa->internal == NULL) {
        WOLFSSL_MSG("Bad function arguments");
        return WOLFSSL_FAILURE;
    }

    ret = wolfSSL_PEM_write_mem_RSAPrivateKey(rsa, enc, kstr, klen, &pem, &plen);
    if (ret != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("wolfSSL_PEM_write_mem_RSAPrivateKey failed");
        return WOLFSSL_FAILURE;
    }

    ret = (int)XFWRITE(pem, plen, 1, fp);
    if (ret != 1) {
        WOLFSSL_MSG("RSA private key file write failed");
        return WOLFSSL_FAILURE;
    }

    XFREE(pem, NULL, DYNAMIC_TYPE_KEY);
    return WOLFSSL_SUCCESS;
}
#endif /* NO_FILESYSTEM */
#endif /* WOLFSSL_KEY_GEN && !NO_RSA && !HAVE_USER_RSA && WOLFSSL_PEM_TO_DER */


#ifdef HAVE_ECC

/* EC_POINT Openssl -> WolfSSL */
static int SetECPointInternal(WOLFSSL_EC_POINT *p)
{
    ecc_point* point;
    WOLFSSL_ENTER("SetECPointInternal");

    if (p == NULL || p->internal == NULL) {
        WOLFSSL_MSG("ECPoint NULL error");
        return WOLFSSL_FATAL_ERROR;
    }

    point = (ecc_point*)p->internal;

    if (p->X != NULL && SetIndividualInternal(p->X, point->x) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("ecc point X error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (p->Y != NULL && SetIndividualInternal(p->Y, point->y) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("ecc point Y error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (p->Z != NULL && SetIndividualInternal(p->Z, point->z) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("ecc point Z error");
        return WOLFSSL_FATAL_ERROR;
    }

    p->inSet = 1;

    return WOLFSSL_SUCCESS;
}
#endif /* HAVE_ECC */
#endif /* OPENSSL_EXTRA */

#if defined(HAVE_ECC) && defined(OPENSSL_EXTRA_X509_SMALL)

/* EC_POINT WolfSSL -> OpenSSL */
static int SetECPointExternal(WOLFSSL_EC_POINT *p)
{
    ecc_point* point;

    WOLFSSL_ENTER("SetECPointExternal");

    if (p == NULL || p->internal == NULL) {
        WOLFSSL_MSG("ECPoint NULL error");
        return WOLFSSL_FATAL_ERROR;
    }

    point = (ecc_point*)p->internal;

    if (SetIndividualExternal(&p->X, point->x) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("ecc point X error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (SetIndividualExternal(&p->Y, point->y) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("ecc point Y error");
        return WOLFSSL_FATAL_ERROR;
    }

    if (SetIndividualExternal(&p->Z, point->z) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("ecc point Z error");
        return WOLFSSL_FATAL_ERROR;
    }

    p->exSet = 1;

    return WOLFSSL_SUCCESS;
}


/* EC_KEY wolfSSL -> OpenSSL */
static int SetECKeyExternal(WOLFSSL_EC_KEY* eckey)
{
    ecc_key* key;

    WOLFSSL_ENTER("SetECKeyExternal");

    if (eckey == NULL || eckey->internal == NULL) {
        WOLFSSL_MSG("ec key NULL error");
        return WOLFSSL_FATAL_ERROR;
    }

    key = (ecc_key*)eckey->internal;

    /* set group (OID, nid and idx) */
    eckey->group->curve_oid = ecc_sets[key->idx].oidSum;
    eckey->group->curve_nid = ecc_sets[key->idx].id;
    eckey->group->curve_idx = key->idx;

    if (eckey->pub_key->internal != NULL) {
        /* set the internal public key */
        if (wc_ecc_copy_point(&key->pubkey,
                             (ecc_point*)eckey->pub_key->internal) != MP_OKAY) {
            WOLFSSL_MSG("SetECKeyExternal ecc_copy_point failed");
            return WOLFSSL_FATAL_ERROR;
        }

        /* set the external pubkey (point) */
        if (SetECPointExternal(eckey->pub_key) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetECKeyExternal SetECPointExternal failed");
            return WOLFSSL_FATAL_ERROR;
        }
    }

    /* set the external privkey */
    if (key->type == ECC_PRIVATEKEY) {
        if (SetIndividualExternal(&eckey->priv_key, &key->k) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("ec priv key error");
            return WOLFSSL_FATAL_ERROR;
        }
    }

    eckey->exSet = 1;

    return WOLFSSL_SUCCESS;
}
#endif /* HAVE_ECC && OPENSSL_EXTRA_X509_SMALL */

#ifdef OPENSSL_EXTRA
#ifdef HAVE_ECC
/* EC_KEY Openssl -> WolfSSL */
static int SetECKeyInternal(WOLFSSL_EC_KEY* eckey)
{
    ecc_key* key;

    WOLFSSL_ENTER("SetECKeyInternal");

    if (eckey == NULL || eckey->internal == NULL) {
        WOLFSSL_MSG("ec key NULL error");
        return WOLFSSL_FATAL_ERROR;
    }

    key = (ecc_key*)eckey->internal;

    /* validate group */
    if ((eckey->group->curve_idx < 0) ||
        (wc_ecc_is_valid_idx(eckey->group->curve_idx) == 0)) {
        WOLFSSL_MSG("invalid curve idx");
        return WOLFSSL_FATAL_ERROR;
    }

    /* set group (idx of curve and corresponding domain parameters) */
    key->idx = eckey->group->curve_idx;
    key->dp = &ecc_sets[key->idx];

    /* set pubkey (point) */
    if (eckey->pub_key != NULL) {
        if (SetECPointInternal(eckey->pub_key) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("ec key pub error");
            return WOLFSSL_FATAL_ERROR;
        }

        /* public key */
        key->type = ECC_PUBLICKEY;
    }

    /* set privkey */
    if (eckey->priv_key != NULL) {
        if (SetIndividualInternal(eckey->priv_key, &key->k) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("ec key priv error");
            return WOLFSSL_FATAL_ERROR;
        }

        /* private key */
        key->type = ECC_PRIVATEKEY;
    }

    eckey->inSet = 1;

    return WOLFSSL_SUCCESS;
}

WOLFSSL_EC_POINT *wolfSSL_EC_KEY_get0_public_key(const WOLFSSL_EC_KEY *key)
{
    WOLFSSL_ENTER("wolfSSL_EC_KEY_get0_public_key");

    if (key == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_KEY_get0_group Bad arguments");
        return NULL;
    }

    return key->pub_key;
}

const WOLFSSL_EC_GROUP *wolfSSL_EC_KEY_get0_group(const WOLFSSL_EC_KEY *key)
{
    WOLFSSL_ENTER("wolfSSL_EC_KEY_get0_group");

    if (key == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_KEY_get0_group Bad arguments");
        return NULL;
    }

    return key->group;
}


/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
int wolfSSL_EC_KEY_set_private_key(WOLFSSL_EC_KEY *key,
                                   const WOLFSSL_BIGNUM *priv_key)
{
    WOLFSSL_ENTER("wolfSSL_EC_KEY_set_private_key");

    if (key == NULL || priv_key == NULL) {
        WOLFSSL_MSG("Bad arguments");
        return WOLFSSL_FAILURE;
    }

    /* free key if previously set */
    if (key->priv_key != NULL)
        wolfSSL_BN_free(key->priv_key);

    key->priv_key = wolfSSL_BN_dup(priv_key);
    if (key->priv_key == NULL) {
        WOLFSSL_MSG("key ecc priv key NULL");
        return WOLFSSL_FAILURE;
    }

    if (SetECKeyInternal(key) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("SetECKeyInternal failed");
        wolfSSL_BN_free(key->priv_key);
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}


WOLFSSL_BIGNUM *wolfSSL_EC_KEY_get0_private_key(const WOLFSSL_EC_KEY *key)
{
    WOLFSSL_ENTER("wolfSSL_EC_KEY_get0_private_key");

    if (key == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_KEY_get0_private_key Bad arguments");
        return NULL;
    }

    return key->priv_key;
}

WOLFSSL_EC_KEY *wolfSSL_EC_KEY_new_by_curve_name(int nid)
{
    WOLFSSL_EC_KEY *key;
    int x;

    WOLFSSL_ENTER("wolfSSL_EC_KEY_new_by_curve_name");

    key = wolfSSL_EC_KEY_new();
    if (key == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_KEY_new failure");
        return NULL;
    }

    /* set the nid of the curve */
    key->group->curve_nid = nid;

    /* search and set the corresponding internal curve idx */
    for (x = 0; ecc_sets[x].size != 0; x++)
        if (ecc_sets[x].id == key->group->curve_nid) {
            key->group->curve_idx = x;
            key->group->curve_oid = ecc_sets[x].oidSum;
            break;
        }

    return key;
}

#endif /* HAVE_ECC */
#endif /* OPENSSL_EXTRA */

#if defined(HAVE_ECC) && (defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL))
static void InitwolfSSL_ECKey(WOLFSSL_EC_KEY* key)
{
    if (key) {
        key->group    = NULL;
        key->pub_key  = NULL;
        key->priv_key = NULL;
        key->internal = NULL;
        key->inSet    = 0;
        key->exSet    = 0;
    }
}

WOLFSSL_EC_KEY *wolfSSL_EC_KEY_new(void)
{
    WOLFSSL_EC_KEY *external;
    ecc_key* key;

    WOLFSSL_ENTER("wolfSSL_EC_KEY_new");

    external = (WOLFSSL_EC_KEY*)XMALLOC(sizeof(WOLFSSL_EC_KEY), NULL,
                                        DYNAMIC_TYPE_ECC);
    if (external == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_KEY_new malloc WOLFSSL_EC_KEY failure");
        return NULL;
    }
    XMEMSET(external, 0, sizeof(WOLFSSL_EC_KEY));

    InitwolfSSL_ECKey(external);

    external->internal = (ecc_key*)XMALLOC(sizeof(ecc_key), NULL,
                                           DYNAMIC_TYPE_ECC);
    if (external->internal == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_KEY_new malloc ecc key failure");
        wolfSSL_EC_KEY_free(external);
        return NULL;
    }
    XMEMSET(external->internal, 0, sizeof(ecc_key));

    wc_ecc_init((ecc_key*)external->internal);

    /* public key */
    external->pub_key = (WOLFSSL_EC_POINT*)XMALLOC(sizeof(WOLFSSL_EC_POINT),
                                                   NULL, DYNAMIC_TYPE_ECC);
    if (external->pub_key == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_KEY_new malloc WOLFSSL_EC_POINT failure");
        wolfSSL_EC_KEY_free(external);
        return NULL;
    }
    XMEMSET(external->pub_key, 0, sizeof(WOLFSSL_EC_POINT));

    key = (ecc_key*)external->internal;
    external->pub_key->internal = wc_ecc_new_point();
    if (wc_ecc_copy_point((ecc_point*)&key->pubkey,
                (ecc_point*)external->pub_key->internal) != MP_OKAY) {
        WOLFSSL_MSG("wc_ecc_copy_point failure");
        wolfSSL_EC_KEY_free(external);
        return NULL;
    }

    /* curve group */
    external->group = (WOLFSSL_EC_GROUP*)XMALLOC(sizeof(WOLFSSL_EC_GROUP), NULL,
                                                 DYNAMIC_TYPE_ECC);
    if (external->group == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_KEY_new malloc WOLFSSL_EC_GROUP failure");
        wolfSSL_EC_KEY_free(external);
        return NULL;
    }
    XMEMSET(external->group, 0, sizeof(WOLFSSL_EC_GROUP));

    /* private key */
    external->priv_key = wolfSSL_BN_new();
    if (external->priv_key == NULL) {
        WOLFSSL_MSG("wolfSSL_BN_new failure");
        wolfSSL_EC_KEY_free(external);
        return NULL;
    }

    return external;
}

void wolfSSL_EC_KEY_free(WOLFSSL_EC_KEY *key)
{
    WOLFSSL_ENTER("wolfSSL_EC_KEY_free");

    if (key != NULL) {
        if (key->internal != NULL) {
            wc_ecc_free((ecc_key*)key->internal);
            XFREE(key->internal, NULL, DYNAMIC_TYPE_ECC);
        }
        wolfSSL_BN_free(key->priv_key);
        wolfSSL_EC_POINT_free(key->pub_key);
        wolfSSL_EC_GROUP_free(key->group);
        InitwolfSSL_ECKey(key); /* set back to NULLs for safety */

        XFREE(key, NULL, DYNAMIC_TYPE_ECC);
        key = NULL;
    }
}
#endif /* HAVE_ECC && (OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL) */

#ifdef OPENSSL_EXTRA
#ifdef HAVE_ECC

#ifndef NO_WOLFSSL_STUB
int wolfSSL_EC_KEY_set_group(WOLFSSL_EC_KEY *key, WOLFSSL_EC_GROUP *group)
{
    (void)key;
    (void)group;

    WOLFSSL_ENTER("wolfSSL_EC_KEY_set_group");
    WOLFSSL_STUB("EC_KEY_set_group");

    return -1;
}
#endif

int wolfSSL_EC_KEY_generate_key(WOLFSSL_EC_KEY *key)
{
    int     initTmpRng = 0;
    WC_RNG* rng = NULL;
#ifdef WOLFSSL_SMALL_STACK
    WC_RNG* tmpRNG = NULL;
#else
    WC_RNG  tmpRNG[1];
#endif

    WOLFSSL_ENTER("wolfSSL_EC_KEY_generate_key");

    if (key == NULL || key->internal == NULL ||
        key->group == NULL || key->group->curve_idx < 0) {
        WOLFSSL_MSG("wolfSSL_EC_KEY_generate_key Bad arguments");
        return 0;
    }

#ifdef WOLFSSL_SMALL_STACK
    tmpRNG = (WC_RNG*)XMALLOC(sizeof(WC_RNG), NULL, DYNAMIC_TYPE_RNG);
    if (tmpRNG == NULL)
        return 0;
#endif

    if (wc_InitRng(tmpRNG) == 0) {
        rng = tmpRNG;
        initTmpRng = 1;
    }
    else {
        WOLFSSL_MSG("Bad RNG Init, trying global");
        if (initGlobalRNG == 0)
            WOLFSSL_MSG("Global RNG no Init");
        else
            rng = &globalRNG;
    }

    if (rng == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_KEY_generate_key failed to set RNG");
#ifdef WOLFSSL_SMALL_STACK
        XFREE(tmpRNG, NULL, DYNAMIC_TYPE_RNG);
#endif
        return 0;
    }

    if (wc_ecc_make_key_ex(rng, 0, (ecc_key*)key->internal,
                                        key->group->curve_nid) != MP_OKAY) {
        WOLFSSL_MSG("wolfSSL_EC_KEY_generate_key wc_ecc_make_key failed");
#ifdef WOLFSSL_SMALL_STACK
        XFREE(tmpRNG, NULL, DYNAMIC_TYPE_RNG);
#endif
        return 0;
    }

    if (initTmpRng)
        wc_FreeRng(tmpRNG);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(tmpRNG, NULL, DYNAMIC_TYPE_RNG);
#endif

    if (SetECKeyExternal(key) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("wolfSSL_EC_KEY_generate_key SetECKeyExternal failed");
        return 0;
    }

    return 1;
}

#ifndef NO_WOLFSSL_STUB
void wolfSSL_EC_KEY_set_asn1_flag(WOLFSSL_EC_KEY *key, int asn1_flag)
{
    (void)key;
    (void)asn1_flag;

    WOLFSSL_ENTER("wolfSSL_EC_KEY_set_asn1_flag");
    WOLFSSL_STUB("EC_KEY_set_asn1_flag");
}
#endif

/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
int wolfSSL_EC_KEY_set_public_key(WOLFSSL_EC_KEY *key,
                                  const WOLFSSL_EC_POINT *pub)
{
    ecc_point *pub_p, *key_p;

    WOLFSSL_ENTER("wolfSSL_EC_KEY_set_public_key");

    if (key == NULL || key->internal == NULL ||
        pub == NULL || pub->internal == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_GROUP_get_order Bad arguments");
        return WOLFSSL_FAILURE;
    }

    if (key->inSet == 0) {
        if (SetECKeyInternal(key) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetECKeyInternal failed");
            return WOLFSSL_FAILURE;
        }
    }

    if (pub->inSet == 0) {
        if (SetECPointInternal((WOLFSSL_EC_POINT *)pub) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetECPointInternal failed");
            return WOLFSSL_FAILURE;
        }
    }

    pub_p = (ecc_point*)pub->internal;
    key_p = (ecc_point*)key->pub_key->internal;

    /* create new point if required */
    if (key_p == NULL)
        key_p = wc_ecc_new_point();

    if (key_p == NULL) {
        WOLFSSL_MSG("key ecc point NULL");
        return WOLFSSL_FAILURE;
    }

    if (wc_ecc_copy_point(pub_p, key_p) != MP_OKAY) {
        WOLFSSL_MSG("ecc_copy_point failure");
        return WOLFSSL_FAILURE;
    }

    if (SetECKeyExternal(key) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("SetECKeyInternal failed");
        return WOLFSSL_FAILURE;
    }

    wolfSSL_EC_POINT_dump("pub", pub);
    wolfSSL_EC_POINT_dump("key->pub_key", key->pub_key);

    return WOLFSSL_SUCCESS;
}
/* End EC_KEY */

void wolfSSL_EC_POINT_dump(const char *msg, const WOLFSSL_EC_POINT *p)
{
#if defined(DEBUG_WOLFSSL)
    char *num;

    WOLFSSL_ENTER("wolfSSL_EC_POINT_dump");

    if (p == NULL) {
        printf("%s = NULL", msg);
        return;
    }

    printf("%s:\n\tinSet=%d, exSet=%d\n", msg, p->inSet, p->exSet);
    num = wolfSSL_BN_bn2hex(p->X);
    printf("\tX = %s\n", num);
    XFREE(num, NULL, DYNAMIC_TYPE_ECC);
    num = wolfSSL_BN_bn2hex(p->Y);
    printf("\tY = %s\n", num);
    XFREE(num, NULL, DYNAMIC_TYPE_ECC);
    num = wolfSSL_BN_bn2hex(p->Z);
    printf("\tZ = %s\n", num);
    XFREE(num, NULL, DYNAMIC_TYPE_ECC);
#else
    (void)msg;
    (void)p;
#endif
}

/* Start EC_GROUP */

/* return code compliant with OpenSSL :
 *   0 if equal, 1 if not and -1 in case of error
 */
int wolfSSL_EC_GROUP_cmp(const WOLFSSL_EC_GROUP *a, const WOLFSSL_EC_GROUP *b,
                         WOLFSSL_BN_CTX *ctx)
{
    (void)ctx;

    WOLFSSL_ENTER("wolfSSL_EC_GROUP_cmp");

    if (a == NULL || b == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_GROUP_cmp Bad arguments");
        return WOLFSSL_FATAL_ERROR;
    }

    /* ok */
    if ((a->curve_idx == b->curve_idx) && (a->curve_nid == b->curve_nid))
        return 0;

    /* ko */
    return 1;
}

#endif /* HAVE_ECC */
#endif /* OPENSSL_EXTRA */

#if defined(HAVE_ECC) && (defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL))
void wolfSSL_EC_GROUP_free(WOLFSSL_EC_GROUP *group)
{
    WOLFSSL_ENTER("wolfSSL_EC_GROUP_free");

    XFREE(group, NULL, DYNAMIC_TYPE_ECC);
    group = NULL;
}
#endif

#ifdef OPENSSL_EXTRA
#ifdef HAVE_ECC
#ifndef NO_WOLFSSL_STUB
void wolfSSL_EC_GROUP_set_asn1_flag(WOLFSSL_EC_GROUP *group, int flag)
{
    (void)group;
    (void)flag;

    WOLFSSL_ENTER("wolfSSL_EC_GROUP_set_asn1_flag");
    WOLFSSL_STUB("EC_GROUP_set_asn1_flag");
}
#endif

WOLFSSL_EC_GROUP *wolfSSL_EC_GROUP_new_by_curve_name(int nid)
{
    WOLFSSL_EC_GROUP *g;
    int x;

    WOLFSSL_ENTER("wolfSSL_EC_GROUP_new_by_curve_name");

    /* curve group */
    g = (WOLFSSL_EC_GROUP*) XMALLOC(sizeof(WOLFSSL_EC_GROUP), NULL,
                                    DYNAMIC_TYPE_ECC);
    if (g == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_GROUP_new_by_curve_name malloc failure");
        return NULL;
    }
    XMEMSET(g, 0, sizeof(WOLFSSL_EC_GROUP));

    /* set the nid of the curve */
    g->curve_nid = nid;

    /* search and set the corresponding internal curve idx */
    for (x = 0; ecc_sets[x].size != 0; x++)
        if (ecc_sets[x].id == g->curve_nid) {
            g->curve_idx = x;
            g->curve_oid = ecc_sets[x].oidSum;
            break;
        }

    return g;
}

/* return code compliant with OpenSSL :
 *   the curve nid if success, 0 if error
 */
int wolfSSL_EC_GROUP_get_curve_name(const WOLFSSL_EC_GROUP *group)
{
    WOLFSSL_ENTER("wolfSSL_EC_GROUP_get_curve_name");

    if (group == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_GROUP_get_curve_name Bad arguments");
        return WOLFSSL_FAILURE;
    }

    return group->curve_nid;
}

/* return code compliant with OpenSSL :
 *   the degree of the curve if success, 0 if error
 */
int wolfSSL_EC_GROUP_get_degree(const WOLFSSL_EC_GROUP *group)
{
    WOLFSSL_ENTER("wolfSSL_EC_GROUP_get_degree");

    if (group == NULL || group->curve_idx < 0) {
        WOLFSSL_MSG("wolfSSL_EC_GROUP_get_degree Bad arguments");
        return WOLFSSL_FAILURE;
    }

    switch(group->curve_nid) {
        case NID_secp112r1:
        case NID_secp112r2:
            return 112;
        case NID_secp128r1:
        case NID_secp128r2:
            return 128;
        case NID_secp160k1:
        case NID_secp160r1:
        case NID_secp160r2:
        case NID_brainpoolP160r1:
            return 160;
        case NID_secp192k1:
        case NID_brainpoolP192r1:
        case NID_X9_62_prime192v1:
            return 192;
        case NID_secp224k1:
        case NID_secp224r1:
        case NID_brainpoolP224r1:
            return 224;
        case NID_secp256k1:
        case NID_brainpoolP256r1:
        case NID_X9_62_prime256v1:
            return 256;
        case NID_brainpoolP320r1:
            return 320;
        case NID_secp384r1:
        case NID_brainpoolP384r1:
            return 384;
        case NID_secp521r1:
        case NID_brainpoolP512r1:
            return 521;
        default:
            return WOLFSSL_FAILURE;
    }
}

/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
int wolfSSL_EC_GROUP_get_order(const WOLFSSL_EC_GROUP *group,
                               WOLFSSL_BIGNUM *order, WOLFSSL_BN_CTX *ctx)
{
    (void)ctx;

    if (group == NULL || order == NULL || order->internal == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_GROUP_get_order NULL error");
        return WOLFSSL_FAILURE;
    }

    if (mp_init((mp_int*)order->internal) != MP_OKAY) {
        WOLFSSL_MSG("wolfSSL_EC_GROUP_get_order mp_init failure");
        return WOLFSSL_FAILURE;
    }

    if (mp_read_radix((mp_int*)order->internal,
                  ecc_sets[group->curve_idx].order, MP_RADIX_HEX) != MP_OKAY) {
        WOLFSSL_MSG("wolfSSL_EC_GROUP_get_order mp_read order failure");
        mp_clear((mp_int*)order->internal);
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}
/* End EC_GROUP */

/* Start EC_POINT */

/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
int wolfSSL_ECPoint_i2d(const WOLFSSL_EC_GROUP *group,
                        const WOLFSSL_EC_POINT *p,
                        unsigned char *out, unsigned int *len)
{
    int err;

    WOLFSSL_ENTER("wolfSSL_ECPoint_i2d");

    if (group == NULL || p == NULL || len == NULL) {
        WOLFSSL_MSG("wolfSSL_ECPoint_i2d NULL error");
        return WOLFSSL_FAILURE;
    }

    if (p->inSet == 0) {
        WOLFSSL_MSG("No ECPoint internal set, do it");

        if (SetECPointInternal((WOLFSSL_EC_POINT *)p) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetECPointInternal SetECPointInternal failed");
            return WOLFSSL_FAILURE;
        }
    }

    if (out != NULL) {
        wolfSSL_EC_POINT_dump("i2d p", p);
    }

    err = wc_ecc_export_point_der(group->curve_idx, (ecc_point*)p->internal,
                                  out, len);
    if (err != MP_OKAY && !(out == NULL && err == LENGTH_ONLY_E)) {
        WOLFSSL_MSG("wolfSSL_ECPoint_i2d wc_ecc_export_point_der failed");
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}

/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
int wolfSSL_ECPoint_d2i(unsigned char *in, unsigned int len,
                        const WOLFSSL_EC_GROUP *group, WOLFSSL_EC_POINT *p)
{
    WOLFSSL_ENTER("wolfSSL_ECPoint_d2i");

    if (group == NULL || p == NULL || p->internal == NULL || in == NULL) {
        WOLFSSL_MSG("wolfSSL_ECPoint_d2i NULL error");
        return WOLFSSL_FAILURE;
    }

    if (wc_ecc_import_point_der(in, len, group->curve_idx,
                                (ecc_point*)p->internal) != MP_OKAY) {
        WOLFSSL_MSG("wc_ecc_import_point_der failed");
        return WOLFSSL_FAILURE;
    }

    if (p->exSet == 0) {
        WOLFSSL_MSG("No ECPoint external set, do it");

        if (SetECPointExternal(p) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetECPointExternal failed");
            return WOLFSSL_FAILURE;
        }
    }

    wolfSSL_EC_POINT_dump("d2i p", p);

    return WOLFSSL_SUCCESS;
}

WOLFSSL_EC_POINT *wolfSSL_EC_POINT_new(const WOLFSSL_EC_GROUP *group)
{
    WOLFSSL_EC_POINT *p;

    WOLFSSL_ENTER("wolfSSL_EC_POINT_new");

    if (group == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_POINT_new NULL error");
        return NULL;
    }

    p = (WOLFSSL_EC_POINT *)XMALLOC(sizeof(WOLFSSL_EC_POINT), NULL,
                                    DYNAMIC_TYPE_ECC);
    if (p == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_POINT_new malloc ecc point failure");
        return NULL;
    }
    XMEMSET(p, 0, sizeof(WOLFSSL_EC_POINT));

    p->internal = wc_ecc_new_point();
    if (p->internal == NULL) {
        WOLFSSL_MSG("ecc_new_point failure");
        XFREE(p, NULL, DYNAMIC_TYPE_ECC);
        return NULL;
    }

    return p;
}

/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
int wolfSSL_EC_POINT_get_affine_coordinates_GFp(const WOLFSSL_EC_GROUP *group,
                                                const WOLFSSL_EC_POINT *point,
                                                WOLFSSL_BIGNUM *x,
                                                WOLFSSL_BIGNUM *y,
                                                WOLFSSL_BN_CTX *ctx)
{
    (void)ctx;

    WOLFSSL_ENTER("wolfSSL_EC_POINT_get_affine_coordinates_GFp");

    if (group == NULL || point == NULL || point->internal == NULL ||
        x == NULL || y == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_POINT_get_affine_coordinates_GFp NULL error");
        return WOLFSSL_FAILURE;
    }

    if (point->inSet == 0) {
        WOLFSSL_MSG("No ECPoint internal set, do it");

        if (SetECPointInternal((WOLFSSL_EC_POINT *)point) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetECPointInternal failed");
            return WOLFSSL_FAILURE;
        }
    }

    BN_copy(x, point->X);
    BN_copy(y, point->Y);

    return WOLFSSL_SUCCESS;
}

#ifndef WOLFSSL_ATECC508A
/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
int wolfSSL_EC_POINT_mul(const WOLFSSL_EC_GROUP *group, WOLFSSL_EC_POINT *r,
                         const WOLFSSL_BIGNUM *n, const WOLFSSL_EC_POINT *q,
                         const WOLFSSL_BIGNUM *m, WOLFSSL_BN_CTX *ctx)
{
    mp_int a, prime;
    int ret;

    (void)ctx;
    (void)n;

    WOLFSSL_ENTER("wolfSSL_EC_POINT_mul");

    if (group == NULL || r == NULL || r->internal == NULL ||
        q == NULL || q->internal == NULL || m == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_POINT_mul NULL error");
        return WOLFSSL_FAILURE;
    }

    if (q->inSet == 0) {
        WOLFSSL_MSG("No ECPoint internal set, do it");

        if (SetECPointInternal((WOLFSSL_EC_POINT *)q) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetECPointInternal q failed");
            return WOLFSSL_FAILURE;
        }
    }

    /* read the curve prime and a */
    if (mp_init_multi(&prime, &a, NULL, NULL, NULL, NULL) != MP_OKAY) {
        return WOLFSSL_FAILURE;
    }

    ret = mp_read_radix(&prime, ecc_sets[group->curve_idx].prime, MP_RADIX_HEX);
    if (ret == MP_OKAY) {
        ret = mp_read_radix(&a, ecc_sets[group->curve_idx].Af, MP_RADIX_HEX);
    }

    /* r = q * m % prime */
    if (ret == MP_OKAY) {
        ret = wc_ecc_mulmod((mp_int*)m->internal, (ecc_point*)q->internal,
                      (ecc_point*)r->internal, &a, &prime, 1);
    }

    mp_clear(&a);
    mp_clear(&prime);

    if (ret == MP_OKAY) {
        r->inSet = 1; /* mark internal set */

        /* set the external value for the computed point */
        ret = SetECPointExternal(r);
        if (ret != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetECPointInternal r failed");
        }
    }
    else {
        ret = WOLFSSL_FAILURE;
    }

    return ret;
}
#endif

void wolfSSL_EC_POINT_clear_free(WOLFSSL_EC_POINT *p)
{
    WOLFSSL_ENTER("wolfSSL_EC_POINT_clear_free");

    wolfSSL_EC_POINT_free(p);
}

/* return code compliant with OpenSSL :
 *   0 if equal, 1 if not and -1 in case of error
 */
int wolfSSL_EC_POINT_cmp(const WOLFSSL_EC_GROUP *group,
                         const WOLFSSL_EC_POINT *a, const WOLFSSL_EC_POINT *b,
                         WOLFSSL_BN_CTX *ctx)
{
    int ret;

    (void)ctx;

    WOLFSSL_ENTER("wolfSSL_EC_POINT_cmp");

    if (group == NULL || a == NULL || a->internal == NULL || b == NULL ||
        b->internal == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_POINT_cmp Bad arguments");
        return WOLFSSL_FATAL_ERROR;
    }

    ret = wc_ecc_cmp_point((ecc_point*)a->internal, (ecc_point*)b->internal);
    if (ret == MP_EQ)
        return 0;
    else if (ret == MP_LT || ret == MP_GT)
        return 1;

    return WOLFSSL_FATAL_ERROR;
}
#endif /* HAVE_ECC */
#endif /* OPENSSL_EXTRA */

#if defined(HAVE_ECC) && (defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL))
void wolfSSL_EC_POINT_free(WOLFSSL_EC_POINT *p)
{
    WOLFSSL_ENTER("wolfSSL_EC_POINT_free");

    if (p != NULL) {
        if (p->internal != NULL) {
            wc_ecc_del_point((ecc_point*)p->internal);
            p->internal = NULL;
        }

        wolfSSL_BN_free(p->X);
        wolfSSL_BN_free(p->Y);
        wolfSSL_BN_free(p->Z);
        p->X = NULL;
        p->Y = NULL;
        p->Z = NULL;
        p->inSet = p->exSet = 0;

        XFREE(p, NULL, DYNAMIC_TYPE_ECC);
        p = NULL;
    }
}
#endif

#ifdef OPENSSL_EXTRA
#ifdef HAVE_ECC
/* return code compliant with OpenSSL :
 *   1 if point at infinity, 0 else
 */
int wolfSSL_EC_POINT_is_at_infinity(const WOLFSSL_EC_GROUP *group,
                                    const WOLFSSL_EC_POINT *point)
{
    int ret;

    WOLFSSL_ENTER("wolfSSL_EC_POINT_is_at_infinity");

    if (group == NULL || point == NULL || point->internal == NULL) {
        WOLFSSL_MSG("wolfSSL_EC_POINT_is_at_infinity NULL error");
        return WOLFSSL_FAILURE;
    }
    if (point->inSet == 0) {
        WOLFSSL_MSG("No ECPoint internal set, do it");

        if (SetECPointInternal((WOLFSSL_EC_POINT *)point) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetECPointInternal failed");
            return WOLFSSL_FAILURE;
        }
    }

    ret = wc_ecc_point_is_at_infinity((ecc_point*)point->internal);
    if (ret <= 0) {
        WOLFSSL_MSG("ecc_point_is_at_infinity failure");
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}

/* End EC_POINT */

/* Start ECDSA_SIG */
void wolfSSL_ECDSA_SIG_free(WOLFSSL_ECDSA_SIG *sig)
{
    WOLFSSL_ENTER("wolfSSL_ECDSA_SIG_free");

    if (sig) {
        wolfSSL_BN_free(sig->r);
        wolfSSL_BN_free(sig->s);

        XFREE(sig, NULL, DYNAMIC_TYPE_ECC);
    }
}

WOLFSSL_ECDSA_SIG *wolfSSL_ECDSA_SIG_new(void)
{
    WOLFSSL_ECDSA_SIG *sig;

    WOLFSSL_ENTER("wolfSSL_ECDSA_SIG_new");

    sig = (WOLFSSL_ECDSA_SIG*) XMALLOC(sizeof(WOLFSSL_ECDSA_SIG), NULL,
                                       DYNAMIC_TYPE_ECC);
    if (sig == NULL) {
        WOLFSSL_MSG("wolfSSL_ECDSA_SIG_new malloc ECDSA signature failure");
        return NULL;
    }

    sig->s = NULL;
    sig->r = wolfSSL_BN_new();
    if (sig->r == NULL) {
        WOLFSSL_MSG("wolfSSL_ECDSA_SIG_new malloc ECDSA r failure");
        wolfSSL_ECDSA_SIG_free(sig);
        return NULL;
    }

    sig->s = wolfSSL_BN_new();
    if (sig->s == NULL) {
        WOLFSSL_MSG("wolfSSL_ECDSA_SIG_new malloc ECDSA s failure");
        wolfSSL_ECDSA_SIG_free(sig);
        return NULL;
    }

    return sig;
}

/* return signature structure on success, NULL otherwise */
WOLFSSL_ECDSA_SIG *wolfSSL_ECDSA_do_sign(const unsigned char *d, int dlen,
                                         WOLFSSL_EC_KEY *key)
{
    WOLFSSL_ECDSA_SIG *sig = NULL;
    int     initTmpRng = 0;
    WC_RNG* rng = NULL;
#ifdef WOLFSSL_SMALL_STACK
    WC_RNG* tmpRNG = NULL;
#else
    WC_RNG  tmpRNG[1];
#endif

    WOLFSSL_ENTER("wolfSSL_ECDSA_do_sign");

    if (d == NULL || key == NULL || key->internal == NULL) {
        WOLFSSL_MSG("wolfSSL_ECDSA_do_sign Bad arguments");
        return NULL;
    }

    /* set internal key if not done */
    if (key->inSet == 0)
    {
        WOLFSSL_MSG("wolfSSL_ECDSA_do_sign No EC key internal set, do it");

        if (SetECKeyInternal(key) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("wolfSSL_ECDSA_do_sign SetECKeyInternal failed");
            return NULL;
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    tmpRNG = (WC_RNG*)XMALLOC(sizeof(WC_RNG), NULL, DYNAMIC_TYPE_RNG);
    if (tmpRNG == NULL)
        return NULL;
#endif

    if (wc_InitRng(tmpRNG) == 0) {
        rng = tmpRNG;
        initTmpRng = 1;
    }
    else {
        WOLFSSL_MSG("wolfSSL_ECDSA_do_sign Bad RNG Init, trying global");
        if (initGlobalRNG == 0)
            WOLFSSL_MSG("wolfSSL_ECDSA_do_sign Global RNG no Init");
        else
            rng = &globalRNG;
    }

    if (rng) {
        mp_int sig_r, sig_s;

        if (mp_init_multi(&sig_r, &sig_s, NULL, NULL, NULL, NULL) == MP_OKAY) {
            if (wc_ecc_sign_hash_ex(d, dlen, rng, (ecc_key*)key->internal,
                                    &sig_r, &sig_s) != MP_OKAY) {
                WOLFSSL_MSG("wc_ecc_sign_hash_ex failed");
            }
            else {
                /* put signature blob in ECDSA structure */
                sig = wolfSSL_ECDSA_SIG_new();
                if (sig == NULL)
                    WOLFSSL_MSG("wolfSSL_ECDSA_SIG_new failed");
                else if (SetIndividualExternal(&(sig->r), &sig_r)!=WOLFSSL_SUCCESS){
                    WOLFSSL_MSG("ecdsa r key error");
                    wolfSSL_ECDSA_SIG_free(sig);
                    sig = NULL;
                }
                else if (SetIndividualExternal(&(sig->s), &sig_s)!=WOLFSSL_SUCCESS){
                    WOLFSSL_MSG("ecdsa s key error");
                    wolfSSL_ECDSA_SIG_free(sig);
                    sig = NULL;
                }

            }
            mp_free(&sig_r);
            mp_free(&sig_s);
        }
    }

    if (initTmpRng)
        wc_FreeRng(tmpRNG);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(tmpRNG, NULL, DYNAMIC_TYPE_RNG);
#endif

    return sig;
}

/* return code compliant with OpenSSL :
 *   1 for a valid signature, 0 for an invalid signature and -1 on error
 */
int wolfSSL_ECDSA_do_verify(const unsigned char *d, int dlen,
                            const WOLFSSL_ECDSA_SIG *sig, WOLFSSL_EC_KEY *key)
{
    int check_sign = 0;

    WOLFSSL_ENTER("wolfSSL_ECDSA_do_verify");

    if (d == NULL || sig == NULL || key == NULL || key->internal == NULL) {
        WOLFSSL_MSG("wolfSSL_ECDSA_do_verify Bad arguments");
        return WOLFSSL_FATAL_ERROR;
    }

    /* set internal key if not done */
    if (key->inSet == 0)
    {
        WOLFSSL_MSG("No EC key internal set, do it");

        if (SetECKeyInternal(key) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetECKeyInternal failed");
            return WOLFSSL_FATAL_ERROR;
        }
    }

    if (wc_ecc_verify_hash_ex((mp_int*)sig->r->internal,
                              (mp_int*)sig->s->internal, d, dlen, &check_sign,
                              (ecc_key *)key->internal) != MP_OKAY) {
        WOLFSSL_MSG("wc_ecc_verify_hash failed");
        return WOLFSSL_FATAL_ERROR;
    }
    else if (check_sign == 0) {
        WOLFSSL_MSG("wc_ecc_verify_hash incorrect signature detected");
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}
/* End ECDSA_SIG */

/* Start ECDH */
/* return code compliant with OpenSSL :
 *   length of computed key if success, -1 if error
 */
int wolfSSL_ECDH_compute_key(void *out, size_t outlen,
                             const WOLFSSL_EC_POINT *pub_key,
                             WOLFSSL_EC_KEY *ecdh,
                             void *(*KDF) (const void *in, size_t inlen,
                                           void *out, size_t *outlen))
{
    word32 len;
    (void)KDF;

    (void)KDF;

    WOLFSSL_ENTER("wolfSSL_ECDH_compute_key");

    if (out == NULL || pub_key == NULL || pub_key->internal == NULL ||
        ecdh == NULL || ecdh->internal == NULL) {
        WOLFSSL_MSG("Bad function arguments");
        return WOLFSSL_FATAL_ERROR;
    }

    /* set internal key if not done */
    if (ecdh->inSet == 0)
    {
        WOLFSSL_MSG("No EC key internal set, do it");

        if (SetECKeyInternal(ecdh) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetECKeyInternal failed");
            return WOLFSSL_FATAL_ERROR;
        }
    }

    len = (word32)outlen;

    if (wc_ecc_shared_secret_ssh((ecc_key*)ecdh->internal,
                                 (ecc_point*)pub_key->internal,
                                 (byte *)out, &len) != MP_OKAY) {
        WOLFSSL_MSG("wc_ecc_shared_secret failed");
        return WOLFSSL_FATAL_ERROR;
    }

    return len;
}
/* End ECDH */

#if !defined(NO_FILESYSTEM)
/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
#ifndef NO_WOLFSSL_STUB
int wolfSSL_PEM_write_EC_PUBKEY(FILE *fp, WOLFSSL_EC_KEY *x)
{
    (void)fp;
    (void)x;
    WOLFSSL_STUB("PEM_write_EC_PUBKEY");
    WOLFSSL_MSG("wolfSSL_PEM_write_EC_PUBKEY not implemented");

    return WOLFSSL_FAILURE;
}
#endif

#endif /* NO_FILESYSTEM */

#if defined(WOLFSSL_KEY_GEN)

/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
#ifndef NO_WOLFSSL_STUB
int wolfSSL_PEM_write_bio_ECPrivateKey(WOLFSSL_BIO* bio, WOLFSSL_EC_KEY* ecc,
                                       const EVP_CIPHER* cipher,
                                       unsigned char* passwd, int len,
                                       pem_password_cb* cb, void* arg)
{
    (void)bio;
    (void)ecc;
    (void)cipher;
    (void)passwd;
    (void)len;
    (void)cb;
    (void)arg;
    WOLFSSL_STUB("PEM_write_bio_ECPrivateKey");
    WOLFSSL_MSG("wolfSSL_PEM_write_bio_ECPrivateKey not implemented");

    return WOLFSSL_FAILURE;
}
#endif

/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
int wolfSSL_PEM_write_mem_ECPrivateKey(WOLFSSL_EC_KEY* ecc,
                                       const EVP_CIPHER* cipher,
                                       unsigned char* passwd, int passwdSz,
                                       unsigned char **pem, int *plen)
{
#if defined(WOLFSSL_PEM_TO_DER) || defined(WOLFSSL_DER_TO_PEM)
    byte *derBuf, *tmp, *cipherInfo = NULL;
    int  der_max_len = 0, derSz = 0;
    const int type = ECC_PRIVATEKEY_TYPE;
    const char* header = NULL;
    const char* footer = NULL;

    WOLFSSL_MSG("wolfSSL_PEM_write_mem_ECPrivateKey");

    if (pem == NULL || plen == NULL || ecc == NULL || ecc->internal == NULL) {
        WOLFSSL_MSG("Bad function arguments");
        return WOLFSSL_FAILURE;
    }

    if (wc_PemGetHeaderFooter(type, &header, &footer) != 0)
        return WOLFSSL_FAILURE;

    if (ecc->inSet == 0) {
        WOLFSSL_MSG("No ECC internal set, do it");

        if (SetECKeyInternal(ecc) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetDsaInternal failed");
            return WOLFSSL_FAILURE;
        }
    }

    /* 4 > size of pub, priv + ASN.1 additional informations
     */
    der_max_len = 4 * wc_ecc_size((ecc_key*)ecc->internal) + AES_BLOCK_SIZE;

    derBuf = (byte*)XMALLOC(der_max_len, NULL, DYNAMIC_TYPE_DER);
    if (derBuf == NULL) {
        WOLFSSL_MSG("malloc failed");
        return WOLFSSL_FAILURE;
    }

    /* Key to DER */
    derSz = wc_EccKeyToDer((ecc_key*)ecc->internal, derBuf, der_max_len);
    if (derSz < 0) {
        WOLFSSL_MSG("wc_DsaKeyToDer failed");
        XFREE(derBuf, NULL, DYNAMIC_TYPE_DER);
        return WOLFSSL_FAILURE;
    }

    /* encrypt DER buffer if required */
    if (passwd != NULL && passwdSz > 0 && cipher != NULL) {
        int ret;

        ret = EncryptDerKey(derBuf, &derSz, cipher,
                            passwd, passwdSz, &cipherInfo);
        if (ret != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("EncryptDerKey failed");
            XFREE(derBuf, NULL, DYNAMIC_TYPE_DER);
            return ret;
        }

        /* tmp buffer with a max size */
        *plen = (derSz * 2) + (int)XSTRLEN(header) + 1 +
            (int)XSTRLEN(footer) + 1 + HEADER_ENCRYPTED_KEY_SIZE;
    }
    else { /* tmp buffer with a max size */
        *plen = (derSz * 2) + (int)XSTRLEN(header) + 1 +
            (int)XSTRLEN(footer) + 1;
    }

    tmp = (byte*)XMALLOC(*plen, NULL, DYNAMIC_TYPE_PEM);
    if (tmp == NULL) {
        WOLFSSL_MSG("malloc failed");
        XFREE(derBuf, NULL, DYNAMIC_TYPE_DER);
        if (cipherInfo != NULL)
            XFREE(cipherInfo, NULL, DYNAMIC_TYPE_STRING);
        return WOLFSSL_FAILURE;
    }

    /* DER to PEM */
    *plen = wc_DerToPemEx(derBuf, derSz, tmp, *plen, cipherInfo, type);
    if (*plen <= 0) {
        WOLFSSL_MSG("wc_DerToPemEx failed");
        XFREE(derBuf, NULL, DYNAMIC_TYPE_DER);
        XFREE(tmp, NULL, DYNAMIC_TYPE_PEM);
        if (cipherInfo != NULL)
            XFREE(cipherInfo, NULL, DYNAMIC_TYPE_STRING);
        return WOLFSSL_FAILURE;
    }
    XFREE(derBuf, NULL, DYNAMIC_TYPE_DER);
    if (cipherInfo != NULL)
        XFREE(cipherInfo, NULL, DYNAMIC_TYPE_STRING);

    *pem = (byte*)XMALLOC((*plen)+1, NULL, DYNAMIC_TYPE_KEY);
    if (*pem == NULL) {
        WOLFSSL_MSG("malloc failed");
        XFREE(tmp, NULL, DYNAMIC_TYPE_PEM);
        return WOLFSSL_FAILURE;
    }
    XMEMSET(*pem, 0, (*plen)+1);

    if (XMEMCPY(*pem, tmp, *plen) == NULL) {
        WOLFSSL_MSG("XMEMCPY failed");
        XFREE(pem, NULL, DYNAMIC_TYPE_KEY);
        XFREE(tmp, NULL, DYNAMIC_TYPE_PEM);
        return WOLFSSL_FAILURE;
    }
    XFREE(tmp, NULL, DYNAMIC_TYPE_PEM);

    return WOLFSSL_SUCCESS;
#else
    (void)ecc;
    (void)cipher;
    (void)passwd;
    (void)passwdSz;
    (void)pem;
    (void)plen;
    return WOLFSSL_FAILURE;
#endif /* WOLFSSL_PEM_TO_DER || WOLFSSL_DER_TO_PEM */
}

#ifndef NO_FILESYSTEM
/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
int wolfSSL_PEM_write_ECPrivateKey(FILE *fp, WOLFSSL_EC_KEY *ecc,
                                   const EVP_CIPHER *enc,
                                   unsigned char *kstr, int klen,
                                   pem_password_cb *cb, void *u)
{
    byte *pem;
    int  plen, ret;

    (void)cb;
    (void)u;

    WOLFSSL_MSG("wolfSSL_PEM_write_ECPrivateKey");

    if (fp == NULL || ecc == NULL || ecc->internal == NULL) {
        WOLFSSL_MSG("Bad function arguments");
        return WOLFSSL_FAILURE;
    }

    ret = wolfSSL_PEM_write_mem_ECPrivateKey(ecc, enc, kstr, klen, &pem, &plen);
    if (ret != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("wolfSSL_PEM_write_mem_ECPrivateKey failed");
        return WOLFSSL_FAILURE;
    }

    ret = (int)XFWRITE(pem, plen, 1, fp);
    if (ret != 1) {
        WOLFSSL_MSG("ECC private key file write failed");
        return WOLFSSL_FAILURE;
    }

    XFREE(pem, NULL, DYNAMIC_TYPE_KEY);
    return WOLFSSL_SUCCESS;
}

#endif /* NO_FILESYSTEM */
#endif /* defined(WOLFSSL_KEY_GEN) */

#endif /* HAVE_ECC */


#ifndef NO_DSA

#if defined(WOLFSSL_KEY_GEN)

/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
int wolfSSL_PEM_write_bio_DSAPrivateKey(WOLFSSL_BIO* bio, WOLFSSL_DSA* dsa,
                                       const EVP_CIPHER* cipher,
                                       unsigned char* passwd, int len,
                                       pem_password_cb* cb, void* arg)
{
    (void)bio;
    (void)dsa;
    (void)cipher;
    (void)passwd;
    (void)len;
    (void)cb;
    (void)arg;

    WOLFSSL_MSG("wolfSSL_PEM_write_bio_DSAPrivateKey not implemented");

    return WOLFSSL_FAILURE;
}

/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
int wolfSSL_PEM_write_mem_DSAPrivateKey(WOLFSSL_DSA* dsa,
                                        const EVP_CIPHER* cipher,
                                        unsigned char* passwd, int passwdSz,
                                        unsigned char **pem, int *plen)
{
#if defined(WOLFSSL_PEM_TO_DER) || defined(WOLFSSL_DER_TO_PEM)
    byte *derBuf, *tmp, *cipherInfo = NULL;
    int  der_max_len = 0, derSz = 0;
    const int type = DSA_PRIVATEKEY_TYPE;
    const char* header = NULL;
    const char* footer = NULL;

    WOLFSSL_MSG("wolfSSL_PEM_write_mem_DSAPrivateKey");

    if (pem == NULL || plen == NULL || dsa == NULL || dsa->internal == NULL) {
        WOLFSSL_MSG("Bad function arguments");
        return WOLFSSL_FAILURE;
    }

    if (wc_PemGetHeaderFooter(type, &header, &footer) != 0)
        return WOLFSSL_FAILURE;

    if (dsa->inSet == 0) {
        WOLFSSL_MSG("No DSA internal set, do it");

        if (SetDsaInternal(dsa) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("SetDsaInternal failed");
            return WOLFSSL_FAILURE;
        }
    }

    /* 4 > size of pub, priv, p, q, g + ASN.1 additional informations
     */
    der_max_len = 4 * wolfSSL_BN_num_bytes(dsa->g) + AES_BLOCK_SIZE;

    derBuf = (byte*)XMALLOC(der_max_len, NULL, DYNAMIC_TYPE_DER);
    if (derBuf == NULL) {
        WOLFSSL_MSG("malloc failed");
        return WOLFSSL_FAILURE;
    }

    /* Key to DER */
    derSz = wc_DsaKeyToDer((DsaKey*)dsa->internal, derBuf, der_max_len);
    if (derSz < 0) {
        WOLFSSL_MSG("wc_DsaKeyToDer failed");
        XFREE(derBuf, NULL, DYNAMIC_TYPE_DER);
        return WOLFSSL_FAILURE;
    }

    /* encrypt DER buffer if required */
    if (passwd != NULL && passwdSz > 0 && cipher != NULL) {
        int ret;

        ret = EncryptDerKey(derBuf, &derSz, cipher,
                            passwd, passwdSz, &cipherInfo);
        if (ret != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("EncryptDerKey failed");
            XFREE(derBuf, NULL, DYNAMIC_TYPE_DER);
            return ret;
        }

        /* tmp buffer with a max size */
        *plen = (derSz * 2) + (int)XSTRLEN(header) + 1 +
            (int)XSTRLEN(footer) + 1 + HEADER_ENCRYPTED_KEY_SIZE;
    }
    else { /* tmp buffer with a max size */
        *plen = (derSz * 2) + (int)XSTRLEN(header) + 1 +
            (int)XSTRLEN(footer) + 1;
    }

    tmp = (byte*)XMALLOC(*plen, NULL, DYNAMIC_TYPE_PEM);
    if (tmp == NULL) {
        WOLFSSL_MSG("malloc failed");
        XFREE(derBuf, NULL, DYNAMIC_TYPE_DER);
        if (cipherInfo != NULL)
            XFREE(cipherInfo, NULL, DYNAMIC_TYPE_STRING);
        return WOLFSSL_FAILURE;
    }

    /* DER to PEM */
    *plen = wc_DerToPemEx(derBuf, derSz, tmp, *plen, cipherInfo, type);
    if (*plen <= 0) {
        WOLFSSL_MSG("wc_DerToPemEx failed");
        XFREE(derBuf, NULL, DYNAMIC_TYPE_DER);
        XFREE(tmp, NULL, DYNAMIC_TYPE_PEM);
        if (cipherInfo != NULL)
            XFREE(cipherInfo, NULL, DYNAMIC_TYPE_STRING);
        return WOLFSSL_FAILURE;
    }
    XFREE(derBuf, NULL, DYNAMIC_TYPE_DER);
    if (cipherInfo != NULL)
        XFREE(cipherInfo, NULL, DYNAMIC_TYPE_STRING);

    *pem = (byte*)XMALLOC((*plen)+1, NULL, DYNAMIC_TYPE_KEY);
    if (*pem == NULL) {
        WOLFSSL_MSG("malloc failed");
        XFREE(tmp, NULL, DYNAMIC_TYPE_PEM);
        return WOLFSSL_FAILURE;
    }
    XMEMSET(*pem, 0, (*plen)+1);

    if (XMEMCPY(*pem, tmp, *plen) == NULL) {
        WOLFSSL_MSG("XMEMCPY failed");
        XFREE(pem, NULL, DYNAMIC_TYPE_KEY);
        XFREE(tmp, NULL, DYNAMIC_TYPE_PEM);
        return WOLFSSL_FAILURE;
    }
    XFREE(tmp, NULL, DYNAMIC_TYPE_PEM);

    return WOLFSSL_SUCCESS;
#else
    (void)dsa;
    (void)cipher;
    (void)passwd;
    (void)passwdSz;
    (void)pem;
    (void)plen;
    return WOLFSSL_FAILURE;
#endif /* WOLFSSL_PEM_TO_DER || WOLFSSL_DER_TO_PEM */
}

#ifndef NO_FILESYSTEM
/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
int wolfSSL_PEM_write_DSAPrivateKey(FILE *fp, WOLFSSL_DSA *dsa,
                                    const EVP_CIPHER *enc,
                                    unsigned char *kstr, int klen,
                                    pem_password_cb *cb, void *u)
{
    byte *pem;
    int  plen, ret;

    (void)cb;
    (void)u;

    WOLFSSL_MSG("wolfSSL_PEM_write_DSAPrivateKey");

    if (fp == NULL || dsa == NULL || dsa->internal == NULL) {
        WOLFSSL_MSG("Bad function arguments");
        return WOLFSSL_FAILURE;
    }

    ret = wolfSSL_PEM_write_mem_DSAPrivateKey(dsa, enc, kstr, klen, &pem, &plen);
    if (ret != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("wolfSSL_PEM_write_mem_DSAPrivateKey failed");
        return WOLFSSL_FAILURE;
    }

    ret = (int)XFWRITE(pem, plen, 1, fp);
    if (ret != 1) {
        WOLFSSL_MSG("DSA private key file write failed");
        return WOLFSSL_FAILURE;
    }

    XFREE(pem, NULL, DYNAMIC_TYPE_KEY);
    return WOLFSSL_SUCCESS;
}

#endif /* NO_FILESYSTEM */
#endif /* defined(WOLFSSL_KEY_GEN) */

#ifndef NO_FILESYSTEM
/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
#ifndef NO_WOLFSSL_STUB
int wolfSSL_PEM_write_DSA_PUBKEY(FILE *fp, WOLFSSL_DSA *x)
{
    (void)fp;
    (void)x;
    WOLFSSL_STUB("PEM_write_DSA_PUBKEY");
    WOLFSSL_MSG("wolfSSL_PEM_write_DSA_PUBKEY not implemented");

    return WOLFSSL_FAILURE;
}
#endif
#endif /* NO_FILESYSTEM */

#endif /* #ifndef NO_DSA */


WOLFSSL_EVP_PKEY* wolfSSL_PEM_read_bio_PrivateKey(WOLFSSL_BIO* bio,
                    WOLFSSL_EVP_PKEY** key, pem_password_cb* cb, void* pass)
{
    WOLFSSL_EVP_PKEY* pkey = NULL;
#ifdef WOLFSSL_SMALL_STACK
    EncryptedInfo* info;
#else
    EncryptedInfo info[1];
#endif /* WOLFSSL_SMALL_STACK */
    pem_password_cb* localCb = cb;
    DerBuffer* der = NULL;

    char* mem = NULL;
    int memSz;
    int ret;
    int eccFlag = 0;

    WOLFSSL_ENTER("wolfSSL_PEM_read_bio_PrivateKey");

    if (bio == NULL) {
        return pkey;
    }

    if ((ret = wolfSSL_BIO_pending(bio)) > 0) {
        memSz = ret;
        mem = (char*)XMALLOC(memSz, bio->heap, DYNAMIC_TYPE_OPENSSL);
        if (mem == NULL) {
            WOLFSSL_MSG("Memory error");
            return NULL;
        }

        if ((ret = wolfSSL_BIO_read(bio, mem, memSz)) <= 0) {
            WOLFSSL_LEAVE("wolfSSL_PEM_read_bio_PrivateKey", ret);
            XFREE(mem, bio->heap, DYNAMIC_TYPE_OPENSSL);
            return NULL;
        }
    }
    else if (bio->type == WOLFSSL_BIO_FILE) {
        int sz  = 100; /* read from file by 100 byte chuncks */
        int idx = 0;
        char* tmp = (char*)XMALLOC(sz, bio->heap, DYNAMIC_TYPE_OPENSSL);

        memSz = 0;
        if (tmp == NULL) {
            WOLFSSL_MSG("Memory error");
            return NULL;
        }

        while ((sz = wolfSSL_BIO_read(bio, tmp, sz)) > 0) {
            if (memSz + sz < 0) {
                /* sanity check */
                break;
            }
            mem = (char*)XREALLOC(mem, memSz + sz, bio->heap,
                    DYNAMIC_TYPE_OPENSSL);
            if (mem == NULL) {
                WOLFSSL_MSG("Memory error");
                XFREE(tmp, bio->heap, DYNAMIC_TYPE_OPENSSL);
                return NULL;
            }
            XMEMCPY(mem + idx, tmp, sz);
            memSz += sz;
            idx   += sz;
            sz = 100; /* read another 100 byte chunck from file */
        }
        XFREE(tmp, bio->heap, DYNAMIC_TYPE_OPENSSL);
        if (memSz <= 0) {
            WOLFSSL_MSG("No data to read from bio");
            if (mem != NULL) {
                XFREE(mem, bio->heap, DYNAMIC_TYPE_OPENSSL);
            }
            return NULL;
        }
    }
    else {
        WOLFSSL_MSG("No data to read from bio");
        return NULL;
    }

#ifdef WOLFSSL_SMALL_STACK
    info = (EncryptedInfo*)XMALLOC(sizeof(EncryptedInfo), NULL,
                                   DYNAMIC_TYPE_TMP_BUFFER);
    if (info == NULL) {
        WOLFSSL_MSG("Error getting memory for EncryptedInfo structure");
        XFREE(mem, bio->heap, DYNAMIC_TYPE_OPENSSL);
        return NULL;
    }
#endif

    XMEMSET(info, 0, sizeof(EncryptedInfo));
    info->passwd_cb       = localCb;
    info->passwd_userdata = pass;
    ret = PemToDer((const unsigned char*)mem, memSz, PRIVATEKEY_TYPE, &der,
        NULL, info, &eccFlag);

    if (ret < 0) {
        WOLFSSL_MSG("Bad Pem To Der");
    }
    else {
        int type;
        const unsigned char* ptr = der->buffer;

        /* write left over data back to bio */
        if ((memSz - (int)info->consumed) > 0 &&
                bio->type != WOLFSSL_BIO_FILE) {
            if (wolfSSL_BIO_write(bio, mem + (int)info->consumed,
                                   memSz - (int)info->consumed) <= 0) {
                WOLFSSL_MSG("Unable to advance bio read pointer");
            }
        }

        if (eccFlag) {
            type = EVP_PKEY_EC;
        }
        else {
            type = EVP_PKEY_RSA;
        }

        /* handle case where reuse is attempted */
        if (key != NULL && *key != NULL) {
            pkey = *key;
        }

        wolfSSL_d2i_PrivateKey(type, &pkey, &ptr, der->length);
        if (pkey == NULL) {
            WOLFSSL_MSG("Error loading DER buffer into WOLFSSL_EVP_PKEY");
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(info, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    XFREE(mem, bio->heap, DYNAMIC_TYPE_OPENSSL);
    FreeDer(&der);

    if (key != NULL) {
        *key = pkey;
    }

    return pkey;
}


#ifndef NO_RSA
/* Uses the same format of input as wolfSSL_PEM_read_bio_PrivateKey but expects
 * the results to be an RSA key.
 *
 * bio  structure to read RSA private key from
 * rsa  if not null is then set to the result
 * cb   password callback for reading PEM
 * pass password string
 *
 * returns a pointer to a new WOLFSSL_RSA structure on success and NULL on fail
 */
WOLFSSL_RSA* wolfSSL_PEM_read_bio_RSAPrivateKey(WOLFSSL_BIO* bio,
        WOLFSSL_RSA** rsa, pem_password_cb* cb, void* pass)
{
    WOLFSSL_EVP_PKEY* pkey;
    WOLFSSL_RSA* local;

    pkey = wolfSSL_PEM_read_bio_PrivateKey(bio, NULL, cb, pass);
    if (pkey == NULL) {
        return NULL;
    }

    /* Since the WOLFSSL_RSA structure is being taken from WOLFSSL_EVP_PEKY the
     * flag indicating that the WOLFSSL_RSA structure is owned should be FALSE
     * to avoid having it free'd */
    pkey->ownRsa = 0;
    local = pkey->rsa;
    if (rsa != NULL) {
        *rsa = local;
    }

    wolfSSL_EVP_PKEY_free(pkey);
    return local;
}
#endif /* !NO_RSA */


/* return of pkey->type which will be EVP_PKEY_RSA for example.
 *
 * type  type of EVP_PKEY
 *
 * returns type or if type is not found then NID_undef
 */
int wolfSSL_EVP_PKEY_type(int type)
{
    WOLFSSL_MSG("wolfSSL_EVP_PKEY_type");

    switch (type) {
    #ifdef OPENSSL_EXTRA
        case EVP_PKEY_RSA:
            return EVP_PKEY_RSA;
        case EVP_PKEY_DSA:
            return EVP_PKEY_DSA;
        case EVP_PKEY_EC:
            return EVP_PKEY_EC;
    #endif
        default:
            return NID_undef;
    }
}


int wolfSSL_EVP_PKEY_base_id(const EVP_PKEY *pkey)
{
    return EVP_PKEY_type(pkey->type);
}


#if !defined(NO_FILESYSTEM)
WOLFSSL_EVP_PKEY *wolfSSL_PEM_read_PUBKEY(FILE *fp, EVP_PKEY **x,
                                          pem_password_cb *cb, void *u)
{
    (void)fp;
    (void)x;
    (void)cb;
    (void)u;

    WOLFSSL_MSG("wolfSSL_PEM_read_PUBKEY not implemented");

    return NULL;
}
#endif /* NO_FILESYSTEM */

#ifndef NO_RSA

#if !defined(NO_FILESYSTEM)
#ifndef NO_WOLFSSL_STUB
WOLFSSL_RSA *wolfSSL_PEM_read_RSAPublicKey(FILE *fp, WOLFSSL_RSA **x,
                                           pem_password_cb *cb, void *u)
{
    (void)fp;
    (void)x;
    (void)cb;
    (void)u;
    WOLFSSL_STUB("PEM_read_RSAPublicKey");
    WOLFSSL_MSG("wolfSSL_PEM_read_RSAPublicKey not implemented");

    return NULL;
}
#endif
/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
#ifndef NO_WOLFSSL_STUB
int wolfSSL_PEM_write_RSAPublicKey(FILE *fp, WOLFSSL_RSA *x)
{
    (void)fp;
    (void)x;
    WOLFSSL_STUB("PEM_write_RSAPublicKey");
    WOLFSSL_MSG("wolfSSL_PEM_write_RSAPublicKey not implemented");

    return WOLFSSL_FAILURE;
}
#endif

/* return code compliant with OpenSSL :
 *   1 if success, 0 if error
 */
#ifndef NO_WOLFSSL_STUB
int wolfSSL_PEM_write_RSA_PUBKEY(FILE *fp, WOLFSSL_RSA *x)
{
    (void)fp;
    (void)x;
    WOLFSSL_STUB("PEM_write_RSA_PUBKEY");
    WOLFSSL_MSG("wolfSSL_PEM_write_RSA_PUBKEY not implemented");

    return WOLFSSL_FAILURE;
}
#endif

#endif /* NO_FILESYSTEM */

WOLFSSL_RSA *wolfSSL_d2i_RSAPublicKey(WOLFSSL_RSA **r, const unsigned char **pp, long len)
{
    WOLFSSL_RSA *rsa = NULL;

    WOLFSSL_ENTER("d2i_RSAPublicKey");
    if(pp == NULL){
        WOLFSSL_MSG("Bad argument");
        return NULL;
    }
    if((rsa = wolfSSL_RSA_new()) == NULL){
        WOLFSSL_MSG("RSA_new failed");
        return NULL;
    }
    
    if(wolfSSL_RSA_LoadDer_ex(rsa, *pp, (int)len, WOLFSSL_RSA_LOAD_PUBLIC)
                                                     != WOLFSSL_SUCCESS){
        WOLFSSL_MSG("RSA_LoadDer failed");
        return NULL;
    }
    if(r != NULL)
        *r = rsa;
    return rsa;
}

#if !defined(HAVE_FAST_RSA)
int wolfSSL_i2d_RSAPublicKey(WOLFSSL_RSA *rsa, const unsigned char **pp)
{
    byte *der;
    int derLen;
    int ret;

    WOLFSSL_ENTER("i2d_RSAPublicKey");
    if((rsa == NULL) || (pp == NULL))
        return WOLFSSL_FATAL_ERROR;
    if((ret = SetRsaInternal(rsa)) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("SetRsaInternal Failed");
        return ret;
    }
    if((derLen = RsaPublicKeyDerSize((RsaKey *)rsa->internal, 1)) < 0)
        return WOLFSSL_FATAL_ERROR;
    der = (byte*)XMALLOC(derLen, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (der == NULL) {
        return WOLFSSL_FATAL_ERROR;
    }
    if((ret = wc_RsaKeyToPublicDer((RsaKey *)rsa->internal, der, derLen)) < 0){
        WOLFSSL_MSG("RsaKeyToPublicDer failed");
        XFREE(der, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return ret;
    }

    *pp = der;
    return ret;
}
#endif /* #if !defined(HAVE_FAST_RSA) */

#endif /* !NO_RSA */
#endif /* OPENSSL_EXTRA */

#if !defined(NO_RSA) && (defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL))
/* return WOLFSSL_SUCCESS if success, WOLFSSL_FATAL_ERROR if error */
int wolfSSL_RSA_LoadDer(WOLFSSL_RSA* rsa, const unsigned char* derBuf, int derSz)
{
  return wolfSSL_RSA_LoadDer_ex(rsa, derBuf, derSz, WOLFSSL_RSA_LOAD_PRIVATE);
}


int wolfSSL_RSA_LoadDer_ex(WOLFSSL_RSA* rsa, const unsigned char* derBuf,
                                                     int derSz, int opt)
{

    word32 idx = 0;
    int    ret;

    WOLFSSL_ENTER("wolfSSL_RSA_LoadDer");

    if (rsa == NULL || rsa->internal == NULL || derBuf == NULL || derSz <= 0) {
        WOLFSSL_MSG("Bad function arguments");
        return WOLFSSL_FATAL_ERROR;
    }

    if (opt == WOLFSSL_RSA_LOAD_PRIVATE) {
        ret = wc_RsaPrivateKeyDecode(derBuf, &idx, (RsaKey*)rsa->internal, derSz);
    }
    else {
        ret = wc_RsaPublicKeyDecode(derBuf, &idx, (RsaKey*)rsa->internal, derSz);
    }

    if (ret < 0) {
        if (opt == WOLFSSL_RSA_LOAD_PRIVATE) {
             WOLFSSL_MSG("RsaPrivateKeyDecode failed");
        }
        else {
             WOLFSSL_MSG("RsaPublicKeyDecode failed");
        }
        return SSL_FATAL_ERROR;
    }

    if (SetRsaExternal(rsa) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("SetRsaExternal failed");
        return WOLFSSL_FATAL_ERROR;
    }

    rsa->inSet = 1;

    return WOLFSSL_SUCCESS;
}
#endif /* NO_RSA */

#ifdef OPENSSL_EXTRA
#ifndef NO_DSA
/* return WOLFSSL_SUCCESS if success, WOLFSSL_FATAL_ERROR if error */
int wolfSSL_DSA_LoadDer(WOLFSSL_DSA* dsa, const unsigned char* derBuf, int derSz)
{
    word32 idx = 0;
    int    ret;

    WOLFSSL_ENTER("wolfSSL_DSA_LoadDer");

    if (dsa == NULL || dsa->internal == NULL || derBuf == NULL || derSz <= 0) {
        WOLFSSL_MSG("Bad function arguments");
        return WOLFSSL_FATAL_ERROR;
    }

    ret = DsaPrivateKeyDecode(derBuf, &idx, (DsaKey*)dsa->internal, derSz);
    if (ret < 0) {
        WOLFSSL_MSG("DsaPrivateKeyDecode failed");
        return WOLFSSL_FATAL_ERROR;
    }

    if (SetDsaExternal(dsa) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("SetDsaExternal failed");
        return WOLFSSL_FATAL_ERROR;
    }

    dsa->inSet = 1;

    return WOLFSSL_SUCCESS;
}
#endif /* NO_DSA */

#ifdef HAVE_ECC
/* return WOLFSSL_SUCCESS if success, WOLFSSL_FATAL_ERROR if error */
int wolfSSL_EC_KEY_LoadDer(WOLFSSL_EC_KEY* key,
                           const unsigned char* derBuf,  int derSz)
{
    word32 idx = 0;
    int    ret;

    WOLFSSL_ENTER("wolfSSL_EC_KEY_LoadDer");

    if (key == NULL || key->internal == NULL || derBuf == NULL || derSz <= 0) {
        WOLFSSL_MSG("Bad function arguments");
        return WOLFSSL_FATAL_ERROR;
    }

    ret = wc_EccPrivateKeyDecode(derBuf, &idx, (ecc_key*)key->internal, derSz);
    if (ret < 0) {
        WOLFSSL_MSG("wc_EccPrivateKeyDecode failed");
        return WOLFSSL_FATAL_ERROR;
    }

    if (SetECKeyExternal(key) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("SetECKeyExternal failed");
        return WOLFSSL_FATAL_ERROR;
    }

    key->inSet = 1;

    return WOLFSSL_SUCCESS;
}
#endif /* HAVE_ECC */


#endif /* OPENSSL_EXTRA */


#ifdef WOLFSSL_ALT_CERT_CHAINS
int wolfSSL_is_peer_alt_cert_chain(const WOLFSSL* ssl)
{
    int isUsing = 0;
    if (ssl)
        isUsing = ssl->options.usingAltCertChain;
    return isUsing;
}
#endif /* WOLFSSL_ALT_CERT_CHAINS */


#ifdef SESSION_CERTS

#ifdef WOLFSSL_ALT_CERT_CHAINS
/* Get peer's alternate certificate chain */
WOLFSSL_X509_CHAIN* wolfSSL_get_peer_alt_chain(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("wolfSSL_get_peer_alt_chain");
    if (ssl)
        return &ssl->session.altChain;

    return 0;
}
#endif /* WOLFSSL_ALT_CERT_CHAINS */


/* Get peer's certificate chain */
WOLFSSL_X509_CHAIN* wolfSSL_get_peer_chain(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("wolfSSL_get_peer_chain");
    if (ssl)
        return &ssl->session.chain;

    return 0;
}


/* Get peer's certificate chain total count */
int wolfSSL_get_chain_count(WOLFSSL_X509_CHAIN* chain)
{
    WOLFSSL_ENTER("wolfSSL_get_chain_count");
    if (chain)
        return chain->count;

    return 0;
}


/* Get peer's ASN.1 DER certificate at index (idx) length in bytes */
int wolfSSL_get_chain_length(WOLFSSL_X509_CHAIN* chain, int idx)
{
    WOLFSSL_ENTER("wolfSSL_get_chain_length");
    if (chain)
        return chain->certs[idx].length;

    return 0;
}


/* Get peer's ASN.1 DER certificate at index (idx) */
byte* wolfSSL_get_chain_cert(WOLFSSL_X509_CHAIN* chain, int idx)
{
    WOLFSSL_ENTER("wolfSSL_get_chain_cert");
    if (chain)
        return chain->certs[idx].buffer;

    return 0;
}


/* Get peer's wolfSSL X509 certificate at index (idx) */
WOLFSSL_X509* wolfSSL_get_chain_X509(WOLFSSL_X509_CHAIN* chain, int idx)
{
    int          ret;
    WOLFSSL_X509* x509 = NULL;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert* cert = NULL;
#else
    DecodedCert  cert[1];
#endif

    WOLFSSL_ENTER("wolfSSL_get_chain_X509");
    if (chain != NULL) {
    #ifdef WOLFSSL_SMALL_STACK
        cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), NULL,
                                                       DYNAMIC_TYPE_DCERT);
        if (cert != NULL)
    #endif
        {
            InitDecodedCert(cert, chain->certs[idx].buffer,
                                  chain->certs[idx].length, NULL);

            if ((ret = ParseCertRelative(cert, CERT_TYPE, 0, NULL)) != 0) {
                WOLFSSL_MSG("Failed to parse cert");
            }
            else {
                x509 = (WOLFSSL_X509*)XMALLOC(sizeof(WOLFSSL_X509), NULL,
                                                             DYNAMIC_TYPE_X509);
                if (x509 == NULL) {
                    WOLFSSL_MSG("Failed alloc X509");
                }
                else {
                    InitX509(x509, 1, NULL);

                    if ((ret = CopyDecodedToX509(x509, cert)) != 0) {
                        WOLFSSL_MSG("Failed to copy decoded");
                        XFREE(x509, NULL, DYNAMIC_TYPE_X509);
                        x509 = NULL;
                    }
                }
            }

            FreeDecodedCert(cert);
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
        #endif
        }
    }
    (void)ret;

    return x509;
}


/* Get peer's PEM certificate at index (idx), output to buffer if inLen big
   enough else return error (-1). If buffer is NULL only calculate
   outLen. Output length is in *outLen WOLFSSL_SUCCESS on ok */
int  wolfSSL_get_chain_cert_pem(WOLFSSL_X509_CHAIN* chain, int idx,
                               unsigned char* buf, int inLen, int* outLen)
{
#if defined(WOLFSSL_PEM_TO_DER) || defined(WOLFSSL_DER_TO_PEM)
    const char* header = NULL;
    const char* footer = NULL;
    int headerLen;
    int footerLen;
    int i;
    int err;
    word32 szNeeded = 0;

    WOLFSSL_ENTER("wolfSSL_get_chain_cert_pem");
    if (!chain || !outLen || idx < 0 || idx >= wolfSSL_get_chain_count(chain))
        return BAD_FUNC_ARG;

    err = wc_PemGetHeaderFooter(CERT_TYPE, &header, &footer);
    if (err != 0)
        return err;

    headerLen = (int)XSTRLEN(header);
    footerLen = (int)XSTRLEN(footer);

    /* Null output buffer return size needed in outLen */
    if(!buf) {
        if(Base64_Encode(chain->certs[idx].buffer, chain->certs[idx].length,
                    NULL, &szNeeded) != LENGTH_ONLY_E)
            return WOLFSSL_FAILURE;
        *outLen = szNeeded + headerLen + footerLen;
        return LENGTH_ONLY_E;
    }

    /* don't even try if inLen too short */
    if (inLen < headerLen + footerLen + chain->certs[idx].length)
        return BAD_FUNC_ARG;

    /* header */
    if (XMEMCPY(buf, header, headerLen) == NULL)
        return WOLFSSL_FATAL_ERROR;

    i = headerLen;

    /* body */
    *outLen = inLen;  /* input to Base64_Encode */
    if ( (err = Base64_Encode(chain->certs[idx].buffer,
                       chain->certs[idx].length, buf + i, (word32*)outLen)) < 0)
        return err;
    i += *outLen;

    /* footer */
    if ( (i + footerLen) > inLen)
        return BAD_FUNC_ARG;
    if (XMEMCPY(buf + i, footer, footerLen) == NULL)
        return WOLFSSL_FATAL_ERROR;
    *outLen += headerLen + footerLen;

    return WOLFSSL_SUCCESS;
#else
    (void)chain;
    (void)idx;
    (void)buf;
    (void)inLen;
    (void)outLen;
    return WOLFSSL_FAILURE;
#endif /* WOLFSSL_PEM_TO_DER || WOLFSSL_DER_TO_PEM */
}


/* get session ID */
const byte* wolfSSL_get_sessionID(const WOLFSSL_SESSION* session)
{
    WOLFSSL_ENTER("wolfSSL_get_sessionID");
    if (session)
        return session->sessionID;

    return NULL;
}


#endif /* SESSION_CERTS */

#ifdef HAVE_FUZZER
void wolfSSL_SetFuzzerCb(WOLFSSL* ssl, CallbackFuzzer cbf, void* fCtx)
{
    if (ssl) {
        ssl->fuzzerCb  = cbf;
        ssl->fuzzerCtx = fCtx;
    }
}
#endif

#ifndef NO_CERTS
#ifdef  HAVE_PK_CALLBACKS

#ifdef HAVE_ECC

void  wolfSSL_CTX_SetEccSignCb(WOLFSSL_CTX* ctx, CallbackEccSign cb)
{
    if (ctx)
        ctx->EccSignCb = cb;
}


void  wolfSSL_SetEccSignCtx(WOLFSSL* ssl, void *ctx)
{
    if (ssl)
        ssl->EccSignCtx = ctx;
}


void* wolfSSL_GetEccSignCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->EccSignCtx;

    return NULL;
}


void  wolfSSL_CTX_SetEccVerifyCb(WOLFSSL_CTX* ctx, CallbackEccVerify cb)
{
    if (ctx)
        ctx->EccVerifyCb = cb;
}


void  wolfSSL_SetEccVerifyCtx(WOLFSSL* ssl, void *ctx)
{
    if (ssl)
        ssl->EccVerifyCtx = ctx;
}


void* wolfSSL_GetEccVerifyCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->EccVerifyCtx;

    return NULL;
}

void wolfSSL_CTX_SetEccSharedSecretCb(WOLFSSL_CTX* ctx, CallbackEccSharedSecret cb)
{
    if (ctx)
        ctx->EccSharedSecretCb = cb;
}

void  wolfSSL_SetEccSharedSecretCtx(WOLFSSL* ssl, void *ctx)
{
    if (ssl)
        ssl->EccSharedSecretCtx = ctx;
}


void* wolfSSL_GetEccSharedSecretCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->EccSharedSecretCtx;

    return NULL;
}
#endif /* HAVE_ECC */

#ifdef HAVE_ED25519
void  wolfSSL_CTX_SetEd25519SignCb(WOLFSSL_CTX* ctx, CallbackEd25519Sign cb)
{
    if (ctx)
        ctx->Ed25519SignCb = cb;
}


void  wolfSSL_SetEd25519SignCtx(WOLFSSL* ssl, void *ctx)
{
    if (ssl)
        ssl->Ed25519SignCtx = ctx;
}


void* wolfSSL_GetEd25519SignCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->Ed25519SignCtx;

    return NULL;
}


void  wolfSSL_CTX_SetEd25519VerifyCb(WOLFSSL_CTX* ctx, CallbackEd25519Verify cb)
{
    if (ctx)
        ctx->Ed25519VerifyCb = cb;
}


void  wolfSSL_SetEd25519VerifyCtx(WOLFSSL* ssl, void *ctx)
{
    if (ssl)
        ssl->Ed25519VerifyCtx = ctx;
}


void* wolfSSL_GetEd25519VerifyCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->Ed25519VerifyCtx;

    return NULL;
}
#endif

#ifdef HAVE_CURVE25519
void wolfSSL_CTX_SetX25519SharedSecretCb(WOLFSSL_CTX* ctx,
        CallbackX25519SharedSecret cb)
{
    if (ctx)
        ctx->X25519SharedSecretCb = cb;
}

void  wolfSSL_SetX25519SharedSecretCtx(WOLFSSL* ssl, void *ctx)
{
    if (ssl)
        ssl->X25519SharedSecretCtx = ctx;
}


void* wolfSSL_GetX25519SharedSecretCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->X25519SharedSecretCtx;

    return NULL;
}
#endif

#ifndef NO_RSA

void  wolfSSL_CTX_SetRsaSignCb(WOLFSSL_CTX* ctx, CallbackRsaSign cb)
{
    if (ctx)
        ctx->RsaSignCb = cb;
}


void  wolfSSL_SetRsaSignCtx(WOLFSSL* ssl, void *ctx)
{
    if (ssl)
        ssl->RsaSignCtx = ctx;
}


void* wolfSSL_GetRsaSignCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->RsaSignCtx;

    return NULL;
}


void  wolfSSL_CTX_SetRsaVerifyCb(WOLFSSL_CTX* ctx, CallbackRsaVerify cb)
{
    if (ctx)
        ctx->RsaVerifyCb = cb;
}

void  wolfSSL_CTX_SetRsaSignCheckCb(WOLFSSL_CTX* ctx, CallbackRsaVerify cb)
{
    if (ctx)
        ctx->RsaSignCheckCb = cb;
}


void  wolfSSL_SetRsaVerifyCtx(WOLFSSL* ssl, void *ctx)
{
    if (ssl)
        ssl->RsaVerifyCtx = ctx;
}


void* wolfSSL_GetRsaVerifyCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->RsaVerifyCtx;

    return NULL;
}

#ifdef WC_RSA_PSS
void  wolfSSL_CTX_SetRsaPssSignCb(WOLFSSL_CTX* ctx, CallbackRsaPssSign cb)
{
    if (ctx)
        ctx->RsaPssSignCb = cb;
}


void  wolfSSL_SetRsaPssSignCtx(WOLFSSL* ssl, void *ctx)
{
    if (ssl)
        ssl->RsaPssSignCtx = ctx;
}


void* wolfSSL_GetRsaPssSignCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->RsaPssSignCtx;

    return NULL;
}


void  wolfSSL_CTX_SetRsaPssVerifyCb(WOLFSSL_CTX* ctx, CallbackRsaPssVerify cb)
{
    if (ctx)
        ctx->RsaPssVerifyCb = cb;
}

void  wolfSSL_CTX_SetRsaPssSignCheckCb(WOLFSSL_CTX* ctx, CallbackRsaPssVerify cb)
{
    if (ctx)
        ctx->RsaPssSignCheckCb = cb;
}


void  wolfSSL_SetRsaPssVerifyCtx(WOLFSSL* ssl, void *ctx)
{
    if (ssl)
        ssl->RsaPssVerifyCtx = ctx;
}


void* wolfSSL_GetRsaPssVerifyCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->RsaPssVerifyCtx;

    return NULL;
}
#endif

void  wolfSSL_CTX_SetRsaEncCb(WOLFSSL_CTX* ctx, CallbackRsaEnc cb)
{
    if (ctx)
        ctx->RsaEncCb = cb;
}


void  wolfSSL_SetRsaEncCtx(WOLFSSL* ssl, void *ctx)
{
    if (ssl)
        ssl->RsaEncCtx = ctx;
}


void* wolfSSL_GetRsaEncCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->RsaEncCtx;

    return NULL;
}

void  wolfSSL_CTX_SetRsaDecCb(WOLFSSL_CTX* ctx, CallbackRsaDec cb)
{
    if (ctx)
        ctx->RsaDecCb = cb;
}


void  wolfSSL_SetRsaDecCtx(WOLFSSL* ssl, void *ctx)
{
    if (ssl)
        ssl->RsaDecCtx = ctx;
}


void* wolfSSL_GetRsaDecCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->RsaDecCtx;

    return NULL;
}


#endif /* NO_RSA */

#endif /* HAVE_PK_CALLBACKS */
#endif /* NO_CERTS */

#if defined(HAVE_PK_CALLBACKS) && !defined(NO_DH)

void wolfSSL_CTX_SetDhAgreeCb(WOLFSSL_CTX* ctx, CallbackDhAgree cb)
{
    if (ctx)
        ctx->DhAgreeCb = cb;
}

void wolfSSL_SetDhAgreeCtx(WOLFSSL* ssl, void *ctx)
{
    if (ssl)
        ssl->DhAgreeCtx = ctx;
}

void* wolfSSL_GetDhAgreeCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->DhAgreeCtx;

    return NULL;
}
#endif /* HAVE_PK_CALLBACKS && !NO_DH */


#ifdef WOLFSSL_HAVE_WOLFSCEP
    /* Used by autoconf to see if wolfSCEP is available */
    void wolfSSL_wolfSCEP(void) {}
#endif


#ifdef WOLFSSL_HAVE_CERT_SERVICE
    /* Used by autoconf to see if cert service is available */
    void wolfSSL_cert_service(void) {}
#endif


#ifdef OPENSSL_EXTRA /*Lighttp compatibility*/

    #ifndef NO_CERTS
    void wolfSSL_X509_NAME_free(WOLFSSL_X509_NAME *name){
        WOLFSSL_ENTER("wolfSSL_X509_NAME_free");
        FreeX509Name(name, NULL);
        XFREE(name, NULL, DYNAMIC_TYPE_X509);
    }


    /* Malloc's a new WOLFSSL_X509_NAME structure
     *
     * returns NULL on failure, otherwise returns a new structure.
     */
    WOLFSSL_X509_NAME* wolfSSL_X509_NAME_new()
    {
        WOLFSSL_X509_NAME* name;

        WOLFSSL_ENTER("wolfSSL_X509_NAME_new");

        name = (WOLFSSL_X509_NAME*)XMALLOC(sizeof(WOLFSSL_X509_NAME), NULL,
                DYNAMIC_TYPE_X509);
        if (name != NULL) {
            InitX509Name(name, 1);
        }
        return name;
    }


#if defined(WOLFSSL_CERT_GEN) && !defined(NO_RSA)
/* needed SetName function from asn.c is wrapped by NO_RSA */
    /* helper function for CopyX509NameToCertName()
     *
     * returns WOLFSSL_SUCCESS on success
     */
    static int CopyX509NameEntry(char* out, int mx, char* in, int inLen)
    {
        if (inLen > mx) {
            WOLFSSL_MSG("Name too long");
            XMEMCPY(out, in, mx);
        }
        else {
            XMEMCPY(out, in, inLen);
            out[inLen] = '\0';
        }

        /* make sure is null terminated */
        out[mx-1] = '\0';

        return WOLFSSL_SUCCESS;
    }


    /* Helper function to copy cert name from a WOLFSSL_X509_NAME structure to
     * a CertName structure.
     *
     * returns WOLFSSL_SUCCESS on success and a negative error value on failure
     */
    static int CopyX509NameToCertName(WOLFSSL_X509_NAME* n, CertName* cName)
    {
        DecodedName* dn = NULL;

        if (n == NULL || cName == NULL) {
            return BAD_FUNC_ARG;
        }

        dn = &(n->fullName);

        /* initialize cert name */
        cName->country[0] = '\0';
        cName->countryEnc = CTC_PRINTABLE;
        cName->state[0] = '\0';
        cName->stateEnc = CTC_UTF8;
        cName->locality[0] = '\0';
        cName->localityEnc = CTC_UTF8;
        cName->sur[0] = '\0';
        cName->surEnc = CTC_UTF8;
        cName->org[0] = '\0';
        cName->orgEnc = CTC_UTF8;
        cName->unit[0] = '\0';
        cName->unitEnc = CTC_UTF8;
        cName->commonName[0] = '\0';
        cName->commonNameEnc = CTC_UTF8;
        cName->email[0] = '\0';


        /* ASN_COUNTRY_NAME */
        WOLFSSL_MSG("Copy Country Name");
        if (CopyX509NameEntry(cName->country, CTC_NAME_SIZE, dn->fullName + dn->cIdx,
                    dn->cLen) != SSL_SUCCESS) {
            return BUFFER_E;
        }

        /* ASN_ORGUNIT_NAME */
        WOLFSSL_MSG("Copy Org Unit Name");
        if (CopyX509NameEntry(cName->unit, CTC_NAME_SIZE, dn->fullName + dn->ouIdx,
                    dn->ouLen) != SSL_SUCCESS) {
            return BUFFER_E;
        }

        /* ASN_ORG_NAME */
        WOLFSSL_MSG("Copy Org Name");
        if (CopyX509NameEntry(cName->org, CTC_NAME_SIZE, dn->fullName + dn->oIdx,
                    dn->oLen) != SSL_SUCCESS) {
            return BUFFER_E;
        }

        /* ASN_STATE_NAME */
        WOLFSSL_MSG("Copy State Name");
        if (CopyX509NameEntry(cName->state, CTC_NAME_SIZE, dn->fullName + dn->stIdx,
                    dn->stLen) != SSL_SUCCESS) {
            return BUFFER_E;
        }

        /* ASN_LOCALITY_NAME */
        WOLFSSL_MSG("Copy Locality Name");
        if (CopyX509NameEntry(cName->locality, CTC_NAME_SIZE,
                    dn->fullName + dn->lIdx, dn->lLen)
                    != SSL_SUCCESS) {
            return BUFFER_E;
        }

        /* ASN_SUR_NAME */
        WOLFSSL_MSG("Copy Sur Name");
        if (CopyX509NameEntry(cName->sur, CTC_NAME_SIZE, dn->fullName + dn->snIdx,
                    dn->snLen) != SSL_SUCCESS) {
            return BUFFER_E;
        }

        /* ASN_COMMON_NAME */
        WOLFSSL_MSG("Copy Common Name");
        if (CopyX509NameEntry(cName->commonName, CTC_NAME_SIZE,
                    dn->fullName + dn->cnIdx, dn->cnLen)
                    != SSL_SUCCESS) {
            return BUFFER_E;
        }

        WOLFSSL_MSG("Copy Email");
        if (CopyX509NameEntry(cName->email, CTC_NAME_SIZE,
                    dn->fullName + dn->emailIdx, dn->emailLen)
                    != SSL_SUCCESS) {
            return BUFFER_E;
        }

        return WOLFSSL_SUCCESS;
    }


    /* Converts the x509 name structure into DER format.
     *
     * out  pointer to either a pre setup buffer or a pointer to null for
     *      creating a dynamic buffer. In the case that a pre-existing buffer is
     *      used out will be incremented the size of the DER buffer on success.
     *
     * returns the size of the buffer on success, or negative value with failure
     */
    int wolfSSL_i2d_X509_NAME(WOLFSSL_X509_NAME* name, unsigned char** out)
    {
        CertName cName;
        unsigned char buf[256]; //ASN_MAX_NAME
        int sz;

        if (out == NULL || name == NULL) {
            return BAD_FUNC_ARG;
        }

        if (CopyX509NameToCertName(name, &cName) != SSL_SUCCESS) {
            WOLFSSL_MSG("Error converting x509 name to internal CertName");
            return SSL_FATAL_ERROR;
        }

        sz = SetName(buf, sizeof(buf), &cName);
        if (sz < 0) {
            return sz;
        }

        /* using buffer passed in */
        if (*out != NULL) {
            XMEMCPY(*out, buf, sz);
            *out += sz;
        }
        else {
            *out = (unsigned char*)XMALLOC(sz, NULL, DYNAMIC_TYPE_OPENSSL);
            if (*out == NULL) {
                return MEMORY_E;
            }
            XMEMCPY(*out, buf, sz);
        }

        return sz;
    }
#endif /* WOLFSSL_CERT_GEN */


    /* Compares the two X509 names. If the size of x is larger then y then a
     * positive value is returned if x is smaller a negative value is returned.
     * In the case that the sizes are equal a the value of memcmp between the
     * two names is returned.
     *
     * x First name for comparision
     * y Second name to compare with x
     */
    int wolfSSL_X509_NAME_cmp(const WOLFSSL_X509_NAME* x,
            const WOLFSSL_X509_NAME* y)
    {
        WOLFSSL_STUB("wolfSSL_X509_NAME_cmp");

        if (x == NULL || y == NULL) {
            WOLFSSL_MSG("Bad argument passed in");
            return -2;
        }

        if ((x->sz - y->sz) != 0) {
            return x->sz - y->sz;
        }
        else {
            return XMEMCMP(x->name, y->name, x->sz); /* y sz is the same */
        }
    }


    WOLFSSL_X509 *wolfSSL_PEM_read_bio_X509(WOLFSSL_BIO *bp, WOLFSSL_X509 **x,
                                                 pem_password_cb *cb, void *u)
    {
        WOLFSSL_X509* x509 = NULL;
#if defined(WOLFSSL_PEM_TO_DER) || defined(WOLFSSL_DER_TO_PEM)
        unsigned char* pem = NULL;
        int pemSz;
        long  i = 0, l;
        const char* footer = NULL;

        WOLFSSL_ENTER("wolfSSL_PEM_read_bio_X509");

        if (bp == NULL) {
            WOLFSSL_LEAVE("wolfSSL_PEM_read_bio_X509", BAD_FUNC_ARG);
            return NULL;
        }

        if (bp->type == WOLFSSL_BIO_MEMORY) {
            l = (long)wolfSSL_BIO_ctrl_pending(bp);
            if (l <= 0) {
                WOLFSSL_MSG("No pending data in WOLFSSL_BIO");
                return NULL;
            }
        }
        else if (bp->type == WOLFSSL_BIO_FILE) {
#ifndef NO_FILESYSTEM
            /* Read in next certificate from file but no more. */
            i = XFTELL(bp->file);
            if (i < 0)
                return NULL;
            if (XFSEEK(bp->file, 0, SEEK_END) != 0)
                return NULL;
            l = XFTELL(bp->file);
            if (l < 0)
                return NULL;
            if (XFSEEK(bp->file, i, SEEK_SET) != 0)
                return NULL;
#else
            WOLFSSL_MSG("Unable to read file with NO_FILESYSTEM defined");
            return NULL;
#endif /* !NO_FILESYSTEM */
        }
        else
            return NULL;

        /* check calculated length */
        if (l - i < 0)
            return NULL;
        pem = (unsigned char*)XMALLOC(l - i, 0, DYNAMIC_TYPE_PEM);
        if (pem == NULL)
            return NULL;

        i = 0;
        if (wc_PemGetHeaderFooter(CERT_TYPE, NULL, &footer) != 0) {
            XFREE(pem, 0, DYNAMIC_TYPE_PEM);
            return NULL;
        }

        /* TODO: Inefficient
         * reading in one byte at a time until see "END CERTIFICATE"
         */
        while ((l = wolfSSL_BIO_read(bp, (char *)&pem[i], 1)) == 1) {
            i++;
            if (i > 26 && XMEMCMP((char *)&pem[i-26], footer, 25) == 0) {
                if (pem[i-1] == '\r') {
                    /* found \r , Windows line ending is \r\n so try to read one
                     * more byte for \n, ignoring return value */
                    (void)wolfSSL_BIO_read(bp, (char *)&pem[i++], 1);
                }
                break;
            }
        }
    #if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX)
        if (l == 0)
            WOLFSSL_ERROR(ASN_NO_PEM_HEADER);
    #endif
        pemSz = (int)i;
        x509 = wolfSSL_X509_load_certificate_buffer(pem, pemSz,
                                                              WOLFSSL_FILETYPE_PEM);

        if (x != NULL) {
            *x = x509;
        }

        XFREE(pem, NULL, DYNAMIC_TYPE_PEM);

#endif /* WOLFSSL_PEM_TO_DER || WOLFSSL_DER_TO_PEM */
        (void)bp;
        (void)x;
        (void)cb;
        (void)u;

        return x509;
    }

#if defined(HAVE_CRL) && !defined(NO_FILESYSTEM)
    WOLFSSL_API WOLFSSL_X509_CRL* wolfSSL_PEM_read_X509_CRL(XFILE fp, WOLFSSL_X509_CRL **crl,
                                                    pem_password_cb *cb, void *u)
    {
#if defined(WOLFSSL_PEM_TO_DER) || defined(WOLFSSL_DER_TO_PEM)
        unsigned char* pem = NULL;
        DerBuffer* der = NULL;
        int pemSz;
        int derSz;
        long  i = 0, l;
        WOLFSSL_X509_CRL* newcrl;
        
        WOLFSSL_ENTER("wolfSSL_PEM_read_X509_CRL");

        if (fp == NULL) {
            WOLFSSL_LEAVE("wolfSSL_PEM_read_X509_CRL", BAD_FUNC_ARG);
            return NULL;
        }
        /* Read in CRL from file */
        i = XFTELL(fp);
        if (i < 0) {
            WOLFSSL_LEAVE("wolfSSL_PEM_read_X509_CRL", BAD_FUNC_ARG);
            return NULL;
        }

        if (XFSEEK(fp, 0, SEEK_END) != 0)
            return NULL;
        l = XFTELL(fp);
        if (l < 0)
            return NULL;
        if (XFSEEK(fp, i, SEEK_SET) != 0)
            return NULL;
        pemSz = (int)(l - i);
        /* check calculated length */
        if (pemSz  < 0)
            return NULL;
        if((pem = (unsigned char*)XMALLOC(pemSz, 0, DYNAMIC_TYPE_PEM)) == NULL)
            return NULL;

        if((int)XFREAD((char *)pem, 1, pemSz, fp) != pemSz)
            goto err_exit;
        if((PemToDer(pem, pemSz, CRL_TYPE, &der, NULL, NULL, NULL)) < 0)
            goto err_exit;
        XFREE(pem, 0, DYNAMIC_TYPE_PEM);

        derSz = der->length;
        if((newcrl = wolfSSL_d2i_X509_CRL(crl, (const unsigned char *)der->buffer, derSz)) == NULL)
            goto err_exit;
        FreeDer(&der);

        return newcrl;

    err_exit:
        if(pem != NULL)
            XFREE(pem, 0, DYNAMIC_TYPE_PEM);
        if(der != NULL)
            FreeDer(&der);
        return NULL;

        (void)cb;
        (void)u;
    #endif

    }
#endif

    /*
     * bp : bio to read X509 from
     * x  : x509 to write to
     * cb : password call back for reading PEM
     * u  : password
     * _AUX is for working with a trusted X509 certificate
     */
    WOLFSSL_X509 *wolfSSL_PEM_read_bio_X509_AUX(WOLFSSL_BIO *bp,
                               WOLFSSL_X509 **x, pem_password_cb *cb, void *u) {
        WOLFSSL_ENTER("wolfSSL_PEM_read_bio_X509");

        /* AUX info is; trusted/rejected uses, friendly name, private key id,
         * and potentially a stack of "other" info. wolfSSL does not store
         * friendly name or private key id yet in WOLFSSL_X509 for human
         * readibility and does not support extra trusted/rejected uses for
         * root CA. */
        return wolfSSL_PEM_read_bio_X509(bp, x, cb, u);
    }

    void wolfSSL_X509_NAME_ENTRY_free(WOLFSSL_X509_NAME_ENTRY* ne)
    {
        if (ne != NULL) {
            if (ne->value != NULL && ne->value != &(ne->data)) {
                wolfSSL_ASN1_STRING_free(ne->value);
            }
            XFREE(ne, NULL, DYNAMIC_TYPE_NAME_ENTRY);
        }
    }


    WOLFSSL_X509_NAME_ENTRY* wolfSSL_X509_NAME_ENTRY_new(void)
    {
        WOLFSSL_X509_NAME_ENTRY* ne = NULL;

        ne = (WOLFSSL_X509_NAME_ENTRY*)XMALLOC(sizeof(WOLFSSL_X509_NAME_ENTRY),
                NULL, DYNAMIC_TYPE_NAME_ENTRY);
        if (ne != NULL) {
            XMEMSET(ne, 0, sizeof(WOLFSSL_X509_NAME_ENTRY));
            ne->value = &(ne->data);
        }

        return ne;
    }


    WOLFSSL_X509_NAME_ENTRY* wolfSSL_X509_NAME_ENTRY_create_by_NID(
            WOLFSSL_X509_NAME_ENTRY** out, int nid, int type,
            unsigned char* data, int dataSz)
    {
        WOLFSSL_X509_NAME_ENTRY* ne = NULL;

        WOLFSSL_ENTER("wolfSSL_X509_NAME_ENTRY_create_by_NID()");

        ne = wolfSSL_X509_NAME_ENTRY_new();
        if (ne == NULL) {
            return NULL;
        }

        ne->nid = nid;
        ne->value = wolfSSL_ASN1_STRING_type_new(type);
        wolfSSL_ASN1_STRING_set(ne->value, (const void*)data, dataSz);
        ne->set = 1;

        if (out != NULL) {
            *out = ne;
        }

        return ne;
    }


    /* Copies entry into name. With it being copied freeing entry becomes the
     * callers responsibility.
     * returns 1 for success and 0 for error */
    int wolfSSL_X509_NAME_add_entry(WOLFSSL_X509_NAME* name,
            WOLFSSL_X509_NAME_ENTRY* entry, int idx, int set)
    {
        int i;

        WOLFSSL_ENTER("wolfSSL_X509_NAME_add_entry()");

        for (i = 0; i < MAX_NAME_ENTRIES; i++) {
            if (name->extra[i].set != 1) { /* not set so overwrited */
                WOLFSSL_X509_NAME_ENTRY* current = &(name->extra[i]);
                WOLFSSL_ASN1_STRING*     str;

                WOLFSSL_MSG("Found place for name entry");

                XMEMCPY(current, entry, sizeof(WOLFSSL_X509_NAME_ENTRY));
                str = entry->value;
                XMEMCPY(&(current->data), str, sizeof(WOLFSSL_ASN1_STRING));
                current->value = &(current->data);
                current->data.data = (char*)XMALLOC(str->length,
                       name->x509->heap, DYNAMIC_TYPE_OPENSSL);

                if (current->data.data == NULL) {
                    return SSL_FAILURE;
                }
                XMEMCPY(current->data.data, str->data, str->length);

                /* make sure is null terminated */
                current->data.data[str->length - 1] = '\0';

                current->set = 1; /* make sure now listed as set */
                break;
            }
        }

        if (i == MAX_NAME_ENTRIES) {
            WOLFSSL_MSG("No spot found for name entry");
            return SSL_FAILURE;
        }

        (void)idx;
        (void)set;
        return SSL_SUCCESS;
    }
    #endif /* ifndef NO_CERTS */


    /* NID variables are dependent on compatibility header files currently
     *
     * returns a pointer to a new WOLFSSL_ASN1_OBJECT struct on success and NULL
     *         on fail
     */
    WOLFSSL_ASN1_OBJECT* wolfSSL_OBJ_nid2obj(int id)
    {
        word32 oidSz = 0;
        const byte* oid;
        word32 type = 0;
        WOLFSSL_ASN1_OBJECT* obj;
        byte objBuf[MAX_OID_SZ + MAX_LENGTH_SZ + 1]; /* +1 for object tag */
        word32 objSz = 0;
        const char* sName;

        WOLFSSL_ENTER("wolfSSL_OBJ_nid2obj()");

        /* get OID type */
        switch (id) {
            /* oidHashType */
        #ifdef WOLFSSL_MD2
            case NID_md2:
                id = MD2h;
                type = oidHashType;
                sName = "md2";
                break;
        #endif
        #ifndef NO_MD5
            case NID_md5:
                id = MD5h;
                type = oidHashType;
                sName = "md5";
                break;
        #endif
        #ifndef NO_SHA
            case NID_sha1:
                id = SHAh;
                type = oidHashType;
                sName = "sha";
                break;
        #endif
            case NID_sha224:
                id = SHA224h;
                type = oidHashType;
                sName = "sha224";
                break;
        #ifndef NO_SHA256
            case NID_sha256:
                id = SHA256h;
                type = oidHashType;
                sName = "sha256";
                break;
        #endif
        #ifdef WOLFSSL_SHA384
            case NID_sha384:
                id = SHA384h;
                type = oidHashType;
                sName = "sha384";
                break;
        #endif
        #ifdef WOLFSSL_SHA512
            case NID_sha512:
                id = SHA512h;
                type = oidHashType;
                sName = "sha512";
                break;
        #endif

            /*  oidSigType */
        #ifndef NO_DSA
            case CTC_SHAwDSA:
                sName = "shaWithDSA";
                type = oidSigType;
                break;

        #endif /* NO_DSA */
        #ifndef NO_RSA
            case CTC_MD2wRSA:
                sName = "md2WithRSA";
                type = oidSigType;
                break;

        #ifndef NO_MD5
            case CTC_MD5wRSA:
                sName = "md5WithRSA";
                type = oidSigType;
                break;
        #endif

            case CTC_SHAwRSA:
                sName = "shaWithRSA";
                type = oidSigType;
                break;

        #ifdef WOLFSSL_SHA224
            case CTC_SHA224wRSA:
                sName = "sha224WithRSA";
                type = oidSigType;
                break;
        #endif

        #ifndef NO_SHA256
            case CTC_SHA256wRSA:
                sName = "sha256WithRSA";
                type = oidSigType;
                break;
        #endif

        #ifdef WOLFSSL_SHA384
            case CTC_SHA384wRSA:
                sName = "sha384WithRSA";
                type = oidSigType;
                break;
        #endif

        #ifdef WOLFSSL_SHA512
            case CTC_SHA512wRSA:
                sName = "sha512WithRSA";
                type = oidSigType;
                break;
        #endif
        #endif /* NO_RSA */
        #ifdef HAVE_ECC
            case CTC_SHAwECDSA:
                sName = "shaWithECDSA";
                type = oidSigType;
                break;

            case CTC_SHA224wECDSA:
                sName = "sha224WithECDSA";
                type = oidSigType;
                break;

            case CTC_SHA256wECDSA:
                sName = "sha256WithECDSA";
                type = oidSigType;
                break;

            case CTC_SHA384wECDSA:
                sName = "sha384WithECDSA";
                type = oidSigType;
                break;

            case CTC_SHA512wECDSA:
                sName = "sha512WithECDSA";
                type = oidSigType;
                break;
        #endif /* HAVE_ECC */

            /* oidKeyType */
        #ifndef NO_DSA
            case DSAk:
                sName = "DSA key";
                type = oidKeyType;
                break;
        #endif /* NO_DSA */
        #ifndef NO_RSA
            case RSAk:
                sName = "RSA key";
                type = oidKeyType;
                break;
        #endif /* NO_RSA */
        #ifdef HAVE_NTRU
            case NTRUk:
                sName = "NTRU key";
                type = oidKeyType;
                break;
        #endif /* HAVE_NTRU */
        #ifdef HAVE_ECC
            case ECDSAk:
                sName = "ECDSA key";
                type = oidKeyType;
                break;
        #endif /* HAVE_ECC */

            /* oidBlkType */
        #ifdef WOLFSSL_AES_128
            case AES128CBCb:
                sName = "AES-128-CBC";
                type = oidBlkType;
                break;
        #endif
        #ifdef WOLFSSL_AES_192
            case AES192CBCb:
                sName = "AES-192-CBC";
                type = oidBlkType;
                break;
        #endif

        #ifdef WOLFSSL_AES_256
            case AES256CBCb:
                sName = "AES-256-CBC";
                type = oidBlkType;
                break;
        #endif

        #ifndef NO_DES3
            case NID_des:
                id = DESb;
                sName = "DES-CBC";
                type = oidBlkType;
                break;

            case NID_des3:
                id = DES3b;
                sName = "DES3-CBC";
                type = oidBlkType;
                break;
        #endif /* !NO_DES3 */

        #ifdef HAVE_OCSP
            case NID_id_pkix_OCSP_basic:
                id = OCSP_BASIC_OID;
                sName = "OCSP_basic";
                type = oidOcspType;
                break;

            case OCSP_NONCE_OID:
                sName = "OCSP_nonce";
                type = oidOcspType;
                break;
        #endif /* HAVE_OCSP */

            /* oidCertExtType */
            case BASIC_CA_OID:
                sName = "X509 basic ca";
                type = oidCertExtType;
                break;

            case ALT_NAMES_OID:
                sName = "X509 alt names";
                type = oidCertExtType;
                break;

            case CRL_DIST_OID:
                sName = "X509 crl";
                type = oidCertExtType;
                break;

            case AUTH_INFO_OID:
                sName = "X509 auth info";
                type = oidCertExtType;
                break;

            case AUTH_KEY_OID:
                sName = "X509 auth key";
                type = oidCertExtType;
                break;

            case SUBJ_KEY_OID:
                sName = "X509 subject key";
                type = oidCertExtType;
                break;

            case KEY_USAGE_OID:
                sName = "X509 key usage";
                type = oidCertExtType;
                break;

            case INHIBIT_ANY_OID:
                id = INHIBIT_ANY_OID;
                sName = "X509 inhibit any";
                type = oidCertExtType;
                break;

            case NID_ext_key_usage:
                id = KEY_USAGE_OID;
                sName = "X509 ext key usage";
                type = oidCertExtType;
                break;

            case NID_name_constraints:
                id = NAME_CONS_OID;
                sName = "X509 name constraints";
                type = oidCertExtType;
                break;

            case NID_certificate_policies:
                id = CERT_POLICY_OID;
                sName = "X509 certificate policies";
                type = oidCertExtType;
                break;

            /* oidCertAuthInfoType */
            case AIA_OCSP_OID:
                sName = "Cert Auth OCSP";
                type = oidCertAuthInfoType;
                break;

            case AIA_CA_ISSUER_OID:
                sName = "Cert Auth CA Issuer";
                type = oidCertAuthInfoType;
                break;

            /* oidCertPolicyType */
            case NID_any_policy:
                id = CP_ANY_OID;
                sName = "Cert any policy";
                type = oidCertPolicyType;
                break;

                /* oidCertAltNameType */
            case NID_hw_name_oid:
                id = HW_NAME_OID;
                sName = "Hardware name";
                type = oidCertAltNameType;
                break;

            /* oidCertKeyUseType */
            case NID_anyExtendedKeyUsage:
                id = EKU_ANY_OID;
                sName = "Cert any extended key";
                type = oidCertKeyUseType;
                break;

            case EKU_SERVER_AUTH_OID:
                sName = "Cert server auth key";
                type = oidCertKeyUseType;
                break;

            case EKU_CLIENT_AUTH_OID:
                sName = "Cert client auth key";
                type = oidCertKeyUseType;
                break;

            case EKU_OCSP_SIGN_OID:
                sName = "Cert OCSP sign key";
                type = oidCertKeyUseType;
                break;

            /* oidKdfType */
            case PBKDF2_OID:
                sName = "PBKDFv2";
                type = oidKdfType;
                break;

                /* oidPBEType */
            case PBE_SHA1_RC4_128:
                sName = "PBE shaWithRC4-128";
                type = oidPBEType;
                break;

            case PBE_SHA1_DES:
                sName = "PBE shaWithDES";
                type = oidPBEType;
                break;

            case PBE_SHA1_DES3:
                sName = "PBE shaWithDES3";
                type = oidPBEType;
                break;

                /* oidKeyWrapType */
        #ifdef WOLFSSL_AES_128
            case AES128_WRAP:
                sName = "AES-128 wrap";
                type = oidKeyWrapType;
                break;
        #endif

        #ifdef WOLFSSL_AES_192
            case AES192_WRAP:
                sName = "AES-192 wrap";
                type = oidKeyWrapType;
                break;
        #endif

        #ifdef WOLFSSL_AES_256
            case AES256_WRAP:
                sName = "AES-256 wrap";
                type = oidKeyWrapType;
                break;
        #endif

                /* oidCmsKeyAgreeType */
        #ifndef NO_SHA
            case dhSinglePass_stdDH_sha1kdf_scheme:
                sName = "DH-SHA kdf";
                type = oidCmsKeyAgreeType;
                break;
        #endif
        #ifdef WOLFSSL_SHA224
            case dhSinglePass_stdDH_sha224kdf_scheme:
                sName = "DH-SHA224 kdf";
                type = oidCmsKeyAgreeType;
                break;
        #endif
        #ifndef NO_SHA256
            case dhSinglePass_stdDH_sha256kdf_scheme:
                sName = "DH-SHA256 kdf";
                type = oidCmsKeyAgreeType;
                break;

        #endif
        #ifdef WOLFSSL_SHA384
            case dhSinglePass_stdDH_sha384kdf_scheme:
                sName = "DH-SHA384 kdf";
                type = oidCmsKeyAgreeType;
                break;
        #endif
        #ifdef WOLFSSL_SHA512
            case dhSinglePass_stdDH_sha512kdf_scheme:
                sName = "DH-SHA512 kdf";
                type = oidCmsKeyAgreeType;
                break;
        #endif
            default:
                WOLFSSL_MSG("NID not in table");
                return NULL;
        }

    #ifdef HAVE_ECC
         if (type == 0 && wc_ecc_get_oid(id, &oid, &oidSz) > 0) {
             type = oidCurveType;
         }
    #endif /* HAVE_ECC */

        if (XSTRLEN(sName) > WOLFSSL_MAX_SNAME - 1) {
            WOLFSSL_MSG("Attempted short name is too large");
            return NULL;
        }

        oid = OidFromId(id, type, &oidSz);

        /* set object ID to buffer */
        obj = wolfSSL_ASN1_OBJECT_new();
        if (obj == NULL) {
            WOLFSSL_MSG("Issue creating WOLFSSL_ASN1_OBJECT struct");
            return NULL;
        }
        obj->type    = id;
        obj->grp     = type;
        obj->dynamic = 1;
        XMEMCPY(obj->sName, (char*)sName, XSTRLEN((char*)sName));

        objBuf[0] = ASN_OBJECT_ID; objSz++;
        objSz += SetLength(oidSz, objBuf + 1);
        XMEMCPY(objBuf + objSz, oid, oidSz);
        objSz     += oidSz;
        obj->objSz = objSz;

        obj->obj = (byte*)XMALLOC(obj->objSz, NULL, DYNAMIC_TYPE_ASN1);
        if (obj->obj == NULL) {
            wolfSSL_ASN1_OBJECT_free(obj);
            return NULL;
        }
        XMEMCPY(obj->obj, objBuf, obj->objSz);

        (void)type;

        return obj;
    }


    /* if no_name is one than use numerical form otherwise can be short name.
     *
     * returns the buffer size on success
     */
    int wolfSSL_OBJ_obj2txt(char *buf, int bufLen, WOLFSSL_ASN1_OBJECT *a, int no_name)
    {
        int bufSz;

        WOLFSSL_ENTER("wolfSSL_OBJ_obj2txt()");

        if (buf == NULL || bufLen <= 1 || a == NULL) {
            WOLFSSL_MSG("Bad input argument");
            return WOLFSSL_FAILURE;
        }

        if (no_name == 1) {
            int    length;
            word32 idx = 0;

            if (a->obj[idx++] != ASN_OBJECT_ID) {
                WOLFSSL_MSG("Bad ASN1 Object");
                return WOLFSSL_FAILURE;
            }

            if (GetLength((const byte*)a->obj, &idx, &length,
                           a->objSz) < 0 || length < 0) {
                return ASN_PARSE_E;
            }

            if (bufLen < MAX_OID_STRING_SZ) {
                bufSz = bufLen - 1;
            }
            else {
                bufSz = MAX_OID_STRING_SZ;
            }

            if ((bufSz = DecodePolicyOID(buf, (word32)bufSz, a->obj + idx,
                        (word32)length)) <= 0) {
                WOLFSSL_MSG("Error decoding OID");
                return WOLFSSL_FAILURE;
            }

        }
        else { /* return short name */
            if (XSTRLEN(a->sName) + 1 < (word32)bufLen - 1) {
                bufSz = (int)XSTRLEN(a->sName);
            }
            else {
                bufSz = bufLen - 1;
            }
            XMEMCPY(buf, a->sName, bufSz);
        }

        buf[bufSz] = '\0';
        return bufSz;
    }

#if defined(OPENSSL_EXTRA) || defined(HAVE_LIGHTY) || \
    defined(WOLFSSL_MYSQL_COMPATIBLE) || defined(HAVE_STUNNEL) || \
    defined(WOLFSSL_NGINX) || defined(HAVE_POCO_LIB) || \
    defined(WOLFSSL_HAPROXY)

#ifndef NO_SHA
    /* One shot SHA1 hash of message.
     *
     * d  message to hash
     * n  size of d buffer
     * md buffer to hold digest. Should be SHA_DIGEST_SIZE.
     *
     * Note: if md is null then a static buffer of SHA_DIGEST_SIZE is used.
     *       When the static buffer is used this function is not thread safe.
     *
     * Returns a pointer to the message digest on success and NULL on failure.
     */
    unsigned char *wolfSSL_SHA1(const unsigned char *d, size_t n,
            unsigned char *md)
    {
        static byte dig[WC_SHA_DIGEST_SIZE];
        wc_Sha sha;

        WOLFSSL_ENTER("wolfSSL_SHA1");

        if (wc_InitSha_ex(&sha, NULL, 0) != 0) {
            WOLFSSL_MSG("SHA1 Init failed");
            return NULL;
        }

        if (wc_ShaUpdate(&sha, (const byte*)d, (word32)n) != 0) {
            WOLFSSL_MSG("SHA1 Update failed");
            return NULL;
        }

        if (wc_ShaFinal(&sha, dig) != 0) {
            WOLFSSL_MSG("SHA1 Final failed");
            return NULL;
        }

        wc_ShaFree(&sha);

        if (md != NULL) {
            XMEMCPY(md, dig, WC_SHA_DIGEST_SIZE);
            return md;
        }
        else {
            return (unsigned char*)dig;
        }
    }
#endif /* ! NO_SHA */

#ifndef NO_SHA256
    /* One shot SHA256 hash of message.
     *
     * d  message to hash
     * n  size of d buffer
     * md buffer to hold digest. Should be WC_SHA256_DIGEST_SIZE.
     *
     * Note: if md is null then a static buffer of WC_SHA256_DIGEST_SIZE is used.
     *       When the static buffer is used this function is not thread safe.
     *
     * Returns a pointer to the message digest on success and NULL on failure.
     */
    unsigned char *wolfSSL_SHA256(const unsigned char *d, size_t n,
            unsigned char *md)
    {
        static byte dig[WC_SHA256_DIGEST_SIZE];
        wc_Sha256 sha;

        WOLFSSL_ENTER("wolfSSL_SHA256");

        if (wc_InitSha256_ex(&sha, NULL, 0) != 0) {
            WOLFSSL_MSG("SHA256 Init failed");
            return NULL;
        }

        if (wc_Sha256Update(&sha, (const byte*)d, (word32)n) != 0) {
            WOLFSSL_MSG("SHA256 Update failed");
            return NULL;
        }

        if (wc_Sha256Final(&sha, dig) != 0) {
            WOLFSSL_MSG("SHA256 Final failed");
            return NULL;
        }

        wc_Sha256Free(&sha);

        if (md != NULL) {
            XMEMCPY(md, dig, WC_SHA256_DIGEST_SIZE);
            return md;
        }
        else {
            return (unsigned char*)dig;
        }
    }
#endif /* ! NO_SHA256 */

#if defined(WOLFSSL_SHA384) && defined(WOLFSSL_SHA512) 
     /* One shot SHA384 hash of message.
      *
      * d  message to hash
      * n  size of d buffer
      * md buffer to hold digest. Should be WC_SHA256_DIGEST_SIZE.
      *
      * Note: if md is null then a static buffer of WC_SHA256_DIGEST_SIZE is used.
      *       When the static buffer is used this function is not thread safe.
      *
      * Returns a pointer to the message digest on success and NULL on failure.
      */
     unsigned char *wolfSSL_SHA384(const unsigned char *d, size_t n,
             unsigned char *md)
     {
         static byte dig[WC_SHA384_DIGEST_SIZE];
         wc_Sha384 sha;

         WOLFSSL_ENTER("wolfSSL_SHA384");

         if (wc_InitSha384_ex(&sha, NULL, 0) != 0) {
             WOLFSSL_MSG("SHA384 Init failed");
             return NULL;
         }

         if (wc_Sha384Update(&sha, (const byte*)d, (word32)n) != 0) {
             WOLFSSL_MSG("SHA384 Update failed");
             return NULL;
         }

         if (wc_Sha384Final(&sha, dig) != 0) {
             WOLFSSL_MSG("SHA384 Final failed");
             return NULL;
         }

         wc_Sha384Free(&sha);

         if (md != NULL) {
             XMEMCPY(md, dig, WC_SHA384_DIGEST_SIZE);
             return md;
         }
         else {
             return (unsigned char*)dig;
         }
     }
#endif /* defined(WOLFSSL_SHA384) && defined(WOLFSSL_SHA512)  */


#if defined(WOLFSSL_SHA512) 
     /* One shot SHA512 hash of message.
      *
      * d  message to hash
      * n  size of d buffer
      * md buffer to hold digest. Should be WC_SHA256_DIGEST_SIZE.
      *
      * Note: if md is null then a static buffer of WC_SHA256_DIGEST_SIZE is used.
      *       When the static buffer is used this function is not thread safe.
      *
      * Returns a pointer to the message digest on success and NULL on failure.
      */
     unsigned char *wolfSSL_SHA512(const unsigned char *d, size_t n,
             unsigned char *md)
     {
         static byte dig[WC_SHA512_DIGEST_SIZE];
         wc_Sha512 sha;

         WOLFSSL_ENTER("wolfSSL_SHA512");

         if (wc_InitSha512_ex(&sha, NULL, 0) != 0) {
             WOLFSSL_MSG("SHA512 Init failed");
             return NULL;
         }

         if (wc_Sha512Update(&sha, (const byte*)d, (word32)n) != 0) {
             WOLFSSL_MSG("SHA512 Update failed");
             return NULL;
         }

         if (wc_Sha512Final(&sha, dig) != 0) {
             WOLFSSL_MSG("SHA512 Final failed");
             return NULL;
         }

         wc_Sha512Free(&sha);

         if (md != NULL) {
             XMEMCPY(md, dig, WC_SHA512_DIGEST_SIZE);
             return md;
         }
         else {
             return (unsigned char*)dig;
         }
     }
#endif /* defined(WOLFSSL_SHA512)  */

    char wolfSSL_CTX_use_certificate(WOLFSSL_CTX *ctx, WOLFSSL_X509 *x)
    {
        int ret;

        WOLFSSL_ENTER("wolfSSL_CTX_use_certificate");

        FreeDer(&ctx->certificate); /* Make sure previous is free'd */
        ret = AllocDer(&ctx->certificate, x->derCert->length, CERT_TYPE,
                       ctx->heap);
        if (ret != 0)
            return 0;

        XMEMCPY(ctx->certificate->buffer, x->derCert->buffer,
                x->derCert->length);
#ifdef KEEP_OUR_CERT
        if (ctx->ourCert != NULL && ctx->ownOurCert) {
            FreeX509(ctx->ourCert);
            XFREE(ctx->ourCert, ctx->heap, DYNAMIC_TYPE_X509);
        }
        ctx->ourCert = x;
        ctx->ownOurCert = 0;
#endif

        /* Update the available options with public keys. */
        switch (x->pubKeyOID) {
            case RSAk:
                ctx->haveRSA = 1;
                break;
        #ifdef HAVE_ED25519
            case ED25519k:
        #endif
            case ECDSAk:
                ctx->haveECC = 1;
            #ifdef HAVE_ECC
                ctx->pkCurveOID = x->pkCurveOID;
            #endif
                break;
        }

        return WOLFSSL_SUCCESS;
    }

    #ifndef NO_WOLFSSL_STUB
    int wolfSSL_BIO_read_filename(WOLFSSL_BIO *b, const char *name) {
    #ifndef NO_FILESYSTEM
        XFILE fp;

        WOLFSSL_ENTER("wolfSSL_BIO_new_file");

        if ((wolfSSL_BIO_get_fp(b, &fp) == WOLFSSL_SUCCESS) && (fp != NULL))
        {
            XFCLOSE(fp);
        }

        fp = XFOPEN(name, "r");
        if (fp == NULL)
            return WOLFSSL_BAD_FILE;

        if (wolfSSL_BIO_set_fp(b, fp, BIO_CLOSE) != WOLFSSL_SUCCESS) {
            XFCLOSE(fp);
            return WOLFSSL_BAD_FILE;
        }

        /* file is closed when bio is free'd */
        return WOLFSSL_SUCCESS;
    #else
        (void)name;
        (void)b;
        return WOLFSSL_NOT_IMPLEMENTED;
    #endif
    }
    #endif

#ifdef HAVE_ECC
    const char * wolfSSL_OBJ_nid2sn(int n) {
        int i;
        WOLFSSL_ENTER("wolfSSL_OBJ_nid2sn");

        /* find based on NID and return name */
        for (i = 0; i < ecc_sets[i].size; i++) {
            if (n == ecc_sets[i].id) {
                return ecc_sets[i].name;
            }
        }
        return NULL;
    }

    int wolfSSL_OBJ_sn2nid(const char *sn) {
        int i;
        WOLFSSL_ENTER("wolfSSL_OBJ_osn2nid");

        /* Nginx uses this OpenSSL string. */
        if (XSTRNCMP(sn, "prime256v1", 10) == 0)
            sn = "SECP256R1";
        if (XSTRNCMP(sn, "secp384r1", 10) == 0)
            sn = "SECP384R1";
        /* find based on name and return NID */
        for (i = 0; i < ecc_sets[i].size; i++) {
            if (XSTRNCMP(sn, ecc_sets[i].name, ECC_MAXNAME) == 0) {
                return ecc_sets[i].id;
            }
        }
        return -1;
    }
#endif /* HAVE_ECC */

    /* Gets the NID value that corresponds with the ASN1 object.
     *
     * o ASN1 object to get NID of
     *
     * Return NID on success and a negative value on failure
     */
    int wolfSSL_OBJ_obj2nid(const WOLFSSL_ASN1_OBJECT *o) {
        word32 oid = 0;
        word32 idx = 0;
        int id;

        WOLFSSL_ENTER("wolfSSL_OBJ_obj2nid");

        if (o == NULL) {
            return -1;
        }

        if ((id = GetObjectId(o->obj, &idx, &oid, o->grp, o->objSz)) < 0) {
            WOLFSSL_MSG("Issue getting OID of object");
            return -1;
        }

        /* get OID type */
        switch (o->grp) {
            /* oidHashType */
            case oidHashType:
                switch (oid) {
                #ifdef WOLFSSL_MD2
                    case MD2h:
                        return NID_md2;
                #endif
                #ifndef NO_MD5
                    case MD5h:
                        return NID_md5;
                #endif
                #ifndef NO_SHA
                    case SHAh:
                        return NID_sha1;
                #endif
                    case SHA224h:
                        return NID_sha224;
                #ifndef NO_SHA256
                    case SHA256h:
                        return NID_sha256;
                #endif
                #ifdef WOLFSSL_SHA384
                    case SHA384h:
                        return NID_sha384;
                #endif
                #ifdef WOLFSSL_SHA512
                    case SHA512h:
                        return NID_sha512;
                #endif
                }
                break;

            /*  oidSigType */
            case oidSigType:
                switch (oid) {
                #ifndef NO_DSA
                    case CTC_SHAwDSA:
                        return CTC_SHAwDSA;
                #endif /* NO_DSA */
                #ifndef NO_RSA
                    case CTC_MD2wRSA:
                        return CTC_MD2wRSA;
                    case CTC_MD5wRSA:
                        return CTC_MD5wRSA;
                    case CTC_SHAwRSA:
                        return CTC_SHAwRSA;
                    case CTC_SHA224wRSA:
                        return CTC_SHA224wRSA;
                    case CTC_SHA256wRSA:
                        return CTC_SHA256wRSA;
                    case CTC_SHA384wRSA:
                        return CTC_SHA384wRSA;
                    case CTC_SHA512wRSA:
                        return CTC_SHA512wRSA;
                #endif /* NO_RSA */
                #ifdef HAVE_ECC
                    case CTC_SHAwECDSA:
                        return CTC_SHAwECDSA;
                    case CTC_SHA224wECDSA:
                        return CTC_SHA224wECDSA;
                    case CTC_SHA256wECDSA:
                        return CTC_SHA256wECDSA;
                    case CTC_SHA384wECDSA:
                        return CTC_SHA384wECDSA;
                    case CTC_SHA512wECDSA:
                        return CTC_SHA512wECDSA;
                #endif /* HAVE_ECC */
                }
                break;

            /* oidKeyType */
            case oidKeyType:
                switch (oid) {
                #ifndef NO_DSA
                    case DSAk:
                        return DSAk;
                #endif /* NO_DSA */
                #ifndef NO_RSA
                    case RSAk:
                        return RSAk;
                #endif /* NO_RSA */
                #ifdef HAVE_NTRU
                    case NTRUk:
                        return NTRUk;
                #endif /* HAVE_NTRU */
                #ifdef HAVE_ECC
                    case ECDSAk:
                        return ECDSAk;
                #endif /* HAVE_ECC */
                }
                break;

            /* oidBlkType */
            case oidBlkType:
                switch (oid) {
                #ifdef WOLFSSL_AES_128
                    case AES128CBCb:
                        return AES128CBCb;
                #endif
                #ifdef WOLFSSL_AES_192
                    case AES192CBCb:
                        return AES192CBCb;
                #endif
                #ifdef WOLFSSL_AES_256
                    case AES256CBCb:
                        return AES256CBCb;
                #endif
                #ifndef NO_DES3
                    case DESb:
                        return NID_des;
                    case DES3b:
                        return NID_des3;
                #endif
                }
                break;

        #ifdef HAVE_OCSP
            case oidOcspType:
                switch (oid) {
                    case OCSP_BASIC_OID:
                        return NID_id_pkix_OCSP_basic;
                    case OCSP_NONCE_OID:
                        return OCSP_NONCE_OID;
                }
                break;
        #endif /* HAVE_OCSP */

            /* oidCertExtType */
            case oidCertExtType:
                switch (oid) {
                    case BASIC_CA_OID:
                        return BASIC_CA_OID;
                    case ALT_NAMES_OID:
                        return ALT_NAMES_OID;
                    case CRL_DIST_OID:
                        return CRL_DIST_OID;
                    case AUTH_INFO_OID:
                        return AUTH_INFO_OID;
                    case AUTH_KEY_OID:
                        return AUTH_KEY_OID;
                    case SUBJ_KEY_OID:
                        return SUBJ_KEY_OID;
                    case INHIBIT_ANY_OID:
                        return INHIBIT_ANY_OID;
                    case KEY_USAGE_OID:
                        return NID_ext_key_usage;
                    case NAME_CONS_OID:
                        return NID_name_constraints;
                    case CERT_POLICY_OID:
                        return NID_certificate_policies;
                }
                break;

            /* oidCertAuthInfoType */
            case oidCertAuthInfoType:
                switch (oid) {
                    case AIA_OCSP_OID:
                        return AIA_OCSP_OID;
                    case AIA_CA_ISSUER_OID:
                        return AIA_CA_ISSUER_OID;
                }
                break;

            /* oidCertPolicyType */
            case oidCertPolicyType:
                switch (oid) {
                    case CP_ANY_OID:
                        return NID_any_policy;
                }
                break;

            /* oidCertAltNameType */
            case oidCertAltNameType:
                switch (oid) {
                    case HW_NAME_OID:
                        return NID_hw_name_oid;
                }
                break;

            /* oidCertKeyUseType */
            case oidCertKeyUseType:
                switch (oid) {
                    case EKU_ANY_OID:
                        return NID_anyExtendedKeyUsage;
                    case EKU_SERVER_AUTH_OID:
                        return EKU_SERVER_AUTH_OID;
                    case EKU_CLIENT_AUTH_OID:
                        return EKU_CLIENT_AUTH_OID;
                    case EKU_OCSP_SIGN_OID:
                        return EKU_OCSP_SIGN_OID;
                }
                break;

            /* oidKdfType */
            case oidKdfType:
                switch (oid) {
                    case PBKDF2_OID:
                        return PBKDF2_OID;
                }
                break;

            /* oidPBEType */
            case oidPBEType:
                switch (oid) {
                    case PBE_SHA1_RC4_128:
                        return PBE_SHA1_RC4_128;
                    case PBE_SHA1_DES:
                        return PBE_SHA1_DES;
                    case PBE_SHA1_DES3:
                        return PBE_SHA1_DES3;
                }
                break;

            /* oidKeyWrapType */
            case oidKeyWrapType:
                switch (oid) {
                #ifdef WOLFSSL_AES_128
                    case AES128_WRAP:
                        return AES128_WRAP;
                #endif
                #ifdef WOLFSSL_AES_192
                    case AES192_WRAP:
                        return AES192_WRAP;
                #endif
                #ifdef WOLFSSL_AES_256
                    case AES256_WRAP:
                        return AES256_WRAP;
                #endif
                }
                break;

            /* oidCmsKeyAgreeType */
            case oidCmsKeyAgreeType:
                switch (oid) {
                    #ifndef NO_SHA
                    case dhSinglePass_stdDH_sha1kdf_scheme:
                        return dhSinglePass_stdDH_sha1kdf_scheme;
                    #endif
                    #ifdef WOLFSSL_SHA224
                    case dhSinglePass_stdDH_sha224kdf_scheme:
                        return dhSinglePass_stdDH_sha224kdf_scheme;
                    #endif
                    #ifndef NO_SHA256
                    case dhSinglePass_stdDH_sha256kdf_scheme:
                        return dhSinglePass_stdDH_sha256kdf_scheme;
                    #endif
                    #ifdef WOLFSSL_SHA384
                    case dhSinglePass_stdDH_sha384kdf_scheme:
                        return dhSinglePass_stdDH_sha384kdf_scheme;
                    #endif
                    #ifdef WOLFSSL_SHA512
                    case dhSinglePass_stdDH_sha512kdf_scheme:
                        return dhSinglePass_stdDH_sha512kdf_scheme;
                    #endif
                }
                break;

            default:
                WOLFSSL_MSG("NID not in table");
                return -1;
        }

        return -1;
    }


#ifndef NO_WOLFSSL_STUB
    char * wolfSSL_OBJ_nid2ln(int n)
    {
        (void)n;
        WOLFSSL_ENTER("wolfSSL_OBJ_nid2ln");
        WOLFSSL_STUB("OBJ_nid2ln");

        return NULL;
    }
#endif

#ifndef NO_WOLFSSL_STUB
    int wolfSSL_OBJ_txt2nid(const char* s)
    {
        (void)s;
        WOLFSSL_STUB("OBJ_txt2nid");

        return 0;
    }
#endif

    /* compatibility function. It's intended use is to remove OID's from an
     * internal table that have been added with OBJ_create. wolfSSL manages it's
     * own interenal OID values and does not currently support OBJ_create. */
    void wolfSSL_OBJ_cleanup(void)
    {
        WOLFSSL_ENTER("wolfSSL_OBJ_cleanup()");
    }


    #ifndef NO_WOLFSSL_STUB
    void wolfSSL_set_verify_depth(WOLFSSL *ssl, int depth) {
        WOLFSSL_ENTER("wolfSSL_set_verify_depth");
#ifndef OPENSSL_EXTRA
        (void)ssl;
        (void)depth;
        WOLFSSL_STUB("wolfSSL_set_verify_depth");
#else
        ssl->options.verifyDepth = (byte)depth;
#endif
    }
    #endif


    #ifndef NO_WOLFSSL_STUB
    WOLFSSL_ASN1_OBJECT * wolfSSL_X509_NAME_ENTRY_get_object(WOLFSSL_X509_NAME_ENTRY *ne) {
        (void)ne;
        WOLFSSL_ENTER("wolfSSL_X509_NAME_ENTRY_get_object");
        WOLFSSL_STUB("X509_NAME_ENTRY_get_object");

        return NULL;
    }
    #endif

    WOLFSSL_X509_NAME_ENTRY *wolfSSL_X509_NAME_get_entry(
                                             WOLFSSL_X509_NAME *name, int loc) {

        int maxLoc = name->fullName.fullNameLen;

        WOLFSSL_ENTER("wolfSSL_X509_NAME_get_entry");

        if (loc < 0 || loc > maxLoc) {
            WOLFSSL_MSG("Bad argument");
            return NULL;
        }

        /* DC component */
        if (name->fullName.dcMode){
            if (name->fullName.fullName != NULL){
                if (loc == name->fullName.dcNum){
                    name->cnEntry.data.data   = &name->fullName.fullName[name->fullName.cIdx];
                    name->cnEntry.data.length = name->fullName.cLen;
                    name->cnEntry.nid         = ASN_COUNTRY_NAME;
                } else {
                    name->cnEntry.data.data   = &name->fullName.fullName[name->fullName.dcIdx[loc]];
                    name->cnEntry.data.length = name->fullName.dcLen[loc];
                    name->cnEntry.nid         = ASN_DOMAIN_COMPONENT;
                }
            }
            name->cnEntry.data.type = CTC_UTF8;
            name->cnEntry.set       = 1;
            return &(name->cnEntry);

         /* common name index case */
        } else if (loc == name->fullName.cnIdx) {
            /* get CN shortcut from x509 since it has null terminator */
            name->cnEntry.data.data   = name->x509->subjectCN;
            name->cnEntry.data.length = name->fullName.cnLen;
            name->cnEntry.data.type   = CTC_UTF8;
            name->cnEntry.nid         = ASN_COMMON_NAME;
            name->cnEntry.set         = 1;
            return &(name->cnEntry);
        }

        /* additionall cases to check for go here */

        WOLFSSL_MSG("Entry not found or implemented");
        (void)name;
        (void)loc;

        return NULL;
    }

    #ifndef NO_WOLFSSL_STUB
    void wolfSSL_sk_X509_NAME_pop_free(WOLF_STACK_OF(WOLFSSL_X509_NAME)* sk, void f (WOLFSSL_X509_NAME*)){
        (void) sk;
        (void) f;
        WOLFSSL_ENTER("wolfSSL_sk_X509_NAME_pop_free");
        WOLFSSL_STUB("sk_X509_NAME_pop_free");
    }
    #endif
    #ifndef NO_WOLFSSL_STUB
    int wolfSSL_X509_check_private_key(WOLFSSL_X509 *x509, WOLFSSL_EVP_PKEY *key){
        (void) x509;
        (void) key;
        WOLFSSL_ENTER("wolfSSL_X509_check_private_key");
        WOLFSSL_STUB("X509_check_private_key");

        return WOLFSSL_SUCCESS;
    }

    WOLF_STACK_OF(WOLFSSL_X509_NAME) *wolfSSL_dup_CA_list( WOLF_STACK_OF(WOLFSSL_X509_NAME) *sk ){
        (void) sk;
        WOLFSSL_ENTER("wolfSSL_dup_CA_list");
        WOLFSSL_STUB("SSL_dup_CA_list");

        return NULL;
    }
    #endif

#endif /* OPENSSL_ALL || HAVE_LIGHTY || WOLFSSL_MYSQL_COMPATIBLE || HAVE_STUNNEL || WOLFSSL_NGINX || HAVE_POCO_LIB || WOLFSSL_HAPROXY */
#endif /* OPENSSL_EXTRA */

#ifdef OPENSSL_EXTRA

/* wolfSSL uses negative values for error states. This function returns an
 * unsigned type so the value returned is the absolute value of the error.
 */
unsigned long wolfSSL_ERR_peek_last_error_line(const char **file, int *line)
{
    WOLFSSL_ENTER("wolfSSL_ERR_peek_last_error");

    (void)line;
    (void)file;
#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || defined(DEBUG_WOLFSSL) || defined(WOLFSSL_HAPROXY)
    {
        int ret;

        if ((ret = wc_PeekErrorNode(-1, file, NULL, line)) < 0) {
            WOLFSSL_MSG("Issue peeking at error node in queue");
            return 0;
        }
    #if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX)
        if (ret == -ASN_NO_PEM_HEADER)
            return (ERR_LIB_PEM << 24) | PEM_R_NO_START_LINE;
    #endif
        return (unsigned long)ret;
    }
#else
    return (unsigned long)(0 - NOT_COMPILED_IN);
#endif
}


#ifndef NO_CERTS
int wolfSSL_CTX_use_PrivateKey(WOLFSSL_CTX *ctx, WOLFSSL_EVP_PKEY *pkey)
{
    WOLFSSL_ENTER("wolfSSL_CTX_use_PrivateKey");

    if (ctx == NULL || pkey == NULL) {
        return WOLFSSL_FAILURE;
    }

    if (pkey->pkey.ptr != NULL) {
        /* ptr for WOLFSSL_EVP_PKEY struct is expected to be DER format */
        return wolfSSL_CTX_use_PrivateKey_buffer(ctx,
                                       (const unsigned char*)pkey->pkey.ptr,
                                       pkey->pkey_sz, SSL_FILETYPE_ASN1);
    }

    WOLFSSL_MSG("wolfSSL private key not set");
    return BAD_FUNC_ARG;
}
#endif /* !NO_CERTS */


void* wolfSSL_CTX_get_ex_data(const WOLFSSL_CTX* ctx, int idx)
{
    WOLFSSL_ENTER("wolfSSL_CTX_get_ex_data");
    #ifdef HAVE_EX_DATA
    if(ctx != NULL && idx < MAX_EX_DATA && idx >= 0) {
        return ctx->ex_data[idx];
    }
    #else
    (void)ctx;
    (void)idx;
    #endif
    return NULL;
}

int wolfSSL_CTX_get_ex_new_index(long idx, void* arg, void* a, void* b,
                                void* c)
{
    static int ctx_idx = 0;

    WOLFSSL_ENTER("wolfSSL_CTX_get_ex_new_index");
    (void)idx;
    (void)arg;
    (void)a;
    (void)b;
    (void)c;

    return ctx_idx++;
}


/* Return the index that can be used for the WOLFSSL structure to store
 * application data.
 *
 */
int wolfSSL_get_ex_new_index(long argValue, void* arg,
        WOLFSSL_CRYPTO_EX_new* cb1, WOLFSSL_CRYPTO_EX_dup* cb2,
        WOLFSSL_CRYPTO_EX_free* cb3)
{
    static int ssl_idx = 0;

    WOLFSSL_ENTER("wolfSSL_get_ex_new_index");

    (void)argValue;
    (void)arg;
    (void)cb1;
    (void)cb2;
    (void)cb3;

    return ssl_idx++;
}


int wolfSSL_CTX_set_ex_data(WOLFSSL_CTX* ctx, int idx, void* data)
{
    WOLFSSL_ENTER("wolfSSL_CTX_set_ex_data");
    #ifdef HAVE_EX_DATA
    if (ctx != NULL && idx < MAX_EX_DATA)
    {
        ctx->ex_data[idx] = data;
        return WOLFSSL_SUCCESS;
    }
    #else
    (void)ctx;
    (void)idx;
    (void)data;
    #endif
    return WOLFSSL_FAILURE;
}


/* Returns char* to app data stored in ex[0].
 *
 * ssl WOLFSSL structure to get app data from
 */
void* wolfSSL_get_app_data(const WOLFSSL *ssl)
{
    /* checkout exdata stuff... */
    WOLFSSL_ENTER("wolfSSL_get_app_data");

    return wolfSSL_get_ex_data(ssl, 0);
}


/* Set ex array 0 to have app data
 *
 * ssl WOLFSSL struct to set app data in
 * arg data to be stored
 *
 * Returns SSL_SUCCESS on sucess and SSL_FAILURE on failure
 */
int wolfSSL_set_app_data(WOLFSSL *ssl, void* arg) {
    WOLFSSL_ENTER("wolfSSL_set_app_data");

    return wolfSSL_set_ex_data(ssl, 0, arg);
}


int wolfSSL_set_ex_data(WOLFSSL* ssl, int idx, void* data)
{
    WOLFSSL_ENTER("wolfSSL_set_ex_data");
#if defined(HAVE_EX_DATA) || defined(FORTRESS)
    if (ssl != NULL && idx < MAX_EX_DATA)
    {
        ssl->ex_data[idx] = data;
        return WOLFSSL_SUCCESS;
    }
#else
    WOLFSSL_MSG("HAVE_EX_DATA macro is not defined");
    (void)ssl;
    (void)idx;
    (void)data;
#endif
    return WOLFSSL_FAILURE;
}



void* wolfSSL_get_ex_data(const WOLFSSL* ssl, int idx)
{
    WOLFSSL_ENTER("wolfSSL_get_ex_data");
#if defined(HAVE_EX_DATA) || defined(FORTRESS)
    if (ssl != NULL && idx < MAX_EX_DATA && idx >= 0)
        return ssl->ex_data[idx];
#else
    WOLFSSL_MSG("HAVE_EX_DATA macro is not defined");
    (void)ssl;
    (void)idx;
#endif
    return 0;
}

#ifndef NO_DSA
WOLFSSL_DSA *wolfSSL_PEM_read_bio_DSAparams(WOLFSSL_BIO *bp, WOLFSSL_DSA **x,
        pem_password_cb *cb, void *u)
{
    WOLFSSL_DSA* dsa;
    DsaKey* key;
    int    length;
    unsigned char*  buf;
    word32 bufSz;
    int ret;
    word32 idx = 0;
    DerBuffer* pDer;

    WOLFSSL_ENTER("wolfSSL_PEM_read_bio_DSAparams");

    ret = wolfSSL_BIO_get_mem_data(bp, &buf);
    if (ret <= 0) {
        WOLFSSL_LEAVE("wolfSSL_PEM_read_bio_DSAparams", ret);
        return NULL;
    }

    bufSz = (word32)ret;

    if (cb != NULL || u != NULL) {
        /*
         * cb is for a call back when encountering encrypted PEM files
         * if cb == NULL and u != NULL then u = null terminated password string
         */
        WOLFSSL_MSG("Not yet supporting call back or password for encrypted PEM");
    }

    if ((ret = PemToDer(buf, (long)bufSz, DSA_PARAM_TYPE, &pDer, NULL, NULL,
                    NULL)) < 0 ) {
        WOLFSSL_MSG("Issue converting from PEM to DER");
        return NULL;
    }

    if ((ret = GetSequence(pDer->buffer, &idx, &length, pDer->length)) < 0) {
        WOLFSSL_LEAVE("wolfSSL_PEM_read_bio_DSAparams", ret);
        FreeDer(&pDer);
        return NULL;
    }

    dsa = wolfSSL_DSA_new();
    if (dsa == NULL) {
        FreeDer(&pDer);
        WOLFSSL_MSG("Error creating DSA struct");
        return NULL;
    }

    key = (DsaKey*)dsa->internal;
    if (key == NULL) {
        FreeDer(&pDer);
        wolfSSL_DSA_free(dsa);
        WOLFSSL_MSG("Error finding DSA key struct");
        return NULL;
    }

    if (GetInt(&key->p,  pDer->buffer, &idx, pDer->length) < 0 ||
        GetInt(&key->q,  pDer->buffer, &idx, pDer->length) < 0 ||
        GetInt(&key->g,  pDer->buffer, &idx, pDer->length) < 0 ) {
        WOLFSSL_MSG("dsa key error");
        FreeDer(&pDer);
        wolfSSL_DSA_free(dsa);
        return NULL;
    }

    if (SetIndividualExternal(&dsa->p, &key->p) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("dsa p key error");
        FreeDer(&pDer);
        wolfSSL_DSA_free(dsa);
        return NULL;
    }

    if (SetIndividualExternal(&dsa->q, &key->q) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("dsa q key error");
        FreeDer(&pDer);
        wolfSSL_DSA_free(dsa);
        return NULL;
    }

    if (SetIndividualExternal(&dsa->g, &key->g) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("dsa g key error");
        FreeDer(&pDer);
        wolfSSL_DSA_free(dsa);
        return NULL;
    }

    if (x != NULL) {
        *x = dsa;
    }

    FreeDer(&pDer);
    return dsa;
}
#endif /* NO_DSA */

#define WOLFSSL_BIO_INCLUDED
#include "src/bio.c"

/* Begin functions for openssl/buffer.h */
WOLFSSL_BUF_MEM* wolfSSL_BUF_MEM_new(void)
{
    WOLFSSL_BUF_MEM* buf;
    buf = (WOLFSSL_BUF_MEM*)XMALLOC(sizeof(WOLFSSL_BUF_MEM), NULL,
                                                        DYNAMIC_TYPE_OPENSSL);
    if (buf) {
        XMEMSET(buf, 0, sizeof(WOLFSSL_BUF_MEM));
    }
    return buf;
}


/* returns length of buffer on success */
int wolfSSL_BUF_MEM_grow(WOLFSSL_BUF_MEM* buf, size_t len)
{
    int len_int = (int)len;
    int mx;

    /* verify provided arguments */
    if (buf == NULL || len_int < 0) {
        return 0; /* BAD_FUNC_ARG; */
    }

    /* check to see if fits in existing length */
    if (buf->length > len) {
        buf->length = len;
        return len_int;
    }

    /* check to see if fits in max buffer */
    if (buf->max >= len) {
        if (buf->data != NULL) {
            XMEMSET(&buf->data[buf->length], 0, len - buf->length);
        }
        buf->length = len;
        return len_int;
    }

    /* expand size, to handle growth */
    mx = (len_int + 3) / 3 * 4;

    /* use realloc */
    buf->data = (char*)XREALLOC(buf->data, mx, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (buf->data == NULL) {
        return 0; /* ERR_R_MALLOC_FAILURE; */
    }

    buf->max = mx;
    XMEMSET(&buf->data[buf->length], 0, len - buf->length);
    buf->length = len;

    return len_int;
}

void wolfSSL_BUF_MEM_free(WOLFSSL_BUF_MEM* buf)
{
    if (buf) {
        if (buf->data) {
            XFREE(buf->data, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            buf->data = NULL;
        }
        buf->max = 0;
        buf->length = 0;
        XFREE(buf, NULL, DYNAMIC_TYPE_OPENSSL);
    }
}
/* End Functions for openssl/buffer.h */

#endif /* OPENSSL_EXTRA */


#if defined(HAVE_LIGHTY) || defined(HAVE_STUNNEL) \
    || defined(WOLFSSL_MYSQL_COMPATIBLE) || defined(OPENSSL_EXTRA)

WOLFSSL_BIO *wolfSSL_BIO_new_file(const char *filename, const char *mode)
{
#ifndef NO_FILESYSTEM
    WOLFSSL_BIO* bio;
    XFILE fp;

    WOLFSSL_ENTER("wolfSSL_BIO_new_file");

    fp = XFOPEN(filename, mode);
    if (fp == NULL)
        return NULL;

    bio = wolfSSL_BIO_new(wolfSSL_BIO_s_file());
    if (bio == NULL) {
        XFCLOSE(fp);
        return bio;
    }

    if (wolfSSL_BIO_set_fp(bio, fp, BIO_CLOSE) != WOLFSSL_SUCCESS) {
        XFCLOSE(fp);
        wolfSSL_BIO_free(bio);
        bio = NULL;
    }

    /* file is closed when BIO is free'd */
    return bio;
#else
    (void)filename;
    (void)mode;
    return NULL;
#endif /* NO_FILESYSTEM */
}


#ifndef NO_DH
WOLFSSL_DH *wolfSSL_PEM_read_bio_DHparams(WOLFSSL_BIO *bio, WOLFSSL_DH **x,
        pem_password_cb *cb, void *u)
{
#ifndef NO_FILESYSTEM
    WOLFSSL_DH* localDh = NULL;
    unsigned char* mem  = NULL;
    word32 size;
    long   sz;
    int    ret;
    DerBuffer *der = NULL;
    byte*  p = NULL;
    byte*  g = NULL;
    word32 pSz = MAX_DH_SIZE;
    word32 gSz = MAX_DH_SIZE;
    int    memAlloced = 0;

    WOLFSSL_ENTER("wolfSSL_PEM_read_bio_DHparams");
    (void)cb;
    (void)u;

    if (bio == NULL) {
        WOLFSSL_MSG("Bad Function Argument bio is NULL");
        return NULL;
    }

    if (bio->type == WOLFSSL_BIO_MEMORY) {
        /* Use the buffer directly. */
        ret = wolfSSL_BIO_get_mem_data(bio, &mem);
        if (mem == NULL || ret <= 0) {
            WOLFSSL_MSG("Failed to get data from bio struct");
            goto end;
        }
        size = ret;
    }
    else if (bio->type == WOLFSSL_BIO_FILE) {
        /* Read whole file into a new buffer. */
        XFSEEK(bio->file, 0, SEEK_END);
        sz = XFTELL(bio->file);
        XFSEEK(bio->file, 0, SEEK_SET);
        if (sz <= 0L)
            goto end;
        mem = (unsigned char*)XMALLOC(sz, NULL, DYNAMIC_TYPE_PEM);
        if (mem == NULL)
            goto end;
        memAlloced = 1;

        if (wolfSSL_BIO_read(bio, (char *)mem, (int)sz) <= 0)
            goto end;
        size = (word32)sz;
    }
    else {
        WOLFSSL_MSG("BIO type not supported for reading DH parameters");
        goto end;
    }

    ret = PemToDer(mem, size, DH_PARAM_TYPE, &der, NULL, NULL, NULL);
    if (ret != 0)
        goto end;

    /* Use the object passed in, otherwise allocate a new object */
    if (x != NULL)
        localDh = *x;
    if (localDh == NULL) {
        localDh = (WOLFSSL_DH*)XMALLOC(sizeof(WOLFSSL_DH), NULL,
                                       DYNAMIC_TYPE_OPENSSL);
        if (localDh == NULL)
            goto end;
        XMEMSET(localDh, 0, sizeof(WOLFSSL_DH));
    }

    /* Load data in manually */
    p = (byte*)XMALLOC(pSz, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
    g = (byte*)XMALLOC(gSz, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
    if (p == NULL || g == NULL)
        goto end;

    /* Extract the p and g as data from the DER encoded DH parameters. */
    ret = wc_DhParamsLoad(der->buffer, der->length, p, &pSz, g, &gSz);
    if (ret != 0) {
        if (x != NULL && localDh != *x)
            XFREE(localDh, NULL, DYNAMIC_TYPE_OPENSSL);
        localDh = NULL;
        goto end;
    }

    if (x != NULL)
        *x = localDh;

    /* Put p and g in as big numbers. */
    if (localDh->p != NULL) {
        wolfSSL_BN_free(localDh->p);
        localDh->p = NULL;
    }
    if (localDh->g != NULL) {
        wolfSSL_BN_free(localDh->g);
        localDh->g = NULL;
    }
    localDh->p = wolfSSL_BN_bin2bn(p, pSz, NULL);
    localDh->g = wolfSSL_BN_bin2bn(g, gSz, NULL);
    if (localDh->p == NULL || localDh->g == NULL) {
        if (x != NULL && localDh != *x)
            wolfSSL_DH_free(localDh);
        localDh = NULL;
    }

end:
    if (memAlloced) XFREE(mem, NULL, DYNAMIC_TYPE_PEM);
    if (der != NULL) FreeDer(&der);
    XFREE(p, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
    XFREE(g, NULL, DYNAMIC_TYPE_PUBLIC_KEY);
    return localDh;
#else
    (void)bio;
    (void)x;
    (void)cb;
    (void)u;
    return NULL;
#endif
}
#endif

#ifdef WOLFSSL_CERT_GEN

#ifdef WOLFSSL_CERT_REQ
/* writes the x509 from x to the WOLFSSL_BIO bp
 *
 * returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on fail
 */
int wolfSSL_PEM_write_bio_X509_REQ(WOLFSSL_BIO *bp, WOLFSSL_X509 *x)
{
    byte* pem;
    int   pemSz = 0;
    const unsigned char* der;
    int derSz;
    int ret;

    WOLFSSL_ENTER("wolfSSL_PEM_write_bio_X509_REQ()");

    if (x == NULL || bp == NULL) {
        return WOLFSSL_FAILURE;
    }

    der = wolfSSL_X509_get_der(x, &derSz);
    if (der == NULL) {
        return WOLFSSL_FAILURE;
    }

    /* get PEM size */
    pemSz = wc_DerToPemEx(der, derSz, NULL, 0, NULL, CERTREQ_TYPE);
    if (pemSz < 0) {
        return WOLFSSL_FAILURE;
    }

    /* create PEM buffer and convert from DER */
    pem = (byte*)XMALLOC(pemSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (pem == NULL) {
        return WOLFSSL_FAILURE;
    }
    if (wc_DerToPemEx(der, derSz, pem, pemSz, NULL, CERTREQ_TYPE) < 0) {
        XFREE(pem, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return WOLFSSL_FAILURE;
    }

    /* write the PEM to BIO */
    ret = wolfSSL_BIO_write(bp, pem, pemSz);
    XFREE(pem, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    if (ret <= 0) return WOLFSSL_FAILURE;
    return WOLFSSL_SUCCESS;
}
#endif /* WOLFSSL_CERT_REQ */


/* writes the x509 from x to the WOLFSSL_BIO bp
 *
 * returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on fail
 */
int wolfSSL_PEM_write_bio_X509_AUX(WOLFSSL_BIO *bp, WOLFSSL_X509 *x)
{
    byte* pem;
    int   pemSz = 0;
    const unsigned char* der;
    int derSz;
    int ret;

    WOLFSSL_ENTER("wolfSSL_PEM_write_bio_X509_AUX()");

    if (bp == NULL || x == NULL) {
        WOLFSSL_MSG("NULL argument passed in");
        return WOLFSSL_FAILURE;
    }

    der = wolfSSL_X509_get_der(x, &derSz);
    if (der == NULL) {
        return WOLFSSL_FAILURE;
    }

    /* get PEM size */
    pemSz = wc_DerToPemEx(der, derSz, NULL, 0, NULL, CERT_TYPE);
    if (pemSz < 0) {
        return WOLFSSL_FAILURE;
    }

    /* create PEM buffer and convert from DER */
    pem = (byte*)XMALLOC(pemSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (pem == NULL) {
        return WOLFSSL_FAILURE;
    }
    if (wc_DerToPemEx(der, derSz, pem, pemSz, NULL, CERT_TYPE) < 0) {
        XFREE(pem, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return WOLFSSL_FAILURE;
    }

    /* write the PEM to BIO */
    ret = wolfSSL_BIO_write(bp, pem, pemSz);
    XFREE(pem, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    if (ret <= 0) return WOLFSSL_FAILURE;
    return WOLFSSL_SUCCESS;
}
#endif /* WOLFSSL_CERT_GEN */

int wolfSSL_PEM_write_bio_X509(WOLFSSL_BIO *bio, WOLFSSL_X509 *cert)
{
    byte* pem;
    int   pemSz = 0;
    const unsigned char* der;
    int derSz;
    int ret;

    WOLFSSL_ENTER("wolfSSL_PEM_write_bio_X509_AUX()");

    if (bio == NULL || cert == NULL) {
        WOLFSSL_MSG("NULL argument passed in");
        return WOLFSSL_FAILURE;
    }

    der = wolfSSL_X509_get_der(cert, &derSz);
    if (der == NULL) {
        return WOLFSSL_FAILURE;
    }

    /* get PEM size */
    pemSz = wc_DerToPemEx(der, derSz, NULL, 0, NULL, CERT_TYPE);
    if (pemSz < 0) {
        return WOLFSSL_FAILURE;
    }

    /* create PEM buffer and convert from DER */
    pem = (byte*)XMALLOC(pemSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (pem == NULL) {
        return WOLFSSL_FAILURE;
    }
    if (wc_DerToPemEx(der, derSz, pem, pemSz, NULL, CERT_TYPE) < 0) {
        XFREE(pem, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return WOLFSSL_FAILURE;
    }

    /* write the PEM to BIO */
    ret = wolfSSL_BIO_write(bio, pem, pemSz);
    XFREE(pem, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    if (ret <= 0) return WOLFSSL_FAILURE;
    return WOLFSSL_SUCCESS;
}


#if defined(OPENSSL_EXTRA) && !defined(NO_DH)
/* Intialize ctx->dh with dh's params. Return WOLFSSL_SUCCESS on ok */
long wolfSSL_CTX_set_tmp_dh(WOLFSSL_CTX* ctx, WOLFSSL_DH* dh)
{
    int pSz, gSz;
    byte *p, *g;
    int ret=0;

    WOLFSSL_ENTER("wolfSSL_CTX_set_tmp_dh");

    if(!ctx || !dh)
        return BAD_FUNC_ARG;

    /* Get needed size for p and g */
    pSz = wolfSSL_BN_bn2bin(dh->p, NULL);
    gSz = wolfSSL_BN_bn2bin(dh->g, NULL);

    if(pSz <= 0 || gSz <= 0)
        return WOLFSSL_FATAL_ERROR;

    p = (byte*)XMALLOC(pSz, ctx->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    if(!p)
        return MEMORY_E;

    g = (byte*)XMALLOC(gSz, ctx->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    if(!g) {
        XFREE(p, ctx->heap, DYNAMIC_TYPE_PUBLIC_KEY);
        return MEMORY_E;
    }

    pSz = wolfSSL_BN_bn2bin(dh->p, p);
    gSz = wolfSSL_BN_bn2bin(dh->g, g);

    if(pSz >= 0 && gSz >= 0) /* Conversion successful */
        ret = wolfSSL_CTX_SetTmpDH(ctx, p, pSz, g, gSz);

    XFREE(p, ctx->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    XFREE(g, ctx->heap, DYNAMIC_TYPE_PUBLIC_KEY);

    return pSz > 0 && gSz > 0 ? ret : WOLFSSL_FATAL_ERROR;
}
#endif /* OPENSSL_EXTRA && !NO_DH */


/* returns the enum value associated with handshake state
 *
 * ssl the WOLFSSL structure to get state of
 */
int wolfSSL_get_state(const WOLFSSL* ssl)
{
    WOLFSSL_ENTER("wolfSSL_get_state");

    if (ssl == NULL) {
        WOLFSSL_MSG("Null argument passed in");
        return SSL_FAILURE;
    }

    return ssl->options.handShakeState;
}
#endif /* HAVE_LIGHTY || HAVE_STUNNEL || WOLFSSL_MYSQL_COMPATIBLE */


/* stunnel compatibility functions*/
#if defined(OPENSSL_EXTRA) && (defined(HAVE_STUNNEL) || defined(WOLFSSL_NGINX))
void WOLFSSL_ERR_remove_thread_state(void* pid)
{
    (void) pid;
    return;
}

#ifndef NO_FILESYSTEM
/***TBD ***/
void wolfSSL_print_all_errors_fp(XFILE *fp)
{
    (void)fp;
}
#endif

int wolfSSL_SESSION_set_ex_data(WOLFSSL_SESSION* session, int idx, void* data)
{
    WOLFSSL_ENTER("wolfSSL_SESSION_set_ex_data");
#ifdef HAVE_EX_DATA
    if(session != NULL && idx < MAX_EX_DATA) {
        session->ex_data[idx] = data;
        return WOLFSSL_SUCCESS;
    }
#else
    (void)session;
    (void)idx;
    (void)data;
#endif
    return WOLFSSL_FAILURE;
}


int wolfSSL_SESSION_get_ex_new_index(long idx, void* data, void* cb1,
       void* cb2, CRYPTO_free_func* cb3)
{
    WOLFSSL_ENTER("wolfSSL_SESSION_get_ex_new_index");
    (void)idx;
    (void)cb1;
    (void)cb2;
    (void)cb3;
    if (XSTRNCMP((const char*)data, "redirect index", 14) == 0) {
        return 0;
    }
    else if (XSTRNCMP((const char*)data, "addr index", 10) == 0) {
        return 1;
    }
    return WOLFSSL_FAILURE;
}


void* wolfSSL_SESSION_get_ex_data(const WOLFSSL_SESSION* session, int idx)
{
    WOLFSSL_ENTER("wolfSSL_SESSION_get_ex_data");
#ifdef HAVE_EX_DATA
    if (session != NULL && idx < MAX_EX_DATA && idx >= 0)
        return session->ex_data[idx];
#else
    (void)session;
    (void)idx;
#endif
    return NULL;
}

#ifndef NO_WOLFSSL_STUB
int wolfSSL_CRYPTO_set_mem_ex_functions(void *(*m) (size_t, const char *, int),
                                void *(*r) (void *, size_t, const char *,
                                            int), void (*f) (void *))
{
    (void) m;
    (void) r;
    (void) f;
    WOLFSSL_ENTER("wolfSSL_CRYPTO_set_mem_ex_functions");
    WOLFSSL_STUB("CRYPTO_set_mem_ex_functions");

    return WOLFSSL_FAILURE;
}
#endif


void wolfSSL_CRYPTO_cleanup_all_ex_data(void){
    WOLFSSL_ENTER("CRYPTO_cleanup_all_ex_data");
}


#ifndef NO_WOLFSSL_STUB
WOLFSSL_DH *wolfSSL_DH_generate_parameters(int prime_len, int generator,
                           void (*callback) (int, int, void *), void *cb_arg)
{
    (void)prime_len;
    (void)generator;
    (void)callback;
    (void)cb_arg;
    WOLFSSL_ENTER("wolfSSL_DH_generate_parameters");
    WOLFSSL_STUB("DH_generate_parameters");

    return NULL;
}
#endif

#ifndef NO_WOLFSSL_STUB
int wolfSSL_DH_generate_parameters_ex(WOLFSSL_DH* dh, int prime_len, int generator,
                           void (*callback) (int, int, void *))
{
    (void)prime_len;
    (void)generator;
    (void)callback;
    (void)dh;
    WOLFSSL_ENTER("wolfSSL_DH_generate_parameters_ex");
    WOLFSSL_STUB("DH_generate_parameters_ex");

    return -1;
}
#endif

void wolfSSL_ERR_load_crypto_strings(void)
{
    WOLFSSL_ENTER("wolfSSL_ERR_load_crypto_strings");
    /* Do nothing */
    return;
}

#ifndef NO_WOLFSSL_STUB
unsigned long wolfSSL_ERR_peek_last_error(void)
{
    WOLFSSL_ENTER("wolfSSL_ERR_peek_last_error");

#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX)
    {
        int ret;

        if ((ret = wc_PeekErrorNode(-1, NULL, NULL, NULL)) < 0) {
            WOLFSSL_MSG("Issue peeking at error node in queue");
            return 0;
        }
        if (ret == -ASN_NO_PEM_HEADER)
            return (ERR_LIB_PEM << 24) | PEM_R_NO_START_LINE;
        return (unsigned long)ret;
    }
#else
    return (unsigned long)(0 - NOT_COMPILED_IN);
#endif
}
#endif
#ifndef NO_WOLFSSL_STUB
int wolfSSL_FIPS_mode(void)
{
    WOLFSSL_ENTER("wolfSSL_FIPS_mode");
    WOLFSSL_STUB("FIPS_mode");

    return WOLFSSL_FAILURE;
}
#endif

#ifndef NO_WOLFSSL_STUB
int wolfSSL_FIPS_mode_set(int r)
{
    (void)r;
    WOLFSSL_ENTER("wolfSSL_FIPS_mode_set");
    WOLFSSL_STUB("FIPS_mode_set");

    return WOLFSSL_FAILURE;
}
#endif

#ifndef NO_WOLFSSL_STUB
int wolfSSL_RAND_set_rand_method(const void *meth)
{
    (void) meth;
    WOLFSSL_ENTER("wolfSSL_RAND_set_rand_method");
    WOLFSSL_STUB("RAND_set_rand_method");

    /* if implemented RAND_bytes and RAND_pseudo_bytes need updated
     * those two functions will call the respective functions from meth */
    return SSL_FAILURE;
}
#endif

int wolfSSL_CIPHER_get_bits(const WOLFSSL_CIPHER *c, int *alg_bits)
{
    int ret = WOLFSSL_FAILURE;
    WOLFSSL_ENTER("wolfSSL_CIPHER_get_bits");
    if(c != NULL && c->ssl != NULL) {
        ret = 8 * c->ssl->specs.key_size;
        if(alg_bits != NULL) {
            *alg_bits = ret;
        }
    }
    return ret;
}
#endif /* #if defined(OPENSSL_EXTRA) && (defined(HAVE_STUNNEL) || defined(WOLFSSL_NGINX)) */


/* stunnel compatibility functions*/
#if defined(OPENSSL_EXTRA) && (defined(HAVE_STUNNEL) || defined(WOLFSSL_NGINX))

int wolfSSL_sk_X509_NAME_num(const WOLF_STACK_OF(WOLFSSL_X509_NAME) *s)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_NAME_num");

    if (s == NULL)
        return -1;
    return (int)s->num;
}


int wolfSSL_sk_X509_num(const WOLF_STACK_OF(WOLFSSL_X509) *s)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_num");

    if (s == NULL)
        return -1;
    return (int)s->num;
}

int wolfSSL_X509_NAME_print_ex(WOLFSSL_BIO* bio, WOLFSSL_X509_NAME* name,
                int indent, unsigned long flags)
{
    int i;
    (void)flags;
    WOLFSSL_ENTER("wolfSSL_X509_NAME_print_ex");

    for (i = 0; i < indent; i++) {
        if (wolfSSL_BIO_write(bio, " ", 1) != 1)
            return WOLFSSL_FAILURE;
    }

    if (flags == XN_FLAG_RFC2253) {
        if (wolfSSL_BIO_write(bio, name->name + 1, name->sz - 2)
                                                                != name->sz - 2)
            return WOLFSSL_FAILURE;
    }
    else if (wolfSSL_BIO_write(bio, name->name, name->sz) != name->sz)
        return WOLFSSL_FAILURE;

    return WOLFSSL_SUCCESS;
}

#ifndef NO_WOLFSSL_STUB
WOLFSSL_ASN1_BIT_STRING* wolfSSL_X509_get0_pubkey_bitstr(const WOLFSSL_X509* x)
{
    (void)x;
    WOLFSSL_ENTER("wolfSSL_X509_get0_pubkey_bitstr");
    WOLFSSL_STUB("X509_get0_pubkey_bitstr");

    return NULL;
}
#endif

#ifndef NO_WOLFSSL_STUB
int wolfSSL_CTX_add_session(WOLFSSL_CTX* ctx, WOLFSSL_SESSION* session)
{
    (void)ctx;
    (void)session;
    WOLFSSL_ENTER("wolfSSL_CTX_add_session");
    WOLFSSL_STUB("SSL_CTX_add_session");

    return WOLFSSL_SUCCESS;
}
#endif


int wolfSSL_version(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("wolfSSL_version");
    if (ssl->version.major == SSLv3_MAJOR) {
        switch (ssl->version.minor) {
            case SSLv3_MINOR :
                return SSL3_VERSION;
            case TLSv1_MINOR :
            case TLSv1_1_MINOR :
            case TLSv1_2_MINOR :
            case TLSv1_3_MINOR :
                return TLS1_VERSION;
            default:
                return WOLFSSL_FAILURE;
        }
    }
    else if (ssl->version.major == DTLS_MAJOR) {
        switch (ssl->version.minor) {
            case DTLS_MINOR :
            case DTLSv1_2_MINOR :
                return DTLS1_VERSION;
            default:
                return WOLFSSL_FAILURE;
        }
    }
    return WOLFSSL_FAILURE;
}


WOLFSSL_CTX* wolfSSL_get_SSL_CTX(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("wolfSSL_get_SSL_CTX");
    return ssl->ctx;
}

int wolfSSL_X509_NAME_get_sz(WOLFSSL_X509_NAME* name)
{
    WOLFSSL_ENTER("wolfSSL_X509_NAME_get_sz");
    if(!name)
        return -1;
    return name->sz;
}

#ifdef HAVE_SNI
int wolfSSL_set_tlsext_host_name(WOLFSSL* ssl, const char* host_name)
{
    int ret;
    WOLFSSL_ENTER("wolfSSL_set_tlsext_host_name");
    ret = wolfSSL_UseSNI(ssl, WOLFSSL_SNI_HOST_NAME,
            host_name, (word16)XSTRLEN(host_name));
    WOLFSSL_LEAVE("wolfSSL_set_tlsext_host_name", ret);
    return ret;
}


#ifndef NO_WOLFSSL_SERVER
const char * wolfSSL_get_servername(WOLFSSL* ssl, byte type)
{
    void * serverName = NULL;
    if (ssl == NULL)
        return NULL;
    TLSX_SNI_GetRequest(ssl->extensions, type, &serverName);
    return (const char *)serverName;
}
#endif /* NO_WOLFSSL_SERVER */
#endif /* HAVE_SNI */

WOLFSSL_CTX* wolfSSL_set_SSL_CTX(WOLFSSL* ssl, WOLFSSL_CTX* ctx)
{
    if (ssl && ctx && SetSSL_CTX(ssl, ctx, 0) == WOLFSSL_SUCCESS)
        return ssl->ctx;
    return NULL;
}


VerifyCallback wolfSSL_CTX_get_verify_callback(WOLFSSL_CTX* ctx)
{
    WOLFSSL_ENTER("wolfSSL_CTX_get_verify_callback");
    if(ctx)
        return ctx->verifyCallback;
    return NULL;
}


void wolfSSL_CTX_set_servername_callback(WOLFSSL_CTX* ctx, CallbackSniRecv cb)
{
    WOLFSSL_ENTER("wolfSSL_CTX_set_servername_callback");
    if (ctx)
        ctx->sniRecvCb = cb;
}

int wolfSSL_CTX_set_tlsext_servername_callback(WOLFSSL_CTX* ctx,
                                               CallbackSniRecv cb)
{
    WOLFSSL_ENTER("wolfSSL_CTX_set_tlsext_servername_callback");
    if (ctx) {
        ctx->sniRecvCb = cb;
        return 1;
    }
    return 0;
}

void wolfSSL_CTX_set_servername_arg(WOLFSSL_CTX* ctx, void* arg)
{
    WOLFSSL_ENTER("wolfSSL_CTX_set_servername_arg");
    if (ctx)
        ctx->sniRecvCbArg = arg;
}

void wolfSSL_ERR_load_BIO_strings(void) {
    WOLFSSL_ENTER("ERR_load_BIO_strings");
    /* do nothing */
}

#ifndef NO_WOLFSSL_STUB
void wolfSSL_THREADID_set_callback(void(*threadid_func)(void*))
{
    WOLFSSL_ENTER("wolfSSL_THREADID_set_callback");
    WOLFSSL_STUB("CRYPTO_THREADID_set_callback");
    (void)threadid_func;
    return;
}
#endif

#ifndef NO_WOLFSSL_STUB
void wolfSSL_THREADID_set_numeric(void* id, unsigned long val)
{
    WOLFSSL_ENTER("wolfSSL_THREADID_set_numeric");
    WOLFSSL_STUB("CRYPTO_THREADID_set_numeric");
    (void)id;
    (void)val;
    return;
}
#endif


#ifndef NO_WOLFSSL_STUB
WOLF_STACK_OF(WOLFSSL_X509)* wolfSSL_X509_STORE_get1_certs(WOLFSSL_X509_STORE_CTX* ctx,
                                                WOLFSSL_X509_NAME* name)
{
    WOLFSSL_ENTER("wolfSSL_X509_STORE_get1_certs");
    WOLFSSL_STUB("X509_STORE_get1_certs");
    (void)ctx;
    (void)name;
    return NULL;
}
#endif

#endif /* OPENSSL_EXTRA and HAVE_STUNNEL */

#if defined(OPENSSL_ALL) || \
    (defined(OPENSSL_EXTRA) && (defined(HAVE_STUNNEL) || \
     defined(WOLFSSL_NGINX)) || defined(WOLFSSL_HAPROXY))

const byte* wolfSSL_SESSION_get_id(WOLFSSL_SESSION* sess, unsigned int* idLen)
{
    WOLFSSL_ENTER("wolfSSL_SESSION_get_id");
    if(!sess || !idLen) {
        WOLFSSL_MSG("Bad func args. Please provide idLen");
        return NULL;
    }
    *idLen = sess->sessionIDSz;
    return sess->sessionID;
}
#endif

#if defined(OPENSSL_ALL) || (defined(OPENSSL_EXTRA) && defined(HAVE_STUNNEL)) \
    || defined(WOLFSSL_MYSQL_COMPATIBLE) || defined(WOLFSSL_NGINX)

int wolfSSL_CTX_get_verify_mode(WOLFSSL_CTX* ctx)
{
    int mode = 0;
    WOLFSSL_ENTER("wolfSSL_CTX_get_verify_mode");

    if(!ctx)
        return WOLFSSL_FATAL_ERROR;

    if (ctx->verifyPeer)
        mode |= WOLFSSL_VERIFY_PEER;
    else if (ctx->verifyNone)
        mode |= WOLFSSL_VERIFY_NONE;

    if (ctx->failNoCert)
        mode |= WOLFSSL_VERIFY_FAIL_IF_NO_PEER_CERT;

    if (ctx->failNoCertxPSK)
        mode |= WOLFSSL_VERIFY_FAIL_EXCEPT_PSK;

    WOLFSSL_LEAVE("wolfSSL_CTX_get_verify_mode", mode);
    return mode;
}
#endif

#if defined(OPENSSL_EXTRA) && defined(HAVE_CURVE25519)
/* return 1 if success, 0 if error
 * output keys are little endian format
 */
int wolfSSL_EC25519_generate_key(unsigned char *priv, unsigned int *privSz,
                                 unsigned char *pub, unsigned int *pubSz)
{
#ifndef WOLFSSL_KEY_GEN
    WOLFSSL_MSG("No Key Gen built in");
    (void) priv;
    (void) privSz;
    (void) pub;
    (void) pubSz;
    return WOLFSSL_FAILURE;
#else /* WOLFSSL_KEY_GEN */
    int ret = WOLFSSL_FAILURE;
    int initTmpRng = 0;
    WC_RNG *rng = NULL;
#ifdef WOLFSSL_SMALL_STACK
    WC_RNG *tmpRNG = NULL;
#else
    WC_RNG tmpRNG[1];
#endif

    WOLFSSL_ENTER("wolfSSL_EC25519_generate_key");

    if (priv == NULL || privSz == NULL || *privSz < CURVE25519_KEYSIZE ||
        pub == NULL || pubSz == NULL || *pubSz < CURVE25519_KEYSIZE) {
        WOLFSSL_MSG("Bad arguments");
        return WOLFSSL_FAILURE;
    }

#ifdef WOLFSSL_SMALL_STACK
    tmpRNG = (WC_RNG*)XMALLOC(sizeof(WC_RNG), NULL, DYNAMIC_TYPE_RNG);
    if (tmpRNG == NULL)
        return WOLFSSL_FAILURE;
#endif
    if (wc_InitRng(tmpRNG) == 0) {
        rng = tmpRNG;
        initTmpRng = 1;
    }
    else {
        WOLFSSL_MSG("Bad RNG Init, trying global");
        if (initGlobalRNG == 0)
            WOLFSSL_MSG("Global RNG no Init");
        else
            rng = &globalRNG;
    }

    if (rng) {
        curve25519_key key;

        if (wc_curve25519_init(&key) != MP_OKAY)
            WOLFSSL_MSG("wc_curve25519_init failed");
        else if (wc_curve25519_make_key(rng, CURVE25519_KEYSIZE, &key)!=MP_OKAY)
            WOLFSSL_MSG("wc_curve25519_make_key failed");
        /* export key pair */
        else if (wc_curve25519_export_key_raw_ex(&key, priv, privSz, pub,
                                                 pubSz, EC25519_LITTLE_ENDIAN)
                 != MP_OKAY)
            WOLFSSL_MSG("wc_curve25519_export_key_raw_ex failed");
        else
            ret = WOLFSSL_SUCCESS;

        wc_curve25519_free(&key);
    }

    if (initTmpRng)
        wc_FreeRng(tmpRNG);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(tmpRNG, NULL, DYNAMIC_TYPE_RNG);
#endif

    return ret;
#endif /* WOLFSSL_KEY_GEN */
}

/* return 1 if success, 0 if error
 * input and output keys are little endian format
 */
int wolfSSL_EC25519_shared_key(unsigned char *shared, unsigned int *sharedSz,
                               const unsigned char *priv, unsigned int privSz,
                               const unsigned char *pub, unsigned int pubSz)
{
#ifndef WOLFSSL_KEY_GEN
    WOLFSSL_MSG("No Key Gen built in");
    (void) shared;
    (void) sharedSz;
    (void) priv;
    (void) privSz;
    (void) pub;
    (void) pubSz;
    return WOLFSSL_FAILURE;
#else /* WOLFSSL_KEY_GEN */
    int ret = WOLFSSL_FAILURE;
    curve25519_key privkey, pubkey;

    WOLFSSL_ENTER("wolfSSL_EC25519_shared_key");

    if (shared == NULL || sharedSz == NULL || *sharedSz < CURVE25519_KEYSIZE ||
        priv == NULL || privSz < CURVE25519_KEYSIZE ||
        pub == NULL || pubSz < CURVE25519_KEYSIZE) {
        WOLFSSL_MSG("Bad arguments");
        return WOLFSSL_FAILURE;
    }

    /* import private key */
    if (wc_curve25519_init(&privkey) != MP_OKAY) {
        WOLFSSL_MSG("wc_curve25519_init privkey failed");
        return ret;
    }
    if (wc_curve25519_import_private_ex(priv, privSz, &privkey,
                                        EC25519_LITTLE_ENDIAN) != MP_OKAY) {
        WOLFSSL_MSG("wc_curve25519_import_private_ex failed");
        wc_curve25519_free(&privkey);
        return ret;
    }

    /* import public key */
    if (wc_curve25519_init(&pubkey) != MP_OKAY) {
        WOLFSSL_MSG("wc_curve25519_init pubkey failed");
        wc_curve25519_free(&privkey);
        return ret;
    }
    if (wc_curve25519_import_public_ex(pub, pubSz, &pubkey,
                                       EC25519_LITTLE_ENDIAN) != MP_OKAY) {
        WOLFSSL_MSG("wc_curve25519_import_public_ex failed");
        wc_curve25519_free(&privkey);
        wc_curve25519_free(&pubkey);
        return ret;
    }

    if (wc_curve25519_shared_secret_ex(&privkey, &pubkey,
                                       shared, sharedSz,
                                       EC25519_LITTLE_ENDIAN) != MP_OKAY)
        WOLFSSL_MSG("wc_curve25519_shared_secret_ex failed");
    else
        ret = WOLFSSL_SUCCESS;

    wc_curve25519_free(&privkey);
    wc_curve25519_free(&pubkey);

    return ret;
#endif /* WOLFSSL_KEY_GEN */
}
#endif /* OPENSSL_EXTRA && HAVE_CURVE25519 */

#if defined(OPENSSL_EXTRA) && defined(HAVE_ED25519)
/* return 1 if success, 0 if error
 * output keys are little endian format
 */
int wolfSSL_ED25519_generate_key(unsigned char *priv, unsigned int *privSz,
                                 unsigned char *pub, unsigned int *pubSz)
{
#ifndef WOLFSSL_KEY_GEN
    WOLFSSL_MSG("No Key Gen built in");
    (void) priv;
    (void) privSz;
    (void) pub;
    (void) pubSz;
    return WOLFSSL_FAILURE;
#else /* WOLFSSL_KEY_GEN */
    int ret = WOLFSSL_FAILURE;
    int initTmpRng = 0;
    WC_RNG *rng = NULL;
#ifdef WOLFSSL_SMALL_STACK
    WC_RNG *tmpRNG = NULL;
#else
    WC_RNG tmpRNG[1];
#endif

    WOLFSSL_ENTER("wolfSSL_ED25519_generate_key");

    if (priv == NULL || privSz == NULL || *privSz < ED25519_PRV_KEY_SIZE ||
        pub == NULL || pubSz == NULL || *pubSz < ED25519_PUB_KEY_SIZE) {
        WOLFSSL_MSG("Bad arguments");
        return WOLFSSL_FAILURE;
    }

#ifdef WOLFSSL_SMALL_STACK
    tmpRNG = (WC_RNG*)XMALLOC(sizeof(WC_RNG), NULL, DYNAMIC_TYPE_RNG);
    if (tmpRNG == NULL)
        return WOLFSSL_FATAL_ERROR;
#endif
    if (wc_InitRng(tmpRNG) == 0) {
        rng = tmpRNG;
        initTmpRng = 1;
    }
    else {
        WOLFSSL_MSG("Bad RNG Init, trying global");
        if (initGlobalRNG == 0)
            WOLFSSL_MSG("Global RNG no Init");
        else
            rng = &globalRNG;
    }

    if (rng) {
        ed25519_key key;

        if (wc_ed25519_init(&key) != MP_OKAY)
            WOLFSSL_MSG("wc_ed25519_init failed");
        else if (wc_ed25519_make_key(rng, ED25519_KEY_SIZE, &key)!=MP_OKAY)
            WOLFSSL_MSG("wc_ed25519_make_key failed");
        /* export private key */
        else if (wc_ed25519_export_key(&key, priv, privSz, pub, pubSz)!=MP_OKAY)
            WOLFSSL_MSG("wc_ed25519_export_key failed");
        else
            ret = WOLFSSL_SUCCESS;

        wc_ed25519_free(&key);
    }

    if (initTmpRng)
        wc_FreeRng(tmpRNG);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(tmpRNG, NULL, DYNAMIC_TYPE_RNG);
#endif

    return ret;
#endif /* WOLFSSL_KEY_GEN */
}

/* return 1 if success, 0 if error
 * input and output keys are little endian format
 * priv is a buffer containing private and public part of key
 */
int wolfSSL_ED25519_sign(const unsigned char *msg, unsigned int msgSz,
                         const unsigned char *priv, unsigned int privSz,
                         unsigned char *sig, unsigned int *sigSz)
{
#ifndef WOLFSSL_KEY_GEN
    WOLFSSL_MSG("No Key Gen built in");
    (void) msg;
    (void) msgSz;
    (void) priv;
    (void) privSz;
    (void) sig;
    (void) sigSz;
    return WOLFSSL_FAILURE;
#else /* WOLFSSL_KEY_GEN */
    ed25519_key key;
    int ret = WOLFSSL_FAILURE;

    WOLFSSL_ENTER("wolfSSL_ED25519_sign");

    if (priv == NULL || privSz != ED25519_PRV_KEY_SIZE ||
        msg == NULL || sig == NULL || *sigSz < ED25519_SIG_SIZE) {
        WOLFSSL_MSG("Bad arguments");
        return WOLFSSL_FAILURE;
    }

    /* import key */
    if (wc_ed25519_init(&key) != MP_OKAY) {
        WOLFSSL_MSG("wc_curve25519_init failed");
        return ret;
    }
    if (wc_ed25519_import_private_key(priv, privSz/2,
                                      priv+(privSz/2), ED25519_PUB_KEY_SIZE,
                                      &key) != MP_OKAY){
        WOLFSSL_MSG("wc_ed25519_import_private failed");
        wc_ed25519_free(&key);
        return ret;
    }

    if (wc_ed25519_sign_msg(msg, msgSz, sig, sigSz, &key) != MP_OKAY)
        WOLFSSL_MSG("wc_curve25519_shared_secret_ex failed");
    else
        ret = WOLFSSL_SUCCESS;

    wc_ed25519_free(&key);

    return ret;
#endif /* WOLFSSL_KEY_GEN */
}

/* return 1 if success, 0 if error
 * input and output keys are little endian format
 * pub is a buffer containing public part of key
 */
int wolfSSL_ED25519_verify(const unsigned char *msg, unsigned int msgSz,
                           const unsigned char *pub, unsigned int pubSz,
                           const unsigned char *sig, unsigned int sigSz)
{
#ifndef WOLFSSL_KEY_GEN
    WOLFSSL_MSG("No Key Gen built in");
    (void) msg;
    (void) msgSz;
    (void) pub;
    (void) pubSz;
    (void) sig;
    (void) sigSz;
    return WOLFSSL_FAILURE;
#else /* WOLFSSL_KEY_GEN */
    ed25519_key key;
    int ret = WOLFSSL_FAILURE, check = 0;

    WOLFSSL_ENTER("wolfSSL_ED25519_verify");

    if (pub == NULL || pubSz != ED25519_PUB_KEY_SIZE ||
        msg == NULL || sig == NULL || sigSz != ED25519_SIG_SIZE) {
        WOLFSSL_MSG("Bad arguments");
        return WOLFSSL_FAILURE;
    }

    /* import key */
    if (wc_ed25519_init(&key) != MP_OKAY) {
        WOLFSSL_MSG("wc_curve25519_init failed");
        return ret;
    }
    if (wc_ed25519_import_public(pub, pubSz, &key) != MP_OKAY){
        WOLFSSL_MSG("wc_ed25519_import_public failed");
        wc_ed25519_free(&key);
        return ret;
    }

    if ((ret = wc_ed25519_verify_msg((byte*)sig, sigSz, msg, msgSz,
                                     &check, &key)) != MP_OKAY) {
        WOLFSSL_MSG("wc_ed25519_verify_msg failed");
    }
    else if (!check)
        WOLFSSL_MSG("wc_ed25519_verify_msg failed (signature invalid)");
    else
        ret = WOLFSSL_SUCCESS;

    wc_ed25519_free(&key);

    return ret;
#endif /* WOLFSSL_KEY_GEN */
}

#endif /* OPENSSL_EXTRA && HAVE_ED25519 */

#ifdef WOLFSSL_JNI

int wolfSSL_set_jobject(WOLFSSL* ssl, void* objPtr)
{
    WOLFSSL_ENTER("wolfSSL_set_jobject");
    if (ssl != NULL)
    {
        ssl->jObjectRef = objPtr;
        return WOLFSSL_SUCCESS;
    }
    return WOLFSSL_FAILURE;
}

void* wolfSSL_get_jobject(WOLFSSL* ssl)
{
    WOLFSSL_ENTER("wolfSSL_get_jobject");
    if (ssl != NULL)
        return ssl->jObjectRef;
    return NULL;
}

#endif /* WOLFSSL_JNI */


#ifdef WOLFSSL_ASYNC_CRYPT
int wolfSSL_CTX_AsyncPoll(WOLFSSL_CTX* ctx, WOLF_EVENT** events, int maxEvents,
    WOLF_EVENT_FLAG flags, int* eventCount)
{
    if (ctx == NULL) {
        return BAD_FUNC_ARG;
    }

    return wolfAsync_EventQueuePoll(&ctx->event_queue, NULL,
                                        events, maxEvents, flags, eventCount);
}

int wolfSSL_AsyncPoll(WOLFSSL* ssl, WOLF_EVENT_FLAG flags)
{
    int ret, eventCount = 0;
    WOLF_EVENT* events[1];

    if (ssl == NULL) {
        return BAD_FUNC_ARG;
    }

    ret = wolfAsync_EventQueuePoll(&ssl->ctx->event_queue, ssl,
        events, sizeof(events)/sizeof(events), flags, &eventCount);
    if (ret == 0) {
        ret = eventCount;
    }

    return ret;
}

#endif /* WOLFSSL_ASYNC_CRYPT */

#ifdef OPENSSL_EXTRA
unsigned long wolfSSL_ERR_peek_error_line_data(const char **file, int *line,
                                               const char **data, int *flags)
{
    WOLFSSL_ENTER("wolfSSL_ERR_peek_error_line_data");

    (void)line;
    (void)file;

    /* No data or flags stored - error display only in Nginx. */
    if (data != NULL) {
        *data = "";
    }
    if (flags != NULL) {
        *flags = 0;
    }

#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || \
    defined(WOLFSSL_HAPROXY) || defined(WOLFSSL_MYSQL_COMPATIBLE)
    {
        int ret = 0;

        while (1) {
            if ((ret = wc_PeekErrorNode(-1, file, NULL, line)) < 0) {
                WOLFSSL_MSG("Issue peeking at error node in queue");
                return 0;
            }
            ret = -ret;

            if (ret == ASN_NO_PEM_HEADER)
                return (ERR_LIB_PEM << 24) | PEM_R_NO_START_LINE;
            if (ret != WANT_READ && ret != WANT_WRITE &&
                    ret != ZERO_RETURN && ret != WOLFSSL_ERROR_ZERO_RETURN &&
                    ret != SOCKET_PEER_CLOSED_E && ret != SOCKET_ERROR_E)
                break;

            wc_RemoveErrorNode(-1);
        }

        return (unsigned long)ret;
    }
#else
    return (unsigned long)(0 - NOT_COMPILED_IN);
#endif
}
#endif

#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)

#ifndef NO_WOLFSSL_STUB
WOLF_STACK_OF(WOLFSSL_CIPHER) *wolfSSL_get_ciphers_compat(const WOLFSSL *ssl)
{
    (void)ssl;
    WOLFSSL_STUB("wolfSSL_get_ciphers_compat");
    return NULL;
}
#endif

#ifndef NO_WOLFSSL_STUB
void wolfSSL_OPENSSL_config(char *config_name)
{
    (void)config_name;
    WOLFSSL_STUB("OPENSSL_config");
}
#endif
#endif

#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY) \
    || defined(OPENSSL_EXTRA)
int wolfSSL_X509_get_ex_new_index(int idx, void *arg, void *a, void *b, void *c)
{
    static int x509_idx = 0;

    WOLFSSL_ENTER("wolfSSL_X509_get_ex_new_index");
    (void)idx;
    (void)arg;
    (void)a;
    (void)b;
    (void)c;

    return x509_idx++;
}

void *wolfSSL_X509_get_ex_data(X509 *x509, int idx)
{
    WOLFSSL_ENTER("wolfSSL_X509_get_ex_data");
    #ifdef HAVE_EX_DATA
    if (x509 != NULL && idx < MAX_EX_DATA && idx >= 0) {
        return x509->ex_data[idx];
    }
    #else
    (void)x509;
    (void)idx;
    #endif
    return NULL;
}
int wolfSSL_X509_set_ex_data(X509 *x509, int idx, void *data)
{
    WOLFSSL_ENTER("wolfSSL_X509_set_ex_data");
    #ifdef HAVE_EX_DATA
    if (x509 != NULL && idx < MAX_EX_DATA)
    {
        x509->ex_data[idx] = data;
        return WOLFSSL_SUCCESS;
    }
    #else
    (void)x509;
    (void)idx;
    (void)data;
    #endif
    return WOLFSSL_FAILURE;
}
int wolfSSL_X509_NAME_digest(const WOLFSSL_X509_NAME *name,
        const WOLFSSL_EVP_MD *type, unsigned char *md, unsigned int *len)
{
    WOLFSSL_ENTER("wolfSSL_X509_NAME_digest");

    if (name == NULL || type == NULL)
        return WOLFSSL_FAILURE;

#ifndef NO_FILESYSTEM
    return wolfSSL_EVP_Digest((unsigned char*)name->fullName.fullName,
                              name->fullName.fullNameLen, md, len, type, NULL);
#else
    (void)md;
    (void)len;
    return NOT_COMPILED_IN;
#endif
}

long wolfSSL_SSL_CTX_get_timeout(const WOLFSSL_CTX *ctx)
{
    WOLFSSL_ENTER("wolfSSL_SSL_CTX_get_timeout");

    if (ctx == NULL)
        return 0;

    return ctx->timeout;
}

#ifdef HAVE_ECC
int wolfSSL_SSL_CTX_set_tmp_ecdh(WOLFSSL_CTX *ctx, WOLFSSL_EC_KEY *ecdh)
{
    WOLFSSL_ENTER("wolfSSL_SSL_CTX_set_tmp_ecdh");

    if (ctx == NULL || ecdh == NULL)
        return BAD_FUNC_ARG;

    ctx->ecdhCurveOID = ecdh->group->curve_oid;

    return WOLFSSL_SUCCESS;
}
#endif

/* Assumes that the session passed in is from the cache. */
int wolfSSL_SSL_CTX_remove_session(WOLFSSL_CTX *ctx, WOLFSSL_SESSION *s)
{
    WOLFSSL_ENTER("wolfSSL_SSL_CTX_remove_session");

    if (ctx == NULL || s == NULL)
        return BAD_FUNC_ARG;

#ifdef HAVE_EXT_CACHE
    if (!ctx->internalCacheOff)
#endif
    {
        /* Don't remove session just timeout session. */
        s->timeout = 0;
    }

#ifdef HAVE_EXT_CACHE
    if (ctx->rem_sess_cb != NULL)
        ctx->rem_sess_cb(ctx, s);
#endif

    return 0;
}

BIO *wolfSSL_SSL_get_rbio(const WOLFSSL *s)
{
    WOLFSSL_ENTER("wolfSSL_SSL_get_rbio");
    (void)s;
    /* Nginx sets the buffer size if the read BIO is different to write BIO.
     * The setting buffer size doesn't do anything so return NULL for both.
     */
    return NULL;
}
BIO *wolfSSL_SSL_get_wbio(const WOLFSSL *s)
{
    WOLFSSL_ENTER("wolfSSL_SSL_get_wbio");
    (void)s;
    /* Nginx sets the buffer size if the read BIO is different to write BIO.
     * The setting buffer size doesn't do anything so return NULL for both.
     */
    return NULL;
}

int wolfSSL_SSL_do_handshake(WOLFSSL *s)
{
    WOLFSSL_ENTER("wolfSSL_SSL_do_handshake");

    if (s == NULL)
        return WOLFSSL_FAILURE;

    if (s->options.side == WOLFSSL_CLIENT_END) {
    #ifndef NO_WOLFSSL_CLIENT
        return wolfSSL_connect(s);
    #else
        WOLFSSL_MSG("Client not compiled in");
        return WOLFSSL_FAILURE;
    #endif
    }

#ifndef NO_WOLFSSL_SERVER
    return wolfSSL_accept(s);
#else
    WOLFSSL_MSG("Server not compiled in");
    return WOLFSSL_FAILURE;
#endif
}

int wolfSSL_SSL_in_init(WOLFSSL *s)
{
    WOLFSSL_ENTER("wolfSSL_SSL_in_init");

    if (s == NULL)
        return WOLFSSL_FAILURE;

    if (s->options.side == WOLFSSL_CLIENT_END)
        return s->options.connectState < SECOND_REPLY_DONE;
    return s->options.acceptState < ACCEPT_THIRD_REPLY_DONE;
}

#ifndef NO_SESSION_CACHE

WOLFSSL_SESSION *wolfSSL_SSL_get0_session(const WOLFSSL *ssl)
{
    WOLFSSL_SESSION *session;

    WOLFSSL_ENTER("wolfSSL_SSL_get0_session");

    if (ssl == NULL) {
        return NULL;
    }

    session = wolfSSL_get_session((WOLFSSL*)ssl);

#ifdef HAVE_EXT_CACHE
    ((WOLFSSL*)ssl)->extSession = session;
#endif

    return session;
}

#endif /* NO_SESSION_CACHE */

int wolfSSL_X509_check_host(X509 *x, const char *chk, size_t chklen,
                    unsigned int flags, char **peername)
{
    int         ret;
    DecodedCert dCert;

    WOLFSSL_ENTER("wolfSSL_X509_check_host");

    /* flags and peername not needed for Nginx. */
    (void)flags;
    (void)peername;

    if (flags == WOLFSSL_NO_WILDCARDS) {
        WOLFSSL_MSG("X509_CHECK_FLAG_NO_WILDCARDS not yet implemented");
        return WOLFSSL_FAILURE;
    }

    InitDecodedCert(&dCert, x->derCert->buffer, x->derCert->length, NULL);
    ret = ParseCertRelative(&dCert, CERT_TYPE, 0, NULL);
    if (ret != 0)
        return WOLFSSL_FAILURE;

    ret = CheckHostName(&dCert, (char *)chk, chklen);
    FreeDecodedCert(&dCert);
    if (ret != 0)
        return WOLFSSL_FAILURE;
    return WOLFSSL_SUCCESS;
}

int wolfSSL_i2a_ASN1_INTEGER(BIO *bp, const WOLFSSL_ASN1_INTEGER *a)
{
    static char num[16] = { '0', '1', '2', '3', '4', '5', '6', '7',
                            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
    int    i;
    word32 j;
    word32 len = 0;

    WOLFSSL_ENTER("wolfSSL_i2a_ASN1_INTEGER");

    if (bp == NULL || a == NULL)
        return WOLFSSL_FAILURE;

    /* Skip ASN.1 INTEGER (type) byte. */
    i = 1;
    /* When indefinte length, can't determine length with data available. */
    if (a->data[i] == 0x80)
        return 0;
    /* One length byte if less than 0x80. */
    if (a->data[i] < 0x80)
        len = a->data[i++];
    /* Multiple length byte if greater than 0x80. */
    else if (a->data[i] > 0x80) {
        switch (a->data[i++] - 0x80) {
            case 4:
                len |= a->data[i++] << 24;
                FALL_THROUGH;
            case 3:
                len |= a->data[i++] << 16;
                FALL_THROUGH;
            case 2:
                len |= a->data[i++] <<  8;
                FALL_THROUGH;
            case 1:
                len |= a->data[i++];
                break;
            default:
                /* Not supporting greater than 4 bytes of length. */
                return 0;
        }
    }

    /* Zero length integer is the value zero. */
    if (len == 0) {
        wolfSSL_BIO_write(bp, "00", 2);
        return 2;
    }

    /* Don't do negative - just write out every byte. */
    for (j = 0; j < len; i++,j++) {
        wolfSSL_BIO_write(bp, &num[a->data[i] >> 4], 1);
        wolfSSL_BIO_write(bp, &num[a->data[i] & 0xf], 1);
    }

    /* Two nibbles written for each byte. */
    return len * 2;
}


#if defined(HAVE_SESSION_TICKET) && !defined(NO_WOLFSSL_SERVER)
/* Expected return values from implementations of OpenSSL ticket key callback.
 */
#define TICKET_KEY_CB_RET_FAILURE    -1
#define TICKET_KEY_CB_RET_NOT_FOUND   0
#define TICKET_KEY_CB_RET_OK          1
#define TICKET_KEY_CB_RET_RENEW       2

/* The ticket key callback as used in OpenSSL is stored here. */
static int (*ticketKeyCb)(WOLFSSL *ssl, unsigned char *name, unsigned char *iv,
    WOLFSSL_EVP_CIPHER_CTX *ectx, WOLFSSL_HMAC_CTX *hctx, int enc) = NULL;

/* Implementation of session ticket encryption/decryption using OpenSSL
 * callback to initialize the cipher and HMAC.
 *
 * ssl           The SSL/TLS object.
 * keyName       The key name - used to identify the key to be used.
 * iv            The IV to use.
 * mac           The MAC of the encrypted data.
 * enc           Encrypt ticket.
 * encTicket     The ticket data.
 * encTicketLen  The length of the ticket data.
 * encLen        The encrypted/decrypted ticket length - output length.
 * ctx           Ignored. Application specific data.
 * returns WOLFSSL_TICKET_RET_OK to indicate success,
 *         WOLFSSL_TICKET_RET_CREATE if a new ticket is required and
 *         WOLFSSL_TICKET_RET_FATAL on error.
 */
static int wolfSSL_TicketKeyCb(WOLFSSL* ssl,
                                  unsigned char keyName[WOLFSSL_TICKET_NAME_SZ],
                                  unsigned char iv[WOLFSSL_TICKET_IV_SZ],
                                  unsigned char mac[WOLFSSL_TICKET_MAC_SZ],
                                  int enc, unsigned char* encTicket,
                                  int encTicketLen, int* encLen, void* ctx)
{
    byte                    digest[WC_MAX_DIGEST_SIZE];
    WOLFSSL_EVP_CIPHER_CTX  evpCtx;
    WOLFSSL_HMAC_CTX        hmacCtx;
    unsigned int            mdSz = 0;
    int                     len = 0;
    int                     ret = WOLFSSL_TICKET_RET_FATAL;
    int                     res;

    (void)ctx;

    if (ticketKeyCb == NULL)
        return WOLFSSL_TICKET_RET_FATAL;

    wolfSSL_EVP_CIPHER_CTX_init(&evpCtx);
    /* Initialize the cipher and HMAC. */
    res = ticketKeyCb(ssl, keyName, iv, &evpCtx, &hmacCtx, enc);
    if (res != TICKET_KEY_CB_RET_OK && res != TICKET_KEY_CB_RET_RENEW)
        return WOLFSSL_TICKET_RET_FATAL;

    if (enc)
    {
        /* Encrypt in place. */
        if (!wolfSSL_EVP_CipherUpdate(&evpCtx, encTicket, &len,
                                      encTicket, encTicketLen))
            goto end;
        encTicketLen = len;
        if (!wolfSSL_EVP_EncryptFinal(&evpCtx, &encTicket[encTicketLen], &len))
            goto end;
        /* Total length of encrypted data. */
        encTicketLen += len;
        *encLen = encTicketLen;

        /* HMAC the encrypted data into the parameter 'mac'. */
        if (!wolfSSL_HMAC_Update(&hmacCtx, encTicket, encTicketLen))
            goto end;
#ifdef WOLFSSL_SHA512
        /* Check for SHA512, which would overrun the mac buffer */
        if (hmacCtx.hmac.macType == WC_SHA512)
            goto end;
#endif
        if (!wolfSSL_HMAC_Final(&hmacCtx, mac, &mdSz))
            goto end;
    }
    else
    {
        /* HMAC the encrypted data and compare it to the passed in data. */
        if (!wolfSSL_HMAC_Update(&hmacCtx, encTicket, encTicketLen))
            goto end;
        if (!wolfSSL_HMAC_Final(&hmacCtx, digest, &mdSz))
            goto end;
        if (XMEMCMP(mac, digest, mdSz) != 0)
            goto end;

        /* Decrypt the ticket data in place. */
        if (!wolfSSL_EVP_CipherUpdate(&evpCtx, encTicket, &len,
                                      encTicket, encTicketLen))
            goto end;
        encTicketLen = len;
        if (!wolfSSL_EVP_DecryptFinal(&evpCtx, &encTicket[encTicketLen], &len))
            goto end;
        /* Total length of decrypted data. */
        *encLen = encTicketLen + len;
    }

    ret = (res == TICKET_KEY_CB_RET_RENEW) ? WOLFSSL_TICKET_RET_CREATE :
                                             WOLFSSL_TICKET_RET_OK;
end:
    return ret;
}

/* Set the callback to use when encrypting/decrypting tickets.
 *
 * ctx  The SSL/TLS context object.
 * cb   The OpenSSL session ticket callback.
 * returns WOLFSSL_SUCCESS to indicate success.
 */
int wolfSSL_CTX_set_tlsext_ticket_key_cb(WOLFSSL_CTX *ctx, int (*cb)(
    WOLFSSL *ssl, unsigned char *name, unsigned char *iv,
    WOLFSSL_EVP_CIPHER_CTX *ectx, WOLFSSL_HMAC_CTX *hctx, int enc))
{
    /* Store callback in a global. */
    ticketKeyCb = cb;
    /* Set the ticket encryption callback to be a wrapper around OpenSSL
     * callback.
     */
    ctx->ticketEncCb = wolfSSL_TicketKeyCb;

    return WOLFSSL_SUCCESS;
}
#endif /* HAVE_SESSION_TICKET */

#endif /* WOLFSSL_NGINX || WOLFSSL_HAPROXY || OPENSSL_EXTRA */

#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
#ifdef HAVE_OCSP
/* Not an OpenSSL API. */
int wolfSSL_get_ocsp_response(WOLFSSL* ssl, byte** response)
{
    *response = ssl->ocspResp;
    return ssl->ocspRespSz;
}

/* Not an OpenSSL API. */
char* wolfSSL_get_ocsp_url(WOLFSSL* ssl)
{
    return ssl->url;
}

/* Not an OpenSSL API. */
int wolfSSL_set_ocsp_url(WOLFSSL* ssl, char* url)
{
    if (ssl == NULL)
        return WOLFSSL_FAILURE;

    ssl->url = url;
    return WOLFSSL_SUCCESS;
}
#endif /* WOLFSSL_NGINX || WOLFSSL_HAPROXY */

#if defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY) || defined(OPENSSL_EXTRA)
int wolfSSL_CTX_get_extra_chain_certs(WOLFSSL_CTX* ctx, WOLF_STACK_OF(X509)** chain)
{
    word32         idx;
    word32         length;
    WOLFSSL_STACK* node;
    WOLFSSL_STACK* last = NULL;

    if (ctx == NULL || chain == NULL) {
        chain = NULL;
        return WOLFSSL_FAILURE;
    }
    if (ctx->x509Chain != NULL) {
        *chain = ctx->x509Chain;
        return WOLFSSL_SUCCESS;
    }

    /* If there are no chains then success! */
    *chain = NULL;
    if (ctx->certChain == NULL || ctx->certChain->length == 0) {
        return WOLFSSL_SUCCESS;
    }

    /* Create a new stack of WOLFSSL_X509 object from chain buffer. */
    for (idx = 0; idx < ctx->certChain->length; ) {
        node = (WOLFSSL_STACK*)XMALLOC(sizeof(WOLFSSL_STACK), NULL,
                                       DYNAMIC_TYPE_OPENSSL);
        if (node == NULL)
            return WOLFSSL_FAILURE;
        node->next = NULL;

        /* 3 byte length | X509 DER data */
        ato24(ctx->certChain->buffer + idx, &length);
        idx += 3;

        /* Create a new X509 from DER encoded data. */
        node->data.x509 = wolfSSL_X509_d2i(NULL, ctx->certChain->buffer + idx,
            length);
        if (node->data.x509 == NULL) {
            XFREE(node, NULL, DYNAMIC_TYPE_OPENSSL);
            /* Return as much of the chain as we created. */
            ctx->x509Chain = *chain;
            return WOLFSSL_FAILURE;
        }
        idx += length;

        /* Add object to the end of the stack. */
        if (last == NULL) {
            node->num = 1;
            *chain = node;
        }
        else {
            (*chain)->num++;
            last->next = node;
        }

        last = node;
    }

    ctx->x509Chain = *chain;

    return WOLFSSL_SUCCESS;
}

int wolfSSL_CTX_set_tlsext_status_cb(WOLFSSL_CTX* ctx,
    int(*cb)(WOLFSSL*, void*))
{
    if (ctx == NULL || ctx->cm == NULL)
        return WOLFSSL_FAILURE;

#if defined(HAVE_CERTIFICATE_STATUS_REQUEST) \
 || defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
    /* Ensure stapling is on for callback to be used. */
    wolfSSL_CTX_EnableOCSPStapling(ctx);

    if (ctx->cm->ocsp_stapling == NULL)
        return WOLFSSL_FAILURE;

    ctx->cm->ocsp_stapling->statusCb = cb;
#else
    (void)cb;
#endif

    return WOLFSSL_SUCCESS;
}

int wolfSSL_X509_STORE_CTX_get1_issuer(WOLFSSL_X509 **issuer,
    WOLFSSL_X509_STORE_CTX *ctx, WOLFSSL_X509 *x)
{
    WOLFSSL_STACK* node;
    Signer* ca = NULL;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert* cert = NULL;
#else
    DecodedCert  cert[1];
#endif

    if (issuer == NULL || ctx == NULL || x == NULL)
        return WOLFSSL_FATAL_ERROR;

    if (ctx->chain != NULL) {
        for (node = ctx->chain; node != NULL; node = node->next) {
            if (wolfSSL_X509_check_issued(node->data.x509, x) == X509_V_OK) {
                *issuer = x;
                return WOLFSSL_SUCCESS;
            }
        }
    }


#ifdef WOLFSSL_SMALL_STACK
    cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), NULL, DYNAMIC_TYPE_DCERT);
    if (cert == NULL)
        return WOLFSSL_FAILURE;
#endif

    /* Use existing CA retrieval APIs that use DecodedCert. */
    InitDecodedCert(cert, x->derCert->buffer, x->derCert->length, NULL);
    if (ParseCertRelative(cert, CERT_TYPE, 0, NULL) == 0) {
    #ifndef NO_SKID
        if (cert->extAuthKeyIdSet)
            ca = GetCA(ctx->store->cm, cert->extAuthKeyId);
        if (ca == NULL)
            ca = GetCAByName(ctx->store->cm, cert->issuerHash);
    #else /* NO_SKID */
        ca = GetCA(ctx->store->cm, cert->issuerHash);
    #endif /* NO SKID */
    }
    FreeDecodedCert(cert);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
#endif

    if (ca == NULL)
        return WOLFSSL_FAILURE;

    *issuer = (WOLFSSL_X509 *)XMALLOC(sizeof(WOLFSSL_X509), 0,
        DYNAMIC_TYPE_OPENSSL);
    if (*issuer == NULL)
        return WOLFSSL_FAILURE;

    /* Create an empty certificate as CA doesn't have a certificate. */
    XMEMSET(*issuer, 0, sizeof(WOLFSSL_X509));
    /* TODO: store the full certificate and dup when required. */

    /* Result is ignored when passed to wolfSSL_OCSP_cert_to_id(). */

    return WOLFSSL_SUCCESS;
}

void wolfSSL_X509_email_free(WOLF_STACK_OF(WOLFSSL_STRING) *sk)
{
    WOLFSSL_STACK *curr;

    while (sk != NULL) {
        curr = sk;
        sk = sk->next;

        XFREE(curr, NULL, DYNAMIC_TYPE_OPENSSL);
    }
}

WOLF_STACK_OF(WOLFSSL_STRING) *wolfSSL_X509_get1_ocsp(WOLFSSL_X509 *x)
{
    WOLFSSL_STACK *list = NULL;

    if (x->authInfoSz == 0)
        return NULL;

    list = (WOLFSSL_STACK*)XMALLOC(sizeof(WOLFSSL_STACK), NULL,
                                   DYNAMIC_TYPE_OPENSSL);
    if (list == NULL)
        return NULL;

    list->data.string = (char*)x->authInfo;
    list->next = NULL;

    return list;
}

int wolfSSL_X509_check_issued(WOLFSSL_X509 *issuer, WOLFSSL_X509 *subject)
{
    WOLFSSL_X509_NAME *issuerName = wolfSSL_X509_get_issuer_name(subject);
    WOLFSSL_X509_NAME *subjectName = wolfSSL_X509_get_subject_name(issuer);

    if (issuerName == NULL || subjectName == NULL)
        return X509_V_ERR_SUBJECT_ISSUER_MISMATCH;

    /* Literal matching of encoded names and key ids. */
    if (issuerName->sz != subjectName->sz ||
           XMEMCMP(issuerName->name, subjectName->name, subjectName->sz) != 0) {
        return X509_V_ERR_SUBJECT_ISSUER_MISMATCH;
    }

    if (subject->authKeyId != NULL && issuer->subjKeyId != NULL) {
        if (subject->authKeyIdSz != issuer->subjKeyIdSz ||
                XMEMCMP(subject->authKeyId, issuer->subjKeyId,
                        issuer->subjKeyIdSz) != 0) {
            return X509_V_ERR_SUBJECT_ISSUER_MISMATCH;
        }
    }

    return X509_V_OK;
}

WOLFSSL_X509* wolfSSL_X509_dup(WOLFSSL_X509 *x)
{
    return wolfSSL_X509_d2i(NULL, x->derCert->buffer, x->derCert->length);
}

char* wolfSSL_sk_WOLFSSL_STRING_value(WOLF_STACK_OF(WOLFSSL_STRING)* strings,
    int idx)
{
    for (; idx > 0 && strings != NULL; idx--)
        strings = strings->next;
    if (strings == NULL)
        return NULL;
    return strings->data.string;
}
#endif /* HAVE_OCSP */

#ifdef HAVE_ALPN
void wolfSSL_get0_alpn_selected(const WOLFSSL *ssl, const unsigned char **data,
                                unsigned int *len)
{
    word16 nameLen;

    if (ssl != NULL && data != NULL && len != NULL) {
        TLSX_ALPN_GetRequest(ssl->extensions, (void **)data, &nameLen);
        *len = nameLen;
    }
}

int wolfSSL_select_next_proto(unsigned char **out, unsigned char *outLen,
                              const unsigned char *in, unsigned int inLen,
                              const unsigned char *clientNames,
                              unsigned int clientLen)
{
    unsigned int i, j;
    byte lenIn, lenClient;

    if (out == NULL || outLen == NULL || in == NULL || clientNames == NULL)
        return OPENSSL_NPN_UNSUPPORTED;

    for (i = 0; i < inLen; i += lenIn) {
        lenIn = in[i++];
        for (j = 0; j < clientLen; j += lenClient) {
            lenClient = clientNames[j++];

            if (lenIn != lenClient)
                continue;

            if (XMEMCMP(in + i, clientNames + j, lenIn) == 0) {
                *out = (unsigned char *)(in + i);
                *outLen = lenIn;
                return OPENSSL_NPN_NEGOTIATED;
            }
        }
    }

    *out = (unsigned char *)clientNames + 1;
    *outLen = clientNames[0];
    return OPENSSL_NPN_NO_OVERLAP;
}

void wolfSSL_CTX_set_alpn_select_cb(WOLFSSL_CTX *ctx,
                                    int (*cb) (WOLFSSL *ssl,
                                               const unsigned char **out,
                                               unsigned char *outlen,
                                               const unsigned char *in,
                                               unsigned int inlen,
                                               void *arg), void *arg)
{
    if (ctx != NULL) {
        ctx->alpnSelect = cb;
        ctx->alpnSelectArg = arg;
    }
}

void wolfSSL_CTX_set_next_protos_advertised_cb(WOLFSSL_CTX *s,
                                           int (*cb) (WOLFSSL *ssl,
                                                      const unsigned char
                                                      **out,
                                                      unsigned int *outlen,
                                                      void *arg), void *arg)
{
    (void)s;
    (void)cb;
    (void)arg;
    WOLFSSL_STUB("wolfSSL_CTX_set_next_protos_advertised_cb");
}

void wolfSSL_CTX_set_next_proto_select_cb(WOLFSSL_CTX *s,
                                      int (*cb) (WOLFSSL *ssl,
                                                 unsigned char **out,
                                                 unsigned char *outlen,
                                                 const unsigned char *in,
                                                 unsigned int inlen,
                                                 void *arg), void *arg)
{
    (void)s;
    (void)cb;
    (void)arg;
    WOLFSSL_STUB("wolfSSL_CTX_set_next_proto_select_cb");
}

void wolfSSL_get0_next_proto_negotiated(const WOLFSSL *s, const unsigned char **data,
                                    unsigned *len)
{
    (void)s;
    (void)data;
    (void)len;
    WOLFSSL_STUB("wolfSSL_get0_next_proto_negotiated");
}
#endif /* HAVE_ALPN */

#endif /* WOLFSSL_NGINX  / WOLFSSL_HAPROXY */

#if defined(OPENSSL_EXTRA) && defined(HAVE_ECC)
WOLFSSL_API int wolfSSL_CTX_set1_curves_list(WOLFSSL_CTX* ctx, char* names)
{
    int idx, start = 0, len;
    int curve;
    char name[MAX_CURVE_NAME_SZ];

    /* Disable all curves so that only the ones the user wants are enabled. */
    ctx->disabledCurves = (word32)-1;
    for (idx = 1; names[idx-1] != '\0'; idx++) {
        if (names[idx] != ':' && names[idx] != '\0')
            continue;

        len = idx - 1 - start;
        if (len > MAX_CURVE_NAME_SZ - 1)
            return WOLFSSL_FAILURE;

        XMEMCPY(name, names + start, len);
        name[len] = 0;

        if ((XSTRNCMP(name, "prime256v1", len) == 0) ||
                                      (XSTRNCMP(name, "secp256r1", len) == 0) ||
                                      (XSTRNCMP(name, "P-256", len) == 0)) {
            curve = WOLFSSL_ECC_SECP256R1;
        }
        else if ((XSTRNCMP(name, "secp384r1", len) == 0) ||
                                          (XSTRNCMP(name, "P-384", len) == 0)) {
            curve = WOLFSSL_ECC_SECP384R1;
        }
        else if ((XSTRNCMP(name, "secp521r1", len) == 0) ||
                                          (XSTRNCMP(name, "P-521", len) == 0)) {
            curve = WOLFSSL_ECC_SECP521R1;
        }
        else if (XSTRNCMP(name, "X25519", len) == 0)
            curve = WOLFSSL_ECC_X25519;
        else if ((curve = wc_ecc_get_curve_id_from_name(name)) < 0)
            return WOLFSSL_FAILURE;

        /* Switch the bit to off and therefore is enabled. */
        ctx->disabledCurves &= ~(1 << curve);
        start = idx + 1;
    }

    return WOLFSSL_SUCCESS;
}
#endif

#ifdef OPENSSL_EXTRA
#ifndef NO_WOLFSSL_STUB
int wolfSSL_CTX_set_msg_callback(WOLFSSL_CTX *ctx, SSL_Msg_Cb cb)
{
    WOLFSSL_STUB("SSL_CTX_set_msg_callback");
    (void)ctx;
    (void)cb;
    return WOLFSSL_FAILURE;
}
#endif


/* Sets a callback for when sending and receiving protocol messages.
 *
 * ssl WOLFSSL structure to set callback in
 * cb  callback to use
 *
 * return SSL_SUCCESS on success and SSL_FAILURE with error case
 */
int wolfSSL_set_msg_callback(WOLFSSL *ssl, SSL_Msg_Cb cb)
{
    WOLFSSL_ENTER("wolfSSL_set_msg_callback");

    if (ssl == NULL) {
        return SSL_FAILURE;
    }

    if (cb != NULL) {
        ssl->toInfoOn = 1;
    }

    ssl->protoMsgCb = cb;
    return SSL_SUCCESS;
}
#ifndef NO_WOLFSSL_STUB
int wolfSSL_CTX_set_msg_callback_arg(WOLFSSL_CTX *ctx, void* arg)
{
    WOLFSSL_STUB("SSL_CTX_set_msg_callback_arg");
    (void)ctx;
    (void)arg;
    return WOLFSSL_FAILURE;
}
#endif

int wolfSSL_set_msg_callback_arg(WOLFSSL *ssl, void* arg)
{
    WOLFSSL_ENTER("wolfSSL_set_msg_callback_arg");
    ssl->protoMsgCtx = arg;
    return WOLFSSL_SUCCESS;
}

void *wolfSSL_OPENSSL_memdup(const void *data, size_t siz, const char* file, int line)
{
    void *ret;
    (void)file;
    (void)line;

    if (data == NULL || siz >= INT_MAX)
        return NULL;

    ret = OPENSSL_malloc(siz);
    if (ret == NULL) {
        return NULL;
    }
    return XMEMCPY(ret, data, siz);
}

int wolfSSL_CTX_set_alpn_protos(WOLFSSL_CTX *ctx, const unsigned char *p,
                            unsigned int p_len)
{
    WOLFSSL_ENTER("wolfSSL_CTX_set_alpn_protos");
    if(ctx == NULL)
        return BAD_FUNC_ARG;
    if((void *)ctx->alpn_cli_protos != NULL)
        wolfSSL_OPENSSL_free((void *)ctx->alpn_cli_protos);
    ctx->alpn_cli_protos =
        (const unsigned char *)wolfSSL_OPENSSL_memdup(p, p_len, NULL, 0);
    if (ctx->alpn_cli_protos == NULL) {
        return SSL_FAILURE;
    }
    ctx->alpn_cli_protos_len = p_len;

    return SSL_SUCCESS;
}

#endif

#endif /* WOLFCRYPT_ONLY */

#if defined(OPENSSL_EXTRA)
int wolfSSL_X509_check_ca(WOLFSSL_X509 *x509)
{
    WOLFSSL_ENTER("X509_check_ca");

    if (x509 == NULL)
        return WOLFSSL_FAILURE;
    if (x509->isCa)
        return 1;
    if (x509->extKeyUsageCrit)
        return 4;

    return 0;
}


const char *wolfSSL_ASN1_tag2str(int tag)
{
    static const char *const tag_label[31] = {
        "EOC", "BOOLEAN", "INTEGER", "BIT STRING", "OCTET STRING", "NULL",
        "OBJECT", "OBJECT DESCRIPTOR", "EXTERNAL", "REAL", "ENUMERATED",
        "<ASN1 11>", "UTF8STRING", "<ASN1 13>", "<ASN1 14>", "<ASN1 15>",
        "SEQUENCE", "SET", "NUMERICSTRING", "PRINTABLESTRING", "T61STRING",
        "VIDEOTEXTSTRING", "IA5STRING", "UTCTIME", "GENERALIZEDTIME",
        "GRAPHICSTRING", "VISIBLESTRING", "GENERALSTRING", "UNIVERSALSTRING",
        "<ASN1 29>", "BMPSTRING"
    };

    if ((tag == V_ASN1_NEG_INTEGER) || (tag == V_ASN1_NEG_ENUMERATED))
        tag &= ~0x100;
    if (tag < 0 || tag > 30)
        return "(unknown)";
    return tag_label[tag];
}

static int check_esc_char(char c, char *esc)
{
    char *ptr = NULL;

    ptr = esc;
    while(*ptr != 0){
        if (c == *ptr)
            return 1;
        ptr++;
    }
    return 0;
}

int wolfSSL_ASN1_STRING_print_ex(WOLFSSL_BIO *out, WOLFSSL_ASN1_STRING *str, 
                                 unsigned long flags)
{
    size_t str_len = 0, type_len = 0;
    unsigned char *typebuf = NULL;
    const char *hash="#";

    WOLFSSL_ENTER("wolfSSL_ASN1_STRING_PRINT_ex");
    if (out == NULL || str == NULL)
        return WOLFSSL_FAILURE;

    /* add ASN1 type tag */
    if (flags & ASN1_STRFLGS_SHOW_TYPE){
        const char *tag = wolfSSL_ASN1_tag2str(str->type);
        /* colon len + tag len + null*/
        type_len = XSTRLEN(tag) + 2;
        typebuf = (unsigned char *)XMALLOC(type_len , NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (typebuf == NULL){
            WOLFSSL_MSG("memory alloc failed.");
            return WOLFSSL_FAILURE;
        }
        XMEMSET(typebuf, 0, type_len);
        XSNPRINTF((char*)typebuf, (size_t)type_len , "%s:", tag);
        type_len--;
    }

    /* dump hex */
    if (flags & ASN1_STRFLGS_DUMP_ALL){
        static const char hex_char[] = { '0', '1', '2', '3', '4', '5', '6',
                                         '7','8', '9', 'A', 'B', 'C', 'D',
                                         'E', 'F' };
        char hex_tmp[4];
        char *str_ptr, *str_end;
        
        if (type_len > 0){
            if (wolfSSL_BIO_write(out, typebuf, (int)type_len) != (int)type_len){
                XFREE(typebuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                return WOLFSSL_FAILURE;
            }
            str_len += type_len;
        }
        if (wolfSSL_BIO_write(out, hash, 1) != 1){
            goto err_exit;
        }
        str_len++;
        if (flags & ASN1_STRFLGS_DUMP_DER){
            hex_tmp[0] = hex_char[str->type >> 4];
            hex_tmp[1] = hex_char[str->type & 0xf];
            hex_tmp[2] = hex_char[str->length >> 4];
            hex_tmp[3] = hex_char[str->length & 0xf];
            if (wolfSSL_BIO_write(out, hex_tmp, 4) != 4){
                goto err_exit;
            }
            str_len += 4;
            XMEMSET(hex_tmp, 0, 4);
        }

        str_ptr = str->data;
        str_end = str->data + str->length; 
        while (str_ptr < str_end){
            hex_tmp[0] = hex_char[*str_ptr >> 4];
            hex_tmp[1] = hex_char[*str_ptr & 0xf];
            if (wolfSSL_BIO_write(out, hex_tmp, 2) != 2){
                goto err_exit;
            }
            str_ptr++;
            str_len += 2;
        }
        if (type_len > 0)
            XFREE(typebuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);

        return (int)str_len;
    }

    if (type_len > 0){
        if (wolfSSL_BIO_write(out, typebuf, (int)type_len) != (int)type_len){
            XFREE(typebuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            return WOLFSSL_FAILURE;
        }
        str_len += type_len;
    }

    if (flags & ASN1_STRFLGS_ESC_2253){
        char esc_ch[] = "+;<>\\";
        char* esc_ptr = NULL; 

        esc_ptr = str->data;
        while (*esc_ptr != 0){
            if (check_esc_char(*esc_ptr, esc_ch)){
                if (wolfSSL_BIO_write(out,"\\", 1) != 1)
                    goto err_exit; 
                str_len++;
            }
            if (wolfSSL_BIO_write(out, esc_ptr, 1) != 1)
                goto err_exit; 
            str_len++;
            esc_ptr++;
        }
        if (type_len > 0)
            XFREE(typebuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return (int)str_len;
    }

    if (wolfSSL_BIO_write(out, str->data, str->length) != str->length){
        goto err_exit;
    }
    str_len += str->length;
    if (type_len > 0)
        XFREE(typebuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    return (int)str_len;

err_exit:
    if (type_len > 0)
        XFREE(typebuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    return WOLFSSL_FAILURE;
}

#ifndef NO_ASN_TIME
WOLFSSL_ASN1_TIME *wolfSSL_ASN1_TIME_to_generalizedtime(WOLFSSL_ASN1_TIME *t,
                                                        WOLFSSL_ASN1_TIME **out)
{
    unsigned char time_type;
    WOLFSSL_ASN1_TIME *ret = NULL;
    unsigned char *data_ptr = NULL;

    WOLFSSL_ENTER("wolfSSL_ASN1_TIME_to_generalizedtime");
    if (t == NULL)
        return NULL;

    time_type = t->data[0];
    if (time_type != ASN_UTC_TIME && time_type != ASN_GENERALIZED_TIME){
        WOLFSSL_MSG("Invalid ASN_TIME type.");
        return NULL;
    }
    if (out == NULL || *out == NULL){
        ret = (WOLFSSL_ASN1_TIME*)XMALLOC(sizeof(WOLFSSL_ASN1_TIME), NULL, 
                                        DYNAMIC_TYPE_TMP_BUFFER);
        if (ret == NULL){
            WOLFSSL_MSG("memory alloc failed.");
            return NULL;
        }
        XMEMSET(ret, 0, sizeof(WOLFSSL_ASN1_TIME));
    } else 
        ret = *out;

    if (time_type == ASN_GENERALIZED_TIME){
        XMEMCPY(ret->data, t->data, ASN_GENERALIZED_TIME_SIZE);
        return ret;
    } else if (time_type == ASN_UTC_TIME){
        ret->data[0] = ASN_GENERALIZED_TIME;
        ret->data[1] = ASN_GENERALIZED_TIME_SIZE;
        data_ptr  = ret->data + 2;
        if (t->data[2] >= '5') 
            XSNPRINTF((char*)data_ptr, ASN_UTC_TIME_SIZE + 2, "19%s", t->data + 2);
        else
            XSNPRINTF((char*)data_ptr, ASN_UTC_TIME_SIZE + 2, "20%s", t->data + 2);

        return ret;
    } 
    
    WOLFSSL_MSG("Invalid ASN_TIME value");
    return NULL;
}
#endif /* !NO_ASN_TIME */


#ifndef NO_ASN
int wolfSSL_i2c_ASN1_INTEGER(WOLFSSL_ASN1_INTEGER *a, unsigned char **pp)
{
    unsigned char *pptr = NULL;
    char pad = 0 ;
    unsigned char pad_val = 0;
    int ret_size = 0;
    unsigned char data1 = 0;
    unsigned char neg = 0;
    int i = 0;

    WOLFSSL_ENTER("wolfSSL_i2c_ASN1_INTEGER");
    if (a == NULL)
        return WOLFSSL_FAILURE;

    ret_size = a->intData[1];
    if (ret_size == 0)
        ret_size = 1;
    else{
        ret_size = (int)a->intData[1];
        neg = a->negative;
        data1 = a->intData[2];
        if (ret_size == 1 && data1 == 0)
            neg = 0;
        /* 0x80 or greater positive number in first byte */
        if (!neg && (data1 > 127)){
            pad = 1;
            pad_val = 0;
        } else if (neg){
            /* negative number */
            if (data1 > 128){
                pad = 1;
                pad_val = 0xff;
            } else if (data1 == 128){
                for (i = 3; i < a->intData[1] + 2; i++){
                    if (a->intData[i]){
                        pad = 1;
                        pad_val = 0xff;
                        break;
                    }
                }
            }
        }
        ret_size += (int)pad;
    }
    if (pp == NULL)
        return ret_size;

    pptr = *pp;
    if (pad)
        *(pptr++) = pad_val;
    if (a->intData[1] == 0)
        *(pptr++) = 0;
    else if (!neg){
        /* positive number */
        for (i=0; i < a->intData[1]; i++){
            *pptr = a->intData[i+2];
            pptr++;
        }
    } else {
        /* negative number */
        int str_len = 0;

        /* 0 padding from end of buffer */
        str_len = (int)a->intData[1];
        pptr += a->intData[1] - 1;
        while (!a->intData[str_len + 2] && str_len > 1){
            *(pptr--) = 0;
            str_len--; 
        }
        /* 2's complement next octet */
        *(pptr--) = ((a->intData[str_len + 1]) ^ 0xff) + 1;
        str_len--;
        /* Complement any octets left */
        while (str_len > 0){
            *(pptr--) = a->intData[str_len + 1] ^ 0xff;
            str_len--;
        }
    }
    *pp += ret_size;
    return ret_size;
}
#endif /* !NO_ASN */

#endif  /* OPENSSLEXTRA */
