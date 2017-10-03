/* internal.c
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

#include <wolfssl/internal.h>
#include <wolfssl/error-ssl.h>
#include <wolfssl/wolfcrypt/asn.h>
#include <wolfssl/wolfcrypt/dh.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#ifdef HAVE_LIBZ
    #include "zlib.h"
#endif

#ifdef HAVE_NTRU
    #include "libntruencrypt/ntru_crypto.h"
#endif

#if defined(DEBUG_WOLFSSL) || defined(SHOW_SECRETS) || \
    defined(CHACHA_AEAD_TEST) || defined(WOLFSSL_SESSION_EXPORT_DEBUG)
    #if defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)
        #if MQX_USE_IO_OLD
            #include <fio.h>
        #else
            #include <nio.h>
        #endif
    #else
        #include <stdio.h>
    #endif
#endif

#ifdef __sun
    #include <sys/filio.h>
#endif


#define ERROR_OUT(err, eLabel) { ret = (err); goto eLabel; }

#ifdef _MSC_VER
    /* disable for while(0) cases at the .c level for now */
    #pragma warning(disable:4127)
#endif

#if defined(WOLFSSL_CALLBACKS) && !defined(LARGE_STATIC_BUFFERS)
    #error \
WOLFSSL_CALLBACKS needs LARGE_STATIC_BUFFERS, please add LARGE_STATIC_BUFFERS
#endif

#if defined(HAVE_SECURE_RENEGOTIATION) && defined(HAVE_RENEGOTIATION_INDICATION)
    #error Cannot use both secure-renegotiation and renegotiation-indication
#endif

#ifndef NO_WOLFSSL_CLIENT
    static int DoHelloVerifyRequest(WOLFSSL* ssl, const byte* input, word32*,
                                                                        word32);
    static int DoServerKeyExchange(WOLFSSL* ssl, const byte* input, word32*,
                                                                        word32);
    #ifndef NO_CERTS
        static int DoCertificateRequest(WOLFSSL* ssl, const byte* input, word32*,
                                                                        word32);
    #endif
    #ifdef HAVE_SESSION_TICKET
        static int DoSessionTicket(WOLFSSL* ssl, const byte* input, word32*,
                                                                        word32);
    #endif
#endif


#ifndef NO_WOLFSSL_SERVER
    static int DoClientKeyExchange(WOLFSSL* ssl, byte* input, word32*, word32);
    #if !defined(NO_RSA) || defined(HAVE_ECC)
        static int DoCertificateVerify(WOLFSSL* ssl, byte*, word32*, word32);
    #endif
    #ifdef WOLFSSL_DTLS
        static int SendHelloVerifyRequest(WOLFSSL*, const byte*, byte);
    #endif /* WOLFSSL_DTLS */
#endif


#ifdef WOLFSSL_DTLS
    static INLINE int DtlsCheckWindow(WOLFSSL* ssl);
    static INLINE int DtlsUpdateWindow(WOLFSSL* ssl);
#endif


enum processReply {
    doProcessInit = 0,
#ifndef NO_WOLFSSL_SERVER
    runProcessOldClientHello,
#endif
    getRecordLayerHeader,
    getData,
    decryptMessage,
    verifyMessage,
    runProcessingOneMessage
};


/* Server random bytes for TLS v1.3 described downgrade protection mechanism. */
static const byte tls13Downgrade[7] = {
    0x44, 0x4f, 0x47, 0x4e, 0x47, 0x52, 0x44
};
#define TLS13_DOWNGRADE_SZ  sizeof(tls13Downgrade)


#ifndef NO_OLD_TLS
static int SSL_hmac(WOLFSSL* ssl, byte* digest, const byte* in, word32 sz,
                    int content, int verify);

#endif

#ifdef HAVE_QSH
    int QSH_Init(WOLFSSL* ssl);
#endif


int IsTLS(const WOLFSSL* ssl)
{
    if (ssl->version.major == SSLv3_MAJOR && ssl->version.minor >=TLSv1_MINOR)
        return 1;

    return 0;
}


int IsAtLeastTLSv1_2(const WOLFSSL* ssl)
{
    if (ssl->version.major == SSLv3_MAJOR && ssl->version.minor >=TLSv1_2_MINOR)
        return 1;
    if (ssl->version.major == DTLS_MAJOR && ssl->version.minor <= DTLSv1_2_MINOR)
        return 1;

    return 0;
}

int IsAtLeastTLSv1_3(const ProtocolVersion pv)
{
    return (pv.major == SSLv3_MAJOR && pv.minor >= TLSv1_3_MINOR);
}


static INLINE int IsEncryptionOn(WOLFSSL* ssl, int isSend)
{
    (void)isSend;

    #ifdef WOLFSSL_DTLS
    /* For DTLS, epoch 0 is always not encrypted. */
    if (ssl->options.dtls && !isSend && ssl->keys.curEpoch == 0)
        return 0;
    #endif /* WOLFSSL_DTLS */

    return ssl->keys.encryptionOn;
}


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


#ifdef HAVE_QSH
/* free all structs that where used with QSH */
static int QSH_FreeAll(WOLFSSL* ssl)
{
    QSHKey* key        = ssl->QSH_Key;
    QSHKey* preKey     = NULL;
    QSHSecret* secret  = ssl->QSH_secret;
    QSHScheme* list    = NULL;
    QSHScheme* preList = NULL;

    /* free elements in struct */
    while (key) {
        preKey = key;
        if (key->pri.buffer) {
            ForceZero(key->pri.buffer, key->pri.length);
            XFREE(key->pri.buffer, ssl->heap, DYNAMIC_TYPE_PRIVATE_KEY);
        }
        if (key->pub.buffer)
            XFREE(key->pub.buffer, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
        key = (QSHKey*)key->next;

        /* free struct */
        XFREE(preKey, ssl->heap, DYNAMIC_TYPE_QSH);
    }
    key = NULL;


    /* free all of peers QSH keys */
    key = ssl->peerQSHKey;
    while (key) {
        preKey = key;
        if (key->pri.buffer) {
            ForceZero(key->pri.buffer, key->pri.length);
            XFREE(key->pri.buffer, ssl->heap, DYNAMIC_TYPE_PRIVATE_KEY);
        }
        if (key->pub.buffer)
            XFREE(key->pub.buffer, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
        key = (QSHKey*)key->next;

        /* free struct */
        XFREE(preKey, ssl->heap, DYNAMIC_TYPE_QSH);
    }
    key = NULL;

    /* free secret information */
    if (secret) {
        /* free up the QSHScheme list in QSHSecret */
        if (secret->list)
            list = secret->list;
        while (list) {
            preList = list;
            if (list->PK)
                XFREE(list->PK, ssl->heap, DYNAMIC_TYPE_SECRET);
            list = (QSHScheme*)list->next;
            XFREE(preList, ssl->heap, DYNAMIC_TYPE_QSH);
        }

        /* free secret buffers */
        if (secret->SerSi) {
            if (secret->SerSi->buffer) {
                /* clear extra secret material that supplemented Master Secret*/
                ForceZero(secret->SerSi->buffer, secret->SerSi->length);
                XFREE(secret->SerSi->buffer, ssl->heap, DYNAMIC_TYPE_SECRET);
            }
            XFREE(secret->SerSi, ssl->heap, DYNAMIC_TYPE_SECRET);
        }
        if (secret->CliSi) {
            if (secret->CliSi->buffer) {
                /* clear extra secret material that supplemented Master Secret*/
                ForceZero(secret->CliSi->buffer, secret->CliSi->length);
                XFREE(secret->CliSi->buffer, ssl->heap, DYNAMIC_TYPE_SECRET);
            }
            XFREE(secret->CliSi, ssl->heap, DYNAMIC_TYPE_SECRET);
        }
    }
    XFREE(secret, ssl->heap, DYNAMIC_TYPE_QSH);
    secret = NULL;

    return 0;
}
#endif


#ifdef HAVE_NTRU
static WC_RNG* rng;
static wolfSSL_Mutex* rngMutex;

static word32 GetEntropy(unsigned char* out, word32 num_bytes)
{
    int ret = 0;

    if (rng == NULL) {
        if ((rng = (WC_RNG*)XMALLOC(sizeof(WC_RNG), 0,
                                                    DYNAMIC_TYPE_RNG)) == NULL)
            return DRBG_OUT_OF_MEMORY;
        wc_InitRng(rng);
    }

    if (rngMutex == NULL) {
        if ((rngMutex = (wolfSSL_Mutex*)XMALLOC(sizeof(wolfSSL_Mutex), 0,
                        DYNAMIC_TYPE_MUTEX)) == NULL)
            return DRBG_OUT_OF_MEMORY;
        wc_InitMutex(rngMutex);
    }

    ret |= wc_LockMutex(rngMutex);
    ret |= wc_RNG_GenerateBlock(rng, out, num_bytes);
    ret |= wc_UnLockMutex(rngMutex);

    if (ret != 0)
        return DRBG_ENTROPY_FAIL;

    return DRBG_OK;
}
#endif /* HAVE_NTRU */

/* used by ssl.c too */
void c32to24(word32 in, word24 out)
{
    out[0] = (in >> 16) & 0xff;
    out[1] = (in >>  8) & 0xff;
    out[2] =  in & 0xff;
}


/* convert 16 bit integer to opaque */
static INLINE void c16toa(word16 u16, byte* c)
{
    c[0] = (u16 >> 8) & 0xff;
    c[1] =  u16 & 0xff;
}


#if !defined(NO_OLD_TLS) || defined(HAVE_CHACHA) || defined(HAVE_AESCCM) \
    || defined(HAVE_AESGCM) || defined(WOLFSSL_SESSION_EXPORT) \
    || defined(WOLFSSL_DTLS) || defined(HAVE_SESSION_TICKET)
/* convert 32 bit integer to opaque */
static INLINE void c32toa(word32 u32, byte* c)
{
    c[0] = (u32 >> 24) & 0xff;
    c[1] = (u32 >> 16) & 0xff;
    c[2] = (u32 >>  8) & 0xff;
    c[3] =  u32 & 0xff;
}

#endif


/* convert a 24 bit integer into a 32 bit one */
static INLINE void c24to32(const word24 u24, word32* u32)
{
    *u32 = (u24[0] << 16) | (u24[1] << 8) | u24[2];
}


/* convert opaque to 16 bit integer */
static INLINE void ato16(const byte* c, word16* u16)
{
    *u16 = (word16) ((c[0] << 8) | (c[1]));
}


#if defined(WOLFSSL_DTLS) || defined(HAVE_SESSION_TICKET) || \
    defined(WOLFSSL_SESSION_EXPORT)

/* convert opaque to 32 bit integer */
static INLINE void ato32(const byte* c, word32* u32)
{
    *u32 = (c[0] << 24) | (c[1] << 16) | (c[2] << 8) | c[3];
}

#endif /* WOLFSSL_DTLS */


#ifdef HAVE_LIBZ

    /* alloc user allocs to work with zlib */
    static void* myAlloc(void* opaque, unsigned int item, unsigned int size)
    {
        (void)opaque;
        return XMALLOC(item * size, opaque, DYNAMIC_TYPE_LIBZ);
    }


    static void myFree(void* opaque, void* memory)
    {
        (void)opaque;
        XFREE(memory, opaque, DYNAMIC_TYPE_LIBZ);
    }


    /* init zlib comp/decomp streams, 0 on success */
    static int InitStreams(WOLFSSL* ssl)
    {
        ssl->c_stream.zalloc = (alloc_func)myAlloc;
        ssl->c_stream.zfree  = (free_func)myFree;
        ssl->c_stream.opaque = (voidpf)ssl->heap;

        if (deflateInit(&ssl->c_stream, Z_DEFAULT_COMPRESSION) != Z_OK)
            return ZLIB_INIT_ERROR;

        ssl->didStreamInit = 1;

        ssl->d_stream.zalloc = (alloc_func)myAlloc;
        ssl->d_stream.zfree  = (free_func)myFree;
        ssl->d_stream.opaque = (voidpf)ssl->heap;

        if (inflateInit(&ssl->d_stream) != Z_OK) return ZLIB_INIT_ERROR;

        return 0;
    }


    static void FreeStreams(WOLFSSL* ssl)
    {
        if (ssl->didStreamInit) {
            deflateEnd(&ssl->c_stream);
            inflateEnd(&ssl->d_stream);
        }
    }


    /* compress in to out, return out size or error */
    static int myCompress(WOLFSSL* ssl, byte* in, int inSz, byte* out, int outSz)
    {
        int    err;
        int    currTotal = (int)ssl->c_stream.total_out;

        ssl->c_stream.next_in   = in;
        ssl->c_stream.avail_in  = inSz;
        ssl->c_stream.next_out  = out;
        ssl->c_stream.avail_out = outSz;

        err = deflate(&ssl->c_stream, Z_SYNC_FLUSH);
        if (err != Z_OK && err != Z_STREAM_END) return ZLIB_COMPRESS_ERROR;

        return (int)ssl->c_stream.total_out - currTotal;
    }


    /* decompress in to out, return out size or error */
    static int myDeCompress(WOLFSSL* ssl, byte* in,int inSz, byte* out,int outSz)
    {
        int    err;
        int    currTotal = (int)ssl->d_stream.total_out;

        ssl->d_stream.next_in   = in;
        ssl->d_stream.avail_in  = inSz;
        ssl->d_stream.next_out  = out;
        ssl->d_stream.avail_out = outSz;

        err = inflate(&ssl->d_stream, Z_SYNC_FLUSH);
        if (err != Z_OK && err != Z_STREAM_END) return ZLIB_DECOMPRESS_ERROR;

        return (int)ssl->d_stream.total_out - currTotal;
    }

#endif /* HAVE_LIBZ */


#ifdef WOLFSSL_SESSION_EXPORT
#ifdef WOLFSSL_DTLS
/* serializes the cipher specs struct for exporting */
static int ExportCipherSpecState(WOLFSSL* ssl, byte* exp, word32 len, byte ver)
{
    word32 idx = 0;
    CipherSpecs* specs;

    WOLFSSL_ENTER("ExportCipherSpecState");

    if (exp == NULL || ssl == NULL) {
        return BAD_FUNC_ARG;
    }

    specs= &(ssl->specs);

    if (DTLS_EXPORT_SPC_SZ > len) {
        return BUFFER_E;
    }

    XMEMSET(exp, 0, DTLS_EXPORT_SPC_SZ);

    c16toa(specs->key_size, exp + idx);      idx += OPAQUE16_LEN;
    c16toa(specs->iv_size, exp + idx);       idx += OPAQUE16_LEN;
    c16toa(specs->block_size, exp + idx);    idx += OPAQUE16_LEN;
    c16toa(specs->aead_mac_size, exp + idx); idx += OPAQUE16_LEN;
    exp[idx++] = specs->bulk_cipher_algorithm;
    exp[idx++] = specs->cipher_type;
    exp[idx++] = specs->mac_algorithm;
    exp[idx++] = specs->kea;
    exp[idx++] = specs->sig_algo;
    exp[idx++] = specs->hash_size;
    exp[idx++] = specs->pad_size;
    exp[idx++] = specs->static_ecdh;

    if (idx != DTLS_EXPORT_SPC_SZ) {
        WOLFSSL_MSG("DTLS_EXPORT_SPC_SZ needs updated and export version");
        return DTLS_EXPORT_VER_E;
    }

    WOLFSSL_LEAVE("ExportCipherSpecState", idx);
    (void)ver;
    return idx;
}


/* serializes the key struct for exporting */
static int ExportKeyState(WOLFSSL* ssl, byte* exp, word32 len, byte ver)
{
    word32 idx = 0;
    byte   sz;
    Keys* keys;

    WOLFSSL_ENTER("ExportKeyState");

    if (exp == NULL || ssl == NULL) {
        return BAD_FUNC_ARG;
    }

    keys = &(ssl->keys);

    if (DTLS_EXPORT_KEY_SZ > len) {
        WOLFSSL_MSG("Buffer not large enough for max key struct size");
        return BUFFER_E;
    }

    XMEMSET(exp, 0, DTLS_EXPORT_KEY_SZ);

    c32toa(keys->peer_sequence_number_hi, exp + idx); idx += OPAQUE32_LEN;
    c32toa(keys->peer_sequence_number_lo, exp + idx); idx += OPAQUE32_LEN;
    c32toa(keys->sequence_number_hi, exp + idx);      idx += OPAQUE32_LEN;
    c32toa(keys->sequence_number_lo, exp + idx);      idx += OPAQUE32_LEN;

    c16toa(keys->peerSeq[0].nextEpoch, exp + idx);  idx += OPAQUE16_LEN;
    c16toa(keys->peerSeq[0].nextSeq_hi, exp + idx); idx += OPAQUE16_LEN;
    c32toa(keys->peerSeq[0].nextSeq_lo, exp + idx); idx += OPAQUE32_LEN;
    c16toa(keys->curEpoch, exp + idx);   idx += OPAQUE16_LEN;
    c16toa(keys->curSeq_hi, exp + idx);  idx += OPAQUE16_LEN;
    c32toa(keys->curSeq_lo, exp + idx);  idx += OPAQUE32_LEN;
    c16toa(keys->peerSeq[0].prevSeq_hi, exp + idx); idx += OPAQUE16_LEN;
    c32toa(keys->peerSeq[0].prevSeq_lo, exp + idx); idx += OPAQUE32_LEN;

    c16toa(keys->dtls_peer_handshake_number, exp + idx); idx += OPAQUE16_LEN;
    c16toa(keys->dtls_expected_peer_handshake_number, exp + idx);
    idx += OPAQUE16_LEN;

    c16toa(keys->dtls_sequence_number_hi, exp + idx);      idx += OPAQUE16_LEN;
    c32toa(keys->dtls_sequence_number_lo, exp + idx);      idx += OPAQUE32_LEN;
    c16toa(keys->dtls_prev_sequence_number_hi, exp + idx); idx += OPAQUE16_LEN;
    c32toa(keys->dtls_prev_sequence_number_lo, exp + idx); idx += OPAQUE32_LEN;
    c16toa(keys->dtls_epoch, exp + idx);                   idx += OPAQUE16_LEN;
    c16toa(keys->dtls_handshake_number, exp + idx);        idx += OPAQUE16_LEN;
    c32toa(keys->encryptSz, exp + idx);                    idx += OPAQUE32_LEN;
    c32toa(keys->padSz, exp + idx);                        idx += OPAQUE32_LEN;
    exp[idx++] = keys->encryptionOn;
    exp[idx++] = keys->decryptedCur;

    {
        word32 i;

        c16toa(WOLFSSL_DTLS_WINDOW_WORDS, exp + idx); idx += OPAQUE16_LEN;
        for (i = 0; i < WOLFSSL_DTLS_WINDOW_WORDS; i++) {
            c32toa(keys->peerSeq[0].window[i], exp + idx);
            idx += OPAQUE32_LEN;
        }
        c16toa(WOLFSSL_DTLS_WINDOW_WORDS, exp + idx); idx += OPAQUE16_LEN;
        for (i = 0; i < WOLFSSL_DTLS_WINDOW_WORDS; i++) {
            c32toa(keys->peerSeq[0].prevWindow[i], exp + idx);
            idx += OPAQUE32_LEN;
        }
    }

#ifdef HAVE_TRUNCATED_HMAC
    sz         = ssl->truncated_hmac ? TRUNCATED_HMAC_SZ: ssl->specs.hash_size;
    exp[idx++] = ssl->truncated_hmac;
#else
    sz         = ssl->specs.hash_size;
    exp[idx++] = 0; /* no truncated hmac */
#endif
    exp[idx++] = sz;
    XMEMCPY(exp + idx, keys->client_write_MAC_secret, sz); idx += sz;
    XMEMCPY(exp + idx, keys->server_write_MAC_secret, sz); idx += sz;

    sz         = ssl->specs.key_size;
    exp[idx++] = sz;
    XMEMCPY(exp + idx, keys->client_write_key, sz); idx += sz;
    XMEMCPY(exp + idx, keys->server_write_key, sz); idx += sz;

    sz         = ssl->specs.iv_size;
    exp[idx++] = sz;
    XMEMCPY(exp + idx, keys->client_write_IV, sz); idx += sz;
    XMEMCPY(exp + idx, keys->server_write_IV, sz); idx += sz;
    XMEMCPY(exp + idx, keys->aead_exp_IV, AEAD_MAX_EXP_SZ);
    idx += AEAD_MAX_EXP_SZ;

    sz         = AEAD_MAX_IMP_SZ;
    exp[idx++] = sz;
    XMEMCPY(exp + idx, keys->aead_enc_imp_IV, sz); idx += sz;
    XMEMCPY(exp + idx, keys->aead_dec_imp_IV, sz); idx += sz;

    /* DTLS_EXPORT_KEY_SZ is max value. idx size can vary */
    if (idx > DTLS_EXPORT_KEY_SZ) {
        WOLFSSL_MSG("DTLS_EXPORT_KEY_SZ needs updated and export version");
        return DTLS_EXPORT_VER_E;
    }

    WOLFSSL_LEAVE("ExportKeyState", idx);
    (void)ver;
    return idx;
}

static int ImportCipherSpecState(WOLFSSL* ssl, byte* exp, word32 len, byte ver)
{
    word32 idx = 0;
    CipherSpecs* specs;

    WOLFSSL_ENTER("ImportCipherSpecState");

    if (exp == NULL || ssl == NULL) {
        return BAD_FUNC_ARG;
    }

    specs= &(ssl->specs);

    if (DTLS_EXPORT_SPC_SZ > len) {
        WOLFSSL_MSG("Buffer not large enough for max spec struct size");
        return BUFFER_E;
    }

    ato16(exp + idx, &specs->key_size);      idx += OPAQUE16_LEN;
    ato16(exp + idx, &specs->iv_size);       idx += OPAQUE16_LEN;
    ato16(exp + idx, &specs->block_size);    idx += OPAQUE16_LEN;
    ato16(exp + idx, &specs->aead_mac_size); idx += OPAQUE16_LEN;
    specs->bulk_cipher_algorithm = exp[idx++];
    specs->cipher_type           = exp[idx++];
    specs->mac_algorithm         = exp[idx++];
    specs->kea                   = exp[idx++];
    specs->sig_algo              = exp[idx++];
    specs->hash_size             = exp[idx++];
    specs->pad_size              = exp[idx++];
    specs->static_ecdh           = exp[idx++];

    WOLFSSL_LEAVE("ImportCipherSpecState", idx);
    (void)ver;
    return idx;
}


static int ImportKeyState(WOLFSSL* ssl, byte* exp, word32 len, byte ver)
{
    word32 idx = 0;
    byte  sz;
    Keys* keys;

    WOLFSSL_ENTER("ImportKeyState");

    if (exp == NULL || ssl == NULL) {
        return BAD_FUNC_ARG;
    }

    keys = &(ssl->keys);

    /* check minimum length -- includes byte used for size indicators */
    if (len < DTLS_EXPORT_MIN_KEY_SZ) {
        return BUFFER_E;
    }
    ato32(exp + idx, &keys->peer_sequence_number_hi); idx += OPAQUE32_LEN;
    ato32(exp + idx, &keys->peer_sequence_number_lo); idx += OPAQUE32_LEN;
    ato32(exp + idx, &keys->sequence_number_hi);      idx += OPAQUE32_LEN;
    ato32(exp + idx, &keys->sequence_number_lo);      idx += OPAQUE32_LEN;

    ato16(exp + idx, &keys->peerSeq[0].nextEpoch);  idx += OPAQUE16_LEN;
    ato16(exp + idx, &keys->peerSeq[0].nextSeq_hi); idx += OPAQUE16_LEN;
    ato32(exp + idx, &keys->peerSeq[0].nextSeq_lo); idx += OPAQUE32_LEN;
    ato16(exp + idx, &keys->curEpoch);   idx += OPAQUE16_LEN;
    ato16(exp + idx, &keys->curSeq_hi);  idx += OPAQUE16_LEN;
    ato32(exp + idx, &keys->curSeq_lo);  idx += OPAQUE32_LEN;
    ato16(exp + idx, &keys->peerSeq[0].prevSeq_hi); idx += OPAQUE16_LEN;
    ato32(exp + idx, &keys->peerSeq[0].prevSeq_lo); idx += OPAQUE32_LEN;

    ato16(exp + idx, &keys->dtls_peer_handshake_number); idx += OPAQUE16_LEN;
    ato16(exp + idx, &keys->dtls_expected_peer_handshake_number);
    idx += OPAQUE16_LEN;

    ato16(exp + idx, &keys->dtls_sequence_number_hi);      idx += OPAQUE16_LEN;
    ato32(exp + idx, &keys->dtls_sequence_number_lo);      idx += OPAQUE32_LEN;
    ato16(exp + idx, &keys->dtls_prev_sequence_number_hi); idx += OPAQUE16_LEN;
    ato32(exp + idx, &keys->dtls_prev_sequence_number_lo); idx += OPAQUE32_LEN;
    ato16(exp + idx, &keys->dtls_epoch);                   idx += OPAQUE16_LEN;
    ato16(exp + idx, &keys->dtls_handshake_number);        idx += OPAQUE16_LEN;
    ato32(exp + idx, &keys->encryptSz);                    idx += OPAQUE32_LEN;
    ato32(exp + idx, &keys->padSz);                        idx += OPAQUE32_LEN;
    keys->encryptionOn = exp[idx++];
    keys->decryptedCur = exp[idx++];

    {
        word16 i, wordCount, wordAdj = 0;

        /* do window */
        ato16(exp + idx, &wordCount);
        idx += OPAQUE16_LEN;

        if (wordCount > WOLFSSL_DTLS_WINDOW_WORDS) {
            wordCount = WOLFSSL_DTLS_WINDOW_WORDS;
            wordAdj = (WOLFSSL_DTLS_WINDOW_WORDS - wordCount) * sizeof(word32);
        }

        XMEMSET(keys->peerSeq[0].window, 0xFF, DTLS_SEQ_SZ);
        for (i = 0; i < wordCount; i++) {
            ato32(exp + idx, &keys->peerSeq[0].window[i]);
            idx += OPAQUE32_LEN;
        }
        idx += wordAdj;

        /* do prevWindow */
        ato16(exp + idx, &wordCount);
        idx += OPAQUE16_LEN;

        if (wordCount > WOLFSSL_DTLS_WINDOW_WORDS) {
            wordCount = WOLFSSL_DTLS_WINDOW_WORDS;
            wordAdj = (WOLFSSL_DTLS_WINDOW_WORDS - wordCount) * sizeof(word32);
        }

        XMEMSET(keys->peerSeq[0].prevWindow, 0xFF, DTLS_SEQ_SZ);
        for (i = 0; i < wordCount; i++) {
            ato32(exp + idx, &keys->peerSeq[0].prevWindow[i]);
            idx += OPAQUE32_LEN;
        }
        idx += wordAdj;

    }

#ifdef HAVE_TRUNCATED_HMAC
    ssl->truncated_hmac = exp[idx++];
#else
    idx++; /* no truncated hmac */
#endif
    sz = exp[idx++];
    if (sz > MAX_DIGEST_SIZE || sz + idx > len) {
        return BUFFER_E;
    }
    XMEMCPY(keys->client_write_MAC_secret, exp + idx, sz); idx += sz;
    XMEMCPY(keys->server_write_MAC_secret, exp + idx, sz); idx += sz;

    sz = exp[idx++];
    if (sz > AES_256_KEY_SIZE || sz + idx > len) {
        return BUFFER_E;
    }
    XMEMCPY(keys->client_write_key, exp + idx, sz); idx += sz;
    XMEMCPY(keys->server_write_key, exp + idx, sz); idx += sz;

    sz = exp[idx++];
    if (sz > MAX_WRITE_IV_SZ || sz + idx > len) {
        return BUFFER_E;
    }
    XMEMCPY(keys->client_write_IV, exp + idx, sz); idx += sz;
    XMEMCPY(keys->server_write_IV, exp + idx, sz); idx += sz;
    XMEMCPY(keys->aead_exp_IV, exp + idx, AEAD_MAX_EXP_SZ);
    idx += AEAD_MAX_EXP_SZ;

    sz = exp[idx++];
    if (sz > AEAD_MAX_IMP_SZ || sz + idx > len) {
        return BUFFER_E;
    }
    XMEMCPY(keys->aead_enc_imp_IV, exp + idx, sz); idx += sz;
    XMEMCPY(keys->aead_dec_imp_IV, exp + idx, sz); idx += sz;

    WOLFSSL_LEAVE("ImportKeyState", idx);
    (void)ver;
    return idx;
}


/* copy over necessary information from Options struct to buffer
 * On success returns size of buffer used on failure returns a negative value */
static int dtls_export_new(WOLFSSL* ssl, byte* exp, word32 len, byte ver)
{
    int idx = 0;
    word16 zero = 0;
    Options* options = &ssl->options;

    WOLFSSL_ENTER("dtls_export_new");

    if (exp == NULL || options == NULL || len < DTLS_EXPORT_OPT_SZ) {
        return BAD_FUNC_ARG;
    }

    XMEMSET(exp, 0, DTLS_EXPORT_OPT_SZ);

    /* these options are kept and sent to indicate verify status and strength
     * of handshake */
    exp[idx++] = options->sendVerify;
    exp[idx++] = options->verifyPeer;
    exp[idx++] = options->verifyNone;
    exp[idx++] = options->downgrade;
#ifndef NO_DH
    c16toa(options->minDhKeySz, exp + idx); idx += OPAQUE16_LEN;
    c16toa(options->dhKeySz, exp + idx);    idx += OPAQUE16_LEN;
#else
    c16toa(zero, exp + idx); idx += OPAQUE16_LEN;
    c16toa(zero, exp + idx); idx += OPAQUE16_LEN;
#endif
#ifndef NO_RSA
    c16toa((word16)(options->minRsaKeySz), exp + idx); idx += OPAQUE16_LEN;
#else
    c16toa(zero, exp + idx); idx += OPAQUE16_LEN;
#endif
#ifdef HAVE_ECC
    c16toa((word16)(options->minEccKeySz), exp + idx); idx += OPAQUE16_LEN;
#else
    c16toa(zero, exp + idx); idx += OPAQUE16_LEN;
#endif

    /* these options are kept to indicate state and behavior */
#ifndef NO_PSK
    exp[idx++] = options->havePSK;
#else
    exp[idx++] = 0;
#endif
    exp[idx++] = options->sessionCacheOff;
    exp[idx++] = options->sessionCacheFlushOff;
    exp[idx++] = options->side;
    exp[idx++] = options->resuming;
    exp[idx++] = options->haveSessionId;
    exp[idx++] = options->tls;
    exp[idx++] = options->tls1_1;
    exp[idx++] = options->dtls;
    exp[idx++] = options->connReset;
    exp[idx++] = options->isClosed;
    exp[idx++] = options->closeNotify;
    exp[idx++] = options->sentNotify;
    exp[idx++] = options->usingCompression;
    exp[idx++] = options->haveRSA;
    exp[idx++] = options->haveECC;
    exp[idx++] = options->haveDH;
    exp[idx++] = options->haveNTRU;
    exp[idx++] = options->haveQSH;
    exp[idx++] = options->haveECDSAsig;
    exp[idx++] = options->haveStaticECC;
    exp[idx++] = options->havePeerVerify;
    exp[idx++] = options->usingPSK_cipher;
    exp[idx++] = options->usingAnon_cipher;
    exp[idx++] = options->sendAlertState;
    exp[idx++] = options->partialWrite;
    exp[idx++] = options->quietShutdown;
    exp[idx++] = options->groupMessages;
#ifdef HAVE_POLY1305
    exp[idx++] = options->oldPoly;
#else
    exp[idx++] = 0;
#endif
#ifdef HAVE_ANON
    exp[idx++] = options->haveAnon;
#else
    exp[idx++] = 0;
#endif
#ifdef HAVE_SESSION_TICKET
    exp[idx++] = options->createTicket;
    exp[idx++] = options->useTicket;
#ifdef WOLFSSL_TLS13
    if (ver > DTLS_EXPORT_VERSION_3) {
        exp[idx++] = options->noTicketTls13;
    }
#else
    if (ver > DTLS_EXPORT_VERSION_3) {
        exp[idx++] = 0;
    }
#endif
#else
    exp[idx++] = 0;
    exp[idx++] = 0;
    if (ver > DTLS_EXPORT_VERSION_3) {
        exp[idx++] = 0;
    }
#endif
    exp[idx++] = options->processReply;
    exp[idx++] = options->cipherSuite0;
    exp[idx++] = options->cipherSuite;
    exp[idx++] = options->serverState;
    exp[idx++] = options->clientState;
    exp[idx++] = options->handShakeState;
    exp[idx++] = options->handShakeDone;
    exp[idx++] = options->minDowngrade;
    exp[idx++] = options->connectState;
    exp[idx++] = options->acceptState;
    exp[idx++] = options->asyncState;

    /* version of connection */
    exp[idx++] = ssl->version.major;
    exp[idx++] = ssl->version.minor;

    (void)zero;

    /* check if changes were made and notify of need to update export version */
    switch (ver) {
        case DTLS_EXPORT_VERSION_3:
            if (idx != DTLS_EXPORT_OPT_SZ_3) {
                WOLFSSL_MSG("Update DTLS_EXPORT_OPT_SZ and version of export");
                return DTLS_EXPORT_VER_E;
            }
            break;

        case DTLS_EXPORT_VERSION:
            if (idx != DTLS_EXPORT_OPT_SZ) {
                WOLFSSL_MSG("Update DTLS_EXPORT_OPT_SZ and version of export");
                return DTLS_EXPORT_VER_E;
            }
            break;

       default:
            WOLFSSL_MSG("New version case needs added to wolfSSL export");
            return DTLS_EXPORT_VER_E;
    }

    WOLFSSL_LEAVE("dtls_export_new", idx);

    return idx;
}


/* copy items from Export struct to Options struct
 * On success returns size of buffer used on failure returns a negative value */
static int dtls_export_load(WOLFSSL* ssl, byte* exp, word32 len, byte ver)
{
    int idx = 0;
    Options* options = &ssl->options;

    switch (ver) {
        case DTLS_EXPORT_VERSION:
            if (len < DTLS_EXPORT_OPT_SZ) {
                WOLFSSL_MSG("Sanity check on buffer size failed");
                return BAD_FUNC_ARG;
            }
            break;

        case DTLS_EXPORT_VERSION_3:
            if (len < DTLS_EXPORT_OPT_SZ_3) {
                WOLFSSL_MSG("Sanity check on buffer size failed");
                return BAD_FUNC_ARG;
            }
            break;

        default:
            WOLFSSL_MSG("Export version not supported");
            return BAD_FUNC_ARG;
    }

    if (exp == NULL || options == NULL) {
        return BAD_FUNC_ARG;
    }


    /* these options are kept and sent to indicate verify status and strength
     * of handshake */
    options->sendVerify = exp[idx++];
    options->verifyPeer = exp[idx++];
    options->verifyNone = exp[idx++];
    options->downgrade  = exp[idx++];
#ifndef NO_DH
    ato16(exp + idx, &(options->minDhKeySz)); idx += OPAQUE16_LEN;
    ato16(exp + idx, &(options->dhKeySz));    idx += OPAQUE16_LEN;
#else
    idx += OPAQUE16_LEN;
    idx += OPAQUE16_LEN;
#endif
#ifndef NO_RSA
    ato16(exp + idx, (word16*)&(options->minRsaKeySz)); idx += OPAQUE16_LEN;
#else
    idx += OPAQUE16_LEN;
#endif
#ifdef HAVE_ECC
    ato16(exp + idx, (word16*)&(options->minEccKeySz)); idx += OPAQUE16_LEN;
#else
    idx += OPAQUE16_LEN;
#endif

    /* these options are kept to indicate state and behavior */
#ifndef NO_PSK
    options->havePSK = exp[idx++];
#else
    idx++;
#endif
    options->sessionCacheOff      = exp[idx++];
    options->sessionCacheFlushOff = exp[idx++];
    options->side                 = exp[idx++];
    options->resuming             = exp[idx++];
    options->haveSessionId    = exp[idx++];
    options->tls              = exp[idx++];
    options->tls1_1           = exp[idx++];
    options->dtls             = exp[idx++];
    options->connReset        = exp[idx++];
    options->isClosed         = exp[idx++];
    options->closeNotify      = exp[idx++];
    options->sentNotify       = exp[idx++];
    options->usingCompression = exp[idx++];
    options->haveRSA          = exp[idx++];
    options->haveECC          = exp[idx++];
    options->haveDH           = exp[idx++];
    options->haveNTRU         = exp[idx++];
    options->haveQSH          = exp[idx++];
    options->haveECDSAsig     = exp[idx++];
    options->haveStaticECC    = exp[idx++];
    options->havePeerVerify   = exp[idx++];
    options->usingPSK_cipher  = exp[idx++];
    options->usingAnon_cipher = exp[idx++];
    options->sendAlertState   = exp[idx++];
    options->partialWrite     = exp[idx++];
    options->quietShutdown    = exp[idx++];
    options->groupMessages    = exp[idx++];
#ifdef HAVE_POLY1305
    options->oldPoly = exp[idx++];      /* set when to use old rfc way of poly*/
#else
    idx++;
#endif
#ifdef HAVE_ANON
    options->haveAnon = exp[idx++];     /* User wants to allow Anon suites */
#else
    idx++;
#endif
#ifdef HAVE_SESSION_TICKET
    options->createTicket  = exp[idx++]; /* Server to create new Ticket */
    options->useTicket     = exp[idx++]; /* Use Ticket not session cache */
#ifdef WOLFSSL_TLS13
    if (ver > DTLS_EXPORT_VERSION_3) {
        options->noTicketTls13 = exp[idx++];/* Server won't create new Ticket */
    }
#else
    if (ver > DTLS_EXPORT_VERSION_3) {
        exp[idx++] = 0;
    }
#endif
#else
    idx++;
    idx++;
    if (ver > DTLS_EXPORT_VERSION_3) {
        idx++;
    }
#endif
    options->processReply   = exp[idx++];
    options->cipherSuite0   = exp[idx++];
    options->cipherSuite    = exp[idx++];
    options->serverState    = exp[idx++];
    options->clientState    = exp[idx++];
    options->handShakeState = exp[idx++];
    options->handShakeDone  = exp[idx++];
    options->minDowngrade   = exp[idx++];
    options->connectState   = exp[idx++];
    options->acceptState    = exp[idx++];
    options->asyncState     = exp[idx++];

    /* version of connection */
    if (ssl->version.major != exp[idx++] || ssl->version.minor != exp[idx++]) {
        WOLFSSL_MSG("Version mismatch ie DTLS v1 vs v1.2");
        return VERSION_ERROR;
    }

    return idx;
}

static int ExportPeerInfo(WOLFSSL* ssl, byte* exp, word32 len, byte ver)
{
    int    idx  = 0;
    int    ipSz = DTLS_EXPORT_IP; /* start as max size */
    int    fam  = 0;
    word16 port = 0;
    char   ip[DTLS_EXPORT_IP];

    if (ver != DTLS_EXPORT_VERSION) {
        WOLFSSL_MSG("Export version not supported");
        return BAD_FUNC_ARG;
    }

    if (ssl == NULL || exp == NULL || len < sizeof(ip) + 3 * DTLS_EXPORT_LEN) {
        return BAD_FUNC_ARG;
    }

    if (ssl->ctx->CBGetPeer == NULL) {
        WOLFSSL_MSG("No get peer call back set");
        return BAD_FUNC_ARG;
    }
    if (ssl->ctx->CBGetPeer(ssl, ip, &ipSz, &port, &fam) != SSL_SUCCESS) {
        WOLFSSL_MSG("Get peer callback error");
        return SOCKET_ERROR_E;
    }

    /* check that ipSz/fam is not negative or too large since user can set cb */
    if (ipSz < 0 || ipSz > DTLS_EXPORT_IP || fam < 0) {
        WOLFSSL_MSG("Bad ipSz or fam returned from get peer callback");
        return SOCKET_ERROR_E;
    }

    c16toa((word16)fam, exp + idx);  idx += DTLS_EXPORT_LEN;
    c16toa((word16)ipSz, exp + idx); idx += DTLS_EXPORT_LEN;
    XMEMCPY(exp + idx, ip, ipSz);    idx += ipSz;
    c16toa(port, exp + idx);         idx += DTLS_EXPORT_LEN;

    return idx;
}


static int ImportPeerInfo(WOLFSSL* ssl, byte* buf, word32 len, byte ver)
{
    word16 idx = 0;
    word16 ipSz;
    word16 fam;
    word16 port;
    char   ip[DTLS_EXPORT_IP];

    if (ver != DTLS_EXPORT_VERSION && ver != DTLS_EXPORT_VERSION_3) {
        WOLFSSL_MSG("Export version not supported");
        return BAD_FUNC_ARG;
    }

    if (ssl == NULL || buf == NULL || len < 3 * DTLS_EXPORT_LEN) {
        return BAD_FUNC_ARG;
    }

    /* import sin family */
    ato16(buf + idx, &fam); idx += DTLS_EXPORT_LEN;

    /* import ip address idx, and ipSz are unsigned but cast for enum */
    ato16(buf + idx, &ipSz); idx += DTLS_EXPORT_LEN;
    if (ipSz >= sizeof(ip) || (word16)(idx + ipSz + DTLS_EXPORT_LEN) > len) {
        return BUFFER_E;
    }
    XMEMSET(ip, 0, sizeof(ip));
    XMEMCPY(ip, buf + idx, ipSz); idx += ipSz;
    ip[ipSz] = '\0'; /* with check that ipSz less than ip this is valid */
    ato16(buf + idx, &port); idx += DTLS_EXPORT_LEN;

    /* sanity check for a function to call, then use it to import peer info */
    if (ssl->ctx->CBSetPeer == NULL) {
        WOLFSSL_MSG("No set peer function");
        return BAD_FUNC_ARG;
    }
    if (ssl->ctx->CBSetPeer(ssl, ip, ipSz, port, fam) != SSL_SUCCESS) {
        WOLFSSL_MSG("Error setting peer info");
        return SOCKET_ERROR_E;
    }

    return idx;
}


/* WOLFSSL_LOCAL function that serializes the current WOLFSSL session
 * buf is used to hold the serialized WOLFSSL struct and sz is the size of buf
 * passed in.
 * On success returns the size of serialized session.*/
int wolfSSL_dtls_export_internal(WOLFSSL* ssl, byte* buf, word32 sz)
{
    int ret;
    word32 idx      = 0;
    word32 totalLen = 0;

    WOLFSSL_ENTER("wolfSSL_dtls_export_internal");

    if (buf == NULL || ssl == NULL) {
        WOLFSSL_LEAVE("wolfSSL_dtls_export_internal", BAD_FUNC_ARG);
        return BAD_FUNC_ARG;
    }

    totalLen += DTLS_EXPORT_LEN * 2; /* 2 protocol bytes and 2 length bytes */
    /* each of the following have a 2 byte length before data */
    totalLen += DTLS_EXPORT_LEN + DTLS_EXPORT_OPT_SZ;
    totalLen += DTLS_EXPORT_LEN + DTLS_EXPORT_KEY_SZ;
    totalLen += DTLS_EXPORT_LEN + DTLS_EXPORT_SPC_SZ;
    totalLen += DTLS_EXPORT_LEN + ssl->buffers.dtlsCtx.peer.sz;

    if (totalLen > sz) {
        WOLFSSL_LEAVE("wolfSSL_dtls_export_internal", BUFFER_E);
        return BUFFER_E;
    }

    buf[idx++] =  (byte)DTLS_EXPORT_PRO;
    buf[idx++] = ((byte)DTLS_EXPORT_PRO & 0xF0) |
                 ((byte)DTLS_EXPORT_VERSION & 0X0F);

    idx += DTLS_EXPORT_LEN; /* leave spot for length */

    c16toa((word16)DTLS_EXPORT_OPT_SZ, buf + idx); idx += DTLS_EXPORT_LEN;
    if ((ret = dtls_export_new(ssl, buf + idx, sz - idx,
                                                    DTLS_EXPORT_VERSION)) < 0) {
        WOLFSSL_LEAVE("wolfSSL_dtls_export_internal", ret);
        return ret;
    }
    idx += ret;

    /* export keys struct and dtls state -- variable length stored in ret */
    idx += DTLS_EXPORT_LEN; /* leave room for length */
    if ((ret = ExportKeyState(ssl, buf + idx, sz - idx,
                                                    DTLS_EXPORT_VERSION)) < 0) {
        WOLFSSL_LEAVE("wolfSSL_dtls_export_internal", ret);
        return ret;
    }
    c16toa((word16)ret, buf + idx - DTLS_EXPORT_LEN); idx += ret;

    /* export of cipher specs struct */
    c16toa((word16)DTLS_EXPORT_SPC_SZ, buf + idx); idx += DTLS_EXPORT_LEN;
    if ((ret = ExportCipherSpecState(ssl, buf + idx, sz - idx,
                                                    DTLS_EXPORT_VERSION)) < 0) {
        WOLFSSL_LEAVE("wolfSSL_dtls_export_internal", ret);
        return ret;
    }
    idx += ret;

    /* export of dtls peer information */
    idx += DTLS_EXPORT_LEN;
    if ((ret = ExportPeerInfo(ssl, buf + idx, sz - idx,
                                                    DTLS_EXPORT_VERSION)) < 0) {
        WOLFSSL_LEAVE("wolfSSL_dtls_export_internal", ret);
        return ret;
    }
    c16toa(ret, buf + idx - DTLS_EXPORT_LEN);
    idx += ret;

    /* place total length of exported buffer minus 2 bytes protocol/version */
    c16toa((word16)(idx - DTLS_EXPORT_LEN), buf + DTLS_EXPORT_LEN);

    /* if compiled with debug options then print the version, protocol, size */
#ifdef WOLFSSL_SESSION_EXPORT_DEBUG
    {
        char debug[256];
        XSNPRINTF(debug, sizeof(debug), "Exporting DTLS session\n"
                   "\tVersion  : %d\n\tProtocol : %02X%01X\n\tLength of: %d\n\n"
               , (int)DTLS_EXPORT_VERSION, buf[0], (buf[1] >> 4), idx - 2);
        WOLFSSL_MSG(debug);
    }
#endif /* WOLFSSL_SESSION_EXPORT_DEBUG */

    WOLFSSL_LEAVE("wolfSSL_dtls_export_internal", idx);
    return idx;
}


/* On success return amount of buffer consumed */
int wolfSSL_dtls_import_internal(WOLFSSL* ssl, byte* buf, word32 sz)
{
    word32 idx    = 0;
    word16 length = 0;
    int version;
    int ret;
    int optSz;

    WOLFSSL_ENTER("wolfSSL_dtls_import_internal");
    /* check at least enough room for protocol and length */
    if (sz < DTLS_EXPORT_LEN * 2 || ssl == NULL) {
        return BAD_FUNC_ARG;
    }

    /* sanity check on protocol ID and size of buffer */
    if (buf[idx++]       !=  (byte)DTLS_EXPORT_PRO ||
       (buf[idx] & 0xF0) != ((byte)DTLS_EXPORT_PRO & 0xF0)) {
        /* don't increment on second idx to next get version */
        WOLFSSL_MSG("Incorrect protocol");
        return BAD_FUNC_ARG;
    }
    version = buf[idx++] & 0x0F;

    ato16(buf + idx, &length); idx += DTLS_EXPORT_LEN;
    if (length > sz - DTLS_EXPORT_LEN) { /* subtract 2 for protocol */
        return BUFFER_E;
    }

    /* if compiled with debug options then print the version, protocol, size */
#ifdef WOLFSSL_SESSION_EXPORT_DEBUG
    {
        char debug[256];
        XSNPRINTF(debug, sizeof(debug), "Importing DTLS session\n"
                   "\tVersion  : %d\n\tProtocol : %02X%01X\n\tLength of: %d\n\n"
               , (int)version, buf[0], (buf[1] >> 4), length);
        WOLFSSL_MSG(debug);
    }
#endif /* WOLFSSL_SESSION_EXPORT_DEBUG */

    /* perform sanity checks and extract Options information used */
    switch (version) {
        case DTLS_EXPORT_VERSION:
            optSz = DTLS_EXPORT_OPT_SZ;
            break;

        case DTLS_EXPORT_VERSION_3:
            WOLFSSL_MSG("Importing older version 3");
            optSz = DTLS_EXPORT_OPT_SZ_3;
            break;

        default:
            WOLFSSL_MSG("Bad export version");
            return BAD_FUNC_ARG;

    }

    if (DTLS_EXPORT_LEN + optSz + idx > sz) {
        WOLFSSL_MSG("Import Options struct error");
        return BUFFER_E;
    }
    ato16(buf + idx, &length); idx += DTLS_EXPORT_LEN;
    if (length != optSz) {
        WOLFSSL_MSG("Import Options struct error");
        return BUFFER_E;
    }
    if ((ret = dtls_export_load(ssl, buf + idx, length, version)) < 0) {
        WOLFSSL_MSG("Import Options struct error");
        return ret;
    }
    idx += length;

    /* perform sanity checks and extract Keys struct */
    if (DTLS_EXPORT_LEN + idx > sz) {
        WOLFSSL_MSG("Import Key struct error");
        return BUFFER_E;
    }
    ato16(buf + idx, &length); idx += DTLS_EXPORT_LEN;
    if (length > DTLS_EXPORT_KEY_SZ || length + idx > sz) {
        WOLFSSL_MSG("Import Key struct error");
        return BUFFER_E;
    }
    if ((ret = ImportKeyState(ssl, buf + idx, length, version)) < 0) {
        WOLFSSL_MSG("Import Key struct error");
        return ret;
    }
    idx += ret;

    /* perform sanity checks and extract CipherSpecs struct */
    if (DTLS_EXPORT_LEN + DTLS_EXPORT_SPC_SZ + idx > sz) {
        WOLFSSL_MSG("Import CipherSpecs struct error");
        return BUFFER_E;
    }
    ato16(buf + idx, &length); idx += DTLS_EXPORT_LEN;
    if ( length != DTLS_EXPORT_SPC_SZ) {
        WOLFSSL_MSG("Import CipherSpecs struct error");
        return BUFFER_E;
    }
    if ((ret = ImportCipherSpecState(ssl, buf + idx, length, version)) < 0) {
        WOLFSSL_MSG("Import CipherSpecs struct error");
        return ret;
    }
    idx += ret;

    /* perform sanity checks and extract DTLS peer info */
    if (DTLS_EXPORT_LEN + idx > sz) {
        WOLFSSL_MSG("Import DTLS peer info error");
        return BUFFER_E;
    }
    ato16(buf + idx, &length); idx += DTLS_EXPORT_LEN;
    if (idx + length > sz) {
        WOLFSSL_MSG("Import DTLS peer info error");
        return BUFFER_E;
    }
    if ((ret = ImportPeerInfo(ssl, buf + idx, length, version)) < 0) {
        WOLFSSL_MSG("Import Peer Addr error");
        return ret;
    }
    idx += ret;

    SetKeysSide(ssl, ENCRYPT_AND_DECRYPT_SIDE);

    /* set hmac function to use when verifying */
    if (ssl->options.tls == 1 || ssl->options.tls1_1 == 1 ||
            ssl->options.dtls == 1) {
        ssl->hmac = TLS_hmac;
    }

    /* make sure is a valid suite used */
    if (wolfSSL_get_cipher(ssl) == NULL) {
        WOLFSSL_MSG("Can not match cipher suite imported");
        return MATCH_SUITE_ERROR;
    }

    /* do not allow stream ciphers with DTLS */
    if (ssl->specs.cipher_type == stream) {
        WOLFSSL_MSG("Can not import stream ciphers for DTLS");
        return SANITY_CIPHER_E;
    }

    return idx;
}
#endif /* WOLFSSL_DTLS */
#endif /* WOLFSSL_SESSION_EXPORT */


void InitSSL_Method(WOLFSSL_METHOD* method, ProtocolVersion pv)
{
    method->version    = pv;
    method->side       = WOLFSSL_CLIENT_END;
    method->downgrade  = 0;
}


/* Initialize SSL context, return 0 on success */
int InitSSL_Ctx(WOLFSSL_CTX* ctx, WOLFSSL_METHOD* method, void* heap)
{
    int ret = 0;

    XMEMSET(ctx, 0, sizeof(WOLFSSL_CTX));

    ctx->method   = method;
    ctx->refCount = 1;          /* so either CTX_free or SSL_free can release */
    ctx->heap     = ctx;        /* defaults to self */
    ctx->timeout  = WOLFSSL_SESSION_TIMEOUT;
    ctx->minDowngrade = TLSv1_MINOR;     /* current default */

    if (wc_InitMutex(&ctx->countMutex) < 0) {
        WOLFSSL_MSG("Mutex error on CTX init");
        ctx->err = CTX_INIT_MUTEX_E;
        return BAD_MUTEX_E;
    }

#ifndef NO_DH
    ctx->minDhKeySz  = MIN_DHKEY_SZ;
#endif
#ifndef NO_RSA
    ctx->minRsaKeySz = MIN_RSAKEY_SZ;
#endif
#ifdef HAVE_ECC
    ctx->minEccKeySz  = MIN_ECCKEY_SZ;
    ctx->eccTempKeySz = ECDHE_SIZE;
#endif
#ifdef OPENSSL_EXTRA
    ctx->verifyDepth = MAX_CHAIN_DEPTH;
#endif

#ifndef WOLFSSL_USER_IO
    #ifdef MICRIUM
        ctx->CBIORecv = MicriumReceive;
        ctx->CBIOSend = MicriumSend;
        #ifdef WOLFSSL_DTLS
            if (method->version.major == DTLS_MAJOR) {
                ctx->CBIORecv   = MicriumReceiveFrom;
                ctx->CBIOSend   = MicriumSendTo;
            }
            #ifdef WOLFSSL_SESSION_EXPORT
                #error Micrium port does not support DTLS session export yet
            #endif
        #endif
    #else
        ctx->CBIORecv = EmbedReceive;
        ctx->CBIOSend = EmbedSend;
        #ifdef WOLFSSL_DTLS
            if (method->version.major == DTLS_MAJOR) {
                ctx->CBIORecv   = EmbedReceiveFrom;
                ctx->CBIOSend   = EmbedSendTo;
            }
            #ifdef WOLFSSL_SESSION_EXPORT
            ctx->CBGetPeer = EmbedGetPeer;
            ctx->CBSetPeer = EmbedSetPeer;
            #endif
        #endif
    #endif /* MICRIUM */
#endif /* WOLFSSL_USER_IO */

#ifdef HAVE_NETX
    ctx->CBIORecv = NetX_Receive;
    ctx->CBIOSend = NetX_Send;
#endif

#ifdef HAVE_NTRU
    if (method->side == WOLFSSL_CLIENT_END)
        ctx->haveNTRU = 1;           /* always on cliet side */
                                     /* server can turn on by loading key */
#endif
#ifdef HAVE_ECC
    if (method->side == WOLFSSL_CLIENT_END) {
        ctx->haveECDSAsig  = 1;        /* always on cliet side */
        ctx->haveECC  = 1;             /* server turns on with ECC key cert */
        ctx->haveStaticECC = 1;        /* server can turn on by loading key */
    }
#endif

    ctx->devId = INVALID_DEVID;

#if defined(WOLFSSL_DTLS) && defined(WOLFSSL_SCTP)
    ctx->dtlsMtuSz = MAX_RECORD_SIZE;
#endif

#ifndef NO_CERTS
    ctx->cm = wolfSSL_CertManagerNew_ex(heap);
    if (ctx->cm == NULL) {
        WOLFSSL_MSG("Bad Cert Manager New");
        return BAD_CERT_MANAGER_ERROR;
    }
    #ifdef OPENSSL_EXTRA
    /* setup WOLFSSL_X509_STORE */
    ctx->x509_store.cm = ctx->cm;
    #endif
#endif

#if defined(HAVE_EXTENDED_MASTER) && !defined(NO_WOLFSSL_CLIENT)
    if (method->side == WOLFSSL_CLIENT_END) {
        if ((method->version.major == SSLv3_MAJOR) &&
             (method->version.minor >= TLSv1_MINOR)) {

            ctx->haveEMS = 1;
        }
#ifdef WOLFSSL_DTLS
        if (method->version.major == DTLS_MAJOR)
            ctx->haveEMS = 1;
#endif /* WOLFSSL_DTLS */
    }
#endif /* HAVE_EXTENDED_MASTER && !NO_WOLFSSL_CLIENT */

#if defined(HAVE_SESSION_TICKET) && !defined(NO_WOLFSSL_SERVER)
    ctx->ticketHint = SESSION_TICKET_HINT_DEFAULT;
#endif

#ifdef HAVE_WOLF_EVENT
    ret = wolfEventQueue_Init(&ctx->event_queue);
#endif /* HAVE_WOLF_EVENT */

#ifdef WOLFSSL_EARLY_DATA
    ctx->maxEarlyDataSz = MAX_EARLY_DATA_SZ;
#endif

    ctx->heap = heap; /* wolfSSL_CTX_load_static_memory sets */

    return ret;
}


/* In case contexts are held in array and don't want to free actual ctx */
void SSL_CtxResourceFree(WOLFSSL_CTX* ctx)
{
#ifdef HAVE_CERTIFICATE_STATUS_REQUEST_V2
    int i;
#endif

#ifdef HAVE_WOLF_EVENT
    wolfEventQueue_Free(&ctx->event_queue);
#endif /* HAVE_WOLF_EVENT */

    XFREE(ctx->method, ctx->heap, DYNAMIC_TYPE_METHOD);
    if (ctx->suites)
        XFREE(ctx->suites, ctx->heap, DYNAMIC_TYPE_SUITES);

#ifndef NO_DH
    XFREE(ctx->serverDH_G.buffer, ctx->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    XFREE(ctx->serverDH_P.buffer, ctx->heap, DYNAMIC_TYPE_PUBLIC_KEY);
#endif /* !NO_DH */

#ifdef SINGLE_THREADED
    if (ctx->rng) {
        wc_FreeRng(ctx->rng);
        XFREE(ctx->rng, ctx->heap, DYNAMIC_TYPE_RNG);
    }
#endif /* SINGLE_THREADED */

#ifndef NO_CERTS
    FreeDer(&ctx->privateKey);
    FreeDer(&ctx->certificate);
    #ifdef KEEP_OUR_CERT
        if (ctx->ourCert && ctx->ownOurCert) {
            FreeX509(ctx->ourCert);
            XFREE(ctx->ourCert, ctx->heap, DYNAMIC_TYPE_X509);
        }
    #endif /* KEEP_OUR_CERT */
    FreeDer(&ctx->certChain);
    wolfSSL_CertManagerFree(ctx->cm);
    #ifdef OPENSSL_EXTRA
        while (ctx->ca_names != NULL) {
            WOLFSSL_STACK *next = ctx->ca_names->next;
            wolfSSL_X509_NAME_free(ctx->ca_names->data.name);
            XFREE(ctx->ca_names->data.name, NULL, DYNAMIC_TYPE_OPENSSL);
            XFREE(ctx->ca_names, NULL, DYNAMIC_TYPE_OPENSSL);
            ctx->ca_names = next;
        }
    #endif
    #if defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
        while (ctx->x509Chain != NULL) {
            WOLFSSL_STACK *next = ctx->x509Chain->next;
            wolfSSL_X509_free(ctx->x509Chain->data.x509);
            XFREE(ctx->x509Chain, NULL, DYNAMIC_TYPE_OPENSSL);
            ctx->x509Chain = next;
        }
    #endif
#endif /* !NO_CERTS */

#ifdef HAVE_TLS_EXTENSIONS
    TLSX_FreeAll(ctx->extensions, ctx->heap);

#ifndef NO_WOLFSSL_SERVER
#if defined(HAVE_CERTIFICATE_STATUS_REQUEST) \
 || defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
    if (ctx->certOcspRequest) {
        FreeOcspRequest(ctx->certOcspRequest);
        XFREE(ctx->certOcspRequest, ctx->heap, DYNAMIC_TYPE_OCSP_REQUEST);
    }
#endif

#ifdef HAVE_CERTIFICATE_STATUS_REQUEST_V2
    for (i = 0; i < MAX_CHAIN_DEPTH; i++) {
        if (ctx->chainOcspRequest[i]) {
            FreeOcspRequest(ctx->chainOcspRequest[i]);
            XFREE(ctx->chainOcspRequest[i], ctx->heap, DYNAMIC_TYPE_OCSP_REQUEST);
        }
    }
#endif /* HAVE_CERTIFICATE_STATUS_REQUEST_V2 */
#endif /* !NO_WOLFSSL_SERVER */

#endif /* HAVE_TLS_EXTENSIONS */

#ifdef WOLFSSL_STATIC_MEMORY
    if (ctx->heap != NULL) {
#ifdef WOLFSSL_HEAP_TEST
        /* avoid derefrencing a test value */
        if (ctx->heap != (void*)WOLFSSL_HEAP_TEST)
#endif
        {
            WOLFSSL_HEAP_HINT* hint = (WOLFSSL_HEAP_HINT*)(ctx->heap);
            wc_FreeMutex(&((WOLFSSL_HEAP*)(hint->memory))->memory_mutex);
        }
    }
#endif /* WOLFSSL_STATIC_MEMORY */
}


void FreeSSL_Ctx(WOLFSSL_CTX* ctx)
{
    int doFree = 0;

    if (wc_LockMutex(&ctx->countMutex) != 0) {
        WOLFSSL_MSG("Couldn't lock count mutex");

        /* check error state, if mutex error code then mutex init failed but
         * CTX was still malloc'd */
        if (ctx->err == CTX_INIT_MUTEX_E) {
            SSL_CtxResourceFree(ctx);
            XFREE(ctx, ctx->heap, DYNAMIC_TYPE_CTX);
        }
        return;
    }
    ctx->refCount--;
    if (ctx->refCount == 0)
        doFree = 1;
    wc_UnLockMutex(&ctx->countMutex);

    if (doFree) {
        WOLFSSL_MSG("CTX ref count down to 0, doing full free");
        SSL_CtxResourceFree(ctx);
        wc_FreeMutex(&ctx->countMutex);
        XFREE(ctx, ctx->heap, DYNAMIC_TYPE_CTX);
    }
    else {
        (void)ctx;
        WOLFSSL_MSG("CTX ref count not 0 yet, no free");
    }
}


/* Set cipher pointers to null */
void InitCiphers(WOLFSSL* ssl)
{
#ifdef BUILD_ARC4
    ssl->encrypt.arc4 = NULL;
    ssl->decrypt.arc4 = NULL;
#endif
#ifdef BUILD_DES3
    ssl->encrypt.des3 = NULL;
    ssl->decrypt.des3 = NULL;
#endif
#ifdef BUILD_AES
    ssl->encrypt.aes = NULL;
    ssl->decrypt.aes = NULL;
#endif
#ifdef HAVE_CAMELLIA
    ssl->encrypt.cam = NULL;
    ssl->decrypt.cam = NULL;
#endif
#ifdef HAVE_HC128
    ssl->encrypt.hc128 = NULL;
    ssl->decrypt.hc128 = NULL;
#endif
#ifdef BUILD_RABBIT
    ssl->encrypt.rabbit = NULL;
    ssl->decrypt.rabbit = NULL;
#endif
#ifdef HAVE_CHACHA
    ssl->encrypt.chacha = NULL;
    ssl->decrypt.chacha = NULL;
#endif
#ifdef HAVE_POLY1305
    ssl->auth.poly1305 = NULL;
#endif
    ssl->encrypt.setup = 0;
    ssl->decrypt.setup = 0;
#ifdef HAVE_ONE_TIME_AUTH
    ssl->auth.setup    = 0;
#endif
#ifdef HAVE_IDEA
    ssl->encrypt.idea = NULL;
    ssl->decrypt.idea = NULL;
#endif
}


/* Free ciphers */
void FreeCiphers(WOLFSSL* ssl)
{
    (void)ssl;
#ifdef BUILD_ARC4
    wc_Arc4Free(ssl->encrypt.arc4);
    wc_Arc4Free(ssl->decrypt.arc4);
    XFREE(ssl->encrypt.arc4, ssl->heap, DYNAMIC_TYPE_CIPHER);
    XFREE(ssl->decrypt.arc4, ssl->heap, DYNAMIC_TYPE_CIPHER);
#endif
#ifdef BUILD_DES3
    wc_Des3Free(ssl->encrypt.des3);
    wc_Des3Free(ssl->decrypt.des3);
    XFREE(ssl->encrypt.des3, ssl->heap, DYNAMIC_TYPE_CIPHER);
    XFREE(ssl->decrypt.des3, ssl->heap, DYNAMIC_TYPE_CIPHER);
#endif
#ifdef BUILD_AES
    wc_AesFree(ssl->encrypt.aes);
    wc_AesFree(ssl->decrypt.aes);
    #if defined(BUILD_AESGCM) || defined(HAVE_AESCCM)
        XFREE(ssl->decrypt.additional, ssl->heap, DYNAMIC_TYPE_AES_BUFFER);
        XFREE(ssl->decrypt.nonce, ssl->heap, DYNAMIC_TYPE_AES_BUFFER);
        XFREE(ssl->encrypt.additional, ssl->heap, DYNAMIC_TYPE_AES_BUFFER);
        XFREE(ssl->encrypt.nonce, ssl->heap, DYNAMIC_TYPE_AES_BUFFER);
    #endif
    XFREE(ssl->encrypt.aes, ssl->heap, DYNAMIC_TYPE_CIPHER);
    XFREE(ssl->decrypt.aes, ssl->heap, DYNAMIC_TYPE_CIPHER);
#endif
#ifdef HAVE_CAMELLIA
    XFREE(ssl->encrypt.cam, ssl->heap, DYNAMIC_TYPE_CIPHER);
    XFREE(ssl->decrypt.cam, ssl->heap, DYNAMIC_TYPE_CIPHER);
#endif
#ifdef HAVE_HC128
    XFREE(ssl->encrypt.hc128, ssl->heap, DYNAMIC_TYPE_CIPHER);
    XFREE(ssl->decrypt.hc128, ssl->heap, DYNAMIC_TYPE_CIPHER);
#endif
#ifdef BUILD_RABBIT
    XFREE(ssl->encrypt.rabbit, ssl->heap, DYNAMIC_TYPE_CIPHER);
    XFREE(ssl->decrypt.rabbit, ssl->heap, DYNAMIC_TYPE_CIPHER);
#endif
#ifdef HAVE_CHACHA
    XFREE(ssl->encrypt.chacha, ssl->heap, DYNAMIC_TYPE_CIPHER);
    XFREE(ssl->decrypt.chacha, ssl->heap, DYNAMIC_TYPE_CIPHER);
#endif
#ifdef HAVE_POLY1305
    XFREE(ssl->auth.poly1305, ssl->heap, DYNAMIC_TYPE_CIPHER);
#endif
#ifdef HAVE_IDEA
    XFREE(ssl->encrypt.idea, ssl->heap, DYNAMIC_TYPE_CIPHER);
    XFREE(ssl->decrypt.idea, ssl->heap, DYNAMIC_TYPE_CIPHER);
#endif
}


void InitCipherSpecs(CipherSpecs* cs)
{
    cs->bulk_cipher_algorithm = INVALID_BYTE;
    cs->cipher_type           = INVALID_BYTE;
    cs->mac_algorithm         = INVALID_BYTE;
    cs->kea                   = INVALID_BYTE;
    cs->sig_algo              = INVALID_BYTE;

    cs->hash_size   = 0;
    cs->static_ecdh = 0;
    cs->key_size    = 0;
    cs->iv_size     = 0;
    cs->block_size  = 0;
}

void InitSuitesHashSigAlgo(Suites* suites, int haveECDSAsig, int haveRSAsig,
                           int haveAnon, int tls1_2, int keySz)
{
    int idx = 0;

    (void)tls1_2;
    (void)keySz;

    if (haveECDSAsig) {
        #ifdef WOLFSSL_SHA512
            suites->hashSigAlgo[idx++] = sha512_mac;
            suites->hashSigAlgo[idx++] = ecc_dsa_sa_algo;
        #endif
        #ifdef WOLFSSL_SHA384
            suites->hashSigAlgo[idx++] = sha384_mac;
            suites->hashSigAlgo[idx++] = ecc_dsa_sa_algo;
        #endif
        #ifndef NO_SHA256
            suites->hashSigAlgo[idx++] = sha256_mac;
            suites->hashSigAlgo[idx++] = ecc_dsa_sa_algo;
        #endif
        #if !defined(NO_SHA) && (!defined(NO_OLD_TLS) || \
                                                defined(WOLFSSL_ALLOW_TLS_SHA1))
            suites->hashSigAlgo[idx++] = sha_mac;
            suites->hashSigAlgo[idx++] = ecc_dsa_sa_algo;
        #endif
        #ifdef HAVE_ED25519
            suites->hashSigAlgo[idx++] = ED25519_SA_MAJOR;
            suites->hashSigAlgo[idx++] = ED25519_SA_MINOR;
        #endif
    }

    if (haveRSAsig) {
        #ifdef WC_RSA_PSS
            if (tls1_2) {
            #ifdef WOLFSSL_SHA512
                if (keySz >= MIN_RSA_SHA512_PSS_BITS) {
                    suites->hashSigAlgo[idx++] = rsa_pss_sa_algo;
                    suites->hashSigAlgo[idx++] = sha512_mac;
                }
            #endif
            #ifdef WOLFSSL_SHA384
                if (keySz >= MIN_RSA_SHA384_PSS_BITS) {
                  suites->hashSigAlgo[idx++] = rsa_pss_sa_algo;
                  suites->hashSigAlgo[idx++] = sha384_mac;
                }
            #endif
            #ifndef NO_SHA256
                suites->hashSigAlgo[idx++] = rsa_pss_sa_algo;
                suites->hashSigAlgo[idx++] = sha256_mac;
            #endif
            }
        #endif
        #ifdef WOLFSSL_SHA512
            suites->hashSigAlgo[idx++] = sha512_mac;
            suites->hashSigAlgo[idx++] = rsa_sa_algo;
        #endif
        #ifdef WOLFSSL_SHA384
            suites->hashSigAlgo[idx++] = sha384_mac;
            suites->hashSigAlgo[idx++] = rsa_sa_algo;
        #endif
        #ifndef NO_SHA256
            suites->hashSigAlgo[idx++] = sha256_mac;
            suites->hashSigAlgo[idx++] = rsa_sa_algo;
        #endif
        #if !defined(NO_SHA) && (!defined(NO_OLD_TLS) || \
                                                defined(WOLFSSL_ALLOW_TLS_SHA1))
            suites->hashSigAlgo[idx++] = sha_mac;
            suites->hashSigAlgo[idx++] = rsa_sa_algo;
        #endif
    }

    if (haveAnon) {
        #ifdef HAVE_ANON
            suites->hashSigAlgo[idx++] = sha_mac;
            suites->hashSigAlgo[idx++] = anonymous_sa_algo;
        #endif
    }

    suites->hashSigAlgoSz = (word16)idx;
}

void InitSuites(Suites* suites, ProtocolVersion pv, int keySz, word16 haveRSA,
                word16 havePSK, word16 haveDH, word16 haveNTRU,
                word16 haveECDSAsig, word16 haveECC,
                word16 haveStaticECC, int side)
{
    word16 idx = 0;
    int    tls    = pv.major == SSLv3_MAJOR && pv.minor >= TLSv1_MINOR;
    int    tls1_2 = pv.major == SSLv3_MAJOR && pv.minor >= TLSv1_2_MINOR;
#ifdef WOLFSSL_TLS13
    int    tls1_3 = IsAtLeastTLSv1_3(pv);
#endif
    int    dtls   = 0;
    int    haveRSAsig = 1;

    (void)tls;  /* shut up compiler */
    (void)tls1_2;
    (void)dtls;
    (void)haveDH;
    (void)havePSK;
    (void)haveNTRU;
    (void)haveStaticECC;
    (void)haveECC;

    if (suites == NULL) {
        WOLFSSL_MSG("InitSuites pointer error");
        return;
    }

    if (suites->setSuites)
        return;      /* trust user settings, don't override */

    if (side == WOLFSSL_SERVER_END && haveStaticECC) {
        haveRSA = 0;   /* can't do RSA with ECDSA key */
        (void)haveRSA; /* some builds won't read */
    }

    if (side == WOLFSSL_SERVER_END && haveECDSAsig) {
        haveRSAsig = 0;     /* can't have RSA sig if signed by ECDSA */
        (void)haveRSAsig;   /* non ecc builds won't read */
    }

#ifdef WOLFSSL_DTLS
    if (pv.major == DTLS_MAJOR) {
        dtls   = 1;
        tls    = 1;
        /* May be dead assignments dependant upon configuration */
        (void) dtls;
        (void) tls;
        tls1_2 = pv.minor <= DTLSv1_2_MINOR;
    }
#endif

#ifdef HAVE_RENEGOTIATION_INDICATION
    if (side == WOLFSSL_CLIENT_END) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_EMPTY_RENEGOTIATION_INFO_SCSV;
    }
#endif

#ifdef BUILD_TLS_QSH
    if (tls) {
        suites->suites[idx++] = QSH_BYTE;
        suites->suites[idx++] = TLS_QSH;
    }
#endif

#ifdef BUILD_TLS_NTRU_RSA_WITH_AES_256_CBC_SHA
   if (tls && haveNTRU && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_NTRU_RSA_WITH_AES_256_CBC_SHA;
   }
#endif

#ifdef BUILD_TLS_NTRU_RSA_WITH_AES_128_CBC_SHA
    if (tls && haveNTRU && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_NTRU_RSA_WITH_AES_128_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_NTRU_RSA_WITH_RC4_128_SHA
    if (!dtls && tls && haveNTRU && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_NTRU_RSA_WITH_RC4_128_SHA;
    }
#endif

#ifdef BUILD_TLS_NTRU_RSA_WITH_3DES_EDE_CBC_SHA
    if (tls && haveNTRU && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_NTRU_RSA_WITH_3DES_EDE_CBC_SHA;
    }
#endif

#ifdef WOLFSSL_TLS13
#ifdef BUILD_TLS_AES_128_GCM_SHA256
    if (tls1_3) {
        suites->suites[idx++] = TLS13_BYTE;
        suites->suites[idx++] = TLS_AES_128_GCM_SHA256;
    }
#endif

#ifdef BUILD_TLS_AES_256_GCM_SHA384
    if (tls1_3) {
        suites->suites[idx++] = TLS13_BYTE;
        suites->suites[idx++] = TLS_AES_256_GCM_SHA384;
    }
#endif

#ifdef BUILD_TLS_CHACHA20_POLY1305_SHA256
    if (tls1_3) {
        suites->suites[idx++] = TLS13_BYTE;
        suites->suites[idx++] = TLS_CHACHA20_POLY1305_SHA256;
    }
#endif

#ifdef BUILD_TLS_AES_128_CCM_SHA256
    if (tls1_3) {
        suites->suites[idx++] = TLS13_BYTE;
        suites->suites[idx++] = TLS_AES_128_CCM_SHA256;
    }
#endif

#ifdef BUILD_TLS_AES_128_CCM_8_SHA256
    if (tls1_3) {
        suites->suites[idx++] = TLS13_BYTE;
        suites->suites[idx++] = TLS_AES_128_CCM_8_SHA256;
    }
#endif
#endif /* WOLFSSL_TLS13 */

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384
    if (tls1_2 && haveECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384;
    }
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256
    if (tls1_2 && haveECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256;
    }
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384
    if (tls1_2 && haveRSA) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384;
    }
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256
    if (tls1_2 && haveRSA) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256;
    }
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384
    if (tls1_2 && haveDH && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_DHE_RSA_WITH_AES_256_GCM_SHA384;
    }
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256
    if (tls1_2 && haveDH && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_DHE_RSA_WITH_AES_128_GCM_SHA256;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_256_GCM_SHA384
    if (tls1_2 && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_RSA_WITH_AES_256_GCM_SHA384;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_128_GCM_SHA256
    if (tls1_2 && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_RSA_WITH_AES_128_GCM_SHA256;
    }
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384
    if (tls1_2 && haveECC && haveStaticECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384;
    }
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256
    if (tls1_2 && haveECC && haveStaticECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256;
    }
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384
    if (tls1_2 && haveRSAsig && haveStaticECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384;
    }
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256
    if (tls1_2 && haveRSAsig && haveStaticECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256;
    }
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_AES_256_GCM_SHA384
    if (tls1_2 && haveDH && havePSK) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_DHE_PSK_WITH_AES_256_GCM_SHA384;
    }
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_AES_128_GCM_SHA256
    if (tls1_2 && haveDH && havePSK) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_DHE_PSK_WITH_AES_128_GCM_SHA256;
    }
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_256_GCM_SHA384
    if (tls1_2 && havePSK) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_PSK_WITH_AES_256_GCM_SHA384;
    }
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_128_GCM_SHA256
    if (tls1_2 && havePSK) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_PSK_WITH_AES_128_GCM_SHA256;
    }
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256
    if (tls1_2 && haveECC) {
        suites->suites[idx++] = CHACHA_BYTE;
        suites->suites[idx++] = TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256;
    }
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256
    if (tls1_2 && haveRSA) {
        suites->suites[idx++] = CHACHA_BYTE;
        suites->suites[idx++] = TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256;
    }
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256
    if (tls1_2 && haveRSA) {
        suites->suites[idx++] = CHACHA_BYTE;
        suites->suites[idx++] = TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256;
    }
#endif

/* Place as higher priority for MYSQL */
#if defined(WOLFSSL_MYSQL_COMPATIBLE)
#ifdef BUILD_TLS_DHE_RSA_WITH_AES_256_CBC_SHA
    if (tls && haveDH && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_DHE_RSA_WITH_AES_256_CBC_SHA;
    }
#endif
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256
    if (tls1_2 && haveRSA) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256;
    }
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256
    if (tls1_2 && haveECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256;
    }
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256
    if (tls1_2 && haveRSAsig && haveStaticECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256;
    }
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256
    if (tls1_2 && haveECC && haveStaticECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256;
    }
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384
    if (tls1_2 && haveRSA) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384;
    }
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384
    if (tls1_2 && haveECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384;
    }
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384
    if (tls1_2 && haveRSAsig && haveStaticECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384;
    }
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384
    if (tls1_2 && haveECC && haveStaticECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384;
    }
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA
    if (tls && haveECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA
    if (tls && haveECC && haveStaticECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA
    if (tls && haveECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA
    if (tls && haveECC && haveStaticECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_RC4_128_SHA
    if (!dtls && tls && haveECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_ECDSA_WITH_RC4_128_SHA;
    }
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_RC4_128_SHA
    if (!dtls && tls && haveECC && haveStaticECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDH_ECDSA_WITH_RC4_128_SHA;
    }
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA
    if (tls && haveECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA
    if (tls && haveECC && haveStaticECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA
    if (tls && haveRSA) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA
    if (tls && haveRSAsig && haveStaticECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDH_RSA_WITH_AES_256_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA
    if (tls && haveRSA) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA
    if (tls && haveRSAsig && haveStaticECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDH_RSA_WITH_AES_128_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_RC4_128_SHA
    if (!dtls && tls && haveRSA) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_RSA_WITH_RC4_128_SHA;
    }
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_RC4_128_SHA
    if (!dtls && tls && haveRSAsig && haveStaticECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDH_RSA_WITH_RC4_128_SHA;
    }
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA
    if (tls && haveRSA) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA
    if (tls && haveRSAsig && haveStaticECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_CCM
    if (tls1_2 && haveECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_ECDSA_WITH_AES_128_CCM;
    }
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8
    if (tls1_2 && haveECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8;
    }
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8
    if (tls1_2 && haveECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_128_CCM_8
    if (tls1_2 && haveRSA) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_RSA_WITH_AES_128_CCM_8;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_256_CCM_8
    if (tls1_2 && haveRSA) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_RSA_WITH_AES_256_CCM_8;
    }
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256
    if (tls && haveDH && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_DHE_RSA_WITH_AES_256_CBC_SHA256;
    }
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256
    if (tls && haveDH && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_DHE_RSA_WITH_AES_128_CBC_SHA256;
    }
#endif

/* Place as higher priority for MYSQL testing */
#if !defined(WOLFSSL_MYSQL_COMPATIBLE)
#ifdef BUILD_TLS_DHE_RSA_WITH_AES_256_CBC_SHA
    if (tls && haveDH && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_DHE_RSA_WITH_AES_256_CBC_SHA;
    }
#endif
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_AES_128_CBC_SHA
    if (tls && haveDH && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_DHE_RSA_WITH_AES_128_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA
    if (tls && haveDH && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_256_CBC_SHA256
    if (tls && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_RSA_WITH_AES_256_CBC_SHA256;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_128_CBC_SHA256
    if (tls && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_RSA_WITH_AES_128_CBC_SHA256;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_256_CBC_SHA
    if (tls && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_RSA_WITH_AES_256_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_128_CBC_SHA
    if (tls && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_RSA_WITH_AES_128_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_CHACHA20_OLD_POLY1305_SHA256
    if (tls1_2 && haveECC) {
        suites->suites[idx++] = CHACHA_BYTE;
        suites->suites[idx++] =
                              TLS_ECDHE_ECDSA_WITH_CHACHA20_OLD_POLY1305_SHA256;
    }
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256
    if (tls1_2 && haveRSA) {
        suites->suites[idx++] = CHACHA_BYTE;
        suites->suites[idx++] = TLS_ECDHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256;
    }
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256
    if (tls1_2 && haveRSA) {
        suites->suites[idx++] = CHACHA_BYTE;
        suites->suites[idx++] = TLS_DHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256;
    }
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_NULL_SHA
    if (tls && haveECC) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_ECDSA_WITH_NULL_SHA;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_NULL_SHA
    if (tls && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_RSA_WITH_NULL_SHA;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_NULL_SHA256
    if (tls && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_RSA_WITH_NULL_SHA256;
    }
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_256_CBC_SHA
    if (tls && havePSK) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_PSK_WITH_AES_256_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_AES_256_CBC_SHA384
    if (tls && haveDH && havePSK) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_DHE_PSK_WITH_AES_256_CBC_SHA384;
    }
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_256_CBC_SHA384
    if (tls && havePSK) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_PSK_WITH_AES_256_CBC_SHA384;
    }
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_AES_128_CBC_SHA256
    if (tls && haveDH && havePSK) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_DHE_PSK_WITH_AES_128_CBC_SHA256;
    }
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_128_CBC_SHA256
    if (tls && havePSK) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_PSK_WITH_AES_128_CBC_SHA256;
    }
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_128_CBC_SHA
    if (tls && havePSK) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_PSK_WITH_AES_128_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_AES_128_CCM
    if (tls && haveDH && havePSK) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_DHE_PSK_WITH_AES_128_CCM;
    }
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_AES_256_CCM
    if (tls && haveDH && havePSK) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_DHE_PSK_WITH_AES_256_CCM;
    }
#endif

#ifdef BUILD_TLS_PSK_WITH_CHACHA20_POLY1305_SHA256
    if (tls && havePSK) {
        suites->suites[idx++] = CHACHA_BYTE;
        suites->suites[idx++] = TLS_PSK_WITH_CHACHA20_POLY1305_SHA256;
    }
#endif

#ifdef BUILD_TLS_ECDHE_PSK_WITH_CHACHA20_POLY1305_SHA256
    if (tls && havePSK) {
        suites->suites[idx++] = CHACHA_BYTE;
        suites->suites[idx++] = TLS_ECDHE_PSK_WITH_CHACHA20_POLY1305_SHA256;
    }
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_CHACHA20_POLY1305_SHA256
    if (tls && havePSK) {
        suites->suites[idx++] = CHACHA_BYTE;
        suites->suites[idx++] = TLS_DHE_PSK_WITH_CHACHA20_POLY1305_SHA256;
    }
#endif

#ifdef BUILD_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256
    if (tls && havePSK) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256;
    }
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_128_CCM
    if (tls && havePSK) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_PSK_WITH_AES_128_CCM;
    }
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_256_CCM
    if (tls && havePSK) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_PSK_WITH_AES_256_CCM;
    }
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_128_CCM_8
    if (tls && havePSK) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_PSK_WITH_AES_128_CCM_8;
    }
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_256_CCM_8
    if (tls && havePSK) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_PSK_WITH_AES_256_CCM_8;
    }
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_NULL_SHA384
    if (tls && haveDH && havePSK) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_DHE_PSK_WITH_NULL_SHA384;
    }
#endif

#ifdef BUILD_TLS_PSK_WITH_NULL_SHA384
    if (tls && havePSK) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_PSK_WITH_NULL_SHA384;
    }
#endif

#ifdef BUILD_TLS_ECDHE_PSK_WITH_NULL_SHA256
    if (tls && havePSK) {
        suites->suites[idx++] = ECC_BYTE;
        suites->suites[idx++] = TLS_ECDHE_PSK_WITH_NULL_SHA256;
    }
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_NULL_SHA256
    if (tls && haveDH && havePSK) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_DHE_PSK_WITH_NULL_SHA256;
    }
#endif

#ifdef BUILD_TLS_PSK_WITH_NULL_SHA256
    if (tls && havePSK) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_PSK_WITH_NULL_SHA256;
    }
#endif

#ifdef BUILD_TLS_PSK_WITH_NULL_SHA
    if (tls && havePSK) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_PSK_WITH_NULL_SHA;
    }
#endif

#ifdef BUILD_SSL_RSA_WITH_RC4_128_SHA
    if (!dtls && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = SSL_RSA_WITH_RC4_128_SHA;
    }
#endif

#ifdef BUILD_SSL_RSA_WITH_RC4_128_MD5
    if (!dtls && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = SSL_RSA_WITH_RC4_128_MD5;
    }
#endif

#ifdef BUILD_SSL_RSA_WITH_3DES_EDE_CBC_SHA
    if (haveRSA ) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = SSL_RSA_WITH_3DES_EDE_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_HC_128_MD5
    if (!dtls && tls && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_RSA_WITH_HC_128_MD5;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_HC_128_SHA
    if (!dtls && tls && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_RSA_WITH_HC_128_SHA;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_HC_128_B2B256
    if (!dtls && tls && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_RSA_WITH_HC_128_B2B256;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_128_CBC_B2B256
    if (tls && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_RSA_WITH_AES_128_CBC_B2B256;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_256_CBC_B2B256
    if (tls && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_RSA_WITH_AES_256_CBC_B2B256;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_RABBIT_SHA
    if (!dtls && tls && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_RSA_WITH_RABBIT_SHA;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_CAMELLIA_128_CBC_SHA
    if (tls && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_RSA_WITH_CAMELLIA_128_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA
    if (tls && haveDH && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_CAMELLIA_256_CBC_SHA
    if (tls && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_RSA_WITH_CAMELLIA_256_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA
    if (tls && haveDH && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256
    if (tls && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256;
    }
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256
    if (tls && haveDH && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256;
    }
#endif

#ifdef BUILD_TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256
    if (tls && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256;
    }
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256
    if (tls && haveDH && haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256;
    }
#endif

#ifdef BUILD_SSL_RSA_WITH_IDEA_CBC_SHA
    if (haveRSA) {
        suites->suites[idx++] = 0;
        suites->suites[idx++] = SSL_RSA_WITH_IDEA_CBC_SHA;
    }
#endif

    suites->suiteSz = idx;

    InitSuitesHashSigAlgo(suites, haveECDSAsig, haveRSAsig, 0, tls1_2, keySz);
}

#if !defined(NO_WOLFSSL_SERVER) || !defined(NO_CERTS) || \
    (!defined(NO_WOLFSSL_CLIENT) && (!defined(NO_DH) || defined(HAVE_ECC)))

/* Decode the signature algorithm.
 *
 * input     The encoded signature algorithm.
 * hashalgo  The hash algorithm.
 * hsType   The signature type.
 */
static INLINE void DecodeSigAlg(const byte* input, byte* hashAlgo, byte* hsType)
{
    switch (input[0]) {
        case NEW_SA_MAJOR:
    #ifdef WC_RSA_PSS
            /* PSS signatures: 0x080[4-6] */
            if (input[1] <= sha512_mac) {
                *hsType   = input[0];
                *hashAlgo = input[1];
            }
    #endif
    #ifdef HAVE_ED25519
            /* ED25519: 0x0807 */
            if (input[1] == ED25519_SA_MINOR) {
                *hsType = ed25519_sa_algo;
                /* Hash performed as part of sign/verify operation. */
                *hashAlgo = sha512_mac;
            }
    #endif
            /* ED448: 0x0808 */
            break;
        default:
            *hashAlgo = input[0];
            *hsType   = input[1];
            break;
    }
}
#endif /* !NO_WOLFSSL_SERVER || !NO_CERTS */

#if !defined(NO_DH) || defined(HAVE_ECC)

static enum wc_HashType HashAlgoToType(int hashAlgo)
{
    switch (hashAlgo) {
    #ifdef WOLFSSL_SHA512
        case sha512_mac:
            return WC_HASH_TYPE_SHA512;
    #endif
    #ifdef WOLFSSL_SHA384
        case sha384_mac:
            return WC_HASH_TYPE_SHA384;
    #endif
    #ifndef NO_SHA256
        case sha256_mac:
            return WC_HASH_TYPE_SHA256;
    #endif
    #if !defined(NO_SHA) && (!defined(NO_OLD_TLS) || \
                             defined(WOLFSSL_ALLOW_TLS_SHA1))
        case sha_mac:
            return WC_HASH_TYPE_SHA;
    #endif
        default:
            WOLFSSL_MSG("Bad hash sig algo");
            break;
    }

    return WC_HASH_TYPE_NONE;
}

#ifndef NO_CERTS


void InitX509Name(WOLFSSL_X509_NAME* name, int dynamicFlag)
{
    (void)dynamicFlag;

    if (name != NULL) {
        name->name        = name->staticName;
        name->dynamicName = 0;
#ifdef OPENSSL_EXTRA
        XMEMSET(&name->fullName, 0, sizeof(DecodedName));
        XMEMSET(&name->cnEntry,  0, sizeof(WOLFSSL_X509_NAME_ENTRY));
        name->cnEntry.value = &(name->cnEntry.data); /* point to internal data*/
        name->x509 = NULL;
#endif /* OPENSSL_EXTRA */
    }
}


void FreeX509Name(WOLFSSL_X509_NAME* name, void* heap)
{
    if (name != NULL) {
        if (name->dynamicName)
            XFREE(name->name, heap, DYNAMIC_TYPE_SUBJECT_CN);
#ifdef OPENSSL_EXTRA
        if (name->fullName.fullName != NULL)
            XFREE(name->fullName.fullName, heap, DYNAMIC_TYPE_X509);
#endif /* OPENSSL_EXTRA */
    }
    (void)heap;
}


/* Initialize wolfSSL X509 type */
void InitX509(WOLFSSL_X509* x509, int dynamicFlag, void* heap)
{
    if (x509 == NULL) {
        WOLFSSL_MSG("Null parameter passed in!");
        return;
    }

    XMEMSET(x509, 0, sizeof(WOLFSSL_X509));

    x509->heap = heap;
    InitX509Name(&x509->issuer, 0);
    InitX509Name(&x509->subject, 0);
    x509->version        = 0;
    x509->pubKey.buffer  = NULL;
    x509->sig.buffer     = NULL;
    x509->derCert        = NULL;
    x509->altNames       = NULL;
    x509->altNamesNext   = NULL;
    x509->dynamicMemory  = (byte)dynamicFlag;
    x509->isCa           = 0;
#ifdef HAVE_ECC
    x509->pkCurveOID = 0;
#endif /* HAVE_ECC */
#ifdef OPENSSL_EXTRA
    x509->pathLength     = 0;
    x509->basicConstSet  = 0;
    x509->basicConstCrit = 0;
    x509->basicConstPlSet = 0;
    x509->subjAltNameSet = 0;
    x509->subjAltNameCrit = 0;
    x509->authKeyIdSet   = 0;
    x509->authKeyIdCrit  = 0;
    x509->authKeyId      = NULL;
    x509->authKeyIdSz    = 0;
    x509->subjKeyIdSet   = 0;
    x509->subjKeyIdCrit  = 0;
    x509->subjKeyId      = NULL;
    x509->subjKeyIdSz    = 0;
    x509->keyUsageSet    = 0;
    x509->keyUsageCrit   = 0;
    x509->keyUsage       = 0;
    #ifdef WOLFSSL_SEP
        x509->certPolicySet  = 0;
        x509->certPolicyCrit = 0;
    #endif /* WOLFSSL_SEP */
#endif /* OPENSSL_EXTRA */
}


/* Free wolfSSL X509 type */
void FreeX509(WOLFSSL_X509* x509)
{
    if (x509 == NULL)
        return;

    FreeX509Name(&x509->issuer, x509->heap);
    FreeX509Name(&x509->subject, x509->heap);
    if (x509->pubKey.buffer)
        XFREE(x509->pubKey.buffer, x509->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    FreeDer(&x509->derCert);
    XFREE(x509->sig.buffer, x509->heap, DYNAMIC_TYPE_SIGNATURE);
    #ifdef OPENSSL_EXTRA
        XFREE(x509->authKeyId, x509->heap, DYNAMIC_TYPE_X509_EXT);
        XFREE(x509->subjKeyId, x509->heap, DYNAMIC_TYPE_X509_EXT);
        if (x509->authInfo != NULL) {
            XFREE(x509->authInfo, x509->heap, DYNAMIC_TYPE_X509_EXT);
        }
        if (x509->extKeyUsageSrc != NULL) {
            XFREE(x509->extKeyUsageSrc, x509->heap, DYNAMIC_TYPE_X509_EXT);
        }
    #endif /* OPENSSL_EXTRA */
    if (x509->altNames)
        FreeAltNames(x509->altNames, NULL);
}

#endif /* !NO_DH || HAVE_ECC */

#if !defined(NO_RSA) || defined(HAVE_ECC)
/* Encode the signature algorithm into buffer.
 *
 * hashalgo  The hash algorithm.
 * hsType   The signature type.
 * output    The buffer to encode into.
 */
static INLINE void EncodeSigAlg(byte hashAlgo, byte hsType, byte* output)
{
    switch (hsType) {
#ifdef HAVE_ECC
        case ecc_dsa_sa_algo:
            output[0] = hashAlgo;
            output[1] = ecc_dsa_sa_algo;
            break;
    #ifdef HAVE_ED25519
        case ed25519_sa_algo:
            output[0] = ED25519_SA_MAJOR;
            output[1] = ED25519_SA_MINOR;
            break;
    #endif
#endif
#ifndef NO_RSA
        case rsa_sa_algo:
            output[0] = hashAlgo;
            output[1] = rsa_sa_algo;
            break;
    #ifdef WC_RSA_PSS
        /* PSS signatures: 0x080[4-6] */
        case rsa_pss_sa_algo:
            output[0] = rsa_pss_sa_algo;
            output[1] = hashAlgo;
            break;
    #endif
#endif
        /* ED448: 0x0808 */
    }
}
static void SetDigest(WOLFSSL* ssl, int hashAlgo)
{
    switch (hashAlgo) {
        #ifndef NO_SHA
        case sha_mac:
            ssl->buffers.digest.buffer = ssl->hsHashes->certHashes.sha;
            ssl->buffers.digest.length = SHA_DIGEST_SIZE;
            break;
        #endif /* !NO_SHA */
        #ifndef NO_SHA256
        case sha256_mac:
            ssl->buffers.digest.buffer = ssl->hsHashes->certHashes.sha256;
            ssl->buffers.digest.length = SHA256_DIGEST_SIZE;
            break;
        #endif /* !NO_SHA256 */
        #ifdef WOLFSSL_SHA384
        case sha384_mac:
            ssl->buffers.digest.buffer = ssl->hsHashes->certHashes.sha384;
            ssl->buffers.digest.length = SHA384_DIGEST_SIZE;
            break;
        #endif /* WOLFSSL_SHA384 */
        #ifdef WOLFSSL_SHA512
        case sha512_mac:
            ssl->buffers.digest.buffer = ssl->hsHashes->certHashes.sha512;
            ssl->buffers.digest.length = SHA512_DIGEST_SIZE;
            break;
        #endif /* WOLFSSL_SHA512 */
    } /* switch */
}
#endif

#ifndef NO_RSA
static int TypeHash(int hashAlgo)
{
    switch (hashAlgo) {
    #ifdef WOLFSSL_SHA512
        case sha512_mac:
            return SHA512h;
    #endif
    #ifdef WOLFSSL_SHA384
        case sha384_mac:
            return SHA384h;
    #endif
    #ifndef NO_SHA256
        case sha256_mac:
            return SHA256h;
    #endif
    #ifndef NO_SHA
        case sha_mac:
            return SHAh;
    #endif
    }

    return 0;
}

#if defined(WC_RSA_PSS)
int ConvertHashPss(int hashAlgo, enum wc_HashType* hashType, int* mgf)
{
    switch (hashAlgo) {
        #ifdef WOLFSSL_SHA512
        case sha512_mac:
            *hashType = WC_HASH_TYPE_SHA512;
            if (mgf != NULL)
                *mgf = WC_MGF1SHA512;
            break;
        #endif
        #ifdef WOLFSSL_SHA384
        case sha384_mac:
            *hashType = WC_HASH_TYPE_SHA384;
            if (mgf != NULL)
                *mgf = WC_MGF1SHA384;
            break;
        #endif
        #ifndef NO_SHA256
        case sha256_mac:
            *hashType = WC_HASH_TYPE_SHA256;
            if (mgf != NULL)
                *mgf = WC_MGF1SHA256;
            break;
        #endif
        default:
            return BAD_FUNC_ARG;
    }

    return 0;
}
#endif

int RsaSign(WOLFSSL* ssl, const byte* in, word32 inSz, byte* out,
            word32* outSz, int sigAlgo, int hashAlgo, RsaKey* key,
            const byte* keyBuf, word32 keySz, void* ctx)
{
    int ret;

    (void)ssl;
    (void)keyBuf;
    (void)keySz;
    (void)ctx;
    (void)sigAlgo;
    (void)hashAlgo;

    WOLFSSL_ENTER("RsaSign");

#ifdef WOLFSSL_ASYNC_CRYPT
    /* intialize event */
    ret = wolfSSL_AsyncInit(ssl, &key->asyncDev, WC_ASYNC_FLAG_CALL_AGAIN);
    if (ret != 0)
        return ret;
#endif

#if defined(WC_RSA_PSS)
    if (sigAlgo == rsa_pss_sa_algo) {
        enum wc_HashType hashType = WC_HASH_TYPE_NONE;
        int mgf = 0;

        ret = ConvertHashPss(hashAlgo, &hashType, &mgf);
        if (ret != 0)
            return ret;

    #if defined(HAVE_PK_CALLBACKS)
        if (ssl->ctx->RsaPssSignCb) {
            ret = ssl->ctx->RsaPssSignCb(ssl, in, inSz, out, outSz,
                                         TypeHash(hashAlgo), mgf,
                                         keyBuf, keySz, ctx);
        }
        else
    #endif
        {
            ret = wc_RsaPSS_Sign(in, inSz, out, *outSz, hashType, mgf, key,
                                                                      ssl->rng);
        }
    }
    else
#endif
#if defined(HAVE_PK_CALLBACKS)
    if (ssl->ctx->RsaSignCb) {
        ret = ssl->ctx->RsaSignCb(ssl, in, inSz, out, outSz, keyBuf, keySz,
                                                                          ctx);
    }
    else
#endif /*HAVE_PK_CALLBACKS */
        ret = wc_RsaSSL_Sign(in, inSz, out, *outSz, key, ssl->rng);

    /* Handle async pending response */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_PENDING_E) {
        ret = wolfSSL_AsyncPush(ssl, &key->asyncDev);
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    /* For positive response return in outSz */
    if (ret > 0) {
        *outSz = ret;
        ret = 0;
    }

    WOLFSSL_LEAVE("RsaSign", ret);

    return ret;
}

int RsaVerify(WOLFSSL* ssl, byte* in, word32 inSz, byte** out, int sigAlgo,
              int hashAlgo, RsaKey* key, const byte* keyBuf, word32 keySz,
              void* ctx)
{
    int ret;

    (void)ssl;
    (void)keyBuf;
    (void)keySz;
    (void)ctx;
    (void)sigAlgo;
    (void)hashAlgo;

    WOLFSSL_ENTER("RsaVerify");

#ifdef WOLFSSL_ASYNC_CRYPT
    /* intialize event */
    ret = wolfSSL_AsyncInit(ssl, &key->asyncDev, WC_ASYNC_FLAG_CALL_AGAIN);
    if (ret != 0)
        return ret;
#endif

#if defined(WC_RSA_PSS)
    if (sigAlgo == rsa_pss_sa_algo) {
        enum wc_HashType hashType = WC_HASH_TYPE_NONE;
        int mgf = 0;

        ret = ConvertHashPss(hashAlgo, &hashType, &mgf);
        if (ret != 0)
            return ret;
#ifdef HAVE_PK_CALLBACKS
        if (ssl->ctx->RsaPssVerifyCb) {
            ret = ssl->ctx->RsaPssVerifyCb(ssl, in, inSz, out,
                                           TypeHash(hashAlgo), mgf,
                                           keyBuf, keySz, ctx);
        }
        else
#endif /*HAVE_PK_CALLBACKS */
            ret = wc_RsaPSS_VerifyInline(in, inSz, out, hashType, mgf, key);
    }
    else
#endif
#ifdef HAVE_PK_CALLBACKS
    if (ssl->ctx->RsaVerifyCb) {
        ret = ssl->ctx->RsaVerifyCb(ssl, in, inSz, out, keyBuf, keySz, ctx);
    }
    else
#endif /*HAVE_PK_CALLBACKS */
    {
        ret = wc_RsaSSL_VerifyInline(in, inSz, out, key);
    }

    /* Handle async pending response */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_PENDING_E) {
        ret = wolfSSL_AsyncPush(ssl, &key->asyncDev);
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    WOLFSSL_LEAVE("RsaVerify", ret);

    return ret;
}

/* Verify RSA signature, 0 on success */
int VerifyRsaSign(WOLFSSL* ssl, byte* verifySig, word32 sigSz,
    const byte* plain, word32 plainSz, int sigAlgo, int hashAlgo, RsaKey* key)
{
    byte* out = NULL;  /* inline result */
    int   ret;

    (void)ssl;
    (void)sigAlgo;
    (void)hashAlgo;

    WOLFSSL_ENTER("VerifyRsaSign");

    if (verifySig == NULL || plain == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    if (sigSz > ENCRYPT_LEN) {
        WOLFSSL_MSG("Signature buffer too big");
        return BUFFER_E;
    }

#ifdef WOLFSSL_ASYNC_CRYPT
    /* intialize event */
    ret = wolfSSL_AsyncInit(ssl, &key->asyncDev, WC_ASYNC_FLAG_CALL_AGAIN);
    if (ret != 0)
        return ret;
#endif

#if defined(WC_RSA_PSS)
    if (sigAlgo == rsa_pss_sa_algo) {
        enum wc_HashType hashType = WC_HASH_TYPE_NONE;
        int mgf = 0;

        ret = ConvertHashPss(hashAlgo, &hashType, &mgf);
        if (ret != 0)
            return ret;
        ret = wc_RsaPSS_VerifyInline(verifySig, sigSz, &out, hashType, mgf,
                                                                           key);
        if (ret > 0) {
            ret = wc_RsaPSS_CheckPadding(plain, plainSz, out, ret, hashType);
            if (ret != 0)
                ret = VERIFY_CERT_ERROR;
        }
    }
    else
#endif
    {
        ret = wc_RsaSSL_VerifyInline(verifySig, sigSz, &out, key);
        if (ret > 0) {
            if (ret != (int)plainSz || !out ||
                                            XMEMCMP(plain, out, plainSz) != 0) {
                WOLFSSL_MSG("RSA Signature verification failed");
                ret = RSA_SIGN_FAULT;
            } else {
                ret = 0;  /* RSA reset */
            }
        }
    }

    /* Handle async pending response */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_PENDING_E) {
        ret = wolfSSL_AsyncPush(ssl, &key->asyncDev);
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    WOLFSSL_LEAVE("VerifyRsaSign", ret);

    return ret;
}

int RsaDec(WOLFSSL* ssl, byte* in, word32 inSz, byte** out, word32* outSz,
    RsaKey* key, const byte* keyBuf, word32 keySz, void* ctx)
{
    int ret;

    (void)ssl;
    (void)keyBuf;
    (void)keySz;
    (void)ctx;

    WOLFSSL_ENTER("RsaDec");

#ifdef WOLFSSL_ASYNC_CRYPT
    /* intialize event */
    ret = wolfSSL_AsyncInit(ssl, &key->asyncDev, WC_ASYNC_FLAG_CALL_AGAIN);
    if (ret != 0)
        return ret;
#endif

#ifdef HAVE_PK_CALLBACKS
    if (ssl->ctx->RsaDecCb) {
            ret = ssl->ctx->RsaDecCb(ssl, in, inSz, out, keyBuf, keySz,
                                                                    ctx);
    }
    else
#endif /* HAVE_PK_CALLBACKS */
    {
        #ifdef WC_RSA_BLINDING
            ret = wc_RsaSetRNG(key, ssl->rng);
            if (ret != 0)
                return ret;
        #endif
        ret = wc_RsaPrivateDecryptInline(in, inSz, out, key);
    }

    /* Handle async pending response */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_PENDING_E) {
        ret = wolfSSL_AsyncPush(ssl, &key->asyncDev);
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    /* For positive response return in outSz */
    if (ret > 0) {
        *outSz = ret;
        ret = 0;
    }

    WOLFSSL_LEAVE("RsaDec", ret);

    return ret;
}

int RsaEnc(WOLFSSL* ssl, const byte* in, word32 inSz, byte* out, word32* outSz,
    RsaKey* key, const byte* keyBuf, word32 keySz, void* ctx)
{
    int ret;

    (void)ssl;
    (void)keyBuf;
    (void)keySz;
    (void)ctx;

    WOLFSSL_ENTER("RsaEnc");

#ifdef WOLFSSL_ASYNC_CRYPT
    /* intialize event */
    ret = wolfSSL_AsyncInit(ssl, &key->asyncDev, WC_ASYNC_FLAG_CALL_AGAIN);
    if (ret != 0)
        return ret;
#endif

#ifdef HAVE_PK_CALLBACKS
    if (ssl->ctx->RsaEncCb) {
            ret = ssl->ctx->RsaEncCb(ssl, in, inSz, out, outSz, keyBuf, keySz,
                                                                        ctx);
    }
    else
#endif /* HAVE_PK_CALLBACKS */
    {
        ret = wc_RsaPublicEncrypt(in, inSz, out, *outSz, key, ssl->rng);
    }

    /* Handle async pending response */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_PENDING_E) {
        ret = wolfSSL_AsyncPush(ssl, &key->asyncDev);
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    /* For positive response return in outSz */
    if (ret > 0) {
        *outSz = ret;
        ret = 0;
    }

    WOLFSSL_LEAVE("RsaEnc", ret);

    return ret;
}

#endif /* NO_RSA */

#ifdef HAVE_ECC

int EccSign(WOLFSSL* ssl, const byte* in, word32 inSz, byte* out,
    word32* outSz, ecc_key* key, byte* keyBuf, word32 keySz, void* ctx)
{
    int ret;

    (void)ssl;
    (void)keyBuf;
    (void)keySz;
    (void)ctx;

    WOLFSSL_ENTER("EccSign");

#ifdef WOLFSSL_ASYNC_CRYPT
    /* intialize event */
    ret = wolfSSL_AsyncInit(ssl, &key->asyncDev, WC_ASYNC_FLAG_CALL_AGAIN);
    if (ret != 0)
        return ret;
#endif

#if defined(HAVE_PK_CALLBACKS)
    if (ssl->ctx->EccSignCb) {
        ret = ssl->ctx->EccSignCb(ssl, in, inSz, out, outSz, keyBuf,
            keySz, ctx);
    }
    else
#endif /* HAVE_PK_CALLBACKS */
    {
        ret = wc_ecc_sign_hash(in, inSz, out, outSz, ssl->rng, key);
    }

    /* Handle async pending response */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_PENDING_E) {
        ret = wolfSSL_AsyncPush(ssl, &key->asyncDev);
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    WOLFSSL_LEAVE("EccSign", ret);

    return ret;
}

int EccVerify(WOLFSSL* ssl, const byte* in, word32 inSz, const byte* out,
    word32 outSz, ecc_key* key, byte* keyBuf, word32 keySz,
    void* ctx)
{
    int ret;

    (void)ssl;
    (void)keyBuf;
    (void)keySz;
    (void)ctx;

    WOLFSSL_ENTER("EccVerify");

#ifdef WOLFSSL_ASYNC_CRYPT
    /* intialize event */
    ret = wolfSSL_AsyncInit(ssl, &key->asyncDev, WC_ASYNC_FLAG_CALL_AGAIN);
    if (ret != 0)
        return ret;
#endif

#ifdef HAVE_PK_CALLBACKS
    if (ssl->ctx->EccVerifyCb) {
        ret = ssl->ctx->EccVerifyCb(ssl, in, inSz, out, outSz, keyBuf, keySz,
            &ssl->eccVerifyRes, ctx);
    }
    else
#endif /* HAVE_PK_CALLBACKS  */
    {
        ret = wc_ecc_verify_hash(in, inSz, out, outSz, &ssl->eccVerifyRes, key);
    }

    /* Handle async pending response */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_PENDING_E) {
        ret = wolfSSL_AsyncPush(ssl, &key->asyncDev);
    }
    else
#endif /* WOLFSSL_ASYNC_CRYPT */
    {
        ret = (ret != 0 || ssl->eccVerifyRes == 0) ? VERIFY_SIGN_ERROR : 0;
    }

    WOLFSSL_LEAVE("EccVerify", ret);

    return ret;
}

#ifdef HAVE_PK_CALLBACKS
    /* Gets ECC key for shared secret callback testing
     * Client side: returns peer key
     * Server side: returns private key
     */
    static int EccGetKey(WOLFSSL* ssl, ecc_key** otherKey)
    {
        int ret = NO_PEER_KEY;
        ecc_key* tmpKey = NULL;

        if (ssl == NULL || otherKey == NULL) {
            return BAD_FUNC_ARG;
        }

        if (ssl->options.side == WOLFSSL_CLIENT_END) {
            if (ssl->specs.static_ecdh) {
                if (!ssl->peerEccDsaKey || !ssl->peerEccDsaKeyPresent ||
                                           !ssl->peerEccDsaKey->dp) {
                    return NO_PEER_KEY;
                }
                tmpKey = (struct ecc_key*)ssl->peerEccDsaKey;
            }
            else {
                if (!ssl->peerEccKey || !ssl->peerEccKeyPresent ||
                                        !ssl->peerEccKey->dp) {
                    return NO_PEER_KEY;
                }
                tmpKey = (struct ecc_key*)ssl->peerEccKey;
            }
        }
        else if (ssl->options.side == WOLFSSL_SERVER_END) {
            if (ssl->specs.static_ecdh) {
                if (ssl->hsKey == NULL) {
                    return NO_PRIVATE_KEY;
                }
                tmpKey = (struct ecc_key*)ssl->hsKey;
            }
            else {
                if (!ssl->eccTempKeyPresent) {
                    return NO_PRIVATE_KEY;
                }
                tmpKey = (struct ecc_key*)ssl->eccTempKey;
            }
        }

        if (tmpKey) {
            *otherKey = tmpKey;
            ret = 0;
        }

        return ret;
    }
#endif /* HAVE_PK_CALLBACKS */

int EccSharedSecret(WOLFSSL* ssl, ecc_key* priv_key, ecc_key* pub_key,
        byte* pubKeyDer, word32* pubKeySz, byte* out, word32* outlen,
        int side, void* ctx)
{
    int ret;
#ifdef HAVE_PK_CALLBACKS
    ecc_key* otherKey = NULL;
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV* asyncDev = &priv_key->asyncDev;
#endif

    (void)ssl;
    (void)pubKeyDer;
    (void)pubKeySz;
    (void)side;
    (void)ctx;

    WOLFSSL_ENTER("EccSharedSecret");

#ifdef HAVE_PK_CALLBACKS
    if (ssl->ctx->EccSharedSecretCb) {
        ret = EccGetKey(ssl, &otherKey);
        if (ret != 0)
            return ret;
    #ifdef WOLFSSL_ASYNC_CRYPT
        asyncDev = &otherKey->asyncDev;
    #endif
    }
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    /* intialize event */
    ret = wolfSSL_AsyncInit(ssl, asyncDev, WC_ASYNC_FLAG_CALL_AGAIN);
    if (ret != 0)
        return ret;
#endif

#ifdef HAVE_PK_CALLBACKS
    if (ssl->ctx->EccSharedSecretCb) {
        ret = ssl->ctx->EccSharedSecretCb(ssl, otherKey, pubKeyDer,
            pubKeySz, out, outlen, side, ctx);
    }
    else
#endif
    {
        ret = wc_ecc_shared_secret(priv_key, pub_key, out, outlen);
    }

    /* Handle async pending response */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_PENDING_E) {
        ret = wolfSSL_AsyncPush(ssl, asyncDev);
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    WOLFSSL_LEAVE("EccSharedSecret", ret);

    return ret;
}

int EccMakeKey(WOLFSSL* ssl, ecc_key* key, ecc_key* peer)
{
    int ret = 0;
    int keySz = 0;

    WOLFSSL_ENTER("EccMakeKey");

#ifdef WOLFSSL_ASYNC_CRYPT
    /* intialize event */
    ret = wolfSSL_AsyncInit(ssl, &key->asyncDev, WC_ASYNC_FLAG_NONE);
    if (ret != 0)
        return ret;
#endif

    if (peer == NULL) {
        keySz = ssl->eccTempKeySz;
    }
    else {
        keySz = peer->dp->size;
    }

    if (ssl->ecdhCurveOID > 0) {
        ret = wc_ecc_make_key_ex(ssl->rng, keySz, key,
                 wc_ecc_get_oid(ssl->ecdhCurveOID, NULL, NULL));
    }
    else {
        ret = wc_ecc_make_key(ssl->rng, keySz, key);
        if (ret == 0)
            ssl->ecdhCurveOID = key->dp->oidSum;
    }

    /* Handle async pending response */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_PENDING_E) {
        ret = wolfSSL_AsyncPush(ssl, &key->asyncDev);
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    WOLFSSL_LEAVE("EccMakeKey", ret);

    return ret;
}
#endif /* HAVE_ECC */

#ifdef HAVE_ED25519
/* Sign the data using EdDSA and key using X25519.
 *
 * ssl    SSL object.
 * in     Data or message to sign.
 * inSz   Length of the data.
 * out    Buffer to hold signature.
 * outSz  On entry, size of the buffer. On exit, the size of the signature.
 * key    The private X25519 key data.
 * keySz  The length of the private key data in bytes.
 * ctx    The callback context.
 * returns 0 on succes, otherwise the valus is an error.
 */
int Ed25519Sign(WOLFSSL* ssl, const byte* in, word32 inSz, byte* out,
                word32* outSz, ed25519_key* key, byte* keyBuf, word32 keySz,
                void* ctx)
{
    int ret;

    (void)ssl;
    (void)keyBuf;
    (void)keySz;
    (void)ctx;

    WOLFSSL_ENTER("Ed25519Sign");

#ifdef WOLFSSL_ASYNC_CRYPT
    /* intialize event */
    ret = wolfSSL_AsyncInit(ssl, &key->asyncDev, WC_ASYNC_FLAG_CALL_AGAIN);
    if (ret != 0)
        return ret;
#endif

#if defined(HAVE_PK_CALLBACKS)
    if (ssl->ctx->Ed25519SignCb) {
        ret = ssl->ctx->Ed25519SignCb(ssl, in, inSz, out, outSz, keyBuf,
            keySz, ctx);
    }
    else
#endif /* HAVE_PK_CALLBACKS */
    {
        ret = wc_ed25519_sign_msg(in, inSz, out, outSz, key);
    }

    /* Handle async pending response */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_PENDING_E) {
        ret = wolfSSL_AsyncPush(ssl, &key->asyncDev);
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    WOLFSSL_LEAVE("Ed25519Sign", ret);

    return ret;
}

/* Verify the data using EdDSA and key using X25519.
 *
 * ssl    SSL object.
 * in     Signature data.
 * inSz   Length of the signature data in bytes.
 * msg    Message to verify.
 * outSz  Length of message in bytes.
 * key    The public X25519 key data.
 * keySz  The length of the private key data in bytes.
 * ctx    The callback context.
 * returns 0 on succes, otherwise the valus is an error.
 */
int Ed25519Verify(WOLFSSL* ssl, const byte* in, word32 inSz, const byte* msg,
                  word32 msgSz, ed25519_key* key, byte* keyBuf, word32 keySz,
                  void* ctx)
{
    int ret;

    (void)ssl;
    (void)keyBuf;
    (void)keySz;
    (void)ctx;

    WOLFSSL_ENTER("Ed25519Verify");

#ifdef WOLFSSL_ASYNC_CRYPT
    /* intialize event */
    ret = wolfSSL_AsyncInit(ssl, &key->asyncDev, WC_ASYNC_FLAG_CALL_AGAIN);
    if (ret != 0)
        return ret;
#endif

#ifdef HAVE_PK_CALLBACKS
    if (ssl->ctx->Ed25519VerifyCb) {
        ret = ssl->ctx->Ed25519VerifyCb(ssl, in, inSz, msg, msgSz, keyBuf,
                                        keySz, &ssl->eccVerifyRes, ctx);
    }
    else
#endif /* HAVE_PK_CALLBACKS  */
    {
        ret = wc_ed25519_verify_msg(in, inSz, msg, msgSz,
                                    &ssl->eccVerifyRes, key);
    }

    /* Handle async pending response */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_PENDING_E) {
        ret = wolfSSL_AsyncPush(ssl, &key->asyncDev);
    }
    else
#endif /* WOLFSSL_ASYNC_CRYPT */
    {
        ret = (ret != 0 || ssl->eccVerifyRes == 0) ? VERIFY_SIGN_ERROR : 0;
    }

    WOLFSSL_LEAVE("Ed25519Verify", ret);

    return ret;
}
#endif /* HAVE_ED25519 */

#ifdef HAVE_CURVE25519
#ifdef HAVE_PK_CALLBACKS
    /* Gets X25519 key for shared secret callback testing
     * Client side: returns peer key
     * Server side: returns private key
     */
    static int X25519GetKey(WOLFSSL* ssl, curve25519_key** otherKey)
    {
        int ret = NO_PEER_KEY;
        struct curve25519_key* tmpKey = NULL;

        if (ssl == NULL || otherKey == NULL) {
            return BAD_FUNC_ARG;
        }

        if (ssl->options.side == WOLFSSL_CLIENT_END) {
            if (!ssl->peerX25519Key || !ssl->peerX25519KeyPresent ||
                                       !ssl->peerX25519Key->dp) {
                return NO_PEER_KEY;
            }
            tmpKey = (struct curve25519_key*)ssl->peerX25519Key;
        }
        else if (ssl->options.side == WOLFSSL_SERVER_END) {
            if (!ssl->eccTempKeyPresent) {
                return NO_PRIVATE_KEY;
            }
            tmpKey = (struct curve25519_key*)ssl->eccTempKey;
        }

        if (tmpKey) {
            *otherKey = (curve25519_key *)tmpKey;
            ret = 0;
        }

        return ret;
    }
#endif /* HAVE_PK_CALLBACKS */

static int X25519SharedSecret(WOLFSSL* ssl, curve25519_key* priv_key,
        curve25519_key* pub_key, byte* pubKeyDer, word32* pubKeySz,
        byte* out, word32* outlen, int side, void* ctx)
{
    int ret;

    (void)ssl;
    (void)pubKeyDer;
    (void)pubKeySz;
    (void)side;
    (void)ctx;

    WOLFSSL_ENTER("X25519SharedSecret");

#ifdef WOLFSSL_ASYNC_CRYPT
    /* intialize event */
    ret = wolfSSL_AsyncInit(ssl, &priv_key->asyncDev, WC_ASYNC_FLAG_CALL_AGAIN);
    if (ret != 0)
        return ret;
#endif

#ifdef HAVE_PK_CALLBACKS
    if (ssl->ctx->X25519SharedSecretCb) {
        curve25519_key* otherKey = NULL;

        ret = X25519GetKey(ssl, &otherKey);
        if (ret == 0) {
            ret = ssl->ctx->X25519SharedSecretCb(ssl, otherKey, pubKeyDer,
                pubKeySz, out, outlen, side, ctx);
        }
    }
    else
#endif
    {
        ret = wc_curve25519_shared_secret_ex(priv_key, pub_key, out, outlen,
                                             EC25519_LITTLE_ENDIAN);
    }

    /* Handle async pending response */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_PENDING_E) {
        ret = wolfSSL_AsyncPush(ssl, &priv_key->asyncDev);
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    WOLFSSL_LEAVE("X25519SharedSecret", ret);

    return ret;
}

static int X25519MakeKey(WOLFSSL* ssl, curve25519_key* key,
        curve25519_key* peer)
{
    int ret = 0;

    (void)peer;

    WOLFSSL_ENTER("X25519MakeKey");

#ifdef WOLFSSL_ASYNC_CRYPT
    /* intialize event */
    ret = wolfSSL_AsyncInit(ssl, &key->asyncDev, WC_ASYNC_FLAG_NONE);
    if (ret != 0)
        return ret;
#endif

    ret = wc_curve25519_make_key(ssl->rng, CURVE25519_KEYSIZE, key);
    if (ret == 0)
        ssl->ecdhCurveOID = ECC_X25519_OID;

    /* Handle async pending response */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_PENDING_E) {
        ret = wolfSSL_AsyncPush(ssl, &key->asyncDev);
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    WOLFSSL_LEAVE("X25519MakeKey", ret);

    return ret;
}
#endif /* HAVE_CURVE25519 */

#endif /* !NO_CERTS */

#if !defined(NO_CERTS) || !defined(NO_PSK)
#if !defined(NO_DH)

int DhGenKeyPair(WOLFSSL* ssl, DhKey* dhKey,
    byte* priv, word32* privSz,
    byte* pub, word32* pubSz)
{
    int ret;

    WOLFSSL_ENTER("DhGenKeyPair");

#ifdef WOLFSSL_ASYNC_CRYPT
    /* intialize event */
    ret = wolfSSL_AsyncInit(ssl, &dhKey->asyncDev, WC_ASYNC_FLAG_NONE);
    if (ret != 0)
        return ret;
#endif

    ret = wc_DhGenerateKeyPair(dhKey, ssl->rng, priv, privSz, pub, pubSz);

    /* Handle async pending response */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_PENDING_E) {
        ret = wolfSSL_AsyncPush(ssl, &dhKey->asyncDev);
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    WOLFSSL_LEAVE("DhGenKeyPair", ret);

    return ret;
}

int DhAgree(WOLFSSL* ssl, DhKey* dhKey,
    const byte* priv, word32 privSz,
    const byte* otherPub, word32 otherPubSz,
    byte* agree, word32* agreeSz)
{
    int ret;

    (void)ssl;

    WOLFSSL_ENTER("DhAgree");

#ifdef WOLFSSL_ASYNC_CRYPT
    /* intialize event */
    ret = wolfSSL_AsyncInit(ssl, &dhKey->asyncDev, WC_ASYNC_FLAG_NONE);
    if (ret != 0)
        return ret;
#endif

    ret = wc_DhAgree(dhKey, agree, agreeSz, priv, privSz, otherPub, otherPubSz);

    /* Handle async pending response */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_PENDING_E) {
        ret = wolfSSL_AsyncPush(ssl, &dhKey->asyncDev);
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    WOLFSSL_LEAVE("DhAgree", ret);

    return ret;
}
#endif /* !NO_DH */
#endif /* !NO_CERTS || !NO_PSK */


/* This function inherits a WOLFSSL_CTX's fields into an SSL object.
   It is used during initialization and to switch an ssl's CTX with
   wolfSSL_Set_SSL_CTX.  Requires ssl->suites alloc and ssl-arrays with PSK
   unless writeDup is on.

   ssl      object to initialize
   ctx      parent factory
   writeDup flag indicating this is a write dup only

   SSL_SUCCESS return value on success */
int SetSSL_CTX(WOLFSSL* ssl, WOLFSSL_CTX* ctx, int writeDup)
{
    byte havePSK = 0;
    byte haveAnon = 0;
    byte newSSL;
    byte haveRSA = 0;
    byte haveMcast = 0;

    (void)haveAnon; /* Squash unused var warnings */
    (void)haveMcast;

    if (!ssl || !ctx)
        return BAD_FUNC_ARG;

    if (ssl->suites == NULL && !writeDup)
        return BAD_FUNC_ARG;

    newSSL = ssl->ctx == NULL; /* Assign after null check */

#ifndef NO_PSK
    if (ctx->server_hint[0] && ssl->arrays == NULL && !writeDup) {
        return BAD_FUNC_ARG;  /* needed for copy below */
    }
#endif


#ifndef NO_RSA
    haveRSA = 1;
#endif
#ifndef NO_PSK
    havePSK = ctx->havePSK;
#endif /* NO_PSK */
#ifdef HAVE_ANON
    haveAnon = ctx->haveAnon;
#endif /* HAVE_ANON*/
#ifdef WOLFSSL_MULTICAST
    haveMcast = ctx->haveMcast;
#endif /* WOLFSSL_MULTICAST */

    /* decrement previous CTX reference count if exists.
     * This should only happen if switching ctxs!*/
    if (!newSSL) {
        WOLFSSL_MSG("freeing old ctx to decrement reference count. Switching ctx.");
        wolfSSL_CTX_free(ssl->ctx);
    }

    /* increment CTX reference count */
    if (wc_LockMutex(&ctx->countMutex) != 0) {
        WOLFSSL_MSG("Couldn't lock CTX count mutex");
        return BAD_MUTEX_E;
    }
    ctx->refCount++;
    wc_UnLockMutex(&ctx->countMutex);
    ssl->ctx     = ctx; /* only for passing to calls, options could change */
    ssl->version = ctx->method->version;

#ifdef HAVE_ECC
    ssl->eccTempKeySz = ctx->eccTempKeySz;
    ssl->pkCurveOID = ctx->pkCurveOID;
    ssl->ecdhCurveOID = ctx->ecdhCurveOID;
#endif

#ifdef OPENSSL_EXTRA
    ssl->options.mask = ctx->mask;
#endif
    ssl->timeout = ctx->timeout;
    ssl->verifyCallback    = ctx->verifyCallback;
    ssl->options.side      = ctx->method->side;
    ssl->options.downgrade    = ctx->method->downgrade;
    ssl->options.minDowngrade = ctx->minDowngrade;

    ssl->options.haveDH        = ctx->haveDH;
    ssl->options.haveNTRU      = ctx->haveNTRU;
    ssl->options.haveECDSAsig  = ctx->haveECDSAsig;
    ssl->options.haveECC       = ctx->haveECC;
    ssl->options.haveStaticECC = ctx->haveStaticECC;

#ifndef NO_PSK
    ssl->options.havePSK   = ctx->havePSK;
    ssl->options.client_psk_cb = ctx->client_psk_cb;
    ssl->options.server_psk_cb = ctx->server_psk_cb;
#endif /* NO_PSK */
#ifdef WOLFSSL_EARLY_DATA
    if (ssl->options.side == WOLFSSL_SERVER_END)
        ssl->options.maxEarlyDataSz = ctx->maxEarlyDataSz;
#endif

#ifdef HAVE_ANON
    ssl->options.haveAnon = ctx->haveAnon;
#endif
#ifndef NO_DH
    ssl->options.minDhKeySz = ctx->minDhKeySz;
#endif
#ifndef NO_RSA
    ssl->options.minRsaKeySz = ctx->minRsaKeySz;
#endif
#ifdef HAVE_ECC
    ssl->options.minEccKeySz = ctx->minEccKeySz;
#endif
#ifdef OPENSSL_EXTRA
    ssl->options.verifyDepth = ctx->verifyDepth;
#endif

    ssl->options.sessionCacheOff      = ctx->sessionCacheOff;
    ssl->options.sessionCacheFlushOff = ctx->sessionCacheFlushOff;
#ifdef HAVE_EXT_CACHE
    ssl->options.internalCacheOff     = ctx->internalCacheOff;
#endif

    ssl->options.verifyPeer     = ctx->verifyPeer;
    ssl->options.verifyNone     = ctx->verifyNone;
    ssl->options.failNoCert     = ctx->failNoCert;
    ssl->options.failNoCertxPSK = ctx->failNoCertxPSK;
    ssl->options.sendVerify     = ctx->sendVerify;

    ssl->options.partialWrite  = ctx->partialWrite;
    ssl->options.quietShutdown = ctx->quietShutdown;
    ssl->options.groupMessages = ctx->groupMessages;

#ifndef NO_DH
    ssl->buffers.serverDH_P = ctx->serverDH_P;
    ssl->buffers.serverDH_G = ctx->serverDH_G;
#endif

#ifndef NO_CERTS
    /* ctx still owns certificate, certChain, key, dh, and cm */
    ssl->buffers.certificate = ctx->certificate;
    ssl->buffers.certChain = ctx->certChain;
#ifdef WOLFSSL_TLS13
    ssl->buffers.certChainCnt = ctx->certChainCnt;
#endif
    ssl->buffers.key     = ctx->privateKey;
    ssl->buffers.keyType = ctx->privateKeyType;
    ssl->buffers.keySz   = ctx->privateKeySz;
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    ssl->devId = ctx->devId;
#endif

    if (writeDup == 0) {
        int keySz = 0;
#ifndef NO_CERTS
        keySz = ssl->buffers.keySz;
#endif

#ifndef NO_PSK
        if (ctx->server_hint[0]) {   /* set in CTX */
            XSTRNCPY(ssl->arrays->server_hint, ctx->server_hint,
                                    sizeof(ssl->arrays->server_hint));
            ssl->arrays->server_hint[MAX_PSK_ID_LEN] = '\0'; /* null term */
        }
#endif /* NO_PSK */

        if (ctx->suites)
            *ssl->suites = *ctx->suites;
        else
            XMEMSET(ssl->suites, 0, sizeof(Suites));

        /* make sure server has DH parms, and add PSK if there, add NTRU too */
        if (ssl->options.side == WOLFSSL_SERVER_END)
            InitSuites(ssl->suites, ssl->version, keySz, haveRSA, havePSK,
                       ssl->options.haveDH, ssl->options.haveNTRU,
                       ssl->options.haveECDSAsig, ssl->options.haveECC,
                       ssl->options.haveStaticECC, ssl->options.side);
        else
            InitSuites(ssl->suites, ssl->version, keySz, haveRSA, havePSK,
                       TRUE, ssl->options.haveNTRU,
                       ssl->options.haveECDSAsig, ssl->options.haveECC,
                       ssl->options.haveStaticECC, ssl->options.side);

#if !defined(NO_CERTS) && !defined(WOLFSSL_SESSION_EXPORT)
        /* make sure server has cert and key unless using PSK, Anon, or
         * Multicast. This should be true even if just switching ssl ctx */
        if (ssl->options.side == WOLFSSL_SERVER_END &&
                !havePSK && !haveAnon && !haveMcast)
            if (!ssl->buffers.certificate || !ssl->buffers.certificate->buffer
                || !ssl->buffers.key || !ssl->buffers.key->buffer) {
                WOLFSSL_MSG("Server missing certificate and/or private key");
                return NO_PRIVATE_KEY;
            }
#endif

    }  /* writeDup check */

#ifdef WOLFSSL_SESSION_EXPORT
    #ifdef WOLFSSL_DTLS
    ssl->dtls_export = ctx->dtls_export; /* export function for session */
    #endif
#endif

#ifdef OPENSSL_EXTRA
    ssl->readAhead = ctx->readAhead;
#endif

    return SSL_SUCCESS;
}

int InitHandshakeHashes(WOLFSSL* ssl)
{
    int ret;

    /* make sure existing handshake hashes are free'd */
    if (ssl->hsHashes != NULL) {
        FreeHandshakeHashes(ssl);
    }

    /* allocate handshake hashes */
    ssl->hsHashes = (HS_Hashes*)XMALLOC(sizeof(HS_Hashes), ssl->heap,
                                                           DYNAMIC_TYPE_HASHES);
    if (ssl->hsHashes == NULL) {
        WOLFSSL_MSG("HS_Hashes Memory error");
        return MEMORY_E;
    }
    XMEMSET(ssl->hsHashes, 0, sizeof(HS_Hashes));

#ifndef NO_OLD_TLS
#ifndef NO_MD5
    ret = wc_InitMd5_ex(&ssl->hsHashes->hashMd5, ssl->heap, ssl->devId);
    if (ret != 0)
        return ret;
#endif
#ifndef NO_SHA
    ret = wc_InitSha_ex(&ssl->hsHashes->hashSha, ssl->heap, ssl->devId);
    if (ret != 0)
        return ret;
#endif
#endif /* !NO_OLD_TLS */
#ifndef NO_SHA256
    ret = wc_InitSha256_ex(&ssl->hsHashes->hashSha256, ssl->heap, ssl->devId);
    if (ret != 0)
        return ret;
#endif
#ifdef WOLFSSL_SHA384
    ret = wc_InitSha384_ex(&ssl->hsHashes->hashSha384, ssl->heap, ssl->devId);
    if (ret != 0)
        return ret;
#endif
#ifdef WOLFSSL_SHA512
    ret = wc_InitSha512_ex(&ssl->hsHashes->hashSha512, ssl->heap, ssl->devId);
    if (ret != 0)
        return ret;
#endif

    return ret;
}

void FreeHandshakeHashes(WOLFSSL* ssl)
{
    if (ssl->hsHashes) {
#ifndef NO_OLD_TLS
    #ifndef NO_MD5
        wc_Md5Free(&ssl->hsHashes->hashMd5);
    #endif
    #ifndef NO_SHA
        wc_ShaFree(&ssl->hsHashes->hashSha);
    #endif
#endif /* !NO_OLD_TLS */
    #ifndef NO_SHA256
        wc_Sha256Free(&ssl->hsHashes->hashSha256);
    #endif
    #ifdef WOLFSSL_SHA384
        wc_Sha384Free(&ssl->hsHashes->hashSha384);
    #endif
    #ifdef WOLFSSL_SHA512
        wc_Sha512Free(&ssl->hsHashes->hashSha512);
    #endif

        XFREE(ssl->hsHashes, ssl->heap, DYNAMIC_TYPE_HASHES);
        ssl->hsHashes = NULL;
    }
}


/* init everything to 0, NULL, default values before calling anything that may
   fail so that destructor has a "good" state to cleanup

   ssl      object to initialize
   ctx      parent factory
   writeDup flag indicating this is a write dup only

   0 on success */
int InitSSL(WOLFSSL* ssl, WOLFSSL_CTX* ctx, int writeDup)
{
    int  ret;

    XMEMSET(ssl, 0, sizeof(WOLFSSL));

#if defined(WOLFSSL_STATIC_MEMORY)
    if (ctx->heap != NULL) {
        WOLFSSL_HEAP_HINT* ssl_hint;
        WOLFSSL_HEAP_HINT* ctx_hint;

        /* avoid derefrencing a test value */
    #ifdef WOLFSSL_HEAP_TEST
        if (ctx->heap == (void*)WOLFSSL_HEAP_TEST) {
            ssl->heap = ctx->heap;
        }
        else {
    #endif
        ssl->heap = (WOLFSSL_HEAP_HINT*)XMALLOC(sizeof(WOLFSSL_HEAP_HINT),
                                               ctx->heap, DYNAMIC_TYPE_SSL);
        if (ssl->heap == NULL) {
            return MEMORY_E;
        }
        XMEMSET(ssl->heap, 0, sizeof(WOLFSSL_HEAP_HINT));
        ssl_hint = ((WOLFSSL_HEAP_HINT*)(ssl->heap));
        ctx_hint = ((WOLFSSL_HEAP_HINT*)(ctx->heap));

        /* lock and check IO count / handshake count */
        if (wc_LockMutex(&(ctx_hint->memory->memory_mutex)) != 0) {
            WOLFSSL_MSG("Bad memory_mutex lock");
            XFREE(ssl->heap, ctx->heap, DYNAMIC_TYPE_SSL);
            ssl->heap = NULL; /* free and set to NULL for IO counter */
            return BAD_MUTEX_E;
        }
        if (ctx_hint->memory->maxHa > 0 &&
                           ctx_hint->memory->maxHa <= ctx_hint->memory->curHa) {
            WOLFSSL_MSG("At max number of handshakes for static memory");
            wc_UnLockMutex(&(ctx_hint->memory->memory_mutex));
            XFREE(ssl->heap, ctx->heap, DYNAMIC_TYPE_SSL);
            ssl->heap = NULL; /* free and set to NULL for IO counter */
            return MEMORY_E;
        }

        if (ctx_hint->memory->maxIO > 0 &&
                           ctx_hint->memory->maxIO <= ctx_hint->memory->curIO) {
            WOLFSSL_MSG("At max number of IO allowed for static memory");
            wc_UnLockMutex(&(ctx_hint->memory->memory_mutex));
            XFREE(ssl->heap, ctx->heap, DYNAMIC_TYPE_SSL);
            ssl->heap = NULL; /* free and set to NULL for IO counter */
            return MEMORY_E;
        }
        ctx_hint->memory->curIO++;
        ctx_hint->memory->curHa++;
        ssl_hint->memory = ctx_hint->memory;
        ssl_hint->haFlag = 1;
        wc_UnLockMutex(&(ctx_hint->memory->memory_mutex));

        /* check if tracking stats */
        if (ctx_hint->memory->flag & WOLFMEM_TRACK_STATS) {
            ssl_hint->stats = (WOLFSSL_MEM_CONN_STATS*)XMALLOC(
               sizeof(WOLFSSL_MEM_CONN_STATS), ctx->heap, DYNAMIC_TYPE_SSL);
            if (ssl_hint->stats == NULL) {
                return MEMORY_E;
            }
            XMEMSET(ssl_hint->stats, 0, sizeof(WOLFSSL_MEM_CONN_STATS));
        }

        /* check if using fixed IO buffers */
        if (ctx_hint->memory->flag & WOLFMEM_IO_POOL_FIXED) {
            if (wc_LockMutex(&(ctx_hint->memory->memory_mutex)) != 0) {
                WOLFSSL_MSG("Bad memory_mutex lock");
                return BAD_MUTEX_E;
            }
            if (SetFixedIO(ctx_hint->memory, &(ssl_hint->inBuf)) != 1) {
                wc_UnLockMutex(&(ctx_hint->memory->memory_mutex));
                return MEMORY_E;
            }
            if (SetFixedIO(ctx_hint->memory, &(ssl_hint->outBuf)) != 1) {
                wc_UnLockMutex(&(ctx_hint->memory->memory_mutex));
                return MEMORY_E;
            }
            if (ssl_hint->outBuf == NULL || ssl_hint->inBuf == NULL) {
                WOLFSSL_MSG("Not enough memory to create fixed IO buffers");
                wc_UnLockMutex(&(ctx_hint->memory->memory_mutex));
                return MEMORY_E;
            }
            wc_UnLockMutex(&(ctx_hint->memory->memory_mutex));
        }
    #ifdef WOLFSSL_HEAP_TEST
        }
    #endif
    }
    else {
        ssl->heap = ctx->heap;
    }
#else
    ssl->heap = ctx->heap; /* carry over user heap without static memory */
#endif /* WOLFSSL_STATIC_MEMORY */

    ssl->buffers.inputBuffer.buffer = ssl->buffers.inputBuffer.staticBuffer;
    ssl->buffers.inputBuffer.bufferSize  = STATIC_BUFFER_LEN;

    ssl->buffers.outputBuffer.buffer = ssl->buffers.outputBuffer.staticBuffer;
    ssl->buffers.outputBuffer.bufferSize  = STATIC_BUFFER_LEN;

#if defined(KEEP_PEER_CERT) || defined(GOAHEAD_WS)
    InitX509(&ssl->peerCert, 0, ssl->heap);
#endif

    ssl->rfd = -1;   /* set to invalid descriptor */
    ssl->wfd = -1;
    ssl->devId = ctx->devId; /* device for async HW (from wolfAsync_DevOpen) */

    ssl->IOCB_ReadCtx  = &ssl->rfd;  /* prevent invalid pointer access if not */
    ssl->IOCB_WriteCtx = &ssl->wfd;  /* correctly set */

#ifdef HAVE_NETX
    ssl->IOCB_ReadCtx  = &ssl->nxCtx;  /* default NetX IO ctx, same for read */
    ssl->IOCB_WriteCtx = &ssl->nxCtx;  /* and write */
#endif

    /* initialize states */
    ssl->options.serverState = NULL_STATE;
    ssl->options.clientState = NULL_STATE;
    ssl->options.connectState = CONNECT_BEGIN;
    ssl->options.acceptState  = ACCEPT_BEGIN;
    ssl->options.handShakeState  = NULL_STATE;
    ssl->options.processReply = doProcessInit;
    ssl->options.asyncState = TLS_ASYNC_BEGIN;
    ssl->options.buildMsgState = BUILD_MSG_BEGIN;
    ssl->encrypt.state = CIPHER_STATE_BEGIN;
    ssl->decrypt.state = CIPHER_STATE_BEGIN;

#ifdef WOLFSSL_DTLS
    #ifdef WOLFSSL_SCTP
        ssl->options.dtlsSctp           = ctx->dtlsSctp;
        ssl->dtlsMtuSz                  = ctx->dtlsMtuSz;
        ssl->dtls_expected_rx           = ssl->dtlsMtuSz;
    #else
        ssl->dtls_expected_rx = MAX_MTU;
    #endif
    ssl->dtls_timeout_init              = DTLS_TIMEOUT_INIT;
    ssl->dtls_timeout_max               = DTLS_TIMEOUT_MAX;
    ssl->dtls_timeout                   = ssl->dtls_timeout_init;
    ssl->buffers.dtlsCtx.rfd            = -1;
    ssl->buffers.dtlsCtx.wfd            = -1;
#endif

    #ifndef NO_OLD_TLS
        ssl->hmac = SSL_hmac; /* default to SSLv3 */
    #else
        ssl->hmac = TLS_hmac;
    #endif


    ssl->cipher.ssl = ssl;

#ifdef HAVE_EXTENDED_MASTER
    ssl->options.haveEMS = ctx->haveEMS;
#endif
    ssl->options.useClientOrder = ctx->useClientOrder;

#ifdef WOLFSSL_TLS13
    #ifdef HAVE_SESSION_TICKET
        ssl->options.noTicketTls13 = ctx->noTicketTls13;
    #endif
    ssl->options.noPskDheKe = ctx->noPskDheKe;
    #if defined(WOLFSSL_POST_HANDSHAKE_AUTH)
        ssl->options.postHandshakeAuth = ctx->postHandshakeAuth;
    #endif
#endif

#ifdef HAVE_TLS_EXTENSIONS
#ifdef HAVE_MAX_FRAGMENT
    ssl->max_fragment = MAX_RECORD_SIZE;
#endif
#ifdef HAVE_ALPN
    ssl->alpn_client_list = NULL;
    #if defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
        ssl->alpnSelect    = ctx->alpnSelect;
        ssl->alpnSelectArg = ctx->alpnSelectArg;
    #endif
#endif
#ifdef HAVE_SUPPORTED_CURVES
    ssl->options.userCurves = ctx->userCurves;
#endif
#endif /* HAVE_TLS_EXTENSIONS */

    /* default alert state (none) */
    ssl->alert_history.last_rx.code  = -1;
    ssl->alert_history.last_rx.level = -1;
    ssl->alert_history.last_tx.code  = -1;
    ssl->alert_history.last_tx.level = -1;

    InitCiphers(ssl);
    InitCipherSpecs(&ssl->specs);

    /* all done with init, now can return errors, call other stuff */

    if (!writeDup) {
        /* arrays */
        ssl->arrays = (Arrays*)XMALLOC(sizeof(Arrays), ssl->heap,
                                                           DYNAMIC_TYPE_ARRAYS);
        if (ssl->arrays == NULL) {
            WOLFSSL_MSG("Arrays Memory error");
            return MEMORY_E;
        }
        XMEMSET(ssl->arrays, 0, sizeof(Arrays));
        ssl->arrays->preMasterSecret = (byte*)XMALLOC(ENCRYPT_LEN, ssl->heap,
            DYNAMIC_TYPE_SECRET);
        if (ssl->arrays->preMasterSecret == NULL) {
            return MEMORY_E;
        }
        XMEMSET(ssl->arrays->preMasterSecret, 0, ENCRYPT_LEN);

        /* suites */
        ssl->suites = (Suites*)XMALLOC(sizeof(Suites), ssl->heap,
                                   DYNAMIC_TYPE_SUITES);
        if (ssl->suites == NULL) {
            WOLFSSL_MSG("Suites Memory error");
            return MEMORY_E;
        }
    }

    /* Initialize SSL with the appropriate fields from it's ctx */
    /* requires valid arrays and suites unless writeDup ing */
    if ((ret =  SetSSL_CTX(ssl, ctx, writeDup)) != SSL_SUCCESS)
        return ret;

    ssl->options.dtls = ssl->version.major == DTLS_MAJOR;

#ifdef SINGLE_THREADED
    ssl->rng = ctx->rng;   /* CTX may have one, if so use it */
#endif

    if (ssl->rng == NULL) {
        /* RNG */
        ssl->rng = (WC_RNG*)XMALLOC(sizeof(WC_RNG), ssl->heap,DYNAMIC_TYPE_RNG);
        if (ssl->rng == NULL) {
            WOLFSSL_MSG("RNG Memory error");
            return MEMORY_E;
        }
        XMEMSET(ssl->rng, 0, sizeof(WC_RNG));
        ssl->options.weOwnRng = 1;

        /* FIPS RNG API does not accept a heap hint */
#ifndef HAVE_FIPS
        if ( (ret = wc_InitRng_ex(ssl->rng, ssl->heap, ssl->devId)) != 0) {
            WOLFSSL_MSG("RNG Init error");
            return ret;
        }
#else
        if ( (ret = wc_InitRng(ssl->rng)) != 0) {
            WOLFSSL_MSG("RNG Init error");
            return ret;
        }
#endif
    }

    if (writeDup) {
        /* all done */
        return 0;
    }

    /* hsHashes */
    ret = InitHandshakeHashes(ssl);
    if (ret != 0)
        return ret;

#if defined(WOLFSSL_DTLS) && !defined(NO_WOLFSSL_SERVER)
    if (ssl->options.dtls && ssl->options.side == WOLFSSL_SERVER_END) {
        ret = wolfSSL_DTLS_SetCookieSecret(ssl, NULL, 0);
        if (ret != 0) {
            WOLFSSL_MSG("DTLS Cookie Secret error");
            return ret;
        }
    }
#endif /* WOLFSSL_DTLS && !NO_WOLFSSL_SERVER */

#ifdef HAVE_SECRET_CALLBACK
    ssl->sessionSecretCb  = NULL;
    ssl->sessionSecretCtx = NULL;
#endif

#ifdef HAVE_SESSION_TICKET
    ssl->session.ticket = ssl->session.staticTicket;
#endif

#ifdef WOLFSSL_MULTICAST
    if (ctx->haveMcast) {
        int i;

        ssl->options.haveMcast = 1;
        ssl->options.mcastID = ctx->mcastID;

        /* Force the state to look like handshake has completed. */
        /* Keying material is supplied externally. */
        ssl->options.serverState = SERVER_FINISHED_COMPLETE;
        ssl->options.clientState = CLIENT_FINISHED_COMPLETE;
        ssl->options.connectState = SECOND_REPLY_DONE;
        ssl->options.acceptState = ACCEPT_THIRD_REPLY_DONE;
        ssl->options.handShakeState = HANDSHAKE_DONE;
        ssl->options.handShakeDone = 1;

        for (i = 0; i < WOLFSSL_DTLS_PEERSEQ_SZ; i++)
            ssl->keys.peerSeq[i].peerId = INVALID_PEER_ID;
    }
#endif

    return 0;
}


/* free use of temporary arrays */
void FreeArrays(WOLFSSL* ssl, int keep)
{
    if (ssl->arrays) {
        if (keep) {
            /* keeps session id for user retrieval */
            XMEMCPY(ssl->session.sessionID, ssl->arrays->sessionID, ID_LEN);
            ssl->session.sessionIDSz = ssl->arrays->sessionIDSz;
        }
        if (ssl->arrays->preMasterSecret) {
            XFREE(ssl->arrays->preMasterSecret, ssl->heap, DYNAMIC_TYPE_SECRET);
            ssl->arrays->preMasterSecret = NULL;
        }
        XFREE(ssl->arrays->pendingMsg, ssl->heap, DYNAMIC_TYPE_ARRAYS);
        ssl->arrays->pendingMsg = NULL;
        ForceZero(ssl->arrays, sizeof(Arrays)); /* clear arrays struct */
    }
    XFREE(ssl->arrays, ssl->heap, DYNAMIC_TYPE_ARRAYS);
    ssl->arrays = NULL;
}

void FreeKey(WOLFSSL* ssl, int type, void** pKey)
{
    if (ssl && pKey && *pKey) {
        switch (type) {
        #ifndef NO_RSA
            case DYNAMIC_TYPE_RSA:
                wc_FreeRsaKey((RsaKey*)*pKey);
                break;
        #endif /* ! NO_RSA */
        #ifdef HAVE_ECC
            case DYNAMIC_TYPE_ECC:
                wc_ecc_free((ecc_key*)*pKey);
                break;
        #endif /* HAVE_ECC */
        #ifdef HAVE_ED25519
            case DYNAMIC_TYPE_ED25519:
                wc_ed25519_free((ed25519_key*)*pKey);
                break;
        #endif /* HAVE_CURVE25519 */
        #ifdef HAVE_CURVE25519
            case DYNAMIC_TYPE_CURVE25519:
                wc_curve25519_free((curve25519_key*)*pKey);
                break;
        #endif /* HAVE_CURVE25519 */
        #ifndef NO_DH
            case DYNAMIC_TYPE_DH:
                wc_FreeDhKey((DhKey*)*pKey);
                break;
        #endif /* !NO_DH */
            default:
                break;
        }
        XFREE(*pKey, ssl->heap, type);

        /* Reset pointer */
        *pKey = NULL;
    }
}

int AllocKey(WOLFSSL* ssl, int type, void** pKey)
{
    int ret = BAD_FUNC_ARG;
    int sz = 0;

    if (ssl == NULL || pKey == NULL) {
        return BAD_FUNC_ARG;
    }

    /* Sanity check key destination */
    if (*pKey != NULL) {
        WOLFSSL_MSG("Key already present!");
        return BAD_STATE_E;
    }

    /* Determine size */
    switch (type) {
    #ifndef NO_RSA
        case DYNAMIC_TYPE_RSA:
            sz = sizeof(RsaKey);
            break;
    #endif /* ! NO_RSA */
    #ifdef HAVE_ECC
        case DYNAMIC_TYPE_ECC:
            sz = sizeof(ecc_key);
            break;
    #endif /* HAVE_ECC */
    #ifdef HAVE_ED25519
        case DYNAMIC_TYPE_ED25519:
            sz = sizeof(ed25519_key);
            break;
    #endif /* HAVE_ED25519 */
    #ifdef HAVE_CURVE25519
        case DYNAMIC_TYPE_CURVE25519:
            sz = sizeof(curve25519_key);
            break;
    #endif /* HAVE_CURVE25519 */
    #ifndef NO_DH
        case DYNAMIC_TYPE_DH:
            sz = sizeof(DhKey);
            break;
    #endif /* !NO_DH */
        default:
            return BAD_FUNC_ARG;
    }

    if (sz == 0) {
        return NOT_COMPILED_IN;
    }

    /* Allocate memeory for key */
    *pKey = XMALLOC(sz, ssl->heap, type);
    if (*pKey == NULL) {
        return MEMORY_E;
    }

    /* Initialize key */
    switch (type) {
    #ifndef NO_RSA
        case DYNAMIC_TYPE_RSA:
            ret = wc_InitRsaKey_ex((RsaKey*)*pKey, ssl->heap, ssl->devId);
            break;
    #endif /* ! NO_RSA */
    #ifdef HAVE_ECC
        case DYNAMIC_TYPE_ECC:
            ret = wc_ecc_init_ex((ecc_key*)*pKey, ssl->heap, ssl->devId);
            break;
    #endif /* HAVE_ECC */
    #ifdef HAVE_ED25519
        case DYNAMIC_TYPE_ED25519:
            wc_ed25519_init((ed25519_key*)*pKey);
            ret = 0;
            break;
    #endif /* HAVE_CURVE25519 */
    #ifdef HAVE_CURVE25519
        case DYNAMIC_TYPE_CURVE25519:
            wc_curve25519_init((curve25519_key*)*pKey);
            ret = 0;
            break;
    #endif /* HAVE_CURVE25519 */
    #ifndef NO_DH
        case DYNAMIC_TYPE_DH:
            ret = wc_InitDhKey_ex((DhKey*)*pKey, ssl->heap, ssl->devId);
            break;
    #endif /* !NO_DH */
        default:
            return BAD_FUNC_ARG;
    }

    /* On error free handshake key */
    if (ret != 0) {
        FreeKey(ssl, type, pKey);
    }

    return ret;
}

#if !defined(NO_RSA) || defined(HAVE_ECC)
static int ReuseKey(WOLFSSL* ssl, int type, void* pKey)
{
    int ret = 0;

    switch (type) {
    #ifndef NO_RSA
        case DYNAMIC_TYPE_RSA:
            wc_FreeRsaKey((RsaKey*)pKey);
            ret = wc_InitRsaKey_ex((RsaKey*)pKey, ssl->heap, ssl->devId);
            break;
    #endif /* ! NO_RSA */
    #ifdef HAVE_ECC
        case DYNAMIC_TYPE_ECC:
            wc_ecc_free((ecc_key*)pKey);
            ret = wc_ecc_init_ex((ecc_key*)pKey, ssl->heap, ssl->devId);
            break;
    #endif /* HAVE_ECC */
    #ifdef HAVE_ED25519
        case DYNAMIC_TYPE_ED25519:
            wc_ed25519_free((ed25519_key*)pKey);
            wc_ed25519_init((ed25519_key*)pKey);
            break;
    #endif /* HAVE_CURVE25519 */
    #ifdef HAVE_CURVE25519
        case DYNAMIC_TYPE_CURVE25519:
            wc_curve25519_free((curve25519_key*)pKey);
            wc_curve25519_init((curve25519_key*)pKey);
            break;
    #endif /* HAVE_CURVE25519 */
    #ifndef NO_DH
        case DYNAMIC_TYPE_DH:
            wc_FreeDhKey((DhKey*)pKey);
            ret = wc_InitDhKey_ex((DhKey*)pKey, ssl->heap, ssl->devId);
            break;
    #endif /* !NO_DH */
        default:
            return BAD_FUNC_ARG;
    }

    return ret;
}
#endif

void FreeKeyExchange(WOLFSSL* ssl)
{
    /* Cleanup signature buffer */
    if (ssl->buffers.sig.buffer) {
        XFREE(ssl->buffers.sig.buffer, ssl->heap, DYNAMIC_TYPE_SIGNATURE);
        ssl->buffers.sig.buffer = NULL;
        ssl->buffers.sig.length = 0;
    }

    /* Cleanup digest buffer */
    if (ssl->buffers.digest.buffer) {
        XFREE(ssl->buffers.digest.buffer, ssl->heap, DYNAMIC_TYPE_DIGEST);
        ssl->buffers.digest.buffer = NULL;
        ssl->buffers.digest.length = 0;
    }

    /* Free handshake key */
    FreeKey(ssl, ssl->hsType, &ssl->hsKey);

#ifndef NO_DH
    /* Free temp DH key */
    FreeKey(ssl, DYNAMIC_TYPE_DH, (void**)&ssl->buffers.serverDH_Key);
#endif

    /* Cleanup async */
#ifdef WOLFSSL_ASYNC_CRYPT
    if (ssl->async.freeArgs) {
        ssl->async.freeArgs(ssl, ssl->async.args);
        ssl->async.freeArgs = NULL;
    }
#endif
}

/* In case holding SSL object in array and don't want to free actual ssl */
void SSL_ResourceFree(WOLFSSL* ssl)
{
    /* Note: any resources used during the handshake should be released in the
     * function FreeHandshakeResources(). Be careful with the special cases
     * like the RNG which may optionally be kept for the whole session. (For
     * example with the RNG, it isn't used beyond the handshake except when
     * using stream ciphers where it is retained. */

    FreeCiphers(ssl);
    FreeArrays(ssl, 0);
    FreeKeyExchange(ssl);
    if (ssl->options.weOwnRng) {
        wc_FreeRng(ssl->rng);
        XFREE(ssl->rng, ssl->heap, DYNAMIC_TYPE_RNG);
    }
    XFREE(ssl->suites, ssl->heap, DYNAMIC_TYPE_SUITES);
    FreeHandshakeHashes(ssl);
    XFREE(ssl->buffers.domainName.buffer, ssl->heap, DYNAMIC_TYPE_DOMAIN);

    /* clear keys struct after session */
    ForceZero(&ssl->keys, sizeof(Keys));

#ifndef NO_DH
    if (ssl->buffers.serverDH_Priv.buffer) {
        ForceZero(ssl->buffers.serverDH_Priv.buffer,
                                             ssl->buffers.serverDH_Priv.length);
    }
    XFREE(ssl->buffers.serverDH_Priv.buffer, ssl->heap, DYNAMIC_TYPE_PRIVATE_KEY);
    XFREE(ssl->buffers.serverDH_Pub.buffer, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    /* parameters (p,g) may be owned by ctx */
    if (ssl->buffers.weOwnDH || ssl->options.side == WOLFSSL_CLIENT_END) {
        XFREE(ssl->buffers.serverDH_G.buffer, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
        XFREE(ssl->buffers.serverDH_P.buffer, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    }
#endif /* !NO_DH */
#ifndef NO_CERTS
    ssl->keepCert = 0; /* make sure certificate is free'd */
    wolfSSL_UnloadCertsKeys(ssl);
#endif
#ifndef NO_RSA
    FreeKey(ssl, DYNAMIC_TYPE_RSA, (void**)&ssl->peerRsaKey);
    ssl->peerRsaKeyPresent = 0;
#endif
    if (ssl->buffers.inputBuffer.dynamicFlag)
        ShrinkInputBuffer(ssl, FORCED_FREE);
    if (ssl->buffers.outputBuffer.dynamicFlag)
        ShrinkOutputBuffer(ssl);
#if defined(WOLFSSL_SEND_HRR_COOKIE) && !defined(NO_WOLFSSL_SERVER)
    XFREE(ssl->buffers.tls13CookieSecret.buffer, ssl->heap,
          DYNAMIC_TYPE_COOKIE_PWD);
#endif
#ifdef WOLFSSL_DTLS
    DtlsMsgPoolReset(ssl);
    if (ssl->dtls_rx_msg_list != NULL) {
        DtlsMsgListDelete(ssl->dtls_rx_msg_list, ssl->heap);
        ssl->dtls_rx_msg_list = NULL;
        ssl->dtls_rx_msg_list_sz = 0;
    }
    XFREE(ssl->buffers.dtlsCtx.peer.sa, ssl->heap, DYNAMIC_TYPE_SOCKADDR);
    ssl->buffers.dtlsCtx.peer.sa = NULL;
#ifndef NO_WOLFSSL_SERVER
    XFREE(ssl->buffers.dtlsCookieSecret.buffer, ssl->heap,
          DYNAMIC_TYPE_COOKIE_PWD);
#endif
#endif /* WOLFSSL_DTLS */
#if defined(OPENSSL_EXTRA) || defined(GOAHEAD_WS)
    if (ssl->biord != ssl->biowr)        /* only free write if different */
        wolfSSL_BIO_free(ssl->biowr);
    wolfSSL_BIO_free(ssl->biord);        /* always free read bio */
#endif
#ifdef HAVE_LIBZ
    FreeStreams(ssl);
#endif
#ifdef HAVE_ECC
    FreeKey(ssl, DYNAMIC_TYPE_ECC, (void**)&ssl->peerEccKey);
    ssl->peerEccKeyPresent = 0;
    FreeKey(ssl, DYNAMIC_TYPE_ECC, (void**)&ssl->peerEccDsaKey);
    ssl->peerEccDsaKeyPresent = 0;
#ifdef HAVE_CURVE25519
    if (!ssl->peerX25519KeyPresent &&
            ssl->eccTempKeyPresent != DYNAMIC_TYPE_CURVE25519)
#endif /* HAVE_CURVE25519 */
    {
        FreeKey(ssl, DYNAMIC_TYPE_ECC, (void**)&ssl->eccTempKey);
        ssl->eccTempKeyPresent = 0;
    }
#ifdef HAVE_CURVE25519
    else {
        FreeKey(ssl, DYNAMIC_TYPE_CURVE25519, (void**)&ssl->eccTempKey);
        ssl->eccTempKeyPresent = 0;
    }
    FreeKey(ssl, DYNAMIC_TYPE_CURVE25519, (void**)&ssl->peerX25519Key);
    ssl->peerX25519KeyPresent = 0;
#endif
#endif /* HAVE_ECC */
#ifdef HAVE_ED25519
    FreeKey(ssl, DYNAMIC_TYPE_ED25519, (void**)&ssl->peerEd25519Key);
    ssl->peerEd25519KeyPresent = 0;
    #ifdef HAVE_PK_CALLBACKS
        if (ssl->buffers.peerEd25519Key.buffer != NULL) {
            XFREE(ssl->buffers.peerEd25519Key.buffer, ssl->heap,
                                                          DYNAMIC_TYPE_ED25519);
            ssl->buffers.peerEd25519Key.buffer = NULL;
        }
    #endif
#endif
#ifdef HAVE_PK_CALLBACKS
    #ifdef HAVE_ECC
        XFREE(ssl->buffers.peerEccDsaKey.buffer, ssl->heap, DYNAMIC_TYPE_ECC);
    #endif /* HAVE_ECC */
    #ifndef NO_RSA
        XFREE(ssl->buffers.peerRsaKey.buffer, ssl->heap, DYNAMIC_TYPE_RSA);
    #endif /* NO_RSA */
#endif /* HAVE_PK_CALLBACKS */
#ifdef HAVE_TLS_EXTENSIONS
    TLSX_FreeAll(ssl->extensions, ssl->heap);

#ifdef HAVE_ALPN
    if (ssl->alpn_client_list != NULL) {
        XFREE(ssl->alpn_client_list, ssl->heap, DYNAMIC_TYPE_ALPN);
        ssl->alpn_client_list = NULL;
    }
#endif
#endif /* HAVE_TLS_EXTENSIONS */
#ifdef HAVE_NETX
    if (ssl->nxCtx.nxPacket)
        nx_packet_release(ssl->nxCtx.nxPacket);
#endif
#if defined(KEEP_PEER_CERT) || defined(GOAHEAD_WS)
    FreeX509(&ssl->peerCert);
#endif

#ifdef HAVE_SESSION_TICKET
    if (ssl->session.isDynamic) {
        XFREE(ssl->session.ticket, ssl->heap, DYNAMIC_TYPE_SESSION_TICK);
        ssl->session.ticket = ssl->session.staticTicket;
        ssl->session.isDynamic = 0;
        ssl->session.ticketLen = 0;
    }
#endif
#ifdef HAVE_EXT_CACHE
    wolfSSL_SESSION_free(ssl->extSession);
#endif
#ifdef HAVE_WRITE_DUP
    if (ssl->dupWrite) {
        FreeWriteDup(ssl);
    }
#endif

#if defined(WOLFSSL_TLS13) && defined(WOLFSSL_POST_HANDSHAKE_AUTH)
    while (ssl->certReqCtx != NULL) {
        CertReqCtx* curr = ssl->certReqCtx;
        ssl->certReqCtx = curr->next;
        XFREE(curr, ssl->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
#endif

#ifdef WOLFSSL_STATIC_MEMORY
    /* check if using fixed io buffers and free them */
    if (ssl->heap != NULL) {
    #ifdef WOLFSSL_HEAP_TEST
    /* avoid dereferencing a test value */
    if (ssl->heap != (void*)WOLFSSL_HEAP_TEST) {
    #endif
        WOLFSSL_HEAP_HINT* ssl_hint = (WOLFSSL_HEAP_HINT*)ssl->heap;
        WOLFSSL_HEAP*      ctx_heap;

        ctx_heap = ssl_hint->memory;
        if (wc_LockMutex(&(ctx_heap->memory_mutex)) != 0) {
            WOLFSSL_MSG("Bad memory_mutex lock");
        }
        ctx_heap->curIO--;
        if (FreeFixedIO(ctx_heap, &(ssl_hint->outBuf)) != 1) {
            WOLFSSL_MSG("Error freeing fixed output buffer");
        }
        if (FreeFixedIO(ctx_heap, &(ssl_hint->inBuf)) != 1) {
            WOLFSSL_MSG("Error freeing fixed output buffer");
        }
        if (ssl_hint->haFlag) { /* check if handshake count has been decreased*/
            ctx_heap->curHa--;
        }
        wc_UnLockMutex(&(ctx_heap->memory_mutex));

        /* check if tracking stats */
        if (ctx_heap->flag & WOLFMEM_TRACK_STATS) {
            XFREE(ssl_hint->stats, ssl->ctx->heap, DYNAMIC_TYPE_SSL);
        }
        XFREE(ssl->heap, ssl->ctx->heap, DYNAMIC_TYPE_SSL);
    #ifdef WOLFSSL_HEAP_TEST
    }
    #endif
    }
#endif /* WOLFSSL_STATIC_MEMORY */
}

/* Free any handshake resources no longer needed */
void FreeHandshakeResources(WOLFSSL* ssl)
{

#ifdef HAVE_SECURE_RENEGOTIATION
    if (ssl->secure_renegotiation && ssl->secure_renegotiation->enabled) {
        WOLFSSL_MSG("Secure Renegotiation needs to retain handshake resources");
        return;
    }
#endif

    /* input buffer */
    if (ssl->buffers.inputBuffer.dynamicFlag)
        ShrinkInputBuffer(ssl, NO_FORCED_FREE);

    /* suites */
    XFREE(ssl->suites, ssl->heap, DYNAMIC_TYPE_SUITES);
    ssl->suites = NULL;

    /* hsHashes */
    FreeHandshakeHashes(ssl);

    /* RNG */
    if (ssl->specs.cipher_type == stream || ssl->options.tls1_1 == 0) {
        if (ssl->options.weOwnRng) {
            wc_FreeRng(ssl->rng);
            XFREE(ssl->rng, ssl->heap, DYNAMIC_TYPE_RNG);
            ssl->rng = NULL;
            ssl->options.weOwnRng = 0;
        }
    }

#ifdef WOLFSSL_DTLS
    /* DTLS_POOL */
    if (ssl->options.dtls) {
        DtlsMsgPoolReset(ssl);
        DtlsMsgListDelete(ssl->dtls_rx_msg_list, ssl->heap);
        ssl->dtls_rx_msg_list = NULL;
        ssl->dtls_rx_msg_list_sz = 0;
    }
#endif

    /* arrays */
    if (ssl->options.saveArrays == 0)
        FreeArrays(ssl, 1);

#ifndef NO_RSA
    /* peerRsaKey */
    FreeKey(ssl, DYNAMIC_TYPE_RSA, (void**)&ssl->peerRsaKey);
    ssl->peerRsaKeyPresent = 0;
#endif

#ifdef HAVE_ECC
    FreeKey(ssl, DYNAMIC_TYPE_ECC, (void**)&ssl->peerEccKey);
    ssl->peerEccKeyPresent = 0;
    FreeKey(ssl, DYNAMIC_TYPE_ECC, (void**)&ssl->peerEccDsaKey);
    ssl->peerEccDsaKeyPresent = 0;
#ifdef HAVE_CURVE25519
    if (ssl->ecdhCurveOID != ECC_X25519_OID)
#endif /* HAVE_CURVE25519 */
    {
        FreeKey(ssl, DYNAMIC_TYPE_ECC, (void**)&ssl->eccTempKey);
        ssl->eccTempKeyPresent = 0;
    }
#ifdef HAVE_CURVE25519
    else {
        FreeKey(ssl, DYNAMIC_TYPE_CURVE25519, (void**)&ssl->eccTempKey);
        ssl->eccTempKeyPresent = 0;
    }
    FreeKey(ssl, DYNAMIC_TYPE_CURVE25519, (void**)&ssl->peerX25519Key);
    ssl->peerX25519KeyPresent = 0;
#endif /* HAVE_CURVE25519 */
#endif /* HAVE_ECC */
#ifndef NO_DH
    if (ssl->buffers.serverDH_Priv.buffer) {
        ForceZero(ssl->buffers.serverDH_Priv.buffer,
                                             ssl->buffers.serverDH_Priv.length);
    }
    XFREE(ssl->buffers.serverDH_Priv.buffer, ssl->heap, DYNAMIC_TYPE_PRIVATE_KEY);
    ssl->buffers.serverDH_Priv.buffer = NULL;
    XFREE(ssl->buffers.serverDH_Pub.buffer, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    ssl->buffers.serverDH_Pub.buffer = NULL;
    /* parameters (p,g) may be owned by ctx */
    if (ssl->buffers.weOwnDH || ssl->options.side == WOLFSSL_CLIENT_END) {
        XFREE(ssl->buffers.serverDH_G.buffer, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
        ssl->buffers.serverDH_G.buffer = NULL;
        XFREE(ssl->buffers.serverDH_P.buffer, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
        ssl->buffers.serverDH_P.buffer = NULL;
    }
#endif /* !NO_DH */
#ifndef NO_CERTS
    wolfSSL_UnloadCertsKeys(ssl);
#endif
#ifdef HAVE_PK_CALLBACKS
    #ifdef HAVE_ECC
        XFREE(ssl->buffers.peerEccDsaKey.buffer, ssl->heap, DYNAMIC_TYPE_ECC);
        ssl->buffers.peerEccDsaKey.buffer = NULL;
    #endif /* HAVE_ECC */
    #ifndef NO_RSA
        XFREE(ssl->buffers.peerRsaKey.buffer, ssl->heap, DYNAMIC_TYPE_RSA);
        ssl->buffers.peerRsaKey.buffer = NULL;
    #endif /* NO_RSA */
    #ifdef HAVE_ED25519
        XFREE(ssl->buffers.peerEd25519Key.buffer, ssl->heap,
                                                          DYNAMIC_TYPE_ED25519);
        ssl->buffers.peerEd25519Key.buffer = NULL;
    #endif
#endif /* HAVE_PK_CALLBACKS */

#ifdef HAVE_QSH
    QSH_FreeAll(ssl);
#endif

#ifdef HAVE_SESSION_TICKET
    if (ssl->session.isDynamic) {
        XFREE(ssl->session.ticket, ssl->heap, DYNAMIC_TYPE_SESSION_TICK);
        ssl->session.ticket = ssl->session.staticTicket;
        ssl->session.isDynamic = 0;
        ssl->session.ticketLen = 0;
    }
#endif

#ifdef WOLFSSL_STATIC_MEMORY
    /* when done with handshake decrement current handshake count */
    if (ssl->heap != NULL) {
    #ifdef WOLFSSL_HEAP_TEST
    /* avoid dereferencing a test value */
    if (ssl->heap != (void*)WOLFSSL_HEAP_TEST) {
    #endif
        WOLFSSL_HEAP_HINT* ssl_hint = (WOLFSSL_HEAP_HINT*)ssl->heap;
        WOLFSSL_HEAP*      ctx_heap;

        ctx_heap = ssl_hint->memory;
        if (wc_LockMutex(&(ctx_heap->memory_mutex)) != 0) {
            WOLFSSL_MSG("Bad memory_mutex lock");
        }
        ctx_heap->curHa--;
        ssl_hint->haFlag = 0; /* set to zero since handshake has been dec */
        wc_UnLockMutex(&(ctx_heap->memory_mutex));
    #ifdef WOLFSSL_HEAP_TEST
    }
    #endif
    }
#endif /* WOLFSSL_STATIC_MEMORY */
}


/* heap argument is the heap hint used when creating SSL */
void FreeSSL(WOLFSSL* ssl, void* heap)
{
    if (ssl->ctx) {
        FreeSSL_Ctx(ssl->ctx); /* will decrement and free underyling CTX if 0 */
    }
    SSL_ResourceFree(ssl);
    XFREE(ssl, heap, DYNAMIC_TYPE_SSL);
    (void)heap;
}


#if !defined(NO_OLD_TLS) || defined(HAVE_CHACHA) || defined(HAVE_AESCCM) \
    || defined(HAVE_AESGCM) || defined(WOLFSSL_DTLS)
static INLINE void GetSEQIncrement(WOLFSSL* ssl, int verify, word32 seq[2])
{
    if (verify) {
        seq[0] = ssl->keys.peer_sequence_number_hi;
        seq[1] = ssl->keys.peer_sequence_number_lo++;
        if (seq[1] > ssl->keys.peer_sequence_number_lo) {
            /* handle rollover */
            ssl->keys.peer_sequence_number_hi++;
        }
    }
    else {
        seq[0] = ssl->keys.sequence_number_hi;
        seq[1] = ssl->keys.sequence_number_lo++;
        if (seq[1] > ssl->keys.sequence_number_lo) {
            /* handle rollover */
            ssl->keys.sequence_number_hi++;
        }
    }
}


#ifdef WOLFSSL_DTLS
static INLINE void DtlsGetSEQ(WOLFSSL* ssl, int order, word32 seq[2])
{
    if (order == PREV_ORDER) {
        /* Previous epoch case */
        if (ssl->options.haveMcast) {
        #ifdef WOLFSSL_MULTICAST
            seq[0] = ((ssl->keys.dtls_epoch - 1) << 16) |
                     (ssl->options.mcastID << 8) |
                     (ssl->keys.dtls_prev_sequence_number_hi & 0xFF);
        #endif
        }
        else
            seq[0] = ((ssl->keys.dtls_epoch - 1) << 16) |
                     (ssl->keys.dtls_prev_sequence_number_hi & 0xFFFF);
        seq[1] = ssl->keys.dtls_prev_sequence_number_lo;
    }
    else if (order == PEER_ORDER) {
        if (ssl->options.haveMcast) {
        #ifdef WOLFSSL_MULTICAST
            seq[0] = (ssl->keys.curEpoch << 16) |
                     (ssl->keys.curPeerId << 8) |
                     (ssl->keys.curSeq_hi & 0xFF);
        #endif
        }
        else
            seq[0] = (ssl->keys.curEpoch << 16) |
                     (ssl->keys.curSeq_hi & 0xFFFF);
        seq[1] = ssl->keys.curSeq_lo; /* explicit from peer */
    }
    else {
        if (ssl->options.haveMcast) {
        #ifdef WOLFSSL_MULTICAST
            seq[0] = (ssl->keys.dtls_epoch << 16) |
                     (ssl->options.mcastID << 8) |
                     (ssl->keys.dtls_sequence_number_hi & 0xFF);
        #endif
        }
        else
            seq[0] = (ssl->keys.dtls_epoch << 16) |
                     (ssl->keys.dtls_sequence_number_hi & 0xFFFF);
        seq[1] = ssl->keys.dtls_sequence_number_lo;
    }
}

static INLINE void DtlsSEQIncrement(WOLFSSL* ssl, int order)
{
    word32 seq;

    if (order == PREV_ORDER) {
        seq = ssl->keys.dtls_prev_sequence_number_lo++;
        if (seq > ssl->keys.dtls_prev_sequence_number_lo) {
            /* handle rollover */
            ssl->keys.dtls_prev_sequence_number_hi++;
        }
    }
    else if (order == PEER_ORDER) {
        seq = ssl->keys.peer_sequence_number_lo++;
        if (seq > ssl->keys.peer_sequence_number_lo) {
            /* handle rollover */
            ssl->keys.peer_sequence_number_hi++;
        }
    }
    else {
        seq = ssl->keys.dtls_sequence_number_lo++;
        if (seq > ssl->keys.dtls_sequence_number_lo) {
            /* handle rollover */
            ssl->keys.dtls_sequence_number_hi++;
        }
    }
}
#endif /* WOLFSSL_DTLS */


static INLINE void WriteSEQ(WOLFSSL* ssl, int verifyOrder, byte* out)
{
    word32 seq[2] = {0, 0};

    if (!ssl->options.dtls) {
        GetSEQIncrement(ssl, verifyOrder, seq);
    }
    else {
#ifdef WOLFSSL_DTLS
        DtlsGetSEQ(ssl, verifyOrder, seq);
#endif
    }

    c32toa(seq[0], out);
    c32toa(seq[1], out + OPAQUE32_LEN);
}
#endif


#ifdef WOLFSSL_DTLS

/* functions for managing DTLS datagram reordering */

/* Need to allocate space for the handshake message header. The hashing
 * routines assume the message pointer is still within the buffer that
 * has the headers, and will include those headers in the hash. The store
 * routines need to take that into account as well. New will allocate
 * extra space for the headers. */
DtlsMsg* DtlsMsgNew(word32 sz, void* heap)
{
    DtlsMsg* msg = NULL;

    (void)heap;
    msg = (DtlsMsg*)XMALLOC(sizeof(DtlsMsg), heap, DYNAMIC_TYPE_DTLS_MSG);

    if (msg != NULL) {
        XMEMSET(msg, 0, sizeof(DtlsMsg));
        msg->buf = (byte*)XMALLOC(sz + DTLS_HANDSHAKE_HEADER_SZ,
                                                heap, DYNAMIC_TYPE_DTLS_BUFFER);
        if (msg->buf != NULL) {
            msg->sz = sz;
            msg->type = no_shake;
            msg->msg = msg->buf + DTLS_HANDSHAKE_HEADER_SZ;
        }
        else {
            XFREE(msg, heap, DYNAMIC_TYPE_DTLS_MSG);
            msg = NULL;
        }
    }

    return msg;
}

void DtlsMsgDelete(DtlsMsg* item, void* heap)
{
    (void)heap;

    if (item != NULL) {
        DtlsFrag* cur = item->fragList;
        while (cur != NULL) {
            DtlsFrag* next = cur->next;
            XFREE(cur, heap, DYNAMIC_TYPE_DTLS_FRAG);
            cur = next;
        }
        if (item->buf != NULL)
            XFREE(item->buf, heap, DYNAMIC_TYPE_DTLS_BUFFER);
        XFREE(item, heap, DYNAMIC_TYPE_DTLS_MSG);
    }
}


void DtlsMsgListDelete(DtlsMsg* head, void* heap)
{
    DtlsMsg* next;
    while (head) {
        next = head->next;
        DtlsMsgDelete(head, heap);
        head = next;
    }
}


/* Create a DTLS Fragment from *begin - end, adjust new *begin and bytesLeft */
static DtlsFrag* CreateFragment(word32* begin, word32 end, const byte* data,
                                byte* buf, word32* bytesLeft, void* heap)
{
    DtlsFrag* newFrag;
    word32 added = end - *begin + 1;

    (void)heap;
    newFrag = (DtlsFrag*)XMALLOC(sizeof(DtlsFrag), heap,
                                 DYNAMIC_TYPE_DTLS_FRAG);
    if (newFrag != NULL) {
        newFrag->next = NULL;
        newFrag->begin = *begin;
        newFrag->end = end;

        XMEMCPY(buf + *begin, data, added);
        *bytesLeft -= added;
        *begin = newFrag->end + 1;
    }

    return newFrag;
}


int DtlsMsgSet(DtlsMsg* msg, word32 seq, const byte* data, byte type,
                                   word32 fragOffset, word32 fragSz, void* heap)
{
    if (msg != NULL && data != NULL && msg->fragSz <= msg->sz &&
                                             (fragOffset + fragSz) <= msg->sz) {
        DtlsFrag* cur = msg->fragList;
        DtlsFrag* prev = cur;
        DtlsFrag* newFrag;
        word32 bytesLeft = fragSz; /* could be overlapping fragment */
        word32 startOffset = fragOffset;
        word32 added;

        msg->seq = seq;
        msg->type = type;

        if (fragOffset == 0) {
            XMEMCPY(msg->buf, data - DTLS_HANDSHAKE_HEADER_SZ,
                    DTLS_HANDSHAKE_HEADER_SZ);
            c32to24(msg->sz, msg->msg - DTLS_HANDSHAKE_FRAG_SZ);
        }

        /* if no mesage data, just return */
        if (fragSz == 0)
            return 0;

        /* if list is empty add full fragment to front */
        if (cur == NULL) {
            newFrag = CreateFragment(&fragOffset, fragOffset + fragSz - 1, data,
                                     msg->msg, &bytesLeft, heap);
            if (newFrag == NULL)
                return MEMORY_E;

            msg->fragSz = fragSz;
            msg->fragList = newFrag;

            return 0;
        }

        /* add to front if before current front, up to next->begin */
        if (fragOffset < cur->begin) {
            word32 end = fragOffset + fragSz - 1;

            if (end >= cur->begin)
                end = cur->begin - 1;

            added = end - fragOffset + 1;
            newFrag = CreateFragment(&fragOffset, end, data, msg->msg,
                                     &bytesLeft, heap);
            if (newFrag == NULL)
                return MEMORY_E;

            msg->fragSz += added;

            newFrag->next = cur;
            msg->fragList = newFrag;
        }

        /* while we have bytes left, try to find a gap to fill */
        while (bytesLeft > 0) {
            /* get previous packet in list */
            while (cur && (fragOffset >= cur->begin)) {
                prev = cur;
                cur = cur->next;
            }

            /* don't add duplicate data */
            if (prev->end >= fragOffset) {
                if ( (fragOffset + bytesLeft - 1) <= prev->end)
                    return 0;
                fragOffset = prev->end + 1;
                bytesLeft = startOffset + fragSz - fragOffset;
            }

            if (cur == NULL)
                /* we're at the end */
                added = bytesLeft;
            else
                /* we're in between two frames */
                added = min(bytesLeft, cur->begin - fragOffset);

            /* data already there */
            if (added == 0)
                continue;

            newFrag = CreateFragment(&fragOffset, fragOffset + added - 1,
                                     data + fragOffset - startOffset,
                                     msg->msg, &bytesLeft, heap);
            if (newFrag == NULL)
                return MEMORY_E;

            msg->fragSz += added;

            newFrag->next = prev->next;
            prev->next = newFrag;
        }
    }

    return 0;
}


DtlsMsg* DtlsMsgFind(DtlsMsg* head, word32 seq)
{
    while (head != NULL && head->seq != seq) {
        head = head->next;
    }
    return head;
}


void DtlsMsgStore(WOLFSSL* ssl, word32 seq, const byte* data,
        word32 dataSz, byte type, word32 fragOffset, word32 fragSz, void* heap)
{
    /* See if seq exists in the list. If it isn't in the list, make
     * a new item of size dataSz, copy fragSz bytes from data to msg->msg
     * starting at offset fragOffset, and add fragSz to msg->fragSz. If
     * the seq is in the list and it isn't full, copy fragSz bytes from
     * data to msg->msg starting at offset fragOffset, and add fragSz to
     * msg->fragSz. Insertions take into account data already in the list
     * in case there are overlaps in the handshake message due to retransmit
     * messages. The new item should be inserted into the list in its
     * proper position.
     *
     * 1. Find seq in list, or where seq should go in list. If seq not in
     *    list, create new item and insert into list. Either case, keep
     *    pointer to item.
     * 2. Copy the data from the message to the stored message where it
     *    belongs without overlaps.
     */

    DtlsMsg* head = ssl->dtls_rx_msg_list;

    if (head != NULL) {
        DtlsMsg* cur = DtlsMsgFind(head, seq);
        if (cur == NULL) {
            cur = DtlsMsgNew(dataSz, heap);
            if (cur != NULL) {
                if (DtlsMsgSet(cur, seq, data, type,
                                                fragOffset, fragSz, heap) < 0) {
                    DtlsMsgDelete(cur, heap);
                }
                else {
                    ssl->dtls_rx_msg_list_sz++;
                    head = DtlsMsgInsert(head, cur);
                }
            }
        }
        else {
            /* If this fails, the data is just dropped. */
            DtlsMsgSet(cur, seq, data, type, fragOffset, fragSz, heap);
        }
    }
    else {
        head = DtlsMsgNew(dataSz, heap);
        if (DtlsMsgSet(head, seq, data, type, fragOffset, fragSz, heap) < 0) {
            DtlsMsgDelete(head, heap);
            head = NULL;
        }
        else {
            ssl->dtls_rx_msg_list_sz++;
        }
    }

    ssl->dtls_rx_msg_list = head;
}


/* DtlsMsgInsert() is an in-order insert. */
DtlsMsg* DtlsMsgInsert(DtlsMsg* head, DtlsMsg* item)
{
    if (head == NULL || item->seq < head->seq) {
        item->next = head;
        head = item;
    }
    else if (head->next == NULL) {
        head->next = item;
    }
    else {
        DtlsMsg* cur = head->next;
        DtlsMsg* prev = head;
        while (cur) {
            if (item->seq < cur->seq) {
                item->next = cur;
                prev->next = item;
                break;
            }
            prev = cur;
            cur = cur->next;
        }
        if (cur == NULL) {
            prev->next = item;
        }
    }

    return head;
}


/* DtlsMsgPoolSave() adds the message to the end of the stored transmit list. */
int DtlsMsgPoolSave(WOLFSSL* ssl, const byte* data, word32 dataSz)
{
    DtlsMsg* item;
    int ret = 0;

    if (ssl->dtls_tx_msg_list_sz > DTLS_POOL_SZ)
        return DTLS_POOL_SZ_E;

    item = DtlsMsgNew(dataSz, ssl->heap);

    if (item != NULL) {
        DtlsMsg* cur = ssl->dtls_tx_msg_list;

        XMEMCPY(item->buf, data, dataSz);
        item->sz = dataSz;
        item->seq = ssl->keys.dtls_epoch;

        if (cur == NULL)
            ssl->dtls_tx_msg_list = item;
        else {
            while (cur->next)
                cur = cur->next;
            cur->next = item;
        }
        ssl->dtls_tx_msg_list_sz++;
    }
    else
        ret = MEMORY_E;

    return ret;
}


/* DtlsMsgPoolTimeout() updates the timeout time. */
int DtlsMsgPoolTimeout(WOLFSSL* ssl)
{
    int result = -1;
    if (ssl->dtls_timeout <  ssl->dtls_timeout_max) {
        ssl->dtls_timeout *= DTLS_TIMEOUT_MULTIPLIER;
        result = 0;
    }
    return result;
}


/* DtlsMsgPoolReset() deletes the stored transmit list and resets the timeout
 * value. */
void DtlsMsgPoolReset(WOLFSSL* ssl)
{
    if (ssl->dtls_tx_msg_list) {
        DtlsMsgListDelete(ssl->dtls_tx_msg_list, ssl->heap);
        ssl->dtls_tx_msg_list = NULL;
        ssl->dtls_tx_msg_list_sz = 0;
        ssl->dtls_timeout = ssl->dtls_timeout_init;
    }
}


int VerifyForDtlsMsgPoolSend(WOLFSSL* ssl, byte type, word32 fragOffset)
{
    /**
     * only the first message from previous flight should be valid
     * to be used for triggering retransmission of whole DtlsMsgPool.
     * change cipher suite type is not verified here
     */
    return ((fragOffset == 0) &&
           (((ssl->options.side == WOLFSSL_SERVER_END) &&
             ((type == client_hello) ||
             ((ssl->options.verifyPeer) && (type == certificate)) ||
             ((!ssl->options.verifyPeer) && (type == client_key_exchange)))) ||
            ((ssl->options.side == WOLFSSL_CLIENT_END) &&
             (type == server_hello))));
}


/* DtlsMsgPoolSend() will send the stored transmit list. The stored list is
 * updated with new sequence numbers, and will be re-encrypted if needed. */
int DtlsMsgPoolSend(WOLFSSL* ssl, int sendOnlyFirstPacket)
{
    int ret = 0;
    DtlsMsg* pool = ssl->dtls_tx_msg_list;

    if (pool != NULL) {

        while (pool != NULL) {
            if (pool->seq == 0) {
                DtlsRecordLayerHeader* dtls;
                int epochOrder;

                dtls = (DtlsRecordLayerHeader*)pool->buf;
                /* If the stored record's epoch is 0, and the currently set
                 * epoch is 0, use the "current order" sequence number.
                 * If the stored record's epoch is 0 and the currently set
                 * epoch is not 0, the stored record is considered a "previous
                 * order" sequence number. */
                epochOrder = (ssl->keys.dtls_epoch == 0) ?
                             CUR_ORDER : PREV_ORDER;

                WriteSEQ(ssl, epochOrder, dtls->sequence_number);
                DtlsSEQIncrement(ssl, epochOrder);
                if ((ret = CheckAvailableSize(ssl, pool->sz)) != 0)
                    return ret;

                XMEMCPY(ssl->buffers.outputBuffer.buffer,
                        pool->buf, pool->sz);
                ssl->buffers.outputBuffer.idx = 0;
                ssl->buffers.outputBuffer.length = pool->sz;
            }
            else if (pool->seq == ssl->keys.dtls_epoch) {
                byte*  input;
                byte*  output;
                int    inputSz, sendSz;

                input = pool->buf;
                inputSz = pool->sz;
                sendSz = inputSz + MAX_MSG_EXTRA;

                if ((ret = CheckAvailableSize(ssl, sendSz)) != 0)
                    return ret;

                output = ssl->buffers.outputBuffer.buffer +
                         ssl->buffers.outputBuffer.length;
                sendSz = BuildMessage(ssl, output, sendSz, input, inputSz,
                                      handshake, 0, 0, 0);
                if (sendSz < 0)
                    return BUILD_MSG_ERROR;

                ssl->buffers.outputBuffer.length += sendSz;
            }

            ret = SendBuffered(ssl);
            if (ret < 0) {
                return ret;
            }

            /**
             * on server side, retranmission is being triggered only by sending
             * first message of given flight, in order to trigger client
             * to retransmit its whole flight. Sending the whole previous flight
             * could lead to retranmission of previous client flight for each
             * server message from previous flight. Therefore one message should
             * be enough to do the trick.
             */
            if (sendOnlyFirstPacket &&
                ssl->options.side == WOLFSSL_SERVER_END) {

                pool = NULL;
            }
            else
                pool = pool->next;
        }
    }

    return ret;
}

#endif /* WOLFSSL_DTLS */

#if defined(WOLFSSL_ALLOW_SSLV3) && !defined(NO_OLD_TLS)

ProtocolVersion MakeSSLv3(void)
{
    ProtocolVersion pv;
    pv.major = SSLv3_MAJOR;
    pv.minor = SSLv3_MINOR;

    return pv;
}

#endif /* WOLFSSL_ALLOW_SSLV3 && !NO_OLD_TLS */


#ifdef WOLFSSL_DTLS

ProtocolVersion MakeDTLSv1(void)
{
    ProtocolVersion pv;
    pv.major = DTLS_MAJOR;
    pv.minor = DTLS_MINOR;

    return pv;
}

ProtocolVersion MakeDTLSv1_2(void)
{
    ProtocolVersion pv;
    pv.major = DTLS_MAJOR;
    pv.minor = DTLSv1_2_MINOR;

    return pv;
}

#endif /* WOLFSSL_DTLS */




#if defined(USER_TICKS)
#if 0
    word32 LowResTimer(void)
    {
        /*
        write your own clock tick function if don't want time(0)
        needs second accuracy but doesn't have to correlated to EPOCH
        */
    }
#endif

#elif defined(TIME_OVERRIDES)

    /* use same asn time overrides unless user wants tick override above */

    #ifndef HAVE_TIME_T_TYPE
        typedef long time_t;
    #endif
    extern time_t XTIME(time_t * timer);

    word32 LowResTimer(void)
    {
        return (word32) XTIME(0);
    }

#elif defined(USE_WINDOWS_API)

    word32 LowResTimer(void)
    {
        static int           init = 0;
        static LARGE_INTEGER freq;
        LARGE_INTEGER        count;

        if (!init) {
            QueryPerformanceFrequency(&freq);
            init = 1;
        }

        QueryPerformanceCounter(&count);

        return (word32)(count.QuadPart / freq.QuadPart);
    }

#elif defined(HAVE_RTP_SYS)

    #include "rtptime.h"

    word32 LowResTimer(void)
    {
        return (word32)rtp_get_system_sec();
    }


#elif defined(MICRIUM)

    word32 LowResTimer(void)
    {
        OS_TICK ticks = 0;
        OS_ERR  err;

        ticks = OSTimeGet(&err);

        return (word32) (ticks / OSCfg_TickRate_Hz);
    }


#elif defined(MICROCHIP_TCPIP_V5)

    word32 LowResTimer(void)
    {
        return (word32) (TickGet() / TICKS_PER_SECOND);
    }


#elif defined(MICROCHIP_TCPIP)

    #if defined(MICROCHIP_MPLAB_HARMONY)

        #include <system/tmr/sys_tmr.h>

        word32 LowResTimer(void)
        {
            return (word32) (SYS_TMR_TickCountGet() /
                             SYS_TMR_TickCounterFrequencyGet());
        }

    #else

        word32 LowResTimer(void)
        {
            return (word32) (SYS_TICK_Get() / SYS_TICK_TicksPerSecondGet());
        }

    #endif

#elif defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)

    word32 LowResTimer(void)
    {
        TIME_STRUCT mqxTime;

        _time_get_elapsed(&mqxTime);

        return (word32) mqxTime.SECONDS;
    }
#elif defined(FREESCALE_FREE_RTOS) || defined(FREESCALE_KSDK_FREERTOS)

    #include "include/task.h"

    unsigned int LowResTimer(void)
    {
        return (unsigned int)(((float)xTaskGetTickCount())/configTICK_RATE_HZ);
    }

#elif defined(FREESCALE_KSDK_BM)

    #include "lwip/sys.h" /* lwIP */
    word32 LowResTimer(void)
    {
        return sys_now()/1000;
    }

#elif defined(WOLFSSL_TIRTOS)

    word32 LowResTimer(void)
    {
        return (word32) Seconds_get();
    }
#elif defined(WOLFSSL_XILINX)
    #include "xrtcpsu.h"

    word32 LowResTimer(void)
    {
        XRtcPsu_Config* con;
        XRtcPsu         rtc;

        con = XRtcPsu_LookupConfig(XPAR_XRTCPSU_0_DEVICE_ID);
        if (con != NULL) {
            if (XRtcPsu_CfgInitialize(&rtc, con, con->BaseAddr)
                    == XST_SUCCESS) {
                return (word32)XRtcPsu_GetCurrentTime(&rtc);
            }
            else {
                WOLFSSL_MSG("Unable to initialize RTC");
            }
        }

        return 0;
    }

#elif defined(WOLFSSL_UTASKER)

    word32 LowResTimer(void)
    {
        return (word32)(uTaskerSystemTick / TICK_RESOLUTION);
    }

#else
    /* Posix style time */
    #include <time.h>

    word32 LowResTimer(void)
    {
        return (word32)time(0);
    }


#endif


#ifndef NO_CERTS
int HashOutputRaw(WOLFSSL* ssl, const byte* output, int sz)
{
    int ret = 0;

    (void)output;
    (void)sz;

    if (ssl->hsHashes == NULL)
        return BAD_FUNC_ARG;

#ifdef HAVE_FUZZER
    if (ssl->fuzzerCb)
        ssl->fuzzerCb(ssl, output, sz, FUZZ_HASH, ssl->fuzzerCtx);
#endif
#ifndef NO_OLD_TLS
#ifndef NO_SHA
    wc_ShaUpdate(&ssl->hsHashes->hashSha, output, sz);
#endif
#ifndef NO_MD5
    wc_Md5Update(&ssl->hsHashes->hashMd5, output, sz);
#endif
#endif /* NO_OLD_TLS */

    if (IsAtLeastTLSv1_2(ssl)) {
#ifndef NO_SHA256
        ret = wc_Sha256Update(&ssl->hsHashes->hashSha256, output, sz);
        if (ret != 0)
            return ret;
#endif
#ifdef WOLFSSL_SHA384
        ret = wc_Sha384Update(&ssl->hsHashes->hashSha384, output, sz);
        if (ret != 0)
            return ret;
#endif
#ifdef WOLFSSL_SHA512
        ret = wc_Sha512Update(&ssl->hsHashes->hashSha512, output, sz);
        if (ret != 0)
            return ret;
#endif
    }

    return ret;
}
#endif /* NO_CERTS */


/* add output to md5 and sha handshake hashes, exclude record header */
int HashOutput(WOLFSSL* ssl, const byte* output, int sz, int ivSz)
{
    int ret = 0;
    const byte* adj;

    adj = output + RECORD_HEADER_SZ + ivSz;
    sz -= RECORD_HEADER_SZ;

#ifdef HAVE_FUZZER
    if (ssl->fuzzerCb)
        ssl->fuzzerCb(ssl, output, sz, FUZZ_HASH, ssl->fuzzerCtx);
#endif
#ifdef WOLFSSL_DTLS
    if (ssl->options.dtls) {
        adj += DTLS_RECORD_EXTRA;
        sz  -= DTLS_RECORD_EXTRA;
    }
#endif
#ifndef NO_OLD_TLS
#ifndef NO_SHA
    wc_ShaUpdate(&ssl->hsHashes->hashSha, adj, sz);
#endif
#ifndef NO_MD5
    wc_Md5Update(&ssl->hsHashes->hashMd5, adj, sz);
#endif
#endif

    if (IsAtLeastTLSv1_2(ssl)) {
#ifndef NO_SHA256
        ret = wc_Sha256Update(&ssl->hsHashes->hashSha256, adj, sz);
        if (ret != 0)
            return ret;
#endif
#ifdef WOLFSSL_SHA384
        ret = wc_Sha384Update(&ssl->hsHashes->hashSha384, adj, sz);
        if (ret != 0)
            return ret;
#endif
#ifdef WOLFSSL_SHA512
        ret = wc_Sha512Update(&ssl->hsHashes->hashSha512, adj, sz);
        if (ret != 0)
            return ret;
#endif
    }

    return ret;
}


/* add input to md5 and sha handshake hashes, include handshake header */
int HashInput(WOLFSSL* ssl, const byte* input, int sz)
{
    int ret = 0;
    const byte* adj;

    adj = input - HANDSHAKE_HEADER_SZ;
    sz += HANDSHAKE_HEADER_SZ;

    (void)adj;

#ifdef WOLFSSL_DTLS
    if (ssl->options.dtls) {
        adj -= DTLS_HANDSHAKE_EXTRA;
        sz  += DTLS_HANDSHAKE_EXTRA;
    }
#endif

    if (ssl->hsHashes == NULL) {
        return BAD_FUNC_ARG;
    }

#ifndef NO_OLD_TLS
#ifndef NO_SHA
    wc_ShaUpdate(&ssl->hsHashes->hashSha, adj, sz);
#endif
#ifndef NO_MD5
    wc_Md5Update(&ssl->hsHashes->hashMd5, adj, sz);
#endif
#endif

    if (IsAtLeastTLSv1_2(ssl)) {
#ifndef NO_SHA256
        ret = wc_Sha256Update(&ssl->hsHashes->hashSha256, adj, sz);
        if (ret != 0)
            return ret;
#endif
#ifdef WOLFSSL_SHA384
        ret = wc_Sha384Update(&ssl->hsHashes->hashSha384, adj, sz);
        if (ret != 0)
            return ret;
#endif
#ifdef WOLFSSL_SHA512
        ret = wc_Sha512Update(&ssl->hsHashes->hashSha512, adj, sz);
        if (ret != 0)
            return ret;
#endif
    }

    return ret;
}


/* add record layer header for message */
static void AddRecordHeader(byte* output, word32 length, byte type, WOLFSSL* ssl)
{
    RecordLayerHeader* rl;

    /* record layer header */
    rl = (RecordLayerHeader*)output;
    if (rl == NULL) {
        return;
    }
    rl->type    = type;
    rl->pvMajor = ssl->version.major;       /* type and version same in each */
#ifdef WOLFSSL_TLS13
    if (IsAtLeastTLSv1_3(ssl->version))
        rl->pvMinor = TLSv1_MINOR;
    else
#endif
        rl->pvMinor = ssl->version.minor;

#ifdef WOLFSSL_ALTERNATIVE_DOWNGRADE
    if (ssl->options.side == WOLFSSL_CLIENT_END
    &&  ssl->options.connectState == CONNECT_BEGIN
    && !ssl->options.resuming) {
        rl->pvMinor = ssl->options.downgrade ? ssl->options.minDowngrade
                                             : ssl->version.minor;
    }
#endif

    if (!ssl->options.dtls) {
        c16toa((word16)length, rl->length);
    }
    else {
#ifdef WOLFSSL_DTLS
        DtlsRecordLayerHeader* dtls;

        /* dtls record layer header extensions */
        dtls = (DtlsRecordLayerHeader*)output;
        WriteSEQ(ssl, 0, dtls->sequence_number);
        c16toa((word16)length, dtls->length);
#endif
    }
}


/* add handshake header for message */
static void AddHandShakeHeader(byte* output, word32 length,
                               word32 fragOffset, word32 fragLength,
                               byte type, WOLFSSL* ssl)
{
    HandShakeHeader* hs;
    (void)fragOffset;
    (void)fragLength;
    (void)ssl;

    /* handshake header */
    hs = (HandShakeHeader*)output;
    hs->type = type;
    c32to24(length, hs->length);         /* type and length same for each */
#ifdef WOLFSSL_DTLS
    if (ssl->options.dtls) {
        DtlsHandShakeHeader* dtls;

        /* dtls handshake header extensions */
        dtls = (DtlsHandShakeHeader*)output;
        c16toa(ssl->keys.dtls_handshake_number++, dtls->message_seq);
        c32to24(fragOffset, dtls->fragment_offset);
        c32to24(fragLength, dtls->fragment_length);
    }
#endif
}


/* add both headers for handshake message */
static void AddHeaders(byte* output, word32 length, byte type, WOLFSSL* ssl)
{
    word32 lengthAdj = HANDSHAKE_HEADER_SZ;
    word32 outputAdj = RECORD_HEADER_SZ;

#ifdef WOLFSSL_DTLS
    if (ssl->options.dtls) {
        lengthAdj += DTLS_HANDSHAKE_EXTRA;
        outputAdj += DTLS_RECORD_EXTRA;
    }
#endif

    AddRecordHeader(output, length + lengthAdj, handshake, ssl);
    AddHandShakeHeader(output + outputAdj, length, 0, length, type, ssl);
}


#ifndef NO_CERTS
static void AddFragHeaders(byte* output, word32 fragSz, word32 fragOffset,
                           word32 length, byte type, WOLFSSL* ssl)
{
    word32 lengthAdj = HANDSHAKE_HEADER_SZ;
    word32 outputAdj = RECORD_HEADER_SZ;
    (void)fragSz;

#ifdef WOLFSSL_DTLS
    if (ssl->options.dtls) {
        lengthAdj += DTLS_HANDSHAKE_EXTRA;
        outputAdj += DTLS_RECORD_EXTRA;
    }
#endif

    AddRecordHeader(output, fragSz + lengthAdj, handshake, ssl);
    AddHandShakeHeader(output + outputAdj, length, fragOffset, fragSz, type, ssl);
}
#endif /* NO_CERTS */


/* return bytes received, -1 on error */
static int Receive(WOLFSSL* ssl, byte* buf, word32 sz)
{
    int recvd;

    if (ssl->ctx->CBIORecv == NULL) {
        WOLFSSL_MSG("Your IO Recv callback is null, please set");
        return -1;
    }

retry:
    recvd = ssl->ctx->CBIORecv(ssl, (char *)buf, (int)sz, ssl->IOCB_ReadCtx);
    if (recvd < 0)
        switch (recvd) {
            case WOLFSSL_CBIO_ERR_GENERAL:        /* general/unknown error */
                return -1;

            case WOLFSSL_CBIO_ERR_WANT_READ:      /* want read, would block */
                return WANT_READ;

            case WOLFSSL_CBIO_ERR_CONN_RST:       /* connection reset */
                #ifdef USE_WINDOWS_API
                if (ssl->options.dtls) {
                    goto retry;
                }
                #endif
                ssl->options.connReset = 1;
                return -1;

            case WOLFSSL_CBIO_ERR_ISR:            /* interrupt */
                /* see if we got our timeout */
                #ifdef WOLFSSL_CALLBACKS
                    if (ssl->toInfoOn) {
                        struct itimerval timeout;
                        getitimer(ITIMER_REAL, &timeout);
                        if (timeout.it_value.tv_sec == 0 &&
                                                timeout.it_value.tv_usec == 0) {
                            XSTRNCPY(ssl->timeoutInfo.timeoutName,
                                    "recv() timeout", MAX_TIMEOUT_NAME_SZ);
                            WOLFSSL_MSG("Got our timeout");
                            return WANT_READ;
                        }
                    }
                #endif
                goto retry;

            case WOLFSSL_CBIO_ERR_CONN_CLOSE:     /* peer closed connection */
                ssl->options.isClosed = 1;
                return -1;

            case WOLFSSL_CBIO_ERR_TIMEOUT:
                #ifdef WOLFSSL_DTLS
                if (IsDtlsNotSctpMode(ssl) &&
                    !ssl->options.handShakeDone &&
                    DtlsMsgPoolTimeout(ssl) == 0 &&
                    DtlsMsgPoolSend(ssl, 0) == 0) {

                    goto retry;
                }
                #endif
                return -1;

            default:
                return recvd;
        }

    return recvd;
}


/* Switch dynamic output buffer back to static, buffer is assumed clear */
void ShrinkOutputBuffer(WOLFSSL* ssl)
{
    WOLFSSL_MSG("Shrinking output buffer\n");
    XFREE(ssl->buffers.outputBuffer.buffer - ssl->buffers.outputBuffer.offset,
          ssl->heap, DYNAMIC_TYPE_OUT_BUFFER);
    ssl->buffers.outputBuffer.buffer = ssl->buffers.outputBuffer.staticBuffer;
    ssl->buffers.outputBuffer.bufferSize  = STATIC_BUFFER_LEN;
    ssl->buffers.outputBuffer.dynamicFlag = 0;
    ssl->buffers.outputBuffer.offset      = 0;
}


/* Switch dynamic input buffer back to static, keep any remaining input */
/* forced free means cleaning up */
void ShrinkInputBuffer(WOLFSSL* ssl, int forcedFree)
{
    int usedLength = ssl->buffers.inputBuffer.length -
                     ssl->buffers.inputBuffer.idx;
    if (!forcedFree && usedLength > STATIC_BUFFER_LEN)
        return;

    WOLFSSL_MSG("Shrinking input buffer\n");

    if (!forcedFree && usedLength > 0)
        XMEMCPY(ssl->buffers.inputBuffer.staticBuffer,
               ssl->buffers.inputBuffer.buffer + ssl->buffers.inputBuffer.idx,
               usedLength);

    XFREE(ssl->buffers.inputBuffer.buffer - ssl->buffers.inputBuffer.offset,
          ssl->heap, DYNAMIC_TYPE_IN_BUFFER);
    ssl->buffers.inputBuffer.buffer = ssl->buffers.inputBuffer.staticBuffer;
    ssl->buffers.inputBuffer.bufferSize  = STATIC_BUFFER_LEN;
    ssl->buffers.inputBuffer.dynamicFlag = 0;
    ssl->buffers.inputBuffer.offset      = 0;
    ssl->buffers.inputBuffer.idx = 0;
    ssl->buffers.inputBuffer.length = usedLength;
}

int SendBuffered(WOLFSSL* ssl)
{
    if (ssl->ctx->CBIOSend == NULL) {
        WOLFSSL_MSG("Your IO Send callback is null, please set");
        return SOCKET_ERROR_E;
    }

#ifdef WOLFSSL_DEBUG_TLS
    if (ssl->buffers.outputBuffer.idx == 0) {
        WOLFSSL_MSG("Data to send");
        WOLFSSL_BUFFER(ssl->buffers.outputBuffer.buffer,
                       ssl->buffers.outputBuffer.length);
    }
#endif

    while (ssl->buffers.outputBuffer.length > 0) {
        int sent = ssl->ctx->CBIOSend(ssl,
                                      (char*)ssl->buffers.outputBuffer.buffer +
                                      ssl->buffers.outputBuffer.idx,
                                      (int)ssl->buffers.outputBuffer.length,
                                      ssl->IOCB_WriteCtx);
        if (sent < 0) {
            switch (sent) {

                case WOLFSSL_CBIO_ERR_WANT_WRITE:        /* would block */
                    return WANT_WRITE;

                case WOLFSSL_CBIO_ERR_CONN_RST:          /* connection reset */
                    ssl->options.connReset = 1;
                    break;

                case WOLFSSL_CBIO_ERR_ISR:               /* interrupt */
                    /* see if we got our timeout */
                    #ifdef WOLFSSL_CALLBACKS
                        if (ssl->toInfoOn) {
                            struct itimerval timeout;
                            getitimer(ITIMER_REAL, &timeout);
                            if (timeout.it_value.tv_sec == 0 &&
                                                timeout.it_value.tv_usec == 0) {
                                XSTRNCPY(ssl->timeoutInfo.timeoutName,
                                        "send() timeout", MAX_TIMEOUT_NAME_SZ);
                                WOLFSSL_MSG("Got our timeout");
                                return WANT_WRITE;
                            }
                        }
                    #endif
                    continue;

                case WOLFSSL_CBIO_ERR_CONN_CLOSE: /* epipe / conn closed */
                    ssl->options.connReset = 1;  /* treat same as reset */
                    break;

                default:
                    return SOCKET_ERROR_E;
            }

            return SOCKET_ERROR_E;
        }

        if (sent > (int)ssl->buffers.outputBuffer.length) {
            WOLFSSL_MSG("SendBuffered() out of bounds read");
            return SEND_OOB_READ_E;
        }

        ssl->buffers.outputBuffer.idx += sent;
        ssl->buffers.outputBuffer.length -= sent;
    }

    ssl->buffers.outputBuffer.idx = 0;

    if (ssl->buffers.outputBuffer.dynamicFlag)
        ShrinkOutputBuffer(ssl);

    return 0;
}


/* Grow the output buffer */
static INLINE int GrowOutputBuffer(WOLFSSL* ssl, int size)
{
    byte* tmp;
#if WOLFSSL_GENERAL_ALIGNMENT > 0
    byte  hdrSz = ssl->options.dtls ? DTLS_RECORD_HEADER_SZ :
                                      RECORD_HEADER_SZ;
    byte align = WOLFSSL_GENERAL_ALIGNMENT;
#else
    const byte align = WOLFSSL_GENERAL_ALIGNMENT;
#endif

#if WOLFSSL_GENERAL_ALIGNMENT > 0
    /* the encrypted data will be offset from the front of the buffer by
       the header, if the user wants encrypted alignment they need
       to define their alignment requirement */

    if (align) {
       while (align < hdrSz)
           align *= 2;
    }
#endif

    tmp = (byte*)XMALLOC(size + ssl->buffers.outputBuffer.length + align,
                             ssl->heap, DYNAMIC_TYPE_OUT_BUFFER);
    WOLFSSL_MSG("growing output buffer\n");

    if (tmp == NULL)
        return MEMORY_E;

#if WOLFSSL_GENERAL_ALIGNMENT > 0
    if (align)
        tmp += align - hdrSz;
#endif

    if (ssl->buffers.outputBuffer.length)
        XMEMCPY(tmp, ssl->buffers.outputBuffer.buffer,
               ssl->buffers.outputBuffer.length);

    if (ssl->buffers.outputBuffer.dynamicFlag)
        XFREE(ssl->buffers.outputBuffer.buffer -
              ssl->buffers.outputBuffer.offset, ssl->heap,
              DYNAMIC_TYPE_OUT_BUFFER);
    ssl->buffers.outputBuffer.dynamicFlag = 1;

#if WOLFSSL_GENERAL_ALIGNMENT > 0
    if (align)
        ssl->buffers.outputBuffer.offset = align - hdrSz;
    else
#endif
        ssl->buffers.outputBuffer.offset = 0;

    ssl->buffers.outputBuffer.buffer = tmp;
    ssl->buffers.outputBuffer.bufferSize = size +
                                           ssl->buffers.outputBuffer.length;
    return 0;
}


/* Grow the input buffer, should only be to read cert or big app data */
int GrowInputBuffer(WOLFSSL* ssl, int size, int usedLength)
{
    byte* tmp;
#if defined(WOLFSSL_DTLS) || WOLFSSL_GENERAL_ALIGNMENT > 0
    byte  align = ssl->options.dtls ? WOLFSSL_GENERAL_ALIGNMENT : 0;
    byte  hdrSz = DTLS_RECORD_HEADER_SZ;
#else
    const byte align = WOLFSSL_GENERAL_ALIGNMENT;
#endif

#if defined(WOLFSSL_DTLS) || WOLFSSL_GENERAL_ALIGNMENT > 0
    /* the encrypted data will be offset from the front of the buffer by
       the dtls record header, if the user wants encrypted alignment they need
       to define their alignment requirement. in tls we read record header
       to get size of record and put actual data back at front, so don't need */

    if (align) {
       while (align < hdrSz)
           align *= 2;
    }
#endif

    if (usedLength < 0 || size < 0) {
        WOLFSSL_MSG("GrowInputBuffer() called with negative number");
        return BAD_FUNC_ARG;
    }

    tmp = (byte*)XMALLOC(size + usedLength + align,
                             ssl->heap, DYNAMIC_TYPE_IN_BUFFER);
    WOLFSSL_MSG("growing input buffer\n");

    if (tmp == NULL)
        return MEMORY_E;

#if defined(WOLFSSL_DTLS) || WOLFSSL_GENERAL_ALIGNMENT > 0
    if (align)
        tmp += align - hdrSz;
#endif

    if (usedLength)
        XMEMCPY(tmp, ssl->buffers.inputBuffer.buffer +
                    ssl->buffers.inputBuffer.idx, usedLength);

    if (ssl->buffers.inputBuffer.dynamicFlag)
        XFREE(ssl->buffers.inputBuffer.buffer - ssl->buffers.inputBuffer.offset,
              ssl->heap,DYNAMIC_TYPE_IN_BUFFER);

    ssl->buffers.inputBuffer.dynamicFlag = 1;
#if defined(WOLFSSL_DTLS) || WOLFSSL_GENERAL_ALIGNMENT > 0
    if (align)
        ssl->buffers.inputBuffer.offset = align - hdrSz;
    else
#endif
        ssl->buffers.inputBuffer.offset = 0;

    ssl->buffers.inputBuffer.buffer = tmp;
    ssl->buffers.inputBuffer.bufferSize = size + usedLength;
    ssl->buffers.inputBuffer.idx    = 0;
    ssl->buffers.inputBuffer.length = usedLength;

    return 0;
}


/* check available size into output buffer, make room if needed */
int CheckAvailableSize(WOLFSSL *ssl, int size)
{
    if (size < 0) {
        WOLFSSL_MSG("CheckAvailableSize() called with negative number");
        return BAD_FUNC_ARG;
    }

    if (ssl->buffers.outputBuffer.bufferSize - ssl->buffers.outputBuffer.length
                                             < (word32)size) {
        if (GrowOutputBuffer(ssl, size) < 0)
            return MEMORY_E;
    }

    return 0;
}


/* do all verify and sanity checks on record header */
static int GetRecordHeader(WOLFSSL* ssl, const byte* input, word32* inOutIdx,
                           RecordLayerHeader* rh, word16 *size)
{
    if (!ssl->options.dtls) {
#ifdef HAVE_FUZZER
        if (ssl->fuzzerCb)
            ssl->fuzzerCb(ssl, input + *inOutIdx, RECORD_HEADER_SZ, FUZZ_HEAD,
                    ssl->fuzzerCtx);
#endif
        XMEMCPY(rh, input + *inOutIdx, RECORD_HEADER_SZ);
        *inOutIdx += RECORD_HEADER_SZ;
        ato16(rh->length, size);
    }
    else {
#ifdef WOLFSSL_DTLS
#ifdef HAVE_FUZZER
        if (ssl->fuzzerCb)
            ssl->fuzzerCb(ssl, input + *inOutIdx, DTLS_RECORD_HEADER_SZ,
                           FUZZ_HEAD, ssl->fuzzerCtx);
#endif
        /* type and version in same sport */
        XMEMCPY(rh, input + *inOutIdx, ENUM_LEN + VERSION_SZ);
        *inOutIdx += ENUM_LEN + VERSION_SZ;
        ato16(input + *inOutIdx, &ssl->keys.curEpoch);
        *inOutIdx += OPAQUE16_LEN;
        if (ssl->options.haveMcast) {
        #ifdef WOLFSSL_MULTICAST
            ssl->keys.curPeerId = input[*inOutIdx];
            ssl->keys.curSeq_hi = input[*inOutIdx+1];
        #endif
        }
        else
            ato16(input + *inOutIdx, &ssl->keys.curSeq_hi);
        *inOutIdx += OPAQUE16_LEN;
        ato32(input + *inOutIdx, &ssl->keys.curSeq_lo);
        *inOutIdx += OPAQUE32_LEN;  /* advance past rest of seq */
        ato16(input + *inOutIdx, size);
        *inOutIdx += LENGTH_SZ;
#endif
    }

#ifdef WOLFSSL_DTLS
    if (IsDtlsNotSctpMode(ssl) &&
        (!DtlsCheckWindow(ssl) ||
         (ssl->options.handShakeDone && ssl->keys.curEpoch == 0))) {
            return SEQUENCE_ERROR;
    }
#endif

    /* catch version mismatch */
#ifndef WOLFSSL_TLS13
    if (rh->pvMajor != ssl->version.major || rh->pvMinor != ssl->version.minor)
#else
    if (rh->pvMajor != ssl->version.major ||
        (rh->pvMinor != ssl->version.minor &&
         (!IsAtLeastTLSv1_3(ssl->version) || rh->pvMinor != TLSv1_MINOR)))
#endif
    {
        if (ssl->options.side == WOLFSSL_SERVER_END &&
            ssl->options.acceptState < ACCEPT_FIRST_REPLY_DONE)

            WOLFSSL_MSG("Client attempting to connect with different version");
        else if (ssl->options.side == WOLFSSL_CLIENT_END &&
                                 ssl->options.downgrade &&
                                 ssl->options.connectState < FIRST_REPLY_DONE)
            WOLFSSL_MSG("Server attempting to accept with different version");
        else if (ssl->options.dtls && rh->type == handshake)
            /* Check the DTLS handshake message RH version later. */
            WOLFSSL_MSG("DTLS handshake, skip RH version number check");
        else {
            WOLFSSL_MSG("SSL version error");
            return VERSION_ERROR;              /* only use requested version */
        }
    }

    /* record layer length check */
#ifdef HAVE_MAX_FRAGMENT
    if (*size > (ssl->max_fragment + MAX_COMP_EXTRA + MAX_MSG_EXTRA)) {
        SendAlert(ssl, alert_fatal, record_overflow);
        return LENGTH_ERROR;
    }
#else
    if (*size > (MAX_RECORD_SIZE + MAX_COMP_EXTRA + MAX_MSG_EXTRA))
        return LENGTH_ERROR;
#endif

    /* verify record type here as well */
    switch (rh->type) {
        case handshake:
        case change_cipher_spec:
        case application_data:
        case alert:
            break;
        case no_type:
        default:
            WOLFSSL_MSG("Unknown Record Type");
            return UNKNOWN_RECORD_TYPE;
    }

    /* haven't decrypted this record yet */
    ssl->keys.decryptedCur = 0;

    return 0;
}


static int GetHandShakeHeader(WOLFSSL* ssl, const byte* input, word32* inOutIdx,
                              byte *type, word32 *size, word32 totalSz)
{
    const byte *ptr = input + *inOutIdx;
    (void)ssl;

    *inOutIdx += HANDSHAKE_HEADER_SZ;
    if (*inOutIdx > totalSz)
        return BUFFER_E;

    *type = ptr[0];
    c24to32(&ptr[1], size);

    return 0;
}


#ifdef WOLFSSL_DTLS
static int GetDtlsHandShakeHeader(WOLFSSL* ssl, const byte* input,
                                  word32* inOutIdx, byte *type, word32 *size,
                                  word32 *fragOffset, word32 *fragSz,
                                  word32 totalSz)
{
    word32 idx = *inOutIdx;

    *inOutIdx += HANDSHAKE_HEADER_SZ + DTLS_HANDSHAKE_EXTRA;
    if (*inOutIdx > totalSz)
        return BUFFER_E;

    *type = input[idx++];
    c24to32(input + idx, size);
    idx += OPAQUE24_LEN;

    ato16(input + idx, &ssl->keys.dtls_peer_handshake_number);
    idx += DTLS_HANDSHAKE_SEQ_SZ;

    c24to32(input + idx, fragOffset);
    idx += DTLS_HANDSHAKE_FRAG_SZ;
    c24to32(input + idx, fragSz);

    if (ssl->curRL.pvMajor != ssl->version.major ||
        ssl->curRL.pvMinor != ssl->version.minor) {

        if (*type != client_hello && *type != hello_verify_request)
            return VERSION_ERROR;
        else {
            WOLFSSL_MSG("DTLS Handshake ignoring hello or verify version");
        }
    }
    return 0;
}
#endif


#if !defined(NO_OLD_TLS) || \
    (defined(NO_OLD_TLS) && defined(WOLFSSL_ALLOW_TLS_SHA1))
/* fill with MD5 pad size since biggest required */
static const byte PAD1[PAD_MD5] =
                              { 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
                                0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
                                0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
                                0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
                                0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36,
                                0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36, 0x36
                              };
static const byte PAD2[PAD_MD5] =
                              { 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
                                0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
                                0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
                                0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
                                0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c,
                                0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c, 0x5c
                              };
#endif /* !NO_OLD_TLS || (NO_OLD_TLS && WOLFSSL_ALLOW_TLS_SHA1) */

#ifndef NO_OLD_TLS

/* calculate MD5 hash for finished */
#ifdef WOLFSSL_TI_HASH
#include <wolfssl/wolfcrypt/hash.h>
#endif

static int BuildMD5(WOLFSSL* ssl, Hashes* hashes, const byte* sender)
{
    int ret;
    byte md5_result[MD5_DIGEST_SIZE];
#ifdef WOLFSSL_SMALL_STACK
    Md5* md5 = (Md5*)XMALLOC(sizeof(Md5), ssl->heap, DYNAMIC_TYPE_HASHCTX);
    if (md5 == NULL)
        return MEMORY_E;
#else
    Md5  md5[1];
#endif

    /* make md5 inner */
    ret = wc_Md5Copy(&ssl->hsHashes->hashMd5, md5);
    if (ret == 0)
        ret = wc_Md5Update(md5, sender, SIZEOF_SENDER);
    if (ret == 0)
        ret = wc_Md5Update(md5, ssl->arrays->masterSecret,SECRET_LEN);
    if (ret == 0)
        ret = wc_Md5Update(md5, PAD1, PAD_MD5);
    if (ret == 0)
        ret = wc_Md5Final(md5, md5_result);

    /* make md5 outer */
    if (ret == 0) {
        ret = wc_InitMd5_ex(md5, ssl->heap, ssl->devId);
        if (ret == 0) {
            ret = wc_Md5Update(md5, ssl->arrays->masterSecret,SECRET_LEN);
            if (ret == 0)
                ret = wc_Md5Update(md5, PAD2, PAD_MD5);
            if (ret == 0)
                ret = wc_Md5Update(md5, md5_result, MD5_DIGEST_SIZE);
            if (ret == 0)
                ret = wc_Md5Final(md5, hashes->md5);
            wc_Md5Free(md5);
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(md5, ssl->heap, DYNAMIC_TYPE_HASHCTX);
#endif

    return ret;
}


/* calculate SHA hash for finished */
static int BuildSHA(WOLFSSL* ssl, Hashes* hashes, const byte* sender)
{
    int ret;
    byte sha_result[SHA_DIGEST_SIZE];
#ifdef WOLFSSL_SMALL_STACK
    Sha* sha = (Sha*)XMALLOC(sizeof(Sha), ssl->heap, DYNAMIC_TYPE_HASHCTX);
    if (sha == NULL)
        return MEMORY_E;
#else
    Sha  sha[1];
#endif
    /* make sha inner */
    ret = wc_ShaCopy(&ssl->hsHashes->hashSha, sha); /* Save current position */
    if (ret == 0)
        ret = wc_ShaUpdate(sha, sender, SIZEOF_SENDER);
    if (ret == 0)
        ret = wc_ShaUpdate(sha, ssl->arrays->masterSecret,SECRET_LEN);
    if (ret == 0)
        ret = wc_ShaUpdate(sha, PAD1, PAD_SHA);
    if (ret == 0)
        ret = wc_ShaFinal(sha, sha_result);

    /* make sha outer */
    if (ret == 0) {
        ret = wc_InitSha_ex(sha, ssl->heap, ssl->devId);
        if (ret == 0) {
            ret = wc_ShaUpdate(sha, ssl->arrays->masterSecret,SECRET_LEN);
            if (ret == 0)
                ret = wc_ShaUpdate(sha, PAD2, PAD_SHA);
            if (ret == 0)
                ret = wc_ShaUpdate(sha, sha_result, SHA_DIGEST_SIZE);
            if (ret == 0)
                ret = wc_ShaFinal(sha, hashes->sha);
            wc_ShaFree(sha);
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(sha, ssl->heap, DYNAMIC_TYPE_HASHCTX);
#endif

    return ret;
}
#endif

/* Finished doesn't support SHA512, not SHA512 cipher suites yet */
static int BuildFinished(WOLFSSL* ssl, Hashes* hashes, const byte* sender)
{
    int ret = 0;
#ifdef WOLFSSL_SHA384
#ifdef WOLFSSL_SMALL_STACK
    Sha384* sha384;
#else
    Sha384 sha384[1];
#endif /* WOLFSSL_SMALL_STACK */
#endif /* WOLFSSL_SHA384 */

    if (ssl == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_SHA384
#ifdef WOLFSSL_SMALL_STACK
    sha384 = (Sha384*)XMALLOC(sizeof(Sha384), ssl->heap, DYNAMIC_TYPE_HASHCTX);
    if (sha384 == NULL)
        return MEMORY_E;
#endif /* WOLFSSL_SMALL_STACK */
#endif /* WOLFSSL_SHA384 */

    /* store current states, building requires get_digest which resets state */
#ifdef WOLFSSL_SHA384
    sha384[0] = ssl->hsHashes->hashSha384;
#endif

#ifndef NO_TLS
    if (ssl->options.tls) {
        ret = BuildTlsFinished(ssl, hashes, sender);
    }
#endif
#ifndef NO_OLD_TLS
    if (!ssl->options.tls) {
        ret = BuildMD5(ssl, hashes, sender);
        if (ret == 0) {
            ret = BuildSHA(ssl, hashes, sender);
        }
    }
#endif

    /* restore */
    if (IsAtLeastTLSv1_2(ssl)) {
    #ifdef WOLFSSL_SHA384
        ssl->hsHashes->hashSha384 = sha384[0];
    #endif
    }

#ifdef WOLFSSL_SHA384
#ifdef WOLFSSL_SMALL_STACK
    XFREE(sha384, ssl->heap, DYNAMIC_TYPE_HASHCTX);
#endif
#endif

    return ret;
}


    /* cipher requirements */
    enum {
        REQUIRES_RSA,
        REQUIRES_DHE,
        REQUIRES_ECC,
        REQUIRES_ECC_STATIC,
        REQUIRES_PSK,
        REQUIRES_NTRU,
        REQUIRES_RSA_SIG
    };



    /* Does this cipher suite (first, second) have the requirement
       an ephemeral key exchange will still require the key for signing
       the key exchange so ECHDE_RSA requires an rsa key thus rsa_kea */
    static int CipherRequires(byte first, byte second, int requirement)
    {

        if (first == CHACHA_BYTE) {

        switch (second) {

        case TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256 :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256 :
            if (requirement == REQUIRES_ECC)
                return 1;
            break;

        case TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256 :
            if (requirement == REQUIRES_RSA)
                return 1;
            if (requirement == REQUIRES_DHE)
                return 1;
            break;

        case TLS_ECDHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256 :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_ECDHE_ECDSA_WITH_CHACHA20_OLD_POLY1305_SHA256 :
            if (requirement == REQUIRES_ECC)
                return 1;
            break;

        case TLS_DHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256 :
            if (requirement == REQUIRES_RSA)
                return 1;
            if (requirement == REQUIRES_DHE)
                return 1;
            break;


        case TLS_ECDHE_PSK_WITH_CHACHA20_POLY1305_SHA256 :
            if (requirement == REQUIRES_PSK)
                return 1;
            break;

        case TLS_PSK_WITH_CHACHA20_POLY1305_SHA256 :
            if (requirement == REQUIRES_PSK)
                return 1;
            break;

        case TLS_DHE_PSK_WITH_CHACHA20_POLY1305_SHA256 :
            if (requirement == REQUIRES_PSK)
                return 1;
            if (requirement == REQUIRES_DHE)
                return 1;
            break;
            }
        }

        /* ECC extensions */
        if (first == ECC_BYTE) {

        switch (second) {

#ifndef NO_RSA
        case TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_ECDH_RSA_WITH_AES_128_CBC_SHA :
            if (requirement == REQUIRES_ECC_STATIC)
                return 1;
            if (requirement == REQUIRES_RSA_SIG)
                return 1;
            break;

#ifndef NO_DES3
        case TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA :
            if (requirement == REQUIRES_ECC_STATIC)
                return 1;
            if (requirement == REQUIRES_RSA_SIG)
                return 1;
            break;
#endif

#ifndef NO_RC4
        case TLS_ECDHE_RSA_WITH_RC4_128_SHA :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_ECDH_RSA_WITH_RC4_128_SHA :
            if (requirement == REQUIRES_ECC_STATIC)
                return 1;
            if (requirement == REQUIRES_RSA_SIG)
                return 1;
            break;
#endif
#endif /* NO_RSA */

#ifndef NO_DES3
        case TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA :
            if (requirement == REQUIRES_ECC)
                return 1;
            break;

        case TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA :
            if (requirement == REQUIRES_ECC_STATIC)
                return 1;
            break;
#endif
#ifndef NO_RC4
        case TLS_ECDHE_ECDSA_WITH_RC4_128_SHA :
            if (requirement == REQUIRES_ECC)
                return 1;
            break;

        case TLS_ECDH_ECDSA_WITH_RC4_128_SHA :
            if (requirement == REQUIRES_ECC_STATIC)
                return 1;
            break;
#endif
#ifndef NO_RSA
        case TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_ECDH_RSA_WITH_AES_256_CBC_SHA :
            if (requirement == REQUIRES_ECC_STATIC)
                return 1;
            if (requirement == REQUIRES_RSA_SIG)
                return 1;
            break;
#endif

        case TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA :
            if (requirement == REQUIRES_ECC)
                return 1;
            break;

        case TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA :
            if (requirement == REQUIRES_ECC_STATIC)
                return 1;
            break;

        case TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA :
            if (requirement == REQUIRES_ECC)
                return 1;
            break;

        case TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA :
            if (requirement == REQUIRES_ECC_STATIC)
                return 1;
            break;

        case TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 :
            if (requirement == REQUIRES_ECC)
                return 1;
            break;

        case TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384 :
            if (requirement == REQUIRES_ECC)
                return 1;
            break;

        case TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256 :
            if (requirement == REQUIRES_ECC_STATIC)
                return 1;
            break;

        case TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384 :
            if (requirement == REQUIRES_ECC_STATIC)
                return 1;
            break;

#ifndef NO_RSA
        case TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256 :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384 :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256 :
            if (requirement == REQUIRES_ECC_STATIC)
                return 1;
            if (requirement == REQUIRES_RSA_SIG)
                return 1;
            break;

        case TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384 :
            if (requirement == REQUIRES_ECC_STATIC)
                return 1;
            if (requirement == REQUIRES_RSA_SIG)
                return 1;
            break;

        case TLS_RSA_WITH_AES_128_CCM_8 :
        case TLS_RSA_WITH_AES_256_CCM_8 :
            if (requirement == REQUIRES_RSA)
                return 1;
            if (requirement == REQUIRES_RSA_SIG)
                return 1;
            break;

        case TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256 :
        case TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384 :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256 :
        case TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384 :
            if (requirement == REQUIRES_RSA_SIG)
                return 1;
            if (requirement == REQUIRES_ECC_STATIC)
                return 1;
            break;
#endif

        case TLS_ECDHE_ECDSA_WITH_AES_128_CCM :
        case TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8 :
        case TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8 :
            if (requirement == REQUIRES_ECC)
                return 1;
            break;

        case TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384 :
        case TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 :
            if (requirement == REQUIRES_ECC)
                return 1;
            break;

        case TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256 :
        case TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384 :
            if (requirement == REQUIRES_ECC)
                return 1;
            if (requirement == REQUIRES_ECC_STATIC)
                return 1;
            break;

        case TLS_PSK_WITH_AES_128_CCM:
        case TLS_PSK_WITH_AES_256_CCM:
        case TLS_PSK_WITH_AES_128_CCM_8:
        case TLS_PSK_WITH_AES_256_CCM_8:
            if (requirement == REQUIRES_PSK)
                return 1;
            break;

        case TLS_DHE_PSK_WITH_AES_128_CCM:
        case TLS_DHE_PSK_WITH_AES_256_CCM:
            if (requirement == REQUIRES_PSK)
                return 1;
            if (requirement == REQUIRES_DHE)
                return 1;
            break;

        case TLS_ECDHE_ECDSA_WITH_NULL_SHA :
            if (requirement == REQUIRES_ECC)
                return 1;
            break;

        case TLS_ECDHE_PSK_WITH_NULL_SHA256 :
            if (requirement == REQUIRES_PSK)
                return 1;
            break;

        case TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256 :
            if (requirement == REQUIRES_PSK)
                return 1;
            break;

        default:
            WOLFSSL_MSG("Unsupported cipher suite, CipherRequires ECC");
            return 0;
        }   /* switch */
        }   /* if     */

        /* Distinct TLS v1.3 cipher suites with cipher and digest only. */
        if (first == TLS13_BYTE) {

            switch (second) {
#ifdef WOLFSSL_TLS13
            case TLS_AES_128_GCM_SHA256:
            case TLS_AES_256_GCM_SHA384:
            case TLS_CHACHA20_POLY1305_SHA256:
            case TLS_AES_128_CCM_SHA256:
            case TLS_AES_128_CCM_8_SHA256:
                break;
#endif

            default:
                WOLFSSL_MSG("Unsupported cipher suite, CipherRequires "
                            "TLS v1.3");
                return 0;
            }
        }

        if (first != ECC_BYTE && first != CHACHA_BYTE &&
            first != TLS13_BYTE) {   /* normal suites */
        switch (second) {

#ifndef NO_RSA
        case SSL_RSA_WITH_RC4_128_SHA :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case SSL_RSA_WITH_RC4_128_MD5 :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case SSL_RSA_WITH_3DES_EDE_CBC_SHA :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_NTRU_RSA_WITH_RC4_128_SHA :
            if (requirement == REQUIRES_NTRU)
                return 1;
            break;

        case TLS_RSA_WITH_AES_128_CBC_SHA :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_RSA_WITH_AES_128_CBC_SHA256 :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_NTRU_RSA_WITH_3DES_EDE_CBC_SHA :
            if (requirement == REQUIRES_NTRU)
                return 1;
            break;

        case TLS_RSA_WITH_AES_256_CBC_SHA :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_NTRU_RSA_WITH_AES_128_CBC_SHA :
            if (requirement == REQUIRES_NTRU)
                return 1;
            break;

        case TLS_RSA_WITH_AES_256_CBC_SHA256 :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_RSA_WITH_NULL_SHA :
        case TLS_RSA_WITH_NULL_SHA256 :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_NTRU_RSA_WITH_AES_256_CBC_SHA :
            if (requirement == REQUIRES_NTRU)
                return 1;
            break;

        case SSL_RSA_WITH_IDEA_CBC_SHA :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;
#endif

        case TLS_PSK_WITH_AES_128_GCM_SHA256 :
        case TLS_PSK_WITH_AES_256_GCM_SHA384 :
        case TLS_PSK_WITH_AES_128_CBC_SHA256 :
        case TLS_PSK_WITH_AES_256_CBC_SHA384 :
        case TLS_PSK_WITH_AES_128_CBC_SHA :
        case TLS_PSK_WITH_AES_256_CBC_SHA :
        case TLS_PSK_WITH_NULL_SHA384 :
        case TLS_PSK_WITH_NULL_SHA256 :
        case TLS_PSK_WITH_NULL_SHA :
            if (requirement == REQUIRES_PSK)
                return 1;
            break;

        case TLS_DHE_PSK_WITH_AES_128_GCM_SHA256 :
        case TLS_DHE_PSK_WITH_AES_256_GCM_SHA384 :
        case TLS_DHE_PSK_WITH_AES_128_CBC_SHA256 :
        case TLS_DHE_PSK_WITH_AES_256_CBC_SHA384 :
        case TLS_DHE_PSK_WITH_NULL_SHA384 :
        case TLS_DHE_PSK_WITH_NULL_SHA256 :
            if (requirement == REQUIRES_DHE)
                return 1;
            if (requirement == REQUIRES_PSK)
                return 1;
            break;

#ifndef NO_RSA
        case TLS_DHE_RSA_WITH_AES_128_CBC_SHA256 :
            if (requirement == REQUIRES_RSA)
                return 1;
            if (requirement == REQUIRES_DHE)
                return 1;
            break;

        case TLS_DHE_RSA_WITH_AES_256_CBC_SHA256 :
            if (requirement == REQUIRES_RSA)
                return 1;
            if (requirement == REQUIRES_DHE)
                return 1;
            break;

        case TLS_DHE_RSA_WITH_AES_128_CBC_SHA :
            if (requirement == REQUIRES_RSA)
                return 1;
            if (requirement == REQUIRES_DHE)
                return 1;
            break;

        case TLS_DHE_RSA_WITH_AES_256_CBC_SHA :
            if (requirement == REQUIRES_RSA)
                return 1;
            if (requirement == REQUIRES_DHE)
                return 1;
            break;

        case TLS_RSA_WITH_HC_128_MD5 :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_RSA_WITH_HC_128_SHA :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_RSA_WITH_HC_128_B2B256:
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_RSA_WITH_AES_128_CBC_B2B256:
        case TLS_RSA_WITH_AES_256_CBC_B2B256:
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_RSA_WITH_RABBIT_SHA :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_RSA_WITH_AES_128_GCM_SHA256 :
        case TLS_RSA_WITH_AES_256_GCM_SHA384 :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_DHE_RSA_WITH_AES_128_GCM_SHA256 :
        case TLS_DHE_RSA_WITH_AES_256_GCM_SHA384 :
            if (requirement == REQUIRES_RSA)
                return 1;
            if (requirement == REQUIRES_DHE)
                return 1;
            break;

        case TLS_RSA_WITH_CAMELLIA_128_CBC_SHA :
        case TLS_RSA_WITH_CAMELLIA_256_CBC_SHA :
        case TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256 :
        case TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256 :
            if (requirement == REQUIRES_RSA)
                return 1;
            break;

        case TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA :
        case TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA :
        case TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256 :
        case TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256 :
            if (requirement == REQUIRES_RSA)
                return 1;
            if (requirement == REQUIRES_RSA_SIG)
                return 1;
            if (requirement == REQUIRES_DHE)
                return 1;
            break;

        case TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA:
            if (requirement == REQUIRES_RSA)
                return 1;
            if (requirement == REQUIRES_RSA_SIG)
                return 1;
            if (requirement == REQUIRES_DHE)
                return 1;
            break;
#endif
#ifdef HAVE_ANON
        case TLS_DH_anon_WITH_AES_128_CBC_SHA :
            if (requirement == REQUIRES_DHE)
                return 1;
            break;
#endif
#ifdef WOLFSSL_MULTICAST
        case WDM_WITH_NULL_SHA256 :
            break;
#endif

        default:
            WOLFSSL_MSG("Unsupported cipher suite, CipherRequires");
            return 0;
        }  /* switch */
        }  /* if ECC / Normal suites else */

        return 0;
    }


#ifndef NO_CERTS


/* Match names with wildcards, each wildcard can represent a single name
   component or fragment but not mulitple names, i.e.,
   *.z.com matches y.z.com but not x.y.z.com

   return 1 on success */
int MatchDomainName(const char* pattern, int len, const char* str)
{
    char p, s;

    if (pattern == NULL || str == NULL || len <= 0)
        return 0;

    while (len > 0) {

        p = (char)XTOLOWER((unsigned char)*pattern++);
        if (p == 0)
            break;

        if (p == '*') {
            while (--len > 0 &&
                (p = (char)XTOLOWER((unsigned char)*pattern++)) == '*') {
            }

            if (len == 0)
                p = '\0';

            while ( (s = (char)XTOLOWER((unsigned char) *str)) != '\0') {
                if (s == p)
                    break;
                if (s == '.')
                    return 0;
                str++;
            }
        }
        else {
            if (p != (char)XTOLOWER((unsigned char) *str))
                return 0;
        }

        if (*str != '\0')
            str++;

        if (len > 0)
            len--;
    }

    return *str == '\0';
}


/* try to find an altName match to domain, return 1 on success */
int CheckAltNames(DecodedCert* dCert, char* domain)
{
    int        match = 0;
    DNS_entry* altName = NULL;

    WOLFSSL_MSG("Checking AltNames");

    if (dCert)
        altName = dCert->altNames;

    while (altName) {
        WOLFSSL_MSG("\tindividual AltName check");

        if (MatchDomainName(altName->name,(int)XSTRLEN(altName->name), domain)){
            match = 1;
            break;
        }

        altName = altName->next;
    }

    return match;
}


#ifdef OPENSSL_EXTRA
/* Check that alternative names, if they exists, match the domain.
 * Fail if there are wild patterns and they didn't match.
 * Check the common name if no alternative names matched.
 *
 * dCert    Decoded cert to get the alternative names from.
 * domain   Domain name to compare against.
 * checkCN  Whether to check the common name.
 * returns whether there was a problem in matching.
 */
static int CheckForAltNames(DecodedCert* dCert, char* domain, int* checkCN)
{
    int        match;
    DNS_entry* altName = NULL;

    WOLFSSL_MSG("Checking AltNames");

    if (dCert)
        altName = dCert->altNames;

    *checkCN = altName == NULL;
    match = 0;
    while (altName) {
        WOLFSSL_MSG("\tindividual AltName check");

        if (MatchDomainName(altName->name, (int)XSTRLEN(altName->name),
                            domain)) {
            match = 1;
            *checkCN = 0;
            break;
        }
        /* No matches and wild pattern match failed. */
        else if (altName->name[0] == '*' && match == 0)
            match = -1;

        altName = altName->next;
    }

    return match != -1;
}

/* Check the domain name matches the subject alternative name or the subject
 * name.
 *
 * dcert          Decoded certificate.
 * domainName     The domain name.
 * domainNameLen  The length of the domain name.
 * returns DOMAIN_NAME_MISMATCH when no match found and 0 on success.
 */
int CheckHostName(DecodedCert* dCert, char *domainName, size_t domainNameLen)
{
    int checkCN;

    /* Assume name is NUL terminated. */
    (void)domainNameLen;

    if (CheckForAltNames(dCert, domainName, &checkCN) == 0) {
        WOLFSSL_MSG("DomainName match on alt names failed too");
        return DOMAIN_NAME_MISMATCH;
    }
    if (checkCN == 1) {
        if (MatchDomainName(dCert->subjectCN, dCert->subjectCNLen,
                            domainName) == 0) {
            WOLFSSL_MSG("DomainName match on common name failed");
            return DOMAIN_NAME_MISMATCH;
        }
    }

    return 0;
}
#endif

#if defined(KEEP_PEER_CERT) || defined(SESSION_CERTS)

/* Copy parts X509 needs from Decoded cert, 0 on success */
int CopyDecodedToX509(WOLFSSL_X509* x509, DecodedCert* dCert)
{
    int ret = 0;

    if (x509 == NULL || dCert == NULL ||
        dCert->subjectCNLen < 0)
        return BAD_FUNC_ARG;

    x509->version = dCert->version + 1;

    XSTRNCPY(x509->issuer.name, dCert->issuer, ASN_NAME_MAX);
    x509->issuer.name[ASN_NAME_MAX - 1] = '\0';
    x509->issuer.sz = (int)XSTRLEN(x509->issuer.name) + 1;
#ifdef OPENSSL_EXTRA
    if (dCert->issuerName.fullName != NULL) {
        XMEMCPY(&x509->issuer.fullName,
                                       &dCert->issuerName, sizeof(DecodedName));
        x509->issuer.fullName.fullName = (char*)XMALLOC(
                        dCert->issuerName.fullNameLen, x509->heap,
                        DYNAMIC_TYPE_X509);
        if (x509->issuer.fullName.fullName != NULL)
            XMEMCPY(x509->issuer.fullName.fullName,
                     dCert->issuerName.fullName, dCert->issuerName.fullNameLen);
    }
    x509->issuer.x509 = x509;
#endif /* OPENSSL_EXTRA */

    XSTRNCPY(x509->subject.name, dCert->subject, ASN_NAME_MAX);
    x509->subject.name[ASN_NAME_MAX - 1] = '\0';
    x509->subject.sz = (int)XSTRLEN(x509->subject.name) + 1;
#ifdef OPENSSL_EXTRA
    if (dCert->subjectName.fullName != NULL) {
        XMEMCPY(&x509->subject.fullName,
                                      &dCert->subjectName, sizeof(DecodedName));
        x509->subject.fullName.fullName = (char*)XMALLOC(
                 dCert->subjectName.fullNameLen, x509->heap, DYNAMIC_TYPE_X509);
        if (x509->subject.fullName.fullName != NULL)
            XMEMCPY(x509->subject.fullName.fullName,
                   dCert->subjectName.fullName, dCert->subjectName.fullNameLen);
    }
    x509->subject.x509 = x509;
#endif /* OPENSSL_EXTRA */
#ifdef WOLFSSL_NGINX
    XMEMCPY(x509->subject.raw, dCert->subjectRaw, dCert->subjectRawLen);
    x509->subject.rawLen = dCert->subjectRawLen;
#endif

    XMEMCPY(x509->serial, dCert->serial, EXTERNAL_SERIAL_SIZE);
    x509->serialSz = dCert->serialSz;
    if (dCert->subjectCNLen < ASN_NAME_MAX) {
        XMEMCPY(x509->subjectCN, dCert->subjectCN, dCert->subjectCNLen);
        x509->subjectCN[dCert->subjectCNLen] = '\0';
    }
    else
        x509->subjectCN[0] = '\0';

#ifdef WOLFSSL_SEP
    {
        int minSz = min(dCert->deviceTypeSz, EXTERNAL_SERIAL_SIZE);
        if (minSz > 0) {
            x509->deviceTypeSz = minSz;
            XMEMCPY(x509->deviceType, dCert->deviceType, minSz);
        }
        else
            x509->deviceTypeSz = 0;
        minSz = min(dCert->hwTypeSz, EXTERNAL_SERIAL_SIZE);
        if (minSz > 0) {
            x509->hwTypeSz = minSz;
            XMEMCPY(x509->hwType, dCert->hwType, minSz);
        }
        else
            x509->hwTypeSz = 0;
        minSz = min(dCert->hwSerialNumSz, EXTERNAL_SERIAL_SIZE);
        if (minSz > 0) {
            x509->hwSerialNumSz = minSz;
            XMEMCPY(x509->hwSerialNum, dCert->hwSerialNum, minSz);
        }
        else
            x509->hwSerialNumSz = 0;
    }
#endif /* WOLFSSL_SEP */
    {
        int minSz = min(dCert->beforeDateLen, MAX_DATE_SZ);
        if (minSz > 0) {
            x509->notBeforeSz = minSz;
            XMEMCPY(x509->notBefore, dCert->beforeDate, minSz);
        }
        else
            x509->notBeforeSz = 0;
        minSz = min(dCert->afterDateLen, MAX_DATE_SZ);
        if (minSz > 0) {
            x509->notAfterSz = minSz;
            XMEMCPY(x509->notAfter, dCert->afterDate, minSz);
        }
        else
            x509->notAfterSz = 0;
    }

    if (dCert->publicKey != NULL && dCert->pubKeySize != 0) {
        x509->pubKey.buffer = (byte*)XMALLOC(
                        dCert->pubKeySize, x509->heap, DYNAMIC_TYPE_PUBLIC_KEY);
        if (x509->pubKey.buffer != NULL) {
            x509->pubKeyOID = dCert->keyOID;
            x509->pubKey.length = dCert->pubKeySize;
            XMEMCPY(x509->pubKey.buffer, dCert->publicKey, dCert->pubKeySize);
        }
        else
            ret = MEMORY_E;
    }

    if (dCert->signature != NULL && dCert->sigLength != 0 &&
            dCert->sigLength <= MAX_ENCODED_SIG_SZ) {
        x509->sig.buffer = (byte*)XMALLOC(
                          dCert->sigLength, x509->heap, DYNAMIC_TYPE_SIGNATURE);
        if (x509->sig.buffer == NULL) {
            ret = MEMORY_E;
        }
        else {
            XMEMCPY(x509->sig.buffer, dCert->signature, dCert->sigLength);
            x509->sig.length = dCert->sigLength;
            x509->sigOID = dCert->signatureOID;
        }
    }

    /* store cert for potential retrieval */
    if (AllocDer(&x509->derCert, dCert->maxIdx, CERT_TYPE, x509->heap) == 0) {
        XMEMCPY(x509->derCert->buffer, dCert->source, dCert->maxIdx);
    }
    else {
        ret = MEMORY_E;
    }

    x509->altNames       = dCert->altNames;
    dCert->weOwnAltNames = 0;
    x509->altNamesNext   = x509->altNames;  /* index hint */

    x509->isCa = dCert->isCA;
#ifdef OPENSSL_EXTRA
    x509->pathLength = dCert->pathLength;
    x509->keyUsage = dCert->extKeyUsage;

    x509->CRLdistSet = dCert->extCRLdistSet;
    x509->CRLdistCrit = dCert->extCRLdistCrit;
    x509->CRLInfo = dCert->extCrlInfo;
    x509->CRLInfoSz = dCert->extCrlInfoSz;
    x509->authInfoSet = dCert->extAuthInfoSet;
    x509->authInfoCrit = dCert->extAuthInfoCrit;
    if (dCert->extAuthInfo != NULL && dCert->extAuthInfoSz > 0) {
        x509->authInfo = (byte*)XMALLOC(dCert->extAuthInfoSz, x509->heap,
                DYNAMIC_TYPE_X509_EXT);
        if (x509->authInfo != NULL) {
            XMEMCPY(x509->authInfo, dCert->extAuthInfo, dCert->extAuthInfoSz);
            x509->authInfoSz = dCert->extAuthInfoSz;
        }
        else {
            ret = MEMORY_E;
        }
    }
    x509->basicConstSet = dCert->extBasicConstSet;
    x509->basicConstCrit = dCert->extBasicConstCrit;
    x509->basicConstPlSet = dCert->pathLengthSet;
    x509->subjAltNameSet = dCert->extSubjAltNameSet;
    x509->subjAltNameCrit = dCert->extSubjAltNameCrit;
    x509->authKeyIdSet = dCert->extAuthKeyIdSet;
    x509->authKeyIdCrit = dCert->extAuthKeyIdCrit;
    if (dCert->extAuthKeyIdSrc != NULL && dCert->extAuthKeyIdSz != 0) {
        x509->authKeyId = (byte*)XMALLOC(dCert->extAuthKeyIdSz, x509->heap,
                                         DYNAMIC_TYPE_X509_EXT);
        if (x509->authKeyId != NULL) {
            XMEMCPY(x509->authKeyId,
                                 dCert->extAuthKeyIdSrc, dCert->extAuthKeyIdSz);
            x509->authKeyIdSz = dCert->extAuthKeyIdSz;
        }
        else
            ret = MEMORY_E;
    }
    x509->subjKeyIdSet = dCert->extSubjKeyIdSet;
    x509->subjKeyIdCrit = dCert->extSubjKeyIdCrit;
    if (dCert->extSubjKeyIdSrc != NULL && dCert->extSubjKeyIdSz != 0) {
        x509->subjKeyId = (byte*)XMALLOC(dCert->extSubjKeyIdSz, x509->heap,
                                         DYNAMIC_TYPE_X509_EXT);
        if (x509->subjKeyId != NULL) {
            XMEMCPY(x509->subjKeyId,
                                 dCert->extSubjKeyIdSrc, dCert->extSubjKeyIdSz);
            x509->subjKeyIdSz = dCert->extSubjKeyIdSz;
        }
        else
            ret = MEMORY_E;
    }
    x509->keyUsageSet = dCert->extKeyUsageSet;
    x509->keyUsageCrit = dCert->extKeyUsageCrit;
    if (dCert->extExtKeyUsageSrc != NULL && dCert->extExtKeyUsageSz > 0) {
        x509->extKeyUsageSrc = (byte*)XMALLOC(dCert->extExtKeyUsageSz,
                x509->heap, DYNAMIC_TYPE_X509_EXT);
        if (x509->extKeyUsageSrc != NULL) {
            XMEMCPY(x509->extKeyUsageSrc, dCert->extExtKeyUsageSrc,
                                                       dCert->extExtKeyUsageSz);
            x509->extKeyUsageSz    = dCert->extExtKeyUsageSz;
            x509->extKeyUsageCrit  = dCert->extExtKeyUsageCrit;
            x509->extKeyUsageCount = dCert->extExtKeyUsageCount;
        }
        else {
            ret = MEMORY_E;
        }
    }
    #ifdef WOLFSSL_SEP
        x509->certPolicySet = dCert->extCertPolicySet;
        x509->certPolicyCrit = dCert->extCertPolicyCrit;
    #endif /* WOLFSSL_SEP */
    #ifdef WOLFSSL_CERT_EXT
        {
            int i;
            for (i = 0; i < dCert->extCertPoliciesNb && i < MAX_CERTPOL_NB; i++)
                XMEMCPY(x509->certPolicies[i], dCert->extCertPolicies[i],
                                                                MAX_CERTPOL_SZ);
            x509->certPoliciesNb = dCert->extCertPoliciesNb;
        }
    #endif /* WOLFSSL_CERT_EXT */
#endif /* OPENSSL_EXTRA */
#ifdef HAVE_ECC
    x509->pkCurveOID = dCert->pkCurveOID;
#endif /* HAVE_ECC */

    return ret;
}

#endif /* KEEP_PEER_CERT || SESSION_CERTS */

typedef struct ProcPeerCertArgs {
    buffer*      certs;
#ifdef WOLFSSL_TLS13
    buffer*      exts; /* extentions */
#endif
    DecodedCert* dCert;
    char*  domain;
    word32 idx;
    word32 begin;
    int    totalCerts; /* number of certs in certs buffer */
    int    count;
    int    dCertInit;
    int    certIdx;
    int    fatal;
    int    lastErr;
#ifdef WOLFSSL_TLS13
    byte   ctxSz;
#endif
#ifdef WOLFSSL_TRUST_PEER_CERT
    byte haveTrustPeer; /* was cert verified by loaded trusted peer cert */
#endif
#ifdef OPENSSL_EXTRA
    char   untrustedDepth;
#endif
} ProcPeerCertArgs;

static void FreeProcPeerCertArgs(WOLFSSL* ssl, void* pArgs)
{
    ProcPeerCertArgs* args = (ProcPeerCertArgs*)pArgs;

    (void)ssl;

    if (args->domain) {
        XFREE(args->domain, ssl->heap, DYNAMIC_TYPE_STRING);
        args->domain = NULL;
    }
    if (args->certs) {
        XFREE(args->certs, ssl->heap, DYNAMIC_TYPE_DER);
        args->certs = NULL;
    }
#ifdef WOLFSSL_TLS13
    if (args->exts) {
        XFREE(args->exts, ssl->heap, DYNAMIC_TYPE_CERT_EXT);
        args->exts = NULL;
    }
#endif
    if (args->dCert) {
        if (args->dCertInit) {
            FreeDecodedCert(args->dCert);
            args->dCertInit = 0;
        }
        XFREE(args->dCert, ssl->heap, DYNAMIC_TYPE_DCERT);
        args->dCert = NULL;
    }
}

int ProcessPeerCerts(WOLFSSL* ssl, byte* input, word32* inOutIdx,
                     word32 totalSz)
{
    int ret = 0;
#ifdef WOLFSSL_ASYNC_CRYPT
    ProcPeerCertArgs* args = (ProcPeerCertArgs*)ssl->async.args;
    typedef char args_test[sizeof(ssl->async.args) >= sizeof(*args) ? 1 : -1];
    (void)sizeof(args_test);
#else
    ProcPeerCertArgs  args[1];
#endif

#ifdef WOLFSSL_TRUST_PEER_CERT
    byte haveTrustPeer = 0; /* was cert verified by loaded trusted peer cert */
#endif

    WOLFSSL_ENTER("ProcessPeerCerts");

#ifdef WOLFSSL_ASYNC_CRYPT
    ret = wolfSSL_AsyncPop(ssl, &ssl->options.asyncState);
    if (ret != WC_NOT_PENDING_E) {
        /* Check for error */
        if (ret < 0)
            goto exit_ppc;
    }
    else
#endif
    {
        /* Reset state */
        ret = 0;
        ssl->options.asyncState = TLS_ASYNC_BEGIN;
        XMEMSET(args, 0, sizeof(ProcPeerCertArgs));
        args->idx = *inOutIdx;
        args->begin = *inOutIdx;
    #ifdef WOLFSSL_ASYNC_CRYPT
        ssl->async.freeArgs = FreeProcPeerCertArgs;
    #endif
    }

    switch (ssl->options.asyncState)
    {
        case TLS_ASYNC_BEGIN:
        {
            word32 listSz;

        #ifdef WOLFSSL_CALLBACKS
            if (ssl->hsInfoOn)
                AddPacketName("Certificate", &ssl->handShakeInfo);
            if (ssl->toInfoOn)
                AddLateName("Certificate", &ssl->timeoutInfo);
        #endif

        #ifdef WOLFSSL_TLS13
            if (ssl->options.tls1_3) {
                byte ctxSz;

                /* Certificate Request Context */
                if ((args->idx - args->begin) + OPAQUE8_LEN > totalSz)
                    return BUFFER_ERROR;
                ctxSz = *(input + args->idx);
                args->idx++;
                if ((args->idx - args->begin) + ctxSz > totalSz)
                    return BUFFER_ERROR;
            #ifndef NO_WOLFSSL_CLIENT
                /* Must be empty when received from server. */
                if (ssl->options.side == WOLFSSL_CLIENT_END) {
                    if (ctxSz != 0) {
                        return INVALID_CERT_CTX_E;
                    }
                }
            #endif
            #ifndef NO_WOLFSSL_SERVER
                /* Must contain value sent in request. */
                if (ssl->options.side == WOLFSSL_SERVER_END) {
                    if (ssl->options.handShakeState != HANDSHAKE_DONE &&
                                                                   ctxSz != 0) {
                        return INVALID_CERT_CTX_E;
                    }
                    else if (ssl->options.handShakeState == HANDSHAKE_DONE) {
                #ifdef WOLFSSL_POST_HANDSHAKE_AUTH
                         CertReqCtx* curr = ssl->certReqCtx;
                         CertReqCtx* prev = NULL;
                         while (curr != NULL) {
                             if ((ctxSz == curr->len) &&
                                 XMEMCMP(&curr->ctx, input + args->idx, ctxSz)
                                                                         == 0) {
                                     if (prev != NULL)
                                         prev->next = curr->next;
                                     else
                                         ssl->certReqCtx = curr->next;
                                     XFREE(curr, ssl->heap,
                                           DYNAMIC_TYPE_TMP_BUFFER);
                                     break;
                             }
                             prev = curr;
                             curr = curr->next;
                        }
                        if (curr == NULL)
                #endif
                            return INVALID_CERT_CTX_E;
                    }
                }
            #endif
                args->idx += ctxSz;

                /* allocate buffer for cert extensions */
                args->exts = (buffer*)XMALLOC(sizeof(buffer) * MAX_CHAIN_DEPTH,
                                            ssl->heap, DYNAMIC_TYPE_CERT_EXT);
                if (args->exts == NULL) {
                    ERROR_OUT(MEMORY_E, exit_ppc);
                }
            }
        #endif

            /* allocate buffer for certs */
            args->certs = (buffer*)XMALLOC(sizeof(buffer) * MAX_CHAIN_DEPTH,
                                            ssl->heap, DYNAMIC_TYPE_DER);
            if (args->certs == NULL) {
                ERROR_OUT(MEMORY_E, exit_ppc);
            }
            XMEMSET(args->certs, 0, sizeof(buffer) * MAX_CHAIN_DEPTH);

            /* Certificate List */
            if ((args->idx - args->begin) + OPAQUE24_LEN > totalSz) {
                ERROR_OUT(BUFFER_ERROR, exit_ppc);
            }
            c24to32(input + args->idx, &listSz);
            args->idx += OPAQUE24_LEN;
            if (listSz > MAX_RECORD_SIZE) {
                ERROR_OUT(BUFFER_ERROR, exit_ppc);
            }
            if ((args->idx - args->begin) + listSz != totalSz) {
                ERROR_OUT(BUFFER_ERROR, exit_ppc);
            }

            WOLFSSL_MSG("Loading peer's cert chain");
            /* first put cert chain into buffer so can verify top down
               we're sent bottom up */
            while (listSz) {
                word32 certSz;

                if (args->totalCerts >= MAX_CHAIN_DEPTH) {
                #ifdef OPENSSL_EXTRA
                    ssl->peerVerifyRet = X509_V_ERR_CERT_CHAIN_TOO_LONG;
                #endif
                    ERROR_OUT(MAX_CHAIN_ERROR, exit_ppc);
                }

                if ((args->idx - args->begin) + OPAQUE24_LEN > totalSz) {
                    ERROR_OUT(BUFFER_ERROR, exit_ppc);
                }

                c24to32(input + args->idx, &certSz);
                args->idx += OPAQUE24_LEN;

                if ((args->idx - args->begin) + certSz > totalSz) {
                    ERROR_OUT(BUFFER_ERROR, exit_ppc);
                }

                args->certs[args->totalCerts].length = certSz;
                args->certs[args->totalCerts].buffer = input + args->idx;

            #ifdef SESSION_CERTS
                if (ssl->session.chain.count < MAX_CHAIN_DEPTH &&
                                               certSz < MAX_X509_SIZE) {
                    ssl->session.chain.certs[
                        ssl->session.chain.count].length = certSz;
                    XMEMCPY(ssl->session.chain.certs[
                        ssl->session.chain.count].buffer,
                            input + args->idx, certSz);
                    ssl->session.chain.count++;
                }
                else {
                    WOLFSSL_MSG("Couldn't store chain cert for session");
                }
            #endif /* SESSION_CERTS */

                args->idx += certSz;
                listSz -= certSz + CERT_HEADER_SZ;

            #ifdef WOLFSSL_TLS13
                /* Extensions */
                if (ssl->options.tls1_3) {
                    word16 extSz;

                    if ((args->idx - args->begin) + OPAQUE16_LEN > totalSz)
                        return BUFFER_ERROR;
                    ato16(input + args->idx, &extSz);
                    args->idx += OPAQUE16_LEN;
                    if ((args->idx - args->begin) + extSz > totalSz)
                        return BUFFER_ERROR;
                    /* Store extension data info for later processing. */
                    args->exts[args->totalCerts].length = extSz;
                    args->exts[args->totalCerts].buffer = input + args->idx;
                    args->idx += extSz;
                    listSz -= extSz + OPAQUE16_LEN;
                }
            #endif

                args->totalCerts++;
                WOLFSSL_MSG("\tPut another cert into chain");
            } /* while (listSz) */

            args->count = args->totalCerts;
            args->certIdx = 0;

            args->dCertInit = 0;
            args->dCert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), ssl->heap,
                                                       DYNAMIC_TYPE_DCERT);
            if (args->dCert == NULL) {
                ERROR_OUT(MEMORY_E, exit_ppc);
            }

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_BUILD;
        } /* case TLS_ASYNC_BEGIN */
        FALL_THROUGH;

        case TLS_ASYNC_BUILD:
        {
            if (args->count > 0) {
            #ifdef WOLFSSL_TRUST_PEER_CERT
                if (args->certIdx == 0) {
                    /* if using trusted peer certs check before verify chain
                       and CA test */
                    TrustedPeerCert* tp;

                    if (!args->dCertInit) {
                        InitDecodedCert(args->dCert,
                            args->certs[args->certIdx].buffer,
                            args->certs[args->certIdx].length, ssl->heap);
                        args->dCert->sigCtx.devId = ssl->devId; /* setup async dev */
                    #ifdef WOLFSSL_ASYNC_CRYPT
                        args->dCert->sigCtx.asyncCtx = ssl;
                    #endif
                        args->dCertInit = 1;
                    }

                    ret = ParseCertRelative(args->dCert, CERT_TYPE, 0,
                                                            ssl->ctx->cm);
                    if (ret != 0 && ret != WC_PENDING_E)
                        goto exit_ppc;

                #ifdef WOLFSSL_ASYNC_CRYPT
                    if (ret == WC_PENDING_E) {
                        ret = wolfSSL_AsyncPush(ssl,
                            args->dCert->sigCtx.asyncDev);
                        goto exit_ppc;
                    }
                #endif

                #ifndef NO_SKID
                    if (args->dCert->extAuthKeyIdSet) {
                        tp = GetTrustedPeer(ssl->ctx->cm,
                                    args->dCert->extSubjKeyId, WC_MATCH_SKID);
                    }
                    else { /* if the cert has no SKID try to match by name */
                        tp = GetTrustedPeer(ssl->ctx->cm,
                                    args->dCert->subjectHash, WC_MATCH_NAME);
                    }
                #else /* NO_SKID */
                    tp = GetTrustedPeer(ssl->ctx->cm, args->dCert->subjectHash,
                                                                 WC_MATCH_NAME);
                #endif /* NO SKID */
                    WOLFSSL_MSG("Checking for trusted peer cert");

                    if (tp == NULL) {
                        /* no trusted peer cert */
                        WOLFSSL_MSG("No matching trusted peer cert. "
                            "Checking CAs");
                        FreeDecodedCert(args->dCert);
                        args->dCertInit = 0;
                    #ifdef OPENSSL_EXTRA
                        args->untrustedDepth = 1;
                    #endif
                    } else if (MatchTrustedPeer(tp, args->dCert)){
                        WOLFSSL_MSG("Found matching trusted peer cert");
                        haveTrustPeer = 1;
                    } else {
                        WOLFSSL_MSG("Trusted peer cert did not match!");
                        FreeDecodedCert(args->dCert);
                        args->dCertInit = 0;
                    #ifdef OPENSSL_EXTRA
                        args->untrustedDepth = 1;
                    #endif
                    }
                }
            #endif /* WOLFSSL_TRUST_PEER_CERT */
            #ifdef OPENSSL_EXTRA
                #ifdef WOLFSSL_TRUST_PEER_CERT
                else
                #endif
                if (args->certIdx == 0) {
                    byte* subjectHash;

                    if (!args->dCertInit) {
                        InitDecodedCert(args->dCert,
                            args->certs[args->certIdx].buffer,
                            args->certs[args->certIdx].length, ssl->heap);
                        args->dCert->sigCtx.devId = ssl->devId;
                    #ifdef WOLFSSL_ASYNC_CRYPT
                        args->dCert->sigCtx.asyncCtx = ssl;
                    #endif
                        args->dCertInit = 1;
                    }

                    ret = ParseCertRelative(args->dCert, CERT_TYPE, 0,
                                                                  ssl->ctx->cm);
                    if (ret != 0 && ret != WC_PENDING_E) {
                        goto exit_ppc;
                    }
                #ifdef WOLFSSL_ASYNC_CRYPT
                    if (ret == WC_PENDING_E) {
                        ret = wolfSSL_AsyncPush(ssl,
                            args->dCert->sigCtx.asyncDev);
                        goto exit_ppc;
                    }
                #endif

                #ifndef NO_SKID
                    subjectHash = args->dCert->extSubjKeyId;
                #else
                    subjectHash = args->dCert->subjectHash;
                #endif
                    if (!AlreadySigner(ssl->ctx->cm, subjectHash))
                        args->untrustedDepth = 1;
                    FreeDecodedCert(args->dCert);
                    args->dCertInit = 0;
                }
            #endif

                /* verify up to peer's first */
                /* do not verify chain if trusted peer cert found */
                while (args->count > 1
                #ifdef WOLFSSL_TRUST_PEER_CERT
                    && !haveTrustPeer
                #endif /* WOLFSSL_TRUST_PEER_CERT */
                ) {
                    byte* subjectHash;

                    args->certIdx = args->count - 1;

                    if (!args->dCertInit) {
                        InitDecodedCert(args->dCert,
                            args->certs[args->certIdx].buffer,
                            args->certs[args->certIdx].length, ssl->heap);
                        args->dCert->sigCtx.devId = ssl->devId; /* setup async dev */
                    #ifdef WOLFSSL_ASYNC_CRYPT
                        args->dCert->sigCtx.asyncCtx = ssl;
                    #endif
                        args->dCertInit = 1;
                    }

                    ret = ParseCertRelative(args->dCert, CERT_TYPE,
                                    !ssl->options.verifyNone, ssl->ctx->cm);
                #ifdef WOLFSSL_ASYNC_CRYPT
                    if (ret == WC_PENDING_E) {
                        ret = wolfSSL_AsyncPush(ssl,
                            args->dCert->sigCtx.asyncDev);
                        goto exit_ppc;
                    }
                #endif

                #ifndef NO_SKID
                    subjectHash = args->dCert->extSubjKeyId;
                #else
                    subjectHash = args->dCert->subjectHash;
                #endif

                    /* Check key sizes for certs. Is redundent check since
                       ProcessBuffer also performs this check. */
                    if (!ssl->options.verifyNone) {
                        switch (args->dCert->keyOID) {
                        #ifndef NO_RSA
                            case RSAk:
                                if (ssl->options.minRsaKeySz < 0 ||
                                        args->dCert->pubKeySize <
                                         (word16)ssl->options.minRsaKeySz) {
                                    WOLFSSL_MSG(
                                        "RSA key size in cert chain error");
                                    ret = RSA_KEY_SIZE_E;
                                }
                                break;
                        #endif /* !NO_RSA */
                        #ifdef HAVE_ECC
                            case ECDSAk:
                                if (ssl->options.minEccKeySz < 0 ||
                                        args->dCert->pubKeySize <
                                         (word16)ssl->options.minEccKeySz) {
                                    WOLFSSL_MSG(
                                        "ECC key size in cert chain error");
                                    ret = ECC_KEY_SIZE_E;
                                }
                                break;
                        #endif /* HAVE_ECC */
                        #ifdef HAVE_ED25519
                            case ED25519k:
                                if (ssl->options.minEccKeySz < 0 ||
                                        ED25519_KEY_SIZE <
                                         (word16)ssl->options.minEccKeySz) {
                                    WOLFSSL_MSG(
                                        "ECC key size in cert chain error");
                                    ret = ECC_KEY_SIZE_E;
                                }
                                break;
                        #endif /* HAVE_ED25519 */
                            default:
                                WOLFSSL_MSG("Key size not checked");
                                /* key not being checked for size if not in
                                   switch */
                                break;
                        } /* switch (dCert->keyOID) */
                    } /* if (!ssl->options.verifyNone) */

                    if (ret == 0 && args->dCert->isCA == 0) {
                        WOLFSSL_MSG("Chain cert is not a CA, not adding as one");
                    }
                    else if (ret == 0 && ssl->options.verifyNone) {
                        WOLFSSL_MSG("Chain cert not verified by option, not adding as CA");
                    }
                    else if (ret == 0 && !AlreadySigner(ssl->ctx->cm, subjectHash)) {
                        DerBuffer* add = NULL;
                        ret = AllocDer(&add, args->certs[args->certIdx].length,
                                                            CA_TYPE, ssl->heap);
                        if (ret < 0)
                            goto exit_ppc;

                        WOLFSSL_MSG("Adding CA from chain");

                        XMEMCPY(add->buffer, args->certs[args->certIdx].buffer,
                                             args->certs[args->certIdx].length);


                    #ifdef WOLFSSL_NGINX
                        if (args->certIdx > args->untrustedDepth)
                            args->untrustedDepth = args->certIdx + 1;
                    #endif

                        /* already verified above */
                        ret = AddCA(ssl->ctx->cm, &add, WOLFSSL_CHAIN_CA, 0);
                        if (ret == 1) {
                            ret = 0;   /* SSL_SUCCESS for external */
                        }
                    }
                    else if (ret != 0) {
                        WOLFSSL_MSG("Failed to verify CA from chain");
                    #ifdef OPENSSL_EXTRA
                        ssl->peerVerifyRet = X509_V_ERR_INVALID_CA;
                    #endif
                    }
                    else {
                        WOLFSSL_MSG("Verified CA from chain and already had it");
                    }

            #if defined(HAVE_OCSP) || defined(HAVE_CRL)
                    if (ret == 0) {
                        int doCrlLookup = 1;
                #ifdef HAVE_OCSP
                    #ifdef HAVE_CERTIFICATE_STATUS_REQUEST_V2
                        if (ssl->status_request_v2) {
                            ret = TLSX_CSR2_InitRequests(ssl->extensions,
                                                    args->dCert, 0, ssl->heap);
                        }
                        else /* skips OCSP and force CRL check */
                    #endif /* HAVE_CERTIFICATE_STATUS_REQUEST_V2 */
                        if (ssl->ctx->cm->ocspEnabled &&
                                            ssl->ctx->cm->ocspCheckAll) {
                            WOLFSSL_MSG("Doing Non Leaf OCSP check");
                            ret = CheckCertOCSP(ssl->ctx->cm->ocsp, args->dCert,
                                                                          NULL);
                        #ifdef WOLFSSL_ASYNC_CRYPT
                            /* non-blocking socket re-entry requires async */
                            if (ret == WANT_READ) {
                                goto exit_ppc;
                            }
                        #endif
                            doCrlLookup = (ret == OCSP_CERT_UNKNOWN);
                            if (ret != 0) {
                                doCrlLookup = 0;
                                WOLFSSL_MSG("\tOCSP Lookup not ok");
                            }
                        }
                #endif /* HAVE_OCSP */

                #ifdef HAVE_CRL
                        if (ret == 0 && doCrlLookup &&
                                    ssl->ctx->cm->crlEnabled &&
                                                ssl->ctx->cm->crlCheckAll) {
                            WOLFSSL_MSG("Doing Non Leaf CRL check");
                            ret = CheckCertCRL(ssl->ctx->cm->crl, args->dCert);
                        #ifdef WOLFSSL_ASYNC_CRYPT
                            /* non-blocking socket re-entry requires async */
                            if (ret == WANT_READ) {
                                goto exit_ppc;
                            }
                        #endif
                            if (ret != 0) {
                                WOLFSSL_MSG("\tCRL check not ok");
                            }
                        }
                #endif /* HAVE_CRL */
                        (void)doCrlLookup;
                    }
            #endif /* HAVE_OCSP || HAVE_CRL */

                    if (ret != 0 && args->lastErr == 0) {
                        args->lastErr = ret;   /* save error from last time */
                        ret = 0; /* reset error */
                    }

                    FreeDecodedCert(args->dCert);
                    args->dCertInit = 0;
                    args->count--;
                } /* while (count > 0 && !haveTrustPeer) */
            } /* if (count > 0) */

            /* Check for error */
            if (ret != 0) {
                goto exit_ppc;
            }

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_DO;
        } /* case TLS_ASYNC_BUILD */
        FALL_THROUGH;

        case TLS_ASYNC_DO:
        {
            /* peer's, may not have one if blank client cert sent by TLSv1.2 */
            if (args->count > 0) {
                WOLFSSL_MSG("Verifying Peer's cert");

                args->certIdx = 0;

                if (!args->dCertInit) {
                    InitDecodedCert(args->dCert,
                        args->certs[args->certIdx].buffer,
                        args->certs[args->certIdx].length, ssl->heap);
                    args->dCert->sigCtx.devId = ssl->devId; /* setup async dev */
                #ifdef WOLFSSL_ASYNC_CRYPT
                    args->dCert->sigCtx.asyncCtx = ssl;
                #endif
                    args->dCertInit = 1;
                }

            #ifdef WOLFSSL_TRUST_PEER_CERT
                if (!haveTrustPeer)
            #endif
                {
                    /* only parse if not already present in dCert from above */
                    ret = ParseCertRelative(args->dCert, CERT_TYPE,
                                    !ssl->options.verifyNone, ssl->ctx->cm);
                #ifdef WOLFSSL_ASYNC_CRYPT
                    if (ret == WC_PENDING_E) {
                        ret = wolfSSL_AsyncPush(ssl,
                            args->dCert->sigCtx.asyncDev);
                        goto exit_ppc;
                    }
                #endif
                }

                if (ret == 0) {
                    WOLFSSL_MSG("Verified Peer's cert");
                #ifdef OPENSSL_EXTRA
                    ssl->peerVerifyRet = X509_V_OK;
                #endif
                    args->fatal = 0;
                }
                else if (ret == ASN_PARSE_E || ret == BUFFER_E) {
                    WOLFSSL_MSG("Got Peer cert ASN PARSE or BUFFER ERROR");
                #ifdef OPENSSL_EXTRA
                    ssl->peerVerifyRet = X509_V_ERR_CERT_REJECTED;
                #endif
                    args->fatal = 1;
                }
                else {
                    WOLFSSL_MSG("Failed to verify Peer's cert");
                #ifdef OPENSSL_EXTRA
                    ssl->peerVerifyRet = X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE;
                #endif
                    if (ssl->verifyCallback) {
                        WOLFSSL_MSG(
                            "\tCallback override available, will continue");
                        args->fatal = 0;
                    }
                    else {
                        WOLFSSL_MSG("\tNo callback override available, fatal");
                        args->fatal = 1;
                    }
                }

            #ifdef HAVE_SECURE_RENEGOTIATION
                if (args->fatal == 0 && ssl->secure_renegotiation
                               && ssl->secure_renegotiation->enabled) {

                    if (IsEncryptionOn(ssl, 0)) {
                        /* compare against previous time */
                        if (XMEMCMP(args->dCert->subjectHash,
                                    ssl->secure_renegotiation->subject_hash,
                                    SHA_DIGEST_SIZE) != 0) {
                            WOLFSSL_MSG(
                                "Peer sent different cert during scr, fatal");
                            args->fatal = 1;
                            ret   = SCR_DIFFERENT_CERT_E;
                        }
                    }

                    /* cache peer's hash */
                    if (args->fatal == 0) {
                        XMEMCPY(ssl->secure_renegotiation->subject_hash,
                                args->dCert->subjectHash, SHA_DIGEST_SIZE);
                    }
                }
            #endif /* HAVE_SECURE_RENEGOTIATION */

            #if defined(HAVE_OCSP) || defined(HAVE_CRL)
                if (args->fatal == 0) {
                    int doLookup = 1;

                    if (ssl->options.side == WOLFSSL_CLIENT_END) {
                #ifdef HAVE_CERTIFICATE_STATUS_REQUEST
                        if (ssl->status_request) {
                            args->fatal = TLSX_CSR_InitRequest(ssl->extensions,
                                                    args->dCert, ssl->heap);
                            doLookup = 0;
                        }
                #endif /* HAVE_CERTIFICATE_STATUS_REQUEST */
                #ifdef HAVE_CERTIFICATE_STATUS_REQUEST_V2
                        if (ssl->status_request_v2) {
                            args->fatal = TLSX_CSR2_InitRequests(ssl->extensions,
                                                    args->dCert, 1, ssl->heap);
                            doLookup = 0;
                        }
                #endif /* HAVE_CERTIFICATE_STATUS_REQUEST_V2 */
                    }

                #ifdef HAVE_OCSP
                    if (doLookup && ssl->ctx->cm->ocspEnabled) {
                        WOLFSSL_MSG("Doing Leaf OCSP check");
                        ret = CheckCertOCSP(ssl->ctx->cm->ocsp, args->dCert,
                                                                          NULL);
                    #ifdef WOLFSSL_ASYNC_CRYPT
                        /* non-blocking socket re-entry requires async */
                        if (ret == WANT_READ) {
                            goto exit_ppc;
                        }
                    #endif
                        doLookup = (ret == OCSP_CERT_UNKNOWN);
                        if (ret != 0) {
                            WOLFSSL_MSG("\tOCSP Lookup not ok");
                            args->fatal = 0;
                        #ifdef OPENSSL_EXTRA
                            ssl->peerVerifyRet = X509_V_ERR_CERT_REJECTED;
                        #endif
                        }
                    }
                #endif /* HAVE_OCSP */

                #ifdef HAVE_CRL
                    if (doLookup && ssl->ctx->cm->crlEnabled) {
                        WOLFSSL_MSG("Doing Leaf CRL check");
                        ret = CheckCertCRL(ssl->ctx->cm->crl, args->dCert);
                    #ifdef WOLFSSL_ASYNC_CRYPT
                        /* non-blocking socket re-entry requires async */
                        if (ret == WANT_READ) {
                            goto exit_ppc;
                        }
                    #endif
                        if (ret != 0) {
                            WOLFSSL_MSG("\tCRL check not ok");
                            args->fatal = 0;
                        #ifdef OPENSSL_EXTRA
                            ssl->peerVerifyRet = X509_V_ERR_CERT_REJECTED;
                        #endif
                        }
                    }
                #endif /* HAVE_CRL */
                    (void)doLookup;
                }
            #endif /* HAVE_OCSP || HAVE_CRL */

            #ifdef KEEP_PEER_CERT
                if (args->fatal == 0) {
                    /* set X509 format for peer cert */
                    int copyRet = CopyDecodedToX509(&ssl->peerCert,
                                                                args->dCert);
                    if (copyRet == MEMORY_E)
                        args->fatal = 1;
                }
            #endif /* KEEP_PEER_CERT */

            #ifndef IGNORE_KEY_EXTENSIONS
                if (args->dCert->extKeyUsageSet) {
                    if ((ssl->specs.kea == rsa_kea) &&
                        (ssl->options.side == WOLFSSL_CLIENT_END) &&
                        (args->dCert->extKeyUsage & KEYUSE_KEY_ENCIPHER) == 0) {
                        ret = KEYUSE_ENCIPHER_E;
                    }
                    if ((ssl->specs.sig_algo == rsa_sa_algo ||
                            (ssl->specs.sig_algo == ecc_dsa_sa_algo &&
                                 !ssl->specs.static_ecdh)) &&
                        (args->dCert->extKeyUsage & KEYUSE_DIGITAL_SIG) == 0) {
                        WOLFSSL_MSG("KeyUse Digital Sig not set");
                        ret = KEYUSE_SIGNATURE_E;
                    }
                }

                if (args->dCert->extExtKeyUsageSet) {
                    if (ssl->options.side == WOLFSSL_CLIENT_END) {
                        if ((args->dCert->extExtKeyUsage &
                                (EXTKEYUSE_ANY | EXTKEYUSE_SERVER_AUTH)) == 0) {
                            WOLFSSL_MSG("ExtKeyUse Server Auth not set");
                            ret = EXTKEYUSE_AUTH_E;
                        }
                    }
                    else {
                        if ((args->dCert->extExtKeyUsage &
                                (EXTKEYUSE_ANY | EXTKEYUSE_CLIENT_AUTH)) == 0) {
                            WOLFSSL_MSG("ExtKeyUse Client Auth not set");
                            ret = EXTKEYUSE_AUTH_E;
                        }
                    }
                }
            #endif /* IGNORE_KEY_EXTENSIONS */

                if (args->fatal) {
                    ssl->error = ret;
                #ifdef OPENSSL_EXTRA
                    ssl->peerVerifyRet = X509_V_ERR_CERT_REJECTED;
                #endif
                    goto exit_ppc;
                }

                ssl->options.havePeerCert = 1;
            } /* if (count > 0) */

            /* Check for error */
            if (args->fatal && ret != 0) {
                goto exit_ppc;
            }

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_VERIFY;
        } /* case TLS_ASYNC_DO */
        FALL_THROUGH;

        case TLS_ASYNC_VERIFY:
        {
            if (args->count > 0) {
                args->domain = (char*)XMALLOC(ASN_NAME_MAX, ssl->heap,
                                                    DYNAMIC_TYPE_STRING);
                if (args->domain == NULL) {
                    ERROR_OUT(MEMORY_E, exit_ppc);
                }

                /* store for callback use */
                if (args->dCert->subjectCNLen < ASN_NAME_MAX) {
                    XMEMCPY(args->domain, args->dCert->subjectCN, args->dCert->subjectCNLen);
                    args->domain[args->dCert->subjectCNLen] = '\0';
                }
                else {
                    args->domain[0] = '\0';
                }

                if (!ssl->options.verifyNone && ssl->buffers.domainName.buffer) {
                    if (MatchDomainName(args->dCert->subjectCN,
                                args->dCert->subjectCNLen,
                                (char*)ssl->buffers.domainName.buffer) == 0) {
                        WOLFSSL_MSG("DomainName match on common name failed");
                        if (CheckAltNames(args->dCert,
                                 (char*)ssl->buffers.domainName.buffer) == 0 ) {
                            WOLFSSL_MSG(
                                "DomainName match on alt names failed too");
                            /* try to get peer key still */
                            ret = DOMAIN_NAME_MISMATCH;
                        }
                    }
                }

                /* decode peer key */
                switch (args->dCert->keyOID) {
                #ifndef NO_RSA
                    case RSAk:
                    {
                        word32 keyIdx = 0;
                        int keyRet = 0;

                        if (ssl->peerRsaKey == NULL) {
                            keyRet = AllocKey(ssl, DYNAMIC_TYPE_RSA,
                                                (void**)&ssl->peerRsaKey);
                        } else if (ssl->peerRsaKeyPresent) {
                            keyRet = ReuseKey(ssl, DYNAMIC_TYPE_RSA,
                                              ssl->peerRsaKey);
                            ssl->peerRsaKeyPresent = 0;
                        }

                        if (keyRet != 0 || wc_RsaPublicKeyDecode(
                                args->dCert->publicKey, &keyIdx, ssl->peerRsaKey,
                                                args->dCert->pubKeySize) != 0) {
                            ret = PEER_KEY_ERROR;
                        }
                        else {
                            ssl->peerRsaKeyPresent = 1;
                    #ifdef HAVE_PK_CALLBACKS
                        #ifndef NO_RSA
                            ssl->buffers.peerRsaKey.buffer =
                                   (byte*)XMALLOC(args->dCert->pubKeySize,
                                                ssl->heap, DYNAMIC_TYPE_RSA);
                            if (ssl->buffers.peerRsaKey.buffer == NULL) {
                                ret = MEMORY_ERROR;
                            }
                            else {
                                XMEMCPY(ssl->buffers.peerRsaKey.buffer,
                                        args->dCert->publicKey,
                                        args->dCert->pubKeySize);
                                ssl->buffers.peerRsaKey.length =
                                    args->dCert->pubKeySize;
                            }
                        #endif /* NO_RSA */
                    #endif /* HAVE_PK_CALLBACKS */
                        }

                        /* check size of peer RSA key */
                        if (ret == 0 && ssl->peerRsaKeyPresent &&
                                          !ssl->options.verifyNone &&
                                          wc_RsaEncryptSize(ssl->peerRsaKey)
                                              < ssl->options.minRsaKeySz) {
                            ret = RSA_KEY_SIZE_E;
                            WOLFSSL_MSG("Peer RSA key is too small");
                        }
                        break;
                    }
                #endif /* NO_RSA */
                #ifdef HAVE_NTRU
                    case NTRUk:
                    {
                        if (args->dCert->pubKeySize > sizeof(ssl->peerNtruKey)) {
                            ret = PEER_KEY_ERROR;
                        }
                        else {
                            XMEMCPY(ssl->peerNtruKey, args->dCert->publicKey,
                                                      args->dCert->pubKeySize);
                            ssl->peerNtruKeyLen =
                                (word16)args->dCert->pubKeySize;
                            ssl->peerNtruKeyPresent = 1;
                        }
                        break;
                    }
                #endif /* HAVE_NTRU */
                #ifdef HAVE_ECC
                    case ECDSAk:
                    {
                        int curveId;
                        int keyRet = 0;
                        if (ssl->peerEccDsaKey == NULL) {
                            /* alloc/init on demand */
                            keyRet = AllocKey(ssl, DYNAMIC_TYPE_ECC,
                                    (void**)&ssl->peerEccDsaKey);
                        } else if (ssl->peerEccDsaKeyPresent) {
                            keyRet = ReuseKey(ssl, DYNAMIC_TYPE_ECC,
                                              ssl->peerEccDsaKey);
                            ssl->peerEccDsaKeyPresent = 0;
                        }

                        curveId = wc_ecc_get_oid(args->dCert->keyOID, NULL, NULL);
                        if (keyRet != 0 ||
                            wc_ecc_import_x963_ex(args->dCert->publicKey,
                                    args->dCert->pubKeySize, ssl->peerEccDsaKey,
                                                            curveId) != 0) {
                            ret = PEER_KEY_ERROR;
                        }
                        else {
                            ssl->peerEccDsaKeyPresent = 1;
                    #ifdef HAVE_PK_CALLBACKS
                            ssl->buffers.peerEccDsaKey.buffer =
                                   (byte*)XMALLOC(args->dCert->pubKeySize,
                                           ssl->heap, DYNAMIC_TYPE_ECC);
                            if (ssl->buffers.peerEccDsaKey.buffer == NULL) {
                                ERROR_OUT(MEMORY_ERROR, exit_ppc);
                            }
                            else {
                                XMEMCPY(ssl->buffers.peerEccDsaKey.buffer,
                                        args->dCert->publicKey,
                                        args->dCert->pubKeySize);
                                ssl->buffers.peerEccDsaKey.length =
                                        args->dCert->pubKeySize;
                            }
                    #endif /*HAVE_PK_CALLBACKS */
                        }

                        /* check size of peer ECC key */
                        if (ret == 0 && ssl->peerEccDsaKeyPresent &&
                                              !ssl->options.verifyNone &&
                                              wc_ecc_size(ssl->peerEccDsaKey)
                                              < ssl->options.minEccKeySz) {
                            ret = ECC_KEY_SIZE_E;
                            WOLFSSL_MSG("Peer ECC key is too small");
                        }
                        break;
                    }
                #endif /* HAVE_ECC */
                #ifdef HAVE_ED25519
                    case ED25519k:
                    {
                        int keyRet = 0;
                        if (ssl->peerEd25519Key == NULL) {
                            /* alloc/init on demand */
                            keyRet = AllocKey(ssl, DYNAMIC_TYPE_ED25519,
                                    (void**)&ssl->peerEd25519Key);
                        } else if (ssl->peerEd25519KeyPresent) {
                            keyRet = ReuseKey(ssl, DYNAMIC_TYPE_ED25519,
                                              ssl->peerEd25519Key);
                            ssl->peerEd25519KeyPresent = 0;
                        }

                        if (keyRet != 0 ||
                            wc_ed25519_import_public(args->dCert->publicKey,
                                                     args->dCert->pubKeySize,
                                                     ssl->peerEd25519Key)
                                                                         != 0) {
                            ret = PEER_KEY_ERROR;
                        }
                        else {
                            ssl->peerEd25519KeyPresent = 1;
                    #ifdef HAVE_PK_CALLBACKS
                            ssl->buffers.peerEd25519Key.buffer =
                                   (byte*)XMALLOC(args->dCert->pubKeySize,
                                           ssl->heap, DYNAMIC_TYPE_ED25519);
                            if (ssl->buffers.peerEd25519Key.buffer == NULL) {
                                ERROR_OUT(MEMORY_ERROR, exit_ppc);
                            }
                            else {
                                XMEMCPY(ssl->buffers.peerEd25519Key.buffer,
                                        args->dCert->publicKey,
                                        args->dCert->pubKeySize);
                                ssl->buffers.peerEd25519Key.length =
                                        args->dCert->pubKeySize;
                            }
                    #endif /*HAVE_PK_CALLBACKS */
                        }

                        /* check size of peer ECC key */
                        if (ret == 0 && ssl->peerEd25519KeyPresent &&
                                  !ssl->options.verifyNone &&
                                  ED25519_KEY_SIZE < ssl->options.minEccKeySz) {
                            ret = ECC_KEY_SIZE_E;
                            WOLFSSL_MSG("Peer ECC key is too small");
                        }
                        break;
                    }
                #endif /* HAVE_ECC */
                    default:
                        break;
                }

                FreeDecodedCert(args->dCert);
                args->dCertInit = 0;

                /* release since we don't need it anymore */
                if (args->dCert) {
                    XFREE(args->dCert, ssl->heap, DYNAMIC_TYPE_DCERT);
                    args->dCert = NULL;
                }
            } /* if (count > 0) */

            /* Check for error */
            if (args->fatal && ret != 0) {
                goto exit_ppc;
            }

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_FINALIZE;
        } /* case TLS_ASYNC_VERIFY */
        FALL_THROUGH;

        case TLS_ASYNC_FINALIZE:
        {
        #ifdef WOLFSSL_SMALL_STACK
            WOLFSSL_X509_STORE_CTX* store = (WOLFSSL_X509_STORE_CTX*)XMALLOC(
                                    sizeof(WOLFSSL_X509_STORE_CTX), ssl->heap,
                                                    DYNAMIC_TYPE_X509_STORE);
            if (store == NULL) {
                ERROR_OUT(MEMORY_E, exit_ppc);
            }
        #else
            WOLFSSL_X509_STORE_CTX  store[1];
        #endif

            XMEMSET(store, 0, sizeof(WOLFSSL_X509_STORE_CTX));

            /* load last error */
            if (args->lastErr != 0 && ret == 0) {
                ret = args->lastErr;
            }

        #ifdef OPENSSL_EXTRA
            if (args->untrustedDepth > ssl->options.verifyDepth) {
                ssl->peerVerifyRet = X509_V_ERR_CERT_CHAIN_TOO_LONG;
                ret = MAX_CHAIN_ERROR;
            }
        #endif
            if (ret != 0) {
                if (!ssl->options.verifyNone) {
                    int why = bad_certificate;

                    if (ret == ASN_AFTER_DATE_E || ret == ASN_BEFORE_DATE_E) {
                        why = certificate_expired;
                    }
                    if (ssl->verifyCallback) {
                        int ok;

                        store->error = ret;
                        store->error_depth = args->totalCerts;
                        store->discardSessionCerts = 0;
                        store->domain = args->domain;
                        store->userCtx = ssl->verifyCbCtx;
                        store->certs = args->certs;
                        store->totalCerts = args->totalCerts;
                    #ifdef KEEP_PEER_CERT
                        if (ssl->peerCert.subject.sz > 0)
                            store->current_cert = &ssl->peerCert;
                        else
                            store->current_cert = NULL;
                    #else
                        store->current_cert = NULL;
                    #endif /* KEEP_PEER_CERT */
                    #if defined(HAVE_EX_DATA) || defined(HAVE_FORTRESS)
                        store->ex_data = ssl;
                    #endif
                        ok = ssl->verifyCallback(0, store);
                        if (ok) {
                            WOLFSSL_MSG("Verify callback overriding error!");
                            ret = 0;
                        }
                    #ifdef SESSION_CERTS
                        if (store->discardSessionCerts) {
                            WOLFSSL_MSG("Verify callback requested discard sess certs");
                            ssl->session.chain.count = 0;
                        }
                    #endif /* SESSION_CERTS */
                    }
                    if (ret != 0) {
                        SendAlert(ssl, alert_fatal, why);   /* try to send */
                        ssl->options.isClosed = 1;
                    }
                }
                ssl->error = ret;
            }
        #ifdef WOLFSSL_ALWAYS_VERIFY_CB
            else {
                if (ssl->verifyCallback) {
                    int ok;

                    store->error = ret;
                #ifdef WOLFSSL_WPAS
                    store->error_depth = 0;
                #else
                    store->error_depth = args->totalCerts;
                #endif
                    store->discardSessionCerts = 0;
                    store->domain = args->domain;
                    store->userCtx = ssl->verifyCbCtx;
                    store->certs = args->certs;
                    store->totalCerts = args->totalCerts;
                #ifdef KEEP_PEER_CERT
                    if (ssl->peerCert.subject.sz > 0)
                        store->current_cert = &ssl->peerCert;
                    else
                        store->current_cert = NULL;
                #endif
                    store->ex_data = ssl;

                    ok = ssl->verifyCallback(1, store);
                    if (!ok) {
                        WOLFSSL_MSG("Verify callback overriding valid certificate!");
                        ret = -1;
                        SendAlert(ssl, alert_fatal, bad_certificate);
                        ssl->options.isClosed = 1;
                    }
                #ifdef SESSION_CERTS
                    if (store->discardSessionCerts) {
                        WOLFSSL_MSG("Verify callback requested discard sess certs");
                        ssl->session.chain.count = 0;
                    }
                #endif /* SESSION_CERTS */
                }
            }
        #endif /* WOLFSSL_ALWAYS_VERIFY_CB */

            if (ssl->options.verifyNone &&
                                      (ret == CRL_MISSING || ret == CRL_CERT_REVOKED)) {
                WOLFSSL_MSG("Ignoring CRL problem based on verify setting");
                ret = ssl->error = 0;
            }

            if (ret == 0 && ssl->options.side == WOLFSSL_CLIENT_END) {
                ssl->options.serverState = SERVER_CERT_COMPLETE;
            }

            if (IsEncryptionOn(ssl, 0)) {
                args->idx += ssl->keys.padSz;
            }

        #ifdef WOLFSSL_SMALL_STACK
            XFREE(store, ssl->heap, DYNAMIC_TYPE_X509_STORE);
        #endif
            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_END;
        } /* case TLS_ASYNC_FINALIZE */
        FALL_THROUGH;

        case TLS_ASYNC_END:
        {
            /* Set final index */
            *inOutIdx = args->idx;

            break;
        }
        default:
            ret = INPUT_CASE_ERROR;
            break;
    } /* switch(ssl->options.asyncState) */

exit_ppc:

    WOLFSSL_LEAVE("ProcessPeerCerts", ret);

#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_PENDING_E || ret == WANT_READ) {
        /* Mark message as not recevied so it can process again */
        ssl->msgsReceived.got_certificate = 0;

        return ret;
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    FreeProcPeerCertArgs(ssl, args);
    FreeKeyExchange(ssl);

    return ret;
}

static int DoCertificate(WOLFSSL* ssl, byte* input, word32* inOutIdx,
                                                                word32 size)
{
    return ProcessPeerCerts(ssl, input, inOutIdx, size);
}

static int DoCertificateStatus(WOLFSSL* ssl, byte* input, word32* inOutIdx,
                                                                    word32 size)
{
    int    ret = 0;
    byte   status_type;
    word32 status_length;

    if (size < ENUM_LEN + OPAQUE24_LEN)
        return BUFFER_ERROR;

    status_type = input[(*inOutIdx)++];

    c24to32(input + *inOutIdx, &status_length);
    *inOutIdx += OPAQUE24_LEN;

    if (size != ENUM_LEN + OPAQUE24_LEN + status_length)
        return BUFFER_ERROR;

    switch (status_type) {

    #if defined(HAVE_CERTIFICATE_STATUS_REQUEST) \
     || defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)

        /* WOLFSSL_CSR_OCSP overlaps with WOLFSSL_CSR2_OCSP */
        case WOLFSSL_CSR2_OCSP: {
            OcspRequest* request;

            #ifdef WOLFSSL_SMALL_STACK
                CertStatus* status;
                OcspResponse* response;
            #else
                CertStatus status[1];
                OcspResponse response[1];
            #endif

            do {
                #ifdef HAVE_CERTIFICATE_STATUS_REQUEST
                    if (ssl->status_request) {
                        request = (OcspRequest*)TLSX_CSR_GetRequest(
                                                               ssl->extensions);
                        ssl->status_request = 0;
                        break;
                    }
                #endif

                #ifdef HAVE_CERTIFICATE_STATUS_REQUEST_V2
                    if (ssl->status_request_v2) {
                        request = (OcspRequest*)TLSX_CSR2_GetRequest(
                                               ssl->extensions, status_type, 0);
                        ssl->status_request_v2 = 0;
                        break;
                    }
                #endif

                return BUFFER_ERROR;
            } while(0);

            if (request == NULL)
                return BAD_CERTIFICATE_STATUS_ERROR; /* not expected */

            #ifdef WOLFSSL_SMALL_STACK
                status = (CertStatus*)XMALLOC(sizeof(CertStatus), ssl->heap,
                                                       DYNAMIC_TYPE_OCSP_STATUS);
                response = (OcspResponse*)XMALLOC(sizeof(OcspResponse), ssl->heap,
                                                       DYNAMIC_TYPE_OCSP_REQUEST);

                if (status == NULL || response == NULL) {
                    if (status)
                        XFREE(status, NULL, DYNAMIC_TYPE_OCSP_STATUS);
                    if (response)
                        XFREE(response, NULL, DYNAMIC_TYPE_OCSP_REQUEST);

                    return MEMORY_ERROR;
                }
            #endif

            InitOcspResponse(response, status, input +*inOutIdx, status_length);

            if (OcspResponseDecode(response, ssl->ctx->cm, ssl->heap, 0) != 0)
                ret = BAD_CERTIFICATE_STATUS_ERROR;
            else if (CompareOcspReqResp(request, response) != 0)
                ret = BAD_CERTIFICATE_STATUS_ERROR;
            else if (response->responseStatus != OCSP_SUCCESSFUL)
                ret = BAD_CERTIFICATE_STATUS_ERROR;
            else if (response->status->status == CERT_REVOKED)
                ret = OCSP_CERT_REVOKED;
            else if (response->status->status != CERT_GOOD)
                ret = BAD_CERTIFICATE_STATUS_ERROR;

            *inOutIdx += status_length;

            #ifdef WOLFSSL_SMALL_STACK
                XFREE(status,   ssl->heap, DYNAMIC_TYPE_OCSP_STATUS);
                XFREE(response, ssl->heap, DYNAMIC_TYPE_OCSP_REQUEST);
            #endif

        }
        break;

    #endif

    #if defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)

        case WOLFSSL_CSR2_OCSP_MULTI: {
            OcspRequest* request;
            word32 list_length = status_length;
            byte   idx = 0;

            #ifdef WOLFSSL_SMALL_STACK
                CertStatus* status;
                OcspResponse* response;
            #else
                CertStatus status[1];
                OcspResponse response[1];
            #endif

            do {
                if (ssl->status_request_v2) {
                    ssl->status_request_v2 = 0;
                    break;
                }

                return BUFFER_ERROR;
            } while(0);

            #ifdef WOLFSSL_SMALL_STACK
                status = (CertStatus*)XMALLOC(sizeof(CertStatus), ssl->heap,
                                                       DYNAMIC_TYPE_OCSP_STATUS);
                response = (OcspResponse*)XMALLOC(sizeof(OcspResponse), ssl->heap,
                                                       DYNAMIC_TYPE_OCSP_REQUEST);

                if (status == NULL || response == NULL) {
                    if (status)
                        XFREE(status, ssl->heap, DYNAMIC_TYPE_OCSP_STATUS);
                    if (response)
                        XFREE(response, ssl->heap, DYNAMIC_TYPE_OCSP_REQUEST);

                    return MEMORY_ERROR;
                }
            #endif

            while (list_length && ret == 0) {
                if (OPAQUE24_LEN > list_length) {
                    ret = BUFFER_ERROR;
                    break;
                }

                c24to32(input + *inOutIdx, &status_length);
                *inOutIdx   += OPAQUE24_LEN;
                list_length -= OPAQUE24_LEN;

                if (status_length > list_length) {
                    ret = BUFFER_ERROR;
                    break;
                }

                if (status_length) {
                    InitOcspResponse(response, status, input +*inOutIdx,
                                                                 status_length);

                    if ((OcspResponseDecode(response, ssl->ctx->cm, ssl->heap,
                                                                        0) != 0)
                    ||  (response->responseStatus != OCSP_SUCCESSFUL)
                    ||  (response->status->status != CERT_GOOD))
                        ret = BAD_CERTIFICATE_STATUS_ERROR;

                    while (ret == 0) {
                        request = (OcspRequest*)TLSX_CSR2_GetRequest(
                                ssl->extensions, status_type, idx++);

                        if (request == NULL)
                            ret = BAD_CERTIFICATE_STATUS_ERROR;
                        else if (CompareOcspReqResp(request, response) == 0)
                            break;
                        else if (idx == 1) /* server cert must be OK */
                            ret = BAD_CERTIFICATE_STATUS_ERROR;
                    }

                    *inOutIdx   += status_length;
                    list_length -= status_length;
                }
            }

            #if defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
                ssl->status_request_v2 = 0;
            #endif

            #ifdef WOLFSSL_SMALL_STACK
                XFREE(status,   NULL, DYNAMIC_TYPE_OCSP_STATUS);
                XFREE(response, NULL, DYNAMIC_TYPE_OCSP_REQUEST);
            #endif

        }
        break;

    #endif

        default:
            ret = BUFFER_ERROR;
    }

    if (ret != 0)
        SendAlert(ssl, alert_fatal, bad_certificate_status_response);

    return ret;
}

#endif /* !NO_CERTS */


static int DoHelloRequest(WOLFSSL* ssl, const byte* input, word32* inOutIdx,
                                                    word32 size, word32 totalSz)
{
    (void)input;

    if (size) /* must be 0 */
        return BUFFER_ERROR;

    if (IsEncryptionOn(ssl, 0)) {
        /* access beyond input + size should be checked against totalSz */
        if (*inOutIdx + ssl->keys.padSz > totalSz)
            return BUFFER_E;

        *inOutIdx += ssl->keys.padSz;
    }

    if (ssl->options.side == WOLFSSL_SERVER_END) {
        SendAlert(ssl, alert_fatal, unexpected_message); /* try */
        return FATAL_ERROR;
    }
#ifdef HAVE_SECURE_RENEGOTIATION
    else if (ssl->secure_renegotiation && ssl->secure_renegotiation->enabled) {
        ssl->secure_renegotiation->startScr = 1;
        return 0;
    }
#endif
    else {
        return SendAlert(ssl, alert_warning, no_renegotiation);
    }
}


int DoFinished(WOLFSSL* ssl, const byte* input, word32* inOutIdx, word32 size,
                                                      word32 totalSz, int sniff)
{
    word32 finishedSz = (ssl->options.tls ? TLS_FINISHED_SZ : FINISHED_SZ);

    if (finishedSz != size)
        return BUFFER_ERROR;

    /* check against totalSz */
    if (*inOutIdx + size + ssl->keys.padSz > totalSz)
        return BUFFER_E;

    #ifdef WOLFSSL_CALLBACKS
        if (ssl->hsInfoOn) AddPacketName("Finished", &ssl->handShakeInfo);
        if (ssl->toInfoOn) AddLateName("Finished", &ssl->timeoutInfo);
    #endif

    if (sniff == NO_SNIFF) {
        if (XMEMCMP(input + *inOutIdx, &ssl->hsHashes->verifyHashes,size) != 0){
            WOLFSSL_MSG("Verify finished error on hashes");
            return VERIFY_FINISHED_ERROR;
        }
    }

#ifdef HAVE_SECURE_RENEGOTIATION
    if (ssl->secure_renegotiation) {
        /* save peer's state */
        if (ssl->options.side == WOLFSSL_CLIENT_END)
            XMEMCPY(ssl->secure_renegotiation->server_verify_data,
                    input + *inOutIdx, TLS_FINISHED_SZ);
        else
            XMEMCPY(ssl->secure_renegotiation->client_verify_data,
                    input + *inOutIdx, TLS_FINISHED_SZ);
    }
#endif

    /* force input exhaustion at ProcessReply consuming padSz */
    *inOutIdx += size + ssl->keys.padSz;

    if (ssl->options.side == WOLFSSL_CLIENT_END) {
        ssl->options.serverState = SERVER_FINISHED_COMPLETE;
        if (!ssl->options.resuming) {
            ssl->options.handShakeState = HANDSHAKE_DONE;
            ssl->options.handShakeDone  = 1;
        }
    }
    else {
        ssl->options.clientState = CLIENT_FINISHED_COMPLETE;
        if (ssl->options.resuming) {
            ssl->options.handShakeState = HANDSHAKE_DONE;
            ssl->options.handShakeDone  = 1;
        }
    }

    return 0;
}


/* Make sure no duplicates, no fast forward, or other problems; 0 on success */
static int SanityCheckMsgReceived(WOLFSSL* ssl, byte type)
{
    /* verify not a duplicate, mark received, check state */
    switch (type) {

#ifndef NO_WOLFSSL_CLIENT
        case hello_request:
            if (ssl->msgsReceived.got_hello_request) {
                WOLFSSL_MSG("Duplicate HelloRequest received");
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_hello_request = 1;

            break;
#endif

#ifndef NO_WOLFSSL_SERVER
        case client_hello:
            if (ssl->msgsReceived.got_client_hello) {
                WOLFSSL_MSG("Duplicate ClientHello received");
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_client_hello = 1;

            break;
#endif

#ifndef NO_WOLFSSL_CLIENT
        case server_hello:
            if (ssl->msgsReceived.got_server_hello) {
                WOLFSSL_MSG("Duplicate ServerHello received");
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_server_hello = 1;

            break;
#endif

#ifndef NO_WOLFSSL_CLIENT
        case hello_verify_request:
            if (ssl->msgsReceived.got_hello_verify_request) {
                WOLFSSL_MSG("Duplicate HelloVerifyRequest received");
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_hello_verify_request = 1;

            break;
#endif

#ifndef NO_WOLFSSL_CLIENT
        case session_ticket:
            if (ssl->msgsReceived.got_session_ticket) {
                WOLFSSL_MSG("Duplicate SessionTicket received");
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_session_ticket = 1;

            break;
#endif

        case certificate:
            if (ssl->msgsReceived.got_certificate) {
                WOLFSSL_MSG("Duplicate Certificate received");
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_certificate = 1;

#ifndef NO_WOLFSSL_CLIENT
            if (ssl->options.side == WOLFSSL_CLIENT_END) {
                if ( ssl->msgsReceived.got_server_hello == 0) {
                    WOLFSSL_MSG("No ServerHello before Cert");
                    return OUT_OF_ORDER_E;
                }
            }
#endif
#ifndef NO_WOLFSSL_SERVER
            if (ssl->options.side == WOLFSSL_SERVER_END) {
                if ( ssl->msgsReceived.got_client_hello == 0) {
                    WOLFSSL_MSG("No ClientHello before Cert");
                    return OUT_OF_ORDER_E;
                }
            }
#endif
            break;

#ifndef NO_WOLFSSL_CLIENT
        case certificate_status:
            if (ssl->msgsReceived.got_certificate_status) {
                WOLFSSL_MSG("Duplicate CertificateSatatus received");
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_certificate_status = 1;

            if (ssl->msgsReceived.got_certificate == 0) {
                WOLFSSL_MSG("No Certificate before CertificateStatus");
                return OUT_OF_ORDER_E;
            }
            if (ssl->msgsReceived.got_server_key_exchange != 0) {
                WOLFSSL_MSG("CertificateStatus after ServerKeyExchange");
                return OUT_OF_ORDER_E;
            }

            break;
#endif

#ifndef NO_WOLFSSL_CLIENT
        case server_key_exchange:
            if (ssl->msgsReceived.got_server_key_exchange) {
                WOLFSSL_MSG("Duplicate ServerKeyExchange received");
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_server_key_exchange = 1;

            if (ssl->msgsReceived.got_server_hello == 0) {
                WOLFSSL_MSG("No ServerHello before ServerKeyExchange");
                return OUT_OF_ORDER_E;
            }
            if (ssl->msgsReceived.got_certificate_status == 0) {
#ifdef HAVE_CERTIFICATE_STATUS_REQUEST
                if (ssl->status_request) {
                    int ret;

                    WOLFSSL_MSG("No CertificateStatus before ServerKeyExchange");
                    if ((ret = TLSX_CSR_ForceRequest(ssl)) != 0)
                        return ret;
                }
#endif
#ifdef HAVE_CERTIFICATE_STATUS_REQUEST_V2
                if (ssl->status_request_v2) {
                    int ret;

                    WOLFSSL_MSG("No CertificateStatus before ServerKeyExchange");
                    if ((ret = TLSX_CSR2_ForceRequest(ssl)) != 0)
                        return ret;
                }
#endif
            }

            break;
#endif

#ifndef NO_WOLFSSL_CLIENT
        case certificate_request:
            if (ssl->msgsReceived.got_certificate_request) {
                WOLFSSL_MSG("Duplicate CertificateRequest received");
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_certificate_request = 1;

            break;
#endif

#ifndef NO_WOLFSSL_CLIENT
        case server_hello_done:
            if (ssl->msgsReceived.got_server_hello_done) {
                WOLFSSL_MSG("Duplicate ServerHelloDone received");
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_server_hello_done = 1;

            if (ssl->msgsReceived.got_certificate == 0) {
                if (ssl->specs.kea == psk_kea ||
                    ssl->specs.kea == dhe_psk_kea ||
                    ssl->specs.kea == ecdhe_psk_kea ||
                    ssl->options.usingAnon_cipher) {
                    WOLFSSL_MSG("No Cert required");
                } else {
                    WOLFSSL_MSG("No Certificate before ServerHelloDone");
                    return OUT_OF_ORDER_E;
                }
            }
            if (ssl->msgsReceived.got_server_key_exchange == 0) {
                int pskNoServerHint = 0;  /* not required in this case */

                #ifndef NO_PSK
                    if (ssl->specs.kea == psk_kea &&
                                               ssl->arrays->server_hint[0] == 0)
                        pskNoServerHint = 1;
                #endif
                if (ssl->specs.static_ecdh == 1 ||
                    ssl->specs.kea == rsa_kea ||
                    ssl->specs.kea == ntru_kea ||
                    pskNoServerHint) {
                    WOLFSSL_MSG("No KeyExchange required");
                } else {
                    WOLFSSL_MSG("No ServerKeyExchange before ServerDone");
                    return OUT_OF_ORDER_E;
                }
            }
            break;
#endif

#ifndef NO_WOLFSSL_SERVER
        case certificate_verify:
            if (ssl->msgsReceived.got_certificate_verify) {
                WOLFSSL_MSG("Duplicate CertificateVerify received");
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_certificate_verify = 1;

            if ( ssl->msgsReceived.got_certificate == 0) {
                WOLFSSL_MSG("No Cert before CertVerify");
                return OUT_OF_ORDER_E;
            }
            break;
#endif

#ifndef NO_WOLFSSL_SERVER
        case client_key_exchange:
            if (ssl->msgsReceived.got_client_key_exchange) {
                WOLFSSL_MSG("Duplicate ClientKeyExchange received");
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_client_key_exchange = 1;

            if (ssl->msgsReceived.got_client_hello == 0) {
                WOLFSSL_MSG("No ClientHello before ClientKeyExchange");
                return OUT_OF_ORDER_E;
            }
            break;
#endif

        case finished:
            if (ssl->msgsReceived.got_finished) {
                WOLFSSL_MSG("Duplicate Finished received");
                return DUPLICATE_MSG_E;
            }
            ssl->msgsReceived.got_finished = 1;

            if (ssl->msgsReceived.got_change_cipher == 0) {
                WOLFSSL_MSG("Finished received before ChangeCipher");
                return NO_CHANGE_CIPHER_E;
            }
            break;

        case change_cipher_hs:
            if (ssl->msgsReceived.got_change_cipher) {
                WOLFSSL_MSG("Duplicate ChangeCipher received");
                return DUPLICATE_MSG_E;
            }
            /* DTLS is going to ignore the CCS message if the client key
             * exchange message wasn't received yet. */
            if (!ssl->options.dtls)
                ssl->msgsReceived.got_change_cipher = 1;

#ifndef NO_WOLFSSL_CLIENT
            if (ssl->options.side == WOLFSSL_CLIENT_END) {
                if (!ssl->options.resuming &&
                                 ssl->msgsReceived.got_server_hello_done == 0) {
                    WOLFSSL_MSG("No ServerHelloDone before ChangeCipher");
                    return OUT_OF_ORDER_E;
                }
                #ifdef HAVE_SESSION_TICKET
                    if (ssl->expect_session_ticket) {
                        WOLFSSL_MSG("Expected session ticket missing");
                        #ifdef WOLFSSL_DTLS
                            if (ssl->options.dtls)
                                return OUT_OF_ORDER_E;
                        #endif
                        return SESSION_TICKET_EXPECT_E;
                    }
                #endif
            }
#endif
#ifndef NO_WOLFSSL_SERVER
            if (ssl->options.side == WOLFSSL_SERVER_END) {
                if (!ssl->options.resuming &&
                               ssl->msgsReceived.got_client_key_exchange == 0) {
                    WOLFSSL_MSG("No ClientKeyExchange before ChangeCipher");
                    return OUT_OF_ORDER_E;
                }
                #ifndef NO_CERTS
                    if (ssl->options.verifyPeer &&
                        ssl->options.havePeerCert) {

                        if (!ssl->options.havePeerVerify) {
                            WOLFSSL_MSG("client didn't send cert verify");
                            #ifdef WOLFSSL_DTLS
                                if (ssl->options.dtls)
                                    return OUT_OF_ORDER_E;
                            #endif
                            return NO_PEER_VERIFY;
                        }
                    }
                #endif
            }
#endif
            if (ssl->options.dtls)
                ssl->msgsReceived.got_change_cipher = 1;
            break;

        default:
            WOLFSSL_MSG("Unknown message type");
            return SANITY_MSG_E;
    }

    return 0;
}


static int DoHandShakeMsgType(WOLFSSL* ssl, byte* input, word32* inOutIdx,
                          byte type, word32 size, word32 totalSz)
{
    int ret = 0;
    word32 expectedIdx;

    WOLFSSL_ENTER("DoHandShakeMsgType");

#ifdef WOLFSSL_TLS13
    if (type == hello_retry_request) {
        return DoTls13HandShakeMsgType(ssl, input, inOutIdx, type, size,
                                       totalSz);
    }
#endif

    /* make sure can read the message */
    if (*inOutIdx + size > totalSz)
        return INCOMPLETE_DATA;

    expectedIdx = *inOutIdx + size +
                  (ssl->keys.encryptionOn ? ssl->keys.padSz : 0);

    /* sanity check msg received */
    if ( (ret = SanityCheckMsgReceived(ssl, type)) != 0) {
        WOLFSSL_MSG("Sanity Check on handshake message type received failed");
        return ret;
    }

#ifdef WOLFSSL_CALLBACKS
    /* add name later, add on record and handshake header part back on */
    if (ssl->toInfoOn) {
        int add = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;
        AddPacketInfo(0, &ssl->timeoutInfo, input + *inOutIdx - add,
                      size + add, ssl->heap);
        AddLateRecordHeader(&ssl->curRL, &ssl->timeoutInfo);
    }
#endif

    if (ssl->options.handShakeState == HANDSHAKE_DONE && type != hello_request){
        WOLFSSL_MSG("HandShake message after handshake complete");
        SendAlert(ssl, alert_fatal, unexpected_message);
        return OUT_OF_ORDER_E;
    }

    if (ssl->options.side == WOLFSSL_CLIENT_END && ssl->options.dtls == 0 &&
               ssl->options.serverState == NULL_STATE && type != server_hello) {
        WOLFSSL_MSG("First server message not server hello");
        SendAlert(ssl, alert_fatal, unexpected_message);
        return OUT_OF_ORDER_E;
    }

    if (ssl->options.side == WOLFSSL_CLIENT_END && ssl->options.dtls &&
            type == server_hello_done &&
            ssl->options.serverState < SERVER_HELLO_COMPLETE) {
        WOLFSSL_MSG("Server hello done received before server hello in DTLS");
        SendAlert(ssl, alert_fatal, unexpected_message);
        return OUT_OF_ORDER_E;
    }

    if (ssl->options.side == WOLFSSL_SERVER_END &&
               ssl->options.clientState == NULL_STATE && type != client_hello) {
        WOLFSSL_MSG("First client message not client hello");
        SendAlert(ssl, alert_fatal, unexpected_message);
        return OUT_OF_ORDER_E;
    }

    /* above checks handshake state */
    /* hello_request not hashed */
    /* Also, skip hashing the client_hello message here for DTLS. It will be
     * hashed later if the DTLS cookie is correct. */
    if (type != hello_request &&
            !(IsDtlsNotSctpMode(ssl) && type == client_hello) &&
            ssl->error != WC_PENDING_E) {
        ret = HashInput(ssl, input + *inOutIdx, size);
        if (ret != 0) return ret;
    }

    switch (type) {

    case hello_request:
        WOLFSSL_MSG("processing hello request");
        ret = DoHelloRequest(ssl, input, inOutIdx, size, totalSz);
        break;

#ifndef NO_WOLFSSL_CLIENT
    case hello_verify_request:
        WOLFSSL_MSG("processing hello verify request");
        ret = DoHelloVerifyRequest(ssl, input,inOutIdx, size);
        break;

    case server_hello:
        WOLFSSL_MSG("processing server hello");
        ret = DoServerHello(ssl, input, inOutIdx, size);
        break;

#ifndef NO_CERTS
    case certificate_request:
        WOLFSSL_MSG("processing certificate request");
        ret = DoCertificateRequest(ssl, input, inOutIdx, size);
        break;
#endif

    case server_key_exchange:
        WOLFSSL_MSG("processing server key exchange");
        ret = DoServerKeyExchange(ssl, input, inOutIdx, size);
        break;

#ifdef HAVE_SESSION_TICKET
    case session_ticket:
        WOLFSSL_MSG("processing session ticket");
        ret = DoSessionTicket(ssl, input, inOutIdx, size);
        break;
#endif /* HAVE_SESSION_TICKET */
#endif

#ifndef NO_CERTS
    case certificate:
        WOLFSSL_MSG("processing certificate");
        ret = DoCertificate(ssl, input, inOutIdx, size);
        break;

    case certificate_status:
        WOLFSSL_MSG("processing certificate status");
        ret = DoCertificateStatus(ssl, input, inOutIdx, size);
        break;
#endif

    case server_hello_done:
        WOLFSSL_MSG("processing server hello done");
    #ifdef WOLFSSL_CALLBACKS
        if (ssl->hsInfoOn)
            AddPacketName("ServerHelloDone", &ssl->handShakeInfo);
        if (ssl->toInfoOn)
            AddLateName("ServerHelloDone", &ssl->timeoutInfo);
    #endif
        ssl->options.serverState = SERVER_HELLODONE_COMPLETE;
        if (IsEncryptionOn(ssl, 0)) {
            *inOutIdx += ssl->keys.padSz;
        }
        if (ssl->options.resuming) {
            WOLFSSL_MSG("Not resuming as thought");
            ssl->options.resuming = 0;
        }
        break;

    case finished:
        WOLFSSL_MSG("processing finished");
        ret = DoFinished(ssl, input, inOutIdx, size, totalSz, NO_SNIFF);
        break;

#ifndef NO_WOLFSSL_SERVER
    case client_hello:
        WOLFSSL_MSG("processing client hello");
        ret = DoClientHello(ssl, input, inOutIdx, size);
        break;

    case client_key_exchange:
        WOLFSSL_MSG("processing client key exchange");
        ret = DoClientKeyExchange(ssl, input, inOutIdx, size);
        break;

#if !defined(NO_RSA) || defined(HAVE_ECC)
    case certificate_verify:
        WOLFSSL_MSG("processing certificate verify");
        ret = DoCertificateVerify(ssl, input, inOutIdx, size);
        break;
#endif /* !NO_RSA || HAVE_ECC */

#endif /* !NO_WOLFSSL_SERVER */

    default:
        WOLFSSL_MSG("Unknown handshake message type");
        ret = UNKNOWN_HANDSHAKE_TYPE;
        break;
    }

    if (ret == 0 && expectedIdx != *inOutIdx) {
        WOLFSSL_MSG("Extra data in handshake message");
        if (!ssl->options.dtls)
            SendAlert(ssl, alert_fatal, decode_error);
        ret = DECODE_E;
    }

#ifdef WOLFSSL_ASYNC_CRYPT
    /* if async, offset index so this msg will be processed again */
    if (ret == WC_PENDING_E && *inOutIdx > 0) {
        *inOutIdx -= HANDSHAKE_HEADER_SZ;
    #ifdef WOLFSSL_DTLS
        if (ssl->options.dtls) {
            *inOutIdx -= DTLS_HANDSHAKE_EXTRA;
        }
    #endif
    }
#endif

    WOLFSSL_LEAVE("DoHandShakeMsgType()", ret);
    return ret;
}


static int DoHandShakeMsg(WOLFSSL* ssl, byte* input, word32* inOutIdx,
                          word32 totalSz)
{
    int    ret = 0;
    word32 inputLength;

    WOLFSSL_ENTER("DoHandShakeMsg()");

    if (ssl->arrays == NULL) {
        byte   type;
        word32 size;

        if (GetHandShakeHeader(ssl,input,inOutIdx,&type, &size, totalSz) != 0)
            return PARSE_ERROR;

        return DoHandShakeMsgType(ssl, input, inOutIdx, type, size, totalSz);
    }

    inputLength = ssl->buffers.inputBuffer.length - *inOutIdx;

    /* If there is a pending fragmented handshake message,
     * pending message size will be non-zero. */
    if (ssl->arrays->pendingMsgSz == 0) {
        byte   type;
        word32 size;

        if (GetHandShakeHeader(ssl,input, inOutIdx, &type, &size, totalSz) != 0)
            return PARSE_ERROR;

        /* Cap the maximum size of a handshake message to something reasonable.
         * By default is the maximum size of a certificate message assuming
         * nine 2048-bit RSA certificates in the chain. */
        if (size > MAX_HANDSHAKE_SZ) {
            WOLFSSL_MSG("Handshake message too large");
            return HANDSHAKE_SIZE_ERROR;
        }

        /* size is the size of the certificate message payload */
        if (inputLength - HANDSHAKE_HEADER_SZ < size) {
            ssl->arrays->pendingMsgType = type;
            ssl->arrays->pendingMsgSz = size + HANDSHAKE_HEADER_SZ;
            ssl->arrays->pendingMsg = (byte*)XMALLOC(size + HANDSHAKE_HEADER_SZ,
                                                     ssl->heap,
                                                     DYNAMIC_TYPE_ARRAYS);
            if (ssl->arrays->pendingMsg == NULL)
                return MEMORY_E;
            XMEMCPY(ssl->arrays->pendingMsg,
                    input + *inOutIdx - HANDSHAKE_HEADER_SZ,
                    inputLength);
            ssl->arrays->pendingMsgOffset = inputLength;
            *inOutIdx += inputLength - HANDSHAKE_HEADER_SZ;
            return 0;
        }

        ret = DoHandShakeMsgType(ssl, input, inOutIdx, type, size, totalSz);
    }
    else {
        if (inputLength + ssl->arrays->pendingMsgOffset
                                                  > ssl->arrays->pendingMsgSz) {

            return BUFFER_ERROR;
        }

        XMEMCPY(ssl->arrays->pendingMsg + ssl->arrays->pendingMsgOffset,
                input + *inOutIdx, inputLength);
        ssl->arrays->pendingMsgOffset += inputLength;
        *inOutIdx += inputLength;

        if (ssl->arrays->pendingMsgOffset == ssl->arrays->pendingMsgSz)
        {
            word32 idx = 0;
            ret = DoHandShakeMsgType(ssl,
                                     ssl->arrays->pendingMsg
                                                          + HANDSHAKE_HEADER_SZ,
                                     &idx, ssl->arrays->pendingMsgType,
                                     ssl->arrays->pendingMsgSz
                                                          - HANDSHAKE_HEADER_SZ,
                                     ssl->arrays->pendingMsgSz);
        #ifdef WOLFSSL_ASYNC_CRYPT
            if (ret == WC_PENDING_E) {
                /* setup to process fragment again */
                ssl->arrays->pendingMsgOffset -= inputLength;
                *inOutIdx -= inputLength;
            }
            else
        #endif
            {
                XFREE(ssl->arrays->pendingMsg, ssl->heap, DYNAMIC_TYPE_ARRAYS);
                ssl->arrays->pendingMsg = NULL;
                ssl->arrays->pendingMsgSz = 0;
            }
        }
    }

    WOLFSSL_LEAVE("DoHandShakeMsg()", ret);
    return ret;
}

#ifdef WOLFSSL_DTLS

static INLINE int DtlsCheckWindow(WOLFSSL* ssl)
{
    word32* window;
    word16 cur_hi, next_hi;
    word32 cur_lo, next_lo, diff;
    int curLT;
    WOLFSSL_DTLS_PEERSEQ* peerSeq = NULL;

    if (!ssl->options.haveMcast)
        peerSeq = ssl->keys.peerSeq;
    else {
#ifdef WOLFSSL_MULTICAST
        WOLFSSL_DTLS_PEERSEQ* p;
        int i;

        for (i = 0, p = ssl->keys.peerSeq;
             i < WOLFSSL_DTLS_PEERSEQ_SZ;
             i++, p++) {

            if (p->peerId == ssl->keys.curPeerId) {
                peerSeq = p;
                break;
            }
        }
#endif
    }

    if (peerSeq == NULL) {
        WOLFSSL_MSG("Could not find peer sequence");
        return 0;
    }

    if (ssl->keys.curEpoch == peerSeq->nextEpoch) {
        next_hi = peerSeq->nextSeq_hi;
        next_lo = peerSeq->nextSeq_lo;
        window = peerSeq->window;
    }
    else if (ssl->keys.curEpoch == peerSeq->nextEpoch - 1) {
        next_hi = peerSeq->prevSeq_hi;
        next_lo = peerSeq->prevSeq_lo;
        window = peerSeq->prevWindow;
    }
    else {
        return 0;
    }

    cur_hi = ssl->keys.curSeq_hi;
    cur_lo = ssl->keys.curSeq_lo;

    /* If the difference between next and cur is > 2^32, way outside window. */
    if ((cur_hi > next_hi + 1) || (next_hi > cur_hi + 1)) {
        WOLFSSL_MSG("Current record from way too far in the future.");
        return 0;
    }

    if (cur_hi == next_hi) {
        curLT = cur_lo < next_lo;
        diff = curLT ? next_lo - cur_lo : cur_lo - next_lo;
    }
    else {
        curLT = cur_hi < next_hi;
        diff = curLT ? cur_lo - next_lo : next_lo - cur_lo;
    }

    /* Check to see that the next value is greater than the number of messages
     * trackable in the window, and that the difference between the next
     * expected sequence number and the received sequence number is inside the
     * window. */
    if ((next_hi || next_lo > DTLS_SEQ_BITS) &&
        curLT && (diff > DTLS_SEQ_BITS)) {

        WOLFSSL_MSG("Current record sequence number from the past.");
        return 0;
    }
#ifndef WOLFSSL_DTLS_ALLOW_FUTURE
    else if (!curLT && (diff > DTLS_SEQ_BITS)) {
        WOLFSSL_MSG("Rejecting message too far into the future.");
        return 0;
    }
#endif
    else if (curLT) {
        word32 idx = diff / DTLS_WORD_BITS;
        word32 newDiff = diff % DTLS_WORD_BITS;

        /* verify idx is valid for window array */
        if (idx >= WOLFSSL_DTLS_WINDOW_WORDS) {
            WOLFSSL_MSG("Invalid DTLS windows index");
            return 0;
        }

        if (window[idx] & (1 << (newDiff - 1))) {
            WOLFSSL_MSG("Current record sequence number already received.");
            return 0;
        }
    }

    return 1;
}


#ifdef WOLFSSL_MULTICAST
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
#endif /* WOLFSSL_MULTICAST */


static INLINE int DtlsUpdateWindow(WOLFSSL* ssl)
{
    word32* window;
    word32* next_lo;
    word16* next_hi;
    int curLT;
    word32 cur_lo, diff;
    word16 cur_hi;
    WOLFSSL_DTLS_PEERSEQ* peerSeq = ssl->keys.peerSeq;

    cur_hi = ssl->keys.curSeq_hi;
    cur_lo = ssl->keys.curSeq_lo;

#ifdef WOLFSSL_MULTICAST
    if (ssl->options.haveMcast) {
        WOLFSSL_DTLS_PEERSEQ* p;
        int i;

        peerSeq = NULL;
        for (i = 0, p = ssl->keys.peerSeq;
             i < WOLFSSL_DTLS_PEERSEQ_SZ;
             i++, p++) {

            if (p->peerId == ssl->keys.curPeerId) {
                peerSeq = p;
                break;
            }
        }

        if (peerSeq == NULL) {
            WOLFSSL_MSG("Couldn't find that peer ID to update window.");
            return 0;
        }

        if (p->highwaterMark && cur_lo >= p->highwaterMark) {
            int cbError = 0;

            if (ssl->ctx->mcastHwCb)
                cbError = ssl->ctx->mcastHwCb(p->peerId,
                                              ssl->ctx->mcastMaxSeq,
                                              cur_lo, ssl->mcastHwCbCtx);
            if (cbError) {
                WOLFSSL_MSG("Multicast highwater callback returned an error.");
                return MCAST_HIGHWATER_CB_E;
            }

            p->highwaterMark = UpdateHighwaterMark(cur_lo,
                                                   ssl->ctx->mcastFirstSeq,
                                                   ssl->ctx->mcastSecondSeq,
                                                   ssl->ctx->mcastMaxSeq);
        }
    }
#endif

    if (ssl->keys.curEpoch == peerSeq->nextEpoch) {
        next_hi = &peerSeq->nextSeq_hi;
        next_lo = &peerSeq->nextSeq_lo;
        window = peerSeq->window;
    }
    else {
        next_hi = &peerSeq->prevSeq_hi;
        next_lo = &peerSeq->prevSeq_lo;
        window = peerSeq->prevWindow;
    }

    if (cur_hi == *next_hi) {
        curLT = cur_lo < *next_lo;
        diff = curLT ? *next_lo - cur_lo : cur_lo - *next_lo;
    }
    else {
        curLT = cur_hi < *next_hi;
        diff = curLT ? cur_lo - *next_lo : *next_lo - cur_lo;
    }

    if (curLT) {
        word32 idx = diff / DTLS_WORD_BITS;
        word32 newDiff = diff % DTLS_WORD_BITS;

        if (idx < WOLFSSL_DTLS_WINDOW_WORDS)
            window[idx] |= (1 << (newDiff - 1));
    }
    else {
        if (diff >= DTLS_SEQ_BITS)
            XMEMSET(window, 0, DTLS_SEQ_SZ);
        else {
            word32 idx, newDiff, temp, i;
            word32 oldWindow[WOLFSSL_DTLS_WINDOW_WORDS];

            temp = 0;
            diff++;
            idx = diff / DTLS_WORD_BITS;
            newDiff = diff % DTLS_WORD_BITS;

            XMEMCPY(oldWindow, window, sizeof(oldWindow));

            for (i = 0; i < WOLFSSL_DTLS_WINDOW_WORDS; i++) {
                if (i < idx)
                    window[i] = 0;
                else {
                    temp |= (oldWindow[i-idx] << newDiff);
                    window[i] = temp;
                    temp = oldWindow[i-idx] >> (DTLS_WORD_BITS - newDiff);
                }
            }
        }
        window[0] |= 1;
        *next_lo = cur_lo + 1;
        if (*next_lo < cur_lo)
            (*next_hi)++;
    }

    return 1;
}


static int DtlsMsgDrain(WOLFSSL* ssl)
{
    DtlsMsg* item = ssl->dtls_rx_msg_list;
    int ret = 0;

    /* While there is an item in the store list, and it is the expected
     * message, and it is complete, and there hasn't been an error in the
     * last messge... */
    while (item != NULL &&
            ssl->keys.dtls_expected_peer_handshake_number == item->seq &&
            item->fragSz == item->sz &&
            ret == 0) {
        word32 idx = 0;
        ssl->keys.dtls_expected_peer_handshake_number++;
        ret = DoHandShakeMsgType(ssl, item->msg,
                                 &idx, item->type, item->sz, item->sz);
        if (ret == WC_PENDING_E) {
            ssl->keys.dtls_expected_peer_handshake_number--;
            break;
        }
        ssl->dtls_rx_msg_list = item->next;
        DtlsMsgDelete(item, ssl->heap);
        item = ssl->dtls_rx_msg_list;
        ssl->dtls_rx_msg_list_sz--;
    }

    return ret;
}


static int DoDtlsHandShakeMsg(WOLFSSL* ssl, byte* input, word32* inOutIdx,
                          word32 totalSz)
{
    byte type;
    word32 size;
    word32 fragOffset, fragSz;
    int ret = 0;

    WOLFSSL_ENTER("DoDtlsHandShakeMsg()");

    /* process any pending DTLS messages - this flow can happen with async */
    if (ssl->dtls_rx_msg_list != NULL) {
        ret = DtlsMsgDrain(ssl);
        if (ret != 0)
            return ret;

        /* if done processing fragment exit with success */
        if (totalSz == *inOutIdx)
            return ret;
    }

    /* parse header */
    if (GetDtlsHandShakeHeader(ssl, input, inOutIdx, &type,
                               &size, &fragOffset, &fragSz, totalSz) != 0)
        return PARSE_ERROR;

    /* check that we have complete fragment */
    if (*inOutIdx + fragSz > totalSz)
        return INCOMPLETE_DATA;

    /* Check the handshake sequence number first. If out of order,
     * add the current message to the list. If the message is in order,
     * but it is a fragment, add the current message to the list, then
     * check the head of the list to see if it is complete, if so, pop
     * it out as the current message. If the message is complete and in
     * order, process it. Check the head of the list to see if it is in
     * order, if so, process it. (Repeat until list exhausted.) If the
     * head is out of order, return for more processing.
     */
    if (ssl->keys.dtls_peer_handshake_number >
                                ssl->keys.dtls_expected_peer_handshake_number) {
        /* Current message is out of order. It will get stored in the list.
         * Storing also takes care of defragmentation. If the messages is a
         * client hello, we need to process this out of order; the server
         * is not supposed to keep state, but the second client hello will
         * have a different handshake sequence number than is expected, and
         * the server shouldn't be expecting any particular handshake sequence
         * number. (If the cookie changes multiple times in quick succession,
         * the client could be sending multiple new client hello messages
         * with newer and newer cookies.) */
        if (type != client_hello) {
            if (ssl->dtls_rx_msg_list_sz < DTLS_POOL_SZ) {
                DtlsMsgStore(ssl, ssl->keys.dtls_peer_handshake_number,
                             input + *inOutIdx, size, type,
                             fragOffset, fragSz, ssl->heap);
            }
            *inOutIdx += fragSz;
            ret = 0;
        }
        else {
            ret = DoHandShakeMsgType(ssl, input, inOutIdx, type, size, totalSz);
            if (ret == 0) {
                ssl->keys.dtls_expected_peer_handshake_number =
                    ssl->keys.dtls_peer_handshake_number + 1;
            }
        }
    }
    else if (ssl->keys.dtls_peer_handshake_number <
                                ssl->keys.dtls_expected_peer_handshake_number) {
        /* Already saw this message and processed it. It can be ignored. */
        *inOutIdx += fragSz;
        if(type == finished ) {
            if (*inOutIdx + ssl->keys.padSz > totalSz) {
                return BUFFER_E;
            }
            *inOutIdx += ssl->keys.padSz;
        }
        if (IsDtlsNotSctpMode(ssl) &&
            VerifyForDtlsMsgPoolSend(ssl, type, fragOffset)) {

            ret = DtlsMsgPoolSend(ssl, 0);
        }
    }
    else if (fragSz < size) {
        /* Since this branch is in order, but fragmented, dtls_rx_msg_list will
         * be pointing to the message with this fragment in it. Check it to see
         * if it is completed. */
        if (ssl->dtls_rx_msg_list_sz < DTLS_POOL_SZ) {
            DtlsMsgStore(ssl, ssl->keys.dtls_peer_handshake_number,
                         input + *inOutIdx, size, type,
                         fragOffset, fragSz, ssl->heap);
        }
        *inOutIdx += fragSz;
        ret = 0;
        if (ssl->dtls_rx_msg_list != NULL &&
            ssl->dtls_rx_msg_list->fragSz >= ssl->dtls_rx_msg_list->sz)
            ret = DtlsMsgDrain(ssl);
    }
    else {
        /* This branch is in order next, and a complete message. */
        ret = DoHandShakeMsgType(ssl, input, inOutIdx, type, size, totalSz);
        if (ret == 0) {
            if (type != client_hello || !IsDtlsNotSctpMode(ssl))
                ssl->keys.dtls_expected_peer_handshake_number++;
            if (ssl->dtls_rx_msg_list != NULL) {
                ret = DtlsMsgDrain(ssl);
            }
        }
    }

    WOLFSSL_LEAVE("DoDtlsHandShakeMsg()", ret);
    return ret;
}
#endif


#ifdef HAVE_AEAD
static INLINE void AeadIncrementExpIV(WOLFSSL* ssl)
{
    int i;
    for (i = AEAD_MAX_EXP_SZ-1; i >= 0; i--) {
        if (++ssl->keys.aead_exp_IV[i]) return;
    }
}


#if defined(HAVE_POLY1305) && defined(HAVE_CHACHA)
/* Used for the older version of creating AEAD tags with Poly1305 */
static int Poly1305TagOld(WOLFSSL* ssl, byte* additional, const byte* out,
                       byte* cipher, word16 sz, byte* tag)
{
    int ret       = 0;
    int msglen    = (sz - ssl->specs.aead_mac_size);
    word32 keySz  = 32;
    byte padding[8]; /* used to temporarily store lengths */

#ifdef CHACHA_AEAD_TEST
      printf("Using old version of poly1305 input.\n");
#endif

    if (msglen < 0)
        return INPUT_CASE_ERROR;

    if ((ret = wc_Poly1305SetKey(ssl->auth.poly1305, cipher, keySz)) != 0)
        return ret;

    if ((ret = wc_Poly1305Update(ssl->auth.poly1305, additional,
                   AEAD_AUTH_DATA_SZ)) != 0)
        return ret;

    /* length of additional input plus padding */
    XMEMSET(padding, 0, sizeof(padding));
    padding[0] = AEAD_AUTH_DATA_SZ;
    if ((ret = wc_Poly1305Update(ssl->auth.poly1305, padding,
                    sizeof(padding))) != 0)
        return ret;


    /* add cipher info and then its length */
    XMEMSET(padding, 0, sizeof(padding));
    if ((ret = wc_Poly1305Update(ssl->auth.poly1305, out, msglen)) != 0)
        return ret;

    /* 32 bit size of cipher to 64 bit endian */
    padding[0] =  msglen        & 0xff;
    padding[1] = (msglen >>  8) & 0xff;
    padding[2] = (msglen >> 16) & 0xff;
    padding[3] = (msglen >> 24) & 0xff;
    if ((ret = wc_Poly1305Update(ssl->auth.poly1305, padding, sizeof(padding)))
        != 0)
        return ret;

    /* generate tag */
    if ((ret = wc_Poly1305Final(ssl->auth.poly1305, tag)) != 0)
        return ret;

    return ret;
}


/* When the flag oldPoly is not set this follows RFC7905. When oldPoly is set
 * the implmentation follows an older draft for creating the nonce and MAC.
 * The flag oldPoly gets set automaticlly depending on what cipher suite was
 * negotiated in the handshake. This is able to be done because the IDs for the
 * cipher suites was updated in RFC7905 giving unique values for the older
 * draft in comparision to the more recent RFC.
 *
 * ssl   WOLFSSL structure to get cipher and TLS state from
 * out   output buffer to hold encrypted data
 * input data to encrypt
 * sz    size of input
 *
 * Return 0 on success negative values in error case
 */
static int  ChachaAEADEncrypt(WOLFSSL* ssl, byte* out, const byte* input,
                              word16 sz)
{
    const byte* additionalSrc = input - RECORD_HEADER_SZ;
    int ret       = 0;
    word32 msgLen = (sz - ssl->specs.aead_mac_size);
    byte tag[POLY1305_AUTH_SZ];
    byte add[AEAD_AUTH_DATA_SZ];
    byte nonce[CHACHA20_NONCE_SZ];
    byte poly[CHACHA20_256_KEY_SIZE]; /* generated key for poly1305 */
    #ifdef CHACHA_AEAD_TEST
        int i;
    #endif

    XMEMSET(tag,   0, sizeof(tag));
    XMEMSET(nonce, 0, sizeof(nonce));
    XMEMSET(poly,  0, sizeof(poly));
    XMEMSET(add,   0, sizeof(add));

    /* opaque SEQ number stored for AD */
    WriteSEQ(ssl, CUR_ORDER, add);

    if (ssl->options.oldPoly != 0) {
        /* get nonce. SEQ should not be incremented again here */
        XMEMCPY(nonce + CHACHA20_OLD_OFFSET, add, OPAQUE32_LEN * 2);
    }

    /* Store the type, version. Unfortunately, they are in
     * the input buffer ahead of the plaintext. */
    #ifdef WOLFSSL_DTLS
        if (ssl->options.dtls) {
            additionalSrc -= DTLS_HANDSHAKE_EXTRA;
            DtlsSEQIncrement(ssl, CUR_ORDER);
        }
    #endif

    /* add TLS message size to additional data */
    add[AEAD_AUTH_DATA_SZ - 2] = (msgLen >> 8) & 0xff;
    add[AEAD_AUTH_DATA_SZ - 1] =  msgLen       & 0xff;

    XMEMCPY(add + AEAD_TYPE_OFFSET, additionalSrc, 3);

    #ifdef CHACHA_AEAD_TEST
        printf("Encrypt Additional : ");
        for (i = 0; i < AEAD_AUTH_DATA_SZ; i++) {
            printf("%02x", add[i]);
        }
        printf("\n\n");
        printf("input before encryption :\n");
        for (i = 0; i < sz; i++) {
            printf("%02x", input[i]);
            if ((i + 1) % 16 == 0)
                printf("\n");
        }
        printf("\n");
    #endif

    if (ssl->options.oldPoly == 0) {
        /* nonce is formed by 4 0x00 byte padded to the left followed by 8 byte
         * record sequence number XORed with client_write_IV/server_write_IV */
        XMEMCPY(nonce, ssl->keys.aead_enc_imp_IV, CHACHA20_IMP_IV_SZ);
        nonce[4]  ^= add[0];
        nonce[5]  ^= add[1];
        nonce[6]  ^= add[2];
        nonce[7]  ^= add[3];
        nonce[8]  ^= add[4];
        nonce[9]  ^= add[5];
        nonce[10] ^= add[6];
        nonce[11] ^= add[7];
    }

    /* set the nonce for chacha and get poly1305 key */
    if ((ret = wc_Chacha_SetIV(ssl->encrypt.chacha, nonce, 0)) != 0) {
        ForceZero(nonce, CHACHA20_NONCE_SZ);
        return ret;
    }

    ForceZero(nonce, CHACHA20_NONCE_SZ); /* done with nonce, clear it */
    /* create Poly1305 key using chacha20 keystream */
    if ((ret = wc_Chacha_Process(ssl->encrypt.chacha, poly,
                                                      poly, sizeof(poly))) != 0)
        return ret;

    /* encrypt the plain text */
    if ((ret = wc_Chacha_Process(ssl->encrypt.chacha, out,
                                                         input, msgLen)) != 0) {
        ForceZero(poly, sizeof(poly));
        return ret;
    }

    /* get the poly1305 tag using either old padding scheme or more recent */
    if (ssl->options.oldPoly != 0) {
        if ((ret = Poly1305TagOld(ssl, add, (const byte* )out,
                                                         poly, sz, tag)) != 0) {
            ForceZero(poly, sizeof(poly));
            return ret;
        }
    }
    else {
        if ((ret = wc_Poly1305SetKey(ssl->auth.poly1305, poly,
                                                          sizeof(poly))) != 0) {
            ForceZero(poly, sizeof(poly));
            return ret;
        }
        if ((ret = wc_Poly1305_MAC(ssl->auth.poly1305, add,
                            sizeof(add), out, msgLen, tag, sizeof(tag))) != 0) {
            ForceZero(poly, sizeof(poly));
            return ret;
        }
    }
    ForceZero(poly, sizeof(poly)); /* done with poly1305 key, clear it */

    /* append tag to ciphertext */
    XMEMCPY(out + msgLen, tag, sizeof(tag));

    AeadIncrementExpIV(ssl);

    #ifdef CHACHA_AEAD_TEST
       printf("mac tag :\n");
        for (i = 0; i < 16; i++) {
           printf("%02x", tag[i]);
           if ((i + 1) % 16 == 0)
               printf("\n");
        }
       printf("\n\noutput after encrypt :\n");
        for (i = 0; i < sz; i++) {
           printf("%02x", out[i]);
           if ((i + 1) % 16 == 0)
               printf("\n");
        }
        printf("\n");
    #endif

    return ret;
}


/* When the flag oldPoly is not set this follows RFC7905. When oldPoly is set
 * the implmentation follows an older draft for creating the nonce and MAC.
 * The flag oldPoly gets set automaticlly depending on what cipher suite was
 * negotiated in the handshake. This is able to be done because the IDs for the
 * cipher suites was updated in RFC7905 giving unique values for the older
 * draft in comparision to the more recent RFC.
 *
 * ssl   WOLFSSL structure to get cipher and TLS state from
 * plain output buffer to hold decrypted data
 * input data to decrypt
 * sz    size of input
 *
 * Return 0 on success negative values in error case
 */
static int ChachaAEADDecrypt(WOLFSSL* ssl, byte* plain, const byte* input,
                           word16 sz)
{
    byte add[AEAD_AUTH_DATA_SZ];
    byte nonce[CHACHA20_NONCE_SZ];
    byte tag[POLY1305_AUTH_SZ];
    byte poly[CHACHA20_256_KEY_SIZE]; /* generated key for mac */
    int ret    = 0;
    int msgLen = (sz - ssl->specs.aead_mac_size);

    #ifdef CHACHA_AEAD_TEST
       int i;
       printf("input before decrypt :\n");
        for (i = 0; i < sz; i++) {
           printf("%02x", input[i]);
           if ((i + 1) % 16 == 0)
               printf("\n");
        }
        printf("\n");
    #endif

    XMEMSET(tag,   0, sizeof(tag));
    XMEMSET(poly,  0, sizeof(poly));
    XMEMSET(nonce, 0, sizeof(nonce));
    XMEMSET(add,   0, sizeof(add));

    /* sequence number field is 64-bits */
    WriteSEQ(ssl, PEER_ORDER, add);

    if (ssl->options.oldPoly != 0) {
        /* get nonce, SEQ should not be incremented again here */
        XMEMCPY(nonce + CHACHA20_OLD_OFFSET, add, OPAQUE32_LEN * 2);
    }

    /* get AD info */
    /* Store the type, version. */
    add[AEAD_TYPE_OFFSET] = ssl->curRL.type;
    add[AEAD_VMAJ_OFFSET] = ssl->curRL.pvMajor;
    add[AEAD_VMIN_OFFSET] = ssl->curRL.pvMinor;

    /* add TLS message size to additional data */
    add[AEAD_AUTH_DATA_SZ - 2] = (msgLen >> 8) & 0xff;
    add[AEAD_AUTH_DATA_SZ - 1] =  msgLen       & 0xff;

    #ifdef CHACHA_AEAD_TEST
        printf("Decrypt Additional : ");
        for (i = 0; i < AEAD_AUTH_DATA_SZ; i++) {
            printf("%02x", add[i]);
        }
        printf("\n\n");
    #endif

    if (ssl->options.oldPoly == 0) {
        /* nonce is formed by 4 0x00 byte padded to the left followed by 8 byte
         * record sequence number XORed with client_write_IV/server_write_IV */
        XMEMCPY(nonce, ssl->keys.aead_dec_imp_IV, CHACHA20_IMP_IV_SZ);
        nonce[4]  ^= add[0];
        nonce[5]  ^= add[1];
        nonce[6]  ^= add[2];
        nonce[7]  ^= add[3];
        nonce[8]  ^= add[4];
        nonce[9]  ^= add[5];
        nonce[10] ^= add[6];
        nonce[11] ^= add[7];
    }

    /* set nonce and get poly1305 key */
    if ((ret = wc_Chacha_SetIV(ssl->decrypt.chacha, nonce, 0)) != 0) {
        ForceZero(nonce, CHACHA20_NONCE_SZ);
        return ret;
    }

    ForceZero(nonce, CHACHA20_NONCE_SZ); /* done with nonce, clear it */
    /* use chacha20 keystream to get poly1305 key for tag */
    if ((ret = wc_Chacha_Process(ssl->decrypt.chacha, poly,
                                                      poly, sizeof(poly))) != 0)
        return ret;

    /* get the tag using Poly1305 */
    if (ssl->options.oldPoly != 0) {
        if ((ret = Poly1305TagOld(ssl, add, input, poly, sz, tag)) != 0) {
            ForceZero(poly, sizeof(poly));
            return ret;
        }
    }
    else {
        if ((ret = wc_Poly1305SetKey(ssl->auth.poly1305, poly,
                                                          sizeof(poly))) != 0) {
            ForceZero(poly, sizeof(poly));
            return ret;
        }
        if ((ret = wc_Poly1305_MAC(ssl->auth.poly1305, add,
                   sizeof(add), (byte*)input, msgLen, tag, sizeof(tag))) != 0) {
            ForceZero(poly, sizeof(poly));
            return ret;
        }
    }
    ForceZero(poly, sizeof(poly)); /* done with poly1305 key, clear it */

    /* check tag sent along with packet */
    if (ConstantCompare(input + msgLen, tag, ssl->specs.aead_mac_size) != 0) {
        WOLFSSL_MSG("MAC did not match");
        if (!ssl->options.dtls)
            SendAlert(ssl, alert_fatal, bad_record_mac);
        return VERIFY_MAC_ERROR;
    }

    /* if the tag was good decrypt message */
    if ((ret = wc_Chacha_Process(ssl->decrypt.chacha, plain,
                                                           input, msgLen)) != 0)
        return ret;

    #ifdef CHACHA_AEAD_TEST
       printf("plain after decrypt :\n");
        for (i = 0; i < sz; i++) {
           printf("%02x", plain[i]);
           if ((i + 1) % 16 == 0)
               printf("\n");
        }
        printf("\n");
    #endif

    return ret;
}
#endif /* HAVE_CHACHA && HAVE_POLY1305 */
#endif /* HAVE_AEAD */


static INLINE int EncryptDo(WOLFSSL* ssl, byte* out, const byte* input,
    word16 sz, int asyncOkay)
{
    int ret = 0;
#ifdef WOLFSSL_ASYNC_CRYPT
    WC_ASYNC_DEV* asyncDev = NULL;
    word32 event_flags = WC_ASYNC_FLAG_CALL_AGAIN;
#else
    (void)asyncOkay;
#endif

    (void)out;
    (void)input;
    (void)sz;

    switch (ssl->specs.bulk_cipher_algorithm) {
    #ifdef BUILD_ARC4
        case wolfssl_rc4:
            wc_Arc4Process(ssl->encrypt.arc4, out, input, sz);
            break;
    #endif

    #ifdef BUILD_DES3
        case wolfssl_triple_des:
        #ifdef WOLFSSL_ASYNC_CRYPT
            /* intialize event */
            asyncDev = &ssl->encrypt.des3->asyncDev;
            ret = wolfSSL_AsyncInit(ssl, asyncDev, event_flags);
            if (ret != 0)
                break;
        #endif

            ret = wc_Des3_CbcEncrypt(ssl->encrypt.des3, out, input, sz);
        #ifdef WOLFSSL_ASYNC_CRYPT
            if (ret == WC_PENDING_E && asyncOkay) {
                ret = wolfSSL_AsyncPush(ssl, asyncDev);
            }
        #endif
            break;
    #endif

    #ifdef BUILD_AES
        case wolfssl_aes:
        #ifdef WOLFSSL_ASYNC_CRYPT
            /* intialize event */
            asyncDev = &ssl->encrypt.aes->asyncDev;
            ret = wolfSSL_AsyncInit(ssl, asyncDev, event_flags);
            if (ret != 0)
                break;
        #endif

            ret = wc_AesCbcEncrypt(ssl->encrypt.aes, out, input, sz);
        #ifdef WOLFSSL_ASYNC_CRYPT
            if (ret == WC_PENDING_E && asyncOkay) {
                ret = wolfSSL_AsyncPush(ssl, asyncDev);
            }
        #endif
            break;
    #endif

    #if defined(BUILD_AESGCM) || defined(HAVE_AESCCM)
        case wolfssl_aes_gcm:
        case wolfssl_aes_ccm:/* GCM AEAD macros use same size as CCM */
        {
            wc_AesAuthEncryptFunc aes_auth_fn;
            const byte* additionalSrc;

        #ifdef WOLFSSL_ASYNC_CRYPT
            /* intialize event */
            asyncDev = &ssl->encrypt.aes->asyncDev;
            ret = wolfSSL_AsyncInit(ssl, asyncDev, event_flags);
            if (ret != 0)
                break;
        #endif

        #if defined(BUILD_AESGCM) && defined(HAVE_AESCCM)
            aes_auth_fn = (ssl->specs.bulk_cipher_algorithm == wolfssl_aes_gcm)
                            ? wc_AesGcmEncrypt : wc_AesCcmEncrypt;
        #elif defined(BUILD_AESGCM)
            aes_auth_fn = wc_AesGcmEncrypt;
        #else
            aes_auth_fn = wc_AesCcmEncrypt;
        #endif
            additionalSrc = input - 5;

            XMEMSET(ssl->encrypt.additional, 0, AEAD_AUTH_DATA_SZ);

            /* sequence number field is 64-bits */
            WriteSEQ(ssl, CUR_ORDER, ssl->encrypt.additional);

            /* Store the type, version. Unfortunately, they are in
             * the input buffer ahead of the plaintext. */
        #ifdef WOLFSSL_DTLS
            if (ssl->options.dtls) {
                additionalSrc -= DTLS_HANDSHAKE_EXTRA;
            }
        #endif
            XMEMCPY(ssl->encrypt.additional + AEAD_TYPE_OFFSET,
                                                        additionalSrc, 3);

            /* Store the length of the plain text minus the explicit
             * IV length minus the authentication tag size. */
            c16toa(sz - AESGCM_EXP_IV_SZ - ssl->specs.aead_mac_size,
                                ssl->encrypt.additional + AEAD_LEN_OFFSET);
            XMEMCPY(ssl->encrypt.nonce,
                                ssl->keys.aead_enc_imp_IV, AESGCM_IMP_IV_SZ);
            XMEMCPY(ssl->encrypt.nonce + AESGCM_IMP_IV_SZ,
                                ssl->keys.aead_exp_IV, AESGCM_EXP_IV_SZ);
            ret = aes_auth_fn(ssl->encrypt.aes,
                    out + AESGCM_EXP_IV_SZ, input + AESGCM_EXP_IV_SZ,
                    sz - AESGCM_EXP_IV_SZ - ssl->specs.aead_mac_size,
                    ssl->encrypt.nonce, AESGCM_NONCE_SZ,
                    out + sz - ssl->specs.aead_mac_size,
                    ssl->specs.aead_mac_size,
                    ssl->encrypt.additional, AEAD_AUTH_DATA_SZ);
        #ifdef WOLFSSL_ASYNC_CRYPT
            if (ret == WC_PENDING_E && asyncOkay) {
                ret = wolfSSL_AsyncPush(ssl, asyncDev);
            }
        #endif
        }
        break;
    #endif /* BUILD_AESGCM || HAVE_AESCCM */

    #ifdef HAVE_CAMELLIA
        case wolfssl_camellia:
            ret = wc_CamelliaCbcEncrypt(ssl->encrypt.cam, out, input, sz);
            break;
    #endif

    #ifdef HAVE_HC128
        case wolfssl_hc128:
            ret = wc_Hc128_Process(ssl->encrypt.hc128, out, input, sz);
            break;
    #endif

    #ifdef BUILD_RABBIT
        case wolfssl_rabbit:
            ret = wc_RabbitProcess(ssl->encrypt.rabbit, out, input, sz);
            break;
    #endif

    #if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
        case wolfssl_chacha:
            ret = ChachaAEADEncrypt(ssl, out, input, sz);
            break;
    #endif

    #ifdef HAVE_NULL_CIPHER
        case wolfssl_cipher_null:
            if (input != out) {
                XMEMMOVE(out, input, sz);
            }
            break;
    #endif

    #ifdef HAVE_IDEA
        case wolfssl_idea:
            ret = wc_IdeaCbcEncrypt(ssl->encrypt.idea, out, input, sz);
            break;
    #endif

        default:
            WOLFSSL_MSG("wolfSSL Encrypt programming error");
            ret = ENCRYPT_ERROR;
    }

#ifdef WOLFSSL_ASYNC_CRYPT
    /* if async is not okay, then block */
    if (ret == WC_PENDING_E && !asyncOkay) {
        ret = wc_AsyncWait(ret, asyncDev, event_flags);
    }
#endif

    return ret;
}

static INLINE int Encrypt(WOLFSSL* ssl, byte* out, const byte* input, word16 sz,
    int asyncOkay)
{
    int ret = 0;

#ifdef WOLFSSL_ASYNC_CRYPT
    if (ssl->error == WC_PENDING_E) {
        ssl->error = 0; /* clear async */
    }
#endif

    switch (ssl->encrypt.state) {
        case CIPHER_STATE_BEGIN:
        {
            if (ssl->encrypt.setup == 0) {
                WOLFSSL_MSG("Encrypt ciphers not setup");
                return ENCRYPT_ERROR;
            }

        #ifdef HAVE_FUZZER
            if (ssl->fuzzerCb)
                ssl->fuzzerCb(ssl, input, sz, FUZZ_ENCRYPT, ssl->fuzzerCtx);
        #endif

        #if defined(BUILD_AESGCM) || defined(HAVE_AESCCM)
            /* make sure AES GCM/CCM memory is allocated */
            /* free for these happens in FreeCiphers */
            if (ssl->specs.bulk_cipher_algorithm == wolfssl_aes_ccm ||
                ssl->specs.bulk_cipher_algorithm == wolfssl_aes_gcm) {
                /* make sure auth iv and auth are allocated */
                if (ssl->encrypt.additional == NULL)
                    ssl->encrypt.additional = (byte*)XMALLOC(AEAD_AUTH_DATA_SZ,
                                                   ssl->heap, DYNAMIC_TYPE_AES_BUFFER);
                if (ssl->encrypt.nonce == NULL)
                    ssl->encrypt.nonce = (byte*)XMALLOC(AESGCM_NONCE_SZ,
                                                   ssl->heap, DYNAMIC_TYPE_AES_BUFFER);
                if (ssl->encrypt.additional == NULL ||
                         ssl->encrypt.nonce == NULL) {
                    return MEMORY_E;
                }
            }
        #endif /* BUILD_AESGCM || HAVE_AESCCM */

            /* Advance state and proceed */
            ssl->encrypt.state = CIPHER_STATE_DO;
        }
        FALL_THROUGH;

        case CIPHER_STATE_DO:
        {
            ret = EncryptDo(ssl, out, input, sz, asyncOkay);

            /* Advance state */
            ssl->encrypt.state = CIPHER_STATE_END;

        #ifdef WOLFSSL_ASYNC_CRYPT
            /* If pending, then leave and return will resume below */
            if (ret == WC_PENDING_E) {
                return ret;
            }
        #endif
        }
        FALL_THROUGH;

        case CIPHER_STATE_END:
        {
        #if defined(BUILD_AESGCM) || defined(HAVE_AESCCM)
            if (ssl->specs.bulk_cipher_algorithm == wolfssl_aes_ccm ||
                ssl->specs.bulk_cipher_algorithm == wolfssl_aes_gcm)
            {
                /* finalize authentication cipher */
                AeadIncrementExpIV(ssl);

                if (ssl->encrypt.nonce)
                    ForceZero(ssl->encrypt.nonce, AESGCM_NONCE_SZ);

            #ifdef WOLFSSL_DTLS
                if (ssl->options.dtls)
                    DtlsSEQIncrement(ssl, CUR_ORDER);
            #endif
            }
        #endif /* BUILD_AESGCM || HAVE_AESCCM */
            break;
        }
    }

    /* Reset state */
    ssl->encrypt.state = CIPHER_STATE_BEGIN;

    return ret;
}

static INLINE int DecryptDo(WOLFSSL* ssl, byte* plain, const byte* input,
                           word16 sz)
{
    int ret = 0;

    (void)plain;
    (void)input;
    (void)sz;

    switch (ssl->specs.bulk_cipher_algorithm)
    {
    #ifdef BUILD_ARC4
        case wolfssl_rc4:
            wc_Arc4Process(ssl->decrypt.arc4, plain, input, sz);
            break;
    #endif

    #ifdef BUILD_DES3
        case wolfssl_triple_des:
        #ifdef WOLFSSL_ASYNC_CRYPT
            /* intialize event */
            ret = wolfSSL_AsyncInit(ssl, &ssl->decrypt.des3->asyncDev,
                WC_ASYNC_FLAG_CALL_AGAIN);
            if (ret != 0)
                break;
        #endif

            ret = wc_Des3_CbcDecrypt(ssl->decrypt.des3, plain, input, sz);
        #ifdef WOLFSSL_ASYNC_CRYPT
            if (ret == WC_PENDING_E) {
                ret = wolfSSL_AsyncPush(ssl, &ssl->decrypt.des3->asyncDev);
            }
        #endif
            break;
    #endif

    #ifdef BUILD_AES
        case wolfssl_aes:
        #ifdef WOLFSSL_ASYNC_CRYPT
            /* intialize event */
            ret = wolfSSL_AsyncInit(ssl, &ssl->decrypt.aes->asyncDev,
                WC_ASYNC_FLAG_CALL_AGAIN);
            if (ret != 0)
                break;
        #endif

            ret = wc_AesCbcDecrypt(ssl->decrypt.aes, plain, input, sz);
        #ifdef WOLFSSL_ASYNC_CRYPT
            if (ret == WC_PENDING_E) {
                ret = wolfSSL_AsyncPush(ssl, &ssl->decrypt.aes->asyncDev);
            }
        #endif
            break;
    #endif

    #if defined(BUILD_AESGCM) || defined(HAVE_AESCCM)
        case wolfssl_aes_gcm:
        case wolfssl_aes_ccm: /* GCM AEAD macros use same size as CCM */
        {
            wc_AesAuthDecryptFunc aes_auth_fn;

        #ifdef WOLFSSL_ASYNC_CRYPT
            /* intialize event */
            ret = wolfSSL_AsyncInit(ssl, &ssl->decrypt.aes->asyncDev,
                WC_ASYNC_FLAG_CALL_AGAIN);
            if (ret != 0)
                break;
        #endif

        #if defined(BUILD_AESGCM) && defined(HAVE_AESCCM)
            aes_auth_fn = (ssl->specs.bulk_cipher_algorithm == wolfssl_aes_gcm)
                            ? wc_AesGcmDecrypt : wc_AesCcmDecrypt;
        #elif defined(BUILD_AESGCM)
            aes_auth_fn = wc_AesGcmDecrypt;
        #else
            aes_auth_fn = wc_AesCcmDecrypt;
        #endif

            XMEMSET(ssl->decrypt.additional, 0, AEAD_AUTH_DATA_SZ);

            /* sequence number field is 64-bits */
            WriteSEQ(ssl, PEER_ORDER, ssl->decrypt.additional);

            ssl->decrypt.additional[AEAD_TYPE_OFFSET] = ssl->curRL.type;
            ssl->decrypt.additional[AEAD_VMAJ_OFFSET] = ssl->curRL.pvMajor;
            ssl->decrypt.additional[AEAD_VMIN_OFFSET] = ssl->curRL.pvMinor;

            c16toa(sz - AESGCM_EXP_IV_SZ - ssl->specs.aead_mac_size,
                                    ssl->decrypt.additional + AEAD_LEN_OFFSET);
            XMEMCPY(ssl->decrypt.nonce, ssl->keys.aead_dec_imp_IV,
                                                            AESGCM_IMP_IV_SZ);
            XMEMCPY(ssl->decrypt.nonce + AESGCM_IMP_IV_SZ, input,
                                                            AESGCM_EXP_IV_SZ);
            if ((ret = aes_auth_fn(ssl->decrypt.aes,
                        plain + AESGCM_EXP_IV_SZ,
                        input + AESGCM_EXP_IV_SZ,
                           sz - AESGCM_EXP_IV_SZ - ssl->specs.aead_mac_size,
                        ssl->decrypt.nonce, AESGCM_NONCE_SZ,
                        input + sz - ssl->specs.aead_mac_size,
                        ssl->specs.aead_mac_size,
                        ssl->decrypt.additional, AEAD_AUTH_DATA_SZ)) < 0) {
            #ifdef WOLFSSL_ASYNC_CRYPT
                if (ret == WC_PENDING_E) {
                    ret = wolfSSL_AsyncPush(ssl, &ssl->decrypt.aes->asyncDev);
                }
            #endif
            }
        }
        break;
    #endif /* BUILD_AESGCM || HAVE_AESCCM */

    #ifdef HAVE_CAMELLIA
        case wolfssl_camellia:
            ret = wc_CamelliaCbcDecrypt(ssl->decrypt.cam, plain, input, sz);
            break;
    #endif

    #ifdef HAVE_HC128
        case wolfssl_hc128:
            ret = wc_Hc128_Process(ssl->decrypt.hc128, plain, input, sz);
            break;
    #endif

    #ifdef BUILD_RABBIT
        case wolfssl_rabbit:
            ret = wc_RabbitProcess(ssl->decrypt.rabbit, plain, input, sz);
            break;
    #endif

    #if defined(HAVE_CHACHA) && defined(HAVE_POLY1305)
        case wolfssl_chacha:
            ret = ChachaAEADDecrypt(ssl, plain, input, sz);
            break;
    #endif

    #ifdef HAVE_NULL_CIPHER
        case wolfssl_cipher_null:
            if (input != plain) {
                XMEMMOVE(plain, input, sz);
            }
            break;
    #endif

    #ifdef HAVE_IDEA
        case wolfssl_idea:
            ret = wc_IdeaCbcDecrypt(ssl->decrypt.idea, plain, input, sz);
            break;
    #endif

        default:
            WOLFSSL_MSG("wolfSSL Decrypt programming error");
            ret = DECRYPT_ERROR;
    }

    return ret;
}

static INLINE int Decrypt(WOLFSSL* ssl, byte* plain, const byte* input,
                           word16 sz)
{
    int ret = 0;

#ifdef WOLFSSL_ASYNC_CRYPT
    ret = wolfSSL_AsyncPop(ssl, &ssl->decrypt.state);
    if (ret != WC_NOT_PENDING_E) {
        /* check for still pending */
        if (ret == WC_PENDING_E)
            return ret;

        ssl->error = 0; /* clear async */

        /* let failures through so CIPHER_STATE_END logic is run */
    }
    else
#endif
    {
        /* Reset state */
        ret = 0;
        ssl->decrypt.state = CIPHER_STATE_BEGIN;
    }

    switch (ssl->decrypt.state) {
        case CIPHER_STATE_BEGIN:
        {
            if (ssl->decrypt.setup == 0) {
                WOLFSSL_MSG("Decrypt ciphers not setup");
                return DECRYPT_ERROR;
            }

        #if defined(BUILD_AESGCM) || defined(HAVE_AESCCM)
            /* make sure AES GCM/CCM memory is allocated */
            /* free for these happens in FreeCiphers */
            if (ssl->specs.bulk_cipher_algorithm == wolfssl_aes_ccm ||
                ssl->specs.bulk_cipher_algorithm == wolfssl_aes_gcm) {
                /* make sure auth iv and auth are allocated */
                if (ssl->decrypt.additional == NULL)
                    ssl->decrypt.additional = (byte*)XMALLOC(AEAD_AUTH_DATA_SZ,
                                                   ssl->heap, DYNAMIC_TYPE_AES_BUFFER);
                if (ssl->decrypt.nonce == NULL)
                    ssl->decrypt.nonce = (byte*)XMALLOC(AESGCM_NONCE_SZ,
                                                   ssl->heap, DYNAMIC_TYPE_AES_BUFFER);
                if (ssl->decrypt.additional == NULL ||
                         ssl->decrypt.nonce == NULL) {
                    return MEMORY_E;
                }
            }
        #endif /* BUILD_AESGCM || HAVE_AESCCM */

            /* Advance state and proceed */
            ssl->decrypt.state = CIPHER_STATE_DO;
        }
        FALL_THROUGH;
        case CIPHER_STATE_DO:
        {
            ret = DecryptDo(ssl, plain, input, sz);

            /* Advance state */
            ssl->decrypt.state = CIPHER_STATE_END;

        #ifdef WOLFSSL_ASYNC_CRYPT
            /* If pending, leave and return below */
            if (ret == WC_PENDING_E) {
                return ret;
            }
        #endif
        }
        FALL_THROUGH;
        case CIPHER_STATE_END:
        {
        #if defined(BUILD_AESGCM) || defined(HAVE_AESCCM)
            /* make sure AES GCM/CCM nonce is cleared */
            if (ssl->specs.bulk_cipher_algorithm == wolfssl_aes_ccm ||
                ssl->specs.bulk_cipher_algorithm == wolfssl_aes_gcm) {
                if (ssl->decrypt.nonce)
                    ForceZero(ssl->decrypt.nonce, AESGCM_NONCE_SZ);

                if (ret < 0)
                    ret = VERIFY_MAC_ERROR;
            }
        #endif /* BUILD_AESGCM || HAVE_AESCCM */
            break;
        }
    }

    /* Reset state */
    ssl->decrypt.state = CIPHER_STATE_BEGIN;

    /* handle mac error case */
    if (ret == VERIFY_MAC_ERROR) {
        if (!ssl->options.dtls)
            SendAlert(ssl, alert_fatal, bad_record_mac);

        #ifdef WOLFSSL_DTLS_DROP_STATS
            ssl->macDropCount++;
        #endif /* WOLFSSL_DTLS_DROP_STATS */
    }

    return ret;
}

/* Check conditions for a cipher to have an explicit IV.
 *
 * ssl  The SSL/TLS object.
 * returns 1 if the cipher in use has an explicit IV and 0 otherwise.
 */
static INLINE int CipherHasExpIV(WOLFSSL *ssl)
{
#ifdef WOLFSSL_TLS13
    if (ssl->options.tls1_3)
        return 0;
#endif
    return (ssl->specs.cipher_type == aead) &&
            (ssl->specs.bulk_cipher_algorithm != wolfssl_chacha);
}

/* check cipher text size for sanity */
static int SanityCheckCipherText(WOLFSSL* ssl, word32 encryptSz)
{
#ifdef HAVE_TRUNCATED_HMAC
    word32 minLength = ssl->truncated_hmac ? (byte)TRUNCATED_HMAC_SZ
                                           : ssl->specs.hash_size;
#else
    word32 minLength = ssl->specs.hash_size; /* covers stream */
#endif

    if (ssl->specs.cipher_type == block) {
        if (encryptSz % ssl->specs.block_size) {
            WOLFSSL_MSG("Block ciphertext not block size");
            return SANITY_CIPHER_E;
        }

        minLength++;  /* pad byte */

        if (ssl->specs.block_size > minLength)
            minLength = ssl->specs.block_size;

        if (ssl->options.tls1_1)
            minLength += ssl->specs.block_size;  /* explicit IV */
    }
    else if (ssl->specs.cipher_type == aead) {
        minLength = ssl->specs.aead_mac_size;    /* authTag size */
        if (CipherHasExpIV(ssl))
            minLength += AESGCM_EXP_IV_SZ;       /* explicit IV  */
    }

    if (encryptSz < minLength) {
        WOLFSSL_MSG("Ciphertext not minimum size");
        return SANITY_CIPHER_E;
    }

    return 0;
}


#ifndef NO_OLD_TLS

static INLINE void Md5Rounds(int rounds, const byte* data, int sz)
{
    Md5 md5;
    int i;

    wc_InitMd5(&md5);   /* no error check on purpose, dummy round */

    for (i = 0; i < rounds; i++)
        wc_Md5Update(&md5, data, sz);
    wc_Md5Free(&md5); /* in case needed to release resources */
}



/* do a dummy sha round */
static INLINE void ShaRounds(int rounds, const byte* data, int sz)
{
    Sha sha;
    int i;

    wc_InitSha(&sha);  /* no error check on purpose, dummy round */

    for (i = 0; i < rounds; i++)
        wc_ShaUpdate(&sha, data, sz);
    wc_ShaFree(&sha); /* in case needed to release resources */
}
#endif


#ifndef NO_SHA256

static INLINE void Sha256Rounds(int rounds, const byte* data, int sz)
{
    Sha256 sha256;
    int i;

    wc_InitSha256(&sha256);  /* no error check on purpose, dummy round */

    for (i = 0; i < rounds; i++) {
        wc_Sha256Update(&sha256, data, sz);
        /* no error check on purpose, dummy round */
    }
    wc_Sha256Free(&sha256); /* in case needed to release resources */
}

#endif


#ifdef WOLFSSL_SHA384

static INLINE void Sha384Rounds(int rounds, const byte* data, int sz)
{
    Sha384 sha384;
    int i;

    wc_InitSha384(&sha384);  /* no error check on purpose, dummy round */

    for (i = 0; i < rounds; i++) {
        wc_Sha384Update(&sha384, data, sz);
        /* no error check on purpose, dummy round */
    }
    wc_Sha384Free(&sha384); /* in case needed to release resources */
}

#endif


#ifdef WOLFSSL_SHA512

static INLINE void Sha512Rounds(int rounds, const byte* data, int sz)
{
    Sha512 sha512;
    int i;

    wc_InitSha512(&sha512);  /* no error check on purpose, dummy round */

    for (i = 0; i < rounds; i++) {
        wc_Sha512Update(&sha512, data, sz);
        /* no error check on purpose, dummy round */
    }
    wc_Sha512Free(&sha512); /* in case needed to release resources */
}

#endif


#ifdef WOLFSSL_RIPEMD

static INLINE void RmdRounds(int rounds, const byte* data, int sz)
{
    RipeMd ripemd;
    int i;

    wc_InitRipeMd(&ripemd);

    for (i = 0; i < rounds; i++)
        wc_RipeMdUpdate(&ripemd, data, sz);
}

#endif


/* Do dummy rounds */
static INLINE void DoRounds(int type, int rounds, const byte* data, int sz)
{
    (void)rounds;
    (void)data;
    (void)sz;

    switch (type) {
        case no_mac :
            break;

#ifndef NO_OLD_TLS
#ifndef NO_MD5
        case md5_mac :
            Md5Rounds(rounds, data, sz);
            break;
#endif

#ifndef NO_SHA
        case sha_mac :
            ShaRounds(rounds, data, sz);
            break;
#endif
#endif

#ifndef NO_SHA256
        case sha256_mac :
            Sha256Rounds(rounds, data, sz);
            break;
#endif

#ifdef WOLFSSL_SHA384
        case sha384_mac :
            Sha384Rounds(rounds, data, sz);
            break;
#endif

#ifdef WOLFSSL_SHA512
        case sha512_mac :
            Sha512Rounds(rounds, data, sz);
            break;
#endif

#ifdef WOLFSSL_RIPEMD
        case rmd_mac :
            RmdRounds(rounds, data, sz);
            break;
#endif

        default:
            WOLFSSL_MSG("Bad round type");
            break;
    }
}


/* do number of compression rounds on dummy data */
static INLINE void CompressRounds(WOLFSSL* ssl, int rounds, const byte* dummy)
{
    if (rounds)
        DoRounds(ssl->specs.mac_algorithm, rounds, dummy, COMPRESS_LOWER);
}


/* check all length bytes for the pad value, return 0 on success */
static int PadCheck(const byte* a, byte pad, int length)
{
    int i;
    int compareSum = 0;

    for (i = 0; i < length; i++) {
        compareSum |= a[i] ^ pad;
    }

    return compareSum;
}


/* get compression extra rounds */
static INLINE int GetRounds(int pLen, int padLen, int t)
{
    int  roundL1 = 1;  /* round up flags */
    int  roundL2 = 1;

    int L1 = COMPRESS_CONSTANT + pLen - t;
    int L2 = COMPRESS_CONSTANT + pLen - padLen - 1 - t;

    L1 -= COMPRESS_UPPER;
    L2 -= COMPRESS_UPPER;

    if ( (L1 % COMPRESS_LOWER) == 0)
        roundL1 = 0;
    if ( (L2 % COMPRESS_LOWER) == 0)
        roundL2 = 0;

    L1 /= COMPRESS_LOWER;
    L2 /= COMPRESS_LOWER;

    L1 += roundL1;
    L2 += roundL2;

    return L1 - L2;
}


/* timing resistant pad/verify check, return 0 on success */
static int TimingPadVerify(WOLFSSL* ssl, const byte* input, int padLen, int t,
                           int pLen, int content)
{
    byte verify[MAX_DIGEST_SIZE];
    byte dmy[sizeof(WOLFSSL) >= MAX_PAD_SIZE ? 1 : MAX_PAD_SIZE] = {0};
    byte* dummy = sizeof(dmy) < MAX_PAD_SIZE ? (byte*) ssl : dmy;
    int  ret = 0;

    (void)dmy;

    if ( (t + padLen + 1) > pLen) {
        WOLFSSL_MSG("Plain Len not long enough for pad/mac");
        PadCheck(dummy, (byte)padLen, MAX_PAD_SIZE);
        ssl->hmac(ssl, verify, input, pLen - t, content, 1); /* still compare */
        ConstantCompare(verify, input + pLen - t, t);

        return VERIFY_MAC_ERROR;
    }

    if (PadCheck(input + pLen - (padLen + 1), (byte)padLen, padLen + 1) != 0) {
        WOLFSSL_MSG("PadCheck failed");
        PadCheck(dummy, (byte)padLen, MAX_PAD_SIZE - padLen - 1);
        ssl->hmac(ssl, verify, input, pLen - t, content, 1); /* still compare */
        ConstantCompare(verify, input + pLen - t, t);

        return VERIFY_MAC_ERROR;
    }

    PadCheck(dummy, (byte)padLen, MAX_PAD_SIZE - padLen - 1);
    ret = ssl->hmac(ssl, verify, input, pLen - padLen - 1 - t, content, 1);

    CompressRounds(ssl, GetRounds(pLen, padLen, t), dummy);

    if (ConstantCompare(verify, input + (pLen - padLen - 1 - t), t) != 0) {
        WOLFSSL_MSG("Verify MAC compare failed");
        return VERIFY_MAC_ERROR;
    }

    /* treat any faulure as verify MAC error */
    if (ret != 0)
        ret = VERIFY_MAC_ERROR;

    return ret;
}


int DoApplicationData(WOLFSSL* ssl, byte* input, word32* inOutIdx)
{
    word32 msgSz   = ssl->keys.encryptSz;
    word32 idx     = *inOutIdx;
    int    dataSz;
    int    ivExtra = 0;
    byte*  rawData = input + idx;  /* keep current  for hmac */
#ifdef HAVE_LIBZ
    byte   decomp[MAX_RECORD_SIZE + MAX_COMP_EXTRA];
#endif

#ifdef WOLFSSL_EARLY_DATA
    if (ssl->earlyData) {
    }
    else
#endif
    if (ssl->options.handShakeDone == 0) {
        WOLFSSL_MSG("Received App data before a handshake completed");
        SendAlert(ssl, alert_fatal, unexpected_message);
        return OUT_OF_ORDER_E;
    }

    if (ssl->specs.cipher_type == block) {
        if (ssl->options.tls1_1)
            ivExtra = ssl->specs.block_size;
    }
    else if (ssl->specs.cipher_type == aead) {
        if (CipherHasExpIV(ssl))
            ivExtra = AESGCM_EXP_IV_SZ;
    }

    dataSz = msgSz - ivExtra - ssl->keys.padSz;
    if (dataSz < 0) {
        WOLFSSL_MSG("App data buffer error, malicious input?");
        return BUFFER_ERROR;
    }
#ifdef WOLFSSL_EARLY_DATA
    if (ssl->earlyData) {
        if (ssl->earlyDataSz + dataSz > ssl->options.maxEarlyDataSz) {
            SendAlert(ssl, alert_fatal, unexpected_message);
            return SSL_FATAL_ERROR;
        }
        ssl->earlyDataSz += dataSz;
    }
#endif

    /* read data */
    if (dataSz) {
        int rawSz = dataSz;       /* keep raw size for idx adjustment */

#ifdef HAVE_LIBZ
        if (ssl->options.usingCompression) {
            dataSz = myDeCompress(ssl, rawData, dataSz, decomp, sizeof(decomp));
            if (dataSz < 0) return dataSz;
        }
#endif
        idx += rawSz;

        ssl->buffers.clearOutputBuffer.buffer = rawData;
        ssl->buffers.clearOutputBuffer.length = dataSz;
    }

    idx += ssl->keys.padSz;

#ifdef HAVE_LIBZ
    /* decompress could be bigger, overwrite after verify */
    if (ssl->options.usingCompression)
        XMEMMOVE(rawData, decomp, dataSz);
#endif

    *inOutIdx = idx;
    return 0;
}


/* process alert, return level */
static int DoAlert(WOLFSSL* ssl, byte* input, word32* inOutIdx, int* type,
                   word32 totalSz)
{
    byte level;
    byte code;

    #ifdef WOLFSSL_CALLBACKS
        if (ssl->hsInfoOn)
            AddPacketName("Alert", &ssl->handShakeInfo);
        if (ssl->toInfoOn)
            /* add record header back on to info + alert bytes level/code */
            AddPacketInfo("Alert", &ssl->timeoutInfo, input + *inOutIdx -
                          RECORD_HEADER_SZ, RECORD_HEADER_SZ + ALERT_SIZE,
                          ssl->heap);
    #endif

    if (++ssl->options.alertCount >= WOLFSSL_ALERT_COUNT_MAX) {
        WOLFSSL_MSG("Alert count exceeded");
        return ALERT_COUNT_E;
    }

    /* make sure can read the message */
    if (*inOutIdx + ALERT_SIZE > totalSz)
        return BUFFER_E;

    level = input[(*inOutIdx)++];
    code  = input[(*inOutIdx)++];
    ssl->alert_history.last_rx.code = code;
    ssl->alert_history.last_rx.level = level;
    *type = code;
    if (level == alert_fatal) {
        ssl->options.isClosed = 1;  /* Don't send close_notify */
    }

    WOLFSSL_MSG("Got alert");
    if (*type == close_notify) {
        WOLFSSL_MSG("\tclose notify");
        ssl->options.closeNotify = 1;
    }
#ifdef WOLFSSL_TLS13
    if (*type == decode_error) {
        WOLFSSL_MSG("    decode error");
    }
    if (*type == illegal_parameter) {
        WOLFSSL_MSG("    illegal parameter");
    }
#endif
    WOLFSSL_ERROR(*type);
    if (IsEncryptionOn(ssl, 0)) {
        if (*inOutIdx + ssl->keys.padSz > totalSz)
            return BUFFER_E;
        *inOutIdx += ssl->keys.padSz;
    }

    return level;
}

static int GetInputData(WOLFSSL *ssl, word32 size)
{
    int in;
    int inSz;
    int maxLength;
    int usedLength;
    int dtlsExtra = 0;


    /* check max input length */
    usedLength = ssl->buffers.inputBuffer.length - ssl->buffers.inputBuffer.idx;
    maxLength  = ssl->buffers.inputBuffer.bufferSize - usedLength;
    inSz       = (int)(size - usedLength);      /* from last partial read */

#ifdef WOLFSSL_DTLS
    if (ssl->options.dtls) {
        if (size < ssl->dtls_expected_rx)
            dtlsExtra = (int)(ssl->dtls_expected_rx - size);
        inSz = ssl->dtls_expected_rx;
    }
#endif

    /* check that no lengths or size values are negative */
    if (usedLength < 0 || maxLength < 0 || inSz <= 0) {
        return BUFFER_ERROR;
    }

    if (inSz > maxLength) {
        if (GrowInputBuffer(ssl, size + dtlsExtra, usedLength) < 0)
            return MEMORY_E;
    }

    /* Put buffer data at start if not there */
    if (usedLength > 0 && ssl->buffers.inputBuffer.idx != 0)
        XMEMMOVE(ssl->buffers.inputBuffer.buffer,
                ssl->buffers.inputBuffer.buffer + ssl->buffers.inputBuffer.idx,
                usedLength);

    /* remove processed data */
    ssl->buffers.inputBuffer.idx    = 0;
    ssl->buffers.inputBuffer.length = usedLength;

    /* read data from network */
    do {
        in = Receive(ssl,
                     ssl->buffers.inputBuffer.buffer +
                     ssl->buffers.inputBuffer.length,
                     inSz);
        if (in == -1)
            return SOCKET_ERROR_E;

        if (in == WANT_READ)
            return WANT_READ;

        if (in > inSz)
            return RECV_OVERFLOW_E;

        ssl->buffers.inputBuffer.length += in;
        inSz -= in;

    } while (ssl->buffers.inputBuffer.length < size);

#ifdef WOLFSSL_DEBUG_TLS
    if (ssl->buffers.inputBuffer.idx == 0) {
        WOLFSSL_MSG("Data received");
        WOLFSSL_BUFFER(ssl->buffers.inputBuffer.buffer,
                       ssl->buffers.inputBuffer.length);
    }
#endif

    return 0;
}


static INLINE int VerifyMac(WOLFSSL* ssl, const byte* input, word32 msgSz,
                            int content, word32* padSz)
{
    int    ivExtra = 0;
    int    ret;
    word32 pad     = 0;
    word32 padByte = 0;
#ifdef HAVE_TRUNCATED_HMAC
    word32 digestSz = ssl->truncated_hmac ? (byte)TRUNCATED_HMAC_SZ
                                          : ssl->specs.hash_size;
#else
    word32 digestSz = ssl->specs.hash_size;
#endif
    byte   verify[MAX_DIGEST_SIZE];

    if (ssl->specs.cipher_type == block) {
        if (ssl->options.tls1_1)
            ivExtra = ssl->specs.block_size;
        pad = *(input + msgSz - ivExtra - 1);
        padByte = 1;

        if (ssl->options.tls) {
            ret = TimingPadVerify(ssl, input, pad, digestSz, msgSz - ivExtra,
                                  content);
            if (ret != 0)
                return ret;
        }
        else {  /* sslv3, some implementations have bad padding, but don't
                 * allow bad read */
            int  badPadLen = 0;
            byte dmy[sizeof(WOLFSSL) >= MAX_PAD_SIZE ? 1 : MAX_PAD_SIZE] = {0};
            byte* dummy = sizeof(dmy) < MAX_PAD_SIZE ? (byte*) ssl : dmy;

            (void)dmy;

            if (pad > (msgSz - digestSz - 1)) {
                WOLFSSL_MSG("Plain Len not long enough for pad/mac");
                pad       = 0;  /* no bad read */
                badPadLen = 1;
            }
            PadCheck(dummy, (byte)pad, MAX_PAD_SIZE);  /* timing only */
            ret = ssl->hmac(ssl, verify, input, msgSz - digestSz - pad - 1,
                            content, 1);
            if (ConstantCompare(verify, input + msgSz - digestSz - pad - 1,
                                digestSz) != 0)
                return VERIFY_MAC_ERROR;
            if (ret != 0 || badPadLen)
                return VERIFY_MAC_ERROR;
        }
    }
    else if (ssl->specs.cipher_type == stream) {
        ret = ssl->hmac(ssl, verify, input, msgSz - digestSz, content, 1);
        if (ConstantCompare(verify, input + msgSz - digestSz, digestSz) != 0){
            return VERIFY_MAC_ERROR;
        }
        if (ret != 0)
            return VERIFY_MAC_ERROR;
    }

    if (ssl->specs.cipher_type == aead) {
        *padSz = ssl->specs.aead_mac_size;
    }
    else {
        *padSz = digestSz + pad + padByte;
    }

    return 0;
}


/* process input requests, return 0 is done, 1 is call again to complete, and
   negative number is error */
int ProcessReply(WOLFSSL* ssl)
{
    int    ret = 0, type, readSz;
    int    atomicUser = 0;
    word32 startIdx = 0;
#if defined(WOLFSSL_DTLS)
    int    used;
#endif

#ifdef ATOMIC_USER
    if (ssl->ctx->DecryptVerifyCb)
        atomicUser = 1;
#endif

    if (ssl->error != 0 && ssl->error != WANT_READ &&
        ssl->error != WANT_WRITE && ssl->error != WC_PENDING_E) {
        WOLFSSL_MSG("ProcessReply retry in error state, not allowed");
        return ssl->error;
    }

    for (;;) {
        switch (ssl->options.processReply) {

        /* in the WOLFSSL_SERVER case, get the first byte for detecting
         * old client hello */
        case doProcessInit:

            readSz = RECORD_HEADER_SZ;

        #ifdef WOLFSSL_DTLS
            if (ssl->options.dtls)
                readSz = DTLS_RECORD_HEADER_SZ;
        #endif

            /* get header or return error */
            if (!ssl->options.dtls) {
                if ((ret = GetInputData(ssl, readSz)) < 0)
                    return ret;
            } else {
            #ifdef WOLFSSL_DTLS
                /* read ahead may already have header */
                used = ssl->buffers.inputBuffer.length -
                       ssl->buffers.inputBuffer.idx;
                if (used < readSz) {
                    if ((ret = GetInputData(ssl, readSz)) < 0)
                        return ret;
                }
            #endif
            }

#ifdef OLD_HELLO_ALLOWED

            /* see if sending SSLv2 client hello */
            if ( ssl->options.side == WOLFSSL_SERVER_END &&
                 ssl->options.clientState == NULL_STATE &&
                 ssl->buffers.inputBuffer.buffer[ssl->buffers.inputBuffer.idx]
                         != handshake) {
                byte b0, b1;

                ssl->options.processReply = runProcessOldClientHello;

                /* sanity checks before getting size at front */
                if (ssl->buffers.inputBuffer.buffer[
                          ssl->buffers.inputBuffer.idx + OPAQUE16_LEN] != OLD_HELLO_ID) {
                    WOLFSSL_MSG("Not a valid old client hello");
                    return PARSE_ERROR;
                }

                if (ssl->buffers.inputBuffer.buffer[
                          ssl->buffers.inputBuffer.idx + OPAQUE24_LEN] != SSLv3_MAJOR &&
                    ssl->buffers.inputBuffer.buffer[
                          ssl->buffers.inputBuffer.idx + OPAQUE24_LEN] != DTLS_MAJOR) {
                    WOLFSSL_MSG("Not a valid version in old client hello");
                    return PARSE_ERROR;
                }

                /* how many bytes need ProcessOldClientHello */
                b0 =
                ssl->buffers.inputBuffer.buffer[ssl->buffers.inputBuffer.idx++];
                b1 =
                ssl->buffers.inputBuffer.buffer[ssl->buffers.inputBuffer.idx++];
                ssl->curSize = (word16)(((b0 & 0x7f) << 8) | b1);
            }
            else {
                ssl->options.processReply = getRecordLayerHeader;
                continue;
            }
            FALL_THROUGH;

        /* in the WOLFSSL_SERVER case, run the old client hello */
        case runProcessOldClientHello:

            /* get sz bytes or return error */
            if (!ssl->options.dtls) {
                if ((ret = GetInputData(ssl, ssl->curSize)) < 0)
                    return ret;
            } else {
            #ifdef WOLFSSL_DTLS
                /* read ahead may already have */
                used = ssl->buffers.inputBuffer.length -
                       ssl->buffers.inputBuffer.idx;
                if (used < ssl->curSize)
                    if ((ret = GetInputData(ssl, ssl->curSize)) < 0)
                        return ret;
            #endif  /* WOLFSSL_DTLS */
            }

            ret = ProcessOldClientHello(ssl, ssl->buffers.inputBuffer.buffer,
                                        &ssl->buffers.inputBuffer.idx,
                                        ssl->buffers.inputBuffer.length -
                                        ssl->buffers.inputBuffer.idx,
                                        ssl->curSize);
            if (ret < 0)
                return ret;

            else if (ssl->buffers.inputBuffer.idx ==
                     ssl->buffers.inputBuffer.length) {
                ssl->options.processReply = doProcessInit;
                return 0;
            }

#endif  /* OLD_HELLO_ALLOWED */
            FALL_THROUGH;

        /* get the record layer header */
        case getRecordLayerHeader:

            ret = GetRecordHeader(ssl, ssl->buffers.inputBuffer.buffer,
                                       &ssl->buffers.inputBuffer.idx,
                                       &ssl->curRL, &ssl->curSize);
#ifdef WOLFSSL_DTLS
            if (ssl->options.dtls && ret == SEQUENCE_ERROR) {
                WOLFSSL_MSG("Silently dropping out of order DTLS message");
                ssl->options.processReply = doProcessInit;
                ssl->buffers.inputBuffer.length = 0;
                ssl->buffers.inputBuffer.idx = 0;
#ifdef WOLFSSL_DTLS_DROP_STATS
                ssl->replayDropCount++;
#endif /* WOLFSSL_DTLS_DROP_STATS */

                if (IsDtlsNotSctpMode(ssl) && ssl->options.dtlsHsRetain) {
                    ret = DtlsMsgPoolSend(ssl, 0);
                    if (ret != 0)
                        return ret;
                }

                continue;
            }
#endif
            if (ret != 0)
                return ret;

            ssl->options.processReply = getData;
            FALL_THROUGH;

        /* retrieve record layer data */
        case getData:

            /* get sz bytes or return error */
            if (!ssl->options.dtls) {
                if ((ret = GetInputData(ssl, ssl->curSize)) < 0)
                    return ret;
            } else {
#ifdef WOLFSSL_DTLS
                /* read ahead may already have */
                used = ssl->buffers.inputBuffer.length -
                       ssl->buffers.inputBuffer.idx;
                if (used < ssl->curSize)
                    if ((ret = GetInputData(ssl, ssl->curSize)) < 0)
                        return ret;
#endif
            }

            ssl->options.processReply = decryptMessage;
            startIdx = ssl->buffers.inputBuffer.idx;  /* in case > 1 msg per */
            FALL_THROUGH;

        /* decrypt message */
        case decryptMessage:

            if (IsEncryptionOn(ssl, 0) && ssl->keys.decryptedCur == 0) {
                bufferStatic* in = &ssl->buffers.inputBuffer;

                ret = SanityCheckCipherText(ssl, ssl->curSize);
                if (ret < 0)
                    return ret;

                if (atomicUser) {
                #ifdef ATOMIC_USER
                    ret = ssl->ctx->DecryptVerifyCb(ssl,
                                  in->buffer + in->idx,
                                  in->buffer + in->idx,
                                  ssl->curSize, ssl->curRL.type, 1,
                                  &ssl->keys.padSz, ssl->DecryptVerifyCtx);
                #endif /* ATOMIC_USER */
                }
                else {
                    if (!ssl->options.tls1_3) {
                        ret = Decrypt(ssl,
                                      in->buffer + in->idx,
                                      in->buffer + in->idx,
                                      ssl->curSize);
                    }
                    else {
                    #ifdef WOLFSSL_TLS13
                        ret = DecryptTls13(ssl,
                                           in->buffer + in->idx,
                                           in->buffer + in->idx,
                                           ssl->curSize);
                    #else
                        ret = DECRYPT_ERROR;
                    #endif /* WOLFSSL_TLS13 */
                    }
                }

            #ifdef WOLFSSL_ASYNC_CRYPT
                if (ret == WC_PENDING_E)
                    return ret;
            #endif

                if (ret >= 0) {
                    /* handle success */
                    if (ssl->options.tls1_1 && ssl->specs.cipher_type == block)
                        ssl->buffers.inputBuffer.idx += ssl->specs.block_size;
                        /* go past TLSv1.1 IV */
                    if (CipherHasExpIV(ssl))
                        ssl->buffers.inputBuffer.idx += AESGCM_EXP_IV_SZ;
                }
                else {
                    WOLFSSL_MSG("Decrypt failed");
                    WOLFSSL_ERROR(ret);
                #ifdef WOLFSSL_EARLY_DATA
                    if (ssl->options.tls1_3) {
                        ssl->earlyDataSz += ssl->curSize;
                        if (ssl->earlyDataSz <= ssl->options.maxEarlyDataSz) {
                            if (ssl->keys.peer_sequence_number_lo-- == 0)
                                ssl->keys.peer_sequence_number_hi--;
                            ssl->options.processReply = doProcessInit;
                            ssl->buffers.inputBuffer.idx =
                                            ssl->buffers.inputBuffer.length;
                            return 0;
                        }
                    }
                #endif
                #ifdef WOLFSSL_DTLS
                    /* If in DTLS mode, if the decrypt fails for any
                     * reason, pretend the datagram never happened. */
                    if (ssl->options.dtls) {
                        ssl->options.processReply = doProcessInit;
                        ssl->buffers.inputBuffer.idx =
                                        ssl->buffers.inputBuffer.length;
                        #ifdef WOLFSSL_DTLS_DROP_STATS
                            ssl->macDropCount++;
                        #endif /* WOLFSSL_DTLS_DROP_STATS */
                    }
                #endif /* WOLFSSL_DTLS */

                    return DECRYPT_ERROR;
                }
            }

            ssl->options.processReply = verifyMessage;
            FALL_THROUGH;

        /* verify digest of message */
        case verifyMessage:

            if (IsEncryptionOn(ssl, 0) && ssl->keys.decryptedCur == 0) {
                if (!atomicUser) {
                    ret = VerifyMac(ssl, ssl->buffers.inputBuffer.buffer +
                                    ssl->buffers.inputBuffer.idx,
                                    ssl->curSize, ssl->curRL.type,
                                    &ssl->keys.padSz);
                #ifdef WOLFSSL_ASYNC_CRYPT
                    if (ret == WC_PENDING_E)
                        return ret;
                #endif
                    if (ret < 0) {
                        WOLFSSL_MSG("VerifyMac failed");
                        WOLFSSL_ERROR(ret);
                        #ifdef WOLFSSL_DTLS
                        /* If in DTLS mode, if the decrypt fails for any
                         * reason, pretend the datagram never happened. */
                        if (ssl->options.dtls) {
                            ssl->options.processReply = doProcessInit;
                            ssl->buffers.inputBuffer.idx =
                                            ssl->buffers.inputBuffer.length;
                            #ifdef WOLFSSL_DTLS_DROP_STATS
                                ssl->macDropCount++;
                            #endif /* WOLFSSL_DTLS_DROP_STATS */
                        }
                        #endif /* WOLFSSL_DTLS */
                        return DECRYPT_ERROR;
                    }
                }

                ssl->keys.encryptSz    = ssl->curSize;
                ssl->keys.decryptedCur = 1;
#ifdef WOLFSSL_TLS13
                if (ssl->options.tls1_3) {
                    /* Get the real content type from the end of the data. */
                    ssl->keys.padSz++;
                    ssl->curRL.type = ssl->buffers.inputBuffer.buffer[
                        ssl->buffers.inputBuffer.length - ssl->keys.padSz];
                }
#endif
            }

            ssl->options.processReply = runProcessingOneMessage;
            FALL_THROUGH;

        /* the record layer is here */
        case runProcessingOneMessage:

        #ifdef WOLFSSL_DTLS
            if (IsDtlsNotSctpMode(ssl)) {
                DtlsUpdateWindow(ssl);
            }
        #endif /* WOLFSSL_DTLS */

            WOLFSSL_MSG("received record layer msg");

            switch (ssl->curRL.type) {
                case handshake :
                    /* debugging in DoHandShakeMsg */
                    if (ssl->options.dtls) {
#ifdef WOLFSSL_DTLS
                        ret = DoDtlsHandShakeMsg(ssl,
                                            ssl->buffers.inputBuffer.buffer,
                                            &ssl->buffers.inputBuffer.idx,
                                            ssl->buffers.inputBuffer.length);
#endif
                    }
                    else if (!IsAtLeastTLSv1_3(ssl->version)) {
                        ret = DoHandShakeMsg(ssl,
                                            ssl->buffers.inputBuffer.buffer,
                                            &ssl->buffers.inputBuffer.idx,
                                            ssl->buffers.inputBuffer.length);
                    }
                    else {
#ifdef WOLFSSL_TLS13
                        ret = DoTls13HandShakeMsg(ssl,
                                            ssl->buffers.inputBuffer.buffer,
                                            &ssl->buffers.inputBuffer.idx,
                                            ssl->buffers.inputBuffer.length);
    #ifdef WOLFSSL_EARLY_DATA
                        if (ret != 0)
                            return ret;
                        if (ssl->options.side == WOLFSSL_SERVER_END &&
                                ssl->earlyData &&
                                ssl->options.handShakeState == HANDSHAKE_DONE) {
                            ssl->earlyData = 0;
                            ssl->options.processReply = doProcessInit;
                            return ZERO_RETURN;
                        }
    #endif
#else
                        ret = BUFFER_ERROR;
#endif
                    }
                    if (ret != 0)
                        return ret;
                    break;

                case change_cipher_spec:
                    WOLFSSL_MSG("got CHANGE CIPHER SPEC");
                    #ifdef WOLFSSL_CALLBACKS
                        if (ssl->hsInfoOn)
                            AddPacketName("ChangeCipher", &ssl->handShakeInfo);
                        /* add record header back on info */
                        if (ssl->toInfoOn) {
                            AddPacketInfo("ChangeCipher", &ssl->timeoutInfo,
                                ssl->buffers.inputBuffer.buffer +
                                ssl->buffers.inputBuffer.idx - RECORD_HEADER_SZ,
                                1 + RECORD_HEADER_SZ, ssl->heap);
                            AddLateRecordHeader(&ssl->curRL, &ssl->timeoutInfo);
                        }
                    #endif

                    ret = SanityCheckMsgReceived(ssl, change_cipher_hs);
                    if (ret != 0) {
                        if (!ssl->options.dtls) {
                            return ret;
                        }
                        else {
                        #ifdef WOLFSSL_DTLS
                        /* Check for duplicate CCS message in DTLS mode.
                         * DTLS allows for duplicate messages, and it should be
                         * skipped. Also skip if out of order. */
                            if (ret != DUPLICATE_MSG_E && ret != OUT_OF_ORDER_E)
                                return ret;

                            if (IsDtlsNotSctpMode(ssl)) {
                                ret = DtlsMsgPoolSend(ssl, 1);
                                if (ret != 0)
                                    return ret;
                            }

                            if (ssl->curSize != 1) {
                                WOLFSSL_MSG("Malicious or corrupted"
                                            " duplicate ChangeCipher msg");
                                return LENGTH_ERROR;
                            }
                            ssl->buffers.inputBuffer.idx++;
                            break;
                        #endif /* WOLFSSL_DTLS */
                        }
                    }

                    if (IsEncryptionOn(ssl, 0) && ssl->options.handShakeDone) {
                        ssl->buffers.inputBuffer.idx += ssl->keys.padSz;
                        ssl->curSize -= (word16) ssl->buffers.inputBuffer.idx;
                    }

                    if (ssl->curSize != 1) {
                        WOLFSSL_MSG("Malicious or corrupted ChangeCipher msg");
                        return LENGTH_ERROR;
                    }

                    ssl->buffers.inputBuffer.idx++;
                    ssl->keys.encryptionOn = 1;

                    /* setup decrypt keys for following messages */
                    /* XXX This might not be what we want to do when
                     * receiving a CCS with multicast. We update the
                     * key when the application updates them. */
                    if ((ret = SetKeysSide(ssl, DECRYPT_SIDE_ONLY)) != 0)
                        return ret;

                    #ifdef WOLFSSL_DTLS
                        if (ssl->options.dtls) {
                            WOLFSSL_DTLS_PEERSEQ* peerSeq = ssl->keys.peerSeq;
#ifdef WOLFSSL_MULTICAST
                            if (ssl->options.haveMcast) {
                                peerSeq += ssl->keys.curPeerId;
                                peerSeq->highwaterMark = UpdateHighwaterMark(0,
                                        ssl->ctx->mcastFirstSeq,
                                        ssl->ctx->mcastSecondSeq,
                                        ssl->ctx->mcastMaxSeq);
                            }
#endif
                            DtlsMsgPoolReset(ssl);
                            peerSeq->nextEpoch++;
                            peerSeq->prevSeq_lo = peerSeq->nextSeq_lo;
                            peerSeq->prevSeq_hi = peerSeq->nextSeq_hi;
                            peerSeq->nextSeq_lo = 0;
                            peerSeq->nextSeq_hi = 0;
                            XMEMCPY(peerSeq->prevWindow, peerSeq->window,
                                    DTLS_SEQ_SZ);
                            XMEMSET(peerSeq->window, 0, DTLS_SEQ_SZ);
                        }
                    #endif

                    #ifdef HAVE_LIBZ
                        if (ssl->options.usingCompression)
                            if ( (ret = InitStreams(ssl)) != 0)
                                return ret;
                    #endif
                    ret = BuildFinished(ssl, &ssl->hsHashes->verifyHashes,
                                       ssl->options.side == WOLFSSL_CLIENT_END ?
                                       server : client);
                    if (ret != 0)
                        return ret;
                    break;

                case application_data:
                    WOLFSSL_MSG("got app DATA");
                    #ifdef WOLFSSL_DTLS
                        if (ssl->options.dtls && ssl->options.dtlsHsRetain) {
                            FreeHandshakeResources(ssl);
                            ssl->options.dtlsHsRetain = 0;
                        }
                    #endif
                    #ifdef WOLFSSL_TLS13
                        if (ssl->keys.keyUpdateRespond) {
                            WOLFSSL_MSG("No KeyUpdate from peer seen");
                            return SANITY_MSG_E;
                        }
                    #endif
                    if ((ret = DoApplicationData(ssl,
                                                ssl->buffers.inputBuffer.buffer,
                                               &ssl->buffers.inputBuffer.idx))
                                                                         != 0) {
                        WOLFSSL_ERROR(ret);
                        return ret;
                    }
                    break;

                case alert:
                    WOLFSSL_MSG("got ALERT!");
                    ret = DoAlert(ssl, ssl->buffers.inputBuffer.buffer,
                                  &ssl->buffers.inputBuffer.idx, &type,
                                   ssl->buffers.inputBuffer.length);
                    if (ret == alert_fatal)
                        return FATAL_ERROR;
                    else if (ret < 0)
                        return ret;

                    /* catch warnings that are handled as errors */
                    if (type == close_notify)
                        return ssl->error = ZERO_RETURN;

                    if (type == decrypt_error)
                        return FATAL_ERROR;
                    break;

                default:
                    WOLFSSL_ERROR(UNKNOWN_RECORD_TYPE);
                    return UNKNOWN_RECORD_TYPE;
            }

            ssl->options.processReply = doProcessInit;

            /* input exhausted? */
            if (ssl->buffers.inputBuffer.idx >= ssl->buffers.inputBuffer.length)
                return 0;

            /* more messages per record */
            else if ((ssl->buffers.inputBuffer.idx - startIdx) < ssl->curSize) {
                WOLFSSL_MSG("More messages in record");

                ssl->options.processReply = runProcessingOneMessage;

                if (IsEncryptionOn(ssl, 0)) {
                    WOLFSSL_MSG("Bundled encrypted messages, remove middle pad");
                    if (ssl->buffers.inputBuffer.idx >= ssl->keys.padSz) {
                        ssl->buffers.inputBuffer.idx -= ssl->keys.padSz;
                    }
                    else {
                        WOLFSSL_MSG("\tmiddle padding error");
                        return FATAL_ERROR;
                    }
                }

                continue;
            }
            /* more records */
            else {
                WOLFSSL_MSG("More records in input");
                ssl->options.processReply = doProcessInit;
                continue;
            }

        default:
            WOLFSSL_MSG("Bad process input state, programming error");
            return INPUT_CASE_ERROR;
        }
    }
}


int SendChangeCipher(WOLFSSL* ssl)
{
    byte              *output;
    int                sendSz = RECORD_HEADER_SZ + ENUM_LEN;
    int                idx    = RECORD_HEADER_SZ;
    int                ret;

    #ifdef WOLFSSL_DTLS
        if (ssl->options.dtls) {
            sendSz += DTLS_RECORD_EXTRA;
            idx    += DTLS_RECORD_EXTRA;
        }
    #endif

    /* are we in scr */
    if (IsEncryptionOn(ssl, 1) && ssl->options.handShakeDone) {
        sendSz += MAX_MSG_EXTRA;
    }

    /* check for avalaible size */
    if ((ret = CheckAvailableSize(ssl, sendSz)) != 0)
        return ret;

    /* get output buffer */
    output = ssl->buffers.outputBuffer.buffer +
             ssl->buffers.outputBuffer.length;

    AddRecordHeader(output, 1, change_cipher_spec, ssl);

    output[idx] = 1;             /* turn it on */

    if (IsEncryptionOn(ssl, 1) && ssl->options.handShakeDone) {
        byte input[ENUM_LEN];
        int  inputSz = ENUM_LEN;

        input[0] = 1;  /* turn it on */
        sendSz = BuildMessage(ssl, output, sendSz, input, inputSz,
                              change_cipher_spec, 0, 0, 0);
        if (sendSz < 0) {
            return sendSz;
        }
    }

    #ifdef WOLFSSL_DTLS
        if (IsDtlsNotSctpMode(ssl)) {
            if ((ret = DtlsMsgPoolSave(ssl, output, sendSz)) != 0)
                return ret;
        }
    #endif
    #ifdef WOLFSSL_CALLBACKS
        if (ssl->hsInfoOn) AddPacketName("ChangeCipher", &ssl->handShakeInfo);
        if (ssl->toInfoOn)
            AddPacketInfo("ChangeCipher", &ssl->timeoutInfo, output, sendSz,
                           ssl->heap);
    #endif
    ssl->buffers.outputBuffer.length += sendSz;

    if (ssl->options.groupMessages)
        return 0;
    #if defined(WOLFSSL_DTLS) && !defined(WOLFSSL_DEBUG_DTLS)
    else if (ssl->options.dtls) {
        /* If using DTLS, force the ChangeCipherSpec message to be in the
         * same datagram as the finished message. */
        return 0;
    }
    #endif
    else
        return SendBuffered(ssl);
}


#ifndef NO_OLD_TLS
static int SSL_hmac(WOLFSSL* ssl, byte* digest, const byte* in, word32 sz,
                 int content, int verify)
{
    byte   result[MAX_DIGEST_SIZE];
    word32 digestSz = ssl->specs.hash_size;            /* actual sizes */
    word32 padSz    = ssl->specs.pad_size;
    int    ret      = 0;

    Md5 md5;
    Sha sha;

    /* data */
    byte seq[SEQ_SZ];
    byte conLen[ENUM_LEN + LENGTH_SZ];     /* content & length */
    const byte* macSecret = wolfSSL_GetMacSecret(ssl, verify);

#ifdef HAVE_FUZZER
    if (ssl->fuzzerCb)
        ssl->fuzzerCb(ssl, in, sz, FUZZ_HMAC, ssl->fuzzerCtx);
#endif

    XMEMSET(seq, 0, SEQ_SZ);
    conLen[0] = (byte)content;
    c16toa((word16)sz, &conLen[ENUM_LEN]);
    WriteSEQ(ssl, verify, seq);

    if (ssl->specs.mac_algorithm == md5_mac) {
        ret =  wc_InitMd5_ex(&md5, ssl->heap, ssl->devId);
        if (ret != 0)
            return ret;

        /* inner */
        ret =  wc_Md5Update(&md5, macSecret, digestSz);
        ret |= wc_Md5Update(&md5, PAD1, padSz);
        ret |= wc_Md5Update(&md5, seq, SEQ_SZ);
        ret |= wc_Md5Update(&md5, conLen, sizeof(conLen));
        /* in buffer */
        ret |= wc_Md5Update(&md5, in, sz);
        if (ret != 0)
            return VERIFY_MAC_ERROR;
        ret = wc_Md5Final(&md5, result);
    #ifdef WOLFSSL_ASYNC_CRYPT
        /* TODO: Make non-blocking */
        if (ret == WC_PENDING_E) {
            ret = wc_AsyncWait(ret, &md5.asyncDev, WC_ASYNC_FLAG_NONE);
        }
    #endif
        if (ret != 0)
            return VERIFY_MAC_ERROR;

        /* outer */
        ret =  wc_Md5Update(&md5, macSecret, digestSz);
        ret |= wc_Md5Update(&md5, PAD2, padSz);
        ret |= wc_Md5Update(&md5, result, digestSz);
        if (ret != 0)
            return VERIFY_MAC_ERROR;
        ret =  wc_Md5Final(&md5, digest);
    #ifdef WOLFSSL_ASYNC_CRYPT
        /* TODO: Make non-blocking */
        if (ret == WC_PENDING_E) {
            ret = wc_AsyncWait(ret, &md5.asyncDev, WC_ASYNC_FLAG_NONE);
        }
    #endif
        if (ret != 0)
            return VERIFY_MAC_ERROR;

        wc_Md5Free(&md5);
    }
    else {
        ret =  wc_InitSha_ex(&sha, ssl->heap, ssl->devId);
        if (ret != 0)
            return ret;

        /* inner */
        ret =  wc_ShaUpdate(&sha, macSecret, digestSz);
        ret |= wc_ShaUpdate(&sha, PAD1, padSz);
        ret |= wc_ShaUpdate(&sha, seq, SEQ_SZ);
        ret |= wc_ShaUpdate(&sha, conLen, sizeof(conLen));
        /* in buffer */
        ret |= wc_ShaUpdate(&sha, in, sz);
        if (ret != 0)
            return VERIFY_MAC_ERROR;
        ret = wc_ShaFinal(&sha, result);
    #ifdef WOLFSSL_ASYNC_CRYPT
        /* TODO: Make non-blocking */
        if (ret == WC_PENDING_E) {
            ret = wc_AsyncWait(ret, &sha.asyncDev, WC_ASYNC_FLAG_NONE);
        }
    #endif
        if (ret != 0)
            return VERIFY_MAC_ERROR;

        /* outer */
        ret =  wc_ShaUpdate(&sha, macSecret, digestSz);
        ret |= wc_ShaUpdate(&sha, PAD2, padSz);
        ret |= wc_ShaUpdate(&sha, result, digestSz);
        if (ret != 0)
            return VERIFY_MAC_ERROR;
        ret =  wc_ShaFinal(&sha, digest);
    #ifdef WOLFSSL_ASYNC_CRYPT
        /* TODO: Make non-blocking */
        if (ret == WC_PENDING_E) {
            ret = wc_AsyncWait(ret, &sha.asyncDev, WC_ASYNC_FLAG_NONE);
        }
    #endif
        if (ret != 0)
            return VERIFY_MAC_ERROR;

        wc_ShaFree(&sha);
    }
    return 0;
}
#endif /* NO_OLD_TLS */


#ifndef NO_CERTS

#if !defined(NO_MD5) && !defined(NO_OLD_TLS)
static int BuildMD5_CertVerify(WOLFSSL* ssl, byte* digest)
{
    int ret;
    byte md5_result[MD5_DIGEST_SIZE];
#ifdef WOLFSSL_SMALL_STACK
    Md5* md5 = (Md5*)XMALLOC(sizeof(Md5), ssl->heap, DYNAMIC_TYPE_HASHCTX);
#else
    Md5  md5[1];
#endif

    /* make md5 inner */
    ret = wc_Md5Copy(&ssl->hsHashes->hashMd5, md5); /* Save current position */
    if (ret == 0)
        ret = wc_Md5Update(md5, ssl->arrays->masterSecret,SECRET_LEN);
    if (ret == 0)
        ret = wc_Md5Update(md5, PAD1, PAD_MD5);
    if (ret == 0)
        ret = wc_Md5Final(md5, md5_result);

    /* make md5 outer */
    if (ret == 0) {
        ret = wc_InitMd5_ex(md5, ssl->heap, ssl->devId);
        if (ret == 0) {
            ret = wc_Md5Update(md5, ssl->arrays->masterSecret, SECRET_LEN);
            if (ret == 0)
                ret = wc_Md5Update(md5, PAD2, PAD_MD5);
            if (ret == 0)
                ret = wc_Md5Update(md5, md5_result, MD5_DIGEST_SIZE);
            if (ret == 0)
                ret = wc_Md5Final(md5, digest);
            wc_Md5Free(md5);
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(md5, ssl->heap, DYNAMIC_TYPE_HASHCTX);
#endif

    return ret;
}
#endif /* !NO_MD5 && !NO_OLD_TLS */

#if !defined(NO_SHA) && (!defined(NO_OLD_TLS) || \
                              defined(WOLFSSL_ALLOW_TLS_SHA1))
static int BuildSHA_CertVerify(WOLFSSL* ssl, byte* digest)
{
    int ret;
    byte sha_result[SHA_DIGEST_SIZE];
#ifdef WOLFSSL_SMALL_STACK
    Sha* sha = (Sha*)XMALLOC(sizeof(Sha), ssl->heap, DYNAMIC_TYPE_HASHCTX);
#else
    Sha  sha[1];
#endif

    /* make sha inner */
    ret = wc_ShaCopy(&ssl->hsHashes->hashSha, sha); /* Save current position */
    if (ret == 0)
        ret = wc_ShaUpdate(sha, ssl->arrays->masterSecret,SECRET_LEN);
    if (ret == 0)
        ret = wc_ShaUpdate(sha, PAD1, PAD_SHA);
    if (ret == 0)
        ret = wc_ShaFinal(sha, sha_result);

    /* make sha outer */
    if (ret == 0) {
        ret = wc_InitSha_ex(sha, ssl->heap, ssl->devId);
        if (ret == 0) {
            ret = wc_ShaUpdate(sha, ssl->arrays->masterSecret,SECRET_LEN);
            if (ret == 0)
                ret = wc_ShaUpdate(sha, PAD2, PAD_SHA);
            if (ret == 0)
                ret = wc_ShaUpdate(sha, sha_result, SHA_DIGEST_SIZE);
            if (ret == 0)
                ret = wc_ShaFinal(sha, digest);
            wc_ShaFree(sha);
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(sha, ssl->heap, DYNAMIC_TYPE_HASHCTX);
#endif

    return ret;
}
#endif /* !NO_SHA && (!NO_OLD_TLS || WOLFSSL_ALLOW_TLS_SHA1) */

int BuildCertHashes(WOLFSSL* ssl, Hashes* hashes)
{
    int ret = 0;

    (void)hashes;

    if (ssl->options.tls) {
    #if !defined(NO_MD5) && !defined(NO_OLD_TLS)
        ret = wc_Md5GetHash(&ssl->hsHashes->hashMd5, hashes->md5);
        if (ret != 0)
            return ret;
    #endif
    #if !defined(NO_SHA)
        ret = wc_ShaGetHash(&ssl->hsHashes->hashSha, hashes->sha);
        if (ret != 0)
            return ret;
    #endif
        if (IsAtLeastTLSv1_2(ssl)) {
            #ifndef NO_SHA256
                ret = wc_Sha256GetHash(&ssl->hsHashes->hashSha256,
                                       hashes->sha256);
                if (ret != 0)
                    return ret;
            #endif
            #ifdef WOLFSSL_SHA384
                ret = wc_Sha384GetHash(&ssl->hsHashes->hashSha384,
                                       hashes->sha384);
                if (ret != 0)
                    return ret;
            #endif
            #ifdef WOLFSSL_SHA512
                ret = wc_Sha512GetHash(&ssl->hsHashes->hashSha512,
                                       hashes->sha512);
                if (ret != 0)
                    return ret;
            #endif
        }
    }
    else {
    #if !defined(NO_MD5) && !defined(NO_OLD_TLS)
        ret = BuildMD5_CertVerify(ssl, hashes->md5);
        if (ret != 0)
            return ret;
    #endif
    #if !defined(NO_SHA) && (!defined(NO_OLD_TLS) || \
                              defined(WOLFSSL_ALLOW_TLS_SHA1))
        ret = BuildSHA_CertVerify(ssl, hashes->sha);
        if (ret != 0)
            return ret;
    #endif
    }

    return ret;
}

#endif /* WOLFSSL_LEANPSK */

/* Persistable BuildMessage arguments */
typedef struct BuildMsgArgs {
    word32 digestSz;
    word32 sz;
    word32 pad;
    word32 idx;
    word32 headerSz;
    word16 size;
    word32 ivSz;      /* TLSv1.1  IV */
    byte   iv[AES_BLOCK_SIZE]; /* max size */
} BuildMsgArgs;

static void FreeBuildMsgArgs(WOLFSSL* ssl, void* pArgs)
{
    BuildMsgArgs* args = (BuildMsgArgs*)pArgs;

    (void)ssl;
    (void)args;

    /* no allocations in BuildMessage */
}

/* Build SSL Message, encrypted */
int BuildMessage(WOLFSSL* ssl, byte* output, int outSz, const byte* input,
             int inSz, int type, int hashOutput, int sizeOnly, int asyncOkay)
{
    int ret = 0;
    BuildMsgArgs* args;
    BuildMsgArgs  lcl_args;
#ifdef WOLFSSL_ASYNC_CRYPT
    args = (BuildMsgArgs*)ssl->async.args;
    typedef char args_test[sizeof(ssl->async.args) >= sizeof(*args) ? 1 : -1];
    (void)sizeof(args_test);
#endif

    WOLFSSL_ENTER("BuildMessage");

    if (ssl == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef WOLFSSL_TLS13
    if (ssl->options.tls1_3) {
        return BuildTls13Message(ssl, output, outSz, input, inSz, type,
                                 hashOutput, sizeOnly, asyncOkay);
    }
#endif

    ret = WC_NOT_PENDING_E;
#ifdef WOLFSSL_ASYNC_CRYPT
    if (asyncOkay) {
        ret = wolfSSL_AsyncPop(ssl, &ssl->options.buildMsgState);
        if (ret != WC_NOT_PENDING_E) {
            /* Check for error */
            if (ret < 0)
                goto exit_buildmsg;
        }
    }
    else
#endif
    {
        args = &lcl_args;
    }

    /* Reset state */
    if (ret == WC_NOT_PENDING_E) {
        ret = 0;
        ssl->options.buildMsgState = BUILD_MSG_BEGIN;
        XMEMSET(args, 0, sizeof(BuildMsgArgs));

        args->sz = RECORD_HEADER_SZ + inSz;
        args->idx  = RECORD_HEADER_SZ;
        args->headerSz = RECORD_HEADER_SZ;
    #ifdef WOLFSSL_ASYNC_CRYPT
        ssl->async.freeArgs = FreeBuildMsgArgs;
    #endif
    }

    switch (ssl->options.buildMsgState) {
        case BUILD_MSG_BEGIN:
        {
            /* catch mistaken sizeOnly parameter */
            if (!sizeOnly && (output == NULL || input == NULL) ) {
                return BAD_FUNC_ARG;
            }
            if (sizeOnly && (output || input) ) {
                WOLFSSL_MSG("BuildMessage w/sizeOnly doesn't need input/output");
                return BAD_FUNC_ARG;
            }

            ssl->options.buildMsgState = BUILD_MSG_SIZE;
        }
        FALL_THROUGH;
        case BUILD_MSG_SIZE:
        {
            args->digestSz = ssl->specs.hash_size;
        #ifdef HAVE_TRUNCATED_HMAC
            if (ssl->truncated_hmac)
                args->digestSz = min(TRUNCATED_HMAC_SZ, args->digestSz);
        #endif
            args->sz += args->digestSz;

        #ifdef WOLFSSL_DTLS
            if (ssl->options.dtls) {
                args->sz       += DTLS_RECORD_EXTRA;
                args->idx      += DTLS_RECORD_EXTRA;
                args->headerSz += DTLS_RECORD_EXTRA;
            }
        #endif

            if (ssl->specs.cipher_type == block) {
                word32 blockSz = ssl->specs.block_size;
                if (ssl->options.tls1_1) {
                    args->ivSz = blockSz;
                    args->sz  += args->ivSz;

                    if (args->ivSz > (word32)sizeof(args->iv))
                        ERROR_OUT(BUFFER_E, exit_buildmsg);
                }
                args->sz += 1;       /* pad byte */
                args->pad = (args->sz - args->headerSz) % blockSz;
                args->pad = blockSz - args->pad;
                args->sz += args->pad;
            }

        #ifdef HAVE_AEAD
            if (ssl->specs.cipher_type == aead) {
                if (ssl->specs.bulk_cipher_algorithm != wolfssl_chacha)
                    args->ivSz = AESGCM_EXP_IV_SZ;

                args->sz += (args->ivSz + ssl->specs.aead_mac_size - args->digestSz);
            }
        #endif

            /* done with size calculations */
            if (sizeOnly)
                goto exit_buildmsg;

            if (args->sz > (word32)outSz) {
                WOLFSSL_MSG("Oops, want to write past output buffer size");
                ERROR_OUT(BUFFER_E, exit_buildmsg);
            }

            if (args->ivSz > 0) {
                ret = wc_RNG_GenerateBlock(ssl->rng, args->iv, args->ivSz);
                if (ret != 0)
                    goto exit_buildmsg;

            }

        #ifdef HAVE_AEAD
            if (ssl->specs.cipher_type == aead) {
                if (ssl->specs.bulk_cipher_algorithm != wolfssl_chacha)
                    XMEMCPY(args->iv, ssl->keys.aead_exp_IV, AESGCM_EXP_IV_SZ);
            }
        #endif

            args->size = (word16)(args->sz - args->headerSz);    /* include mac and digest */
            AddRecordHeader(output, args->size, (byte)type, ssl);

            /* write to output */
            if (args->ivSz) {
                XMEMCPY(output + args->idx, args->iv,
                                        min(args->ivSz, sizeof(args->iv)));
                args->idx += args->ivSz;
            }
            XMEMCPY(output + args->idx, input, inSz);
            args->idx += inSz;

            ssl->options.buildMsgState = BUILD_MSG_HASH;
        }
        FALL_THROUGH;
        case BUILD_MSG_HASH:
        {
            word32 i;

            if (type == handshake && hashOutput) {
                ret = HashOutput(ssl, output, args->headerSz + inSz, args->ivSz);
                if (ret != 0)
                    goto exit_buildmsg;
            }
            if (ssl->specs.cipher_type == block) {
                word32 tmpIdx = args->idx + args->digestSz;

                for (i = 0; i <= args->pad; i++)
                    output[tmpIdx++] = (byte)args->pad; /* pad byte gets pad value */
            }

            ssl->options.buildMsgState = BUILD_MSG_VERIFY_MAC;
        }
        FALL_THROUGH;
        case BUILD_MSG_VERIFY_MAC:
        {
            /* User Record Layer Callback handling */
        #ifdef ATOMIC_USER
            if (ssl->ctx->MacEncryptCb) {
                ret = ssl->ctx->MacEncryptCb(ssl, output + args->idx,
                                output + args->headerSz + args->ivSz, inSz, type, 0,
                                output + args->headerSz, output + args->headerSz, args->size,
                                ssl->MacEncryptCtx);
                goto exit_buildmsg;
            }
        #endif

            if (ssl->specs.cipher_type != aead) {
        #ifdef HAVE_TRUNCATED_HMAC
            if (ssl->truncated_hmac && ssl->specs.hash_size > args->digestSz) {
            #ifdef WOLFSSL_SMALL_STACK
                byte* hmac = NULL;
            #else
                byte  hmac[MAX_DIGEST_SIZE];
            #endif

            #ifdef WOLFSSL_SMALL_STACK
                hmac = (byte*)XMALLOC(MAX_DIGEST_SIZE, ssl->heap,
                                                       DYNAMIC_TYPE_DIGEST);
                if (hmac == NULL)
                    ERROR_OUT(MEMORY_E, exit_buildmsg);
            #endif

                ret = ssl->hmac(ssl, hmac, output + args->headerSz + args->ivSz, inSz,
                                                                       type, 0);
                XMEMCPY(output + args->idx, hmac, args->digestSz);

            #ifdef WOLFSSL_SMALL_STACK
                XFREE(hmac, ssl->heap, DYNAMIC_TYPE_DIGEST);
            #endif
            }
            else
        #endif
                ret = ssl->hmac(ssl, output + args->idx, output + args->headerSz + args->ivSz,
                                                                inSz, type, 0);
            #ifdef WOLFSSL_DTLS
                if (ssl->options.dtls)
                    DtlsSEQIncrement(ssl, CUR_ORDER);
            #endif
            }
            if (ret != 0)
                goto exit_buildmsg;

            ssl->options.buildMsgState = BUILD_MSG_ENCRYPT;
        }
        FALL_THROUGH;
        case BUILD_MSG_ENCRYPT:
        {
            ret = Encrypt(ssl, output + args->headerSz, output + args->headerSz, args->size,
                asyncOkay);
            break;
        }
    }

exit_buildmsg:

    WOLFSSL_LEAVE("BuildMessage", ret);

#ifdef WOLFSSL_ASYNC_CRYPT
    if (ret == WC_PENDING_E) {
        return ret;
    }
#endif

    /* make sure build message state is reset */
    ssl->options.buildMsgState = BUILD_MSG_BEGIN;

    /* return sz on success */
    if (ret == 0)
        ret = args->sz;

    /* Final cleanup */
    FreeBuildMsgArgs(ssl, args);

    return ret;
}


int SendFinished(WOLFSSL* ssl)
{
    int              sendSz,
                     finishedSz = ssl->options.tls ? TLS_FINISHED_SZ :
                                                     FINISHED_SZ;
    byte             input[FINISHED_SZ + DTLS_HANDSHAKE_HEADER_SZ];  /* max */
    byte            *output;
    Hashes*          hashes;
    int              ret;
    int              headerSz = HANDSHAKE_HEADER_SZ;
    int              outputSz;

    /* setup encrypt keys */
    if ((ret = SetKeysSide(ssl, ENCRYPT_SIDE_ONLY)) != 0)
        return ret;

    /* check for available size */
    outputSz = sizeof(input) + MAX_MSG_EXTRA;
    if ((ret = CheckAvailableSize(ssl, outputSz)) != 0)
        return ret;

    #ifdef WOLFSSL_DTLS
        if (ssl->options.dtls) {
            headerSz += DTLS_HANDSHAKE_EXTRA;
            ssl->keys.dtls_epoch++;
            ssl->keys.dtls_prev_sequence_number_hi =
                    ssl->keys.dtls_sequence_number_hi;
            ssl->keys.dtls_prev_sequence_number_lo =
                    ssl->keys.dtls_sequence_number_lo;
            ssl->keys.dtls_sequence_number_hi = 0;
            ssl->keys.dtls_sequence_number_lo = 0;
        }
    #endif

    /* get output buffer */
    output = ssl->buffers.outputBuffer.buffer +
             ssl->buffers.outputBuffer.length;

    AddHandShakeHeader(input, finishedSz, 0, finishedSz, finished, ssl);

    /* make finished hashes */
    hashes = (Hashes*)&input[headerSz];
    ret = BuildFinished(ssl, hashes,
                     ssl->options.side == WOLFSSL_CLIENT_END ? client : server);
    if (ret != 0) return ret;

#ifdef HAVE_SECURE_RENEGOTIATION
    if (ssl->secure_renegotiation) {
        if (ssl->options.side == WOLFSSL_CLIENT_END)
            XMEMCPY(ssl->secure_renegotiation->client_verify_data, hashes,
                    TLS_FINISHED_SZ);
        else
            XMEMCPY(ssl->secure_renegotiation->server_verify_data, hashes,
                    TLS_FINISHED_SZ);
    }
#endif

    #ifdef WOLFSSL_DTLS
        if (IsDtlsNotSctpMode(ssl)) {
            if ((ret = DtlsMsgPoolSave(ssl, input, headerSz + finishedSz)) != 0)
                return ret;
        }
    #endif

    sendSz = BuildMessage(ssl, output, outputSz, input, headerSz + finishedSz,
                                                          handshake, 1, 0, 0);
    if (sendSz < 0)
        return BUILD_MSG_ERROR;

    if (!ssl->options.resuming) {
#ifndef NO_SESSION_CACHE
        AddSession(ssl);    /* just try */
#endif
        if (ssl->options.side == WOLFSSL_SERVER_END) {
            ssl->options.handShakeState = HANDSHAKE_DONE;
            ssl->options.handShakeDone  = 1;
        }
    }
    else {
        if (ssl->options.side == WOLFSSL_CLIENT_END) {
            ssl->options.handShakeState = HANDSHAKE_DONE;
            ssl->options.handShakeDone  = 1;
        }
    }

    #ifdef WOLFSSL_CALLBACKS
        if (ssl->hsInfoOn) AddPacketName("Finished", &ssl->handShakeInfo);
        if (ssl->toInfoOn)
            AddPacketInfo("Finished", &ssl->timeoutInfo, output, sendSz,
                          ssl->heap);
    #endif

    ssl->buffers.outputBuffer.length += sendSz;

    return SendBuffered(ssl);
}


#ifndef NO_CERTS
int SendCertificate(WOLFSSL* ssl)
{
    int    ret = 0;
    word32 certSz, certChainSz, headerSz, listSz, payloadSz;
    word32 length, maxFragment;

    if (ssl->options.usingPSK_cipher || ssl->options.usingAnon_cipher)
        return 0;  /* not needed */

    if (ssl->options.sendVerify == SEND_BLANK_CERT) {
        certSz = 0;
        certChainSz = 0;
        headerSz = CERT_HEADER_SZ;
        length = CERT_HEADER_SZ;
        listSz = 0;
    }
    else {
        if (!ssl->buffers.certificate) {
            WOLFSSL_MSG("Send Cert missing certificate buffer");
            return BUFFER_ERROR;
        }
        certSz = ssl->buffers.certificate->length;
        headerSz = 2 * CERT_HEADER_SZ;
        /* list + cert size */
        length = certSz + headerSz;
        listSz = certSz + CERT_HEADER_SZ;

        /* may need to send rest of chain, already has leading size(s) */
        if (certSz && ssl->buffers.certChain) {
            certChainSz = ssl->buffers.certChain->length;
            length += certChainSz;
            listSz += certChainSz;
        }
        else
            certChainSz = 0;
    }

    payloadSz = length;

    if (ssl->fragOffset != 0)
        length -= (ssl->fragOffset + headerSz);

    maxFragment = MAX_RECORD_SIZE;
    if (ssl->options.dtls) {
    #ifdef WOLFSSL_DTLS
        maxFragment = MAX_MTU - DTLS_RECORD_HEADER_SZ
                      - DTLS_HANDSHAKE_HEADER_SZ - 100;
    #endif /* WOLFSSL_DTLS */
    }

    #ifdef HAVE_MAX_FRAGMENT
    if (ssl->max_fragment != 0 && maxFragment >= ssl->max_fragment)
        maxFragment = ssl->max_fragment;
    #endif /* HAVE_MAX_FRAGMENT */

    while (length > 0 && ret == 0) {
        byte*  output = NULL;
        word32 fragSz = 0;
        word32 i = RECORD_HEADER_SZ;
        int    sendSz = RECORD_HEADER_SZ;

        if (!ssl->options.dtls) {
            if (ssl->fragOffset == 0)  {
                if (headerSz + certSz + certChainSz <=
                    maxFragment - HANDSHAKE_HEADER_SZ) {

                    fragSz = headerSz + certSz + certChainSz;
                }
                else {
                    fragSz = maxFragment - HANDSHAKE_HEADER_SZ;
                }
                sendSz += fragSz + HANDSHAKE_HEADER_SZ;
                i += HANDSHAKE_HEADER_SZ;
            }
            else {
                fragSz = min(length, maxFragment);
                sendSz += fragSz;
            }

            if (IsEncryptionOn(ssl, 1))
                sendSz += MAX_MSG_EXTRA;
        }
        else {
        #ifdef WOLFSSL_DTLS
            fragSz = min(length, maxFragment);
            sendSz += fragSz + DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA
                      + HANDSHAKE_HEADER_SZ;
            i      += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA
                      + HANDSHAKE_HEADER_SZ;
        #endif
        }

        /* check for available size */
        if ((ret = CheckAvailableSize(ssl, sendSz)) != 0)
            return ret;

        /* get output buffer */
        output = ssl->buffers.outputBuffer.buffer +
                 ssl->buffers.outputBuffer.length;

        if (ssl->fragOffset == 0) {
            if (!ssl->options.dtls) {
                AddFragHeaders(output, fragSz, 0, payloadSz, certificate, ssl);
                if (!IsEncryptionOn(ssl, 1))
                    HashOutputRaw(ssl, output + RECORD_HEADER_SZ,
                                  HANDSHAKE_HEADER_SZ);
            }
            else {
            #ifdef WOLFSSL_DTLS
                AddHeaders(output, payloadSz, certificate, ssl);
                if (!IsEncryptionOn(ssl, 1))
                    HashOutputRaw(ssl,
                                  output + RECORD_HEADER_SZ + DTLS_RECORD_EXTRA,
                                  HANDSHAKE_HEADER_SZ + DTLS_HANDSHAKE_EXTRA);
                /* Adding the headers increments these, decrement them for
                 * actual message header. */
                ssl->keys.dtls_handshake_number--;
                AddFragHeaders(output, fragSz, 0, payloadSz, certificate, ssl);
                ssl->keys.dtls_handshake_number--;
            #endif /* WOLFSSL_DTLS */
            }

            /* list total */
            c32to24(listSz, output + i);
            if (!IsEncryptionOn(ssl, 1))
                HashOutputRaw(ssl, output + i, CERT_HEADER_SZ);
            i += CERT_HEADER_SZ;
            length -= CERT_HEADER_SZ;
            fragSz -= CERT_HEADER_SZ;
            if (certSz) {
                c32to24(certSz, output + i);
                if (!IsEncryptionOn(ssl, 1))
                    HashOutputRaw(ssl, output + i, CERT_HEADER_SZ);
                i += CERT_HEADER_SZ;
                length -= CERT_HEADER_SZ;
                fragSz -= CERT_HEADER_SZ;

                if (!IsEncryptionOn(ssl, 1)) {
                    HashOutputRaw(ssl, ssl->buffers.certificate->buffer, certSz);
                    if (certChainSz)
                        HashOutputRaw(ssl, ssl->buffers.certChain->buffer,
                                      certChainSz);
                }
            }
        }
        else {
            if (!ssl->options.dtls) {
                AddRecordHeader(output, fragSz, handshake, ssl);
            }
            else {
            #ifdef WOLFSSL_DTLS
                AddFragHeaders(output, fragSz, ssl->fragOffset + headerSz,
                               payloadSz, certificate, ssl);
                ssl->keys.dtls_handshake_number--;
            #endif /* WOLFSSL_DTLS */
            }
        }

        /* member */
        if (certSz && ssl->fragOffset < certSz) {
            word32 copySz = min(certSz - ssl->fragOffset, fragSz);
            XMEMCPY(output + i,
                    ssl->buffers.certificate->buffer + ssl->fragOffset, copySz);
            i += copySz;
            ssl->fragOffset += copySz;
            length -= copySz;
            fragSz -= copySz;
        }
        if (certChainSz && fragSz) {
            word32 copySz = min(certChainSz + certSz - ssl->fragOffset, fragSz);
            XMEMCPY(output + i,
                    ssl->buffers.certChain->buffer + ssl->fragOffset - certSz,
                    copySz);
            i += copySz;
            ssl->fragOffset += copySz;
            length -= copySz;
        }

        if (IsEncryptionOn(ssl, 1)) {
            byte* input = NULL;
            int   inputSz = i - RECORD_HEADER_SZ; /* build msg adds rec hdr */

            if (inputSz < 0) {
                WOLFSSL_MSG("Send Cert bad inputSz");
                return BUFFER_E;
            }

            if (inputSz > 0) {  /* clang thinks could be zero, let's help */
                input = (byte*)XMALLOC(inputSz, ssl->heap,
                                       DYNAMIC_TYPE_IN_BUFFER);
                if (input == NULL)
                    return MEMORY_E;
                XMEMCPY(input, output + RECORD_HEADER_SZ, inputSz);
            }

            sendSz = BuildMessage(ssl, output, sendSz, input, inputSz,
                                                          handshake, 1, 0, 0);

            if (inputSz > 0)
                XFREE(input, ssl->heap, DYNAMIC_TYPE_IN_BUFFER);

            if (sendSz < 0)
                return sendSz;
        }
        else {
        #ifdef WOLFSSL_DTLS
            if (ssl->options.dtls)
                DtlsSEQIncrement(ssl, CUR_ORDER);
        #endif
        }

    #ifdef WOLFSSL_DTLS
        if (IsDtlsNotSctpMode(ssl)) {
            if ((ret = DtlsMsgPoolSave(ssl, output, sendSz)) != 0)
                return ret;
        }
    #endif

    #ifdef WOLFSSL_CALLBACKS
        if (ssl->hsInfoOn)
            AddPacketName("Certificate", &ssl->handShakeInfo);
        if (ssl->toInfoOn)
            AddPacketInfo("Certificate", &ssl->timeoutInfo, output, sendSz,
                           ssl->heap);
    #endif

        ssl->buffers.outputBuffer.length += sendSz;
        if (!ssl->options.groupMessages)
            ret = SendBuffered(ssl);
    }

    if (ret != WANT_WRITE) {
        /* Clean up the fragment offset. */
        ssl->fragOffset = 0;
        #ifdef WOLFSSL_DTLS
            if (ssl->options.dtls)
                ssl->keys.dtls_handshake_number++;
        #endif
        if (ssl->options.side == WOLFSSL_SERVER_END)
            ssl->options.serverState = SERVER_CERT_COMPLETE;
    }

    return ret;
}


int SendCertificateRequest(WOLFSSL* ssl)
{
    byte   *output;
    int    ret;
    int    sendSz;
    word32 i = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;
    word32 dnLen = 0;
#ifdef WOLFSSL_NGINX
    STACK_OF(WOLFSSL_X509_NAME)* names;
#endif

    int  typeTotal = 1;  /* only 1 for now */
    int  reqSz = ENUM_LEN + typeTotal + REQ_HEADER_SZ;  /* add auth later */

    if (IsAtLeastTLSv1_2(ssl))
        reqSz += LENGTH_SZ + ssl->suites->hashSigAlgoSz;

#ifdef WOLFSSL_NGINX
    /* Certificate Authorities */
    names = ssl->ctx->ca_names;
    while (names != NULL) {
        byte seq[MAX_SEQ_SZ];

        /* 16-bit length | SEQ | Len | DER of name */
        dnLen += OPAQUE16_LEN + SetSequence(names->data.name->rawLen, seq) +
                 names->data.name->rawLen;
        names = names->next;
    }
    reqSz += dnLen;
#endif

    if (ssl->options.usingPSK_cipher || ssl->options.usingAnon_cipher)
        return 0;  /* not needed */

    sendSz = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ + reqSz;

    #ifdef WOLFSSL_DTLS
        if (ssl->options.dtls) {
            sendSz += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
            i      += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
        }
    #endif
    /* check for available size */
    if ((ret = CheckAvailableSize(ssl, sendSz)) != 0)
        return ret;

    /* get output buffer */
    output = ssl->buffers.outputBuffer.buffer +
             ssl->buffers.outputBuffer.length;

    AddHeaders(output, reqSz, certificate_request, ssl);

    /* write to output */
    output[i++] = (byte)typeTotal;  /* # of types */
#ifdef HAVE_ECC
    if ((ssl->options.cipherSuite0 == ECC_BYTE ||
         ssl->options.cipherSuite0 == CHACHA_BYTE) &&
                     ssl->specs.sig_algo == ecc_dsa_sa_algo) {
        output[i++] = ecdsa_sign;
    } else
#endif /* HAVE_ECC */
    {
        output[i++] = rsa_sign;
    }

    /* supported hash/sig */
    if (IsAtLeastTLSv1_2(ssl)) {
        c16toa(ssl->suites->hashSigAlgoSz, &output[i]);
        i += LENGTH_SZ;

        XMEMCPY(&output[i],
                         ssl->suites->hashSigAlgo, ssl->suites->hashSigAlgoSz);
        i += ssl->suites->hashSigAlgoSz;
    }

    /* Certificate Authorities */
    c16toa((word16)dnLen, &output[i]);  /* auth's */
    i += REQ_HEADER_SZ;
#ifdef WOLFSSL_NGINX
    names = ssl->ctx->ca_names;
    while (names != NULL) {
        byte seq[MAX_SEQ_SZ];

        c16toa(names->data.name->rawLen +
               SetSequence(names->data.name->rawLen, seq), &output[i]);
        i += OPAQUE16_LEN;
        i += SetSequence(names->data.name->rawLen, output + i);
        XMEMCPY(output + i, names->data.name->raw, names->data.name->rawLen);
        i += names->data.name->rawLen;
        names = names->next;
    }
#endif
    (void)i;

    #ifdef WOLFSSL_DTLS
        if (IsDtlsNotSctpMode(ssl)) {
            if ((ret = DtlsMsgPoolSave(ssl, output, sendSz)) != 0)
                return ret;
        }
        if (ssl->options.dtls)
            DtlsSEQIncrement(ssl, CUR_ORDER);
    #endif

    ret = HashOutput(ssl, output, sendSz, 0);
    if (ret != 0)
        return ret;

    #ifdef WOLFSSL_CALLBACKS
        if (ssl->hsInfoOn)
            AddPacketName("CertificateRequest", &ssl->handShakeInfo);
        if (ssl->toInfoOn)
            AddPacketInfo("CertificateRequest", &ssl->timeoutInfo, output,
                          sendSz, ssl->heap);
    #endif
    ssl->buffers.outputBuffer.length += sendSz;
    if (ssl->options.groupMessages)
        return 0;
    else
        return SendBuffered(ssl);
}

#ifndef NO_WOLFSSL_SERVER
#if defined(HAVE_CERTIFICATE_STATUS_REQUEST) \
 || defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
static int BuildCertificateStatus(WOLFSSL* ssl, byte type, buffer* status,
                                                                     byte count)
{
    byte*  output  = NULL;
    word32 idx     = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;
    word32 length  = ENUM_LEN;
    int    sendSz  = 0;
    int    ret     = 0;
    int    i       = 0;

    WOLFSSL_ENTER("BuildCertificateStatus");

    switch (type) {
        case WOLFSSL_CSR2_OCSP_MULTI:
            length += OPAQUE24_LEN;
            FALL_THROUGH; /* followed by */

        case WOLFSSL_CSR2_OCSP:
            for (i = 0; i < count; i++)
                length += OPAQUE24_LEN + status[i].length;
        break;

        default:
            return 0;
    }

    sendSz = idx + length;

    if (ssl->keys.encryptionOn)
        sendSz += MAX_MSG_EXTRA;

    if ((ret = CheckAvailableSize(ssl, sendSz)) == 0) {
        output = ssl->buffers.outputBuffer.buffer +
                 ssl->buffers.outputBuffer.length;

        AddHeaders(output, length, certificate_status, ssl);

        output[idx++] = type;

        if (type == WOLFSSL_CSR2_OCSP_MULTI) {
            c32to24(length - (ENUM_LEN + OPAQUE24_LEN), output + idx);
            idx += OPAQUE24_LEN;
        }

        for (i = 0; i < count; i++) {
            c32to24(status[i].length, output + idx);
            idx += OPAQUE24_LEN;

            XMEMCPY(output + idx, status[i].buffer, status[i].length);
            idx += status[i].length;
        }

        if (IsEncryptionOn(ssl, 1)) {
            byte* input;
            int   inputSz = idx - RECORD_HEADER_SZ;

            input = (byte*)XMALLOC(inputSz, ssl->heap, DYNAMIC_TYPE_IN_BUFFER);
            if (input == NULL)
                return MEMORY_E;

            XMEMCPY(input, output + RECORD_HEADER_SZ, inputSz);
            sendSz = BuildMessage(ssl, output, sendSz, input, inputSz,
                                                           handshake, 1, 0, 0);
            XFREE(input, ssl->heap, DYNAMIC_TYPE_IN_BUFFER);

            if (sendSz < 0)
                ret = sendSz;
        }
        else {
            #ifdef WOLFSSL_DTLS
                if (ssl->options.dtls)
                    DtlsSEQIncrement(ssl, CUR_ORDER);
            #endif
            ret = HashOutput(ssl, output, sendSz, 0);
        }

    #ifdef WOLFSSL_DTLS
        if (ret == 0 && IsDtlsNotSctpMode(ssl))
            ret = DtlsMsgPoolSave(ssl, output, sendSz);
    #endif

    #ifdef WOLFSSL_CALLBACKS
        if (ret == 0 && ssl->hsInfoOn)
            AddPacketName("CertificateStatus", &ssl->handShakeInfo);
        if (ret == 0 && ssl->toInfoOn)
            AddPacketInfo("CertificateStatus", &ssl->timeoutInfo, output,
                                                             sendSz, ssl->heap);
    #endif

        if (ret == 0) {
            ssl->buffers.outputBuffer.length += sendSz;
            if (!ssl->options.groupMessages)
                ret = SendBuffered(ssl);
        }
    }

    WOLFSSL_LEAVE("BuildCertificateStatus", ret);
    return ret;
}
#endif
#endif /* NO_WOLFSSL_SERVER */


int SendCertificateStatus(WOLFSSL* ssl)
{
    int ret = 0;
    byte status_type = 0;

    WOLFSSL_ENTER("SendCertificateStatus");

    (void) ssl;

#ifdef HAVE_CERTIFICATE_STATUS_REQUEST
    status_type = ssl->status_request;
#endif

#ifdef HAVE_CERTIFICATE_STATUS_REQUEST_V2
    status_type = status_type ? status_type : ssl->status_request_v2;
#endif

    switch (status_type) {

    #ifndef NO_WOLFSSL_SERVER
    #if defined(HAVE_CERTIFICATE_STATUS_REQUEST) \
     || defined(HAVE_CERTIFICATE_STATUS_REQUEST_V2)
        /* case WOLFSSL_CSR_OCSP: */
        case WOLFSSL_CSR2_OCSP:
        {
            OcspRequest* request = ssl->ctx->certOcspRequest;
            buffer response;

            XMEMSET(&response, 0, sizeof(response));

            /* unable to fetch status. skip. */
            if (ssl->ctx->cm == NULL || ssl->ctx->cm->ocspStaplingEnabled == 0)
                return 0;

            if (request == NULL || ssl->buffers.weOwnCert) {
                DerBuffer* der = ssl->buffers.certificate;
                #ifdef WOLFSSL_SMALL_STACK
                    DecodedCert* cert = NULL;
                #else
                    DecodedCert  cert[1];
                #endif

                /* unable to fetch status. skip. */
                if (der->buffer == NULL || der->length == 0)
                    return 0;

            #ifdef WOLFSSL_SMALL_STACK
                cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), ssl->heap,
                                             DYNAMIC_TYPE_DCERT);
                if (cert == NULL)
                    return MEMORY_E;
            #endif

                InitDecodedCert(cert, der->buffer, der->length, ssl->heap);
                /* TODO: Setup async support here */
                if ((ret = ParseCertRelative(cert, CERT_TYPE, VERIFY,
                                                          ssl->ctx->cm)) != 0) {
                    WOLFSSL_MSG("ParseCert failed");
                }
                else {
                    request = (OcspRequest*)XMALLOC(sizeof(OcspRequest),
                                          ssl->heap, DYNAMIC_TYPE_OCSP_REQUEST);
                    if (request) {
                        ret = InitOcspRequest(request, cert, 0, ssl->heap);
                        if (ret == 0) {
                            /* make sure ctx OCSP request is updated */
                            if (!ssl->buffers.weOwnCert) {
                                wolfSSL_Mutex* ocspLock =
                                    &ssl->ctx->cm->ocsp_stapling->ocspLock;
                                if (wc_LockMutex(ocspLock) == 0) {
                                    if (ssl->ctx->certOcspRequest == NULL)
                                        ssl->ctx->certOcspRequest = request;
                                    wc_UnLockMutex(ocspLock);
                                }
                            }
                        }
                        else {
                            XFREE(request, ssl->heap, DYNAMIC_TYPE_OCSP_REQUEST);
                            request = NULL;
                        }
                    }
                    else {
                        ret = MEMORY_E;
                    }
                }

                FreeDecodedCert(cert);

            #ifdef WOLFSSL_SMALL_STACK
                XFREE(cert, ssl->heap, DYNAMIC_TYPE_DCERT);
            #endif
            }

            if (ret == 0) {
            #if defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
                request->ssl = ssl;
            #endif
                ret = CheckOcspRequest(ssl->ctx->cm->ocsp_stapling, request,
                                                                     &response);

                /* Suppressing, not critical */
                if (ret == OCSP_CERT_REVOKED ||
                    ret == OCSP_CERT_UNKNOWN ||
                    ret == OCSP_LOOKUP_FAIL) {
                    ret = 0;
                }

                if (response.buffer) {
                    if (ret == 0)
                        ret = BuildCertificateStatus(ssl, status_type,
                                                                  &response, 1);

                    XFREE(response.buffer, ssl->heap, DYNAMIC_TYPE_OCSP_REQUEST);
                    response.buffer = NULL;
                }

            }

            if (request != ssl->ctx->certOcspRequest)
                XFREE(request, ssl->heap, DYNAMIC_TYPE_OCSP_REQUEST);

            break;
        }

    #endif /* HAVE_CERTIFICATE_STATUS_REQUEST    */
           /* HAVE_CERTIFICATE_STATUS_REQUEST_V2 */

    #if defined HAVE_CERTIFICATE_STATUS_REQUEST_V2
        case WOLFSSL_CSR2_OCSP_MULTI:
        {
            OcspRequest* request = ssl->ctx->certOcspRequest;
            buffer responses[1 + MAX_CHAIN_DEPTH];
            int i = 0;

            XMEMSET(responses, 0, sizeof(responses));

            /* unable to fetch status. skip. */
            if (ssl->ctx->cm == NULL || ssl->ctx->cm->ocspStaplingEnabled == 0)
                return 0;

            if (!request || ssl->buffers.weOwnCert) {
                DerBuffer* der = ssl->buffers.certificate;
            #ifdef WOLFSSL_SMALL_STACK
                DecodedCert* cert = NULL;
            #else
                DecodedCert  cert[1];
            #endif

                /* unable to fetch status. skip. */
                if (der->buffer == NULL || der->length == 0)
                    return 0;

            #ifdef WOLFSSL_SMALL_STACK
                cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), ssl->heap,
                                             DYNAMIC_TYPE_DCERT);
                if (cert == NULL)
                    return MEMORY_E;
            #endif

                InitDecodedCert(cert, der->buffer, der->length, ssl->heap);
                /* TODO: Setup async support here */
                if ((ret = ParseCertRelative(cert, CERT_TYPE, VERIFY,
                                                          ssl->ctx->cm)) != 0) {
                    WOLFSSL_MSG("ParseCert failed");
                }
                else {
                    request = (OcspRequest*)XMALLOC(sizeof(OcspRequest),
                                          ssl->heap, DYNAMIC_TYPE_OCSP_REQUEST);
                    if (request) {
                        ret = InitOcspRequest(request, cert, 0, ssl->heap);
                        if (ret == 0) {
                            /* make sure ctx OCSP request is updated */
                            if (!ssl->buffers.weOwnCert) {
                                wolfSSL_Mutex* ocspLock =
                                    &ssl->ctx->cm->ocsp_stapling->ocspLock;
                                if (wc_LockMutex(ocspLock) == 0) {
                                    if (ssl->ctx->certOcspRequest == NULL)
                                        ssl->ctx->certOcspRequest = request;
                                    wc_UnLockMutex(ocspLock);
                                }
                            }
                        }
                        else {
                            XFREE(request, ssl->heap, DYNAMIC_TYPE_OCSP_REQUEST);
                            request = NULL;
                        }
                    }
                    else {
                        ret = MEMORY_E;
                    }
                }

                FreeDecodedCert(cert);

            #ifdef WOLFSSL_SMALL_STACK
                XFREE(cert, ssl->heap, DYNAMIC_TYPE_DCERT);
            #endif
            }

            if (ret == 0) {
            #if defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
                request->ssl = ssl;
            #endif
                ret = CheckOcspRequest(ssl->ctx->cm->ocsp_stapling, request,
                                                                 &responses[0]);

                /* Suppressing, not critical */
                if (ret == OCSP_CERT_REVOKED ||
                    ret == OCSP_CERT_UNKNOWN ||
                    ret == OCSP_LOOKUP_FAIL) {
                    ret = 0;
                }
            }

            if (request != ssl->ctx->certOcspRequest)
                XFREE(request, ssl->heap, DYNAMIC_TYPE_OCSP_REQUEST);

            if (ret == 0 && (!ssl->ctx->chainOcspRequest[0]
                                              || ssl->buffers.weOwnCertChain)) {
                buffer der;
                word32 idx = 0;
            #ifdef WOLFSSL_SMALL_STACK
                DecodedCert* cert = NULL;
            #else
                DecodedCert  cert[1];
            #endif

                XMEMSET(&der, 0, sizeof(buffer));

            #ifdef WOLFSSL_SMALL_STACK
                cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), ssl->heap,
                                             DYNAMIC_TYPE_DCERT);
                if (cert == NULL)
                    return MEMORY_E;
            #endif

                while (idx + OPAQUE24_LEN < ssl->buffers.certChain->length) {
                    c24to32(ssl->buffers.certChain->buffer + idx, &der.length);
                    idx += OPAQUE24_LEN;

                    der.buffer = ssl->buffers.certChain->buffer + idx;
                    idx += der.length;

                    if (idx > ssl->buffers.certChain->length)
                        break;

                    InitDecodedCert(cert, der.buffer, der.length, ssl->heap);
                    /* TODO: Setup async support here */
                    if ((ret = ParseCertRelative(cert, CERT_TYPE, VERIFY,
                                                      ssl->ctx->cm)) != 0) {
                        WOLFSSL_MSG("ParseCert failed");
                        break;
                    }
                    else {
                        request = (OcspRequest*)XMALLOC(sizeof(OcspRequest),
                                          ssl->heap, DYNAMIC_TYPE_OCSP_REQUEST);
                        if (request == NULL) {
                            FreeDecodedCert(cert);

                            ret = MEMORY_E;
                            break;
                        }

                        ret = InitOcspRequest(request, cert, 0, ssl->heap);
                        if (ret == 0) {
                            /* make sure ctx OCSP request is updated */
                            if (!ssl->buffers.weOwnCertChain) {
                                wolfSSL_Mutex* ocspLock =
                                    &ssl->ctx->cm->ocsp_stapling->ocspLock;
                                if (wc_LockMutex(ocspLock) == 0) {
                                    if (ssl->ctx->chainOcspRequest[i] == NULL)
                                        ssl->ctx->chainOcspRequest[i] = request;
                                    wc_UnLockMutex(ocspLock);
                                }
                            }
                        }
                        else {
                            FreeDecodedCert(cert);
                            XFREE(request, ssl->heap, DYNAMIC_TYPE_OCSP_REQUEST);
                            request = NULL;
                            break;
                        }

                    #if defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
                        request->ssl = ssl;
                    #endif
                        ret = CheckOcspRequest(ssl->ctx->cm->ocsp_stapling,
                                                    request, &responses[i + 1]);

                        /* Suppressing, not critical */
                        if (ret == OCSP_CERT_REVOKED ||
                            ret == OCSP_CERT_UNKNOWN ||
                            ret == OCSP_LOOKUP_FAIL) {
                            ret = 0;
                        }

                        if (request != ssl->ctx->chainOcspRequest[i])
                            XFREE(request, ssl->heap, DYNAMIC_TYPE_OCSP_REQUEST);

                        i++;
                    }

                    FreeDecodedCert(cert);
                }

            #ifdef WOLFSSL_SMALL_STACK
                XFREE(cert, ssl->heap, DYNAMIC_TYPE_DCERT);
            #endif
            }
            else {
                while (ret == 0 &&
                            NULL != (request = ssl->ctx->chainOcspRequest[i])) {
                #if defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
                    request->ssl = ssl;
                #endif
                    ret = CheckOcspRequest(ssl->ctx->cm->ocsp_stapling,
                                                request, &responses[++i]);

                    /* Suppressing, not critical */
                    if (ret == OCSP_CERT_REVOKED ||
                        ret == OCSP_CERT_UNKNOWN ||
                        ret == OCSP_LOOKUP_FAIL) {
                        ret = 0;
                    }
                }
            }

            if (responses[0].buffer) {
                if (ret == 0)
                    ret = BuildCertificateStatus(ssl, status_type,
                                                        responses, (byte)i + 1);

                for (i = 0; i < 1 + MAX_CHAIN_DEPTH; i++)
                    if (responses[i].buffer)
                        XFREE(responses[i].buffer, ssl->heap,
                                                       DYNAMIC_TYPE_OCSP_REQUEST);
            }

            break;
        }
    #endif /* HAVE_CERTIFICATE_STATUS_REQUEST_V2 */
    #endif /* NO_WOLFSSL_SERVER */

        default:
            break;
    }

    return ret;
}

#endif /* !NO_CERTS */


int SendData(WOLFSSL* ssl, const void* data, int sz)
{
    int sent = 0,  /* plainText size */
        sendSz,
        ret,
        dtlsExtra = 0;

    if (ssl->error == WANT_WRITE || ssl->error == WC_PENDING_E)
        ssl->error = 0;

#ifdef WOLFSSL_DTLS
    if (ssl->options.dtls) {
        /* In DTLS mode, we forgive some errors and allow the session
         * to continue despite them. */
        if (ssl->error == VERIFY_MAC_ERROR || ssl->error == DECRYPT_ERROR)
            ssl->error = 0;
    }
#endif /* WOLFSSL_DTLS */

#ifdef WOLFSSL_EARLY_DATA
    if (ssl->earlyData) {
        if (ssl->options.handShakeState == HANDSHAKE_DONE) {
            WOLFSSL_MSG("handshake complete, trying to send early data");
            return BUILD_MSG_ERROR;
        }
    }
    else
#endif
    if (ssl->options.handShakeState != HANDSHAKE_DONE) {
        int err;
        WOLFSSL_MSG("handshake not complete, trying to finish");
        if ( (err = wolfSSL_negotiate(ssl)) != SSL_SUCCESS) {
            /* if async would block return WANT_WRITE */
            if (ssl->error == WC_PENDING_E) {
                return WOLFSSL_CBIO_ERR_WANT_WRITE;
            }
            return  err;
        }
    }

    /* last time system socket output buffer was full, try again to send */
    if (ssl->buffers.outputBuffer.length > 0) {
        WOLFSSL_MSG("output buffer was full, trying to send again");
        if ( (ssl->error = SendBuffered(ssl)) < 0) {
            WOLFSSL_ERROR(ssl->error);
            if (ssl->error == SOCKET_ERROR_E && ssl->options.connReset)
                return 0;     /* peer reset */
            return ssl->error;
        }
        else {
            /* advance sent to previous sent + plain size just sent */
            sent = ssl->buffers.prevSent + ssl->buffers.plainSz;
            WOLFSSL_MSG("sent write buffered data");

            if (sent > sz) {
                WOLFSSL_MSG("error: write() after WANT_WRITE with short size");
                return ssl->error = BAD_FUNC_ARG;
            }
        }
    }

#ifdef WOLFSSL_DTLS
    if (ssl->options.dtls) {
        dtlsExtra = DTLS_RECORD_EXTRA;
    }
#endif

    for (;;) {
        int   len;
        byte* out;
        byte* sendBuffer = (byte*)data + sent;  /* may switch on comp */
        int   buffSz;                           /* may switch on comp */
        int   outputSz;
#ifdef HAVE_LIBZ
        byte  comp[MAX_RECORD_SIZE + MAX_COMP_EXTRA];
#endif

        if (sent == sz) break;

        len = min(sz - sent, OUTPUT_RECORD_SIZE);
#ifdef HAVE_MAX_FRAGMENT
        len = min(len, ssl->max_fragment);
#endif

#ifdef WOLFSSL_DTLS
        if (IsDtlsNotSctpMode(ssl)) {
            len = min(len, MAX_UDP_SIZE);
        }
#endif
        buffSz = len;

        /* check for available size */
        outputSz = len + COMP_EXTRA + dtlsExtra + MAX_MSG_EXTRA;
        if ((ret = CheckAvailableSize(ssl, outputSz)) != 0)
            return ssl->error = ret;

        /* get output buffer */
        out = ssl->buffers.outputBuffer.buffer +
              ssl->buffers.outputBuffer.length;

#ifdef HAVE_LIBZ
        if (ssl->options.usingCompression) {
            buffSz = myCompress(ssl, sendBuffer, buffSz, comp, sizeof(comp));
            if (buffSz < 0) {
                return buffSz;
            }
            sendBuffer = comp;
        }
#endif
        if (!ssl->options.tls1_3) {
            sendSz = BuildMessage(ssl, out, outputSz, sendBuffer, buffSz,
                                  application_data, 0, 0, 1);
        }
        else {
#ifdef WOLFSSL_TLS13
            sendSz = BuildTls13Message(ssl, out, outputSz, sendBuffer, buffSz,
                                       application_data, 0, 0, 1);
#else
            sendSz = BUFFER_ERROR;
#endif
        }
        if (sendSz < 0) {
        #ifdef WOLFSSL_ASYNC_CRYPT
            if (sendSz == WC_PENDING_E)
                ssl->error = sendSz;
        #endif
            return BUILD_MSG_ERROR;
        }

        ssl->buffers.outputBuffer.length += sendSz;

        if ( (ret = SendBuffered(ssl)) < 0) {
            WOLFSSL_ERROR(ret);
            /* store for next call if WANT_WRITE or user embedSend() that
               doesn't present like WANT_WRITE */
            ssl->buffers.plainSz  = len;
            ssl->buffers.prevSent = sent;
            if (ret == SOCKET_ERROR_E && ssl->options.connReset)
                return 0;  /* peer reset */
            return ssl->error = ret;
        }

        sent += len;

        /* only one message per attempt */
        if (ssl->options.partialWrite == 1) {
            WOLFSSL_MSG("Paritial Write on, only sending one record");
            break;
        }
    }

    return sent;
}

/* process input data */
int ReceiveData(WOLFSSL* ssl, byte* output, int sz, int peek)
{
    int size;

    WOLFSSL_ENTER("ReceiveData()");

    /* reset error state */
    if (ssl->error == WANT_READ || ssl->error == WC_PENDING_E) {
        ssl->error = 0;
    }

#ifdef WOLFSSL_DTLS
    if (ssl->options.dtls) {
        /* In DTLS mode, we forgive some errors and allow the session
         * to continue despite them. */
        if (ssl->error == VERIFY_MAC_ERROR || ssl->error == DECRYPT_ERROR)
            ssl->error = 0;
    }
#endif /* WOLFSSL_DTLS */

    if (ssl->error != 0 && ssl->error != WANT_WRITE) {
        WOLFSSL_MSG("User calling wolfSSL_read in error state, not allowed");
        return ssl->error;
    }

#ifdef WOLFSSL_EARLY_DATA
    if (ssl->earlyData) {
    }
    else
#endif
    if (ssl->options.handShakeState != HANDSHAKE_DONE) {
        int err;
        WOLFSSL_MSG("Handshake not complete, trying to finish");
        if ( (err = wolfSSL_negotiate(ssl)) != SSL_SUCCESS) {
        #ifdef WOLFSSL_ASYNC_CRYPT
            /* if async would block return WANT_WRITE */
            if (ssl->error == WC_PENDING_E) {
                return WOLFSSL_CBIO_ERR_WANT_READ;
            }
        #endif
            return  err;
        }
    }

#ifdef HAVE_SECURE_RENEGOTIATION
startScr:
    if (ssl->secure_renegotiation && ssl->secure_renegotiation->startScr) {
        int err;
        ssl->secure_renegotiation->startScr = 0;  /* only start once */
        WOLFSSL_MSG("Need to start scr, server requested");
        if ( (err = wolfSSL_Rehandshake(ssl)) != SSL_SUCCESS)
            return  err;
    }
#endif

    while (ssl->buffers.clearOutputBuffer.length == 0) {
        if ( (ssl->error = ProcessReply(ssl)) < 0) {
            WOLFSSL_ERROR(ssl->error);
            if (ssl->error == ZERO_RETURN) {
                WOLFSSL_MSG("Zero return, no more data coming");
                return 0;         /* no more data coming */
            }
            if (ssl->error == SOCKET_ERROR_E) {
                if (ssl->options.connReset || ssl->options.isClosed) {
                    WOLFSSL_MSG("Peer reset or closed, connection done");
                    ssl->error = SOCKET_PEER_CLOSED_E;
                    WOLFSSL_ERROR(ssl->error);
                    return 0;     /* peer reset or closed */
                }
            }
            return ssl->error;
        }
        #ifdef HAVE_SECURE_RENEGOTIATION
            if (ssl->secure_renegotiation &&
                ssl->secure_renegotiation->startScr) {
                goto startScr;
            }
        #endif
    }

    if (sz < (int)ssl->buffers.clearOutputBuffer.length)
        size = sz;
    else
        size = ssl->buffers.clearOutputBuffer.length;

    XMEMCPY(output, ssl->buffers.clearOutputBuffer.buffer, size);

    if (peek == 0) {
        ssl->buffers.clearOutputBuffer.length -= size;
        ssl->buffers.clearOutputBuffer.buffer += size;
    }

    if (ssl->buffers.clearOutputBuffer.length == 0 &&
                                           ssl->buffers.inputBuffer.dynamicFlag)
       ShrinkInputBuffer(ssl, NO_FORCED_FREE);

    WOLFSSL_LEAVE("ReceiveData()", size);
    return size;
}


/* send alert message */
int SendAlert(WOLFSSL* ssl, int severity, int type)
{
    byte input[ALERT_SIZE];
    byte *output;
    int  sendSz;
    int  ret;
    int  outputSz;
    int  dtlsExtra = 0;

#ifdef HAVE_WRITE_DUP
    if (ssl->dupWrite && ssl->dupSide == READ_DUP_SIDE) {
        int notifyErr = 0;

        WOLFSSL_MSG("Read dup side cannot write alerts, notifying sibling");

        if (type == close_notify) {
            notifyErr = ZERO_RETURN;
        } else if (severity == alert_fatal) {
            notifyErr = FATAL_ERROR;
        }

        if (notifyErr != 0) {
            return NotifyWriteSide(ssl, notifyErr);
        }

        return 0;
    }
#endif

    /* if sendalert is called again for nonblocking */
    if (ssl->options.sendAlertState != 0) {
        ret = SendBuffered(ssl);
        if (ret == 0)
            ssl->options.sendAlertState = 0;
        return ret;
    }

   #ifdef WOLFSSL_DTLS
        if (ssl->options.dtls)
           dtlsExtra = DTLS_RECORD_EXTRA;
   #endif

    /* check for available size */
    outputSz = ALERT_SIZE + MAX_MSG_EXTRA + dtlsExtra;
    if ((ret = CheckAvailableSize(ssl, outputSz)) != 0)
        return ret;

    /* get output buffer */
    output = ssl->buffers.outputBuffer.buffer +
             ssl->buffers.outputBuffer.length;

    input[0] = (byte)severity;
    input[1] = (byte)type;
    ssl->alert_history.last_tx.code = type;
    ssl->alert_history.last_tx.level = severity;
    if (severity == alert_fatal) {
        ssl->options.isClosed = 1;  /* Don't send close_notify */
    }

    /* only send encrypted alert if handshake actually complete, otherwise
       other side may not be able to handle it */
    if (IsEncryptionOn(ssl, 1) && ssl->options.handShakeDone)
        sendSz = BuildMessage(ssl, output, outputSz, input, ALERT_SIZE,
                                                          alert, 0, 0, 0);
    else {

        AddRecordHeader(output, ALERT_SIZE, alert, ssl);
        output += RECORD_HEADER_SZ;
        #ifdef WOLFSSL_DTLS
            if (ssl->options.dtls)
                output += DTLS_RECORD_EXTRA;
        #endif
        XMEMCPY(output, input, ALERT_SIZE);

        sendSz = RECORD_HEADER_SZ + ALERT_SIZE;
        #ifdef WOLFSSL_DTLS
            if (ssl->options.dtls)
                sendSz += DTLS_RECORD_EXTRA;
        #endif
    }
    if (sendSz < 0)
        return BUILD_MSG_ERROR;

    #ifdef WOLFSSL_CALLBACKS
        if (ssl->hsInfoOn)
            AddPacketName("Alert", &ssl->handShakeInfo);
        if (ssl->toInfoOn)
            AddPacketInfo("Alert", &ssl->timeoutInfo, output, sendSz,ssl->heap);
    #endif

    ssl->buffers.outputBuffer.length += sendSz;
    ssl->options.sendAlertState = 1;

    return SendBuffered(ssl);
}

const char* wolfSSL_ERR_reason_error_string(unsigned long e)
{
#ifdef NO_ERROR_STRINGS

    (void)e;
    return "no support for error strings built in";

#else

    int error = (int)e;

    /* pass to wolfCrypt */
    if (error < MAX_CODE_E && error > MIN_CODE_E) {
        return wc_GetErrorString(error);
    }

    switch (error) {

    case UNSUPPORTED_SUITE :
        return "unsupported cipher suite";

    case INPUT_CASE_ERROR :
        return "input state error";

    case PREFIX_ERROR :
        return "bad index to key rounds";

    case MEMORY_ERROR :
        return "out of memory";

    case VERIFY_FINISHED_ERROR :
        return "verify problem on finished";

    case VERIFY_MAC_ERROR :
        return "verify mac problem";

    case PARSE_ERROR :
        return "parse error on header";

    case SIDE_ERROR :
        return "wrong client/server type";

    case NO_PEER_CERT :
        return "peer didn't send cert";

    case UNKNOWN_HANDSHAKE_TYPE :
        return "weird handshake type";

    case SOCKET_ERROR_E :
        return "error state on socket";

    case SOCKET_NODATA :
        return "expected data, not there";

    case INCOMPLETE_DATA :
        return "don't have enough data to complete task";

    case UNKNOWN_RECORD_TYPE :
        return "unknown type in record hdr";

    case DECRYPT_ERROR :
        return "error during decryption";

    case FATAL_ERROR :
        return "revcd alert fatal error";

    case ENCRYPT_ERROR :
        return "error during encryption";

    case FREAD_ERROR :
        return "fread problem";

    case NO_PEER_KEY :
        return "need peer's key";

    case NO_PRIVATE_KEY :
        return "need the private key";

    case NO_DH_PARAMS :
        return "server missing DH params";

    case RSA_PRIVATE_ERROR :
        return "error during rsa priv op";

    case MATCH_SUITE_ERROR :
        return "can't match cipher suite";

    case COMPRESSION_ERROR :
        return "compression mismatch error";

    case BUILD_MSG_ERROR :
        return "build message failure";

    case BAD_HELLO :
        return "client hello malformed";

    case DOMAIN_NAME_MISMATCH :
        return "peer subject name mismatch";

    case WANT_READ :
    case SSL_ERROR_WANT_READ :
        return "non-blocking socket wants data to be read";

    case NOT_READY_ERROR :
        return "handshake layer not ready yet, complete first";

    case PMS_VERSION_ERROR :
        return "premaster secret version mismatch error";

    case VERSION_ERROR :
        return "record layer version error";

    case WANT_WRITE :
    case SSL_ERROR_WANT_WRITE :
        return "non-blocking socket write buffer full";

    case BUFFER_ERROR :
        return "malformed buffer input error";

    case VERIFY_CERT_ERROR :
        return "verify problem on certificate";

    case VERIFY_SIGN_ERROR :
        return "verify problem based on signature";

    case CLIENT_ID_ERROR :
        return "psk client identity error";

    case SERVER_HINT_ERROR:
        return "psk server hint error";

    case PSK_KEY_ERROR:
        return "psk key callback error";

    case NTRU_KEY_ERROR:
        return "NTRU key error";

    case NTRU_DRBG_ERROR:
        return "NTRU drbg error";

    case NTRU_ENCRYPT_ERROR:
        return "NTRU encrypt error";

    case NTRU_DECRYPT_ERROR:
        return "NTRU decrypt error";

    case ZLIB_INIT_ERROR:
        return "zlib init error";

    case ZLIB_COMPRESS_ERROR:
        return "zlib compress error";

    case ZLIB_DECOMPRESS_ERROR:
        return "zlib decompress error";

    case GETTIME_ERROR:
        return "gettimeofday() error";

    case GETITIMER_ERROR:
        return "getitimer() error";

    case SIGACT_ERROR:
        return "sigaction() error";

    case SETITIMER_ERROR:
        return "setitimer() error";

    case LENGTH_ERROR:
        return "record layer length error";

    case PEER_KEY_ERROR:
        return "cant decode peer key";

    case ZERO_RETURN:
    case SSL_ERROR_ZERO_RETURN:
        return "peer sent close notify alert";

    case ECC_CURVETYPE_ERROR:
        return "Bad ECC Curve Type or unsupported";

    case ECC_CURVE_ERROR:
        return "Bad ECC Curve or unsupported";

    case ECC_PEERKEY_ERROR:
        return "Bad ECC Peer Key";

    case ECC_MAKEKEY_ERROR:
        return "ECC Make Key failure";

    case ECC_EXPORT_ERROR:
        return "ECC Export Key failure";

    case ECC_SHARED_ERROR:
        return "ECC DHE shared failure";

    case NOT_CA_ERROR:
        return "Not a CA by basic constraint error";

    case HTTP_TIMEOUT:
        return "HTTP timeout for OCSP or CRL req";

    case BAD_CERT_MANAGER_ERROR:
        return "Bad Cert Manager error";

    case OCSP_CERT_REVOKED:
        return "OCSP Cert revoked";

    case CRL_CERT_REVOKED:
        return "CRL Cert revoked";

    case CRL_MISSING:
        return "CRL missing, not loaded";

    case MONITOR_SETUP_E:
        return "CRL monitor setup error";

    case THREAD_CREATE_E:
        return "Thread creation problem";

    case OCSP_NEED_URL:
        return "OCSP need URL";

    case OCSP_CERT_UNKNOWN:
        return "OCSP Cert unknown";

    case OCSP_LOOKUP_FAIL:
        return "OCSP Responder lookup fail";

    case MAX_CHAIN_ERROR:
        return "Maximum Chain Depth Exceeded";

    case COOKIE_ERROR:
        return "DTLS Cookie Error";

    case SEQUENCE_ERROR:
        return "DTLS Sequence Error";

    case SUITES_ERROR:
        return "Suites Pointer Error";

    case SSL_NO_PEM_HEADER:
        return "No PEM Header Error";

    case OUT_OF_ORDER_E:
        return "Out of order message, fatal";

    case BAD_KEA_TYPE_E:
        return "Bad KEA type found";

    case SANITY_CIPHER_E:
        return "Sanity check on ciphertext failed";

    case RECV_OVERFLOW_E:
        return "Receive callback returned more than requested";

    case GEN_COOKIE_E:
        return "Generate Cookie Error";

    case NO_PEER_VERIFY:
        return "Need peer certificate verify Error";

    case FWRITE_ERROR:
        return "fwrite Error";

    case CACHE_MATCH_ERROR:
        return "Cache restore header match Error";

    case UNKNOWN_SNI_HOST_NAME_E:
        return "Unrecognized host name Error";

    case UNKNOWN_MAX_FRAG_LEN_E:
        return "Unrecognized max frag len Error";

    case KEYUSE_SIGNATURE_E:
        return "Key Use digitalSignature not set Error";

    case KEYUSE_ENCIPHER_E:
        return "Key Use keyEncipherment not set Error";

    case EXTKEYUSE_AUTH_E:
        return "Ext Key Use server/client auth not set Error";

    case SEND_OOB_READ_E:
        return "Send Callback Out of Bounds Read Error";

    case SECURE_RENEGOTIATION_E:
        return "Invalid Renegotiation Error";

    case SESSION_TICKET_LEN_E:
        return "Session Ticket Too Long Error";

    case SESSION_TICKET_EXPECT_E:
        return "Session Ticket Error";

    case SCR_DIFFERENT_CERT_E:
        return "Peer sent different cert during SCR";

    case SESSION_SECRET_CB_E:
        return "Session Secret Callback Error";

    case NO_CHANGE_CIPHER_E:
        return "Finished received from peer before Change Cipher Error";

    case SANITY_MSG_E:
        return "Sanity Check on message order Error";

    case DUPLICATE_MSG_E:
        return "Duplicate HandShake message Error";

    case SNI_UNSUPPORTED:
        return "Protocol version does not support SNI Error";

    case SOCKET_PEER_CLOSED_E:
        return "Peer closed underlying transport Error";

    case BAD_TICKET_KEY_CB_SZ:
        return "Bad user session ticket key callback Size Error";

    case BAD_TICKET_MSG_SZ:
        return "Bad session ticket message Size Error";

    case BAD_TICKET_ENCRYPT:
        return "Bad user ticket callback encrypt Error";

    case DH_KEY_SIZE_E:
        return "DH key too small Error";

    case SNI_ABSENT_ERROR:
        return "No Server Name Indication extension Error";

    case RSA_SIGN_FAULT:
        return "RSA Signature Fault Error";

    case HANDSHAKE_SIZE_ERROR:
        return "Handshake message too large Error";

    case UNKNOWN_ALPN_PROTOCOL_NAME_E:
        return "Unrecognized protocol name Error";

    case BAD_CERTIFICATE_STATUS_ERROR:
        return "Bad Certificate Status Message Error";

    case OCSP_INVALID_STATUS:
        return "Invalid OCSP Status Error";

    case RSA_KEY_SIZE_E:
        return "RSA key too small";

    case ECC_KEY_SIZE_E:
        return "ECC key too small";

    case DTLS_EXPORT_VER_E:
        return "Version needs updated after code change or version mismatch";

    case INPUT_SIZE_E:
        return "Input size too large Error";

    case CTX_INIT_MUTEX_E:
        return "Initialize ctx mutex error";

    case EXT_MASTER_SECRET_NEEDED_E:
        return "Extended Master Secret must be enabled to resume EMS session";

    case DTLS_POOL_SZ_E:
        return "Maximum DTLS pool size exceeded";

    case DECODE_E:
        return "Decode handshake message error";

    case WRITE_DUP_READ_E:
        return "Write dup write side can't read error";

    case WRITE_DUP_WRITE_E:
        return "Write dup read side can't write error";

    case INVALID_CERT_CTX_E:
        return "Certificate context does not match request or not empty";

    case BAD_KEY_SHARE_DATA:
        return "The Key Share data contains group that was in Client Hello";

    case MISSING_HANDSHAKE_DATA:
        return "The handshake message is missing required data";

    case BAD_BINDER:
        return "Binder value does not match value server calculated";

    case EXT_NOT_ALLOWED:
        return "Extension type not allowed in handshake message type";

    case INVALID_PARAMETER:
        return "The security parameter is invalid";

    case KEY_SHARE_ERROR:
        return "Key share extension did not contain a valid named group";

    case POST_HAND_AUTH_ERROR:
        return "Client will not do post handshake authentication";

    case HRR_COOKIE_ERROR:
        return "Cookie does not match one sent in HelloRetryRequest";

    case MCAST_HIGHWATER_CB_E:
        return "Multicast highwater callback returned error";

    case ALERT_COUNT_E:
        return "Alert Count exceeded error";

    case EXT_MISSING:
        return "Required TLS extension missing";

    default :
        return "unknown error number";
    }

#endif /* NO_ERROR_STRINGS */
}

void SetErrorString(int error, char* str)
{
    XSTRNCPY(str, wolfSSL_ERR_reason_error_string(error), WOLFSSL_MAX_ERROR_SZ);
}


/* be sure to add to cipher_name_idx too !!!! */
static const char* const cipher_names[] =
{
#ifdef BUILD_SSL_RSA_WITH_RC4_128_SHA
    "RC4-SHA",
#endif

#ifdef BUILD_SSL_RSA_WITH_RC4_128_MD5
    "RC4-MD5",
#endif

#ifdef BUILD_SSL_RSA_WITH_3DES_EDE_CBC_SHA
    "DES-CBC3-SHA",
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_128_CBC_SHA
    "AES128-SHA",
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_256_CBC_SHA
    "AES256-SHA",
#endif

#ifdef BUILD_TLS_RSA_WITH_NULL_SHA
    "NULL-SHA",
#endif

#ifdef BUILD_TLS_RSA_WITH_NULL_SHA256
    "NULL-SHA256",
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_AES_128_CBC_SHA
    "DHE-RSA-AES128-SHA",
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_AES_256_CBC_SHA
    "DHE-RSA-AES256-SHA",
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_AES_256_GCM_SHA384
    "DHE-PSK-AES256-GCM-SHA384",
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_AES_128_GCM_SHA256
    "DHE-PSK-AES128-GCM-SHA256",
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_256_GCM_SHA384
    "PSK-AES256-GCM-SHA384",
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_128_GCM_SHA256
    "PSK-AES128-GCM-SHA256",
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_AES_256_CBC_SHA384
    "DHE-PSK-AES256-CBC-SHA384",
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_AES_128_CBC_SHA256
    "DHE-PSK-AES128-CBC-SHA256",
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_256_CBC_SHA384
    "PSK-AES256-CBC-SHA384",
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_128_CBC_SHA256
    "PSK-AES128-CBC-SHA256",
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_128_CBC_SHA
    "PSK-AES128-CBC-SHA",
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_256_CBC_SHA
    "PSK-AES256-CBC-SHA",
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_AES_128_CCM
    "DHE-PSK-AES128-CCM",
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_AES_256_CCM
    "DHE-PSK-AES256-CCM",
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_128_CCM
    "PSK-AES128-CCM",
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_256_CCM
    "PSK-AES256-CCM",
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_128_CCM_8
    "PSK-AES128-CCM-8",
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_256_CCM_8
    "PSK-AES256-CCM-8",
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_NULL_SHA384
    "DHE-PSK-NULL-SHA384",
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_NULL_SHA256
    "DHE-PSK-NULL-SHA256",
#endif

#ifdef BUILD_TLS_PSK_WITH_NULL_SHA384
    "PSK-NULL-SHA384",
#endif

#ifdef BUILD_TLS_PSK_WITH_NULL_SHA256
    "PSK-NULL-SHA256",
#endif

#ifdef BUILD_TLS_PSK_WITH_NULL_SHA
    "PSK-NULL-SHA",
#endif

#ifdef BUILD_TLS_RSA_WITH_HC_128_MD5
    "HC128-MD5",
#endif

#ifdef BUILD_TLS_RSA_WITH_HC_128_SHA
    "HC128-SHA",
#endif

#ifdef BUILD_TLS_RSA_WITH_HC_128_B2B256
    "HC128-B2B256",
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_128_CBC_B2B256
    "AES128-B2B256",
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_256_CBC_B2B256
    "AES256-B2B256",
#endif

#ifdef BUILD_TLS_RSA_WITH_RABBIT_SHA
    "RABBIT-SHA",
#endif

#ifdef BUILD_TLS_NTRU_RSA_WITH_RC4_128_SHA
    "NTRU-RC4-SHA",
#endif

#ifdef BUILD_TLS_NTRU_RSA_WITH_3DES_EDE_CBC_SHA
    "NTRU-DES-CBC3-SHA",
#endif

#ifdef BUILD_TLS_NTRU_RSA_WITH_AES_128_CBC_SHA
    "NTRU-AES128-SHA",
#endif

#ifdef BUILD_TLS_NTRU_RSA_WITH_AES_256_CBC_SHA
    "NTRU-AES256-SHA",
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_128_CCM_8
    "AES128-CCM-8",
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_256_CCM_8
    "AES256-CCM-8",
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_CCM
    "ECDHE-ECDSA-AES128-CCM",
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8
    "ECDHE-ECDSA-AES128-CCM-8",
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8
    "ECDHE-ECDSA-AES256-CCM-8",
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA
    "ECDHE-RSA-AES128-SHA",
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA
    "ECDHE-RSA-AES256-SHA",
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA
    "ECDHE-ECDSA-AES128-SHA",
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA
    "ECDHE-ECDSA-AES256-SHA",
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_RC4_128_SHA
    "ECDHE-RSA-RC4-SHA",
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA
    "ECDHE-RSA-DES-CBC3-SHA",
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_RC4_128_SHA
    "ECDHE-ECDSA-RC4-SHA",
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA
    "ECDHE-ECDSA-DES-CBC3-SHA",
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_128_CBC_SHA256
    "AES128-SHA256",
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_256_CBC_SHA256
    "AES256-SHA256",
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256
    "DHE-RSA-AES128-SHA256",
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256
    "DHE-RSA-AES256-SHA256",
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA
    "ECDH-RSA-AES128-SHA",
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA
    "ECDH-RSA-AES256-SHA",
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA
    "ECDH-ECDSA-AES128-SHA",
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA
    "ECDH-ECDSA-AES256-SHA",
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_RC4_128_SHA
    "ECDH-RSA-RC4-SHA",
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA
    "ECDH-RSA-DES-CBC3-SHA",
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_RC4_128_SHA
    "ECDH-ECDSA-RC4-SHA",
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA
    "ECDH-ECDSA-DES-CBC3-SHA",
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_128_GCM_SHA256
    "AES128-GCM-SHA256",
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_256_GCM_SHA384
    "AES256-GCM-SHA384",
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256
    "DHE-RSA-AES128-GCM-SHA256",
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384
    "DHE-RSA-AES256-GCM-SHA384",
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256
    "ECDHE-RSA-AES128-GCM-SHA256",
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384
    "ECDHE-RSA-AES256-GCM-SHA384",
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256
    "ECDHE-ECDSA-AES128-GCM-SHA256",
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384
    "ECDHE-ECDSA-AES256-GCM-SHA384",
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256
    "ECDH-RSA-AES128-GCM-SHA256",
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384
    "ECDH-RSA-AES256-GCM-SHA384",
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256
    "ECDH-ECDSA-AES128-GCM-SHA256",
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384
    "ECDH-ECDSA-AES256-GCM-SHA384",
#endif

#ifdef BUILD_TLS_RSA_WITH_CAMELLIA_128_CBC_SHA
    "CAMELLIA128-SHA",
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA
    "DHE-RSA-CAMELLIA128-SHA",
#endif

#ifdef BUILD_TLS_RSA_WITH_CAMELLIA_256_CBC_SHA
    "CAMELLIA256-SHA",
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA
    "DHE-RSA-CAMELLIA256-SHA",
#endif

#ifdef BUILD_TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256
    "CAMELLIA128-SHA256",
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256
    "DHE-RSA-CAMELLIA128-SHA256",
#endif

#ifdef BUILD_TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256
    "CAMELLIA256-SHA256",
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256
    "DHE-RSA-CAMELLIA256-SHA256",
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256
    "ECDHE-RSA-AES128-SHA256",
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256
    "ECDHE-ECDSA-AES128-SHA256",
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256
    "ECDH-RSA-AES128-SHA256",
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256
    "ECDH-ECDSA-AES128-SHA256",
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384
    "ECDHE-RSA-AES256-SHA384",
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384
    "ECDHE-ECDSA-AES256-SHA384",
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384
    "ECDH-RSA-AES256-SHA384",
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384
    "ECDH-ECDSA-AES256-SHA384",
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256
    "ECDHE-RSA-CHACHA20-POLY1305",
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256
    "ECDHE-ECDSA-CHACHA20-POLY1305",
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256
    "DHE-RSA-CHACHA20-POLY1305",
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256
    "ECDHE-RSA-CHACHA20-POLY1305-OLD",
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_CHACHA20_OLD_POLY1305_SHA256
    "ECDHE-ECDSA-CHACHA20-POLY1305-OLD",
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256
    "DHE-RSA-CHACHA20-POLY1305-OLD",
#endif

#ifdef BUILD_TLS_DH_anon_WITH_AES_128_CBC_SHA
    "ADH-AES128-SHA",
#endif

#ifdef BUILD_TLS_QSH
    "QSH",
#endif

#ifdef HAVE_RENEGOTIATION_INDICATION
    "RENEGOTIATION-INFO",
#endif

#ifdef BUILD_SSL_RSA_WITH_IDEA_CBC_SHA
    "IDEA-CBC-SHA",
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_NULL_SHA
    "ECDHE-ECDSA-NULL-SHA",
#endif

#ifdef BUILD_TLS_ECDHE_PSK_WITH_NULL_SHA256
    "ECDHE-PSK-NULL-SHA256",
#endif

#ifdef BUILD_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256
    "ECDHE-PSK-AES128-CBC-SHA256",
#endif

#ifdef BUILD_TLS_PSK_WITH_CHACHA20_POLY1305_SHA256
    "PSK-CHACHA20-POLY1305",
#endif

#ifdef BUILD_TLS_ECDHE_PSK_WITH_CHACHA20_POLY1305_SHA256
    "ECDHE-PSK-CHACHA20-POLY1305",
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_CHACHA20_POLY1305_SHA256
    "DHE-PSK-CHACHA20-POLY1305",
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA
    "EDH-RSA-DES-CBC3-SHA",
#endif

#ifdef BUILD_TLS_AES_128_GCM_SHA256
    "TLS13-AES128-GCM-SHA256",
#endif

#ifdef BUILD_TLS_AES_256_GCM_SHA384
    "TLS13-AES256-GCM-SHA384",
#endif

#ifdef BUILD_TLS_CHACHA20_POLY1305_SHA256
    "TLS13-CHACHA20-POLY1305-SHA256",
#endif

#ifdef BUILD_TLS_AES_128_CCM_SHA256
    "TLS13-AES128-CCM-SHA256",
#endif

#ifdef BUILD_TLS_AES_128_CCM_8_SHA256
    "TLS13-AES128-CCM-8-SHA256",
#endif

#ifdef BUILD_WDM_WITH_NULL_SHA256
    "WDM-NULL-SHA256",
#endif
};


/* cipher suite number that matches above name table */
static int cipher_name_idx[] =
{

#ifdef BUILD_SSL_RSA_WITH_RC4_128_SHA
    SSL_RSA_WITH_RC4_128_SHA,
#endif

#ifdef BUILD_SSL_RSA_WITH_RC4_128_MD5
    SSL_RSA_WITH_RC4_128_MD5,
#endif

#ifdef BUILD_SSL_RSA_WITH_3DES_EDE_CBC_SHA
    SSL_RSA_WITH_3DES_EDE_CBC_SHA,
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_128_CBC_SHA
    TLS_RSA_WITH_AES_128_CBC_SHA,
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_256_CBC_SHA
    TLS_RSA_WITH_AES_256_CBC_SHA,
#endif

#ifdef BUILD_TLS_RSA_WITH_NULL_SHA
    TLS_RSA_WITH_NULL_SHA,
#endif

#ifdef BUILD_TLS_RSA_WITH_NULL_SHA256
    TLS_RSA_WITH_NULL_SHA256,
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_AES_128_CBC_SHA
    TLS_DHE_RSA_WITH_AES_128_CBC_SHA,
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_AES_256_CBC_SHA
    TLS_DHE_RSA_WITH_AES_256_CBC_SHA,
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_AES_256_GCM_SHA384
    TLS_DHE_PSK_WITH_AES_256_GCM_SHA384,
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_AES_128_GCM_SHA256
    TLS_DHE_PSK_WITH_AES_128_GCM_SHA256,
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_256_GCM_SHA384
    TLS_PSK_WITH_AES_256_GCM_SHA384,
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_128_GCM_SHA256
    TLS_PSK_WITH_AES_128_GCM_SHA256,
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_AES_256_CBC_SHA384
    TLS_DHE_PSK_WITH_AES_256_CBC_SHA384,
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_AES_128_CBC_SHA256
    TLS_DHE_PSK_WITH_AES_128_CBC_SHA256,
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_256_CBC_SHA384
    TLS_PSK_WITH_AES_256_CBC_SHA384,
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_128_CBC_SHA256
    TLS_PSK_WITH_AES_128_CBC_SHA256,
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_128_CBC_SHA
    TLS_PSK_WITH_AES_128_CBC_SHA,
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_256_CBC_SHA
    TLS_PSK_WITH_AES_256_CBC_SHA,
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_AES_128_CCM
    TLS_DHE_PSK_WITH_AES_128_CCM,
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_AES_256_CCM
    TLS_DHE_PSK_WITH_AES_256_CCM,
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_128_CCM
    TLS_PSK_WITH_AES_128_CCM,
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_256_CCM
    TLS_PSK_WITH_AES_256_CCM,
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_128_CCM_8
    TLS_PSK_WITH_AES_128_CCM_8,
#endif

#ifdef BUILD_TLS_PSK_WITH_AES_256_CCM_8
    TLS_PSK_WITH_AES_256_CCM_8,
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_NULL_SHA384
    TLS_DHE_PSK_WITH_NULL_SHA384,
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_NULL_SHA256
    TLS_DHE_PSK_WITH_NULL_SHA256,
#endif

#ifdef BUILD_TLS_PSK_WITH_NULL_SHA384
    TLS_PSK_WITH_NULL_SHA384,
#endif

#ifdef BUILD_TLS_PSK_WITH_NULL_SHA256
    TLS_PSK_WITH_NULL_SHA256,
#endif

#ifdef BUILD_TLS_PSK_WITH_NULL_SHA
    TLS_PSK_WITH_NULL_SHA,
#endif

#ifdef BUILD_TLS_RSA_WITH_HC_128_MD5
    TLS_RSA_WITH_HC_128_MD5,
#endif

#ifdef BUILD_TLS_RSA_WITH_HC_128_SHA
    TLS_RSA_WITH_HC_128_SHA,
#endif

#ifdef BUILD_TLS_RSA_WITH_HC_128_B2B256
    TLS_RSA_WITH_HC_128_B2B256,
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_128_CBC_B2B256
    TLS_RSA_WITH_AES_128_CBC_B2B256,
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_256_CBC_B2B256
    TLS_RSA_WITH_AES_256_CBC_B2B256,
#endif

#ifdef BUILD_TLS_RSA_WITH_RABBIT_SHA
    TLS_RSA_WITH_RABBIT_SHA,
#endif

#ifdef BUILD_TLS_NTRU_RSA_WITH_RC4_128_SHA
    TLS_NTRU_RSA_WITH_RC4_128_SHA,
#endif

#ifdef BUILD_TLS_NTRU_RSA_WITH_3DES_EDE_CBC_SHA
    TLS_NTRU_RSA_WITH_3DES_EDE_CBC_SHA,
#endif

#ifdef BUILD_TLS_NTRU_RSA_WITH_AES_128_CBC_SHA
    TLS_NTRU_RSA_WITH_AES_128_CBC_SHA,
#endif

#ifdef BUILD_TLS_NTRU_RSA_WITH_AES_256_CBC_SHA
    TLS_NTRU_RSA_WITH_AES_256_CBC_SHA,
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_128_CCM_8
    TLS_RSA_WITH_AES_128_CCM_8,
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_256_CCM_8
    TLS_RSA_WITH_AES_256_CCM_8,
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_CCM
    TLS_ECDHE_ECDSA_WITH_AES_128_CCM,
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8
    TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8,
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8
    TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8,
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA
    TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA,
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA
    TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA,
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA
    TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA,
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA
    TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA,
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_RC4_128_SHA
    TLS_ECDHE_RSA_WITH_RC4_128_SHA,
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA
    TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA,
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_RC4_128_SHA
    TLS_ECDHE_ECDSA_WITH_RC4_128_SHA,
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA
    TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA,
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_128_CBC_SHA256
    TLS_RSA_WITH_AES_128_CBC_SHA256,
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_256_CBC_SHA256
    TLS_RSA_WITH_AES_256_CBC_SHA256,
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256
    TLS_DHE_RSA_WITH_AES_128_CBC_SHA256,
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256
    TLS_DHE_RSA_WITH_AES_256_CBC_SHA256,
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA
    TLS_ECDH_RSA_WITH_AES_128_CBC_SHA,
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA
    TLS_ECDH_RSA_WITH_AES_256_CBC_SHA,
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA
    TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA,
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA
    TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA,
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_RC4_128_SHA
    TLS_ECDH_RSA_WITH_RC4_128_SHA,
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA
    TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA,
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_RC4_128_SHA
    TLS_ECDH_ECDSA_WITH_RC4_128_SHA,
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA
    TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA,
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_128_GCM_SHA256
    TLS_RSA_WITH_AES_128_GCM_SHA256,
#endif

#ifdef BUILD_TLS_RSA_WITH_AES_256_GCM_SHA384
    TLS_RSA_WITH_AES_256_GCM_SHA384,
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256
    TLS_DHE_RSA_WITH_AES_128_GCM_SHA256,
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384
    TLS_DHE_RSA_WITH_AES_256_GCM_SHA384,
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256
    TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384
    TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256
    TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384
    TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256
    TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256,
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384
    TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384,
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256
    TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256,
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384
    TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384,
#endif

#ifdef BUILD_TLS_RSA_WITH_CAMELLIA_128_CBC_SHA
    TLS_RSA_WITH_CAMELLIA_128_CBC_SHA,
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA
    TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA,
#endif

#ifdef BUILD_TLS_RSA_WITH_CAMELLIA_256_CBC_SHA
    TLS_RSA_WITH_CAMELLIA_256_CBC_SHA,
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA
    TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA,
#endif

#ifdef BUILD_TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256
    TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256,
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256
    TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256,
#endif

#ifdef BUILD_TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256
    TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256,
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256
    TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256,
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256
    TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256,
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256
    TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256,
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256
    TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256,
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256
    TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256,
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384
    TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384,
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384
    TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384,
#endif

#ifdef BUILD_TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384
    TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384,
#endif

#ifdef BUILD_TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384
    TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384,
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256
    TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256,
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256
    TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256,
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256
    TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256,
#endif

#ifdef BUILD_TLS_ECDHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256
    TLS_ECDHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256,
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_CHACHA20_OLD_POLY1305_SHA256
    TLS_ECDHE_ECDSA_WITH_CHACHA20_OLD_POLY1305_SHA256,
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256
    TLS_DHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256,
#endif

#ifdef BUILD_TLS_DH_anon_WITH_AES_128_CBC_SHA
    TLS_DH_anon_WITH_AES_128_CBC_SHA,
#endif

#ifdef BUILD_TLS_QSH
    TLS_QSH,
#endif

#ifdef HAVE_RENEGOTIATION_INDICATION
    TLS_EMPTY_RENEGOTIATION_INFO_SCSV,
#endif

#ifdef BUILD_SSL_RSA_WITH_IDEA_CBC_SHA
    SSL_RSA_WITH_IDEA_CBC_SHA,
#endif

#ifdef BUILD_TLS_ECDHE_ECDSA_WITH_NULL_SHA
    TLS_ECDHE_ECDSA_WITH_NULL_SHA,
#endif

#ifdef BUILD_TLS_ECDHE_PSK_WITH_NULL_SHA256
    TLS_ECDHE_PSK_WITH_NULL_SHA256,
#endif

#ifdef BUILD_TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256
    TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256,
#endif

#ifdef BUILD_TLS_PSK_WITH_CHACHA20_POLY1305_SHA256
    TLS_PSK_WITH_CHACHA20_POLY1305_SHA256,
#endif

#ifdef BUILD_TLS_ECDHE_PSK_WITH_CHACHA20_POLY1305_SHA256
    TLS_ECDHE_PSK_WITH_CHACHA20_POLY1305_SHA256,
#endif

#ifdef BUILD_TLS_DHE_PSK_WITH_CHACHA20_POLY1305_SHA256
    TLS_DHE_PSK_WITH_CHACHA20_POLY1305_SHA256,
#endif

#ifdef BUILD_TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA
    TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA,
#endif

#ifdef BUILD_TLS_AES_128_GCM_SHA256
    TLS_AES_128_GCM_SHA256,
#endif

#ifdef BUILD_TLS_AES_256_GCM_SHA384
    TLS_AES_256_GCM_SHA384,
#endif

#ifdef BUILD_TLS_CHACHA20_POLY1305_SHA256
    TLS_CHACHA20_POLY1305_SHA256,
#endif

#ifdef BUILD_TLS_AES_128_CCM_SHA256
    TLS_AES_128_CCM_SHA256,
#endif

#ifdef BUILD_TLS_AES_128_CCM_8_SHA256
    TLS_AES_128_CCM_8_SHA256,
#endif

#ifdef BUILD_WDM_WITH_NULL_SHA256
    WDM_WITH_NULL_SHA256,
#endif
};


/* returns the cipher_names array */
const char* const* GetCipherNames(void)
{
    return cipher_names;
}


/* returns the size of the cipher_names array */
int GetCipherNamesSize(void)
{
    return (int)(sizeof(cipher_names) / sizeof(char*));
}

/* gets cipher name in the format DHE-RSA-... rather then TLS_DHE... */
const char* GetCipherNameInternal(const char* cipherName, int cipherSuite)
{
    const char* result = NULL;
    const char* first;
    int i;

    if (cipherName == NULL) {
        WOLFSSL_MSG("Bad argument");
        return NULL;
    }

    first = (XSTRSTR(cipherName, "CHACHA")) ? "CHACHA"
          : (XSTRSTR(cipherName, "EC"))     ? "EC"
          : (XSTRSTR(cipherName, "CCM"))    ? "CCM"
          : NULL; /* normal */

    for (i = 0; i < (int)(sizeof(cipher_name_idx)/sizeof(int)); i++) {
        if (cipher_name_idx[i] == cipherSuite) {
            const char* nameFound = cipher_names[i];

            /* extra sanity check on returned cipher name */
            if (nameFound == NULL) {
                continue;
            }

            /* if first is null then not any */
            if (first == NULL) {
                if (    !XSTRSTR(nameFound, "CHACHA") &&
                        !XSTRSTR(nameFound, "EC") &&
                        !XSTRSTR(nameFound, "CCM")) {
                    result = nameFound;
                    break;
                }
            }
            else if (XSTRSTR(nameFound, first)) {
                result = nameFound;
                break;
            }
        }
    }

    return result;
}

const char* wolfSSL_get_cipher_name_internal(WOLFSSL* ssl)
{
    if (ssl == NULL) {
        WOLFSSL_MSG("Bad argument");
        return NULL;
    }

    return GetCipherNameInternal(
        wolfSSL_CIPHER_get_name(&ssl->cipher),
        ssl->options.cipherSuite);
}


const char* wolfSSL_get_cipher_name_from_suite(const unsigned char cipherSuite,
    const unsigned char cipherSuite0)
{

    WOLFSSL_ENTER("wolfSSL_get_cipher_name_from_suite");

    (void)cipherSuite;
    (void)cipherSuite0;

#ifndef NO_ERROR_STRINGS

#if defined(HAVE_CHACHA)
    if (cipherSuite0 == CHACHA_BYTE) {
        /* ChaCha suites */
        switch (cipherSuite) {
#ifdef HAVE_POLY1305
#ifndef NO_RSA
            case TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256 :
                return "TLS_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256";

            case TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256 :
                return "TLS_DHE_RSA_WITH_CHACHA20_POLY1305_SHA256";

            case TLS_ECDHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256 :
                return "TLS_ECDHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256";

            case TLS_DHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256 :
                return "TLS_DHE_RSA_WITH_CHACHA20_OLD_POLY1305_SHA256";
#endif
            case TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256 :
                return "TLS_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256";

            case TLS_ECDHE_ECDSA_WITH_CHACHA20_OLD_POLY1305_SHA256 :
                return "TLS_ECDHE_ECDSA_WITH_CHACHA20_OLD_POLY1305_SHA256";
#ifndef NO_PSK
            case TLS_ECDHE_PSK_WITH_CHACHA20_POLY1305_SHA256 :
                return "TLS_ECDHE_PSK_WITH_CHACHA20_POLY1305_SHA256";
            case TLS_PSK_WITH_CHACHA20_POLY1305_SHA256 :
                return "TLS_PSK_WITH_CHACHA20_POLY1305_SHA256";
            case TLS_DHE_PSK_WITH_CHACHA20_POLY1305_SHA256 :
                return "TLS_DHE_PSK_WITH_CHACHA20_POLY1305_SHA256";
#endif /* NO_PSK */
#endif /* HAVE_POLY1305 */
        } /* switch */
    } /* chacha */
#endif /* HAVE_CHACHA */

#if defined(HAVE_ECC) || defined(HAVE_AESCCM)
        /* Awkwardly, the ECC cipher suites use the ECC_BYTE as expected,
         * but the AES-CCM cipher suites also use it, even the ones that
         * aren't ECC. */
    if (cipherSuite0 == ECC_BYTE) {
        /* ECC suites */
        switch (cipherSuite) {
#ifdef HAVE_ECC
    #ifndef NO_RSA
            case TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256 :
                return "TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256";
    #endif /* !NO_RSA */
            case TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256 :
                return "TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256";
    #ifndef NO_RSA
            case TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256 :
                return "TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256";
    #endif /* !NO_RSA */
            case TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256 :
                return "TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256";
    #ifndef NO_RSA
            case TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384 :
                return "TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA384";
    #endif /* !NO_RSA */
            case TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384 :
                return "TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA384";
    #ifndef NO_RSA
            case TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384 :
                return "TLS_ECDH_RSA_WITH_AES_256_CBC_SHA384";
    #endif /* !NO_RSA */
            case TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384 :
                return "TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA384";
#ifndef NO_SHA
    #ifndef NO_RSA
            case TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA :
                return "TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA";
            case TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA :
                return "TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA";
    #endif /* !NO_RSA */
            case TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA :
                return "TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA";
            case TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA :
                return "TLS_ECDHE_ECDSA_WITH_AES_256_CBC_SHA";
    #ifndef NO_RC4
        #ifndef NO_RSA
            case TLS_ECDHE_RSA_WITH_RC4_128_SHA :
                return "TLS_ECDHE_RSA_WITH_RC4_128_SHA";
        #endif /* !NO_RSA */
            case TLS_ECDHE_ECDSA_WITH_RC4_128_SHA :
                return "TLS_ECDHE_ECDSA_WITH_RC4_128_SHA";
    #endif /* !NO_RC4 */
    #ifndef NO_DES3
        #ifndef NO_RSA
            case TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA :
                return "TLS_ECDHE_RSA_WITH_3DES_EDE_CBC_SHA";
        #endif /* !NO_RSA */
            case TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA :
                return "TLS_ECDHE_ECDSA_WITH_3DES_EDE_CBC_SHA";
    #endif /* !NO_DES3 */

    #ifndef NO_RSA
            case TLS_ECDH_RSA_WITH_AES_128_CBC_SHA :
                return "TLS_ECDH_RSA_WITH_AES_128_CBC_SHA";
            case TLS_ECDH_RSA_WITH_AES_256_CBC_SHA :
                return "TLS_ECDH_RSA_WITH_AES_256_CBC_SHA";
    #endif /* !NO_RSA */
            case TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA :
                return "TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA";
            case TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA :
                return "TLS_ECDH_ECDSA_WITH_AES_256_CBC_SHA";
    #ifndef NO_RC4
        #ifndef NO_RSA
            case TLS_ECDH_RSA_WITH_RC4_128_SHA :
                return "TLS_ECDH_RSA_WITH_RC4_128_SHA";
        #endif /* !NO_RSA */
            case TLS_ECDH_ECDSA_WITH_RC4_128_SHA :
                return "TLS_ECDH_ECDSA_WITH_RC4_128_SHA";
    #endif /* !NO_RC4 */
    #ifndef NO_DES3
        #ifndef NO_RSA
            case TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA :
                return "TLS_ECDH_RSA_WITH_3DES_EDE_CBC_SHA";
        #endif /* !NO_RSA */
            case TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA :
                return "TLS_ECDH_ECDSA_WITH_3DES_EDE_CBC_SHA";
    #endif /* !NO_DES3 */
#endif /* HAVE_ECC */

#ifdef HAVE_AESGCM
    #ifndef NO_RSA
            case TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256 :
                return "TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256";
            case TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384 :
                return "TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384";
    #endif /* !NO_RSA */
            case TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256 :
                return "TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256";
            case TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384 :
                return "TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384";
    #ifndef NO_RSA
            case TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256 :
                return "TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256";
            case TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384 :
                return "TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384";
    #endif /* !NO_RSA */
            case TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256 :
                return "TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256";
            case TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384 :
                return "TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384";
#endif /* HAVE_AESGCM */

            case TLS_ECDHE_ECDSA_WITH_NULL_SHA :
                return "TLS_ECDHE_ECDSA_WITH_NULL_SHA";
    #ifndef NO_PSK
            case TLS_ECDHE_PSK_WITH_NULL_SHA256 :
                return "TLS_ECDHE_PSK_WITH_NULL_SHA256";
            case TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256 :
                return "TLS_ECDHE_PSK_WITH_AES_128_CBC_SHA256";
    #endif /* !NO_PSK */
    #ifndef NO_RSA
            case TLS_RSA_WITH_AES_128_CCM_8 :
                return "TLS_RSA_WITH_AES_128_CCM_8";
            case TLS_RSA_WITH_AES_256_CCM_8 :
                return "TLS_RSA_WITH_AES_256_CCM_8";
    #endif /* !NO_RSA */
    #ifndef NO_PSK
            case TLS_PSK_WITH_AES_128_CCM_8 :
                return "TLS_PSK_WITH_AES_128_CCM_8";
            case TLS_PSK_WITH_AES_256_CCM_8 :
                return "TLS_PSK_WITH_AES_256_CCM_8";
            case TLS_PSK_WITH_AES_128_CCM :
                return "TLS_PSK_WITH_AES_128_CCM";
            case TLS_PSK_WITH_AES_256_CCM :
                return "TLS_PSK_WITH_AES_256_CCM";
            case TLS_DHE_PSK_WITH_AES_128_CCM :
                return "TLS_DHE_PSK_WITH_AES_128_CCM";
            case TLS_DHE_PSK_WITH_AES_256_CCM :
                return "TLS_DHE_PSK_WITH_AES_256_CCM";
    #endif /* !NO_PSK */
    #ifdef HAVE_ECC
            case TLS_ECDHE_ECDSA_WITH_AES_128_CCM:
                return "TLS_ECDHE_ECDSA_WITH_AES_128_CCM";
            case TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8:
                return "TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8";
            case TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8 :
                return "TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8";
    #endif /* HAVE_ECC */
#endif /* HAVE_AESGCM */

            default:
                return "NONE";
        } /* switch */
    } /* ECC and AES CCM/GCM */
#endif  /* HAVE_ECC || HAVE_AESCCM*/

    if (cipherSuite0 == TLS13_BYTE) {
        /* TLS v1.3 suites */
        switch (cipherSuite) {
#ifdef WOLFSSL_TLS13
    #ifdef HAVE_AESGCM
            case TLS_AES_128_GCM_SHA256 :
                return "TLS_AES_128_GCM_SHA256";
            case TLS_AES_256_GCM_SHA384 :
                return "TLS_AES_256_GCM_SHA384";
    #endif

    #ifdef HAVE_CHACHA
            case TLS_CHACHA20_POLY1305_SHA256 :
                return "TLS_CHACHA20_POLY1305_SHA256";
    #endif

    #ifdef HAVE_AESCCM
            case TLS_AES_128_CCM_SHA256 :
                return "TLS_AES_128_CCM_SHA256";
            case TLS_AES_128_CCM_8_SHA256 :
                return "TLS_AES_256_CCM_8_SHA256";
    #endif
#endif

            default:
                return "NONE";
        }
    }

    if (cipherSuite0 != ECC_BYTE &&
        cipherSuite0 != CHACHA_BYTE &&
        cipherSuite0 != TLS13_BYTE) {

        /* normal suites */
        switch (cipherSuite) {
#ifndef NO_RSA
    #ifndef NO_RC4
        #ifndef NO_SHA
            case SSL_RSA_WITH_RC4_128_SHA :
                return "SSL_RSA_WITH_RC4_128_SHA";
        #endif /* !NO_SHA */
        #ifndef NO_MD5
            case SSL_RSA_WITH_RC4_128_MD5 :
                return "SSL_RSA_WITH_RC4_128_MD5";
        #endif /* !NO_MD5 */
    #endif /* !NO_RC4 */
    #ifndef NO_SHA
        #ifndef NO_DES3
            case SSL_RSA_WITH_3DES_EDE_CBC_SHA :
                return "SSL_RSA_WITH_3DES_EDE_CBC_SHA";
        #endif /* !NO_DES3 */
        #ifdef HAVE_IDEA
            case SSL_RSA_WITH_IDEA_CBC_SHA :
                return "SSL_RSA_WITH_IDEA_CBC_SHA";
        #endif /* HAVE_IDEA */

            case TLS_RSA_WITH_AES_128_CBC_SHA :
                return "TLS_RSA_WITH_AES_128_CBC_SHA";
            case TLS_RSA_WITH_AES_256_CBC_SHA :
                return "TLS_RSA_WITH_AES_256_CBC_SHA";
    #endif /* !NO_SHA */
            case TLS_RSA_WITH_AES_128_CBC_SHA256 :
                return "TLS_RSA_WITH_AES_128_CBC_SHA256";
            case TLS_RSA_WITH_AES_256_CBC_SHA256 :
                return "TLS_RSA_WITH_AES_256_CBC_SHA256";
    #ifdef HAVE_BLAKE2
            case TLS_RSA_WITH_AES_128_CBC_B2B256:
                return "TLS_RSA_WITH_AES_128_CBC_B2B256";
            case TLS_RSA_WITH_AES_256_CBC_B2B256:
                return "TLS_RSA_WITH_AES_256_CBC_B2B256";
    #endif /* HAVE_BLAKE2 */
    #ifndef NO_SHA
            case TLS_RSA_WITH_NULL_SHA :
                return "TLS_RSA_WITH_NULL_SHA";
    #endif /* !NO_SHA */
            case TLS_RSA_WITH_NULL_SHA256 :
                return "TLS_RSA_WITH_NULL_SHA256";
#endif /* NO_RSA */

#ifndef NO_PSK
    #ifndef NO_SHA
            case TLS_PSK_WITH_AES_128_CBC_SHA :
                return "TLS_PSK_WITH_AES_128_CBC_SHA";
            case TLS_PSK_WITH_AES_256_CBC_SHA :
                return "TLS_PSK_WITH_AES_256_CBC_SHA";
    #endif /* !NO_SHA */
    #ifndef NO_SHA256
            case TLS_PSK_WITH_AES_128_CBC_SHA256 :
                return "TLS_PSK_WITH_AES_128_CBC_SHA256";
            case TLS_PSK_WITH_NULL_SHA256 :
                return "TLS_PSK_WITH_NULL_SHA256";
            case TLS_DHE_PSK_WITH_AES_128_CBC_SHA256 :
                return "TLS_DHE_PSK_WITH_AES_128_CBC_SHA256";
            case TLS_DHE_PSK_WITH_NULL_SHA256 :
                return "TLS_DHE_PSK_WITH_NULL_SHA256";
        #ifdef HAVE_AESGCM
            case TLS_PSK_WITH_AES_128_GCM_SHA256 :
                return "TLS_PSK_WITH_AES_128_GCM_SHA256";
            case TLS_DHE_PSK_WITH_AES_128_GCM_SHA256 :
                return "TLS_DHE_PSK_WITH_AES_128_GCM_SHA256";
        #endif /* HAVE_AESGCM */
    #endif /* !NO_SHA256 */
    #ifdef WOLFSSL_SHA384
            case TLS_PSK_WITH_AES_256_CBC_SHA384 :
                return "TLS_PSK_WITH_AES_256_CBC_SHA384";
            case TLS_PSK_WITH_NULL_SHA384 :
                return "TLS_PSK_WITH_NULL_SHA384";
            case TLS_DHE_PSK_WITH_AES_256_CBC_SHA384 :
                return "TLS_DHE_PSK_WITH_AES_256_CBC_SHA384";
            case TLS_DHE_PSK_WITH_NULL_SHA384 :
                return "TLS_DHE_PSK_WITH_NULL_SHA384";
        #ifdef HAVE_AESGCM
            case TLS_PSK_WITH_AES_256_GCM_SHA384 :
                return "TLS_PSK_WITH_AES_256_GCM_SHA384";
            case TLS_DHE_PSK_WITH_AES_256_GCM_SHA384 :
                return "TLS_DHE_PSK_WITH_AES_256_GCM_SHA384";
        #endif /* HAVE_AESGCM */
    #endif /* WOLFSSL_SHA384 */
    #ifndef NO_SHA
            case TLS_PSK_WITH_NULL_SHA :
                return "TLS_PSK_WITH_NULL_SHA";
    #endif /* !NO_SHA */
    #endif /* NO_PSK */

    #ifndef NO_RSA
            case TLS_DHE_RSA_WITH_AES_128_CBC_SHA256 :
                return "TLS_DHE_RSA_WITH_AES_128_CBC_SHA256";
            case TLS_DHE_RSA_WITH_AES_256_CBC_SHA256 :
                return "TLS_DHE_RSA_WITH_AES_256_CBC_SHA256";
    #ifndef NO_SHA
            case TLS_DHE_RSA_WITH_AES_128_CBC_SHA :
                return "TLS_DHE_RSA_WITH_AES_128_CBC_SHA";
            case TLS_DHE_RSA_WITH_AES_256_CBC_SHA :
                return "TLS_DHE_RSA_WITH_AES_256_CBC_SHA";
        #ifndef NO_DES3
            case TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA:
                return "TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA";
         #endif
    #endif /* !NO_RSA */
    #ifndef NO_HC128
        #ifndef NO_MD5
            case TLS_RSA_WITH_HC_128_MD5 :
                return "TLS_RSA_WITH_HC_128_MD5";
        #endif /* !NO_MD5 */
        #ifndef NO_SHA
            case TLS_RSA_WITH_HC_128_SHA :
                return "TLS_RSA_WITH_HC_128_SHA";
        #endif /* !NO_SHA */
        #ifdef HAVE_BLAKE2
            case TLS_RSA_WITH_HC_128_B2B256:
                return "TLS_RSA_WITH_HC_128_B2B256";
        #endif /* HAVE_BLAKE2 */
    #endif /* !NO_HC128 */
    #ifndef NO_SHA
        #ifndef NO_RABBIT
            case TLS_RSA_WITH_RABBIT_SHA :
                return "TLS_RSA_WITH_RABBIT_SHA";
        #endif /* !NO_RABBIT */
        #ifdef HAVE_NTRU
            #ifndef NO_RC4
            case TLS_NTRU_RSA_WITH_RC4_128_SHA :
                return "TLS_NTRU_RSA_WITH_RC4_128_SHA";
            #endif /* !NO_RC4 */
            #ifndef NO_DES3
            case TLS_NTRU_RSA_WITH_3DES_EDE_CBC_SHA :
                return "TLS_NTRU_RSA_WITH_3DES_EDE_CBC_SHA";
            #endif /* !NO_DES3 */
            case TLS_NTRU_RSA_WITH_AES_128_CBC_SHA :
                return "TLS_NTRU_RSA_WITH_AES_128_CBC_SHA";
            case TLS_NTRU_RSA_WITH_AES_256_CBC_SHA :
                return "TLS_NTRU_RSA_WITH_AES_256_CBC_SHA";
        #endif /* HAVE_NTRU */

        #ifdef HAVE_QSH
            case TLS_QSH :
                return "TLS_QSH";
        #endif /* HAVE_QSH */
    #endif /* !NO_SHA */

            case TLS_RSA_WITH_AES_128_GCM_SHA256 :
                return "TLS_RSA_WITH_AES_128_GCM_SHA256";
            case TLS_RSA_WITH_AES_256_GCM_SHA384 :
                return "TLS_RSA_WITH_AES_256_GCM_SHA384";
            case TLS_DHE_RSA_WITH_AES_128_GCM_SHA256 :
                return "TLS_DHE_RSA_WITH_AES_128_GCM_SHA256";
            case TLS_DHE_RSA_WITH_AES_256_GCM_SHA384 :
                return "TLS_DHE_RSA_WITH_AES_256_GCM_SHA384";
    #ifndef NO_SHA
            case TLS_RSA_WITH_CAMELLIA_128_CBC_SHA :
                return "TLS_RSA_WITH_CAMELLIA_128_CBC_SHA";
            case TLS_RSA_WITH_CAMELLIA_256_CBC_SHA :
                return "TLS_RSA_WITH_CAMELLIA_256_CBC_SHA";
    #endif /* !NO_SHA */
            case TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256 :
                return "TLS_RSA_WITH_CAMELLIA_128_CBC_SHA256";
            case TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256 :
                return "TLS_RSA_WITH_CAMELLIA_256_CBC_SHA256";
    #ifndef NO_SHA
            case TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA :
                return "TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA";
            case TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA :
                return "TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA";
    #endif /* !NO_SHA */
            case TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256 :
                return "TLS_DHE_RSA_WITH_CAMELLIA_128_CBC_SHA256";
            case TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256 :
                return "TLS_DHE_RSA_WITH_CAMELLIA_256_CBC_SHA256";
#endif /* !NO_PSK */

#ifdef BUILD_TLS_DH_anon_WITH_AES_128_CBC_SHA
            case TLS_DH_anon_WITH_AES_128_CBC_SHA :
                return "TLS_DH_anon_WITH_AES_128_CBC_SHA";
#endif
#ifdef BUILD_WDM_WITH_NULL_SHA256
            case WDM_WITH_NULL_SHA256 :
                return "WDM_WITH_NULL_SHA256";
#endif
            default:
                return "NONE";
        } /* switch */
    } /* normal / PSK */
#endif /* NO_ERROR_STRINGS */

    return "NONE";
}


/**
Set the enabled cipher suites.

@param [out] suites Suites structure.
@param [in]  list   List of cipher suites, only supports full name from
                    cipher_names[] delimited by ':'.

@return true on success, else false.
*/
int SetCipherList(WOLFSSL_CTX* ctx, Suites* suites, const char* list)
{
    int       ret          = 0;
    int       idx          = 0;
    int       haveRSAsig   = 0;
    int       haveECDSAsig = 0;
    int       haveAnon     = 0;
    const int suiteSz      = GetCipherNamesSize();
    char*     next         = (char*)list;

    if (suites == NULL || list == NULL) {
        WOLFSSL_MSG("SetCipherList parameter error");
        return 0;
    }

    if (next[0] == 0 || XSTRNCMP(next, "ALL", 3) == 0 ||
                        XSTRNCMP(next, "DEFAULT", 7) == 0)
        return 1; /* wolfSSL defualt */

    do {
        char*  current = next;
        char   name[MAX_SUITE_NAME + 1];
        int    i;
        word32 length;

        next   = XSTRSTR(next, ":");
        length = min(sizeof(name), !next ? (word32)XSTRLEN(current) /* last */
                                         : (word32)(next - current));

        XSTRNCPY(name, current, length);
        name[(length == sizeof(name)) ? length - 1 : length] = 0;

        for (i = 0; i < suiteSz; i++) {
            if (XSTRNCMP(name, cipher_names[i], sizeof(name)) == 0) {
            #ifdef WOLFSSL_DTLS
                /* don't allow stream ciphers with DTLS */
                if (ctx->method->version.major == DTLS_MAJOR) {
                    if (XSTRSTR(name, "RC4") ||
                        XSTRSTR(name, "HC128") ||
                        XSTRSTR(name, "RABBIT"))
                    {
                        WOLFSSL_MSG("Stream ciphers not supported with DTLS");
                        continue;
                    }

                }
            #endif /* WOLFSSL_DTLS */

                if (idx + 1 >= WOLFSSL_MAX_SUITE_SZ) {
                    WOLFSSL_MSG("WOLFSSL_MAX_SUITE_SZ set too low");
                    return 0; /* suites buffer not large enough, error out */
                }

                suites->suites[idx++] = (XSTRSTR(name, "TLS13"))  ? TLS13_BYTE
                                      : (XSTRSTR(name, "CHACHA")) ? CHACHA_BYTE
                                      : (XSTRSTR(name, "QSH"))    ? QSH_BYTE
                                      : (XSTRSTR(name, "EC"))     ? ECC_BYTE
                                      : (XSTRSTR(name, "CCM"))    ? ECC_BYTE
                                      : 0x00; /* normal */
                suites->suites[idx++] = (byte)cipher_name_idx[i];

                /* The suites are either ECDSA, RSA, PSK, or Anon. The RSA
                 * suites don't necessarily have RSA in the name. */
                if (XSTRSTR(name, "TLS13")) {
                    haveRSAsig = 1;
                    haveECDSAsig = 1;
                }
                else if ((haveECDSAsig == 0) && XSTRSTR(name, "ECDSA"))
                    haveECDSAsig = 1;
                else if (XSTRSTR(name, "ADH"))
                    haveAnon = 1;
                else if ((haveRSAsig == 0) && (XSTRSTR(name, "PSK") == NULL))
                    haveRSAsig = 1;

                ret = 1; /* found at least one */
                break;
            }
        }
    }
    while (next++); /* ++ needed to skip ':' */

    if (ret) {
        int keySz = 0;
    #ifndef NO_CERTS
        keySz = ctx->privateKeySz;
    #endif
        suites->setSuites = 1;
        suites->suiteSz   = (word16)idx;
        InitSuitesHashSigAlgo(suites, haveECDSAsig, haveRSAsig, haveAnon, 1,
                              keySz);
    }

    (void)ctx;

    return ret;
}

#if !defined(NO_WOLFSSL_SERVER) || !defined(NO_CERTS)
void PickHashSigAlgo(WOLFSSL* ssl, const byte* hashSigAlgo,
                     word32 hashSigAlgoSz)
{
    word32 i;

    ssl->suites->sigAlgo = ssl->specs.sig_algo;

    /* set defaults */
    if (IsAtLeastTLSv1_3(ssl->version)) {
        ssl->suites->hashAlgo = sha256_mac;
    #ifndef NO_CERTS
        ssl->suites->sigAlgo = ssl->buffers.keyType;
    #endif
    }
    else if (IsAtLeastTLSv1_2(ssl)) {
    #ifdef WOLFSSL_ALLOW_TLS_SHA1
        ssl->suites->hashAlgo = sha_mac;
    #else
        ssl->suites->hashAlgo = sha256_mac;
    #endif
    }
    else {
        ssl->suites->hashAlgo = sha_mac;
    }

    /* i+1 since peek a byte ahead for type */
    for (i = 0; (i+1) < hashSigAlgoSz; i += HELLO_EXT_SIGALGO_SZ) {
        byte hashAlgo = 0, sigAlgo = 0;

        DecodeSigAlg(&hashSigAlgo[i], &hashAlgo, &sigAlgo);
    #ifdef HAVE_ECC
        if (ssl->pkCurveOID == ECC_ED25519_OID && sigAlgo != ed25519_sa_algo)
            continue;

        if (sigAlgo == ed25519_sa_algo &&
                                      ssl->suites->sigAlgo == ecc_dsa_sa_algo) {
            ssl->suites->sigAlgo = sigAlgo;
            ssl->suites->hashAlgo = sha512_mac;
            break;
        }
    #endif
        if (sigAlgo == ssl->suites->sigAlgo || (sigAlgo == rsa_pss_sa_algo &&
                                         ssl->suites->sigAlgo == rsa_sa_algo)) {
            if (hashAlgo == sha_mac) {
                ssl->suites->sigAlgo = sigAlgo;
                break;
            }
            #ifndef NO_SHA256
            else if (hashAlgo == sha256_mac) {
                ssl->suites->hashAlgo = sha256_mac;
                ssl->suites->sigAlgo = sigAlgo;
                break;
            }
            #endif
            #ifdef WOLFSSL_SHA384
            else if (hashAlgo == sha384_mac) {
                ssl->suites->hashAlgo = sha384_mac;
                ssl->suites->sigAlgo = sigAlgo;
                break;
            }
            #endif
            #ifdef WOLFSSL_SHA512
            else if (hashAlgo == sha512_mac) {
                ssl->suites->hashAlgo = sha512_mac;
                ssl->suites->sigAlgo = sigAlgo;
                break;
            }
            #endif
        }
        else if (ssl->specs.sig_algo == 0) {
            ssl->suites->hashAlgo = ssl->specs.mac_algorithm;
        }
    }

}
#endif /* !defined(NO_WOLFSSL_SERVER) || !defined(NO_CERTS) */

#ifdef WOLFSSL_CALLBACKS

    /* Initialisze HandShakeInfo */
    void InitHandShakeInfo(HandShakeInfo* info, WOLFSSL* ssl)
    {
        int i;

        info->ssl = ssl;
        info->cipherName[0] = 0;
        for (i = 0; i < MAX_PACKETS_HANDSHAKE; i++)
            info->packetNames[i][0] = 0;
        info->numberPackets = 0;
        info->negotiationError = 0;
    }

    /* Set Final HandShakeInfo parameters */
    void FinishHandShakeInfo(HandShakeInfo* info)
    {
        int i;
        int sz = sizeof(cipher_name_idx)/sizeof(int);

        for (i = 0; i < sz; i++)
            if (info->ssl->options.cipherSuite == (byte)cipher_name_idx[i]) {
                if (info->ssl->options.cipherSuite0 == ECC_BYTE)
                    continue;   /* ECC suites at end */
                XSTRNCPY(info->cipherName, cipher_names[i], MAX_CIPHERNAME_SZ);
                break;
            }

        /* error max and min are negative numbers */
        if (info->ssl->error <= MIN_PARAM_ERR && info->ssl->error >= MAX_PARAM_ERR)
            info->negotiationError = info->ssl->error;
    }


    /* Add name to info packet names, increase packet name count */
    void AddPacketName(const char* name, HandShakeInfo* info)
    {
        if (info->numberPackets < MAX_PACKETS_HANDSHAKE) {
            XSTRNCPY(info->packetNames[info->numberPackets++], name,
                    MAX_PACKETNAME_SZ);
        }
    }


    /* Initialisze TimeoutInfo */
    void InitTimeoutInfo(TimeoutInfo* info)
    {
        int i;

        info->timeoutName[0] = 0;
        info->flags          = 0;

        for (i = 0; i < MAX_PACKETS_HANDSHAKE; i++) {
            info->packets[i].packetName[0]     = 0;
            info->packets[i].timestamp.tv_sec  = 0;
            info->packets[i].timestamp.tv_usec = 0;
            info->packets[i].bufferValue       = 0;
            info->packets[i].valueSz           = 0;
        }
        info->numberPackets        = 0;
        info->timeoutValue.tv_sec  = 0;
        info->timeoutValue.tv_usec = 0;
    }


    /* Free TimeoutInfo */
    void FreeTimeoutInfo(TimeoutInfo* info, void* heap)
    {
        int i;
        (void)heap;
        for (i = 0; i < MAX_PACKETS_HANDSHAKE; i++)
            if (info->packets[i].bufferValue) {
                XFREE(info->packets[i].bufferValue, heap, DYNAMIC_TYPE_INFO);
                info->packets[i].bufferValue = 0;
            }

    }


    /* Add PacketInfo to TimeoutInfo */
    void AddPacketInfo(const char* name, TimeoutInfo* info, const byte* data,
                       int sz, void* heap)
    {
        if (info->numberPackets < (MAX_PACKETS_HANDSHAKE - 1)) {
            Timeval currTime;

            /* may add name after */
            if (name)
                XSTRNCPY(info->packets[info->numberPackets].packetName, name,
                        MAX_PACKETNAME_SZ);

            /* add data, put in buffer if bigger than static buffer */
            info->packets[info->numberPackets].valueSz = sz;
            if (sz < MAX_VALUE_SZ)
                XMEMCPY(info->packets[info->numberPackets].value, data, sz);
            else {
                info->packets[info->numberPackets].bufferValue =
                                    (byte*)XMALLOC(sz, heap, DYNAMIC_TYPE_INFO);
                if (!info->packets[info->numberPackets].bufferValue)
                    /* let next alloc catch, just don't fill, not fatal here  */
                    info->packets[info->numberPackets].valueSz = 0;
                else
                    XMEMCPY(info->packets[info->numberPackets].bufferValue,
                           data, sz);
            }
            gettimeofday(&currTime, 0);
            info->packets[info->numberPackets].timestamp.tv_sec  =
                                                             currTime.tv_sec;
            info->packets[info->numberPackets].timestamp.tv_usec =
                                                             currTime.tv_usec;
            info->numberPackets++;
        }
    }


    /* Add packet name to previsouly added packet info */
    void AddLateName(const char* name, TimeoutInfo* info)
    {
        /* make sure we have a valid previous one */
        if (info->numberPackets > 0 && info->numberPackets <
                                                        MAX_PACKETS_HANDSHAKE) {
            XSTRNCPY(info->packets[info->numberPackets - 1].packetName, name,
                    MAX_PACKETNAME_SZ);
        }
    }

    /* Add record header to previsouly added packet info */
    void AddLateRecordHeader(const RecordLayerHeader* rl, TimeoutInfo* info)
    {
        /* make sure we have a valid previous one */
        if (info->numberPackets > 0 && info->numberPackets <
                                                        MAX_PACKETS_HANDSHAKE) {
            if (info->packets[info->numberPackets - 1].bufferValue)
                XMEMCPY(info->packets[info->numberPackets - 1].bufferValue, rl,
                       RECORD_HEADER_SZ);
            else
                XMEMCPY(info->packets[info->numberPackets - 1].value, rl,
                       RECORD_HEADER_SZ);
        }
    }

#endif /* WOLFSSL_CALLBACKS */



/* client only parts */
#ifndef NO_WOLFSSL_CLIENT

    int SendClientHello(WOLFSSL* ssl)
    {
        byte              *output;
        word32             length, idx = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;
        int                sendSz;
        int                idSz = ssl->options.resuming
                                ? ssl->session.sessionIDSz
                                : 0;
        int                ret;
        word16             extSz = 0;


#ifdef WOLFSSL_TLS13
        if (IsAtLeastTLSv1_3(ssl->version))
            return SendTls13ClientHello(ssl);
#endif

        if (ssl->suites == NULL) {
            WOLFSSL_MSG("Bad suites pointer in SendClientHello");
            return SUITES_ERROR;
        }

#ifdef HAVE_SESSION_TICKET
        if (ssl->options.resuming && ssl->session.ticketLen > 0) {
            SessionTicket* ticket;

            ticket = TLSX_SessionTicket_Create(0, ssl->session.ticket,
                                             ssl->session.ticketLen, ssl->heap);
            if (ticket == NULL) return MEMORY_E;

            ret = TLSX_UseSessionTicket(&ssl->extensions, ticket, ssl->heap);
            if (ret != SSL_SUCCESS) return ret;

            idSz = 0;
        }
#endif
        length = VERSION_SZ + RAN_LEN
               + idSz + ENUM_LEN
               + ssl->suites->suiteSz + SUITE_LEN
               + COMP_LEN + ENUM_LEN;

#ifdef HAVE_TLS_EXTENSIONS
        /* auto populate extensions supported unless user defined */
        if ((ret = TLSX_PopulateExtensions(ssl, 0)) != 0)
            return ret;
    #ifdef HAVE_QSH
        if (QSH_Init(ssl) != 0)
            return MEMORY_E;
    #endif
        extSz = TLSX_GetRequestSize(ssl, client_hello);
        if (extSz != 0)
            length += extSz;
#else
        if (IsAtLeastTLSv1_2(ssl) && ssl->suites->hashSigAlgoSz)
            extSz += HELLO_EXT_SZ + HELLO_EXT_SIGALGO_SZ
                   + ssl->suites->hashSigAlgoSz;
#ifdef HAVE_EXTENDED_MASTER
        if (ssl->options.haveEMS)
            extSz += HELLO_EXT_SZ;
#endif
        if (extSz != 0)
            length += extSz + HELLO_EXT_SZ_SZ;
#endif
        sendSz = length + HANDSHAKE_HEADER_SZ + RECORD_HEADER_SZ;

#ifdef WOLFSSL_DTLS
        if (ssl->options.dtls) {
            length += ENUM_LEN;   /* cookie */
            if (ssl->arrays->cookieSz != 0) length += ssl->arrays->cookieSz;
            sendSz  = length + DTLS_HANDSHAKE_HEADER_SZ + DTLS_RECORD_HEADER_SZ;
            idx    += DTLS_HANDSHAKE_EXTRA + DTLS_RECORD_EXTRA;
        }
#endif

        if (IsEncryptionOn(ssl, 1))
            sendSz += MAX_MSG_EXTRA;

        /* check for available size */
        if ((ret = CheckAvailableSize(ssl, sendSz)) != 0)
            return ret;

        /* get output buffer */
        output = ssl->buffers.outputBuffer.buffer +
                 ssl->buffers.outputBuffer.length;

        AddHeaders(output, length, client_hello, ssl);

        /* client hello, first version */
        output[idx++] = ssl->version.major;
        output[idx++] = ssl->version.minor;
        ssl->chVersion = ssl->version;  /* store in case changed */

        /* then random */
        if (ssl->options.connectState == CONNECT_BEGIN) {
            ret = wc_RNG_GenerateBlock(ssl->rng, output + idx, RAN_LEN);
            if (ret != 0)
                return ret;

            /* store random */
            XMEMCPY(ssl->arrays->clientRandom, output + idx, RAN_LEN);
        } else {
#ifdef WOLFSSL_DTLS
            /* send same random on hello again */
            XMEMCPY(output + idx, ssl->arrays->clientRandom, RAN_LEN);
#endif
        }
        idx += RAN_LEN;

        /* then session id */
        output[idx++] = (byte)idSz;
        if (idSz) {
            XMEMCPY(output + idx, ssl->session.sessionID,
                                                      ssl->session.sessionIDSz);
            idx += ssl->session.sessionIDSz;
        }

        /* then DTLS cookie */
#ifdef WOLFSSL_DTLS
        if (ssl->options.dtls) {
            byte cookieSz = ssl->arrays->cookieSz;

            output[idx++] = cookieSz;
            if (cookieSz) {
                XMEMCPY(&output[idx], ssl->arrays->cookie, cookieSz);
                idx += cookieSz;
            }
        }
#endif
        /* then cipher suites */
        c16toa(ssl->suites->suiteSz, output + idx);
        idx += OPAQUE16_LEN;
        XMEMCPY(output + idx, &ssl->suites->suites, ssl->suites->suiteSz);
        idx += ssl->suites->suiteSz;

        /* last, compression */
        output[idx++] = COMP_LEN;
        if (ssl->options.usingCompression)
            output[idx++] = ZLIB_COMPRESSION;
        else
            output[idx++] = NO_COMPRESSION;

#ifdef HAVE_TLS_EXTENSIONS
        idx += TLSX_WriteRequest(ssl, output + idx, client_hello);

        (void)idx; /* suppress analyzer warning, keep idx current */
#else
        if (extSz != 0) {
            c16toa(extSz, output + idx);
            idx += HELLO_EXT_SZ_SZ;

            if (IsAtLeastTLSv1_2(ssl)) {
                if (ssl->suites->hashSigAlgoSz) {
                    int i;
                    /* extension type */
                    c16toa(HELLO_EXT_SIG_ALGO, output + idx);
                    idx += HELLO_EXT_TYPE_SZ;
                    /* extension data length */
                    c16toa(HELLO_EXT_SIGALGO_SZ + ssl->suites->hashSigAlgoSz,
                           output + idx);
                    idx += HELLO_EXT_SZ_SZ;
                    /* sig algos length */
                    c16toa(ssl->suites->hashSigAlgoSz, output + idx);
                    idx += HELLO_EXT_SIGALGO_SZ;
                    for (i = 0; i < ssl->suites->hashSigAlgoSz; i++, idx++) {
                        output[idx] = ssl->suites->hashSigAlgo[i];
                    }
                }
            }
#ifdef HAVE_EXTENDED_MASTER
            if (ssl->options.haveEMS) {
                c16toa(HELLO_EXT_EXTMS, output + idx);
                idx += HELLO_EXT_TYPE_SZ;
                c16toa(0, output + idx);
                idx += HELLO_EXT_SZ_SZ;
            }
#endif
        }
#endif

        if (IsEncryptionOn(ssl, 1)) {
            byte* input;
            int   inputSz = idx - RECORD_HEADER_SZ; /* build msg adds rec hdr */

            input = (byte*)XMALLOC(inputSz, ssl->heap, DYNAMIC_TYPE_IN_BUFFER);
            if (input == NULL)
                return MEMORY_E;

            XMEMCPY(input, output + RECORD_HEADER_SZ, inputSz);
            sendSz = BuildMessage(ssl, output, sendSz, input, inputSz,
                                  handshake, 1, 0, 0);
            XFREE(input, ssl->heap, DYNAMIC_TYPE_IN_BUFFER);

            if (sendSz < 0)
                return sendSz;
        } else {
            #ifdef WOLFSSL_DTLS
                if (ssl->options.dtls)
                    DtlsSEQIncrement(ssl, CUR_ORDER);
            #endif
            ret = HashOutput(ssl, output, sendSz, 0);
            if (ret != 0)
                return ret;
        }

        #ifdef WOLFSSL_DTLS
            if (IsDtlsNotSctpMode(ssl)) {
                if ((ret = DtlsMsgPoolSave(ssl, output, sendSz)) != 0)
                    return ret;
            }
        #endif

        ssl->options.clientState = CLIENT_HELLO_COMPLETE;

#ifdef WOLFSSL_CALLBACKS
        if (ssl->hsInfoOn) AddPacketName("ClientHello", &ssl->handShakeInfo);
        if (ssl->toInfoOn)
            AddPacketInfo("ClientHello", &ssl->timeoutInfo, output, sendSz,
                          ssl->heap);
#endif

        ssl->buffers.outputBuffer.length += sendSz;

        return SendBuffered(ssl);
    }


    static int DoHelloVerifyRequest(WOLFSSL* ssl, const byte* input,
                                    word32* inOutIdx, word32 size)
    {
        ProtocolVersion pv;
        byte            cookieSz;
        word32          begin = *inOutIdx;

#ifdef WOLFSSL_CALLBACKS
        if (ssl->hsInfoOn) AddPacketName("HelloVerifyRequest",
                                         &ssl->handShakeInfo);
        if (ssl->toInfoOn) AddLateName("HelloVerifyRequest", &ssl->timeoutInfo);
#endif

#ifdef WOLFSSL_DTLS
        if (ssl->options.dtls) {
            DtlsMsgPoolReset(ssl);
        }
#endif

        if ((*inOutIdx - begin) + OPAQUE16_LEN + OPAQUE8_LEN > size)
            return BUFFER_ERROR;

        XMEMCPY(&pv, input + *inOutIdx, OPAQUE16_LEN);
        *inOutIdx += OPAQUE16_LEN;

        if (pv.major != DTLS_MAJOR ||
                         (pv.minor != DTLS_MINOR && pv.minor != DTLSv1_2_MINOR))
            return VERSION_ERROR;

        cookieSz = input[(*inOutIdx)++];

        if (cookieSz) {
            if ((*inOutIdx - begin) + cookieSz > size)
                return BUFFER_ERROR;

#ifdef WOLFSSL_DTLS
            if (cookieSz <= MAX_COOKIE_LEN) {
                XMEMCPY(ssl->arrays->cookie, input + *inOutIdx, cookieSz);
                ssl->arrays->cookieSz = cookieSz;
            }
#endif
            *inOutIdx += cookieSz;
        }

        ssl->options.serverState = SERVER_HELLOVERIFYREQUEST_COMPLETE;
        return 0;
    }


    static INLINE int DSH_CheckSessionId(WOLFSSL* ssl)
    {
        int ret = 0;

#ifdef HAVE_SECRET_CALLBACK
        /* If a session secret callback exists, we are using that
         * key instead of the saved session key. */
        ret = ret || (ssl->sessionSecretCb != NULL);
#endif

#ifdef HAVE_SESSION_TICKET
        /* server may send blank ticket which may not be expected to indicate
         * existing one ok but will also be sending a new one */
        ret = ret || (ssl->session.ticketLen > 0);
#endif

        ret = ret ||
              (ssl->options.haveSessionId && XMEMCMP(ssl->arrays->sessionID,
                                          ssl->session.sessionID, ID_LEN) == 0);

        return ret;
    }

    /* Check the version in the received message is valid and set protocol
     * version to use.
     *
     * ssl  The SSL/TLS object.
     * pv   The protocol version from the packet.
     * returns 0 on success, otherwise failure.
     */
    int CheckVersion(WOLFSSL *ssl, ProtocolVersion pv)
    {
#ifdef WOLFSSL_TLS13
        /* TODO: [TLS13] Remove this.
         * Translate the draft TLS v1.3 version to final version.
         */
        if (pv.major == TLS_DRAFT_MAJOR) {
            pv.major = SSLv3_MAJOR;
            pv.minor = TLSv1_3_MINOR;
        }
#endif

        /* Check for upgrade attack. */
        if (pv.minor > ssl->version.minor) {
            WOLFSSL_MSG("Server using higher version, fatal error");
            return VERSION_ERROR;
        }
        if (pv.minor < ssl->version.minor) {
            WOLFSSL_MSG("server using lower version");

            /* Check for downgrade attack. */
            if (!ssl->options.downgrade) {
                WOLFSSL_MSG("\tno downgrade allowed, fatal error");
                return VERSION_ERROR;
            }
            if (pv.minor < ssl->options.minDowngrade) {
                WOLFSSL_MSG("\tversion below minimum allowed, fatal error");
                return VERSION_ERROR;
            }

            #ifdef HAVE_SECURE_RENEGOTIATION
                if (ssl->secure_renegotiation &&
                                         ssl->secure_renegotiation->enabled &&
                                         ssl->options.handShakeDone) {
                    WOLFSSL_MSG("Server changed version during scr");
                    return VERSION_ERROR;
                }
            #endif

            /* Checks made - OK to downgrade. */
            if (pv.minor == SSLv3_MINOR) {
                /* turn off tls */
                WOLFSSL_MSG("\tdowngrading to SSLv3");
                ssl->options.tls    = 0;
                ssl->options.tls1_1 = 0;
                ssl->version.minor  = SSLv3_MINOR;
            }
            else if (pv.minor == TLSv1_MINOR) {
                /* turn off tls 1.1+ */
                WOLFSSL_MSG("\tdowngrading to TLSv1");
                ssl->options.tls1_1 = 0;
                ssl->version.minor  = TLSv1_MINOR;
            }
            else if (pv.minor == TLSv1_1_MINOR) {
                WOLFSSL_MSG("\tdowngrading to TLSv1.1");
                ssl->version.minor  = TLSv1_1_MINOR;
            }
            else if (pv.minor == TLSv1_2_MINOR) {
                WOLFSSL_MSG("    downgrading to TLSv1.2");
                ssl->version.minor  = TLSv1_2_MINOR;
            }
        }

#ifdef OPENSSL_EXTRA
        /* check if option is set to not allow the current version
         * set from either wolfSSL_set_options or wolfSSL_CTX_set_options */
        if (!ssl->options.dtls && ssl->options.downgrade &&
                ssl->options.mask > 0) {
            if (ssl->version.minor == TLSv1_2_MINOR &&
             (ssl->options.mask & SSL_OP_NO_TLSv1_2) == SSL_OP_NO_TLSv1_2) {
                WOLFSSL_MSG("\tOption set to not allow TLSv1.2, Downgrading");
                ssl->version.minor = TLSv1_1_MINOR;
            }
            if (ssl->version.minor == TLSv1_1_MINOR &&
             (ssl->options.mask & SSL_OP_NO_TLSv1_1) == SSL_OP_NO_TLSv1_1) {
                WOLFSSL_MSG("\tOption set to not allow TLSv1.1, Downgrading");
                ssl->options.tls1_1 = 0;
                ssl->version.minor = TLSv1_MINOR;
            }
            if (ssl->version.minor == TLSv1_MINOR &&
                (ssl->options.mask & SSL_OP_NO_TLSv1) == SSL_OP_NO_TLSv1) {
                WOLFSSL_MSG("\tOption set to not allow TLSv1, Downgrading");
                ssl->options.tls    = 0;
                ssl->options.tls1_1 = 0;
                ssl->version.minor = SSLv3_MINOR;
            }
            if (ssl->version.minor == SSLv3_MINOR &&
                (ssl->options.mask & SSL_OP_NO_SSLv3) == SSL_OP_NO_SSLv3) {
                WOLFSSL_MSG("\tError, option set to not allow SSLv3");
                return VERSION_ERROR;
            }

            if (ssl->version.minor < ssl->options.minDowngrade) {
                WOLFSSL_MSG("\tversion below minimum allowed, fatal error");
                return VERSION_ERROR;
            }
        }
#endif

        return 0;
    }

    int DoServerHello(WOLFSSL* ssl, const byte* input, word32* inOutIdx,
                      word32 helloSz)
    {
        byte            cs0;   /* cipher suite bytes 0, 1 */
        byte            cs1;
        ProtocolVersion pv;
        byte            compression;
        word32          i = *inOutIdx;
        word32          begin = i;
        int             ret;

#ifdef WOLFSSL_CALLBACKS
        if (ssl->hsInfoOn) AddPacketName("ServerHello", &ssl->handShakeInfo);
        if (ssl->toInfoOn) AddLateName("ServerHello", &ssl->timeoutInfo);
#endif

        /* protocol version, random and session id length check */
        if (OPAQUE16_LEN + RAN_LEN + OPAQUE8_LEN > helloSz)
            return BUFFER_ERROR;

        /* protocol version */
        XMEMCPY(&pv, input + i, OPAQUE16_LEN);
        i += OPAQUE16_LEN;

        ret = CheckVersion(ssl, pv);
        if (ret != 0)
            return ret;

#ifdef WOLFSSL_TLS13
        if (IsAtLeastTLSv1_3(pv))
            return DoTls13ServerHello(ssl, input, inOutIdx, helloSz);
#endif

        /* random */
        XMEMCPY(ssl->arrays->serverRandom, input + i, RAN_LEN);
        i += RAN_LEN;

#ifdef WOLFSSL_TLS13
        if (IsAtLeastTLSv1_3(ssl->ctx->method->version)) {
            /* TLS v1.3 capable client not allowed to downgrade when connecting
             * to TLS v1.3 capable server.
             */
            if (XMEMCMP(input + i - (TLS13_DOWNGRADE_SZ + 1),
                             tls13Downgrade, TLS13_DOWNGRADE_SZ) == 0 &&
                             (*(input + i - 1) == 0 || *(input + i - 1) == 1)) {
                SendAlert(ssl, alert_fatal, illegal_parameter);
                return VERSION_ERROR;
            }
        }
        else
#endif
        if (ssl->ctx->method->version.major == SSLv3_MAJOR &&
                             ssl->ctx->method->version.minor == TLSv1_2_MINOR) {
            /* TLS v1.2 capable client not allowed to downgrade when connecting
             * to TLS v1.2 capable server.
             */
            if (XMEMCMP(input + i - (TLS13_DOWNGRADE_SZ + 1),
                        tls13Downgrade, TLS13_DOWNGRADE_SZ) == 0 &&
                                                        *(input + i - 1) == 0) {
                SendAlert(ssl, alert_fatal, illegal_parameter);
                return VERSION_ERROR;
            }
        }

        /* session id */
        ssl->arrays->sessionIDSz = input[i++];

        if (ssl->arrays->sessionIDSz > ID_LEN) {
            WOLFSSL_MSG("Invalid session ID size");
            ssl->arrays->sessionIDSz = 0;
            return BUFFER_ERROR;
        }
        else if (ssl->arrays->sessionIDSz) {
            if ((i - begin) + ssl->arrays->sessionIDSz > helloSz)
                return BUFFER_ERROR;

            XMEMCPY(ssl->arrays->sessionID, input + i,
                                                      ssl->arrays->sessionIDSz);
            i += ssl->arrays->sessionIDSz;
            ssl->options.haveSessionId = 1;
        }


        /* suite and compression */
        if ((i - begin) + OPAQUE16_LEN + OPAQUE8_LEN > helloSz)
            return BUFFER_ERROR;

        cs0 = input[i++];
        cs1 = input[i++];

#ifdef HAVE_SECURE_RENEGOTIATION
        if (ssl->secure_renegotiation && ssl->secure_renegotiation->enabled &&
                                         ssl->options.handShakeDone) {
            if (ssl->options.cipherSuite0 != cs0 ||
                ssl->options.cipherSuite  != cs1) {
                WOLFSSL_MSG("Server changed cipher suite during scr");
                return MATCH_SUITE_ERROR;
            }
        }
#endif

        ssl->options.cipherSuite0 = cs0;
        ssl->options.cipherSuite  = cs1;
        compression = input[i++];

        if (compression != NO_COMPRESSION && !ssl->options.usingCompression) {
            WOLFSSL_MSG("Server forcing compression w/o support");
            return COMPRESSION_ERROR;
        }

        if (compression != ZLIB_COMPRESSION && ssl->options.usingCompression) {
            WOLFSSL_MSG("Server refused compression, turning off");
            ssl->options.usingCompression = 0;  /* turn off if server refused */
        }

        *inOutIdx = i;

#ifdef HAVE_TLS_EXTENSIONS
        if ( (i - begin) < helloSz) {
            if (TLSX_SupportExtensions(ssl)) {
                word16 totalExtSz;

                if ((i - begin) + OPAQUE16_LEN > helloSz)
                    return BUFFER_ERROR;

                ato16(&input[i], &totalExtSz);
                i += OPAQUE16_LEN;

                if ((i - begin) + totalExtSz > helloSz)
                    return BUFFER_ERROR;

                if ((ret = TLSX_Parse(ssl, (byte *) input + i,
                                                          totalExtSz, 0, NULL)))
                    return ret;

                i += totalExtSz;
                *inOutIdx = i;
            }
            else
                *inOutIdx = begin + helloSz; /* skip extensions */
        }
        else
            ssl->options.haveEMS = 0; /* If no extensions, no EMS */
#else
        {
            int allowExt = 0;
            byte pendingEMS = 0;

            if ( (i - begin) < helloSz) {
                if (ssl->version.major == SSLv3_MAJOR &&
                    ssl->version.minor >= TLSv1_MINOR) {

                    allowExt = 1;
                }
#ifdef WOLFSSL_DTLS
                if (ssl->version.major == DTLS_MAJOR)
                    allowExt = 1;
#endif

                if (allowExt) {
                    word16 totalExtSz;

                    if ((i - begin) + OPAQUE16_LEN > helloSz)
                        return BUFFER_ERROR;

                    ato16(&input[i], &totalExtSz);
                    i += OPAQUE16_LEN;

                    if ((i - begin) + totalExtSz > helloSz)
                        return BUFFER_ERROR;

                    while (totalExtSz) {
                        word16 extId, extSz;

                        if (OPAQUE16_LEN + OPAQUE16_LEN > totalExtSz)
                            return BUFFER_ERROR;

                        ato16(&input[i], &extId);
                        i += OPAQUE16_LEN;
                        ato16(&input[i], &extSz);
                        i += OPAQUE16_LEN;

                        if (OPAQUE16_LEN + OPAQUE16_LEN + extSz > totalExtSz)
                            return BUFFER_ERROR;

                        if (extId == HELLO_EXT_EXTMS)
                            pendingEMS = 1;
                        else
                            i += extSz;

                        totalExtSz -= OPAQUE16_LEN + OPAQUE16_LEN + extSz;
                    }

                    *inOutIdx = i;
                }
                else
                    *inOutIdx = begin + helloSz; /* skip extensions */
            }

            if (!pendingEMS && ssl->options.haveEMS)
                ssl->options.haveEMS = 0;
        }
#endif

        ssl->options.serverState = SERVER_HELLO_COMPLETE;

        if (IsEncryptionOn(ssl, 0)) {
            *inOutIdx += ssl->keys.padSz;
        }

#ifdef HAVE_SECRET_CALLBACK
        if (ssl->sessionSecretCb != NULL) {
            int secretSz = SECRET_LEN;
            ret = ssl->sessionSecretCb(ssl, ssl->session.masterSecret,
                                              &secretSz, ssl->sessionSecretCtx);
            if (ret != 0 || secretSz != SECRET_LEN)
                return SESSION_SECRET_CB_E;
        }
#endif /* HAVE_SECRET_CALLBACK */

        if (ssl->options.resuming) {
            if (DSH_CheckSessionId(ssl)) {
                if (SetCipherSpecs(ssl) == 0) {

                    XMEMCPY(ssl->arrays->masterSecret,
                            ssl->session.masterSecret, SECRET_LEN);
            #ifdef NO_OLD_TLS
                    ret = DeriveTlsKeys(ssl);
            #else
                    ret = -1; /* default value */
                #ifndef NO_TLS
                    if (ssl->options.tls)
                        ret = DeriveTlsKeys(ssl);
                #endif
                    if (!ssl->options.tls)
                        ret = DeriveKeys(ssl);
            #endif /* NO_OLD_TLS */
                    ssl->options.serverState = SERVER_HELLODONE_COMPLETE;

                    return ret;
                }
                else {
                    WOLFSSL_MSG("Unsupported cipher suite, DoServerHello");
                    return UNSUPPORTED_SUITE;
                }
            }
            else {
                WOLFSSL_MSG("Server denied resumption attempt");
                ssl->options.resuming = 0; /* server denied resumption try */
            }
        }
        #ifdef WOLFSSL_DTLS
            if (ssl->options.dtls) {
                DtlsMsgPoolReset(ssl);
            }
        #endif

        return SetCipherSpecs(ssl);
    }


    /* Make sure client setup is valid for this suite, true on success */
    int VerifyClientSuite(WOLFSSL* ssl)
    {
        int  havePSK = 0;
        byte first   = ssl->options.cipherSuite0;
        byte second  = ssl->options.cipherSuite;

        WOLFSSL_ENTER("VerifyClientSuite");

        #ifndef NO_PSK
            havePSK = ssl->options.havePSK;
        #endif

        if (CipherRequires(first, second, REQUIRES_PSK)) {
            WOLFSSL_MSG("Requires PSK");
            if (havePSK == 0) {
                WOLFSSL_MSG("Don't have PSK");
                return 0;
            }
        }

        return 1;  /* success */
    }


#ifndef NO_CERTS
    /* just read in and ignore for now TODO: */
    static int DoCertificateRequest(WOLFSSL* ssl, const byte* input, word32*
                                    inOutIdx, word32 size)
    {
        word16 len;
        word32 begin = *inOutIdx;

        #ifdef WOLFSSL_CALLBACKS
            if (ssl->hsInfoOn)
                AddPacketName("CertificateRequest", &ssl->handShakeInfo);
            if (ssl->toInfoOn)
                AddLateName("CertificateRequest", &ssl->timeoutInfo);
        #endif

        if ((*inOutIdx - begin) + OPAQUE8_LEN > size)
            return BUFFER_ERROR;

        len = input[(*inOutIdx)++];

        if ((*inOutIdx - begin) + len > size)
            return BUFFER_ERROR;

        /* types, read in here */
        *inOutIdx += len;

        /* signature and hash signature algorithm */
        if (IsAtLeastTLSv1_2(ssl)) {
            if ((*inOutIdx - begin) + OPAQUE16_LEN > size)
                return BUFFER_ERROR;

            ato16(input + *inOutIdx, &len);
            *inOutIdx += OPAQUE16_LEN;

            if ((*inOutIdx - begin) + len > size)
                return BUFFER_ERROR;

            PickHashSigAlgo(ssl, input + *inOutIdx, len);
            *inOutIdx += len;
    #ifdef WC_RSA_PSS
            ssl->pssAlgo = 0;
            if (ssl->suites->sigAlgo == rsa_pss_sa_algo)
                ssl->pssAlgo |= 1 << ssl->suites->hashAlgo;
    #endif
        }

        /* authorities */
        if ((*inOutIdx - begin) + OPAQUE16_LEN > size)
            return BUFFER_ERROR;

        ato16(input + *inOutIdx, &len);
        *inOutIdx += OPAQUE16_LEN;

        if ((*inOutIdx - begin) + len > size)
            return BUFFER_ERROR;

        while (len) {
            word16 dnSz;

            if ((*inOutIdx - begin) + OPAQUE16_LEN > size)
                return BUFFER_ERROR;

            ato16(input + *inOutIdx, &dnSz);
            *inOutIdx += OPAQUE16_LEN;

            if ((*inOutIdx - begin) + dnSz > size)
                return BUFFER_ERROR;

            *inOutIdx += dnSz;
            len -= OPAQUE16_LEN + dnSz;
        }

        /* don't send client cert or cert verify if user hasn't provided
           cert and private key */
        if (ssl->buffers.certificate && ssl->buffers.certificate->buffer &&
            ssl->buffers.key && ssl->buffers.key->buffer)
            ssl->options.sendVerify = SEND_CERT;
        else if (IsTLS(ssl))
            ssl->options.sendVerify = SEND_BLANK_CERT;

        if (IsEncryptionOn(ssl, 0))
            *inOutIdx += ssl->keys.padSz;

        return 0;
    }
#endif /* !NO_CERTS */


#ifdef HAVE_ECC

    static int CheckCurveId(int tlsCurveId)
    {
        int ret = ECC_CURVE_ERROR;

        switch (tlsCurveId) {
    #if defined(HAVE_ECC160) || defined(HAVE_ALL_CURVES)
        #ifndef NO_ECC_SECP
            case WOLFSSL_ECC_SECP160R1: return ECC_SECP160R1_OID;
        #endif /* !NO_ECC_SECP */
        #ifdef HAVE_ECC_SECPR2
            case WOLFSSL_ECC_SECP160R2: return ECC_SECP160R2_OID;
        #endif /* HAVE_ECC_SECPR2 */
        #ifdef HAVE_ECC_KOBLITZ
            case WOLFSSL_ECC_SECP160K1: return ECC_SECP160K1_OID;
        #endif /* HAVE_ECC_KOBLITZ */
    #endif
    #if defined(HAVE_ECC192) || defined(HAVE_ALL_CURVES)
        #ifndef NO_ECC_SECP
            case WOLFSSL_ECC_SECP192R1: return ECC_SECP192R1_OID;
        #endif /* !NO_ECC_SECP */
        #ifdef HAVE_ECC_KOBLITZ
            case WOLFSSL_ECC_SECP192K1: return ECC_SECP192K1_OID;
        #endif /* HAVE_ECC_KOBLITZ */
    #endif
    #if defined(HAVE_ECC224) || defined(HAVE_ALL_CURVES)
        #ifndef NO_ECC_SECP
            case WOLFSSL_ECC_SECP224R1: return ECC_SECP224R1_OID;
        #endif /* !NO_ECC_SECP */
        #ifdef HAVE_ECC_KOBLITZ
            case WOLFSSL_ECC_SECP224K1: return ECC_SECP224K1_OID;
        #endif /* HAVE_ECC_KOBLITZ */
    #endif
    #if !defined(NO_ECC256)  || defined(HAVE_ALL_CURVES)
        #ifdef HAVE_CURVE25519
            case WOLFSSL_ECC_X25519: return ECC_X25519_OID;
        #endif
        #ifndef NO_ECC_SECP
            case WOLFSSL_ECC_SECP256R1: return ECC_SECP256R1_OID;
        #endif /* !NO_ECC_SECP */
        #ifdef HAVE_ECC_KOBLITZ
            case WOLFSSL_ECC_SECP256K1: return ECC_SECP256K1_OID;
        #endif /* HAVE_ECC_KOBLITZ */
        #ifdef HAVE_ECC_BRAINPOOL
            case WOLFSSL_ECC_BRAINPOOLP256R1: return ECC_BRAINPOOLP256R1_OID;
        #endif /* HAVE_ECC_BRAINPOOL */
    #endif
    #if defined(HAVE_ECC384) || defined(HAVE_ALL_CURVES)
        #ifndef NO_ECC_SECP
            case WOLFSSL_ECC_SECP384R1: return ECC_SECP384R1_OID;
        #endif /* !NO_ECC_SECP */
        #ifdef HAVE_ECC_BRAINPOOL
            case WOLFSSL_ECC_BRAINPOOLP384R1: return ECC_BRAINPOOLP384R1_OID;
        #endif /* HAVE_ECC_BRAINPOOL */
    #endif
    #if defined(HAVE_ECC512) || defined(HAVE_ALL_CURVES)
        #ifdef HAVE_ECC_BRAINPOOL
            case WOLFSSL_ECC_BRAINPOOLP512R1: return ECC_BRAINPOOLP512R1_OID;
        #endif /* HAVE_ECC_BRAINPOOL */
    #endif
    #if defined(HAVE_ECC521) || defined(HAVE_ALL_CURVES)
        #ifndef NO_ECC_SECP
            case WOLFSSL_ECC_SECP521R1: return ECC_SECP521R1_OID;
        #endif /* !NO_ECC_SECP */
    #endif
        }

        return ret;
    }

#endif /* HAVE_ECC */


/* Persistable DoServerKeyExchange arguments */
typedef struct DskeArgs {
    byte*  output; /* not allocated */
#if !defined(NO_DH) || defined(HAVE_ECC)
    byte*  verifySig;
#endif
    word32 idx;
    word32 begin;
#if !defined(NO_DH) || defined(HAVE_ECC)
    word16 verifySigSz;
#endif
    word16 sigSz;
    byte   sigAlgo;
    byte   hashAlgo;
} DskeArgs;

static void FreeDskeArgs(WOLFSSL* ssl, void* pArgs)
{
    DskeArgs* args = (DskeArgs*)pArgs;

    (void)ssl;
    (void)args;

#if !defined(NO_DH) || defined(HAVE_ECC)
    if (args->verifySig) {
        XFREE(args->verifySig, ssl->heap, DYNAMIC_TYPE_SIGNATURE);
        args->verifySig = NULL;
    }
#endif
}

static int DoServerKeyExchange(WOLFSSL* ssl, const byte* input,
                               word32* inOutIdx, word32 size)
{
    int ret = 0;
#ifdef WOLFSSL_ASYNC_CRYPT
    DskeArgs* args = (DskeArgs*)ssl->async.args;
    typedef char args_test[sizeof(ssl->async.args) >= sizeof(*args) ? 1 : -1];
    (void)sizeof(args_test);
#else
    DskeArgs  args[1];
#endif

    WOLFSSL_ENTER("DoServerKeyExchange");

#ifdef WOLFSSL_ASYNC_CRYPT
    ret = wolfSSL_AsyncPop(ssl, &ssl->options.asyncState);
    if (ret != WC_NOT_PENDING_E) {
        /* Check for error */
        if (ret < 0)
            goto exit_dske;
    }
    else
#endif
    {
        /* Reset state */
        ret = 0;
        ssl->options.asyncState = TLS_ASYNC_BEGIN;
        XMEMSET(args, 0, sizeof(DskeArgs));
        args->idx = *inOutIdx;
        args->begin = *inOutIdx;
        args->sigAlgo = ssl->specs.sig_algo;
        args->hashAlgo = sha_mac;
    #ifdef WOLFSSL_ASYNC_CRYPT
        ssl->async.freeArgs = FreeDskeArgs;
    #endif
    }

    switch(ssl->options.asyncState)
    {
        case TLS_ASYNC_BEGIN:
        {
        #ifdef WOLFSSL_CALLBACKS
            if (ssl->hsInfoOn)
                AddPacketName("ServerKeyExchange", &ssl->handShakeInfo);
            if (ssl->toInfoOn)
                AddLateName("ServerKeyExchange", &ssl->timeoutInfo);
        #endif

            switch(ssl->specs.kea)
            {
            #ifndef NO_PSK
                case psk_kea:
                {
                    int srvHintLen;
                    word16 length;

                    if ((args->idx - args->begin) + OPAQUE16_LEN > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    ato16(input + args->idx, &length);
                    args->idx += OPAQUE16_LEN;

                    if ((args->idx - args->begin) + length > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    /* get PSK server hint from the wire */
                    srvHintLen = min(length, MAX_PSK_ID_LEN);
                    XMEMCPY(ssl->arrays->server_hint, input + args->idx,
                                                                    srvHintLen);
                    ssl->arrays->server_hint[srvHintLen] = '\0'; /* null term */
                    args->idx += length;
                    break;
                }
            #endif /* !NO_PSK */
            #ifndef NO_DH
                case diffie_hellman_kea:
                {
                    word16 length;

                    /* p */
                    if ((args->idx - args->begin) + OPAQUE16_LEN > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    ato16(input + args->idx, &length);
                    args->idx += OPAQUE16_LEN;

                    if ((args->idx - args->begin) + length > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    if (length < ssl->options.minDhKeySz) {
                        WOLFSSL_MSG("Server using a DH key that is too small");
                        SendAlert(ssl, alert_fatal, handshake_failure);
                        ERROR_OUT(DH_KEY_SIZE_E, exit_dske);
                    }

                    ssl->buffers.serverDH_P.buffer =
                        (byte*)XMALLOC(length, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                    if (ssl->buffers.serverDH_P.buffer) {
                        ssl->buffers.serverDH_P.length = length;
                    }
                    else {
                        ERROR_OUT(MEMORY_ERROR, exit_dske);
                    }

                    XMEMCPY(ssl->buffers.serverDH_P.buffer, input + args->idx,
                                                                        length);
                    args->idx += length;

                    ssl->options.dhKeySz = length;

                    /* g */
                    if ((args->idx - args->begin) + OPAQUE16_LEN > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    ato16(input + args->idx, &length);
                    args->idx += OPAQUE16_LEN;

                    if ((args->idx - args->begin) + length > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    ssl->buffers.serverDH_G.buffer =
                        (byte*)XMALLOC(length, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                    if (ssl->buffers.serverDH_G.buffer) {
                        ssl->buffers.serverDH_G.length = length;
                    }
                    else {
                        ERROR_OUT(MEMORY_ERROR, exit_dske);
                    }

                    XMEMCPY(ssl->buffers.serverDH_G.buffer, input + args->idx,
                                                                        length);
                    args->idx += length;

                    /* pub */
                    if ((args->idx - args->begin) + OPAQUE16_LEN > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    ato16(input + args->idx, &length);
                    args->idx += OPAQUE16_LEN;

                    if ((args->idx - args->begin) + length > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    ssl->buffers.serverDH_Pub.buffer =
                        (byte*)XMALLOC(length, ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                    if (ssl->buffers.serverDH_Pub.buffer) {
                        ssl->buffers.serverDH_Pub.length = length;
                    }
                    else {
                        ERROR_OUT(MEMORY_ERROR, exit_dske);
                    }

                    XMEMCPY(ssl->buffers.serverDH_Pub.buffer, input + args->idx,
                                                                        length);
                    args->idx += length;
                    break;
                }
            #endif /* !NO_DH */
            #ifdef HAVE_ECC
                case ecc_diffie_hellman_kea:
                {
                    byte b;
                    int curveId, curveOid;
                    word16 length;

                    if ((args->idx - args->begin) + ENUM_LEN + OPAQUE16_LEN +
                                                        OPAQUE8_LEN > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    b = input[args->idx++];
                    if (b != named_curve) {
                        ERROR_OUT(ECC_CURVETYPE_ERROR, exit_dske);
                    }

                    args->idx += 1;   /* curve type, eat leading 0 */
                    b = input[args->idx++];
                    if ((curveOid = CheckCurveId(b)) < 0) {
                        ERROR_OUT(ECC_CURVE_ERROR, exit_dske);
                    }
                    ssl->ecdhCurveOID = curveOid;

                    length = input[args->idx++];
                    if ((args->idx - args->begin) + length > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

             #ifdef HAVE_CURVE25519
                    if (ssl->ecdhCurveOID == ECC_X25519_OID) {
                        if (ssl->peerX25519Key == NULL) {
                            ret = AllocKey(ssl, DYNAMIC_TYPE_CURVE25519,
                                           (void**)&ssl->peerX25519Key);
                            if (ret != 0) {
                                goto exit_dske;
                            }
                        } else if (ssl->peerEccKeyPresent) {
                            ret = ReuseKey(ssl, DYNAMIC_TYPE_CURVE25519,
                                           ssl->peerX25519Key);
                            ssl->peerX25519KeyPresent = 0;
                            if (ret != 0) {
                                goto exit_dske;
                            }
                        }

                        if (wc_curve25519_import_public_ex(input + args->idx,
                                length, ssl->peerX25519Key,
                                EC25519_LITTLE_ENDIAN) != 0) {
                            ERROR_OUT(ECC_PEERKEY_ERROR, exit_dske);
                        }

                        args->idx += length;
                        ssl->peerX25519KeyPresent = 1;
                        break;
                    }
             #endif
                    if (ssl->peerEccKey == NULL) {
                        ret = AllocKey(ssl, DYNAMIC_TYPE_ECC,
                                       (void**)&ssl->peerEccKey);
                        if (ret != 0) {
                            goto exit_dske;
                        }
                    } else if (ssl->peerEccKeyPresent) {
                        ret = ReuseKey(ssl, DYNAMIC_TYPE_ECC, ssl->peerEccKey);
                        ssl->peerEccKeyPresent = 0;
                        if (ret != 0) {
                            goto exit_dske;
                        }
                    }

                    curveId = wc_ecc_get_oid(curveOid, NULL, NULL);
                    if (wc_ecc_import_x963_ex(input + args->idx, length,
                                        ssl->peerEccKey, curveId) != 0) {
                        ERROR_OUT(ECC_PEERKEY_ERROR, exit_dske);
                    }

                    args->idx += length;
                    ssl->peerEccKeyPresent = 1;
                    break;
                }
            #endif /* HAVE_ECC */
            #if !defined(NO_DH) && !defined(NO_PSK)
                case dhe_psk_kea:
                {
                    int srvHintLen;
                    word16 length;

                    if ((args->idx - args->begin) + OPAQUE16_LEN > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    ato16(input + args->idx, &length);
                    args->idx += OPAQUE16_LEN;

                    if ((args->idx - args->begin) + length > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    /* get PSK server hint from the wire */
                    srvHintLen = min(length, MAX_PSK_ID_LEN);
                    XMEMCPY(ssl->arrays->server_hint, input + args->idx,
                                                                srvHintLen);
                    ssl->arrays->server_hint[srvHintLen] = '\0'; /* null term */
                    args->idx += length;

                    /* p */
                    if ((args->idx - args->begin) + OPAQUE16_LEN > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    ato16(input + args->idx, &length);
                    args->idx += OPAQUE16_LEN;

                    if ((args->idx - args->begin) + length > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    if (length < ssl->options.minDhKeySz) {
                        WOLFSSL_MSG("Server using a DH key that is too small");
                        SendAlert(ssl, alert_fatal, handshake_failure);
                        ERROR_OUT(DH_KEY_SIZE_E, exit_dske);
                    }

                    ssl->buffers.serverDH_P.buffer = (byte*)XMALLOC(length,
                                                ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                    if (ssl->buffers.serverDH_P.buffer) {
                        ssl->buffers.serverDH_P.length = length;
                    }
                    else {
                        ERROR_OUT(MEMORY_ERROR, exit_dske);
                    }

                    XMEMCPY(ssl->buffers.serverDH_P.buffer, input + args->idx,
                                                                        length);
                    args->idx += length;

                    ssl->options.dhKeySz = length;

                    /* g */
                    if ((args->idx - args->begin) + OPAQUE16_LEN > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    ato16(input + args->idx, &length);
                    args->idx += OPAQUE16_LEN;

                    if ((args->idx - args->begin) + length > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    ssl->buffers.serverDH_G.buffer = (byte*)XMALLOC(length,
                                                ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                    if (ssl->buffers.serverDH_G.buffer) {
                        ssl->buffers.serverDH_G.length = length;
                    }
                    else {
                        ERROR_OUT(MEMORY_ERROR, exit_dske);
                    }

                    XMEMCPY(ssl->buffers.serverDH_G.buffer, input + args->idx,
                                                                        length);
                    args->idx += length;

                    /* pub */
                    if ((args->idx - args->begin) + OPAQUE16_LEN > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    ato16(input + args->idx, &length);
                    args->idx += OPAQUE16_LEN;

                    if ((args->idx - args->begin) + length > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    ssl->buffers.serverDH_Pub.buffer = (byte*)XMALLOC(length,
                                                ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                    if (ssl->buffers.serverDH_Pub.buffer) {
                        ssl->buffers.serverDH_Pub.length = length;
                    }
                    else {
                        ERROR_OUT(MEMORY_ERROR, exit_dske);
                    }

                    XMEMCPY(ssl->buffers.serverDH_Pub.buffer, input + args->idx,
                                                                        length);
                    args->idx += length;
                    break;
                }
            #endif /* !NO_DH || !NO_PSK */
            #if defined(HAVE_ECC) && !defined(NO_PSK)
                case ecdhe_psk_kea:
                {
                    byte b;
                    int curveOid, curveId;
                    int srvHintLen;
                    word16 length;

                    if ((args->idx - args->begin) + OPAQUE16_LEN > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    ato16(input + args->idx, &length);
                    args->idx += OPAQUE16_LEN;

                    if ((args->idx - args->begin) + length > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    /* get PSK server hint from the wire */
                    srvHintLen = min(length, MAX_PSK_ID_LEN);
                    XMEMCPY(ssl->arrays->server_hint, input + args->idx,
                                                                    srvHintLen);
                    ssl->arrays->server_hint[srvHintLen] = '\0'; /* null term */

                    args->idx += length;

                    if ((args->idx - args->begin) + ENUM_LEN + OPAQUE16_LEN +
                        OPAQUE8_LEN > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    /* Check curve name and ID */
                    b = input[args->idx++];
                    if (b != named_curve) {
                        ERROR_OUT(ECC_CURVETYPE_ERROR, exit_dske);
                    }

                    args->idx += 1;   /* curve type, eat leading 0 */
                    b = input[args->idx++];
                    if ((curveOid = CheckCurveId(b)) < 0) {
                        ERROR_OUT(ECC_CURVE_ERROR, exit_dske);
                    }

                    length = input[args->idx++];
                    if ((args->idx - args->begin) + length > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                #ifdef HAVE_CURVE25519
                    if (ssl->ecdhCurveOID == ECC_X25519_OID) {
                        if (ssl->peerX25519Key == NULL) {
                            ret = AllocKey(ssl, DYNAMIC_TYPE_CURVE25519,
                                           (void**)&ssl->peerX25519Key);
                            if (ret != 0) {
                                goto exit_dske;
                            }
                        } else if (ssl->peerEccKeyPresent) {
                            ret = ReuseKey(ssl, DYNAMIC_TYPE_CURVE25519,
                                           ssl->peerX25519Key);
                            ssl->peerX25519KeyPresent = 0;
                            if (ret != 0) {
                                goto exit_dske;
                            }
                        }

                        if (wc_curve25519_import_public_ex(input + args->idx,
                                length, ssl->peerX25519Key,
                                EC25519_LITTLE_ENDIAN) != 0) {
                            ERROR_OUT(ECC_PEERKEY_ERROR, exit_dske);
                        }

                        args->idx += length;
                        ssl->peerX25519KeyPresent = 1;
                        break;
                    }
                #endif

                    if (ssl->peerEccKey == NULL) {
                        AllocKey(ssl, DYNAMIC_TYPE_ECC,
                                 (void**)&ssl->peerEccKey);
                        if (ret != 0) {
                            goto exit_dske;
                        }
                    } else if (ssl->peerEccKeyPresent) {
                        ret = ReuseKey(ssl, DYNAMIC_TYPE_ECC, ssl->peerEccKey);
                        ssl->peerEccKeyPresent = 0;
                        if (ret != 0) {
                            goto exit_dske;
                        }
                    }

                    curveId = wc_ecc_get_oid(curveOid, NULL, NULL);
                    if (wc_ecc_import_x963_ex(input + args->idx, length,
                        ssl->peerEccKey, curveId) != 0) {
                        ERROR_OUT(ECC_PEERKEY_ERROR, exit_dske);
                    }

                    args->idx += length;
                    ssl->peerEccKeyPresent = 1;
                    break;
                }
            #endif /* HAVE_ECC || !NO_PSK */
                default:
                    ret = BAD_KEA_TYPE_E;
            } /* switch(ssl->specs.kea) */

            /* Check for error */
            if (ret != 0) {
                goto exit_dske;
            }

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_BUILD;
        } /* case TLS_ASYNC_BEGIN */
        FALL_THROUGH;

        case TLS_ASYNC_BUILD:
        {
            switch(ssl->specs.kea)
            {
                case psk_kea:
                case dhe_psk_kea:
                case ecdhe_psk_kea:
                {
                    /* Nothing to do in this sub-state */
                    break;
                }

                case diffie_hellman_kea:
                case ecc_diffie_hellman_kea:
                {
            #if defined(NO_DH) && !defined(HAVE_ECC)
                    ERROR_OUT(NOT_COMPILED_IN, exit_dske);
            #else
                    enum wc_HashType hashType;
                    word16  verifySz;

                    if (ssl->options.usingAnon_cipher) {
                        break;
                    }

                    verifySz = (word16)(args->idx - args->begin);
                    if (verifySz > MAX_DH_SZ) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    if (IsAtLeastTLSv1_2(ssl)) {
                        if ((args->idx - args->begin) + ENUM_LEN + ENUM_LEN >
                                                                        size) {
                            ERROR_OUT(BUFFER_ERROR, exit_dske);
                        }

                        DecodeSigAlg(&input[args->idx], &args->hashAlgo,
                                     &args->sigAlgo);
                        args->idx += 2;
                        hashType = HashAlgoToType(args->hashAlgo);
                        if (hashType == WC_HASH_TYPE_NONE) {
                            ERROR_OUT(ALGO_ID_E, exit_dske);
                        }
                    } else {
                        /* only using sha and md5 for rsa */
                        #ifndef NO_OLD_TLS
                            hashType = WC_HASH_TYPE_SHA;
                            if (args->sigAlgo == rsa_sa_algo) {
                                hashType = WC_HASH_TYPE_MD5_SHA;
                            }
                        #else
                            ERROR_OUT(ALGO_ID_E, exit_dske);
                        #endif
                    }

                    /* signature */
                    if ((args->idx - args->begin) + OPAQUE16_LEN > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    ato16(input + args->idx, &args->verifySigSz);
                    args->idx += OPAQUE16_LEN;

                    if ((args->idx - args->begin) + args->verifySigSz > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dske);
                    }

                    /* buffer for signature */
                    ssl->buffers.sig.buffer = (byte*)XMALLOC(SEED_LEN + verifySz,
                                            ssl->heap, DYNAMIC_TYPE_SIGNATURE);
                    if (ssl->buffers.sig.buffer == NULL) {
                        ERROR_OUT(MEMORY_E, exit_dske);
                    }
                    ssl->buffers.sig.length = SEED_LEN + verifySz;

                    /* build message to hash */
                    XMEMCPY(ssl->buffers.sig.buffer,
                        ssl->arrays->clientRandom, RAN_LEN);
                    XMEMCPY(&ssl->buffers.sig.buffer[RAN_LEN],
                        ssl->arrays->serverRandom, RAN_LEN);
                    XMEMCPY(&ssl->buffers.sig.buffer[RAN_LEN * 2],
                        input + args->begin, verifySz); /* message */

                    if (args->sigAlgo != ed25519_sa_algo) {
                        /* buffer for hash */
                        ssl->buffers.digest.length =
                                                 wc_HashGetDigestSize(hashType);
                        ssl->buffers.digest.buffer = (byte*)XMALLOC(
                            ssl->buffers.digest.length, ssl->heap,
                            DYNAMIC_TYPE_DIGEST);
                        if (ssl->buffers.digest.buffer == NULL) {
                            ERROR_OUT(MEMORY_E, exit_dske);
                        }

                        /* Perform hash */
                        ret = wc_Hash(hashType, ssl->buffers.sig.buffer,
                                                    ssl->buffers.sig.length,
                                                    ssl->buffers.digest.buffer,
                                                    ssl->buffers.digest.length);
                        if (ret != 0) {
                            goto exit_dske;
                        }
                    }

                    switch (args->sigAlgo)
                    {
                    #ifndef NO_RSA
                    #ifdef WC_RSA_PSS
                        case rsa_pss_sa_algo:
                    #endif
                        case rsa_sa_algo:
                        {
                            if (ssl->peerRsaKey == NULL ||
                                                    !ssl->peerRsaKeyPresent) {
                                ERROR_OUT(NO_PEER_KEY, exit_dske);
                            }
                            break;
                        }
                    #endif /* !NO_RSA */
                    #ifdef HAVE_ECC
                        case ecc_dsa_sa_algo:
                        {
                            if (!ssl->peerEccDsaKeyPresent) {
                                ERROR_OUT(NO_PEER_KEY, exit_dske);
                            }
                            break;
                        }
                    #endif /* HAVE_ECC */
                    #ifdef HAVE_ED25519
                        case ed25519_sa_algo:
                        {
                            if (!ssl->peerEd25519KeyPresent) {
                                ERROR_OUT(NO_PEER_KEY, exit_dske);
                            }
                            break;
                        }
                    #endif /* HAVE_ECC */

                    default:
                        ret = ALGO_ID_E;
                    } /* switch (args->sigAlgo) */

            #endif /* NO_DH && !HAVE_ECC */
                    break;
                }
                default:
                    ret = BAD_KEA_TYPE_E;
            } /* switch(ssl->specs.kea) */

            /* Check for error */
            if (ret != 0) {
                goto exit_dske;
            }

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_DO;
        } /* case TLS_ASYNC_BUILD */
        FALL_THROUGH;

        case TLS_ASYNC_DO:
        {
            switch(ssl->specs.kea)
            {
                case psk_kea:
                case dhe_psk_kea:
                case ecdhe_psk_kea:
                {
                    /* Nothing to do in this sub-state */
                    break;
                }

                case diffie_hellman_kea:
                case ecc_diffie_hellman_kea:
                {
            #if defined(NO_DH) && !defined(HAVE_ECC)
                    ERROR_OUT(NOT_COMPILED_IN, exit_dske);
            #else
                    if (ssl->options.usingAnon_cipher) {
                        break;
                    }

                    if (args->verifySig == NULL) {
                        args->verifySig = (byte*)XMALLOC(args->verifySigSz,
                                            ssl->heap, DYNAMIC_TYPE_SIGNATURE);
                        if (args->verifySig == NULL) {
                            ERROR_OUT(MEMORY_E, exit_dske);
                        }
                        XMEMCPY(args->verifySig, input + args->idx,
                                                            args->verifySigSz);
                    }

                    switch (args->sigAlgo)
                    {
                    #ifndef NO_RSA
                    #ifdef WC_RSA_PSS
                        case rsa_pss_sa_algo:
                    #endif
                        case rsa_sa_algo:
                        {
                            ret = RsaVerify(ssl,
                                args->verifySig, args->verifySigSz,
                                &args->output,
                                args->sigAlgo, args->hashAlgo,
                                ssl->peerRsaKey,
                            #ifdef HAVE_PK_CALLBACKS
                                ssl->buffers.peerRsaKey.buffer,
                                ssl->buffers.peerRsaKey.length,
                                ssl->RsaVerifyCtx
                            #else
                                NULL, 0, NULL
                            #endif
                            );

                            if (ret >= 0) {
                                args->sigSz = (word16)ret;
                                ret = 0;
                            }
                            break;
                        }
                    #endif /* !NO_RSA */
                    #ifdef HAVE_ECC
                        case ecc_dsa_sa_algo:
                        {
                            ret = EccVerify(ssl,
                                args->verifySig, args->verifySigSz,
                                ssl->buffers.digest.buffer,
                                ssl->buffers.digest.length,
                                ssl->peerEccDsaKey,
                            #ifdef HAVE_PK_CALLBACKS
                                ssl->buffers.peerEccDsaKey.buffer,
                                ssl->buffers.peerEccDsaKey.length,
                                ssl->EccVerifyCtx
                            #else
                                NULL, 0, NULL
                            #endif
                            );

                            break;
                        }
                    #endif /* HAVE_ECC */
                    #ifdef HAVE_ED25519
                        case ed25519_sa_algo:
                        {
                            ret = Ed25519Verify(ssl,
                                args->verifySig, args->verifySigSz,
                                ssl->buffers.sig.buffer,
                                ssl->buffers.sig.length,
                                ssl->peerEd25519Key,
                            #ifdef HAVE_PK_CALLBACKS
                                ssl->buffers.peerEccDsaKey.buffer,
                                ssl->buffers.peerEccDsaKey.length,
                                ssl->Ed25519VerifyCtx
                            #else
                                NULL, 0, NULL
                            #endif
                            );

                            break;
                        }
                    #endif /* HAVE_ECC */

                    default:
                        ret = ALGO_ID_E;
                    } /* switch (sigAlgo) */
            #endif /* NO_DH && !HAVE_ECC */
                    break;
                }
                default:
                    ret = BAD_KEA_TYPE_E;
            } /* switch(ssl->specs.kea) */

            /* Check for error */
            if (ret != 0) {
                goto exit_dske;
            }

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_VERIFY;
        } /* case TLS_ASYNC_DO */
        FALL_THROUGH;

        case TLS_ASYNC_VERIFY:
        {
            switch(ssl->specs.kea)
            {
                case psk_kea:
                case dhe_psk_kea:
                case ecdhe_psk_kea:
                {
                    /* Nothing to do in this sub-state */
                    break;
                }

                case diffie_hellman_kea:
                case ecc_diffie_hellman_kea:
                {
            #if defined(NO_DH) && !defined(HAVE_ECC)
                    ERROR_OUT(NOT_COMPILED_IN, exit_dske);
            #else
                    if (ssl->options.usingAnon_cipher) {
                        break;
                    }

                    /* increment index after verify is done */
                    args->idx += args->verifySigSz;

                    switch(args->sigAlgo)
                    {
                    #ifndef NO_RSA
                    #ifdef WC_RSA_PSS
                        case rsa_pss_sa_algo:
                            ret = wc_RsaPSS_CheckPadding(
                                             ssl->buffers.digest.buffer,
                                             ssl->buffers.digest.length,
                                             args->output, args->sigSz,
                                             HashAlgoToType(args->hashAlgo));
                            if (ret != 0)
                                return ret;
                            break;
                    #endif
                        case rsa_sa_algo:
                        {
                            if (IsAtLeastTLSv1_2(ssl)) {
                            #ifdef WOLFSSL_SMALL_STACK
                                byte*  encodedSig = NULL;
                            #else
                                byte   encodedSig[MAX_ENCODED_SIG_SZ];
                            #endif
                                word32 encSigSz;

                            #ifdef WOLFSSL_SMALL_STACK
                                encodedSig = (byte*)XMALLOC(MAX_ENCODED_SIG_SZ,
                                                ssl->heap, DYNAMIC_TYPE_SIGNATURE);
                                if (encodedSig == NULL) {
                                    ERROR_OUT(MEMORY_E, exit_dske);
                                }
                            #endif

                                encSigSz = wc_EncodeSignature(encodedSig,
                                    ssl->buffers.digest.buffer,
                                    ssl->buffers.digest.length,
                                    TypeHash(args->hashAlgo));
                                if (encSigSz != args->sigSz || !args->output ||
                                    XMEMCMP(args->output, encodedSig,
                                            min(encSigSz, MAX_ENCODED_SIG_SZ)) != 0) {
                                    ret = VERIFY_SIGN_ERROR;
                                }
                            #ifdef WOLFSSL_SMALL_STACK
                                XFREE(encodedSig, ssl->heap, DYNAMIC_TYPE_SIGNATURE);
                            #endif
                                if (ret != 0) {
                                    goto exit_dske;
                                }
                            }
                            else if (args->sigSz != FINISHED_SZ ||
                                    !args->output ||
                                    XMEMCMP(args->output,
                                            ssl->buffers.digest.buffer,
                                            FINISHED_SZ) != 0) {
                                ERROR_OUT(VERIFY_SIGN_ERROR, exit_dske);
                            }
                            break;
                        }
                    #endif /* !NO_RSA */
                    #ifdef HAVE_ECC
                        case ecc_dsa_sa_algo:
                            /* Nothing to do in this algo */
                            break;
                    #endif /* HAVE_ECC */
                    #ifdef HAVE_ED25519
                        case ed25519_sa_algo:
                            /* Nothing to do in this algo */
                            break;
                    #endif /* HAVE_ECC */
                        default:
                            ret = ALGO_ID_E;
                    } /* switch (sigAlgo) */
            #endif /* NO_DH && !HAVE_ECC */
                    break;
                }
                default:
                    ret = BAD_KEA_TYPE_E;
            } /* switch(ssl->specs.kea) */

            /* Check for error */
            if (ret != 0) {
                goto exit_dske;
            }

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_FINALIZE;
        } /* case TLS_ASYNC_VERIFY */
        FALL_THROUGH;

        case TLS_ASYNC_FINALIZE:
        {
            if (IsEncryptionOn(ssl, 0)) {
                args->idx += ssl->keys.padSz;
            }

            /* QSH extensions */
        #ifdef HAVE_QSH
            if (ssl->peerQSHKeyPresent) {
                word16 name;
                int    qshSz;

                /* extension name */
                ato16(input + args->idx, &name);
                args->idx += OPAQUE16_LEN;

                if (name == TLSX_QUANTUM_SAFE_HYBRID) {
                    /* if qshSz is larger than 0 it is the length of
                       buffer used */
                    if ((qshSz = TLSX_QSHCipher_Parse(ssl, input + args->idx,
                                                       size, 0)) < 0) {
                        ERROR_OUT(qshSz, exit_dske);
                    }
                    args->idx += qshSz;
                }
                else {
                    /* unknown extension sent server ignored handshake */
                    ERROR_OUT(BUFFER_ERROR, exit_dske);
                }
            }
        #endif

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_END;
        } /* case TLS_ASYNC_FINALIZE */
        FALL_THROUGH;

        case TLS_ASYNC_END:
        {
            /* return index */
            *inOutIdx = args->idx;

            ssl->options.serverState = SERVER_KEYEXCHANGE_COMPLETE;
            break;
        }
        default:
            ret = INPUT_CASE_ERROR;
    } /* switch(ssl->options.asyncState) */

exit_dske:

    WOLFSSL_LEAVE("DoServerKeyExchange", ret);

#ifdef WOLFSSL_ASYNC_CRYPT
    /* Handle async operation */
    if (ret == WC_PENDING_E) {
        /* Mark message as not recevied so it can process again */
        ssl->msgsReceived.got_server_key_exchange = 0;

        return ret;
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    /* Final cleanup */
    FreeDskeArgs(ssl, args);
    FreeKeyExchange(ssl);

    return ret;
}


#ifdef HAVE_QSH

#ifdef HAVE_NTRU
/* Encrypt a byte array using ntru
   key    a struct containing the public key to use
   bufIn  array to be encrypted
   inSz   size of bufIn array
   bufOut cipher text out
   outSz  will be set to the new size of cipher text
 */
static int NtruSecretEncrypt(QSHKey* key, byte* bufIn, word32 inSz,
        byte* bufOut, word16* outSz)
{
    int    ret;
    DRBG_HANDLE drbg;

    /* sanity checks on input arguments */
    if (key == NULL || bufIn == NULL || bufOut == NULL || outSz == NULL)
        return BAD_FUNC_ARG;

    if (key->pub.buffer == NULL)
        return BAD_FUNC_ARG;

    switch (key->name) {
        case WOLFSSL_NTRU_EESS439:
        case WOLFSSL_NTRU_EESS593:
        case WOLFSSL_NTRU_EESS743:
            break;
        default:
            WOLFSSL_MSG("Unknown QSH encryption key!");
            return -1;
    }

    /* set up ntru drbg */
    ret = ntru_crypto_drbg_external_instantiate(GetEntropy, &drbg);
    if (ret != DRBG_OK)
        return NTRU_DRBG_ERROR;

    /* encrypt the byte array */
    ret = ntru_crypto_ntru_encrypt(drbg, key->pub.length, key->pub.buffer,
        inSz, bufIn, outSz, bufOut);
    ntru_crypto_drbg_uninstantiate(drbg);
    if (ret != NTRU_OK)
        return NTRU_ENCRYPT_ERROR;

    return ret;
}

/* Decrypt a byte array using ntru
   key    a struct containing the private key to use
   bufIn  array to be decrypted
   inSz   size of bufIn array
   bufOut plain text out
   outSz  will be set to the new size of plain text
 */

static int NtruSecretDecrypt(QSHKey* key, byte* bufIn, word32 inSz,
        byte* bufOut, word16* outSz)
{
    int    ret;
    DRBG_HANDLE drbg;

    /* sanity checks on input arguments */
    if (key == NULL || bufIn == NULL || bufOut == NULL || outSz == NULL)
        return BAD_FUNC_ARG;

    if (key->pri.buffer == NULL)
        return BAD_FUNC_ARG;

    switch (key->name) {
        case WOLFSSL_NTRU_EESS439:
        case WOLFSSL_NTRU_EESS593:
        case WOLFSSL_NTRU_EESS743:
            break;
        default:
            WOLFSSL_MSG("Unknown QSH decryption key!");
            return -1;
    }


    /* set up drbg */
    ret = ntru_crypto_drbg_external_instantiate(GetEntropy, &drbg);
    if (ret != DRBG_OK)
        return NTRU_DRBG_ERROR;

    /* decrypt cipher text */
    ret = ntru_crypto_ntru_decrypt(key->pri.length, key->pri.buffer,
        inSz, bufIn, outSz, bufOut);
    ntru_crypto_drbg_uninstantiate(drbg);
    if (ret != NTRU_OK)
        return NTRU_ENCRYPT_ERROR;

    return ret;
}
#endif /* HAVE_NTRU */

int QSH_Init(WOLFSSL* ssl)
{
    /* check so not initialising twice when running DTLS */
    if (ssl->QSH_secret != NULL)
        return 0;

    /* malloc memory for holding generated secret information */
    if ((ssl->QSH_secret = (QSHSecret*)XMALLOC(sizeof(QSHSecret), ssl->heap,
                    DYNAMIC_TYPE_QSH)) == NULL)
        return MEMORY_E;

    ssl->QSH_secret->CliSi = (buffer*)XMALLOC(sizeof(buffer), ssl->heap,
            DYNAMIC_TYPE_SECRET);
    if (ssl->QSH_secret->CliSi == NULL)
        return MEMORY_E;

    ssl->QSH_secret->SerSi = (buffer*)XMALLOC(sizeof(buffer), ssl->heap,
            DYNAMIC_TYPE_SECRET);
    if (ssl->QSH_secret->SerSi == NULL)
        return MEMORY_E;

    /* initialize variables */
    ssl->QSH_secret->list = NULL;
    ssl->QSH_secret->CliSi->length = 0;
    ssl->QSH_secret->CliSi->buffer = NULL;
    ssl->QSH_secret->SerSi->length = 0;
    ssl->QSH_secret->SerSi->buffer = NULL;

    return 0;
}


static int QSH_Encrypt(QSHKey* key, byte* in, word32 szIn,
                                                       byte* out, word32* szOut)
{
    int ret = 0;
    word16 size = *szOut;

    (void)in;
    (void)szIn;
    (void)out;
    (void)szOut;

    WOLFSSL_MSG("Encrypting QSH key material");

    switch (key->name) {
    #ifdef HAVE_NTRU
        case WOLFSSL_NTRU_EESS439:
        case WOLFSSL_NTRU_EESS593:
        case WOLFSSL_NTRU_EESS743:
            ret = NtruSecretEncrypt(key, in, szIn, out, &size);
            break;
    #endif
        default:
            WOLFSSL_MSG("Unknown QSH encryption key!");
            return -1;
    }

    *szOut = size;

    return ret;
}


/* Decrypt using Quantum Safe Handshake algorithms */
int QSH_Decrypt(QSHKey* key, byte* in, word32 szIn, byte* out, word16* szOut)
{
    int ret = 0;
    word16 size = *szOut;

    (void)in;
    (void)szIn;
    (void)out;
    (void)szOut;

    WOLFSSL_MSG("Decrypting QSH key material");

    switch (key->name) {
    #ifdef HAVE_NTRU
        case WOLFSSL_NTRU_EESS439:
        case WOLFSSL_NTRU_EESS593:
        case WOLFSSL_NTRU_EESS743:
            ret = NtruSecretDecrypt(key, in, szIn, out, &size);
            break;
    #endif
        default:
            WOLFSSL_MSG("Unknown QSH decryption key!");
            return -1;
    }

    *szOut = size;

    return ret;
}


/* Get the max cipher text for corresponding encryption scheme
   (encrypting  48 or max plain text whichever is smaller)
 */
static word32 QSH_MaxSecret(QSHKey* key)
{
    int ret = 0;
#ifdef HAVE_NTRU
    byte isNtru = 0;
    word16 inSz = 48;
    word16 outSz;
    DRBG_HANDLE drbg = 0;
    byte bufIn[48];
#endif

    if (key == NULL || key->pub.length == 0)
        return 0;

    switch(key->name) {
#ifdef HAVE_NTRU
            case WOLFSSL_NTRU_EESS439:
                isNtru   = 1;
                break;
            case WOLFSSL_NTRU_EESS593:
                isNtru   = 1;
                break;
            case WOLFSSL_NTRU_EESS743:
                isNtru   = 1;
                break;
#endif
        default:
            WOLFSSL_MSG("Unknown QSH encryption scheme size!");
            return 0;
    }

#ifdef HAVE_NTRU
    if (isNtru) {
        ret = ntru_crypto_drbg_external_instantiate(GetEntropy, &drbg);
        if (ret != DRBG_OK)
            return NTRU_DRBG_ERROR;
        ret = ntru_crypto_ntru_encrypt(drbg, key->pub.length,
                            key->pub.buffer, inSz, bufIn, &outSz, NULL);
        if (ret != NTRU_OK) {
            return NTRU_ENCRYPT_ERROR;
        }
        ntru_crypto_drbg_uninstantiate(drbg);
        ret = outSz;
    }
#endif

    return ret;
}

/* Generate the secret byte material for pms
   returns length on success and -1 on fail
 */
static int QSH_GenerateSerCliSecret(WOLFSSL* ssl, byte isServer)
{
    int sz       = 0;
    int plainSz  = 48; /* lesser of 48 and max plain text able to encrypt */
    int offset   = 0;
    word32 tmpSz = 0;
    buffer* buf;
    QSHKey* current = ssl->peerQSHKey;
    QSHScheme* schmPre = NULL;
    QSHScheme* schm    = NULL;

    if (ssl == NULL)
        return -1;

    WOLFSSL_MSG("Generating QSH secret key material");

    /* get size of buffer needed */
    while (current) {
        if (current->pub.length != 0) {
            sz += plainSz;
        }
        current = (QSHKey*)current->next;
    }

    /* allocate memory for buffer */
    if (isServer) {
        buf = ssl->QSH_secret->SerSi;
    }
    else {
        buf = ssl->QSH_secret->CliSi;
    }
    buf->length = sz;
    buf->buffer = (byte*)XMALLOC(sz, ssl->heap, DYNAMIC_TYPE_SECRET);
    if (buf->buffer == NULL) {
        WOLFSSL_ERROR(MEMORY_E);
    }

    /* create secret information */
    sz = 0;
    current = ssl->peerQSHKey;
    while (current) {
        schm = (QSHScheme*)XMALLOC(sizeof(QSHScheme), ssl->heap,
                                                       DYNAMIC_TYPE_QSH);
        if (schm == NULL)
            return MEMORY_E;

        /* initialize variables */
        schm->name  = 0;
        schm->PK    = NULL;
        schm->PKLen = 0;
        schm->next  = NULL;
        if (ssl->QSH_secret->list == NULL) {
            ssl->QSH_secret->list = schm;
        }
        else {
            if (schmPre)
                schmPre->next = schm;
        }

        tmpSz = QSH_MaxSecret(current);

        if ((schm->PK = (byte*)XMALLOC(tmpSz, ssl->heap,
                                              DYNAMIC_TYPE_SECRET)) == NULL)
            return -1;

        /* store info for writing extension */
        schm->name = current->name;

        /* no key to use for encryption */
        if (tmpSz == 0) {
            current = (QSHKey*)current->next;
            continue;
        }

        if (wc_RNG_GenerateBlock(ssl->rng, buf->buffer + offset, plainSz)
                                                                         != 0) {
            return -1;
        }
        if (QSH_Encrypt(current, buf->buffer + offset, plainSz, schm->PK,
                                                                 &tmpSz) != 0) {
            return -1;
        }
        schm->PKLen = tmpSz;

        sz += tmpSz;
        offset += plainSz;
        schmPre = schm;
        current = (QSHKey*)current->next;
    }

    return sz;
}


static word32 QSH_KeyGetSize(WOLFSSL* ssl)
{
    word32 sz = 0;
    QSHKey* current = ssl->peerQSHKey;

    if (ssl == NULL)
        return -1;

    sz += OPAQUE16_LEN; /* type of extension ie 0x00 0x18 */
    sz += OPAQUE24_LEN;
    /* get size of buffer needed */
    while (current) {
        sz += OPAQUE16_LEN; /* scheme id */
        sz += OPAQUE16_LEN; /* encrypted key len*/
        sz += QSH_MaxSecret(current);
        current = (QSHKey*)current->next;
    }

    return sz;
}


/* handle QSH key Exchange
   return 0 on success
 */
static word32 QSH_KeyExchangeWrite(WOLFSSL* ssl, byte isServer)
{
    int ret = 0;

    WOLFSSL_ENTER("QSH KeyExchange");

    ret = QSH_GenerateSerCliSecret(ssl, isServer);
    if (ret < 0)
        return MEMORY_E;

    return 0;
}

#endif /* HAVE_QSH */


typedef struct SckeArgs {
    byte*  output; /* not allocated */
    byte*  encSecret;
    byte*  input;
    word32 encSz;
    word32 length;
    int    sendSz;
    int    inputSz;
} SckeArgs;

static void FreeSckeArgs(WOLFSSL* ssl, void* pArgs)
{
    SckeArgs* args = (SckeArgs*)pArgs;

    (void)ssl;

    if (args->encSecret) {
        XFREE(args->encSecret, ssl->heap, DYNAMIC_TYPE_SECRET);
        args->encSecret = NULL;
    }
    if (args->input) {
        XFREE(args->input, ssl->heap, DYNAMIC_TYPE_IN_BUFFER);
        args->input = NULL;
    }
}

int SendClientKeyExchange(WOLFSSL* ssl)
{
    int ret = 0;
#ifdef WOLFSSL_ASYNC_CRYPT
    SckeArgs* args = (SckeArgs*)ssl->async.args;
    typedef char args_test[sizeof(ssl->async.args) >= sizeof(*args) ? 1 : -1];
    (void)sizeof(args_test);
#else
    SckeArgs  args[1];
#endif

    WOLFSSL_ENTER("SendClientKeyExchange");

#ifdef WOLFSSL_ASYNC_CRYPT
    ret = wolfSSL_AsyncPop(ssl, &ssl->options.asyncState);
    if (ret != WC_NOT_PENDING_E) {
        /* Check for error */
        if (ret < 0)
            goto exit_scke;
    }
    else
#endif
    {
        /* Reset state */
        ret = 0;
        ssl->options.asyncState = TLS_ASYNC_BEGIN;
        XMEMSET(args, 0, sizeof(SckeArgs));
    #ifdef WOLFSSL_ASYNC_CRYPT
        ssl->async.freeArgs = FreeSckeArgs;
    #endif
    }

    switch(ssl->options.asyncState)
    {
        case TLS_ASYNC_BEGIN:
        {
            switch (ssl->specs.kea) {
            #ifndef NO_RSA
                case rsa_kea:
                    if (ssl->peerRsaKey == NULL ||
                        ssl->peerRsaKeyPresent == 0) {
                        ERROR_OUT(NO_PEER_KEY, exit_scke);
                    }
                    break;
            #endif
            #ifndef NO_DH
                case diffie_hellman_kea:
                    if (ssl->buffers.serverDH_P.buffer == NULL ||
                        ssl->buffers.serverDH_G.buffer == NULL ||
                        ssl->buffers.serverDH_Pub.buffer == NULL) {
                        ERROR_OUT(NO_PEER_KEY, exit_scke);
                    }
                    break;
            #endif /* NO_DH */
            #ifndef NO_PSK
                case psk_kea:
                    /* sanity check that PSK client callback has been set */
                    if (ssl->options.client_psk_cb == NULL) {
                        WOLFSSL_MSG("No client PSK callback set");
                        ERROR_OUT(PSK_KEY_ERROR, exit_scke);
                    }
                    break;
            #endif /* NO_PSK */
            #if !defined(NO_DH) && !defined(NO_PSK)
                case dhe_psk_kea:
                    if (ssl->buffers.serverDH_P.buffer == NULL ||
                        ssl->buffers.serverDH_G.buffer == NULL ||
                        ssl->buffers.serverDH_Pub.buffer == NULL) {
                        ERROR_OUT(NO_PEER_KEY, exit_scke);
                    }

                    /* sanity check that PSK client callback has been set */
                    if (ssl->options.client_psk_cb == NULL) {
                        WOLFSSL_MSG("No client PSK callback set");
                        ERROR_OUT(PSK_KEY_ERROR, exit_scke);
                    }
                    break;
            #endif /* !NO_DH && !NO_PSK */
            #if (defined(HAVE_ECC) || defined(HAVE_CURVE25519)) && !defined(NO_PSK)
                case ecdhe_psk_kea:
                    /* sanity check that PSK client callback has been set */
                    if (ssl->options.client_psk_cb == NULL) {
                        WOLFSSL_MSG("No client PSK callback set");
                        ERROR_OUT(PSK_KEY_ERROR, exit_scke);
                    }

                #ifdef HAVE_CURVE25519
                    if (ssl->peerX25519KeyPresent) {
                        /* Check client ECC public key */
                        if (!ssl->peerX25519Key || !ssl->peerX25519Key->dp) {
                            ERROR_OUT(NO_PEER_KEY, exit_scke);
                        }

                    #ifdef HAVE_PK_CALLBACKS
                        /* if callback then use it for shared secret */
                        if (ssl->ctx->X25519SharedSecretCb != NULL) {
                            break;
                        }
                    #endif

                        /* create private key */
                        ssl->hsType = DYNAMIC_TYPE_CURVE25519;
                        ret = AllocKey(ssl, ssl->hsType, &ssl->hsKey);
                        if (ret != 0) {
                            goto exit_scke;
                        }

                        ret = X25519MakeKey(ssl, (curve25519_key*)ssl->hsKey,
                                            ssl->peerX25519Key);
                        break;
                    }
                #endif
                    /* Check client ECC public key */
                    if (!ssl->peerEccKey || !ssl->peerEccKeyPresent ||
                                            !ssl->peerEccKey->dp) {
                        ERROR_OUT(NO_PEER_KEY, exit_scke);
                    }

                #ifdef HAVE_PK_CALLBACKS
                    /* if callback then use it for shared secret */
                    if (ssl->ctx->EccSharedSecretCb != NULL) {
                        break;
                    }
                #endif

                    /* create private key */
                    ssl->hsType = DYNAMIC_TYPE_ECC;
                    ret = AllocKey(ssl, ssl->hsType, &ssl->hsKey);
                    if (ret != 0) {
                        goto exit_scke;
                    }

                    ret = EccMakeKey(ssl, (ecc_key*)ssl->hsKey, ssl->peerEccKey);

                    break;
            #endif /* (HAVE_ECC || HAVE_CURVE25519) && !NO_PSK */
            #ifdef HAVE_NTRU
                case ntru_kea:
                    if (ssl->peerNtruKeyPresent == 0) {
                        ERROR_OUT(NO_PEER_KEY, exit_scke);
                    }
                    break;
            #endif /* HAVE_NTRU */
            #if defined(HAVE_ECC) || defined(HAVE_CURVE25519)
                case ecc_diffie_hellman_kea:
                {
                #ifdef HAVE_ECC
                    ecc_key* peerKey;
                #endif

            #ifdef HAVE_PK_CALLBACKS
                    /* if callback then use it for shared secret */
                #ifdef HAVE_CURVE25519
                    if (ssl->ecdhCurveOID == ECC_X25519_OID) {
                        if (ssl->ctx->X25519SharedSecretCb != NULL)
                            break;
                    }
                    else
                #endif
                    if (ssl->ctx->EccSharedSecretCb != NULL) {
                        break;
                    }
            #endif /* HAVE_PK_CALLBACKS */

                #ifdef HAVE_CURVE25519
                    if (ssl->peerX25519KeyPresent) {
                        if (!ssl->peerX25519Key || !ssl->peerX25519Key->dp) {
                            ERROR_OUT(NO_PEER_KEY, exit_scke);
                        }

                        /* create private key */
                        ssl->hsType = DYNAMIC_TYPE_CURVE25519;
                        ret = AllocKey(ssl, ssl->hsType, &ssl->hsKey);
                        if (ret != 0) {
                            goto exit_scke;
                        }

                        ret = X25519MakeKey(ssl, (curve25519_key*)ssl->hsKey,
                                            ssl->peerX25519Key);
                        break;
                    }
                #endif
                #ifdef HAVE_ECC
                    if (ssl->specs.static_ecdh) {
                        /* TODO: EccDsa is really fixed Ecc change naming */
                        if (!ssl->peerEccDsaKey ||
                                !ssl->peerEccDsaKeyPresent ||
                                    !ssl->peerEccDsaKey->dp) {
                            ERROR_OUT(NO_PEER_KEY, exit_scke);
                        }
                        peerKey = ssl->peerEccDsaKey;
                    }
                    else {
                        if (!ssl->peerEccKey || !ssl->peerEccKeyPresent ||
                                                !ssl->peerEccKey->dp) {
                            ERROR_OUT(NO_PEER_KEY, exit_scke);
                        }
                        peerKey = ssl->peerEccKey;
                    }
                    if (peerKey == NULL) {
                        ERROR_OUT(NO_PEER_KEY, exit_scke);
                    }

                    /* create private key */
                    ssl->hsType = DYNAMIC_TYPE_ECC;
                    ret = AllocKey(ssl, ssl->hsType, &ssl->hsKey);
                    if (ret != 0) {
                        goto exit_scke;
                    }

                    ret = EccMakeKey(ssl, (ecc_key*)ssl->hsKey, peerKey);
                #endif

                    break;
                }
            #endif /* HAVE_ECC || HAVE_CURVE25519 */

                default:
                    ret = BAD_KEA_TYPE_E;
            } /* switch(ssl->specs.kea) */

            /* Check for error */
            if (ret != 0) {
                goto exit_scke;
            }

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_BUILD;
        } /* case TLS_ASYNC_BEGIN */
        FALL_THROUGH;

        case TLS_ASYNC_BUILD:
        {
            args->encSz = MAX_ENCRYPT_SZ;
            args->encSecret = (byte*)XMALLOC(args->encSz, ssl->heap,
                                                    DYNAMIC_TYPE_SECRET);
            if (args->encSecret == NULL) {
                ERROR_OUT(MEMORY_E, exit_scke);
            }

            switch(ssl->specs.kea)
            {
            #ifndef NO_RSA
                case rsa_kea:
                {
                    ret = wc_RNG_GenerateBlock(ssl->rng,
                        ssl->arrays->preMasterSecret, SECRET_LEN);
                    if (ret != 0) {
                        goto exit_scke;
                    }

                    ssl->arrays->preMasterSecret[0] = ssl->chVersion.major;
                    ssl->arrays->preMasterSecret[1] = ssl->chVersion.minor;
                    ssl->arrays->preMasterSz = SECRET_LEN;
                    break;
                }
            #endif /* !NO_RSA */
            #ifndef NO_DH
                case diffie_hellman_kea:
                {
                    ssl->buffers.sig.length = ENCRYPT_LEN;
                    ssl->buffers.sig.buffer = (byte*)XMALLOC(ENCRYPT_LEN,
                                            ssl->heap, DYNAMIC_TYPE_SIGNATURE);
                    if (ssl->buffers.sig.buffer == NULL) {
                        ERROR_OUT(MEMORY_E, exit_scke);
                    }

                    ret = AllocKey(ssl, DYNAMIC_TYPE_DH,
                                            (void**)&ssl->buffers.serverDH_Key);
                    if (ret != 0) {
                        goto exit_scke;
                    }

                    ret = wc_DhSetKey(ssl->buffers.serverDH_Key,
                        ssl->buffers.serverDH_P.buffer,
                        ssl->buffers.serverDH_P.length,
                        ssl->buffers.serverDH_G.buffer,
                        ssl->buffers.serverDH_G.length);
                    if (ret != 0) {
                        goto exit_scke;
                    }

                    /* for DH, encSecret is Yc, agree is pre-master */
                    ret = DhGenKeyPair(ssl, ssl->buffers.serverDH_Key,
                        ssl->buffers.sig.buffer, &ssl->buffers.sig.length,
                        args->encSecret, &args->encSz);

                    /* set the max agree result size */
                    ssl->arrays->preMasterSz = ENCRYPT_LEN;
                    break;
                }
            #endif /* !NO_DH */
            #ifndef NO_PSK
                case psk_kea:
                {
                    byte* pms = ssl->arrays->preMasterSecret;
                    ssl->arrays->psk_keySz = ssl->options.client_psk_cb(ssl,
                        ssl->arrays->server_hint, ssl->arrays->client_identity,
                        MAX_PSK_ID_LEN, ssl->arrays->psk_key, MAX_PSK_KEY_LEN);
                    if (ssl->arrays->psk_keySz == 0 ||
                        ssl->arrays->psk_keySz > MAX_PSK_KEY_LEN) {
                        ERROR_OUT(PSK_KEY_ERROR, exit_scke);
                    }
                    ssl->arrays->client_identity[MAX_PSK_ID_LEN] = '\0'; /* null term */
                    args->encSz = (word32)XSTRLEN(ssl->arrays->client_identity);
                    if (args->encSz > MAX_PSK_ID_LEN) {
                        ERROR_OUT(CLIENT_ID_ERROR, exit_scke);
                    }
                    XMEMCPY(args->encSecret, ssl->arrays->client_identity,
                                                                args->encSz);

                    /* make psk pre master secret */
                    /* length of key + length 0s + length of key + key */
                    c16toa((word16)ssl->arrays->psk_keySz, pms);
                    pms += OPAQUE16_LEN;
                    XMEMSET(pms, 0, ssl->arrays->psk_keySz);
                    pms += ssl->arrays->psk_keySz;
                    c16toa((word16)ssl->arrays->psk_keySz, pms);
                    pms += OPAQUE16_LEN;
                    XMEMCPY(pms, ssl->arrays->psk_key, ssl->arrays->psk_keySz);
                    ssl->arrays->preMasterSz = (ssl->arrays->psk_keySz * 2) +
                        (2 * OPAQUE16_LEN);
                    ForceZero(ssl->arrays->psk_key, ssl->arrays->psk_keySz);
                    ssl->arrays->psk_keySz = 0; /* No further need */
                    break;
                }
            #endif /* !NO_PSK */
            #if !defined(NO_DH) && !defined(NO_PSK)
                case dhe_psk_kea:
                {
                    word32 esSz = 0;
                    args->output = args->encSecret;

                    ssl->arrays->psk_keySz = ssl->options.client_psk_cb(ssl,
                         ssl->arrays->server_hint, ssl->arrays->client_identity,
                         MAX_PSK_ID_LEN, ssl->arrays->psk_key, MAX_PSK_KEY_LEN);
                    if (ssl->arrays->psk_keySz == 0 ||
                                     ssl->arrays->psk_keySz > MAX_PSK_KEY_LEN) {
                        ERROR_OUT(PSK_KEY_ERROR, exit_scke);
                    }
                    ssl->arrays->client_identity[MAX_PSK_ID_LEN] = '\0'; /* null term */
                    esSz = (word32)XSTRLEN(ssl->arrays->client_identity);

                    if (esSz > MAX_PSK_ID_LEN) {
                        ERROR_OUT(CLIENT_ID_ERROR, exit_scke);
                    }

                    ssl->buffers.sig.length = ENCRYPT_LEN;
                    ssl->buffers.sig.buffer = (byte*)XMALLOC(ENCRYPT_LEN,
                                            ssl->heap, DYNAMIC_TYPE_SIGNATURE);
                    if (ssl->buffers.sig.buffer == NULL) {
                        ERROR_OUT(MEMORY_E, exit_scke);
                    }

                    c16toa((word16)esSz, args->output);
                    args->output += OPAQUE16_LEN;
                    XMEMCPY(args->output, ssl->arrays->client_identity, esSz);
                    args->output += esSz;
                    args->encSz = esSz + OPAQUE16_LEN;

                    args->length = 0;

                    ret = AllocKey(ssl, DYNAMIC_TYPE_DH,
                                            (void**)&ssl->buffers.serverDH_Key);
                    if (ret != 0) {
                        goto exit_scke;
                    }

                    ret = wc_DhSetKey(ssl->buffers.serverDH_Key,
                        ssl->buffers.serverDH_P.buffer,
                        ssl->buffers.serverDH_P.length,
                        ssl->buffers.serverDH_G.buffer,
                        ssl->buffers.serverDH_G.length);
                    if (ret != 0) {
                        goto exit_scke;
                    }

                    /* for DH, encSecret is Yc, agree is pre-master */
                    ret = DhGenKeyPair(ssl, ssl->buffers.serverDH_Key,
                        ssl->buffers.sig.buffer, &ssl->buffers.sig.length,
                        args->output + OPAQUE16_LEN, &args->length);
                    break;
                }
            #endif /* !NO_DH && !NO_PSK */
            #if (defined(HAVE_ECC) || defined(HAVE_CURVE25519)) && !defined(NO_PSK)
                case ecdhe_psk_kea:
                {
                    word32 esSz = 0;
                    args->output = args->encSecret;

                    /* Send PSK client identity */
                    ssl->arrays->psk_keySz = ssl->options.client_psk_cb(ssl,
                         ssl->arrays->server_hint, ssl->arrays->client_identity,
                         MAX_PSK_ID_LEN, ssl->arrays->psk_key, MAX_PSK_KEY_LEN);
                    if (ssl->arrays->psk_keySz == 0 ||
                                     ssl->arrays->psk_keySz > MAX_PSK_KEY_LEN) {
                        ERROR_OUT(PSK_KEY_ERROR, exit_scke);
                    }
                    ssl->arrays->client_identity[MAX_PSK_ID_LEN] = '\0'; /* null term */
                    esSz = (word32)XSTRLEN(ssl->arrays->client_identity);
                    if (esSz > MAX_PSK_ID_LEN) {
                        ERROR_OUT(CLIENT_ID_ERROR, exit_scke);
                    }

                    /* place size and identity in output buffer sz:identity */
                    c16toa((word16)esSz, args->output);
                    args->output += OPAQUE16_LEN;
                    XMEMCPY(args->output, ssl->arrays->client_identity, esSz);
                    args->output += esSz;
                    args->encSz = esSz + OPAQUE16_LEN;

                    /* length is used for public key size */
                    args->length = MAX_ENCRYPT_SZ;

                    /* Create shared ECC key leaving room at the begining
                       of buffer for size of shared key. */
                    ssl->arrays->preMasterSz = ENCRYPT_LEN - OPAQUE16_LEN;

                #ifdef HAVE_CURVE25519
                    if (ssl->ecdhCurveOID == ECC_X25519_OID) {
                    #ifdef HAVE_PK_CALLBACKS
                        /* if callback then use it for shared secret */
                        if (ssl->ctx->X25519SharedSecretCb != NULL) {
                            break;
                        }
                    #endif

                        ret = wc_curve25519_export_public_ex(
                                (curve25519_key*)ssl->hsKey,
                                args->output + OPAQUE8_LEN, &args->length,
                                EC25519_LITTLE_ENDIAN);
                        if (ret != 0) {
                            ERROR_OUT(ECC_EXPORT_ERROR, exit_scke);
                        }

                        break;
                    }
                #endif
                #ifdef HAVE_PK_CALLBACKS
                    /* if callback then use it for shared secret */
                    if (ssl->ctx->EccSharedSecretCb != NULL) {
                        break;
                    }
                #endif

                    /* Place ECC key in output buffer, leaving room for size */
                    ret = wc_ecc_export_x963((ecc_key*)ssl->hsKey,
                                    args->output + OPAQUE8_LEN, &args->length);
                    if (ret != 0) {
                        ERROR_OUT(ECC_EXPORT_ERROR, exit_scke);
                    }

                    break;
                }
            #endif /* HAVE_ECC && !NO_PSK */
            #ifdef HAVE_NTRU
                case ntru_kea:
                {
                    ret = wc_RNG_GenerateBlock(ssl->rng,
                                  ssl->arrays->preMasterSecret, SECRET_LEN);
                    if (ret != 0) {
                        goto exit_scke;
                    }

                    ssl->arrays->preMasterSz = SECRET_LEN;
                    args->encSz = MAX_ENCRYPT_SZ;
                    break;
                }
            #endif /* HAVE_NTRU */
            #if defined(HAVE_ECC) || defined(HAVE_CURVE25519)
                case ecc_diffie_hellman_kea:
                {
                    ssl->arrays->preMasterSz = ENCRYPT_LEN;

                #ifdef HAVE_CURVE25519
                    if (ssl->hsType == DYNAMIC_TYPE_CURVE25519) {
                    #ifdef HAVE_PK_CALLBACKS
                        /* if callback then use it for shared secret */
                        if (ssl->ctx->X25519SharedSecretCb != NULL) {
                            break;
                        }
                    #endif

                        ret = wc_curve25519_export_public_ex(
                                (curve25519_key*)ssl->hsKey,
                                args->encSecret + OPAQUE8_LEN, &args->encSz,
                                EC25519_LITTLE_ENDIAN);
                        if (ret != 0) {
                            ERROR_OUT(ECC_EXPORT_ERROR, exit_scke);
                        }

                        break;
                    }
                #endif
                #ifdef HAVE_ECC
                #ifdef HAVE_PK_CALLBACKS
                    /* if callback then use it for shared secret */
                    if (ssl->ctx->EccSharedSecretCb != NULL) {
                        break;
                    }
                #endif

                    /* Place ECC key in buffer, leaving room for size */
                    ret = wc_ecc_export_x963((ecc_key*)ssl->hsKey,
                                args->encSecret + OPAQUE8_LEN, &args->encSz);
                    if (ret != 0) {
                        ERROR_OUT(ECC_EXPORT_ERROR, exit_scke);
                    }
                #endif /* HAVE_ECC */
                    break;
                }
            #endif /* HAVE_ECC || HAVE_CURVE25519 */

                default:
                    ret = BAD_KEA_TYPE_E;
            } /* switch(ssl->specs.kea) */

            /* Check for error */
            if (ret != 0) {
                goto exit_scke;
            }

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_DO;
        } /* case TLS_ASYNC_BUILD */
        FALL_THROUGH;

        case TLS_ASYNC_DO:
        {
            switch(ssl->specs.kea)
            {
            #ifndef NO_RSA
                case rsa_kea:
                {
                    ret = RsaEnc(ssl,
                        ssl->arrays->preMasterSecret, SECRET_LEN,
                        args->encSecret, &args->encSz,
                        ssl->peerRsaKey,
                    #if defined(HAVE_PK_CALLBACKS)
                        ssl->buffers.peerRsaKey.buffer,
                        ssl->buffers.peerRsaKey.length,
                        ssl->RsaEncCtx
                    #else
                        NULL, 0, NULL
                    #endif
                    );

                    break;
                }
            #endif /* !NO_RSA */
            #ifndef NO_DH
                case diffie_hellman_kea:
                {
                    ret = DhAgree(ssl, ssl->buffers.serverDH_Key,
                        ssl->buffers.sig.buffer, ssl->buffers.sig.length,
                        ssl->buffers.serverDH_Pub.buffer,
                        ssl->buffers.serverDH_Pub.length,
                        ssl->arrays->preMasterSecret,
                        &ssl->arrays->preMasterSz);
                    break;
                }
            #endif /* !NO_DH */
            #ifndef NO_PSK
                case psk_kea:
                {
                    break;
                }
            #endif /* !NO_PSK */
            #if !defined(NO_DH) && !defined(NO_PSK)
                case dhe_psk_kea:
                {
                    ret = DhAgree(ssl, ssl->buffers.serverDH_Key,
                        ssl->buffers.sig.buffer, ssl->buffers.sig.length,
                        ssl->buffers.serverDH_Pub.buffer,
                        ssl->buffers.serverDH_Pub.length,
                        ssl->arrays->preMasterSecret + OPAQUE16_LEN,
                        &ssl->arrays->preMasterSz);
                    break;
                }
            #endif /* !NO_DH && !NO_PSK */
            #if (defined(HAVE_ECC) || defined(HAVE_CURVE25519)) && !defined(NO_PSK)
                case ecdhe_psk_kea:
                {
                #ifdef HAVE_CURVE25519
                    if (ssl->peerX25519KeyPresent) {
                        ret = X25519SharedSecret(ssl,
                            (curve25519_key*)ssl->hsKey, ssl->peerX25519Key,
                            args->output + OPAQUE8_LEN, &args->length,
                            ssl->arrays->preMasterSecret + OPAQUE16_LEN,
                            &ssl->arrays->preMasterSz,
                            WOLFSSL_CLIENT_END,
                        #ifdef HAVE_PK_CALLBACKS
                            ssl->EccSharedSecretCtx
                        #else
                            NULL
                        #endif
                        );
                        break;
                    }
                #endif
                    ret = EccSharedSecret(ssl,
                        (ecc_key*)ssl->hsKey, ssl->peerEccKey,
                        args->output + OPAQUE8_LEN, &args->length,
                        ssl->arrays->preMasterSecret + OPAQUE16_LEN,
                        &ssl->arrays->preMasterSz,
                        WOLFSSL_CLIENT_END,
                    #ifdef HAVE_PK_CALLBACKS
                        ssl->EccSharedSecretCtx
                    #else
                        NULL
                    #endif
                    );
                    break;
                }
            #endif /* (HAVE_ECC || HAVE_CURVE25519) && !NO_PSK */
            #ifdef HAVE_NTRU
                case ntru_kea:
                {
                    word32 rc;
                    word16 tmpEncSz = (word16)args->encSz;
                    DRBG_HANDLE drbg;

                    rc = ntru_crypto_drbg_external_instantiate(GetEntropy, &drbg);
                    if (rc != DRBG_OK) {
                        ERROR_OUT(NTRU_DRBG_ERROR, exit_scke);
                    }
                    rc = ntru_crypto_ntru_encrypt(drbg, ssl->peerNtruKeyLen,
                                                  ssl->peerNtruKey,
                                                  ssl->arrays->preMasterSz,
                                                  ssl->arrays->preMasterSecret,
                                                  &tmpEncSz,
                                                  args->encSecret);
                    args->encSz = tmpEncSz;
                    ntru_crypto_drbg_uninstantiate(drbg);
                    if (rc != NTRU_OK) {
                        ERROR_OUT(NTRU_ENCRYPT_ERROR, exit_scke);
                    }
                    ret = 0;
                    break;
                }
            #endif /* HAVE_NTRU */
            #if defined(HAVE_ECC) || defined(HAVE_CURVE25519)
                case ecc_diffie_hellman_kea:
                {
                #ifdef HAVE_ECC
                    ecc_key* peerKey;
                #endif

                #ifdef HAVE_CURVE25519
                    if (ssl->peerX25519KeyPresent) {
                        ret = X25519SharedSecret(ssl,
                            (curve25519_key*)ssl->hsKey, ssl->peerX25519Key,
                            args->encSecret + OPAQUE8_LEN, &args->encSz,
                            ssl->arrays->preMasterSecret,
                            &ssl->arrays->preMasterSz,
                            WOLFSSL_CLIENT_END,
                        #ifdef HAVE_PK_CALLBACKS
                            ssl->EccSharedSecretCtx
                        #else
                            NULL
                        #endif
                        );
                        break;
                    }
                #endif
                #ifdef HAVE_ECC
                    peerKey = (ssl->specs.static_ecdh) ?
                              ssl->peerEccDsaKey : ssl->peerEccKey;

                    ret = EccSharedSecret(ssl,
                        (ecc_key*)ssl->hsKey, peerKey,
                        args->encSecret + OPAQUE8_LEN, &args->encSz,
                        ssl->arrays->preMasterSecret,
                        &ssl->arrays->preMasterSz,
                        WOLFSSL_CLIENT_END,
                    #ifdef HAVE_PK_CALLBACKS
                        ssl->EccSharedSecretCtx
                    #else
                        NULL
                    #endif
                    );
                #endif

                    break;
                }
            #endif /* HAVE_ECC || HAVE_CURVE25519 */

                default:
                    ret = BAD_KEA_TYPE_E;
            } /* switch(ssl->specs.kea) */

            /* Check for error */
            if (ret != 0) {
                goto exit_scke;
            }

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_VERIFY;
        } /* case TLS_ASYNC_DO */
        FALL_THROUGH;

        case TLS_ASYNC_VERIFY:
        {
            switch(ssl->specs.kea)
            {
            #ifndef NO_RSA
                case rsa_kea:
                {
                    break;
                }
            #endif /* !NO_RSA */
            #ifndef NO_DH
                case diffie_hellman_kea:
                {
                    break;
                }
            #endif /* !NO_DH */
            #ifndef NO_PSK
                case psk_kea:
                {
                    break;
                }
            #endif /* !NO_PSK */
            #if !defined(NO_DH) && !defined(NO_PSK)
                case dhe_psk_kea:
                {
                    byte* pms = ssl->arrays->preMasterSecret;

                    /* validate args */
                    if (args->output == NULL || args->length == 0) {
                        ERROR_OUT(BAD_FUNC_ARG, exit_scke);
                    }

                    c16toa((word16)args->length, args->output);
                    args->encSz += args->length + OPAQUE16_LEN;
                    c16toa((word16)ssl->arrays->preMasterSz, pms);
                    ssl->arrays->preMasterSz += OPAQUE16_LEN;
                    pms += ssl->arrays->preMasterSz;

                    /* make psk pre master secret */
                    /* length of key + length 0s + length of key + key */
                    c16toa((word16)ssl->arrays->psk_keySz, pms);
                    pms += OPAQUE16_LEN;
                    XMEMCPY(pms, ssl->arrays->psk_key, ssl->arrays->psk_keySz);
                    ssl->arrays->preMasterSz +=
                                         ssl->arrays->psk_keySz + OPAQUE16_LEN;
                    ForceZero(ssl->arrays->psk_key, ssl->arrays->psk_keySz);
                    ssl->arrays->psk_keySz = 0; /* No further need */
                    break;
                }
            #endif /* !NO_DH && !NO_PSK */
            #if defined(HAVE_ECC) && !defined(NO_PSK)
                case ecdhe_psk_kea:
                {
                    byte* pms = ssl->arrays->preMasterSecret;

                    /* validate args */
                    if (args->output == NULL || args->length > ENCRYPT_LEN) {
                        ERROR_OUT(BAD_FUNC_ARG, exit_scke);
                    }

                    /* place size of public key in output buffer */
                    *args->output = (byte)args->length;
                    args->encSz += args->length + OPAQUE8_LEN;

                    /* Create pre master secret is the concatination of
                       eccSize + eccSharedKey + pskSize + pskKey */
                    c16toa((word16)ssl->arrays->preMasterSz, pms);
                    ssl->arrays->preMasterSz += OPAQUE16_LEN;
                    pms += ssl->arrays->preMasterSz;

                    c16toa((word16)ssl->arrays->psk_keySz, pms);
                    pms += OPAQUE16_LEN;
                    XMEMCPY(pms, ssl->arrays->psk_key, ssl->arrays->psk_keySz);
                    ssl->arrays->preMasterSz +=
                                          ssl->arrays->psk_keySz + OPAQUE16_LEN;

                    ForceZero(ssl->arrays->psk_key, ssl->arrays->psk_keySz);
                    ssl->arrays->psk_keySz = 0; /* No further need */
                    break;
                }
            #endif /* HAVE_ECC && !NO_PSK */
            #ifdef HAVE_NTRU
                case ntru_kea:
                {
                    break;
                }
            #endif /* HAVE_NTRU */
            #ifdef HAVE_ECC
                case ecc_diffie_hellman_kea:
                {
                    /* place size of public key in buffer */
                    *args->encSecret = (byte)args->encSz;
                    args->encSz += OPAQUE8_LEN;
                    break;
                }
            #endif /* HAVE_ECC */

                default:
                    ret = BAD_KEA_TYPE_E;
            } /* switch(ssl->specs.kea) */

            /* Check for error */
            if (ret != 0) {
                goto exit_scke;
            }

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_FINALIZE;
        } /* case TLS_ASYNC_VERIFY */
        FALL_THROUGH;

        case TLS_ASYNC_FINALIZE:
        {
            word32 tlsSz = 0;
            word32 idx = 0;

        #ifdef HAVE_QSH
            word32 qshSz = 0;
            if (ssl->peerQSHKeyPresent) {
                qshSz = QSH_KeyGetSize(ssl);
            }
        #endif

            if (ssl->options.tls || ssl->specs.kea == diffie_hellman_kea) {
                tlsSz = 2;
            }

            if (ssl->specs.kea == ecc_diffie_hellman_kea ||
                ssl->specs.kea == dhe_psk_kea ||
                ssl->specs.kea == ecdhe_psk_kea) { /* always off */
                tlsSz = 0;
            }

            idx = HANDSHAKE_HEADER_SZ + RECORD_HEADER_SZ;
            args->sendSz = args->encSz + tlsSz + idx;

        #ifdef WOLFSSL_DTLS
            if (ssl->options.dtls) {
                idx    += DTLS_HANDSHAKE_EXTRA + DTLS_RECORD_EXTRA;
                args->sendSz += DTLS_HANDSHAKE_EXTRA + DTLS_RECORD_EXTRA;
            }
        #endif

            if (IsEncryptionOn(ssl, 1)) {
                args->sendSz += MAX_MSG_EXTRA;
            }

        #ifdef HAVE_QSH
            args->encSz += qshSz;
            args->sendSz += qshSz;
        #endif

            /* check for available size */
            if ((ret = CheckAvailableSize(ssl, args->sendSz)) != 0) {
                goto exit_scke;
            }

            /* get output buffer */
            args->output = ssl->buffers.outputBuffer.buffer +
                           ssl->buffers.outputBuffer.length;

        #ifdef HAVE_QSH
            if (ssl->peerQSHKeyPresent) {
                byte idxSave = idx;
                idx = args->sendSz - qshSz;

                if (QSH_KeyExchangeWrite(ssl, 0) != 0) {
                    ERROR_OUT(MEMORY_E, exit_scke);
                }

                /* extension type */
                c16toa(TLSX_QUANTUM_SAFE_HYBRID, args->output + idx);
                idx += OPAQUE16_LEN;

                /* write to output and check amount written */
                if (TLSX_QSHPK_Write(ssl->QSH_secret->list,
                            args->output + idx) > qshSz - OPAQUE16_LEN) {
                    ERROR_OUT(MEMORY_E, exit_scke);
                }

                idx = idxSave;
            }
        #endif

            AddHeaders(args->output, args->encSz + tlsSz, client_key_exchange, ssl);

        #ifdef HAVE_QSH
            if (ssl->peerQSHKeyPresent) {
                args->encSz -= qshSz;
            }
        #endif
            if (tlsSz) {
                c16toa((word16)args->encSz, &args->output[idx]);
                idx += OPAQUE16_LEN;
            }
            XMEMCPY(args->output + idx, args->encSecret, args->encSz);
            idx += args->encSz;

            if (IsEncryptionOn(ssl, 1)) {
                args->inputSz = idx - RECORD_HEADER_SZ; /* buildmsg adds rechdr */
                args->input = (byte*)XMALLOC(args->inputSz, ssl->heap,
                                                       DYNAMIC_TYPE_IN_BUFFER);
                if (args->input == NULL) {
                    ERROR_OUT(MEMORY_E, exit_scke);
                }

                XMEMCPY(args->input, args->output + RECORD_HEADER_SZ,
                                                                args->inputSz);
            }

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_END;
        } /* case TLS_ASYNC_FINALIZE */
        FALL_THROUGH;

        case TLS_ASYNC_END:
        {
            if (IsEncryptionOn(ssl, 1)) {
                ret = BuildMessage(ssl, args->output, args->sendSz,
                            args->input, args->inputSz, handshake, 1, 0, 0);
                XFREE(args->input, ssl->heap, DYNAMIC_TYPE_IN_BUFFER);
                args->input = NULL; /* make sure its not double free'd on cleanup */

                if (ret >= 0) {
                    args->sendSz = ret;
                    ret = 0;
                }
            }
            else {
            #ifdef WOLFSSL_DTLS
                if (ssl->options.dtls)
                    DtlsSEQIncrement(ssl, CUR_ORDER);
            #endif
                ret = HashOutput(ssl, args->output, args->sendSz, 0);
            }

            if (ret != 0) {
                goto exit_scke;
            }

        #ifdef WOLFSSL_DTLS
            if (IsDtlsNotSctpMode(ssl)) {
                if ((ret = DtlsMsgPoolSave(ssl, args->output, args->sendSz)) != 0) {
                    goto exit_scke;
                }
            }
        #endif

        #ifdef WOLFSSL_CALLBACKS
            if (ssl->hsInfoOn)
                AddPacketName("ClientKeyExchange", &ssl->handShakeInfo);
            if (ssl->toInfoOn)
                AddPacketInfo("ClientKeyExchange", &ssl->timeoutInfo,
                              args->output, args->sendSz, ssl->heap);
        #endif

            ssl->buffers.outputBuffer.length += args->sendSz;

            if (!ssl->options.groupMessages) {
                ret = SendBuffered(ssl);
            }
            if (ret == 0 || ret == WANT_WRITE) {
                int tmpRet = MakeMasterSecret(ssl);
                if (tmpRet != 0) {
                    ret = tmpRet;   /* save WANT_WRITE unless more serious */
                }
                ssl->options.clientState = CLIENT_KEYEXCHANGE_COMPLETE;
            }
            break;
        }
        default:
            ret = INPUT_CASE_ERROR;
    } /* switch(ssl->options.asyncState) */

exit_scke:

    WOLFSSL_LEAVE("SendClientKeyExchange", ret);

#ifdef WOLFSSL_ASYNC_CRYPT
    /* Handle async operation */
    if (ret == WC_PENDING_E)
        return ret;
#endif

    /* No further need for PMS */
    ForceZero(ssl->arrays->preMasterSecret, ssl->arrays->preMasterSz);
    ssl->arrays->preMasterSz = 0;

    /* Final cleanup */
    FreeSckeArgs(ssl, args);
    FreeKeyExchange(ssl);

    return ret;
}


#ifndef NO_CERTS
/* Decode the private key - RSA, ECC, or Ed25519 - and creates a key object.
 * The signature type is set as well.
 * The maximum length of a signature is returned.
 *
 * ssl     The SSL/TLS object.
 * length  The length of a signature.
 * returns 0 on success, otherwise failure.
 */
int DecodePrivateKey(WOLFSSL *ssl, word16* length)
{
    int      ret;
    int      keySz;
    word32   idx;

    /* make sure private key exists */
    if (ssl->buffers.key == NULL || ssl->buffers.key->buffer == NULL) {
        WOLFSSL_MSG("Private key missing!");
        ERROR_OUT(NO_PRIVATE_KEY, exit_dpk);
    }

#ifndef NO_RSA
    ssl->hsType = DYNAMIC_TYPE_RSA;
    ret = AllocKey(ssl, ssl->hsType, &ssl->hsKey);
    if (ret != 0) {
        goto exit_dpk;
    }

    WOLFSSL_MSG("Trying RSA private key");

    /* Set start of data to beginning of buffer. */
    idx = 0;
    /* Decode the key assuming it is an RSA private key. */
    ret = wc_RsaPrivateKeyDecode(ssl->buffers.key->buffer, &idx,
                (RsaKey*)ssl->hsKey, ssl->buffers.key->length);
    if (ret == 0) {
        WOLFSSL_MSG("Using RSA private key");

        /* It worked so check it meets minimum key size requirements. */
        keySz = wc_RsaEncryptSize((RsaKey*)ssl->hsKey);
        if (keySz < 0) { /* check if keySz has error case */
            ERROR_OUT(keySz, exit_dpk);
        }

        if (keySz < ssl->options.minRsaKeySz) {
            WOLFSSL_MSG("RSA key size too small");
            ERROR_OUT(RSA_KEY_SIZE_E, exit_dpk);
        }

        /* Return the maximum signature length. */
        *length = (word16)keySz;

        goto exit_dpk;
    }
#endif /* !NO_RSA */

#ifdef HAVE_ECC
#ifndef NO_RSA
    FreeKey(ssl, ssl->hsType, (void**)&ssl->hsKey);
#endif /* !NO_RSA */

    ssl->hsType = DYNAMIC_TYPE_ECC;
    ret = AllocKey(ssl, ssl->hsType, &ssl->hsKey);
    if (ret != 0) {
        goto exit_dpk;
    }

#ifndef NO_RSA
    WOLFSSL_MSG("Trying ECC private key, RSA didn't work");
#else
    WOLFSSL_MSG("Trying ECC private key");
#endif

    /* Set start of data to beginning of buffer. */
    idx = 0;
    /* Decode the key assuming it is an ECC private key. */
    ret = wc_EccPrivateKeyDecode(ssl->buffers.key->buffer, &idx,
                                 (ecc_key*)ssl->hsKey,
                                 ssl->buffers.key->length);
    if (ret == 0) {
        WOLFSSL_MSG("Using ECC private key");

        /* Check it meets the minimum ECC key size requirements. */
        keySz = wc_ecc_size((ecc_key*)ssl->hsKey);
        if (keySz < ssl->options.minEccKeySz) {
            WOLFSSL_MSG("ECC key size too small");
            ERROR_OUT(ECC_KEY_SIZE_E, exit_dpk);
        }

        /* Return the maximum signature length. */
        *length = (word16)wc_ecc_sig_size((ecc_key*)ssl->hsKey);

        goto exit_dpk;
    }
#endif
#ifdef HAVE_ED25519
#if !defined(NO_RSA) || defined(HAVE_ECC)
    FreeKey(ssl, ssl->hsType, (void**)&ssl->hsKey);
#endif

    ssl->hsType = DYNAMIC_TYPE_ED25519;
    ret = AllocKey(ssl, ssl->hsType, &ssl->hsKey);
    if (ret != 0) {
        goto exit_dpk;
    }

#ifdef HAVE_ECC
    WOLFSSL_MSG("Trying ED25519 private key, ECC didn't work");
#elif !defined(NO_RSA)
    WOLFSSL_MSG("Trying ED25519 private key, RSA didn't work");
#else
    WOLFSSL_MSG("Trying ED25519 private key");
#endif

    /* Set start of data to beginning of buffer. */
    idx = 0;
    /* Decode the key assuming it is an ED25519 private key. */
    ret = wc_Ed25519PrivateKeyDecode(ssl->buffers.key->buffer, &idx,
                                     (ed25519_key*)ssl->hsKey,
                                     ssl->buffers.key->length);
    if (ret == 0) {
        WOLFSSL_MSG("Using ED25519 private key");

        /* Check it meets the minimum ECC key size requirements. */
        if (ED25519_KEY_SIZE < ssl->options.minEccKeySz) {
            WOLFSSL_MSG("ED25519 key size too small");
            ERROR_OUT(ECC_KEY_SIZE_E, exit_dpk);
        }

        /* Return the maximum signature length. */
        *length = ED25519_SIG_SIZE;

        goto exit_dpk;
    }
#endif

exit_dpk:
    return ret;
}


typedef struct ScvArgs {
    byte*  output; /* not allocated */
#ifndef NO_RSA
    byte*  verifySig;
#endif
    byte*  verify; /* not allocated */
    byte*  input;
    word32 idx;
    word32 extraSz;
    word32 sigSz;
    int    sendSz;
    int    inputSz;
    word16 length;
    byte   sigAlgo;
} ScvArgs;

static void FreeScvArgs(WOLFSSL* ssl, void* pArgs)
{
    ScvArgs* args = (ScvArgs*)pArgs;

    (void)ssl;

#ifndef NO_RSA
    if (args->verifySig) {
        XFREE(args->verifySig, ssl->heap, DYNAMIC_TYPE_SIGNATURE);
        args->verifySig = NULL;
    }
#endif
    if (args->input) {
        XFREE(args->input, ssl->heap, DYNAMIC_TYPE_IN_BUFFER);
        args->input = NULL;
    }
}

int SendCertificateVerify(WOLFSSL* ssl)
{
    int ret = 0;
#ifdef WOLFSSL_ASYNC_CRYPT
    ScvArgs* args = (ScvArgs*)ssl->async.args;
    typedef char args_test[sizeof(ssl->async.args) >= sizeof(*args) ? 1 : -1];
    (void)sizeof(args_test);
#else
    ScvArgs  args[1];
#endif

    WOLFSSL_ENTER("SendCertificateVerify");

#ifdef WOLFSSL_ASYNC_CRYPT
    ret = wolfSSL_AsyncPop(ssl, &ssl->options.asyncState);
    if (ret != WC_NOT_PENDING_E) {
        /* Check for error */
        if (ret < 0)
            goto exit_scv;
    }
    else
#endif
    {
        /* Reset state */
        ret = 0;
        ssl->options.asyncState = TLS_ASYNC_BEGIN;
        XMEMSET(args, 0, sizeof(ScvArgs));
    #ifdef WOLFSSL_ASYNC_CRYPT
        ssl->async.freeArgs = FreeScvArgs;
    #endif
    }

    switch(ssl->options.asyncState)
    {
        case TLS_ASYNC_BEGIN:
        {
            if (ssl->options.sendVerify == SEND_BLANK_CERT) {
                return 0;  /* sent blank cert, can't verify */
            }

            args->sendSz = MAX_CERT_VERIFY_SZ;
            if (IsEncryptionOn(ssl, 1)) {
                args->sendSz += MAX_MSG_EXTRA;
            }

            /* check for available size */
            if ((ret = CheckAvailableSize(ssl, args->sendSz)) != 0) {
                goto exit_scv;
            }

            /* get output buffer */
            args->output = ssl->buffers.outputBuffer.buffer +
                           ssl->buffers.outputBuffer.length;

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_BUILD;
        } /* case TLS_ASYNC_BEGIN */
        FALL_THROUGH;

        case TLS_ASYNC_BUILD:
        {
            ret = BuildCertHashes(ssl, &ssl->hsHashes->certHashes);
            if (ret != 0) {
                goto exit_scv;
            }

            /* Decode private key. */
            ret = DecodePrivateKey(ssl, &args->length);
            if (ret != 0) {
                goto exit_scv;
            }

            /* idx is used to track verify pointer offset to output */
            args->idx = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;
            args->verify = &args->output[RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ];
            args->extraSz = 0;  /* tls 1.2 hash/sig */

            /* build encoded signature buffer */
            ssl->buffers.sig.length = MAX_ENCODED_SIG_SZ;
            ssl->buffers.sig.buffer = (byte*)XMALLOC(ssl->buffers.sig.length,
                                        ssl->heap, DYNAMIC_TYPE_SIGNATURE);
            if (ssl->buffers.sig.buffer == NULL) {
                ERROR_OUT(MEMORY_E, exit_scv);
            }

        #ifdef WOLFSSL_DTLS
            if (ssl->options.dtls) {
                args->idx += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
                args->verify += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
            }
        #endif

    #ifndef NO_OLD_TLS
        #ifndef NO_SHA
            /* old tls default */
            SetDigest(ssl, sha_mac);
        #endif
    #else
        #ifndef NO_SHA256
            /* new tls default */
            SetDigest(ssl, sha256_mac);
        #endif
    #endif /* !NO_OLD_TLS */

            if (ssl->hsType == DYNAMIC_TYPE_RSA) {
        #ifdef WC_RSA_PSS
                if (IsAtLeastTLSv1_2(ssl) &&
                                (ssl->pssAlgo & (1 << ssl->suites->hashAlgo))) {
                    args->sigAlgo = rsa_pss_sa_algo;
                }
                else
        #endif
                    args->sigAlgo = rsa_sa_algo;
            }
            else if (ssl->hsType == DYNAMIC_TYPE_ECC)
                args->sigAlgo = ecc_dsa_sa_algo;
            else if (ssl->hsType == DYNAMIC_TYPE_ED25519)
                args->sigAlgo = ed25519_sa_algo;

            if (IsAtLeastTLSv1_2(ssl)) {
                EncodeSigAlg(ssl->suites->hashAlgo, args->sigAlgo,
                             args->verify);
                args->extraSz = HASH_SIG_SIZE;
                SetDigest(ssl, ssl->suites->hashAlgo);
            }
        #ifndef NO_OLD_TLS
            else {
                /* if old TLS load MD5 and SHA hash as value to sign */
                XMEMCPY(ssl->buffers.sig.buffer,
                    (byte*)ssl->hsHashes->certHashes.md5, FINISHED_SZ);
            }
        #endif

        #ifndef NO_RSA
            if (args->sigAlgo == rsa_sa_algo) {
                ssl->buffers.sig.length = FINISHED_SZ;
                args->sigSz = ENCRYPT_LEN;

                if (IsAtLeastTLSv1_2(ssl)) {
                    ssl->buffers.sig.length = wc_EncodeSignature(
                            ssl->buffers.sig.buffer, ssl->buffers.digest.buffer,
                            ssl->buffers.digest.length,
                            TypeHash(ssl->suites->hashAlgo));
                }

                /* prepend hdr */
                c16toa(args->length, args->verify + args->extraSz);
            }
            else if (args->sigAlgo == rsa_pss_sa_algo) {
                XMEMCPY(ssl->buffers.sig.buffer, ssl->buffers.digest.buffer,
                        ssl->buffers.digest.length);
                ssl->buffers.sig.length = ssl->buffers.digest.length;
                args->sigSz = ENCRYPT_LEN;

                /* prepend hdr */
                c16toa(args->length, args->verify + args->extraSz);
            }
        #endif /* !NO_RSA */

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_DO;
        } /* case TLS_ASYNC_BUILD */
        FALL_THROUGH;

        case TLS_ASYNC_DO:
        {
        #ifdef HAVE_ECC
           if (ssl->hsType == DYNAMIC_TYPE_ECC) {
                ecc_key* key = (ecc_key*)ssl->hsKey;

                ret = EccSign(ssl,
                    ssl->buffers.digest.buffer, ssl->buffers.digest.length,
                    ssl->buffers.sig.buffer, &ssl->buffers.sig.length,
                    key,
            #if defined(HAVE_PK_CALLBACKS)
                    ssl->buffers.key->buffer,
                    ssl->buffers.key->length,
                    ssl->EccSignCtx
            #else
                    NULL, 0, NULL
            #endif
                );
            }
        #endif /* HAVE_ECC */
        #ifdef HAVE_ED25519
           if (ssl->hsType == DYNAMIC_TYPE_ED25519) {
                ed25519_key* key = (ed25519_key*)ssl->hsKey;

                ret = Ed25519Sign(ssl,
                    ssl->buffers.digest.buffer, ssl->buffers.digest.length,
                    ssl->buffers.sig.buffer, &ssl->buffers.sig.length,
                    key,
            #if defined(HAVE_PK_CALLBACKS)
                    ssl->buffers.key->buffer,
                    ssl->buffers.key->length,
                    ssl->Ed25519SignCtx
            #else
                    NULL, 0, NULL
            #endif
                );
            }
        #endif /* HAVE_ECC */
        #ifndef NO_RSA
            if (ssl->hsType == DYNAMIC_TYPE_RSA) {
                RsaKey* key = (RsaKey*)ssl->hsKey;

                /* restore verify pointer */
                args->verify = &args->output[args->idx];

                ret = RsaSign(ssl,
                    ssl->buffers.sig.buffer, ssl->buffers.sig.length,
                    args->verify + args->extraSz + VERIFY_HEADER, &args->sigSz,
                    args->sigAlgo, ssl->suites->hashAlgo, key,
                    ssl->buffers.key->buffer, ssl->buffers.key->length,
                #ifdef HAVE_PK_CALLBACKS
                    ssl->RsaSignCtx
                #else
                    NULL
                #endif
                );
            }
        #endif /* !NO_RSA */

            /* Check for error */
            if (ret != 0) {
                goto exit_scv;
            }

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_VERIFY;
        } /* case TLS_ASYNC_DO */
        FALL_THROUGH;

        case TLS_ASYNC_VERIFY:
        {
            /* restore verify pointer */
            args->verify = &args->output[args->idx];

        #ifdef HAVE_ECC
            if (ssl->hsType == DYNAMIC_TYPE_ECC) {
                args->length = (word16)ssl->buffers.sig.length;
                /* prepend hdr */
                c16toa(args->length, args->verify + args->extraSz);
                XMEMCPY(args->verify + args->extraSz + VERIFY_HEADER,
                        ssl->buffers.sig.buffer, ssl->buffers.sig.length);
            }
        #endif /* HAVE_ECC */
        #ifdef HAVE_ECC
            if (ssl->hsType == DYNAMIC_TYPE_ED25519) {
                args->length = (word16)ssl->buffers.sig.length;
                /* prepend hdr */
                c16toa(args->length, args->verify + args->extraSz);
                XMEMCPY(args->verify + args->extraSz + VERIFY_HEADER,
                        ssl->buffers.sig.buffer, ssl->buffers.sig.length);
            }
        #endif /* HAVE_ECC */
        #ifndef NO_RSA
            if (ssl->hsType == DYNAMIC_TYPE_RSA) {
                RsaKey* key = (RsaKey*)ssl->hsKey;

                if (args->verifySig == NULL) {
                    args->verifySig = (byte*)XMALLOC(args->sigSz, ssl->heap,
                                      DYNAMIC_TYPE_SIGNATURE);
                    if (args->verifySig == NULL) {
                        ERROR_OUT(MEMORY_E, exit_scv);
                    }
                    XMEMCPY(args->verifySig, args->verify + args->extraSz +
                                                    VERIFY_HEADER, args->sigSz);
                }

                /* check for signature faults */
                ret = VerifyRsaSign(ssl,
                    args->verifySig, args->sigSz,
                    ssl->buffers.sig.buffer, ssl->buffers.sig.length,
                    args->sigAlgo, ssl->suites->hashAlgo, key
                );
            }
        #endif /* !NO_RSA */

            /* Check for error */
            if (ret != 0) {
                goto exit_scv;
            }

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_FINALIZE;
        } /* case TLS_ASYNC_VERIFY */
        FALL_THROUGH;

        case TLS_ASYNC_FINALIZE:
        {
            if (args->output == NULL) {
                ERROR_OUT(BUFFER_ERROR, exit_scv);
            }
            AddHeaders(args->output, (word32)args->length + args->extraSz +
                                        VERIFY_HEADER, certificate_verify, ssl);

            args->sendSz = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ +
                           (word32)args->length + args->extraSz + VERIFY_HEADER;

        #ifdef WOLFSSL_DTLS
            if (ssl->options.dtls) {
                args->sendSz += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
            }
        #endif

            if (IsEncryptionOn(ssl, 1)) {
                args->inputSz = args->sendSz - RECORD_HEADER_SZ;
                                /* build msg adds rec hdr */
                args->input = (byte*)XMALLOC(args->inputSz, ssl->heap,
                                                       DYNAMIC_TYPE_IN_BUFFER);
                if (args->input == NULL) {
                    ERROR_OUT(MEMORY_E, exit_scv);
                }

                XMEMCPY(args->input, args->output + RECORD_HEADER_SZ,
                                                                args->inputSz);
            }

            /* Advance state and proceed */
            ssl->options.asyncState = TLS_ASYNC_END;
        } /* case TLS_ASYNC_FINALIZE */
        FALL_THROUGH;

        case TLS_ASYNC_END:
        {
            if (IsEncryptionOn(ssl, 1)) {
                ret = BuildMessage(ssl, args->output,
                                      MAX_CERT_VERIFY_SZ + MAX_MSG_EXTRA,
                                      args->input, args->inputSz, handshake,
                                      1, 0, 1);
            #ifdef WOLFSSL_ASYNC_CRYPT
                if (ret == WC_PENDING_E)
                    goto exit_scv;
            #endif

                XFREE(args->input, ssl->heap, DYNAMIC_TYPE_IN_BUFFER);
                args->input = NULL;  /* make sure its not double free'd on cleanup */

                if (ret >= 0) {
                    args->sendSz = ret;
                    ret = 0;
                }
            }
            else {
            #ifdef WOLFSSL_DTLS
                if (ssl->options.dtls)
                    DtlsSEQIncrement(ssl, CUR_ORDER);
            #endif
                ret = HashOutput(ssl, args->output, args->sendSz, 0);
            }

            if (ret != 0) {
                goto exit_scv;
            }

        #ifdef WOLFSSL_DTLS
            if (IsDtlsNotSctpMode(ssl)) {
                ret = DtlsMsgPoolSave(ssl, args->output, args->sendSz);
            }
        #endif


        #ifdef WOLFSSL_CALLBACKS
            if (ssl->hsInfoOn)
                AddPacketName("CertificateVerify", &ssl->handShakeInfo);
            if (ssl->toInfoOn)
                AddPacketInfo("CertificateVerify", &ssl->timeoutInfo,
                              args->output, args->sendSz, ssl->heap);
        #endif

            ssl->buffers.outputBuffer.length += args->sendSz;

            if (!ssl->options.groupMessages) {
                ret = SendBuffered(ssl);
            }
            break;
        }
        default:
            ret = INPUT_CASE_ERROR;
    } /* switch(ssl->options.asyncState) */

exit_scv:

    WOLFSSL_LEAVE("SendCertificateVerify", ret);

#ifdef WOLFSSL_ASYNC_CRYPT
    /* Handle async operation */
    if (ret == WC_PENDING_E) {
        return ret;
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    /* Digest is not allocated, so do this to prevent free */
    ssl->buffers.digest.buffer = NULL;
    ssl->buffers.digest.length = 0;

    /* Final cleanup */
    FreeScvArgs(ssl, args);
    FreeKeyExchange(ssl);

    return ret;
}

#endif /* NO_CERTS */


#ifdef HAVE_SESSION_TICKET
int SetTicket(WOLFSSL* ssl, const byte* ticket, word32 length)
{
    /* Free old dynamic ticket if we already had one */
    if (ssl->session.isDynamic) {
        XFREE(ssl->session.ticket, ssl->heap, DYNAMIC_TYPE_SESSION_TICK);
        ssl->session.ticket = ssl->session.staticTicket;
        ssl->session.isDynamic = 0;
    }

    if (length > sizeof(ssl->session.staticTicket)) {
        byte* sessionTicket =
                   (byte*)XMALLOC(length, ssl->heap, DYNAMIC_TYPE_SESSION_TICK);
        if (sessionTicket == NULL)
            return MEMORY_E;
        ssl->session.ticket = sessionTicket;
        ssl->session.isDynamic = 1;
    }
    ssl->session.ticketLen = length;

    if (length > 0) {
        XMEMCPY(ssl->session.ticket, ticket, length);
        if (ssl->session_ticket_cb != NULL) {
            ssl->session_ticket_cb(ssl,
                                   ssl->session.ticket, ssl->session.ticketLen,
                                   ssl->session_ticket_ctx);
        }
        /* Create a fake sessionID based on the ticket, this will
         * supercede the existing session cache info. */
        ssl->options.haveSessionId = 1;
        XMEMCPY(ssl->arrays->sessionID,
                                 ssl->session.ticket + length - ID_LEN, ID_LEN);
    }

    return 0;
}

static int DoSessionTicket(WOLFSSL* ssl, const byte* input, word32* inOutIdx,
    word32 size)
{
    word32 begin = *inOutIdx;
    word32 lifetime;
    word16 length;
    int    ret;

    if (ssl->expect_session_ticket == 0) {
        WOLFSSL_MSG("Unexpected session ticket");
        return SESSION_TICKET_EXPECT_E;
    }

    if ((*inOutIdx - begin) + OPAQUE32_LEN > size)
        return BUFFER_ERROR;

    ato32(input + *inOutIdx, &lifetime);
    *inOutIdx += OPAQUE32_LEN;

    if ((*inOutIdx - begin) + OPAQUE16_LEN > size)
        return BUFFER_ERROR;

    ato16(input + *inOutIdx, &length);
    *inOutIdx += OPAQUE16_LEN;

    if ((*inOutIdx - begin) + length > size)
        return BUFFER_ERROR;

    if ((ret = SetTicket(ssl, input + *inOutIdx, length)) != 0)
        return ret;
    *inOutIdx += length;
    if (length > 0) {
        ssl->timeout = lifetime;
#ifndef NO_SESSION_CACHE
        AddSession(ssl);
#endif
    }

    if (IsEncryptionOn(ssl, 0)) {
        *inOutIdx += ssl->keys.padSz;
    }

    ssl->expect_session_ticket = 0;

    return 0;
}
#endif /* HAVE_SESSION_TICKET */

#endif /* NO_WOLFSSL_CLIENT */

#ifndef NO_WOLFSSL_SERVER

    int SendServerHello(WOLFSSL* ssl)
    {
        byte              *output;
        word32             length, idx = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;
        int                sendSz;
        int                ret;
        byte               sessIdSz = ID_LEN;
        byte               echoId   = 0;  /* ticket echo id flag */
        byte               cacheOff = 0;  /* session cache off flag */

        length = VERSION_SZ + RAN_LEN
               + ID_LEN + ENUM_LEN
               + SUITE_LEN
               + ENUM_LEN;

#ifdef HAVE_TLS_EXTENSIONS
        length += TLSX_GetResponseSize(ssl, server_hello);
    #ifdef HAVE_SESSION_TICKET
        if (ssl->options.useTicket) {
            /* echo session id sz can be 0,32 or bogus len inbetween */
            sessIdSz = ssl->arrays->sessionIDSz;
            if (sessIdSz > ID_LEN) {
                WOLFSSL_MSG("Bad bogus session id len");
                return BUFFER_ERROR;
            }
            if (!IsAtLeastTLSv1_3(ssl->version))
                length -= (ID_LEN - sessIdSz);  /* adjust ID_LEN assumption */
            echoId = 1;
        }
    #endif /* HAVE_SESSION_TICKET */
#else
        if (ssl->options.haveEMS) {
            length += HELLO_EXT_SZ_SZ + HELLO_EXT_SZ;
        }
#endif

        /* is the session cahce off at build or runtime */
#ifdef NO_SESSION_CACHE
        cacheOff = 1;
#else
        if (ssl->options.sessionCacheOff == 1) {
            cacheOff = 1;
        }
#endif

        /* if no session cache don't send a session ID unless we're echoing
         * an ID as part of session tickets */
        if (echoId == 0 && cacheOff == 1) {
            length -= ID_LEN;    /* adjust ID_LEN assumption */
            sessIdSz = 0;
        }

        /* check for avalaible size */
        if ((ret = CheckAvailableSize(ssl, MAX_HELLO_SZ)) != 0)
            return ret;

        /* get output buffer */
        output = ssl->buffers.outputBuffer.buffer +
                 ssl->buffers.outputBuffer.length;

        sendSz = length + HANDSHAKE_HEADER_SZ + RECORD_HEADER_SZ;
        #ifdef WOLFSSL_DTLS
        if (ssl->options.dtls) {
            /* Server Hello should use the same sequence number as the
             * Client Hello. */
            ssl->keys.dtls_sequence_number_hi = ssl->keys.curSeq_hi;
            ssl->keys.dtls_sequence_number_lo = ssl->keys.curSeq_lo;
            idx    += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
            sendSz += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
        }
        #endif /* WOLFSSL_DTLS */
        AddHeaders(output, length, server_hello, ssl);

        /* now write to output */
        /* first version */
        output[idx++] = ssl->version.major;
        output[idx++] = ssl->version.minor;

        /* then random and session id */
        if (!ssl->options.resuming) {
            /* generate random part and session id */
            ret = wc_RNG_GenerateBlock(ssl->rng, output + idx,
                RAN_LEN + sizeof(sessIdSz) + sessIdSz);
            if (ret != 0)
                return ret;

#ifdef WOLFSSL_TLS13
            if (IsAtLeastTLSv1_3(ssl->ctx->method->version)) {
                /* TLS v1.3 capable server downgraded. */
                XMEMCPY(output + idx + RAN_LEN - (TLS13_DOWNGRADE_SZ + 1),
                        tls13Downgrade, TLS13_DOWNGRADE_SZ);
                output[idx + RAN_LEN - 1] = IsAtLeastTLSv1_2(ssl);
            }
            else
#endif
            if (ssl->ctx->method->version.major == SSLv3_MAJOR &&
                          ssl->ctx->method->version.minor == TLSv1_2_MINOR &&
                                                       !IsAtLeastTLSv1_2(ssl)) {
                /* TLS v1.2 capable server downgraded. */
                XMEMCPY(output + idx + RAN_LEN - (TLS13_DOWNGRADE_SZ + 1),
                        tls13Downgrade, TLS13_DOWNGRADE_SZ);
                output[idx + RAN_LEN - 1] = 0;
            }

            /* store info in SSL for later */
            XMEMCPY(ssl->arrays->serverRandom, output + idx, RAN_LEN);
            idx += RAN_LEN;
            output[idx++] = sessIdSz;
            XMEMCPY(ssl->arrays->sessionID, output + idx, sessIdSz);
            ssl->arrays->sessionIDSz = sessIdSz;
        }
        else {
            /* If resuming, use info from SSL */
            XMEMCPY(output + idx, ssl->arrays->serverRandom, RAN_LEN);
            idx += RAN_LEN;
            output[idx++] = sessIdSz;
            XMEMCPY(output + idx, ssl->arrays->sessionID, sessIdSz);
        }
        idx += sessIdSz;

#ifdef SHOW_SECRETS
        {
            int j;
            printf("server random: ");
            for (j = 0; j < RAN_LEN; j++)
                printf("%02x", ssl->arrays->serverRandom[j]);
            printf("\n");
        }
#endif

        /* then cipher suite */
        output[idx++] = ssl->options.cipherSuite0;
        output[idx++] = ssl->options.cipherSuite;

        /* then compression */
        if (ssl->options.usingCompression)
            output[idx++] = ZLIB_COMPRESSION;
        else
            output[idx++] = NO_COMPRESSION;

        /* last, extensions */
#ifdef HAVE_TLS_EXTENSIONS
        TLSX_WriteResponse(ssl, output + idx, server_hello);
#else
#ifdef HAVE_EXTENDED_MASTER
        if (ssl->options.haveEMS) {
            c16toa(HELLO_EXT_SZ, output + idx);
            idx += HELLO_EXT_SZ_SZ;

            c16toa(HELLO_EXT_EXTMS, output + idx);
            idx += HELLO_EXT_TYPE_SZ;
            c16toa(0, output + idx);
            /*idx += HELLO_EXT_SZ_SZ;*/
            /* idx is not used after this point. uncomment the line above
             * if adding any more extentions in the future. */
        }
#endif
#endif

        ssl->buffers.outputBuffer.length += sendSz;
        #ifdef WOLFSSL_DTLS
            if (IsDtlsNotSctpMode(ssl)) {
                if ((ret = DtlsMsgPoolSave(ssl, output, sendSz)) != 0)
                    return ret;
            }

            if (ssl->options.dtls) {
                DtlsSEQIncrement(ssl, CUR_ORDER);
            }
        #endif

        ret = HashOutput(ssl, output, sendSz, 0);
        if (ret != 0)
            return ret;


    #ifdef WOLFSSL_CALLBACKS
        if (ssl->hsInfoOn)
            AddPacketName("ServerHello", &ssl->handShakeInfo);
        if (ssl->toInfoOn)
            AddPacketInfo("ServerHello", &ssl->timeoutInfo, output, sendSz,
                          ssl->heap);
    #endif

        ssl->options.serverState = SERVER_HELLO_COMPLETE;

        if (ssl->options.groupMessages)
            return 0;
        else
            return SendBuffered(ssl);
    }


#ifdef HAVE_ECC

    static byte SetCurveId(ecc_key* key)
    {
        if (key == NULL || key->dp == NULL) {
            WOLFSSL_MSG("SetCurveId: Invalid key!");
            return 0;
        }

        switch(key->dp->oidSum) {
        #if defined(HAVE_ECC160) || defined(HAVE_ALL_CURVES)
        #ifndef NO_ECC_SECP
            case ECC_SECP160R1_OID:
                return WOLFSSL_ECC_SECP160R1;
        #endif /* !NO_ECC_SECP */
        #ifdef HAVE_ECC_SECPR2
            case ECC_SECP160R2_OID:
                return WOLFSSL_ECC_SECP160R2;
        #endif /* HAVE_ECC_SECPR2 */
        #ifdef HAVE_ECC_KOBLITZ
            case ECC_SECP160K1_OID:
                return WOLFSSL_ECC_SECP160K1;
        #endif /* HAVE_ECC_KOBLITZ */
    #endif
    #if defined(HAVE_ECC192) || defined(HAVE_ALL_CURVES)
        #ifndef NO_ECC_SECP
            case ECC_SECP192R1_OID:
                return WOLFSSL_ECC_SECP192R1;
        #endif /* !NO_ECC_SECP */
        #ifdef HAVE_ECC_KOBLITZ
            case ECC_SECP192K1_OID:
                return WOLFSSL_ECC_SECP192K1;
        #endif /* HAVE_ECC_KOBLITZ */
    #endif
    #if defined(HAVE_ECC224) || defined(HAVE_ALL_CURVES)
        #ifndef NO_ECC_SECP
            case ECC_SECP224R1_OID:
                return WOLFSSL_ECC_SECP224R1;
        #endif /* !NO_ECC_SECP */
        #ifdef HAVE_ECC_KOBLITZ
            case ECC_SECP224K1_OID:
                return WOLFSSL_ECC_SECP224K1;
        #endif /* HAVE_ECC_KOBLITZ */
    #endif
    #if !defined(NO_ECC256)  || defined(HAVE_ALL_CURVES)
        #ifndef NO_ECC_SECP
            case ECC_SECP256R1_OID:
                return WOLFSSL_ECC_SECP256R1;
        #endif /* !NO_ECC_SECP */
        #ifdef HAVE_ECC_KOBLITZ
            case ECC_SECP256K1_OID:
                return WOLFSSL_ECC_SECP256K1;
        #endif /* HAVE_ECC_KOBLITZ */
        #ifdef HAVE_ECC_BRAINPOOL
            case ECC_BRAINPOOLP256R1_OID:
                return WOLFSSL_ECC_BRAINPOOLP256R1;
        #endif /* HAVE_ECC_BRAINPOOL */
    #endif
    #if defined(HAVE_ECC384) || defined(HAVE_ALL_CURVES)
        #ifdef HAVE_CURVE25519
            case ECC_X25519_OID:
                return WOLFSSL_ECC_X25519;
        #endif
        #ifndef NO_ECC_SECP
            case ECC_SECP384R1_OID:
                return WOLFSSL_ECC_SECP384R1;
        #endif /* !NO_ECC_SECP */
        #ifdef HAVE_ECC_BRAINPOOL
            case ECC_BRAINPOOLP384R1_OID:
                return WOLFSSL_ECC_BRAINPOOLP384R1;
        #endif /* HAVE_ECC_BRAINPOOL */
    #endif
    #if defined(HAVE_ECC512) || defined(HAVE_ALL_CURVES)
        #ifdef HAVE_ECC_BRAINPOOL
            case ECC_BRAINPOOLP512R1_OID:
                return WOLFSSL_ECC_BRAINPOOLP512R1;
        #endif /* HAVE_ECC_BRAINPOOL */
    #endif
    #if defined(HAVE_ECC521) || defined(HAVE_ALL_CURVES)
        #ifndef NO_ECC_SECP
            case ECC_SECP521R1_OID:
                return WOLFSSL_ECC_SECP521R1;
        #endif /* !NO_ECC_SECP */
    #endif
            default:
                return 0;
        }
    }

#endif /* HAVE_ECC */

    typedef struct SskeArgs {
        byte*  output; /* not allocated */
    #if defined(HAVE_ECC) || (!defined(NO_DH) && !defined(NO_RSA))
        byte*  sigDataBuf;
    #endif
    #if defined(HAVE_ECC)
        byte*  exportBuf;
    #endif
    #ifndef NO_RSA
        byte*  verifySig;
    #endif
        word32 idx;
        word32 tmpSigSz;
        word32 length;
        word32 sigSz;
    #if defined(HAVE_ECC) || (!defined(NO_DH) && !defined(NO_RSA))
        word32 sigDataSz;
    #endif
    #if defined(HAVE_ECC)
        word32 exportSz;
    #endif
    #ifdef HAVE_QSH
        word32 qshSz;
    #endif
        int    sendSz;
    } SskeArgs;

    static void FreeSskeArgs(WOLFSSL* ssl, void* pArgs)
    {
        SskeArgs* args = (SskeArgs*)pArgs;

        (void)ssl;

    #if defined(HAVE_ECC)
        if (args->exportBuf) {
            XFREE(args->exportBuf, ssl->heap, DYNAMIC_TYPE_DER);
            args->exportBuf = NULL;
        }
    #endif
    #if defined(HAVE_ECC) || (!defined(NO_DH) && !defined(NO_RSA))
        if (args->sigDataBuf) {
            XFREE(args->sigDataBuf, ssl->heap, DYNAMIC_TYPE_SIGNATURE);
            args->sigDataBuf = NULL;
        }
    #endif
    #ifndef NO_RSA
        if (args->verifySig) {
            XFREE(args->verifySig, ssl->heap, DYNAMIC_TYPE_SIGNATURE);
            args->verifySig = NULL;
        }
    #endif
        (void)args;
    }

    int SendServerKeyExchange(WOLFSSL* ssl)
    {
        int ret;
    #ifdef WOLFSSL_ASYNC_CRYPT
        SskeArgs* args = (SskeArgs*)ssl->async.args;
        typedef char args_test[sizeof(ssl->async.args) >= sizeof(*args) ? 1 : -1];
        (void)sizeof(args_test);
    #else
        SskeArgs  args[1];
    #endif

        WOLFSSL_ENTER("SendServerKeyExchange");

    #ifdef WOLFSSL_ASYNC_CRYPT
        ret = wolfSSL_AsyncPop(ssl, &ssl->options.asyncState);
        if (ret != WC_NOT_PENDING_E) {
            /* Check for error */
            if (ret < 0)
                goto exit_sske;
        }
        else
    #endif
        {
            /* Reset state */
            ret = 0;
            ssl->options.asyncState = TLS_ASYNC_BEGIN;
            XMEMSET(args, 0, sizeof(SskeArgs));
        #ifdef WOLFSSL_ASYNC_CRYPT
            ssl->async.freeArgs = FreeSskeArgs;
        #endif
        }

        switch(ssl->options.asyncState)
        {
            case TLS_ASYNC_BEGIN:
            {
            #ifdef HAVE_QSH
                if (ssl->peerQSHKeyPresent && ssl->options.haveQSH) {
                    args->qshSz = QSH_KeyGetSize(ssl);
                }
            #endif

                /* Do some checks / debug msgs */
                switch(ssl->specs.kea)
                {
                #if defined(HAVE_ECC) && !defined(NO_PSK)
                    case ecdhe_psk_kea:
                    {
                        WOLFSSL_MSG("Using ephemeral ECDH PSK");
                        break;
                    }
                #endif /* HAVE_ECC && !NO_PSK */
                #ifdef HAVE_ECC
                    case ecc_diffie_hellman_kea:
                    {
                        if (ssl->specs.static_ecdh) {
                            WOLFSSL_MSG("Using Static ECDH, not sending ServerKeyExchange");
                            ERROR_OUT(0, exit_sske);
                        }

                        /* make sure private key exists */
                        if (ssl->buffers.key == NULL ||
                                            ssl->buffers.key->buffer == NULL) {
                            ERROR_OUT(NO_PRIVATE_KEY, exit_sske);
                        }

                        WOLFSSL_MSG("Using ephemeral ECDH");
                        break;
                    }
                #endif /* HAVE_ECC */
                }

                /* Preparing keys */
                switch(ssl->specs.kea)
                {
                #ifndef NO_PSK
                    case psk_kea:
                    {
                        /* Nothing to do in this sub-state */
                        break;
                    }
                #endif /* !NO_PSK */
                #if !defined(NO_DH) && (!defined(NO_PSK) || !defined(NO_RSA))
                #if !defined(NO_PSK)
                    case dhe_psk_kea:
                #endif
                #if !defined(NO_RSA)
                    case diffie_hellman_kea:
                #endif
                    {
                        /* Allocate DH key buffers and generate key */
                        if (ssl->buffers.serverDH_P.buffer == NULL ||
                            ssl->buffers.serverDH_G.buffer == NULL) {
                            ERROR_OUT(NO_DH_PARAMS, exit_sske);
                        }

                        if (ssl->buffers.serverDH_Pub.buffer == NULL) {
                            /* Free'd in SSL_ResourceFree and FreeHandshakeResources */
                            ssl->buffers.serverDH_Pub.buffer = (byte*)XMALLOC(
                                    ssl->buffers.serverDH_P.length + OPAQUE16_LEN,
                                    ssl->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                            if (ssl->buffers.serverDH_Pub.buffer == NULL) {
                                ERROR_OUT(MEMORY_E, exit_sske);
                            }
                        }

                        if (ssl->buffers.serverDH_Priv.buffer == NULL) {
                            /* Free'd in SSL_ResourceFree and FreeHandshakeResources */
                            ssl->buffers.serverDH_Priv.buffer = (byte*)XMALLOC(
                                    ssl->buffers.serverDH_P.length + OPAQUE16_LEN,
                                    ssl->heap, DYNAMIC_TYPE_PRIVATE_KEY);
                            if (ssl->buffers.serverDH_Priv.buffer == NULL) {
                                ERROR_OUT(MEMORY_E, exit_sske);
                            }
                        }

                        ssl->options.dhKeySz =
                                (word16)ssl->buffers.serverDH_P.length;

                        ret = AllocKey(ssl, DYNAMIC_TYPE_DH,
                                            (void**)&ssl->buffers.serverDH_Key);
                        if (ret != 0) {
                            goto exit_sske;
                        }

                        ret = wc_DhSetKey(ssl->buffers.serverDH_Key,
                            ssl->buffers.serverDH_P.buffer,
                            ssl->buffers.serverDH_P.length,
                            ssl->buffers.serverDH_G.buffer,
                            ssl->buffers.serverDH_G.length);
                        if (ret != 0) {
                            goto exit_sske;
                        }

                        ret = DhGenKeyPair(ssl, ssl->buffers.serverDH_Key,
                            ssl->buffers.serverDH_Priv.buffer,
                            &ssl->buffers.serverDH_Priv.length,
                            ssl->buffers.serverDH_Pub.buffer,
                            &ssl->buffers.serverDH_Pub.length);
                        break;
                    }
                #endif /* !NO_DH && (!NO_PSK || !NO_RSA) */
                #if defined(HAVE_ECC) && !defined(NO_PSK)
                    case ecdhe_psk_kea:
                        /* Fall through to create temp ECC key */
                #endif /* HAVE_ECC && !NO_PSK */
                #if defined(HAVE_ECC) || defined(HAVE_CURVE25519)
                    case ecc_diffie_hellman_kea:
                    {
                    #ifdef HAVE_CURVE25519
                        if (ssl->ecdhCurveOID == ECC_X25519_OID) {
                            /* need ephemeral key now, create it if missing */
                            if (ssl->eccTempKey == NULL) {
                                /* alloc/init on demand */
                                ret = AllocKey(ssl, DYNAMIC_TYPE_CURVE25519,
                                    (void**)&ssl->eccTempKey);
                                if (ret != 0) {
                                    goto exit_sske;
                                }
                            }

                            if (ssl->eccTempKeyPresent == 0) {
                                ret = X25519MakeKey(ssl,
                                        (curve25519_key*)ssl->eccTempKey, NULL);
                                if (ret == 0 || ret == WC_PENDING_E) {
                                    ssl->eccTempKeyPresent =
                                        DYNAMIC_TYPE_CURVE25519;
                                }
                            }
                            break;
                        }
                    #endif
                    #ifdef HAVE_ECC
                        /* need ephemeral key now, create it if missing */
                        if (ssl->eccTempKey == NULL) {
                            /* alloc/init on demand */
                            ret = AllocKey(ssl, DYNAMIC_TYPE_ECC,
                                (void**)&ssl->eccTempKey);
                            if (ret != 0) {
                                goto exit_sske;
                            }
                        }

                        if (ssl->eccTempKeyPresent == 0) {
                            /* TODO: Need to first do wc_EccPrivateKeyDecode,
                                then we know curve dp */
                            ret = EccMakeKey(ssl, ssl->eccTempKey, NULL);
                            if (ret == 0 || ret == WC_PENDING_E) {
                                ssl->eccTempKeyPresent = DYNAMIC_TYPE_ECC;
                            }
                        }
                    #endif
                        break;
                    }
                #endif /* HAVE_ECC || HAVE_CURVE25519 */
                    default:
                        /* Skip ServerKeyExchange */
                        goto exit_sske;
                } /* switch(ssl->specs.kea) */

                /* Check for error */
                if (ret != 0) {
                    goto exit_sske;
                }

                /* Advance state and proceed */
                ssl->options.asyncState = TLS_ASYNC_BUILD;
            } /* case TLS_ASYNC_BEGIN */
            FALL_THROUGH;

            case TLS_ASYNC_BUILD:
            {
            #if (!defined(NO_DH) && !defined(NO_RSA)) || defined(HAVE_ECC)
                word32 preSigSz, preSigIdx;
            #endif

                switch(ssl->specs.kea)
                {
                #ifndef NO_PSK
                    case psk_kea:
                    {
                        args->idx = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;

                        if (ssl->arrays->server_hint[0] == 0) {
                            ERROR_OUT(0, exit_sske); /* don't send */
                        }

                        /* include size part */
                        args->length = (word32)XSTRLEN(ssl->arrays->server_hint);
                        if (args->length > MAX_PSK_ID_LEN) {
                            ERROR_OUT(SERVER_HINT_ERROR, exit_sske);
                        }

                        args->length += HINT_LEN_SZ;
                        args->sendSz = args->length + HANDSHAKE_HEADER_SZ +
                                                            RECORD_HEADER_SZ;

                    #ifdef HAVE_QSH
                        args->length += args->qshSz;
                        args->sendSz += args->qshSz;
                    #endif

                    #ifdef WOLFSSL_DTLS
                        if (ssl->options.dtls) {
                            args->sendSz += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
                            args->idx    += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
                        }
                    #endif
                        /* check for available size */
                        if ((ret = CheckAvailableSize(ssl, args->sendSz)) != 0) {
                            goto exit_sske;
                        }

                        /* get ouput buffer */
                        args->output = ssl->buffers.outputBuffer.buffer +
                                       ssl->buffers.outputBuffer.length;

                        AddHeaders(args->output, args->length,
                                                    server_key_exchange, ssl);

                        /* key data */
                    #ifdef HAVE_QSH
                        c16toa((word16)(args->length - args->qshSz -
                                        HINT_LEN_SZ), args->output + args->idx);
                    #else
                        c16toa((word16)(args->length - HINT_LEN_SZ),
                                                      args->output + args->idx);
                    #endif

                        args->idx += HINT_LEN_SZ;
                        XMEMCPY(args->output + args->idx,
                                ssl->arrays->server_hint,
                                args->length - HINT_LEN_SZ);
                        break;
                    }
                #endif /* !NO_PSK */
                #if !defined(NO_DH) && !defined(NO_PSK)
                    case dhe_psk_kea:
                    {
                        word32 hintLen;

                        args->idx = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;
                        args->length = LENGTH_SZ * 3 + /* p, g, pub */
                                 ssl->buffers.serverDH_P.length +
                                 ssl->buffers.serverDH_G.length +
                                 ssl->buffers.serverDH_Pub.length;

                        /* include size part */
                        hintLen = (word32)XSTRLEN(ssl->arrays->server_hint);
                        if (hintLen > MAX_PSK_ID_LEN) {
                            ERROR_OUT(SERVER_HINT_ERROR, exit_sske);
                        }
                        args->length += hintLen + HINT_LEN_SZ;
                        args->sendSz = args->length + HANDSHAKE_HEADER_SZ +
                                                            RECORD_HEADER_SZ;

                    #ifdef HAVE_QSH
                        args->length += args->qshSz;
                        args->sendSz += args->qshSz;
                    #endif
                    #ifdef WOLFSSL_DTLS
                        if (ssl->options.dtls) {
                            args->sendSz += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
                            args->idx    += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
                        }
                    #endif

                        /* check for available size */
                        if ((ret = CheckAvailableSize(ssl, args->sendSz)) != 0) {
                            goto exit_sske;
                        }

                        /* get ouput buffer */
                        args->output = ssl->buffers.outputBuffer.buffer +
                                       ssl->buffers.outputBuffer.length;

                        AddHeaders(args->output, args->length,
                                                    server_key_exchange, ssl);

                        /* key data */
                        c16toa((word16)hintLen, args->output + args->idx);
                        args->idx += HINT_LEN_SZ;
                        XMEMCPY(args->output + args->idx,
                                            ssl->arrays->server_hint, hintLen);
                        args->idx += hintLen;

                        /* add p, g, pub */
                        c16toa((word16)ssl->buffers.serverDH_P.length,
                            args->output + args->idx);
                        args->idx += LENGTH_SZ;
                        XMEMCPY(args->output + args->idx,
                                ssl->buffers.serverDH_P.buffer,
                                ssl->buffers.serverDH_P.length);
                        args->idx += ssl->buffers.serverDH_P.length;

                        /*  g */
                        c16toa((word16)ssl->buffers.serverDH_G.length,
                            args->output + args->idx);
                        args->idx += LENGTH_SZ;
                        XMEMCPY(args->output + args->idx,
                                ssl->buffers.serverDH_G.buffer,
                                ssl->buffers.serverDH_G.length);
                        args->idx += ssl->buffers.serverDH_G.length;

                        /*  pub */
                        c16toa((word16)ssl->buffers.serverDH_Pub.length,
                            args->output + args->idx);
                        args->idx += LENGTH_SZ;
                        XMEMCPY(args->output + args->idx,
                                ssl->buffers.serverDH_Pub.buffer,
                                ssl->buffers.serverDH_Pub.length);
                        /* No need to update idx, since sizes are already set */
                        /* args->idx += ssl->buffers.serverDH_Pub.length; */
                        break;
                    }
                #endif /* !defined(NO_DH) && !defined(NO_PSK) */
                #if defined(HAVE_ECC) && !defined(NO_PSK)
                    case ecdhe_psk_kea:
                    {
                        word32 hintLen;

                        /* curve type, named curve, length(1) */
                        args->idx = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;
                        args->length = ENUM_LEN + CURVE_LEN + ENUM_LEN;

                        args->exportSz = MAX_EXPORT_ECC_SZ;
                        args->exportBuf = (byte*)XMALLOC(args->exportSz,
                                            ssl->heap, DYNAMIC_TYPE_DER);
                        if (args->exportBuf == NULL) {
                            ERROR_OUT(MEMORY_E, exit_sske);
                        }
                    #ifdef HAVE_CURVE25519
                        if (ssl->ecdhCurveOID == ECC_X25519_OID) {
                            if (wc_curve25519_export_public_ex(
                                    (curve25519_key*)ssl->eccTempKey,
                                    args->exportBuf, &args->exportSz,
                                    EC25519_LITTLE_ENDIAN) != 0) {
                                ERROR_OUT(ECC_EXPORT_ERROR, exit_sske);
                            }
                        }
                        else
                    #endif
                        {
                            if (wc_ecc_export_x963(ssl->eccTempKey,
                                       args->exportBuf, &args->exportSz) != 0) {
                                ERROR_OUT(ECC_EXPORT_ERROR, exit_sske);
                            }
                        }
                        args->length += args->exportSz;

                        /* include size part */
                        hintLen = (word32)XSTRLEN(ssl->arrays->server_hint);
                        if (hintLen > MAX_PSK_ID_LEN) {
                            ERROR_OUT(SERVER_HINT_ERROR, exit_sske);
                        }
                        args->length += hintLen + HINT_LEN_SZ;
                        args->sendSz = args->length + HANDSHAKE_HEADER_SZ + RECORD_HEADER_SZ;

                    #ifdef HAVE_QSH
                        args->length += args->qshSz;
                        args->sendSz += args->qshSz;
                    #endif
                    #ifdef WOLFSSL_DTLS
                        if (ssl->options.dtls) {
                            args->sendSz += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
                            args->idx    += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
                        }
                    #endif
                        /* check for available size */
                        if ((ret = CheckAvailableSize(ssl, args->sendSz)) != 0) {
                            goto exit_sske;
                        }

                        /* get output buffer */
                        args->output = ssl->buffers.outputBuffer.buffer +
                                       ssl->buffers.outputBuffer.length;

                        /* key data */
                        c16toa((word16)hintLen, args->output + args->idx);
                        args->idx += HINT_LEN_SZ;
                        XMEMCPY(args->output + args->idx,
                                            ssl->arrays->server_hint, hintLen);
                        args->idx += hintLen;

                        /* ECC key exchange data */
                        args->output[args->idx++] = named_curve;
                        args->output[args->idx++] = 0x00;          /* leading zero */
                    #ifdef HAVE_CURVE25519
                        if (ssl->ecdhCurveOID == ECC_X25519_OID)
                            args->output[args->idx++] = WOLFSSL_ECC_X25519;
                        else
                    #endif
                        {
                            args->output[args->idx++] =
                                                    SetCurveId(ssl->eccTempKey);
                        }
                        args->output[args->idx++] = (byte)args->exportSz;
                        XMEMCPY(args->output + args->idx, args->exportBuf,
                                                                args->exportSz);
                        break;
                    }
                #endif /* HAVE_ECC && !NO_PSK */
                #ifdef HAVE_ECC
                    case ecc_diffie_hellman_kea:
                    {
                        enum wc_HashType hashType;

                        /* curve type, named curve, length(1) */
                        args->idx = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;
                        args->length = ENUM_LEN + CURVE_LEN + ENUM_LEN;

                        /* Export temp ECC key and add to length */
                        args->exportSz = MAX_EXPORT_ECC_SZ;
                        args->exportBuf = (byte*)XMALLOC(args->exportSz,
                                            ssl->heap, DYNAMIC_TYPE_DER);
                        if (args->exportBuf == NULL) {
                            ERROR_OUT(MEMORY_E, exit_sske);
                        }
                    #ifdef HAVE_CURVE25519
                        if (ssl->ecdhCurveOID == ECC_X25519_OID) {
                            if (wc_curve25519_export_public_ex(
                                        (curve25519_key*)ssl->eccTempKey,
                                        args->exportBuf, &args->exportSz,
                                        EC25519_LITTLE_ENDIAN) != 0) {
                                ERROR_OUT(ECC_EXPORT_ERROR, exit_sske);
                            }
                        }
                        else
                    #endif
                        {
                            if (wc_ecc_export_x963(ssl->eccTempKey,
                                       args->exportBuf, &args->exportSz) != 0) {
                                ERROR_OUT(ECC_EXPORT_ERROR, exit_sske);
                            }
                       }
                        args->length += args->exportSz;

                        preSigSz  = args->length;
                        preSigIdx = args->idx;

                        switch(ssl->suites->sigAlgo)
                        {
                        #ifndef NO_RSA
                        #ifdef WC_RSA_PSS
                            case rsa_pss_sa_algo:
                        #endif
                            case rsa_sa_algo:
                            {
                                word32 i = 0;
                                int    keySz;

                                ssl->hsType = DYNAMIC_TYPE_RSA;
                                ret = AllocKey(ssl, ssl->hsType, &ssl->hsKey);
                                if (ret != 0) {
                                    goto exit_sske;
                                }

                                ret = wc_RsaPrivateKeyDecode(
                                    ssl->buffers.key->buffer,
                                    &i,
                                    (RsaKey*)ssl->hsKey,
                                    ssl->buffers.key->length);
                                if (ret != 0) {
                                    goto exit_sske;
                                }
                                keySz = wc_RsaEncryptSize((RsaKey*)ssl->hsKey);
                                if (keySz < 0) { /* test if keySz has error */
                                    ERROR_OUT(keySz, exit_sske);
                                }

                                args->tmpSigSz = (word32)keySz;
                                if (keySz < ssl->options.minRsaKeySz) {
                                    WOLFSSL_MSG("RSA signature key size too small");
                                    ERROR_OUT(RSA_KEY_SIZE_E, exit_sske);
                                }
                                break;
                            }
                        #endif /* !NO_RSA */
                            case ecc_dsa_sa_algo:
                            {
                                word32 i = 0;

                                ssl->hsType = DYNAMIC_TYPE_ECC;
                                ret = AllocKey(ssl, ssl->hsType, &ssl->hsKey);
                                if (ret != 0) {
                                    goto exit_sske;
                                }

                                ret = wc_EccPrivateKeyDecode(
                                    ssl->buffers.key->buffer,
                                    &i,
                                    (ecc_key*)ssl->hsKey,
                                    ssl->buffers.key->length);
                                if (ret != 0) {
                                    goto exit_sske;
                                }
                                /* worst case estimate */
                                args->tmpSigSz = wc_ecc_sig_size(
                                    (ecc_key*)ssl->hsKey);

                                /* check the minimum ECC key size */
                                if (wc_ecc_size((ecc_key*)ssl->hsKey) <
                                        ssl->options.minEccKeySz) {
                                    WOLFSSL_MSG("ECC key size too small");
                                    ret = ECC_KEY_SIZE_E;
                                    goto exit_sske;
                                }
                                break;
                            }
                        #ifdef HAVE_ED25519
                            case ed25519_sa_algo:
                            {
                                word32 i = 0;

                                ssl->hsType = DYNAMIC_TYPE_ED25519;
                                ret = AllocKey(ssl, ssl->hsType, &ssl->hsKey);
                                if (ret != 0) {
                                    goto exit_sske;
                                }

                                ret = wc_Ed25519PrivateKeyDecode(
                                    ssl->buffers.key->buffer,
                                    &i,
                                    (ed25519_key*)ssl->hsKey,
                                    ssl->buffers.key->length);
                                if (ret != 0) {
                                    goto exit_sske;
                                }
                                /* worst case estimate */
                                args->tmpSigSz = ED25519_SIG_SIZE;

                                /* check the minimum ECC key size */
                                if (ED25519_KEY_SIZE <
                                        ssl->options.minEccKeySz) {
                                    WOLFSSL_MSG("Ed25519 key size too small");
                                    ret = ECC_KEY_SIZE_E;
                                    goto exit_sske;
                                }
                                break;
                            }
                        #endif
                            default:
                                ERROR_OUT(ALGO_ID_E, exit_sske);  /* unsupported type */
                        } /* switch(ssl->specs.sig_algo) */

                        /* sig length */
                        args->length += LENGTH_SZ;
                        args->length += args->tmpSigSz;

                        if (IsAtLeastTLSv1_2(ssl)) {
                            args->length += HASH_SIG_SIZE;
                        }

                        args->sendSz = args->length + HANDSHAKE_HEADER_SZ + RECORD_HEADER_SZ;

                    #ifdef HAVE_QSH
                        args->length += args->qshSz;
                        args->sendSz += args->qshSz;
                    #endif
                    #ifdef WOLFSSL_DTLS
                        if (ssl->options.dtls) {
                            args->sendSz += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
                            args->idx    += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
                            preSigIdx = args->idx;
                        }
                    #endif
                        /* check for available size */
                        if ((ret = CheckAvailableSize(ssl, args->sendSz)) != 0) {
                            goto exit_sske;
                        }

                        /* get ouput buffer */
                        args->output = ssl->buffers.outputBuffer.buffer +
                                       ssl->buffers.outputBuffer.length;

                        /* record and message headers will be added below, when we're sure
                           of the sig length */

                        /* key exchange data */
                        args->output[args->idx++] = named_curve;
                        args->output[args->idx++] = 0x00;          /* leading zero */
                    #ifdef HAVE_CURVE25519
                        if (ssl->ecdhCurveOID == ECC_X25519_OID)
                            args->output[args->idx++] = WOLFSSL_ECC_X25519;
                        else
                    #endif
                        {
                            args->output[args->idx++] =
                                                    SetCurveId(ssl->eccTempKey);
                        }
                        args->output[args->idx++] = (byte)args->exportSz;
                        XMEMCPY(args->output + args->idx, args->exportBuf, args->exportSz);
                        args->idx += args->exportSz;

                        /* Determine hash type */
                        if (IsAtLeastTLSv1_2(ssl)) {
                            EncodeSigAlg(ssl->suites->hashAlgo,
                                         ssl->suites->sigAlgo,
                                         &args->output[args->idx]);
                            args->idx += 2;

                            hashType = HashAlgoToType(ssl->suites->hashAlgo);
                            if (hashType == WC_HASH_TYPE_NONE) {
                                ERROR_OUT(ALGO_ID_E, exit_sske);
                            }

                        } else {
                            /* only using sha and md5 for rsa */
                        #ifndef NO_OLD_TLS
                            hashType = WC_HASH_TYPE_SHA;
                            if (ssl->suites->sigAlgo == rsa_sa_algo) {
                                hashType = WC_HASH_TYPE_MD5_SHA;
                            }
                        #else
                            ERROR_OUT(ALGO_ID_E, exit_sske);
                        #endif
                        }

                        /* Signtaure length will be written later, when we're sure what it is */

                    #ifdef HAVE_FUZZER
                        if (ssl->fuzzerCb) {
                            ssl->fuzzerCb(ssl, args->output + preSigIdx,
                                preSigSz, FUZZ_SIGNATURE, ssl->fuzzerCtx);
                        }
                    #endif

                        /* Assemble buffer to hash for signature */
                        args->sigDataSz = RAN_LEN + RAN_LEN + preSigSz;
                        args->sigDataBuf = (byte*)XMALLOC(args->sigDataSz,
                                            ssl->heap, DYNAMIC_TYPE_SIGNATURE);
                        if (args->sigDataBuf == NULL) {
                            ERROR_OUT(MEMORY_E, exit_sske);
                        }
                        XMEMCPY(args->sigDataBuf, ssl->arrays->clientRandom,
                                                                       RAN_LEN);
                        XMEMCPY(args->sigDataBuf+RAN_LEN,
                                            ssl->arrays->serverRandom, RAN_LEN);
                        XMEMCPY(args->sigDataBuf+RAN_LEN+RAN_LEN,
                                args->output + preSigIdx, preSigSz);

                        if (ssl->suites->sigAlgo != ed25519_sa_algo) {
                            ssl->buffers.sig.length =
                                                 wc_HashGetDigestSize(hashType);
                            ssl->buffers.sig.buffer = (byte*)XMALLOC(
                                            ssl->buffers.sig.length,
                                            ssl->heap, DYNAMIC_TYPE_SIGNATURE);
                            if (ssl->buffers.sig.buffer == NULL) {
                                ERROR_OUT(MEMORY_E, exit_sske);
                            }

                            /* Perform hash */
                            ret = wc_Hash(hashType, args->sigDataBuf,
                                                       args->sigDataSz,
                                                       ssl->buffers.sig.buffer,
                                                       ssl->buffers.sig.length);
                            if (ret != 0) {
                                goto exit_sske;
                            }
                        }

                        args->sigSz = args->tmpSigSz;

                        /* Sign hash to create signature */
                        switch (ssl->suites->sigAlgo)
                        {
                        #ifndef NO_RSA
                            case rsa_sa_algo:
                            {
                                /* For TLS 1.2 re-encode signature */
                                if (IsAtLeastTLSv1_2(ssl)) {
                                    byte* encodedSig = (byte*)XMALLOC(
                                                  MAX_ENCODED_SIG_SZ, ssl->heap,
                                                       DYNAMIC_TYPE_SIGNATURE);
                                    if (encodedSig == NULL) {
                                        ERROR_OUT(MEMORY_E, exit_sske);
                                    }

                                    ssl->buffers.sig.length =
                                        wc_EncodeSignature(encodedSig,
                                            ssl->buffers.sig.buffer,
                                            ssl->buffers.sig.length,
                                            TypeHash(ssl->suites->hashAlgo));

                                    /* Replace sig buffer with new one */
                                    XFREE(ssl->buffers.sig.buffer, ssl->heap,
                                                       DYNAMIC_TYPE_SIGNATURE);
                                    ssl->buffers.sig.buffer = encodedSig;
                                }

                                /* write sig size here */
                                c16toa((word16)args->sigSz,
                                    args->output + args->idx);
                                args->idx += LENGTH_SZ;
                                break;
                            }
                        #ifdef WC_RSA_PSS
                            case rsa_pss_sa_algo:
                                /* write sig size here */
                                c16toa((word16)args->sigSz,
                                    args->output + args->idx);
                                args->idx += LENGTH_SZ;
                                break;
                        #endif
                        #endif /* !NO_RSA */
                        #ifdef HAVE_ED25519
                            case ed25519_sa_algo:
                        #endif
                            case ecc_dsa_sa_algo:
                            {
                                break;
                            }
                        } /* switch(ssl->specs.sig_algo) */
                        break;
                    }
                #endif /* HAVE_ECC */
                #if !defined(NO_DH) && !defined(NO_RSA)
                    case diffie_hellman_kea:
                    {
                        enum wc_HashType hashType;

                        args->idx = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;
                        args->length = LENGTH_SZ * 3;  /* p, g, pub */
                        args->length += ssl->buffers.serverDH_P.length +
                                        ssl->buffers.serverDH_G.length +
                                        ssl->buffers.serverDH_Pub.length;

                        preSigIdx = args->idx;
                        preSigSz  = args->length;

                        if (!ssl->options.usingAnon_cipher) {
                            word32   i = 0;
                            int      keySz;

                            /* make sure private key exists */
                            if (ssl->buffers.key == NULL ||
                                            ssl->buffers.key->buffer == NULL) {
                                ERROR_OUT(NO_PRIVATE_KEY, exit_sske);
                            }

                            ssl->hsType = DYNAMIC_TYPE_RSA;
                            ret = AllocKey(ssl, ssl->hsType, &ssl->hsKey);
                            if (ret != 0) {
                                goto exit_sske;
                            }

                            /* sig length */
                            args->length += LENGTH_SZ;

                            ret = wc_RsaPrivateKeyDecode(
                                ssl->buffers.key->buffer, &i,
                                (RsaKey*)ssl->hsKey, ssl->buffers.key->length);
                            if (ret != 0) {
                                goto exit_sske;
                            }
                            keySz = wc_RsaEncryptSize((RsaKey*)ssl->hsKey);
                            if (keySz < 0) { /* test if keySz has error */
                                ERROR_OUT(keySz, exit_sske);
                            }
                            args->tmpSigSz = (word32)keySz;
                            args->length += args->tmpSigSz;

                            if (keySz < ssl->options.minRsaKeySz) {
                                WOLFSSL_MSG("RSA key size too small");
                                ERROR_OUT(RSA_KEY_SIZE_E, exit_sske);
                            }

                            if (IsAtLeastTLSv1_2(ssl)) {
                                args->length += HASH_SIG_SIZE;
                            }
                        }

                        args->sendSz = args->length + HANDSHAKE_HEADER_SZ +
                                                            RECORD_HEADER_SZ;

                    #ifdef HAVE_QSH
                        args->length += args->qshSz;
                        args->sendSz += args->qshSz;
                    #endif
                    #ifdef WOLFSSL_DTLS
                        if (ssl->options.dtls) {
                            args->sendSz += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
                            args->idx    += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
                            preSigIdx = args->idx;
                        }
                    #endif

                        /* check for available size */
                        if ((ret = CheckAvailableSize(ssl, args->sendSz)) != 0) {
                            goto exit_sske;
                        }

                        /* get ouput buffer */
                        args->output = ssl->buffers.outputBuffer.buffer +
                                       ssl->buffers.outputBuffer.length;

                        AddHeaders(args->output, args->length,
                                                    server_key_exchange, ssl);

                        /* add p, g, pub */
                        c16toa((word16)ssl->buffers.serverDH_P.length,
                                                    args->output + args->idx);
                        args->idx += LENGTH_SZ;
                        XMEMCPY(args->output + args->idx,
                                              ssl->buffers.serverDH_P.buffer,
                                              ssl->buffers.serverDH_P.length);
                        args->idx += ssl->buffers.serverDH_P.length;

                        /*  g */
                        c16toa((word16)ssl->buffers.serverDH_G.length,
                                                    args->output + args->idx);
                        args->idx += LENGTH_SZ;
                        XMEMCPY(args->output + args->idx,
                                              ssl->buffers.serverDH_G.buffer,
                                              ssl->buffers.serverDH_G.length);
                        args->idx += ssl->buffers.serverDH_G.length;

                        /*  pub */
                        c16toa((word16)ssl->buffers.serverDH_Pub.length,
                                                    args->output + args->idx);
                        args->idx += LENGTH_SZ;
                        XMEMCPY(args->output + args->idx,
                                              ssl->buffers.serverDH_Pub.buffer,
                                              ssl->buffers.serverDH_Pub.length);
                        args->idx += ssl->buffers.serverDH_Pub.length;

                    #ifdef HAVE_FUZZER
                        if (ssl->fuzzerCb) {
                            ssl->fuzzerCb(ssl, args->output + preSigIdx,
                                preSigSz, FUZZ_SIGNATURE, ssl->fuzzerCtx);
                        }
                    #endif

                        if (ssl->options.usingAnon_cipher) {
                            break;
                        }

                        /* Determine hash type */
                        if (IsAtLeastTLSv1_2(ssl)) {
                            EncodeSigAlg(ssl->suites->hashAlgo,
                                         ssl->suites->sigAlgo,
                                         &args->output[args->idx]);
                            args->idx += 2;

                            hashType = HashAlgoToType(ssl->suites->hashAlgo);
                            if (hashType == WC_HASH_TYPE_NONE) {
                                ERROR_OUT(ALGO_ID_E, exit_sske);
                            }
                        } else {
                            /* only using sha and md5 for rsa */
                        #ifndef NO_OLD_TLS
                            hashType = WC_HASH_TYPE_SHA;
                            if (ssl->suites->sigAlgo == rsa_sa_algo) {
                                hashType = WC_HASH_TYPE_MD5_SHA;
                            }
                        #else
                            ERROR_OUT(ALGO_ID_E, exit_sske);
                        #endif
                        }

                        /* signature size */
                        c16toa((word16)args->tmpSigSz, args->output + args->idx);
                        args->idx += LENGTH_SZ;

                        /* Assemble buffer to hash for signature */
                        args->sigDataSz = RAN_LEN + RAN_LEN + preSigSz;
                        args->sigDataBuf = (byte*)XMALLOC(args->sigDataSz,
                                            ssl->heap, DYNAMIC_TYPE_SIGNATURE);
                        if (args->sigDataBuf == NULL) {
                            ERROR_OUT(MEMORY_E, exit_sske);
                        }
                        XMEMCPY(args->sigDataBuf, ssl->arrays->clientRandom,
                                                                    RAN_LEN);
                        XMEMCPY(args->sigDataBuf+RAN_LEN,
                                        ssl->arrays->serverRandom, RAN_LEN);
                        XMEMCPY(args->sigDataBuf+RAN_LEN+RAN_LEN,
                            args->output + preSigIdx, preSigSz);

                        if (ssl->suites->sigAlgo != ed25519_sa_algo) {
                            ssl->buffers.sig.length =
                                                 wc_HashGetDigestSize(hashType);
                            ssl->buffers.sig.buffer = (byte*)XMALLOC(
                                             ssl->buffers.sig.length, ssl->heap,
                                                       DYNAMIC_TYPE_SIGNATURE);
                            if (ssl->buffers.sig.buffer == NULL) {
                                ERROR_OUT(MEMORY_E, exit_sske);
                            }

                            /* Perform hash */
                            ret = wc_Hash(hashType, args->sigDataBuf,
                                                       args->sigDataSz,
                                                       ssl->buffers.sig.buffer,
                                                       ssl->buffers.sig.length);
                            if (ret != 0) {
                                goto exit_sske;
                            }
                        }

                        args->sigSz = args->tmpSigSz;

                        /* Sign hash to create signature */
                        switch (ssl->suites->sigAlgo)
                        {
                        #ifndef NO_RSA
                            case rsa_sa_algo:
                            {
                                /* For TLS 1.2 re-encode signature */
                                if (IsAtLeastTLSv1_2(ssl)) {
                                    byte* encodedSig = (byte*)XMALLOC(
                                                  MAX_ENCODED_SIG_SZ, ssl->heap,
                                                       DYNAMIC_TYPE_SIGNATURE);
                                    if (encodedSig == NULL) {
                                        ERROR_OUT(MEMORY_E, exit_sske);
                                    }

                                    ssl->buffers.sig.length =
                                        wc_EncodeSignature(encodedSig,
                                            ssl->buffers.sig.buffer,
                                            ssl->buffers.sig.length,
                                            TypeHash(ssl->suites->hashAlgo));

                                    /* Replace sig buffer with new one */
                                    XFREE(ssl->buffers.sig.buffer, ssl->heap,
                                                       DYNAMIC_TYPE_SIGNATURE);
                                    ssl->buffers.sig.buffer = encodedSig;
                                }
                                break;
                            }
                        #endif /* NO_RSA */
                        } /* switch (ssl->suites->sigAlgo) */
                        break;
                    }
                #endif /* !defined(NO_DH) && !defined(NO_RSA) */
                } /* switch(ssl->specs.kea) */

                /* Check for error */
                if (ret != 0) {
                    goto exit_sske;
                }

                /* Advance state and proceed */
                ssl->options.asyncState = TLS_ASYNC_DO;
            } /* case TLS_ASYNC_BUILD */
            FALL_THROUGH;

            case TLS_ASYNC_DO:
            {
                switch(ssl->specs.kea)
                {
                #ifndef NO_PSK
                    case psk_kea:
                    {
                        break;
                    }
                #endif /* !NO_PSK */
                #if !defined(NO_DH) && !defined(NO_PSK)
                    case dhe_psk_kea:
                    {
                        break;
                    }
                #endif /* !defined(NO_DH) && !defined(NO_PSK) */
                #if defined(HAVE_ECC) && !defined(NO_PSK)
                    case ecdhe_psk_kea:
                    {
                        break;
                    }
                #endif /* HAVE_ECC && !NO_PSK */
                #ifdef HAVE_ECC
                    case ecc_diffie_hellman_kea:
                    {
                        /* Sign hash to create signature */
                        switch (ssl->suites->sigAlgo)
                        {
                        #ifndef NO_RSA
                        #ifdef WC_RSA_PSS
                            case rsa_pss_sa_algo:
                        #endif
                            case rsa_sa_algo:
                            {
                                RsaKey* key = (RsaKey*)ssl->hsKey;

                                ret = RsaSign(ssl,
                                    ssl->buffers.sig.buffer,
                                    ssl->buffers.sig.length,
                                    args->output + args->idx,
                                    &args->sigSz,
                                    ssl->suites->sigAlgo, ssl->suites->hashAlgo,
                                    key,
                                    ssl->buffers.key->buffer,
                                    ssl->buffers.key->length,
                            #ifdef HAVE_PK_CALLBACKS
                                    ssl->RsaSignCtx
                            #else
                                    NULL
                            #endif
                                );
                                break;
                            }
                        #endif /* !NO_RSA */
                            case ecc_dsa_sa_algo:
                            {
                                ecc_key* key = (ecc_key*)ssl->hsKey;

                                ret = EccSign(ssl,
                                    ssl->buffers.sig.buffer,
                                    ssl->buffers.sig.length,
                                    args->output + LENGTH_SZ + args->idx,
                                    &args->sigSz,
                                    key,
                            #if defined(HAVE_PK_CALLBACKS)
                                    ssl->buffers.key->buffer,
                                    ssl->buffers.key->length,
                                    ssl->EccSignCtx
                            #else
                                    NULL, 0, NULL
                            #endif
                                );
                                break;
                            }
                        #ifdef HAVE_ED25519
                            case ed25519_sa_algo:
                            {
                                ed25519_key* key = (ed25519_key*)ssl->hsKey;

                                ret = Ed25519Sign(ssl,
                                    args->sigDataBuf, args->sigDataSz,
                                    args->output + LENGTH_SZ + args->idx,
                                    &args->sigSz,
                                    key,
                            #if defined(HAVE_PK_CALLBACKS)
                                    ssl->buffers.key->buffer,
                                    ssl->buffers.key->length,
                                    ssl->Ed25519SignCtx
                            #else
                                    NULL, 0, NULL
                            #endif
                                );
                                break;
                            }
                        #endif
                        } /* switch(ssl->specs.sig_algo) */
                        break;
                    }
                #endif /* HAVE_ECC */
                #if !defined(NO_DH) && !defined(NO_RSA)
                    case diffie_hellman_kea:
                    {
                        /* Sign hash to create signature */
                        switch (ssl->suites->sigAlgo)
                        {
                        #ifndef NO_RSA
                        #ifdef WC_RSA_PSS
                            case rsa_pss_sa_algo:
                        #endif
                            case rsa_sa_algo:
                            {
                                RsaKey* key = (RsaKey*)ssl->hsKey;

                                if (ssl->options.usingAnon_cipher) {
                                    break;
                                }

                                ret = RsaSign(ssl,
                                    ssl->buffers.sig.buffer,
                                    ssl->buffers.sig.length,
                                    args->output + args->idx,
                                    &args->sigSz,
                                    ssl->suites->sigAlgo, ssl->suites->hashAlgo,
                                    key,
                                    ssl->buffers.key->buffer,
                                    ssl->buffers.key->length,
                                #ifdef HAVE_PK_CALLBACKS
                                    ssl->RsaSignCtx
                                #else
                                    NULL
                                #endif
                                );
                                break;
                            }
                        #endif /* NO_RSA */
                        } /* switch (ssl->suites->sigAlgo) */

                        break;
                    }
                #endif /* !defined(NO_DH) && !defined(NO_RSA) */
                } /* switch(ssl->specs.kea) */

                /* Check for error */
                if (ret != 0) {
                    goto exit_sske;
                }

                /* Advance state and proceed */
                ssl->options.asyncState = TLS_ASYNC_VERIFY;
            } /* case TLS_ASYNC_DO */
            FALL_THROUGH;

            case TLS_ASYNC_VERIFY:
            {
                switch(ssl->specs.kea)
                {
                #ifndef NO_PSK
                    case psk_kea:
                    {
                        /* Nothing to do in this sub-state */
                        break;
                    }
                #endif /* !NO_PSK */
                #if !defined(NO_DH) && !defined(NO_PSK)
                    case dhe_psk_kea:
                    {
                        /* Nothing to do in this sub-state */
                        break;
                    }
                #endif /* !defined(NO_DH) && !defined(NO_PSK) */
                #if defined(HAVE_ECC) && !defined(NO_PSK)
                    case ecdhe_psk_kea:
                    {
                        /* Nothing to do in this sub-state */
                        break;
                    }
                #endif /* HAVE_ECC && !NO_PSK */
                #ifdef HAVE_ECC
                    case ecc_diffie_hellman_kea:
                    {
                        switch(ssl->suites->sigAlgo)
                        {
                        #ifndef NO_RSA
                        #ifdef WC_RSA_PSS
                            case rsa_pss_sa_algo:
                        #endif
                            case rsa_sa_algo:
                            {
                                RsaKey* key = (RsaKey*)ssl->hsKey;

                                if (args->verifySig == NULL) {
                                    if (args->sigSz == 0) {
                                        ERROR_OUT(BAD_COND_E, exit_sske);
                                    }
                                    args->verifySig = (byte*)XMALLOC(
                                                    args->sigSz, ssl->heap,
                                                    DYNAMIC_TYPE_SIGNATURE);
                                    if (!args->verifySig) {
                                        ERROR_OUT(MEMORY_E, exit_sske);
                                    }
                                    XMEMCPY(args->verifySig,
                                        args->output + args->idx, args->sigSz);
                                }

                                /* check for signature faults */
                                ret = VerifyRsaSign(ssl,
                                    args->verifySig, args->sigSz,
                                    ssl->buffers.sig.buffer,
                                    ssl->buffers.sig.length,
                                    ssl->suites->sigAlgo, ssl->suites->hashAlgo,
                                    key
                                );
                                break;
                            }
                        #endif
                            case ecc_dsa_sa_algo:
                            {
                                /* Now that we know the real sig size, write it. */
                                c16toa((word16)args->sigSz,
                                                    args->output + args->idx);

                                /* And adjust length and sendSz from estimates */
                                args->length += args->sigSz - args->tmpSigSz;
                                args->sendSz += args->sigSz - args->tmpSigSz;
                                break;
                            }
                        #ifdef HAVE_ED25519
                            case ed25519_sa_algo:
                            {
                                /* Now that we know the real sig size, write it. */
                                c16toa((word16)args->sigSz,
                                                    args->output + args->idx);

                                /* And adjust length and sendSz from estimates */
                                args->length += args->sigSz - args->tmpSigSz;
                                args->sendSz += args->sigSz - args->tmpSigSz;
                                break;
                            }
                        #endif
                            default:
                                ERROR_OUT(ALGO_ID_E, exit_sske);  /* unsupported type */
                        } /* switch(ssl->specs.sig_algo) */
                        break;
                    }
                #endif /* HAVE_ECC */
                #if !defined(NO_DH) && !defined(NO_RSA)
                    case diffie_hellman_kea:
                    {
                        switch (ssl->suites->sigAlgo)
                        {
                        #ifndef NO_RSA
                        #ifndef WC_RSA_PSS
                            case rsa_pss_sa_algo:
                        #endif
                            case rsa_sa_algo:
                            {
                                RsaKey* key = (RsaKey*)ssl->hsKey;

                                if (ssl->options.usingAnon_cipher) {
                                    break;
                                }

                                if (args->verifySig == NULL) {
                                    if (args->sigSz == 0) {
                                        ERROR_OUT(BAD_COND_E, exit_sske);
                                    }
                                    args->verifySig = (byte*)XMALLOC(
                                                      args->sigSz, ssl->heap,
                                                      DYNAMIC_TYPE_SIGNATURE);
                                    if (!args->verifySig) {
                                        ERROR_OUT(MEMORY_E, exit_sske);
                                    }
                                    XMEMCPY(args->verifySig,
                                        args->output + args->idx, args->sigSz);
                                }

                                /* check for signature faults */
                                ret = VerifyRsaSign(ssl,
                                    args->verifySig, args->sigSz,
                                    ssl->buffers.sig.buffer,
                                    ssl->buffers.sig.length,
                                    ssl->suites->sigAlgo, ssl->suites->hashAlgo,
                                    key
                                );
                                break;
                            }
                        #endif
                        } /* switch (ssl->suites->sigAlgo) */
                        break;
                    }
                #endif /* !defined(NO_DH) && !defined(NO_RSA) */
                } /* switch(ssl->specs.kea) */

                /* Check for error */
                if (ret != 0) {
                    goto exit_sske;
                }

                /* Advance state and proceed */
                ssl->options.asyncState = TLS_ASYNC_FINALIZE;
            } /* case TLS_ASYNC_VERIFY */
            FALL_THROUGH;

            case TLS_ASYNC_FINALIZE:
            {
            #ifdef HAVE_QSH
                if (ssl->peerQSHKeyPresent) {
                    if (args->qshSz > 0) {
                        args->idx = args->sendSz - args->qshSz;
                        if (QSH_KeyExchangeWrite(ssl, 1) != 0) {
                            ERROR_OUT(MEMORY_E, exit_sske);
                        }

                        /* extension type */
                        c16toa(TLSX_QUANTUM_SAFE_HYBRID,
                                                    args->output + args->idx);
                        args->idx += OPAQUE16_LEN;

                        /* write to output and check amount written */
                        if (TLSX_QSHPK_Write(ssl->QSH_secret->list,
                            args->output + args->idx) >
                                                args->qshSz - OPAQUE16_LEN) {
                            ERROR_OUT(MEMORY_E, exit_sske);
                        }
                    }
                }
            #endif

            #if defined(HAVE_ECC)
                if (ssl->specs.kea == ecdhe_psk_kea ||
                    ssl->specs.kea == ecc_diffie_hellman_kea) {
                    /* Check output to make sure it was set */
                    if (args->output) {
                        AddHeaders(args->output, args->length,
                                                    server_key_exchange, ssl);
                    }
                    else {
                        ERROR_OUT(BUFFER_ERROR, exit_sske);
                    }
                }
            #endif /* HAVE_ECC */

            #ifdef WOLFSSL_DTLS
                if (IsDtlsNotSctpMode(ssl)) {
                    if ((ret = DtlsMsgPoolSave(ssl, args->output, args->sendSz)) != 0) {
                        goto exit_sske;
                    }
                }

                if (ssl->options.dtls)
                    DtlsSEQIncrement(ssl, CUR_ORDER);
            #endif

                ret = HashOutput(ssl, args->output, args->sendSz, 0);
                if (ret != 0) {
                    goto exit_sske;
                }

            #ifdef WOLFSSL_CALLBACKS
                if (ssl->hsInfoOn) {
                    AddPacketName("ServerKeyExchange", &ssl->handShakeInfo);
                }
                if (ssl->toInfoOn) {
                    AddPacketInfo("ServerKeyExchange", &ssl->timeoutInfo,
                        args->output, args->sendSz, ssl->heap);
                }
            #endif

                /* Advance state and proceed */
                ssl->options.asyncState = TLS_ASYNC_END;
            } /* case TLS_ASYNC_FINALIZE */
            FALL_THROUGH;

            case TLS_ASYNC_END:
            {
                ssl->buffers.outputBuffer.length += args->sendSz;
                if (!ssl->options.groupMessages) {
                    ret = SendBuffered(ssl);
                }

                ssl->options.serverState = SERVER_KEYEXCHANGE_COMPLETE;
                break;
            }
            default:
                ret = INPUT_CASE_ERROR;
        } /* switch(ssl->options.asyncState) */

    exit_sske:

        WOLFSSL_LEAVE("SendServerKeyExchange", ret);

    #ifdef WOLFSSL_ASYNC_CRYPT
        /* Handle async operation */
        if (ret == WC_PENDING_E)
            return ret;
    #endif /* WOLFSSL_ASYNC_CRYPT */

        /* Final cleanup */
        FreeSskeArgs(ssl, args);
        FreeKeyExchange(ssl);

        return ret;
    }

#ifdef HAVE_SERVER_RENEGOTIATION_INFO

    /* search suites for specific one, idx on success, negative on error */
    static int FindSuite(Suites* suites, byte first, byte second)
    {
        int i;

        if (suites == NULL || suites->suiteSz == 0) {
            WOLFSSL_MSG("Suites pointer error or suiteSz 0");
            return SUITES_ERROR;
        }

        for (i = 0; i < suites->suiteSz-1; i += SUITE_LEN) {
            if (suites->suites[i]   == first &&
                suites->suites[i+1] == second )
                return i;
        }

        return MATCH_SUITE_ERROR;
    }

#endif

    /* Make sure server cert/key are valid for this suite, true on success */
    static int VerifyServerSuite(WOLFSSL* ssl, word16 idx)
    {
        int  haveRSA = !ssl->options.haveStaticECC;
        int  havePSK = 0;
        byte first;
        byte second;

        WOLFSSL_ENTER("VerifyServerSuite");

        if (ssl->suites == NULL) {
            WOLFSSL_MSG("Suites pointer error");
            return 0;
        }

        first   = ssl->suites->suites[idx];
        second  = ssl->suites->suites[idx+1];

        #ifndef NO_PSK
            havePSK = ssl->options.havePSK;
        #endif

        if (ssl->options.haveNTRU)
            haveRSA = 0;

        if (CipherRequires(first, second, REQUIRES_RSA)) {
            WOLFSSL_MSG("Requires RSA");
            if (haveRSA == 0) {
                WOLFSSL_MSG("Don't have RSA");
                return 0;
            }
        }

        if (CipherRequires(first, second, REQUIRES_DHE)) {
            WOLFSSL_MSG("Requires DHE");
            if (ssl->options.haveDH == 0) {
                WOLFSSL_MSG("Don't have DHE");
                return 0;
            }
        }

        if (CipherRequires(first, second, REQUIRES_ECC)) {
            WOLFSSL_MSG("Requires ECC");
            if (ssl->options.haveECC == 0) {
                WOLFSSL_MSG("Don't have ECC");
                return 0;
            }
        }

        if (CipherRequires(first, second, REQUIRES_ECC_STATIC)) {
            WOLFSSL_MSG("Requires static ECC");
            if (ssl->options.haveStaticECC == 0) {
                WOLFSSL_MSG("Don't have static ECC");
                return 0;
            }
        }

        if (CipherRequires(first, second, REQUIRES_PSK)) {
            WOLFSSL_MSG("Requires PSK");
            if (havePSK == 0) {
                WOLFSSL_MSG("Don't have PSK");
                return 0;
            }
        }

        if (CipherRequires(first, second, REQUIRES_NTRU)) {
            WOLFSSL_MSG("Requires NTRU");
            if (ssl->options.haveNTRU == 0) {
                WOLFSSL_MSG("Don't have NTRU");
                return 0;
            }
        }

        if (CipherRequires(first, second, REQUIRES_RSA_SIG)) {
            WOLFSSL_MSG("Requires RSA Signature");
            if (ssl->options.side == WOLFSSL_SERVER_END &&
                                           ssl->options.haveECDSAsig == 1) {
                WOLFSSL_MSG("Don't have RSA Signature");
                return 0;
            }
        }

#ifdef HAVE_SUPPORTED_CURVES
        if (!TLSX_ValidateEllipticCurves(ssl, first, second)) {
            WOLFSSL_MSG("Don't have matching curves");
                return 0;
        }
#endif

        /* ECCDHE is always supported if ECC on */

#ifdef HAVE_QSH
        /* need to negotiate a classic suite in addition to TLS_QSH */
        if (first == QSH_BYTE && second == TLS_QSH) {
            if (TLSX_SupportExtensions(ssl)) {
                ssl->options.haveQSH = 1; /* matched TLS_QSH */
            }
            else {
                WOLFSSL_MSG("Version of SSL connection does not support TLS_QSH");
            }
            return 0;
        }
#endif

#ifdef WOLFSSL_TLS13
        if (IsAtLeastTLSv1_3(ssl->version) &&
            ssl->options.side == WOLFSSL_SERVER_END) {
            /* Try to establish a key share. */
            int ret = TLSX_KeyShare_Establish(ssl);
            if (ret == KEY_SHARE_ERROR)
                ssl->options.serverState = SERVER_HELLO_RETRY_REQUEST;
            else if (ret != 0)
                return 0;
        }
#endif

        return 1;
    }

#ifndef NO_WOLFSSL_SERVER
    static int CompareSuites(WOLFSSL* ssl, Suites* peerSuites, word16 i,
                             word16 j)
    {
        if (ssl->suites->suites[i]   == peerSuites->suites[j] &&
            ssl->suites->suites[i+1] == peerSuites->suites[j+1] ) {

            if (VerifyServerSuite(ssl, i)) {
                int result;
                WOLFSSL_MSG("Verified suite validity");
                ssl->options.cipherSuite0 = ssl->suites->suites[i];
                ssl->options.cipherSuite  = ssl->suites->suites[i+1];
                result = SetCipherSpecs(ssl);
                if (result == 0)
                    PickHashSigAlgo(ssl, peerSuites->hashSigAlgo,
                                    peerSuites->hashSigAlgoSz);
                return result;
            }
            else {
                WOLFSSL_MSG("Could not verify suite validity, continue");
            }
        }

        return MATCH_SUITE_ERROR;
    }

    int MatchSuite(WOLFSSL* ssl, Suites* peerSuites)
    {
        int ret;
        word16 i, j;

        WOLFSSL_ENTER("MatchSuite");

        /* & 0x1 equivalent % 2 */
        if (peerSuites->suiteSz == 0 || peerSuites->suiteSz & 0x1)
            return MATCH_SUITE_ERROR;

        if (ssl->suites == NULL)
            return SUITES_ERROR;

        if (!ssl->options.useClientOrder) {
            /* Server order */
            for (i = 0; i < ssl->suites->suiteSz; i += 2) {
                for (j = 0; j < peerSuites->suiteSz; j += 2) {
                    ret = CompareSuites(ssl, peerSuites, i, j);
                    if (ret != MATCH_SUITE_ERROR)
                        return ret;
                }
            }
        }
        else {
            /* Client order */
            for (j = 0; j < peerSuites->suiteSz; j += 2) {
                for (i = 0; i < ssl->suites->suiteSz; i += 2) {
                    ret = CompareSuites(ssl, peerSuites, i, j);
                    if (ret != MATCH_SUITE_ERROR)
                        return ret;
                }
            }
        }

        return MATCH_SUITE_ERROR;
    }
#endif

#ifdef OLD_HELLO_ALLOWED

    /* process old style client hello, deprecate? */
    int ProcessOldClientHello(WOLFSSL* ssl, const byte* input, word32* inOutIdx,
                              word32 inSz, word16 sz)
    {
        word32          idx = *inOutIdx;
        word16          sessionSz;
        word16          randomSz;
        word16          i, j;
        ProtocolVersion pv;
        Suites          clSuites;

        (void)inSz;
        WOLFSSL_MSG("Got old format client hello");
#ifdef WOLFSSL_CALLBACKS
        if (ssl->hsInfoOn)
            AddPacketName("ClientHello", &ssl->handShakeInfo);
        if (ssl->toInfoOn)
            AddLateName("ClientHello", &ssl->timeoutInfo);
#endif

        /* manually hash input since different format */
#ifndef NO_OLD_TLS
#ifndef NO_MD5
        wc_Md5Update(&ssl->hsHashes->hashMd5, input + idx, sz);
#endif
#ifndef NO_SHA
        wc_ShaUpdate(&ssl->hsHashes->hashSha, input + idx, sz);
#endif
#endif
#ifndef NO_SHA256
        if (IsAtLeastTLSv1_2(ssl)) {
            int shaRet = wc_Sha256Update(&ssl->hsHashes->hashSha256,
                                         input + idx, sz);
            if (shaRet != 0)
                return shaRet;
        }
#endif

        /* does this value mean client_hello? */
        idx++;

        /* version */
        pv.major = input[idx++];
        pv.minor = input[idx++];
        ssl->chVersion = pv;  /* store */

        if (ssl->version.minor > pv.minor) {
            byte haveRSA = 0;
            byte havePSK = 0;
            if (!ssl->options.downgrade) {
                WOLFSSL_MSG("Client trying to connect with lesser version");
                return VERSION_ERROR;
            }
            if (pv.minor < ssl->options.minDowngrade) {
                WOLFSSL_MSG("\tversion below minimum allowed, fatal error");
                return VERSION_ERROR;
            }
            if (pv.minor == SSLv3_MINOR) {
                /* turn off tls */
                WOLFSSL_MSG("\tdowngrading to SSLv3");
                ssl->options.tls    = 0;
                ssl->options.tls1_1 = 0;
                ssl->version.minor  = SSLv3_MINOR;
            }
            else if (pv.minor == TLSv1_MINOR) {
                WOLFSSL_MSG("\tdowngrading to TLSv1");
                /* turn off tls 1.1+ */
                ssl->options.tls1_1 = 0;
                ssl->version.minor  = TLSv1_MINOR;
            }
            else if (pv.minor == TLSv1_1_MINOR) {
                WOLFSSL_MSG("\tdowngrading to TLSv1.1");
                ssl->version.minor  = TLSv1_1_MINOR;
            }
            else if (pv.minor == TLSv1_2_MINOR) {
                WOLFSSL_MSG("    downgrading to TLSv1.2");
                ssl->version.minor  = TLSv1_2_MINOR;
            }
#ifndef NO_RSA
            haveRSA = 1;
#endif
#ifndef NO_PSK
            havePSK = ssl->options.havePSK;
#endif

            InitSuites(ssl->suites, ssl->version, ssl->keySz, haveRSA, havePSK,
                       ssl->options.haveDH, ssl->options.haveNTRU,
                       ssl->options.haveECDSAsig, ssl->options.haveECC,
                       ssl->options.haveStaticECC, ssl->options.side);
        }

        /* suite size */
        ato16(&input[idx], &clSuites.suiteSz);
        idx += OPAQUE16_LEN;

        if (clSuites.suiteSz > WOLFSSL_MAX_SUITE_SZ)
            return BUFFER_ERROR;
        clSuites.hashSigAlgoSz = 0;

        /* session size */
        ato16(&input[idx], &sessionSz);
        idx += OPAQUE16_LEN;

        if (sessionSz > ID_LEN)
            return BUFFER_ERROR;

        /* random size */
        ato16(&input[idx], &randomSz);
        idx += OPAQUE16_LEN;

        if (randomSz > RAN_LEN)
            return BUFFER_ERROR;

        /* suites */
        for (i = 0, j = 0; i < clSuites.suiteSz; i += 3) {
            byte first = input[idx++];
            if (!first) { /* implicit: skip sslv2 type */
                XMEMCPY(&clSuites.suites[j], &input[idx], SUITE_LEN);
                j += SUITE_LEN;
            }
            idx += SUITE_LEN;
        }
        clSuites.suiteSz = j;

        /* session id */
        if (sessionSz) {
            XMEMCPY(ssl->arrays->sessionID, input + idx, sessionSz);
            ssl->arrays->sessionIDSz = (byte)sessionSz;
            idx += sessionSz;
            ssl->options.resuming = 1;
        }

        /* random */
        if (randomSz < RAN_LEN)
            XMEMSET(ssl->arrays->clientRandom, 0, RAN_LEN - randomSz);
        XMEMCPY(&ssl->arrays->clientRandom[RAN_LEN - randomSz], input + idx,
               randomSz);
        idx += randomSz;

        if (ssl->options.usingCompression)
            ssl->options.usingCompression = 0;  /* turn off */

        ssl->options.clientState = CLIENT_HELLO_COMPLETE;
        *inOutIdx = idx;

        ssl->options.haveSessionId = 1;
        /* DoClientHello uses same resume code */
        if (ssl->options.resuming) {  /* let's try */
            int ret = -1;
            WOLFSSL_SESSION* session = GetSession(ssl,
                                                  ssl->arrays->masterSecret, 1);
            #ifdef HAVE_SESSION_TICKET
                if (ssl->options.useTicket == 1) {
                    session = &ssl->session;
                }
            #endif

            if (!session) {
                WOLFSSL_MSG("Session lookup for resume failed");
                ssl->options.resuming = 0;
            } else {
            #ifdef HAVE_EXT_CACHE
                wolfSSL_SESSION_free(session);
            #endif
                if (MatchSuite(ssl, &clSuites) < 0) {
                    WOLFSSL_MSG("Unsupported cipher suite, OldClientHello");
                    return UNSUPPORTED_SUITE;
                }

                ret = wc_RNG_GenerateBlock(ssl->rng, ssl->arrays->serverRandom,
                                                                       RAN_LEN);
                if (ret != 0)
                    return ret;

                #ifdef NO_OLD_TLS
                    ret = DeriveTlsKeys(ssl);
                #else
                    #ifndef NO_TLS
                        if (ssl->options.tls)
                            ret = DeriveTlsKeys(ssl);
                    #endif
                        if (!ssl->options.tls)
                            ret = DeriveKeys(ssl);
                #endif
                ssl->options.clientState = CLIENT_KEYEXCHANGE_COMPLETE;

                return ret;
            }
        }

        return MatchSuite(ssl, &clSuites);
    }

#endif /* OLD_HELLO_ALLOWED */


    int DoClientHello(WOLFSSL* ssl, const byte* input, word32* inOutIdx,
                             word32 helloSz)
    {
        byte            b;
        byte            bogusID = 0;   /* flag for a bogus session id */
        ProtocolVersion pv;
        Suites          clSuites;
        word32          i = *inOutIdx;
        word32          begin = i;
#ifdef WOLFSSL_DTLS
        Hmac            cookieHmac;
        byte            peerCookie[MAX_COOKIE_LEN];
        byte            peerCookieSz = 0;
        byte            cookieType;
        byte            cookieSz = 0;

        XMEMSET(&cookieHmac, 0, sizeof(Hmac));
#endif /* WOLFSSL_DTLS */

#ifdef WOLFSSL_CALLBACKS
        if (ssl->hsInfoOn) AddPacketName("ClientHello", &ssl->handShakeInfo);
        if (ssl->toInfoOn) AddLateName("ClientHello", &ssl->timeoutInfo);
#endif

        /* protocol version, random and session id length check */
        if ((i - begin) + OPAQUE16_LEN + RAN_LEN + OPAQUE8_LEN > helloSz)
            return BUFFER_ERROR;

        /* protocol version */
        XMEMCPY(&pv, input + i, OPAQUE16_LEN);
        ssl->chVersion = pv;   /* store */
#ifdef WOLFSSL_DTLS
        if (IsDtlsNotSctpMode(ssl)) {
            int ret;
            #if defined(NO_SHA) && defined(NO_SHA256)
                #error "DTLS needs either SHA or SHA-256"
            #endif /* NO_SHA && NO_SHA256 */

            #if !defined(NO_SHA) && defined(NO_SHA256)
                cookieType = SHA;
                cookieSz = SHA_DIGEST_SIZE;
            #endif /* NO_SHA */
            #ifndef NO_SHA256
                cookieType = SHA256;
                cookieSz = SHA256_DIGEST_SIZE;
            #endif /* NO_SHA256 */
            ret = wc_HmacSetKey(&cookieHmac, cookieType,
                                ssl->buffers.dtlsCookieSecret.buffer,
                                ssl->buffers.dtlsCookieSecret.length);
            if (ret != 0) return ret;
            ret = wc_HmacUpdate(&cookieHmac,
                                (const byte*)ssl->buffers.dtlsCtx.peer.sa,
                                ssl->buffers.dtlsCtx.peer.sz);
            if (ret != 0) return ret;
            ret = wc_HmacUpdate(&cookieHmac, input + i, OPAQUE16_LEN);
            if (ret != 0) return ret;
        }
#endif /* WOLFSSL_DTLS */
        i += OPAQUE16_LEN;

        if ((!ssl->options.dtls && ssl->version.minor > pv.minor) ||
            (ssl->options.dtls && ssl->version.minor != DTLS_MINOR
             && ssl->version.minor != DTLSv1_2_MINOR && pv.minor != DTLS_MINOR
             && pv.minor != DTLSv1_2_MINOR)) {

            word16 haveRSA = 0;
            word16 havePSK = 0;
            int    keySz   = 0;

            if (!ssl->options.downgrade) {
                WOLFSSL_MSG("Client trying to connect with lesser version");
                return VERSION_ERROR;
            }
            if (pv.minor < ssl->options.minDowngrade) {
                WOLFSSL_MSG("\tversion below minimum allowed, fatal error");
                return VERSION_ERROR;
            }

            if (pv.minor == SSLv3_MINOR) {
                /* turn off tls */
                WOLFSSL_MSG("\tdowngrading to SSLv3");
                ssl->options.tls    = 0;
                ssl->options.tls1_1 = 0;
                ssl->version.minor  = SSLv3_MINOR;
            }
            else if (pv.minor == TLSv1_MINOR) {
                /* turn off tls 1.1+ */
                WOLFSSL_MSG("\tdowngrading to TLSv1");
                ssl->options.tls1_1 = 0;
                ssl->version.minor  = TLSv1_MINOR;
            }
            else if (pv.minor == TLSv1_1_MINOR) {
                WOLFSSL_MSG("\tdowngrading to TLSv1.1");
                ssl->version.minor  = TLSv1_1_MINOR;
            }
            else if (pv.minor == TLSv1_2_MINOR) {
                WOLFSSL_MSG("    downgrading to TLSv1.2");
                ssl->version.minor  = TLSv1_2_MINOR;
            }
#ifndef NO_RSA
            haveRSA = 1;
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
        }

#ifdef OPENSSL_EXTRA
        /* check if option is set to not allow the current version
         * set from either wolfSSL_set_options or wolfSSL_CTX_set_options */
        if (!ssl->options.dtls && ssl->options.downgrade &&
                ssl->options.mask > 0) {
            int reset = 0;
            if (ssl->version.minor == TLSv1_2_MINOR &&
             (ssl->options.mask & SSL_OP_NO_TLSv1_2) == SSL_OP_NO_TLSv1_2) {
                WOLFSSL_MSG("\tOption set to not allow TLSv1.2, Downgrading");
                ssl->version.minor = TLSv1_1_MINOR;
                reset = 1;
            }
            if (ssl->version.minor == TLSv1_1_MINOR &&
             (ssl->options.mask & SSL_OP_NO_TLSv1_1) == SSL_OP_NO_TLSv1_1) {
                WOLFSSL_MSG("\tOption set to not allow TLSv1.1, Downgrading");
                ssl->options.tls1_1 = 0;
                ssl->version.minor = TLSv1_MINOR;
                reset = 1;
            }
            if (ssl->version.minor == TLSv1_MINOR &&
                (ssl->options.mask & SSL_OP_NO_TLSv1) == SSL_OP_NO_TLSv1) {
                WOLFSSL_MSG("\tOption set to not allow TLSv1, Downgrading");
                ssl->options.tls    = 0;
                ssl->options.tls1_1 = 0;
                ssl->version.minor = SSLv3_MINOR;
                reset = 1;
            }
            if (ssl->version.minor == SSLv3_MINOR &&
                (ssl->options.mask & SSL_OP_NO_SSLv3) == SSL_OP_NO_SSLv3) {
                WOLFSSL_MSG("\tError, option set to not allow SSLv3");
                return VERSION_ERROR;
            }

            if (ssl->version.minor < ssl->options.minDowngrade) {
                WOLFSSL_MSG("\tversion below minimum allowed, fatal error");
                return VERSION_ERROR;
            }

            if (reset) {
                word16 haveRSA = 0;
                word16 havePSK = 0;
                int    keySz   = 0;

            #ifndef NO_RSA
                haveRSA = 1;
            #endif
            #ifndef NO_PSK
                havePSK = ssl->options.havePSK;
            #endif
            #ifndef NO_CERTS
                keySz = ssl->buffers.keySz;
            #endif

                /* reset cipher suites to account for TLS version change */
                InitSuites(ssl->suites, ssl->version, keySz, haveRSA, havePSK,
                       ssl->options.haveDH, ssl->options.haveNTRU,
                       ssl->options.haveECDSAsig, ssl->options.haveECC,
                       ssl->options.haveStaticECC, ssl->options.side);
            }
        }
#endif

        /* random */
        XMEMCPY(ssl->arrays->clientRandom, input + i, RAN_LEN);
#ifdef WOLFSSL_DTLS
        if (IsDtlsNotSctpMode(ssl)) {
            int ret = wc_HmacUpdate(&cookieHmac, input + i, RAN_LEN);
            if (ret != 0) return ret;
        }
#endif /* WOLFSSL_DTLS */
        i += RAN_LEN;

#ifdef SHOW_SECRETS
        {
            int j;
            printf("client random: ");
            for (j = 0; j < RAN_LEN; j++)
                printf("%02x", ssl->arrays->clientRandom[j]);
            printf("\n");
        }
#endif

        /* session id */
        b = input[i++];

#ifdef HAVE_SESSION_TICKET
        if (b > 0 && b < ID_LEN) {
            bogusID = 1;
            WOLFSSL_MSG("Client sent bogus session id, let's allow for echo");
        }
#endif

        if (b == ID_LEN || bogusID) {
            if ((i - begin) + b > helloSz)
                return BUFFER_ERROR;

            XMEMCPY(ssl->arrays->sessionID, input + i, b);
#ifdef WOLFSSL_DTLS
            if (IsDtlsNotSctpMode(ssl)) {
                int ret = wc_HmacUpdate(&cookieHmac, input + i - 1, b + 1);
                if (ret != 0) return ret;
            }
#endif /* WOLFSSL_DTLS */
            ssl->arrays->sessionIDSz = b;
            i += b;
            ssl->options.resuming = 1; /* client wants to resume */
            WOLFSSL_MSG("Client wants to resume session");
        }
        else if (b) {
            WOLFSSL_MSG("Invalid session ID size");
            return BUFFER_ERROR; /* session ID nor 0 neither 32 bytes long */
        }

        #ifdef WOLFSSL_DTLS
            /* cookie */
            if (ssl->options.dtls) {

                if ((i - begin) + OPAQUE8_LEN > helloSz)
                    return BUFFER_ERROR;

                peerCookieSz = input[i++];

                if (peerCookieSz) {
                    if (peerCookieSz > MAX_COOKIE_LEN)
                        return BUFFER_ERROR;

                    if ((i - begin) + peerCookieSz > helloSz)
                        return BUFFER_ERROR;

                    XMEMCPY(peerCookie, input + i, peerCookieSz);

                    i += peerCookieSz;
                }
            }
        #endif

        /* suites */
        if ((i - begin) + OPAQUE16_LEN > helloSz)
            return BUFFER_ERROR;

        ato16(&input[i], &clSuites.suiteSz);
        i += OPAQUE16_LEN;

        /* suites and compression length check */
        if ((i - begin) + clSuites.suiteSz + OPAQUE8_LEN > helloSz)
            return BUFFER_ERROR;

        if (clSuites.suiteSz > WOLFSSL_MAX_SUITE_SZ)
            return BUFFER_ERROR;

        XMEMCPY(clSuites.suites, input + i, clSuites.suiteSz);

#ifdef HAVE_SERVER_RENEGOTIATION_INFO
        /* check for TLS_EMPTY_RENEGOTIATION_INFO_SCSV suite */
        if (FindSuite(&clSuites, 0, TLS_EMPTY_RENEGOTIATION_INFO_SCSV) >= 0) {
            int ret = 0;

            ret = TLSX_AddEmptyRenegotiationInfo(&ssl->extensions, ssl->heap);
            if (ret != SSL_SUCCESS)
                return ret;
        }
#endif /* HAVE_SERVER_RENEGOTIATION_INFO */

#ifdef WOLFSSL_DTLS
        if (IsDtlsNotSctpMode(ssl)) {
            int ret = wc_HmacUpdate(&cookieHmac,
                                    input + i - OPAQUE16_LEN,
                                    clSuites.suiteSz + OPAQUE16_LEN);
            if (ret != 0) return ret;
        }
#endif /* WOLFSSL_DTLS */
        i += clSuites.suiteSz;
        clSuites.hashSigAlgoSz = 0;

        /* compression length */
        b = input[i++];

        if ((i - begin) + b > helloSz)
            return BUFFER_ERROR;

        if (b == 0) {
            WOLFSSL_MSG("No compression types in list");
            return COMPRESSION_ERROR;
        }

#ifdef WOLFSSL_DTLS
        if (IsDtlsNotSctpMode(ssl)) {
            byte newCookie[MAX_COOKIE_LEN];
            int ret;

            ret = wc_HmacUpdate(&cookieHmac, input + i - 1, b + 1);
            if (ret != 0) return ret;
            ret = wc_HmacFinal(&cookieHmac, newCookie);
            if (ret != 0) return ret;

            /* If a cookie callback is set, call it to overwrite the cookie.
             * This should be deprecated. The code now calculates the cookie
             * using an HMAC as expected. */
            if (ssl->ctx->CBIOCookie != NULL &&
                ssl->ctx->CBIOCookie(ssl, newCookie, cookieSz,
                                             ssl->IOCB_CookieCtx) != cookieSz) {
                return COOKIE_ERROR;
            }

            /* Check the cookie, see if we progress the state machine. */
            if (peerCookieSz != cookieSz ||
                XMEMCMP(peerCookie, newCookie, cookieSz) != 0) {

                /* Send newCookie to client in a HelloVerifyRequest message
                 * and let the state machine alone. */
                ssl->msgsReceived.got_client_hello = 0;
                ssl->keys.dtls_handshake_number = 0;
                ssl->keys.dtls_expected_peer_handshake_number = 0;
                *inOutIdx += helloSz;
                return SendHelloVerifyRequest(ssl, newCookie, cookieSz);
            }

            /* This was skipped in the DTLS case so we could handle the hello
             * verify request. */
            ret = HashInput(ssl, input + *inOutIdx, helloSz);
            if (ret != 0) return ret;
        }
#endif /* WOLFSSL_DTLS */

        {
            /* copmression match types */
            int matchNo = 0;
            int matchZlib = 0;

            while (b--) {
                byte comp = input[i++];

                if (comp == NO_COMPRESSION) {
                    matchNo = 1;
                }
                if (comp == ZLIB_COMPRESSION) {
                    matchZlib = 1;
                }
            }

            if (ssl->options.usingCompression == 0 && matchNo) {
                WOLFSSL_MSG("Matched No Compression");
            } else if (ssl->options.usingCompression && matchZlib) {
                WOLFSSL_MSG("Matched zlib Compression");
            } else if (ssl->options.usingCompression && matchNo) {
                WOLFSSL_MSG("Could only match no compression, turning off");
                ssl->options.usingCompression = 0;  /* turn off */
            } else {
                WOLFSSL_MSG("Could not match compression");
                return COMPRESSION_ERROR;
            }
        }

        *inOutIdx = i;

        /* tls extensions */
        if ((i - begin) < helloSz) {
#ifdef HAVE_TLS_EXTENSIONS
        #ifdef HAVE_QSH
            QSH_Init(ssl);
        #endif
            if (TLSX_SupportExtensions(ssl)) {
                int ret = 0;
#else
            if (IsAtLeastTLSv1_2(ssl)) {
#endif
                /* Process the hello extension. Skip unsupported. */
                word16 totalExtSz;

#ifdef HAVE_TLS_EXTENSIONS
                /* auto populate extensions supported unless user defined */
                if ((ret = TLSX_PopulateExtensions(ssl, 1)) != 0)
                    return ret;
#endif

                if ((i - begin) + OPAQUE16_LEN > helloSz)
                    return BUFFER_ERROR;

                ato16(&input[i], &totalExtSz);
                i += OPAQUE16_LEN;

                if ((i - begin) + totalExtSz > helloSz)
                    return BUFFER_ERROR;

#ifdef HAVE_TLS_EXTENSIONS
                /* tls extensions */
                if ((ret = TLSX_Parse(ssl, (byte *) input + i, totalExtSz,
                                      client_hello, &clSuites)))
                    return ret;
    #ifdef WOLFSSL_TLS13
                if (TLSX_Find(ssl->extensions,
                                             TLSX_SUPPORTED_VERSIONS) != NULL) {
                    TLSX_FreeAll(ssl->extensions, ssl->heap);
                    ssl->extensions = NULL;
                    ssl->version.minor = TLSv1_3_MINOR;
                    *inOutIdx = begin;
                    if ((ret = InitHandshakeHashes(ssl)) != 0)
                        return ret;
                    return DoTls13ClientHello(ssl, input, inOutIdx, helloSz);
                }
    #endif
#if defined(HAVE_STUNNEL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
                if((ret=SNI_Callback(ssl)))
                    return ret;
                ssl->options.side = WOLFSSL_SERVER_END;
#endif /*HAVE_STUNNEL*/

                i += totalExtSz;
#else
                while (totalExtSz) {
                    word16 extId, extSz;

                    if (OPAQUE16_LEN + OPAQUE16_LEN > totalExtSz)
                        return BUFFER_ERROR;

                    ato16(&input[i], &extId);
                    i += OPAQUE16_LEN;
                    ato16(&input[i], &extSz);
                    i += OPAQUE16_LEN;

                    if (OPAQUE16_LEN + OPAQUE16_LEN + extSz > totalExtSz)
                        return BUFFER_ERROR;

                    if (extId == HELLO_EXT_SIG_ALGO) {
                        ato16(&input[i], &clSuites.hashSigAlgoSz);
                        i += OPAQUE16_LEN;

                        if (OPAQUE16_LEN + clSuites.hashSigAlgoSz > extSz)
                            return BUFFER_ERROR;

                        XMEMCPY(clSuites.hashSigAlgo, &input[i],
                            min(clSuites.hashSigAlgoSz, HELLO_EXT_SIGALGO_MAX));
                        i += clSuites.hashSigAlgoSz;

                        if (clSuites.hashSigAlgoSz > HELLO_EXT_SIGALGO_MAX)
                            clSuites.hashSigAlgoSz = HELLO_EXT_SIGALGO_MAX;
                    }
#ifdef HAVE_EXTENDED_MASTER
                    else if (extId == HELLO_EXT_EXTMS)
                        ssl->options.haveEMS = 1;
#endif
                    else
                        i += extSz;

                    totalExtSz -= OPAQUE16_LEN + OPAQUE16_LEN + extSz;
                }
#endif
                *inOutIdx = i;
            }
            else
                *inOutIdx = begin + helloSz; /* skip extensions */
        }

        ssl->options.clientState   = CLIENT_HELLO_COMPLETE;
        ssl->options.haveSessionId = 1;

        /* ProcessOld uses same resume code */
        if (ssl->options.resuming) {
            int ret = -1;
            WOLFSSL_SESSION* session = GetSession(ssl,
                                                  ssl->arrays->masterSecret, 1);
            #ifdef HAVE_SESSION_TICKET
                if (ssl->options.useTicket == 1) {
                    session = &ssl->session;
                } else if (bogusID == 1 && ssl->options.rejectTicket == 0) {
                    WOLFSSL_MSG("Bogus session ID without session ticket");
                    return BUFFER_ERROR;
                }
            #endif

            if (!session) {
                WOLFSSL_MSG("Session lookup for resume failed");
                ssl->options.resuming = 0;
            }
            else if (session->haveEMS != ssl->options.haveEMS) {
                /* RFC 7627, 5.3, server-side */
                /* if old sess didn't have EMS, but new does, full handshake */
                if (!session->haveEMS && ssl->options.haveEMS) {
                    WOLFSSL_MSG("Attempting to resume a session that didn't "
                                "use EMS with a new session with EMS. Do full "
                                "handshake.");
                    ssl->options.resuming = 0;
                }
                /* if old sess used EMS, but new doesn't, MUST abort */
                else if (session->haveEMS && !ssl->options.haveEMS) {
                    WOLFSSL_MSG("Trying to resume a session with EMS without "
                                "using EMS");
                    return EXT_MASTER_SECRET_NEEDED_E;
                }
#ifdef HAVE_EXT_CACHE
                wolfSSL_SESSION_free(session);
#endif
            }
            else {
#ifdef HAVE_EXT_CACHE
                wolfSSL_SESSION_free(session);
#endif
                if (MatchSuite(ssl, &clSuites) < 0) {
                    WOLFSSL_MSG("Unsupported cipher suite, ClientHello");
                    return UNSUPPORTED_SUITE;
                }

                ret = wc_RNG_GenerateBlock(ssl->rng, ssl->arrays->serverRandom,
                                                                       RAN_LEN);
                if (ret != 0)
                    return ret;

                #ifdef NO_OLD_TLS
                    ret = DeriveTlsKeys(ssl);
                #else
                    #ifndef NO_TLS
                        if (ssl->options.tls)
                            ret = DeriveTlsKeys(ssl);
                    #endif
                        if (!ssl->options.tls)
                            ret = DeriveKeys(ssl);
                #endif
                ssl->options.clientState = CLIENT_KEYEXCHANGE_COMPLETE;

                return ret;
            }
        }
        return MatchSuite(ssl, &clSuites);
    }


#if !defined(NO_RSA) || defined(HAVE_ECC)

    typedef struct DcvArgs {
        byte*  output; /* not allocated */
        word32 sendSz;
        word16 sz;
        word32 sigSz;
        word32 idx;
        word32 begin;
        byte   hashAlgo;
        byte   sigAlgo;
    } DcvArgs;

    static void FreeDcvArgs(WOLFSSL* ssl, void* pArgs)
    {
        DcvArgs* args = (DcvArgs*)pArgs;

        (void)ssl;
        (void)args;
    }

    static int DoCertificateVerify(WOLFSSL* ssl, byte* input,
                                word32* inOutIdx, word32 size)
    {
        int ret = 0;
    #ifdef WOLFSSL_ASYNC_CRYPT
        DcvArgs* args = (DcvArgs*)ssl->async.args;
        typedef char args_test[sizeof(ssl->async.args) >= sizeof(*args) ? 1 : -1];
        (void)sizeof(args_test);
    #else
        DcvArgs  args[1];
    #endif

        WOLFSSL_ENTER("DoCertificateVerify");

    #ifdef WOLFSSL_ASYNC_CRYPT
        ret = wolfSSL_AsyncPop(ssl, &ssl->options.asyncState);
        if (ret != WC_NOT_PENDING_E) {
            /* Check for error */
            if (ret < 0)
                goto exit_dcv;
        }
        else
    #endif
        {
            /* Reset state */
            ret = 0;
            ssl->options.asyncState = TLS_ASYNC_BEGIN;
            XMEMSET(args, 0, sizeof(DcvArgs));
            args->hashAlgo = sha_mac;
            args->sigAlgo = anonymous_sa_algo;
            args->idx = *inOutIdx;
            args->begin = *inOutIdx;
        #ifdef WOLFSSL_ASYNC_CRYPT
            ssl->async.freeArgs = FreeDcvArgs;
        #endif
        }

        switch(ssl->options.asyncState)
        {
            case TLS_ASYNC_BEGIN:
            {
            #ifdef WOLFSSL_CALLBACKS
                if (ssl->hsInfoOn)
                    AddPacketName("CertificateVerify", &ssl->handShakeInfo);
                if (ssl->toInfoOn)
                    AddLateName("CertificateVerify", &ssl->timeoutInfo);
            #endif

                /* Advance state and proceed */
                ssl->options.asyncState = TLS_ASYNC_BUILD;
            } /* case TLS_ASYNC_BEGIN */
            FALL_THROUGH;

            case TLS_ASYNC_BUILD:
            {
                if (IsAtLeastTLSv1_2(ssl)) {
                    if ((args->idx - args->begin) + ENUM_LEN + ENUM_LEN > size) {
                        ERROR_OUT(BUFFER_ERROR, exit_dcv);
                    }

                    DecodeSigAlg(&input[args->idx], &args->hashAlgo,
                                 &args->sigAlgo);
                    args->idx += 2;
                }
            #ifndef NO_RSA
                else if (ssl->peerRsaKey != NULL && ssl->peerRsaKeyPresent != 0)
                    args->sigAlgo = rsa_sa_algo;
            #endif
            #ifdef HAVE_ECC
                else if (ssl->peerEccDsaKeyPresent)
                    args->sigAlgo = ecc_dsa_sa_algo;
            #endif
            #ifdef HAVE_ED25519
                else if (ssl->peerEd25519KeyPresent)
                    args->sigAlgo = ed25519_sa_algo;
            #endif

                if ((args->idx - args->begin) + OPAQUE16_LEN > size) {
                    ERROR_OUT(BUFFER_ERROR, exit_dcv);
                }

                ato16(input + args->idx, &args->sz);
                args->idx += OPAQUE16_LEN;

                if ((args->idx - args->begin) + args->sz > size ||
                                                    args->sz > ENCRYPT_LEN) {
                    ERROR_OUT(BUFFER_ERROR, exit_dcv);
                }

            #ifdef HAVE_ECC
                if (ssl->peerEccDsaKeyPresent) {

                    WOLFSSL_MSG("Doing ECC peer cert verify");

                /* make sure a default is defined */
                #if !defined(NO_SHA)
                    SetDigest(ssl, sha_mac);
                #elif !defined(NO_SHA256)
                    SetDigest(ssl, sha256_mac);
                #elif defined(WOLFSSL_SHA384)
                    SetDigest(ssl, sha384_mac);
                #elif defined(WOLFSSL_SHA512)
                    SetDigest(ssl, sha512_mac);
                #else
                    #error No digest enabled for ECC sig verify
                #endif

                    if (IsAtLeastTLSv1_2(ssl)) {
                        if (args->sigAlgo != ecc_dsa_sa_algo) {
                            WOLFSSL_MSG("Oops, peer sent ECC key but not in verify");
                        }

                        SetDigest(ssl, args->hashAlgo);
                    }
                }
            #endif /* HAVE_ECC */
            #ifdef HAVE_ED25519
                if (ssl->peerEd25519KeyPresent) {
                    WOLFSSL_MSG("Doing ED25519 peer cert verify");
                    if (IsAtLeastTLSv1_2(ssl) &&
                                             args->sigAlgo != ed25519_sa_algo) {
                        WOLFSSL_MSG(
                               "Oops, peer sent ED25519 key but not in verify");
                    }
                }
            #endif /* HAVE_ED25519 */

                /* Advance state and proceed */
                ssl->options.asyncState = TLS_ASYNC_DO;
            } /* case TLS_ASYNC_BUILD */
            FALL_THROUGH;

            case TLS_ASYNC_DO:
            {
            #ifndef NO_RSA
                if (ssl->peerRsaKey != NULL && ssl->peerRsaKeyPresent != 0) {
                    WOLFSSL_MSG("Doing RSA peer cert verify");

                    ret = RsaVerify(ssl,
                        input + args->idx,
                        args->sz,
                        &args->output,
                        args->sigAlgo, args->hashAlgo,
                        ssl->peerRsaKey,
                    #ifdef HAVE_PK_CALLBACKS
                        ssl->buffers.peerRsaKey.buffer,
                        ssl->buffers.peerRsaKey.length,
                        ssl->RsaVerifyCtx
                    #else
                        NULL, 0, NULL
                    #endif
                    );
                    if (ret >= 0) {
                        if (args->sigAlgo == rsa_sa_algo)
                            args->sendSz = ret;
                        else {
                            args->sigSz = ret;
                            args->sendSz = ssl->buffers.digest.length;
                        }
                        ret = 0;
                    }
                }
            #endif /* !NO_RSA */
            #ifdef HAVE_ECC
                if (ssl->peerEccDsaKeyPresent) {
                    WOLFSSL_MSG("Doing ECC peer cert verify");

                    ret = EccVerify(ssl,
                        input + args->idx, args->sz,
                        ssl->buffers.digest.buffer, ssl->buffers.digest.length,
                        ssl->peerEccDsaKey,
                    #ifdef HAVE_PK_CALLBACKS
                        ssl->buffers.peerEccDsaKey.buffer,
                        ssl->buffers.peerEccDsaKey.length,
                        ssl->EccVerifyCtx
                    #else
                        NULL, 0, NULL
                    #endif
                    );
                }
            #endif /* HAVE_ECC */

                /* Check for error */
                if (ret != 0) {
                    goto exit_dcv;
                }

                /* Advance state and proceed */
                ssl->options.asyncState = TLS_ASYNC_VERIFY;
            } /* case TLS_ASYNC_DO */
            FALL_THROUGH;

            case TLS_ASYNC_VERIFY:
            {
            #ifndef NO_RSA
                if (ssl->peerRsaKey != NULL && ssl->peerRsaKeyPresent != 0) {
                    if (IsAtLeastTLSv1_2(ssl)) {
                    #ifdef WC_RSA_PSS
                        if (args->sigAlgo == rsa_pss_sa_algo) {
                            SetDigest(ssl, args->hashAlgo);

                            ret = wc_RsaPSS_CheckPadding(
                                             ssl->buffers.digest.buffer,
                                             ssl->buffers.digest.length,
                                             args->output, args->sigSz,
                                             HashAlgoToType(args->hashAlgo));
                            if (ret != 0)
                                return ret;
                        }
                        else
                    #endif
                        {
                        #ifdef WOLFSSL_SMALL_STACK
                            byte* encodedSig = NULL;
                        #else
                            byte  encodedSig[MAX_ENCODED_SIG_SZ];
                        #endif

                        #ifdef WOLFSSL_SMALL_STACK
                            encodedSig = (byte*)XMALLOC(MAX_ENCODED_SIG_SZ,
                                                ssl->heap, DYNAMIC_TYPE_SIGNATURE);
                            if (encodedSig == NULL) {
                                ERROR_OUT(MEMORY_E, exit_dcv);
                            }
                        #endif

                            if (args->sigAlgo != rsa_sa_algo) {
                                WOLFSSL_MSG("Oops, peer sent RSA key but not in verify");
                            }

                            SetDigest(ssl, args->hashAlgo);

                            args->sigSz = wc_EncodeSignature(encodedSig,
                                ssl->buffers.digest.buffer,
                                ssl->buffers.digest.length,
                                TypeHash(args->hashAlgo));

                            if (args->sendSz != args->sigSz || !args->output ||
                                XMEMCMP(args->output, encodedSig,
                                    min(args->sigSz, MAX_ENCODED_SIG_SZ)) != 0) {
                                ret = VERIFY_CERT_ERROR;
                            }

                        #ifdef WOLFSSL_SMALL_STACK
                            XFREE(encodedSig, ssl->heap, DYNAMIC_TYPE_SIGNATURE);
                        #endif
                        }
                    }
                    else {
                        if (args->sendSz != FINISHED_SZ || !args->output ||
                            XMEMCMP(args->output,
                                &ssl->hsHashes->certHashes, FINISHED_SZ) != 0) {
                            ret = VERIFY_CERT_ERROR;
                        }
                    }
                }
            #endif /* !NO_RSA */

                /* Advance state and proceed */
                ssl->options.asyncState = TLS_ASYNC_FINALIZE;
            } /* case TLS_ASYNC_VERIFY */
            FALL_THROUGH;

            case TLS_ASYNC_FINALIZE:
            {
                ssl->options.havePeerVerify = 1;

                /* Set final index */
                args->idx += args->sz;
                *inOutIdx = args->idx;

                /* Advance state and proceed */
                ssl->options.asyncState = TLS_ASYNC_END;
            } /* case TLS_ASYNC_FINALIZE */

            case TLS_ASYNC_END:
            {
                break;
            }
            default:
                ret = INPUT_CASE_ERROR;
        } /* switch(ssl->options.asyncState) */

    exit_dcv:

        WOLFSSL_LEAVE("DoCertificateVerify", ret);

    #ifdef WOLFSSL_ASYNC_CRYPT
        /* Handle async operation */
        if (ret == WC_PENDING_E) {
            /* Mark message as not recevied so it can process again */
            ssl->msgsReceived.got_certificate_verify = 0;

            return ret;
        }
    #endif /* WOLFSSL_ASYNC_CRYPT */

        /* Digest is not allocated, so do this to prevent free */
        ssl->buffers.digest.buffer = NULL;
        ssl->buffers.digest.length = 0;

        /* Final cleanup */
        FreeDcvArgs(ssl, args);
        FreeKeyExchange(ssl);

        return ret;
    }

#endif /* !NO_RSA || HAVE_ECC */

    int SendServerHelloDone(WOLFSSL* ssl)
    {
        byte* output;
        int   sendSz = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;
        int   ret;

    #ifdef WOLFSSL_DTLS
        if (ssl->options.dtls)
            sendSz += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
    #endif

        /* check for available size */
        if ((ret = CheckAvailableSize(ssl, sendSz)) != 0)
            return ret;

        /* get output buffer */
        output = ssl->buffers.outputBuffer.buffer +
                 ssl->buffers.outputBuffer.length;

        AddHeaders(output, 0, server_hello_done, ssl);

    #ifdef WOLFSSL_DTLS
        if (IsDtlsNotSctpMode(ssl)) {
            if ((ret = DtlsMsgPoolSave(ssl, output, sendSz)) != 0)
                return 0;
        }

        if (ssl->options.dtls)
            DtlsSEQIncrement(ssl, CUR_ORDER);
    #endif

        ret = HashOutput(ssl, output, sendSz, 0);
            if (ret != 0)
                return ret;

    #ifdef WOLFSSL_CALLBACKS
        if (ssl->hsInfoOn)
            AddPacketName("ServerHelloDone", &ssl->handShakeInfo);
        if (ssl->toInfoOn)
            AddPacketInfo("ServerHelloDone", &ssl->timeoutInfo, output, sendSz,
                          ssl->heap);
    #endif
        ssl->options.serverState = SERVER_HELLODONE_COMPLETE;

        ssl->buffers.outputBuffer.length += sendSz;

        return SendBuffered(ssl);
    }


#ifdef HAVE_SESSION_TICKET

#define WOLFSSL_TICKET_FIXED_SZ (WOLFSSL_TICKET_NAME_SZ + \
                WOLFSSL_TICKET_IV_SZ + WOLFSSL_TICKET_MAC_SZ + LENGTH_SZ)
#define WOLFSSL_TICKET_ENC_SZ (SESSION_TICKET_LEN - WOLFSSL_TICKET_FIXED_SZ)

    /* our ticket format */
    typedef struct InternalTicket {
        ProtocolVersion pv;                    /* version when ticket created */
        byte            suite[SUITE_LEN];      /* cipher suite when created */
        byte            msecret[SECRET_LEN];   /* master secret */
        word32          timestamp;             /* born on */
        word16          haveEMS;               /* have extended master secret */
#ifdef WOLFSSL_TLS13
        word32          ageAdd;                /* Obfuscation of age */
        byte            namedGroup;            /* Named group used */
    #ifdef WOLFSSL_EARLY_DATA
        word32          maxEarlyDataSz;        /* Max size of early data */
    #endif
#endif
    } InternalTicket;

    /* fit within SESSION_TICKET_LEN */
    typedef struct ExternalTicket {
        byte key_name[WOLFSSL_TICKET_NAME_SZ];  /* key context name */
        byte iv[WOLFSSL_TICKET_IV_SZ];          /* this ticket's iv */
        byte enc_len[LENGTH_SZ];                /* encrypted length */
        byte enc_ticket[WOLFSSL_TICKET_ENC_SZ]; /* encrypted internal ticket */
        byte mac[WOLFSSL_TICKET_MAC_SZ];        /* total mac */
        /* !! if add to structure, add to TICKET_FIXED_SZ !! */
    } ExternalTicket;

    /* create a new session ticket, 0 on success */
    int CreateTicket(WOLFSSL* ssl)
    {
        InternalTicket  it;
        ExternalTicket* et = (ExternalTicket*)ssl->session.ticket;
        int encLen;
        int ret;
        byte zeros[WOLFSSL_TICKET_MAC_SZ];   /* biggest cmp size */

        XMEMSET(&it, 0, sizeof(it));

        /* build internal */
        it.pv.major = ssl->version.major;
        it.pv.minor = ssl->version.minor;

        it.suite[0] = ssl->options.cipherSuite0;
        it.suite[1] = ssl->options.cipherSuite;

    #ifdef WOLFSSL_EARLY_DATA
        it.maxEarlyDataSz = ssl->options.maxEarlyDataSz;
    #endif

        if (!ssl->options.tls1_3) {
            XMEMCPY(it.msecret, ssl->arrays->masterSecret, SECRET_LEN);
            c32toa(LowResTimer(), (byte*)&it.timestamp);
            it.haveEMS = ssl->options.haveEMS;
        }
        else {
#ifdef WOLFSSL_TLS13
            /* Client adds to ticket age to obfuscate. */
            ret = wc_RNG_GenerateBlock(ssl->rng, (byte*)&it.ageAdd,
                                       sizeof(it.ageAdd));
            if (ret != 0)
                return BAD_TICKET_ENCRYPT;
            ssl->session.ticketAdd = it.ageAdd;
            it.namedGroup = ssl->session.namedGroup;
            it.timestamp = TimeNowInMilliseconds();
            /* Resumption master secret. */
            XMEMCPY(it.msecret, ssl->session.masterSecret, SECRET_LEN);
#endif
        }

        /* build external */
        XMEMCPY(et->enc_ticket, &it, sizeof(InternalTicket));

        /* encrypt */
        encLen = WOLFSSL_TICKET_ENC_SZ;  /* max size user can use */
        ret = ssl->ctx->ticketEncCb(ssl, et->key_name, et->iv, et->mac, 1,
                                    et->enc_ticket, sizeof(InternalTicket),
                                    &encLen, ssl->ctx->ticketEncCtx);
        if (ret == WOLFSSL_TICKET_RET_OK) {
            if (encLen < (int)sizeof(InternalTicket) ||
                encLen > WOLFSSL_TICKET_ENC_SZ) {
                WOLFSSL_MSG("Bad user ticket encrypt size");
                return BAD_TICKET_KEY_CB_SZ;
            }

            /* sanity checks on encrypt callback */

            /* internal ticket can't be the same if encrypted */
            if (XMEMCMP(et->enc_ticket, &it, sizeof(InternalTicket)) == 0) {
                WOLFSSL_MSG("User ticket encrypt didn't encrypt");
                return BAD_TICKET_ENCRYPT;
            }

            XMEMSET(zeros, 0, sizeof(zeros));

            /* name */
            if (XMEMCMP(et->key_name, zeros, WOLFSSL_TICKET_NAME_SZ) == 0) {
                WOLFSSL_MSG("User ticket encrypt didn't set name");
                return BAD_TICKET_ENCRYPT;
            }

            /* iv */
            if (XMEMCMP(et->iv, zeros, WOLFSSL_TICKET_IV_SZ) == 0) {
                WOLFSSL_MSG("User ticket encrypt didn't set iv");
                return BAD_TICKET_ENCRYPT;
            }

            /* mac */
            if (XMEMCMP(et->mac, zeros, WOLFSSL_TICKET_MAC_SZ) == 0) {
                WOLFSSL_MSG("User ticket encrypt didn't set mac");
                return BAD_TICKET_ENCRYPT;
            }

            /* set size */
            c16toa((word16)encLen, et->enc_len);
            ssl->session.ticketLen = (word16)(encLen + WOLFSSL_TICKET_FIXED_SZ);
            if (encLen < WOLFSSL_TICKET_ENC_SZ) {
                /* move mac up since whole enc buffer not used */
                XMEMMOVE(et->enc_ticket +encLen, et->mac,WOLFSSL_TICKET_MAC_SZ);
            }
        }

        return ret;
    }


    /* Parse ticket sent by client, returns callback return value */
    int DoClientTicket(WOLFSSL* ssl, const byte* input, word32 len)
    {
        ExternalTicket* et;
        InternalTicket* it;
        int             ret;
        int             outLen;
        word16          inLen;

        if (len > SESSION_TICKET_LEN ||
             len < (word32)(sizeof(InternalTicket) + WOLFSSL_TICKET_FIXED_SZ)) {
            return BAD_TICKET_MSG_SZ;
        }

        et = (ExternalTicket*)input;
        it = (InternalTicket*)et->enc_ticket;

        /* decrypt */
        ato16(et->enc_len, &inLen);
        if (inLen > (word16)(len - WOLFSSL_TICKET_FIXED_SZ)) {
            return BAD_TICKET_MSG_SZ;
        }
        outLen = inLen;   /* may be reduced by user padding */
        ret = ssl->ctx->ticketEncCb(ssl, et->key_name, et->iv,
                                    et->enc_ticket + inLen, 0,
                                    et->enc_ticket, inLen, &outLen,
                                    ssl->ctx->ticketEncCtx);
        if (ret == WOLFSSL_TICKET_RET_FATAL || ret < 0) return ret;
        if (outLen > inLen || outLen < (int)sizeof(InternalTicket)) {
            WOLFSSL_MSG("Bad user ticket decrypt len");
            return BAD_TICKET_KEY_CB_SZ;
        }

        /* get master secret */
        if (ret == WOLFSSL_TICKET_RET_OK || ret == WOLFSSL_TICKET_RET_CREATE) {
            if (!IsAtLeastTLSv1_3(ssl->version)) {
                XMEMCPY(ssl->arrays->masterSecret, it->msecret, SECRET_LEN);
                /* Copy the haveExtendedMasterSecret property from the ticket to
                 * the saved session, so the property may be checked later. */
                ssl->session.haveEMS = it->haveEMS;
            }
            else {
#ifdef WOLFSSL_TLS13
                /* Restore information to renegotiate. */
                ssl->session.ticketSeen = it->timestamp;
                ssl->session.ticketAdd = it->ageAdd;
                ssl->session.cipherSuite0 = it->suite[0];
                ssl->session.cipherSuite = it->suite[1];
    #ifdef WOLFSSL_EARLY_DATA
                ssl->session.maxEarlyDataSz = it->maxEarlyDataSz;
    #endif
                /* Resumption master secret. */
                XMEMCPY(ssl->session.masterSecret, it->msecret, SECRET_LEN);
                ssl->session.namedGroup = it->namedGroup;
#endif
            }
        }

        return ret;
    }


    /* send Session Ticket */
    int SendTicket(WOLFSSL* ssl)
    {
        byte*              output;
        int                ret;
        int                sendSz;
        word32             length = SESSION_HINT_SZ + LENGTH_SZ;
        word32             idx    = RECORD_HEADER_SZ + HANDSHAKE_HEADER_SZ;

        if (ssl->options.createTicket) {
            ret = CreateTicket(ssl);
            if (ret != 0) return ret;
        }

        length += ssl->session.ticketLen;
        sendSz = length + HANDSHAKE_HEADER_SZ + RECORD_HEADER_SZ;

        #ifdef WOLFSSL_DTLS
        if (ssl->options.dtls) {
            sendSz += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
            idx    += DTLS_RECORD_EXTRA + DTLS_HANDSHAKE_EXTRA;
        }
        #endif
        /* check for available size */
        if ((ret = CheckAvailableSize(ssl, sendSz)) != 0)
            return ret;

        /* get output buffer */
        output = ssl->buffers.outputBuffer.buffer +
                 ssl->buffers.outputBuffer.length;

        AddHeaders(output, length, session_ticket, ssl);

        /* hint */
        c32toa(ssl->ctx->ticketHint, output + idx);
        idx += SESSION_HINT_SZ;

        /* length */
        c16toa(ssl->session.ticketLen, output + idx);
        idx += LENGTH_SZ;

        /* ticket */
        XMEMCPY(output + idx, ssl->session.ticket, ssl->session.ticketLen);
        /* idx += ssl->session.ticketLen; */

        #ifdef WOLFSSL_DTLS
        if (ssl->options.dtls) {
            if ((ret = DtlsMsgPoolSave(ssl, output, sendSz)) != 0)
                return ret;

            DtlsSEQIncrement(ssl, CUR_ORDER);
        }
        #endif

        ret = HashOutput(ssl, output, sendSz, 0);
        if (ret != 0) return ret;
        ssl->buffers.outputBuffer.length += sendSz;

        return SendBuffered(ssl);
    }

#endif /* HAVE_SESSION_TICKET */


#ifdef WOLFSSL_DTLS
    static int SendHelloVerifyRequest(WOLFSSL* ssl,
                                      const byte* cookie, byte cookieSz)
    {
        byte* output;
        int   length = VERSION_SZ + ENUM_LEN + cookieSz;
        int   idx    = DTLS_RECORD_HEADER_SZ + DTLS_HANDSHAKE_HEADER_SZ;
        int   sendSz = length + idx;
        int   ret;

        /* check for available size */
        if ((ret = CheckAvailableSize(ssl, sendSz)) != 0)
            return ret;

        /* get output buffer */
        output = ssl->buffers.outputBuffer.buffer +
                 ssl->buffers.outputBuffer.length;

        /* Hello Verify Request should use the same sequence number as the
         * Client Hello. */
        ssl->keys.dtls_sequence_number_hi = ssl->keys.curSeq_hi;
        ssl->keys.dtls_sequence_number_lo = ssl->keys.curSeq_lo;
        AddHeaders(output, length, hello_verify_request, ssl);

#ifdef OPENSSL_EXTRA
        output[idx++] = DTLS_MAJOR;
        output[idx++] = DTLS_MINOR;
#else
        output[idx++] = ssl->version.major;
        output[idx++] = ssl->version.minor;
#endif

        output[idx++] = cookieSz;
        if (cookie == NULL || cookieSz == 0)
            return COOKIE_ERROR;

        XMEMCPY(output + idx, cookie, cookieSz);

#ifdef WOLFSSL_CALLBACKS
        if (ssl->hsInfoOn)
            AddPacketName("HelloVerifyRequest", &ssl->handShakeInfo);
        if (ssl->toInfoOn)
            AddPacketInfo("HelloVerifyRequest", &ssl->timeoutInfo, output,
                          sendSz, ssl->heap);
#endif

        ssl->buffers.outputBuffer.length += sendSz;

        return SendBuffered(ssl);
    }
#endif /* WOLFSSL_DTLS */

    typedef struct DckeArgs {
        byte*  output; /* not allocated */
        word32 length;
        word32 idx;
        word32 begin;
        word32 sigSz;
    } DckeArgs;

    static void FreeDckeArgs(WOLFSSL* ssl, void* pArgs)
    {
        DckeArgs* args = (DckeArgs*)pArgs;

        (void)ssl;
        (void)args;
    }

    static int DoClientKeyExchange(WOLFSSL* ssl, byte* input, word32* inOutIdx,
                                                                    word32 size)
    {
        int ret;
    #ifdef WOLFSSL_ASYNC_CRYPT
        DckeArgs* args = (DckeArgs*)ssl->async.args;
        typedef char args_test[sizeof(ssl->async.args) >= sizeof(*args) ? 1 : -1];
        (void)sizeof(args_test);
    #else
        DckeArgs  args[1];
    #endif

        WOLFSSL_ENTER("DoClientKeyExchange");

    #ifdef WOLFSSL_ASYNC_CRYPT
        ret = wolfSSL_AsyncPop(ssl, &ssl->options.asyncState);
        if (ret != WC_NOT_PENDING_E) {
            /* Check for error */
            if (ret < 0)
                goto exit_dcke;
        }
        else
    #endif /* WOLFSSL_ASYNC_CRYPT */
        {
            /* Reset state */
            ret = 0;
            ssl->options.asyncState = TLS_ASYNC_BEGIN;
            XMEMSET(args, 0, sizeof(DckeArgs));
            args->idx = *inOutIdx;
            args->begin = *inOutIdx;
        #ifdef WOLFSSL_ASYNC_CRYPT
            ssl->async.freeArgs = FreeDckeArgs;
        #endif
        }

        /* Do Client Key Exchange State Machine */
        switch(ssl->options.asyncState)
        {
            case TLS_ASYNC_BEGIN:
            {
                /* Sanity checks */
                if (ssl->options.side != WOLFSSL_SERVER_END) {
                    WOLFSSL_MSG("Client received client keyexchange, attack?");
                    WOLFSSL_ERROR(ssl->error = SIDE_ERROR);
                    ERROR_OUT(SSL_FATAL_ERROR, exit_dcke);
                }

                if (ssl->options.clientState < CLIENT_HELLO_COMPLETE) {
                    WOLFSSL_MSG("Client sending keyexchange at wrong time");
                    SendAlert(ssl, alert_fatal, unexpected_message);
                    ERROR_OUT(OUT_OF_ORDER_E, exit_dcke);
                }

            #ifndef NO_CERTS
                if (ssl->options.verifyPeer && ssl->options.failNoCert) {
                    if (!ssl->options.havePeerCert) {
                        WOLFSSL_MSG("client didn't present peer cert");
                        ERROR_OUT(NO_PEER_CERT, exit_dcke);
                    }
                }

                if (ssl->options.verifyPeer && ssl->options.failNoCertxPSK) {
                    if (!ssl->options.havePeerCert &&
                                             !ssl->options.usingPSK_cipher) {
                        WOLFSSL_MSG("client didn't present peer cert");
                        return NO_PEER_CERT;
                    }
                }
            #endif /* !NO_CERTS */

            #ifdef WOLFSSL_CALLBACKS
                if (ssl->hsInfoOn) {
                    AddPacketName("ClientKeyExchange", &ssl->handShakeInfo);
                }
                if (ssl->toInfoOn) {
                    AddLateName("ClientKeyExchange", &ssl->timeoutInfo);
                }
            #endif

                switch (ssl->specs.kea) {
                #ifndef NO_RSA
                    case rsa_kea:
                    {
                        /* make sure private key exists */
                        if (ssl->buffers.key == NULL ||
                                            ssl->buffers.key->buffer == NULL) {
                            ERROR_OUT(NO_PRIVATE_KEY, exit_dcke);
                        }
                        break;
                    } /* rsa_kea */
                #endif /* !NO_RSA */
                #ifndef NO_PSK
                    case psk_kea:
                    {
                        /* sanity check that PSK server callback has been set */
                        if (ssl->options.server_psk_cb == NULL) {
                           WOLFSSL_MSG("No server PSK callback set");
                           ERROR_OUT(PSK_KEY_ERROR, exit_dcke);
                        }
                        break;
                    }
                #endif /* !NO_PSK */
                #ifdef HAVE_NTRU
                    case ntru_kea:
                    {
                        /* make sure private key exists */
                        if (ssl->buffers.key == NULL ||
                                            ssl->buffers.key->buffer == NULL) {
                            ERROR_OUT(NO_PRIVATE_KEY, exit_dcke);
                        }
                        break;
                    }
                #endif /* HAVE_NTRU */
                #ifdef HAVE_ECC
                    case ecc_diffie_hellman_kea:
                    {
                        break;
                    }
                #endif /* HAVE_ECC */
                #ifndef NO_DH
                    case diffie_hellman_kea:
                    {
                        break;
                    }
                #endif /* !NO_DH */
                #if !defined(NO_DH) && !defined(NO_PSK)
                    case dhe_psk_kea:
                    {
                        /* sanity check that PSK server callback has been set */
                        if (ssl->options.server_psk_cb == NULL) {
                            WOLFSSL_MSG("No server PSK callback set");
                            ERROR_OUT(PSK_KEY_ERROR, exit_dcke);
                        }
                        break;
                    }
                #endif /* !NO_DH && !NO_PSK */
                #if defined(HAVE_ECC) && !defined(NO_PSK)
                    case ecdhe_psk_kea:
                    {
                        /* sanity check that PSK server callback has been set */
                        if (ssl->options.server_psk_cb == NULL) {
                            WOLFSSL_MSG("No server PSK callback set");
                            ERROR_OUT(PSK_KEY_ERROR, exit_dcke);
                        }
                        break;
                    }
                #endif /* HAVE_ECC && !NO_PSK */
                    default:
                        WOLFSSL_MSG("Bad kea type");
                        ret = BAD_KEA_TYPE_E;
                } /* switch (ssl->specs.kea) */

                /* Check for error */
                if (ret != 0) {
                    goto exit_dcke;
                }

                /* Advance state and proceed */
                ssl->options.asyncState = TLS_ASYNC_BUILD;
            } /* TLS_ASYNC_BEGIN */
            FALL_THROUGH;

            case TLS_ASYNC_BUILD:
            {
                switch (ssl->specs.kea) {
                #ifndef NO_RSA
                    case rsa_kea:
                    {
                        word32 i = 0;
                        int    keySz;

                        ssl->hsType = DYNAMIC_TYPE_RSA;
                        ret = AllocKey(ssl, ssl->hsType, &ssl->hsKey);
                        if (ret != 0) {
                            goto exit_dcke;
                        }

                        ret = wc_RsaPrivateKeyDecode(ssl->buffers.key->buffer,
                            &i, (RsaKey*)ssl->hsKey, ssl->buffers.key->length);
                        if (ret != 0) {
                            goto exit_dcke;
                        }
                        keySz = wc_RsaEncryptSize((RsaKey*)ssl->hsKey);
                        if (keySz < 0) { /* test if keySz has error */
                            ERROR_OUT(keySz, exit_dcke);
                        }
                        args->length = (word32)keySz;

                        if (keySz < ssl->options.minRsaKeySz) {
                            WOLFSSL_MSG("Peer RSA key is too small");
                            ERROR_OUT(RSA_KEY_SIZE_E, exit_dcke);
                        }
                        ssl->arrays->preMasterSz = SECRET_LEN;

                        if (ssl->options.tls) {
                            word16 check;

                            if ((args->idx - args->begin) + OPAQUE16_LEN > size) {
                                ERROR_OUT(BUFFER_ERROR, exit_dcke);
                            }

                            ato16(input + args->idx, &check);
                            args->idx += OPAQUE16_LEN;

                            if ((word32)check != args->length) {
                                WOLFSSL_MSG("RSA explicit size doesn't match");
                                ERROR_OUT(RSA_PRIVATE_ERROR, exit_dcke);
                            }
                        }

                        if ((args->idx - args->begin) + args->length > size) {
                            WOLFSSL_MSG("RSA message too big");
                            ERROR_OUT(BUFFER_ERROR, exit_dcke);
                        }

                        args->output = NULL;
                        break;
                    } /* rsa_kea */
                #endif /* !NO_RSA */
                #ifndef NO_PSK
                    case psk_kea:
                    {
                        byte* pms = ssl->arrays->preMasterSecret;
                        word16 ci_sz;

                        if ((args->idx - args->begin) + OPAQUE16_LEN > size) {
                            ERROR_OUT(BUFFER_ERROR, exit_dcke);
                        }

                        ato16(input + args->idx, &ci_sz);
                        args->idx += OPAQUE16_LEN;

                        if (ci_sz > MAX_PSK_ID_LEN) {
                            ERROR_OUT(CLIENT_ID_ERROR, exit_dcke);
                        }

                        if ((args->idx - args->begin) + ci_sz > size) {
                            ERROR_OUT(BUFFER_ERROR, exit_dcke);
                        }

                        XMEMCPY(ssl->arrays->client_identity,
                                                    input + args->idx, ci_sz);
                        args->idx += ci_sz;

                        ssl->arrays->client_identity[ci_sz] = '\0'; /* null term */
                        ssl->arrays->psk_keySz = ssl->options.server_psk_cb(ssl,
                            ssl->arrays->client_identity, ssl->arrays->psk_key,
                            MAX_PSK_KEY_LEN);

                        if (ssl->arrays->psk_keySz == 0 ||
                                ssl->arrays->psk_keySz > MAX_PSK_KEY_LEN) {
                            ERROR_OUT(PSK_KEY_ERROR, exit_dcke);
                        }

                        /* make psk pre master secret */
                        /* length of key + length 0s + length of key + key */
                        c16toa((word16) ssl->arrays->psk_keySz, pms);
                        pms += OPAQUE16_LEN;

                        XMEMSET(pms, 0, ssl->arrays->psk_keySz);
                        pms += ssl->arrays->psk_keySz;

                        c16toa((word16) ssl->arrays->psk_keySz, pms);
                        pms += OPAQUE16_LEN;

                        XMEMCPY(pms, ssl->arrays->psk_key, ssl->arrays->psk_keySz);
                        ssl->arrays->preMasterSz =
                            (ssl->arrays->psk_keySz * 2) + (OPAQUE16_LEN * 2);
                        break;
                    }
                #endif /* !NO_PSK */
                #ifdef HAVE_NTRU
                    case ntru_kea:
                    {
                        word16 cipherLen;
                        word16 plainLen = ENCRYPT_LEN;

                        if ((args->idx - args->begin) + OPAQUE16_LEN > size) {
                            ERROR_OUT(BUFFER_ERROR, exit_dcke);
                        }

                        ato16(input + args->idx, &cipherLen);
                        args->idx += OPAQUE16_LEN;

                        if (cipherLen > MAX_NTRU_ENCRYPT_SZ) {
                            ERROR_OUT(NTRU_KEY_ERROR, exit_dcke);
                        }

                        if ((args->idx - args->begin) + cipherLen > size) {
                            ERROR_OUT(BUFFER_ERROR, exit_dcke);
                        }

                        if (NTRU_OK != ntru_crypto_ntru_decrypt(
                                    (word16) ssl->buffers.key->length,
                                    ssl->buffers.key->buffer, cipherLen,
                                    input + args->idx, &plainLen,
                                    ssl->arrays->preMasterSecret)) {
                            ERROR_OUT(NTRU_DECRYPT_ERROR, exit_dcke);
                        }

                        if (plainLen != SECRET_LEN) {
                            ERROR_OUT(NTRU_DECRYPT_ERROR, exit_dcke);
                        }

                        args->idx += cipherLen;
                        ssl->arrays->preMasterSz = plainLen;
                        break;
                    }
                #endif /* HAVE_NTRU */
                #if defined(HAVE_ECC) || defined(HAVE_CURVE25519)
                    case ecc_diffie_hellman_kea:
                    {
                    #ifdef HAVE_ECC
                        ecc_key* private_key = ssl->eccTempKey;

                        /* handle static private key */
                        if (ssl->specs.static_ecdh &&
                                          ssl->ecdhCurveOID != ECC_X25519_OID) {
                            word32 i = 0;

                            ssl->hsType = DYNAMIC_TYPE_ECC;
                            ret = AllocKey(ssl, ssl->hsType, &ssl->hsKey);
                            if (ret != 0) {
                                goto exit_dcke;
                            }

                            ret = wc_EccPrivateKeyDecode(
                                ssl->buffers.key->buffer,
                                &i,
                                (ecc_key*)ssl->hsKey,
                                ssl->buffers.key->length);
                            if (ret == 0) {
                                private_key = (ecc_key*)ssl->hsKey;
                                if (wc_ecc_size(private_key) <
                                                ssl->options.minEccKeySz) {
                                    WOLFSSL_MSG("ECC key too small");
                                    ERROR_OUT(ECC_KEY_SIZE_E, exit_dcke);
                                }
                            }
                        }
                    #endif

                        /* import peer ECC key */
                        if ((args->idx - args->begin) + OPAQUE8_LEN > size) {
                            ERROR_OUT(BUFFER_ERROR, exit_dcke);
                        }

                        args->length = input[args->idx++];

                        if ((args->idx - args->begin) + args->length > size) {
                            ERROR_OUT(BUFFER_ERROR, exit_dcke);
                        }

                        ssl->arrays->preMasterSz = ENCRYPT_LEN;

                    #ifdef HAVE_CURVE25519
                        if (ssl->ecdhCurveOID == ECC_X25519_OID) {
                        #ifdef HAVE_PK_CALLBACKS
                            /* if callback then use it for shared secret */
                            if (ssl->ctx->X25519SharedSecretCb != NULL) {
                                break;
                            }
                        #endif
                            if (ssl->peerX25519Key == NULL) {
                                /* alloc/init on demand */
                                ret = AllocKey(ssl, DYNAMIC_TYPE_CURVE25519,
                                    (void**)&ssl->peerX25519Key);
                                if (ret != 0) {
                                    goto exit_dcke;
                                }
                            } else if (ssl->peerX25519KeyPresent) {
                                ret = ReuseKey(ssl, DYNAMIC_TYPE_CURVE25519,
                                               ssl->peerX25519Key);
                                ssl->peerX25519KeyPresent = 0;
                                if (ret != 0) {
                                    goto exit_dcke;
                                }
                            }

                            if (wc_curve25519_import_public_ex(
                                    input + args->idx, args->length,
                                    ssl->peerX25519Key,
                                    EC25519_LITTLE_ENDIAN)) {
                                ERROR_OUT(ECC_PEERKEY_ERROR, exit_dcke);
                            }

                            ssl->peerX25519KeyPresent = 1;

                            if (ret != 0) {
                                goto exit_dcke;
                            }
                            break;
                        }
                    #endif
                #ifdef HAVE_ECC
                    #ifdef HAVE_PK_CALLBACKS
                        /* if callback then use it for shared secret */
                        if (ssl->ctx->EccSharedSecretCb != NULL) {
                            break;
                        }
                    #endif

                        if (!ssl->specs.static_ecdh &&
                            ssl->eccTempKeyPresent == 0) {
                            WOLFSSL_MSG("Ecc ephemeral key not made correctly");
                            ERROR_OUT(ECC_MAKEKEY_ERROR, exit_dcke);
                        }

                        if (ssl->peerEccKey == NULL) {
                            /* alloc/init on demand */
                            ret = AllocKey(ssl, DYNAMIC_TYPE_ECC,
                                (void**)&ssl->peerEccKey);
                            if (ret != 0) {
                                goto exit_dcke;
                            }
                        } else if (ssl->peerEccKeyPresent) {
                            ret = ReuseKey(ssl, DYNAMIC_TYPE_ECC,
                                           ssl->peerEccKey);
                            ssl->peerEccKeyPresent = 0;
                            if (ret != 0) {
                                goto exit_dcke;
                            }
                        }

                        if (wc_ecc_import_x963_ex(input + args->idx, args->length,
                                        ssl->peerEccKey, private_key->dp->id)) {
                            ERROR_OUT(ECC_PEERKEY_ERROR, exit_dcke);
                        }

                        ssl->peerEccKeyPresent = 1;
                #endif /* HAVE_ECC */

                        if (ret != 0) {
                            goto exit_dcke;
                        }
                        break;
                    }
                #endif /* HAVE_ECC || HAVE_CURVE25519 */
                #ifndef NO_DH
                    case diffie_hellman_kea:
                    {
                        word16 clientPubSz;

                        if ((args->idx - args->begin) + OPAQUE16_LEN > size) {
                            ERROR_OUT(BUFFER_ERROR, exit_dcke);
                        }

                        ato16(input + args->idx, &clientPubSz);
                        args->idx += OPAQUE16_LEN;

                        if ((args->idx - args->begin) + clientPubSz > size) {
                            ERROR_OUT(BUFFER_ERROR, exit_dcke);
                        }

                        args->sigSz = clientPubSz;

                        ret = AllocKey(ssl, DYNAMIC_TYPE_DH,
                                            (void**)&ssl->buffers.serverDH_Key);
                        if (ret != 0) {
                            goto exit_dcke;
                        }

                        ret = wc_DhSetKey(ssl->buffers.serverDH_Key,
                            ssl->buffers.serverDH_P.buffer,
                            ssl->buffers.serverDH_P.length,
                            ssl->buffers.serverDH_G.buffer,
                            ssl->buffers.serverDH_G.length);

                        /* set the max agree result size */
                        ssl->arrays->preMasterSz = ENCRYPT_LEN;
                        break;
                    }
                #endif /* !NO_DH */
                #if !defined(NO_DH) && !defined(NO_PSK)
                    case dhe_psk_kea:
                    {
                        word16 clientSz;

                        /* Read in the PSK hint */
                        if ((args->idx - args->begin) + OPAQUE16_LEN > size) {
                            ERROR_OUT(BUFFER_ERROR, exit_dcke);
                        }

                        ato16(input + args->idx, &clientSz);
                        args->idx += OPAQUE16_LEN;
                        if (clientSz > MAX_PSK_ID_LEN) {
                            ERROR_OUT(CLIENT_ID_ERROR, exit_dcke);
                        }

                        if ((args->idx - args->begin) + clientSz > size) {
                            ERROR_OUT(BUFFER_ERROR, exit_dcke);
                        }

                        XMEMCPY(ssl->arrays->client_identity, input + args->idx,
                                                                    clientSz);
                        args->idx += clientSz;
                        ssl->arrays->client_identity[clientSz] = '\0'; /* null term */

                        /* Read in the DHE business */
                        if ((args->idx - args->begin) + OPAQUE16_LEN > size) {
                            ERROR_OUT(BUFFER_ERROR, exit_dcke);
                        }

                        ato16(input + args->idx, &clientSz);
                        args->idx += OPAQUE16_LEN;

                        if ((args->idx - args->begin) + clientSz > size) {
                            ERROR_OUT(BUFFER_ERROR, exit_dcke);
                        }

                        args->sigSz = clientSz;

                        ret = AllocKey(ssl, DYNAMIC_TYPE_DH,
                                            (void**)&ssl->buffers.serverDH_Key);
                        if (ret != 0) {
                            goto exit_dcke;
                        }

                        ret = wc_DhSetKey(ssl->buffers.serverDH_Key,
                            ssl->buffers.serverDH_P.buffer,
                            ssl->buffers.serverDH_P.length,
                            ssl->buffers.serverDH_G.buffer,
                            ssl->buffers.serverDH_G.length);

                        break;
                    }
                #endif /* !NO_DH && !NO_PSK */
                #if (defined(HAVE_ECC) || defined(HAVE_CURVE25519)) && !defined(NO_PSK)
                    case ecdhe_psk_kea:
                    {
                        word16 clientSz;

                        /* Read in the PSK hint */
                        if ((args->idx - args->begin) + OPAQUE16_LEN > size) {
                            ERROR_OUT(BUFFER_ERROR, exit_dcke);
                        }

                        ato16(input + args->idx, &clientSz);
                        args->idx += OPAQUE16_LEN;
                        if (clientSz > MAX_PSK_ID_LEN) {
                            ERROR_OUT(CLIENT_ID_ERROR, exit_dcke);
                        }
                        if ((args->idx - args->begin) + clientSz > size) {
                            ERROR_OUT(BUFFER_ERROR, exit_dcke);
                        }

                        XMEMCPY(ssl->arrays->client_identity,
                                                   input + args->idx, clientSz);
                        args->idx += clientSz;
                        ssl->arrays->client_identity[clientSz] = '\0'; /* null term */

                        /* import peer ECC key */
                        if ((args->idx - args->begin) + OPAQUE8_LEN > size) {
                            ERROR_OUT(BUFFER_ERROR, exit_dcke);
                        }

                        args->length = input[args->idx++];

                        if ((args->idx - args->begin) + args->length > size) {
                            ERROR_OUT(BUFFER_ERROR, exit_dcke);
                        }

                        args->sigSz = ENCRYPT_LEN - OPAQUE16_LEN;

                    #ifdef HAVE_CURVE25519
                        if (ssl->ecdhCurveOID == ECC_X25519_OID) {
                        #ifdef HAVE_PK_CALLBACKS
                            /* if callback then use it for shared secret */
                            if (ssl->ctx->X25519SharedSecretCb != NULL) {
                                break;
                            }
                        #endif

                            if (ssl->eccTempKeyPresent == 0) {
                                WOLFSSL_MSG(
                                     "X25519 ephemeral key not made correctly");
                                ERROR_OUT(ECC_MAKEKEY_ERROR, exit_dcke);
                            }

                            if (ssl->peerX25519Key == NULL) {
                                /* alloc/init on demand */
                                ret = AllocKey(ssl, DYNAMIC_TYPE_CURVE25519,
                                    (void**)&ssl->peerX25519Key);
                                if (ret != 0) {
                                    goto exit_dcke;
                                }
                            } else if (ssl->peerX25519KeyPresent) {
                                ret = ReuseKey(ssl, DYNAMIC_TYPE_CURVE25519,
                                               ssl->peerX25519Key);
                                ssl->peerX25519KeyPresent = 0;
                                if (ret != 0) {
                                    goto exit_dcke;
                                }
                            }

                            if (wc_curve25519_import_public_ex(
                                    input + args->idx, args->length,
                                    ssl->peerX25519Key,
                                    EC25519_LITTLE_ENDIAN)) {
                                ERROR_OUT(ECC_PEERKEY_ERROR, exit_dcke);
                            }

                            ssl->peerX25519KeyPresent = 1;

                            break;
                        }
                    #endif
                    #ifdef HAVE_PK_CALLBACKS
                        /* if callback then use it for shared secret */
                        if (ssl->ctx->EccSharedSecretCb != NULL) {
                            break;
                        }
                    #endif

                        if (ssl->eccTempKeyPresent == 0) {
                            WOLFSSL_MSG("Ecc ephemeral key not made correctly");
                            ERROR_OUT(ECC_MAKEKEY_ERROR, exit_dcke);
                        }

                        if (ssl->peerEccKey == NULL) {
                            /* alloc/init on demand */
                            ret = AllocKey(ssl, DYNAMIC_TYPE_ECC,
                                (void**)&ssl->peerEccKey);
                            if (ret != 0) {
                                goto exit_dcke;
                            }
                        }
                        else if (ssl->peerEccKeyPresent) {
                            ret = ReuseKey(ssl, DYNAMIC_TYPE_ECC,
                                           ssl->peerEccKey);
                            ssl->peerEccKeyPresent = 0;
                            if (ret != 0) {
                                goto exit_dcke;
                            }
                        }
                        if (wc_ecc_import_x963_ex(input + args->idx, args->length,
                                 ssl->peerEccKey, ssl->eccTempKey->dp->id)) {
                            ERROR_OUT(ECC_PEERKEY_ERROR, exit_dcke);
                        }

                        ssl->peerEccKeyPresent = 1;
                        break;
                    }
                #endif /* (HAVE_ECC || HAVE_CURVE25519) && !NO_PSK */
                    default:
                        ret = BAD_KEA_TYPE_E;
                } /* switch (ssl->specs.kea) */

                /* Check for error */
                if (ret != 0) {
                    goto exit_dcke;
                }

                /* Advance state and proceed */
                ssl->options.asyncState = TLS_ASYNC_DO;
            } /* TLS_ASYNC_BUILD */
            FALL_THROUGH;

            case TLS_ASYNC_DO:
            {
                switch (ssl->specs.kea) {
                #ifndef NO_RSA
                    case rsa_kea:
                    {
                        RsaKey* key = (RsaKey*)ssl->hsKey;
                        ret = RsaDec(ssl,
                            input + args->idx,
                            args->length,
                            &args->output,
                            &args->sigSz,
                            key,
                        #if defined(HAVE_PK_CALLBACKS)
                            ssl->buffers.key->buffer,
                            ssl->buffers.key->length,
                            ssl->RsaDecCtx
                        #else
                            NULL, 0, NULL
                        #endif
                        );
                        break;
                    } /* rsa_kea */
                #endif /* !NO_RSA */
                #ifndef NO_PSK
                    case psk_kea:
                    {
                        break;
                    }
                #endif /* !NO_PSK */
                #ifdef HAVE_NTRU
                    case ntru_kea:
                    {
                        break;
                    }
                #endif /* HAVE_NTRU */
                #if defined(HAVE_ECC) || defined(HAVE_CURVE25519)
                    case ecc_diffie_hellman_kea:
                    {
                        void* private_key = ssl->eccTempKey;

                    #ifdef HAVE_CURVE25519
                        if (ssl->ecdhCurveOID == ECC_X25519_OID) {
                            ret = X25519SharedSecret(ssl,
                                (curve25519_key*)private_key,
                                ssl->peerX25519Key,
                                input + args->idx, &args->length,
                                ssl->arrays->preMasterSecret,
                                &ssl->arrays->preMasterSz,
                                WOLFSSL_SERVER_END,
                            #ifdef HAVE_PK_CALLBACKS
                                ssl->EccSharedSecretCtx
                            #else
                                NULL
                            #endif
                            );
                            break;
                        }
                    #endif
                    #ifdef HAVE_ECC
                        if (ssl->specs.static_ecdh) {
                            private_key = ssl->hsKey;
                        }

                        /* Generate shared secret */
                        ret = EccSharedSecret(ssl,
                            (ecc_key*)private_key, ssl->peerEccKey,
                            input + args->idx, &args->length,
                            ssl->arrays->preMasterSecret,
                            &ssl->arrays->preMasterSz,
                            WOLFSSL_SERVER_END,
                        #ifdef HAVE_PK_CALLBACKS
                            ssl->EccSharedSecretCtx
                        #else
                            NULL
                        #endif
                        );
                    #endif
                        break;
                    }
                #endif /* HAVE_ECC || HAVE_CURVE25519 */
                #ifndef NO_DH
                    case diffie_hellman_kea:
                    {
                        ret = DhAgree(ssl, ssl->buffers.serverDH_Key,
                            ssl->buffers.serverDH_Priv.buffer,
                            ssl->buffers.serverDH_Priv.length,
                            input + args->idx,
                            (word16)args->sigSz,
                            ssl->arrays->preMasterSecret,
                            &ssl->arrays->preMasterSz);
                        break;
                    }
                #endif /* !NO_DH */
                #if !defined(NO_DH) && !defined(NO_PSK)
                    case dhe_psk_kea:
                    {
                        ret = DhAgree(ssl, ssl->buffers.serverDH_Key,
                            ssl->buffers.serverDH_Priv.buffer,
                            ssl->buffers.serverDH_Priv.length,
                            input + args->idx,
                            (word16)args->sigSz,
                            ssl->arrays->preMasterSecret + OPAQUE16_LEN,
                            &ssl->arrays->preMasterSz);
                        break;
                    }
                #endif /* !NO_DH && !NO_PSK */
                #if (defined(HAVE_ECC) || defined(HAVE_CURVE25519)) && !defined(NO_PSK)
                    case ecdhe_psk_kea:
                    {
                    #ifdef HAVE_CURVE25519
                        if (ssl->ecdhCurveOID == ECC_X25519_OID) {
                            ret = X25519SharedSecret(ssl,
                                (curve25519_key*)ssl->eccTempKey,
                                ssl->peerX25519Key,
                                input + args->idx, &args->length,
                                ssl->arrays->preMasterSecret + OPAQUE16_LEN,
                                &args->sigSz,
                                WOLFSSL_SERVER_END,
                            #ifdef HAVE_PK_CALLBACKS
                                ssl->EccSharedSecretCtx
                            #else
                                NULL
                            #endif
                            );
                            break;
                        }
                    #endif
                        /* Generate shared secret */
                        ret = EccSharedSecret(ssl,
                            ssl->eccTempKey, ssl->peerEccKey,
                            input + args->idx, &args->length,
                            ssl->arrays->preMasterSecret + OPAQUE16_LEN,
                            &args->sigSz,
                            WOLFSSL_SERVER_END,
                        #ifdef HAVE_PK_CALLBACKS
                            ssl->EccSharedSecretCtx
                        #else
                            NULL
                        #endif
                        );
                        break;
                    }
                #endif /* (HAVE_ECC || HAVE_CURVE25519) && !NO_PSK */
                    default:
                        ret = BAD_KEA_TYPE_E;
                } /* switch (ssl->specs.kea) */

                /* Check for error */
                if (ret != 0) {
                    goto exit_dcke;
                }

                /* Advance state and proceed */
                ssl->options.asyncState = TLS_ASYNC_VERIFY;
            } /* TLS_ASYNC_DO */
            FALL_THROUGH;

            case TLS_ASYNC_VERIFY:
            {
                switch (ssl->specs.kea) {
                #ifndef NO_RSA
                    case rsa_kea:
                    {
                        /* Add the signature length to idx */
                        args->idx += args->length;

                        if (args->sigSz == SECRET_LEN && args->output != NULL) {
                            XMEMCPY(ssl->arrays->preMasterSecret, args->output, SECRET_LEN);
                            if (ssl->arrays->preMasterSecret[0] != ssl->chVersion.major ||
                                ssl->arrays->preMasterSecret[1] != ssl->chVersion.minor) {
                                ERROR_OUT(PMS_VERSION_ERROR, exit_dcke);
                            }
                        }
                        else {
                            ERROR_OUT(RSA_PRIVATE_ERROR, exit_dcke);
                        }
                        break;
                    } /* rsa_kea */
                #endif /* !NO_RSA */
                #ifndef NO_PSK
                    case psk_kea:
                    {
                        break;
                    }
                #endif /* !NO_PSK */
                #ifdef HAVE_NTRU
                    case ntru_kea:
                    {
                        break;
                    }
                #endif /* HAVE_NTRU */
                #ifdef HAVE_ECC
                    case ecc_diffie_hellman_kea:
                    {
                        /* skip past the imported peer key */
                        args->idx += args->length;
                        break;
                    }
                #endif /* HAVE_ECC */
                #ifndef NO_DH
                    case diffie_hellman_kea:
                    {
                        args->idx += (word16)args->sigSz;
                        break;
                    }
                #endif /* !NO_DH */
                #if !defined(NO_DH) && !defined(NO_PSK)
                    case dhe_psk_kea:
                    {
                        byte* pms = ssl->arrays->preMasterSecret;
                        word16 clientSz = (word16)args->sigSz;

                        args->idx += clientSz;
                        c16toa((word16)ssl->arrays->preMasterSz, pms);
                        ssl->arrays->preMasterSz += OPAQUE16_LEN;
                        pms += ssl->arrays->preMasterSz;

                        /* Use the PSK hint to look up the PSK and add it to the
                         * preMasterSecret here. */
                        ssl->arrays->psk_keySz = ssl->options.server_psk_cb(ssl,
                            ssl->arrays->client_identity, ssl->arrays->psk_key,
                            MAX_PSK_KEY_LEN);

                        if (ssl->arrays->psk_keySz == 0 ||
                                ssl->arrays->psk_keySz > MAX_PSK_KEY_LEN) {
                            ERROR_OUT(PSK_KEY_ERROR, exit_dcke);
                        }

                        c16toa((word16) ssl->arrays->psk_keySz, pms);
                        pms += OPAQUE16_LEN;

                        XMEMCPY(pms, ssl->arrays->psk_key,
                                                    ssl->arrays->psk_keySz);
                        ssl->arrays->preMasterSz += ssl->arrays->psk_keySz +
                                                                OPAQUE16_LEN;
                        break;
                    }
                #endif /* !NO_DH && !NO_PSK */
                #if defined(HAVE_ECC) && !defined(NO_PSK)
                    case ecdhe_psk_kea:
                    {
                        byte* pms = ssl->arrays->preMasterSecret;
                        word16 clientSz = (word16)args->sigSz;

                        /* skip past the imported peer key */
                        args->idx += args->length;

                        /* Add preMasterSecret */
                        c16toa(clientSz, pms);
                        ssl->arrays->preMasterSz += OPAQUE16_LEN + clientSz;
                        pms += ssl->arrays->preMasterSz;

                        /* Use the PSK hint to look up the PSK and add it to the
                         * preMasterSecret here. */
                        ssl->arrays->psk_keySz = ssl->options.server_psk_cb(ssl,
                            ssl->arrays->client_identity, ssl->arrays->psk_key,
                            MAX_PSK_KEY_LEN);

                        if (ssl->arrays->psk_keySz == 0 ||
                                   ssl->arrays->psk_keySz > MAX_PSK_KEY_LEN) {
                            ERROR_OUT(PSK_KEY_ERROR, exit_dcke);
                        }

                        c16toa((word16) ssl->arrays->psk_keySz, pms);
                        pms += OPAQUE16_LEN;

                        XMEMCPY(pms, ssl->arrays->psk_key, ssl->arrays->psk_keySz);
                        ssl->arrays->preMasterSz +=
                                      ssl->arrays->psk_keySz + OPAQUE16_LEN;
                        break;
                    }
                #endif /* HAVE_ECC && !NO_PSK */
                    default:
                        ret = BAD_KEA_TYPE_E;
                } /* switch (ssl->specs.kea) */

                /* Check for error */
                if (ret != 0) {
                    goto exit_dcke;
                }

                /* Advance state and proceed */
                ssl->options.asyncState = TLS_ASYNC_FINALIZE;
            } /* TLS_ASYNC_VERIFY */
            FALL_THROUGH;

            case TLS_ASYNC_FINALIZE:
            {
            #ifdef HAVE_QSH
                word16 name;

                if (ssl->options.haveQSH) {
                    /* extension name */
                    ato16(input + args->idx, &name);
                    args->idx += OPAQUE16_LEN;

                    if (name == TLSX_QUANTUM_SAFE_HYBRID) {
                        int    qshSz;
                        /* if qshSz is larger than 0 it is the
                           length of buffer used */
                        if ((qshSz = TLSX_QSHCipher_Parse(ssl,
                                input + args->idx,
                                size - args->idx + args->begin, 1)) < 0) {
                            ERROR_OUT(qshSz, exit_dcke);
                        }
                        args->idx += qshSz;
                    }
                    else {
                        /* unknown extension sent client ignored handshake */
                        ERROR_OUT(BUFFER_ERROR, exit_dcke);
                    }
                }
            #endif /* HAVE_QSH */
                ret = MakeMasterSecret(ssl);

                /* Check for error */
                if (ret != 0) {
                    goto exit_dcke;
                }

                /* Advance state and proceed */
                ssl->options.asyncState = TLS_ASYNC_END;
            } /* TLS_ASYNC_FINALIZE */
            FALL_THROUGH;

            case TLS_ASYNC_END:
            {
                /* Set final index */
                *inOutIdx = args->idx;

                ssl->options.clientState = CLIENT_KEYEXCHANGE_COMPLETE;
            #ifndef NO_CERTS
                if (ssl->options.verifyPeer) {
                    ret = BuildCertHashes(ssl, &ssl->hsHashes->certHashes);
                }
            #endif
                break;
            } /* TLS_ASYNC_END */
            default:
                ret = INPUT_CASE_ERROR;
        } /* switch(ssl->options.asyncState) */

    exit_dcke:

        WOLFSSL_LEAVE("DoClientKeyExchange", ret);

    #ifdef WOLFSSL_ASYNC_CRYPT
        /* Handle async operation */
        if (ret == WC_PENDING_E) {
            /* Mark message as not recevied so it can process again */
            ssl->msgsReceived.got_client_key_exchange = 0;

            return ret;
        }
    #endif /* WOLFSSL_ASYNC_CRYPT */

        /* Cleanup PMS */
        ForceZero(ssl->arrays->preMasterSecret, ssl->arrays->preMasterSz);
        ssl->arrays->preMasterSz = 0;

        /* Final cleanup */
        FreeDckeArgs(ssl, args);
        FreeKeyExchange(ssl);

        return ret;
    }


#if defined(HAVE_STUNNEL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
    int SNI_Callback(WOLFSSL* ssl)
    {
        /* Stunnel supports a custom sni callback to switch an SSL's ctx
        * when SNI is received. Call it now if exists */
        if(ssl && ssl->ctx && ssl->ctx->sniRecvCb) {
            WOLFSSL_MSG("Calling custom sni callback");
            if(ssl->ctx->sniRecvCb(ssl, NULL, ssl->ctx->sniRecvCbArg)
                    == alert_fatal) {
                WOLFSSL_MSG("Error in custom sni callback. Fatal alert");
                SendAlert(ssl, alert_fatal, unrecognized_name);
                return FATAL_ERROR;
            }
        }
        return 0;
    }
#endif /* HAVE_STUNNEL || WOLGSSL_NGINX */
#endif /* NO_WOLFSSL_SERVER */


#ifdef WOLFSSL_ASYNC_CRYPT
int wolfSSL_AsyncPop(WOLFSSL* ssl, byte* state)
{
    int ret = 0;
    WC_ASYNC_DEV* asyncDev;
    WOLF_EVENT* event;

    if (ssl == NULL) {
        return BAD_FUNC_ARG;
    }

    /* check for pending async */
    asyncDev = ssl->async.dev;
    if (asyncDev) {
        /* grab event pointer */
        event = &asyncDev->event;

        ret = wolfAsync_EventPop(event, WOLF_EVENT_TYPE_ASYNC_WOLFSSL);
        if (ret != WC_NOT_PENDING_E && ret != WC_PENDING_E) {

            /* advance key share state if doesn't need called again */
            if (state && (asyncDev->event.flags & WC_ASYNC_FLAG_CALL_AGAIN) == 0) {
                (*state)++;
            }

            /* clear event */
            XMEMSET(&asyncDev->event, 0, sizeof(WOLF_EVENT));

            /* clear async dev */
            ssl->async.dev = NULL;
        }
    }
    else {
        ret = WC_NOT_PENDING_E;
    }

    WOLFSSL_LEAVE("wolfSSL_AsyncPop", ret);

    return ret;
}

int wolfSSL_AsyncInit(WOLFSSL* ssl, WC_ASYNC_DEV* asyncDev, word32 flags)
{
    int ret;
    WOLF_EVENT* event;

    if (ssl == NULL || asyncDev == NULL) {
        return BAD_FUNC_ARG;
    }

    /* grab event pointer */
    event = &asyncDev->event;

    /* init event */
    ret = wolfAsync_EventInit(event, WOLF_EVENT_TYPE_ASYNC_WOLFSSL, ssl, flags);

    WOLFSSL_LEAVE("wolfSSL_AsyncInit", ret);

    return ret;
}

int wolfSSL_AsyncPush(WOLFSSL* ssl, WC_ASYNC_DEV* asyncDev)
{
    int ret;
    WOLF_EVENT* event;

    if (ssl == NULL || asyncDev == NULL) {
        return BAD_FUNC_ARG;
    }

    /* grab event pointer */
    event = &asyncDev->event;

    /* store reference to active async operation */
    ssl->async.dev = asyncDev;

    /* place event into queue */
    ret = wolfAsync_EventQueuePush(&ssl->ctx->event_queue, event);

    /* success means return WC_PENDING_E */
    if (ret == 0) {
        ret = WC_PENDING_E;
    }

    WOLFSSL_LEAVE("wolfSSL_AsyncPush", ret);

    return ret;
}

#endif /* WOLFSSL_ASYNC_CRYPT */


#undef ERROR_OUT

#endif /* WOLFCRYPT_ONLY */
