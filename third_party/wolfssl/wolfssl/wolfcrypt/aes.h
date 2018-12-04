/* aes.h
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

/*!
    \file wolfssl/wolfcrypt/aes.h
*/


#ifndef WOLF_CRYPT_AES_H
#define WOLF_CRYPT_AES_H

#include <wolfssl/wolfcrypt/types.h>

#ifndef NO_AES

/* included for fips @wc_fips */
#ifdef HAVE_FIPS
#include <cyassl/ctaocrypt/aes.h>
#if defined(CYASSL_AES_COUNTER) && !defined(WOLFSSL_AES_COUNTER)
    #define WOLFSSL_AES_COUNTER
#endif
#if !defined(WOLFSSL_AES_DIRECT) && defined(CYASSL_AES_DIRECT)
    #define WOLFSSL_AES_DIRECT
#endif
#endif

#ifndef HAVE_FIPS /* to avoid redefinition of macros */

#ifdef WOLFSSL_AESNI

#include <wmmintrin.h>
#include <emmintrin.h>
#include <smmintrin.h>

#endif /* WOLFSSL_AESNI */

#ifdef WOLFSSL_XILINX_CRYPT
#include "xsecure_aes.h"
#endif

#endif /* HAVE_FIPS */

#ifdef __cplusplus
    extern "C" {
#endif

/* these are required for FIPS and non-FIPS */
enum {    
    AES_128_KEY_SIZE    = 16,  /* for 128 bit             */
    AES_192_KEY_SIZE    = 24,  /* for 192 bit             */
    AES_256_KEY_SIZE    = 32,  /* for 256 bit             */

    AES_IV_SIZE         = 16,  /* always block size       */
};


#ifndef HAVE_FIPS /* to avoid redefinition of structures */

#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif

enum {
    AES_ENC_TYPE   = WC_CIPHER_AES,   /* cipher unique type */
    AES_ENCRYPTION = 0,
    AES_DECRYPTION = 1,

    AES_BLOCK_SIZE      = 16,

    KEYWRAP_BLOCK_SIZE  = 8,
};


typedef struct Aes {
    /* AESNI needs key first, rounds 2nd, not sure why yet */
    ALIGN16 word32 key[60];
    word32  rounds;
    int     keylen;

    ALIGN16 word32 reg[AES_BLOCK_SIZE / sizeof(word32)];      /* for CBC mode */
    ALIGN16 word32 tmp[AES_BLOCK_SIZE / sizeof(word32)];      /* same         */

#ifdef HAVE_AESGCM
    ALIGN16 byte H[AES_BLOCK_SIZE];
#ifdef GCM_TABLE
    /* key-based fast multiplication table. */
    ALIGN16 byte M0[256][AES_BLOCK_SIZE];
#endif /* GCM_TABLE */
#endif /* HAVE_AESGCM */
#ifdef WOLFSSL_AESNI
    byte use_aesni;
#endif /* WOLFSSL_AESNI */
#ifdef WOLFSSL_ASYNC_CRYPT
    word32 asyncKey[AES_MAX_KEY_SIZE/8/sizeof(word32)]; /* raw key */
    word32 asyncIv[AES_BLOCK_SIZE/sizeof(word32)]; /* raw IV */
    WC_ASYNC_DEV asyncDev;
#endif /* WOLFSSL_ASYNC_CRYPT */
#if defined(WOLFSSL_AES_COUNTER) || defined(WOLFSSL_AES_CFB)
    word32  left;            /* unused bytes left from last call */
#endif
#ifdef WOLFSSL_XILINX_CRYPT
    XSecure_Aes xilAes;
    XCsuDma     dma;
    word32      key_init[8];
    word32      kup;
#endif
    void*  heap; /* memory hint to use */
} Aes;

#ifdef WOLFSSL_AES_XTS
typedef struct XtsAes {
    Aes aes;
    Aes tweak;
} XtsAes;
#endif

#ifdef HAVE_AESGCM
typedef struct Gmac {
    Aes aes;
} Gmac;
#endif /* HAVE_AESGCM */
#endif /* HAVE_FIPS */


/* Authenticate cipher function prototypes */
typedef int (*wc_AesAuthEncryptFunc)(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);
typedef int (*wc_AesAuthDecryptFunc)(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   const byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);

/* AES-CBC */
WOLFSSL_API int  wc_AesSetKey(Aes* aes, const byte* key, word32 len,
                              const byte* iv, int dir);
WOLFSSL_API int  wc_AesSetIV(Aes* aes, const byte* iv);
WOLFSSL_API int  wc_AesCbcEncrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz);
WOLFSSL_API int  wc_AesCbcDecrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz);

#ifdef WOLFSSL_AES_CFB
WOLFSSL_API int wc_AesCfbEncrypt(Aes* aes, byte* out,
                                    const byte* in, word32 sz);
