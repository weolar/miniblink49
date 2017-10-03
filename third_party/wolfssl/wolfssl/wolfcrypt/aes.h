/* aes.h
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

#ifndef HAVE_FIPS /* to avoid redefinition of structures */

#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif

enum {
    AES_ENC_TYPE   = 1,   /* cipher unique type */
    AES_ENCRYPTION = 0,
    AES_DECRYPTION = 1,
    KEYWRAP_BLOCK_SIZE = 8,
    AES_BLOCK_SIZE = 16
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
#ifdef WOLFSSL_AES_COUNTER
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
/*!
    \ingroup AES

    \brief This is to help with setting keys to correct encrypt or decrypt type.

    \note Is up to user to call wc_AesXtsFree on aes key when done.

    \return 0 Success

    \param aes   AES keys for encrypt/decrypt process
    \param key   buffer holding aes key | tweak key
    \param len   length of key buffer in bytes. Should be twice that of key size.
                 i.e. 32 for a 16 byte key.
    \param dir   direction, either AES_ENCRYPTION or AES_DECRYPTION
    \param heap  heap hint to use for memory. Can be NULL
    \param devId id to use with async crypto. Can be 0

    _Example_
    \code
    XtsAes aes;

    if(wc_AesXtsSetKey(&aes, key, sizeof(key), AES_ENCRYPTION, NULL, 0) != 0)
    {
        // Handle error
    }
    wc_AesXtsFree(&aes);
    \endcode

    \sa wc_AesXtsEncrypt
    \sa wc_AesXtsDecrypt
    \sa wc_AesXtsFree
*/
WOLFSSL_API int wc_AesXtsSetKey(XtsAes* aes, const byte* key,
         word32 len, int dir, void* heap, int devId);


/*!
    \ingroup AES

    \brief Same process as wc_AesXtsEncrypt but uses a word64 type as the tweak
           value instead of a byte array. This just converts the word64 to a
           byte array and calls wc_AesXtsEncrypt.

    \return 0 Success

    \param aes    AES keys to use for block encrypt/decrypt
    \param out    output buffer to hold cipher text
    \param in     input plain text buffer to encrypt
    \param sz     size of both out and in buffers
    \param sector value to use for tweak

    _Example_
    \code
    XtsAes aes;
    unsigned char plain[SIZE];
    unsigned char cipher[SIZE];
    word64 s = VALUE;

    //set up keys with AES_ENCRYPTION as dir

    if(wc_AesXtsEncryptSector(&aes, cipher, plain, SIZE, s) != 0)
    {
        // Handle error
    }
    wc_AesXtsFree(&aes);
    \endcode

    \sa wc_AesXtsEncrypt
    \sa wc_AesXtsDecrypt
    \sa wc_AesXtsSetKey
    \sa wc_AesXtsFree
*/
WOLFSSL_API int wc_AesXtsEncryptSector(XtsAes* aes, byte* out,
         const byte* in, word32 sz, word64 sector);


/*!
    \ingroup AES

    \brief Same process as wc_AesXtsDecrypt but uses a word64 type as the tweak
           value instead of a byte array. This just converts the word64 to a
           byte array.

    \return 0 Success

    \param aes    AES keys to use for block encrypt/decrypt
    \param out    output buffer to hold plain text
    \param in     input cipher text buffer to decrypt
    \param sz     size of both out and in buffers
    \param sector value to use for tweak

    _Example_
    \code
    XtsAes aes;
    unsigned char plain[SIZE];
    unsigned char cipher[SIZE];
    word64 s = VALUE;

    //set up aes key with AES_DECRYPTION as dir and tweak with AES_ENCRYPTION

    if(wc_AesXtsDecryptSector(&aes, plain, cipher, SIZE, s) != 0)
    {
        // Handle error
    }
    wc_AesXtsFree(&aes);
    \endcode

    \sa wc_AesXtsEncrypt
    \sa wc_AesXtsDecrypt
    \sa wc_AesXtsSetKey
    \sa wc_AesXtsFree
*/
WOLFSSL_API int wc_AesXtsDecryptSector(XtsAes* aes, byte* out,
         const byte* in, word32 sz, word64 sector);


/*!
    \ingroup AES

    \brief AES with XTS mode. (XTS) XEX encryption with Tweak and cipher text
           Stealing.

    \return 0 Success

    \param aes   AES keys to use for block encrypt/decrypt
    \param out   output buffer to hold cipher text
    \param in    input plain text buffer to encrypt
    \param sz    size of both out and in buffers
    \param i     value to use for tweak
    \param iSz   size of i buffer, should always be AES_BLOCK_SIZE but having
                 this input adds a sanity check on how the user calls the
                 function.

    _Example_
    \code
    XtsAes aes;
    unsigned char plain[SIZE];
    unsigned char cipher[SIZE];
    unsigned char i[AES_BLOCK_SIZE];

    //set up key with AES_ENCRYPTION as dir

    if(wc_AesXtsEncrypt(&aes, cipher, plain, SIZE, i, sizeof(i)) != 0)
    {
        // Handle error
    }
    wc_AesXtsFree(&aes);
    \endcode

    \sa wc_AesXtsDecrypt
    \sa wc_AesXtsSetKey
    \sa wc_AesXtsFree
*/
WOLFSSL_API int wc_AesXtsEncrypt(XtsAes* aes, byte* out,
         const byte* in, word32 sz, const byte* i, word32 iSz);


/*!
    \ingroup AES

    \brief Same process as encryption but Aes key is AES_DECRYPTION type.

    \return 0 Success

    \param aes   AES keys to use for block encrypt/decrypt
    \param out   output buffer to hold plain text
    \param in    input cipher text buffer to decrypt
    \param sz    size of both out and in buffers
    \param i     value to use for tweak
    \param iSz   size of i buffer, should always be AES_BLOCK_SIZE but having
                 this input adds a sanity check on how the user calls the
                 function.
    _Example_
    \code
    XtsAes aes;
    unsigned char plain[SIZE];
    unsigned char cipher[SIZE];
    unsigned char i[AES_BLOCK_SIZE];

    //set up key with AES_DECRYPTION as dir and tweak with AES_ENCRYPTION

    if(wc_AesXtsDecrypt(&aes, plain, cipher, SIZE, i, sizeof(i)) != 0)
    {
        // Handle error
    }
    wc_AesXtsFree(&aes);
    \endcode

    \sa wc_AesXtsEncrypt
    \sa wc_AesXtsSetKey
    \sa wc_AesXtsFree
*/
WOLFSSL_API int wc_AesXtsDecrypt(XtsAes* aes, byte* out,
        const byte* in, word32 sz, const byte* i, word32 iSz);


/*!
    \ingroup AES

    \brief This is to free up any resources used by the XtsAes structure

    \return 0 Success

    \param aes AES keys to free

    _Example_
    \code
    XtsAes aes;

    if(wc_AesXtsSetKey(&aes, key, sizeof(key), AES_ENCRYPTION, NULL, 0) != 0)
    {
        // Handle error
    }
    wc_AesXtsFree(&aes);
    \endcode

    \sa wc_AesXtsEncrypt
    \sa wc_AesXtsDecrypt
    \sa wc_AesXtsSetKey
*/
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