#ifdef HAVE_AES_DECRYPT
WOLFSSL_API int wc_AesCfbDecrypt(Aes* aes, byte* out,
                                    const byte* in, word32 sz);
#endif /* HAVE_AES_DECRYPT */
#endif /* WOLFSSL_AES_CFB */

#ifdef HAVE_AES_ECB
WOLFSSL_API int wc_AesEcbEncrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz);
WOLFSSL_API int wc_AesEcbDecrypt(Aes* aes, byte* out,
                                  const byte* in, word32 sz);
#endif

/* AES-CTR */
#ifdef WOLFSSL_AES_COUNTER
 WOLFSSL_API int wc_AesCtrEncrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz);
#endif
/* AES-DIRECT */
#if defined(WOLFSSL_AES_DIRECT)
 WOLFSSL_API void wc_AesEncryptDirect(Aes* aes, byte* out, const byte* in);
 WOLFSSL_API void wc_AesDecryptDirect(Aes* aes, byte* out, const byte* in);
 WOLFSSL_API int  wc_AesSetKeyDirect(Aes* aes, const byte* key, word32 len,
                                const byte* iv, int dir);
#endif
#ifdef HAVE_AESGCM
#ifdef WOLFSSL_XILINX_CRYPT
 WOLFSSL_API int  wc_AesGcmSetKey_ex(Aes* aes, const byte* key, word32 len,
         word32 kup);
#endif
 WOLFSSL_API int  wc_AesGcmSetKey(Aes* aes, const byte* key, word32 len);
 WOLFSSL_API int  wc_AesGcmEncrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);
 WOLFSSL_API int  wc_AesGcmDecrypt(Aes* aes, byte* out,
                                   const byte* in, word32 sz,
                                   const byte* iv, word32 ivSz,
                                   const byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);

 WOLFSSL_API int wc_GmacSetKey(Gmac* gmac, const byte* key, word32 len);
 WOLFSSL_API int wc_GmacUpdate(Gmac* gmac, const byte* iv, word32 ivSz,
                               const byte* authIn, word32 authInSz,
                               byte* authTag, word32 authTagSz);
 WOLFSSL_LOCAL void GHASH(Aes* aes, const byte* a, word32 aSz, const byte* c,
                               word32 cSz, byte* s, word32 sSz);
#endif /* HAVE_AESGCM */
#ifdef HAVE_AESCCM
 WOLFSSL_API int  wc_AesCcmSetKey(Aes* aes, const byte* key, word32 keySz);
 WOLFSSL_API int  wc_AesCcmEncrypt(Aes* aes, byte* out,
                                   const byte* in, word32 inSz,
                                   const byte* nonce, word32 nonceSz,
                                   byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);
 WOLFSSL_API int  wc_AesCcmDecrypt(Aes* aes, byte* out,
                                   const byte* in, word32 inSz,
                                   const byte* nonce, word32 nonceSz,
                                   const byte* authTag, word32 authTagSz,
                                   const byte* authIn, word32 authInSz);
#endif /* HAVE_AESCCM */
#ifdef HAVE_AES_KEYWRAP
 WOLFSSL_API int  wc_AesKeyWrap(const byte* key, word32 keySz,
                                const byte* in, word32 inSz,
                                byte* out, word32 outSz,
                                const byte* iv);
 WOLFSSL_API int  wc_AesKeyUnWrap(const byte* key, word32 keySz,
                                const byte* in, word32 inSz,
                                byte* out, word32 outSz,
                                const byte* iv);
#endif /* HAVE_AES_KEYWRAP */

#ifdef WOLFSSL_AES_XTS

WOLFSSL_API int wc_AesXtsSetKey(XtsAes* aes, const byte* key,
         word32 len, int dir, void* heap, int devId);

WOLFSSL_API int wc_AesXtsEncryptSector(XtsAes* aes, byte* out,
         const byte* in, word32 sz, word64 sector);

WOLFSSL_API int wc_AesXtsDecryptSector(XtsAes* aes, byte* out,
         const byte* in, word32 sz, word64 sector);

WOLFSSL_API int wc_AesXtsEncrypt(XtsAes* aes, byte* out,
         const byte* in, word32 sz, const byte* i, word32 iSz);

WOLFSSL_API int wc_AesXtsDecrypt(XtsAes* aes, byte* out,
        const byte* in, word32 sz, const byte* i, word32 iSz);

WOLFSSL_API int wc_AesXtsFree(XtsAes* aes);
#endif

WOLFSSL_API int wc_AesGetKeySize(Aes* aes, word32* keySize);

WOLFSSL_API int  wc_AesInit(Aes*, void*, int);
WOLFSSL_API void wc_AesFree(Aes*);

#ifdef __cplusplus
    } /* extern "C" */
#endif


#endif /* NO_AES */
#endif /* WOLF_CRYPT_AES_H */
